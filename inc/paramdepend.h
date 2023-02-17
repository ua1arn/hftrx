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
	//#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ARM_OVERREALTIME_PRIORITY	AT91C_AIC_PRIOR_HIGHEST
	#define ARM_REALTIME_PRIORITY		(AT91C_AIC_PRIOR_LOWEST + 1)
	#define ARM_SYSTEM_PRIORITY			AT91C_AIC_PRIOR_LOWEST

	#define ADCVREF_CPU	33		// 3.3 volt
	//#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCBITS 8	/* АЦП работает с 8-битными значениями */
	//#define HARDWARE_ADCINPUTS	40	/* до 8-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений


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
	//#define BOARD_SYSTICK_FREQ CPU_FREQ	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

	#define TICKS_FREQUENCY		(200uL * 1) // at ARM - 400 Hz

	// ADC clock frequency: 0.6..14 MHz
	#define ADC_FREQ	12000000uL	/* тактовая частота SAR преобразователя АЦП. */
	#define SCL_CLOCK	400000uL	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (PCLK1_FREQ / 4)	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	//#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ADCVREF_CPU	33		// 3.3 volt
	#define DACVREF_CPU	33		// 3.3 volt
	#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */
	//#define HARDWARE_ADCINPUTS	40	/* до 32-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений
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
	//#define BOARD_SYSTICK_FREQ CPU_FREQ	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

	#define TICKS_FREQUENCY		(200uL * 1) // at ARM - 200 Hz

	// ADC clock frequency: 0.6..14 MHz
	#define ADC_FREQ	12000000uL	/* тактовая частота SAR преобразователя АЦП. */
	#define SCL_CLOCK	400000uL	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (PCLK1_FREQ / 1)	/* 8 MHz на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	//#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ADCVREF_CPU	30		// 3.3 volt
	#define DACVREF_CPU	33		// 3.3 volt
	#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */
	//#define HARDWARE_ADCINPUTS	40	/* до 32-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений
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
	//#define BOARD_SYSTICK_FREQ CPU_FREQ	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

	#define SPISPEED (PCLK1_FREQ / 4)	/* 9.0 MHz на SCLK - требуемая скорость передачи по SPI */
	#define TICKS_FREQUENCY		(200uL * 1) // at ARM - 200 Hz
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	//#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */


	// ADC clock frequency: 1..20 MHz
	#define ADC_FREQ	2000000uL	/* тактовая частота SAR преобразователя АЦП. */
	//#define ADC_FREQ	16000000uL	/* тактовая частота SAR преобразователя АЦП. */

	#define SCL_CLOCK	400000uL	/* 400 kHz I2C/TWI speed */

	#define ADCVREF_CPU	33		// 3.3 volt
	#define DACVREF_CPU	33		// 3.3 volt
	#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */
	//#define HARDWARE_ADCINPUTS	40	/* до 32-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений
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
			// с внешним генератором
			#define	REFINFREQ WITHCPUXOSC
		#elif WITHCPUXTAL
			// с внешним кварцевым резонатором
			#define	REFINFREQ WITHCPUXTAL
		#elif CPUSTYLE_STM32H7XX
			// На внутреннем RC генераторе
			#define	REFINFREQ 64000000uL
		#else /* WITHCPUXTAL */
			// На внутреннем RC генераторе
			#define	REFINFREQ 16000000uL
		#endif /* WITHCPUXTAL */

		#if defined(STM32F401xC)

			#define LSEFREQ 32768uL	// должно быть в файле конфигурации платы

			#define PLL_FREQ	(REFINFREQ / REF1_DIV * REF1_MUL)
			#define PLL2_FREQ	(REFINFREQ / REF2_DIV * REF2_MUL)	// STM32H7xxx
			#define PLL3_FREQ	(REFINFREQ / REF3_DIV * REF3_MUL)	// STM32H7xxx

			#define PLLI2S_FREQ (REFINFREQ / REF1_DIV * PLLI2SN_MUL)
			#define	PLLI2S_FREQ_OUT (PLLI2S_FREQ / 2)		// Frequency after PLLI2S_DivQ

			//#define PLLSAI_FREQ (REFINFREQ / REF1_DIV * SAIREF1_MUL)
			//#define PLLSAI_FREQ_OUT (PLLSAI_FREQ / 2)	// Frequency after PLLSAI_DivQ

			#define CPU_FREQ (stm32f4xx_get_sysclk_freq())	// 172032000uL

			/* частоты, подающиеся на периферию */
			//#define	PCLK1_FREQ (CPU_FREQ / 2)	// 42 MHz PCLK1 frequency - timer clocks is 85 MHz
			//#define	PCLK1_TIMERS_FREQ (CPU_FREQ / 1)	// 42 MHz PCLK1 frequency - timer clocks is 85 MHz
			//#define	PCLK2_FREQ (CPU_FREQ / 1)	// 84 MHz PCLK2 frequency
			//#define BOARD_SYSTICK_FREQ CPU_FREQ	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

 		#elif CPUSTYLE_STM32H7XX

			#define PLL_FREQ	(REFINFREQ / REF1_DIV * REF1_MUL)
			#define PLL2_FREQ	(REFINFREQ / REF2_DIV * REF2_MUL)	// STM32H7xxx
			#define PLL3_FREQ	(REFINFREQ / REF3_DIV * REF3_MUL)	// STM32H7xxx

			#define PLLI2S_FREQ (REFINFREQ / REF1_DIV * PLLI2SN_MUL)
			#define	PLLI2S_FREQ_OUT (PLLI2S_FREQ / 2)		// Frequency after PLLI2S_DivQ

			#define PLLSAI_FREQ (REFINFREQ / REF1_DIV * SAIREF1_MUL)
			#define PLLSAI_FREQ_OUT (PLLSAI_FREQ / 2)	// Frequency after PLLSAI_DivQ

			#define CPU_FREQ (PLL_FREQ / 2)	// 172032000uL

			#define HSIFREQ 64000000uL
			#define HSI48FREQ 48000000uL
			#if !defined  (CSI_VALUE)
			  #define CSI_VALUE    4000000U /*!< Value of the Internal oscillator in Hz*/
			#endif /* CSI_VALUE */

			#define LSEFREQ 32768uL	// должно быть в файле конфигурации платы

			#define BOARD_SPI_FREQ (hardware_get_spi_freq())

			#define WITHCPUNAME "STM32H7xx"

		#elif CPUSTYLE_STM32F7XX

			#define LSEFREQ 32768uL	// должно быть в файле конфигурации платы

			#define PLL_FREQ	(stm32f7xx_get_pll_freq())
			//#define PLL2_FREQ	(REFINFREQ / REF2_DIV * REF2_MUL)	// STM32H7xxx
			//#define PLL3_FREQ	(REFINFREQ / REF3_DIV * REF3_MUL)	// STM32H7xxx

			#define PLLI2S_FREQ (stm32f7xx_get_plli2s_freq())
			#define	PLLI2S_FREQ_OUT (PLLI2S_FREQ / 2)		// Frequency after PLLI2S_DivQ

			#define PLLSAI_FREQ (stm32f7xx_get_pllsai_freq())
			#define PLLSAI_FREQ_OUT (PLLSAI_FREQ / 2)	// Frequency after PLLSAI_DivQ

			#define CPU_FREQ (stm32f7xx_get_sys_freq())	// 172032000uL
			#define BOARD_SPI_FREQ (hardware_get_spi_freq())

			#define HSIFREQ 16000000uL

			#define WITHCPUNAME "STM32F7xx"

		#elif CPUSTYLE_STM32F4XX

			#define LSEFREQ 32768uL	// должно быть в файле конфигурации платы

			#define CPU_FREQ (stm32f4xx_get_sysclk_freq())	// 172032000uL
			#define BOARD_SPI_FREQ (stm32f4xx_get_spi1_freq())

			#define HSIFREQ 16000000uL	// 16 MHz

			#define WITHCPUNAME "STM32F4xx"

		#endif

	#elif CPUSTYLE_STM32F30X
		//
		//#define WITHCPUXTAL 8000000uL	// Если есть внешний кварц на процессоре.
		#if WITHCPUXTAL
			#define	REFINFREQ WITHCPUXTAL
			#define REF1_DIV 1
			#define REF1_MUL 9	// Up to 16 supported - вынести в конфигурационный файл платы
		#else
			#define	REFINFREQ 8000000uL
			#define REF1_DIV 2
			#define REF1_MUL 9	// Up to 16 supported - вынести в конфигурационный файл платы
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
		//#define BOARD_SYSTICK_FREQ CPU_FREQ	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

	#endif

	#define TICKS_FREQUENCY		200uL	// at ARM - 200 Hz

	// ADC clock frequency: 1..20 MHz
	#define SCL_CLOCK	400000uL	/* 400 kHz I2C/TWI speed */

	//#define SPISPEED (PCLK1_FREQ / 16)	/* 3.5 MHz на SCLK - требуемая скорость передачи по SPI */
	//#define SPISPEED (PCLK1_FREQ / 8)	/* 7 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEED (BOARD_SPI_FREQ / 4)	/* 14 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 12000000uL//(PCLK1_FREQ / 2)	/* 28 на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	//#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ADCVREF_CPU	33		// 3.3 volt
	#define DACVREF_CPU	33		// 3.3 volt
	#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	//#define HARDWARE_ADCINPUTS	40	/* до 32-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;	
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

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
	//#define BOARD_SYSTICK_FREQ CPU_FREQ	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

	// ADC clock frequency: 1..20 MHz
	#define ADC_FREQ	2000000uL	/* тактовая частота SAR преобразователя АЦП. */
	//#define ADC_FREQ	16000000uL	/* тактовая частота SAR преобразователя АЦП. */

	#define TICKS_FREQUENCY		(200uL * 1) // at ARM - 400 Hz

	//#define SCL_CLOCK  100000uL	/* 100 kHz I2C/TWI speed */
	#define SCL_CLOCK	400000uL	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (4UL * 1000 * 1000)	/* 4 MHz на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	//#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ADCVREF_CPU	33		// 3.3 volt
	//#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCBITS 10	/* АЦП работает с 10-битными значениями */
	//#define HARDWARE_ADCINPUTS	40	/* до 16-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#define WITHCPUNAME "ATSAM3S"

