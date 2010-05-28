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
@  r11=oscall table
@ destroys: r10, r12
load_snippet:
  stmfd   sp!, {lr}
	adr r10, snippets_list
	ldr r12, [r10, r12]
	add r10, r10, r12
	bx r10

snippets_list:      @ defined by SNIPPET_*
	.word file_open - snippets_list
	.word file_read - snippets_list
	.word file_write - snippets_list
	.word file_close - snippets_list
	.word file_unlink - snippets_list

@ params: char *pathname, int flags
file_open:
	adr r1, rb
	oscall fopen
	ldmfd sp, {pc}
rb:	.asciz "rb"

file_read:

file_write:

file_close:

file_unlink:
