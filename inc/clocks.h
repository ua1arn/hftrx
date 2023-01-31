/* $Id: $ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//


#ifndef INC_CLOCKS_H_
#define INC_CLOCKS_H_

uint_fast32_t stm32f4xx_get_hse_freq(void);
uint_fast32_t stm32f4xx_get_hsi_freq(void);
uint_fast32_t stm32f4xx_get_pllreference_freq(void);
uint_fast32_t stm32f4xx_get_pll_freq(void);
uint_fast32_t stm32f4xx_get_pll_p_freq(void);
uint_fast32_t stm32f4xx_get_pll_q_freq(void);
uint_fast32_t stm32f4xx_get_plli2s_freq(void);
uint_fast32_t stm32f4xx_get_plli2s_q_freq(void);
uint_fast32_t stm32f4xx_get_plli2s_r_freq(void);
uint_fast32_t stm32f4xx_get_pllsai_freq(void);
uint_fast32_t stm32f4xx_get_pllsai_q_freq(void);
uint_fast32_t stm32f4xx_get_pllsai_r_freq(void);
uint_fast32_t stm32f4xx_get_sysclk_freq(void);
uint_fast32_t stm32f4xx_get_ahb_freq(void);
uint_fast32_t stm32f4xx_get_hclk_freq(void);
uint_fast32_t stm32f4xx_get_apb2_freq(void);
uint_fast32_t stm32f4xx_get_apb1_freq(void);
uint_fast32_t stm32f4xx_get_apb2timer_freq(void);
uint_fast32_t stm32f4xx_get_apb1timer_freq(void);
uint_fast32_t stm32f4xx_get_spi1_freq(void);
uint_fast32_t stm32f4xx_get_tim3_freq(void);

uint_fast32_t stm32f7xx_get_hse_freq(void);
uint_fast32_t stm32f7xx_get_pll_freq(void);
uint_fast32_t stm32f7xx_get_pllsai_freq(void);
uint_fast32_t stm32f7xx_get_plli2s_freq(void);
uint_fast32_t stm32f7xx_get_pll_p_freq(void);
uint_fast32_t stm32f7xx_get_sys_freq(void);
uint_fast32_t stm32f7xx_get_ahb_freq(void);
uint_fast32_t stm32f7xx_get_pclk1_freq(void);
uint_fast32_t stm32f7xx_get_pclk2_freq(void);
uint_fast32_t stm32f7xx_get_usart1_freq(void);
uint_fast32_t stm32f7xx_get_usart2_freq(void);
uint_fast32_t stm32f7xx_get_usart3_freq(void);
uint_fast32_t stm32f7xx_get_uart4_freq(void);
uint_fast32_t stm32f7xx_get_uart5_freq(void);
uint_fast32_t stm32f7xx_get_usart6_freq(void);
uint_fast32_t stm32f7xx_get_uart7_freq(void);
uint_fast32_t stm32f7xx_get_uart8_freq(void);
uint_fast32_t stm32f7xx_get_apb1_freq(void);

uint_fast32_t stm32h7xx_get_hse_freq(void);
uint_fast32_t stm32h7xx_get_pll1_freq(void);
uint_fast32_t stm32h7xx_get_pll1_p_freq(void);
uint_fast32_t stm32h7xx_get_pll1_q_freq(void);
uint_fast32_t stm32h7xx_get_pll2_freq(void);
uint_fast32_t stm32h7xx_get_pll2_p_freq(void);
uint_fast32_t stm32h7xx_get_pll2_q_freq(void);
uint_fast32_t stm32h7xx_get_pll3_freq(void);
uint_fast32_t stm32h7xx_get_pll3_p_freq(void);
uint_fast32_t stm32h7xx_get_pll3_q_freq(void);
uint_fast32_t stm32h7xx_get_pll3_r_freq(void);
uint_fast32_t stm32h7xx_get_sys_freq(void);
uint_fast32_t stm32h7xx_get_stm32h7xx_get_d1cpre_freq(void);
uint_fast32_t stm32h7xx_get_hclk3_freq(void);
uint_fast32_t stm32h7xx_get_aclk_freq(void);
uint_fast32_t stm32h7xx_get_hclk1_freq(void);
uint_fast32_t stm32h7xx_get_hclk2_freq(void);
uint_fast32_t stm32h7xx_get_apb1_freq(void);
uint_fast32_t stm32h7xx_get_apb2_freq(void);
uint_fast32_t stm32h7xx_get_timx_freq(void);
uint_fast32_t stm32h7xx_get_pclk1_freq(void);
uint_fast32_t stm32h7xx_get_timy_freq(void);
uint_fast32_t stm32h7xx_get_pclk2_freq(void);
uint_fast32_t stm32h7xx_get_per_freq(void);
uint_fast32_t stm32h7xx_get_pclk3_freq(void);
uint_fast32_t stm32h7xx_get_usart1_6_freq(void);
uint_fast32_t stm32h7xx_get_usart2_to_8_freq(void);
uint_fast32_t stm32h7xx_get_spi1_2_3_freq(void);
uint_fast32_t stm32h7xx_get_spi4_5_freq(void);
uint_fast32_t stm32h7xx_get_adc_freq(void);
uint_fast32_t stm32f7xx_pllq_initialize(void); // Настроить выход PLLQ на 48 МГц

void stm32mp1_pll_initialize(void);
void stm32mp1_usb_clocks_initialize(void);
void stm32mp1_audio_clocks_initialize(void);
uint_fast32_t stm32mp1_get_hse_freq(void);
uint_fast32_t stm32mp1_get_hsi_freq(void);
uint_fast32_t stm32mp1_get_pll1_2_ref_freq(void);
uint_fast32_t stm32mp1_get_pll3_ref_freq(void);
uint_fast32_t stm32mp1_get_pll4_ref_freq(void);
uint_fast32_t stm32mp1_get_pll1_freq(void);
uint_fast32_t stm32mp1_get_pll1_p_freq(void);
uint_fast32_t stm32mp1_get_mpuss_freq(void);
uint_fast32_t stm32mp1_get_pll2_freq(void);
uint_fast32_t stm32mp1_get_pll2_p_freq(void);
uint_fast32_t stm32mp1_get_pll2_r_freq(void);
uint_fast32_t stm32mp1_get_pll3_freq(void);
uint_fast32_t stm32mp1_get_pll3_p_freq(void);
uint_fast32_t stm32mp1_get_pll3_q_freq(void);
uint_fast32_t stm32mp1_get_pll3_r_freq(void);
uint_fast32_t stm32mp1_get_pll4_freq(void);
uint_fast32_t stm32mp1_get_pll4_q_freq(void);
uint_fast32_t stm32mp1_get_pll4_p_freq(void);
uint_fast32_t stm32mp1_get_pll4_r_freq(void);
uint_fast32_t stm32mp1_get_eth_freq(void);
uint_fast32_t stm32mp1_get_ethptp_freq(void);
uint_fast32_t stm32mp1_get_axiss_freq(void);
uint_fast32_t stm32mp1_get_per_freq(void);
uint_fast32_t stm32mp1_get_mcuss_freq(void);
uint_fast32_t stm32mp1_get_aclk_freq(void);
uint_fast32_t stm32mp1_get_hclk5_freq(void);
uint_fast32_t stm32mp1_get_hclk6_freq(void);
uint_fast32_t stm32mp1_get_mlhclk_freq(void);
uint_fast32_t stm32mp1_get_pclk1_freq(void);
uint_fast32_t stm32mp1_get_pclk2_freq(void);
uint_fast32_t stm32mp1_get_pclk3_freq(void);
uint_fast32_t stm32mp1_get_pclk4_freq(void);
uint_fast32_t stm32mp1_get_pclk5_freq(void);
uint_fast32_t stm32mp1_get_usbphy_freq(void);
uint_fast32_t stm32mp1_get_usbotg_freq(void);
uint_fast32_t stm32mp1_uart1_get_freq(void);
uint_fast32_t stm32mp1_uart2_4_get_freq(void);
uint_fast32_t stm32mp1_uart3_5_get_freq(void);
uint_fast32_t stm32mp1_uart7_8_get_freq(void);
uint_fast32_t stm32mp1_sdmmc1_2_get_freq(void);
uint_fast32_t stm32mp1_get_timg1_freq(void);
uint_fast32_t stm32mp1_get_timg2_freq(void);
uint_fast32_t stm32mp1_get_spi1_freq(void);
uint_fast32_t stm32mp1_get_adc_freq(void);
uint_fast32_t stm32mp1_get_qspi_freq(void);

//unsigned long xc7z_get_pllsreference_freq(void);
//uint_fast64_t xc7z_get_arm_pll_freq(void);
//uint_fast64_t xc7z_get_ddr_pll_freq(void);
uint_fast64_t xc7z_get_io_pll_freq(void);
unsigned long xc7z_get_arm_freq(void);
unsigned long xc7z_get_ddr_x2clk_freq(void);
unsigned long xc7z_get_ddr_x3clk_freq(void);
unsigned long xc7z_get_uart_freq(void);
unsigned long xc7z_get_sdio_freq(void);
unsigned long xc7z_get_spi_freq(void);
unsigned long xc7z_get_qspi_freq(void);

// platform-independed functions.
unsigned long hardware_get_spi_freq(void);	// получение тактовой частоты тактирования блока SPI, использующенося в данной конфигурации
unsigned long hardware_get_apb1_freq(void);
unsigned long hardware_get_apb1_tim_freq(void);
unsigned long hardware_get_apb2_freq(void);
unsigned long hardware_get_apb2_tim_freq(void);

// Allwinner t113-s3
uint_fast32_t allwnrt113_get_hosc_freq(void);
uint_fast32_t allwnrt113_get_usart_freq(void);
uint_fast32_t allwnrt113_get_spi0_freq(void);
uint_fast32_t allwnrt113_get_spi1_freq(void);
uint_fast32_t allwnrt113_get_arm_freq(void);
uint_fast32_t allwnrf133_get_riscv_freq(void);
uint_fast32_t allwnrt113_get_pl1_timer_freq(void);
uint_fast32_t allwnrt113_get_video0pllx4_freq(void);
uint_fast32_t allwnrt113_get_video0_x2_freq(void);
uint_fast32_t allwnrt113_get_video0_x1_freq(void);
uint_fast32_t allwnrt113_get_video1pllx4_freq(void);
uint_fast32_t allwnrt113_get_video1_x2_freq(void);
uint_fast32_t allwnrt113_get_video1_x1_freq(void);
uint_fast32_t allwnrt113_get_vepll_freq(void);
uint_fast32_t allwnrt113_get_audio0pll4x_freq(void);
uint_fast32_t allwnrt113_get_audio0pll1x_freq(void);
uint_fast32_t allwnrt113_get_audio1pll1x_freq(void);
uint_fast32_t allwnrt113_get_audio1pll_div2_freq(void);
uint_fast32_t allwnrt113_get_audio1pll_div5_freq(void);
uint_fast64_t allwnrt113_get_pll_cpu_freq(void);
uint_fast64_t allwnrt113_get_pll_ddr_freq(void);
uint_fast32_t allwnrt113_get_dram_freq(void);
uint_fast64_t allwnrt113_get_pll_peri_freq(void);
uint_fast32_t allwnrt113_get_peripll2x_freq(void);
uint_fast32_t allwnrt113_get_pll_peri_800M_freq(void);
uint_fast32_t allwnrt113_get_i2s1_freq(void);
uint_fast32_t allwnrt113_get_i2s2_freq(void);
uint_fast32_t allwnrt113_get_g2d_freq(void);
uint_fast32_t allwnrt113_get_de_freq(void);
uint_fast32_t allwnrt113_get_di_freq(void);
uint_fast32_t allwnrt113_get_ce_freq(void);
uint_fast32_t allwnrt113_get_ve_freq(void);
uint_fast32_t allwnrt113_get_dsp_freq(void);

/* PLL initialization functions. Take care on glitches. */
void allwnrt113_set_pll_cpux(unsigned m, unsigned n);
void allwnrt113_set_pll_ddr(unsigned m, unsigned n);
void allwnrt113_set_pll_peri(unsigned m, unsigned n);
void allwnrt113_set_pll_vieo0(unsigned m, unsigned n);
void allwnrt113_set_pll_vieo1(unsigned m, unsigned n);
void allwnrt113_set_pll_ve(unsigned m, unsigned n);
void allwnrt113_set_pll_audio0(unsigned m, unsigned n);
void allwnrt113_set_pll_audio1(unsigned m, unsigned n);