#elif CPUSTYLE_ATSAM4S

	//#define CPU_FREQ ((18432000uL * 73) / 14 / 2)	// satmod9if_v0 ARM board
	#define CPU_FREQ 64000000uL
	//#define CPU_FREQ 48000000uL
	//#define CPU_FREQ 32000000uL
	//#define CPU_FREQ 12000000uL
	//#define BOARD_SYSTICK_FREQ CPU_FREQ	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

	// ADC clock frequency: 1..20 MHz
	#define ADC_FREQ	2000000uL	/* тактовая частота SAR преобразователя АЦП. */
	//#define ADC_FREQ	16000000uL	/* тактовая частота SAR преобразователя АЦП. */

	#define TICKS_FREQUENCY		(200uL * 1) // at ARM - 400 Hz

	//#define SCL_CLOCK  100000uL	/* 100 kHz I2C/TWI speed */
	#define SCL_CLOCK	400000uL	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (4UL * 1000 * 1000)	/* 4 MHz на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	//#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ADCVREF_CPU	33		// 3.3 volt
	//#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCBITS 10	/* АЦП работает с 10-битными значениями */
	//#define HARDWARE_ADCINPUTS	40	/* до 16-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#define WITHCPUNAME "ATSAM4S"

#elif CPUSTYLE_AT91SAM9XE

	#define CPU_FREQ (196608000uL / 2)	// частота периферии (процессорная после деления).
	//#define CPU_FREQ 14400000uL
	//#define SCL_CLOCK  100000L	/* 100 kHz I2C/TWI speed */
	#define SCL_CLOCK  400000uL	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (4UL * 1000 * 1000)	/* 4 MHz на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	//#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define TICKS_FREQUENCY		200U // 200 Hz

	#define ARM_REALTIME_PRIORITY	AT91C_AIC_PRIOR_HIGHEST 
	#define ARM_SYSTEM_PRIORITY		AT91C_AIC_PRIOR_LOWEST 

	#define ADCVREF_CPU	33		// 3.3 volt
	//#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */
	//#define HARDWARE_ADCINPUTS	40	/* до 8-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

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
	//#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ADC_FREQ	250000uL	/* тактовая частота SAR преобразователя АЦП. */

	#define ADCVREF_CPU	33		// 3.3 volt
	//#define ADCVREF_CPU	50		// 5.0 volt

	//#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	//#define HARDWARE_ADCBITS 8	/* АЦП работает с 8-битными значениями */
	#define HARDWARE_ADCBITS 10	/* АЦП работает с 10-битными значениями */

	//#define HARDWARE_ADCINPUTS	40	/* до 8-ти входов АЦП */

	#if HARDWARE_ADCBITS == 8
		/* тип для хранения данных, считанных с АЦП */
		typedef uint_fast16_t adcvalholder_t;
		typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений
	#elif HARDWARE_ADCBITS == 10
		/* тип для хранения данных, считанных с АЦП */
		typedef uint_fast16_t adcvalholder_t;		
		typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений
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
	//#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define ADC_FREQ	125000uL	/* тактовая частота SAR преобразователя АЦП. */


	#define ADCVREF_CPU	25		// 2.5 volt
	#define HARDWARE_ADCBITS 8	/* АЦП работает с 8-битными значениями */

	//#define HARDWARE_ADCINPUTS	40	/* до 8-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast8_t adcvalholder_t;		
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

#elif CPUSTYLE_TMS320F2833X

	#define TICKS_FREQUENCY	 (200U * 2)	// 400 Hz - use compare/match interrupt
	#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

#elif CPUSTYLE_R7S721

	//#define WITHCPUXTAL 12000000uL			/* На процессоре установлен кварц 12.000 МГц */
	#define CPU_FREQ	(30 * WITHCPUXTAL)		/* 12 MHz * 30 - clock mode 0, xtal 12 MHz */

	#define BCLOCK_FREQ		(CPU_FREQ / 3)		// 120 MHz
	#define P1CLOCK_FREQ	(CPU_FREQ / 6)		// 60 MHz
	#define P0CLOCK_FREQ	(CPU_FREQ / 12)		// 30 MHz

	#define TICKS_FREQUENCY		(200uL * 1) // at ARM - 400 Hz

	// ADC clock frequency: 1..20 MHz
	#define ADC_FREQ	2000000u	/* тактовая частота SAR преобразователя АЦП. */
	#define SCL_CLOCK	400000u		/* 400 kHz I2C/TWI speed */

	#define SPISPEED 8000000u	/* 8 MHz (10.5) на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST (P1CLOCK_FREQ / 3)	// 20 MHz
	#define	SPISPEED400k	400000u				/* 400 kHz для низкоскоростных микросхем */
	//#define	SPISPEED100k	100000u			/* 100 kHz для низкоскоростных микросхем */

	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */

	//#define HARDWARE_ADCINPUTS	40	/* до 8-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

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
	//#define ARM_CA9_PRIORITYSHIFT 3	/* ICCPMR[7:3] is valid bit */

	//#define GICC_PMR		(INTC.ICCPMR)	// 4.4.2 Interrupt Priority Mask Register, GICC_PMR
	//#define GICC_RPR		((uint32_t) INTC.ICCRPR)	// 4.4.6 Running Priority Register, GICC_RPR
	//#define GICC_HPPIR		(INTC.ICCHPIR)
	//#define GICC_IAR		(INTC.ICCIAR)
	//#define GICC_BPR		(INTC.ICCBPR)
	//#define GICC_CTLR		(INTC.ICCICR)
	//#define GICD_IPRIORITYRn(n) (((volatile uint8_t *) & INTC.ICDIPR0) [(n)])

	// GIC_SetConfiguration parameters
	#define GIC_CONFIG_EDGE 0x03
	#define GIC_CONFIG_LEVEL 0x01

	#define WITHCPUNAME "R7S721"

