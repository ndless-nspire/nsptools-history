# buggy
# TODO we should rewrite the code to avoid any copyright issue
  .xdef drawString

drawString:
  mov     r12, sp
  stmfd   sp!, {r4-r6, r11, r12, lr, pc}
  sub     r11, r12, #4
  sub     sp, sp, #4
  ldrb    r3, [r0]
  mov     r5, r0
  cmp     r3, #0
  mov     r6, r1
  mov     r4, r2
  ldmeqed sp, {r4-r6, r11, sp, pc}
  ldr     r1, [r1]
  mov     r2, r3

_drawString_loop_char:
  tst     r2, #0x80
  moveq   r0, r2
  movne   r0, #0x20
  cmp     r1, #0x34
  movgt   r1, #0
  addgt   r4, r4, #1
  strgt   r1, [r6]
  cmp     r4, #0x1d
  movgt   r4, #0
  mov     r2, r4
  bl      drawChar
  ldr     r3, [r6]
  add     r5, r5, #1
  add     r3, r3, #1
  str     r3, [r6]
  ldrb    r2, [r5]
  mov     r1, r3
  cmp     r2, #0
  bne     _drawString_loop_char

ll:
  b       ll
  ldmed   sp, {r4-r6, r11, sp, pc}

drawString2:  
  mov     r12, sp
  stmfd   sp!, {r4-r6,r11,r12,lr,pc}
  sub     r11, r12, #4
  sub     sp, sp, #4
  mov     r5, r0
  ldrb    r0, [r0]
  mov     r4, r1
  cmp     r0, #0
  mov     r6, r2
  ldmeqed sp, {r4-r6,r11,sp,pc}
  ldr     r1, [r1]

_drawString2_1:
  mov     r2, r6
  bl      drawChar
  ldr     r3, [r4]
  add     r3, r3, #1
  str     r3, [r4]
  ldrb    r0, [r5, #1]!
  mov     r1, r3
  cmp     r0, #0
  bne     _drawString2_1
l2:
  b       l2
  ldmed   sp, {r4-r6, r11, sp, pc}

drawChar:
  stmfd   sp!, {r4-r7, lr}
  adr     r7, fontTable
  mov     r3, r2, lsl#5
  add     r1, r1, r1, lsl#1
  add     r3, r3, r2, lsl#3
  mov     r4, r1, lsl#1
  mov     r6, r0, lsl#3
  mov     r1, r3, lsl#5
  mov     r5, #0

_drawChar_1:
  add     r3, r6, r5
  ldr     r0, [r7, r3, lsl#2]
  mov     lr, #0
  b       _drawChar_3
  
_drawChar_2:
  ldrb    r2, [r3]
  tst     lr, #1
  add     lr, lr, #1
  and     r12, r2, #0xf0
  andeq   r12, r2, #0xf
  cmp     lr, #5
  strb    r12, [r3]
  bgt     _drawChar_4

_drawChar_3:
  add     r3, r4, lr
  rsb     r2, lr, #5
  add     r3, r3, r3, lsr#31
  mov     r2, r0, lsr r2
  add     r3, r1, r3, asr#1
  add     r3, r3, #0xA4000000
  tst     r2, #1
  add     r3, r3, #0x100
  bne     _drawChar_2
  ldrb    r12, [r3]
  tst     lr, #1
  orr     r2, r12, #0xf
  add     lr, lr, #1
  mvneq   r2, r12, lsl#28
  mvneq   r2, r2, lsr#28
  cmp     lr, #5
  strb    r2, [r3]
  ble     _drawChar_3

_drawChar_4:
  add     r5, r5, #1
  cmp     r5, #7
  add     r1, r1, #0xa0
  ble     _drawChar_1
  ldmfd   sp!, {r4-r7, pc}
	
  .ltorg
fontTable:  .incbin "components/Font8X.bin"
