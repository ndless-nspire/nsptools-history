#include "emu.h"

/* DC000000: Interrupt controller */
struct interrupt_state intr;

static void get_current_int(int is_fiq, int *current) {
	u32 masked_status = intr.status & intr.mask[is_fiq];
	int pri_limit = intr.priority_limit[is_fiq];
	int i;
	for (i = 0; i < 32; i++) {
		if (masked_status & (1 << i) && intr.priority[i] < pri_limit) {
			*current = i;
			pri_limit = intr.priority[i];
		}
	}
}

static void update() {
	u32 prev_raw_status = intr.raw_status;
	intr.raw_status = intr.active ^ ~intr.noninverted;

	intr.sticky_status |= (intr.raw_status & ~prev_raw_status);
	intr.status = (intr.raw_status    & ~intr.sticky)
	            | (intr.sticky_status &  intr.sticky);

	int is_fiq;
	for (is_fiq = 0; is_fiq < 2; is_fiq++) {
		int i = -1;
		get_current_int(is_fiq, &i);
		if (i >= 0) {
			arm.interrupts |= 0x80 >> is_fiq;
			cpu_int_check();
		}
	}
}

u32 int_read_word(u32 addr) {
	int group = addr >> 8 & 3;
	if (group < 2) {
		int is_fiq = group;
		int current = 0;
		switch (addr & 0xFF) {
			case 0x00:
				return intr.status & intr.mask[is_fiq];
			case 0x04:
				return intr.status;
			case 0x08:
			case 0x0C:
				return intr.mask[is_fiq];
			case 0x20:
				get_current_int(is_fiq, &current);
				return current;
			case 0x24:
				get_current_int(is_fiq, &current);
				intr.prev_pri_limit[is_fiq] = intr.priority_limit[is_fiq];
				intr.priority_limit[is_fiq] = intr.priority[current];
				return current;
			case 0x28:
				current = -1;
				get_current_int(is_fiq, &current);
				if (current < 0) {
					arm.interrupts &= ~(0x80 >> is_fiq);
					cpu_int_check();
				}
				return intr.prev_pri_limit[is_fiq];
			case 0x2C:
				return intr.priority_limit[is_fiq];
		}
	} else if (group == 2) {
		switch (addr & 0xFF) {
			case 0x00: return intr.noninverted;
			case 0x04: return intr.sticky;
			case 0x08: return 0;
		}
	} else {
		if (!(addr & 0x80))
			return intr.priority[addr >> 2 & 0x1F];
	}
	return bad_read_word(addr);
}
void int_write_word(u32 addr, u32 value) {
	int group = addr >> 8 & 3;
	if (group < 2) {
		int is_fiq = group;
		switch (addr & 0xFF) {
			case 0x04: intr.sticky_status &= ~value; update(); return;
			case 0x08: intr.mask[is_fiq] |= value; update(); return;
			case 0x0C: intr.mask[is_fiq] &= ~value; update(); return;
			case 0x2C: intr.priority_limit[is_fiq] = value & 0x0F; update(); return;
		}
	} else if (group == 2) {
		switch (addr & 0xFF) {
			case 0x00: intr.noninverted = value; update(); return;
			case 0x04: intr.sticky = value; update(); return;
			case 0x08: return;
		}
	} else {
		if (!(addr & 0x80)) {
			intr.priority[addr >> 2 & 0x1F] = value & 7;
			return;
		}
	}
	return bad_write_word(addr, value);
}

void int_set(u32 int_num, bool on) {
	if (on) intr.active |= 1 << int_num;
	else    intr.active &= ~(1 << int_num);
	update();
}

struct int_saved_state {
	struct interrupt_state intr;
};

void *int_save_state(size_t *size) {
	*size = sizeof(struct int_saved_state);
	struct int_saved_state *state = malloc(*size);
	memcpy(&state->intr, &intr, sizeof(intr));
	return state;
}

void int_reload_state(void *state) {
	struct int_saved_state *_state = (struct int_saved_state *)state;
	memcpy(&intr, &_state->intr, sizeof(intr));
}