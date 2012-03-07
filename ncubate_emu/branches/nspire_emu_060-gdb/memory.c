#include <stdio.h>
#include <stdlib.h>
#include "emu.h"
#include "os-win32.h"

u8   (*read_byte_map[64])(u32 addr);
u16  (*read_half_map[64])(u32 addr);
u32  (*read_word_map[64])(u32 addr);
void (*write_byte_map[64])(u32 addr, u8 value);
void (*write_half_map[64])(u32 addr, u16 value);
void (*write_word_map[64])(u32 addr, u32 value);

/* For invalid/unknown physical addresses */
u8 bad_read_byte(u32 addr)               { warn("Bad read_byte: %08x", addr); debugger(); return 0; }
u16 bad_read_half(u32 addr)              { warn("Bad read_half: %08x", addr); debugger(); return 0; }
u32 bad_read_word(u32 addr)              { warn("Bad read_word: %08x", addr); debugger(); return 0; }
void bad_write_byte(u32 addr, u8 value)  { warn("Bad write_byte: %08x %02x",addr,value); debugger(); }
void bad_write_half(u32 addr, u16 value) { warn("Bad write_half: %08x %04x",addr,value); debugger(); }
void bad_write_word(u32 addr, u32 value) { warn("Bad write_word: %08x %08x",addr,value); debugger(); }

u8 *mem_and_flags;
struct mem_area_desc mem_areas[4] = {
	{ 0x00000000, ROM_SIZE,    NULL },
	{ 0x10000000, 0,           NULL },
	{ 0xA4000000, RAM_A4_SIZE, NULL },
};

void *phys_mem_ptr(u32 addr, u32 size) {
	unsigned int i;
	for (i = 0; i < sizeof(mem_areas)/sizeof(*mem_areas); i++) {
		u32 offset = addr - mem_areas[i].base;
		if (offset < mem_areas[i].size && size <= mem_areas[i].size - offset)
			return mem_areas[i].ptr + offset;
	}
	return NULL;
}

u32 phys_mem_addr(void *ptr) {
	int i;
	for (i = 0; i < 3; i++) {
		u32 offset = (u8 *)ptr - mem_areas[i].ptr;
		if (offset < mem_areas[i].size)
			return mem_areas[i].base + offset;
	}
	return -1; // should never happen
}


#define DO_READ_ACTION (RF_READ_BREAKPOINT)
void read_action(void *ptr) {
	u32 addr = phys_mem_addr(ptr);
	printf("Hit read breakpoint at %08x. Entering debugger.\n", addr);
	debugger();
}

#define DO_WRITE_ACTION (RF_WRITE_BREAKPOINT | RF_CODE_TRANSLATED | RF_CODE_NO_TRANSLATE)
void write_action(void *ptr) {
	u32 addr = phys_mem_addr(ptr);
	u32 *flags = &RAM_FLAGS((size_t)ptr & ~3);
	if (*flags & RF_WRITE_BREAKPOINT) {
		printf("Hit write breakpoint at %08x. Entering debugger.\n", addr);
		debugger();
	}
	if (*flags & RF_CODE_TRANSLATED) {
		logprintf(LOG_CPU, "Wrote to translated code at %08x. Deleting translations.\n", addr);
		invalidate_translation(*flags >> RFS_TRANSLATION_INDEX);
	} else {
		*flags &= ~RF_CODE_NO_TRANSLATE;
	}
}

