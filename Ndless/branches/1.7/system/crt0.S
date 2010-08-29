@ startup code

	@ signature identifying a program
	.asciz "PRG"

base:

	@ GOT-based relocation, required for C global variables. The program must be built with the GCC option -fpic.
relocate:
	@ Get the absolute address of the GOT. See http://www.google.com/codesearch/p?hl=en#FiIujMxKUHU/sites/sources.redhat.com/pub/glibc/snapshots/glibc-ports-latest.tar.bz2%7CDNu48aiJSpY/glibc-ports-20090518/sysdeps/arm/dl-machine.h&q=%22.word%20_GLOBAL_OFFSET_TABLE_%22
	ldr r0, got_offset
get_got_offset:	
	add r0, pc, r0
	b after_got_offset
got_offset:
	.word _GLOBAL_OFFSET_TABLE_ - (get_got_offset+8)
after_got_offset:
	adr r1, base
relocate_loop:
	ldr r2, [r0]       @ next GOT entry
	cmp r2, #0
	addne r2, r2, r1   @ calculate the absolute address
	strne r2, [r0], #4 @ store it back to the GOT
	bne relocate_loop

	b main
