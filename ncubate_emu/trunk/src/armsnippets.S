/*
 * This file is wrapped as host-side object file pointed to by the symbol binary_snippets_bin_start
 * and ending at binary_armsnippets_bin_end.
 */

start:

entry_points_list:    @ defined by SNIPPETS_EP_*
	.word oscalls_tables_list - entry_points_list
	.word load_snippet - entry_points_list

oscalls_tables_list:
	.word 1 @ size
	@ format: OS_VERSION_*, offset from start
@	.word 0x102132A0, oscalls_1_7_cas - start
	.word 0x10211290, oscalls_1_7_non_cas - start

	.equ fopen, 0
	.equ fread, 1
	.equ fwrite, 2
	.equ fclose, 3
	.equ unlink, 4

oscalls_1_7_cas:
@ TODO

oscalls_1_7_non_cas:
	.word 0x102A0788 @ TCC_Current_HISR_Pointer_patch: cmp r0, #4
	.word 0x102A4CBC @ fopen
	.word 0x102A4EB0 @ fread
	.word 0x102A5264 @ fwrite
	.word 0x102A46B4 @ fclose
	.word 0x102AA5A8 @ unlink

	.macro oscall id
	mov lr, pc
	ldr pc, [r11,#\id]
	.endm
	
@ The snippets should return to the caller with ldmfd sp, {pc}
@ input:
@  lr=return address
@  r12=index (SNIPPET_*)
@  r11=oscall table (caution, the snippets depend on it)
@ destroys: r10, r12
load_snippet:
  stmfd sp!, {lr}
	adr   r10, snippets_list
	ldr   r12, [r10, r12, lsl#2]
	add   r10, r10, r12
	bx    r10

@ Some file related syscalls call a sub-function which don't check correctly the return
@ of TCC_Current_HISR_Pointer. It may be null depending when the control flow is interrupted
@ by teh armloader.
@ This function patches the check.
@ params: r11=oscall table
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

snippets_list:      @ defined by SNIPPET_*
	.word 0 @ reserved for TCC_Current_HISR_Pointer_patch
	.word file_open - snippets_list
	.word file_read - snippets_list
	.word file_write - snippets_list
	.word file_close - snippets_list
	.word file_unlink - snippets_list

@ params: char *pathname, int flags
file_open:
	mov  r4, r0
	bl   patch_TCC_Current_HISR_Pointer_return_check
	adr  r1, rb
	oscall fopen
	mov  r4, r0
	bl   unpatch_TCC_Current_HISR_Pointer_return_check
	mov  r0, r4
	ldmfd sp, {pc}
rb:	.asciz "rb"

file_read:

file_write:

file_close:

file_unlink:
