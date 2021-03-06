#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emu.h"

#include "disasm.c"

void backtrace(u32 fp) {
	u32 *frame;
	printf("Frame     PrvFrame Self     Return   Start\n");
	do {
		printf("%08X:", fp);
		frame = ram_ptr(fp - 12, 16);
		if (!frame) {
			printf(" invalid address\n");
			break;
		}
		vprintf(" %08X %08X %08X %08X\n", (va_list)frame);
		if (frame[0] <= fp) /* don't get stuck in infinite loop :) */
			break;
		fp = frame[0];
	} while (frame[2] != 0);
}

static void dump(u32 addr) {
	u32 start = addr;
	u32 end = addr + 0x7F;

	u32 row, col;
	for (row = start & ~0xF; row <= end; row += 0x10) {
		u8 *ptr = ram_ptr(row, 16);
		if (!ptr) {
			printf("Address %08X is not in RAM.\n", row);
			break;
		}
		printf("%08X  ", row);
		for (col = 0; col < 0x10; col++) {
			addr = row + col;
			if (addr < start || addr > end)
				printf("  ");
			else
				printf("%02X", ptr[col]);
			putchar(col == 7 && addr >= start && addr < end ? '-' : ' ');
		}
		printf("  ");
		for (col = 0; col < 0x10; col++) {
			addr = row + col;
			if (addr < start || addr > end)
				putchar(' ');
			else if (ptr[col] < 0x20)
				putchar('.');
			else
				putchar(ptr[col]);
		}
		putchar('\n');
	}
}

u32 debug_next_brkpt_adr;
/* if next_adr is null, simply clears the 'next' breakpoint */
void debug_set_next_brkpt(u32 next_adr) {
	if (debug_next_brkpt_adr != 0)
		RAM_FLAGS(ram_ptr(debug_next_brkpt_adr, 4)) &= ~RF_EXEC_DEBUG_NEXT;
	if (next_adr != 0) {
		if (RAM_FLAGS(ram_ptr(next_adr, 4)) & RF_CODE_TRANSLATED)
			flush_translations();
		RAM_FLAGS(ram_ptr(next_adr, 4)) |= RF_EXEC_DEBUG_NEXT;
	}
	debug_next_brkpt_adr = next_adr;
}

bool is_gdb_debugger = false;

FILE *debugger_stdin;

