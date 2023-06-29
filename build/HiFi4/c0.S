.section .entry.text
	movi 	a3, 0x0250
	slli	a3, a3, 16
	movi 	a7, 0x23
	s32i  	a7, a3, 0
	j .
	//.data
rrr:
	movi	a0, 0			// keep this register zero.
	//movi	a1, 0x00401000		// default SP
	//movi	sp, a1
	j xmain
	j .