#elif CPUSTYLE_STM32MP1

	//! GPIO Alternate Functions
	typedef enum {
		AF_SYSTEM,		//!< AF0 - SYSTEM*
		AF_TIM1,		//!< AF1 - TIM1/2
		AF_TIM2 = 1,	//!< AF1 - TIM1/2
		AF_TIM17 = 1,	//!< AF1 - TIM17
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
		AF_QUADSPI_AF9 = 9,	//!< AF9 - AF_QUADSPI
		AF_OTGFS,		//!< AF10 - OTGFS
		AF_QUADSPI_AF10 = 10,	//!< AF10 - AF_QUADSPI
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

	/* Частоты встроенных RC генераторов процессора */
	#define HSI64FREQ 64000000u
	#define CSI_VALUE 4000000U
	#define LSIFREQ 32000u

	//
	#if WITHCPUXOSC
		// с внешним генератором
		#define	REF1INFREQ WITHCPUXOSC
		#define	REF2INFREQ REF1INFREQ

	#elif WITHCPUXTAL
		// с внешним кварцевым резонатором
		#define	REF1INFREQ WITHCPUXTAL
		#define	REF2INFREQ REF1INFREQ

	#else /* WITHCPUXTAL */
		// На внутреннем RC генераторе
		#define	REF1INFREQ (stm32mp1_get_hsi_freq())
		#define	REF2INFREQ HSI64FREQ
	#endif /* WITHCPUXTAL */

	#define CPU_FREQ	(stm32mp1_get_mpuss_freq())
	#define DDR_FREQ	(stm32mp1_get_pll2_r_freq())
	#define AXISS_FREQ	(stm32mp1_get_axiss_freq())

	#define BOARD_SPI_FREQ (hardware_get_spi_freq())
	#define BOARD_QSPI_FREQ (stm32mp1_get_qspi_freq())

	#define TICKS_FREQUENCY	 (200U)	// 200 Hz

	// ADC clock frequency: 1..20 MHz
	#define SCL_CLOCK	400000u	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (BOARD_SPI_FREQ / 4)	/* 14 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 24000000u			/* 2требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000u			/* 400 kHz для низкоскоростных микросхем */
	//#define	SPISPEED100k	100000u		/* 100 kHz для низкоскоростных микросхем */

	#define ADCVREF_CPU	33		// 3.3 volt
	#define DACVREF_CPU	33		// 3.3 volt
	#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	//#define HARDWARE_ADCINPUTS	40	/* до 32-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений
	//#define HARDWARE_ADCBITS 8	/* АЦП работает с 8-битными значениями */
	//#define HARDWARE_ADCBITS 10	/* АЦП работает с 10-битными значениями */
	#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */
	//#define HARDWARE_ADCBITS 14	/* АЦП работает с 14-битными значениями */
	//#define HARDWARE_ADCBITS 16	/* АЦП работает с 16-битными значениями */
	#define ADC_FREQ	10000000uL	/* тактовая частота SAR преобразователя АЦП. */
	#define WITHREFSENSORVAL	1240	/* Reference voltage: STM32H743 1.180 1.216 1.255 */

	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */

	//#define HARDWARE_ADCINPUTS	40	/* до 8-ти входов АЦП */

	#define DACVREF_CPU	33		// 3.3 volt
	#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */

	#define HARDWARE_NCORES 2

#elif CPUSTYLE_XC7Z

	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#if WITHCPUXOSC
		// с внешним генератором
		#define	REFINFREQ WITHCPUXOSC
	#elif WITHCPUXTAL
		// с внешним кварцевым резонатором
		#define	REFINFREQ WITHCPUXTAL
	#endif /* WITHCPUXTAL */

	#define CPU_FREQ	(xc7z_get_arm_freq())
	#define BOARD_SPI_FREQ (xc7z_get_spi_freq())

	#define TICKS_FREQUENCY 200
	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED (12000000u)	/* 14 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 12000000u		//(PCLK1_FREQ / 2)	/* 28 на SCLK - требуемая скорость передачи по SPI */
	//#define	SPISPEED400k	400000u			/* 400 kHz для низкоскоростных микросхем */
	//#define	SPISPEED100k	100000u		/* 100 kHz для низкоскоростных микросхем */

	#define HARDWARE_NCORES 2

#elif CPUSTYLE_A64

	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#if WITHCPUXOSC
		// с внешним генератором
		#define	REFINFREQ WITHCPUXOSC
	#elif WITHCPUXTAL
		// с внешним кварцевым резонатором
		#define	REFINFREQ WITHCPUXTAL
	#endif /* WITHCPUXTAL */

	#define HARDWARE_CLK32K_FREQ 32000u
	#define HARDWARE_CLK16M_RC_FREQ 16000000u

	#define CPU_FREQ	(allwnr_a64_get_cpux_freq())
	#define BOARD_SPI_FREQ (allwnrt113_get_spi_freq())
	#define BOARD_USART_FREQ (allwnrt113_get_usart_freq())

	#define TICKS_FREQUENCY 200
	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED 		12000000u	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 	24000000u	/* 12 MHzна SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000u	/* 400 kHz для низкоскоростных микросхем */
	//#define	SPISPEED100k	100000u	/* 100 kHz для низкоскоростных микросхем */


	typedef enum {
		GPIO_CFG_IN  = 0x00,
		GPIO_CFG_OUT = 0x01,
		GPIO_CFG_AF2 = 0x02,
		GPIO_CFG_AF3 = 0x03,
		GPIO_CFG_AF4 = 0x04,
		GPIO_CFG_AF5 = 0x05,
		GPIO_CFG_AF6 = 0x06,
		GPIO_CFG_EINT = 0x06,	/* external interrupt sense (input) */
		GPIO_CFG_IODISABLE = 0x07,
	} GPIOMode_TypeDef;

	#define HARDWARE_NCORES 4

#elif CPUSTYLE_T113

	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#if WITHCPUXOSC
		// с внешним генератором
		#define	REFINFREQ WITHCPUXOSC
	#elif WITHCPUXTAL
		// с внешним кварцевым резонатором
		#define	REFINFREQ WITHCPUXTAL
	#endif /* WITHCPUXTAL */

	#define HARDWARE_CLK32K_FREQ 32000uL
	#define HARDWARE_CLK16M_RC_FREQ 16000000uL

	#define CPU_FREQ	(allwnrt113_get_arm_freq())
	#define BOARD_SPI_FREQ (allwnrt113_get_spi_freq())
	#define BOARD_USART_FREQ (allwnrt113_get_usart_freq())

	#define TICKS_FREQUENCY 200
	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED 		12000000u	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 	24000000u	/* 12 MHzна SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000u	/* 400 kHz для низкоскоростных микросхем */
	//#define	SPISPEED100k	100000u	/* 100 kHz для низкоскоростных микросхем */


	typedef enum {
		GPIO_CFG_IN  = 0x00,
		GPIO_CFG_OUT = 0x01,
		GPIO_CFG_AF2 = 0x02,
		GPIO_CFG_AF3 = 0x03,
		GPIO_CFG_AF4 = 0x04,
		GPIO_CFG_AF5 = 0x05,
		GPIO_CFG_AF6 = 0x06,
		GPIO_CFG_AF7 = 0x07,
		GPIO_CFG_AF8 = 0x08,
		GPIO_CFG_AF9 = 0x09,
		GPIO_CFG_AF10 = 0x0A,
		GPIO_CFG_AF11 = 0x0B,
		GPIO_CFG_AF12 = 0x0C,
		GPIO_CFG_AF13 = 0x0D,
		GPIO_CFG_EINT = 0x0E,	/* external interrupt sense (input) */
		GPIO_CFG_IODISABLE = 0x0F,
	} GPIOMode_TypeDef;

	#define HARDWARE_NCORES 2

#elif CPUSTYLE_F133

	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#if WITHCPUXOSC
		// с внешним генератором
		#define	REFINFREQ WITHCPUXOSC
	#elif WITHCPUXTAL
		// с внешним кварцевым резонатором
		#define	REFINFREQ WITHCPUXTAL
	#endif /* WITHCPUXTAL */

	#define HARDWARE_CLK32K_FREQ 32000uL
	#define HARDWARE_CLK16M_RC_FREQ 16000000uL

	#define CPU_FREQ	(allwnrf133_get_riscv_freq())
	#define BOARD_SPI_FREQ (allwnrt113_get_spi_freq())
	#define BOARD_USART_FREQ (allwnrt113_get_usart_freq())

	#define TICKS_FREQUENCY 200
	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED 		12000000uL	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 	24000000u	/* 12 MHzна SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	//#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */


	typedef enum {
		GPIO_CFG_IN  = 0x00,
		GPIO_CFG_OUT = 0x01,
		GPIO_CFG_AF2 = 0x02,
		GPIO_CFG_AF3 = 0x03,
		GPIO_CFG_AF4 = 0x04,
		GPIO_CFG_AF5 = 0x05,
		GPIO_CFG_AF6 = 0x06,
		GPIO_CFG_AF7 = 0x07,
		GPIO_CFG_AF8 = 0x08,
		GPIO_CFG_AF9 = 0x09,
		GPIO_CFG_AF10 = 0x0A,
		GPIO_CFG_AF11 = 0x0B,
		GPIO_CFG_AF12 = 0x0C,
		GPIO_CFG_AF13 = 0x0D,
		GPIO_CFG_EINT = 0x0E,	/* external interrupt sense (input) */
		GPIO_CFG_IODISABLE = 0x0F,
	} GPIOMode_TypeDef;

#elif CPUSTYLE_XCZU
	// Zynq UltraScale+ Device
	// XCZU2..XCZU9, XCZU11

	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#if WITHCPUXOSC
		// с внешним генератором
		#define	REFINFREQ WITHCPUXOSC
	#elif WITHCPUXTAL
		// с внешним кварцевым резонатором
		#define	REFINFREQ WITHCPUXTAL
	#endif /* WITHCPUXTAL */

	#define CPU_FREQ	1000000000u //(xc7z_get_arm_freq())
	#define BOARD_SPI_FREQ (xc7z_get_spi_freq())

	#define TICKS_FREQUENCY 200
	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED (12000000uL)	/* 14 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 12000000uL//(PCLK1_FREQ / 2)	/* 28 на SCLK - требуемая скорость передачи по SPI */
	#define	SPISPEED400k	400000uL	/* 400 kHz для низкоскоростных микросхем */
	//#define	SPISPEED100k	100000uL	/* 100 kHz для низкоскоростных микросхем */

	#define HARDWARE_NCORES 4

#elif defined(_WIN32)

	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */

	//#define HARDWARE_ADCINPUTS	40	/* до 8-ти входов АЦП */

	#define DACVREF_CPU	33		// 3.3 volt
	#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define TICKS_FREQUENCY 200


#else

	#error Undefined CPUSTYLE_XXX

#endif

#if defined (__NVIC_PRIO_BITS)

	#if WITHNESTEDINTERRUPTS

		// The processor does not process any exception with a priority value greater than or equal to BASEPRI.
		extern uint32_t gARM_OVERREALTIME_PRIORITY;
		extern uint32_t gARM_REALTIME_PRIORITY;
		extern uint32_t gARM_SYSTEM_PRIORITY;
		extern uint32_t gARM_BASEPRI_ONLY_REALTIME;
		extern uint32_t gARM_BASEPRI_ONLY_OVERREALTIME;
		extern uint32_t gARM_BASEPRI_ALL_ENABLED;
		// See usage of functions NVIC_PriorityGroupConfig and NVIC_SetPriorityGrouping
		// A lower priority value indicating a higher priority of running handler
		#define ARM_OVERREALTIME_PRIORITY	((const uint32_t) gARM_OVERREALTIME_PRIORITY)
		#define ARM_REALTIME_PRIORITY	((const uint32_t) gARM_REALTIME_PRIORITY)
		#define ARM_SYSTEM_PRIORITY	((const uint32_t) gARM_SYSTEM_PRIORITY)

		#define system_enableIRQ() do { __set_BASEPRI(gARM_BASEPRI_ALL_ENABLED); } while (0)	// разрешены все
		#define system_disableIRQ() do { __set_BASEPRI(gARM_BASEPRI_ONLY_REALTIME); } while (0) // разрешены только realtime
		
	#else /* WITHNESTEDINTERRUPTS */

		#define ARM_OVERREALTIME_PRIORITY	(0)
		#define ARM_REALTIME_PRIORITY	(0)
		#define ARM_SYSTEM_PRIORITY	(0)

		#define system_enableIRQ() do { __enable_irq(); } while (0)
		#define system_disableIRQ() do { __disable_irq(); } while (0)

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

	#define system_enableIRQ() do { sei(); } while (0)
	#define system_disableIRQ() do { cli(); } while (0)

	#define global_enableIRQ() do { sei(); } while (0)
	#define global_disableIRQ() do { cli(); } while (0)

#elif (__GIC_PRESENT == 1)

	#define ICPIDR0	(* (const volatile uint32_t *) (GIC_DISTRIBUTOR_BASE + 0xFE0))
	#define ICPIDR1	(* (const volatile uint32_t *) (GIC_DISTRIBUTOR_BASE + 0xFE4))
	#define ICPIDR2	(* (const volatile uint32_t *) (GIC_DISTRIBUTOR_BASE + 0xFE8))

	#if WITHNESTEDINTERRUPTS

		// The processor does not process any exception with a priority value greater than or equal to BASEPRI.
		extern uint32_t gARM_OVERREALTIME_PRIORITY;
		extern uint32_t gARM_REALTIME_PRIORITY;
		extern uint32_t gARM_SYSTEM_PRIORITY;
		extern uint32_t gARM_BASEPRI_ONLY_REALTIME;
		extern uint32_t gARM_BASEPRI_ONLY_OVERREALTIME;
		extern uint32_t gARM_BASEPRI_ALL_ENABLED;

		#define ARM_CA9_ENCODE_PRIORITY(v) ((v) << (GIC_GetBinaryPoint() + 1))
		/*
			GICC_PMR == INTC.ICCPMR

			Provides an interrupt priority filter. 
			Only interrupts with higher priority than the value in this
			register are signaled to the processor.

		*/
		enum
		{
			PRI_IPC,	/* Приоритет SGI прерывания для синхронизации приоритетов GIC на остальных процессорах */
			PRI_IPC_ONLY,
			PRI_OVRT,
			PRI_RT,
			PRI_SYS,
			PRI_USER,

//			gARM_OVERREALTIME_PRIORITY = ARM_CA9_ENCODE_PRIORITY(PRI_OVRT),	// value for GIC_SetPriority
//			gARM_REALTIME_PRIORITY = ARM_CA9_ENCODE_PRIORITY(PRI_RT),	// value for GIC_SetPriority
//			gARM_SYSTEM_PRIORITY = ARM_CA9_ENCODE_PRIORITY(PRI_SYS),		// value for GIC_SetPriority
//
//			gARM_BASEPRI_ONLY_REALTIME = ARM_CA9_ENCODE_PRIORITY(PRI_SYS),	// value for GIC_SetInterfacePriorityMask
//			gARM_BASEPRI_ALL_ENABLED = ARM_CA9_ENCODE_PRIORITY(PRI_USER)	// value for GIC_SetInterfacePriorityMask
			PRI_count
		};

		#define BOARD_SGI_IRQ SGI1_IRQn		/* Прерываниедля синхронизации приоритетов GIC на остальных процессорах  */
		#define BOARD_SGI_PRIO	ARM_CA9_ENCODE_PRIORITY(PRI_IPC)

		#define RUNNING_PRI	((GICInterface->RPR & 0xFF) >> ARM_CA9_PRIORITYSHIFT) // The current running priority on the CPU interface.

		// A lower priority value indicating a higher priority
		#define ARM_OVERREALTIME_PRIORITY	((const uint32_t) gARM_OVERREALTIME_PRIORITY)
		#define ARM_REALTIME_PRIORITY	((const uint32_t) gARM_REALTIME_PRIORITY)
		#define ARM_SYSTEM_PRIORITY	((const uint32_t) gARM_SYSTEM_PRIORITY)

		#if defined(__aarch64__)

			// разрешены все
			#define system_enableIRQ() do { \
				} while (0)
			// разрешены только realtime
			#define system_disableIRQ() do { \
				} while (0)

			#define global_enableIRQ() do { \
				} while (0)
			#define global_disableIRQ() do { \
			} while (0)

		#else /* defined(__aarch64__) */

			// разрешены все
			#define system_enableIRQ() do { \
					/*ASSERT(RUNNING_PRI == 0x1F); */\
					GIC_SetInterfacePriorityMask(gARM_BASEPRI_ALL_ENABLED); \
				} while (0)
			// разрешены только realtime
			#define system_disableIRQ() do { \
					/*ASSERT(RUNNING_PRI == 0x1F); */\
					GIC_SetInterfacePriorityMask(gARM_BASEPRI_ONLY_REALTIME); \
				} while (0)

			#define global_enableIRQ() do { \
				__enable_irq(); \
				} while (0)
			#define global_disableIRQ() do { \
				__disable_irq(); \
			} while (0)
		#endif /* defined(__aarch64__) */

	#else /* WITHNESTEDINTERRUPTS */

		#define ARM_OVERREALTIME_PRIORITY	0
		#define ARM_REALTIME_PRIORITY	0
		#define ARM_SYSTEM_PRIORITY		0

		#define system_enableIRQ() do { \
			__enable_irq(); \
			} while (0)
		#define system_disableIRQ() do { \
			__disable_irq(); \
		} while (0)

		#define global_enableIRQ() do { \
			__enable_irq(); \
			} while (0)
		#define global_disableIRQ() do { \
			__disable_irq(); \
		} while (0)

	#endif /* WITHNESTEDINTERRUPTS */

