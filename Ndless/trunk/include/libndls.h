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

#include <stdint.h>

typedef struct {
	uint16_t width;
	uint16_t height;
} touchpad_info_t;

typedef struct {
	unsigned char contact; /* "touched". TRUE or FALSE */
	unsigned char proximity;
	uint16_t x;
	uint16_t y;
	unsigned char x_velocity;
	unsigned char y_velocity;
	uint16_t dummy;
	unsigned char pressed; /* "key pressed". TRUE or FALSE */
} touchpad_report_t;

/* internal */
typedef enum tpad_arrow {TPAD_ARROW_NONE, TPAD_ARROW_UP, TPAD_ARROW_UPRIGHT, TPAD_ARROW_RIGHT, TPAD_ARROW_RIGHTDOWN, TPAD_ARROW_DOWN, TPAD_ARROW_DOWNLEFT, TPAD_ARROW_LEFT, TPAD_ARROW_LEFTUP, TPAD_ARROW_CLICK} tpad_arrow_t;

/* os.h's dependencies depend on the definitions above... */
#include <os.h>

BOOL any_key_pressed(void);
void clear_cache(void);
void clrscr(void);
int feof(FILE * stream);
char *fgets(char * str, int num, FILE *stream);
int fputs(const char *str, FILE *stream);
void idle(void);
int isalnum(int c);
int iscntrl(int c);
void nputs(const char *str);
void nprintf(const char *fmt, ...);
void rewind(FILE * stream);
void show_msgbox(const char *title, const char *msg);
void sleep(unsigned millisec);
size_t strcspn(const char * str1, const char * str2);
size_t strspn(const char * str1, const char * str2);
touchpad_info_t *touchpad_getinfo(void);
int touchpad_scan(touchpad_report_t *report);
BOOL touchpad_arrow_pressed(tpad_arrow_t arrow); /* internal, use isKeyPressed() */
void wait_key_pressed(void);
void wait_no_key_pressed(void);

/* defined in crt0.S */
extern BOOL is_touchpad;

static inline void halt(void) {
	__asm volatile("0: b 0b");
}

#endif /* GNU_AS */
#endif /* _LIBNDLS_H_ */
