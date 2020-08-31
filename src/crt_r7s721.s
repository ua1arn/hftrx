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
   ARM_MODE_HYP	  = 0x1a      /* Hypervisor Interrupts Mode            		  */
   ARM_MODE_UNDEF = 0x1B      /* Undefined Instructions Mode                  */
   ARM_MODE_SYS   = 0x1F      /* System Running in Priviledged Operating Mode */
   ARM_MODE_MASK  = 0x1F
   
	/* Standard definitions of mode bits and interrupt (I & F) flags in PSRs */
	I_BIT          = 0x80      /* disable IRQ when I bit is set */
	F_BIT          = 0x40      /* disable FIQ when F bit is set */
 
	 STACKSIZEUND = 256
	 STACKSIZEABT = 256
	 STACKSIZEFIQ = 256
	 STACKSIZEIRQ = 256
	 STACKSIZESVC = 256
	 STACKSIZEHYP = 256
	 STACKSIZEMON = 256
	 STACKSIZESYS = 256

	.global __Vectors
	.section .vectors,"ax"
	.code 32
        
/****************************************************************************/
/*               Vector table and reset entry                               */
/* Table B1-3 The vector tables */
/****************************************************************************/
__Vectors:
   ldr pc, ResetAddr    /* Reset                 */
   ldr pc, UndefAddr    /* Undefined instruction */
   ldr pc, SWIAddr      /* Software interrupt    */
   ldr pc, PAbortAddr   /* Prefetch abort        */
   ldr pc, DAbortAddr   /* Data abort            */
   ldr pc, HypAddr 		/* Entry to Hyp mode	 */
   ldr pc, IRQAddr      /* IRQ interrupt         */
   ldr pc, FIQAddr      /* FIQ interrupt         */


ResetAddr:     .word Reset_Handler7
UndefAddr:     .word Undef_Handler
SWIAddr:       .word SWI_Handler
PAbortAddr:    .word PAbort_Handler
DAbortAddr:    .word DAbort_Handler
HypAddr:  	   .word Hyp_Handler
IRQAddr:       .word IRQHandlerNested
FIQAddr:       .word FIQ_Handler

   .ltorg


   .section .init, "ax"
   .code 32
   
   .extern _start
   /*.extern __libc_init_array*/
   .extern SystemInit
   .global Reset_Handler7
/****************************************************************************/
/*                           Reset handler                                  */
/****************************************************************************/
Reset_Handler7:

	/* Mask interrupts */
    mov   	lr, #0
	cpsid   if
	mrc     p15, 0, r0, c0, c0, 5      /* Read MPIDR */
	ands    r0, r0, #3
gotosleep:
	wfine
	bne     gotosleep

  /*
    * Setup a stack for each mode
    */    
   msr   CPSR_c, #ARM_MODE_UNDEF | I_BIT | F_BIT   /* 0x1b Undefined Instruction Mode */
   ldr   sp, =__stack_und_end
   mov   lr, #0
   
   msr   CPSR_c, #ARM_MODE_ABORT | I_BIT | F_BIT   /* 0x17 Abort Mode */
   ldr   sp, =__stack_abt_end
   mov   lr, #0
   
   msr   CPSR_c, #ARM_MODE_FIQ | I_BIT | F_BIT     /* 0x11 FIQ Mode */
   ldr   sp, =__stack_fiq_end
   mov   lr, #0
   
   msr   CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT     /* 0x12 IRQ Mode */
   ldr   sp, =__stack_irq_end
   mov   lr, #0

   msr   CPSR_c, #ARM_MODE_MON | I_BIT | F_BIT     /* 0x16 Monitor Mode */
   ldr   sp, =__stack_mon_end
   mov   lr, #0

   msr   CPSR_c, #ARM_MODE_HYP | I_BIT | F_BIT     /* 0x1B Hypervisor Mode */
   ldr   sp, =__stack_hyp_end
   mov   lr, #0

   msr   CPSR_c, #ARM_MODE_SYS | I_BIT | F_BIT     /* 0x1F Priviledged Operating Mode */
   ldr   sp, =__stack_sys_end
   mov   lr, #0

   msr   CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT     /* 0x13 Supervisor Mode */
   ldr   sp, =__stack	/* __stack_svc_end */
   mov   lr, #0

#if 0
	/* Clean Data Cache  */
    /* code is specific to Cortex-A5 processors with 32KB caches */
    MOV R0, #(0 << 5)
way_loop:
    MOV R1, #(0 << 30)
line_loop:
    ORR R2,R1,R0
    MCR p15,0,R2,c7,c10,2
    ADD R1,R1,#(1 << 30)
    CMP R1,#0
    BNE line_loop
    ADD R0,R0,#(1 << 5)
    CMP R0,#(1 << 13)
    BNE way_loop
