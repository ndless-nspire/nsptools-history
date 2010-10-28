/****************************************************************************
 * Ndless - Common definitions
 ****************************************************************************/

#ifndef _COMMON_H_
#define _COMMON_H_

/***********************************
 * Hardware
 ***********************************/
#define SCREEN_BASE_ADDRESS     ADDR_(0xA4000100)
#define KEY_MAP                 ADDR_(0x900E0000)

/***********************************
 * Addresses
 ***********************************/
#define OS_BASE_ADDRESS         ADDR_(0x10000000)

/***********************************
 * Others
 ***********************************/
#define SCREEN_BYTES_SIZE       38400
#define SCREEN_WIDTH            320
#define SCREEN_HEIGHT           240

#define BLACK                   0x0
#define WHITE                   0xF

/***********************************
 * Keys (key=(offset, 2^bit #)
 ***********************************/
#define KEY_NSPIRE_RET          KEY_(0x10, 0x001)
#define KEY_NSPIRE_ENTER        KEY_(0x10, 0x002)
#define KEY_NSPIRE_SPACE        KEY_(0x10, 0x004)
#define KEY_NSPIRE_NEGATIVE     KEY_(0x10, 0x008)
#define KEY_NSPIRE_Z            KEY_(0x10, 0x010)
#define KEY_NSPIRE_PERIOD       KEY_(0x10, 0x020)
#define KEY_NSPIRE_Y            KEY_(0x10, 0x040)
#define KEY_NSPIRE_0            KEY_(0x10, 0x080)
#define KEY_NSPIRE_X            KEY_(0x10, 0x100)
#define KEY_NSPIRE_THETA        KEY_(0x10, 0x400)
#define KEY_NSPIRE_COMMA        KEY_(0x12, 0x001)
#define KEY_NSPIRE_PLUS         KEY_(0x12, 0x002)
#define KEY_NSPIRE_W            KEY_(0x12, 0x004)
#define KEY_NSPIRE_3            KEY_(0x12, 0x008)
#define KEY_NSPIRE_V            KEY_(0x12, 0x010)
#define KEY_NSPIRE_2            KEY_(0x12, 0x020)
#define KEY_NSPIRE_U            KEY_(0x12, 0x040)
#define KEY_NSPIRE_1            KEY_(0x12, 0x080)
#define KEY_NSPIRE_T            KEY_(0x12, 0x100)
#define KEY_NSPIRE_eEXP         KEY_(0x12, 0x200)
#define KEY_NSPIRE_PI           KEY_(0x12, 0x400)
#define KEY_NSPIRE_QUES         KEY_(0x14, 0x001)
#define KEY_NSPIRE_MINUS        KEY_(0x14, 0x002)
#define KEY_NSPIRE_S            KEY_(0x14, 0x004)
#define KEY_NSPIRE_6            KEY_(0x14, 0x008)
#define KEY_NSPIRE_R            KEY_(0x14, 0x010)
#define KEY_NSPIRE_5            KEY_(0x14, 0x020)
#define KEY_NSPIRE_Q            KEY_(0x14, 0x040)
#define KEY_NSPIRE_4            KEY_(0x14, 0x080)
#define KEY_NSPIRE_P            KEY_(0x14, 0x100)
#define KEY_NSPIRE_TENX         KEY_(0x14, 0x200)
#define KEY_NSPIRE_EE           KEY_(0x14, 0x400)
#define KEY_NSPIRE_COLON        KEY_(0x16, 0x001)
#define KEY_NSPIRE_MULTIPLY     KEY_(0x16, 0x002)
#define KEY_NSPIRE_O            KEY_(0x16, 0x004)
#define KEY_NSPIRE_9            KEY_(0x16, 0x008)
#define KEY_NSPIRE_N            KEY_(0x16, 0x010)
#define KEY_NSPIRE_8            KEY_(0x16, 0x020)
#define KEY_NSPIRE_M            KEY_(0x16, 0x040)
#define KEY_NSPIRE_7            KEY_(0x16, 0x080)
#define KEY_NSPIRE_L            KEY_(0x16, 0x100)
#define KEY_NSPIRE_SQU          KEY_(0x16, 0x200)
#define KEY_NSPIRE_II           KEY_(0x16, 0x400) 
#define KEY_NSPIRE_QUOTE        KEY_(0x18, 0x001)
#define KEY_NSPIRE_DIVIDE       KEY_(0x18, 0x002)
#define KEY_NSPIRE_K            KEY_(0x18, 0x004)
#define KEY_NSPIRE_TAN          KEY_(0x18, 0x008)
#define KEY_NSPIRE_J            KEY_(0x18, 0x010)
#define KEY_NSPIRE_COS          KEY_(0x18, 0x020)
#define KEY_NSPIRE_I            KEY_(0x18, 0x040)
#define KEY_NSPIRE_SIN          KEY_(0x18, 0x080)
#define KEY_NSPIRE_H            KEY_(0x18, 0x100)
#define KEY_NSPIRE_EXP          KEY_(0x18, 0x200)
#define KEY_NSPIRE_GTHAN        KEY_(0x18, 0x400)
#define KEY_NSPIRE_APOSTROPHE   KEY_(0x1A, 0x001)
#define KEY_NSPIRE_CAT          KEY_(0x1A, 0x002)
#define KEY_NSPIRE_G            KEY_(0x1A, 0x004)
#define KEY_NSPIRE_RP           KEY_(0x1A, 0x008)
#define KEY_NSPIRE_F            KEY_(0x1A, 0x010)
#define KEY_NSPIRE_LP           KEY_(0x1A, 0x020)
#define KEY_NSPIRE_E            KEY_(0x1A, 0x040)
#define KEY_NSPIRE_VAR          KEY_(0x1A, 0x080)
#define KEY_NSPIRE_D            KEY_(0x1A, 0x100)
#define KEY_NSPIRE_CAPS         KEY_(0x1A, 0x200)
#define KEY_NSPIRE_LTHAN        KEY_(0x1A, 0x400)
#define KEY_NSPIRE_FLAG         KEY_(0x1C, 0x001)
#define KEY_NSPIRE_CLICK        KEY_(0x1C, 0x002)
#define KEY_NSPIRE_C            KEY_(0x1C, 0x004)
#define KEY_NSPIRE_HOME         KEY_(0x1C, 0x008)
#define KEY_NSPIRE_B            KEY_(0x1C, 0x010)
#define KEY_NSPIRE_MENU         KEY_(0x1C, 0x020)
#define KEY_NSPIRE_A            KEY_(0x1C, 0x040)
#define KEY_NSPIRE_ESC          KEY_(0x1C, 0x080)
#define KEY_NSPIRE_BAR          KEY_(0x1C, 0x100)
#define KEY_NSPIRE_TAB          KEY_(0x1C, 0x200)
#define KEY_NSPIRE_EQU          KEY_(0x1C, 0x400)
#define KEY_NSPIRE_UP           KEY_(0x1E, 0x001)
#define KEY_NSPIRE_UPRIGHT      KEY_(0x1E, 0x002)
#define KEY_NSPIRE_RIGHT        KEY_(0x1E, 0x004)
#define KEY_NSPIRE_RIGHTDOWN    KEY_(0x1E, 0x008)
#define KEY_NSPIRE_DOWN         KEY_(0x1E, 0x010)
#define KEY_NSPIRE_DOWNLEFT     KEY_(0x1E, 0x020)
#define KEY_NSPIRE_LEFT         KEY_(0x1E, 0x040)
#define KEY_NSPIRE_LEFTUP       KEY_(0x1E, 0x080)
#define KEY_NSPIRE_CLEAR        KEY_(0x1E, 0x100)
#define KEY_NSPIRE_CTRL         KEY_(0x1E, 0x200)

