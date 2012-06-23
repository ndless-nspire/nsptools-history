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
#include <sys/types.h>

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
	unsigned char arrow; /* area of the pad currently touched. see tpad_arrow_t. */
} touchpad_report_t;

typedef enum tpad_arrow {TPAD_ARROW_NONE, TPAD_ARROW_UP, TPAD_ARROW_UPRIGHT, TPAD_ARROW_RIGHT, TPAD_ARROW_RIGHTDOWN, TPAD_ARROW_DOWN, TPAD_ARROW_DOWNLEFT, TPAD_ARROW_LEFT, TPAD_ARROW_LEFTUP, TPAD_ARROW_CLICK} tpad_arrow_t;

/* for set_cpu_speed() */
#define CPU_SPEED_150MHZ 0x00000002
#define CPU_SPEED_120MHZ 0x000A1002
#define CPU_SPEED_90MHZ  0x00141002

/* os.h's dependencies depend on the definitions above... */
#include <os.h>

void assert_ndless_rev(unsigned required_rev);
BOOL any_key_pressed(void);
void clear_cache(void);
void clrscr(void);
int feof(FILE * stream);
char *fgets(char * str, int num, FILE *stream);
int fputs(const char *str, FILE *stream);
void idle(void);
int isalnum(int c);
int iscntrl(int c);
BOOL lcd_isincolor(void);
void lcd_incolor(void);
void lcd_ingray(void);
void nputs(const char *str);
void nprintf(const char *fmt, ...);
BOOL on_key_pressed(void);
void refresh_osscr(void);
void rewind(FILE * stream);
unsigned _scrsize(void);
unsigned set_cpu_speed(unsigned speed);
unsigned _show_msgbox(const char *title, const char *msg, unsigned button_num, ...);
int show_msg_user_input(const char * title, const char * msg, char * defaultvalue, char ** value_ref);
int show_1numeric_input(const char * title, const char * subtitle, const char * msg, int * value_ref, int min_value, int max_value);
int show_2numeric_input(const char * title, const char * subtitle, const char * msg1, int * value1_ref, int min_value1, int max_value1, const char * msg2, int * value2_ref, int min_value2, int max_value2);
void sleep(unsigned millisec);
size_t strcspn(const char * str1, const char * str2);
size_t strspn(const char * str1, const char * str2);
touchpad_info_t *touchpad_getinfo(void);
int touchpad_scan(touchpad_report_t *report);
BOOL touchpad_arrow_pressed(tpad_arrow_t arrow); /* internal, use isKeyPressed() */
int truncate(const char *path, off_t length);
void wait_key_pressed(void);
void wait_no_key_pressed(void);

BOOL _is_touchpad(void);
#define is_touchpad _is_touchpad()

static inline void halt(void) {
	__asm volatile("0: b 0b");
}

static inline void bkpt(void) {
	asm(".long 0xE1212374");
}

#define show_msgbox(title, msg) _show_msgbox(title, msg, 0)
#define show_msgbox_2b(title, msg, button1, button2) _show_msgbox(title, msg, 2, button1, button2)
#define show_msgbox_3b(title, msg, button1, button2, button3) _show_msgbox(title, msg, 3, button1, button2, button3)

unsigned hwtype(void);
#define is_cx (hwtype() == 1)
#define is_classic (hwtype() < 1)
#define has_colors (!is_classic)
#define IO(a,b) (((volatile unsigned*[]){ (unsigned*)a, (unsigned*)b })[hwtype()])
#define IO_LCD_CONTROL IO(0xC000001C, 0xC0000018)

#define SCREEN_BASE_ADDRESS     ADDR_(*(volatile unsigned*)0xC0000010)
#define SCREEN_BYTES_SIZE       ((int)({_scrsize();}))
#define SCREEN_WIDTH            320
#define SCREEN_HEIGHT           240
#define BLACK                   0x0
#define WHITE                   0xF


#endif /* GNU_AS */
#endif /* _LIBNDLS_H_ */
