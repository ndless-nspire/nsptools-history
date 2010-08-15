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
		printf("Wrote to translated code at %08x. Deleting translations.\n", addr);
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

/* 8C000000 */
void unknown_8C_write_word(u32 addr, u32 value) {
	switch (addr) {
		case 0x8FFF0000: return;
		case 0x8FFF0004: return;
		case 0x8FFF0008: return;
		case 0x8FFF000C: return;
		case 0x8FFF0010: return;
		case 0x8FFF0014: return;
	}
	bad_write_word(addr, value);
}

/* A8000000 - newly used in OS 2.1 */
u32 unknown_A8_read_word(u32 addr) {
	switch (addr - 0xA9000000) {
		case 0x10: return -1; // returning 0 hangs
		case 0x1C: return 0;
		case 0x20: return 0;
	}
	return bad_read_word(addr);
}
void unknown_A8_write_word(u32 addr, u32 value) {
	switch (addr - 0xA9000000) {
		case 0x0C: return;
		case 0x1C: return;
		case 0x20: return;
	}
	bad_write_word(addr, value);
}

/* AC000000 */
u8 unknown_AC_read_byte(u32 addr) {
	switch (addr & 0x3FFFFFF) {
		case 0x29: return 0;
	}
	return bad_read_byte(addr);
}

