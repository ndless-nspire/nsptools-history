/****************************************************************************
 * @(#) Ndless bootstrapper
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
 * Contributor(s): 
 *                 Geoffrey ANNEHEIM <geoffrey.anneheim@gmail.com>
 ****************************************************************************/

/* In the 1MB section normally used for boot2 */
#define STACK_PTR_BASE_ADDR           0x1800E000

/* First set of instructions executed at installation time, as an IRQ handler */  
	@ Interrupt vectors: jump instructions
 .byte 0x18, 0xF0, 0x9F, 0xE5
 .byte 0x18, 0xF0, 0x9F, 0xE5, 0x18, 0xF0, 0x9F, 0xE5, 0x18, 0xF0, 0x9F, 0xE5
 .byte 0x18, 0xF0, 0x9F, 0xE5, 0x18, 0xF0, 0x9F, 0xE5, 0x18, 0xF0, 0x9F, 0xE5, 0x18, 0xF0, 0x9F, 0xE5
	@ Interrupt vectors: addresses (used by the jump instructions)
	@ Reset: our entry point
	.long main
	@ TODO set interrupt vectors
	.fill 7, 4, 0

main:
	mrs r0, cpsr_all
  bic r0, r0, #0b00011111  @ clear the mode
  orr r0, r0, #0b11010000  @ to user mode, and disable the interrupts
  msr cpsr_all, r0
	ldr sp, =STACK_PTR_BASE_ADDR
	bl ints_setup_handlers
halt:
	b halt
	swi 0
	bl ld_load
halt2:
	b halt2