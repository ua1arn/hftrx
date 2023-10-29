/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef HARDWARE_H_INCLUDED
#define HARDWARE_H_INCLUDED

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if \
	defined (__AVR_ATmega32__) || defined (__AVR_ATmega32A__) || \
	defined (__AVR_ATmega16__) || defined (__AVR_ATmega16A__) || \
	0

	#define CPUSTYLE_ATMEGA	1		/* архитектура процессора */
	#define CPUSTYLE_ATMEGA32	1	/* Набор портов и расположение выводов - DIP40 или TQFP48 */

	#ifndef F_CPU
	#define F_CPU 8000000ul
	#endif

	#include <avr/io.h>			/* Pin manipulation functions */
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>		/* только для случая использования внутренней памяти процессора */
	#include <avr/interrupt.h>
	#include <avr/cpufunc.h>
	#include <util/delay.h>

	#define ALIGNX_BEGIN /* __attribute__ ((aligned(64)))  */
	#define ALIGNX_END /* nothing */

#elif \
	defined (__AVR_ATmega8__)  || \
	defined (__AVR_ATmega8A__)  || \
	0

	#define CPUSTYLE_ATMEGA	1		/* архитектура процессора */

	#ifndef F_CPU
	#define F_CPU 8000000ul
	#endif

	#include <avr/io.h>			/* Pin manipulation functions */
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>		/* только для случая использования внутренней памяти процессора */
	#include <avr/interrupt.h>
	#include <avr/cpufunc.h>
	#include <util/delay.h>

	#define ALIGNX_BEGIN /* __attribute__ ((aligned(64)))  */
	#define ALIGNX_END /* nothing */

#elif \
	defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__) || defined (__AVR_ATmega328A__) || \
	defined (__AVR_ATmega168P__) || defined (__AVR_ATmega168__) || defined (__AVR_ATmega168PA__) || defined (__AVR_ATmega168A__) || \
	defined (__AVR_ATmega48P__) || defined (__AVR_ATmega48__) || defined (__AVR_ATmega48PA__) || defined (__AVR_ATmega48A__) || \
	0

	#define CPUSTYLE_ATMEGA	1		/* архитектура процессора */
	#define CPUSTYLE_ATMEGA328	1
	#define CPUSTYLE_ATMEGA_XXX4 1

	#ifndef F_CPU
	#define F_CPU 8000000ul
	#endif

	#include <avr/io.h>			/* Pin manipulation functions */
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>		/* только для случая использования внутренней памяти процессора */
	#include <avr/interrupt.h>
	#include <avr/cpufunc.h>
	#include <util/delay.h>

	#define ALIGNX_BEGIN /* __attribute__ ((aligned(64)))  */
	#define ALIGNX_END /* nothing */

#elif \
	defined (__AVR_ATmega324P__) || defined (__AVR_ATmega324A__) || defined (__AVR_ATmega644__) || \
	defined (__AVR_ATmega644P__) || defined (__AVR_ATmega644PA__) || \
	defined (__AVR_ATmega1284P__) || \
	0

	#define CPUSTYLE_ATMEGA	1		/* архитектура процессора */
	#define CPUSTYLE_ATMEGA32	1	/* Набор портов и расположение выводов - DIP40 или TQFP48 */
	#define CPUSTYLE_ATMEGA_XXX4 1	// Atmega324, Atmega644 and so on

	#ifndef F_CPU
		#define F_CPU 10000000ul
	#endif

	#include <avr/io.h>			/* Pin manipulation functions */
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>		/* только для случая использования внутренней памяти процессора */
	#include <avr/interrupt.h>
	#include <avr/cpufunc.h>
	#include <util/delay.h>

	#define ALIGNX_BEGIN /* __attribute__ ((aligned(64)))  */
	#define ALIGNX_END /* nothing */

#elif \
	defined (__AVR_ATmega64__) || defined (__AVR_ATmega64A__) || \
	defined (__AVR_ATmega128__) || defined (__AVR_ATmega128A__) || \
	0

	#define CPUSTYLE_ATMEGA	1		/* архитектура процессора */
	#define CPUSTYLE_ATMEGA128	1	/* Набор портов и расположение выводов - TQFP64 */

	#ifndef F_CPU
	#define F_CPU 8000000ul
	#endif

	#include <avr/io.h>			/* Pin manipulation functions */
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>		/* только для случая использования внутренней памяти процессора */
	#include <avr/interrupt.h>
	#include <avr/cpufunc.h>
	#include <util/delay.h>

	#define ALIGNX_BEGIN /* __attribute__ ((aligned(64)))  */
	#define ALIGNX_END /* nothing */

#elif \
	defined (__AVR_ATxmega32A4__) || \
	defined (__AVR_ATxmega32A4U__) || \
	defined (__AVR_ATxmega128A4U__) || \
	0

	#define CPUSTYLE_ATXMEGA	1
	#define CPUSTYLE_ATXMEGAXXXA4	1
	
	#ifndef F_CPU
	#define F_CPU 32000000uL
	#endif

	#include <avr/io.h>			/* Pin manipulation functions */
	#include <avr/pgmspace.h>
	#include <avr/eeprom.h>		/* только для случая использования внутренней памяти процессора */
	#include <avr/interrupt.h>
	#include <avr/cpufunc.h>
	#include <util/delay.h>

	#define ALIGNX_BEGIN /* __attribute__ ((aligned(64)))  */
	#define ALIGNX_END /* nothing */

