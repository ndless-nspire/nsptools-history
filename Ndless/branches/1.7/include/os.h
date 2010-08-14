/****************************************************************************
 * Ndless - main header file
 ****************************************************************************/

#ifndef _OS_H_
#define _OS_H_

#include "common.h"
#include "syscalls.h"

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
/* The following definitions for syscalls are quite complicated because: 
 * - the SWI calling convention is the C calling convention for the parameters
 * - without ellipsis in the SWI function definition, GCC optimizes the call and does not set the input registers
 * - defining a syscall macro for each parameter number as Linux does is cumbersome and is not suitable for more than 4 parameters, for which the stack should be used
 * Our solution is:
 * - to use a SWI variadic function for 2 parameters or more, with a wrapper with typed parameters. This kind of call cannot be inlined by GCC even if declared so.
 * - to use specific definitions for syscalls with 0 or 1 parameters.
 * - to use a specific definition for variadic syscalls (which are incompatible with the SWI variadic function) */
#define _SYSCALL_SWI(rettype, funcname, param1) static inline rettype __attribute__((naked))     funcname##_swi(param1, ...)   {asm(" swi " _XSTRINGIFY(_SYSCALL_ENUM(funcname)) "\n bx lr" ::: "r0", "r1", "r2", "r3");}
#define _SYSCALL0(rettype, funcname) static inline rettype                                       funcname(void)                {asm(" swi " _XSTRINGIFY(_SYSCALL_ENUM(funcname)) ::: "r0", "r1", "r2", "r3");}
#define _SYSCALL1(rettype, funcname, type1) static inline rettype                                funcname(type1 __param1)      {register unsigned __r0 asm("r0") = (unsigned)__param1; asm(" swi " _XSTRINGIFY(_SYSCALL_ENUM(funcname)) :: "r" (__r0) :  "r1", "r2", "r3");}
#define _SYSCALLVAR(rettype, funcname, param1, ...) static inline rettype __attribute__((naked)) funcname(param1, __VA_ARGS__) {asm(" swi " _XSTRINGIFY(_SYSCALL_ENUM(funcname)) "\n bx lr" ::: "r0", "r1", "r2", "r3");}
#define _SYSCALL(rettype, funcname, param1, ...) _SYSCALL_SWI(rettype, funcname, param1) static inline rettype funcname(param1, __VA_ARGS__)
// Use in conjunction with _SYSCALL
#define _SYSCALL_ARGS(rettype, funcname, param1, ...) {return funcname##_swi(param1, __VA_ARGS__);}

#define NULL ((void*)0)
typedef enum bool {FALSE = 0, TRUE = 1} BOOL;
typedef struct{} FILE;
typedef unsigned long size_t;
  
_SYSCALL(int, mkdir, char *path, int a, int b, int c, int d) _SYSCALL_ARGS(int, mkdir, path, a, b, c, d)
_SYSCALL1(void*, malloc, size_t)
_SYSCALLVAR(int __attribute__((__format__(__printf__,1,2))), printf, const char *format, ...)
_SYSCALL1(int, puts, const char *)
_SYSCALL1(int, TCT_Local_Control_Interrupts, int)

#endif // GCC C
#endif
