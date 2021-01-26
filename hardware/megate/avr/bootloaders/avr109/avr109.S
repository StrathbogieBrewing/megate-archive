#include 	<avr/io.h>

; this bootloader is compatible with avrdude and avrprog.
; it is broadly based on the avr 109 format
; first program the micro with this code in ISP mode
; rc oscillator calibrated to 1 MHz from 32.768 KHz external crystal
; baud rate 9600 bps, full duplex or half duplex

#define		BOOTSTART 	0x0F00
#define		ZERO		R4
#define		STATUS		R16
#define		ADDRL		R26
#define		ADDRH		R27
#define		WORDL		R18
#define		WORDH		R19
#define		RXCHAR		R20
#define		TEMP		R21
#define		TEMPL		R24
#define		TEMPH		R25

#define		PART_CODE	0x76
#define		SW_MAJOR  	'2'
#define		SW_MINOR  	'3'
#define		HW_MAJOR  	'4'
#define		HW_MINOR  	'5'
#define  	BUFFERSIZE	(SPM_PAGESIZE)

	.lcomm 	buffer, 	BUFFERSIZE

	.section .text

startTestExternalReset:
; *** test if bootloader requested by external reset ***
  in		TEMP, _SFR_IO_ADDR(MCUCSR)
  sbrs	TEMP, EXTRF

  ; jump to application code
	rjmp	- (BOOTSTART << 1)

  ; clear the external reset flag
  ldi   TEMP, ~(_BV(EXTRF))
  out   _SFR_IO_ADDR(MCUCSR), TEMP

	rjmp	startBootloader


; place strings in low and known area of flash to simplify indexing to single byte
programmerType:		.byte	'A', 'V', 'R', 'B', 'O', 'O', 'T', 0
signatureBytes:		.byte	SIGNATURE_2, SIGNATURE_1, SIGNATURE_0, 0
softwareVersion:	.byte 	SW_MAJOR, SW_MINOR, 0, 0

; *** internal rc calibration ***
calibrate:
	in		TEMPL, _SFR_IO_ADDR(TCNT2)
	andi	TEMPL, 0x07
	brne	calibrate

	; average latency of start loop is ((3 + 7) / 2) = 4.5 cpu cycles
calibrateWaitStart:
	in		TEMPH, _SFR_IO_ADDR(TCNT2) ; 1 cycle
	andi	TEMPH, 0x07                ; 1 cycle
	breq	calibrateWaitStart         ; 1 cycle if no branch, 2 cycles if branch

	; calibrateTimerLoop is executed for 31 periods of 32.768 kHz clock or 946 us
  ; plus average latency of (4.5 + 2.5) = 7 cpu cycles at 1 us / cycle
  ; total 946 + 7 = 951 us or 951 cpu cycles
  ; 951 / 5 cycles in loop = 190 counts

  ; 871.875 clock cycles @ 921.6 kHz
	; count should be 871.875 / 5 = 174.375 counts, 2.5 cylcles average latency = 174 counts

  ; average latency of stop loop is ((0 + 5) / 2) = 2.5 cpu cycles
calibrateTimerLoop:
	in		TEMPH, _SFR_IO_ADDR(TCNT2) ; 1 cycle
	inc		TEMPL                      ; 1 cycle
	andi	TEMPH, 0x20                ; 1 cycle
	breq	calibrateTimerLoop         ; 1 cycle if no branch, 2 cycles if branch

	ldi		TEMPH, 0

	; aim for target of between 189 and 191 counts
  ; average error is zero and allows for OSCCAL step size
calibrateSlow:
	cpi		TEMPL, 189
	brsh	calibrateFast
	ldi		TEMPH, 1

calibrateFast:
	cpi		TEMPL, 192
	brlo	calibrateDone
	ldi		TEMPH, -1

calibrateDone:
	in      TEMPL, _SFR_IO_ADDR(OSCCAL)
	add		TEMPL, TEMPH
	out     _SFR_IO_ADDR(OSCCAL), TEMPL
	ret

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

; *** calibrate rc clock routine ***
	; set ASSR for async clock
  ldi		TEMPH, (1 << AS2)
  out		_SFR_IO_ADDR(ASSR), TEMPH
	; set timer 2 with no prescaling in CTC mode
	ldi		TEMPH, (1 << CS20)
  out		_SFR_IO_ADDR(TCCR2), TEMPH

calibrationLoop:
	rcall	calibrate
	cpi		TEMPH, 0
	brne	calibrationLoop

