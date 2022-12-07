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

// __attribute__ ((interrupt ("machine")))

.macro trap_entry
	addi	sp,sp,-288
	sd	a5,208(sp)
	sd	ra,280(sp)
	sd	t0,272(sp)
	sd	t1,264(sp)
	sd	t2,256(sp)
	sd	a0,248(sp)
	sd	a1,240(sp)
	sd	a2,232(sp)
	sd	a3,224(sp)
	sd	a4,216(sp)
	sd	a6,200(sp)
	sd	a7,192(sp)
	sd	t3,184(sp)
	sd	t4,176(sp)
	sd	t5,168(sp)
	sd	t6,160(sp)
	fsd	ft0,152(sp)
	fsd	ft1,144(sp)
	fsd	ft2,136(sp)
	fsd	ft3,128(sp)
	fsd	ft4,120(sp)
	fsd	ft5,112(sp)
	fsd	ft6,104(sp)
	fsd	ft7,96(sp)
	fsd	fa0,88(sp)
	fsd	fa1,80(sp)
	fsd	fa2,72(sp)
	fsd	fa3,64(sp)
	fsd	fa4,56(sp)
	fsd	fa5,48(sp)
	fsd	fa6,40(sp)
	fsd	fa7,32(sp)
	fsd	ft8,24(sp)
	fsd	ft9,16(sp)
	fsd	ft10,8(sp)
	fsd	ft11,0(sp)
.endm


.macro trap_exit
	ld	ra,280(sp)
	ld	t0,272(sp)
	ld	t1,264(sp)
	ld	t2,256(sp)
	ld	a0,248(sp)
	ld	a1,240(sp)
	ld	a2,232(sp)
	ld	a3,224(sp)
	ld	a4,216(sp)
	ld	a5,208(sp)
	ld	a6,200(sp)
	ld	a7,192(sp)
	ld	t3,184(sp)
	ld	t4,176(sp)
	ld	t5,168(sp)
	ld	t6,160(sp)
	fld	ft0,152(sp)
	fld	ft1,144(sp)
	fld	ft2,136(sp)
	fld	ft3,128(sp)
	fld	ft4,120(sp)
	fld	ft5,112(sp)
	fld	ft6,104(sp)
	fld	ft7,96(sp)
	fld	fa0,88(sp)
	fld	fa1,80(sp)
	fld	fa2,72(sp)
	fld	fa3,64(sp)
	fld	fa4,56(sp)
	fld	fa5,48(sp)
	fld	fa6,40(sp)
	fld	fa7,32(sp)
	fld	ft8,24(sp)
	fld	ft9,16(sp)
	fld	ft10,8(sp)
	fld	ft11,0(sp)
	addi	sp,sp,288
.endm

.macro mtrap_handler func, tail=_Handler
trap_\func:
	trap_entry
	call \func\tail
	trap_exit
	mret
.endm

	.global Reset_Handler
.section .startup0
.align 8
Reset_Handler:
	/* set all registers to zero */
	mv  x1, x0
	mv  x2, x1
	mv  x3, x1
	mv  x4, x1
	mv  x5, x1
	mv  x6, x1
	mv  x7, x1
	mv  x8, x1
	mv  x9, x1
	mv x10, x1
	mv x11, x1
	mv x12, x1
	mv x13, x1
	mv x14, x1
	mv x15, x1
	mv x16, x1
	mv x17, x1
	mv x18, x1
	mv x19, x1
	mv x20, x1
	mv x21, x1
	mv x22, x1
	mv x23, x1
	mv x24, x1
	mv x25, x1
	mv x26, x1
	mv x27, x1
	mv x28, x1
	mv x29, x1
	mv x30, x1
	mv x31, x1

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


