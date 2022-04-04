/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#include "board.h"
#include "gpio.h"
#include "clocks.h"
#include "spi.h"
#include "formats.h"	// for debug prints


uint_fast32_t
calcdivround2(
	uint_fast32_t ref,	/* частота на входе делителя, в герцах. */
	uint_fast32_t freq	/* требуемая частота на выходе делителя, в герцах. */
	)
{
	return (ref < freq) ? 1 : ((ref + freq / 2) / freq);
}

#if defined(STM32F401xC)


#elif CPUSTYLE_STM32F4XX

unsigned long stm32f4xx_get_hse_freq(void)
{
#if WITHCPUXTAL
	return WITHCPUXTAL;
#elif WITHCPUXOSC
	return WITHCPUXOSC;
#else
	#warning WITHCPUXOSC or WITHCPUXTAL should be defined
	return 16000000uL;
#endif
}

unsigned long stm32f4xx_get_hsi_freq(void)
{
	return HSIFREQ;	// 16 MHz
}

unsigned long stm32f4xx_get_pllreference_freq(void)
{
	const uint_fast32_t divval = (RCC->PLLCFGR & RCC_PLLCFGR_PLLM_Msk) >> RCC_PLLCFGR_PLLM_Pos;
	return stm32f4xx_get_hse_freq() / divval;
}

unsigned long stm32f4xx_get_pll_freq(void)
{
	const uint_fast32_t mulval = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN_Msk) >> RCC_PLLCFGR_PLLN_Pos;
	return stm32f4xx_get_pllreference_freq() * mulval;
}

unsigned long stm32f4xx_get_pll_p_freq(void)
{
	const uint_fast32_t divval = (RCC->PLLCFGR & RCC_PLLCFGR_PLLP_Msk) >> RCC_PLLCFGR_PLLP_Pos;
	return stm32f4xx_get_pll_freq() / divval;
}

unsigned long stm32f4xx_get_pll_q_freq(void)
{
	const uint_fast32_t divval = (RCC->PLLCFGR & RCC_PLLCFGR_PLLQ_Msk) >> RCC_PLLCFGR_PLLQ_Pos;
	return stm32f4xx_get_pll_freq() / divval;
}

unsigned long stm32f4xx_get_plli2s_freq(void)
{
	const uint_fast32_t mulval = (RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SN_Msk) >> RCC_PLLI2SCFGR_PLLI2SN_Pos;
	return stm32f4xx_get_pllreference_freq() * mulval;
}

unsigned long stm32f4xx_get_plli2s_q_freq(void)
{
	const uint_fast32_t divval = (RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SQ_Msk) >> RCC_PLLI2SCFGR_PLLI2SQ_Pos;
	return stm32f4xx_get_plli2s_freq() / divval;
}

unsigned long stm32f4xx_get_plli2s_r_freq(void)
{
	const uint_fast32_t divval = (RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SR_Msk) >> RCC_PLLI2SCFGR_PLLI2SR_Pos;
	return stm32f4xx_get_plli2s_freq() / divval;
}

unsigned long stm32f4xx_get_pllsai_freq(void)
{
	const uint_fast32_t mulval = (RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIN_Msk) >> RCC_PLLSAICFGR_PLLSAIN_Pos;
	return stm32f4xx_get_pllreference_freq() * mulval;
}

unsigned long stm32f4xx_get_pllsai_q_freq(void)
{
	const uint_fast32_t divval = (RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIQ_Msk) >> RCC_PLLSAICFGR_PLLSAIQ_Pos;
	return stm32f4xx_get_pllsai_freq() / divval;
}

unsigned long stm32f4xx_get_pllsai_r_freq(void)
{
	const uint_fast32_t divval = (RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIR_Msk) >> RCC_PLLSAICFGR_PLLSAIR_Pos;
	return stm32f4xx_get_pllsai_freq() / divval;
}

unsigned long stm32f4xx_get_sysclk_freq(void)
{
	//	00: HSI oscillator used as the system clock
	//	01: HSE oscillator used as the system clock
	//	10: PLL used as the system clock
	//	11: not applicable
	switch ((RCC->CFGR & RCC_CFGR_SW_Msk) >> RCC_CFGR_SW_Pos)
	{
	default:
	case 0x00:
		return stm32f4xx_get_hsi_freq();
	case 0x01:
		return stm32f4xx_get_hse_freq();
	case 0x02:
		return stm32f4xx_get_pll_p_freq();
	}
}

unsigned long stm32f4xx_get_ahb_freq(void)
{
	//	0xxx: system clock not divided
	//	1000: system clock divided by 2
	//	1001: system clock divided by 4
	//	1010: system clock divided by 8
	//	1011: system clock divided by 16
	//	1100: system clock divided by 64
	//	1101: system clock divided by 128
	//	1110: system clock divided by 256
	//	1111: system clock divided by 512
	switch ((RCC->CFGR & RCC_CFGR_HPRE_Msk) >> RCC_CFGR_HPRE_Pos)
	{
	default:
	case 0x00:
		return stm32f4xx_get_sysclk_freq();
	case 0x08:
		return stm32f4xx_get_sysclk_freq() / 2;
	case 0x09:
		return stm32f4xx_get_sysclk_freq() / 4;
	case 0x0A:
		return stm32f4xx_get_sysclk_freq() / 8;
	case 0x0B:
		return stm32f4xx_get_sysclk_freq() / 16;
	case 0x0C:
		return stm32f4xx_get_sysclk_freq() / 64;
	case 0x0D:
		return stm32f4xx_get_sysclk_freq() / 128;
	case 0x0E:
		return stm32f4xx_get_sysclk_freq() / 256;
	case 0x0F:
		return stm32f4xx_get_sysclk_freq() / 512;
	}
}

unsigned long stm32f4xx_get_hclk_freq(void)
{
	return stm32f4xx_get_ahb_freq();
}

// PPRE2: APB high-speed prescaler (APB2)
unsigned long stm32f4xx_get_apb2_freq(void)
{
	const uint_fast32_t ppre2 = (RCC->CFGR & RCC_CFGR_PPRE2_Msk) >> RCC_CFGR_PPRE2_Pos;
	switch (ppre2)
	{
	default:
	case 0x00:
		return stm32f4xx_get_ahb_freq();
	case 0x04:
		return stm32f4xx_get_ahb_freq() / 2;
	case 0x05:
		return stm32f4xx_get_ahb_freq() / 4;
	case 0x06:
		return stm32f4xx_get_ahb_freq() / 8;
	case 0x07:
		return stm32f4xx_get_ahb_freq() / 16;
	}
}

// PPRE1: APB Low speed prescaler (APB1)
unsigned long stm32f4xx_get_apb1_freq(void)
{
	const uint_fast32_t ppre1 = (RCC->CFGR & RCC_CFGR_PPRE1_Msk) >> RCC_CFGR_PPRE1_Pos;
	switch (ppre1)
	{
	default:
	case 0x00:
		return stm32f4xx_get_ahb_freq();
	case 0x04:
		return stm32f4xx_get_ahb_freq() / 2;
	case 0x05:
		return stm32f4xx_get_ahb_freq() / 4;
	case 0x06:
		return stm32f4xx_get_ahb_freq() / 8;
	case 0x07:
		return stm32f4xx_get_ahb_freq() / 16;
	}
}
// PPRE2: APB high-speed prescaler (APB2)
unsigned long stm32f4xx_get_apb2timer_freq(void)
{
	const uint_fast32_t timpre  = (RCC->DCKCFGR & RCC_DCKCFGR_TIMPRE_Msk) >> RCC_DCKCFGR_TIMPRE_Pos;
	const uint_fast32_t ppre2 = (RCC->CFGR & RCC_CFGR_PPRE2_Msk) >> RCC_CFGR_PPRE2_Pos;
	//	0: If the APB prescaler (PPRE1, PPRE2 in the RCC_CFGR register) is configured to a
	//	division factor of 1, TIMxCLK = PCLKx. Otherwise, the timer clock frequencies are set to
	//	twice to the frequency of the APB domain to which the timers are connected:
	//	TIMxCLK = 2xPCLKx.
	//	1:If the APB prescaler (PPRE1, PPRE2 in the RCC_CFGR register) is configured to a
	//	division factor of 1, 2 or 4, TIMxCLK = HCLK. Otherwise, the timer clock frequencies are set
	//	to four times to the frequency of the APB domain to which the timers are connected:
	//	TIMxCLK = 4xPCLKx.
	switch (ppre2)
	{
	default:
	case 0x00:	// /1
		return timpre ? stm32f4xx_get_hclk_freq() : stm32f4xx_get_ahb_freq();
	case 0x04:	// /2
		return timpre ? stm32f4xx_get_hclk_freq() : stm32f4xx_get_ahb_freq();
	case 0x05:	// /4
		return timpre ? stm32f4xx_get_hclk_freq() : stm32f4xx_get_ahb_freq() / 2;
	case 0x06:	// /8
		return timpre ? stm32f4xx_get_ahb_freq() / 2 : stm32f4xx_get_ahb_freq() / 4;
	case 0x07:	// /16
		return timpre ? stm32f4xx_get_ahb_freq() / 4 : stm32f4xx_get_ahb_freq() / 8;
	}
}

// PPRE1: APB Low speed prescaler (APB1)
unsigned long stm32f4xx_get_apb1timer_freq(void)
{
	const uint_fast32_t timpre  = (RCC->DCKCFGR & RCC_DCKCFGR_TIMPRE_Msk) >> RCC_DCKCFGR_TIMPRE_Pos;
	const uint_fast32_t ppre1 = (RCC->CFGR & RCC_CFGR_PPRE1_Msk) >> RCC_CFGR_PPRE1_Pos;
	//	0: If the APB prescaler (PPRE1, PPRE2 in the RCC_CFGR register) is configured to a
	//	division factor of 1, TIMxCLK = PCLKx. Otherwise, the timer clock frequencies are set to
	//	twice to the frequency of the APB domain to which the timers are connected:
	//	TIMxCLK = 2xPCLKx.
	//	1:If the APB prescaler (PPRE1, PPRE2 in the RCC_CFGR register) is configured to a
	//	division factor of 1, 2 or 4, TIMxCLK = HCLK. Otherwise, the timer clock frequencies are set
	//	to four times to the frequency of the APB domain to which the timers are connected:
	//	TIMxCLK = 4xPCLKx.
	switch (ppre1)
	{
	default:
	case 0x00:	// /1
		return timpre ? stm32f4xx_get_hclk_freq() : stm32f4xx_get_ahb_freq();
	case 0x04:	// /2
		return timpre ? stm32f4xx_get_hclk_freq() : stm32f4xx_get_ahb_freq();
	case 0x05:	// /4
		return timpre ? stm32f4xx_get_hclk_freq() : stm32f4xx_get_ahb_freq() / 2;
	case 0x06:	// /8
		return timpre ? stm32f4xx_get_ahb_freq() / 2 : stm32f4xx_get_ahb_freq() / 4;
	case 0x07:	// /16
		return timpre ? stm32f4xx_get_ahb_freq() / 4 : stm32f4xx_get_ahb_freq() / 8;
	}
}

unsigned long stm32f4xx_get_spi1_freq(void)
{
	return stm32f4xx_get_apb2_freq();
}

unsigned long stm32f4xx_get_tim3_freq(void)
{
	return stm32f4xx_get_apb1timer_freq();
}

#define BOARD_SYSTICK_FREQ (stm32f4xx_get_sysclk_freq() / 1)	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора
#define BOARD_TIM3_FREQ (stm32f4xx_get_tim3_freq())
#define BOARD_ADC_FREQ (stm32f4xx_get_apb2_freq())
#define BOARD_USART1_FREQ (stm32f4xx_get_apb1_freq())	// TODO: verify

#elif CPUSTYLE_STM32F7XX


/* частоты, подающиеся на периферию */
//#define	PCLK1_FREQ (CPU_FREQ / 4)	// 42 MHz PCLK1 frequency
//#define	PCLK1_TIMERS_FREQ (CPU_FREQ / 4)	// 42 MHz PCLK1 frequency
//#define	PCLK2_FREQ (CPU_FREQ / 2)	// 84 MHz PCLK2 frequency
/* проверить функцию stm32f7xx_get_sysclk_freq */
#define BOARD_SYSTICK_FREQ (stm32f7xx_get_sys_freq() / 1)	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора
//#define BOARD_SYSTICK_FREQ (stm32f7xx_get_sysclk_freq() / 1)	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

#define BOARD_USART1_FREQ (stm32f7xx_get_usart1_freq())
#define BOARD_TIM3_FREQ (stm32f7xx_get_apb1_tim_freq())	// TODO: verify
#define BOARD_ADC_FREQ (stm32f7xx_get_apb2_freq())
#define BOARD_USART2_FREQ 	(stm32f7xx_get_apb1_freq())

unsigned long stm32f7xx_get_hse_freq(void)
{
#if WITHCPUXTAL
	return WITHCPUXTAL;
#elif WITHCPUXOSC
	return WITHCPUXOSC;
#else
	#warning WITHCPUXOSC or WITHCPUXTAL should be defined
	return 24000000uL;
#endif
}

unsigned long stm32f7xx_get_pll_freq(void)
{
	const unsigned long pllm = (RCC->PLLCFGR & RCC_PLLCFGR_PLLM_Msk) >> RCC_PLLCFGR_PLLM_Pos;
	const unsigned long plln = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN_Msk) >> RCC_PLLCFGR_PLLN_Pos;

	return (uint_fast64_t) REFINFREQ * plln / pllm;
}

unsigned long stm32f7xx_get_pllsai_freq(void)
{
	const unsigned long pllm = (RCC->PLLCFGR & RCC_PLLCFGR_PLLM_Msk) >> RCC_PLLCFGR_PLLM_Pos;
	const unsigned long pllsain = (RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIN_Msk) >> RCC_PLLSAICFGR_PLLSAIN_Pos;

	return (uint_fast64_t) REFINFREQ * pllsain / pllm;
}

unsigned long stm32f7xx_get_plli2s_freq(void)
{
	const unsigned long pllm = (RCC->PLLCFGR & RCC_PLLCFGR_PLLM_Msk) >> RCC_PLLCFGR_PLLM_Pos;
	const unsigned long plli2sn = (RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SN_Msk) >> RCC_PLLI2SCFGR_PLLI2SN_Pos;

	return (uint_fast64_t) REFINFREQ * plli2sn / pllm;
}

unsigned long stm32f7xx_get_pll_p_freq(void)
{
	const unsigned long pll = stm32f7xx_get_pll_freq();
	switch ((RCC->PLLCFGR & RCC_PLLCFGR_PLLP_Msk) >> RCC_PLLCFGR_PLLP_Pos)
	{
	default:
	case 0x00: return pll / 2;
	case 0x01: return pll / 4;
	case 0x02: return pll / 6;
	case 0x03: return pll / 8;
	}
}

// PLLCLK=PLL_P
// HSI/HSE/PLLCLK
// SWS
unsigned long stm32f7xx_get_sys_freq(void)
{
	//	00: HSI oscillator used as the system clock
	//	01: HSE oscillator used as the system clock
	//	10: PLL used as the
	switch ((RCC->CFGR & RCC_CFGR_SWS_Msk) >> RCC_CFGR_SWS_Pos)
	{
	default:
	case 0x00: return HSIFREQ;
	case 0x01: return stm32f7xx_get_hse_freq();
	case 0x02: return stm32f7xx_get_pll_p_freq();
	}
}

// TODO: check
//unsigned long stm32f7xx_get_sysclk_freq(void)
//{
//	return stm32f7xx_get_sys_freq();
//}

// AHB prescaler
// HPRE output
unsigned long stm32f7xx_get_ahb_freq(void)
{
	//	0xxx: system clock not divided
	//	1000: system clock divided by 2
	//	1001: system clock divided by 4
	//	1010: system clock divided by 8
	//	1011: system clock divided by 16
	//	1100: system clock divided by 64
	//	1101: system clock divided by 128
	//	1110: system clock divided by 256
	//	1111: system clock divided by 512
	const unsigned long sys = stm32f7xx_get_sys_freq();
	switch ((RCC->CFGR & RCC_CFGR_HPRE_Msk) >> RCC_CFGR_HPRE_Pos)
	{
	default: return sys;
	case 0x08: return sys / 2;
	case 0x09: return sys / 4;
	case 0x0A: return sys / 8;
	case 0x0B: return sys / 16;
	case 0x0C: return sys / 64;
	case 0x0D: return sys / 128;
	case 0x0E: return sys / 256;
	case 0x0F: return sys / 512;
	}
}

// TODO: проверить
// APB Low-speed prescaler (APB1)
// PPRE1 output
unsigned long stm32f7xx_get_apb1_freq(void)
{
	//	0xx: AHB clock not divided
	//	100: AHB clock divided by 2
	//	101: AHB clock divided by 4
	//	110: AHB clock divided by 8
	//	111: AHB clock divided by 16
	const unsigned long ahb = stm32f7xx_get_ahb_freq();
	switch ((RCC->CFGR & RCC_CFGR_PPRE1_Msk) >> RCC_CFGR_PPRE1_Pos)
	{
	default: return ahb;
	case 0x04: return ahb / 2;
	case 0x05: return ahb / 4;
	case 0x06: return ahb / 8;
	case 0x07: return ahb / 16;
	}
}

// TODO: проверить
unsigned long stm32f7xx_get_apb1_tim_freq(void)
{
	const unsigned long sysclk = stm32f7xx_get_sys_freq();
	const uint8_t timpre = (RCC->DCKCFGR1 & RCC_DCKCFGR1_TIMPRE_Msk) != 0;

	// timpre 0:
	//	If the APB prescaler (PPRE1, PPRE2 in the RCC_CFGR register) is configured to a
	//	division factor of 1, TIMxCLK = PCLKx. Otherwise, the timer clock frequencies are set to
	//	twice to the frequency of the APB domain to which the timers are connected:
	//	TIMxCLK = 2xPCLKx.

	// timpre 1:
	//	If the APB prescaler (PPRE1, PPRE2 in the RCC_CFGR register) is configured to a
	//	division factor of 1, 2 or 4, TIMxCLK = HCLK. Otherwise, the timer clock frequencies are set
	//	to four times to the frequency of the APB domain to which the timers are connected:
	//	TIMxCLK = 4xPCLKx.

	const unsigned long ahb = stm32f7xx_get_ahb_freq();
	switch ((RCC->CFGR & RCC_CFGR_PPRE1_Msk) >> RCC_CFGR_PPRE1_Pos)
	{
	default: return ahb;
	case 0x04: return timpre ? ahb / 1 : ahb / 2;
	case 0x05: return timpre ? ahb / 2 : ahb / 4;
	case 0x06: return timpre ? ahb / 4 : ahb / 8;
	case 0x07: return timpre ? ahb / 8 : ahb / 16;
	}
}

// TODO: проверить
// APB high-speed prescaler (APB2)
// PPRE2 output
unsigned long stm32f7xx_get_apb2_freq(void)
{
	//	0xx: AHB clock not divided
	//	100: AHB clock divided by 2
	//	101: AHB clock divided by 4
	//	110: AHB clock divided by 8
	//	111: AHB clock divided by 16
	const unsigned long ahb = stm32f7xx_get_ahb_freq();
	switch ((RCC->CFGR & RCC_CFGR_PPRE2_Msk) >> RCC_CFGR_PPRE2_Pos)
	{
	default: return ahb;
	case 0x04: return ahb / 2;
	case 0x05: return ahb / 4;
	case 0x06: return ahb / 8;
	case 0x07: return ahb / 16;
	}
}

// TODO: проверить
unsigned long stm32f7xx_get_apb2_tim_freq(void)
{
	const unsigned long sysclk = stm32f7xx_get_sys_freq();
	const uint8_t timpre = (RCC->DCKCFGR1 & RCC_DCKCFGR1_TIMPRE_Msk) != 0;

	// timpre 0:
	//	If the APB prescaler (PPRE1, PPRE2 in the RCC_CFGR register) is configured to a
	//	division factor of 1, TIMxCLK = PCLKx. Otherwise, the timer clock frequencies are set to
	//	twice to the frequency of the APB domain to which the timers are connected:
	//	TIMxCLK = 2xPCLKx.

	// timpre 1:
	//	If the APB prescaler (PPRE1, PPRE2 in the RCC_CFGR register) is configured to a
	//	division factor of 1, 2 or 4, TIMxCLK = HCLK. Otherwise, the timer clock frequencies are set
	//	to four times to the frequency of the APB domain to which the timers are connected:
	//	TIMxCLK = 4xPCLKx.

	const unsigned long ahb = stm32f7xx_get_ahb_freq();
	switch ((RCC->CFGR & RCC_CFGR_PPRE2_Msk) >> RCC_CFGR_PPRE2_Pos)
	{
	default: return ahb;
	case 0x04: return timpre ? ahb / 1 : ahb / 2;
	case 0x05: return timpre ? ahb / 2 : ahb / 4;
	case 0x06: return timpre ? ahb / 4 : ahb / 8;
	case 0x07: return timpre ? ahb / 8 : ahb / 16;
	}
}

// TODO: проверить
unsigned long stm32f7xx_get_pclk1_freq(void)
{
	return hardware_get_apb1_freq();
}

// TODO: проверить
unsigned long stm32f7xx_get_pclk2_freq(void)
{
	return hardware_get_apb2_freq();
}

// TODO: проверить
// получение тактовой частоты тактирования блока SPI, использующенося в данной конфигурации
unsigned long hardware_get_spi_freq(void)
{
	const unsigned long sysclk = stm32f7xx_get_sys_freq();
	return sysclk / 4;
}

unsigned long stm32f7xx_get_usart1_freq(void)
{
	//	00: APB2 clock (PCLK2) is selected as USART 1 clock
	//	01: System clock is selected as USART 1 clock
	//	10: HSI clock is selected as USART 1 clock
	//	11: LSE clock is selected as USART 1 clock
	switch ((RCC->DCKCFGR2 & RCC_DCKCFGR2_USART1SEL_Msk) >> RCC_DCKCFGR2_USART1SEL_Pos)
	{
	default:
	case 0x00: return stm32f7xx_get_pclk2_freq();
	case 0x01: return stm32f7xx_get_sys_freq();
	case 0x02: return HSIFREQ;
	case 0x03: return LSEFREQ;
	}
}

unsigned long stm32f7xx_get_usart2_freq(void)
{
	//	00: APB1 clock (PCLK1) is selected as USART 2 clock
	//	01: System clock is selected as USART 2 clock
	//	10: HSI clock is selected as USART 2 clock
	//	11: LSE clock is selected as USART 2 clock
	switch ((RCC->DCKCFGR2 & RCC_DCKCFGR2_USART2SEL_Msk) >> RCC_DCKCFGR2_USART2SEL_Pos)
	{
	default:
	case 0x00: return stm32f7xx_get_pclk1_freq();
	case 0x01: return stm32f7xx_get_sys_freq();
	case 0x02: return HSIFREQ;
	case 0x03: return LSEFREQ;
	}
}

unsigned long stm32f7xx_get_usart3_freq(void)
{
	//	00: APB1 clock (PCLK1) is selected as USART 3 clock
	//	01: System clock is selected as USART 3 clock
	//	10: HSI clock is selected as USART 3 clock
	//	11: LSE clock is selected as USART 3 clock
	switch ((RCC->DCKCFGR2 & RCC_DCKCFGR2_USART3SEL_Msk) >> RCC_DCKCFGR2_USART3SEL_Pos)
	{
	default:
	case 0x00: return stm32f7xx_get_pclk1_freq();
	case 0x01: return stm32f7xx_get_sys_freq();
	case 0x02: return HSIFREQ;
	case 0x03: return LSEFREQ;
	}
}

unsigned long stm32f7xx_get_uart4_freq(void)
{
	//	00: APB1 clock (PCLK1) is selected as USART 4 clock
	//	01: System clock is selected as USART 4 clock
	//	10: HSI clock is selected as USART 4 clock
	//	11: LSE clock is selected as USART 4 clock
	switch ((RCC->DCKCFGR2 & RCC_DCKCFGR2_UART4SEL_Msk) >> RCC_DCKCFGR2_UART4SEL_Pos)
	{
	default:
	case 0x00: return stm32f7xx_get_pclk1_freq();
	case 0x01: return stm32f7xx_get_sys_freq();
	case 0x02: return HSIFREQ;
	case 0x03: return LSEFREQ;
	}
}

unsigned long stm32f7xx_get_uart5_freq(void)
{
	//	00: APB1 clock (PCLK1) is selected as USART 5 clock
	//	01: System clock is selected as USART 5 clock
	//	10: HSI clock is selected as USART 5 clock
	//	11: LSE clock is selected as USART 5 clock
	switch ((RCC->DCKCFGR2 & RCC_DCKCFGR2_UART5SEL_Msk) >> RCC_DCKCFGR2_UART5SEL_Pos)
	{
	default:
	case 0x00: return stm32f7xx_get_pclk1_freq();
	case 0x01: return stm32f7xx_get_sys_freq();
	case 0x02: return HSIFREQ;
	case 0x03: return LSEFREQ;
	}
}

unsigned long stm32f7xx_get_usart6_freq(void)
{
	//	00: APB2 clock (PCLK2) is selected as USART 6 clock
	//	01: System clock is selected as USART 6 clock
	//	10: HSI clock is selected as USART 6 clock
	//	11: LSE clock is selected as USART 6 clock
	switch ((RCC->DCKCFGR2 & RCC_DCKCFGR2_USART6SEL_Msk) >> RCC_DCKCFGR2_USART6SEL_Pos)
	{
	default:
	case 0x00: return stm32f7xx_get_pclk2_freq();
	case 0x01: return stm32f7xx_get_sys_freq();
	case 0x02: return HSIFREQ;
	case 0x03: return LSEFREQ;
	}
}

unsigned long stm32f7xx_get_uart7_freq(void)
{
	//	00: APB1 clock (PCLK1) is selected as USART 7 clock
	//	01: System clock is selected as USART 7 clock
	//	10: HSI clock is selected as USART 7 clock
	//	11: LSE clock is selected as USART 7 clock
	switch ((RCC->DCKCFGR2 & RCC_DCKCFGR2_UART7SEL_Msk) >> RCC_DCKCFGR2_UART7SEL_Pos)
	{
	default:
	case 0x00: return stm32f7xx_get_pclk1_freq();
	case 0x01: return stm32f7xx_get_sys_freq();
	case 0x02: return HSIFREQ;
	case 0x03: return LSEFREQ;
	}
}

unsigned long stm32f7xx_get_uart8_freq(void)
{
	//	00: APB1 clock (PCLK1) is selected as USART 8 clock
	//	01: System clock is selected as USART 8 clock
	//	10: HSI clock is selected as USART 8 clock
	//	11: LSE clock is selected as USART 8 clock
	switch ((RCC->DCKCFGR2 & RCC_DCKCFGR2_UART8SEL_Msk) >> RCC_DCKCFGR2_UART8SEL_Pos)
	{
	default:
	case 0x00: return stm32f7xx_get_pclk1_freq();
	case 0x01: return stm32f7xx_get_sys_freq();
	case 0x02: return HSIFREQ;
	case 0x03: return LSEFREQ;
	}
}

#elif CPUSTYLE_STM32H7XX

// HAL data
//uint32_t SystemCoreClock = 64000000;
uint32_t SystemD2Clock = 64000000;
const  uint8_t D1CorePrescTable[16] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};

/* частоты, подающиеся на периферию */
//#define	PCLK1_FREQ (CPU_FREQ / 4)	// 42 MHz PCLK1 frequency
//#define	PCLK1_TIMERS_FREQ (CPU_FREQ / 2)	// 42 MHz PCLK1 frequency
//#define	PCLK2_FREQ (CPU_FREQ / 4)	// 84 MHz PCLK2 frequency
//#define	PCLK2_TIMERS_FREQ (CPU_FREQ / 2)	// 84 MHz PCLK2 frequency

#define BOARD_ADC_FREQ (stm32h7xx_get_adc_freq())

// See Table 8. Register boundary addresses
#define BOARD_USART1_FREQ 	(stm32h7xx_get_usart1_6_freq())
#define BOARD_USART2_FREQ 	(stm32h7xx_get_usart2_to_8_freq())
#define BOARD_USART3_FREQ 	(stm32h7xx_get_usart2_to_8_freq())
#define BOARD_TIM3_FREQ 	(stm32h7xx_get_timx_freq())	// TIM2..TIM7, TIM12..TIM14, LPTIM1, : APB1 D2 bus
#define BOARD_SYSTICK_FREQ 	(stm32h7xx_get_sys_freq() / 1)	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора
//#define BOARD_SYSTICK_FREQ (stm32h7xx_get_sysclk_freq() / 1)	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

unsigned long stm32h7xx_get_hse_freq(void)
{
#if WITHCPUXTAL
	return WITHCPUXTAL;
#elif WITHCPUXOSC
	return WITHCPUXOSC;
#else
	#warning WITHCPUXOSC or WITHCPUXTAL should be defined
	return 24000000uL;
#endif
}
//
//// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора
//unsigned long stm32h7xx_get_systick_freq(void)
//{
//
//
//}

unsigned long stm32h7xx_get_pll1_freq(void)
{
	const uint32_t pll1divm1 = ((RCC->PLLCKSELR & RCC_PLLCKSELR_DIVM1_Msk) >> RCC_PLLCKSELR_DIVM1_Pos);	// reference divisor - not need 1substracted
	const uint32_t pll1divn1 = ((RCC->PLL1DIVR & RCC_PLL1DIVR_N1_Msk) >> RCC_PLL1DIVR_N1_Pos) + 1;
	return (uint_fast64_t) REFINFREQ * pll1divn1 / pll1divm1;
}

unsigned long stm32h7xx_get_pll1_p_freq(void)
{
	const unsigned long pll1divp = ((RCC->PLL1DIVR & RCC_PLL1DIVR_P1_Msk) >> RCC_PLL1DIVR_P1_Pos) + 1;
	return stm32h7xx_get_pll1_freq() / pll1divp;
}

unsigned long stm32h7xx_get_pll1_q_freq(void)
{
	const unsigned long pll1divq = ((RCC->PLL1DIVR & RCC_PLL1DIVR_Q1_Msk) >> RCC_PLL1DIVR_Q1_Pos) + 1;
	return stm32h7xx_get_pll1_freq() / pll1divq;
}

unsigned long stm32h7xx_get_pll2_freq(void)
{
	const uint32_t pll2divm2 = ((RCC->PLLCKSELR & RCC_PLLCKSELR_DIVM2_Msk) >> RCC_PLLCKSELR_DIVM2_Pos);	// reference divisor - not need 1substracted
	const uint32_t pll2divn2 = ((RCC->PLL2DIVR & RCC_PLL2DIVR_N2_Msk) >> RCC_PLL2DIVR_N2_Pos) + 1;
	return (uint_fast64_t) REFINFREQ * pll2divn2 / pll2divm2;
}

unsigned long stm32h7xx_get_pll2_p_freq(void)
{
	const unsigned long pll2divp = ((RCC->PLL2DIVR & RCC_PLL2DIVR_P2_Msk) >> RCC_PLL2DIVR_P2_Pos) + 1;
	return stm32h7xx_get_pll2_freq() / pll2divp;
}

unsigned long stm32h7xx_get_pll2_q_freq(void)
{
	const unsigned long pll2divq = ((RCC->PLL2DIVR & RCC_PLL2DIVR_Q2_Msk) >> RCC_PLL2DIVR_Q2_Pos) + 1;
	return stm32h7xx_get_pll2_freq() / pll2divq;
}

unsigned long stm32h7xx_get_pll3_freq(void)
{
	const uint32_t pll3divm3 = ((RCC->PLLCKSELR & RCC_PLLCKSELR_DIVM3_Msk) >> RCC_PLLCKSELR_DIVM3_Pos);	// reference divisor - not need 1substracted
	const uint32_t pll3divn3 = ((RCC->PLL3DIVR & RCC_PLL3DIVR_N3_Msk) >> RCC_PLL3DIVR_N3_Pos) + 1;
	return (uint_fast64_t) REFINFREQ * pll3divn3 / pll3divm3;
}

unsigned long stm32h7xx_get_pll3_p_freq(void)
{
	const unsigned long pll3divp = ((RCC->PLL3DIVR & RCC_PLL3DIVR_P3_Msk) >> RCC_PLL3DIVR_P3_Pos) + 1;
	return stm32h7xx_get_pll3_freq() / pll3divp;
}

unsigned long stm32h7xx_get_pll3_q_freq(void)
{
	const unsigned long pll3divq = ((RCC->PLL3DIVR & RCC_PLL3DIVR_Q3_Msk) >> RCC_PLL3DIVR_Q3_Pos) + 1;
	return stm32h7xx_get_pll3_freq() / pll3divq;
}

unsigned long stm32h7xx_get_pll3_r_freq(void)
{
	const unsigned long pll3divr = ((RCC->PLL3DIVR & RCC_PLL3DIVR_R3_Msk) >> RCC_PLL3DIVR_R3_Pos) + 1;
	return stm32h7xx_get_pll3_freq() / pll3divr;
}

unsigned long stm32h7xx_get_sys_freq(void)
{
	//	000: HSI used as system clock (hsi_ck) (default after reset)
	//	001: CSI used as system clock (csi_ck)
	//	010: HSE used as system clock (hse_ck)
	//	011: PLL1 used as system clock (pll1_p_ck)
	//	others: Reserved
	switch ((RCC->CFGR & RCC_CFGR_SWS_Msk) >> RCC_CFGR_SWS_Pos)
	{
	default: return HSIFREQ;
	case 0x01: return CSI_VALUE;
	case 0x02: return stm32h7xx_get_hse_freq();
	case 0x03: return stm32h7xx_get_pll1_p_freq();
	}
}

// sys_d1cpre_ck
// rcc_c_ck
// rcc_fclk_c
unsigned long stm32h7xx_get_stm32h7xx_get_d1cpre_freq(void)
{
	const unsigned long sys_ck = stm32h7xx_get_sys_freq();
	//	0xxx: sys_ck not divided (default after reset)
	//	1000: sys_ck divided by 2
	//	1001: sys_ck divided by 4
	//	1010: sys_ck divided by 8
	//	1011: sys_ck divided by 16
	//	1100: sys_ck divided by 64
	//	1101: sys_ck divided by 128
	//	1110: sys_ck divided by 256
	//	1111: sys_ck divided by 512
	switch ((RCC->D1CFGR & RCC_D1CFGR_D1CPRE_Msk) >> RCC_D1CFGR_D1CPRE_Pos)
	{
	default: return sys_ck;
	case 0x08: return sys_ck / 2;
	case 0x09: return sys_ck / 4;
	case 0x0A: return sys_ck / 8;
	case 0x0B: return sys_ck / 16;
	case 0x0C: return sys_ck / 64;
	case 0x0D: return sys_ck / 128;
	case 0x0E: return sys_ck / 256;
	case 0x0F: return sys_ck / 512;
	}

}

unsigned long stm32h7xx_get_hclk3_freq(void)
{
	const unsigned long rcc_d1cpre = stm32h7xx_get_stm32h7xx_get_d1cpre_freq();
	// HPRE output
	//	0xxx: rcc_hclk3 = sys_d1cpre_ck (default after reset)
	//	1000: rcc_hclk3 = sys_d1cpre_ck / 2
	//	1001: rcc_hclk3 = sys_d1cpre_ck / 4
	//	1010: rcc_hclk3 = sys_d1cpre_ck / 8
	//	1011: rcc_hclk3 = sys_d1cpre_ck / 16
	//	1100: rcc_hclk3 = sys_d1cpre_ck / 64
	//	1101: rcc_hclk3 = sys_d1cpre_ck / 128
	//	1110: rcc_hclk3 = sys_d1cpre_ck / 256
	//	1111: rcc_hclk3 = sys_d1cpre_ck / 512
	switch ((RCC->D1CFGR & RCC_D1CFGR_HPRE_Msk) >> RCC_D1CFGR_HPRE_Pos)
	{
	default: return rcc_d1cpre;
	case 0x08: return rcc_d1cpre / 2;
	case 0x09: return rcc_d1cpre / 4;
	case 0x0A: return rcc_d1cpre / 8;
	case 0x0B: return rcc_d1cpre / 16;
	case 0x0C: return rcc_d1cpre / 64;
	case 0x0D: return rcc_d1cpre / 128;
	case 0x0E: return rcc_d1cpre / 256;
	case 0x0F: return rcc_d1cpre / 512;
	}
}

// HPRE output
unsigned long stm32h7xx_get_aclk_freq(void)
{
	return stm32h7xx_get_hclk3_freq();
}

// HPRE output
// rcc_hclk1
unsigned long stm32h7xx_get_hclk1_freq(void)
{
	return stm32h7xx_get_hclk3_freq();
}

// HPRE output
// rcc_hclk2
unsigned long stm32h7xx_get_hclk2_freq(void)
{
	return stm32h7xx_get_hclk3_freq();
}

// apb1
// rcc_timx_ker_ck
// rcc_pclk1
unsigned long stm32h7xx_get_apb1_freq(void)
{
	// D2PPRE1
	//	0xx: rcc_pclk1 = rcc_hclk1 (default after reset)
	//	100: rcc_pclk1 = rcc_hclk1 / 2
	//	101: rcc_pclk1 = rcc_hclk1 / 4
	//	110: rcc_pclk1 = rcc_hclk1 / 8
	//	111: rcc_pclk1 = rcc_hclk1 / 16
	switch ((RCC->D2CFGR & RCC_D2CFGR_D2PPRE1_Msk) >> RCC_D2CFGR_D2PPRE1_Pos)
	{
	default: return stm32h7xx_get_hclk1_freq();
	case 0x04: return stm32h7xx_get_hclk1_freq() / 2;
	case 0x05: return stm32h7xx_get_hclk1_freq() / 4;
	case 0x06: return stm32h7xx_get_hclk1_freq() / 8;
	case 0x07: return stm32h7xx_get_hclk1_freq() / 16;
	}
}

// apb2
// rcc_timy_ker_ck
// rcc_pclk2
unsigned long stm32h7xx_get_apb2_freq(void)
{
	// D2PPRE2
	//	0xx: rcc_pclk2 = rcc_hclk1 (default after reset)
	//	100: rcc_pclk2 = rcc_hclk1 / 2
	//	101: rcc_pclk2 = rcc_hclk1 / 4
	//	110: rcc_pclk2 = rcc_hclk1 / 8
	//	111: rcc_pclk2 = rcc_hclk1 / 16
	switch ((RCC->D2CFGR & RCC_D2CFGR_D2PPRE2_Msk) >> RCC_D2CFGR_D2PPRE2_Pos)
	{
	default: return stm32h7xx_get_hclk1_freq();
	case 0x04: return stm32h7xx_get_hclk1_freq() / 2;
	case 0x05: return stm32h7xx_get_hclk1_freq() / 4;
	case 0x06: return stm32h7xx_get_hclk1_freq() / 8;
	case 0x07: return stm32h7xx_get_hclk1_freq() / 16;
	}
}

