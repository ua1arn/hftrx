.section .entry.text
	movi	a0, 0			// keep this register zero.
//	movi	a1, __stack		// default SP
//	movi	sp, __stack
	j xmain
	j .

