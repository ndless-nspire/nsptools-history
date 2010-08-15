#include <string.h>

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

extern u64 cycle_count;
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
extern bool emulate_cas;
extern bool turbo_mode;
extern bool show_speed;

#define NUM_KEYPAD_TYPES 4
#define KEYPAD_ROWS 10
#define KEYPAD_COLUMNS 16
extern int keypad_type;
extern volatile u16 key_map[KEYPAD_ROWS];

enum { LOG_MISC, LOG_CPU, LOG_IO, LOG_HASH, LOG_FLASH, LOG_INTS, LOG_ICOUNT, LOG_USB, MAX_LOG };
#define LOG_TYPE_TBL "MCIHFQ#U";
extern int log_enabled[MAX_LOG];
void logprintf(int type, char *str, ...);

void save_state(void);
bool reload_state(void);
void warn(char *fmt, ...);
__attribute__((noreturn)) void error(char *fmt, ...);
void throttle_timer_on();
void throttle_timer_off();
int exec_hack();
typedef void fault_proc(u32 mva, u8 status);
fault_proc prefetch_abort, data_abort;

/* Declarations for apb.c */

void serial_byte_in(u8 byte);

extern u32 reg_900A0004;

u8 apb_read_byte(u32 addr);
u16 apb_read_half(u32 addr);
u32 apb_read_word(u32 addr);
void apb_write_byte(u32 addr, u8 value);
void apb_write_half(u32 addr, u16 value);
void apb_write_word(u32 addr, u32 value);

extern struct timer {
	int counts_per_int;  /* Write value of +0000 */
	int count;           /* Read value of +0000  */
	int ticks_per_count; /* Write value of +0004 */
	int ticks;
} timer[2];

extern u32 keypad_int_active;
extern void keypad_int_check();

void *apb_save_state(size_t *size);
void apb_reload_state(void *state);

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
void *cpu_save_state(size_t *size);
void cpu_reload_state(void *state);

/* Declarations for debug.c */

void backtrace(u32 fp);
void debugger();
void *debug_save_state(size_t *size);
void debug_reload_state(void *state);

/* Declarations for flash.c */

extern u8 flash_data[0x10000][0x210];
extern char flash_filename[];

u32 nand_flash_read_word(u32 addr);
void nand_flash_write_word(u32 addr, u32 value);
void flash_save_changes();
int flash_save_as(void);
void flash_load(const char *filename);
void flash_reload(void);
void flash_initialize(char *preload_boot2, char *preload_diags, char *preload_os);
void *flash_save_state(size_t *size);
void flash_reload_state(void *state);

/* Declarations for gui.c */

void gui_initialize();
void get_messages();
#ifdef _WINUSER_H
extern HWND hwndMain;
#endif

/* Declarations for sha256.c */

u32 sha256_read_word(u32 addr);
void sha256_write_word(u32 addr, u32 value);
void *sha256_save_state(size_t *size);
void sha256_reload_state(void *state);

/* Declarations for lcd.c */

void lcd_draw_frame(u8 buffer[240][160]);
u32 lcd_read_word(u32 addr);
void lcd_write_word(u32 addr, u32 value);
void *lcd_save_state(size_t *size);
void lcd_reload_state(void *state);

/* Declarations for link.c */

void send_file(char *filename);

u8 ti84_io_link_read_byte(u32 addr);
u16 ti84_io_link_read_half(u32 addr);
u32 ti84_io_link_read_word(u32 addr);
void ti84_io_link_write_byte(u32 addr, u8 value);
void ti84_io_link_write_half(u32 addr, u16 value);
void ti84_io_link_write_word(u32 addr, u32 value);
void *link_save_state(size_t *size);
void link_reload_state(void *state);

/* Declarations for memory.c */

#define ROM_SIZE    0x80000   /* 512 kB */
#define RAM_10_SIZE 0x2000000 /* 32 MB */
#define RAM_A4_SIZE 0x20000   /* 128 kB */

#define MEM_SIZE (ROM_SIZE + RAM_10_SIZE + RAM_A4_SIZE)

extern u8 mem_and_flags[MEM_SIZE * 2];
#define rom_00 (&mem_and_flags[0])
#define ram_10 (&mem_and_flags[ROM_SIZE])
#define ram_A4 (&mem_and_flags[ROM_SIZE + RAM_10_SIZE])

