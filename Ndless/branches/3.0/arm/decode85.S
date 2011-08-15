/****************************************************************************
 * Ascii85 decoder.
 *
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Ndless code.
 *
 * The Initial Developer of the Original Code is Olivier ARMAND
 * <olivier.calc@gmail.com>.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 ****************************************************************************/

#include <os.h>

@#if 0
main: .global main
	stmfd sp!, {r0-r12, lr}
	adr r0, teststr
	adr r1, out
	adr r2, decode85
	add r2, r2, #1
	halt
	mov lr, pc
	bx  r2
	ldmfd sp!, {r0-r12, pc}
teststr:
	.ascii "<~BOu!rD]j7BEbo7nz~>"
out:
	.ascii "                "
	.align
@#endif
	
@ Algorithm based on http://www.stillhq.com/svn/trunk/ascii85/decode85.c
@ The text should *not* contain space characters (including \b, \n, ...)
@ r0=void *src, r1=void *dest
@ destroys r0-r6, r12
	.thumb
decode85:
	.global decode85
	.thumb_func
	mov  r12, lr
	adr  r5, pow85
	add  r0, r0, #2  @ skip '<~'
reset:
	mov  r2, #0  @ tuple
	mov  r3, #0  @ count
loop85:
	ldrb r4, [r0]  @ next char
	add  r0, r0, #1
	cmp  r4, #'z'
	bne  check_tilde
wput4:
	mov  r3, #4
	bl   wput
	b    reset
check_tilde:
	cmp  r4, #'~'
	bne  default
	movs r6, r3
	beq  quit
	lsl  r6, r6, #2
	ldr  r6, [r5, r6]
	add  r2, r2, r6  @ tuple += pow85[count]
	bl   wput
quit:
	bx   r12
default:
	sub  r4, r4, #'!'
	mov  r6, r3
	lsl  r6, r6, #2
	ldr  r6, [r5, r6]
	add  r3, r3, #1
	mul  r6, r4, r6 @ tuple += (c - '!') * pow85[count++]
	add  r2, r2, r6
	cmp  r3, #5
	bne  loop85
	b    wput4

@ Writes 'tuple' of size 'count', big endian.
@ destroys r4, r6
wput:
	mov  r6, #24
wput_loop:
	mov  r4, r2
	lsr  r4, r4, r6  @ tuple << 24, 16, 8, or 1
	strb r4, [r1]
	add  r1, #1
	sub  r6, #8
	sub  r3, r3, #1
	bne  wput_loop
	bx   lr

	.align 2
pow85: .long 85*85*85*85, 85*85*85, 85*85, 85, 1