#elif CPUSTYLE_RISCV

	#if WITHNESTEDINTERRUPTS

		#define ARM_OVERREALTIME_PRIORITY	3	/* валкодер */
		#define ARM_REALTIME_PRIORITY		2	/* звук */
		#define ARM_SYSTEM_PRIORITY			1	/* таймеры, USB */
		#define ARM_USER_PRIORITY			0	/*  Значение, на которое инициализируется PLIC->PLIC_MTH_REG */

		#define system_enableIRQ() do { PLIC->PLIC_MTH_REG = ARM_USER_PRIORITY; } while (0)
		#define system_disableIRQ() do { PLIC->PLIC_MTH_REG = ARM_SYSTEM_PRIORITY; } while (0)

		#define global_enableIRQ() do { csr_set_bits_mie(MIE_MEI_BIT_MASK); } while (0)
		#define global_disableIRQ() do { csr_clr_bits_mie(MIE_MEI_BIT_MASK); } while (0)

	#else /* WITHNESTEDINTERRUPTS */

		#define ARM_OVERREALTIME_PRIORITY	1
		#define ARM_REALTIME_PRIORITY		1
		#define ARM_SYSTEM_PRIORITY			1
		#define ARM_USER_PRIORITY			0	/*  Значение, на которое инициализируется PLIC->PLIC_MTH_REG */

		#define system_enableIRQ() do { csr_set_bits_mie(MIE_MEI_BIT_MASK); } while (0)
		#define system_disableIRQ() do { csr_set_bits_mie(MIE_MEI_BIT_MASK); } while (0)

		#define global_enableIRQ() do { csr_set_bits_mie(MIE_MEI_BIT_MASK); } while (0)
		#define global_disableIRQ() do { csr_clr_bits_mie(MIE_MEI_BIT_MASK); } while (0)

	#endif /* WITHNESTEDINTERRUPTS */

