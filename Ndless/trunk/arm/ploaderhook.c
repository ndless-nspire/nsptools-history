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
#include "ndless.h"


struct assoc_file_recur_cb_ctx {
	char *prgm_name;
	char *docpath;
	char *arg1;
	int *argc;
};

int assoc_file_recur_cb(const char *path, void *context) {
	struct assoc_file_recur_cb_ctx *ctx = context;
	if (!strcmp(ctx->prgm_name, strrchr(path, '/') + 1)) {
		strncpy(ctx->arg1, ctx->docpath, FILENAME_MAX - 1);
		strncpy(ctx->docpath, path, FILENAME_MAX - 1);
		*ctx->argc = 2;
		return 1;
	}
	return 0;
}

// Try to run a document. Returns non zero if can't run it.
int ld_exec(const char *path) {
	char docpath[FILENAME_MAX];
	int ret;
	unsigned i;
	char arg1[FILENAME_MAX];
	int argc = 1;
	FILE *docfile = NULL;
	strcpy(docpath, path);

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
			struct assoc_file_recur_cb_ctx context = {prgm_name, docpath, arg1, &argc};
			ut_file_recur_each("/documents", assoc_file_recur_cb, &context);
		}
		cfg_close();
	}

	docfile = fopen(docpath, "rb");
	struct stat docstat;
	if (!docfile || (ret = stat(docpath, &docstat))) {
cantopen:
		puts("ld_exec: can't open doc");
		return 1;
	}

	void *docptr = emu_debug_alloc_ptr ? emu_debug_alloc_ptr : malloc(docstat.st_size);
	if (!docptr) {
		puts("ld_exec: can't malloc");
		return 1;
	}
	if (!fread(docptr, docstat.st_size, 1, docfile)) {
		puts("ld_exec: can't read doc");
		if (!emu_debug_alloc_ptr)
			free(docptr);
		return 1;
	}
	fclose(docfile);
	if (strcmp(PRGMSIG, docptr)) { /* not a program */
		if (!emu_debug_alloc_ptr)
			free(docptr);
		return 1;
	}
	int intmask = TCT_Local_Control_Interrupts(-1); /* TODO workaround: disable the interrupts to avoid the clock on the screen */
	wait_no_key_pressed(); // let the user release the Enter key, to avoid being read by the program
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
	wait_no_key_pressed(); // let the user release the key used to exit the program, to avoid being read by the OS
	TCT_Local_Control_Interrupts(intmask);
	if (!emu_debug_alloc_ptr)
		free(docptr);
	return 0;
}

// When opening a document
HOOK_DEFINE(plh_hook) {
	char *halfpath; // [docfolder/]file.tns
	char docpath[FILENAME_MAX];
	halfpath = (char*)(HOOK_SAVED_REGS(plh_hook)[5] /* r5 */ + 32);
	// TODO use snprintf
	sprintf(docpath, "/documents/%s", halfpath);
	if (ld_exec(docpath)) {
		HOOK_SAVED_REGS(plh_hook)[3] = HOOK_SAVED_REGS(plh_hook)[0]; // 'mov r3, r0' was overwritten by the hook
		HOOK_RESTORE_RETURN_SKIP(plh_hook, -0x114, 0); // to the error dialog about the unsupported format (we've overwritten a branch with our hook)
	} else {
		HOOK_RESTORE_RETURN_SKIP(plh_hook, -0xDC, 1); // skip the error dialog about the unsupported format
	}
}

static int startup_file_recur_cb(const char *path, __attribute__((unused)) void *context) {
	ld_exec(path);
	return 0;
}

BOOL plh_isstartup = FALSE;

// Try to run all the documents in the startup folder and its sub-folders
HOOK_DEFINE(plh_startup_hook) {
	if (!isKeyPressed(KEY_NSPIRE_ESC)) {
		plh_isstartup = TRUE;
		ut_file_recur_each("/documents/ndless/startup", startup_file_recur_cb, NULL);
		plh_isstartup = FALSE;
	}
	HOOK_RETURN(plh_startup_hook);
}
