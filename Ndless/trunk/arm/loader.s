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
# Move the strings resource backup if necessary.
# If the file loader.tns exists in ndless folder, install this new loader.
# Else fork OS addresses (execute_hook).
# -------------------------------------------------------------------------
  ldr     sp, =HACK_BASE_ADDRESS
  
  # Desactivate interrupts
  mov     r0, #0xFFFFFFFF
  oscall  TCT_Local_Control_Interrupts
  
  # Move strbackup.tns from Ndless Installer directory to system directory if
  # it exists (first execution)
  adr     r4, fileResourceStringsBackupInstaller
  mov     r0, r4
  bl      fileExists
  cmp     r0, #0
  bne     _res_strings_backed_up
  adr     r5, fileResourceStringsBackup
  mov     r0, r5
  oscall  unlink
  adr     r0, dirNdlessSys
  oscall  mkdir
  mov     r0, r4               @ fileResourceStringsBackupInstaller
  mov     r1, r5               @ fileResourceStringsBackup
  oscall  rename
 
_res_strings_backed_up:
  adr     r0, fileLoaderInstaller
  bl      fileExists
  cmp     r0, #0
  beq     update_loader

# -------------------------------------------------------------------------
# Fork OS addresses, to re-run the loader.
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
  add     r2, r6, #execute_hook       @ call execute_hook
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
  bl      get_file_res_strings_backup      @ use the resource saved instead of standard resource (hacked) 
_restore_resource_continue:
  adr     r1, rb
  ldr     pc, =(OS_OFFSET_HACKED_RESTORE + 8)

# -------------------------------------------------------------------------
# Strings: must not be too far from the pc-relative references to avoid the
# assembler error 'invalid constant (xxx) after fixup'
# -------------------------------------------------------------------------
fileResourceStringsBackupInstaller:  .string "/documents/ndless/strbackup.tns"
fileResourceStringsBackup:           .string "/phoenix/ndless/strbackup.tns"
dirNdlessSys:                        .string "/phoenix/ndless"
fileResourceStrings:                 .string "/phoenix/syst/locales/en/strings.res"
filedefaultResourceStringsBackup:    .string "/phoenix/syst/locales/fr/strings.res"
fileLoaderInstaller:                 .string "/documents/ndless/loader.tns"
fileHook:                            .string "/documents/ndless/hook.tns"

# -------------------------------------------------------------------------
# Update the loader with a version in Ndless installer directory.
# 1) Erase strings.res by loader.tns (prevent an update of loader)
# 2) Reboot OS. Force to execute the new loader.
# -------------------------------------------------------------------------
update_loader:
  # Erase strings.res
  adr     r0, fileResourceStrings
  oscall  unlink
  
  # install new loader
  adr     r0, fileLoaderInstaller
  adr     r1, fileResourceStrings
  oscall  rename
  
  ldr     pc, =OS_BASE_ADDRESS        @ reboot OS. Force to execute the new loader
 
# ------------------------------------------------------------------------------
# Fall back to another language if the backup cannot be found.
# ------------------------------------------------------------------------------
get_file_res_strings_backup:
  stmfd   sp!, {r4-r12, lr}
  adr     r4, fileResourceStringsBackup
  mov     r0, r4
	bl      fileExists
	cmp     r0, #0
	beq     file_res_strings_backup_found
	adr     r4, filedefaultResourceStringsBackup
file_res_strings_backup_found:
	mov     r0, r4
	ldmfd   sp!, {r4-r12, pc}
 
# -------------------------------------------------------------------------
# Remove hack from the file system.
# 1) Remove loader saved into the phoenix folder.
# 2) Replace strings.res by strbackup.tns stored into the phoenix folder.
# 3) Reboot calculator. Force to install a fresh OS in RAM.
# -------------------------------------------------------------------------
remove_hack:
  # Replace strings.res by its backup
  adr     r0, fileResourceStrings
  oscall  unlink
  bl      get_file_res_strings_backup
  adr     r1, fileResourceStrings
  oscall  rename
  adr     r0,dirNdlessSys
  oscall  NU_Remove_Dir
  bl      rebootCalculator            @ Remove all forked address

# ------------------------------------------------------------------------------
# Execute hook.tns (hook installation)
# 1) If the key PI is hold down, don't execute the hook installation.
# 2) Else execute the hook installation.
# 3) Restore OS instructions for a valid execution.
# ------------------------------------------------------------------------------
execute_hook:
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

# TODO : Better messages ^^
#dialogTitle:
#  .string "Ndless version beta 2"

#welcomeMessage:
#  .ascii "Welcome to Ndless hack installation.\n\n" \
#         "Ndless allows to run arbitrary code on your Nspire calculator.\n" \
#         "During the initialization of the Operating System you can:\n" \
#         "- Hold down THETA key to remove Ndless.\n" \
#         "- Hold down PI key to avoid running the hook installation.\0"
#  .align
  
#removeMessage:
#  .ascii "Ndless was removed.\n"
#  .ascii "You can now delete the folder 'Ndless'.\0"
#  .align
  
  .end
