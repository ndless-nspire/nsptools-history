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

#include <os.h>
#include "ndless.h"

extern void *ints_next_descriptor_ptr; // but static
extern void ints_undef_instr_handler(void);
extern void ints_swi_handler(void);
extern void ints_prefetch_abort_handler(void);
extern void ints_data_abort_handler(void);

void ints_setup_handlers(void) {
#ifdef _NDLS_LIGHT
	*(void**)INTS_SWI_HANDLER_ADDR = &ints_swi_handler;
#else
	void **adr_ptr = (void**)INTS_INIT_HANDLER_ADDR;
	// The address is used by nspire_emu for OS version detection and must be restored to the OS value
	*adr_ptr++ = *(void**)(OS_BASE_ADDRESS + INTS_INIT_HANDLER_ADDR);
	*adr_ptr++ = &ints_undef_instr_handler;
	*adr_ptr++ = &ints_swi_handler;
	*adr_ptr++ = &ints_prefetch_abort_handler;
	*adr_ptr = &ints_data_abort_handler;
	// also change the SWI handler in the OS code, required by the N-ext convention
	*(void**)(OS_BASE_ADDRESS + INTS_SWI_HANDLER_ADDR) = &ints_swi_handler;
 	ints_next_descriptor_ptr = &ut_next_descriptor;
#endif
}

/* All the code run with _NDLS_LIGHT defined must be PC-relative (the loader is not relocated)
 * TODO:
 * Check that the swi number is correct
 * Check if it is null. If not null, reboot
 */
asm(
" .arm \n"
#ifndef STAGE1
" @ N-ext convention: a signature and a pointer to the descriptor, before the SWI handler address in the OS copy of the vectors \n"
" .long " STRINGIFY(NEXT_SIGNATURE) "\n"
"ints_next_descriptor_ptr: .long 0 \n"
#endif
"ints_swi_handler: .global ints_swi_handler  @ caution: 1) only supports calls from the svc and user mode (many syscalls will reboot with TCT_Check_Stack in other modes anyway) 2) destroys the caller's mode lr \n"
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
" subs pc, pc, #4         @  move spsr to cpsr (get back to the mode of the caller and restore the ints mask) \n"
"	stmfd sp!, {r0-r2, r3}  @ r3 is dummy and will be overwritten with the syscall address. Caution, update the offset below if reg list changed. \n"
"@ extract the syscall number from the comment field of the swi instruction \n"
" tst   lr, #1            @ was the caller in thumb state? \n"
"	ldreq r0, [lr, #-4]     @ ARM state \n"
" biceq r0, r0, #0xFF000000 \n"
" ldrneh r0, [lr, #-3]    @ thumb state (-2-1, because of the previous +1) \n"
" bicne r0, r0, #0xFF00 \n"
#ifndef _NDLS_LIGHT // with extension/emu support
" mov   r1, r0            @ syscall number \n"
" and   r1, #0xE00000   @ keep the 3-bit flag \n"
" bic   r0, #0xE00000   @ clear the flag \n"
" cmp   r1, #" STRINGIFY(__SYSCALLS_ISEXT) "\n"
" beq   is_ext_syscall \n"
" cmp   r1, #" STRINGIFY(__SYSCALLS_ISEMU) "\n"
" beq   is_emu_syscall \n"
#endif
" ldr   r2, sc_addrs_ptr  @ OS syscalls table \n"
" ldr   r0, [r2, r0, lsl #2] @ syscall address \n"
#ifndef _NDLS_LIGHT // with var support
" cmp   r1, #" STRINGIFY(__SYSCALLS_ISVAR) "\n"
" bne   jmp_to_syscall \n"
" str   r0, [sp]          @ overwrite the saved r0: it's the return valeu \n"
" mov   r0, lr            @ return from the swi instead of jumping to the syscall \n"
#endif
"jmp_to_syscall: \n"
" str   r0, [sp, #12]     @ overwrite the dummy register previously saved \n"
" ldmfd sp!, {r0-r2, pc}  @ restore the regs and jump to the syscall. lr is still the return address \n"

#ifndef _NDLS_LIGHT // with extension/emu support
"is_ext_syscall: \n"
" ldr   r1, get_ext_table_reloc @ from here...\n"
" ldr   r2, get_ext_table_reloc+4 \n"
"get_ext_table: \n"
" add   r1, pc \n"
"get_table: \n"
" ldr   r2, [r1, r2]      @ ...to there: GOT-based access to sc_ext_table (defined in another .o). TODO: Could be optimized with http://gcc.gnu.org/bugzilla/show_bug.cgi?id=43129 once available? \n"
" ldr   r0, [r2, r0, lsl #2] @ syscall address \n"
" b jmp_to_syscall \n"

"is_emu_syscall: \n"
" ldr   r1, get_emu_table_reloc \n"
" ldr   r2, get_emu_table_reloc+4 \n"
"get_emu_table: \n"
" add   r1, pc \n"
" b get_table \n"
#endif

"sc_addrs_ptr: .global sc_addrs_ptr @ defined here because accessed with pc-relative instruction \n"
" .long 0 \n"
#ifndef _NDLS_LIGHT
"get_ext_table_reloc: .long _GLOBAL_OFFSET_TABLE_-(get_ext_table+8) \n"
" .long sc_ext_table(GOT) \n"
"get_emu_table_reloc: .long _GLOBAL_OFFSET_TABLE_-(get_emu_table+8) \n"
" .long emu_sysc_table(GOT) \n"
#endif
);

/* Exception handlers for the installer are in bootstrapper.S */
#ifndef _NDLS_LIGHT
// Exception handlers when Ndless is installed, to make debugging on real hw easier
asm(
" .arm \n"
"ints_data_abort_handler: .global ints_data_abort_handler \n"
" adr r0, 1f \n"
" b 10f \n"
"ints_prefetch_abort_handler: .global ints_prefetch_abort_handler \n"
" adr r0, 2f \n"
" b 10f \n"
"ints_undef_instr_handler: .global ints_undef_instr_handler \n"
" adr r0, 3f \n"
"10: \n"
" mov r1, lr \n"
" stmfd sp!, {r0, r1} \n"
" ldmfd sp, {r0, r1}^     @ ^: to user-mode regs \n"
" add   sp, sp, #8        @ 'sp!' with ^ in the previous instruction is considered to produce an unpredictable result by GAS \n"
" mrs   r2, cpsr \n"
" bic   r2, #0b1111       @ to user mode, to be able to call syscalls \n"
" msr   cpsr, r2 \n"
" bl nprintf \n"
" mov r0, #" STRINGIFY(INTS_EXCEPTION_SLEEP_CNT) "\n"
"0: \n"
" subs r0, #1 \n"
" bne 0b \n"
" b ut_calc_reboot \n"
"1: .asciz \"data abort exception, lr=%08x\\n\" \n"
"2: .asciz \"prefetch abort exception, lr=%08x\\n\"\n"
"3: .asciz \"undefined instruction exception, lr=%08x\\n\"\n"
);
#endif /* _NDLS_LIGHT */
