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

#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

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
			//#define CSI_VALUE    4000000U /*!< Value of the Internal oscillator in Hz*/

			#define LSEFREQ 32768u	// должно быть в файле конфигурации платы

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
			//#define HARDWARE_SPI_FREQ (hardware_get_spi_freq())

			#define HSIFREQ 16000000u

			#define HARDWARE_NCORES 1
			#define WITHCPUNAME "STM32F7xx"

		#elif CPUSTYLE_STM32F4XX

			#define LSEFREQ 32768u	// должно быть в файле конфигурации платы

			#define CPU_FREQ (stm32f4xx_get_sysclk_freq())	// 172032000uL
			//#define HARDWARE_SPI_FREQ (stm32f4xx_get_spi1_freq())

			#define HSIFREQ 16000000u	// 16 MHz

			#define HARDWARE_NCORES 1
			#define WITHCPUNAME "STM32F4xx"

		#endif

	#endif

	#define TICKS_FREQUENCY 200	// Hz

	#define SEQ_TICKS_PERIOD    5    // 5 ms
	#define KBD_TICKS_PERIOD    5    // 5 ms - keyboard and HW ADC restart period
	#define ENC_TICKS_PERIOD    5    // 5 ms

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

#elif CPUSTYLE_R7S721

	//#define WITHCPUXTAL 12000000uL			/* На процессоре установлен кварц 12.000 МГц */
	#define CPU_FREQ	(30 * WITHCPUXTAL)		/* 12 MHz * 30 - clock mode 0, xtal 12 MHz */

	#define BCLOCK_FREQ		(CPU_FREQ / 3)		// 120 MHz
	#define P1CLOCK_FREQ	(CPU_FREQ / 6)		// 60 MHz
	#define P0CLOCK_FREQ	(CPU_FREQ / 12)		// 30 MHz

	#define TICKS_FREQUENCY		200	// Hz

	#define SEQ_TICKS_PERIOD    5    // 5 ms
	#define KBD_TICKS_PERIOD    5    // 5 ms - keyboard and HW ADC restart period
	#define ENC_TICKS_PERIOD    5    // 5 ms

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
	#define CSI_VALUE 4000000U /*!< Value of the Internal oscillator in Hz*/
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

	#define BOARD_USART1_FREQ  (stm32mp1_uart1_get_freq())

	#define TICKS_FREQUENCY	 200	// Hz

	#define SEQ_TICKS_PERIOD    5    // 5 ms
	#define KBD_TICKS_PERIOD    5    // 5 ms - keyboard and HW ADC restart period
	#define ENC_TICKS_PERIOD    5    // 5 ms

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

	#define CPU_FREQ	(xc7z_get_arm_freq())
	//#define HARDWARE_SPI_FREQ (xc7z_get_spi_freq())

	#define TICKS_FREQUENCY 1000	// Hz

	#define SEQ_TICKS_PERIOD    5    // 5 ms
	#define KBD_TICKS_PERIOD    5    // 5 ms - keyboard and HW ADC restart period
	#define ENC_TICKS_PERIOD    5    // 5 ms

	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED (12000000u)	/* 14 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 12000000u		//(PCLK1_FREQ / 2)	/* 28 на SCLK - требуемая скорость передачи по SPI */

	#define HARDWARE_NCORES 2

#elif CPUSTYLE_A64

	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#define HARDWARE_CLK16M_RC_FREQ 16000000u

	#define CPU_FREQ	(allwnr_a64_get_cpux_freq())
	#define HARDWARE_UART_FREQ (allwnr_a64_get_uart_freq())
	#define CPU_PL1_FREQ (allwnr_a64_get_hosc_freq())	/* PL1 times source frequency */
	#define HARDWARE_HOSC_FREQ (allwnr_a64_get_hosc_freq())	/* PL1 times source frequency */

	#define TICKS_FREQUENCY 1000	// Hz

	#define SEQ_TICKS_PERIOD    5    // 5 ms
	#define KBD_TICKS_PERIOD    5    // 5 ms - keyboard and HW ADC restart period
	#define ENC_TICKS_PERIOD    5    // 5 ms

	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED 		(12500000)	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 	(25000000)	/* 24 MHz на SCLK - требуемая скорость передачи по SPI */

	#define HARDWARE_NCORES 4
	#if defined(__aarch64__)
		#define WITHCPUNAME "Allw A64_64"
	#else
		#define WITHCPUNAME "Allw A64"
	#endif


