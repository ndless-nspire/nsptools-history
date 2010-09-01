/****************************************************************************
 * Ndless interrupt handlers
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

#include "ndless.h"

extern void *next_descriptor_ptr;
extern void ints_swi_handler(void);

void ints_setup_handlers(void) {
 *(void**)INTS_SWI_HANDLER_ADDR = &ints_swi_handler;
}

/* similar to ints_setup_handlers(), but:
 * - on the OS copy of the vectors, for installation at next reboot
 * - sets the next_descriptor pointer */
void ints_hook_handlers(void) {
	*(void**)(OS_BASE_ADDRESS + INTS_SWI_HANDLER_ADDR) = &ints_swi_handler;
 	next_descriptor_ptr = &ut_next_descriptor;
}

/* TODO:
 * Check that the swi number is correct
 * Check it is null; if not null, reboot
 */
asm(
" .arm \n"
" @ N-ext convention: a signature and a pointer to the descriptor, before the SWI handler address in the OS copy of the vectors \n"
" .long " XSTRINGIFY(NEXT_SIGNATURE) "\n"
"next_descriptor_ptr: .long 0 \n"
"ints_swi_handler:        @ caution: 1) only supports calls from the svc and user mode (many syscalls will reboot with TCT_Check_Stack in other modes anyway) 2) destroys the caller's mode lr \n"
" str   r0, [sp, #-4]!    @ push r0 \n"
" mrs	  r0, spsr \n"
" tst   r0, #0b100000     @ caller in thumb state? \n"
" beq   stateok           @ ARM state \n"
" add   lr, lr, #1        @ so that the final 'bx lr' of the syscall switches back to thumb state \n"
" bic   r0, #0b100000     @ clear the caller's thumb bit. The syscall is run in 32-bit state \n"
" msr   spsr, r0 \n"
"stateok: \n"
" ands  r0, #0b1111       @ keep the caller's mode \n"
" ldr   r0, [sp], #4      @ pop r0 \n"
" bne   restorespsr       @ non-user mode (=svc mode): the current lr is the caller's lr and doesn't need to bet set \n"
" str   lr, [sp, #-4]!    @ push lr \n"
" ldmfd sp, {lr}^         @ ^: move lr_svc (return address) to lr_user \n"
" add   sp, sp, #4        @ 'sp!' with ^ in the previous instruction is considered to produce an unpredictable result by GAS \n"
"restorespsr: \n"
" stmfd sp!, {pc}         @ points to the instruction after the following one \n"
" ldmfd sp!, {pc}^        @ ^: jump to the next instruction and move spsr to cpsr (get back to the mode of the caller and restore the ints mask) \n"
"	stmfd sp!, {r0-r1, r2}  @ r2 is dummy and will be overwritten with the syscall address \n"
" ldr   r1, sc_addrs_ptr \n"
"@ extract the syscall number from the comment field of the swi instruction \n"
" tst   lr, #1            @ was the caller in thumb state? \n"
"	ldreq r0, [lr, #-4]     @ ARM state \n"
" biceq r0, r0, #0xFF000000 \n"
" ldrneh r0, [lr, #-3]     @ thumb state (-2-1, because of the previous +1) \n"
" bicne r0, r0, #0xFF00 \n"
" ldr   r0, [r1, r0, lsl #2] @ syscall address \n"
" str   r0, [sp, #8]      @ overwrite the dummy r2 previously saved \n"
" ldmfd sp!, {r0-r1, pc}  @ restore the regs and jump to the syscall. lr is still the return address \n"
);

/* The global var is defined, and placed in a .text section to be able to use 'adr' in ints_swi_handler
 * from the same section).
 * The \n\t is an ugly hack to avoid GCC's warning "ignoring changed section attributes for .text"
 * See http://forums.ps2dev.org/viewtopic.php?p=50638 and http://sources.redhat.com/ml/binutils/2000-06/msg00099.html */
 // its size is SYSCALLS_NUM
 __attribute__ ((section (".text\n\t#"))) unsigned *sc_addrs_ptr;

// Used for any exception for which we choose to halt
asm(
" .arm \n"
"ints_halt_handler: .global ints_halt_handler \n"
" 0: b 0b"
);

// Used for any exception for which we want to return back immediatly
// Only works for exceptions where lr = orig_pc + 4
asm(
" .arm \n"
"ints_empty_handler4: .global ints_empty_handler4 \n"
" subs pc, lr, #4" 
);
