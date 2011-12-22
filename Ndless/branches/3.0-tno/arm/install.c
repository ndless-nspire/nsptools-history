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
extern char _binary_ndless_installer_bin_3_1_0_ncas_tmp_o_start[];
extern char _binary_ndless_installer_bin_3_1_0_ncas_tmp_o_end[];
extern char _binary_ndless_installer_bin_3_1_0_cas_tmp_o_start[];
extern char _binary_ndless_installer_bin_3_1_0_cas_tmp_o_end[];
extern char _binary_ndless_installer_bin_3_1_0_ncascx_tmp_o_start[];
extern char _binary_ndless_installer_bin_3_1_0_ncascx_tmp_o_end[];
extern char _binary_ndless_installer_bin_3_1_0_cascx_tmp_o_start[];
extern char _binary_ndless_installer_bin_3_1_0_cascx_tmp_o_end[];

// OS-specific
// Offset of the boot2.img's file name size in the installed OS file
static unsigned const persistent_offsets[] = {0x9688DC, 0x8BA5FF, 0x8943AC, 0x8D28A5};
// OS-specific
static char* const os_patch_data_addrs[] = {
	_binary_ndless_installer_bin_3_1_0_ncas_tmp_o_start,
	_binary_ndless_installer_bin_3_1_0_cas_tmp_o_start,
	_binary_ndless_installer_bin_3_1_0_ncascx_tmp_o_start,
	_binary_ndless_installer_bin_3_1_0_cascx_tmp_o_start,
};
// OS-specific
static char* const os_patch_data_end_addrs[] = {
	_binary_ndless_installer_bin_3_1_0_ncas_tmp_o_end, 
	_binary_ndless_installer_bin_3_1_0_cas_tmp_o_end,
	_binary_ndless_installer_bin_3_1_0_ncascx_tmp_o_end,
	_binary_ndless_installer_bin_3_1_0_cascx_tmp_o_end,
};

static void ins_persitent(void) {
	FILE *f = fopen("/phoenix/install/TI-Nspire.tnc", "r+b");
	if (!f) ut_panic("can't open OS file");
	if (fseek(f, persistent_offsets[ut_os_version_index], SEEK_SET)) ut_panic("can't fseek in OS file");
	nl_relocdata((unsigned*)os_patch_data_addrs, sizeof(os_patch_data_addrs)/sizeof(os_patch_data_addrs[0]));
	nl_relocdata((unsigned*)os_patch_data_end_addrs, sizeof(os_patch_data_end_addrs)/sizeof(os_patch_data_end_addrs[0]));
	unsigned patch_data_size = os_patch_data_end_addrs[ut_os_version_index] - os_patch_data_addrs[ut_os_version_index];
	if (fwrite(os_patch_data_addrs[ut_os_version_index], 1, patch_data_size, f) != patch_data_size)
		ut_panic("can't fwrite to OS file");
	fclose(f);
}

// OS-specific
// Call to the dialog box display telling that the format isn't recognized.
static unsigned const ploader_hook_addrs[] = {0x10009984, 0x1000995C, 0x10009924, 0x10009924};

// OS-specific
static unsigned const init_task_return_addrs[] = {0x10001548, 0x10001548, 0x10001510, 0x10001510};

int main(void) {
	ut_debug_trace(INSTTR_INS_ENTER);
	ut_read_os_version_index();

	struct next_descriptor *installed_next_descriptor = ut_get_next_descriptor();
	if (installed_next_descriptor) {
		if (*(unsigned*)installed_next_descriptor->ext_name == 0x534C444E) { // 'NDLS'
			puts("uninstalling");
			ut_calc_reboot();
		}
		else
			ut_panic("unknown N-ext");
	}

	ints_setup_handlers();
	sc_setup();

	HOOK_INSTALL(ploader_hook_addrs[ut_os_version_index], plh_hook);
	ins_persitent();
	
	NU_TASK *current_task  = TCC_Current_Task_Pointer();
	char *task_name = ((char*)current_task) + 16;
	if (!strcmp(task_name, "API-100.")) {
		// Installation over USB
		show_msgbox("Ndless", "Ndless installed successfully!");
	}
	else if (!strcmp(task_name, "gui")) {
		// Continue OS startup
		// Simulate the prolog of the thread function for correct function return. Set r4 to a dummy variable, written to by a sub-function that follows.
		__asm volatile("add lr, pc, #8; stmfd sp!, {r4-r6,lr}; sub sp, sp, #0x18; mov r4, sp; mov pc, %0" : : "r" (init_task_return_addrs[ut_os_version_index]));
	}
	ut_debug_trace(INSTTR_INS_END);
	TCC_Terminate_Task(current_task);
	// Never reached
	return 0;
}
