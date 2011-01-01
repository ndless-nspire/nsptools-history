#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "emu.h"

/* The APB (Advanced Peripheral Bus) hosts peripherals that do not require
 * high bandwidth. The bridge to the APB is accessed via addresses 90xxxxxx. */

/* 90000000 */
u32 gpio_read(u32 addr) {
	if (!(addr & 0xFF00)) {
		int i = addr >> 6 & 3;
		switch (addr & 0x3F) {
			case 0x10: case 0x14: case 0x1C: case 0x20: case 0x24:
				return 0;
			case 0x18:
				return 0x0104 >> (i * 8) & 0xFF;
		}
	}
	return bad_read_word(addr);
}
void gpio_write(u32 addr, u32 value) {
	if (!(addr & 0xFF00)) {
		switch (addr & 0x3F) {
			case 0x04: case 0x08: case 0x0C:
			case 0x10: case 0x14: case 0x1C: case 0x20: case 0x24:
				return;
		}
	}
	bad_write_word(addr, value);
}

/* 90010000, 900C0000, 900D0000 */
struct timerpair timerpairs[3];
#define ADDR_TO_TP(addr) (&timerpairs[((addr) >> 16) % 5])

u32 timer_read(u32 addr) {
	struct timerpair *tp = ADDR_TO_TP(addr);
	cycle_count_delta = 0; // Avoid slowdown by fast-forwarding through polling loops
	switch (addr & 0x003F) {
		case 0x00: return tp->timers[0].value;
		case 0x04: return tp->timers[0].divider;
		case 0x08: return tp->timers[0].control;
		case 0x0C: return tp->timers[1].value;
		case 0x10: return tp->timers[1].divider;
		case 0x14: return tp->timers[1].control;
		case 0x18: case 0x1C: case 0x20: case 0x24: case 0x28: case 0x2C:
			return tp->completion_value[((addr & 0x3F) - 0x18) >> 2];
	}
	return bad_read_word(addr);
}
void timer_write(u32 addr, u32 value) {
	struct timerpair *tp = ADDR_TO_TP(addr);
	switch (addr & 0x003F) {
		case 0x00: tp->timers[0].start_value = tp->timers[0].value = value; return;
		case 0x04: tp->timers[0].divider = value; return;
		case 0x08: tp->timers[0].control = value & 0x1F; return;
		case 0x0C: tp->timers[1].start_value = tp->timers[1].value = value; return;
		case 0x10: tp->timers[1].divider = value; return;
		case 0x14: tp->timers[1].control = value & 0x1F; return;
		case 0x18: case 0x1C: case 0x20: case 0x24: case 0x28: case 0x2C:
			tp->completion_value[((addr & 0x3F) - 0x18) >> 2] = value; return;
		case 0x30: return;
	}
	bad_write_word(addr, value);
}
static void timer_int_check(struct timerpair *tp) {
	int_set(INT_TIMER0 + (tp - timerpairs), tp->int_status & tp->int_mask);
}
void timer_advance(struct timerpair *tp, int ticks) {
	struct timer *t;
	for (t = &tp->timers[0]; t != &tp->timers[2]; t++) {
		int newticks;
		if (t->control & 0x10)
			continue;
		for (newticks = t->ticks + ticks; newticks > t->divider; newticks -= (t->divider + 1)) {
			int compl = t->control & 7;
			t->ticks = 0;

			if (compl == 0 && t->value == 0)
				/* nothing */;
			else if (compl != 0 && compl != 7 && t->value == tp->completion_value[compl - 1])
				t->value = t->start_value;
			else
				t->value += (t->control & 8) ? +1 : -1;

			if (t == &tp->timers[0]) {
				for (compl = 0; compl < 6; compl++) {
					if (t->value == tp->completion_value[compl]) {
						tp->int_status |= 1 << compl;
						timer_int_check(tp);
					}
				}
			}
		}
		t->ticks = newticks;
	}
}

/* 90020000 */
struct serial {
	u8 rx_char;
	u8 interrupts;
	u8 DLL;
	u8 DLM;
	u8 IER;
	u8 LCR;
} serial;

static inline void serial_int_check() {
	int_set(INT_SERIAL, serial.interrupts & serial.IER);
}

void serial_byte_in(u8 byte) {
	serial.rx_char = byte;
	serial.interrupts |= 1;
	serial_int_check();
}

