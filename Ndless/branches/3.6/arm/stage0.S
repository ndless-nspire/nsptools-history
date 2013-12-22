/****************************************************************************
 * Stage 0 of the installation: entry point
 * Unescapes and loads stage1.
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
 * The Initial Developer of the Original Code is Fabian Vogt.
 * 
 * Portions created by the Initial Developer are Copyright (C) 2013
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): Olivier ARMAND <olivier.calc@gmail.com>
 ****************************************************************************/

#include <os.h>

@ Unescapes forbidden half-words (see tools/EscapeInstaller) and loads stage1.
@ Will also unescape crt0.
@ This code must *not* contain 0000 and 0009.

	@ we are in abt mode, switch to the standard svc mode
	mrs   r1, cpsr
	eor   r1, #0b100 @ svc mode
	msr   cpsr, r1
	
	@ deactivate D- and I-Cache
	mrc p15, 0, r0, c1, c0
	bic r0, #0x0004
	bic r0, #0x1000
	mcr p15, 0, r0, c1, c0

	@ Input: FF 00 FF 09 => 00 09
	adr r2, escaped_start
	add r0, r2, #4 @ Source (skip length)
	mov r1, #0x11800000
	mov lr, r1
	ldr r2, [r2] @ Length of escaped ORed with 0xFF000000
	and r2, r2, #0x00FFFFFF
	add r2, r2, r0 @End

  unescape_loop:
	@ Load first word
	ldr r3, [r0]
	add r0, r0, #4
	@ Load second word
	ldr r4, [r0]
	add r0, r0, #4

	@ r5 is unescaped word
	mov r5, #0
	@ r6 is unescaped byte
	mov r6, r3, lsr #16
	and r6, r6, #0xFF
	orr r5, r5, r6

	mov r6, r3, lsr #0
	and r6, r6, #0xFF
	orr r5, r5, r6, lsl #8

	mov r6, r4, lsr #16
	and r6, r6, #0xFF
	orr r5, r5, r6, lsl #16

	mov r6, r4, lsr #0
	and r6, r6, #0xFF
	orr r5, r5, r6, lsl #24

	str r5, [r1]
	add r1, r1, #4

	cmp r0, r2 @ Have we finished?
	bls unescape_loop
	
	mov pc, lr

escaped_start:
	@ concatenated here