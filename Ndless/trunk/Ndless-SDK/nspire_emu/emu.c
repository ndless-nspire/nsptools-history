#include <conio.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emu.h"
#include "os-win32.h"

/* cycle_count_delta is a (usually negative) number telling what the time is relative
 * to the next scheduled event. See sched.c */
int cycle_count_delta = 0;

int throttle_delay = 10; /* in milliseconds */

u32 cpu_events;

bool exiting;
bool do_translate = true;
int product = 0x0E;
int asic_user_flags;
bool emulate_cx;

bool turbo_mode;
bool is_halting;
bool show_speed;

jmp_buf restart_after_exception;

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
	cpu_events |= EVENT_RESET;
	longjmp(restart_after_exception, 0);
}

int exec_hack() {
	if (arm.reg[15] == 0x10040) {
		arm.reg[15] = arm.reg[14];
		warn("BOOT1 is required to run this version of BOOT2.");
		return 1;
	}
	return 0;
}

void prefetch_abort(u32 mva, u8 status) {
	logprintf(LOG_CPU, "Prefetch abort: address=%08x status=%02x\n", mva, status);
	arm.reg[15] += 4;
	// Fault address register not changed
	arm.instruction_fault_status = status;
	cpu_exception(EX_PREFETCH_ABORT);
	if (mva == arm.reg[15])
		error("Abort occurred with exception vectors unmapped");
	longjmp(restart_after_exception, 0);
}

void data_abort(u32 mva, u8 status) {
	logprintf(LOG_CPU, "Data abort: address=%08x status=%02x\n", mva, status);
	fix_pc_for_fault();
	arm.reg[15] += 8;
	arm.fault_address = mva;
	arm.data_fault_status = status;
	cpu_exception(EX_DATA_ABORT);
	longjmp(restart_after_exception, 0);
}

os_frequency_t perffreq;

void throttle_interval_event(int index) {
	event_repeat(index, 27000000 / 100);

	/* Throttle interval (defined arbitrarily as 100Hz) - used for
	 * keeping the emulator speed down, and other miscellaneous stuff
	 * that needs to be done periodically */
	static int intervals;
	intervals++;

	extern void usblink_timer();
	usblink_timer();

	if (_kbhit()) {
		char c = _getch();
		if (c == 4)
			debugger();
		else
			serial_byte_in(c);
	}

	get_messages();

	os_time_t interval_end;
	os_query_time(interval_end);

	{	// Update graphics (frame rate is arbitrary)
		static os_time_t prev;
		s64 time = os_time_diff(interval_end, prev);
		if (time >= os_frequency_hz(perffreq) >> 5) {
			os_redraw_screen();
			prev = interval_end;
		}
	}

	if (show_speed) {
		// Show speed
		static int prev_intervals;
		static os_time_t prev;
		s64 time = os_time_diff(interval_end, prev);
		if (time >= os_frequency_hz(perffreq) >> 1) {
			double speed = (double)os_frequency_hz(perffreq) * (intervals - prev_intervals) / time;
			char buf[40];
			sprintf(buf, "nspire_emu - %.1f%%", speed);
			os_set_window_title(buf);
			prev_intervals = intervals;
			prev = interval_end;
		}
	}
	if (!turbo_mode || is_halting)
		throttle_timer_wait();
	if (is_halting)
		is_halting--;
}

