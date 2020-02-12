/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#ifndef PARAMDEPEND_H_INCLUDED
#define PARAMDEPEND_H_INCLUDED 1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if CPUSTYLE_AT91SAM7S

	//#define CPU_FREQ ((18432000uL * 73) / 14 / 2)	// satmod9if_v0 ARM board
	#define CPU_FREQ 48000000uL
	//#define CPU_FREQ 12000000uL

	#define ADC_FREQ	500000uL	/* тактовая частота SAR преобразователя АЦП. */

	#define TICKS_FREQUENCY		(200uL * 1) // at ARM - 400 Hz

	//#define SCL_CLOCK  100000uL	/* 100 kHz I2C/TWI speed */
	#define SCL_CLOCK	400000uL	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (4UL * 1000 * 1000)	/* 4 MHz на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ARM_REALTIME_PRIORITY	AT91C_AIC_PRIOR_HIGHEST 
	#define ARM_SYSTEM_PRIORITY		AT91C_AIC_PRIOR_LOWEST 

	#define ADCVREF_CPU	33		// 3.3 volt
	//#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCBITS 8	/* АЦП работает с 8-битными значениями */
	#define HARDWARE_ADCINPUTS	40	/* до 8-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		


#elif CPUSTYLE_STM32F0XX
	//
	// STM32F030F4P6, STM32F051C6T6 processors

	//#define REF1_DIV 1
	//#define REF1_MUL 1	// 8 MHz
	#define PLL_FREQ	(REFINFREQ / REF1_DIV * REF1_MUL)

	#define CPU_FREQ (PLL_FREQ / 1)	// 48 MHz
	//#define CPU_FREQ 180000000uL
	//#define CPU_FREQ 168000000uL
	/* частоты, подающиеся на периферию */
	#define	PCLK1_FREQ (CPU_FREQ / 1)	// 48 MHz PCLK1 frequency
	#define	PCLK2_FREQ (CPU_FREQ / 1)	// 48 MHz PCLK2 frequency
	#define SYSTICK_FREQ CPU_FREQ	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

	#define TICKS_FREQUENCY		(200uL * 1) // at ARM - 400 Hz

	// ADC clock frequency: 0.6..14 MHz
	#define ADC_FREQ	12000000uL	/* тактовая частота SAR преобразователя АЦП. */
	#define SCL_CLOCK	400000uL	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (PCLK1_FREQ / 4)	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ADCVREF_CPU	33		// 3.3 volt
	#define DACVREF_CPU	33		// 3.3 volt
	#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */
	#define HARDWARE_ADCINPUTS	40	/* до 32-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		
	#define WITHREFSENSORVAL	1210	/* Reference voltage: STM32F746, STM32F429, STM32F446 = 1.21V */

#elif CPUSTYLE_STM32L0XX
	//
	// STM32L051K6T processor

	#define	REFINFREQ 16000000uL 		/* definition from stm32f0xx.h нельзя использовать - SPISPEED использцтся в условной компиляции */
	//#define	REFINFREQ 4000000uL 	/* definition from stm32f0xx.h нельзя использовать - SPISPEED использцтся в условной компиляции */
	//#define	REFINFREQ 2100000uL 	/* definition from stm32f0xx.h нельзя использовать - SPISPEED использцтся в условной компиляции */
	//#define REF1_DIV 2
	//#define REF1_MUL 12	// 48 MHz
	// HSI without PLL
	#define REF1_DIV 1
	#define REF1_MUL 1	// 8 MHz
	#define PLL_FREQ	(REFINFREQ / REF1_DIV * REF1_MUL)

	#define CPU_FREQ (PLL_FREQ / 1)	// 48 MHz
	//#define CPU_FREQ 180000000uL
	//#define CPU_FREQ 168000000uL
	/* частоты, подающиеся на периферию */
	#define	PCLK1_FREQ (CPU_FREQ / 1)	// 48 MHz PCLK1 frequency
	#define	PCLK2_FREQ (CPU_FREQ / 1)	// 48 MHz PCLK2 frequency
	#define SYSTICK_FREQ CPU_FREQ	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

	#define TICKS_FREQUENCY		(200uL * 1) // at ARM - 200 Hz

	// ADC clock frequency: 0.6..14 MHz
	#define ADC_FREQ	12000000uL	/* тактовая частота SAR преобразователя АЦП. */
	#define SCL_CLOCK	400000uL	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (PCLK1_FREQ / 1)	/* 8 MHz на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ADCVREF_CPU	30		// 3.3 volt
	#define DACVREF_CPU	33		// 3.3 volt
	#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */
	#define HARDWARE_ADCINPUTS	40	/* до 32-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		
	#define WITHREFSENSORVAL	1224	/* Reference voltage: STM32L031xx = 1.224V */

#elif CPUSTYLE_STM32F1XX
	//
	#define PLL_FREQ	(REFINFREQ / REF1_DIV * REF1_MUL)
	#define CPU_FREQ (PLL_FREQ / 1)

 	#if CPU_FREQ >= 48000000uL
		#define	PCLK1_FREQ (CPU_FREQ / 2)	// PCLK1 frequency
		#define	PCLK1_TIMERS_FREQ (CPU_FREQ / 2)
		#define	PCLK2_FREQ (CPU_FREQ / 1)	// PCLK2 frequency
	#else
		#define	PCLK1_FREQ (CPU_FREQ / 1)	// PCLK1 frequency
		#define	PCLK1_TIMERS_FREQ (CPU_FREQ / 1)
		#define	PCLK2_FREQ (CPU_FREQ / 1)	// PCLK2 frequency
	#endif
	#define SYSTICK_FREQ CPU_FREQ	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

	#define SPISPEED (PCLK1_FREQ / 4)	/* 9.0 MHz на SCLK - требуемая скорость передачи по SPI */
	#define TICKS_FREQUENCY		(200uL * 1) // at ARM - 200 Hz
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */


	// ADC clock frequency: 1..20 MHz
	#define ADC_FREQ	2000000uL	/* тактовая частота SAR преобразователя АЦП. */
	//#define ADC_FREQ	16000000uL	/* тактовая частота SAR преобразователя АЦП. */

	#define SCL_CLOCK	400000uL	/* 400 kHz I2C/TWI speed */

	#define ADCVREF_CPU	33		// 3.3 volt
	#define DACVREF_CPU	33		// 3.3 volt
	#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */
	#define HARDWARE_ADCINPUTS	40	/* до 32-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		
	#define WITHREFSENSORVAL	1210	/* Reference voltage: STM32F746, STM32F429, STM32F446 = 1.21V */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

		//! GPIO Alternate Functions
		typedef enum {
			AF_SYSTEM,		//!< AF0 - SYSTEM*
			AF_TIM1,		//!< AF1 - TIM1/2
			AF_TIM2 = 1,	//!< AF1 - TIM1/2
			AF_TIM3,		//!< AF2 - TIM3/4/5
			AF_TIM4 = 2,	//!< AF2 - TIM3/4/5
			AF_TIM5 = 2,	//!< AF2 - TIM3/4/5
			AF_TIM8,		//!< AF3 - TIM9/10/11
			AF_TIM9 = 3,	//!< AF3 - TIM9/10/11
			AF_TIM10 = 3,	//!< AF3 - TIM9/10/11
			AF_TIM11 = 3,	//!< AF3 - TIM9/10/11
			AF_I2C1,		//!< AF4 - I2C1/2/3
			AF_I2C2 = 4,	//!< AF4 - I2C1/2/3
			AF_I2C3 = 4,	//!< AF4 - I2C1/2/3
			AF_SPI1,		//!< AF5 - SPI1/2
			AF_SPI2 = 5,	//!< AF5 - SPI1/2
			AF_SPI3,		//!< AF6 - SPI3
			AF_SPI2ext = 6,	//!< AF6 - SPI3
			AF_SAI = 6,		//!< AF6 - SAI1/SAI2
			AF_USART1,		//!< AF7 - USART1/2/3
			AF_USART2 = 7,	//!< AF7 - USART1/2/3
			AF_USART3 = 7,	//!< AF7 - USART1/2/3 
			AF_USART4,		//!< AF8 - USART4/5/6
			AF_USART5 = 8,	//!< AF8 - USART4/5/6
			AF_USART6 = 8,	//!< AF8 - USART4/5/6
			AF_CAN1,		//!< AF9 - CAN1/2
			AF_CAN2 = 9,	//!< AF9 - CAN1/2
			AF_TIM12 = 9,	//!< AF9 - TIM12/13/14
			AF_TIM13 = 9,	//!< AF9 - TIM12/13/14
			AF_TIM14 = 9,	//!< AF9 - TIM12/13/14
			AF_OTGFS,		//!< AF10 - OTGFS
			AF_OTGHS = 10,	//!< AF10 - OTGHS
			AF_SAI2 = 10,	//!< AF10 - SAI2
			AF_ETH,			//!< AF11 - ETH
			AF_FSMC,		//!< AF12 - FSMC
			AF_SDIO = 12,	//!< AF12 - SDIO
			AF_OTGHS_FS = 12, //!< AF12 - OTG HS configured in FS
			AF_DCMI,		//!< AF13 - DCMI
			AF_AF14,		//!< AF14 - RI
			AF_EVENT		//!< AF15 - SYSTEM (EVENTOUT)
		} GPIO_AFLH_t;

		//
		#if WITHCPUXOSC
			// с генератором
			#define	REFINFREQ WITHCPUXOSC
		#elif WITHCPUXTAL
			// с кварцем
			#define	REFINFREQ WITHCPUXTAL
		#elif CPUSTYLE_STM32H7XX
			// На внутреннем генераторе
			#define	REFINFREQ 64000000uL
		#else /* WITHCPUXTAL */
			// На внутреннем генераторе
			#define	REFINFREQ 16000000uL
		#endif /* WITHCPUXTAL */

		#define PLL_FREQ	(REFINFREQ / REF1_DIV * REF1_MUL)
		#define PLL2_FREQ	(REFINFREQ / REF2_DIV * REF2_MUL)	// STM32H7xxx
		#define PLL3_FREQ	(REFINFREQ / REF3_DIV * REF3_MUL)	// STM32H7xxx

		#if defined(STM32F401xC)

			#define PLLI2S_FREQ (REFINFREQ / REF1_DIV * PLLI2SN_MUL)
			#define	PLLI2S_FREQ_OUT (PLLI2S_FREQ / 2)		// Frequency after PLLI2S_DivQ

			//#define PLLSAI_FREQ (REFINFREQ / REF1_DIV * SAIREF1_MUL)
			//#define PLLSAI_FREQ_OUT (PLLSAI_FREQ / 2)	// Frequency after PLLSAI_DivQ

			#define CPU_FREQ (PLL_FREQ / 4)	// 172032000uL

			/* частоты, подающиеся на периферию */
			#define	PCLK1_FREQ (CPU_FREQ / 2)	// 42 MHz PCLK1 frequency - ti,er clocks is 85 MHz
			#define	PCLK1_TIMERS_FREQ (CPU_FREQ / 1)	// 42 MHz PCLK1 frequency - ti,er clocks is 85 MHz
			#define	PCLK2_FREQ (CPU_FREQ / 1)	// 84 MHz PCLK2 frequency
			#define SYSTICK_FREQ CPU_FREQ	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

 		#elif CPUSTYLE_STM32H7XX

			#define PLLI2S_FREQ (REFINFREQ / REF1_DIV * PLLI2SN_MUL)
			#define	PLLI2S_FREQ_OUT (PLLI2S_FREQ / 2)		// Frequency after PLLI2S_DivQ

			#define PLLSAI_FREQ (REFINFREQ / REF1_DIV * SAIREF1_MUL)
			#define PLLSAI_FREQ_OUT (PLLSAI_FREQ / 2)	// Frequency after PLLSAI_DivQ

			#define CPU_FREQ (PLL_FREQ / 2)	// 172032000uL

			/* частоты, подающиеся на периферию */
			#define	PCLK1_FREQ (CPU_FREQ / 4)	// 42 MHz PCLK1 frequency
			#define	PCLK1_TIMERS_FREQ (CPU_FREQ / 2)	// 42 MHz PCLK1 frequency
			#define	PCLK2_FREQ (CPU_FREQ / 4)	// 84 MHz PCLK2 frequency
			#define	PCLK2_TIMERS_FREQ (CPU_FREQ / 2)	// 84 MHz PCLK2 frequency
			#define SYSTICK_FREQ CPU_FREQ	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора
			#define PER_CK_FREQ 64000000uL	// 2. The per_ck clock could be hse_ck, hsi_ker_ck or csi_ker_ck according to CKPERSEL selection.

		#else

			#define PLLI2S_FREQ (REFINFREQ / REF1_DIV * PLLI2SN_MUL)
			#define	PLLI2S_FREQ_OUT (PLLI2S_FREQ / 2)		// Frequency after PLLI2S_DivQ

			#define PLLSAI_FREQ (REFINFREQ / REF1_DIV * SAIREF1_MUL)
			#define PLLSAI_FREQ_OUT (PLLSAI_FREQ / 2)	// Frequency after PLLSAI_DivQ

			#define CPU_FREQ (PLL_FREQ / 2)	// 172032000uL

			/* частоты, подающиеся на периферию */
			#define	PCLK1_FREQ (CPU_FREQ / 4)	// 42 MHz PCLK1 frequency
			#define	PCLK1_TIMERS_FREQ (CPU_FREQ / 4)	// 42 MHz PCLK1 frequency
			#define	PCLK2_FREQ (CPU_FREQ / 2)	// 84 MHz PCLK2 frequency
			#define SYSTICK_FREQ CPU_FREQ	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора
		#endif

	#elif CPUSTYLE_STM32F30X
		//
		//#define WITHCPUXTAL 8000000uL	// Если есть внешний кварц на процессоре.
		#if WITHCPUXTAL
			#define	REFINFREQ WITHCPUXTAL
			#define REF1_DIV 1
			#define REF1_MUL 9	// Up to 16 supported
		#else
			#define	REFINFREQ 8000000uL
			#define REF1_DIV 2
			#define REF1_MUL 9	// Up to 16 supported
		#endif

		#define PLL_FREQ	(REFINFREQ / REF1_DIV * REF1_MUL)
		#define CPU_FREQ (PLL_FREQ / 1)

 		#if CPU_FREQ >= 48000000uL
			#define	PCLK1_FREQ (CPU_FREQ / 2)	// PCLK1 frequency
			#define	PCLK2_FREQ (CPU_FREQ / 1)	// PCLK2 frequency
		#else
			#define	PCLK1_FREQ (CPU_FREQ / 1)	// PCLK1 frequency
			#define	PCLK2_FREQ (CPU_FREQ / 1)	// PCLK2 frequency
		#endif
		#define SYSTICK_FREQ CPU_FREQ	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

	#endif

	#define TICKS_FREQUENCY		200uL	// at ARM - 200 Hz

	// ADC clock frequency: 1..20 MHz
	#define SCL_CLOCK	400000uL	/* 400 kHz I2C/TWI speed */

	//#define SPISPEED (PCLK1_FREQ / 16)	/* 3.5 MHz на SCLK - требуемая скорость передачи по SPI */
	//#define SPISPEED (PCLK1_FREQ / 8)	/* 7 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEED (PCLK1_FREQ / 4)	/* 14 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 12000000uL//(PCLK1_FREQ / 2)	/* 28 на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ADCVREF_CPU	33		// 3.3 volt
	#define DACVREF_CPU	33		// 3.3 volt
	#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCINPUTS	40	/* до 32-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;	
	#if CPUSTYLE_STM32H7XX
		//#define HARDWARE_ADCBITS 8	/* АЦП работает с 8-битными значениями */
		//#define HARDWARE_ADCBITS 10	/* АЦП работает с 10-битными значениями */
		#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */
		//#define HARDWARE_ADCBITS 14	/* АЦП работает с 14-битными значениями */
		//#define HARDWARE_ADCBITS 16	/* АЦП работает с 16-битными значениями */
		#define ADC_FREQ	10000000uL	/* тактовая частота SAR преобразователя АЦП. */
		#define WITHREFSENSORVAL	1240	/* Reference voltage: STM32H743 1.180 1.216 1.255 */
	#elif CPUSTYLE_STM32F7XX || CPUSTYLE_STM32F4XX
		//#define HARDWARE_ADCBITS 6	/* АЦП работает с 6-битными значениями */
		//#define HARDWARE_ADCBITS 8	/* АЦП работает с 8-битными значениями */
		//#define HARDWARE_ADCBITS 10	/* АЦП работает с 10-битными значениями */
		#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */
		#define ADC_FREQ	28000000uL	/* тактовая частота SAR преобразователя АЦП. */
		#define WITHREFSENSORVAL	1210	/* Reference voltage: STM32F746, STM32F429, STM32F446 = 1.21V */
	#else
		#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */
		#define ADC_FREQ	2000000uL	/* тактовая частота SAR преобразователя АЦП. */
		#define WITHREFSENSORVAL	1210	/* Reference voltage: STM32F746, STM32F429, STM32F446 = 1.21V */
	#endif

