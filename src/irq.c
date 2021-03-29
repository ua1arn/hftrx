/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#include "board.h"
#include "formats.h"	// for debug prints


#if CPUSTYLE_ARM && WITHSMPSYSTEM

// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHEJCHB.html
// Memory attribute SHARED required for ldrex.. and strex.. functionality
void spin_lock(volatile spinlock_t * p, const char * file, int line)
{
#if WITHDEBUG
	unsigned v = 0xFFFFFFF;
#endif /* WITHDEBUG */
	// Note: __LDREXW and __STREXW are CMSIS functions
	int status;
	do
	{
		while (__LDREXW(& p->lock) != 0)// Wait until
		{
			__NOP();	// !!!! strange, but unstable work without this line...
#if WITHDEBUG
			if (-- v == 0)
			{
				PRINTF("Locked by %s(%d), wait at %s(%d)\n", p->file, p->line, file, line);
				for (;;)
					;
			}
#endif /* WITHDEBUG */
		}
		// Lock_Variable is free
		status = __STREXW(1, & p->lock); // Try to set
	// Lock_Variable
	} while (status != 0); //retry until lock successfully
	__DMB();		// Do not start any other memory access
	// until memory barrier is completed
#if WITHDEBUG
	p->file = file;
	p->line = line;
#endif /* WITHDEBUG */
}
/*

// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHEJCHB.html
// Memory attribute SHARED required for ldrex.. and strex.. functionality
void spin_lock2(volatile spinlock_t * p, const char * file, int line)
{
	// Note: __LDREXW and __STREXW are CMSIS functions
	int status;
	do
	{
		while (__LDREXW(& p->lock) != 0)// Wait until
		{
			__NOP();	// !!!! strange, but unstable work without this line...
#if WITHDEBUG
			{
				PRINTF("Locked2 by %s(%d), wait at %s(%d)\n", p->file, p->line, file, line);
				for (;;)
					;
			}
#endif  WITHDEBUG
		}
		// Lock_Variable is free
		status = __STREXW(1, & p->lock); // Try to set
	// Lock_Variable
	} while (status != 0); //retry until lock successfully
	__DMB();		// Do not start any other memory access
	// until memory barrier is completed
#if WITHDEBUG
	p->file = file;
	p->line = line;
#endif  WITHDEBUG
}
*/

void spin_unlock(volatile spinlock_t *p)
{
	// Note: __LDREXW and __STREXW are CMSIS functions
	__DMB(); // Ensure memory operations completed before
	// releasing lock
	p->lock = 0;
	return;
}

#if 0


#define LOCK(p) do { lock_impl((p), __LINE__, __FILE__, # p); } while (0)
#define UNLOCK(p) do { unlock_impl((p), __LINE__, __FILE__, # p); } while (0)


typedef struct
{
	volatile uint8_t lock;
	int line;
	const char * file;
} LOCK_T;

static void lock_impl(volatile LOCK_T * p, int line, const char * file, const char * variable)
{

	uint8_t r;
	do
		r = __LDREXB(& p->lock);
	while (__STREXB(1, & p->lock));
	if (r != 0)
	{
		PRINTF(PSTR("LOCK @%p %s already locked at %d in %s by %d in %s\n"), p, variable, line, file, p->line, p->file);
		for (;;)
			;
	}
	else
	{
		p->file = file;
		p->line = line;
	}

}

static void unlock_impl(volatile LOCK_T * p, int line, const char * file, const char * variable)
{

	uint8_t r;
	do
		r = __LDREXB(& p->lock);
	while (__STREXB(0, & p->lock));
	if (r == 0)
	{
		PRINTF(PSTR("LOCK @%p %s already unlocked at %d in %s by %d in %s\n"), p, variable, line, file, p->line, p->file);
		for (;;)
			;
	}
	else
	{
		p->file = file;
		p->line = line;
	}

}
#endif

#endif /* CPUSTYLE_ARM && WITHSMPSYSTEM */

#if CPUSTYLE_ARM
// Set interrupt vector wrapper
void arm_hardware_set_handler(uint_fast16_t int_id, void (* handler)(void), uint_fast8_t priority, uint_fast8_t targetcpu)
{
#if CPUSTYLE_AT91SAM7S

	const uint_fast32_t mask32 = (1UL << int_id);

	AT91C_BASE_AIC->AIC_IDCR = mask32;		// disable interrupt
	AT91C_BASE_AIC->AIC_SVR [int_id] = (AT91_REG) handler;	// vector

	AT91C_BASE_AIC->AIC_SMR [int_id] =
		(AT91C_AIC_SRCTYPE & AT91C_AIC_SRCTYPE_HIGH_LEVEL) |
		(AT91C_AIC_PRIOR & priority /*AT91C_AIC_PRIOR_HIGHEST*/) |
		0;

	AT91C_BASE_AIC->AIC_ICCR = mask32;		// clear pending interrupt
	AT91C_BASE_AIC->AIC_IECR = mask32;	// enable interrupt

#elif defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U)
	// Cortex-A computers

	VERIFY(IRQ_Disable(int_id) == 0);
	VERIFY(IRQ_SetHandler(int_id, handler) == 0);
	VERIFY(IRQ_SetPriority(int_id, priority) == 0);
	GIC_SetTarget(int_id, targetcpu);

#if CPUSTYLE_STM32MP1
	// peripheral (hardware) interrupts using the GIC 1-N model.
	uint_fast32_t cfg = GIC_GetConfiguration(int_id);
	cfg &= ~ 0x02;	/* Set level sensitive configuration */
	cfg |= 0x01;	/* Set 1-N model - Only one processor handles this interrupt. */
	GIC_SetConfiguration(int_id, cfg);
#endif /* CPUSTYLE_STM32MP1 */

	VERIFY(IRQ_Enable(int_id) == 0);

#else /* CPUSTYLE_STM32MP1 */

	NVIC_DisableIRQ(int_id);
	NVIC_SetVector(int_id, (uintptr_t) handler);
	NVIC_SetPriority(int_id, priority);
	NVIC_EnableIRQ(int_id);

#endif /* CPUSTYLE_STM32MP1 */
}

// Set interrupt vector wrapper
void arm_hardware_set_handler_overrealtime(uint_fast16_t int_id, void (* handler)(void))
{
	arm_hardware_set_handler(int_id, handler, ARM_OVERREALTIME_PRIORITY, TARGETCPU_OVRT);
}

// Set interrupt vector wrapper
void arm_hardware_set_handler_realtime(uint_fast16_t int_id, void (* handler)(void))
{
	arm_hardware_set_handler(int_id, handler, ARM_REALTIME_PRIORITY, TARGETCPU_RT);
}

// Set interrupt vector wrapper
void arm_hardware_set_handler_system(uint_fast16_t int_id, void (* handler)(void))
{
	arm_hardware_set_handler(int_id, handler, ARM_SYSTEM_PRIORITY, TARGETCPU_SYSTEM);
}

#endif /* CPUSTYLE_ARM */
