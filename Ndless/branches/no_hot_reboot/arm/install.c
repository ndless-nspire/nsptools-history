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
// call to the dialog box display telling that the format isn't recognized
static unsigned const ins_ploader_hook_addrs[] = {0x1000921C, 0x100091E8};

// OS-specific
// call to the dialog box display warning about low memory
unsigned const ins_lowmem_hook_addrs[] = {0x10012F24, 0}; // TODO CAS

BOOL ins_lowmem_hook_installed = TRUE; 

// At installation time, a low memory warning may pop up. Skip it.
// Since it may be called several during installation, it is uninstalled asynchronously by the ploaderhook.
HOOK_DEFINE(ins_lowmem_hook) {
  HOOK_RESTORE_RETURN_SKIP(ins_lowmem_hook, 4); // skip the original warning display
  HOOK_RESTORE_RETURN(ins_lowmem_hook);
}

void main(void) {
	ut_read_os_version_index();
	sc_setup();
	ints_setup_handlers();
	HOOK_INSTALL(ins_ploader_hook_addrs[ut_os_version_index], plh_hook);
	HOOK_INSTALL(ins_lowmem_hook_addrs[ut_os_version_index], ins_lowmem_hook);
}
