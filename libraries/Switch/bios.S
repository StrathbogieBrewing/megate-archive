#include 	<avr/io.h>

#define		TEMPL	r24
#define		TEMPH	r25

#define		kShortPressTime	(64 / 16)
#define		kLongPressTime	(512 / 16)
#define		kRepeatTime	(128 / 16)
#define		kRepeatDelay	(1024 / 16)

#define 	PORTBSEGMENTMASK ((1 << PB0) | (1 << PB3) | (1 << PB4) | (1 << PB5))
#define 	PORTDSEGMENTMASK ((1 << PD4) | (1 << PD5) | (1 << PD6) | (1 << PD7))

	.align	1
	.lcomm 	bios_segment, 	8
	.lcomm 	bios_seconds, 	1
	.lcomm 	bios_minutes, 	1
	.lcomm 	bios_hours, 	1
	.lcomm 	bios_days, 		1
	.lcomm	key,			4
	.lcomm 	bios_ticks, 	1


	.section .text

	.global bios_init
	.global bios_segment
	.global bios_ticks
	.global bios_seconds
	.global bios_minutes
	.global bios_hours
	.global bios_days
	.global	bios_getKey
	.global TIMER2_COMP_vect


bios_init:
	ldi		TEMPL, (1 << AS2)
	out		_SFR_IO_ADDR(ASSR), TEMPL		// select asynchronous operation of timer2 (32,768kHz)

	ldi		TEMPL, 127						// set timer2 compare value for divide by 128
	out		_SFR_IO_ADDR(OCR2), TEMPL

	ldi		TEMPL, (1 << CS20) | (1 << WGM21)	// start timer2 with no prescaling in CTC mode
	out		_SFR_IO_ADDR(TCCR2), TEMPL

	in		TEMPL, _SFR_IO_ADDR(TIFR)		// enable interrupts
	ori		TEMPL, (1 << OCF2)
	out		_SFR_IO_ADDR(TIFR), TEMPL
	in		TEMPL, _SFR_IO_ADDR(TIMSK)
	ori		TEMPL, (1 << OCIE2)
	out		_SFR_IO_ADDR(TIMSK), TEMPL
	sei
	
	sbi		_SFR_IO_ADDR(DDRB), 1			// enable outputs
	sbi		_SFR_IO_ADDR(DDRB), 2
	
	sbi		_SFR_IO_ADDR(DDRC), 2			// enable aux power for analogue inputs
	sbi		_SFR_IO_ADDR(PORTC), 2
	
	ret

bios_getKey:
	cli
	lds		TEMPL, key
	clr		TEMPH
	sts		key, TEMPH
	sei
	ret


TIMER2_COMP_vect:
	push	TEMPL
 	push	TEMPH
	push	ZL
	push	ZH
	in		TEMPL, _SFR_IO_ADDR(SREG)
	push	TEMPL

periodicUpdate:
	ldi		ZL, lo8(bios_segment)
	ldi		ZH, hi8(bios_segment)


tickUpdate:
	ldd		TEMPL, Z + 16
	ldd		TEMPH, Z + 8
	adiw	TEMPL, 1
	std		Z + 16, TEMPL
seconds:	
	std		Z + 8, TEMPH
	cpi		TEMPH, 60
	brne	displayUpdate
	clr		TEMPH
	std		Z + 8, TEMPH
minutes:	
	ldd		TEMPH, Z + 9
	inc		TEMPH
	std		Z + 9, TEMPH
	cpi		TEMPH, 60
	brne	displayUpdate
	clr		TEMPH
	std		Z + 9, TEMPH
hours:
	ldd		TEMPH, Z + 10
	inc		TEMPH
	std		Z + 10, TEMPH
	cpi		TEMPH, 24
	brne	displayUpdate
	clr		TEMPH
	std		Z + 10, TEMPH
days:	
	ldd		TEMPH, Z + 11
	inc		TEMPH
	std		Z + 11, TEMPH

displayUpdate:
	ldd		TEMPH, Z + 16
	sbrc	TEMPH, 1
	rjmp	digitThree

digitOne:
	sbrc	TEMPH, 0
	rjmp	digitTwo

	cbi		_SFR_IO_ADDR(PORTC), 3

	ldi 	TEMPL, 103
	out 	_SFR_IO_ADDR(OCR2), TEMPL

	cbi		_SFR_IO_ADDR(DDRD), 2
	cbi		_SFR_IO_ADDR(PORTD), 2

	rcall	clearSegments

	ldd		TEMPH, Z + 13
	in		TEMPL, _SFR_IO_ADDR(PIND)
	andi	TEMPL, (1 << PD2)
	ori		TEMPL, (1 << 6)
	rcall	debounce
	std		Z + 13, TEMPH

	ldd		TEMPH, Z + 16
	ldd		TEMPL, Z + 0
	sbrc	TEMPH, 5
	ldd		TEMPL, Z + 4
	rcall	setSegments

	sbi		_SFR_IO_ADDR(PORTD), 2
	sbi		_SFR_IO_ADDR(DDRD), 2

	rjmp	periodicUpdateDone

digitTwo:
	cbi		_SFR_IO_ADDR(PORTD), 2

	ldi 	TEMPL, 199
	out 	_SFR_IO_ADDR(OCR2), TEMPL

	ldd		TEMPH, Z + 16
	ldd		TEMPL, Z + 1
	sbrc	TEMPH, 5
	ldd		TEMPL, Z + 5
	rcall	setSegments

	sbi		_SFR_IO_ADDR(PORTC), 6
	sbi		_SFR_IO_ADDR(DDRC), 6

	rjmp	periodicUpdateDone

