  #include "../include/syscalls/syscalls.h"

  .xdef get_os_version_index
  .xdef get_os_syscalls_index
  #.xdef show_dialog_box2_ascii

  #define OS_VERSION_COUNT      5

# --------------------------------------------------------------------------
# Get OS version index
# Check INT_Initialize address
#
# Input:
#
# Output:
#   -1  Unknown
#   0   1.1.9170
#   1   1.1.9253
#   2   1.7.2741
#   3   2.0.1188
# --------------------------------------------------------------------------
get_os_version_index:
  mov     r12, sp
  stmfd   sp!, {r1-r4, r11-r12, lr, pc}
  ldr     r1, =0x10000020
  ldr     r1, [r1]
  adr     r3, _os_version_table_begin
  adr     r4, _os_version_table_end

  mov     r0, #0
_get_os_version_index_loop:
  ldr     r2, [r3]
  cmp     r1, r2
  beq     _get_os_version_index_exit
  add     r0, #1
  add     r3, #4
  cmp     r3, r4
  bne     _get_os_version_index_loop
  
  mov     r0, #-1
  
_get_os_version_index_exit:  
  ldmfd   sp, {r1-r4, r11, sp, pc}
  
_os_version_table_begin:
  .long 0x1014A9F0  @ 1.1.9170
  .long 0x1014A9C0  @ 1.1.9253
  .long 0x10211290  @ 1.7.2741
  .long 0x1024FF00  @ 2.0.1188
  .long 0x10266030  @ 2.0.1.60
_os_version_table_end:
  .align

# --------------------------------------------------------------------------
# Get syscalls table index from a OS version index
#
# Input:
#   r0 = OS version index
#
# Output:
#   Syscalls table index or -1
# --------------------------------------------------------------------------  
get_os_syscalls_index:
  mov     r12, sp
  stmfd   sp!, {r1, r11-r12, lr, pc}
 
  cmp     r0, #OS_VERSION_COUNT
  bge     _get_os_syscalls_index_error
  
  adr     r1, _get_os_syscalls_index_table_begin
  ldrb    r0, [r1, r0]
  cmp     r0, #0xFF
  beq     _get_os_syscalls_index_error
  
_get_os_syscalls_index_exit: 
  ldmfd   sp, {r1, r11, sp, pc}

_get_os_syscalls_index_error:
  mov     r0, #-1
  b       _get_os_syscalls_index_exit

_get_os_syscalls_index_table_begin:
  .byte 0xFF  @ 1.1.9170
  .byte 0xFF  @ 1.1.9253
  .byte 0x00  @ 1.7.2741
  .byte 0x01  @ 2.0.1188
  .byte 0x02  @ 2.0.1.60
_get_os_syscalls_index_table_end:
  .align

/*show_dialog_box2_ascii:
  mov     r12, sp
  stmfd   sp!, {r9-r12, lr, pc}
  
  sub     sp, sp, #0x500
  mov     r11, sp
  add     r10, r11, #0x100
  
  mov     r9, r1
  
  mov     r1, r0
  mov     r0, r11
  mov     r2, #0x100
  swi     ascii2utf16
  
  mov     r0, r10
  mov     r1, r9
  mov     r2, #0x400
  swi     ascii2utf16
  
  mov     r0, #0
  mov     r1, r11
  mov     r2, r10
  swi     show_dialog_box2
  
  add     sp, sp, #0x500
  ldmfd   sp, {r9-r11, sp, pc}*/
  