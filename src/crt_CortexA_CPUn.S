/* $Id$ */
/****************************************************************************
 * Copyright (c) 2024 by Genadi V. Zawidowski. All rights reserved.
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
   //CA53 not have ARM_MODE_HYP	  = 0x1A      /* Hypervisor Interrupts Mode            		  */
   ARM_MODE_UNDEF = 0x1B      /* Undefined Instructions Mode                  */
   ARM_MODE_SYS   = 0x1F      /* System Running in Priviledged Operating Mode */
   
	/* Standard definitions of mode bits and interrupt (I & F) flags in PSRs */
	I_BIT          = 0x80      /* disable IRQ when I bit is set */
	F_BIT          = 0x40      /* disable FIQ when F bit is set */
 	STACKSIZEALL = 512

	.section ".text"
	.code 32

	.align 4, 0
	.global aarch32_stack_top
aarch32_stack_top:
	.word 0

	.section ".text"
	.code 32
	.align 4, 0

  	.extern Reset_CPUn_Handler

  	.global Reset_CPUx_Handler
    .func   Reset_CPUx_Handler
    /* invoked at ARM_MODE_SVC */
Reset_CPUx_Handler:
	cpsid 	fi
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
	ldr		r2, = aarch32_stack_top
	ldr		r2, [r2]
	mov		r3, #STACKSIZEALL
	msr   CPSR_c, #ARM_MODE_UNDEF | I_BIT | F_BIT   /* 0x1b Undefined Instruction Mode */
	mov   sp, r2
	mov   lr, #0
	sub	r2, r2, r3

	msr   CPSR_c, #ARM_MODE_ABORT | I_BIT | F_BIT   /* 0x17 Abort Mode */
	mov   sp, r2
	mov   lr, #0
	sub	r2, r2, r3

	msr   CPSR_c, #ARM_MODE_FIQ | I_BIT | F_BIT     /* 0x11 FIQ Mode */
	mov   sp, r2
	mov   lr, #0
	sub	r2, r2, r3

	msr   CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT     /* 0x12 IRQ Mode */
	mov   sp, r2
	mov   lr, #0
	sub	r2, r2, r3

	msr   CPSR_c, #ARM_MODE_MON | I_BIT | F_BIT     /* 0x16 Monitor Mode */
	mov   sp, r2
	mov   lr, #0
	sub	r2, r2, r3

	msr   CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT     /* 0x13 Supervisor Mode */
	mov   sp, r2
	mov   lr, #0
	sub	r2, r2, r3

	msr   CPSR_c, #ARM_MODE_SYS | I_BIT | F_BIT     /* 0x1F Priviledged Operating Mode */
	mov   sp, r2
	mov   lr, #0

	ldr		r2, =Reset_CPUn_Handler
	mov		lr, pc
	bx		r2     /* And jump... */

1:
	wfi
	b     1b

		.endfunc
   .ltorg

/*** EOF ***/   
