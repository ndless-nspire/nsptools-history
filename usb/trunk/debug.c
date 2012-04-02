#include <os.h>
#include "nspireio2.h"

void dbg_dump(nio_console *c, void *addr) {
	clrscr();
	unsigned i, j;
	for (i = 0; i <8; i++) {
		nio_printf(c, "\n%p  ", addr);
		for (j = 0; j < 8; j++) {
			nio_printf(c, "%02X ", (unsigned)*(unsigned char *)addr++);
		}
	}
	nio_printf(c, "\n");
}
