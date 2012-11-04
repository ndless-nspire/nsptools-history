#ifndef _OCD_H

/* Must be called at the beginning of the program to debug */
void ocd_init(void);
/* Must be called at the end of the program to debug */
void ocd_cleanup(void);
/* Hard-coded breakpoint: sows the debugger console when executed */
static inline void ocd_break(void) {
	asm(".long 0xE12FFF7F"); // immed = FFFF
}
/* Programmatically sets a breakpoint at the give address */
void ocd_set_breakpoint(unsigned addr);
/* Programmatically remove the breakpoint at the give address */
void ocd_remove_breakpoint(unsigned addr);

#endif