// Table 56. Ratio between clock timer and pclk
// rcc_timx_ker_ck
// APB1 timers
unsigned long stm32h7xx_get_timx_freq(void)
{
	const unsigned long rcc_hclk1 = stm32h7xx_get_hclk1_freq();
	const uint8_t timpre = (RCC->CFGR & RCC_CFGR_TIMPRE) != 0;
	switch ((RCC->D2CFGR & RCC_D2CFGR_D2PPRE1_Msk) >> RCC_D2CFGR_D2PPRE1_Pos)
	{
	default: return rcc_hclk1;
	case 0x05: return timpre ? rcc_hclk1 / 1 : rcc_hclk1 / 2;
	case 0x06: return timpre ? rcc_hclk1 / 2 : rcc_hclk1 / 4;
	case 0x07: return timpre ? rcc_hclk1 / 4 : rcc_hclk1 / 8;
	}
}

// Table 56. Ratio between clock timer and pclk
// rcc_pclk1
unsigned long stm32h7xx_get_pclk1_freq(void)
{
	const unsigned long rcc_hclk1 = stm32h7xx_get_hclk1_freq();
	switch ((RCC->D2CFGR & RCC_D2CFGR_D2PPRE1_Msk) >> RCC_D2CFGR_D2PPRE1_Pos)
	{
	default: return rcc_hclk1;
	case 0x04: return rcc_hclk1 / 2;
	case 0x05: return rcc_hclk1 / 4;
	case 0x06: return rcc_hclk1 / 8;
	case 0x07: return rcc_hclk1 / 16;
	}
}

// Table 56. Ratio between clock timer and pclk
// rcc_timy_ker_ck
// APB2 timers
unsigned long stm32h7xx_get_timy_freq(void)
{
	const unsigned long rcc_hclk1 = stm32h7xx_get_hclk2_freq();
	const uint8_t timpre = (RCC->CFGR & RCC_CFGR_TIMPRE) != 0;
	switch ((RCC->D2CFGR & RCC_D2CFGR_D2PPRE2_Msk) >> RCC_D2CFGR_D2PPRE2_Pos)
	{
	default: return rcc_hclk1;
	case 0x05: return timpre ? rcc_hclk1 / 1 : rcc_hclk1 / 2;
	case 0x06: return timpre ? rcc_hclk1 / 2 : rcc_hclk1 / 4;
	case 0x07: return timpre ? rcc_hclk1 / 4 : rcc_hclk1 / 8;
	}
}

// Table 56. Ratio between clock timer and pclk
// rcc_pclk2
unsigned long stm32h7xx_get_pclk2_freq(void)
{
	const unsigned long rcc_hclk1 = stm32h7xx_get_hclk2_freq();
	switch ((RCC->D2CFGR & RCC_D2CFGR_D2PPRE2_Msk) >> RCC_D2CFGR_D2PPRE2_Pos)
	{
	default: return rcc_hclk1;
	case 0x04: return rcc_hclk1 / 2;
	case 0x05: return rcc_hclk1 / 4;
	case 0x06: return rcc_hclk1 / 8;
	case 0x07: return rcc_hclk1 / 16;
	}
}

unsigned long stm32h7xx_get_per_freq(void)
{
	// CKPERSEL
	//	00: hsi_ker_ck clock selected as per_ck clock (default after reset)
	//	01: csi_ker_ck clock selected as per_ck clock
	//	10: hse_ck clock selected as per_ck clock
	//	11: reserved, the per_ck clock is disabled
	switch ((RCC->D1CCIPR & RCC_D1CCIPR_CKPERSEL_Msk) >> RCC_D1CCIPR_CKPERSEL_Pos)
	{
	case 0x00: return HSIFREQ;
	case 0x01: return CSI_VALUE;
	case 0x02: return stm32h7xx_get_hse_freq();
	default: return HSIFREQ;
	}
}

// D1PPRE output
// rcc_pclk3
unsigned long stm32h7xx_get_pclk3_freq(void)
{
	const unsigned long rcc_hclk3 = stm32h7xx_get_hclk3_freq();	// HPRE output
	//	0xx: rcc_pclk3 = rcc_hclk3 (default after reset)
	//	100: rcc_pclk3 = rcc_hclk3 / 2
	//	101: rcc_pclk3 = rcc_hclk3 / 4
	//	110: rcc_pclk3 = rcc_hclk3 / 8
	//	111: rcc_pclk3 = rcc_hclk3 / 16

	switch ((RCC->D1CFGR & RCC_D1CFGR_D1PPRE_Msk) >> RCC_D1CFGR_D1PPRE_Pos)
	{
	default: return rcc_hclk3;
	case 0x04: return rcc_hclk3 / 2;
	case 0x05: return rcc_hclk3 / 4;
	case 0x06: return rcc_hclk3 / 8;
	case 0x07: return rcc_hclk3 / 16;
	}
}

// USART1 and 6 kernel clock source selection
unsigned long stm32h7xx_get_usart1_6_freq(void)
{
	//	000: rcc_pclk2 clock is selected as kernel clock (default after reset)
	//	001: pll2_q_ck clock is selected as kernel clock
	//	010: pll3_q_ck clock is selected as kernel clock
	//	011: hsi_ker_ck clock is selected as kernel clock
	//	100: csi_ker_ck clock is selected as kernel clock
	//	101: lse_ck clock is selected as kernel clock
	//	others: reserved, the kernel clock is disabled
	switch ((RCC->D2CCIP1R & RCC_D2CCIP2R_USART16SEL_Msk) >> RCC_D2CCIP2R_USART16SEL_Pos)
	{
	case 0x00: return stm32h7xx_get_pclk2_freq();	// D2PPRE2 output
	case 0x01: return stm32h7xx_get_pll2_q_freq();
	case 0x02: return stm32h7xx_get_pll3_q_freq();
	case 0x03: return HSIFREQ;
	case 0x04: return CSI_VALUE;
	case 0x05: return LSEFREQ;
	default: return HSIFREQ;
	}
}

// USART2/3, UART4,5, 7/8 (APB1) kernel clock source selection
unsigned long stm32h7xx_get_usart2_to_8_freq(void)
{
	// RCC Domain 2 Kernel Clock Configuration Register
	//	000: rcc_pclk1 clock is selected as kernel clock (default after reset)
	//	001: pll2_q_ck clock is selected as kernel clock
	//	010: pll3_q_ck clock is selected as kernel clock
	//	011: hsi_ker_ck clock is selected as kernel clock
	//	100: csi_ker_ck clock is selected as kernel clock
	//	101: lse_ck clock is selected as kernel clock
	// others: reserved, the kernel clock is disabled
	switch ((RCC->D2CCIP1R & RCC_D2CCIP2R_USART28SEL_Msk) >> RCC_D2CCIP2R_USART28SEL_Pos)
	{
	case 0x00: return stm32h7xx_get_pclk1_freq();	// D2PPRE1 output
	case 0x01: return stm32h7xx_get_pll2_q_freq();
	case 0x02: return stm32h7xx_get_pll3_q_freq();
	case 0x03: return HSIFREQ;
	case 0x04: return CSI_VALUE;
	case 0x05: return LSEFREQ;
	default: return HSIFREQ;
	}
}


unsigned long stm32h7xx_get_spi1_2_3_freq(void)
{
	// RCC Domain 2 Kernel Clock Configuration Register
	//	000: pll1_q_ck clock selected as SPI/I2S1,2 and 3 kernel clock (default after reset)
	//	001: pll2_p_ck clock selected as SPI/I2S1,2 and 3 kernel clock
	//	010: pll3_p_ck clock selected as SPI/I2S1,2 and 3 kernel clock
	//	011: I2S_CKIN clock selected as SPI/I2S1,2 and 3 kernel clock
	//	100: per_ck clock selected as SPI/I2S1,2 and 3 kernel clock
	//	others: reserved, the kernel clock is disabled
	//	Note: I2S_CKIN is an external clock taken from a pin.
	switch ((RCC->D2CCIP1R & RCC_D2CCIP1R_SPI123SEL_Msk) >> RCC_D2CCIP1R_SPI123SEL_Pos)
	{
	case 0x00: return stm32h7xx_get_pll1_q_freq();
	case 0x01: return stm32h7xx_get_pll2_p_freq();
	case 0x02: return stm32h7xx_get_pll3_p_freq();
#if (defined BOARD_I2S_CKIN_FREQ)
	case 0x03: return BOARD_I2S_CKIN_FREQ;
#endif /* (defined BOARD_I2S_CKIN_FREQ) */
	case 0x04: return stm32h7xx_get_per_freq();
	default: return stm32h7xx_get_per_freq();
	}
}

unsigned long stm32h7xx_get_spi4_5_freq(void)
{
	// RCC Domain 2 Kernel Clock Configuration Register
	//	000: APB clock is selected as kernel clock (default after reset)
	//	001: pll2_q_ck clock is selected as kernel clock
	//	010: pll3_q_ck clock is selected as kernel clock
	//	011: hsi_ker_ck clock is selected as kernel clock
	//	100: csi_ker_ck clock is selected as kernel clock
	//	101: hse_ck clock is selected as kernel clock
	//	others: reserved, the kernel clock is disabled
	switch ((RCC->D2CCIP1R & RCC_D2CCIP1R_SPI45SEL_Msk) >> RCC_D2CCIP1R_SPI45SEL_Pos)
	{
	case 0x00: return stm32h7xx_get_pclk2_freq();	// specific for D2 domain - APB2
	case 0x01: return stm32h7xx_get_pll2_q_freq();
	case 0x02: return stm32h7xx_get_pll3_q_freq();
	case 0x03: return HSIFREQ;
	case 0x04: return CSI_VALUE;
	case 0x05: return stm32h7xx_get_hse_freq();
	default: return HSIFREQ;
	}
}

unsigned long stm32h7xx_get_adc_freq(void)
{
	//	00: pll2_p_ck clock selected as kernel peripheral clock (default after reset)
	//	01: pll3_r_ck clock selected as kernel peripheral clock
	//	10: per_ck clock selected as kernel peripheral clock
	//	others: reserved, the kernel clock is disabled
	switch ((RCC->D3CCIPR & RCC_D3CCIPR_ADCSEL_Msk) >> RCC_D3CCIPR_ADCSEL_Pos)
	{
	default:
	case 0x00: return stm32h7xx_get_pll2_p_freq();
	case 0x01: return stm32h7xx_get_pll3_r_freq();
	case 0x02: return stm32h7xx_get_per_freq();
	}
}

// получение тактовой частоты тактирования блока SPI, использующенося в данной конфигурации
unsigned long hardware_get_spi_freq(void)
{
	return stm32h7xx_get_spi1_2_3_freq();
}

#elif CPUSTYLE_STM32MP1

/* частоты, подающиеся на периферию */
#define BOARD_USART1_FREQ 	(stm32mp1_uart1_get_freq())
#define BOARD_USART2_FREQ 	(stm32mp1_uart2_4_get_freq())
#define BOARD_USART3_FREQ 	(stm32mp1_uart3_5_get_freq())
#define BOARD_UART4_FREQ 	(stm32mp1_uart2_4_get_freq())
#define BOARD_UART5_FREQ 	(stm32mp1_uart3_5_get_freq())
#define BOARD_USART6_FREQ 	(stm32mp1_usart6_get_freq())
#define BOARD_UART7_FREQ 	(stm32mp1_uart7_8_get_freq())
#define BOARD_UART8_FREQ 	(stm32mp1_uart7_8_get_freq())
#define BOARD_TIM3_FREQ 	(stm32mp1_get_timg1_freq())
#define BOARD_TIM5_FREQ 	(stm32mp1_get_timg1_freq())
#define BOARD_ADC_FREQ 		(stm32mp1_get_adc_freq())

unsigned long stm32mp1_get_hse_freq(void)
{
#if WITHCPUXTAL
	return WITHCPUXTAL;
#elif WITHCPUXOSC
	return WITHCPUXOSC;
#else
	#warning WITHCPUXOSC or WITHCPUXTAL should be defined
	return 24000000uL;
#endif
}

// hsi_ck
// hsi_ker_ck
unsigned long stm32mp1_get_hsi_freq(void)
{
	const uint_fast32_t hsi = HSI64FREQ;
	const uint_fast32_t hsidiv = (RCC->HSICFGR & RCC_HSICFGR_HSIDIV_Msk) >> RCC_HSICFGR_HSIDIV_Pos;
	return hsi >> hsidiv;
}

unsigned long stm32mp1_get_pll1_2_ref_freq(void)
{
	// PLL1, PLL2 source mux
	// 0x0: HSI selected as PLL clock (hsi_ck) (default after reset)
	// 0x1: HSE selected as PLL clock (hse_ck)
	switch ((RCC->RCK12SELR & RCC_RCK12SELR_PLL12SRC_Msk) >> RCC_RCK12SELR_PLL12SRC_Pos)
	{
	default:
	case 0x00:
		return stm32mp1_get_hsi_freq();
	case 0x01:
		return stm32mp1_get_hse_freq();
	}
}

unsigned long stm32mp1_get_pll3_ref_freq(void)
{
	// PLL3 source mux
	//	0x0: HSI selected as PLL clock (hsi_ck) (default after reset)
	//	0x1: HSE selected as PLL clock (hse_ck)
	//	0x2: CSI selected as PLL clock (csi_ck)
	//	0x3: No clock send to DIVMx divisor and PLLs
	switch ((RCC->RCK3SELR & RCC_RCK3SELR_PLL3SRC_Msk) >> RCC_RCK3SELR_PLL3SRC_Pos)
	{
	default:
	case 0x00:
		return stm32mp1_get_hsi_freq();
	case 0x01:
		return stm32mp1_get_hse_freq();
	case 0x02:
		return CSI_VALUE;
	}
}

unsigned long stm32mp1_get_pll4_ref_freq(void)
{
	// PLL4 source mux
	//	0x0: HSI selected as PLL clock (hsi_ck) (default after reset)
	//	0x1: HSE selected as PLL clock (hse_ck)
	//	0x2: CSI selected as PLL clock (csi_ck)
	//	0x3: Signal I2S_CKIN used as reference clock
	switch ((RCC->RCK4SELR & RCC_RCK4SELR_PLL4SRC_Msk) >> RCC_RCK4SELR_PLL4SRC_Pos)
	{
	default:
	case 0x00:
		return stm32mp1_get_hsi_freq();
	case 0x01:
		return stm32mp1_get_hse_freq();
	case 0x02:
		return CSI_VALUE;
#if (defined BOARD_I2S_CKIN_FREQ)
	case 0x03: return BOARD_I2S_CKIN_FREQ;
#endif /* (defined BOARD_I2S_CKIN_FREQ) */
	}
}

// PLL1 methods
unsigned long stm32mp1_get_pll1_freq(void)
{
	const uint_fast32_t pll1divn = ((RCC->PLL1CFGR1 & RCC_PLL1CFGR1_DIVN_Msk) >> RCC_PLL1CFGR1_DIVN_Pos) + 1;
	const uint_fast32_t pll1divm = ((RCC->PLL1CFGR1 & RCC_PLL1CFGR1_DIVM1_Msk) >> RCC_PLL1CFGR1_DIVM1_Pos) + 1;
	return (uint_fast64_t) stm32mp1_get_pll1_2_ref_freq() * pll1divn / pll1divm;
}

unsigned long stm32mp1_get_pll1_p_freq(void)
{
	const uint_fast32_t pll1divp = ((RCC->PLL1CFGR2 & RCC_PLL1CFGR2_DIVP_Msk) >> RCC_PLL1CFGR2_DIVP_Pos) + 1;
	return stm32mp1_get_pll1_freq() / pll1divp;
}

// MPU frequency
// mpuss_ck
unsigned long stm32mp1_get_mpuss_freq(void)
{
	//	0x0: The MPUDIV is disabled; i.e. no clock generated
	//	0x1: The mpuss_ck is equal to pll1_p_ck divided by 2 (default after reset)
	//	0x2: The mpuss_ck is equal to pll1_p_ck divided by 4
	//	0x3: The mpuss_ck is equal to pll1_p_ck divided by 8
	//	others: The mpuss_ck is equal to pll1_p_ck divided by 16

	const uint_fast32_t mpudiv = 1uL << ((RCC->MPCKDIVR & RCC_MPCKDIVR_MPUDIV_Msk) >> RCC_MPCKDIVR_MPUDIV_Pos);

	//	0x0: HSI selected as MPU sub-system clock (hsi_ck) (default after reset)
	//	0x1: HSE selected as MPU sub-system clock (hse_ck)
	//	0x2: PLL1 selected as MPU sub-system clock (pll1_p_ck)
	//	0x3: PLL1 via MPUDIV is selected as MPU sub-system clock (pll1_p_ck / (2 ^ MPUDIV)).
	switch ((RCC->MPCKSELR & RCC_MPCKSELR_MPUSRC_Msk) >> RCC_MPCKSELR_MPUSRC_Pos)
	{
	default:
	case 0x00:
		return stm32mp1_get_hsi_freq();
	case 0x01:
		return stm32mp1_get_hse_freq();
	case 0x02:
		return stm32mp1_get_pll1_p_freq();
	case 0x03:
		return stm32mp1_get_pll1_p_freq() / mpudiv;
	}
}

// PLL2 methods
unsigned long stm32mp1_get_pll2_freq(void)
{
	const uint_fast32_t pll2divn = ((RCC->PLL2CFGR1 & RCC_PLL2CFGR1_DIVN_Msk) >> RCC_PLL2CFGR1_DIVN_Pos) + 1;
	const uint_fast32_t pll2divm = ((RCC->PLL2CFGR1 & RCC_PLL2CFGR1_DIVM2_Msk) >> RCC_PLL2CFGR1_DIVM2_Pos) + 1;
	return (uint_fast64_t) stm32mp1_get_pll1_2_ref_freq() * pll2divn / pll2divm;
}

unsigned long stm32mp1_get_pll2_p_freq(void)
{
	const uint_fast32_t pll2divp = ((RCC->PLL2CFGR2 & RCC_PLL2CFGR2_DIVP_Msk) >> RCC_PLL2CFGR2_DIVP_Pos) + 1;
	return stm32mp1_get_pll2_freq() / pll2divp;
}

unsigned long stm32mp1_get_pll2_r_freq(void)
{
	const uint_fast32_t pll2divr = ((RCC->PLL2CFGR2 & RCC_PLL2CFGR2_DIVR_Msk) >> RCC_PLL2CFGR2_DIVR_Pos) + 1;
	return stm32mp1_get_pll2_freq() / pll2divr;
}

// PLL3 methods
unsigned long stm32mp1_get_pll3_freq(void)
{
	const uint_fast32_t pll3divn = ((RCC->PLL3CFGR1 & RCC_PLL3CFGR1_DIVN_Msk) >> RCC_PLL3CFGR1_DIVN_Pos) + 1;
	const uint_fast32_t pll3divm = ((RCC->PLL3CFGR1 & RCC_PLL3CFGR1_DIVM3_Msk) >> RCC_PLL3CFGR1_DIVM3_Pos) + 1;
	return (uint_fast64_t) stm32mp1_get_pll3_ref_freq() * pll3divn / pll3divm;
}

unsigned long stm32mp1_get_pll3_p_freq(void)
{
	const uint_fast32_t pll3divp = ((RCC->PLL3CFGR2 & RCC_PLL3CFGR2_DIVP_Msk) >> RCC_PLL3CFGR2_DIVP_Pos) + 1;
	return stm32mp1_get_pll3_freq() / pll3divp;
}

unsigned long stm32mp1_get_pll3_q_freq(void)
{
	const uint_fast32_t pll3divq = ((RCC->PLL3CFGR2 & RCC_PLL3CFGR2_DIVQ_Msk) >> RCC_PLL3CFGR2_DIVQ_Pos) + 1;
	return stm32mp1_get_pll3_freq() / pll3divq;
}

unsigned long stm32mp1_get_pll3_r_freq(void)
{
	const uint_fast32_t pll3divr = ((RCC->PLL3CFGR2 & RCC_PLL3CFGR2_DIVR_Msk) >> RCC_PLL3CFGR2_DIVR_Pos) + 1;
	return stm32mp1_get_pll3_freq() / pll3divr;
}

// PLL4 methods
unsigned long stm32mp1_get_pll4_freq(void)
{
	//#define PLL4_FREQ	(REFINFREQ / (PLL4DIVM) * (PLL4DIVN))
	const uint32_t pll4divn = ((RCC->PLL4CFGR1 & RCC_PLL4CFGR1_DIVN_Msk) >> RCC_PLL4CFGR1_DIVN_Pos) + 1;
	const uint32_t pll4divm = ((RCC->PLL4CFGR1 & RCC_PLL4CFGR1_DIVM4_Msk) >> RCC_PLL4CFGR1_DIVM4_Pos) + 1;
	return (uint_fast64_t) stm32mp1_get_pll4_ref_freq() * pll4divn / pll4divm;
}

unsigned long stm32mp1_get_pll4_q_freq(void)
{
	const uint_fast32_t pll4divq = ((RCC->PLL4CFGR2 & RCC_PLL4CFGR2_DIVQ_Msk) >> RCC_PLL4CFGR2_DIVQ_Pos) + 1;
	return stm32mp1_get_pll4_freq() / pll4divq;
}

unsigned long stm32mp1_get_pll4_p_freq(void)
{
	const uint_fast32_t pll4divp = ((RCC->PLL4CFGR2 & RCC_PLL4CFGR2_DIVP_Msk) >> RCC_PLL4CFGR2_DIVP_Pos) + 1;
	return stm32mp1_get_pll4_freq() / pll4divp;
}

unsigned long stm32mp1_get_pll4_r_freq(void)
{
	const uint_fast32_t pll4divr = ((RCC->PLL4CFGR2 & RCC_PLL4CFGR2_DIVR_Msk) >> RCC_PLL4CFGR2_DIVR_Pos) + 1;
	return stm32mp1_get_pll4_freq() / pll4divr;
}

// Ethernet controller freq
// ETHSRC
unsigned long stm32mp1_get_eth_freq(void)
{
	//0x0: pll4_p_ck clock selected as kernel peripheral clock (default after reset)
	//0x1: pll3_q_ck clock selected as kernel peripheral clock
	//others: the kernel clock is disabled
	switch ((RCC->ETHCKSELR & RCC_ETHCKSELR_ETHSRC_Msk) >> RCC_ETHCKSELR_ETHSRC_Pos)
	{
	default:
	case 0x00: return stm32mp1_get_pll4_p_freq();
	case 0x01: return stm32mp1_get_pll3_q_freq();
	}
}

// clk_ptp_ref_i
unsigned long stm32mp1_get_ethptp_freq(void)
{
	const unsigned long d = ((RCC->ETHCKSELR & RCC_ETHCKSELR_ETHPTPDIV_Msk) >> RCC_ETHCKSELR_ETHPTPDIV_Pos) + 1;
	return stm32mp1_get_eth_freq() / d;
}

// Internal AXI clock frequency
unsigned long stm32mp1_get_axiss_freq(void)
{
	//0x0: HSI selected as AXI sub-system clock (hsi_ck) (default after reset)
	//0x1: HSE selected as AXI sub-system clock (hse_ck)
	//0x2: PLL2 selected as AXI sub-system clock (pll2_p_ck)
	//others: axiss_ck is gated
	// axiss_ck 266 MHz Max
	switch ((RCC->ASSCKSELR & RCC_ASSCKSELR_AXISSRC_Msk) >> RCC_ASSCKSELR_AXISSRC_Pos)
	{
	case 0x00: return stm32mp1_get_hsi_freq();
	case 0x01: return stm32mp1_get_hse_freq();
	case 0x02: return stm32mp1_get_pll2_p_freq();
	default: return HSI64FREQ;
	}
}

unsigned long stm32mp1_get_per_freq(void)
{
	// per_ck source clock selection
	//0x0: hsi_ker_ck clock selected (default after reset)
	//0x1: csi_ker_ck clock selected
	//0x2: hse_ker_ck clock selected
	//0x3: Clock disabled
	switch ((RCC->CPERCKSELR & RCC_CPERCKSELR_CKPERSRC_Msk) >> RCC_CPERCKSELR_CKPERSRC_Pos)
	{
	case 0x00:	return stm32mp1_get_hsi_freq();
	case 0x01:	return CSI_VALUE;
	case 0x02:	return stm32mp1_get_hse_freq();
	default: return HSI64FREQ;
	}

}
unsigned long stm32mp1_get_mcuss_freq(void)
{
	// MCUSSRC
	//	0x0: HSI selected as MCU sub-system clock (hsi_ck) (default after reset)
	//	0x1: HSE selected as MCU sub-system clock (hse_ck)
	//	0x2: CSI selected as MCU sub-system clock (csi_ck)
	//	0x3: PLL3 selected as MCU sub-system clock (pll3_p_ck)
	switch ((RCC->MSSCKSELR & RCC_MSSCKSELR_MCUSSRC_Msk) >> RCC_MSSCKSELR_MCUSSRC_Pos)
	{
	default:
	case 0x00: return stm32mp1_get_hsi_freq();
	case 0x01: return stm32mp1_get_hse_freq();
	case 0x02: return CSI_VALUE;
	case 0x03: return stm32mp1_get_pll3_p_freq();
	}
}

// hclk5, hclk6, aclk
unsigned long stm32mp1_get_aclk_freq(void)
{
	// AXI, AHB5 and AHB6 clock divisor
	//	0x0: axiss_ck (default after reset)
	//	0x1: axiss_ck / 2
	//	0x2: axiss_ck / 3
	//	others: axiss_ck / 4
	switch ((RCC->AXIDIVR & RCC_AXIDIVR_AXIDIV_Msk) >> RCC_AXIDIVR_AXIDIV_Pos)
	{
	case 0x00: return stm32mp1_get_axiss_freq();
	case 0x01: return stm32mp1_get_axiss_freq() / 2;
	case 0x02: return stm32mp1_get_axiss_freq() / 3;
	default: return stm32mp1_get_axiss_freq();
	}
}

// Internal AHB5 clock frequency
unsigned long stm32mp1_get_hclk5_freq(void)
{
	return stm32mp1_get_aclk_freq();
}

// Internal AHB6 clock frequency
unsigned long stm32mp1_get_hclk6_freq(void)
{
	return stm32mp1_get_aclk_freq();
}

// mlhclk_ck
// 209 MHz max
unsigned long stm32mp1_get_mlhclk_freq(void)
{
	// MCUDIV
	switch ((RCC->MCUDIVR & RCC_MCUDIVR_MCUDIV_Msk) >> RCC_MCUDIVR_MCUDIV_Pos)
	{
	case 0: return stm32mp1_get_mcuss_freq() / 1;
	case 1: return stm32mp1_get_mcuss_freq() / 2;
	case 2: return stm32mp1_get_mcuss_freq() / 4;
	case 3: return stm32mp1_get_mcuss_freq() / 8;
	case 4: return stm32mp1_get_mcuss_freq() / 16;
	case 5: return stm32mp1_get_mcuss_freq() / 32;
	case 6: return stm32mp1_get_mcuss_freq() / 64;
	case 7: return stm32mp1_get_mcuss_freq() / 128;
	case 8: return stm32mp1_get_mcuss_freq() / 256;
	default: return stm32mp1_get_mcuss_freq() / 512;
	}
}

// Internal APB1 clock frequency
// 104.5 MHz max
unsigned long stm32mp1_get_pclk1_freq(void)
{
	// APB1 Output divisor (output max 104.5 MHz)
	// Input MLHCK (209 MHz max)
	//0x0: mlhclk (default after reset)
	//0x1: mlhclk / 2
	//0x2: mlhclk / 4
	//0x3: mlhclk / 8
	//0x4: mlhclk / 16
	switch ((RCC->APB1DIVR & RCC_APB1DIVR_APB1DIV_Msk) >> RCC_APB1DIVR_APB1DIV_Pos)
	{
	default:
	case 0x00: return stm32mp1_get_mlhclk_freq() / 1;
	case 0x01: return stm32mp1_get_mlhclk_freq() / 2;
	case 0x02: return stm32mp1_get_mlhclk_freq() / 4;
	case 0x03: return stm32mp1_get_mlhclk_freq() / 8;
	case 0x04: return stm32mp1_get_mlhclk_freq() / 16;
	}
}

// Internal APB2 clock frequency
// 104.5 MHz max
unsigned long stm32mp1_get_pclk2_freq(void)
{
	// APB2 Output divisor (output max 104.5 MHz)
	// Input MLHCK (209 MHz max)
	//0x0: mlhclk (default after reset)
	//0x1: mlhclk / 2
	//0x2: mlhclk / 4
	//0x3: mlhclk / 8
	//0x4: mlhclk / 16
	switch ((RCC->APB2DIVR & RCC_APB2DIVR_APB2DIV_Msk) >> RCC_APB2DIVR_APB2DIV_Pos)
	{
	default:
	case 0x00: return stm32mp1_get_mlhclk_freq() / 1;
	case 0x01: return stm32mp1_get_mlhclk_freq() / 2;
	case 0x02: return stm32mp1_get_mlhclk_freq() / 4;
	case 0x03: return stm32mp1_get_mlhclk_freq() / 8;
	case 0x04: return stm32mp1_get_mlhclk_freq() / 16;
	}
}

// Internal APB3 clock frequency
// 104.5 MHz max
unsigned long stm32mp1_get_pclk3_freq(void)
{
	// APB3 Output divisor (output max 104.5 MHz)
	// Input MLHCK (209 MHz max)
	//0x0: mlhclk (default after reset)
	//0x1: mlhclk / 2
	//0x2: mlhclk / 4
	//0x3: mlhclk / 8
	//0x4: mlhclk / 16
	switch ((RCC->APB3DIVR & RCC_APB3DIVR_APB3DIV_Msk) >> RCC_APB3DIVR_APB3DIV_Pos)
	{
	default:
	case 0x00: return stm32mp1_get_mlhclk_freq() / 1;
	case 0x01: return stm32mp1_get_mlhclk_freq() / 2;
	case 0x02: return stm32mp1_get_mlhclk_freq() / 4;
	case 0x03: return stm32mp1_get_mlhclk_freq() / 8;
	case 0x04: return stm32mp1_get_mlhclk_freq() / 16;
	}
}

// Internal APB4 clock frequency
// 133 MHz max
unsigned long stm32mp1_get_pclk4_freq(void)
{
	// APB4 Output divisor
	//	0x0: aclk (default after reset)
	//	0x1: aclk / 2
	//	0x2: aclk / 4
	//	0x3: aclk / 8
	//	others: aclk / 16
	switch ((RCC->APB4DIVR & RCC_APB4DIVR_APB4DIV_Msk) >> RCC_APB4DIVR_APB4DIV_Pos)
	{
	case 0x00:	return stm32mp1_get_aclk_freq() / 1;
	case 0x01:	return stm32mp1_get_aclk_freq() / 2;
	case 0x02:	return stm32mp1_get_aclk_freq() / 4;
	case 0x03:	return stm32mp1_get_aclk_freq() / 8;
	default:	return stm32mp1_get_aclk_freq() / 16;
	}
}

// Internal APB5 clock frequency
// 133 MHz max
unsigned long stm32mp1_get_pclk5_freq(void)
{
	// APB5 Output divisor
	//	0x0: aclk (default after reset)
	//	0x1: aclk / 2
	//	0x2: aclk / 4
	//	0x3: aclk / 8
	//	others: aclk / 16
	switch ((RCC->APB5DIVR & RCC_APB5DIVR_APB5DIV_Msk) >> RCC_APB5DIVR_APB5DIV_Pos)
	{
	case 0x00:	return stm32mp1_get_aclk_freq() / 1;
	case 0x01:	return stm32mp1_get_aclk_freq() / 2;
	case 0x02:	return stm32mp1_get_aclk_freq() / 4;
	case 0x03:	return stm32mp1_get_aclk_freq() / 8;
	default:	return stm32mp1_get_aclk_freq() / 16;
	}
}

unsigned long stm32mp1_get_usbphy_freq(void)
{
	//	0x0: hse_ker_ck clock selected as kernel peripheral clock (default after reset)
	//	0x1: pll4_r_ck clock selected as kernel peripheral clock
	//	0x2: hse_ker_ck/2 clock selected as kernel peripheral clock
	//	other: Clock disabled
	switch ((RCC->USBCKSELR & RCC_USBCKSELR_USBPHYSRC_Msk) >> RCC_USBCKSELR_USBPHYSRC_Pos)
	{
	default:
	case 0x00:
		return stm32mp1_get_hse_freq();
	case 0x01:
		return stm32mp1_get_pll4_r_freq();
	case 0x02:
		return stm32mp1_get_hse_freq();
	}
}

unsigned long stm32mp1_get_qspi_freq(void)
{
	//	0x0: aclk clock selected as kernel peripheral clock (default after reset)
	//	0x1: pll3_r_ck clock selected as kernel peripheral clock
	//	0x2: pll4_p_ck clock selected as kernel peripheral clock
	//	0x3: per_ck clock selected as kernel peripheral clock
	switch ((RCC->QSPICKSELR & RCC_QSPICKSELR_QSPISRC_Msk) >> RCC_QSPICKSELR_QSPISRC_Pos)
	{
	default:
	case 0x00:
		return stm32mp1_get_aclk_freq();
	case 0x01:
		return stm32mp1_get_pll3_r_freq();
	case 0x02:
		return stm32mp1_get_pll4_p_freq();
	case 0x03:
		return stm32mp1_get_per_freq();
	}
}

unsigned long stm32mp1_get_usbotg_freq(void)
{
	//	0: pll4_r_ck clock selected as kernel peripheral clock (default after reset)
	//	1: clock provided by the USB PHY (rcc_ck_usbo_48m) selected as kernel peripheral clock

	switch ((RCC->USBCKSELR & RCC_USBCKSELR_USBOSRC_Msk) >> RCC_USBCKSELR_USBOSRC_Pos)
	{
	default:
	case 0x00:
		return stm32mp1_get_pll4_r_freq();
	case 0x01:
		return 48000000uL; //rcc_ck_usbo_48m;
	}
}

unsigned long stm32mp1_uart1_get_freq(void)
{
	//	0x0: pclk5 clock selected as kernel peripheral clock (default after reset)
	//	0x1: pll3_q_ck clock selected as kernel peripheral clock
	//	0x2: hsi_ker_ck clock selected as kernel peripheral clock
	//	0x3: csi_ker_ck clock selected as kernel peripheral clock
	//	0x4: pll4_q_ck clock selected as kernel peripheral clock
	//	0x5: hse_ker_ck clock selected as kernel peripheral clock
	switch ((RCC->UART1CKSELR & RCC_UART1CKSELR_UART1SRC_Msk) >> RCC_UART1CKSELR_UART1SRC_Pos)
	{
	case 0x00:
		return stm32mp1_get_pclk5_freq();
	case 0x01:
		return stm32mp1_get_pll3_q_freq();
	case 0x02:
		return stm32mp1_get_hsi_freq();
	case 0x03:
		return CSI_VALUE;
	case 0x04:
		return stm32mp1_get_pll4_q_freq();
	case 0x05:
		return stm32mp1_get_hse_freq();
	default:
		return HSI64FREQ;
	}
}

unsigned long stm32mp1_uart2_4_get_freq(void)
{
	// UART2, UART4
	//	0x0: pclk1 clock selected as kernel peripheral clock (default after reset)
	//	0x1: pll4_q_ck clock selected as kernel peripheral clock
	//	0x2: hsi_ker_ck clock selected as kernel peripheral clock
	//	0x3: csi_ker_ck clock selected as kernel peripheral clock
	//	0x4: hse_ker_ck clock selected as kernel peripheral clock
	switch ((RCC->UART24CKSELR & RCC_UART24CKSELR_UART24SRC_Msk) >> RCC_UART24CKSELR_UART24SRC_Pos)
	{
	case 0x00:
		return stm32mp1_get_pclk1_freq();
	case 0x01:
		return stm32mp1_get_pll4_q_freq();
	case 0x02:
		return stm32mp1_get_hsi_freq();
	case 0x03:
		return CSI_VALUE;
	case 0x04:
		return stm32mp1_get_hse_freq();
	default:
		return HSI64FREQ;
	}

}

unsigned long stm32mp1_uart3_5_get_freq(void)
{
	//	0x0: pclk1 clock selected as kernel peripheral clock (default after reset)
	//	0x1: pll4_q_ck clock selected as kernel peripheral clock
	//	0x2: hsi_ker_ck clock selected as kernel peripheral clock
	//	0x3: csi_ker_ck clock selected as kernel peripheral clock
	//	0x4: hse_ker_ck clock selected as kernel peripheral clock
	//	others: reserved, the kernel clock is disabled
	switch ((RCC->UART35CKSELR & RCC_UART35CKSELR_UART35SRC_Msk) >> RCC_UART35CKSELR_UART35SRC_Pos)
	{
	case 0x00:
		return stm32mp1_get_pclk1_freq();
	case 0x01:
		return stm32mp1_get_pll4_q_freq();
	case 0x02:
		return stm32mp1_get_hsi_freq();
	case 0x03:
		return CSI_VALUE;
	case 0x04:
		return stm32mp1_get_hse_freq();
	default:
		return HSI64FREQ;
	}
}

unsigned long stm32mp1_uart7_8_get_freq(void)
{
	// UART7, UART8
	//0x0: pclk1 clock selected as kernel peripheral clock (default after reset)
	//0x1: pll4_q_ck clock selected as kernel peripheral clock
	//0x2: hsi_ker_ck clock selected as kernel peripheral clock
	//0x3: csi_ker_ck clock selected as kernel peripheral clock
	//0x4: hse_ker_ck clock selected as kernel peripheral clock
	switch ((RCC->UART78CKSELR & RCC_UART78CKSELR_UART78SRC_Msk) >> RCC_UART78CKSELR_UART78SRC_Pos)
	{
	case 0x00:
		return stm32mp1_get_pclk1_freq();
	case 0x01:
		return stm32mp1_get_pll4_q_freq();
	case 0x02:
		return stm32mp1_get_hsi_freq();
	case 0x03:
		return CSI_VALUE;
	case 0x04:
		return stm32mp1_get_hse_freq();
	default:
		return HSI64FREQ;
	}

}

