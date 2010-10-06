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

/* OS-specific: addresses patched by ld_heap_alloc/ld_heap_patch():
 * A: sysmem_dm_pool address, B: system dynamic mem pool size constant, C: upper memory cleanup limit calculation
 * The instruction pointed to by C is sub r3, r3, #0x100000 */
static unsigned const ld_hook_alloc_patch_addrs[][3] = {
	{0x107957B0, 0x100002C0, 0x10000088}, // 1.7 non-CAS
	{0x107597A0, 0x100002C0, 0x10000088} // 1.7 CAS
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
	*(unsigned*)ld_hook_alloc_patch_addrs[ut_os_version_index][2] = 0xE2433603; // lower the limit with: sub r3, r3, #0x300000: avoids cleaning up the hook on OS startup
}

/* OS-specific: addresses of the name of the directory containing the documen
 * being opened */
static unsigned const ld_currentdocdir_addr[] = {0x10669A9C, 0x1069BD64};

/* Returns the hook size */
static void ld_copy_hook(void *hook_dest, unsigned hook_size, const char *respath) {
	FILE *hook_file = fopen(respath, "rb");
	if (!hook_file)
		ut_panic("rs");
	if (fread(hook_dest, 1, hook_size, hook_file) != hook_size)
		ut_panic("rs");
}

void __attribute__((noreturn)) ld_load(void) {
	struct stat res_stat;
	char respath[0x300 + 40];
	ut_read_os_version_index();
	sc_setup();
	sprintf(respath, "/documents/%s/ndless_resources.tns",
	        (char*)ld_currentdocdir_addr[ut_os_version_index]);
	if (stat(respath, &res_stat))
		ut_panic("rs");
	void *hook_block = ld_hook_alloc(res_stat.st_size);
	ld_copy_hook(hook_block, res_stat.st_size, respath);
	ld_heap_patch(res_stat.st_size);
	((void (*)(void))(char*)(hook_block + sizeof("PRG")))();
	while(1); // noreturn attribute isn't available for function pointers...
}
