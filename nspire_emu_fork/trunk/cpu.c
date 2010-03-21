#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "emu.h"

struct arm_state arm;

void cpu_irq_check() {
	if (current_irqs != 0 && !(arm.cpsr_low28 & 0x80))
		cpu_events |= EVENT_IRQ;
	else
		cpu_events &= ~EVENT_IRQ;
}

/* Access the Current Program Status Register.
 * The flag bits (NZCV) are stored separately since they are so
 * frequently written to independently. */
u32 __attribute__((fastcall)) get_cpsr() {
	return arm.cpsr_n << 31
	     | arm.cpsr_z << 30
	     | arm.cpsr_c << 29
	     | arm.cpsr_v << 28
	     | arm.cpsr_low28;
}
void set_cpsr_full(u32 cpsr) {
	if ((cpsr ^ arm.cpsr_low28) & 0x1F) {
		/* Switching to a different processor mode. Swap out registers of old mode */
		if ((arm.cpsr_low28 & 0x1F) == MODE_FIQ)
			memcpy(arm.r8_fiq, &arm.reg[8], 20);
		else
			memcpy(arm.r8_usr, &arm.reg[8], 20);
		switch (arm.cpsr_low28 & 0x1F) {
			case MODE_USR:
			case MODE_SYS: memcpy(arm.r13_usr, &arm.reg[13], 8); break;
			case MODE_FIQ: memcpy(arm.r13_fiq, &arm.reg[13], 8); break;
			case MODE_IRQ: memcpy(arm.r13_irq, &arm.reg[13], 8); break;
			case MODE_SVC: memcpy(arm.r13_svc, &arm.reg[13], 8); break;
			case MODE_ABT: memcpy(arm.r13_abt, &arm.reg[13], 8); break;
			case MODE_UND: memcpy(arm.r13_und, &arm.reg[13], 8); break;
			default:       error("Invalid previous processor mode (This can't happen)\n");
		}

		/* Swap in registers of new mode */
		if ((cpsr & 0x1F) == MODE_FIQ)
			memcpy(&arm.reg[8], arm.r8_fiq, 20);
		else
			memcpy(&arm.reg[8], arm.r8_usr, 20);
		switch (cpsr & 0x1F) {
			case MODE_USR:
			case MODE_SYS: memcpy(&arm.reg[13], arm.r13_usr, 8); break;
			case MODE_FIQ: memcpy(&arm.reg[13], arm.r13_fiq, 8); break;
			case MODE_IRQ: memcpy(&arm.reg[13], arm.r13_irq, 8); break;
			case MODE_SVC: memcpy(&arm.reg[13], arm.r13_svc, 8); break;
			case MODE_ABT: memcpy(&arm.reg[13], arm.r13_abt, 8); break;
			case MODE_UND: memcpy(&arm.reg[13], arm.r13_und, 8); break;
			default:       error("Invalid new processor mode\n");
		}
	}

	arm.cpsr_n = cpsr >> 31 & 1;
	arm.cpsr_z = cpsr >> 30 & 1;
	arm.cpsr_c = cpsr >> 29 & 1;
	arm.cpsr_v = cpsr >> 28 & 1;
	arm.cpsr_low28 = cpsr & 0x0FFFFFFF;
	cpu_irq_check();
}
void __attribute__((fastcall)) set_cpsr(u32 cpsr, u32 mask) {
	if (!(arm.cpsr_low28 & 0x0F)) {
		/* User mode. Don't change privileged or execution state bits */
		mask &= ~0x010000FF;
	}
	mask &= 0xF90000FF; /* Mask off reserved bits */
	cpsr = (cpsr & mask) | (get_cpsr() & ~mask);
	if (cpsr & 0x01000020)
		error("J/T bits are not implemented");
	set_cpsr_full(cpsr);
}

/* Access the Saved Program Status Register. */
static u32 *ptr_spsr() {
	switch (arm.cpsr_low28 & 0x1F) {
		case MODE_FIQ: return &arm.spsr_fiq;
		case MODE_IRQ: return &arm.spsr_irq;
		case MODE_SVC: return &arm.spsr_svc;
		case MODE_ABT: return &arm.spsr_abt;
		case MODE_UND: return &arm.spsr_und;
	}
	error("Attempted to access SPSR from user or system mode");
}
inline u32 __attribute__((fastcall)) get_spsr() {
	return *ptr_spsr();
}
inline void set_spsr_full(u32 spsr) {
	*ptr_spsr() = spsr;
}
inline void __attribute__((fastcall)) set_spsr(u32 spsr, u32 mask) {
	*ptr_spsr() ^= (*ptr_spsr() ^ spsr) & mask;
}

