#include <os.h>
#include "nspireio2.h"
#include "debug.h"

static const unsigned strtok_addrs[] = {0x1037C050, 0, 0, 0}; // non-CAS 3.1, CAS 3.1, non-CAS CX 3.1, CAS CX 3.1 addresses
#define strtok SYSCALL_CUSTOM(strtok_addrs, char *, char *str, const char *delimiters)

static void __attribute__ ((noreturn)) reboot(void) {
	*(unsigned*)0x900A0008 = 2; //CPU reset
	__builtin_unreachable();
}

static unsigned *regs;
unsigned *debug_next = NULL;
enum e_spsr {SPSR_N = (1<<4), SPSR_Z=(1<<3), SPSR_C=(1<<2), SPSR_V=(1<<1), SPSR_Q=(1<<0)};	
#define SPSR_BIT(bit) ((spsr >> 27) & bit)
static unsigned spsr;


static void (*orig_prefetch_abort_addr)(void);
#define MAX_BKPTS 16
// BCONT is used only used to restore the previous breakpoint removed to continue
enum e_bkpt_type {BSTD = 0, BTMP, BCONT, BEXPL};
struct breakpoint {
	BOOL used;
	enum e_bkpt_type type;
	unsigned addr;
	unsigned orig_instr;
};
static struct breakpoint breakpoints[MAX_BKPTS];

void _dbg_set_breakpoint(unsigned addr, enum e_bkpt_type btype) {
	unsigned i = 0;
	for (i = 0; i < MAX_BKPTS; i++) {
		if (!(breakpoints[i].used)) break;
	}
	if (i >= MAX_BKPTS) return;
	breakpoints[i].used = TRUE;
	breakpoints[i].type = btype;
	breakpoints[i].addr = addr;
	breakpoints[i].orig_instr = *(unsigned*)addr;
	*(unsigned*)addr = 0xE1200070 | i; // 'bkpt #i'
	clear_cache();
}

void dbg_set_breakpoint(unsigned addr) {
	_dbg_set_breakpoint(addr, BSTD);
}

// from nspire_emu
static unsigned parse_expr(char *str) {
	unsigned sum = 0;
	int sign = 1;
	if (str == NULL)
		return 0;
	while (*str) {
		int reg;
		if (isxdigit(*str)) {
			sum += sign * strtoul(str, &str, 16);
			sign = 1;
		} else if (*str == '+') {
			str++;
		} else if (*str == '-') {
			sign = -1;
			str++;
		} else if (*str == 'r') {
			reg = strtoul(str + 1, &str, 10);
			sum += sign * regs[reg];
			sign = 1;
		} else if (isxdigit(*str)) {
			sum += sign * strtoul(str, &str, 16);
			sign = 1;
		} else {
			for (reg = 13; reg < 16; reg++) {
				if (!memcmp(str, reg_name[reg], 2)) {
					str += 2;
					sum += sign * regs[reg];
					sign = 1;
					goto ok;
				}
			}
			printf("syntax error\n");
			return 0;
			ok:;
		}
	}
	return sum;
}

static nio_console dbg_csl;

static u32 disasm_insn(u32 pc) {
	return disasm_arm_insn(pc, &dbg_csl);
}

static void disasm(u32 addr) {
	int i;
	for (i = 0; i < 8; i++) {
		u32 len = disasm_insn(addr);
		if (!len) {
			printf("Address %08X is not in RAM.\n", addr);
			break;
		}
		addr += len;
	}
}

static void disasm_cmd(void) {
	char *arg = strtok(NULL, " \n");
	u32 addr = arg ? parse_expr(arg) : regs[15];
	disasm(addr);
}

// adapted from nspire_emu
static void dump(unsigned addr) {
	u32 start = addr;
	u32 end = addr + 0x3F;

	u32 row, col;
	for (row = start & ~0x7; row <= end; row += 0x8) {
		u8 *ptr = virt_mem_ptr(row, 16);
		nio_printf(&dbg_csl, "%08X  ", row);
		for (col = 0; col < 0x8; col++) {
			addr = row + col;
			if (addr < start || addr > end)
				nio_printf(&dbg_csl, "   ");
			else
				nio_printf(&dbg_csl, "%02X ", ptr[col]);
		}
		nio_printf(&dbg_csl, " ");
		for (col = 0; col < 0x8; col++) {
			addr = row + col;
			if (addr < start || addr > end)
				nio_printf(&dbg_csl, " ");
			else if (ptr[col] < 0x20)
				nio_printf(&dbg_csl, ".");
			else
				nio_printf(&dbg_csl, "%c", ptr[col]);
		}
		nio_printf(&dbg_csl, "\n");
	}
}