/* 00000000, 10000000, A4000000: ROM and RAM */
u8 memory_read_byte(u32 addr) {
	u8 *ptr = phys_mem_ptr(addr, 1);
	if (!ptr) return bad_read_byte(addr);
	if (RAM_FLAGS((size_t)ptr & ~3) & DO_READ_ACTION) read_action(ptr);
	return *ptr;
}
u16 memory_read_half(u32 addr) {
	u16 *ptr = phys_mem_ptr(addr, 2);
	if (!ptr) return bad_read_half(addr);
	if (RAM_FLAGS((size_t)ptr & ~3) & DO_READ_ACTION) read_action(ptr);
	return *ptr;
}
u32 memory_read_word(u32 addr) {
	u32 *ptr = phys_mem_ptr(addr, 4);
	if (!ptr) return bad_read_word(addr);
	if (RAM_FLAGS(ptr) & DO_READ_ACTION) read_action(ptr);
	return *ptr;
}
void memory_write_byte(u32 addr, u8 value) {
	u8 *ptr = phys_mem_ptr(addr, 1);
	if (!ptr) { bad_write_byte(addr, value); return; }
	u32 flags = RAM_FLAGS((size_t)ptr & ~3);
	if (flags & RF_READ_ONLY) { bad_write_byte(addr, value); return; }
	if (flags & DO_WRITE_ACTION) write_action(ptr);
	*ptr = value;
}
void memory_write_half(u32 addr, u16 value) {
	u16 *ptr = phys_mem_ptr(addr, 2);
	if (!ptr) { bad_write_half(addr, value); return; }
	u32 flags = RAM_FLAGS((size_t)ptr & ~3);
	if (flags & RF_READ_ONLY) { bad_write_half(addr, value); return; }
	if (flags & DO_WRITE_ACTION) write_action(ptr);
	*ptr = value;
}
void memory_write_word(u32 addr, u32 value) {
	u32 *ptr = phys_mem_ptr(addr, 4);
	if (!ptr) { bad_write_word(addr, value); return; }
	u32 flags = RAM_FLAGS(ptr);
	if (flags & RF_READ_ONLY) { bad_write_word(addr, value); return; }
	if (flags & DO_WRITE_ACTION) write_action(ptr);
	*ptr = value;
}

/* 8FFF0000 */
void sdramctl_write_word(u32 addr, u32 value) {
	switch (addr - 0x8FFF0000) {
		case 0x00: return;
		case 0x04: return;
		case 0x08: return;
		case 0x0C: return;
		case 0x10: return;
		case 0x14: return;
	}
	bad_write_word(addr, value);
}

u32 memctl_cx_status;
u32 memctl_cx_config;
u32 memctl_cx_read_word(u32 addr) {
	switch (addr - 0x8FFF0000) {
		case 0x0000: return memctl_cx_status | 0x80;
		case 0x000C: return memctl_cx_config;
		case 0x0FE0: return 0x40;
		case 0x0FE4: return 0x13;
		case 0x0FE8: return 0x14;
		case 0x0FEC: return 0x00;
		case 0x1000: return 0x20; // memc_status (raw interrupt bit set when flash op complete?)
		case 0x1FE0: return 0x51;
		case 0x1FE4: return 0x13;
		case 0x1FE8: return 0x34;
		case 0x1FEC: return 0x00;
	}
	return bad_read_word(addr);
}
void memctl_cx_write_word(u32 addr, u32 value) {
	switch (addr - 0x8FFF0000) {
		case 0x0004:
			switch (value) {
				case 0: memctl_cx_status = 1; return; // go
				case 1: memctl_cx_status = 3; return; // sleep
				case 2: case 3: memctl_cx_status = 2; return; // wakeup, pause
				case 4: memctl_cx_status = 0; return; // configure
			}
			break;
		case 0x0008: return;
		case 0x000C: memctl_cx_config = value; return;
		case 0x0010: return; // refresh_prd
		case 0x0018: return; // t_dqss
		case 0x0028: return; // t_rcd
		case 0x002C: return; // t_rfc
		case 0x0030: return; // t_rp
		case 0x0104: return;
		case 0x0200: return;
		case 0x1008: return; // memc_cfg_set
		case 0x1010: return; // direct_cmd
		case 0x1014: return; // set_cycles
		case 0x1018: return; // set_opmode
		case 0x1204: nand_writable = value & 1; return;
	}
	bad_write_word(addr, value);
}