void allwnrt113_pll_initialize(void);
void set_pll_cpux_axi(unsigned n);
void set_pll_riscv_axi(unsigned n);

uint_fast8_t
calcdivider(
	uint_fast32_t divisor, // ожидаемый коэффициент деления всей системы
	uint_fast8_t width,			// количество разрядов в счётчике
	uint_fast16_t taps,			// маска битов - выходов прескалера. 0x01 - означает bypass, 0x02 - делитель на 2... 0x400 - делитель на 1024
	unsigned * dvalue,		// Значение для записи в регистр сравнения делителя
	uint_fast8_t substract);


#if CPUSTYLE_AT91SAM7S

	// Параметры функции calcdivider().
	enum
	{
		AT91SAM7_TIMER_WIDTH = 16,	AT91SAM7_TIMER_TAPS = (1024 | 128 | 32 | 8 | 2),
		AT91SAM7_UART_BRGR_WIDTH = 16,	AT91SAM7_UART_BRGR_TAPS = (16),	// Регистр UART_BRGR не требует уменьшения на 1
		AT91SAM7_USART_BRGR_WIDTH = 16,	AT91SAM7_USART_BRGR_TAPS = (16 | 8),	// Регистр US_BRGR не требует уменьшения на 1
		AT91SAM7_PITPIV_WIDTH = 20,	AT91SAM7_PITPIV_TAPS = (16),	// Periodic Interval Timer (PIT)
		AT91SAM7_TWI_WIDTH = 8,	AT91SAM7_TWI_TAPS = (255),	// I2C interface (TWI) используется MCLK / 2
		AT91SAM7_ADC_PRESCAL_WIDTH = 6, AT91SAM7_ADC_PRESCAL_TAPS = 2,	// используется MCLK / 2
		//
		AT91SAM7_fillPAD
	};

	// Mask: AT91C_TC_CLKS
	static const unsigned long tc_cmr_clks [] =
	{
		AT91C_TC_CLKS_TIMER_DIV1_CLOCK, // is a TCxCLK = MCLK / 2
		AT91C_TC_CLKS_TIMER_DIV2_CLOCK, // is a TCxCLK = MCLK / 8
		AT91C_TC_CLKS_TIMER_DIV3_CLOCK, // is a TCxCLK = MCLK / 32
		AT91C_TC_CLKS_TIMER_DIV4_CLOCK, // is a TCxCLK = MCLK / 128
		AT91C_TC_CLKS_TIMER_DIV5_CLOCK, // is a TCxCLK = MCLK / 1024
	};

