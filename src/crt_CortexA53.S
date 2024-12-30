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
 
	 STACKSIZEUND = 256
	 STACKSIZEABT = 256
	 STACKSIZEFIQ = 256
	 STACKSIZEHYP = 256
	 STACKSIZEMON = 256
	 STACKSIZESVC = 256
	 STACKSIZEIRQ = 256

	 STACKSIZESYSBOOT = 4096

	.section ".isr_vector"
	//.code 64
        
/****************************************************************************/
/*               Vector table and reset entry                               */
/* Table B1-3 The vector tables */
/****************************************************************************/
	// DDI0500J_cortex_a53_r0p4_trm.pdf 4.3.74 Vector Base Address Register, EL3
	// ARMv8-A-Programmer-Guide.pdf 10.4 AArch64 exception table
	// Bits 10..0 of address should be zero
	.global __Vectors64
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

   	.section ".startup0"
	.align 3
   	//.code 64
   
   	.extern _start
   	/*.extern __libc_init_array*/
   	.extern SystemInit
   	.global Reset_Handler
 /****************************************************************************/
/*                           Reset handler                                  */
/****************************************************************************/
Reset_Handler:
	mov	x0, XZR
	mov x1, x0
	mov x2, x0
	mov x3, x0
	mov x4, x0
	mov x5, x0
	mov x6, x0
	mov x7, x0
	mov x8, x0
	mov x9, x0
	mov x10, x0
	mov x11, x0
	mov x12, x0
	mov x13, x0
	mov x14, x0
	mov x15, x0
	mov x16, x0
	mov x17, x0
	mov x18, x0
	mov x19, x0
	mov x20, x0
	mov x21, x0
	mov x22, x0
	mov x23, x0
	mov x24, x0
	mov x25, x0
	mov x26, x0
	mov x27, x0
	mov x28, x0
	mov x29, x0
	mov x30, x0

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
	bl SystemInit
	ldr x30, =__stack
	mov sp, x30
	mov x30, XZR
	bl __riscv_start
1:
	b 1b
	//call
#if 000
//	b	skipMarker
//	.ascii "Xmonitor"
//	.align 8, 0
//skipMarker:

#if 0
	ldr	r0, =0x05000000
	ldr r1, =0x23
	str	r1, [r0]
rrrr:
	b rrrr
#endif

	/* Mask interrupts */
    mov   	lr, #0
	cpsid   if
	mrc     p15, 0, r0, c0, c0, 5      /* Read MPIDR */
	ands    r0, r0, #3
gotosleep:
	wfine
	bne     gotosleep

   /*
    * Reset SCTLR Settings
    */
	MRC     p15, 0, R0, c1, c0, 0    // Read CP15 System Control register
	BIC     R0, R0, #(0x1 << 12)     // Clear I bit 12 to disable I Cache
	BIC     R0, R0, #(0x1 <<  2)     // Clear C bit  2 to disable D Cache
	BIC     R0, R0, #0x1             // Clear M bit  0 to disable MMU
	BIC     R0, R0, #(0x1 << 11)     // Clear Z bit 11 to disable branch prediction
	BIC     R0, R0, #(0x1 << 13)     // Clear V bit 13 to disable hivecs
	MCR     p15, 0, R0, c1, c0, 0    // Write value back to CP15 System Control register
	ISB

  /*
    * Setup a stack for each mode
    */    
   msr   CPSR_c, #ARM_MODE_UNDEF | I_BIT | F_BIT   /* 0x1b Undefined Instruction Mode */
   ldr   sp, =__stack_cpu0_und_end
   mov   lr, #0
   
   msr   CPSR_c, #ARM_MODE_ABORT | I_BIT | F_BIT   /* 0x17 Abort Mode */
   ldr   sp, =__stack_cpu0_abt_end
   mov   lr, #0
   
   msr   CPSR_c, #ARM_MODE_FIQ | I_BIT | F_BIT     /* 0x11 FIQ Mode */
   ldr   sp, =__stack_cpu0_fiq_end
   mov   lr, #0
   
   msr   CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT     /* 0x12 IRQ Mode */
   ldr   sp, =__stack_cpu0_irq_end
   mov   lr, #0

   msr   CPSR_c, #ARM_MODE_MON | I_BIT | F_BIT     /* 0x16 Monitor Mode */
   ldr   sp, =__stack_cpu0_mon_end
   mov   lr, #0

//   msr   CPSR_c, #ARM_MODE_HYP | I_BIT | F_BIT     /* 0x1B Hypervisor Mode */
//   ldr   sp, =__stack_cpu0_hyp_end
//  mov   lr, #0

   msr   CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT     /* 0x13 Supervisor Mode */
   ldr   sp, = __stack_cpu0_svc_end
   mov   lr, #0

   msr   CPSR_c, #ARM_MODE_SYS | I_BIT | F_BIT     /* 0x1F Priviledged Operating Mode */
   ldr   sp, =__stack_cpu0_sys_end
   mov   lr, #0

	/* low-level CPU peripherials init */

	ldr   r2, =SystemInit
	mov   lr, pc
	bx    r2     /* And jump... */

	ldr   sp, =__stack	/* New stack may be placed in DDR RAM */

/*  Firstly it copies data from read only memory to RAM. There are two schemes
 *  to copy. One can copy more than one sections. Another can only copy
 *  one section.  The former scheme needs more instructions and read-only
 *  data to implement than the latter.
 *  Macro __STARTUP_COPY_MULTIPLE is used to choose between two schemes.  */

/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of triplets, each of which specify:
 *    offset 0: LMA of start of a section to copy from
 *    offset 4: VMA of start of a section to copy to
 *    offset 8: size of the section to copy. Must be multiply of 4
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
	ldr	r4, =__copy_table_start__
	ldr	r5, =__copy_table_end__

