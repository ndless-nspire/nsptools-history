typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef signed char        s8;
typedef signed short       s16;
typedef signed int         s32;
typedef signed long long   s64;

typedef enum { false, true } bool;

static inline u16 BSWAP16(u16 x) { return x << 8 | x >> 8; }
static inline u32 BSWAP32(u32 x) {
	if (__builtin_constant_p(x)) return x << 24 | (x << 8 & 0xFF0000) | (x >> 8 & 0xFF00) | x >> 24;
	asm ("bswap %0" : "=r" (x) : "0" (x)); return x;
}

/* Declarations for emu.c */

extern int cycle_count_delta;

extern int throttle_delay;

extern u32 cpu_events;
#define EVENT_IRQ 1
#define EVENT_FIQ 2
#define EVENT_RESET 4
#define EVENT_DEBUG_STEP 8
#define EVENT_WAITING 16

extern bool exiting;
extern bool do_translate;
extern int product;
extern int asic_user_flags;
extern bool emulate_cx;
extern bool turbo_mode;
extern bool is_halting;
extern bool show_speed;

enum { LOG_CPU, LOG_IO, LOG_FLASH, LOG_INTS, LOG_ICOUNT, LOG_USB, MAX_LOG };
#define LOG_TYPE_TBL "CIFQ#U";
extern int log_enabled[MAX_LOG];
void logprintf(int type, char *str, ...);

void warn(char *fmt, ...);
__attribute__((noreturn)) void error(char *fmt, ...);
void throttle_timer_on();
void throttle_timer_off();
int exec_hack();
typedef void fault_proc(u32 mva, u8 status);
fault_proc prefetch_abort, data_abort;

/* Declarations for apb.c */

union gpio_reg { u32 w; u8 b[4]; };
extern struct gpio_state {
	union gpio_reg direction;
	union gpio_reg output;
	union gpio_reg input;
	union gpio_reg invert;
	union gpio_reg sticky;
	union gpio_reg unknown_24;
} gpio;

void gpio_reset();

void serial_byte_in(u8 byte);

void watchdog_reset();

extern struct pmu_state {
	u32 clocks_load;
	u32 wake_mask;
	u32 disable;
	u32 disable2;
	u32 clocks;
} pmu;
void pmu_reset(void);

extern struct timerpair {
	struct timer {
		u16 ticks;
		u16 start_value;     /* Write value of +00 */
		u16 value;           /* Read value of +00  */
		u16 divider;         /* Value of +04 */
		u16 control;         /* Value of +08 */
	} timers[2];
	u16 completion_value[6];
	u8 int_mask;
	u8 int_status;
} timerpairs[3];
void timer_reset(void);
void timer_cx_reset(void);

extern struct apb_map_entry {
	u32 (*read)(u32 addr);
	void (*write)(u32 addr, u32 value);
} apb_map[0x12];
extern const struct apb_map_entry apb_map_normal[0x12];
extern const struct apb_map_entry apb_map_cx[0x12];

u8 apb_read_byte(u32 addr);
u16 apb_read_half(u32 addr);
u32 apb_read_word(u32 addr);
void apb_write_byte(u32 addr, u8 value);
void apb_write_half(u32 addr, u16 value);
void apb_write_word(u32 addr, u32 value);

/* Declarations for cpu.c */

struct arm_state {  // Remember to update asmcode.S if this gets rearranged
	u32 reg[16];    // Registers for current mode.

	u32 cpsr_low28; // CPSR bits 0-27
	u8  cpsr_n;     // CPSR bit 31
	u8  cpsr_z;     // CPSR bit 30
	u8  cpsr_c;     // CPSR bit 29
	u8  cpsr_v;     // CPSR bit 28

	u32 r8_usr[5], r13_usr[2];
	u32 r8_fiq[5], r13_fiq[2], spsr_fiq;
	u32 r13_irq[2], spsr_irq;
	u32 r13_svc[2], spsr_svc;
	u32 r13_abt[2], spsr_abt;
	u32 r13_und[2], spsr_und;

	u8  interrupts;

	/* CP15 registers */
	u32 control;
	u32 translation_table_base;
	u32 domain_access_control;
	u8  data_fault_status, instruction_fault_status;
	u32 fault_address;
};
extern struct arm_state arm;

