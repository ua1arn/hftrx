/* $Id$ */
/****************************************************************************
*  Copyright (c) 2006 by Michael Fischer. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*  
*  1. Redistributions of source code must retain the above copyright 
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the 
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of its contributors may 
*     be used to endorse or promote products derived from this software 
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
*  SUCH DAMAGE.
*
****************************************************************************
*
*  History:
*
*  18.12.06  mifi   First Version
*                   The hardware initialization is based on the startup file
*                   crtat91sam7x256_rom.S from NutOS 4.2.1. 
*                   Therefore partial copyright by egnite Software GmbH.
****************************************************************************/

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
 
	 STACKSIZEUND = 64
	 STACKSIZEABT = 64
	 STACKSIZEFIQ = 64
	 STACKSIZEHYP = 64
	 STACKSIZEMON = 64
	 STACKSIZESVC = 64
	 STACKSIZEIRQ = 64

	 STACKSIZESYSBOOT = 512
  
	.section .isr_vector, "ax"
	.code 32
        
/****************************************************************************/
/*               Vector table and reset entry                               */
/* Table B1-3 The vector tables */
/****************************************************************************/
	.align 6
	.global __Vectors
__Vectors:
	ldr pc, ResetAddr    /* Reset                 */
	ldr pc, UndefAddr    /* Undefined instruction */
	ldr pc, SWIAddr      /* Software interrupt    */
	ldr pc, PAbortAddr   /* Prefetch abort        */
	ldr pc, DAbortAddr   /* Data abort            */
	ldr pc, HypAddr 		/* Entry to Hyp mode	 */
	ldr pc, IRQAddr      /* IRQ interrupt         */
	ldr pc, FIQAddr      /* FIQ interrupt         */


ResetAddr:     .word Reset_Handler
UndefAddr:     .word Undef_Handler
SWIAddr:       .word SWI_Handler
PAbortAddr:    .word PAbort_Handler
DAbortAddr:    .word DAbort_Handler
HypAddr:  	   .word Hyp_Handler
IRQAddr:       .word IRQ_Handler
FIQAddr:       .word FIQ_Handler
	.ltorg

	.align 6
	.global __Vectors_nested
__Vectors_nested:
	ldr pc, ResetAddr_n    /* Reset                 */
	ldr pc, UndefAddr_n    /* Undefined instruction */
	ldr pc, SWIAddr_n      /* Software interrupt    */
	ldr pc, PAbortAddr_n   /* Prefetch abort        */
	ldr pc, DAbortAddr_n   /* Data abort            */
	ldr pc, HypAddr_n 		/* Entry to Hyp mode	 */
	ldr pc, IRQAddr_n      /* IRQ interrupt         */
	ldr pc, FIQAddr_n      /* FIQ interrupt         */


ResetAddr_n:     .word Reset_Handler
UndefAddr_n:     .word Undef_Handler
SWIAddr_n:       .word SWI_Handler
PAbortAddr_n:    .word PAbort_Handler
DAbortAddr_n:    .word DAbort_Handler
HypAddr_n:  	 .word Hyp_Handler
IRQAddr_n:       .word IRQ_Handler_n
FIQAddr_n:       .word FIQ_Handler
   .ltorg


	.align 6
	.global __Vectors_rtos
__Vectors_rtos:
	ldr pc, ResetAddr_rtos    /* Reset                 */
	ldr pc, UndefAddr_rtos    /* Undefined instruction */
	ldr pc, SWIAddr_rtos      /* Software interrupt    */
	ldr pc, PAbortAddr_rtos   /* Prefetch abort        */
	ldr pc, DAbortAddr_rtos   /* Data abort            */
	ldr pc, HypAddr_rtos 		/* Entry to Hyp mode	 */
	ldr pc, IRQAddr_rtos      /* IRQ interrupt         */
	ldr pc, FIQAddr_rtos      /* FIQ interrupt         */


ResetAddr_rtos:     .word Reset_Handler
UndefAddr_rtos:     .word Undef_Handler
SWIAddr_rtos:       .word FreeRTOS_SWI_Handler
PAbortAddr_rtos:    .word PAbort_Handler
DAbortAddr_rtos:    .word DAbort_Handler
HypAddr_rtos:  	 .word Hyp_Handler
IRQAddr_rtos:       .word FreeRTOS_IRQ_Handler
FIQAddr_rtos:       .word FIQ_Handler
   .ltorg


   .section .startup0, "ax"
   .code 32
   
   .extern _start
   /*.extern __libc_init_array*/
   .extern SystemInit
   .global Reset_Handler
 /****************************************************************************/
/*                           Reset handler                                  */
/****************************************************************************/
Reset_Handler:

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

   .ltorg

	.align 4, 0
	.ascii " DREAM RX project " __DATE__ " " __TIME__ " "
	.align 4, 0

/****************************************************************************/
/*                         Default interrupt handler                        */
/****************************************************************************/
	.section .text
   .code 32
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
IRQ_Handler_n:
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
   	.ltorg

	.section .noinit
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
