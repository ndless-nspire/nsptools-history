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
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 *                 Geoffrey ANNEHEIM <geoffrey.anneheim@gmail.com>
 ****************************************************************************/

#include "ndless.h"

unsigned ut_os_version_index;

/* Writes to ut_os_version_index a zero-based index identifying the OS version and HW model.
 * Should be called only once.
 * May be used for OS-specific arrays of constants (marked with "// OS-specific"). */
void ut_read_os_version_index(void) {
	/* The heuristic is based on the address of INT_Initialize - Thanks Goplat.
	 * The address is read from the RAM copy and not the real vector which is
	 * destroyed at installation time */
	switch (*(unsigned*)0x10000020) {
		// OS-specific
		case 0x10211290: ut_os_version_index = 0; break; // 1.7 non-CAS
		case 0x102132A0: ut_os_version_index = 1; break; // 1.7 CAS
		default:
			ut_os_version_index = 0xFFFFFFFF;
			ut_panic("unknown OS version");		
	}
}

// addresses of OS global variables which must be reinitialized for proper OS reboot
static unsigned const ut_os_reboot_reset_addrs[][3] = {
	{0x106DAFC4, 0x106F2A0C, 0x106F2AF0},  // 1.7 non-CAS
	{0, 0, 0} // 1.7 CAS TODO
};

void __attribute__ ((noreturn)) ut_os_reboot(void) {
	unsigned i;
	if (ut_os_version_index == 0xFFFFFFFF)
		ut_calc_reboot();
	for (i = 0; i < sizeof(ut_os_reboot_reset_addrs[0])/sizeof(unsigned); i++)
		*(unsigned*)(ut_os_reboot_reset_addrs[ut_os_version_index][i]) = 1;
	goto *OS_BASE_ADDRESS;
}

void __attribute__ ((noreturn)) ut_calc_reboot(void) {
	*(unsigned*)0x900A0008 = 2; //CPU reset
	while(1);
}

void __attribute__ ((noreturn)) ut_panic(const char * msg) {
	puts(msg);
	ut_os_reboot();
}
