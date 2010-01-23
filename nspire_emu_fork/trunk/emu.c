#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

#include "emu.h"

/* cycle_count_delta is a (usually negative) number telling what the time is relative
 * to the next timer interrupt that will be executed.
 * (cycle_count + cycle_count_delta) is the total number of cycles executed so far */
u64 cycle_count = 0;
int cycle_count_delta = 0;

u32 cpu_events;

bool exiting;
bool do_translate = true;
bool emulate_cas;
bool emulate_ti84_keypad;
bool turbo_mode;
bool show_speed;

const char log_type_tbl[] = LOG_TYPE_TBL;
int log_enabled[MAX_LOG];
FILE *log_file[MAX_LOG];
void logprintf(int type, char *str, ...) {
	if (log_enabled[type]) {
		va_list va;
		va_start(va, str);
		vfprintf(log_file[type], str, va);
		va_end(va);
	}
}

volatile u16 key_map[9];

void warn(char *fmt, ...) {
	va_list va;
	fprintf(stderr, "Warning at PC=%08X: ", arm.reg[15]);
	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);
	fprintf(stderr, "\n");
}

__attribute__((noreturn))
void error(char *fmt, ...) {
	va_list va;
	fprintf(stderr, "Error at PC=%08X: ", arm.reg[15]);
	va_start(va, fmt);
	vfprintf(stderr, fmt, va);
	va_end(va);
	fprintf(stderr, "\n\tBacktrace:\n");
	backtrace(arm.reg[11]);
	debugger();
	exit(1);
}

HANDLE hTimerEvent;
UINT uTimerID;
void timer_on() {
	uTimerID = timeSetEvent(10, 10, (LPTIMECALLBACK)hTimerEvent, 0,
	                        TIME_PERIODIC | TIME_CALLBACK_EVENT_SET);
	if (uTimerID == 0) {
		printf("timeSetEvent failed\n");
		exit(1);
	}
}
void timer_off() {
	if (uTimerID != 0)
		timeKillEvent(uTimerID);
	uTimerID = 0;
}

int exec_hack() {
	u32 pc = arm.reg[15];
	//if (pc == 0x1194D0A8) {
	//	arm.reg[0] = 1000000;	// navnet log level
	//	return 0;
	//} else
	if (pc == usblink_addr_schedule) {
		usblink_hook_schedule();
		return 1;
	} else if (pc == usblink_addr_submit_read_buffer) {
		usblink_hook_submit_read_buffer();
		return 1;
	} else if (pc == usblink_addr_submit_write_buffer) {
		usblink_hook_submit_write_buffer();
		return 1;
	}
	return 0;
}