digitThree:
	sbrc	TEMPH, 0
	rjmp	digitFour

	cbi		_SFR_IO_ADDR(PORTC), 6

	ldi 	TEMPL, 103
	out 	_SFR_IO_ADDR(OCR2), TEMPL

	cbi		_SFR_IO_ADDR(DDRD), 3
	cbi		_SFR_IO_ADDR(PORTD), 3

	rcall	clearSegments

	ldd		TEMPH, Z + 14
	in		TEMPL, _SFR_IO_ADDR(PIND)
	andi	TEMPL, (1 << PD3)
	ori		TEMPL, (1 << 7)
	rcall	debounce
	std		Z + 14, TEMPH

	ldd		TEMPH, Z + 16
	ldd		TEMPL, Z + 2
	sbrc	TEMPH, 5
	ldd		TEMPL, Z + 6
	rcall	setSegments

	sbi		_SFR_IO_ADDR(PORTD), 3
	sbi		_SFR_IO_ADDR(DDRD), 3

	rjmp	periodicUpdateDone

digitFour:
	cbi		_SFR_IO_ADDR(PORTD), 3

	ldi 	TEMPL, 103
	out 	_SFR_IO_ADDR(OCR2), TEMPL

	cbi		_SFR_IO_ADDR(DDRC), 3
	cbi		_SFR_IO_ADDR(PORTC), 3

	rcall	clearSegments

	ldd		TEMPH, Z + 15
	in		TEMPL, _SFR_IO_ADDR(PINC)
	andi	TEMPL, (1 << PC3)
	lsr		TEMPL
	lsr		TEMPL
	ori		TEMPL, (1 << 5)
	rcall	debounce
	std		Z + 15, TEMPH

	ldd		TEMPH, Z + 16
	ldd		TEMPL, Z + 3
	sbrc	TEMPH, 5
	ldd		TEMPL, Z + 7
	rcall	setSegments

	sbi		_SFR_IO_ADDR(PORTC), 3
	sbi		_SFR_IO_ADDR(DDRC), 3


periodicUpdateDone:
	pop		TEMPL
	out		_SFR_IO_ADDR(SREG), TEMPL
	pop		ZH
	pop		ZL
	pop		TEMPH
	pop		TEMPL
	reti


; state of button in TEMPL, zero if released
; debounce counter in TEMPH
debounce:
	swap	TEMPL
	cpi		TEMPL, 0X10
	brlo	debounceNoPress

debounceIncrement:
	sbrs	TEMPH, 7
	inc		TEMPH
	sbrc	TEMPH, 7
	dec		TEMPH
	cpi		TEMPH, -kLongPressTime
	brlt	debounceLongKey	
	cpi 	TEMPH, kLongPressTime
	brge	debounceLongKey
	ret

debounceLongKey:
	ldd		TEMPH, Z + 12
	andi	TEMPL, 0xF0
	or		TEMPH, TEMPL
	std		Z + 12, TEMPH
	ldi		TEMPH, -1
	ret
	
debounceNoPress:
	cpi		TEMPH, kShortPressTime
	brlt	debounceNoKey	

debounceShortKey:
	ldd		TEMPH, Z + 12
	or		TEMPH, TEMPL
	std		Z + 12, TEMPH

debounceNoKey:
	clr		TEMPH
	ret
	

; clear the segment outputs
clearSegments:
	ldi		TEMPL, 0


; set up segment outputs to match the pattern provided in TEMPL
setSegments:
	; transpose port b bits

	sbrs	TEMPL, 0
	sbi		_SFR_IO_ADDR(PORTB), 5
	sbrc	TEMPL, 0
	cbi		_SFR_IO_ADDR(PORTB), 5

	sbrs	TEMPL, 3
	sbi		_SFR_IO_ADDR(PORTB), 4
	sbrc	TEMPL, 3
	cbi		_SFR_IO_ADDR(PORTB), 4

	sbrs	TEMPL, 4
	sbi		_SFR_IO_ADDR(PORTB), 3
	sbrc	TEMPL, 4
	cbi		_SFR_IO_ADDR(PORTB), 3

	sbrs	TEMPL, 7
	sbi		_SFR_IO_ADDR(PORTB), 0
	sbrc	TEMPL, 7
	cbi		_SFR_IO_ADDR(PORTB), 0

	; transpose port d bits

	sbrs	TEMPL, 1
	sbi		_SFR_IO_ADDR(PORTD), 5
	sbrc	TEMPL, 1
	cbi		_SFR_IO_ADDR(PORTD), 5

	sbrs	TEMPL, 2
	sbi		_SFR_IO_ADDR(PORTD), 4
	sbrc	TEMPL, 2
	cbi		_SFR_IO_ADDR(PORTD), 4

	sbrs	TEMPL, 5
	sbi		_SFR_IO_ADDR(PORTD), 6
	sbrc	TEMPL, 5
	cbi		_SFR_IO_ADDR(PORTD), 6

	sbrs	TEMPL, 6
	sbi		_SFR_IO_ADDR(PORTD), 7
	sbrc	TEMPL, 6
	cbi		_SFR_IO_ADDR(PORTD), 7

	; set segment pins as outputs
	in 		TEMPL, _SFR_IO_ADDR(DDRB)
	ori		TEMPL, PORTBSEGMENTMASK
	out 	_SFR_IO_ADDR(DDRB), TEMPL
	in 		TEMPL, _SFR_IO_ADDR(DDRD)
	ori		TEMPL, PORTDSEGMENTMASK
	out 	_SFR_IO_ADDR(DDRD), TEMPL

	ret
