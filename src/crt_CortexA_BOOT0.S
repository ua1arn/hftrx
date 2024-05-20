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

   .section .startup0
   .code 32
   
   .extern _start
   /*.extern __libc_init_array*/
   .extern SystemInit_BOOT0
   .global Reset_Handler
 /****************************************************************************/
/*                           Reset handler                                  */
/****************************************************************************/
Reset_Handler:

	/* Mask interrupts */
    mov   	lr, #0
	cpsid   if
	mrc     p15, 0, r0, c0, c0, 5      /* Read MPIDR */
	ands    r0, r0, #3
gotosleep:
	wfine
	bne     gotosleep
#if 0
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
#endif

#if 0

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

#endif

   msr   CPSR_c, #ARM_MODE_SYS | I_BIT | F_BIT     /* 0x1F Priviledged Operating Mode */
   ldr   sp, =__stack_cpu0_sys_end
   mov   lr, #0

	/* low-level CPU peripherials init */

	ldr   r2, =SystemInit_BOOT0
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
   b ExitFunction   

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
	.global __stack_cpu0_sys_end
__stack_cpu0_sys_end = .

	.space	STACKSIZEIRQ
__stack_cpu0_irq_end = .

	.word 0		/* fix non-zero size of this section */


/*** EOF ***/   
