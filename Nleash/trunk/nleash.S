/**
 * TODO : check current version/CAS support/2.1 compatible (auto-search syscalls?)
 * reboot (to maintenance menu? or uninstall/corrupt os?)
 */

#include <os.h>

#define STACK_PTR_BASE_ADDR           0x1800E000
#define OS_END_ADDR                   0x11000000
#define MIN_VER_NAND_START            0x150000
#define MIN_VER_NAND_END              0x15FFFF
#define MIN_OS_VER_MARKER             0x928CC6AA
#define NEW_MIN_OS_VERSION            0x01010000
#define CPU_RESET_PORT                0x900A0008
#define NAND_PAGE_SIZE                512

#define SYSCALLS_NUM 5
#define SYSCALL_VERSION_NUM 3
#define _nand_erase_range  0
#define _write_min_os_version_to_nand 1
#define _unlink  2
#define _memset 3
#define _TCT_Local_Control_Interrupts 4

@ the syscall table ptr must be kept in r10
	.macro syscall syscindex
	mov lr, pc
	ldr pc, [r10, #\syscindex*4]
	.endm

@ caution, do not write to the screen, this will overwrite the following code
.global main
main:
  @ Interrupt vectors
  .fill 16, 4, 0
  @ when testing with Ndless
  @.string "PRG"
  
  ldr sp, =STACK_PTR_BASE_ADDR
  
  @ determine the syscall table ptr
  ldr r0, =0x10000020
	ldr r0, [r0]
	adr r10, syscall_table
	mov r1, #SYSCALL_VERSION_NUM
syscall_ptr_loop:
	ldr r2, [r10]
	cmp r2, r0
	beq syscall_ptr_found
	add r10, #(SYSCALLS_NUM+1)*4 @ +1: version
  subs r1, #1
  bne syscall_ptr_loop
  b reboot @ unsupported version
syscall_ptr_found:
	add r10, #4 @ jump the version

  mov r0, #0xFFFFFFFF
	syscall _TCT_Local_Control_Interrupts
	
	ldr r0, =MIN_VER_NAND_START
	ldr r1, =MIN_VER_NAND_END
	syscall _nand_erase_range @ clear all the min versions
	
	sub sp, sp, #NAND_PAGE_SIZE
	mov r0, sp
	mov r1, #-1
	ldr r2, =NAND_PAGE_SIZE
	syscall _memset
	
	mov r0, sp
	ldr r1, =MIN_OS_VER_MARKER
	str r1, [r0],  #4
	ldr r1, =NEW_MIN_OS_VERSION
	str r1, [r0], #4
	mov r1, #0
	str r1, [r0], #4 @ bytes controlling the press-to-test mode are somewhere there
	str r1, [r0], #4 @ bytes controlling the press-to-test mode are somewhere there
	str r1, [r0] @ disable diags loading at boot time
	mov r0, sp
	@oscall 0x1004C5E0
	syscall _write_min_os_version_to_nand
	add sp, sp, #NAND_PAGE_SIZE
	
	adr r0, os_file
	syscall _unlink @ uninstall the OS (the version is written by boot 2 each time the OS is load)

reboot:
	ldr r0, =CPU_RESET_PORT
	mov r1, #2
	str r1, [r0]

@ see nspire_emu's usblink.c for the version list
syscall_table: @format: version, syscall list, version, ...
	.long 0x10211290 @ 1.7
	.long 0x10226150
	.long 0x1007707C
	.long 0x102AA5A8
	.long 0x102A62D4
	.long 0x10211C08
	.long 0x1027A640 @ 2.1 CAS
	.long 0x1028A908
	.long 0x1009331C
	.long 0x102FE68C
	.long 0x102F94FC
	.long 0x1027AFC8
	.long 0x10279D70 @ 2.1
	.long 0x1028A038
	.long 0x10092E6C
	.long 0x102FDDBC
	.long 0x102F8C2C
	.long 0x1027A6F8

os_file: .asciz "/phoenix/install/TI-Nspire.tnc"
