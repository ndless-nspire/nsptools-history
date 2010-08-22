/****************************************************************************
 * Ndless loader: loads the installer
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

#define NDLESS_RES_FILE "/documents/ndless/ndless_resources.tns"

/* OS-specific
 * addresses patched by ld_heap_alloc/ld_heap_patch():
 * sysmem_dm_pool address, system dynamic mem pool size constant */
static unsigned const ld_hook_alloc_patch_addrs[][2] = {
	{0x107957B0, 0x100002C0}, // 1.7 non-CAS
	{0, 0} // 1.7 CAS TODO
};

/* Allocate space at the end of the heap for the hook.
 * Returns a pointer to the block allocated. */
static void *ld_hook_alloc(unsigned hook_size) {
	return (unsigned*)(ld_hook_alloc_patch_addrs[ut_os_version_index][0] // pool base
	                   + *(unsigned*)ld_hook_alloc_patch_addrs[ut_os_version_index][1] // pool size
	                   - hook_size);
}

/* Patch the OS to rebase the heap after the hook block. */
static void ld_heap_patch(unsigned hook_size) {
	*(unsigned*)ld_hook_alloc_patch_addrs[ut_os_version_index][1] -= hook_size; // ajust the pool size
}

/* Returns the hook size */
static void ld_copy_hook(void *hook_dest, unsigned hook_size) {
	FILE *hook_file = fopen(NDLESS_RES_FILE, "rb");
	if (!hook_file)
		ut_panic("can't open res");
	if (fread(hook_dest, 1, hook_size, hook_file) != hook_size)
		ut_panic("can't read res");
}

void ld_load(void) {
	struct stat res_stat;
	ut_read_os_version_index();
	sc_setup();
	if (stat(NDLESS_RES_FILE, &res_stat))
		ut_panic("can't open res");
	void *hook_block = ld_hook_alloc(res_stat.st_size);
	ld_copy_hook(hook_block, res_stat.st_size);
	ld_heap_patch(res_stat.st_size);
	halt();
	((void (*)(void))hook_block)();
}
