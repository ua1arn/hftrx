/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#include "board.h"
#include "formats.h"	// for debug prints

#if (__CORTEX_M != 0)

uint32_t gARM_OVERREALTIME_PRIORITY;
uint32_t gARM_REALTIME_PRIORITY;
uint32_t gARM_SYSTEM_PRIORITY;
uint32_t gARM_BASEPRI_ONLY_REALTIME;
uint32_t gARM_BASEPRI_ONLY_OVERREALTIME;
uint32_t gARM_BASEPRI_ALL_ENABLED;

static void
arm_cpu_CMx_initialize_NVIC(void)
{
	// See usage of functions NVIC_PriorityGroupConfig and NVIC_SetPriorityGrouping

	//NVIC_SetPriorityGrouping(7);	// no preemption, 4 bit of subprio
	//NVIC_SetPriorityGrouping(6);	// 1 bit preemption, 3 bit of subprio
	NVIC_SetPriorityGrouping(5);	// 2 bit preemption, 2 bit of subprio
	//NVIC_SetPriorityGrouping(4);	// 3 bit preemption, 1 bit of subprio
	//NVIC_SetPriorityGrouping(3);	// 4 bit preemption, 0 bit of subprio
	//NVIC_SetPriorityGrouping(0);

	// Вычисление значений приоритета для данной конфигурации
	gARM_OVERREALTIME_PRIORITY = NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0);
	gARM_REALTIME_PRIORITY = NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0);
	gARM_SYSTEM_PRIORITY = NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0);
	// The processor does not process any exception with a priority value greater than or equal to BASEPRI.
	gARM_BASEPRI_ONLY_REALTIME = ((gARM_SYSTEM_PRIORITY << (8 - __NVIC_PRIO_BITS)) & 0xff);
	gARM_BASEPRI_ONLY_OVERREALTIME = ((gARM_REALTIME_PRIORITY << (8 - __NVIC_PRIO_BITS)) & 0xff);
	gARM_BASEPRI_ALL_ENABLED = 0;

	/* System interrupt init*/
	/* MemoryManagement_IRQn interrupt configuration */
	NVIC_SetPriority(MemoryManagement_IRQn, ARM_SYSTEM_PRIORITY);
	/* BusFault_IRQn interrupt configuration */
	NVIC_SetPriority(BusFault_IRQn, ARM_SYSTEM_PRIORITY);
	/* UsageFault_IRQn interrupt configuration */
	NVIC_SetPriority(UsageFault_IRQn, ARM_SYSTEM_PRIORITY);
	/* SVCall_IRQn interrupt configuration */
	NVIC_SetPriority(SVCall_IRQn, ARM_SYSTEM_PRIORITY);
	/* DebugMonitor_IRQn interrupt configuration */
	NVIC_SetPriority(DebugMonitor_IRQn, ARM_SYSTEM_PRIORITY);
	/* PendSV_IRQn interrupt configuration */
	NVIC_SetPriority(PendSV_IRQn, ARM_SYSTEM_PRIORITY);
	/* SysTick_IRQn interrupt configuration */
	NVIC_SetPriority(SysTick_IRQn, ARM_SYSTEM_PRIORITY);

	// Назначить таймеру приоритет, равный всем остальным прерываниям. Разрешать через NVIC не требуется
	NVIC_SetVector(SysTick_IRQn, (uintptr_t) & SysTick_Handler);
	NVIC_SetPriority(SysTick_IRQn, ARM_SYSTEM_PRIORITY);

	//__set_BASEPRI(gARM_BASEPRI_ALL_ENABLED);
}

#endif /* (__CORTEX_M != 0) */

#if defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U)
/*
	ARM IHI 0048B.b (IHI0048B_b_gic_architecture_specification.pdf).
	4.3.11 Interrupt Priority Registers, GICD_IPRIORITYRn says:
	These registers are byte-accessible.
*/


