  #include <os.h>
  #include "bootstrapper.h"

  #define nn_call_installos_callback        0x102A93C8  // NON_CAS

  .string "PRG"

  .text
_start: .global _start
  mov     r0, pc
  mov     r12, sp
  stmfd   sp!, {r0-r12, lr, pc}
  sub     r6, r0, #12
  
  mov     r0, #0
  oscall  TCT_Local_Control_Interrupts
  
  add     r2, r6, #install_ndless
  fork_address  0x1004F7DC
  
  adr     r0, filePhoenixInstalled
  adr     r1, filePhoenixTemp
  oscall  rename
  
  mov     r0, #0
  mov     r1, #0
  oscall  nn_call_installos_callback

  ldmfd   sp, {r0-r11, sp, r0}
  add     sp, sp, #0x100
  ldmfd   sp, {r1-r3, r9-r11, sp, pc}
  
install_ndless:
  mov     r0, #-1
  oscall  TCT_Local_Control_Interrupts
  
  adr     r0, fileResourceStrings
  oscall  unlink
  adr     r0, fileResourceStringsInstalled
  adr     r1, fileResourceStrings
  oscall  rename
    
  mov     r0, #0
  oscall  TCT_Local_Control_Interrupts
  
  sub     sp, r11, #0x28
  ldmfd   sp, {r4-r11, sp, pc}
  
filePhoenixInstalled:          .string "ndless/phoenix.tns"
filePhoenixTemp:               .string "/tmp/TI-nspire.tnc"
fileResourceStringsInstalled:  .string "/documents/ndless/strings.tns"
fileResourceStrings:           .string "/phoenix/syst/locales/en/strings.res"
  .align
  
.end
