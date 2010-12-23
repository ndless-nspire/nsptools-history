@ startup code

#include <os.h>

__base: .global __base
	@ signature identifying a program
	.asciz "PRG"

	stmfd sp!, {r4-r11,lr} @ and never destroy r0 and r1 which are C's argc and argv
	@ GOT-based relocation, required for C global variables. The program must be built with the GCC option -fpic.
relocate:
	@ Get the absolute address of the GOT. See http://www.google.com/codesearch/p?hl=en#FiIujMxKUHU/sites/sources.redhat.com/pub/glibc/snapshots/glibc-ports-latest.tar.bz2%7CDNu48aiJSpY/glibc-ports-20090518/sysdeps/arm/dl-machine.h&q=%22.word%20_GLOBAL_OFFSET_TABLE_%22
	ldr   r2, got_offset
get_got_offset:	
	add   r2, pc, r2
	adr   r3, __base
	ldr   r5, =__got_size
relocate_loop:
	subs  r5, #1
	ldrge r4, [r2]       @ next GOT entry
	addge r4, r4, r3     @ calculate the absolute address
	strge r4, [r2], #4   @ store it back to the GOT
	bge   relocate_loop

cache_keypad_type:
	mov  r2, r0          @ save it
	osvar(keypad_type)
	mov  r3, #0
	ldrb r3, [r0]
	mov  r0, r2          @ restore
	cmp  r3, #3
	bne runmain
	adr r2, is_touchpad
	mov r3, #1           @ TRUE
	str r3, [r2]

runmain:
	str  sp, __crt0_savedsp
	bl   main
__crt0exit: .global __crt0exit
	ldmfd sp!, {r4-r11,pc}

got_offset:
	.word _GLOBAL_OFFSET_TABLE_ - (get_got_offset+8)
__crt0_savedsp: .global __crt0_savedsp
	.long 0
is_touchpad: .global is_touchpad
	.long 0              @ default: FALSE
