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

# define cfi_startproc			.cfi_startproc
# define cfi_endproc			.cfi_endproc
# define cfi_def_cfa(reg, off)		.cfi_def_cfa reg, off
# define cfi_def_cfa_register(reg)	.cfi_def_cfa_register reg
# define cfi_def_cfa_offset(off)	.cfi_def_cfa_offset off
# define cfi_adjust_cfa_offset(off)	.cfi_adjust_cfa_offset off
# define cfi_offset(reg, off)		.cfi_offset reg, off
# define cfi_rel_offset(reg, off)	.cfi_rel_offset reg, off
# define cfi_register(r1, r2)		.cfi_register r1, r2
# define cfi_return_column(reg)	.cfi_return_column reg
# define cfi_restore(reg)		.cfi_restore reg
# define cfi_same_value(reg)		.cfi_same_value reg
# define cfi_undefined(reg)		.cfi_undefined reg
# define cfi_remember_state		.cfi_remember_state
# define cfi_restore_state		.cfi_restore_state
# define cfi_window_save		.cfi_window_save
# define cfi_personality(enc, exp)	.cfi_personality enc, exp
# define cfi_lsda(enc, exp)		.cfi_lsda enc, exp

.macro trap_entry
    addi sp, sp, -64
    sw x1,  0(sp)
    sw x5,  4(sp)
    sw x6,  8(sp)
    sw x7,  12(sp)
    sw x10, 16(sp)
    sw x11, 20(sp)
    sw x12, 24(sp)
    sw x13, 28(sp)
    sw x14, 32(sp)
    sw x15, 36(sp)
    sw x16, 40(sp)
    sw x17, 44(sp)
    sw x28, 48(sp)
    sw x29, 52(sp)
    sw x30, 56(sp)
    sw x31, 60(sp)
.endm


.macro trap_exit
  lw x1,  0(sp)
  lw x5,  4(sp)
  lw x6,  8(sp)
  lw x7,  12(sp)
  lw x10, 16(sp)
  lw x11, 20(sp)
  lw x12, 24(sp)
  lw x13, 28(sp)
  lw x14, 32(sp)
  lw x15, 36(sp)
  lw x16, 40(sp)
  lw x17, 44(sp)
  lw x28, 48(sp)
  lw x29, 52(sp)
  lw x30, 56(sp)
  lw x31, 60(sp)
  addi sp, sp, 64
  mret
.endm

	.global Reset_Handler
.section .startup0
.align 8
	cfi_startproc
	cfi_undefined (ra)
Reset_Handler:
	//lla   gp, 0//__global_pointer$
	/*
   mv  a1, zero
   mv  x1, zero
   mv  x2, zero
   mv  x3, zero
   mv  x4, zero
   mv  x5, zero
   mv  x6, zero
   mv  x7, zero
   mv  x8, zero
   mv  x9, zero
   mv  x10, zero
   mv  x11, zero
   mv  x12, zero
   mv  x13, zero
   mv  x14, zero
   mv  x15, zero
   mv  x16, zero
   mv  x17, zero
   mv  x18, zero
   mv  x19, zero
   mv  x20, zero
   mv  x21, zero
   mv  x22, zero
   mv  x23, zero
   mv  x24, zero
   mv  x25, zero
   mv  x26, zero
   mv  x27, zero
   mv  x28, zero
   mv  x29, zero
   mv  x30, zero
   mv  x31, zero
*/

	lla    sp, __stack
	mv	gp, zero
    mv  a0, zero
    mv  a1, zero
	call SystemInit
	call __cmsis_start
	j _start
	cfi_endproc

.section .isr_vector
.align 8

.global IV

IV:
    j _synctrap
    .align 2
    j trap_empty
    .align 2
    j trap_empty
    .align 2
    j trap_vmsi
    .align 2
    j trap_empty
    .align 2
    j trap_empty
    .align 2
    j trap_empty
    .align 2
    j trap_vmti
    .align 2
    j trap_empty
    .align 2
    j trap_empty
    .align 2
    j trap_empty
    .align 2
    j trap_vmei
    .align 2
    j trap_empty
    .align 2
    j trap_empty
    .align 2
    j trap_empty
    .align 2
    j trap_empty
    .align 2
    j trap_irq0
    .align 2
    j trap_irq1
    .align 2
    j trap_irq2
    .align 2
    j trap_irq3
    .align 2
    j trap_irq4
    .align 2
    j trap_irq5
    .align 2
    j trap_irq6
    .align 2
    j trap_irq7
    .align 2
    j trap_irq8
    .align 2
    j trap_irq9
    .align 2
    j trap_irq10
    .align 2
    j trap_irq11
    .align 2
    j trap_irq12
    .align 2
    j trap_irq13
    .align 2
    j trap_irq14
    .align 2
    j trap_irq15
    .align 2

_synctrap:
  trap_entry
  jal isr_synctrap
  trap_exit

trap_vmsi:
  trap_entry
  jal isr_vmsi
  trap_exit

trap_vmti:
  trap_entry
  jal isr_vmti
  trap_exit

trap_vmei:
  trap_entry
  jal isr_vmei
  trap_exit

trap_irq0:
  trap_entry
  jal isr_irq0
  trap_exit

trap_irq1:
  trap_entry
  jal isr_irq1
  trap_exit

trap_irq2:
  trap_entry
  jal isr_irq2
  trap_exit

trap_irq3:
  trap_entry
  jal isr_irq3
  trap_exit

trap_irq4:
  trap_entry
  jal isr_irq4
  trap_exit

trap_irq5:
  trap_entry
  jal isr_irq5
  trap_exit

trap_irq6:
  trap_entry
  jal isr_irq6
  trap_exit

trap_irq7:
  trap_entry
  jal isr_irq7
  trap_exit

trap_irq8:
  trap_entry
  jal isr_irq8
  trap_exit

trap_irq9:
  trap_entry
  jal isr_irq9
  trap_exit

trap_irq10:
  trap_entry
  jal isr_irq10
  trap_exit

trap_irq11:
  trap_entry
  jal isr_irq11
  trap_exit

trap_irq12:
  trap_entry
  jal isr_irq12
  trap_exit

trap_irq13:
  trap_entry
  jal isr_irq13
  trap_exit

trap_irq14:
  trap_entry
  jal isr_irq14
  trap_exit

trap_irq15:
  trap_entry
  jal isr_irq15
  trap_exit
trap_empty:
    nop