#elif CPUSTYLE_STM32H7XX
	// STM32H743ZIT6 processors, up to 400 MHz

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_STM32F		1		/* архитектура процессора STM32F */
	#define CPUSTYLE_ARM_CM7	1		/* архитектура процессора CORTEX M7 */

	#define CORE_CM7 1	/* for STM32H745 dual-core config */

	#include "arch/stm32h7xx/stm32h7xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(32)))
	#define ALIGNX_END /* nothing */

	//#define DCACHEROWSIZE __SCB_DCACHE_LINE_SIZE  //32U /*!< Cortex-M7 cache line size is fixed to 32 bytes (8 words). See also register SCB_CCSIDR */
	//#define ICACHEROWSIZE __SCB_ICACHE_LINE_SIZE  //32U /*!< Cortex-M7 cache line size is fixed to 32 bytes (8 words). See also register SCB_CCSIDR */
	#define DCACHEROWSIZE 32U /*!< Cortex-M7 cache line size is fixed to 32 bytes (8 words). See also register SCB_CCSIDR */
	#define ICACHEROWSIZE 32U /*!< Cortex-M7 cache line size is fixed to 32 bytes (8 words). See also register SCB_CCSIDR */

#elif CPUSTYLE_STM32F7XX
	// STM32F745ZGT6 processors, up to 216 MHz 

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_STM32F		1		/* архитектура процессора STM32F */
	#define CPUSTYLE_ARM_CM7	1		/* архитектура процессора CORTEX M7 */

	#include "arch/stm32f7xx/stm32f7xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(32)))
	#define ALIGNX_END /* nothing */

	//#define DCACHEROWSIZE __SCB_DCACHE_LINE_SIZE  //32U /*!< Cortex-M7 cache line size is fixed to 32 bytes (8 words). See also register SCB_CCSIDR */
	//#define ICACHEROWSIZE __SCB_ICACHE_LINE_SIZE  //32U /*!< Cortex-M7 cache line size is fixed to 32 bytes (8 words). See also register SCB_CCSIDR */
	#define DCACHEROWSIZE 32U /*!< Cortex-M7 cache line size is fixed to 32 bytes (8 words). See also register SCB_CCSIDR */
	#define ICACHEROWSIZE 32U /*!< Cortex-M7 cache line size is fixed to 32 bytes (8 words). See also register SCB_CCSIDR */

#elif CPUSTYLE_STM32F4XX

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_STM32F		1		/* архитектура процессора STM32F */
	#define CPUSTYLE_ARM_CM4	1		/* архитектура процессора CORTEX M4 */

	#include "arch/stm32f4xx/stm32f4xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(4)))
	#define ALIGNX_END /* nothing */

	//#define DCACHEROWSIZE __SCB_DCACHE_LINE_SIZE  //32U /*!< Cortex-M7 cache line size is fixed to 32 bytes (8 words). See also register SCB_CCSIDR */
	//#define ICACHEROWSIZE __SCB_ICACHE_LINE_SIZE  //32U /*!< Cortex-M7 cache line size is fixed to 32 bytes (8 words). See also register SCB_CCSIDR */
	#define DCACHEROWSIZE 16U /*!< Cortex-M7 cache line size is fixed to 32 bytes (8 words). See also register SCB_CCSIDR */
	#define ICACHEROWSIZE 16U /*!< Cortex-M7 cache line size is fixed to 32 bytes (8 words). See also register SCB_CCSIDR */

#elif CPUSTYLE_STM32F30X

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_STM32F		1		/* архитектура процессора STM32F */
	#define CPUSTYLE_ARM_CM4	1		/* архитектура процессора CORTEX M3 */

	// STM32F303VC processors
	#include "arch/stm32f30x/stm32f30x.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(4)))
	#define ALIGNX_END /* nothing */

#elif CPUSTYLE_STM32F1XX

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_STM32F		1		/* архитектура процессора STM32F */
	#define CPUSTYLE_ARM_CM3	1		/* архитектура процессора CORTEX M3 */

	#include "arch/stm32f1xx/stm32f1xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(4)))
	#define ALIGNX_END /* nothing */

elif CPUSTYLE_STM32F0XX

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_STM32F		1		/* архитектура процессора STM32F */
	#define CPUSTYLE_ARM_CM0	1		/* архитектура процессора CORTEX M0 */
	
	#include "arch/stm32f0xx/stm32f0xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(4)))
	#define ALIGNX_END /* nothing */

#elif CPUSTYLE_STM32L0XX

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_STM32F		1		/* архитектура процессора STM32F */
	#define CPUSTYLE_ARM_CM0	1		/* архитектура процессора CORTEX M0 */
	#include "arch/stm32l0xx/stm32l0xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(4)))
	#define ALIGNX_END /* nothing */

#elif CPUSTYLE_ATSAM3S

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_ARM_CM3	1		/* архитектура процессора CORTEX M3 */

	#define __SAM3S4C__ 1		// replace to __SAM3S4B__
	#include "sam3s.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

#elif CPUSTYLE_ATSAM4S

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_ARM_CM4	1		/* архитектура процессора CORTEX M3 */

	#define __SAM4SA16C__ 1
	#include "sam4s.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

#elif CPUSTYLE_AT91SAM7S

	#define CPUSTYLE_ARM		1		/* архитектура процессора ARM */
	#define	CPUSTYLE_ARM7TDMI	1
	#include "arch/at91sam/at91sam7s64.h"

	#define __ASM            __asm           /*!< asm keyword for GNU Compiler          */
	#define __INLINE         inline          /*!< inline keyword for GNU Compiler       */
	#define __STATIC_INLINE  static inline

	#include "arch/tdmi7_gcc.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

#elif CPUSTYLE_AT91SAM9XE

	// ARM926EJ-S ARMv5TEJ
	// Use -mcpu=arm926ej-s -mfloat-abi=soft
	// arm-none-eabi-gcc.exe -mcpu=arm926ej-s -mfloat-abi=soft -E -dM  -c -o tt.o tt.c

	#define CPUSTYLE_ARM		1		/* архитектура процессора ARM */
	#define	CPUSTYLE_ARM9		1

	#include "arch/at91sam/at91sam9xe512.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

	#define __ASM            __asm           /*!< asm keyword for GNU Compiler          */
	#define __INLINE         inline          /*!< inline keyword for GNU Compiler       */
	#define __STATIC_INLINE  static inline

	#include "arch/tdmi7_gcc.h"

#elif CPUSTYLE_R7S721

	// Renesas CPU
	#define CPUSTYLE_ARM		1		/* архитектура процессора ARM */

	/* MCU Lineup */
	#define TARGET_RZA1LC           (0x00200000)
	#define TARGET_RZA1L            (0x00300000)
	#define TARGET_RZA1LU           (0x00300010)
	#define TARGET_RZA1M            (0x00500000)
	#define TARGET_RZA1H            (0x00A00000)

	//#define TARGET_RZA1             (TARGET_RZA1L)	// перенесено в Makefile

	#include "arch/renesasRZA1/Renesas_RZ_A1.h"	// IRQs, some bitss
	#include "arch/renesasRZA1/iodefine.h"		// peripherial registers
	#include "arch/renesasRZA1/rza_io_regrw.h"	// iobitmasks

	#include "irq_ctrl.h"


	#define DCACHEROWSIZE 32
	#define ICACHEROWSIZE 32

	#define ALIGNX_BEGIN __attribute__ ((aligned(DCACHEROWSIZE)))
	#define ALIGNX_END /* nothing */

#elif CPUSTYLE_STM32MP1

	// ST dual core A7 + M4

	// STM32MP157Axx
	// STM32MP157Dxx
	// STM32MP157AAB3
	// STM32MP157DAB1

	#define CORE_CA7	1
	#define CPUSTYLE_ARM		1		/* архитектура процессора ARM */

	#include "arch/stm32mp1xx/stm32mp1xx.h"
	#include "irq_ctrl.h"

	#define DCACHEROWSIZE 64
	#define ICACHEROWSIZE 32

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

#elif CPUSTYLE_F133

	// RISC-V processor Allwinner D1 XuanTie C906

	#define CPUSTYLE_RISCV		1		/* архитектура процессора RISC-V */
	#define CPUSTYLE_ALLWINNER	1		/* */

	#include "allwnr_f133.h"
	//#include "irq_ctrl.h"

	#define DCACHEROWSIZE 64
	#define ICACHEROWSIZE 64

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

#elif CPUSTYLE_A64

	#define CPUSTYLE_ARM		1		/* архитектура процессора ARM */
	#define CPUSTYLE_ALLWINNER	1		/* */

	#include "allwnr_a64.h"
	#include "irq_ctrl.h"

	#define DCACHEROWSIZE 64
	#define ICACHEROWSIZE 32

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */


#elif CPUSTYLE_T507

	#define CPUSTYLE_ARM		1		/* архитектура процессора ARM */
	#define CPUSTYLE_ALLWINNER	1		/* */

	#include "allwnr_t507.h"
	#include "irq_ctrl.h"

	//	I-Cache L1: 32 KB, 256 Sets, 64 Bytes/Line, 2-Way
	//	D-Cache L1: 32 KB, 128 Sets, 64 Bytes/Line, 4-Way
	//	Unified-Cache L2: 1024 KB, 1024 Sets, 64 Bytes/Line, 16-Way

	#define DCACHEROWSIZE 64
	#define ICACHEROWSIZE 64

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

#elif CPUSTYLE_V853X

	#define CPUSTYLE_ARM		1		/* архитектура процессора ARM */
	#define CPUSTYLE_ALLWINNER	1		/* */

	#include "allwnr_v853x.h"
	#include "irq_ctrl.h"

	#define DCACHEROWSIZE 64
	#define ICACHEROWSIZE 32

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

#elif CPUSTYLE_T113

	#define CPUSTYLE_ARM		1		/* архитектура процессора ARM */
	#define CPUSTYLE_ALLWINNER	1		/* */

	#include "allwnr_t113s3.h"
	#include "irq_ctrl.h"

	#define DCACHEROWSIZE 64
	#define ICACHEROWSIZE 32

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

#elif CPUSTYLE_VM14

	// 1892ВМ14Я ELVEES multicore.ru
	#define CPUSTYLE_ARM		1		/* архитектура процессора ARM */

	#include "elvees_vm14.h"
	#include "irq_ctrl.h"

	#define DCACHEROWSIZE 32
	#define ICACHEROWSIZE 32

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

#elif CPUSTYLE_XC7Z

	// Zynq®-7000 SoC Family

	// XC7Z010
	// XC7Z015
	// XC7Z020

	#define CPUSTYLE_ARM		1		/* архитектура процессора ARM */

#if ! LINUX_SUBSYSTEM
	#include "arch/zynq/zynq7000.h"
	#include "irq_ctrl.h"
#endif /* ! LINUX_SUBSYSTEM */

	#define DCACHEROWSIZE 32
	#define ICACHEROWSIZE 32

	#define ALIGNX_BEGIN __attribute__ ((aligned(32)))
	#define ALIGNX_END /* nothing */

#elif \
	defined (CPUSTYLE_UBLAZE) || \
	0

	#include "u_blaze.h"

	#define DCACHEROWSIZE 64
	#define ICACHEROWSIZE 32

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */
	//#define CPU_FREQ 150000000uL
	#define CPU_FREQ 100000000uL
	//#define CPU_FREQ 5000000uL		// Частота без инициализаwии PLL - xtal / 4

#elif \
	defined (__TMS320C28X__) || \
	0

#else 

	#error Not handled processor definition (GCC argument -m=XXXXX)
	#error In Makefile not defined CPUSTYLE_XXX

