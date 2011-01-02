/****************************************************************************
 * Stage 2 of the installation, loaded by stage 1 in a size-constrained
 * temporary buffer.
 * Puts the calculator back to a stable state and loads asyncronously
 * with a hook the last installation steps.
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

// OS-specific
// after the inflate loop, we want to simulate a null-return check and quit
static unsigned const s2_tizip_hook_addrs[] = {0x1019708C, 0x101990A4, 0x101EAC68, 0x101EB530, 0x101FE9CC, 0x101FF294};

static void s2_run_install(void);

HOOK_DEFINE(s2_tizip_hook) {
	ut_debug_trace(INSTTR_S2_TZHOOK);
	HOOK_UNINSTALL(s2_tizip_hook_addrs[ut_os_version_index], s2_tizip_hook); // uninstall itself
	// At this stage, malloc is possible: run the core installation
	s2_run_install();
	ut_debug_trace(INSTTR_S2_END);
	// Jump to the function's exit code (TI_ZIPArchive_Uncompress) to abort the decompression loop.
	// Caution, the offset may depend on the OS version.
	int skip_offset = ut_os_version_index <= 3 ? 0x24 : -0x20;
	HOOK_RESTORE_RETURN_SKIP(s2_tizip_hook, skip_offset);
}

void main(void) {
	ut_debug_trace(INSTTR_S2_HOOK);
	ut_read_os_version_index();
	ints_setup_handlers();
	struct next_descriptor *installed_next_descriptor = ut_get_next_descriptor();
	if (installed_next_descriptor) {
		if (*(unsigned*)installed_next_descriptor->ext_name == 0x534C444E) { // 'NDLS'
			puts("uninstalling");
			ut_calc_reboot();
		}
		else
			ut_panic("unknown N-ext");
	}
	HOOK_INSTALL(s2_tizip_hook_addrs[ut_os_version_index], s2_tizip_hook);
}

static void s2_run_install(void) {
	struct stat res_stat;
	unsigned stage2_size;
	char respath[0x300 + 40];
	char *path = respath;
	sprintf(respath, "/documents/%s/ndless_resources.tns",
	        (char*)ut_currentdocdir_addr[ut_os_version_index]);
	if (ut_os_version_index >= 2)
		path += 11; // strlen("/documents/"). Already included in ut_currentdocdir_addr for these OS versions.
	if (stat(path, &res_stat))
		ut_panic("s2st");
	FILE *res_file = fopen(path, "rb");
	if (!res_file)
		ut_panic("s2fo");
	// ndless_resources.tns starts with the size of stage2
	if (fread(&stage2_size, sizeof(stage2_size), 1, res_file) != 1)
		ut_panic("ldfrs");
	unsigned core_size = res_stat.st_size - stage2_size - sizeof(unsigned) /* stage 2 size */;
	void *hook_dest = malloc(core_size); // malloc is now possible
	if (!hook_dest)
		ut_panic("s2ma");
	if (fseek(res_file, sizeof(unsigned) /* stage 2 size */ + stage2_size, SEEK_SET))
		ut_panic("s2fs");
	if (fread(hook_dest, core_size, 1, res_file) != 1)
		ut_panic("s2fr");
	fclose(res_file);
	ut_debug_trace(INSTTR_S2_LOADINST);
	clear_cache();
	((void (*)(void))(char*)(hook_dest + sizeof("PRG")))(); // Run the core installation
}