/* Retrieve an ARM register. Deal with the annoying fact that accessing R15 (PC)
 * gives you the next instruction plus 4. (But when *modifying* PC, you set it
 * to the next instruction to execute plus NOTHING. What were they thinking?!) */
static u32 get_reg_pc(int rn) {
	return arm.reg[rn] + ((rn == 15) ? 4 : 0);
}
static inline void set_reg_pc(int rn, u32 value) {
	arm.reg[rn] = value;
}
static u32 get_reg(int rn) {
	if (rn == 15) error("Invalid use of R15");
	return arm.reg[rn];
}
static void set_reg(int rn, u32 value) {
	if (rn == 15) error("Invalid use of R15");
	arm.reg[rn] = value;
}

static inline void set_nz_flags(u32 value) {
	arm.cpsr_n = value >> 31;
	arm.cpsr_z = value == 0;
}

static inline void set_nz_flags_64(u64 value) {
	arm.cpsr_n = value >> 63;
	arm.cpsr_z = value == 0;
}

void cpu_exception(int type) {
	static const u8 flags[] = {
		MODE_SVC | 0xC0, /* Reset */
		MODE_UND | 0x80, /* Undefined instruction */
		MODE_SVC | 0x80, /* Software interrupt */
		MODE_ABT | 0x80, /* Prefetch abort */
		MODE_ABT | 0x80, /* Data abort */
		0,               /* Reserved */
		MODE_IRQ | 0x80, /* IRQ */
		MODE_FIQ | 0xC0, /* FIQ */
	};
	
	/* GDB relies on some exceptions, for example for soft breakpoints */
	if (type == EX_UNDEFINED && is_gdb_debugger) {
		gdbstub_exception(type);
		return;
	}

	/* Switch mode, disable interrupts */
	u32 old_cpsr = get_cpsr();
	set_cpsr_full((old_cpsr & ~0x3F) | flags[type]);
	set_spsr_full(old_cpsr);

	/* Branch-and-link to exception handler.
	 * Note: Exception handlers should really be at 0x00000000, not 0xA4000000,
	 * but the MMU is used to remap the former to the latter. Until the MMU is
	 * implemented, this should suffice. */
	arm.reg[14] = arm.reg[15];
	arm.reg[15] = 0xA4000000 | type << 2;
}

void cpu_exception_warn(int type, char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	warn(fmt, va);
	va_end(va);
	cpu_exception(type);
}

u32 calc_address(u32 insn, int base_reg, u32 offset) {
	u32 addr = get_reg_pc(base_reg);

	if (!(insn & (1 << 23))) // Subtract the offset
		offset = -offset;

	if (insn & (1 << 24)) { // Offset or pre-indexed addressing
		addr += offset;
		if (insn & (1 << 21)) // Pre-indexed addressing
			set_reg(base_reg, addr);
	} else {
		if (insn & (1 << 21))
			error("T-type memory access not implemented");
		set_reg(base_reg, addr + offset);
	}
	return addr;
}

/* Detect overflow after an addition or subtraction. */
#define ADD_OVERFLOW(left, right, sum) ((s32)(((left) ^ (sum)) & ((right) ^ (sum))) < 0)
#define SUB_OVERFLOW(left, right, sum) ((s32)(((left) ^ (right)) & ((left) ^ (sum))) < 0)

/* Do an addition, setting C/V flags accordingly. */
static u32 add(u32 left, u32 right, int carry, int setcc) {
	u32 sum = left + right + carry;
	if (setcc) {
		if (sum < left) carry = 1;
		if (sum > left) carry = 0;
		arm.cpsr_c = carry;
		arm.cpsr_v = ADD_OVERFLOW(left, right, sum);
	}
	return sum;
}

static int get_shifted_immed(int insn, int setcc) {
	int count = insn >> 7 & 30;
	s32 val = insn & 0xFF;
	val = val >> count | val << (32 - count);
	if (count != 0 && setcc)
		arm.cpsr_c = val < 0;
	return val;
}

