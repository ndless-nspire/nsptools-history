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
 * Portions created by the Initial Developer are Copyright (C) 2010
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
static unsigned const ins_ploader_hook_addrs[] = {0x1000921C, 0x100091F0, 0x10008BCC, 0x10008B9C, 0x10008D94, 0x10008D64};

// OS-specific
// Call to the dialog box display warning about low memory.
// OS 2.x don't use show_dialog_box2.
unsigned const ins_lowmem_hook_addrs[] = {0x10012F24, 0x10012E6C, 0x10013F6C, 0x10013E94, 0x100145A0, 0x100144E4};

BOOL ins_lowmem_hook_installed = TRUE; 

// At installation time, a low memory warning may pop up. Skip it.
// Since it may be called several during installation, it is uninstalled asynchronously by the ploaderhook.
HOOK_DEFINE(ins_lowmem_hook) {
  HOOK_RESTORE_RETURN_SKIP(ins_lowmem_hook, 4); // skip the original warning display
}

int main(void) {
	ut_debug_trace(INSTTR_INS_INSTALL);
	ut_read_os_version_index();
	sc_setup();
	ints_setup_handlers();
	HOOK_INSTALL(ins_ploader_hook_addrs[ut_os_version_index], plh_hook);
	HOOK_INSTALL(ins_lowmem_hook_addrs[ut_os_version_index], ins_lowmem_hook);
	return 0;
}
