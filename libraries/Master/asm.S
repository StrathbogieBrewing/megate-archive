	.section .text
	.global asm_htons

asm_htons:
	mov		r18, r24
	mov		r24, r25
	mov		r25, r18
	ret

