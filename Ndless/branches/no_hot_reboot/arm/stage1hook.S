/****************************************************************************
 * Hook and hook installation for stage 1.
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
 ****************************************************************************/

/* Post-inflate 'bne' inflate() failure branch we want to force to stop the inflations.
 * The hook also launch stage2. 
 * Written in assembly because of the size constraints of the installer.
 * The HOOK_() functions would be too heavy, and switching from ARM state (required
 * by these functions) to Thumb state would require relocation currently not
 * possible. */

#include <os.h>

	.thumb_func
s1_install_hook: .global s1_install_hook
	push {lr}
	bl s1_get_hook_addr
	@ hook with ldr pc, [pc, #-4]; .long hook_addr
	ldr r2, =0xE51FF004 @ ldr pc, [pc, #-4]
	str r2, [r0]
	add r0, #4
	adr r2, s1_inflate_hook
	str r2, [r0]
	pop {pc}

s1_get_hook_addr:
	got_get ut_os_version_index, r0, r1
	ldr r0, [r0]
	adr r1, s1_inflate_hook_addrs
	ldr r0, [r1, r0]
	bx lr

	got_var ut_os_version_index

	.arm              @ the hook is called by the OS in ARM state 
s1_inflate_hook:
	to_thumb r0       @ r0 won't be used once we return and can be destroyed
	push {r6-r7}      @ save the only registers required once we return back to the OS
	push {lr}         @ separated, because popping lr directly is not possible in thumb state
	@ first uninstall itself: restore the 2 instructions overwritten
	bl  s1_get_hook_addr
	mov r4, r0
	adr r1, s1_hook_orig
	ldr r2, [r1]
	add r1, #4
	str r2, [r0]
	add r0, #4
	ldr r2, [r1]
	str r2, [r0]

	@ Free a 1kb memory block allocated by the function hooked.
	@ We need this memory for s1_load to be able to call some file-related syscalls.
	mov r0, r5
	syscall(free)
	@ The function will try to re-free it once we return: force a free(NULL)
	mov r5, #0
	
	bl s1_load

	pop {r0}           @ pop lr
	mov lr, r0
	pop {r6-r7}
	add r4, #0x4C      @ To the function cleanup and exit. Caution, the offset may change in future OS versions.
	bx  r4             @ and back to ARM state

// OS-specific
	.align 2
s1_inflate_hook_addrs:
	.long 0x101976B4, 0x0
	
@ Instructions overwritten
	.align 2
s1_hook_orig:
	.long 0xE1A03213, 0x8A000001
	