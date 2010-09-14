/****************************************************************************
 * Ndless bootstrapper
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

#include <os.h>

ints_vectors:
	@ Interrupt vectors: jump instructions
 .byte 0x18, 0xF0, 0x9F, 0xE5
 .byte 0x18, 0xF0, 0x9F, 0xE5, 0x18, 0xF0, 0x9F, 0xE5, 0x18, 0xF0, 0x9F, 0xE5
 .byte 0x18, 0xF0, 0x9F, 0xE5, 0x18, 0xF0, 0x9F, 0xE5, 0x18, 0xF0, 0x9F, 0xE5, 0x18, 0xF0, 0x9F, 0xE5
	@ Interrupt vectors: default addresses (used by the jump instructions)
	.long ints_halt_handler @ initialize
	.long ints_halt_handler @ undef instr
	.long ints_empty_handler4 @ software
	.long ints_halt_handler @ prefetch abort
	.long ints_empty_handler4 @ data abort
	.long ints_halt_handler @ reserved
ints_irq_addr:
	.long main @ IRQ - our entry point by default
	.long ints_empty_handler4 @ FIQ

/* First set of instructions executed at installation time, as an IRQ handler */  
main:
	adr r0, ints_irq_addr
	ldr r1, =(OS_BASE_ADDRESS+ints_irq_addr-ints_vectors)
	ldr r1, [r1]
	str r1, [r0]    @ restore the IRQ handler
	subs pc, pc, #4 @ move spsr to cpsr (get back to caller mode: svc)
	bl ints_setup_handlers
	ldr r0, =ld_load @ no relocation required, 0 is the base address
	bx r0 @ and switch to thumb mode
	@ never reached
