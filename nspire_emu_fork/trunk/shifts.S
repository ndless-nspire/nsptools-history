#define FLAG_C 70(%ebx)

	.text
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
