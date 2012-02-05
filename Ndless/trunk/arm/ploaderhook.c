/****************************************************************************
 * Ndless program loader hooks
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
 * Portions created by the Initial Developer are Copyright (C) 2010-2012
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 *                 Geoffrey ANNEHEIM <geoffrey.anneheim@gmail.com>
 ****************************************************************************/

#include <os.h>
#include <stdlib.h>
#include "ndless.h"


static int scmp(const void *sp1, const void *sp2) {
	return strcmp(*(char**)sp1, *(char**)sp2);
}

// Return the file path, or NULL if not found. folder will be destroyed.
static char *find_file(char folder[FILENAME_MAX], const char *filename) {
	char subfolder[FILENAME_MAX];
	DIR *dp;
	struct dirent *ep;     
	struct stat statbuf;
	#define MAX_FILES_IN_DIR 100
	#define MEAN_FILE_NAME_SIZE 50
	#define FILENAMES_BUF_SIZE (MEAN_FILE_NAME_SIZE * MAX_FILES_IN_DIR)
	char *filenames;
	char *filenames_ptrs[MAX_FILES_IN_DIR];
	if (!(filenames = malloc(FILENAMES_BUF_SIZE)))
		return NULL;
	if (!(dp = opendir(folder))) {
		free(filenames);
		return NULL;
	}
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
		strcpy(subfolder, folder);
		strcat(subfolder, "/");
		strcat(subfolder, filenames_ptrs[i]);
		if (stat(subfolder, &statbuf) == -1)
			continue;
		if (!strcmp(filename, filenames_ptrs[i]) && S_ISREG(statbuf.st_mode)) {
			strcpy(folder, subfolder);
			closedir(dp);
			free(filenames);
			return folder;
		}
		if (S_ISDIR(statbuf.st_mode)) {
			char *found = find_file(subfolder, filename);
			if (found) {
				closedir(dp);
				free(filenames);
				return found;
			}
		}
	}
	closedir(dp);
	free(filenames);
	return NULL;
}

// When opening a document
HOOK_DEFINE(plh_hook) {
	char *halfpath; // [docfolder/]file.tns
	char docpath[FILENAME_MAX];
	int ret;
	unsigned i;
	char arg1[FILENAME_MAX];
	char argc = 1;
	FILE *docfile = NULL;
	halfpath = (char*)(HOOK_SAVED_REGS(plh_hook)[5] /* r5 */ + 32);
	// TODO use snprintf
	sprintf(docpath, "/documents/%s", halfpath);

	// File association
	char extbuf[FILENAME_MAX];
	strcpy(extbuf, docpath);
	char *ext = strrchr(extbuf, '.');
	if (!ext || ext == extbuf) goto cantopen; // shouldn't happen, all files have a .tns extension
	*ext = '\0'; // keep the extension before .tns
	ext = strrchr(extbuf, '.');
	unsigned pathlen = strlen(extbuf);
	// without '.'
	#define MAX_EXT_LEN 8
	if (ext && extbuf + pathlen - ext <= (MAX_EXT_LEN+1) && extbuf + pathlen - ext > 1) { // looks like an extension
		cfg_open();
		char ext_key[4 + MAX_EXT_LEN + 1]; // ext.extension
		strcpy(ext_key, "ext");
		strcat(ext_key, ext);
		char *prgm_name_noext = cfg_get(ext_key);
		if (prgm_name_noext) {
			char prgm_name[FILENAME_MAX + 4];
			strcpy(prgm_name, prgm_name_noext);
			strcat(prgm_name, ".tns");
			char folder[FILENAME_MAX];
			strcpy(folder, "/documents");
			char *found_prgm = find_file(folder, prgm_name);
			if (found_prgm) {
				strncpy(arg1, docpath, sizeof(arg1) - 1);
				strncpy(docpath, found_prgm, sizeof(docpath) - 1);
				argc = 2;
			}
		}
		cfg_close();
	}

	docfile = fopen(docpath, "rb");
	struct stat docstat;
	if (!docfile || (ret = stat(docpath, &docstat))) {
cantopen:
		puts("ploaderhook: can't open doc");
error_dialog:
		HOOK_SAVED_REGS(plh_hook)[3] = HOOK_SAVED_REGS(plh_hook)[0]; // 'mov r3, r0' was overwritten by the hook
		HOOK_RESTORE_RETURN_SKIP(plh_hook, -0x114, 0); // to the error dialog about the unsupported format (we've overwritten a branch with our hook)
	}

	void *docptr = emu_debug_alloc_ptr ? emu_debug_alloc_ptr : malloc(docstat.st_size);
	if (!docptr) {
		puts("ploaderhook: can't malloc");
		goto error_dialog;
	}
	if (!fread(docptr, docstat.st_size, 1, docfile)) {
		puts("ploaderhook: can't read doc");
		if (!emu_debug_alloc_ptr)
			free(docptr);
		goto error_dialog;
	}
	fclose(docfile);
	if (strcmp(PRGMSIG, docptr)) { /* not a program */
		if (!emu_debug_alloc_ptr)
			free(docptr);
		goto error_dialog;
	}
	int intmask = TCT_Local_Control_Interrupts(-1); /* TODO workaround: disable the interrupts to avoid the clock on the screen */
	void *savedscr = malloc(SCREEN_BYTES_SIZE);
	memcpy(savedscr, SCREEN_BASE_ADDRESS, SCREEN_BYTES_SIZE);
	if (has_colors) {
		volatile unsigned *palette = (volatile unsigned*)0xC0000200;
		for (i = 0; i < 16/2; i++)
			*palette++ = ((i * 2 + 1) << (1 + 16)) | ((i * 2 + 1) << (6 + 16)) | ((i * 2 + 1) << (11 + 16)) | ((i * 2) << 1) | ((i * 2) << 6) | ((i * 2) << 11); // set the grayscale palette
		ut_disable_watchdog(); // seems to be sometimes renabled by the OS
	}
	clear_cache();
	((void (*)(int argc, char *argv[]))(docptr + sizeof(PRGMSIG)))(argc, argc == 1 ? ((char*[]){docpath, NULL}) : ((char*[]){docpath, arg1, NULL})); /* run the program */
	if (has_colors) {
		lcd_incolor(); // in case not restored by the program
	}
	memcpy(SCREEN_BASE_ADDRESS, savedscr, SCREEN_BYTES_SIZE);
	free(savedscr);
	TCT_Local_Control_Interrupts(intmask);
	if (!emu_debug_alloc_ptr)
		free(docptr);
	HOOK_RESTORE_RETURN_SKIP(plh_hook, -0xDC, 1); // skip the error dialog about the unsupported format
}
