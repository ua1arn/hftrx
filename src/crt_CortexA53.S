/* $Id$ */
// Inspired by
// https://github.com/NienfengYao/armv8-bare-metal
// See also
// Application Note Bare-metal Boot Code for ARMv8-A Processors Version 1.0

#define vector_table_align .align 11    /* Vector tables must be placed at a 2KB-aligned address */
#define vector_entry_align .align 7     /* Each entry is 128B in size*/
#define text_align .align  2            /* Text alignment */

/*
 * Some defines for the program status registers
 */
	ARM_MODE_USER  = 0x10      /* Normal User Mode                             */
	ARM_MODE_FIQ   = 0x11      /* FIQ Fast Interrupts Mode                     */
	ARM_MODE_IRQ   = 0x12      /* IRQ Standard Interrupts Mode                 */
	ARM_MODE_SVC   = 0x13      /* Supervisor Interrupts Mode            		  */
	ARM_MODE_MON   = 0x16      /* Monitor Interrupts Mode (With Security Extensions) */
	ARM_MODE_ABORT = 0x17      /* Abort Processing memory Faults Mode          */
	ARM_MODE_HYP	  = 0x1A      /* Hypervisor Interrupts Mode            		  */
	ARM_MODE_UNDEF = 0x1B      /* Undefined Instructions Mode                  */
	ARM_MODE_SYS   = 0x1F      /* System Running in Priviledged Operating Mode */
   
	/* Standard definitions of mode bits and interrupt (I & F) flags in PSRs */
	I_BIT          = 0x80      /* disable IRQ when I bit is set */
	F_BIT          = 0x40      /* disable FIQ when F bit is set */
 
	 STACKSIZEUND = 4096
	 STACKSIZEABT = 4096
	 STACKSIZEFIQ = 4096
	 STACKSIZEHYP = 4096
	 STACKSIZEMON = 4096
	 STACKSIZESVC = 4096
	 STACKSIZEIRQ = 4096

	 STACKSIZESYSBOOT = 4096

	.section ".isr_vector", "ax"
	//.code 64
        
/****************************************************************************/
/*               Vector table and reset entry                               */
/* Table B1-3 The vector tables */
/****************************************************************************/
	// DDI0500J_cortex_a53_r0p4_trm.pdf 4.3.74 Vector Base Address Register, EL3
	// ARMv8-A-Programmer-Guide.pdf 10.4 AArch64 exception table
	// Bits 10..0 of address should be zero
	.global __Vectors64
	// Vector Base Address Register (Bits 10..0 of address should be zero)
	.align 11, 0	/* 2048 bytes align */
__Vectors64:
	.org __Vectors64 + 0x000	// Current EL with SP0
	b	_curr_el_sp0_sync		/* Synchronous */
	.org __Vectors64 + 0x080	// Current EL with SP0
	b	_curr_el_sp0_irq		/* IRQ/vIRQ */
	.org __Vectors64 + 0x100	// Current EL with SP0
	b	_curr_el_sp0_fiq		/* FIQ/vFIQ */
	.org __Vectors64 + 0x180	// Current EL with SP0
	b	_curr_el_sp0_serror		/* SError/vSError */

	.org __Vectors64 + 0x200	// Current EL with SPx
	b	_curr_el_spx_sync		/* Synchronous */
	.org __Vectors64 + 0x280	// Current EL with SPx
	b	_curr_el_spx_irq		/* IRQ/vIRQ */
	.org __Vectors64 + 0x300	// Current EL with SPx
	b	_curr_el_spx_fiq		/* FIQ/vFIQ */
	.org __Vectors64 + 0x380	// Current EL with SPx
	b	_curr_el_spx_serror		/* SError/vSError */

	.org __Vectors64 + 0x400	// Lower EL using AArch64
	b	_lower_el_aarch64_sync
	.org __Vectors64 + 0x480	// Lower EL using AArch64
	b	_lower_el_aarch64_irq
	.org __Vectors64 + 0x500	// Lower EL using AArch64
	b	_lower_el_aarch64_fiq
	.org __Vectors64 + 0x580	// Lower EL using AArch64
	b	_lower_el_aarch64_serror

	.org __Vectors64 + 0x600	// Lower EL using AArch32
	b	_lower_el_aarch32_sync
	.org __Vectors64 + 0x680	// Lower EL using AArch32
	b	_lower_el_aarch32_irq
	.org __Vectors64 + 0x700	// Lower EL using AArch32
	b	_lower_el_aarch32_fiq
	.org __Vectors64 + 0x780	// Lower EL using AArch32
	b	_lower_el_aarch32_serror
   	.ltorg

   	.section ".startup0", "ax"
   	//.code 64
   
	.global Reset_Handler
   	.extern addr64_preparedata
   	.extern SystemInit
   	.extern _start
   	.extern __stack

