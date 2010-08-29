/****************************************************************************
 * Definitions
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

#ifndef _NDLESS_H_
#define _NDLESS_H_

#include <OS.h>

/* ints.h */
extern unsigned *sc_addrs_ptr;
void ints_setup_handlers(void);

/* utils.c */
extern unsigned ut_os_version_index;
void ut_read_os_version_index(void);
void __attribute__ ((noreturn)) ut_os_reboot(void);
void __attribute__ ((noreturn)) ut_calc_reboot(void);
void __attribute__ ((noreturn)) ut_panic(const char * msg);

/* syscalls.c */
void sc_setup(void);

#endif
