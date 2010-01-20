/****************************************************************************
 * @(#) Ndless hack loader
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
 
/**
 * TODO:
 * -----
 *
 * - Avoid to replace 'strings.res' by 'strbackup.tns'. Fork the OS address used
 * to load resources (CAS: S_OFFSET_HACKED_RESTORE,0x10052084)
 * restore_resource:
 *   cmp     r7, #2               
 *   bne     _restore_resource_continue    @ resource not located into syst folder
 *   adr     r0, fileResourceStringsBackup @ use the resource saved instead of standard resource (hacked) 
 *
 * _restore_resource_continue:
 *   adr     r1, rb
 *   ldr     pc, =(OS_OFFSET_HACKED_RESTORE + 8)
 *
 * - Move files 'loader.tns' and 'strbackup.tns' into the folder /phoenix.
 * - Fork the OS address used to select the user language.
 */
     
  #include "headers/os.h"
  #include "headers/defines.h"
  
  #ifdef CAS
    .set  OS_OFFSET_HACKED,             0x10008F58
    .set  OS_SHADOWED_CALL,             0x1004C7B4
  #elif NON_CAS
    .set  OS_OFFSET_HACKED,             0x10008F88
    .set  OS_SHADOWED_CALL,             0x1004C800
  #endif
  
  .set  HACK_BASE_ADDRESS,              0x1800E15C
  .set  HACK_BYTES_SIZE,                5120
    
  .text

_start: .global _start
  b       install_hack
  b       execute_hack

# -------------------------------------------------------------------------
# Install hack in OS memory space.
# If the key theta is hold on, the resource file 'strings.res' is restored.
# (i.e Remove the hack) 
# 1) Allocate a block memory.
# 2) Copy the code of the loader in the block memory.
# 3) Inject the block memory address in the OS space.
# 4) Restore resource 'strings.res' to perform a valid OS reboot.
# 5) Restart OS.
# -------------------------------------------------------------------------
install_hack:
  ldr     sp, =HACK_BASE_ADDRESS
  
  # Desactivate interrupts
  mov     r0, #0xFFFFFFFF
  oscall  TCT_Local_Control_Interrupts
  
  # Remove hack if key theta is pressed
_is_theta_pressed:
  is_key_pressed  0x10, #KEY_NSPIRE_THETA
  beq     _restore_resource
  
  # Allocate memory
  mov     r5, #HACK_BYTES_SIZE
  mov     r0, r5
  oscall  malloc
  
  # Copy loader code
  ldr     r1, =HACK_BASE_ADDRESS
  mov     r2, r5
  oscall  memcpy
  
  # Inject buffer address
  add     r2, r0,  #4        @ loader address, jump to execute_hack
  ldr     r0, =OS_OFFSET_HACKED
  ldr     r1, =0xE51FF004   @ ldr pc, [pc, #-4]
  str     r1, [r0]          @ write instruction
  str     r2, [r0, #4]      @ write buffer address

  # Prepare a valid OS reboot
_restore_resource:
  adr     r0, fileResourceStringsENBackup
  adr     r1, fileResourceStringsEN
  bl      copyFile
  
  # Reboot OS
_restart_os:
  ldr     pc, =OS_BASE_ADDRESS

# -----------------------------------------------------------------------------
# 1) Reinstall the loader in the resource file 'strings' for the next reboot.
# 2) If the key PI is hold on, don't execute the hook installation.
# 3) Else execute the hook installation.
# 4) Restore OS instructions for a valid execution.
# ------------------------------------------------------------------------------
execute_hack:
  # Save OS state
  stmfd   sp!, {r0-r12, lr}
  
  # Reinstall loader for the next reboot
  adr     r0, fileLoader
  adr     r1, fileResourceStringsEN
  bl      copyFile

  # Don't execute the hook if the key PI is pressed
_is_pi_pressed:  
  is_key_pressed  0x12, #KEY_NSPIRE_PI
  beq     _restore_os_state
  
  # Execute the hook installation
_execute_hook:  
  # Open file
  adr     r0, fileHook
  bl      openFileBufferized
  cmp     r0, #0
  beq     _restore_os_state
  
  # Run code
  mov     lr, pc
  mov     pc, r0

_restore_os_state:
  # Restore OS state
  ldmfd   sp!, {r0-r12, lr}
  
  # Restore OS instructions
  oscall  OS_SHADOWED_CALL      @ execute the OS instruction previously erased by install_hack
  cmp     r0, #0
  ldr     pc, =(OS_OFFSET_HACKED + 8)

fileResourceStringsEN:          .string "/phoenix/syst/locales/en/strings.res"
fileResourceStringsENBackup:    .string "/documents/ndless-installation/strbackup.tns"
fileLoader:                     .string "/documents/ndless-installation/loader.tns"
fileHook:                       .string "/documents/ndless-installation/hook.tns"

  .end
