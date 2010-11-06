// arm_state structure
#define REG_PC 60(%ebx)
#define REG_CPSR 64(%ebx)
#define FLAG_C 70(%ebx)

// translation structure
#define TRANS_JUMP_TABLE 4
#define TRANS_END_PTR 12

#define RAM_FLAGS 0x80000+0x2000000+0x20000
#define RF_READ_BREAKPOINT   1
#define RF_WRITE_BREAKPOINT  2
#define RF_EXEC_BREAKPOINT   4
#define RF_EXEC_DEBUG_NEXT   8
#define RF_EXEC_HACK         16
#define RF_CODE_TRANSLATED   32
#define RF_CODE_NO_TRANSLATE 64
#define RF_READ_ONLY         128
#define RFS_TRANSLATION_INDEX 8

#define WRITE_SPECIAL_FLAGS 128+64+32+2

	.text
.globl	_translation_enter
_translation_enter:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	movl	%esp, _in_translation_esp

	movl	$_arm, %ebx
	movl	REG_PC, %eax
	jmp	_translation_next

.globl	_translation_next_bx
_translation_next_bx:
	testb	$1, %al
	jne	switch_to_thumb

.globl	_translation_next
_translation_next:
	movl	%eax, REG_PC

	cmpl	$0, _cycle_count_delta
	jns	return

	cmpl	$0, _cpu_events
	jnz	return

	// eax = VM_MEM_PTR(eax)
	movl	%eax, %ecx
	shrl	$10, %ecx
	addl	_addr_cache_delta(, %ecx, 4), %eax
	testl	$0x80000003, %eax
	jnz	return
addr_ok:

	movl	RAM_FLAGS(%eax), %edx
	testb	$RF_CODE_TRANSLATED, %dl
	jz	return         // Not translated

	movl	%eax, _in_translation_pc_ptr

	shrl	$RFS_TRANSLATION_INDEX, %edx
	shll	$4, %edx
	addl	$_translation_table, %edx

	// Add one cycle for each instruction from this point to the end
	movl	TRANS_END_PTR(%edx), %ecx
	subl	%eax, %ecx
	shrl	$2, %ecx
	addl	%ecx, _cycle_count_delta

	movl	TRANS_JUMP_TABLE(%edx), %edx
	jmp	*(%edx, %eax)

return:
	andl	$0, _in_translation_esp
	popl	%edi
	popl	%esi
	popl	%ebx
	popl	%ebp
	ret

switch_to_thumb:
	decl	%eax
	movl	%eax, REG_PC
	orb	$0x20, REG_CPSR
	jmp	return

	.align	4
.globl _arm_shift_proc
_arm_shift_proc:
	.long	lsl
	.long	lsr
	.long	asr
	.long	0
	.long	lsl_carry
	.long	lsr_carry
	.long	asr_carry
	.long	ror_carry

	.text
lsl:
	cmpb	$32, %cl
	jae	ls_32
	shll	%cl, %eax
	ret

lsr:
	cmpb	$32, %cl
	jae	ls_32
	shrl	%cl, %eax
	ret
ls_32:
	xorl	%eax, %eax
	ret

asr:
	cmpb	$32, %cl
	jae	asr_32
	sarl	%cl, %eax
	ret
asr_32:
	sarl	$31, %eax
	ret

lsl_carry:
	cmpb	$32, %cl
	jae	lsl_carry_32
	testb	%cl, %cl
	je	lsl_carry_zero
	shll	%cl, %eax
	setc	FLAG_C
lsl_carry_zero:
	ret
lsl_carry_32:
	jne	ls_carry_33
	shrl	$1, %eax
	setc	FLAG_C
	xorl	%eax, %eax
	ret

lsr_carry:
	cmpb	$32, %cl
	jae	lsr_carry_32
	testb	%cl, %cl
	je	lsr_carry_zero
	shrl	%cl, %eax
	setc	FLAG_C
lsr_carry_zero:
	ret
lsr_carry_32:
	jne	ls_carry_33
	shll	$1, %eax
	setc	FLAG_C
	xorl	%eax, %eax
	ret
ls_carry_33:
	xorl	%eax, %eax
	movb	%al, FLAG_C
	ret

asr_carry:
	cmpb	$32, %cl
	jae	asr_carry_32
	testb	%cl, %cl
	je	asr_carry_zero
	sarl	%cl, %eax
	setc	FLAG_C
asr_carry_zero:
	ret
asr_carry_32:
	sarl	$16, %eax
	sarl	$16, %eax
	setc	FLAG_C
	ret

ror_carry:
	testb	$31, %cl
	jz	ror_carry_mult_32
	rorl	%cl, %eax
	setc	FLAG_C
ror_carry_zero:
	ret
ror_carry_mult_32:
	testb	%cl, %cl
	je	ror_carry_zero
	rorl	$16, %eax
	rorl	$16, %eax
	setc	FLAG_C
	ret

// u32 __attribute__((fastcall)) read_byte(u32 addr);
.globl	@read_byte@4
.align	16
@read_byte@4:
	movl	%ecx, %eax
	shrl	$10, %eax
	addl	_addr_cache_delta(, %eax, 4), %ecx
	testl	%ecx, %ecx
	js	rb_miss
	movl	%ecx, %edx
	andl	$-4, %edx
	testb	$RF_READ_BREAKPOINT, RAM_FLAGS(%edx)
	jnz	rb_special
	movzbl	(%ecx), %eax
	ret
rb_special:
	subl	_addr_cache_delta(, %eax, 4), %ecx
	jmp	@slow_read_byte@4
