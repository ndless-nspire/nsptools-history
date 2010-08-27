/****************************************************************************
 * Ndless - main header file
 ****************************************************************************/

#ifndef _OS_H_
#define _OS_H_

#include "common.h"
#ifdef _SYSCALLS_LIGHT
// Use the light version of the syscalls table
#include "syscalls-light.h"
#else
#include "syscalls.h"
#endif

/** GNU AS */
#ifdef GNU_AS
  .macro oscall address
      mov	lr,pc
      ldr pc,=\address
  .endm

#define _oscall(rettype,funcaddr,...) funcaddr

/** GNU C Compiler */
#else
#define _oscall(rettype,funcaddr,...) (*((rettype(*)(__VA_ARGS__))(funcaddr)))
#define _STRINGIFY(s) #s
#define _XSTRINGIFY(s) _STRINGIFY(s)
#define _SYSCALL_ENUM(syscall_name) e_##syscall_name
/* The SWI calling convention is the C calling convention for the parameters.
 * We define:
 * - Macros for syscalls with 0, 1, 2, 3 and 4 parameters
 * - A macro fo syscalls with more than 4 parameters. We force GCC to pass the 5+ parameters through
 *   the stack using a variadic function and a wrapper with typed parameters.
 * - A macro for variadic syscalls
 * Caution, lr is destroyed by our swi calling convention (and r0-r3,r12 by the C calling convention of the syscalls) */
#define _SYSCALL0(rettype, funcname) static inline rettype funcname(void) { \
	register unsigned __r0 asm("r0"); \
	asm volatile( \
		" swi " _XSTRINGIFY(_SYSCALL_ENUM(funcname)) \
		:"=r" (__r0) :: "r0", "r1", "r2", "r3", "r12", "lr"); \
	return (rettype)__r0; \
}
#define _SYSCALL1(rettype, funcname, type1) static inline rettype funcname(type1 __param1) { \
	register unsigned __r0 asm("r0") = (unsigned)__param1; \
	asm volatile( \
		" swi " _XSTRINGIFY(_SYSCALL_ENUM(funcname)) \
		: "=r" (__r0) : "r" (__r0) :  "r1", "r2", "r3", "r12", "lr"); \
	return (rettype)__r0; \
}
#define _SYSCALL2(rettype, funcname, type1, type2) static inline rettype funcname(type1 __param1, type2 __param2) { \
	register unsigned __r0 asm("r0") = (unsigned)__param1; \
	register unsigned __r1 asm("r1") = (unsigned)__param2; \
	asm volatile( \
		" swi " _XSTRINGIFY(_SYSCALL_ENUM(funcname)) \
		: "=r" (__r0) : "r" (__r0), "r" (__r1) : "r2", "r3", "r12", "lr"); \
	return (rettype)__r0; \
}
#define _SYSCALL3(rettype, funcname, type1, type2, type3) static inline rettype funcname(type1 __param1, type2 __param2, type3 __param3) { \
	register unsigned __r0 asm("r0") = (unsigned)__param1; \
	register unsigned __r1 asm("r1") = (unsigned)__param2; \
	register unsigned __r2 asm("r2") = (unsigned)__param3; \
	asm volatile( \
		" swi " _XSTRINGIFY(_SYSCALL_ENUM(funcname)) \
		: "=r" (__r0) : "r" (__r0), "r" (__r1), "r" (__r2) : "r3", "r12", "lr"); \
	return (rettype)__r0; \
}
#define _SYSCALL4(rettype, funcname, type1, type2, type3, type4) static inline rettype funcname(type1 __param1, type2 __param2,  type3 __param3,  type4 __param4) { \
	register unsigned __r0 asm("r0") = (unsigned)__param1; \
	register unsigned __r1 asm("r1") = (unsigned)__param2; \
	register unsigned __r2 asm("r2") = (unsigned)__param3; \
	register unsigned __r3 asm("r3") = (unsigned)__param4; \
	asm volatile( \
		" swi " _XSTRINGIFY(_SYSCALL_ENUM(funcname)) \
		: "=r" (__r0) : "r" (__r0), "r" (__r1), "r" (__r2) , "r" (__r3) : "r12", "lr"); \
	return (rettype)__r0; \
}
#define _SYSCALLVAR(rettype, funcname, param1, ...) static rettype __attribute__((naked)) funcname(param1, __VA_ARGS__) { \
	asm volatile( \
		" push {lr}\n swi " _XSTRINGIFY(_SYSCALL_ENUM(funcname)) "\n" \
		" ldr pc, [sp], #4" \
		::: "r0", "r1", "r2", "r3"); \
}
// Force the use of the stack for the parameters
#define _SYSCALL_SWI(rettype, funcname, param1) static rettype __attribute__((naked)) funcname##_swi(param1, ...) { \
	asm volatile( \
		" push {lr}\n swi " _XSTRINGIFY(_SYSCALL_ENUM(funcname)) "\n" \
	  " ldr pc, [sp], #4" \
		::: "r0", "r1", "r2", "r3"); \
	}
#define _SYSCALL(rettype, funcname, param1, ...) _SYSCALL_SWI(rettype, funcname, param1) static inline rettype funcname(param1, __VA_ARGS__)
// Use in conjunction with _SYSCALL for 5+ parameters
#define _SYSCALL_ARGS(rettype, funcname, param1, ...) {return funcname##_swi(param1, __VA_ARGS__);}

#define NULL ((void*)0)
typedef enum bool {FALSE = 0, TRUE = 1} BOOL;
typedef struct{} FILE;
typedef unsigned long size_t;
  
_SYSCALL1(void*, malloc, size_t)
_SYSCALL1(void, free, void *)
_SYSCALL3(void*, memset, void *, int, size_t)
_SYSCALL3(void*, memcpy, void *, const void *, size_t)
_SYSCALL3(int, memcmp, const void *, const void *, size_t)
_SYSCALLVAR(int __attribute__((__format__(__printf__,1,2))), printf, const char *format, ...)
_SYSCALLVAR(int __attribute__((__format__(__printf__,2,3))), sprintf, char *s, const char *format, ...)
_SYSCALL1(int, puts, const char *)
_SYSCALL1(int, TCT_Local_Control_Interrupts, int)
_SYSCALL2(FILE*, fopen, const char *, const char *)
_SYSCALL4(size_t, fread, void *, size_t, size_t, FILE *)
_SYSCALL4(size_t, fwrite, const void *, size_t, size_t, FILE *)
_SYSCALL1(int, fclose, FILE *)
_SYSCALL2(int, mkdir, const char*, int)
_SYSCALL2(int, stat, const char *, struct stat *)
_SYSCALL2(int, NU_Get_First, struct dstat *, const char * /* pattern */)
_SYSCALL1(int,  NU_Get_Next, struct dstat *)
_SYSCALL1(void, NU_Done, struct dstat *)

#endif // GCC C
#endif