#elif (CPUSTYLE_A133 || CPUSTYLE_R828)

	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#define HARDWARE_CLK16M_RC_FREQ 16000000u

	#define CPU_FREQ	(allwnr_a133_get_cpux_freq())
	//#define HARDWARE_SPI_FREQ (allwnr_a133_get_spi1_freq())
	#define HARDWARE_UART_FREQ (allwnr_a133_get_uart_freq())

	#define CPU_PL1_FREQ (allwnr_a133_get_hosc_freq())	/* PL1 times source frequency */
	#define HARDWARE_HOSC_FREQ (allwnr_a133_get_hosc_freq())	/* PL1 times source frequency */

	#define TICKS_FREQUENCY 1000	// Hz

	#define SEQ_TICKS_PERIOD	1	// 5 ms
	#define KBD_TICKS_PERIOD    5    // 5 ms - keyboard and HW ADC restart period
	#define ENC_TICKS_PERIOD	5	// 5 ms

	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED 		(12500000)	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 	(25000000)	/* 24 MHz на SCLK - требуемая скорость передачи по SPI */

	#define HARDWARE_NCORES 4
	#if defined(__aarch64__)
		#define WITHCPUNAME "Allw A133_64"
	#else
		#define WITHCPUNAME "Allw A133"
	#endif


#elif CPUSTYLE_T507

	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#define HARDWARE_CLK16M_RC_FREQ 16000000u

	#define CPU_FREQ	(allwnr_t507_get_cpux_freq())
	#define HARDWARE_UART_FREQ (allwnr_t507_get_uart_freq())

	#define CPU_PL1_FREQ (allwnr_t507_get_hosc_freq())	/* PL1 times source frequency */
	#define HARDWARE_HOSC_FREQ (allwnr_t507_get_hosc_freq())	/* PL1 times source frequency */

	#define TICKS_FREQUENCY 1000	// Hz

	#define SEQ_TICKS_PERIOD	1	// 5 ms
	#define KBD_TICKS_PERIOD    5    // 5 ms - keyboard and HW ADC restart period
	#define ENC_TICKS_PERIOD	5	// 5 ms

	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED 		(12500000)	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 	(25000000)	/* 24 MHz на SCLK - требуемая скорость передачи по SPI */

	#define HARDWARE_NCORES 4
	#if defined(__aarch64__)
		#define WITHCPUNAME "Allw T507_64"
	#else
		#define WITHCPUNAME "Allw T507"
	#endif

#elif CPUSTYLE_T113

	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#define HARDWARE_CLK16M_RC_FREQ 16000000uL

	#define CPU_FREQ	(allwnr_t113_get_arm_freq())
	#define HARDWARE_UART_FREQ (allwnr_t113_get_uart_freq())
	#define CPU_PL1_FREQ (allwnr_t113_get_hosc_freq())	/* PL1 times source frequency */
	#define HARDWARE_HOSC_FREQ (allwnr_t113_get_hosc_freq())	/* PL1 times source frequency */

	#define TICKS_FREQUENCY 1000	// Hz

	#define SEQ_TICKS_PERIOD	1	// 5 ms
	#define KBD_TICKS_PERIOD    5    // 5 ms - keyboard and HW ADC restart period
	#define ENC_TICKS_PERIOD	5	// 5 ms

	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED 		(12500000)	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 	(25000000)	/* 24 MHz на SCLK - требуемая скорость передачи по SPI */

	#define HARDWARE_NCORES 2
	#define WITHCPUNAME "Allw T113-s3"