unsigned long stm32mp1_sdmmc1_2_get_freq(void)
{
	// SDMMC1
	//	0x0: hclk6 clock selected as kernel peripheral clock
	//	0x1: pll3_r_ck clock selected as kernel peripheral clock
	//	0x2: pll4_p_ck clock selected as kernel peripheral clock
	//	0x3: hsi_ker_ck clock selected as kernel peripheral clock (default after reset)
	//	others: reserved, the kernel clock is disabled
	switch ((RCC->SDMMC12CKSELR & RCC_SDMMC12CKSELR_SDMMC12SRC_Msk) >> RCC_SDMMC12CKSELR_SDMMC12SRC_Pos)
	{
	case 0x00:
		return stm32mp1_get_hclk6_freq();
	case 0x01:
		return stm32mp1_get_pll3_r_freq();
	case 0x02:
		return stm32mp1_get_pll4_p_freq();
	case 0x03:
		return stm32mp1_get_hsi_freq();
	default:
		return HSI64FREQ;
	}
}

// частота для деления таймером
// timg1_ck: TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM12, TIM13 and TIM14
// kernel: Ftimg1_ck 209 MHz max
// bus: Fpclk1 104.5 MHz max
unsigned long stm32mp1_get_timg1_freq(void)
{
	const unsigned g1 = (RCC->TIMG1PRER & RCC_TIMG1PRER_TIMG1PRE_Msk) != 0;
	switch ((RCC->APB1DIVR & RCC_APB1DIVR_APB1DIV_Msk) >> RCC_APB1DIVR_APB1DIV_Pos)
	{
	default:
	case 0x00: return stm32mp1_get_mlhclk_freq();
	case 0x01: return stm32mp1_get_mlhclk_freq();
	case 0x02: return g1 ? stm32mp1_get_mlhclk_freq() : (stm32mp1_get_mlhclk_freq() / 2);
	case 0x03: return g1 ? (stm32mp1_get_mlhclk_freq() / 2) : (stm32mp1_get_mlhclk_freq() / 4);
	case 0x04: return g1 ? (stm32mp1_get_mlhclk_freq() / 4) : (stm32mp1_get_mlhclk_freq() / 8);
	}
}

// частота для деления таймером
// timg2_ck: TIM1, TIM8, TIM15, TIM16, and TIM17
// kernel: Ftimg1_ck 209 MHz max
// bus: Fpclk2 104.5 MHz max
unsigned long stm32mp1_get_timg2_freq(void)
{
	const unsigned g2 = (RCC->TIMG2PRER & RCC_TIMG2PRER_TIMG2PRE_Msk) != 0;
	switch ((RCC->APB2DIVR & RCC_APB2DIVR_APB2DIV_Msk) >> RCC_APB2DIVR_APB2DIV_Pos)
	{
	default:
	case 0x00: return stm32mp1_get_mlhclk_freq();
	case 0x01: return stm32mp1_get_mlhclk_freq();
	case 0x02: return g2 ? stm32mp1_get_mlhclk_freq() : (stm32mp1_get_mlhclk_freq() / 2);
	case 0x03: return g2 ? (stm32mp1_get_mlhclk_freq() / 2) : (stm32mp1_get_mlhclk_freq() / 4);
	case 0x04: return g2 ? (stm32mp1_get_mlhclk_freq() / 4) : (stm32mp1_get_mlhclk_freq() / 8);
	}
}

unsigned long stm32mp1_get_spi1_freq(void)
{
	//	0x0: pll4_p_ck clock selected as kernel peripheral clock (default after reset)
	//	0x1: pll3_q_ck clock selected as kernel peripheral clock
	//	0x2: I2S_CKIN clock selected as kernel peripheral clock
	//	0x3: per_ck clock selected as kernel peripheral clock
	//	0x4: pll3_r_ck clock selected as kernel peripheral clock
	//	others: reserved, the kernel clock is disabled
	switch ((RCC->SPI2S1CKSELR & RCC_SPI2S1CKSELR_SPI1SRC_Msk) >> RCC_SPI2S1CKSELR_SPI1SRC_Pos)
	{
	case 0x00: return stm32mp1_get_pll4_p_freq();
	case 0x01: return stm32mp1_get_pll3_q_freq();
#if (defined BOARD_I2S_CKIN_FREQ)
	case 0x02: return BOARD_I2S_CKIN_FREQ;
#endif /* (defined BOARD_I2S_CKIN_FREQ) */
	case 0x03: return stm32mp1_get_per_freq();
	case 0x04: return stm32mp1_get_pll3_r_freq();
	default: return stm32mp1_get_per_freq();
	}
}

unsigned long stm32mp1_get_adc_freq(void)
{
	//	0x0: pll4_r_ck clock selected as kernel peripheral clock (default after reset)
	//	0x1: per_ck clock selected as kernel peripheral clock
	//	0x2: pll3_q_ck clock selected as kernel peripheral clock
	//	others: the kernel clock is disabled
	switch ((RCC->ADCCKSELR & RCC_ADCCKSELR_ADCSRC_Msk) >> RCC_ADCCKSELR_ADCSRC_Pos)
	{
	case 0x00: return stm32mp1_get_pll4_r_freq();
	case 0x01: return stm32mp1_get_per_freq();
	case 0x02: return stm32mp1_get_pll3_q_freq();
	default: return stm32mp1_get_per_freq();
	}
}

// получение тактовой частоты тактирования блока SPI, использующенося в данной конфигурации
unsigned long hardware_get_spi_freq(void)
{
	return stm32mp1_get_spi1_freq();
}

#elif CPUSTYLE_STM32F1XX && defined (STM32F101xB)

// placeholders
#define BOARD_USART1_FREQ (CPU_FREQ / 1)
#define BOARD_SPI_FREQ (CPU_FREQ / 1)
#define BOARD_I2C_FREQ (CPU_FREQ / 1)
#define BOARD_TIM3_FREQ (CPU_FREQ / 1)
#warning TODO: use real clocks

#elif CPUSTYLE_STM32F1XX && defined (STM32F103xB)

// placeholders
#define BOARD_USART1_FREQ (CPU_FREQ / 1)
#define BOARD_SPI_FREQ (CPU_FREQ / 1)
#define BOARD_I2C_FREQ (CPU_FREQ / 1)
#define BOARD_TIM3_FREQ (CPU_FREQ / 1)
#warning TODO: use real clocks

#endif /* CPUSTYLE_STM32MP1 */




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

#if CPUSTYLE_STM32MP1

#elif CPUSTYLE_STM32F

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


	/*******************************************************************************
	* Function Name: r7s721_pll_initialize
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
	void r7s721_pll_initialize(void)
	{
	    /* Cancel L2C standby status before clock change */
	    L2CREG15_POWER_CTRL = 0x00000001;
		(void) L2CREG15_POWER_CTRL;

	    /* standby_mode_en bit of Power Control Register setting */
	    //*(volatile uint32_t *)(0x3fffff80) = 0x00000001;
	    //(void) *(volatile uint32_t *)(0x3fffff80);

	    /* ==== CPG Settings ==== */

	    /* PLL(x30), I:G:B:P1:P0 = 30:20:10:5:5/2 */
	    //CPG.FRQCR  = 0x1035u;
	    CPG.FRQCR  = 0x3035u;	// CKIO pin = hi-z
		(void) CPG.FRQCR;

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



#else
	// other CPUs

	#define BOARD_SYSTICK_FREQ CPU_FREQ

#endif

 // возврат позиции старшего значащего бита в числе
static uint_fast8_t
countbits2(
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
	uint_fast32_t divisor, // ожидаемый коэффициент деления всей системы
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
			const uint_fast32_t modulus = ((divisor + prescaler / 2) / prescaler) - substract;
			if (countbits2(modulus) <= width)
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

	//PRINTF("calcdivider: no parameters for divisor=%u, width=%u, taps=%08X\n", (unsigned long) divisor, (unsigned) width, (unsigned) taps);

	// Не подобрать комбинацию прескалера и делителя для ожидаемого коэффициента деления.
	* dvalue = (1U << width) - 1;	// просто пустышка - максимальный делитель
	return (rbi - 1);	// если надо обраьатывать невозможность подбора - возврат rbmax
}

static uint_fast32_t stm32f7xx_pllq_initialize(void);	// Настроить выход PLLQ на 48 МГц

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

#else
	//#warning Undefined CPUSTYLE_XXX
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
#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU
	local_delay_us(5);
#elif _WIN32
#else
	// Cortex A9
	__NOP();
	__NOP();
#endif
}

#if CPUSTYLE_STM32MP1

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
		else
		{
			ASSERT(0);
		}
	}
	#endif /* WITHELKEY */

	void
	TIM5_IRQHandler(void)
	{
		const portholder_t st = TIM5->SR;
		if ((st & TIM_SR_UIF) != 0)
		{
			TIM5->SR = ~ TIM_SR_UIF;	// clear UIF interrupt request
			spool_systimerbundle1();	// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
			spool_systimerbundle2();	// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
		}
		else
		{
			ASSERT(0);
		}
	}

	static uint_fast32_t gtimloadvalue;

	void
	SecurePhysicalTimer_IRQHandler(void)
	{
		//IRQ_ClearPending (SecurePhysicalTimer_IRQn);
		PL1_SetLoadValue(gtimloadvalue);

		spool_systimerbundle1();	// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
		spool_systimerbundle2();	// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
	}

#elif CPUSTYLE_R7S721

	// Таймер "тиков"
	static void OSTMI0TINT_IRQHandler(void)
	{
		spool_systimerbundle1();	// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
		spool_systimerbundle2();	// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
	}

	// Таймер электронного ключа
	static void OSTMI1TINT_IRQHandler(void)
	{
		spool_elkeybundle();
	}

#elif CPUSTYLE_XC7Z /* || CPUSTYLE_XCZU */

	// Используется только один из обработчиков

	// Global timer use
	void
	GTC_Handler(void)
	{
		GTC->GTISR = 0x0001;	// ckear interrupt
		spool_systimerbundle1();	// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
		spool_systimerbundle2();	// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
	}

	#if (__CORTEX_A == 5U) || (__CORTEX_A == 9U)
		// Private timer use
		void
		PTIM_Handler(void)
		{
			PTIM_ClearEventFlag();
			spool_systimerbundle1();	// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
			spool_systimerbundle2();	// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
		}
	#endif

#elif CPUSTYLE_STM32F

	void
	SysTick_Handler(void)
	{
	}

	void
	SysTick_Handler_Active(void)
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
	#endif /* WITHELKEY */

#elif CPUSTYLE_AT91SAM7S

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
				spool_systimerbundle1();	// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
			spool_systimerbundle2();
		}
	}

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	void
	SysTick_Handler(void)
	{
	}

	void RAMFUNC_NONILINE
	SysTick_Handler_Active(void)
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

#elif CPUSTYLE_ATMEGA

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


#endif

// Инициализация таймера, вызывающего прерывания с частотой TICKS_FREQUENCY.
// Должо вызываться позже настройки ADC
void
hardware_timer_initialize(uint_fast32_t ticksfreq)
{
	tickers_initialize();

#if CPUSTYLE_ARM_CM3 || CPUSTYLE_ARM_CM4 || CPUSTYLE_ARM_CM7

	arm_hardware_set_handler_system(SysTick_IRQn, SysTick_Handler_Active);		// разрешение прерывания игнорируется для системныз векторов
	SysTick_Config(calcdivround2(BOARD_SYSTICK_FREQ, ticksfreq));	// CMSIS устанавливает SysTick_CTRL_CLKSOURCE_Msk

#elif CPUSTYLE_ATMEGA328

	// ATMega328
	// Timer/Counter 2 initialization
	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, ticksfreq), ATMEGA_TIMER2_WIDTH, ATMEGA_TIMER2_TAPS, & value, 1);

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
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, ticksfreq), ATMEGA_TIMER0_WIDTH, ATMEGA_TIMER0_TAPS, & value, 1);
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
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, ticksfreq), ATMEGA128_TIMER0_WIDTH, ATMEGA128_TIMER0_TAPS, & value, 1);
	TCCR0 = (1U << WGM01) | (prei + 1);	// прескалер
	OCR0 = value;	// делитель - программирование полного периода

	TIMSK |= (1U << OCIE0);	// enable interrupt from Timer/Counter 0 - use TIMER0_COMP_vect

	TCNT0 = 0x00;

#elif CPUSTYLE_ATMEGA32
	// Classic ATMega32

	// Timer/Counter 0 initialization
	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, ticksfreq), ATMEGA_TIMER0_WIDTH, ATMEGA_TIMER0_TAPS, & value, 1);
	TCCR0 = (1U << WGM01) | (prei + 1);	// прескалер
	OCR0 = value;	// делитель - программирование полного периода

	TIMSK |= (1U << OCIE0);	// enable interrupt from Timer/Counter 0 - use TIMER0_COMP_vect

	TCNT0 = 0x00;

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	/* const uint_fast8_t prei = */ calcdivider(calcdivround2(CPU_FREQ, ticksfreq), AT91SAM7_PITPIV_WIDTH, AT91SAM7_PITPIV_TAPS, & value, 1);

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
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, ticksfreq), ATXMEGA_TIMER_WIDTH, ATXMEGA_TIMER_TAPS, & value, 1);
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

	arm_hardware_set_handler_system(OSTMI0TINT_IRQn, OSTMI0TINT_IRQHandler);

	OSTM0.OSTMnTS = 0x01u;      /* Start counting */

#elif CPUSTYLE_STM32MP1

	RCC->MP_APB1ENSETR = RCC_MP_APB1ENSETR_TIM5EN;   // подаем тактирование на TIM5
	(void) RCC->MP_APB1ENSETR;
	RCC->MP_APB1LPENSETR = RCC_MP_APB1LPENSETR_TIM5LPEN;   // подаем тактирование на TIM5
	(void) RCC->MP_APB1LPENSETR;

	TIM5->DIER = TIM_DIER_UIE;        	 // разрешить событие от таймера

	// Use basic timer
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(BOARD_TIM5_FREQ, ticksfreq), STM32F_TIM5_TIMER_WIDTH, STM32F_TIM5_TIMER_TAPS, & value, 1);

	TIM5->PSC = ((1UL << prei) - 1);
	TIM5->ARR = value;
	TIM5->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости - если счётчик успевал превысить значение ARR - считал до конца */

	arm_hardware_set_handler_system(TIM5_IRQn, TIM5_IRQHandler);

	// Prepare funcionality: use CNTP
	const uint_fast32_t gtimfreq = stm32mp1_get_hsi_freq();

	PL1_SetCounterFrequency(gtimfreq);	// CNTFRQ

	gtimloadvalue = calcdivround2(gtimfreq, ticksfreq) - 1;
	// Private timer use
	// Disable Private Timer and set load value
	PL1_SetControl(PL1_GetControl() & ~ 0x01);	// CNTP_CTL
	PL1_SetLoadValue(gtimloadvalue);	// CNTP_TVAL
	// Enable timer control
	PL1_SetControl(PL1_GetControl() | 0x01);

	//arm_hardware_set_handler_system(SecurePhysicalTimer_IRQn, SecurePhysicalTimer_IRQHandler);

#elif CPUSTYLE_XC7Z /* || CPUSTYLE_XCZU */

	#if 1
		const uint_fast32_t period = calcdivround2(CPU_FREQ, ticksfreq * 2);	// Global Timer runs with the system frequency / 2
		// Global timer use
		GTC->GTCLR = 0;

		GTC->GTCTRL = period;
		GTC->GTCTRH = 0;
		GTC->GTCOMPL = 0;
		GTC->GTCOMPH = 0;
		GTC->GTCAIR = period;
		GTC->GTCLR |= 0x0E;	// auto increment mode. IRQ_Enable, Comp_Enablea
		arm_hardware_set_handler_system(GlobalTimer_IRQn, GTC_Handler);

		GTC->GTCLR |= 0x1;	// Timer_Enable

	#else
		// (__CORTEX_A == 5U) || (__CORTEX_A == 9U)
		// Private timer use
		// Disable Private Timer and set load value
		PTIM_SetControl(0);
		PTIM_SetCurrentValue(0);
		PTIM_SetLoadValue(calcdivround2(CPU_FREQ, ticksfreq * 2));	// Private Timer runs with the system frequency / 2
		// Set bits: IRQ enable and Auto reload
		PTIM_SetControl(0x06U);

		arm_hardware_set_handler_system(PrivTimer_IRQn, PTIM_Handler);

		// Start the Private Timer
		PTIM_SetControl(PTIM_GetControl() | 0x01);

	#endif

#else
	#warning Undefined CPUSTYLE_XXX
#endif
}


#if CPUSTYLE_STM32MP1

// return 1 if CPU supports 800 MHz clock
uint_fast8_t stm32mp1_overdrived(void)
{
	RCC->MP_APB5ENSETR = RCC_MP_APB5ENSETR_BSECEN;
	(void) RCC->MP_APB5ENSETR;
	RCC->MP_APB5LPENSETR = RCC_MP_APB5LPENSETR_BSECLPEN;
	(void) RCC->MP_APB5LPENSETR;

	const unsigned rpn = ((* (volatile uint32_t *) RPN_BASE) & RPN_ID_Msk) >> RPN_ID_Pos;
	return (rpn & 0x80) != 0;
}

// Снижение тактовой частоты процессора для уменьшения потребления
void stm32mp1_pll1_slow(uint_fast8_t slow)
{
	//	0x0: HSI selected as MPU sub-system clock (hsi_ck) (default after reset)
	//	0x1: HSE selected as MPU sub-system clock (hse_ck)
	//	0x2: PLL1 selected as MPU sub-system clock (pll1_p_ck)
	//	0x3: PLL1 via MPUDIV is selected as MPU sub-system clock (pll1_p_ck / (2 ^ MPUDIV)).
	RCC->MPCKSELR = (RCC->MPCKSELR & ~ (RCC_MPCKSELR_MPUSRC_Msk)) |
			((slow ? 0x03uL : 0x02uL) << RCC_MPCKSELR_MPUSRC_Pos) |
			0;
	while((RCC->MPCKSELR & RCC_MPCKSELR_MPUSRCRDY_Msk) == 0)
		;
}


void stm32mp1_pll_initialize(void)
{

	//return;
	// PLL1 DIVN=0x1f. DIVM=0x4, DIVP=0x0
	// HSI 64MHz/5*32 = 409.6 MHz
	// HSI 64MHz/5*42 = 537.6 MHz
	//RCC->MP_APB5ENSETR = RCC_MP_APB5ENSETR_TZPCEN;
	//(void) RCC->MP_APB5ENSETR;
	RCC->TZCR &= ~ (RCC_TZCR_MCKPROT);
	//RCC->TZCR &= ~ (RCC_TZCR_TZEN);

	// compensation cell enable
	if (1)
	{
	//	Sequence to enable IO compensation:
	//	1. Ensure the CSI oscillator is enabled and ready (in RCC)
	//	2. Set SYSCFG_CMPENSETR.MCU_EN or MPU_EN=1
	//	3. Wait SYSCFG_CMPCR.READY = 1
	//	4. Set SYSCFG_CMPCR.SW_CTRL = 0

		/* SYSCFG clock enable */
		RCC->MP_APB3ENSETR = RCC_MP_APB3ENSETR_SYSCFGEN;
		(void) RCC->MP_APB3ENSETR;
		RCC->MP_APB3LPENSETR = RCC_MP_APB3LPENSETR_SYSCFGLPEN;
		(void) RCC->MP_APB3LPENSETR;

		// CSI ON (The CSI oscillator must be enabled and ready (controlled in RCC) before MPU_EN could be set to 1)
		RCC->OCENSETR = RCC_OCENSETR_CSION;
		(void) RCC->OCENSETR;
		while ((RCC->OCRDYR & RCC_OCRDYR_CSIRDY) == 0)
			;

		// IO compoensation cell enable
		SYSCFG->CMPENSETR = SYSCFG_CMPENSETR_MPU_EN;
		(void) SYSCFG->CMPENSETR;

		while ((SYSCFG->CMPCR & SYSCFG_CMPCR_READY_Msk) == 0)
			;

		SYSCFG->CMPCR = (SYSCFG->CMPCR & SYSCFG_CMPCR_SW_CTRL_Msk) |
			//(1uL << SYSCFG_CMPCR_SW_CTRL_Pos) |	// 1: IO compensation values come from RANSRC[3:0] and RAPSRC[3:0]
			(0uL << SYSCFG_CMPCR_SW_CTRL_Pos) |	// 0: IO compensation values come from ANSRC[3:0] and APSRC[3:0]
			0;
	}

	// переключение на HSI на всякий случай перед программированием PLL
	// HSI ON
	RCC->OCENSETR = RCC_OCENSETR_HSION;
	(void) RCC->OCENSETR;
	while ((RCC->OCRDYR & RCC_OCRDYR_HSIRDY) == 0)
		;

	// Wait for HSI ready
	while ((RCC->OCRDYR & RCC_OCRDYR_HSIRDY_Msk) == 0)
		;

	// HSIDIV
	//	0x0: Division by 1, hsi_ck (hsi_ker_ck) = 64 MHz (default after reset)
	//	0x1: Division by 2, hsi_ck (hsi_ker_ck) = 32 MHz
	//	0x2: Division by 4, hsi_ck (hsi_ker_ck) = 16 MHz
	//	0x3: Division by 8, hsi_ck (hsi_ker_ck) = 8 MHz
	RCC->HSICFGR = (RCC->HSICFGR & ! (RCC_HSICFGR_HSIDIV_Msk)) |
		(0uL << RCC_HSICFGR_HSIDIV_Pos) |
		0;
	(void) RCC->HSICFGR;

	// Wait for HSI DIVIDER ready
	while ((RCC->OCRDYR & RCC_OCRDYR_HSIDIVRDY_Msk) == 0)
		;

	//0x0: HSI selected as AXI sub-system clock (hsi_ck) (default after reset)
	//0x1: HSE selected as AXI sub-system clock (hse_ck)
	//0x2: PLL2 selected as AXI sub-system clock (pll2_p_ck)
	//others: axiss_ck is gated
	RCC->ASSCKSELR = (RCC->ASSCKSELR & ~ (RCC_ASSCKSELR_AXISSRC_Msk)) |
			(0x00 << RCC_ASSCKSELR_AXISSRC_Pos) |	// HSI
			0;
	while ((RCC->ASSCKSELR & RCC_ASSCKSELR_AXISSRCRDY_Msk) == 0)
		;

	//	0x0: HSI selected as MPU sub-system clock (hsi_ck) (default after reset)
	//	0x1: HSE selected as MPU sub-system clock (hse_ck)
	//	0x2: PLL1 selected as MPU sub-system clock (pll1_p_ck)
	//	0x3: PLL1 via MPUDIV is selected as MPU sub-system clock (pll1_p_ck / (2 ^ MPUDIV)).
	RCC->MPCKSELR = (RCC->MPCKSELR & ~ (RCC_MPCKSELR_MPUSRC_Msk)) |
		((uint_fast32_t) 0x00 << RCC_MPCKSELR_MPUSRC_Pos) |	// HSI
		0;
	while((RCC->MPCKSELR & RCC_MPCKSELR_MPUSRCRDY_Msk) == 0)
		;

	//	0x0: HSI selected as MCU sub-system clock (hsi_ck) (default after reset)
	//	0x1: HSE selected as MCU sub-system clock (hse_ck)
	//	0x2: CSI selected as MCU sub-system clock (csi_ck)
	//	0x3: PLL3 selected as MCU sub-system clock (pll3_p_ck).
	RCC->MSSCKSELR = (RCC->MSSCKSELR & ~ (RCC_MSSCKSELR_MCUSSRC_Msk)) |
		(0x00 << RCC_MSSCKSELR_MCUSSRC_Pos) |	// HSI
		0;
	while((RCC->MSSCKSELR & RCC_MSSCKSELR_MCUSSRCRDY_Msk) == 0)
		;

	// Для работы функции снижения тактовой частоты процессора
	//	0x0: The MPUDIV is disabled; i.e. no clock generated
	//	0x1: The mpuss_ck is equal to pll1_p_ck divided by 2 (default after reset)
	//	0x2: The mpuss_ck is equal to pll1_p_ck divided by 4
	//	0x3: The mpuss_ck is equal to pll1_p_ck divided by 8
	//	others: The mpuss_ck is equal to pll1_p_ck divided by 16
	RCC->MPCKDIVR = (RCC->MPCKDIVR &= ~ (RCC_MPCKDIVR_MPUDIV_Msk)) |
		(0x02uL << RCC_MPCKDIVR_MPUDIV_Pos) |	// pll1_p_ck divided by 4
		0;

	// Stop PLL4
	RCC->PLL4CR &= ~ RCC_PLL4CR_PLLON_Msk;
	(void) RCC->PLL4CR;
	while ((RCC->PLL4CR & RCC_PLL4CR_PLLON_Msk) != 0)
		;

	// Stop PLL3
	RCC->PLL3CR &= ~ RCC_PLL3CR_PLLON_Msk;
	(void) RCC->PLL3CR;
	while ((RCC->PLL3CR & RCC_PLL3CR_PLLON_Msk) != 0)
		;

	// Stop PLL2
	RCC->PLL2CR &= ~ RCC_PLL2CR_PLLON_Msk;
	(void) RCC->PLL2CR;
	while ((RCC->PLL2CR & RCC_PLL2CR_PLLON_Msk) != 0)
		;

	// Stop PLL1
	//RCC->PLL1CR &= ~ RCC_PLL1CR_DIVPEN_Msk;
	//(void) RCC->PLL1CR;
	RCC->PLL1CR &= ~ RCC_PLL1CR_PLLON_Msk;
	(void) RCC->PLL1CR;
	while ((RCC->PLL1CR & RCC_PLL1CR_PLLON_Msk) != 0)
		;

	#if WITHCPUXOSC
		// с внешним генератором
		RCC->OCENCLRR = RCC_OCENCLRR_HSEON;
		(void) RCC->OCENCLRR;
		while ((RCC->OCRDYR & RCC_OCRDYR_HSERDY) != 0)
			;
		RCC->OCENSETR = RCC_OCENSETR_DIGBYP;
		(void) RCC->OCENSETR;
		RCC->OCENSETR = RCC_OCENSETR_HSEBYP;
		(void) RCC->OCENSETR;
		RCC->OCENSETR = RCC_OCENSETR_HSEON;
		(void) RCC->OCENSETR;
		while ((RCC->OCRDYR & RCC_OCRDYR_HSERDY) == 0)
			;

	#elif WITHCPUXTAL
		//#error rr2
		// с внешним кварцем
		// HSE ON
		RCC->OCENSETR = RCC_OCENSETR_HSEON;
		(void) RCC->OCENSETR;
		while ((RCC->OCRDYR & RCC_OCRDYR_HSERDY) == 0)
			;

	#else
		// На внутреннем генераторе
		// выключаем внешний
		RCC->OCENCLRR = RCC_OCENCLRR_HSEON;
		(void) RCC->OCENCLRR;

	#endif /* WITHCPUXTAL */

#if 1 // pll1
	// PLL1, PLL2 source mux
	// 0x0: HSI selected as PLL clock (hsi_ck) (default after reset)
	// 0x1: HSE selected as PLL clock (hse_ck)
	RCC->RCK12SELR = (RCC->RCK12SELR & ~ (RCC_RCK12SELR_PLL12SRC_Msk)) |
	#if WITHCPUXOSC || WITHCPUXTAL
		// с внешним генератором
		// с внешним кварцем
		(0x01 << RCC_RCK12SELR_PLL12SRC_Pos) |	// HSE
	#else
		// На внутреннем генераторе
		(0x00 << RCC_RCK12SELR_PLL12SRC_Pos) |	// HSI
	#endif
		0;
	while ((RCC->RCK12SELR & RCC_RCK12SELR_PLL12SRCRDY_Msk) == 0)
		;

	RCC->PLL1CR = (RCC->PLL1CR & ~ (RCC_PLL1CR_DIVPEN_Msk | RCC_PLL1CR_DIVQEN_Msk | RCC_PLL1CR_DIVREN_Msk));
	(void) RCC->PLL1CR;

	RCC->PLL1CFGR1 = (RCC->PLL1CFGR1 & ~ (RCC_PLL1CFGR1_DIVN_Msk | RCC_PLL1CFGR1_DIVM1_Msk)) |
		((uint_fast32_t) (PLL1DIVM - 1) << RCC_PLL1CFGR1_DIVM1_Pos) |
		((uint_fast32_t) (PLL1DIVN - 1) << RCC_PLL1CFGR1_DIVN_Pos) |
		0;
	(void) RCC->PLL1CFGR1;

	RCC->PLL1CFGR2 = (RCC->PLL1CFGR2 & ~ (RCC_PLL1CFGR2_DIVP_Msk | RCC_PLL1CFGR2_DIVQ_Msk | RCC_PLL1CFGR2_DIVR_Msk)) |
		((uint_fast32_t) (PLL1DIVP - 1) << RCC_PLL1CFGR2_DIVP_Pos) |
		((uint_fast32_t) (PLL1DIVQ - 1) << RCC_PLL1CFGR2_DIVQ_Pos) |
		((uint_fast32_t) (PLL1DIVR - 1) << RCC_PLL1CFGR2_DIVR_Pos) |
		0;
	(void) RCC->PLL1CFGR2;

	RCC->PLL1CR |= RCC_PLL1CR_PLLON_Msk;
	(void) RCC->PLL1CR;
	while ((RCC->PLL1CR & RCC_PLL1CR_PLL1RDY_Msk) == 0)
		;

	RCC->PLL1CR &= ~ RCC_PLL1CR_SSCG_CTRL_Msk;
	(void) RCC->PLL1CR;

	RCC->PLL1CR |= RCC_PLL1CR_DIVPEN_Msk;	// P output enable
	(void) RCC->PLL1CR;
#endif // pll1

#if 1 // PLL2
	// PLL2
	RCC->PLL2CR = (RCC->PLL2CR & ~ (RCC_PLL2CR_DIVPEN_Msk | RCC_PLL2CR_DIVQEN_Msk | RCC_PLL2CR_DIVREN_Msk));
	(void) RCC->PLL2CR;

	RCC->PLL2CFGR1 = (RCC->PLL2CFGR1 & ~ (RCC_PLL2CFGR1_DIVN_Msk | RCC_PLL2CFGR1_DIVM2_Msk)) |
		((PLL2DIVN - 1) << RCC_PLL2CFGR1_DIVN_Pos) |
		((PLL2DIVM - 1) << RCC_PLL2CFGR1_DIVM2_Pos) |
		0;
	(void) RCC->PLL2CFGR1;

	RCC->PLL2CFGR2 = (RCC->PLL2CFGR2 & ~ (RCC_PLL2CFGR2_DIVP_Msk | RCC_PLL2CFGR2_DIVQ_Msk | RCC_PLL2CFGR2_DIVR_Msk)) |
		((uint_fast32_t) (PLL2DIVP - 1) << RCC_PLL2CFGR2_DIVP_Pos) |	// pll2_p_ck - AXI clock (1..128 -> 0x00..0x7f)
		((uint_fast32_t) (PLL2DIVQ - 1) << RCC_PLL2CFGR2_DIVQ_Pos) |	// GPU clock (1..128 -> 0x00..0x7f)
		((uint_fast32_t) (PLL2DIVR - 1) << RCC_PLL2CFGR2_DIVR_Pos) |	// DDR clock (1..128 -> 0x00..0x7f)
		0;
	(void) RCC->PLL2CFGR2;

	RCC->PLL2CR |= RCC_PLL2CR_PLLON_Msk;
	(void) RCC->PLL2CR;
	while ((RCC->PLL2CR & RCC_PLL2CR_PLL2RDY_Msk) == 0)
		;

	RCC->PLL2CR |= RCC_PLL2CR_DIVPEN_Msk;	// pll2_p_ck - AXI clock
	(void) RCC->PLL2CR;

	RCC->PLL2CR |= RCC_PLL2CR_DIVQEN_Msk;	// GPU clock
	(void) RCC->PLL2CR;

#if 1//WITHSDRAMHW
	// В загрузчике еще может и не быть этой периферии
	RCC->PLL2CR |= RCC_PLL2CR_DIVREN_Msk;	// DDR clock
	(void) RCC->PLL2CR;
#endif /* WITHSDRAMHW */

	RCC->PLL2CR &= ~ RCC_PLL2CR_SSCG_CTRL_Msk;
	(void) RCC->PLL2CR;
#endif // pll2

	// AXI, AHB5 and AHB6 clock divisor
	//	0x0: axiss_ck (default after reset)
	//	0x1: axiss_ck / 2
	//	0x2: axiss_ck / 3
	//	others: axiss_ck / 4
	RCC->AXIDIVR = (RCC->AXIDIVR & ~ (RCC_AXIDIVR_AXIDIV_Msk)) |
		((uint_fast32_t) (0x01 - 1) << RCC_AXIDIVR_AXIDIV_Pos) |	// div1 (no divide)
		0;
	while((RCC->AXIDIVR & RCC_AXIDIVR_AXIDIVRDY_Msk) == 0)
		;

	// APB1 Output divisor (output max 104.5 MHz)
	// Input MLHCK (209 MHz max)
	//0x0: mlhclk (default after reset)
	//0x1: mlhclk / 2
	//0x2: mlhclk / 4
	//0x3: mlhclk / 8
	//0x4: mlhclk / 16
	RCC->APB1DIVR = (RCC->APB1DIVR & ~ (RCC_APB1DIVR_APB1DIV_Msk)) |
		((uint_fast32_t) (1) << RCC_APB1DIVR_APB1DIV_Pos) |	// div2
		0;
	while((RCC->APB1DIVR & RCC_APB1DIVR_APB1DIVRDY_Msk) == 0)
		;

	// APB2 Output divisor (output max 104.5 MHz)
	// Input MLHCK (209 MHz max)
	//0x0: mlhclk (default after reset)
	//0x1: mlhclk / 2
	//0x2: mlhclk / 4
	//0x3: mlhclk / 8
	//0x4: mlhclk / 16
	RCC->APB2DIVR = (RCC->APB2DIVR & ~ (RCC_APB2DIVR_APB2DIV_Msk)) |
		((uint_fast32_t) (1) << RCC_APB2DIVR_APB2DIV_Pos) |	// div2
		0;
	while((RCC->APB2DIVR & RCC_APB2DIVR_APB2DIVRDY_Msk) == 0)
		;

	// APB3 Output divisor (output max 104.5 MHz)
	// Input MLHCK (209 MHz max)
	//0x0: hclk (default after reset)
	//0x1: hclk / 2
	//0x2: hclk / 4
	//0x3: hclk / 8
	//others: hclk / 16
	RCC->APB3DIVR = (RCC->APB3DIVR & ~ (RCC_APB3DIVR_APB3DIV_Msk)) |
		((uint_fast32_t) (1) << RCC_APB3DIVR_APB3DIV_Pos) |	// div2
		0;
	while((RCC->APB3DIVR & RCC_APB3DIVR_APB3DIVRDY_Msk) == 0)
		;

	// APB4 Output divisor
	//	0x0: aclk (default after reset)
	//	0x1: aclk / 2
	//	0x2: aclk / 4
	//	0x3: aclk / 8
	//	others: aclk / 16
	RCC->APB4DIVR = (RCC->APB4DIVR & ~ (RCC_APB4DIVR_APB4DIV_Msk)) |
		((uint_fast32_t) (0x02 - 1) << RCC_APB4DIVR_APB4DIV_Pos) |	// div2
		0;
	while((RCC->APB4DIVR & RCC_APB4DIVR_APB4DIVRDY_Msk) == 0)
		;

	// APB5 Output divisor
	RCC->APB5DIVR = (RCC->APB5DIVR & ~ (RCC_APB5DIVR_APB5DIV_Msk)) |
		((uint_fast32_t) (0x04 - 1) << RCC_APB5DIVR_APB5DIV_Pos) |	// div4
		0;
	while((RCC->APB5DIVR & RCC_APB5DIVR_APB5DIVRDY_Msk) == 0)
		;

	// Значения 0x02 и 0x03 проверены - 0x03 действительно ниже тактовая
	//	0x0: HSI selected as MPU sub-system clock (hsi_ck) (default after reset)
	//	0x1: HSE selected as MPU sub-system clock (hse_ck)
	//	0x2: PLL1 selected as MPU sub-system clock (pll1_p_ck)
	//	0x3: PLL1 via MPUDIV is selected as MPU sub-system clock (pll1_p_ck / (2 ^ MPUDIV)).
	RCC->MPCKSELR = (RCC->MPCKSELR & ~ (RCC_MPCKSELR_MPUSRC_Msk)) |
		((uint_fast32_t) 0x02uL << RCC_MPCKSELR_MPUSRC_Pos) |	// PLL1_P
		0;
	while((RCC->MPCKSELR & RCC_MPCKSELR_MPUSRCRDY_Msk) == 0)
		;

	// per_ck source clock selection
	//0x0: hsi_ker_ck clock selected (default after reset)
	//0x1: csi_ker_ck clock selected
	//0x2: hse_ker_ck clock selected
	//0x3: Clock disabled
	RCC->CPERCKSELR = (RCC->CPERCKSELR & ~ (RCC_CPERCKSELR_CKPERSRC_Msk)) |
		(0x00 << RCC_CPERCKSELR_CKPERSRC_Pos) |	// hsi_ker_ck
		0;
	(void) RCC->CPERCKSELR;

	//0x0: HSI selected as AXI sub-system clock (hsi_ck) (default after reset)
	//0x1: HSE selected as AXI sub-system clock (hse_ck)
	//0x2: PLL2 selected as AXI sub-system clock (pll2_p_ck)
	//others: axiss_ck is gated
	// axiss_ck 266 MHz Max
	RCC->ASSCKSELR = (RCC->ASSCKSELR & ~ (RCC_ASSCKSELR_AXISSRC_Msk)) |
			((uint_fast32_t) 0x02 << RCC_ASSCKSELR_AXISSRC_Pos) |	// pll2_p_ck
			0;
	while ((RCC->ASSCKSELR & RCC_ASSCKSELR_AXISSRCRDY_Msk) == 0)
		;

	// PLL3
	// PLL3 source mux
	//0x0: HSI selected as PLL clock (hsi_ck) (default after reset)
	//0x1: HSE selected as PLL clock (hse_ck)
	//0x2: CSI selected as PLL clock (csi_ck)
	//0x3: No clock send to DIVMx divisor and PLLs
	RCC->RCK3SELR = (RCC->RCK3SELR & ~ (RCC_RCK3SELR_PLL3SRC_Msk)) |
	#if WITHCPUXOSC || WITHCPUXTAL
		// с внешним генератором
		// с внешним кварцем
		((uint_fast32_t) 0x01 << RCC_RCK3SELR_PLL3SRC_Pos) |	// HSE
	#else
		// На внутреннем генераторе
		((uint_fast32_t) 0x00 << RCC_RCK3SELR_PLL3SRC_Pos) |	// HSI
	#endif
		0;
	while ((RCC->RCK3SELR & RCC_RCK3SELR_PLL3SRCRDY_Msk) == 0)
		;

#if 1//WITHUART1HW
	// В загрузчике еще может и не быть этой периферии
	// usart1
	//	0x0: pclk5 clock selected as kernel peripheral clock (default after reset)
	//	0x1: pll3_q_ck clock selected as kernel peripheral clock
	//	0x2: hsi_ker_ck clock selected as kernel peripheral clock
	//	0x3: csi_ker_ck clock selected as kernel peripheral clock
	//	0x4: pll4_q_ck clock selected as kernel peripheral clock
	//	0x5: hse_ker_ck clock selected as kernel peripheral clock
	RCC->UART1CKSELR = (RCC->UART1CKSELR & ~ (RCC_UART1CKSELR_UART1SRC_Msk)) |
		(0x02 << RCC_UART1CKSELR_UART1SRC_Pos) | // HSI
		0;
	(void) RCC->UART1CKSELR;
#endif /* WITHUART1HW */

#if 1//WITHUART2HW || WITHUART4HW
	// В загрузчике еще может и не быть этой периферии
	// UART2, UART4
	//	0x0: pclk1 clock selected as kernel peripheral clock (default after reset)
	//	0x1: pll4_q_ck clock selected as kernel peripheral clock
	//	0x2: hsi_ker_ck clock selected as kernel peripheral clock
	//	0x3: csi_ker_ck clock selected as kernel peripheral clock
	//	0x4: hse_ker_ck clock selected as kernel peripheral clock
	RCC->UART24CKSELR = (RCC->UART24CKSELR & ~ (RCC_UART24CKSELR_UART24SRC_Msk)) |
		((uint_fast32_t) 0x02 << RCC_UART24CKSELR_UART24SRC_Pos) |	// hsi_ker_ck
		//((uint_fast32_t) 0x00 << RCC_UART24CKSELR_UART24SRC_Pos) |	// pclk1
		0;
	(void) RCC->UART24CKSELR;
#endif /* WITHUART2HW || WITHUART4HW */

#if 1//WITHUART3HW || WITHUART5HW
	// В загрузчике еще может и не быть этой периферии
	// UART3, UART5
	//	0x0: pclk1 clock selected as kernel peripheral clock (default after reset)
	//	0x1: pll4_q_ck clock selected as kernel peripheral clock
	//	0x2: hsi_ker_ck clock selected as kernel peripheral clock
	//	0x3: csi_ker_ck clock selected as kernel peripheral clock
	//	0x4: hse_ker_ck clock selected as kernel peripheral clock
	//	others: reserved, the kernel clock is disabled
	RCC->UART35CKSELR = (RCC->UART35CKSELR & ~ (RCC_UART35CKSELR_UART35SRC_Msk)) |
		(0x02 << RCC_UART35CKSELR_UART35SRC_Pos) |	// hsi_ker_ck
		//(0x00 << RCC_UART35CKSELR_UART35SRC_Pos) |	// pclk1
		0;
	(void) RCC->UART35CKSELR;
#endif /* WITHUART3HW || WITHUART5HW */

#if 1//WITHUART7HW || WITHUART8HW
	// В загрузчике еще может и не быть этой периферии
	// UART7, UART8
	//0x0: pclk1 clock selected as kernel peripheral clock (default after reset)
	//0x1: pll4_q_ck clock selected as kernel peripheral clock
	//0x2: hsi_ker_ck clock selected as kernel peripheral clock
	//0x3: csi_ker_ck clock selected as kernel peripheral clock
	//0x4: hse_ker_ck clock selected as kernel peripheral clock
	RCC->UART78CKSELR = (RCC->UART78CKSELR & ~ (RCC_UART78CKSELR_UART78SRC_Msk)) |
		((uint_fast32_t) 0x02 << RCC_UART78CKSELR_UART78SRC_Pos) |	// hsi_ker_ck
		//((uint_fast32_t) 0x00 << RCC_UART78CKSELR_UART78SRC_Pos) |	// pclk1
		0;
	(void) RCC->UART78CKSELR;
#endif /* WITHUART7HW || WITHUART8HW */

#if 1//WITHSDHCHW
	// В загрузчике еще может и не быть этой периферии
	// SDMMC1
	//	0x0: hclk6 clock selected as kernel peripheral clock
	//	0x1: pll3_r_ck clock selected as kernel peripheral clock
	//	0x2: pll4_p_ck clock selected as kernel peripheral clock
	//	0x3: hsi_ker_ck clock selected as kernel peripheral clock (default after reset)
	//	others: reserved, the kernel clock is disabled
	RCC->SDMMC12CKSELR = (RCC->SDMMC12CKSELR & ~ (RCC_SDMMC12CKSELR_SDMMC12SRC_Msk)) |
		((uint_fast32_t) 0x3 << RCC_SDMMC12CKSELR_SDMMC12SRC_Pos) |	// hsi_ker_ck
		0;
	(void) RCC->SDMMC12CKSELR;
#endif /* WITHSDHCHW */

#if 1//WITHSPIHW
	// В загрузчике еще может и не быть этой периферии
	//0x0: pll4_p_ck clock selected as kernel peripheral clock (default after reset)
	//0x1: pll3_q_ck clock selected as kernel peripheral clock
	//0x2: I2S_CKIN clock selected as kernel peripheral clock
	//0x3: per_ck clock selected as kernel peripheral clock
	//0x4: pll3_r_ck clock selected as kernel peripheral clock
	RCC->SPI2S1CKSELR = (RCC->SPI2S1CKSELR & ~ (RCC_SPI2S1CKSELR_SPI1SRC_Msk)) |
		((uint_fast32_t) 0x03 << RCC_SPI2S1CKSELR_SPI1SRC_Pos) |	// per_ck
		0;
	(void) RCC->SPI2S1CKSELR;
#endif /* WITHSPIHW */

#if 1//WIHSPIDFHW
	// В загрузчике еще может и не быть этой периферии
	//0x0: aclk clock selected as kernel peripheral clock (default after reset)
	//0x1: pll3_r_ck clock selected as kernel peripheral clock
	//0x2: pll4_p_ck clock selected as kernel peripheral clock
	//0x3: per_ck clock selected as kernel peripheral clock
	RCC->QSPICKSELR = (RCC->QSPICKSELR & ~ (RCC_QSPICKSELR_QSPISRC_Msk)) |
	((uint_fast32_t) 0x00 << RCC_QSPICKSELR_QSPISRC_Pos) |	// aclk (AXI CLOCK)
		0;
	(void) RCC->QSPICKSELR;
#endif /* WIHSPIDFHW */

	// prescaler value of timers located into APB1 domain
	// TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM12, TIM13 and TIM14.
	//	0: The Timers kernel clock is equal to mlhclk if APB1DIV is corresponding to a division by 1
	//	or 2, else it is equal to 2 x Fpclk1 (default after reset)
	//	1: The Timers kernel clock is equal to mlhclk if APB1DIV is corresponding to division by 1, 2
	//	or 4, else it is equal to 4 x Fpclk1
	RCC->TIMG1PRER = (RCC->TIMG1PRER & ~ (RCC_TIMG1PRER_TIMG1PRE_Msk)) |
		((uint_fast32_t) 0x00 << RCC_TIMG1PRER_TIMG1PRE_Pos) |
		0;
	(void) RCC->TIMG1PRER;
	while ((RCC->TIMG1PRER & RCC_TIMG1PRER_TIMG1PRERDY_Msk) == 0)
		;

	// TIM1, TIM8, TIM15, TIM16, and TIM17.
	//	0: The Timers kernel clock is equal to mlhclk if APB2DIV is corresponding to a division by 1
	//	or 2, else it is equal to 2 x Fpclk2 (default after reset)
	//	1: The Timers kernel clock is equal to mlhclk if APB2DIV is corresponding to division by 1, 2
	//	or 4, else it is equal to 4 x Fpclk2
	RCC->TIMG2PRER = (RCC->TIMG2PRER & ~ (RCC_TIMG2PRER_TIMG2PRE_Msk)) |
		(0x00 << RCC_TIMG2PRER_TIMG2PRE_Pos) |
		0;
	(void) RCC->TIMG2PRER;
	while ((RCC->TIMG2PRER & RCC_TIMG2PRER_TIMG2PRERDY_Msk) == 0)
		;

	//	ADC1 and 2 kernel clock source selection
	//	Set and reset by software.
	//	0x0: pll4_r_ck clock selected as kernel peripheral clock (default after reset)
	//	0x1: per_ck clock selected as kernel peripheral clock
	//	0x2: pll3_q_ck clock selected as kernel peripheral
	RCC->ADCCKSELR = (RCC->ADCCKSELR & ~ (RCC_ADCCKSELR_ADCSRC_Msk)) |
		(0x01 << RCC_ADCCKSELR_ADCSRC_Pos) |
		0;
	(void) RCC->ADCCKSELR;

}

