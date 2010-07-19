#include <os.h>

	.string "PRG"

	.global main	
main:
	ldr     pc, =0x10000000
