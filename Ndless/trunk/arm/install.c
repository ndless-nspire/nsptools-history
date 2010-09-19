/****************************************************************************
 * Ndless installation
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
// Call to dialog box display saying that the format isn't recognized
static unsigned const ins_ploader_hook_addrs[] = {0x1000921C, 0x0};

static void ins_install_ploader_hook(void) {
	HOOK_INSTALL(ins_ploader_hook_addrs[ut_os_version_index], plh_hook);
}

void main(void) {
	ut_read_os_version_index();
	sc_setup();	
	ints_setup_handlers();
	struct next_descriptor *installed_next_descriptor = ut_get_next_descriptor();
	if (installed_next_descriptor) {
		if (*(unsigned*)installed_next_descriptor->ext_name == 0x4E444C53) { // 'NDLS'
			puts("uninstalling");
			ut_calc_reboot();
		}
		else
			ut_panic("unknown N-ext");
	}
	ints_hook_handlers();
	ins_install_ploader_hook();
	TCT_Local_Control_Interrupts(-1);
	ut_puts("Ndless installed!");
	ut_os_reboot();
}