; *** initialise uart ***
inituart:
	; set baud rate to 9600 bps
	ldi     TEMP, 12
	out     _SFR_IO_ADDR(UBRRL), TEMP
  ; enable double speed
  sbi     _SFR_IO_ADDR(UCSRA), U2X
	; enable tx and rx
	ldi     TEMP, ((1 << RXEN) | (1 << TXEN))
	out     _SFR_IO_ADDR(UCSRB), TEMP
	rjmp	mainLoop

; *** send subroutine (value in TEMP) ***
uartPut:
  ; force half duplex to block any echo if connected to a bus
  cbi		_SFR_IO_ADDR(UCSRB), RXEN		; disable rx
	out    _SFR_IO_ADDR(UDR), TEMP
uartTxLoop:
	sbis   _SFR_IO_ADDR(UCSRA), TXC
	rjmp   uartTxLoop
	sbi    _SFR_IO_ADDR(UCSRA), TXC
  sbi		_SFR_IO_ADDR(UCSRB), RXEN		; enable rx
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
	sbis    _SFR_IO_ADDR(UCSRA), RXC
	rjmp	  uartGet
	in      RXCHAR, _SFR_IO_ADDR(UDR)
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
	sbic 	_SFR_IO_ADDR(EECR), EEWE
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
	sbic 	_SFR_IO_ADDR(EECR), EEWE
	rjmp 	writeEepromWait
	out 	_SFR_IO_ADDR(EEARH), ADDRH
	out 	_SFR_IO_ADDR(EEARL), ADDRL
	out 	_SFR_IO_ADDR(EEDR), TEMP
	sbi 	_SFR_IO_ADDR(EECR), EEMWE
	sbi 	_SFR_IO_ADDR(EECR), EEWE
	adiw	ADDRL, 1
	sbiw	TEMPL, 1
	brne	writeEeprom

writeFlash:
	sbic 	_SFR_IO_ADDR(EECR), EEWE
	rjmp 	writeFlash
	movw	ZL, ADDRL
	add		ZL, ZL
	adc		ZH, ZH
	movw	WORDL, ZL

	ldi 	TEMP, (1<<PGERS) | (1<<SPMEN)		; page erase
	rcall	writeFlashSPM
	ldi 	TEMP, (1<<RWWSRE) | (1<<SPMEN)		; re-enable the RWW section
	rcall	writeFlashSPM

writeFlashLoop:
	ld		R0, Y+
	ld		R1, Y+
	ldi 	TEMP, (1<<SPMEN)					; copy word to flash write buffer
	rcall	writeFlashSPM
	adiw	ZL, 2
	adiw	ADDRL, 1
	sbiw	TEMPL, 2
	brne	writeFlashLoop

	movw	ZL, WORDL
	rcall	writeFlashPage
	rjmp	uartPutReturnAndMainLoop

writeFlashSPM:
	in 		STATUS, _SFR_IO_ADDR(SPMCR)			; check for previous SPM complete
	sbrc 	STATUS, SPMEN
	rjmp 	writeFlashSPM
	out 	_SFR_IO_ADDR(SPMCR), TEMP			; execute spm with action given by TEMP
	spm
	ret

writeFlashPage:
	cpi		ZH, hi8(BOOTSTART << 1)
	brsh	writeBootLoaderInhibit
	ldi 	TEMP, (1<<PGWRT) | (1<<SPMEN)		; execute page write
	rcall	writeFlashSPM
	ldi 	TEMP, (1<<RWWSRE) | (1<<SPMEN)		; re-enable the RWW section
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
  out   _SFR_IO_ADDR(WDTCR), TEMP
infiniteLoop:
  rjmp  infiniteLoop

getProgrammerType:
	cpi		RXCHAR, 'p'
	brne	reportSupportedDeviceCodes
	ldi		TEMP, 'S'
  rjmp uartPutAndMainLoop

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
	rcall 	uartGet
	rjmp	uartPutReturnAndMainLoop

returnProgrammerID:
	cpi		RXCHAR, 'S'
	brne	returnSoftwareVersion
	ldi		ZL, lo8(programmerType)
	rjmp	putString

returnSoftwareVersion:
	cpi		RXCHAR, 'V'
	brne	returnSignatureBytes
	ldi		ZL, lo8(softwareVersion)
	rjmp	putString

returnSignatureBytes:
	cpi		RXCHAR, 's'
	brne	syncCharacter
	ldi		ZL, lo8(signatureBytes)

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