/* A9000000: SPI */
u32 spi_read_word(u32 addr) {
	switch (addr - 0xA9000000) {
		case 0x0C: return 0;
		case 0x10: return 1;
		case 0x14: return 0;
		case 0x18: return -1;
		case 0x1C: return -1;
		case 0x20: return 0;
	}
	return bad_read_word(addr);
}
void spi_write_word(u32 addr, u32 value) {
	switch (addr - 0xA9000000) {
		case 0x08: return;
		case 0x0C: return;
		case 0x14: return;
		case 0x18: return;
		case 0x1C: return;
		case 0x20: return;
	}
	bad_write_word(addr, value);
}

/* AC000000: SDIO */
u8 sdio_read_byte(u32 addr) {
	switch (addr & 0x3FFFFFF) {
		case 0x29: return -1;
	}
	return bad_read_byte(addr);
}
u16 sdio_read_half(u32 addr) {
	switch (addr & 0x3FFFFFF) {
		case 0x10: return -1;
		case 0x12: return -1;
		case 0x14: return -1;
		case 0x16: return -1;
		case 0x18: return -1;
		case 0x1A: return -1;
		case 0x1C: return -1;
		case 0x1E: return -1;
		case 0x2C: return -1;
		case 0x30: return -1;
	}
	return bad_read_half(addr);
}
u32 sdio_read_word(u32 addr) {
	switch (addr & 0x3FFFFFF) {
		case 0x20: return -1;
	}
	return bad_read_word(addr);
}
void sdio_write_byte(u32 addr, u8 value) {
	switch (addr & 0x3FFFFFF) {
		case 0x29: return;
		case 0x2E: return;
		case 0x2F: return;
	}
	bad_write_byte(addr, value);
}
void sdio_write_half(u32 addr, u16 value) {
	switch (addr & 0x3FFFFFF) {
		case 0x04: return;
		case 0x0C: return;
		case 0x0E: return;
		case 0x2C: return;
		case 0x30: return;
		case 0x32: return;
		case 0x34: return;
		case 0x36: return;
	}
	bad_write_half(addr, value);
}
void sdio_write_word(u32 addr, u32 value) {
	switch (addr & 0x3FFFFFF) {
		case 0x00: return;
		case 0x08: return;
		case 0x20: return;
	}
	bad_write_word(addr, value);
}

/* B8000000 */
u32 sramctl_read_word(u32 addr) {
	switch (addr - 0xB8001000) {
		case 0xFE0: return 0x52;
		case 0xFE4: return 0x13;
		case 0xFE8: return 0x34;
		case 0xFEC: return 0x00;
	}
	return bad_read_word(addr);
}
void sramctl_write_word(u32 addr, u32 value) {
	switch (addr - 0xB8001000) {
		case 0x010: return;
		case 0x014: return;
		case 0x018: return;
	}
	return bad_write_word(addr, value);
}

/* BC000000 */
u32 unknown_BC_read_word(u32 addr) {
	switch (addr & 0x3FFFFFF) {
		case 0xC: return 0;
	}
	return bad_read_word(addr);
}