#elif CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

	// Параметры функции calcdivider().
	//
	// В stm32 три вида таймеров - General-purpose (2..5, 9..14), Advanced-control (1 & 8) & Basic (6 & 7)
	enum
	{
		//STM32F_GP_TIMER_WIDTH = 16,	STM32F_GP_TIMER_TAPS = (65535), // General-purpose timers
		// 32-bit timer: TIM2 on STM32F3xxx, TIM2 and TIM5 on STM32F4xxx
		//STM32F_AC_TIMER_WIDTH = 16,	STM32F_AC_TIMER_TAPS = (65535), // Advanced-control timers
		//STM32F_BA_TIMER_WIDTH = 16,	STM32F_BA_TIMER_TAPS = (65535), // Basic timers

		// General-purpose timers (TIM2/TIM3/TIM4/TIM5)
#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
		STM32F_TIM2_TIMER_WIDTH = 32,	STM32F_TIM2_TIMER_TAPS = (65535), // General-purpose timers TIM2 and TIM5 on CPUSTYLE_STM32F4XX
		STM32F_TIM5_TIMER_WIDTH = 32,	STM32F_TIM5_TIMER_TAPS = (65535), // General-purpose timers TIM2 and TIM5 on CPUSTYLE_STM32F4XX
#else /* CPUSTYLE_STM32F4XX */
		STM32F_TIM2_TIMER_WIDTH = 16,	STM32F_TIM2_TIMER_TAPS = (65535), // General-purpose timers TIM2 and TIM5 on CPUSTYLE_STM32F4XX
		STM32F_TIM5_TIMER_WIDTH = 16,	STM32F_TIM5_TIMER_TAPS = (65535), // General-purpose timers TIM2 and TIM5 on CPUSTYLE_STM32F4XX
#endif /* CPUSTYLE_STM32F4XX */
		STM32F_TIM1_TIMER_WIDTH = 16,	STM32F_TIM1_TIMER_TAPS = (65535),
		STM32F_TIM3_TIMER_WIDTH = 16,	STM32F_TIM3_TIMER_TAPS = (65535),
		STM32F_TIM4_TIMER_WIDTH = 16,	STM32F_TIM4_TIMER_TAPS = (65535),

		STM32F_SPIBR_WIDTH = 0,	STM32F_SPIBR_TAPS = (256 | 128 | 64 | 32 | 16 | 8 | 4 | 2),

		// LTDC dot clock parameters
		STM32F_LTDC_DIV_WIDTH = 3, // valid values for RCC_PLLSAICFGR_PLLSAIR: 2..7
			STM32F_LTDC_DIV_TAPS = (16 | 8 | 4 | 2),	// valid values for RCC_DCKCFGR_PLLSAIDIVR: 0: /2, 1: /4, 2: /8, 3: /16

		//STM32F103_BRGR_WIDTH = 16,	STM32F103_BRGR_TAPS = (16 | 8),	// Регистр US_BRGR не требует уменьшения на 1
		//STM32F103_PITPIV_WIDTH = 20,	STM32F103_PITPIV_TAPS = (16),	// Periodic Interval Timer (PIT)
		//STM32F103_TWI_WIDTH = 8,	STM32F103_TWI_TAPS = (255),	// I2C interface (TWI)
		//STM32F103_ADC_PRESCAL_WIDTH = 8, STM32F103_ADC_PRESCAL_TAPS = 2,	// используется MCLK / 2
		//
		STM32F_fillPAD
	};

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	// Параметры функции calcdivider().
	enum
	{
		ATSAM3S_TIMER_WIDTH = 16,	ATSAM3S_TIMER_TAPS = (1024 | 128 | 32 | 8 | 2),
		ATSAM3S_UART_BRGR_WIDTH = 16,	ATSAM3S_UART_BRGR_TAPS = (16),	// Регистр US_BRGR не требует уменьшения на 1
		ATSAM3S_USART_BRGR_WIDTH = 16,	ATSAM3S_USART_BRGR_TAPS = (16 | 8),	// Регистр US_BRGR не требует уменьшения на 1
		ATSAM3S_PITPIV_WIDTH = 20,	ATSAM3S_PITPIV_TAPS = (16),	// Periodic Interval Timer (PIT)
		ATSAM3S_TWI_WIDTH = 8,	ATSAM3S_TWI_TAPS = (255),	// I2C interface (TWI)
		ATSAM3S_ADC_PRESCAL_WIDTH = 8, ATSAM3S_ADC_PRESCAL_TAPS = 2,	// используется MCLK / 2
		//
		ATSAM3S_fillPAD
	};

	// Mask: AT91C_TC_CLKS
	static const unsigned long tc_cmr_tcclks [] =
	{
		TC_CMR_TCCLKS_TIMER_CLOCK1, // is a TCxCLK = MCLK / 2
		TC_CMR_TCCLKS_TIMER_CLOCK2, // is a TCxCLK = MCLK / 8
		TC_CMR_TCCLKS_TIMER_CLOCK3, // is a TCxCLK = MCLK / 32
		TC_CMR_TCCLKS_TIMER_CLOCK4, // is a TCxCLK = MCLK / 128
		TC_CMR_TCCLKS_TIMER_CLOCK5, // is a TCxCLK = MCLK / 1024
	};

