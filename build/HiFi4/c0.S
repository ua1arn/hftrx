	.section .entry.text, "x"
	//.text
	.global	_start
	.type	_start,@function

_start:
#if 1

	movi	a0, 0			// keep this register zero.
	movi 	a3, 0x0042
	slli	a3, a3, 16
	mov		sp, a3
	mov		a1, a3

	j .ee
	.align 4
.ee:
	//entry a1, 32
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
j xmain
	.size	_start, . - _start