int main(int argc, char **argv) {
	int i;
	static FILE *boot2_file = NULL;
	static char *boot1_filename = NULL, *boot2_filename = NULL, *flash_filename = NULL;
	char *preload_filename[4] = { NULL };
	bool preload = false;
	volatile u32 boot2_base = 0x11800000;
	u32 sdram_size;
	bool large_sdram = false;
	bool large_flash = false;

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
					if (*arg == '@') {
						boot2_base = 0x10000000 | (strtoul(arg + 1, &arg, 16) & 0x01FFFFFF);
					}
					if (*arg == '=') arg++;
					boot2_filename = arg;
					boot2_file = fopen(boot2_filename, "rb");
					if (!boot2_file) {
						perror(boot2_filename);
						return 1;
					}
					break;
				case 'D':
					if (*arg) goto usage;
					cpu_events |= EVENT_DEBUG_STEP;
					break;
				case 'F':
					if (*arg == '=') arg++;
					flash_filename = arg;
					break;
				case 'K':
					keypad_type = 1;
					if (*arg) {
						keypad_type = atoi(arg) - 1;
						if (keypad_type < 0 || keypad_type >= NUM_KEYPAD_TYPES)
							goto usage;
					}
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
				case 'M':
					switch (toupper(*arg)) {
						case 'L': product = 0x0D; arg++; goto nocas;
						case 'X': product = 0x10; arg++; break;
						case 'M': product = 0x12; arg++; break;
					}
					if (toupper(*arg) == 'C') { // CAS
						product -= (product == 0x0E ? 2 : 1);
						arg++;
					}
				nocas:
					if (*arg) goto usage;
					break;
				case 'N':
					if (*arg) goto usage;
					large_flash = true;
					break;
				case 'P': {
					static const char tbl[] = "MBDO";
					char *p = strchr(tbl, toupper(*arg++));
					if (!p) goto usage;
					if (*arg == '=') arg++;
					preload_filename[p - tbl] = arg;
					preload = true;
					break;
				}
				case 'R':
					if (*arg) goto usage;
					large_sdram = true;
					break;
				default:
usage:
					printf(
						"nspire emulator v0.60\n"
						"  /1=boot1	- location of BOOT1 image\n"
						"  /B=boot2	- location of decompressed BOOT2 image\n"
						"  /D		- enter debugger at start\n"
						"  /F=file	- flash image filename\n"
						"  /Kn		- set keypad type (2 = TI-84 Plus, 4 = Touchpad)\n"
						"  /M[X|M][C]	- set model (original/CX/CM, non-CAS/CAS)\n"
						"  /N		- large NAND flash size\n"
						"  /PB=boot2.img	- preload flash with BOOT2 (.img file)\n"
						"  /PD=diags.img	- preload flash with DIAGS image\n"
						"  /PO=osfile	- preload flash with OS (.tnc/.tno file)\n"
						"  /R		- large SDRAM size\n");
					return 1;
			}
		} else {
			goto usage;
		}
	}

	switch ((boot1_filename != NULL) + (boot2_filename != NULL)) {
		case 0: goto usage;
		case 1: break;
		default: printf("Must use exactly one of /1 or /B.\n"); return 0;
	}

	if (flash_filename) {
		if (preload) {
			printf("Can't preload to an existing flash image\n");
			return 1;
		}
		flash_open(flash_filename);
	} else {
		nand_initialize(large_flash);
		flash_create_new(preload_filename, product, large_sdram);
	}

	flash_read_settings(&sdram_size);

	memory_initialize(sdram_size);

	memset(MEM_PTR(0x00000000), -1, 0x80000);
	for (i = 0x00000; i < 0x80000; i += 4) {
		RAM_FLAGS(&rom_00[i]) = RF_READ_ONLY;
	}
	if (boot1_filename) {
		/* Load the ROM */
		FILE *f = fopen(boot1_filename, "rb");
		if (!f) {
			perror(boot1_filename);
			return 1;
		}
		fread(MEM_PTR(0x00000000), 1, 0x80000, f);
		fclose(f);
	}

	insn_buffer = os_alloc_executable(INSN_BUFFER_SIZE);
	insn_bufptr = insn_buffer;

	os_exception_frame_t frame;
	addr_cache_init(&frame);
	des_initialize();

	os_query_frequency(perffreq);

	gui_initialize();

	throttle_timer_on();
	atexit(throttle_timer_off);

