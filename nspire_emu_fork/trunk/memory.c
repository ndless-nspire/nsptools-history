#include <stdio.h>
#include "emu.h"

u8   (*read_byte_map[64])(u32 addr);
u16  (*read_half_map[64])(u32 addr);
u32  (*read_word_map[64])(u32 addr);
void (*write_byte_map[64])(u32 addr, u8 value);
void (*write_half_map[64])(u32 addr, u16 value);
void (*write_word_map[64])(u32 addr, u32 value);

/* For invalid addresses */
u8 bad_read_byte(u32 addr)               { warn("Bad read_byte: %08x", addr); debugger(); return 0; }
u16 bad_read_half(u32 addr)              { warn("Bad read_half: %08x", addr); debugger(); return 0; }
u32 bad_read_word(u32 addr)              { warn("Bad read_word: %08x", addr); debugger(); return 0; }
void bad_write_byte(u32 addr, u8 value)  { warn("Bad write_byte: %08x %02x",addr,value); debugger(); }
void bad_write_half(u32 addr, u16 value) { warn("Bad write_half: %08x %04x",addr,value); debugger(); }
void bad_write_word(u32 addr, u32 value) { warn("Bad write_word: %08x %08x",addr,value); debugger(); }

const struct ram_area_desc ram_areas[] = {
	{ 0x00000000, ROM_SIZE,    rom_00 },
	{ 0x10000000, RAM_10_SIZE, ram_10 },
	{ 0xA4000000, RAM_A4_SIZE, ram_A4 },
};

void *ram_ptr(u32 addr, u32 size) {
	int i;
	for (i = 0; i < 3; i++) {
		u32 offset = addr - ram_areas[i].base;
		if (offset < ram_areas[i].size && size <= ram_areas[i].size - offset)
			return ram_areas[i].ptr + offset;
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

#define RAM_FUNCS(name, base, size) \
u8 name##_read_byte(u32 addr) { \
	if (addr < base + size) { \
		u8 *ptr = &name[addr - base]; \
		if (RAM_FLAGS((size_t)ptr & ~3) & DO_READ_ACTION) read_action(addr); \
		return *(u8 *)ptr; \
	} \
	return bad_read_byte(addr); \
} \
u16 name##_read_half(u32 addr) { \
	if (addr < base + size) { \
		u16 *ptr = (u16 *)&name[addr - base]; \
		if (RAM_FLAGS((size_t)ptr & ~3) & DO_READ_ACTION) read_action(addr); \
		return *(u16 *)ptr; \
	} \
	return bad_read_half(addr); \
} \
u32 name##_read_word(u32 addr) { \
	if (addr < base + size) { \
		u32 *ptr = (u32 *)&name[addr - base]; \
		if (RAM_FLAGS(ptr) & DO_READ_ACTION) read_action(addr); \
		return *ptr; \
	} \
	return bad_read_word(addr); \
} \
void name##_write_byte(u32 addr, u8 value) { \
	if (addr < base + size) { \
		u8 *ptr = &name[addr - base]; \
		u32 *flags = &RAM_FLAGS((size_t)ptr & ~3); \
		if (*flags & DO_WRITE_ACTION) write_action(flags, addr); \
		*ptr = value; \
	} else \
		return bad_write_byte(addr, value); \
} \
void name##_write_half(u32 addr, u16 value) { \
	if (addr < base + size) { \
		u16 *ptr = (u16 *)&name[addr - base]; \
		u32 *flags = &RAM_FLAGS((size_t)ptr & ~3); \
		if (*flags & DO_WRITE_ACTION) write_action(flags, addr); \
		*ptr = value; \
	} else \
		return bad_write_half(addr, value); \
} \
void name##_write_word(u32 addr, u32 value) { \
	if (addr < base + size) { \
		u32 *ptr = (u32 *)&name[addr - base]; \
		u32 *flags = &RAM_FLAGS(ptr); \
		if (*flags & DO_WRITE_ACTION) write_action(flags, addr); \
		*ptr = value; \
	} else \
		return bad_write_word(addr, value); \
}

RAM_FUNCS(ram_10, 0x10000000, RAM_10_SIZE)
RAM_FUNCS(ram_18, 0x18000000, RAM_18_SIZE)
RAM_FUNCS(ram_A4, 0xA4000000, RAM_A4_SIZE)