#define MODE_USR 0x10
#define MODE_FIQ 0x11
#define MODE_IRQ 0x12
#define MODE_SVC 0x13
#define MODE_ABT 0x17
#define MODE_UND 0x1B
#define MODE_SYS 0x1F
#define PRIVILEGED_MODE() (arm.cpsr_low28 & 3)
#define USER_MODE()       (!(arm.cpsr_low28 & 3))

#define EX_RESET          0
#define EX_UNDEFINED      1
#define EX_SWI            2
#define EX_PREFETCH_ABORT 3
#define EX_DATA_ABORT     4
#define EX_IRQ            6
#define EX_FIQ            7

void cpu_int_check();
u32 __attribute__((fastcall)) get_cpsr();
void __attribute__((fastcall)) set_cpsr(u32 cpsr, u32 mask);
u32 __attribute__((fastcall)) get_spsr();
void __attribute__((fastcall)) set_spsr(u32 cpsr, u32 mask);
void cpu_exception(int type);
void cpu_interpret_instruction(u32 insn);
void cpu_arm_loop();
void cpu_thumb_loop();

/* Declarations for debug.c */

void backtrace(u32 fp);
void debugger();

/* Declarations for des.c */

void des_initialize();
u32 des_read_word(u32 addr);
void des_write_word(u32 addr, u32 value);

/* Declarations for flash.c */

extern void nand_initialize(bool large);
extern bool nand_writable;
u32 nand_phx_read_word(u32 addr);
void nand_phx_write_word(u32 addr, u32 value);
u8 nand_cx_read_byte(u32 addr);
u32 nand_cx_read_word(u32 addr);
void nand_cx_write_byte(u32 addr, u8 value);
void nand_cx_write_word(u32 addr, u32 value);

void flash_open(char *filename);
void flash_save_changes();
int flash_save_as(char *filename);
void flash_create_new(char **preload, int product, bool large_sdram);
void flash_read_settings(u32 *sdram_size);

/* Declarations for gui.c */

void gui_initialize();
void get_messages();

/* Declarations for interrupt.c */

#define INT_SERIAL   1
#define INT_WATCHDOG 3
#define INT_USB      8
#define INT_ADC      11
#define INT_POWER    15
#define INT_KEYPAD   16
#define INT_TIMER0   17
#define INT_TIMER1   18
#define INT_TIMER2   19
#define INT_LCD      21

extern struct interrupt_state {
	u32 active;
	u32 raw_status;         // .active ^ ~.noninverted
	u32 sticky_status;      // set on rising transition of .raw_status
	u32 status;             // +x04: mixture of bits from .raw_status and .sticky_status
	                        //       (determined by .sticky)
	u32 mask[2];            // +x08: enabled interrupts
	u8  prev_pri_limit[2];  // +x28: saved .priority_limit from reading +x24
	u8  priority_limit[2];  // +x2C: interrupts with priority >= this value are disabled
	u32 noninverted;        // +200: which interrupts not to invert in .raw_status
	u32 sticky;             // +204: which interrupts to use .sticky_status
	u8  priority[32];       // +3xx: priority per interrupt (0=max, 7=min)
} intr;
u32 int_read_word(u32 addr);
void int_write_word(u32 addr, u32 value);
u32 int_cx_read_word(u32 addr);
void int_cx_write_word(u32 addr, u32 value);
void int_set(u32 int_num, bool on);

/* Declarations for sha256.c */

u32 sha256_read_word(u32 addr);
void sha256_write_word(u32 addr, u32 value);

/* Declarations for keypad.c */

#define NUM_KEYPAD_TYPES 4
extern volatile int keypad_type;
extern volatile u16 key_map[16];
extern volatile u8 touchpad_proximity;
extern volatile u16 touchpad_x;
extern volatile u16 touchpad_y;
extern volatile u8 touchpad_down;

extern struct keypad_controller_state {
	u32 control;
	u32 size;
	u8  current_row;
	u8  int_active;
	u8  int_enable;
	u16 data[16];
	u32 gpio_int_enable;
	u32 gpio_int_active;
} kpc;
void keypad_reset();
void keypad_int_check();
u32 keypad_read(u32 addr);
void keypad_write(u32 addr, u32 value);
u32 touchpad_cx_read(u32 addr);
void touchpad_cx_write(u32 addr, u32 value);