/****************************************************************************/
/*                           Reset handler                                  */
/****************************************************************************/
Reset_Handler:
	ldr x30, =__stack_cpu0_fiq_end
	MSR SPSR_fiq, x30
	ldr x30, =__stack_cpu0_und_end
	MSR SPSR_und, x30
	ldr x30, =__stack_cpu0_abt_end
	MSR SPSR_abt, x30
	ldr x30, =__stack_cpu0_irq_end
	MSR SPSR_irq, x30
	ldr x30, =__stack_cpu0_sys_end
	mov sp, x30

	mov x30, XZR
	//BL addr64_preparedata	/* fill .bss and prepare constant data */
	BL SystemInit
	BL addr64_preparedata	/* fill .bss and prepare constant data */

	ldr x30, =__stack
	mov sp, x30
	mov x30, XZR
/*
 * Boot CPU general-purpose register settings:
 *   x0 = physical address of device tree blob (dtb) in system RAM.
 *   x1 = 0 (reserved for future use)
 *   x2 = 0 (reserved for future use)
 *   x3 = 0 (reserved for future use)
 */
 	mov	x0, XZR
 	mov	x1, XZR
 	mov	x2, XZR
 	mov	x3, XZR

	BL _start
1:
	b 1b

   .ltorg

	.align 4, 0
	.ascii " DREAM RX project " __DATE__ " " __TIME__ " "
	.align 4, 0
/****************************************************************************/
/*                         Default interrupt handler                        */
/****************************************************************************/
#if 0
	.section ".text"
   //.code 64
/* ================================================================== */
/* Entry point for the IRQ handler */
/* ================================================================== */
/*
	• 13 general-purpose 32-bit registers, R0 to R12.
	• Three 32-bit registers with special uses, SP, LR, and PC, that can be described as R13 to R15.

	Figure B1-2 ARM core registers, PSRs, and ELR_hyp, showing register banking
*/
/*
	11.66 MSR (general-purpose register to PSR)
	fields mnemonics:
	c	control field mask byte, PSR[7:0] (privileged software execution)
	x	extension field mask byte, PSR[15:8] (privileged software execution)
	s	status field mask byte, PSR[23:16] (privileged software execution)
	f	flags field mask byte, PSR[31:24] (privileged software execution).
*/
 	.align 	4, 0
    .func   IRQ_Handler
IRQ_Handler:

	sub     lr, lr, #4                  // Pre-adjust LR
	srsfd   sp!, #ARM_MODE_SYS              // Save LR_irq and SPSR_irq on to the SYS stack
	//cps		#ARM_MODE_SYS                   // Change to SYS mode
	cpsid   if,#ARM_MODE_SYS                   // Change to SYS mode and disable interrupts
	push    {r0-r3, r4-r11, r12, lr}            // Save APCS corruptible registers

	mov     r3, sp                      // Move SP into R3
	and     r3, r3, #7                  // Get stack adjustment to ensure 8-byte alignment
	sub     sp, sp, r3                  // Adjust stack
	push    {r3, r4}                    // Store stack adjustment(R3) and user data(R4)

 	VMRS	R1, FPSID
	VMSR	FPSID, R1

	VMRS	R1, FPSCR
	VMRS	R2, FPEXC
 	PUSH 	{R1, R2}

	VPUSH.F32	{D0-D15}
	VPUSH.F32	{D16-D31}

    // Initialise FPSCR to a known state
    // FPSCR Loaded in to R1
 	LDR     R3,=0x00086060	//Mask off all bits that do not have to be preserved. Non-preserved bits can/should be zero.
	AND     R1,R1,R3
	VMSR    FPSCR,R1	// Initialise FPSCR to a known state

 	LDR		R0, =IRQ_Handler_GIC
	MOV		LR, PC
	BX		R0     /* and jump... */

 	VMRS	R1, FPSID
	VMSR	FPSID, R1

	VPOP.F32	{D16-D31}
 	VPOP.F32	{D0-D15}
	POP 	{R1, R2}
	VMSR	FPSCR, R1
	VMSR	FPEXC, R2

	pop     {r3, r4}                    // Restore stack adjustment(R3) and user data(R4)
	add     sp, sp, r3                  // Unadjust stack


	//clrex                               // Clear exclusive monitor for interrupted code
	pop     {r0-r3, r4-r11, r12, lr}            // Restore stacked APCS registers
	rfefd   sp!                         // Return from IRQ handler - RFEIA instruction

	.endfunc