#elif CPUSTYLE_H3

	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#define HARDWARE_CLK16M_RC_FREQ 16000000uL

	#define CPU_FREQ	(allwnr_h3_get_cpux_freq())
	#define HARDWARE_UART_FREQ (allwnr_h3_get_uart_freq())
	#define CPU_PL1_FREQ (allwnr_h3_get_hosc_freq())	/* PL1 times source frequency */
	#define HARDWARE_HOSC_FREQ (allwnr_h3_get_hosc_freq())	/* PL1 times source frequency */

	#define TICKS_FREQUENCY 1000	// Hz

	#define SEQ_TICKS_PERIOD    5    // 5 ms
	#define KBD_TICKS_PERIOD    5    // 5 ms - keyboard and HW ADC restart period
	#define ENC_TICKS_PERIOD    5    // 5 ms

	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED 		(12500000)	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 	(25000000)	/* 24 MHz на SCLK - требуемая скорость передачи по SPI */

	#define HARDWARE_NCORES 4
	#if defined(__aarch64__)
		#define WITHCPUNAME "Allw H3_64"
	#else
		#define WITHCPUNAME "Allw H3"
	#endif


#elif CPUSTYLE_V3S

	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#define HARDWARE_CLK16M_RC_FREQ 16000000uL

	#define CPU_FREQ			(allwnr_v3s_get_cpu_freq())
	#define HARDWARE_UART_FREQ 	(allwnr_v3s_get_uart_freq())
	#define CPU_PL1_FREQ 		(allwnr_v3s_get_hosc_freq())	/* PL1 times source frequency */
	#define HARDWARE_HOSC_FREQ 	(allwnr_v3s_get_hosc_freq())	/* PL1 times source frequency */

	#define TICKS_FREQUENCY 200

	#define SEQ_TICKS_PERIOD    5    // 5 ms
	#define KBD_TICKS_PERIOD    5    // 5 ms - keyboard and HW ADC restart period
	#define ENC_TICKS_PERIOD    5    // 5 ms

	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED 		(12500000)	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 	(25000000)	/* 24 MHz на SCLK - требуемая скорость передачи по SPI */

	#define HARDWARE_NCORES 1
	#define WITHCPUNAME "Allw V3s"

#elif CPUSTYLE_F133

	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#define HARDWARE_CLK16M_RC_FREQ 16000000uL

	#define CPU_FREQ	(allwnr_f133_get_riscv_freq())
	#define HARDWARE_UART_FREQ (allwnr_t113_get_uart_freq())
	#define HARDWARE_HOSC_FREQ (allwnr_t113_get_hosc_freq())	/* PL1 times source frequency */

	#define TICKS_FREQUENCY 200

	#define SEQ_TICKS_PERIOD    5    // 5 ms
	#define KBD_TICKS_PERIOD    5    // 5 ms - keyboard and HW ADC restart period
	#define ENC_TICKS_PERIOD    5    // 5 ms

	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED 		(12500000)	/* 12 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 	(25000000)	/* 24 MHz на SCLK - требуемая скорость передачи по SPI */

	#define HARDWARE_NCORES 1
	#define WITHCPUNAME "Allw F133-A"

#elif CPUSTYLE_RK356X
	// Rockchip RK356х

	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#define CPU_FREQ	1000000000u //(xc7z_get_arm_freq())
	//#define HARDWARE_SPI_FREQ (xc7z_get_spi_freq())
	#define HARDWARE_UART_FREQ (24000000)
	#define HARDWARE_HOSC_FREQ (24000000)	/* PL1 times source frequency */

	#define TICKS_FREQUENCY 1000	// Hz

	#define SEQ_TICKS_PERIOD    5    // 5 ms
	#define KBD_TICKS_PERIOD    5    // 5 ms - keyboard and HW ADC restart period
	#define ENC_TICKS_PERIOD    5    // 5 ms

	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED (12000000uL)	/* 14 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 12000000uL//(PCLK1_FREQ / 2)	/* 28 на SCLK - требуемая скорость передачи по SPI */

	#define HARDWARE_NCORES 4
	#if defined(__aarch64__)
		#define WITHCPUNAME "RK356x_64"
	#else
		#define WITHCPUNAME "RK356x"
	#endif