void stm32mp1_usb_clocks_initialize(void)
{
	if (RCC_USBCKSELR_USBOSRC_VAL == 0x00 || RCC_USBCKSELR_USBPHYSRC_VAL == 0x01)
	{
		// Stop PLL4
		RCC->PLL4CR &= ~ RCC_PLL4CR_PLLON_Msk;
		(void) RCC->PLL4CR;
		while ((RCC->PLL4CR & RCC_PLL4CR_PLLON_Msk) != 0)
			;
		// PLL4 source mux
		//	0x0: HSI selected as PLL clock (hsi_ck) (default after reset)
		//	0x1: HSE selected as PLL clock (hse_ck)
		//	0x2: CSI selected as PLL clock (csi_ck)
		//	0x3: Signal I2S_CKIN used as reference clock
		RCC->RCK4SELR = (RCC->RCK4SELR & ~ (RCC_RCK4SELR_PLL4SRC_Msk)) |
		#if WITHCPUXOSC || WITHCPUXTAL
			// с внешним генератором
			// с внешним кварцем
			((uint_fast32_t) 0x01 << RCC_RCK4SELR_PLL4SRC_Pos) |	// HSE
		#else
			// На внутреннем генераторе
			((uint_fast32_t) 0x00 << RCC_RCK4SELR_PLL4SRC_Pos) |	// HSI
		#endif
			0;
		while ((RCC->RCK4SELR & RCC_RCK4SELR_PLL4SRCRDY_Msk) == 0)
			;

		RCC->PLL4CFGR1 = (RCC->PLL4CFGR1 & ~ (RCC_PLL4CFGR1_DIVN_Msk | RCC_PLL4CFGR1_DIVM4_Msk)) |
	        ((uint_fast32_t) (PLL4DIVN - 1) << RCC_PLL4CFGR1_DIVN_Pos) |
	        ((uint_fast32_t) (PLL4DIVM - 1) << RCC_PLL4CFGR1_DIVM4_Pos) |
	        0;
	    (void) RCC->PLL4CFGR1;

	    RCC->PLL4CFGR2 = (RCC->PLL4CFGR2 & ~ (RCC_PLL4CFGR2_DIVR_Msk)) |
			((uint_fast32_t) (PLL4DIVR - 1) << RCC_PLL4CFGR2_DIVR_Pos) |	// USBPHY clock (1..128 -> 0x00..0x7f)
			0;
		(void) RCC->PLL4CFGR2;

		RCC->PLL4CR |= RCC_PLL4CR_DIVREN_Msk;	// USBPHY clock
		(void) RCC->PLL4CR;

		// Start PLL4
		RCC->PLL4CR |= RCC_PLL4CR_PLLON_Msk;
		while ((RCC->PLL4CR & RCC_PLL4CR_PLL4RDY_Msk) == 0)
			;
	}

	//	In addition, if the USBO is used in full-speed mode only, the application can choose the
	//	48 MHz clock source to be provided to the USBO:
	// USBOSRC
	//	0: pll4_r_ck clock selected as kernel peripheral clock (default after reset)
	//	1: clock provided by the USB PHY (rcc_ck_usbo_48m) selected as kernel peripheral clock
	// USBPHYSRC
	//  0x0: hse_ker_ck clock selected as kernel peripheral clock (default after reset)
	//  0x1: pll4_r_ck clock selected as kernel peripheral clock
	//  0x2: hse_ker_ck/2 clock selected as kernel peripheral clock
	RCC->USBCKSELR = (RCC->USBCKSELR & ~ (RCC_USBCKSELR_USBOSRC_Msk | RCC_USBCKSELR_USBPHYSRC_Msk)) |
		(RCC_USBCKSELR_USBOSRC_VAL << RCC_USBCKSELR_USBOSRC_Pos) |	// 50 MHz max rcc_ck_usbo_48m or pll4_r_ck (можно использовать только 48 МГц)
		(RCC_USBCKSELR_USBPHYSRC_VAL << RCC_USBCKSELR_USBPHYSRC_Pos) |	// 38.4 MHz max hse_ker_ck	- входная частота для PHYC PLL
		0;
	(void) RCC->USBCKSELR;

}

void stm32mp1_audio_clocks_initialize(void)
{
	// Stop PLL3
	RCC->PLL3CR &= ~ RCC_PLL3CR_PLLON_Msk;
	(void) RCC->PLL3CR;
	while ((RCC->PLL3CR & RCC_PLL3CR_PLLON_Msk) != 0)
		;
	// PLL3
	// PLL3 source mux
	//	0x0: HSI selected as PLL clock (hsi_ck) (default after reset)
	//	0x1: HSE selected as PLL clock (hse_ck)
	//	0x2: CSI selected as PLL clock (csi_ck)
	//	0x3: No clock send to DIVMx divider and PLLs
	RCC->RCK3SELR = (RCC->RCK3SELR & ~ (RCC_RCK3SELR_PLL3SRC_Msk)) |
	#if WITHCPUXOSC || WITHCPUXTAL
		// с внешним генератором
		// с внешним кварцем
		((uint_fast32_t) 0x01 << RCC_RCK3SELR_PLL3SRC_Pos) |	// HSE
	#else
		// На внутреннем генераторе
		((uint_fast32_t) 0x00 << RCC_RCK3SELR_PLL3SRC_Pos) |	// HSI
	#endif
		0;
	while ((RCC->RCK3SELR & RCC_RCK3SELR_PLL3SRCRDY_Msk) == 0)
		;

//	RCC->PLL3CR = (RCC->PLL3CR & ~ (RCC_PLL3CR_DIVPEN_Msk | RCC_PLL3CR_DIVQEN_Msk | RCC_PLL3CR_DIVREN_Msk));
//	(void) RCC->PLL3CR;

	RCC->PLL3CFGR1 = (RCC->PLL3CFGR1 & ~ (RCC_PLL3CFGR1_DIVM3_Msk | RCC_PLL3CFGR1_DIVN_Msk)) |
		((uint_fast32_t) (PLL3DIVM - 1) << RCC_PLL3CFGR1_DIVM3_Pos) |
		((uint_fast32_t) (PLL3DIVN - 1) << RCC_PLL3CFGR1_DIVN_Pos) |
		0;
	(void) RCC->PLL3CFGR1;

	RCC->PLL3CFGR2 = (RCC->PLL3CFGR2 & ~ (RCC_PLL3CFGR2_DIVQ_Msk)) |
		((uint_fast32_t) (PLL3DIVQ - 1) << RCC_PLL3CFGR2_DIVQ_Pos) |	// pll3_p_ck - xxxxx (1..128 -> 0x00..0x7f)
		0;
	(void) RCC->PLL3CFGR2;

	RCC->PLL3CR |= RCC_PLL2CR_DIVQEN_Msk;
	(void) RCC->PLL3CR;

	RCC->PLL3CR &= ~ RCC_PLL3CR_SSCG_CTRL_Msk;
	(void) RCC->PLL3CR;

	RCC->PLL3CR |= RCC_PLL3CR_PLLON_Msk;
	while ((RCC->PLL3CR & RCC_PLL3CR_PLL3RDY_Msk) == 0)
		;
}

void hardware_set_dotclock(unsigned long dotfreq)
{
	const uint_fast32_t pll4divq = calcdivround2(stm32mp1_get_pll4_freq(), dotfreq);
	ASSERT(pll4divq >= 1);

	// Stop PLL4
	RCC->PLL4CR &= ~ RCC_PLL4CR_PLLON_Msk;
	(void) RCC->PLL4CR;
	while ((RCC->PLL4CR & RCC_PLL4CR_PLLON_Msk) != 0)
		;

	// PLL4 source mux
	//	0x0: HSI selected as PLL clock (hsi_ck) (default after reset)
	//	0x1: HSE selected as PLL clock (hse_ck)
	//	0x2: CSI selected as PLL clock (csi_ck)
	//	0x3: Signal I2S_CKIN used as reference clock
	RCC->RCK4SELR = (RCC->RCK4SELR & ~ (RCC_RCK4SELR_PLL4SRC_Msk)) |
	#if WITHCPUXOSC || WITHCPUXTAL
		// с внешним генератором
		// с внешним кварцем
		((uint_fast32_t) 0x01 << RCC_RCK4SELR_PLL4SRC_Pos) |	// HSE
	#else
		// На внутреннем генераторе
		((uint_fast32_t) 0x00 << RCC_RCK4SELR_PLL4SRC_Pos) |	// HSI
	#endif
		0;
	while ((RCC->RCK4SELR & RCC_RCK4SELR_PLL4SRCRDY_Msk) == 0)
		;

	RCC->PLL4CFGR1 = (RCC->PLL4CFGR1 & ~ (RCC_PLL4CFGR1_DIVM4_Msk | RCC_PLL4CFGR1_DIVN_Msk)) |
		((uint_fast32_t) (PLL4DIVM - 1) << RCC_PLL4CFGR1_DIVM4_Pos) |
		((uint_fast32_t) (PLL4DIVN - 1) << RCC_PLL4CFGR1_DIVN_Pos) |
		0;
	(void) RCC->PLL4CFGR1;

	RCC->PLL4CFGR2 = (RCC->PLL4CFGR2 & ~ (RCC_PLL4CFGR2_DIVQ_Msk)) |
		((pll4divq - 1) << RCC_PLL4CFGR2_DIVQ_Pos) |	// LTDC clock (1..128 -> 0x00..0x7f)
		0;
	(void) RCC->PLL4CFGR2;

	RCC->PLL4CR |= RCC_PLL4CR_DIVQEN_Msk;	// LTDC clock
	(void) RCC->PLL4CR;

	RCC->PLL4CR &= ~ RCC_PLL4CR_SSCG_CTRL_Msk;
	(void) RCC->PLL4CR;

	// Start PLL4
	RCC->PLL4CR |= RCC_PLL4CR_PLLON_Msk;
	while ((RCC->PLL4CR & RCC_PLL4CR_PLL4RDY_Msk) == 0)
		;
}

// округление тактовой частоты дисплейного контроллера к возможностям системы синхронизации
unsigned long hardware_get_dotclock(unsigned long dotfreq)
{
	const uint_fast32_t pll4divq = calcdivround2(stm32mp1_get_pll4_freq(), dotfreq);
	return stm32mp1_get_pll4_freq() / pll4divq;
}

#endif /* CPUSTYLE_STM32MP1 */


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
static void sam7s_pll_init_clock_xtal(
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
static void sam7s_pll_init_clock_from_xtal(void)
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
void at91sam9x_pll_initialize(
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
	program_mckr_pres(PMC_MCKR_PRES_CLK_2);	// with /2 divisor
	program_mckr_css(PMC_MCKR_CSS_PLLA_CLK);
}

// If a new value for CSS field corresponds to Main Clock
static void program_mckr_switchtomain(void)
{
	program_mckr_css(PMC_MCKR_CSS_MAIN_CLK);
#ifdef PMC_MCKR_PRES_CLK_1
	program_mckr_pres(PMC_MCKR_PRES_CLK_1);	// w/o divisor
#else
	program_mckr_pres(PMC_MCKR_PRES_CLK);	// w/o divisor
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
	program_mckr_pres(PMC_MCKR_PRES_CLK_1);	// w/o divisor
#else
	program_mckr_pres(PMC_MCKR_PRES_CLK);	// w/o divisor
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
static void sam3s_init_clock_12_RC12(void)
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


#if CPUSTYLE_STM32F7XX
// Настроить выход PLLQ на 48 МГц
static uint_fast32_t stm32f7xx_pllq_initialize(void)
{
	const uint32_t stm32f4xx_pllq = calcdivround2(stm32f7xx_get_pll_freq(), 48000000uL);	// Как было сделано при инициализации PLL
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

// Настроить выход PLLQ на 48 МГц
static uint_fast32_t stm32f7xx_pllq_initialize(void)
{
	const uint32_t stm32f4xx_pllq = calcdivround2(stm32f4xx_get_pll_freq(), 48000000uL);	// Как было сделано при инициализации PLL
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

static void
stm32f4xx_pll_initialize(void)
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

    // switch CPU clock to HSI before PLL programming
	RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_SW)) |
        RCC_CFGR_SW_HSI |
        0;
	while ((RCC->CFGR & RCC_CFGR_SWS) != (RCC_CFGR_SWS_0 * 0))
		;
	RCC->CR &= ~ RCC_CR_PLLON;				// Выключил PLL

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

	RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2 | RCC_CFGR_SW)) |
#if defined(STM32F401xC)
		(RCC_CFGR_HPRE_0 * 8) |		// AHB clock division factor. 0x08: system clock divided by 2s
		(RCC_CFGR_PPRE1_0 * 4) |	// APB1 prescaler 0x05: AHB clock divided by 2: понизил частоту APB1 до 42МГц
		(RCC_CFGR_PPRE2_0 * 0) |	// APB2 prescaler 0x04: AHB clock divided by 1: 84MHz
#else
		(RCC_CFGR_HPRE_0 * 0) |		// AHB clock division factor. 0x00: system clock divided by 1
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
stm32f4xx_pllsai_initialize(void)
{
	//#error TODO: write code to imitialize SAI PLL and LTDC output divisor
	/* для устройств на шине APB2 (up to 72 MHz) */
	auto uint_fast32_t
	calcdivround_saifreq(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		//#error TODO: check freq at outputs vsync/hsync
		return (stm32f4xx_get_pllsai_freq() + freq / 2) / freq;
	}

	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_saifreq(display_getdotclock(& vdmode0)), STM32F_LTDC_DIV_WIDTH, STM32F_LTDC_DIV_TAPS, & value, 0);
	ASSERT(value >= 2);
	PRINTF(PSTR("stm32f4xx_pllsai_initialize: value=%u, prei=%u\n"), value, prei);
	// Настройка PLLSAI
	// Частота сравнения та же самая, что и в основной PLL
	// RCC_PLLSAICFGR_PLLSAIQ используется, если для SAI используется отдельная PLL - эта.
	RCC->PLLSAICFGR = (RCC->PLLSAICFGR & ~ (RCC_PLLSAICFGR_PLLSAIN | /*RCC_PLLSAICFGR_PLLSAIQ | */ RCC_PLLSAICFGR_PLLSAIR)) |
		((SAIREF1_MUL << RCC_PLLSAICFGR_PLLSAIN_Pos) & RCC_PLLSAICFGR_PLLSAIN) |	// PLLI2SN bits = multiplier, freq=192..432 MHz, vale = 2..432
		((value * RCC_PLLSAICFGR_PLLSAIR_0) & RCC_PLLSAICFGR_PLLSAIR) |	// PLLI2SR bits - output divisor, 2..7
		0;

	RCC->DCKCFGR = (RCC->DCKCFGR & ~ RCC_DCKCFGR_PLLSAIDIVR) |
		((prei << RCC_DCKCFGR_PLLSAIDIVR_Pos) & RCC_DCKCFGR_PLLSAIDIVR) |	// division factor for LCD_CLK. 2: PLLSAIDIVR = /8  3: PLLSAIDIVR = /16
		0;

	RCC->CR |= RCC_CR_PLLSAION;				// Включил PLL
	while ((RCC->CR & RCC_CR_PLLSAIRDY) == 0)	// пока заработает PLL
		;
}

void hardware_set_dotclock(unsigned long dotfreq)
{
	//#error TODO: write code to imitialize SAI PLL and LTDC output divisor
	/* для устройств на шине APB2 (up to 72 MHz) */
	auto uint_fast32_t
	calcdivround_saifreq(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		//#error TODO: check freq at outputs vsync/hsync
		return (stm32f4xx_get_pllsai_freq() + freq / 2) / freq;
	}

	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_saifreq(display_getdotclock(& vdmode0)), STM32F_LTDC_DIV_WIDTH, STM32F_LTDC_DIV_TAPS, & value, 0);
	ASSERT(value >= 2);
	// Настройка PLLSAI
	// Частота сравнения та же самая, что и в основной PLL
	// RCC_PLLSAICFGR_PLLSAIQ используется, если для SAI используется отдельная PLL - эта.
	RCC->PLLSAICFGR = (RCC->PLLSAICFGR & ~ (RCC_PLLSAICFGR_PLLSAIN | /*RCC_PLLSAICFGR_PLLSAIQ | */ RCC_PLLSAICFGR_PLLSAIR)) |
		((SAIREF1_MUL << RCC_PLLSAICFGR_PLLSAIN_Pos) & RCC_PLLSAICFGR_PLLSAIN) |	// PLLI2SN bits = multiplier, freq=192..432 MHz, vale = 2..432
		((value * RCC_PLLSAICFGR_PLLSAIR_0) & RCC_PLLSAICFGR_PLLSAIR) |	// PLLI2SR bits - output divisor, 2..7
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
stm32f4xx_MCOx_test(void)
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
stm32f7xx_pll_initialize(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;     // включить тактирование альтернативных функций
	__DSB();
	SYSCFG->CMPCR |= SYSCFG_CMPCR_CMP_PD;	// enable i/o compensaion cell
	while ((SYSCFG->CMPCR & SYSCFG_CMPCR_READY) == 0)
		;

	RCC->CR |= RCC_CR_HSION;		//включаю внутренний генератор
	while ((RCC->CR & RCC_CR_HSIRDY) == 0)//жду пока не заработает
		;

    // switch CPU clock to HSI before PLL programming
	RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_SW)) |
        RCC_CFGR_SW_HSI |
        0;
	while ((RCC->CFGR & RCC_CFGR_SWS) != (RCC_CFGR_SWS_0 * 0))
		;
	RCC->CR &= ~ RCC_CR_PLLON;				// Выключил PLL

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
		(RCC_CFGR_HPRE_0 * 0) |		// AHB clock division factor. 0x00: system clock divided by 1
		(RCC_CFGR_PPRE1_0 * 5) |	// APB1 prescaler 0x05: AHB clock divided by 4: понизил частоту APB1 до 42МГц
		(RCC_CFGR_PPRE2_0 * 4) |	// APB2 prescaler 0x04: AHB clock divided by 2: 84MHz
		RCC_CFGR_SW_PLL; // PLL as system clock

	while ((RCC->CFGR & RCC_CFGR_SWS) != (RCC_CFGR_SWS_0 * 2))
		;

	RCC->DCKCFGR1 = (RCC->DCKCFGR1 & ~ RCC_DCKCFGR1_TIMPRE) |
		(0 << RCC_DCKCFGR1_TIMPRE_Pos)	|	// Timers clocks prescalers selection
		0;

	#if WITHSAICLOCKFROMPIN
		RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_I2SSRC)) |
				1 * RCC_CFGR_I2SSRC |	// 1: External clock mapped on the I2S_CKIN pin used as I2S clock source
				0;
		// SAI part of DCKCFGR1
		RCC->DCKCFGR1 = (RCC->DCKCFGR1 & ~ (RCC_DCKCFGR1_SAI1SEL | RCC_DCKCFGR1_SAI2SEL)) |
			(2 * RCC_DCKCFGR1_SAI1SEL_0) |	// 10: SAI1 clock frequency = Alternate function input frequency
			(2 * RCC_DCKCFGR1_SAI2SEL_0) |	// 10: SAI2 clock frequency = Alternate function input frequency
			0;
	#elif WITHSAICLOCKFROMI2S

	#endif /* WITHSAICLOCKFROMPIN */

	//RCC->CR &= ~ RCC_CR_HSION;		//HSI DISABLE

}

#if WITHUSESAIPLL

static void
stm32f7xx_pllsai_initialize(void)
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
	const uint_fast8_t prei = calcdivider(calcdivround_saifreq(display_getdotclock(& vdmode0)), STM32F_LTDC_DIV_WIDTH, STM32F_LTDC_DIV_TAPS, & value, 0);
	ASSERT(value >= 2);
	PRINTF(PSTR("stm32f7xx_pllsai_initialize: value=%u, prei=%u\n"), value, prei);


	// Настройка PLLSAI
	// Частота сравнения та же самая, что и в основной PLL
	// RCC_PLLSAICFGR_PLLSAIQ используется, если для SAI используется отдельная PLL - эта.
	RCC->PLLSAICFGR = (RCC->PLLSAICFGR & ~ (RCC_PLLSAICFGR_PLLSAIN | /*RCC_PLLSAICFGR_PLLSAIQ | */ RCC_PLLSAICFGR_PLLSAIR)) |
		((SAIREF1_MUL << RCC_PLLSAICFGR_PLLSAIN_Pos) & RCC_PLLSAICFGR_PLLSAIN) |	// PLLI2SN bits = multiplier, freq=192..432 MHz, vale = 2..432
		((value << RCC_PLLSAICFGR_PLLSAIR_Pos) & RCC_PLLSAICFGR_PLLSAIR) |	// PLLI2SR bits - output divisor, 2..7
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

void hardware_set_dotclock(unsigned long dotfreq)
{
	auto uint_fast32_t
	calcdivround_saifreq(
		uint_fast32_t freq		/* требуемая частота на выходе делителя, в герцах. */
		)
	{
		//#error TODO: check freq at outputs vsync/hsync
		return (PLLSAI_FREQ + freq / 2) / freq;
	}

	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_saifreq(display_getdotclock(& vdmode0)), STM32F_LTDC_DIV_WIDTH, STM32F_LTDC_DIV_TAPS, & value, 0);
	ASSERT(value >= 2);
	PRINTF(PSTR("stm32f7xx_pllsai_initialize: value=%u, prei=%u\n"), value, prei);
}

#endif /* WITHUSESAIPLL */

#endif /* CPUSTYLE_STM32F7XX */

#if CPUSTYLE_STM32H7XX

// Программируем на 384 МГц
static void
stm32h7xx_pll_initialize(void)
{
#if 1
	RCC->APB4ENR |= RCC_APB4ENR_SYSCFGEN;     // включить тактирование альтернативных функций
	(void) RCC->APB4ENR;
	SYSCFG->CCCSR |= SYSCFG_CCCSR_EN;	// enable i/o compensation cell
	//while ((SYSCFG->CCCSR & SYSCFG_CCCSR_READY) == 0)
	//	;
#endif

	RCC->CR |= RCC_CR_HSION;		// 64 MHz включаю внутренний генератор
	while ((RCC->CR & RCC_CR_HSIRDY) == 0) //жду пока не заработает
		;
	RCC->CR &= ~ RCC_CR_PLLON;				// Выключил PLL

    // switch CPU clock to HSI before PLL programming
	RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_SW)) |
        RCC_CFGR_SW_HSI |
        0;
	while ((RCC->CFGR & RCC_CFGR_SWS) != (RCC_CFGR_SWS_0 * 0))
		;

	RCC->CR |= RCC_CR_HSI48ON;		// 48 MHz включаю внутренний генератор
	while ((RCC->CR & RCC_CR_HSI48RDY) == 0) //жду пока не заработает
		;

	// VOS0 (revidion V) - up to 480 MHz
	// VOS1 (revidion Y and other) - up to 400 MHz

#if defined PWR_CR3_SCUEN
	/* Configure voltage regulator */
	// Set the highest core voltage (Scale 1)
	PWR->CR3 = PWR_CR3_LDOEN | PWR_CR3_SCUEN;
	PWR->D3CR = (PWR->D3CR & ~ (PWR_D3CR_VOS)) |
		PWR_D3CR_VOS_value |
		//PWR_D3CR_VOS_0 * 3 |		// Rev Y: SCALE 0 1.218 mV VOS0, same as Rev V
		//PWR_D3CR_VOS_0 * 2 |		// Rev Y: SCALE 1 1.130 mV VOS1
		//PWR_D3CR_VOS_0 * 1 |		// Rev Y: SCALE 2 1.064 mV VOS2
		//PWR_D3CR_VOS_0 * 0 |		// Rev Y: SCALE 3 1.064 mV VOS3
		0;
	(void) PWR->D3CR;
	// Wait for LDO ready
	while ((PWR->D3CR & PWR_D3CR_VOSRDY) == 0)
		;
#else
	#warning LDO control should be implemented
#endif

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
	//	0xxx: sys_ck not divided (default after reset)
	//	1000: sys_ck divided by 2
	//	1001: sys_ck divided by 4
	//	1010: sys_ck divided by 8
	//	1011: sys_ck divided by 16
	//	1100: sys_ck divided by 64
	//	1101: sys_ck divided by 128
	//	1110: sys_ck divided by 256
	//	1111: sys_ck divided by 512
	RCC->D1CFGR = (RCC->D1CFGR & ~ (RCC_D1CFGR_D1CPRE)) |
		0 * RCC_D1CFGR_D1CPRE_0 |	// not divided
		0;
	(void) RCC->D1CFGR;

	// D1 domain AHB prescaler
	//	0xxx: rcc_hclk3 = sys_d1cpre_ck (default after reset)
	//	1000: rcc_hclk3 = sys_d1cpre_ck / 2
	//	1001: rcc_hclk3 = sys_d1cpre_ck / 4
	//	1010: rcc_hclk3 = sys_d1cpre_ck / 8
	//	1011: rcc_hclk3 = sys_d1cpre_ck / 16
	//	1100: rcc_hclk3 = sys_d1cpre_ck / 64
	//	1101: rcc_hclk3 = sys_d1cpre_ck / 128
	//	1110: rcc_hclk3 = sys_d1cpre_ck / 256
	//	1111: rcc_hclk3 = sys_d1cpre_ck / 512
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
		((REF1_DIV << RCC_PLLCKSELR_DIVM1_Pos) & RCC_PLLCKSELR_DIVM1) |	// Reference divisor - не требуется корректировань число
		0;
	//
	const uint32_t stm32h7xx_pllq = calcdivround2(PLL_FREQ, 48000000uL);	// Как было сделано при инициализации PLL
	RCC->PLL1DIVR = (RCC->PLL1DIVR & ~ (RCC_PLL1DIVR_N1 | RCC_PLL1DIVR_P1 | RCC_PLL1DIVR_Q1)) |
		(((REF1_MUL - 1) << RCC_PLL1DIVR_N1_Pos) & RCC_PLL1DIVR_N1) |
		(((2 - 1) << RCC_PLL1DIVR_P1_Pos) & RCC_PLL1DIVR_P1) |	// divede to 2, 3 is not allowed
		(((stm32h7xx_pllq - 1) << RCC_PLL1DIVR_Q1_Pos) & RCC_PLL1DIVR_Q1) |	// нужно для нормального переключения SPI clock USB clock
		0;
	RCC->PLLCFGR = (RCC->PLLCFGR & ~ (RCC_PLLCFGR_DIVP1EN | RCC_PLLCFGR_DIVQ1EN | RCC_PLLCFGR_PLL1VCOSEL | RCC_PLLCFGR_PLL1RGE)) |
		RCC_PLLCFGR_DIVP1EN |	// This bit can be written only when the PLL1 is disabled (PLL1ON = '0' and PLL1RDY = '0').
		RCC_PLLCFGR_DIVQ1EN |	// This bit can be written only when the PLL1 is disabled (PLL1ON = '0' and PLL1RDY = '0').
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


#if WITHSAICLOCKFROMI2S && ! WITHUSEPLL2
	#error WITHUSEPLL2 should be defined if WITHSAICLOCKFROMI2S used.
#endif /* LCDMODE_LTDC && ! WITHUSEPLL3 */

#if WITHUSEPLL2
	// PLL2 P output used for SAI1, SAI2, SAI3 clocking

	RCC->PLLCKSELR = (RCC->PLLCKSELR & ~ RCC_PLLCKSELR_DIVM2) |
		((REF2_DIV << RCC_PLLCKSELR_DIVM2_Pos) & RCC_PLLCKSELR_DIVM2) |	// Reference divisor - не требуется корректировань число
		0;
	//
	RCC->PLL2DIVR = (RCC->PLL2DIVR & ~ (RCC_PLL2DIVR_N2 | RCC_PLL2DIVR_P2)) |
		(((REF2_MUL - 1) << RCC_PLL2DIVR_N2_Pos) & RCC_PLL2DIVR_N2) |
		(((PLL2_DIVP - 1) << RCC_PLL2DIVR_P2_Pos) & RCC_PLL2DIVR_P2) |
		0;
	RCC->PLLCFGR = (RCC->PLLCFGR & ~ (RCC_PLLCFGR_DIVR2EN | RCC_PLLCFGR_PLL2RGE | RCC_PLLCFGR_PLL2VCOSEL)) |
		RCC_PLLCFGR_DIVP2EN |	// This bit can be written only when the PLL2 is disabled (PLL2ON = '0' and PLL3RDY = '0').
#if PLL2_FREQ >= 150000000uL && PLL2_FREQ <= 420000000uL
		1 * RCC_PLLCFGR_PLL2VCOSEL |	// 1: Medium VCO range: 150 to 420 MHz
#else
		0 * RCC_PLLCFGR_PLL2VCOSEL |	// 0: Wide VCO range: 192 to 836 MHz (default after reset)
#endif
		0 * RCC_PLLCFGR_PLL2RGE_0 |	// 00: The PLL2 input (ref3_ck) clock range frequency is between 1 and 2 MHz
		0;

	RCC->CR |= RCC_CR_PLL2ON;				// Включил PLL2


	while ((RCC->CR & RCC_CR_PLL2RDY) == 0)	// пока заработает PLL
		;


#endif /* WITHUSEPLL2 */

#if LCDMODE_LTDC && ! WITHUSEPLL3
	#error WITHUSEPLL3 should be defined if LCDMODE_LTDC used.
#endif /* LCDMODE_LTDC && ! WITHUSEPLL3 */

#if WITHUSEPLL3

	RCC->PLLCKSELR = (RCC->PLLCKSELR & ~ RCC_PLLCKSELR_DIVM3) |
		((REF3_DIV << RCC_PLLCKSELR_DIVM3_Pos) & RCC_PLLCKSELR_DIVM3) |	// Reference divisor - не требуется корректировань число
		0;
	//
	const uint32_t ltdc_divr = calcdivround2(PLL3_FREQ, display_getdotclock(& vdmode0));
	RCC->PLL3DIVR = (RCC->PLL3DIVR & ~ (RCC_PLL3DIVR_N3_Msk | RCC_PLL3DIVR_R3_Msk)) |
		(((REF3_MUL - 1) << RCC_PLL3DIVR_N3_Pos) & RCC_PLL3DIVR_N3_Msk) |
		(((ltdc_divr - 1) << RCC_PLL3DIVR_R3_Pos) & RCC_PLL3DIVR_R3_Msk) |	// нужно для нормального переключения SPI clock USB clock
		0;
	RCC->PLLCFGR = (RCC->PLLCFGR & ~ (RCC_PLLCFGR_DIVR3EN_Msk | RCC_PLLCFGR_PLL3RGE_Msk | RCC_PLLCFGR_PLL3VCOSEL_Msk)) |
		RCC_PLLCFGR_DIVR3EN |	// This bit can be written only when the PLL3 is disabled (PLL3ON = '0' and PLL3RDY = '0').
#if PLL3_FREQ >= 150000000uL && PLL3_FREQ <= 420000000uL
		1 * RCC_PLLCFGR_PLL3VCOSEL_Msk |	// 1: Medium VCO range: 150 to 420 MHz
#else
		0 * RCC_PLLCFGR_PLL3VCOSEL_Msk |	// 0: Wide VCO range: 192 to 836 MHz (default after reset)
#endif
		0 * RCC_PLLCFGR_PLL3RGE_0 |	// 00: The PLL3 input (ref3_ck) clock range frequency is between 1 and 2 MHz
		0;

	RCC->CR |= RCC_CR_PLL3ON_Msk;				// Включил PLL3


	while ((RCC->CR & RCC_CR_PLL3RDY_Msk) == 0)	// пока заработает PLL
		;

#endif /* WITHUSEPLL3 */

	const portholder_t flash_acr_latency = HARDWARE_FLASH_LATENCY; // Задержка для работы с памятью 5 WS for 168 MHz at 3.3 volt
	/* Блок настройки ФЛЭШ */
	/* Reserved bits must be kept cleared. */
	FLASH->ACR = (FLASH->ACR & ~ (FLASH_ACR_LATENCY_Msk | FLASH_ACR_WRHIGHFREQ_Msk)) |
		(FLASH_ACR_WRHIGHFREQ_0 * 3) |
		(flash_acr_latency << FLASH_ACR_LATENCY_Pos) |		//Задержка для работы с памятью
		0;

	while ((FLASH->ACR & FLASH_ACR_LATENCY) != flash_acr_latency)
		;

	RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_SW_Msk)) |
		RCC_CFGR_SW_PLL1 | // PLL as system clock
		0;

	while ((RCC->CFGR & RCC_CFGR_SWS) != (RCC_CFGR_SWS_0 * 3))
		;

	//while ((RCC->CR & (RCC_CR_D1CKRDY | RCC_CR_D2CKRDY)) != (RCC_CR_D1CKRDY | RCC_CR_D2CKRDY))
	//	;

	// RCC Domain 1 Kernel Clock Configuration Register
	// Set per_ck clock output
	RCC->D1CCIPR = (RCC->D1CCIPR & ~ (RCC_D1CCIPR_CKPERSEL_Msk)) |
		0 * RCC_D1CCIPR_CKPERSEL_0 |	// 00: hsi_ker_ck clock selected as per_ck clock (default after reset) - 64 MHz - used as PER_CK_FREQ
		0;

	// RCC Domain 1 Kernel Clock Configuration Register
	RCC->D1CCIPR = (RCC->D1CCIPR & ~ (RCC_D1CCIPR_SDMMCSEL_Msk)) |
#if WITHSDHCHW
		0 * RCC_D1CCIPR_SDMMCSEL_Msk |			// 0: pll1_q_ck clock is selected as kernel peripheral clock (default after reset)
#endif /* WITHSDHCHW */
		0;

	// RCC Domain 2 Kernel Clock Configuration Register
	RCC->D2CCIP1R = (RCC->D2CCIP1R & ~ (RCC_D2CCIP1R_SPI123SEL_Msk | RCC_D2CCIP1R_SPI45SEL_Msk)) |
		4 * RCC_D2CCIP1R_SPI123SEL_0 |		// per_ck
		3 * RCC_D2CCIP1R_SPI45SEL_0 |		// 011: hsi_ker_ck clock is selected as kernel clock
		0;
	// RCC Domain 2 Kernel Clock Configuration Register
	RCC->D2CCIP2R = (RCC->D2CCIP2R & ~ (
					RCC_D2CCIP2R_USART16SEL_Msk | RCC_D2CCIP2R_USART28SEL_Msk |
					RCC_D2CCIP2R_RNGSEL_Msk | RCC_D2CCIP2R_I2C123SEL_Msk)) |
		0 * RCC_D2CCIP2R_USART16SEL_0 |		// rcc_pclk2
		0 * RCC_D2CCIP2R_USART28SEL_0 |		// rcc_pclk1
		1 * RCC_D2CCIP2R_RNGSEL_0 |			// 01: pll1_q_ck clock is selected as kernel clock
		0 * RCC_D2CCIP2R_I2C123SEL_0 |		// rcc_pclk1
		0;

	// Выбор источника тактирования блока USB
	// RCC Domain 2 Kernel Clock Configuration Register
	// USBOTG 1 and 2 kernel clock source selection
	RCC->D2CCIP2R = (RCC->D2CCIP2R & ~ (RCC_D2CCIP2R_USBSEL_Msk)) |
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
		RCC->CFGR = (RCC->CFGR & ~ (RCC_CFGR_MCO1_Msk | RCC_CFGR_MCO1PRE_Msk)) |
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
static uint_fast32_t stm32f7xx_pllq_initialize(void)
{
	const uint32_t stm32h7xx_pllq = ((RCC->PLL1DIVR & RCC_PLL1DIVR_Q1_Msk) >> RCC_PLL1DIVR_Q1_Pos) + 1;
	return stm32h7xx_pllq;
}

void hardware_set_dotclock(unsigned long dotfreq)
{
	// при разрешенной PLLSAI модификация регистров невозможна
	(void) dotfreq;
}

#if WITHUSESAIPLL

static void stm32h7xx_pllsai_initialize(void)
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
	const uint_fast8_t prei = calcdivider(calcdivround_saifreq(display_getdotclock(& vdmode0)), STM32F_LTDC_DIV_WIDTH, STM32F_LTDC_DIV_TAPS, & value, 0);
	ASSERT(value >= 2);
	PRINTF(PSTR("stm32h7xx_pllsai_initialize: value=%u, prei=%u\n"), value, prei);

	// Настройка PLLSAI
	// Частота сравнения та же самая, что и в основной PLL
	// RCC_PLLSAICFGR_PLLSAIQ используется, если для SAI используется отдельная PLL - эта.
	RCC->PLLSAICFGR = (RCC->PLLSAICFGR & ~ (RCC_PLLSAICFGR_PLLSAIN_Msk | /*RCC_PLLSAICFGR_PLLSAIQ_Msk | */ RCC_PLLSAICFGR_PLLSAIR_Msk)) |
		((SAIREF1_MUL << RCC_PLLSAICFGR_PLLSAIN_Pos) & RCC_PLLSAICFGR_PLLSAIN_Msk) |	// PLLI2SN bits = multiplier, freq=192..432 MHz, vale = 2..432
		((value << RCC_PLLSAICFGR_PLLSAIR_Pos) & RCC_PLLSAICFGR_PLLSAIR_Msk) |	// PLLI2SR bits - output divisor, 2..7
		0;

	RCC->DCKCFGR = (RCC->DCKCFGR & ~ RCC_DCKCFGR_PLLSAIDIVR) |
		((prei << RCC_DCKCFGR_PLLSAIDIVR_Pos) & RCC_DCKCFGR_PLLSAIDIVR_Msk) |	// division factor for LCD_CLK. 2: PLLSAIDIVR = /8  3: PLLSAIDIVR = /16
		0;

	RCC->CR |= RCC_CR_PLLSAION_Msk;				// Включил PLL
	while ((RCC->CR & RCC_CR_PLLSAIRDY_Msk) == 0)	// пока заработает PLL
		;
}

#endif /* WITHUSESAIPLL */
#endif /* CPUSTYLE_STM32H7XX */


#if CPUSTYLE_STM32F1XX

static void
lowlevel_stm32f10x_pll_clock(void)
{
	#if WITHCPUXOSC
		// Внешний генератор
		// Enable HSI
		RCC->CR = (RCC->CR & ~ (RCC_CR_HSEON_Msk | RCC_CR_HSION_Msk | RCC_CR_HSEBYP_Msk)) |
			RCC_CR_HSEON_Msk |
			RCC_CR_HSEBYP_Msk |
			0;
		while (!(RCC->CR & RCC_CR_HSERDY_Msk))
			;
	#elif WITHCPUXTAL
		// внешний кварцевый резонатор
		// Enable HSI
		RCC->CR = (RCC->CR & ~ (RCC_CR_HSEON_Msk | RCC_CR_HSION_Msk | RCC_CR_HSEBYP_Msk)) | RCC_CR_HSEON_Msk;
		while (!(RCC->CR & RCC_CR_HSERDY_Msk))
			;
	#else /* WITHCPUXTAL */
		// внутренний генератор
		// Enable HSI
		RCC->CR = (RCC->CR & ~ (RCC_CR_HSEON_Msk | RCC_CR_HSION_Msk | RCC_CR_HSEBYP_Msk)) | RCC_CR_HSION_Msk;
		while (!(RCC->CR & RCC_CR_HSIRDY_Msk))
			;
	#endif /* WITHCPUXTAL */

	// Enable Prefetch Buffer
	FLASH->ACR |= FLASH_ACR_PRFTBE_Msk;

#if (CPU_FREQ < 24000000ul)
	// Flash 0 wait state
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY_Msk) | FLASH_ACR_LATENCY_0;
#elif (CPU_FREQ <= 48000000ul)
	// Flash 1 wait state
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY_Msk) | FLASH_ACR_LATENCY_1;
#else
 	// Flash 2 wait state (if freq in 24..48 MHz range - 1WS.)
	#if CPUSTYLE_STM32F1XX
		FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY_Msk) | FLASH_ACR_LATENCY_2;
	#else
		FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY_Msk) | FLASH_ACR_LATENCY_1;
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
stm32f30x_pll_clock(void)
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
	program_mckr_pres(PMC_MCKR_PRES_CLK_2);	// with /2 divisor
	program_mckr_css(PMC_MCKR_CSS_PLLA_CLK);
}

// If a new value for CSS field corresponds to Main Clock
static void program_mckr_switchtomain(void)
{
	program_mckr_css(PMC_MCKR_CSS_MAIN_CLK);
#ifdef PMC_MCKR_PRES_CLK_1
	program_mckr_pres(PMC_MCKR_PRES_CLK_1);	// w/o divisor
#else
	program_mckr_pres(PMC_MCKR_PRES_CLK);	// w/o divisor
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
	program_mckr_pres(PMC_MCKR_PRES_CLK_1);	// w/o divisor
#else
	program_mckr_pres(PMC_MCKR_PRES_CLK);	// w/o divisor
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
static void sam4s_init_clock_12_RC12(void)
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
stm32f0xx_hsi_clock(void)
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
stm32f0xx_pll_clock(void)
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
			(RCC_CFGR_HPRE_0 * 0) |		// AHB clock division factor. 0x00: system clock divided by 1
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
	const uint_fast8_t prei = calcdivider(calcdivround2(BOARD_TIM21_FREQ, 1000), STM32F_TIM3_TIMER_WIDTH, STM32F_TIM3_TIMER_TAPS, & value, 1);

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
	// Enable HSI
	RCC->CR |= RCC_CR_HSION;
	while (!(RCC->CR & RCC_CR_HSIRDY))
		;
	// Select HSI as system clock source
	RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
    // Wait till HSI is used as system clock source
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI)
		;

	#if WITHCPUXTAL
		// внешний кварцевый резонатор
		// Enable HSE
		RCC->CR |= RCC_CR_HSEON;
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
			(0 * RCC_CFGR_HPRE_0) |		// AHB clock division factor. 0x00: system clock divided by 1
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

#if CPUSTYLE_XC7Z /* || CPUSTYLE_XCZU */

static void xc7z_arm_pll_initialize(void)
{
	const uint_fast32_t arm_pll_mul = ARM_PLL_MUL;	// ARM_PLL_CFG.PLL_FDIV
	const uint_fast32_t arm_pll_div = ARM_PLL_DIV;	// ARM_CLK_CTRL.DIVISOR

	SCLR->SLCR_UNLOCK = 0x0000DF0DU;
	//	EMIT_MASKWRITE(0XF8000110, 0x003FFFF0U ,0x000FA220U),	// ARM_PLL_CFG
	SCLR->ARM_PLL_CFG = (SCLR->ARM_PLL_CFG & ~ (0x003FFFF0U)) |
			0x000FA220U |
			0;
	//	EMIT_MASKWRITE(0XF8000100, 0x0007F000U ,0x00028000U),	// ARM_PLL_CTRL
	SCLR->ARM_PLL_CTRL = (SCLR->ARM_PLL_CTRL & ~ (0x0007F000U)) |
			(arm_pll_mul << 12) |	// PLL_FDIV (multiplier)
			0;

	//	EMIT_MASKWRITE(0XF8000100, 0x00000010U ,0x00000010U),	// ARM_PLL_CTRL
	SCLR->ARM_PLL_CTRL |= 0x0010uL;	// PLL_BYPASS_FORCE

	//	EMIT_MASKWRITE(0XF8000100, 0x00000001U ,0x00000001U),	// ARM_PLL_CTRL
	SCLR->ARM_PLL_CTRL |= 0x0001uL;	// PLL_RESET
	//	EMIT_MASKWRITE(0XF8000100, 0x00000001U ,0x00000000U),	// ARM_PLL_CTRL
	SCLR->ARM_PLL_CTRL &= ~ 0x0001uL;	// PLL_RESET

	//	EMIT_MASKPOLL(0XF800010C, 0x00000001U),					// PLL_STATUS
	while ((SCLR->PLL_STATUS & (0x01uL << 0)) == 0)	// ARM_PLL_LOCK
		;
	//	EMIT_MASKWRITE(0XF8000100, 0x00000010U ,0x00000000U),	// ARM_PLL_CTRL
	SCLR->ARM_PLL_CTRL &= ~ 0x0010uL;	// PLL_BYPASS_FORCE

	//	////EMIT_MASKWRITE(0XF8000120, 0x1F003F30U ,0x1F000200U),	// ARM_CLK_CTRL
	SCLR->ARM_CLK_CTRL = (SCLR->ARM_CLK_CTRL & ~ (0x1F003F30U)) |
			(0x01uL << 28) |	// CPU_PERI_CLKACT
			(0x01uL << 27) |	// CPU_1XCLKACT
			(0x01uL << 26) |	// CPU_2XCLKACT
			(0x01uL << 25) |	// CPU_3OR2XCLKACT
			(0x01uL << 24) |	// CPU_6OR4XCLKACT
			(arm_pll_div << 8) |	// DIVISOR - Frequency divisor for the CPU clock source.
			(0x00uL << 4) |	// SRCSEL: 0x: ARM PLL
			0;
}

static void xc7z_ddr_pll_initialize(void)
{
	SCLR->SLCR_UNLOCK = 0x0000DF0DU;
	//	EMIT_MASKWRITE(0XF8000114, 0x003FFFF0U ,0x0012C220U),	// DDR_PLL_CFG
	SCLR->DDR_PLL_CFG = (SCLR->DDR_PLL_CFG & ~ (0x003FFFF0U)) |
			0x0012C220U |
			0;
	//	EMIT_MASKWRITE(0XF8000104, 0x0007F000U ,0x00020000U),	// DDR_PLL_CTRL
	SCLR->DDR_PLL_CTRL = (SCLR->DDR_PLL_CTRL & ~ (0x0007F000U)) |
			((uint_fast32_t) DDR_PLL_MUL << 12) |
			0;
	//	EMIT_MASKWRITE(0XF8000104, 0x00000010U ,0x00000010U),	// DDR_PLL_CTRL
	SCLR->DDR_PLL_CTRL = (SCLR->DDR_PLL_CTRL & ~ (0x00000010U)) |
			0x00000010U |
			0;
	//	EMIT_MASKWRITE(0XF8000104, 0x00000001U ,0x00000001U),	// DDR_PLL_CTRL
	SCLR->DDR_PLL_CTRL = (SCLR->DDR_PLL_CTRL & ~ (0x00000001U)) |
			0x00000001U |
			0;
	//	EMIT_MASKWRITE(0XF8000104, 0x00000001U ,0x00000000U),	// DDR_PLL_CTRL
	SCLR->DDR_PLL_CTRL = (SCLR->DDR_PLL_CTRL & ~ (0x00000001U)) |
			0x00000000U |
			0;

	//	EMIT_MASKPOLL(0XF800010C, 0x00000002U),					// PLL_STATUS
	while ((SCLR->PLL_STATUS & (0x01uL << 1)) == 0)	// DDR_PLL_LOCK
		;
	//	EMIT_MASKWRITE(0XF8000104, 0x00000010U ,0x00000000U),	// DDR_PLL_CTRL
	SCLR->DDR_PLL_CTRL = (SCLR->DDR_PLL_CTRL & ~ (0x00000010U)) |
			0x00000000U |
			0;
	//	EMIT_MASKWRITE(0XF8000124, 0xFFF00003U ,0x0C200003U),	// DDR_CLK_CTRL
	SCLR->DDR_CLK_CTRL = (SCLR->DDR_CLK_CTRL & ~ (0xFFF00003U)) |
			((uint_fast32_t) DDR_2XCLK_DIVISOR << 26) |	// DDR_2XCLK_DIVISOR
			((uint_fast32_t) DDR_3XCLK_DIVISOR << 20) |	// DDR_3XCLK_DIVISOR (only even)
			(0x01uL << 1) |	// DDR_2XCLKACT
			(0x01uL << 0) | // DDR_3XCLKACT
			0;
}

static void xc7z_io_pll_initialize(void)
{
	SCLR->SLCR_UNLOCK = 0x0000DF0DU;
	//	EMIT_MASKWRITE(0XF8000118, 0x003FFFF0U ,0x000FA240U),	// IO_PLL_CFG
	SCLR->IO_PLL_CFG = (SCLR->IO_PLL_CFG & ~ (0x003FFFF0U)) |
			0x000FA240uL |
			0;
	//	EMIT_MASKWRITE(0XF8000108, 0x0007F000U ,0x00030000U),	// IO_PLL_CTRL
	SCLR->IO_PLL_CTRL = (SCLR->IO_PLL_CTRL & ~ (0x0007F000U)) |	// PLL_FDIV
			((uint_fast32_t) IO_PLL_MUL << 12) |
			0;
	//	EMIT_MASKWRITE(0XF8000108, 0x00000010U ,0x00000010U),	// IO_PLL_CTRL
	SCLR->IO_PLL_CTRL = (SCLR->IO_PLL_CTRL & ~ (0x00000010U)) |
			0x00000010U |
			0;
	//	EMIT_MASKWRITE(0XF8000108, 0x00000001U ,0x00000001U),	// IO_PLL_CTRL
	SCLR->IO_PLL_CTRL = (SCLR->IO_PLL_CTRL & ~ (0x00000001U)) |
			0x00000001U |
			0;
	//	EMIT_MASKWRITE(0XF8000108, 0x00000001U ,0x00000000U),	// IO_PLL_CTRL
	SCLR->IO_PLL_CTRL = (SCLR->IO_PLL_CTRL & ~ (0x00000001U)) |
			0x00000000U |
			0;

	//	EMIT_MASKPOLL(0XF800010C, 0x00000004U),					// PLL_STATUS
	while ((SCLR->PLL_STATUS & (0x01uL << 2)) == 0)	// IO_PLL_LOCK
		;
	//	EMIT_MASKWRITE(0XF8000108, 0x00000010U ,0x00000000U),	// IO_PLL_CTRL
	SCLR->IO_PLL_CTRL = (SCLR->IO_PLL_CTRL & ~ (0x00000010U)) |
			0x00000000U |
			0;
}

static unsigned long xc7z_get_pllsreference_freq(void)
{
	return WITHCPUXOSC;
}

static uint_fast64_t xc7z_get_arm_pll_freq(void)
{
	const uint_fast32_t arm_pll_mul = (SCLR->ARM_PLL_CTRL >> 12) & 0x07FF;	// PLL_FDIV

	return (uint_fast64_t) xc7z_get_pllsreference_freq() * arm_pll_mul;
}

static uint_fast64_t xc7z_get_ddr_pll_freq(void)
{
	const uint_fast32_t ddr_pll_mul = (SCLR->DDR_PLL_CTRL >> 12) & 0x07FF;	// PLL_FDIV

	return (uint_fast64_t) xc7z_get_pllsreference_freq() * ddr_pll_mul;
}

uint_fast64_t xc7z_get_io_pll_freq(void)
{
	const uint_fast32_t io_pll_mul = (SCLR->IO_PLL_CTRL >> 12) & 0x07FF;	// PLL_FDIV

	return (uint_fast64_t) xc7z_get_pllsreference_freq() * io_pll_mul;
}

unsigned long  xc7z_get_arm_freq(void)
{
	const uint_fast32_t divisor = (SCLR->ARM_CLK_CTRL >> 8) & 0x003F;	// DIVISOR

	return xc7z_get_arm_pll_freq() / divisor;
}

unsigned long  xc7z_get_ddr_x2clk_freq(void)
{
	const uint_fast32_t divisor = (SCLR->DDR_CLK_CTRL >> 26) & 0x003F;	// [31:26] DDR_2XCLK_DIVISOR

	return xc7z_get_ddr_pll_freq() / divisor;
}

unsigned long  xc7z_get_ddr_x3clk_freq(void)
{
	const uint_fast32_t divisor = (SCLR->DDR_CLK_CTRL >> 20) & 0x003F;	// [25:20] DDR_2XCLK_DIVISOR

	return xc7z_get_ddr_pll_freq() / divisor;
}

unsigned long  xc7z_get_uart_freq(void)
{
	const uint_fast32_t divisor = (SCLR->UART_CLK_CTRL >> 8) & 0x003F;	// DIVISOR
	switch ((SCLR->UART_CLK_CTRL & 0x30) >> 4)	// SRCSEL
	{
	default:
	case 0x00:
		// 0x: Source for generated clock is IO PLL.
		return xc7z_get_io_pll_freq() / divisor;
	case 0x01:
		// 10: Source for generated clock is ARM PLL.
		return xc7z_get_arm_pll_freq() / divisor;
	case 0x03:
		// 11: Source for generated clock is DDR PLL
		return xc7z_get_ddr_pll_freq() / divisor;
	}
}

unsigned long xc7z_get_sdio_freq(void)
{
	const uint_fast32_t divisor = (SCLR->SDIO_CLK_CTRL >> 8) & 0x003F;	// DIVISOR
	switch ((SCLR->SDIO_CLK_CTRL & 0x30) >> 4)	// SRCSEL
	{
	default:
	case 0x00:
		// 0x: Source for generated clock is IO PLL.
		return xc7z_get_io_pll_freq() / divisor;
	case 0x01:
		// 10: Source for generated clock is ARM PLL.
		return xc7z_get_arm_pll_freq() / divisor;
	case 0x03:
		// 11: Source for generated clock is DDR PLL
		return xc7z_get_ddr_pll_freq() / divisor;
	}
}

unsigned long  xc7z_get_spi_freq(void)
{
	const uint_fast32_t divisor = (SCLR->SPI_CLK_CTRL >> 8) & 0x003F;	// DIVISOR
	switch ((SCLR->SPI_CLK_CTRL & 0x30) >> 4)	// SRCSEL
	{
	default:
	case 0x00:
		// 0x: Source for generated clock is IO PLL.
		return xc7z_get_io_pll_freq() / divisor;
	case 0x01:
		// 10: Source for generated clock is ARM PLL.
		return xc7z_get_arm_pll_freq() / divisor;
	case 0x03:
		// 11: Source for generated clock is DDR PLL
		return xc7z_get_ddr_pll_freq() / divisor;
	}
}

unsigned long  xc7z_get_qspi_freq(void)
{
	const uint_fast32_t divisor = (SCLR->LQSPI_CLK_CTRL >> 8) & 0x003F;	// DIVISOR
	switch ((SCLR->LQSPI_CLK_CTRL & 0x30) >> 4)	// SRCSEL
	{
	default:
	case 0x00:
		// 0x: Source for generated clock is IO PLL.
		return xc7z_get_io_pll_freq() / divisor;
	case 0x01:
		// 10: Source for generated clock is ARM PLL.
		return xc7z_get_arm_pll_freq() / divisor;
	case 0x03:
		// 11: Source for generated clock is DDR PLL
		return xc7z_get_ddr_pll_freq() / divisor;
	}
}

void hardware_set_dotclock(unsigned long dotfreq)
{
	unsigned long f1 = (unsigned long) ( xc7z_get_io_pll_freq() / 1000);
	dotfreq /= 1000;
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(f1, dotfreq), XC7Z_FPGAx_CLK_WIDTH, XC7Z_FPGAx_CLK_TAPS, & value, 0);

	//PRINTF("xc7z_setltdcfreq: FPGA0_CLK_CTRL.DIVISOR0=%u, DIVISOR1=%u, iopll=%lu, dotclk=%lu\n", 1u << prei, value, (unsigned long) xc7z_get_io_pll_freq(), ((unsigned long) xc7z_get_io_pll_freq() >> prei) / value);

#if 1
	// PL Clock 0 Output control
	SCLR->FPGA0_CLK_CTRL = (SCLR->FPGA0_CLK_CTRL & ~ (0x03F03F30U)) |
			(((uint_fast32_t) 1 << prei) << 8) | // 13:8 DIVISOR0 - First cascade divider.
			((uint_fast32_t) value << 20) | // 25:20 DIVISOR1 - Second cascade divide
			(0x00uL << 4) |	// SRCSEL - 0x: IO PLL
			0;
#endif
}

#endif /* CPUSTYLE_XC7Z || CPUSTYLE_XCZU */

uint32_t SystemCoreClock;     /*!< System Clock Frequency (Core Clock)  */

// PLL initialize
void FLASHMEMINITFUNC
sysinit_pll_initialize(void)
{
#ifdef USE_HAL_DRIVER
	HAL_Init();
#endif /* USE_HAL_DRIVER */
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

	cpu_stm32f1xx_setmapr(0);	/* переключить отладочный интерфейс в SWD */
	// Разрешить работу компаратора напряжения питания (нужно для разряда емкостей преобразователя питания дисплея)
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;     //включить тактирование power management
	(void) RCC->APB1ENR;
	PWR->CR = (PWR->CR & ~ PWR_CR_PLS) | PWR_CR_PLS_2V8 | PWR_CR_PVDE;

#elif CPUSTYLE_STM32F4XX

	stm32f4xx_pll_initialize();
	stm32f4xx_MCOx_test();
	stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц

	RCC->APB1ENR |= RCC_APB1ENR_PWREN;	// включить тактирование power management
	(void) RCC->APB1ENR;

	#if WITHUSESAIPLL
		stm32f4xx_pllsai_initialize();
	#endif /* WITHUSESAIPLL */

#elif CPUSTYLE_STM32H7XX

	stm32h7xx_pll_initialize();
	stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
	//lowlevel_stm32h7xx_mpu_initialize();

	/* AXI SRAM Slave */
	//AXI_TARG7_FN_MOD |= READ_ISS_OVERRIDE;
	  /* dual core CM7 or single core line */
	  if((DBGMCU->IDCODE & 0xFFFF0000U) < 0x20000000U)
	  {
		/* if stm32h7 revY*/
		/* Change  the switch matrix read issuing capability to 1 for the AXI SRAM target (Target 7) */
		*((__IO uint32_t*) 0x51008108) = 0x1; //Change  the switch matrix read issuing capability to 1 (Errata BUG fix)
	  }
	/* Change  the switch matrix read issuing capability to 1 for the AXI SRAM target (Target 7) */
	//*((__IO uint32_t*)0x51008108) = 0x000000001;

	RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM1EN;
	(void) RCC->AHB2ENR;
	RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM2EN;
	(void) RCC->AHB2ENR;
	RCC->AHB2ENR |= RCC_AHB2ENR_D2SRAM3EN;
	(void) RCC->AHB2ENR;

//	RCC->AHB4ENR |= RCC_AHB4ENR_D3SRAM1EN;
//	(void) RCC->AHB4ENR;

	//RCC->APB1ENR |= RCC_APB1ENR_PWREN;	// включить тактирование power management
	//__DSB();

	#if WITHUSESAIPLL
		stm32h7xx_pllsai_initialize();
	#endif /* WITHUSESAIPLL */

#elif CPUSTYLE_STM32F7XX

	stm32f7xx_pll_initialize();
	stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
#if WITHUSESAIPLL
	stm32f7xx_pllsai_initialize();
#endif /* WITHUSESAIPLL */

	RCC->APB1ENR |= RCC_APB1ENR_PWREN;	// включить тактирование power management
	(void) RCC->APB1ENR;

#elif CPUSTYLE_STM32F30X

	stm32f30x_pll_clock();
	stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц

	// Разрешить работу компаратора напряжения питания (нужно для разряда емкостей преобразователя питания дисплея)
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;     // включить тактирование power management
	(void) RCC->APB1ENR;
	PWR->CR = (PWR->CR & ~ PWR_CR_PLS) | PWR_CR_PLS_LEV3 | PWR_CR_PVDE;

#elif CPUSTYLE_STM32F0XX

	stm32f0xx_pll_clock();
	//stm32f0xx_hsi_clock();

	// Разрешить работу компаратора напряжения питания (нужно для разряда емкостей преобразователя питания дисплея)
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;     // включить тактирование power management
	(void) RCC->APB1ENR;
	PWR->CR = (PWR->CR & ~ PWR_CR_PLS) | PWR_CR_PLS_LEV3 | PWR_CR_PVDE;

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

	// Разрешить работу компаратора напряжения питания (нужно для разряда емкостей преобразователя питания дисплея)
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;     // включить тактирование power management
	(void) RCC->APB1ENR;
	PWR->CR = (PWR->CR & ~ PWR_CR_PLS) | PWR_CR_PLS_LEV3 | PWR_CR_PVDE;

#elif CPUSTYLE_ATSAM3S

	// Disable Watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;
	sam3s_init_clock_12_RC12();	// программирует на работу от 12 МГц RC - для ускорения работы.
	// только из SRAM
	arm_cpu_atsam3s_pll_initialize();

#elif CPUSTYLE_ATSAM4S

	// Disable Watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;
	sam4s_init_clock_12_RC12();	// программирует на работу от 12 МГц RC - для ускорения работы.
	// только из SRAM
	arm_cpu_atsam4s_pll_initialize();

#elif CPUSTYLE_AT91SAM7S

	// Disable Watchdog
	AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;

	// Enable NRST input. Требуется для удобства при отладке.
	AT91C_BASE_RSTC->RSTC_RMR = AT91C_RSTC_URSTEN | (AT91C_RSTC_KEY & (0xA5UL << 24));

	// init clock sources and memory timings
	// use one of alternatives
	//

	#if CPU_FREQ == 48000000UL
		sam7s_pll_init_clock_xtal(8, 1);
	#elif CPU_FREQ == ((18432000UL * 73) / 14 / 2)
		sam7s_pll_init_clock_xtal(73, 14);
	#elif CPU_FREQ == 12000000UL
		sam7s_pll_init_clock_from_xtal();
	#else
		#error Unsupported CPU_FREQ value
	#endif

	usb_disable();

#elif CPUSTYLE_AT91SAM9XE

	// Disable Watchdog
	AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;

	// Enable NRST input. Требуется для удобства при отладке.
	AT91C_BASE_RSTC->RSTC_RMR = AT91C_RSTC_URSTEN | (AT91C_RSTC_KEY & (0xA5UL << 24));

	at91sam9x_pll_initialize(96, 9);
	//at91sam9x_pll_48x4_initialize();

	//cp15_enable_i_cache();
	__set_SCTLR(__get_SCTLR() | SCTLR_I_Msk);

	usb_disable();

#elif CPUSTYLE_R7S721

#if WITHISBOOTLOADER
	r7s721_pll_initialize();
#endif /* WITHISBOOTLOADER */
	// Программа исполняется из SERIAL FLASH - переключать режимы пока нельзя.
	//while ((SPIBSC0.CMNSR & (1u << 0)) == 0)	// TEND bit
	//	;
	//SPIBSC0.SSLDR = 0x00;
	//SPIBSC0.SPBCR = 0x200;
	//SPIBSC0.DRCR = 0x0100;

	/* Установить скорость обмена с SERIAL FLASH повыше */
	if ((CPG.STBCR9 & CPG_STBCR9_BIT_MSTP93) == 0)
	{
		SPIBSC0.SPBCR = (SPIBSC0.SPBCR & ~ (SPIBSC_SPBCR_BRDV | SPIBSC_SPBCR_SPBR)) |
			(0 << SPIBSC_SPBCR_BRDV_SHIFT) |	// 0..3
			(2 << SPIBSC_SPBCR_SPBR_SHIFT) |	// 0..255
			0;
	}

#if WITHISBOOTLOADER
	{
		/* ----  Writing to On-Chip Data-Retention RAM is enabled. ---- */
		// Нельзя отключить - т.к. r7s721_ttb_map работает со страницами по 1 мегабайту
		// Нельзя отключить - botloader не может загрузить программу на выполнение по DFU.
		// Странно, почему? Судя по описанию, области перекрываются...
		CPG.SYSCR3 = (CPG_SYSCR3_RRAMWE3 | CPG_SYSCR3_RRAMWE2 | CPG_SYSCR3_RRAMWE1 | CPG_SYSCR3_RRAMWE0);
		(void) CPG.SYSCR3;
	}
#endif /* WITHISBOOTLOADER */
	/* далее будет выполняться копирование data и инициализация bss - для нормальной работы RESET требуется без DATA CACHE */

#elif CPUSTYLE_STM32MP1

	#if WITHISBOOTLOADER
		// PLL только в bootloader.
		// посеольку программа выполняется из DDR RAM, перерпрограммировать PLL нельзя.
		stm32mp1_pll_initialize();
	#endif /* WITHISBOOTLOADER */

	stm32mp1_usb_clocks_initialize();
	stm32mp1_audio_clocks_initialize();

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU
	#if WITHISBOOTLOADER

		SCLR->SLCR_UNLOCK = 0x0000DF0DU;
		SCLR->FPGA_RST_CTRL	= 0xF;	// Assert FPGA top-level output resets.
		SCLR->LVL_SHFTR_EN 	= 0;	// Disable the level shifters.

		SCLR->APER_CLK_CTRL = 0;	// All AMBA Clock control disable

		// PLL только в bootloader.
		// посеольку программа выполняется из DDR RAM, перерпрограммировать PLL нельзя.

		xc7z_arm_pll_initialize();
		xc7z_ddr_pll_initialize();
		xc7z_io_pll_initialize();


		SCLR->SLCR_UNLOCK = 0x0000DF0DU;
		XDCFG->CTRL &= ~ (1uL << 29);	// PCFG_POR_CNT_4K

		////EMIT_MASKWRITE(0XF8000170, 0x03F03F30U ,0x00400800U),	// FPGA0_CLK_CTRL PL Clock 0 Output control

		// PL Clock 0 Output control
		SCLR->FPGA0_CLK_CTRL = (SCLR->FPGA0_CLK_CTRL & ~ (0x03F03F30U)) |
				((uint_fast32_t) SCLR_FPGA0_CLK_CTRL_DIVISOR0 << 8) | // 13:8DIVISOR0 - First cascade divider.
				((uint_fast32_t) SCLR_FPGA0_CLK_CTRL_DIVISOR1 << 20) | // 25:20 DIVISOR1 - Second cascade divide
				(0x00uL << 4) |	// SRCSEL - 0x: IO PLL
				0;

		// PL Clock 1 Output control
		SCLR->FPGA1_CLK_CTRL = (SCLR->FPGA1_CLK_CTRL & ~ (0x03F03F30U)) |
				((uint_fast32_t) SCLR_FPGA1_CLK_CTRL_DIVISOR0 << 8) | // 13:8DIVISOR0 - First cascade divider.
				((uint_fast32_t) SCLR_FPGA1_CLK_CTRL_DIVISOR1 << 20) | // 25:20 DIVISOR1 - Second cascade divide
				(0x00uL << 4) |	// SRCSEL - 0x: IO PLL
				0;

		// PL Clock 2 Output control
		SCLR->FPGA2_CLK_CTRL = (SCLR->FPGA2_CLK_CTRL & ~ (0x03F03F30U)) |
				((uint_fast32_t) SCLR_FPGA2_CLK_CTRL_DIVISOR0 << 8) | // 13:8DIVISOR0 - First cascade divider.
				((uint_fast32_t) SCLR_FPGA2_CLK_CTRL_DIVISOR1 << 20) | // 25:20 DIVISOR1 - Second cascade divide
				(0x00uL << 4) |	// SRCSEL - 0x: IO PLL
				0;

		// PL Clock 3 Output control
		SCLR->FPGA3_CLK_CTRL = (SCLR->FPGA3_CLK_CTRL & ~ (0x03F03F30U)) |
				((uint_fast32_t) SCLR_FPGA3_CLK_CTRL_DIVISOR0 << 8) | // 13:8DIVISOR0 - First cascade divider.
				((uint_fast32_t) SCLR_FPGA3_CLK_CTRL_DIVISOR1 << 20) | // 25:20 DIVISOR1 - Second cascade divide
				(0x00uL << 4) |	// SRCSEL - 0x: IO PLL
				0;

		////EMIT_MASKWRITE(0XF80001C4, 0x00000001U ,0x00000001U),	// CLK_621_TRUE CPU Clock Ratio Mode select
		SCLR->CLK_621_TRUE = (SCLR->CLK_621_TRUE & ~ (0x00000001U)) |
				0x00000001U |
				0;

		SCLR->APER_CLK_CTRL |= (0x01uL << 22);	/* APER_CLK_CTRL.GPIO_CPU_1XCLKACT */

	#endif /* WITHISBOOTLOADER */

#endif
	SystemCoreClock = CPU_FREQ;
}