int main(int argc, char **argv) {
	int i;
	FILE *f;

	char *boot1_filename = NULL, *boot2_filename = NULL;
	char *debug_cmds_filename = NULL;
	bool new_flash_image = false;
	int gdb_port = 0;

	char *preload_boot2 = NULL, *preload_diags = NULL, *preload_os = NULL;

	for (i = 1; i < argc; i++) {
		char *arg = argv[i];
		if (*arg == '/' || *arg == '-') {
			arg++;
			switch (toupper(*arg++)) {
				case '1':
					if (*arg == '=') arg++;
					boot1_filename = arg;
					break;
				case 'B':
					if (*arg == '=') arg++;
					boot2_filename = arg;
					break;
				case 'C':
					if (*arg) goto usage;
					emulate_cas = true;
					break;
				case 'D':
					if (*arg) goto usage;
					cpu_events |= EVENT_DEBUG_STEP;
					break;
				case 'F':
					if (*arg == '=') arg++;
					flash_filename = arg;
					break;
				case 'G':
					if (*arg == '=') arg++;
					gdb_port = atoi(arg);
					if (!gdb_port) {
						printf("Invalid listen port for GDB stub%s%s\n", *arg ? ": " : "", arg);
						exit(1);
					}
					gdb_debugger = true;
					break;
				case 'K':
					if (*arg) goto usage;
					emulate_ti84_keypad = true;
					break;
				case 'L': {
					char *p = strchr(log_type_tbl, toupper(*arg++));
					int type = p - log_type_tbl;
					if (!p) goto usage;
					log_enabled[type] = 1;
					if (arg[0] == '=') {
						char *p2 = strchr(log_type_tbl, toupper(arg[1]));
						int type2 = p2 - log_type_tbl;
						if (!p2) goto usage;
						log_file[type] = log_file[type2];
					} else if (strcmp(arg, "-") == 0) {
						log_file[type] = stdout;
					} else {
						log_file[type] = fopen(arg, "wb");
					}
					if (!log_file[type]) {
						perror(arg);
						exit(1);
					}
					break;
				}
				case 'N':
					if (*arg) goto usage;
					new_flash_image = true;
					break;
				case 'P': {
					char **pp;
					switch (toupper(*arg++)) {
						case 'B': pp = &preload_boot2; break;
						case 'D': pp = &preload_diags; break;
						case 'O': pp = &preload_os; break;
						default: goto usage;
					}
					if (*arg == '=') arg++;
					*pp = arg;
					break;
				}
				case 'R':
					cpu_events |= EVENT_DEBUG_STEP;
					if (*arg == '=') arg++;
					debug_cmds_filename = arg;
					break;
				default:
usage:
					printf(
						"nspire emulator v0.20\n"
						"  /1=boot1      - location of BOOT1 image\n"
						"  /B=boot2      - location of decompressed BOOT2 image\n"
						"  /C            - emulate CAS hardware version\n"
						"  /D            - enter debugger at start\n"
						"  /F=file       - flash image filename\n"
						"  /G=port       - debug with external GDB through TCP\n"
						"  /K            - emulate TI-84+ keypad\n"
						"  /N            - create new flash image\n"
						"  /PB=boot2.img - preload flash with BOOT2 (.img file)\n"
						"  /PO=osfile    - preload flash with OS (.tnc/.tno file)\n"
						"  /R=cmdfile    - run debugger commands at start\n");
					return 1;
			}
		} else {
			goto usage;
		}
	}

	switch ((boot1_filename != NULL) + (boot2_filename != NULL) + new_flash_image) {
		case 0: goto usage;
		case 1: break;
		default: printf("Must use exactly one of /1, /B, or /N.\n"); return 0;
	}

	if (new_flash_image) {
		flash_initialize(preload_boot2, preload_diags, preload_os);
		f = fopen(flash_filename, "wb");
		if (!f) {
			perror(flash_filename);
			return 1;
		}
		if (!fwrite(flash_data, sizeof flash_data, 1, f)) {
			printf("Could not write flash data to %s\n", flash_filename);
			return 1;
		}
		fclose(f);
		printf("Created flash image %s\n", flash_filename);
		return 0;
	}

	if (flash_filename) {
		if (preload_boot2 || preload_diags || preload_os) {
			printf("Can't preload to an existing flash image\n");
			return 1;
		}
		f = fopen(flash_filename, "rb");
		if (!f) {
			perror(flash_filename);
			return 1;
		}
		if (!fread(flash_data, sizeof flash_data, 1, f)) {
			printf("Could not read flash image from %s\n", flash_filename);
			return 1;
		}
		fclose(f);
	} else {
		flash_initialize(preload_boot2, preload_diags, preload_os);
	}

	if (boot1_filename) {
		/* Load the ROM */
		f = fopen(boot1_filename, "rb");
		if (!f) {
			perror(boot1_filename);
			return 1;
		}
		fread(RAM_PTR(0x00000000), 1, 0x80000, f);
		fclose(f);
	}
	
	if (debug_cmds_filename) {
		debugger_stdin = fopen(debug_cmds_filename, "rt");
		if (!debugger_stdin) {
			perror(debug_cmds_filename);
			return 1;
		}
}

	memory_initialize();

	LARGE_INTEGER perffreq;
	QueryPerformanceFrequency(&perffreq);

	hTimerEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	gui_initialize();

	timer_on();
	atexit(timer_off);
	int ticks = 0;
	//FILE *untrans = fopen("untrans.out", "wb");

reset:
	memset(arm.reg, 0x55, sizeof arm.reg);
	arm.cpsr_low28 = MODE_SVC | 0xC0;
	cpu_events &= ~(EVENT_IRQ | EVENT_RESET);
	if (boot2_filename) {
		/* Start from BOOT2. (needs to be re-loaded on each reset since
		 * it can get overwritten in memory) */
		f = fopen(boot2_filename, "rb");
		if (!f) {
			perror(boot2_filename);
			return 1;
		}
		fseek(f, 0, SEEK_SET);
		fread(RAM_PTR(0x11800000), 1, 0x800000, f);
		fclose(f);
		arm.reg[15] = 0x11800000;

		/* To enter maintenance mode (home+enter+P), address A4012ECC
		 * must contain an array indicating those keys before BOOT2 starts */
		memcpy(RAM_PTR(0xA4012ECC), (void *)key_map, 0x12);
	} else {
		/* Start from BOOT1. */
		arm.reg[15] = 0;

		/* Kluge around the fact that we have exceptions jump to A40000xx
		 * instead of 000000xx like they're supposed to (see cpu_exception)*/
		memcpy(RAM_PTR(0xA4000000), RAM_PTR(0x00000000), 0x40);
	}

	if (gdb_debugger)
		gdbstub_init(gdb_port);

	while (!exiting) {
		while (cycle_count_delta < 0) {
			u32 pc = arm.reg[15];
			u32 *insnp = RAM_PTR(pc);
			if (!insnp)
				error("Bad PC: %08x\n", pc);

			u32 flags = RAM_FLAGS(insnp);

			if (cpu_events != 0) {
				if (cpu_events & EVENT_IRQ) {
					logprintf(LOG_IRQS, "Dispatching an IRQ\n");
					arm.reg[15] += 4;
					cpu_exception(EX_IRQ);
					continue;
				}
				if (cpu_events & EVENT_RESET) {
					printf("CPU reset\n");
					goto reset;
				}
				if (cpu_events & EVENT_DEBUG_STEP) {
					goto enter_debugger;
				}
			}

			if (flags & RF_CODE_TRANSLATED) {
				struct translation *tt = &translation_table[flags >> RFS_TRANSLATION_INDEX];
				cycle_count_delta += (tt->end_addr - pc) >> 2;
				arm.reg[15] = tt->code(pc);
			} else {
				if (flags & (RF_EXEC_BREAKPOINT | RF_EXEC_DEBUG_NEXT | RF_EXEC_HACK)) {
					if (flags & (RF_EXEC_BREAKPOINT | RF_EXEC_DEBUG_NEXT)) {
						if (flags & RF_EXEC_BREAKPOINT)
							printf("Hit breakpoint at %08X. Entering debugger.\n", pc);
enter_debugger:
						debugger();
					}
					if (flags & RF_EXEC_HACK)
						if (exec_hack())
							continue;
				} else {
					if (do_translate && !(flags & (RF_CODE_NO_TRANSLATE))) {
						translate(pc);
						continue;
					}
				}
				//fwrite(insnp, 4, 1, untrans);
				arm.reg[15] += 4;
				cpu_interpret_instruction(*insnp);
				cycle_count_delta++;
			}
		}

		logprintf(LOG_IRQS, "Timer tick %d @ %d (pc=%x)\n", ++ticks, GetTickCount(), arm.reg[15]);
		irq_activate(1 << IRQ_TIMER);

		if (reg_900A0004 & 0x80) // just guessing here...
			irq_activate(1 << IRQ_APDTMR);

		if (_kbhit()) {
			char c = _getch();
			if (c == 4)
				debugger();
			else
				serial_byte_in(c);
		}

		cycle_count       += 900000;
		cycle_count_delta -= 900000;

		get_messages();

		LARGE_INTEGER tick_end;
		QueryPerformanceCounter(&tick_end);

		{	// Update graphics (frame rate is arbitrary)
			static LARGE_INTEGER prev;
			LONGLONG time = tick_end.QuadPart - prev.QuadPart;
			if (time >= (perffreq.QuadPart >> 5)) {
				InvalidateRect(hwndMain, NULL, FALSE);
				prev = tick_end;
			}
		}

		if (show_speed) {
			// Show speed
			static int prev_ticks;
			static LARGE_INTEGER prev;
			LONGLONG time = tick_end.QuadPart - prev.QuadPart;
			if (time >= (perffreq.QuadPart >> 1)) {
				double speed = (double)perffreq.QuadPart * (ticks - prev_ticks) / time;
				char buf[40];
				sprintf(buf, "nspire_emu - %.1f%%", speed);
				SetWindowText(hwndMain, buf);
				prev_ticks = ticks;
				prev = tick_end;
			}
		}

		if (!turbo_mode)
			WaitForSingleObject(hTimerEvent, INFINITE);

		if (log_enabled[LOG_ICOUNT]) {
			static LARGE_INTEGER tick_start;
			LARGE_INTEGER nexttick_start;
			QueryPerformanceCounter(&nexttick_start);

			static u64 prev_cycles;
			u64 cycles = cycle_count + cycle_count_delta;
			if ((ticks & 31) == 16)
			logprintf(LOG_ICOUNT, "Time=%7d (CPU=%7d Idle=%7d) Insns=%8d Rate=%9I64d\n",
				nexttick_start.LowPart - tick_start.LowPart,
				tick_end.LowPart - tick_start.LowPart,
				nexttick_start.LowPart - tick_end.LowPart,
				(u32)(cycles - prev_cycles),
				(cycles - prev_cycles) * perffreq.QuadPart / (tick_end.LowPart - tick_start.LowPart));
			prev_cycles = cycles;
			tick_start = nexttick_start;
		}
	}
	return 0;
}