#elif CPUSTYLE_BROADCOM
	// Broadcom BCM2711

	typedef uint_fast16_t adcvalholder_t;
	typedef int_fast16_t sadcvalholder_t;	// для хранения знаковых значений

	#define CPU_FREQ	1000000000u //(xc7z_get_arm_freq())
	//#define HARDWARE_SPI_FREQ (xc7z_get_spi_freq())
	#define HARDWARE_UART_FREQ (24000000)
	#define HARDWARE_HOSC_FREQ (24000000)	/* PL1 times source frequency */

	#define TICKS_FREQUENCY 1000	// Hz

	#define SEQ_TICKS_PERIOD    5    // 5 ms
	#define KBD_TICKS_PERIOD    5    // 5 ms - keyboard and HW ADC restart period
	#define ENC_TICKS_PERIOD    5    // 5 ms

	#define ADCVREF_CPU	33		// 3.3 volt
	#define HARDWARE_ADCBITS 12

	#define SPISPEED (12000000uL)	/* 14 MHz на SCLK - требуемая скорость передачи по SPI */
	#define SPISPEEDUFAST 12000000uL//(PCLK1_FREQ / 2)	/* 28 на SCLK - требуемая скорость передачи по SPI */

	#define HARDWARE_NCORES 4
	#if defined(__aarch64__)
		#define WITHCPUNAME "BCM2711_64"
	#else
		#define WITHCPUNAME "BCM2711"
	#endif

#else

	#error Undefined CPUSTYLE_XXX

#endif

#if LINUX_SUBSYSTEM

	/* Linux targets: No any hardware IRQ control */

	typedef uint_fast32_t IRQL_t;

	#define IRQL_USER 				0
	#define IRQL_IPC 				0
	#define IRQL_SYSTEM 			0
	#define IRQL_BOARD	 			0
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
	extern uint32_t gARM_BOARD_PRIORITY;
	extern uint32_t gARM_BASEPRI_ONLY_REALTIME;
	extern uint32_t gARM_BASEPRI_ONLY_OVERREALTIME;
	extern uint32_t gARM_BASEPRI_ALL_ENABLED;
	// See usage of functions NVIC_PriorityGroupConfig and NVIC_SetPriorityGrouping
	// A lower priority value indicating a higher priority of running handler
	#define ARM_OVERREALTIME_PRIORITY	((const uint32_t) gARM_OVERREALTIME_PRIORITY)
	#define ARM_REALTIME_PRIORITY	((const uint32_t) gARM_REALTIME_PRIORITY)
	#define ARM_SYSTEM_PRIORITY	((const uint32_t) gARM_SYSTEM_PRIORITY)

	#define IRQL_USER 0xFF	// TODO: verify value
	#define IRQL_BOARD ((NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 3, 0) << (8 - __NVIC_PRIO_BITS)) & 0xff)	// value for __set_BASEPRI
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

