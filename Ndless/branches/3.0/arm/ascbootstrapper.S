/****************************************************************************
 * Ndless ASCII bootstrapper. Must contain only ASCII characters.
 * Loads the ascii85-encoded bootstrapper.
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
 * Portions created by the Initial Developer are Copyright (C) 2011
 * the Initial Developer. All Rights Reserved.
 *
 ****************************************************************************/

#include <os.h>
#include "ndless.h"

ints_vectors:
	@ Optimize: the vectors table is used as code
  @ Don't save any registers. The stack unwinding of the bootstrapper will restore those needed.
	to_thumb r0
	adr  r0, bootstrapper
	@ Use the screen as buffer
	ldr  r1, =(SCREEN_BASE_ADDRESS+(SCREEN_WIDTH/2)*40)
	add  r7, r1, #1  @ keep it (thumb address)
	bl    decode85
	b after_ints_irqs

	.org 0x18
	.arm
ints_irq:
@ First set of instructions executed at installation time, as an IRQ handler
	b ints_vectors
	
	.thumb
after_ints_irqs:
	@ inlined clear_cache
	to_arm r0
ccache_loop:
	mrc  p15, 0, r15, c7, c10, 3 @ test and clean DCache
	bne  ccache_loop
	mov  r0, #0
	mcr  p15, 0, r0, c7, c7, 0 @ invalidate ICache and DCache
	
	bx   r8

	.include "decode85.S"

bootstrapper: @ encoded bootstrapper appended here by the build chain