/* Get a pointer for a physical RAM/ROM address. Macro is for when address is constant
 * or when speed is needed. Otherwise, use the function */
#define MEM_PTR(addr) ((void *)( \
	((u32)((addr) - 0x10000000) < RAM_10_SIZE) ? &ram_10[(addr) - 0x10000000] : \
	((u32)((addr) - 0xA4000000) < RAM_A4_SIZE) ? &ram_A4[(addr) - 0xA4000000] : \
	((u32)((addr) - 0x00000000) < ROM_SIZE)    ? &rom_00[(addr) - 0x00000000] : \
	NULL))

struct mem_area_desc {
	u32 base, size;
	u8 *ptr;
};
extern const struct mem_area_desc mem_areas[3];
void *phys_mem_ptr(u32 addr, u32 size);

/* Each word of memory has a flag word associated with it. For fast access,
 * flags are located at a constant offset from the memory data itself.
 *
 * These can't be per-byte because a translation index wouldn't fit then.
 * This does mean byte/halfword accesses have to mask off the low bits to
 * check flags, but the alternative would be another 32MB of memory overhead. */
#define RAM_FLAGS(memptr) (*(u32 *)((u8 *)(memptr) + MEM_SIZE))

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

extern u32 current_ints[2];
#define INT_SERIAL 1
#define INT_USB    8
#define INT_ADC    11
#define INT_KEYPAD 16
#define INT_TIMER1 18
#define INT_TIMER2 19
void int_activate(u32 int_mask);
void int_deactivate(u32 int_mask);

u32 __attribute__((fastcall)) slow_read_byte(u32 addr);
u32 __attribute__((fastcall)) slow_read_half(u32 addr);
u32 __attribute__((fastcall)) slow_read_word(u32 addr);
void __attribute__((fastcall)) slow_write_byte(u32 addr, u32 value);
void __attribute__((fastcall)) slow_write_half(u32 addr, u32 value);
void __attribute__((fastcall)) slow_write_word(u32 addr, u32 value);

void memory_initialize();
void *memory_save_state(size_t *size);
void memory_reload_state(void *state);

/* Declarations for mmu.c */

/* Translate a VA to a PA, using a page table lookup */
u32 mmu_translate(u32 addr, fault_proc *fault);

/* Table for quickly accessing RAM and ROM by virtual addresses.
 * This contains, for each 1kB of virtual address space, whatever must
 * be added to the VA to get a pointer to the appropriate memory.
 * (i.e. &addr_cache_delta[va >> 10][va] gets a pointer from a VA.)
 * 
 * If entry is not valid, the pointer is AC_INVALID_PTR, which has
 * bit 0x80000000 set to allow for simultaneous validity + alignment checking.
 * (Valid pointers will always be inside the statically allocated mem_and_flags
 * array, and thus will never have this bit set.) */
extern u8 *addr_cache_delta[4194304];
#define AC_INVALID_PTR 0x80000000

void addr_cache_init();
void *addr_cache_miss(u32 addr, u32 align_bits, fault_proc *fault);
void addr_cache_flush();
void *mmu_save_state(size_t *size);
void mmu_reload_state(void *state);

/* Declarations for asmcode.S */

void translation_enter();

u32 __attribute__((fastcall)) read_byte(u32 addr);
u32 __attribute__((fastcall)) read_half(u32 addr);
u32 __attribute__((fastcall)) read_word(u32 addr);
void __attribute__((fastcall)) write_byte(u32 addr, u32 value);
void __attribute__((fastcall)) write_half(u32 addr, u32 value);
void __attribute__((fastcall)) write_word(u32 addr, u32 value);

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
void *translate_save_state(size_t *size);
void translate_reload_state(void *state);

/* Declarations for usblink.c */

void usblink_put_file(char *filepath, char *folder);
void usblink_send_os(char *filepath);
void usblink_connect();
void usblink_disconnect();

extern u32 usblink_addr_schedule;
extern u32 usblink_addr_submit_read_buffer;
extern u32 usblink_addr_submit_write_buffer;
void usblink_hook_schedule();
void usblink_hook_submit_read_buffer();
void usblink_hook_submit_write_buffer();
