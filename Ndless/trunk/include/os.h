/****************************************************************************
 * Ndless - main header file
 ****************************************************************************/

#ifndef _OS_H_
#define _OS_H_

#include "common.h"
#ifdef _NDLS_LIGHT
// Use the light version of the syscalls table
#include "syscalls-light.h"
#else
#include "syscalls.h"
#endif
/* GNU AS */
#ifdef GNU_AS
// unfortunately using a GAS macro for this is not possible
#define syscall(syscall_name) swi e_##syscall_name
#define osvar(osvar_name) swi (__SYSCALLS_ISVAR|e_##osvar_name)

/* GNU C Compiler */
#else

extern int __base;

#define _SYSCALL_ENUM(syscall_name) e_##syscall_name
/* The SWI calling convention is the C calling convention for the parameters.
 * We define:
 * - Macros for syscalls with 0, 1, 2, 3 and 4 parameters
 * - A macro fo syscalls with more than 4 parameters. We force GCC to pass the 5+ parameters through
 *   the stack using a variadic function and a wrapper with typed parameters.
 * - A macro for variadic syscalls
 * Caution, lr is destroyed by our swi calling convention (and r0-r3,r12 by the C calling convention of the syscalls) */
#define _SYSCALL0(rettype, funcname) static inline rettype funcname(void) { \
	register unsigned __r0 __asm("r0"); \
	__asm volatile( \
		" swi " STRINGIFY(_SYSCALL_ENUM(funcname)) \
		:"=r" (__r0) :: "memory", "r1", "r2", "r3", "r12", "lr"); \
	return (rettype)__r0; \
}
#define _SYSCALL1(rettype, funcname, type1) static inline rettype funcname(type1 __param1) { \
	register unsigned __r0 __asm("r0") = (unsigned)__param1; \
	__asm volatile( \
		" swi " STRINGIFY(_SYSCALL_ENUM(funcname)) \
		: "=r" (__r0) : "r" (__r0) :  "memory", "r1", "r2", "r3", "r12", "lr"); \
	return (rettype)__r0; \
}
// __r1 in output operand list gives an hint to GCC that r1 is clobbered. As an __asm-specifier for __r1, r1 cannot be put in the clobber list.
// "memory" in the list of the clobbered register is required for pointer parameters used for writes by the syscall, to avoid any caching.
#define _SYSCALL2(rettype, funcname, type1, type2) static inline rettype funcname(type1 __param1, type2 __param2) { \
	register unsigned __r0 __asm("r0") = (unsigned)__param1; \
	register unsigned __r1 __asm("r1") = (unsigned)__param2; \
	__asm volatile( \
		" swi " STRINGIFY(_SYSCALL_ENUM(funcname)) \
		: "=r" (__r0), "=r" (__r1) : "r" (__r0), "r" (__r1) : "memory", "r2", "r3", "r12", "lr"); \
	return (rettype)__r0; \
}
#define _SYSCALL3(rettype, funcname, type1, type2, type3) static inline rettype funcname(type1 __param1, type2 __param2, type3 __param3) { \
	register unsigned __r0 __asm("r0") = (unsigned)__param1; \
	register unsigned __r1 __asm("r1") = (unsigned)__param2; \
	register unsigned __r2 __asm("r2") = (unsigned)__param3; \
	__asm volatile( \
		" swi " STRINGIFY(_SYSCALL_ENUM(funcname)) \
		: "=r" (__r0), "=r" (__r1), "=r" (__r2) : "r" (__r0), "r" (__r1), "r" (__r2) : "memory", "r3", "r12", "lr"); \
	return (rettype)__r0; \
}
#define _SYSCALL4(rettype, funcname, type1, type2, type3, type4) static inline rettype funcname(type1 __param1, type2 __param2,  type3 __param3,  type4 __param4) { \
	register unsigned __r0 __asm("r0") = (unsigned)__param1; \
	register unsigned __r1 __asm("r1") = (unsigned)__param2; \
	register unsigned __r2 __asm("r2") = (unsigned)__param3; \
	register unsigned __r3 __asm("r3") = (unsigned)__param4; \
	__asm volatile( \
		" swi " STRINGIFY(_SYSCALL_ENUM(funcname)) \
		: "=r" (__r0), "=r" (__r1), "=r" (__r2), "=r" (__r3) : "r" (__r0), "r" (__r1), "r" (__r2) , "r" (__r3) : "memory", "r12", "lr"); \
	return (rettype)__r0; \
}
/* used to access through the got the global variable _syscallvar_savedlr. Returns the ptr to reg1. */
#ifndef __thumb__
#define _SYSCALL_GETSAVEDLR_PTR(reg1,tmpreg) \
		" ldr " #reg1 ", 1f \n" \
		" ldr " #tmpreg ", 1f+4 \n" \
		"0:	\n" \
		" add " #reg1 ", pc, " #reg1 " \n" \
		" ldr " #reg1 ", [" #reg1 ", " #tmpreg "] \n" \
		" b 2f \n" \
		" .align 2 \n" \
		"1: \n" \
		" .long _GLOBAL_OFFSET_TABLE_ - (0b+8) \n" \
		" .long _syscallvar_savedlr(GOT) \n" \
		"2: \n"
#else
#define _SYSCALL_GETSAVEDLR_PTR(reg1,tmpreg) \
		" ldr " #reg1 ", 1f \n" \
		" ldr " #tmpreg ", 1f+4 \n" \
		"0:	\n" \
		" add " #reg1 ", pc, " #reg1 " \n" \
		" ldr " #reg1 ", [" #reg1 ", " #tmpreg "] \n" \
		" b 2f \n" \
		" .align 2 \n" \
		"1: \n" \
		" .long _GLOBAL_OFFSET_TABLE_ - (0b+4) \n" \
		" .long _syscallvar_savedlr(GOT) \n" \
		"2: \n"
#endif // ndef __thumb__
/* all parameters must be marked with  __attribute__((unused)) */
#ifdef _NDLS_LIGHT // can't use _SYSCALL_GETSAVEDLR_PTR which depends on the GOT: save lr to the stack, syscallvars with more than 4 parameters can't be used
#define _SYSCALLVAR(rettype, attributes, funcname, param1, ...) static inline rettype attributes __attribute__((naked)) funcname(param1, __VA_ARGS__) { \
	register unsigned __r0 __asm("r0"); \
	__asm volatile( \
		" push {lr} \n" \
		" swi " STRINGIFY(_SYSCALL_ENUM(funcname)) "\n" \
	  " pop {pc}" \
		: "=r" (__r0):: "memory", "r1", "r2", "r3"); \
	return (rettype)__r0; \
}
#else
#ifndef __thumb__
#define _SYSCALLVAR(rettype, attributes, funcname, param1, ...) static inline rettype attributes __attribute__((naked)) funcname(param1, __VA_ARGS__) { \
	register unsigned __r0 __asm("r0"); \
	__asm volatile( \
		" push {r4, r5} \n" \
		_SYSCALL_GETSAVEDLR_PTR(r4, r5) \
		" str lr, [r4] \n" \
		" pop {r4, r5} \n" \
		" swi " STRINGIFY(_SYSCALL_ENUM(funcname)) "\n" \
		_SYSCALL_GETSAVEDLR_PTR(r1, r2) \
		" ldr pc, [r1] \n" \
		: "=r" (__r0):: "memory", "r1", "r2", "r3"); \
	return (rettype)__r0; \
}
#else // slightly less optimized
#define _SYSCALLVAR(rettype, attributes, funcname, param1, ...) static inline rettype attributes __attribute__((naked)) funcname(param1, __VA_ARGS__) { \
	register unsigned __r0 __asm("r0"); \
	__asm volatile( \
		" push {r4, r5} \n" \
		_SYSCALL_GETSAVEDLR_PTR(r4, r5) \
		" mov r5, lr \n" \
		" str r5, [r4] \n" \
		" pop {r4, r5} \n" \
		" swi " STRINGIFY(_SYSCALL_ENUM(funcname)) "\n" \
		_SYSCALL_GETSAVEDLR_PTR(r1, r2) \
		" ldr r1, [r1] \n" \
		" bx r1 \n" \
		: "=r" (__r0):: "memory", "r1", "r2", "r3"); \
	return (rettype)__r0; \
}
#endif // ndef __thumb__
#endif // def _NDLS_LIGHT
// We can't push it onto the stack during the syscall, so save it in a global variable
// attribute unused: avoids the warning. The symbol will be redefined by the ldscript.
static __attribute__ ((unused)) unsigned _syscallvar_savedlr;
/* Force the use of the stack for the parameters */
#ifndef __thumb__
#define _SYSCALL_SWI(rettype, attributes, funcname, param1) static inline rettype attributes __attribute__((naked)) funcname##_swi(param1, ...) { \
	register unsigned __r0 __asm("r0"); \
	__asm volatile( \
		" push {r4, r5} \n" \
		_SYSCALL_GETSAVEDLR_PTR(r4, r5) \
		" str lr, [r4] \n" \
		" pop {r4, r5} \n" \
		" swi " STRINGIFY(_SYSCALL_ENUM(funcname)) "\n" \
		_SYSCALL_GETSAVEDLR_PTR(r1, r2) \
		" ldr pc, [r1] \n" \
		: "=r" (__r0):: "memory", "r1", "r2", "r3"); \
	return (rettype)__r0; \
}
#else // slightly less optimized
#define _SYSCALL_SWI(rettype, attributes, funcname, param1) static inline rettype attributes __attribute__((naked)) funcname##_swi(param1, ...) { \
	register unsigned __r0 __asm("r0"); \
	__asm volatile( \
		" push {r4, r5} \n" \
		_SYSCALL_GETSAVEDLR_PTR(r4, r5) \
		" mov r5, lr \n" \
		" str r5, [r4] \n" \
		" pop {r4, r5} \n" \
		" swi " STRINGIFY(_SYSCALL_ENUM(funcname)) "\n" \
		_SYSCALL_GETSAVEDLR_PTR(r1, r2) \
		" ldr r1, [r1] \n" \
		" bx r1 \n" \
		: "=r" (__r0):: "memory", "r1", "r2", "r3"); \
	return (rettype)__r0; \
}
#endif // ndef __thumb__
#define _SYSCALL(rettype, funcname, param1, ...) _SYSCALL_SWI(rettype, funcname, param1) static inline rettype funcname(param1, __VA_ARGS__)
// Use in conjunction with _SYSCALL for 5+ parameters
#define _SYSCALL_ARGS(rettype, funcname, param1, ...) {return funcname##_swi(param1, __VA_ARGS__);}
/* Custom syscalls: these are syscalls currently not implemented by Ndless you want to define yourself.
 * The syntax is:
 * static const unsigned puts_addrs[] = {<1.7-address>, <1.7-CAS-address>, ...}; // see nl_osvalue for the order of the addresses
 * #define puts SYSCALL_CUSTOM(puts_addrs, int, const char *) */
#define SYSCALL_CUSTOM(addresses, rettype, ...) ((rettype(*)(__VA_ARGS__))nl_osvalue((int*)addresses, sizeof(addresses)/sizeof(addresses[0])))
/* Access to OS variables */
#define _SYSCALL_OSVAR(type, name) static inline type name(void) { \
	register unsigned __r0 __asm("r0"); \
	__asm volatile( \
		" swi " STRINGIFY(__SYSCALLS_ISVAR|_SYSCALL_ENUM(name)) \
		: "=r" (__r0) : "r" (__r0) :  "r1", "r2", "r3", "r12", "lr"); \
	return (type)__r0; \
}

/* OS syscalls */
_SYSCALL1(int, read_unaligned_longword, void *)
_SYSCALL1(int, read_unaligned_word, void *)
_SYSCALL3(void, ascii2utf16, void *, const char *, int)
_SYSCALL3(void, show_dialog_box2, int /* undef */, const char * /* title */, const char * /* msg */)

_SYSCALL0(int *, errno_addr)
#define errno (*errno_addr())

_SYSCALL1(int, isalpha, int)
_SYSCALL1(int, isascii, int)
_SYSCALL1(int, isdigit, int)
_SYSCALL1(int, islower, int)
_SYSCALL1(int, isprint, int)
_SYSCALL1(int, isspace, int)
_SYSCALL1(int, isupper, int)
_SYSCALL1(int, isxdigit, int)
_SYSCALL1(int, tolower, int)
_SYSCALL1(int, toupper, int)
_SYSCALL1(int, atoi, const char *)
_SYSCALL1(double, atof, char *)
_SYSCALL2(double, strtod, const char *, char *)
_SYSCALL3(long int, strtol, const char *, char **, int)
#define strtoul(s,e,b) ((unsigned long int)strtol((s),(e),(b)))

_SYSCALL1(void *, malloc, size_t)
_SYSCALL1(void, free, void *)
_SYSCALL2(void *, calloc, size_t, size_t)
_SYSCALL2(void *, realloc, void *, size_t)
_SYSCALL3(void *, memset, void *, int, size_t)
_SYSCALL3(void *, memcpy, void *, const void *, size_t)
_SYSCALL3(int, memcmp, const void *, const void *, size_t)
_SYSCALL3(void *, memmove, void *, const void *, size_t)
_SYSCALL2(void, memrev, char *, size_t)
_SYSCALL2(char *, strcpy, char *, const char *)
_SYSCALL3(char *, strncpy, char *, const char *, size_t)
_SYSCALL2(int, strcmp, const char *, const char *)
#define strcoll strcmp
_SYSCALL3(int, strncmp, const char *, const char *, size_t)
_SYSCALL1(int, strlen, const char *)
_SYSCALL3(char *, strncat, char *, const char *, size_t)
_SYSCALL2(char *, strchr, const char *, int)
_SYSCALL2(char *, strrchr, const char *, int)
_SYSCALL2(char *, strpbrk, const char *, const char *)
_SYSCALL1(char *, strerror, int)
_SYSCALL2(char *, strcat, char *, const char *)
_SYSCALL2(char *, strstr, const char *, const char *)

