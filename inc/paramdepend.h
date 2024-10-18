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

	//#define CPU_FREQ ((18432000u * 73) / 14 / 2)	// satmod9if_v0 ARM board
	#define CPU_FREQ 48000000u
	//#define CPU_FREQ 12000000u

	#define ADC_FREQ	500000u	/* тактовая частота SAR преобразователя АЦП. */

	#define TICKS_FREQUENCY		(200uL * 1) // at ARM - 400 Hz

	//#define SCL_CLOCK  100000u	/* 100 kHz I2C/TWI speed */
	#define SCL_CLOCK	400000u	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (4UL * 1000 * 1000)	/* 4 MHz на SCLK - требуемая скорость передачи по SPI */

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

	#define TICKS_FREQUENCY		(200u * 1) // at ARM - 400 Hz

	// ADC clock frequency: 0.6..14 MHz
	#define ADC_FREQ	12000000u	/* тактовая частота SAR преобразователя АЦП. */
	#define SCL_CLOCK	400000u	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (PCLK1_FREQ / 4)	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */

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

	#define	REFINFREQ 16000000u 	/* definition from stm32f0xx.h нельзя использовать - SPISPEED использцтся в условной компиляции */
	//#define	REFINFREQ 4000000u 	/* definition from stm32f0xx.h нельзя использовать - SPISPEED использцтся в условной компиляции */
	//#define	REFINFREQ 2100000u 	/* definition from stm32f0xx.h нельзя использовать - SPISPEED использцтся в условной компиляции */
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

	#define TICKS_FREQUENCY		(200u * 1) // at ARM - 200 Hz

	#define BOARD_TIM21_FREQ PCLK1_FREQ

	// ADC clock frequency: 0.6..14 MHz
	#define ADC_FREQ	12000000u	/* тактовая частота SAR преобразователя АЦП. */
	#define SCL_CLOCK	400000u	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (PCLK1_FREQ / 1)	/* 8 MHz на SCLK - требуемая скорость передачи по SPI */

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
	#define TICKS_FREQUENCY		(200u * 1) // at ARM - 200 Hz

	// ADC clock frequency: 1..20 MHz
	#define ADC_FREQ	2000000u	/* тактовая частота SAR преобразователя АЦП. */
	//#define ADC_FREQ	16000000uL	/* тактовая частота SAR преобразователя АЦП. */

	#define SCL_CLOCK	400000u	/* 400 kHz I2C/TWI speed */

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
			#define	REFINFREQ 64000000u
		#else /* WITHCPUXTAL */
			// На внутреннем RC генераторе
			#define	REFINFREQ 16000000u
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

			#define HSIFREQ 64000000u
			#define HSI48FREQ 48000000u
			#if !defined  (CSI_VALUE)
			  #define CSI_VALUE    4000000U /*!< Value of the Internal oscillator in Hz*/
			#endif /* CSI_VALUE */

			#define LSEFREQ 32768u	// должно быть в файле конфигурации платы

			#define HARDWARE_SPI_FREQ (hardware_get_spi_freq())

			#define HARDWARE_NCORES 1
			#define WITHCPUNAME "STM32H7xx"

		#elif CPUSTYLE_STM32F7XX

			#define LSEFREQ 32768u	// должно быть в файле конфигурации платы

			#define PLL_FREQ	(stm32f7xx_get_pll_freq())
			//#define PLL2_FREQ	(REFINFREQ / REF2_DIV * REF2_MUL)	// STM32H7xxx
			//#define PLL3_FREQ	(REFINFREQ / REF3_DIV * REF3_MUL)	// STM32H7xxx

			#define PLLI2S_FREQ (stm32f7xx_get_plli2s_freq())
			#define	PLLI2S_FREQ_OUT (PLLI2S_FREQ / 2)		// Frequency after PLLI2S_DivQ

			#define PLLSAI_FREQ (stm32f7xx_get_pllsai_freq())
			#define PLLSAI_FREQ_OUT (PLLSAI_FREQ / 2)	// Frequency after PLLSAI_DivQ

			#define CPU_FREQ (stm32f7xx_get_sys_freq())	// 172032000uL
			#define HARDWARE_SPI_FREQ (hardware_get_spi_freq())

			#define HSIFREQ 16000000u

			#define HARDWARE_NCORES 1
			#define WITHCPUNAME "STM32F7xx"

		#elif CPUSTYLE_STM32F4XX

			#define LSEFREQ 32768u	// должно быть в файле конфигурации платы

			#define CPU_FREQ (stm32f4xx_get_sysclk_freq())	// 172032000uL
			#define HARDWARE_SPI_FREQ (stm32f4xx_get_spi1_freq())

			#define HSIFREQ 16000000u	// 16 MHz

			#define HARDWARE_NCORES 1
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
			#define	REFINFREQ 8000000u
			#define REF1_DIV 2
			#define REF1_MUL 9	// Up to 16 supported - вынести в конфигурационный файл платы
		#endif

		#define PLL_FREQ	(REFINFREQ / REF1_DIV * REF1_MUL)
		#define CPU_FREQ (PLL_FREQ / 1)

 		#if CPU_FREQ >= 48000000u
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
	#define SPISPEED (HARDWARE_SPI_FREQ / 4)	/* 14 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 12000000u//(PCLK1_FREQ / 2)	/* 28 на SCLK - требуемая скорость передачи по SPI */

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
		#define ADC_FREQ	10000000u	/* тактовая частота SAR преобразователя АЦП. */
		#define WITHREFSENSORVAL	1240	/* Reference voltage: STM32H743 1.180 1.216 1.255 */
	#elif CPUSTYLE_STM32F7XX || CPUSTYLE_STM32F4XX
		//#define HARDWARE_ADCBITS 6	/* АЦП работает с 6-битными значениями */
		//#define HARDWARE_ADCBITS 8	/* АЦП работает с 8-битными значениями */
		//#define HARDWARE_ADCBITS 10	/* АЦП работает с 10-битными значениями */
		#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */
		#define ADC_FREQ	28000000u	/* тактовая частота SAR преобразователя АЦП. */
		#define WITHREFSENSORVAL	1210	/* Reference voltage: STM32F746, STM32F429, STM32F446 = 1.21V */
	#else
		#define HARDWARE_ADCBITS 12	/* АЦП работает с 12-битными значениями */
		#define ADC_FREQ	2000000u	/* тактовая частота SAR преобразователя АЦП. */
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

	#define ADCVREF_CPU	33		// 3.3 volt
	//#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCBITS 10	/* АЦП работает с 10-битными значениями */
	//#define HARDWARE_ADCINPUTS	40	/* до 16-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#define HARDWARE_NCORES 1
	#define WITHCPUNAME "ATSAM3S"