.L_loop0:
	cmp	r4, r5
	bge	.L_loop0_done
	ldr	r1, [r4]
	ldr	r2, [r4, #4]
	ldr	r3, [r4, #8]

.L_loop0_0:
	subs	r3, #4
	ittt	ge
	ldrge	r0, [r1, r3]
	strge	r0, [r2, r3]
	bge	.L_loop0_0

	adds	r4, #12
	b	.L_loop0

.L_loop0_done:

/*  This part of work usually is done in C library startup code. Otherwise,
 *  define this macro to enable it in this startup.
 *
 *  There are two schemes too. One can clear multiple BSS sections. Another
 *  can only clear one section. The former is more size expensive than the
 *  latter.
 *
 *  Define macro __STARTUP_CLEAR_BSS_MULTIPLE to choose the former.
 *  Otherwise efine macro __STARTUP_CLEAR_BSS to choose the later.
 */
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of tuples specifying:
 *    offset 0: Start of a BSS section
 *    offset 4: Size of this BSS section. Must be multiply of 4
 */
	ldr	r3, =__zero_table_start__
	ldr	r4, =__zero_table_end__

.L_loop2:
	cmp	r3, r4
	bge	.L_loop2_done
	ldr	r1, [r3]
	ldr	r2, [r3, #4]
	ldr r0, =0
	//movs	r0, 0

.L_loop2_0:
	subs	r2, #4
	itt	ge
	strge	r0, [r1, r2]
	bge	.L_loop2_0

	adds	r3, #8
	b	.L_loop2
.L_loop2_done:

	ldr   r2, =_start
	mov   lr, pc
	bx    r2     /* And jump... */
                       
ExitFunction:
   nop
   nop
   nop
   b ExitFunction   

#endif

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
    .func   VIRQ_Handler
VIRQ_Handler:

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

.macro build_trapframe exc_type
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
	/*
	 * Store (spsr, x0)
	 */
	mrs	x21, spsr_el1
	stp	x21, x0, [sp, #-16]!
	/*
	 * Allocate a room for sp_el0 and store elr
	 */
	mrs	x21, elr_el1
	stp	xzr, x21, [sp, #-16]!
	/*
	 * store exception type and esr
	 */
	mov	x21, #(\exc_type)
	mrs	x22, esr_el1
	stp	x21, x22, [sp, #-16]!
.endm

.macro store_traped_sp
	mrs	x21, sp_el0
	str	x21, [sp, #EXC_EXC_SP_OFFSET]
.endm

.macro call_common_trap_handler, codeN, tail=fname
	mov	x0, sp
	bl uncommon_trap_handler_\codeN
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

.macro restore_trapframe

	/*
	 * Drop exception type, esr,
	 */
	add	sp, sp, #16
	/*
	 * Drop exception stack pointer and restore elr_el1
	 */
	ldp	x21, x22, [sp], #16
	msr	elr_el1, x22

	/*
	 * Retore spsr and x0
	 */
	ldp	x21, x0, [sp], #16
	msr	spsr_el1, x21

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

	eret
.endm

	text_align
_curr_el_sp0_sync:
	build_trapframe AARCH64_EXC_SYNC_SP0
	store_traped_sp
	call_common_trap_handler 1
	restore_traped_sp
	restore_trapframe

	text_align
_curr_el_sp0_irq:
	build_trapframe AARCH64_EXC_IRQ_SP0
	store_traped_sp
	call_common_trap_handler 2
	restore_traped_sp
	restore_trapframe

	text_align
_curr_el_sp0_fiq:
	build_trapframe AARCH64_EXC_FIQ_SP0
	store_traped_sp
	call_common_trap_handler 3
	restore_traped_sp
	restore_trapframe

	text_align
_curr_el_sp0_serror:
	build_trapframe AARCH64_EXC_SERR_SP0
	store_traped_sp
	call_common_trap_handler 4
	restore_traped_sp
	restore_trapframe

	text_align
_curr_el_spx_sync:
	build_trapframe AARCH64_EXC_SYNC_SPX
	store_nested_sp
	//call_common_trap_handler 5
	bl SError_Handler
	restore_trapframe

	text_align
_curr_el_spx_irq:
	build_trapframe AARCH64_EXC_IRQ_SPX
	store_nested_sp
	//call_common_trap_handler 6
	bl VIRQ_Handler		// Calls from EL3 system
	restore_trapframe

	text_align
_curr_el_spx_fiq:
	build_trapframe AARCH64_EXC_FIQ_SPX
	store_nested_sp
	call_common_trap_handler 7
	restore_trapframe

	text_align
_curr_el_spx_serror:
	build_trapframe AARCH64_EXC_SERR_SPX
	store_nested_sp
	call_common_trap_handler 8
	restore_trapframe


	text_align
_lower_el_aarch64_sync:
	build_trapframe AARCH64_EXC_SYNC_AARCH64
	store_traped_sp
	call_common_trap_handler 9
	restore_traped_sp
	restore_trapframe

	text_align
_lower_el_aarch64_irq:
	build_trapframe AARCH64_EXC_IRQ_AARCH64
	store_traped_sp
	call_common_trap_handler 10
	restore_traped_sp
	restore_trapframe

	text_align
_lower_el_aarch64_fiq:
	build_trapframe AARCH64_EXC_FIQ_AARCH64
	store_traped_sp
	call_common_trap_handler 11
	restore_traped_sp
	restore_trapframe

	text_align
_lower_el_aarch64_serror:
	build_trapframe AARCH64_EXC_SERR_AARCH64
	store_traped_sp
	call_common_trap_handler 12
	restore_traped_sp
	restore_trapframe


	text_align
_lower_el_aarch32_sync:
	build_trapframe AARCH64_EXC_SYNC_AARCH32
	store_traped_sp
	call_common_trap_handler 13
	restore_traped_sp
	restore_trapframe

	text_align
_lower_el_aarch32_irq:
	build_trapframe AARCH64_EXC_IRQ_AARCH32
	store_traped_sp
	call_common_trap_handler 14
	restore_traped_sp
	restore_trapframe

	text_align
_lower_el_aarch32_fiq:
	build_trapframe AARCH64_EXC_FIQ_AARCH32
	store_traped_sp
	call_common_trap_handler 15
	restore_traped_sp
	restore_trapframe

	text_align
_lower_el_aarch32_serror:
	build_trapframe AARCH64_EXC_SERR_AARCH32
	store_traped_sp
	call_common_trap_handler 16
	restore_traped_sp
	restore_trapframe




   	.ltorg

	.section ".noinit"
	.align 3

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