static inline u32 get_reg_pc_bx(u32 value) {
	if (value & 1)
		return value - 1;
	else return value;
}

/* Retrieve an ARM register. Deal with the annoying effect of the CPU pipeline
 * that accessing R15 (PC) gives you the next instruction plus 4 (8 for str/stm) */
static u32 get_reg_pc(int rn) {
	return regs[rn] + ((rn == 15) ? 4 : 0);
}

static u32 shift(int type, u32 res, u32 count) {
	if (count == 0) {
		/* For all types, a count of 0 does nothing and does not affect carry. */
		return res;
	}

	switch (type) {
		default: /* not used, obviously - here to shut up gcc warning */
		case 0: /* LSL */
			if (count >= 32) {
				return 0;
			}
			return res << count;
		case 1: /* LSR */
			if (count >= 32) {
				return 0;
			}
			return res >> count;
		case 2: /* ASR */
			if (count >= 32) {
				count = 31;
			}
			return (s32)res >> count;
		case 3: /* ROR */
			count &= 31;
			res = res >> count | res << (32 - count);
			return res;
	}
}

static u32 get_reg(int rn) {
	return regs[rn];
}

static int get_shifted_immed(int insn) {
	int count = insn >> 7 & 30;
	s32 val = insn & 0xFF;
	val = val >> count | val << (32 - count);
	return val;
}

static int get_shifted_reg(int insn) {
	u32 res = get_reg_pc(insn & 15);
	int type = insn >> 5 & 3;
	int count; 

	if (insn & (1 << 4)) {
		count = get_reg(insn >> 8 & 15) & 0xFF;
	} else {
		count = insn >> 7 & 31;
		if (count == 0) {
			switch (type) {
				case 0: /* LSL #0 */ return res;
				case 1: /* LSR #32 */ count = 32; break;
				case 2: /* ASR #32 */ count = 32; break;
				case 3: /* RRX */ {
					u32 ret = SPSR_BIT(SPSR_C) << 31 | res >> 1;
					return ret;
				}
			}
		}
	}
	return shift(type, res, count);
}

