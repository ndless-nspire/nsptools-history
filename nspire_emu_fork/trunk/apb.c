#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emu.h"

/* The APB (Advanced Peripheral Bus) hosts peripherals that do not require
 * high bandwidth. The bridge to the APB is accessed via addresses 900xxxxx. */

/* 90000000 */
u8 unknown_9000_read_byte(u32 addr) {
	switch (addr & 0xFFFF) {
		case 0x10: case 0x14: case 0x1C: case 0x20: case 0x24:
		case 0x50: case 0x64:
		case 0x90: case 0x94: case 0x9C: case 0xA0:
		case 0xD0: case 0xD4: case 0xDC: case 0xE0: case 0xE4:
			return 0;
		case 0x58:
			return 1;
	}
	return bad_read_byte(addr);
}
u16 unknown_9000_read_half(u32 addr) {
	return bad_read_half(addr);
}
u32 unknown_9000_read_word(u32 addr) {
	return unknown_9000_read_byte(addr);
}
void unknown_9000_write_byte(u32 addr, u8 value) {
	switch (addr & 0xFFFF) {
		case 0x04: case 0x08: case 0x0C: case 0x10: case 0x14: case 0x1C: case 0x20: case 0x24:
		case 0x44: case 0x48: case 0x4C: case 0x50: case 0x54: case 0x5C: case 0x60: case 0x64:
		case 0x84: case 0x88: case 0x8C: case 0x90: case 0x94: case 0x9C: case 0xA0: case 0xA4:
		case 0xC4: case 0xC8: case 0xCC: case 0xD0: case 0xD4: case 0xDC: case 0xE0: case 0xE4:
			return;
	}
	bad_write_byte(addr, value);
}
void unknown_9000_write_half(u32 addr, u16 value) {
	bad_write_half(addr, value);
}
void unknown_9000_write_word(u32 addr, u32 value) {
	// Both byte and word accesses are used... for the same registers even
	unknown_9000_write_byte(addr, value);
}

/* 90020000 */
u8 serial_rx_char;
int serial_rx_ready;
int serial_tx_ready;
u8 serial_DLL;
u8 serial_DLM;
u8 serial_IER;
u8 serial_LCR;

void serial_byte_in(u8 byte) {
	serial_rx_char = byte;
	serial_rx_ready = 1;
	irq_activate(1 << IRQ_SERIAL);
}

static inline void serial_int_check() {
	if (!(serial_rx_ready || serial_tx_ready))
		irq_deactivate(1 << IRQ_SERIAL);
}

u8 serial_read_byte(u32 addr) {
	switch (addr & 0xFFFF) {
		case 0x00:
			if (serial_LCR & 0x80)
				return serial_DLL; /* Divisor Latch LSB */
			if (!serial_rx_ready)
				error("Attempted to read empty RBR");
			serial_rx_ready = 0;
			serial_int_check();
			return serial_rx_char;
		case 0x04:
			if (serial_LCR & 0x80)
				return serial_DLM; /* Divisor Latch MSB */
			return serial_IER; /* Interrupt Enable Register */
		case 0x08: /* Interrupt Identification Register */
			if (serial_rx_ready) {
				return 4;
			} else if (serial_tx_ready) {
				serial_tx_ready = 0;
				serial_int_check();
				return 2;
			} else {
				return 1;
			}
		case 0x0C: /* Line Control Register */
			return serial_LCR;
		case 0x14: /* Line Status Register */
			return 0x60 | serial_rx_ready;
	}
	return bad_read_byte(addr);
}
u16 serial_read_half(u32 addr) {
	return bad_read_half(addr);
}
u32 serial_read_word(u32 addr) {
	return serial_read_byte(addr);
}
void serial_write_byte(u32 addr, u8 value) {
	switch (addr & 0xFFFF) {
		case 0x00:
			if (serial_LCR & 0x80) {
				serial_DLL = value;
			} else {
				putchar(value);
				serial_tx_ready = 1;
				irq_activate(1 << IRQ_SERIAL);
			}
			return;
		case 0x04:
			if (serial_LCR & 0x80) {
				serial_DLM = value;
			} else {
				if (value & 0xF0)
					error("Serial IER = %02X\n", value);
				serial_IER = value;
				logprintf(LOG_IO, "Serial: write IER = %02X\n", value);
			}
			return;
		case 0x08: /* FIFO Control Register */
			logprintf(LOG_IO, "Serial: write FCR = %02X\n", value);
			if (value & ~0x06)
				error("Serial FIFOs not implemented");
			return;
		case 0x0C: /* Line Control Register */
			serial_LCR = value;
			return;
		case 0x10: /* Modem Control Register */
			logprintf(LOG_IO, "Serial: write MCR = %02X\n", value);
			return;
	}
	bad_write_byte(addr, value);
}
void serial_write_half(u32 addr, u16 value) {
	bad_write_half(addr, value);
}
void serial_write_word(u32 addr, u32 value) {
	bad_write_word(addr, value);
}

