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
#include "ndless.h"

ints_vectors:
	@ Interrupt vectors: jump instructions
 .byte 0x18, 0xF0, 0x9F, 0xE5
 .byte 0x18, 0xF0, 0x9F, 0xE5, 0x18, 0xF0, 0x9F, 0xE5, 0x18, 0xF0, 0x9F, 0xE5
 .byte 0x18, 0xF0, 0x9F, 0xE5, 0x18, 0xF0, 0x9F, 0xE5
 .byte 0x18, 0xF0, 0x9F, 0xE5, 0x18, 0xF0, 0x9F, 0xE5
	@ Interrupt vectors: default addresses (used by the jump instructions)
ints_init_addr:
	.long ints_light_exception_handler @ initialize
ints_undef_addr:
	.long ints_light_exception_handler @ undef instr
	.long 0 @ software
ints_prefetch_addr:
	.long ints_light_exception_handler @ prefetch abort
ints_data_addr:
	.long ints_light_exception_handler @ data abort
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
	@ set up the exception handlers
	adr   r1, ints_light_exception_handler
	mov   r0, #0
	str   r1, [r0, #ints_init_addr-ints_vectors]
	str   r1, [r0, #ints_undef_addr-ints_vectors]
	str   r1, [r0, #ints_prefetch_addr-ints_vectors]
	str   r1, [r0, #ints_data_addr-ints_vectors]

	ldr   r1, =(OS_BASE_ADDRESS+ints_irq_addr)
	ldr   r2, [r1]
	add   r1, #4
	str   r2, [r0, #ints_irq_addr-ints_vectors]  @ restore the IRQ handler of the OS (and keep it)
	add   r0, #4
	ldr   r3, [r1]
	str   r3, [r0]             @ restore the FIQ handler of the OS
	to_arm r0
	add   lr, pc, #4           @ lr=return address+4 for IRQ interrupts. Will return to our code.
	bx    r2                   @ jump to the IRQ handler of the OS
	
	@ we are now in service mode
	to_thumb r0
	push  {r5-r7}              @ registers used and required by the cleanup code when jumping at pop_and_cleanup. Only popped if the IRQ was triggered directly in sub_101975EC (on OS 1.7)
	mov   r0, #INSTTR_BS_STACKUNWIND
	bl    ut_debug_trace
	mov   r4, r11              @ required later for stack unwinding
	bl    ints_setup_handlers
	bl    ut_read_os_version_index
	
	got_get ut_os_version_index, r0, r1
	ldr r0, [r0]
	adr r1, bs_stack_unwind_addrs
	lsl r0, #3                 @ sizeof(bs_stack_unwind_addrs)=8
	add r6, r1, r0             @ point to the entry of bs_stack_unwind_addrs for this OS version

	@ A Data Abort Exception may occur in this low-memory state.
	@ Prepare to unwind the call stack to a known state (after the call of inflate()).
	mov   r7, #0               @ depth of the call stack
	ldr   r3, [r6]             @ after the prologue of the caller of inflate()
	add   r6, #4
stack_unwind_loop:
	ldr   r0, [r4]             @ where pc is pushed by the ldmfd of the function's prologue
	cmp   r0, r3
	beq   frame_found
	add   r7, #1
	cmp   r7, #10              @ max depth
	beq   ints_light_exception_handler_thumb @ frame not found
	mov   r5, r4               @ r5=previous frame
	sub   r4, #4*3             @ pops r11 before r12, lr, and pc pushed by the function's prologue
	ldr   r4, [r4]
	b     stack_unwind_loop

frame_found:
	mov   r0, #INSTTR_BS_LOADS1
	bl    ut_debug_trace
	bl    s1_load              @ load stage1, which will load stage2, which will install its hooks
	mov   r0, #INSTTR_BS_CLEANUP
	bl    ut_debug_trace
	ldr   r0, [r6]             @ cleanup code
	@ We want to restore the registers for this frame to able to run the cleanup code.
	cmp   r7, #0               @ check the depth
	beq   pop_and_cleanup      @ the first frame is the current frame: no need to restore the registers
	@ point to the saved registers of the previous frame, as in a function prologue.
	@ Caution, this will only work for functions pushing r4-r11 (inflate() and sub_1009DF50 (OS 1.7)),
	@ and we should be one of them
	sub   r5, r5, #0x28
	to_arm r1
	ldmfd r5, {r4-r11,sp}      @ pop the regiters saved by the sub-function (but not pc)
	bx    r0                   @ jump to the heap cleanup code. The hooks installed by stage2 will then be called.

	.thumb
pop_and_cleanup:
	pop  {r5-r7}               @ restore the current registers required by the cleanup
	bx    r0

	got_var ut_os_version_index

@ OS-specific
@ Each OS version is on its own line:
@ 0) After the prologue of the caller of inflate()
@ 1) Cleanup code of this function
	.align 2
bs_stack_unwind_addrs:
	.long 0x101975FC, 0x10197700
	.long 0x0, 0x0 @ TODO

@ Exception handlers for bootstrapping
@ Defined here to be reference with pc-relative instructions for a lighter installer
	.arm
ints_light_exception_handler:
	to_thumb r0
ints_light_exception_handler_thumb:
	@ Short delay to display the ut_debug_traces.
	ldr  r0, =INTS_EXCEPTION_SLEEP_CNT
exception_sleep:
	sub r0, #1
	cmp r0, #0
	bne exception_sleep
	b ut_calc_reboot
