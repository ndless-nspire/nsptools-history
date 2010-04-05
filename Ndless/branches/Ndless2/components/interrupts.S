  .xdef install_vector_swi
  .xdef vector_swi

# -------------------------------------------------------------------------
# Install a syscalls handler
#
# Input:
#   r0 = OS Version Index
#   r1 = Syscalls table bytes size
#   r2 = Syscalls table begin address
#
# Output:
#
# Destroy:
# -------------------------------------------------------------------------
install_vector_swi:  
  mul     r1, r0, r1
  add     r1, r2, r1
  
  adr     r0, syscalls_table_pointer
  str     r1, [r0]
  
  adr     r1, vector_swi
  ldr     r0, =0xA4000028
  str     r1, [r0]
  
  ldr     r1, =0xE59FF018
  ldr     r0, =0xA4000008
  str     r1, [r0]
  bx      lr

# -------------------------------------------------------------------------
# INT_Software (syscalls handler)
# -------------------------------------------------------------------------  
syscalls_table_pointer:
  .long 0

# TODO: Find a better code  
vector_swi:
  stmfd   sp!, {r0-r2, r12, lr}
  mrs     r2, cpsr
  bic     r0, r2, #0x1F
  orr     r0, r0, #0x13
  msr     cpsr_cf, r0
  ldr     r1, syscalls_table_pointer
  ldr     r0, [lr, #-4]
  bic     r0, r0, #0xFF000000
  ldr     r0, [r1, r0, lsl #2]
  str     r0, [sp, #12]
  msr     cpsr_cf, r2
  mov     lr, pc
  ldmfd   sp!, {r0-r2, pc}
  ldmfd   sp!, {pc}^
  
  .align
  