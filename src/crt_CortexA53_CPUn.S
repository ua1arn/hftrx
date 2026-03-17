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

	STACKSIZEALL = 4096

	.section ".noinit"

	.align 4, 0
	.global aarch64_stack_top
aarch64_stack_top:
	.dword 0

	.section ".text"
	.align 4, 0

  	.extern Reset_CPUn_Handler

  	.global Reset_CPUx_Handler
    .func   Reset_CPUx_Handler
    /* invoked at ARM_MODE_SVC */
Reset_CPUx_Handler:
	ldr x29, = STACKSIZEALL
	ldr x30, = aarch64_stack_top
	ldr x30, [x30]

	MSR SPSR_fiq, x30
	sub x30, x30, x29
	MSR SPSR_und, x30
	sub x30, x30, x29
	MSR SPSR_abt, x30
	sub x30, x30, x29
	MSR SPSR_irq, x30
	sub x30, x30, x29
	mov sp, x30
	mov x30, XZR
	B Reset_CPUn_Handler

	.endfunc

   .ltorg



/*** EOF ***/   