/* 90060000 */
u8 unknown_9006_read_byte(u32 addr) {
	return bad_read_byte(addr);
}
u16 unknown_9006_read_half(u32 addr) {
	return bad_read_half(addr);
}
u32 unknown_9006_read_word(u32 addr) {
	return bad_read_word(addr);
}
void unknown_9006_write_byte(u32 addr, u8 value) {
	bad_write_byte(addr, value);
}
void unknown_9006_write_half(u32 addr, u16 value) {
	bad_write_half(addr, value);
}
void unknown_9006_write_word(u32 addr, u32 value) {
	switch (addr & 0xFFFF) {
		case 0x000: return;
		case 0x008: return;
		case 0xC00: return;
	}
	bad_write_word(addr, value);
}

/* 90080000 */
u8 unknown_9008_read_byte(u32 addr) {
	return bad_read_byte(addr);
}
u16 unknown_9008_read_half(u32 addr) {
	return bad_read_half(addr);
}
u32 unknown_9008_read_word(u32 addr) {
	return bad_read_word(addr);
}
void unknown_9008_write_byte(u32 addr, u8 value) {
	bad_write_byte(addr, value);
}
void unknown_9008_write_half(u32 addr, u16 value) {
	bad_write_half(addr, value);
}
void unknown_9008_write_word(u32 addr, u32 value) {
	switch (addr & 0xFFFF) {
		case 0x8: return;
		case 0xC: return;
		case 0x10: return;
		case 0x14: return;
		case 0x18: return;
		case 0x1C: return;
	}
	bad_write_word(addr, value);
}

/* 90090000 */
u8 rtc_read_byte(u32 addr) {
	return bad_read_byte(addr);
}
u16 rtc_read_half(u32 addr) {
	return bad_read_half(addr);
}
u32 rtc_read_word(u32 addr) {
	switch (addr & 0xFFFF) {
		case 0x00: {
			static time_t time_diff;
			if (!time_diff) {
				struct tm epoch = { .tm_mday = 1, .tm_year = 97 };
				time_diff = mktime(&epoch);
			}
			return time(NULL) - time_diff;
		}
		case 0x14: return 0;
	}
	return bad_read_word(addr);
}
void rtc_write_byte(u32 addr, u8 value) {
	bad_write_byte(addr, value);
}
void rtc_write_half(u32 addr, u16 value) {
	bad_write_half(addr, value);
}
void rtc_write_word(u32 addr, u32 value) {
	switch (addr & 0xFFFF) {
		case 0x04: return;
		case 0x08: return;
		case 0x0C: return;
		case 0x10: return;
	}
	bad_write_word(addr, value);
}

