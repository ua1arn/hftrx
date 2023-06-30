	.section .entry.text, "x"
	//.text
	.global	_start

_start:
.stsss:
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop

	j .stsss

	movi	a0, 0			// keep this register zero.
	movi 	a3, 0x0250
	slli	a3, a3, 16
	movi 	a7, 0x23
	s32i  	a7, a3, 0
	j .
	//.data
rrr:
	//movi	a1, __stack		// default SP
	//movi	sp, __stack
	j xmain
	j .

