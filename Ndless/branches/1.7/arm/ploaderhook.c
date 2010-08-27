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

// When opening a document
HOOK_DEFINE(plh_hook) {
	char *docfolder, *filename;
	char docpath[100];
	int ret;
	asm(
		" mov %0, r0 \n"
		" mov %1, r1"
		: "=r"(docfolder), "=r"(filename));
	// TODO move the 3 following lines to an OS startup hook
	ut_read_os_version_index();
	sc_setup();	
	ints_setup_handlers();
	// TODO use snprintf
	sprintf(docpath, "/documents/%s/%s", docfolder, filename);
	struct stat docstat;
	ret = stat(docpath, &docstat);
	FILE *docfile = fopen(docpath, "rb");
	if (!docfile || !ret) {
		puts("ploaderhook: can't open doc");
		HOOK_RESTORE_RETURN(plh_hook);
	}
	void *docptr = malloc(docstat.st_size);
	if (!docptr) {
		puts("ploaderhook: can't malloc");
		HOOK_RESTORE_RETURN(plh_hook);
	}
	if (!fread(docptr, docstat.st_size, 1, docfile)) {
		puts("ploaderhook: can't read doc");
		free(docptr);
	}
	fclose(docfile);
	free(docptr);
	HOOK_RESTORE(plh_hook);
	asm volatile(" mov pc, lr");
	//HOOK_RETURN(plh_hook);
}