/* 900A0000 */
u32 reg_900A0004;
u8 unknown_900A_read_byte(u32 addr) {
	return bad_read_byte(addr);
}
u16 unknown_900A_read_half(u32 addr) {
	return bad_read_half(addr);
}
u32 unknown_900A_read_word(u32 addr) {
	switch (addr & 0xFFFF) {
		case 0x00: return 0;
		case 0x04: return reg_900A0004;
		/* Registers 28 and 2C give a 64-bit number (28 is low, 2C is high),
		 * which comprises 56 data bits and 8 parity checking bits:
		 *    Bit 0 is a parity check of all data bits
		 *    Bits 1, 2, 4, 8, 16, and 32 are parity checks of the data bits whose
		 *       positions, expressed in binary, have that respective bit set.
		 *    Bit 63 is a parity check of bits 1, 2, 4, 8, 16, and 32.
		 * With this system, any single-bit error can be detected and corrected.
		 * (But why would that happen?! I have no idea.)
		 *
		 * Anyway, bits 58-62 are the "ASIC user flags", a byte which must
		 * match the 80E0 field in an OS image. 01 = CAS, 00 = non-CAS. */
		case 0x28: return emulate_cas ? 0x00010105 : 0;
		case 0x2C: return emulate_cas ? 0x04000001 : 0;
	}
	return bad_read_word(addr);
}
void unknown_900A_write_byte(u32 addr, u8 value) {
	bad_write_byte(addr, value);
}
void unknown_900A_write_half(u32 addr, u16 value) {
	bad_write_half(addr, value);
}
void unknown_900A_write_word(u32 addr, u32 value) {
	switch (addr & 0xFFFF) {
		case 0x04: reg_900A0004 = value; return;
		case 0x08: cpu_events |= EVENT_RESET; return;
		case 0x18: irq_deactivate(1 << IRQ_APDTMR); return;
		case 0x1C: return;
		case 0x20: irq_deactivate(1 << IRQ_TIMER); return;
		case 0x24: return;
		case 0xF04: return;
	}
	bad_write_word(addr, value);
}

/* 900B0000 */
u8 unknown_900B_read_byte(u32 addr) {
	return bad_read_byte(addr);
}
u16 unknown_900B_read_half(u32 addr) {
	return bad_read_half(addr);
}
u32 unknown_900B_read_word(u32 addr) {
	switch (addr & 0xFFFF) {
		/* Register 0 includes the speeds of the various clocks.
		 *    Bits 1-7:   Multiply by 2 to get base/CPU ratio
		 *    Bit  8:     If set, base clock is 27 MHz, else see bits 16-20
		 *    Bits 12-14: Add 1 to get CPU/AHB ratio
		 *    Bits 16-20: If bit 8 is clear, base clock is (300 - 6*this) MHz
		 */
		case 0x00: return 0x00141002; // CPU = 90MHz   AHB = 45MHz   APB = 22MHz
		case 0x08: return 0;
		case 0x0C: return 0;
		case 0x18: return 0;
		case 0x20: return 0;
		case 0x28: return 0x14; // break out of loops in OS
	}
	return bad_read_word(addr);
}
void unknown_900B_write_byte(u32 addr, u8 value) {
	bad_write_byte(addr, value);
}
void unknown_900B_write_half(u32 addr, u16 value) {
	bad_write_half(addr, value);
}
void unknown_900B_write_word(u32 addr, u32 value) {
	switch (addr & 0xFFFF) {
		case 0x00: return;
		case 0x04: return;
		case 0x08: return;
		case 0x0C: return;
		case 0x14: return;
		case 0x18: return;
		case 0x20: return;
	}
	bad_write_word(addr, value);
}

/* 900C0000 */
u8 unknown_900C_read_byte(u32 addr) {
	return bad_read_byte(addr);
}
u16 unknown_900C_read_half(u32 addr) {
	return bad_read_half(addr);
}
u32 unknown_900C_read_word(u32 addr) {
	switch (addr & 0xFFFF) {
		case 0x08: return 0;
	}
	return bad_read_word(addr);
}
void unknown_900C_write_byte(u32 addr, u8 value) {
	bad_write_byte(addr, value);
}
void unknown_900C_write_half(u32 addr, u16 value) {
	bad_write_half(addr, value);
}
void unknown_900C_write_word(u32 addr, u32 value) {
	switch (addr & 0xFFFF) {
		case 0x00: case 0x04: case 0x08: case 0x18: case 0x30:
			return;
	}
	bad_write_word(addr, value);
}