/* 00000000 */
u8 rom_read_byte(u32 addr) {
	if (arm.reg[15] >= ROM_SIZE)
		warn("Null pointer access (addr=%08x)", addr);
	if (addr < ROM_SIZE) {
		u8 *ptr = &rom_00[addr];
		if (RAM_FLAGS((size_t)ptr & ~3) & DO_READ_ACTION) read_action(addr);
		return *ptr;
	}
	return bad_read_byte(addr);
}
u16 rom_read_half(u32 addr) {
	if (arm.reg[15] >= ROM_SIZE)
		warn("Null pointer access (addr=%08x)", addr);
	if (addr < ROM_SIZE) {
		u16 *ptr = (u16 *)&rom_00[addr];
		if (RAM_FLAGS((size_t)ptr & ~3) & DO_READ_ACTION) read_action(addr);
		return *ptr;
	}
	return bad_read_half(addr);
}
u32 rom_read_word(u32 addr) {
	if (arm.reg[15] >= ROM_SIZE)
		warn("Null pointer access (addr=%08x)", addr);
	if (addr < ROM_SIZE) {
		u32 *ptr = (u32 *)&rom_00[addr];
		if (RAM_FLAGS(ptr) & DO_READ_ACTION) read_action(addr);
		return *ptr;
	}
	return bad_read_word(addr);
}
void rom_write_byte(u32 addr, u8 value) {
	warn("Null pointer access (addr=%08x, value=%02x)", addr, value);
}
void rom_write_half(u32 addr, u16 value) {
	warn("Null pointer access (addr=%08x, value=%04x)", addr, value);
}
void rom_write_word(u32 addr, u32 value) {
	warn("Null pointer access (addr=%08x, value=%08x)", addr, value);
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

/* AC000000 */
u8 unknown_AC_read_byte(u32 addr) {
	switch (addr & 0x3FFFFFF) {
		case 0x29: return 0;
	}
	return bad_read_byte(addr);
}

/* B0000000: USB */
u32 usb_read_word(u32 addr) {
	//printf("usb read %08x\n", addr);
	switch (addr & 0x3FFFFFF) {
		case 0x100: return 0;
		case 0x104: return 0;
		case 0x108: return 0;
		case 0x124: return 0;
		case 0x144: return 0;
		case 0x148: return 0;
		// returning 0 causes OS to detect a "probe" (?)
		// and hang trying to communicate with it
		case 0x164: return -1;
		case 0x170: return 0;
		case 0x174: return 0;
		case 0x180: return 0;
		case 0x1A4: return 0;
	}
	return bad_read_word(addr);
}
void usb_write_word(u32 addr, u32 value) {
	//printf("usb write %08x %08x\n", addr, value);
	switch (addr & 0x3FFFFFF) {
		case 0x100: return;
		case 0x104: return;
		case 0x108: return;
		case 0x110: return;
		case 0x118: return;
		case 0x140: return;
		case 0x144: return;
		case 0x148: return;
		case 0x164: return;
		case 0x168: return;
		case 0x16C: return;
		case 0x174: return;
		case 0x180: return;
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

/* C0000000: Display */
u8 *lcd_framebuffer = RAM_PTR(0xA4000100);
u32 lcd_read_word(u32 addr) {
	switch (addr & 0x3FFFFFF) {
		case 0x01C:
		case 0x200: case 0x204: case 0x208: case 0x20C:
		case 0x210: case 0x214: case 0x218: case 0x21C:
			return 0;
	}
	return bad_read_word(addr);
}
static void lcd_write_word(u32 addr, u32 value) {
	switch (addr & 0x3FFFFFF) {
		case 0x000: case 0x004: case 0x008: case 0x00C:
			return;
		case 0x010:
			lcd_framebuffer = ram_ptr(value, 0x9600);
			if (lcd_framebuffer == NULL)
				error("Bad LCD framebuffer pointer\n");
			return;
		case 0x014: case 0x018: case 0x01C:
			return;
		case 0x200: case 0x204: case 0x208: case 0x20C:
		case 0x210: case 0x214: case 0x218: case 0x21C:
			return;
	}
	return bad_write_word(addr, value);
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
					return emulate_ti84_keypad ? 31 : 10;
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
					irq_deactivate(1 << IRQ_ADC);
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
					irq_activate(1 << IRQ_ADC);
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
u32 current_irqs;
u32 active_irqs;
u32 enabled_irqs;

static u32 irq_read_word(u32 addr) {
	int i;
	switch (addr & 0x3FFFFFF) {
		case 0x008: return enabled_irqs;
		case 0x020: return 0;
		case 0x024: // Current IRQ type
			for (i = 0; i < 32; i++)
				if (current_irqs & (1 << i))
					return i;
			error("No IRQ active");
		case 0x028: return 0;
		case 0x108: return 0;
		case 0x200: return 0;
		case 0x204: return 0;
		case 0x208: return 0;
	}
	return bad_read_word(addr);
}
static void irq_write_word(u32 addr, u32 value) {
	switch (addr & 0x3FFFFFF) {
		case 0x004: // Acknowledge?
			current_irqs &= (~value | (active_irqs & enabled_irqs));
			cpu_irq_check();
			return;
		case 0x008: // Enable
			enabled_irqs |= value;
			current_irqs |= (active_irqs & enabled_irqs);
			cpu_irq_check();
			return;
		case 0x00C: // Disable
			enabled_irqs &= ~value;
			// Should this also remove them from the current set?
			return;
		case 0x02C: return;
		case 0x108: return;
		case 0x10C: return;
		case 0x200: return;
		case 0x204: return;
		case 0x208: return;
		case 0x304: return;
		case 0x31C: return;
		case 0x320: return;
		case 0x32C: return;
		case 0x338: return;
		case 0x348: return;
		case 0x34C: return;
		case 0x358: return;
	}
	return bad_write_word(addr, value);
}

void irq_activate(u32 irq_mask) {
	active_irqs |= irq_mask;
	current_irqs |= irq_mask & enabled_irqs;
	cpu_irq_check();
}

void irq_deactivate(u32 irq_mask) {
	active_irqs &= ~irq_mask;
	// Should this also remove them from the current set?
}

/* -------------------------------------------------------------------------- */

u32 __attribute__((fastcall)) slow_read_byte(u32 addr) {
	return read_byte_map[addr >> 26](addr);
}

u32 __attribute__((fastcall)) slow_read_half(u32 addr) {
	if (addr & 1) error("Unaligned read_half: %x", addr);
	return read_half_map[addr >> 26](addr);
}

u32 __attribute__((fastcall)) slow_read_word(u32 addr) {
	if (addr & 3) error("Unaligned read_word: %x", addr);
	return read_word_map[addr >> 26](addr);
}

void __attribute__((fastcall)) slow_write_byte(u32 addr, u32 value) {
	write_byte_map[addr >> 26](addr, value);
}

void __attribute__((fastcall)) slow_write_half(u32 addr, u32 value) {
	if (addr & 1) error("Unaligned write_half: %x", addr);
	write_half_map[addr >> 26](addr, value);
}

void __attribute__((fastcall)) slow_write_word(u32 addr, u32 value) {
	if (addr & 3) error("Unaligned write_word: %x", addr);
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

	read_byte_map[0x00 >> 2] = rom_read_byte;
	read_half_map[0x00 >> 2] = rom_read_half;
	read_word_map[0x00 >> 2] = rom_read_word;
	write_byte_map[0x00 >> 2] = rom_write_byte;
	write_half_map[0x00 >> 2] = rom_write_half;
	write_word_map[0x00 >> 2] = rom_write_word;

	read_byte_map[0x10 >> 2] = ram_10_read_byte;
	read_half_map[0x10 >> 2] = ram_10_read_half;
	read_word_map[0x10 >> 2] = ram_10_read_word;
	write_byte_map[0x10 >> 2] = ram_10_write_byte;
	write_half_map[0x10 >> 2] = ram_10_write_half;
	write_word_map[0x10 >> 2] = ram_10_write_word;

	read_byte_map[0x18 >> 2] = ram_18_read_byte;
	read_half_map[0x18 >> 2] = ram_18_read_half;
	read_word_map[0x18 >> 2] = ram_18_read_word;
	write_byte_map[0x18 >> 2] = ram_18_write_byte;
	write_half_map[0x18 >> 2] = ram_18_write_half;
	write_word_map[0x18 >> 2] = ram_18_write_word;

	write_word_map[0x8C >> 2] = unknown_8C_write_word;

	read_byte_map[0x90 >> 2] = apb_read_byte;
	read_half_map[0x90 >> 2] = apb_read_half;
	read_word_map[0x90 >> 2] = apb_read_word;
	write_byte_map[0x90 >> 2] = apb_write_byte;
	write_half_map[0x90 >> 2] = apb_write_half;
	write_word_map[0x90 >> 2] = apb_write_word;

	read_byte_map[0xA4 >> 2] = ram_A4_read_byte;
	read_half_map[0xA4 >> 2] = ram_A4_read_half;
	read_word_map[0xA4 >> 2] = ram_A4_read_word;
	write_byte_map[0xA4 >> 2] = ram_A4_write_byte;
	write_half_map[0xA4 >> 2] = ram_A4_write_half;
	write_word_map[0xA4 >> 2] = ram_A4_write_word;

	read_byte_map[0xAC >> 2] = unknown_AC_read_byte;

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

	read_word_map[0xDC >> 2] = irq_read_word;
	write_word_map[0xDC >> 2] = irq_write_word;
}