#elif (__GIC_PRESENT == 1)

	typedef uint8_t IRQL_t;

	enum
	{
		PRIOv_IPC = 1,	/* Приоритет SGI прерывания для синхронизации приоритетов GIC на остальных процессорах */
		PRIOv_IPC_ONLY,
		PRIOv_OVRT,
		PRIOv_RT,
		PRIOv_SYS,		/* таймеры, USB */
		PRIOv_BRD,		/* папаметы фильттров, board_set_xxx */
		PRIOv_USER,
		//
		PRIOv_count
	};

	// The processor does not process any exception with a priority value greater than or equal to BASEPRI.

	/*
		GICC_PMR == INTC.ICCPMR

		Provides an interrupt priority filter.
		Only interrupts with higher priority than the value in this
		register are signaled to the processor.

	*/

	#define GIC_BINARY_POINT 2//(GIC_GetBinaryPoint())

	#define GICD_ENCODE_PRIORITY(priov) 	((uint32_t) ((priov) << (GIC_BINARY_POINT + 1)))	// Value for GICDistributor->IPRIORITYR[n]
	#define GICI_ENCODE_IRQL(priov) 		((IRQL_t) ((priov) << (GIC_BINARY_POINT + 1)))	// value for GICInterface->PMR

	// value for GIC_SetInterfacePriorityMask
	#define IRQL_IPC 			(GICI_ENCODE_IRQL(PRIOv_IPC))
	#define IRQL_IPC_ONLY 		(GICI_ENCODE_IRQL(PRIOv_IPC_ONLY))
	#define IRQL_OVERREALTIME 	(GICI_ENCODE_IRQL(PRIOv_OVRT))
	#define IRQL_REALTIME	 	(GICI_ENCODE_IRQL(PRIOv_RT))
	#define IRQL_SYSTEM 		(GICI_ENCODE_IRQL(PRIOv_SYS))
	#define IRQL_BOARD			(GICI_ENCODE_IRQL(PRIOv_BRD))
	#define IRQL_USER 			(GICI_ENCODE_IRQL(PRIOv_USER))

	// A lower priority value indicating a higher priority
	// Value for GICD
	#define ARM_IPC_PRIORITY			(GICD_ENCODE_PRIORITY(PRIOv_IPC))
	#define ARM_OVERREALTIME_PRIORITY	(GICD_ENCODE_PRIORITY(PRIOv_OVRT))	/* валкодер и телеграф */
	#define ARM_REALTIME_PRIORITY		(GICD_ENCODE_PRIORITY(PRIOv_RT))	/* звук */
	#define ARM_SYSTEM_PRIORITY			(GICD_ENCODE_PRIORITY(PRIOv_SYS))	/* таймеры, USB */

	#define BOARD_SGI_IRQ 	SGI1_IRQn		/* Прерывание для синхронизации приоритетов GIC на остальных процессорах  */

	// https://github.com/NienfengYao/armv8-bare-metal/blob/572c6f95880e70aa92fe9fed4b8ad7697082a764/aarch64.c#L63
	#if defined(__aarch64__)
		// MSR DAIFCLR, #IRQ_bit
		#define global_enableIRQ() do { \
				__ASM volatile("MSR DAIFClr, %0\n\t" : : "i" (7)  : "memory"); \
				/*__set_RG32C("DAIFCLR", 0x07); */ /* I bit of DAIF */ \
			} while (0)
		// MSR DAIFSET, #IRQ_bit
		#define global_disableIRQ() do { \
				__ASM volatile("MSR DAIFSet, %0\n\t" : : "i" (7)  : "memory"); \
				/*__set_RG32C("DAIFSET", 0x07); *//* I bit of DAIF */ \
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

	#define ARM_IPC_PRIORITY			5
	#define ARM_OVERREALTIME_PRIORITY	4	/* валкодер и телеграф */
	#define ARM_REALTIME_PRIORITY		3	/* звук */
	#define ARM_SYSTEM_PRIORITY			2	/* таймеры, USB */
	#define ARM_BOARD_PRIORITY			1	/* установка параметров */
	#define ARM_USER_PRIORITY			0	/* Значение, на которое инициализируется PLIC->PLIC_MTH_REG */

	#define global_enableIRQ() do { csr_set_bits_mstatus(MSTATUS_MIE_BIT_MASK); } while (0)
	#define global_disableIRQ() do { csr_clr_bits_mstatus(MSTATUS_MIE_BIT_MASK); } while (0)

	#define IRQL_USER				ARM_USER_PRIORITY
	#define IRQL_SYSTEM 			ARM_SYSTEM_PRIORITY
	#define IRQL_BOARD				ARM_BOARD_PRIORITY
	#define IRQL_REALTIME 			ARM_REALTIME_PRIORITY
	#define IRQL_OVERREALTIME		ARM_OVERREALTIME_PRIORITY

#else /* CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 */

	#warning Unsupported target IRQ control

	typedef uint_fast32_t IRQL_t;

	#define IRQL_SYSTEM 			0
	#define IRQL_REALTIME 			0
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
#define DDS_TYPE_LINUX		10

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
#define CODEC_TYPE_ALSA			47	// Linux ALSA
#define CODEC_TYPE_LINUX		48	// Linux, for CODEC2_TYPE

#define RTC_TYPE_DS1305		50	/* MAXIM DS1305EN RTC clock chip with SPI interface */
#define RTC_TYPE_DS1307		51	/* MAXIM DS1307/DS3231 RTC clock chip with I2C interface */
#define RTC_TYPE_M41T81		52	/* ST M41T81M6/M41T81SM6F RTC clock chip with I2C interface */
#define RTC_TYPE_STM32F4xx	53	/* STM32F4xx internal RTC peripherial */
#define RTC_TYPE_STM32F0xx	54	/* STM32F0xx internal RTC peripherial */
#define RTC_TYPE_STM32L0xx	55	/* STM32L0xx internal RTC peripherial */
#define RTC_TYPE_LINUX		56	/* Linux local time */
#define RTC_TYPE_GPS		57	/* GPS time */

#define TSC_TYPE_STMPE811	60	// Resistive touch screen controller ST STMPE811
#define TSC_TYPE_GT911		61	// Capacitive touch screen controller Goodix GT911
#define TSC_TYPE_S3402		62	// Capacitive touch screen controller S3402 (on panel H497TLB01.4)
#define TSC_TYPE_FT5336 	63	// Capacitive touch screen controller FocalTech FT5336
#define TSC_TYPE_XPT2046 	64	// Resistive touch screen controller SHENZHEN XPTEK TECHNOLOGY CO., LTD http://www.xptek.com.cn
#define TSC_TYPE_ILI2102	65	// Capacitive touch screen controller Ilitek ILI2102
#define TSC_TYPE_AWTPADC	66	// Allwinner F133/t113-s3 resistive touch screen controller
#define TSC_TYPE_EVDEV		67	// Linux input device

// Start of NVRAM definitions section
// NOTE: DO NOT USE any types of FLASH memory chips, only EEPROM or FRAM chips are supported.
// поддерживаются только FRAM или EEPROM. FLASH не поддерживаются.
#define NVRAM_TYPE_LINUX		69	/* SQLite3 database for NVRAM */
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
	#elif defined (NVRAM_TYPE) && (NVRAM_TYPE == NVRAM_TYPE_LINUX)
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

#endif /* LCDMODE_LQ043T3DX02K */

#if LCDMODE_AT070TN90	/* AT070TN90 panel (800*480) - 7" display */
	#define LCDMODE_SPI_RN 1 // эти дисплеи требуют только RESET
	#define DIM_X 800
	#define DIM_Y 480
	#define LCDMODE_COLORED	1

#endif /* LCDMODE_AT070TN90 */

#if LCDMODE_AT070TNA2	/* AT070TNA2 panel (1024*600) - 7" display */
	#define LCDMODE_SPI_RN 1 // эти дисплеи требуют только RESET
	#define DIM_X 1024
	#define DIM_Y 600
	#define LCDMODE_COLORED	1

#endif /* LCDMODE_AT070TNA2 */

#if LCDMODE_LQ123K3LG01	/* LQ123K3LG01 panel (1280*480) - 12.3" display LVDS mode */
	#define DIM_X 1280
	#define DIM_Y 480
	#define LCDMODE_COLORED	1

#endif /* LCDMODE_LQ123K3LG01 */

#if LCDMODE_TV101WXM	/* LQ123K3LG01 panel (1280*480) - 12.3" display LVDS mode */
	#define DIM_X 800
	#define DIM_Y 1280
	#define LCDMODE_COLORED	1

#endif /* LCDMODE_TV101WXM */

#if LCDMODE_HSD100IF3	/* 1024 x 600 display LVDS mode */
	#define DIM_X 1024
	#define DIM_Y 600
	#define LCDMODE_COLORED	1

	#define LCD_LVDS_IF_REG_VALUE ( \
		1 * (UINT32_C(1) << 31) | \
		0 * (UINT32_C(1) << 30) | \
		1 * (UINT32_C(1) << 27) | \
		0 * (UINT32_C(1) << 26) | \
		1 * (UINT32_C(1) << 20) | \
		0 * (UINT32_C(1) << 25) |  \
		0 * (UINT32_C(1) << 24) | \
		0 * (UINT32_C(1) << 4) | \
		0 * (UINT32_C(0x0F) << 0) |  \
		0
		)

#endif /* LCDMODE_HSD100IF3 */

#if LCDMODE_TCG104XGLPAPNN	/* TCG104XGLPAPNN-AN30 panel (1024*768) - 10.4" display - DE mode required */
	#define LCDMODE_SPI_RN 1 // эти дисплеи требуют только RESET
	#define DIM_X 1024
	#define DIM_Y 768
	#define LCDMODE_COLORED	1

#endif /* LCDMODE_TCG104XGLPAPNN */

#if LCDMODE_H497TLB01P4	/* 720xRGBx1280 - 5" AMOELD Panel H497TLB01.4 */
	#define DIM_X 720
	#define DIM_Y 1280
	#define LCDMODE_COLORED	1

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

#elif (DIM_X == 800 && DIM_Y == 480) || (DIM_X == 1366 && DIM_Y == 768)
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

#elif DIM_X == 1920 && DIM_Y == 1080
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

	#define DISPLAY_FPS	10	/* обновление показаний частоты за секунду */
	#define DISPLAYSWR_FPS 5	/* количество обновлений SWR за секунду */
#else
	#error Unrecognized dislay layout used (LCDMODE_XXXX)
#endif

#define X2GRID(pix) ((pix) / GRID2X(1))	// переход от пикселей к ячейкам разметки экрана
#define Y2GRID(pix) ((pix) / GRID2Y(1))	// переход от пикселей к ячейкам разметки экрана


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

#define HARDWARE_DELAY_MS(t) do { local_delay_ms(t); } while (0)	//HAL_Delay(t)
#define HARDWARE_DELAY_US(t) do { local_delay_us(t); } while (0)
#define HARDWARE_GETTICK_MS() ((uint32_t) 0) // HAL_GetTick()

#define WITHSPECTRUMWF (WITHIF4DSP && (WITHRTS96 || WITHRTS192) && ! LCDMODE_DUMMY)

/* Сброс мощности при запросе TUNE от автотюнера или извне */
#define WITHLOWPOWEREXTTUNE	(defined (HARDWARE_GET_TUNE) || WITHAUTOTUNER)

/* Зависимости для поределения конфигурации видеосистемы */

#if LCDMODE_PALETTE256_WITH_SHADES
	/* Экран с двумя видеобуферами L8 */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	#define LCDMODE_MAIN_L8	1
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

#elif LCDMODE_PALETTE256
	/* Экран с двумя видеобуферами L8 */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	#define LCDMODE_MAIN_L8	1
	#define LCDMODE_PIXELSIZE 1

	// 0..COLORPIP_BASE-1 - волопад
	// COLORPIP_BASE..127 - надписи и элементы дизайна
	// то же с кодом больше на 128 - затененные цвета для получения полупрозрачности
	// 0..95 - палитра водопада
	// 96..111 - норм цвета
	// 112..127 - первая степень AA
	// Заполнение палитры производится в display2_xltrgb24()

	//#define COLORPIP_SHADED 128
	#define COLORPIP_ALIASED 16
	#define COLORPIP_BASE 96	// should be match to PALETTESIZE

	#define LCDMODE_MAIN_L8		1	/* используется 8 бит на пиксель представление экрана. Иначе - 16 бит - RGB565. */

#elif LCDMODE_RGB565
	/* Экран 16 бит двумя видеобуферами */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	#define LCDMODE_PIXELSIZE 2

#elif LCDMODE_ARGB8888
	/* Экран 32 бит ARGB8888 */
	#define LCDMODE_LTDC	1		/* Use framebuffer-based LCD-TFT Controller (LTDC) */
	#define LCDMODE_PIXELSIZE 4

#elif LCDMODE_DUMMY

	#define LCDMODE_MAIN_L8		1	/* используется 8 бит на пиксель представление экрана. Иначе - 16 бит - RGB565. */
	#define LCDMODE_PIXELSIZE 1

#endif

#if WIHSPIDFSW && WIHSPIDFHW
	#error WIHSPIDFSW and WIHSPIDFHW can not be used in same time
#endif /* WIHSPIDFSW && WIHSPIDFHW */

#define WITHNOTXDACCONTROL	1	/* в этой версии нет ЦАП управления смещением TXDAC передатчика */
//#define WITHPRERENDER (1 && WITHLTDCHW && (LCDMODE_RGB565 || LCDMODE_ARGB8888))		/* использование предварительно построенных изображений при отображении частоты */

#if WITHTOUCHGUI

#if ! defined TSC1_TYPE
	#warning WITHTOUCHGUI without TSC1_TYPE can not compile
	#undef WITHTOUCHGUI									// Компиляция GUI без тачскрина бессмысленна
#endif /* TSC1_TYPE */

#if (DIM_X < 800 || DIM_Y < 480)						// не соблюдены требования к разрешению экрана
	#warning WITHTOUCHGUI and (DIM_X < 800 || DIM_Y < 480)
	#undef WITHTOUCHGUI									// для функционирования touch GUI
#endif

#if ! defined (__CORTEX_M)
	#define FORMATFROMLIBRARY 		1
#endif

#if ! WITHMENU
	#error WITHMENU must be defined for WITHTOUCHGUI
#endif

#if ! defined WITHGUIHEAP || WITHGUIHEAP < (80 * 1024uL)
	#undef WITHGUIHEAP
	#define WITHGUIHEAP 		(80 * 1024uL)			// требуемый размер кучи для touch GUI
#endif /* ! defined WITHGUIHEAP || WITHGUIHEAP < (80 * 1024uL) */

#if ! defined WITHALTERNATIVELAYOUT || ! WITHALTERNATIVELAYOUT
	#undef WITHALTERNATIVELAYOUT
	#define WITHALTERNATIVELAYOUT	1
#endif

#endif /* WITHTOUCHGUI */

#if WITHKEEPNVRAM && defined (NVRAM_TYPE) && (NVRAM_TYPE == NVRAM_TYPE_FM25XXXX)
	#error WITHKEEPNVRAM and NVRAM_TYPE_FM25XXXX can not be used together
#endif

// Поддержка FatFS если запрошена поддержка однорго из носителей
#define WITHUSEFATFS (WITHUSESDCARD || WITHUSEUSBFLASH || WITHUSERAMDISK)

#if (WIHSPIDFSW || WIHSPIDFHW) && WIHSPIDFOVERSPI
	#error WIHSPIDFOVERSPI and ( WIHSPIDFSW or WIHSPIDFHW ) can not be used together
#endif /* (WIHSPIDFSW || WIHSPIDFHW) && WIHSPIDFOVERSPI */

#if WITHIF4DSP
	#define BOARDPOWERMIN	0	// Нижний предел регулировки (показываемый на дисплее)
	#define BOARDPOWERMAX	100	// Верхний предел регулировки (показываемый на дисплее)

	#define BOARDDACSCALEMIN	0	// Нижний предел мощности (аргумент board_set_dacscale() */
	#define BOARDDACSCALEMAX	10000	// Верхний предел мощности (аргумент board_set_dacscale() */
#endif /* WITHIF4DSP */

#if defined (RTC1_TYPE) && (RTC1_TYPE == RTC_TYPE_GPS) && ! defined WITHNMEA
	#error RTC_TYPE_GPS and WITHNMEA must be used in same time
#endif /* defined (RTC1_TYPE) && (RTC1_TYPE == RTC_TYPE_GPS) && ! defined WITHNMEA */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* PARAMDEPEND_H_INCLUDED */
