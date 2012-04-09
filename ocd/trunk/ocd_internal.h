#ifndef _OCD_INTERNAL_H

/* Private definitions */

#include <nspireio2.h>

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
