/**
 * TODO : check current version/CAS support/2.1 compatible (auto-search syscalls?)
 * reboot (to maintenance menu? or uninstall/corrupt os?)
 */

#include <os.h>

#define STACK_PTR_BASE_ADDR           0x1800E000
#define OS_BASE_ADDR                  0x10000000
#define OS_END_ADDR                   0x11000000
#define MIN_VER_NAND_START            0x150000
#define MIN_VER_NAND_END              0x15FFFF
#define CPU_RESET_PORT                0x900A0008

#define _TCT_Local_Control_Interrupts 0
#define _nand_erase_range  1
#define _unlink  2

@ portable oscall: scans the OS code with the corresponding signature
@ id: syscall id (position in syscalls_signatures array) 
@ destroys r4
  .macro pt_oscall id
  	mov r4, #\id
    bl _pt_oscall
  .endm

.global main
main:
  @ Interrupt vectors
  .fill 16, 4, 0
  
  ldr     sp, =STACK_PTR_BASE_ADDR
  mov r0, #0xFFFFFFFF
	pt_oscall _TCT_Local_Control_Interrupts
	
	ldr r0, =MIN_VER_NAND_START
	ldr r1, =MIN_VER_NAND_END
	pt_oscall _nand_erase_range @ clear all the min versions
	
	adr r0, os_file
	pt_oscall _unlink @ uninstall the OS (the version is written by boot 2 each time the OS is load)

	ldr r0, =CPU_RESET_PORT
	mov r1, #2
	str r1, [r0]

syscalls_signatures:
@ entry format: signature size, signature
@ TCT_Local_Control_Interrupts
	.word 32
	.byte 0x00, 0x30, 0x0F, 0xE1, 0xC0, 0x10, 0x00, 0xE2, 0xC0, 0x20, 0xC3, 0xE3, 0x01, 0x20, 0x82, 0xE1, 0x02, 0xF0, 0x2F, 0xE1, 0xC0, 0x00, 0x13, 0xE2, 0x00, 0x00, 0xE0, 0x13, 0x00, 0x00, 0xA0, 0x03
@nand_erase_range
	.word 32
	.byte 0x0D, 0xC0, 0xA0, 0xE1, 0x30, 0xD8, 0x2D, 0xE9, 0x04, 0xB0, 0x4C, 0xE2, 0x01, 0x40, 0xA0, 0xE1, 0x10, 0xFE, 0xFF, 0xEB, 0x00, 0x50, 0xA0, 0xE1, 0x04, 0x00, 0xA0, 0xE1, 0x0D, 0xFE, 0xFF, 0xEB
@ unlink
	.word 20
	.byte 0x0D, 0xC0, 0xA0, 0xE1, 0x70, 0xD8, 0x2D, 0xE9, 0x04, 0xB0, 0x4C, 0xE2, 0x4B, 0xDF, 0x4D, 0xE2, 0x00, 0x50, 0xA0, 0xE1

@ supports only word-aligned pointers
@ destroys r0-r4
_memcmp:
	ldr	r3, [r0], #4
	ldr	r4, [r1], #4
	subs	r2, r2, #4
	beq _memcmp_end
	cmp	r3, r4
	beq	_memcmp
_memcmp_end:
	sub	r0, r3, r4
	bx lr

@ r4: syscall index
_pt_oscall:
  mov     r12, sp
  stmfd   sp!, {r12, lr, pc}
  stmfd   sp!, {r0-r3}
	adr r1, syscalls_signatures
	mov r2, #0
	mov r0, r4
get_sig_loop:
	cmp r0, r2
	beq sig_found
	ldr r3, [r1]
	add r1, r1, r3
	add r1, r1, #4
	add r2, r2, #1
	b get_sig_loop
sig_found:
	ldr r7, [r1], #4 @ signature size
	mov r5, r1 @ signature
	ldr r6, =OS_BASE_ADDR-4
scan_sig_loop:
	add r6, r6, #4
	cmp r6, #OS_END_ADDR
	bcs break @ too far
	mov r0, r5
	mov r1, r6
	mov r2, r7
	bl _memcmp
	cmp r0, #0
	bne scan_sig_loop
	ldmfd   sp!, {r0-r3}
	mov lr, pc
	mov pc, r6
  ldmfd   sp, {sp, pc}

break:
	b break

os_file: .asciz "/phoenix/install/TI-Nspire.tnc"
