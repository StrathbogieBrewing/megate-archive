#include 	<avr/io.h>

; this bootloader is compatible with avrdude and avrprog.
; it is broadly based on the avr 109 format
; first program the micro with this code in ISP mode
; xtal oscillator at 4 MHz
; baud rate 9600 bps, full duplex or half duplex

#define		BOOTSTART 	0x1F00
#define		ZERO		R4
#define		STATUS	R16
#define		ADDRL		R26
#define		ADDRH		R27
#define		WORDL		R18
#define		WORDH		R19
#define		RXCHAR	R20
#define		TEMP		R21
#define		TEMPL		R24
#define		TEMPH		R25

#define		PART_CODE	0x79
#define		SW_MAJOR  	'2'
#define		SW_MINOR  	'3'

#define  	BUFFERSIZE	(SPM_PAGESIZE)

	.lcomm 	buffer, 	BUFFERSIZE
	.section .text

mainEntry:

; *** test if bootloader requested by external reset ***
  in		TEMP, _SFR_IO_ADDR(MCUSR)

	; clear the external and watchdog reset flag
	wdr
  ldi   TEMPL, ~((1<<EXTRF) | (1<<WDRF))
  out   _SFR_IO_ADDR(MCUSR), TEMPL

	; disable watchdog timer
	ldi   TEMPL, (1<<WDCE) | (1<<WDE)
	sts   _SFR_MEM_ADDR(WDTCSR), TEMPL
	clr   TEMPL
	sts   _SFR_MEM_ADDR(WDTCSR), TEMPL

  ; jump to application code if not external reset
	sbrs	TEMP, EXTRF
	jmp	  0

; place strings in low and known area of flash to simplify indexing to single byte
programmerType:		.byte	'A', 'V', 'R', 'B', 'O', 'O', 'T', 0
signatureBytes:		.byte	SIGNATURE_2, SIGNATURE_1, SIGNATURE_0, 0
softwareVersion:	.byte 	SW_MAJOR, SW_MINOR, 0, 0

; *** application boot entry point ***
startBootloader:
; *** initialise micro ***
	; initialise registers
	clr		ZERO
	; clear status register
	out		_SFR_IO_ADDR(SREG), ZERO
	; set up stack to top of available sram
	ldi		TEMPL, lo8(RAMEND)
	ldi		TEMPH, hi8(RAMEND)
	out		_SFR_IO_ADDR(SPH), TEMPH
	out		_SFR_IO_ADDR(SPL), TEMPL


; *** initialise uart ***
inituart:
	; set baud rate to 9600 bps
	ldi     TEMP, 25
	sts     _SFR_MEM_ADDR(UBRR0L), TEMP
	; clear txc
	ldi     TEMP, (1 << TXC0)
  sts     _SFR_MEM_ADDR(UCSR0A), TEMP
	; enable rx and tx
	ldi     TEMP, ((1 << RXEN0) | (1 << TXEN0))
	sts     _SFR_MEM_ADDR(UCSR0B), TEMP
	rjmp	  mainLoop

; *** send subroutine (value in TEMP) ***
uartPut:
  ; force half duplex to block any echo if connected to a bus
	sts    _SFR_MEM_ADDR(UDR0), TEMP
	ldi     TEMP, ((1 << TXEN0)) ; disable rx
	sts     _SFR_MEM_ADDR(UCSR0B), TEMP
uartTxLoop:
	lds    TEMP, _SFR_MEM_ADDR(UCSR0A)
	sbrs   TEMP, TXC0
	rjmp   uartTxLoop
	ldi     TEMP, (1 << TXC0)
	sts     _SFR_MEM_ADDR(UCSR0A), TEMP
	ldi     TEMP, ((1 << RXEN0) | (1 << TXEN0))  ; re-enable rx
	sts     _SFR_MEM_ADDR(UCSR0B), TEMP
	ret

; *** put string from progmem subroutine (LSB of address in R30) ***
uartPutString:
	ldi		ZH, hi8(BOOTSTART << 1)
	lpm		TEMP, Z+
	or		TEMP, TEMP
	breq	uartPutStringExit
	rcall	uartPut
	rjmp	uartPutString
uartPutStringExit:
	ret

; *** receive subroutine (value returned in RXCHAR) ***
uartGet:
	lds     RXCHAR, _SFR_MEM_ADDR(UCSR0A)
	sbrs    RXCHAR, RXC0
	rjmp	  uartGet
	lds     RXCHAR, _SFR_MEM_ADDR(UDR0)
	ret

