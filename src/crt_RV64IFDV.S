// See:
// https://github.com/syntacore/sc-bl/blob/master/src/startup.S
// https://codebrowser.dev/glibc/glibc/sysdeps/riscv/start.S.html

// https://github.com/riscv-non-isa/riscv-asm-manual/blob/master/riscv-asm.md

/**
 * RISC-V bootup test
 * Author: Daniele Lacamera <root@danielinux.net>
 *
 * MIT License
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

.macro trap_entry
	addi	sp,sp,-512
//	sd x0,0(sp)		/* zero register */
	sd x1,8(sp)		/* ABI link register ra */
//	sd x2,16(sp)	/* ABI stack pointer sp */
	sd x3,24(sp)
	sd x4,32(sp)
	sd x5,40(sp)
	sd x6,48(sp)
	sd x7,56(sp)
	sd x8,64(sp)
	sd x9,72(sp)
	sd x10,80(sp)
	sd x11,88(sp)
	sd x12,96(sp)
	sd x13,104(sp)
	sd x14,112(sp)
	sd x15,120(sp)
	sd x16,128(sp)
	sd x17,136(sp)
	sd x18,144(sp)
	sd x19,152(sp)
	sd x20,160(sp)
	sd x21,168(sp)
	sd x22,176(sp)
	sd x23,184(sp)
	sd x24,192(sp)
	sd x25,200(sp)
	sd x26,208(sp)
	sd x27,216(sp)
	sd x28,224(sp)
	sd x29,232(sp)
	sd x30,240(sp)
	sd x31,248(sp)
	fsd f0,256(sp)
	fsd f1,264(sp)
	fsd f2,272(sp)
	fsd f3,280(sp)
	fsd f4,288(sp)
	fsd f5,296(sp)
	fsd f6,304(sp)
	fsd f7,312(sp)
	fsd f8,320(sp)
	fsd f9,328(sp)
	fsd f10,336(sp)
	fsd f11,344(sp)
	fsd f12,352(sp)
	fsd f13,360(sp)
	fsd f14,368(sp)
	fsd f15,376(sp)
	fsd f16,384(sp)
	fsd f17,392(sp)
	fsd f18,400(sp)
	fsd f19,408(sp)
	fsd f20,416(sp)
	fsd f21,424(sp)
	fsd f22,432(sp)
	fsd f23,440(sp)
	fsd f24,448(sp)
	fsd f25,456(sp)
	fsd f26,464(sp)
	fsd f27,472(sp)
	fsd f28,480(sp)
	fsd f29,488(sp)
	fsd f30,496(sp)
	fsd f31,504(sp)
.endm

.macro trap_exit
//	ld x0,0(sp)		/* zero register */
	ld x1,8(sp)		/* ABI link register ra */
//	ld x2,16(sp)	/* ABI stack pointer sp */
	ld x3,24(sp)
	ld x4,32(sp)
	ld x5,40(sp)
	ld x6,48(sp)
	ld x7,56(sp)
	ld x8,64(sp)
	ld x9,72(sp)
	ld x10,80(sp)
	ld x11,88(sp)
	ld x12,96(sp)
	ld x13,104(sp)
	ld x14,112(sp)
	ld x15,120(sp)
	ld x16,128(sp)
	ld x17,136(sp)
	ld x18,144(sp)
	ld x19,152(sp)
	ld x20,160(sp)
	ld x21,168(sp)
	ld x22,176(sp)
	ld x23,184(sp)
	ld x24,192(sp)
	ld x25,200(sp)
	ld x26,208(sp)
	ld x27,216(sp)
	ld x28,224(sp)
	ld x29,232(sp)
	ld x30,240(sp)
	ld x31,248(sp)
	fld f0,256(sp)
	fld f1,264(sp)
	fld f2,272(sp)
	fld f3,280(sp)
	fld f4,288(sp)
	fld f5,296(sp)
	fld f6,304(sp)
	fld f7,312(sp)
	fld f8,320(sp)
	fld f9,328(sp)
	fld f10,336(sp)
	fld f11,344(sp)
	fld f12,352(sp)
	fld f13,360(sp)
	fld f14,368(sp)
	fld f15,376(sp)
	fld f16,384(sp)
	fld f17,392(sp)
	fld f18,400(sp)
	fld f19,408(sp)
	fld f20,416(sp)
	fld f21,424(sp)
	fld f22,432(sp)
	fld f23,440(sp)
	fld f24,448(sp)
	fld f25,456(sp)
	fld f26,464(sp)
	fld f27,472(sp)
	fld f28,480(sp)
	fld f29,488(sp)
	fld f30,496(sp)
	fld f31,504(sp)
	addi	sp,sp,512
.endm

