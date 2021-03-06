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
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 *                 Geoffrey ANNEHEIM <geoffrey.anneheim@gmail.com>
 ****************************************************************************/

#include <os.h>
#include "ndless.h"

// Marker at the beginning of a program
#define PRGMSIG "PRG"

// When opening a document
HOOK_DEFINE(plh_hook) {
	char *halfpath; // docfolder/file.tns
	char docpath[100];
	int ret;
	halfpath =  (char*)(HOOK_SAVED_REGS(plh_hook)[11] /* r11 */ - 0x124); // on the stack
	// the hook is called at installation time. Show the installation message.
	if (!strcmp("ndless_installer.tns", strrchr(halfpath, '/') + 1)) {
		char title[20];
		char msg[150];
	  ascii2utf16(title, "Ndless", sizeof(title));
	  ascii2utf16(msg, "Ndless installed successfully!", sizeof(msg));
	  show_dialog_box2(0, title, msg);
		goto silent; // skip the error dialog
	}
	// Asynchronous uninstallation
	if (ins_lowmem_hook_installed) {
		HOOK_UNINSTALL(ins_lowmem_hook_addrs[ut_os_version_index], ins_lowmem_hook);
		ins_lowmem_hook_installed = FALSE;
	}
	// TODO use snprintf
	sprintf(docpath, "/documents/%s", halfpath);
	struct stat docstat;
	ret = stat(docpath, &docstat);
	FILE *docfile = fopen(docpath, "rb");
	if (!docfile || ret) {
		puts("ploaderhook: can't open doc");
		HOOK_RESTORE_RETURN(plh_hook);
	}
	void *docptr = emu_debug_alloc_ptr ? emu_debug_alloc_ptr : malloc(docstat.st_size);
	if (!docptr) {
		puts("ploaderhook: can't malloc");
		HOOK_RESTORE_RETURN(plh_hook);
	}
	if (!fread(docptr, docstat.st_size, 1, docfile)) {
		puts("ploaderhook: can't read doc");
		if (!emu_debug_alloc_ptr)
			free(docptr);
		HOOK_RESTORE_RETURN(plh_hook);
	}
	fclose(docfile);
	if (strcmp(PRGMSIG, docptr)) { /* not a program */
		if (!emu_debug_alloc_ptr)
			free(docptr);
		HOOK_RESTORE_RETURN(plh_hook);
	}
	int intmask = TCT_Local_Control_Interrupts(-1); /* TODO workaround: disable the interrupts to avoid the clock on the screen */
	((void (*)(int argc, char *argv[]))(docptr + sizeof(PRGMSIG)))(1, (char*[]){docpath, NULL}); /* run the program */
	TCT_Local_Control_Interrupts(intmask);
	if (!emu_debug_alloc_ptr)
		free(docptr);
silent:
	HOOK_RESTORE_RETURN_SKIP(plh_hook, 4); // skip the error dialog about the unrecognized format
}