#define TOUCHPAD_X_MAX 0x0918
#define TOUCHPAD_Y_MAX 0x069B
void touchpad_set(u8 proximity, u16 x, u16 y, u8 down);
void touchpad_gpio_change();

/* Declarations for lcd.c */

void lcd_draw_frame(u8 buffer[240][160]);
void lcd_cx_draw_frame(u16 buffer[240][320], u32 colormasks[3]);
void lcd_reset(void);
u32 lcd_read_word(u32 addr);
void lcd_write_word(u32 addr, u32 value);

/* Declarations for link.c */

void send_file(char *filename);

u32 ti84_io_link_read(u32 addr);
void ti84_io_link_write(u32 addr, u32 value);

/* Declarations for memory.c */

#define ROM_SIZE       0x80000   /* 512 kB */
#define RAM_A4_SIZE    0x20000   /* 128 kB */
#define RAM_10_MAXSIZE 0x4000000 /* 64 MB */

#define MEM_MAXSIZE (ROM_SIZE + RAM_A4_SIZE + RAM_10_MAXSIZE)

// Must be allocated below 2GB (see comments for mmu.c)
extern u8 *mem_and_flags;
#define rom_00 (&mem_and_flags[0])
#define ram_A4 (&mem_and_flags[ROM_SIZE])
#define ram_10 (&mem_and_flags[ROM_SIZE + RAM_A4_SIZE])

/* Get a pointer for a physical RAM/ROM address. Macro is for when address is constant
 * or when speed is needed. Otherwise, use the function */
#define MEM_PTR(addr) ((void *)( \
	((u32)((addr) - 0x10000000) < RAM_10_MAXSIZE) ? &ram_10[(addr) - 0x10000000] : \
	((u32)((addr) - 0xA4000000) < RAM_A4_SIZE)    ? &ram_A4[(addr) - 0xA4000000] : \
	((u32)((addr) - 0x00000000) < ROM_SIZE)       ? &rom_00[(addr) - 0x00000000] : \
	NULL))

struct mem_area_desc {
	u32 base, size;
	u8 *ptr;
};
extern struct mem_area_desc mem_areas[4];
void *phys_mem_ptr(u32 addr, u32 size);

/* Each word of memory has a flag word associated with it. For fast access,
 * flags are located at a constant offset from the memory data itself.
 *
 * These can't be per-byte because a translation index wouldn't fit then.
 * This does mean byte/halfword accesses have to mask off the low bits to
 * check flags, but the alternative would be another 32MB of memory overhead. */
#define RAM_FLAGS(memptr) (*(u32 *)((u8 *)(memptr) + MEM_MAXSIZE))

#define RF_READ_BREAKPOINT   1
#define RF_WRITE_BREAKPOINT  2
#define RF_EXEC_BREAKPOINT   4
#define RF_EXEC_DEBUG_NEXT   8
#define RF_EXEC_HACK         16
#define RF_CODE_TRANSLATED   32
#define RF_CODE_NO_TRANSLATE 64
#define RF_READ_ONLY         128
#define RFS_TRANSLATION_INDEX 8

u8 bad_read_byte(u32 addr);
u16 bad_read_half(u32 addr);
u32 bad_read_word(u32 addr);
void bad_write_byte(u32 addr, u8 value);
void bad_write_half(u32 addr, u16 value);
void bad_write_word(u32 addr, u32 value);

u32 __attribute__((fastcall)) mmio_read_byte(u32 addr);
u32 __attribute__((fastcall)) mmio_read_half(u32 addr);
u32 __attribute__((fastcall)) mmio_read_word(u32 addr);
void __attribute__((fastcall)) mmio_write_byte(u32 addr, u32 value);
void __attribute__((fastcall)) mmio_write_half(u32 addr, u32 value);
void __attribute__((fastcall)) mmio_write_word(u32 addr, u32 value);

void memory_initialize();

/* Declarations for mmu.c */

/* Translate a VA to a PA, using a page table lookup */
u32 mmu_translate(u32 addr, bool writing, fault_proc *fault);