/* C4000000: ADC (Analog-to-Digital Converter) */
struct {
	u32 int_status;
	u32 int_mask;
	struct adc_channel {
		u32 unknown;
		u32 count;
		u32 address;
		u16 value;
		u16 speed;
	} channel[7];
} adc;
static u16 adc_read_channel(int n) {
	if (pmu.disable2 & 0x10)
		return 0x3FF;

	// Scale for channels 1-2:   155 units = 1 volt
	// Scale for other channels: 310 units = 1 volt
	if (n == 3) {
		// A value from 0 to 20 indicates normal TI-Nspire keypad.
		// A value from 21 to 42 indicates TI-84+ keypad.
		return 10 + (keypad_type * 21);
	} else {
		return 930;
	}
}
u32 adc_read_word(u32 addr) {
	int n;
	if (!(addr & 0x100)) {
		switch (addr & 0xFF) {
			case 0x00: return adc.int_status & adc.int_mask;
			case 0x04: return adc.int_status;
			case 0x08: return adc.int_mask;
		}
	} else if ((n = addr >> 5 & 7) < 7) {
		struct adc_channel *c = &adc.channel[n];
		switch (addr & 0x1F) {
			case 0x00: return 0;
			case 0x04: return c->unknown;
			case 0x08: return c->count;
			case 0x0C: return c->address;
			case 0x10: return c->value;
			case 0x14: return c->speed;
		}
	}
	return bad_read_word(addr);
}
void adc_write_word(u32 addr, u32 value) {
	int n;
	if (!(addr & 0x100)) {
		switch (addr & 0xFF) {
			case 0x04: // Interrupt acknowledge
				adc.int_status &= ~value;
				int_set(INT_ADC, adc.int_status & adc.int_mask);
				return;
			case 0x08: // Interrupt enable
				adc.int_mask = value & 0xFFFFFFF;
				int_set(INT_ADC, adc.int_status & adc.int_mask);
				return;
			case 0x0C:
			case 0x10:
			case 0x14:
				return;
		}
	} else if ((n = addr >> 5 & 7) < 7) {
		struct adc_channel *c = &adc.channel[n];
		switch (addr & 0x1F) {
			case 0x00: // Command register - write 1 to measure voltage and store to +10
				// Other commands do exist, including some
				// that write to memory; not implemented yet.
				c->value = adc_read_channel(n);
				adc.int_status |= 3 << (4 * n);
				int_set(INT_ADC, adc.int_status & adc.int_mask);
				return;
			case 0x04: c->unknown = value & 0xFFFFFFF; return;
			case 0x08: c->count = value & 0x1FFFFFF; return;
			case 0x0C: c->address = value & ~3; return;
			case 0x14: c->speed = value & 0x3FF; return;
		}
	}
	return bad_write_word(addr, value);
}

/* -------------------------------------------------------------------------- */

void __attribute__((fastcall)) read_align_fail(u32 addr) {
	error("Read align fail: %08x", addr);
}
void __attribute__((fastcall)) write_align_fail(u32 addr) {
	error("Write align fail: %08x", addr);
}

u32 __attribute__((fastcall)) mmio_read_byte(u32 addr) {
	return read_byte_map[addr >> 26](addr);
}
u32 __attribute__((fastcall)) mmio_read_half(u32 addr) {
	return read_half_map[addr >> 26](addr);
}
u32 __attribute__((fastcall)) mmio_read_word(u32 addr) {
	return read_word_map[addr >> 26](addr);
}
void __attribute__((fastcall)) mmio_write_byte(u32 addr, u32 value) {
	write_byte_map[addr >> 26](addr, value);
}
void __attribute__((fastcall)) mmio_write_half(u32 addr, u32 value) {
	write_half_map[addr >> 26](addr, value);
}
void __attribute__((fastcall)) mmio_write_word(u32 addr, u32 value) {
	write_word_map[addr >> 26](addr, value);
}

