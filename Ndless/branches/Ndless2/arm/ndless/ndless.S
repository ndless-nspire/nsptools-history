/****************************************************************************
 * @(#) Ndless
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
  
  #include "ndless.h"
  #include "../../include/os.h"
  #include "../../include/syscalls/syscalls.h"  
      
  .text
  
_start: .global _start
  bl      get_os_version_index
  bl      get_os_syscalls_index
  mov     r3, r0
  
  ldr     r1, =SYSCALLS_TABLE_BYTES_SIZE
  adr     r2, syscalls_table
  bl      install_vector_swi
  
  # Save vector swi for a futur reboot
  ldr     r0, =0xA4000028
  ldr     r1, =0x10000028
  ldr     r0, [r0]
  str     r0, [r1]
  
  # Restore OS flag for a proper OS reboot
  mov     r0, r3
  bl      restore_os_flags
  
  # Get hook base address from OS syscalls table index
  mov     r0, r3
  bl      get_hook_table_base_address
  
  # Write hook dynamic addresses
  ldr     r1, [r0]
  add     r1, r1, #8
  adr     r2, os_offset_hacked
  str     r1, [r2]
  
  ldr     r1, [r0, #4]
  adr     r2, os_shadowed_call
  str     r1, [r2]
  
  # Inject hook address (open_document)
  ldr     r0, [r0]
  ldr     r1, =0xE51FF004     @ ldr pc, [pc, #-4]
  str     r1, [r0]
  adr     r1, open_document
  str     r1, [r0, #4]
  
  ldr     pc, =0x10000000

os_offset_hacked:
  .long 0 
os_shadowed_call:
  .long 0

_restore:
  cmp     r0, #0              @ execute the OS instruction previously erased by install_hook
  ldr     pc, os_offset_hacked
    
open_document:
  # Save OS state
  stmfd   sp!, {r0-r12, lr}
  
  # r0 = Folder name
  # r1 = File name
  bl      _exec_program
  cmp     r0, #0              @ it's a TI document
  beq     _ti_open_file
  
_exit: 
  # Restore OS state
  ldmfd   sp!, {r0-r12, lr}
  mov     r0, #0              @ document not opened
  b       _restore

_ti_open_file:
  # Restore OS state
  ldmfd   sp!, {r0-r12, lr}
  
  # Restore OS instructions
  mov     lr, pc
  ldr     pc, os_shadowed_call @ call the OS function that open a TI document   
  b       _restore

_exec_program:
  mov     r12, sp
  stmfd   sp!, {r1-r3, r9-r12, lr, pc}
  sub     sp, sp, #0x100
  
  # Create file path
  mov     r9, sp
  mov     r2, r0
  mov     r3, r1
  mov     r0, r9
  adr     r1, pathFormat
  swi     sprintf
  
  # Open file
  mov     r0, r9
  bl      openFileBufferized
  cmp     r0, #0              @ cannot read file content (out of memory)
  beq     _exec_program_exit_error
  
  # Check signature
  mov     r10, r0              @ file buffer
  adr     r1, prgSignature
  mov     r2, #PRG_SIGNATURE_BYTES_SIZE
  swi     memcmp
  cmp     r0, #0              @ isn't a program (i.e is a TI document)
  bne     _exec_program_exit_error
  
  # Desactivate interrupts
  mov     r0, #0xFFFFFFFF
  swi     TCT_Local_Control_Interrupts
  mov     r11, r0
  
  # Run code
  # r11 = Interrupts state
  # r10 = Program block address
  # r9 = Program path
  add     r0, r10, #PRG_SIGNATURE_BYTES_SIZE @ jump signature
  mov     lr, pc
  mov     pc, r0
  
  # Restore interrupts
  mov     r0, r11
  swi     TCT_Local_Control_Interrupts
  
  # Unallocate memory
  mov     r0, r10
  swi     free
  mov     r0, #1              @ the program has been executed
  
_exec_program_exit:
  add     sp, sp, #0x100
  ldmfd   sp, {r1-r3, r9-r11, sp, pc}

# Could not execute the program, it's a TI document
_exec_program_exit_error:
  mov     r0, #0
  b       _exec_program_exit

prgSignature:       .string "PRG"
pathFormat:         .string "/documents/%s/%s"
  .align   

# --------------------------------------------------------------------------
# Get hook base address from a OS syscalls table index
#
# Input:
#   r0 = OS syscalls table index
#
# Output:
#
# --------------------------------------------------------------------------
get_hook_table_base_address:
  mov     r12, sp
  stmfd   sp!, {r1, r11-r12, lr, pc}
  
  adr     r1, os_hook_addresses_table
  lsl     r0, r0, #3
  add     r0, r1, r0
  
  ldmfd   sp, {r1, r11, sp, pc}

os_hook_addresses_table:
  @ OS 1.7.2741
  .long 0x100097AC, 0x10008F68
  
  @ OS 2.0.1188
  .long 0x00000000, 0x00000000

  .align

# --------------------------------------------------------------------------
# Prepare a proper OS reboot
#
# Input:
#   r0 = OS Version table index
#
# Output:
#
# --------------------------------------------------------------------------
restore_os_flags:
  mov     r12, sp
  stmfd   sp!, {r1-r3, r11-r12, lr, pc}
  
  mov     r1, #OS_FLAGS_TABLE_BYTES_SIZE
  mul     r0, r1, r0
  adr     r1, restore_os_flags_table
  add     r1, r1, r0
  
  mov     r0, #OS_FLAGS_COUNT
_restore_os_flags_loop:
  ldr     r2, [r1]
  cmp     r2, #0x00000000
  beq     _restore_os_flags_exit
  
  ldr     r3, [r1, #4]
  str     r3, [r2]
  add     r1, r1, #8
  sub     r0, r0, #1
  cmp     r0, #0
  bne     _restore_os_flags_loop
  
_restore_os_flags_exit:  
  ldmfd   sp, {r1-r3, r11, sp, pc}

restore_os_flags_table:
  @ OS 1.7.2741
  # Flags
  .long 0x106DAFC4, 0x00000001
  .long 0x106F2A0C, 0x00000001
  .long 0x106F2AF0, 0x00000001
  
  # NOP instructions, avoid to clear memory pool
  .long 0x1000028C, 0x00000000
  .long 0x10000290, 0x00000000
  .long 0x10000294, 0x00000000
  
  @ OS 2.0.1188
  # Flags
  .long 0x00000000, 0x00000000
  .long 0x00000000, 0x00000000
  .long 0x00000000, 0x00000000
  
  # NOP instructions, avoid to clear memory pool
  .long 0x00000000, 0x00000000
  .long 0x00000000, 0x00000000
  .long 0x00000000, 0x00000000

  .align

syscalls_table:
  #include "../../include/syscalls/syscalls_table.S"
  .align
  
  .end