u32 serial_read(u32 addr) {
	switch (addr & 0x3F) {
		case 0x00:
			if (serial.LCR & 0x80)
				return serial.DLL; /* Divisor Latch LSB */
			if (!(serial.interrupts & 1))
				error("Attempted to read empty RBR");
			serial.interrupts &= ~1;
			serial_int_check();
			return serial.rx_char;
		case 0x04:
			if (serial.LCR & 0x80)
				return serial.DLM; /* Divisor Latch MSB */
			return serial.IER; /* Interrupt Enable Register */
		case 0x08: /* Interrupt Identification Register */
			if (serial.interrupts & serial.IER & 1) {
				return 4;
			} else if (serial.interrupts & serial.IER & 2) {
				serial.interrupts &= ~2;
				serial_int_check();
				return 2;
			} else {
				return 1;
			}
		case 0x0C: /* Line Control Register */
			return serial.LCR;
		case 0x14: /* Line Status Register */
			return 0x60 | (serial.interrupts & 1);
	}
	return bad_read_word(addr);
}
void serial_write(u32 addr, u32 value) {
	switch (addr & 0x3F) {
		case 0x00:
			if (serial.LCR & 0x80) {
				serial.DLL = value;
			} else {
				putchar(value);
				serial.interrupts |= 2;
				serial_int_check();
			}
			return;
		case 0x04:
			if (serial.LCR & 0x80) {
				serial.DLM = value;
			} else {
				serial.IER = value & 0x0F;
				serial_int_check();
			}
			return;
		case 0x0C: /* Line Control Register */
			serial.LCR = value;
			return;
		case 0x08: /* FIFO Control Register */
		case 0x10: /* Modem Control Register */
		case 0x20: /* unknown, used by DIAGS */
			return;
	}
	bad_write_word(addr, value);
}

/* 90060000 */
void watchdog_write(u32 addr, u32 value) {
	switch (addr & 0xFFFF) {
		case 0x000: return;
		case 0x008: return;
		case 0xC00: return;
	}
	bad_write_word(addr, value);
}

