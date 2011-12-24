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

static void freados(void * ptr, size_t count, FILE * stream) {
	if (fread(ptr, 1, count, stream) != count)
		ut_panic("can't fread from OS file");
}

static void fwriteos(const void *ptr, size_t count, FILE *stream) {
	if (fwrite(ptr, 1, count, stream) != count)
		ut_panic("can't fwrite to OS file");
}

static void fseekos(FILE *stream, long int offset, int origin) {
	if (fseek(stream, offset, origin))
		ut_panic("can't fseek in OS file");
}

static const char zipped_file_header[] = "\x50\x4B\x03\x04\x0A\x00\x00\x00\x00\x00\xCA\x89\x0E\x3F\x07\xDF\x3A\xED\x1A\x6B\x6D\x00\x1A\x6B\x6D\x00";

static void ins_persitent(void) {
	FILE *f = fopen("/phoenix/install/TI-Nspire.tnc", "r+b");
	if (!f) ut_panic("can't open OS file");

	// Insert before boot2.cer, read at OS startup, or at the end of the zip file
	// Search the position
	int c;
	do {
		c = fgetc(f);
		if (c == EOF)
			ut_panic("can't skip OS file header");
	} while(c != 0x1A);
	while (1) {
		unsigned sig;
		freados(&sig, sizeof(sig), f);
		if (sig == 0x04034B50) { // local file header
			fseekos(f, 18 - 4, SEEK_CUR);
			unsigned short filename_len, extra_field_len;
			unsigned int compressed_size;
			freados(&compressed_size, sizeof(compressed_size), f);
			fseekos(f, 4, SEEK_CUR);
			freados(&filename_len, sizeof(filename_len), f);
			freados(&extra_field_len, sizeof(extra_field_len), f);
			char filename[20];
			if (filename_len >= 20)
				ut_panic("filename too long in OS file");
			if (!fgets(filename, filename_len, f))
				ut_panic("can't fgets in OS file");
			if (!strcmp(filename, "boot2.cer")) {
				fseekos(f, -(filename_len + 30), SEEK_CUR);
				break;
			} else {
				fseekos(f, extra_field_len + compressed_size, SEEK_CUR);
			}
		} else if (sig == 0x02014B50) { // central directory
			fseekos(f, -4, SEEK_CUR);
			break;
		} else {
			ut_panic("bad OS file format");
		}
	}

	nl_relocdata((unsigned*)os_patch_data_addrs, sizeof(os_patch_data_addrs)/sizeof(os_patch_data_addrs[0]));
	nl_relocdata((unsigned*)os_patch_data_end_addrs, sizeof(os_patch_data_end_addrs)/sizeof(os_patch_data_end_addrs[0]));
	unsigned patch_data_size = os_patch_data_end_addrs[ut_os_version_index] - os_patch_data_addrs[ut_os_version_index];
	
	// Extend the file: shift the content from the end with 'patch_data_size'-long chunks
	unsigned insert_size = patch_data_size + sizeof(zipped_file_header) - 1;
	long destpos = ftell(f);
	fseekos(f, -insert_size, SEEK_END);
	long curpos = ftell(f);
	void *shift_buf = malloc(insert_size);
	if (!shift_buf) ut_panic("can't malloc shift_buf");
	while (curpos >= destpos) {
		fseekos(f, curpos, SEEK_SET);
		freados(shift_buf, insert_size, f);
		fwriteos(shift_buf, insert_size, f);
		curpos -= insert_size;
	}
	if (curpos + insert_size - destpos) {
		fseekos(f, destpos, SEEK_SET);
		freados(shift_buf, curpos + insert_size - destpos, f);
		fseekos(f, destpos + insert_size, SEEK_SET);
		fwriteos(shift_buf, curpos + insert_size - destpos, f);
	}
	free(shift_buf);
	// Write the header
	fseekos(f, destpos, SEEK_SET);
	fwriteos(zipped_file_header, sizeof(zipped_file_header) - 1, f);
	fseekos(f, -(sizeof(zipped_file_header) - 1) + 18, SEEK_CUR); // compressed size
	fwriteos(&patch_data_size, sizeof(patch_data_size), f);
	fseekos(f, 4, SEEK_CUR); // skip the uncompressed size
	// write the payload
	fwriteos(os_patch_data_addrs[ut_os_version_index], patch_data_size, f);
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
	
	NU_TASK *current_task  = TCC_Current_Task_Pointer();
	char *task_name = ((char*)current_task) + 16;
	if (!strcmp(task_name, "API-100.")) {
		// Installation over USB
		ins_persitent();
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
