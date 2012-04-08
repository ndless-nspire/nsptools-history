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

static void disasm(u32 (*dis_func)(u32 pc)) {
	char *arg = strtok(NULL, " \n");
	u32 addr = arg ? parse_expr(arg) : regs[15];
	int i;
	for (i = 0; i < 8; i++) {
		u32 len = dis_func(addr);
		if (!len) {
			printf("Address %08X is not in RAM.\n", addr);
			break;
		}
		addr += len;
	}
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

static void (*orig_prefetch_abort_addr)(void);
#define MAX_BKPTS 16
// BCONT is used only used to restore the previous breakpoint removed to continue
enum bkpt_type {BSTD = 0, BTMP, BCONT};
struct breakpoint {
	BOOL used;
	enum bkpt_type type;
	unsigned addr;
	unsigned orig_instr;
};
static struct breakpoint breakpoints[MAX_BKPTS];


// Local frame not available for naked functions. non-static so that it's not inlined.
// Returns the return address to jump to.
void *dbg_prefetch_abort_handler_body(unsigned *exception_addr) {
	int bkpt_num = ((*exception_addr & 0xFFF00) >> 4) | (*exception_addr & 0xF);
	if (bkpt_num > MAX_BKPTS) {
		bkpt_num = -1;
		nio_printf(&dbg_csl, "Explicit breakpoint hit at %08x\n", exception_addr);
	} else
		nio_printf(&dbg_csl, "Breakpoint #%u hit at %08x\n", bkpt_num, exception_addr);

	char line[100];
	while (1) {
		char *cmd;
		nio_printf(&dbg_csl, "> ");
		if (!nio_GetStr(&dbg_csl, line))
			continue;
		cmd = strtok(line, " ");
		if (!strcmp("c", cmd)) {
			nio_printf(&dbg_csl, "Continuing...\n");
			if (bkpt_num >= 0) {
				*exception_addr = breakpoints[bkpt_num].orig_instr;
				clear_cache();
				return (void*)exception_addr; // orig instr
			} else {
				return (void*)(exception_addr + 1); // next insttr
			}
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
			disasm(disasm_insn);
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
		" adr r0, prefetch_abort_handler_exception_addr \n" 
		" ldr %0, [r0] \n" 
		" str %0, [sp, #15*4] \n" /* to the reg list, replacing pc */
		: "=r" (exception_addr), "=r" (regs));
	if ((*exception_addr & 0xFFF000F0) != 0xE1200070) // bkpt instr?
		reboot(); // no
	__asm volatile(" str %0, [sp, #15*4] \n" /* overwrite pc */
		" adr r0, saved_spsr \n" 
		" ldr r0, [r0] \n"
		" msr cpsr, r0 \n"
		" ldmfd sp!, {r0-r12} \n"
		" add sp, sp, #4 \n" /* don't restore sp from the reg list */
		" ldmfd sp!, {lr, pc}^ \n"
		: : "r" (dbg_prefetch_abort_handler_body(exception_addr)));
}

#define PREFETCH_ADDR ((void(**)(void))0x2C)

void dbg_init(void) {
		// 53 columns, 15 rows. 0/110px offset for x/y. Background color 15 (white), foreground color 0 (black)
	nio_InitConsole(&dbg_csl, 53, 15, 0, 0, 15, 0);
	nio_DrawConsole(&dbg_csl);
	orig_prefetch_abort_addr = *PREFETCH_ADDR;
	*PREFETCH_ADDR = prefetch_abort_handler;
	memset(breakpoints, 0, sizeof(breakpoints));
}

void dbg_cleanup(void) {
	*PREFETCH_ADDR = orig_prefetch_abort_addr;
	nio_CleanUp(&dbg_csl);
}

void dbg_set_breakpoint(unsigned addr) {
	unsigned i = 0;
	for (i = 0; i < MAX_BKPTS; i++) {
		if (!breakpoints[i].used) break;
	}
	if (i >= MAX_BKPTS) return;
	breakpoints[i].used = TRUE;
	breakpoints[i].type = BSTD;
	breakpoints[i].addr = addr;
	breakpoints[i].orig_instr = *(unsigned*)addr;
	*(unsigned*)addr = 0xE1200070 | i; // 'bkpt #i'
	clear_cache();
}