#if WITHDCDCFREQCTL
	//static uint_fast16_t dcdcrefdiv = 62;	/* делится частота внутреннего генератора 48 МГц */
	/*
		получение делителя частоты для синхронизации DC-DC конверторов
		для исключения попадания в полосу обзора панорамы гармоник этой частоты.
	*/
	uint_fast32_t
	hardware_dcdc_calcdivider(
		uint_fast32_t freq
		)
	{
		struct FREQ
		{
			uint32_t dcdcdiv;
			uint32_t fmin;
			uint32_t fmax;
		};

#if CPUSTYLE_R7S721

		return calcdivround_p0clock(760000uL * 2);	// на выходе формирователя делитель на 2 - требуемую частоту умножаем на два

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	#if CPUSTYLE_STM32H7XX
		// пока для проверки работоспособности. Таблицу надо расчитать.
		static const FLASHMEM struct FREQ freqs [] = {
		  { 63, 26900000uL,  UINT32_MAX },
		  { 63, 6900000uL,  26900000uL },
		  { 62, 0,		6900000uL },
		};
	#elif CPUSTYLE_STM32MP1
		//const unsigned long ifreq = stm32mp1_get_timg2_freq();	// TIM17 это timg2
		//PRINTF("hardware_dcdc_calcdivider: ifreq=%lu\n", ifreq);
		// пока для проверки работоспособности. Таблицу надо расчитать.
		// сейчас делит 32 MHz
		static const FLASHMEM struct FREQ freqs [] = {
		  { 29, 26900000uL,  UINT32_MAX },
		  { 29, 6900000uL,  26900000uL },
		  { 29, 0,		6900000uL },
		};
	#endif

		uint_fast8_t high = (sizeof freqs / sizeof freqs [0]);
		uint_fast8_t low = 0;
		uint_fast8_t middle = 0;	// результат поиска

		// Двоичный поиск
		while (low < high)
		{
			middle = (high - low) / 2 + low;
			if (freq < freqs [middle].fmin)	// нижняя граница не включается - для обеспечения формального попадания частоты DCO в рабочий диапазон
				low = middle + 1;
			else if (freq >= freqs [middle].fmax)
				high = middle;		// переходим к поиску в меньших индексах
			else
				goto found;
		}

	found:
		return freqs [middle].dcdcdiv;
#endif /* CPUSTYLE_STM32H7XX, CPUSTYLE_R7S721 */
	}

#if CPUSTYLE_STM32H7XX

	void hardware_dcdcfreq_tim16_ch1_initialize(void)
	{
		/* TIM16_CH1 */
		RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;   //подаем тактирование на TIM16
		(void) RCC->APB2ENR;

		TIM16->CCMR1 =
			3 * TIM_CCMR1_OC1M_0 |	// для кодов более 7 использовать TIM_CCMR1_OC1M_3. Output Compare 1 Mode = 3
			0;
		TIM16->CCER = TIM_CCER_CC1E;
		//TIM16->DIER = TIM_DIER_UIE;        	 // разрешить событие от таймера
		TIM16->BDTR = TIM_BDTR_MOE;
	}

	void hardware_dcdcfreq_tim16_ch1_setdiv(uint_fast32_t v)
	{
		/* TIM16_CH1 */
		unsigned value;	/* делитель */
		const uint_fast8_t prei = calcdivider(v, STM32F_TIM4_TIMER_WIDTH, STM32F_TIM4_TIMER_TAPS, & value, 1);
		TIM16->PSC = ((1UL << prei) - 1) & TIM_PSC_PSC;

		TIM16->ARR = value;
		//TIM16->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости - если счётчик успевал превысить значение ARR - считал до конца */

		//TIM16->CCR1 = (value / 2) & TIM_CCR1_CCR1;	// TIM16_CH1 - wave output
		//TIM16->ARR = value;
		TIM16->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE;	/* разрешить перезагрузку и включить таймер */
	}

#elif CPUSTYLE_STM32MP1

	void hardware_dcdcfreq_tim17_ch1_initialize(void)
	{
		const unsigned long ifreq = stm32mp1_get_timg2_freq();	// TIM17 это timg2
		PRINTF("hardware_dcdcfreq_tim17_ch1_initialize: ifreq=%lu\n", ifreq);
		/* TIM17_CH1 */
		RCC->MP_APB2ENSETR = RCC_MP_APB2ENSETR_TIM17EN;   //подаем тактирование на TIM17
		(void) RCC->MP_APB2ENSETR;
		RCC->MP_APB2LPENSETR = RCC_MP_APB2LPENSETR_TIM17LPEN;   //подаем тактирование на TIM17
		(void) RCC->MP_APB2LPENSETR;

		TIM17->CCMR1 =
			3 * TIM_CCMR1_OC1M_0 |	// для кодов более 7 использовать TIM_CCMR1_OC1M_3. Output Compare 1 Mode = 3
			0;
		TIM17->CCER = TIM_CCER_CC1E;
		//TIM17->DIER = TIM_DIER_UIE;        	 // разрешить событие от таймера
		TIM17->BDTR = TIM_BDTR_MOE;
	}

	void hardware_dcdcfreq_tim17_ch1_setdiv(uint_fast32_t v)
	{
		/* TIM17_CH1 */
		unsigned value;	/* делитель */
		const uint_fast8_t prei = calcdivider(v, STM32F_TIM4_TIMER_WIDTH, STM32F_TIM4_TIMER_TAPS, & value, 1);
		TIM17->PSC = ((1UL << prei) - 1) & TIM_PSC_PSC;

		TIM17->ARR = value;
		//TIM17->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости - если счётчик успевал превысить значение ARR - считал до конца */

		//TIM17->CCR1 = (value / 2) & TIM_CCR1_CCR1;	// TIM16_CH1 - wave output
		//TIM17->ARR = value;
		TIM17->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE;	/* разрешить перезагрузку и включить таймер */
	}

#elif CPUSTYLE_R7S721

	void hardware_dcdcfreq_tioc0a_mtu0_initialize(void)
	{
		/* P2_8 TIOC0A (MTU0 output) */
		/* ---- Supply clock to the video display controller 5  ---- */
		CPG.STBCR3 &= ~ CPG_STBCR3_MSTP33;	// Module Stop 33 0: The multi-function timer pulse unit 2 runs.
		(void) CPG.STBCR3;					/* Dummy read */

		/* Enable access to write-protected MTU2 registers */
		MTU2.TRWER = 1u;	// Channels 3 & 4 regidters

		MTU2.TMDR_0 =
			(1 << MTU2_TMDR_0_BFA_SHIFT) | // 1: TGRA and TGRC used together for buffer operation
			(0x00 << MTU2_TMDR_0_MD_SHIFT) | // MD 0: Normal operation
			0;

		// TGRA_0 Function and TIOC0A Pin Function
		MTU2.TCR_0 =
			(0x01 << MTU2_TCR_0_CCLR_SHIFT) | // CCLR 1: TCNT cleared by TGRA compare match/input capture
			0;


		MTU2.TIORH_0 = (MTU2.TIORH_0 & ~ (MTU2_TIORH_0_IOA)) |
			(0x07 << MTU2_TIORH_0_IOA_SHIFT) |
			0;

		MTU2.TGRC_0 = calcdivround_p0clock(1200000uL * 2) - 1;	// Use C intstead of A

		MTU2.TSTR |= MTU2_TSTR_CST0;

	}

	void hardware_dcdcfreq_tioc0a_mtu0_setdiv(uint_fast32_t v)
	{

		/* P2_8 TIOC0A (MTU0 output) */
		MTU2.TGRC_0 = v - 1;	// Use C intstead of A
	}

#endif

#endif /* WITHDCDCFREQCTL */


#if WITHSPIHW

	#if SPI_BIDIRECTIONAL
		#error WITHSPIHW and SPI_BIDIRECTIONAL can not be used together
	#endif

	#if CPUSTYLE_AT91SAM7S || CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S
		static portholder_t spi_csr_val8w [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
		static portholder_t spi_csr_val16w [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 в режиме 16-ти битных слов. */
	#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1
		static portholder_t spi_cfg1_val8w [SPIC_SPEEDS_COUNT];
		static portholder_t spi_cfg1_val16w [SPIC_SPEEDS_COUNT];
		static portholder_t spi_cfg1_val32w [SPIC_SPEEDS_COUNT];
		static portholder_t spi_cfg2_val [SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
	#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32L0XX
		static portholder_t spi_cr1_val8w [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
		static portholder_t spi_cr1_val16w [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 в режиме 16-ти битных слов. */
	#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX
		static portholder_t spi_cr1_val8w [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
		static portholder_t spi_cr1_val16w [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
	#elif CPUSTYLE_ATMEGA
		static portholder_t spcr_val [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
		static portholder_t spsr_val [SPIC_SPEEDS_COUNT];
	#elif CPUSTYLE_ATXMEGA
		static portholder_t spi_ctl_val [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
	#elif CPUSTYLE_R7S721
		static portholder_t spi_spbr_val [SPIC_SPEEDS_COUNT];
		static portholder_t spi_spcmd0_val8w [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
		static portholder_t spi_spcmd0_val16w [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
		static portholder_t spi_spcmd0_val32w [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
	#elif CPUSTYLE_XC7Z
		static portholder_t xc7z_spi_cr_val [SPIC_SPEEDS_COUNT][SPIC_MODES_COUNT];	/* для spi mode0..mode3 */
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
		DMA2_Stream0->PAR = (uintptr_t) & SPI1->RXDR;
	#else /* CPUSTYLE_STM32H7XX */
		const uint_fast8_t ch = 3;
		DMA2_Stream0->PAR = (uintptr_t) & SPI1->DR;
	#endif /* CPUSTYLE_STM32H7XX */

	DMA2_Stream0->FCR &= ~ (DMA_SxFCR_FEIE_Msk | DMA_SxFCR_DMDIS_Msk);	// use Direct mode
	//DMA2_Stream0->FCR |= DMA_SxFCR_DMDIS;	// Direct mode disabled
	(void) DMA2_Stream0->FCR;

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
	(void) DMA2_Stream0->CR;

#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX
	// DMAMUX init
	// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
	// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
	DMAMUX1_Channel8->CCR = 37 * DMAMUX_CxCR_DMAREQ_ID_0;	// SPI1_RX
	(void) DMAMUX1_Channel8->CCR;
#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX */

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
		DMA2_Stream3->PAR = (uintptr_t) & SPI1->TXDR;
	#else /* CPUSTYLE_STM32H7XX */
		const uint_fast8_t ch = 3;
		DMA2_Stream3->PAR = (uintptr_t) & SPI1->DR;
	#endif /* CPUSTYLE_STM32H7XX */


	DMA2_Stream3->FCR &= ~ (DMA_SxFCR_FEIE_Msk | DMA_SxFCR_DMDIS_Msk);	// use direct mode
	//DMA2_Stream3->FCR |= DMA_SxFCR_DMDIS;	// Direct mode disabled
	(void) DMA2_Stream3->FCR;

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
	(void) DMA2_Stream3->CR;

#if CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX
	// DMAMUX init
	// DMAMUX1 channels 0 to 7 are connected to DMA1 channels 0 to 7
	// DMAMUX1 channels 8 to 15 are connected to DMA2 channels 0 to 7
	DMAMUX1_Channel11->CCR = 38 * DMAMUX_CxCR_DMAREQ_ID_0;	// SPI1_TX
	(void) DMAMUX1_Channel11->CCR;
#endif /* CPUSTYLE_STM32MP1 || CPUSTYLE_STM32H7XX */

}

#if 1
// Ожидание завершения обмена
static void DMA2_waitTC(
	uint_fast8_t i		// 0..7 - номер Stream
	)
{
	uint_fast8_t mask = 1UL << ((i & 0x01) * 6);
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
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // подать тактирование
	(void) RCC->APB2ENR;
	SPI1->CR1 = 0x0000;             //очистить первый управляющий регистр
	SPI1->CR2 = 0x0000;	// SPI_CR2_SSOE;             //очистить второй управляющий регистр

	#if WITHSPIHWDMA
		DMA2_SPI1_TX_initialize();	// stream 3, канал 3
		DMA2_SPI1_RX_initialize();	// stream 0. канал 3
	#endif /* WITHSPIHWDMA */

	/* настраиваем в режиме disconnect */
	SPIIO_INITIALIZE();

#elif CTLSTYLE_V3D		// SPI2

	// Настроим модуль SPI.
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN; // подать тактирование
	(void) RCC->APB1ENR;

	/* настраиваем в режиме disconnect */
	SPIIO_INITIALIZE();

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32L0XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX

	// Настроим модуль SPI.
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // подать тактирование
	(void) RCC->APB2ENR;
	RCC->APB2LPENR |= RCC_APB2LPENR_SPI1LPEN; // подать тактирование
	(void) RCC->APB2LPENR;
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

	uint_fast8_t mid = 0x48;
	if (HW_SPIUSED == & RSPI0)
	{
		/* ---- Supply clock to the RSPI(channel 0) ---- */
		CPG.STBCR10 &= ~ CPG_STBCR10_BIT_MSTP107;	// Module Stop 107 RSPI0
		(void) CPG.STBCR10;			/* Dummy read */
		// Values from Table 9.4 On-Chip Peripheral Module Requests
		// SPTI0 (transmit data empty)
		mid = 0x48;
	}
	else if (HW_SPIUSED == & RSPI1)
	{
		/* ---- Supply clock to the RSPI(channel 1) ---- */
		CPG.STBCR10 &= ~ CPG_STBCR10_BIT_MSTP106;	// Module Stop 106 RSPI1
		(void) CPG.STBCR10;			/* Dummy read */
		// Values from Table 9.4 On-Chip Peripheral Module Requests
		// SPTI1 (transmit data empty)
		mid = 0x49;
	}
	else if (HW_SPIUSED == & RSPI2)
	{
		/* ---- Supply clock to the RSPI(channel 2) ---- */
		CPG.STBCR10 &= ~ CPG_STBCR10_BIT_MSTP105;	// Module Stop 105 RSPI2
		(void) CPG.STBCR10;			/* Dummy read */
		// Values from Table 9.4 On-Chip Peripheral Module Requests
		// SPTI2 (transmit data empty)
		mid = 0x4a;
	}

	HW_SPIUSED->SPCR =		/* Control Register (SPCR) */
		0;

	HW_SPIUSED->SPPCR =		/* Pin Control Register (SPPCR) */
		0x00 |
		0;
	HW_SPIUSED->SPSCR =		/*  (SPSCR) */
		0x00 |
		0;
	// Сбросить буферы
	HW_SPIUSED->SPBFCR =		/* Buffer Control Register (SPBFCR) */
		(1U << 7) |		// TXRST - TX buffer reset
		(1U << 6) |		// RXRST - TX buffer reset
		0;
	// Разрешить буферы
	HW_SPIUSED->SPBFCR =		/* Buffer Control Register (SPBFCR) */
		(3U << 4) |		// TX buffer trigger level = 0
		0;

	HW_SPIUSED->SPCR =		/* Control Register (SPCR) */
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
		//DMAC15.N0DA_n = (uint32_t) & HW_SPIUSED->SPDR.UINT8 [R_IO_LL];	// Fixed destination address for 8-bit transfers
		DMAC15.N0DA_n = (uint32_t) & HW_SPIUSED->SPDR.UINT16 [R_IO_L];	// Fixed destination address for 16-bit transfers

		/* Set Transfer Size */
		//DMAC15.N0TB_n = DMABUFFSIZE16 * sizeof (aubufv_t);	// размер в байтах
		//DMAC15.N1TB_n = DMABUFFSIZE16 * sizeof (aubufv_t);	// размер в байтах

		// Values from Table 9.4 On-Chip Peripheral Module Requests
		// SPTI0 (transmit data empty)
		//const uint_fast8_t mid = 0x48;
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
			// connect to interrupt
			//arm_hardware_set_handler_system(DMAINT15_IRQn, r7s721_usb0_dma1_dmatx_handler);
		}

		DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_SWRST;		// SWRST
		DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_CLRINTMSK;	// CLRINTMSK
		//DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_SETEN;		// SETEN
	}
#endif /* WITHSPIHWDMA */

	SPIIO_INITIALIZE();

#elif CPUSTYLE_STM32MP1

	RCC->MP_APB2ENSETR = RCC_MP_APB2ENSETR_SPI1EN; // подать тактирование
	(void) RCC->MP_APB2ENSETR;
	RCC->MP_APB2LPENSETR = RCC_MP_APB2LPENSETR_SPI1LPEN; // подать тактирование
	(void) RCC->MP_APB2LPENSETR;
	/* настраиваем в режиме disconnect */
	SPIIO_INITIALIZE();

#elif CPUSTYLE_XC7Z

	SCLR->SLCR_UNLOCK = 0x0000DF0DU;
	SCLR->APER_CLK_CTRL |= (0x01uL << 14);	// APER_CLK_CTRL.SPI0_CPU_1XCLKACT
	(void) SCLR->APER_CLK_CTRL;


	// Set DIVISOR
	SCLR->SPI_CLK_CTRL = (SCLR->SPI_CLK_CTRL & ~ (0x3FuL << 8)) |
			(SCLR_SPI_CLK_CTRL_DIVISOR_VALUE << 8) |
			0;

//	PRINTF("1 XQSPIPS->CR=%08lX\n", XQSPIPS->CR);
	// после reset не работает
//	SCLR->LQSPI_RST_CTRL |= 0x01;
//	(void) SCLR->LQSPI_RST_CTRL;
//	SCLR->LQSPI_RST_CTRL &= ~ 0x01;
//	(void) SCLR->LQSPI_RST_CTRL;

//	PRINTF("2 XQSPIPS->CR=%08lX\n", XQSPIPS->CR);

	//PRINTF("SPI0->Mod_id_reg0=%08lX (expected 0x00090106)\n", SPI0->Mod_id_reg0);
	ASSERT(SPI0->Mod_id_reg0 == 0x00090106uL);

	SPIIO_INITIALIZE();

#else
	#error Wrong CPUSTYLE macro

#endif
}

#endif /* WITHSPIHW */

#if WITHSPIHW


void hardware_spi_master_setfreq(spi_speeds_t spispeedindex, int_fast32_t spispeed)
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

	// Использование автоматического расчёта предделителя
	unsigned value;	/* делителя нет, есть только прескалер - значение делителя не используется */
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, spispeed), ATMEGA_SPCR_WIDTH, ATMEGA_SPCR_TAPS, & value, 1);
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
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, spispeed), ATXMEGA_SPIBR_WIDTH, ATXMEGA_SPIBR_TAPS, & value, 1);
	const uint_fast8_t ctl = spi_ctl [prei] | SPI_MASTER_bm | SPI_ENABLE_bm;
	// С FRAM FM25L04 работает MODE3 и MODE0
	spi_ctl_val [spispeedindex][SPIC_MODE0] = SPI_MODE_0_gc | ctl;	// SPI MODE0,
	spi_ctl_val [spispeedindex][SPIC_MODE1] = SPI_MODE_1_gc | ctl;	// SPI MODE1,
	spi_ctl_val [spispeedindex][SPIC_MODE2] = SPI_MODE_2_gc | ctl;	// SPI MODE2,
	spi_ctl_val [spispeedindex][SPIC_MODE3] = SPI_MODE_3_gc | ctl;	// SPI MODE3,

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX || CPUSTYLE_STM32L0XX

	unsigned value;	/* делителя нет, есть только прескалер - значение делителя не используется */
	const uint_fast8_t prei = calcdivider(calcdivround2(BOARD_SPI_FREQ, spispeed), STM32F_SPIBR_WIDTH, STM32F_SPIBR_TAPS, & value, 1);

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
	const uint_fast8_t prei = calcdivider(calcdivround2(BOARD_SPI_FREQ, spispeed), STM32F_SPIBR_WIDTH, STM32F_SPIBR_TAPS, & value, 1);

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

	spi_cr1_val16w [spispeedindex][SPIC_MODE0] = cr1bits | CR1_MODE0;
	spi_cr1_val16w [spispeedindex][SPIC_MODE1] = cr1bits | CR1_MODE1;
	spi_cr1_val16w [spispeedindex][SPIC_MODE2] = cr1bits | CR1_MODE2;
	spi_cr1_val16w [spispeedindex][SPIC_MODE3] = cr1bits | CR1_MODE3;

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	unsigned value;	/* делителя нет, есть только прескалер - значение делителя не используется */
	const uint_fast8_t prei = calcdivider(calcdivround2(BOARD_SPI_FREQ, spispeed), STM32F_SPIBR_WIDTH, STM32F_SPIBR_TAPS, & value, 1);
	const uint_fast32_t cfg1baudrate = (prei * SPI_CFG1_MBR_0) & SPI_CFG1_MBR_Msk;
	const uint_fast32_t cfg1 = cfg1baudrate;// | (SPI_CFG1_CRCSIZE_0 * 7);
	//PRINTF(PSTR("hardware_spi_master_setfreq: prei=%u, value=%u, spispeed=%u\n"), prei, value, spispeed);

	spi_cfg1_val8w [spispeedindex] = cfg1 |
		7 * SPI_CFG1_DSIZE_0 |
		0;

	spi_cfg1_val16w [spispeedindex] = cfg1 |
		15 * SPI_CFG1_DSIZE_0 |
		0;

	spi_cfg1_val32w [spispeedindex] = cfg1 |
		31 * SPI_CFG1_DSIZE_0 |
		0;

	const uint_fast32_t cfg2bits =
			SPI_CFG2_SSOM_Msk |
			SPI_CFG2_SSOE_Msk |
			SPI_CFG2_SSM_Msk |	// 1: SS input value is determined by the SSI bit
			SPI_CFG2_MASTER_Msk |
			SPI_CFG2_AFCNTR_Msk | // 1: the peripheral keeps always control of all associated GPIOs
			0;
	enum
	{
		CFG2_MODE0 = 0,				// TODO: not tested
		CFG2_MODE1 = SPI_CFG2_CPHA_Msk,	// TODO: not tested
		CFG2_MODE2 = SPI_CFG2_CPOL_Msk,	// CLK leave HIGH
		CFG2_MODE3 = SPI_CFG2_CPOL_Msk | SPI_CFG2_CPHA_Msk		// wrk = CLK leave "HIGH"
	};

	// подготовка управляющих слов для разных spi mode, используемых контроллером.
	// 8-битная или 16-битная передача программируется в CR2
	spi_cfg2_val [SPIC_MODE0] = cfg2bits | CFG2_MODE0;
	spi_cfg2_val [SPIC_MODE1] = cfg2bits | CFG2_MODE1;
	spi_cfg2_val [SPIC_MODE2] = cfg2bits | CFG2_MODE2;
	spi_cfg2_val [SPIC_MODE3] = cfg2bits | CFG2_MODE3;

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

	//value = 59, prei = 0;	// 500 kHz
	//value = 59, prei = 1;	// 250 kHz
	//value = 59, prei = 2;	// 125 kHz
	//value = 29, prei = 3;	// 125 kHz
	//value = 149, prei = 1;	// 100 kHz

	//PRINTF(PSTR("hardware_spi_master_setfreq: prei=%u, value=%u, spispeed[%u]=%lu\n"), prei, value, spispeedindex, (unsigned long) spispeed);

	const uint_fast8_t spcmd0 =	// Command Register (SPCMD)
		(RSPIn_SPCMD0_BRDV & (prei << RSPIn_SPCMD0_BRDV_SHIFT)) |	// BRDV1..BRDV0 - Bit Rate Division Setting /1, /2, /4. /8
		0;

	spi_spbr_val [spispeedindex] = value;	// Bit Rate Register (SPBR)
	// подготовка управляющих слов для разных spi mode, используемых контроллером.
	// 8-битная передача
	const uint_fast16_t spcmd8bw = spcmd0 | 0x0700;	// 0x0700 - 8 bit

	spi_spcmd0_val8w [spispeedindex][SPIC_MODE0] = spcmd8bw | SPCMD_MODE0;
	spi_spcmd0_val8w [spispeedindex][SPIC_MODE1] = spcmd8bw | SPCMD_MODE1;
	spi_spcmd0_val8w [spispeedindex][SPIC_MODE2] = spcmd8bw | SPCMD_MODE2;
	spi_spcmd0_val8w [spispeedindex][SPIC_MODE3] = spcmd8bw | SPCMD_MODE3;

	// подготовка управляющих слов для разных spi mode, используемых контроллером.
	// 16-битная передача
	const uint16_t spcmd16w = spcmd0 | 0x0F00;	// 0x0F00 - 16 bit

	spi_spcmd0_val16w [spispeedindex][SPIC_MODE0] = spcmd16w | SPCMD_MODE0;
	spi_spcmd0_val16w [spispeedindex][SPIC_MODE1] = spcmd16w | SPCMD_MODE1;
	spi_spcmd0_val16w [spispeedindex][SPIC_MODE2] = spcmd16w | SPCMD_MODE2;
	spi_spcmd0_val16w [spispeedindex][SPIC_MODE3] = spcmd16w | SPCMD_MODE3;

	// подготовка управляющих слов для разных spi mode, используемых контроллером.
	// 16-битная передача
	const uint16_t spcmd32w = spcmd0 | 0x0200;	// 0x0200 or 0x0300 - 32 bit

	spi_spcmd0_val32w [spispeedindex][SPIC_MODE0] = spcmd32w | SPCMD_MODE0;
	spi_spcmd0_val32w [spispeedindex][SPIC_MODE1] = spcmd32w | SPCMD_MODE1;
	spi_spcmd0_val32w [spispeedindex][SPIC_MODE2] = spcmd32w | SPCMD_MODE2;
	spi_spcmd0_val32w [spispeedindex][SPIC_MODE3] = spcmd32w | SPCMD_MODE3;

#elif CPUSTYLE_XC7Z

	enum
	{
		SPICR_CPHA = 1u << 2,
		SPICR_CPOL = 1u << 1,

		SPICR_MODE0 = 0,
		SPICR_MODE1 = SPICR_CPHA,
		SPICR_MODE2 = SPICR_CPOL,
		SPICR_MODE3 = SPICR_CPOL | SPICR_CPHA
	};

	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(BOARD_SPI_FREQ, spispeed), XC7Z_SPI_BR_WIDTH, XC7Z_SPI_BR_TAPS, & value, 1);

	unsigned brdiv = ulmin(prei + 1, 7);
	//PRINTF("hardware_spi_master_setfreq: prei=%u, value=%u, spispeed=%u, brdiv=%u (clk=%lu)\n", prei, value, spispeed, brdiv, xc7z_get_spi_freq());

	const portholder_t cr_val =
			//(1uL << 17) |	// ModeFail Generation Enable
			//(1uL << 16) |	// Manual Start Command
			//(1uL << 15) |	// Manual Start Enable
//			(1uL << 14) |	// Manual CS
//			(0x0FuL << 10) |	// 1111 - No slave selected
			(brdiv << 3) |	// BAUD_RATE_DIV: 001: divide by 4, ... 111: divide by 256
			(1uL << 0) |	// 1: the SPI is in master mode
			0;

	xc7z_spi_cr_val [spispeedindex][SPIC_MODE0] = cr_val | SPICR_MODE0;
	xc7z_spi_cr_val [spispeedindex][SPIC_MODE1] = cr_val | SPICR_MODE1;
	xc7z_spi_cr_val [spispeedindex][SPIC_MODE2] = cr_val | SPICR_MODE2;
	xc7z_spi_cr_val [spispeedindex][SPIC_MODE3] = cr_val | SPICR_MODE3;

#else
	#error Wrong CPUSTYLE macro

#endif
}

/* управление состоянием "подключено" */
void hardware_spi_connect(spi_speeds_t spispeedindex, spi_modes_t spimode)
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

	// В этих процессорах и входы и выходы переключаются на ALT FN
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

#elif CTLSTYLE_V3D		// SPI2

	HARDWARE_SPI_CONNECT();
	SPI2->CR1 = spi_cr1_val8w [spispeedindex][spimode];
	SPI2->CR2 = (SPI2->CR2 & ~ (SPI_CR2_DS)) |
		7 * SPI_CR2_DS_0 |	// 8 bit word length
		1 * SPI_CR2_FRXTH |			// RXFIFO threshold is set to 8 bits (FRXTH=1).
		0;

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F7XX

	// В этих процессорах и входы и выходы переключаются на ALT FN
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

	SPI1->CFG1 = spi_cfg1_val8w [spispeedindex];
	SPI1->CFG2 = spi_cfg2_val [spimode];
	SPI1->CR1 |= SPI_CR1_SSI;

	SPI1->CR1 |= SPI_CR1_SPE;
	SPI1->CR1 |= SPI_CR1_CSTART;

#elif CPUSTYLE_R7S721

	HW_SPIUSED->SPDCR =		/* Data Control Register (SPDCR) */
		(0x01 << 5) |	// 0x01: 8 bit. Specifies the width for accessing the data register (SPDR)
		0;
	HW_SPIUSED->SPBR = spi_spbr_val [spispeedindex];
	HW_SPIUSED->SPCMD0 = spi_spcmd0_val8w [spispeedindex][spimode];

	HARDWARE_SPI_CONNECT();

#elif CPUSTYLE_STM32MP1

	HARDWARE_SPI_CONNECT();

	SPI1->CFG1 = spi_cfg1_val8w [spispeedindex];
	SPI1->CFG2 = spi_cfg2_val [spimode];
	SPI1->CR1 |= SPI_CR1_SSI;

	SPI1->CR1 |= SPI_CR1_SPE;
	SPI1->CR1 |= SPI_CR1_CSTART;

#elif CPUSTYLE_XC7Z

	SPI0->CR = xc7z_spi_cr_val [spispeedindex][spimode];
	SPI0->ER = 0x0001;	// 1: enable the SPI

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

#elif CPUSTYLE_STM32H7XX

	SPI1->CR1 |= SPI_CR1_CSUSP;
	while ((SPI1->CR1 & SPI_CR1_CSTART) != 0)
		;
	SPI1->CR1 &= ~ SPI_CR1_SPE;
	// connect back to GPIO
	HARDWARE_SPI_DISCONNECT();

#elif CTLSTYLE_V3D		// SPI2

	SPI2->CR1 &= ~ SPI_CR1_SPE;
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

#elif CPUSTYLE_STM32MP1
	//#warning Insert code for CPUSTYLE_STM32MP1

	SPI1->CR1 |= SPI_CR1_CSUSP;
	while ((SPI1->CR1 & SPI_CR1_CSTART) != 0)
		;
	SPI1->CR1 &= ~ SPI_CR1_SPE;
	// connect back to GPIO
	HARDWARE_SPI_DISCONNECT();

#elif CPUSTYLE_XC7Z

	SPI0->ER = 0x0000;	// 0: disable the SPI
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

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	//while ((SPI1->SR & SPI_SR_TXC) == 0)
	//	;
	while ((SPI1->SR & SPI_SR_RXP) == 0)
		;
	(void) * (volatile uint8_t *) & SPI1->RXDR;	/* clear SPI_SR_RXP in status register */

#elif CTLSTYLE_V3D		// SPI2

	while ((SPI2->SR & SPI_SR_RXNE) == 0)
			;
		(void) SPI2->DR;	/* clear SPI_SR_RXNE in status register */

#elif CPUSTYLE_STM32F

	while ((SPI1->SR & SPI_SR_RXNE) == 0)
		;
	(void) SPI1->DR;	/* clear SPI_SR_RXNE in status register */

#elif CPUSTYLE_R7S721

	while ((HW_SPIUSED->SPSR & (1U << 7)) == 0)	// SPRF bit
		;
	(void) HW_SPIUSED->SPDR.UINT8 [R_IO_LL]; // LL=0

#elif CPUSTYLE_XC7Z

	while ((SPI0->SR & (1uL << 4)) == 0)	// RX FIFO not empty
	{
		//PRINTF("SPI0->SR=%08lX\n", SPI0->SR);
		//local_delay_ms(100);
		;
	}
	(void) SPI0->RXD;

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
	return (SPI->SPI_RDR & SPI_TDR_TD_Msk) & 0xFF;

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

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	//while ((SPI1->SR & SPI_SR_TXC) == 0)
	//	;
	while ((SPI1->SR & SPI_SR_RXP) == 0)
		;
	const portholder_t t = * (volatile uint8_t *) & SPI1->RXDR;	// prevent data packing feature
	return t;

#elif CTLSTYLE_V3D		// SPI2

	while ((SPI2->SR & SPI_SR_RXNE) == 0)
		;
	const portholder_t t = SPI2->DR & 0xFF;	/* clear SPI_SR_RXNE in status register */
	while ((SPI2->SR & SPI_SR_BSY) != 0)
		;
	return t;

#elif CPUSTYLE_STM32F

	while ((SPI1->SR & SPI_SR_RXNE) == 0)
		;
	const portholder_t t = SPI1->DR & 0xFF;	/* clear SPI_SR_RXNE in status register */
	while ((SPI1->SR & SPI_SR_BSY) != 0)
		;
	return t;

#elif CPUSTYLE_R7S721

	while ((HW_SPIUSED->SPSR & (1U << 7)) == 0)	// SPRF bit
		;
	return HW_SPIUSED->SPDR.UINT8 [R_IO_LL]; // LL=0

#elif CPUSTYLE_XC7Z

	while ((SPI0->SR & (1uL << 4)) == 0)	// RX FIFO not empty
	{
		//PRINTF("SPI0->SR=%08lX\n", SPI0->SR);
		//local_delay_ms(100);
		;
	}
	return SPI0->RXD & 0xFF;

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
	DMAC15.N0SA_n = (uint32_t) & HW_SPIUSED->SPDR.UINT8 [R_IO_LL];	// Fixed destination address for 8-bit transfers
	//DMAC15.N0SA_n = (uint32_t) & HW_SPIUSED->SPDR.UINT16 [R_IO_L];	// Fixed destination address for 16-bit transfers
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
	//DMAC15.N0SA_n = (uint32_t) & HW_SPIUSED->SPDR.UINT8 [R_IO_LL];	// Fixed source address for 8-bit transfers
	DMAC15.N0SA_n = (uint32_t) & HW_SPIUSED->SPDR.UINT16 [R_IO_L];	// Fixed source address for 16-bit transfers
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
	DMAC15.N0DA_n = (uint32_t) & HW_SPIUSED->SPDR.UINT8 [R_IO_LL];	// Fixed destination address for 8-bit transfers
	//DMAC15.N0DA_n = (uint32_t) & HW_SPIUSED->SPDR.UINT16 [R_IO_L];	// Fixed destination address for 16-bit transfers
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
	//DMAC15.N0DA_n = (uint32_t) & HW_SPIUSED->SPDR.UINT8 [R_IO_LL];	// Fixed destination address for 8-bit transfers
	DMAC15.N0DA_n = (uint32_t) & HW_SPIUSED->SPDR.UINT16 [R_IO_L];	// Fixed destination address for 16-bit transfers
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
		PRINTF(PSTR("hardware_spi_master_send_frame: use CCM\n"));
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

	DMA2_Stream3->CR &= ~ DMA_SxCR_EN;
	while ((DMA2_Stream3->CR &  DMA_SxCR_EN) != 0)
		;

	#if CPUSTYLE_STM32H7XX
		SPI1->CFG1 &= ~ SPI_CFG1_TXDMAEN; // запретить DMA по передаче
	#else /* CPUSTYLE_STM32H7XX */
		SPI1->CR2 &= ~ SPI_CR2_TXDMAEN; // запретить DMA по передаче
	#endif /* CPUSTYLE_STM32H7XX */

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

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX
	#warning TODO: implement SPI over DMA

	//xprog_spi_send_frame(target, buffer, size);

#elif CPUSTYLE_STM32F1XX
	#warning TODO: implement SPI over DMA

	//xprog_spi_send_frame(target, buffer, size);

#elif CPUSTYLE_ATXMEGA
	#warning TODO: implement SPI over DMA

	//xprog_spi_send_frame(target, buffer, size);

#elif CPUSTYLE_R7S721

	HW_SPIUSED->SPBFCR |= RSPIn_SPBFCR_RXRST;		// Запретить прием

	DMAC15.N0TB_n = (uint_fast32_t) size * sizeof (* buffer);	// размер в байтах
	DMAC15.N0SA_n = (uintptr_t) buffer;			// source address
	DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_SETEN;		// SETEN

	/* ждем окончания пересылки */
	while ((DMAC15.CHSTAT_n & DMAC15_CHSTAT_n_END) == 0)	// END
		;

	/* ждем окончания передачи последнего элемента */
	while ((HW_SPIUSED->SPSR & RSPIn_SPSR_TEND) == 0)	// TEND bit
		;

	DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_CLREN;		// CLREN
	DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_CLRTC;		// CLRTC
	DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_CLREND;		// CLREND

	HW_SPIUSED->SPBFCR &= ~ RSPIn_SPBFCR_RXRST;		// Разрешить прием

#elif CPUSTYLE_STM32MP1
	#warning Insert code for CPUSTYLE_STM32MP1

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
		PRINTF(PSTR("hardware_spi_master_send_frame: use CCM\n"));
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

	DMA2_Stream3->CR &= ~ DMA_SxCR_EN;
	while ((DMA2_Stream3->CR &  DMA_SxCR_EN) != 0)
		;

	#if CPUSTYLE_STM32H7XX
		SPI1->CFG1 &= ~ SPI_CFG1_TXDMAEN; // запретить DMA по передаче
	#else /* CPUSTYLE_STM32H7XX */
		SPI1->CR2 &= ~ SPI_CR2_TXDMAEN; // запретить DMA по передаче
	#endif /* CPUSTYLE_STM32H7XX */

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

	HW_SPIUSED->SPBFCR |= RSPIn_SPBFCR_RXRST;		// Запретить прием

	DMAC15.N0TB_n = (uint_fast32_t) size * sizeof (* buffer);	// размер в байтах
	DMAC15.N0SA_n = (uintptr_t) buffer;			// source address
	DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_SETEN;		// SETEN

	/* ждем окончания пересылки */
	while ((DMAC15.CHSTAT_n & DMAC15_CHSTAT_n_END) == 0)	// END
		;

	/* ждем окончания передачи последнего элемента */
	while ((HW_SPIUSED->SPSR & RSPIn_SPSR_TEND) == 0)	// TEND bit
		;

	DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_CLREN;		// CLREN
	DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_CLRTC;		// CLRTC
	DMAC15.CHCTRL_n = DMAC15_CHCTRL_n_CLREND;		// CLREND

	HW_SPIUSED->SPBFCR &= ~ RSPIn_SPBFCR_RXRST;		// Запретить прием

#elif CPUSTYLE_STM32MP1
	#warning Insert code for CPUSTYLE_STM32MP1

#else
	#error Undefined CPUSTYLE_xxxx
#endif
}

// Read a frame of bytes via SPI
// На сигнале MOSI при это должно обеспечиваться состояние логической "1" для корректной работы SD CARD
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
		PRINTF(PSTR("hardware_spi_master_read_frame: use CCM\n"));
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
	//#warning TODO: Add code for R7S721 SPI DMA support to hardware_spi_master_read_frame_16bpartial

	HARDWARE_SPI_DISCONNECT_MOSI();	// выход данных в "1" - для нормальной работы SD CARD

	HARDWARE_SPI_CONNECT_MOSI();	// Возвращаем в обычный режим работы

#elif CPUSTYLE_STM32MP1
	#warning Insert code for CPUSTYLE_STM32MP1


#else
	#error Undefined CPUSTYLE_xxxx

#endif
}

#endif /* WITHSPI16BIT */

// Read a frame of bytes via SPI
// На сигнале MOSI при это должно обеспечиваться состояние логической "1" для корректной работы SD CARD
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
		PRINTF(PSTR("hardware_spi_master_read_frame: use CCM\n"));
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
	//#warning TODO: Add code for R7S721 SPI DMA support to hardware_spi_master_read_frame_8bpartial

	HARDWARE_SPI_DISCONNECT_MOSI();	// выход данных в "1" - для нормальной работы SD CARD

	HARDWARE_SPI_CONNECT_MOSI();	// Возвращаем в обычный режим работы

#elif CPUSTYLE_STM32MP1
	#warning Insert code for CPUSTYLE_STM32MP1


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

/* управление состоянием "подключено" - работа в режиме 16-ти битных слов.*/
void hardware_spi_connect_b16(spi_speeds_t spispeedindex, spi_modes_t spimode)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	// инициализация контроллера SPI
	enum { OUTMASK = PIO_PA13A_MOSI | PIO_PA14A_SPCK };		// битовая маска, определяет каким выводом шевелить
	enum { INPMASK = PIO_PA12A_MISO };		// битовая маска, определяет откуда ввод
	enum { WORKMASK = OUTMASK | INPMASK };		// битовая маска, включает и ввод и вывод

	SPI->SPI_CSR [0] = spi_csr_val16w [spispeedindex] [spimode];

	(void) SPI->SPI_RDR;		/* clear AT91C_SPI_RDRF in status register */
	HARDWARE_SPI_CONNECT();

#elif CPUSTYLE_AT91SAM7S

	enum { OUTMASK = AT91C_PA13_MOSI | AT91C_PA14_SPCK };		// битовая маска, определяет каким выводом шевелить
	enum { INPMASK = AT91C_PA12_MISO };		// битовая маска, определяет откуда ввод
	enum { WORKMASK = OUTMASK | INPMASK };		// битовая маска, включает и ввод и вывод

	AT91C_BASE_SPI->SPI_CSR [0] = spi_csr_val16w [spispeedindex] [spimode];

	(void) AT91C_BASE_SPI->SPI_RDR;		/* clear AT91C_SPI_RDRF in status register */
	HARDWARE_SPI_CONNECT();

#elif CPUSTYLE_STM32F1XX

	HARDWARE_SPI_CONNECT();
	SPI1->CR1 = spi_cr1_val16w [spispeedindex] [spimode];
	#if WITHTWIHW
		// Silicon errata:
		// 2.6.7 I2C1 with SPI1 remapped and used in master mode
		// Workaround:
		// When using SPI1 remapped, the I2C1 clock must be disabled.
		RCC->APB1ENR &= ~ (RCC_APB1ENR_I2C1EN); // выкл тактирование контроллера I2C
		__DSB();
	#endif

#elif CPUSTYLE_STM32F4XX || CPUSTYLE_STM32L0XX

	// В этих процессорах и входы и выходы переключаются на ALT FN
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

	// В этих процессорах и входы и выходы переключаются на ALT FN
	HARDWARE_SPI_CONNECT();

	SPI1->CR1 = spi_cr1_val16w [spispeedindex] [spimode];
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

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

		HARDWARE_SPI_CONNECT();

		SPI1->CFG1 = spi_cfg1_val16w [spispeedindex];
		SPI1->CFG2 = spi_cfg2_val [spimode];
		SPI1->CR1 |= SPI_CR1_SSI;

		SPI1->CR1 |= SPI_CR1_SPE;
		SPI1->CR1 |= SPI_CR1_CSTART;

#elif CPUSTYLE_R7S721

	HW_SPIUSED->SPDCR =		/* Data Control Register (SPDCR) */
		(0x02 << 5) |	// 10: SPDR is accessed in words (16 bits).
		0;
	HW_SPIUSED->SPBR = spi_spbr_val [spispeedindex];
	HW_SPIUSED->SPCMD0 = spi_spcmd0_val16w [spispeedindex] [spimode];

	HARDWARE_SPI_CONNECT();

#else
	#error Wrong CPUSTYLE macro
#endif

}