#elif CPUSTYLE_ATSAM4S

	//#define CPU_FREQ ((18432000uL * 73) / 14 / 2)	// satmod9if_v0 ARM board
	#define CPU_FREQ 64000000uL
	//#define CPU_FREQ 48000000uL
	//#define CPU_FREQ 32000000uL
	//#define CPU_FREQ 12000000uL
	//#define BOARD_SYSTICK_FREQ CPU_FREQ	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

	// ADC clock frequency: 1..20 MHz
	#define ADC_FREQ	2000000u	/* тактовая частота SAR преобразователя АЦП. */
	//#define ADC_FREQ	16000000uL	/* тактовая частота SAR преобразователя АЦП. */

	#define TICKS_FREQUENCY		(200u * 1) // at ARM - 400 Hz

	//#define SCL_CLOCK  100000u	/* 100 kHz I2C/TWI speed */
	#define SCL_CLOCK	400000u	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (4UL * 1000 * 1000)	/* 4 MHz на SCLK - требуемая скорость передачи по SPI */

	#define ADCVREF_CPU	33		// 3.3 volt
	//#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */
	#define HARDWARE_ADCBITS 10	/* АЦП работает с 10-битными значениями */
	//#define HARDWARE_ADCINPUTS	40	/* до 16-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast16_t adcvalholder_t;		
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#define HARDWARE_NCORES 1
	#define WITHCPUNAME "ATSAM4S"

#elif CPUSTYLE_AT91SAM9XE

	#define CPU_FREQ (196608000uL / 2)	// частота периферии (процессорная после деления).
	//#define CPU_FREQ 14400000uL
	//#define SCL_CLOCK  100000L	/* 100 kHz I2C/TWI speed */
	#define SCL_CLOCK  400000uL	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (4UL * 1000 * 1000)	/* 4 MHz на SCLK - требуемая скорость передачи по SPI */

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

	#define TICKS_FREQUENCY	 (200u)	// 200 Hz - use compare/match interrupt

	//#define SCL_CLOCK  100000u	/* 100 kHz I2C/TWI speed */
	//#define SCL_CLOCK  250000u		/* 250 kHz I2C/TWI speed */
	#define SCL_CLOCK  400000u	/* 400 kHz I2C/TWI speed */
	//#define SCL_CLOCK  40000u	/* 40 kHz I2C/TWI speed */

	#define SPISPEED (CPU_FREQ / 2) /* 4 (5) MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST SPISPEED

	#define ADC_FREQ	250000u	/* тактовая частота SAR преобразователя АЦП. */

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
	#define HARDWARE_NCORES 1