/* B0000000: USB */
u8 usb_read_byte(u32 addr) {
	if (addr == 0xB0000100) return 0x40; // operational registers start at +40
	return bad_read_byte(addr);
}
u16 usb_read_half(u32 addr) {
	if (addr == 0xB0000102) return 0x0100; // EHCI 1.0
	return bad_read_half(addr);
}
u32 usb_read_word(u32 addr) {
	switch (addr & 0x3FFFFFF) {
		case 0x00C: return 0;
		case 0x100: return 0x01000040; // EHCI 1.0, operational registers start at +40
		case 0x104: return 0x00010011; // Port indicator control, port power control, 1 port
		case 0x108: return 6;
		case 0x10C: return 0;
		case 0x124: return 0;
		case 0x140: return 0;
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
	switch (addr & 0x3FFFFFF) {
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

/* C4000000: ADC (Analog-to-Digital Converter?) */
static u32 adc_int_active;
static u32 adc_int_enable;

static u32 adc_read_word(u32 addr) {
	if (!(addr & 0x100)) {
		switch (addr & 0x3FFFFFF) {
			case 0x00:
				return adc_int_active;
			case 0x08:
				return adc_int_enable;
		}
	} else {
		int n = addr >> 5 & 7;
		switch (addr & 0x3FFFE1F) {
			case 0x00:
				return 0;
			case 0x10:
				if (n == 3) {
					// A value from 0 to 20 indicates normal TI-Nspire keypad.
					// A value from 21 to 42 indicates TI-84+ keypad.
					return 10 + (keypad_type * 21);
				} else {
					// Not sure what these should actually be, but
					// this gets rid of low battery warning.
					return 5000; 
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
					int_deactivate(1 << INT_ADC);
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
					int_activate(1 << INT_ADC);
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

/* DC000000: Interrupt controller */
u32 active_ints;
u32 current_ints[2];
u32 enabled_ints[2];

static u32 int_read_word(u32 addr) {
	int is_fiq = addr >> 8 & 1;
	int i;
	switch (addr & 0x3FFFFFF) {
		case 0x000: case 0x100: return current_ints[is_fiq];
		case 0x004: case 0x104: return active_ints;
		case 0x008: case 0x108:
		case 0x00C: case 0x10C: return enabled_ints[is_fiq];
		case 0x020: case 0x120: return 0;
		case 0x024: case 0x124: // Current interrupt type
			for (i = 0; i < 32; i++)
				if (current_ints[addr >> 8 & 1] & (1 << i))
					return i;
			error("No interrupt");
		case 0x028: case 0x128: return 0;
		case 0x200: return 0;
		case 0x204: return 0;
		case 0x208: return 0;
	}
	return bad_read_word(addr);
}
static void int_write_word(u32 addr, u32 value) {
	int is_fiq = addr >> 8 & 1;
	switch (addr & 0x3FFFFFF) {
		case 0x004: case 0x104: // Acknowledge?
			current_ints[is_fiq] &= (~value | (active_ints & enabled_ints[is_fiq]));
			cpu_int_check();
			return;
		case 0x008: case 0x108: // Enable
			enabled_ints[is_fiq] |= value;
			current_ints[is_fiq] |= (active_ints & enabled_ints[is_fiq]);
			cpu_int_check();
			return;
		case 0x00C: case 0x10C: // Disable
			enabled_ints[is_fiq] &= ~value;
			// Should this also remove them from the current set?
			return;
		case 0x02C: case 0x12C: return;
		case 0x200: return;
		case 0x204: return;
		case 0x208: return;
		case 0x304: return;
		case 0x30C: return;
		case 0x31C: return;
		case 0x320: return;
		case 0x324: return;
		case 0x32C: return;
		case 0x338: return;
		case 0x340: return;
		case 0x348: return;
		case 0x34C: return;
		case 0x358: return;
	}
	return bad_write_word(addr, value);
}

void int_activate(u32 int_mask) {
	active_ints |= int_mask;
	current_ints[0] |= int_mask & enabled_ints[0];
	current_ints[1] |= int_mask & enabled_ints[1];
	cpu_int_check();
}

void int_deactivate(u32 int_mask) {
	active_ints &= ~int_mask;
	// Should this also remove them from the current set?
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
		read_byte_map[i] = bad_read_byte;
		read_half_map[i] = bad_read_half;
		read_word_map[i] = bad_read_word;
		write_byte_map[i] = bad_write_byte;
		write_half_map[i] = bad_write_half;
		write_word_map[i] = bad_write_word;
	}

	read_byte_map[0x00 >> 2] = memory_read_byte;
	read_half_map[0x00 >> 2] = memory_read_half;
	read_word_map[0x00 >> 2] = memory_read_word;
	write_byte_map[0x00 >> 2] = memory_write_byte;
	write_half_map[0x00 >> 2] = memory_write_half;
	write_word_map[0x00 >> 2] = memory_write_word;

	read_byte_map[0x10 >> 2] = memory_read_byte;
	read_half_map[0x10 >> 2] = memory_read_half;
	read_word_map[0x10 >> 2] = memory_read_word;
	write_byte_map[0x10 >> 2] = memory_write_byte;
	write_half_map[0x10 >> 2] = memory_write_half;
	write_word_map[0x10 >> 2] = memory_write_word;

	read_byte_map[0x18 >> 2] = memory_read_byte;
	read_half_map[0x18 >> 2] = memory_read_half;
	read_word_map[0x18 >> 2] = memory_read_word;
	write_byte_map[0x18 >> 2] = memory_write_byte;
	write_half_map[0x18 >> 2] = memory_write_half;
	write_word_map[0x18 >> 2] = memory_write_word;

	write_word_map[0x8C >> 2] = unknown_8C_write_word;

	read_byte_map[0x90 >> 2] = apb_read_byte;
	read_half_map[0x90 >> 2] = apb_read_half;
	read_word_map[0x90 >> 2] = apb_read_word;
	write_byte_map[0x90 >> 2] = apb_write_byte;
	write_half_map[0x90 >> 2] = apb_write_half;
	write_word_map[0x90 >> 2] = apb_write_word;

	read_byte_map[0xA4 >> 2] = memory_read_byte;
	read_half_map[0xA4 >> 2] = memory_read_half;
	read_word_map[0xA4 >> 2] = memory_read_word;
	write_byte_map[0xA4 >> 2] = memory_write_byte;
	write_half_map[0xA4 >> 2] = memory_write_half;
	write_word_map[0xA4 >> 2] = memory_write_word;

	read_word_map[0xA8 >> 2] = unknown_A8_read_word;
	write_word_map[0xA8 >> 2] = unknown_A8_write_word;

	read_byte_map[0xAC >> 2] = unknown_AC_read_byte;

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
	u32 active_ints;
	u32 current_ints[2];
	u32 enabled_ints[2];
};

void *memory_save_state(size_t *size) {
	*size = sizeof(struct memory_saved_state);
	struct memory_saved_state *state = malloc(*size);
	memcpy(&state->mem_and_flags, mem_and_flags, sizeof(mem_and_flags));
	state->adc_int_active = adc_int_active;
	state->adc_int_enable = adc_int_enable;
	state->active_ints = active_ints;
	memcpy(&state->current_ints, current_ints, sizeof(current_ints));
	memcpy(&state->enabled_ints, enabled_ints, sizeof(enabled_ints));
	return state;
}

void memory_reload_state(void *state) {
	struct memory_saved_state *_state = (struct memory_saved_state *)state;
	memcpy(mem_and_flags, &_state->mem_and_flags, sizeof(mem_and_flags));
	adc_int_active = _state->adc_int_active;
	adc_int_enable = _state->adc_int_enable;
	active_ints = _state->active_ints;
	memcpy(current_ints, &_state->current_ints, sizeof(current_ints));
	memcpy(enabled_ints, &_state->enabled_ints, sizeof(enabled_ints));
}
