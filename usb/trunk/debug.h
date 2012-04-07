#ifndef _DEBUG_H

#include "nspireio2.h"

void dbg_dump(nio_console *c, void *addr);
void dbg_init(void);
void dbg_cleanup(void);
void dbg_set_breakpoint(unsigned addr);
static inline void dbg_break(void) {
	asm(".long 0xE12FFF7F"); // immed = FFFF
}
	
#endif