#elif CPUSTYLE_ATXMEGA

	// ATMega processors

	#define CPU_FREQ (F_CPU)

	#define TICKS_FREQUENCY	 (200U * 2)	// 400 Hz - use compare/match interrupt

	//#define SCL_CLOCK  100000uL	/* 100 kHz I2C/TWI speed */
	//#define SCL_CLOCK  250000uL		/* 250 kHz I2C/TWI speed */
	#define SCL_CLOCK  400000uL	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (8000000) /* 4 (5) MHz на SCLK - требуемая скорость передачи по SPI */

	#define ADC_FREQ	125000uL	/* тактовая частота SAR преобразователя АЦП. */


	#define ADCVREF_CPU	25		// 2.5 volt
	#define HARDWARE_ADCBITS 8	/* АЦП работает с 8-битными значениями */

	//#define HARDWARE_ADCINPUTS	40	/* до 8-ти входов АЦП */
	/* тип для хранения данных, считанных с АЦП */
	typedef uint_fast8_t adcvalholder_t;		
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений
	#define HARDWARE_NCORES 1

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
	//#define GIC_PRIORITYSHIFT 3	/* ICCPMR[7:3] is valid bit */

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

	#define HARDWARE_NCORES 1
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
	#define CPU_PL1_FREQ (stm32mp1_get_hsi_freq())	/* PL1 times source frequency */

	#define HARDWARE_SPI_FREQ (hardware_get_spi_freq())
	#define BOARD_QSPI_FREQ (stm32mp1_get_qspi_freq())
	#define BOARD_USART1_FREQ  (stm32mp1_uart1_get_freq())

	#define TICKS_FREQUENCY	 (200U)	// 200 Hz

	// ADC clock frequency: 1..20 MHz
	#define SCL_CLOCK	400000u	/* 400 kHz I2C/TWI speed */

	#define SPISPEED (HARDWARE_SPI_FREQ / 4)	/* 14 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 24000000u			/* 2требуемая скорость передачи по SPI */

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
	#define HARDWARE_SPI_FREQ (xc7z_get_spi_freq())

	#define TICKS_FREQUENCY 200
	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED (12000000u)	/* 14 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 12000000u		//(PCLK1_FREQ / 2)	/* 28 на SCLK - требуемая скорость передачи по SPI */

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
	#define HARDWARE_UART_FREQ (allwnr_a64_get_uart_freq())
	#define CPU_PL1_FREQ (allwnr_a64_get_hosc_freq())	/* PL1 times source frequency */
	#define HARDWARE_HOSC_FREQ (allwnr_a64_get_hosc_freq())	/* PL1 times source frequency */

	#define TICKS_FREQUENCY 200
	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED 		(allwnr_v3s_get_hosc_freq() / 2)	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 	(allwnr_v3s_get_hosc_freq())	/* 24 MHz на SCLK - требуемая скорость передачи по SPI */

	typedef enum {
		GPIO_CFG_IN  = 0x00,
		GPIO_CFG_OUT = 0x01,
		GPIO_CFG_AF2 = 0x02,
		GPIO_CFG_AF3 = 0x03,
		GPIO_CFG_AF4 = 0x04,
		GPIO_CFG_AF5 = 0x05,
		GPIO_CFG_EINT = 0x06,	/* external interrupt sense (input) */
		GPIO_CFG_IODISABLE = 0x07,
	} GPIOMode_TypeDef;

	#define HARDWARE_NCORES 4
	#define WITHCPUNAME "Allw A64"


#elif CPUSTYLE_T507 || CPUSTYLE_H616

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

	#define CPU_FREQ	(allwnr_t507_get_cpux_freq())
	#define HARDWARE_UART_FREQ (allwnr_t507_get_uart_freq())
	#define CPU_PL1_FREQ (allwnr_t507_get_hosc_freq())	/* PL1 times source frequency */
	#define HARDWARE_HOSC_FREQ (allwnr_t507_get_hosc_freq())	/* PL1 times source frequency */

	#define TICKS_FREQUENCY 200
	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED 		(allwnr_t507_get_hosc_freq() / 2)	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 	(allwnr_t507_get_hosc_freq())	/* 24 MHz на SCLK - требуемая скорость передачи по SPI */

	typedef enum {
		GPIO_CFG_IN  = 0x00,
		GPIO_CFG_OUT = 0x01,
		GPIO_CFG_AF2 = 0x02,
		GPIO_CFG_AF3 = 0x03,
		GPIO_CFG_AF4 = 0x04,
		GPIO_CFG_AF5 = 0x05,
		GPIO_CFG_EINT = 0x06,	/* external interrupt sense (input) */
		GPIO_CFG_IODISABLE = 0x07,
	} GPIOMode_TypeDef;

	#define HARDWARE_NCORES 4
	#define WITHCPUNAME "Allw T507"

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

	//#define HARDWARE_CLK32K_FREQ 32000uL
	#define HARDWARE_CLK16M_RC_FREQ 16000000uL

	#define CPU_FREQ	(allwnr_t113_get_arm_freq())
	#define HARDWARE_UART_FREQ (allwnr_t113_get_uart_freq())
	#define CPU_PL1_FREQ (allwnr_t113_get_hosc_freq())	/* PL1 times source frequency */
	#define HARDWARE_HOSC_FREQ (allwnr_t113_get_hosc_freq())	/* PL1 times source frequency */

	#define TICKS_FREQUENCY 200
	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED 		(allwnr_t113_get_hosc_freq() / 2)	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 	(allwnr_t113_get_hosc_freq())	/* 24 MHz на SCLK - требуемая скорость передачи по SPI */

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
	#define WITHCPUNAME "Allw T113-s3"

