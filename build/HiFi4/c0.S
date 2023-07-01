/*  PS register fields:  */
#define PS_WOE_SHIFT		18
#define PS_WOE_MASK		0x00040000
#define PS_WOE			PS_WOE_MASK
#define PS_CALLINC_SHIFT	16
#define PS_CALLINC_MASK		0x00030000
#define PS_CALLINC(n)		(((n)&3)<<PS_CALLINC_SHIFT)	/* n = 0..3 */
#define PS_OWB_SHIFT		8
#define PS_OWB_MASK		0x00000F00
#define PS_OWB(n)		(((n)&15)<<PS_OWB_SHIFT)	/* n = 0..15 (or 0..7) */
#define PS_RING_SHIFT		6
#define PS_RING_MASK		0x000000C0
#define PS_RING(n)		(((n)&3)<<PS_RING_SHIFT)	/* n = 0..3 */
#define PS_UM_SHIFT		5
#define PS_UM_MASK		0x00000020
#define PS_UM			PS_UM_MASK
#define PS_EXCM_SHIFT		4
#define PS_EXCM_MASK		0x00000010
#define PS_EXCM			PS_EXCM_MASK
#define PS_INTLEVEL_SHIFT	0
#define PS_INTLEVEL_MASK	0x0000000F
#define PS_INTLEVEL(n)		((n)&PS_INTLEVEL_MASK)		/* n = 0..15 */
/*  ABI-derived field values:  */
//#ifdef __XTENSA_CALL0_ABI__
//#define PS_WOE_ABI		0
//#define PS_WOECALL4_ABI		0
//#else
#define PS_WOE_ABI		PS_WOE				/* 0x40000 */
#define PS_WOECALL4_ABI		(PS_WOE | PS_CALLINC(1))	/* 0x50000, per call4 */
//#endif

	.section .entry.text, "x"
	j tra0
	.align	2
startaddr:
	.word _start0
tra0:
	l32r	a0, startaddr
	jx		a0

/////////////////////////////

	.text
	.align	4
	.type	_start0,@function
_start0:
	movi	a0, 0			// keep this register zero.

	movi	a3, PS_UM | PS_WOE_ABI	// PS.WOE = 0|1, PS.UM = 1, PS.EXCM = 0, PS.INTLEVEL = 0
	wsr.ps	a3

	/*  Setup window registers for first caller: */
	movi	a3, 1
	movi	a4, 0
	wsr	a3, WINDOWSTART
	wsr	a4, WINDOWBASE
	rsync
	/* Note:  register window has rotated, ie. a0..a15 clobbered. */

	/* Reload initial stack pointer: */
	movi	a1, __stack		// default SP
	movi	a3, _start
	callx4	a3
	j .						// keep halted

	.size	_start0, . - _start0