#endif

/* low-level CPU peripherials init */
	
   ldr   r2, =SystemInit
   mov   lr, pc
   bx    r2     /* And jump... */

/*  Firstly it copies data from read only memory to RAM. There are two schemes
 *  to copy. One can copy more than one sections. Another can only copy
 *  one section.  The former scheme needs more instructions and read-only
 *  data to implement than the latter.
 *  Macro __STARTUP_COPY_MULTIPLE is used to choose between two schemes.  */

#if 1 //def __STARTUP_COPY_MULTIPLE
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
#else
/*  Single section scheme.
 *
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
	ldr	r1, =__etext
	ldr	r2, =__data_start__
	ldr	r3, =__data_end__

.L_loop1:
	cmp	r2, r3
	ittt	lt
	ldrlt	r0, [r1], #4
	strlt	r0, [r2], #4
	blt	.L_loop1
#endif /*__STARTUP_COPY_MULTIPLE */

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
#if 1 //def __STARTUP_CLEAR_BSS_MULTIPLE
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
#elif defined (__STARTUP_CLEAR_BSS)
/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
	ldr	r1, =__bss_start__
	ldr	r2, =__bss_end__
	ldr r0, =0
	//movs	r0, 0
.L_loop3:
	cmp	r1, r2
	itt	lt
	strlt	r0, [r1], #4
	blt	.L_loop3
#endif /* __STARTUP_CLEAR_BSS_MULTIPLE || __STARTUP_CLEAR_BSS */

#ifndef __NO_SYSTEM_INIT
	/* bl	SystemInit */
#endif

#ifndef __START
#define __START _start
#endif
	bl	__START
                       
ExitFunction:
   nop
   nop
   nop
   b ExitFunction   

	.align 4, 0
	.ascii " DREAM RX project " __DATE__ " " __TIME__ " "
	.align 4, 0

/****************************************************************************/
/*                         Default interrupt handler                        */
/****************************************************************************/
   .section .text
   .code 32

	.align 4, 0

/* ================================================================== */
/* Entry point for the IRQ handler */
/* ================================================================== */

    .func   IRQHandlerNested
IRQHandlerNested:

       PUSH    {R0,LR}          // save register context
       MRS     LR, SPSR                // Copy SPSR_irq to LR
       PUSH    {R0,LR}                    // Save SPSR_irq
       MSR     CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT         // Disable IRQ (Svc Mode)
       PUSH    {r1-r3, r4, r12, lr}                    // Save LR

		// save VFP/Neon FPSCR register
		// save VFP/Neon FPEXC register
		FMRX	R0, FPSCR
		FMRX	LR, FPEXC
		PUSH	{R0, LR}

#if __ARM_NEON == 1
		// save Neon data registers
		VPUSH.F64	{q8-q15}
#endif /* __ARM_NEON == 1 */
		// save VFP/Neon data registers
		VPUSH.F64	{q0-q7}

		ldr		r0, =IRQ_Handler
		mov		lr, pc
		bx		r0     /* And jump... */
		// restore VFP data registers
		VPOP.F64   {q0-q7}
#if __ARM_NEON == 1
		// restore VFP/Neon data registers
		VPOP.F64	{q8-q15}
#endif /* __ARM_NEON == 1 */

		// restore VFP/Neon FPEXC register
		// restore VFP/Neon FPSCR register
		POP		{R0, LR}
		FMXR	FPEXC, LR
		FMXR	FPSCR, R0

       POP     {r1-r3, r4, r12, lr}                    // Restore LR
       MSR     CPSR_c, #ARM_MODE_IRQ  | I_BIT  // Disable IRQ (IRQ Mode)
       POP     {R0,LR}                    // Restore SPSR_irq to LR
       MSR     SPSR_cxsf, LR           // Copy LR to SPSR_irq

       POP     {R0,LR}          // restore register context
       SUBS    R15,R14,#0x0004         // return from interrupt
		.endfunc

	.bss
	.align 8
	.space	STACKSIZEUND
__stack_und_end = .
	.space	STACKSIZEABT
__stack_abt_end = .
	.space	STACKSIZEFIQ
__stack_fiq_end = .
	.space	STACKSIZEIRQ
__stack_irq_end = .
	.space	STACKSIZESVC
__stack_svc_end = .
	.space	STACKSIZEMON
__stack_mon_end = .
	.space	STACKSIZEHYP
__stack_hyp_end = .
	.space	STACKSIZESYS
__stack_sys_end = .

   .ltorg
/*** EOF ***/   
  

