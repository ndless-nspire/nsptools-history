/****************************************************************************
 * @(#) Demo program
 *
 * Copyright (C) 2010 by ANNEHEIM Geoffrey and ARMAND Olivier
 * Contact: geoffrey.anneheim@gmail.com / olivier.armand@gmail.com
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * RCSID $Id$
 ****************************************************************************/
 
  #include "headers/os.h"
  #include "headers/defines.h"
    
  .text

  .string "PRG"
  
_start: .global _start
  mov     r12, sp
  stmfd   sp!, {r1-r12, lr, pc}
  
  ldr     r0, =SCREEN_BASE_ADDRESS
  adr     r1, image
  ldr     r2, =SCREEN_BYTES_SIZE
  oscall  memcpy
  
_wait_key_enter_pressed:
  is_key_pressed  0x10, #KEY_NSPIRE_ENTER
  bne     _wait_key_enter_pressed

_wait_key_enter_released:
  is_key_pressed  0x10, #KEY_NSPIRE_ENTER
  beq     _wait_key_enter_released
  
exit:
  ldmfd   sp, {r1-r11, sp, pc}

  .ltorg @ required before image, else constant pool will be too far
image:  .incbin "demo_picture.bin"

  .end