#endif

/* По значению маски получить значение младшего бита, входящего в неё. */
#define MASK2LSB(mask)  ((mask) & (0 - (mask)))

/* макрооределение для расчёта количества "тиков" для выдержки величин задержек. */
#define NTICKS(t_mS) ((uint_fast16_t) (((uint_fast32_t) (t_mS) * TICKS_FREQUENCY + 500) / 1000))
#define NTICKSADC(t_uS) ((uint_fast16_t) (((uint_fast32_t) (t_uS) * ADC_FREQ + 5) / 1000000))
#define NTICKSADC01(t_uS01) ((uint_fast16_t) (((uint_fast32_t) (t_uS01) * ADC_FREQ + 5) / 10000000))


void cpu_initialize(void);
void arm_hardware_reset(void);
void cpu_initdone(void);	// секция init больше не нужна
uint_fast32_t cpu_getdebugticks(void);	// получение из аппаратного счетчика монотонно увеличивающегося кода
uint_fast32_t cpu_getdebugticksfreq(void);	// получение частоты, с которой инкрементируется счетчик

void tickers_initialize(void);
void spool_systimerbundle1(void);
void spool_systimerbundle2(void);
void spool_elkeybundle(void);
void sysinit_pll_initialize(int forced);	// PLL initialize
void sysinit_gpio_initialize(void);	// временная подготовка к работе с gpio.
void sysinit_pmic_initialize(void);	// инициализация контроллера питания (не только DDR память. бывает и GPIO)
void hardware_adc_startonescan(void);
void stm32mp1_pll1_slow(uint_fast8_t slow);

void hardware_timer_initialize(uint_fast32_t ticksfreq);

void spool_encinterrupt(void);	/* прерывание по изменению сигнала на входах от валкодера */
void spool_encinterrupt2(void);	/* прерывание по изменению сигнала на входах от валкодера #2 */
void hardware_encoder_initialize(void);
uint_fast8_t hardware_get_encoder_bits(void);/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */
uint_fast8_t hardware_get_encoder2_bits(void);/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */

void gt911_interrupt_handler(void);
void stmpe811_interrupt_handler(void);

void hardware_adc_initialize(void);

void watchdog_initialize(void);	/* разрешение сторожевого таймера в устройстве */
void watchdog_ping(void);	/* перезапуск сторожевого таймера */


#if CPUSTYLE_ARM

	/* тип для передачи параметра "адрес устройства на SPI шине" */
	/* это может быть битовая маска в порту ввода-вывода */
	typedef uint_fast32_t spitarget_t;		
	/* тип для хранения данных, считанных из порта ввода-вывода или для формируемого значения */
	typedef uint_fast32_t portholder_t;		

	#define FLASHMEM //__flash
	#define NOINLINEAT // __attribute__((noinline))
	#define strlen_P(s) strlen(s)

	#define ATTRWEAK __WEAK
	// Use __attribute__ ((weak, alias("Default_Handler")))

	#define PSTR(s) (s)
	//#define PSTR(s) (__extension__({static const char __c[] FLASHMEM = (s); &__c[0];}))

	void local_delay_us(int timeUS);
	void local_delay_ms(int timeMS);

#elif CPUSTYLE_RISCV

	/* тип для передачи параметра "адрес устройства на SPI шине" */
	/* это может быть битовая маска в порту ввода-вывода */
	typedef uint_fast32_t spitarget_t;
	/* тип для хранения данных, считанных из порта ввода-вывода или для формируемого значения */
	typedef uint_fast32_t portholder_t;

	#define FLASHMEM //__flash
	#define NOINLINEAT // __attribute__((noinline))
	#define strlen_P(s) strlen(s)

	#define ATTRWEAK __WEAK
	// Use __attribute__ ((weak, alias("Default_Handler")))

	#define PSTR(s) (s)
	//#define PSTR(s) (__extension__({static const char __c[] FLASHMEM = (s); &__c[0];}))

	void local_delay_us(int timeUS);
	void local_delay_ms(int timeMS);

#elif CPUSTYLE_ATMEGA || CPUSTYLE_ATXMEGA
	// ATMega, ATXMega target

	/* тип для передачи параметра "адрес устройства на SPI шине" */
	/* это может быть битовая маска в порту ввода-вывода */
	typedef uint_fast8_t spitarget_t;
	/* тип для хранения данных, считанный из порта ввода-вывода или для формируемого значения */
	typedef uint_fast8_t portholder_t;		

	#define local_delay_us(t) do { if ((t) <= 1) _delay_us(0); else if ((t <= 10)) _delay_us(10); else _delay_us(100); } while (0)
	#define local_delay_ms(t) do { if ((t) <= 1) _delay_ms(0); else if ((t <= 10)) _delay_ms(10); else _delay_ms(100); } while (0)
 
	#if (FLASHEND > 0x7FFF)	
		// нет нужды экономить память FLASH
		#define NOINLINEAT // __attribute__((noinline))
		#define RAMFUNC_NONILINE // __attribute__((noinline))
		#define RAMFUNC // __attribute__((__section__(".ramfunc"), noinline))
	#else
		#define NOINLINEAT __attribute__((noinline))	// On small FLASH ATMega CPUs
		#define RAMFUNC_NONILINE __attribute__((noinline))	// On small FLASH ATMega CPUs
		#define RAMFUNC			 // __attribute__((__section__(".ramfunc")))
	#endif

	#define ATTRWEAK __attribute__ ((weak))
	#define __WEAK __attribute__ ((weak))
	#define __NO_RETURN	__attribute__((__noreturn__))

	/* stubs */

	#define IRQL_SYSTEM 			1
	#define IRQL_REALTIME 			1
	#define IRQL_OVERREALTIME 		1