static void native_debugger(void) {
	char line[80];
	char *cmd;
	bool show_insn = false;

	// Did we hit the "next" breakpoint?
	if (arm.reg[15] == debug_next_brkpt_adr) {
		debug_set_next_brkpt(0);
		show_insn = 1;
	}

	if (cpu_events & EVENT_DEBUG_STEP) {
		cpu_events &= ~EVENT_DEBUG_STEP;
		show_insn = 1;
	}

	if (show_insn) {
		u32 cur_insn = *(u32 *)ram_ptr(arm.reg[15], 4);
		disasm_arm_insn(arm.reg[15], cur_insn, line);
		printf("%08x: %08x\t%s\n", arm.reg[15], cur_insn, line);
	}

	timer_off();
	while (1) {
		printf("debug> ");
		if (!debugger_stdin)
			debugger_stdin = stdin;
readstdin:
		if (!fgets(line, sizeof line, debugger_stdin)) {
			if (debugger_stdin != stdin) {
				fclose(debugger_stdin);
				debugger_stdin = stdin;
				goto readstdin;
			}
			else
				exit(1);
		}
		if (debugger_stdin != stdin) {
			printf("%s\n", line);
		}
		cmd = strtok(line, " \n");
		if (!cmd) {
			continue;
		} else if (!stricmp(cmd, "?") || !stricmp(cmd, "h")) {
			printf(
				"Debugger commands:\n"
				"b - stack backtrace\n"
				"c - continue\n"
				"d <address> - dump memory\n"
				"k <address> <+r|+w|+x|-r|-w|-x> - add/remove breakpoint\n"
				"k - show breakpoints\n"
				"n - continue until next instruction\n"
				"q - quit\n"
				"r - show registers\n"
				"rs <regnum|pc> <value> - change register value\n"
				"s - step instruction\n"
				"t+ - enable instruction translation\n"
				"t- - disable instruction translation\n"
				"u [address] - disassemble memory\n"
				"w <file> <start> <size> - write memory to file\n");
		} else if (!stricmp(cmd, "b")) {
			char *fp = strtok(NULL, " \n");
			backtrace(fp ? strtoul(fp, 0, 16) : arm.reg[11]);
		} else if (!stricmp(cmd, "r")) {
			int i, show_spsr;
			u32 cpsr = get_cpsr();
			char *mode;
			for (i = 0; i < 16; i++) {
				int newline = ((1 << 5) | (1 << 11) | (1 << 15)) & (1 << i);
				printf("%3s=%08x%c", reg_name[i], arm.reg[i], newline ? '\n' : ' ');
			}
			switch (cpsr & 0x1F) {
				case MODE_USR: mode = "usr"; show_spsr = 0; break;
				case MODE_SYS: mode = "sys"; show_spsr = 0; break;
				case MODE_FIQ: mode = "fiq"; show_spsr = 1; break;
				case MODE_IRQ: mode = "irq"; show_spsr = 1; break;
				case MODE_SVC: mode = "svc"; show_spsr = 1; break;
				case MODE_ABT: mode = "abt"; show_spsr = 1; break;
				case MODE_UND: mode = "und"; show_spsr = 1; break;
				default:       mode = "???"; show_spsr = 0; break;
			}
			printf("cpsr=%08x (N=%d Z=%d C=%d V=%d Q=%d IRQ=%s FIQ=%s Mode=%s)",
				cpsr,
				arm.cpsr_n, arm.cpsr_z, arm.cpsr_c, arm.cpsr_v,
				cpsr >> 27 & 1,
				(cpsr & 0x80) ? "off" : "on ",
				(cpsr & 0x40) ? "off" : "on ",
				mode);
			if (show_spsr)
				printf(" spsr=%08x", get_spsr());
			printf("\n");
		} else if (!stricmp(cmd, "rs")) {
		  char *reg = strtok(NULL, " \n");
		  if (!reg)
		    printf("Parameters are missing.\n");
		  else {
		    char *value = strtok(NULL, " \n");
		    if (!value)
		      printf("Missing value parameter.\n");
		    else {
    			int regi = atoi(reg);
    			int valuei = strtoul(value, NULL, 16);
    			if (!strcmp(reg, "pc"))
    			  arm.reg[15] = valuei;
  			  else if (regi >= 0 && regi < 15)
  				  arm.reg[regi] = valuei;
  			  else
  			    printf("Invalid register.\n");
  			}
  		}
		} else if (!stricmp(cmd, "k")) {
			char *addr_str = strtok(NULL, " \n");
			char *flag_str = strtok(NULL, " \n");
			if (addr_str && flag_str) {
				u32 addr = strtoul(addr_str, 0, 16);
				void *ptr = ram_ptr(addr & ~3, 4);
				if (ptr) {
					u32 *flags = &RAM_FLAGS(ptr);
					bool on = true;
					for (; *flag_str; flag_str++) {
						switch (tolower(*flag_str)) {
							case '+': on = true; break;
							case '-': on = false; break;
							case 'r':
								if (on) *flags |= RF_READ_BREAKPOINT;
								else *flags &= ~RF_READ_BREAKPOINT;
								break;
							case 'w':
								if (on) *flags |= RF_WRITE_BREAKPOINT;
								else *flags &= ~RF_WRITE_BREAKPOINT;
								break;
							case 'x':
								if (on) {
									if (*flags & RF_CODE_TRANSLATED) flush_translations();
									*flags |= RF_EXEC_BREAKPOINT;
								} else
									*flags &= ~RF_EXEC_BREAKPOINT;
								break;
						}
					}
				} else {
					printf("Address %08X is not in RAM.\n", addr);
				}
			} else {
				int area;
				for (area = 0; area < 3; area++) {
					u32 *flags;
					u32 *flags_start = &RAM_FLAGS(ram_areas[area].ptr);
					u32 *flags_end = &RAM_FLAGS(ram_areas[area].ptr + ram_areas[area].size);
					for (flags = flags_start; flags != flags_end; flags++) {
						if (*flags & (RF_READ_BREAKPOINT | RF_WRITE_BREAKPOINT | RF_EXEC_BREAKPOINT)) {
							printf("%08x %c%c%c\n",
								ram_areas[area].base + ((u8 *)flags - (u8 *)flags_start),
								(*flags & RF_READ_BREAKPOINT)  ? 'r' : ' ',
								(*flags & RF_WRITE_BREAKPOINT) ? 'w' : ' ',
								(*flags & RF_EXEC_BREAKPOINT)  ? 'x' : ' ');
						}
					}
				}
			}
		} else if (!stricmp(cmd, "c")) {
			break;
		} else if (!stricmp(cmd, "s")) {
			cpu_events |= EVENT_DEBUG_STEP;
			break;
		} else if (!stricmp(cmd, "n")) {
			debug_set_next_brkpt(arm.reg[15] + 4);
			break;
		} else if (!stricmp(cmd, "d")) {
			char *arg = strtok(NULL, " \n");
			if (!arg)
				printf("Missing address parameter.\n");
			else {
				u32 addr = strtoul(arg, 0, 16);
				dump(addr);
			}
		} else if (!stricmp(cmd, "u")) {
			char *arg = strtok(NULL, " \n");
			u32 addr = arg ? strtoul(arg, 0, 16) & ~3 : arm.reg[15];
			int i;
			for (i = 0; i < 16; i++) {
				u32 *insnp = (u32 *)ram_ptr(addr, 4);
				if (!insnp) {
					printf("Address %08X is not in RAM.\n", addr);
					break;
				}
				disasm_arm_insn(addr, *insnp, line);
				printf("%08x: %08x\t%s\n", addr, *insnp, line);
				addr += 4;
			}
		} else if (!stricmp(cmd, "taskinfo")) {
			u32 task = strtoul(strtok(NULL, " \n"), 0, 16);
			u8 *p = ram_ptr(task, 52);
			if (p) {
				printf("Previous:        %08x\n", *(u32 *)&p[0]);
				printf("Next:            %08x\n", *(u32 *)&p[4]);
				printf("ID:              %c%c%c%c\n", p[15], p[14], p[13], p[12]);
				printf("Name:            %.8s\n", &p[16]);
				printf("Status:          %02x\n", p[24]);
				printf("Delayed suspend: %d\n", p[25]);
				printf("Priority:        %02x\n", p[26]);
				printf("Preemption:      %d\n", p[27]);
				printf("Stack start:     %08x\n", *(u32 *)&p[36]);
				printf("Stack end:       %08x\n", *(u32 *)&p[40]);
				printf("Stack pointer:   %08x\n", *(u32 *)&p[44]);
				printf("Stack size:      %08x\n", *(u32 *)&p[48]);
				u32 sp = *(u32 *)&p[44];
				u32 *psp = ram_ptr(sp, 18 * 4);
				if (psp) {
					printf("Stack type:      %d (%s)\n", psp[0], psp[0] ? "Interrupt" : "Normal");
					if (psp[0]) {
						vprintf("cpsr=%08x  r0=%08x r1=%08x r2=%08x r3=%08x  r4=%08x\n"
						        "  r5=%08x  r6=%08x r7=%08x r8=%08x r9=%08x r10=%08x\n"
						        " r11=%08x r12=%08x sp=%08x lr=%08x pc=%08x\n",
							(va_list)&psp[1]);
					} else {
						vprintf("cpsr=%08x  r4=%08x  r5=%08x  r6=%08x r7=%08x r8=%08x\n"
						        "  r9=%08x r10=%08x r11=%08x r12=%08x pc=%08x\n",
							(va_list)&psp[1]);
					}
				}
			}
		} else if (!stricmp(cmd, "tasklist")) {
			u32 tasklist = strtoul(strtok(NULL, " \n"), 0, 16);
			u8 *p = ram_ptr(tasklist, 4);
			if (p) {
				u32 first = *(u32 *)p;
				u32 task = first;
				printf("Task      ID   Name     St D Pr P | StkStart StkEnd   StkPtr   StkSize\n");
				do {
					p = ram_ptr(task, 52);
					if (!p)
						break;
					printf("%08X: %c%c%c%c %-8.8s %02x %d %02x %d | %08x %08x %08x %08x\n",
						task, p[15], p[14], p[13], p[12],
						&p[16], /* name */
						p[24],  /* status */
						p[25],  /* delayed suspend */
						p[26],  /* priority */
						p[27],  /* preemption */
						*(u32 *)&p[36], /* stack start */
						*(u32 *)&p[40], /* stack end */
						*(u32 *)&p[44], /* stack pointer */
						*(u32 *)&p[48]  /* stack size */
						);
					task = *(u32 *)&p[4]; /* next */
				} while (task != first);
			}
		} else if (!stricmp(cmd, "t+")) {
			do_translate = 1;
		} else if (!stricmp(cmd, "t-")) {
			flush_translations();
			do_translate = 0;
		} else if (!stricmp(cmd, "q")) {
			exit(1);
		} else if (!stricmp(cmd, "w")) {
			char *filename = strtok(NULL, " \n");
			u32 start = strtoul(strtok(NULL, " \n"), 0, 16);
			u32 size = strtoul(strtok(NULL, " \n"), 0, 16);
			void *ram = ram_ptr(start, size);
			if (!ram) {
				printf("Address range %08x-%08x is not in RAM.\n", start, start + size - 1);
				continue;
			}
			FILE *f = fopen(filename, "wb");
			if (!f || (!fwrite(ram, size, 1, f) | fclose(f))) {
				perror(filename);
				continue;
			}
		} else if (!stricmp(cmd, "irq")) {
			extern u32 current_irqs;
			extern u32 active_irqs;
			extern u32 enabled_irqs;
			printf("current=%08x active=%08x enabled=%08x\n",
				current_irqs, active_irqs, enabled_irqs);
		} else if (!stricmp(cmd, "irq+")) {
			irq_activate(1 << atoi(strtok(NULL, " \n")));
		} else if (!stricmp(cmd, "irq-")) {
			irq_deactivate(1 << atoi(strtok(NULL, " \n")));
		} else {
			printf("Unknown command %s\n", cmd);
		}
	}
	timer_on();
}

void debugger(void) {
	if (is_gdb_debugger)
		gdbstub_debugger();
	else
		native_debugger();
}
