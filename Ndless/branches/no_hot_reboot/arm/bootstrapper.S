/****************************************************************************
 * Ndless bootstrapper. Installs the stage1 hook.
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
 .byte 0x18, 0xF0, 0x9F, 0xE5, 0x18, 0xF0, 0x9F, 0xE5
int_irq:
 .byte 0x18, 0xF0, 0x9F, 0xE5, 0x18, 0xF0, 0x9F, 0xE5
	@ Interrupt vectors: default addresses (used by the jump instructions)
	.long 0 @ initialize
	.long 0 @ undef instr
	.long ints_swi_handler @ software
	.long 0 @ prefetch abort
	.long 0 @ data abort
	.long 0 @ reserved
ints_irq_addr:
	.long main @ IRQ - our entry point by default
	.long main @ FIQ

@ First set of instructions executed at installation time, as an IRQ handler
@ Caution, calling syscalls in IRQ mode is not possible
main:
	stmfd sp!, {r0-r3,r12,lr}  @ registers possibly destroyed by the calling convention

	adr   r0, ints_irq_addr
	ldr   r1, =(OS_BASE_ADDRESS+ints_irq_addr-ints_vectors)
	ldr   r2, [r1], #4
	str   r2, [r0], #4         @ restore the IRQ handler of the OS
	ldr   r2, [r1]
	str   r2, [r0]             @ restore the FIQ handler of the OS
	
	bl    ut_read_os_version_index
	bl    s1_install_hook
	ldmfd sp!, {r0-r3,r12,lr}
	b     int_irq              @ jump to the IRQ handler of the OS
	