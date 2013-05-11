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
 * Portions created by the Initial Developer are Copyright (C) 2010-2013
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 ****************************************************************************/

#include <os.h>
#include "ndless.h"

#ifndef _NDLS_LIGHT
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

unsigned sc_nl_hwtype(void) {
	return ut_os_version_index >= 2; // 1 if CX
}

BOOL sc_nl_isstartup(void) {
	return plh_isstartup;
}

unsigned sc_nl_ndless_rev(void) {
	return NDLESS_REVISION;
}

void sc_nl_no_scr_redraw(void) {
	plh_noscrredraw = TRUE;
}

/* Extension syscalls table */
/* Caution, these ones cannot call themselves other syscalls, because of the non-reentrant swi handler */
unsigned sc_ext_table[] = {
	(unsigned)sc_nl_osvalue, (unsigned)sc_ext_relocdatab, (unsigned)sc_nl_hwtype, (unsigned)sc_nl_isstartup,
	(unsigned)luaext_getstate, (unsigned)ld_set_resident, (unsigned)sc_nl_ndless_rev, (unsigned)sc_nl_no_scr_redraw,
	(unsigned)ins_loaded_by_3rd_party_loader,
};

void sc_setup(void) {
	sc_ext_relocdatab(sc_ext_table, __SYSCALLS_LASTEXT + 1, &__base);
	sc_ext_relocdatab(emu_sysc_table, __SYSCALLS_LASTEMU + 1, &__base);
}

#endif // ndef _NDLS_LIGHT
