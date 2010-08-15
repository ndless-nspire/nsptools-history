#include <stdlib.h>
#include <string.h>
#include "emu.h"

static u32 lcd_framebuffer;
static u32 lcd_control;
static u16 lcd_palette[256];

/* Draw the current screen into a 4bpp upside-down bitmap. (SetDIBitsToDevice
 * supports either orientation, but some programs can't paste right-side-up bitmaps) */
void lcd_draw_frame(u8 buffer[240][160]) {
	u32 bpp = 1 << (lcd_control >> 1 & 3);
	u32 *in = phys_mem_ptr(lcd_framebuffer, (320 * 240) / 8 * bpp);
	if (!in) {
		memset(buffer, 0, 160 * 240);
		return;
	}
	int row;
	for (row = 239; row >= 0; row--) {
		u32 mask = (1 << bpp) - 1;
		u32 pal_shift = lcd_control & (1 << 8) ? 11 : 1;
		u32 bi = (lcd_control & (1 << 9)) ? 0 : 24;
		if (!(lcd_control & (1 << 10)))
			bi ^= (8 - bpp);
		int words = (320 / 32) * bpp;
		u8 *out = buffer[row];
		do {
			u32 word = *in++;
			int bitpos = 32;
			do {
				int color1 = lcd_palette[word >> ((bitpos -= bpp) ^ bi) & mask] >> pal_shift & 15;
				int color2 = lcd_palette[word >> ((bitpos -= bpp) ^ bi) & mask] >> pal_shift & 15;
				*out++ = color1 << 4 | color2;
				color1 = lcd_palette[word >> ((bitpos -= bpp) ^ bi) & mask] >> pal_shift & 15;
				color2 = lcd_palette[word >> ((bitpos -= bpp) ^ bi) & mask] >> pal_shift & 15;
				*out++ = color1 << 4 | color2;
			} while (bitpos != 0);
		} while (--words != 0);
	}
}

u32 lcd_read_word(u32 addr) {
	if (addr >= 0xC0000200 && addr < 0xC0000400)
		return *(u32 *)((u8 *)lcd_palette + addr - 0xC0000200);
	switch (addr & 0x3FFFFFF) {
		case 0x010: return lcd_framebuffer;
		case 0x01C: return lcd_control;
	}
	return bad_read_word(addr);
}

void lcd_write_word(u32 addr, u32 value) {
	if (addr >= 0xC0000200 && addr < 0xC0000400) {
		*(u32 *)((u8 *)lcd_palette + addr - 0xC0000200) = value;
		return;
	}
	switch (addr & 0x3FFFFFF) {
		case 0x000: case 0x004: case 0x008: case 0x00C:
			return;
		case 0x010:
			lcd_framebuffer = value;
			return;
		case 0x014: case 0x018:
			return;
		case 0x01C:
			lcd_control = value;
			return;
	}
	return bad_write_word(addr, value);
}

struct lcd_saved_state {
	u32 lcd_framebuffer;
	u32 lcd_control;
	u16 lcd_palette[256];
};

void *lcd_save_state(size_t *size) {
	*size = sizeof(struct lcd_saved_state);
	struct lcd_saved_state *state = malloc(*size);
	state->lcd_framebuffer = lcd_framebuffer;
	state->lcd_control = lcd_control;
	memcpy(&state->lcd_palette, lcd_palette, sizeof(lcd_palette));
	return state;
}

void lcd_reload_state(void *state) {
	struct lcd_saved_state *_state = (struct lcd_saved_state *)state;
	lcd_framebuffer = _state->lcd_framebuffer;
	lcd_control = _state->lcd_control;
	memcpy(lcd_palette, &_state->lcd_palette, sizeof(lcd_palette));
}