/* Return -1 if an exception occurs */
static int get_shifted_reg(int *ret, int insn, int setcc) {
	u32 res = get_reg_pc(insn & 15);
	int type = insn >> 5 & 3;
	int count; 

	if (insn & (1 << 4)) {
		if (insn & (1 << 7)) {
			cpu_exception_warn(EX_UNDEFINED, "shift by reg, bit 7 set");
			return -1;
		}
		count = get_reg(insn >> 8 & 15) & 0xFF;
	} else {
		count = insn >> 7 & 31;
		if (count == 0) {
			switch (type) {
				case 0: /* LSL #0 */ break;
				case 1: /* LSR #32 */ count = 32; break;
				case 2: /* ASR #32 */ count = 32; break;
				case 3: /* RRX */ {
					*ret = arm.cpsr_c << 31 | res >> 1;
					if (setcc) arm.cpsr_c = res & 1;
					return 0;
				}
			}
		}
	}

	if (count == 0) {
		/* For all types, a count of 0 does nothing and does not affect carry. */
		*ret = res;
		return 0;
	}

	switch (type) {
		default: /* not used, obviously - here to shut up gcc warning */
		case 0: /* LSL */
			if (count >= 32) {
				if (setcc) arm.cpsr_c = (count == 32) ? (res & 1) : 0;
				*ret = 0;
				return 0;
			}
			if (setcc) arm.cpsr_c = res >> (32 - count) & 1;
			return res << count;
		case 1: /* LSR */
			if (count >= 32) {
				if (setcc) arm.cpsr_c = (count == 32) ? (res >> 31) : 0;
				*ret = 0;
				return 0;
			}
			if (setcc) arm.cpsr_c = res >> (count - 1) & 1;
			*ret = res >> count;
			return 0;
		case 2: /* ASR */
			if (count >= 32) {
				count = 31;
				if (setcc) arm.cpsr_c = res >> 31;
			} else {
				if (setcc) arm.cpsr_c = res >> (count - 1) & 1;
			}
			return (s32)res >> count;
		case 3: /* ROR */
			count &= 31;
			*ret = res >> count | res << (32 - count);
			return 0;
			if (setcc) arm.cpsr_c = res >> 31;
	}
}

