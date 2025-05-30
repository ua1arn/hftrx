/*
 * Copyright (c) 2013-2024, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

//#include <asm_macros.S>

	.globl	spin_lock
	.globl	spin_unlock
	.globl	bit_lock
	.globl	bit_unlock

#if USE_SPINLOCK_CAS
#if !ARM_ARCH_AT_LEAST(8, 1)
#error USE_SPINLOCK_CAS option requires at least an ARMv8.1 platform
#endif

/*
 * When compiled for ARMv8.1 or later, choose spin locks based on Compare and
 * Swap instruction.
 */

/*
 * Acquire lock using Compare and Swap instruction.
 *
 * Compare for 0 with acquire semantics, and swap 1. If failed to acquire, use
 * load exclusive semantics to monitor the address and enter WFE.
 *
 * void spin_lock(spinlock_t *lock);
 */
.func spin_lock
	mov	w2, #1
1:	mov	w1, wzr
2:	casa	w1, w2, [x0]
	cbz	w1, 3f
	ldxr	w1, [x0]
	cbz	w1, 2b
	wfe
	b	1b
3:
	ret
.endfunc spin_lock

#else /* !USE_SPINLOCK_CAS */

/*
 * Acquire lock using load-/store-exclusive instruction pair.
 *
 * void spin_lock(spinlock_t *lock);
 */
.func spin_lock
	mov	w2, #1
	sevl
l1:	wfe
l2:	ldaxr	w1, [x0]
	cbnz	w1, l1
	stxr	w1, w2, [x0]
	cbnz	w1, l2
	ret
.endfunc //spin_lock

#endif /* USE_SPINLOCK_CAS */

/*
 * Release lock previously acquired by spin_lock.
 *
 * Use store-release to unconditionally clear the spinlock variable.
 * Store operation generates an event to all cores waiting in WFE
 * when address is monitored by the global monitor.
 *
 * void spin_unlock(spinlock_t *lock);
 */
.func spin_unlock
	stlr	wzr, [x0]
	ret
.endfunc //spin_unlock

/*
 * Atomic bit clear and set instructions require FEAT_LSE which is
 * mandatory from Armv8.1.
 */
#if 0//ARM_ARCH_AT_LEAST(8, 1)

/*
 * Acquire bitlock using atomic bit set on byte. If the original read value
 * has the bit set, use load exclusive semantics to monitor the address and
 * enter WFE.
 *
 * void bit_lock(bitlock_t *lock, uint8_t mask);
 */
.func bit_lock
1:	ldsetab	w1, w2, [x0]
	tst	w2, w1
	b.eq	2f
	ldxrb	w2, [x0]
	tst	w2, w1
	b.eq	1b
	wfe
	b	1b
2:
	ret
.endfunc //bit_lock

/*
 * Use atomic bit clear store-release to unconditionally clear bitlock variable.
 * Store operation generates an event to all cores waiting in WFE when address
 * is monitored by the global monitor.
 *
 * void bit_unlock(bitlock_t *lock, uint8_t mask);
 */
.func bit_unlock
	stclrlb	w1, [x0]
	ret
.endfunc //bit_unlock

#endif /* ARM_ARCH_AT_LEAST(8, 1) */
