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
 
	 STACKSIZEUND = 4096
	 STACKSIZEABT = 4096
	 STACKSIZEFIQ = 4096
	 STACKSIZEHYP = 4096
	 STACKSIZEMON = 4096
	 STACKSIZESVC = 4096
	 STACKSIZEIRQ = 4096

	 STACKSIZESYSCPUn = 16 * 1024 * 1024	/* 1MB */

	.section ".text"

	.align 4, 0
	.global aarch64_reset_handlers
aarch64_reset_handlers:
	.dword 0
	.dword Reset_CPU1_Handler
	.dword Reset_CPU2_Handler
	.dword Reset_CPU3_Handler
	.dword 0
	.dword 0
	.dword 0
	.dword 0

	.section ".text"
	.align 4, 0

  	.extern Reset_CPUn_Handler

  	.global Reset_CPU1_Handler
    .func   Reset_CPU1_Handler
    /* invoked at ARM_MODE_SVC */
Reset_CPU1_Handler:
	ldr x30, =__stack_cpu1_fiq_end
	MSR SPSR_fiq, x30
	ldr x30, =__stack_cpu1_und_end
	MSR SPSR_und, x30
	ldr x30, =__stack_cpu1_abt_end
	MSR SPSR_abt, x30
	ldr x30, =__stack_cpu1_irq_end
	MSR SPSR_irq, x30
	ldr x30, =__stack_cpu1_sys_end
	mov sp, x30
	mov x30, XZR
	B Reset_CPUn_Handler

	.endfunc

   .ltorg

	.section ".noinit"
	.align 4

	.space	STACKSIZEIRQ
__stack_cpu1_irq_end = .
	.space	STACKSIZEUND
__stack_cpu1_und_end = .
	.space	STACKSIZEABT
__stack_cpu1_abt_end = .
	.space	STACKSIZEFIQ
__stack_cpu1_fiq_end = .
	.space	STACKSIZEMON
__stack_cpu1_mon_end = .
//	.space	STACKSIZEHYP
//__stack_cpu1_hyp_end = .
	.space	STACKSIZESVC
__stack_cpu1_svc_end = .
	.space	STACKSIZESYSCPUn
__stack_cpu1_sys_end = .

	.word 0		/* fix non-zero size of this section */

	.section ".text"

  	.global Reset_CPU2_Handler
    .func   Reset_CPU2_Handler
    /* invoked at ARM_MODE_SVC */
Reset_CPU2_Handler:
	ldr x30, =__stack_cpu2_fiq_end
	MSR SPSR_fiq, x30
	ldr x30, =__stack_cpu2_und_end
	MSR SPSR_und, x30
	ldr x30, =__stack_cpu2_abt_end
	MSR SPSR_abt, x30
	ldr x30, =__stack_cpu2_irq_end
	MSR SPSR_irq, x30
	ldr x30, =__stack_cpu2_sys_end
	mov sp, x30
	mov x30, XZR
	B Reset_CPUn_Handler

	.endfunc

   .ltorg

	.section ".noinit"
	.align 4

	.space	STACKSIZEIRQ
__stack_cpu2_irq_end = .
	.space	STACKSIZEUND
__stack_cpu2_und_end = .
	.space	STACKSIZEABT
__stack_cpu2_abt_end = .
	.space	STACKSIZEFIQ
__stack_cpu2_fiq_end = .
	.space	STACKSIZEMON
__stack_cpu2_mon_end = .
//	.space	STACKSIZEHYP
//__stack_cpu2_hyp_end = .
	.space	STACKSIZESVC
__stack_cpu2_svc_end = .
	.space	STACKSIZESYSCPUn
__stack_cpu2_sys_end = .

	.word 0		/* fix non-zero size of this section */

	.section ".text"

  	.global Reset_CPU3_Handler
    .func   Reset_CPU3_Handler
    /* invoked at ARM_MODE_SVC */
Reset_CPU3_Handler:
	ldr x30, =__stack_cpu3_fiq_end
	MSR SPSR_fiq, x30
	ldr x30, =__stack_cpu3_und_end
	MSR SPSR_und, x30
	ldr x30, =__stack_cpu3_abt_end
	MSR SPSR_abt, x30
	ldr x30, =__stack_cpu3_irq_end
	MSR SPSR_irq, x30
	ldr x30, =__stack_cpu3_sys_end
	mov sp, x30
	mov x30, XZR
	B Reset_CPUn_Handler

	.endfunc

   .ltorg

	.section ".noinit"
	.align 4

	.space	STACKSIZEIRQ
__stack_cpu3_irq_end = .
	.space	STACKSIZEUND
__stack_cpu3_und_end = .
	.space	STACKSIZEABT
__stack_cpu3_abt_end = .
	.space	STACKSIZEFIQ
__stack_cpu3_fiq_end = .
	.space	STACKSIZEMON
__stack_cpu3_mon_end = .
//	.space	STACKSIZEHYP
//__stack_cpu3_hyp_end = .
	.space	STACKSIZESVC
__stack_cpu3_svc_end = .
	.space	STACKSIZESYSCPUn
__stack_cpu3_sys_end = .

	.word 0		/* fix non-zero size of this section */


/*** EOF ***/   
