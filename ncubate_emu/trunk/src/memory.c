#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emu.h"

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

u8 mem_and_flags[MEM_SIZE * 2];
const struct mem_area_desc mem_areas[] = {
	{ 0x00000000, ROM_SIZE,    rom_00 },
	{ 0x10000000, RAM_10_SIZE, ram_10 },
	{ 0xA4000000, RAM_A4_SIZE, ram_A4 },
};

void *phys_mem_ptr(u32 addr, u32 size) {
	int i;
	for (i = 0; i < 3; i++) {
		u32 offset = addr - mem_areas[i].base;
		if (offset < mem_areas[i].size && size <= mem_areas[i].size - offset)
			return mem_areas[i].ptr + offset;
	}
	return NULL;
}

void *virt_mem_ptr(u32 addr, u32 size) {
	return phys_mem_ptr(mmu_translate(addr, NULL), size);
}

#define DO_READ_ACTION (RF_READ_BREAKPOINT)
void read_action(u32 addr) {
	printf("Hit read breakpoint at %08x. Entering debugger.\n", addr);
	debugger();
}

#define DO_WRITE_ACTION (RF_WRITE_BREAKPOINT | RF_CODE_TRANSLATED | RF_CODE_NO_TRANSLATE)
void write_action(u32 *flags, u32 addr) {
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
	if (RAM_FLAGS((size_t)ptr & ~3) & DO_READ_ACTION) read_action(addr);
	return *ptr;
}
u16 memory_read_half(u32 addr) {
	u16 *ptr = phys_mem_ptr(addr, 2);
	if (!ptr) return bad_read_half(addr);
	if (RAM_FLAGS((size_t)ptr & ~3) & DO_READ_ACTION) read_action(addr);
	return *ptr;
}
u32 memory_read_word(u32 addr) {
	u32 *ptr = phys_mem_ptr(addr, 4);
	if (!ptr) return bad_read_word(addr);
	if (RAM_FLAGS(ptr) & DO_READ_ACTION) read_action(addr);
	return *ptr;
}
void memory_write_byte(u32 addr, u8 value) {
	u8 *ptr = phys_mem_ptr(addr, 1);
	if (!ptr) { bad_write_byte(addr, value); return; }
	u32 *flags = &RAM_FLAGS((size_t)ptr & ~3);
	if (*flags & RF_READ_ONLY) { bad_write_byte(addr, value); return; }
	if (*flags & DO_WRITE_ACTION) write_action(flags, addr);
	*ptr = value;
}
void memory_write_half(u32 addr, u16 value) {
	u16 *ptr = phys_mem_ptr(addr, 2);
	if (!ptr) { bad_write_half(addr, value); return; }
	u32 *flags = &RAM_FLAGS((size_t)ptr & ~3);
	if (*flags & RF_READ_ONLY) { bad_write_half(addr, value); return; }
	if (*flags & DO_WRITE_ACTION) write_action(flags, addr);
	*ptr = value;
}
void memory_write_word(u32 addr, u32 value) {
	u32 *ptr = phys_mem_ptr(addr, 4);
	if (!ptr) { bad_write_word(addr, value); return; }
	u32 *flags = &RAM_FLAGS(ptr);
	if (*flags & RF_READ_ONLY) { bad_write_word(addr, value); return; }
	if (*flags & DO_WRITE_ACTION) write_action(flags, addr);
	*ptr = value;
}

/* 8FFF0000 */
void unknown_8C_write_word(u32 addr, u32 value) {
	switch (addr - 0x8FFF0000 ) {
		case 0x00: return;
		case 0x04: return;
		case 0x08: return;
		case 0x0C: return;
		case 0x10: return;
		case 0x14: return;
	}
	bad_write_word(addr, value);
}

