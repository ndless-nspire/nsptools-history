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
	.long 0 @ software
	.long 0 @ prefetch abort
	.long 0 @ data abort
	.long 0 @ reserved
ints_irq_addr:
	.long main @ IRQ - our entry point by default
	.long main @ FIQ

@ First set of instructions executed at installation time, as an IRQ handler
@ Caution, calling syscalls in IRQ mode is not possible
main:
	stmfd sp!, {r0-r3,r12,lr, pc}  @ registers possibly destroyed by the calling convention. pc is dummy and will be overwritten
	to_thumb r0

	@ Copy the shellcode further down on the screen, to avoid being damaged
	@ by the icon refresh on the upper-right corner
	mov   r0, #0    @ ints_vectors
	ldr   r1, =(SCREEN_BASE_ADDRESS+(SCREEN_WIDTH/2)*40)
	mov   r3, r1    @ keep it
	ldr   r2, =1024 @ maximum size of the installer (see MakeLoader)
copy_installer_loop:
	ldr   r4, [r0]
	add   r0, #4
	str   r4, [r1]
	add   r1, #4
	sub   r2, #4    @ size--
	cmp   r2, #0
	bgt   copy_installer_loop

	@ GOT-based relocation for the copy, required for C global variables.
	@ r3 is the base address
	@ derived from system/crt0.S
relocate:
	@ Get the absolute address of the GOT
	ldr   r2, got_offset
get_got_offset:	
	add   r2, pc, r2
	add   r2, r3        @ use the GOT of the copy of the installer
	ldr   r1, =__got_size
relocate_loop:
	sub   r1, #1
	cmp   r1, #0
	blt   relocated
	ldr  r0, [r2]       @ next GOT entry
	add  r0, r0, r3     @ calculate the absolute address
	str  r0, [r2]       @ store it back to the GOT
	add  r2, #4
	b   relocate_loop

relocated:
	add   r3, #main_copy-ints_vectors+1 @ +1: still in thumb state
	bx    r3        @ jump to main_copy in the copy

	.align 2
got_offset:
	.long _GLOBAL_OFFSET_TABLE_ - (get_got_offset+4)   @ +4: we are in thumb state

main_copy:
	mov   r0, #ints_irq_addr-ints_vectors
	ldr   r1, =(OS_BASE_ADDRESS+ints_irq_addr)
	ldr   r2, [r1]
	add   r1, #4
	str   r2, [r0]             @ restore the IRQ handler of the OS (and keep it)
	add   r0, #4
	ldr   r3, [r1]
	str   r3, [r0]             @ restore the FIQ handler of the OS
	str   r2, [sp, #6*4]       @ IRQ handler of the OS. Overwrite the dummy pc previously pushed. Will by used the ldmfd.
	
	bl    ints_setup_handlers
	bl    ut_read_os_version_index
	bl    s1_install_hook

	to_arm r0
	ldmfd sp!, {r0-r3,r12,lr, pc} @ and jump to the IRQ handler of the OS
