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

/** GNU C Compiler */
#else
  #define _oscall(rettype,funcname,...) (*((rettype(*)(__VA_ARGS__))(funcname)))

  #define NULL ((void*)0)
  typedef struct{} FILE;
  typedef unsigned long size_t;
  #define fopen             (_oscall(FILE*, fopen_, const char* filename, const char* mode))
  #define fwrite            (_oscall(size_t, fwrite_, const void* ptr, size_t size, size_t count, FILE* stream))
  #define fclose            (_oscall(int, fclose_, FILE* stream))
  #define malloc            (_oscall(void*, malloc_, size_t size))
  #define free              (_oscall(void, free_, void* ptr))
  #define memset            (_oscall(void*, memset_, void* ptr, int value, size_t num))
  #define mkdir             (_oscall(int, mkdir_, const char* path, int mode))
  #define ascii2utf16       (_oscall(void, ascii2utf16_, void* buff, const char* str, int max_size))
  #define show_dialog_box2  (_oscall(void, show_dialog_box2_, int undef, const char* title, const char* msg))
  #define log_rs232         (_oscall(void, log_rs232_, const char*))
  //#define printf_rs232      (_oscall(void, printf_rs232_, const char*, ...))  
#endif

#endif