#elif CPUSTYLE_CA53
		// aarch64 mode - no GIC

		#define ARM_OVERREALTIME_PRIORITY	1
		#define ARM_REALTIME_PRIORITY		1
		#define ARM_SYSTEM_PRIORITY			1
		#define ARM_USER_PRIORITY			0

		void (system_disableIRQ)(void);
		void (system_enableIRQ)(void);

		#define global_enableIRQ() do { (system_enableIRQ)(); } while (0)
		#define global_disableIRQ() do { (system_disableIRQ)(); } while (0)

#else /* CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 */

	// For CPUSTYLE_ARM7TDMI
	//#define ARM_REALTIME_PRIORITY	(0)
	//#define ARM_SYSTEM_PRIORITY	(0)

	void (system_disableIRQ)(void);
	void (system_enableIRQ)(void);

	#define global_enableIRQ() do { (system_enableIRQ)(); } while (0)
	#define global_disableIRQ() do { (system_disableIRQ)(); } while (0)

#endif /* CPUSTYLE_ARM_CM3 */

/* STM32: In HS mode and when the DMA is used, all variables and data structures dealing
   with the DMA during the transaction process should be 4-bytes aligned */    

#if defined   (__GNUC__)        /* GNU Compiler */
	#pragma GCC diagnostic ignored "-Wunused-function"
	#pragma GCC diagnostic ignored "-Wunused-variable"
	#pragma GCC diagnostic ignored "-Wunused-const-variable"
	#pragma GCC diagnostic error "-Wwrite-strings"

	#define __ALIGN4_END    __attribute__ ((aligned (4)))
	#define __ALIGN4_BEGIN         

	#define ATTRPACKED __attribute__ ((packed))
	#define ATTRNORETURN __attribute__ ((__noreturn__))
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

	#define ATTRPACKED __attribute__ ((packed))
	#define ATTRNORETURN __attribute__ ((__noreturn__))
	#define KEYWORDPACKED __packed

#endif /* __GNUC__ */



/* типы применяемых микросхем */

#define DDS_TYPE_AD9852		1	// AD9854 also supported
#define DDS_TYPE_AD9857		2
#define DDS_TYPE_AD9951		3	// AD9852, AD9853 and AD9854 also supported
#define DDS_TYPE_AD9834		4	// AD9834, AD9833 and AD9838 supported
#define DDS_TYPE_AD9851		5	// AD9850 and AD9851 supported
#define DDS_TYPE_AD9835		6	// AD9832 and AD9835 supported
#define DDS_TYPE_FPGAV1		7	// NCO, DDC/DUC
#define DDS_TYPE_ATTINY2313	8	// experemental: nco=/7, dds=/9
#define DDS_TYPE_ZYNQ_PL 	9
#define DDS_TYPE_GW2A_V0 	10

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

#define CODEC_TYPE_FPGAV1		40	// квадратуры получаем от FPGA
#define CODEC_TYPE_TLV320AIC23B	41	// TI TLV320AIC23B
#define CODEC_TYPE_CS4272		42	// CS CS4272
#define CODEC_TYPE_NAU8822L		43	// NUVOTON NAU8822L
#define CODEC_TYPE_WM8994		44	// CIRRUS LOGIC WM8994ECS/R
#define CODEC_TYPE_CS42L51		45	// CIRRUS LOGIC CS42L51

#define RTC_TYPE_DS1305		50	/* MAXIM DS1305EN RTC clock chip with SPI interface */
#define RTC_TYPE_DS1307		51	/* MAXIM DS1307/DS3231 RTC clock chip with I2C interface */
#define RTC_TYPE_M41T81		52	/* ST M41T81M6/M41T81SM6F RTC clock chip with I2C interface */
#define RTC_TYPE_STM32F4xx	53	/* STM32F4xx internal RTC peripherial */
#define RTC_TYPE_STM32F0xx	54	/* STM32F0xx internal RTC peripherial */
#define RTC_TYPE_STM32L0xx	55	/* STM32L0xx internal RTC peripherial */
#define RTC_TYPE_LINUX		56	/* Linux local time */

#define TSC_TYPE_TSC2046	60	// Resistive touch screen controller TI TSC2046
#define TSC_TYPE_STMPE811	61	// Resistive touch screen controller ST STMPE811
#define TSC_TYPE_GT911		62	// Capacitive touch screen controller Goodix GT911
#define TSC_TYPE_S3402		63	// Capacitive touch screen controller S3402 (on panel H497TLB01.4)
#define TSC_TYPE_FT5336 	64	// Capacitive touch screen controller FocalTech FT5336
#define TSC_TYPE_XPT2046 	65	// Resistive touch screen controller SHENZHEN XPTEK TECHNOLOGY CO., LTD http://www.xptek.com.cn
#define TSC_TYPE_ILI2102	66	// Capacitive touch screen controller Ilitek ILI2102
#define TSC_TYPE_AWTPADC	67	// Allwinner F133/t113-s3 resistive touch screen controller

// Start of NVRAM definitions section
// NOTE: DO NOT USE any types of FLASH memory chips, only EEPROM or FRAM chips are supported.
// поддерживаются только FRAM или EEPROM. FLASH не поддерживаются.
#define NVRAM_TYPE_FM25XXXX		70	/* SERIAL FRAM AUTODETECT	*/
#define NVRAM_TYPE_FM25L04		71	/* SERIAL FRAM 4KBit	*/
#define NVRAM_TYPE_FM25L16		72	/* SERIAL FRAM 16Kbit	*/
#define NVRAM_TYPE_FM25L64		73	/* SERIAL FRAM 64Kbit	*/
#define NVRAM_TYPE_FM25L256		74	/* SERIAL FRAM 256KBit	*/

#define NVRAM_TYPE_AT25040A		75	/* SERIAL EEPROM 4KBit	*/
#define NVRAM_TYPE_AT25L16		76	/* SERIAL EEPROM 16Kbit	*/
#define NVRAM_TYPE_AT25256A		77	/* SERIAL EEPROM 256KBit	*/

#define NVRAM_TYPE_CPUEEPROM		78	/* EEPROM память процессора */
#define	NVRAM_TYPE_BKPSRAM			79	/* Backup SRAM */
#define	NVRAM_TYPE_NOTHING			80	/* вообще отсутствует */

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

	#elif defined (NVRAM_TYPE) && (NVRAM_TYPE == NVRAM_TYPE_BKPSRAM) && CPUSTYLE_STM32MP1

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
#endif /* LCDMODE_S1D13781 */

#if LCDMODE_AT070TN90	/* AT070TN90 panel (800*480) - 7" display */
	#define LCDMODE_SPI_RN 1 // эти дисплеи требуют только RESET
	#define DIM_X 800
	#define DIM_Y 480
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
#endif /* LCDMODE_AT070TN90 */

#if 1 && LCDMODE_AT070TNA2	/* AT070TNA2 panel (1024*600) - 7" display */
	#define LCDMODE_SPI_RN 1 // эти дисплеи требуют только RESET
	#define DIM_X 1024
	#define DIM_Y 600
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
#endif /* LCDMODE_AT070TNA2 */

