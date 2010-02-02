  .xdef rebootCalculator
  .xdef showSimpleDialogBox

# --------------------------------------------------------------------------
# Reboot calculator
#
# Input:
# Output:
# --------------------------------------------------------------------------
rebootCalculator:
  mov     r12, sp
  stmfd   sp!, {r0-r1, r11-r12, lr, pc}
  ldr     r0, =0xA4012C00
  add     r0, r0, #0x2B0
  mov     r1, #2
  strh    r1, [r0, #0x34]
  oscall  power_off
  ldmfd   sp, {r0-r1, r11, sp, pc}

# --------------------------------------------------------------------------
# Show a dialog box with only a OK button
#
# Input:
#   r0 = Title
#   r1 = Message
#
# Output:
# --------------------------------------------------------------------------
showSimpleDialogBox:
  mov     r12, sp
  stmfd   sp!, {r2, r6-r8, r11-r12, lr, pc}
  
  mov     r7, r0
  mov     r8, r1
  
  mov     r0, #0x400
  oscall  malloc
  mov     r6, r0
  
  mov     r1, r7
  mov     r2, #0x200
  oscall  ascii2utf16
  
  add     r0, r6, #0x200
  mov     r1, r8
  mov     r2, #0x200
  oscall  ascii2utf16
  
  mov     r0, #0
  mov     r1, r6
  add     r2, r6, #0x200
  oscall  show_dialog_box2
  
  mov     r0, r6
  oscall  free
  
  ldmfd   sp, {r2, r6-r8, r11, sp, pc}