#elif CPUSTYLE_H3

	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#if WITHCPUXOSC
		// с внешним генератором
		#define	REFINFREQ WITHCPUXOSC
	#elif WITHCPUXTAL
		// с внешним кварцевым резонатором
		#define	REFINFREQ WITHCPUXTAL
	#endif /* WITHCPUXTAL */

	//#define HARDWARE_CLK32K_FREQ 32000uL
	#define HARDWARE_CLK16M_RC_FREQ 16000000uL

	#define CPU_FREQ	(allwnr_h3_get_cpux_freq())
	#define HARDWARE_UART_FREQ (allwnr_h3_get_uart_freq())
	#define CPU_PL1_FREQ (allwnr_h3_get_hosc_freq())	/* PL1 times source frequency */
	#define HARDWARE_HOSC_FREQ (allwnr_h3_get_hosc_freq())	/* PL1 times source frequency */

	#define TICKS_FREQUENCY 200
	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED 		(allwnr_h3_get_hosc_freq() / 2)	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 	(allwnr_h3_get_hosc_freq())	/* 24 MHz на SCLK - требуемая скорость передачи по SPI */

	typedef enum {
		GPIO_CFG_IN  = 0x00,
		GPIO_CFG_OUT = 0x01,
		GPIO_CFG_AF2 = 0x02,
		GPIO_CFG_AF3 = 0x03,
		GPIO_CFG_AF4 = 0x04,
		GPIO_CFG_AF5 = 0x05,
		GPIO_CFG_EINT = 0x06,	/* external interrupt sense (input) */
		GPIO_CFG_IODISABLE = 0x07,
	} GPIOMode_TypeDef;

	#define HARDWARE_NCORES 4
	#define WITHCPUNAME "Allw H3"


#elif CPUSTYLE_V3S

	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#if WITHCPUXOSC
		// с внешним генератором
		#define	REFINFREQ WITHCPUXOSC
	#elif WITHCPUXTAL
		// с внешним кварцевым резонатором
		#define	REFINFREQ WITHCPUXTAL
	#endif /* WITHCPUXTAL */

	//#define HARDWARE_CLK32K_FREQ 32000uL
	#define HARDWARE_CLK16M_RC_FREQ 16000000uL

	#define CPU_FREQ			(allwnr_v3s_get_cpu_freq())
	#define HARDWARE_UART_FREQ 	(allwnr_v3s_get_uart_freq())
	#define CPU_PL1_FREQ 		(allwnr_v3s_get_hosc_freq())	/* PL1 times source frequency */
	#define HARDWARE_HOSC_FREQ 	(allwnr_v3s_get_hosc_freq())	/* PL1 times source frequency */

	#define TICKS_FREQUENCY 200
	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED 		(allwnr_t113_get_hosc_freq() / 2)	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 	(allwnr_t113_get_hosc_freq())	/* 24 MHz на SCLK - требуемая скорость передачи по SPI */

	typedef enum {
		GPIO_CFG_IN  = 0x00,
		GPIO_CFG_OUT = 0x01,
		GPIO_CFG_AF2 = 0x02,
		GPIO_CFG_AF3 = 0x03,
		GPIO_CFG_AF4 = 0x04,
		GPIO_CFG_AF5 = 0x05,
		GPIO_CFG_EINT = 0x06,	/* external interrupt sense (input) */
		GPIO_CFG_IODISABLE = 0x07,
	} GPIOMode_TypeDef;

	#define HARDWARE_NCORES 1
	#define WITHCPUNAME "Allw V3s"

