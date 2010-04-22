/*
 * This file is wrapped as host-side object file pointed to by the symbol binary_snippets_bin_start
 * and ending at binary_armsnippets_bin_end.
 * The return address of the snippets is lr
 */

@ input:
@  lr=return address
@  r12=index (SNIPPET_*)
@ destroys: r11, r12
load_snippet:
	adr r11, snippets_list
	ldr r12, [r11, r12]
	add r11, r11, r12
	bx r11

snippets_list:      @ each snippet in the list is referenced by its SNIPPET_ index
	.word file_open - snippets_list
	.word file_read - snippets_list
	.word file_write - snippets_list
	.word file_close - snippets_list
	.word file_unlink - snippets_list

@ params: char *pathname, int flags
file_open:
	bx lr

file_read:

file_write:

file_close:

file_unlink:
