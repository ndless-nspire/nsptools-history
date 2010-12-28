/****************************************************************************
 * libndls definitions
 ****************************************************************************/

#ifndef _LIBNDLS_H_
#define _LIBNDLS_H_

BOOL any_key_pressed(void);
void clrscr(void);
int feof(FILE * stream);
char *fgets(char * str, int num, FILE *stream);
int fputs(const char *str, FILE *stream);
int isalnum(int c);
int iscntrl(int c);
void rewind(FILE * stream);
void sleep(unsigned millisec);
size_t strcspn(const char * str1, const char * str2);
size_t strspn(const char * str1, const char * str2);

/* defined in crt0.S */
extern BOOL is_touchpad;

#endif