#elif CPUSTYLE_TMS320F2833X
	/* тип для передачи параметра "адрес устройства на SPI шине" */
	/* это может быть битовая маска в порту ввода-вывода */
	typedef uint_fast32_t spitarget_t;
	/* тип для хранения данных, считанный из порта ввода-вывода или для формируемого значения */
	typedef uint_fast32_t portholder_t;		

	#define global_enableIRQ() do { asm(" NOP"); } while (0)
	#define global_disableIRQ() do { asm(" NOP"); } while (0)

	#define ATTRWEAK __attribute__ ((weak))

#elif CPUSTYLE_UBLAZE
	/* тип для передачи параметра "адрес устройства на SPI шине" */
	/* это может быть битовая маска в порту ввода-вывода */
	typedef uint_fast32_t spitarget_t;
	/* тип для хранения данных, считанный из порта ввода-вывода или для формируемого значения */
	typedef uint_fast32_t portholder_t;

	typedef uint_fast32_t adcvalholder_t;
	typedef int_fast32_t sadcvalholder_t;	// для хранения знаковых значений

	#define FLASHMEM //__flash
	#define NOINLINEAT // __attribute__((noinline))
	#define strlen_P(s) strlen(s)
	#define PSTR(s) s

	#define ATTRWEAK __WEAK
	#define __NO_RETURN

	// Use __attribute__ ((weak, alias("Default_Handler")))
	void local_delay_us(int timeUS);
	void local_delay_ms(int timeMS);

#else
	#error Undefined CPUSTYLE_xxxx
#endif

// Sample usage:
///* все возможные в данной конфигурации фильтры */
//#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_1P8 | IF3_FMASK_2P7 | IF3_FMASK_3P1)

void spool_nmeapps(void);	// Обработчик вызывается при приходе очередного импульса PPS

// получить прескалер и значение для программирования таймера
uint_fast8_t
hardware_calc_sound_params(
	uint_least16_t tonefreq01,	/* tonefreq - частота в десятых долях герца. Минимум - 400 герц (определено набором команд CAT). */
	unsigned * pvalue);
// установить прескалер и значение для программирования таймера
// called from interrupt or with disabled interrupts
void hardware_sounds_setfreq(
	uint_fast8_t prei,
	unsigned value
	);
// выключаем звук
void hardware_sounds_disable(void);
/* вызывается при запрещённых прерываниях. */
void hardware_beep_initialize(void);

/* SAI, I2S и подключенная на них периферия */
void hardware_channels_initialize(void);
void hardware_channels_enable(void);

void hardware_dac_initialize(void);		/* инициализация DAC на STM32F4xx */
void hardware_dac_ch1_setvalue(uint_fast16_t v);	// вывод 12-битного значения на ЦАП - канал 1
void hardware_dac_ch2_setvalue(uint_fast16_t v);	// вывод 12-битного значения на ЦАП - канал 2


void hardware_spi_io_delay(void);	/* распространение сигналов до slave */

/* отладочная выдача через USB CDC */
void debugusb_initialize(void);				/* Вызывается из user-mode программы при запрещённых прерываниях. */
uint_fast8_t debugusb_putchar(uint_fast8_t c);/* передача символа если готов порт */
uint_fast8_t debugusb_getchar(char * cp); /* приём символа, если готов порт */
void debugusb_parsechar(uint_fast8_t c);	/* вызывается из обработчика прерываний */
void debugusb_sendchar(void * ctx);			/* вызывается из обработчика прерываний */

/* TWI (I2C) interface */
#if ! LINUX_SUBSYSTEM
	#define I2C_RETRIES 3	/* количество повторов */
#endif /* ! LINUX_SUBSYSTEM */

/* target device speed */
typedef struct i2c_parameters
{
	uint8_t ch;
	uint8_t usdelayv;	/* время задержки на полпериода скрости обмпена при программном формировании I2C */
} i2cp_t;

#define I2CP_I2C1	0	/* i2c_xxx */
#define I2CP_I2C2	1	/* i2c2_xxx */

void i2cp_intiialize(i2cp_t * p, unsigned ch, unsigned freq);	/* канал и скорость обмена */

#define I2C_READ_ACK 0  // i2c_read parameter
#define I2C_READ_ACK_1 1  // i2c_read parameter
#define I2C_READ_NACK 2		// ack_type - last parameterr in read block
#define I2C_READ_ACK_NACK 3		// чтение первого и единственного байта по I2C

void i2c_initialize(void);
void i2c_start(uint_fast8_t address);
void i2c_read(uint8_t * pdata, uint_fast8_t acknak);
void i2c_write(uint_fast8_t data);
void i2c_write_withrestart(uint_fast8_t data);	// запись, после чего restart
void i2c_waitsend(void);	// Вызвать после последнего i2c_write()
void i2c_stop(void);

// Работа со вторым каналом I2C
void i2c2_start(uint_fast8_t address);
void i2c2_read(uint8_t * pdata, uint_fast8_t acknak);
void i2c2_write(uint_fast8_t data);
void i2c2_write_withrestart(uint_fast8_t data);	// запись, после чего restart
void i2c2_waitsend(void);	// Вызвать после последнего i2c_write()
void i2c2_stop(void);

void i2cp_i2c_initialize(void);

/* Версии функций с указанием скорости и порта I2C */
void i2cp_start(const i2cp_t * p, uint_fast8_t address);
void i2cp_read(const i2cp_t * p, uint8_t * pdata, uint_fast8_t acknak);
void i2cp_write(const i2cp_t * p, uint_fast8_t data);
void i2cp_write_withrestart(const i2cp_t * p, uint_fast8_t data);	// запись, после чего restart
void i2cp_waitsend(const i2cp_t * p);	// Вызвать после последнего i2c_write()
void i2cp_stop(const i2cp_t * p);

