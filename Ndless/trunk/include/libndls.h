/****************************************************************************
 * libndls definitions
 ****************************************************************************/

#ifndef _LIBNDLS_H_
#define _LIBNDLS_H_

#ifdef GNU_AS
	.macro halt
halt\@: b halt\@
	.endm

#else /* GNU_AS */

BOOL any_key_pressed(void);
void clear_cache(void);
void clrscr(void);
int feof(FILE * stream);
char *fgets(char * str, int num, FILE *stream);
int fputs(const char *str, FILE *stream);
int isalnum(int c);
int iscntrl(int c);
void rewind(FILE * stream);
void show_msgbox(const char *title, const char *msg);
void sleep(unsigned millisec);
size_t strcspn(const char * str1, const char * str2);
size_t strspn(const char * str1, const char * str2);

/* defined in crt0.S */
extern BOOL is_touchpad;

static inline void halt(void) {
	__asm volatile("0: b 0b");
}

static inline void idle(void) {
  unsigned int sbz = 0;
  __asm volatile("mcr p15, 0, %0, c7, c0, 4" : "=r"(sbz) );
}

#endif /* GNU_AS */
#endif /* _LIBNDLS_H_ */