#elif CPUSTYLE_VM14

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

	#define CPU_FREQ	(elveesvm14_get_arm_freq())
	#define HARDWARE_SPI_FREQ (elveesvm14_get_spi_freq())
	#define HARDWARE_UART_FREQ (elveesvm14_get_usart_freq())

	#define TICKS_FREQUENCY 200
	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED 		12000000u	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 	24000000u	/* 24 MHz на SCLK - требуемая скорость передачи по SPI */

	#define HARDWARE_NCORES 2
	#define WITHCPUNAME "Elvees VM14"

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

	//#define HARDWARE_CLK32K_FREQ 32000uL
	#define HARDWARE_CLK16M_RC_FREQ 16000000uL

	#define CPU_FREQ	(allwnr_f133_get_riscv_freq())
	#define HARDWARE_UART_FREQ (allwnr_t113_get_uart_freq())
	#define HARDWARE_HOSC_FREQ (allwnr_t113_get_hosc_freq())	/* PL1 times source frequency */

	#define TICKS_FREQUENCY 200
	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED 		(allwnr_t113_get_hosc_freq() / 2)	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 	(allwnr_t113_get_hosc_freq())	/* 24 MHz на SCLK - требуемая скорость передачи по SPI */

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

	#define HARDWARE_NCORES 1
	#define WITHCPUNAME "Allw F133-A"

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
	#define HARDWARE_SPI_FREQ (xc7z_get_spi_freq())

	#define TICKS_FREQUENCY 200
	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED (12000000uL)	/* 14 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 12000000uL//(PCLK1_FREQ / 2)	/* 28 на SCLK - требуемая скорость передачи по SPI */

	#define HARDWARE_NCORES 4
	#define WITHCPUNAME "Zynq XCZU"

#else

	#error Undefined CPUSTYLE_XXX

#endif

#if LINUX_SUBSYSTEM

	/* Linux targets: No any hardware IRQ control */

	typedef uint_fast32_t IRQL_t;

	#define IRQL_SYSTEM 			0
	#define IRQL_REALTIME 			0
	#define IRQL_OVERREALTIME 		0

	#define global_enableIRQ() do {  } while (0)
	#define global_disableIRQ() do {  } while (0)

#elif defined (__CORTEX_M)

	/* Cortex-M targets */

	typedef uint_fast32_t IRQL_t;

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

	#define IRQL_SYSTEM ((NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0) << (8 - __NVIC_PRIO_BITS)) & 0xff)	// value for __set_BASEPRI
	#define IRQL_REALTIME ((NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 1, 0) << (8 - __NVIC_PRIO_BITS)) & 0xff)	// value for __set_BASEPRI
	#define IRQL_OVERREALTIME ((NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0) << (8 - __NVIC_PRIO_BITS)) & 0xff)	// value for __set_BASEPRI

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

	typedef uint_fast8_t IRQL_t;

	#define global_enableIRQ() do { sei(); } while (0)
	#define global_disableIRQ() do { cli(); } while (0)

#elif (__GIC_PRESENT == 1)

	typedef uint_fast32_t IRQL_t;

	// The processor does not process any exception with a priority value greater than or equal to BASEPRI.
	extern uint32_t gARM_OVERREALTIME_PRIORITY;
	extern uint32_t gARM_REALTIME_PRIORITY;
	extern uint32_t gARM_SYSTEM_PRIORITY;
	extern uint32_t gARM_BASEPRI_ONLY_REALTIME;
	extern uint32_t gARM_BASEPRI_ONLY_OVERREALTIME;
	extern uint32_t gARM_BASEPRI_ALL_ENABLED;

	#define GIC_ENCODE_PRIORITY(v) ((v) << (GIC_GetBinaryPoint() + 1))

	#define IRQL_SYSTEM GIC_ENCODE_PRIORITY(PRI_SYS)	// value for GIC_SetInterfacePriorityMask
	#define IRQL_REALTIME GIC_ENCODE_PRIORITY(PRI_RT)	// value for GIC_SetInterfacePriorityMask
	#define IRQL_OVERREALTIME GIC_ENCODE_PRIORITY(PRI_OVRT)	// value for GIC_SetInterfacePriorityMask
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