void hardware_twi_master_configure(void);

/* return non-zero then error */
uint16_t i2chw_read(uint16_t slave_address, uint8_t * buf, uint32_t size);
uint16_t i2chw_write(uint16_t slave_address, const uint8_t * buf, uint32_t size);

uint32_t hardware_get_random(void);

void arm_hardware_dma2d_initialize(void);	// Graphic 2D engine
void arm_hardware_mdma_initialize(void);	// Graphic 2D engine
void arm_hardware_sdram_initialize(void);	// External memory region(s)


typedef struct videomode_tag
{
	unsigned width; 		/* LCD pixel width            */
	unsigned height; 		/* LCD pixel height           */
	unsigned hsync; 		/* horizontal synchronization */
	unsigned hbp; 			/* horizontal back porch      */
	unsigned hfp; 			/* horizontal front porch     */

	unsigned vsync; 		/* vertical synchronization   */
	unsigned vbp; 			/* vertical back porch        */
	unsigned vfp; 			/* vertical front porch       */

	// mode: de/sync mode select.
	// de mode: mode="1", vs and hs must pull high.
	// sync mode: mode="0". de must be grounded
	unsigned vsyncneg; 		/* negative polarity required for vsync signal */
	unsigned hsyncneg; 		/* negative polarity required for hsync signal */
	unsigned deneg; 		/* negative de polarity: (normal: de is 0 while sync) */
	unsigned lq43reset; /* требуется формирование сигнала RESET для панели по этому выводу после начала формирования синхронизации */
	unsigned fps;	/* frames per second */

} videomode_t;

extern const videomode_t vdmode0;
void hardware_ltdc_initialize(const uintptr_t * frames, const videomode_t * vdmode);	// LCD-TFT Controller (LTDC) with framebuffer
uint_fast32_t display_getdotclock(const videomode_t * vdmode);

void hardware_ltdc_main_set(uintptr_t addr);	/* Set MAIN frame buffer address. Wait for VSYNC. */
void hardware_ltdc_main_set4(uintptr_t layer0, uintptr_t layer1, uintptr_t layer2, uintptr_t layer3);	/* Set MAIN frame buffer address. Waiting for VSYNC. */
void hardware_ltdc_main_set_no_vsync(uintptr_t addr);	/* Set MAIN frame buffer address. No waiting for VSYNC. */
void hardware_ltdc_pip_set(uintptr_t addr);	// Set PIP frame buffer address.
void hardware_ltdc_pip_off(void);	// Turn PIP off (main layer only).
void hardware_ltdc_L8_palette(void);	// Palette reload

uint_fast8_t usbd_cdc1_getrts(void);
uint_fast8_t usbd_cdc1_getdtr(void);

uint_fast8_t usbd_cdc2_getrts(void);
uint_fast8_t usbd_cdc2_getdtr(void);

void AT91F_PIOA_IRQHandler(void);
void AT91F_ADC_IRQHandler(void);

void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI5_IRQHandler(void);
void EXTI6_IRQHandler(void);
void EXTI7_IRQHandler(void);
void EXTI8_IRQHandler(void);
void EXTI9_IRQHandler(void);
void EXTI10_IRQHandler(void);
void EXTI11_IRQHandler(void);
void EXTI12_IRQHandler(void);
void EXTI13_IRQHandler(void);
void EXTI14_IRQHandler(void);
void EXTI15_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

void SDIO_IRQHandler(void);
void SDMMC1_IRQHandler(void);
void DMA2_Stream6_IRQHandler(void);

void SDIO0_IRQHandler(void);	// ZYNQ

void SysTick_Handler(void);
void Reset_CPUn_Handler(void);

void ADC_IRQHandler(void);
void ADC1_IRQHandler(void);
void ADC2_IRQHandler(void);
void ADC3_IRQHandler(void);
void ADC1_2_IRQHandler(void);
void ADC1_COMP_IRQHandler(void);
void r7s721_adi_irq_handler(void);
void TIM3_IRQHandler(void);
void TIM5_IRQHandler(void);

void GPU_IRQHandler(void);	// STM32MP1 specific

void r7s721_intc_initialize(void);

/* Отображение номеров каналов АЦП процессоров STM32Fxxx в каналы процессора STM32H7 */
typedef struct adcinmap_tag
{
	uint_fast8_t ch;	// номер канала в периферийном блоке процессора
#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
	ADC_TypeDef * adc;	// периферийный блок процессора
	ADC_Common_TypeDef * adccommon;
#endif /* CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1 */
	uint_fast8_t thold_uS01;	// минимальное время выборки для данного канала - десятые доли микросекунды
} adcinmap_t;

const adcinmap_t * getadcmap(uint_fast8_t adci);

void spool_adcdonebundle(void);
void adcdones_initialize(void);
uint_fast8_t isadchw(uint_fast8_t adci); // Проверка что индекс входа АЦП относится ко встроенной периферии процессора


void cpump_initialize(void);
void cpump_runuser(void);	/* остальным ядрам разрешаем выполнять прерывания */

void cpu_stm32f1xx_setmapr(unsigned long bits);

void hardware_tim21_initialize(void);

void hardware_dcdcfreq_tim16_ch1_initialize(void);
void hardware_dcdcfreq_tim17_ch1_initialize(void);
void hardware_dcdcfreq_pwm_initialize(unsigned pwmch);	/* Allwinner */
void hardware_dcdcfreq_tioc0a_mtu0_initialize(void);

