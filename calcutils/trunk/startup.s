	.text

_start: .global _start
	ldr sp,=0x1800E15C
	b jumpconstpool
	.ltorg
jumpconstpool:
