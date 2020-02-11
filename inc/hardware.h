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
//#include <assert.h>

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

	#include "armcpu/stm32h7xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

	#include "arm_math.h"
	#include "arm_const_structs.h"

#elif CPUSTYLE_STM32F7XX
	// STM32F745ZGT6 processors, up to 216 MHz 

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_STM32F		1		/* архитектура процессора STM32F */
	#define CPUSTYLE_ARM_CM7	1		/* архитектура процессора CORTEX M7 */

	#include "armcpu/stm32f7xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

	#include "arm_math.h"
	#include "arm_const_structs.h"

#elif CPUSTYLE_STM32F4XX

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_STM32F		1		/* архитектура процессора STM32F */
	#define CPUSTYLE_ARM_CM4	1		/* архитектура процессора CORTEX M4 */

	#include "armcpu/stm32f4xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

	#include "arm_math.h"
	#include "arm_const_structs.h"

#elif CPUSTYLE_STM32F30X

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_STM32F		1		/* архитектура процессора STM32F */
	#define CPUSTYLE_ARM_CM4	1		/* архитектура процессора CORTEX M3 */

	// STM32F303VC processors
	#include "armcpu/stm32f30x.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

	#include "arm_math.h"
	#include "arm_const_structs.h"

#elif CPUSTYLE_STM32F1XX

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_STM32F		1		/* архитектура процессора STM32F */
	#define CPUSTYLE_ARM_CM3	1		/* архитектура процессора CORTEX M3 */

	#include "armcpu/stm32f1xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

	#include "arm_math.h"
	#include "arm_const_structs.h"

#elif CPUSTYLE_STM32F0XX

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_STM32F		1		/* архитектура процессора STM32F */
	#define CPUSTYLE_ARM_CM0	1		/* архитектура процессора CORTEX M0 */
	
	#include "armcpu/stm32f0xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

	#include "arm_math.h"
	#include "arm_const_structs.h"

#elif CPUSTYLE_STM32L0XX

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_STM32F		1		/* архитектура процессора STM32F */
	#define CPUSTYLE_ARM_CM0	1		/* архитектура процессора CORTEX M0 */
	#include "armcpu/stm32l0xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

	#include "arm_math.h"
	#include "arm_const_structs.h"

#elif CPUSTYLE_ATSAM3S

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_ARM_CM3	1		/* архитектура процессора CORTEX M3 */

	#define __SAM3S4C__ 1		// replace to __SAM3S4B__
	#include "sam3s.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

	#include "arm_math.h"
	#include "arm_const_structs.h"

#elif CPUSTYLE_ATSAM4S

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_ARM_CM4	1		/* архитектура процессора CORTEX M3 */

	#define __SAM4SA16C__ 1
	#include "sam4s.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

	#include "arm_math.h"
	#include "arm_const_structs.h"

#elif CPUSTYLE_MK20DX	// Teensy 3.1 - Freescale Semiconductor - KINETIS MK20DX256VLH7

	#define CPUSTYLE_ARM	1		/* архитектура процессора ARM */
	#define CPUSTYLE_ARM_CM4	1		/* архитектура процессора CORTEX M3 */

	#include "armcpu/MK20D5.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

	#include "arm_math.h"
	#include "arm_const_structs.h"

#elif CPUSTYLE_AT91SAM7S

	#define CPUSTYLE_ARM		1		/* архитектура процессора ARM */
	#define	CPUSTYLE_ARM7TDMI	1
	#include "armcpu/at91sam7s64.h"

	#define __ASM            __asm           /*!< asm keyword for GNU Compiler          */
	#define __INLINE         inline          /*!< inline keyword for GNU Compiler       */
	#define __STATIC_INLINE  static inline

	#include "tdmi7_gcc.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

#elif CPUSTYLE_AT91SAM9XE

	#define CPUSTYLE_ARM		1		/* архитектура процессора ARM */
	#define	CPUSTYLE_ARM7TDMI	1
	#include "armcpu/at91sam9xe512.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

	#define __ASM            __asm           /*!< asm keyword for GNU Compiler          */
	#define __INLINE         inline          /*!< inline keyword for GNU Compiler       */
	#define __STATIC_INLINE  static inline

	#include "tdmi7_gcc.h"

#elif CPUSTYLE_R7S721

	// Renesas CPU
	#define CPUSTYLE_ARM		1		/* архитектура процессора ARM */
	#define	CPUSTYLE_ARM_CA9	1

	#include "irq_ctrl.h"
	#include "armcpu/Renesas_RZ_A1.h"

	#include "armcpu/iodefine.h"
	#include "rza_io_regrw.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

	#if __ARM_NEON
		#define ARM_MATH_NEON 1
	#endif /* __ARM_NEON */

	#include "arm_math.h"
	#include "arm_const_structs.h"

#elif CPUSTYLE_STM32MP1

	// ST dual core A7 + M4

	// CPUSTYLE_STM32MP157A
	// STM32MP157Axx
	// STM32MP157AAB3

	#define CORE_CA7	1
	#define CPUSTYLE_ARM		1		/* архитектура процессора ARM */
	#define	CPUSTYLE_ARM_CA7	1


	#include "irq_ctrl.h"
	#include "armcpu/stm32mp1xx.h"

	#define ALIGNX_BEGIN __attribute__ ((aligned(64)))
	#define ALIGNX_END /* nothing */

	#if __ARM_NEON
		#define ARM_MATH_NEON 1
	#endif /* __ARM_NEON */

	#include "arm_math.h"
	#include "arm_const_structs.h"

#elif \
	defined (__TMS320C28X__) || \
	0

	#define CPUSTYPE_TMS320F2833X 1
	#include "tms320f2833x.h"

	//#define CPU_FREQ 150000000uL
	#define CPU_FREQ 100000000uL
	//#define CPU_FREQ 5000000uL		// Частота без инициализаwии PLL - xtal / 4

	#define ALIGNX_BEGIN /* __attribute__ ((aligned(64)))  */
	#define ALIGNX_END /* nothing */

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
void cpu_initdone(void);	// секция init больше не нужна
uint_fast32_t cpu_getdebugticks(void);	// получение из аппаратного счетчика монотонно увеличивающегося кода


void hardware_timer_initialize(uint_fast32_t ticksfreq);

void spool_encinterrupt(void);	/* прерывание по изменению сигнала на входах от валкодера */
void spool_encinterrupt2(void);	/* прерывание по изменению сигнала на входах от валкодера #2 */
void hardware_encoder_initialize(void);
uint_fast8_t hardware_get_encoder_bits(void);/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */
uint_fast8_t hardware_get_encoder2_bits(void);/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */

void hardware_adc_initialize(void);

#if CPUSTYLE_ARM

	/* тип для передачи параметра "адрес устройства на SPI шине" */
	/* это может быть битовая маска в порту ввода-вывода */
	typedef uint_fast32_t spitarget_t;		
	/* тип для хранения данных, считанных из порта ввода-вывода или для формируемого значения */
	typedef uint_fast32_t portholder_t;		

	#define FLASHMEM //__flash
	#define NOINLINEAT // __attribute__((noinline))

	#if CPUSTYLE_R7S721
		#define FLASHMEMINIT	__attribute__((section(".initdata"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
		#define FLASHMEMINITFUNC	__attribute__((section(".initfunc"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
		#define RAMFUNC_NONILINE // __attribute__((__section__(".ramfunc"), noinline))
		#define RAMFUNC			 // __attribute__((__section__(".ramfunc")))
		#define RAMNOINIT_D1	__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
		#define RAMFRAMEBUFF	__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
		#define RAMDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
		#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
		#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
		#define RAMHEAP __attribute__((used, section(".heap"), aligned(64))) // memory used as heap zone
	#elif CPUSTYLE_STM32MP1
		// TODO: Use SYSRAM as DTCM/ITCM
		#define FLASHMEMINIT	__attribute__((section(".initdata"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
		#define FLASHMEMINITFUNC	__attribute__((section(".initfunc"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
		#define RAMFUNC_NONILINE __attribute__((__section__(".itcm"), noinline))
		#define RAMFUNC			 __attribute__((__section__(".itcm")))
		#define RAMNOINIT_D1	__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
		#define RAMFRAMEBUFF	__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
		#define RAMDTCM			__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
		#define RAMBIGDTCM		__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
		#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
		#define RAMHEAP __attribute__((used, section(".heap"), aligned(64))) // memory used as heap zone
	#elif (CPUSTYLE_STM32H7XX)
		#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
		#define FLASHMEMINIT	//__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
		#define FLASHMEMINITFUNC	//__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
		#define RAMFUNC_NONILINE __attribute__((noinline,__section__(".itcm")))  // удаление управления noinline добавило 2-3 процента быстродействия __attribute__((__section__(".ramfunc"), noinline))
		#define RAMFUNC			 __attribute__((__section__(".itcm")))
		#define RAMNOINIT_D1	__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
		#define RAMFRAMEBUFF	__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
		#define RAMDTCM			__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
		#define RAMBIGDTCM		__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
		#define RAMBIG			__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
		#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
	#elif (CPUSTYLE_STM32F7XX)
		#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
		#define FLASHMEMINIT	__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
		#define FLASHMEMINITFUNC	//__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
		#define RAMFUNC_NONILINE __attribute__((noinline,__section__(".itcm")))  // удаление управления noinline добавило 2-3 процента быстродействия __attribute__((__section__(".ramfunc"), noinline))
		#define RAMFUNC			 __attribute__((__section__(".itcm")))
		#define RAMNOINIT_D1	//__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
		#define RAMFRAMEBUFF	//__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
		#define RAMDTCM			__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
		#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
		#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
		#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
	#elif CPUSTYLE_STM32F4XX && (defined (STM32F429xx) || defined(STM32F407xx))
		#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
		#define FLASHMEMINIT	//__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
		#define FLASHMEMINITFUNC	//__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
		#define RAMFUNC_NONILINE  //__attribute__((__section__(".itcm"), noinline))
		#define RAMFUNC			 // __attribute__((__section__(".itcm")))
		#define RAMNOINIT_D1	//__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
		#define RAMFRAMEBUFF	//__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
		#define RAMDTCM			__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
		#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
		#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
		#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
	#elif CPUSTYLE_STM32F4XX
		#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
		#define FLASHMEMINIT	//__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
		#define FLASHMEMINITFUNC	//__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
		#define RAMFUNC_NONILINE // __attribute__((__section__(".ramfunc"), noinline))  
		#define RAMFUNC			 // __attribute__((__section__(".ramfunc")))  
		#define RAMNOINIT_D1	//__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
		#define RAMFRAMEBUFF	//__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
		#define RAMDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
		#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
		#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
		#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
	#else
		#define VTRATTR	__attribute__ ((section("vtable"), used, aligned(256 * 4)))
		#define FLASHMEMINIT	//__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
		#define FLASHMEMINITFUNC	//__attribute__((section(".init"))) /* не требуется быстрый доступ - например образ загружаемый в FPGA */
		#define RAMFUNC_NONILINE __attribute__((noinline,__section__(".itcm")))  // удаление управления noinline добавило 2-3 процента быстродействия __attribute__((__section__(".ramfunc"), noinline))
		#define RAMFUNC			 __attribute__((__section__(".itcm")))
		#define RAMNOINIT_D1	//__attribute__((section(".noinit"))) /* размещение в памяти SRAM_D1 */
		#define RAMFRAMEBUFF	//__attribute__((section(".framebuff"))) /* размещение в памяти SRAM_D1 */
		#define RAMDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM */
		#define RAMBIGDTCM	//__attribute__((section(".dtcm"))) /* размещение в памяти DTCM на процессорах где её много */
		#define RAMBIG			//__attribute__((section(".ram_d1"))) /* размещение в памяти SRAM_D1 */
		#define RAMHEAP __attribute__((used, section(".heap"), aligned(16))) // memory used as heap zone
	#endif

	#define ATTRWEAK __attribute__ ((weak))
	// Use __attribute__ ((weak, alias("Default_Handler")))

	#define PSTR(s) (s)
	//#define PSTR(s) (__extension__({static const char __c[] FLASHMEM = (s); &__c[0];}))

	void RAMFUNC local_delay_us(int timeUS);
	void local_delay_ms(int timeMS);

#elif CPUSTYLE_ATMEGA || CPUSTYLE_ATXMEGA
	// ATMega, ATXMega target

	/* тип для передачи параметра "адрес устройства на SPI шине" */
	/* это может быть битовая маска в порту ввода-вывода */
	typedef uint_fast8_t spitarget_t;
	/* тип для хранения данных, считанный из порта ввода-вывода или для формируемого значения */
	typedef uint_fast8_t portholder_t;		

	#define local_delay_us(t) do { _delay_us(t); } while (0)
	#define local_delay_ms(t) do { _delay_ms(t); } while (0)
 
	#define FLASHMEM __flash
	#define FLASHMEMINIT	__flash	/* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define FLASHMEMINITFUNC	/* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define RAMDTCM

	#if (FLASHEND > 0x7FFF)	
		// нет нужды экономить память FLASH
		#define NOINLINEAT // __attribute__((noinline))
	#else
		#define NOINLINEAT __attribute__((noinline))	// On small FLASH ATMega CPUs
	#endif
	#define RAMFUNC_NONILINE // __attribute__((__section__(".ramfunc"), noinline))  
	#define RAMFUNC			 // __attribute__((__section__(".ramfunc")))  
	#define RAMNOINIT_D1
	#define RAMHEAP 		//__attribute__((used, section(".heap"))) // memory used as heap zone
	#define ATTRWEAK __attribute__ ((weak))

#elif CPUSTYPE_TMS320F2833X
	/* тип для передачи параметра "адрес устройства на SPI шине" */
	/* это может быть битовая маска в порту ввода-вывода */
	typedef uint_fast32_t spitarget_t;
	/* тип для хранения данных, считанный из порта ввода-вывода или для формируемого значения */
	typedef uint_fast32_t portholder_t;		

	#define enableIRQ() do { asm(" NOP"); } while (0)
	#define disableIRQ() do { asm(" NOP"); } while (0)

	#define global_enableIRQ() do { asm(" NOP"); } while (0)
	#define global_disableIRQ() do { asm(" NOP"); } while (0)

	#define FLASHMEM //__flash
	#define FLASHMEMINIT	/* не требуется быстрый доступ - например образ загружаемый в FPGA */
	#define FLASHMEMINITFUNC	/* не требуется быстрый доступ - например образ загружаемый в FPGA */

	// нет нужды экономить память FLASH
	#define NOINLINEAT // __attribute__((noinline))
	#define RAMFUNC_NONILINE // __attribute__((__section__(".ramfunc"), noinline))  
	#define RAMFUNC			 // __attribute__((__section__(".ramfunc")))  
	#define RAMNOINIT_D1
	#define ATTRWEAK __attribute__ ((weak))

#else
	#error Undefined CPUSTYLE_xxxx
#endif

// Sample usage:
///* все возможные в данной конфигурации фильтры */
//#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_1P8 | IF3_FMASK_2P7 | IF3_FMASK_3P1)

// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
void kbd_spool(void);
void display_spool(void);	// отсчёт времени по запрещению обновления дисплея при вращении валкодера

void spool_secound(void);		// вызывается раз в секунду из таймерного прерывания.

void spool_nmeapps(void);	// Обработчик вызывается при приходе очередного импульса PPS

// получить прескалер и значение для программирования таймера
uint_fast8_t
hardware_calc_sound_params(
	uint_least16_t tonefreq,	/* tonefreq - частота в десятых долях герца. Минимум - 400 герц (определено набором команд CAT). */
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

// +++ dsp
// Интерфейс к НЧ кодеку
void hardware_audiocodec_enable(void);		// Интерфейс к НЧ кодеку
void hardware_audiocodec_initialize(void);	// Интерфейс к НЧ кодеку

// Интерфейс к ПЧ кодеку или FPGA
void hardware_fpgacodec_enable(void);		// Интерфейс к ВЧ кодеку
void hardware_fpgacodec_initialize(void);	// Интерфейс к ВЧ кодеку
void hardware_fpgaspectrum_enable(void);	// Интерфейс к источнику данных о спектре
void hardware_fpgaspectrum_initialize(void);	// Интерфейс к источнику данных о спектре

void hardware_dac_initialize(void);		/* инициализация DAC на STM32F4xx */
void hardware_dac_ch1_setvalue(uint_fast16_t v);	// вывод 12-битного значения на ЦАП - канал 1
void hardware_dac_ch2_setvalue(uint_fast16_t v);	// вывод 12-битного значения на ЦАП - канал 2


void hardware_spi_io_delay(void);

void cat2_parsechar(uint_fast8_t c);				/* вызывается из обработчика прерываний */
void cat2_rxoverflow(void);							/* вызывается из обработчика прерываний */
void cat2_disconnect(void);							/* вызывается из обработчика прерываний произошёл разрыв связи при работе по USB CDC */
void cat2_sendchar(void * ctx);							/* вызывается из обработчика прерываний */

// Функции тестирования работы компорта по прерываниям
void cat3_parsechar(uint_fast8_t c);				/* вызывается из обработчика прерываний */
void cat3_rxoverflow(void);							/* вызывается из обработчика прерываний */
void cat3_disconnect(void);							/* вызывается из обработчика прерываний */
void cat3_sendchar(void * ctx);							/* вызывается из обработчика прерываний */

void modem_parsechar(uint_fast8_t c);				/* вызывается из обработчика прерываний */
void modem_rxoverflow(void);						/* вызывается из обработчика прерываний */
void modem_disconnect(void);						/* вызывается из обработчика прерываний */
void modem_sendchar(void * ctx);							/* вызывается из обработчика прерываний */

void nmea_parsechar(uint_fast8_t c);				/* вызывается из обработчика прерываний */
void nmea_rxoverflow(void);							/* вызывается из обработчика прерываний */
void nmea_sendchar(void * ctx);							/* вызывается из обработчика прерываний */

void hardware_uart1_initialize(uint_fast8_t debug);
void hardware_uart1_set_speed(uint_fast32_t baudrate);
void hardware_uart1_tx(void * ctx, uint_fast8_t c);	/* передача символа после прерывания о готовности передатчика */
void hardware_uart1_enabletx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
void hardware_uart1_enablerx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
uint_fast8_t hardware_usart1_putchar(uint_fast8_t c);/* передача символа если готов порт */
uint_fast8_t hardware_usart1_getchar(char * cp); /* приём символа, если готов порт */

void hardware_uart2_initialize(uint_fast8_t debug);
void hardware_uart2_set_speed(uint_fast32_t baudrate);
void hardware_uart2_tx(void * ctx, uint_fast8_t c);	/* передача символа после прерывания о готовности передатчика */
void hardware_uart2_enabletx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
void hardware_uart2_enablerx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
uint_fast8_t hardware_usart2_putchar(uint_fast8_t c);/* передача символа если готов порт */
uint_fast8_t hardware_usart2_getchar(char * cp); /* приём символа, если готов порт */

void usbd_cdc_tx(void * ctx, uint_fast8_t c);			/* передача символа после прерывания о готовности передатчика - вызывается из HARDWARE_CDC_ONTXCHAR */
void usbd_cdc_enabletx(uint_fast8_t state);	/* вызывается из обработчика прерываний */
void usbd_cdc_enablerx(uint_fast8_t state);	/* вызывается из обработчика прерываний */

/* отладочная выдача через USB CDC */
void debugusb_initialize(void);				/* Вызывается из user-mode программы при запрещённых прерываниях. */
uint_fast8_t debugusb_putchar(uint_fast8_t c);/* передача символа если готов порт */
uint_fast8_t debugusb_getchar(char * cp); /* приём символа, если готов порт */
void debugusb_parsechar(uint_fast8_t c);	/* вызывается из обработчика прерываний */
void debugusb_sendchar(void * ctx);			/* вызывается из обработчика прерываний */

// spool-based functions for debug
int dbg_puts_impl_P(const FLASHMEM char * s);
int dbg_puts_impl(const char * s);
int dbg_putchar(int c);
int dbg_getchar(char * r);

/* TWI (I2C) interface */
#define I2C_RETRIES 3	/* количество повторов */
void i2c_initialize(void);
void i2c_start(uint_fast8_t address);

void i2c_read(uint_fast8_t * pdata, uint_fast8_t acknak);

#define I2C_READ_ACK 0  // i2c_read parameter
#define I2C_READ_ACK_1 1  // i2c_read parameter
#define I2C_READ_NACK 2		// ack_type - last parameterr in read block
#define I2C_READ_ACK_NACK 3		// чтение первого и единственного байта по I2C
void i2c_write(uint_fast8_t data);
void i2c_write_withrestart(uint_fast8_t data);	// запись, после чего restart
void i2c_waitsend(void);	// Вызвать после последнего i2c_write()
void i2c_stop(void);

void hardware_twi_master_configure(void);

uint32_t hardware_get_random(void);

void arm_hardware_ltdc_initialize(void);	// LCD-TFT Controller (LTDC) with framebuffer
void arm_hardware_dma2d_initialize(void);	// Graphic engine
void arm_hardware_sdram_initialize(void);	// External memory region(s)
void arm_hardware_ltdc_pip_set(uintptr_t addr);	// set PIP framebuffer address
void arm_hardware_ltdc_pip_off(void);	// set PIP framebuffer address

uint_fast8_t usbd_cdc_getrts(void);
uint_fast8_t usbd_cdc_getdtr(void);

void AT91F_PIOA_IRQHandler(void);

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

void cpu_stm32f1xx_setmapr(unsigned long bits);

void hardware_tim21_initialize(void);

void hardware_blfreq_initialize(void);
void hardware_blfreq_setdivider(uint_fast32_t v);
uint_fast16_t getbldivider(uint_fast32_t freq);


void hardware_sdhost_initialize(void);
void hardware_sdhost_setspeed(unsigned long ticksfreq);
void hardware_sdhost_setbuswidth(uint_fast8_t use4bit);

void lowtests(void);
void midtests(void);
void hightests(void);
void looptests(void);	// Периодически вызывается в главном цикле

#define ADDPAD 32	// TODO: должно быть без этого

void arm_hardware_invalidate(uintptr_t base, size_t size);	// Сейчас в эту память будем читать по DMA
void arm_hardware_flush(uintptr_t base, size_t size);	// Сейчас эта память будет записываться по DMA куда-то
void arm_hardware_flush_invalidate(uintptr_t base, size_t size);	// Сейчас эта память будет записываться по DMA куда-то. Потом содержимое не требуется
void arm_hardware_flush_all(void);

void r7s721_sdhi0_dma_handler(void);

uint_fast32_t 
NOINLINEAT
calcdivround2(
	uint_fast32_t ref,	/* частота на входе делителя, в герцах. */
	uint_fast32_t freq	/* требуемая частота на выходе делителя, в герцах. */
	);

#if CPUSTYLE_R7S721
	//  Renesas parameters
	#define HARDWARE_USBD_PIPE_ISOC_OUT	1	// ISOC OUT Аудиоданные от компьютера в TRX - D0FIFOB0
	#define HARDWARE_USBD_PIPE_ISOC_IN	2	// ISOC IN Аудиоданные в компьютер из TRX - D0FIFOB1
	#define HARDWARE_USBD_PIPE_CDC_OUT	3	// CDC OUT Данные ком-порта от компьютера в TRX
	#define HARDWARE_USBD_PIPE_CDC_IN	4	// CDC IN Данные ком-порта в компьютер из TRX
	#define HARDWARE_USBD_PIPE_CDC_OUTb	14	// CDC OUT - без передачи данных
	#define HARDWARE_USBD_PIPE_CDC_INb	15	// CDC IN - без передачи данных
	#define HARDWARE_USBD_PIPE_CDC_INT	6	//
	#define HARDWARE_USBD_PIPE_CDC_INTb	7	//
	#define HARDWARE_USBD_PIPE_RNDIS_OUT	12	// RNDIS OUT Данные RNDIS от компьютера в TRX
	#define HARDWARE_USBD_PIPE_RNDIS_IN		13	// RNDIS IN Данные RNDIS в компьютер из TRX
	#define HARDWARE_USBD_PIPE_RNDIS_INT	8	//
#endif /* CPUSTYLE_R7S721 */

#define CATPCOUNTSIZE (13)
#define MSGBUFFERSIZE8 (9 + CATPCOUNTSIZE)

enum messagetypes
{
	MSGT_EMPTY,
	MSGT_1SEC,
	MSGT_KEYB,	// 1 byte - key code
	MSGT_CAT,	// 12 bytes as parameter
	//
	MSGT_count
};

uint_fast8_t takemsgready_user(uint8_t * * dest);	// Буферы с принятымти от обработчиков прерываний сообщениями
void releasemsgbuffer_user(uint8_t * dest);	// Освобождение обработанного буфера сообщения
size_t takemsgbufferfree_low(uint8_t * * dest);	// Буфер для формирования сообщения
void placesemsgbuffer_low(uint_fast8_t type, uint8_t * dest);	// поместить сообщение в очередь к исполнению

#include "list.h"

typedef struct ticker
{
	LIST_ENTRY item;
	unsigned period;
	unsigned fired;
	void (* cb)(void *);
	void * ctx;
} ticker_t;

void ticker_initialize(ticker_t * p, unsigned nticks, void (* cb)(void *), void * ctx);
void bootloader_copyapp(uintptr_t apparea);
uint_fast8_t bootloader_get_start(uintptr_t apparea, uintptr_t * ip);
void bootloader_detach(uintptr_t ip);

#define HARDWARE_ADCINPUTS	40	/* до 8-ти входов АЦП */

#define BOARD_ADCX0BASE 24	// on-board ADC base index
#define BOARD_ADCX1BASE 32	// PA board ADC base index
#define BOARD_ADCXIN(ch) (BOARD_ADCX0BASE + (ch))
#define BOARD_ADCX1IN(ch) (BOARD_ADCX0BASE + (ch))
#define BOARD_ADCX2IN(ch) (BOARD_ADCX1BASE + (ch))

// Cortex-A7/A9 handlers
void Undef_Handler(void);
void SWI_Handler(void);
void PAbort_Handler(void);
void DAbort_Handler(void);
void FIQ_Handler(void);
void IRQ_Handler(void);

// Set interrupt vector wrappers
void arm_hardware_set_handler(uint_fast16_t int_id, void (* handler)(void), uint_fast8_t priority);
void arm_hardware_set_handler_overrealtime(uint_fast16_t int_id, void (* handler)(void));
void arm_hardware_set_handler_realtime(uint_fast16_t int_id, void (* handler)(void));
void arm_hardware_set_handler_system(uint_fast16_t int_id, void (* handler)(void));

void audioproc_spool_user(void);	// вызывать при выполнении длительных операций

void hardware_set_dotclock(unsigned long dotfreq);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#include "product.h"
#include "taildefs.h"

#define USBALIGN_BEGIN __attribute__ ((aligned (64)))
#define USBALIGN_END /* nothing */
#define UNUSED(x) ((void)(x))


#define AUDIORECBUFFSIZE16 (2048 * 4)	// размер данных должен быть не меньше размера кластера на SD карте

#endif // HARDWARE_H_INCLUDED