void cpu_interpret_instruction(u32 insn) {
	int exec;
	switch (insn >> 29) {
		case 0:  /* EQ/NE */ exec = arm.cpsr_z; break;
		case 1:  /* CS/CC */ exec = arm.cpsr_c; break;
		case 2:  /* MI/PL */ exec = arm.cpsr_n; break;
		case 3:  /* VS/VC */ exec = arm.cpsr_v; break;
		case 4:  /* HI/LS */ exec = !arm.cpsr_z && arm.cpsr_c; break;
		case 5:  /* GE/LT */ exec = arm.cpsr_n == arm.cpsr_v; break;
		case 6:  /* GT/LE */ exec = !arm.cpsr_z && arm.cpsr_n == arm.cpsr_v; break;
		default: /* AL/-- */ exec = 1;
		                     if (insn & (1 << 28)) {
		                     	cpu_exception_warn(EX_UNDEFINED, "Invalid condition code");
		                     	return;
		                     }
		                     break;
	}
	if (!(exec ^ (insn >> 28 & 1)))
		return;

	if ((insn & 0xE000090) == 0x0000090) {
		int type = insn >> 5 & 3;
		if (type == 0) {
			if ((insn & 0xFC000F0) == 0x0000090) {
				/* MUL, MLA: 32x32 to 32 multiplications */
				u32 res = get_reg(insn & 15)
				        * get_reg(insn >> 8 & 15);
				if (insn & 0x0200000)
					res += get_reg(insn >> 12 & 15);

				set_reg(insn >> 16 & 15, res);
				if (insn & 0x0100000) set_nz_flags(res);
			} else if ((insn & 0xF8000F0) == 0x0800090) {
				/* UMULL, UMLAL, SMULL, SMLAL: 32x32 to 64 multiplications */
				u32 left   = get_reg(insn & 15);
				u32 right  = get_reg(insn >> 8 & 15);
				u32 reg_lo = insn >> 12 & 15;
				u32 reg_hi = insn >> 16 & 15;

				if (reg_lo == reg_hi) {
					cpu_exception_warn(EX_UNDEFINED, "RdLo and RdHi cannot be same for 64-bit multiply");
					return;
				}

				u64 res;
				if (insn & 0x0400000) res = (s64)(s32)left * (s32)right;
				else                  res = (u64)left * right;
				if (insn & 0x0200000) {
					/* Accumulate */
					res += (u64)get_reg(reg_hi) << 32 | get_reg(reg_lo);
				}

				set_reg(reg_lo, res);
				set_reg(reg_hi, res >> 32);
				if (insn & 0x0100000) set_nz_flags_64(res);
			} else if ((insn & 0xFB00FF0) == 0x1000090) {
				/* SWP, SWPB */
				u32 addr = get_reg(insn >> 16 & 15);
				u32 ld, st = get_reg(insn & 15);
				if (insn & 0x0400000) {
					ld = read_byte(addr); write_byte(addr, st);
				} else {
					ld = read_word(addr); write_word(addr, st);
				}
				set_reg(insn >> 12 & 15, ld);
			} else {
				goto bad_insn;
			}
		} else {
			/* Load/store halfword, signed byte/halfword, or doubleword */
			int base_reg = insn >> 16 & 15;
			int offset = (insn & (1 << 22))
				? (insn & 0x0F) | (insn >> 4 & 0xF0)
				: get_reg(insn & 15);
			u32 addr = calc_address(insn, base_reg, offset);

			int data_reg = insn >> 12 & 15;
			if (insn & (1 << 20)) {
				u32 data;
				if      (type == 1) data =      read_half(addr); /* LDRH  */
				else if (type == 2) data = (s8) read_byte(addr); /* LDRSB */
				else                data = (s16)read_half(addr); /* LDRSH */
				set_reg(data_reg, data);
			} else if (type == 1) { /* STRH */
				write_half(addr, get_reg(data_reg));
			} else {
				if (data_reg & 1) {
					cpu_exception_warn(EX_UNDEFINED, "LDRD/STRD with odd-numbered data register");
					return;
				}
				if (type == 2) { /* LDRD */
					set_reg(data_reg,     read_word(addr));
					set_reg(data_reg + 1, read_word(addr + 4));
				} else { /* STRD */
					write_word(addr,     get_reg(data_reg));
					write_word(addr + 4, get_reg(data_reg + 1));
				}
			}
		}
	} else if ((insn & 0xD900000) == 0x1000000) {
		/* Miscellaneous */
		if ((insn & 0xFFFFFD0) == 0x12FFF10) {
			/* B(L)X: Branch(, link,) and exchange T bit (T bit not implemented) */
			u32 target = get_reg_pc(insn & 15);
			if (insn & 0x20)
				arm.reg[14] = arm.reg[15];
			arm.reg[15] = target;
		} else if ((insn & 0xFBF0FFF) == 0x10F0000) {
			/* MRS: Move reg <- status */
			set_reg(insn >> 12 & 15, insn & 0x0400000 ? get_spsr() : get_cpsr());
		} else if ((insn & 0xFB0FFF0) == 0x120F000 ||
				   (insn & 0xFB0F000) == 0x320F000) {
			/* MSR: Move status <- reg */
			u32 val, mask = 0;
			if (insn & 0x2000000)
				val = get_shifted_immed(insn, 0);
			else
				val = get_reg(insn & 15);
			if (insn & 0x0080000) mask |= 0xFF000000;
			if (insn & 0x0040000) mask |= 0x00FF0000;
			if (insn & 0x0020000) mask |= 0x0000FF00;
			if (insn & 0x0010000) mask |= 0x000000FF;
			if (insn & 0x0400000)
				set_spsr(val, mask);
			else
				set_cpsr(val, mask);
		} else if ((insn & 0xF900090) == 0x1000080) {
			s32 left = get_reg(insn & 15);
			s16 right = get_reg(insn >> 8 & 15) >> (insn & 0x40 ? 16 : 0);
			s32 product;
			int type = insn >> 21 & 3;

			if (type == 1) {
				/* SMULW<y>, SMLAW<y>: Signed 32x16 to 48 multiply, uses only top 32 bits */
				product = (s64)left * right >> 16;
				if (!(insn & 0x20))
					goto accumulate;
			} else {
				/* SMUL<x><y>, SMLA<x><y>, SMLAL<x><y>: Signed 16x16 to 32 multiply */
				product = (s16)(left >> (insn & 0x20 ? 16 : 0)) * right;
			}
			if (type == 2) {
				/* SMLAL<x><y>: 64-bit accumulate */
				u32 reg_lo = insn >> 12 & 15;
				u32 reg_hi = insn >> 16 & 15;
				s64 sum;
				if (reg_lo == reg_hi) {
					cpu_exception_warn(EX_UNDEFINED, "RdLo and RdHi cannot be same for 64-bit accumulate");
					return;
				}
				sum = product + ((u64)get_reg(reg_hi) << 32 | get_reg(reg_lo));
				set_reg(reg_lo, sum);
				set_reg(reg_hi, sum >> 32);
			} else if (type == 0) {
			accumulate:
				{	/* SMLA<x><y>, SMLAW<y>: 32-bit accumulate */
					s32 acc = get_reg(insn >> 12 & 15);
					s32 sum = product + acc;
					/* Set Q flag on overflow */
					arm.cpsr_low28 |= ADD_OVERFLOW(product, acc, sum) << 27;
					set_reg(insn >> 16 & 15, sum);
				}
			} else {
				/* SMUL<x><y>, SMULW<y>: No accumulate */
				set_reg(insn >> 16 & 15, product);
			}
		} else if ((insn & 0xF900FF0) == 0x1000050) {
			/* QADD, QSUB, QDADD, QDSUB: Saturated arithmetic */
			s32 left  = get_reg(insn       & 15);
			s32 right = get_reg(insn >> 16 & 15);
			s32 res, overflow;
			if (insn & 0x400000) {
				/* Doubled right operand */
				res = right << 1;
				if (ADD_OVERFLOW(right, right, res)) {
					/* Overflow, set Q flag and saturate */
					arm.cpsr_low28 |= 1 << 27;
					res = (res < 0) ? 0x7FFFFFFF : 0x80000000;
				}
				right = res;
			}
			if (!(insn & 0x200000)) {
				res = left + right;
				overflow = ADD_OVERFLOW(left, right, res);
			} else {
				res = left - right;
				overflow = SUB_OVERFLOW(left, right, res);
			}
			if (overflow) {
				/* Set Q flag and saturate */
				arm.cpsr_low28 |= 1 << 27;
				res = (res < 0) ? 0x7FFFFFFF : 0x80000000;
			}
			set_reg(insn >> 12 & 15, res);
		} else if ((insn & 0xFFF0FF0) == 0x16F0F10) {
			/* CLZ: Count leading zeros */
			s32 value = get_reg(insn & 15);
			u32 zeros;
			for (zeros = 0; zeros < 32 && value >= 0; zeros++)
				value <<= 1;
			set_reg(insn >> 12 & 15, zeros);
		} else if ((insn & 0xFFF000F0) == 0xE1200070) {
			printf("Software breakpoint at %08x (%04x)\n",
				arm.reg[15], (insn >> 4 & 0xFFF0) | (insn & 0xF));
			debugger();
		} else {
			goto bad_insn;
		}
	} else if ((insn & 0xC000000) == 0) {
		/* Data processing instructions */
		u32 left, right, res;
		int setcc = insn >> 20 & 1;
		int opcode = insn >> 21 & 15;
		int dest_reg = insn >> 12 & 15;

		u8 c = arm.cpsr_c;

		left = get_reg_pc(insn >> 16 & 15);
		if (insn & (1 << 25))
			right = get_shifted_immed(insn, setcc);
		else
			if (get_shifted_reg(&right, insn, setcc) == -1)
				return;

		switch (opcode) {
			default: /* not used, obviously - here to shut up gcc warning */
			case 0:  /* AND */ res = left & right; break;
			case 1:  /* EOR */ res = left ^ right; break;
			case 2:  /* SUB */ res = add( left, ~right, 1, setcc); break;
			case 3:  /* RSB */ res = add(~left,  right, 1, setcc); break;
			case 4:  /* ADD */ res = add( left,  right, 0, setcc); break;
			case 5:  /* ADC */ res = add( left,  right, c, setcc); break;
			case 6:  /* SBC */ res = add( left, ~right, c, setcc); break;
			case 7:  /* RSC */ res = add(~left,  right, c, setcc); break;
			case 8:  /* TST */ res = left & right; break;
			case 9:  /* TEQ */ res = left ^ right; break;
			case 10: /* CMP */ res = add( left, ~right, 1, setcc); break;
			case 11: /* CMN */ res = add( left,  right, 0, setcc); break;
			case 12: /* ORR */ res = left | right; break;
			case 13: /* MOV */ res = right; break;
			case 14: /* BIC */ res = left & ~right; break;
			case 15: /* MVN */ res = ~right; break;
		}

		if ((opcode & 12) == 8) {
			if (dest_reg != 0) {
				cpu_exception_warn(EX_UNDEFINED, "Compare instruction has nonzero destination reg");
				return;
			}
		} else {
			set_reg_pc(dest_reg, res);
		}

		if (setcc) {
			set_nz_flags(res);
			if (dest_reg == 15) set_cpsr(get_spsr(), -1);
		}
	} else if ((insn & 0xC000000) == 0x4000000) {
		/* LDR(B), STR(B): Byte/word memory access */
		int base_reg = insn >> 16 & 15;

		u32 offset;
		if (insn & (1 << 25)) {
			if (insn & (1 << 4)) {
				cpu_exception_warn(EX_UNDEFINED, "Cannot shift memory offset by register");
				return;
			}
			if (get_shifted_reg(&offset, insn, 0) == -1)
				return;
		} else {
			offset = insn & 0xFFF;
		}

		u32 addr = calc_address(insn, base_reg, offset);

		if (insn & (1 << 20)) {
			if (insn & (1 << 22)) set_reg_pc(insn >> 12 & 15, read_byte(addr));
			else                  set_reg_pc(insn >> 12 & 15, read_word(addr));
		} else {
			if (insn & (1 << 22)) write_byte(addr, get_reg_pc(insn >> 12 & 15));
			else                  write_word(addr, get_reg_pc(insn >> 12 & 15));
		}
	} else if ((insn & 0xE000000) == 0xA000000) {
		/* B, BL: Branch, branch-and-link */
		if (insn & (1 << 24))
			arm.reg[14] = arm.reg[15];
		arm.reg[15] += 4 + ((s32)insn << 8 >> 6);
	} else if ((insn & 0xE000000) == 0x8000000) {
		/* LDM, STM: Load/store multiple */
		u32 addr = get_reg(insn >> 16 & 15);
		int delta, start, end, i;
		if (insn & (1 << 23)) {
			start = 0; end = 16; delta = 1;
		} else {
			start = 15; end = -1; delta = -1;
		}

		if (insn & (1 << 24)) // Preincrement
			addr += delta << 2;
		for (i = start; i != end; i += delta) {
			if (insn >> i & 1) {
				if (insn & (1 << 20)) {
					set_reg_pc(i, read_word(addr));
				} else {
					write_word(addr, get_reg_pc(i));
				}
				addr += delta << 2;
			}
		}
		if (insn & (1 << 21)) {
			if (insn & (1 << 24))
				addr -= delta << 2;
			arm.reg[insn >> 16 & 15] = addr;
		}

		if (insn & (1 << 22)) {
			if (insn & (1 << 20) && insn & 0x8000) {
				set_cpsr(get_spsr(), -1);
			} else {
				error("Unimplemented: LDM/STM with user mode regs");
			}
		}
	} else if ((insn & 0xF000000) == 0xE000000) {
		/* Coprocessor instructions */
		if (insn == 0xEE070F90) {
			// MCR p15, 0, <Rd>, c7, c0, 4
			// Wait for interrupt. For now, just wait for next clock tick
			cycle_count += cycle_count_delta;
			cycle_count_delta = 0;
		} else {
			logprintf(LOG_CPU, "Ignoring coprocessor instruction %08X\n", insn);
			if (insn == 0xEE17FF7E) {
				// MRC p15, 0, pc, c7, c14, 3
				// Used to invalidate cache, looped with BNE
				arm.cpsr_z = 1;
			}
		}
	} else if ((insn & 0xF000000) == 0xF000000) {
		/* SWI - Software interrupt */
		cpu_exception(EX_SWI);
	} else {
bad_insn:
		cpu_exception_warn(EX_UNDEFINED, "Unrecognized instruction %08x\n", insn);
	}
}
