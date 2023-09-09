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

#define XCHAL_PS_OWB_SHIFT		8
#define XCHAL_PS_OWB_BITS		4
#define EXCSAVE_1			209

//---------------------------------------------

	.section .entry.text,"x"
	.align 4

	.global Start
Start:

	j		BypassLiteral

//---------------------------------------------

	.section .literal,"a"

	.global Literal
Literal:

	//Literals located here

//---------------------------------------------

	.text
	.align 4

	.global BypassLiteral
BypassLiteral:

	movi		a0,0
	wsr.intenable	a0

	movi		a2,VECTORS		//vector table base address
	wsr.vecbase	a2

	movi		a3,PS_UM|PS_WOE_ABI	//PS.WOE = 0|1, PS.UM = 1, PS.EXCM = 0, PS.INTLEVEL = 0
	wsr.ps		a3

	movi		a2,0x144
	wsr.prefctl	a2

	//Setup window registers for first caller:

	movi		a3,0			//was: 1
	movi		a4,0
	wsr.WINDOWSTART	a3			//Call-window start bits
	wsr.WINDOWBASE	a4			//4 bits width: Base of current address-register window
	rsync

	//Note: register window has rotated, ie. a0..a15 clobbered.

	movi		a4,0
	wsr.excsave2	a4
	wsr.excsave3	a4

	//Reload initial stack pointer:

	movi		a1,__stack		//default SP
	movi		sp,__stack		//added

	movi		a0,0			//mark start of call frames in stack

#if 1

	j		libc_init		//jump instead call (no need spend stack)

#else
	call0		libc_init

//	movi		a3,libc_init
//	callx8		a3

	j		.			//keep halted

#endif

//---------------------------------------------

/*
https://sachin0x18.github.io/posts/demystifying-xtensa-isa/
https://github.com/esp-rs/xtensa-lx-rt/issues/5

Xtensa_lx Overview handbook.pdf
isa-summary.pdf

---

Window exception handlers

Exception 		Offset
WindowOverflow4 	0x000
WindowUnderflow4 	0x040
WindowOverflow8 	0x080
WindowUnderflow8 	0x0C0
WindowOverflow12 	0x100
WindowUnderflow12 	0x140

Actually useful exception handlers

Exception 		Offset
UserException 		0x340
Level2Interrupt 	0x180
Level3Interrupt 	0x1c0

Less useful exception handlers

Exception 		Offset
Level4Interrupt 	0x200
Level5Interrupt 	0x240
DebugException 		0x2c0
NMIException 		0x300
KernelException 	0x340
DoubleException 	0x3c0
Reset 			0x400
*/

//---------------------------------------------

    .section .Vectors.text,"ax"

    .global VECTORS
VECTORS:

    .org    0x0
    .global _WindowOverflow4
_WindowOverflow4:

    s32e    a0, a5, -16     /* save a0 to call[j+1]'s stack frame */
    s32e    a1, a5, -12     /* save a1 to call[j+1]'s stack frame */
    s32e    a2, a5,  -8     /* save a2 to call[j+1]'s stack frame */
    s32e    a3, a5,  -4     /* save a3 to call[j+1]'s stack frame */
    rfwo                    /* rotates back to call[i] position */

    .org    0x40
    .global _WindowUnderflow4
_WindowUnderflow4:

    l32e    a0, a5, -16     /* restore a0 from call[i+1]'s stack frame */
    l32e    a1, a5, -12     /* restore a1 from call[i+1]'s stack frame */
    l32e    a2, a5,  -8     /* restore a2 from call[i+1]'s stack frame */
    l32e    a3, a5,  -4     /* restore a3 from call[i+1]'s stack frame */
    rfwu

#if 1

    .align  4
    .global _xt_alloca_exc
_xt_alloca_exc:

    rsr     a0, WINDOWBASE  /* grab WINDOWBASE before rotw changes it */
    rotw    -1              /* WINDOWBASE goes to a4, new a0-a3 are scratch */
    rsr     a2, PS
    extui   a3, a2, XCHAL_PS_OWB_SHIFT, XCHAL_PS_OWB_BITS
    xor     a3, a3, a4      /* bits changed from old to current windowbase */
    rsr     a4, EXCSAVE_1   /* restore original a0 (now in a4) */
    slli    a3, a3, XCHAL_PS_OWB_SHIFT
    xor     a2, a2, a3      /* flip changed bits in old window base */
    wsr     a2, PS          /* update PS.OWB to new window base */
    rsync

    _bbci.l a4, 31, _WindowUnderflow4
    rotw    -1              /* original a0 goes to a8 */
    _bbci.l a8, 30, _WindowUnderflow8
    rotw    -1
    j               _WindowUnderflow12

