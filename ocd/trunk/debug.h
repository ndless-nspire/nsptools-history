#ifndef _DEBUG_H

#include "nspireio2.h"

void dbg_dump(nio_console *c, void *addr);
void dbg_init(void);
void dbg_cleanup(void);
void dbg_set_breakpoint(unsigned addr);
static inline void dbg_break(void) {
	asm(".long 0xE12FFF7F"); // immed = FFFF
}

// private
extern const char reg_name[16][4];
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef signed char        s8;
typedef signed short       s16;
typedef signed int         s32;
static inline void *virt_mem_ptr(u32 addr, u32 __attribute__((unused)) size) {
	return (void*)addr;
}	
u32 disasm_arm_insn(u32 pc, nio_console *csl);
#endif
