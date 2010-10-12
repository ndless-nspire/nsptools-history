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

#ifndef _NDLS_LIGHT
// OS-specific
extern unsigned syscalls_ncas_1_7[];
extern unsigned syscalls_cas_1_7[];

/* Ndless extensions exposed as syscalls. See os.h for documentation. */

int sc_nl_osvalue(const int *values, unsigned size) {
	if (ut_os_version_index >= size)
		return 0;
	return values[ut_os_version_index];
}

/* Our lightweight relocation support unfortunately cannot handle 
 * initializers with relocation (for example arrays of function pointers).
 * data.rel and data.rel.ro sections are created, but may contain both
 * non-relocable and relocable data, for which we have no clue.
 * This function allows to relocate an array of pointers. */
void sc_ext_relocdatab(unsigned *dataptr, unsigned size, void *base) {
	unsigned i;
	for (i = size; i > 0; i--) {
		*dataptr++ += (unsigned)base;
	}
}

/* Extension syscalls table */
unsigned sc_ext_table[] = {
	(unsigned)sc_nl_osvalue, (unsigned)sc_ext_relocdatab
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
	sc_ext_relocdatab(sc_ext_table, __SYSCALLS_LASTEXT + 1, &__base);
	sc_ext_relocdatab(emu_sysc_table, __SYSCALLS_LASTEMU + 1, &__base);
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

#endif // ndef _NDLS_LIGHT