#elif CPUSTYLE_ATSAM3S

	//#define CPU_FREQ ((18432000uL * 73) / 14 / 2)	// satmod9if_v0 ARM board
	#define CPU_FREQ 64000000uL
	//#define CPU_FREQ 48000000uL
	//#define CPU_FREQ 32000000uL
	//#define CPU_FREQ 12000000uL
	#define SYSTICK_FREQ CPU_FREQ	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

	// ADC clock frequency: 1..20 MHz
	#define ADC_FREQ	2000000uL	/* тактовая частота SAR преобразователя АЦП. */
	//#define ADC_FREQ	16000000uL	/* тактовая частота SAR преобразователя АЦП. */

	#define TICKS_FREQUENCY		(200uL * 1) // at ARM - 400 Hz

	//#define SCL_CLOCK  100000uL	/* 100 kHz I2C/TWI speed */
	#define SCL_CLOCK	400000uL	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (4UL * 1000 * 1000)	/* 4 MHz на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ADCVREF_CPU	33		// 3.3 volt
	//#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCBITS 10	/* АЦП работает с 10-битными значениями */
	#define HARDWARE_ADCINPUTS	40	/* до 16-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		

#elif CPUSTYLE_ATSAM4S

	//#define CPU_FREQ ((18432000uL * 73) / 14 / 2)	// satmod9if_v0 ARM board
	#define CPU_FREQ 64000000uL
	//#define CPU_FREQ 48000000uL
	//#define CPU_FREQ 32000000uL
	//#define CPU_FREQ 12000000uL
	#define SYSTICK_FREQ CPU_FREQ	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

	// ADC clock frequency: 1..20 MHz
	#define ADC_FREQ	2000000uL	/* тактовая частота SAR преобразователя АЦП. */
	//#define ADC_FREQ	16000000uL	/* тактовая частота SAR преобразователя АЦП. */

	#define TICKS_FREQUENCY		(200uL * 1) // at ARM - 400 Hz

	//#define SCL_CLOCK  100000uL	/* 100 kHz I2C/TWI speed */
	#define SCL_CLOCK	400000uL	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (4UL * 1000 * 1000)	/* 4 MHz на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ADCVREF_CPU	33		// 3.3 volt
	//#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCBITS 10	/* АЦП работает с 10-битными значениями */
	#define HARDWARE_ADCINPUTS	40	/* до 16-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		

#elif CPUSTYLE_AT91SAM9XE

	#define CPU_FREQ (196608000uL / 2)	// частота периферии (процессорная после деления).
	//#define CPU_FREQ 14400000uL
	//#define SCL_CLOCK  100000L	/* 100 kHz I2C/TWI speed */
	#define SCL_CLOCK  400000uL	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (4UL * 1000 * 1000)	/* 4 MHz на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define TICKS_FREQUENCY		200U // 200 Hz

	#define ARM_REALTIME_PRIORITY	AT91C_AIC_PRIOR_HIGHEST 
	#define ARM_SYSTEM_PRIORITY		AT91C_AIC_PRIOR_LOWEST 

	#define ADCVREF_CPU	33		// 3.3 volt
	//#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */
	#define HARDWARE_ADCINPUTS	40	/* до 8-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		

#elif CPUSTYLE_MK20DX	// Tennesy 3.1 - Freescale Semiconductor - KINETIS MK20DX256VLH7

	// todo: заполнить правильными значениями
	#define CPU_FREQ 72000000	// частота периферии (процессорная после деления).
	#define SCL_CLOCK  400000uL	/* 400 kHz I2C/TWI speed */
	#define SPISPEED (4UL * 1000 * 1000)	/* 4 MHz на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define TICKS_FREQUENCY		200U // 200 Hz

	#define ADCVREF_CPU	33		// 3.3 volt
	//#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCBITS 10	/* АЦП работает с 10-битными значениями */
	#define HARDWARE_ADCINPUTS	40	/* до 16-ти входов АЦП */

	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		

#elif CPUSTYLE_ATMEGA

	// ATMega processors

	#define CPU_FREQ (F_CPU)

	#define TICKS_FREQUENCY	 (200U)	// 200 Hz - use compare/match interrupt

	//#define SCL_CLOCK  100000uL	/* 100 kHz I2C/TWI speed */
	//#define SCL_CLOCK  250000uL		/* 250 kHz I2C/TWI speed */
	#define SCL_CLOCK  400000uL	/* 400 kHz I2C/TWI speed */
	//#define SCL_CLOCK  40000uL	/* 40 kHz I2C/TWI speed */

	#define SPISPEED (CPU_FREQ / 2) /* 4 (5) MHz на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ADC_FREQ	250000uL	/* тактовая частота SAR преобразователя АЦП. */

	#define ADCVREF_CPU	33		// 3.3 volt
	//#define ADCVREF_CPU	50		// 5.0 volt

	//#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	//#define HARDWARE_ADCBITS 8	/* АЦП работает с 8-битными значениями */
	#define HARDWARE_ADCBITS 10	/* АЦП работает с 10-битными значениями */

	#define HARDWARE_ADCINPUTS	40	/* до 8-ти входов АЦП */

	#if HARDWARE_ADCBITS == 8
		/* тип для хранения данных, считанных с АЦП */
		typedef uint_fast8_t adcvalholder_t;		
	#elif HARDWARE_ADCBITS == 10
		/* тип для хранения данных, считанных с АЦП */
		typedef uint_fast16_t adcvalholder_t;		
	#else
		#error Erong HARDWARE_ADCBITS value
	#endif

#elif CPUSTYLE_ATXMEGA

	// ATMega processors

	#define CPU_FREQ (F_CPU)

	#define TICKS_FREQUENCY	 (200U * 2)	// 400 Hz - use compare/match interrupt

	//#define SCL_CLOCK  100000uL	/* 100 kHz I2C/TWI speed */
	//#define SCL_CLOCK  250000uL		/* 250 kHz I2C/TWI speed */
	#define SCL_CLOCK  400000uL	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (8000000) /* 4 (5) MHz на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ADC_FREQ	125000uL	/* тактовая частота SAR преобразователя АЦП. */


	#define ADCVREF_CPU	25		// 2.5 volt
	#define HARDWARE_ADCBITS 8	/* АЦП работает с 8-битными значениями */

	#define HARDWARE_ADCINPUTS	40	/* до 8-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast8_t adcvalholder_t;		