void memory_initialize(u32 sdram_size) {
	mem_and_flags = os_reserve(MEM_MAXSIZE * 2);
	if (!mem_and_flags ||
	    !os_commit(&mem_and_flags[0], ROM_SIZE + RAM_A4_SIZE + sdram_size) ||
	    !os_commit(&mem_and_flags[MEM_MAXSIZE], ROM_SIZE + RAM_A4_SIZE + sdram_size))
	{
		printf("Couldn't allocate memory\n");
		exit(1);
	}
	mem_areas[0].ptr = &mem_and_flags[0];
	mem_areas[1].size = sdram_size;
	mem_areas[1].ptr = &mem_and_flags[ROM_SIZE + RAM_A4_SIZE];
	mem_areas[2].ptr = &mem_and_flags[ROM_SIZE];

	if (product == 0x0D) {
		// Lab cradle OS reads calibration data from F007xxxx,
		// probably a mirror of ROM at 0007xxxx
		mem_areas[3].base = 0xF0000000;
		mem_areas[3].size = ROM_SIZE;
		mem_areas[3].ptr = mem_areas[0].ptr;
	}

	int i;
	for (i = 0; i < 64; i++) {
		/* will fallback to bad_* on non-memory addresses */
		read_byte_map[i] = memory_read_byte;
		read_half_map[i] = memory_read_half;
		read_word_map[i] = memory_read_word;
		write_byte_map[i] = memory_write_byte;
		write_half_map[i] = memory_write_half;
		write_word_map[i] = memory_write_word;
	}

	read_byte_map[0x90 >> 2] = apb_read_byte;
	read_half_map[0x90 >> 2] = apb_read_half;
	read_word_map[0x90 >> 2] = apb_read_word;
	write_byte_map[0x90 >> 2] = apb_write_byte;
	write_half_map[0x90 >> 2] = apb_write_half;
	write_word_map[0x90 >> 2] = apb_write_word;

	read_byte_map[0xAC >> 2] = sdio_read_byte;
	read_half_map[0xAC >> 2] = sdio_read_half;
	read_word_map[0xAC >> 2] = sdio_read_word;
	write_byte_map[0xAC >> 2] = sdio_write_byte;
	write_half_map[0xAC >> 2] = sdio_write_half;
	write_word_map[0xAC >> 2] = sdio_write_word;

	read_byte_map[0xB0 >> 2] = usb_read_byte;
	read_half_map[0xB0 >> 2] = usb_read_half;
	read_word_map[0xB0 >> 2] = usb_read_word;
	write_word_map[0xB0 >> 2] = usb_write_word;

	read_word_map[0xC0 >> 2] = lcd_read_word;
	write_word_map[0xC0 >> 2] = lcd_write_word;

	read_word_map[0xC4 >> 2] = adc_read_word;
	write_word_map[0xC4 >> 2] = adc_write_word;

	read_word_map[0xC8 >> 2] = des_read_word;
	write_word_map[0xC8 >> 2] = des_write_word;

	read_word_map[0xCC >> 2] = sha256_read_word;
	write_word_map[0xCC >> 2] = sha256_write_word;

	if (!emulate_cx) {
		write_word_map[0x8F >> 2] = sdramctl_write_word;

		memcpy(apb_map, apb_map_normal, sizeof apb_map);

		read_word_map[0xA9 >> 2] = spi_read_word;
		write_word_map[0xA9 >> 2] = spi_write_word;

		read_word_map[0xB8 >> 2] = nand_phx_read_word;
		write_word_map[0xB8 >> 2] = nand_phx_write_word;

		read_word_map[0xBC >> 2] = unknown_BC_read_word;

		read_word_map[0xDC >> 2] = int_read_word;
		write_word_map[0xDC >> 2] = int_write_word;
	} else {
		read_byte_map[0x80 >> 2] = nand_cx_read_byte;
		read_word_map[0x80 >> 2] = nand_cx_read_word;
		write_byte_map[0x80 >> 2] = nand_cx_write_byte;
		write_word_map[0x80 >> 2] = nand_cx_write_word;

		read_word_map[0x8F >> 2] = memctl_cx_read_word;
		write_word_map[0x8F >> 2] = memctl_cx_write_word;

		memcpy(apb_map, apb_map_cx, sizeof apb_map);

		read_word_map[0xB4 >> 2] = usb_read_word;

		read_word_map[0xB8 >> 2] = sramctl_read_word;
		write_word_map[0xB8 >> 2] = sramctl_write_word;

		read_word_map[0xDC >> 2] = int_cx_read_word;
		write_word_map[0xDC >> 2] = int_cx_write_word;
	}
}
