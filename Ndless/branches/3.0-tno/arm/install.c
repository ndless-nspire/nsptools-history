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
 * Portions created by the Initial Developer are Copyright (C) 2010-2011
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 *                 Geoffrey ANNEHEIM <geoffrey.anneheim@gmail.com>
 ****************************************************************************/

#include <os.h>
#include "ndless.h"

// OS-specific
// Call to the dialog box display telling that the format isn't recognized.
// On OS 2.x, it has been moved to a sub-function.
static unsigned const ins_ploader_hook_addrs[] = {0x10009984};

int main(void) {
	ut_debug_trace(INSTTR_INS_ENTER);
	ut_read_os_version_index();
	ints_setup_handlers();
	sc_setup();
	HOOK_INSTALL(ins_ploader_hook_addrs[ut_os_version_index], plh_hook);
	ut_debug_trace(INSTTR_INS_END);
	TCC_Terminate_Task(TCC_Current_Task_Pointer());
	// Never reached
	return 0;
}