#elif CPUSTYPE_TMS320F2833X

	#define TICKS_FREQUENCY	 (200U * 2)	// 400 Hz - use compare/match interrupt
	#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		

#elif CPUSTYLE_R7S721

	//#define WITHCPUXTAL 12000000uL			/* На процессоре установлен кварц 12.000 МГц */
	#define CPU_FREQ	(30 * WITHCPUXTAL)		/* 12 MHz * 30 - clock mode 0, xtal 12 MHz */

	#define BCLOCK_FREQ		(CPU_FREQ / 3)		// 120 MHz
	#define P1CLOCK_FREQ	(CPU_FREQ / 6)		// 60 MHz
	#define P0CLOCK_FREQ	(CPU_FREQ / 12)		// 30 MHz

	#define TICKS_FREQUENCY		(200uL * 1) // at ARM - 400 Hz

	// ADC clock frequency: 1..20 MHz
	#define ADC_FREQ	2000000uL	/* тактовая частота SAR преобразователя АЦП. */
	#define SCL_CLOCK	400000uL	/* 400 kHz I2C/TWI speed */

	#define SPISPEED 8000000uL	/* 8 MHz (10.5) на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST (P1CLOCK_FREQ / 3)	// 20 MHz
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */

	#define HARDWARE_ADCINPUTS	40	/* до 8-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		

	enum
	{
		R7S721_PIOALT_1	= 0x00,	/* 1st Alternative */
		R7S721_PIOALT_2	= 0x01,	/* 2nd Alternative */
		R7S721_PIOALT_3	= 0x02,	/* 3rd Alternative */
		R7S721_PIOALT_4	= 0x03,	/* 4th Alternative */
		R7S721_PIOALT_5	= 0x04,	/* 5th Alternative */
		R7S721_PIOALT_6	= 0x05,	/* 6th Alternative */
		R7S721_PIOALT_7	= 0x06,	/* 7th Alternative */
		R7S721_PIOALT_8	= 0x07	/* 8th Alternative */
	};

	/* видимые в контроллере прерывания регистры от ARM CORE */
	#define ARM_CA9_PRIORITYSHIFT 3	/* ICCPMR[7:3] is valid bit */

	//#define GICC_PMR		(INTC.ICCPMR)	// 4.4.2 Interrupt Priority Mask Register, GICC_PMR
	//#define GICC_RPR		((uint32_t) INTC.ICCRPR)	// 4.4.6 Running Priority Register, GICC_RPR
	//#define GICC_HPPIR		(INTC.ICCHPIR)
	//#define GICC_IAR		(INTC.ICCIAR)
	//#define GICC_BPR		(INTC.ICCBPR)
	//#define GICC_CTLR		(INTC.ICCICR)
	//#define GICD_IPRIORITYRn(n) (((volatile uint8_t *) & INTC.ICDIPR0) [(n)])

	#define ARM_CA9_CACHELEVELMAX	1	/* максимальный уровень cache в процессоре */

	// GIC_SetConfiguration parameters
	#define GIC_CONFIG_EDGE 0x03
	#define GIC_CONFIG_LEVEL 0x01

#elif CPUSTYLE_STM32MP1


	//! GPIO Alternate Functions
	typedef enum {
		AF_SYSTEM,		//!< AF0 - SYSTEM*
		AF_TIM1,		//!< AF1 - TIM1/2
		AF_TIM2 = 1,	//!< AF1 - TIM1/2
		AF_TIM3,		//!< AF2 - TIM3/4/5
		AF_TIM4 = 2,	//!< AF2 - TIM3/4/5
		AF_TIM5 = 2,	//!< AF2 - TIM3/4/5
		AF_TIM8,		//!< AF3 - TIM9/10/11
		AF_TIM9 = 3,	//!< AF3 - TIM9/10/11
		AF_TIM10 = 3,	//!< AF3 - TIM9/10/11
		AF_TIM11 = 3,	//!< AF3 - TIM9/10/11
		AF_USART1,		//!< AF4 - USART1
		AF_I2C1 = 4,	//!< AF4 - I2C1/2/3
		AF_I2C2 = 4,	//!< AF4 - I2C1/2/3
		AF_I2C3 = 4,	//!< AF4 - I2C1/2/3
		AF_SPI1,		//!< AF5 - SPI1/2
		AF_SPI2 = 5,	//!< AF5 - SPI1/2
		AF_SPI3,		//!< AF6 - SPI3
		AF_SPI2ext = 6,	//!< AF6 - SPI3
		AF_SAI = 6,		//!< AF6 - SAI1/SAI2
		AF_USART2,		//!< AF7 - USART2/3
		AF_USART3 = 7,	//!< AF7 - USART1/2/3
		AF_USART4,		//!< AF8 - USART4/5/6
		AF_USART5 = 8,	//!< AF8 - USART4/5/6
		AF_USART6 = 8,	//!< AF8 - USART4/5/6
		AF_CAN1,		//!< AF9 - CAN1/2
		AF_CAN2 = 9,	//!< AF9 - CAN1/2
		AF_TIM12 = 9,	//!< AF9 - TIM12/13/14
		AF_TIM13 = 9,	//!< AF9 - TIM12/13/14
		AF_TIM14 = 9,	//!< AF9 - TIM12/13/14
		AF_OTGFS,		//!< AF10 - OTGFS
		AF_OTGHS = 10,	//!< AF10 - OTGHS
		AF_SAI2 = 10,	//!< AF10 - SAI2
		AF_ETH,			//!< AF11 - ETH
		AF_FSMC,		//!< AF12 - FSMC
		AF_SDIO = 12,	//!< AF12 - SDIO
		AF_OTGHS_FS = 12, //!< AF12 - OTG HS configured in FS
		AF_DCMI,		//!< AF13 - DCMI
		AF_AF14,		//!< AF14 - RI
		AF_EVENT		//!< AF15 - SYSTEM (EVENTOUT)
	} GPIO_AFLH_t;

	// все параметры требуют уточнения, пока заглушки
	#define ARM_CA9_CACHELEVELMAX	1	/* максимальный уровень cache в процессоре */
	#define ARM_CA9_PRIORITYSHIFT 3	/* ICCPMR[7:3] is valid bit */

	#define HSIFREQ 64000000uL
	//
	#if WITHCPUXOSC
		// с генератором
		#define	REFINFREQ WITHCPUXOSC
	#elif WITHCPUXTAL
		// с кварцем
		#define	REFINFREQ WITHCPUXTAL
	#else /* WITHCPUXTAL */
		// На внутреннем генераторе
		#define	REFINFREQ HSIFREQ
	#endif /* WITHCPUXTAL */

	#define CPU_FREQ	(REFINFREQ / (PLL1DIVM) * (PLL1DIVN) / (PLL1DIVP))
	#define AXISS_FREQ	(REFINFREQ / (PLL2DIVM) * (PLL2DIVN) / (PLL2DIVP))
	#define DDR_FREQ 	(REFINFREQ / (PLL2DIVM) * (PLL2DIVN) / (PLL2DIVR))
	#define PLL3_FREQ	(REFINFREQ / (PLL3DIVM) * (PLL3DIVN))
	#define PLL4_FREQ	(REFINFREQ / (PLL4DIVM) * (PLL4DIVN))
	/* частоты, подающиеся на периферию */
	#define	PCLK1_FREQ (AXISS_FREQ / 4)	// 42 MHz PCLK1 frequency
	#define	PCLK1_TIMERS_FREQ (AXISS_FREQ / 2)	// 42 MHz PCLK1 frequency
	#define	PCLK2_FREQ (AXISS_FREQ / 4)	// 84 MHz PCLK2 frequency
	#define	PCLK2_TIMERS_FREQ (AXISS_FREQ / 2)	// 84 MHz PCLK2 frequency
	#define SYSTICK_FREQ AXISS_FREQ	// SysTick_Config станавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора
	#define PER_CK_FREQ HSIFREQ	// 2. The per_ck clock could be hse_ck, hsi_ker_ck or csi_ker_ck according to CKPERSEL selection.

	#define TICKS_FREQUENCY	 (200U)	// 200 Hz

	// ADC clock frequency: 1..20 MHz
	#define SCL_CLOCK	400000uL	/* 400 kHz I2C/TWI speed */

	//#define SPISPEED (PCLK1_FREQ / 16)	/* 3.5 MHz на SCLK - требуемая скорость передачи по SPI */
	//#define SPISPEED (PCLK1_FREQ / 8)	/* 7 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEED (PCLK1_FREQ / 4)	/* 14 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 12000000uL//(PCLK1_FREQ / 2)	/* 28 на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ADCVREF_CPU	33		// 3.3 volt
	#define DACVREF_CPU	33		// 3.3 volt
	#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCINPUTS	40	/* до 32-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;
	//#define HARDWARE_ADCBITS 8	/* АЦП работает с 8-битными значениями */
	//#define HARDWARE_ADCBITS 10	/* АЦП работает с 10-битными значениями */
	#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */
	//#define HARDWARE_ADCBITS 14	/* АЦП работает с 14-битными значениями */
	//#define HARDWARE_ADCBITS 16	/* АЦП работает с 16-битными значениями */
	#define ADC_FREQ	10000000uL	/* тактовая частота SAR преобразователя АЦП. */
	#define WITHREFSENSORVAL	1240	/* Reference voltage: STM32H743 1.180 1.216 1.255 */

	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */

	#define HARDWARE_ADCINPUTS	40	/* до 8-ти входов АЦП */

	#define DACVREF_CPU	33		// 3.3 volt
	#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */

#else

	#error Undefined CPUSTYLE_XXX

#endif

#if CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7 || CPUSTYLE_ARM_CM0

	#if WITHNESTEDINTERRUPTS

		// The processor does not process any exception with a priority value greater than or equal to BASEPRI.
		extern uint32_t gARM_OVERREALTIME_PRIORITY;
		extern uint32_t gARM_REALTIME_PRIORITY;
		extern uint32_t gARM_SYSTEM_PRIORITY;
		extern uint32_t gARM_BASEPRI_ONLY_REALTIME;
		extern uint32_t gARM_BASEPRI_ALL_ENABLED;
		// See usage of functions NVIC_PriorityGroupConfig and NVIC_SetPriorityGrouping
		// A lower priority value indicating a higher priority of running handler
		#define ARM_OVERREALTIME_PRIORITY	((const uint32_t) gARM_OVERREALTIME_PRIORITY)
		#define ARM_REALTIME_PRIORITY	((const uint32_t) gARM_REALTIME_PRIORITY)
		#define ARM_SYSTEM_PRIORITY	((const uint32_t) gARM_SYSTEM_PRIORITY)

		#define ASSERT_IRQL_ALL_ENABLED() ASSERT(1)
		#define ASSERT_IRQL_SYSTEM() ASSERT(1)
		#define ASSERT_IRQL_USER() ASSERT(1)

		#if 0 && WITHDEBUG
			// отладочная версия - контроль правильного контекста запрета/разрешения прерываний
			#define enableIRQ() do { \
					if (__get_BASEPRI() != gARM_BASEPRI_ONLY_REALTIME) \
					{ \
						TP(); \
						debug_printf_P(PSTR("enableIRQ: wrong __get_BASEPRI() value: %08lX\n"), __get_BASEPRI()); \
						for (;;) ; \
					} \
					__set_BASEPRI(gARM_BASEPRI_ALL_ENABLED); \
				} while (0)
			#define disableIRQ() do { \
					if (__get_BASEPRI() != gARM_BASEPRI_ALL_ENABLED) \
					{ \
						TP(); \
						debug_printf_P(PSTR("disableIRQ: wrong __get_BASEPRI() value: %08lX\n"), __get_BASEPRI()); \
						for (;;) ; \
					} \
					__set_BASEPRI(gARM_BASEPRI_ONLY_REALTIME); \
				} while (0)
		#else
			#define enableIRQ() do { __set_BASEPRI(gARM_BASEPRI_ALL_ENABLED); } while (0)	// разрешены все
			#define disableIRQ() do { __set_BASEPRI(gARM_BASEPRI_ONLY_REALTIME); } while (0) // разрешены только realtime
		#endif
		
	#else /* WITHNESTEDINTERRUPTS */

		#define ARM_OVERREALTIME_PRIORITY	(0)
		#define ARM_REALTIME_PRIORITY	(0)
		#define ARM_SYSTEM_PRIORITY	(0)

		#define enableIRQ() do { __enable_irq(); } while (0)
		#define disableIRQ() do { __disable_irq(); } while (0)

		#define ASSERT_IRQL_ALL_ENABLED() ASSERT(1)
		#define ASSERT_IRQL_SYSTEM() ASSERT(1)
		#define ASSERT_IRQL_USER() ASSERT(1)

	#endif /* WITHNESTEDINTERRUPTS */

	#define global_enableIRQ() do { __enable_irq(); } while (0)
	#define global_disableIRQ() do { __disable_irq(); } while (0)

	/* получение адреса для атомарного доступа к битам (с) demiurg_spb */
	/*
	static __attribute__( ( always_inline ) ) volatile uint32_t* bb_bit_address(volatile uint32_t* p, uint_fast8_t bit)
	{
		return ( (volatile uint32_t*) (((uint32_t) p & (uint32_t) 0xf0000000uL) + (uint32_t) 0x02000000uL
		          + (((uint32_t)p & (uint32_t) 0x000fffffUL)<<5) + ((uint32_t) bit<<2)));
	}
	*/

#elif CPUSTYLE_ATMEGA || CPUSTYLE_ATXMEGA

	#define enableIRQ() do { sei(); } while (0)
	#define disableIRQ() do { cli(); } while (0)

	#define global_enableIRQ() do { sei(); } while (0)
	#define global_disableIRQ() do { cli(); } while (0)

	#define ASSERT_IRQL_ALL_ENABLED() ASSERT(1)
	#define ASSERT_IRQL_SYSTEM() ASSERT(1)
	#define ASSERT_IRQL_USER() ASSERT(1)

#elif (CPUSTYLE_ARM_CA9 || CPUSTYLE_ARM_CA7)

	#if CPUSTYLE_ARM_CA9
		#define DCACHEROWSIZE 32
		#define ICACHEROWSIZE 32
	#elif CPUSTYLE_ARM_CA7
		#define DCACHEROWSIZE 64
		#define ICACHEROWSIZE 64
	#endif

	#define ICPIDR0	(* (const volatile uint32_t *) (GIC_INTERFACE_BASE + 0xFE0))
	#define ICPIDR1	(* (const volatile uint32_t *) (GIC_INTERFACE_BASE + 0xFE4))
	#define ICPIDR2	(* (const volatile uint32_t *) (GIC_INTERFACE_BASE + 0xFE8))

	#if WITHNESTEDINTERRUPTS

		#define ARM_CA9_ENCODE_PRIORITY(v) ((v) << ARM_CA9_PRIORITYSHIFT)
		/*
			GICC_PMR == INTC.ICCPMR

			Provides an interrupt priority filter. 
			Only interrupts with higher priority than the value in this
			register are signaled to the processor.

		*/
		enum
		{
			PRI_OVRT = 0,	// RPR value
			PRI_RT = 1,
			PRI_SYS = 2,
			PRI_USER = 3,

			gARM_OVERREALTIME_PRIORITY = ARM_CA9_ENCODE_PRIORITY(PRI_OVRT),	// value for GIC_SetPriority
			gARM_REALTIME_PRIORITY = ARM_CA9_ENCODE_PRIORITY(PRI_RT),	// value for GIC_SetPriority
			gARM_SYSTEM_PRIORITY = ARM_CA9_ENCODE_PRIORITY(PRI_SYS),		// value for GIC_SetPriority

			gARM_BASEPRI_ONLY_REALTIME = ARM_CA9_ENCODE_PRIORITY(PRI_SYS),	// value for GIC_SetInterfacePriorityMask
			gARM_BASEPRI_ALL_ENABLED = ARM_CA9_ENCODE_PRIORITY(PRI_USER)	// value for GIC_SetInterfacePriorityMask
		};

		//#define RUNNING_PRI	((GICC_RPR & 0xFF) >> ARM_CA9_PRIORITYSHIFT) // The current running priority on the CPU interface.

		// A lower priority value indicating a higher priority
		#define ARM_OVERREALTIME_PRIORITY	((const uint32_t) gARM_OVERREALTIME_PRIORITY)
		#define ARM_REALTIME_PRIORITY	((const uint32_t) gARM_REALTIME_PRIORITY)
		#define ARM_SYSTEM_PRIORITY	((const uint32_t) gARM_SYSTEM_PRIORITY)

		#define IS_RPR_SYSTEM() 1 //((GICC_RPR & 0xFF) == ARM_SYSTEM_PRIORITY)
		#define IS_RPR_USER() 1 //((GICC_RPR & 0xFF) > ARM_SYSTEM_PRIORITY)

		#define ASSERT_IRQL_SYSTEM() ASSERT(IS_RPR_SYSTEM())	/* executed from non-realtime interrupts */
		#define ASSERT_IRQL_USER() ASSERT(IS_RPR_USER())	/* executed from user level */

		#if 0 && WITHDEBUG
			// отладочная версия - контроль правильного контекста запрета/разрешения прерываний
			#define enableIRQ() do { \
					if (GIC_GetInterfacePriorityMask() != gARM_BASEPRI_ONLY_REALTIME) \
					{ \
						TP(); \
						debug_printf_P(PSTR("enableIRQ: wrong GIC_GetInterfacePriorityMask() value: %08lX\n"), GIC_GetInterfacePriorityMask()); \
						for (;;) ; \
					} \
					GIC_SetInterfacePriorityMask(gARM_BASEPRI_ALL_ENABLED); \
				} while (0)
			#define disableIRQ() do { \
					if (GIC_GetInterfacePriorityMask() != gARM_BASEPRI_ALL_ENABLED) \
					{ \
						TP(); \
						debug_printf_P(PSTR("disableIRQ: wrong GIC_GetInterfacePriorityMask() value: %08lX\n"), GIC_GetInterfacePriorityMask()); \
						for (;;) ; \
					} \
					GIC_SetInterfacePriorityMask(gARM_BASEPRI_ONLY_REALTIME); \
				} while (0)
		#else
			// разрешены все
			#define enableIRQ() do { \
					GIC_SetInterfacePriorityMask(gARM_BASEPRI_ALL_ENABLED); \
				} while (0)
			// разрешены только realtime
			#define disableIRQ() do { \
					GIC_SetInterfacePriorityMask(gARM_BASEPRI_ONLY_REALTIME); \
				} while (0)
		#endif
	#else /* WITHNESTEDINTERRUPTS */

		#define ASSERT_IRQL_ALL_ENABLED() ASSERT(1)
		#define ASSERT_IRQL_SYSTEM() ASSERT(1)
		#define ASSERT_IRQL_USER() ASSERT(1)

		#define ARM_OVERREALTIME_PRIORITY	0
		#define ARM_REALTIME_PRIORITY	0
		#define ARM_SYSTEM_PRIORITY		0

		#define enableIRQ() do { __enable_irq(); } while (0)
		#define disableIRQ() do { __disable_irq(); } while (0)

	#endif /* WITHNESTEDINTERRUPTS */

	#define global_enableIRQ() do { __enable_irq(); } while (0)
	#define global_disableIRQ() do { __disable_irq(); } while (0)

#else /* CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 */

	// For CPUSTYLE_ARM7TDMI
	//#define ARM_REALTIME_PRIORITY	(0)
	//#define ARM_SYSTEM_PRIORITY	(0)

	unsigned (disableIRQ)(void);
	unsigned (enableIRQ)(void);

	#define global_enableIRQ() do { (enableIRQ)(); } while (0)
	#define global_disableIRQ() do { (disableIRQ)(); } while (0)

	unsigned RAMFUNC (enableIRQ)(void);
	unsigned RAMFUNC (disableIRQ)(void);

#endif /* CPUSTYLE_ARM_CM3 */

/* STM32: In HS mode and when the DMA is used, all variables and data structures dealing
   with the DMA during the transaction process should be 4-bytes aligned */    

#if defined   (__GNUC__)        /* GNU Compiler */
	#pragma GCC diagnostic ignored "-Wunused-function"
	#pragma GCC diagnostic ignored "-Wunused-variable"
	#pragma GCC diagnostic ignored "-Wunused-const-variable"
	#define __ALIGN4_END    __attribute__ ((aligned (4)))
	#define __ALIGN4_BEGIN         
	#define ATTRPACKED __attribute__ ((packed))
	#define KEYWORDPACKED __packed
#else                           
	#if defined   (__CC_ARM)      /* ARM Compiler */
		#define __ALIGN4_BEGIN    __align(4)  
		#define __ALIGN4_END
	#elif defined (__ICCARM__)    /* IAR Compiler */
		#define __ALIGN4_BEGIN 
		#define __ALIGN4_END
	#elif defined  (__TASKING__)  /* TASKING Compiler */
		#define __ALIGN4_BEGIN    __align(4) 
		#define __ALIGN4_END
	#endif /* __CC_ARM */  
#endif /* __GNUC__ */ 



/* типы применяемых микросхем */

#define DDS_TYPE_AD9852		1	// AD9854 also supported
#define DDS_TYPE_AD9857		2
#define DDS_TYPE_AD9951		3	// AD9852, AD9853 and AD9854 also supported
#define DDS_TYPE_AD9834		4	// AD9834, AD9833 and AD9838 supported
#define DDS_TYPE_AD9851		5	// AD9850 and AD9851 supported
#define DDS_TYPE_AD9835		6	// AD9832 and AD9835 supported
#define DDS_TYPE_FPGAV1		7	// NCO, DDC/DUC
#define DDS_TYPE_FPGAV2		8	// NCO, DDC/DUC
#define DDS_TYPE_ATTINY2313	9	// experemental: nco=/7, dds=/9

#define	ADC_TYPE_AD9246		15

#define PLL_TYPE_LMX2306	20
#define PLL_TYPE_ADF4001	21	// ADF4000 also supported
#define PLL_TYPE_MC145170	22
#define PLL_TYPE_ADF4360	23
#define PLL_TYPE_LMX1601	24
#define PLL_TYPE_CMX992		25
#define	PLL_TYPE_TSA6057	26
#define	PLL_TYPE_UMA1501	27	// NXP UMA1501
#define PLL_TYPE_LM7001		28	// Sanyo LM7001
#define PLL_TYPE_HMC830		29	// Hittite HMC830 - code owned by Karen Tadevosyan, RA3APW
#define PLL_TYPE_SI570		30	// Silabs Si570
#define PLL_TYPE_SI5351A	31	// Silabs Si5351A 10-MSOP (three outputs)
#define	PLL_TYPE_RFFC5071	32	// RFMD (Qorvo) RFFC5071 http://www.rfmd.com/store/integrated-synthesizers-with-mixers/rffc5071-1.html
#define PLL_TYPE_NONE		33	// ФАПЧ внешняя, никак не управляется.

#define DAC_TYPE_AD5260		36		// 256-positions potentiometer

#define CODEC_TYPE_FPGAV1		37	// квадратуры получаем от FPGA
#define CODEC_TYPE_TLV320AIC23B	38	// TI TLV320AIC23B
#define CODEC_TYPE_CS4272		39	// CS CS4272
#define CODEC_TYPE_NAU8822L		40	// NUVOTON NAU8822L
#define CODEC_TYPE_WM8994		41	// CIRRUS LOGIC WM8994ECS/R

#define RTC_TYPE_DS1305		42	/* MAXIM DS1305EN RTC clock chip with SPI interface */
#define RTC_TYPE_DS1307		43	/* MAXIM DS1307/DS3231 RTC clock chip with I2C interface */
#define RTC_TYPE_M41T81		44	/* ST M41T81M6/M41T81SM6F RTC clock chip with I2C interface */
#define RTC_TYPE_STM32F4xx	45	/* STM32F4xx internal RTC peripherial */
#define RTC_TYPE_STM32F0xx	46	/* STM32F0xx internal RTC peripherial */
#define RTC_TYPE_STM32L0xx	47	/* STM32L0xx internal RTC peripherial */

#define TSC_TYPE_TSC2046	48	// Resistive touch screen controller TI TSC2046
#define TSC_TYPE_STMPE811	49	// Resistive touch screen controller ST STMPE811

// Start of NVRAM definitions section
// NOTE: DO NOT USE any types of FLASH memory chips, only EEPROM or FRAM chips are supported.
// поддерживаются только FRAM или EEPROM. FLASH не поддерживаются.
#define NVRAM_TYPE_FM25XXXX		50	/* SERIAL FRAM AUTODETECT	*/
#define NVRAM_TYPE_FM25L04		51	/* SERIAL FRAM 4KBit	*/
#define NVRAM_TYPE_FM25L16		52	/* SERIAL FRAM 16Kbit	*/
#define NVRAM_TYPE_FM25L64		53	/* SERIAL FRAM 64Kbit	*/
#define NVRAM_TYPE_FM25L256		54	/* SERIAL FRAM 256KBit	*/

#define NVRAM_TYPE_AT25040A		55	/* SERIAL EEPROM 4KBit	*/
#define NVRAM_TYPE_AT25L16		56	/* SERIAL EEPROM 16Kbit	*/
#define NVRAM_TYPE_AT25256A		57	/* SERIAL EEPROM 256KBit	*/

#define NVRAM_TYPE_CPUEEPROM		58	/* EEPROM память процессора */
#define	NVRAM_TYPE_BKPSRAM			59	/* Backup SRAM */
#define	NVRAM_TYPE_NOTHING			60	/* вообще отсутствует */

#define IF3_TYPE_DCRX	1
#define IF3_TYPE_128	2
#define IF3_TYPE_200	3
#define IF3_TYPE_215	4
#define IF3_TYPE_455	5
#define IF3_TYPE_500	6
#define IF3_TYPE_5000	7
#define IF3_TYPE_5500	8
#define IF3_TYPE_5645	9	// Drake R-4C and Drake T-4XC (Drake Twins) - 5645 kHz
#define IF3_TYPE_6000	10
#define IF3_TYPE_8000	11
#define IF3_TYPE_8192	12
#define IF3_TYPE_8215	13	// кварцевые фильтры от  трансивера FT-747 - 8215 kHz
#define IF3_TYPE_8868	14
#define IF3_TYPE_9045	15
#define IF3_TYPE_9000	16
#define IF3_TYPE_10000	17
#define IF3_TYPE_10700	18
#define IF3_TYPE_CUSTOM	19	// параметры частот задаются отдельными define, вынесеными в board\*_cylstyle_*.h
#define IF3_TYPE_BYPASS	20
#define IF3_TYPE_6000_SW2015	21	// слегка другая частота верхнего ската
#define IF3_TYPE_5250	22

/* все возможные фильтры. Не ноль соответствующем бите IF3_FMASK разрешает включение/выключение данного фильтра. */
#define IF3_FMASK_0P3	(1U << 0)	/* наличие фильтра 0.3 кГц	*/
#define IF3_FMASK_0P5	(1U << 1)	/* наличие фильтра 0.5 кГц	*/
#define IF3_FMASK_1P0	(1U << 2)	/* наличие фильтра 1.0 кГц	*/
#define IF3_FMASK_1P5	(1U << 3)	/* наличие фильтра 1.5 кГц	*/
#define IF3_FMASK_1P8	(1U << 4)	/* наличие фильтра 1.8 кГц	*/
#define IF3_FMASK_2P1	(1U << 5)	/* наличие фильтра 2.1 кГц	*/
#define IF3_FMASK_2P4	(1U << 6)	/* наличие фильтра 2.4 кГц	*/
#define IF3_FMASK_2P7	(1U << 7)	/* наличие фильтра 2.7 кГц	*/
#define IF3_FMASK_3P1	(1U << 8)	/* наличие фильтра 3.1 кГц	*/
#define IF3_FMASK_6P0	(1U << 9)	/* наличие фильтра 6.0 кГц	*/
#define IF3_FMASK_7P8	(1U << 10)	/* наличие фильтра 7.8 кГц	*/
#define IF3_FMASK_8P0	(1U << 11)	/* наличие фильтра 8.0 кГц	*/
#define IF3_FMASK_9P0	(1U << 12)	/* наличие фильтра 9.0 кГц	*/
#define IF3_FMASK_15P0	(1U << 13)	/* наличие фильтра 15.0 кГц	*/
#define IF3_FMASK_17P0	(1U << 14)	/* наличие фильтра 17.0 кГц	*/
#define IF3_FMASK_120P0	(1U << 15)	/* наличие фильтра 120 кГц	*/



#if defined (NVRAM_END)
	#error NVRAM_END already defined
#endif


#if 0	// Heavy eeprom size optimization
	// последний шанс уменьшить занимаемую программой в ПЗУ память.

	#define NVRAM_END	255U

#else
	#if defined (NVRAM_TYPE) && (NVRAM_TYPE == NVRAM_TYPE_FM25XXXX)
		#define NVRAM_END 511U	// если автоопределение - должно влезать в самй маленький чип.
	#elif defined (NVRAM_TYPE) && (NVRAM_TYPE == NVRAM_TYPE_FM25L04)
		#define NVRAM_END 511U
	#elif defined (NVRAM_TYPE) && (NVRAM_TYPE == NVRAM_TYPE_AT25040A)
		#define NVRAM_END 511U
	#elif defined (NVRAM_TYPE) && (NVRAM_TYPE == NVRAM_TYPE_FM25L16)
		#define NVRAM_END 2047U
	#elif defined (NVRAM_TYPE) && (NVRAM_TYPE == NVRAM_TYPE_AT25L16)
		#define NVRAM_END 2047U
	#elif defined (NVRAM_TYPE) && (NVRAM_TYPE == NVRAM_TYPE_FM25L64)
		#define NVRAM_END 8191U
	#elif defined (NVRAM_TYPE) && (NVRAM_TYPE == NVRAM_TYPE_FM25L256)
		#define NVRAM_END 32767U
	#elif defined (NVRAM_TYPE) && (NVRAM_TYPE == NVRAM_TYPE_AT25256A)

		#define NVRAM_END 32767U		/* на самом деле 64 килобайт, но в этом применении использeем не более 32K */

	#elif defined (NVRAM_TYPE) && (NVRAM_TYPE == NVRAM_TYPE_CPUEEPROM) && CPUSTYLE_ATMEGA

		#define NVRAM_END E2END

	#elif defined (NVRAM_TYPE) && (NVRAM_TYPE == NVRAM_TYPE_BKPSRAM) && CPUSTYLE_STM32F

		#define NVRAM_END 4095U		/* 4K of RAM at BKPSRAM_BASE */

	#endif // ATMEGA

#endif	// Heavy eeprom size optimization

#if defined (PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_MC145170)
	//#define WITHSPISW 	1	/* Использование программного контроллера SPI */
#endif
#if defined (PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_MC145170)
	//#define WITHSPISW 	1	/* Использование программного контроллера SPI */
#endif


#if LCDMODE_RDX0154 || LCDMODE_RDX0120 || LCDMODE_RDX0077 || LCDMODE_RDT065
	#define LCDMODE_UC1601 1
#endif /* LCDMODE_RDX0154 || LCDMODE_RDX0120 || LCDMODE_RDX0077 || LCDMODE_RDT065 */
#if LCDMODE_TIC218 || LCDMODE_TIC154
	#define LCDMODE_PCF8535 1
#endif /* LCDMODE_TIC218 */
#if LCDMODE_G1203H
	#define LCDMODE_PCF8531 1
#endif /* LCDMODE_G1203H */
#if LCDMODE_M9546
	#define LCDMODE_PCF8576C 1
#endif /* LCDMODE_M9546 */
#if LCDMODE_WH2002 || LCDMODE_WH1602 || LCDMODE_WH2004 || LCDMODE_WH1604 || LCDMODE_WH2002_IGOR
	#define LCDMODE_HD44780 1
#endif /* LCDMODE_WH2002 || LCDMODE_WH1602 || LCDMODE_WH2004 || LCDMODE_WH1604 ||LCDMODE_WH2002_IGOR */

#if LCDMODE_RDX0154
	#define LCDMODE_HARD_I2C	1	/* I2C interface */
	#define LCDMODE_SPI_RN	1	/* SPI only, with Reset, no Address need */
	#define DIM_X 132
	#define DIM_Y 64
#endif /* LCDMODE_RDX0154 */


#if LCDMODE_LQ043T3DX02K		/* PSP-1000 with S1D113780, or attached to LTDC hardware controller */
	//#define LCDMODE_HARD_SPI	1	/* SPI interface */
	//#define LCDMODE_SPI_RN	1	/* SPI only, with Reset, no Address need */
	#define DIM_X 480
	#define DIM_Y 272
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
	#define LTDC_DOTCLK	9000000uL	// частота пикселей при работе с интерфейсом RGB
#endif /* LCDMODE_S1D13781 */

#if LCDMODE_AT070TN90	/* AT070TN90 panel (800*480) - 7" display */
	#define LCDMODE_SPI_RN 1 // эти дисплеи требуют только RESET
	#define DIM_X 800
	#define DIM_Y 480
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
	#define LTDC_DOTCLK	30000000uL	// частота пикселей при работе с интерфейсом RGB
#endif /* LCDMODE_AT070TN90 */

#if LCDMODE_AT070TNA2	/* AT070TNA2 panel (1024*600) - 7" display */
	#define LCDMODE_SPI_RN 1 // эти дисплеи требуют только RESET
	#define DIM_X 1024
	#define DIM_Y 600
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
	#define LTDC_DOTCLK	51200000uL	// частота пикселей при работе с интерфейсом RGB 40.8..67.2
#endif /* LCDMODE_AT070TNA2 */

#if LCDMODE_S1D13781		/* PSP-1000 with S1D113780 */
	#define LCDMODE_HARD_SPI	1	/* SPI interface */
	#define LCDMODE_SPI_RN	1	/* SPI only, with Reset, no Address need */
	#define DIM_X 480
	#define DIM_Y 272
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
#endif /* LCDMODE_S1D13781 */

#if LCDMODE_TIC154 || LCDMODE_TIC218
	#define LCDMODE_HARD_I2C	1	/* I2C interface */
	#define LCDMODE_SPI_RN	1	/* SPI only, with Reset, no Address need */
	#define DIM_X 133
	#define DIM_Y 64
#endif /* LCDMODE_TIC154 || LCDMODE_TIC218 */