//		gARM_OVERREALTIME_PRIORITY = GIC_ENCODE_PRIORITY(PRI_OVRT),	// value for GIC_SetPriority
//		gARM_REALTIME_PRIORITY = GIC_ENCODE_PRIORITY(PRI_RT),	// value for GIC_SetPriority
//		gARM_SYSTEM_PRIORITY = GIC_ENCODE_PRIORITY(PRI_SYS),		// value for GIC_SetPriority
//
//		gARM_BASEPRI_ONLY_REALTIME = GIC_ENCODE_PRIORITY(PRI_SYS),	// value for GIC_SetInterfacePriorityMask
//		gARM_BASEPRI_ALL_ENABLED = GIC_ENCODE_PRIORITY(PRI_USER)	// value for GIC_SetInterfacePriorityMask
		PRI_count
	};

	#define BOARD_SGI_IRQ 	SGI1_IRQn		/* Прерывание для синхронизации приоритетов GIC на остальных процессорах  */
	#define BOARD_SGI_PRIO	GIC_ENCODE_PRIORITY(PRI_IPC)

	#define RUNNING_PRI	((GICInterface->RPR & 0xFF) >> GIC_PRIORITYSHIFT) // The current running priority on the CPU interface.

	// A lower priority value indicating a higher priority
	#define ARM_OVERREALTIME_PRIORITY	((const uint32_t) gARM_OVERREALTIME_PRIORITY)
	#define ARM_REALTIME_PRIORITY	((const uint32_t) gARM_REALTIME_PRIORITY)
	#define ARM_SYSTEM_PRIORITY	((const uint32_t) gARM_SYSTEM_PRIORITY)

	#if defined(__aarch64__)

		#define global_enableIRQ() do { \
			} while (0)
		#define global_disableIRQ() do { \
		} while (0)

	#else /* defined(__aarch64__) */

		#define global_enableIRQ() do { \
			__enable_irq(); \
			} while (0)
		#define global_disableIRQ() do { \
			__disable_irq(); \
		} while (0)
	#endif /* defined(__aarch64__) */

#elif CPUSTYLE_RISCV

	typedef uint_xlen_t IRQL_t;

	#define ARM_IPC_PRIORITY			4
	#define ARM_OVERREALTIME_PRIORITY	3	/* валкодер */
	#define ARM_REALTIME_PRIORITY		2	/* звук */
	#define ARM_SYSTEM_PRIORITY			1	/* таймеры, USB */
	#define ARM_USER_PRIORITY			0	/* Значение, на которое инициализируется PLIC->PLIC_MTH_REG */

	#define global_enableIRQ() do { csr_set_bits_mstatus(MSTATUS_MIE_BIT_MASK); } while (0)
	#define global_disableIRQ() do { csr_clr_bits_mstatus(MSTATUS_MIE_BIT_MASK); } while (0)

	#define IRQL_SYSTEM 			ARM_SYSTEM_PRIORITY
	#define IRQL_REALTIME 			ARM_REALTIME_PRIORITY
	#define IRQL_OVERREALTIME		ARM_OVERREALTIME_PRIORITY

#else /* CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 */

	#warning Unsupported target IRQ control

	typedef uint_fast32_t IRQL_t;

	#define IRQL_SYSTEM 			0
	#define IRQL_REALTIME 		0
	#define IRQL_OVERREALTIME 		0

	#define global_enableIRQ() do {  } while (0)
	#define global_disableIRQ() do {  } while (0)

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
#define CODEC_TYPE_AWHWCODEC	46	// Allwinner embedded audio codec

#define RTC_TYPE_DS1305		50	/* MAXIM DS1305EN RTC clock chip with SPI interface */
#define RTC_TYPE_DS1307		51	/* MAXIM DS1307/DS3231 RTC clock chip with I2C interface */
#define RTC_TYPE_M41T81		52	/* ST M41T81M6/M41T81SM6F RTC clock chip with I2C interface */
#define RTC_TYPE_STM32F4xx	53	/* STM32F4xx internal RTC peripherial */
#define RTC_TYPE_STM32F0xx	54	/* STM32F0xx internal RTC peripherial */
#define RTC_TYPE_STM32L0xx	55	/* STM32L0xx internal RTC peripherial */
#define RTC_TYPE_LINUX		56	/* Linux local time */
#define RTC_TYPE_GPS		57	/* GPS time */

#define TSC_TYPE_TSC2046	60	// Resistive touch screen controller TI TSC2046 - use TSC_TYPE_XPT2046
#define TSC_TYPE_STMPE811	61	// Resistive touch screen controller ST STMPE811
#define TSC_TYPE_GT911		62	// Capacitive touch screen controller Goodix GT911
#define TSC_TYPE_S3402		63	// Capacitive touch screen controller S3402 (on panel H497TLB01.4)
#define TSC_TYPE_FT5336 	64	// Capacitive touch screen controller FocalTech FT5336
#define TSC_TYPE_XPT2046 	65	// Resistive touch screen controller SHENZHEN XPTEK TECHNOLOGY CO., LTD http://www.xptek.com.cn
#define TSC_TYPE_ILI2102	66	// Capacitive touch screen controller Ilitek ILI2102
#define TSC_TYPE_AWTPADC	67	// Allwinner F133/t113-s3 resistive touch screen controller
#define TSC_TYPE_EVDEV		68	// Linux input device

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

