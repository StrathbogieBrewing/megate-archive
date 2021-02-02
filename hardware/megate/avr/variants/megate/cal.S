#include 	<avr/io.h>

	.section .text
  .global cal_init

  ; *** calibrate rc clock routine ***
cal_init:
	; set ASSR for async clock, no interrupts to ruin the timing
  cli
  ldi		R25, (1 << AS2)
  out		_SFR_IO_ADDR(ASSR), R25
	; set timer 2 with no prescaling in CTC mode
	ldi		R25, (1 << CS20)
  out		_SFR_IO_ADDR(TCCR2), R25
calibrationLoop:
	rcall	calibrate
	cpi		R25, 0
	brne	calibrationLoop
  ret

; *** internal rc calibration ***
calibrate:
	in		R24, _SFR_IO_ADDR(TCNT2)
	andi	R24, 0x07
	brne	calibrate

	; average latency of start loop is ((3 + 7) / 2) = 4.5 cpu cycles
calibrateWaitStart:
	in		R25, _SFR_IO_ADDR(TCNT2)   ; 1 cycle
	andi	R25, 0x07                  ; 1 cycle
	breq	calibrateWaitStart         ; 1 cycle if no branch, 2 cycles if branch

	; calibrateTimerLoop is executed for 31 periods of 32.768 kHz clock or 946 us
  ; plus average latency of (4.5 + 2.5) = 7 cpu cycles at 1 us / cycle
  ; total 946 + 7 = 951 us or 951 cpu cycles
  ; 951 / 5 cycles in loop = 190 counts

  ; average latency of stop loop is ((0 + 5) / 2) = 2.5 cpu cycles
calibrateTimerLoop:
	in		R25, _SFR_IO_ADDR(TCNT2) ; 1 cycle
	inc		R24                      ; 1 cycle
	andi	R25, 0x20                ; 1 cycle
	breq	calibrateTimerLoop         ; 1 cycle if no branch, 2 cycles if branch

	ldi		R25, 0

	; aim for target of between 189 and 191 counts
  ; average error is zero and allows for OSCCAL step size
calibrateSlow:
	cpi		R24, 189
	brsh	calibrateFast
	ldi		R25, 1

calibrateFast:
	cpi		R24, 192
	brlo	calibrateDone
	ldi		R25, -1

calibrateDone:
	in      R24, _SFR_IO_ADDR(OSCCAL)
	add		R24, R25
	out     _SFR_IO_ADDR(OSCCAL), R24
	ret