#if LCDMODE_RDX0077			/* RDX0077	*/
	#define LCDMODE_HARD_I2C	1	/* I2C interface */
	#define LCDMODE_SPI_RN	1	/* SPI only, with Reset, no Address need */
	#define DIM_X 128
	#define DIM_Y 64
#endif /* LCDMODE_RDX0077 */

#if LCDMODE_RDX0077			/* RDX0077	*/
	#define LCDMODE_HARD_I2C	1	/* I2C interface */
	#define LCDMODE_SPI_RN	1	/* SPI only, with Reset, no Address need */
	#define DIM_X 128
	#define DIM_Y 64
#endif /* LCDMODE_RDX0077 */

#if LCDMODE_RDT065				/* RDT065 - Индикатор 132*64 с контроллером UC1601, XMIRROR & TOPDOWN */
	#define LCDMODE_HARD_I2C	1	/* I2C interface */
	#define LCDMODE_SPI_RN	1	/* SPI only, with Reset, no Address need */
	#define DIM_X 132
	#define DIM_Y 64
#endif /* LCDMODE_RDT065 */

#if LCDMODE_G1203H				/* G1203H	*/
	#define LCDMODE_HARD_I2C	1	/* I2C interface */
	#define LCDMODE_SPI_RN	1	/* SPI only, with Reset, no Address need */
	#define DIM_X 128
	#define DIM_Y 32
#endif /* LCDMODE_G1203H */

#if LCDMODE_PCF8535 || LCDMODE_UC1601 || LCDMODE_PCF8531
	#define LCDMODE_HARD_I2C	1	/* I2C interface */
#endif

#if LCDMODE_UC1601
	#define LCDMODE_HARD_I2C	1	/* I2C interface */
	#define LCDMODE_SPI_RN	1	/* SPI only, with Reset, no Address need */
#elif LCDMODE_LS020
	#define LCDMODE_HARD_SPI	1	/* SPI interface */
	#define LCDMODE_SPI_RA	1	/* SPI only, with Reset, with Address */
#elif LCDMODE_LPH88 || LCDMODE_EM027BS013
	#define LCDMODE_HARD_SPI	1	/* SPI interface */
	#define LCDMODE_SPI_RN	1	/* SPI only, with Reset, no Address need */
#elif LCDMODE_L2F50
	#define LCDMODE_HARD_SPI	1	/* SPI interface */
	#define LCDMODE_SPI_RA	1	/* SPI only, with Reset, with Address */
#endif

#if LCDMODE_LS020 || LCDMODE_LPH88 || LCDMODE_L2F50	/* LS02 color graphics display */
	#define DIM_X 176
	#define DIM_Y 132
	#define LCDMODE_COLORED	1
#endif /* LCDMODE_LS020 */

#if LCDMODE_ILI9320	/* ILI9320 color graphics display */
	#define LCDMODE_HARD_SPI	1	/* SPI interface */
	#define LCDMODE_SPI	1	/* SPI only, no Reset, no Address need */
	#define DIM_X 320
	#define DIM_Y 240
	#define LCDMODE_COLORED	1
#endif /* LCDMODE_ILI9320 */

#if LCDMODE_ILI9225	/* SF-TC220H-9223A-N - ILI9225С based color graphics display */
	#define LCDMODE_HARD_SPI	1	/* SPI interface */
	#define LCDMODE_SPI_RA	1	/* SPI only, with Reset, with Address */
	#define DIM_X 220
	#define DIM_Y 176
	#define LCDMODE_COLORED	1
#endif /* LCDMODE_ILI9225 */

#if LCDMODE_ST7781	/* 328*240 TFT color display  */
	//#define LCDMODE_HARD_SPI	1	/* SPI interface */
	#define LCDMODE_SPI_RA	1	/* SPI only, with Reset, with Address */
	#define DIM_X 320
	#define DIM_Y 240
	#define LCDMODE_COLORED	1
#endif /* LCDMODE_ST7781 */

#if LCDMODE_UC1608
	#define LCDMODE_HARD_SPI	1	/* SPI interface */
	#define LCDMODE_SPI_RA	1	/* SPI only, with Reset, with Address */
	#define DIM_X 240
	#define DIM_Y 128
#endif /* LCDMODE_UC1608 */

#if LCDMODE_ST7735
	#define LCDMODE_HARD_SPI	1	/* SPI interface */
	#define LCDMODE_SPI_RA	1	/* SPI only, with Reset, with Address */
	#define DIM_X 160
	#define DIM_Y 128
	#define LCDMODE_COLORED	1
	#define LCDMODE_RGB565 1	// старый дисплей
	//#define LCDMODE_BGR565 1	// перевернутые цыета
#endif /* LCDMODE_ST7735 */

#if LCDMODE_ILI9341
	#define LCDMODE_HARD_SPI	1	/* SPI interface */
	#define LCDMODE_SPI_RA	1	/* SPI only, with Reset, with Address */
	#define DIM_X 320
	#define DIM_Y 240
	#define LCDMODE_COLORED	1
	#define LTDC_DOTCLK	3000000uL	// частота пикселей при работе с интерфейсом RGB
	#define LCDMODE_RGB565 1
#endif

#if LCDMODE_ILI8961
	#define LCDMODE_HARD_SPI	1	/* SPI interface */
	#define LCDMODE_SPI	1	/* SPI only, no Reset, no Address need */
	#define DIM_X 320
	#define DIM_Y 240
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
	#define LTDC_DOTCLK	24000000uL	// частота пикселей при работе с интерфейсом RGB (в результате оказалось 7.2 МГц)
#endif

#if LCDMODE_ILI9163			/* этот контроллер обслуживается в основном совпадающим с ST7735 кодом */
	#define LCDMODE_HARD_SPI	1	/* SPI interface */
	#define LCDMODE_SPI_RA	1	/* SPI only, with Reset, with Address */
	#define DIM_X 176
	#define DIM_Y 132
	#define LCDMODE_COLORED	1
	#define LCDMODE_BGR565 1


	#if LCDMODE_ILI9163_TOPDOWN
		#undef LCDMODE_ST7735_TOPDOWN 	// в основном совпадающий набор функций
	#else
		#define LCDMODE_ST7735_TOPDOWN 1	// в основном совпадающий набор функций
	#endif
#endif

#if LCDMODE_ST7565S || LCDMODE_PTE1206
	#define LCDMODE_HARD_SPI	1	/* SPI interface */
	#define LCDMODE_SPI_RA	1	/* SPI only, with Reset, with Address */
	#define DIM_X 128
	#define DIM_Y 64
#endif /* LCDMODE_ST7565S || LCDMODE_PTE1206 */

/*
 * Выбор описателя расположения элементов, отбражаемых на дисплее.
 */
#if DIM_X == 480 && DIM_Y == 272
	#define DSTYLE_G_X480_Y272	1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 800 && DIM_Y == 480
	#define DSTYLE_G_X800_Y480	1	/* AT070TN90 panel (800*480) - 7" display */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 1024 && DIM_Y == 600
	#define DSTYLE_G_X800_Y480	1	/* AT070TN90 panel (800*480) - 7" display */
	//#define DSTYLE_G_X1024_Y600	1	/* AT070TNA2 panel (1024*600) - 7" display */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 220 && DIM_Y == 176
	#define DSTYLE_G_X220_Y176	1	// Индикатор 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 с контроллером ILI9225С
	#define CHAR_W	8
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */
		
#elif DIM_X == 240 && DIM_Y == 128
	#define DSTYLE_G_X240_Y128	1
	#define CHAR_W	8
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 2)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 160 && DIM_Y == 128
	#define DSTYLE_G_X160_Y128	1	// ST7735
	#define CHAR_W	8
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 128 && DIM_Y == 64
	#define DSTYLE_G_X128_Y64	1	
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 132 && DIM_Y == 64
	#define DSTYLE_G_X132_Y64	1
	//#define DSTYLE_G_X128_Y64	1	/* RDX0077 - проверки дизайна на других индикаторах	*/
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 133 && DIM_Y == 64
	// LCDMODE_TIC154
	// LCDMODE_TIC218
	#define DSTYLE_G_X132_Y64	1
	//#define DSTYLE_G_X128_Y64	1	/* RDX0077 - проверки дизайна на других индикаторах	*/
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 176 && DIM_Y == 132		
	#define DSTYLE_G_X176_Y132	1		// ls020/lph88
	#define CHAR_W	8
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 320 && DIM_Y == 240
	#define DSTYLE_G_X320_Y240	1	
	#define CHAR_W	10
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 64 && DIM_Y == 32
	#define DSTYLE_G_X64_Y32	1	
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 128 && DIM_Y == 32
	#define DSTYLE_G_X64_Y32	1	/* G1203H - проверки	*/
	//#define DSTYLE_G_X128_Y32	1	
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif LCDMODE_HD44780 && LCDMODE_WH2002
	#define DSTYLE_T_X20_Y2	1	

#elif LCDMODE_HD44780 && LCDMODE_WH2002_IGOR
	#define DSTYLE_T_X20_Y2_IGOR	1	

#elif LCDMODE_HD44780 && LCDMODE_WH2004
	#define DSTYLE_T_X20_Y4	1	

#elif LCDMODE_HD44780 && LCDMODE_WH1602
	#define DSTYLE_T_X16_Y2	1	

#elif LCDMODE_HD44780 && LCDMODE_WH1604
	#define DSTYLE_T_X16_Y4	1	

#elif LCDMODE_DUMMY

	#define DSTYLE_G_DUMMY	1	// пустой список лтображения

	#define DIM_X 480
	#define DIM_Y 272
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
	#define LTDC_DOTCLK	9000000uL	// частота пикселей при работе с интерфейсом RGB

	#define HALFCOUNT_FREQA 1
	#define HALFCOUNT_SMALL 1
	#define HALFCOUNT_BIG 1
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */


	#define DISPLAYMODES_FPS 5	/* количество обновлений отображения режимов работы */
	#define DISPLAY_FPS	10	/* обновление показаний частоты за секунду */
	#define DISPLAYSWR_FPS 5	/* количество обновлений SWR за секунду */

	// Цветное изображение не поддерживается на этом дисплее (заглушка).
	#define TFTRGB(r, g, b)	(0)
	typedef uint_fast16_t COLOR_T;	/* цвета не поддерживаются - заглушка */
	typedef uint16_t PACKEDCOLOR_T;

	#define DISPLAY_BUS_INITIALIZE() do {} while (0)

#else
	#error Unrecognized dislay layout used (LCDMODE_XXXX)
#endif


#if LCDMODE_HARD_I2C && LCDMODE_HARD_SPI
	#error Wrong LCDMODE_HARD_I2C and LCDMODE_HARD_SPI definitions
#endif

// Определения функциональности в зависимости от того, какой набор управляющих битов имеется.
#if CTLREGSTYLE_SW2012_MINI

	#define WITHPOWERTRIM		1	// Имеется управление мощностью
	#define WITHPOWERTRIMMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHPOWERTRIMMAX	4	// Верхний предел регулировки (показываемый на дисплее)
	//#define WITHPOWERTRIMATU	2	// Значение для работы автотюнера
	#define	WITHMUTEALL			1	// Отключение микрофона во всех режимах
	#define WITHONEATTONEAMP	1	/* только одно положение аттенюатора и УВЧ */


	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	#define BOARD_DETECTOR_WFM 0x00
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

	//#define	BOARD_AGCCODE_0		0x00
	//#define	BOARD_AGCCODE_1		0x01
	#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */
	#define BOARD_AGCCODE_OFF	0