/* 900D0000 */
u8 timer_read_byte(u32 addr) {
	return bad_read_byte(addr);
}
u16 timer_read_half(u32 addr) {
	return bad_read_half(addr);
}
u32 timer_read_word(u32 addr) {
	static u16 timer;
	switch (addr & 0xFFFF) {
		case 0xC: return --timer; // break timing loops
	}
	return bad_read_word(addr);
}
void timer_write_byte(u32 addr, u8 value) {
	bad_write_byte(addr, value);
}
void timer_write_half(u32 addr, u16 value) {
	bad_write_half(addr, value);
}
void timer_write_word(u32 addr, u32 value) {
	switch (addr & 0xFFFF) {
		case 0x00: return;
		case 0x04: return;
		case 0x08: return;
		case 0x0C: return;
		case 0x14: return;
		case 0x1C: return;
	}
	bad_write_word(addr, value);
}

/* 900E0000 */
u8 keypad_read_byte(u32 addr) {
	return bad_read_byte(addr);
}
u16 keypad_read_half(u32 addr) {
	u32 rel_addr = addr & 0xFFFF;
	if (rel_addr >= 0x10 && rel_addr <= 0x20)
		return ~key_map[(rel_addr - 0x10) >> 1];
	return bad_read_half(addr);
}
u32 keypad_read_word(u32 addr) {
	switch (addr & 0xFFFF) {
		case 0x0: return 0;
	}
	return bad_read_word(addr);
}
void keypad_write_byte(u32 addr, u8 value) {
	bad_write_byte(addr, value);
}
void keypad_write_half(u32 addr, u16 value) {
	bad_write_half(addr, value);
}
void keypad_write_word(u32 addr, u32 value) {
	switch (addr & 0xFFFF) {
		case 0x0: return;
		case 0x4: return;
	}
	bad_write_word(addr, value);
}

/* 900F0000 */
u8 unknown_900F_read_byte(u32 addr) {
	return bad_read_byte(addr);
}
u16 unknown_900F_read_half(u32 addr) {
	return bad_read_half(addr);
}
u32 unknown_900F_read_word(u32 addr) {
	switch (addr & 0xFFFF) {
		case 0x0C: return 0;
		case 0x10: return 0;
	}
	return bad_read_word(addr);
}
void unknown_900F_write_byte(u32 addr, u8 value) {
	bad_write_byte(addr, value);
}
void unknown_900F_write_half(u32 addr, u16 value) {
	bad_write_half(addr, value);
}
void unknown_900F_write_word(u32 addr, u32 value) {
	switch (addr & 0xFFFF) {
		case 0x0C: return;
		case 0x20: return;
	}
	bad_write_word(addr, value);
}

/* 90110000 */
u8 unknown_9011_read_byte(u32 addr) {
	return bad_read_byte(addr);
}
u16 unknown_9011_read_half(u32 addr) {
	return bad_read_half(addr);
}
u32 unknown_9011_read_word(u32 addr) {
	switch (addr & 0xFFFF) {
	}
	return bad_read_word(addr);
}
void unknown_9011_write_byte(u32 addr, u8 value) {
	bad_write_byte(addr, value);
}
void unknown_9011_write_half(u32 addr, u16 value) {
	bad_write_half(addr, value);
}
void unknown_9011_write_word(u32 addr, u32 value) {
	switch (addr & 0xFFFF) {
		case 0xB00: return;
		case 0xB04: return;
		case 0xB08: return;
		case 0xB0C: return;
		case 0xB10: return;
	}
	bad_write_word(addr, value);
}


