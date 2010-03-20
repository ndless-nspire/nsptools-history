  .equ screen_address,0xA4000100
	.equ screen_size_in_bytes,38400

	.text

_start: .global _start  
  ldr sp, =0x180F0000

	mov r12,sp
	stmfd sp!,{r4-r12,lr,pc}
	sub fp,r12,#4 @ frame ptr

	@@@ display the image
	ldr r0,=screen_address
	adr r1,image
	ldr R2,=screen_size_in_bytes
	bl  memcpy
loop:
  b   loop
	
exit:
	ldmfd sp,{r4-fp,sp,pc}

memcpy:
  CMP     R2, #0x1F
  MOV     R12, R0
  BLS     loc_102A6188
  EOR     R3, R0, R1
  TST     R3, #3
  BNE     loc_102A6188
  TST     R0, #3
  BEQ     loc_102A6150

loc_102A613C:
  LDRB    R3, [R1],#1
  SUB     R2, R2, #1
  STRB    R3, [R12],#1
  TST     R12, #3
  BNE     loc_102A613C

loc_102A6150:
  CMP     R2, #3
  BLS     loc_102A616C

loc_102A6158:
  LDR     R3, [R1],#4
  SUB     R2, R2, #4
  CMP     R2, #3
  STR     R3, [R12],#4
  BHI     loc_102A6158

loc_102A616C:
  CMP     R2, #0
  BXEQ    LR

loc_102A6174:
  LDRB    R3, [R1],#1
  SUBS    R2, R2, #1
  STRB    R3, [R12],#1
  BNE     loc_102A6174
  BX      LR
  
loc_102A6188:
  CMP     R2, #0
  BXEQ    LR

loc_102A6190:
  LDRB    R3, [R1],#1
  SUBS    R2, R2, #1
  STRB    R3, [R12],#1
  BNE     loc_102A6190
  BX      LR

	.ltorg @ required before image, else constant pool will be too far
image:	.incbin "image.bin"
	.end