typedef __builtin_va_list va_list;
#define va_start(ap,p)  __builtin_va_start(ap, p)
#define va_arg(ap,type) __builtin_va_arg(ap, type)
#define va_end(ap) __builtin_va_end(ap)

_SYSCALLVAR(int, __attribute__((__format__(__printf__,1,2))), printf, __attribute__((unused)) const char *format, ...)
_SYSCALLVAR(int, __attribute__((__format__(__printf__,2,3))), sprintf, __attribute__((unused)) char *s, __attribute__((unused)) const char *format, ...)
_SYSCALLVAR(int, __attribute__((__format__(__printf__,2,3))), fprintf, __attribute__((unused)) FILE *stream, __attribute__((unused)) const char *format, ...)
// wrapper with cast to avoid the GCC warning "error: aggregate value used where an integer was expected"
#define vsprintf(str, fmt, arg) _vsprintf(str, fmt, *(void**)&arg)
_SYSCALL3(int, _vsprintf, char *, const char *, void*)

_SYSCALL1(int, puts, const char *)
_SYSCALL2(int, fputc, int, FILE *)
#define putc fputc
_SYSCALL1(int, fgetc, FILE *)
#define getc fgetc
_SYSCALL2(int, ungetc, int, FILE *)

_SYSCALL_OSVAR(FILE *, stdin)
#define stdin stdin()
_SYSCALL_OSVAR(FILE *, stdout)
#define stdout stdout()
_SYSCALL_OSVAR(FILE *, stderr)
#define stderr stderr()

_SYSCALL2(FILE *, fopen, const char *, const char *)
_SYSCALL3(FILE *, freopen, const char *, const char *, FILE *)
_SYSCALL4(size_t, fread, void *, size_t, size_t, FILE *)
_SYSCALL4(size_t, fwrite, const void *, size_t, size_t, FILE *)
_SYSCALL1(int, fflush, FILE *)
_SYSCALL1(int, fclose, FILE *)
_SYSCALL1(int, ferror, FILE *)
_SYSCALL3(int, fseek, FILE *, long int, int)
_SYSCALL1(int, remove, const char *)

_SYSCALL2(int, mkdir, const char *, int)
_SYSCALL1(int, chdir, char *)
_SYSCALL1(int, rmdir, const char *)
_SYSCALL2(int, stat, const char *, struct stat *)
_SYSCALL2(int, rename, const char *, const char *)
_SYSCALL1(int, unlink, const char *)

_SYSCALL1(int, TCT_Local_Control_Interrupts, int)

_SYSCALL2(int, NU_Current_Dir, const char *, const char *)
_SYSCALL2(int, NU_Get_First, struct dstat *, const char * /* pattern */)
_SYSCALL1(int, NU_Get_Next, struct dstat *)
_SYSCALL1(void, NU_Done, struct dstat *)
_SYSCALL1(int, NU_Set_Current_Dir, const char *)

/* 1: clickpad, 2: 84+, 3: touchpad prototype, 4: touchpad */
_SYSCALL_OSVAR(unsigned char *, keypad_type)
#define keypad_type keypad_type()

/* Ndless extensions. Not available in thumb state. */
// Given a list of OS-specific value and its size, returns the value for the current OS.
// The order must be: 1.7, 1.7 CAS, 2.0.1, 2.0.1 CAS, 2.1.0, 2.1.0 CAS
// If the array isn't enough long for the current OS, returns 0.
// You may cast 'values' from unsigned* to int*.
_SYSCALL2(int, nl_osvalue, const int * /* values */, unsigned /* size */)
// Relocates a global variable initialized with symbols (for example an array of function pointers)
#define nl_relocdata(ptr, size) nl_relocdatab(ptr, size, &__base)
_SYSCALL3(void, nl_relocdatab, unsigned * /* dataptr */, unsigned /* size */, void * /* base */)

/* stdlib replacements not directly available as syscalls */
extern unsigned __crt0exit;
extern unsigned __crt0_savedsp;
static inline void __attribute__((noreturn, naked)) exit(int __attribute__((unused)) status) {
	__asm volatile(
		" mov sp, %0 \n"
		" mov pc, %1"
		:: "r" (__crt0_savedsp), "r" (&__crt0exit));
	__builtin_unreachable();
}

#endif // GCC C
#endif