#elif CTLREGSTYLE_SW2012C

	#define WITHPOWERTRIM		1	// Имеется управление мощностью
	#define WITHPOWERTRIMMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHPOWERTRIMMAX	4	// Верхний предел регулировки (показываемый на дисплее)
	//#define WITHPOWERTRIMATU	2	// Значение для работы автотюнера
	#define	WITHMUTEALL			1	// Отключение микрофона во всех режимах
	#define WITHONEATTONEAMP	1	/* только одно положение аттенюатора и УВЧ */


	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	#define BOARD_DETECTOR_WFM 0x00
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

	//#define	BOARD_AGCCODE_0		0x00
	//#define	BOARD_AGCCODE_1		0x01
	#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */
	#define BOARD_AGCCODE_OFF	0

#elif CTLREGSTYLE_SW2012CN
	// управляющие регистры SW2012MINI COLOR 2 c цветным дисплеем ILI9163 и регулировкой выходной мощности
	// управляющие регистры SW2016MINI

	#define WITHPOWERTRIM		1	// Имеется управление мощностью
	#define WITHPOWERTRIMMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHPOWERTRIMMAX	4	// Верхний предел регулировки (показываемый на дисплее)
	//#define WITHPOWERTRIMATU	2	// Значение для работы автотюнера
	#define	WITHMUTEALL			1	// Отключение микрофона во всех режимах
	#define WITHONEATTONEAMP	1	/* только одно положение аттенюатора и УВЧ */


	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	#define BOARD_DETECTOR_WFM 0x00
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

	//#define	BOARD_AGCCODE_0		0x00
	//#define	BOARD_AGCCODE_1		0x01
	#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */
	#define BOARD_AGCCODE_OFF	0

#elif CTLREGSTYLE_SW2012CN_RN3ZOB

	#define WITHPOWERTRIM		1	// Имеется управление мощностью
	#define WITHPOWERTRIMMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHPOWERTRIMMAX	4	// Верхний предел регулировки (показываемый на дисплее)
	//#define WITHPOWERTRIMATU	2	// Значение для работы автотюнера
	#define	WITHMUTEALL			1	// Отключение микрофона во всех режимах
	#define WITHONEATTONEAMP	1	/* только одно положение аттенюатора и УВЧ */


	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	#define BOARD_DETECTOR_WFM 0x00
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

	//#define	BOARD_AGCCODE_0		0x00
	//#define	BOARD_AGCCODE_1		0x01
	#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */
	#define BOARD_AGCCODE_OFF	0

#elif CTLREGSTYLE_SW2013SF

	#define WITHPOWERLPHP		1	// Имеется переключение и отображение HP / LP
	#define WITHPOWERTRIMMIN	0	// Нижний предел регулировки
	#define WITHPOWERTRIMMAX	1	// Верхний предел регулировки
	//#define WITHPOWERTRIMATU	0	// Значение для работы автотюнера
	#define	WITHMUTEALL			1	// Отключение микрофона во всех режимах
	#define WITHONEATTONEAMP	1	/* только одно положение аттенюатора и УВЧ */


	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	#define BOARD_DETECTOR_WFM 0x00
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

	//#define	BOARD_AGCCODE_0		0x00
	//#define	BOARD_AGCCODE_1		0x01
	#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */
	#define BOARD_AGCCODE_OFF	0

#elif CTLREGSTYLE_SW2013SF_V1		// for UT4UA

	#define WITHPOWERLPHP		1	// Имеется переключение и отображение HP / LP
	#define WITHPOWERTRIMMIN	0	// Нижний предел регулировки
	#define WITHPOWERTRIMMAX	1	// Верхний предел регулировки
	//#define WITHPOWERTRIMATU	0	// Значение для работы автотюнера
	#define	WITHMUTEALL			1	// Отключение микрофона во всех режимах
	#define WITHONEATTONEAMP	1	/* только одно положение аттенюатора и УВЧ */
	#define WITHLCDBACKLIGHT	1	// Имеется управление подсветкой дисплея
	#define WITHLCDBACKLIGHTMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываемый на дисплее)

	//#define	BOARD_AGCCODE_0		0x00
	//#define	BOARD_AGCCODE_1		0x01
	#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */
	#define BOARD_AGCCODE_OFF	0

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	#define BOARD_DETECTOR_WFM 0x00
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

#elif CTLREGSTYLE_SW2013RDX

	#define WITHLCDBACKLIGHT	1	// Имеется управление подсветкой дисплея
	#define WITHLCDBACKLIGHTMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHLCDBACKLIGHTMAX	4	// Верхний предел регулировки (показываемый на дисплее)
	#define WITHPOWERLPHP		1	// Имеется переключение и отображение HP / LP
	#define WITHPOWERTRIMMIN	0	// Нижний предел регулировки
	#define WITHPOWERTRIMMAX	1	// Верхний предел регулировки
	#define	WITHMUTEALL			1	// Отключение микрофона во всех режимах
	#define WITHONEATTONEAMP	1	/* только одно положение аттенюатора и УВЧ */

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	#define BOARD_DETECTOR_WFM 0x00
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

	//#define	BOARD_AGCCODE_0		0x00
	//#define	BOARD_AGCCODE_1		0x01
	#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */
	#define BOARD_AGCCODE_OFF	0
	#define WITHNOTCHONOFF		1	/* notch on/off */

#elif CTLREGSTYLE_SW2013RDX_UY5UM_WO240

	#define WITHLCDBACKLIGHT	1	// Имеется управление подсветкой дисплея
	#define WITHLCDBACKLIGHTMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHLCDBACKLIGHTMAX	4	// Верхний предел регулировки (показываемый на дисплее)
	#define WITHPOWERLPHP		1	// Имеется переключение и отображение HP / LP
	#define WITHPOWERTRIMMIN	0	// Нижний предел регулировки
	#define WITHPOWERTRIMMAX	1	// Верхний предел регулировки
	#define	WITHMUTEALL			1	// Отключение микрофона во всех режимах
	#define WITHONEATTONEAMP	1	/* только одно положение аттенюатора и УВЧ */

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	#define BOARD_DETECTOR_WFM 0x00
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

	//#define	BOARD_AGCCODE_0		0x00
	//#define	BOARD_AGCCODE_1		0x01
	#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */
	#define BOARD_AGCCODE_OFF	0
	#define WITHNOTCHONOFF		1	/* notch on/off */

#elif CTLREGSTYLE_SW2014NFM

	#define WITHPOWERLPHP		1	// Имеется переключение и отображение HP / LP
	#define WITHPOWERTRIMMIN	0	// Нижний предел регулировки
	#define WITHPOWERTRIMMAX	1	// Верхний предел регулировки
	#define	WITHMUTEALL			1	// Отключение микрофона во всех режимах
	#define WITHONEATTONEAMP	1	/* только одно положение аттенюатора и УВЧ */

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	#define BOARD_DETECTOR_WFM 0x00
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

	//#define	BOARD_AGCCODE_0		0x00
	//#define	BOARD_AGCCODE_1		0x01
	#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */
	#define BOARD_AGCCODE_OFF	0

	#define WITHNBONOFF		1	/* управление включением-выключением шумоподавителя при приёме NFM */
	#define WITHSUBTONES	1	/* выполняется формирование субтона при передаче NFM */


#elif CTLREGSTYLE_SW2016VHF

	#define WITHPOWERLPHP		1	// Имеется переключение и отображение HP / LP
	#define WITHPOWERTRIMMIN	0	// Нижний предел регулировки
	#define WITHPOWERTRIMMAX	1	// Верхний предел регулировки
	#define	WITHMUTEALL			1	// Отключение микрофона во всех режимах
	#define WITHONEATTONEAMP	1	/* только одно положение аттенюатора и УВЧ */

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	#define BOARD_DETECTOR_WFM 0x00
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

	//#define	BOARD_AGCCODE_0		0x00
	//#define	BOARD_AGCCODE_1		0x01
	#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */
	#define BOARD_AGCCODE_OFF	0

	#define WITHNBONOFF		1	/* управление включением-выключением шумоподавителя при приёме NFM */
	#define WITHSUBTONES	1	/* выполняется формирование субтона при передаче NFM */

#elif CTLREGSTYLE_SW2013RDX_UY5UM	/* с регулировкой мощности R-2R на сигналах выбора диапазонного фильтра */

	#define WITHPOWERTRIM		1	// Имеется управление мощностью
	#define WITHPOWERTRIMMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHPOWERTRIMMAX	16	// Верхний предел регулировки (показываемый на дисплее)
	//#define WITHPOWERTRIMATU	4	// Значение для работы автотюнера

	#define WITHLCDBACKLIGHT	1	// Имеется управление подсветкой дисплея
	#define WITHLCDBACKLIGHTMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHLCDBACKLIGHTMAX	4	// Верхний предел регулировки (показываемый на дисплее)
	#define	WITHMUTEALL			1	// Отключение микрофона во всех режимах
	#define WITHONEATTONEAMP	1	/* только одно положение аттенюатора и УВЧ */

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	#define BOARD_DETECTOR_WFM 0x00
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

	//#define	BOARD_AGCCODE_0		0x00
	//#define	BOARD_AGCCODE_1		0x01
	#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */
	#define BOARD_AGCCODE_OFF	0

#elif CTLREGSTYLE_SW2013RDX_LTIYUR	// SW2013SF с индикатором RDX154

	#define WITHLCDBACKLIGHT	1	// Имеется управление подсветкой дисплея
	#define WITHLCDBACKLIGHTMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHLCDBACKLIGHTMAX	4	// Верхний предел регулировки (показываемый на дисплее)
	#define WITHPOWERLPHP		1	// Имеется переключение и отображение HP / LP
	#define WITHPOWERTRIMMIN	0	// Нижний предел регулировки
	#define WITHPOWERTRIMMAX	1	// Верхний предел регулировки
	#define	WITHMUTEALL			1	// Отключение микрофона во всех режимах
	#define WITHONEATTONEAMP	1	/* только одно положение аттенюатора и УВЧ */

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	#define BOARD_DETECTOR_WFM 0x00
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

	//#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */
	#define WITHAGCMODEONOFF 1	/* среди режимов АРУ есть только включено-выключено */
	#define WITHNOTCHONOFF		1	/* notch on/off */

	#define	BOARD_AGCCODE_0		0x00
	#define	BOARD_AGCCODE_1		0x01

#elif CTLREGMODE8_UR5YFV	/* 8 бит - версия синтезатора 4z5ky */

	#define WITHONEATTONEAMP	1	/* только одно положение аттенюатора и УВЧ - "по кругу". */
	#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	#define	BOARD_FILTERCODE_1	0x01

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	//#define BOARD_FILTER_0P5		0x01	/* 0.5 or 0.3 kHz filter */
	//#define BOARD_FILTER_3P1		0x00	/* 3.1 or 2.75 kHz filter */

#elif CTLREGMODE16

	/* Виды управление УВЧ и аттенюатором */
	//#define WITHATT1PRE1		1	// Управление УВЧ и одиночным аттенюатором
	//#define WITHATT2_6DB		1	// Управление двухкаскадным аттенюатором без управления УВЧ
	//#define WITHONEATTONEAMP	1	/* только одно положение аттенюатора и УВЧ - "по кругу". */
	#define WITHPREAMPATT2_6DB		1	// Управление УВЧ и двухкаскадным аттенюатором с затуханиями 0 - 6 - 12 - 18 dB */

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */
	//#define BOARD_DETECTOR_WFM 0x04
	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

	#define	BOARD_AGCCODE_0		0x00
	#define	BOARD_AGCCODE_1		0x01
	#define WITHAGCMODENONE 1	/* среди режимов АРУ есть только включено-выключено */
	#define BOARD_AGCCODE_OFF	1