rb_miss:
	subl	_addr_cache_delta(, %eax, 4), %ecx
	pushl	%ecx
	pushl	$_data_abort
	pushl	$0
	pushl	%ecx
	call	_addr_cache_miss
	addl	$12, %esp
	popl	%ecx
	test	%eax, %eax
	jnz	@read_byte@4
	jmp	@slow_read_byte@4

// u32 __attribute__((fastcall)) read_half(u32 addr);
.globl	@read_half@4
.align	16
@read_half@4:
	movl	%ecx, %eax
	shrl	$10, %eax
	addl	_addr_cache_delta(, %eax, 4), %ecx
	testl	$0x80000001, %ecx
	jnz	rh_miss
	movl	%ecx, %edx
	andl	$-4, %edx
	testb	$RF_READ_BREAKPOINT, RAM_FLAGS(%edx)
	jnz	rh_special
	movzwl	(%ecx), %eax
	ret
rh_special:
	subl	_addr_cache_delta(, %eax, 4), %ecx
	jmp	@slow_read_half@4
rh_miss:
	subl	_addr_cache_delta(, %eax, 4), %ecx
	pushl	%ecx
	pushl	$_data_abort
	pushl	$1
	pushl	%ecx
	call	_addr_cache_miss
	addl	$12, %esp
	popl	%ecx
	test	%eax, %eax
	jnz	@read_half@4
	jmp	@slow_read_half@4

// u32 __attribute__((fastcall)) read_word(u32 addr);
.globl	@read_word@4
.align	16
@read_word@4:
	movl	%ecx, %eax
	shrl	$10, %eax
	addl	_addr_cache_delta(, %eax, 4), %ecx
	testl	$0x80000003, %ecx
	jnz	rw_miss
	testb	$RF_READ_BREAKPOINT, RAM_FLAGS(%ecx)
	jnz	rw_special
	movl	(%ecx), %eax
	ret
rw_special:
	subl	_addr_cache_delta(, %eax, 4), %ecx
	jmp	@slow_read_word@4
rw_miss:
	subl	_addr_cache_delta(, %eax, 4), %ecx
	pushl	%ecx
	pushl	$_data_abort
	pushl	$3
	pushl	%ecx
	call	_addr_cache_miss
	addl	$12, %esp
	popl	%ecx
	test	%eax, %eax
	jnz	@read_word@4
	jmp	@slow_read_word@4

// void __attribute__((fastcall)) write_byte(u32 addr, u8 value);
.globl	@write_byte@8
.align	16
@write_byte@8:
	movl	%ecx, %eax
	shrl	$10, %eax
	addl	_addr_cache_delta(, %eax, 4), %ecx
	testl	%ecx, %ecx
	js	wb_miss
	pushl	%eax
	movl	%ecx, %eax
	andl	$-4, %eax
	testb	$WRITE_SPECIAL_FLAGS, RAM_FLAGS(%eax)
	popl	%eax
	jnz	wb_special
	movb	%dl, (%ecx)
	ret
wb_special:
	subl	_addr_cache_delta(, %eax, 4), %ecx
	jmp	@slow_write_byte@8
wb_miss:
	subl	_addr_cache_delta(, %eax, 4), %ecx
	pushl	%ecx
	pushl	%edx
	pushl	$_data_abort
	pushl	$0
	pushl	%ecx
	call	_addr_cache_miss
	addl	$12, %esp
	popl	%edx
	popl	%ecx
	test	%eax, %eax
	jnz	@write_byte@8
	jmp	@slow_write_byte@8

// void __attribute__((fastcall)) write_half(u32 addr, u16 value);
.globl	@write_half@8
.align	16
@write_half@8:
	movl	%ecx, %eax
	shrl	$10, %eax
	addl	_addr_cache_delta(, %eax, 4), %ecx
	testl	$0x80000001, %ecx
	jnz	wh_miss
	pushl	%eax
	movl	%ecx, %eax
	andl	$-4, %eax
	testb	$WRITE_SPECIAL_FLAGS, RAM_FLAGS(%eax)
	popl	%eax
	jnz	wh_special
	movw	%dx, (%ecx)
	ret
wh_special:
	subl	_addr_cache_delta(, %eax, 4), %ecx
	jmp	@slow_write_half@8
wh_miss:
	subl	_addr_cache_delta(, %eax, 4), %ecx
	pushl	%ecx
	pushl	%edx
	pushl	$_data_abort
	pushl	$1
	pushl	%ecx
	call	_addr_cache_miss
	addl	$12, %esp
	popl	%edx
	popl	%ecx
	test	%eax, %eax
	jnz	@write_half@8
	jmp	@slow_write_half@8

// void __attribute__((fastcall)) write_word(u32 addr, u32 value);
.globl	@write_word@8
.align	16
@write_word@8:
	movl	%ecx, %eax
	shrl	$10, %eax
	addl	_addr_cache_delta(, %eax, 4), %ecx
	testl	$0x80000003, %ecx
	jnz	ww_miss
	testb	$WRITE_SPECIAL_FLAGS, RAM_FLAGS(%ecx)
	jnz	ww_special
	movl	%edx, (%ecx)
	ret
ww_special:
	subl	_addr_cache_delta(, %eax, 4), %ecx
	jmp	@slow_write_word@8
ww_miss:
	subl	_addr_cache_delta(, %eax, 4), %ecx
	pushl	%ecx
	pushl	%edx
	pushl	$_data_abort
	pushl	$3
	pushl	%ecx
	call	_addr_cache_miss
	addl	$12, %esp
	popl	%edx
	popl	%ecx
	test	%eax, %eax
	jnz	@write_word@8
	jmp	@slow_write_word@8
