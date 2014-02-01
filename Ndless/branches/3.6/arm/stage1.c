/****************************************************************************
 * Stage 1 of the installation.
 * Set a hook and hot reboot the OS to get it back to a stable state.
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
 * The Initial Developer of the Original Code is Fabian Vogt.
 * Portions created by the Initial Developer are Copyright (C) 2013-2014
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): Excale, Olivier ARMAND <olivier.calc@gmail.com>.
 *                 
 ****************************************************************************/

#include "ndless.h"
#include <stdint.h>

// As expected by the patch headers generated by MakeHotRebootPtch
#define PATCH_SETW(A,B)   *(uint32_t *)(A) = (B)
#define PATCH_SETZ(A,B,C) memset32((void *)(A), (C), (B)-(A))

static void memset32(uint32_t *p, uint32_t value, size_t num) {
	num = num/4;
	while(num--) {
		*(p++) = value;
	}
}

static void write_i2c(uint8_t client, uint8_t addr, uint8_t value) {
	PATCH_SETW(0x9005006c, 0); //Disable I2C
	PATCH_SETW(0x90050004, client); //Set target address
	PATCH_SETW(0x9005006c, 1); //Enable I2C
		
	volatile uint32_t *status = (uint32_t*) 0x90050070;
	
	PATCH_SETW(0x90050010, addr);
	while(*status & 1); //Wait until transmitted
	PATCH_SETW(0x90050010, value);
	while(*status & 1); //Wait until transmitted
}

static void write_touchpad(uint16_t port, uint8_t value) {
	write_i2c(0x20, 0xFF, port >> 8);
	write_i2c(0x20, port & 0xFF, value);
}

// OS-specific
static unsigned const ndless_inst_resident_hook_addrs[] = {0x10012598, 0x1001251C, 0x100123BC, 0x10012370};

// Install the resident part
HOOK_DEFINE(s1_startup_hook) {
	ut_read_os_version_index();
	ints_setup_handlers();
	struct stat res_stat;
	const char *res_path = NDLESS_DIR "/ndless_resources.tns";
	FILE *res_file = fopen(res_path, "rb");
	if (!res_file) {
		//show_msgbox("Ndless", "You have forgotten to transfer 'ndless_resources'. Ndless won't be installed.");
		goto s1_startup_hook_return;
	}
	stat(res_path, &res_stat);
	char *core = malloc(res_stat.st_size);
	fread(core, res_stat.st_size, 1, res_file);
	fclose(res_file);
	char *res_params = NULL; // Dummy filename to tell the installer we are booting or installing
	clear_cache();
	((void (*)(int argc, void* argv))(char*)core + sizeof(PRGMSIG))(1, &res_params); // Run the core installation
s1_startup_hook_return:
	HOOK_RESTORE_RETURN(s1_startup_hook);
}

int main(void) {
	ut_disable_watchdog();
	ut_read_os_version_index();
	ints_setup_handlers();
	ut_disable_watchdog();
		
	// Disable all interrupts
	if (ut_os_version_index < 2) {
			PATCH_SETW(0xDC00000C, 0xFFFFFFFF);
	} else {
	PATCH_SETW(0xDC000014, 0xFFFFFFFF);
	}
	if (ut_os_version_index < 2) {
		uint32_t RX;
		__asm volatile (
			"mrs %0, cpsr      \n"
			"bic %0, %0, #0x80 \n"
			"msr cpsr_c, %0    \n"
		: "=r" (RX));
	}
	if (ut_os_version_index < 2) {
			uint32_t dummyint = *((volatile uint32_t *)(0xDC000028));
			(void)dummyint; // unused warning
	}
	
	// Reset IRQ flags
	if (ut_os_version_index > 1) {
			PATCH_SETW(0x90010008, 0);
			PATCH_SETW(0x90010008, 0);
			PATCH_SETW(0x9001000C, 1);
			PATCH_SETW(0x90010028, 0);
			PATCH_SETW(0x9001002C, 1);
			PATCH_SETW(0x900C0008, 0);
			PATCH_SETW(0x900C000C, 1);
			PATCH_SETW(0x900C0028, 0);
			PATCH_SETW(0x900C002C, 1);
			PATCH_SETW(0x900D0008, 0);
			PATCH_SETW(0x900D000C, 1);
			PATCH_SETW(0x900D0028, 0);
			PATCH_SETW(0x900D002c, 1);
	}
		
	//Reset USB
	volatile int z;
	PATCH_SETW(0x900B0018, *(volatile uint32_t*)0x900B0018 & 0b11111111111111111111111110011111);
	for (z = 0; z <= 0x10000; z++)
		;
	uint32_t usb_cmd = *((volatile uint32_t *)(0xB0000140));
	usb_cmd &= ~1;
	PATCH_SETW(0xB0000140, usb_cmd);
	PATCH_SETW(0xB4000140, usb_cmd);
	for (z = 0; z <= 0x10000; z++)
		;
	usb_cmd = *((volatile uint32_t *)(0xB0000140));
	usb_cmd |= 0x2;
	PATCH_SETW(0xB0000140, usb_cmd);
	while ( ((*((volatile uint32_t *)(0xB0000140)))&0x2) != 0x0 )
			;
	PATCH_SETW(0xB00001A4, 0x003C1120);
	
	if (ut_os_version_index > 1) {
		// Reset touchpad
		write_touchpad(0x0004, 0x01);
		// Disable I2C IRQ
		PATCH_SETW(0x90050030, 0);
		// Disable I2C
		PATCH_SETW(0x9005006C, 0);
	}
	// Disable RTC IRQ
	PATCH_SETW(0x9009000C, 1);

	// Disable keypad and touchpad IRQs
	PATCH_SETW(0x900E000C, 0);
	PATCH_SETW(0x900E0040, 0);

	// Reset OS global variables to their initial values
	// Reset internal RAM state, else instable without USB plugged-in
	switch (ut_os_version_index) {
		case 0:
				#include "hrpatches-os-ncas-3.6.0.h"
		break;
		case 1:
				#include "hrpatches-os-cas-3.6.0.h"
		 break;
		case 2:
				#include "hrpatches-os-ncascx-3.6.0.h"
		break;
		case 3:
				#include "hrpatches-os-cascx-3.6.0.h"
		break;
	}
	
	// disable the OS monitor thread that would throw a discrepancy error and wipe out the OS
	// this thread use signature data passed by the boot2 and copied to the first OS variable at the beginning of the BSS
	// this signature data may have been overwritten (and is always on classic TI-Nspire after opening the Lua installer)
	// OS-specific
	if (ut_os_version_index < 2) {
		static unsigned const os_monitor_thread_addrs[] = {0x10135DF4, 0x10136418};
		PATCH_SETW(os_monitor_thread_addrs[ut_os_version_index], 0xE12FFF1E); // "bx lr" at the beginning of the thread
	}
		
	// post hot-reboot hook
	HOOK_INSTALL(ndless_inst_resident_hook_addrs[ut_os_version_index], s1_startup_hook);
	
	clear_cache();
	((void(*)(void))0x10000000)(); // Hot-reboot the OS
	__builtin_unreachable();
	return 0;
}