#elif CTLREGMODE16_UR3VBM

	/* Виды управление УВЧ и аттенюатором */
	//#define WITHATT1PRE1		1	// Управление УВЧ и одиночным аттенюатором
	//#define WITHATT2_6DB		1	// Управление двухкаскадным аттенюатором без управления УВЧ
	//#define WITHONEATTONEAMP	1	/* только одно положение аттенюатора и УВЧ - "по кругу". */
	#define WITHPREAMPATT2_6DB		1	// Управление УВЧ и двухкаскадным аттенюатором с затуханиями 0 - 6 - 12 - 18 dB */

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */
	//#define BOARD_DETECTOR_WFM 0x04
	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

	#define	BOARD_AGCCODE_0		0x00
	#define	BOARD_AGCCODE_1		0x01
	#define WITHAGCMODENONE 1	/* среди режимов АРУ есть только включено-выключено */
	#define BOARD_AGCCODE_OFF	1

#elif CTLREGMODE16_RN3ZOB
	#define WITHATT2_6DB		1	// Управление двухкаскадным аттенюатором без управления УВЧ
	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	//#define BOARD_DETECTOR_WFM 0x04
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */
	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

	#define	BOARD_AGCCODE_0		0x00
	#define	BOARD_AGCCODE_1		0x01
	//#define WITHAGCMODEONOFF 1	/* среди режимов АРУ есть только включено-выключено */
#elif CTLREGMODE16_DC

	#define WITHPREAMPATT2_6DB		1	// Управление УВЧ и двухкаскадным аттенюатором с затуханиями 0 - 6 - 12 - 18 dB */
	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	//#define BOARD_DETECTOR_WFM 0x04
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */
	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

	#define	BOARD_AGCCODE_0		0x00
	#define	BOARD_AGCCODE_1		0x01
	//#define WITHAGCMODEONOFF 1	/* среди режимов АРУ есть только включено-выключено */

#elif CTLREGSTYLE_SW2011

	#define WITHLCDBACKLIGHT	1	// Имеется управление подсветкой дисплея
	#define WITHLCDBACKLIGHTMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываемый на дисплее)
	#define	WITHMUTEALL			1	// Отключение микрофона во всех режимах
	#define WITHONEATTONEAMP	1	/* только одно положение аттенюатора и УВЧ */


	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	//#define BOARD_DETECTOR_WFM 0x04
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

	//#define	BOARD_AGCCODE_0		0x00
	//#define	BOARD_AGCCODE_1		0x01
	#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */
	#define BOARD_AGCCODE_OFF	0	// stub

#elif CTLREGSTYLE_SW2011N

	#define WITHLCDBACKLIGHT	1	// Имеется управление подсветкой дисплея
	#define WITHLCDBACKLIGHTMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываемый на дисплее)
	#define	WITHMUTEALL			1	// Отключение микрофона во всех режимах
	#define WITHONEATTONEAMP	1	/* только одно положение аттенюатора и УВЧ */


	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x00
	#define BOARD_DETECTOR_FM 0x00
	//#define BOARD_DETECTOR_WFM 0x04
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */
	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01

	#define WITHMODESETMIXONLY3 1

	//#define	BOARD_AGCCODE_0		0x00
	//#define	BOARD_AGCCODE_1		0x01
	#define WITHAGCMODENONE		1	/* Режимами АРУ не управляем */
	#define BOARD_AGCCODE_OFF	0	// stub

#elif CTLREGMODE32_V1	/* управляющий регистр - "Воробей-3" с 3*ULN2003 */

	#define WITHLCDBACKLIGHT	1	// Имеется управление подсветкой дисплея
	#define WITHLCDBACKLIGHTMIN	0	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываемый на дисплее)
	#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры

	/* коды входов коммутатора источников сигнала для УНЧ приёмника (2 бита) */
	#define BOARD_DETECTOR_FM 	0x01
	#define BOARD_DETECTOR_SSB 	0x00
	#define BOARD_DETECTOR_AM 	0x02
	#define BOARD_DETECTOR_MUTE 0x00
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x01
	#define	BOARD_FILTERCODE_1	0x02
	#define	BOARD_FILTERCODE_2	0x04
	//#define WITHAGCMODEONOFF 1	/* среди режимов АРУ есть только включено-выключено */
	//#define	BOARD_AGCCODE_0		0x00
	//#define	BOARD_AGCCODE_1		0x01
	#define	BOARD_AGCCODE_ON		0x00
	#define	BOARD_AGCCODE_OFF		0x01

#elif CTLREGMODE24_V1	/* управляющий регистр - "Воробей" и "Колибри" */

	#define WITHLCDBACKLIGHT	1	// Имеется управление подсветкой дисплея
	#define WITHLCDBACKLIGHTMIN	0	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHLCDBACKLIGHTMAX	3	// Верхний предел регулировки (показываемый на дисплее)
	#define WITHKBDBACKLIGHT	1	// Имеется управление подсветкой клавиатуры

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 	(0x02)
	#define BOARD_DETECTOR_SSB 	(0x00)
	#define BOARD_DETECTOR_AM 	(0x01)
	#define BOARD_DETECTOR_FM 	(0x03)
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */
	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01
	#define	BOARD_FILTERCODE_2	0x02
	#define	BOARD_FILTERCODE_3	0x03
	#define	BOARD_AGCCODE_ON		0x00
	#define	BOARD_AGCCODE_OFF		0x01

#elif CTLREGMODE24_UA3DKC

	#define WITHPREAMPATT2_10DB		1	// Управление УВЧ и двухкаскадным аттенюатором
	#define WITHAGCMODEONOFF	1	// АРУ вкл/выкл
	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0x02
	#define BOARD_DETECTOR_SSB 0x00
	#define BOARD_DETECTOR_AM 0x01
	#define BOARD_DETECTOR_FM 0x03
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x00
	#define	BOARD_FILTERCODE_1	0x01
	#define	BOARD_FILTERCODE_2	0x02
	#define	BOARD_FILTERCODE_3	0x03

	#define	BOARD_AGCCODE_ON	0x00
	#define	BOARD_AGCCODE_OFF	0x01

#elif CTLREGMODE_V8A			// воронёнок

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 	(0x02)
	#define BOARD_DETECTOR_SSB 	(0x00)
	#define BOARD_DETECTOR_AM 	(0x01)
	#define BOARD_DETECTOR_FM 	(0x03)
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */

	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0x01
	#define	BOARD_FILTERCODE_1	0x02
	#define	BOARD_FILTERCODE_2	0x04
	#define	BOARD_FILTERCODE_3	0x08

	#define WITHPREAMPATT2_6DB		1	// Управление УВЧ и двухкаскадным аттенюатором с затуханиями 0 - 6 - 12 - 18 dB */

	#if WITHIF4DSP
		// Временно, для проверки
		//#define WITHAGCMODEONOFF	1	// АРУ вкл/выкл
		//#define	WITHMIC1LEVEL		1	// установка усиления микрофона

	#else /* WITHIF4DSP */

		#define	BOARD_AGCCODE_0		0x00
		#define	BOARD_AGCCODE_1		0x01
		#define	BOARD_AGCCODE_2		0x02
		#define	BOARD_AGCCODE_3		0x04
		#define	BOARD_AGCCODE_OFF	0x08

		#define	BOARD_AGCCODE_FAST	BOARD_AGCCODE_0
		#define	BOARD_AGCCODE_MED	BOARD_AGCCODE_1
		#define	BOARD_AGCCODE_SLOW	BOARD_AGCCODE_2
		#define	BOARD_AGCCODE_LONG	BOARD_AGCCODE_3
		#define WITHAGCMODE5STAGES	1	// 4 скорости и выключенно
	#endif /* WITHIF4DSP */

#elif CTLSTYLE_V5

	#define WITHPREAMPATT2_10DB		1	// Управление УВЧ и двухкаскадным аттенюатором

	#define	BOARD_AGCCODE_0		0x00
	#define	BOARD_AGCCODE_1		0x01
	#define	BOARD_AGCCODE_2		0x02
	#define	BOARD_AGCCODE_3		0x04
	#define	BOARD_AGCCODE_OFF	0x08

	#define	BOARD_AGCCODE_FAST	BOARD_AGCCODE_0
	#define	BOARD_AGCCODE_MED	BOARD_AGCCODE_1
	#define	BOARD_AGCCODE_SLOW	BOARD_AGCCODE_2
	#define	BOARD_AGCCODE_LONG	BOARD_AGCCODE_3

#elif (ATMEGA_CTLSTYLE_V7_H_INCLUDED || ARM_CTLSTYLE_V7_H_INCLUDED || ARM_CTLSTYLE_V7A_H_INCLUDED)

	#define WITHPREAMPATT2_10DB		1	// Управление УВЧ и двухкаскадным аттенюатором
	#define WITHAGCMODE5STAGES	1	// 4 скорости и выключенно

	#define	BOARD_AGCCODE_0		0x00
	#define	BOARD_AGCCODE_1		0x01
	#define	BOARD_AGCCODE_2		0x02
	#define	BOARD_AGCCODE_3		0x04
	#define	BOARD_AGCCODE_OFF	0x08

	#define	BOARD_AGCCODE_FAST	BOARD_AGCCODE_0
	#define	BOARD_AGCCODE_MED	BOARD_AGCCODE_1
	#define	BOARD_AGCCODE_SLOW	BOARD_AGCCODE_2
	#define	BOARD_AGCCODE_LONG	BOARD_AGCCODE_3

#elif CTLREGSTYLE_DISCO32

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0
	#define BOARD_DETECTOR_SSB 	0
	#define BOARD_DETECTOR_AM 	0
	#define BOARD_DETECTOR_FM 	0
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */
	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0
	#define	BOARD_FILTERCODE_1	0
	#define	BOARD_FILTERCODE_2	0
	#define	BOARD_FILTERCODE_3	0

#elif CTLREGSTYLE_NOCTLREG			// Отсутствующий регистр управления

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0
	#define BOARD_DETECTOR_SSB 	0
	#define BOARD_DETECTOR_AM 	0
	#define BOARD_DETECTOR_FM 	0
	#define BOARD_DETECTOR_TUNE 0x00	/* конфигурация платы для режима TUNE (CWZ на передачу) */
	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0
	#define	BOARD_FILTERCODE_1	0
	#define	BOARD_FILTERCODE_2	0
	#define	BOARD_FILTERCODE_3	0

#else
	//#error Undefined CTLREGMODE_xxx option
#endif
#define WITHHARDINTERLOCK (WITHNESTEDINTERRUPTS && (CPUSTYLE_ARM_CM7 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM0 || CPUSTYLE_ARM_CA9 || CPUSTYLE_ARM_CA7))

#ifndef WITHFLATMENU
	#define WITHFLATMENU (CTLSTYLE_SW2011ALL && ! CPUSTYLE_ATMEGA_XXX4)
#endif /* WITHFLATMENU */

#define HARDWARE_DELAY_MS(t) do { local_delay_ms(t); } while (0)	//HAL_Delay(t)
#define HARDWARE_DELAY_US(t) do { local_delay_us(t); } while (0)
#define HARDWARE_GETTICK_MS() ((uint32_t) 0) // HAL_GetTick()

#define WITHSPECTRUMWF (WITHIF4DSP && (WITHRTS96 || WITHRTS192) && ! LCDMODE_HD44780)

/* Сброс мощности при запросе TUNE от автотюнера или извне */
#define WITHLOWPOWEREXTTUNE	(defined (HARDWARE_GET_TUNE) || WITHAUTOTUNER)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PARAMDEPEND_H_INCLUDED */
