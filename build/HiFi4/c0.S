	.section .entry.text, "x"
	//.text
	.global	_start
	.type	_start,@function

_start:
#if 1
	/* Make 0x20048000 value */
	movi	a0, 0			// keep this register zero.
	movi 	a3, 0x200
	slli	a3, a3, 20
	movi 	a4, 0x480
	slli	a4, a4, 8
	addi	a4, a3, 0

	mov		sp, a3
	mov		a1, a3

	//movi	a1, __stack		// default SP
	//movi	sp, __stack

	j xmain + 3

#endif
#if 0
	entry a1, 32
	movi	a0, 0			// keep this register zero.
	movi 	a3, 0x0250
	slli	a3, a3, 16
	movi 	a7, 0x25
	s32i  	a7, a3, 0
	j .
#endif
j xmain + 3
.ee2:
	.word __stack

	.size	_start, . - _start