void hardware_dcdcfreq_tim16_ch1_setdiv(uint_fast32_t v);
void hardware_dcdcfreq_tim17_ch1_setdiv(uint_fast32_t v);
void hardware_dcdcfreq_pwm_setdiv(unsigned pwmch, uint_fast32_t cycle);	/* Allwinner */
void hardware_dcdcfreq_tioc0a_mtu0_setdiv(uint_fast32_t v);

uint_fast32_t hardware_dcdc_calcdivider(uint_fast32_t freq);


void hardware_sdhost_initialize(void);
void hardware_sdhost_setspeed(unsigned long ticksfreq);
void hardware_sdhost_setbuswidth(uint_fast8_t use4bit);
void hardware_sdhost_detect(uint_fast8_t present);		// в ответ на прерывание изменения состояния card detect

void lowtests(void);
void midtests(void);
void hightests(void);
void looptests(void);	// Периодически вызывается в главном цикле

void dcache_invalidate(uintptr_t base, int_fast32_t size);	// Сейчас в эту память будем читать по DMA
void dcache_clean(uintptr_t base, int_fast32_t size);	// Сейчас эта память будет записываться по DMA куда-то
void dcache_clean_invalidate(uintptr_t base, int_fast32_t size);	// Сейчас эта память будет записываться по DMA куда-то. Потом содержимое не требуется
void dcache_clean_all(void);

void r7s721_sdhi0_dma_handler(void);

uint_fast32_t 
calcdivround2(
	uint_fast32_t ref,	/* частота на входе делителя, в герцах. */
	uint_fast32_t freq	/* требуемая частота на выходе делителя, в герцах. */
	);

#include "mslist.h"

enum ticker_mode
{
	TICKERMD_PERIODIC,
	TICKERMD_MANUAL
};
typedef struct ticker_tag
{
	VLIST_ENTRY item;
	unsigned period;
	//unsigned fired;
	unsigned ticks;		// текущее количество тиков
	void (* cb)(void *);
	void * ctx;
	enum ticker_mode mode;
} ticker_t;

void ticker_initialize(ticker_t * p, unsigned nticks, void (* cb)(void *), void * ctx);
void ticker_initialize_ext(ticker_t * p, unsigned nticks, void (* cb)(void *), void * ctx, enum ticker_mode mode);
void ticker_setperiod(ticker_t * p, unsigned nticks);	/* изменение периода запущенного тикера */
void ticker_start(ticker_t * p);	/* начало интервала в случае TICKERMD_MANUAL */
void ticker_add(ticker_t * p);
void ticker_del(ticker_t * p);

typedef struct adcdone_tag
{
	VLIST_ENTRY item;
	void (* cb)(void *);
	void * ctx;
} adcdone_t;

void adcdone_initialize(adcdone_t * p, void (* cb)(void *), void * ctx);
void adcdone_add(adcdone_t * p);
void adcdone_del(adcdone_t * p);

void bootloader_deffereddetach(void * arg);
int bootloader_withusb(void);

// targetadc2 - on-board ADC MCP3208-BI/SL chip select (potentiometers)
// targetadck - on-board ADC MCP3208-BI/SL chip select (KEYBOARD)
// targetxad2 - external SPI device (PA BOARD ADC)

#define BOARD_ADCX0BASE 24	// on-board ADC base index - targetadc2
#define BOARD_ADCXKBASE 32	// on-board ADC base index (kbd, for example) - targetadck
#define BOARD_ADCX1BASE 40	// PA board ADC base index = targetxad2
#define BOARD_ADCMRRBASE 48	// mirror - значения АЦП устанавливабтся выходами программных компонентов, без считывания с аппаратуры.

#define HARDWARE_ADCINPUTS	72	/* до 16-ти входов АЦП на каждый тип источников */

#define BOARD_ADCXIN(ch)  (BOARD_ADCX0BASE + (ch))
#define BOARD_ADCX1IN(ch) (BOARD_ADCX0BASE + (ch))
#define BOARD_ADCXKIN(ch) (BOARD_ADCXKBASE + (ch))
#define BOARD_ADCX2IN(ch) (BOARD_ADCX1BASE + (ch))
#define BOARD_ADCMRRIN(ch) (BOARD_ADCMRRBASE + (ch))

// Cortex-A7/A9 handlers
void Undef_Handler(void);
void SWI_Handler(void);
void PAbort_Handler(void);
void DAbort_Handler(void);
void FIQ_Handler(void);
void Hyp_Handler(void);

void IRQ_Handler_GICv1(void);
void IRQ_Handler_GICv2(void);
void IRQ_Handler(void);			// No GIC

void Reset_CPUn_Handler(void);

// Set interrupt vector wrappers
void arm_hardware_set_handler(uint_fast16_t int_id, void (* handler)(void), uint_fast8_t priority, uint_fast8_t targetcpu);
void arm_hardware_set_handler_overrealtime(uint_fast16_t int_id, void (* handler)(void));
void arm_hardware_set_handler_realtime(uint_fast16_t int_id, void (* handler)(void));
void arm_hardware_set_handler_system(uint_fast16_t int_id, void (* handler)(void));
void arm_hardware_disable_handler(uint_fast16_t int_id);
uint_fast8_t arm_hardware_cpuid(void);	// This processor index (0..n-1)
uint_fast8_t arm_hardware_clustersize(void);
void arm_hardware_populte_second_initialize(void);

void audioproc_spool_user(void);	// вызывать при выполнении длительных операций

void hardware_set_dotclock(unsigned long dotfreq);
unsigned long hardware_get_dotclock(unsigned long dotfreq);
void hardware_nonguiyield(void);
uint_fast8_t stm32mp1_overdrived(void);	// return 1 if CPU supports 800 MHz clock

int toshiba_ddr_power_init(void);
void stpmic1_dump_regulators(void);

void network_initialize(void);
void init_netif(void);

