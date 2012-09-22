/*
 * This file is wrapped as host-side object file pointed to by the symbol binary_snippets_bin_start
 * and ending at binary_armsnippets_bin_end.
 * TODO: Currently most file_* snippets are broken, because an error for any file read/write operation is
 * returned by the OS,probably because the armloader interrupts the OS in a state not ready for these
 * syscalls.
 */

// From Ndless's include/syscalls.h
#define __SYSCALLS_ISEMU 0x400000
#define NDLSEMU_DEBUG_ALLOC (__SYSCALLS_ISEMU | 0)
#define NDLSEMU_DEBUG_FREE (__SYSCALLS_ISEMU | 1)

start:

entry_points_list:    @ defined by SNIPPETS_EP_*
	.word syscalls_tables_list - entry_points_list
	.word load_snippet - entry_points_list

syscalls_tables_list:
	.word 4 @ size
	@ format: OS_VERSION_*, offset from start
	.word 0x102132A0, syscalls_1_7_cas - start
	.word 0x10211290, syscalls_1_7_non_cas - start
	.word 0, syscalls_2_0_1_cas - start
	.word 0x10266030, syscalls_2_0_1_non_cas - start

	.equ TCC_Current_HISR_Pointer_patch, 0
	.equ fopen, 1
	.equ fread, 2
	.equ fwrite, 3
	.equ fclose, 4
	.equ unlink, 5
	.equ free, 6
	.equ stat, 7
	.equ malloc, 8

syscalls_1_7_cas:
@ TODO

syscalls_1_7_non_cas:
	.word 0x102A0788 @ TCC_Current_HISR_Pointer_patch: cmp r0, #4
	.word 0x102A4CBC @ fopen
	.word 0x102A4EB0 @ fread
	.word 0x102A5264 @ fwrite
	.word 0x102A46B4 @ fclose
	.word 0x102AA5A8 @ unlink
	.word 0x102A11BC @ free
	.word 0x102AA2CC @ stat
	.word 0x102A11C8 @ malloc

syscalls_2_0_1_non_cas:
@ TODO

syscalls_2_0_1_cas:
@ TODO

	.macro syscall id
	mov lr, pc
	ldr pc, [r11, #\id*4]
	.endm

snippets_list:      @ defined by SNIPPET_*
	.word file_export - snippets_list
	.word free_block - snippets_list
	.word file_open - snippets_list
	.word file_read - snippets_list
	.word file_write - snippets_list
	.word file_close - snippets_list
	.word file_unlink - snippets_list
	.word ndls_debug_alloc - snippets_list
	.word ndls_debug_free - snippets_list 

@ The snippets should return to the caller with ldmfd sp, {pc}
@ input:
@  lr=return address
@  r12=index (SNIPPET_*)
@  r11=syscall table (caution, the snippets depend on it)
@ destroys: r10, r12
load_snippet:
  stmfd sp!, {lr}
	adr   r10, snippets_list
	ldr   r12, [r10, r12, lsl#2]
	add   r10, r10, r12
	bx    r10

@ Some file related syscalls call a sub-function which don't check correctly the return
@ of TCC_Current_HISR_Pointer. It may be null depending when the control flow is interrupted
@ by the armloader.
@ This function patches the check.
@ params: r11=syscall table
patch_TCC_Current_HISR_Pointer_return_check:
	ldr  r0, [r11, #0]  @ TCC_Current_HISR_Pointer_patch
	adr  r1, unpatch_TCC_Current_HISR_Pointer_addr
	str  r0, [r1]
	mov  r1, #0
	strb r1, [r0, #0] @ converts 'cmp r0, #4' to 'cmp r0, #0'
	bx   lr

unpatch_TCC_Current_HISR_Pointer_addr: .long 0

unpatch_TCC_Current_HISR_Pointer_return_check:
	adr  r0, unpatch_TCC_Current_HISR_Pointer_addr
	ldr  r0, [r0]
	mov  r1, #4
	strb r1, [r0, #0] @ converts back 'cmp r0, #0' to 'cmp r0, #4'
	bx   lr	

@ param: char *full_path
@ returns:
@  pointer to a malloc-ed buffer holding the file content, which must be freed with free_block
@    null in case of error
@  file size
file_export:
	full_path .req r4
	buf .req r6
	mov  buf, #0
	mov  full_path, r0
	bl   patch_TCC_Current_HISR_Pointer_return_check
	mov  r0, full_path
	.equ sstat_size, 36   @ word aligned
	sub  sp, sp, #sstat_size
	mov  r1, sp        @ sstat
	syscall stat
	mov  r1, sp        @ sstat
	add sp, sp, #sstat_size
	cmp	 r0, #0
	@ bne  file_export_exit @ TODO
	fsize .req r7
	ldr  fsize, [r1, #20] @ sstat.st_size
	ldr  fsize, =500
0: b 0b
	mov  r0, fsize
	syscall malloc
	subs buf, r0, #0
	beq  file_export_exit
	mov  r0, full_path
	adr  r1, rb
	syscall fopen
	file .req r5
	subs file, r0, #0
	beq  file_export_free_exit
	.set block_size, 4 @ TODO 1000
	readcnt .req r8
	mov  readcnt, fsize
0:
	mov  r0, buf
	mov  r1, #block_size
	sub  readcnt, readcnt, r1
	mov  r2, #1       @ count
	mov  r3, file
	syscall fread
	cmp  r0, #1
	bne  file_export_close_free_exit
	cmp  readcnt, #0
	bgt  0b
	mov  r0, file
	syscall fclose
	b   file_export_exit
file_export_close_free_exit:
	mov  r0, file
	syscall fclose
file_export_free_exit:
  mov r0, buf
	syscall free
file_export_exit:
	bl   unpatch_TCC_Current_HISR_Pointer_return_check
	mov  r0, buf
	mov  r1, fsize
	ldmfd sp, {pc}

@ calls free
free_block:
	syscall free
	ldmfd sp, {pc}

@ params: char *pathname, int flags
file_open:
	mov  r4, r0
	bl   patch_TCC_Current_HISR_Pointer_return_check
	adr  r1, rb
	syscall fopen
	mov  r4, r0
	bl   unpatch_TCC_Current_HISR_Pointer_return_check
	mov  r0, r4
	ldmfd sp, {pc}
rb:	.asciz "rb"
	.align

file_read:

file_write:

file_close:

file_unlink:

// See Ndless's emu.c
ndls_debug_alloc:
	swi  NDLSEMU_DEBUG_ALLOC
	ldmfd sp, {pc}

ndls_debug_free:
	swi  NDLSEMU_DEBUG_FREE
	ldmfd sp, {pc}