uint32_t gARM_OVERREALTIME_PRIORITY;
uint32_t gARM_REALTIME_PRIORITY;
uint32_t gARM_SYSTEM_PRIORITY;
uint32_t gARM_BASEPRI_ONLY_REALTIME;
uint32_t gARM_BASEPRI_ONLY_OVERREALTIME;
uint32_t gARM_BASEPRI_ALL_ENABLED;


void
arm_gic_initialize(void)
{
	IRQ_Initialize();

	GIC_Enable();

#if CPUSTYLE_R7S721
	r7s721_intc_initialize();
#endif /* CPUSTYLE_R7S721 */

#if WITHNESTEDINTERRUPTS
	gARM_OVERREALTIME_PRIORITY = ARM_CA9_ENCODE_PRIORITY(PRI_OVRT);	// value for GIC_SetPriority
	gARM_REALTIME_PRIORITY = ARM_CA9_ENCODE_PRIORITY(PRI_RT);	// value for GIC_SetPriority
	gARM_SYSTEM_PRIORITY = ARM_CA9_ENCODE_PRIORITY(PRI_SYS);		// value for GIC_SetPriority

	gARM_BASEPRI_ONLY_REALTIME = ARM_CA9_ENCODE_PRIORITY(PRI_SYS);	// value for GIC_SetInterfacePriorityMask
	gARM_BASEPRI_ONLY_OVERREALTIME = ARM_CA9_ENCODE_PRIORITY(PRI_RT);	// value for GIC_SetInterfacePriorityMask
	gARM_BASEPRI_ALL_ENABLED = ARM_CA9_ENCODE_PRIORITY(PRI_USER);	// value for GIC_SetInterfacePriorityMask

	GIC_SetInterfacePriorityMask(gARM_BASEPRI_ALL_ENABLED);
#endif /* WITHNESTEDINTERRUPTS */
}

#endif /* defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U) */

#if (__CORTEX_M != 0)

/*----------------------------------------------------------------------------
 *        Exported variables
 *----------------------------------------------------------------------------*/

/* Initialize segments */
void Default_Handler(void)
{
	PRINTF(PSTR("Default_Handler trapped, ICSR=%08lX (IRQn=%u).\n"), SCB->ICSR, (SCB->ICSR & 0xFF) - 16);
	for (;;)
		;
}
////////////////////////////

#if WITHDEBUG && (CPUSTYLE_ARM_CM7 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM3)

//Эта функция извлекает из стека регистры сохраненные при возникновении исключения.
static void
__attribute__((used))
prvGetRegistersFromStack( uint32_t *pulFaultStackAddress )
{
// Эти переменные объявлены как volatile для предотвращения оптимизации компилятором/линкером, так как компилятор
//предположит, что пременные никогда не используются и может устранить их из кода. Если отладчик не показывает
//значения этих переменных, тогда нужно сделать их глобальными, выснеся их определения за пределы этой фукнкции.
volatile uint32_t r0;
volatile uint32_t r1;
volatile uint32_t r2;
volatile uint32_t r3;
volatile uint32_t r12;
volatile uint32_t lr; // Регистры связи.
volatile uint32_t pc; // Программный счетчик.
volatile uint32_t psr;// Регистр статуса программы.

    r0 = pulFaultStackAddress [0];
    r1 = pulFaultStackAddress [1];
    r2 = pulFaultStackAddress [2];
    r3 = pulFaultStackAddress [3];

    r12 = pulFaultStackAddress [4];
    lr = pulFaultStackAddress [5];
    pc = pulFaultStackAddress [6];
    psr = pulFaultStackAddress [7];


	PRINTF(PSTR("HardFault_Handler trapped.\n"));
 	PRINTF(PSTR(" CPUID=%08lx\n"), SCB->CPUID);
	PRINTF(PSTR(" HFSR=%08lx\n"), SCB->HFSR);
	PRINTF(PSTR(" CFSR=%08lx\n"), SCB->CFSR);
	PRINTF(PSTR(" BFAR=%08lx\n"), SCB->BFAR);

	PRINTF(PSTR(" R0=%08lx\n"), r0);
	PRINTF(PSTR(" R1=%08lx\n"), r1);
	PRINTF(PSTR(" R2=%08lx\n"), r2);
	PRINTF(PSTR(" R3=%08lx\n"), r3);

	PRINTF(PSTR(" R12=%08lx\n"), r12);
	PRINTF(PSTR(" LR=%08lx\n"), lr);
	PRINTF(PSTR(" PC=%08lx\n"), pc);
	PRINTF(PSTR(" PSR=%08lx\n"), psr);

    // Когда мы добрались до этой строки, то в переменных содержатся значения регистров.
    for( ;; )
		;
}