#if LCDMODE_LQ043T3DX02K		/* PSP-1000 with S1D113780, or attached to LTDC hardware controller */
	#define DIM_X 480
	#define DIM_Y 272
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
#endif /* LCDMODE_LQ043T3DX02K */

#if LCDMODE_AT070TN90	/* AT070TN90 panel (800*480) - 7" display */
	#define LCDMODE_SPI_RN 1 // эти дисплеи требуют только RESET
	#define DIM_X 800
	#define DIM_Y 480
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
#endif /* LCDMODE_AT070TN90 */

#if LCDMODE_AT070TNA2	/* AT070TNA2 panel (1024*600) - 7" display */
	#define LCDMODE_SPI_RN 1 // эти дисплеи требуют только RESET
	#define DIM_X 1024
	#define DIM_Y 600
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
#endif /* LCDMODE_AT070TNA2 */

#if LCDMODE_LQ123K3LG01	/* LQ123K3LG01 panel (1280*480) - 12.3" display LVDS mode */
	#define DIM_X 1280
	#define DIM_Y 480
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
#endif /* LCDMODE_LQ123K3LG01 */

#if LCDMODE_TCG104XGLPAPNN	/* TCG104XGLPAPNN-AN30 panel (1024*768) - 10.4" display - DE mode required */
	#define LCDMODE_SPI_RN 1 // эти дисплеи требуют только RESET
	#define DIM_X 1024
	#define DIM_Y 768
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
#endif /* LCDMODE_TCG104XGLPAPNN */

#if LCDMODE_H497TLB01P4	/* 720xRGBx1280 - 5" AMOELD Panel H497TLB01.4 */
	#define DIM_X 720
	#define DIM_Y 1280
	#define LCDMODE_COLORED	1
	#define LCDMODE_HORFILL	1
#endif /* LCDMODE_H497TLB01P4 */

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
	//#define DSTYLE_G_X800_Y480	1	/* AT070TN90 panel (800*480) - 7" display */
	#define DSTYLE_G_X1024_Y600	1	/* AT070TNA2 panel (1024*600) - 7" display */
	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#elif DIM_X == 1280 && DIM_Y == 480
	//#define DSTYLE_G_X800_Y480	1	/* AT070TN90 panel (800*480) - 7" display */
	#define DSTYLE_G_X1024_Y600	1	/* AT070TNA2 panel (1024*600) - 7" display */
	//#define DSTYLE_G_X1280_Y480	1	/* LQ123K3LG01 panel (1280*480) - 12.3" display LVDS mode */
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
s
#elif LCDMODE_DUMMY

	#define DSTYLE_G_DUMMY	1	// пустой список отображения

	#define DIM_X 480
	#define DIM_Y 272
	#define LCDMODE_COLORED	1

	#define CHAR_W 6
	#define CHAR_H 8

	#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейки сетки разметки отображния */
	#define GRID2X(cellsx) ((cellsx) * CHAR_W)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	#define GRID2Y(cellsy) ((cellsy) * CHAR_H)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

	#define DISPLAYMODES_FPS 5	/* количество обновлений отображения режимов работы */
	#define DISPLAY_FPS	10	/* обновление показаний частоты за секунду */
	#define DISPLAYSWR_FPS 5	/* количество обновлений SWR за секунду */

#else
	#error Unrecognized dislay layout used (LCDMODE_XXXX)
#endif

#define LCDMODE_HORFILL	1

#define HALFCOUNT_FREQA 1
#define HALFCOUNT_SMALL 1
#define HALFCOUNT_BIG 1

// Определения функциональности в зависимости от того, какой набор управляющих битов имеется.
#if CTLREGMODE_NOCTLREG			// Отсутствующий регистр управления

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
#define WITHHARDINTERLOCK (CPUSTYLE_ARM_CM7 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM0 || (__CORTEX_A != 0))

#ifndef WITHFLATMENU
	#define WITHFLATMENU (CTLSTYLE_SW2011ALL && ! CPUSTYLE_ATMEGA_XXX4)
#endif /* WITHFLATMENU */

#define HARDWARE_DELAY_MS(t) do { local_delay_ms(t); } while (0)	//HAL_Delay(t)
#define HARDWARE_DELAY_US(t) do { local_delay_us(t); } while (0)
#define HARDWARE_GETTICK_MS() ((uint32_t) 0) // HAL_GetTick()

#define WITHSPECTRUMWF (WITHIF4DSP && (WITHRTS96 || WITHRTS192) && ! LCDMODE_DUMMY)

/* Сброс мощности при запросе TUNE от автотюнера или извне */
#define WITHLOWPOWEREXTTUNE	(defined (HARDWARE_GET_TUNE) || WITHAUTOTUNER)

/* Зависимости для поределения конфигурации видеосистемы */

