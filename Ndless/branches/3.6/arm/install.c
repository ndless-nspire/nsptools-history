/****************************************************************************
 * Final steps of the installation.
 * Installs the hooks at their target addresses.
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
 * Portions created by the Initial Developer are Copyright (C) 2010-2013
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 *                 Geoffrey ANNEHEIM <geoffrey.anneheim@gmail.com>
 ****************************************************************************/

#include <os.h>
#include "ndless.h"

// OS-specific
// Call to the dialog box display telling that the format isn't recognized.
static unsigned const ploader_hook_addrs[] = {0, 0, 0, 0x1000A924};

// initialized at load time. Kept in resident program memory, use nl_is_3rd_party_loader to read it.
static BOOL loaded_by_3rd_party_loader = FALSE;

BOOL ins_loaded_by_3rd_party_loader(void) {
	return loaded_by_3rd_party_loader;
}

static unsigned const end_of_init_addrs[] = {0, 0, 0, 0x10012424};

/* argv[0]=
 *         NULL if loaded by Ndless's stage1 at installation or OS startup
 *         "L" if loaded by a third party loader such as nLaunchy
 *         <path to ndless_resources> if run from the OS documents screen for uninstallation      
 */
int main(int __attribute__((unused)) argc, char* argv[]) {
	ut_debug_trace(INSTTR_INS_ENTER);
	ut_read_os_version_index();
	BOOL installed = FALSE;

// useless if non persistent and won't work since stage1 set it up
#if 0
	struct next_descriptor *installed_next_descriptor = ut_get_next_descriptor();
	if (installed_next_descriptor) {
		if (*(unsigned*)installed_next_descriptor->ext_name == 0x534C444E) // 'NDLS'
			installed = TRUE;
		else
			ut_panic("unknown N-ext");
	}
#endif

	if (!argv[0] || argv[0][0] == 'L') { // not opened from the Documents screen
		ints_setup_handlers();
		sc_setup();
	} else {
		installed = TRUE;
	}

	if (!installed) {
		// Startup programs cannot be run safely there, as stage1 is being executed in unregistered memory. Run them asynchronously in another hook.
		HOOK_INSTALL(end_of_init_addrs[ut_os_version_index], plh_startup_hook);
		HOOK_INSTALL(ploader_hook_addrs[ut_os_version_index], plh_hook);
		lua_install_hooks();
	}
	
	if (argv[0] && argv[0][0] == 'L') { // third-party launcher
		loaded_by_3rd_party_loader = TRUE;
		return 0;
	}
	
	if (installed) { // ndless_resources.tns run: uninstall
		if (show_msgbox_2b("Ndless", "Do you really want to uninstall Ndless r" STRINGIFY(NDLESS_REVISION) "?\nThe device will reboot.", "Yes", "No") == 2)
			return 0;
		ut_calc_reboot();
	}
	// continue OS startup
	return 0;
}
