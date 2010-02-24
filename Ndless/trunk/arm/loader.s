/****************************************************************************
 * @(#) Ndless hack loader
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
@ -------------------------------------------------------------------------
@ The loader is either installed in /phoenix/syst/locales/en/strings.res
@ (by Ndless Installer, first execution), or in
@ /phoenix/ndls/locales/<language>/strings.res (next executions) and run by
@ a buffer overflow.
@ At the first execution, the loader finishes its installation: syst's
@ strings.res is restored, a new component 'ndls' is registered and the
@ loader is installed in ndls's strings.res, to make the hack persist in
@ all languages.
@ The loader may update itself and/or the hooks from files in the installer
@ directory.
@ The loader then load the OS hooks (hook.s) and continue the OS boot
@ up.
@ -------------------------------------------------------------------------
  ldr     sp, =HACK_BASE_ADDRESS
  
  @ Check if Ndless is installed
_check_is_installed:
  adr     r0, pathNdls
  oscall  set_current_path
  cmp     r0, #0
  bne     install_hack

  @ Check if Ndless should be updated
 _check_update_hook:
	@ Move possible new hook.tns from installer directory to system directory
	@ Installing a new hook doesn't require an OS reboot
	adr     r0, fileHookInstaller
	mov     r4, r0
	bl      fileExists
	cmp     r0, #0
	bne    _check_update_loader
	
  adr     r0, fileHookInstalled
  mov     r5, r0
  oscall  unlink
  mov     r0, r4                      @ installer
  mov     r1, r5                      @ installed
  oscall  rename
 
 @ Check if the loader should be updated, else fork the OS
_check_update_loader:
  adr     r0, fileLoaderInstaller
  bl      fileExists
  cmp     r0, #0
  bne     fork_os
  
  @ Overwrite strings.res by content of loader.tns for all language
  ldr     r0, =copy_resource_file
  adr     r1, folderNdls
  adr     r2, fileLoaderInstaller
  stmfd   sp!, {r1-r2}
  bl      iterate_locale_names_callback
  add     sp, sp, #8
  
  @ Remove loader.tns located in user documents
  adr     r0, fileLoaderInstaller
  oscall  unlink
 
  ldr     pc, =OS_BASE_ADDRESS        @ reboot OS (execute the new loader)
 
fileHookInstalled:                    .string "/phoenix/ndls/hook.tns"
  .align

@ -------------------------------------------------------------------------
@ Prepare a proper OS execution during the next reboot.
@ Ignore reading of strings.res of ndls component
@ -------------------------------------------------------------------------
restore_resource:
  cmp     r7, #1               
  bne     _restore_resource_continue       @ resource not located into ndls folder
  sub     sp, r11, #0x28                   @ exit load resource OS call
  ldmfd   sp, {r4-r11, sp, pc}
 
_restore_resource_continue:
  mov     r0, r6
  adr     r1, openFileMode_rb
  ldr     pc, =(OS_OFFSET_HACKED_RESTORE + 8)
  
openFileMode_rb:                     .string "rb"
  .align

@ -------------------------------------------------------------------------
@ Remove hack from the file system.
@ 1) Restore the components file
@ 2) Delete Ndless installation directory
@ 3) Fully reboot the calculator, to install a fresh OS in RAM.
@ -------------------------------------------------------------------------
remove_hack:
  @ Rewrite /phoenix/components
  mov     r0, #0        @ exclude ndls folder
  bl      rewrite_components
  
  @ Remove ndls folder
  adr     r0, pathNdls
  oscall  purge_directory_
  adr     r0, pathNdls
  oscall  rmdir

  bl      rebootCalculator            @ Remove all forked address

pathNdls:                            .string "/phoenix/ndls"
fileLoaderInstaller:                 .string "/documents/ndless/loader.tns"
fileHookInstaller:                   .string "/documents/ndless/hook.tns"
  .align
    
@ -------------------------------------------------------------------------
@ Install hack in the file system.
@ 1) Create the ndls component tree
@ 2) Register the ndls component in the file 'components'
@ 3) Copy the hack in all strings.res of all languages of the ndls component
@ 4) Restore syst's strings.res
@ 5) Move hook.tns from user to system directory
@ 6) Fork the OS
@ -------------------------------------------------------------------------
install_hack:
  @ TODO: Check if/documents/ndless/strbackup.tns exists
  @ Create localization paths
  mov     r1, #0
  adr     r0, pathPhoenix
  oscall  set_current_path
  adr     r0, folderNdls
  oscall  mkdir
  adr     r0, folderNdls
  oscall  set_current_path
  adr     r0, pathLocales
  oscall  mkdir
  adr     r0, pathLocales
  oscall  set_current_path
  ldr     r0, =mkdir
  bl      iterate_locale_names_callback
  
  @ Rewrite /phoenix/components
  mov     r0, #1        @ include ndls folder
  bl      rewrite_components
  
  @ Copy strings.res (loader) for all language into ndls folder
  ldr     r0, =copy_resource_file
  adr     r1, folderNdls
  adr     r2, fileResourceStrings
  stmfd   sp!, {r1-r2}
  bl      iterate_locale_names_callback
  add     sp, sp, #8
  
  @ Overwrite strings.res (loader) by strbackup.tns
  adr     r0, fileResourceStrings
  oscall  unlink
  adr     r0, fileResourceStringsBackup
  adr     r1, fileResourceStrings
  oscall  rename
  
  @ Move hook.tns from the installer directory to the system directory
  @ if it exists
  adr     r0, fileHookInstaller
  adr     r1, fileHookInstalled
  oscall  rename
  
  @ Remove loader.tns from user documents (avoid to update loader during the next reboot)
  adr     r0, fileLoaderInstaller
  oscall  unlink
  
  b       fork_os

folderNdls:                          .string "ndls"  
pathPhoenix:                         .string "/phoenix"
pathLocales:                         .string "locales"
fileResourceStringsBackup:           .string "/documents/ndless/strbackup.tns"
fileResourceStrings:                 .string "/phoenix/syst/locales/en/strings.res"
    .align

@ -------------------------------------------------------------------------
@ Fork OS addresses.
@ 1) If the key theta is hold down, the hack is removed.
@ 2) Allocate a memory block.
@ 3) Copy the code of the loader to the memory block.
@ 4) Inject the address of memory block in the memory space of OS.
@ 5) Inject the address of restore_resource in the memory space of OS.
@ 6) Reboot OS.
@ -------------------------------------------------------------------------
fork_os:  
  @ Remove hack if key theta is pressed
_is_theta_pressed:
  isKeyPressed  KEY_NSPIRE_THETA
  beq     remove_hack
  
  @ Allocate memory
  mov     r5, #HACK_BYTES_SIZE
  mov     r0, r5
  oscall  malloc
  mov     r6, r0
  
  @ Copy loader code
  ldr     r1, =HACK_BASE_ADDRESS
  mov     r2, r5
  oscall  memcpy
  
  @ Inject buffer address
  add     r2, r6, #execute_hook       @ call execute_hook
  fork_address  OS_OFFSET_HACKED_EXECUTE
  
  @ Inject restore_resource address
  add     r2, r6, #restore_resource   @ call restore_resource
  fork_address  OS_OFFSET_HACKED_RESTORE
  ldr     pc, =OS_BASE_ADDRESS        @ reboot OS

@ ------------------------------------------------------------------------------
@ Execute hook.tns (hook installation)
@ 1) If the key PI is hold down, don't execute the hook installation.
@ 2) Else execute the hook installation.
@ 3) Restore OS instructions for a valid execution.
@ ------------------------------------------------------------------------------
execute_hook:
  @ Save OS state
  stmfd   sp!, {r0-r12, lr}

  @ Don't execute the hook if the key PI is pressed
_is_pi_pressed:  
  isKeyPressed  KEY_NSPIRE_PI
  beq     _restore_os_state
  
  @ Execute the hook installation
_execute_hook:  
  @ Open file
  adr     r0, fileHookInstalled
  bl      openFileBufferized
  cmp     r0, #0
  beq     _restore_os_state
  
  @ Run code
  mov     lr, pc
  mov     pc, r0

_restore_os_state:
  @ Restore OS state
  ldmfd   sp!, {r0-r12, lr}
  
  @ Restore OS instructions
  oscall  OS_SHADOWED_CALL_EXECUTE      @ execute OS instructions previously overwritten by fork_os
  cmp     r0, #0
  ldr     pc, =(OS_OFFSET_HACKED_EXECUTE + 8)

@ ------------------------------------------------------------------------------
@ Rewrite the file /phoenix/components which contains the list of components
@
@ Input:
@   r0 = 1 include ndls folder, else 0
@
@ Output:
@ ------------------------------------------------------------------------------
rewrite_components:
  mov     r12, sp
  stmfd   sp!, {r1-r4, r11-r12, lr, pc}
  
  mov     r5, r0
  
  @ Open components file
  adr     r0, fileComponents
  adr     r1, openFileMode_wb
  oscall  fopen_
  mov     r4, r0
  
  @ Write components hierarchy
  adr     r0, tblComponents
  mov     r1, #45
  cmp     r5, #0
  beq     _rewrite_components_continue
  add     r1, r1, #5
_rewrite_components_continue:
  mov     r2, #1
  mov     r3, r4
  oscall  fwrite
  
  @ Close components file
  mov     r0, r4
  oscall  fclose
  
  ldmfd   sp, {r1-r4, r11, sp, pc}
  
openFileMode_wb:                     .string "wb"
fileComponents:                      .string "/phoenix/components"
tblComponents:
  .ascii  "clnk\n" \
          "ctlg\n" \
          "dcol\n" \
          "geog\n" \
          "math\n" \
          "ntpd\n" \
          "scpd\n" \
          "syst\n" \
          "tblt\n" \
          "ndls\n"
  .align

@ ------------------------------------------------------------------------------
@ Iterate all locale name
@
@ Input:
@   r0 = function (3 parameters maximum)
@
@ Output:
@ ------------------------------------------------------------------------------
iterate_locale_names_callback:
  mov     r12, sp
  stmfd   sp!, {r1-r6, r11-r12, lr, pc}
  sub     sp, sp, #0x100
  mov     r11, sp
  
  mov     r6, r0
  
  adr     r5, tblLanguages
  mov     r4, #6
_iterate_locale_names_callback_loop:
  mov     r0, r5
  
  stmfd   sp!, {r12}
  ldmfd   r12, {r1-r3}
  mov     lr, pc
  mov     pc, r6
  ldmfd   sp!, {r12}
  
  @ Prepare the next step
  add     r5, r5, #3
  sub     r4, r4, #1
  cmp     r4, #0
  bne     _iterate_locale_names_callback_loop
  
  add     sp, sp, #0x100
  ldmfd   sp, {r1-r6, r11, sp, pc}

tblLanguages:
  .ascii  "en\0" \
          "da\0" \
          "de\0" \
          "fr\0" \
          "it\0" \
          "no\0"
  .align

@ ------------------------------------------------------------------------------
@ Copy a resource file into a localization folder
@
@ Input:
@   r0 = language
@   r1 = component folder name
@   r2 = resource file to copy
@
@ Output:
@ ------------------------------------------------------------------------------  
copy_resource_file:
  mov     r12, sp
  stmfd   sp!, {r3-r4, r11-r12, lr, pc}
  sub     sp, sp, #0x100
  mov     r11, sp

  mov     r4, r2

  @ Create localization path
  mov     r2, r1
  adr     r1, formatResourcePath
  mov     r3, r0
  mov     r0, r11
  oscall  sprintf

  @ Copy resource file specified into localization folder previously defined
  mov     r0, r4
  mov     r1, r11
  bl      copyFile
  
  add     sp, sp, #0x100
  ldmfd   sp, {r3-r4, r11, sp, pc}
  
formatResourcePath:                 .string "/phoenix/%s/locales/%s/strings.res"
  .align

  .end
