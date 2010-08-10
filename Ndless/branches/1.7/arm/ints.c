/****************************************************************************
 * @(#) Ndless interrupt handlers
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

extern void ints_swi_handler(void);

void ints_setup_handlers(void) {
	*(void**)0x28 = &ints_swi_handler; // replace the address for the swi jump instruction
}

/*
 * TODO:
 * Check that the swi number is correct
 */
asm (
"ints_swi_handler:        @ caution: 1) only supports calls from the user mode. 2) destroys the user lr \n"
" stmfd sp!, {lr} \n"
" ldmfd sp!, {lr}^        @ ^: move lr_svc (return address) to lr_user \n"
" stmfd sp!, {pc}         @ points to the instruction after the following one \n"
" ldmfd sp!, {pc}^        @ ^: jump to the next instruction and move spsr to cpsr (get back to the mode of the caller) \n"
"	stmfd sp!, {r0-r1, r2}  @ r2 is dummy and will be overwritten with the syscall address \n"
" adr   r1, syscalls_table \n"
"	ldr   r0, [lr, #-4]     @ extract the syscall number from the comment field of the swi instruction \n"
" bic   r0, r0, #0xFF000000 \n"
" ldr   r0, [r1, r0, lsl #2] @ syscall address \n"
" str   r0, [sp, #8]      @ overwrite the dummy r2 previously saved \n"
" ldmfd sp!, {r0-r1, pc}  @ restore the regs and jump to the syscall. lr is still the return address \n"
);

/*
 * the .text is required to be able to use 'adr' in ints_swi_handler (from the same section)
 * the \n\t is an ugly hack to avoid GCC's warning "ignoring changed section attributes for .text"
 * see http://forums.ps2dev.org/viewtopic.php?p=50638 and http://sources.redhat.com/ml/binutils/2000-06/msg00099.html
 */
unsigned __attribute__ ((section (".text\n\t#"))) syscalls_table[] = {
	0x102A274C
};
