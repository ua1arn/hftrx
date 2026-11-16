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
	/* Invoked in AArch64 EL3 */
Reset_CPUx_Handler:
	/* Ensure all exceptions are masked (D, A, I, F) */
	msr     daifset, #0xF

	/* Select SP_EL3 as the active stack pointer (SPSel = 1) */
	msr     spsel, #1
	isb

	/* Load top of allocated stack and enforce 16-byte alignment */
	ldr     x30, =aarch64_stack_top
	ldr     x30, [x30]
	and     x30, x30, #~0xF
	mov     sp, x30

	/* Optionally initialize lower EL stack pointers to zero */
	msr     sp_el0, xzr
	msr     sp_el1, xzr
	msr     sp_el2, xzr

	/* Enable FP/SIMD access in EL3 (clear TFP bit 10 in CPTR_EL3) */
	mrs     x0, cptr_el3
	bic     x0, x0, #(1 << 10)
	msr     cptr_el3, x0
	isb

	mov     x30, xzr
	b       Reset_CPUn_Handler

	.endfunc
	.ltorg

/*** EOF ***/   
