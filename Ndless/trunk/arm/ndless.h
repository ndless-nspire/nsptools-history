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

#include <os.h>

/* ints.h */
extern unsigned ints_scextnum;
extern unsigned *sc_addrs_ptr;
void ints_setup_handlers(void);
#define INTS_UNDEF_INSTR_HANDLER_ADDR 0x24
#define INTS_SWI_HANDLER_ADDR 0x28
#define INTS_PREFETCH_ABORT_HANDLER_ADDR 0x2C
#define INTS_DATA_ABORT_HANDLER_ADDR 0x30

/* utils.c */
// 'NEXT'
#define NEXT_SIGNATURE 0x4E455854
/* N-ext is a convention for TI-Nspire extensions such as Ndless.
 * Only one N-ext-based extension can be installed at a time.
 * The N-ext descriptor is referenced before the SWI handler.
 * It gives defines the currently installed extension to programs, emulators, ...
 * and ourself for uninstallation. */
struct next_descriptor {
	unsigned next_version;
	char ext_name[4];
	unsigned ext_version;
};
extern struct next_descriptor ut_next_descriptor;
extern unsigned ut_os_version_index;
void ut_read_os_version_index(void);
void __attribute__ ((noreturn)) ut_os_reboot(void);
void __attribute__ ((noreturn)) ut_calc_reboot(void);
void __attribute__ ((noreturn)) ut_panic(const char * msg);
void ut_debug_trace(unsigned line);
void ut_puts(const char *str);
void ut_printf(const char *fmt, ...);
static inline struct next_descriptor *ut_get_next_descriptor(void) {
	if (*(*(unsigned**)(OS_BASE_ADDRESS + INTS_SWI_HANDLER_ADDR) - 2) != NEXT_SIGNATURE)
		return NULL;
	return (struct next_descriptor*)(*(unsigned**)(OS_BASE_ADDRESS + INTS_SWI_HANDLER_ADDR) - 1);
}

/* syscalls.c */
void sc_ext_relocdatab(unsigned *dataptr, unsigned size, void *base);
void sc_setup(void);

#endif