#elif CPUSTYLE_ATMEGA

	// Параметры функции calcdivider().
	enum
	{
		ATMEGA_SPCR_WIDTH = 0,			ATMEGA_SPCR_TAPS = (128 | 64 | 32 | 16 | 8 | 4 | 2),

		ATMEGA128_TIMER0_WIDTH = 8,		ATMEGA128_TIMER0_TAPS = (1024 | 256 | 128 | 64 | 32 | 8 | 1),
		ATMEGA_TIMER0_WIDTH = 8,		ATMEGA_TIMER0_TAPS = (1024 | 256 | 64 | 8 | 1),
		ATMEGA_TIMER1_WIDTH = 16,		ATMEGA_TIMER1_TAPS = (1024 | 256 | 64 | 8 | 1),
		ATMEGA_TIMER2_WIDTH = 8,		ATMEGA_TIMER2_TAPS = (1024 | 256 | 128 | 64 | 32 | 8 | 1),
		ATMEGAXXX4_TIMER3_WIDTH = 16,	ATMEGAXXX4_TIMER3_TAPS = (1024 | 256 | 64 | 8 | 1),	/* avaliable only on ATMega1284P */

		ATMEGA_UBR_WIDTH = 12,	ATMEGA_UBR_TAPS = (16 | 8),	/* UBR: USART Baud Rate Register */
		ATMEGA_TWBR_WIDTH = 8,	ATMEGA_TWBR_TAPS = (64 | 16 | 4 | 1),	/* TWBR: TWI Bit Rate Register */
		ATMEGA_ADPS_WIDTH = 0,	ATMEGA_ADPS_TAPS = (128 | 64 | 32 | 16 | 8 | 4 | 2),	/* ADPS2:0: ADC Prescaler Select Bits */

		ATMEGA8_TIMER2_WIDTH = 8,		ATMEGA8_TIMER2_TAPS = (1024 | 256 | 128 | 64 | 32 | 8 | 1),
		//
		ATMEGA8_fillPAD

	};
	// spcr и spsr - скорость SPI. Индексы соответствуют номерам битов в ATMEGA_SPCR_TAPS
	// Document: 8272E-AVR-04/2013, Table 18-5. Relationship between SCK and the oscillator frequency.
	static const FLASHMEM struct spcr_spsr_tag { uint_fast8_t spsr, spcr; } spcr_spsr [] =
	{
		{ (1U << SPI2X), 	(0U << SPR1) | (0U << SPR0), }, 	/* /2 */
		{ (0U << SPI2X), 	(0U << SPR1) | (0U << SPR0), }, 	/* /4 */
		{ (1U << SPI2X), 	(0U << SPR1) | (1U << SPR0), }, 	/* /8 */
		{ (0U << SPI2X), 	(0U << SPR1) | (1U << SPR0), }, 	/* /16 */
		{ (1U << SPI2X), 	(1U << SPR1) | (0U << SPR0), }, 	/* /32 */
		{ (0U << SPI2X), 	(1U << SPR1) | (0U << SPR0), }, 	/* /64 */
		{ (0U << SPI2X), 	(1U << SPR1) | (1U << SPR0), }, 	/* /128 */
	};

#elif CPUSTYLE_ATXMEGA
	// Параметры функции calcdivider().
	enum
	{
		ATXMEGA_TIMER_WIDTH = 16,	ATXMEGA_TIMER_TAPS = (1024 | 256 | 64 | 8 | 4 | 2 | 1),
		ATXMEGA_SPIBR_WIDTH = 0,	ATXMEGA_SPIBR_TAPS = (128 | 64 | 32 | 16 | 8 | 4 | 2),
		ATXMEGA_UBR_WIDTH = 12,
			ATXMEGA_UBR_GRADE = 3, // Допустимы значения 0 (без дробного делителя), 1, 2 и 3 (с периодом 8)
			ATXMEGA_UBR_TAPS = (16 | 8) >> ATXMEGA_UBR_GRADE, ATXMEGA_UBR_BSEL = (0 - ATXMEGA_UBR_GRADE),
		//
		ATXMEGA_fillPAD
	};
	static const FLASHMEM uint_fast8_t spi_ctl [] =
	{
		(1U << SPI_CLK2X_bp) | 	SPI_PRESCALER_DIV4_gc,  /* /2 */
		(0U << SPI_CLK2X_bp) | 	SPI_PRESCALER_DIV4_gc,  /* /4 */
		(1U << SPI_CLK2X_bp) |	SPI_PRESCALER_DIV16_gc, 	/* /8 */
		(0U << SPI_CLK2X_bp) |	SPI_PRESCALER_DIV16_gc, 	/* /16 */
		(1U << SPI_CLK2X_bp) |	SPI_PRESCALER_DIV64_gc, 	/* /32 */
		(0U << SPI_CLK2X_bp) |	SPI_PRESCALER_DIV64_gc, 	/* /64 */
		(0U << SPI_CLK2X_bp) |	SPI_PRESCALER_DIV128_gc,	/* /128 */
	};
#elif CPUSTYLE_STM32F30X
	#warning TODO: Add code for STM32F30X timers support

#elif CPUSTYLE_R7S721

	enum
	{
		R7S721_SCIF_SCBRR_WIDTH = 8,	R7S721_SCIF_SCBRR_TAPS = (2048 | 1024 | 512 | 256 | 128 | 64 | 32 | 16 ),	// Регистр SCIFx.SCBRR требует уменьшение на 1
		R7S721_RSPI_SPBR_WIDTH = 8,		R7S721_RSPI_SPBR_TAPS = (16 | 8 | 4 | 2),
	};

	uint_fast32_t
	calcdivround_p1clock(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		);

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	enum
	{
		XC7Z_FPGAx_CLK_WIDTH = 6,	XC7Z_FPGAx_CLK_TAPS = (32 | 16 | 8 | 4 | 2 | 1),	// FPGA0_CLK_CTRL
		XC7Z_SPI_BR_WIDTH = 0, XC7Z_SPI_BR_TAPS = (256 | 128 | 64 | 32 | 16 | 8 | 4)
	};

#elif CPUSTYLE_XCZU

	enum
	{
		XC7Z_FPGAx_CLK_WIDTH = 6,	XC7Z_FPGAx_CLK_TAPS = (32 | 16 | 8 | 4 | 2 | 1)	// FPGA0_CLK_CTRL
	};

#elif CPUSTYLE_T113 || CPUSTYLE_F133
	enum
	{
		ALLWNR_TIMER_WIDTH = 32, ALLWNR_TIMER_TAPS = (128 | 64 | 32 | 16 | 8 | 4 | 2 | 1),
		ALLWNR_PWM_WIDTH = 8, ALLWNR_PWM_TAPS = (512 | 256 | 128 | 64 | 32 | 16 | 8 | 4 | 2),
		ALLWNT113_I2Sx_CLK_WIDTH = 5, ALLWNT113_I2Sx_CLK_TAPS = ( 8 | 4 | 2 | 1 ),
		ALLWNT113_AudioCodec_CLK_WIDTH = 5, ALLWNT113_AudioCodec_CLK_TAPS = ( 8 | 4 | 2 | 1 ),
		//
		ALLWNT113_pad
	};

#else
	//#warning Undefined CPUSTYLE_XXX
#endif

#endif /* INC_CLOCKS_H_ */