/* parts taken from nspire_emu's cpu.c */
static unsigned next_pc(unsigned addr) {
	unsigned insn = *(unsigned*)addr;
	int exec;
	regs[15] += 4;
	switch (insn >> 29) {
		case 0:  /* EQ/NE */ exec = SPSR_BIT(SPSR_Z); break;
		case 1:  /* CS/CC */ exec  =SPSR_BIT(SPSR_C); break;
		case 2:  /* MI/PL */ exec = SPSR_BIT(SPSR_N); break;
		case 3:  /* VS/VC */ exec = SPSR_BIT(SPSR_V); break;
		case 4:  /* HI/LS */ exec = !SPSR_BIT(SPSR_Z) && SPSR_BIT(SPSR_C); break;
		case 5:  /* GE/LT */ exec = SPSR_BIT(SPSR_N) == SPSR_BIT(SPSR_V); break;
		case 6:  /* GT/LE */ exec = !SPSR_BIT(SPSR_Z) && SPSR_BIT(SPSR_N) == SPSR_BIT(SPSR_V); break;
		default: /* AL/-- */ exec = 1;
			if (insn & (1 << 28)) {
				 if ((insn & 0xFE000000) == 0xFA000000) {
					/* BLX: Branch, link, and exchange T bit */
					return regs[15] + 4 + ((s32)insn << 8 >> 6) + (insn >> 23 & 2);
				}
			}
	}
	if (!(exec ^ (insn >> 28 & 1)))
		return regs[15];
	
	if ((insn & 0xD900000) == 0x1000000) {
		/* Miscellaneous */
		if ((insn & 0xFFFFFD0) == 0x12FFF10) {
			/* B(L)X: Branch(, link,) and exchange T bit */
			u32 target = get_reg_pc(insn & 15);
			return get_reg_pc_bx(target);
		}
	} else if ((insn & 0xC000000) == 0) {
		/* Data processing instructions */
		u32 left, right, res;
		int opcode = insn >> 21 & 15;
		int dest_reg = insn >> 12 & 15;

		u8 c = SPSR_BIT(SPSR_C);

		left = get_reg_pc(insn >> 16 & 15);
		if (insn & (1 << 25))
			right = get_shifted_immed(insn);
		else
			right = get_shifted_reg(insn);

		switch (opcode) {
			default: /* not used, obviously - here to shut up gcc warning */
			case 0:  /* AND */ res = left & right; break;
			case 1:  /* EOR */ res = left ^ right; break;
			case 2:  /* SUB */ res = left + ~right + 1; break;
			case 3:  /* RSB */ res = ~left + right + 1; break;
			case 4:  /* ADD */ res = left + right; break;
			case 5:  /* ADC */ res = left + right + c; break;
			case 6:  /* SBC */ res = left + ~right + c; break;
			case 7:  /* RSC */ res = ~left + right + c; break;
			case 8:  /* TST */ res = left & right; break;
			case 9:  /* TEQ */ res = left ^ right; break;
			case 10: /* CMP */ res = left + ~right + 1; break;
			case 11: /* CMN */ res = left + right; break;
			case 12: /* ORR */ res = left | right; break;
			case 13: /* MOV */ res = right; break;
			case 14: /* BIC */ res = left & ~right; break;
			case 15: /* MVN */ res = ~right; break;
		}
		
		if ((opcode & 12) == 8 && dest_reg == 15)
			return res;
	} else if ((insn & 0xC000000) == 0x4000000) {
		/* LDR(B), STR(B): Byte/word memory access */
		int base_reg = insn >> 16 & 15;
		int data_reg = insn >> 12 & 15;
		if (data_reg == 15) {
			u32 offset;
			if (insn & (1 << 25)) {
				offset = get_shifted_reg(insn);
			} else {
				offset = insn & 0xFFF;
			}
	
			u32 read_addr = get_reg_pc(base_reg);
	
			if (!(insn & (1 << 23))) // Subtracted offset
				offset = -offset;
	
			if (insn & (1 << 24)) { // Offset or pre-indexed addressing
				read_addr += offset;
				offset = 0;
			}
			if (insn & (1 << 20)) {
				if (insn & (1 << 22)) return get_reg_pc_bx(*(unsigned char*)(read_addr));
				else                  return get_reg_pc_bx(*(unsigned *)(read_addr));
			}
		}
	} else if ((insn & 0xE000000) == 0xA000000) {
		/* B, BL: Branch, branch-and-link */
		return regs[15] + 4 + ((s32)insn << 8 >> 6);
	}
	return regs[15];
}

// Local frame not available for naked functions. non-static so that it's not inlined.
// Returns the return address to jump to.
unsigned dbg_prefetch_abort_handler_body(unsigned *exception_addr) {
	int bkpt_num = ((*exception_addr & 0xFFF00) >> 4) | (*exception_addr & 0xF);
	if (bkpt_num > MAX_BKPTS) {
		_dbg_set_breakpoint(next_pc((unsigned)exception_addr), BEXPL); // explicit breakpoint
		return (unsigned)(exception_addr + 1); // skip it
	}
	*exception_addr = breakpoints[bkpt_num].orig_instr;
	clear_cache();
	// Did we hit the "next" breakpoint?
	if (exception_addr == debug_next) {
		debug_next = NULL;
		disasm_insn((u32)exception_addr);
	} else {
		if (breakpoints[bkpt_num].type == BEXPL)
			nio_printf(&dbg_csl, "Explicit breakpoint hit at %08x\n", exception_addr);
		else
			nio_printf(&dbg_csl, "Breakpoint #%u hit at %08x\n", bkpt_num, exception_addr);
		disasm((u32)exception_addr);
	}
	if (breakpoints[bkpt_num].type == BTMP || breakpoints[bkpt_num].type == BEXPL)
		breakpoints[bkpt_num].used = FALSE;

	char line[100];
	while (1) {
		char *cmd;
		nio_printf(&dbg_csl, "> ");
		if (!nio_GetStr(&dbg_csl, line))
			continue;
		cmd = strtok(line, " ");
		if (!strcmp("c", cmd)) {
			nio_printf(&dbg_csl, "Continuing...\n");
			return (unsigned)exception_addr;
		} else if (!strcmp("d", cmd)) {
			dump(parse_expr(strtok(NULL, " ")));
		} else if (!strcmp(cmd, "r")) {
			int i;
			for (i = 0; i < 16; i++) {
				int newline = !((i + 1) % 3);
				nio_printf(&dbg_csl, "%3s=%08x%c", reg_name[i], regs[i], newline ? '\n' : ' ');
			}
			nio_printf(&dbg_csl, "\n");
		} else if (!strcmp(cmd, "u")) {
			disasm_cmd();
		} else if (!strcmp(cmd, "s")) {
			debug_next = (unsigned*)next_pc((unsigned)exception_addr);
			_dbg_set_breakpoint((unsigned)debug_next, BTMP);
			return (unsigned)exception_addr;
		} else if (!strcmp(cmd, "n")) {
			debug_next = exception_addr + 1;
			_dbg_set_breakpoint((unsigned)debug_next, BTMP);
			return (unsigned)exception_addr;
		} else if (!strcmp(cmd, "k") || !strcmp(cmd, "kt")) {
			char *addr_str = strtok(NULL, " \n");
			if (addr_str) {
				u32 addr = parse_expr(addr_str);
				_dbg_set_breakpoint(addr, cmd[1] == 't' ? BTMP : BSTD);
			} else {
				nio_printf(&dbg_csl, "Missing address\n");
			}
		} else {
			nio_printf(&dbg_csl, "Unknown command\n");
		}
	}
}

