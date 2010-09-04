@ startup code

__base: .global __base
	@ signature identifying a program
	.asciz "PRG"

	@ GOT-based relocation, required for C global variables. The program must be built with the GCC option -fpic.
__relocate:
	@ Get the absolute address of the GOT. See http://www.google.com/codesearch/p?hl=en#FiIujMxKUHU/sites/sources.redhat.com/pub/glibc/snapshots/glibc-ports-latest.tar.bz2%7CDNu48aiJSpY/glibc-ports-20090518/sysdeps/arm/dl-machine.h&q=%22.word%20_GLOBAL_OFFSET_TABLE_%22
	ldr   r0, __got_offset
__get_got_offset:	
	add   r0, pc, r0
	b __after_got_offset
__got_offset:
	.word _GLOBAL_OFFSET_TABLE_ - (__get_got_offset+8)
__after_got_offset:
	adr   r1, __base
	ldr   r3, __got_size
__relocate_loop:
	subs  r3, #1
	ldrge r2, [r0]       @ next GOT entry
	addge r2, r2, r1   @ calculate the absolute address
	strge r2, [r0], #4 @ store it back to the GOT
	bge   __relocate_loop

	b main
