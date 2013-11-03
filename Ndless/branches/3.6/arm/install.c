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
static unsigned const ploader_hook_addrs[] = {0x10009984, 0x1000995C, 0x10009924, 0x10009924, 0x100098CC, 0x100098CC};

// OS-specific
static unsigned const init_task_return_addrs[] = {0x10001548, 0x10001548, 0x10001510, 0x10001510, 0x100014F8, 0x100014F8};

// OS-specific
static unsigned const api100_task_return_addrs[] = {0x100777A0, 0x10077708, 0x10076E9C, 0x10076e2c, 0x100738F0, 0x10073880};

// OS-specific
static unsigned const end_of_init_addrs[] = {0X100104F0, 0x10010478, 0x100104BC, 0x1001046C, 0x1000ED30, 0x1000ECE0};

// initialized at load time. Kept in resident program memory, use nl_is_3rd_party_loader to read it.
static BOOL loaded_by_3rd_party_loader = FALSE;

BOOL ins_loaded_by_3rd_party_loader(void) {
	return loaded_by_3rd_party_loader;
}

/* argv[0]=
 *         NULL if loaded by Ndless's stage1 at installation or OS startup
 *         "L" if loaded by a third party loader such as nLaunchy: we won't install ourself, and we'll exit the regular way
 *         <path to ndless_resources> if run from the OS documents screen for uninstallation      
 */
int main(int __attribute__((unused)) argc, char* argv[]) {
	ut_debug_trace(INSTTR_INS_ENTER);
	ut_read_os_version_index();
	BOOL installed = FALSE;

	struct next_descriptor *installed_next_descriptor = ut_get_next_descriptor();
	if (installed_next_descriptor) {
		if (*(unsigned*)installed_next_descriptor->ext_name == 0x534C444E) // 'NDLS'
			installed = TRUE;
		else
			ut_panic("unknown N-ext");
	}

	if (!argv[0] || argv[0][0] == 'L') { // not ndless_resources run
		ints_setup_handlers();
		sc_setup();
	}

	if (!installed) {
		HOOK_INSTALL(ploader_hook_addrs[ut_os_version_index], plh_hook);
		HOOK_INSTALL(end_of_init_addrs[ut_os_version_index], plh_startup_hook);
		lua_install_hooks();
	}
	
	if (argv[0] && argv[0][0] == 'L') { // third-party launcher
		loaded_by_3rd_party_loader = TRUE;
		return 0;
	}
	
	NU_TASK *current_task  = TCC_Current_Task_Pointer();
	char *task_name = ((char*)current_task) + 16;
	if (!strcmp(task_name, "API-100.")) { // Installation over USB
		// TODO
	} else { // either OS startup or ndless_resources.tns run
		if (installed) { // ndless_resources.tns run: uninstall
			if (nl_loaded_by_3rd_party_loader())
				return 0; // do nothing
			if (show_msgbox_2b("Ndless", "Do you really want to uninstall Ndless r" STRINGIFY(NDLESS_REVISION) "?\nThe device will reboot.", "Yes", "No") == 2)
				return 0;
			ut_calc_reboot();
		}
		// Continue OS startup
		// Simulate the prolog of the thread function for correct function return. Set r4 to a dummy variable, written to by a sub-function that follows.
		__asm volatile("add lr, pc, #8; stmfd sp!, {r4-r6,lr}; sub sp, sp, #0x18; mov r4, sp; mov pc, %0" : : "r" (init_task_return_addrs[ut_os_version_index]));
	}
	// never reached
	return 0;
}