/* Table for quickly accessing RAM and ROM by virtual addresses. This contains
 * two entries for each 1kB of virtual address space, one for reading and one
 * for writing, and each entry may contain one of three kinds of values:
 *
 * a) Pointer entry
 *    The result of adding the virtual address (VA) to the entry has bit 31
 *    clear, and that sum is a pointer to within mem_and_flags.
 *    It would be cleaner to just use bit 0 or 1 to distinguish this case, but
 *    this way we can simultaneously check both that this is a pointer entry,
 *    and that the address is aligned, with a single bit test instruction.
 * b) Physical address entry
 *    VA + entry has bit 31 set, and the entry (not the sum) has bit 22 clear.
 *    Bits 0-21 contain the difference between virtual and physical address.
 * c) Invalid entry
 *    VA + entry has bit 31 set, entry has bit 22 set. Entry is invalid and
 *    addr_cache_miss must be called.
 */
 
#define AC_NUM_ENTRIES (4194304*2)
typedef u8 *ac_entry;
extern ac_entry *addr_cache;
#define AC_SET_ENTRY_PTR(entry, va, ptr) \
	entry = (ptr) - (va);
#define AC_NOT_PTR 0x80000000
#define AC_SET_ENTRY_PHYS(entry, va, pa) \
	entry = (ac_entry)(((pa) - (va)) >> 10); \
	entry += (~(u32)((va) + entry) & AC_NOT_PTR);
#define AC_SET_ENTRY_INVALID(entry, va) \
	entry = (ac_entry)(1 << 22); \
	entry += (~(u32)((va) + entry) & AC_NOT_PTR);

bool addr_cache_pagefault(void *addr);
void *addr_cache_miss(u32 addr, bool writing, fault_proc *fault);
void addr_cache_flush();

/* Declarations for asmcode.S */

void translation_enter();

u32 __attribute__((fastcall)) read_byte(u32 addr);
u32 __attribute__((fastcall)) read_half(u32 addr);
u32 __attribute__((fastcall)) read_word(u32 addr);
void __attribute__((fastcall)) write_byte(u32 addr, u32 value);
void __attribute__((fastcall)) write_half(u32 addr, u32 value);
void __attribute__((fastcall)) write_word(u32 addr, u32 value);

/* Declarations for schedule.c */

enum clock_id { CLOCK_CPU, CLOCK_AHB, CLOCK_APB, CLOCK_27M, CLOCK_12M, CLOCK_32K };
enum sched_item_index {
	SCHED_THROTTLE,
	SCHED_KEYPAD,
	SCHED_LCD,
	SCHED_TIMERS,
	SCHED_WATCHDOG,
	SCHED_NUM_ITEMS
};
extern struct sched_item {
	enum clock_id clock;
	int second; // -1 = disabled
	u32 tick;
	u32 cputick;
	void (*proc)(int index);
} sched_items[SCHED_NUM_ITEMS];

void sched_reset(void);
void event_repeat(int index, u32 ticks);
void sched_update_next_event(u32 cputick);
u32 sched_process_pending_events();
void event_clear(int index);
void event_set(int index, int ticks);
u32 event_ticks_remaining(int index);
void sched_set_clocks(int count, u32 *new_rates);

/* Declarations for translate.c */

struct translation {
	u32 unused;
	u32 jump_table;
	u32 *start_ptr;
	u32 *end_ptr;
};
extern struct translation translation_table[];
#define INSN_BUFFER_SIZE 10000000
extern u8 *insn_buffer;
extern u8 *insn_bufptr;

int translate(u32 start_pc, u32 *insnp);
void flush_translations();
void invalidate_translation(int index);
void fix_pc_for_fault();

/* Declarations for usb.c */

extern struct usb_state {
	u32 usbcmd;      // +140
	u32 usbsts;      // +144
	u32 usbintr;     // +148
	u32 deviceaddr;  // +154
	u32 eplistaddr;  // +158
	u32 portsc;      // +184
	u32 otgsc;       // +1A4
	u32 epsetupsr;   // +1AC
	u32 epsr;        // +1B8
	u32 epcomplete;  // +1BC
} usb;
void usb_reset(void);
u8 usb_read_byte(u32 addr);
u16 usb_read_half(u32 addr);
u32 usb_read_word(u32 addr);
void usb_write_word(u32 addr, u32 value);

/* Declarations for usblink.c */

void usblink_put_file(char *filepath, char *folder);
void usblink_send_os(char *filepath);

void usblink_reset();
void usblink_connect();
void usblink_disconnect();
