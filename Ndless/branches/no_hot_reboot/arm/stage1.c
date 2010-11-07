/****************************************************************************
 * Stage 1 of the installation. Installed as a hook to be run in a stable
 * state, and not directly by the bootstrapper which is trigger by an IRQ.
 * Loads stage 2 from ndless_resources.
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
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 *                 Geoffrey ANNEHEIM <geoffrey.anneheim@gmail.com>
 ****************************************************************************/

#include "ndless.h"

void s1_load(void) {
	unsigned stage2_size;
	ut_read_os_version_index();
	ints_setup_handlers();
#if 1
	char *respath = "/documents/ndless/ndless_resources.tns";
#else
	char respath[0x300 + 40];
	sprintf(respath, "/documents/%s/ndless_resources.tns",
	        (char*)ut_currentdocdir_addr[ut_os_version_index]);
#endif
	// We can't malloc in this low-memory conditions.
	// The screen is used as a temporary buffer for stage2.
	// Copy it at 3/4 from the top of the screen. We must avoid any dynamic icon.
	void *stage2_dest = (char*)SCREEN_BASE_ADDRESS + (SCREEN_WIDTH/2) * (3 * SCREEN_HEIGHT / 4);
	FILE *res_file = fopen(respath, "rb");
	if (!res_file)
		;//ut_panic("ldfo");
	// ndless_resources.tns starts with the size of stage2
	if (fread(&stage2_size, sizeof(stage2_size), 1, res_file) != 1)
		;//ut_panic("ldfrs");
	if (stage2_size > 0xFFFF)
		;//ut_panic("ldrs");
	if (fread(stage2_dest, stage2_size, 1, res_file) != 1)
		;//ut_panic("ldfr");
	fclose(res_file);
	((void (*)(void))(char*)(stage2_dest + sizeof("PRG")))(); // Run stage2
}
