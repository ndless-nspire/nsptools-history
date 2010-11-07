/****************************************************************************
 * Hook and hook installation for stage 1. HOOK_() functions only work in
 * ARM mode, so this compilation unit is separated from stage1.c.
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

// OS-specific
// post-inflate 'bne' inflate() failure branch we want to force to stop the inflations
static unsigned const s1_inflate_hook_addrs[] = {0x101976BC, 0x0};

HOOK_DEFINE(s1_inflate_hook) {
	//HOOK_UNINSTALL(s1_inflate_hook_addrs[ut_os_version_index], s1_inflate_hook); // uninstall itself
	ut_debug_trace(1);
	s1_load();
	HOOK_RESTORE_RETURN_SKIP(s1_inflate_hook, 0x3C); // Jump to the function exit. Caution, the offset may depend on the OS version.
}

HOOK_SKIP_VAR(s1_inflate_hook, 0x3C);

void s1_install_hook(void) {
	HOOK_INSTALL(s1_inflate_hook_addrs[ut_os_version_index], s1_inflate_hook);
}
