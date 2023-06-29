.section .entry.text
	j rrr
	//.data
rrr:
	movi	a0, 0			// keep this register zero.
	//movi	a1, 0x00401000		// default SP
	//movi	sp, a1
	j xmain
	j .

