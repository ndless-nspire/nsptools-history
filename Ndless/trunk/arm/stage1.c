/****************************************************************************
 * Stage 1 of the installation.
 * Loads the installer.
 * Error checking is disabled due to the size constraint of ndless_installer.
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
 * Portions created by the Initial Developer are Copyright (C) 2010-2011
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 *                 Geoffrey ANNEHEIM <geoffrey.anneheim@gmail.com>
 ****************************************************************************/

#include "ndless.h"

/* The error handling is commented and only enabled for debugging purposes
 * because of the size constraints of the installer.
 */
void stage1(void) {
	struct stat res_stat;
	unsigned *uptr;
	unsigned *osptr;
	unsigned i;
	
	ut_debug_trace(INSTTR_S1_ENTER);

	ut_disable_watchdog();

	// Restore a few bytes at 0 destroyed by the TNO installation
	for (i = 0, uptr = 0, osptr = OS_BASE_ADDRESS; i < 5; i++)
		*uptr++ = *osptr++;
	
	ut_read_os_version_index();
	ints_setup_handlers();
	
	const char *res_path = "/documents/ndless/ndless_resources.tns";
	FILE *res_file = fopen(res_path, "rb");
	stat(res_path, &res_stat);
	char *core = malloc(res_stat.st_size);
	fread(core, res_stat.st_size, 1, res_file);
	fclose(res_file);
	ut_debug_trace(INSTTR_S1_LOADINST);
	char *res_params = " "; // Dummy filename to tell the installer we are booting or installing, and not running ndless_resources
	clear_cache();
	((void (*)(int argc, void* argv))(char*)core + sizeof(PRGMSIG))(1, &res_params); // Run the core installation
}