reset:
	memset(&arm, 0, sizeof arm);
	arm.control = 0x00050078;
	arm.cpsr_low28 = MODE_SVC | 0xC0;
	cpu_events &= EVENT_DEBUG_STEP;
	if (boot2_file) {
		/* Start from BOOT2. (needs to be re-loaded on each reset since
		 * it can get overwritten in memory) */
		fseek(boot2_file, 0, SEEK_SET);
		fread(MEM_PTR(boot2_base), 1, 0x12000000 - boot2_base, boot2_file);
		arm.reg[15] = boot2_base;
		if (*(u8 *)MEM_PTR(boot2_base+3) < 0xE0) {
			printf("%s does not appear to be an uncompressed BOOT2 image.\n", boot2_filename);
			return 1;
		}

		/* To enter maintenance mode (home+enter+P), address A4012ECC
		 * must contain an array indicating those keys before BOOT2 starts */
		memcpy(MEM_PTR(0xA4012ECC), (void *)key_map, 0x12);

		/* BOOT1 is expected to store the address of a function pointer table
		 * to A4012EE8. OS 3.0 calls some of these functions... */
		static const struct {
			u32 ptrs[8];
			u16 code[16];
		} stuff = { {
			0x10020+0x01, // function 0: return *r0
			0x10020+0x05, // function 1: *r0 = r1
			0x10020+0x09, // function 2: *r0 |= r1
			0x10020+0x11, // function 3: *r0 &= ~r1
			0x10020+0x19, // function 4: *r0 ^= r1
			0x10020+0x20, // function 5: related to C801xxxx (not implemented)
			0x10020+0x03, // function 6: related to 9011xxxx (not implemented)
			0x10020+0x03, // function 7: related to 9011xxxx (not implemented)
		}, {
			0x6800,0x4770,               // return *r0
			0x6001,0x4770,               // *r0 = r1
			0x6802,0x430A,0x6002,0x4770, // *r0 |= r1
			0x6802,0x438A,0x6002,0x4770, // *r0 &= ~r1
			0x6802,0x404A,0x6002,0x4770, // *r0 ^= r1
		} };
		*(u32 *)MEM_PTR(0xA4012EE8) = 0x10000;
		memcpy(MEM_PTR(0x00010000), &stuff, sizeof stuff);
		RAM_FLAGS(MEM_PTR(0x00010040)) |= RF_EXEC_HACK;
	}
	addr_cache_flush();
	flush_translations();

	sched_reset();

	memset(&intr, 0, sizeof intr);
	intr.noninverted = -1;
	intr.priority_limit[0] = 8;
	intr.priority_limit[1] = 8;

	gpio_reset();
	keypad_reset();
	lcd_reset();
	pmu_reset();
	if (!emulate_cx)
		timer_reset();
	else
		timer_cx_reset();
	usb_reset();
	usblink_reset();
	watchdog_reset();

	sched_items[SCHED_THROTTLE].clock = CLOCK_27M;
	sched_items[SCHED_THROTTLE].proc = throttle_interval_event;

	sched_update_next_event(0);

	setjmp(restart_after_exception);

	while (!exiting) {
		sched_process_pending_events();
		while (cycle_count_delta < 0) {
			if (cpu_events & EVENT_RESET) {
				printf("Reset\n");
				goto reset;
			}

			if (cpu_events & (EVENT_FIQ | EVENT_IRQ)) {
				if (cpu_events & EVENT_WAITING)
					arm.reg[15] += 4; // Skip over wait instruction
				logprintf(LOG_INTS, "Dispatching an interrupt\n");
				arm.reg[15] += 4;
				cpu_exception((cpu_events & EVENT_FIQ) ? EX_FIQ : EX_IRQ);
			}
			cpu_events &= ~EVENT_WAITING;

			if (arm.cpsr_low28 & 0x20)
				cpu_thumb_loop();
			else
				cpu_arm_loop();
		}
	}
	return 0;
}