#endif



/* Vector Table
 * see 5.1.1 Setting up a vector table in
 * Application Note Bare-metal Boot Code for ARMv8-A Processors Version 1.0
 */

/*
 * AArch64 exception types
 */
/* Current EL with SP0 */
#define AARCH64_EXC_SYNC_SP0      (0x1)   /* Synchronous */
#define AARCH64_EXC_IRQ_SP0       (0x2)   /* IRQ/vIRQ */
#define AARCH64_EXC_FIQ_SP0       (0x3)   /* FIQ/vFIQ */
#define AARCH64_EXC_SERR_SP0      (0x4)   /* SError/vSError */
/* Current EL with SPx */
#define AARCH64_EXC_SYNC_SPX      (0x11)
#define AARCH64_EXC_IRQ_SPX       (0x12)
#define AARCH64_EXC_FIQ_SPX       (0x13)
#define AARCH64_EXC_SERR_SPX      (0x14)
/* Lower EL using AArch64 */
#define AARCH64_EXC_SYNC_AARCH64  (0x21)
#define AARCH64_EXC_IRQ_AARCH64   (0x22)
#define AARCH64_EXC_FIQ_AARCH64   (0x23)
#define AARCH64_EXC_SERR_AARCH64  (0x24)
/* Lower EL using AArch32 */
#define AARCH64_EXC_SYNC_AARCH32  (0x31)
#define AARCH64_EXC_IRQ_AARCH32   (0x32)
#define AARCH64_EXC_FIQ_AARCH32   (0x33)
#define AARCH64_EXC_SERR_AARCH32  (0x34)

#if defined(ASM_FILE)
#define vector_table_align .align 11    /* Vector tables must be placed at a 2KB-aligned address */
#define vector_entry_align .align 7     /* Each entry is 128B in size*/
#define text_align .align  2            /* Text alignment */
#endif /* ASM_FILE */


/*
 * exception_frame offset definitions
 */
#define EXC_FRAME_SIZE (288)	/* sizeof(struct _exception_frame) */
#define EXC_EXC_TYPE_OFFSET (0)	/* __asm_offsetof(struct _exception_frame, exc_type) */
#define EXC_EXC_ESR_OFFSET (8)	/* __asm_offsetof(struct _exception_frame, exc_esr) */
#define EXC_EXC_SP_OFFSET (16)	/* __asm_offsetof(struct _exception_frame, exc_sp) */
#define EXC_EXC_ELR_OFFSET (24)	/* __asm_offsetof(struct _exception_frame, exc_elr) */
#define EXC_EXC_SPSR_OFFSET (32)/* __asm_offsetof(struct _exception_frame, exc_spsr) */

	.section ".text"

