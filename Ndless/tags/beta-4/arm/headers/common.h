/****************************************************************************
 * @(#) Ndless - Common definitions
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

#ifndef _COMMON_H_
#define _COMMON_H_

/***********************************
 * Hardware
 ***********************************/
#define SCREEN_BASE_ADDRESS     ADDR_(0xA4000100)
#define KEY_MAP                 ADDR_(0x900E0000)

/***********************************
 * Addresses
 ***********************************/
#define OS_BASE_ADDRESS         ADDR_(0x10000000)

/***********************************
 * Others
 ***********************************/
#define SCREEN_BYTES_SIZE       38400
#define SCREEN_WIDTH            320
#define SCREEN_HEIGHT           240

#define BLACK                   0x0
#define WHITE                   0xF

/***********************************
 * Keys
 ***********************************/
#define KEY_NSPIRE_RET          KEY_(0x10, 0x001)
#define KEY_NSPIRE_ENTER        KEY_(0x10, 0x002)
#define KEY_NSPIRE_PLUS         KEY_(0x12, 0x002)
#define KEY_NSPIRE_MINUS        KEY_(0x14, 0x002)
#define KEY_NSPIRE_MULTIPLY     KEY_(0x16, 0x002)
#define KEY_NSPIRE_DIVIDE       KEY_(0x18, 0x002)
#define KEY_NSPIRE_ESC          KEY_(0x1C, 0x080)
#define KEY_NSPIRE_C            KEY_(0x1C, 0x004)
#define KEY_NSPIRE_S            KEY_(0x14, 0x004)
#define KEY_NSPIRE_T            KEY_(0x12, 0x100)
#define KEY_NSPIRE_THETA        KEY_(0x10, 0x400)
#define KEY_NSPIRE_PI           KEY_(0x12, 0x400)

/** GNU AS */
#ifdef GNU_AS
  #define DIESE(s)          #
  #define ADDR_(addr)       addr
  #define KEY_(row, col)    row, DIESE(s)##col

  .macro isKeyPressed row, col
    ldr     r0, =(KEY_MAP + \row)
    ldrh    r0, [r0]
    tst     r0, \col
  .endm

/** GNU C Compiler */
#else
  typedef struct {
    int row, col;
  } t_key;

  #define ADDR_(addr)             (void*)addr
  #define KEY_(row, col)          (t_key){row, col}
  #define isKeyPressed(key)       (!((*(short*)(KEY_MAP + (key).row)) & (key).col))
#endif

#endif
