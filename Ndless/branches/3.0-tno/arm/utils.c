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
 * Portions created by the Initial Developer are Copyright (C) 2010-2011
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
extern unsigned syscalls_ncas_3_0_1[];
extern unsigned syscalls_light_ncas_3_0_1[];
extern unsigned syscalls_cas_3_0_1[];
extern unsigned syscalls_light_cas_3_0_1[];
extern unsigned syscalls_ncascx_3_0_1[];
extern unsigned syscalls_light_ncascx_3_0_1[];
extern unsigned syscalls_cascx_3_0_1[];
extern unsigned syscalls_light_cascx_3_0_1[];
extern unsigned syscalls_ncas_3_0_2[];
extern unsigned syscalls_light_ncas_3_0_2[];
extern unsigned syscalls_cas_3_0_2[];
extern unsigned syscalls_light_cas_3_0_2[];
extern unsigned syscalls_ncascx_3_0_2[];
extern unsigned syscalls_light_ncascx_3_0_2[];
extern unsigned syscalls_cascx_3_0_2[];
extern unsigned syscalls_light_cascx_3_0_2[];
extern unsigned syscalls_ncas_3_1_0[];
extern unsigned syscalls_light_ncas_3_1_0[];
extern unsigned syscalls_cas_3_1_0[];
extern unsigned syscalls_light_cas_3_1_0[];
extern unsigned syscalls_ncascx_3_1_0[];
extern unsigned syscalls_light_ncascx_3_1_0[];
extern unsigned syscalls_cascx_3_1_0[];
extern unsigned syscalls_light_cascx_3_1_0[];

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
		case 0x102ED240:  // 3.0.1 non-CAS
			ut_os_version_index = 0;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_ncas_,OS_VERSION);
#else
			sc_addrs_ptr = syscalls_ncas_3_0_1;
#endif
			break;
		case 0x102ED960:  // 3.0.1 CAS
			ut_os_version_index = 1;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_cas_,OS_VERSION);
#else
			sc_addrs_ptr = syscalls_cas_3_0_1;
#endif
			break;
		case 0x102ECCD0:  // 3.0.1 non-CAS CX
			ut_os_version_index = 2;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_ncascx_,OS_VERSION);
#else
			sc_addrs_ptr = syscalls_ncascx_3_0_1;
#endif
			break;
		case 0x102ED420:  // 3.0.1 CAS CX
			ut_os_version_index = 3;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_cascx_,OS_VERSION);
#else
			sc_addrs_ptr = syscalls_cascx_3_0_1;
#endif
			break;
		case 0x102ED6D0:  // 3.0.2 non-CAS
			ut_os_version_index = 4;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_ncas_,OS_VERSION);
#else
			sc_addrs_ptr = syscalls_ncas_3_0_2;
#endif
			break;
		case 0x102EDFE4:  // 3.0.2 CAS
			ut_os_version_index = 5;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_cas_,OS_VERSION);
#else
			sc_addrs_ptr = syscalls_cas_3_0_2;
#endif
			break;
		case 0x102ED170:  // 3.0.2 non-CAS CX
			ut_os_version_index = 6;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_ncascx_,OS_VERSION);
#else
			sc_addrs_ptr = syscalls_ncascx_3_0_2;
#endif
			break;
		case 0x102ED8C0:  // 3.0.2 CAS CX
			ut_os_version_index = 7;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_cascx_,OS_VERSION);
#else
			sc_addrs_ptr = syscalls_cascx_3_0_2;
#endif
			break;
		case 0x102F0FA0:  // 3.1.0 non-CAS
			ut_os_version_index = 8;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_ncas_,OS_VERSION);
#else
			sc_addrs_ptr = syscalls_ncas_3_1_0;
#endif
			break;
		case 0x102F16D0:  // 3.1.0 CAS
			ut_os_version_index = 9;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_cas_,OS_VERSION);
#else
			sc_addrs_ptr = syscalls_cas_3_1_0;
#endif
			break;
		case 0x102F0A10:  // 3.1.0 non-CAS CX
			ut_os_version_index = 10;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_cascx_,OS_VERSION);
#else
			sc_addrs_ptr = syscalls_ncascx_3_1_0;
#endif
			break;
		case 0x102F11A0:  // 3.1.0 CAS CX
			ut_os_version_index = 11;
#if defined STAGE1
		sc_addrs_ptr = CONCAT(syscalls_light_cascx_,OS_VERSION);
#else
			sc_addrs_ptr = syscalls_cascx_3_1_0;
#endif
			break;
#ifndef STAGE1
		default:
			ut_panic("v?");
#endif
	}
}
void __attribute__ ((noreturn)) ut_calc_reboot(void) {
	*(unsigned*)0x900A0008 = 2; //CPU reset
	__builtin_unreachable();
}

void __attribute__ ((noreturn)) ut_panic(const char *msg) {
	puts(msg);
	ut_calc_reboot();
}

/* draw a dotted line. Line 0 is at the bottom of the screen (to avoid overwriting the installer) */
void ut_debug_trace(unsigned line) {
	volatile unsigned *ptr = (unsigned*)((char*)SCREEN_BASE_ADDRESS + (SCREEN_WIDTH/2) * (SCREEN_HEIGHT - 1 - line));
	unsigned i;
	for (i = 0; i < (SCREEN_WIDTH/2) / 4; i++)
		*ptr++ = line & 1 ? 0xFFFF0000 : 0x0000FFFF;
}
