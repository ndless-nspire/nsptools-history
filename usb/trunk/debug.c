#include <os.h>
#include "nspireio2.h"

void dbg_dump(nio_console *c, void *addr) {
	clrscr();
	unsigned i, j;
	for (i = 0; i <8; i++) {
		nio_printf(c, "\n%p  ", addr);
		for (j = 0; j < 8; j++) {
			nio_printf(c, "%02X ", (unsigned)*(unsigned char *)addr++);
		}
	}
	nio_printf(c, "\n");
}

static nio_console dbg_csl;
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


// local frame not available for naked functions. non-static so that it's not inlined.
void dbg_prefetch_abort_handler_body(unsigned *exception_addr) {
	//int bkpt_num = ((*exception_addr & 0xFFF00) >> 4) | (*exception_addr & 0xF);
	asm(".long 0xE12FFF7E"); // immed = FFFF
	puts("hello");
	halt();
nio_printf(&dbg_csl, "hello"); // broken because of SWI ?
	
	int bkpt_num = 255;
	if (bkpt_num > MAX_BKPTS) {
		bkpt_num = -1;
		nio_printf(&dbg_csl, "Explicit breakpoint hit at address %08x", exception_addr);
	} else
		nio_printf(&dbg_csl, "Breakpoint #%u hit at address %08x", bkpt_num, exception_addr);


	char input[100];
	while (1) {
		while (!nio_GetStr(&dbg_csl, input)) ;
		if (!strcmp("c", input)) {
			nio_printf(&dbg_csl, "Continuing...");
			if (bkpt_num >= 0) {
				*exception_addr = breakpoints[bkpt_num].orig_instr;
				clear_cache(); // TODO set lr/pc
			}
			break;
		}
	}
}

// triggered by bkpt instructions
static void __attribute__((naked)) prefetch_abort_handler(void) {
	__asm volatile(" stmfd sp!, {r0-r12, lr}; sub lr, lr, #4");
	unsigned *exception_addr;
	__asm volatile(" mov %0, lr" : "=r" (exception_addr));
	if ((*exception_addr & 0xFFF000F0) == 0xE1200070) // bkpt instr?
		dbg_prefetch_abort_handler_body(exception_addr);
	__asm volatile(" ldmfd sp!, {r0-r12, pc}^");
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