/* TI-84+ Keypad Mappings */
#define KEY_84_DOWN            KEY_(0x10, 0x001)
#define KEY_84_LEFT            KEY_(0x10, 0x002)
#define KEY_84_RIGHT           KEY_(0x10, 0x004)
#define KEY_84_UP              KEY_(0x10, 0x008)
#define KEY_84_ENTER           KEY_(0x12, 0x001)
#define KEY_84_PLUS            KEY_(0x12, 0x002)
#define KEY_84_MINUS           KEY_(0x12, 0x004)
#define KEY_84_MULTIPLY        KEY_(0x12, 0x008)
#define KEY_84_DIVIDE          KEY_(0x12, 0x010)
#define KEY_84_EXP             KEY_(0x12, 0x020)
#define KEY_84_CLEAR           KEY_(0x12, 0x040)
#define KEY_84_NEGATIVE        KEY_(0x14, 0x001)
#define KEY_84_3               KEY_(0x14, 0x002)
#define KEY_84_6               KEY_(0x14, 0x004)
#define KEY_84_9               KEY_(0x14, 0x008)
#define KEY_84_RP              KEY_(0x14, 0x010)
#define KEY_84_TAN             KEY_(0x14, 0x020)
#define KEY_84_VARS            KEY_(0x14, 0x040)
#define KEY_84_PERIOD          KEY_(0x16, 0x001)
#define KEY_84_2               KEY_(0x16, 0x002)
#define KEY_84_5               KEY_(0x16, 0x004)
#define KEY_84_8               KEY_(0x16, 0x008)
#define KEY_84_LP              KEY_(0x16, 0x010)
#define KEY_84_COS             KEY_(0x16, 0x020)
#define KEY_84_PRGM            KEY_(0x16, 0x040)
#define KEY_84_STAT            KEY_(0x16, 0x080)
#define KEY_84_0               KEY_(0x18, 0x001)
#define KEY_84_1               KEY_(0x18, 0x002)
#define KEY_84_4               KEY_(0x18, 0x004)
#define KEY_84_7               KEY_(0x18, 0x008)
#define KEY_84_COMMA           KEY_(0x18, 0x010)
#define KEY_84_SIN             KEY_(0x18, 0x020)
#define KEY_84_APPS            KEY_(0x18, 0x040)
#define KEY_84_X               KEY_(0x18, 0x080)
#define KEY_84_STO             KEY_(0x1A, 0x002)
#define KEY_84_LN              KEY_(0x1A, 0x004)
#define KEY_84_LOG             KEY_(0x1A, 0x008)
#define KEY_84_SQU             KEY_(0x1A, 0x010)
#define KEY_84_INV             KEY_(0x1A, 0x020)
#define KEY_84_MATH            KEY_(0x1A, 0x040)
#define KEY_84_ALPHA           KEY_(0x1A, 0x080)
#define KEY_84_GRAPH           KEY_(0x1C, 0x001)
#define KEY_84_TRACE           KEY_(0x1C, 0x002)
#define KEY_84_ZOOM            KEY_(0x1C, 0x004)
#define KEY_84_WIND            KEY_(0x1C, 0x008)
#define KEY_84_YEQU            KEY_(0x1C, 0x010)
#define KEY_84_2ND             KEY_(0x1C, 0x020)
#define KEY_84_MODE            KEY_(0x1C, 0x040)
#define KEY_84_DEL             KEY_(0x1C, 0x080)

