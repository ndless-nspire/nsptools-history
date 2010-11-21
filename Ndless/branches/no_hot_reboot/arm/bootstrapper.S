/****************************************************************************
 * Ndless bootstrapper. Loads stage1 and breaks the inflate() loop.
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
main:   @ don't save any registers. The stack unwinding will restore those needed.
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
	bx    r3            @ jump to main_copy in the copy

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
	to_arm r0
	add   lr, pc, #4           @ lr=return address+4 for IRQ interrupts. Will return to our code.
	bx    r2                   @ jump to the IRQ handler of the OS
	@ we are now in service mode
	to_thumb r0
	mov   r4, r11              @ required later for stack unwinding
	bl    ints_setup_handlers
	bl    ut_read_os_version_index

	@ A Data Abort Exception may occur at any moment in this low-memory state.
	@ Unwind the stack to a known state (after the call of inflate()), instead of placing
	@ a hook and waiting for the calls to return.
	mov   r1, #0             @ depth of the call stack
stack_unwind_loop:
	ldr   r0, [r4]            @ where pc is pushed by the ldmfd of the function's prologue
	ldr   r3, =0x101975FC     @ TODO don't hard code. After the prologue of the caller of inflate()
	cmp   r0, r3
	beq   frame_found
	add   r1, #1
	cmp   r1, #10             @ max depth
	beq   frame_not_found
	mov   r5, r4              @ r5=last frame
	sub   r4, #4*3            @ pops r11 before r12, lr, and pc pushed by the function's prologue
	ldr   r4, [r4]      
	b     stack_unwind_loop
frame_not_found:
	halt	                    @ error, the frame was not found. TODO write error on screen

frame_found:
	bl    s1_load             @ load stage1, which will load stage2, which will install its hooks
	sub   r5, r5, #0x28       @ last frame: point to the saved registers (as in a function prologue)
	ldr   r0, =0x10197700
	to_arm r1
	ldmfd r5, {r4-r11,sp}     @ pop the regiters saved by the sub-function (but not pc)
	bx    r0                  @ jump to the heap cleanup code. The hooks installed by stage2 will then be called.
