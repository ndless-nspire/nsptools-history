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

#ifndef STAGE1
struct next_descriptor ut_next_descriptor = {
	.next_version = 0x00010000,
	.ext_name = "NDLS",
	.ext_version = 0x00010007 // will be incremented only if new functionnalities exposed to third-party tools
};
#endif

unsigned ut_os_version_index;

// OS-specific
extern unsigned syscalls_light_ncas_1_7[];
extern unsigned syscalls_light_cas_1_7[];
extern unsigned syscalls_ncas_1_7[];
extern unsigned syscalls_cas_1_7[];
extern unsigned syscalls_light_ncas_2_0_1[];
extern unsigned syscalls_light_cas_2_0_1[];
extern unsigned syscalls_ncas_2_0_1[];
extern unsigned syscalls_cas_2_0_1[];
extern unsigned syscalls_light_ncas_2_1_0[];
extern unsigned syscalls_light_cas_2_1_0[];
extern unsigned syscalls_ncas_2_1_0[];
extern unsigned syscalls_cas_2_1_0[];

/* Writes to ut_os_version_index a zero-based index identifying the OS version and HW model.
 * Also sets up the syscalls table.
 * Should be called only once.
 * May be used for OS-specific arrays of constants (marked with "// OS-specific"). */
void ut_read_os_version_index(void) {
	/* The heuristic is based on the address of INT_Initialize - Thanks Goplat.
	 * The address is read from the RAM copy and not the real vector which is
	 * destroyed at installation time */
	switch (*(unsigned*)(OS_BASE_ADDRESS + 0x20)) {
		// OS-specific
		case 0x10211290:  // 1.7 non-CAS
			ut_os_version_index = 0;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_ncas_,OS_VERSION);
#elif defined STAGE2
			sc_addrs_ptr = syscalls_light_ncas_1_7;
#else 
			sc_addrs_ptr = syscalls_ncas_1_7;
#endif
			break;
		case 0x102132A0:  // 1.7 CAS
			ut_os_version_index = 1;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_cas_,OS_VERSION);
#elif defined STAGE2
			sc_addrs_ptr = syscalls_light_cas_1_7;
#else
			sc_addrs_ptr = syscalls_cas_1_7;
#endif
			break;
		case 0x10266030:  // 2.0.1 non-CAS
			ut_os_version_index = 2;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_ncas_,OS_VERSION);
#elif defined STAGE2
			sc_addrs_ptr = syscalls_light_ncas_2_0_1;
#else
			sc_addrs_ptr = syscalls_ncas_2_0_1;
#endif
			break;
		case 0x10266900:  // 2.0.1 CAS
			ut_os_version_index = 3;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_cas_,OS_VERSION);
#elif defined STAGE2
			sc_addrs_ptr = syscalls_light_cas_2_0_1;
#else
			sc_addrs_ptr = syscalls_cas_2_0_1;
#endif
			break;
		case 0x10279D70:  // 2.1.0 non-CAS
			ut_os_version_index = 4;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_ncas_,OS_VERSION);
#elif defined STAGE2
			sc_addrs_ptr = syscalls_light_ncas_2_1_0;
#else
			sc_addrs_ptr = syscalls_ncas_2_1_0;
#endif
			break;
		case 0x1027A640:  // 2.1.0 CAS
			ut_os_version_index = 5;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_cas_,OS_VERSION);
#elif defined STAGE2
			sc_addrs_ptr = syscalls_light_cas_2_1_0;
#else
			sc_addrs_ptr = syscalls_cas_2_1_0;
#endif
			break;
#ifndef STAGE1
		default:
			ut_panic("v?");
#endif
	}
}

/* OS-specific: addresses of the name of the directory containing the document
 * being opened. Prefixed with '/documents/' on OS 2.0 and higher.
 * Caution, special characters such as '.' are filtered out of the name.
 * Found at development time with a full memory search thanks to Ncubate's "ss" command. */
unsigned const ut_currentdocdir_addr[] = {0x10669A9C, 0x1069BD64, 0x1088F164, 0x10857154, 0x109A2B74, 0x10966B74};

void __attribute__ ((noreturn)) ut_calc_reboot(void) {
	*(unsigned*)0x900A0008 = 2; //CPU reset
	__builtin_unreachable();
}

#ifndef STAGE1
void __attribute__ ((noreturn)) ut_panic(const char *msg) {
	puts(msg);
	ut_calc_reboot();
}
#endif

#if !defined(_NDLS_LIGHT)
/* draw a dotted line. Line 0 is at the bottom of the screen (to avoid overwriting the installer) */
void ut_debug_trace(unsigned line) {
	volatile unsigned *ptr = (unsigned*)((char*)SCREEN_BASE_ADDRESS + (SCREEN_WIDTH/2) * (SCREEN_HEIGHT - 1 - line));
	unsigned i;
	for (i = 0; i < (SCREEN_WIDTH/2) / 4; i++)
		*ptr++ = line & 1 ? 0xFFFF0000 : 0x0000FFFF;
}
#endif