.macro push_trapframe
	/*
	 * store generic registers from (x29,x30) pair to (x1,x2) pair.
	 */
	stp	x29, x30, [sp, #-16]!
	stp	x27, x28, [sp, #-16]!
	stp	x25, x26, [sp, #-16]!
	stp	x23, x24, [sp, #-16]!
	stp	x21, x22, [sp, #-16]!
	stp	x19, x20, [sp, #-16]!
	stp	x17, x18, [sp, #-16]!
	stp	x15, x16, [sp, #-16]!
	stp	x13, x14, [sp, #-16]!
	stp	x11, x12, [sp, #-16]!
	stp	x9, x10, [sp, #-16]!
	stp	x7, x8, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!
	stp	x3, x4, [sp, #-16]!
	stp	x1, x2, [sp, #-16]!

	/* Wait for FPU operation done */
 	mrs	x1, FPSR

	/*
	 * store floating point registers from (q30,q31) pair to (q0,q1) pair.
	 */
	stp q30, q31, [sp, #-32]!
	stp q28, q29, [sp, #-32]!
	stp q26, q27, [sp, #-32]!
	stp q24, q25, [sp, #-32]!
	stp q22, q23, [sp, #-32]!
	stp q20, q21, [sp, #-32]!
	stp q18, q19, [sp, #-32]!
	stp q16, q17, [sp, #-32]!
	stp q14, q15, [sp, #-32]!
	stp q12, q13, [sp, #-32]!
	stp q10, q11, [sp, #-32]!
	stp q8, q9, [sp, #-32]!
	stp q6, q7, [sp, #-32]!
	stp q4, q5, [sp, #-32]!
	stp q2, q3, [sp, #-32]!
	stp q0, q1, [sp, #-32]!

	/* Save VFP control state */
	mrs	x1, FPCR
	mrs x2, FPEXC32_EL2
	stp	x1, x2, [sp, #-16]!

	//mrs	x1, FPCR
	mrs x2, FPSR
	stp	x1, x2, [sp, #-16]!
.endm

.macro store_traped_sp
	mrs	x21, sp_el0
	str	x21, [sp, #EXC_EXC_SP_OFFSET]
.endm

.macro store_nested_sp
	mov	x21, sp
	add	x21, x21, #EXC_FRAME_SIZE
	str	x21, [sp, #EXC_EXC_SP_OFFSET]
.endm

.macro restore_traped_sp
	ldr	x21, [sp, #EXC_EXC_SP_OFFSET]
	msr	sp_el0, x21
.endm

.macro build_trapframe exc_type

	/*
	 * Store (spsr, x0)
	 */
	mrs	x21, SPSR_EL3
	stp	x21, x0, [sp, #-16]!
	/*
	 * Allocate a room for sp_el0 and store elr
	 */
	mrs	x21, ELR_EL3
	stp	xzr, x21, [sp, #-16]!
	/*
	 * store exception type and esr
	 */
	mov	x21, #(\exc_type)
	mrs	x22, ESR_EL3
	stp	x21, x22, [sp, #-16]!
.endm

.macro restore_trapframe

	/*
	 * Drop exception type, esr,
	 */
	add	sp, sp, #16
	/*
	 * Drop exception stack pointer and restore ELR_EL3
	 */
	ldp	x21, x22, [sp], #16
	msr	ELR_EL3, x22

	/*
	 * Retore spsr and x0
	 */
	ldp	x21, x0, [sp], #16
	msr	SPSR_EL3, x21
.endm

.macro pop_trapframe

	/* Wait for FPU operation done */
 	mrs	x1, FPSR
	/* Restire VFP control state */
	ldp	x1, x2, [sp], #16
	//msr	FPCR, x1
	msr FPSR, x2

	ldp	x1, x2, [sp], #16
	msr	FPCR, x1
	msr FPEXC32_EL2, x2

	/*
	 * Restore gstore floating point registers from (q0,q1) pair to (q30,q31) pair.
	 */
	ldp q0, q1, [sp], #32
	ldp q2, q3, [sp], #32
	ldp q4, q5, [sp], #32
	ldp q6, q7, [sp], #32
	ldp q8, q9, [sp], #32
	ldp q10, q11, [sp], #32
	ldp q12, q13, [sp], #32
	ldp q14, q15, [sp], #32
	ldp q16, q17, [sp], #32
	ldp q18, q19, [sp], #32
	ldp q20, q21, [sp], #32
	ldp q22, q23, [sp], #32
	ldp q24, q25, [sp], #32
	ldp q26, q27, [sp], #32
	ldp q28, q29, [sp], #32
	ldp q30, q31, [sp], #32

	/*
	 * Restore generic registers from (x29,x30) pair to (x1,x2) pair.
	 */
	ldp	x1, x2, [sp], #16
	ldp	x3, x4, [sp], #16
	ldp	x5, x6, [sp], #16
	ldp	x7, x8, [sp], #16
	ldp	x9, x10, [sp], #16
	ldp	x11, x12, [sp], #16
	ldp	x13, x14, [sp], #16
	ldp	x15, x16, [sp], #16
	ldp	x17, x18, [sp], #16
	ldp	x19, x20, [sp], #16
	ldp	x21, x22, [sp], #16
	ldp	x23, x24, [sp], #16
	ldp	x25, x26, [sp], #16
	ldp	x27, x28, [sp], #16
	ldp	x29, x30, [sp], #16

.endm

.macro call_common_trap_handler, codeN, tail=fname
	mov	x0, sp
	BL uncommon_trap_handler_\codeN
.endm

	text_align
_curr_el_sp0_sync:
	push_trapframe
	build_trapframe AARCH64_EXC_SYNC_SP0
	store_traped_sp
	call_common_trap_handler 1
	restore_traped_sp
	restore_trapframe
	pop_trapframe
	ERET

	text_align
_curr_el_sp0_irq:
	push_trapframe
	build_trapframe AARCH64_EXC_IRQ_SP0
	store_traped_sp
	call_common_trap_handler 2
	restore_traped_sp
	restore_trapframe
	pop_trapframe
	ERET

	text_align
_curr_el_sp0_fiq:
	push_trapframe
	build_trapframe AARCH64_EXC_FIQ_SP0
	store_traped_sp
	call_common_trap_handler 3
	restore_traped_sp
	restore_trapframe
	pop_trapframe
	ERET

	text_align
_curr_el_sp0_serror:
	push_trapframe
	build_trapframe AARCH64_EXC_SERR_SP0
	store_traped_sp
	call_common_trap_handler 4
	restore_traped_sp
	restore_trapframe
	pop_trapframe
	ERET

	text_align
_curr_el_spx_sync:
	push_trapframe
	build_trapframe AARCH64_EXC_SYNC_SPX
	store_nested_sp
	//call_common_trap_handler 5
	BL SError_Handler
	restore_trapframe
	pop_trapframe
	ERET

	text_align
_curr_el_spx_irq:
	push_trapframe
	build_trapframe AARCH64_EXC_IRQ_SPX
	store_nested_sp
	//call_common_trap_handler 6
	BL VIRQ_Handler		// Calls from EL3 system
	restore_trapframe
	pop_trapframe
	ERET

	text_align
_curr_el_spx_fiq:
	push_trapframe
	build_trapframe AARCH64_EXC_FIQ_SPX
	store_nested_sp
	call_common_trap_handler 7
	restore_trapframe
	pop_trapframe
	ERET

	text_align
_curr_el_spx_serror:
	push_trapframe
	build_trapframe AARCH64_EXC_SERR_SPX
	store_nested_sp
	call_common_trap_handler 8
	restore_trapframe
	pop_trapframe
	ERET


	text_align
_lower_el_aarch64_sync:
	push_trapframe
	build_trapframe AARCH64_EXC_SYNC_AARCH64
	store_traped_sp
	call_common_trap_handler 9
	restore_traped_sp
	restore_trapframe
	pop_trapframe
	ERET

	text_align
_lower_el_aarch64_irq:
	push_trapframe
	build_trapframe AARCH64_EXC_IRQ_AARCH64
	store_traped_sp
	call_common_trap_handler 10
	restore_traped_sp
	restore_trapframe
	pop_trapframe
	ERET

	text_align
_lower_el_aarch64_fiq:
	push_trapframe
	build_trapframe AARCH64_EXC_FIQ_AARCH64
	store_traped_sp
	call_common_trap_handler 11
	restore_traped_sp
	restore_trapframe
	pop_trapframe
	ERET

	text_align
_lower_el_aarch64_serror:
	push_trapframe
	build_trapframe AARCH64_EXC_SERR_AARCH64
	store_traped_sp
	call_common_trap_handler 12
	restore_traped_sp
	restore_trapframe
	pop_trapframe
	ERET


	text_align
_lower_el_aarch32_sync:
	push_trapframe
	build_trapframe AARCH64_EXC_SYNC_AARCH32
	store_traped_sp
	call_common_trap_handler 13
	restore_traped_sp
	restore_trapframe
	pop_trapframe
	ERET

	text_align
_lower_el_aarch32_irq:
	push_trapframe
	build_trapframe AARCH64_EXC_IRQ_AARCH32
	store_traped_sp
	call_common_trap_handler 14
	restore_traped_sp
	restore_trapframe
	pop_trapframe
	ERET

	text_align
_lower_el_aarch32_fiq:
	push_trapframe
	build_trapframe AARCH64_EXC_FIQ_AARCH32
	store_traped_sp
	call_common_trap_handler 15
	restore_traped_sp
	restore_trapframe
	pop_trapframe
	ERET

	text_align
_lower_el_aarch32_serror:
	push_trapframe
	build_trapframe AARCH64_EXC_SERR_AARCH32
	store_traped_sp
	call_common_trap_handler 16
	restore_traped_sp
	restore_trapframe
	pop_trapframe
	ERET



   	.ltorg

	.section ".noinit"
	.align 4

	.space	STACKSIZEUND
__stack_cpu0_und_end = .
	.space	STACKSIZEABT
__stack_cpu0_abt_end = .
	.space	STACKSIZEFIQ
__stack_cpu0_fiq_end = .
	.space	STACKSIZEMON
__stack_cpu0_mon_end = .
//	.space	STACKSIZEHYP
//__stack_cpu0_hyp_end = .
	.space	STACKSIZESVC
__stack_cpu0_svc_end = .

	.space	STACKSIZESYSBOOT
__stack_cpu0_sys_end = .

	.space	STACKSIZEIRQ
__stack_cpu0_irq_end = .

	.word 0		/* fix non-zero size of this section */

/*** EOF ***/   