static const void * volatile const ph =  prvGetRegistersFromStack;	// 'used' ignored...


#endif /* WITHDEBUG && (CPUSTYLE_ARM_CM7 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM3) */


///////////////////////////
//
// taken from: http://forum.easyelectronics.ru/viewtopic.php?p=396176#p396176

/*=================================================================================================================================
*  Обработчик HardFault исключений. В нем вызывается функция prvGetRegistersFromStack(), которая сохраняет в переменных, значения
* регистров программы, в момент возникновения исключения и входит в бесконечный цикл. Таким образом, можно по значениям переменных
* узнать причину возникновения исключения.
=================================================================================================================================*/


void
HardFault_Handler(void)
{
#if WITHDEBUG
#if 0 && (CPUSTYLE_ARM_CM7 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM3)

   __asm volatile
   (
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler2_address_const                            \n"
        " bx r2                                                     \n"
        " handler2_address_const: .word prvGetRegistersFromStack    \n"
    );

#elif CPUSTYLE_ARM_CM0

	PRINTF(PSTR("HardFault_Handler trapped.\n"));
	PRINTF(PSTR(" CPUID=%08lx\n"), SCB->CPUID);

#else

//	dbg_putchar('S');	// "SK"
//	dbg_putchar('K');
	PRINTF(PSTR("HardFault_Handler trapped.\n"));
	PRINTF(PSTR(" CPUID=%08lx\n"), SCB->CPUID);

#endif
	//PRINTF(PSTR("HardFault_Handler trapped. HFSR=%08lx\n"), SCB->HFSR);
	//PRINTF(PSTR("HardFault_Handler trapped"));
	//return;
	//if ((SCB->HFSR & SCB_HFSR_FORCED_Msk) != 0)
	//{
	//}
	//local_delay_ms(10);
#endif /* WITHDEBUG */

	for (;;)
		; // WDT->WDT_CR = WDT_CR_WDRSTT | WDT_CR_KEY(0xA5);

}

void
NMI_Handler(void)
{
	PRINTF(PSTR("NMI_Handler trapped\n"));
	for (;;)
		; // WDT->WDT_CR = WDT_CR_WDRSTT | WDT_CR_KEY(0xA5);
}

void
MemManage_Handler(void)
{
	PRINTF(PSTR("MemManage_Handler trapped\n"));
	for (;;)
		;
}

void
BusFault_Handler(void)
{
	PRINTF(PSTR("BusFault_Handler trapped\n"));
	for (;;)
		;
}

void
UsageFault_Handler(void)
{
	PRINTF(PSTR("UsageFault_Handler trapped\n"));
	for (;;)
		;
}

void
SVC_Handler(void)
{
	PRINTF(PSTR("SVC_Handler trapped\n"));
	for (;;)
		;
}

void
DebugMon_Handler(void)
{
	PRINTF(PSTR("DebugMon_Handler trapped\n"));
	for (;;)
		;
}

void
PendSV_Handler(void)
{
	PRINTF(PSTR("PendSV_Handler trapped\n"));
	for (;;)
		;
}

typedef void (* IntFunc)(void);

extern unsigned long __stack;

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */

void Reset_Handler(void)
{
	  SystemInit();                             /* CMSIS System Initialization */
	  __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
/*------------------------------------------------------------------------------
 *         Exception Table
 *------------------------------------------------------------------------------*/


const
__VECTOR_TABLE_ATTRIBUTE
IntFunc __Vectors [NVIC_USER_IRQ_OFFSET] = {

    /* Configure Initial Stack Pointer, using linker-generated symbols */
    (IntFunc)(& __stack),
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    NULL,         /* Reserved */
	NULL,         /* Reserved */
	NULL,         /* Reserved */
	NULL,         /* Reserved */
    SVC_Handler,
    DebugMon_Handler,
	NULL,                  /* Reserved  */
    PendSV_Handler,		/* -2 */
    SysTick_Handler,	/* -1 */
};

// Таблица находится в области вне Data Cache
// Отладочная печать тут еще недопустима.
static VTRATTR volatile IntFunc ramVectors [256];

static void vectors_relocate(void)
{
	unsigned i;

	//PRINTF(PSTR("SCB->VTOR=%08lX\n"), SCB->VTOR);
	memcpy((void *) ramVectors, __Vectors, NVIC_USER_IRQ_OFFSET * 4);
	for (i = NVIC_USER_IRQ_OFFSET; i < (sizeof ramVectors / sizeof ramVectors [0]); ++ i)
	{
		ramVectors [i] = Default_Handler;
	}
	SCB->VTOR = (uint32_t) & ramVectors;

	// Отладочная печать тут еще недопустима.
	//PRINTF(PSTR("SCB->VTOR=%08lX\n"), SCB->VTOR);
	//ASSERT(memcmp((void *) ramVectors, __Vectors, NVIC_USER_IRQ_OFFSET * 4) == 0);
	//ASSERT(SCB->VTOR == (uint32_t) & ramVectors);
}
#endif /* (__CORTEX_M != 0) */

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

// Вызывается из main
void cpu_initialize(void)
{
	//PRINTF("cpu_initialize\n");
//	PRINTF("TTB_PARA_DEVICE=%08lX (xxx)\n", (unsigned long) TTB_PARA_DEVICE);
//	PRINTF("TTB_PARA_STRGLY=%08lX (0x00DE2)\n", (unsigned long) TTB_PARA_STRGLY);
//	PRINTF("TTB_PARA_NORMAL_CACHE=%08lX (0x01DEEuL)\n", (unsigned long) TTB_PARA_NORMAL_CACHE);
//	PRINTF("TTB_PARA_NORMAL_NOT_CACHE=%08lX (0x01DE2uL)\n", (unsigned long) TTB_PARA_NORMAL_NOT_CACHE);

//	extern unsigned long __etext, __bss_start__, __bss_end__, __data_end__, __data_start__, __stack, __Vectors;
//
//	PRINTF(PSTR("cpu_initialize1: CP15=%08lX, __data_start__=%p\n"), __get_SCTLR(), & __data_start__);
//	PRINTF(PSTR("__etext=%p, __bss_start__=%p, __bss_end__=%p, __data_start__=%p, __data_end__=%p\n"), & __etext, & __bss_start__, & __bss_end__, & __data_start__, & __data_end__);
//	PRINTF(PSTR("__stack=%p, SystemInit=%p, __Vectors=%p\n"), & __stack, SystemInit, & __Vectors);

//	ca9_ca7_cache_diag();	// print

#if (__CORTEX_A == 7U) || (__CORTEX_A == 9U)

	cpump_initialize();

#elif CPUSTYLE_ATMEGA328
	// управления JTAG нету

#elif CPUSTYLE_ATMEGA128

#elif CPUSTYLE_ATMEGA32

	#if CPUSTYLE_ATMEGA_XXX4

		// Для забывших выключить JTAG
		MCUCR = (1u << JTD);	// must write this bit to the desired value twice within four cycles to change its value.
		MCUCR = (1u << JTD);

		CLKPR = (1u << CLKPCE);	// CLKPCE is cleared by hardware four cycles after it is written or when CLKPS bits are written.
		CLKPR = 0x00;


	#else

		// Для забывших выключить JTAG
		MCUCSR = (1u << JTD);	// must write this bit to the desired value twice within four cycles to change its value.
		MCUCSR = (1u << JTD);

	#endif

#elif CPUSTYLE_ATXMEGA

	cpu_atxmega_switchto32MHz();

#elif CPUSTYLE_AT91SAM7S

	usb_disable();

#endif

#if CPUSTYLE_R7S721
	#if 1
		// Перенесено из systeminit
		// Не получается разместить эти функции во FLASH
		L1C_EnableCaches();
		L1C_EnableBTAC();
		//__set_ACTLR(__get_ACTLR() | ACTLR_L1PE_Msk);	// Enable Dside prefetch
		#if (__L2C_PRESENT == 1)
		  // Enable Level 2 Cache
		  L2C_Enable();
		#endif
	#endif

	/* TN-RZ*-A011A/E recommends switch off USB_X1 if usb USB not used */
	CPG.STBCR7 &= ~ CPG_STBCR7_MSTP70;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
	CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
	(void) CPG.STBCR7;			/* Dummy read */
	USB200.SYSCFG0 |= USB_SYSCFG_UCKSEL; // UCKSEL 1: The 12-MHz EXTAL clock is selected.
	(void) USB200.SYSCFG0;			/* Dummy read */
	local_delay_ms(2);	// required 1 ms delay - see R01UH0437EJ0200 Rev.2.00 28.4.1 System Control and Oscillation Control
	CPG.STBCR7 |= CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module halts.
	CPG.STBCR7 |= CPG_STBCR7_MSTP70;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module halts.

	CPG.STBCR7 &= ~ CPG_STBCR7_MSTP70;	// Module Stop 70 0: Channel 1 of the USB 2.0 host/function module runs.
	(void) CPG.STBCR7;			/* Dummy read */
	USB201.SYSCFG0 |= USB_SYSCFG_UCKSEL; // UCKSEL 1: The 12-MHz EXTAL clock is selected.
	(void) USB201.SYSCFG0;			/* Dummy read */
	local_delay_ms(2);	// required 1 ms delay - see R01UH0437EJ0200 Rev.2.00 28.4.1 System Control and Oscillation Control
	CPG.STBCR7 |= CPG_STBCR7_MSTP70;	// Module Stop 70 0: Channel 1 of the USB 2.0 host/function module halts.

#elif CPUSTYPE_TMS320F2833X

	EALLOW;
	WDCR = 0x0068;
	//
	/* Copy the ramfuncs section */
	if (1)
	{
		extern unsigned int RamfuncsLoadStart;
		extern unsigned int RamfuncsLoadEnd;
		extern unsigned int RamfuncsRunStart;

		memcpy(& RamfuncsRunStart, & RamfuncsLoadStart, (uint32_t) & RamfuncsLoadEnd - (uint32_t) & RamfuncsLoadStart);
	}
	/* Setup PLL and FLASH */
	#if (CPU_FREQ > 120000000UL)
		enum { FLASHWS = 5, OTPWS = 8 };
	#elif (CPU_FREQ > 100000000UL)
		enum { FLASHWS = 4, OTPWS = 7 };
	#else
		enum { FLASHWS = 3, OTPWS = 5 };
	#endif
	cpu_tms320f2833x_pll_initialize(1, 1);		// 20 MHz / 4 = 5 MHz
	#if (CPU_FREQ == 150000000UL)
		cpu_tms320f2833x_pll_initialize(15, 2);		// 20 MHz * 15 / 2 = 150 MHz	- 15 is illegal value
		cpu_tms320f2833x_flash_waitstates(5, 8);		// commented in RAM configuration
	#elif (CPU_FREQ == 100000000UL)
		cpu_tms320f2833x_pll_initialize(10, 2);		// 20 MHz * 10 / 2 = 100 MHz
		cpu_tms320f2833x_flash_waitstates(3, 5);		// commented in RAM configuration
	#endif

#endif /*  */

#if (__CORTEX_M != 0)

	// Таблица находится в области вне Data Cache
	vectors_relocate();
	arm_cpu_CMx_initialize_NVIC();

#elif defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U)

	arm_gic_initialize();

#endif /* CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7 */
	//PRINTF("cpu_initialize done\n");
}