#if LCDMODE_TCG104XGLPAPNN	/* TCG104XGLPAPNN-AN30 panel (1024*768) - 10.4" display - DE mode required */
	#define LCDMODE_SPI_RN 1 // эти дисплеи требуют только RESET
	#define DIM_X 1024
	#define DIM_Y 768
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
#endif /* LCDMODE_TCG104XGLPAPNN */

#if 0 && LCDMODE_AT070TNA2	/* xxxx panel (1280*720) - 10" display */
	#define LCDMODE_SPI_RN 1 // эти дисплеи требуют только RESET
	#define DIM_X 1280
	#define DIM_Y 720
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
#endif /* LCDMODE_AT070TNA2 */

#if LCDMODE_H497TLB01P4	/* 720xRGBx1280 - 5" AMOELD Panel H497TLB01.4 */
	//#define LCDMODE_SPI_RN 1 // эти дисплеи требуют только RESET
	#define DIM_X 720
	#define DIM_Y 1280
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
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
	#define LCDMODE_PIXELSIZE 2
#endif /* LCDMODE_ST7735 */

#if LCDMODE_ILI9341
	#define LCDMODE_HARD_SPI	1	/* SPI interface */
	#define LCDMODE_SPI_RA	1	/* SPI only, with Reset, with Address */
	#define DIM_X 320
	#define DIM_Y 240
	#define LCDMODE_COLORED	1
	#define LCDMODE_RGB565 1
	#define LCDMODE_PIXELSIZE 2
#endif

#if LCDMODE_ILI8961
	#define LCDMODE_HARD_SPI	1	/* SPI interface */
	#define LCDMODE_SPI	1	/* SPI only, no Reset, no Address need */
	#define DIM_X 320
	#define DIM_Y 240
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
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
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 800 && DIM_Y == 480
	#define DSTYLE_G_X800_Y480	1	/* AT070TN90 panel (800*480) - 7" display */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 1024 && DIM_Y == 600
	#define DSTYLE_G_X800_Y480	1	/* AT070TN90 panel (800*480) - 7" display */
	//#define DSTYLE_G_X1024_Y600	1	/* AT070TNA2 panel (1024*600) - 7" display */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 1024 && DIM_Y == 768
	#define DSTYLE_G_X800_Y480	1	/* AT070TN90 panel (800*480) - 7" display */
	//#define DSTYLE_G_X1024_Y768	1	/* TCG104XGLPAPNN-AN30 panel (1024*768) - 10.4" display - DE mode required */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 1280 && DIM_Y == 720
	#define DSTYLE_G_X800_Y480	1	/* AT070TN90 panel (800*480) - 7" display */
	//#define DSTYLE_G_X1024_Y600	1	/* AT070TNA2 panel (1024*600) - 7" display */
	//#define DSTYLE_G_X1280_Y720	1	/* xxxxx panel (1280*720) - 7" display */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 720 && DIM_Y == 1280
	#define DSTYLE_G_X480_Y272	1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 220 && DIM_Y == 176
	#define DSTYLE_G_X220_Y176	1	// Индикатор 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 с контроллером ILI9225С
	#define CHAR_W	8
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */
		
#elif DIM_X == 240 && DIM_Y == 128
	#define DSTYLE_G_X240_Y128	1
	#define CHAR_W	8
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 2)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 160 && DIM_Y == 128
	#define DSTYLE_G_X160_Y128	1	// ST7735
	#define CHAR_W	8
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 128 && DIM_Y == 64
	#define DSTYLE_G_X128_Y64	1	
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 132 && DIM_Y == 64
	#define DSTYLE_G_X132_Y64	1
	//#define DSTYLE_G_X128_Y64	1	/* RDX0077 - проверки дизайна на других индикаторах	*/
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
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
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 176 && DIM_Y == 132		
	#define DSTYLE_G_X176_Y132	1		// ls020/lph88
	#define CHAR_W	8
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 320 && DIM_Y == 240
	#define DSTYLE_G_X320_Y240	1	
	#define CHAR_W	10
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 64 && DIM_Y == 32
	#define DSTYLE_G_X64_Y32	1	
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 128 && DIM_Y == 32
	#define DSTYLE_G_X64_Y32	1	/* G1203H - проверки	*/
	//#define DSTYLE_G_X128_Y32	1	
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
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

	#define DSTYLE_G_DUMMY	1	// пустой список отображения

	#define DIM_X 480
	#define DIM_Y 272
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1

	#define HALFCOUNT_FREQA 1
	#define HALFCOUNT_SMALL 1
	#define HALFCOUNT_BIG 1

	#define CHAR_W 6
	#define CHAR_H 8

	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

	#define DISPLAYMODES_FPS 5	/* количество обновлений отображения режимов работы */
	#define DISPLAY_FPS	10	/* обновление показаний частоты за секунду */
	#define DISPLAYSWR_FPS 5	/* количество обновлений SWR за секунду */

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
	#define BOARDPOWERMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define BOARDPOWERMAX	4	// Верхний предел регулировки (показываемый на дисплее)
	#define WITHMUTEALL			1	// Отключение микрофона во всех режимах
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
	#define BOARDPOWERMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define BOARDPOWERMAX	4	// Верхний предел регулировки (показываемый на дисплее)
	#define WITHMUTEALL			1	// Отключение микрофона во всех режимах
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

	#define WITHLCDBACKLIGHTMIN 0
	#define WITHLCDBACKLIGHTMAX 1

#elif CTLREGSTYLE_SW2012CN
	// управляющие регистры SW2012MINI COLOR 2 c цветным дисплеем ILI9163 и регулировкой выходной мощности
	// управляющие регистры SW2016MINI

	#define WITHPOWERTRIM		1	// Имеется управление мощностью
	#define BOARDPOWERMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define BOARDPOWERMAX	4	// Верхний предел регулировки (показываемый на дисплее)
	#define WITHMUTEALL			1	// Отключение микрофона во всех режимах
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

#define WITHLCDBACKLIGHTMIN 0
#define WITHLCDBACKLIGHTMAX 1

#elif CTLREGSTYLE_SW2012CN_RN3ZOB

	#define WITHPOWERTRIM		1	// Имеется управление мощностью
	#define BOARDPOWERMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define BOARDPOWERMAX	4	// Верхний предел регулировки (показываемый на дисплее)
	#define WITHMUTEALL			1	// Отключение микрофона во всех режимах
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

#define WITHLCDBACKLIGHTMIN 0
#define WITHLCDBACKLIGHTMAX 1

#elif CTLREGSTYLE_SW2013SF

	#define WITHPOWERLPHP		1	// Имеется переключение и отображение HP / LP
	#define BOARDPOWERMIN	0	// Нижний предел регулировки
	#define BOARDPOWERMAX	1	// Верхний предел регулировки
	#define WITHMUTEALL			1	// Отключение микрофона во всех режимах
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
	#define BOARDPOWERMIN	0	// Нижний предел регулировки
	#define BOARDPOWERMAX	1	// Верхний предел регулировки
	#define WITHMUTEALL			1	// Отключение микрофона во всех режимах
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
	#define BOARDPOWERMIN	0	// Нижний предел регулировки
	#define BOARDPOWERMAX	1	// Верхний предел регулировки
	#define WITHMUTEALL			1	// Отключение микрофона во всех режимах
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
	//#define WITHNOTCHONOFF		1	/* notch on/off */

#elif CTLREGSTYLE_SW2013RDX_UY5UM_WO240

	#define WITHLCDBACKLIGHT	1	// Имеется управление подсветкой дисплея
	#define WITHLCDBACKLIGHTMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHLCDBACKLIGHTMAX	4	// Верхний предел регулировки (показываемый на дисплее)
	#define WITHPOWERLPHP		1	// Имеется переключение и отображение HP / LP
	#define BOARDPOWERMIN	0	// Нижний предел регулировки
	#define BOARDPOWERMAX	1	// Верхний предел регулировки
	#define WITHMUTEALL			1	// Отключение микрофона во всех режимах
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
	#define BOARDPOWERMIN	0	// Нижний предел регулировки
	#define BOARDPOWERMAX	1	// Верхний предел регулировки
	#define WITHMUTEALL			1	// Отключение микрофона во всех режимах
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
	#define BOARDPOWERMIN	0	// Нижний предел регулировки
	#define BOARDPOWERMAX	1	// Верхний предел регулировки
	#define WITHMUTEALL			1	// Отключение микрофона во всех режимах
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
	#define BOARDPOWERMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define BOARDPOWERMAX	16	// Верхний предел регулировки (показываемый на дисплее)

	#define WITHLCDBACKLIGHT	1	// Имеется управление подсветкой дисплея
	#define WITHLCDBACKLIGHTMIN	1	// Нижний предел регулировки (показываемый на дисплее)
	#define WITHLCDBACKLIGHTMAX	4	// Верхний предел регулировки (показываемый на дисплее)
	#define WITHMUTEALL			1	// Отключение микрофона во всех режимах
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
	#define BOARDPOWERMIN	0	// Нижний предел регулировки
	#define BOARDPOWERMAX	1	// Верхний предел регулировки
	#define WITHMUTEALL			1	// Отключение микрофона во всех режимах
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
	#define WITHMUTEALL			1	// Отключение микрофона во всех режимах
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
	#define WITHMUTEALL			1	// Отключение микрофона во всех режимах
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
		//#define WITHMIC1LEVEL		1	// установка усиления микрофона

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

