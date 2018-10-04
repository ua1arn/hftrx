/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "keyboard.h"

#include <string.h>
#include <math.h>

#include "board.h"
#include "spifuncs.h"
#include "audio.h"
#include "pio.h"

#include "formats.h"	// for debug prints


static unsigned long ulmin(
	unsigned long a,
	unsigned long b)
{
	return a < b ? a : b;
}

static unsigned long ulmax(
	unsigned long a,
	unsigned long b)
{
	return a > b ? a : b;
}

// ATMega32 timers:
// 8 bit timer0 - system ticks
// 16 bit timer1 - прерывания с периодом 1/ELKEY_DISCRETE от длительности точки
// 8 bit timer2 - beep (CW sidetone) - генерация сигнала самоконтроля на PD7(OC2)

// ATMega644 timers:
// 8 bit timer0 - system ticks
// 16 bit timer1 - прерывания с периодом 1/ELKEY_DISCRETE от длительности точки
// 8 bit timer2 - beep (CW sidetone) - генерация сигнала самоконтроля на PD7(OC2)
// 16 bit timer3 - UNUSED (avaliable only on ATMega1284P)

// ATMega328 timers:
// 8 bit timer0 - beep (CW sidetone) - генерация сигнала самоконтроля на PD6(OC0A)
// 16 bit timer1 - прерывания с периодом 1/ELKEY_DISCRETE от длительности точки
// 8 bit timer2 - system ticks

// ATXMega timers:
// TCC1: 1/ELKEY_DISCRETE dot length timer 
// TCD1: beep (CW sidetone) - генерация сигнала самоконтроля
// TCC0: system ticks

// AT91SAM7Sxxx timers
// TC0: LFM timer 
// TC1: beep (CW sidetone) - генерация сигнала самоконтроля
// TC2: 1/ELKEY_DISCRETE dot length timer

// ATSAMSSxxx timers
// TC0: LFM timer 
// TC1: beep (CW sidetone) - генерация сигнала самоконтроля
// TC2: 1/ELKEY_DISCRETE dot length timer
// TC3: UNUSED 
// TC4: UNUSED 
// TC5: UNUSED 

// STM32 timers
// TIM3: 1/ELKEY_DISCRETE dot length timer 
// TIM4: beep (CW sidetone) - генерация сигнала самоконтроля

// R7S721xxxx timers
// OSTM1: 1/ELKEY_DISCRETE dot length timer 
// OSTM0: system ticks

static uint_fast32_t 
NOINLINEAT
calcdivround2(
	uint_fast32_t ref,	/* частота на входе делителя, в герцах. */
	uint_fast32_t freq	/* требуемая частота на выходе делителя, в герцах. */
	)
{
	return (ref < freq) ? 1 : ((ref + freq / 2) / freq);
}

#if CPUSTYLE_STM32F
	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора
	static uint_fast32_t 
	NOINLINEAT
	calcdivround_systick(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		return calcdivround2(SYSTICK_FREQ, freq);
	}

	/* для устройств на шине APB1 (up to 36 MHz) */
	static uint_fast32_t 
	NOINLINEAT
	calcdivround_pclk1(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		return calcdivround2(PCLK1_FREQ, freq);
	}

	/* для устройствтактирующихся от HSI48 */
	static uint_fast32_t 
	NOINLINEAT
	calcdivround_hsi48(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		return calcdivround2(48000000uL, freq);
	}

	/* для устройств на шине APB1 (up to 36 MHz) - таймеры */
	static uint_fast32_t 
	NOINLINEAT
	calcdivround_pclk1_timers(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		return calcdivround2(PCLK1_TIMERS_FREQ, freq);
	}

	/* для устройств на шине APB2 (up to 72 MHz) */
	static uint_fast32_t 
	NOINLINEAT
	calcdivround_pclk2(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		return calcdivround2(PCLK2_FREQ, freq);
	}
#if SIDETONE_TARGET_BIT != 0
	/* для устройств на шине APB2 (up to 72 MHz) */
	static uint_fast32_t 
	NOINLINEAT
	calcdivround10_pclk2(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в десятых долях герца. */
		)
	{
		return calcdivround2(10UL * PCLK2_FREQ, freq);
	}
#endif /* SIDETONE_TARGET_BIT != 0 */

#if CPUSTYLE_STM32H7XX
	/* для устройств тактирующихся от per_ck */
	static uint_fast32_t 
	NOINLINEAT
	calcdivround_per_ck(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		return calcdivround2(PER_CK_FREQ, freq);
	}
#endif /* CPUSTYLE_STM32H7XX */

#elif CPUSTYLE_R7S721

	/* для устройств тактирующихся от P0 clock */
	static uint_fast32_t 
	calcdivround_p0clock(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		return calcdivround2(P0CLOCK_FREQ, freq);
	}

	/* для устройств тактирующихся от P1 clock */
	static uint_fast32_t 
	calcdivround_p1clock(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		return calcdivround2(P1CLOCK_FREQ, freq);
	}


#else
	// other CPUs

	static uint_fast32_t 
	NOINLINEAT
	calcdivround(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		return calcdivround2(CPU_FREQ, freq);
	}

	static uint_fast32_t 
	NOINLINEAT
	calcdivround_systick(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		return calcdivround2(CPU_FREQ, freq);
	}

#if SIDETONE_TARGET_BIT != 0
	static uint_fast32_t 
	NOINLINEAT
	calcdivround10(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в десятых долях герца. */
		)
	{
		return calcdivround2(10UL * CPU_FREQ, freq);
	}
#endif /* SIDETONE_TARGET_BIT != 0 */

#endif

 // возврат позиции старшего значащего бита в числе
static uint_fast8_t 
ilog2(
	unsigned long v		// число на анализ
	)
{
	uint_fast8_t n;

	for (n = 0; v != 0; ++ n)
		v >>= 1;

	return n;
}


// Вариант функции для расчёта делителя определяющего скорость передачи
// на UART AT91SAM7S, делитель для АЦП ATMega (значение делителя не требуется уменьшать на 1).
static uint_fast8_t 
//NOINLINEAT
calcdivider(
	uint_fast32_t divider, // ожидаемый коэффициент деления всей системы
	uint_fast8_t width,			// количество разрядов в счётчике
	uint_fast16_t taps,			// маска битов - выходов прескалера. 0x01 - означает bypass, 0x02 - делитель на 2... 0x400 - делитель на 1024
	unsigned * dvalue,		// Значение для записи в регистр сравнения делителя
	uint_fast8_t substract)
{
	const uint_fast8_t rbmax = 16; //позиция старшего значащего бита в маске TAPS
	uint_fast8_t rb, rbi;
	uint_fast16_t prescaler = 1U;

	for (rb = rbi = 0; rb <= rbmax; ++ rb, prescaler *= 2)
	{
		if ((taps & prescaler) != 0)
		{
			// такой предделитель существует.
			const uint_fast32_t modulus = ((divider + prescaler / 2) / prescaler) - substract;
			if (ilog2(modulus) <= width)
			{
				// найдена подходящая комбинация
				// rb - степень двойки - деление предделителя.
				// rbi - номер кода для записи в регистр предделителя.
				// modulus - что загрузить в регистр сравнения делителя.
				* dvalue = (unsigned) modulus;
				return rbi;
			}
			++ rbi;		// переходим к следующему предделителю в списке.
		}
	}
	// Не подобрать комбинацию прескалера и делителя для ожидаемого коэффициента деления.
	* dvalue = (1U << width) - 1;	// просто пустышка - максимальный делитель
	return (rbi - 1);	// если надо обраьатывать невозможность подбора - возврат rbmax
}


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

#elif CPUSTYLE_STM32F

	// Параметры функции calcdivider().
	// 
	// В stm32 три вида таймеров - General-purpose (2..5, 9..14), Advanced-control (1 & 8) & Basic (6 & 7)
	enum 
	{ 
		//STM32F_GP_TIMER_WIDTH = 16,	STM32F_GP_TIMER_TAPS = (65535), // General-purpose timers
		// 32-bit timer: TIM2 on STM32F3xxx, TIM2 and TIM5 on STM32F4xxx
		//STM32F_AC_TIMER_WIDTH = 16,	STM32F_AC_TIMER_TAPS = (65535), // Advanced-control timers
		//STM32F_BA_TIMER_WIDTH = 16,	STM32F_BA_TIMER_TAPS = (65535), // Basic timers

#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32FHXX
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
	// Document: 8272E–AVR–04/2013, Table 18-5. Relationship between SCK and the oscillator frequency.
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

	// scemr:
	// b0=1: 1: Base clock frequency is 8 times the bit rate, 
	// b0=0: 0: Base clock frequency is 16 times the bit rate
	// scmsr:
	// b1..b0: 0: /1, 1: /4, 2: /16, 3: /64
	enum
	{
		SCEMR_x16 = 0x00,	// ABCS=0
		SCEMR_x8 = 0x01,	// ABCS=1
		SCSMR_DIV1 = 0x00,
		SCSMR_DIV4 = 0x01,
		SCSMR_DIV16 = 0x02,
		SCSMR_DIV64 = 0x03,
	};
	static const FLASHMEM struct spcr_spsr_tag { uint_fast8_t scemr, scsmr; } scemr_scsmr [] =
	{
		{ SCEMR_x8, 	SCSMR_DIV1, },		/* /8 = 8 * 1 */
		{ SCEMR_x16, 	SCSMR_DIV1, }, 		/* /16 = 16 * 1 */
		{ SCEMR_x8, 	SCSMR_DIV4, },		/* /32 = 8 * 4 */
		{ SCEMR_x16, 	SCSMR_DIV4, },		/* /64 = 16 * 4 */
		{ SCEMR_x8, 	SCSMR_DIV16, }, 	/* /128 = 8 * 16 */
		{ SCEMR_x16, 	SCSMR_DIV16, }, 	/* /256 = 16 * 16 */
		{ SCEMR_x8, 	SCSMR_DIV64, },  	/* /512 = 8 * 64 */
		{ SCEMR_x16, 	SCSMR_DIV64, }, 	/* /1024 = 16 * 64 */
	};

#else
	//#error Undefined CPUSTYLE_XXX
#endif

void hardware_spi_io_delay(void)
{
#if CPUSTYLE_ATMEGA || CPUSTYLE_ATXMEGA
	_NOP();
	_NOP();
#elif	CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7
	__DSB(); 
	//__ISB(); 
	__NOP();
	__NOP();
#elif	CPUSTYLE_ARM_CM0
	__NOP();
#else
	// Cortex A9
	__NOP();
	__NOP();
#endif
}

#if WITHUART1HW

void 
hardware_uart1_set_speed(uint_fast32_t baudrate)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		// Использование автоматического расчёта предделителя
		unsigned value;
		const uint_fast8_t prei = calcdivider(calcdivround(baudrate), ATSAM3S_USART_BRGR_WIDTH, ATSAM3S_USART_BRGR_TAPS, & value, 0);
		USART0->US_BRGR = value;
		if (prei == 0)
		{
			USART0->US_MR |= US_MR_OVER;
		}
		else
		{
			USART0->US_MR &= ~ US_MR_OVER;
		}
	#elif HARDWARE_ARM_USEUSART1
		// Использование автоматического расчёта предделителя
		unsigned value;
		const uint_fast8_t prei = calcdivider(calcdivround(baudrate), ATSAM3S_USART_BRGR_WIDTH, ATSAM3S_USART_BRGR_TAPS, & value, 0);
		USART1->US_BRGR = value;
		if (prei == 0)
		{
			USART1->US_MR |= US_MR_OVER;
		}
		else
		{
			USART1->US_MR &= ~ US_MR_OVER;
		}

	#elif HARDWARE_ARM_USEUART0
		// Использование автоматического расчёта предделителя
		unsigned value;
		calcdivider(calcdivround(baudrate), ATSAM3S_UART_BRGR_WIDTH, ATSAM3S_UART_BRGR_TAPS, & value, 0);
		UART0->UART_BRGR = value;
	#elif HARDWARE_ARM_USEUART1
		// Использование автоматического расчёта предделителя
		unsigned value;
		calcdivider(calcdivround(baudrate), ATSAM3S_UART_BRGR_WIDTH, ATSAM3S_UART_BRGR_TAPS, & value, 0);
		UART1->UART_BRGR = value;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(baudrate), AT91SAM7_USART_BRGR_WIDTH, AT91SAM7_USART_BRGR_TAPS, & value, 0);
	
	#if HARDWARE_ARM_USEUSART0
		AT91C_BASE_US0->US_BRGR = value;
		if (prei == 0)
		{
			AT91C_BASE_US0->US_MR |= AT91C_US_OVER;
		}
		else
		{
			AT91C_BASE_US0->US_MR &= ~ AT91C_US_OVER;
		}
	#elif HARDWARE_ARM_USEUSART1
		AT91C_BASE_US1->US_BRGR = value;
		if (prei == 0)
		{
			AT91C_BASE_US1->US_MR |= AT91C_US_OVER;
		}
		else
		{
			AT91C_BASE_US1->US_MR &= ~ AT91C_US_OVER;
		}
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_ATMEGA_XXX4

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR0A |= (1U << U2X0);
	else
		UCSR0A &= ~ (1U << U2X0);

	UBRR0 = value;	/* Значение получено уже уменьшенное на 1 */


#elif CPUSTYLE_ATMEGA128

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR0A |= (1U << U2X0);
	else
		UCSR0A &= ~ (1U << U2X0);

	UBRR0H = (value >> 8) & 0xff;	/* Значение получено уже уменьшенное на 1 */
	UBRR0L = value & 0xff;

#elif CPUSTYLE_ATMEGA

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSRA |= (1U << U2X);
	else
		UCSRA &= ~ (1U << U2X);

	UBRRH = (value >> 8) & 0xff;	/* Значение получено уже уменьшенное на 1 */
	UBRRL = value & 0xff;

#elif CPUSTYLE_ATXMEGAXXXA4

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(baudrate), ATXMEGA_UBR_WIDTH, ATXMEGA_UBR_TAPS, & value, 1);
	if (prei == 0)
		USARTE0.CTRLB |= USART_CLK2X_bm;
	else
		USARTE0.CTRLB &= ~USART_CLK2X_bm;
	// todo: проверить требование к порядку обращения к портам 
	USARTE0.BAUDCTRLA = (value & 0xff);	/* Значение получено уже уменьшенное на 1 */
	USARTE0.BAUDCTRLB = (ATXMEGA_UBR_BSEL << 4) | ((value >> 8) & 0x0f);

#elif CPUSTYLE_STM32F

	// uart1 on apb2 up to 72/36 MHz

	USART1->BRR = calcdivround_pclk2(baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYPE_TMS320F2833X

	const unsigned long lspclk = CPU_FREQ / 4;
	const unsigned long brr = (lspclk / 8) / baudrate;	// @ CPU_FREQ = 100 MHz, 9600 can not be programmed

	SCIAHBAUD = (brr - 1) >> 8;		// write 8 bits, not 16
	SCIALBAUD = (brr - 1) >> 0;

#elif CPUSTYLE_R7S721

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_p1clock(baudrate), R7S721_SCIF_SCBRR_WIDTH, R7S721_SCIF_SCBRR_TAPS, & value, 1);

	SCIF0.SCSMR = (SCIF0.SCSMR & ~ 0x03) | 
		scemr_scsmr [prei].scsmr |	// prescaler: 0: /1, 1: /4, 2: /16, 3: /64
		0;
	SCIF0.SCEMR = (SCIF0.SCEMR & ~ (0x80 | 0x01)) | 
		0 * 0x80 |						// BGDM
		scemr_scsmr [prei].scemr |	// ABCS = 8/16 clocks per bit
		0;
	SCIF0.SCBRR = value;	/* Bit rate register */

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

#endif /* WITHUART1HW */


#if WITHUART2HW

void 
hardware_uart2_set_speed(uint_fast32_t baudrate)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		// Использование автоматического расчёта предделителя
		unsigned value;
		const uint_fast8_t prei = calcdivider(calcdivround(baudrate), ATSAM3S_USART_BRGR_WIDTH, ATSAM3S_USART_BRGR_TAPS, & value, 0);
		USART0->US_BRGR = value;
		if (prei == 0)
		{
			USART0->US_MR |= US_MR_OVER;
		}
		else
		{
			USART0->US_MR &= ~ US_MR_OVER;
		}
	#elif HARDWARE_ARM_USEUSART1
		// Использование автоматического расчёта предделителя
		unsigned value;
		const uint_fast8_t prei = calcdivider(calcdivround(baudrate), ATSAM3S_USART_BRGR_WIDTH, ATSAM3S_USART_BRGR_TAPS, & value, 0);
		USART1->US_BRGR = value;
		if (prei == 0)
		{
			USART1->US_MR |= US_MR_OVER;
		}
		else
		{
			USART1->US_MR &= ~ US_MR_OVER;
		}

	#elif HARDWARE_ARM_USEUART0
		// Использование автоматического расчёта предделителя
		unsigned value;
		calcdivider(calcdivround(baudrate), ATSAM3S_UART_BRGR_WIDTH, ATSAM3S_UART_BRGR_TAPS, & value, 0);
		UART0->UART_BRGR = value;
	#elif HARDWARE_ARM_USEUART1
		// Использование автоматического расчёта предделителя
		unsigned value;
		calcdivider(calcdivround(baudrate), ATSAM3S_UART_BRGR_WIDTH, ATSAM3S_UART_BRGR_TAPS, & value, 0);
		UART1->UART_BRGR = value;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(baudrate), AT91SAM7_USART_BRGR_WIDTH, AT91SAM7_USART_BRGR_TAPS, & value, 0);
	
	#if HARDWARE_ARM_USEUSART0
		AT91C_BASE_US0->US_BRGR = value;
		if (prei == 0)
		{
			AT91C_BASE_US0->US_MR |= AT91C_US_OVER;
		}
		else
		{
			AT91C_BASE_US0->US_MR &= ~ AT91C_US_OVER;
		}
	#elif HARDWARE_ARM_USEUSART1
		AT91C_BASE_US1->US_BRGR = value;
		if (prei == 0)
		{
			AT91C_BASE_US1->US_MR |= AT91C_US_OVER;
		}
		else
		{
			AT91C_BASE_US1->US_MR &= ~ AT91C_US_OVER;
		}
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_ATMEGA_XXX4

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR1A |= (1U << U2X1);
	else
		UCSR1A &= ~ (1U << U2X1);

	UBRR1 = value;	/* Значение получено уже уменьшенное на 1 */


#elif CPUSTYLE_ATMEGA128

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR1A |= (1U << U2X1);
	else
		UCSR1A &= ~ (1U << U2X1);

	UBRR1H = (value >> 8) & 0xff;	/* Значение получено уже уменьшенное на 1 */
	UBRR1L = value & 0xff;

#elif CPUSTYLE_ATMEGA

	#error WITHUART2HW not supported with CPUSTYLE_ATMEGA 

#elif CPUSTYLE_ATXMEGAXXXA4

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(baudrate), ATXMEGA_UBR_WIDTH, ATXMEGA_UBR_TAPS, & value, 1);
	if (prei == 0)
		USARTE1.CTRLB |= USART_CLK2X_bm;
	else
		USARTE1.CTRLB &= ~USART_CLK2X_bm;
	// todo: проверить требование к порядку обращения к портам 
	USARTE1.BAUDCTRLA = (value & 0xff);	/* Значение получено уже уменьшенное на 1 */
	USARTE1.BAUDCTRLB = (ATXMEGA_UBR_BSEL << 4) | ((value >> 8) & 0x0f);

#elif CPUSTYLE_STM32F

	// uart2 on apb1

	USART2->BRR = calcdivround_pclk1(baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYPE_TMS320F2833X

	const unsigned long lspclk = CPU_FREQ / 4;
	const unsigned long brr = (lspclk / 8) / baudrate;	// @ CPU_FREQ = 100 MHz, 9600 can not be programmed

	SCIBHBAUD = (brr - 1) >> 8;		// write 8 bits, not 16
	SCIBLBAUD = (brr - 1) >> 0;

#elif CPUSTYLE_R7S721

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_p1clock(baudrate), R7S721_SCIF_SCBRR_WIDTH, R7S721_SCIF_SCBRR_TAPS, & value, 1);

	SCIF1.SCSMR = (SCIF1.SCSMR & ~ 0x03) | 
		scemr_scsmr [prei].scsmr |	// prescaler: 0: /1, 1: /4, 2: /16, 3: /64
		0;
	SCIF1.SCEMR = (SCIF1.SCEMR & ~ (0x80 | 0x01)) | 
		0 * 0x80 |						// BGDM
		scemr_scsmr [prei].scemr |	// ABCS = 8/16 clocks per bit
		0;
	SCIF1.SCBRR = value;	/* Bit rate register */

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

#endif /* WITHUART2HW */

#if WITHTWIHW

void hardware_twi_master_configure(void)
{
#if CPUSTYLE_ATMEGA

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(SCL_CLOCK * 2) - 8, ATMEGA_TWBR_WIDTH, ATMEGA_TWBR_TAPS, & value, 0);

	TWSR = prei; 	/* prescaler */
	TWBR = value;
	TWCR = (1U << TWEN);

#elif CPUSTYLE_AT91SAM7S

	AT91C_BASE_PMC->PMC_PCER = (1UL << AT91C_ID_TWI) | (1UL << AT91C_ID_PIOA);
	//
    // Reset the TWI
    AT91C_BASE_TWI->TWI_CR = AT91C_TWI_SWRST;
    (void) AT91C_BASE_TWI->TWI_RHR;

    // Set master mode
    AT91C_BASE_TWI->TWI_CR = AT91C_TWI_MSEN;
	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(SCL_CLOCK * 2) - 3, AT91SAM7_TWI_WIDTH, AT91SAM7_TWI_TAPS, & value, 0);

    AT91C_BASE_TWI->TWI_CWGR = (prei << 16) | (value << 8) | value;

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	PMC->PMC_PCER0 = (1UL << ID_TWI0);	 // разрешить тактированние этого блока
	//
    // Reset the TWI
    TWI0->TWI_CR = TWI_CR_SWRST;
    (void) TWI0->TWI_RHR;

    // Set master mode
    TWI0->TWI_CR = TWI_CR_MSEN;
	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(SCL_CLOCK * 2) - 4, ATSAM3S_TWI_WIDTH, ATSAM3S_TWI_TAPS, & value, 1);
	//prei = 0;
	//value = 70;
    TWI0->TWI_CWGR = TWI_CWGR_CKDIV(prei) | TWI_CWGR_CHDIV(value) | TWI_CWGR_CLDIV(value);

#elif CPUSTYLE_ATXMEGA

	TARGET_TWI.MASTER.BAUD = ((CPU_FREQ / (2 * SCL_CLOCK)) - 5);
	TARGET_TWI.MASTER.CTRLA = TWI_MASTER_ENABLE_bm;
  	//TARGET_TWI.MASTER.CTRLB = TWI_MASTER_SMEN_bm; 
	TARGET_TWI.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	//конфигурирую непосредствено І2С
	RCC->APB1ENR |= (RCC_APB1ENR_I2C1EN); //вкл тактирование контроллера I2C
	__DSB();

	I2C1->CR1 |= I2C_CR1_SWRST;
	I2C1->CR1 &= ~ I2C_CR1_SWRST;
	I2C1->CR1 &= ~ I2C_CR1_PE; // все конфигурации необходимо проводить только со сброшеным битом PE

/*
	The FREQ bits must be configured with the APB clock frequency value (I2C peripheral
	connected to APB). The FREQ field is used by the peripheral to generate data setup and
	hold times compliant with the I2C specifications. The minimum allowed frequency is 2 MHz,
	the maximum frequency is limited by the maximum APB frequency (42 MHz) and an intrinsic
	limitation of 46 MHz.

*/

	I2C1->CR2 = (I2C1->CR2 & ~ (I2C_CR2_FREQ)) |
		((I2C_CR2_FREQ_0 * 42) & I2C_CR2_FREQ) |
		0;
	// (1000 ns / 125 ns = 8 + 1)
	// (1000 ns / 22 ns = 45 + 1)
	I2C1->TRISE = 46; //время установления логического уровня в количестве цыклах тактового генератора I2C
	
	I2C1->CCR = (I2C1->CCR & ~ (I2C_CCR_CCR | I2C_CCR_FS | I2C_CCR_DUTY)) |
		(calcdivround_pclk1(SCL_CLOCK * 25) & I2C_CCR_CCR) |	// Делитель для получения 10 МГц (400 кHz * 25)
	#if SCL_CLOCK == 400000UL
		I2C_CCR_FS | 
		I2C_CCR_DUTY | // T high = 9 * CCR * TPCLK1, T low = 16 * CCR * TPCLK1: full cycle = 25 * CCR * TPCLK1
	#endif /* SCL_CLOCK == 400000UL */
		0;

	I2C1->CR1 |= I2C_CR1_ACK | I2C_CR1_PE; // включаю тактирование переферии І2С

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	//конфигурирую непосредствено І2С
	RCC->APB1ENR |= (RCC_APB1ENR_I2C1EN); //вкл тактирование контроллера I2C
	__DSB();
    // set I2C1 clock to PCLCK (72/64/36 MHz)
    RCC->CFGR3 |= RCC_CFGR3_I2C1SW;		// PCLK1_FREQ or PCLK2_FREQ (PCLK of this BUS, PCLK1) selected as I2C spi clock source


	I2C1->CR1 &= ~ I2C_CR1_PE; // все конфигурации необходимо проводить только со сброшеным битом PE

	//I2C1->CR2 = (I2C1->CR2 & ~I2C_CR2_FREQ) | I2C_CR2_FREQ_0 * ( PCLK2_FREQ / SCL_CLOCK); // частота тактирования модуля I2C1 до делителя равна FREQ_IN
	//I2C1->CR2 = I2C_CR2_FREQ_0 * 4; //255; // |= I2C_CR2_FREQ;	// debug

	I2C1->TIMINGR = (I2C1->TIMINGR & ~ I2C_TIMINGR_PRESC) | (4UL << 28);

	//I2C1->CCR &= ~I2C_CCR_CCR;
	//I2C1->CCR |= (1000/(2*40000)) * ((I2C1->CR2&I2C_CR2_FREQ) / I2C_CR2_FREQ_0); // конечный коэффциент деления
	////I2C1->CCR = 40; //36; // / 4;	//|= I2C_CCR_CCR;	// debug
	//I2C1->CCR |= I2C_CCR_FS;

	//I2C1->TRISE = 9; //время установления логического уровня в количестве циклов тактового генератора I2C

    // disable analog filter
    I2C1->CR1 |= I2C_CR1_ANFOFF;
    // from stm32f3_i2c_calc.py (400KHz, 125ns rise/fall time, no AF/DFN)
    const uint_fast8_t sdadel = 7;
    const uint_fast8_t scldel = 5;
    I2C1->TIMINGR = 0x30000C19 | ((scldel & 0x0F) << 20) | ((sdadel & 0x0F) << 16);

	I2C1->CR1 |= I2C_CR1_PE; // включаю тактирование периферии І2С

#elif CPUSTYLE_STM32F7XX
	//конфигурирую непосредствено І2С
	RCC->APB1ENR |= (RCC_APB1ENR_I2C1EN); //вкл тактирование контроллера I2C
	__DSB();

	// Disable the I2Cx peripheral
	I2C1->CR1 &= ~ I2C_CR1_PE;
	while ((I2C1->CR1 & I2C_CR1_PE) != 0)
		;

	// Set timings. Asuming I2CCLK is 50 MHz (APB1 clock source)
	I2C1->TIMINGR = 
		//0x00912732 |		// Discovery BSP code from ST examples
		0x00913742 |		// подобрано для 400 кГц
		4 * (1uL << I2C_TIMINGR_PRESC_Pos) |			// prescaler, was: 0
		0;



	// Use 7-bit addresses
	I2C1->CR2 &= ~ I2C_CR2_ADD10;

	// Enable auto-end mode
	//I2C1->CR2 |= I2C_CR2_AUTOEND;

	// Disable the analog filter
	I2C1->CR1 |= I2C_CR1_ANFOFF;

	// Disable NOSTRETCH
	I2C1->CR1 |= I2C_CR1_NOSTRETCH;

	// Enable the I2Cx peripheral
	I2C1->CR1 |= I2C_CR1_PE;

#elif CPUSTYLE_STM32H7XX
	//конфигурирую непосредствено І2С
	RCC->APB1LENR |= (RCC_APB1LENR_I2C1EN); //вкл тактирование контроллера I2C
	__DSB();

	// Disable the I2Cx peripheral
	I2C1->CR1 &= ~ I2C_CR1_PE;
	while ((I2C1->CR1 & I2C_CR1_PE) != 0)
		;

	// Set timings. Asuming I2CCLK is 50 MHz (APB1 clock source)
	I2C1->TIMINGR = 
		//0x00912732 |		// Discovery BSP code from ST examples
		0x00913742 |		// подобрано для 400 кГц
		4 * (1uL << I2C_TIMINGR_PRESC_Pos) |			// prescaler, was: 0
		0;



	// Use 7-bit addresses
	I2C1->CR2 &= ~ I2C_CR2_ADD10;

	// Enable auto-end mode
	//I2C1->CR2 |= I2C_CR2_AUTOEND;

	// Disable the analog filter
	I2C1->CR1 |= I2C_CR1_ANFOFF;

	// Disable NOSTRETCH
	I2C1->CR1 |= I2C_CR1_NOSTRETCH;

	// Enable the I2Cx peripheral
	I2C1->CR1 |= I2C_CR1_PE;

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#endif /* WITHTWIHW */

#if WITHCPUADCHW
static void hardware_adc_startonescan(void);		// хотя бы один вход (s-метр) есть.
#endif /* WITHCPUADCHW */

/* 
	Машинно-независимый обработчик прерываний. 
	Вызывается с периодом 1/ELKEY_DISCRETE от длительности точки 
*/
static RAMFUNC void spool_elkeybundle(void)
{
#if WITHOPERA4BEACON
	spool_0p128();
#elif WITHELKEY
	elkey_spool_dots();		// вызывается с периодом 1/ELKEY_DISCRETE от длительности точки
#endif /* WITHOPERA4BEACON */
}

/* 
	Машинно-независимый обработчик прерываний. 
	Вызывается при изменении состояния входов электронного ключа,
    входа манипуляции от CAT (CAT_DTR). 
*/
static RAMFUNC void spool_elkeyinputsbundle(void)
{
	//key_spool_inputs();	// опрос состояния электронного ключа и запоминание факта нажатия
}


/* Машинно-независимый обработчик прерываний. */
// Функции с побочным эффектом - отсчитывание времени. 
// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
static RAMFUNC void spool_systimerbundle1(void)
{
	static uint_fast16_t spool_1stickcount;
	//beacon_255();

	enum { TICKS1000MS = NTICKS(1000) };
	//spool_lfm();
	seq_spool_ticks();
#if WITHENCODER
	enc_spool();	// таймер для динамического изменения шага перестройки
#endif /* WITHENCODER */
	display_spool();	// отсчёт времени по запрещению обновления дисплея при вращении валкодера
#if WITHMODEM 
	modem_spool();
#endif /* WITHMODEM */
#if WITHUSBHW
	board_usb_spool();
#endif /* WITHUSBHW */
	buffers_spool();
	spool_encinterrupt2();	/* прерывание по изменению сигнала на входах от валкодера #2*/
	// Формирование секундного прерывания
	if (++ spool_1stickcount >= TICKS1000MS)
	{
		spool_1stickcount = 0;
		spool_secound();
	}
}

/* Машинно-независимый обработчик прерываний. */
// Функции с побочным эффектом редиспетчеризайии. 
// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
static RAMFUNC void spool_systimerbundle2(void)
{
#if WITHKEYBOARD
#if ! KEYBOARD_USE_ADC
	kbd_spool();	// 
#endif /* ! KEYBOARD_USE_ADC */
#endif /* WITHKEYBOARD */
#if WITHCPUADCHW
	hardware_adc_startonescan();	// хотя бы один вход (s-метр) есть.
#endif /* WITHCPUADCHW */
}

#if WITHCPUADCHW
/* 
	Машинно-независимый обработчик прерываний. 
	Вызывается с периодом 1/TIMETICKS по окончании получения данных всех каналов АЦП, 
	перечисленных в таблице adcinputs.
*/
static RAMFUNC void spool_adcdonebundle(void)
{
#if WITHTX && WITHVOX
	vox_probe(board_getvox(), board_getavox());
#endif /* WITHTX && WITHVOX */
#if WITHKEYBOARD
#if KEYBOARD_USE_ADC
	kbd_spool();	// 
#endif /* KEYBOARD_USE_ADC */
#endif /* WITHKEYBOARD */
}
#endif /* WITHCPUADCHW */

#if CPUSTYLE_STM32F

	void  
	SysTick_Handler(void)
	{
		spool_systimerbundle1();	// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
		spool_systimerbundle2();	// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
	}
	
	#if WITHELKEY
	// 1/20 dot length interval timer
	void  
	TIM3_IRQHandler(void)
	{
		const portholder_t st = TIM3->SR;
		if ((st & TIM_SR_UIF) != 0)
		{
			TIM3->SR = ~ TIM_SR_UIF;	// clear UIF interrupt request
			spool_elkeybundle();
		}
	}
	#else
	void  
	TIM3_IRQHandler(void)
	{
	}

	#endif /* WITHELKEY */

	/* прерывания от валколера при наличии в системе вложенных прерываний вызываются на уровне приоритета REALTINE */
	static RAMFUNC void stm32fxxx_pinirq(portholder_t pr)
	{
	#if WITHELKEY && defined (ELKEY_BIT_LEFT) && defined (ELKEY_BIT_RIGHT)
		if ((pr & (ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT)) != 0)
		{
			spool_elkeyinputsbundle();
		}
	#endif /* WITHELKEY && defined (ELKEY_BIT_LEFT) && defined (ELKEY_BIT_RIGHT) */
	#if WITHENCODER && defined (ENCODER_BITS)
		if ((pr & ENCODER_BITS) != 0)
		{
			spool_encinterrupt();	/* прерывание по изменению сигнала на входах от валкодера #1*/
		}
	#endif /* WITHENCODER && defined (ENCODER_BITS) */
	#if WITHENCODER && defined (ENCODER2_BITS)
		if ((pr & ENCODER2_BITS) != 0)
		{
			spool_encinterrupt2();	/* прерывание по изменению сигнала на входах от валкодера #2*/
		}
	#endif /* WITHENCODER && ENCODER2_BITS */
	}

	#if CPUSTYLE_STM32L0XX

	void ATTRWEAK EXTI0_1_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_IM0 | EXTI_IMR_IM1);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}
	void ATTRWEAK EXTI2_3_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_IM2 | EXTI_IMR_IM3);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}
	void ATTRWEAK EXTI4_15_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (
				EXTI_IMR_IM15 | EXTI_IMR_IM14 | EXTI_IMR_IM13 | EXTI_IMR_IM12 | 
				EXTI_IMR_IM11 | EXTI_IMR_IM10 | EXTI_IMR_IM9 | EXTI_IMR_IM8 | 
				EXTI_IMR_IM7 | EXTI_IMR_IM6 | EXTI_IMR_IM5 | EXTI_IMR_IM4
				);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	#elif CPUSTYLE_STM32H7XX

	void ATTRWEAK EXTI0_IRQHandler(void)
	{
		const portholder_t pr = EXTI_D1->PR1 & (EXTI_PR1_PR0);
		EXTI_D1->PR1 = pr;		// reset all existing requests
		(void) EXTI_D1->PR1;
		stm32fxxx_pinirq(pr);
	}

	
	void ATTRWEAK EXTI1_IRQHandler(void)
	{
		const portholder_t pr = EXTI_D1->PR1 & (EXTI_PR1_PR1);
		EXTI_D1->PR1 = pr;		// reset all existing requests
		(void) EXTI_D1->PR1;
		stm32fxxx_pinirq(pr);
	}

	void ATTRWEAK EXTI2_IRQHandler(void)
	{
		const portholder_t pr = EXTI_D1->PR1 & (EXTI_PR1_PR2);
		EXTI_D1->PR1 = pr;		// reset all existing requests
		(void) EXTI_D1->PR1;
		stm32fxxx_pinirq(pr);
	}

	void ATTRWEAK EXTI3_IRQHandler(void)
	{
		const portholder_t pr = EXTI_D1->PR1 & (EXTI_PR1_PR3);
		EXTI_D1->PR1 = pr;		// reset all existing requests
		(void) EXTI_D1->PR1;
		stm32fxxx_pinirq(pr);
	}

	void ATTRWEAK EXTI4_IRQHandler(void)
	{
		const portholder_t pr = EXTI_D1->PR1 & (EXTI_PR1_PR4);
		EXTI_D1->PR1 = pr;		// reset all existing requests
		(void) EXTI_D1->PR1;
		stm32fxxx_pinirq(pr);
	}

	void ATTRWEAK EXTI9_5_IRQHandler(void)
	{
		const portholder_t pr = EXTI_D1->PR1 & (EXTI_PR1_PR9 | EXTI_PR1_PR8 | EXTI_PR1_PR7 | EXTI_PR1_PR6 | EXTI_PR1_PR5);
		EXTI_D1->PR1 = pr;		// reset all existing requests
		(void) EXTI_D1->PR1;
		stm32fxxx_pinirq(pr);
	}
	void ATTRWEAK EXTI15_10_IRQHandler(void)
	{
		const portholder_t pr = EXTI_D1->PR1 & (EXTI_PR1_PR15 | EXTI_PR1_PR14 | EXTI_PR1_PR13 | EXTI_PR1_PR12 | EXTI_PR1_PR11 | EXTI_PR1_PR10);
		EXTI_D1->PR1 = pr;		// reset all existing requests
		(void) EXTI_D1->PR1;
		stm32fxxx_pinirq(pr);
	}

	#else

	void ATTRWEAK EXTI0_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR0);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	void ATTRWEAK EXTI0_1_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR0 | EXTI_IMR_MR1);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	void ATTRWEAK EXTI1_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR1);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	void ATTRWEAK EXTI2_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR2);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	void ATTRWEAK EXTI2_3_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR2 | EXTI_IMR_MR3);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	void ATTRWEAK EXTI4_15_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (
				EXTI_IMR_MR15 | EXTI_IMR_MR14 | EXTI_IMR_MR13 | EXTI_IMR_MR12 | 
				EXTI_IMR_MR11 | EXTI_IMR_MR10 | EXTI_IMR_MR9 | EXTI_IMR_MR8 | 
				EXTI_IMR_MR7 | EXTI_IMR_MR6 | EXTI_IMR_MR5 | EXTI_IMR_MR4
				);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	void ATTRWEAK EXTI3_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR3);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	void ATTRWEAK EXTI4_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR4);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}

	void ATTRWEAK EXTI9_5_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR9 | EXTI_IMR_MR8 | EXTI_IMR_MR7 | EXTI_IMR_MR6 | EXTI_IMR_MR5);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}
	void ATTRWEAK EXTI15_10_IRQHandler(void)
	{
		const portholder_t pr = EXTI->PR & (EXTI_IMR_MR15 | EXTI_IMR_MR14 | EXTI_IMR_MR13 | EXTI_IMR_MR12 | EXTI_IMR_MR11 | EXTI_IMR_MR10);
		EXTI->PR = pr;		// reset all existing requests
		//(void) EXTI->PR;
		stm32fxxx_pinirq(pr);
	}
	#endif

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	void RAMFUNC_NONILINE
	PIOA_Handler(void)
	{
		//display_menu_label(PSTR("PIOA_IrqHandler"));
		//for (;;)
		//	;
		// When the software reads PIO_ISR, all the interrupts are automatically cleared. This signifies that
		// all the interrupts that are pending when PIO_ISR is read must be handled.
		const portholder_t state = PIOA->PIO_ISR;
	#if WITHENCODER && defined (ENCODER_BITS)
		if ((state & (ENCODER_BITS)) != 0) // re-enable interrupt from PIO
		{
			spool_encinterrupt();	/* прерывание по изменению сигнала на входах от валкодера */
		}
	#endif /* WITHENCODER && defined (ENCODER_BITS) */
	#if WITHELKEY && defined (ELKEY_BIT_LEFT) && defined (ELKEY_BIT_RIGHT)
		if ((state & (ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT)) != 0) // re-enable interrupt from PIO
		{
			spool_elkeyinputsbundle();
		}
	#endif /* WITHELKEY && defined (ELKEY_BIT_LEFT) && defined (ELKEY_BIT_RIGHT) */
	#if WITHNMEA
		if ((state & FROMCAT_BIT_DTR) != 0 && (FROMCAT_TARGET_PIN_DTR & FROMCAT_BIT_DTR) != 0)
		{
			spool_nmeapps();
		}
	#endif /* WITHNMEA */
	}

	void RAMFUNC_NONILINE 
	SysTick_Handler(void)
	{
		spool_systimerbundle1();	// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
		spool_systimerbundle2();	// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
	}

	// AT91C_ID_TC2 - 1/20 dot length interval timer
	RAMFUNC_NONILINE void 
	TC2_Handler(void) 
	{
		// TC2 used for generate 1/20 of morse dot length intervals
		if ((TC0->TC_CHANNEL [2].TC_SR & TC_SR_CPCS) != 0)	// read status register - reset interrupt request
		{
			spool_elkeybundle();
		}
	}
	// AT91C_ID_TC0 - lfm update timer
	RAMFUNC_NONILINE void 
	TC0_Handler(void) 
	{
		// TC2 used for generate 1/20 of morse dot length intervals
		if ((TC0->TC_CHANNEL [0].TC_SR & TC_SR_CPCS) != 0)	// read status register - reset interrupt request
		{
#if WITHLFM
			spool_lfm();
#endif
		}
	}

	RAMFUNC_NONILINE void ATTRWEAK USART0_Handler(void) 
	{
	}

	RAMFUNC_NONILINE void ATTRWEAK USART1_Handler(void) 
	{
	}


#elif CPUSTYLE_AT91SAM7S 

	RAMFUNC_NONILINE void AT91F_PIOA_IRQHandler(void) 
	{
		// When the software reads PIO_ISR, all the interrupts are automatically cleared. This signifies that
		// all the interrupts that are pending when PIO_ISR is read must be handled.
		const portholder_t state = AT91C_BASE_PIOA->PIO_ISR;
	#if WITHENCODER && defined (ENCODER_BITS)
		if ((state & (ENCODER_BITS)) != 0) // re-enable interrupt from PIO
		{
			spool_encinterrupt();	/* прерывание по изменению сигнала на входах от валкодера */
		}
	#endif /* WITHENCODER && defined (ENCODER_BITS) */
	#if WITHELKEY && defined (ELKEY_BIT_LEFT) && defined (ELKEY_BIT_RIGHT)
		if ((state & (ELKEY_BIT_LEFT | ELKEY_BIT_RIGHT)) != 0) // re-enable interrupt from PIO
		{
			spool_elkeyinputsbundle();
		}
	#endif /* WITHELKEY && defined (ELKEY_BIT_LEFT) && defined (ELKEY_BIT_RIGHT) */
	#if WITHNMEA
		if ((state & FROMCAT_BIT_DTR) != 0 && (FROMCAT_TARGET_PIN_DTR & FROMCAT_BIT_DTR) != 0)
		{
			spool_nmeapps();
		}
	#endif /* WITHNMEA */
	}

	// AT91C_ID_TC2 - 1/20 dot length interval timer
	static RAMFUNC_NONILINE void AT91F_TC2_IRQHandler(void) 
	{
		// TC2 used for generate 1/20 of morse dot length intervals
		if ((AT91C_BASE_TCB->TCB_TC2.TC_SR & AT91C_TC_CPCS) != 0)	// read status register - reset interrupt request
		{
			spool_elkeybundle();
		}
	}

	// AT91C_ID_TC0 - LFM periodical update
	static RAMFUNC_NONILINE void AT91F_TC0_IRQHandler(void) 
	{
		// TC0 used for generate LFM sweep
		if ((AT91C_BASE_TCB->TCB_TC0.TC_SR & AT91C_TC_CPCS) != 0)	// read status register - reset interrupt request
		{
#if WITHLFM
			spool_lfm();
#endif
		}
	}

	static RAMFUNC_NONILINE void AT91F_SYS_IRQHandler(void) 
	{
		if ((AT91C_BASE_PITC->PITC_PISR & AT91C_PITC_PITS) != 0)
		{
			// Обработчик Periodic Interval Timer (PIT)
			uint_fast32_t cnt = (AT91C_BASE_PITC->PITC_PIVR & AT91C_PITC_PICNT) >> 20;	// Reset interrupt request from Periodic interval timer.
			while (cnt --)
				spool_systimerbundle1();
			spool_systimerbundle2();
		}
	}

#elif CPUSTYLE_ATMEGA

	ISR(INT0_vect)
	{
		spool_encinterrupt();	/* прерывание по изменению сигнала на входах от валкодера */
	}

	ISR(INT1_vect)
	{
		spool_encinterrupt();	/* прерывание по изменению сигнала на входах от валкодера */
	}

	// Обработчик вызывается с частотой TICKS_FREQUENCY герц.
	#if CPUSTYLE_ATMEGA328
		ISR(TIMER2_COMPA_vect)
		{
			spool_systimerbundle1();	// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
			spool_systimerbundle2();	// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
		}
	#elif CPUSTYLE_ATMEGA_XXX4
		ISR(TIMER0_COMPA_vect)
		{
			spool_systimerbundle1();	// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
			spool_systimerbundle2();	// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
		}
	#else /* CPUSTYLE_ATMEGA_XXX4 */
		ISR(TIMER0_COMP_vect)
		{
			spool_systimerbundle1();	// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
			spool_systimerbundle2();	// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
		}
	#endif /* CPUSTYLE_ATMEGA_XXX4 */


	// Timer 1 output compare A interrupt service routine
	ISR(TIMER1_COMPA_vect)
	{
		spool_elkeybundle();
	}
	// Обработчик по изменению состояния входов PTT и электронного ключа
	#if CPUSTYLE_ATMEGA_XXX4
		#if WITHELKEY && defined (ELKEY_BIT_LEFT) && defined (ELKEY_BIT_RIGHT)
			// PC7 - PTT input, PC6 & PC5 - eectronic key inputs
			ISR(PCIVECT)
			{
				spool_elkeyinputsbundle();
			}
		#endif /* (WITHELKEY && defined (ELKEY_BIT_LEFT) && defined (ELKEY_BIT_RIGHT)) */
		#if defined (FROMCAT_BIT_DTR) && defined (DTRPCICR_BIT) && (PCICR_BIT != DTRPCICR_BIT)
			ISR(DTRPCIVECT)
			{
				spool_elkeyinputsbundle();	// по изменению PTT
			}
		#endif
	#endif /* CPUSTYLE_ATMEGA_XXX4 && defined (PCIVECT) */

#elif CPUSTYLE_ATXMEGA

	#if CPUSTYLE_ATXMEGAXXXA4

	ISR(PORTC_INT0_vect)
	{
		spool_encinterrupt();	/* прерывание по изменению сигнала на входах от валкодера */
	}

	ISR(PORTC_INT1_vect)
	{
		spool_encinterrupt();	/* прерывание по изменению сигнала на входах от валкодера */
	}

	ISR(TCC0_CCA_vect)
	{
		spool_systimerbundle1();	// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
		spool_systimerbundle2();	// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
	}

	// Timer 1 output compare A interrupt service routine
	ISR(TCC1_CCA_vect)
	{
		spool_elkeybundle();
	}
	// Обработчик по изменению состояния входов PTT и электронного ключа
	//#if CPUSTYLE_ATMEGA_XXX4
	// PC7 - PTT input, PC6 & PC5 - eectronic key inputs
	//ISR(PCIVECT)
	//{
	//	spool_elkeyinputsbundle();
	//}
	#endif /* CPUSTYLE_ATXMEGAXXXA4 */

#elif CPUSTYLE_STM32F30X
	#warning TODO: Add code for STM32F30X support

#elif CPUSTYLE_R7S721

	// Таймер "тиков"
	static void r7s721_ostm0_interrupt(void)
	{
		spool_systimerbundle1();	// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
		spool_systimerbundle2();	// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
	}

	// Таймер электронного ключа
	static void r7s721_ostm1_interrupt(void)
	{
		spool_elkeybundle();
	}

#else

	#error Undefined CPUSTYLE_XXX interrrupts handlers
#endif

//static volatile uint_fast8_t hardware_reqshutdown;
/* возвращаем запрос на выключение - от компаратора питания */
uint_fast8_t 
hardware_getshutdown(void)
{
#if CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F30X || CPUSTYLE_STM32L0XX
	//return hardware_reqshutdown;
	return (PWR->CR & PWR_CR_PVDE) && (PWR->CSR & PWR_CSR_PVDO);

#else
	return 0;
#endif
}

// Инициализация таймера, вызывающего прерывания с частотой TICKS_FREQUENCY.
// Должо вызываться позже настройки ADC
void 
hardware_timer_initialize(uint_fast32_t ticksfreq)
{

#if CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7

	SysTick_Config(calcdivround_systick(ticksfreq));	// Call SysTick_Handler

#elif CPUSTYLE_ATMEGA328

	// ATMega328
	// Timer/Counter 2 initialization
	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(ticksfreq), ATMEGA_TIMER2_WIDTH, ATMEGA_TIMER2_TAPS, & value, 1);

	ASSR = 0x00;
	TCCR2A = (1u << WGM01);	// CTC mode = 0x02
	TCCR2B = prei + 1; // прескалер
	OCR2A = value;	// делитель - программирование полного периода
	OCR2B = 0x00;
	TIMSK2 |= (1U << OCIE2A);	//0x02;	// enable interrupt from Timer/Counter 2 - use TIMER2_COMP_vect

	TCNT2 = 0x00;

#elif CPUSTYLE_ATMEGA_XXX4

	// Modern ATMega644

	// Timer/Counter 0 initialization
	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(ticksfreq), ATMEGA_TIMER0_WIDTH, ATMEGA_TIMER0_TAPS, & value, 1);
	TCCR0A = (1u << WGM01);	// CTC mode = 0x02
	TCCR0B = prei + 1; // прескалер
	OCR0A = value;	// делитель - программирование полного периода
	OCR0B = 0x00;
	TIMSK0 |= (1U << OCIE0A);	// enable interrupt from Timer/Counter 0 - use TIMER0_COMPA_vect

	TCNT0 = 0x00;

#elif CPUSTYLE_ATMEGA128
	// ATMega128/ATMega64

	// Timer/Counter 0 initialization
	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(ticksfreq), ATMEGA128_TIMER0_WIDTH, ATMEGA128_TIMER0_TAPS, & value, 1);
	TCCR0 = (1U << WGM01) | (prei + 1);	// прескалер
	OCR0 = value;	// делитель - программирование полного периода

	TIMSK |= (1U << OCIE0);	// enable interrupt from Timer/Counter 0 - use TIMER0_COMP_vect

	TCNT0 = 0x00;

#elif CPUSTYLE_ATMEGA32
	// Classic ATMega32

	// Timer/Counter 0 initialization
	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(ticksfreq), ATMEGA_TIMER0_WIDTH, ATMEGA_TIMER0_TAPS, & value, 1);
	TCCR0 = (1U << WGM01) | (prei + 1);	// прескалер
	OCR0 = value;	// делитель - программирование полного периода

	TIMSK |= (1U << OCIE0);	// enable interrupt from Timer/Counter 0 - use TIMER0_COMP_vect

	TCNT0 = 0x00;

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	/* const uint_fast8_t prei = */ calcdivider(calcdivround(ticksfreq), AT91SAM7_PITPIV_WIDTH, AT91SAM7_PITPIV_TAPS, & value, 1);

	// Periodic interval timer enable - see TICKS_FREQUENCY
	AT91C_BASE_PITC->PITC_PIMR = 
		(value & AT91C_PITC_PIV) | 
		AT91C_PITC_PITEN |
		AT91C_PITC_PITIEN;

	// possible chip errata
	AT91C_BASE_RTTC->RTTC_RTMR &= ~AT91C_RTTC_ALMIEN;		// запретить Real Time Timer Controller

	// programming interrupts from SYS
    AT91C_BASE_AIC->AIC_IDCR = (1UL << AT91C_ID_SYS);		// disable interrupt
    AT91C_BASE_AIC->AIC_SVR [AT91C_ID_SYS] = (AT91_REG) AT91F_SYS_IRQHandler;
	AT91C_BASE_AIC->AIC_SMR [AT91C_ID_SYS] = 
		(AT91C_AIC_SRCTYPE & AT91C_AIC_SRCTYPE_HIGH_LEVEL) |
		(AT91C_AIC_PRIOR & AT91C_AIC_PRIOR_LOWEST);
	AT91C_BASE_AIC->AIC_ICCR = (1UL << AT91C_ID_SYS);		// clear pending interrupt
    AT91C_BASE_AIC->AIC_IECR = (1UL << AT91C_ID_SYS);	// enable inerrupt

#elif CPUSTYLE_ATXMEGAXXXA4

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(ticksfreq), ATXMEGA_TIMER_WIDTH, ATXMEGA_TIMER_TAPS, & value, 1);
	// программирование таймера
	TCC0.CCA = value;	// timer/counter C0, compare register A, see TCC0_CCA_vect
	TCC0.CTRLA = (prei + 1);
	TCC0.CTRLB = (TC_WGMODE_FRQ_gc);
	TCC0.INTCTRLB = (TC_CCAINTLVL_HI_gc);
	// разрешение прерываний на входе в PMIC
	PMIC.CTRL |= (PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm);

#elif CPUSTYLE_R7S721

	// OSTM0
    /* ==== Module standby clear ==== */
    /* ---- Supply clock to the OSTM(channel 0) ---- */
	CPG.STBCR5 &= ~ CPG_STBCR5_MSTP51;	// Module Stop 51
	(void) CPG.STBCR5;			/* Dummy read */

    /* ==== Start counting with OS timer 500ms ===== */
    /* ---- OSTM count stop trigger register (TT) setting ---- */
    OSTM0.OSTMnTT = 0x01u;      /* Stop counting */

	OSTM0.OSTMnCTL = (OSTM0.OSTMnCTL & ~ 0x03) |
		0 * (1U << 1) |	// Interval Timer Mode
		1 * (1U << 0) |	// Enables the interrupts when counting starts.
		0;

	OSTM0.OSTMnCMP = calcdivround_p0clock(ticksfreq) - 1;

	{
		const uint16_t int_id = OSTMI0TINT_IRQn;
		r7s721_intc_registintfunc(int_id, r7s721_ostm0_interrupt);	/* ==== Register OS timer interrupt handler ==== */
		GIC_SetPriority(int_id, ARM_SYSTEM_PRIORITY);		/* ==== Set priority of OS timer interrupt to 5 ==== */
		GIC_EnableIRQ(int_id);		/* ==== Validate OS timer interrupt ==== */
	}

	OSTM0.OSTMnTS = 0x01u;      /* Start counting */


#else
	#error Undefined CPUSTYLE_XXX
#endif
}

void 
hardware_encoder_initialize(void)
{
#if WITHENCODER
	ENCODER_INITIALIZE();
#endif /* WITHENCODER */
}

/* Чтение состояния выходов валкодера #1 - в два младших бита */
/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */

uint_fast8_t 
hardware_get_encoder_bits(void)
{
#if WITHENCODER && defined (ENCODER_BITS) && defined (ENCODER_SHIFT)
	return (ENCODER_INPUT_PORT & ENCODER_BITS) >> ENCODER_SHIFT;	// Биты валкодера #1
#elif WITHENCODER && defined (ENCODER_BITS)
	const portholder_t v = ENCODER_INPUT_PORT;
	return ((v & ENCODER_BITA) != 0) * 2 + ((v & ENCODER_BITB) != 0);	// Биты идут не подряд
#else /* WITHENCODER */
	return 0;
#endif /* WITHENCODER */
}

/* Чтение состояния выходов валкодера #2 - в два младших бита */
/* Состояние фазы A - в бите с весом 2, фазы B - в бите с весом 1 */

uint_fast8_t 
hardware_get_encoder2_bits(void)
{
#if WITHENCODER && ENCODER2_BITS && defined (ENCODER2_SHIFT)
	return (ENCODER2_INPUT_PORT & ENCODER2_BITS) >> ENCODER2_SHIFT;	// Биты валкодера #2
#elif WITHENCODER && ENCODER2_BITS
	const portholder_t v = ENCODER2_INPUT_PORT;
	return ((v & ENCODER2_BITA) != 0) * 2 + ((v & ENCODER2_BITB) != 0);	// Биты идут не подряд
#else /* WITHENCODER */
	return 0;
#endif /* WITHENCODER */
}

// ADC intgerface functions

#if WITHCPUADCHW

//#define ADCINPUTS_COUNT (board_get_adcinputs())

static uint_fast8_t adc_input;


#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

void RAMFUNC_NONILINE ADC_Handler(void)
{
	//const unsigned long sr = ADC->ADC_ISR;	// ADC_IER_DRDY, ADC_ISR_DRDY
	(void) ADC->ADC_ISR;	// ADC_IER_DRDY, ADC_ISR_DRDY

	// ATSAM3Sxx считывает только 10 или 12 бит
	// Read the 8 most significant bits
	// of the AD conversion result
	board_adc_store_data(board_get_adcch(adc_input), ADC->ADC_LCDR & ADC_LCDR_LDATA_Msk);	// на этом цикле используем результат
	// Select next ADC input
	if (++ adc_input >= board_get_adcinputs())
	{
		board_adc_filtering();
		spool_adcdonebundle();
	}
	else
	{
		// Select next ADC input (only one)
		const portholder_t mask = ADC_CHER_CH0 << board_get_adcch(adc_input);
		ADC->ADC_CHER = mask; /* enable ADC */
		ADC->ADC_CHDR = ~ mask; /* disable ADC */
		ADC->ADC_CR = ADC_CR_START;	// Start the AD conversion
	}
}
	
#elif CPUSTYLE_AT91SAM7S

static RAMFUNC_NONILINE void AT91F_ADC_IRQHandler(void) 
{
	(void) AT91C_BASE_ADC->ADC_SR;
	// Read the 8 most significant bits
	// of the AD conversion result
	board_adc_store_data(board_get_adcch(adc_input), AT91C_BASE_ADC->ADC_LCDR & AT91C_ADC_LDATA);	// на этом цикле используем результат
	if (++ adc_input >= board_get_adcinputs())
	{
		board_adc_filtering();
		spool_adcdonebundle();
	}
	else
	{
		// Select next ADC input (only one)
		const portholder_t mask = AT91C_ADC_CH0 << board_get_adcch(adc_input);
		AT91C_BASE_ADC->ADC_CHDR = ~ mask; /* disable ADC inputs */
		AT91C_BASE_ADC->ADC_CHER = mask; /* enable ADC */
		AT91C_BASE_ADC->ADC_CR = AT91C_ADC_START;	// Start the AD conversion
	}
}

#elif CPUSTYLE_ATMEGA
	///////adc
	// получение кода выбора входа
	static uint_fast8_t hardware_atmega_admux(uint_fast8_t ch)
	{
		enum { ATMEGA_ADC_VREF_TYPE = ((0UL << REFS1) | (1UL << REFS0))	}; // AVCC used as reference volage
		#if HARDWARE_ADCBITS == 8
			return ch | ATMEGA_ADC_VREF_TYPE | (1UL << ADLAR);
		#else
			return ch | ATMEGA_ADC_VREF_TYPE;
		#endif
	}

	ISR(ADC_vect)
	{
		#if HARDWARE_ADCBITS == 8
			// Read the 8 most significant bits
			// of the AD conversion result
			board_adc_store_data(board_get_adcch(adc_input), ADCH);
		#else
			// Read the AD conversion result
			board_adc_store_data(board_get_adcch(adc_input), ADCW);
		#endif 
		// Select next ADC input
		if (++ adc_input >= board_get_adcinputs())
		{
			board_adc_filtering();
			spool_adcdonebundle();
		}
		else
		{
			// Select next ADC input
			ADMUX = hardware_atmega_admux(board_get_adcch(adc_input));
			ADCSRA |= (1U << ADSC);			// Start the AD conversion
		}
	}
#elif CPUSTYLE_ATXMEGAXXXA4

	#warning TODO: write atxmega code for ADC interrupt handler


	// adc
	ISR(ADCA_CH0_vect)
	{
			// на этом цикле используем результат
		#if HARDWARE_ADCBITS == 8
			// Select next ADC input
			// Read the 8 most significant bits
			// of the AD conversion result
			board_adc_store_data(board_get_adcch(adc_input), ADCA.CH0.RESH);
		#else
			// Read the AD conversion result
			board_adc_store_data(board_get_adcch(adc_input), ADCA.CH0.RESH * 256 + ADCA.CH0.RESL);
		#endif 
		// Select next ADC input
		if (++ adc_input >= board_get_adcinputs())
		{
			board_adc_filtering();
			spool_adcdonebundle();
		}
		else
		{
			// Select next ADC input
			ADCA.CH0.MUXCTRL = board_get_adcch(adc_input);
			ADCA.CH0.CTRL |= (1U << ADC_CH_START_bp);			// Start the AD conversion
		}
	}


#elif CPUSTYLE_STM32H7XX

// For SM32H7XXX: ADC_IRQn is a same vector as ADC1_2_IRQn (decimal 18)


/* Отображение номеров каналов АЦП процессоров STM32Fxxx в каналы процессора STM32H7 */
typedef struct adcinmap_tag
{
	uint_fast8_t ch;	// номер канала в периферийном блоке процессора
	ADC_TypeDef * adc;	// периферийный блок процессора
	ADC_Common_TypeDef * adccommon;
	uint_fast8_t thold_uS01;	// минимальное время выборки для данного канала - десятые доли микросекунды
} adcinmap_t;

static const adcinmap_t * getadcmap(uint_fast8_t adci)
{
	static const adcinmap_t adcinmaps [] =
	{
		{	16,	ADC1,	ADC12_COMMON,	15,	},	// @0:	PA0	ADC1_INP16 (PA0_C ADC12_INP0)
		{	17,	ADC1,	ADC12_COMMON,	15,	},	// @1:	PA1	ADC1_INP17 (PA1_C ADC12_INP1)
		{	14,	ADC1,	ADC12_COMMON,	15,	},	// @2:	PA2	ADC12_INP14
		{	15,	ADC1,	ADC12_COMMON,	15,	},	// @3:	PA3	ADC12_INP15
		{	18,	ADC1,	ADC12_COMMON,	15,	},	// @4:	PA4	ADC12_INP18
		{	19,	ADC1,	ADC12_COMMON,	15,	},	// @5:	PA5	ADC12_INP19
		{	3,	ADC1,	ADC12_COMMON,	15,	},	// @6:	PA6	ADC12_INP3
		{	7,	ADC1,	ADC12_COMMON,	15,	},	// @7:	PA7	ADC12_INP7
		{	9,	ADC1,	ADC12_COMMON,	15,	},	// @8:	PB0	ADC12_INP9
		{	5,	ADC1,	ADC12_COMMON,	15,	},	// @9:	PB1	ADC12_INP5
		{	10,	ADC1,	ADC12_COMMON,	15,	},	// @10:	PC0	ADC123_INP10
		{	11,	ADC1,	ADC12_COMMON,	15,	},	// @11:	PC1	ADC123_INP11
		{	12,	ADC1,	ADC12_COMMON,	15,	},	// @12:	PC2	ADC123_INP12 (PC2_C ADC3_INP0)
		{	13,	ADC1,	ADC12_COMMON,	15,	},	// @13:	PC3	ADC12_INP13 (PC3_C ADC3_INP1)
		{	4,	ADC1,	ADC12_COMMON,	15,	},	// @14:	PC4	ADC12_INP4
		{	8,	ADC1,	ADC12_COMMON,	15,	},	// @15:	PC5	ADC12_INP8
		{	18,	ADC3,	ADC3_COMMON,	90,	},	// @16:	Temperature sensor (VSENSE) - 9.0 uS required
		{	19,	ADC3,	ADC3_COMMON,	43,	},	// @17:	Reference voltage (VREFINT) - 4.3 uS required
	};

	ASSERT(adci < (sizeof adcinmaps / sizeof adcinmaps [0]));
	return & adcinmaps [adci];
}

static void 
ADCs_IRQHandler(ADC_TypeDef * p)
{
	ASSERT(adc_input < board_get_adcinputs());
	const adcinmap_t * const adcmap = getadcmap(board_get_adcch(adc_input));
	ADC_TypeDef * const adc = adcmap->adc;
	ASSERT(adc == p);
	ASSERT((adc->ISR & ADC_ISR_OVR) == 0);
	ASSERT((adc->CR & (ADC_CR_JADSTART | ADC_CR_ADSTART)) == 0);
	if ((adc->ISR & ADC_ISR_EOS) != 0)
	{
		adc->ISR = ADC_ISR_EOS;		// EOS (end of regular sequence) flag
		board_adc_store_data(board_get_adcch(adc_input), (adc->DR & ADC_DR_RDATA) >> ADC_DR_RDATA_Pos);	// на этом цикле используем результат
		// Select next ADC input
		if (++ adc_input >= board_get_adcinputs())
		{
			board_adc_filtering();
			spool_adcdonebundle();
		}
		else
		{
			// Установить следующий вход (блок ADC может измениться)
			const adcinmap_t * const adcmap = getadcmap(board_get_adcch(adc_input));
			ADC_TypeDef * const adc = adcmap->adc;

			ASSERT((adc->CR & (ADC_CR_JADSTART | ADC_CR_ADSTART)) == 0);
			adc->SQR1 = (adc->SQR1 & ~ (ADC_SQR1_L | ADC_SQR1_SQ1)) |
				0 * ADC_SQR1_L_0 |	// Выбираем преобразование с одного канала. Сканирования нет.
				adcmap->ch * ADC_SQR1_SQ1_0 |
				0;
			adc->CR |= ADC_CR_ADSTART;	// Запуск преобразования
		}
	}
	else
	{
		ASSERT(0);
	}
}

void 
ADC_IRQHandler(void)
{
	ADCs_IRQHandler(ADC1);
}

void 
ADC3_IRQHandler(void)
{
	ADCs_IRQHandler(ADC3);
}

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

void 
ADC1_2_IRQHandler(void)
{
	ASSERT(adc_input < board_get_adcinputs());
	//const unsigned long sr = ADC1->SR;
	ADC1->SR = 0;		// Сбрасываем все запросы прерывания.
	board_adc_store_data(board_get_adcch(adc_input), ADC1->DR & ADC_DR_DATA);	// на этом цикле используем результат
	// Select next ADC input
	if (++ adc_input >= board_get_adcinputs())
	{
		board_adc_filtering();
		spool_adcdonebundle();
	}
	else
	{
		// Установить следующий вход
		ADC1->SQR3 = (ADC1->SQR3 & ~ ADC_SQR3_SQ1) | (ADC_SQR3_SQ1_0 * board_get_adcch(adc_input)); 
		#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX
		ADC1->CR2 |= ADC_CR2_SWSTART;	// !!!!
		#endif
	}
}

#elif CPUSTYLE_STM32F0XX
	#if STM32F0XX_MD
		void 
		ADC1_COMP_IRQHandler(void)
		{
			ASSERT(adc_input < board_get_adcinputs());
			board_adc_store_data(board_get_adcch(adc_input), ADC1->DR & ADC_DR_DATA);	// используем результат
			ADC1->ISR = ADC_ISR_EOC;
			ADC1->CHSELR = 1UL <<  board_get_adcch(adc_input);
			// Select next ADC input
			if (++ adc_input >= board_get_adcinputs())
			{
				board_adc_filtering();
				spool_adcdonebundle();
			}
			else
			{
				// Установить следующий вход
				ADC1->CHSELR = 1UL <<  board_get_adcch(adc_input);
				ADC1->CR = ADC_CR_ADSTART;	// ADC Start of Regular conversion
			}
		}
	#else /* STM32F0XX_MD */
		void 
		ADC1_IRQHandler(void)
		{
			ASSERT(adc_input < board_get_adcinputs());
			board_adc_store_data(board_get_adcch(adc_input), ADC1->DR & ADC_DR_DATA);	// используем результат
			ADC1->ISR = ADC_ISR_EOC;
			ADC1->CHSELR = 1UL <<  board_get_adcch(adc_input);
			// Select next ADC input
			if (++ adc_input >= board_get_adcinputs())
			{
				board_adc_filtering();
				spool_adcdonebundle();
			}
			else
			{
				// Установить следующий вход
				ADC1->CHSELR = 1UL <<  board_get_adcch(adc_input);
				ADC1->CR = ADC_CR_ADSTART;	// ADC Start of Regular conversion
			}
		}
	#endif /* STM32F0XX_MD */

#elif CPUSTYLE_STM32L0XX

	void 
	ADC1_COMP_IRQHandler(void)
	{
		ASSERT(adc_input < board_get_adcinputs());
		board_adc_store_data(board_get_adcch(adc_input), ADC1->DR & ADC_DR_DATA);	// используем результат
		ADC1->ISR = ADC_ISR_EOC;
		ADC1->CHSELR |= 1UL <<  board_get_adcch(adc_input);
		// Select next ADC input
		if (++ adc_input >= board_get_adcinputs())
		{
			board_adc_filtering();
			spool_adcdonebundle();
		}
		else
		{
			// Установить следующий вход
			ADC1->CHSELR |= 1UL <<  board_get_adcch(adc_input);
			ADC1->CR = ADC_CR_ADSTART;	// ADC Start of Regular conversion
		}
	}

#elif CPUSTYLE_STM32F30X

void 
ADC1_2_IRQHandler(void)
{
	ASSERT(adc_input < board_get_adcinputs());
	//const unsigned long sr = ADC1->SR;
	ADC1->ISR = 0;		// Сбрасываем все запросы прерывания.
	board_adc_store_data(board_get_adcch(adc_input), ADC1->DR & ADC_DR_RDATA);	// на этом цикле используем результат
	// Select next ADC input
	if (++ adc_input >= board_get_adcinputs())
	{
		board_adc_filtering();
		spool_adcdonebundle();
	}
	else
	{
		// Установить следующий вход
		ADC1->SQR1 = (ADC1->SQR1 & ~ ADC_SQR1_SQ1) | (ADC_SQR1_SQ1_0 * board_get_adcch(adc_input)); 
		ADC1->CR |= ADC_CR_ADSTART;	// ADC Start of Regular conversion
	}
}

#elif CPUSTYLE_R7S721

static void RAMFUNC_NONILINE 
r7s721_adi_irq_handler(void)
{

	//dbg_putchar('.');
	//dbg_putchar('0' + adc_input);
	//dbg_putchar('0' + board_get_adcch(adc_input));

	//dbg_putchar(' ');
	/*
	static const volatile uint16_t * const adcports [HARDWARE_ADCINPUTS] =
	{
		& ADC.ADDRA,
		& ADC.ADDRB,
		& ADC.ADDRC,
		& ADC.ADDRD,
		& ADC.ADDRE,
		& ADC.ADDRF,
		& ADC.ADDRG,
		& ADC.ADDRH,
	};
	*/
	ASSERT(adc_input < board_get_adcinputs());
	//const unsigned long sr = ADC1->SR;
	////ADC1->ISR = 0;		// Сбрасываем все запросы прерывания.
	board_adc_store_data(board_get_adcch(adc_input), (& ADC.ADDRA) [board_get_adcch(adc_input)] >> 4);	// на этом цикле используем результат
	// Select next ADC input
	if (++ adc_input >= board_get_adcinputs())
	{
		ADC.ADCSR &= ~ (1uL << 15);	// ADF: A/D end flag - Cleared by reading ADF while ADF = 1, then writing 0 to ADF

		//dbg_putchar('L');
		// Это был последний вход
		board_adc_filtering();
		spool_adcdonebundle();
	}
	else
	{
		// Установить следующий вход
		ADC.ADCSR = (ADC.ADCSR & ~ ((1uL << 15) | 0x07)) | 
			board_get_adcch(adc_input) |	// канал для преобразования
			1 * ADC_ADCSR_ADST |	// ADST: Single mode: A/D conversion starts
			0;
	}
}

#else
	#error No CPUSTYLE_XXXXX defined
#endif

// Получение битов используемых каналов АЦП (до шестнадцати штук).
static portholder_t
build_adc_mask(void)
{
	portholder_t mask = 0;
	uint_fast8_t i;

	for (i = 0; i < board_get_adcinputs(); ++ i)
	{
		mask |= (portholder_t) 1 << board_get_adcch(i);	// ADC_CHER_CH0 или AT91C_ADC_CH0
	}
	return mask;
}

#if 0
	// Set up ADCB0 on PB0 to read temp sensor. More of this can be achieved by using driver from appnote AVR1300
	PORTQ.PIN2CTRL = (PORTQ.PIN2CTRL & ~PORT_OPC_gm) | PORT_OPC_PULLDOWN_gc;	// This pin must be grounded to "enable" NTC-resistor
	PORTB.DIRCLR = PIN0;
	PORTB.PIN0CTRL = (PORTB.PIN0CTRL & ~PORT_OPC_gm);

	ADC_CalibrationValues_Load(&ADCB);  // Load factory calibration data for ADC
	ADCB.CH0.CTRL = (ADCB.CH0.CTRL & ~ADC_CH_INPUTMODE_gm) | ADC_CH_INPUTMODE_SINGLEENDED_gc; // Single ended input
	ADCB.CH0.MUXCTRL = (ADCB.CH0.MUXCTRL & ~ADC_CH_MUXPOS_gm) | ADC_CH_MUXPOS_PIN0_gc; // Pin 0 is input	
	ADCB.REFCTRL = (ADCB.REFCTRL & ~ADC_REFSEL_gm) | ADC_REFSEL_VCC_gc;	// Internal AVCC/1.6 as reference

	ADCB.CTRLB |= ADC_FREERUN_bm; // Free running mode
	ADCB.PRESCALER = (ADCB.PRESCALER & ~ADC_PRESCALER_gm) | ADC_PRESCALER_DIV512_gc; // Divide clock by 1024.
	ADCB.CTRLB = (ADCB.CTRLB & ~ADC_RESOLUTION_gm) | ADC_RESOLUTION_8BIT_gc; // Set 8 bit resolution
	ADCB.CTRLA |= ADC_ENABLE_bm; // Enable ADC
	
#endif

void hardware_adc_initialize(void)
{
	//debug_printf_P(PSTR("hardware_adc_initialize\n"));

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

    PMC->PMC_PCER0 = (1UL << ID_ADC);		// разрешить тактовую для ADC
	ADC->ADC_CR = ADC_CR_SWRST;	// reset ADC
	/* программирование канала PDC, связанного с ADC */

	//ADC->ADC_PTCR = PERIPH_PTCR_RXTDIS | PERIPH_PTCR_TXTDIS;

	// ADC characteristics:
	// ADC clock frequency: 1..20 MHz
	// Sampling Frequency - 1 MHz
	// ADC Startup time:
	//   From Standby Mode to Normal Mode: 4/8/12 uS
	//   From OFF Mode to Normal Mode: 20/30/40 uS
	// Track and Hold Time Time - >= 160 nS (12 bit mode - 205 nS)

	
	unsigned prescal;
	calcdivider(calcdivround(ADC_FREQ), ATSAM3S_ADC_PRESCAL_WIDTH, ATSAM3S_ADC_PRESCAL_TAPS, & prescal, 1);
	// Settling time to change offset and gain
	const unsigned long tADCnS = (1000000000UL + (ADC_FREQ / 2)) / ADC_FREQ;	// Количество наносекунд в периоде частоты ADC_FREQ
    const unsigned int tracktime = ulmin(16, ulmax(1, (205 + (tADCnS / 2)) / (tADCnS == 0 ? 1 : tADCnS)));

	ADC->ADC_MR = 
		ADC_MR_SETTLING_AST17 |		// Settling time to change offset and gain
		(ADC_MR_TRACKTIM(tracktime - 1)) |		// SampleAndHoldTime
		(ADC_MR_PRESCAL(prescal)) |	
		//ADC_MR_TRANSFER(0) |
	#if HARDWARE_ADCBITS == 10
		ADC_MR_LOWRES_BITS_10 |	// Канал PDC при этом передаёт по два байта
	#elif HARDWARE_ADCBITS == 12
		ADC_MR_LOWRES_BITS_12 |	// Канал PDC при этом передаёт по два байта
	#else
		#error Wrong HARDWARE_ADCBITS value
	#endif
		ADC_MR_TRGEN_DIS	| // запуск переобразования от команд
		0;

	/* какие из каналов включать.. */
	ADC->ADC_IER = ADC_IER_DRDY;	/* прерывание после завершения очередного преобразования. */

	////ADC->ADC_CR = ADC_CR_START;	// start ADC
	////while ((ADC->ADC_ISR & ADC_ISR_DRDY) == 0)
	////	;

	NVIC_SetPriority(ADC_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(ADC_IRQn);		// enable ADC_Handler();

#elif CPUSTYLE_AT91SAM7S

	AT91C_BASE_PMC->PMC_PCER = (1UL << AT91C_ID_ADC);		// разрешить тактовую для ADC
	AT91C_BASE_ADC->ADC_CR = AT91C_ADC_SWRST;	// reset ADC

	// adc characteristics: in 10 bit mode ADC clock max is 5 MHz, in 8 bit mode - 8 MHz
	// Track and Hold Acquisition Time - 600 nS

	unsigned prescal;
	calcdivider(calcdivround(ADC_FREQ), AT91SAM7_ADC_PRESCAL_WIDTH, AT91SAM7_ADC_PRESCAL_TAPS, & prescal, 1);
	const unsigned long tADCnS = (1000000000UL + (ADC_FREQ / 2)) / ADC_FREQ;	// Количество наносекунд в периоде частоты ADC_FREQ
    const unsigned int shtm = ulmin(15, ulmax(0, (600 + (tADCnS / 2)) / (tADCnS == 0 ? 1 : tADCnS)));

	AT91C_BASE_ADC->ADC_MR = 
		(AT91C_ADC_SHTIM & (shtm << 24)) |		// SampleAndHoldTime - up to 15
		(AT91C_ADC_PRESCAL & ((prescal) << 8)) |	// up to 1023	- mck 
	#if HARDWARE_ADCBITS == 10
		AT91C_ADC_LOWRES_10_BIT |	// Канал PDC при этом передаёт по два байта
	#elif HARDWARE_ADCBITS == 8
		AT91C_ADC_LOWRES_8_BIT |	// Канал PDC при этом передаёт по байту
	#else
		#error Wrong HARDWARE_ADCBITS value
	#endif
		0;

	AT91C_BASE_ADC->ADC_IER = AT91C_ADC_DRDY;	/* прерывание после завершения очередного преобразования. */

	// programming interrupts from ADC
	AT91C_BASE_AIC->AIC_IDCR = (1UL << AT91C_ID_ADC);
	AT91C_BASE_AIC->AIC_SVR [AT91C_ID_ADC] = (AT91_REG) AT91F_ADC_IRQHandler;
	AT91C_BASE_AIC->AIC_SMR [AT91C_ID_ADC] = 
		(AT91C_AIC_SRCTYPE & AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL) |
		(AT91C_AIC_PRIOR & AT91C_AIC_PRIOR_LOWEST);
	AT91C_BASE_AIC->AIC_ICCR = (1UL << AT91C_ID_ADC);		// clear pending interrupt
	AT91C_BASE_AIC->AIC_IECR = (1UL << AT91C_ID_ADC);	// enable inerrupt

#elif CPUSTYLE_ATMEGA

	// Использование автоматического расчёта предделителя
	// Хотя, 128 (prei = 6) годится для всех частот - 8 МГц и выше. Ниже - уменьшаем.
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(ADC_FREQ), ATMEGA_ADPS_WIDTH, ATMEGA_ADPS_TAPS, & value, 1);

	#if CPUSTYLE_ATMEGA_XXX4

		DIDR0 = build_adc_mask();	// запретить цифровые входы на входах АЦП
		ADCSRA = (1U << ADEN) | (1U << ADIE ) | prei;

	#else /* CPUSTYLE_ATMEGA_XXX4 */

		ADCSRA = (1U << ADEN) | (1U << ADIE ) | prei;


	#endif	/* CPUSTYLE_ATMEGA_XXX4 */

#elif CPUSTYLE_ATXMEGAXXXA4

	#warning TODO: write atxmega code - ADC init
	// Использование автоматического расчёта предделителя
	// Хотя, 128 (prei = 6) годится для всех частот - 8 МГц и выше. Ниже - уменьшаем.
	////unsigned value;
	////const uint_fast8_t prei = calcdivider(calcdivround(ADC_FREQ), ATXMEGA_ADPS_WIDTH, ATXMEGA_ADPS_TAPS, & value, 1);

	////ADCA.PRESCALER = prei;
	//DIDR0 = build_adc_mask();	// запретить цифровые входы на входах АЦП
	//ADCSRA = (1U << ADEN) | (1U << ADIE );

#if 0
		// код из electronix.ru
	/* инициализация АЦП */
	ADCA.CTRLA = 0x05;
	ADCA.CTRLB = 0x00;
	ADCA.PRESCALER = ADC_PRESCALER_DIV256_gc; // 0x6; // ADC_PRESCALER_DIV256_gc
	ADCA.CH1.CTRL =	ADC_CH_INPUTMODE_SINGLEENDED_gc; // 0x01;


	uint16_t get_adc()
	{	
		ADCA.REFCTRL |= (1U << 5); // подаем смещение с пина AREFA	
		ADCA.CH1.MUXCTRL =	0x20; // выбираем ножку 	
		ADCA.CH1.CTRL =	(ADC_CH_START_bm | ADC_CH_INPUTMODE_SINGLEENDED_gc); //0x81;	//запускаем преобразавние	
		_delay_us(30);	
		return ADCA.CH1.RES;
	}
#endif

#elif CPUSTYLE_STM32F1XX

	const uint_fast32_t ainmask = build_adc_mask();
	HARDWARE_ADC_INITIALIZE(ainmask);

	//Initialization ADC. PortC.0 ADC 10  
	RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_ADCPRE)) | RCC_CFGR_ADCPRE_DIV4; // RCC_CFGR_ADCPRE_DIV2;    //  

	#if defined (ADC2)

		RCC->APB2ENR |= (RCC_APB2ENR_ADC1EN | RCC_APB2ENR_ADC2EN);    // Затактировали АЦП  
		__DSB();
		NVIC_SetPriority(ADC1_2_IRQn, ARM_SYSTEM_PRIORITY);
		NVIC_EnableIRQ(ADC1_2_IRQn);    //Включаем прерывания с АЦП. Обрабатывает ADC1_2_IRQHandler() 

	#else /* defined (ADC2) */

		RCC->APB2ENR |= (RCC_APB2ENR_ADC1EN);    // Затактировали АЦП  
		__DSB();
		NVIC_SetPriority(ADC1_IRQn, ARM_SYSTEM_PRIORITY);
		NVIC_EnableIRQ(ADC1_IRQn);    //Включаем прерывания с АЦП. Обрабатывает ADC1_2_IRQHandler() 

	#endif /* defined (ADC2) */


	ADC1->CR2 &= ~ ADC_CR2_CONT;     //Сбрасываем бит. Включение одиночных преобразований.  
	ADC1->SQR1 &= ~ ADC_SQR1_L;     //Выбираем преобразование с одного канала. Сканирования нет.  

	#if WITHREFSENSOR || WITHTEMPSENSOR
		ADC1->CR2 |= ADC_CR2_TSVREFE;
	#endif /* WITHREFSENSOR || WITHTEMPSENSOR */



	// установка врмени выборки для данного канала
	//const uint_fast32_t cycles = NTICKSADC01(adcmap->thold_uS01);	// в десятых долях микросекунды
	const uint_fast8_t t = 4;	// 0..7
	/* Устанавливаем sample time одинаковое на всех каналах 
		Значения и время выборки
		0: 1.5 cycles
		1: 7.5 cycles
		2: 13.5 cycles
		3: 28.5 cycles
		4: 41.5 cycles
		5: 55.5 cycles
		6: 71.5 cycles
		7: 239.5 cycles
	*/
	// 9..0 ch
	ADC1->SMPR2 =
		(ADC1->SMPR2 & ~ (ADC_SMPR2_SMP9 | ADC_SMPR2_SMP8 | ADC_SMPR2_SMP7 | ADC_SMPR2_SMP6 | 
						ADC_SMPR2_SMP5 | ADC_SMPR2_SMP4 | ADC_SMPR2_SMP3 | 
						ADC_SMPR2_SMP2 | ADC_SMPR2_SMP1 | ADC_SMPR2_SMP0)
				) |
		t * (
			ADC_SMPR2_SMP9_0 | ADC_SMPR2_SMP8_0 | ADC_SMPR2_SMP7_0 | ADC_SMPR2_SMP6_0 |
			ADC_SMPR2_SMP5_0 | ADC_SMPR2_SMP4_0 | ADC_SMPR2_SMP3_0 | 
			ADC_SMPR2_SMP2_0 | ADC_SMPR2_SMP1_0 | ADC_SMPR2_SMP0_0) |
		0;

	// 17..10 ch
	ADC1->SMPR1 = 
		(ADC1->SMPR1 & ~ (ADC_SMPR1_SMP17 | ADC_SMPR1_SMP16 | ADC_SMPR1_SMP15 | 
						ADC_SMPR1_SMP14 | ADC_SMPR1_SMP13 | ADC_SMPR1_SMP12 | 
						ADC_SMPR1_SMP11 | ADC_SMPR1_SMP10)
				) |
		t * (
			ADC_SMPR1_SMP17_0 | ADC_SMPR1_SMP16_0 | ADC_SMPR1_SMP15_0 | 
			ADC_SMPR1_SMP14_0 | ADC_SMPR1_SMP13_0 | ADC_SMPR1_SMP12_0 | 
			ADC_SMPR1_SMP11_0 | ADC_SMPR1_SMP10_0) |
		0;

	ADC1->CR1 |= ADC_CR1_EOCIE;    //Включили прерываия при окончании преобразования.  

	//	ADC1->CR2 |= ADC_CR2_TSVREFE;	// для тестов

	ADC1->CR2 |= ADC_CR2_CAL; //Запуск калибровки АЦП
	while ((ADC1->CR2 & ADC_CR2_CAL) == 0)
		; //Ожидаем окончания калибровки

	#if defined (ADC2)
		ADC2->CR2 |= ADC_CR2_CAL; //Запуск калибровки АЦП
		while ((ADC2->CR2 & ADC_CR2_CAL) == 0)
			; //Ожидаем окончания калибровки
	#endif /* defined (ADC2) */

#elif CPUSTYLE_STM32H7XX

	const uint_fast32_t ainmask = build_adc_mask();
	HARDWARE_ADC_INITIALIZE(ainmask);

	RCC->AHB1ENR |= RCC_AHB1ENR_ADC12EN;	// Затактировали АЦП  
	__DSB();
	RCC->AHB4ENR |= RCC_AHB4ENR_ADC3EN;		// Затактировали АЦП  
	__DSB();
	#if WITHREFSENSOR
		RCC->APB4ENR |= RCC_APB4ENR_VREFEN;		// Затактировали источник опорного напрядения (для нормирования значений с АЦП)  
	__DSB();
	#endif /* WITHREFSENSOR */

	// расчет делителя для тактирования АЦП
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_hsi48(ADC_FREQ), 0, (512 | 256 | 128 | 64 | 32 | 16 | 8 | 4 | 2), & value, 0);
	static const uint_fast8_t presc [] = 
	{
		0x00,	// 0000: input ADC clock not divided
		0x01,	// 0001: input ADC clock divided by 2
		0x02,	// 0010: input ADC clock divided by 4
		0x04,	// 0100: input ADC clock divided by 8
		0x07,	// 0111: input ADC clock divided by 16
		0x08,	// 1000: input ADC clock divided by 32
		0x09,	// 1001: input ADC clock divided by 64
		0x0A,	// 1010: input ADC clock divided by 128
		0x0B,	// 1011: input ADC clock divided by 256
	};
	uint_fast8_t i;
	//debug_printf_P(PSTR("hardware_adc_initialize: prei=%u, presc=0x%02X\n"), prei, presc [prei]);

	// Power-on ADCs
	for (i = 0; i < board_get_adcinputs(); ++ i)
	{
		const uint_fast8_t ainp = board_get_adcch(i);
		const adcinmap_t * const adcmap = getadcmap(ainp);
		ADC_TypeDef * const adc = adcmap->adc;

		if ((adc->CR & ADC_CR_ADVREGEN) != 0)
			continue;

		adc->CR &= ~ ADC_CR_DEEPPWD;	// Disable deep sleep ADC mode
		adc->CR |= ADC_CR_ADVREGEN;		// Enable ADC regulator

	}
	local_delay_ms(5); // Wait for regulator is on

	// Initialize ADCs
	for (i = 0; i < board_get_adcinputs(); ++ i)
	{
		const uint_fast8_t ainp = board_get_adcch(i);
		const adcinmap_t * const adcmap = getadcmap(ainp);
		ADC_TypeDef * const adc = adcmap->adc;


		adcmap->adccommon->CCR = 
			(adcmap->adccommon->CCR & ~ (ADC_CCR_PRESC | ADC_CCR_CKMODE | ADC_CCR_VREFEN | ADC_CCR_TSEN)) |
			(presc [prei] << ADC_CCR_PRESC_Pos) |
			(0 << ADC_CCR_CKMODE_Pos) |
		#if WITHREFSENSOR
			(1 << ADC_CCR_VREFEN_Pos) | 
		#endif /* WITHREFSENSOR */
		#if WITHTEMPSENSOR
			(1 << ADC_CCR_TSEN_Pos) | 
		#endif /* WITHTEMPSENSOR */
			0;

		adc->PCSEL |= ((1U << adcmap->ch) << ADC_PCSEL_PCSEL_Pos) & ADC_PCSEL_PCSEL_Msk;

		// установка врмени выборки для данного канала
		const uint_fast32_t cycles = NTICKSADC01(adcmap->thold_uS01);	// в десятых долях микросекунды
		uint_fast8_t tcode;
		if (cycles >= 8105)	// 810.5 clocks
			tcode = 7;
		else if (cycles >= 3875)	// 387.5 clocks
			tcode = 6;
		else if (cycles >= 645)
			tcode = 5;
		else if (cycles >= 325)
			tcode = 4;
		else if (cycles >= 165)
			tcode = 3;
		else if (cycles >= 85)
			tcode = 2;
		else if (cycles >= 25)	// 2.5 clocks
			tcode = 1;
		else 
			tcode = 0;

		++ tcode;

		//debug_printf_P(PSTR("ch=%u, time=%u, cycles=%u, tcode=%u\n"), adcmap->ch, adcmap->thold_uS01, cycles, tcode);
		if (adcmap->ch < 10)
		{
			uint_fast8_t shift = adcmap->ch * 3;
			adc->SMPR1 = (adc->SMPR1 & ~ (ADC_SMPR1_SMP0 << shift)) |
				((tcode * ADC_SMPR1_SMP0_0) << shift) |
				0;
		}
		else
		{
			uint_fast8_t shift = (adcmap->ch - 10) * 3;
			adc->SMPR2 = (adc->SMPR2 & ~ (ADC_SMPR2_SMP10 << shift)) |
				((tcode * ADC_SMPR2_SMP10_0) << shift) |
				0;
		}

		adc->CFGR = (adc->CFGR & ~ (ADC_CFGR_RES)) |
		#if HARDWARE_ADCBITS == 8
			(4 << ADC_CFGR_RES_Pos) |	// ADC 8-bit resolution
		#elif HARDWARE_ADCBITS == 10
			(3 << ADC_CFGR_RES_Pos) |	// ADC 10-bit resolution
		#elif HARDWARE_ADCBITS == 12
			(2 << ADC_CFGR_RES_Pos) |	// ADC 12-bit resolution
		#elif HARDWARE_ADCBITS == 14
			(1 << ADC_CFGR_RES_Pos) |	// ADC 16-bit resolution
		#elif HARDWARE_ADCBITS == 16
			(0 << ADC_CFGR_RES_Pos) |	// ADC 16-bit resolution
		#else
			#error Wrong HARDWARE_ADCBITS parameter
		#endif
			0;

		adc->IER = ADC_IER_EOS;		// EOS (end of regular sequence) flag
	}

	// Set ADC_CR_ADEN
	for (i = 0; i < board_get_adcinputs(); ++ i)
	{
		const uint_fast8_t ainp = board_get_adcch(i);
		const adcinmap_t * const adcmap = getadcmap(ainp);
		ADC_TypeDef * const adc = adcmap->adc;

		if ((adc->ISR & ADC_ISR_ADRD) != 0)
			continue;	// already enabled

		adc->CR |= ADC_CR_ADEN;

		// Wait for  ADC_ISR_ADRD
		while ((adc->ISR & ADC_ISR_ADRD) == 0)
			;
	}

	NVIC_SetPriority(ADC_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(ADC_IRQn);    // Включаем прерывания с АЦП. Обрабатывает ADC_IRQHandler() 
	NVIC_SetPriority(ADC3_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(ADC3_IRQn);    // Включаем прерывания с АЦП. Обрабатывает ADC3_IRQHandler() 

	// первый запуск производится в hardware_adc_startonescan().
	// А здесь всё...

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX

	const uint_fast32_t ainmask = build_adc_mask();
	HARDWARE_ADC_INITIALIZE(ainmask);
	// Initialization ADC

#if defined (RCC_APB2ENR_ADC2EN)
	RCC->APB2ENR |= (RCC_APB2ENR_ADC1EN | RCC_APB2ENR_ADC2EN /* | RCC_APB2ENR_ADC3EN */);    //Затактировали АЦП  
#else
	RCC->APB2ENR |= (RCC_APB2ENR_ADC1EN /* | RCC_APB2ENR_ADC2EN | RCC_APB2ENR_ADC3EN */);    //Затактировали АЦП  
#endif
	__DSB();

	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_pclk2(ADC_FREQ), 0, (8 | 4 | 2), & value, 0);
	// STM32F767
	// STM32F429
	static const uint_fast8_t presc [] = 
	{
		0x00,	// 00: PCLK2 divided by 2
		0x01,	// 01: PCLK2 divided by 4
		0x03,	// 11: PCLK2 divided by 8
	};
	const uint_fast32_t cycles = NTICKSADC01(10);	// в десятых долях микросекунды
	debug_printf_P(PSTR("hardware_adc_initialize: prei=%u, presc=0x%02X, cycles=%u\n"), prei, presc [prei], cycles);

	ADC->CCR = (ADC->CCR & ~ (ADC_CCR_ADCPRE | ADC_CCR_TSVREFE)) | 
		presc [prei] * ADC_CCR_ADCPRE_0 |
	#if WITHREFSENSOR || WITHTEMPSENSOR
		ADC_CCR_TSVREFE |
	#endif /* WITHREFSENSOR || WITHTEMPSENSOR */
		0;
	

	NVIC_SetPriority(ADC_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(ADC_IRQn);    //Включаем прерывания с АЦП. Обрабатывает ADC_IRQHandler() 

	ADC1->CR2 &= ~ ADC_CR2_CONT;     //Сбрасываем бит. Включение одиночных преобразований.  
	ADC1->SQR1 &= ~ ADC_SQR1_L;     //Выбираем преобразование с одного канала. Сканирования нет.  


	/* Устанавливаем sample time одинаковое на всех каналах 
		Значения и время выборки
		0: 1.5 cycles
		1: 7.5 cycles
		2: 13.5 cycles
		3: 28.5 cycles
		4: 41.5 cycles
		5: 55.5 cycles
		6: 71.5 cycles
		7: 239.5 cycles
	*/

	/* F7
			Note: 
			000: 3 cycles
			001: 15 cycles
			010: 28 cycles
			011: 56 cycles
			100: 84 cycles
			101: 112 cycles
			110: 144 cycles
			111: 480 cycles
	*/
	// установка врмени выборки для данного канала
	//const uint_fast32_t cycles = NTICKSADC01(adcmap->thold_uS01);	// в десятых долях микросекунды
	// The temperature sensor is internally connected to the same input channel as VBAT, ADC1_IN18,
#if defined(STM32F767xx)
	// sampling time = 100nS .. 16uS
	const uint_fast8_t tts = 3;	// 000: 3 cycles, 001: 15 cycles 010: 28 cycles
	const uint_fast8_t ts = 3;	// 000: 3 cycles, 001: 15 cycles 010: 28 cycles
#else
	const uint_fast8_t tts = 3;	// 0..7
	const uint_fast8_t ts = 3;	// 0..7
#endif

	// 9..0 ch
	ADC1->SMPR2 =
		(ADC1->SMPR2 & ~ (ADC_SMPR2_SMP9 | ADC_SMPR2_SMP8 | ADC_SMPR2_SMP7 | ADC_SMPR2_SMP6 | 
						ADC_SMPR2_SMP5 | ADC_SMPR2_SMP4 | ADC_SMPR2_SMP3 | 
						ADC_SMPR2_SMP2 | ADC_SMPR2_SMP1 | ADC_SMPR2_SMP0)
				) |
		ts * (
			ADC_SMPR2_SMP9_0 | ADC_SMPR2_SMP8_0 | ADC_SMPR2_SMP7_0 | ADC_SMPR2_SMP6_0 |
			ADC_SMPR2_SMP5_0 | ADC_SMPR2_SMP4_0 | ADC_SMPR2_SMP3_0 | ADC_SMPR2_SMP2_0 | 
			ADC_SMPR2_SMP1_0 | ADC_SMPR2_SMP0_0
			) |
		0;

	// 18..10 ch
	ADC1->SMPR1 = 
		(ADC1->SMPR1 & ~ (ADC_SMPR1_SMP18 | ADC_SMPR1_SMP17 | ADC_SMPR1_SMP16 | ADC_SMPR1_SMP15 | 
						ADC_SMPR1_SMP14 | ADC_SMPR1_SMP13 | ADC_SMPR1_SMP12 | 
						ADC_SMPR1_SMP11 | ADC_SMPR1_SMP10)
				) |
		ts * (
			ADC_SMPR1_SMP18_0 | ADC_SMPR1_SMP17_0 | ADC_SMPR1_SMP16_0 | ADC_SMPR1_SMP15_0 | ADC_SMPR1_SMP14_0 | 
			ADC_SMPR1_SMP13_0 | ADC_SMPR1_SMP12_0 | ADC_SMPR1_SMP11_0 | ADC_SMPR1_SMP10_0
			) |
		0;

	ADC1->SMPR1 = (ADC1->SMPR1 & ~ (ADC_SMPR1_SMP18 | ADC_SMPR1_SMP17)) |
		tts * ADC_SMPR1_SMP17_0 |		// VREF
		tts * ADC_SMPR1_SMP18_0 |		// TEMPERATURE SENSOR
		0;

	ADC1->CR1 = (ADC1->CR1 & ~ (ADC_CR1_RES | ADC_CR1_EOCIE)) |
		ADC_CR1_EOCIE |   //Включили прерываия при окончании преобразования.  
	#if HARDWARE_ADCBITS == 6
		(3 << ADC_CR1_RES_Pos) |	// ADC 6-bit resolution
	#elif HARDWARE_ADCBITS == 8
		(2 << ADC_CR1_RES_Pos) |	// ADC 8-bit resolution
	#elif HARDWARE_ADCBITS == 10
		(1 << ADC_CR1_RES_Pos) |	// ADC 10-bit resolution
	#elif HARDWARE_ADCBITS == 12
		(0 << ADC_CR1_RES_Pos) |	// ADC 12-bit resolution
	#else
		#error Wrong HARDWARE_ADCBITS parameter
	#endif
		0;

	ADC1->CR2 |= ADC_CR2_ADON;	// Запуск преобразования
	local_delay_us(10);		// tSTAB

	// первый запуск производится в hardware_adc_startonescan().
	// А здесь всё...

#elif CPUSTYLE_STM32F30X

	#warning TODO: Verify code for STM32F30X support

	const uint_fast32_t ainmask = build_adc_mask();
	HARDWARE_ADC_INITIALIZE(ainmask);

	// Initialization ADC. 
	RCC->CFGR2 = (RCC->CFGR2 & ~ (RCC_CFGR2_ADCPRE12)) | RCC_CFGR2_ADCPRE12_DIV4; // RCC_CFGR_ADCPRE12_DIV2;    //  
	//RCC->APB2ENR |= (RCC_APB2ENR_ADC1EN | RCC_APB2ENR_ADC2EN);    //Затактировали АЦП  


	NVIC_SetPriority(ADC1_2_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(ADC1_2_IRQn);    //Включаем прерывания с АЦП. Обрабатывает ADC1_2_IRQHandler() 

	ADC1->CFGR &= ~ ADC_CFGR_CONT;     //Сбрасываем бит. Включение одиночных преобразований.  
	ADC1->SQR1 &= ~ ADC_SQR1_L;     //Выбираем преобразование с одного канала. Сканирования нет.  


	/* Устанавливаем sample time одинаковое на всех каналах 
		Значения и время выборки
		0: 1.5 cycles
		1: 7.5 cycles
		2: 13.5 cycles
		3: 28.5 cycles
		4: 41.5 cycles
		5: 55.5 cycles
		6: 71.5 cycles
		7: 239.5 cycles
	*/

	const uint_fast8_t t = 4;	// 0..7
	// The temperature sensor is internally connected to the same input channel as VBAT, ADC1_IN18,
	// 18..10 ch
	ADC1->SMPR2 = 
		(ADC1->SMPR2 & ~ (ADC_SMPR2_SMP18 | ADC_SMPR2_SMP17 | ADC_SMPR2_SMP16 | ADC_SMPR2_SMP15 | 
						ADC_SMPR2_SMP14 | ADC_SMPR2_SMP13 | ADC_SMPR2_SMP12 | 
						ADC_SMPR2_SMP11 | ADC_SMPR2_SMP10)
				) |
		t * (
			ADC_SMPR2_SMP18_0 |
			ADC_SMPR2_SMP17_0 | ADC_SMPR2_SMP16_0 | ADC_SMPR2_SMP15_0 | ADC_SMPR2_SMP14_0 | 
			ADC_SMPR2_SMP13_0 | ADC_SMPR2_SMP12_0 | ADC_SMPR2_SMP11_0 | ADC_SMPR2_SMP10_0) |
		0;
	// 9..0 ch
	ADC1->SMPR1 =
		(ADC1->SMPR1 & ~ (ADC_SMPR1_SMP9 | ADC_SMPR1_SMP8 | ADC_SMPR1_SMP7 | ADC_SMPR1_SMP6 | 
						ADC_SMPR1_SMP5 | ADC_SMPR1_SMP4 | ADC_SMPR1_SMP3 | 
						ADC_SMPR1_SMP2 | ADC_SMPR1_SMP1 | ADC_SMPR1_SMP0)
				) |
		t * (
			ADC_SMPR1_SMP9_0 | ADC_SMPR1_SMP8_0 | ADC_SMPR1_SMP7_0 | ADC_SMPR1_SMP6_0 |
			ADC_SMPR1_SMP5_0 | ADC_SMPR1_SMP4_0 | ADC_SMPR1_SMP3_0 | ADC_SMPR1_SMP2_0 | 
			ADC_SMPR1_SMP1_0 | ADC_SMPR1_SMP0_0) |
		0;

	ADC1->IER |= ADC_IER_EOS;    //Включили прерываия при окончании преобразования.  

	//	ADC1->CR2 |= ADC_CR2_TSVREFE;	// для тестов

	//ADC1->CR |= ADC_CR_ADCAL; //Запуск калибровки АЦП
	//while ((ADC1->CR & ADC_CR_ADCAL) == 0)
	//	; //Ожидаем окончания калибровки

	//ADC2->CR |= ADC_CR_ADCAL; //Запуск калибровки АЦП
	//while ((ADC2->CR & ADC_CR_ADCAL) == 0)
	//	; //Ожидаем окончания калибровки

	// первый запуск производится в hardware_adc_startonescan().
	// А здесь всё...

#elif CPUSTYLE_STM32F0XX

	const uint_fast32_t ainmask = build_adc_mask();
	HARDWARE_ADC_INITIALIZE(ainmask);

	#if WITHREFSENSOR
		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
		__DSB();
		SYSCFG->CFGR3 |= SYSCFG_CFGR3_ENBUF_VREFINT_ADC;
		while ((SYSCFG->SYSCFG_VREFINT_ADC_RDYF) == 0)
			;
	#endif /* WITHREFSENSOR */

	#if WITHTEMPSENSOR
		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; 
		__DSB();
		SYSCFG->CFGR3 |= SYSCFG_CFGR3_ENBUF_SENSOR_ADC;
		while ((SYSCFG->SYSCFG_CFGR3_SENSOR_ADC_RDYF) == 0)
	#endif /* WITHTEMPSENSOR */

	//Initialization ADC. PortC.0 ADC 10  
	RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_ADCPRE)) | RCC_CFGR_ADCPRE_DIV4; // RCC_CFGR_ADCPRE12_DIV2;    //  
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;    //Затактировали АЦП  
	__DSB();

	ADC1->CFGR1 =
		1 * ADC_CFGR1_DISCEN |
		2 * ADC_CFGR1_RES_0 |	// 2: 8 bit
		0;

	ADC1->CFGR2 =
		0;

	ADC1->IER = ADC_IER_EOCIE;

	// Калибровка.
	ADC1->CR = ADC_CR_ADCAL;
	while ((ADC1->CR & ADC_CR_ADCAL) != 0)
		;
	(void) (ADC1->DR & 0x7f); // 0..127 values - calibration factor;

	ADC1->CR = ADC_CR_ADEN;

	#if STM32F0XX_MD
		NVIC_SetPriority(ADC1_COMP_IRQn, ARM_SYSTEM_PRIORITY);
		NVIC_EnableIRQ(ADC1_COMP_IRQn);    //Включаем прерывания с АЦП. Обрабатывает ADC1_COMP_IRQHandler() 
	#else /* STM32F0XX_MD */
		NVIC_SetPriority(ADC1_IRQn, ARM_SYSTEM_PRIORITY);
		NVIC_EnableIRQ(ADC1_IRQn);    //Включаем прерывания с АЦП. Обрабатывает ADC1_IRQHandler() 
	#endif /* STM32F0XX_MD */

	// первый запуск производится в hardware_adc_startonescan().
	// А здесь всё...

#elif CPUSTYLE_R7S721

	//#warning TODO: Add code for R7S721 ADC support

	CPG.STBCR6 &= ~ (1U << CPG_STBCR6_MSTP67_SHIFT);	// Module Stop 67 0: The A/D converter runs.
	(void) CPG.STBCR6;			/* Dummy read */

	const uint_fast32_t ainmask = build_adc_mask();

	ADC.ADCSR =
		1 * (1uL << ADC_ADCSR_ADIE_SHIFT) |	// ADIE - 1: A/D conversion end interrupt (ADI) request is enabled
		3 * (1uL << ADC_ADCSR_CKS_SHIFT) |	// CKS[2..0] - Clock Select - 011: Conversion time = 382 tcyc (maximum)
		0;

	HARDWARE_ADC_INITIALIZE(ainmask);

	// connect to interrupt
	{
		const uint16_t int_id = ADI_IRQn;	/* 12bit A/D converter                */
		r7s721_intc_registintfunc(int_id, r7s721_adi_irq_handler);
		GIC_SetPriority(int_id, ARM_SYSTEM_PRIORITY);
		GIC_EnableIRQ(int_id);
	}

	// первый запуск производится в hardware_adc_startonescan().
	// А здесь всё...

#elif CPUSTYLE_STM32L0XX

	#warning TODO: Add code for CPUSTYLE_STM32L0XX ADC support
	#if 0
	const uint_fast32_t ainmask = build_adc_mask();
	HARDWARE_ADC_INITIALIZE(ainmask);

	//Initialization ADC. PortC.0 ADC 10  
	RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_ADCPRE)) | RCC_CFGR_ADCPRE_DIV4; // RCC_CFGR_ADCPRE12_DIV2;    //  
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;    //Затактировали АЦП  
	__DSB();

	ADC1->CFGR1 =
		1 * ADC_CFGR1_DISCEN |
		2 * ADC_CFGR1_RES_0 |	// 2: 8 bit
		0;

	ADC1->CFGR2 =
		0;

	ADC1->IER = ADC_IER_EOCIE;

	// Калибровка.
	ADC1->CR = ADC_CR_ADCAL;
	while ((ADC1->CR & ADC_CR_ADCAL) != 0)
		;
	(void) (ADC1->DR & 0x7f); // 0..127 values - calibration factor;

	ADC1->CR = ADC_CR_ADEN;

	NVIC_SetPriority(ADC1_COMP_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(ADC1_COMP_IRQn);    //Включаем прерывания с АЦП. Обрабатывает ADC1_COMP_IRQHandler() 
	#endif

	// первый запуск производится в hardware_adc_startonescan().
	// А здесь всё...

#else
	#error Undefined CPUSTYLE_XXX
#endif

	//debug_printf_P(PSTR("hardware_adc_initialize done\n"));
}

// хотя бы один вход (s-метр) есть.
static void 
hardware_adc_startonescan(void)
{
	//ASSERT((adc_input == 0) || (adc_input == board_get_adcinputs()));	// проверяем, успело ли отработать ранее запущенное преобразование
	if ((adc_input != 0) && (adc_input != board_get_adcinputs()))
		return;	// не успели
	adc_input = 0;

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	// Select next ADC input (only one)
	const portholder_t mask = ADC_CHER_CH0 << board_get_adcch(adc_input);
	ADC->ADC_CHER = mask; /* enable ADC */
	ADC->ADC_CHDR = ~ mask; /* disable ADC */
	ADC->ADC_CR = ADC_CR_START;	// Start the AD conversion

#elif CPUSTYLE_AT91SAM7S

	// Select next ADC input (only one)
	const portholder_t mask = AT91C_ADC_CH0 << board_get_adcch(adc_input);
	AT91C_BASE_ADC->ADC_CHDR = ~ mask; /* disable ADC inputs */
	AT91C_BASE_ADC->ADC_CHER = mask; /* enable ADC */
	AT91C_BASE_ADC->ADC_CR = AT91C_ADC_START;	// Start the AD conversion

#elif CPUSTYLE_ATMEGA

	ADMUX = hardware_atmega_admux(board_get_adcch(adc_input));
	// Start the AD conversion
	ADCSRA |= (1U << ADSC);

#elif CPUSTYLE_ATXMEGAXXXA4

	#warning TODO: write atxmega code - ADC start
	ADCA.CH0.MUXCTRL = board_get_adcch(adc_input);
	// Start the AD conversion
	ADCA.CH0.CTRL |= (1U << ADC_CH_START_bp);			// Start the AD conversion

#elif CPUSTYLE_STM32H7XX
	
	// Установить следующий вход (блок ADC может измениться)
	const adcinmap_t * const adcmap = getadcmap(board_get_adcch(adc_input));
	ADC_TypeDef * const adc = adcmap->adc;

	ASSERT((adc->CR & (ADC_CR_JADSTART | ADC_CR_ADSTART)) == 0);
	adc->SQR1 = (adc->SQR1 & ~ (ADC_SQR1_L | ADC_SQR1_SQ1)) |
		0 * ADC_SQR1_L_0 |	//Выбираем преобразование с одного канала. Сканирования нет.
		adcmap->ch * ADC_SQR1_SQ1_0 |
		0;
	adc->CR |= ADC_CR_ADSTART;	// Запуск преобразования

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX
	//#warning TODO: to be implemented for CPUSTYLE_STM32F1XX

	// Установить следующий вход
	ADC1->SQR3 = (ADC1->SQR3 & ~ ADC_SQR3_SQ1) | (ADC_SQR3_SQ1_0 * board_get_adcch(adc_input)); 
	#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX
		ADC1->CR2 |= ADC_CR2_SWSTART;	// !!!!
	#endif

#elif CPUSTYLE_STM32F30X
	#warning TODO: Add code for STM32F30X support

	// Установить следующий вход
	ADC1->SQR1 = (ADC1->SQR1 & ~ ADC_SQR1_SQ1) | (ADC_SQR1_SQ1_0 * board_get_adcch(adc_input)); 
	ADC1->CR |= ADC_CR_ADSTART;	// ADC Start of Regular conversion

#elif CPUSTYLE_STM32L0XX

	ADC1->CHSELR = 1UL <<  board_get_adcch(adc_input);
	ADC1->CR = ADC_CR_ADSTART;	// ADC Start of Regular conversion

#elif CPUSTYLE_R7S721
	//#warning TODO: Add code for R7S721 ADC support
	// 27.4.1 Single Mode
	// Установить следующий вход
	ADC.ADCSR = (ADC.ADCSR & ~ (0x07)) | 
		board_get_adcch(adc_input) |	// канал для преобразования
		1 * ADC_ADCSR_ADST |	// ADST: Single mode: A/D conversion starts
		0;

#elif CPUSTYLE_STM32F0XX
	#warning: #warning Must be implemented for this CPU

#else

	#error Undefined CPUSTYLE_XXX

#endif
}

#else /* WITHCPUADCHW */

	void ATTRWEAK
	ADC1_2_IRQHandler(void)
	{
			debug_printf_P(PSTR("ADC1_2_IRQHandler trapped\n"));
			for (;;)
				;
	}

	void ATTRWEAK
	ADC_IRQHandler(void)
	{
			debug_printf_P(PSTR("ADC1_2_IRQHandler trapped\n"));
			for (;;)
				;
	}

	void ATTRWEAK
	ADC3_IRQHandler(void)
	{
			debug_printf_P(PSTR("ADC3_IRQHandler trapped\n"));
			for (;;)
				;
	}

#endif /* WITHCPUADCHW */


#if WITHDCDCFREQCTL
	/* TIM16_CH1 - PF6 */
	void hardware_blfreq_initialize(void)
	{
		RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;   //подаем тактирование на TIM16
		__DSB();

		TIM16->CCMR1 = 
			3 * TIM_CCMR1_OC1M_0 |	// для кодов более 7 использовать TIM_CCMR1_OC1M_3. Output Compare 1 Mode = 3
			0;
		TIM16->CCER = TIM_CCER_CC1E;
		//TIM16->DIER = TIM_DIER_UIE;        	 // разрешить событие от таймера
		TIM16->BDTR = TIM_BDTR_MOE;
	}

	/* TIM16_CH1 - PF6 */
	void hardware_blfreq_setfreq(uint_fast32_t v)
	{
		unsigned value;	/* делитель */
		const uint_fast8_t prei = calcdivider(v, STM32F_TIM4_TIMER_WIDTH, STM32F_TIM4_TIMER_TAPS, & value, 1);
		TIM16->PSC = ((1UL << prei) - 1) & TIM_PSC_PSC;

		TIM16->ARR = value;
		//TIM16->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости - если счётчик успевал превысить значение ARR - считал до конца */

		//TIM16->CCR1 = (value / 2) & TIM_CCR1_CCR1;	// TIM16_CH1 - wave output
		//TIM16->ARR = value;
		TIM16->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE;	/* разрешить перезагрузку и включить таймер */
	}

#endif /* WITHDCDCFREQCTL */

#if SIDETONE_TARGET_BIT != 0


#if CPUSTYLE_ATMEGA

	#if CPUSTYLE_ATMEGA_XXX4
		//enum { TCCR2A_WORK = (1U << COM2A0) | (1U << WGM21), TCCR2A_DISCONNECT = (1U << WGM21) };
	#elif CPUSTYLE_ATMEGA328
	#else
		enum { TCCR2WGM = (1U << WGM21) | (1U << COM20) };	// 0x18
	#endif
#endif

/* после изменения набора формируемых звуков - обновление программирования таймера. */
void hardware_sounds_disable(void)
{

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	HARDWARE_SIDETONE_DISCONNECT();

#elif CPUSTYLE_AT91SAM7S

	HARDWARE_SIDETONE_DISCONNECT();

#elif CPUSTYLE_ATMEGA328

	// генерация сигнала самоконтроля на PD6(OC0A)
	TCCR0B = 0x00;	// 0 - Normal port operation, OC0A disconnected.

#elif CPUSTYLE_ATMEGA_XXX4
	// генерация сигнала самоконтроля на PD7(OC2)

	// 8-bit таймер должен сгенерировать переключения выхода с частотой минимум 800 герц (для получения тона 400 герц).
	TCCR2B = 0x00;	// 0 - Normal port operation, OC2 disconnected.

#elif CPUSTYLE_ATMEGA128
	// ATMega128/ATMega64
	// генерация сигнала самоконтроля на PD7(OC2)

	// 8-bit таймер должен сгенерировать переключения выхода с частотой минимум 800 герц (для получения тона 400 герц).
	TCCR2 = TCCR2WGM;	// (1U << WGM21) | (1U << COM20)

#elif CPUSTYLE_ATMEGA32
	// генерация сигнала самоконтроля на PD7(OC2)

	// 8-bit таймер должен сгенерировать переключения выхода с частотой минимум 800 герц (для получения тона 400 герц).
	TCCR2 = TCCR2WGM;	// (1U << WGM21) | (1U << COM20)

#elif CPUSTYLE_ATXMEGAXXXA4

	TCD1.CTRLA = 0x00;

#elif CPUSTYLE_STM32F

	TIM4->CR1 = 0x00;

#else
	#error Undefined CPUSTYLE_XXX
#endif

}




// called from interrupt or with disabled interrupts
// всегда включаем генерацию выходного сигнала
void hardware_sounds_setfreq(
	uint_fast8_t prei,
	unsigned value
	)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	TC0->TC_CHANNEL [1].TC_CMR =
		(TC0->TC_CHANNEL [1].TC_CMR & ~ TC_CMR_TCCLKS_Msk) | tc_cmr_tcclks [prei];
	TC0->TC_CHANNEL [1].TC_RC = value;	// программирование полупериода (выход с триггерам)

	HARDWARE_SIDETONE_CONNECT();

#elif CPUSTYLE_AT91SAM7S

	AT91C_BASE_TCB->TCB_TC1.TC_CMR =
		(AT91C_BASE_TCB->TCB_TC1.TC_CMR & ~ AT91C_TC_CLKS) | tc_cmr_clks [prei];
	AT91C_BASE_TCB->TCB_TC1.TC_RC = value;	// программирование полупериода (выход с триггерам)

	HARDWARE_SIDETONE_CONNECT();

#elif CPUSTYLE_ATMEGA328
	// 
	// compare match после записи делителя отменяется на один цикл
	// timer2 - 8 bit wide.
	// генерация сигнала самоконтроля на PD6(OC0A) - выход делителя на 2
	// TCCR2B – Timer/Counter Control Register B
	const uint_fast8_t tccrXBval = (prei + 1);
	if ((TCCR0B != tccrXBval) || (OCR0A > value))	// таймер может отработать до максимального значения счётчика, если уменьшаем TOP
	{
		TCCR0B = 0x00;		// останавливаем таймер.
		OCR0A = value;		// загружаем новое значение TOP
		TCNT0 = 0x00;		// сбрасыаваем счётчик
		TCCR0B = tccrXBval;	// запускаем таймер
	}
	else
		OCR0A = value;	// период станет длиннее

#elif CPUSTYLE_ATMEGA_XXX4
	// 
	// timer2 - 8 bit wide.
	// генерация сигнала самоконтроля на PD7(OC2) - выход делителя на 2
	// Пототму в расчёте используется tonefreq * 2
	// TCCR2B – Timer/Counter Control Register B
	const uint_fast8_t tccrXBval = (prei + 1);
	if ((TCCR2B != tccrXBval) || (OCR2A > value))	// таймер может отработать до максимального значения счётчика, если уменьшаем TOP
	{
		TCCR2B = 0x00;		// останавливаем таймер.
		OCR2A = value;		// загружаем новое значение TOP
		TCNT2 = 0x00;		// сбрасыаваем счётчик
		TCCR2B = tccrXBval;	// запускаем таймер
	}
	else
		OCR2A = value;	// период станет длиннее

#elif CPUSTYLE_ATMEGA128
	// ATMega128/ATMega64

	// 
	// timer2 - 8 bit wide.
	// генерация сигнала самоконтроля на PD7(OC2) - выход делителя на 2
	// Пототму в расчёте используется tonefreq * 2
	// TCCR2WGM = (1U << WGM21) | (1U << COM20)
	const uint_fast8_t tccrXBval = TCCR2WGM | (prei + 1);
	if ((TCCR2 != tccrXBval) || (OCR2 > value))		// таймер может отработать до максимального значения счётчика, если уменьшаем TOP
	{
		TCCR2 = TCCR2WGM;	// (1U << WGM21) | (1U << COM20) останавливаем таймер
		OCR2 = value;		// загружаем новое значение TOP
		TCNT2 = 0x00;		// сбрасыаваем счётчик
		TCCR2 = tccrXBval;	// запускаем таймер
	}
	else
		OCR2 = value;		// период станет длиннее

#elif CPUSTYLE_ATMEGA32

	// 
	// timer2 - 8 bit wide.
	// генерация сигнала самоконтроля на PD7(OC2) - выход делителя на 2
	// Пототму в расчёте используется tonefreq * 2
	// TCCR2WGM = (1U << WGM21) | (1U << COM20)
	const uint_fast8_t tccrXBval = TCCR2WGM | (prei + 1);
	if ((TCCR2 != tccrXBval) || (OCR2 > value))		// таймер может отработать до максимального значения счётчика, если уменьшаем TOP
	{
		TCCR2 = TCCR2WGM;	// (1U << WGM21) | (1U << COM20) останавливаем таймер
		OCR2 = value;		// загружаем новое значение TOP
		TCNT2 = 0x00;		// сбрасыаваем счётчик
		TCCR2 = tccrXBval;	// запускаем таймер
	}
	else
		OCR2 = value;		// период станет длиннее

#elif CPUSTYLE_ATXMEGAXXXA4

	// программирование таймера
	TCD1.CCA = value;	// timer/counter C1, compare register A, see TCC1_CCA_vect
	TCD1.CTRLA = (prei + 1);
	//TCC2.INTCTRLB = (TC_CCAINTLVL_MED_gc);
	// разрешение прерываний на входе в PMIC
	//PMIC.CTRL |= (PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm);

#elif CPUSTYLE_STM32F

	TIM4->PSC = ((1UL << prei) - 1) & TIM_PSC_PSC;

	TIM4->CCR3 = (value / 2) & TIM_CCR3_CCR3;	// TIM4_CH3 - sound output
	TIM4->ARR = value;
	TIM4->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE;	/* разрешить перезагрузку и включить таймер */

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

/* 
	формирование сигнала самоконтроля.
	На процессоре AT91SAM7S64 манипуляция осуществляется отключением таймера от выхода.
	Повторный запуск таймера возможен только с флагом AT91C_TC_SWTRG, а при этом
	происходит его сброс (перезапуск) - тон искажённый.
*/
void 
hardware_beep_initialize(void)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	// --- TC1 used to generate CW sidetone ---
	// PA15: Peripheral B: TIOA1
	PMC->PMC_PCER0 = (1UL << ID_TC1);	 // разрешить тактированние этого блока (ID_TC0..ID_TC5 avaliable)
	
	//TC0->TC_BMR = (TC0->TC_BMR & ~ TC_BMR_TC1XC1S_Msk) | TC_BMR_TC1XC1S_TIOA0;
	TC0->TC_CHANNEL [1].TC_CCR = TC_CCR_CLKDIS; // disable TC1 clock

	TC0->TC_CHANNEL [1].TC_CMR =
					TC_CMR_BSWTRG_NONE | TC_CMR_BEEVT_NONE | TC_CMR_BCPC_NONE | TC_CMR_BCPB_NONE // TIOB: none
					| TC_CMR_ASWTRG_NONE | TC_CMR_AEEVT_NONE | TC_CMR_ACPC_TOGGLE | TC_CMR_ACPA_NONE // TIOA: toggle on RC
					| TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC /*TC_CMR_WAVESEL_UP_AUTO */ // waveform mode, up to RC
					| TC_CMR_ENETRG | TC_CMR_EEVT_XC2 | TC_CMR_EEVTEDG_NONE // no ext. trigger
					| (0 * TC_CMR_CPCDIS) 
					| (0 * TC_CMR_CPCSTOP) 
					| TC_CMR_BURST_NONE 
					| (0 * TC_CMR_CLKI) 
					| TC_CMR_TCCLKS_TIMER_CLOCK1;

	TC0->TC_CHANNEL [1].TC_CCR = TC_CCR_SWTRG | TC_CCR_CLKEN; // reset and enable clock
	//AT91C_BASE_TCB->TCB_TC1.TC_CCR = TC_CMR_CLKDIS; // reset and enable TC1 clock

	// Timer outputs, connected to pin(s)
	HARDWARE_SIDETONE_INITIALIZE();

#elif CPUSTYLE_AT91SAM7S

	// --- TC1 used to generate CW sidetone ---
	// PA15: Peripheral B: TIOA1
	AT91C_BASE_PMC->PMC_PCER = (1UL << AT91C_ID_TC1); // разрешить тактированние этого блока (AT91C_ID_TC0..AT91C_ID_TC2 avaliable)
	
	////AT91C_BASE_TCB->TCB_BMR = (AT91C_BASE_TCB->TCB_BMR & ~ AT91C_TCB_TC1XC1S) | AT91C_TCB_TC1XC1S_TIOA0;
	AT91C_BASE_TCB->TCB_TC1.TC_CCR = AT91C_TC_CLKDIS; // disable TC1 clock

	AT91C_BASE_TCB->TCB_TC1.TC_CMR =
					AT91C_TC_BSWTRG_NONE | AT91C_TC_BEEVT_NONE | AT91C_TC_BCPC_NONE | AT91C_TC_BCPB_NONE // TIOB: none
					| AT91C_TC_ASWTRG_NONE | AT91C_TC_AEEVT_NONE | AT91C_TC_ACPC_TOGGLE | AT91C_TC_ACPA_NONE // TIOA: toggle on RC
					| AT91C_TC_WAVE | AT91C_TC_WAVESEL_UP_AUTO // waveform mode, up to RC
					| AT91C_TC_ENETRG | AT91C_TC_EEVT_XC2 | AT91C_TC_EEVTEDG_NONE // no ext. trigger
					| (0 * AT91C_TC_CPCDIS) 
					| (0 * AT91C_TC_CPCSTOP) 
					| AT91C_TC_BURST_NONE 
					| (0 * AT91C_TC_CLKI) 
					| AT91C_TC_CLKS_TIMER_DIV1_CLOCK;

	AT91C_BASE_TCB->TCB_TC1.TC_CCR = AT91C_TC_SWTRG | AT91C_TC_CLKEN; // reset and enable clock
	//AT91C_BASE_TCB->TCB_TC1.TC_CCR = AT91C_TC_CLKDIS; // reset and enable TC1 clock

	// Timer outputs, connected to pin(s)
	HARDWARE_SIDETONE_INITIALIZE();

#elif CPUSTYLE_ATMEGA328
	// генерация сигнала самоконтроля на PD6(OC0A)
	// Timer/Counter 0 initialization
	// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: Timer 0 Stopped
	// Mode: CTC top=OCR0A
	// OC0A output: Toggle on compare match
	// OC0B output: Disconnected
	TCCR0A = 0x42;
	TCCR0B = 0x00;
	// обязательно - настройка вывода процессора как выхода.
	SIDETONE_TARGET_DDR |= SIDETONE_TARGET_BIT; 	// output pin connection - test without this string need.
	SIDETONE_TARGET_PORT &= ~ SIDETONE_TARGET_BIT; // disable pull-up

#elif CPUSTYLE_ATMEGA_XXX4
	// Timer/Counter 2 initialization
	// Clock source: System Clock
	// Clock value: Timer 2 Stopped
	// Mode: CTC top=OCR2A
	// OC2A output: Toggle on compare match
	// OC2B output: Disconnected
	ASSR = 0x00;
	TCCR2A = (1U << COM2A0) | (1U << WGM21);	// 0x42, (1U << WGM21) only - OC2A disconnected
	TCCR2B = 0x00;
	// обязательно - настройка вывода процессора как выхода.
	SIDETONE_TARGET_DDR |= SIDETONE_TARGET_BIT; // output pin connection - strongly need for working
	SIDETONE_TARGET_PORT &= ~ SIDETONE_TARGET_BIT; // disable pull-up

#elif CPUSTYLE_ATMEGA128
	// ATMega128/ATMega64
	// Timer/Counter 2 initialization
	// Clock source: System Clock
	// Clock value: Timer 2 Stopped
	// Mode: CTC top=OCR2
	// OC2 output: Toggle on compare match
	ASSR = 0x00;
	TCCR2 = TCCR2WGM;	// (1U << WGM21) | (1U << COM20)
	// обязательно - настройка вывода процессора как выхода.
	SIDETONE_TARGET_DDR |= SIDETONE_TARGET_BIT; // (1U << DDD7);	// output pin connection - test without this string need.
	SIDETONE_TARGET_PORT &= ~ SIDETONE_TARGET_BIT; // (1U << PD7);	// disable pull-up

#elif CPUSTYLE_ATMEGA32
	// Timer/Counter 2 initialization
	// Clock source: System Clock
	// Clock value: Timer 2 Stopped
	// Mode: CTC top=OCR2
	// OC2 output: Toggle on compare match
	ASSR = 0x00;
	TCCR2 = TCCR2WGM;	// (1U << WGM21) | (1U << COM20)
	// обязательно - настройка вывода процессора как выхода.
	SIDETONE_TARGET_DDR |= SIDETONE_TARGET_BIT; // (1U << DDD7);	// output pin connection - test without this string need.
	SIDETONE_TARGET_PORT &= ~ SIDETONE_TARGET_BIT; // (1U << PD7);	// disable pull-up

#elif CPUSTYLE_ATXMEGAXXXA4

	TCD1.CTRLB = (TC1_CCAEN_bm | TC_WGMODE_FRQ_gc);
	SIDETONE_TARGET_DDR |= SIDETONE_TARGET_BIT; // (1U << DDD7);	// output pin connection - test without this string need.

#elif CPUSTYLE_STM32F

	// apb1
	RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;   //подаем тактирование на TIM4
	__DSB();

	TIM4->CCMR2 = TIM_CCMR2_OC3M_0 | TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;	// Output Compare 3 Mode
	TIM4->CCER = TIM_CCER_CC3E;
	
	HARDWARE_SIDETONE_INITIALIZE();

#else
	#error Undefined CPUSTYLE_XXX
#endif

}

// return code: prescaler
uint_fast8_t
hardware_calc_sound_params(
	uint_least16_t tonefreq,	/* tonefreq - частота в десятых долях герца. Минимум - 400 герц (определено набором команд CAT). */
	unsigned * pvalue)
{

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	return calcdivider(calcdivround10(tonefreq * 2), ATSAM3S_TIMER_WIDTH, ATSAM3S_TIMER_TAPS, pvalue, 1);

#elif CPUSTYLE_AT91SAM7S

	return calcdivider(calcdivround10(tonefreq * 2), AT91SAM7_TIMER_WIDTH, AT91SAM7_TIMER_TAPS, pvalue, 1);

#elif CPUSTYLE_ATMEGA328
	// 
	// compare match после записи делителя отменяется на один цикл
	// timer0 - 8 bit wide.
	// генерация сигнала самоконтроля на PD6(OC0A) - выход делителя на 2
	return calcdivider(calcdivround10(tonefreq * 2), ATMEGA_TIMER0_WIDTH, ATMEGA_TIMER0_TAPS, pvalue, 1);

#elif CPUSTYLE_ATMEGA_XXX4
	// 
	// timer2 - 8 bit wide.
	// генерация сигнала самоконтроля на PD7(OC2) - выход делителя на 2
	// Пототму в расчёте используется tonefreq * 2
	return calcdivider(calcdivround10(tonefreq * 2), ATMEGA_TIMER2_WIDTH, ATMEGA_TIMER2_TAPS, pvalue, 1);

#elif CPUSTYLE_ATMEGA128
	// ATMega128/ATMega64

	// 
	// timer2 - 8 bit wide.
	// генерация сигнала самоконтроля на PD7(OC2) - выход делителя на 2
	// Пототму в расчёте используется tonefreq * 2
	return calcdivider(calcdivround10(tonefreq * 2), ATMEGA_TIMER2_WIDTH, ATMEGA_TIMER2_TAPS, pvalue, 1);

#elif CPUSTYLE_ATMEGA32

	// 
	// timer2 - 8 bit wide.
	// генерация сигнала самоконтроля на PD7(OC2) - выход делителя на 2
	// Пототму в расчёте используется tonefreq * 2
	return calcdivider(calcdivround10(tonefreq * 2), ATMEGA_TIMER2_WIDTH, ATMEGA_TIMER2_TAPS, pvalue, 1);

#elif CPUSTYLE_ATXMEGAXXXA4

	return calcdivider(calcdivround10(tonefreq * 2), ATXMEGA_TIMER_WIDTH, ATXMEGA_TIMER_TAPS, pvalue, 1);

#elif CPUSTYLE_STM32F

	// for tim1 use apb2, for other apb1
	// now - tim4
	// TIM4 - 16-bit timer
	return calcdivider(calcdivround10_pclk2(tonefreq), STM32F_TIM4_TIMER_WIDTH, STM32F_TIM4_TIMER_TAPS, pvalue, 1);

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

#else /* SIDETONE_TARGET_BIT != 0 */

void 
hardware_beep_initialize(void)
{
}

#endif /* SIDETONE_TARGET_BIT != 0 */

#if WITHSPIHW

	#if SPI_BIDIRECTIONAL
		#error WITHSPIHW and SPI_BIDIRECTIONAL can not be used together
	#endif

	#if CPUSTYLE_AT91SAM7S || CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S
		static portholder_t spi_csr_val8w [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
		static portholder_t spi_csr_val16w [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 в режиме 16-ти битных слов. */
	#elif CPUSTYLE_STM32H7XX
		static portholder_t spi_cfg1_val8w;
		static portholder_t spi_cfg1_val16w;
		static portholder_t spi_cfg2_val [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
	#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32L0XX
		static portholder_t spi_cr1_val8w [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
		static portholder_t spi_cr1_val16w [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 в режиме 16-ти битных слов. */
	#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX
		static portholder_t spi_cr1_val8w [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
	#elif CPUSTYLE_ATMEGA
		static portholder_t spcr_val [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
		static portholder_t spsr_val [SPIC_SPEEDS_COUNT];
	#elif CPUSTYLE_ATXMEGA
		static portholder_t spi_ctl_val [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
	#elif CPUSTYLE_R7S721
		static portholder_t spi_spbr_val [SPIC_SPEEDS_COUNT];
		static portholder_t spi_spcmd0_val8w [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
		static portholder_t spi_spcmd0_val16w [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
	#endif /* CPUSTYLE_STM32F1XX */

#if WITHSPIHWDMA

#if CPUSTYLE_STM32H7XX
#else /* CPUSTYLE_STM32H7XX */
#endif /* CPUSTYLE_STM32H7XX */

#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

/* Инициализация DMA для прёма по SPI1 */
	// DMA2: SPI1_RX: Stream 0: Channel 3
static void DMA2_SPI1_RX_initialize(void)
{
	/* SPI1_RX - Stream0, Channel3 */ 
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;//включил DMA2 
	__DSB();

	#if CPUSTYLE_STM32H7XX
		// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
		// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
		enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
		DMAMUX1_Channel8->CCR = 37 * DMAMUX_CxCR_DMAREQ_ID_0;	// SPI1_RX
		DMA2_Stream0->PAR = (uintptr_t) & SPI1->RXDR;
	#else /* CPUSTYLE_STM32H7XX */
		const uint_fast8_t ch = 3;
		DMA2_Stream0->PAR = (uintptr_t) & SPI1->DR;
	#endif /* CPUSTYLE_STM32H7XX */

	DMA2_Stream0->FCR &= ~ DMA_SxFCR_DMDIS;	// use Direct mode
	//DMA2_Stream0->FCR |= DMA_SxFCR_DMDIS;	// Direct mode disabled
	DMA2_Stream0->CR =
		(ch * DMA_SxCR_CHSEL_0) |	// канал
		(3 * DMA_SxCR_MBURST_0) |	// INCR16 (incremental burst of 16 beats) - ignored in Direct mode
		(3 * DMA_SxCR_PBURST_0) |	// INCR16 (incremental burst of 16 beats) - ignored in Direct mode
		(0 * DMA_SxCR_PL_0) |		// Priority level - low
		(0 * DMA_SxCR_DIR_0) |		// 00: Peripheral-to-memory
		(1 * DMA_SxCR_MINC) |		// инкремент адреса памяти
		(0 * DMA_SxCR_MSIZE_0) |	// длина в памяти - 8 bit - устанавливается перед обменом
		(0 * DMA_SxCR_PSIZE_0) |	// длина в DR - 8 bit - устанавливается перед обменом
		//(1 * DMA_SxCR_CIRC) |		// циклический режим не требуется при DBM
		(0 * DMA_SxCR_CT) |			// M0AR selected
		//(1 * DMA_SxCR_DBM) |		// double buffer mode seelcted
		0;
}

// Инициализация DMA для передачи SPI1
// DMA2: SPI1_TX: Stream 3: Channel 3
static void DMA2_SPI1_TX_initialize(void)
{
	// DMA2: SPI1_TX: Stream 3: Channel 3
	/* DMA для передачи по SPI1 */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;	// включил DMA2 
	__DSB();

	#if CPUSTYLE_STM32H7XX
		// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
		// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
		enum { ch = 0, DMA_SxCR_CHSEL_0 = 0 };
		DMAMUX1_Channel11->CCR = 38 * DMAMUX_CxCR_DMAREQ_ID_0;	// SPI1_TX
		DMA2_Stream3->PAR = (uintptr_t) & SPI1->TXDR;
	#else /* CPUSTYLE_STM32H7XX */
		const uint_fast8_t ch = 3;
		DMA2_Stream3->PAR = (uintptr_t) & SPI1->DR;
	#endif /* CPUSTYLE_STM32H7XX */


	DMA2_Stream3->FCR &= ~ DMA_SxFCR_DMDIS;	// use direct mode
	//DMA2_Stream3->FCR |= DMA_SxFCR_DMDIS;	// Direct mode disabled
	DMA2_Stream3->CR =
		(ch * DMA_SxCR_CHSEL_0) |	// канал
		(3 * DMA_SxCR_MBURST_0) |	// INCR16 (incremental burst of 16 beats) - ignored in Direct mode
		(3 * DMA_SxCR_PBURST_0) |	// INCR16 (incremental burst of 16 beats) - ignored in Direct mode
		(0 * DMA_SxCR_PL_0) |		// Priority level - low
		(1 * DMA_SxCR_DIR_0) |		// направление - память - периферия
		(1 * DMA_SxCR_MINC) |		// инкремент адреса памяти
		(0 * DMA_SxCR_MSIZE_0) |	// длина в памяти - 8bit - устанавливается перед обменом
		(0 * DMA_SxCR_PSIZE_0) |	// длина в SPI_DR- 8bit - устанавливается перед обменом
		//(1 * DMA_SxCR_CIRC) |		// циклический режим не требуется при DBM
		(0 * DMA_SxCR_CT) |			// M0AR selected
		//(1 * DMA_SxCR_DBM) |		// double buffer mode seelcted
		0;
}

#if 1
// Ожидание завершения обмена
static void DMA2_waitTC(
	uint_fast8_t i		// 0..7 - номер Stream
	)
{
	uint_fast8_t mask = 1UL <<((i & 0x01) * 6);
	if (i >= 4)
	{
		if (i >= 6)
		{
			// Дожидаемся завершения обмена канала DMA
			while ((DMA2->HISR & (DMA_HISR_TCIF6 * mask)) == 0)	// ожидаем завершения обмена по соответствушему stream
				;
			DMA2->HIFCR = DMA_HIFCR_CTCIF6 * mask;		// сбросил флаг соответствующий stream
		}
		else
		{
			// Дожидаемся завершения обмена канала DMA
			while ((DMA2->HISR & (DMA_HISR_TCIF4 * mask)) == 0)	// ожидаем завершения обмена по соответствушему stream
				;
			DMA2->HIFCR = DMA_HIFCR_CTCIF4 * mask;		// сбросил флаг соответствующий stream
		}
	}
	else
	{
		if (i >= 2)
		{
			// Дожидаемся завершения обмена канала DMA
			while ((DMA2->LISR & (DMA_LISR_TCIF2 * mask)) == 0)	// ожидаем завершения обмена по соответствушему stream
				;
			DMA2->LIFCR = DMA_LIFCR_CTCIF2 * mask;		// сбросил флаг соответствующий stream
		}
		else
		{
			// Дожидаемся завершения обмена канала DMA
			while ((DMA2->LISR & (DMA_LISR_TCIF0 * mask)) == 0)	// ожидаем завершения обмена по соответствушему stream
				;
			DMA2->LIFCR = DMA_LIFCR_CTCIF0 * mask;		// сбросил флаг соответствующий stream
		}
	}
}

#endif

#elif CPUSTYLE_STM32F1XX

// Инициализация DMA для прёма по SPI1
static void DMA2_SPI1_RX_initialize(void)
{
}

// Инициализация DMA для передачи SPI1
static void DMA2_SPI1_TX_initialize(void)
{
}

#endif /* CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX */

#endif /* WITHSPIHWDMA */

/* Управление SPI. Так как некоторые периферийные устройства не могут работать с 8-битовыми блоками
   на шине, в таких случаях формирование делается программно - аппаратный SPI при этом отключается
   */
/* инициализация и перевод в состояние "отключено" */
void hardware_spi_master_initialize(void)		
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	// инициализация контроллера SPI

	// Get clock 
    PMC->PMC_PCER0 = (1UL << ID_PIOA) | (1UL << ID_SPI);	/* Need PIO too */

    // setup PIO pins for SPI bus, disconnect from peripherials
	SPIIO_INITIALIZE();

    // reset and enable SPI
    SPI->SPI_CR = SPI_CR_SWRST;
    SPI->SPI_CR = SPI_CR_SWRST;
    SPI->SPI_CR = SPI_CR_SPIDIS;


	// Работаем с Fixed Peripheral Selectionб и без Peripheral Chip Select Decoding
    // USE following line for MASTER MODE operation
    //SPI->SPI_MR = AT91C_SPI_MSTR | AT91C_SPI_MODFDIS | AT91C_SPI_PS_FIXED; 
    SPI->SPI_MR = SPI_MR_MSTR | SPI_MR_MODFDIS | (SPI_MR_PS * 0); 

	#if WITHSPIHWDMA
		SPI->SPI_PTCR = SPI_PTCR_RXTDIS | SPI_PTCR_TXTDIS;

		SPI->SPI_TNCR = 0;
		SPI->SPI_RNCR = 0;
		SPI->SPI_RCR = 0;
		SPI->SPI_TCR = 0;

		SPI->SPI_PTCR = SPI_PTCR_RXTEN | SPI_PTCR_TXTEN;
	#endif /* WITHSPIHWDMA */

    
	SPI->SPI_IDR = ~ 0; /* Disable all interrupts */ 
    SPI->SPI_CR = SPI_CR_SPIEN;

#elif CPUSTYLE_AT91SAM7S

	// инициализация контроллера SPI
   
   // Get clock 
    AT91C_BASE_PMC->PMC_PCER = (1UL << AT91C_ID_PIOA) | (1UL << AT91C_ID_SPI);/* Need PIO too */

    // setup PIO pins for SPI bus, disconnect from peripherials
	SPIIO_INITIALIZE();
    
    // reset and enable SPI
    AT91C_BASE_SPI->SPI_CR = AT91C_SPI_SWRST;
    AT91C_BASE_SPI->SPI_CR = AT91C_SPI_SWRST;
    AT91C_BASE_SPI->SPI_CR = AT91C_SPI_SPIDIS;

	// Работаем с Fixed Peripheral Selectionб и без Peripheral Chip Select Decoding
    // USE following line for MASTER MODE operation
    AT91C_BASE_SPI->SPI_MR = AT91C_SPI_MSTR | AT91C_SPI_MODFDIS | AT91C_SPI_PS_FIXED; 


	#if WITHSPIHWDMA
		AT91C_BASE_SPI->SPI_PTCR = AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;

		AT91C_BASE_SPI->SPI_TNCR = 0;
		AT91C_BASE_SPI->SPI_RNCR = 0;
		AT91C_BASE_SPI->SPI_RCR = 0;
		AT91C_BASE_SPI->SPI_TCR = 0;

		AT91C_BASE_SPI->SPI_PTCR = AT91C_PDC_RXTEN | AT91C_PDC_TXTEN;
	#endif /* WITHSPIHWDMA */

    
	AT91C_BASE_SPI->SPI_IDR = ~ 0; /* Disable all interrupts */ 
    AT91C_BASE_SPI->SPI_CR = AT91C_SPI_SPIEN;

#elif CPUSTYLE_ATMEGA

	// SPI initialization
	SPCR = 0x00;	/* отключить */

    // setup PIO pins for SPI bus, disconnect from peripherials
	SPIIO_INITIALIZE();
	

#elif CPUSTYLE_ATXMEGA

	// SPI initialization
	TARGETHARD_SPI.CTRL = 0x00;	/* отключить */

    // setup PIO pins for SPI bus, disconnect from peripherials
	SPIIO_INITIALIZE();

#elif CPUSTYLE_STM32F1XX

	// Начнем с настройки порта:
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     //включить тактирование альтернативных функций
	__DSB();
	cpu_stm32f1xx_setmapr(AFIO_MAPR_SPI1_REMAP);

	// Теперь настроим модуль SPI.
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //подать тактирование                                 
	__DSB();
	SPI1->CR1 = 0x0000;             //очистить первый управляющий регистр
	SPI1->CR2 = 0x0000;	// SPI_CR2_SSOE;             //очистить второй управляющий регистр

	#if WITHSPIHWDMA
		DMA2_SPI1_TX_initialize();	// stream 3, канал 3
		DMA2_SPI1_RX_initialize();	// stream 0. канал 3
	#endif /* WITHSPIHWDMA */

	/* настраиваем в режиме disconnect */
	SPIIO_INITIALIZE();

#elif CPUSTYLE_STM32F4XX
	// Начнем с настройки порта:
	//RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     //включить тактирование альтернативных функций
	//__DSB();
	//cpu_stm32f1xx_setmapr(AFIO_MAPR_SPI1_REMAP);

	// Теперь настроим модуль SPI.
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //подать тактирование                                 
	__DSB();
	SPI1->CR1 = 0x0000;             //очистить первый управляющий регистр
	SPI1->CR2 = 0x0000;	// SPI_CR2_SSOE;             //очистить второй управляющий регистр

	#if WITHSPIHWDMA
		DMA2_SPI1_TX_initialize();	// stream 3, канал 3
		DMA2_SPI1_RX_initialize();	// stream 0. канал 3
	#endif /* WITHSPIHWDMA */

	/* настраиваем в режиме disconnect */
	SPIIO_INITIALIZE();

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	// Настроим модуль SPI.
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; //подать тактирование                                 
	__DSB();
	//SPI1->CR1 = 0x0000;             //очистить первый управляющий регистр
	//SPI1->CR2 = 0x0000;

	#if WITHSPIHWDMA
		DMA2_SPI1_TX_initialize();	// stream 3, канал 3
		DMA2_SPI1_RX_initialize();	// stream 0. канал 3
	#endif /* WITHSPIHWDMA */

	/* настраиваем в режиме disconnect */
	SPIIO_INITIALIZE();

#elif CPUSTYLE_R7S721
	// Renesas Serial Peripheral Interface 0
	// RSPCK0	P6_0	ALT3
	// MOSI0	P6_2	ALT3
	// MISO0	P6_3	ALT3
	// SSL00	P6_1	ALT3

    /* ---- Supply clock to the RSPI(channel 0) ---- */
	CPG.STBCR10 &= ~ (1U << 7);	// Module Stop 107
	(void) CPG.STBCR10;			/* Dummy read */

	RSPI0.SPCR =		/* Control Register (SPCR) */
		0;

	RSPI0.SPPCR =		/* Pin Control Register (SPPCR) */
		0x00 |
		0;
	RSPI0.SPSCR =		/*  (SPSCR) */
		0x00 |
		0;
	// Сбросить буферы
	RSPI0.SPBFCR =		/* Buffer Control Register (SPBFCR) */
		(1U << 7) |		// TXRST - TX buffer reset
		(1U << 6) |		// RXRST - TX buffer reset
		0;
	// Разрешить буферы
	RSPI0.SPBFCR =		/* Buffer Control Register (SPBFCR) */
		(3U << 4) |		// TX buffer trigger level = 0
		0;

	RSPI0.SPCR =		/* Control Register (SPCR) */
		(1U << 3) |		// MSTR - master
		(1U << 6) |		// SPE - Function Enable
		(1U << 5) |		// SPTIE  - Transmit Interrupt Enable (for DMA transfers)
		(1U << 7) |		// SPRIE  - Receive Interrupt Enable (for DMA transfers)
		0;

#if WITHSPIHWDMA
	{
		enum { id = 15 };	// 15: DMAC15
		// DMAC15
		/* Set Destination Start Address */
		//DMAC15.N0DA_n = (uint32_t) & RSPI0.SPDR.UINT8 [R_IO_LL];	// Fixed destination address for 8-bit transfers
		DMAC15.N0DA_n = (uint32_t) & RSPI0.SPDR.UINT16 [R_IO_L];	// Fixed destination address for 16-bit transfers

		/* Set Transfer Size */
		//DMAC15.N0TB_n = DMABUFFSIZE16 * sizeof (int16_t);	// размер в байтах
		//DMAC15.N1TB_n = DMABUFFSIZE16 * sizeof (int16_t);	// размер в байтах

		// Values from Table 9.4 On-Chip Peripheral Module Requests
		// SPTI0 (transmit data empty)
		const uint_fast8_t mid = 0x48;	
		const uint_fast8_t rid = 1;		
		const uint_fast8_t tm = 0;		// single transfer mode
		const uint_fast8_t am = 2;		
		const uint_fast8_t lvl = 1;		
		const uint_fast8_t hien = 1;	
		const uint_fast8_t reqd = 1;	

		DMAC15.CHCFG_n =
			0 * (1U << DMAC15_CHCFG_n_DMS_SHIFT) |		// DMS	0: Register mode
			0 * (1U << DMAC15_CHCFG_n_REN_SHIFT) |		// REN	0: Does not continue DMA transfers.
			0 * (1U << DMAC15_CHCFG_n_RSW_SHIFT) |		// RSW	1: Inverts RSEL automatically after a DMA transaction.
			0 * (1U << DMAC15_CHCFG_n_RSEL_SHIFT) |		// RSEL	0: Executes the Next0 Register Set
			0 * (1U << DMAC15_CHCFG_n_SBE_SHIFT) |		// SBE	0: Stops the DMA transfer without sweeping the buffer (initial value).
			0 * (1U << DMAC15_CHCFG_n_DEM_SHIFT) |		// DEM	0: Does not mask the DMA transfer end interrupt - прерывания каждый раз после TC
			tm * (1U << DMAC15_CHCFG_n_TM_SHIFT) |		// TM	0: Single transfer mode - берётся из Table 9.4
			1 * (1U << DMAC15_CHCFG_n_DAD_SHIFT) |		// DAD	1: Fixed destination address
			0 * (1U << DMAC15_CHCFG_n_SAD_SHIFT) |		// SAD	0: Increment source address
			1 * (1U << DMAC15_CHCFG_n_DDS_SHIFT) |		// DDS	2: 32 bits, 1: 16 bits (Destination Data Size)
			1 * (1U << DMAC15_CHCFG_n_SDS_SHIFT) |		// SDS	2: 32 bits, 1: 16 bits (Source Data Size)
			am * (1U << DMAC15_CHCFG_n_AM_SHIFT) |		// AM	1: ACK mode: Level mode (active until the transfer request from an on-chip peripheral module
			lvl * (1U << DMAC15_CHCFG_n_LVL_SHIFT) |	// LVL	1: Detects based on the level.
			hien * (1U << DMAC15_CHCFG_n_HIEN_SHIFT) |	// HIEN	1: When LVL = 1: Detects a request when the signal is at the High level.
			reqd * (1U << DMAC15_CHCFG_n_REQD_SHIFT) |	// REQD		Request Direction
			(id & 0x07) * (1U << DMAC15_CHCFG_n_SEL_SHIFT) |		// SEL	0: CH0/CH8
			0;

		enum { dmarsshift = (id & 0x01) * 16 };
		DMAC1415.DMARS = (DMAC1415.DMARS & ~ (0x1FFul << dmarsshift)) |
			mid * (1U << (2 + dmarsshift)) |		// MID
			rid * (1U << (0 + dmarsshift)) |		// RID
			0;

		DMAC815.DCTRL_0_7 = (DMAC815.DCTRL_0_7 & ~ (/*(1U << 1) | */(1U << 0))) |
			//1 * (1U << 1) |		// LVINT	1: Level output
			1 * (1U << 0) |		// PR		1: Round robin mode
			0;

		{
			//const uint16_t int_id = DMAINT15_IRQn;
			//r7s721_intc_registintfunc(int_id, r7s721_usb0_dma1_dmatx_handler);
			//GIC_SetPriority(int_id, ARM_REALTIME_PRIORITY);
			//GIC_EnableIRQ(int_id);
		}

		DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_SWRST;		// SWRST
		DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_CLRINTMSK;	// CLRINTMSK
		//DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_SETEN;		// SETEN
	}
#endif /* WITHSPIHWDMA */

	SPIIO_INITIALIZE();
#else
	#error Wrong CPUSTYLE macro
#endif
}

void hardware_spi_master_setfreq(uint_fast8_t spispeedindex, int_fast32_t spispeed)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	const ldiv_t v = ldiv(CPU_FREQ, spispeed);
	const unsigned long scbr = ulmin(255, (CPU_FREQ > spispeed) ? (v.quot + (v.rem != 0)) : 1);	// 72 MHz / scbr = SPI clock freq
	const unsigned dlybs = 0;
	const unsigned dlybct = 0;

	// 8-ми битые передачи
    const unsigned long csrbits = 
		SPI_CSR_BITS_8_BIT  |	// (SPI) 8 Bits Per transfer
		SPI_CSR_SCBR(scbr) | // (SPI_CSR_SCBR_Msk & (scbr <<  SPI_CSR_SCBR_Pos)) |	// (SPI) Serial Clock Baud Rate
		SPI_CSR_CSAAT |	// (SPI) Chip Select Active After Transfer
		SPI_CSR_DLYBS(dlybs) | // (SPI_CSR_DLYBS_Msk & (dlybs << SPI_CSR_DLYBS_Pos)) |
		SPI_CSR_DLYBCT(dlybct) | // (SPI_CSR_DLYBCT_Msk & (dlybct << SPI_CSR_DLYBCT_Pos)) |
		0;
	// 16-ти битые передачи
    const unsigned long csrbits16w = 
		SPI_CSR_BITS_16_BIT |	// (SPI) 16 Bits Per transfer
		SPI_CSR_SCBR(scbr) | // (SPI_CSR_SCBR_Msk & (scbr <<  SPI_CSR_SCBR_Pos)) |	// (SPI) Serial Clock Baud Rate
		SPI_CSR_CSAAT |	// (SPI) Chip Select Active After Transfer
		SPI_CSR_DLYBS(dlybs) | // (SPI_CSR_DLYBS_Msk & (dlybs << SPI_CSR_DLYBS_Pos)) |
		SPI_CSR_DLYBCT(dlybct) | // (SPI_CSR_DLYBCT_Msk & (dlybct << SPI_CSR_DLYBCT_Pos)) |
		0;

	// подготовка управляющих слов для разных spi mode, используемых контроллером.
	// 8-битная передача
	spi_csr_val8w [spispeedindex][SPIC_MODE0] = csrbits | SPI_CSR_NCPHA;
	spi_csr_val8w [spispeedindex][SPIC_MODE1] = csrbits;
	spi_csr_val8w [spispeedindex][SPIC_MODE2] = csrbits | SPI_CSR_CPOL | SPI_CSR_NCPHA;
	spi_csr_val8w [spispeedindex][SPIC_MODE3] = csrbits | SPI_CSR_CPOL;

	// подготовка управляющих слов для разных spi mode, используемых контроллером.
	// 16-битная передача
	spi_csr_val16w [spispeedindex][SPIC_MODE0] = csrbits16w | SPI_CSR_NCPHA;
	spi_csr_val16w [spispeedindex][SPIC_MODE1] = csrbits16w;
	spi_csr_val16w [spispeedindex][SPIC_MODE2] = csrbits16w | SPI_CSR_CPOL | SPI_CSR_NCPHA;
	spi_csr_val16w [spispeedindex][SPIC_MODE3] = csrbits16w | SPI_CSR_CPOL;

#elif CPUSTYLE_AT91SAM7S

	const ldiv_t v = ldiv(CPU_FREQ, spispeed);
	const unsigned long scbr = ulmin(255, (CPU_FREQ > spispeed) ? (v.quot + (v.rem != 0)) : 1);	// 72 MHz / scbr = SPI clock freq
	const unsigned dlybs = 0;
	const unsigned dlybct = 0;

	// 8-ми битые передачи
    const unsigned long csrbits = 
		AT91C_SPI_BITS_8 |	// (SPI) 8 Bits Per transfer
		(AT91C_SPI_SCBR & (scbr <<  8)) |	// (SPI) Serial Clock Baud Rate
		AT91C_SPI_CSAAT |	// (SPI) Chip Select Active After Transfer
		(AT91C_SPI_DLYBS & (dlybs << 16)) |
		(AT91C_SPI_DLYBCT & (dlybct << 24)) |
		0;
	// 16-ти битые передачи
    const unsigned long csrbits16w = 
		AT91C_SPI_BITS_16 |	// (SPI) 16 Bits Per transfer
		(AT91C_SPI_SCBR & (scbr <<  8)) |	// (SPI) Serial Clock Baud Rate
		AT91C_SPI_CSAAT |	// (SPI) Chip Select Active After Transfer
		(AT91C_SPI_DLYBS & (dlybs << 16)) |
		(AT91C_SPI_DLYBCT & (dlybct << 24)) |
		0;

	// подготовка управляющих слов для разных spi mode, используемых контроллером.
	// 8-битная передача
	spi_csr_val8w [spispeedindex][SPIC_MODE0] = csrbits | AT91C_SPI_NCPHA;
	spi_csr_val8w [spispeedindex][SPIC_MODE1] = csrbits;
	spi_csr_val8w [spispeedindex][SPIC_MODE2] = csrbits | AT91C_SPI_CPOL | AT91C_SPI_NCPHA;
	spi_csr_val8w [spispeedindex][SPIC_MODE3] = csrbits | AT91C_SPI_CPOL;

	// подготовка управляющих слов для разных spi mode, используемых контроллером.
	// 16-битная передача
	spi_csr_val16w [spispeedindex][SPIC_MODE0] = csrbits16w | AT91C_SPI_NCPHA;
	spi_csr_val16w [spispeedindex][SPIC_MODE1] = csrbits16w;
	spi_csr_val16w [spispeedindex][SPIC_MODE2] = csrbits16w | AT91C_SPI_CPOL | AT91C_SPI_NCPHA;
	spi_csr_val16w [spispeedindex][SPIC_MODE3] = csrbits16w | AT91C_SPI_CPOL;

#elif CPUSTYLE_ATMEGA

	// SPI initialization

	// Использование автоматического расчёта предделителя
	unsigned value;	/* делителя нет, есть только прескалер - значение делителя не используется */
	const uint_fast8_t prei = calcdivider(calcdivround(spispeed), ATMEGA_SPCR_WIDTH, ATMEGA_SPCR_TAPS, & value, 1);
	const uint_fast8_t spcr = spcr_spsr [prei].spcr | (1U << SPE) | (1U << MSTR);
	// С FRAM FM25L04 работает MODE3 и MODE0
	spcr_val [spispeedindex][SPIC_MODE0] = (0U << CPOL) | (0U << CPHA) | spcr;
	spcr_val [spispeedindex][SPIC_MODE1] = (0U << CPOL) | (1U << CPHA) | spcr;
	spcr_val [spispeedindex][SPIC_MODE2] = (1U << CPOL) | (0U << CPHA) | spcr;
	spcr_val [spispeedindex][SPIC_MODE3] = (1U << CPOL) | (1U << CPHA) | spcr;

	spsr_val [spispeedindex] = spcr_spsr [prei].spsr;	// SPI2X bit

#elif CPUSTYLE_ATXMEGA

	// SPI initialization

	// Использование автоматического расчёта предделителя
	unsigned value;	/* делителя нет, есть только прескалер - значение делителя не используется */
	const uint_fast8_t prei = calcdivider(calcdivround(spispeed), ATXMEGA_SPIBR_WIDTH, ATXMEGA_SPIBR_TAPS, & value, 1);
	const uint_fast8_t ctl = spi_ctl [prei] | SPI_MASTER_bm | SPI_ENABLE_bm;
	// С FRAM FM25L04 работает MODE3 и MODE0
	spi_ctl_val [spispeedindex][SPIC_MODE0] = SPI_MODE_0_gc | ctl;	// SPI MODE0,
	spi_ctl_val [spispeedindex][SPIC_MODE1] = SPI_MODE_1_gc | ctl;	// SPI MODE1,
	spi_ctl_val [spispeedindex][SPIC_MODE2] = SPI_MODE_2_gc | ctl;	// SPI MODE2,
	spi_ctl_val [spispeedindex][SPIC_MODE3] = SPI_MODE_3_gc | ctl;	// SPI MODE3,

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32L0XX

	unsigned value;	/* делителя нет, есть только прескалер - значение делителя не используется */
	const uint_fast8_t prei = calcdivider(calcdivround_pclk2(spispeed), STM32F_SPIBR_WIDTH, STM32F_SPIBR_TAPS, & value, 1);

	const uint_fast32_t cr1baudrate = (prei * SPI_CR1_BR_0) & SPI_CR1_BR;
	// When the SSM bit is set, the NSS pin input is replaced with the value from the SSI bit.
	// This bit has an effect only when the SSM bit is set. The value of this bit is forced onto the NSS pin and the IO value of the NSS pin is ignored.
	const uint_fast32_t cr1bits = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR | SPI_CR1_SPE | cr1baudrate;
	const uint_fast32_t cr1bits16w = cr1bits | SPI_CR1_DFF;
	enum
	{
		CR1_MODE0 = 0,				// TODO: not tested
		CR1_MODE1 = SPI_CR1_CPHA,	// TODO: not tested
		CR1_MODE2 = SPI_CR1_CPOL,	// CLK leave HIGH	
		CR1_MODE3 = SPI_CR1_CPOL | SPI_CR1_CPHA		// wrk = CLK leave "HIGH"
	};

	// подготовка управляющих слов для разных spi mode, используемых контроллером.
	// 8-битная передача
	spi_cr1_val8w [spispeedindex][SPIC_MODE0] = cr1bits | CR1_MODE0;
	spi_cr1_val8w [spispeedindex][SPIC_MODE1] = cr1bits | CR1_MODE1;
	spi_cr1_val8w [spispeedindex][SPIC_MODE2] = cr1bits | CR1_MODE2;
	spi_cr1_val8w [spispeedindex][SPIC_MODE3] = cr1bits | CR1_MODE3; 

	// подготовка управляющих слов для разных spi mode, используемых контроллером.
	// 16-битная передача
	spi_cr1_val16w [spispeedindex][SPIC_MODE0] = cr1bits16w | CR1_MODE0;
	spi_cr1_val16w [spispeedindex][SPIC_MODE1] = cr1bits16w | CR1_MODE1;
	spi_cr1_val16w [spispeedindex][SPIC_MODE2] = cr1bits16w | CR1_MODE2;	
	spi_cr1_val16w [spispeedindex][SPIC_MODE3] = cr1bits16w | CR1_MODE3; 

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX

	unsigned value;	/* делителя нет, есть только прескалер - значение делителя не используется */
	const uint_fast8_t prei = calcdivider(calcdivround_pclk2(spispeed), STM32F_SPIBR_WIDTH, STM32F_SPIBR_TAPS, & value, 1);

	const uint_fast32_t cr1baudrate = (prei * SPI_CR1_BR_0) & SPI_CR1_BR;
	const uint_fast32_t cr1bits = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR | SPI_CR1_SPE | cr1baudrate;
	enum
	{
		CR1_MODE0 = 0,				// TODO: not tested
		CR1_MODE1 = SPI_CR1_CPHA,	// TODO: not tested
		CR1_MODE2 = SPI_CR1_CPOL,	// CLK leave HIGH	
		CR1_MODE3 = SPI_CR1_CPOL | SPI_CR1_CPHA		// wrk = CLK leave "HIGH"
	};

	// подготовка управляющих слов для разных spi mode, используемых контроллером.
	// 8-битная или 16-битная передача программируется в CR2
	spi_cr1_val8w [spispeedindex][SPIC_MODE0] = cr1bits | CR1_MODE0;
	spi_cr1_val8w [spispeedindex][SPIC_MODE1] = cr1bits | CR1_MODE1;
	spi_cr1_val8w [spispeedindex][SPIC_MODE2] = cr1bits | CR1_MODE2;
	spi_cr1_val8w [spispeedindex][SPIC_MODE3] = cr1bits | CR1_MODE3; 

#elif CPUSTYLE_STM32H7XX

	unsigned value;	/* делителя нет, есть только прескалер - значение делителя не используется */
	const uint_fast8_t prei = calcdivider(calcdivround_per_ck(spispeed), STM32F_SPIBR_WIDTH, STM32F_SPIBR_TAPS, & value, 1);
	const uint_fast32_t cfg1baudrate = (prei * SPI_CFG1_MBR_0) & SPI_CFG1_MBR;
	//debug_printf_P(PSTR("hardware_spi_master_setfreq: prei=%u, value=%u, spispeed=%u\n"), prei, value, spispeed);

	spi_cfg1_val8w = cfg1baudrate | 
		7 * SPI_CFG1_DSIZE_0 |
		0;

	spi_cfg1_val16w = cfg1baudrate | 
		15 * SPI_CFG1_DSIZE_0 |
		0;

	const uint_fast32_t cfg2bits = SPI_CFG2_SSM | SPI_CFG2_MASTER /* | SPI_CFG2_AFCNTR */;
	enum
	{
		CFG2_MODE0 = 0,				// TODO: not tested
		CFG2_MODE1 = SPI_CFG2_CPHA,	// TODO: not tested
		CFG2_MODE2 = SPI_CFG2_CPOL,	// CLK leave HIGH	
		CFG2_MODE3 = SPI_CFG2_CPOL | SPI_CFG2_CPHA		// wrk = CLK leave "HIGH"
	};

	// подготовка управляющих слов для разных spi mode, используемых контроллером.
	// 8-битная или 16-битная передача программируется в CR2
	spi_cfg2_val [spispeedindex][SPIC_MODE0] = cfg2bits | CFG2_MODE0;
	spi_cfg2_val [spispeedindex][SPIC_MODE1] = cfg2bits | CFG2_MODE1;
	spi_cfg2_val [spispeedindex][SPIC_MODE2] = cfg2bits | CFG2_MODE2;	
	spi_cfg2_val [spispeedindex][SPIC_MODE3] = cfg2bits | CFG2_MODE3; 

#elif CPUSTYLE_R7S721

	// Command Register (SPCMD)
	// подготовка управляющих слов для разных spi mode, используемых контроллером.
	enum
	{
		SPCMD_CPHA = RSPIn_SPCMD0_CPHA,
		SPCMD_CPOL = RSPIn_SPCMD0_CPOL,

		SPCMD_MODE0 = 0,
		SPCMD_MODE1 = SPCMD_CPHA,
		SPCMD_MODE2 = SPCMD_CPOL,
		SPCMD_MODE3 = SPCMD_CPOL | SPCMD_CPHA
	};

	unsigned value;	
	const uint_fast8_t prei = calcdivider(calcdivround_p1clock(spispeed), R7S721_RSPI_SPBR_WIDTH, R7S721_RSPI_SPBR_TAPS, & value, 1);

	const uint_fast8_t spcmd0 =	// Command Register (SPCMD)
		(RSPIn_SPCMD0_BRDV & (prei << RSPIn_SPCMD0_BRDV_SHIFT)) |	// BRDV1..BRDV0 - Bit Rate Division Setting /1, /2, /4. /8
		0;

	spi_spbr_val [spispeedindex] = value;	// Bit Rate Register (SPBR)
	// подготовка управляющих слов для разных spi mode, используемых контроллером.
	// 8-битная передача
	const uint16_t spcmd8bw = spcmd0 | 0x0700;	// 0x0700 - 8 bit

	spi_spcmd0_val8w [spispeedindex][SPIC_MODE0] = spcmd8bw | SPCMD_MODE0;
	spi_spcmd0_val8w [spispeedindex][SPIC_MODE1] = spcmd8bw | SPCMD_MODE1;
	spi_spcmd0_val8w [spispeedindex][SPIC_MODE2] = spcmd8bw | SPCMD_MODE2;
	spi_spcmd0_val8w [spispeedindex][SPIC_MODE3] = spcmd8bw | SPCMD_MODE3;

	// подготовка управляющих слов для разных spi mode, используемых контроллером.
	// 16-битная передача
	const uint16_t spcmd16w = spcmd0 | 0x0F00;	// 0x0200 or 0x0300 - 32 bit

	spi_spcmd0_val16w [spispeedindex][SPIC_MODE0] = spcmd16w | SPCMD_MODE0;
	spi_spcmd0_val16w [spispeedindex][SPIC_MODE1] = spcmd16w | SPCMD_MODE1;
	spi_spcmd0_val16w [spispeedindex][SPIC_MODE2] = spcmd16w | SPCMD_MODE2;
	spi_spcmd0_val16w [spispeedindex][SPIC_MODE3] = spcmd16w | SPCMD_MODE3;

#else
	#error Wrong CPUSTYLE macro
#endif
}

/* управление состоянием "подключено */
void hardware_spi_connect(uint_fast8_t spispeedindex, uint_fast8_t spimode)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	// инициализация контроллера SPI
	enum { OUTMASK = PIO_PA13A_MOSI | PIO_PA14A_SPCK };		// битовая маска, определяет каким выводом шевелить
	enum { INPMASK = PIO_PA12A_MISO };		// битовая маска, определяет откуда ввод
	enum { WORKMASK = OUTMASK | INPMASK };		// битовая маска, включает и ввод и вывод

	SPI->SPI_CSR [0] = spi_csr_val8w [spispeedindex][spimode];

	(void) SPI->SPI_RDR;		/* clear AT91C_SPI_RDRF in status register */

	HARDWARE_SPI_CONNECT();

#elif CPUSTYLE_AT91SAM7S

	enum { OUTMASK = AT91C_PA13_MOSI | AT91C_PA14_SPCK };		// битовая маска, определяет каким выводом шевелить
	enum { INPMASK = AT91C_PA12_MISO };		// битовая маска, определяет откуда ввод
	enum { WORKMASK = OUTMASK | INPMASK };		// битовая маска, включает и ввод и вывод

	AT91C_SPI_CSR [0] = spi_csr_val8w [spispeedindex][spimode];

	(void) AT91C_BASE_SPI->SPI_RDR;		/* clear AT91C_SPI_RDRF in status register */

	HARDWARE_SPI_CONNECT();

#elif CPUSTYLE_ATMEGA

	SPSR = spsr_val [spispeedindex];		// D0 is SPI2X bit, other bits has no effect at write.
	SPCR = spcr_val [spispeedindex][spimode];

	HARDWARE_SPI_CONNECT();

	(void) SPDR;	/* clear SPIF in status register */

#elif CPUSTYLE_ATXMEGA

	TARGETHARD_SPI.CTRL = spi_ctl_val [spispeedindex][spimode];
 	/* MOSI and SCK as output, MISO as input. */
	//SPI_TARGET_DDR_S  = SPI_MOSI_BIT | SPI_SCLK_BIT;
	//SPI_TARGET_DDR_C  = SPI_MISO_BIT;

	HARDWARE_SPI_CONNECT();

	(void) TARGETHARD_SPI.DATA;	/* clear SPIF in status register */

#elif CPUSTYLE_STM32F1XX

	HARDWARE_SPI_CONNECT();

	#if WITHTWIHW
		// Silicon errata: 
		// 2.6.7 I2C1 with SPI1 remapped and used in master mode
		// Workaround:
		// When using SPI1 remapped, the I2C1 clock must be disabled.
		RCC->APB1ENR &= ~ (RCC_APB1ENR_I2C1EN); // выкл тактирование контроллера I2C
		__DSB();
	#endif
	SPI1->CR1 = spi_cr1_val8w [spispeedindex][spimode];

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32L0XX

	// В этих процессорах и входы и выходы перекдючаются на ALT FN
	HARDWARE_SPI_CONNECT();

	#if WITHTWIHW
		// Silicon errata: 
		// 2.6.7 I2C1 with SPI1 remapped and used in master mode
		// Workaround:
		// When using SPI1 remapped, the I2C1 clock must be disabled.
		RCC->APB1ENR &= ~ (RCC_APB1ENR_I2C1EN); // выкл тактирование контроллера I2C
		__DSB();
	#endif
	SPI1->CR1 = spi_cr1_val8w [spispeedindex][spimode];

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX

	// В этих процессорах и входы и выходы перекдючаются на ALT FN
	HARDWARE_SPI_CONNECT();

	#if WITHTWIHW
		// Silicon errata: 
		// 2.6.7 I2C1 with SPI1 remapped and used in master mode
		// Workaround:
		// When using SPI1 remapped, the I2C1 clock must be disabled.
		RCC->APB1ENR &= ~ (RCC_APB1ENR_I2C1EN); // выкл тактирование контроллера I2C
		__DSB();
	#endif
	SPI1->CR1 = spi_cr1_val8w [spispeedindex][spimode];
	SPI1->CR2 = (SPI1->CR2 & ~ (SPI_CR2_DS)) |
		7 * SPI_CR2_DS_0 |	// 8 bit word length
		1 * SPI_CR2_FRXTH |			// RXFIFO threshold is set to 8 bits (FRXTH=1).
		0;

#elif CPUSTYLE_STM32H7XX

	HARDWARE_SPI_CONNECT();

	SPI1->CR1 = SPI_CR1_SSI;

	SPI1->CFG1 = spi_cfg1_val8w;
	SPI1->CFG2 = spi_cfg2_val [spispeedindex][spimode];

	SPI1->CR1 |= SPI_CR1_SPE;
	SPI1->CR1 |= SPI_CR1_CSTART;

#elif CPUSTYLE_R7S721

	RSPI0.SPDCR =		/* Data Control Register (SPDCR) */
		(0x01 << 5) |	// 0x01: 8 bit. Specifies the width for accessing the data register (SPDR)
		0;
	RSPI0.SPBR = spi_spbr_val [spispeedindex];
	RSPI0.SPCMD0 = spi_spcmd0_val8w [spispeedindex][spimode];

	HARDWARE_SPI_CONNECT();

#else
	#error Wrong CPUSTYLE macro
#endif

}

/* управление состоянием "подключено" */
void hardware_spi_disconnect(void)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	HARDWARE_SPI_DISCONNECT();

#elif CPUSTYLE_AT91SAM7S

	HARDWARE_SPI_DISCONNECT();

#elif CPUSTYLE_ATMEGA

	HARDWARE_SPI_DISCONNECT();

#elif CPUSTYLE_ATXMEGA

	HARDWARE_SPI_DISCONNECT();

#elif CPUSTYLE_STM32F

	SPI1->CR1 &= ~ SPI_CR1_SPE;

	#if WITHTWIHW && ! CPUSTYLE_STM32H7XX
		// Silicon errata: 
		// 2.6.7 I2C1 with SPI1 remapped and used in master mode
		// Workaround:
		// When using SPI1 remapped, the I2C1 clock must be disabled.
		RCC->APB1ENR |= (RCC_APB1ENR_I2C1EN); // вкл тактирование контроллера I2C
		__DSB();
	#endif

	// connect back to GPIO
	HARDWARE_SPI_DISCONNECT();

#elif CPUSTYLE_R7S721

	HARDWARE_SPI_DISCONNECT();

#else
	#error Wrong CPUSTYLE macro
#endif

}

/* дождаться завершения передачи (на atmega оптимизированно по скорости - без чиения регистра данных). */
static void
hardware_spi_ready_b8_void(void)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	/* дождаться завершения приёма/передачи */
	while ((SPI->SPI_SR & SPI_SR_RDRF) == 0)
		;
	(void) SPI->SPI_RDR;

#elif CPUSTYLE_AT91SAM7S

	/* дождаться завершения приёма/передачи */
	while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_RDRF) == 0)
		;
	(void) AT91C_BASE_SPI->SPI_RDR;

#elif CPUSTYLE_ATMEGA

	/* дождаться завершения приёма/передачи */
	while ((SPSR & (1U << SPIF)) == 0)
		;
	//(void) SPDR;

#elif CPUSTYLE_ATXMEGA

	/* дождаться завершения приёма/передачи */
	while ((TARGETHARD_SPI.STATUS & SPI_IF_bm) == 0)
		;
	//(void) TARGETHARD_SPI.DATA;

#elif CPUSTYLE_STM32H7XX

	//while ((SPI1->SR & SPI_SR_TXC) == 0)	
	//	;
	while ((SPI1->SR & SPI_SR_RXP) == 0)	
		;
	(void) * (volatile uint8_t *) & SPI1->RXDR;	/* clear SPI_SR_RXP in status register */


#elif CPUSTYLE_STM32F

	while ((SPI1->SR & SPI_SR_RXNE) == 0)	
		;
	(void) SPI1->DR;	/* clear SPI_SR_RXNE in status register */

#elif CPUSTYLE_R7S721

	while ((RSPI0.SPSR & (1U << 7)) == 0)	// SPRF bit
		;
	(void) RSPI0.SPDR.UINT8 [R_IO_LL]; // LL=0

#else
	#error Wrong CPUSTYLE macro
#endif
}

portholder_t hardware_spi_complete_b8(void)	/* дождаться готовности */
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	/* дождаться завершения приёма/передачи */
	while ((SPI->SPI_SR & SPI_SR_RDRF) == 0)
		;
	return (SPI->SPI_RDR & SPI_TDR_TD_Msk);

#elif CPUSTYLE_AT91SAM7S

	/* дождаться завершения приёма/передачи */
	while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_RDRF) == 0)
		;
	return (AT91C_BASE_SPI->SPI_RDR & AT91C_SPI_TD);

#elif CPUSTYLE_ATMEGA

	/* дождаться завершения приёма/передачи */
	while ((SPSR & (1U << SPIF)) == 0)
		;
	return SPDR;

#elif CPUSTYLE_ATXMEGA

	/* дождаться завершения приёма/передачи */
	while ((TARGETHARD_SPI.STATUS & SPI_IF_bm) == 0)
		;
	return TARGETHARD_SPI.DATA;

#elif CPUSTYLE_STM32H7XX

	//while ((SPI1->SR & SPI_SR_TXC) == 0)	
	//	;
	while ((SPI1->SR & SPI_SR_RXP) == 0)	
		;
	const portholder_t t = * (volatile uint8_t *) & SPI1->RXDR;	// prevent data packing feature
	return t;

#elif CPUSTYLE_STM32F

	while ((SPI1->SR & SPI_SR_RXNE) == 0)	
		;
	const portholder_t t = SPI1->DR & 0xFF;	/* clear SPI_SR_RXNE in status register */
	while ((SPI1->SR & SPI_SR_BSY) != 0)	
		;
	return t;

#elif CPUSTYLE_R7S721

	while ((RSPI0.SPSR & (1U << 7)) == 0)	// SPRF bit
		;
	return RSPI0.SPDR.UINT8 [R_IO_LL]; // LL=0

#else
	#error Wrong CPUSTYLE macro
#endif
}


#if WITHSPIHWDMA

static void 
hardware_spi_master_setdma8bit_rx(void)
{
#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	// DMA2: SPI1_RX: Stream 0: Channel 3
	DMA2_Stream0->CR = (DMA2_Stream0->CR & ~ (DMA_SxCR_MSIZE | DMA_SxCR_PSIZE)) |
		(0 * DMA_SxCR_MSIZE_0) |	// длина в памяти - 8bit
		(0 * DMA_SxCR_PSIZE_0) |	// длина в SPI_DR- 8bit
		0;
#elif CPUSTYLE_R7S721
	DMAC15.N0SA_n = (uint32_t) & RSPI0.SPDR.UINT8 [R_IO_LL];	// Fixed destination address for 8-bit transfers
	//DMAC15.N0SA_n = (uint32_t) & RSPI0.SPDR.UINT16 [R_IO_L];	// Fixed destination address for 16-bit transfers
	DMAC15.CHCFG_n = (DMAC15.CHCFG_n & ~ (DMAC15_CHCFG_n_DDS | DMAC15_CHCFG_n_SDS | DMAC15_CHCFG_n_DAD | DMAC15_CHCFG_n_SAD)) |
		0 * (1U << DMAC15_CHCFG_n_DDS_SHIFT) |	// DDS	2: 32 bits, 1: 16 bits (Destination Data Size)
		0 * (1U << DMAC15_CHCFG_n_SDS_SHIFT) |	// SDS	2: 32 bits, 1: 16 bits (Source Data Size)
		0 * (1U << DMAC15_CHCFG_n_DAD_SHIFT) |		// DAD	0: Increment destination address
		1 * (1U << DMAC15_CHCFG_n_SAD_SHIFT) |		// SAD	1: Fixed source address
		0;
#endif
}

static void 
hardware_spi_master_setdma16bit_rx(void)
{
#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	// DMA2: SPI1_RX: Stream 0: Channel 3
	DMA2_Stream0->CR = (DMA2_Stream0->CR & ~ (DMA_SxCR_MSIZE | DMA_SxCR_PSIZE)) |
		(1 * DMA_SxCR_MSIZE_0) |	// длина в памяти - 16bit
		(1 * DMA_SxCR_PSIZE_0) |	// длина в SPI_DR- 16bit
		0;
#elif CPUSTYLE_R7S721
	//DMAC15.N0SA_n = (uint32_t) & RSPI0.SPDR.UINT8 [R_IO_LL];	// Fixed source address for 8-bit transfers
	DMAC15.N0SA_n = (uint32_t) & RSPI0.SPDR.UINT16 [R_IO_L];	// Fixed source address for 16-bit transfers
	DMAC15.CHCFG_n = (DMAC15.CHCFG_n & ~ (DMAC15_CHCFG_n_DDS | DMAC15_CHCFG_n_SDS | DMAC15_CHCFG_n_DAD | DMAC15_CHCFG_n_SAD)) |
		1 * (1U << DMAC15_CHCFG_n_DDS_SHIFT) |	// DDS	2: 32 bits, 1: 16 bits (Destination Data Size)
		1 * (1U << DMAC15_CHCFG_n_SDS_SHIFT) |	// SDS	2: 32 bits, 1: 16 bits (Source Data Size)
		0 * (1U << DMAC15_CHCFG_n_DAD_SHIFT) |		// DAD	0: Increment destination address
		1 * (1U << DMAC15_CHCFG_n_SAD_SHIFT) |		// SAD	1: Fixed source address
		0;
#endif
}

static void 
hardware_spi_master_setdma8bit_tx(void)
{
#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	// DMA2: SPI1_TX: Stream 3: Channel 3
	DMA2_Stream3->CR = (DMA2_Stream3->CR & ~ (DMA_SxCR_MSIZE | DMA_SxCR_PSIZE)) |
		(0 * DMA_SxCR_MSIZE_0) |	// длина в памяти - 8bit
		(0 * DMA_SxCR_PSIZE_0) |	// длина в SPI_DR- 8bit
		0;
#elif CPUSTYLE_R7S721
	DMAC15.N0DA_n = (uint32_t) & RSPI0.SPDR.UINT8 [R_IO_LL];	// Fixed destination address for 8-bit transfers
	//DMAC15.N0DA_n = (uint32_t) & RSPI0.SPDR.UINT16 [R_IO_L];	// Fixed destination address for 16-bit transfers
	DMAC15.CHCFG_n = (DMAC15.CHCFG_n & ~ (DMAC15_CHCFG_n_DDS | DMAC15_CHCFG_n_SDS | DMAC15_CHCFG_n_DAD | DMAC15_CHCFG_n_SAD)) |
		0 * (1U << DMAC15_CHCFG_n_DDS_SHIFT) |	// DDS	2: 32 bits, 1: 16 bits (Destination Data Size)
		0 * (1U << DMAC15_CHCFG_n_SDS_SHIFT) |	// SDS	2: 32 bits, 1: 16 bits (Source Data Size)
		1 * (1U << DMAC15_CHCFG_n_DAD_SHIFT) |		// DAD	1: Fixed destination address
		0 * (1U << DMAC15_CHCFG_n_SAD_SHIFT) |		// SAD	0: Increment source address
		0;
#endif
}

static void 
hardware_spi_master_setdma16bit_tx(void)
{
#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	// DMA2: SPI1_TX: Stream 3: Channel 3
	DMA2_Stream3->CR = (DMA2_Stream3->CR & ~ (DMA_SxCR_MSIZE | DMA_SxCR_PSIZE)) |
		(1 * DMA_SxCR_MSIZE_0) |	// длина в памяти - 16bit
		(1 * DMA_SxCR_PSIZE_0) |	// длина в SPI_DR- 16bit
		0;
#elif CPUSTYLE_R7S721
	//DMAC15.N0DA_n = (uint32_t) & RSPI0.SPDR.UINT8 [R_IO_LL];	// Fixed destination address for 8-bit transfers
	DMAC15.N0DA_n = (uint32_t) & RSPI0.SPDR.UINT16 [R_IO_L];	// Fixed destination address for 16-bit transfers
	DMAC15.CHCFG_n = (DMAC15.CHCFG_n & ~ (DMAC15_CHCFG_n_DDS | DMAC15_CHCFG_n_SDS | DMAC15_CHCFG_n_DAD | DMAC15_CHCFG_n_SAD)) |
		1 * (1U << DMAC15_CHCFG_n_DDS_SHIFT) |	// DDS	2: 32 bits, 1: 16 bits (Destination Data Size)
		1 * (1U << DMAC15_CHCFG_n_SDS_SHIFT) |	// SDS	2: 32 bits, 1: 16 bits (Source Data Size)
		1 * (1U << DMAC15_CHCFG_n_DAD_SHIFT) |		// DAD	1: Fixed destination address
		0 * (1U << DMAC15_CHCFG_n_SAD_SHIFT) |		// SAD	0: Increment source address
		0;
#endif
}

// Send a frame of bytes via SPI
static void 
hardware_spi_master_send_frame_8bpartial(
	//spitarget_t target,	/* addressing to chip */
	const uint8_t * buffer, 
	uint_fast32_t size		/* количество пересылаемых 8-ти битных элементов */
	)
{
#if 0
	// имитация
	if (size == 1)
	{
		hardware_spi_b8_p1(* buffer);
		hardware_spi_complete_b8();
	}
	else
	{
		hardware_spi_b8_p1(* buffer ++);
		size -= 1;
		while (size --)
			hardware_spi_b8_p2(* buffer ++);
		hardware_spi_complete_b8();
	}

#elif CPUSTYLE_SAM9XE

	AT91C_BASE_SPI1->SPI_TPR = (unsigned long) buffer;
	AT91C_BASE_SPI1->SPI_TCR = size;	// запуск передатчика

	while ((AT91C_BASE_SPI1->SPI_SR & AT91C_SPI_ENDTX) == 0)
		;
	// дождаться, пока последний байт выйдет из передатчика
	while ((AT91C_BASE_SPI1->SPI_SR & AT91C_SPI_TXEMPTY) == 0)
		;
	// сбростить возможно имеющийся флаг готовности приёмника
	(void) AT91C_BASE_SPI1->SPI_RDR;

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	SPI->SPI_TPR = (unsigned long) buffer;
	SPI->SPI_TCR = size;	// запуск передатчика

	while ((SPI->SPI_SR & SPI_SR_ENDTX) == 0)
		;
	// дождаться, пока последний байт выйдет из передатчика
	while ((SPI->SPI_SR & SPI_SR_TXEMPTY) == 0)
		;
	// сбростить возможно имеющийся флаг готовности приёмника
	(void) SPI->SPI_RDR;

#elif CPUSTYLE_AT91SAM7S

	AT91C_BASE_SPI->SPI_TPR = (unsigned long) buffer;
	AT91C_BASE_SPI->SPI_TCR = size;	// запуск передатчика

	while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_ENDTX) == 0)
		;
	// дождаться, пока последний байт выйдет из передатчика
	while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0)
		;
	// сбростить возможно имеющийся флаг готовности приёмника
	(void) AT91C_BASE_SPI->SPI_RDR;

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	// buffer should be allocated in RAM, not in CCM or FLASH

	/*
	if (((uint32_t) buffer & 0xFF000000) == CCMDATARAM_BASE)
	{
		debug_printf_P(PSTR("hardware_spi_master_send_frame: use CCM\n"));
		// Safe version
		prog_spi_send_frame(target, buffer, size);
		return;
	}
	*/

	// DMA2: SPI1_TX: Stream 3: Channel 3
	#if CPUSTYLE_STM32H7XX
		SPI1->CFG1 |= SPI_CFG1_TXDMAEN; // DMA по передаче
	#else /* CPUSTYLE_STM32H7XX */
		SPI1->CR2 |= SPI_CR2_TXDMAEN; // DMA по передаче
	#endif /* CPUSTYLE_STM32H7XX */

	DMA2_Stream3->M0AR = (uintptr_t) buffer;
	DMA2_Stream3->NDTR = (DMA2_Stream3->NDTR & ~ DMA_SxNDT) |
		(size * DMA_SxNDT_0);
	DMA2_Stream3->CR |= DMA_SxCR_EN;		// перезапуск DMA

	// Дожидаемся завершения обмена передающего канала DMA
	while ((DMA2->LISR & DMA_LISR_TCIF3) == 0)	// ожидаем завершения обмена по соответствушему stream
		;
	DMA2->LIFCR = DMA_LIFCR_CTCIF3;		// сбросил флаг соответствующий stream
	//DMA2_waitTC(3);	// ожидаем завершения обмена по соответствушему stream

	#if CPUSTYLE_STM32H7XX

		while ((SPI1->SR & SPI_SR_TXC) == 0)	
			;
		//while ((SPI1->SR & SPI_SR_BSY) != 0)	
		//	;
		(void) SPI1->RXDR;	/* clear SPI_SR_RXNE in status register */

	#else /* CPUSTYLE_STM32H7XX */

		while ((SPI1->SR & SPI_SR_TXE) == 0)	
			;
		while ((SPI1->SR & SPI_SR_BSY) != 0)	
			;
		(void) SPI1->DR;	/* clear SPI_SR_RXNE in status register */

	#endif /* CPUSTYLE_STM32H7XX */


	#if CPUSTYLE_STM32H7XX
		SPI1->CFG1 &= ~ SPI_CFG1_TXDMAEN; // запретить DMA по передаче
	#else /* CPUSTYLE_STM32H7XX */
		SPI1->CR2 &= ~ SPI_CR2_TXDMAEN; // запретить DMA по передаче
	#endif /* CPUSTYLE_STM32H7XX */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX
	#warning TODO: implement SPI over DMA

	//prog_spi_send_frame(target, buffer, size);

#elif CPUSTYLE_STM32F1XX
	#warning TODO: implement SPI over DMA

	//prog_spi_send_frame(target, buffer, size);

#elif CPUSTYLE_ATXMEGA
	#warning TODO: implement SPI over DMA
	
	//prog_spi_send_frame(target, buffer, size);

#elif CPUSTYLE_R7S721

	RSPI0.SPBFCR |= RSPIn_SPBFCR_RXRST;		// Запретить прием

	DMAC15.N0TB_n = (uint_fast32_t) size * sizeof (* buffer);	// размер в байтах
	DMAC15.N0SA_n = (uintptr_t) buffer;			// source address
	DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_SETEN;		// SETEN

	/* ждем окончания пересылки */
	while ((DMAC15.CHSTAT_n & DMAC15_CHSTAT_n_END) == 0)	// END
		;

	/* ждем окончания пеердачи последнего элемента */
	while ((RSPI0.SPSR & RSPIn_SPSR_TEND) == 0)	// TEND bit
		;

	DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_CLREN;		// CLREN
	DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_CLRTC;		// CLRTC
	DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_CLREND;		// CLREND

	RSPI0.SPBFCR &= ~ RSPIn_SPBFCR_RXRST;		// Разрешить прием

#else
	#error Undefined CPUSTYLE_xxxx
#endif
}

#if WITHSPI16BIT

// Send a frame of 16-bit words via SPI
static void 
hardware_spi_master_send_frame_16bpartial(
	//spitarget_t target,	/* addressing to chip */
	const uint16_t * buffer, 
	uint_fast32_t size		/* количество пересылаемых 16-ти битных элементов */
	)
{
#if 0
	// имитация
	if (size == 1)
	{
		hardware_spi_b16_p1(* buffer);
		hardware_spi_complete_b16();
	}
	else
	{
		hardware_spi_b16_p1(* buffer ++);
		size -= 1;
		while (size --)
			hardware_spi_b16_p2(* buffer ++);
		hardware_spi_complete_b16();
	}

#elif CPUSTYLE_SAM9XE

	AT91C_BASE_SPI1->SPI_TPR = (unsigned long) buffer;
	AT91C_BASE_SPI1->SPI_TCR = size;	// запуск передатчика

	while ((AT91C_BASE_SPI1->SPI_SR & AT91C_SPI_ENDTX) == 0)
		;
	// дождаться, пока последний байт выйдет из передатчика
	while ((AT91C_BASE_SPI1->SPI_SR & AT91C_SPI_TXEMPTY) == 0)
		;
	// сбростить возможно имеющийся флаг готовности приёмника
	(void) AT91C_BASE_SPI1->SPI_RDR;

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	SPI->SPI_TPR = (unsigned long) buffer;
	SPI->SPI_TCR = size;	// запуск передатчика

	while ((SPI->SPI_SR & SPI_SR_ENDTX) == 0)
		;
	// дождаться, пока последний байт выйдет из передатчика
	while ((SPI->SPI_SR & SPI_SR_TXEMPTY) == 0)
		;
	// сбростить возможно имеющийся флаг готовности приёмника
	(void) SPI->SPI_RDR;

#elif CPUSTYLE_AT91SAM7S

	AT91C_BASE_SPI->SPI_TPR = (unsigned long) buffer;
	AT91C_BASE_SPI->SPI_TCR = size;	// запуск передатчика

	while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_ENDTX) == 0)
		;
	// дождаться, пока последний байт выйдет из передатчика
	while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0)
		;
	// сбростить возможно имеющийся флаг готовности приёмника
	(void) AT91C_BASE_SPI->SPI_RDR;

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	// buffer should be allocated in RAM, not in CCM or FLASH

	/*
	if (((uint32_t) buffer & 0xFF000000) == CCMDATARAM_BASE)
	{
		debug_printf_P(PSTR("hardware_spi_master_send_frame: use CCM\n"));
		// Safe version
		prog_spi_send_frame(target, buffer, size);
		return;
	}
	*/

	// DMA2: SPI1_TX: Stream 3: Channel 3
	#if CPUSTYLE_STM32H7XX
		SPI1->CFG1 |= SPI_CFG1_TXDMAEN; // DMA по передаче
	#else /* CPUSTYLE_STM32H7XX */
		SPI1->CR2 |= SPI_CR2_TXDMAEN; // DMA по передаче
	#endif /* CPUSTYLE_STM32H7XX */

	DMA2_Stream3->M0AR = (uintptr_t) buffer;
	DMA2_Stream3->NDTR = (DMA2_Stream3->NDTR & ~ DMA_SxNDT) |
		(size * DMA_SxNDT_0);
	DMA2_Stream3->CR |= DMA_SxCR_EN;		// перезапуск DMA

	// Дожидаемся завершения обмена передающего канала DMA
	while ((DMA2->LISR & DMA_LISR_TCIF3) == 0)	// ожидаем завершения обмена по соответствушему stream
		;
	DMA2->LIFCR = DMA_LIFCR_CTCIF3;		// сбросил флаг соответствующий stream
	//DMA2_waitTC(3);	// ожидаем завершения обмена по соответствушему stream

	#if CPUSTYLE_STM32H7XX

		while ((SPI1->SR & SPI_SR_TXC) == 0)	
			;
		//while ((SPI1->SR & SPI_SR_BSY) != 0)	
		//	;
		(void) SPI1->RXDR;	/* clear SPI_SR_RXNE in status register */

	#else /* CPUSTYLE_STM32H7XX */

		while ((SPI1->SR & SPI_SR_TXE) == 0)	
			;
		while ((SPI1->SR & SPI_SR_BSY) != 0)	
			;
		(void) SPI1->DR;	/* clear SPI_SR_RXNE in status register */

	#endif /* CPUSTYLE_STM32H7XX */

	DMA2_Stream3->CR &= ~ DMA_SxCR_EN;
	while ((DMA2_Stream3->CR &  DMA_SxCR_EN) != 0)
		;

	#if CPUSTYLE_STM32H7XX
		SPI1->CFG1 &= ~ SPI_CFG1_TXDMAEN; // запретить DMA по передаче
	#else /* CPUSTYLE_STM32H7XX */
		SPI1->CR2 &= ~ SPI_CR2_TXDMAEN; // запретить DMA по передаче
	#endif /* CPUSTYLE_STM32H7XX */

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX
	#warning TODO: implement SPI over DMA

	//prog_spi_send_frame_b16(target, buffer, size);

#elif CPUSTYLE_STM32F1XX
	#warning TODO: implement SPI over DMA

	//prog_spi_send_frame_b16(target, buffer, size);

#elif CPUSTYLE_ATXMEGA
	#warning TODO: implement SPI over DMA
	
	//prog_spi_send_frame_b16(target, buffer, size);

#elif CPUSTYLE_R7S721

	RSPI0.SPBFCR |= RSPIn_SPBFCR_RXRST;		// Запретить прием

	DMAC15.N0TB_n = (uint_fast32_t) size * sizeof (* buffer);	// размер в байтах
	DMAC15.N0SA_n = (uintptr_t) buffer;			// source address
	DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_SETEN;		// SETEN

	/* ждем окончания пересылки */
	while ((DMAC15.CHSTAT_n & DMAC15_CHSTAT_n_END) == 0)	// END
		;

	/* ждем окончания пеердачи последнего элемента */
	while ((RSPI0.SPSR & RSPIn_SPSR_TEND) == 0)	// TEND bit
		;

	DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_CLREN;		// CLREN
	DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_CLRTC;		// CLRTC
	DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_CLREND;		// CLREND

	RSPI0.SPBFCR &= ~ RSPIn_SPBFCR_RXRST;		// Разрешить прием

#else
	#error Undefined CPUSTYLE_xxxx
#endif
}

// Read a frame of bytes via SPI
// На сигнале MOSI при это должно обеспачиваться состояние логической "1" для корректной работы SD CARD
static void 
hardware_spi_master_read_frame_16bpartial(
	//spitarget_t target,	/* addressing to chip */
	uint16_t * buffer, 
	uint_fast32_t size		/* количество пересылаемых 16-ти битных элементов */
	)
{
#if CPUSTYLE_SAM9XE

	HARDWARE_SPI_DISCONNECT_MOSI();	// выход данных в "1" - для нормальной работы SD CARD

	AT91C_BASE_SPI1->SPI_TPR = (unsigned long) buffer;
	AT91C_BASE_SPI1->SPI_RPR = (unsigned long) buffer;
	AT91C_BASE_SPI1->SPI_RCR = size;	// разрешить работу приёмника
	AT91C_BASE_SPI1->SPI_TCR = size;	// запуск передатчика (выдача синхронизации)

	while ((AT91C_BASE_SPI1->SPI_SR & AT91C_SPI_ENDRX) == 0)	// было TX
		;
	// дождаться, пока последний байт выйдет из передатчика
	while ((AT91C_BASE_SPI1->SPI_SR & AT91C_SPI_TXEMPTY) == 0)
		;

	HARDWARE_SPI_CONNECT_MOSI();	// Возвращаем в обычный режим работы

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	HARDWARE_SPI_DISCONNECT_MOSI();	// выход данных в "1" - для нормальной работы SD CARD

	SPI->SPI_TPR = (unsigned long) buffer;
	SPI->SPI_RPR = (unsigned long) buffer;
	SPI->SPI_RCR = size;	// разрешить работу приёмника
	SPI->SPI_TCR = size;	// запуск передатчика (выдача синхронизации)

	while ((SPI->SPI_SR & SPI_SR_ENDRX) == 0)	// было TX
		;
	// дождаться, пока последний байт выйдет из передатчика
	while ((SPI->SPI_SR & SPI_SR_TXEMPTY) == 0)
		;

	HARDWARE_SPI_CONNECT_MOSI();	// Возвращаем в обычный режим работы

#elif CPUSTYLE_AT91SAM7S

	HARDWARE_SPI_DISCONNECT_MOSI();	// выход данных в "1" - для нормальной работы SD CARD

	AT91C_BASE_SPI->SPI_TPR = (unsigned long) buffer;
	AT91C_BASE_SPI->SPI_RPR = (unsigned long) buffer;
	AT91C_BASE_SPI->SPI_RCR = size;	// разрешить работу приёмника
	AT91C_BASE_SPI->SPI_TCR = size;	// запуск передатчика (выдача синхронизации)

	while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_ENDRX) == 0)	// было TX
		;
	// дождаться, пока последний байт выйдет из передатчика
	while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0)
		;

	HARDWARE_SPI_CONNECT_MOSI();	// Возвращаем в обычный режим работы

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	
	/*
	if (((uint32_t) buffer & 0xFF000000) == CCMDATARAM_BASE)
	{
		debug_printf_P(PSTR("hardware_spi_master_read_frame: use CCM\n"));
		// Safe version
		prog_spi_read_frame(target, buffer, size);
		return;
	}
	*/

	HARDWARE_SPI_DISCONNECT_MOSI();	// выход данных в "1" - для нормальной работы SD CARD

	// DMA2: SPI1_RX: Stream 0: Channel 3
	#if CPUSTYLE_STM32H7XX
		SPI1->CFG1 |= SPI_CFG1_RXDMAEN; // DMA по приему (master)
	#else /* CPUSTYLE_STM32H7XX */
		SPI1->CR2 |= SPI_CR2_RXDMAEN; // DMA по приему (master)
	#endif /* CPUSTYLE_STM32H7XX */

	DMA2_Stream0->M0AR = (uintptr_t) buffer;
	DMA2_Stream0->NDTR = (DMA2_Stream0->NDTR & ~ DMA_SxNDT) |
		(size * DMA_SxNDT_0) |
		0;
	DMA2_Stream0->CR |= DMA_SxCR_EN;		// перезапуск DMA

	// DMA2: SPI1_TX: Stream 3: Channel 3
	#if CPUSTYLE_STM32H7XX
		SPI1->CFG1 |= SPI_CFG1_TXDMAEN; // DMA по передаче
	#else /* CPUSTYLE_STM32H7XX */
		SPI1->CR2 |= SPI_CR2_TXDMAEN; // DMA по передаче
	#endif /* CPUSTYLE_STM32H7XX */

	DMA2_Stream3->M0AR = (uintptr_t) buffer;
	DMA2_Stream3->NDTR = (DMA2_Stream3->NDTR & ~ DMA_SxNDT) |
		(size * DMA_SxNDT_0) |
		0;
	DMA2_Stream3->CR |= DMA_SxCR_EN;		// запуск DMA передатчика (выдача синхронизации)

	// Дожидаемся завершения обмена передающего канала DMA
	while ((DMA2->LISR & DMA_LISR_TCIF3) == 0)	// ожидаем завершения обмена по соответствушему stream
		;
	DMA2->LIFCR = DMA_LIFCR_CTCIF3;		// сбросил флаг соответствующий stream
	//DMA2_waitTC(3);	// ожидаем завершения обмена по соответствушему stream

	// Дожидаемся завершения обмена принимающего канала DMA
	while ((DMA2->LISR & DMA_LISR_TCIF0) == 0)	// ожидаем завершения обмена по соответствушему stream
		;
	DMA2->LIFCR = DMA_LIFCR_CTCIF0;		// сбросил флаг соответствующий stream
	//DMA2_waitTC(0);	// ожидаем завершения обмена по соответствушему stream

	#if CPUSTYLE_STM32H7XX

		SPI1->CFG1 &= ~ SPI_CFG1_TXDMAEN; // DMA по передаче (master)
		SPI1->CFG1 &= ~ SPI_CFG1_RXDMAEN; // DMA по приему (master)

	#else /* CPUSTYLE_STM32H7XX */

		SPI1->CR2 &= ~ SPI_CR2_TXDMAEN; // DMA по передаче (master)
		SPI1->CR2 &= ~ SPI_CR2_RXDMAEN; // DMA по приему (master)

	#endif /* CPUSTYLE_STM32H7XX */

	HARDWARE_SPI_CONNECT_MOSI();	// Возвращаем в обычный режим работы

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX
	#warning TODO: implement SPI over DMA

	//prog_spi_read_frame(target, buffer, size);

#elif CPUSTYLE_STM32F1XX
	#warning TODO: implement SPI over DMA

	//prog_spi_read_frame(target, buffer, size);

#elif CPUSTYLE_ATXMEGA
	#warning TODO: implement SPI over DMA
	
	//prog_spi_read_frame(target, buffer, size);

#elif CPUSTYLE_R7S721
	#warning TODO: Add code for R7S721 SPI DMA support to hardware_spi_master_read_frame_16bpartial

	HARDWARE_SPI_DISCONNECT_MOSI();	// выход данных в "1" - для нормальной работы SD CARD

	HARDWARE_SPI_CONNECT_MOSI();	// Возвращаем в обычный режим работы

#else
	#error Undefined CPUSTYLE_xxxx

#endif
}

#endif /* WITHSPI16BIT */

// Read a frame of bytes via SPI
// На сигнале MOSI при это должно обеспачиваться состояние логической "1" для корректной работы SD CARD
static void 
hardware_spi_master_read_frame_8bpartial(
	//spitarget_t target,	/* addressing to chip */
	uint8_t * buffer, 
	uint_fast32_t size		/* количество пересылаемых 8-ти битных элементов */
	)
{
#if CPUSTYLE_SAM9XE

	HARDWARE_SPI_DISCONNECT_MOSI();	// выход данных в "1" - для нормальной работы SD CARD

	AT91C_BASE_SPI1->SPI_TPR = (unsigned long) buffer;
	AT91C_BASE_SPI1->SPI_RPR = (unsigned long) buffer;
	AT91C_BASE_SPI1->SPI_RCR = size;	// разрешить работу приёмника
	AT91C_BASE_SPI1->SPI_TCR = size;	// запуск передатчика (выдача синхронизации)

	while ((AT91C_BASE_SPI1->SPI_SR & AT91C_SPI_ENDRX) == 0)	// было TX
		;
	// дождаться, пока последний байт выйдет из передатчика
	while ((AT91C_BASE_SPI1->SPI_SR & AT91C_SPI_TXEMPTY) == 0)
		;

	HARDWARE_SPI_CONNECT_MOSI();	// Возвращаем в обычный режим работы

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	HARDWARE_SPI_DISCONNECT_MOSI();	// выход данных в "1" - для нормальной работы SD CARD

	SPI->SPI_TPR = (unsigned long) buffer;
	SPI->SPI_RPR = (unsigned long) buffer;
	SPI->SPI_RCR = size;	// разрешить работу приёмника
	SPI->SPI_TCR = size;	// запуск передатчика (выдача синхронизации)

	while ((SPI->SPI_SR & SPI_SR_ENDRX) == 0)	// было TX
		;
	// дождаться, пока последний байт выйдет из передатчика
	while ((SPI->SPI_SR & SPI_SR_TXEMPTY) == 0)
		;

	HARDWARE_SPI_CONNECT_MOSI();	// Возвращаем в обычный режим работы

#elif CPUSTYLE_AT91SAM7S

	HARDWARE_SPI_DISCONNECT_MOSI();	// выход данных в "1" - для нормальной работы SD CARD

	AT91C_BASE_SPI->SPI_TPR = (unsigned long) buffer;
	AT91C_BASE_SPI->SPI_RPR = (unsigned long) buffer;
	AT91C_BASE_SPI->SPI_RCR = size;	// разрешить работу приёмника
	AT91C_BASE_SPI->SPI_TCR = size;	// запуск передатчика (выдача синхронизации)

	while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_ENDRX) == 0)	// было TX
		;
	// дождаться, пока последний байт выйдет из передатчика
	while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_TXEMPTY) == 0)
		;

	HARDWARE_SPI_CONNECT_MOSI();	// Возвращаем в обычный режим работы

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX
	
	/*
	if (((uint32_t) buffer & 0xFF000000) == CCMDATARAM_BASE)
	{
		debug_printf_P(PSTR("hardware_spi_master_read_frame: use CCM\n"));
		// Safe version
		prog_spi_read_frame(target, buffer, size);
		return;
	}
	*/

	HARDWARE_SPI_DISCONNECT_MOSI();	// выход данных в "1" - для нормальной работы SD CARD

	// DMA2: SPI1_RX: Stream 0: Channel 3
	#if CPUSTYLE_STM32H7XX
		SPI1->CFG1 |= SPI_CFG1_RXDMAEN; // DMA по приему (master)
	#else /* CPUSTYLE_STM32H7XX */
		SPI1->CR2 |= SPI_CR2_RXDMAEN; // DMA по приему (master)
	#endif /* CPUSTYLE_STM32H7XX */

	DMA2_Stream0->M0AR = (uintptr_t) buffer;
	DMA2_Stream0->NDTR = (DMA2_Stream0->NDTR & ~ DMA_SxNDT) |
		(size * DMA_SxNDT_0) |
		0;
	DMA2_Stream0->CR |= DMA_SxCR_EN;		// перезапуск DMA

	// DMA2: SPI1_TX: Stream 3: Channel 3
	#if CPUSTYLE_STM32H7XX
		SPI1->CFG1 |= SPI_CFG1_TXDMAEN; // DMA по передаче
	#else /* CPUSTYLE_STM32H7XX */
		SPI1->CR2 |= SPI_CR2_TXDMAEN; // DMA по передаче
	#endif /* CPUSTYLE_STM32H7XX */

	DMA2_Stream3->M0AR = (uintptr_t) buffer;
	DMA2_Stream3->NDTR = (DMA2_Stream3->NDTR & ~ DMA_SxNDT) |
		(size * DMA_SxNDT_0) |
		0;
	DMA2_Stream3->CR |= DMA_SxCR_EN;		// запуск DMA передатчика (выдача синхронизации)

	// Дожидаемся завершения обмена передающего канала DMA
	while ((DMA2->LISR & DMA_LISR_TCIF3) == 0)	// ожидаем завершения обмена по соответствушему stream
		;
	DMA2->LIFCR = DMA_LIFCR_CTCIF3;		// сбросил флаг соответствующий stream
	//DMA2_waitTC(3);	// ожидаем завершения обмена по соответствушему stream

	// Дожидаемся завершения обмена принимающего канала DMA
	while ((DMA2->LISR & DMA_LISR_TCIF0) == 0)	// ожидаем завершения обмена по соответствушему stream
		;
	DMA2->LIFCR = DMA_LIFCR_CTCIF0;		// сбросил флаг соответствующий stream
	//DMA2_waitTC(0);	// ожидаем завершения обмена по соответствушему stream

	#if CPUSTYLE_STM32H7XX

		SPI1->CFG1 &= ~ SPI_CFG1_TXDMAEN; // DMA по передаче (master)
		SPI1->CFG1 &= ~ SPI_CFG1_RXDMAEN; // DMA по приему (master)

	#else /* CPUSTYLE_STM32H7XX */

		SPI1->CR2 &= ~ SPI_CR2_TXDMAEN; // DMA по передаче (master)
		SPI1->CR2 &= ~ SPI_CR2_RXDMAEN; // DMA по приему (master)

	#endif /* CPUSTYLE_STM32H7XX */

	HARDWARE_SPI_CONNECT_MOSI();	// Возвращаем в обычный режим работы

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX
	#warning TODO: implement SPI over DMA

	//prog_spi_read_frame(target, buffer, size);

#elif CPUSTYLE_STM32F1XX
	#warning TODO: implement SPI over DMA

	//prog_spi_read_frame(target, buffer, size);

#elif CPUSTYLE_ATXMEGA
	#warning TODO: implement SPI over DMA
	
	//prog_spi_read_frame(target, buffer, size);

#elif CPUSTYLE_R7S721
	#warning TODO: Add code for R7S721 SPI DMA support to hardware_spi_master_read_frame_8bpartial

	HARDWARE_SPI_DISCONNECT_MOSI();	// выход данных в "1" - для нормальной работы SD CARD

	HARDWARE_SPI_CONNECT_MOSI();	// Возвращаем в обычный режим работы

#else
	#error Undefined CPUSTYLE_xxxx

#endif
}

#if WITHSPI16BIT

void hardware_spi_master_send_frame_16b(
	//spitarget_t target,	/* addressing to chip */
	const uint16_t * buffer, 
	uint_fast32_t size		/* количество пересылаемых 16-ти битных элементов */
	)
{
	hardware_spi_master_setdma16bit_tx();
#if CPUSTYLE_R7S721
	// в этом процессоре счетчик байтов 32-х разрядный
	uint_fast32_t score;
	for (score = 0; score < size; )
	{
		const uint_fast32_t chunk = ulmin(size - score, 0x7FFFFF00uL);
		hardware_spi_master_send_frame_16bpartial(buffer + score, chunk);
		score += chunk;
	}
#else
	uint_fast32_t score;
	for (score = 0; score < size; )
	{
		const uint_fast16_t chunk = ulmin(size - score, 0xFF00uL);
		hardware_spi_master_send_frame_16bpartial(buffer + score, chunk);
		score += chunk;
	}
#endif
}

void hardware_spi_master_read_frame_16b(
	//spitarget_t target,	/* addressing to chip */
	uint16_t * buffer, 
	uint_fast32_t size		/* количество пересылаемых 16-ти битных элементов */
	)
{
	hardware_spi_master_setdma16bit_rx();
#if CPUSTYLE_R7S721
	// в этом процессоре счетчик байтов 32-х разрядный
	uint_fast32_t score;
	for (score = 0; score < size; )
	{
		const uint_fast32_t chunk = ulmin(size - score, 0x7FFFFF00uL);
		hardware_spi_master_read_frame_16bpartial(buffer + score, chunk);
		score += chunk;
	}
#else
	uint_fast32_t score;
	for (score = 0; score < size; )
	{
		const uint_fast16_t chunk = ulmin(size - score, 0xFF00uL);
		hardware_spi_master_read_frame_16bpartial(buffer + score, chunk);
		score += chunk;
	}
#endif
}

#endif /* WITHSPI16BIT */

void hardware_spi_master_send_frame(
	//spitarget_t target,	/* addressing to chip */
	const uint8_t * buffer, 
	uint_fast32_t size		/* количество пересылаемых 8-ти битных элементов */
	)
{
	hardware_spi_master_setdma8bit_tx();
#if CPUSTYLE_R7S721
	hardware_spi_master_send_frame_8bpartial(buffer, size);
#else
	uint_fast32_t score;
	for (score = 0; score < size; )
	{
		const uint_fast16_t chunk = ulmin(size - score, 0xFF00uL);
		hardware_spi_master_send_frame_8bpartial(buffer + score, chunk);
		score += chunk;
	}
#endif
}


void hardware_spi_master_read_frame(
	//spitarget_t target,	/* addressing to chip */
	uint8_t * buffer, 
	uint_fast32_t size		/* количество пересылаемых 8-ти битных элементов */
	)
{
	hardware_spi_master_setdma8bit_rx();
#if CPUSTYLE_R7S721
	hardware_spi_master_read_frame_8bpartial(buffer, size);
#else
	uint_fast32_t score;
	for (score = 0; score < size; )
	{
		const uint_fast16_t chunk = ulmin(size - score, 0xFF00uL);
		hardware_spi_master_read_frame_8bpartial(buffer + score, chunk);
		score += chunk;
	}
#endif
}

#endif /* WITHSPIHWDMA */

#if WITHSPI16BIT

/* управление состоянием "подключено - работа в режиме 16-ти битных слов.*/
void hardware_spi_connect_b16(uint_fast8_t spispeedindex, uint_fast8_t spimode)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	// инициализация контроллера SPI
	enum { OUTMASK = PIO_PA13A_MOSI | PIO_PA14A_SPCK };		// битовая маска, определяет каким выводом шевелить
	enum { INPMASK = PIO_PA12A_MISO };		// битовая маска, определяет откуда ввод
	enum { WORKMASK = OUTMASK | INPMASK };		// битовая маска, включает и ввод и вывод

	SPI->SPI_CSR [0] = spi_csr_val16w [spispeedindex][spimode];

	(void) SPI->SPI_RDR;		/* clear AT91C_SPI_RDRF in status register */
	HARDWARE_SPI_CONNECT();

#elif CPUSTYLE_AT91SAM7S

	enum { OUTMASK = AT91C_PA13_MOSI | AT91C_PA14_SPCK };		// битовая маска, определяет каким выводом шевелить
	enum { INPMASK = AT91C_PA12_MISO };		// битовая маска, определяет откуда ввод
	enum { WORKMASK = OUTMASK | INPMASK };		// битовая маска, включает и ввод и вывод

	AT91C_BASE_SPI->SPI_CSR [0] = spi_csr_val16w [spispeedindex][spimode];

	(void) AT91C_BASE_SPI->SPI_RDR;		/* clear AT91C_SPI_RDRF in status register */
	HARDWARE_SPI_CONNECT();

#elif CPUSTYLE_STM32F1XX

	HARDWARE_SPI_CONNECT();
	SPI1->CR1 = spi_cr1_val16w [spispeedindex][spimode];
	#if WITHTWIHW
		// Silicon errata: 
		// 2.6.7 I2C1 with SPI1 remapped and used in master mode
		// Workaround:
		// When using SPI1 remapped, the I2C1 clock must be disabled.
		RCC->APB1ENR &= ~ (RCC_APB1ENR_I2C1EN); // выкл тактирование контроллера I2C
		__DSB();
	#endif

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32L0XX

	// В этих процессорах и входы и выходы перекдючаются на ALT FN
	HARDWARE_SPI_CONNECT();

	SPI1->CR1 = spi_cr1_val16w [spispeedindex][spimode];
	#if WITHTWIHW
		// Silicon errata: 
		// 2.6.7 I2C1 with SPI1 remapped and used in master mode
		// Workaround:
		// When using SPI1 remapped, the I2C1 clock must be disabled.
		RCC->APB1ENR &= ~ (RCC_APB1ENR_I2C1EN); // выкл тактирование контроллера I2C
		__DSB();
	#endif

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX

	// В этих процессорах и входы и выходы перекдючаются на ALT FN
	HARDWARE_SPI_CONNECT();

	SPI1->CR1 = spi_cr1_val8w [spispeedindex][spimode];
	SPI1->CR2 = (SPI1->CR2 & ~ (SPI_CR2_DS)) |
		15 * SPI_CR2_DS_0 |	// 16 bit word length
		0 * SPI_CR2_FRXTH |			// RXFIFO threshold is set to 16 bits (FRXTH=0).
		0;
	#if WITHTWIHW
		// Silicon errata: 
		// 2.6.7 I2C1 with SPI1 remapped and used in master mode
		// Workaround:
		// When using SPI1 remapped, the I2C1 clock must be disabled.
		RCC->APB1ENR &= ~ (RCC_APB1ENR_I2C1EN); // выкл тактирование контроллера I2C
		__DSB();
	#endif

#elif CPUSTYLE_STM32H7XX

	HARDWARE_SPI_CONNECT();

	SPI1->CR1 = SPI_CR1_SSI;

	SPI1->CFG1 = spi_cfg1_val16w;
	SPI1->CFG2 = spi_cfg2_val [spispeedindex][spimode];

	SPI1->CR1 |= SPI_CR1_SPE;
	SPI1->CR1 |= SPI_CR1_CSTART;

#elif CPUSTYLE_R7S721

	RSPI0.SPDCR =		/* Data Control Register (SPDCR) */
		(0x02 << 5) |	// 0x02: 16 bit. Specifies the width for accessing the data register (SPDR)
		0;
	RSPI0.SPBR = spi_spbr_val [spispeedindex];
	RSPI0.SPCMD0 = spi_spcmd0_val16w [spispeedindex][spimode];

	HARDWARE_SPI_CONNECT();

#else
	#error Wrong CPUSTYLE macro
#endif

}

portholder_t hardware_spi_complete_b16(void)	/* дождаться готовности */
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	/* дождаться завершения приёма/передачи */
	while ((SPI->SPI_SR & SPI_SR_RDRF) == 0)
		;
	return (SPI->SPI_RDR & SPI_TDR_TD_Msk);

#elif CPUSTYLE_AT91SAM7S

	/* дождаться завершения приёма/передачи */
	while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_RDRF) == 0)
		;
	return (AT91C_BASE_SPI->SPI_RDR & AT91C_SPI_TD);

#elif CPUSTYLE_STM32H7XX

	//while ((SPI1->SR & SPI_SR_TXC) == 0)	
	//	;
	while ((SPI1->SR & SPI_SR_RXP) == 0)	
		;
	const portholder_t t = * (volatile uint16_t *) & SPI1->RXDR;	/* SPI_RXDR_RXDR clear SPI_SR_RXNE in status register */
	return t;

#elif CPUSTYLE_STM32F

	while ((SPI1->SR & SPI_SR_RXNE) == 0)	
		;
	const portholder_t t = SPI1->DR & SPI_DR_DR;	/* clear SPI_SR_RXNE in status register */
	while ((SPI1->SR & SPI_SR_BSY) != 0)	
		;
	return t;

#elif CPUSTYLE_R7S721

	while ((RSPI0.SPSR & (1U << 7)) == 0)	// SPRF bit
		;
	return RSPI0.SPDR.UINT16 [R_IO_L]; // L=0

#else
	#error Wrong CPUSTYLE macro
#endif
}

static void hardware_spi_ready_b16_void(void)	/* дождаться готовности */
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	/* дождаться завершения приёма/передачи */
	while ((SPI->SPI_SR & SPI_SR_RDRF) == 0)
		;
	(void) SPI->SPI_RDR;

#elif CPUSTYLE_AT91SAM7S

	/* дождаться завершения приёма/передачи */
	while ((AT91C_BASE_SPI->SPI_SR & AT91C_SPI_RDRF) == 0)
		;
	(void) AT91C_BASE_SPI->SPI_RDR;

#elif CPUSTYLE_STM32H7XX

	//while ((SPI1->SR & SPI_SR_TXC) == 0)	
	//	;
	while ((SPI1->SR & SPI_SR_RXP) == 0)	
		;
	(void) * (volatile uint16_t *) & SPI1->RXDR;	/* clear SPI_SR_RXNE in status register */


#elif CPUSTYLE_STM32F

	while ((SPI1->SR & SPI_SR_RXNE) == 0)	
		;
	(void) SPI1->DR;	/* clear SPI_SR_RXNE in status register */

#elif CPUSTYLE_R7S721

	while ((RSPI0.SPSR & (1U << 7)) == 0)	// SPRF bit
		;
	(void) RSPI0.SPDR.UINT16 [R_IO_L];	 // L=0

#else
	#error Wrong CPUSTYLE macro
#endif
}


/* группа функций для использования в групповых передачах по SPI */
/* передача первого байта в последовательности - Не проверяем готовность перед передачей, 
   завершение передачи будут проверять другие.
*/
void hardware_spi_b16_p1(
	portholder_t v		/* значениеслова для передачи */
	)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	SPI->SPI_TDR = SPI_TDR_TD(v);

#elif CPUSTYLE_AT91SAM7S

	AT91C_BASE_SPI->SPI_TDR = v & AT91C_SPI_TD;

#elif CPUSTYLE_STM32H7XX
	
	* (volatile uint16_t *) & (SPI1)->TXDR = v;	// prevent data packing feature

#elif CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F30X || CPUSTYLE_STM32F7XX

	* (volatile uint16_t *) & (SPI1)->DR = v;	// prevent data packing feature

#elif CPUSTYLE_STM32F

	(SPI1)->DR = v;

#elif CPUSTYLE_R7S721

	RSPI0.SPDR.UINT16 [R_IO_L] = v; // L=0

#else
	#error Wrong CPUSTYLE macro
#endif
}

/* передача одного из средних байтов/слов в последовательности */
/* дождаться готовности, передача байта */
void hardware_spi_b16_p2(
	portholder_t v		/* значение байта для передачи */
	)
{
	hardware_spi_ready_b16_void();	/* дождаться завершения передачи */
	hardware_spi_b16_p1(v);	/* передать символ */
}

/* передача байта/слова, возврат считанного */
portholder_t hardware_spi_b16(
	portholder_t v		/* значение байта для передачи */
	)
{
	hardware_spi_b16_p1(v);	/* передать символ */
	return hardware_spi_complete_b16();	/* дождаться завершения передачи */
}

#endif /* WITHSPI16BIT */

void hardware_spi_b8_p1(
	portholder_t v		/* значение байта/слова для передачи */
	)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	SPI->SPI_TDR = SPI_TDR_TD(v);

#elif CPUSTYLE_AT91SAM7S

	AT91C_BASE_SPI->SPI_TDR = v & AT91C_SPI_TD;

#elif CPUSTYLE_ATMEGA

	SPDR = v; // запуск передачи

#elif CPUSTYLE_ATXMEGA

	TARGETHARD_SPI.DATA = v; // запуск передачи

#elif CPUSTYLE_STM32H7XX

	* (volatile uint8_t *) & (SPI1)->TXDR = v;	// prevent data packing feature

#elif CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F30X || CPUSTYLE_STM32F7XX

	* (volatile uint8_t *) & (SPI1)->DR = v;	// prevent data packing feature

#elif CPUSTYLE_STM32F

	SPI1->DR = v;

#elif CPUSTYLE_R7S721

	RSPI0.SPDR.UINT8 [R_IO_LL] = v; // LL=0

#else
	#error Wrong CPUSTYLE macro
#endif
}

/* передача одного из средних байтов/слов в последовательности */
/* дождаться готовности, передача байта */
void hardware_spi_b8_p2(
	portholder_t v		/* значение байта для передачи */
	)
{
	hardware_spi_ready_b8_void();	/* дождаться завершения передачи (на atmega оптимизированно по скорости - без чиения регистра данных). */
	hardware_spi_b8_p1(v);	/* передать символ */
}
/* передача байта/слова, возврат считанного */
portholder_t hardware_spi_b8(
	portholder_t v		/* значение байта для передачи */
	)
{
	hardware_spi_b8_p1(v);	/* передать символ */
	return hardware_spi_complete_b8();	/* дождаться завершения передачи */
}

#else  /* WITHSPIHW */

// При отсутствующем аппаратном контроллере ничего не делает.

void hardware_spi_master_setfreq(uint_fast8_t spispeedindex, int_fast32_t spispeed)
{
	(void) spispeedindex;
	(void) spispeed;
}

#endif /* WITHSPIHW */

#if WITHELKEY

void 
hardware_elkey_timer_initialize(void)
{
#if CPUSTYLE_ATMEGA

	// Timer/Counter 1 used for electronic key synchronisation with 1/20 of dot length
	//// TCCR1B = (1U << WGM12) | (1U << CS12) | (1U << CS10);		// CTC mode (mode4) and 1/1024 prescaler
	//// OCR1A = 0xffff;

	#if CPUSTYLE_ATMEGA_XXX4
		// Timer/Counter 1 Interrupt(s) initialization
		TIMSK1 |= (1U << OCIE1A);	// Timer/Counter 1 interrupt enable
	#elif CPUSTYLE_ATMEGA328
		// Timer/Counter 1 Interrupt(s) initialization
		TIMSK1 |= (1U << OCIE1A);	// Timer/Counter 1 interrupt enable
	#else /* CPUSTYLE_ATMEGA_XXX4 */
		TIMSK |= (1U << OCIE1A);	// Timer/Counter 1 interrupt enable
		//TIMSK |= 0x10;	// Timer/Counter 1 interrupt enable
	#endif /* CPUSTYLE_ATMEGA_XXX4 */

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S
	// TC2 used for electronic key synchronisation with 1/20 of dot length
	// TC2 used for generate 1/20 of morse dot length intervals
	//
	PMC->PMC_PCER0 = (1UL << ID_TC2);	// разрешить тактированние этого блока (ID_TC0..ID_TC5 avaliable)

	TC0->TC_CHANNEL [2].TC_CMR = 
	                (TC_CMR_CLKI * 0) |
	                TC_CMR_BURST_NONE |
	                TC_CMR_WAVSEL_UP_RC | /* TC_CMR_WAVESEL_UP_AUTO */
	                TC_CMR_WAVE |
					TC_CMR_TCCLKS_TIMER_CLOCK5 |	// TC0CLK = MCLK/32
					0
				;

	TC0->TC_CHANNEL [2].TC_CCR = TC_CCR_SWTRG | TC_CCR_CLKEN; // reset and enable TC2 clock

	TC0->TC_CHANNEL [2].TC_IER = TC_IER_CPCS ; // Interrupt on RC compare

	// enable interrupts from TC2
	NVIC_SetPriority(TC2_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(TC2_IRQn);		// enable TC2_Handler();

#elif CPUSTYLE_AT91SAM7S
	// TC2 used for electronic key synchronisation with 1/20 of dot length
	// TC2 used for generate 1/20 of morse dot length intervals
	//
	AT91C_BASE_PMC->PMC_PCER = (1UL << AT91C_ID_TC2); // разрешить тактированние этого блока (AT91C_ID_TC0..AT91C_ID_TC2 avaliable)

	AT91C_BASE_TCB->TCB_TC2.TC_CMR = 
	                (AT91C_TC_CLKI * 0) |
	                AT91C_TC_BURST_NONE |
	                AT91C_TC_WAVESEL_UP_AUTO |
	                AT91C_TC_WAVE |
					AT91C_TC_CLKS_TIMER_DIV5_CLOCK |
					0
					;

	AT91C_BASE_TCB->TCB_TC2.TC_CCR = AT91C_TC_SWTRG | AT91C_TC_CLKEN; // reset and enable TC2 clock

	AT91C_BASE_TCB->TCB_TC2.TC_IER = AT91C_TC_CPCS ; // Interrupt on RC compare

	// enable interrupts from TC2
	{
		enum { irqID = AT91C_ID_TC2 };

		AT91C_BASE_AIC->AIC_IDCR = (1UL << irqID);		// disable interrupt
		AT91C_BASE_AIC->AIC_SVR [irqID] = (AT91_REG) AT91F_TC2_IRQHandler;
		AT91C_BASE_AIC->AIC_SMR [irqID] = 
			(AT91C_AIC_SRCTYPE & AT91C_AIC_SRCTYPE_HIGH_LEVEL) |
			(AT91C_AIC_PRIOR & AT91C_AIC_PRIOR_HIGHEST);
		AT91C_BASE_AIC->AIC_ICCR = (1UL << irqID);		// clear pending interrupt
		AT91C_BASE_AIC->AIC_IECR = (1UL << irqID);	// enable interrupt
	}

#elif CPUSTYLE_ATXMEGA

	TCC1.INTCTRLB = 0;		// останавливаем таймер - будет запущен в функции установке частоты

#elif CPUSTYLE_STM32H7XX

	RCC->APB1LENR |= RCC_APB1LENR_TIM3EN;   // подаем тактирование на TIM3
	__DSB();
	TIM3->DIER = TIM_DIER_UIE;        	 // разрешить событие от таймера

	NVIC_SetPriority(TIM3_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(TIM3_IRQn);		// enable TIM3_IRQHandler();

#elif CPUSTYLE_STM32F

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;   // подаем тактирование на TIM3
	__DSB();
	TIM3->DIER = TIM_DIER_UIE;        	 // разрешить событие от таймера

	NVIC_SetPriority(TIM3_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(TIM3_IRQn);		// enable TIM3_IRQHandler();

#elif CPUSTYLE_R7S721

	// OSTM1
    /* ==== Module standby clear ==== */
    /* ---- Supply clock to the OSTM(channel 1) ---- */
	CPG.STBCR5 &= ~ CPG_STBCR5_MSTP50;	// Module Stop 50 0: Channel 1 of the OS timer runs.
	(void) CPG.STBCR5;			/* Dummy read */

    /* ---- OSTM count stop trigger register (TT) setting ---- */
    OSTM1.OSTMnTT = 0x01u;      /* Stop counting */

	{
		const uint16_t int_id = OSTMI1TINT_IRQn;
		r7s721_intc_registintfunc(int_id, r7s721_ostm1_interrupt);	/* ==== Register OS timer interrupt handler ==== */
		GIC_SetPriority(int_id, ARM_SYSTEM_PRIORITY);		/* ==== Set priority of OS timer interrupt to 5 ==== */
		GIC_EnableIRQ(int_id);		/* ==== Validate OS timer interrupt ==== */
	}

	OSTM1.OSTMnTS = 0x01u;      /* Start counting */

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

void hardware_elkey_set_speed(uint_fast32_t ticksfreq)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(ticksfreq), ATSAM3S_TIMER_WIDTH, ATSAM3S_TIMER_TAPS, & value, 1);
	TC0->TC_CHANNEL [2].TC_CMR =
		(TC0->TC_CHANNEL [2].TC_CMR & ~ TC_CMR_TCCLKS_Msk) | tc_cmr_tcclks [prei];
	TC0->TC_CHANNEL [2].TC_RC = value;	// программирование полного периода

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(ticksfreq), AT91SAM7_TIMER_WIDTH, AT91SAM7_TIMER_TAPS, & value, 1);
	AT91C_BASE_TCB->TCB_TC2.TC_CMR =
		(AT91C_BASE_TCB->TCB_TC2.TC_CMR & ~ AT91C_TC_CLKS) | tc_cmr_clks [prei];
	AT91C_BASE_TCB->TCB_TC2.TC_RC = value;	// программирование полного периода

#elif CPUSTYLE_ATMEGA

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(ticksfreq), ATMEGA_TIMER1_WIDTH, ATMEGA_TIMER1_TAPS, & value, 1);
	// WGM12 = WGMn2 bit in timer 1
	// (1U << WGM12) - mode4: CTC
	TCCR1B = (1U << WGM12) | (prei + 1);	// прескалер
	OCR1A = value;	// делитель - программирование полного периода

#elif CPUSTYLE_ATXMEGA

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(ticksfreq), ATXMEGA_TIMER_WIDTH, ATXMEGA_TIMER_TAPS, & value, 1);

	// программирование таймера
	TCC1.CCA = value;	// timer/counter C1, compare register A, see TCC1_CCA_vect
	TCC1.CTRLA = (prei + 1);
	TCC1.CTRLB = (TC_WGMODE_FRQ_gc);
	TCC1.INTCTRLB = (TC_CCAINTLVL_MED_gc);
	// разрешение прерываний на входе в PMIC
	PMIC.CTRL |= (PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm);
	
#elif CPUSTYLE_STM32H7XX
	// TIM2 & TIM5 on CPUSTYLE_STM32F4XX have 32-bit CNT and ARR registers
	// TIM7 located on APB1
	// TIM7 on APB1
	// Use basic timer
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_pclk1_timers(ticksfreq), STM32F_TIM3_TIMER_WIDTH, STM32F_TIM3_TIMER_TAPS, & value, 1);

	TIM3->PSC = ((1UL << prei) - 1) & TIM_PSC_PSC;
	TIM3->ARR = value;
	TIM3->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости - если счётчик успевал превысить значение ARR - считал до конца */

#elif CPUSTYLE_STM32F
	// TIM2 & TIM5 on CPUSTYLE_STM32F4XX have 32-bit CNT and ARR registers
	// TIM7 located on APB1
	// TIM7 on APB1
	// Use basic timer
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_pclk2(ticksfreq), STM32F_TIM3_TIMER_WIDTH, STM32F_TIM3_TIMER_TAPS, & value, 1);

	TIM3->PSC = ((1UL << prei) - 1) & TIM_PSC_PSC;
	TIM3->ARR = value;
	TIM3->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости - если счётчик успевал превысить значение ARR - считал до конца */

#elif CPUSTYLE_R7S721

	// OSTM1
	OSTM1.OSTMnCMP = calcdivround_p0clock(ticksfreq) - 1;

	OSTM1.OSTMnCTL = (OSTM1.OSTMnCTL & ~ 0x03) |
		0 * (1U << 1) |	// Interval Timer Mode
		1 * (1U << 0) |	// Enables the interrupts when counting starts.
		0;

#else
	#error Undefined CPUSTYLE_XXX
#endif
}


void hardware_elkey_set_speed128(uint_fast32_t ticksfreq, int scale)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(scale * calcdivround(ticksfreq), ATSAM3S_TIMER_WIDTH, ATSAM3S_TIMER_TAPS, & value, 1);
	TC0->TC_CHANNEL [2].TC_CMR =
		(TC0->TC_CHANNEL [2].TC_CMR & ~ TC_CMR_TCCLKS_Msk) | tc_cmr_tcclks [prei];
	TC0->TC_CHANNEL [2].TC_RC = value;	// программирование полного периода

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(scale * calcdivround(ticksfreq), AT91SAM7_TIMER_WIDTH, AT91SAM7_TIMER_TAPS, & value, 1);
	AT91C_BASE_TCB->TCB_TC2.TC_CMR =
		(AT91C_BASE_TCB->TCB_TC2.TC_CMR & ~ AT91C_TC_CLKS) | tc_cmr_clks [prei];
	AT91C_BASE_TCB->TCB_TC2.TC_RC = value;	// программирование полного периода

#elif CPUSTYLE_ATMEGA

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(scale * calcdivround(ticksfreq), ATMEGA_TIMER1_WIDTH, ATMEGA_TIMER1_TAPS, & value, 1);
	// WGM12 = WGMn2 bit in timer 1
	// (1U << WGM12) - mode4: CTC
	TCCR1B = (1U << WGM12) | (prei + 1);	// прескалер
	OCR1A = value;	// делитель - программирование полного периода

#elif CPUSTYLE_ATXMEGA

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(scale * calcdivround(ticksfreq), ATXMEGA_TIMER_WIDTH, ATXMEGA_TIMER_TAPS, & value, 1);

	// программирование таймера
	TCC1.CCA = value;	// timer/counter C1, compare register A, see TCC1_CCA_vect
	TCC1.CTRLA = (prei + 1);
	TCC1.CTRLB = (TC_WGMODE_FRQ_gc);
	TCC1.INTCTRLB = (TC_CCAINTLVL_MED_gc);
	// разрешение прерываний на входе в PMIC
	PMIC.CTRL |= (PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm);
	
#elif CPUSTYLE_STM32H7XX
	// TIM2 & TIM5 on CPUSTYLE_STM32F4XX have 32-bit CNT and ARR registers
	// TIM7 located on APB1
	// TIM7 on APB1
	// Use basic timer
	unsigned value;
	const uint_fast8_t prei = calcdivider(scale * calcdivround_pclk1_timers(ticksfreq), STM32F_TIM3_TIMER_WIDTH, STM32F_TIM3_TIMER_TAPS, & value, 1);

	TIM3->PSC = ((1UL << prei) - 1) & TIM_PSC_PSC;
	TIM3->ARR = value;
	TIM3->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости - если счётчик успевал превысить значение ARR - считал до конца */

#elif CPUSTYLE_STM32F
	// TIM2 & TIM5 on CPUSTYLE_STM32F4XX have 32-bit CNT and ARR registers
	// TIM7 located on APB1
	// TIM7 on APB1
	// Use basic timer
	unsigned value;
	const uint_fast8_t prei = calcdivider(scale * calcdivround_pclk2(ticksfreq), STM32F_TIM3_TIMER_WIDTH, STM32F_TIM3_TIMER_TAPS, & value, 1);

	TIM3->PSC = ((1UL << prei) - 1) & TIM_PSC_PSC;
	TIM3->ARR = value;
	TIM3->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости - если счётчик успевал превысить значение ARR - считал до конца */

#elif CPUSTYLE_R7S721

	// OSTM1
	OSTM1.OSTMnCMP = scale * calcdivround_p0clock(ticksfreq) - 1;

	OSTM1.OSTMnCTL = (OSTM1.OSTMnCTL & ~ 0x03) |
		0 * (1U << 1) |	// Interval Timer Mode
		1 * (1U << 0) |	// Enables the interrupts when counting starts.
		0;

#else
	#error Undefined CPUSTYLE_XXX
#endif
}


#endif /* WITHELKEY */


uint32_t hardware_get_random(void)
{
#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	#if defined(STM32F446xx)
		#warning RNG not exist
		return 0;
	#elif defined(STM32F401xC)
		#warning RNG not exist
		return 0;
	#else
		if ((RCC->AHB2ENR & RCC_AHB2ENR_RNGEN) == 0)
		{
			RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;	/* RNG clock enable */
			__DSB();
			RNG->CR |= RNG_CR_RNGEN;
		}

		while ((RNG->SR & RNG_SR_DRDY) == 0)
			;
		return RNG->DR;
	#endif

#elif CPUSTYLE_STM32L0XX && defined (RCC_AHBENR_RNGEN)

	if ((RCC->AHBENR & RCC_AHBENR_RNGEN) == 0)
	{
		RCC->AHBENR |= RCC_AHBENR_RNGEN;	/* RNG clock enable */
		__DSB();
		RNG->CR |= RNG_CR_RNGEN;
	}

	while ((RNG->SR & RNG_SR_DRDY) == 0)
		;
	return RNG->DR;

#else

	#warning RNG not exist - hardware_get_random not work
	return 0;

#endif


}

void hardware_lfm_timer_initialize(void)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S
	// TC2 used for electronic key synchronisation with 1/20 of dot length
	// TC2 used for generate 1/20 of morse dot length intervals
	// TC0 used as LFM periodic timer
	//
	PMC->PMC_PCER0 = (1UL << ID_TC0);	// разрешить тактированние этого блока (ID_TC0..ID_TC5 avaliable)

	TC0->TC_CHANNEL [0].TC_CMR = 
	                (TC_CMR_CLKI * 0)
	                | TC_CMR_BURST_NONE
	                | TC_CMR_WAVSEL_UP_RC /* TC_CMR_WAVESEL_UP_AUTO */
	                | TC_CMR_WAVE
					| TC_CMR_TCCLKS_TIMER_CLOCK5	// TC0CLK = MCLK/32
					;

	TC0->TC_CHANNEL [0].TC_CCR = TC_CCR_SWTRG | TC_CCR_CLKEN; // reset and enable TC0 clock

	TC0->TC_CHANNEL [0].TC_IER = TC_IER_CPCS ; // Interrupt on RC compare

	// enable interrupts from TC2
	NVIC_SetPriority(TC0_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(TC0_IRQn);		// enable TC0_IrqHandler();

#elif CPUSTYLE_AT91SAM7S
	// TC2 used for electronic key synchronisation with 1/20 of dot length
	// TC2 used for generate 1/20 of morse dot length intervals
	//
	AT91C_BASE_PMC->PMC_PCER = (1UL << AT91C_ID_TC0); // разрешить тактированние этого блока (AT91C_ID_TC0..AT91C_ID_TC2 avaliable)

	AT91C_BASE_TCB->TCB_TC0.TC_CMR = 
	                (AT91C_TC_CLKI * 0)
	                | AT91C_TC_BURST_NONE
	                | AT91C_TC_WAVESEL_UP_AUTO
	                | AT91C_TC_WAVE
					| AT91C_TC_CLKS_TIMER_DIV5_CLOCK
					;

	AT91C_BASE_TCB->TCB_TC0.TC_CCR = AT91C_TC_SWTRG | AT91C_TC_CLKEN; // reset and enable TC2 clock

	AT91C_BASE_TCB->TCB_TC0.TC_IER = AT91C_TC_CPCS ; // Interrupt on RC compare

	// enable interrupts from TC0
	{
		enum { irqID = AT91C_ID_TC0 };

		AT91C_BASE_AIC->AIC_IDCR = (1UL << irqID);		// disable interrupt
		AT91C_BASE_AIC->AIC_SVR [irqID] = (AT91_REG) AT91F_TC0_IRQHandler;
		AT91C_BASE_AIC->AIC_SMR [irqID] = 
			(AT91C_AIC_SRCTYPE & AT91C_AIC_SRCTYPE_HIGH_LEVEL) |
			(AT91C_AIC_PRIOR & AT91C_AIC_PRIOR_HIGHEST);
		AT91C_BASE_AIC->AIC_ICCR = (1UL << irqID);		// clear pending interrupt
		AT91C_BASE_AIC->AIC_IECR = (1UL << irqID);	// enable inerrupt
	}

#endif
}

void hardware_lfm_setupdatefreq(unsigned ticksfreq)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(ticksfreq), ATSAM3S_TIMER_WIDTH, ATSAM3S_TIMER_TAPS, & value, 1);
	TC0->TC_CHANNEL [0].TC_CMR =
		(TC0->TC_CHANNEL [0].TC_CMR & ~ TC_CMR_TCCLKS_Msk) | tc_cmr_tcclks [prei];
	TC0->TC_CHANNEL [0].TC_RC = value;	// программирование полного периода

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround(ticksfreq), AT91SAM7_TIMER_WIDTH, AT91SAM7_TIMER_TAPS, & value, 1);
	AT91C_BASE_TCB->TCB_TC0.TC_CMR =
		(AT91C_BASE_TCB->TCB_TC0.TC_CMR & ~ AT91C_TC_CLKS) | tc_cmr_clks [prei];
	AT91C_BASE_TCB->TCB_TC0.TC_RC = value;	// программирование полного периода

#endif
}

#if WITHSDHCHW

void hardware_sdhost_setbuswidth(uint_fast8_t use4bit)
{
	debug_printf_P(PSTR("hardware_sdhost_setbuswidth: use4bit=%u\n"), (unsigned) use4bit);

#if CPUSTYLE_R7S721

	if (use4bit != 0)
		SDHI0.SD_OPTION &= ~ (1U << 15);	// WIDTH 0: 4-bit width 
	else
		SDHI0.SD_OPTION |= (1U << 15);		// WIDTH 1: 1-bit width 

#elif CPUSTYLE_STM32F4XX 

	SDIO->CLKCR = (SDIO->CLKCR & ~ (SDIO_CLKCR_WIDBUS)) |
		(use4bit != 0 ? 0x01 : 0x00) * SDIO_CLKCR_WIDBUS_0 |	// 01: 4-wide bus mode: SDMMC_D[3:0] used
		0;

#elif CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	SDMMC1->CLKCR = (SDMMC1->CLKCR & ~ (SDMMC_CLKCR_WIDBUS)) |
		(use4bit != 0 ? 0x01 : 0x00) * SDMMC_CLKCR_WIDBUS_0 |	// 01: 4-wide bus mode: SDMMC_D[3:0] used
		0;

#else
	#error Wrong CPUSTYLE_xxx
#endif
}

void hardware_sdhost_setspeed(unsigned long ticksfreq)
{
#if CPUSTYLE_R7S721
	// Использование автоматического расчёта предделителя
	//unsigned long ticksfreq = 400000uL;	// 400 kHz -> 260 kHz
	//unsigned long ticksfreq = 24000000uL;	// 24 MHz -> 16.666 MHz
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_p1clock(ticksfreq), 0, (512 | 256 | 128 | 64 | 32 | 16 | 8 | 4 | 2), & value, 0);
	debug_printf_P(PSTR("hardware_sdhost_setspeed: ticksfreq=%lu, prei=%lu\n"), (unsigned long) ticksfreq, (unsigned long) prei);

	while ((SDHI0.SD_INFO2 & (1uL << 13)) == 0)	// SCLKDIVEN
	{
		//debug_printf_P(PSTR("hardware_sdhost_setspeed: SCLKDIVEN set clock prohibited, SD_INFO2=%08lX\n"), SDHI0.SD_INFO2);
		TP();
	}
	while ((SDHI0.SD_INFO2 & (1uL << 14)) != 0)	// CBSY
	{
		//debug_printf_P(PSTR("hardware_sdhost_setspeed: CBSY set clock prohibited, SD_INFO2=%08lX\n"), SDHI0.SD_INFO2);
		TP();
	}

	SDHI0.SD_CLK_CTRL = 
		0 * (1U << 9) |		// SDCLKOFFEN=0 
		1 * (1U << 8) |		// SCLKEN=1 
		((1U << prei) >> 1) * (1U << 0) |
		0;

#elif CPUSTYLE_STM32F4XX 

	#if defined (RCC_DCKCFGR2_SDIOSEL)
		// stm32f446xx и некоторые другие

		RCC->DCKCFGR2 = (RCC->DCKCFGR2 & ~ (RCC_DCKCFGR2_SDIOSEL)) |
			0 * RCC_DCKCFGR2_SDIOSEL |	// 0: 48 MHz clock is selected as SDMMC clock
			0;

	#elif defined (RCC_DCKCFGR_SDIOSEL)
		// stm32f469xx, stm32f479xx

		RCC->DCKCFGR = (RCC->DCKCFGR & ~ (RCC_DCKCFGR_SDIOSEL)) |
			0 * RCC_DCKCFGR_SDIOSEL |	// 0: 48 MHz clock is selected as SDMMC clock
			0;
	#else
		// Остальные
	#endif

	const uint_fast32_t stm32f4xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
	// Использование автоматического расчёта делителя
	// PLLQ: Main PLL (PLL) division factor for USB OTG FS, SDIO and random number generator clocks
	// Should be 48 MHz or less for SDIO and 48 MHz with small tolerance.
	// See RCC_PLLCFGR_PLLQ usage
	const uint32_t SDIOCLK = PLL_FREQ / stm32f4xx_pllq;

	const unsigned value = ulmin(calcdivround2(SDIOCLK, ticksfreq) - 2, 255);

	SDIO->CLKCR = (SDIO->CLKCR & ~ (SDIO_CLKCR_CLKDIV_Msk)) |
		(value & SDIO_CLKCR_CLKDIV_Msk);

#elif CPUSTYLE_STM32F7XX

	RCC->DCKCFGR2 = (RCC->DCKCFGR2 & ~ (RCC_DCKCFGR2_SDMMC1SEL)) |
		0 * RCC_DCKCFGR2_SDMMC1SEL |	// 0: 48 MHz clock is selected as SDMMC clock
		0;

	const uint_fast32_t stm32f7xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
	// Использование автоматического расчёта делителя
	// PLLQ: Main PLL (PLL) division factor for USB OTG FS, SDIO and random number generator clocks
	// Should be 48 MHz or less for SDIO and 48 MHz with small tolerance.
	// See RCC_PLLCFGR_PLLQ usage
	const uint32_t SDMMCCLK = PLL_FREQ / stm32f7xx_pllq;
	// Использование автоматического расчёта делителя
	// Источником тактирования SDMMC сейчас установлен внутренний генератор 48 МГц
	//const uint32_t stm32f4xx_48mhz = PLL_FREQ / stm32f7xx_pllq;
	const unsigned value = ulmin(calcdivround2(SDMMCCLK, ticksfreq) - 2, 255);

	//debug_printf_P(PSTR("hardware_sdhost_setspeed: stm32f7xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f7xx_pllq, stm32f4xx_48mhz);
	debug_printf_P(PSTR("hardware_sdhost_setspeed: CLKCR_CLKDIV=%lu\n"), (unsigned long) value);

	SDMMC1->CLKCR = (SDMMC1->CLKCR & ~ (SDMMC_CLKCR_CLKDIV)) |
		(value & SDMMC_CLKCR_CLKDIV);

#elif CPUSTYLE_STM32H7XX

	//RCC->DCKCFGR2 = (RCC->DCKCFGR2 & ~ (RCC_DCKCFGR2_SDMMC1SEL)) |
	//	0 * RCC_DCKCFGR2_SDMMC1SEL |	// 0: 48 MHz clock is selected as SDMMC clock
	//	0;

	const uint_fast32_t stm32h7xx_pllq = arm_hardware_stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
	// Использование автоматического расчёта делителя
	// PLLQ: Main PLL (PLL) division factor for USB OTG FS, SDIO and random number generator clocks
	// Should be 48 MHz or less for SDIO and 48 MHz with small tolerance.
	// See RCC_PLLCFGR_PLLQ usage
	const uint32_t SDMMCCLK = PLL_FREQ / stm32h7xx_pllq;
	// Использование автоматического расчёта делителя
	// Источником тактирования SDMMC сейчас установлен внутренний генератор 48 МГц
	//const uint32_t stm32f4xx_48mhz = PLL_FREQ / stm32h7xx_pllq;
	const unsigned value = ulmin(calcdivround2(SDMMCCLK / 2, ticksfreq), 0x03FF);

	debug_printf_P(PSTR("hardware_sdhost_setspeed: stm32h7xx_pllq=%lu, SDMMCCLK=%lu, PLL_FREQ=%lu\n"), (unsigned long) stm32h7xx_pllq, SDMMCCLK, PLL_FREQ);
	debug_printf_P(PSTR("hardware_sdhost_setspeed: CLKCR_CLKDIV=%lu\n"), (unsigned long) value);

	SDMMC1->CLKCR = (SDMMC1->CLKCR & ~ (SDMMC_CLKCR_CLKDIV)) |
		(value & SDMMC_CLKCR_CLKDIV);


#else
	#error Wrong CPUSTYLE_xxx
#endif
}

#if CPUSTYLE_R7S721

void r7s721_sdhi0_dma_handler(void)
{
	debug_printf_P(PSTR("r7s721_sdhi0_dma_handler trapped\n"));
	for (;;)
		;
}

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX
// TODO: убрать в sdcard.c
/*
void DMA2_Stream6_IRQHandler(void)
{

}
*/
#endif

void hardware_sdhost_initialize(void)
{
#if CPUSTYLE_R7S721

	// Инициализация SD CARD интерфейса на R7S721
	/* ---- Supply clock to the SDHI(channel 0) ---- */
	CPG.STBCR12 &= ~ ((1U << 3) | (1U << 2));	// Module Stop 123, 122  00: The SD host interface 00 runs.
	(void) CPG.STBCR12;			/* Dummy read */

	SDHI0.SOFT_RST = 0x0000;	// SDRST 0: Reset
	SDHI0.SOFT_RST = 0x0001;	// SDRST 1: Reset released

	hardware_sdhost_setbuswidth(0);
	hardware_sdhost_setspeed(400000uL);

	{
		const uint16_t int_id = DMAINT14_IRQn;
		r7s721_intc_registintfunc(int_id, r7s721_sdhi0_dma_handler);
		GIC_SetPriority(int_id, ARM_SYSTEM_PRIORITY);
		GIC_EnableIRQ(int_id);
	}

	HARDWARE_SDIO_INITIALIZE();	// Подсоединить контроллер к выводам процессора

#elif CPUSTYLE_STM32F4XX 

	RCC->APB2ENR |= RCC_APB2ENR_SDIOEN;   // подаем тактирование на SDIO
	__DSB();

	// hwrdware flow control отключен - от этого зависит проверка состояния txfifo & rxfifo
	SDIO->CLKCR =
		1 * SDIO_CLKCR_CLKEN |
		(255 & SDIO_CLKCR_CLKDIV_Msk) |
	#if WITHSDHCHW4BIT
		1 * SDIO_CLKCR_WIDBUS_0 |	// 01: 4-wide bus mode: SDIO_D[3:0] used
	#endif /* WITHSDHCHW4BIT */
		1 * SDIO_CLKCR_HWFC_EN |
		1 * SDIO_CLKCR_PWRSAV |		// выключается clock без обращений
		0;

	hardware_sdhost_setbuswidth(0);
	hardware_sdhost_setspeed(400000uL);

	NVIC_SetPriority(SDIO_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(SDIO_IRQn);	// SDIO_IRQHandler() enable
	NVIC_SetPriority(DMA2_Stream6_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(DMA2_Stream6_IRQn);	// DMA2_Stream6_IRQHandler() enable

	HARDWARE_SDIO_INITIALIZE();	// Подсоединить контроллер к выводам процессора
	// разрешить тактирование карты памяти
	SDIO->POWER = 3 * SDIO_POWER_PWRCTRL_0;

#elif CPUSTYLE_STM32F7XX

	RCC->APB2ENR |= RCC_APB2ENR_SDMMC1EN;   // подаем тактирование на SDMMC1
	__DSB();
	
	// hwrdware flow control отключен - от этого зависит проверка состояния txfifo & rxfifo
	SDMMC1->CLKCR =
		1 * SDMMC_CLKCR_CLKEN |
		(255 & SDMMC_CLKCR_CLKDIV) |
	#if WITHSDHCHW4BIT
		1 * SDMMC_CLKCR_WIDBUS_0 |	// 01: 4-wide bus mode: SDMMC_D[3:0] used
	#endif /* WITHSDHCHW4BIT */
		1 * SDMMC_CLKCR_HWFC_EN |
		1 * SDMMC_CLKCR_PWRSAV |		// выключается clock без обращений
		0;

	hardware_sdhost_setbuswidth(0);
	hardware_sdhost_setspeed(400000uL);

	NVIC_SetPriority(SDMMC1_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(SDMMC1_IRQn);	// SDIO_IRQHandler() enable
	NVIC_SetPriority(DMA2_Stream6_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(DMA2_Stream6_IRQn);	// DMA2_Stream6_IRQHandler() enable

	HARDWARE_SDIO_INITIALIZE();	// Подсоединить контроллер к выводам процессора
	// разрешить тактирование карты памяти
	SDMMC1->POWER = 3 * SDMMC_POWER_PWRCTRL_0;

#elif CPUSTYLE_STM32H7XX

	RCC->AHB3ENR |= RCC_AHB3ENR_SDMMC1EN;   // подаем тактирование на SDMMC1
	(void) RCC->AHB3ENR;
	__DSB();

	HARDWARE_SDIO_INITIALIZE();	// Подсоединить контроллер к выводам процессора
	
	// hwrdware flow control отключен - от этого зависит проверка состояния txfifo & rxfifo
	SDMMC1->CLKCR =
		SDMMC_CLKCR_CLKDIV |
	#if WITHSDHCHW4BIT
		1 * SDMMC_CLKCR_WIDBUS_0 |	// 01: 4-wide bus mode: SDMMC_D[3:0] used
	#endif /* WITHSDHCHW4BIT */
		1 * SDMMC_CLKCR_HWFC_EN |
		1 * SDMMC_CLKCR_PWRSAV |		// выключается clock без обращений
		0;

	hardware_sdhost_setbuswidth(0);
	hardware_sdhost_setspeed(400000uL);

	NVIC_SetPriority(SDMMC1_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(SDMMC1_IRQn);	// SDIO_IRQHandler() enable

	// разрешить тактирование карты памяти
	SDMMC1->POWER = 3 * SDMMC_POWER_PWRCTRL_0;

#else

	#error Wrong CPUSTYLE_xxx

#endif
}

#endif /* WITHSDHCHW */

#if CPUSTYLE_ARM || CPUSTYPE_TMS320F2833X

#if CPUSTYPE_TMS320F2833X
	#pragma CODE_SECTION(local_delay_us, "ramfuncs")
#endif /* CPUSTYPE_TMS320F2833X */

void local_delay_us(int timeUS)
{
	#if CPUSTYLE_AT91SAM7S
		const int top = timeUS * 175 / (CPU_FREQ / 1000000);
	#elif CPUSTYLE_ATSAM3S
		const int top = timeUS * 270 / (CPU_FREQ / 1000000);
	#elif CPUSTYLE_ATSAM4S
		const int top = timeUS * 270 / (CPU_FREQ / 1000000);
	#elif CPUSTYLE_STM32F0XX
		const int top = timeUS * 190 / (CPU_FREQ / 1000000);
	#elif CPUSTYLE_STM32L0XX
		const int top = timeUS * 20 / (CPU_FREQ / 1000000);
	#elif CPUSTYLE_STM32F1XX
		const int top = timeUS * 345 / (CPU_FREQ / 1000000);
	#elif CPUSTYLE_STM32F30X
		const int top = timeUS * 430 / (CPU_FREQ / 1000000);
	#elif CPUSTYLE_STM32F4XX
		const int top = timeUS * 3800 / (CPU_FREQ / 1000000);
	#elif CPUSTYLE_STM32F7XX
		const int top = timeUS * 6150 / (CPU_FREQ / 1000000);
	#elif CPUSTYLE_STM32H7XX
		const int top = timeUS * 11000 / (CPU_FREQ / 1000000);
	#elif CPUSTYLE_R7S721
		const int top = timeUS * 13800 / (CPU_FREQ / 1000000);
	#elif CPUSTYPE_TMS320F2833X && 1 // RAM code
		const unsigned long top = timeUS * 760UL / (CPU_FREQ / 1000000);	// tested @ 100 MHz Execute from RAM
		//const unsigned long top = timeUS * 1600UL / (CPU_FREQ / 1000000);	// tested @ 150 MHz Execute from RAM
	#elif CPUSTYPE_TMS320F2833X	&& 0	// FLASH code
		const unsigned long top = timeUS * 480UL / (CPU_FREQ / 1000000);	// Execute from RAM
	#else
		#error TODO: calibrate local_delay_us constant
		const int top = timeUS * 175 / (CPU_FREQ / 1000000);
	#endif
	//
	volatile int n;
	for (n = 0; n < top; ++ n)
	{
	}
}
// exactly as required
//
void local_delay_ms(int timeMS)
{
	int n;
	for (n = 0; n < timeMS; ++ n)
	{
		local_delay_us(1000);
	}
}


#endif /* CPUSTYLE_ARM || CPUSTYPE_TMS320F2833X */

#if CPUSTYLE_AT91SAM7S || CPUSTYLE_AT91SAM9XE

static void RAMFUNC_NONILINE AT91F_FIQHandler(void)
{
	for (;;)
	{
	}
}

// это прерывание происходит при чтении IVR (interrupt vector register)
// без причины - при отсутствии запомненного запроса.
static void RAMFUNC_NONILINE AT91F_Spurious_handler(void)
{
	//for (;;)
	//{
	//}
}

static void AT91F_DEF_IRQHandler(void) 
{
	for (;;)
	{
	}
}

#if 0
static void lowlevel_init_direct_clock(void)
{
	// before reprogramming - set safe waitstates
	AT91C_BASE_MC->MC_FMR = AT91C_MC_FWS_2FWS;

	AT91C_BASE_PMC->PMC_MCKR = AT91C_PMC_PRES_CLK;	// 32 / 1 = 32

	// Wait until the master clock is established
	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY))
		;

	// use main clock
	AT91C_BASE_PMC->PMC_MCKR = (AT91C_PMC_PRES_CLK | AT91C_PMC_CSS_MAIN_CLK);
	// Wait until the master clock is established
	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY))
		;
	// as final stage - set desired waitstates
	AT91C_BASE_MC->MC_FMR = AT91C_MC_FWS_1FWS; // 0 Wait State to work at 20 MHz, 1 ws up to 48
}
#endif
/* 
  инициализация внутреннего умножителя частоты.
  Вход - 18.432 МГц, кварцевый резонатор
  внутренняя тактовая - 48 МГц,
  частота генератора - 96 МГц
  Частота сравнения PLL = 1.316571 МГц
*/
/* 
  инициализация внутреннего умножителя частоты.
  Вход - 12 МГц, кварцевый резонатор
  внутренняя тактовая - 48 МГц,
  частота генератора - 96 МГц
  Частота сравнения PLL = 12 МГц
*/
static void lowlevel_init_pll_clock_xtal(
	unsigned osc_mul,	// Умножитель петли ФАПЧ
	unsigned osc_div	// Делитель опорного сигнала петли ФАПЧ
	)
{
	// before reprogramming - set safe waitstates
	AT91C_BASE_MC->MC_FMR = AT91C_MC_FWS_2FWS;

	///////////////////////////////////////////////////////////////////////////
	// Init PMC Step 1. Enable Main Oscillator
	// Main Oscillator startup time is board specific:
	// Main Oscillator Startup Time worst case (3MHz) corresponds to 15ms
	// (0x40 for AT91C_CKGR_OSCOUNT field)
	///////////////////////////////////////////////////////////////////////////
	AT91C_BASE_PMC->PMC_MOR = (((AT91C_CKGR_OSCOUNT & (0x40 << 8)) | AT91C_CKGR_MOSCEN));
	// Wait Main Oscillator stabilization
	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCS))
		;
	///////////////////////////////////////////////////////////////////////////
	// Init PMC Step 2.
	// Set PLL to 96MHz (96,109MHz) and UDP Clock to 48MHz
	// PLL Startup time depends on PLL RC filter: worst case is choosen
	// UDP Clock (48,058MHz) is compliant with the Universal Serial Bus
	// Specification (+/- 0.25% for full speed)
	///////////////////////////////////////////////////////////////////////////
	AT91C_BASE_PMC->PMC_PLLR = 
						AT91C_CKGR_USBDIV_1 |
						AT91C_CKGR_OUT_0 |
						(AT91C_CKGR_PLLCOUNT & (16 << 8)) |		// PLL lock signalling delay
						(AT91C_CKGR_MUL & ((osc_mul - 1) << 16)) |
						(AT91C_CKGR_DIV & osc_div);		
	
	// Wait for PLL stabilization
	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCK))
		;
	
	// Wait until the master clock is established for the case we already
	// turn on the PLL
	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY))
		;

	///////////////////////////////////////////////////////////////////////////
	// Init PMC Step 3.
	// Selection of Master Clock MCK equal to (Processor Clock PCK) PLL/2=48MHz
	// The PMC_MCKR register must not be programmed in a single write operation
	// (see. Product Errata Sheet)
	///////////////////////////////////////////////////////////////////////////
	AT91C_BASE_PMC->PMC_MCKR = AT91C_PMC_PRES_CLK_2;	// 96 / 2 = 48

	// Wait until the master clock is established
	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY))
		;

	AT91C_BASE_PMC->PMC_MCKR = (AT91C_PMC_PRES_CLK_2 | AT91C_PMC_CSS_PLL_CLK);

	// Wait until the master clock is established
	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY))
		;

	// as final stage - set desired waitstates
	AT91C_BASE_MC->MC_FMR = AT91C_MC_FWS_1FWS; // 1 Wait State to work at 48 MHz
}

/* 
  инициализация внутреннего тактового генератора без умножителя
  Вход - кварцевый резонатор
  внутренняя тактовая - частота резонатора,
*/
static void lowlevel_init_pll_clock_from_xtal(void)
{
	// before reprogramming - set safe waitstates
	AT91C_BASE_MC->MC_FMR = AT91C_MC_FWS_1FWS;

	///////////////////////////////////////////////////////////////////////////
	// Init PMC Step 1. Enable Main Oscillator
	// Main Oscillator startup time is board specific:
	// Main Oscillator Startup Time worst case (3MHz) corresponds to 15ms
	// (0x40 for AT91C_CKGR_OSCOUNT field)
	///////////////////////////////////////////////////////////////////////////
	AT91C_BASE_PMC->PMC_MOR = (((AT91C_CKGR_OSCOUNT & (0x40 << 8)) | AT91C_CKGR_MOSCEN));
	// Wait Main Oscillator stabilization
	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCS))
		;
	
	// Wait until the master clock is established for the case we already
	// turn on the PLL
	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY))
		;

	///////////////////////////////////////////////////////////////////////////
	// Init PMC Step 3.
	// Selection of Master Clock MCK equal to (Processor Clock PCK) PLL/2=48MHz
	// The PMC_MCKR register must not be programmed in a single write operation
	// (see. Product Errata Sheet)
	///////////////////////////////////////////////////////////////////////////
	AT91C_BASE_PMC->PMC_MCKR = AT91C_PMC_PRES_CLK;	// 12 / 1 = 12

	// Wait until the master clock is established
	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY))
		;

	AT91C_BASE_PMC->PMC_MCKR = (AT91C_PMC_PRES_CLK | AT91C_PMC_CSS_MAIN_CLK);

	// Wait until the master clock is established
	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY))
		;

	// as final stage - set desired waitstates
	//AT91C_BASE_MC->MC_FMR = AT91C_MC_FWS_1FWS; // 1 Wait State to work at 48 MHz
	AT91C_BASE_MC->MC_FMR = AT91C_MC_FWS_0FWS; // 0 Wait State to work at 12 MHz
}

#if CPUSTYLE_AT91SAM9XE
//------------------------------------------------------------------------------
/// Performs the low-level initialization of the chip.
//------------------------------------------------------------------------------
void at91sam9x_clocks(	
	unsigned osc_mul,	// Умножитель петли ФАПЧ (96)
	unsigned osc_div	// Делитель опорного сигнала петли ФАПЧ (9)
	)
{
	/// Main oscillator startup time (in number of slow clock ticks). 
	enum { BOARD_OSCOUNT  =         (AT91C_CKGR_OSCOUNT & (64 << 8)) };

	/// PLLA frequency range.
	// AT91C_CKGR_OUTA_2 - for freqos > 150 MHz
	enum { BOARD_CKGR_PLLA  =       (AT91C_CKGR_SRCA | AT91C_CKGR_OUTA_2) };
	/// PLLA startup time (in number of slow clock ticks).
	enum { BOARD_PLLACOUNT  =       (63 << 8) };
	/// PLLA MUL value.
	const unsigned BOARD_MULA     =         (AT91C_CKGR_MULA & ((osc_mul - 1) << 16)) ;
	//enum { BOARD_MULA     =         (AT91C_CKGR_MULA & (78 << 16)) };
	//enum { BOARD_MULA     =         (AT91C_CKGR_MULA & (24 << 16)) };
	/// PLLA DIV value.
	const unsigned BOARD_DIVA     =         (AT91C_CKGR_DIVA & osc_div);	// 18432 kHz / 9 = 2048 kHz
	/// Master clock prescaler value.
	// 
	// 
	enum { BOARD_PRESCALER     =    AT91C_PMC_PRES_CLK | AT91C_PMC_MDIV_2	 };


#if 0
	/// PLLB frequency range
	enum { BOARD_CKGR_PLLB  =       AT91C_CKGR_OUTB_1 };
	/// PLLB startup time (in number of slow clock ticks).
	enum { BOARD_PLLBCOUNT =         BOARD_PLLACOUNT };
	/// PLLB MUL value.
	enum { BOARD_MULB      =        (124 << 16) };
	/// PLLB DIV value.
	enum { BOARD_DIVB       =       12 };

	/// USB PLL divisor value to obtain a 48MHz clock.
	enum { BOARD_USBDIV     =       AT91C_CKGR_USBDIV_2 };

#endif

    // Set flash wait states
    //----------------------
    //T91C_BASE_EFC->EFC_FMR = 6 << 8;
    AT91C_BASE_EFC->EFC_FMR = AT91C_EFC_FWS_3WS;
    //AT91C_BASE_EFC->EFC_FMR = AT91C_EFC_FWS_1WS;

//#if !defined(sdram)
    // Initialize main oscillator
    //---------------------------
    AT91C_BASE_PMC->PMC_MOR = BOARD_OSCOUNT | AT91C_CKGR_MOSCEN;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCS))
		;

    // Initialize PLLA at 200MHz (198.656)
    AT91C_BASE_PMC->PMC_PLLAR = BOARD_CKGR_PLLA
                                | BOARD_PLLACOUNT
                                | BOARD_MULA
                                | BOARD_DIVA;

    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKA))
		;

#if 0
    // Initialize PLLB for USB usage (if not already locked)
    if (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKB)) {
        AT91C_BASE_PMC->PMC_PLLBR = BOARD_USBDIV
                                    | BOARD_CKGR_PLLB
                                    | BOARD_PLLBCOUNT
                                    | BOARD_MULB
                                    | BOARD_DIVB;
        while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKB))
			;
    }

    // Wait for the master clock if it was already initialized
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY))
	;
#endif

    // Switch to fast clock
    //---------------------
    // Switch to main oscillator + prescaler
    AT91C_BASE_PMC->PMC_MCKR = BOARD_PRESCALER;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY))
		;

    // Switch to PLL + prescaler
    AT91C_BASE_PMC->PMC_MCKR |= AT91C_PMC_CSS_PLLA_CLK;
    while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY))
		;

//#endif //#if !defined(sdram)


#if 0
	{
		unsigned char i;

		// Initialize AIC
		//---------------
		AT91C_BASE_AIC->AIC_IDCR = 0xFFFFFFFF;
		AT91C_BASE_AIC->AIC_SVR[0] = (unsigned int) DefaultFiqHandler;
		for (i = 1; i < 31; i++) {

			AT91C_BASE_AIC->AIC_SVR[i] = (unsigned int) DefaultIrqHandler;
		}
		AT91C_BASE_AIC->AIC_SPU = (unsigned int) DefaultSpuriousHandler;

		// Unstack nested interrupts
		for (i = 0; i < 8 ; i++) {

			AT91C_BASE_AIC->AIC_EOICR = 0;
		}
	}
#endif
    // Watchdog initialization
    //------------------------
    AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;

    // Remap
    //------
    //BOARD_RemapRam();

    // Disable RTT and PIT interrupts (potential problem when program A
    // configures RTT, then program B wants to use PIT only, interrupts
    // from the RTT will still occur since they both use AT91C_ID_SYS)
    AT91C_BASE_RTTC->RTTC_RTMR &= ~ (AT91C_RTTC_ALMIEN | AT91C_RTTC_RTTINCIEN);
    AT91C_BASE_PITC->PITC_PIMR &= ~AT91C_PITC_PITIEN;
}
#endif	/* CPUSTYLE_AT91SAM9XE */

static void lowlevwl_interrupts_init(void)
{
	unsigned i;

	// Disable all interrupts
	AT91C_BASE_AIC->AIC_IDCR = 0xFFFFFFFF;
	// Clear all interrupts
	AT91C_BASE_AIC->AIC_ICCR = 0xFFFFFFFF;

	// Perform 8 IT acknoledge (write any value in EOICR)
	for (i = 0; i < 8 ; i++) {
		AT91C_BASE_AIC->AIC_EOICR = 0;
	}

	// Set up the default interrupts handler vectors
	AT91C_BASE_AIC->AIC_SVR [0] = (unsigned int) AT91F_FIQHandler;
	for (i = 1; i < 31; ++ i)
	{
	    AT91C_BASE_AIC->AIC_SVR [i] = (unsigned int) AT91F_DEF_IRQHandler;
	}
	AT91C_BASE_AIC->AIC_SPU  = (unsigned int) AT91F_Spurious_handler;

	// Enable the Debug mode
	////AT91C_BASE_AIC->AIC_DCR = AT91C_AIC_DCR_PROT;
	
}
/*
Тактирование все же надо включать до подключения, причем аж в двух местах:
Код
    AT91C_BASE_PMC->PMC_SCER = AT91C_PMC_UDP;
    AT91C_BASE_PMC->PMC_PCER = 1UL << AT91C_ID_UDP;


*/

// отключение неиспользуемого USB порта.
static void usb_disable(void)
{
#if defined(AT91C_ID_UDP)
    AT91C_BASE_PMC->PMC_SCER = AT91C_PMC_UDP;
	AT91C_BASE_PMC->PMC_PCER = 1UL << AT91C_ID_UDP; // разрешить тактированние этого блока

	AT91C_BASE_UDP->UDP_TXVC = AT91C_UDP_TXVDIS;	// запрет usb приемо-передатчика

	AT91C_BASE_PMC->PMC_PCDR = 1UL << AT91C_ID_UDP; // запретить тактированние этого блока
	AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_UDP;
#endif // AT91C_ID_UDP
}


#endif /* CPUSTYLE_AT91SAM7S || CPUSTYLE_AT91SAM9XE */

#if CPUSTYLE_ATSAM3S

// Clock Source Selection
static void program_mckr_css(unsigned long cssvalue)
{
    PMC->PMC_MCKR = (PMC->PMC_MCKR & ~ PMC_MCKR_CSS_Msk) | (cssvalue & PMC_MCKR_CSS_Msk);
	// Wiat MCLK ready
    while ((PMC->PMC_SR & PMC_SR_MCKRDY) == 0)
		;
}
// CPU prescaler
static void program_mckr_pres(unsigned long presvalue)
{
    PMC->PMC_MCKR = (PMC->PMC_MCKR & ~ PMC_MCKR_PRES_Msk) | (presvalue & PMC_MCKR_PRES_Msk);
	// Wiat MCLK ready
    while ((PMC->PMC_SR & PMC_SR_MCKRDY) == 0)
		;
}

// If a new value for CSS field corresponds to PLL Clock,
static void program_mckr_switchtopll_a(void)
{
	program_mckr_pres(PMC_MCKR_PRES_CLK_2);	// with /2 divider
	program_mckr_css(PMC_MCKR_CSS_PLLA_CLK);
}

// If a new value for CSS field corresponds to Main Clock
static void program_mckr_switchtomain(void)
{
	program_mckr_css(PMC_MCKR_CSS_MAIN_CLK);
#ifdef PMC_MCKR_PRES_CLK_1
	program_mckr_pres(PMC_MCKR_PRES_CLK_1);	// w/o divider
#else
	program_mckr_pres(PMC_MCKR_PRES_CLK);	// w/o divider
#endif
}

static void program_use_xtal(
	int useXtalFlag	/* 0 - использование RC генератора, не-0 - использование кварцевого генератора */
	)
{
	// бит CKGR_MOR_MOSCSEL - источник MAINCK это кварцевый генератор
	const unsigned long mor = PMC->CKGR_MOR & ~ CKGR_MOR_KEY_Msk;
	if (((mor & CKGR_MOR_MOSCSEL) != 0) == (useXtalFlag != 0))
		return;		// переключение не требуется

	if (useXtalFlag != 0)
		PMC->CKGR_MOR = CKGR_MOR_KEY(0x37) | (mor | CKGR_MOR_MOSCSEL);
	else
		PMC->CKGR_MOR = CKGR_MOR_KEY(0x37) | (mor & ~ CKGR_MOR_MOSCSEL);

	// ожидание переключения кварцевого генератора
	while ((PMC->PMC_SR & PMC_SR_MOSCSELS) == 0)
		;
}

// Enable on-chip RC oscillator
static void program_enable_RC_12MHz(void)
{
#ifdef CKGR_MOR_MOSCRCF_12_MHz
    PMC->CKGR_MOR = (PMC->CKGR_MOR & ~ (CKGR_MOR_MOSCRCF_Msk | CKGR_MOR_KEY_Msk)) | // остальные биты не трогаем
		CKGR_MOR_KEY(0x37) | CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCRCF_12_MHz;
#else
    PMC->CKGR_MOR = (PMC->CKGR_MOR & ~ (CKGR_MOR_MOSCRCF_Msk | CKGR_MOR_KEY_Msk)) | // остальные биты не трогаем
		CKGR_MOR_KEY(0x37) | CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCRCF_12MHZ;
#endif
	// ожидание запуска RC генератора
    while ((PMC->PMC_SR & PMC_SR_MOSCRCS) == 0)	
		;
}

static void program_disable_rc(void)
{
	const unsigned long mor = PMC->CKGR_MOR & ~ (CKGR_MOR_KEY_Msk | CKGR_MOR_MOSCRCEN);
    PMC->CKGR_MOR = mor | CKGR_MOR_KEY(0x37);
}

static void program_disable_xtal(void)
{
	const unsigned long mor = PMC->CKGR_MOR & ~ (CKGR_MOR_KEY_Msk | CKGR_MOR_MOSCXTEN);
    PMC->CKGR_MOR = mor | CKGR_MOR_KEY(0x37);
}

// Enable high-frequency XTAL oscillator
static void program_enable_xtal(void)
{
	const unsigned long mor = PMC->CKGR_MOR & ~ (CKGR_MOR_KEY_Msk | CKGR_MOR_MOSCXTST_Msk);
	if ((mor & CKGR_MOR_MOSCXTEN) != 0)
		return;		// кварцевый генератор уже запущен

    PMC->CKGR_MOR = 
		mor |	// стврые значения битов
		CKGR_MOR_KEY(0x37) | 
		CKGR_MOR_MOSCXTST(128) | 
		CKGR_MOR_MOSCXTEN;
	// ожидание запуска кварцевого генератора
    while ((PMC->PMC_SR & PMC_SR_MOSCXTS) == 0)
		;
}

static void program_enable_plla(unsigned pllmul, unsigned plldiv)
{
#ifdef CKGR_PLLAR_ONE
    /* Initialize PLLA */
    PMC->CKGR_PLLAR = 
		(CKGR_PLLAR_ONE |	// всегда должен быть установлен
		((pllmul - 1) << CKGR_PLLAR_MULA_Pos) | 
		(0x4 << CKGR_PLLAR_PLLACOUNT_Pos) | 
		(plldiv << CKGR_PLLAR_DIVA_Pos));
#else
    /* Initialize PLLA */
    PMC->CKGR_PLLAR = 
		(CKGR_PLLAR_STUCKTO1 |	// всегда должен быть установлен
		((pllmul - 1) << CKGR_PLLAR_MULA_Pos) | 
		(0x4 << CKGR_PLLAR_PLLACOUNT_Pos) | 
		(plldiv << CKGR_PLLAR_DIVA_Pos));
#endif
	// Ожидание запуска PLL A
    while (!(PMC->PMC_SR & PMC_SR_LOCKA))
		;
}

#if 0
// unused now.
static void program_enable_pllb(void)
{
	//unsigned timer = 0xffffff;
	//enum { osc_mul = 32, osc_div = 6 };	// 12 MHz / 6 * 32 = 64 MHz
	enum { osc_mul = 8, osc_div = 1 };	// 12 MHz / 1 * 8 = 96 MHz
	//enum { osc_mul = 32, osc_div = 3 };	// 12 MHz / 3 * 32 = 128 MHz

    /* Initialize PLLA */
    PMC->CKGR_PLLBR = 
		//CKGR_PLLBR_STUCKTO1 |	// всегда должен быть установлен
		((osc_mul - 1) << CKGR_PLLBR_MULB_Pos) | 
		(0x1 << CKGR_PLLBR_PLLBCOUNT_Pos) | 
		(osc_div << CKGR_PLLBR_DIVB_Pos);
    //timeout = 0;
    //while (!(PMC->PMC_SR & PMC_SR_LOCKA) && (timeout++ < CLOCK_TIMEOUT))
    while ((PMC->PMC_SR & PMC_SR_LOCKB) == 0)
		;
}
#endif


// If a new value for CSS field corresponds to Slow Clock,
static void program_mckr_switchtoslow(void)
{
	program_mckr_css(PMC_MCKR_CSS_SLOW_CLK);
#ifdef PMC_MCKR_PRES_CLK_1
	program_mckr_pres(PMC_MCKR_PRES_CLK_1);	// w/o divider
#else
	program_mckr_pres(PMC_MCKR_PRES_CLK);	// w/o divider
#endif
}

// В описании процессора упоминается о том, что модификацию EEFC_FMR
// нельзя выполняить кодом из FLASH ROM. Данная пфункция копируется в SRAM
// и работает оттуда
static void RAMFUNC_NONILINE lowlevel_sam3s_setws(unsigned fws)
{
	EFC->EEFC_FMR = EEFC_FMR_FWS(fws);	// Flash Wait State 
}

// Перенастройка на работу с внутренним RC генератором 12 МГц	
static void lowlevel_sam3s_init_clock_12_RC12(void)
{
	program_enable_RC_12MHz();
	program_use_xtal(0);			// use RC
	program_disable_xtal();
	program_mckr_switchtomain();
}

	
/* 
  инициализация внутреннего умножителя частоты.
*/
static void lowlevel_sam3s_init_pll_clock_RC12(unsigned pllmul, unsigned plldiv, unsigned fws)
{
	// Embedded Flash Wait State VDDCORE set at 1.80V
	// FWS field = 0: up to 22 MHz
	// FWS field = 1: up to 38 MHz
	// FWS field = 2: up to 64 MHz
	lowlevel_sam3s_setws(3);	// Flash Wait State 

	//program_mckr_switchtoslow();	// переключаем на внутренний генератор 32 кГц
	program_mckr_switchtomain();	// выключить ФАПЧ, если была включена
	program_enable_RC_12MHz();
	program_use_xtal(0);
	program_disable_xtal();
	program_enable_plla(pllmul, plldiv);
	//program_enable_pllb();
	program_mckr_switchtopll_a();

	lowlevel_sam3s_setws(fws);	// Flash Wait State 
}

/* 
  инициализация внутреннего умножителя частоты.
  Вход - 12 МГц, кварцевый резонатор
  внутренняя тактовая - 64 МГц,
  частота генератора - 12 МГц
  Частота сравнения PLL = 4 МГц
*/
static void 
lowlevel_sam3s_init_pll_clock_xtal(unsigned pllmul, unsigned plldiv, unsigned ws)
{
	// Embedded Flash Wait State VDDCORE set at 1.80V
	// FWS field = 0: up to 22 MHz
	// FWS field = 1: up to 38 MHz
	// FWS field = 2: up to 64 MHz
	lowlevel_sam3s_setws(3);	// Flash Wait State 

	//program_mckr_switchtoslow();	// переключаем на внутренний генератор 32 кГц
	program_mckr_switchtomain();	// выключить ФАПЧ, если была включена
	program_enable_xtal();
	program_use_xtal(1);
	program_disable_rc();
	program_enable_plla(pllmul, plldiv);
	//program_enable_pllb();
	program_mckr_switchtopll_a();

	lowlevel_sam3s_setws(ws);	// Flash Wait State 
}


#endif /* CPUSTYLE_ATSAM3S */


#if CPUSTYLE_STM32F7XX || CPUSTYLE_STM32F4XX
// Настроить выход PLLQ на 48 МГц
uint_fast32_t arm_hardware_stm32f7xx_pllq_initialize(void)
{
	const uint32_t stm32f4xx_pllq = calcdivround2(PLL_FREQ, 48000000uL);	// Как было сделано при инициализации PLL
	// PLLQ: Main PLL (PLL) division factor for USB OTG FS, SDIO and random number generator clocks
	// Should be 48 MHz or less for SDIO and 48 MHz with small tolerance.
	RCC->PLLCFGR = (RCC->PLLCFGR & ~ RCC_PLLCFGR_PLLQ) | 
		((RCC_PLLCFGR_PLLQ_0 * stm32f4xx_pllq) & RCC_PLLCFGR_PLLQ) |
		0;

#if defined(RCC_DCKCFGR2_CK48MSEL)
	RCC->DCKCFGR2 = (RCC->DCKCFGR2 & ~ (RCC_DCKCFGR2_CK48MSEL)) |
		0 * RCC_DCKCFGR2_CK48MSEL |
		0;
#elif defined(RCC_DCKCFGR_CK48MSEL)
	// stm32f469x, stm32f479xx
	RCC->DCKCFGR = (RCC->DCKCFGR & ~ (RCC_DCKCFGR_CK48MSEL)) |
		0 * RCC_DCKCFGR_CK48MSEL |
		0;
#endif /* defined(CC_DCKCFGR2_CK48MSEL) */

	return stm32f4xx_pllq;
}
#endif /* CPUSTYLE_STM32F7XX || CPUSTYLE_STM32F4XX */


#if CPUSTYLE_STM32F4XX

static void 
lowlevel_stm32f4xx_pll_initialize(void)
{
	//const unsigned PLL1M = REF1_MUL;		// N умножитель в PLL1
	//const unsigned PLL1P = PLL1_P;			//  делитель перед SYSTEM CLOCK MUX в PLL1
	//const unsigned PLL1Q = PLL1_Q;			// делитель на выход 48 МГц

	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;     // включить тактирование альтернативных функций
	__DSB();
	SYSCFG->CMPCR |= SYSCFG_CMPCR_CMP_PD;	// enable i/o compensaion cell
	while ((SYSCFG->CMPCR & SYSCFG_CMPCR_READY) == 0)
		;

	RCC->CR |= RCC_CR_HSION;		//включаю внутренний генератор
	while ((RCC->CR & RCC_CR_HSIRDY) == 0)//жду пока не заработает
		;

	RCC->CFGR |= RCC_CFGR_SW_HSI;
	while ((RCC->CFGR & RCC_CFGR_SWS) != (RCC_CFGR_SWS_0 * 0))
		;

	#if WITHCPUXTAL
		// Внешний кварцевый резонатор
		RCC->CR = (RCC->CR & ~ RCC_CR_HSEBYP) | RCC_CR_HSEON;	// включаю внешний генератор
		while ((RCC->CR & RCC_CR_HSERDY) == 0)	// жду пока не заработает
			;

		RCC->PLLCFGR = (RCC->PLLCFGR & ~ RCC_PLLCFGR_PLLSRC) | RCC_PLLCFGR_PLLSRC_HSE;//HSE - исчтоник для PLL

	#else /* WITHCPUXTAL */
		// внутренний RC генератор 16 МГц
		RCC->PLLCFGR = (RCC->PLLCFGR & ~ RCC_PLLCFGR_PLLSRC) | RCC_PLLCFGR_PLLSRC_HSI;//HSI - исчтоник для PLL

	#endif /* WITHCPUXTAL */



	RCC->PLLCFGR = (RCC->PLLCFGR & ~ (RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLP)) | 
		((REF1_DIV * RCC_PLLCFGR_PLLM_0) & RCC_PLLCFGR_PLLM) |
		((REF1_MUL * RCC_PLLCFGR_PLLN_0) & RCC_PLLCFGR_PLLN) |
#if PLL1_P == 4
		((1 * RCC_PLLCFGR_PLLP_0) & RCC_PLLCFGR_PLLP) |	// 0: VCO / 2, 1: VCO / 4
#else
		((0 * RCC_PLLCFGR_PLLP_0) & RCC_PLLCFGR_PLLP) |	// 0: VCO / 2, 1: VCO / 4
#endif
		0;

	RCC->CR |= RCC_CR_PLLON;				// Включил PLL

#if defined (PWR_CR_VOS_0)

	// Use overdrive
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;	// включить тактирование power management
	__DSB();

	PWR->CR = (PWR->CR & ~ (PWR_CR_VOS)) |
		3 * PWR_CR_VOS_0 |
		0;

	#if defined (PWR_CR_ODEN)
		// Over Drive enable  
		PWR->CR |= PWR_CR_ODEN;
		__DSB();
		while((PWR->CSR & PWR_CSR_ODRDY) == 0)
			;
		// Over Drive switch enabled 
		PWR->CR |= PWR_CR_ODSWEN;
		__DSB();
		while((PWR->CSR & PWR_CSR_ODSWRDY) == 0)
			;
	#endif /* defined (PWR_CR_ODEN) */

#endif /* (PWR_CR_VOS_0) */

	while ((RCC->CR & RCC_CR_PLLRDY) == 0)	// пока заработает PLL
		;

	const portholder_t flash_acr_latency = HARDWARE_FLASH_LATENCY; // Задержка для работы с памятью 5 WS for 168 MHz at 3.3 volt
	/* Блок настройки ФЛЭШ */
	/* Reserved bits must be kept cleared. */
    //static const uint32_t REVISION_Z = 0x10010000;	// У STM32F407/STM32F417 prefetch включать только в "Z"
	FLASH->ACR = 
		FLASH_ACR_DCEN |		// Включил КЭШ данных
		FLASH_ACR_ICEN |		// Включил КЭШ команд
		//((DBGMCU->IDCODE & DBGMCU_IDCODE_REV_ID) == REVISION_Z ? FLASH_ACR_PRFTEN : 0) |		// Включил Pefetch для ускорения
		FLASH_ACR_PRFTEN |
		flash_acr_latency |		//Задержка для работы с памятью
		0;

	while ((FLASH->ACR & FLASH_ACR_LATENCY) != flash_acr_latency)
		;

#if defined(STM32F401xC)
	RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_HPRE)) |
		RCC_CFGR_HPRE_DIV2 |
		0;
#endif

	RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2 | RCC_CFGR_SW)) |
#if defined(STM32F401xC)
		(RCC_CFGR_PPRE1_0 * 4) |	// APB1 prescaler 0x05: AHB clock divided by 2: понизил частоту APB1 до 42МГц
		(RCC_CFGR_PPRE2_0 * 0) |	// APB2 prescaler 0x04: AHB clock divided by 1: 84MHz
#else
		(RCC_CFGR_PPRE1_0 * 5) |	// APB1 prescaler 0x05: AHB clock divided by 4: понизил частоту APB1 до 42МГц
		(RCC_CFGR_PPRE2_0 * 4) |	// APB2 prescaler 0x04: AHB clock divided by 2: 84MHz
#endif
		RCC_CFGR_SW_PLL | // PLL as system clock
		0;

	while ((RCC->CFGR & RCC_CFGR_SWS) != (RCC_CFGR_SWS_0 * 2))
		;

#if defined (RCC_DCKCFGR_TIMPRE)
	RCC->DCKCFGR = (RCC->DCKCFGR & ~ RCC_DCKCFGR_TIMPRE) |
		((0x00 * MASK2LSB(RCC_DCKCFGR_TIMPRE)) & RCC_DCKCFGR_TIMPRE)	|	// Timers clocks prescalers selection
		0;
#endif /*  defined (RCC_DCKCFGR_TIMPRE) */

	//RCC->CR &= ~ RCC_CR_HSION;		//HSI DISABLE


#if WITHCPUXTAL
	// HSI (16 MHz RC) off
	//RCC->CR &= ~ RCC_CR_HSION;
#endif
}

#if WITHUSESAIPLL

static void 
lowlevel_stm32f4xx_pllsai_initialize(void)
{
	//#error TODO: write code to imitialize SAI PLL and LTDC output divider
	/* для устройств на шине APB2 (up to 72 MHz) */
	auto uint_fast32_t 
	calcdivround_saifreq(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		//#error TODO: check freq at outputs vsync/hsync
		return (PLLSAI_FREQ + freq / 2) / freq;
	}

	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_saifreq(LTDC_DOTCLK), STM32F_LTDC_DIV_WIDTH, STM32F_LTDC_DIV_TAPS, & value, 0);
	ASSERT(value >= 2);
	debug_printf_P(PSTR("lowlevel_stm32f4xx_pllsai_initialize: value=%u, prei=%u\n"), value, prei);
	// Настройка PLLSAI
	// Частота сравнения та же самая, что и в основной PLL
	// RCC_PLLSAICFGR_PLLSAIQ используется, если для SAI используется отдельная PLL - эта.
	RCC->PLLSAICFGR = (RCC->PLLSAICFGR & ~ (RCC_PLLSAICFGR_PLLSAIN | /*RCC_PLLSAICFGR_PLLSAIQ | */ RCC_PLLSAICFGR_PLLSAIR)) |
		((SAIREF1_MUL << RCC_PLLSAICFGR_PLLSAIN_Pos) & RCC_PLLSAICFGR_PLLSAIN) |	// PLLI2SN bits = multiplier, freq=192..432 MHz, vale = 2..432
		((value * RCC_PLLSAICFGR_PLLSAIR_0) & RCC_PLLSAICFGR_PLLSAIR) |	// PLLI2SR bits - output divider, 2..7
		0;

	RCC->DCKCFGR = (RCC->DCKCFGR & ~ RCC_DCKCFGR_PLLSAIDIVR) |
		((prei << RCC_DCKCFGR_PLLSAIDIVR_Pos) & RCC_DCKCFGR_PLLSAIDIVR) |	// division factor for LCD_CLK. 2: PLLSAIDIVR = /8  3: PLLSAIDIVR = /16
		0;

	RCC->CR |= RCC_CR_PLLSAION;				// Включил PLL
	while ((RCC->CR & RCC_CR_PLLSAIRDY) == 0)	// пока заработает PLL
		;
}

#endif /* WITHUSESAIPLL */

static void 
lowlevel_stm32f4xx_MCOx_test(void)
{
	if (0)
	{
		// Тестирование тактовой частоты - подача на сигнал MCO1
		RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_MCO1 | RCC_CFGR_MCO1PRE)) |
			6 * RCC_CFGR_MCO1PRE_0 |	// Смотрим sysclk / 4
			0 * RCC_CFGR_MCO1_0 |	// 0: sysclk
			0;
		arm_hardware_pioa_altfn50(1U << 8, AF_SYSTEM);	// PA8, AF=0: MCO1
		for (;;)
			;
	}
	if (0)
	{
		// Тестирование тактовой частоты - подача на сигнал MCO2
		RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_MCO2 | RCC_CFGR_MCO2PRE)) |
			6 * RCC_CFGR_MCO2PRE_0 |	// Смотрим sysclk / 4
			0 * RCC_CFGR_MCO2_0 |	// 0: sysclk
			0;
		arm_hardware_pioc_altfn50(1U << 9, AF_SYSTEM);	// PC9, AF=0: MCO2
		for (;;)
			;
	}
}

#endif /* CPUSTYLE_STM32F4XX */

#if CPUSTYLE_STM32F7XX


// Программируем на 216 МГц
static void 
lowlevel_stm32f7xx_pll_initialize(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;     // включить тактирование альтернативных функций
	__DSB();
	SYSCFG->CMPCR |= SYSCFG_CMPCR_CMP_PD;	// enable i/o compensaion cell
	while ((SYSCFG->CMPCR & SYSCFG_CMPCR_READY) == 0)
		;

	RCC->CR |= RCC_CR_HSION;		//включаю внутренний генератор
	while ((RCC->CR & RCC_CR_HSIRDY) == 0)//жду пока не заработает
		;

	RCC->CFGR |= RCC_CFGR_SW_HSI;
	while ((RCC->CFGR & RCC_CFGR_SWS) != (RCC_CFGR_SWS_0 * 0))
		;

	#if WITHCPUXOSC
		// Внешний кварцевый генератор
		RCC->CR = (RCC->CR & ~ RCC_CR_HSEBYP) | 
			RCC_CR_HSEBYP |	// приходит внешний тактовый сигнал
			RCC_CR_HSEON |	// включаю внешний генератор
			0;
		while ((RCC->CR & RCC_CR_HSERDY) == 0)	// жду пока не заработает
			;

		RCC->PLLCFGR = (RCC->PLLCFGR & ~ RCC_PLLCFGR_PLLSRC) | RCC_PLLCFGR_PLLSRC_HSE;//HSE - исчтоник для PLL

	#elif WITHCPUXTAL
		// Внешний кварцевый резонатор
		RCC->CR = (RCC->CR & ~ RCC_CR_HSEBYP) | 
			//RCC_CR_HSEBYP |	// приходит внешний тактовый сигнал
			RCC_CR_HSEON |	// включаю внешний генератор
			0;
		while ((RCC->CR & RCC_CR_HSERDY) == 0)	// жду пока не заработает
			;

		RCC->PLLCFGR = (RCC->PLLCFGR & ~ RCC_PLLCFGR_PLLSRC) | RCC_PLLCFGR_PLLSRC_HSE;//HSE - исчтоник для PLL

	#else /* WITHCPUXTAL */
		// внутренний RC генератор 16 МГц
		RCC->PLLCFGR = (RCC->PLLCFGR & ~ RCC_PLLCFGR_PLLSRC) | RCC_PLLCFGR_PLLSRC_HSI;//HSI - исчтоник для PLL

	#endif /* WITHCPUXTAL */

	RCC->PLLCFGR = (RCC->PLLCFGR & ~ (RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLP)) | 
		((REF1_DIV * RCC_PLLCFGR_PLLM_0) & RCC_PLLCFGR_PLLM) |	// 18.432 MHz / 18 = 1.024 MHz
		((REF1_MUL * RCC_PLLCFGR_PLLN_0) & RCC_PLLCFGR_PLLN) | // 1.024 MHz * 350 = 358.4 MHz
		((0 * RCC_PLLCFGR_PLLP_0) & RCC_PLLCFGR_PLLP) |	// 0: VCO / 2, 1: VCO / 4
		0;

	RCC->CR |= RCC_CR_PLLON;				// Включил PLL

#if 1

	// Use overdrive
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;	// включить тактирование power management
	__DSB();

	PWR->CR1 = (PWR->CR1 & ~ (PWR_CR1_VOS)) |
		3 * PWR_CR1_VOS_0 |
		0;
	// Over Drive enable  
	PWR->CR1 |= PWR_CR1_ODEN;
	__DSB();
	while((PWR->CSR1 & PWR_CSR1_ODRDY) == 0)
		;
	// Over Drive switch enabled 
	PWR->CR1 |= PWR_CR1_ODSWEN;
	__DSB();
	while((PWR->CSR1 & PWR_CSR1_ODSWRDY) == 0)
		;
#endif

	while ((RCC->CR & RCC_CR_PLLRDY) == 0)	// пока заработает PLL
		;

	const portholder_t flash_acr_latency = HARDWARE_FLASH_LATENCY; // Задержка для работы с памятью 5 WS for 168 MHz at 3.3 volt
	/* Блок настройки ФЛЭШ */
	/* Reserved bits must be kept cleared. */
	FLASH->ACR = 
		FLASH_ACR_PRFTEN |		//Включил Pefetch для ускорения
		FLASH_ACR_ARTEN |		// работает только при обращенияя через FLASHITCM
		flash_acr_latency |		//Задержка для работы с памятью
		0;
	while ((FLASH->ACR & FLASH_ACR_LATENCY) != flash_acr_latency)
		;

	RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2)) |
		(RCC_CFGR_PPRE1_0 * 5) |	// APB1 prescaler 0x05: AHB clock divided by 4: понизил частоту APB1 до 42МГц
		(RCC_CFGR_PPRE2_0 * 4) |	// APB2 prescaler 0x04: AHB clock divided by 2: 84MHz
		RCC_CFGR_SW_PLL; // PLL as system clock

	while ((RCC->CFGR & RCC_CFGR_SWS) != (RCC_CFGR_SWS_0 * 2))
		;

	RCC->DCKCFGR1 = (RCC->DCKCFGR1 & ~ RCC_DCKCFGR1_TIMPRE) |
		((0x00 << 24) & RCC_DCKCFGR1_TIMPRE)	|	// Timers clocks prescalers selection
		0;

	//RCC->CR &= ~ RCC_CR_HSION;		//HSI DISABLE

}

#if WITHUSESAIPLL

static void
lowlevel_stm32f7xx_pllsai_initialize(void)
{

	/* для устройств на шине APB2 (up to 72 MHz) */
	auto uint_fast32_t 
	calcdivround_saifreq(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		//#error TODO: check freq at outputs vsync/hsync
		return (PLLSAI_FREQ + freq / 2) / freq;
	}

#if defined (RCC_PLLSAICFGR_PLLSAIR)

	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_saifreq(LTDC_DOTCLK), STM32F_LTDC_DIV_WIDTH, STM32F_LTDC_DIV_TAPS, & value, 0);
	ASSERT(value >= 2);
	debug_printf_P(PSTR("lowlevel_stm32f7xx_pllsai_initialize: value=%u, prei=%u\n"), value, prei);


	// Настройка PLLSAI
	// Частота сравнения та же самая, что и в основной PLL
	// RCC_PLLSAICFGR_PLLSAIQ используется, если для SAI используется отдельная PLL - эта.
	RCC->PLLSAICFGR = (RCC->PLLSAICFGR & ~ (RCC_PLLSAICFGR_PLLSAIN | /*RCC_PLLSAICFGR_PLLSAIQ | */ RCC_PLLSAICFGR_PLLSAIR)) |
		((SAIREF1_MUL << RCC_PLLSAICFGR_PLLSAIN_Pos) & RCC_PLLSAICFGR_PLLSAIN) |	// PLLI2SN bits = multiplier, freq=192..432 MHz, vale = 2..432
		((value << RCC_PLLSAICFGR_PLLSAIR_Pos) & RCC_PLLSAICFGR_PLLSAIR) |	// PLLI2SR bits - output divider, 2..7
		0;

	RCC->DCKCFGR1 = (RCC->DCKCFGR1 & ~ RCC_DCKCFGR1_PLLSAIDIVR) |
		((prei << RCC_DCKCFGR1_PLLSAIDIVR_Pos) & RCC_DCKCFGR1_PLLSAIDIVR) |	// division factor for LCD_CLK. 2: PLLSAIDIVR = /8  3: PLLSAIDIVR = /16
		0;
#else
	// Настройка PLLSAI
	// Частота сравнения та же самая, что и в основной PLL
	// RCC_PLLSAICFGR_PLLSAIQ используется, если для SAI используется отдельная PLL - эта.
	RCC->PLLSAICFGR = (RCC->PLLSAICFGR & ~ (RCC_PLLSAICFGR_PLLSAIN)) |
		((SAIREF1_MUL << RCC_PLLSAICFGR_PLLSAIN_Pos) & RCC_PLLSAICFGR_PLLSAIN) |	// PLLI2SN bits = multiplier, freq=192..432 MHz, vale = 2..432
		0;

#endif

	RCC->CR |= RCC_CR_PLLSAION;				// Включил PLL
	while ((RCC->CR & RCC_CR_PLLSAIRDY) == 0)	// пока заработает PLL
		;
}
#endif /* WITHUSESAIPLL */

#endif /* CPUSTYLE_STM32F7XX */

#if CPUSTYLE_STM32H7XX

// Программируем на 384 МГц
static void 
lowlevel_stm32h7xx_pll_initialize(void)
{
	RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;     // включить тактирование альтернативных функций
	__DSB();
	//SYSCFG->CMPCR |= SYSCFG_CMPCR_CMP_PD;	// enable i/o compensaion cell
	//while ((SYSCFG->CMPCR & SYSCFG_CMPCR_READY) == 0)
	//	;

	RCC->CR |= RCC_CR_HSION;		// 64 MHz включаю внутренний генератор
	while ((RCC->CR & RCC_CR_HSIRDY) == 0) //жду пока не заработает
		;

	RCC->CFGR |= RCC_CFGR_SW_HSI;
	while ((RCC->CFGR & RCC_CFGR_SWS) != (RCC_CFGR_SWS_0 * 0))
		;

	RCC->CR |= RCC_CR_HSI48ON;		// 48 MHz включаю внутренний генератор
	while ((RCC->CR & RCC_CR_HSI48RDY) == 0) //жду пока не заработает
		;

	/* Configure voltage regulator */
	//Set the highest core voltage (Scale 1)
	PWR->CR3 = PWR_CR3_LDOEN | PWR_CR3_SCUEN;
	PWR->D3CR = (PWR->D3CR & ~ (PWR_D3CR_VOS)) |
		PWR_D3CR_VOS_0 * 3 |		// SCALE 1
		0;
	__DSB();
	//Wait for LDO ready
	while ((PWR->D3CR & PWR_D3CR_VOSRDY) == 0)
		;

	#if WITHCPUXOSC
		// Внешний кварцевый генератор
		RCC->CR = (RCC->CR & ~ RCC_CR_HSEBYP) | 
			RCC_CR_HSEBYP |	// приходит внешний тактовый сигнал
			RCC_CR_HSEON |	// включаю внешний генератор
			0;
		while ((RCC->CR & RCC_CR_HSERDY) == 0)	// жду пока не заработает
			;

		RCC->PLLCFGR = (RCC->PLLCFGR & ~ RCC_PLLCFGR_PLLSRC) | RCC_PLLCFGR_PLLSRC_HSE;//HSE - исчтоник для PLL

	#elif WITHCPUXTAL
		// Внешний кварцевый резонатор
		RCC->CR = (RCC->CR & ~ RCC_CR_HSEBYP) | 
			//RCC_CR_HSEBYP |	// приходит внешний тактовый сигнал
			RCC_CR_HSEON |	// включаю внешний генератор
			0;
		while ((RCC->CR & RCC_CR_HSERDY) == 0)	// жду пока не заработает
			;

		RCC->PLLCKSELR = (RCC->PLLCKSELR & ~ RCC_PLLCKSELR_PLLSRC) | 
			RCC_PLLCKSELR_PLLSRC_HSE |	// HSE - исчтоник для PLL
			0;

	#else /* WITHCPUXTAL */
		// внутренний RC генератор 64 МГц
		RCC->PLLCKSELR = (RCC->PLLCKSELR & ~ RCC_PLLCKSELR_PLLSRC) | 
			RCC_PLLCKSELR_PLLSRC_HSI |	// HSI - исчтоник для PLL
			0;

	#endif /* WITHCPUXTAL */

	// D1 domain Core prescaler (to CPU)
	RCC->D1CFGR = (RCC->D1CFGR & ~ (RCC_D1CFGR_D1CPRE)) |	
		0 * RCC_D1CFGR_D1CPRE_0 |	// not divided
		0;
	(void) RCC->D1CFGR;

	// D1 domain AHB prescaler
	RCC->D1CFGR = (RCC->D1CFGR & ~ (RCC_D1CFGR_HPRE)) |
		8 * RCC_D1CFGR_HPRE_0 |		// 1000: rcc_hclk3 = sys_d1cpre_ck / 2
		0;
	(void) RCC->D1CFGR;

	// D1 domain APB3 prescaler
	RCC->D1CFGR = (RCC->D1CFGR & ~ (RCC_D1CFGR_D1PPRE)) |
		4 * RCC_D1CFGR_D1PPRE_0 |	// 100: rcc_pclk3 = rcc_hclk3 / 2
		0;
	(void) RCC->D1CFGR;

	// D2 domain APB1 prescaler
	RCC->D2CFGR = (RCC->D2CFGR & ~ (RCC_D2CFGR_D2PPRE1)) |
		4 * RCC_D2CFGR_D2PPRE1_0 |	// 100: rcc_pclk1 = rcc_hclk1 / 2	
		0;
	(void) RCC->D2CFGR;

	// D2 domain APB2 prescaler
	RCC->D2CFGR = (RCC->D2CFGR & ~ (RCC_D2CFGR_D2PPRE2)) |
		5 * RCC_D2CFGR_D2PPRE2_0 |	// 100: rcc_pclk2 = rcc_hclk1 / 2
		0;
	(void) RCC->D2CFGR;

	// D3 domain APB4 prescaler
	RCC->D3CFGR = (RCC->D3CFGR & ~ (RCC_D3CFGR_D3PPRE)) |
		4 * RCC_D3CFGR_D3PPRE_0 |	// 100: rcc_pclk4 = rcc_hclk4 / 2
		0;
	(void) RCC->D3CFGR;


	// PLL1 setup
	RCC->PLLCKSELR = (RCC->PLLCKSELR & ~ RCC_PLLCKSELR_DIVM1) | 
		((REF1_DIV << RCC_PLLCKSELR_DIVM1_Pos) & RCC_PLLCKSELR_DIVM1) |	// Reference divider - не требуется корректировань число
		0;
	// 
	const uint32_t stm32h7xx_pllq = calcdivround2(PLL_FREQ, 48000000uL);	// Как было сделано при инициализации PLL
	RCC->PLL1DIVR = (RCC->PLL1DIVR & ~ (RCC_PLL1DIVR_N1 | RCC_PLL1DIVR_P1 | RCC_PLL1DIVR_Q1)) |
		(((REF1_MUL - 1) << RCC_PLL1DIVR_N1_Pos) & RCC_PLL1DIVR_N1) |
		(((2 - 1) << RCC_PLL1DIVR_P1_Pos) & RCC_PLL1DIVR_P1) |	// divede to 2, 3 is not allowed
		(((stm32h7xx_pllq - 1) << RCC_PLL1DIVR_Q1_Pos) & RCC_PLL1DIVR_Q1) |	// нужно для нормального переключения SPI clock USB clock
		0;
	RCC->PLLCFGR = (RCC->PLLCFGR & ~ (RCC_PLLCFGR_DIVP1EN | RCC_PLLCFGR_DIVQ1EN | RCC_PLLCFGR_PLL1VCOSEL | RCC_PLLCFGR_PLL1RGE)) |
		RCC_PLLCFGR_DIVP1EN |	// This bit can be written only when the PLL1 is disabled (PLL1ON = ‘0’ and PLL1RDY = ‘0’).
		RCC_PLLCFGR_DIVQ1EN |	// This bit can be written only when the PLL1 is disabled (PLL1ON = ‘0’ and PLL1RDY = ‘0’).
#if PLL_FREQ >= 150000000uL && PLL_FREQ <= 420000000uL
		1 * RCC_PLLCFGR_PLL1VCOSEL |	// 1: Medium VCO range: 150 to 420 MHz
#else
		0 * RCC_PLLCFGR_PLL1VCOSEL |	// 0: Wide VCO range: 192 to 836 MHz (default after reset)
#endif
		0 * RCC_PLLCFGR_PLL1RGE_0 |	// 00: The PLL1 input (ref1_ck) clock range frequency is between 1 and 2 MHz
		0;

	RCC->CR |= RCC_CR_PLL1ON;				// Включил PLL


	while ((RCC->CR & RCC_CR_PLL1RDY) == 0)	// пока заработает PLL
		;

#if LCDMODE_LTDC && ! WITHUSEPLL3
	#error WITHUSEPLL3 should be defined if LCDMODE_LTDC used.
#endif /* LCDMODE_LTDC && ! WITHUSEPLL3 */

#if WITHUSEPLL3 && defined (LTDC_DOTCLK)

	RCC->PLLCKSELR = (RCC->PLLCKSELR & ~ RCC_PLLCKSELR_DIVM3) | 
		((REF3_DIV << RCC_PLLCKSELR_DIVM3_Pos) & RCC_PLLCKSELR_DIVM3) |	// Reference divider - не требуется корректировань число
		0;
	// 
	const uint32_t ltdc_divr = calcdivround2(PLL3_FREQ, LTDC_DOTCLK);
	RCC->PLL3DIVR = (RCC->PLL3DIVR & ~ (RCC_PLL3DIVR_N3 | RCC_PLL3DIVR_R3)) |
		(((REF3_MUL - 1) << RCC_PLL3DIVR_N3_Pos) & RCC_PLL3DIVR_N3) |
		(((ltdc_divr - 1) << RCC_PLL3DIVR_R3_Pos) & RCC_PLL3DIVR_R3) |	// нужно для нормального переключения SPI clock USB clock
		0;
	RCC->PLLCFGR = (RCC->PLLCFGR & ~ (RCC_PLLCFGR_DIVR3EN | RCC_PLLCFGR_PLL3RGE | RCC_PLLCFGR_PLL3VCOSEL)) |
		RCC_PLLCFGR_DIVR3EN |	// This bit can be written only when the PLL3 is disabled (PLL3ON = ‘0’ and PLL3RDY = ‘0’).
#if PLL3_FREQ >= 150000000uL && PLL3_FREQ <= 420000000uL
		1 * RCC_PLLCFGR_PLL3VCOSEL |	// 1: Medium VCO range: 150 to 420 MHz
#else
		0 * RCC_PLLCFGR_PLL3VCOSEL |	// 0: Wide VCO range: 192 to 836 MHz (default after reset)
#endif
		0 * RCC_PLLCFGR_PLL3RGE_0 |	// 00: The PLL3 input (ref3_ck) clock range frequency is between 1 and 2 MHz
		0;

	RCC->CR |= RCC_CR_PLL3ON;				// Включил PLL3


	while ((RCC->CR & RCC_CR_PLL3RDY) == 0)	// пока заработает PLL
		;

#endif /* WITHUSEPLL3 && defined (LTDC_DOTCLK) */

	const portholder_t flash_acr_latency = HARDWARE_FLASH_LATENCY; // Задержка для работы с памятью 5 WS for 168 MHz at 3.3 volt
	/* Блок настройки ФЛЭШ */
	/* Reserved bits must be kept cleared. */
	FLASH->ACR = (FLASH->ACR & ~ (FLASH_ACR_LATENCY | FLASH_ACR_WRHIGHFREQ)) |
		FLASH_ACR_WRHIGHFREQ_3 |
		flash_acr_latency |		//Задержка для работы с памятью
		0;

	while ((FLASH->ACR & FLASH_ACR_LATENCY) != flash_acr_latency)
		;

	RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_SW)) |
		RCC_CFGR_SW_PLL1 | // PLL as system clock
		0;

	while ((RCC->CFGR & RCC_CFGR_SWS) != (RCC_CFGR_SWS_0 * 3))
		;

	//while ((RCC->CR & (RCC_CR_D1CKRDY | RCC_CR_D2CKRDY)) != (RCC_CR_D1CKRDY | RCC_CR_D2CKRDY))
	//	;

	// RCC Domain 1 Kernel Clock Configuration Register
	// Set per_ck clock output
	RCC->D1CCIPR = (RCC->D1CCIPR & ~ (RCC_D1CCIPR_CKPERSEL)) |
		0 * RCC_D1CCIPR_CKPERSEL_0 |	// 00: hsi_ker_ck clock selected as per_ck clock (default after reset) - 64 MHz - used as PER_CK_FREQ
		0;

	// RCC Domain 1 Kernel Clock Configuration Register
	RCC->D1CCIPR = (RCC->D1CCIPR & ~ (RCC_D1CCIPR_SDMMCSEL)) |
#if WITHSDHCHW
		0 * RCC_D1CCIPR_SDMMCSEL |			// 0: pll1_q_ck clock is selected as kernel peripheral clock (default after reset)
#endif /* WITHSDHCHW */
		0;

	// RCC Domain 2 Kernel Clock Configuration Register
	RCC->D2CCIP1R = (RCC->D2CCIP1R & ~ (RCC_D2CCIP1R_SPI123SEL | RCC_D2CCIP1R_SPI45SEL)) |
		4 * RCC_D2CCIP1R_SPI123SEL_0 |		// per_ck
		3 * RCC_D2CCIP1R_SPI45SEL_0 |		// 011: hsi_ker_ck clock is selected as kernel clock
		0;
	// RCC Domain 2 Kernel Clock Configuration Register
	RCC->D2CCIP2R = (RCC->D2CCIP2R & ~ (
					RCC_D2CCIP2R_USART16SEL | RCC_D2CCIP2R_USART28SEL | 
					RCC_D2CCIP2R_RNGSEL | RCC_D2CCIP2R_I2C123SEL)) |
		0 * RCC_D2CCIP2R_USART16SEL_0 |		// rcc_pclk2
		0 * RCC_D2CCIP2R_USART28SEL_0 |		// rcc_pclk1
		1 * RCC_D2CCIP2R_RNGSEL_0 |			// 01: pll1_q_ck clock is selected as kernel clock
		0 * RCC_D2CCIP2R_I2C123SEL_0 |		// rcc_pclk1
		0;

	// Выбор источника тактирования блока USB
	// RCC Domain 2 Kernel Clock Configuration Register
	// USBOTG 1 and 2 kernel clock source selection
	RCC->D2CCIP2R = (RCC->D2CCIP2R & ~ (RCC_D2CCIP2R_USBSEL)) |
		1 * RCC_D2CCIP2R_USBSEL_0 |			// pll1_q_ck
		//3 * RCC_D2CCIP2R_USBSEL_0 |			// hsi48_ck
		0;

	// RCC Domain 3 Kernel Clock Configuration Register
	RCC->D3CCIPR = (RCC->D3CCIPR & ~ (RCC_D3CCIPR_ADCSEL)) |
#if WITHCPUADCHW
		2 * RCC_D3CCIPR_ADCSEL_0 |		// 10: per_ck clock selected as kernel peripheral clock
#endif /* WITHCPUADCHW */
		0;

	if (0)
	{
		// Тестирование тактовой частоты - подача на сигнал MCO1
		arm_hardware_pioa_altfn50(1U << 8, AF_SYSTEM);			/* PA0 MCO1 */
		RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_MCO1 | RCC_CFGR_MCO1PRE)) |
			1 * RCC_CFGR_MCO1PRE_0 |	// divide to 1: bypass
			3 * RCC_CFGR_MCO1_0 |	// 011: PLL1 clock selected (pll1_q_ck)
			0;

		//for (;;)
		//	;

	}
}

// Настроить выход PLLQ на 48 МГц, подключить SDMMC и USB к нему.
// Настройка делителя делается при инициализации PLL, здесь измениь делитель не получается.
// Версия для STM32H7 возвращает текушее значение делитедя.
uint_fast32_t arm_hardware_stm32f7xx_pllq_initialize(void)
{
	const uint32_t stm32h7xx_pllq = ((RCC->PLL1DIVR & RCC_PLL1DIVR_Q1) >> RCC_PLL1DIVR_Q1_Pos) + 1;
	return stm32h7xx_pllq;
}

#if WITHUSESAIPLL

static void lowlevel_stm32h7xx_pllsai_initialize(void)
{

	/* для устройств на шине APB2 (up to 72 MHz) */
	auto uint_fast32_t 
	calcdivround_saifreq(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		//#error TODO: check freq at outputs vsync/hsync
		return (PLLSAI_FREQ + freq / 2) / freq;
	}

	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_saifreq(LTDC_DOTCLK), STM32F_LTDC_DIV_WIDTH, STM32F_LTDC_DIV_TAPS, & value, 0);
	ASSERT(value >= 2);
	debug_printf_P(PSTR("lowlevel_stm32h7xx_pllsai_initialize: value=%u, prei=%u\n"), value, prei);

	// Настройка PLLSAI
	// Частота сравнения та же самая, что и в основной PLL
	// RCC_PLLSAICFGR_PLLSAIQ используется, если для SAI используется отдельная PLL - эта.
	RCC->PLLSAICFGR = (RCC->PLLSAICFGR & ~ (RCC_PLLSAICFGR_PLLSAIN | /*RCC_PLLSAICFGR_PLLSAIQ | */ RCC_PLLSAICFGR_PLLSAIR)) |
		((SAIREF1_MUL << RCC_PLLSAICFGR_PLLSAIN_Pos) & RCC_PLLSAICFGR_PLLSAIN) |	// PLLI2SN bits = multiplier, freq=192..432 MHz, vale = 2..432
		((value << RCC_PLLSAICFGR_PLLSAIR_Pos) & RCC_PLLSAICFGR_PLLSAIR) |	// PLLI2SR bits - output divider, 2..7
		0;

	RCC->DCKCFGR = (RCC->DCKCFGR & ~ RCC_DCKCFGR_PLLSAIDIVR) |
		((prei << RCC_DCKCFGR_PLLSAIDIVR_Pos) & RCC_DCKCFGR_PLLSAIDIVR) |	// division factor for LCD_CLK. 2: PLLSAIDIVR = /8  3: PLLSAIDIVR = /16
		0;

	RCC->CR |= RCC_CR_PLLSAION;				// Включил PLL
	while ((RCC->CR & RCC_CR_PLLSAIRDY) == 0)	// пока заработает PLL
		;
}

#endif /* WITHUSESAIPLL */

	// MPU initialize
static void lowlevel_stm32h7xx_mpu_initialize(void)
{
	/* Disables the MPU */
	MPU->CTRL = (MPU->CTRL & ~ (MPU_CTRL_ENABLE_Msk)) |
		0 * MPU_CTRL_ENABLE_Msk |
		0;


 
#define INNER_NORMAL_WB_RWA_TYPE(x)   (( 0x04 << MPU_RASR_TEX_Pos ) | ( DISABLE  << MPU_RASR_C_Pos ) | ( ENABLE  << MPU_RASR_B_Pos )  | ( x << MPU_RASR_S_Pos )) 
#define INNER_NORMAL_WB_NWA_TYPE(x)   (( 0x04 << MPU_RASR_TEX_Pos ) | ( ENABLE  << MPU_RASR_C_Pos )  | ( ENABLE  << MPU_RASR_B_Pos )  | ( x << MPU_RASR_S_Pos )) 
#define STRONGLY_ORDERED_SHAREABLE_TYPE      (( 0x00 << MPU_RASR_TEX_Pos ) | ( DISABLE << MPU_RASR_C_Pos ) | ( DISABLE << MPU_RASR_B_Pos ))     // DO not care // 
#define SHAREABLE_DEVICE_TYPE                (( 0x00 << MPU_RASR_TEX_Pos ) | ( DISABLE << MPU_RASR_C_Pos ) | ( ENABLE  << MPU_RASR_B_Pos ))     // DO not care // 
 

	// SRAM
	/* Set the Region base address and region number */
	MPU->RBAR = D1_AXISRAM_BASE | MPU_RBAR_VALID_Msk | 0x00;
    MPU->RASR = 
		(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
		(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
		(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
		(0x01 << MPU_RASR_C_Pos)    |	// IsCacheable
	 	(0x01 << MPU_RASR_B_Pos)    |	// IsBufferable
		(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
		(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
		(0x12 << MPU_RASR_SIZE_Pos) |	// Size 512 kB
		(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
		0;
	// ITCM
	/* Set the Region base address and region number */
	MPU->RBAR = D1_ITCMRAM_BASE | MPU_RBAR_VALID_Msk | 0x01;
    MPU->RASR = 
		(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
		(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
		(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
		(0x01 << MPU_RASR_C_Pos)    |	// IsCacheable
	 	(0x01 << MPU_RASR_B_Pos)    |	// IsBufferable
		(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
		(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
		(0x0F << MPU_RASR_SIZE_Pos) |	// Size 64 kB
		(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
		0;
	// DTCM
	/* Set the Region base address and region number */
	MPU->RBAR = D1_DTCMRAM_BASE | MPU_RBAR_VALID_Msk | 0x02;
    MPU->RASR = 
		(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
		(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
		(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
		(0x01 << MPU_RASR_C_Pos)    |	// IsCacheable
	 	(0x01 << MPU_RASR_B_Pos)    |	// IsBufferable
		(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
		(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
		(0x10 << MPU_RASR_SIZE_Pos) |	// Size 128 kB
		(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
		0;
	// FLASH
	/* Set the Region base address and region number */
	MPU->RBAR = D1_AXIFLASH_BASE | MPU_RBAR_VALID_Msk | 0x03;
    MPU->RASR = 
		(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
		(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
		(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
		(0x01 << MPU_RASR_C_Pos)    |	// IsCacheable
	 	(0x01 << MPU_RASR_B_Pos)    |	// IsBufferable
		(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
		(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
		(0x14 << MPU_RASR_SIZE_Pos) |	// Size 2 MB
		(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
		0;
	// DEVICE
	/* Set the Region base address and region number */
	MPU->RBAR = PERIPH_BASE | MPU_RBAR_VALID_Msk | 0x04;
    MPU->RASR = 
		(0x00 << MPU_RASR_XN_Pos)   |	// DisableExec
		(0x03 << MPU_RASR_AP_Pos)   |	// AccessPermission
		(0x04 << MPU_RASR_TEX_Pos)  |	// TypeExtField
		(0x00 << MPU_RASR_C_Pos)    |	// IsCacheable
	 	(0x00 << MPU_RASR_B_Pos)    |	// IsBufferable
		(0x00 << MPU_RASR_S_Pos)    |	// IsShareable
		(0x00 << MPU_RASR_SRD_Pos)  |	// SubRegionDisable (8 bits mask)
		(0x1B << MPU_RASR_SIZE_Pos) |	// Size 256 MB
		(0x01 << MPU_RASR_ENABLE_Pos) |	// Enable
		0;
	/* Enables the MPU */
	MPU->CTRL = (MPU->CTRL & ~ (MPU_CTRL_ENABLE_Msk)) |
		1 * MPU_CTRL_ENABLE_Msk |
		0;
}

#endif /* CPUSTYLE_STM32H7XX */

#if CPUSTYLE_STM32F1XX

static void 
lowlevel_stm32f10x_pll_clock(void)
{
	#if WITHCPUXOSC
		// Внешний генератор
		// Enable HSI
		RCC->CR = (RCC->CR & ~ (RCC_CR_HSEON | RCC_CR_HSION | RCC_CR_HSEBYP)) | 
			RCC_CR_HSEON |
			RCC_CR_HSEBYP |
			0;
		while (!(RCC->CR & RCC_CR_HSERDY)) 
			;
	#elif WITHCPUXTAL
		// внешний кварцевый резонатор
		// Enable HSI
		RCC->CR = (RCC->CR & ~ (RCC_CR_HSEON | RCC_CR_HSION | RCC_CR_HSEBYP)) | RCC_CR_HSEON;
		while (!(RCC->CR & RCC_CR_HSERDY)) 
			;
	#else /* WITHCPUXTAL */
		// внутренний генератор
		// Enable HSI
		RCC->CR = (RCC->CR & ~ (RCC_CR_HSEON | RCC_CR_HSION | RCC_CR_HSEBYP)) | RCC_CR_HSION;
		while (!(RCC->CR & RCC_CR_HSIRDY)) 
			;
	#endif /* WITHCPUXTAL */

	// Enable Prefetch Buffer
	FLASH->ACR |= FLASH_ACR_PRFTBE;

#if (CPU_FREQ < 24000000ul)
	// Flash 0 wait state
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_0;
#elif (CPU_FREQ <= 48000000ul)
	// Flash 1 wait state
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_1;
#else
 	// Flash 2 wait state (if freq in 24..48 MHz range - 1WS.)
	#if CPUSTYLE_STM32F1XX
		FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_2;
	#else
		FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_1;
	#endif
#endif

	// PPRE2: APB high-speed prescaler (APB2)
	// PPRE1: APB low-speed prescaler (APB1) = PREDIV1 bits
	// HPRE: AHB prescaler

 	#if CPU_FREQ >= 48000000UL
	   // HCLK = SYSCLK, PCLK2 = HCLK, PCLK1 = HCLK/1
		RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_HPRE | RCC_CFGR_PPRE2 | RCC_CFGR_PPRE1)) |
				RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PPRE1_DIV2;
	#else
	   // HCLK = SYSCLK, PCLK2 = HCLK, PCLK1 = HCLK/1
		RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_HPRE | RCC_CFGR_PPRE2 | RCC_CFGR_PPRE1)) |
				RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PPRE1_DIV1;
	#endif

	// For STM32F30x:
	// PLLXTPRE: This bit is the same as the LSB of PREDIV in RCC_CFGR2
	// RCC_CFGR_PLLSRC_HSI_Div2
	#if WITHCPUXTAL || WITHCPUXOSC
		#if (REF1_DIV != 1) && (REF1_DIV != 2)
			#error REF1_DIV wrong value - 1 or 2 supported by CPU
		#endif
		RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_PLLSRC_Msk | RCC_CFGR_PLLXTPRE_Msk | RCC_CFGR_PLLMULL_Msk)) |
			(REF1_DIV == 2) * RCC_CFGR_PLLXTPRE | 
			RCC_CFGR_PLLSRC |
			(REF1_MUL - 2) * RCC_CFGR_PLLMULL_0 |
			0;
	#else /* WITHCPUXTAL */
		#if (REF1_DIV != 2)
			#error REF1_DIV wrong value - only 2 supported by CPU
		#endif
		RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_PLLSRC_Msk | RCC_CFGR_PLLXTPRE_Msk | RCC_CFGR_PLLMULL_Msk)) |
			0 | /* RCC_CFGR_PLLSRC_HSI_Div2 */
			(REF1_MUL - 2) * RCC_CFGR_PLLMULL_0 |
			0;
	#endif /* WITHCPUXTAL */


	// Enable PLL
	RCC->CR |= RCC_CR_PLLON;

	// Wait till PLL is ready
	while ((RCC->CR & RCC_CR_PLLRDY) == 0) 
		;

	// Select PLL as system clock source
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;

    // Wait till PLL is used as system clock source
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) 
		;

#if WITHCPUXTAL
	// HSI (8 MHz RC) off
	RCC->CR &= ~ RCC_CR_HSION;
#endif

#if 0
	// Тестирование тактовой частоты - подача на сигнал MCO
	//RCC->CFGR = (RCC->CFGR & ~ RCC_CFGR_MCO) | RCC_CFGR_MCO_SYSCLK; 
	RCC->CFGR = (RCC->CFGR & ~ RCC_CFGR_MCO) | RCC_CFGR_MCO_PLL;	// Смотрим PLL / 2
	//RCC->CFGR = (RCC->CFGR & ~ RCC_CFGR_MCO) | RCC_CFGR_MCO_HSI; 
	while ((RCC->CFGR & RCC_CFGR_MCOF) == 0) 
		;
	arm_hardware_pioa_altfn50(1U << 8, AF_SYSTEM);	// PA8, AF=0: MCO

#endif
}

#endif /* CPUSTYLE_STM32F1XX */

#if CPUSTYLE_STM32F30X

static void 
lowlevel_stm32f30x_pll_clock(void)
{
	#if WITHCPUXTAL
		// внешний кварцевый резонатор
		// Enable HSI
		RCC->CR = (RCC->CR & ~ (RCC_CR_HSEON | RCC_CR_HSION | RCC_CR_HSEBYP)) | RCC_CR_HSEON;
		while (!(RCC->CR & RCC_CR_HSERDY)) 
			;
	#else /* WITHCPUXTAL */
		// внутренний генератор
		// Enable HSI
		RCC->CR = (RCC->CR & ~ (RCC_CR_HSEON | RCC_CR_HSION | RCC_CR_HSEBYP)) | RCC_CR_HSION;
		while (!(RCC->CR & RCC_CR_HSIRDY)) 
			;
	#endif /* WITHCPUXTAL */

	// Enable Prefetch Buffer
	FLASH->ACR |= FLASH_ACR_PRFTBE;

#if (CPU_FREQ < 24000000ul)
	// Flash 0 wait state
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_0;
#elif (CPU_FREQ <= 48000000ul)
	// Flash 1 wait state
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_1;
#else
 	// Flash 2 wait state (if freq in 24..48 MHz range - 1WS.)
	#if CPUSTYLE_STM32F1XX
		FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_2;
	#else
		FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_1;
	#endif
#endif

	// PPRE2: APB high-speed prescaler (APB2)
	// PPRE1: APB low-speed prescaler (APB1) = PREDIV1 bits
	// HPRE: AHB prescaler

 	#if CPU_FREQ >= 48000000UL
	   // HCLK = SYSCLK, PCLK2 = HCLK, PCLK1 = HCLK/1
		RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_HPRE | RCC_CFGR_PPRE2 | RCC_CFGR_PPRE1)) |
				RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PPRE1_DIV2;
	#else
	   // HCLK = SYSCLK, PCLK2 = HCLK, PCLK1 = HCLK/1
		RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_HPRE | RCC_CFGR_PPRE2 | RCC_CFGR_PPRE1)) |
				RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE2_DIV1 | RCC_CFGR_PPRE1_DIV1;
	#endif

	// For STM32F30x:
	// PLLXTPRE: This bit is the same as the LSB of PREDIV in RCC_CFGR2
	// RCC_CFGR_PLLSRC_HSI_Div2
	#if WITHCPUXTAL
		#if (REF1_DIV != 1) && (REF1_DIV != 2)
			#error REF1_DIV wrong value - 1 or 2 supported by CPU
		#endif
		RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL)) |
			(REF1_DIV == 2) * RCC_CFGR_PLLXTPRE | 
			RCC_CFGR_PLLSRC |
			(REF1_MUL - 2) * RCC_CFGR_PLLMULL_0 |
			0;
	#else /* WITHCPUXTAL */
		#if (REF1_DIV != 2)
			#error REF1_DIV wrong value - only 2 supported by CPU
		#endif
		RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL)) |
			RCC_CFGR_PLLSRC_HSI_Div2 |
			(REF1_MUL - 2) * RCC_CFGR_PLLMULL_0 |
			0;
	#endif /* WITHCPUXTAL */


	// Enable PLL
	RCC->CR |= RCC_CR_PLLON;

	// Wait till PLL is ready
	while ((RCC->CR & RCC_CR_PLLRDY) == 0) 
		;

	// Select PLL as system clock source
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;

    // Wait till PLL is used as system clock source
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) 
		;

#if WITHCPUXTAL
	// HSI (8 MHz RC) off
	RCC->CR &= ~ RCC_CR_HSION;
#endif

#if 0
	// Тестирование тактовой частоты - подача на сигнал MCO
	//RCC->CFGR = (RCC->CFGR & ~ RCC_CFGR_MCO) | RCC_CFGR_MCO_SYSCLK; 
	RCC->CFGR = (RCC->CFGR & ~ RCC_CFGR_MCO) | RCC_CFGR_MCO_PLL;	// Смотрим PLL / 2
	//RCC->CFGR = (RCC->CFGR & ~ RCC_CFGR_MCO) | RCC_CFGR_MCO_HSI; 
	while ((RCC->CFGR & RCC_CFGR_MCOF) == 0) 
		;
	arm_hardware_pioa_altfn50(1U << 8, AF_SYSTEM);	// PA8, AF=0: MCO

#endif
}

#endif /* CPUSTYLE_STM32F30X */

#if CPUSTYLE_ATSAM4S

// Clock Source Selection
static void program_mckr_css(unsigned long cssvalue)
{
    PMC->PMC_MCKR = (PMC->PMC_MCKR & ~ PMC_MCKR_CSS_Msk) | (cssvalue & PMC_MCKR_CSS_Msk);
	// Wiat MCLK ready
    while ((PMC->PMC_SR & PMC_SR_MCKRDY) == 0)
		;
}
// CPU prescaler
static void program_mckr_pres(unsigned long presvalue)
{
    PMC->PMC_MCKR = (PMC->PMC_MCKR & ~ PMC_MCKR_PRES_Msk) | (presvalue & PMC_MCKR_PRES_Msk);
	// Wiat MCLK ready
    while ((PMC->PMC_SR & PMC_SR_MCKRDY) == 0)
		;
}

// If a new value for CSS field corresponds to PLL Clock,
static void program_mckr_switchtopll_a(void)
{
	program_mckr_pres(PMC_MCKR_PRES_CLK_2);	// with /2 divider
	program_mckr_css(PMC_MCKR_CSS_PLLA_CLK);
}

// If a new value for CSS field corresponds to Main Clock
static void program_mckr_switchtomain(void)
{
	program_mckr_css(PMC_MCKR_CSS_MAIN_CLK);
#ifdef PMC_MCKR_PRES_CLK_1
	program_mckr_pres(PMC_MCKR_PRES_CLK_1);	// w/o divider
#else
	program_mckr_pres(PMC_MCKR_PRES_CLK);	// w/o divider
#endif
}

static void program_use_xtal(
	int useXtalFlag	/* 0 - использование RC генератора, не-0 - использование кварцевого генератора */
	)
{
	// бит CKGR_MOR_MOSCSEL - источник MAINCK это кварцевый генератор
	const unsigned long mor = PMC->CKGR_MOR & ~ CKGR_MOR_KEY_Msk;
	if (((mor & CKGR_MOR_MOSCSEL) != 0) == (useXtalFlag != 0))
		return;		// переключение не требуется

	if (useXtalFlag != 0)
		PMC->CKGR_MOR = CKGR_MOR_KEY_PASSWD | (mor | CKGR_MOR_MOSCSEL);
	else
		PMC->CKGR_MOR = CKGR_MOR_KEY_PASSWD | (mor & ~ CKGR_MOR_MOSCSEL);

	// ожидание переключения кварцевого генератора
	while ((PMC->PMC_SR & PMC_SR_MOSCSELS) == 0)
		;
}

// Enable on-chip RC oscillator
static void program_enable_RC_12MHz(void)
{
#ifdef CKGR_MOR_MOSCRCF_12_MHz
    PMC->CKGR_MOR = (PMC->CKGR_MOR & ~ (CKGR_MOR_MOSCRCF_Msk | CKGR_MOR_KEY_Msk)) | // остальные биты не трогаем
		CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCRCF_12_MHz;
#else
    PMC->CKGR_MOR = (PMC->CKGR_MOR & ~ (CKGR_MOR_MOSCRCF_Msk | CKGR_MOR_KEY_Msk)) | // остальные биты не трогаем
		CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCRCEN | CKGR_MOR_MOSCRCF_12MHZ;
#endif
	// ожидание запуска RC генератора
    while ((PMC->PMC_SR & PMC_SR_MOSCRCS) == 0)	
		;
}

static void program_disable_rc(void)
{
	const unsigned long mor = PMC->CKGR_MOR & ~ (CKGR_MOR_KEY_Msk | CKGR_MOR_MOSCRCEN);
    PMC->CKGR_MOR = mor | CKGR_MOR_KEY_PASSWD;
}

static void program_disable_xtal(void)
{
	const unsigned long mor = PMC->CKGR_MOR & ~ (CKGR_MOR_KEY_Msk | CKGR_MOR_MOSCXTEN);
    PMC->CKGR_MOR = mor | CKGR_MOR_KEY_PASSWD;
}

// Enable high-frequency XTAL oscillator
static void program_enable_xtal(void)
{
	const unsigned long mor = PMC->CKGR_MOR & ~ (CKGR_MOR_KEY_Msk | CKGR_MOR_MOSCXTST_Msk);
	if ((mor & CKGR_MOR_MOSCXTEN) != 0)
		return;		// кварцевый генератор уже запущен

    PMC->CKGR_MOR = 
		mor |	// стврые значения битов
		CKGR_MOR_KEY_PASSWD | 
		CKGR_MOR_MOSCXTST(128) | 
		CKGR_MOR_MOSCXTEN;
	// ожидание запуска кварцевого генератора
    while ((PMC->PMC_SR & PMC_SR_MOSCXTS) == 0)
		;
}

static void program_enable_plla(unsigned pllmul, unsigned plldiv)
{
#ifdef CKGR_PLLAR_ONE
    /* Initialize PLLA */
    PMC->CKGR_PLLAR = 
		(CKGR_PLLAR_ONE |	// всегда должен быть установлен
		((pllmul - 1) << CKGR_PLLAR_MULA_Pos) | 
		(0x4 << CKGR_PLLAR_PLLACOUNT_Pos) | 
		(plldiv << CKGR_PLLAR_DIVA_Pos));
#else
    /* Initialize PLLA */
    PMC->CKGR_PLLAR = 
		(CKGR_PLLAR_STUCKTO1 |	// всегда должен быть установлен
		((pllmul - 1) << CKGR_PLLAR_MULA_Pos) | 
		(0x4 << CKGR_PLLAR_PLLACOUNT_Pos) | 
		(plldiv << CKGR_PLLAR_DIVA_Pos));
#endif
	// Ожидание запуска PLL A
    while (!(PMC->PMC_SR & PMC_SR_LOCKA))
		;
}

#if 0
// unused now.
static void program_enable_pllb(void)
{
	//unsigned timer = 0xffffff;
	//enum { osc_mul = 32, osc_div = 6 };	// 12 MHz / 6 * 32 = 64 MHz
	enum { osc_mul = 8, osc_div = 1 };	// 12 MHz / 1 * 8 = 96 MHz
	//enum { osc_mul = 32, osc_div = 3 };	// 12 MHz / 3 * 32 = 128 MHz

    /* Initialize PLLA */
    PMC->CKGR_PLLBR = 
		//CKGR_PLLBR_STUCKTO1 |	// всегда должен быть установлен
		((osc_mul - 1) << CKGR_PLLBR_MULB_Pos) | 
		(0x1 << CKGR_PLLBR_PLLBCOUNT_Pos) | 
		(osc_div << CKGR_PLLBR_DIVB_Pos);
    //timeout = 0;
    //while (!(PMC->PMC_SR & PMC_SR_LOCKA) && (timeout++ < CLOCK_TIMEOUT))
    while ((PMC->PMC_SR & PMC_SR_LOCKB) == 0)
		;
}
#endif


// If a new value for CSS field corresponds to Slow Clock,
static void program_mckr_switchtoslow(void)
{
	program_mckr_css(PMC_MCKR_CSS_SLOW_CLK);
#ifdef PMC_MCKR_PRES_CLK_1
	program_mckr_pres(PMC_MCKR_PRES_CLK_1);	// w/o divider
#else
	program_mckr_pres(PMC_MCKR_PRES_CLK);	// w/o divider
#endif
}

// В описании процессора упоминается о том, что модификацию EEFC_FMR
// нельзя выполняить кодом из FLASH ROM. Данная пфункция копируется в SRAM
// и работает оттуда
static void RAMFUNC_NONILINE lowlevel_sam4s_setws(unsigned fws)
{
	EFC0->EEFC_FMR = EEFC_FMR_FWS(fws);	// Flash Wait State 
	//EFC1->EEFC_FMR = EEFC_FMR_FWS(fws);	// Flash Wait State 
}

// Перенастройка на работу с внутренним RC генератором 12 МГц	
static void lowlevel_sam4s_init_clock_12_RC12(void)
{
	program_enable_RC_12MHz();
	program_use_xtal(0);			// use RC
	program_disable_xtal();
	program_mckr_switchtomain();
}

	
/* 
  инициализация внутреннего умножителя частоты.
*/
static void lowlevel_sam4s_init_pll_clock_RC12(unsigned pllmul, unsigned plldiv, unsigned fws)
{
	// Embedded Flash Wait State VDDCORE set at 1.80V
	// FWS field = 0: up to 22 MHz
	// FWS field = 1: up to 38 MHz
	// FWS field = 2: up to 64 MHz
	lowlevel_sam4s_setws(3);	// Flash Wait State 

	//program_mckr_switchtoslow();	// переключаем на внутренний генератор 32 кГц
	program_mckr_switchtomain();	// выключить ФАПЧ, если была включена
	program_enable_RC_12MHz();
	program_use_xtal(0);
	program_disable_xtal();
	program_enable_plla(pllmul, plldiv);
	//program_enable_pllb();
	program_mckr_switchtopll_a();

	lowlevel_sam4s_setws(fws);	// Flash Wait State 
}

/* 
  инициализация внутреннего умножителя частоты.
  Вход - 12 МГц, кварцевый резонатор
  внутренняя тактовая - 64 МГц,
  частота генератора - 12 МГц
  Частота сравнения PLL = 4 МГц
*/
static void 
lowlevel_sam4s_init_pll_clock_xtal(unsigned pllmul, unsigned plldiv, unsigned ws)
{
	// Embedded Flash Wait State VDDCORE set at 1.80V
	// FWS field = 0: up to 22 MHz
	// FWS field = 1: up to 38 MHz
	// FWS field = 2: up to 64 MHz
	lowlevel_sam4s_setws(3);	// Flash Wait State 

	//program_mckr_switchtoslow();	// переключаем на внутренний генератор 32 кГц
	program_mckr_switchtomain();	// выключить ФАПЧ, если была включена
	program_enable_xtal();
	program_use_xtal(1);
	program_disable_rc();
	program_enable_plla(pllmul, plldiv);
	//program_enable_pllb();
	program_mckr_switchtopll_a();

	lowlevel_sam4s_setws(ws);	// Flash Wait State 
}


#endif /* CPUSTYLE_ATSAM4S */

#if CPUSTYLE_STM32F0XX

// chip use internal 8 MHz RC ckock generator as main clock
static void 
lowlevel_stm32f0xx_hsi_clock(void)
{
	// внутренний генератор
	// Enable HSI
	RCC->CR = (RCC->CR & ~ (RCC_CR_HSEON | RCC_CR_HSION | RCC_CR_HSEBYP)) | RCC_CR_HSION;
	while (!(RCC->CR & RCC_CR_HSIRDY)) 
		;
	// Enable Prefetch Buffer
	FLASH->ACR |= FLASH_ACR_PRFTBE;

	// PPRE: APB high-speed prescaler (APB2) PRE[2:0] bits (APB prescaler) 
	// HPRE: AHB prescaler

   // HCLK = SYSCLK, PCLK2 = HCLK, PCLK1 = HCLK/1
	RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_HPRE | RCC_CFGR_PPRE)) |
			RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE_DIV1;

	//000: Zero wait state, if 0 < SYSCLK <= 24 MHz
	FLASH->ACR &= ~ FLASH_ACR_LATENCY;
}

static void 
lowlevel_stm32f0xx_pll_clock(void)
{
	#if WITHCPUXTAL
		// внешний кварцевый резонатор
		// Enable HSI
		RCC->CR = (RCC->CR & ~ (RCC_CR_HSEON | RCC_CR_HSION | RCC_CR_HSEBYP)) | RCC_CR_HSEON;
		while (!(RCC->CR & RCC_CR_HSERDY)) 
			;
	#else /* WITHCPUXTAL */
		// внутренний генератор
		// Enable HSI
		RCC->CR = (RCC->CR & ~ (RCC_CR_HSEON | RCC_CR_HSION | RCC_CR_HSEBYP)) | RCC_CR_HSION;
		while (!(RCC->CR & RCC_CR_HSIRDY)) 
			;
	#endif /* WITHCPUXTAL */

	// Enable Prefetch Buffer
	FLASH->ACR |= FLASH_ACR_PRFTBE;

#if (CPU_FREQ < 24000000ul)
	// Flash 0 wait state
 	#if CPUSTYLE_STM32F0XX
		//000: Zero wait state, if 0 < SYSCLK <= 24 MHz
		FLASH->ACR &= ~ FLASH_ACR_LATENCY;
	#else
	   FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_0;
	#endif
#else
	// Flash 1 wait state
	#if CPUSTYLE_STM32F0XX
		// 001: One wait state, if 24 MHz < SYSCLK <= 48 MHz
		FLASH->ACR |= FLASH_ACR_LATENCY;
	#else
		FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_1;
	#endif
#endif

	// PPRE: APB high-speed prescaler (APB2) PRE[2:0] bits (APB prescaler) 
	// HPRE: AHB prescaler

   // HCLK = SYSCLK, PCLK2 = HCLK, PCLK1 = HCLK/1
	RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_HPRE | RCC_CFGR_PPRE)) |
			RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE_DIV1;

	// For STM32F30x:
	// PLLXTPRE: This bit is the same as the LSB of PREDIV in RCC_CFGR2
	// RCC_CFGR_PLLSRC_HSI_Div2
	#if WITHCPUXTAL
		// PLLCLK = 8MHz / 2 * 12 = 48 MHz
		RCC->CFGR = ((RCC->CFGR & ~ (RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL)) |
			(REF1_MUL - 2) * RCC_CFGR_PLLMULL_0 | 
			RCC_CFGR_PLLXTPRE | 
			RCC_CFGR_PLLSRC |
			0);
	#else /* WITHCPUXTAL */
		// PLLCLK = 8MHz / 2 * 12 = 48 MHz
		RCC->CFGR = ((RCC->CFGR & ~ (RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL)) |
			(REF1_MUL - 2) * RCC_CFGR_PLLMULL_0 | 
			RCC_CFGR_PLLSRC_HSI_Div2 |
			0);
	#endif /* WITHCPUXTAL */


	// Enable PLL
	RCC->CR |= RCC_CR_PLLON;

	// Wait till PLL is ready
	while ((RCC->CR & RCC_CR_PLLRDY) == 0) 
		;

	// Select PLL as system clock source
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;

    // Wait till PLL is used as system clock source
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) 
		;

#if WITHCPUXTAL
	// HSI (8 MHz RC) off
	RCC->CR &= ~ RCC_CR_HSION;
#endif

#if 0
	// Тестирование тактовой частоты - подача на сигнал MCO
	//RCC->CFGR = (RCC->CFGR & ~ RCC_CFGR_MCO) | RCC_CFGR_MCO_SYSCLK; 
	RCC->CFGR = (RCC->CFGR & ~ RCC_CFGR_MCO) | RCC_CFGR_MCO_PLL;	// Смотрим PLL / 2
	//RCC->CFGR = (RCC->CFGR & ~ RCC_CFGR_MCO) | RCC_CFGR_MCO_HSI; 
	while ((RCC->CFGR & RCC_CFGR_MCOF) == 0) 
		;
	arm_hardware_pioa_altfn50(1U << 8, AF_SYSTEM);	// PA8, AF=0: MCO

#endif
}

#endif /* CPUSTYLE_STM32F0XX */

#if CPUSTYLE_STM32L0XX

// chip use internal 16 MHz RC ckock generator as main clock
static void 
lowlevel_stm32l0xx_hsi_clock(void)
{
	// внутренний генератор
	// Enable HSI
	RCC->CR = (RCC->CR & ~ (RCC_CR_HSEON | RCC_CR_HSION | RCC_CR_HSEBYP)) | RCC_CR_HSION;
	while ((RCC->CR & RCC_CR_HSIRDY) == 0) 
		;

	//RCC->CR = (RCC->CR & ~ (RCC_CR_HSIDIVEN));
	//while ((RCC->CR & RCC_CR_HSIDIVF) != 0) 
	//	;

	//RCC->CR = (RCC->CR | (RCC_CR_HSIDIVEN));
	//while ((RCC->CR & RCC_CR_HSIDIVF) == 0) 
	//	;

	// Enable Prefetch Buffer
	FLASH->ACR |= FLASH_ACR_PRFTEN;

	RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2)) |
			(0 * RCC_CFGR_PPRE2_0) |	// APB high-speed prescaler (APB2)
			(0 * RCC_CFGR_PPRE1_0) |	// APB low-speed prescaler (APB1)
			0;


	//000: Zero wait state, if 0 < SYSCLK <= 24 MHz
	FLASH->ACR &= ~ FLASH_ACR_LATENCY;

	// Select HSI as system clock source
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSI;

    // Wait till PLL is used as system clock source
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) 
		;

	RCC->CR &= ~ (RCC_CR_MSION);
	while ((RCC->CR & RCC_CR_MSIRDY) != 0) 
		;

}

void hardware_tim21_initialize(void)
{

	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_pclk2(1000), STM32F_TIM3_TIMER_WIDTH, STM32F_TIM3_TIMER_TAPS, & value, 1);

	// test: initialize TIM21, PA3 - output
	// TIM5 включён на выход TIM2
	RCC->APB2ENR |= RCC_APB2ENR_TIM21EN;   // подаем тактирование на TIM2 & TIM5
	__DSB();

	TIM21->CCMR1 = TIM_CCMR1_OC2M_0 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;	// Output Compare 3 Mode
	TIM21->CCER = TIM_CCER_CC2E;
	HARDWARE_ALTERNATE_INITIALIZE();	 /* PA3 - TIM21_CH2 output  */

	//TIM3->DIER = TIM_DIER_UIE;        	 // разрешить событие от таймера
	TIM21->PSC = 1;
	TIM21->CCR2 = (value / 2) & TIM_CCR2_CCR2;	// TIM4_CH3 - sound output
	TIM21->ARR = value - 1;
	TIM21->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости */
}


#endif /* CPUSTYLE_STM32L0XX */

#if 0 && CPUSTYLE_STM32L0XX

// NOTE: max CPU. AHB. APB1. APB2 frequenci is 32 MHz

static void 
lowlevel_stm32l0xx_pll_clock(void)
{
	#if WITHCPUXTAL
		// внешний кварцевый резонатор
		// Enable HSI
		RCC->CR = (RCC->CR & ~ (RCC_CR_HSEON | RCC_CR_HSION | RCC_CR_HSEBYP)) | RCC_CR_HSEON;
		while (!(RCC->CR & RCC_CR_HSERDY)) 
			;
	#else /* WITHCPUXTAL */
		// внутренний генератор
		// Enable HSI
		RCC->CR = (RCC->CR & ~ (RCC_CR_HSEON | RCC_CR_HSION | RCC_CR_HSEBYP)) | RCC_CR_HSION;
		while (!(RCC->CR & RCC_CR_HSIRDY)) 
			;
	#endif /* WITHCPUXTAL */

	// Enable Prefetch Buffer
	FLASH->ACR |= FLASH_ACR_PRFTEN;

#if (CPU_FREQ < 24000000ul)
	// Flash 0 wait state
 	#if CPUSTYLE_STM32F0XX
		//000: Zero wait state, if 0 < SYSCLK <= 24 MHz
		FLASH->ACR &= ~ FLASH_ACR_LATENCY;
	#else
	   FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_0;
	#endif
#else
	// Flash 1 wait state
	#if CPUSTYLE_STM32F0XX
		// 001: One wait state, if 24 MHz < SYSCLK <= 48 MHz
		FLASH->ACR |= FLASH_ACR_LATENCY;
	#else
		FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_1;
	#endif
#endif

	// PPRE: APB high-speed prescaler (APB2) PRE[2:0] bits (APB prescaler) 
	// HPRE: AHB prescaler

   // HCLK = SYSCLK, PCLK2 = HCLK, PCLK1 = HCLK/1
	RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2)) |
			(0 * RCC_CFGR_PPRE2_0) |	// APB high-speed prescaler (APB2) / 2
			(4 * RCC_CFGR_PPRE1_0) |	// APB low-speed prescaler (APB1)
			0;

	// For STM32F30x:
	// PLLXTPRE: This bit is the same as the LSB of PREDIV in RCC_CFGR2
	// RCC_CFGR_PLLSRC_HSI_Div2
	#if WITHCPUXTAL
		// PLLCLK = 8MHz / 2 * 12 = 48 MHz
		RCC->CFGR = ((RCC->CFGR & ~ (RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL)) |
			(REF1_MUL - 2) * RCC_CFGR_PLLMULL_0 | 
			RCC_CFGR_PLLXTPRE | 
			RCC_CFGR_PLLSRC |
			0);
	#else /* WITHCPUXTAL */
		// PLLCLK = 8MHz / 2 * 12 = 48 MHz
		RCC->CFGR = ((RCC->CFGR & ~ (RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL)) |
			(REF1_MUL - 2) * RCC_CFGR_PLLMULL_0 | 
			RCC_CFGR_PLLSRC_HSI_Div2 |
			0);
	#endif /* WITHCPUXTAL */


	// Enable PLL
	RCC->CR |= RCC_CR_PLLON;

	// Wait till PLL is ready
	while ((RCC->CR & RCC_CR_PLLRDY) == 0) 
		;

	// Select PLL as system clock source
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;

    // Wait till PLL is used as system clock source
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) 
		;

#if WITHCPUXTAL
	// HSI (8 MHz RC) off
	RCC->CR &= ~ RCC_CR_HSION;
#endif

#if 0
	// Тестирование тактовой частоты - подача на сигнал MCO
	//RCC->CFGR = (RCC->CFGR & ~ RCC_CFGR_MCO) | RCC_CFGR_MCO_SYSCLK; 
	RCC->CFGR = (RCC->CFGR & ~ RCC_CFGR_MCO) | RCC_CFGR_MCO_PLL;	// Смотрим PLL / 2
	//RCC->CFGR = (RCC->CFGR & ~ RCC_CFGR_MCO) | RCC_CFGR_MCO_HSI; 
	while ((RCC->CFGR & RCC_CFGR_MCOF) == 0) 
		;
	arm_hardware_pioa_altfn50(1U << 8, AF_SYSTEM);	// PA8, AF=0: MCO

#endif
}

#endif /* CPUSTYLE_STM32L0XX */

#if CPUSTYLE_R7S721

#if 1

void UndefHandler(void)
{
	debug_printf_P(PSTR("UndefHandler trapped.\n"));
	for (;;)
		;
}
void SWIHandler(void)
{
	debug_printf_P(PSTR("SWIHandler trapped.\n"));
	for (;;)
		;
}
void PAbortHandler(void)
{
	debug_printf_P(PSTR("PAbortHandler trapped.\n"));
	for (;;)
		;
}
void DAbortHandler(void)
{
	debug_printf_P(PSTR("DAbortHandler trapped.\n"));
	for (;;)
		;
}
void FIQHandler(void)
{
	debug_printf_P(PSTR("FIQHandler trapped.\n"));
	for (;;)
		;
}
/*
void IRQHandler(void)
{
	debug_printf_P(PSTR("IRQHandler trapped.\n"));
	for (;;)
		;
}
*/
#endif

#if 0
 
/** \brief  Set CSSELR
 */
__STATIC_FORCEINLINE void __set_CSSELR(uint32_t value)
{
//  __ASM volatile("MCR p15, 2, %0, c0, c0, 0" : : "r"(value) : "memory");
  __set_CP(15, 2, value, 0, 0, 0);
}

/** \brief Get MVBAR
This function returns the value of the Monitor Vector Base Address Register.

\return               Monitor Vector Base Address Register


*/
 __STATIC_FORCEINLINE uint32_t __get_MVBAR(void)
 {
 uint32_t result;
 __get_CP(15, 0, result, 12, 0, 1);
 return result;
 }

/** \brief Set MVBAR
This function assigns the given value to the Monitor Vector Base Address Register.

\param [in]    mvbar  Monitor Vector Base Address Register value to set


*/
 __STATIC_FORCEINLINE void __set_MVBAR(uint32_t vbar)
 {
 __set_CP(15, 0, vbar, 12, 0, 1);
 }

/** \brief Get VBAR
This function returns the value of the Vector Base Address Register.

\return               Vector Base Address Register


*/
 __STATIC_FORCEINLINE uint32_t __get_VBAR(void)
 {
 uint32_t result;
 __get_CP(15, 0, result, 12, 0, 0);
 return result;
 }

/** \brief Set VBAR
This function assigns the given value to the Vector Base Address Register.

\param [in]    vbar  Vector Base Address Register value to set


*/
 __STATIC_FORCEINLINE void __set_VBAR(uint32_t vbar)
 {
 __set_CP(15, 0, vbar, 12, 0, 0);
 }

#endif

static void arm_hardware_VFPEnable(void)
{
	const uint32_t VFPEnable = 0x40000000uL;

	__asm volatile( "FMXR 	FPEXC, %0" :: "r" (VFPEnable) );
}


#define CPACC_FULL(n)		(3uL << ((n) * 2))
#define CPACC_SVC(n)		(1uL << ((n) * 2))
#define CPACC_DISABLE(n)	(0uL << ((n) * 2))


static void vfp_access_enable(void)
{
	unsigned int access = __get_CPACR();

	/*
	 * Enable full access to VFP (cp10 and cp11)
	 */
	__set_CPACR(access | CPACC_FULL(10) | CPACC_FULL(11));
}

/*******************************************************************************
* Function Name: CPG_Init
* Description  : Executes initial setting for the CPG.
*              : In the sample code, the internal clock ratio is set to be 
*              : I:G:B:P1:P0 = 30:20:10:5:5/2 in the state that the 
*              : clock mode is 0. The frequency is set to be as below when the
*              : input clock is 13.33MHz.
*              : CPU clock (I clock)              : 400MHz
*              : Image processing clock (G clock) : 266.67MHz
*              : Internal bus clock (B clock)     : 133.33MHz
*              : Peripheral clock1 (P1 clock)     : 66.67MHz
*              : Peripheral clock0 (P0 clock)     : 33.33MHz
*              : Sets the data-retention RAM area (H'2000 0000 to H'2001 FFFF)
*              : to be enabled for writing.
* Arguments    : none
* Return Value : none
*******************************************************************************/
static 
FLASHMEMINITFUNC
void CPG_Init(void)
{
    /* standby_mode_en bit of Power Control Register setting */
    *(volatile uint32_t *)(0x3fffff80) = 0x00000001;
    (void) *(volatile uint32_t *)(0x3fffff80);

    /* ==== CPG Settings ==== */

    /* PLL(x30), I:G:B:P1:P0 = 30:20:10:5:5/2 */
    //CPG.FRQCR  = 0x1035u;
    CPG.FRQCR  = 0x3035u;	// CKIO pin = hi-z

    /* CKIO:Output at time usually output     *
     * when bus right is opened output at     *
     * standby "L"                            *
	 * Clockin  = 13.33MHz, CKIO = 66.67MHz,  *
	 * I  Clock = 400.00MHz,                  *
	 * G  Clock = 266.67MHz,                  *
	 * B  Clock = 133.33MHz,                  *
	 * P1 Clock =  66.67MHz,                  *
	 * P0 Clock =  33.33MHz                   */

    /* CKIO:Output at time usually output     *
     * when bus right is opened output at     *
     * standby "L"                            *
	 * Clockin  = 12.00MHz, CKIO = 60.0MHz,  *
	 * I  Clock = 360.00MHz,                  *
	 * G  Clock = 240.00MHz,                  *
	 * B  Clock = 120.00MHz,                  *
	 * P1 Clock =  60.00MHz,                  *
	 * P0 Clock =  30.00MHz                   */

#if ((TARGET_RZA1 == TARGET_RZA1H) || (TARGET_RZA1 == TARGET_RZA1M))
    CPG.FRQCR2 = 0x0001u;
#endif
}

#endif /* CPUSTYLE_R7S721 */

#if CPUSTYLE_ARM_CM7

// Сейчас в эту память будем читать по DMA
// Убрать копию этой области из кэша
// Используется только в startup
void arm_hardware_invalidate(uintptr_t base, size_t size)
{
	//ASSERT((base % 32) == 0);		// при работе с BACKUP SRAM невыровненно
	SCB_InvalidateDCache_by_Addr((void *) base, size);	// DCIMVAC register used.
}

// Сейчас эта память будет записываться по DMA куда-то
// Записать содержимое кэша данных в память
void arm_hardware_flush(uintptr_t base, size_t size)
{
	//ASSERT((base % 32) == 0);		// при работе с BACKUP SRAM невыровненно
	SCB_CleanDCache_by_Addr((void *) base, size);	// DCCMVAC register used.
}

// Записать содержимое кэша данных в память
// применяетмся после начальной инициализации среды выполнния
void arm_hardware_flush_all(void)
{
	SCB_CleanDCache();	// DCCMVAC register used.
}

// Сейчас эта память будет записываться по DMA куда-то. Потом содержимое не требуется
// Записать содержимое кэша данных в память
// Убрать копию этой области из кэша
void arm_hardware_flush_invalidate(uintptr_t base, size_t size)
{
	//ASSERT((base % 32) == 0);		// при работе с BACKUP SRAM невыровненно
	SCB_CleanInvalidateDCache_by_Addr((void *) base, size);	// DCCIMVAC register used.
}

#elif CPUSTYLE_ARM_CA9

static unsigned long DCACHEROWSIZE; // 32
static unsigned long ICACHEROWSIZE; // 32

#define MK_MVA(addr) ((uintptr_t) (addr) & ~ (uintptr_t) (DCACHEROWSIZE - 1))

// Сейчас в эту память будем читать по DMA
// Используется только в startup
void arm_hardware_invalidate(uintptr_t base, size_t size)
{
	unsigned long len = (size + (DCACHEROWSIZE - 1)) / DCACHEROWSIZE + (((unsigned long) base & (DCACHEROWSIZE - 1)) != 0);
	while (len --)
	{
		uintptr_t mva = MK_MVA(base);
		L1C_InvalidateDCacheMVA((void *) mva);	// очистить кэш
		base += DCACHEROWSIZE;
	}
}


/* считать конфигурационные параметры data cache */
static void ca9_cache_setup(void)
{
	uint32_t ccsidr0 [8];	// data cache parameters
	uint32_t ccsidr1 [8];	// instruction cache parameters

	uint_fast32_t leveli;
	for (leveli = 0; leveli <= ARM_CA9_CACHELEVELMAX; ++ leveli)
	{
		__set_CSSELR(leveli * 2 + 0);	// data cache select
		ccsidr0 [leveli] = __get_CCSIDR();

		//const uint32_t assoc0 = (ccsidr0 >> 3) & 0x3FF;
		//const int passoc0 = ilog2(assoc0);
		//const uint32_t maxsets0 = (ccsidr0 >> 13) & 0x7FFF;

		__set_CSSELR(leveli * 2 + 1);	// instruction cache select
		ccsidr1 [leveli] = __get_CCSIDR();

		//const uint32_t assoc1 = (ccsidr1 >> 3) & 0x3FF;
		//const int passoc1 = ilog2(assoc1);
		//const uint32_t maxsets1 = (ccsidr1 >> 13) & 0x7FFF;
	}

	// Установка размера строки кэша
	DCACHEROWSIZE = 4uL << (((ccsidr0 [0] >> 0) & 0x07) + 2);
	ICACHEROWSIZE = 4uL << (((ccsidr1 [0] >> 0) & 0x07) + 2);
}

// используется в startup
static void 
arm_hardware_invalidate_all(void)
{
	L1C_InvalidateDCacheAll();
	L1C_InvalidateICacheAll();
	L1C_InvalidateBTAC();
}

// Сейчас эта память будет записываться по DMA куда-то
void arm_hardware_flush(uintptr_t base, size_t size)
{
	unsigned long len = (size + (DCACHEROWSIZE - 1)) / DCACHEROWSIZE + (((unsigned long) base & (DCACHEROWSIZE - 1)) != 0);
	while (len --)
	{
		uintptr_t mva = MK_MVA(base);
		L1C_CleanDCacheMVA((void *) mva);		// записать буфер, кэш продолжает хранить
		base += DCACHEROWSIZE;
	}
}


// Записать содержимое кэша данных в память
// применяетмся после начальной инициализации среды выполнния
void arm_hardware_flush_all(void)
{
	L1C_CleanDCacheAll();
}

// Сейчас эта память будет записываться по DMA куда-то. Потом содержимое не требуется
void arm_hardware_flush_invalidate(uintptr_t base, size_t size)
{
	unsigned long len = (size + (DCACHEROWSIZE - 1)) / DCACHEROWSIZE + (((unsigned long) base & (DCACHEROWSIZE - 1)) != 0);
	while (len --)
	{
		uintptr_t mva = MK_MVA(base);
		L1C_CleanInvalidateDCacheMVA((void *) mva);	// записать буфер, очистить кэш
		base += DCACHEROWSIZE;
	}
}

#else

// Заглушки
// Сейчас в эту память будем читать по DMA
// Используется только в startup
void arm_hardware_invalidate(uintptr_t base, size_t size)
{
}

// Сейчас эта память будет записываться по DMA куда-то
void arm_hardware_flush(uintptr_t base, size_t size)
{
}

// Записать содержимое кэша данных в память
// применяетмся после начальной инициализации среды выполнния
void arm_hardware_flush_all(void)
{
}

// Сейчас эта память будет записываться по DMA куда-то. Потом содержимое не требуется
void arm_hardware_flush_invalidate(uintptr_t base, size_t size)
{
}

#endif /* CPUSTYLE_ARM_CM7 */


// получение из аппаратного счетчика монотонно увеличивающегося кода
// see arm_cpu_initialize() in hardware.c
uint_fast32_t cpu_getdebugticks(void)
{
#if CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7
	return DWT->CYCCNT;
#elif CPUSTYLE_ARM_CA9
	{
		uint32_t result;
		// Read CCNT Register
		//asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n": "=r"(value));  
		__get_CP(15, 0, result, 9, 13, 0);
		return(result);
	}
#else
	#warning Wromg CPUSTYLE_xxx - cpu_getdebugticks not work
	return 0;
#endif
}

// call after __preinit_array_xxx and before __init_array_xxx passing
void 
_init(void) 
{
}


/* функция вызывается из start-up до копирования в SRAM всех "быстрых" функций и до инициализации переменных
*/
// watchdog disable, clock initialize, cache enable
void 
FLASHMEMINITFUNC
arm_cpu_initialize(void)
{
#if CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7

	#if WITHDEBUG && WITHINTEGRATEDDSP && CPUSTYLE_ARM_CM7
		// Поддержка для функций диагностики быстродействия BEGINx_STAMP/ENDx_STAMP - audio.c
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->LAR = 0xC5ACCE55;	// Key value for unlock
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
		DWT->LAR = 0x00000000;	// Key value for lock
	#endif /* WITHDEBUG && WITHINTEGRATEDDSP */

	#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)

		/* FPU enable on Cortex M4F */
		SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2));  /* set CP10 and CP11 Full Access */

		#if 0
			/* Lazy stacking enabled, automatic state saving enabled is a default state */
			/* http://infocenter.arm.com/help/topic/com.arm.doc.dai0298a/DAI0298A_cortex_m4f_lazy_stacking_and_context_switching.pdf */
			__set_FPSCR(			/* Floating-Point Context Control Register */
				(__get_FPSCR() & ~ (FPU_FPCCR_LSPEN_Msk)) | /* disable Lazy stacking feature */
				FPU_FPCCR_ASPEN_Msk | 
				0);
		#endif

	#endif
	#ifdef UNALIGNED_SUPPORT_DISABLE
		SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
	#endif
#endif /* CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7 */

#if CPUSTYLE_ARM_CA9
	#if WITHDEBUG
	{
		// Поддержка для функций диагностики быстродействия BEGINx_STAMP/ENDx_STAMP - audio.c
		// From https://stackoverflow.com/questions/3247373/how-to-measure-program-execution-time-in-arm-cortex-a8-processor

		enum { do_reset = 0, enable_divider = 0 };
		// in general enable all counters (including cycle counter)
		int32_t value = 1;

		// peform reset:  
		if (do_reset)
		{
			value |= 2;     // reset all counters to zero.
			value |= 4;     // reset cycle counter to zero.
		} 

		if (enable_divider)
			value |= 8;     // enable "by 64" divider for CCNT.

		value |= 16;

		// program the performance-counter control-register:
		//asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(value));  
		__set_CP(15, 0, value, 9, 12, 0);

		// enable all counters:  
		//asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x8000000f));  
		__set_CP(15, 0, 0x8000000f, 9, 12, 1);

		// clear overflows:
		//asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000000f));
		__set_CP(15, 0, 0x8000000f, 9, 12, 3);
	}
	#endif /* WITHDEBUG */
#endif /* CPUSTYLE_ARM_CA9 */

#if CPUSTYLE_STM32F1XX

	lowlevel_stm32f10x_pll_clock();
	if (1)
	{
		// PC13, PC14 and PC15 as the common IO:
		RCC->APB1ENR |=  RCC_APB1ENR_BKPEN;     // включить тактирование Backup interface
		__DSB();

		PWR->CR |= PWR_CR_DBP; // cancel the backup area write protection  
		//RCC->BDCR &= ~ RCC_BDCR_LSEON; // close external low-speed oscillator, PC14, PC15 as ordinary IO  
		BKP->CR &= ~ BKP_CR_TPE; // TAMPER pin; intrusion detection (PC13) used as a universal IO port  
		PWR->CR &= ~ PWR_CR_DBP; // backup area write protection </span> 

		RCC->APB1ENR &=  ~ RCC_APB1ENR_BKPEN;     // выключить тактирование Backup interface
		__DSB();
	}

#elif CPUSTYLE_STM32F4XX

	lowlevel_stm32f4xx_pll_initialize();
	lowlevel_stm32f4xx_MCOx_test();

#elif CPUSTYLE_STM32H7XX

	lowlevel_stm32h7xx_pll_initialize();
	//lowlevel_stm32h7xx_mpu_initialize();

	/* AXI SRAM Slave */
	//AXI_TARG7_FN_MOD |= READ_ISS_OVERRIDE;
	*((__IO uint32_t*) 0x51008108) = 0x1; //Change  the switch matrix read issuing capability to 1 (Errata BUG fix)
	/* Change  the switch matrix read issuing capability to 1 for the AXI SRAM target (Target 7) */
	//*((__IO uint32_t*)0x51008108) = 0x000000001;

	RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM1EN;
	(void) RCC->AHB2ENR;
	RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM2EN;
	(void) RCC->AHB2ENR;
	RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM3EN;
	(void) RCC->AHB2ENR;

	RCC->AHB4ENR |= RCC_AHB4ENR_D3SRAM1EN;
	(void) RCC->AHB4ENR;
	
	SCB_InvalidateICache();
	SCB_EnableICache();

	SCB_InvalidateDCache();
	SCB_EnableDCache();

#elif CPUSTYLE_STM32F7XX

	lowlevel_stm32f7xx_pll_initialize();

	SCB_InvalidateICache();
	SCB_EnableICache();

	SCB_InvalidateDCache();
	SCB_EnableDCache();

#elif CPUSTYLE_STM32F30X

	lowlevel_stm32f30x_pll_clock();

#elif CPUSTYLE_STM32F0XX

	lowlevel_stm32f0xx_pll_clock();
	//lowlevel_stm32f0xx_hsi_clock();

#elif CPUSTYLE_STM32L0XX

// Плата с процессором STM32L051K6T (TQFP-32)

#if ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED
	// power on bit
	{
		enum { WORKMASK = 1U << 11 };	/* PA11 */
		arm_hardware_pioa_outputs(WORKMASK, WORKMASK * (1 != 0));

	}
#endif /* ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED */
	//lowlevel_stm32l0xx_pll_clock();
	lowlevel_stm32l0xx_hsi_clock();

#elif CPUSTYLE_ATSAM3S

	// Disable Watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;
	lowlevel_sam3s_init_clock_12_RC12();	// программирует на работу от 12 МГц RC - для ускорения работы.
	// инициализация PLL и программирование wait states (только из SRAM) делается позже.

#elif CPUSTYLE_ATSAM4S

	// Disable Watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;
	lowlevel_sam4s_init_clock_12_RC12();	// программирует на работу от 12 МГц RC - для ускорения работы.
	// инициализация PLL и программирование wait states (только из SRAM) делается позже.

#elif CPUSTYLE_AT91SAM7S

	// Disable Watchdog
	AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;

	// Enable NRST input. Требуется для удобства при отладке.
	AT91C_BASE_RSTC->RSTC_RMR = AT91C_RSTC_URSTEN | (AT91C_RSTC_KEY & (0xA5UL << 24));

	// init clock sources and memory timings
	// use one of alternatives
	//

	#if CPU_FREQ == 48000000UL
		lowlevel_init_pll_clock_xtal(8, 1);
	#elif CPU_FREQ == ((18432000UL * 73) / 14 / 2)
		lowlevel_init_pll_clock_xtal(73, 14);
	#elif CPU_FREQ == 12000000UL
		lowlevel_init_pll_clock_from_xtal();
	#else
		#error Unsupported CPU_FREQ value
	#endif

#elif CPUSTYLE_AT91SAM9XE

	// Disable Watchdog
	AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;

	// Enable NRST input. Требуется для удобства при отладке.
	AT91C_BASE_RSTC->RSTC_RMR = AT91C_RSTC_URSTEN | (AT91C_RSTC_KEY & (0xA5UL << 24));

	at91sam9x_clocks(96, 9);
	//at91sam9x_clocks_48x4();
	//cp15_enable_i_cache();
	__set_SCTLR(__get_SCTLR() | SCTLR_I_Msk);

#elif CPUSTYLE_R7S721

	// Программа исполняется из SERIAL FLASH - переключать режимы пока нельзя.
	//while ((SPIBSC0.CMNSR & (1u << 0)) == 0)	// TEND bit
	//	;
	//SPIBSC0.SSLDR = 0x00;
	//SPIBSC0.SPBCR = 0x200;
	//SPIBSC0.DRCR = 0x0100;

	CPG_Init();


    /* ----  Writing to On-Chip Data-Retention RAM is enabled. ---- */
	if (1)
	{
		// Нельзя отключить - т.к. r7s721_ttb_map работает со страницами по 1 мегабайту
		CPG.SYSCR3 = (CPG_SYSCR3_RRAMWE3 | CPG_SYSCR3_RRAMWE2 | CPG_SYSCR3_RRAMWE1 | CPG_SYSCR3_RRAMWE0);
		(void) CPG.SYSCR3;
	}


	* (volatile unsigned long *) 0x3FFFFF80 &= ~ 0x01uL;	// L2CTL Clear standby_mode_en bit of the power control register in the PL310
	* (volatile unsigned long *) 0x3FFFF100 |= 0x01uL;		// REG1 Set bit L2 Cache enable

	//INB.RMPR &= ~ (1U << 1);		// 0: Address remapping is enabled 0x20000000 visible at 0x00000000.
	//(void) INB.RMPR;

    /* ==== Initial setting of the level 1 cache ==== */
	//__set_SCTLR(0);		
    //L1CacheInit();

	L1C_EnableCaches();
	L1C_EnableBTAC();
	__set_ACTLR(__get_ACTLR() | ACTLR_L1PE_Msk);	// Enable Dside prefetch

	/* далее будет выполняться копирование data и инициализация bss - для нормальной работы RESET требуется без DATA CACHE */

#else
	//#error Undefined CPUSTYLE_XXX

#endif
}

#if CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7 || CPUSTYLE_ARM_CM0

uint32_t gARM_OVERREALTIME_PRIORITY = 0;
uint32_t gARM_REALTIME_PRIORITY = 0;
uint32_t gARM_SYSTEM_PRIORITY = 0;
uint32_t gARM_BASEPRI_ONLY_REALTIME = 0;
uint32_t gARM_BASEPRI_ALL_ENABLED = 0;

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
	gARM_BASEPRI_ALL_ENABLED = 0;

	// Назначить таймеру приоритет, равный всем остальным прерываниям
	NVIC_SetPriority(SysTick_IRQn, ARM_SYSTEM_PRIORITY);
}

#endif /* CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7 */

#if CPUSTYLE_R7S721

#include "hardware.h"
#include "formats.h"
//#include "hardware_r7s721.h"

#define IRQ_GIC_LINE_COUNT           (587)   /* The number of interrupt sources */

//#define INTC_LEVEL_SENSITIVE    (0)     /* Level sense  */
//#define INTC_EDGE_TRIGGER       (1)     /* Edge trigger */

/* ==== Interrupt detection ==== */

static void Userdef_INTC_Dummy_Interrupt(void)
{
	debug_printf_P(PSTR("Userdef_INTC_Dummy_Interrupt()\n"));
	for (;;)
		;
}

/* Interrupt handler table */
static void (* intc_func_table [IRQ_GIC_LINE_COUNT])(void);

/* Вызывается из crt_r7s721.s со сброшенным флагом прерываний */
void IRQHandlerSafe(void)
{
	/* const uint32_t icchpir = */ (void) GICC_HPPIR;	/* GICC_HPPIR */
	const uint32_t icciar = GICC_IAR;				/* GICC_IAR */
	const uint16_t int_id = icciar & INTC_ICCIAR_ACKINTID;

	// See R01UH0437EJ0200 Rev.2.00 7.8.3 Reading Interrupt ID Values from Interrupt Acknowledge Register (ICCIAR)
	// IHI0048B_b_gic_architecture_specification.pdf
	// See ARM IHI 0048B.b 3.4.2 Special interrupt numbers when a GIC supports interrupt grouping

	if (int_id >= 1020)
	{
		GICD_IPRIORITYRn(0) = GICD_IPRIORITYRn(0);
	}
	else if (int_id != 0 || (INTC.ICDABR0 & 0x0001) != 0)
	{
	#if WITHNESTEDINTERRUPTS

		__enable_irq();						/* modify I bit in CPSR */
		(* intc_func_table [int_id])();	    /* Call interrupt handler */
		__disable_irq();					/* modify I bit in CPSR */

	#else /* WITHNESTEDINTERRUPTS */

		(* intc_func_table [int_id])();	    /* Call interrupt handler */

	#endif /* WITHNESTEDINTERRUPTS */
		INTC.ICCEOIR = int_id;				/* GICC_EOIR */
	}
	else
	{
		GICD_IPRIORITYRn(0) = GICD_IPRIORITYRn(0);
	}
}

/******************************************************************************
* Function Name: r7s721_intc_initialize
* Description  : Executes initial setting for the INTC.
*              : The interrupt mask level is set to 31 to receive interrupts 
*              : with the interrupt priority level 0 to 30.
* Arguments    : none
* Return Value : none
******************************************************************************/
static void r7s721_intc_initialize(void)
{

	/* ==== Total number of registers ==== */
	enum { INTC_ICDISR_REG_TOTAL   = (IRQ_GIC_LINE_COUNT + 31) / 32 };	// 19 == INTC_ICDISR0_COUNT
	enum { INTC_ICDICFR_REG_TOTAL  = (IRQ_GIC_LINE_COUNT + 15) / 16 };	// 37 == INTC_ICDICFR0_COUNT
	enum { INTC_ICDIPR_REG_TOTAL   = (IRQ_GIC_LINE_COUNT + 3) /  4 };	// 147 == INTC_ICDIPR0_COUNT
	enum { INTC_ICDIPTR_REG_TOTAL  = (IRQ_GIC_LINE_COUNT + 3) /  4 };	// 147 == INTC_ICDIPTR0_COUNT
	//enum { INTC_ICDISER_REG_TOTAL  = (IRQ_GIC_LINE_COUNT + 31) / 32 };	// 19 == INTC_ICDISER0_COUNT
	enum { INTC_ICDICER_REG_TOTAL  = (IRQ_GIC_LINE_COUNT + 31) / 32 };	// 19 == INTC_ICDICER0_COUNT

	/* Initial value table of Interrupt Configuration Registers */
	// Table 4-19 GICD_ICFGR Int_config[0] encoding in some early GIC implementations
	// каждая пара бит кодирует:

	// [0] == 0: Corresponding interrupt is handled using the N-N model.
	// [0] == 1: Corresponding interrupt is handled using the 1-N model.
	// [1] == 0: Corresponding interrupt is level-sensitive.
	// [1] == 1: Corresponding interrupt is edge-triggered.
	static const uint32_t intc_icdicfrn_table [INTC_ICDICFR_REG_TOTAL] =
	{                          /*           Interrupt ID */
		0xAAAAAAAA,            /* ICDICFR0  :  15 to   0 */
		0x00000055,            /* ICDICFR1  :  19 to  16 */
		0xFFFD5555,            /* ICDICFR2  :  47 to  32 */
		0x555FFFFF,            /* ICDICFR3  :  63 to  48 */
		0x55555555,            /* ICDICFR4  :  79 to  64 */
		0x55555555,            /* ICDICFR5  :  95 to  80 */
		0x55555555,            /* ICDICFR6  : 111 to  96 */
		0x55555555,            /* ICDICFR7  : 127 to 112 */
		0x5555F555,            /* ICDICFR8  : 143 to 128 */
		0x55555555,            /* ICDICFR9  : 159 to 144 */
		0x55555555,            /* ICDICFR10 : 175 to 160 */
		0xF5555555,            /* ICDICFR11 : 191 to 176 */
		0xF555F555,            /* ICDICFR12 : 207 to 192 */
		0x5555F555,            /* ICDICFR13 : 223 to 208 */
		0x55555555,            /* ICDICFR14 : 239 to 224 */
		0x55555555,            /* ICDICFR15 : 255 to 240 */
		0x55555555,            /* ICDICFR16 : 271 to 256 */
		0xFD555555,            /* ICDICFR17 : 287 to 272 */
		0x55555557,            /* ICDICFR18 : 303 to 288 */
		0x55555555,            /* ICDICFR19 : 319 to 304 */
		0x55555555,            /* ICDICFR20 : 335 to 320 */
		0x5F555555,            /* ICDICFR21 : 351 to 336 */
		0xFD55555F,            /* ICDICFR22 : 367 to 352 */
		0x55555557,            /* ICDICFR23 : 383 to 368 */
		0x55555555,            /* ICDICFR24 : 399 to 384 */
		0x55555555,            /* ICDICFR25 : 415 to 400 */
		0x55555555,            /* ICDICFR26 : 431 to 416 */
		0x55555555,            /* ICDICFR27 : 447 to 432 */
		0x55555555,            /* ICDICFR28 : 463 to 448 */
		0x55555555,            /* ICDICFR29 : 479 to 464 */
		0x55555555,            /* ICDICFR30 : 495 to 480 */
		0x55555555,            /* ICDICFR31 : 511 to 496 */
		0x55555555,            /* ICDICFR32 : 527 to 512 */
		0x55555555,            /* ICDICFR33 : 543 to 528 */
		0x55555555,            /* ICDICFR34 : 559 to 544 */
		0x55555555,            /* ICDICFR35 : 575 to 560 */
		0x00155555             /* ICDICFR36 : 586 to 576 */
	};

    uint16_t offset;
    volatile uint32_t * addr;

	//GIC_Enable();	// инициализирует не совсем так как надо для работы

	/* default interrut handlers setup */
    for (offset = 0; offset < IRQ_GIC_LINE_COUNT; ++ offset)
    {
        intc_func_table [offset] = Userdef_INTC_Dummy_Interrupt;    /* Set all interrupts default handlers */
    }
	
    /* ==== Initial setting 1 to receive GIC interrupt request ==== */
    /* Interrupt Security Registers setting */
    addr = (volatile uint32_t *) & INTC.ICDISR0;
    for (offset = 0; offset < INTC_ICDISR_REG_TOTAL; ++ offset)
    {
        * (addr + offset) = 0x00000000uL;    /* Set all interrupts to be secured */
    }

    /* Interrupt Configuration Registers setting */
    addr = (volatile uint32_t *) & INTC.ICDICFR0;
    for (offset = 0; offset < INTC_ICDICFR_REG_TOTAL; ++ offset)
    {
        * (addr + offset) = intc_icdicfrn_table [offset];
    }

    /* Interrupt Priority Registers setting */
    addr = (volatile uint32_t *) & GICD_IPRIORITYRn(0);
    for (offset = 0; offset < INTC_ICDIPR_REG_TOTAL; ++ offset)
    {
        /* Set the priority for all interrupts to 31 */
        * (addr + offset) = (31 << ARM_CA9_PRIORITYSHIFT) * 0x01010101uL;
    }

    /* Interrupt Processor Targets Registers setting */
    /* Initialise ICDIPTR8 to ICDIPTRn                     */
    /* (n = The number of interrupt sources / 4)           */
    /*   - ICDIPTR0 to ICDIPTR4 are dedicated for main CPU */
    /*   - ICDIPTR5 is dedicated for sub CPU               */
    /*   - ICDIPTR6 to 7 are reserved                      */
    addr = (volatile uint32_t *) & INTC.ICDIPTR0;
    for (offset = 8; offset < INTC_ICDIPTR_REG_TOTAL; ++ offset)
    {
    	/* Set the target for all interrupts to main CPU */
        * (addr + offset) = 0x01010101uL;
    }

    /* Interrupt Clear-Enable Registers setting */
    addr = (volatile uint32_t *) & INTC.ICDICER0;
    for (offset = 0; offset < INTC_ICDICER_REG_TOTAL; ++ offset)
    {
    	 /* Set all interrupts to be disabled */
    	* (addr + offset) = 0xFFFFFFFFuL;
    }

	//GIC_Enable();	// инициализирует не совсем так как надо для работы
if (0)
{
	// а так работает...
  uint32_t i;
  uint32_t priority_field;

  //A reset sets all bits in the IGROUPRs corresponding to the SPIs to 0,
  //configuring all of the interrupts as Secure.

  //Disable interrupt forwarding
  GIC_DisableInterface();

  /* Priority level is implementation defined.
   To determine the number of priority bits implemented write 0xFF to an IPRIORITYR
   priority field and read back the value stored.*/
  GIC_SetPriority((IRQn_Type)0U, 0xFFU);
  priority_field = GIC_GetPriority((IRQn_Type)0U);

  //SGI and PPI
  for (i = 0U; i < 32U; i++)
  {
    if(i > 15U) {
      //Set level-sensitive (and N-N model) for PPI
      GIC_SetConfiguration((IRQn_Type)i, 0U);
    }
    //Disable SGI and PPI interrupts
    GIC_DisableIRQ((IRQn_Type)i);
    //Set priority
    GIC_SetPriority((IRQn_Type)i, priority_field/2U);
  }
  //Enable interface
  GIC_EnableInterface();
  //Set binary point to 0
  GIC_SetBinaryPoint(0U);
  //Set priority mask
  GIC_SetInterfacePriorityMask(0xFFU);
}


    /* Interrupt Priority Mask Register setting */
    /* Enable priorities for all interrupts */
    GIC_SetInterfacePriorityMask(ARM_CA9_ENCODE_PRIORITY(31));	// GICC_PMR

    /* Binary Point Register setting */
    /* Group priority field [7:3], Subpriority field [2:0](Do not use) */
    //INTC.ICCBPR = 0x00000002uL; // Binary Point Register, GICC_BPR, may be ARM_CA9_PRIORITYSHIFT - 1
	GIC_SetBinaryPoint(2);
    /* CPU Interface Control Register setting */
    //INTC.ICCICR = 0x00000003uL;	// GICC_CTLR
	GIC_EnableInterface();	// cjeck GICInterface->CTLR a same for INTC.ICCICR

    /* Initial setting 2 to receive GIC interrupt request */
    /* Distributor Control Register setting */
    //INTC.ICDDCR = 0x00000001uL;
	GIC_EnableDistributor();	// check GICDistributor->CTLR a same for INTC.ICDDCR
}

/******************************************************************************
* Function Name: R_INTC_RegistIntFunc
* Description  : Registers the function specified by the func to the element 
*              : specified by the int_id in the INTC interrupt handler function
*              : table.
* Arguments    : uint16_t int_id         : Interrupt ID
*              : void (* func)(uint32_t) : Function to be registered to INTC
*              :                         : interrupt hander table
* Return Value : DEVDRV_SUCCESS          : Success of registration of INTC 
*              :                         : interrupt handler function
*              : DEVDRV_ERROR            : Failure of registration of INTC 
*              :                         : interrupt handler function
******************************************************************************/
void r7s721_intc_registintfunc(uint_fast16_t int_id, void (* func)(void))
{

    intc_func_table [int_id] = func;     /* Register specified interrupt functions */
}

/******************************************************************************/

// TTB initialize

// SAM9XE512 bits
//#define TLB_NCNB 0x0DF2 // Noncachable, Nonbufferable 11 0 1111 1 00 10
//#define TLB_WT 0x0DFA // Write-through 11 0 1111 1 10 10
//#define TLB_WB 0x0DFE // Write-back 11 0 1111 1 11 10
//#define TLB_INVALID 0x0000 // Write-back 11 0 1111 1 11 10


/*

; ---- Parameter setting to level1 descriptor (bits 19:0) ----
; setting for Strongly-ordered memory
TTB_PARA_STRGLY             EQU     2_00000000000000000000110111100010
; setting for Outer and inner not cache normal memory
TTB_PARA_NORMAL_NOT_CACHE   EQU     2_00000000000000000001110111100010
; setting for Outer and inner write back, write allocate normal memory (Cacheable)
TTB_PARA_NORMAL_CACHE       EQU     2_00000000000000000001110111101110

; ---- Memory area size (MB) ----
M_SIZE_NOR      EQU     128             ; [Area00] CS0, CS1 area (for NOR flash)
M_SIZE_SDRAM    EQU     128             ; [Area01] CS2, CS3 area (for SDRAM)
M_SIZE_CS45     EQU     128             ; [Area02] CS4, CS5 area
M_SIZE_SPI      EQU     128             ; [Area03] SPI, SP2 area (for Serial flash)
M_SIZE_RAM      EQU     10              ; [Area04] Internal RAM
M_SIZE_IO_1     EQU     502             ; [Area05] I/O area 1
M_SIZE_NOR_M    EQU     128             ; [Area06] CS0, CS1 area (for NOR flash) (mirror)
M_SIZE_SDRAM_M  EQU     128             ; [Area07] CS2, CS3 area (for SDRAM) (mirror)
M_SIZE_CS45_M   EQU     128             ; [Area08] CS4, CS5 area (mirror)
M_SIZE_SPI_M    EQU     128             ; [Area09] SPI, SP2 area (for Serial flash) (mirror)
M_SIZE_RAM_M    EQU     10              ; [Area10] Internal RAM (mirror)
M_SIZE_IO_2     EQU     2550            ; [Area11] I/O area 2
;===================================================================
; Cortex-A9 MMU Configuration
; Set translation table base
;===================================================================
    ;;; Cortex-A9 supports two translation tables
    ;;; Configure translation table base (TTB) control register cp15,c2
    ;;; to a value of all zeros, indicates we are using TTB register 0.
    MOV  r0,#0x0
    MCR  p15, 0, r0, c2, c0, 2      ;;; TTBCR

    ;;; write the address of our page table base to TTB register 0
    LDR  r0,=||Image$$TTB$$ZI$$Base||
    MOV  r1, #0x08                  ;;; RGN=b01  (outer cacheable write-back cached, write allocate)
                                    ;;; S=0      (translation table walk to non-shared memory)
    ORR  r1,r1,#0x40                ;;; IRGN=b01 (inner cacheability for the translation table walk is Write-back Write-allocate)
    ORR  r0,r0,r1
    MCR  p15, 0, r0, c2, c0, 0      ;;; TTBR0

;===================================================================
; PAGE TABLE generation 
; Generate the page tables
; Build a flat translation table for the whole address space.
; ie: Create 4096 1MB sections from 0x000xxxxx to 0xFFFxxxxx
; 31                 20 19  18  17  16 15  14   12 11 10  9  8     5   4    3 2   1 0
; |section base address| 0  0  |nG| S |AP2|  TEX  |  AP | P | Domain | XN | C B | 1 0|
;
; Bits[31:20]   - Top 12 bits of VA is pointer into table
; nG[17]=0      - Non global, enables matching against ASID in the TLB when set.
; S[16]=0       - Indicates normal memory is shared when set.
; AP2[15]=0  
; AP[11:10]=11  - Configure for full read/write access in all modes
; TEX[14:12]=000
; CB[3:2]= 00   - Set attributes to Strongly-ordered memory.
;                 (except for the descriptor where code segment is based, see below)
; IMPP[9]=0     - Ignored
; Domain[5:8]=1111   - Set all pages to use domain 15
; XN[4]=0       - Execute never disabled
; Bits[1:0]=10  - Indicate entry is a 1MB section
;===================================================================

  */
// See B3.5.2 in DDI0406C_C_arm_architecture_reference_manual.pdf

//; setting for Strongly-ordered memory
//#define	TTB_PARA_STRGLY             0b_0000_0000_1101_1110_0010
#define	TTB_PARA_STRGLY             0b00000000110111100010UL	// 0x00DE2

//; setting for Outer and inner not cache normal memory
//#define	TTB_PARA_NORMAL_NOT_CACHE   0b_0000_0001_1101_1110_0010
#define	TTB_PARA_NORMAL_NOT_CACHE   0b00000001110111100010UL	// 0x01DE2

//; setting for Outer and inner write back, write allocate normal memory (Cacheable)
//#define	TTB_PARA_NORMAL_CACHE       0b_0000_0001_1101_1110_1110
#define	TTB_PARA_NORMAL_CACHE       0b00000001110111101110UL	// 01DEE

static uint32_t 
r7s721_accessbits(uintptr_t a)
{
	const uint32_t addrbase = a & 0xFFF00000uL;

	if (a >= 0x00000000uL && a < 0x00A00000uL)			// up to 10 MB
		return addrbase | TTB_PARA_NORMAL_CACHE;
	if (a >= 0x20000000uL && a < 0x20A00000uL)			// up to 10 MB
		return addrbase | TTB_PARA_NORMAL_CACHE;
	return addrbase | TTB_PARA_STRGLY;
}

static void r7s721_ttb_initialize(void)
{
	extern volatile uint32_t __ttb_start__;		// получено из скрипта линкера
	volatile uint32_t * const tlbbase = & __ttb_start__;
	unsigned i;

	for (i = 0; i < 4096; ++ i)
	{
		const uintptr_t address = (uint32_t) i << 20;
		tlbbase [i] =  r7s721_accessbits(address);
	}	

	//CP15_writeTTBCR(0);
	__set_TTBR0((unsigned int) tlbbase | 0x48);	// TTBR0
	//CP15_writeTTB1((unsigned int) tlbbase | 0x48);	// TTBR1

	// Program the domain access register
	//__set_DACR(0x55555555); // domain 15: access are not checked
	__set_DACR(0xFFFFFFFF); // domain 15: access are not checked
}

// с точностью до 1 мегабайта
static void r7s721_ttb_map(
	uintptr_t va,	/* virtual address */
	uintptr_t la	/* linear (physical) address */
	)
{
	volatile extern uint32_t __ttb_start__;		// получено из скрипта линкера
	volatile uint32_t * const tlbbase = & __ttb_start__;
	unsigned i = va >> 20;
	tlbbase [i] =  r7s721_accessbits(la);
}

#if LCDMODE_LTDC && LCDMODE_LTDCSDRAMBUFF

#if LCDMODE_LTDCSDRAMBUFF

void arm_hardware_sdram_initialize(void)
{

}
#endif /* LCDMODE_LTDCSDRAMBUFF */

void arm_hardware_ltdc_initialize(void)
{
}

#endif /* LCDMODE_LTDC && LCDMODE_LTDCSDRAMBUFF */

#endif /* CPUSTYLE_R7S721 */

#if CPUSTYLE_ARM_CA9
/* 
	ARM IHI 0048B.b (IHI0048B_b_gic_architecture_specification.pdf).
	4.3.11 Interrupt Priority Registers, GICD_IPRIORITYRn says:
	These registers are byte-accessible.
*/

static void 
arm_gic_initialize(void)
{
#if CPUSTYLE_R7S721
	r7s721_intc_initialize();
#endif /* CPUSTYLE_R7S721 */
	
#if WITHNESTEDINTERRUPTS
    GIC_SetInterfacePriorityMask(gARM_BASEPRI_ALL_ENABLED);
#endif /* WITHNESTEDINTERRUPTS */
}

#endif /* CPUSTYLE_ARM_CA9 */

#if CPUSTYLE_ATSAM3S

static void 
arm_cpu_atsam3s_pll_initialize(void)
{
	// Disable Watchdog
	//WDT->WDT_MR = WDT_MR_WDDIS;

	// Embedded Flash Wait State VDDCORE set at 1.65V
	// 17 MHz - 1 cycle = FWS = 0
	// 30 MHz - 2 cycle = FWS = 1
	// 54 MHz - 3 cycle = FWS = 2
	// 64 MHz - 4 cycle = FWS = 3

	// Embedded Flash Wait State VDDCORE set at 1.80V
	// 32 MHz - 1 cycle = FWS = 0
	// 38 MHz - 2 cicle = FWS = 1
	// 64 MHz - 3 cycls = FWS = 2

#if CPU_FREQ == 64000000UL
	enum { OSC_MUL = 32, OSC_DIV = 3, FWS = 2, noneedpll = 0 };	// 12 MHz / 3 * 32 = 128 MHz
#elif CPU_FREQ == 48000000UL
	enum { OSC_MUL = 8, OSC_DIV = 1, FWS = 2, noneedpll = 0 };	// 12 MHz / 1 * 8 = 96 MHz
#elif CPU_FREQ == 32000000UL
	enum { OSC_MUL = 16, OSC_DIV = 3, FWS = 1, noneedpll = 0 };	// 12 MHz / 3 * 16 = 96 MHz
#else
	enum { OSC_MUL = 1, OSC_DIV = 1, FWS = 1, noneedpll = 1 };	// 12 MHz / 3 * 16 = 96 MHz
	//#error Unsupported CPU_FREQ value
#endif

	if (noneedpll)
	{
	}
	else if (1)
	{
		// умножение кварцевого генератора
		lowlevel_sam3s_init_pll_clock_xtal(OSC_MUL, OSC_DIV, FWS);
	}
	else if (0)
	{
		// умножение от внутреннего RC генератора
		lowlevel_sam3s_init_pll_clock_RC12(OSC_MUL, OSC_DIV, FWS);	
	}
}
#endif /* CPUSTYLE_ATSAM3S */

#if CPUSTYLE_ATSAM4S
static void 
arm_cpu_atsam4s_pll_initialize(void)
{
	// Disable Watchdog
	//WDT->WDT_MR = WDT_MR_WDDIS;

	// Embedded Flash Wait State VDDCORE set at 1.65V
	// 17 MHz - 1 cycle = FWS = 0
	// 30 MHz - 2 cycle = FWS = 1
	// 54 MHz - 3 cycle = FWS = 2
	// 64 MHz - 4 cycle = FWS = 3

	// Embedded Flash Wait State VDDCORE set at 1.80V
	// 32 MHz - 1 cycle = FWS = 0
	// 38 MHz - 2 cicle = FWS = 1
	// 64 MHz - 3 cycls = FWS = 2

#if CPU_FREQ == 120000000UL
	enum { OSC_MUL = 60, OSC_DIV = 3, FWS = 5, noneedpll = 0 };	// 12 MHz / 3 * 60 = 240 MHz
#elif CPU_FREQ == 112000000UL
	enum { OSC_MUL = 56, OSC_DIV = 3, FWS = 5, noneedpll = 0 };	// 12 MHz / 3 * 40 = 224 MHz
#elif CPU_FREQ == 104000000UL
	enum { OSC_MUL = 52, OSC_DIV = 3, FWS = 5, noneedpll = 0 };	// 12 MHz / 3 * 52 = 208 MHz
#elif CPU_FREQ == 96000000UL
	enum { OSC_MUL = 48, OSC_DIV = 3, FWS = 5, noneedpll = 0 };	// 12 MHz / 3 * 40 = 160 MHz
#elif CPU_FREQ == 80000000UL
	enum { OSC_MUL = 40, OSC_DIV = 3, FWS = 4, noneedpll = 0 };	// 12 MHz / 3 * 40 = 160 MHz
#elif CPU_FREQ == 64000000UL
	enum { OSC_MUL = 32, OSC_DIV = 3, FWS = 3, noneedpll = 0 };	// 12 MHz / 3 * 32 = 128 MHz
#elif CPU_FREQ == 48000000UL
	enum { OSC_MUL = 8, OSC_DIV = 1, FWS = 2, noneedpll = 0 };	// 12 MHz / 1 * 8 = 96 MHz
#elif CPU_FREQ == 32000000UL
	enum { OSC_MUL = 16, OSC_DIV = 3, FWS = 1, noneedpll = 0 };	// 12 MHz / 3 * 16 = 96 MHz
#else
	enum { OSC_MUL = 1, OSC_DIV = 1, FWS = 1, noneedpll = 1 };	// 12 MHz / 3 * 16 = 96 MHz
	//#error Unsupported CPU_FREQ value
#endif

	if (noneedpll)
	{
	}
	else if (1)
	{
		// умножение кварцевого генератора
		lowlevel_sam4s_init_pll_clock_xtal(OSC_MUL, OSC_DIV, FWS);
	}
	else if (0)
	{
		// умножение от внутреннего RC генератора
		lowlevel_sam4s_init_pll_clock_RC12(OSC_MUL, OSC_DIV, FWS);	
	}
}
#endif /* CPUSTYLE_ATSAM4S */

#if CPUSTYLE_ATXMEGA

static uint8_t CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_t clockSource )
{
	auto void CCPWrite(volatile uint8_t * address, uint8_t value)
	{
		volatile uint8_t * const tmpAddr = address;
		//disableIRQ();
	#ifdef RAMPZ
		RAMPZ = 0;
	#endif
		asm volatile(
			"movw r30,  %0"	      "\n\t"
			"ldi  r16,  %2"	      "\n\t"	// take signature to R16
			"out   %3, r16"	      "\n\t"	// write to CCP
			"st     Z,  %1"       "\n\t"	// write to desired register
			:
			: "r" (tmpAddr), "r" (value), "M" (CCP_IOREG_gc), "i" (& CCP)
			: "r16", "r30", "r31"
			);

		//enableIRQ();
	}

	const uint8_t clkCtrl = (CLK.CTRL & ~CLK_SCLKSEL_gm) | clockSource;
#if 0
	// Так нельзя, работоспособность кода зависит от оптимизаций.
	CCP = CCP_IOREG_gc;   	
	CLK.CTRL = clkCtrl;
#else
	CCPWrite(& CLK.CTRL, clkCtrl);
#endif
	return (CLK.CTRL & clockSource);
}


static void cpu_atxmega_switchto32MHz()
{
	// switch to 32 MHz
	OSC.CTRL |= OSC_RC32MEN_bm;
	while ((OSC.STATUS & OSC_RC32MRDY_bm ) == 0)
		;
	CLKSYS_Main_ClockSource_Select(CLK_SCLKSEL_RC32M_gc);
	OSC.CTRL &= ~ (OSC_RC2MEN_bm | OSC_RC32KEN_bm);
}

#endif /* CPUSTYLE_ATXMEGA */

#if CPUSTYPE_TMS320F2833X

void cpu_tms320f2833x_pll_initialize(
		uint_fast8_t pllcrDIV, 		// PLL multiplier
		uint_fast8_t pllstsDIVSEL)	// PLL divider (from PLL to CPU)
{
    /* check if running in Limp mode; if yes, abort */
    if (PLLSTS & PLLSTS_MCLKSTS_BIT) {
        //Boot_limpAbort();
    	for (;;)
    		;
    }

    /* set max divide select (DIVSEL = 0) */
    PLLSTS &= ~PLLSTS_DIVSEL_BITS;

    /* temporarily disable failed oscillator detect */
    PLLSTS |= PLLSTS_MCLKOFF_BIT;

    /* set the new PLL multiplier value */
    PLLCR = pllcrDIV;

    /* wait for the PLL to relock */
    while (!(PLLSTS & PLLSTS_PLLLOCKS_BIT)) {
    };

    /* re-enable failed oscillator detection */
    PLLSTS &= ~PLLSTS_MCLKOFF_BIT;

    /* set divide select bits (DIVSEL) */
    PLLSTS |= pllstsDIVSEL << PLLSTS_DIVSEL_SHIFTBITS;
}


#pragma CODE_SECTION(cpu_tms320f2833x_flash_waitstates, "ramfuncs")

static void
cpu_tms320f2833x_flash_waitstates(uint_fast8_t flashws, uint_fast8_t otpws)
{
	// To ensure the FLASH in high power mode
	FPWR = 0x003;
	asm(" RPT #8 || NOP");

	//Enable Flash Pipeline mode to improve performance
	//of code executed from Flash.
	//FlashRegs.FOPT.bit.ENPIPE = 1;
	FOPT |= 0x0001;
	asm(" RPT #8 || NOP");

	//                CAUTION
	//Minimum waitstates required for the flash operating
	//at a given CPU rate must be characterized by TI.
	//Refer to the datasheet for the latest information.

	//Set the Paged Waitstate for the Flash.
	//FlashRegs.FBANKWAIT.bit.PAGEWAIT = 3;
	FBANKWAIT = (FBANKWAIT & ~ 0x0f00) | (flashws << 8);
	asm(" RPT #8 || NOP");

	//Set the Random Waitstate for the Flash.
	//FlashRegs.FBANKWAIT.bit.RANDWAIT = 3;
	FBANKWAIT = (FBANKWAIT & ~ 0x000f) | (flashws << 0);
	asm(" RPT #8 || NOP");

	//Set the Waitstate for the OTP.
	//FlashRegs.FOTPWAIT.bit.OTPWAIT = 5;
	FOTPWAIT = (FOTPWAIT & ~ 0x000f) | (otpws << 0);
	asm(" RPT #8 || NOP");

	//                CAUTION
	//ONLY THE DEFAULT VALUE FOR THESE 2 REGISTERS SHOULD BE USED
	//FlashRegs.FSTDBYWAIT.bit.STDBYWAIT = 0x01FF;
	FSTDBYWAIT = (FSTDBYWAIT & ~ 0x01FF) | 0x01FF;
	asm(" RPT #8 || NOP");
	//FlashRegs.FACTIVEWAIT.bit.ACTIVEWAIT = 0x01FF;
	FACTIVEWAIT = (FACTIVEWAIT & ~ 0x01FF) | 0x01FF;
	asm(" RPT #8 || NOP");

	//Force a pipeline flush to ensure that the write to
	//the last register configured occurs before returning.

	asm(" RPT #8 || NOP");
}
#endif /* CPUSTYPE_TMS320F2833X */

// Вызывается из main
void cpu_initialize(void)
{
#if CPUSTYLE_STM32F1XX

	cpu_stm32f1xx_setmapr(0);	/* переключить отладочный интерфейс в SWD */
	// Разрешить работу компаратора напряжения питания (нужно для разряда емкостей преобразователя питания дисплея)
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;     //включить тактирование power management
	__DSB();
	PWR->CR = (PWR->CR & ~ PWR_CR_PLS) | PWR_CR_PLS_2V8 | PWR_CR_PVDE;

#elif CPUSTYLE_STM32F4XX

	RCC->APB1ENR |= RCC_APB1ENR_PWREN;	// включить тактирование power management
	__DSB();

	#if WITHUSESAIPLL
		lowlevel_stm32f4xx_pllsai_initialize();
	#endif /* WITHUSESAIPLL */

#elif CPUSTYLE_STM32H7XX

	//RCC->APB1ENR |= RCC_APB1ENR_PWREN;	// включить тактирование power management
	//__DSB();

	#if WITHUSESAIPLL
		lowlevel_stm32h7xx_pllsai_initialize();
	#endif /* WITHUSESAIPLL */

#elif CPUSTYLE_STM32F7XX

	RCC->APB1ENR |= RCC_APB1ENR_PWREN;	// включить тактирование power management
	__DSB();

	arm_hardware_flush_all();
	#if WITHUSESAIPLL
		lowlevel_stm32f7xx_pllsai_initialize();
	#endif /* WITHUSESAIPLL */

#elif CPUSTYLE_STM32F0XX
 
	// Разрешить работу компаратора напряжения питания (нужно для разряда емкостей преобразователя питания дисплея)
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;     // включить тактирование power management
	__DSB();
	PWR->CR = (PWR->CR & ~ PWR_CR_PLS) | PWR_CR_PLS_LEV3 | PWR_CR_PVDE;

#elif CPUSTYLE_STM32L0XX
 
	// Разрешить работу компаратора напряжения питания (нужно для разряда емкостей преобразователя питания дисплея)
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;     // включить тактирование power management
	__DSB();
	PWR->CR = (PWR->CR & ~ PWR_CR_PLS) | PWR_CR_PLS_LEV3 | PWR_CR_PVDE;

#elif CPUSTYLE_STM32F30X

	// Разрешить работу компаратора напряжения питания (нужно для разряда емкостей преобразователя питания дисплея)
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;     // включить тактирование power management
	__DSB();
	PWR->CR = (PWR->CR & ~ PWR_CR_PLS) | PWR_CR_PLS_LEV3 | PWR_CR_PVDE;

#elif CPUSTYLE_ATSAM3S

	arm_cpu_atsam3s_pll_initialize();

#elif CPUSTYLE_ATSAM4S

	arm_cpu_atsam4s_pll_initialize();

#elif CPUSTYLE_AT91SAM7S

	lowlevwl_interrupts_init();

	usb_disable();

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

#elif CPUSTYLE_R7S721

	extern unsigned long __etext, __bss_start__, __bss_end__, __data_end__, __data_start__, __stack;

	debug_printf_P(PSTR("cpu_initialize1: CP15=%08lX, __data_start__=%p\n"), __get_SCTLR(), & __data_start__);
	debug_printf_P(PSTR("__etext=%p, __bss_start__=%p, __bss_end__=%p, __data_start__=%p, __data_end__=%p\n"), & __etext, & __bss_start__, & __bss_end__, & __data_start__, & __data_end__);
	debug_printf_P(PSTR("__stack=%p, arm_cpu_initialize=%p\n"), & __stack, arm_cpu_initialize);

    /* ==== Writeback Cache ==== */
    //io_cache_writeback();

    /* ==== Vector base address setting ==== */
    //VbarInit();

	//MMU_Disable();
	// MMU setup
	r7s721_ttb_initialize();
	// Отображение 10 мегабайт с 0x20000000 в 0x00000000
	// Хотя, достаточно и одной страницы c с переходами на обработчики прерываний - код выполняется на 0x20000000
	r7s721_ttb_map(0x00000000uL, (uint32_t) & __data_start__);	// с точностью до 1 мегабайта
	//unsigned long offset;
	//for (offset = 0; offset < 10uL * 1024 * 1024; offset += 1uL * 1024 * 1024)
	//	r7s721_ttb_map(0x00000000uL + offset, __data_start__ + offset); // с точностью до 1 мегабайта
	//CP15_writeTLBIALLIS(0);	// Invalidate TLB
	MMU_InvalidateTLB();
	
	// диагностическая печать параметров CACHE
	//const uint32_t clidr = __get_CLIDR();
	//debug_printf_P(PSTR("cpu_initialize1: clidr=%08lX\n"), clidr);
	//ASSERT((clidr & 0x03) != 0 && ARM_CA9_CACHELEVELMAX == 1);

	// Обеспечиваем нормальную обработку RESEТ
	arm_hardware_invalidate_all();

	//CP15_enableMMU();
	MMU_Enable();

	ca9_cache_setup();

#if WITHDEBUG
	uint_fast32_t leveli;
	for (leveli = 0; leveli <= ARM_CA9_CACHELEVELMAX; ++ leveli)
	{

		__set_CSSELR(leveli * 2 + 0);	// data cache select
		const uint32_t ccsidr0 = __get_CCSIDR();
		const uint32_t assoc0 = (ccsidr0 >> 3) & 0x3FF;
		const int passoc0 = ilog2(assoc0);
		const uint32_t maxsets0 = (ccsidr0 >> 13) & 0x7FFF;
		const uint32_t linesize0 = 4uL << (((ccsidr0 >> 0) & 0x07) + 2);
		debug_printf_P(PSTR("cpu_initialize1: level=%d, passoc=%d, assoc=%u, maxsets=%u, data cache row size = %u\n"), leveli, passoc0, assoc0, maxsets0, linesize0);

		__set_CSSELR(leveli * 2 + 1);	// instruction cache select
		const uint32_t ccsidr1 = __get_CCSIDR();
		const uint32_t assoc1 = (ccsidr1 >> 3) & 0x3FF;
		const int passoc1 = ilog2(assoc1);
		const uint32_t maxsets1 = (ccsidr1 >> 13) & 0x7FFF;
		const uint32_t linesize1 = 4uL << (((ccsidr1 >> 0) & 0x07) + 2);
		debug_printf_P(PSTR("cpu_initialize1: level=%d, passoc=%d, assoc=%u, maxsets=%u, instr cache row size = %u\n"), leveli, passoc1, assoc1, maxsets1, linesize1);
	}
#endif /* WITHDEBUG */

	//INB.RMPR &= ~ (1U << 1);		// 0: Address remapping is enabled 0x20000000 visible at 0x00000000.
	//(void) INB.RMPR;

	//extern unsigned long __isr_vector__;
	//CP15_set_vbase_address((unsigned int) & __isr_vector__); // Set Vector Base Address Register
	//CP15_set_mvbase_address((unsigned int) & __isr_vector__);	//  Set Monitor Vector Base Address Register

	__set_VBAR(0);	 // Set Vector Base Address Register
	//CP15_set_vbase_address(0); // Set Vector Base Address Register

	__set_MVBAR(0);	 // Set Vector Base Address Register
	//CP15_set_mvbase_address(0);	//  Set Monitor Vector Base Address Register

	//cp15_vectors_reloc_disable();
	__set_SCTLR(__get_SCTLR() & ~ SCTLR_V_Msk);

	debug_printf_P(PSTR("cpu_initialize2: CP15=%08lX\n"), __get_SCTLR());

	/* TN-RZ*-A011A/E recommends switch off USB_X1 if usb USB not used */
	CPG.STBCR7 &= ~ CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module runs.
	(void) CPG.STBCR7;			/* Dummy read */
	USB200.SYSCFG0 |= USB_SYSCFG_UCKSEL; // UCKSEL 1: The 12-MHz EXTAL clock is selected.
	local_delay_ms(2);	// required 1 ms delay - see R01UH0437EJ0200 Rev.2.00 28.4.1 System Control and Oscillation Control
	CPG.STBCR7 |= CPG_STBCR7_MSTP71;	// Module Stop 71 0: Channel 0 of the USB 2.0 host/function module halts.
	
	CPG.STBCR7 &= ~ CPG_STBCR7_MSTP70;	// Module Stop 70 0: Channel 1 of the USB 2.0 host/function module runs.
	(void) CPG.STBCR7;			/* Dummy read */
	USB201.SYSCFG0 |= USB_SYSCFG_UCKSEL; // UCKSEL 1: The 12-MHz EXTAL clock is selected.
	local_delay_ms(2);	// required 1 ms delay - see R01UH0437EJ0200 Rev.2.00 28.4.1 System Control and Oscillation Control
	CPG.STBCR7 |= CPG_STBCR7_MSTP70;	// Module Stop 70 0: Channel 1 of the USB 2.0 host/function module halts.
	
#else
	#error Undefined CPUSTYLE_XXX
#endif

	// Инициализация контроллера прерываний

#if CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7 || CPUSTYLE_ARM_CM0

	arm_cpu_CMx_initialize_NVIC();

#elif CPUSTYLE_ARM_CA9

	// FPU
	vfp_access_enable();
	arm_hardware_VFPEnable();
	//__FPU_Enable();

	arm_gic_initialize();

#endif /* CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7 */
}

// секция init больше не нужна
void cpu_initdone(void)
{
#if CPUSTYLE_R7S721
	// Когда загрузочный образ FPGA будт оставаться в SERIAL FLASH, запретить отключение.
	while ((SPIBSC0.CMNSR & (1u << 0)) == 0)	// TEND bit
		;
	// SPI multi-io Read Cache Flush
	SPIBSC0.DRCR = (1u << 9);	// RCF bit
	(void) SPIBSC0.DRCR;		/* Dummy read */

	SPIBSC0.SMCR = 0;
	// spi multi-io hang off
	CPG.STBCR9 |= (1U << 3);	// Module Stop 93	- 1: Clock supply to channel 0 of the SPI multi I/O bus controller is halted.
	(void) CPG.STBCR9;			/* Dummy read */
	arm_hardware_pio4_inputs(0xFC);		// Отключить процессор от SERIAL FLASH
#endif /* CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7 */
}

// optimizer test: from electronix.ru - should be one divmod call
/*
uint8_t xxxxxpos(uint8_t num) // num = 0..8
{
    uint8_t pos = 0x40;
    while (num >= 3) {
        pos += 0x40;
        num -= 3;
    }
    pos += num * 7;
    return pos;
}

*/

#if CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM0 || CPUSTYLE_ARM_CM7
/*----------------------------------------------------------------------------
 *        Exported variables
 *----------------------------------------------------------------------------*/

/* Initialize segments */
void Default_Handler(void)
{
	debug_printf_P(PSTR("Default_Handler trapped.\n"));
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


	debug_printf_P(PSTR("HardFault_Handler trapped.\n"));
 	debug_printf_P(PSTR(" CPUID=%08lx\n"), SCB->CPUID);
	debug_printf_P(PSTR(" HFSR=%08lx\n"), SCB->HFSR);
	debug_printf_P(PSTR(" CFSR=%08lx\n"), SCB->CFSR);
	debug_printf_P(PSTR(" BFAR=%08lx\n"), SCB->BFAR);

	debug_printf_P(PSTR(" R0=%08lx\n"), r0);
	debug_printf_P(PSTR(" R1=%08lx\n"), r1);
	debug_printf_P(PSTR(" R2=%08lx\n"), r2);
	debug_printf_P(PSTR(" R3=%08lx\n"), r3);

	debug_printf_P(PSTR(" R12=%08lx\n"), r12);
	debug_printf_P(PSTR(" LR=%08lx\n"), lr);
	debug_printf_P(PSTR(" PC=%08lx\n"), pc);
	debug_printf_P(PSTR(" PSR=%08lx\n"), psr);

    // Когда мы добрались до этой строки, то в переменных содержатся значения регистров.
    for( ;; )
		;
}

#endif /* WITHDEBUG && (CPUSTYLE_ARM_CM7 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM3) */

///////////////////////////
//
// taken from: http://forum.easyelectronics.ru/viewtopic.php?p=396176#p396176

/*=================================================================================================================================
*  Обработчик HardFault исключений. В нем вызывается функция prvGetRegistersFromStack(), которая сохраняет в переменных, значения
* регистров программы, в момент возникновения исключения и входит в бесконечный цикл. Таким образом, можно по значениям переменных
* узнать причину возникновения исключения.
=================================================================================================================================*/


void ATTRWEAK 
HardFault_Handler(void)
{
#if WITHDEBUG
#if CPUSTYLE_ARM_CM7 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM3

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

	debug_printf_P(PSTR("HardFault_Handler trapped.\n"));
	debug_printf_P(PSTR(" CPUID=%08lx\n"), SCB->CPUID);

#else

	dbg_putchar('S');	// "SK"
	dbg_putchar('K');

#endif
	//debug_printf_P(PSTR("HardFault_Handler trapped. HFSR=%08lx\n"), SCB->HFSR);
	//debug_printf_P(PSTR("HardFault_Handler trapped"));
	//return;
	//if ((SCB->HFSR & SCB_HFSR_FORCED_Msk) != 0)
	//{
	//}
	//local_delay_ms(10);
#endif /* WITHDEBUG */

	for (;;)
		; // WDT->WDT_CR = WDT_CR_WDRSTT | WDT_CR_KEY(0xA5);

}

void ATTRWEAK 
NMI_Handler(void)
{
	debug_printf_P(PSTR("NMI_Handler trapped\n"));
	for (;;)
		; // WDT->WDT_CR = WDT_CR_WDRSTT | WDT_CR_KEY(0xA5);
}

void ATTRWEAK 
MemManage_Handler(void)
{
	debug_printf_P(PSTR("MemManage_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
BusFault_Handler(void)
{
	debug_printf_P(PSTR("BusFault_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
UsageFault_Handler(void)
{
	debug_printf_P(PSTR("UsageFault_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
SVC_Handler(void)
{
	debug_printf_P(PSTR("SVC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
DebugMon_Handler(void)
{
	debug_printf_P(PSTR("DebugMon_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
PendSV_Handler(void)
{
	debug_printf_P(PSTR("PendSV_Handler trapped\n"));
	for (;;)
		;
}

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

void ATTRWEAK 
SUPC_Handler(void)
{
	debug_printf_P(PSTR("SUPC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
RSTC_Handler(void)
{
	debug_printf_P(PSTR("SUPC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
RTC_Handler(void)
{
	debug_printf_P(PSTR("SUPC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
RTT_Handler(void)
{
	debug_printf_P(PSTR("SUPC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
WDT_Handler(void)
{
	debug_printf_P(PSTR("SUPC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
PMC_Handler(void)
{
	debug_printf_P(PSTR("SUPC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
EFC_Handler(void)
{
	debug_printf_P(PSTR("SUPC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
UART0_Handler(void)
{
	debug_printf_P(PSTR("UART0_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
UART1_Handler(void)
{
	debug_printf_P(PSTR("UART1_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
SMC_Handler(void)
{
	debug_printf_P(PSTR("SMC_Handler trapped\n"));
	for (;;)
		;
}
void ATTRWEAK 
PIOB_Handler(void)
{
	debug_printf_P(PSTR("SMC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
PIOC_Handler(void)
{
	debug_printf_P(PSTR("SMC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
HSMCI_Handler(void)
{
	debug_printf_P(PSTR("SMC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
TWI0_Handler(void)
{
	debug_printf_P(PSTR("SMC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
TWI1_Handler(void)
{
	debug_printf_P(PSTR("SMC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
SPI_Handler(void)
{
	debug_printf_P(PSTR("SMC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
SSC_Handler(void)
{
	debug_printf_P(PSTR("SMC_Handler trapped\n"));
	for (;;)
		;
}


void ATTRWEAK 
TC1_Handler(void)
{
	debug_printf_P(PSTR("TC1_Handler trapped\n"));
	for (;;)
		;
}

/*
void ATTRWEAK 
TC2_Handler(void)
{
	debug_printf_P(PSTR("TC2_Handler trapped\n"));
	for (;;)
		;
}
*/
void ATTRWEAK 
TC3_Handler(void)
{
	debug_printf_P(PSTR("SMC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
TC4_Handler(void)
{
	debug_printf_P(PSTR("SMC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
TC5_Handler(void)
{
	debug_printf_P(PSTR("SMC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
DACC_Handler(void)
{
	debug_printf_P(PSTR("SMC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
PWM_Handler(void)
{
	debug_printf_P(PSTR("SMC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
CRCCU_Handler(void)
{
	debug_printf_P(PSTR("SMC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
ACC_Handler(void)
{
	debug_printf_P(PSTR("SMC_Handler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK 
UDP_Handler(void)
{
	debug_printf_P(PSTR("SMC_Handler trapped\n"));
	for (;;)
		;
}

#elif CPUSTYLE_STM32F

void ATTRWEAK USART1_IRQHandler(void)
{
	debug_printf_P(PSTR("USART1_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK USART2_IRQHandler(void)
{
	debug_printf_P(PSTR("USART2_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK USART3_IRQHandler(void)
{
	debug_printf_P(PSTR("USART3_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK WWDG_IRQHandler(void)
{
	debug_printf_P(PSTR("PendSV_Handler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK PVD_IRQHandler(void)
{
	//hardware_reqshutdown = 1;
	debug_printf_P(PSTR("PendSV_Handler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK TAMPER_IRQHandler(void)
{
	debug_printf_P(PSTR("PendSV_Handler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK RTC_IRQHandler(void)
{
	debug_printf_P(PSTR("PendSV_Handler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK FLASH_IRQHandler(void)
{
	debug_printf_P(PSTR("PendSV_Handler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK RCC_IRQHandler(void)
{
	debug_printf_P(PSTR("PendSV_Handler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA1_Stream0_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Stream0_IRQHandler() trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA1_Stream1_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Stream1_IRQHandler() trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA1_Stream2_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Stream2_IRQHandler() trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA1_Stream3_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Stream3_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA1_Stream4_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Stream4_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA1_Stream5_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Stream5_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA1_Stream6_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Stream6_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA1_Stream7_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Stream7_IRQHandler() trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA2_Stream0_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA2_Stream0_IRQHandler() trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA2_Stream1_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA2_Stream1_IRQHandler() trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA2_Stream2_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA2_Stream2_IRQHandler() trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA2_Stream3_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA2_Stream3_IRQHandler() trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA2_Stream4_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA2_Stream4_IRQHandler() trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA2_Stream5_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA2_Stream5_IRQHandler() trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA2_Stream6_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA2_Stream6_IRQHandler() trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA2_Stream7_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA2_Stream7_IRQHandler() trapped.\n"));
	for (;;)
		;
}


void ATTRWEAK DMA1_Channel4_5_6_7_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Channel4_5_6_7_IRQHandler trapped.\n"));
	for (;;)
		;
}
void ATTRWEAK DMA1_Channel2_3_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Channel2_3_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA1_Channel4_5_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Channel4_5_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA1_Channel1_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Channel1_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA1_Channel2_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Channel2_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA1_Channel3_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Channel3_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA1_Channel4_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Channel4_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA1_Channel5_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Channel5_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA1_Channel6_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Channel6_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK DMA1_Channel7_IRQHandler(void)
{
	debug_printf_P(PSTR("DMA1_Channel7_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK TIM1_BRK_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM1_BRK_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK TIM1_UP_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM1_UP_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK TIM1_TRG_COM_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM1_TRG_COM_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK TIM1_CC_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM1_CC_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK TIM2_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM2_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK TIM4_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM4_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK TIM12_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM12_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK TIM13_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM13_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK TIM14_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM14_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK TIM15_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM15_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK TIM16_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM16_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK TIM17_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM17_IRQHandler trapped.\n"));
	for (;;)
		;
}


void ATTRWEAK TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM1_BRK_UP_TRG_COM_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK TIM6_DAC_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM6_DAC_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK USBWakeUp_IRQHandler(void)
{
	debug_printf_P(PSTR("USBWakeUp_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK TS_IRQHandler(void)
{
	debug_printf_P(PSTR("TS_IRQHandler trapped.\n"));
	for (;;)
		;
}

// CPUSTYLE_STM32F0XX
void ATTRWEAK I2C1_IRQHandler(void)
{
	debug_printf_P(PSTR("I2C1_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK I2C1_EV_IRQHandler(void)
{
	debug_printf_P(PSTR("PendSV_Handler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK I2C1_ER_IRQHandler(void)
{
	debug_printf_P(PSTR("PendSV_Handler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK I2C2_IRQHandler(void)
{
	debug_printf_P(PSTR("I2C2_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK I2C2_EV_IRQHandler(void)
{
	debug_printf_P(PSTR("I2C2_EV_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK I2C2_ER_IRQHandler(void)
{
	debug_printf_P(PSTR("I2C2_ER_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK SPI1_IRQHandler(void)
{
	debug_printf_P(PSTR("SPI1_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK SPI2_IRQHandler(void)
{
	debug_printf_P(PSTR("SPI2_IRQHandler trapped.\n"));
	for (;;)
		;
}


void ATTRWEAK SPI3_IRQHandler(void)
{
	debug_printf_P(PSTR("SPI3_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK RTCAlarm_IRQHandler(void)
{
	debug_printf_P(PSTR("RTCAlarm_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK USB_HP_CAN1_TX_IRQHandler(void)
{
	debug_printf_P(PSTR("USB_HP_CAN1_TX_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK CAN1_RX0_IRQHandler(void)
{
	debug_printf_P(PSTR("CAN1_RX0_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK CAN1_RX1_IRQHandler(void)
{
	debug_printf_P(PSTR("CAN1_RX1_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK CAN1_SCE_IRQHandler(void)
{
	debug_printf_P(PSTR("CAN1_SCE_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK OTG_FS_IRQHandler(void)
{
	debug_printf_P(PSTR("OTG_FS_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK OTG_HS_IRQHandler(void)
{
	debug_printf_P(PSTR("OTG_HS_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK OTG_HS_EP1_OUT_IRQHandler(void)
{
	debug_printf_P(PSTR("OTG_HS_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK OTG_HS_EP1_IN_IRQHandler(void)
{
	debug_printf_P(PSTR("OTG_HS_EP1_IN_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK OTG_HS_WKUP_IRQHandler(void)
{
	debug_printf_P(PSTR("OTG_HS_WKUP_IRQHandler trapped.\n"));
	for (;;)
		;
}

#if ! WITHCPUADCHW
void ATTRWEAK ADC1_COMP_IRQHandler(void)
{
	debug_printf_P(PSTR("ADC1_COMP_IRQHandler trapped.\n"));
	for (;;)
		;
}
#endif /* ! WITHCPUADCHW */

void ATTRWEAK LPTIM1_IRQHandler(void)
{
	debug_printf_P(PSTR("LPTIM1_IRQHandler trapped.\n"));
	for (;;)
		;
}
void ATTRWEAK LPUART1_IRQHandler(void)
{
	debug_printf_P(PSTR("LPUART1_IRQHandler trapped.\n"));
	for (;;)
		;
}
void ATTRWEAK TIM21_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM21_IRQHandler trapped.\n"));
	for (;;)
		;
}
void ATTRWEAK TIM22_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM22_IRQHandler trapped.\n"));
	for (;;)
		;
}

void ATTRWEAK TIM8_BRK_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM8_BRK_IRQHandler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK TIM8_UP_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM8_UP_IRQHandler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK TIM8_TRG_COM_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM8_TRG_COM_IRQHandler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK TIM8_CC_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM8_CC_IRQHandler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK FSMC_IRQHandler(void)
{
	debug_printf_P(PSTR("FSMC_IRQHandler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK SDIO_IRQHandler(void)
{
	debug_printf_P(PSTR("SDIO_IRQHandler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK TIM5_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM5_IRQHandler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK UART4_IRQHandler(void)
{
	debug_printf_P(PSTR("UART4_IRQHandler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK UART5_IRQHandler(void)
{
	debug_printf_P(PSTR("UART5_IRQHandler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK USART6_IRQHandler(void)
{
	debug_printf_P(PSTR("USART6_IRQHandler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK SDMMC1_IRQHandler(void)
{
	debug_printf_P(PSTR("SDMMC1_IRQHandler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK TIM6_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM6_IRQHandler trapped\n"));
	for (;;)
		;
}

void ATTRWEAK TIM7_IRQHandler(void)
{
	debug_printf_P(PSTR("TIM7_IRQHandler trapped\n"));
	for (;;)
		;
}

#if defined (STM32F429xx) || defined(STM32F446xx) || defined(STM32F746xx) || defined(STM32F767xx) || defined(STM32F723xx) || defined (STM32F769xx)

	#else

	void ATTRWEAK DMA2_Channel1_IRQHandler(void)
	{
		debug_printf_P(PSTR("DMA2_Channel1_IRQHandler trapped\n"));
		for (;;)
			;
	}

	void ATTRWEAK DMA2_Channel2_IRQHandler(void)
	{
		debug_printf_P(PSTR("DMA2_Channel2_IRQHandler trapped\n"));
		for (;;)
			;
	}

	void ATTRWEAK DMA2_Channel3_IRQHandler(void)
	{
		debug_printf_P(PSTR("DMA2_Channel3_IRQHandler trapped\n"));
		for (;;)
			;
	}

	void ATTRWEAK DMA2_Channel4_5_IRQHandler(void)
	{
		debug_printf_P(PSTR("DMA2_Channel4_5_IRQHandler trapped\n"));
		for (;;)
			;
	}
	#endif

#endif /* CPUSTYLE_STM32F */

typedef void (* IntFunc)(void);

extern unsigned long __etext;	// где во FLASH лежит образ инициализирующих data данных
extern unsigned long __bss_start__, __bss_end__, __data_end__, __data_start__, __stack;

#if CPUSTYLE_ARM_CM4

	extern unsigned long __ccm_start__, __ccm_end__;	// Cortex-M4 specific

#elif CPUSTYLE_ARM_CM7
	extern unsigned long __dtcm_start__, __dtcm_end__;	// Cortex-M7 specific
	extern unsigned long __itcm_start__, __itcm_end__;	// Cortex-M7 specific
	extern unsigned long __itcmdata_start__, __itcmdata_end__;	// Cortex-M7 specific
	extern unsigned long __ramfunctext;
#endif

extern int main(void);
extern void __libc_init_array(void);

/** \endcond */
/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler(void)
{
#if 0
	{
		// Формирование импульсов на выводе процессора
		for (;;)
		{
			const uint32_t WORKMASK = 1UL << 31;	// PA31

			arm_hardware_pioa_outputs(WORKMASK, WORKMASK * 1);
			hardware_spi_io_delay();
			arm_hardware_pioa_outputs(WORKMASK, WORKMASK * 0);
			hardware_spi_io_delay();
		}
	}
#endif
#if 0
	// PD13 signal
	enum { WORKMASK	 = 1ul << 13 };
	arm_hardware_piod_outputs(WORKMASK, WORKMASK);

	for (;;)
	{
		arm_hardware_piod_outputs(WORKMASK, 1 * WORKMASK);
		hardware_spi_io_delay();
		arm_hardware_piod_outputs(WORKMASK, 0 * WORKMASK);
		hardware_spi_io_delay();
	}
#endif


    /* Low level Initialize */
	arm_cpu_initialize();		// watchdog disable, clock initialize, cache enable
#if 0
	{
		// Формирование импульсов на выводе процессора
		for (;;)
		{
			const uint32_t WORKMASK = 1UL << 31;	// PA31

			arm_hardware_pioa_outputs(WORKMASK, WORKMASK * 1);
			hardware_spi_io_delay();
			arm_hardware_pioa_outputs(WORKMASK, WORKMASK * 0);
			hardware_spi_io_delay();
		}
	}
#endif
#if 0
	// PD13 signal
	enum { WORKMASK = 1ul << 13 };
	arm_hardware_piod_outputs(WORKMASK, WORKMASK);

	for (;;)
	{
		arm_hardware_piod_outputs(WORKMASK, 1 * WORKMASK);
		hardware_spi_io_delay();
		arm_hardware_piod_outputs(WORKMASK, 0 * WORKMASK);
		hardware_spi_io_delay();
	}
#endif
#if 0
	// PG13 signal
	enum { WORKMASK = 1ul << 13 };
	arm_hardware_piog_outputs(WORKMASK, WORKMASK);

	for (;;)
	{
		GPIOG->ODR ^= WORKMASK;	// Debug LEDs
		hardware_spi_io_delay();
		//local_delay_ms(300);
		local_delay_ms(400);
	}
#endif
	// Хотя возвращатся из данной функции не предполагается - память и адрес возврата могли бы затираются,
	// но вместе с ними будут стёрты и автоматические переменые, используемые в данной функции. Поэтому,
	// стирать ВСЮ память по её размеру нельзя.

	#if CPUSTYLE_ARM_CM4
		/* zero-init variables */
		//memset(& __ccm_start__, 0, (& __ccm_end__ - & __ccm_start__) * sizeof __ccm_end__);
	#elif CPUSTYLE_ARM_CM7
		/* zero-init variables */
		//memset(& __dtcm_start__, 0, (& __dtcm_end__ - & __dtcm_start__) * sizeof __dtcm_end__);
		memcpy(& __itcm_start__, & __ramfunctext, (& __itcmdata_end__ - & __itcmdata_start__) * sizeof __itcmdata_end__);
		arm_hardware_flush((uintptr_t) & __itcm_start__, (& __itcmdata_end__ - & __itcmdata_start__) * sizeof __itcmdata_end__);
	#endif
	 /* copy-init variables */
    memcpy(& __data_start__, & __etext, (& __data_end__ - & __data_start__) * sizeof __data_end__);
    /* zero-init variables */
    memset(& __bss_start__, 0, (& __bss_end__ - & __bss_start__) * sizeof __bss_end__);

	arm_hardware_flush((uintptr_t) & __data_start__, (& __data_end__ - & __data_start__) * sizeof __data_end__);
	arm_hardware_flush((uintptr_t) & __bss_start__, (& __bss_end__ - & __bss_start__) * sizeof __bss_end__);

#if 0

    /* Set the vector table base address */
    pSrc = (uint32_t *)&_sfixed;
    SCB->VTOR = ( (uint32_t)pSrc & SCB_VTOR_TBLOFF_Msk ) ;
    
    if ( ((uint32_t)pSrc >= IRAM_ADDR) && ((uint32_t)pSrc < IRAM_ADDR+IRAM_SIZE) )
    {
	    SCB->VTOR |= 1 << SCB_VTOR_TBLBASE_Pos ;
    }

    /* Initialize the C library */
    //__libc_init_array() ;
#endif

#if 0
	{
		// Формирование импульсов на выводе процессора
		for (;;)
		{
			const uint32_t WORKMASK = 1UL << 31;	// PA31

			arm_hardware_pioa_outputs(WORKMASK, WORKMASK * 1);
			hardware_spi_io_delay();
			arm_hardware_pioa_outputs(WORKMASK, WORKMASK * 0);
			hardware_spi_io_delay();
		}
	}
#endif
#if 0
	// PD13 signal
	enum { WORKMASK	 = 1ul << 13 };
	arm_hardware_piod_outputs(WORKMASK, WORKMASK);

	for (;;)
	{
		arm_hardware_piod_outputs(WORKMASK, 1 * WORKMASK);
		hardware_spi_io_delay();
		local_delay_ms(300);
		arm_hardware_piod_outputs(WORKMASK, 0 * WORKMASK);
		hardware_spi_io_delay();
		local_delay_ms(300);
	}
#endif
#if 0
	// PG13 signal
	enum { WORKMASK = 1ul << 13 };
	arm_hardware_piog_outputs(WORKMASK, WORKMASK);

	for (;;)
	{
		GPIOG->ODR ^= WORKMASK;	// Debug LEDs
		hardware_spi_io_delay();
		local_delay_ms(300);
	}
#endif
	//SystemInit();
	__libc_init_array();	// invoke constructors
    /* Branch to main function */
    main();

    /* Infinite loop */
	for (;;)
		;
}

/*------------------------------------------------------------------------------
 *         Exception Table
 *------------------------------------------------------------------------------*/


const
__attribute__((used, section(".vectors")))
IntFunc __Vectors [/*256*/] = {

    /* Configure Initial Stack Pointer, using linker-generated symbols */
    (IntFunc)(& __stack),
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    0, 0, 0, 0,         /* Reserved */
    SVC_Handler,
    DebugMon_Handler,
    0,                  /* Reserved  */
    PendSV_Handler,		/* -2 */
    SysTick_Handler,	/* -1 */

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S
	/* Configurable interrupts  */
	SUPC_Handler,   /* 0  Supply Controller */
	RSTC_Handler,   /* 1  Reset Controller */
	RTC_Handler,    /* 2  Real Time Clock */
	RTT_Handler,    /* 3  Real Time Timer */
	WDT_Handler,    /* 4  Watchdog Timer */
	PMC_Handler,    /* 5  PMC */
	EFC_Handler,    /* 6  EFC */
	Default_Handler,  /* 7  Reserved */
	UART0_Handler,  /* 8  UART0 */
	UART1_Handler,  /* 9  UART1 */
#ifdef ID_SMC
	SMC_Handler,    /* 10 SMC */
#else
	Default_Handler,
#endif
	PIOA_Handler,   /* 11 Parallel IO Controller A */
	PIOB_Handler,   /* 12 Parallel IO Controller B */
#ifdef ID_PIOC
	PIOC_Handler,   /* 13 Parallel IO Controller C */
#else
	Default_Handler,
#endif
	USART0_Handler, /* 14 USART 0 */
#ifdef ID_USART1
	USART1_Handler, /* 15 USART 1 */
#else
	Default_Handler,
#endif
	Default_Handler,  /* 16 Reserved */
	Default_Handler,  /* 17 Reserved */
#ifdef ID_HSMCI
	HSMCI_Handler,  /* 18 HSMCI */
#else
	Default_Handler,
#endif
	TWI0_Handler,   /* 19 TWI 0 */
	TWI1_Handler,   /* 20 TWI 1 */
	SPI_Handler,    /* 21 SPI */
	SSC_Handler,    /* 22 SSC */
	TC0_Handler,    /* 23 Timer Counter 0 */
	TC1_Handler,    /* 24 Timer Counter 1 */
	TC2_Handler,    /* 25 Timer Counter 2 */
#ifdef ID_TC3
	TC3_Handler,    /* 26 Timer Counter 3 */
#else
	Default_Handler,
#endif
#ifdef ID_TC4
	TC4_Handler,    /* 27 Timer Counter 4 */
#else
	Default_Handler,
#endif
#ifdef ID_TC5
	TC5_Handler,    /* 28 Timer Counter 5 */
#else
	Default_Handler,
#endif
	ADC_Handler,    /* 29 ADC controller */
#ifdef ID_DACC
	DACC_Handler,   /* 30 DACC controller */
#else
	Default_Handler,
#endif
	PWM_Handler,    /* 31 PWM */
	CRCCU_Handler,  /* 32 CRC Calculation Unit */
	ACC_Handler,    /* 33 Analog Comparator */
	UDP_Handler,    /* 34 USB Device Port */
	Default_Handler   /* 35 not used */

#elif CPUSTYLE_STM32F0XX

	#if defined (STM32F0XX_MD) || defined (STM32F030X8)
	/******  STM32F0XX_MD and STM32F030X8 specific Interrupt Numbers *************************************/
	  WWDG_IRQHandler,				// = 0,      /*!< Window WatchDog Interrupt                               */
	  PVD_IRQHandler,				// = 1,      /*!< PVD through EXTI Line detect Interrupt                  */
	  RTC_IRQHandler,				// = 2,      /*!< RTC through EXTI Line Interrupt                         */
	  FLASH_IRQHandler,				// = 3,      /*!< FLASH Interrupt                                         */
	  RCC_IRQHandler,				// = 4,      /*!< RCC Interrupt                                           */
	  EXTI0_1_IRQHandler,			// = 5,      /*!< EXTI Line 0 and 1 Interrupts                            */
	  EXTI2_3_IRQHandler,			// = 6,      /*!< EXTI Line 2 and 3 Interrupts                            */
	  EXTI4_15_IRQHandler,			// = 7,      /*!< EXTI Line 4 to 15 Interrupts                            */
	  TS_IRQHandler,				// = 8,      /*!< TS Interrupt                                            */
	  DMA1_Channel1_IRQHandler,		// = 9,      /*!< DMA1 Channel 1 Interrupt                                */
	  DMA1_Channel2_3_IRQHandler,	// = 10,     /*!< DMA1 Channel 2 and Channel 3 Interrupts                 */
	  DMA1_Channel4_5_IRQHandler,	// = 11,     /*!< DMA1 Channel 4 and Channel 5 Interrupts                 */
	  ADC1_COMP_IRQHandler,			// = 12,     /*!< ADC1, COMP1 and COMP2 Interrupts                        */
	  TIM1_BRK_UP_TRG_COM_IRQHandler,//= 13,     /*!< TIM1 Break, Update, Trigger and Commutation Interrupts  */
	  TIM1_CC_IRQHandler,			// = 14,     /*!< TIM1 Capture Compare Interrupt                          */
	  TIM2_IRQHandler,				// = 15,     /*!< TIM2 Interrupt                                          */
	  TIM3_IRQHandler,				// = 16,     /*!< TIM3 Interrupt                                          */
	  TIM6_DAC_IRQHandler,			// = 17,     /*!< TIM6 and DAC Interrupts                                 */
	  Default_Handler,				// = 18      /*!< xxx Interrupt                                           */
	  TIM14_IRQHandler,				// = 19,     /*!< TIM14 Interrupt                                         */
	  TIM15_IRQHandler,				// = 20,     /*!< TIM15 Interrupt                                         */
	  TIM16_IRQHandler,				// = 21,     /*!< TIM16 Interrupt                                         */
	  TIM17_IRQHandler,				// = 22,     /*!< TIM17 Interrupt                                         */
	  I2C1_IRQHandler,				// = 23,     /*!< I2C1 Interrupt                                          */
	  I2C2_IRQHandler,				// = 24,     /*!< I2C2 Interrupt                                          */
	  SPI1_IRQHandler,				// = 25,     /*!< SPI1 Interrupt                                          */
	  SPI2_IRQHandler,				// = 26,     /*!< SPI2 Interrupt                                          */
	  USART1_IRQHandler,			// = 27,     /*!< USART1 Interrupt                                        */
	  USART2_IRQHandler,			// = 28,     /*!< USART2 Interrupt                                        */
	  Default_Handler,				// = 29,     /*!< xxxxxx Interrupt                                        */
	  CEC_IRQHandler,				// = 30      /*!< CEC Interrupt                                           */
	#elif defined (STM32F0XX_LD) || defined (STM32F030X6)
	/******  STM32F0XX_LD and STM32F030X6 specific Interrupt Numbers *************************************/
	  WWDG_IRQHandler,				// = 0,      /*!< Window WatchDog Interrupt                               */
	  PVD_IRQHandler,				// = 1,      /*!< PVD through EXTI Line detect Interrupt                  */
	  RTC_IRQHandler,				// = 2,      /*!< RTC through EXTI Line Interrupt                         */
	  FLASH_IRQHandler,				// = 3,      /*!< FLASH Interrupt                                         */
	  RCC_IRQHandler,				// = 4,      /*!< RCC Interrupt                                           */
	  EXTI0_1_IRQHandler,			// = 5,      /*!< EXTI Line 0 and 1 Interrupts                            */
	  EXTI2_3_IRQHandler,			// = 6,      /*!< EXTI Line 2 and 3 Interrupts                            */
	  EXTI4_15_IRQHandler,			// = 7,      /*!< EXTI Line 4 to 15 Interrupts                            */
	  Default_Handler,				// = 8,      /*!< TS Interrupt                                            */
	  DMA1_Channel1_IRQHandler,		// = 9,      /*!< DMA1 Channel 1 Interrupt                                */
	  DMA1_Channel2_3_IRQHandler,	// = 10,     /*!< DMA1 Channel 2 and Channel 3 Interrupts                 */
	  DMA1_Channel4_5_IRQHandler,	// = 11,     /*!< DMA1 Channel 4 and Channel 5 Interrupts                 */
	  ADC1_IRQHandler,				// = 12,     /*!< ADC1 Interrupt                                          */
	  TIM1_BRK_UP_TRG_COM_IRQHandler,//= 13,     /*!< TIM1 Break, Update, Trigger and Commutation Interrupts  */
	  TIM1_CC_IRQHandler,			// = 14,     /*!< TIM1 Capture Compare Interrupt                          */
	  TIM2_IRQHandler,				// = 15,     /*!< TIM2 Interrupt                                          */
	  TIM3_IRQHandler,				// = 16,     /*!< TIM3 Interrupt                                          */
	  Default_Handler,				// = 17      /*!< xxx Interrupt                                           */
	  Default_Handler,				// = 18      /*!< xxx Interrupt                                           */
	  TIM14_IRQHandler,				// = 19,     /*!< TIM14 Interrupt                                         */
	  Default_Handler,				// = 20,     /*!< TIM15 Interrupt                                         */
	  TIM16_IRQHandler,				// = 21,     /*!< TIM16 Interrupt                                         */
	  TIM17_IRQHandler,				// = 22,     /*!< TIM17 Interrupt                                         */
	  I2C1_IRQHandler,				// = 23,     /*!< I2C1 Interrupt                                          */
	  Default_Handler,				// = 24,     /*!< I2C2 Interrupt                                          */
	  SPI1_IRQHandler,				// = 25,     /*!< SPI1 Interrupt                                          */
	  Default_Handler,				// = 26,     /*!< SPI2 Interrupt                                          */
	  USART1_IRQHandler,			// = 27,     /*!< USART1 Interrupt                                        */
	  Default_Handler,				// = 28,     /*!< USART2 Interrupt                                        */
	  Default_Handler,				// = 29,     /*!< xxxxxx Interrupt                                        */
	  Default_Handler,				// = 30      /*!< CEC Interrupt                                           */
	#endif /* STM32F0XX_MD or STM32F030X8 */

#elif CPUSTYLE_STM32L0XX
	/******  STM32L-0 specific Interrupt Numbers *********************************************************/
	WWDG_IRQHandler, //                   = 0,      /*!< Window WatchDog Interrupt                                     */
	PVD_IRQHandler, //                    = 1,      /*!< PVD through EXTI Line detect Interrupt                        */
	RTC_IRQHandler, //                    = 2,      /*!< RTC through EXTI Line Interrupt                               */
	FLASH_IRQHandler, //                  = 3,      /*!< FLASH Interrupt                                               */
	RCC_IRQHandler, //                    = 4,      /*!< RCC Interrupt                                                 */
	EXTI0_1_IRQHandler, //                = 5,      /*!< EXTI Line 0 and 1 Interrupts                                  */
	EXTI2_3_IRQHandler, //                = 6,      /*!< EXTI Line 2 and 3 Interrupts                                  */
	EXTI4_15_IRQHandler, //               = 7,      /*!< EXTI Line 4 to 15 Interrupts                                  */
	DMA1_Channel1_IRQHandler, //          = 9,      /*!< DMA1 Channel 1 Interrupt                                      */
	DMA1_Channel2_3_IRQHandler, //        = 10,     /*!< DMA1 Channel 2 and Channel 3 Interrupts                       */
	DMA1_Channel4_5_6_7_IRQHandler, //    = 11,     /*!< DMA1 Channel 4, Channel 5, Channel 6 and Channel 7 Interrupts */
	ADC1_COMP_IRQHandler, //              = 12,     /*!< ADC1, COMP1 and COMP2 Interrupts                              */
	LPTIM1_IRQHandler, //                 = 13,     /*!< LPTIM1 Interrupt                                              */
	TIM2_IRQHandler, //                   = 15,     /*!< TIM2 Interrupt                                                */
	TIM6_IRQHandler, //                   = 17,     /*!< TIM6  Interrupt                                               */
	TIM21_IRQHandler, //                  = 20,     /*!< TIM21 Interrupt                                               */
	TIM22_IRQHandler, //                  = 22,     /*!< TIM22 Interrupt                                               */
	I2C1_IRQHandler, //                   = 23,     /*!< I2C1 Interrupt                                                */
	I2C2_IRQHandler, //                   = 24,     /*!< I2C2 Interrupt                                                */
	SPI1_IRQHandler, //                   = 25,     /*!< SPI1 Interrupt                                                */
	SPI2_IRQHandler, //                   = 26,     /*!< SPI2 Interrupt                                                */
	USART1_IRQHandler, //                 = 27,     /*!< USART1 Interrupt                                              */
	USART2_IRQHandler, //                 = 28,     /*!< USART2 Interrupt                                              */
	LPUART1_IRQHandler, //                = 29,     /*!< LPUART1 Interrupts                                            */

#elif defined (STM32H743xx)

	Default_Handler,	//WWDG_IRQHandler           , /* Window WatchDog              */                                        
	Default_Handler,	//PVD_AVD_IRQHandler        , /* PVD/AVD through EXTI Line detection */                        
	Default_Handler,	//TAMP_STAMP_IRQHandler     , /* Tamper and TimeStamps through the EXTI line */            
	Default_Handler,	//RTC_WKUP_IRQHandler       , /* RTC Wakeup through the EXTI line */                      
	Default_Handler,	//FLASH_IRQHandler          , /* FLASH                        */                                          
	Default_Handler,	//RCC_IRQHandler            , /* RCC                          */                                            
	EXTI0_IRQHandler          , /* EXTI Line0                   */                        
	EXTI1_IRQHandler          , /* EXTI Line1                   */                          
	EXTI2_IRQHandler          , /* EXTI Line2                   */                          
	EXTI3_IRQHandler          , /* EXTI Line3                   */                          
	EXTI4_IRQHandler          , /* EXTI Line4                   */                          
	DMA1_Stream0_IRQHandler   , /* DMA1 Stream 0                */                  
	DMA1_Stream1_IRQHandler   , /* DMA1 Stream 1                */                   
	DMA1_Stream2_IRQHandler   , /* DMA1 Stream 2                */                   
	DMA1_Stream3_IRQHandler   , /* DMA1 Stream 3                */                   
	DMA1_Stream4_IRQHandler   , /* DMA1 Stream 4                */                   
	DMA1_Stream5_IRQHandler   , /* DMA1 Stream 5                */                   
	DMA1_Stream6_IRQHandler   , /* DMA1 Stream 6                */                   
	ADC_IRQHandler            , /* ADC1, ADC2 and ADC3s         */                   
	Default_Handler,	//FDCAN1_IT0_IRQHandler     , /* FDCAN1 interrupt line 0      */                    
	Default_Handler,	//FDCAN2_IT0_IRQHandler     , /* FDCAN2 interrupt line 0      */                    
	Default_Handler,	//FDCAN1_IT1_IRQHandler     , /* FDCAN1 interrupt line 1      */                    
	Default_Handler,	//FDCAN2_IT1_IRQHandler     , /* FDCAN2 interrupt line 1      */                    
	EXTI9_5_IRQHandler        , /* External Line[9:5]s          */                          
	TIM1_BRK_IRQHandler       , /* TIM1 Break interrupt         */         
	TIM1_UP_IRQHandler        , /* TIM1 Update interrupt        */         
	TIM1_TRG_COM_IRQHandler   , /* TIM1 Trigger and Commutation interrupt */
	TIM1_CC_IRQHandler        , /* TIM1 Capture Compare         */                          
	TIM2_IRQHandler           , /* TIM2                         */                   
	TIM3_IRQHandler           , /* TIM3                         */                   
	TIM4_IRQHandler           , /* TIM4                         */                   
	Default_Handler,	//I2C1_EV_IRQHandler        , /* I2C1 Event                   */                          
	Default_Handler,	//I2C1_ER_IRQHandler        , /* I2C1 Error                   */                          
	Default_Handler,	//I2C2_EV_IRQHandler        , /* I2C2 Event                   */                          
	Default_Handler,	//I2C2_ER_IRQHandler        , /* I2C2 Error                   */                            
	SPI1_IRQHandler           , /* SPI1                         */                   
	SPI2_IRQHandler           , /* SPI2                         */                   
	USART1_IRQHandler         , /* USART1                       */                   
	USART2_IRQHandler         , /* USART2                       */                   
	USART3_IRQHandler         , /* USART3                       */                   
	EXTI15_10_IRQHandler      , /* External Line[15:10]s        */                          
	Default_Handler,	//RTC_Alarm_IRQHandler      , /* RTC Alarm (A and B) through EXTI Line */                 
	0                         , /* Reserved                     */                       
	Default_Handler,	//TIM8_BRK_TIM12_IRQHandler , /* TIM8 Break and TIM12         */         
	Default_Handler,	//TIM8_UP_TIM13_IRQHandler  , /* TIM8 Update and TIM13        */         
	Default_Handler,	//TIM8_TRG_COM_TIM14_IRQHandler     /* TIM8 Trigger and Commutation and TIM14 */
	Default_Handler,	//TIM8_CC_IRQHandler        , /* TIM8 Capture Compare         */                          
	DMA1_Stream7_IRQHandler   , /* DMA1 Stream7                 */                          
	Default_Handler,	//FMC_IRQHandler            , /* FMC                          */                   
	SDMMC1_IRQHandler         , /* SDMMC1                       */                   
	TIM5_IRQHandler           , /* TIM5                         */                   
	SPI3_IRQHandler           , /* SPI3                         */                   
	UART4_IRQHandler          , /* UART4                        */                   
	UART5_IRQHandler          , /* UART5                        */                   
	Default_Handler,	//TIM6_DAC_IRQHandler       , /* TIM6 and DAC1&2 underrun errors */                   
	Default_Handler,	//TIM7_IRQHandler           , /* TIM7                         */
	DMA2_Stream0_IRQHandler   , /* DMA2 Stream 0                */                   
	DMA2_Stream1_IRQHandler   , /* DMA2 Stream 1                */                   
	DMA2_Stream2_IRQHandler   , /* DMA2 Stream 2                */                   
	DMA2_Stream3_IRQHandler   , /* DMA2 Stream 3                */                   
	DMA2_Stream4_IRQHandler   , /* DMA2 Stream 4                */                   
	Default_Handler,	//ETH_IRQHandler            , /* Ethernet                     */                   
	Default_Handler,	//ETH_WKUP_IRQHandler       , /* Ethernet Wakeup through EXTI line */                     
	Default_Handler,	//FDCAN_CAL_IRQHandler      , /* FDCAN calibration unit interrupt*/                                               
	0                         , /* Reserved                     */                          
	0                         , /* Reserved                     */                       
	0                         , /* Reserved                     */                           
	0                         , /* Reserved                     */                  
	DMA2_Stream5_IRQHandler   , /* DMA2 Stream 5                */                   
	DMA2_Stream6_IRQHandler   , /* DMA2 Stream 6                */                   
	DMA2_Stream7_IRQHandler   , /* DMA2 Stream 7                */                   
	USART6_IRQHandler         , /* USART6                       */                    
	Default_Handler,	//I2C3_EV_IRQHandler        , /* I2C3 event                   */                          
	Default_Handler,	//I2C3_ER_IRQHandler        , /* I2C3 error                   */                          
	Default_Handler,	//OTG_HS_EP1_OUT_IRQHandler , /* USB OTG HS End Point 1 Out   */                   
	Default_Handler,	//OTG_HS_EP1_IN_IRQHandler  , /* USB OTG HS End Point 1 In    */                   
	Default_Handler,	//OTG_HS_WKUP_IRQHandler    , /* USB OTG HS Wakeup through EXTI */                         
	OTG_HS_IRQHandler         , /* USB OTG HS                   */                   
	Default_Handler,	//DCMI_IRQHandler           , /* DCMI                         */                   
	0                         , /* Reserved                     */                   
	Default_Handler,	//RNG_IRQHandler            , /* Rng                          */
	Default_Handler,	//FPU_IRQHandler            , /* FPU                          */
	Default_Handler,	//UART7_IRQHandler          , /* UART7                        */      
	Default_Handler,	//UART8_IRQHandler          , /* UART8                        */
	Default_Handler,	//SPI4_IRQHandler           , /* SPI4                         */
	Default_Handler,	//SPI5_IRQHandler           , /* SPI5                         */
	Default_Handler,	//SPI6_IRQHandler           , /* SPI6                         */
	Default_Handler,	//SAI1_IRQHandler           , /* SAI1                         */
	Default_Handler,	//LTDC_IRQHandler           , /* LTDC                         */
	Default_Handler,	//LTDC_ER_IRQHandler        , /* LTDC error                   */
	Default_Handler,	//DMA2D_IRQHandler          , /* DMA2D                        */
	Default_Handler,	//SAI2_IRQHandler           , /* SAI2                         */
	Default_Handler,	//QUADSPI_IRQHandler        , /* QUADSPI                      */
	Default_Handler,	//LPTIM1_IRQHandler         , /* LPTIM1                       */
	Default_Handler,	//CEC_IRQHandler            , /* HDMI_CEC                     */
	Default_Handler,	//I2C4_EV_IRQHandler        , /* I2C4 Event                   */
	Default_Handler,	//I2C4_ER_IRQHandler        , /* I2C4 Error                   */
	Default_Handler,	//SPDIF_RX_IRQHandler       , /* SPDIF_RX                     */ 
	Default_Handler,	//OTG_FS_EP1_OUT_IRQHandler , /* USB OTG FS End Point 1 Out   */   
	Default_Handler,	//OTG_FS_EP1_IN_IRQHandler  , /* USB OTG FS End Point 1 In    */   
	Default_Handler,	//OTG_FS_WKUP_IRQHandler    , /* USB OTG FS Wakeup through EXTI */  
	OTG_FS_IRQHandler         , /* USB OTG FS                   */
	Default_Handler,	//DMAMUX1_OVR_IRQHandler    , /* DMAMUX1 Overrun interrupt    */  
	Default_Handler,	//HRTIM1_Master_IRQHandler  , /* HRTIM Master Timer global Interrupt */
	Default_Handler,	//HRTIM1_TIMA_IRQHandler    , /* HRTIM Timer A global Interrupt */  
	Default_Handler,	//HRTIM1_TIMB_IRQHandler    , /* HRTIM Timer B global Interrupt */  
	Default_Handler,	//HRTIM1_TIMC_IRQHandler    , /* HRTIM Timer C global Interrupt */  
	Default_Handler,	//HRTIM1_TIMD_IRQHandler    , /* HRTIM Timer D global Interrupt */  
	Default_Handler,	//HRTIM1_TIME_IRQHandler    , /* HRTIM Timer E global Interrupt */  
	Default_Handler,	//HRTIM1_FLT_IRQHandler     , /* HRTIM Fault global Interrupt   */ 
	Default_Handler,	//DFSDM1_FLT0_IRQHandler    , /* DFSDM Filter0 Interrupt        */
	Default_Handler,	//DFSDM1_FLT1_IRQHandler    , /* DFSDM Filter1 Interrupt        */     
	Default_Handler,	//DFSDM1_FLT2_IRQHandler    , /* DFSDM Filter2 Interrupt        */    
	Default_Handler,	//DFSDM1_FLT3_IRQHandler    , /* DFSDM Filter3 Interrupt        */   
	Default_Handler,	//SAI3_IRQHandler           , /* SAI3 global Interrupt          */   
	Default_Handler,	//SWPMI1_IRQHandler         , /* Serial Wire Interface 1 global interrupt */
	Default_Handler,	//TIM15_IRQHandler          , /* TIM15 global Interrupt      */        
	Default_Handler,	//TIM16_IRQHandler          , /* TIM16 global Interrupt      */      
	Default_Handler,	//TIM17_IRQHandler          , /* TIM17 global Interrupt      */      
	Default_Handler,	//MDIOS_WKUP_IRQHandler     , /* MDIOS Wakeup  Interrupt     */     
	Default_Handler,	//MDIOS_IRQHandler          , /* MDIOS global Interrupt      */    
	Default_Handler,	//JPEG_IRQHandler           , /* JPEG global Interrupt       */    
	Default_Handler,	//MDMA_IRQHandler           , /* MDMA global Interrupt       */    
	0                         , /* Reserved                    */    
	Default_Handler,	//SDMMC2_IRQHandler         , /* SDMMC2 global Interrupt     */    
	Default_Handler,	//HSEM1_IRQHandler          , /* HSEM1 global Interrupt      */    
	0                         , /* Reserved                    */    
	ADC3_IRQHandler           , /* ADC3 global Interrupt       */    
	Default_Handler,	//DMAMUX2_OVR_IRQHandler    , /* DMAMUX Overrun interrupt    */    
	Default_Handler,	//BDMA_Channel0_IRQHandler  , /* BDMA Channel 0 global Interrupt */
	Default_Handler,	//BDMA_Channel1_IRQHandler  , /* BDMA Channel 1 global Interrupt */ 
	Default_Handler,	//BDMA_Channel2_IRQHandler  , /* BDMA Channel 2 global Interrupt */ 
	Default_Handler,	//BDMA_Channel3_IRQHandler  , /* BDMA Channel 3 global Interrupt */ 
	Default_Handler,	//BDMA_Channel4_IRQHandler  , /* BDMA Channel 4 global Interrupt */ 
	Default_Handler,	//BDMA_Channel5_IRQHandler  , /* BDMA Channel 5 global Interrupt */ 
	Default_Handler,	//BDMA_Channel6_IRQHandler  , /* BDMA Channel 6 global Interrupt */ 
	Default_Handler,	//BDMA_Channel7_IRQHandler  , /* BDMA Channel 7 global Interrupt */ 
	Default_Handler,	//COMP1_IRQHandler          , /* COMP1 global Interrupt     */          
	Default_Handler,	//LPTIM2_IRQHandler         , /* LP TIM2 global interrupt   */      
	Default_Handler,	//LPTIM3_IRQHandler         , /* LP TIM3 global interrupt   */     
	Default_Handler,	//LPTIM4_IRQHandler         , /* LP TIM4 global interrupt   */     
	Default_Handler,	//LPTIM5_IRQHandler         , /* LP TIM5 global interrupt   */     
	Default_Handler,	//LPUART1_IRQHandler        , /* LP UART1 interrupt         */     
	0                         , /* Reserved                   */     
	Default_Handler,	//CRS_IRQHandler            , /* Clock Recovery Global Interrupt */ 
	0                         , /* Reserved                   */     
	Default_Handler,	//SAI4_IRQHandler           , /* SAI4 global interrupt      */      
	0                         , /* Reserved                   */      
	0                         , /* Reserved                   */      
	Default_Handler,	//WAKEUP_PIN_IRQHandler     , /* Interrupt for all 6 wake-up pins */

#elif CPUSTYLE_STM32F
	/* Device interrupt vectors */
	/* 0x40 */
	WWDG_IRQHandler,	// 0
	PVD_IRQHandler,		// 1
	TAMPER_IRQHandler,	// 2
	RTC_IRQHandler,		// 3
	FLASH_IRQHandler,	// 4
	RCC_IRQHandler,		// 5
	EXTI0_IRQHandler,	// 6
	EXTI1_IRQHandler,
	EXTI2_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,	// 10
#if defined (STM32F429xx) || defined(STM32F446xx) || defined(STM32F746xx) || defined(STM32F767xx) || defined(STM32F723xx) || defined (STM32F769xx)
	DMA1_Stream0_IRQHandler,	//  = 11,     /*!< DMA1 Stream 0 global Interrupt                                    */
	DMA1_Stream1_IRQHandler,	//  = 12,     /*!< DMA1 Stream 1 global Interrupt                                    */
	DMA1_Stream2_IRQHandler,	//  = 13,     /*!< DMA1 Stream 2 global Interrupt                                    */
	DMA1_Stream3_IRQHandler,	//  = 14,     /*!< DMA1 Stream 3 global Interrupt                                    */
	DMA1_Stream4_IRQHandler,	//  = 15,     /*!< DMA1 Stream 4 global Interrupt                                    */
	DMA1_Stream5_IRQHandler,	//  = 16,     /*!< DMA1 Stream 5 global Interrupt                                    */
	DMA1_Stream6_IRQHandler,	//  = 17,     /*!< DMA1 Stream 6 global Interrupt                                    */
#else /* defined (STM32F429xx) || defined(STM32F446xx) */
	DMA1_Channel1_IRQHandler,	// 11
	DMA1_Channel2_IRQHandler,
	DMA1_Channel3_IRQHandler,
	DMA1_Channel4_IRQHandler,	// 14
	DMA1_Channel5_IRQHandler,
	DMA1_Channel6_IRQHandler,
	DMA1_Channel7_IRQHandler,	// 17
#endif /* defined (STM32F429xx) || defined(STM32F446xx) */

	ADC1_2_IRQHandler,			// 18

	/* 0x8C */
	USB_HP_CAN1_TX_IRQHandler,	// 19
	CAN1_RX0_IRQHandler,
	CAN1_RX1_IRQHandler,
	CAN1_SCE_IRQHandler,	// 22
	/* 0x9C */
	EXTI9_5_IRQHandler,	// 23
	TIM1_BRK_IRQHandler,	// 24
	TIM1_UP_IRQHandler,		// 25
	TIM1_TRG_COM_IRQHandler,	// 26
	TIM1_CC_IRQHandler,			// 27
	TIM2_IRQHandler,			// 28
	TIM3_IRQHandler,			// 29

	TIM4_IRQHandler,	// 30

	/* 0xBC */
	I2C1_EV_IRQHandler,		// 31
	I2C1_ER_IRQHandler,		// 32

	I2C2_EV_IRQHandler,		// 33
	I2C2_ER_IRQHandler,		// 34
	SPI1_IRQHandler,		// 35
	SPI2_IRQHandler,		// 36

	/* 0xD4 */
	USART1_IRQHandler,		// 37
	USART2_IRQHandler,		// 38
	USART3_IRQHandler,		// 39

	EXTI15_10_IRQHandler,	// 40
	RTCAlarm_IRQHandler,	// 41
	USBWakeUp_IRQHandler,

	/* 0xEC */
	NULL, 
	/* 0xF0 */
	NULL, 
	/* 0xF4 */
	NULL, 
	/* 0xF8 */
	NULL,

	/* 0xFC */
	NULL,

#if defined (STM32F429xx) || defined(STM32F446xx) || defined(STM32F746xx) || defined(STM32F767xx) || defined(STM32F723xx) || defined (STM32F769xx)
	NULL,
	SDIO_IRQHandler,		// 49 - SDIO_IRQHandler or SDMMC1_IRQHandler
#else
	/* STM32F10X_LD_VL, STM32F10X_MD_VL, STM32F10X_LD, STM32F10X_MD, STM32F10X_CL */
	NULL, NULL,			// 48, 49
#endif

	TIM5_IRQHandler,	// 50
	SPI3_IRQHandler,	// 51
	UART4_IRQHandler,	// 52
	UART5_IRQHandler,	// 53

	/* 0x118 */
#if   defined(STM32F10X_LD_VL) || defined(STM32F10X_MD_VL) || defined(STM32F10X_HD_VL)
	TIM6_IRQHandler,	// 54
	TIM7_IRQHandler,	// 55
#else
	NULL,	// 54
	NULL,	// 55
#endif

	/* 0x120 */
#if defined (STM32F429xx) || defined(STM32F446xx) || defined(STM32F746xx) || defined(STM32F767xx) || defined(STM32F401xC) || defined(STM32F407xx) || defined(STM32F723xx) || defined (STM32F769xx)
	DMA2_Stream0_IRQHandler,	// 56
	DMA2_Stream1_IRQHandler,
	DMA2_Stream2_IRQHandler,
	DMA2_Stream3_IRQHandler,
	DMA2_Stream4_IRQHandler,	// 60
#elif   defined(STM32F10X_HD_VL)
	DMA2_Channel1_IRQHandler,
	DMA2_Channel2_IRQHandler,
	DMA2_Channel3_IRQHandler,
	DMA2_Channel4_5_IRQHandler,
	DMA2_Channel5_IRQHandler,
#elif defined(STM32F10X_CL)
	DMA2_Channel1_IRQHandler,
	DMA2_Channel2_IRQHandler,
	DMA2_Channel3_IRQHandler,
	DMA2_Channel4_IRQHandler,
	DMA2_Channel5_IRQHandler,
#elif defined(STM32F10X_HD) || defined(STM32F10X_XL)
	DMA2_Channel1_IRQHandler,	// 56
	DMA2_Channel2_IRQHandler,
	DMA2_Channel3_IRQHandler,
	DMA2_Channel4_5_IRQHandler,
	NULL,						// 60
#endif

	/* 0x134 */
#if defined (STM32F429xx) || defined(STM32F446xx) || defined(STM32F746xx) || defined(STM32F767xx) || defined(STM32F401xC) || defined(STM32F407xx) || defined(STM32F723xx) || defined (STM32F769xx)
  Default_Handler,	//ETH_IRQHandler,		/*!< Ethernet global Interrupt   61                                      */
  Default_Handler,	//ETH_WKUP_IRQHandler,		/*!< Ethernet Wakeup through EXTI line Interrupt                       */
  Default_Handler,	//CAN2_TX_IRQHandler,		/*!< CAN2 TX Interrupt                                                 */
  Default_Handler,	//CAN2_RX0_IRQHandler,		/*!< CAN2 RX0 Interrupt                                                */
  Default_Handler,	//CAN2_RX1_IRQHandler,		/*!< CAN2 RX1 Interrupt                                                */
  Default_Handler,	//CAN2_SCE_IRQHandler,		/*!< CAN2 SCE Interrupt                                                */
  OTG_FS_IRQHandler,			/*!< USB OTG FS global Interrupt                                       */
  DMA2_Stream5_IRQHandler,		/*!< DMA2 Stream 5 global interrupt                                    */
  DMA2_Stream6_IRQHandler,		/*!< DMA2 Stream 6 global interrupt                                    */
  DMA2_Stream7_IRQHandler,		/*!< DMA2 Stream 7 global interrupt                                    */
  USART6_IRQHandler,		/*!< USART6 global interrupt                                           */
  Default_Handler,	//I2C3_EV_IRQHandler,		/*!< I2C3 event interrupt                                              */
  Default_Handler,	//I2C3_ER_IRQHandler,		/*!< I2C3 error interrupt                                              */
  OTG_HS_EP1_OUT_IRQHandler,		/*!< USB OTG HS End Point 1 Out global interrupt                       */
  OTG_HS_EP1_IN_IRQHandler,		/*!< USB OTG HS End Point 1 In global interrupt                        */
  OTG_HS_WKUP_IRQHandler,		/*!< USB OTG HS Wakeup through EXTI interrupt                          */
  OTG_HS_IRQHandler,		/*!< USB OTG HS global interrupt                                       */
  Default_Handler,	//DCMI_IRQHandler,		/*!< DCMI global interrupt                                             */
  Default_Handler,	//CRYP_IRQHandler,		/*!< CRYP crypto global interrupt                                      */
  Default_Handler,	//HASH_RNG_IRQHandler,		/*!< Hash and Rng global interrupt                                     */
  Default_Handler,	//FPU_IRQHandler,		/*!< FPU global interrupt                                              */
  Default_Handler,	//UART7_IRQHandler,		/*!< UART7 global interrupt                                            */
  Default_Handler,	//UART8_IRQHandler,		/*!< UART8 global interrupt                                            */
  Default_Handler,	//SPI4_IRQHandler,		/*!< SPI4 global Interrupt                                             */
  Default_Handler,	//Default_Handler,	//SPI5_IRQHandler,		/*!< SPI5 global Interrupt                                             */
  Default_Handler,	//SPI6_IRQHandler,		/*!< SPI6 global Interrupt                                             */
  Default_Handler,	//SAI1_IRQHandler,		/*!< SAI1 global Interrupt                                             */
  Default_Handler,	//LTDC_IRQHandler,		/*!< LTDC global Interrupt                                             */
  Default_Handler,	//LTDC_ER_IRQHandler,		/*!< LTDC Error global Interrupt                                       */
  Default_Handler,	//DMA2D_IRQHandler,		/*!< DMA2D global Interrupt     90                                       */   

#elif   defined(STM32F10X_CL)
	ETH_IRQHandler,
	ETH_WKUP_IRQHandler,
	CAN2_TX_IRQHandler,
	CAN2_RX0_IRQHandler,
	CAN2_RX1_IRQHandler,
	CAN2_SCE_IRQHandler,
	OTG_FS_IRQHandler,
	/* 0x150 */
	0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0,
	0,0,0,0,
	0, //(void (*)(void))0xF1E0F85F /* @0x1E0 */
#elif defined(STM32F10X_LD_VL) || defined(STM32F10X_MD_VL)
	0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0,
	0,0,0,
	0, //(void (*)(void))0xF108F85F /* @0x01CC */
#elif defined(STM32F10X_HD_VL)
	0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0,
	0,0,0
	0, //(void (*)(void))0xF108F85F /* @0x1E0 */
#elif defined(STM32F10X_HD) || defined(STM32F10X_XL)
	0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0,
	0,0,0,0,
	0, //(void (*)(void))0xF1E0F85F  /* @0x1E0 */
#endif


#endif
};


#endif /* CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM0 || CPUSTYLE_ARM_CM7 */


// hack for eliminate exception handling unwinding code
//
//char __attribute__ ((used)) __aeabi_unwind_cpp_pr0 [0];
