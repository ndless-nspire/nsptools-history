/****************************************************************************
 * Ndless syscalls setup and handlers
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

#ifndef _SYSCALLS_LIGHT
// OS-specific
extern unsigned syscalls_ncas_1_7[];
extern unsigned syscalls_cas_1_7[];

/* Ndless extensions exposed as syscalls */
int sc_nl_osvalue(const int *values, unsigned size) {
	// TODO doc. Returns 0. Which OS version.
	if (ut_os_version_index >= size)
		return 0;
	return values[ut_os_version_index];
}

unsigned sc_ext_table[] = {
	(unsigned)sc_nl_osvalue, (unsigned)sc_nl_osvalue
};

void sc_setup(void) {
	switch (ut_os_version_index) {
		// OS-specific
		case 0:
			sc_addrs_ptr = syscalls_ncas_1_7;
			break;
		case 1:
			sc_addrs_ptr = syscalls_cas_1_7;
			break;
	}
	ut_reloc_reldata(sc_ext_table, sizeof(sc_ext_table)/sizeof(unsigned));
}

#else
// OS-specific
extern unsigned syscalls_light_ncas_1_7[];
extern unsigned syscalls_light_cas_1_7[];

/* Used by the loader: switch to light syscall array */
void sc_setup(void) {
	switch (ut_os_version_index) {
		// OS-specific
		case 0:
			sc_addrs_ptr = syscalls_light_ncas_1_7;
			break;
		case 1:
			sc_addrs_ptr = syscalls_light_cas_1_7;
			break;
	}
}

#endif // ndef _SYSCALLS_LIGHT
