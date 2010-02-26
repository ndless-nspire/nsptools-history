/*****************************************************************************
 * @(#) Ndless hook (i.e run user programs thanks to the TI documents browser)
 *
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Ndless code.
 *
 * The Initial Developer of the Original Code is Geoffrey ANNEHEIM
 * <geoffrey.anneheim@gmail.com>.
 * Portions created by the Initial Developer are Copyright (C) 2010
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 *                 Olivier ARMAND <olivier.calc@gmail.com>
 ****************************************************************************/

  #include "headers/os.h"
  #include "bootstrapper.h"
  
  #ifdef CAS
    .set  OS_OFFSET_HACKED,             0x10005E40
    .set  OS_SHADOWED_CALL,             0x10005758
  #elif NON_CAS
    .set  OS_OFFSET_HACKED,             0x10005E58
    .set  OS_SHADOWED_CALL,             0x10005770
  #endif
  
  .set  PRG_SIGNATURE_BYTES_SIZE,       4
    
  .text

_start: .global _start
  mov     r0, pc
  b       install_hook
  b       open_document
  
install_hook:
  mov     r12, sp
  stmfd   sp!, {r1-r2, r11-r12, lr, pc}
  
  # Inject hook address
  mov     r2, r0              @ hook address, jump to 'open_document'
  fork_address  OS_OFFSET_HACKED
  
  ldmfd   sp, {r1-r2, r11, sp, pc}
  
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
  cmp     r0, #0              @ execute the OS instruction previously erased by install_hook
  ldr     pc, =(OS_OFFSET_HACKED + 8)

_ti_open_file:
  # Restore OS state
  ldmfd   sp!, {r0-r12, lr}
  
  # Restore OS instructions
  oscall  OS_SHADOWED_CALL    @ call the OS function that open a TI document
  cmp     r0, #0              @ execute the OS instruction previously erased by install_hook
  ldr     pc, =(OS_OFFSET_HACKED + 8)

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
  oscall  sprintf
  
  # Open file
  mov     r0, r9
  bl      openFileBufferized
  cmp     r0, #0              @ cannot read file content (out of memory)
  beq     _exec_program_exit_error
  
  # Check signature
  mov     r10, r0              @ file buffer
  adr     r1, prgSignature
  mov     r2, #PRG_SIGNATURE_BYTES_SIZE
  oscall  memcmp
  cmp     r0, #0              @ isn't a program (i.e is a TI document)
  bne     _exec_program_exit_error
  
  # Desactivate interrupts
  mov     r0, #0xFFFFFFFF
  oscall  TCT_Local_Control_Interrupts
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
  oscall  TCT_Local_Control_Interrupts_
  
  # Unallocate memory
  mov     r0, r10
  oscall  free
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
  .end
