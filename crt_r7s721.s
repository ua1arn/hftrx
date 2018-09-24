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
   ARM_MODE_SVC   = 0x13      /* Supervisor Interrupts Mode                   */
   ARM_MODE_ABORT = 0x17      /* Abort Processing memory Faults Mode          */
   ARM_MODE_UNDEF = 0x1B      /* Undefined Instructions Mode                  */
   ARM_MODE_SYS   = 0x1F      /* System Running in Priviledged Operating Mode */
   ARM_MODE_MASK  = 0x1F
   
/* Standard definitions of mode bits and interrupt (I & F) flags in PSRs */
   I_BIT          = 0x80      /* disable IRQ when I bit is set */
   F_BIT          = 0x40      /* disable FIQ when I bit is set */
 
	 STACKSIZE = 4096
  
	.global __Vectors
	.section .vectors,"ax"
	.code 32
        
/****************************************************************************/
/*               Vector table and reset entry                               */
/****************************************************************************/
__Vectors:
   ldr pc, ResetAddr    /* Reset                 */
   ldr pc, UndefAddr    /* Undefined instruction */
   ldr pc, SWIAddr      /* Software interrupt    */
   ldr pc, PAbortAddr   /* Prefetch abort        */
   ldr pc, DAbortAddr   /* Data abort            */
   ldr pc, ReservedAddr /* Reserved              */
   ldr pc, IRQAddr      /* IRQ interrupt         */
   ldr pc, FIQAddr      /* FIQ interrupt         */


ResetAddr:     .word DummyResetHandler
UndefAddr:     .word UndefHandler
SWIAddr:       .word SWIHandler
PAbortAddr:    .word PAbortHandler
DAbortAddr:    .word DAbortHandler
ReservedAddr:  .word 0
IRQAddr:       .word IRQHandler
FIQAddr:       .word FIQHandler

   .ltorg


   .section .init, "ax"
   .code 32
   
   .global Reset_Handler
   .extern main
   .extern __libc_init_array
   .extern arm_cpu_initialize
/****************************************************************************/
/*                           Reset handler                                  */
/****************************************************************************/
Reset_Handler:

  /*
    * Setup a stack for each mode
    */    
   msr   CPSR_c, #ARM_MODE_UNDEF   /* Undefined Instruction Mode */     
   ldr   sp, =__stack_und_end
   
   msr   CPSR_c, #ARM_MODE_ABORT   /* Abort Mode */
   ldr   sp, =__stack_abt_end
   
   msr   CPSR_c, #ARM_MODE_FIQ     /* FIQ Mode */   
   ldr   sp, =__stack_fiq_end
   
   msr   CPSR_c, #ARM_MODE_IRQ     /* IRQ Mode */   
   ldr   sp, =__stack_irq_end
   
   msr   CPSR_c, #ARM_MODE_SVC     /* Supervisor Mode */
   ldr   sp, =__stack	/* __stack_svc_end */

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
	
   ldr   r2, =arm_cpu_initialize
   mov   lr, pc
   bx    r2     /* And jump... */

   /* copy initialized variables .data section  (Copy from ROM to RAM) */
   ldr     r1, =__etext
   ldr     r2, =__data_start__
   ldr     r3, =__data_end__
data_copy_loop:
   cmp     r2, r3
   ldrlo   r0, [r1], #4
   strlo   r0, [r2], #4
   blo     data_copy_loop

   /*
    * Clear .bss section
    */
   ldr   r1, =__bss_start__
   ldr   r2, =__bss_end__
   ldr   r3, =0
bss_clear_loop:
   cmp   r1, r2
   strne r3, [r1], #+4
   bne   bss_clear_loop
   
   /*
    * Jump to main
    */
   //mrs   r0, cpsr
   //bic   r0, r0, #I_BIT | F_BIT     /* Enable FIQ and IRQ interrupt */
   //msr   cpsr, r0
   
   ldr   r2, =__libc_init_array
   mov   lr, pc
   bx    r2     /* And jump... */


   mov   r0, #0 /* No arguments */
   mov   r1, #0 /* No arguments */
   ldr   r2, =main
   mov   lr, pc
   bx    r2     /* And jump... */
                       
ExitFunction:
   nop
   nop
   nop
   b ExitFunction   

DRCR0_addr:		.word 0x3FEFA00C

	.ascii " DREAM RX project " __DATE__ " " __TIME__ " "
	.align 8

/****************************************************************************/
/*                         Default interrupt handler                        */
/****************************************************************************/
   .section .text
   .code 32

	.align 4
DummyResetHandler:
   b DummyResetHandler

#if 0
UndefHandler:
   b UndefHandler
   
SWIHandler:
   b SWIHandler

PAbortHandler:
   b PAbortHandler

DAbortHandler:
   b DAbortHandler
   
FIQHandler:
   b FIQHandler

//IRQHandler:
//   b IRQHandler

#endif

/* ================================================================== */
/* Entry point for the IRQ handler */
/* ================================================================== */

    .func   IRQHandler
IRQHandler:

		/* Save interrupt context on the stack to allow nesting */
		sub		lr, lr, #4
		stmfd   sp!, {lr}
		mrs     lr, SPSR
		stmfd   sp!, {r0, lr}

        msr     CPSR_c, #ARM_MODE_SVC | I_BIT
		stmfd   sp!, {r1-r3, r4, r12, lr}

#if __ARM_NEON == 1
		// save Neon data registers
		VPUSH.F64	{q8-q15}
#endif /* __ARM_NEON == 1 */
		// save VFP/Neon data registers
		VPUSH.F64	{q0-q7}
		// save VFP/Neon FPSCR register
		FMRX	r2, FPSCR
		PUSH	{r2}
		// save VFP/Neon FPEXC register
		FMRX	r2, FPEXC
		PUSH	{r2}

		ldr		r2, =IRQHandlerSafe
		mov		lr, pc
		bx		r2     /* And jump... */

		// restore VFP/Neon FPEXC register
		POP		{r2}
		FMXR	FPEXC, r2
		// restore VFP/Neon FPSCR register
		POP		{r2}
		FMXR	FPSCR, r2
		// restore VFP data registers
		VPOP.F64   {q0-q7}
#if __ARM_NEON == 1
		// restore VFP/Neon data registers
		VPOP.F64	{q8-q15}
#endif /* __ARM_NEON == 1 */

		ldmia   sp!, {r1-r3, r4, r12, lr}
        msr     CPSR_c, #ARM_MODE_IRQ | I_BIT

		ldmia   sp!, {r0, lr}
		msr     SPSR_cxsf, lr
		ldmia   sp!, {pc}^
		.endfunc

	.bss
	.align 8
	.space	STACKSIZE
__stack_und_end = .
	.space	STACKSIZE
__stack_abt_end = .
	.space	STACKSIZE
__stack_fiq_end = .
	.space	STACKSIZE
__stack_irq_end = .

   .ltorg
/*** EOF ***/   
  

