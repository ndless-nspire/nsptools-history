/****************************************************************************
 * Stage 0 of the installation: entry point
 * Loads stage1.
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
 * Portions created by the Initial Developer are Copyright (C) 2010-2013
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 ****************************************************************************/

#include <os.h>

@ Unescapes forbidden half-words (see tools/MakeEscapeTable) and loads stage1.
@ Will also unescape crt0.
@ Must *not* contain 0000 and 0009.

@ first function called by crt0
@ parameter=__base
_crt0_init: .global _crt0_init
	adr r1, escape_table_offset
	ldrh r1, [r1]
	mov r3, r0 @ __base
	add r0, r1
	ldrh r1, [r0], #2 @ r1=escape table size, r0=escape table
unescape_loop:
	ldrh r2, [r0], #2
	add r2, r3 @ __base + offset
	ldrh r4, [r2]
			   @ use tst and not cmp to avoid the code to contain 0x0000
	tst r4, #0b10000 @ 0xFFFF is 0x0000 escaped -> bit 0=1. 0xEEEE is 0x0009 -> bit 0=0. 
	movne r4, #0
	moveq r4, #9
	strh r4, [r2]
	subs r1, #1
	bne unescape_loop
unescaped:
	bx lr

main: .global main
	bkpt
	@ to thumb mode as required by stage1
	to_thumb r0
	b stage1

// marker for MakeEscapeTable. The first half-word will be overwritten.
escape_table_offset: .long 0xDEADBEEF
