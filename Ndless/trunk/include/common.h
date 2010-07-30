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
 * Keys (key=(offset, 2^bit #)
 ***********************************/
#define KEY_NSPIRE_RET          KEY_(0x10, 0x001)
#define KEY_NSPIRE_ENTER        KEY_(0x10, 0x002)
#define KEY_NSPIRE_SPACE        KEY_(0x10, 0x004)
#define KEY_NSPIRE_NEGATIVE     KEY_(0x10, 0x008)
#define KEY_NSPIRE_Z            KEY_(0x10, 0x010)
#define KEY_NSPIRE_PERIOD       KEY_(0x10, 0x020)
#define KEY_NSPIRE_Y            KEY_(0x10, 0x040)
#define KEY_NSPIRE_0            KEY_(0x10, 0x080)
#define KEY_NSPIRE_X            KEY_(0x10, 0x100)
#define KEY_NSPIRE_THETA        KEY_(0x10, 0x400)
#define KEY_NSPIRE_COMMA        KEY_(0x12, 0x001)
#define KEY_NSPIRE_PLUS         KEY_(0x12, 0x002)
#define KEY_NSPIRE_W            KEY_(0x12, 0x004)
#define KEY_NSPIRE_3            KEY_(0x12, 0x008)
#define KEY_NSPIRE_V            KEY_(0x12, 0x010)
#define KEY_NSPIRE_2            KEY_(0x12, 0x020)
#define KEY_NSPIRE_U            KEY_(0x12, 0x040)
#define KEY_NSPIRE_1            KEY_(0x12, 0x080)
#define KEY_NSPIRE_T            KEY_(0x12, 0x100)
#define KEY_NSPIRE_eEXP         KEY_(0x12, 0x200)
#define KEY_NSPIRE_PI           KEY_(0x12, 0x400)
#define KEY_NSPIRE_QUES         KEY_(0x14, 0x001)
#define KEY_NSPIRE_MINUS        KEY_(0x14, 0x002)
#define KEY_NSPIRE_S            KEY_(0x14, 0x004)
#define KEY_NSPIRE_6            KEY_(0x14, 0x008)
#define KEY_NSPIRE_R            KEY_(0x14, 0x010)
#define KEY_NSPIRE_5            KEY_(0x14, 0x020)
#define KEY_NSPIRE_Q            KEY_(0x14, 0x040)
#define KEY_NSPIRE_4            KEY_(0x14, 0x080)
#define KEY_NSPIRE_P            KEY_(0x14, 0x100)
#define KEY_NSPIRE_TENX         KEY_(0x14, 0x200)
#define KEY_NSPIRE_EE           KEY_(0x14, 0x400)
#define KEY_NSPIRE_COLON        KEY_(0x16, 0x001)
#define KEY_NSPIRE_MULTIPLY     KEY_(0x16, 0x002)
#define KEY_NSPIRE_O            KEY_(0x16, 0x004)
#define KEY_NSPIRE_9            KEY_(0x16, 0x008)
#define KEY_NSPIRE_N            KEY_(0x16, 0x010)
#define KEY_NSPIRE_8            KEY_(0x16, 0x020)
#define KEY_NSPIRE_M            KEY_(0x16, 0x040)
#define KEY_NSPIRE_7            KEY_(0x16, 0x080)
#define KEY_NSPIRE_L            KEY_(0x16, 0x100)
#define KEY_NSPIRE_SQU          KEY_(0x16, 0x200)
#define KEY_NSPIRE_II           KEY_(0x16, 0x400) 
#define KEY_NSPIRE_QUOTE        KEY_(0x18, 0x001)
#define KEY_NSPIRE_DIVIDE       KEY_(0x18, 0x002)
#define KEY_NSPIRE_K            KEY_(0x18, 0x004)
#define KEY_NSPIRE_TAN          KEY_(0x18, 0x008)
#define KEY_NSPIRE_J            KEY_(0x18, 0x010)
#define KEY_NSPIRE_COS          KEY_(0x18, 0x020)
#define KEY_NSPIRE_I            KEY_(0x18, 0x040)
#define KEY_NSPIRE_SIN          KEY_(0x18, 0x080)
#define KEY_NSPIRE_H            KEY_(0x18, 0x100)
#define KEY_NSPIRE_EXP          KEY_(0x18, 0x200)
#define KEY_NSPIRE_GTHAN        KEY_(0x18, 0x400)
#define KEY_NSPIRE_APOSTROPHE   KEY_(0x1A, 0x001)
#define KEY_NSPIRE_CAT          KEY_(0x1A, 0x002)
#define KEY_NSPIRE_G            KEY_(0x1A, 0x004)
#define KEY_NSPIRE_RP           KEY_(0x1A, 0x008)
#define KEY_NSPIRE_F            KEY_(0x1A, 0x010)
#define KEY_NSPIRE_LP           KEY_(0x1A, 0x020)
#define KEY_NSPIRE_E            KEY_(0x1A, 0x040)
#define KEY_NSPIRE_VAR          KEY_(0x1A, 0x080)
#define KEY_NSPIRE_D            KEY_(0x1A, 0x100)
#define KEY_NSPIRE_CAPS         KEY_(0x1A, 0x200)
#define KEY_NSPIRE_LTHAN        KEY_(0x1A, 0x400)
#define KEY_NSPIRE_FLAG         KEY_(0x1C, 0x001)
#define KEY_NSPIRE_CLICK        KEY_(0x1C, 0x002)
#define KEY_NSPIRE_C            KEY_(0x1C, 0x004)
#define KEY_NSPIRE_HOME         KEY_(0x1C, 0x008)
#define KEY_NSPIRE_B            KEY_(0x1C, 0x010)
#define KEY_NSPIRE_MENU         KEY_(0x1C, 0x020)
#define KEY_NSPIRE_A            KEY_(0x1C, 0x040)
#define KEY_NSPIRE_ESC          KEY_(0x1C, 0x080)
#define KEY_NSPIRE_BAR          KEY_(0x1C, 0x100)
#define KEY_NSPIRE_TAB          KEY_(0x1C, 0x200)
#define KEY_NSPIRE_EQU          KEY_(0x1C, 0x400)
#define KEY_NSPIRE_UP           KEY_(0x1E, 0x001)
#define KEY_NSPIRE_UPRIGHT      KEY_(0x1E, 0x002)
#define KEY_NSPIRE_RIGHT        KEY_(0x1E, 0x004)
#define KEY_NSPIRE_RIGHTDOWN    KEY_(0x1E, 0x008)
#define KEY_NSPIRE_DOWN         KEY_(0x1E, 0x010)
#define KEY_NSPIRE_DOWNLEFT     KEY_(0x1E, 0x020)
#define KEY_NSPIRE_LEFT         KEY_(0x1E, 0x040)
#define KEY_NSPIRE_LEFTUP       KEY_(0x1E, 0x080)
#define KEY_NSPIRE_CLEAR        KEY_(0x1E, 0x100)
#define KEY_NSPIRE_CTRL         KEY_(0x1E, 0x200)

/* TI-84+ Keypad Mappings */
#define KEY_84_DOWN            KEY_(0x10, 0x001)
#define KEY_84_LEFT            KEY_(0x10, 0x002)
#define KEY_84_RIGHT           KEY_(0x10, 0x004)
#define KEY_84_UP              KEY_(0x10, 0x008)
#define KEY_84_ENTER           KEY_(0x12, 0x001)
#define KEY_84_PLUS            KEY_(0x12, 0x002)
#define KEY_84_MINUS           KEY_(0x12, 0x004)
#define KEY_84_MULTIPLY        KEY_(0x12, 0x008)
#define KEY_84_DIVIDE          KEY_(0x12, 0x010)
#define KEY_84_EXP             KEY_(0x12, 0x020)
#define KEY_84_CLEAR           KEY_(0x12, 0x040)
#define KEY_84_NEGATIVE        KEY_(0x14, 0x001)
#define KEY_84_3               KEY_(0x14, 0x002)
#define KEY_84_6               KEY_(0x14, 0x004)
#define KEY_84_9               KEY_(0x14, 0x008)
#define KEY_84_RP              KEY_(0x14, 0x010)
#define KEY_84_TAN             KEY_(0x14, 0x020)
#define KEY_84_VARS            KEY_(0x14, 0x040)
#define KEY_84_PERIOD          KEY_(0x16, 0x001)
#define KEY_84_2               KEY_(0x16, 0x002)
#define KEY_84_5               KEY_(0x16, 0x004)
#define KEY_84_8               KEY_(0x16, 0x008)
#define KEY_84_LP              KEY_(0x16, 0x010)
#define KEY_84_COS             KEY_(0x16, 0x020)
#define KEY_84_PRGM            KEY_(0x16, 0x040)
#define KEY_84_STAT            KEY_(0x16, 0x080)
#define KEY_84_0               KEY_(0x18, 0x001)
#define KEY_84_1               KEY_(0x18, 0x002)
#define KEY_84_4               KEY_(0x18, 0x004)
#define KEY_84_7               KEY_(0x18, 0x008)
#define KEY_84_COMMA           KEY_(0x18, 0x010)
#define KEY_84_SIN             KEY_(0x18, 0x020)
#define KEY_84_APPS            KEY_(0x18, 0x040)
#define KEY_84_X               KEY_(0x18, 0x080)
#define KEY_84_STO             KEY_(0x1A, 0x002)
#define KEY_84_LN              KEY_(0x1A, 0x004)
#define KEY_84_LOG             KEY_(0x1A, 0x008)
#define KEY_84_SQU             KEY_(0x1A, 0x010)
#define KEY_84_INV             KEY_(0x1A, 0x020)
#define KEY_84_MATH            KEY_(0x1A, 0x040)
#define KEY_84_ALPHA           KEY_(0x1A, 0x080)
#define KEY_84_GRAPH           KEY_(0x1C, 0x001)
#define KEY_84_TRACE           KEY_(0x1C, 0x002)
#define KEY_84_ZOOM            KEY_(0x1C, 0x004)
#define KEY_84_WIND            KEY_(0x1C, 0x008)
#define KEY_84_YEQU            KEY_(0x1C, 0x010)
#define KEY_84_2ND             KEY_(0x1C, 0x020)
#define KEY_84_MODE            KEY_(0x1C, 0x040)
#define KEY_84_DEL             KEY_(0x1C, 0x080)

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
  
  .macro halt
halt\@: b halt\@
  .endm

/** GNU C Compiler */
#else
  typedef struct {
    int row, col;
  } t_key;

  #define ADDR_(addr)             (void*)addr
  #define KEY_(row, col)          (t_key){row, col}
  #define isKeyPressed(key)       (!((*(short*)(KEY_MAP + (key).row)) & (key).col))

/***********************************
 * Misc inline functions
 ***********************************/

static inline void HALT(void) {
  asm volatile("0: b 0b");
}

/* switch to low-power state until next interrupt */
static inline void idle(void) {
  unsigned int sbz = 0;
  asm volatile ("mcr p15, 0, %0, c7, c0, 4" : "=r"(sbz) );
}
 
#endif /* GCC */

#endif