u8 (*const apb_read_byte_map[0x12])(u32 addr) = {
	unknown_9000_read_byte,
	bad_read_byte,
	serial_read_byte,
	bad_read_byte,
	bad_read_byte,
	bad_read_byte,
	unknown_9006_read_byte,
	bad_read_byte,
	unknown_9008_read_byte,
	rtc_read_byte,
	unknown_900A_read_byte,
	unknown_900B_read_byte,
	unknown_900C_read_byte,
	timer_read_byte,
	keypad_read_byte,
	unknown_900F_read_byte,
	ti84_io_link_read_byte,
	unknown_9011_read_byte,
};
u16 (*const apb_read_half_map[0x12])(u32 addr) = {
	unknown_9000_read_half,
	bad_read_half,
	serial_read_half,
	bad_read_half,
	bad_read_half,
	bad_read_half,
	unknown_9006_read_half,
	bad_read_half,
	unknown_9008_read_half,
	rtc_read_half,
	unknown_900A_read_half,
	unknown_900B_read_half,
	unknown_900C_read_half,
	timer_read_half,
	keypad_read_half,
	unknown_900F_read_half,
	ti84_io_link_read_half,
	unknown_9011_read_half,
};
u32 (*const apb_read_word_map[0x12])(u32 addr) = {
	unknown_9000_read_word,
	bad_read_word,
	serial_read_word,
	bad_read_word,
	bad_read_word,
	bad_read_word,
	unknown_9006_read_word,
	bad_read_word,
	unknown_9008_read_word,
	rtc_read_word,
	unknown_900A_read_word,
	unknown_900B_read_word,
	unknown_900C_read_word,
	timer_read_word,
	keypad_read_word,
	unknown_900F_read_word,
	ti84_io_link_read_word,
	unknown_9011_read_word,
};
void (*const apb_write_byte_map[0x12])(u32 addr, u8 value) = {
	unknown_9000_write_byte,
	bad_write_byte,
	serial_write_byte,
	bad_write_byte,
	bad_write_byte,
	bad_write_byte,
	unknown_9006_write_byte,
	bad_write_byte,
	unknown_9008_write_byte,
	rtc_write_byte,
	unknown_900A_write_byte,
	unknown_900B_write_byte,
	unknown_900C_write_byte,
	timer_write_byte,
	keypad_write_byte,
	unknown_900F_write_byte,
	ti84_io_link_write_byte,
	unknown_9011_write_byte,
};
void (*const apb_write_half_map[0x12])(u32 addr, u16 value) = {
	unknown_9000_write_half,
	bad_write_half,
	serial_write_half,
	bad_write_half,
	bad_write_half,
	bad_write_half,
	unknown_9006_write_half,
	bad_write_half,
	unknown_9008_write_half,
	rtc_write_half,
	unknown_900A_write_half,
	unknown_900B_write_half,
	unknown_900C_write_half,
	timer_write_half,
	keypad_write_half,
	unknown_900F_write_half,
	ti84_io_link_write_half,
	unknown_9011_write_half,
};
void (*const apb_write_word_map[0x12])(u32 addr, u32 value) = {
	unknown_9000_write_word,
	bad_write_word,
	serial_write_word,
	bad_write_word,
	bad_write_word,
	bad_write_word,
	unknown_9006_write_word,
	bad_write_word,
	unknown_9008_write_word,
	rtc_write_word,
	unknown_900A_write_word,
	unknown_900B_write_word,
	unknown_900C_write_word,
	timer_write_word,
	keypad_write_word,
	unknown_900F_write_word,
	ti84_io_link_write_word,
	unknown_9011_write_word,
};

static void apb_check(u32 addr) {
	if (addr >= 0x90120000) error("Bad I/O address: %08x\n", addr);
}

u8 apb_read_byte(u32 addr) {
	apb_check(addr); return apb_read_byte_map[addr >> 16 & 31](addr);
}
u16 apb_read_half(u32 addr) {
	apb_check(addr); return apb_read_half_map[addr >> 16 & 31](addr);
}
u32 apb_read_word(u32 addr) {
	apb_check(addr); return apb_read_word_map[addr >> 16 & 31](addr);
}
void apb_write_byte(u32 addr, u8 value) {
	apb_check(addr); apb_write_byte_map[addr >> 16 & 31](addr, value);
}
void apb_write_half(u32 addr, u16 value) {
	apb_check(addr); apb_write_half_map[addr >> 16 & 31](addr, value);
}
void apb_write_word(u32 addr, u32 value) {
	apb_check(addr); apb_write_word_map[addr >> 16 & 31](addr, value);
}