// triggered by bkpt instructions
static void __attribute__((naked)) prefetch_abort_handler(void) {
	// Save lr, and switch back to caller's mode (Ndless's swi handler doesn't support abort mode)
	unsigned *exception_addr;
	__asm volatile(" b 0f \n"
		"prefetch_abort_handler_exception_addr: .long 0 \n"
		"saved_spsr: .long 0 \n"
		"0: stmfd sp!, {r0-r1, r2} \n" /* dummy r2, will be overwritten */
		" adr r0, prefetch_abort_handler_exception_addr \n"
		" sub lr, lr, #4 \n"
		" str lr, [r0], #4 \n"
		" mrs	r1, spsr \n"
		" str r1, [r0] \n"
		" adr r0, 1f \n "
		" str r0, [sp, #4*2] \n" /* overwrite dummy r2 */
		" ldmfd sp!, {r0-r1, pc}^ \n" /* switch back to caller's mode */
		"1: stmfd sp!, {sp, lr, pc} \n" /* high part of the reg list. pc will be overwritten by the exception addr */
		" stmfd sp!, {r0-r12} \n" /* low part of the reg list (sp cannot be stored as last reg with stmfd) */
		" mov %1, sp \n" /* reg list */
		" adr r1, prefetch_abort_handler_exception_addr \n" 
		" ldr %0, [r1], #4 \n" 
		" str %0, [sp, #15*4] \n" /* to the reg list, replacing pc */
		" ldr %2, [r1] \n"
		: "=r" (exception_addr), "=r" (regs), "=r" (spsr));
	if ((*exception_addr & 0xFFF000F0) != 0xE1200070) // bkpt instr?
		reboot(); // no
	__asm volatile(" str %0, [sp, #15*4] \n" /* overwrite pc */
		" msr cpsr, %1 \n"
		" ldmfd sp!, {r0-r12} \n"
		" add sp, sp, #4 \n" /* don't restore sp from the reg list */
		" ldmfd sp!, {lr, pc}^ \n"
		: : "r" (dbg_prefetch_abort_handler_body(exception_addr)), "r" (spsr));
}

#define PREFETCH_ADDR ((void(**)(void))0x2C)

void dbg_init(void) {
		// 53 columns, 15 rows. 0/110px offset for x/y. Background color 15 (white), foreground color 0 (black)
	nio_InitConsole(&dbg_csl, 53, 15, 0, 0, 15, 0);
	nio_DrawConsole(&dbg_csl);
	orig_prefetch_abort_addr = *PREFETCH_ADDR;
	*PREFETCH_ADDR = prefetch_abort_handler;
	memset(breakpoints, 0, sizeof(breakpoints));
	debug_next = NULL;
}

void dbg_cleanup(void) {
	*PREFETCH_ADDR = orig_prefetch_abort_addr;
	nio_CleanUp(&dbg_csl);
}