/* A9000000: SPI - newly used in OS 2.1 */
u32 spi_read_word(u32 addr) {
	switch (addr - 0xA9000000) {
		case 0x10: return -1; // returning 0 hangs
		case 0x1C: return 0;
		case 0x20: return 0;
	}
	return bad_read_word(addr);
}
void spi_write_word(u32 addr, u32 value) {
	switch (addr - 0xA9000000) {
		case 0x0C: return;
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

/* B0000000 (and B4000000?): USB */
u8 usb_read_byte(u32 addr) {
	if ((addr & 0x1FF) == 0x100) return 0x40; // operational registers start at +40
	return bad_read_byte(addr);
}
u16 usb_read_half(u32 addr) {
	if ((addr & 0x1FF) == 0x102) return 0x0100; // EHCI 1.0
	return bad_read_half(addr);
}
u32 usb_read_word(u32 addr) {
	switch (addr & 0x1FF) {
		case 0x00C: return 0;
		case 0x100: return 0x01000040; // EHCI 1.0, operational registers start at +40
		case 0x104: return 0x00010011; // Port indicator control, port power control, 1 port
		case 0x108: return 6;
		case 0x10C: return 0;
		case 0x124: return 0;
		case 0x140: return 0;
		case 0x148: return 0;
		case 0x164: return 0;
		case 0x180: return 0;
		case 0x184: return 0;
		case 0x188: return 0;
		// returning 0 causes OS 1.1 to detect a "probe" (?)
		// and hang trying to communicate with it
		case 0x1A4: return -1;
		case 0x1B0: return 0;
		case 0x1B4: return 0;
		case 0x1C0: return 0;
	}
	return bad_read_word(addr);
}
void usb_write_word(u32 addr, u32 value) {
	switch (addr & 0x1FF) {
		case 0x080: return; // used by diags
		case 0x084: return; // used by diags
		case 0x140: return;
		case 0x144: return;
		case 0x148: return;
		case 0x150: return;
		case 0x158: return;
		case 0x180: return;
		case 0x184: return;
		case 0x188: return;
		case 0x1A4: return;
		case 0x1A8: return;
		case 0x1AC: return;
		case 0x1B4: return;
		case 0x1C0: return;
	}
	bad_write_word(addr, value);
}

/* BC000000 */
u32 unknown_BC_read_word(u32 addr) {
	switch (addr & 0x3FFFFFF) {
		case 0xC: return 0;
	}
	return bad_read_word(addr);
}

/* C4000000: ADC (Analog-to-Digital Converter) */
static u32 adc_int_active;
static u32 adc_int_enable;

static u32 adc_read_word(u32 addr) {
	if (!(addr & 0x100)) {
		switch (addr & 0x3FFFFFF) {
			case 0x00: case 0x04:
				return adc_int_active;
			case 0x08:
				return adc_int_enable;
		}
	} else {
		int channel = addr >> 5 & 7;
		switch (addr & 0x3FFFE1F) {
			case 0x00:
				return 0;
			case 0x10:
				// Scale for channels 1-2:   155 units = 1 volt
				// Scale for other channels: 310 units = 1 volt
				if (channel == 3) {
					// A value from 0 to 20 indicates normal TI-Nspire keypad.
					// A value from 21 to 42 indicates TI-84+ keypad.
					return 10 + (keypad_type * 21);
				} else {
					return 930;
				}
		}
	}
	return bad_read_word(addr);
}
static void adc_write_word(u32 addr, u32 value) {
	if (!(addr & 0x100)) {
		switch (addr & 0x3FFFFFF) {
			case 0x04: // Acknowledge interrupt?
				adc_int_active &= ~value;
				if (adc_int_active == 0)
					int_set(INT_ADC, 0);
				return;
			case 0x08: // Interrupt enable?
				adc_int_enable = value;
				return;
			case 0x10: return;
			case 0x14: return;
		}
	} else {
		int n = addr >> 5 & 7;
		switch (addr & 0x3FFFE1F) {
			case 0x00:
				if (value & 1) {
					// start a measurement, I guess
					adc_int_active |= 2 << (n << 2);
					int_set(INT_ADC, 1);
				}
				return;
			case 0x04:
			case 0x08:
			case 0x0C:
			case 0x14:
				return;
		}
	}
	return bad_write_word(addr, value);
}

/* -------------------------------------------------------------------------- */

u32 __attribute__((fastcall)) slow_read_byte(u32 addr) {
	addr = mmu_translate(addr, data_abort);
	return read_byte_map[addr >> 26](addr);
}

u32 __attribute__((fastcall)) slow_read_half(u32 addr) {
	if (addr & 1) error("Unaligned read_half: %x", addr);
	addr = mmu_translate(addr, data_abort);
	return read_half_map[addr >> 26](addr);
}

u32 __attribute__((fastcall)) slow_read_word(u32 addr) {
	if (addr & 3) error("Unaligned read_word: %x", addr);
	addr = mmu_translate(addr, data_abort);
	return read_word_map[addr >> 26](addr);
}

void __attribute__((fastcall)) slow_write_byte(u32 addr, u32 value) {
	addr = mmu_translate(addr, data_abort);
	write_byte_map[addr >> 26](addr, value);
}

void __attribute__((fastcall)) slow_write_half(u32 addr, u32 value) {
	if (addr & 1) error("Unaligned write_half: %x", addr);
	addr = mmu_translate(addr, data_abort);
	write_half_map[addr >> 26](addr, value);
}

void __attribute__((fastcall)) slow_write_word(u32 addr, u32 value) {
	if (addr & 3) error("Unaligned write_word: %x", addr);
	addr = mmu_translate(addr, data_abort);
	write_word_map[addr >> 26](addr, value);
}

void memory_initialize() {
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

	write_word_map[0x8F >> 2] = unknown_8C_write_word;

	read_byte_map[0x90 >> 2] = apb_read_byte;
	read_half_map[0x90 >> 2] = apb_read_half;
	read_word_map[0x90 >> 2] = apb_read_word;
	write_byte_map[0x90 >> 2] = apb_write_byte;
	write_half_map[0x90 >> 2] = apb_write_half;
	write_word_map[0x90 >> 2] = apb_write_word;

	read_word_map[0xA9 >> 2] = spi_read_word;
	write_word_map[0xA9 >> 2] = spi_write_word;

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

	read_word_map[0xB8 >> 2] = nand_flash_read_word;
	write_word_map[0xB8 >> 2] = nand_flash_write_word;

	read_word_map[0xBC >> 2] = unknown_BC_read_word;

	read_word_map[0xC0 >> 2] = lcd_read_word;
	write_word_map[0xC0 >> 2] = lcd_write_word;

	read_word_map[0xC4 >> 2] = adc_read_word;
	write_word_map[0xC4 >> 2] = adc_write_word;

	read_word_map[0xCC >> 2] = sha256_read_word;
	write_word_map[0xCC >> 2] = sha256_write_word;

	read_word_map[0xDC >> 2] = int_read_word;
	write_word_map[0xDC >> 2] = int_write_word;
}

struct memory_saved_state {
	u8 mem_and_flags[MEM_SIZE * 2];
	u32 adc_int_active;
	u32 adc_int_enable;
};

void *memory_save_state(size_t *size) {
	*size = sizeof(struct memory_saved_state);
	struct memory_saved_state *state = malloc(*size);
	memcpy(&state->mem_and_flags, mem_and_flags, sizeof(mem_and_flags));
	state->adc_int_active = adc_int_active;
	state->adc_int_enable = adc_int_enable;
	return state;
}

void memory_reload_state(void *state) {
	struct memory_saved_state *_state = (struct memory_saved_state *)state;
	memcpy(mem_and_flags, &_state->mem_and_flags, sizeof(mem_and_flags));
	adc_int_active = _state->adc_int_active;
	adc_int_enable = _state->adc_int_enable;
}
