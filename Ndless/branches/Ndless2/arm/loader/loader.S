/****************************************************************************
 * @(#) Ndless - Loader
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
 
  #include "loader.h"
  #include "../../include/syscalls/syscalls.h"

  .text
  
_start: .global _start
  # Interrupt vectors
  andeq   r0, r0, r0
  andeq   r0, r0, r0
  andeq   r0, r0, r0
  andeq   r0, r0, r0
  andeq   r0, r0, r0
  andeq   r0, r0, r0
  andeq   r0, r0, r0
  andeq   r0, r0, r0
  
  # Interrupt addresses
  andeq   r0, r0, r0
  andeq   r0, r0, r0
  andeq   r0, r0, r0
  andeq   r0, r0, r0
  andeq   r0, r0, r0
  andeq   r0, r0, r0
  andeq   r0, r0, r0
  andeq   r0, r0, r0
  
  # Set stack pointer
  ldr     sp, =STACK_PTR_BASE_ADDRESS
  
  bl      get_os_version_index
  # TODO: OS Unknown?
  bl      get_os_syscalls_index
  # TODO: Syscalls index invalid
  
  ldr     r1, =SYSCALLS_TABLE_BYTES_SIZE
  adr     r2, basic_syscalls_table_begin
  bl      install_vector_swi
  
  # Desactivate interrupts
  mov     r0, #0xFFFFFFFF
  swi     TCT_Local_Control_Interrupts
  
  # Open ndless.tns
  adr     r0, fileNdless
  adr     r1, fileMode_rb
  swi     fopen
  mov     r4, r0
  
  ldr     r0, =MEMSPACE_BASE_ADDRESS
  mov     r1, #1
  ldr     r2, =MEMSPACE_MAX_BYTES_SIZE
  mov     r3, r4
  swi     fread
  
  mov     r0, r4
  swi     fclose
  
  # Execute ndless code
  ldr     pc, =MEMSPACE_BASE_ADDRESS

fileMode_rb:          .string "rb"
fileNdless:           .string "/documents/ndless/demo.tns"
  .align    

basic_syscalls_table_begin:
  #include "../../include/syscalls/syscalls_table.S"
  .align

  .end
