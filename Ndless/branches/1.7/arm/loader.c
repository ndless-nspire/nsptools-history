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

/* OS-specific
 * addresses patched by ld_hook_alloc():
 * INT_bss_end constant, system dynamic mem pool size constant */
static unsigned const ld_hook_alloc_patch_addrs[][2] = {
	{0x1021121C, 0x100002C0}, // 1.7 non-CAS
	{0, 0} // 1.7 CAS TODO
};

/* Allocate space at the beginning of the heap for the hook.
 * Returns a pointer to the block allocated.
 */
static void *ld_hook_alloc(void) {
	return (unsigned*)ld_hook_alloc_patch_addrs[ut_os_version_index][0];
}

/*
 * Patch the OS to rebase the heap after the hook block.
 */
static void ld_heap_rebase(unsigned hook_size) {
	*(unsigned*)ld_hook_alloc_patch_addrs[ut_os_version_index][0] += hook_size; // moves the heap base up
	*(unsigned*)ld_hook_alloc_patch_addrs[ut_os_version_index][1] -= hook_size; // and ajust the pool size
}

/* Returns the hook size */
static unsigned ld_copy_hook(void *hook_dest) {
	FILE *hook_file = fopen("/documents/ndless/ndless_resources.tns", "rb");
	if (!hook_file)
		ut_panic("can't find ndless_resources.tns");
	// TODO stat
	// we aren't freading directly to hook_dest: fread depends on a valid heap
	void *buf = malloc(1000);
	halt();
	size_t hook_size = fread(buf, 1, 1000, hook_file); // maximum hook size
	if (!hook_size)
		ut_panic("can't read ndless_resources.tns");
	//memcpy(hook_dest, buf, 1000);
	// no free, no fclose: the beginning of the heap has been overwritten and we are about to reboot
	return hook_size;
}

void ld_load(void) {
	ut_read_os_version_index();
	sc_setup();
	void *hook_block = ld_hook_alloc();
	ld_heap_rebase(ld_copy_hook(hook_block));
	puts("Ndless installed!");
	ut_os_reboot();
}
