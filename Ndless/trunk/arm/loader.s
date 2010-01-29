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
 * - Fork the OS address used to select the user language.
 */
     
  #include "headers/os.h"
  #include "headers/defines.h"
  #include "bootstrapper.h"
  
  #ifdef CAS
    .set  OS_OFFSET_HACKED_EXECUTE,             0x10008F58
    .set  OS_SHADOWED_CALL_EXECUTE,             0x1004C7B4
    .set  OS_OFFSET_HACKED_RESTORE,             0x10052084
  #elif NON_CAS
    .set  OS_OFFSET_HACKED_EXECUTE,             0x10008F88
    .set  OS_SHADOWED_CALL_EXECUTE,             0x1004C800
    .set  OS_OFFSET_HACKED_RESTORE,             0x100520D0
  #endif
  
  .set  HACK_BASE_ADDRESS,                    0x1800E15C
  .set  HACK_BYTES_SIZE,                      5120
    
  .text
  
_start: .global _start
# -------------------------------------------------------------------------
# If the file loader.tns exists in ndless folder, install hack.
# Else fork OS addresses (execute_hack).
# -------------------------------------------------------------------------
  ldr     sp, =HACK_BASE_ADDRESS
  
  # Desactivate interrupts
  mov     r0, #0xFFFFFFFF
  oscall  TCT_Local_Control_Interrupts
  
  # Is it the first installation of the hack in OS memory space ?
  adr     r0, fileLoaderInstalled
  adr     r1, rb
  oscall  fopen
  cmp     r0, #0
  bne     install_hack

# -------------------------------------------------------------------------
# Fork OS addresses.
# 1) If the key theta is hold down, the resource file 'strings.res' is
# restored and the hack is removed.
# 2) Allocate a memory block.
# 3) Copy the code of the loader to the memory block.
# 4) Inject the address of memory block in the memory space of OS.
# 5) Inject the address of restore_resource in the memory space of OS.
# 6) Reboot OS.
# -------------------------------------------------------------------------
fork_os:  
  # Remove hack if key theta is pressed
_is_theta_pressed:
  is_key_pressed  0x10, #KEY_NSPIRE_THETA
  beq     remove_hack
  
  # Allocate memory
  mov     r5, #HACK_BYTES_SIZE
  mov     r0, r5
  oscall  malloc
  mov     r6, r0
  
  # Copy loader code
  ldr     r1, =HACK_BASE_ADDRESS
  mov     r2, r5
  oscall  memcpy
  
  # Inject buffer address
  add     r2, r6, #execute_hack       @ call execute_hack
  fork_address  OS_OFFSET_HACKED_EXECUTE
  
  # Inject restore_resource address
  add     r2, r6, #restore_resource   @ call restore_resource
  fork_address  OS_OFFSET_HACKED_RESTORE
  ldr     pc, =OS_BASE_ADDRESS        @ reboot OS

# -------------------------------------------------------------------------
# Prepare a proper OS execution during the next reboot.
# Force to read strbackup.tns instead of strings.res.
# -------------------------------------------------------------------------
restore_resource:
  cmp     r7, #2               
  bne     _restore_resource_continue       @ resource not located into syst folder
  adr     r0, fileResourceStringsBackup    @ use the resource saved instead of standard resource (hacked) 
 
_restore_resource_continue:
  adr     r1, rb
  ldr     pc, =(OS_OFFSET_HACKED_RESTORE + 8)

# -------------------------------------------------------------------------
# Install hack in the file system.
# 1) Save loader into the phoenix folder.
# 2) Erase strings.res by loader.tns (prevent an update of loader)
# 3) Move strbackup.tns into the phoenix folder.
# 4) Reboot OS. Force to execute the new loader.
# -------------------------------------------------------------------------
install_hack:
  # Close file
  oscall  fclose
  
  # Copy loader.tns (usefull to reinstall hack if the user changes language)
  adr     r0, fileLoaderInstalled
  adr     r1, fileLoaderBackup
  bl      copyFile
  
  # Erase strings.res (it's probably a new loader)
  adr     r0, fileResourceStrings
  oscall  unlink
  adr     r0, fileLoaderInstalled
  adr     r1, fileResourceStrings
  oscall  rename
  
  # Move strbackup.tns
  # TODO : Check if phoenix/strbackup.tns or ndless/strbackup.tns exists
  adr     r0, fileResourceStringsBackup
  oscall  unlink
  adr     r0, fileResourceStringsBackupInstalled
  adr     r1, fileResourceStringsBackup
  oscall  rename
  ldr     pc, =OS_BASE_ADDRESS        @ reboot OS. Force to execute the new loader
  
# -------------------------------------------------------------------------
# Remove hack from the file system.
# 1) Remove loader saved into the phoenix folder.
# 2) Replace strings.res by strbackup.tns stored into the phoenix folder.
# 3) Reboot calculator. Force to install a fresh OS in RAM.
# -------------------------------------------------------------------------
remove_hack:
  # Remove loader
  adr     r0, fileLoaderBackup
  oscall  unlink

  # Replace strings.res by strbackup.tns
  adr     r0, fileResourceStrings
  oscall  unlink
  adr     r0, fileResourceStringsBackup
  adr     r1, fileResourceStrings
  oscall  rename
  bl      rebootCalculator            @ Remove all forked address

# ------------------------------------------------------------------------------
# Execute hook.tns (hook installation)
# 1) If the key PI is hold down, don't execute the hook installation.
# 2) Else execute the hook installation.
# 3) Restore OS instructions for a valid execution.
# ------------------------------------------------------------------------------
execute_hack:
  # Save OS state
  stmfd   sp!, {r0-r12, lr}

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
  oscall  OS_SHADOWED_CALL_EXECUTE      @ execute OS instructions previously erased by fork_os
  cmp     r0, #0
  ldr     pc, =(OS_OFFSET_HACKED_EXECUTE + 8)

# ??? The order must be preserved, else you will get the following error message
# 'Error: invalid constant (xxx) after fixup' 
fileResourceStrings:                 .string "/phoenix/syst/locales/en/strings.res"
fileLoaderBackup:                    .string "/phoenix/loader.tns"
fileResourceStringsBackup:           .string "/phoenix/strbackup.tns"
fileLoaderInstalled:                 .string "/documents/ndless/loader.tns"
fileResourceStringsBackupInstalled:  .string "/documents/ndless/strbackup.tns"
fileHook:                            .string "/documents/ndless/hook.tns"

# TODO : Better messages ^^
#dialogTitle:
#  .string "Ndless version bêta 2"

#welcomeMessage:
#  .ascii "Welcome to Ndless hack installation.\n\n" \
#         "Ndless allows to run arbitrary code on your Nspire calculator.\n" \
#         "During initialization of Operating System you can :\n" \
#         "- Hold down THETA key to remove Ndless.\n" \
#         "- Hold down PI key to avoid running hook installation.\0"
#  .align
  
#removeMessage:
#  .ascii "Ndless was removed.\n"
#  .ascii "You can now delete the folder 'Ndless'.\0"
#  .align
  
  .end