/* 90080000 */
void unknown_9008_write(u32 addr, u32 value) {
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
static time_t rtc_time_diff;
u32 rtc_read(u32 addr) {
	switch (addr & 0xFFFF) {
		case 0x00: return time(NULL) - rtc_time_diff;
		case 0x14: return 0;
	}
	return bad_read_word(addr);
}
void rtc_write(u32 addr, u32 value) {
	switch (addr & 0xFFFF) {
		case 0x04: return;
		case 0x08: rtc_time_diff = time(NULL) - value; return;
		case 0x0C: return;
		case 0x10: return;
	}
	bad_write_word(addr, value);
}

/* 900A0000 */
u32 unknown_900A_read(u32 addr) {
	struct timerpair *tp = &timerpairs[(addr - 0x10) >> 3 & 3];
	switch (addr & 0x0FFF) {
		case 0x00: return 0;
		case 0x04: return 0;
		case 0x10: case 0x18: case 0x20: return tp->int_status;
		case 0x14: case 0x1C: case 0x24: return tp->int_mask;
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
void unknown_900A_write(u32 addr, u32 value) {
	struct timerpair *tp = &timerpairs[(addr - 0x10) >> 3 & 3];
	switch (addr & 0x0FFF) {
		case 0x04: return;
		case 0x08: cpu_events |= EVENT_RESET; return;
		case 0x10: case 0x18: case 0x20: tp->int_status &= ~value; timer_int_check(tp); return;
		case 0x14: case 0x1C: case 0x24: tp->int_mask = value & 0x3F; timer_int_check(tp); return;
		case 0xF04: return;
	}
	bad_write_word(addr, value);
}

/* 900B0000 */
u32 unknown_900B_read(u32 addr) {
	switch (addr & 0x003F) {
		/* Register 0 includes the speeds of the various clocks.
		 *    Bits 1-7:   Multiply by 2 to get base/CPU ratio
		 *    Bit  8:     If set, base clock is 27 MHz, else see bits 16-20
		 *    Bits 12-14: Add 1 to get CPU/AHB ratio
		 *    Bits 16-20: If bit 8 is clear, base clock is (300 - 6*this) MHz
		 */
		case 0x00: return 0x00141002; // CPU = 90MHz   AHB = 45MHz   APB = 22MHz
		case 0x08: return 0;
		case 0x0C: return 0;
		case 0x14: return 0;
		case 0x18: return 0;
		case 0x20: return 0;
		/* Bit 4 clear when ON key pressed */
		case 0x28: return 0x114 & ~(key_map[0] >> 5 & 0x10);
	}
	return bad_read_word(addr);
}
void unknown_900B_write(u32 addr, u32 value) {
	switch (addr & 0x003F) {
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

/* 900E0000 */
u32 keypad_int_active;
u32 keypad_int_enable;
void keypad_int_check() {
	int_set(INT_KEYPAD, keypad_int_enable & keypad_int_active);
}
u32 keypad_read(u32 addr) {
	switch (addr & 0xFFFF) {
		case 0x00: return 0;
		case 0x04: return 9; // Bits 0-4: number of halfwords starting at +10
		case 0x08: return keypad_int_active;
		case 0x0C: return keypad_int_enable;
		case 0x10: case 0x14: case 0x18: case 0x1C: case 0x20:
			keypad_int_active = 0; // very dubious: it seems interrupts should be acknowledged
			keypad_int_check();    // by writing to 900e0008, but that doesn't work well for diags
			return ~*(u32 *)&key_map[(addr - 0x10) >> 1 & 15];
		case 0x40: return 0; // related to another set of keypad interrupts
		case 0x44: return 0; // related to another set of keypad interrupts
	}
	return bad_read_word(addr);
}
void keypad_write(u32 addr, u32 value) {
	switch (addr & 0xFFFF) {
		case 0x00: return;
		case 0x04: return;
		case 0x08: /* keypad_int_active &= ~value; keypad_int_check(); */ return;
		case 0x0C: keypad_int_enable = value; keypad_int_check(); return;
		case 0x44: return; // related to another set of keypad interrupts
	}
	bad_write_word(addr, value);
}

/* 900F0000 */
u8 lcd_contrast;
u32 unknown_900F_read(u32 addr) {
	switch (addr & 0xFFFF) {
		case 0x08: return 0; // newly used in OS 2.1
		case 0x0C: return 0;
		case 0x10: return 0;
		case 0x14: return 0; // newly used in OS 2.1
		case 0x20: return lcd_contrast;
	}
	return bad_read_word(addr);
}
void unknown_900F_write(u32 addr, u32 value) {
	switch (addr & 0xFFFF) {
		case 0x04: return; // newly used in OS 2.1
		case 0x0C: return;
		case 0x14: return; // newly used in OS 2.1
		case 0x20: lcd_contrast = value; return;
	}
	bad_write_word(addr, value);
}

/* 90110000 */
void unknown_9011_write(u32 addr, u32 value) {
	switch (addr & 0xFFFF) {
		case 0xB00: return;
		case 0xB04: return;
		case 0xB08: return;
		case 0xB0C: return;
		case 0xB10: return;
	}
	bad_write_word(addr, value);
}

/* The AMBA specification does not mention anything about transfer sizes in APB,
 * so probably all reads/writes are effectively 32 bit. */

const struct {
	u32 (*read)(u32 addr);
	void (*write)(u32 addr, u32 value);
} apb_map[0x12] = {
	{ gpio_read,         gpio_write         },
	{ timer_read,        timer_write        },
	{ serial_read,       serial_write       },
	{ bad_read_word,     bad_write_word     },
	{ bad_read_word,     bad_write_word     },
	{ bad_read_word,     bad_write_word     },
	{ bad_read_word,     watchdog_write     },
	{ bad_read_word,     bad_write_word     },
	{ bad_read_word,     unknown_9008_write },
	{ rtc_read,          rtc_write          },
	{ unknown_900A_read, unknown_900A_write },
	{ unknown_900B_read, unknown_900B_write },
	{ timer_read,        timer_write        },
	{ timer_read,        timer_write        },
	{ keypad_read,       keypad_write       },
	{ unknown_900F_read, unknown_900F_write },
	{ ti84_io_link_read, ti84_io_link_write },
	{ bad_read_word,     unknown_9011_write },
};

static void apb_check(u32 addr) {
	if (addr >= 0x90120000) error("Bad I/O address: %08x\n", addr);
}

u8 apb_read_byte(u32 addr) {
	apb_check(addr);
	return apb_map[addr >> 16 & 31].read(addr & ~3) >> ((addr & 3) << 3);
}
u16 apb_read_half(u32 addr) {
	apb_check(addr);
	return apb_map[addr >> 16 & 31].read(addr & ~2) >> ((addr & 2) << 3);
}
u32 apb_read_word(u32 addr) {
	apb_check(addr);
	return apb_map[addr >> 16 & 31].read(addr);
}
void apb_write_byte(u32 addr, u8 value) {
	apb_check(addr);
	apb_map[addr >> 16 & 31].write(addr & ~3, value * 0x01010101);
}
void apb_write_half(u32 addr, u16 value) {
	apb_check(addr);
	apb_map[addr >> 16 & 31].write(addr & ~2, value * 0x00010001);
}
void apb_write_word(u32 addr, u32 value) {
	apb_check(addr);
	apb_map[addr >> 16 & 31].write(addr, value);
}

struct apb_saved_state {
	struct serial serial;
	u32 keypad_int_active;
	u32 keypad_int_enable;
	struct timerpair timerpairs[3];
};

void *apb_save_state(size_t *size) {
	*size = sizeof(struct apb_saved_state);
	struct apb_saved_state *state = malloc(*size);
	memcpy(&state->serial, &serial, sizeof(serial));
	state->keypad_int_active = keypad_int_active;
	state->keypad_int_enable = keypad_int_enable;
	memcpy(&state->timerpairs, timerpairs, sizeof(timerpairs));
	return state;
}

void apb_reload_state(void *state) {
	struct apb_saved_state *_state = (struct apb_saved_state *)state;
	memcpy(&serial, &_state->serial, sizeof(serial));
	keypad_int_active = _state->keypad_int_active;
	keypad_int_enable = _state->keypad_int_enable;
	memcpy(timerpairs, &_state->timerpairs, sizeof(timerpairs));
}