; *** send return character and fall back into main loop ***
uartPutReturnAndMainLoop:
	ldi		TEMP, '\r'
uartPutAndMainLoop:
	rcall 	uartPut

; *** main program loop ***
mainLoop:

	; get received character
	rcall	uartGet

blockSupport:
	cpi		RXCHAR, 'b'
	brne	readBlock
	ldi		TEMP, 'Y'
	rcall	uartPut
	ldi		TEMP, 0
	rcall	uartPut
	ldi		TEMP, BUFFERSIZE
  rjmp uartPutAndMainLoop

readBlock:
	cpi		RXCHAR, 'g'
	brne	writeBlock
	rcall	uartGet
	ldi		TEMPH, 0			; limit buffer size to 256 characters
	rcall	uartGet
	mov		TEMPL, RXCHAR
	rcall	uartGet
	cpi		RXCHAR, 'E'
	breq	readEeprom

readFlash:
	movw	ZL, ADDRL
	add		ZL, ZL
	adc		ZH, ZH
readFlashLoop:
	lpm		TEMP, Z+
	rcall	uartPut
	lpm		TEMP, Z+
	rcall	uartPut
	adiw	ADDRL, 1
	sbiw	TEMPL, 2
	brne	readFlashLoop
	rjmp	readDone

readEeprom:
	sbic 	_SFR_IO_ADDR(EECR), EEPE
	rjmp 	readEeprom
	out 	_SFR_IO_ADDR(EEARH), ADDRH
	out 	_SFR_IO_ADDR(EEARL), ADDRL
	sbi 	_SFR_IO_ADDR(EECR), EERE
	in 		TEMP, _SFR_IO_ADDR(EEDR)
	rcall	uartPut
	adiw	ADDRL, 1
	sbiw	TEMPL, 1
	brne	readEeprom

readDone:
	rjmp	mainLoop

writeBlock:
	cpi		RXCHAR, 'B'
	breq	writeGetParams
	rjmp	autoIncrement

writeGetParams:
	rcall	uartGet
	ldi		TEMPH, 0					; limit buffer size to 256 characters
	rcall	uartGet
	mov		TEMPL, RXCHAR
	rcall	uartGet
	mov		STATUS, RXCHAR
	movw	WORDL, TEMPL
	ldi		YL, lo8(buffer)
	ldi		YH, hi8(buffer)

writeGetData:
	rcall	uartGet
	st		Y+, RXCHAR
	sbiw	TEMPL, 1
	brne	writeGetData
	ldi		YL, lo8(buffer)
	ldi		YH, hi8(buffer)
	movw	TEMPL, WORDL
	cpi		STATUS, 'E'
	brne	writeFlash

writeEeprom:
	ld		TEMP, Y+
writeEepromWait:
	sbic 	_SFR_IO_ADDR(EECR), EEPE
	rjmp 	writeEepromWait
	out 	_SFR_IO_ADDR(EEARH), ADDRH
	out 	_SFR_IO_ADDR(EEARL), ADDRL
	out 	_SFR_IO_ADDR(EEDR), TEMP
	sbi 	_SFR_IO_ADDR(EECR), EEMPE
	sbi 	_SFR_IO_ADDR(EECR), EEPE
	adiw	ADDRL, 1
	sbiw	TEMPL, 1
	brne	writeEeprom
	rjmp	uartPutReturnAndMainLoop

writeFlash:
	sbic 	_SFR_IO_ADDR(EECR), EEPE
	rjmp 	writeFlash
	movw	ZL, ADDRL
	add		ZL, ZL
	adc		ZH, ZH
	movw	WORDL, ZL

	ldi 	TEMP, (1<<PGERS) | (1<<SELFPRGEN)		; page erase
	rcall	writeFlashSPM
	ldi 	TEMP, (1<<RWWSRE) | (1<<SELFPRGEN)		; re-enable the RWW section
	rcall	writeFlashSPM

writeFlashLoop:
	ld		R0, Y+
	ld		R1, Y+
	ldi 	TEMP, (1<<SELFPRGEN)					; copy word to flash write buffer
	rcall	writeFlashSPM
	adiw	ZL, 2
	adiw	ADDRL, 1
	sbiw	TEMPL, 2
	brne	writeFlashLoop

	movw	ZL, WORDL
	rcall	writeFlashPage
	rjmp	uartPutReturnAndMainLoop

