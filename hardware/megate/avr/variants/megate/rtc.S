#include 	<avr/io.h>

#define rtc_kTarget (488)
#define rtc_kTargetLow (rtc_kTarget - rtc_kTarget / 100)
#define rtc_kTargetHigh (rtc_kTarget + rtc_kTarget / 100)

	.align	1
	.lcomm 	rtc_seconds, 	1
	.lcomm 	rtc_minutes, 	1
	.lcomm 	rtc_hours, 	  1
	.lcomm 	rtc_days, 		1

	.section .text

	.global rtc_seconds
	.global	rtc_minutes
	.global	rtc_hours
	.global rtc_days

  .global rtc_init
	.global rtc_calibrate
	.global TIMER2_OVF_vect


; ******************************
; *** initialize rtc routine ***
; ******************************
rtc_init:
	; set ASSR for async clock,
  ldi		R22, (1 << AS2)
  out		_SFR_IO_ADDR(ASSR), R22
	; set timer 2 with 128 prescaling for 256 Hz clock (1 Hz overflow)
	ldi		R22, (1 << CS22) | (1 << CS20)
  out		_SFR_IO_ADDR(TCCR2), R22
waitASSR:
	in		R22, _SFR_IO_ADDR(ASSR)
	andi  R22, ((1 << TCN2UB) | (1 << OCR2UB) | (1 << TCR2UB))
	brne  waitASSR

	; initial OSCCAL calibration



	; enable timer overflow interrupt
	in		R22, _SFR_IO_ADDR(TIMSK)
	ori		R22, (1 << TOV2)
	out		_SFR_IO_ADDR(TIMSK), R22
	sei

	ret

; ***************************************
; *** calibrate rc oscillator routine ***
; ***************************************
rtc_calibrate:
	; disable interrupts and clear calibration timer
  cli
	clr R24
	clr r25

	; wait for TCNT2 lsb to change
	; avg start latency is (5 + 3) / 2 = 4 cycles
	in		R23, _SFR_IO_ADDR(TCNT2)
waitTCNT2Loop:
	in		R22, _SFR_IO_ADDR(TCNT2)  ; 1 cycle
	eor   R22, R23                  ; 1 cycle
	sbrs  R22, 0									  ; 1 cycle if no skip, 2 cycles if skip
	rjmp  waitTCNT2Loop             ; 2 cycles

	; calibrateTimerLoop is executed for 128 periods of 32.768 kHz clock
	; loop delay period is 8 cycles
	; for 1 MHz clock the target count is
	; 1000000 Hz / (256 Hz * 8 Cycles) = 488

	; wait for TCNT2 lsb to change again
	; average latency of stop loop is ((0 + 8) / 2) = 4 cycles
	; average start and stop latencies cancel (4 / 4)
calibrateLoop:
	nop                  						; 1 cycle
	in		R22, _SFR_IO_ADDR(TCNT2) 	; 1 cycle
	adiw	R24, 1                    ; 2 cycle
	eor	  R22, R23                  ; 1 cycle
	sbrc  R22, 0									  ; 1 cycle if no skip, 2 cycles if skip
	rjmp	calibrateLoop             ; 2 cycles

	sei

	; update OSCCAL value
	ldi		R22, lo8(rtc_kTargetLow)
	ldi		R23, hi8(rtc_kTargetLow)
	cp		R22, R24
	cpc		R23, R25
	brsh  increaseOSCCAL

	ldi		R22, lo8(rtc_kTargetHigh)
	ldi		R23, hi8(rtc_kTargetHigh)
	cp		R24, R22
	cpc		R25, R23
	brsh  decreaseOSCCAL

	movw  R24, R22
	clr   R24
	clr   R25
	sub   R24, R22
	sbc   R25, R23

	ret

increaseOSCCAL:
	in    R22, _SFR_IO_ADDR(OSCCAL)
	inc   R22
	out   _SFR_IO_ADDR(OSCCAL), R22
	ret

decreaseOSCCAL:
	in    R22, _SFR_IO_ADDR(OSCCAL)
	dec   R22
	out   _SFR_IO_ADDR(OSCCAL), R22
	ret


; **********************************************************
; *** 1 Hz interrupt update of real time clock registers ***
; **********************************************************
TIMER2_OVF_vect:
 	push	R25
	push	ZL
	push	ZH
	in		R25, _SFR_IO_ADDR(SREG)
	push	R25

	ldi		ZL, lo8(rtc_seconds)
	ldi		ZH, hi8(rtc_seconds)

; seconds
	ldd		R25, Z + 0
	inc   R25
	std		Z + 0, R25
	cpi		R25, 60
	brne	updateDone
	clr		R25
	std		Z + 0, R25

; minutes
	ldd		R25, Z + 1
	inc		R25
	std		Z + 1, R25
	cpi		R25, 60
	brne	updateDone
	clr		R25
	std		Z + 1, R25

; hours
	ldd		R25, Z + 2
	inc		R25
	std		Z + 2, R25
	cpi		R25, 24
	brne	updateDone
	clr		R25
	std		Z + 2, R25

; days
	ldd		R25, Z + 3
	inc		R25
	std		Z + 3, R25

updateDone:
	pop		R25
	out		_SFR_IO_ADDR(SREG), R25
	pop		ZH
	pop		ZL
	pop		R25
	reti