portholder_t RAMFUNC hardware_spi_complete_b16(void)	/* дождаться готовности */
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

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

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

	while ((HW_SPIUSED->SPSR & (1U << 7)) == 0)	// SPRF bit
		;
	return HW_SPIUSED->SPDR.UINT16 [R_IO_L]; // L=0

#else
	#error Wrong CPUSTYLE macro
#endif
}

static RAMFUNC void hardware_spi_ready_b16_void(void)	/* дождаться готовности */
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

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

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

	while ((HW_SPIUSED->SPSR & (1U << 7)) == 0)	// SPRF bit
		;
	(void) HW_SPIUSED->SPDR.UINT16 [R_IO_L];	 // L=0

#else
	#error Wrong CPUSTYLE macro
#endif
}


/* группа функций для использования в групповых передачах по SPI */
/* передача первого байта в последовательности - Не проверяем готовность перед передачей,
   завершение передачи будут проверять другие.
*/
void RAMFUNC hardware_spi_b16_p1(
	portholder_t v		/* значениеслова для передачи */
	)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	SPI->SPI_TDR = SPI_TDR_TD(v);

#elif CPUSTYLE_AT91SAM7S

	AT91C_BASE_SPI->SPI_TDR = v & AT91C_SPI_TD;

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	* (volatile uint16_t *) & (SPI1)->TXDR = v;	// prevent data packing feature

#elif CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F30X || CPUSTYLE_STM32F7XX

	* (volatile uint16_t *) & (SPI1)->DR = v;	// prevent data packing feature

#elif CPUSTYLE_STM32F

	(SPI1)->DR = v;

#elif CPUSTYLE_R7S721

	HW_SPIUSED->SPDR.UINT16 [R_IO_L] = v; // L=0

#else
	#error Wrong CPUSTYLE macro
#endif
}

/* передача одного из средних байтов/слов в последовательности */
/* дождаться готовности, передача байта */
portholder_t RAMFUNC hardware_spi_b16_p2(
	portholder_t v		/* значение байта для передачи */
	)
{
	portholder_t r;
	r = hardware_spi_complete_b16();	/* дождаться завершения передачи */
	hardware_spi_b16_p1(v);	/* передать символ */
	return r;
}

/* передача байта/слова, возврат считанного */
portholder_t RAMFUNC hardware_spi_b16(
	portholder_t v		/* значение байта для передачи */
	)
{
	hardware_spi_b16_p1(v);	/* передать символ */
	return hardware_spi_complete_b16();	/* дождаться завершения передачи */
}

#endif /* WITHSPI16BIT */

#if WITHSPI32BIT

/* управление состоянием "подключено" - работа в режиме 32-ти битных слов. */
void hardware_spi_connect_b32(spi_speeds_t spispeedindex, spi_modes_t spimode)
{
#if CPUSTYLE_STM32H7XX

	HARDWARE_SPI_CONNECT();

	SPI1->CFG1 = spi_cfg1_val32w [spispeedindex];
	SPI1->CFG2 = spi_cfg2_val [spimode];
	SPI1->CR1 |= SPI_CR1_SSI;

	SPI1->CR1 |= SPI_CR1_SPE;
	SPI1->CR1 |= SPI_CR1_CSTART;

#elif CPUSTYLE_R7S721

	HW_SPIUSED->SPDCR =		/* Data Control Register (SPDCR) */
		(0x03 << 5) |	// 11: SPDR is accessed in longwords (32 bits).
		0;
	HW_SPIUSED->SPBR = spi_spbr_val [spispeedindex];
	HW_SPIUSED->SPCMD0 = spi_spcmd0_val32w [spispeedindex] [spimode];

	HARDWARE_SPI_CONNECT();

#elif CPUSTYLE_STM32MP1

	HARDWARE_SPI_CONNECT();

	SPI1->CFG1 = spi_cfg1_val32w [spispeedindex];
	SPI1->CFG2 = spi_cfg2_val [spimode];
	SPI1->CR1 |= SPI_CR1_SSI;

	SPI1->CR1 |= SPI_CR1_SPE;
	SPI1->CR1 |= SPI_CR1_CSTART;

#else
	#error Wrong CPUSTYLE macro
#endif

}

portholder_t hardware_spi_complete_b32(void)	/* дождаться готовности */
{
#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	//while ((SPI1->SR & SPI_SR_TXC) == 0)
	//	;
	while ((SPI1->SR & SPI_SR_RXP) == 0)
		;
	const portholder_t t = * (volatile uint32_t *) & SPI1->RXDR;	/* SPI_RXDR_RXDR clear SPI_SR_RXNE in status register */
	return t;

#elif CPUSTYLE_R7S721

	while ((HW_SPIUSED->SPSR & (1U << 7)) == 0)	// SPRF bit
		;
	return HW_SPIUSED->SPDR.UINT32;

#else
	#error Wrong CPUSTYLE macro
#endif
}

static void hardware_spi_ready_b32_void(void)	/* дождаться готовности */
{
#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	//while ((SPI1->SR & SPI_SR_TXC) == 0)
	//	;
	while ((SPI1->SR & SPI_SR_RXP) == 0)
		;
	(void) * (volatile uint32_t *) & SPI1->RXDR;	/* clear SPI_SR_RXNE in status register */

#elif CPUSTYLE_R7S721

	while ((HW_SPIUSED->SPSR & (1U << 7)) == 0)	// SPRF bit
		;
	(void) HW_SPIUSED->SPDR.UINT32;

#else
	#error Wrong CPUSTYLE macro
#endif
}


/* группа функций для использования в групповых передачах по SPI */
/* передача первого байта в последовательности - Не проверяем готовность перед передачей,
   завершение передачи будут проверять другие.
*/
void hardware_spi_b32_p1(
	portholder_t v		/* значениеслова для передачи */
	)
{
#if CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	* (volatile uint32_t *) & (SPI1)->TXDR = v;	// prevent data packing feature

#elif CPUSTYLE_R7S721

	HW_SPIUSED->SPDR.UINT32 = v;

#else
	#error Wrong CPUSTYLE macro
#endif
}

/* передача одного из средних байтов/слов в последовательности */
/* дождаться готовности, передача байта */
portholder_t hardware_spi_b32_p2(
	portholder_t v		/* значение байта для передачи */
	)
{
	portholder_t r;
	r = hardware_spi_complete_b32();	/* дождаться завершения передачи */
	hardware_spi_b32_p1(v);	/* передать символ */
	return r;
}

/* передача байта/слова, возврат считанного */
portholder_t hardware_spi_b32(
	portholder_t v		/* значение байта для передачи */
	)
{
	hardware_spi_b32_p1(v);	/* передать символ */
	return hardware_spi_complete_b32();	/* дождаться завершения передачи */
}

#endif /* WITHSPI32BIT */

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

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	* (volatile uint8_t *) & (SPI1)->TXDR = v;	// prevent data packing feature

#elif CTLSTYLE_V3D		// SPI2

	* (volatile uint8_t *) & (SPI2)->DR = v;	// prevent data packing feature

#elif CPUSTYLE_STM32F0XX || CPUSTYLE_STM32F30X || CPUSTYLE_STM32F7XX

	* (volatile uint8_t *) & (SPI1)->DR = v;	// prevent data packing feature

#elif CPUSTYLE_STM32F

	SPI1->DR = v;

#elif CPUSTYLE_R7S721

	HW_SPIUSED->SPDR.UINT8 [R_IO_LL] = v; // LL=0

#elif CPUSTYLE_XC7Z

	SPI0->TXD = v;
	while ((SPI0->SR & (1uL << 2)) == 0)	// TX FIFO not full
	{
		//PRINTF("SPI0->SR=%08lX\n", SPI0->SR);
		//local_delay_ms(100);
		;
	}

#else
	#error Wrong CPUSTYLE macro
#endif
}

/* передача одного из средних байтов/слов в последовательности */
/* дождаться готовности, передача байта */
portholder_t hardware_spi_b8_p2(
	portholder_t v		/* значение байта для передачи */
	)
{
	portholder_t r;
	r = hardware_spi_complete_b8();	/* дождаться завершения передачи (на atmega оптимизированно по скорости - без чиения регистра данных). */
	hardware_spi_b8_p1(v);	/* передать символ */
	return r;
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


// ADC init function

#if WITHCPUADCHW

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
	calcdivider(calcdivround2(CPU_FREQ, ADC_FREQ), ATSAM3S_ADC_PRESCAL_WIDTH, ATSAM3S_ADC_PRESCAL_TAPS, & prescal, 1);
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

	NVIC_SetVector(ADC_IRQn, (uintptr_t) & ADC_Handler);
	NVIC_SetPriority(ADC_IRQn, ARM_SYSTEM_PRIORITY);
	NVIC_EnableIRQ(ADC_IRQn);		// enable ADC_Handler();

#elif CPUSTYLE_AT91SAM7S

	AT91C_BASE_PMC->PMC_PCER = (1UL << AT91C_ID_ADC);		// разрешить тактовую для ADC
	AT91C_BASE_ADC->ADC_CR = AT91C_ADC_SWRST;	// reset ADC

	// adc characteristics: in 10 bit mode ADC clock max is 5 MHz, in 8 bit mode - 8 MHz
	// Track and Hold Acquisition Time - 600 nS

	unsigned prescal;
	calcdivider(calcdivround2(CPU_FREQ, ADC_FREQ), AT91SAM7_ADC_PRESCAL_WIDTH, AT91SAM7_ADC_PRESCAL_TAPS, & prescal, 1);
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
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, ADC_FREQ), ATMEGA_ADPS_WIDTH, ATMEGA_ADPS_TAPS, & value, 1);

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
	////const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, ADC_FREQ), ATXMEGA_ADPS_WIDTH, ATXMEGA_ADPS_TAPS, & value, 1);

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
		(void) RCC->APB2ENR;
		arm_hardware_set_handler_system(ADC1_2_IRQn, ADC1_2_IRQHandler);

	#else /* defined (ADC2) */

		RCC->APB2ENR |= (RCC_APB2ENR_ADC1EN);    // Затактировали АЦП
		(void) RCC->APB2ENR;
		arm_hardware_set_handler_system(ADC1_IRQn, ADC1_IRQHandler);

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

#elif CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	const uint_fast32_t ainmask = build_adc_mask();
	HARDWARE_ADC_INITIALIZE(ainmask);

	#if CPUSTYLE_STM32MP1
		RCC->MP_AHB2ENSETR = RCC_MP_AHB2ENSETR_ADC12EN;	// Затактировали АЦП
		(void) RCC->MP_AHB2ENSETR;
		RCC->MP_AHB2LPENSETR = RCC_MP_AHB2LPENSETR_ADC12LPEN;	// Затактировали АЦП
		(void) RCC->MP_AHB2LPENSETR;

	#elif CPUSTYLE_STM32H7XX
		RCC->AHB1ENR |= RCC_AHB1ENR_ADC12EN;	// Затактировали АЦП
		(void) RCC->AHB1ENR;
		RCC->AHB4ENR |= RCC_AHB4ENR_ADC3EN;		// Затактировали АЦП
		(void) RCC->AHB4ENR;
		#if WITHREFSENSOR
			RCC->APB4ENR |= RCC_APB4ENR_VREFEN;		// Затактировали источник опорного напрядения (для нормирования значений с АЦП)
			(void) RCC->AHB4ENR;
		#endif /* WITHREFSENSOR */

	#endif

	// расчет делителя для тактирования АЦП
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(BOARD_ADC_FREQ, ADC_FREQ), 0, (512 | 256 | 128 | 64 | 32 | 16 | 8 | 4 | 2), & value, 0);
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
	//PRINTF(PSTR("hardware_adc_initialize: prei=%u, presc=0x%02X\n"), prei, presc [prei]);

	// Power-on ADCs
	for (i = 0; i < board_get_adcinputs(); ++ i)
	{
		const uint_fast8_t adci = board_get_adcch(i);
		if (! isadchw(adci))
			continue;
		const adcinmap_t * const adcmap = getadcmap(adci);
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
		const uint_fast8_t adci = board_get_adcch(i);
		if (! isadchw(adci))
			continue;
		const adcinmap_t * const adcmap = getadcmap(adci);
		ADC_TypeDef * const adc = adcmap->adc;


		adcmap->adccommon->CCR =
			(adcmap->adccommon->CCR & ~ (ADC_CCR_PRESC | ADC_CCR_CKMODE |
#if WITHREFSENSOR
					ADC_CCR_VREFEN |
#endif /* WITHREFSENSOR */
#if WITHTEMPSENSOR
					ADC_CCR_TSEN |
#endif /* WITHTEMPSENSOR */
					0)) |
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

		//PRINTF(PSTR("ch=%u, time=%u, cycles=%u, tcode=%u\n"), adcmap->ch, adcmap->thold_uS01, cycles, tcode);
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

		adc->IER = ADC_IER_EOSIE;		// EOS (end of regular sequence) flag
	}

	// Set ADC_CR_ADEN
	for (i = 0; i < board_get_adcinputs(); ++ i)
	{
		const uint_fast8_t adci = board_get_adcch(i);
		if (! isadchw(adci))
			continue;
		const adcinmap_t * const adcmap = getadcmap(adci);
		ADC_TypeDef * const adc = adcmap->adc;

		if ((adc->CR & ADC_CR_ADEN) != 0)
			continue;	// already enabled

		// Калибровка.
		adc->CR |= ADC_CR_ADCAL; // Запуск калибровки АЦП
		while ((adc->CR & ADC_CR_ADCAL) != 0)
			; // Ожидаем окончания калибровки

		// Enable
		adc->CR |= ADC_CR_ADEN;

		// Wait for  ADC_ISR_ADRDY
		while ((adc->ISR & ADC_ISR_ADRDY) == 0)
			;
	}

	// connect to interrupt
#if CPUSTYLE_STM32MP1
	arm_hardware_set_handler_system(ADC1_IRQn, ADC1_IRQHandler);
	arm_hardware_set_handler_system(ADC2_IRQn, ADC2_IRQHandler);
#elif CPUSTYLE_STM32H7XX
	arm_hardware_set_handler_system(ADC_IRQn, ADC_IRQHandler);
	arm_hardware_set_handler_system(ADC3_IRQn, ADC3_IRQHandler);
#endif /* CPUSTYLE_STM32H7XX */

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
	const uint_fast8_t prei = calcdivider(calcdivround2(BOARD_ADC_FREQ, ADC_FREQ), 0, (8 | 4 | 2), & value, 0);
	// STM32F767
	// STM32F429
	static const uint_fast8_t presc [] =
	{
		0x00,	// 00: PCLK2 divided by 2
		0x01,	// 01: PCLK2 divided by 4
		0x03,	// 11: PCLK2 divided by 8
	};
	const uint_fast32_t cycles = NTICKSADC01(10);	// в десятых долях микросекунды
	PRINTF(PSTR("hardware_adc_initialize: prei=%u, presc=0x%02X, cycles=%u\n"), prei, presc [prei], cycles);

	ADC->CCR = (ADC->CCR & ~ (ADC_CCR_ADCPRE | ADC_CCR_TSVREFE)) |
		presc [prei] * ADC_CCR_ADCPRE_0 |
	#if WITHREFSENSOR || WITHTEMPSENSOR
		ADC_CCR_TSVREFE |
	#endif /* WITHREFSENSOR || WITHTEMPSENSOR */
		0;

	NVIC_SetVector(ADC_IRQn, (uintptr_t) & ADC_IRQHandler);
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

	NVIC_SetVector(ADC1_2_IRQn, (uintptr_t) & ADC1_2_IRQHandler);
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
	//while ((ADC1->CR & ADC_CR_ADCAL) != 0)
	//	; // Ожидаем окончания калибровки

	//ADC2->CR |= ADC_CR_ADCAL; //Запуск калибровки АЦП
	//while ((ADC2->CR & ADC_CR_ADCAL) != 0)
	//	; // Ожидаем окончания калибровки

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
		arm_hardware_set_handler_system(ADC1_COMP_IRQn, & ADC1_COMP_IRQHandler);
	#else /* STM32F0XX_MD */
		arm_hardware_set_handler_system(ADC1_IRQn, & ADC1_IRQHandler);
	#endif /* STM32F0XX_MD */

	// первый запуск производится в hardware_adc_startonescan().
	// А здесь всё...

#elif CPUSTYLE_R7S721

	//#warning TODO: Add code for R7S721 ADC support

	CPG.STBCR6 &= ~ (1U << CPG_STBCR6_MSTP67_SHIFT);	// Module Stop 67 0: The A/D converter runs.
	(void) CPG.STBCR6;			/* Dummy read */

	const uint_fast32_t ainmask = build_adc_mask();

	ADC.ADCSR =
		1 * (1uL << ADC_SR_ADIE_SHIFT) |	// ADIE - 1: A/D conversion end interrupt (ADI) request is enabled
		3 * (1uL << ADC_SR_CKS_SHIFT) |	// CKS[2..0] - Clock Select - 011: Conversion time = 382 tcyc (maximum)
		0;

	HARDWARE_ADC_INITIALIZE(ainmask);

	// connect to interrupt
	arm_hardware_set_handler_system(ADI_IRQn, r7s721_adi_irq_handler);

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

		arm_hardware_set_handler_system(ADC1_COMP_IRQn, & ADC1_COMP_IRQHandler);
	#endif

	// первый запуск производится в hardware_adc_startonescan().
	// А здесь всё...

#else
	#warning Undefined CPUSTYLE_XXX
#endif

	//PRINTF(PSTR("hardware_adc_initialize done\n"));
}

#endif /* WITHCPUADCHW */

#if WITHLFM

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
	NVIC_SetVector(TC0_IRQn, (uintptr_t) & TC0_IRQHandler);
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
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, ticksfreq), ATSAM3S_TIMER_WIDTH, ATSAM3S_TIMER_TAPS, & value, 1);
	TC0->TC_CHANNEL [0].TC_CMR =
		(TC0->TC_CHANNEL [0].TC_CMR & ~ TC_CMR_TCCLKS_Msk) | tc_cmr_tcclks [prei];
	TC0->TC_CHANNEL [0].TC_RC = value;	// программирование полного периода

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, ticksfreq), AT91SAM7_TIMER_WIDTH, AT91SAM7_TIMER_TAPS, & value, 1);
	AT91C_BASE_TCB->TCB_TC0.TC_CMR =
		(AT91C_BASE_TCB->TCB_TC0.TC_CMR & ~ AT91C_TC_CLKS) | tc_cmr_clks [prei];
	AT91C_BASE_TCB->TCB_TC0.TC_RC = value;	// программирование полного периода

#endif
}
#endif /* WITHLFM */


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
	#warning Undefined CPUSTYLE_XXX
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
	// TCCR2B - Timer/Counter Control Register B
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
	// TCCR2B - Timer/Counter Control Register B
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
	#warning Undefined CPUSTYLE_XXX
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
	#warning Undefined CPUSTYLE_XXX
#endif

}

// return code: prescaler
uint_fast8_t
hardware_calc_sound_params(
	uint_least16_t tonefreq,	/* tonefreq - частота в десятых долях герца. Минимум - 400 герц (определено набором команд CAT). */
	unsigned * pvalue)
{

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	return calcdivider(calcdivround2(10UL * CPU_FREQ, tonefreq * 2), ATSAM3S_TIMER_WIDTH, ATSAM3S_TIMER_TAPS, pvalue, 1);

#elif CPUSTYLE_AT91SAM7S

	return calcdivider(calcdivround2(10UL * CPU_FREQ, tonefreq * 2), AT91SAM7_TIMER_WIDTH, AT91SAM7_TIMER_TAPS, pvalue, 1);

#elif CPUSTYLE_ATMEGA328
	//
	// compare match после записи делителя отменяется на один цикл
	// timer0 - 8 bit wide.
	// генерация сигнала самоконтроля на PD6(OC0A) - выход делителя на 2
	return calcdivider(calcdivround2(10UL * CPU_FREQ, tonefreq * 2), ATMEGA_TIMER0_WIDTH, ATMEGA_TIMER0_TAPS, pvalue, 1);

#elif CPUSTYLE_ATMEGA_XXX4
	//
	// timer2 - 8 bit wide.
	// генерация сигнала самоконтроля на PD7(OC2) - выход делителя на 2
	// Пототму в расчёте используется tonefreq * 2
	return calcdivider(calcdivround2(10UL * CPU_FREQ, tonefreq * 2), ATMEGA_TIMER2_WIDTH, ATMEGA_TIMER2_TAPS, pvalue, 1);

#elif CPUSTYLE_ATMEGA128
	// ATMega128/ATMega64

	//
	// timer2 - 8 bit wide.
	// генерация сигнала самоконтроля на PD7(OC2) - выход делителя на 2
	// Пототму в расчёте используется tonefreq * 2
	return calcdivider(calcdivround2(10UL * CPU_FREQ, tonefreq * 2), ATMEGA_TIMER2_WIDTH, ATMEGA_TIMER2_TAPS, pvalue, 1);

#elif CPUSTYLE_ATMEGA32

	//
	// timer2 - 8 bit wide.
	// генерация сигнала самоконтроля на PD7(OC2) - выход делителя на 2
	// Пототму в расчёте используется tonefreq * 2
	return calcdivider(calcdivround2(10UL * CPU_FREQ, tonefreq * 2), ATMEGA_TIMER2_WIDTH, ATMEGA_TIMER2_TAPS, pvalue, 1);

#elif CPUSTYLE_ATXMEGAXXXA4

	return calcdivider(calcdivround2(10UL * CPU_FREQ, tonefreq * 2), ATXMEGA_TIMER_WIDTH, ATXMEGA_TIMER_TAPS, pvalue, 1);

#elif CPUSTYLE_STM32F

	// for tim1 use apb2, for other apb1
	// now - tim4
	// TIM4 - 16-bit timer
	return calcdivider(calcdivround2(10UL * PCLK2_FREQ, tonefreq), STM32F_TIM4_TIMER_WIDTH, STM32F_TIM4_TIMER_TAPS, pvalue, 1);

#else
	#warning Undefined CPUSTYLE_XXX
#endif
}

#else /* SIDETONE_TARGET_BIT != 0 */

void
hardware_beep_initialize(void)
{
}

#endif /* SIDETONE_TARGET_BIT != 0 */

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

	TC2->TC_CHANNEL [2].TC_CMR =
	                (TC_CMR_CLKI * 0) |
	                TC_CMR_BURST_NONE |
	                TC_CMR_WAVSEL_UP_RC | /* TC_CMR_WAVESEL_UP_AUTO */
	                TC_CMR_WAVE |
					TC_CMR_TCCLKS_TIMER_CLOCK5 |	// TC0CLK = MCLK/32
					0
				;

	TC2->TC_CHANNEL [2].TC_CCR = TC_CCR_SWTRG | TC_CCR_CLKEN; // reset and enable TC2 clock

	TC2->TC_CHANNEL [2].TC_IER = TC_IER_CPCS ; // Interrupt on RC compare

	// enable interrupts from TC2
	NVIC_SetVector(TC2_IRQn, (uintptr_t) & TC2_IRQHandler);
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
	(void) RCC->APB1LENR;
	TIM3->DIER = TIM_DIER_UIE;        	 // разрешить событие от таймера

	arm_hardware_set_handler_system(TIM3_IRQn, TIM3_IRQHandler);

#elif CPUSTYLE_STM32F

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;   // подаем тактирование на TIM3
	(void) RCC->APB1ENR;
	TIM3->DIER = TIM_DIER_UIE;        	 // разрешить событие от таймера

	arm_hardware_set_handler_system(TIM3_IRQn, TIM3_IRQHandler);

#elif CPUSTYLE_R7S721

	// OSTM1
    /* ==== Module standby clear ==== */
    /* ---- Supply clock to the OSTM(channel 1) ---- */
	CPG.STBCR5 &= ~ CPG_STBCR5_MSTP50;	// Module Stop 50 0: Channel 1 of the OS timer runs.
	(void) CPG.STBCR5;			/* Dummy read */

    /* ---- OSTM count stop trigger register (TT) setting ---- */
    OSTM1.OSTMnTT = 0x01u;      /* Stop counting */

    arm_hardware_set_handler_system(OSTMI1TINT_IRQn, OSTMI1TINT_IRQHandler);

	OSTM1.OSTMnTS = 0x01u;      /* Start counting */

#elif CPUSTYLE_STM32MP1

	RCC->MP_APB1ENSETR = RCC_MP_APB1ENSETR_TIM3EN;   // подаем тактирование на TIM3
	(void) RCC->MP_APB1ENSETR;
	RCC->MP_APB1LPENSETR = RCC_MP_APB1LPENSETR_TIM3LPEN;   // подаем тактирование на TIM3
	(void) RCC->MP_APB1LPENSETR;

	TIM3->DIER = TIM_DIER_UIE;        	 // разрешить событие от таймера

	arm_hardware_set_handler_system(TIM3_IRQn, TIM3_IRQHandler);

#else
	#warning Undefined CPUSTYLE_XXX
#endif
}

void hardware_elkey_set_speed(uint_fast32_t ticksfreq)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, ticksfreq), ATSAM3S_TIMER_WIDTH, ATSAM3S_TIMER_TAPS, & value, 1);
	TC2->TC_CHANNEL [2].TC_CMR =
		(TC0->TC_CHANNEL [2].TC_CMR & ~ TC_CMR_TCCLKS_Msk) | tc_cmr_tcclks [prei];
	TC2->TC_CHANNEL [2].TC_RC = value;	// программирование полного периода

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, ticksfreq), AT91SAM7_TIMER_WIDTH, AT91SAM7_TIMER_TAPS, & value, 1);
	AT91C_BASE_TCB->TCB_TC2.TC_CMR =
		(AT91C_BASE_TCB->TCB_TC2.TC_CMR & ~ AT91C_TC_CLKS) | tc_cmr_clks [prei];
	AT91C_BASE_TCB->TCB_TC2.TC_RC = value;	// программирование полного периода

#elif CPUSTYLE_ATMEGA

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, ticksfreq), ATMEGA_TIMER1_WIDTH, ATMEGA_TIMER1_TAPS, & value, 1);
	// WGM12 = WGMn2 bit in timer 1
	// (1U << WGM12) - mode4: CTC
	TCCR1B = (1U << WGM12) | (prei + 1);	// прескалер
	OCR1A = value;	// делитель - программирование полного периода

#elif CPUSTYLE_ATXMEGA

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, ticksfreq), ATXMEGA_TIMER_WIDTH, ATXMEGA_TIMER_TAPS, & value, 1);

	// программирование таймера
	TCC1.CCA = value;	// timer/counter C1, compare register A, see TCC1_CCA_vect
	TCC1.CTRLA = (prei + 1);
	TCC1.CTRLB = (TC_WGMODE_FRQ_gc);
	TCC1.INTCTRLB = (TC_CCAINTLVL_MED_gc);
	// разрешение прерываний на входе в PMIC
	PMIC.CTRL |= (PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm);

#elif CPUSTYLE_STM32MP1
	// TIM2 & TIM5 on CPUSTYLE_STM32F4XX have 32-bit CNT and ARR registers
	// TIM7 located on APB1
	// TIM7 on APB1
	// Use basic timer
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(BOARD_TIM3_FREQ, ticksfreq), STM32F_TIM3_TIMER_WIDTH, STM32F_TIM3_TIMER_TAPS, & value, 1);

	TIM3->PSC = ((1UL << prei) - 1) & TIM_PSC_PSC;
	TIM3->ARR = value;
	TIM3->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости - если счётчик успевал превысить значение ARR - считал до конца */


#elif CPUSTYLE_STM32H7XX
	// TIM2 & TIM5 on CPUSTYLE_STM32F4XX have 32-bit CNT and ARR registers
	// TIM7 located on APB1
	// TIM7 on APB1
	// Use basic timer
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(BOARD_TIM3_FREQ, ticksfreq), STM32F_TIM3_TIMER_WIDTH, STM32F_TIM3_TIMER_TAPS, & value, 1);

	TIM3->PSC = ((1UL << prei) - 1) & TIM_PSC_PSC;
	TIM3->ARR = value;
	TIM3->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости - если счётчик успевал превысить значение ARR - считал до конца */

#elif CPUSTYLE_STM32F
	// TIM2 & TIM5 on CPUSTYLE_STM32F4XX have 32-bit CNT and ARR registers
	// TIM7 located on APB1
	// TIM7 on APB1
	// Use basic timer
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(BOARD_TIM3_FREQ, ticksfreq), STM32F_TIM3_TIMER_WIDTH, STM32F_TIM3_TIMER_TAPS, & value, 1);

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
	#warning Undefined CPUSTYLE_XXX
#endif
}


void hardware_elkey_set_speed128(uint_fast32_t ticksfreq, int scale)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(scale * calcdivround2(CPU_FREQ, ticksfreq), ATSAM3S_TIMER_WIDTH, ATSAM3S_TIMER_TAPS, & value, 1);
	TC0->TC_CHANNEL [2].TC_CMR =
		(TC0->TC_CHANNEL [2].TC_CMR & ~ TC_CMR_TCCLKS_Msk) | tc_cmr_tcclks [prei];
	TC0->TC_CHANNEL [2].TC_RC = value;	// программирование полного периода

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(scale * calcdivround2(CPU_FREQ, ticksfreq), AT91SAM7_TIMER_WIDTH, AT91SAM7_TIMER_TAPS, & value, 1);
	AT91C_BASE_TCB->TCB_TC2.TC_CMR =
		(AT91C_BASE_TCB->TCB_TC2.TC_CMR & ~ AT91C_TC_CLKS) | tc_cmr_clks [prei];
	AT91C_BASE_TCB->TCB_TC2.TC_RC = value;	// программирование полного периода

#elif CPUSTYLE_ATMEGA

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(scale * calcdivround2(CPU_FREQ, ticksfreq), ATMEGA_TIMER1_WIDTH, ATMEGA_TIMER1_TAPS, & value, 1);
	// WGM12 = WGMn2 bit in timer 1
	// (1U << WGM12) - mode4: CTC
	TCCR1B = (1U << WGM12) | (prei + 1);	// прескалер
	OCR1A = value;	// делитель - программирование полного периода

#elif CPUSTYLE_ATXMEGA

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(scale * calcdivround2(CPU_FREQ, ticksfreq), ATXMEGA_TIMER_WIDTH, ATXMEGA_TIMER_TAPS, & value, 1);

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
	const uint_fast8_t prei = calcdivider(scale * calcdivround2(BOARD_TIM3_FREQ, ticksfreq), STM32F_TIM3_TIMER_WIDTH, STM32F_TIM3_TIMER_TAPS, & value, 1);

	TIM3->PSC = ((1UL << prei) - 1) & TIM_PSC_PSC;
	TIM3->ARR = value;
	TIM3->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости - если счётчик успевал превысить значение ARR - считал до конца */

#elif CPUSTYLE_STM32F
	// TIM2 & TIM5 on CPUSTYLE_STM32F4XX have 32-bit CNT and ARR registers
	// TIM7 located on APB1
	// TIM7 on APB1
	// Use basic timer
	unsigned value;
	const uint_fast8_t prei = calcdivider(scale * calcdivround2(BOARD_TIM3_FREQ, ticksfreq), STM32F_TIM3_TIMER_WIDTH, STM32F_TIM3_TIMER_TAPS, & value, 1);

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

#elif CPUSTYLE_STM32MP1
	// TIM2 & TIM5 on CPUSTYLE_STM32F4XX have 32-bit CNT and ARR registers
	// TIM7 located on APB1
	// TIM7 on APB1
	// Use basic timer
	unsigned value;
	const uint_fast8_t prei = calcdivider(scale * calcdivround2(BOARD_TIM3_FREQ, ticksfreq), STM32F_TIM3_TIMER_WIDTH, STM32F_TIM3_TIMER_TAPS, & value, 1);

	TIM3->PSC = ((1UL << prei) - 1) & TIM_PSC_PSC;
	TIM3->ARR = value;
	TIM3->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости - если счётчик успевал превысить значение ARR - считал до конца */

#else
	#warning Undefined CPUSTYLE_XXX
#endif
}

#endif /* WITHELKEY */

#if WITHSDHCHW

