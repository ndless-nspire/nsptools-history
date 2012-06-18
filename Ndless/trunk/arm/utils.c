/****************************************************************************
 * Common functions
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

#include "ndless.h"
#include <stdlib.h>

#ifndef STAGE1
struct next_descriptor ut_next_descriptor = {
	.next_version = 0x00010000,
	.ext_name = "NDLS",
	.ext_version = 0x00010007 // will be incremented only if new functionnalities exposed to third-party tools
};
#endif

unsigned ut_os_version_index;

// OS-specific
extern unsigned syscalls_ncas_3_1_0[];
extern unsigned syscalls_light_ncas_3_1_0[];
extern unsigned syscalls_cas_3_1_0[];
extern unsigned syscalls_light_cas_3_1_0[];
extern unsigned syscalls_ncascx_3_1_0[];
extern unsigned syscalls_light_ncascx_3_1_0[];
extern unsigned syscalls_cascx_3_1_0[];
extern unsigned syscalls_light_cascx_3_1_0[];

/* Writes to ut_os_version_index a zero-based index identifying the OS version and HW model.
 * Also sets up the syscalls table.
 * Should be called only once.
 * May be used for OS-specific arrays of constants (marked with "// OS-specific"). */
void ut_read_os_version_index(void) {
	#if defined STAGE1
			sc_addrs_ptr = CONCAT(CONCAT(CONCAT(syscalls_light_, MODEL), _), OS_VERSION);
	switch (*(unsigned*)(OS_BASE_ADDRESS + 0x20)) {
		// OS-specific
		case 0x102F0FA0:  // 3.1.0 non-CAS
			ut_os_version_index = 0;
			break;
		case 0x102F16D0:  // 3.1.0 CAS
			ut_os_version_index = 1;
			break;
		case 0x102F0A10:  // 3.1.0 non-CAS CX
			ut_os_version_index = 2;
			break;
		case 0x102F11A0:  // 3.1.0 CAS CX
			ut_os_version_index = 3;
			break;
	}
	#else
	/* The heuristic is based on the address of INT_Initialize - Thanks Goplat.
	 * The address is read from the RAM copy and not the real vector which is
	 * destroyed at installation time */
	switch (*(unsigned*)(OS_BASE_ADDRESS + 0x20)) {
		// OS-specific
		case 0x102F0FA0:  // 3.1.0 non-CAS
			ut_os_version_index = 0;
			sc_addrs_ptr = syscalls_ncas_3_1_0;
			break;
		case 0x102F16D0:  // 3.1.0 CAS
			ut_os_version_index = 1;
			sc_addrs_ptr = syscalls_cas_3_1_0;
			break;
		case 0x102F0A10:  // 3.1.0 non-CAS CX
			ut_os_version_index = 2;
			sc_addrs_ptr = syscalls_ncascx_3_1_0;
			break;
		case 0x102F11A0:  // 3.1.0 CAS CX
			ut_os_version_index = 3;
			sc_addrs_ptr = syscalls_cascx_3_1_0;
			break;
		default:
			ut_panic("v?");
	}
	#endif
}
void __attribute__ ((noreturn)) ut_calc_reboot(void) {
	*(unsigned*)0x900A0008 = 2; //CPU reset
	__builtin_unreachable();
}

void __attribute__ ((noreturn)) ut_panic(const char *msg) {
	puts(msg);
	ut_calc_reboot();
}

#if 0
/* draw a dotted line. Line 0 is at the bottom of the screen (to avoid overwriting the installer) */
void ut_debug_trace(unsigned line) {
	volatile unsigned *ptr = (unsigned*)((char*)SCREEN_BASE_ADDRESS + (SCREEN_WIDTH/2) * (SCREEN_HEIGHT - 1 - line));
	unsigned i;
	for (i = 0; i < (SCREEN_WIDTH/2) / 4; i++)
		*ptr++ = line & 1 ? 0xFFFF0000 : 0x0000FFFF;
}
#endif

void ut_disable_watchdog(void) {
	// Disable the watchdog on CX that may trigger a reset
	*(volatile unsigned*)0x90060C00 = 0x1ACCE551; // enable write access to all other watchdog registers
	*(volatile unsigned*)0x90060008 = 0; // disable reset, counter and interrupt
	*(volatile unsigned*)0x90060C00 = 0; // disable write access to all other watchdog registers
}

#ifndef _NDLS_LIGHT
static int scmp(const void *sp1, const void *sp2) {
	return strcmp(*(char**)sp1, *(char**)sp2);
}


// Calls callback() for each file and folder found in folder and its subfolders. context is passed to callback() and can be NULL.
// callback() must return:
//  * 0 to continue the scan
//  * 1 to abort the scan
//  * 2 to continue the scan but not recurse through the current directory
// Returns non-zero if callback() asked to abort.
int ut_file_recur_each(const char *folder, int (*callback)(const char *path, void *context), void *context) {
	char subfolder_or_file[FILENAME_MAX];
	DIR *dp;
	struct dirent *ep;     
	struct stat statbuf;
	#define MAX_FILES_IN_DIR 100
	#define MEAN_FILE_NAME_SIZE 50
	#define FILENAMES_BUF_SIZE (MEAN_FILE_NAME_SIZE * MAX_FILES_IN_DIR)
	char *filenames;
	char *filenames_ptrs[MAX_FILES_IN_DIR];
	if (!(filenames = malloc(FILENAMES_BUF_SIZE)))
		return 0;
	if (!(dp = opendir(folder))) {
		free(filenames);
		return 0;
	}
	// list the directory to sort its content
	unsigned i;
	unsigned filenames_used_bytes = 0;
	char *ptr;
	for (i = 0, ptr = filenames; (ep = readdir(dp)) && i < MAX_FILES_IN_DIR && ptr < filenames + FILENAMES_BUF_SIZE; i++) {
		if (!strcmp(ep->d_name, ".") || !strcmp(ep->d_name, "..")) {
			i--;
			continue;
		}
		size_t dname_len = strlen(ep->d_name);
		if (FILENAMES_BUF_SIZE - filenames_used_bytes < dname_len + 1)
			break;
		strcpy(ptr, ep->d_name);
		filenames_ptrs[i] = ptr;
		ptr += dname_len + 1;
	}
	unsigned filenum = i;
	qsort(filenames_ptrs, filenum, sizeof(char*), scmp);
	
	for (i = 0; i < filenum; i++) {
		strcpy(subfolder_or_file, folder);
		if (subfolder_or_file[strlen(subfolder_or_file) - 1] != '/')
			strcat(subfolder_or_file, "/");
		strcat(subfolder_or_file, filenames_ptrs[i]);
		if (stat(subfolder_or_file, &statbuf) == -1)
			continue;
		int next_action = callback(subfolder_or_file, context);
		if (next_action == 1) {
			closedir(dp);
			free(filenames);
			return 1;
		}
		if (S_ISDIR(statbuf.st_mode) && next_action != 2) {
			if (ut_file_recur_each(subfolder_or_file, callback, context)) {
				closedir(dp);
				free(filenames);
				return 1;
			}
		}
	}
	closedir(dp);
	free(filenames);
	return 0;
}
#endif // _NDLS_LIGHT