void usb_polling(void);	/* LWIP support */
void sys_check_timeouts(void);	/* LWIP support */
void network_spool(void);

#define USBALIGN_BEGIN
#define USBALIGN_END __attribute__ ((aligned (64))) /* nothing */

//#define UNUSED(x) ((void)(x))

#define AUDIORECBUFFSIZE16 (16384)	// размер данных должен быть не меньше размера кластера на SD карте


#define  ARRAY_SIZE(a)  (sizeof (a) / sizeof (a) [0])


#define HWADDR                          0x30,0x89,0x84,0x6A,0x96,0x34

extern uint8_t myIP [4];
extern uint8_t myNETMASK [4];
extern uint8_t myGATEWAY [4];

#if CPUSTYLE_XC7Z

#define AX_PWM_AXI_SLV_REG0_OFFSET 0
#define AX_PWM_AXI_SLV_REG1_OFFSET 4
#define AX_PWM_AXI_SLV_REG2_OFFSET 8
#define AX_PWM_AXI_SLV_REG3_OFFSET 12

#define AX_PWM_mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

#define AX_PWM_mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

void xcz_dcdc_sync(uint32_t freq);
void xc7z_hardware_initialize(void);
float xc7z_get_cpu_temperature(void);
uint_fast8_t xc7z_readpin(uint8_t pin);
void xc7z_writepin(uint8_t pin, uint8_t val);
void xc7z_gpio_input(uint8_t pin);
void xc7z_gpio_output(uint8_t pin);

void xcz_rxtx_state(uint8_t tx);
void xcz_ah_preinit(void);

void xcz_audio_rx_init(void);
void xcz_audio_tx_init(void);
void xcz_audio_rx_enable(uint_fast8_t state);
void xcz_audio_tx_enable(uint_fast8_t state);

void xcz_if_rx_init(void);
void xcz_if_tx_init(void);
void xcz_if_rx_enable(uint_fast8_t state);
void xcz_if_tx_enable(uint_fast8_t state);
void xcz_adcrand_set(uint8_t val);

void xcz_dds_ftw(const uint_least64_t * value);	// Установка центральной частоты тракта основного приёмника
void xcz_dds_ftw_sub(const uint_least64_t * value);// Установка центральной частоты тракта дополнительного приёмника
void xcz_dds_rts(const uint_least64_t * value);// Установка центральной частоты панорамного индикатора

#endif /* CPUSTYLE_XC7Z */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#if 1
	#include "product.h"
#else
	#include "boards/arm_stm32h7xx_tqfp100_ctlstyle_storch_v7z_vt.h"	// rmainunit_v5km0.pcb, rmainunit_v5km1.pcb STM32H743IIT6, TFT 4.3", 2xmini-USB, mini SD-CARD, NAU8822L и FPGA EP4CE22E22I7N
	#include "paramdepend.h"							/* проверка зависимостей параметров конфигурации */
	#include "boards/arm_stm32h7xx_tqfp100_cpustyle_storch_v7z_vt.h"	// Rmainunit_v5l.pcb (mini USBx2, wide display interface) - mini RX
#endif

#include "taildefs.h"
#include "radio.h"	/* Определения, специфические для устройств, относящихся к радиосвязи. */
#include "clocks.h"
uint32_t sys_now(void);
uint32_t board_millis(void);	// tinyusb
int  sys_dram_init(void);	// 0 - error

// RTOS test stuff
int blinky_main(void);
#define GTIM_IRQ_NUM SecurePhysicalTimer_IRQn

struct menudef;
void
//NOINLINEAT
processmessages(
	uint_fast8_t * kbch,
	uint_fast8_t * kbready,
	uint_fast8_t inmenu,
	const FLASHMEM struct menudef * mp
	);


uintptr_t getRamDiskBase(void);
size_t getRamDiskSize(void);

void lowinitialize(void);
void applowinitialize(void);
void initialize2(void);
void application_mainloop(void);
void application_initialize(void);

#if LINUX_SUBSYSTEM
	#include <src/linux/linux_subsystem.h>
#endif /* LINUX_SUBSYSTEM */

#if (__CORTEX_A == 53U) && 1

// DDI0500J_cortex_a53_r0p4_trm.pdf

// 4.5.76 CPU Auxiliary Control Register

/** \brief  Get CPUACTLR
    \return                CPU Auxiliary Control Register
 */
__STATIC_FORCEINLINE uint64_t __get_CPUACTLR(void)
{
	uint64_t result;
  __get_CP64(15, 0, result, 15);
  return(result);
}

/** \brief  Set CPUACTLR
    \param [in]    cpuactlr   CPU Auxiliary Control Register
 */
__STATIC_FORCEINLINE void __set_CPUACTLR(uint64_t cpuactlr)
{
	__set_CP64(15, 0, cpuactlr, 15);
}

// 4.5.77 CPU Extended Control Register

#define CPUECTLR_SMPEN_Msk (1u << 6)	// SMPEN 1: Enables data coherency with other cores in the cluster.

/** \brief  Get CPUECTLR
    \return               CPU Extended Control Register
 */
__STATIC_FORCEINLINE uint64_t __get_CPUECTLR(void)
{
	uint64_t result;
  __get_CP64(15, 1, result, 15);
  return(result);
}

/** \brief  Set CPUECTLR
    \param [in]    cpuectlr  CPU Extended Control Register
 */
__STATIC_FORCEINLINE void __set_CPUECTLR(uint64_t cpuectlr)
{
	__set_CP64(15, 1, cpuectlr, 15);
}

#endif /* (__CORTEX_A == 53U) */

#include "utils.h"
// Substitutions for t507 ddr ram init
//#define i2c_read local_i2c_read
//#define i2c_write local_i2c_write

#endif // HARDWARE_H_INCLUDED