#elif CTLREGMODE_NOCTLREG			// Отсутствующий регистр управления

	/* коды входов коммутатора источников сигнала для УНЧ приёмника */
	#define BOARD_DETECTOR_MUTE 0
	#define BOARD_DETECTOR_SSB 	0
	#define BOARD_DETECTOR_AM 	0
	#define BOARD_DETECTOR_FM 	0
	#define BOARD_DETECTOR_TUNE 	0		// Заглушка конфигурация платы для режима TUNE (CWZ на передачу)
	/* коды фильтров второй ПЧ, выдаваемые на дешифраторы */
	#define	BOARD_FILTERCODE_0	0
	#define	BOARD_FILTERCODE_1	0
	#define	BOARD_FILTERCODE_2	0
	#define	BOARD_FILTERCODE_3	0

#else
	//#error Undefined CTLREGMODE_xxx option
#endif
#define WITHHARDINTERLOCK (WITHNESTEDINTERRUPTS && (CPUSTYLE_ARM_CM7 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM0 || (__CORTEX_A != 0)))

#ifndef WITHFLATMENU
	#define WITHFLATMENU (CTLSTYLE_SW2011ALL && ! CPUSTYLE_ATMEGA_XXX4)
#endif /* WITHFLATMENU */

#define HARDWARE_DELAY_MS(t) do { local_delay_ms(t); } while (0)	//HAL_Delay(t)
#define HARDWARE_DELAY_US(t) do { local_delay_us(t); } while (0)
#define HARDWARE_GETTICK_MS() ((uint32_t) 0) // HAL_GetTick()

#define WITHSPECTRUMWF (WITHIF4DSP && (WITHRTS96 || WITHRTS192) && ! LCDMODE_HD44780)

/* Сброс мощности при запросе TUNE от автотюнера или извне */
#define WITHLOWPOWEREXTTUNE	(defined (HARDWARE_GET_TUNE) || WITHAUTOTUNER)

/* Зависимости для поределения конфигурации видеосистемы */

//#define LCDMODE_V0	1	/* Обычная конфигурация без PIP с L8 на основном экране */
//#define LCDMODE_V1	1	/* Обычная конфигурация с PIP на часть экрана */
//#define LCDMODE_V1A	1	/* Обычная конфигурация с PIP на часть экрана (но главный экран 16 бит) */
//#define LCDMODE_V2	1	/* только главный экран, без PIP */
//#define LCDMODE_V2A	1	/* только главный экран, без PIP (но главный экран 16 бит) */

#if LCDMODE_V0
	/* Обычная конфигурация одна страница без PIP с L8 на основном экране */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	#define LCDMODE_MAIN_L8	1
	//#define LCDMODE_MAIN_RGB565	1
	#define LCDMODE_MAIN_PAGES	1
	#define LCDMODE_PIXELSIZE 1

	//#define LCDMODE_PIP_L8	1
	//#define LCDMODE_PIP_RGB565	1
	//#define LCDMODE_PIP_PAGES	3

	// 0..COLORPIP_BASE-1 - волопад
	// COLORPIP_BASE..127 - надписи и элементы дизайна
	// то же с кодом больше на 128 - затененные цвета для получения полупрозрачности
	// 0..95 - палитра водопада
	// 96..111 - норм цвета
	// 112..127 - первая степень AA
	// Заполнение палитры производится в display2_xltrgb24()

	#define COLORPIP_SHADED 128
	#define COLORPIP_ALIASED 16
	#define COLORPIP_BASE 96	// should be match to PALETTESIZE

	#define LCDMODE_MAIN_L8		1	/* используется 8 бит на пиксель представление экрана. Иначе - 16 бит - RGB565. */
	//#define LCDMODE_PIP_RGB565	1	/* используется PIP с форматом 16 бит - RGB565 */
	//#define LCDMODE_PIP_L8	1	/* используется PIP с форматом 8 бит - индексные цвета */

	#define WITHFASTWATERFLOW 1

#elif LCDMODE_V1
	#error Use LCDMODE_V2 instedd of LCDMODE_V1

	/* Обычная конфигурация L8 с PIP16 на часть экрана */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	#define LCDMODE_MAIN_L8	1
	//#define LCDMODE_MAIN_RGB565	1
	#define LCDMODE_MAIN_PAGES	1
	#define LCDMODE_PIXELSIZE 1

	//#define LCDMODE_PIP_L8	1
	#define LCDMODE_PIP_RGB565	1
	#define LCDMODE_PIP_PAGES	3
	//#define COLORPIP_SHADED 128

	#define LCDMODE_MAIN_L8		1	/* используется 8 бит на пиксель представление экрана. Иначе - 16 бит - RGB565. */
	#define LCDMODE_PIP_RGB565	1	/* используется PIP с форматом 16 бит - RGB565 */
	//#define LCDMODE_PIP_L8	1	/* используется PIP с форматом 8 бит - индексные цвета */

	#define WITHFASTWATERFLOW 1

#elif LCDMODE_V1A
	#error Use LCDMODE_V2 instedd of LCDMODE_V1A

	/* Обычная конфигурация RGB565 с PIP16 на часть экрана (но главный экран 16 бит) */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	//#define LCDMODE_MAIN_L8	1
	#define LCDMODE_MAIN_RGB565	1
	#define LCDMODE_MAIN_PAGES	1
	#define LCDMODE_PIXELSIZE 2

	//#define LCDMODE_PIP_L8	1
	#define LCDMODE_PIP_RGB565	1
	#define LCDMODE_PIP_PAGES	3
	//#define COLORPIP_SHADED 128
	//#define LCDMODE_MAIN_L8		1	/* используется 8 бит на пиксель представление экрана. Иначе - 16 бит - RGB565. */
	#define LCDMODE_PIP_RGB565	1	/* используется PIP с форматом 16 бит - RGB565 */
	//#define LCDMODE_PIP_L8	1	/* используется PIP с форматом 8 бит - индексные цвета */

	#define WITHFASTWATERFLOW 1

#elif LCDMODE_V2
	/* только главный экран с двумя видеобуферами L8, без PIP */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	#define LCDMODE_MAIN_L8	1
	//#define LCDMODE_MAIN_RGB565	1
	#define LCDMODE_MAIN_PAGES	2
	#define LCDMODE_PIXELSIZE 1

	//#define LCDMODE_PIP_L8	1
	//#define LCDMODE_PIP_RGB565	1
	//#define LCDMODE_PIP_PAGES	3

	// 0..COLORPIP_BASE-1 - волопад
	// COLORPIP_BASE..127 - надписи и элементы дизайна
	// то же с кодом больше на 128 - затененные цвета для получения полупрозрачности
	// 0..95 - палитра водопада
	// 96..111 - норм цвета
	// 112..127 - первая степень AA
	// Заполнение палитры производится в display2_xltrgb24()

	#define COLORPIP_SHADED 128
	#define COLORPIP_ALIASED 16
	#define COLORPIP_BASE 96	// should be match to PALETTESIZE

	#define LCDMODE_MAIN_L8		1	/* используется 8 бит на пиксель представление экрана. Иначе - 16 бит - RGB565. */
	//#define LCDMODE_PIP_RGB565	1	/* используется PIP с форматом 16 бит - RGB565 */
	//#define LCDMODE_PIP_L8	1	/* используется PIP с форматом 8 бит - индексные цвета */

	#define WITHFASTWATERFLOW 1

