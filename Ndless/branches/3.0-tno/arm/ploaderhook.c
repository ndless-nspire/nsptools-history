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
 * Portions created by the Initial Developer are Copyright (C) 2010-2011
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 *                 Geoffrey ANNEHEIM <geoffrey.anneheim@gmail.com>
 ****************************************************************************/

#include <os.h>
#include "ndless.h"

// When opening a document
HOOK_DEFINE(plh_hook) {
	char *halfpath; // [docfolder/]file.tns
	char docpath[100];
	int ret;
	unsigned i;
	halfpath = (char*)(HOOK_SAVED_REGS(plh_hook)[5] /* r5 */ + 32);
	// TODO use snprintf
	sprintf(docpath, "/documents/%s", halfpath);
	struct stat docstat;
	ret = stat(docpath, &docstat);
	FILE *docfile = fopen(docpath, "rb");
	if (!docfile || ret) {
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
	((void (*)(int argc, char *argv[]))(docptr + sizeof(PRGMSIG)))(1, (char*[]){docpath, NULL}); /* run the program */
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