#endif

    .org    0x80
    .global _WindowOverflow8
_WindowOverflow8:

    s32e    a0, a9, -16     /* save a0 to call[j+1]'s stack frame */
    l32e    a0, a1, -12     /* a0 <- call[j-1]'s sp
                               (used to find end of call[j]'s frame) */
    s32e    a1, a9, -12     /* save a1 to call[j+1]'s stack frame */
    s32e    a2, a9,  -8     /* save a2 to call[j+1]'s stack frame */
    s32e    a3, a9,  -4     /* save a3 to call[j+1]'s stack frame */
    s32e    a4, a0, -32     /* save a4 to call[j]'s stack frame */
    s32e    a5, a0, -28     /* save a5 to call[j]'s stack frame */
    s32e    a6, a0, -24     /* save a6 to call[j]'s stack frame */
    s32e    a7, a0, -20     /* save a7 to call[j]'s stack frame */
    rfwo                    /* rotates back to call[i] position */

    .org    0xC0
    .global _WindowUnderflow8
_WindowUnderflow8:

    l32e    a0, a9, -16     /* restore a0 from call[i+1]'s stack frame */
    l32e    a1, a9, -12     /* restore a1 from call[i+1]'s stack frame */
    l32e    a2, a9,  -8     /* restore a2 from call[i+1]'s stack frame */
    l32e    a7, a1, -12     /* a7 <- call[i-1]'s sp
                               (used to find end of call[i]'s frame) */
    l32e    a3, a9,  -4     /* restore a3 from call[i+1]'s stack frame */
    l32e    a4, a7, -32     /* restore a4 from call[i]'s stack frame */
    l32e    a5, a7, -28     /* restore a5 from call[i]'s stack frame */
    l32e    a6, a7, -24     /* restore a6 from call[i]'s stack frame */
    l32e    a7, a7, -20     /* restore a7 from call[i]'s stack frame */
    rfwu

    .org    0x100
    .global _WindowOverflow12
_WindowOverflow12:

    s32e    a0,  a13, -16   /* save a0 to call[j+1]'s stack frame */
    l32e    a0,  a1,  -12   /* a0 <- call[j-1]'s sp
                               (used to find end of call[j]'s frame) */
    s32e    a1,  a13, -12   /* save a1 to call[j+1]'s stack frame */
    s32e    a2,  a13,  -8   /* save a2 to call[j+1]'s stack frame */
    s32e    a3,  a13,  -4   /* save a3 to call[j+1]'s stack frame */
    s32e    a4,  a0,  -48   /* save a4 to end of call[j]'s stack frame */
    s32e    a5,  a0,  -44   /* save a5 to end of call[j]'s stack frame */
    s32e    a6,  a0,  -40   /* save a6 to end of call[j]'s stack frame */
    s32e    a7,  a0,  -36   /* save a7 to end of call[j]'s stack frame */
    s32e    a8,  a0,  -32   /* save a8 to end of call[j]'s stack frame */
    s32e    a9,  a0,  -28   /* save a9 to end of call[j]'s stack frame */
    s32e    a10, a0,  -24   /* save a10 to end of call[j]'s stack frame */
    s32e    a11, a0,  -20   /* save a11 to end of call[j]'s stack frame */
    rfwo                    /* rotates back to call[i] position */

    .org 0x140
    .global _WindowUnderflow12
_WindowUnderflow12:

    l32e    a0,  a13, -16   /* restore a0 from call[i+1]'s stack frame */
    l32e    a1,  a13, -12   /* restore a1 from call[i+1]'s stack frame */
    l32e    a2,  a13,  -8   /* restore a2 from call[i+1]'s stack frame */
    l32e    a11, a1,  -12   /* a11 <- call[i-1]'s sp
                               (used to find end of call[i]'s frame) */
    l32e    a3,  a13,  -4   /* restore a3 from call[i+1]'s stack frame */
    l32e    a4,  a11, -48   /* restore a4 from end of call[i]'s stack frame */
    l32e    a5,  a11, -44   /* restore a5 from end of call[i]'s stack frame */
    l32e    a6,  a11, -40   /* restore a6 from end of call[i]'s stack frame */
    l32e    a7,  a11, -36   /* restore a7 from end of call[i]'s stack frame */
    l32e    a8,  a11, -32   /* restore a8 from end of call[i]'s stack frame */
    l32e    a9,  a11, -28   /* restore a9 from end of call[i]'s stack frame */
    l32e    a10, a11, -24   /* restore a10 from end of call[i]'s stack frame */
    l32e    a11, a11, -20   /* restore a11 from end of call[i]'s stack frame */
    rfwu

//---------------------------------------------
