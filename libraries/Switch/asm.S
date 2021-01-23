	.section .text
	.global asm_htons
	.global asm_lo
	.global asm_hi

asm_htons:
	mov		r18, r24
	mov		r24, r25
	mov		r25, r18
	ret

asm_lo:
	clr		r25
	ret

asm_hi:
	mov		r24, r25
	clr		r25
	ret
