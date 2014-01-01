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
 * Contributor(s): Olivier ARMAND <olivier.calc@gmail.com>, Excale
 ****************************************************************************/

#include <os.h>

@ Unescapes forbidden half-words (see tools/EscapeInstaller) and loads stage1.
@ Will also unescape crt0.
@ This code must *not* contain 0000 and 0009.

	@ we are in abt mode, switch to the standard svc mode
	mrs   r1, cpsr
	eor   r1, #0b100 @ svc mode
	msr   cpsr, r1

	mov r1, #0x11800000
	mov lr, r1
	
	@ Input: FF 00 FF 09 => 00 09
	adr r2, escaped_start
	add r0, r2, #4 @ Source (skip length)
	ldr r2, [r2] @ Length of escaped ORed with 0xFF000000
	and r2, r2, #0x00FFFFFF
	add r2, r2, r0 @End

  unescape_loop:
	@ Load bytes
	ldrb r4, [r0], #2
	ldrb r3, [r0], #2
	ldrb r6, [r0], #2
	ldrb r5, [r0], #2

	orr r3, r3, r4, lsl #8
	orr r3, r3, r5, lsl #16
	orr r3, r3, r6, lsl #24

	@ Write word
	str r3, [r1], #4
	
	cmp r0, r2 @ Have we finished?
	bls unescape_loop
	
	@ inlined clear_cache, because branching to libndls function generates a 0000
clear_cache_loop:
	mrc p15, 0, r15, c7, c10, 3 @ test and clean DCache
	bne clear_cache_loop
	mov r1, #0
	mcr p15, 0, r1, c7, c7, 0 @ invalidate ICache and DCache

	mov pc, lr

escaped_start:
	@ concatenated here