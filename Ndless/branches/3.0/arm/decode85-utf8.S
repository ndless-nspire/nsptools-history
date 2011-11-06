/****************************************************************************
 * Ascii85 decoder, writtent with UTF-8-compliant instructions.
 * Follows doc/UTF-8-Shellcode.txt recommandations.
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
	stmfd sp!, {r0-r7, lr}
	adr r4, teststr
	adr r1, out
	adr r2, decode85
	add r2, r2, #1
	halt
	mov lr, pc
	bx  r2
	ldmfd sp!, {r0-r7, pc}
teststr:
	.ascii "<~BOu!rD]j7BEbo7nz~>"
out:
	.ascii "                "
	.align
@#endif
	
@ Algorithm based on http://www.stillhq.com/svn/trunk/ascii85/decode85.c
@ The text should *not* contain space characters (including \b, \n, ...)
@ r4=void *src, r1=void *dest
@ destroys r0-r7, r7
	.thumb
decode85:
	.global decode85
	.thumb_func
	mov  r7, lr
	adr  r5, pow85  @ TODO pow85's offset must be between 20-7F
	add  r4, #0x22  @ 2 instrs: add r4, #2  -> skip '<~'
	sub  r4, #0x20
reset:
	eor  r2, r2  @ tuple
	eor  r3, r3  @ count
loop85:
	ldrb r0, [r4]  @ next char
	add  r4, #0x22 @ 2 instrs: add r4, #1
	sub  r4, #0x21
	cmp  r0, #'z'
	bne  check_tilde @ TODO offset must be negative
wput4:
	mov r3, #0x24 @ 2 insts: mov r3, #4
	sub r3, #0x20

@ Inlined function, since 'bl' is not possible.
@ If r5 is 0x20, quits decode85 with 'bx r7'
@ Writes 'tuple' of size 'count', big endian.
@ destroys r0, r6
wput:
	mov  r6, #24
wput_loop:
	mov  r0, r2
	lsr  r0, r0, r6  @ tuple << 24, 16, 8, or 1
	strb r0, [r1]
	add  r1, #1
	sub  r6, #8
	sub  r3, r3, #1
	bne  wput_loop
	cmp  r5, #0x20
	bne  reset
quit:
	bx   r7

check_tilde:
	cmp  r0, #'~'
	bne  default
	mov r6, r3
	adds r6, #0x20   @ 2 instrs: cmp r6, #0
	subs r6, #0x20
	beq  quit
	mul  r6, #4
@ TODO UTF-8 rewrite stopped here

	ldr  r6, [r5, r6]
	add  r2, r2, r6  @ tuple += pow85[count]
	mov  r5, #0x20   @ wput will quit decode85
	b    wput
default:
	sub  r0, r0, #'!'
	mov  r6, r3
	lsl  r6, r6, #2
	ldr  r6, [r5, r6]
	add  r3, r3, #1
	mul  r6, r0, r6 @ tuple += (c - '!') * pow85[count++]
	add  r2, r2, r6
	cmp  r3, #5
	bne  loop85
	b    wput4

	.align 2
pow85: .long 85*85*85*85, 85*85*85, 85*85, 85, 1