writeFlashSPM:
	in 		STATUS, _SFR_IO_ADDR(SPMCSR)			; check for previous SPM complete
	sbrc 	STATUS, SELFPRGEN
	rjmp 	writeFlashSPM
	out 	_SFR_IO_ADDR(SPMCSR), TEMP			; execute spm with action given by TEMP
	spm
	ret

writeFlashPage:
	cpi		ZH, hi8(BOOTSTART << 1)
	brsh	writeBootLoaderInhibit
	ldi 	TEMP, (1<<PGWRT) | (1<<SELFPRGEN)		; execute page write
	rcall	writeFlashSPM
	ldi 	TEMP, (1<<RWWSRE) | (1<<SELFPRGEN)	; re-enable the RWW section
	rcall	writeFlashSPM
writeBootLoaderInhibit:
	ret

autoIncrement:
	cpi		RXCHAR, 'a'
	brne	setAddress
autoIncrementSupported:
	ldi		TEMP, 'Y'
  rjmp uartPutAndMainLoop

setAddress:
	cpi		RXCHAR, 'A'
	brne	eraseChip
	rcall	uartGet
	mov		ADDRH, RXCHAR
	rcall	uartGet
	mov		ADDRL, RXCHAR
	rjmp	uartPutReturnAndMainLoop

eraseChip:
	cpi		RXCHAR, 'e'							; dont do anything because we erase page by page
	brne	exitBootloader
	rjmp	uartPutReturnAndMainLoop

exitBootloader:
	cpi		RXCHAR, 'E'
	brne	getProgrammerType
	ldi		TEMP, '\r'
	rcall 	uartPut
  ldi   TEMP, (1 << WDE)  ; force watchdog reset
  sts   _SFR_MEM_ADDR(WDTCSR), TEMP
infiniteLoop:
  rjmp  infiniteLoop

getProgrammerType:
	cpi		RXCHAR, 'p'
	brne	reportSupportedDeviceCodes
	ldi		TEMP, 'S'
  rjmp  uartPutAndMainLoop

reportSupportedDeviceCodes:
	cpi		RXCHAR, 't'
	brne	setLED
	ldi		TEMP, PART_CODE
	rcall	uartPut
	ldi		TEMP, 0x00
  rjmp uartPutAndMainLoop

setLED:
	cpi		RXCHAR, 'x'
	breq	getByteAndPutReturnAndMainLoop

clearLED:
	cpi		RXCHAR, 'y'
	brne	enterProgrammingMode

getByteAndPutReturnAndMainLoop:
	rcall 	uartGet
putReturnAndMainLoop:
	rjmp	uartPutReturnAndMainLoop

enterProgrammingMode:
	cpi		RXCHAR, 'P'
	breq	putReturnAndMainLoop

leaveProgrammingMode:
	cpi		RXCHAR, 'L'
	breq	putReturnAndMainLoop

getFuseBytes:
	cpi		RXCHAR, 'r'
	breq	putZeroAndMainLoop
	cpi		RXCHAR, 'F'
	breq	putZeroAndMainLoop
	cpi		RXCHAR, 'N'
	breq	putZeroAndMainLoop
	cpi		RXCHAR, 'Q'
	breq	putZeroAndMainLoop

setDeviceType:
	cpi		RXCHAR, 'T'
	brne	returnProgrammerID
	rcall uartGet
	rjmp	uartPutReturnAndMainLoop

returnProgrammerID:
	cpi		RXCHAR, 'S'
	brne	returnSoftwareVersion
	ldi		ZL, lo8(programmerType)
	ldi   ZH, hi8(programmerType)
	rjmp	putString

returnSoftwareVersion:
	cpi		RXCHAR, 'V'
	brne	returnSignatureBytes
	ldi		ZL, lo8(softwareVersion)
	ldi		ZH, hi8(softwareVersion)
	rjmp	putString

returnSignatureBytes:
	cpi		RXCHAR, 's'
	brne	syncCharacter
	ldi		ZL, lo8(signatureBytes)
	ldi   ZH, hi8(signatureBytes)
putString:
	rcall	uartPutString
syncCharacterDone:
	rjmp	mainLoop

syncCharacter:
	cpi		RXCHAR, 0x1b
	breq	syncCharacterDone
	ldi		TEMP, '?'
  rjmp	uartPutAndMainLoop

putZeroAndMainLoop:
	ldi		TEMP, 0
	rjmp	uartPutAndMainLoop
