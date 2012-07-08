#include <os.h>

main: .global main
	push {r4, lr}
	mov  r0, #0
	@ required because stdout needs the interrupts currently disabled by Ndless
	syscall(TCT_Local_Control_Interrupts)
	mov  r4, r0
	adr  r0, hellostr
	syscall(puts)
	mov  r0, r4
	syscall(TCT_Local_Control_Interrupts)
	pop  {r4, pc}
	
hellostr: .asciz "hello world!"