/** GNU AS */
#ifdef GNU_AS
  #define SHARP(s)          #
  #define ADDR_(addr)       addr
  #define KEY_(row, col)    row, SHARP(s)##col

  .macro isKeyPressed row, col
    ldr     r0, =(KEY_MAP + \row)
    ldrh    r0, [r0]
    tst     r0, \col
  .endm
  
  .macro halt
halt\@: b halt\@
  .endm

/** GNU C Compiler */
#else

#include "libndls.h"

#define _STRINGIFY(s) #s
#define STRINGIFY(s) _STRINGIFY(s)
#define NULL ((void*)0)
typedef enum bool {FALSE = 0, TRUE = 1} BOOL;
typedef struct{} FILE;
typedef unsigned long size_t;

typedef struct {
  int row, col;
} t_key;

#define ADDR_(addr)             (void*)addr
#define KEY_(row, col)          (t_key){row, col}
#define isKeyPressed(key)       (!((*(volatile short*)(KEY_MAP + (key).row)) & (key).col))

#ifndef abs
#define abs(x) ({typeof(x) __x = (x); __x >= 0 ? __x : -__x;})
#endif
#ifndef max
#define max(a,b) ({typeof(a) __a = (a); typeof(b) __b = (b); (__a > __b) ? __a : __b;})
#define min(a,b) ({typeof(a) __a = (a); typeof(b) __b = (b); (__a < __b) ? __a : __b;})
#endif

/***********************************
 * Misc inline functions
 ***********************************/

static inline void halt(void) {
  asm volatile("0: b 0b");
}

/* switch to low-power state until next interrupt */
static inline void idle(void) {
  unsigned int sbz = 0;
  asm volatile("mcr p15, 0, %0, c7, c0, 4" : "=r"(sbz) );
}

#define HOOK_INSTALL(address, hookname) do { \
	extern unsigned hookname; \
	extern unsigned __##hookname##_end_instrs[4]; /* orig_instrs1; orig_instrs2; ldr pc, [pc, #-4]; .long return_addr */ \
	__##hookname##_end_instrs[3] = (unsigned)(address) + 8; \
	__##hookname##_end_instrs[0] = *(unsigned*)(address); \
	*(unsigned*)(address) = 0xE51FF004; /* ldr pc, [pc, #-4] */ \
	__##hookname##_end_instrs[1] = *(unsigned*)((address) + 4); \
	*(unsigned*)((address) + 4) = (unsigned)&hookname; \
	__##hookname##_end_instrs[2] = 0xE51FF004; /* ldr pc, [pc, #-4] */ \
	} while (0)

/* Caution, hooks aren't re-entrant.
 * A non-inlined body is required because naked function cannot use local variables.
 * A naked function is required because the return is handled by the hook, and to avoid any
 * register modification before they are saved */
#define HOOK_DEFINE(hookname) \
	unsigned __##hookname##_end_instrs[4]; \
	extern unsigned __##hookname##_saved_sp; \
	asm(STRINGIFY(__##hookname##_saved_sp) ": .long 0"); /* accessed with pc-relative instruction */ \
	void __##hookname##_body(void); \
	void __attribute__((naked)) hookname(void) { \
		asm volatile(" stmfd sp!, {r0-r12,lr}"); /* used by HOOK_RESTORE_STATE() */ \
		/* save sp */ \
		asm volatile( \
			" str r0, [sp, #-4] @ push r0 but don't change sp \n " \
			" adr r0," STRINGIFY(__##hookname##_saved_sp) "\n" \
			" str sp, [r0] \n" \
			" ldr r0, [sp, #-4] @ pop r0 but don't change sp \n" \
		); \
		 __##hookname##_body(); \
	} \
	void __##hookname##_body(void)

/* Jump out of the body */
#define HOOK_RESTORE_SP(hookname) do { \
	asm volatile( \
		" str lr, [sp, #-4]! @ push lr \n" \
		" adr lr," STRINGIFY(__##hookname##_saved_sp) "\n" \
		" ldr lr, [lr] \n" \
		" str lr, [sp, #-4]! \n" /* push lr=saved_sp. trick to restore both saved_sp and the original lr */ \
		" ldmfd sp, {sp, lr} \n" /* lr has been used instead of r0 to avoid a GAS warning about reg order on this instr */ \
	); \
} while (0)

/* May be used to access the values that had the registers when the hook was called: {r0-r12,lr} */
#define HOOK_SAVED_REGS(hookname) ((unsigned*) __##hookname##_saved_sp)

#define HOOK_RESTORE_STATE() do { \
	asm volatile(" ldmfd sp!, {r0-r12,lr}"); \
} while (0)


/* Call HOOK_RESTORE() alone to return manually with asm(). */
#define HOOK_RESTORE(hookname) { \
	HOOK_RESTORE_SP(hookname); \
	HOOK_RESTORE_STATE(); \
} while (0)

/* If register values needs to be changed before a hook return, call HOOK_RESTORE(),
 * set the registers then call HOOK_RETURN. Caution, only assembly without local
 * variables can between the 2 calls. */
#define HOOK_RETURN(hookname) do { \
	asm volatile(" b " STRINGIFY(__##hookname##_end_instrs)); \
} while (0)

/* Standard hook return */
#define HOOK_RESTORE_RETURN(hookname) do { \
	HOOK_RESTORE(hookname); \
	HOOK_RETURN(hookname); \
} while (0)

/* Hook return skipping instructions */
#define HOOK_RESTORE_RETURN_SKIP(hookname, offset) do { \
	volatile unsigned __end_instrs_skip##offset[4]; \
	/* Copy the default end instructions */ \
	memcpy((void*)__end_instrs_skip##offset, __##hookname##_end_instrs, sizeof(__end_instrs_skip##offset)); \
	/* Patch the final jump to skip instructions */ \
	__end_instrs_skip##offset[3] += offset; \
	/* Patch the next asm() to branch to this copy */ \
	asm volatile( \
		" adr r0, " STRINGIFY(__##hookname##_end_instrs_jump_offset_skip##offset) "\n" \
		"	str %0, [r0] \n" \
		:: "r"(&__end_instrs_skip##offset) : "r0"); \
	HOOK_RESTORE(hookname); \
	/* Branch to the end instrs copy */ \
	asm volatile( \
		" ldr pc, [pc, #-4] \n" \
	  STRINGIFY(__##hookname##_end_instrs_jump_offset_skip##offset) ":" \
		" .long 0"); \
} while (0)

/***********************************
 * Nucleus
 ***********************************/

#define ARDONLY 0x1     /* MS-DOS File attributes */ 
#define AHIDDEN 0x2 
#define ASYSTEM 0x4 
#define AVOLUME 0x8  
#define ADIRENT 0x10 
#define ARCHIVE 0x20 
#define ANORMAL 0x00 

struct dstat {
	char unknown1[13];
  char filepath[266];        /* Null terminated: A:\... */
  unsigned char fattribute;  /* File attributes: see A* constants above */
  unsigned short unknown2;
  unsigned short unknown3;
  unsigned short unknown4;
  unsigned short unknown5;
  unsigned long fsize;      /* File size */
  void *unknown6, *unknown7;
  unsigned int unknown8;
  unsigned short unknown9;
};
 
/***********************************
 * POSIX
 ***********************************/

#define S_IFMT  00170000
#define S_IFSOCK 0140000
#define S_IFLNK  0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000

#define S_ISLNK(m)      (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)      (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)      (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)      (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)      (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)     (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m)     (((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

struct stat {
	unsigned short st_dev;
	unsigned int st_ino; // 0
	unsigned int st_mode; // see S_ macros above
	unsigned short st_nlink; // 1
	unsigned short st_uid; // 0
	unsigned short st_gid; // 0
	unsigned short st_rdev; // = st_dev
	unsigned int st_size;
	unsigned int st_atime;
	unsigned int st_mtime;
	unsigned int st_ctime;
};

#endif /* GCC C */

#endif
