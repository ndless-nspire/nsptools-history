#include <conio.h>
#include <ctype.h>
#include <setjmp.h>
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

int throttle_delay = 10; /* in milliseconds */

u32 cpu_events;

bool exiting;
bool do_translate = true;
bool emulate_cas;

int keypad_type;
volatile u16 key_map[16];

bool turbo_mode;
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
	exit(1);
}

HANDLE hTimerEvent;
UINT uTimerID;
void throttle_timer_on() {
	uTimerID = timeSetEvent(throttle_delay, throttle_delay, (LPTIMECALLBACK)hTimerEvent, 0,
	                        TIME_PERIODIC | TIME_CALLBACK_EVENT_SET);
	if (uTimerID == 0) {
		printf("timeSetEvent failed\n");
		exit(1);
	}
}
void throttle_timer_off() {
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

struct emu_saved_state {
	u32 cpu_events;
};

void *emu_save_state(size_t *size) {
	*size = sizeof(struct emu_saved_state);
	struct emu_saved_state *state = malloc(*size);
	state->cpu_events = cpu_events;
	return state;
}

void emu_reload_state(void *state) {
	struct emu_saved_state *_state = (struct emu_saved_state *)state;
	cpu_events = _state->cpu_events;
}

static void get_saved_state_filename(char out_filename[MAX_PATH]) {
	strncpy(out_filename, flash_filename, MAX_PATH - 5); // '.sav\0'
	char *pFile = strrchr(out_filename, '/');
	if (!pFile)
		pFile = strrchr(out_filename, '\\');
	pFile = !pFile ? out_filename : pFile + 1;
	char *pExt = strrchr(pFile, '.');
	if (pExt != NULL)
		strcpy(pExt, ".sav");
	else
		strcat(pFile, ".sav");
}

/* increment each time the save file format is changed */
#define SAVE_STATE_VERSION 1
void save_state(void) {
	char state_filename[MAX_PATH+1];
	get_saved_state_filename(state_filename);
	FILE *state_file = fopen(state_filename, "wb");
	if (!state_file) {
		printf("cannot open saved state file\n");
		exit(1);
	}
	size_t chunk_size, total_size = 0, written_size = 0;
	void *chunk_data;
	// format of a chunk: (size_t)data size, data
	#define SAVE_STATE_WRITE_CHUNK(module) \
		chunk_data = module##_save_state(&chunk_size); \
		total_size += sizeof(size_t) + chunk_size; \
		written_size += fwrite(&chunk_size, 1, sizeof(size_t), state_file); \
		written_size += fwrite(chunk_data, 1, chunk_size, state_file); \
		free(chunk_data)
	printf("Saving state...\n");
	const u32 save_state_version = SAVE_STATE_VERSION;
	fwrite(&save_state_version, 1, sizeof(save_state_version), state_file);
	// ordered in reload order
	SAVE_STATE_WRITE_CHUNK(emu);
	flush_translations();
	SAVE_STATE_WRITE_CHUNK(memory);
	SAVE_STATE_WRITE_CHUNK(cpu);
	SAVE_STATE_WRITE_CHUNK(apb);
	SAVE_STATE_WRITE_CHUNK(debug);
	SAVE_STATE_WRITE_CHUNK(flash);
	SAVE_STATE_WRITE_CHUNK(gui);
	SAVE_STATE_WRITE_CHUNK(lcd);
	SAVE_STATE_WRITE_CHUNK(link);
	SAVE_STATE_WRITE_CHUNK(mmu);
	SAVE_STATE_WRITE_CHUNK(sha256);
	SAVE_STATE_WRITE_CHUNK(translate);
	SAVE_STATE_WRITE_CHUNK(usblink);
	flash_save_changes();
	fclose(state_file);
	if (written_size != total_size) {
		printf("Could not write saved state file\n");
		exit(1);
	}
	printf("State saved.\n");
}

// Returns true on success
bool reload_state(void) {
	char state_filename[MAX_PATH];
	size_t chunk_size;
	void *chunk_data;
	get_saved_state_filename(state_filename);
	FILE *state_file = fopen(state_filename, "rb");
	if (!state_file) 
		return false;
	u32 save_state_version;
	if (fread(&save_state_version, 1, sizeof(save_state_version), state_file) != sizeof(save_state_version)) {
		printf("cannot read saved state file\n");
		exit(1);
	}
	if (save_state_version != SAVE_STATE_VERSION) {
		printf("The version of this save file is not supported.");
		fclose(state_file);
		return false;
	}
	#define RELOAD_STATE_READ_CHUNK(module) \
		if (fread(&chunk_size, 1, sizeof(size_t), state_file) != sizeof(size_t)) {  \
			printf("cannot read saved state file\n"); \
			exit(1); \
		} \
		if (chunk_size > 100 * 1024 * 1024) { \
			printf("invalid chunk size in state file\n"); \
			exit(1); \
		} \
		chunk_data = malloc(chunk_size); \
		if (!chunk_data && chunk_size) { \
			printf("cannot malloc chunk of saved state\n"); \
			exit(1); \
		} \
		if (fread(chunk_data, 1, chunk_size, state_file) != chunk_size) { \
			perror("cannot read saved state file"); \
			exit(1); \
		} \
		module##_reload_state(chunk_data); \
		free(chunk_data)
	printf("Reloading state...\n");
	flash_reload();
	// ordered
	RELOAD_STATE_READ_CHUNK(emu);
	RELOAD_STATE_READ_CHUNK(memory);
	RELOAD_STATE_READ_CHUNK(cpu);
	RELOAD_STATE_READ_CHUNK(apb);
	RELOAD_STATE_READ_CHUNK(debug);
	RELOAD_STATE_READ_CHUNK(flash);
	RELOAD_STATE_READ_CHUNK(gui);
	RELOAD_STATE_READ_CHUNK(lcd);
	RELOAD_STATE_READ_CHUNK(link);
	RELOAD_STATE_READ_CHUNK(mmu);
	RELOAD_STATE_READ_CHUNK(sha256);
	RELOAD_STATE_READ_CHUNK(translate);
	RELOAD_STATE_READ_CHUNK(usblink);
	fclose(state_file);
	printf("State reloaded.\n");
	return true;
}

int main(int argc, char **argv) {
	int i;
	FILE *f;
	static FILE *boot2_file = NULL;
	static char *boot1_filename = NULL, *boot2_filename = NULL, *flash_filename = NULL;
	bool new_flash_image = false;
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
					boot2_file = fopen(boot2_filename, "rb");
					if (!boot2_file) {
						perror(boot2_filename);
						return 1;
					}
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
				default:
usage:
					printf(
						"nspire emulator v0.31\n"
						"  /1=boot1      - location of BOOT1 image\n"
						"  /B=boot2      - location of decompressed BOOT2 image\n"
						"  /C            - emulate CAS hardware version\n"
						"  /D            - enter debugger at start\n"
						"  /F=file       - flash image filename\n"
						"  /Kn           - set keypad type (2 = TI-84 Plus, 3 = Touchpad)\n"
						"  /N            - create new flash image\n"
						"  /PB=boot2.img - preload flash with BOOT2 (.img file)\n"
						"  /PD=diags.img - preload flash with DIAGS image\n"
						"  /PO=osfile    - preload flash with OS (.tnc/.tno file)\n");
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
		return flash_save_as();
	}

	if (flash_filename) {
		if (preload_boot2 || preload_diags || preload_os) {
			printf("Can't preload to an existing flash image\n");
			return 1;
		}
		flash_load(flash_filename);
	} else {
		flash_initialize(preload_boot2, preload_diags, preload_os);
	}

	memset(MEM_PTR(0x00000000), -1, 0x80000);
	for (i = 0x00000; i < 0x80000; i += 4) {
		RAM_FLAGS(&rom_00[i]) = RF_READ_ONLY;
	}
	if (boot1_filename) {
		/* Load the ROM */
		f = fopen(boot1_filename, "rb");
		if (!f) {
			perror(boot1_filename);
			return 1;
		}
		fread(MEM_PTR(0x00000000), 1, 0x80000, f);
		fclose(f);
	} else {
		/* Little hack to get OS 1.1 to work */
		*(u32 *)MEM_PTR(0x0000001C) = 0x1234;
		*(u32 *)MEM_PTR(0x00001234) = 0xE12FFF1E; /* bx lr */
	}

	insn_buffer = VirtualAlloc(NULL, INSN_BUFFER_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	insn_bufptr = insn_buffer;

	memory_initialize();
	addr_cache_init();

	LARGE_INTEGER perffreq;
	QueryPerformanceFrequency(&perffreq);

	hTimerEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	gui_initialize();

	throttle_timer_on();
	atexit(throttle_timer_off);
	//FILE *untrans = fopen("untrans.out", "wb");

	if (!reload_state()) {
reset:
		memset(&arm, 0, sizeof arm);
		arm.cpsr_low28 = MODE_SVC | 0xC0;
		cpu_events &= EVENT_DEBUG_STEP;
		if (boot2_file) {
			/* Start from BOOT2. (needs to be re-loaded on each reset since
			 * it can get overwritten in memory) */
			fseek(boot2_file, 0, SEEK_SET);
			fread(MEM_PTR(0x11800000), 1, 0x800000, boot2_file);
			arm.reg[15] = 0x11800000;
			if (*(u8 *)MEM_PTR(0x11800003) < 0xE0) {
				printf("%s does not appear to be an uncompressed BOOT2 image.\n", boot2_filename);
				return 1;
			}
	
			/* To enter maintenance mode (home+enter+P), address A4012ECC
			 * must contain an array indicating those keys before BOOT2 starts */
			memcpy(MEM_PTR(0xA4012ECC), (void *)key_map, 0x12);
	
			/* Disable all FIQs (since BOOT2 neglects to do this) */
			enabled_ints[1] = 0;
		} else {
			/* Start from BOOT1. */
			arm.reg[15] = 0;
		}
		arm.control = 0x00050078;
		addr_cache_flush();
		flush_translations();
	}

	setjmp(restart_after_exception);

	while (!exiting) {
		while (cycle_count_delta < 0) {
			if (cpu_events & EVENT_RESET) {
				printf("CPU reset\n");
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

		// Timer ticks are approx. 32 kHz
		cycle_count       += 2812;
		cycle_count_delta -= 2812;

		if (++timer[0].ticks > timer[0].ticks_per_count) {
			timer[0].ticks = 0;
			if (--timer[0].count < 0) {
				timer[0].count = (timer[0].counts_per_int - 1) & 0xFFFF;
				if (reg_900A0004 & 0x80) // just guessing here...
					int_activate(1 << INT_TIMER1);
			}
		}

		if (++timer[1].ticks > timer[1].ticks_per_count) {
			timer[1].ticks = 0;
			if (--timer[1].count < 0) {
				timer[1].count = (timer[1].counts_per_int - 1) & 0xFFFF;
				int_activate(1 << INT_TIMER2);
			}
		}

		// after polling keypad as a result of getting the second interrupt,
		// OS 2.1 does not wake up keypad task until it receives this one
		keypad_int_active |= 1;
		keypad_int_check();

		static int interval_ticks;
		if (--interval_ticks < 0) {
			/* Throttle interval (defined arbitrarily as 100Hz) - used for
			 * keeping the emulator speed down, and other miscellaneous stuff
			 * that needs to be done periodically */
			static int intervals;
			intervals++;
			interval_ticks += 320;

			if (_kbhit()) {
				char c = _getch();
				if (c == 4)
					debugger();
				else
					serial_byte_in(c);
			}

			get_messages();

			LARGE_INTEGER interval_end;
			QueryPerformanceCounter(&interval_end);

			{	// Update graphics (frame rate is arbitrary)
				static LARGE_INTEGER prev;
				LONGLONG time = interval_end.QuadPart - prev.QuadPart;
				if (time >= (perffreq.QuadPart >> 5)) {
					InvalidateRect(hwndMain, NULL, FALSE);
					prev = interval_end;
				}
			}

			if (show_speed) {
				// Show speed
				static int prev_intervals;
				static LARGE_INTEGER prev;
				LONGLONG time = interval_end.QuadPart - prev.QuadPart;
				if (time >= (perffreq.QuadPart >> 1)) {
					double speed = (double)perffreq.QuadPart * (intervals - prev_intervals) / time;
					char buf[40];
					sprintf(buf, "nspire_emu - %.1f%%", speed);
					SetWindowText(hwndMain, buf);
					prev_intervals = intervals;
					prev = interval_end;
				}
			}

			if (!turbo_mode)
				WaitForSingleObject(hTimerEvent, INFINITE);

			if (log_enabled[LOG_ICOUNT]) {
				static LARGE_INTEGER interval_start;
				LARGE_INTEGER next_start;
				QueryPerformanceCounter(&next_start);

				static u64 prev_cycles;
				u64 cycles = cycle_count + cycle_count_delta;
				logprintf(LOG_ICOUNT, "Time=%7d (CPU=%7d Idle=%7d) Insns=%8d Rate=%9I64d\n",
					next_start.LowPart - interval_start.LowPart,
					interval_end.LowPart - interval_start.LowPart,
					next_start.LowPart - interval_end.LowPart,
					(u32)(cycles - prev_cycles),
					(cycles - prev_cycles) * perffreq.QuadPart / (interval_end.LowPart - interval_start.LowPart));
				prev_cycles = cycles;
				interval_start = next_start;
			}
		}
	}
	return 0;
}