#if LCDMODE_V2
	/* только главный экран с двумя видеобуферами L8, без PIP */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	#define LCDMODE_MAIN_L8	1
	//#define LCDMODE_MAIN_RGB565	1
	#define LCDMODE_PIXELSIZE 1

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

	#define WITHFASTWATERFLOW 1

#elif LCDMODE_V2_2PAGE
	/* только главный экран с двумя видеобуферами L8, без PIP */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	#define LCDMODE_MAIN_L8	1
	//#define LCDMODE_MAIN_RGB565	1
	#define LCDMODE_PIXELSIZE 1

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

	#define WITHFASTWATERFLOW 1

#elif LCDMODE_V2A
	/* только главный экран 16 бит двумя видеобуферами, без PIP */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	//#define LCDMODE_MAIN_L8	1
	#define LCDMODE_MAIN_RGB565	1
	#define LCDMODE_PIXELSIZE 2

	#define WITHFASTWATERFLOW 1

#elif LCDMODE_V2A_2PAGE
	/* только главный экран 16 бит двумя видеобуферами, без PIP */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	//#define LCDMODE_MAIN_L8	1
	#define LCDMODE_MAIN_RGB565	1
	#define LCDMODE_PIXELSIZE 2

	#define WITHFASTWATERFLOW 1

#elif LCDMODE_V5A
	/* только главный экран с двумя видеобуферами 32 бит ARGB8888, без PIP */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	//#define LCDMODE_MAIN_L8	1
	#define LCDMODE_MAIN_ARGB8888	1
	#define LCDMODE_PIXELSIZE 4

	#define WITHFASTWATERFLOW 1

#elif LCDMODE_V2B
	/* только главный экран 16 бит двумя видеобуферами, без PIP */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	//#define LCDMODE_MAIN_L8	1
	#define LCDMODE_MAIN_RGB565	1
	#define LCDMODE_PIXELSIZE 2

	#define WITHFASTWATERFLOW 1

#elif LCDMODE_DUMMY

	#define LCDMODE_MAIN_L8		1	/* используется 8 бит на пиксель представление экрана. Иначе - 16 бит - RGB565. */
	//#define LCDMODE_MAIN_RGB565	1
	#define LCDMODE_PIXELSIZE 1

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

#define WITHPRERENDER (1 && WITHLTDCHW && (LCDMODE_MAIN_RGB565 || LCDMODE_MAIN_ARGB8888))		/* использование предварительно построенных изображений при отображении частоты */

#if WITHTOUCHGUI

#if ! defined TSC1_TYPE
	#warning WITHTOUCHGUI without TSC1_TYPE can not compile
	#undef WITHTOUCHGUI									// Компиляция GUI без тачскрина бессмысленна
#endif /* TSC1_TYPE */

#if (DIM_X != 800 || DIM_Y != 480)						// не соблюдены требования к разрешению экрана
	#warning WITHTOUCHGUI and (DIM_X != 800 || DIM_Y != 480)
	#undef WITHTOUCHGUI									// для функционирования touch GUI
#endif

#if ! defined (__CORTEX_M)
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

#if (WIHSPIDFSW || WIHSPIDFHW) && WIHSPIDFOVERSPI
	#error WIHSPIDFOVERSPI and ( WIHSPIDFSW or WIHSPIDFHW ) can not be used together
#endif /* (WIHSPIDFSW || WIHSPIDFHW) && WIHSPIDFOVERSPI */

#if WITHIF4DSP
	#define BOARDPOWERMIN	0	// Нижний предел регулировки (показываемый на дисплее)
	#define BOARDPOWERMAX	100	// Верхний предел регулировки (показываемый на дисплее)

	#define BOARDDACSCALEMIN	0	// Нижний предел мощности (аргумент board_set_dacscale() */
	#define BOARDDACSCALEMAX	10000	// Верхний предел мощности (аргумент board_set_dacscale() */
#endif /* WITHIF4DSP */

#if defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_TSC2046)
	#error Use TSC_TYPE_XPT2046 instead TSC_TYPE_TSC2046
#endif /* defined (TSC1_TYPE) && (TSC1_TYPE == TSC_TYPE_TSC2046) */

#if defined (RTC1_TYPE) && (RTC1_TYPE == RTC_TYPE_GPS) && ! defined WITHNMEA
	#error RTC_TYPE_GPS and WITHNMEA must be used in same time
#endif /* defined (RTC1_TYPE) && (RTC1_TYPE == RTC_TYPE_GPS) && ! defined WITHNMEA */

#if LINUX_SUBSYSTEM && WITHLVGL
	#undef WITHTOUCHGUI
	#undef TSC1_TYPE
#endif /* LINUX_SUBSYSTEM && WITHLVGL */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PARAMDEPEND_H_INCLUDED */
