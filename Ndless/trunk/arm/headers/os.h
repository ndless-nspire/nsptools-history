/****************************************************************************
 * @(#) Ndless - OS Calls
 *
 * Copyright (C) 2010 by ANNEHEIM Geoffrey and ARMAND Olivier
 * Contact: geoffrey.anneheim@gmail.com / olivier.calc@gmail.com
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * RCSID $Id$
 ****************************************************************************/

#ifndef _OS_H_
#define _OS_H_

#include "common.h"

#ifdef CAS
  #include "os_cas_1.1.9170.h"
#elif NON_CAS
  #include "os_1.1.9253.h"
#else
  #error You must specify a Nspire hardware (CAS or NON-CAS)
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

  #define NULL ((void*)0)
  typedef enum bool {FALSE = 0, TRUE = 1} BOOL;
  typedef struct{} FILE;
  typedef unsigned long size_t;
#endif

#define fopen             (_oscall(FILE*, fopen_, const char* filename, const char* mode))
#define fread             (_oscall(size_t, fread_, void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream))
#define fwrite            (_oscall(size_t, fwrite_, const void* ptr, size_t size, size_t count, FILE* stream))
#define fclose            (_oscall(int, fclose_, FILE* stream))
#define malloc            (_oscall(void*, malloc_, size_t size))
#define free              (_oscall(void, free_, void* ptr))
#define memset            (_oscall(void*, memset_, void* ptr, int value, size_t num))
#define memcpy            (_oscall(void *, memcpy_, void *restrict s1, const void *restrict s2, size_t n))
#define memcmp            (_oscall(int, memcmp_, const void * ptr1, const void * ptr2, size_t num ))
#define sprintf           (_oscall(int, sprintf_, char *restrict s, const char *restrict format, ...))
#define ascii2utf16       (_oscall(void, ascii2utf16_, void* buff, const char* str, int max_size))
#define show_dialog_box2  (_oscall(void, show_dialog_box2_, int undef, const char* title, const char* msg))
#define mkdir             (_oscall(int, mkdir_, const char* path, int mode))
#define rmdir             (_oscall(int, rmdir_, const char *path))
#define set_current_path  (_oscall(void, set_current_path_, const char*))
#define stat              (_oscall(int, stat_, const char *restric path, struct stat *restrict buf))
#define unlink            (_oscall(int, unlink_, const char *))
#define rename            (_oscall(int, rename_, const char *old, const char *new))
#define log_rs232         (_oscall(void, log_rs232_, const char*))
//#define printf_rs232      (_oscall(void, printf_rs232_, const char*, ...))
#define power_off         (_oscall(void, power_off_, void))
#define TCT_Local_Control_Interrupts (_oscall(int, TCT_Local_Control_Interrupts_, int newlevel))

#endif