void hardware_sdhost_setbuswidth(uint_fast8_t use4bit)
{
	//PRINTF(PSTR("hardware_sdhost_setbuswidth: use4bit=%u\n"), (unsigned) use4bit);

#if ! WITHSDHCHW4BIT
	use4bit = 0;
#endif /* ! WITHSDHCHW4BIT */

#if CPUSTYLE_R7S721

	if (use4bit != 0)
		SDHI0.SD_OPTION &= ~ (1U << 15);	// WIDTH 0: 4-bit width
	else
		SDHI0.SD_OPTION |= (1U << 15);		// WIDTH 1: 1-bit width

#elif CPUSTYLE_STM32F4XX

	SDIO->CLKCR = (SDIO->CLKCR & ~ (SDIO_CLKCR_WIDBUS)) |
		(use4bit != 0 ? 0x01 : 0x00) * SDIO_CLKCR_WIDBUS_0 |	// 01: 4-wide bus mode: SDMMC_D[3:0] used
		0;

#elif CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1

	SDMMC1->CLKCR = (SDMMC1->CLKCR & ~ (SDMMC_CLKCR_WIDBUS)) |
		(use4bit != 0 ? 0x01 : 0x00) * SDMMC_CLKCR_WIDBUS_0 |	// 01: 4-wide bus mode: SDMMC_D[3:0] used
		0;

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	SD0->HOST_CTRL_BLOCK_GAP_CTRL = (SD0->HOST_CTRL_BLOCK_GAP_CTRL & ~ (0x02uL)) |
				(use4bit != 0) * 0x02uL |	// Data_Transfer_Width_SD1_or_SD4
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
	PRINTF(PSTR("hardware_sdhost_setspeed: ticksfreq=%lu, prei=%lu\n"), (unsigned long) ticksfreq, (unsigned long) prei);

	while ((SDHI0.SD_INFO2 & (1uL << 13)) == 0)	// SCLKDIVEN
	{
		//PRINTF(PSTR("hardware_sdhost_setspeed: SCLKDIVEN set clock prohibited, SD_INFO2=%08lX\n"), SDHI0.SD_INFO2);
		TP();
	}
	while ((SDHI0.SD_INFO2 & (1uL << 14)) != 0)	// CBSY
	{
		//PRINTF(PSTR("hardware_sdhost_setspeed: CBSY set clock prohibited, SD_INFO2=%08lX\n"), SDHI0.SD_INFO2);
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

	const uint_fast32_t stm32f4xx_pllq = stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
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

	const uint_fast32_t stm32f7xx_pllq = stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
	// Использование автоматического расчёта делителя
	// PLLQ: Main PLL (PLL) division factor for USB OTG FS, SDIO and random number generator clocks
	// Should be 48 MHz or less for SDIO and 48 MHz with small tolerance.
	// See RCC_PLLCFGR_PLLQ usage
	const uint32_t SDMMCCLK = PLL_FREQ / stm32f7xx_pllq;
	// Использование автоматического расчёта делителя
	// Источником тактирования SDMMC сейчас установлен внутренний генератор 48 МГц
	//const uint32_t stm32f4xx_48mhz = PLL_FREQ / stm32f7xx_pllq;
	const unsigned value = ulmin(calcdivround2(SDMMCCLK, ticksfreq) - 2, 255);

	//PRINTF(PSTR("hardware_sdhost_setspeed: stm32f7xx_pllq=%lu, freq=%lu\n"), (unsigned long) stm32f7xx_pllq, stm32f4xx_48mhz);
	PRINTF(PSTR("hardware_sdhost_setspeed: CLKCR_CLKDIV=%lu\n"), (unsigned long) value);

	SDMMC1->CLKCR = (SDMMC1->CLKCR & ~ (SDMMC_CLKCR_CLKDIV)) |
		(value & SDMMC_CLKCR_CLKDIV);

#elif CPUSTYLE_STM32H7XX

	//RCC->DCKCFGR2 = (RCC->DCKCFGR2 & ~ (RCC_DCKCFGR2_SDMMC1SEL)) |
	//	0 * RCC_DCKCFGR2_SDMMC1SEL |	// 0: 48 MHz clock is selected as SDMMC clock
	//	0;

	const uint_fast32_t stm32h7xx_pllq = stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
	// Использование автоматического расчёта делителя
	// PLLQ: Main PLL (PLL) division factor for USB OTG FS, SDIO and random number generator clocks
	// Should be 48 MHz or less for SDIO and 48 MHz with small tolerance.
	// See RCC_PLLCFGR_PLLQ usage
	const uint32_t SDMMCCLK = PLL_FREQ / stm32h7xx_pllq;;
	// Использование автоматического расчёта делителя
	// Источником тактирования SDMMC сейчас установлен внутренний генератор 48 МГц
	//const uint32_t stm32f4xx_48mhz = PLL_FREQ / stm32h7xx_pllq;
	const unsigned value = ulmin(calcdivround2(SDMMCCLK / 2, ticksfreq), 0x03FF);

	PRINTF(PSTR("hardware_sdhost_setspeed: stm32h7xx_pllq=%lu, SDMMCCLK=%lu, PLL_FREQ=%lu\n"), (unsigned long) stm32h7xx_pllq, SDMMCCLK, PLL_FREQ);
	PRINTF(PSTR("hardware_sdhost_setspeed: CLKCR_CLKDIV=%lu\n"), (unsigned long) value);

	SDMMC1->CLKCR = (SDMMC1->CLKCR & ~ (SDMMC_CLKCR_CLKDIV_Msk)) |
		((value << SDMMC_CLKCR_CLKDIV_Pos) & SDMMC_CLKCR_CLKDIV_Msk);



#elif CPUSTYLE_STM32MP1

	//RCC->DCKCFGR2 = (RCC->DCKCFGR2 & ~ (RCC_DCKCFGR2_SDMMC1SEL)) |
	//	0 * RCC_DCKCFGR2_SDMMC1SEL |	// 0: 48 MHz clock is selected as SDMMC clock
	//	0;

	//const uint_fast32_t stm32h7xx_pllq = stm32f7xx_pllq_initialize();	// Настроить выход PLLQ на 48 МГц
	// Использование автоматического расчёта делителя
	// PLLQ: Main PLL (PLL) division factor for USB OTG FS, SDIO and random number generator clocks
	// Should be 48 MHz or less for SDIO and 48 MHz with small tolerance.
	// See RCC_PLLCFGR_PLLQ usage
	const uint32_t SDMMCCLK = stm32mp1_sdmmc1_2_get_freq();
	// Использование автоматического расчёта делителя
	// Источником тактирования SDMMC сейчас установлен внутренний генератор 48 МГц
	//const uint32_t stm32f4xx_48mhz = PLL_FREQ / stm32h7xx_pllq;
	const unsigned clkdiv = ulmin(calcdivround2(SDMMCCLK / 2, ticksfreq), 0x03FF);

	//PRINTF(PSTR("hardware_sdhost_setspeed: stm32h7xx_pllq=%lu, SDMMCCLK=%lu, PLL_FREQ=%lu\n"), (unsigned long) stm32h7xx_pllq, SDMMCCLK, PLL_FREQ);
	//PRINTF(PSTR("hardware_sdhost_setspeed: CLKCR_CLKDIV=%lu\n"), (unsigned long) value);

	SDMMC1->CLKCR = (SDMMC1->CLKCR & ~ (SDMMC_CLKCR_CLKDIV_Msk)) |
		((clkdiv << SDMMC_CLKCR_CLKDIV_Pos) & SDMMC_CLKCR_CLKDIV_Msk);


#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	const unsigned long ref = xc7z_get_sdio_freq();
	unsigned divider = calcdivround2(ref / 2, ticksfreq);
	divider = ulmin(divider, 255);
	divider = ulmax(divider, 1);
	PRINTF("hardware_sdhost_setspeed: ref=%lu, divider=%u\n", ref, divider);
	if (ticksfreq <= 400000uL)
	{
		SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL =
			(SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL & ~ (0x00FF00uL)) |
			(0x80uL << 8) |	// SDCLK_Frequency_Select: 80h - base clock divided by 256
			0;
	}
	else
	{
		SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL =
			(SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL & ~ (0x00FF00uL)) |
			((uint_fast32_t) divider << 8) |	// SDCLK_Frequency_Select: 10h - base clock divided by 32
			0;
	}

	SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL |= 0x01;	// Internal_Clock_Enable
	// Wait Internal_Clock_Stable
	while ((SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL & 0x02) == 0)
		;

#else
	#error Wrong CPUSTYLE_xxx
#endif
}

#if CPUSTYLE_R7S721

void r7s721_sdhi0_dma_handler(void)
{
	PRINTF(PSTR("r7s721_sdhi0_dma_handler trapped\n"));
	for (;;)
		;
}

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

    arm_hardware_set_handler_system(DMAINT14_IRQn, r7s721_sdhi0_dma_handler);

	HARDWARE_SDIO_INITIALIZE();	// Подсоединить контроллер к выводам процессора

#elif CPUSTYLE_STM32F4XX

	RCC->APB2ENR |= RCC_APB2ENR_SDIOEN;   // подаем тактирование на SDIO
	__DSB();

	// hwrdware flow control отключен - от этого зависит проверка состояния txfifo & rxfifo
	SDIO->CLKCR =
		1 * SDIO_CLKCR_CLKEN |
		(255 & SDIO_CLKCR_CLKDIV_Msk) |
		1 * SDIO_CLKCR_HWFC_EN |
		1 * SDIO_CLKCR_PWRSAV |		// выключается clock без обращений
		0;

	hardware_sdhost_setbuswidth(0);
	hardware_sdhost_setspeed(400000uL);

	arm_hardware_set_handler_system(SDIO_IRQn, SDIO_IRQHandler);
	arm_hardware_set_handler_system(DMA2_Stream6_IRQn, DMA2_Stream6_IRQHandler);

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
		1 * SDMMC_CLKCR_HWFC_EN |
		1 * SDMMC_CLKCR_PWRSAV |		// выключается clock без обращений
		0;

	hardware_sdhost_setbuswidth(0);
	hardware_sdhost_setspeed(400000uL);

	arm_hardware_set_handler_system(SDMMC1_IRQn, SDMMC1_IRQHandler);
	arm_hardware_set_handler_system(DMA2_Stream6_IRQn, DMA2_Stream6_IRQHandler);

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
		1 * SDMMC_CLKCR_HWFC_EN |
		1 * SDMMC_CLKCR_PWRSAV |		// выключается clock без обращений
		0;

	hardware_sdhost_setbuswidth(0);
	hardware_sdhost_setspeed(400000uL);

	arm_hardware_set_handler_system(SDMMC1_IRQn, SDMMC1_IRQHandler);

	// разрешить тактирование карты памяти
	SDMMC1->POWER = 3 * SDMMC_POWER_PWRCTRL_0;

#elif CPUSTYLE_STM32MP1

	//	0x0: hclk6 clock selected as kernel peripheral clock
	//	0x1: pll3_r_ck clock selected as kernel peripheral clock
	//	0x2: pll4_p_ck clock selected as kernel peripheral clock
	//	0x3: hsi_ker_ck clock selected as kernel peripheral clock (default after reset)

	RCC->SDMMC12CKSELR = (RCC->SDMMC12CKSELR & ~ (RCC_SDMMC12CKSELR_SDMMC12SRC_Msk)) |
			(0x03uL << RCC_SDMMC12CKSELR_SDMMC12SRC_Pos) |	// hsi_ker_ck
			0;
	(void) RCC->SDMMC12CKSELR;

	RCC->MP_AHB6ENSETR = RCC_MP_AHB6ENSETR_SDMMC1EN;   // подаем тактирование на SDMMC1
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6LPENSETR = RCC_MP_AHB6LPENSETR_SDMMC1LPEN;   // подаем тактирование на SDMMC1
	(void) RCC->MP_AHB6LPENSETR;

	HARDWARE_SDIO_INITIALIZE();	// Подсоединить контроллер к выводам процессора

	// hwrdware flow control отключен - от этого зависит проверка состояния txfifo & rxfifo
	SDMMC1->CLKCR =
		SDMMC_CLKCR_CLKDIV |
		1 * SDMMC_CLKCR_HWFC_EN |
		1 * SDMMC_CLKCR_PWRSAV |		// выключается clock без обращений
		0;

	hardware_sdhost_setbuswidth(0);
	hardware_sdhost_setspeed(400000uL);

	arm_hardware_set_handler_system(SDMMC1_IRQn, SDMMC1_IRQHandler);

	// разрешить тактирование карты памяти
	SDMMC1->POWER = 3 * SDMMC_POWER_PWRCTRL_0;

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	const unsigned sdioix = 0;	// SD0

//    SCLR->SDIO_RST_CTRL |= (0x11uL << sdioix);
//    SCLR->SDIO_RST_CTRL &= ~ (0x11uL << sdioix);

	SCLR->SLCR_UNLOCK = 0x0000DF0DU;
	SCLR->APER_CLK_CTRL |= (0x01uL << (10 + sdioix));	// APER_CLK_CTRL.SDI0_CPU_1XCLKACT
    //EMIT_MASKWRITE(0XF8000150, 0x00003F33U ,0x00001001U),	// SDIO_CLK_CTRL
	SCLR->SDIO_CLK_CTRL = (SCLR->SDIO_CLK_CTRL & ~ (0x00003F33U)) |
		((uint_fast32_t) SCLR_SDIO_CLK_CTRL_DIVISOR_VALUE << 8) | // DIVISOR
		(0x00uL << 4) |	// SRCSEL - 0x: IO PLL
		(0x01uL << sdioix) | // CLKACT0 - SDIO 0 reference clock active
		0;


	SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL =
		(SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL & ~ ((0x0FuL << 16))) |
		(0x0EuL << 16);	// Data_Timeout_Counter_Value

	// SD_Bus_Power off
	SD0->HOST_CTRL_BLOCK_GAP_CTRL =
			(SD0->HOST_CTRL_BLOCK_GAP_CTRL & ~ (0x01uL << 8)) |
			0 * (0x01uL << 8) |	// 0 - Power off
			0;
	// SD_Bus_Voltage_Select
	SD0->HOST_CTRL_BLOCK_GAP_CTRL =
			(SD0->HOST_CTRL_BLOCK_GAP_CTRL & ~ (0x07uL << 9)) |
			(0x07uL << 9) |	// 111b - 3.3 Flattop
			0;
	// SD_Bus_Power on
	SD0->HOST_CTRL_BLOCK_GAP_CTRL =
			(SD0->HOST_CTRL_BLOCK_GAP_CTRL & ~ (0x01uL << 8)) |
			1 * (0x01uL << 8) |	// 1 - Power on
			0;
	// DMA_Select
	SD0->HOST_CTRL_BLOCK_GAP_CTRL =
			(SD0->HOST_CTRL_BLOCK_GAP_CTRL & ~ (0x03uL << 3)) |
			(0x00uL << 3) |	// SDMA select
			0;

	HARDWARE_SDIO_INITIALIZE();	// Подсоединить контроллер к выводам процессора
	ASSERT(((SD0->Vendor_Version_Number & 0xFFFF0000uL) >> 16) == 0x8901uL);

	hardware_sdhost_setbuswidth(0);
	hardware_sdhost_setspeed(400000uL);

//	PRINTF("SD0->CAPABILITIES=%08lX\n", SD0->CAPABILITIES);
//	PRINTF("SD0->CAPABILITIES.SDMA_Support=%d\n", (SD0->CAPABILITIES >> 22) & 0x01);
//	PRINTF("SD0->CAPABILITIES.Voltage_Support_3_3_V=%d\n", (SD0->CAPABILITIES >> 24) & 0x01);

	//arm_hardware_set_handler_system(SDIO0_IRQn, SDIO0_IRQHandler);

#else

	#error Wrong CPUSTYLE_xxx

#endif
}

// в ответ на прерывание изменения состояния card detect
void hardware_sdhost_detect(uint_fast8_t Card_Inserted)
{
#if CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	//	This bit indicates whether a card has been
	//	inserted. Changing from 0 to 1 generates a Card
	//	Insertion interrupt in the Normal Interrupt Status
	//	register and changing from 1 to 0 generates a
	//	Card Removal Interrupt in the Normal Interrupt
	//	Status register. The Software Reset For All in the
	//	Software Reset register shall not affect this bit. If
	//	a Card is removed while its power is on and its
	//	clock is oscillating, the HC shall clear SD Bus
	//	Power in the Power Control register and SD Clock
	//	Enable in the Clock control register. In addition
	//	the HD should clear the HC by the Software Reset
	//	For All in Software register. The card detect is
	//	active regardless of the SD Bus Power.

	return;

	// SD_Bus_Power off
	SD0->HOST_CTRL_BLOCK_GAP_CTRL =
			(SD0->HOST_CTRL_BLOCK_GAP_CTRL & ~ (0x01uL << 8)) |
			0 * (0x01uL << 8) |	// 0 - Power off
			0;

	SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL |= (1uL << 24);	// Software_Reset_for_All
		PRINTF("SD0->CAPABILITIES=%08lX\n", SD0->CAPABILITIES);
		PRINTF("SD0->CAPABILITIES=%08lX\n", SD0->CAPABILITIES);
		PRINTF("SD0->CAPABILITIES=%08lX\n", SD0->CAPABILITIES);
		PRINTF("SD0->CAPABILITIES=%08lX\n", SD0->CAPABILITIES);
	SD0->TIMEOUT_CTRL_SW_RESET_CLOCK_CTRL &= ~ (1uL << 24);	// Software_Reset_for_All

	// SD_Bus_Voltage_Select
	SD0->HOST_CTRL_BLOCK_GAP_CTRL =
			(SD0->HOST_CTRL_BLOCK_GAP_CTRL & ~ (0x07uL << 9)) |
			(0x07uL << 9) |	// 111b - 3.3 Flattop
			0;
	// SD_Bus_Power on
	SD0->HOST_CTRL_BLOCK_GAP_CTRL =
			(SD0->HOST_CTRL_BLOCK_GAP_CTRL & ~ (0x01uL << 8)) |
			1 * (0x01uL << 8) |	// 1 - Power on
			0;


    //hardware_sdhost_initialize();

 #endif
}

#endif /* WITHSDHCHW */


#if WITHUART1HW

void
hardware_uart1_set_speed(uint_fast32_t baudrate)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		// Использование автоматического расчёта предделителя
		unsigned value;
		const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_USART_BRGR_WIDTH, ATSAM3S_USART_BRGR_TAPS, & value, 0);
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
		const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_USART_BRGR_WIDTH, ATSAM3S_USART_BRGR_TAPS, & value, 0);
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
		calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_UART_BRGR_WIDTH, ATSAM3S_UART_BRGR_TAPS, & value, 0);
		UART0->UART_BRGR = value;
	#elif HARDWARE_ARM_USEUART1
		// Использование автоматического расчёта предделителя
		unsigned value;
		calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_UART_BRGR_WIDTH, ATSAM3S_UART_BRGR_TAPS, & value, 0);
		UART1->UART_BRGR = value;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), AT91SAM7_USART_BRGR_WIDTH, AT91SAM7_USART_BRGR_TAPS, & value, 0);

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
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR0A |= (1U << U2X0);
	else
		UCSR0A &= ~ (1U << U2X0);

	UBRR0 = value;	/* Значение получено уже уменьшенное на 1 */


#elif CPUSTYLE_ATMEGA128

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR0A |= (1U << U2X0);
	else
		UCSR0A &= ~ (1U << U2X0);

	UBRR0H = (value >> 8) & 0xff;	/* Значение получено уже уменьшенное на 1 */
	UBRR0L = value & 0xff;

#elif CPUSTYLE_ATMEGA

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSRA |= (1U << U2X);
	else
		UCSRA &= ~ (1U << U2X);

	UBRRH = (value >> 8) & 0xff;	/* Значение получено уже уменьшенное на 1 */
	UBRRL = value & 0xff;

#elif CPUSTYLE_ATXMEGAXXXA4

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATXMEGA_UBR_WIDTH, ATXMEGA_UBR_TAPS, & value, 1);
	if (prei == 0)
		USARTE0.CTRLB |= USART_CLK2X_bm;
	else
		USARTE0.CTRLB &= ~USART_CLK2X_bm;
	// todo: проверить требование к порядку обращения к портам
	USARTE0.BAUDCTRLA = (value & 0xff);	/* Значение получено уже уменьшенное на 1 */
	USARTE0.BAUDCTRLB = (ATXMEGA_UBR_BSEL << 4) | ((value >> 8) & 0x0f);

#elif CPUSTYLE_STM32MP1

	// usart1
	USART1->BRR = calcdivround2(BOARD_USART1_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_STM32F

	// uart1 on apb2 up to 72/36 MHz

	USART1->BRR = calcdivround2(BOARD_USART1_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

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

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	  uint32_t r; // Temporary value variable
	  r = UART0->CR;
	  r &= ~(XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN); // Clear Tx & Rx Enable
	  r |= XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS; // Tx & Rx Disable
	  UART0->CR = r;
	  const unsigned long sel_clk = xc7z_get_uart_freq();
	  const unsigned long bdiv = 8;
	  // baud_rate = sel_clk / (CD * (BDIV + 1) (ref: UG585 - TRM - Ch. 19 UART)
	  UART0->BAUDDIV = bdiv - 1; // ("BDIV")
	  UART0->BAUDGEN = calcdivround2(sel_clk, baudrate * bdiv); // ("CD")
	  // Baud Rate = 100Mhz / (124 * (6 + 1)) = 115200 bps
	  UART0->CR |= (XUARTPS_CR_TXRST | XUARTPS_CR_RXRST); // TX & RX logic reset

	  r = UART0->CR;
	  r |= XUARTPS_CR_RX_EN | XUARTPS_CR_TX_EN; // Set TX & RX enabled
	  r &= ~(XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS); // Clear TX & RX disabled
	  UART0->CR = r;

#else
	#warning Undefined CPUSTYLE_XXX
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
		const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_USART_BRGR_WIDTH, ATSAM3S_USART_BRGR_TAPS, & value, 0);
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
		const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_USART_BRGR_WIDTH, ATSAM3S_USART_BRGR_TAPS, & value, 0);
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
		calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_UART_BRGR_WIDTH, ATSAM3S_UART_BRGR_TAPS, & value, 0);
		UART0->UART_BRGR = value;
	#elif HARDWARE_ARM_USEUART1
		// Использование автоматического расчёта предделителя
		unsigned value;
		calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_UART_BRGR_WIDTH, ATSAM3S_UART_BRGR_TAPS, & value, 0);
		UART1->UART_BRGR = value;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), AT91SAM7_USART_BRGR_WIDTH, AT91SAM7_USART_BRGR_TAPS, & value, 0);

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
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR1A |= (1U << U2X1);
	else
		UCSR1A &= ~ (1U << U2X1);

	UBRR1 = value;	/* Значение получено уже уменьшенное на 1 */


#elif CPUSTYLE_ATMEGA128

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

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
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATXMEGA_UBR_WIDTH, ATXMEGA_UBR_TAPS, & value, 1);
	if (prei == 0)
		USARTE1.CTRLB |= USART_CLK2X_bm;
	else
		USARTE1.CTRLB &= ~USART_CLK2X_bm;
	// todo: проверить требование к порядку обращения к портам
	USARTE1.BAUDCTRLA = (value & 0xff);	/* Значение получено уже уменьшенное на 1 */
	USARTE1.BAUDCTRLB = (ATXMEGA_UBR_BSEL << 4) | ((value >> 8) & 0x0f);

#elif CPUSTYLE_STM32MP1

	// uart2
	USART2->BRR = calcdivround2(BOARD_USART2_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_STM32F

	// uart2 on apb1

	USART2->BRR = calcdivround2(BOARD_USART2_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYPE_TMS320F2833X

	const unsigned long lspclk = CPU_FREQ / 4;
	const unsigned long brr = (lspclk / 8) / baudrate;	// @ CPU_FREQ = 100 MHz, 9600 can not be programmed

	SCIBHBAUD = (brr - 1) >> 8;		// write 8 bits, not 16
	SCIBLBAUD = (brr - 1) >> 0;

#elif CPUSTYLE_R7S721

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_p1clock(baudrate), R7S721_SCIF_SCBRR_WIDTH, R7S721_SCIF_SCBRR_TAPS, & value, 1);

	SCIF3.SCSMR = (SCIF3.SCSMR & ~ 0x03) |
		scemr_scsmr [prei].scsmr |	// prescaler: 0: /1, 1: /4, 2: /16, 3: /64
		0;
	SCIF3.SCEMR = (SCIF3.SCEMR & ~ (0x80 | 0x01)) |
		0 * 0x80 |						// BGDM
		scemr_scsmr [prei].scemr |	// ABCS = 8/16 clocks per bit
		0;
	SCIF3.SCBRR = value;	/* Bit rate register */

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

	  uint32_t r; // Temporary value variable
	  r = UART1->CR;
	  r &= ~(XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN); // Clear Tx & Rx Enable
	  r |= XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS; // Tx & Rx Disable
	  UART1->CR = r;
	  const unsigned long sel_clk = xc7z_get_uart_freq();
	  const unsigned long bdiv = 8;
	  // baud_rate = sel_clk / (CD * (BDIV + 1) (ref: UG585 - TRM - Ch. 19 UART)
	  UART1->BAUDDIV = bdiv - 1; // ("BDIV")
	  UART1->BAUDGEN = calcdivround2(sel_clk, baudrate * bdiv); // ("CD")
	  // Baud Rate = 100Mhz / (124 * (6 + 1)) = 115200 bps
	  UART1->CR |= (XUARTPS_CR_TXRST | XUARTPS_CR_RXRST); // TX & RX logic reset

	  r = UART1->CR;
	  r |= XUARTPS_CR_RX_EN | XUARTPS_CR_TX_EN; // Set TX & RX enabled
	  r &= ~(XUARTPS_CR_RX_DIS | XUARTPS_CR_TX_DIS); // Clear TX & RX disabled
	  UART1->CR = r;


#else
	#warning Undefined CPUSTYLE_XXX
#endif

}

#endif /* WITHUART2HW */


#if WITHUART4HW

void
hardware_uart4_set_speed(uint_fast32_t baudrate)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		// Использование автоматического расчёта предделителя
		unsigned value;
		const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_USART_BRGR_WIDTH, ATSAM3S_USART_BRGR_TAPS, & value, 0);
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
		const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_USART_BRGR_WIDTH, ATSAM3S_USART_BRGR_TAPS, & value, 0);
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
		calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_UART_BRGR_WIDTH, ATSAM3S_UART_BRGR_TAPS, & value, 0);
		UART0->UART_BRGR = value;
	#elif HARDWARE_ARM_USEUART1
		// Использование автоматического расчёта предделителя
		unsigned value;
		calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_UART_BRGR_WIDTH, ATSAM3S_UART_BRGR_TAPS, & value, 0);
		UART1->UART_BRGR = value;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), AT91SAM7_USART_BRGR_WIDTH, AT91SAM7_USART_BRGR_TAPS, & value, 0);

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
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR1A |= (1U << U2X1);
	else
		UCSR1A &= ~ (1U << U2X1);

	UBRR1 = value;	/* Значение получено уже уменьшенное на 1 */


#elif CPUSTYLE_ATMEGA128

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

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
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATXMEGA_UBR_WIDTH, ATXMEGA_UBR_TAPS, & value, 1);
	if (prei == 0)
		USARTE1.CTRLB |= USART_CLK2X_bm;
	else
		USARTE1.CTRLB &= ~USART_CLK2X_bm;
	// todo: проверить требование к порядку обращения к портам
	USARTE1.BAUDCTRLA = (value & 0xff);	/* Значение получено уже уменьшенное на 1 */
	USARTE1.BAUDCTRLB = (ATXMEGA_UBR_BSEL << 4) | ((value >> 8) & 0x0f);

#elif CPUSTYLE_STM32MP1

	// uart4
	UART4->BRR = calcdivround2(BOARD_UART4_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_STM32F

	// uart4 on apb1

	UART4->BRR = calcdivround2(BOARD_UART4_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYPE_TMS320F2833X

	const unsigned long lspclk = CPU_FREQ / 4;
	const unsigned long brr = (lspclk / 8) / baudrate;	// @ CPU_FREQ = 100 MHz, 9600 can not be programmed

	SCIBHBAUD = (brr - 1) >> 8;		// write 8 bits, not 16
	SCIBLBAUD = (brr - 1) >> 0;

#elif CPUSTYLE_R7S721

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_p1clock(baudrate), R7S721_SCIF_SCBRR_WIDTH, R7S721_SCIF_SCBRR_TAPS, & value, 1);

	SCIF3.SCSMR = (SCIF3.SCSMR & ~ 0x03) |
		scemr_scsmr [prei].scsmr |	// prescaler: 0: /1, 1: /4, 2: /16, 3: /64
		0;
	SCIF3.SCEMR = (SCIF3.SCEMR & ~ (0x80 | 0x01)) |
		0 * 0x80 |						// BGDM
		scemr_scsmr [prei].scemr |	// ABCS = 8/16 clocks per bit
		0;
	SCIF3.SCBRR = value;	/* Bit rate register */

#else
	#warning Undefined CPUSTYLE_XXX
#endif

}

#endif /* WITHUART4HW */


#if WITHUART5HW

void
hardware_uart5_set_speed(uint_fast32_t baudrate)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		// Использование автоматического расчёта предделителя
		unsigned value;
		const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_USART_BRGR_WIDTH, ATSAM3S_USART_BRGR_TAPS, & value, 0);
		USART0->US_BRGR = value;
		if (prei == 0)
		{
			USART0->US_MR |= US_MR_OVER;
		}
		else
		{
			USART0->US_MR &= ~ US_MR_OVER;
		}
	#elif HARDWARE_ARM_USEUSART5
		// Использование автоматического расчёта предделителя
		unsigned value;
		const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_USART_BRGR_WIDTH, ATSAM3S_USART_BRGR_TAPS, & value, 0);
		USART5->US_BRGR = value;
		if (prei == 0)
		{
			USART5->US_MR |= US_MR_OVER;
		}
		else
		{
			USART5->US_MR &= ~ US_MR_OVER;
		}

	#elif HARDWARE_ARM_USEUART0
		// Использование автоматического расчёта предделителя
		unsigned value;
		calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_UART_BRGR_WIDTH, ATSAM3S_UART_BRGR_TAPS, & value, 0);
		UART0->UART_BRGR = value;
	#elif HARDWARE_ARM_USEUART1
		// Использование автоматического расчёта предделителя
		unsigned value;
		calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_UART_BRGR_WIDTH, ATSAM3S_UART_BRGR_TAPS, & value, 0);
		UART1->UART_BRGR = value;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), AT91SAM7_USART_BRGR_WIDTH, AT91SAM7_USART_BRGR_TAPS, & value, 0);

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
	#elif HARDWARE_ARM_USEUSART5
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
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR1A |= (1U << U2X1);
	else
		UCSR1A &= ~ (1U << U2X1);

	UBRR1 = value;	/* Значение получено уже уменьшенное на 1 */


#elif CPUSTYLE_ATMEGA128

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR1A |= (1U << U2X1);
	else
		UCSR1A &= ~ (1U << U2X1);

	UBRR1H = (value >> 8) & 0xff;	/* Значение получено уже уменьшенное на 1 */
	UBRR1L = value & 0xff;

#elif CPUSTYLE_ATMEGA

	#error WITHUART5HW not supported with CPUSTYLE_ATMEGA

#elif CPUSTYLE_ATXMEGAXXXA4

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATXMEGA_UBR_WIDTH, ATXMEGA_UBR_TAPS, & value, 1);
	if (prei == 0)
		USARTE1.CTRLB |= USART_CLK2X_bm;
	else
		USARTE1.CTRLB &= ~USART_CLK2X_bm;
	// todo: проверить требование к порядку обращения к портам
	USARTE1.BAUDCTRLA = (value & 0xff);	/* Значение получено уже уменьшенное на 1 */
	USARTE1.BAUDCTRLB = (ATXMEGA_UBR_BSEL << 4) | ((value >> 8) & 0x0f);

#elif CPUSTYLE_STM32MP1

	// uart5
	UART5->BRR = calcdivround2(BOARD_UART5_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_STM32F

	// uart5 on apb1

	USART5->BRR = calcdivround2(BOARD_UART5_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYPE_TMS320F2833X

	const unsigned long lspclk = CPU_FREQ / 4;
	const unsigned long brr = (lspclk / 8) / baudrate;	// @ CPU_FREQ = 100 MHz, 9600 can not be programmed

	SCIBHBAUD = (brr - 1) >> 8;		// write 8 bits, not 16
	SCIBLBAUD = (brr - 1) >> 0;

#elif CPUSTYLE_R7S721

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_p1clock(baudrate), R7S721_SCIF_SCBRR_WIDTH, R7S721_SCIF_SCBRR_TAPS, & value, 1);

	SCIF4.SCSMR = (SCIF4.SCSMR & ~ 0x03) |
		scemr_scsmr [prei].scsmr |	// prescaler: 0: /1, 1: /4, 2: /16, 3: /64
		0;
	SCIF4.SCEMR = (SCIF4.SCEMR & ~ (0x80 | 0x01)) |
		0 * 0x80 |						// BGDM
		scemr_scsmr [prei].scemr |	// ABCS = 8/16 clocks per bit
		0;
	SCIF4.SCBRR = value;	/* Bit rate register */

#else
	#warning Undefined CPUSTYLE_XXX
#endif

}

#endif /* WITHUART5HW */

#if WITHUART7HW

void
hardware_uart7_set_speed(uint_fast32_t baudrate)
{
#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

	#if HARDWARE_ARM_USEUSART0
		// Использование автоматического расчёта предделителя
		unsigned value;
		const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_USART_BRGR_WIDTH, ATSAM3S_USART_BRGR_TAPS, & value, 0);
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
		const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_USART_BRGR_WIDTH, ATSAM3S_USART_BRGR_TAPS, & value, 0);
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
		calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_UART_BRGR_WIDTH, ATSAM3S_UART_BRGR_TAPS, & value, 0);
		UART0->UART_BRGR = value;
	#elif HARDWARE_ARM_USEUART1
		// Использование автоматического расчёта предделителя
		unsigned value;
		calcdivider(calcdivround2(CPU_FREQ, baudrate), ATSAM3S_UART_BRGR_WIDTH, ATSAM3S_UART_BRGR_TAPS, & value, 0);
		UART1->UART_BRGR = value;
	#else	/* HARDWARE_ARM_USExxx */
		#error Wrong HARDWARE_ARM_USExxx value
	#endif

#elif CPUSTYLE_AT91SAM7S

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), AT91SAM7_USART_BRGR_WIDTH, AT91SAM7_USART_BRGR_TAPS, & value, 0);

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
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

	if (prei == 0)
		UCSR1A |= (1U << U2X1);
	else
		UCSR1A &= ~ (1U << U2X1);

	UBRR1 = value;	/* Значение получено уже уменьшенное на 1 */


#elif CPUSTYLE_ATMEGA128

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATMEGA_UBR_WIDTH, ATMEGA_UBR_TAPS, & value, 1);

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
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, baudrate), ATXMEGA_UBR_WIDTH, ATXMEGA_UBR_TAPS, & value, 1);
	if (prei == 0)
		USARTE1.CTRLB |= USART_CLK2X_bm;
	else
		USARTE1.CTRLB &= ~USART_CLK2X_bm;
	// todo: проверить требование к порядку обращения к портам
	USARTE1.BAUDCTRLA = (value & 0xff);	/* Значение получено уже уменьшенное на 1 */
	USARTE1.BAUDCTRLB = (ATXMEGA_UBR_BSEL << 4) | ((value >> 8) & 0x0f);

#elif CPUSTYLE_STM32MP1

	// uart7
	UART7->BRR = calcdivround2(BOARD_UART7_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYLE_STM32F

	// uart7 on apb1

	USART7->BRR = calcdivround2(BOARD_UART7_FREQ, baudrate);		// младшие 4 бита - это дробная часть.

#elif CPUSTYPE_TMS320F2833X

	const unsigned long lspclk = CPU_FREQ / 4;
	const unsigned long brr = (lspclk / 8) / baudrate;	// @ CPU_FREQ = 100 MHz, 9600 can not be programmed

	SCIBHBAUD = (brr - 1) >> 8;		// write 8 bits, not 16
	SCIBLBAUD = (brr - 1) >> 0;

#elif CPUSTYLE_R7S721

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround_p1clock(baudrate), R7S721_SCIF_SCBRR_WIDTH, R7S721_SCIF_SCBRR_TAPS, & value, 1);

	SCIF6.SCSMR = (SCIF6.SCSMR & ~ 0x03) |
		scemr_scsmr [prei].scsmr |	// prescaler: 0: /1, 1: /4, 2: /16, 3: /64
		0;
	SCIF6.SCEMR = (SCIF6.SCEMR & ~ (0x80 | 0x01)) |
		0 * 0x80 |						// BGDM
		scemr_scsmr [prei].scemr |	// ABCS = 8/16 clocks per bit
		0;
	SCIF6.SCBRR = value;	/* Bit rate register */

#else
	#warning Undefined CPUSTYLE_XXX
#endif

}

#endif /* WITHUART7HW */

#if WITHTWIHW

void hardware_twi_master_configure(void)
{
#if CPUSTYLE_ATMEGA

	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, SCL_CLOCK * 2) - 8, ATMEGA_TWBR_WIDTH, ATMEGA_TWBR_TAPS, & value, 0);

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
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, SCL_CLOCK * 2) - 3, AT91SAM7_TWI_WIDTH, AT91SAM7_TWI_TAPS, & value, 0);

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
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, SCL_CLOCK * 2) - 4, ATSAM3S_TWI_WIDTH, ATSAM3S_TWI_TAPS, & value, 1);
	//prei = 0;
	//value = 70;
    TWI0->TWI_CWGR = TWI_CWGR_CKDIV(prei) | TWI_CWGR_CHDIV(value) | TWI_CWGR_CLDIV(value);

#elif CPUSTYLE_ATXMEGA

	TARGET_TWI.MASTER.BAUD = ((CPU_FREQ / (2 * SCL_CLOCK)) - 5);
	TARGET_TWI.MASTER.CTRLA = TWI_MASTER_ENABLE_bm;
  	//TARGET_TWI.MASTER.CTRLB = TWI_MASTER_SMEN_bm;
	TARGET_TWI.MASTER.STATUS = TWI_MASTER_BUSSTATE_IDLE_gc;

#elif CPUSTYLE_STM32F1XX || CPUSTYLE_STM32F4XX

	//конфигурирую непосредствено I2С
	RCC->APB1ENR |= (RCC_APB1ENR_I2C1EN); //вкл тактирование контроллера I2C
	(void) RCC->APB1ENR;

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
		(calcdivround2(BOARD_I2C_FREQ, SCL_CLOCK * 25) & I2C_CCR_CCR) |	// Делитель для получения 10 МГц (400 кHz * 25)
	#if SCL_CLOCK == 400000UL
		I2C_CCR_FS |
		I2C_CCR_DUTY | // T high = 9 * CCR * TPCLK1, T low = 16 * CCR * TPCLK1: full cycle = 25 * CCR * TPCLK1
	#endif /* SCL_CLOCK == 400000UL */
		0;

	I2C1->CR1 |= I2C_CR1_ACK | I2C_CR1_PE; // включаю тактирование переферии I2С

#elif CPUSTYLE_STM32F30X || CPUSTYLE_STM32F0XX

	//конфигурирую непосредствено I2С
	RCC->APB1ENR |= (RCC_APB1ENR_I2C1EN); //вкл тактирование контроллера I2C
	(void) RCC->APB1ENR;
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

	I2C1->CR1 |= I2C_CR1_PE; // включаю тактирование периферии I2С

#elif CPUSTYLE_STM32F7XX
	//конфигурирую непосредствено I2С
	RCC->APB1ENR |= (RCC_APB1ENR_I2C1EN); //вкл тактирование контроллера I2C
	(void) RCC->APB1ENR;

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
	//конфигурирую непосредствено I2С
	RCC->APB1LENR |= (RCC_APB1LENR_I2C1EN); //вкл тактирование контроллера I2C
	(void) RCC->APB1LENR;

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

#elif CPUSTYLE_XC7Z || CPUSTYLE_XCZU

#else
	#warning Undefined CPUSTYLE_XXX
#endif
}

#endif /* WITHTWIHW */

