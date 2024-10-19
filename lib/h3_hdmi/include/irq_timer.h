/**
 * @file irq_timer.h
 *
 */
/* Copyright (C) 2016-2020 by Arjan van Vught mailto:info@orangepi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef IRQ_TIMER_H_
#define IRQ_TIMER_H_

#include <stdint.h>

typedef enum irq_timers {
	IRQ_TIMER_0,
	IRQ_TIMER_1
} _irq_timers;

typedef void (*thunk_irq_timer_t)(const uint32_t);
typedef void (*thunk_irq_timer_arm_t)(void);

#ifdef __cplusplus
extern "C" {
#endif

extern void irq_timer_set(_irq_timers, thunk_irq_timer_t);
extern void irq_timer_arm_physical_set(thunk_irq_timer_arm_t);
extern void irq_timer_arm_virtual_set(thunk_irq_timer_arm_t, uint32_t);

extern void irq_timer_init(void);

#ifdef __cplusplus
}
#endif

#endif /* IRQ_TIMER_H_ */
