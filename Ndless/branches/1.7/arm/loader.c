/****************************************************************************
 * Ndless loader: installs Ndless
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

// TODO use stat instead
#define HOOK_MAX_SIZE 10000

/* OS-specific
 * addresses patched by ld_heap_alloc/ld_heap_patch():
 * sysmem_dm_pool address, system dynamic mem pool size constant */
static unsigned const ld_hook_alloc_patch_addrs[][2] = {
	{0x107957B0, 0x100002C0}, // 1.7 non-CAS
	{0, 0} // 1.7 CAS TODO
};

/* Allocate space at the end of the heap for the hook.
 * Returns a pointer to the block allocated. */
static void *ld_hook_alloc(void) {
	return (unsigned*)(ld_hook_alloc_patch_addrs[ut_os_version_index][0] // pool base
	                   + *(unsigned*)ld_hook_alloc_patch_addrs[ut_os_version_index][1] // pool size
	                   - HOOK_MAX_SIZE);
}

/* Patch the OS to rebase the heap after the hook block. */
static void ld_heap_patch(unsigned hook_size) {
	*(unsigned*)ld_hook_alloc_patch_addrs[ut_os_version_index][1] -= hook_size; // ajust the pool size
}

/* Returns the hook size */
static unsigned ld_copy_hook(void *hook_dest) {
	FILE *hook_file = fopen("/documents/ndless/ndless_resources.tns", "rb");
	if (!hook_file)
		ut_panic("can't find ndless_resources.tns");
	halt();
	size_t hook_size = fread(hook_dest, 1, HOOK_MAX_SIZE, hook_file); // TODO stat : maximum hook size
	if (!hook_size)
		ut_panic("can't read ndless_resources.tns");
	return hook_size;
}

void ld_load(void) {
	ut_read_os_version_index();
	sc_setup();
	void *hook_block = ld_hook_alloc();
	ld_heap_patch(ld_copy_hook(hook_block));
	puts("Ndless installed!");
	ut_os_reboot();
}