/* set all integer registers to zero */
.macro fill0
	mv	x3, zero	/* x0 */
	mv  x4, x3
	mv  x5, x3
	mv  x6, x3
	mv  x7, x3
	mv  x8, x3
	mv  x9, x3
	mv x10, x3
	mv x11, x3
	mv x12, x3
	mv x13, x3
	mv x14, x3
	mv x15, x3
	mv x16, x3
	mv x17, x3
	mv x18, x3
	mv x19, x3
	mv x20, x3
	mv x21, x3
	mv x22, x3
	mv x23, x3
	mv x24, x3
	mv x25, x3
	mv x26, x3
	mv x27, x3
	mv x28, x3
	mv x29, x3
	mv x30, x3
	mv x31, x3
.endm

/* set all floating registers to zero */
.macro fill0fp
	fcvt.s.w f0, x0
	fcvt.s.w f1, x0
	fcvt.s.w f2, x0
	fcvt.s.w f3, x0
	fcvt.s.w f4, x0
	fcvt.s.w f5, x0
	fcvt.s.w f6, x0
	fcvt.s.w f7, x0
	fcvt.s.w f8, x0
	fcvt.s.w f9, x0
	fcvt.s.w f10, x0
	fcvt.s.w f11, x0
	fcvt.s.w f12, x0
	fcvt.s.w f13, x0
	fcvt.s.w f14, x0
	fcvt.s.w f15, x0
	fcvt.s.w f16, x0
	fcvt.s.w f17, x0
	fcvt.s.w f18, x0
	fcvt.s.w f19, x0
	fcvt.s.w f20, x0
	fcvt.s.w f21, x0
	fcvt.s.w f22, x0
	fcvt.s.w f23, x0
	fcvt.s.w f24, x0
	fcvt.s.w f25, x0
	fcvt.s.w f26, x0
	fcvt.s.w f27, x0
	fcvt.s.w f28, x0
	fcvt.s.w f29, x0
	fcvt.s.w f30, x0
	fcvt.s.w f31, x0
.endm

.macro mtrap_handler func, tail=_Handler
trap_\func:
	trap_entry
	//fill0
	//fill0fp		/* set all floating registers to zero */
	call \func\tail
	trap_exit
	mret
.endm

	.global Reset_Handler
.section .startup0
.align 8
Reset_Handler:
	fill0		/* set all integer registers to zero */

	/* stack initilization */
	la    sp, __stack
	mv	gp, zero
    mv  a0, zero
    mv  a1, zero
	call SystemInit
	call __riscv_start
    /* to let simulator know program end point */
    addi x1,x0,1
    addi x2,x0,-1
    sw x1, 0(x2)

.section .isr_vector
.align 8

.global __Vectors

__Vectors:
    j trap_SYNCTRAP
    .align 2
    j trap_EMPTY
    .align 2
    j trap_EMPTY
    .align 2
    j trap_VMSI
    .align 2
    j trap_EMPTY
    .align 2
    j trap_EMPTY
    .align 2
    j trap_EMPTY
    .align 2
    j trap_VMTI
    .align 2
    j trap_EMPTY
    .align 2
    j trap_EMPTY
    .align 2
    j trap_EMPTY
    .align 2
    j trap_VMEI
    .align 2
    j trap_EMPTY
    .align 2
    j trap_EMPTY
    .align 2
    j trap_EMPTY
    .align 2
    j trap_EMPTY
    .align 2
    j trap_IRQ0
    .align 2
    j trap_IRQ1
    .align 2
    j trap_IRQ2
    .align 2
    j trap_IRQ3
    .align 2
    j trap_IRQ4
    .align 2
    j trap_IRQ5
    .align 2
    j trap_IRQ6
    .align 2
    j trap_IRQ7
    .align 2
    j trap_IRQ8
    .align 2
    j trap_IRQ9
    .align 2
    j trap_IRQ10
    .align 2
    j trap_IRQ11
    .align 2
    j trap_IRQ12
    .align 2
    j trap_IRQ13
    .align 2
    j trap_IRQ14
    .align 2
    j trap_IRQ15
    .align 2

 //   .space (4096 - (16 + 16) * 4)

.section .text

	mtrap_handler SYNCTRAP
	mtrap_handler VMSI
	mtrap_handler VMTI
	mtrap_handler VMEI

	mtrap_handler IRQ0
	mtrap_handler IRQ1
	mtrap_handler IRQ2
	mtrap_handler IRQ3
	mtrap_handler IRQ4
	mtrap_handler IRQ5
	mtrap_handler IRQ6
	mtrap_handler IRQ7
	mtrap_handler IRQ8
	mtrap_handler IRQ9
	mtrap_handler IRQ10
	mtrap_handler IRQ11
	mtrap_handler IRQ12
	mtrap_handler IRQ13
	mtrap_handler IRQ14
	mtrap_handler IRQ15

	mtrap_handler EMPTY