#elif LCDMODE_V2_2PAGE
	/* только главный экран с двумя видеобуферами L8, без PIP */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	#define LCDMODE_MAIN_L8	1
	//#define LCDMODE_MAIN_RGB565	1
	#define LCDMODE_MAIN_PAGES	2
	#define LCDMODE_PIXELSIZE 1

	//#define LCDMODE_PIP_L8	1
	//#define LCDMODE_PIP_RGB565	1
	//#define LCDMODE_PIP_PAGES	3

	// 0..COLORPIP_BASE-1 - волопад
	// COLORPIP_BASE..127 - надписи и элементы дизайна
	// то же с кодом больше на 128 - затененные цвета для получения полупрозрачности
	// 0..95 - палитра водопада
	// 96..111 - норм цвета
	// 112..127 - первая степень AA
	// Заполнение палитры производится в display2_xltrgb24()

	#define COLORPIP_SHADED 128
	#define COLORPIP_ALIASED 16
	#define COLORPIP_BASE 96	// should be match to PALETTESIZE

	#define LCDMODE_MAIN_L8		1	/* используется 8 бит на пиксель представление экрана. Иначе - 16 бит - RGB565. */
	//#define LCDMODE_PIP_RGB565	1	/* используется PIP с форматом 16 бит - RGB565 */
	//#define LCDMODE_PIP_L8	1	/* используется PIP с форматом 8 бит - индексные цвета */

	#define WITHFASTWATERFLOW 1

#elif LCDMODE_V2A
	/* только главный экран 16 бит двумя видеобуферами, без PIP */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	//#define LCDMODE_MAIN_L8	1
	#define LCDMODE_MAIN_RGB565	1
	#define LCDMODE_MAIN_PAGES	2
	#define LCDMODE_PIXELSIZE 2

	//#define LCDMODE_PIP_L8	1
	//#define LCDMODE_PIP_RGB565	1
	//#define LCDMODE_PIP_PAGES	3
	//#define COLORPIP_SHADED 128

	//#define LCDMODE_MAIN_L8		1	/* используется 8 бит на пиксель представление экрана. Иначе - 16 бит - RGB565. */
	//#define LCDMODE_PIP_RGB565	1	/* используется PIP с форматом 16 бит - RGB565 */
	//#define LCDMODE_PIP_L8	1	/* используется PIP с форматом 8 бит - индексные цвета */

	#define WITHFASTWATERFLOW 1

#elif LCDMODE_V2A_2PAGE
	/* только главный экран 16 бит двумя видеобуферами, без PIP */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	//#define LCDMODE_MAIN_L8	1
	#define LCDMODE_MAIN_RGB565	1
	#define LCDMODE_MAIN_PAGES	2
	#define LCDMODE_PIXELSIZE 2

	//#define LCDMODE_PIP_L8	1
	//#define LCDMODE_PIP_RGB565	1
	//#define LCDMODE_PIP_PAGES	3
	//#define COLORPIP_SHADED 128

	//#define LCDMODE_MAIN_L8		1	/* используется 8 бит на пиксель представление экрана. Иначе - 16 бит - RGB565. */
	//#define LCDMODE_PIP_RGB565	1	/* используется PIP с форматом 16 бит - RGB565 */
	//#define LCDMODE_PIP_L8	1	/* используется PIP с форматом 8 бит - индексные цвета */

	#define WITHFASTWATERFLOW 1

#elif LCDMODE_V5A
	/* только главный экран с двумя видеобуферами 32 бит ARGB8888, без PIP */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	//#define LCDMODE_MAIN_L8	1
	#define LCDMODE_MAIN_ARGB8888	1

	#define LCDMODE_MAIN_PAGES	2
	#define LCDMODE_PIXELSIZE 4

	#define WITHFASTWATERFLOW 1


#elif LCDMODE_V5A_3PAGES
	/* только главный экран с двумя видеобуферами 32 бит ARGB8888, без PIP */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	//#define LCDMODE_MAIN_L8	1
	#define LCDMODE_MAIN_ARGB8888	1

	#define LCDMODE_MAIN_PAGES	3
	#define LCDMODE_PIXELSIZE 4

	#define WITHFASTWATERFLOW 1

#elif LCDMODE_V2B
	/* только главный экран 16 бит двумя видеобуферами, без PIP */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	//#define LCDMODE_MAIN_L8	1
	#define LCDMODE_MAIN_RGB565	1
	#define LCDMODE_MAIN_PAGES	2
	#define LCDMODE_PIXELSIZE 2

	//#define LCDMODE_PIP_L8	1
	//#define LCDMODE_PIP_RGB565	1
	//#define LCDMODE_PIP_PAGES	3
	//#define COLORPIP_SHADED 128

	//#define LCDMODE_MAIN_L8		1	/* используется 8 бит на пиксель представление экрана. Иначе - 16 бит - RGB565. */
	//#define LCDMODE_PIP_RGB565	1	/* используется PIP с форматом 16 бит - RGB565 */
	//#define LCDMODE_PIP_L8	1	/* используется PIP с форматом 8 бит - индексные цвета */

	#define WITHFASTWATERFLOW 1

#elif LCDMODE_DUMMY

	#define LCDMODE_MAIN_L8		1	/* используется 8 бит на пиксель представление экрана. Иначе - 16 бит - RGB565. */
	//#define LCDMODE_MAIN_RGB565	1
	#define LCDMODE_MAIN_PAGES	0
	#define LCDMODE_PIXELSIZE 1

	#define LCDMODE_PIP_L8	1
	#define LCDMODE_PIP_PAGES	0
	//#define COLORPIP_SHADED 128


#endif

#if LCDMODE_LTDC

	#if LCDMODE_HORFILL
		#define DIM_FIRST DIM_Y
		#define DIM_SECOND DIM_X
	#else /* LCDMODE_HORFILL */
		#define DIM_FIRST DIM_X
		#define DIM_SECOND DIM_Y
	#endif /* LCDMODE_HORFILL */

#endif /* LCDMODE_LTDC */

#if WIHSPIDFSW && WIHSPIDFHW
	#error WIHSPIDFSW and WIHSPIDFHW can not be used in same time
#endif /* WIHSPIDFSW && WIHSPIDFHW */

#define WITHNOTXDACCONTROL	1	/* в этой версии нет ЦАП управления смещением TXDAC передатчика */


#if WITHTOUCHGUI

#if ! defined TSC1_TYPE
	#warning WITHTOUCHGUI without TSC1_TYPE can not compile
	#undef WITHTOUCHGUI									// Компиляция GUI без тачскрина бессмысленна
#endif /* TSC1_TYPE */

#if (DIM_X != 800 || DIM_Y != 480)						// не соблюдены требования к разрешению экрана
	#warning WITHTOUCHGUI and (DIM_X != 800 || DIM_Y != 480)
	#undef WITHTOUCHGUI									// для функционирования touch GUI
#endif

#if (__CORTEX_M == 0)
	#define FORMATFROMLIBRARY 		1
#endif

#if ! defined WITHUSEMALLOC								// необходима поддержка динамического управления памятью
	#define WITHUSEMALLOC		1
#endif /* ! defined WITHUSEMALLOC */

#if ! WITHMENU
	#error WITHMENU must be defined for WITHTOUCHGUI
#endif

#if ! defined WITHGUIHEAP || WITHGUIHEAP < (80 * 1024uL)
	#undef WITHGUIHEAP
	#define WITHGUIHEAP 		(80 * 1024uL)			// требуемый размер кучи для touch GUI
#endif /* ! defined WITHGUIHEAP || WITHGUIHEAP < (80 * 1024uL) */

#endif /* WITHTOUCHGUI */

#if WITHKEEPNVRAM && defined (NVRAM_TYPE) && (NVRAM_TYPE == NVRAM_TYPE_FM25XXXX)
	#error WITHKEEPNVRAM and NVRAM_TYPE_FM25XXXX can not be used together
#endif

// Поддержка FatFS если запрошена поддержка однорго из носителей
#define WITHUSEFATFS (WITHUSESDCARD || WITHUSEUSBFLASH || WITHUSERAMDISK)

#if defined WITHDSPLOCALFIR && defined WITHDSPLOCALTXFIR
	#undef WITHDSPLOCALTXFIR
#endif

// Работа с кешированным состоянием RTC
#define WITHRTCCACHED (WITHSPILOWSUPPORTT && defined (RTC1_TYPE) && (RTC1_TYPE == RTC_TYPE_DS1305))

#if (WIHSPIDFSW || WIHSPIDFHW) && WIHSPIDFOVERSPI
	#error WIHSPIDFOVERSPI and ( WIHSPIDFSW or WIHSPIDFHW ) can not be used together
#endif /* (WIHSPIDFSW || WIHSPIDFHW) && WIHSPIDFOVERSPI */


#define BOARDPOWERMIN	0	// Нижний предел регулировки (показываемый на дисплее)
#define BOARDPOWERMAX	100	// Верхний предел регулировки (показываемый на дисплее)

#define BOARDDACSCALEMIN	0	// Нижний предел мощности (аргумент board_set_dacscale() */
#define BOARDDACSCALEMAX	10000	// Верхний предел мощности (аргумент board_set_dacscale() */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PARAMDEPEND_H_INCLUDED */
