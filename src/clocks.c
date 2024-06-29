/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints

#include "board.h"
#include "gpio.h"
#include "clocks.h"


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

uint_fast32_t stm32f4xx_get_hse_freq(void)
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

uint_fast32_t stm32f4xx_get_hsi_freq(void)
{
	return HSIFREQ;	// 16 MHz
}

uint_fast32_t stm32f4xx_get_pllreference_freq(void)
{
	const uint_fast32_t divval = (RCC->PLLCFGR & RCC_PLLCFGR_PLLM_Msk) >> RCC_PLLCFGR_PLLM_Pos;
	return stm32f4xx_get_hse_freq() / divval;
}

uint_fast32_t stm32f4xx_get_pll_freq(void)
{
	const uint_fast32_t mulval = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN_Msk) >> RCC_PLLCFGR_PLLN_Pos;
	return stm32f4xx_get_pllreference_freq() * mulval;
}

uint_fast32_t stm32f4xx_get_pll_p_freq(void)
{
	const uint_fast32_t divval = (RCC->PLLCFGR & RCC_PLLCFGR_PLLP_Msk) >> RCC_PLLCFGR_PLLP_Pos;
	return stm32f4xx_get_pll_freq() / divval;
}

uint_fast32_t stm32f4xx_get_pll_q_freq(void)
{
	const uint_fast32_t divval = (RCC->PLLCFGR & RCC_PLLCFGR_PLLQ_Msk) >> RCC_PLLCFGR_PLLQ_Pos;
	return stm32f4xx_get_pll_freq() / divval;
}

uint_fast32_t stm32f4xx_get_plli2s_freq(void)
{
	const uint_fast32_t mulval = (RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SN_Msk) >> RCC_PLLI2SCFGR_PLLI2SN_Pos;
	return stm32f4xx_get_pllreference_freq() * mulval;
}

uint_fast32_t stm32f4xx_get_plli2s_q_freq(void)
{
	const uint_fast32_t divval = (RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SQ_Msk) >> RCC_PLLI2SCFGR_PLLI2SQ_Pos;
	return stm32f4xx_get_plli2s_freq() / divval;
}

uint_fast32_t stm32f4xx_get_plli2s_r_freq(void)
{
	const uint_fast32_t divval = (RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SR_Msk) >> RCC_PLLI2SCFGR_PLLI2SR_Pos;
	return stm32f4xx_get_plli2s_freq() / divval;
}

uint_fast32_t stm32f4xx_get_pllsai_freq(void)
{
	const uint_fast32_t mulval = (RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIN_Msk) >> RCC_PLLSAICFGR_PLLSAIN_Pos;
	return stm32f4xx_get_pllreference_freq() * mulval;
}

uint_fast32_t stm32f4xx_get_pllsai_q_freq(void)
{
	const uint_fast32_t divval = (RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIQ_Msk) >> RCC_PLLSAICFGR_PLLSAIQ_Pos;
	return stm32f4xx_get_pllsai_freq() / divval;
}

uint_fast32_t stm32f4xx_get_pllsai_r_freq(void)
{
	const uint_fast32_t divval = (RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIR_Msk) >> RCC_PLLSAICFGR_PLLSAIR_Pos;
	return stm32f4xx_get_pllsai_freq() / divval;
}

uint_fast32_t stm32f4xx_get_sysclk_freq(void)
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

uint_fast32_t stm32f4xx_get_ahb_freq(void)
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

uint_fast32_t stm32f4xx_get_hclk_freq(void)
{
	return stm32f4xx_get_ahb_freq();
}

// PPRE2: APB high-speed prescaler (APB2)
uint_fast32_t stm32f4xx_get_apb2_freq(void)
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
uint_fast32_t stm32f4xx_get_apb1_freq(void)
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
uint_fast32_t stm32f4xx_get_apb2timer_freq(void)
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
uint_fast32_t stm32f4xx_get_apb1timer_freq(void)
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

uint_fast32_t stm32f4xx_get_spi1_freq(void)
{
	return stm32f4xx_get_apb2_freq();
}

uint_fast32_t stm32f4xx_get_tim3_freq(void)
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

#define BOARD_TIM3_FREQ (stm32f7xx_get_apb1_tim_freq())	// TODO: verify
#define BOARD_ADC_FREQ (stm32f7xx_get_apb2_freq())

uint_fast32_t stm32f7xx_get_hse_freq(void)
{
#if WITHCPUXTAL
	return WITHCPUXTAL;
#elif WITHCPUXOSC
	return WITHCPUXOSC;
#else
	#warning WITHCPUXOSC or WITHCPUXTAL should be defined
	return 24000000u;
#endif
}

uint_fast32_t stm32f7xx_get_pll_freq(void)
{
	const uint_fast32_t pllm = (RCC->PLLCFGR & RCC_PLLCFGR_PLLM_Msk) >> RCC_PLLCFGR_PLLM_Pos;
	const uint_fast32_t plln = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN_Msk) >> RCC_PLLCFGR_PLLN_Pos;

	return (uint_fast64_t) REFINFREQ * plln / pllm;
}

uint_fast32_t stm32f7xx_get_pllsai_freq(void)
{
	const uint_fast32_t pllm = (RCC->PLLCFGR & RCC_PLLCFGR_PLLM_Msk) >> RCC_PLLCFGR_PLLM_Pos;
	const uint_fast32_t pllsain = (RCC->PLLSAICFGR & RCC_PLLSAICFGR_PLLSAIN_Msk) >> RCC_PLLSAICFGR_PLLSAIN_Pos;

	return (uint_fast64_t) REFINFREQ * pllsain / pllm;
}

uint_fast32_t stm32f7xx_get_plli2s_freq(void)
{
	const uint_fast32_t pllm = (RCC->PLLCFGR & RCC_PLLCFGR_PLLM_Msk) >> RCC_PLLCFGR_PLLM_Pos;
	const uint_fast32_t plli2sn = (RCC->PLLI2SCFGR & RCC_PLLI2SCFGR_PLLI2SN_Msk) >> RCC_PLLI2SCFGR_PLLI2SN_Pos;

	return (uint_fast64_t) REFINFREQ * plli2sn / pllm;
}

uint_fast32_t stm32f7xx_get_pll_p_freq(void)
{
	const uint_fast32_t pll = stm32f7xx_get_pll_freq();
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
uint_fast32_t stm32f7xx_get_sys_freq(void)
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
//uint_fast32_t stm32f7xx_get_sysclk_freq(void)
//{
//	return stm32f7xx_get_sys_freq();
//}

// AHB prescaler
// HPRE output
uint_fast32_t stm32f7xx_get_ahb_freq(void)
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
	const uint_fast32_t sys = stm32f7xx_get_sys_freq();
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
uint_fast32_t stm32f7xx_get_apb1_freq(void)
{
	//	0xx: AHB clock not divided
	//	100: AHB clock divided by 2
	//	101: AHB clock divided by 4
	//	110: AHB clock divided by 8
	//	111: AHB clock divided by 16
	const uint_fast32_t ahb = stm32f7xx_get_ahb_freq();
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
uint_fast32_t stm32f7xx_get_apb1_tim_freq(void)
{
	const uint_fast32_t sysclk = stm32f7xx_get_sys_freq();
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

	const uint_fast32_t ahb = stm32f7xx_get_ahb_freq();
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
uint_fast32_t stm32f7xx_get_apb2_freq(void)
{
	//	0xx: AHB clock not divided
	//	100: AHB clock divided by 2
	//	101: AHB clock divided by 4
	//	110: AHB clock divided by 8
	//	111: AHB clock divided by 16
	const uint_fast32_t ahb = stm32f7xx_get_ahb_freq();
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
uint_fast32_t stm32f7xx_get_apb2_tim_freq(void)
{
	const uint_fast32_t sysclk = stm32f7xx_get_sys_freq();
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

	const uint_fast32_t ahb = stm32f7xx_get_ahb_freq();
	switch ((RCC->CFGR & RCC_CFGR_PPRE2_Msk) >> RCC_CFGR_PPRE2_Pos)
	{
	default: return ahb;
	case 0x04: return timpre ? ahb / 1 : ahb / 2;
	case 0x05: return timpre ? ahb / 2 : ahb / 4;
	case 0x06: return timpre ? ahb / 4 : ahb / 8;
	case 0x07: return timpre ? ahb / 8 : ahb / 16;
	}
}

// TODO: проверить - USART3 работает
uint_fast32_t stm32f7xx_get_pclk1_freq(void)
{
	return stm32f7xx_get_apb1_freq();
}

// TODO: проверить
uint_fast32_t stm32f7xx_get_pclk2_freq(void)
{
	return hardware_get_apb2_freq();
}

// TODO: проверить
// получение тактовой частоты тактирования блока SPI, использующенося в данной конфигурации
unsigned long hardware_get_spi_freq(void)
{
	const uint_fast32_t sysclk = stm32f7xx_get_sys_freq();
	return sysclk / 4;
}

uint_fast32_t stm32f7xx_get_usart1_freq(void)
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

uint_fast32_t stm32f7xx_get_usart2_freq(void)
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

uint_fast32_t stm32f7xx_get_usart3_freq(void)
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

uint_fast32_t stm32f7xx_get_uart4_freq(void)
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

uint_fast32_t stm32f7xx_get_uart5_freq(void)
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

uint_fast32_t stm32f7xx_get_usart6_freq(void)
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

uint_fast32_t stm32f7xx_get_uart7_freq(void)
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

uint_fast32_t stm32f7xx_get_uart8_freq(void)
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
#define BOARD_TIM3_FREQ 	(stm32h7xx_get_timx_freq())	// TIM2..TIM7, TIM12..TIM14, LPTIM1, : APB1 D2 bus
#define BOARD_SYSTICK_FREQ 	(stm32h7xx_get_sys_freq() / 1)	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора
//#define BOARD_SYSTICK_FREQ (stm32h7xx_get_sysclk_freq() / 1)	// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора

uint_fast32_t stm32h7xx_get_hse_freq(void)
{
#if WITHCPUXTAL
	return WITHCPUXTAL;
#elif WITHCPUXOSC
	return WITHCPUXOSC;
#else
	#warning WITHCPUXOSC or WITHCPUXTAL should be defined
	return 24000000u;
#endif
}
//
//// SysTick_Config устанавливает SysTick_CTRL_CLKSOURCE_Msk - используется частота процессора
//uint_fast32_t stm32h7xx_get_systick_freq(void)
//{
//
//
//}

uint_fast32_t stm32h7xx_get_pll1_freq(void)
{
	const uint32_t pll1divm1 = ((RCC->PLLCKSELR & RCC_PLLCKSELR_DIVM1_Msk) >> RCC_PLLCKSELR_DIVM1_Pos);	// reference divisor - not need 1substracted
	const uint32_t pll1divn1 = ((RCC->PLL1DIVR & RCC_PLL1DIVR_N1_Msk) >> RCC_PLL1DIVR_N1_Pos) + 1;
	return (uint_fast64_t) REFINFREQ * pll1divn1 / pll1divm1;
}

uint_fast32_t stm32h7xx_get_pll1_p_freq(void)
{
	const uint_fast32_t pll1divp = ((RCC->PLL1DIVR & RCC_PLL1DIVR_P1_Msk) >> RCC_PLL1DIVR_P1_Pos) + 1;
	return stm32h7xx_get_pll1_freq() / pll1divp;
}

uint_fast32_t stm32h7xx_get_pll1_q_freq(void)
{
	const uint_fast32_t pll1divq = ((RCC->PLL1DIVR & RCC_PLL1DIVR_Q1_Msk) >> RCC_PLL1DIVR_Q1_Pos) + 1;
	return stm32h7xx_get_pll1_freq() / pll1divq;
}

uint_fast32_t stm32h7xx_get_pll2_freq(void)
{
	const uint32_t pll2divm2 = ((RCC->PLLCKSELR & RCC_PLLCKSELR_DIVM2_Msk) >> RCC_PLLCKSELR_DIVM2_Pos);	// reference divisor - not need 1substracted
	const uint32_t pll2divn2 = ((RCC->PLL2DIVR & RCC_PLL2DIVR_N2_Msk) >> RCC_PLL2DIVR_N2_Pos) + 1;
	return (uint_fast64_t) REFINFREQ * pll2divn2 / pll2divm2;
}

uint_fast32_t stm32h7xx_get_pll2_p_freq(void)
{
	const uint_fast32_t pll2divp = ((RCC->PLL2DIVR & RCC_PLL2DIVR_P2_Msk) >> RCC_PLL2DIVR_P2_Pos) + 1;
	return stm32h7xx_get_pll2_freq() / pll2divp;
}

uint_fast32_t stm32h7xx_get_pll2_q_freq(void)
{
	const uint_fast32_t pll2divq = ((RCC->PLL2DIVR & RCC_PLL2DIVR_Q2_Msk) >> RCC_PLL2DIVR_Q2_Pos) + 1;
	return stm32h7xx_get_pll2_freq() / pll2divq;
}

uint_fast32_t stm32h7xx_get_pll3_freq(void)
{
	const uint32_t pll3divm3 = ((RCC->PLLCKSELR & RCC_PLLCKSELR_DIVM3_Msk) >> RCC_PLLCKSELR_DIVM3_Pos);	// reference divisor - not need 1substracted
	const uint32_t pll3divn3 = ((RCC->PLL3DIVR & RCC_PLL3DIVR_N3_Msk) >> RCC_PLL3DIVR_N3_Pos) + 1;
	return (uint_fast64_t) REFINFREQ * pll3divn3 / pll3divm3;
}

uint_fast32_t stm32h7xx_get_pll3_p_freq(void)
{
	const uint_fast32_t pll3divp = ((RCC->PLL3DIVR & RCC_PLL3DIVR_P3_Msk) >> RCC_PLL3DIVR_P3_Pos) + 1;
	return stm32h7xx_get_pll3_freq() / pll3divp;
}

uint_fast32_t stm32h7xx_get_pll3_q_freq(void)
{
	const uint_fast32_t pll3divq = ((RCC->PLL3DIVR & RCC_PLL3DIVR_Q3_Msk) >> RCC_PLL3DIVR_Q3_Pos) + 1;
	return stm32h7xx_get_pll3_freq() / pll3divq;
}

uint_fast32_t stm32h7xx_get_pll3_r_freq(void)
{
	const uint_fast32_t pll3divr = ((RCC->PLL3DIVR & RCC_PLL3DIVR_R3_Msk) >> RCC_PLL3DIVR_R3_Pos) + 1;
	return stm32h7xx_get_pll3_freq() / pll3divr;
}

uint_fast32_t stm32h7xx_get_sys_freq(void)
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
uint_fast32_t stm32h7xx_get_stm32h7xx_get_d1cpre_freq(void)
{
	const uint_fast32_t sys_ck = stm32h7xx_get_sys_freq();
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

uint_fast32_t stm32h7xx_get_hclk3_freq(void)
{
	const uint_fast32_t rcc_d1cpre = stm32h7xx_get_stm32h7xx_get_d1cpre_freq();
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
uint_fast32_t stm32h7xx_get_aclk_freq(void)
{
	return stm32h7xx_get_hclk3_freq();
}

// HPRE output
// rcc_hclk1
uint_fast32_t stm32h7xx_get_hclk1_freq(void)
{
	return stm32h7xx_get_hclk3_freq();
}

// HPRE output
// rcc_hclk2
uint_fast32_t stm32h7xx_get_hclk2_freq(void)
{
	return stm32h7xx_get_hclk3_freq();
}

// apb1
// rcc_timx_ker_ck
// rcc_pclk1
uint_fast32_t stm32h7xx_get_apb1_freq(void)
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
uint_fast32_t stm32h7xx_get_apb2_freq(void)
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
uint_fast32_t stm32h7xx_get_timx_freq(void)
{
	const uint_fast32_t rcc_hclk1 = stm32h7xx_get_hclk1_freq();
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
uint_fast32_t stm32h7xx_get_pclk1_freq(void)
{
	const uint_fast32_t rcc_hclk1 = stm32h7xx_get_hclk1_freq();
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
uint_fast32_t stm32h7xx_get_timy_freq(void)
{
	const uint_fast32_t rcc_hclk1 = stm32h7xx_get_hclk2_freq();
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
uint_fast32_t stm32h7xx_get_pclk2_freq(void)
{
	const uint_fast32_t rcc_hclk1 = stm32h7xx_get_hclk2_freq();
	switch ((RCC->D2CFGR & RCC_D2CFGR_D2PPRE2_Msk) >> RCC_D2CFGR_D2PPRE2_Pos)
	{
	default: return rcc_hclk1;
	case 0x04: return rcc_hclk1 / 2;
	case 0x05: return rcc_hclk1 / 4;
	case 0x06: return rcc_hclk1 / 8;
	case 0x07: return rcc_hclk1 / 16;
	}
}

uint_fast32_t stm32h7xx_get_per_freq(void)
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
uint_fast32_t stm32h7xx_get_pclk3_freq(void)
{
	const uint_fast32_t rcc_hclk3 = stm32h7xx_get_hclk3_freq();	// HPRE output
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
uint_fast32_t stm32h7xx_get_usart1_6_freq(void)
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
uint_fast32_t stm32h7xx_get_usart2_to_8_freq(void)
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


uint_fast32_t stm32h7xx_get_spi1_2_3_freq(void)
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

uint_fast32_t stm32h7xx_get_spi4_5_freq(void)
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

uint_fast32_t stm32h7xx_get_adc_freq(void)
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
#define BOARD_TIM3_FREQ 	(stm32mp1_get_timg1_freq())
#define BOARD_TIM5_FREQ 	(stm32mp1_get_timg1_freq())
#define BOARD_ADC_FREQ 		(stm32mp1_get_adc_freq())

uint_fast32_t stm32mp1_get_hse_freq(void)
{
#if WITHCPUXTAL
	return WITHCPUXTAL;
#elif WITHCPUXOSC
	return WITHCPUXOSC;
#else
	#warning WITHCPUXOSC or WITHCPUXTAL should be defined
	return 24000000u;
#endif
}

// hsi_ck
// hsi_ker_ck
uint_fast32_t stm32mp1_get_hsi_freq(void)
{
	const uint_fast32_t hsi = HSI64FREQ;
	const uint_fast32_t hsidiv = (RCC->HSICFGR & RCC_HSICFGR_HSIDIV_Msk) >> RCC_HSICFGR_HSIDIV_Pos;
	return hsi >> hsidiv;
}

uint_fast32_t stm32mp1_get_pll1_2_ref_freq(void)
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

uint_fast32_t stm32mp1_get_pll3_ref_freq(void)
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

uint_fast32_t stm32mp1_get_pll4_ref_freq(void)
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
uint_fast32_t stm32mp1_get_pll1_freq(void)
{
	const uint_fast32_t pll1divn = ((RCC->PLL1CFGR1 & RCC_PLL1CFGR1_DIVN_Msk) >> RCC_PLL1CFGR1_DIVN_Pos) + 1;
	const uint_fast32_t pll1divm = ((RCC->PLL1CFGR1 & RCC_PLL1CFGR1_DIVM1_Msk) >> RCC_PLL1CFGR1_DIVM1_Pos) + 1;
	return (uint_fast64_t) stm32mp1_get_pll1_2_ref_freq() * pll1divn / pll1divm;
}

uint_fast32_t stm32mp1_get_pll1_p_freq(void)
{
	const uint_fast32_t pll1divp = ((RCC->PLL1CFGR2 & RCC_PLL1CFGR2_DIVP_Msk) >> RCC_PLL1CFGR2_DIVP_Pos) + 1;
	return stm32mp1_get_pll1_freq() / pll1divp;
}

// MPU frequency
// mpuss_ck
uint_fast32_t stm32mp1_get_mpuss_freq(void)
{
	//	0x0: The MPUDIV is disabled; i.e. no clock generated
	//	0x1: The mpuss_ck is equal to pll1_p_ck divided by 2 (default after reset)
	//	0x2: The mpuss_ck is equal to pll1_p_ck divided by 4
	//	0x3: The mpuss_ck is equal to pll1_p_ck divided by 8
	//	others: The mpuss_ck is equal to pll1_p_ck divided by 16

	const uint_fast32_t mpudiv = UINT32_C(1) << ((RCC->MPCKDIVR & RCC_MPCKDIVR_MPUDIV_Msk) >> RCC_MPCKDIVR_MPUDIV_Pos);

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
uint_fast32_t stm32mp1_get_pll2_freq(void)
{
	const uint_fast32_t pll2divn = ((RCC->PLL2CFGR1 & RCC_PLL2CFGR1_DIVN_Msk) >> RCC_PLL2CFGR1_DIVN_Pos) + 1;
	const uint_fast32_t pll2divm = ((RCC->PLL2CFGR1 & RCC_PLL2CFGR1_DIVM2_Msk) >> RCC_PLL2CFGR1_DIVM2_Pos) + 1;
	return (uint_fast64_t) stm32mp1_get_pll1_2_ref_freq() * pll2divn / pll2divm;
}

uint_fast32_t stm32mp1_get_pll2_p_freq(void)
{
	const uint_fast32_t pll2divp = ((RCC->PLL2CFGR2 & RCC_PLL2CFGR2_DIVP_Msk) >> RCC_PLL2CFGR2_DIVP_Pos) + 1;
	return stm32mp1_get_pll2_freq() / pll2divp;
}

uint_fast32_t stm32mp1_get_pll2_r_freq(void)
{
	const uint_fast32_t pll2divr = ((RCC->PLL2CFGR2 & RCC_PLL2CFGR2_DIVR_Msk) >> RCC_PLL2CFGR2_DIVR_Pos) + 1;
	return stm32mp1_get_pll2_freq() / pll2divr;
}

// PLL3 methods
uint_fast32_t stm32mp1_get_pll3_freq(void)
{
	const uint_fast32_t pll3divn = ((RCC->PLL3CFGR1 & RCC_PLL3CFGR1_DIVN_Msk) >> RCC_PLL3CFGR1_DIVN_Pos) + 1;
	const uint_fast32_t pll3divm = ((RCC->PLL3CFGR1 & RCC_PLL3CFGR1_DIVM3_Msk) >> RCC_PLL3CFGR1_DIVM3_Pos) + 1;
	return (uint_fast64_t) stm32mp1_get_pll3_ref_freq() * pll3divn / pll3divm;
}

uint_fast32_t stm32mp1_get_pll3_p_freq(void)
{
	const uint_fast32_t pll3divp = ((RCC->PLL3CFGR2 & RCC_PLL3CFGR2_DIVP_Msk) >> RCC_PLL3CFGR2_DIVP_Pos) + 1;
	return stm32mp1_get_pll3_freq() / pll3divp;
}

uint_fast32_t stm32mp1_get_pll3_q_freq(void)
{
	const uint_fast32_t pll3divq = ((RCC->PLL3CFGR2 & RCC_PLL3CFGR2_DIVQ_Msk) >> RCC_PLL3CFGR2_DIVQ_Pos) + 1;
	return stm32mp1_get_pll3_freq() / pll3divq;
}

uint_fast32_t stm32mp1_get_pll3_r_freq(void)
{
	const uint_fast32_t pll3divr = ((RCC->PLL3CFGR2 & RCC_PLL3CFGR2_DIVR_Msk) >> RCC_PLL3CFGR2_DIVR_Pos) + 1;
	return stm32mp1_get_pll3_freq() / pll3divr;
}

// PLL4 methods
uint_fast32_t stm32mp1_get_pll4_freq(void)
{
	//#define PLL4_FREQ	(REFINFREQ / (PLL4DIVM) * (PLL4DIVN))
	const uint32_t pll4divn = ((RCC->PLL4CFGR1 & RCC_PLL4CFGR1_DIVN_Msk) >> RCC_PLL4CFGR1_DIVN_Pos) + 1;
	const uint32_t pll4divm = ((RCC->PLL4CFGR1 & RCC_PLL4CFGR1_DIVM4_Msk) >> RCC_PLL4CFGR1_DIVM4_Pos) + 1;
	return (uint_fast64_t) stm32mp1_get_pll4_ref_freq() * pll4divn / pll4divm;
}

uint_fast32_t stm32mp1_get_pll4_q_freq(void)
{
	const uint_fast32_t pll4divq = ((RCC->PLL4CFGR2 & RCC_PLL4CFGR2_DIVQ_Msk) >> RCC_PLL4CFGR2_DIVQ_Pos) + 1;
	return stm32mp1_get_pll4_freq() / pll4divq;
}

uint_fast32_t stm32mp1_get_pll4_p_freq(void)
{
	const uint_fast32_t pll4divp = ((RCC->PLL4CFGR2 & RCC_PLL4CFGR2_DIVP_Msk) >> RCC_PLL4CFGR2_DIVP_Pos) + 1;
	return stm32mp1_get_pll4_freq() / pll4divp;
}

uint_fast32_t stm32mp1_get_pll4_r_freq(void)
{
	const uint_fast32_t pll4divr = ((RCC->PLL4CFGR2 & RCC_PLL4CFGR2_DIVR_Msk) >> RCC_PLL4CFGR2_DIVR_Pos) + 1;
	return stm32mp1_get_pll4_freq() / pll4divr;
}

// Ethernet controller freq
// ETHSRC
uint_fast32_t stm32mp1_get_eth_freq(void)
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
uint_fast32_t stm32mp1_get_ethptp_freq(void)
{
	const uint_fast32_t d = ((RCC->ETHCKSELR & RCC_ETHCKSELR_ETHPTPDIV_Msk) >> RCC_ETHCKSELR_ETHPTPDIV_Pos) + 1;
	return stm32mp1_get_eth_freq() / d;
}

// Internal AXI clock frequency
uint_fast32_t stm32mp1_get_axiss_freq(void)
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

uint_fast32_t stm32mp1_get_per_freq(void)
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
uint_fast32_t stm32mp1_get_mcuss_freq(void)
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
uint_fast32_t stm32mp1_get_aclk_freq(void)
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

// Internal AHB5 clock frequency (up to 266 MHz)
uint_fast32_t stm32mp1_get_hclk5_freq(void)
{
	return stm32mp1_get_aclk_freq();
}

// Internal AHB6 clock frequency (up to 266 MHz)
uint_fast32_t stm32mp1_get_hclk6_freq(void)
{
	return stm32mp1_get_aclk_freq();
}

// mlhclk_ck
// 209 MHz max
uint_fast32_t stm32mp1_get_mlhclk_freq(void)
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
uint_fast32_t stm32mp1_get_pclk1_freq(void)
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
uint_fast32_t stm32mp1_get_pclk2_freq(void)
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
uint_fast32_t stm32mp1_get_pclk3_freq(void)
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
uint_fast32_t stm32mp1_get_pclk4_freq(void)
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
uint_fast32_t stm32mp1_get_pclk5_freq(void)
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

uint_fast32_t stm32mp1_get_usbphy_freq(void)
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

uint_fast32_t stm32mp1_get_qspi_freq(void)
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

uint_fast32_t stm32mp1_get_usbotg_freq(void)
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

uint_fast32_t stm32mp1_uart1_get_freq(void)
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

uint_fast32_t stm32mp1_uart2_4_get_freq(void)
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

uint_fast32_t stm32mp1_uart3_5_get_freq(void)
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

uint_fast32_t stm32mp1_uart7_8_get_freq(void)
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

uint_fast32_t stm32mp1_sdmmc1_2_get_freq(void)
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
uint_fast32_t stm32mp1_get_timg1_freq(void)
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
uint_fast32_t stm32mp1_get_timg2_freq(void)
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

uint_fast32_t stm32mp1_get_spi1_freq(void)
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

uint_fast32_t stm32mp1_get_adc_freq(void)
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

#elif CPUSTYLE_V3S

// V3s
uint_fast32_t allwnrt113_get_hosc_freq(void)
{
    return REFINFREQ;	// 24 MHz usually
}

// V3s
uint_fast32_t allwnrt113_get_losc_freq(void)
{
	const uint_fast32_t reg = RTC->LOSC_CTRL_REG;

	// LOSC_SRC_SEL 0: Internal 32KHz, 1: External 32.768KHz OSC.
	return (reg & 0x01) ? LSEFREQ : allwnrt113_get_hosc_freq() / 750;
}

//	The PLL Output = 24MHz*N*K/2.
//	Note: The PLL Output should be fixed to 600MHz, it is not recommended to
//	vary this value arbitrarily.
//	In the Clock Control Module, PLL(2X) output= PLL*2 = 24MHz*N*K.
//	The PLL output clock must be in the range of 200MHz~1.8GHz.
//	Its default is 600MHz.
// V3s
uint_fast32_t allwnr_v3s_get_pll_periph0_x2_freq(void)
{
	const uint_fast32_t reg = CCU->PLL_PERIPH0_CTRL_REG;
	//const uint_fast32_t P = UINT32_C(1) + ((reg >> 16) & 0x0F);	// PLL_24M_POST_DIV
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0x1F);	// PLL_FACTOR_N
	const uint_fast32_t K = UINT32_C(1) + ((reg >> 4) & 0x03);	// PLL_FACTOR_K
	//const uint_fast32_t M = UINT32_C(1) + ((reg >> 0) & 0x03);	// PLL_FACTOR_M = PLL Factor M (M = Factor + 1) is only valid in plltest debug.

	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N  * K;
}

// V3s
uint_fast32_t allwnr_v3s_get_pll_periph0_freq(void)
{
	return allwnr_v3s_get_pll_periph0_x2_freq() / 2;
}


uint_fast32_t allwnr_v3s_get_cpu_freq(void)
{
	return 1008000000;
}

// V3s
uint_fast32_t allwnr_v3s_get_sysapb_freq(void)
{
    return REFINFREQ;	// 24 MHz usually
}

// V3s
uint_fast32_t allwnr_v3s_get_axi_freq(void)
{
    return REFINFREQ;	// 24 MHz usually
}

// V3s
uint_fast32_t allwnr_v3s_get_ahb1_freq(void)
{
    return REFINFREQ;	// 24 MHz usually
}

// V3s
uint_fast32_t allwnr_v3s_get_ahb2_freq(void)
{
    return REFINFREQ;	// 24 MHz usually
}

// V3s
uint_fast32_t allwnr_v3s_get_apb1_freq(void)
{
    return REFINFREQ;	// 24 MHz usually
}

// V3s
uint_fast32_t allwnr_v3s_get_apb2_freq(void)
{
	const uint_fast32_t clkreg = CCU->APB2_CFG_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 16) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x1F);
	//	APB2 Clock Source Select
	//	00: LOSC
	//	01: OSC24M
	//	1X: PLL_PERIPH0
	// This clock is used for some special module apbclk(UART、TWI). Because
	// these modules need special clock rate even if the apb1clk changed
	switch ((clkreg >> 24) & 0x03)	/* APB2_CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 00: LOSC
		return allwnrt113_get_losc_freq() / (M * N);
	case 0x01:
		// 01: OSC24M
		return allwnrt113_get_hosc_freq() / (M * N);
	case 0x02:
	case 0x03:
		// 1X: PLL_PERIPH0
		return allwnr_v3s_get_pll_periph0_freq() / (M * N);
	}
}

// V3s
//uint_fast32_t allwnr_v3s_get_apb1_freq(void)
//{
//    return REFINFREQ;	// 24 MHz usually
//}

// V3s
// apbclk
uint_fast32_t allwnrt113_get_uart_freq(void)
{
    return allwnr_v3s_get_apb2_freq();
}

// V3s
// apbclk
uint_fast32_t allwnrt113_get_twi_freq(void)
{
    return allwnr_v3s_get_apb2_freq();
}

/////////////////////

static void clock_set_pll_cpu(uint32_t clk)
{
	int p = 0;
	int k = 1;
	int m = 1;
	uint32_t val;

	if(clk > 1152000000)
	{
		k = 2;
	}
	else if(clk > 768000000)
	{
		k = 3;
		m = 2;
	}

	/* Switch to 24MHz clock while changing cpu pll */
	val = (2 << 0) | (1 << 8) | (1 << 16);
	CCU->CPU_AXI_CFG_REG = val;

	CCU->PLL_CPU_CTRL_REG = 0;
	/* cpu pll rate = ((24000000 * n * k) >> p) / m */
	val = (0x1 << 31);
	val |= ((p & 0x3) << 16);
	val |= ((((clk / (24000000 * k / m)) - 1) & 0x1f) << 8);
	val |= (((k - 1) & 0x3) << 4);
	val |= (((m - 1) & 0x3) << 0);
	CCU->PLL_CPU_CTRL_REG = val;
	//sdelay(200);
	while((CCU->PLL_CPU_CTRL_REG & (1 << 28)) == 0)
		;

	/* Switch clock source */
	val = (2 << 0) | (1 << 8) | (2 << 16);
	CCU->CPU_AXI_CFG_REG = val;
}
///

// V3s
void allwnr_v3s_pll_initialize(void)
{
	clock_set_pll_cpu(1008000000);

	CCU->APB2_CFG_REG =
		0x01 * (UINT32_C(1) << 24) |
		//0x02 * (UINT32_C(1) << 16) |	// N 0..3
		//0x00 * (UINT32_C(1) << 0) |	// M 0..31
		0;
	(void) CCU->APB2_CFG_REG;

	/* pll video - 396MHZ */
	CCU->PLL_VIDEO_CTRL_REG = 0x91004107;
	while((CCU->PLL_VIDEO_CTRL_REG & (1 << 28)) == 0)
		;

	/* pll periph0 - 600MHZ */
	CCU->PLL_PERIPH0_CTRL_REG = 0x90041811;
	while((CCU->PLL_PERIPH0_CTRL_REG & (1 << 28)) == 0)
		;

	/* ahb1 = pll periph0 / 3, apb1 = ahb1 / 2 */
	CCU->AHB_APB0_CFG_REG = 0x00003180;
	(void) CCU->AHB_APB0_CFG_REG;

	/* mbus  = pll periph0 / 4 */
	CCU->MBUS_CLK_REG = 0x81000003;
	(void) CCU->MBUS_CLK_REG;
}

#elif CPUSTYLE_A64

// A64
void set_a64_pll_cpux_axi(unsigned n, unsigned k, unsigned m, unsigned p)
{
	uint32_t val;

	// The PLL Output= (24MHz*N*K)/(M*P)
	//TP();
    //PRINTF("freq = %lu, PLL_CPU_CTRL_REG=%08lX,CPU_AXI_CFG_REG=%08lX\n", allwnrt113_get_pll_cpu_freq(), CCU->PLL_CPU_CTRL_REG, CCU->CPU_AXI_CFG_REG);

	/* Select cpux clock src to osc24m, axi divide ratio is 3, system apb clk ratio is 4 */
	CCU->CPUX_AXI_CFG_REG =
			(1 << 16) | // CPUX_CLK_SRC_SEL 01: OSC24M
			(3 << 8) |	// old 0x03 old CPU_DIV2=4, new same
			(1 << 0) |	// old 0x01 old CPU_DIV1, new same
			0;

	/* Disable pll */
	val = CCU->PLL_CPUX_CTRL_REG;
	val &= ~(UINT32_C(1) << 31);			// PLL_ENABLE
	CCU->PLL_CPUX_CTRL_REG = val;

	/* Set default clk to 1008mhz */
	val = CCU->PLL_CPUX_CTRL_REG;
	val &= ~ ((0x3 << 16) | (0x1f << 8) | (0x3 << 4) | (0x3 << 0));
	val |= ((p - 1) << 16);	// PLL_FACTOR_P PLL_OUT_EXT_DIVP
	val |= ((n - 1) << 8);	// PLL_FACTOR_N
	val |= ((k - 1) << 4);	// PLL_FACTOR_K
	val |= ((m - 1) << 0);	// PLL_FACTOR_M
	CCU->PLL_CPUX_CTRL_REG = val;


	val = CCU->PLL_CPUX_CTRL_REG;
	val &= ~(1 << 29);	// PLL Lock Enable
	CCU->PLL_CPUX_CTRL_REG = val;
	/* Lock enable */
	val = CCU->PLL_CPUX_CTRL_REG;
	val |= (1 << 29);
	CCU->PLL_CPUX_CTRL_REG = val;

	/* Enable pll */
	val = CCU->PLL_CPUX_CTRL_REG;
	val |= (UINT32_C(1) << 31);	// PLL_ENABLE
	CCU->PLL_CPUX_CTRL_REG = val;

	//TP();
	/* Wait pll stable */
	while((CCU->PLL_CPUX_CTRL_REG & (0x1 << 31)) == 0)
		;
	//TP();

	/* Lock disable */
//	val = CCU->PLL_CPU_CTRL_REG;
//	val &= ~(1 << 29);
//	CCU->PLL_CPU_CTRL_REG = val;
	//local_delay_ms(1);

	/* Set and change cpu clk src */
	val = CCU->CPUX_AXI_CFG_REG;
	val &= ~ ((0x3 << 16 ) | ( 0x3 << 8 ) | ( 0xf << 0));
	val |=
		(0x2 << 16) |	// CPUX_CLK_SRC_SEL 1X: PLL_CPUX
		(0x3 << 8) |	// CPU_APB_CLK_DIV
		(0x1 << 0) |	// AXI_CLK_DIV_RATIO
		0;
	CCU->CPUX_AXI_CFG_REG = val;

	//local_delay_ms(1);
	//sys_uart_puts("set_pll_cpux_axi Ok \n");
//	TP();
//    PRINTF("freq = %lu, PLL_CPU_CTRL_REG=%08lX,CPU_AXI_CFG_REG=%08lX\n", allwnrt113_get_pll_cpu_freq(), CCU->PLL_CPU_CTRL_REG, CCU->CPU_AXI_CFG_REG);
}

// A64
static void allwnr_a64_pll_initialize(void)
{
	set_a64_pll_cpux_axi(PLL_CPU_N, PLL_CPU_K, PLL_CPU_M, PLL_CPU_P);	// see sdram.c
}

// A64
static void allwnr_a64_mbus_initialize(void)
{
	unsigned M = 2;
//		CCU->MBUS_CLK_REG |= (UINT32_C(1) << 31);		// MBUS_SCLK_GATING.
	CCU->MBUS_CLK_REG =
		(UINT32_C(1) << 31) | 	// MBUS_SCLK_GATING.
		(0x02 << 24) | 	// MBUS_SCLK_SRC 01: PLL_PERIPH0(2X) 11: PLL_DDR1.
		((M - 1) << 0) | // MBUS_SCLK_RATIO_M (M=1..8, code=0..7)
		0;
	(void) CCU->MBUS_CLK_REG;
	(void) CCU->MBUS_CLK_REG;
	(void) CCU->MBUS_CLK_REG;
	(void) CCU->MBUS_CLK_REG;

//	CCU->MBUS_RST_REG &= ~ (UINT32_C(1) << 31);		// MBUS_RESET.
//	CCU->MBUS_RST_REG |= (UINT32_C(1) << 31);		// MBUS_RESET.
	CCU->MBUS_CLK_REG |= (UINT32_C(1) << 31);		// MBUS_SCLK_GATING.
}

// A64
static void allwnr_a64_module_pll_enable(volatile uint32_t * reg)
{

	if(!(* reg & (UINT32_C(1) << 31)))
	{
		uint32_t val;
		* reg |= (UINT32_C(1) << 31) | (UINT32_C(1) << 30);
		(void) * reg;

		/* Lock enable */
		* reg |= (UINT32_C(1) << 29);
		(void) * reg;

		/* Wait pll stable */
		while(!(* reg & (UINT32_C(1) << 28)))
			;
		//local_delay_ms(20);

		/* Lock disable */
//		val = * reg;
//		val &= ~(1 << 29);
//		* reg = val;
	}
}

// A64
static void allwnr_a64_module_pllaudio_enable(void)
{
	const unsigned p = 5;
	const unsigned n = 64;
	const unsigned m = 25;
	// 0x90035514
	//	CCU->PLL_AUDIO_CTRL_REG =
	//		(0x03 << 16) | // P
	//		(0x3fu << 8) | // N
	//		(0x14 << 0) | // M
	//		0;
	//	The PLL_AUDIO= (24MHz*N)/(M*P).
	//	The PLL_AUDIO(8X) = (24MHz*N*2)/M

	// Need same as PLL_AUDIO1_CTRL_REG in t1113-s3
	//	PLL_AUDIO1 = 24MHz*N/M
	//	PLL_AUDIO1(DIV2) = 24MHz*N/M/P0
	//	PLL_AUDIO1(DIV5) = 24MHz*N/M/P1

	// pll0: p=0x14, n=0x55, m1,m0=0
	// pll1: p1=4(5), p0=1(2), n=0x7F, m=0

	// (24MHz*N)/P must be in the range of 72MHz~504MHz.
	CCU->PLL_AUDIO_CTRL_REG &= ~ (UINT32_C(1) << 31);
	(void) CCU->PLL_AUDIO_CTRL_REG;
	// 307.2
	CCU->PLL_AUDIO_CTRL_REG =
		((p - 1) << 16) | // P The range is from 1 to 16.	- pre-divider
		((n - 1) << 8) | // N 1..128
		((m - 1) << 0) | // M 1..32
		0;
	(void) CCU->PLL_AUDIO_CTRL_REG;


	if(!(CCU->PLL_AUDIO_CTRL_REG & (UINT32_C(1) << 31)))
	{
		uint32_t val;
		CCU->PLL_AUDIO_CTRL_REG |= (UINT32_C(1) << 31) | (UINT32_C(1) << 30);
		(void) CCU->PLL_AUDIO_CTRL_REG;

		/* Lock enable */
		CCU->PLL_AUDIO_CTRL_REG |= (UINT32_C(1) << 29);
		(void) CCU->PLL_AUDIO_CTRL_REG;
		//local_delay_ms(10);

		/* Wait pll stable */
		while(!(CCU->PLL_AUDIO_CTRL_REG & (UINT32_C(1) << 28)))
			;
		//local_delay_ms(20);

		/* Lock disable */
//		val = CCU->PLL_AUDIO_CTRL_REG;
//		val &= ~(1 << 29);
//		CCU->PLL_AUDIO_CTRL_REG = val;
	}
}

// A64
uint_fast32_t allwnrt113_get_hosc_freq(void)
{
    return REFINFREQ;	// 24 MHz usually
}

// A64
uint_fast32_t allwnrt113_get_losc_freq(void)
{
	return LSEFREQ;
}

//val = CCU->PLL_CPUX_CTRL_REG;
//val &= ~ ((0x3 << 16) | (0x1f << 8) | (0x3 << 4) | (0x3 << 0));
//val |= ((p - 1) << 16);	// PLL_FACTOR_P PLL_OUT_EXT_DIVP
//val |= ((n - 1) << 8);	// PLL_FACTOR_N
//val |= ((k - 1) << 4);	// PLL_FACTOR_K
//val |= ((m - 1) << 0);	// PLL_FACTOR_M
//CCU->PLL_CPUX_CTRL_REG = val;

// A64
uint_fast64_t allwnr_a64_get_pll_cpux_freq(void)
{
	const uint_fast32_t reg = CCU->PLL_CPUX_CTRL_REG;
	const uint_fast32_t P = UINT32_C(1) << ((reg >> 16) & 0x03);
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0x1F);
	const uint_fast32_t K = UINT32_C(1) + ((reg >> 4) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((reg >> 0) & 0x03);
	//  (24MHz*N*K)/(M*P)
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N * K / (M * P);
}

// A64
uint_fast64_t allwnr_a64_get_pll_ddr0_freq(void)
{
	const uint_fast32_t pllreg = CCU->PLL_DDR0_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((pllreg >> 8) & 0x1F);	// PLL_FACTOR_N
	const uint_fast32_t K = UINT32_C(1) + ((pllreg >> 4) & 0x03);	// PLL_FACTOR_K
	const uint_fast32_t M = UINT32_C(1) + ((pllreg >> 0) & 0x03);	// PLL_FACTOR_M
	//	PLL_DDR0 = (24MHz*N*K)/M.
	//	The default value of PLL_DDR0 is 432MHz.
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N * K / M;
}

// A64
uint_fast64_t allwnr_a64_get_pll_ddr1_freq(void)
{
	const uint_fast32_t pllreg = CCU->PLL_DDR1_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((pllreg >> 8) & 0x1F);	// PLL_FACTOR_N
	const uint_fast32_t K = UINT32_C(1) + ((pllreg >> 4) & 0x03);	// PLL_FACTOR_K
	const uint_fast32_t M = UINT32_C(1) + ((pllreg >> 0) & 0x03);	// PLL_FACTOR_M
	//	PLL_DDR0 = (24MHz*N*K)/M.
	//	The default value of PLL_DDR0 is 432MHz.
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N * K / M;
}

// A64
uint_fast32_t allwnr_a64_get_cpux_freq(void)
{
	const uint_fast32_t clkreg = CCU->CPUX_AXI_CFG_REG;
	switch ((clkreg >> 16) & 0x03)	/* CPUX_CLK_SRC_SEL */
	{
	case 0x00:
		// 00: LOSC
		return allwnrt113_get_losc_freq();
	case 0x01:
		// 01: OSC24M
		return allwnrt113_get_hosc_freq();
	default:
		// 1X: PLL_CPUX
		return allwnr_a64_get_pll_cpux_freq();
	}
}

// A64
//	The PLL_AUDIO(8X) = (24MHz*N*2)/M
uint_fast32_t allwnr_a64_get_audiopll8x_freq(void)
{
	const uint_fast32_t reg = CCU->PLL_AUDIO_CTRL_REG;
	//const uint_fast32_t P = UINT32_C(1) + ((reg >> 16) & 0x0F);
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0x7F);
	const uint_fast32_t M = UINT32_C(1) + ((reg >> 0) & 0x1F);

	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N * 2 / M;
}

// A64
//	The PLL_AUDIO= (24MHz*N)/(M*P).
uint_fast32_t allwnr_a64_get_audiopll_freq(void)
{
	const uint_fast32_t reg = CCU->PLL_AUDIO_CTRL_REG;
	const uint_fast32_t P = UINT32_C(1) + ((reg >> 16) & 0x0F);
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0x7F);
	const uint_fast32_t M = UINT32_C(1) + ((reg >> 0) & 0x1F);

	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / (M * P);
}

// A64
// The PLL_PERIPH0(2X) should be fixed to 1.2GHz
uint_fast64_t allwnrt113_get_pll_periph0_x2_freq(void)
{
	const uint_fast32_t reg = CCU->PLL_PERIPH0_CTRL_REG;
	//const uint_fast32_t P = UINT32_C(1) + ((reg >> 16) & 0x0F);	// PLL_24M_POST_DIV
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0x1F);	// PLL_FACTOR_N
	const uint_fast32_t K = UINT32_C(1) + ((reg >> 4) & 0x03);	// PLL_FACTOR_K
	//const uint_fast32_t M = UINT32_C(1) + ((reg >> 0) & 0x03);	// PLL_FACTOR_M = PLL Factor M (M = Factor + 1) is only valid in plltest debug.

	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N  * K;
}

// A64
// The PLL_PERIPH1(2X) should be fixed to 1.2GHz
uint_fast64_t allwnrt113_get_pll_periph1_x2_freq(void)
{
	const uint_fast32_t reg = CCU->PLL_PERIPH1_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0x1F);	// PLL_FACTOR_N
	const uint_fast32_t K = UINT32_C(1) + ((reg >> 4) & 0x03);	// PLL_FACTOR_K
	//const uint_fast32_t M = UINT32_C(1) + ((reg >> 0) & 0x03);	// PLL_FACTOR_M - PLL Factor M (M = Factor + 1) is only valid in plltest debug.

	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N  * K;
}

// A64
// The PLL_PERIPH1(2X) should be fixed to 1.2GHz,
uint_fast32_t allwnr_a64_get_pll_hsic_freq(void)
{
	// PLL_HSIC = (24MHz*N)/M.
	const uint_fast32_t reg = CCU->PLL_HSIC_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0x7F);	// PLL_FACTOR_N
	const uint_fast32_t M = UINT32_C(1) + ((reg >> 0) & 0x0F);	// PLL_FACTOR_M - PLL Pre-div Factor(M = Factor+1).

	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N  / M;
}

// A64
uint_fast64_t allwnrt113_get_pll_video0_x2_freq(void)
{
	const uint_fast32_t reg = CCU->PLL_VIDEO0_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0x7F);	// PLL_FACTOR_N
	const uint_fast32_t M = UINT32_C(1) + ((reg >> 0) & 0x0F);	// PLL_FACTOR_M - PLL Pre-div Factor(M = Factor+1).

	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N  / M;
}

// A64
uint_fast64_t allwnrt113_get_pll_video1_x2_freq(void)
{
	const uint_fast32_t reg = CCU->PLL_VIDEO1_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0x7F);	// PLL_FACTOR_N
	const uint_fast32_t M = UINT32_C(1) + ((reg >> 0) & 0x0F);	// PLL_FACTOR_M - PLL Pre-div Factor(M = Factor+1).

	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N  / M;
}

// A64
uint_fast64_t allwnrt113_get_pll_de_freq(void)
{
	const uint_fast32_t reg = CCU->PLL_DE_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0x7F);	// PLL_FACTOR_N
	const uint_fast32_t M = UINT32_C(1) + ((reg >> 0) & 0x0F);	// PLL_FACTOR_M - PLL Pre-div Factor(M = Factor+1).

	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N  / M;
}

// A64
uint_fast64_t allwnrt113_get_pll_periph0_x1_freq(void)
{
	return allwnrt113_get_pll_periph0_x2_freq() / 2;
}

// A64
uint_fast64_t allwnrt113_get_pll_periph1_x1_freq(void)
{
	return allwnrt113_get_pll_periph1_x2_freq() / 2;
}

// A64
uint_fast64_t allwnrt113_get_pll_video0_x1_freq(void)
{
	return allwnrt113_get_pll_video0_x2_freq() / 2;
}

// A64
uint_fast64_t allwnrt113_get_pll_video1_x1_freq(void)
{
	return allwnrt113_get_pll_video1_x2_freq() / 2;
}

// A64
uint_fast64_t allwnrt113_get_pll_mipi_freq(void)
{
	const uint_fast32_t reg = CCU->PLL_MIPI_CTRL_REG;
	const int vfb_sel = (reg >> 16) & 0x01;	// 0: MIPI Mode(N, K, M valid), 1:HDMI Mode(sint_frac,sdiv2,s6p25_7p5 , pll_feedback_div valid)

	if (vfb_sel == 0)
	{
		// PLL_MIPI default value is 594MHz.
		const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0x0F);	// PLL_FACTOR_N
		const uint_fast32_t K = UINT32_C(1) + ((reg >> 4) & 0x03);	// PLL_FACTOR_K
		const uint_fast32_t M = UINT32_C(1) + ((reg >> 0) & 0x0F);	// PLL_FACTOR_M

		return (uint_fast64_t) allwnrt113_get_pll_video0_x1_freq() * N  * K  / M;
	}
	else
	{
		const uint_fast32_t sint_frac = (reg >> 27) & 0x01;	// SINT_FRAC. 0: Integer Mode, 1: Fractional Mode.
		const uint_fast32_t sdiv2 = (reg >> 26) & 0x01;		// SDIV2. 0: PLL Output, 1: PLL Output X2.
		const uint_fast32_t s6p25 = (reg >> 25) & 0x01;		// S6P25. 0: PLL Output=PLL Input*6.25, 1: PLL Output= PLL Input *7.5. PLL Output is selected by this bit when VFB_SEL=1 and SINT_FRAC=1, otherwise no meaning.
		const uint_fast32_t pll_feedback_div = (reg >> 17) & 0x01;		// PLL_FEEDBACK_DIV. 0:Divided by 5, 1:Divided by 7.
		const uint_fast32_t M = pll_feedback_div ? 7 : 5;
		const uint_fast64_t multiplier = sint_frac ? (s6p25 ? 75 : 625) : 1;
		const uint_fast64_t divider = sint_frac ? (s6p25 ? 10 : 100) : 1;

		return (uint_fast64_t) allwnrt113_get_pll_video0_x1_freq() * M * (sdiv2 + 1) * multiplier / divider;
	}
}

// A64
uint_fast32_t allwnr_a64_get_mbus_freq(void)
{
	const uint_fast32_t clkreg = CCU->MBUS_CLK_REG;
	const uint_fast32_t clkdiv = UINT32_C(1) + ((clkreg >> 0) & 0x07);	// MBUS_SCLK_RATIO_M
	switch ((clkreg >> 24) & 0x03)	/* MBUS_SCLK_SRC */
	{
	default:
	case 0x00:
		// 00: 00: OSC24M
		return allwnrt113_get_hosc_freq() / clkdiv;
	case 0x01:
		// 01: PLL_PERIPH0(2X)
		return allwnrt113_get_pll_periph0_x2_freq() / clkdiv;
	case 0x02:
		// 10: PLL_DDR0
		return allwnr_a64_get_pll_ddr0_freq() / clkdiv;
	case 0x03:
		// 11: PLL_DDR1
		return allwnr_a64_get_pll_ddr1_freq() / clkdiv;
	}
}

// A64
static uint_fast32_t allwnr_a64_get_axi_freq(void)
{
	const uint_fast32_t clkreg = CCU->CPUX_AXI_CFG_REG;
	const uint_fast32_t clkdiv = UINT32_C(1) << ((clkreg >> 0) & 0x03);	// AXI_CLK_DIV_RATIO
	return allwnr_a64_get_cpux_freq() / clkdiv;
}

// A64
static uint_fast32_t allwnr_a64_get_apb_freq(void)
{
	const uint_fast32_t clkreg = CCU->CPUX_AXI_CFG_REG;
	const uint_fast32_t clkdiv = UINT32_C(1) << ((clkreg >> 8) & 0x03);	// CPU_APB_CLK_DIV
	return allwnr_a64_get_cpux_freq() / clkdiv;
}

// A64
uint_fast32_t allwnrt113_get_spi0_freq(void)
{
	const uint_fast32_t clkreg = CCU->SPI0_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 16) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	//	00: OSC24M
	//	01: PLL_PERIPH0(1X)
	//	10: PLL_PERIPH1(1X)
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 00: OSC24M
		return allwnrt113_get_hosc_freq() / (M * N);
	case 0x01:
		// 01: PLL_PERIPH0(1X)
		return allwnrt113_get_pll_periph0_x1_freq() / (M * N);
	case 0x02:
		// 10: PLL_PERIPH1(1X)
		return allwnrt113_get_pll_periph1_x1_freq() / (M * N);
	}
}

// A64
uint_fast32_t allwnrt113_get_spi1_freq(void)
{
	const uint_fast32_t clkreg = CCU->SPI1_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 16) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	//	00: OSC24M
	//	01: PLL_PERIPH0(1X)
	//	10: PLL_PERIPH1(1X)
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 00: OSC24M
		return allwnrt113_get_hosc_freq() / (M * N);
	case 0x01:
		// 01: PLL_PERIPH0(1X)
		return allwnrt113_get_pll_periph0_x1_freq() / (M * N);
	case 0x02:
		// 10: PLL_PERIPH1(1X)
		return allwnrt113_get_pll_periph1_x1_freq() / (M * N);
	}
}

// A64
uint_fast32_t allwnr_a64_get_apb2_freq(void)
{
	const uint_fast32_t clkreg = CCU->APB2_CFG_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 16) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x1F);
	//	00: OSC24M
	//	01: PLL_PERIPH0(1X)
	//	10: PLL_PERIPH1(1X)
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 00: LOSC
		return allwnrt113_get_losc_freq() / (M * N);
	case 0x01:
		// 01: OSC24M
		return allwnrt113_get_hosc_freq() / (M * N);
	case 0x02:
	case 0x03:
		// 1X: PLL_PERIPH0(2X)
		return allwnrt113_get_pll_periph0_x2_freq() / (M * N);
	}
}

// a64: not implemented finnaly - only stub
uint_fast32_t allwnr_a64_get_apbs2_freq(void)
{
	const uint_fast32_t clkreg = CCU->APB2_CFG_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 16) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x1F);
	//	00: OSC24M
	//	01: PLL_PERIPH0(1X)
	//	10: PLL_PERIPH1(1X)
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 00: LOSC
		return allwnrt113_get_losc_freq() / (M * N);
	case 0x01:
		// 01: OSC24M
		return allwnrt113_get_hosc_freq() / (M * N);
	case 0x02:
	case 0x03:
		// 1X: PLL_PERIPH0(2X)
		return allwnrt113_get_pll_periph0_x2_freq() / (M * N);
	}
}

// A64
// The clock of the UART is from APB2
uint_fast32_t allwnrt113_get_uart_freq(void)
{
	return allwnr_a64_get_apb2_freq();
}

// A64
// The clock of the TWI is from APB2
uint_fast32_t allwnrt113_get_twi_freq(void)
{
	return allwnr_a64_get_apb2_freq();
}

// A64
// The clock of the TWI is from APB2
uint_fast32_t allwnrt113_get_s_twi_freq(void)
{
	return allwnr_a64_get_apbs2_freq();
}

// A64
uint_fast32_t allwnr_a64_get_hdmi_freq(void)
{
	const uint_fast32_t clkreg = CCU->HDMI_CLK_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 00: PLL_VIDEO0(1X)
		return allwnrt113_get_pll_video0_x1_freq() / M;
	case 0x01:
		// 00: PLL_VIDEO1(1X)
		return allwnrt113_get_pll_video1_x1_freq() / M;
	}
}

// A64
uint_fast32_t allwnrt113_get_tcon0_freq(void)
{
	const uint_fast32_t clkreg = CCU->TCON0_CLK_REG;
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 000: PLL_MIPI
		return allwnrt113_get_pll_mipi_freq();
	case 0x02:
		// 010: PLL_VIDEO0(2X)
		return allwnrt113_get_pll_video0_x2_freq();
	}
}

// A64
uint_fast32_t allwnrt113_get_tcon1_freq(void)
{
	const uint_fast32_t clkreg = CCU->TCON1_CLK_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 00: PLL_VIDEO0(1X)
		return allwnrt113_get_pll_video0_x1_freq() / M;
	case 0x02:
		// 10: PLL_VIDEO1(1X)
		return allwnrt113_get_pll_video1_x1_freq() / M;
	}
}

// A64
uint_fast32_t allwnrt113_get_de_freq(void)
{
	const uint_fast32_t clkreg = CCU->DE_CLK_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 000: PLL_PERIPH0(2X)
		return allwnrt113_get_pll_periph0_x2_freq();
	case 0x02:
		// 010: PLL_DE
		return allwnrt113_get_pll_de_freq();
	}
}

// A64
uint_fast32_t allwnrt113_get_ce_freq(void)
{
	const uint_fast32_t clkreg = CCU->DE_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 16) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 00: OSC24M
		return allwnrt113_get_hosc_freq() / (N * M);
	case 0x01:
		// 01: PLL_PERIPH0（2X）
		return allwnrt113_get_pll_periph0_x2_freq() / (N * M);
	case 0x02:
		// 10: PLL_PERIPH1（2X）
		return allwnrt113_get_pll_periph1_x2_freq() / (N * M);
	}
}

// A64
uint_fast32_t allwnrt113_get_ths_freq(void)
{
	static const unsigned dividers [4] = { 1, 2, 4, 6 };
	const uint_fast32_t clkreg = CCU->THS_CLK_REG;
	const uint_fast32_t divider = dividers [(clkreg >> 0) & 0x03];
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 00: OSC24M
		return allwnrt113_get_hosc_freq() / divider;
	}
}

// A64
uint_fast32_t allwnrt113_get_nand_freq(void)
{
	const uint_fast32_t clkreg = CCU->NAND_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 16) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 00: OSC24M
		return allwnrt113_get_hosc_freq() / (N * M);
	case 0x01:
		// 01: PLL_PERIPH0
		return allwnrt113_get_pll_periph0_x1_freq() / (N * M);
	case 0x02:
		// 10: PLL_PERIPH0
		return allwnrt113_get_pll_periph1_x1_freq() / (N * M);
	}
}

#elif CPUSTYLE_T507

static void set_t507_pll_cpux_axi(unsigned N, unsigned Ppow)
{
	const unsigned APBdiv = 4;
	const unsigned AXIdiv = 2;//3;	// default - 2 = PSI to MEMORY interface

	// switch CPU clock to OSC24M
	CCU->CPUX_AXI_CFG_REG = (CCU->CPUX_AXI_CFG_REG & ~ (UINT32_C(0x07) << 24) & ~ (UINT32_C(0x03) << 8) & ~ (UINT32_C(0x03) << 0)) |
		0x00 * (UINT32_C(1) << 24) |	// OSC24
		(APBdiv - 1) * (UINT32_C(1) << 8) |		// CPUX_APB_FACTOR_N = APB divider
		(AXIdiv - 1) * (UINT32_C(1) << 0) |		// FACTOR_M - AXI divider
		0;
	// Programming PLL
	// Stop PLL
	CCU->PLL_CPUX_CTRL_REG &= ~ (UINT32_C(1) << 31);
	while ((CCU->PLL_CPUX_CTRL_REG & (UINT32_C(1) << 31)) != 0)
		;
	// Set PLL dividers
	CCU->PLL_CPUX_CTRL_REG = (CCU->PLL_CPUX_CTRL_REG & ~ ((UINT32_C(0xFF) << 8) | (UINT32_C(0x03) << 16))) |
		(N - 1) * (UINT32_C(1) << 8) |	// PLL_FACTOR_N
		Ppow * (UINT32_C(1) << 16) |		// PLL_OUT_EXT_DIVP
		0;
	// Start PLL
	CCU->PLL_CPUX_CTRL_REG |= (UINT32_C(1) << 31) | (UINT32_C(1) << 29);
	// Waitig for PLL stable
	while ((CCU->PLL_CPUX_CTRL_REG & (UINT32_C(1) << 28)) == 0)	// LOCK
		;
	// switch CPU clock to PLL_CPUX
	//	CPUX Clock = Clock Source
	//	CPUX_AXI Clock = Clock Source/M
	//	CPUX_APB Clock = Clock Source/N
	CCU->CPUX_AXI_CFG_REG = (CCU->CPUX_AXI_CFG_REG & ~ (UINT32_C(0x07) << 24) & ~ (UINT32_C(0x03) << 8) & ~ (UINT32_C(0x03) << 0)) |
		0x03 * (UINT32_C(1) << 24) |	// 011: PLL_CPUX
		(APBdiv - 1) * (UINT32_C(1) << 8) |		// CPUX_APB_FACTOR_N = APB divider
		(AXIdiv - 1) * (UINT32_C(1) << 0) |		// FACTOR_M - AXI divider
		0;
}


static void allwnr_t507_module_pll_enable(volatile uint32_t * reg)
{

	if(!(* reg & (UINT32_C(1) << 31)))	// PLL_ENABLE
	{
		* reg |= (UINT32_C(1) << 31);	// PLL_ENABLE

		/* Lock enable */
		* reg |= (UINT32_C(1) << 29);	// LOCK_ENABLE

		/* Wait pll stable */
		while(! (* reg & (UINT32_C(1) << 28)))	// LOCK
			;
		//local_delay_ms(20);

		/* Lock disable */
//		val = * reg;
//		val &= ~(1 << 29);
//		* reg = val;
	}
}

void allwnr_t507_pll_initialize(int forced)
{
}

// T507
uint_fast32_t allwnrt113_get_hosc_freq(void)
{
    return REFINFREQ;	// 24 MHz usually
}

// T507
uint_fast32_t allwnr_t507_get_rtc32k_freq(void)
{
    return LSEFREQ;
}

// T507
uint_fast32_t allwnr_t507_get_rc16m_freq(void)
{
    return 16000000u;
}

// T507
uint_fast64_t allwnr_t507_get_pll_cpux_freq(void)
{
	const uint_fast32_t pllreg = CCU->PLL_CPUX_CTRL_REG;
	const uint_fast32_t P = UINT32_C(1) << ((pllreg >> 16) & 0x03);
	const uint_fast32_t N = UINT32_C(1) + ((pllreg >> 8) & 0xFF);
	// PLL_CPUX=24 MHz*N/P
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / P;
}

uint_fast64_t allwnr_t507_get_pll_ddr0_x2_freq(void)
{
	const uint_fast32_t pllreg = CCU->PLL_DDR0_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((pllreg >> 8) & 0xFF);	// PLL_FACTOR_N
	const uint_fast32_t M1 = UINT32_C(1) + ((pllreg >> 1) & 0x01);	// PLL_INPUT_DIV_M1
	const uint_fast32_t M0 = UINT32_C(1) + ((pllreg >> 0) & 0x01);	// PLL_OUTPUT_DIV _M0
	//	PLL_DDR= 24MHz*N/M0/ M1
	//	The default value of PLL_DDR0 is 432MHz.
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / (M0 * M1);
}

uint_fast64_t allwnr_t507_get_pll_ddr1_x2_freq(void)
{
	const uint_fast32_t pllreg = CCU->PLL_DDR1_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((pllreg >> 8) & 0xFF);	// PLL_FACTOR_N
	const uint_fast32_t M1 = UINT32_C(1) + ((pllreg >> 1) & 0x01);	// PLL_INPUT_DIV_M1
	const uint_fast32_t M0 = UINT32_C(1) + ((pllreg >> 0) & 0x01);	// PLL_OUTPUT_DIV _M0
	//	PLL_DDR= 24MHz*N/M0/ M1
	//	The default value of PLL_DDR1 is 432MHz.
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / (M0 * M1);
}

uint_fast64_t allwnr_t507_get_pll_peri0_x2_freq(void)
{
	const uint_fast32_t pllreg = CCU->PLL_PERI0_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((pllreg >> 8) & 0xFF);	// PLL_FACTOR_N
	const uint_fast32_t M1 = UINT32_C(1) + ((pllreg >> 1) & 0x01);	// PLL_INPUT_DIV_M1
	const uint_fast32_t M0 = UINT32_C(1) + ((pllreg >> 0) & 0x01);	// PLL_OUTPUT_DIV _M0
	//	PLL_PERI0(2X) = 24MHz*N/M0/M1
	//	PLL_PERI0(1X) = 24MHz*N/M0/M1/2
	//	The default value of PLL_PERI0(2X) is 1.2 GHz. It is not recomme nded to modify the value
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / (M0 * M1);
}

uint_fast64_t allwnr_t507_get_pll_peri1_x2_freq(void)
{
	const uint_fast32_t pllreg = CCU->PLL_PERI1_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((pllreg >> 8) & 0xFF);	// PLL_FACTOR_N
	const uint_fast32_t M1 = UINT32_C(1) + ((pllreg >> 1) & 0x01);	// PLL_INPUT_DIV_M1
	const uint_fast32_t M0 = UINT32_C(1) + ((pllreg >> 0) & 0x01);	// PLL_OUTPUT_DIV _M0
	//	PLL_PERI1(2X) = 24MHz*N/M0/M1
	//	PLL_PERI1(1X) = 24MHz*N/M0/M1/2
	//	The default value of PLL_PERI1(2X) is 1.2 GHz. It is not recomme nded to modify the value
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / (M0 * M1);
}

uint_fast64_t allwnr_t507_get_pll_gpu0_freq(void)
{
	const uint_fast32_t pllreg = CCU->PLL_GPU0_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((pllreg >> 8) & 0xFF);	// PLL_FACTOR_N
	const uint_fast32_t M1 = UINT32_C(1) + ((pllreg >> 1) & 0x01);	// PLL_INPUT_DIV_M1
	const uint_fast32_t M0 = UINT32_C(1) + ((pllreg >> 0) & 0x01);	// PLL_OUTPUT_DIV _M0
	//	PLL_GPU0 = 24 MHz*N/M0/M1.
	//	The default value of PLL_GPU0 is 432 MHz.
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / (M0 * M1);
}

uint_fast64_t allwnr_t507_get_pll_video0_x4_freq(void)
{
	const uint_fast32_t pllreg = CCU->PLL_VIDEO0_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((pllreg >> 8) & 0xFF);	// PLL_FACTOR_N
	const uint_fast32_t M = UINT32_C(1) + ((pllreg >> 1) & 0x01);	// PLL_INPUT_DIV_M
	//	PLL_VIDEO0(4X)= 24MHz*N/M.
	//	PLL_VIDEO0(1X)=24MHz*N/M/4.
	//	The default value of PLL_VIDEO0(4X) is 1188 MHz.
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / M;
}

uint_fast64_t allwnr_t507_get_pll_video1_x4_freq(void)
{
	const uint_fast32_t pllreg = CCU->PLL_VIDEO1_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((pllreg >> 8) & 0xFF);	// PLL_FACTOR_N
	const uint_fast32_t M = UINT32_C(1) + ((pllreg >> 1) & 0x01);	// PLL_INPUT_DIV_M
	//	PLL_VIDEO1(4X)= 24MHz*N/M.
	//	PLL_VIDEO1(1X)=24MHz*N/M/4.
	//	The default value of PLL_VIDEO1(4X) is 1188 MHz.
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / M;
}

uint_fast64_t allwnr_t507_get_pll_video2_x4_freq(void)
{
	const uint_fast32_t pllreg = CCU->PLL_VIDEO2_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((pllreg >> 8) & 0xFF);	// PLL_FACTOR_N
	const uint_fast32_t M1 = UINT32_C(1) + ((pllreg >> 1) & 0x01);	// PLL_INPUT_DIV_M1
	const uint_fast32_t M0 = UINT32_C(1) + ((pllreg >> 0) & 0x01);	// PLL_OUTPUT_DIV _M0
	//	PLL_VIDEO2(4X)= 24MHz*N/M.
	//	PLL_VIDEO2(1X)=24MHz*N/M/4.
	//	The default value of PLL_VIDEO2(4X) is 1188 MHz.
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / (M0 * M1);
}

uint_fast64_t allwnr_t507_get_pll_video0_x1_freq(void)
{
	return allwnr_t507_get_pll_video0_x4_freq() / 4;
}

uint_fast64_t allwnr_t507_get_pll_video1_x1_freq(void)
{
	return allwnr_t507_get_pll_video1_x4_freq() / 4;
}

uint_fast64_t allwnr_t507_get_pll_ve_freq(void)
{
	const uint_fast32_t pllreg = CCU->PLL_VE_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((pllreg >> 8) & 0xFF);	// PLL_FACTOR_N
	const uint_fast32_t M1 = UINT32_C(1) + ((pllreg >> 1) & 0x01);	// PLL_INPUT_DIV_M1
	const uint_fast32_t M0 = UINT32_C(1) + ((pllreg >> 0) & 0x01);	// PLL_OUTPUT_DIV _M0
	//	The	PLL_VE = 24 MHz*N/M0/M1.
	//	The default value of PLL_VE is 432MHz.
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / (M0 * M1);
}

uint_fast64_t allwnr_t507_get_pll_de_freq(void)
{
	const uint_fast32_t pllreg = CCU->PLL_DE_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((pllreg >> 8) & 0xFF);	// PLL_FACTOR_N
	const uint_fast32_t M1 = UINT32_C(1) + ((pllreg >> 1) & 0x01);	// PLL_INPUT_DIV_M1
	const uint_fast32_t M0 = UINT32_C(1) + ((pllreg >> 0) & 0x01);	// PLL_OUTPUT_DIV _M0
	//	The	PLL_DE = 24 MHz*N/M0/M1.
	//	The default value of PLL_DE is 432MHz.
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / (M0 * M1);
}

uint_fast64_t allwnr_t507_get_pll_csi_freq(void)
{
	const uint_fast32_t pllreg = CCU->PLL_CSI_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((pllreg >> 8) & 0xFF);	// PLL_FACTOR_N
	const uint_fast32_t M1 = UINT32_C(1) + ((pllreg >> 1) & 0x01);	// PLL_INPUT_DIV_M1
	const uint_fast32_t M0 = UINT32_C(1) + ((pllreg >> 0) & 0x01);	// PLL_OUTPUT_DIV _M0
	//	The	PLL_CSI = 24 MHz*N/M0/M1.
	//	The default value of PLL_CSI is 432MHz.
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / (M0 * M1);
}

uint_fast32_t t507_get_pll_audio_4x_freq(void)
{
	const uint_fast32_t pllreg = CCU->PLL_AUDIO_CTRL_REG;
	const uint_fast32_t P = UINT32_C(1) + ((pllreg >> 16) & 0x3F);	// PLL_FACTOR_P
	const uint_fast32_t N = UINT32_C(1) + ((pllreg >> 8) & 0xFF);	// PLL_FACTOR_N
	const uint_fast32_t M1 = UINT32_C(1) + ((pllreg >> 1) & 0x01);	// PLL_INPUT_DIV_M1
	const uint_fast32_t M0 = UINT32_C(1) + ((pllreg >> 0) & 0x01);	// PLL_OUTPUT_DIV _M0
	//	PLL_AUDIO(hs) = 24 MHz*N/M1
	//	PLL_AUDIO(4x) = 24 MHz*N/M0/M1/P
	//	PLL_AUDIO(2X) = 24 MHz*N/ M0/M1/P/2
	//	PLL_AUDIO(1X) =	24 MHz*N/ M0/M1/P/4
	//	The default value of PLL_AUDIO(4X) is 24.5714 MHz.
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / M0 / M1 / P;
}

uint_fast32_t allwnr_t507_get_pll_peri0_x1_freq(void)
{
	return allwnr_t507_get_pll_peri0_x2_freq() / 2;
}

uint_fast64_t allwnr_t507_get_pll_peri1_x1_freq(void)
{
	return allwnr_t507_get_pll_peri1_x2_freq() / 2;
}

uint_fast64_t allwnr_t507_get_pll_ddr0_freq(void)
{
	return allwnr_t507_get_pll_ddr0_x2_freq() / 2;
}

uint_fast64_t allwnr_t507_get_pll_ddr1_freq(void)
{
	return allwnr_t507_get_pll_ddr1_x2_freq() / 2;
}

uint_fast32_t allwnr_t507_get_cpux_freq(void)
{
	const uint_fast32_t clkreg = CCU->CPUX_AXI_CFG_REG;
	//	CPUX Clock = Clock Source
	//	000: OSC24M
	//	001: RTC_32K
	//	010: RC16M
	//	011: PLL_CPUX
	//	100: PLL_PERI0(1X)
	switch ((clkreg >> 24) & 0x07)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 000: OSC24M
		return allwnrt113_get_hosc_freq();
	case 0x01:
		// 001: RTC_32K
		return allwnr_t507_get_rtc32k_freq();
	case 0x02:
		// 010: RC16M
		return allwnr_t507_get_rc16m_freq();
	case 0x03:
		// 011: PLL_CPUX
		return allwnr_t507_get_pll_cpux_freq();
	case 0x04:
		// 100: PLL_PERI0(1X)
		return allwnr_t507_get_pll_peri0_x1_freq();
	}
}

uint_fast32_t allwnr_t507_get_axi_freq(void)
{
	//	CPUX Clock = Clock Source
	//	CPUX_AXI Clock = Clock Source/M
	//	CPUX_APB Clock = Clock Source/N
	const uint_fast32_t clkreg = CCU->CPUX_AXI_CFG_REG;
	const uint_fast32_t N = UINT32_C(1) + ((clkreg >> 8) & 0x03);	// FACTOR_N
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x03);	// FACTOR_M

	return allwnr_t507_get_cpux_freq() / M;
}

uint_fast32_t allwnr_t507_get_apb_freq(void)
{
	//	CPUX Clock = Clock Source
	//	CPUX_AXI Clock = Clock Source/M
	//	CPUX_APB Clock = Clock Source/N
	const uint_fast32_t clkreg = CCU->CPUX_AXI_CFG_REG;
	const uint_fast32_t N = UINT32_C(1) + ((clkreg >> 8) & 0x03);	// FACTOR_N
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x03);	// FACTOR_M

	return allwnr_t507_get_cpux_freq() / N;
}

uint_fast32_t allwnr_t507_get_psi_ahb1_ahb2_freq(void)
{
	const uint_fast32_t clkreg = CCU->PSI_AHB1_AHB2_CFG_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);	// FACTOR_N
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x03);	// FACTOR_M
	const uint_fast32_t divider = N * M;
	//	PSI_AHB1_AHB2_CLK = Clock Source M/N
	//	00: OSC24M
	//	01: RTC_32K
	//	10: RC16M
	//	11: PLL_PERI0(1X)
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 00: OSC24M
		return allwnrt113_get_hosc_freq() / divider;
	case 0x01:
		// 001: RTC_32K
		return allwnr_t507_get_rtc32k_freq() / divider;
	case 0x02:
		// 10: RC16M
		return allwnr_t507_get_rc16m_freq() / divider;
	case 0x03:
		// 11: PLL_PERI0(1X)
		return allwnr_t507_get_pll_peri0_x1_freq() / divider;
	}
}

uint_fast32_t allwnr_t507_get_ahb3_freq(void)
{
	const uint_fast32_t clkreg = CCU->AHB3_CFG_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);	// FACTOR_N
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x03);	// FACTOR_M
	const uint_fast32_t divider = N * M;
	//	AHB3_CLK = Clock Source M/ N
	//	00: OSC24M
	//	01: RTC_32K
	//	10: PSI
	//	11: PLL_PERI0(1X)
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 00: OSC24M
		return allwnrt113_get_hosc_freq() / divider;
	case 0x01:
		// 001: RTC_32K
		return allwnr_t507_get_rtc32k_freq() / divider;
	case 0x02:
		// 10: PSI
		return allwnr_t507_get_psi_ahb1_ahb2_freq() / divider;
	case 0x03:
		// 11: PLL_PERI0(1X)
		return allwnr_t507_get_pll_peri0_x1_freq() / divider;
	}
}

uint_fast32_t allwnr_t507_get_apb1_freq(void)
{
	const uint_fast32_t clkreg = CCU->APB1_CFG_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);	// FACTOR_N
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x03);	// FACTOR_M
	const uint_fast32_t divider = N * M;
	//	APB1_CLK = Clock Source M/ N
	//	00: OSC24M
	//	01: RTC_32K
	//	10: PSI
	//	11: PLL_PERI0(1X)
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 00: OSC24M
		return allwnrt113_get_hosc_freq() / divider;
	case 0x01:
		// 001: RTC_32K
		return allwnr_t507_get_rtc32k_freq() / divider;
	case 0x02:
		// 10: PSI
		return allwnr_t507_get_psi_ahb1_ahb2_freq() / divider;
	case 0x03:
		// 11: PLL_PERI0(1X)
		return allwnr_t507_get_pll_peri0_x1_freq() / divider;
	}
}

uint_fast32_t allwnr_t507_get_apb2_freq(void)
{
	const uint_fast32_t clkreg = CCU->APB2_CFG_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);	// FACTOR_N
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x03);	// FACTOR_M
	const uint_fast32_t divider = N * M;
	//	APB2_CLK = Clock Source M/N
	//	00: OSC24M
	//	01: RTC_32K
	//	10: PSI
	//	11: PLL_PERI0(1X)
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 00: OSC24M
		return allwnrt113_get_hosc_freq() / divider;
	case 0x01:
		// 001: RTC_32K
		return allwnr_t507_get_rtc32k_freq() / divider;
	case 0x02:
		// 10: PSI
		return allwnr_t507_get_psi_ahb1_ahb2_freq() / divider;
	case 0x03:
		// 11: PLL_PERI0(1X)
		return allwnr_t507_get_pll_peri0_x1_freq() / divider;
	}
}

uint_fast32_t allwnr_t507_get_mbus_freq(void)
{
	const uint_fast32_t clkreg = CCU->MBUS_CFG_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x07);	// FACTOR_M
	const uint_fast32_t divider = M;
	//	MBUS_CLK = Clock Source/M.
	//	00: OSC24M
	//	01: PLL_PERI0(2X)
	//	10: PLL_DDR0
	//	11: PLL_DDR1
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 00: OSC24M
		return allwnrt113_get_hosc_freq() / divider;
	case 0x01:
		// 01: PLL_PERI0(2X)
		return allwnr_t507_get_pll_peri0_x2_freq() / divider;
	case 0x02:
		// 10: PLL_DDR0
		return allwnr_t507_get_pll_ddr0_freq() / divider;
	case 0x03:
		// 11: PLL_DDR1
		return allwnr_t507_get_pll_ddr1_freq() / divider;
	}
}

uint_fast32_t allwnr_t507_get_de_freq(void)
{
	const uint_fast32_t clkreg = CCU->DE_CLK_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);	// FACTOR_M
	const uint_fast32_t divider = M;
	//	SCLK = Clock Source/M.
	//	0: 	PLL_DE
	//	1: 	PLL_PERI0(2X)
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 0: 	PLL_DE
		return allwnr_t507_get_pll_de_freq() / divider;
	case 0x01:
		// 1: 	PLL_PERI0(2X)
		return allwnr_t507_get_pll_peri0_x2_freq() / divider;
	}
}

uint_fast32_t allwnr_t507_get_di_freq(void)
{
	const uint_fast32_t clkreg = CCU->DI_CLK_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);	// FACTOR_M
	const uint_fast32_t divider = M;
	//	SCLK = Clock Source/M.
	//	0: 	PLL_DE
	//	1: 	PLL_PERI0(2X)
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 0: 	PLL_DE
		return allwnr_t507_get_pll_de_freq() / divider;
	case 0x01:
		// 1: 	PLL_PERI0(2X)
		return allwnr_t507_get_pll_peri0_x2_freq() / divider;
	}
}

uint_fast32_t allwnr_t507_get_g2d_freq(void)
{
	const uint_fast32_t clkreg = CCU->G2D_CLK_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);	// FACTOR_M
	const uint_fast32_t divider = M;
	//	SCLK = Clock Source/M.
	//	0: 	PLL_DE
	//	1: 	PLL_PERI0(2X)
	switch ((clkreg >> 24) & 0x01)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 0: 	PLL_DE
		return allwnr_t507_get_pll_de_freq() / divider;
	case 0x01:
		// 1: 	PLL_PERI0(2X)
		return allwnr_t507_get_pll_peri0_x2_freq() / divider;
	}
}

uint_fast32_t allwnr_t507_get_ce_freq(void)
{
	const uint_fast32_t clkreg = CCU->CE_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) + ((clkreg >> 8) & 0x03);	// FACTOR_N
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);	// FACTOR_M
	const uint_fast32_t divider = N * M;
	//	SCLK = Clock Source/M/N
	//	0:	OSC24M
	//	1: 	PLL_PERI0(2X)
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 0:	OSC24M
		return allwnrt113_get_hosc_freq() / divider;
	case 0x01:
		// 1: 	PLL_PERI0(2X)
		return allwnr_t507_get_pll_peri0_x2_freq() / divider;
	}
}

uint_fast32_t allwnr_t507_get_ve_freq(void)
{
	const uint_fast32_t clkreg = CCU->VE_CLK_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);	// FACTOR_M
	const uint_fast32_t divider = M;
	//	SCLK = Clock Source/M.
	//	0: 	PLL_VE
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 0: 	PLL_VE
		return allwnr_t507_get_pll_ve_freq() / divider;
	}
}

uint_fast32_t allwnr_t507_get_avs_freq(void)
{
	return allwnrt113_get_hosc_freq();
}

uint_fast32_t t507_get_pll_audio_hs_freq(void)
{
    const uint_fast32_t pllreg = CCU->PLL_AUDIO_CTRL_REG;
    const uint_fast32_t N = UINT32_C(1) + ((pllreg >> 8) & 0xFF);    // PLL_FACTOR_N
    const uint_fast32_t M1 = UINT32_C(1) + ((pllreg >> 1) & 0x01);    // PLL_INPUT_DIV_M1
    //    PLL_AUDIO(hs) = 24 MHz*N/M1
    //    PLL_AUDIO(4x) = 24 MHz*N/M0/M1/P
    //    PLL_AUDIO(2X) = 24 MHz*N/ M0/M1/P/2
    //    PLL_AUDIO(1X) =    24 MHz*N/ M0/M1/P/4
    //    The default value of PLL_AUDIO(4X) is 24.5714 MHz.
    return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / M1;
}

uint_fast32_t t507_get_pll_audio_2x_freq(void)
{
	return t507_get_pll_audio_4x_freq() / 2;
}

uint_fast32_t t507_get_pll_audio_1x_freq(void)
{
	return t507_get_pll_audio_4x_freq() / 4;
}

uint_fast32_t allwnr_t507_get_audio_codec_1x_freq(void)
{
	const uint_fast32_t clkreg = CCU->AUDIO_CODEC_1X_CLK_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	// SCLK = Clock Source/M.
	//	Clock Source Select
	//	00: PLL_AUDIO(1X)
	//	01: PLL_AUDIO(2X)
	//	10: PLL_AUDIO(4X)
	//	11: PLL_AUDIO(hs)
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:	/* 00: PLL_AUDIO(1X) */
		return t507_get_pll_audio_1x_freq() / M;
	case 0x01:	/* 01: PLL_AUDIO(2X) */
		return t507_get_pll_audio_2x_freq() / M;
	case 0x02: 	/* 10: PLL_AUDIO(4X) */
		return t507_get_pll_audio_4x_freq() / M;
	case 0x03: 	/* 11: PLL_AUDIO(hs) */
		return t507_get_pll_audio_hs_freq() / M;
	}
}

uint_fast32_t allwnr_t507_get_ahub_freq(void)
{
	const uint_fast32_t clkreg = CCU->AUDIO_HUB_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);	// FACTOR_N
	const uint_fast32_t divider = N;
	// SCLK = Clock Source/N.
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 00: PLL_AUDIO(1X)
		return t507_get_pll_audio_1x_freq() / divider;
	case 0x01:
		// 01: PLL_AUDIO(2X)
		return t507_get_pll_audio_2x_freq() / divider;
	case 0x02:
		// 10: PLL_AUDIO(4X)
		return t507_get_pll_audio_4x_freq() / divider;
	case 0x03:
		// 11: PLL_AUDIO(hs)
		return t507_get_pll_audio_hs_freq() / divider;
	}
}

uint_fast32_t allwnr_t507_get_audio_codec_4x_freq(void)
{
	const uint_fast32_t clkreg = CCU->AUDIO_CODEC_4X_CLK_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	// SCLK = Clock Source/M.
	//	Clock Source Select
	//	00: PLL_AUDIO(1X)
	//	01: PLL_AUDIO(2X)
	//	10: PLL_AUDIO(4X)
	//	11: PLL_AUDIO(hs)
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:	/* 00: PLL_AUDIO(1X) */
		return t507_get_pll_audio_1x_freq() / M;
	case 0x01:	/* 01: PLL_AUDIO(2X) */
		return t507_get_pll_audio_2x_freq() / M;
	case 0x02: 	/* 10: PLL_AUDIO(4X) */
		return t507_get_pll_audio_4x_freq() / M;
	case 0x03: 	/* 11: PLL_AUDIO(hs) */
		return t507_get_pll_audio_hs_freq() / M;
	}
}

uint_fast32_t allwnr_t507_get_dram_freq(void)
{
	const uint_fast32_t clkreg = CCU->DRAM_CLK_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);	// FACTOR_M
	const uint_fast32_t divider = M;
	//	SCLK = Clock Source/M.
	//	00 : PLL_DDR0
	//	01 : PLL_DDR1
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 00 : PLL_DDR0
		return allwnr_t507_get_pll_ddr0_freq() / divider;
	case 0x01:
		// 01 : PLL_DDR1
		return allwnr_t507_get_pll_ddr1_freq() / divider;
	}
}

uint_fast32_t allwnr_t507_get_tcon_lcd0_freq(void)
{
	const uint_fast32_t clkreg = CCU->TCON_LCD0_CLK_REG;
	switch ((clkreg >> 24) & 0x07)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 000: PLL_VIDEO0(1X)
		return allwnr_t507_get_pll_video0_x1_freq();
	case 0x01:
		// 001: PLL_VIDEO0(4X)
		return allwnr_t507_get_pll_video0_x4_freq();
	case 0x02:
		// 010: PLL_VIDEO1(1X)
		return allwnr_t507_get_pll_video1_x1_freq();
	case 0x03:
		// 011: PLL_VIDEO1(4X)
		return allwnr_t507_get_pll_video1_x4_freq();
	}
}

uint_fast32_t allwnr_t507_get_tcon_lcd1_freq(void)
{
	const uint_fast32_t clkreg = CCU->TCON_LCD1_CLK_REG;
	switch ((clkreg >> 24) & 0x07)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 000: PLL_VIDEO0(1X)
		return allwnr_t507_get_pll_video0_x1_freq();
	case 0x01:
		// 001: PLL_VIDEO0(4X)
		return allwnr_t507_get_pll_video0_x4_freq();
	case 0x02:
		// 010: PLL_VIDEO1(1X)
		return allwnr_t507_get_pll_video1_x1_freq();
	case 0x03:
		// 011: PLL_VIDEO1(4X)
		return allwnr_t507_get_pll_video1_x4_freq();
	}
}

uint_fast32_t allwnr_t507_get_tcon_tv0_freq(void)
{
	const uint_fast32_t clkreg = CCU->TCON_TV0_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);	// FACTOR_N
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);	// FACTOR_M
	const uint_fast32_t pgdiv = M * N;
	// SCLK = Clock Source/M/N
	switch ((clkreg >> 24) & 0x07)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 000: PLL_VIDEO0(1X)
		return allwnr_t507_get_pll_video0_x1_freq() / pgdiv;
	case 0x01:
		// 001: PLL_VIDEO0(4X)
		return allwnr_t507_get_pll_video0_x4_freq() / pgdiv;
	case 0x02:
		// 010: PLL_VIDEO1(1X)
		return allwnr_t507_get_pll_video1_x1_freq() / pgdiv;
	case 0x03:
		// 011: PLL_VIDEO1(4X)
		return allwnr_t507_get_pll_video1_x4_freq() / pgdiv;
	}
}

uint_fast32_t allwnr_t507_get_tcon_tv1_freq(void)
{
	const uint_fast32_t clkreg = CCU->TCON_TV1_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);	// FACTOR_N
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);	// FACTOR_M
	const uint_fast32_t pgdiv = M * N;
	// SCLK = Clock Source/M/N
	switch ((clkreg >> 24) & 0x07)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 000: PLL_VIDEO0(1X)
		return allwnr_t507_get_pll_video0_x1_freq() / pgdiv;
	case 0x01:
		// 001: PLL_VIDEO0(4X)
		return allwnr_t507_get_pll_video0_x4_freq() / pgdiv;
	case 0x02:
		// 010: PLL_VIDEO1(1X)
		return allwnr_t507_get_pll_video1_x1_freq() / pgdiv;
	case 0x03:
		// 011: PLL_VIDEO1(4X)
		return allwnr_t507_get_pll_video1_x4_freq() / pgdiv;
	}
}

uint_fast32_t allwnr_t507_get_pll_peri_bak_freq(void)
{
	const uint_fast32_t clkreg = CCU->GPU_CLK1_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x03);	// FACTOR_M
//	PLL_PERI_BAK = Clock M
//	Clock Source is from PLL_PERI0(2X)
	return allwnr_t507_get_pll_peri0_x2_freq() / M;
}

uint_fast32_t allwnr_t507_get_gpu_freq(void)
{
	const uint_fast32_t clkreg = CCU->GPU_CLK0_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x03);	// FACTOR_M
	switch ((clkreg >> 24) & 0x01)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:
		// 0: PLL_GPU0
		return allwnr_t507_get_pll_gpu0_freq() / M;
	case 0x01:
		// 1: PLL_PERI_BAK_CLK(PLL_PERI_BAK_CLK is from GPU_CLK1_REG)
		return allwnr_t507_get_pll_peri_bak_freq() / M;
	}
}

// T507
uint_fast32_t allwnrt113_get_spi0_freq(void)
{
	const uint_fast32_t clkreg = CCU->SPI0_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	// SCLK = Clock Source/M/N.
	const uint_fast32_t pgdiv = M * N;
	switch ((clkreg >> 24) & 0x07)
	{
	default:
	case 0x00:
		/* 000: HOSC */
		return allwnrt113_get_hosc_freq() / pgdiv;
	case 0x01:
		/* 001: PLL_PERI0(1X) */
		return allwnr_t507_get_pll_peri0_x1_freq() / pgdiv;
	case 0x02:
		/* 010: PLL_PERI1(1X) */
		return allwnr_t507_get_pll_peri0_x1_freq() / pgdiv;
	case 0x03:
		/* 011: PLL_PERI0(2X) */
		return allwnr_t507_get_pll_peri0_x2_freq() / pgdiv;
	case 0x04:
		/* 100: PLL_PERI1(2X) */
		return allwnr_t507_get_pll_peri1_x2_freq() / pgdiv;
	}
}

// T507
uint_fast32_t allwnrt113_get_spi1_freq(void)
{
	const uint_fast32_t clkreg = CCU->SPI1_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	// SCLK = Clock Source/M/N.
	const uint_fast32_t pgdiv = M * N;
	switch ((clkreg >> 24) & 0x07)
	{
	default:
	case 0x00:
		/* 000: HOSC */
		return allwnrt113_get_hosc_freq() / pgdiv;
	case 0x01:
		/* 001: PLL_PERI0(1X) */
		return allwnr_t507_get_pll_peri0_x1_freq() / pgdiv;
	case 0x02:
		/* 010: PLL_PERI1(1X) */
		return allwnr_t507_get_pll_peri0_x1_freq() / pgdiv;
	case 0x03:
		/* 011: PLL_PERI0(2X) */
		return allwnr_t507_get_pll_peri0_x2_freq() / pgdiv;
	case 0x04:
		/* 100: PLL_PERI1(2X) */
		return allwnr_t507_get_pll_peri1_x2_freq() / pgdiv;
	}
}

// The working clock of UART is APB2
uint_fast32_t allwnrt113_get_uart_freq(void)
{
     return allwnr_t507_get_apb2_freq();
}

// The working clock of TWI is APB2.
uint_fast32_t allwnrt113_get_twi_freq(void)
{
	return allwnr_t507_get_apb2_freq();
}

// The working clock of S_TWI is APBS2.
uint_fast32_t allwnrt113_get_s_twi_freq(void)
{
	return allwnr_t507_get_apb2_freq();
	//return allwnr_t507_get_apbs2_freq();
}

// T507
uint_fast32_t allwnrt113_get_smhc0_freq(void)
{
	const uint_fast32_t clkreg = CCU->SMHC0_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	// SMHC0_CLK = Clock Source/M/N.
	const uint_fast32_t pgdiv = M * N;	// post-gate dividers: clkdiv4 and clkdiv2y
	switch ((clkreg >> 24) & 0x03)
	{
	default:
	case 0x00:
		/* 00: HOSC */
		return allwnrt113_get_hosc_freq() / pgdiv;
	case 0x01:
		/* 01: PLL_PERI0(2X) */
		return allwnr_t507_get_pll_peri0_x2_freq() / pgdiv;
	case 0x02:
		/* 01: PLL_PERI1(2X) */
		return allwnr_t507_get_pll_peri1_x2_freq() / pgdiv;
	}
}

// T507
uint_fast32_t allwnrt113_get_smhc1_freq(void)
{
	const uint_fast32_t clkreg = CCU->SMHC1_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	// SMHC0_CLK = Clock Source/M/N.
	const uint_fast32_t pgdiv = M * N;	// post-gate dividers: clkdiv4 and clkdiv2y
	switch ((clkreg >> 24) & 0x03)
	{
	default:
	case 0x00:
		/* 00: HOSC */
		return allwnrt113_get_hosc_freq() / pgdiv;
	case 0x01:
		/* 01: PLL_PERI0(2X) */
		return allwnr_t507_get_pll_peri0_x2_freq() / pgdiv;
	case 0x02:
		/* 01: PLL_PERI1(2X) */
		return allwnr_t507_get_pll_peri1_x2_freq() / pgdiv;
	}
}

// T507
uint_fast32_t allwnrt113_get_smhc2_freq(void)
{
	const uint_fast32_t clkreg = CCU->SMHC2_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	// SMHC0_CLK = Clock Source/M/N.
	const uint_fast32_t pgdiv = M * N;	// post-gate dividers: clkdiv4 and clkdiv2y
	switch ((clkreg >> 24) & 0x03)
	{
	default:
	case 0x00:
		/* 00: HOSC */
		return allwnrt113_get_hosc_freq() / pgdiv;
	case 0x01:
		/* 01: PLL_PERI0(2X) */
		return allwnr_t507_get_pll_peri0_x2_freq() / pgdiv;
	case 0x02:
		/* 01: PLL_PERI1(2X) */
		return allwnr_t507_get_pll_peri1_x2_freq() / pgdiv;
	}
}


#elif CPUSTYLE_T113 || CPUSTYLE_F133

static void set_pll_cpu(unsigned n)
{
	uint32_t val;

	/* Disable pll gating */
	val = CCU->PLL_CPU_CTRL_REG;
	val &= ~ (UINT32_C(1) << 27);
	CCU->PLL_CPU_CTRL_REG = val;

	/* Enable pll ldo */
	val = CCU->PLL_CPU_CTRL_REG;
	val |= (UINT32_C(1) << 30);
	CCU->PLL_CPU_CTRL_REG = val;

	/* Set default clk to 1008mhz */
	val = CCU->PLL_CPU_CTRL_REG;
	val &= ~ ((0x3u << 16) | (0xffu << 8) | (0x3u << 0));
	val |= ((n - 1) << 8);
	CCU->PLL_CPU_CTRL_REG = val;

	val = CCU->PLL_CPU_CTRL_REG;
	val &= ~ (UINT32_C(1) << 29);	// PLL Lock Enable
	CCU->PLL_CPU_CTRL_REG = val;
	/* Lock enable */
	val = CCU->PLL_CPU_CTRL_REG;
	val |= (UINT32_C(1) << 29);
	CCU->PLL_CPU_CTRL_REG = val;

	/* Enable pll */
	val = CCU->PLL_CPU_CTRL_REG;
	val |= (UINT32_C(1) << 31);
	CCU->PLL_CPU_CTRL_REG = val;

	//TP();
	/* Wait pll stable */
	while((CCU->PLL_CPU_CTRL_REG & (UINT32_C(1) << 28)) == 0)
		;
	//TP();

	/* Enable pll gating */
	val = CCU->PLL_CPU_CTRL_REG;
	val |= (UINT32_C(1) << 27);
	CCU->PLL_CPU_CTRL_REG = val;

	/* Lock disable */
//	val = CCU->PLL_CPU_CTRL_REG;
//	val &= ~ (UINT32_C(1) << 29);
//	CCU->PLL_CPU_CTRL_REG = val;
//	//local_delay_ms(1);
}
//#if CPUSTYLE_F133

void set_pll_riscv_axi(unsigned n)
{
	uint32_t val;

	// After ddr3_init
	// PLL_CPU_CTRL_REG=CA002900
	// CPU_AXI_CFG_REG=03000301
	//TP();
    //PRINTF("freq = %lu, PLL_CPU_CTRL_REG=%08lX,CPU_AXI_CFG_REG=%08lX\n", allwnrt113_get_pll_cpu_freq(), CCU->PLL_CPU_CTRL_REG, CCU->CPU_AXI_CFG_REG);

	/* Select cpux clock src to osc24m, axi divide ratio is 3, system apb clk ratio is 4 */
	CCU->RISC_CLK_REG =
			(0 << 24) | // 000: HOSC
			(1 << 8) |	// RISC_AXI_DIV_CFG
			(0 << 0) |	// RISC_DIV_CFG
			0;
	(void) CCU->RISC_CLK_REG;

	set_pll_cpu(n);

	/* Set and change cpu clk src */
	val = CCU->RISC_CLK_REG;
	val &= ~ ((0x07 << 24) | ( 0x3 << 8 ) | ( 0x1F << 0));
	val |=
		(0x05 << 24) |	// 101: PLL_CPU
		(0x1 << 8) |	// RISC_AXI_DIV_CFG
		(0x0 << 0) |	// RISC_DIV_CFG
		0;
	CCU->RISC_CLK_REG = val;

	//local_delay_ms(1);
	//sys_uart_puts("set_pll_cpux_axi Ok \n");
//	TP();
//    PRINTF("freq = %lu, PLL_CPU_CTRL_REG=%08lX,CPU_AXI_CFG_REG=%08lX\n", allwnrt113_get_pll_cpu_freq(), CCU->PLL_CPU_CTRL_REG, CCU->CPU_AXI_CFG_REG);
}
//#endif /* CPUSTYLE_F133 */

//#if CPUSTYLE_T113
void set_pll_cpux_axi(unsigned n)
{
	uint32_t val;

	// After ddr3_init
	// PLL_CPU_CTRL_REG=CA002900
	// CPU_AXI_CFG_REG=03000301
	//TP();
    //PRINTF("freq = %lu, PLL_CPU_CTRL_REG=%08lX,CPU_AXI_CFG_REG=%08lX\n", allwnrt113_get_pll_cpu_freq(), CCU->PLL_CPU_CTRL_REG, CCU->CPU_AXI_CFG_REG);

	/* Select cpux clock src to osc24m, axi divide ratio is 3, system apb clk ratio is 4 */
	CCU->CPU_AXI_CFG_REG =
			(0 << 24) | // old 0x03, old 011: PLL_CPU/P, new 000: HOSC
			(3 << 8) |	// old 0x03 old CPU_DIV2=4, new same
			(1 << 0) |	// old 0x01 old CPU_DIV1, new same
			0;

	set_pll_cpu(n);

	/* Lock disable */
//	val = CCU->PLL_CPU_CTRL_REG;
//	val &= ~(1 << 29);
//	CCU->PLL_CPU_CTRL_REG = val;
	//local_delay_ms(1);

	/* Set and change cpu clk src */
	val = CCU->CPU_AXI_CFG_REG;
	val &= ~ ((0x07 << 24) | (0x3 << 16 ) | ( 0x3 << 8 ) | ( 0xf << 0));
	val |=
		(0x03 << 24) |
		(0x0 << 16) |	// PLL_CPU_OUT_EXT_DIVP
		(0x3 << 8) |
		(0x1 << 0) |
		0;
	CCU->CPU_AXI_CFG_REG = val;

	//local_delay_ms(1);
	//sys_uart_puts("set_pll_cpux_axi Ok \n");
//	TP();
//    PRINTF("freq = %lu, PLL_CPU_CTRL_REG=%08lX,CPU_AXI_CFG_REG=%08lX\n", allwnrt113_get_pll_cpu_freq(), CCU->PLL_CPU_CTRL_REG, CCU->CPU_AXI_CFG_REG);
}
//#endif /* CPUSTYLE_T113 */

static void set_pll_periph0(void)
{
	uint32_t val;

	/* Periph0 has been enabled */
	if (CCU->PLL_PERI_CTRL_REG & (UINT32_C(1) << 31))
	{
		//sys_uart_puts("PLL_PERI_REG = ");
		//sys_uart_send_u32_t(read32(T113_CCU_BASE + CCU_PLL_PERI_CTRL_REG));
		//sys_uart_puts("\n");
		return;
	}

	/* Change psi src to osc24m */
	val = CCU->PSI_CLK_REG;
	val &= (~(0x3 << 24));
	CCU->PSI_CLK_REG = val;

	/* Set default val */
	CCU->PLL_PERI_CTRL_REG = 0x63 << 8;

	/* Lock enable */
	val = CCU->PLL_PERI_CTRL_REG;
	val |= (1 << 29);
	CCU->PLL_PERI_CTRL_REG = val;

	/* Enabe pll 600m(1x) 1200m(2x) */
	val = CCU->PLL_PERI_CTRL_REG;
	val |= (UINT32_C(1) << 31);
	CCU->PLL_PERI_CTRL_REG = val;

	/* Wait pll stable */
	while(!(CCU->PLL_PERI_CTRL_REG & (0x1 << 28)))
		;
	local_delay_ms(20);

	/* Lock disable */
	val = CCU->PLL_PERI_CTRL_REG;
	val &= ~(1 << 29);
	CCU->PLL_PERI_CTRL_REG = val;

	//sys_uart_puts("set_pll_periph0 Ok\n");
	//sys_uart_puts("PLL_PERI_REG = ");
	//sys_uart_send_u32_t(read32(T113_CCU_BASE + CCU_PLL_PERI_CTRL_REG));
	//sys_uart_puts("\n");
}

static void set_ahb(void)
{
#if 1
	// 300 MHz
	CCU->PSI_CLK_REG =
		(0x03 << 24) |
		(1 << 8) |			// N = 1
		((1 - 1) << 0) |	// M (1..4)
		0;
	(void) CCU->PSI_CLK_REG;
#else
	// 200 MHz
	CCU->PSI_CLK_REG =
		(0x03 << 24) |
		(0 << 8) |			// N = 1
		((3 - 1) << 0) |	// M (1..4)
		0;
	(void) CCU->PSI_CLK_REG;
#endif
}

static void set_apb(void)
{
	CCU->APB0_CLK_REG = (2 << 0) | (1 << 8);
	CCU->APB0_CLK_REG |= (0x03 << 24);
	(void) CCU->APB0_CLK_REG;

	// UARTx
	CCU->APB1_CLK_REG = (2 << 0) | (1 << 8);
	CCU->APB1_CLK_REG |= (0x03 << 24);	/* 11: PLL_PERI(1X) */
	//CCU->APB1_CLK_REG |= (0x02 << 24);	/* 10: PSI_CLK */
	(void) CCU->APB1_CLK_REG;

/*
	sys_uart_puts("APB0_CLK_REG = ");
	sys_uart_send_u32_t(read32(T113_CCU_BASE + CCU_APB0_CLK_REG));
	sys_uart_puts("\n");

	sys_uart_puts("APB1_CLK_REG = ");
	sys_uart_send_u32_t(read32(T113_CCU_BASE + CCU_APB1_CLK_REG));
	sys_uart_puts("\n");*/
}

static void set_dma(void)
{
	CCU->DMA_BGR_REG |= (UINT32_C(1) << 0);			// DMA_GATING 1: Pass clock

	CCU->DMA_BGR_REG &= ~ (UINT32_C(1) << 16);
	CCU->DMA_BGR_REG |= (UINT32_C(1) << 16);
}

static void set_mbus(void)
{
	/* Reset mbus domain */
//	CCU->MBUS_CLK_REG &= ~ (UINT32_C(1) << 30);				// MBUS Reset 1: Assert reset
//	(void) CCU->MBUS_CLK_REG;
	CCU->MBUS_CLK_REG |= (UINT32_C(1) << 30);				// MBUS Reset 1: De-assert reset
	(void) CCU->MBUS_CLK_REG;
	/* Enable mbus master clock gating */
//	CCU->MBUS_MAT_CLK_GATING_REG = 0x00000d87;
#if (CPUSTYLE_F133 || CPUSTYLE_T113_S4)
	CCU->MBUS_MAT_CLK_GATING_REG |= (UINT32_C(1) << 11);				// RISC-V_MCLK_EN
#endif /* (CPUSTYLE_F133 || CPUSTYLE_T113_S4) */
}

static void allwnrt113_module_pll_enable(volatile uint32_t * reg)
{

	if(!(* reg & (UINT32_C(1) << 31)))
	{
		uint32_t val;
		* reg |= (UINT32_C(1) << 31) | (UINT32_C(1) << 30);

		/* Lock enable */
		* reg |= (UINT32_C(1) << 29);

		/* Wait pll stable */
		while(!(* reg & (0x1u << 28)))
			;
		//local_delay_ms(20);

		/* Lock disable */
//		val = * reg;
//		val &= ~(1 << 29);
//		* reg = val;
	}
}

void allwnrt113_set_pll_cpux(unsigned m, unsigned n)
{
	// PLL_CPU = InputFreq*N.
	ASSERT(m == 2);
	uint_fast32_t val = CCU->PLL_CPU_CTRL_REG;

	/* Set default clk to 1008mhz */
	val = CCU->PLL_CPU_CTRL_REG;
	val &= ~ ((0xffu << 8) | (0x3u << 0));
	val |= ((n - 1) << 8);		//was: PLL_CPU_N
	val |= ((m - 1) << 0);

	CCU->PLL_CPU_CTRL_REG = val;
}
//
//void allwnrt113_set_pll_ddr(unsigned m, unsigned n)
//{
//	// PLL_DDR = InputFreq*N/M1/M0.
//	uint_fast32_t reg = CCU->PLL_DDR_CTRL_REG;
//
//}
//
//void allwnrt113_set_pll_peri(unsigned m, unsigned n)
//{
//	uint_fast32_t reg = CCU->PLL_PERI_CTRL_REG;
//
//}
//
//void allwnrt113_set_pll_vieo0(unsigned m, unsigned n)
//{
//	uint_fast32_t reg = CCU->PLL_VIDEO0_CTRL_REG;
//
//}
//
//void allwnrt113_set_pll_vieo1(unsigned m, unsigned n)
//{
//	uint_fast32_t reg = CCU->PLL_VIDEO1_CTRL_REG;
//
//}
//
//void allwnrt113_set_pll_ve(unsigned m, unsigned n)
//{
//	uint_fast32_t reg = CCU->PLL_VE_CTRL_REG;
//
//}
//
//void allwnrt113_set_pll_audio0(unsigned m, unsigned n)
//{
//	uint_fast32_t reg = CCU->PLL_AUDIO0_CTRL_REG;
//
//}
//
//void allwnrt113_set_pll_audio1(unsigned m, unsigned n)
//{
//	uint_fast32_t reg = CCU->PLL_AUDIO1_CTRL_REG;
//
//}

uint_fast32_t allwnrt113_get_hosc_freq(void)
{
    return REFINFREQ;	// 24 MHz usually
}

// T113-s3
uint_fast32_t allwnrt113_get_16M_freq(void)
{
    return HARDWARE_CLK16M_RC_FREQ;	// 16 MHz RC
}

// T113-s3
uint_fast32_t allwnrt113_get_32k_freq(void)
{
	return allwnrt113_get_hosc_freq() / 750;	// Найдено на блок-схеме CCU
}

// T113-s3
// unused
uint_fast32_t allwnrt113_get_losc_freq(void)
{
	const uint_fast32_t reg = RTC->LOSC_CTRL_REG;
	//	LOSC_SRC_SEL
	//	LOSC Clock Source Select
	//	0: Low frequency clock from 16M RC
	//	1: External 32.768 kHz OSC
	return (reg & 0x01) ? LSEFREQ : HARDWARE_CLK16M_RC_FREQ;
}

uint_fast64_t allwnrt113_get_pll_cpu_freq(void)
{
	// PLL_CPU = InputFreq*N
	const uint_fast32_t reg = CCU->PLL_CPU_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0xFF);
	const uint_fast32_t M = UINT32_C(1) + ((reg >> 0) & 0x03);
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / M;
}

uint_fast64_t allwnrt113_get_pll_ddr_freq(void)
{
	// PLL_DDR = InputFreq*N/M1/M0
	const uint_fast32_t reg = CCU->PLL_DDR_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0xFF);
	const uint_fast32_t M1 = UINT32_C(1) + ((reg >> 1) & 0x01);	// PLL input divider
	const uint_fast32_t M0 = UINT32_C(1) + ((reg >> 0) & 0x01);	// PLL outpur divider
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / M1 / M0;
}

// T113-s3
//	PLL_PERI(2X) = 24 MHz*N/M/P0
//	PLL_PERI(1X) = 24 MHz*N/M/P0/2
//	PLL_PERI(800M) = 24 MHz*N/M/P1
uint_fast64_t allwnrt113_get_pll_peri_freq(void)
{
	const uint_fast32_t reg = CCU->PLL_PERI_CTRL_REG;
	const uint_fast32_t P1 = UINT32_C(1) + ((reg >> 20) & 0x07);
	const uint_fast32_t P0 = UINT32_C(1) + ((reg >> 16) & 0x07);
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0xFF);
	const uint_fast32_t M = UINT32_C(1) + ((reg >> 1) & 0x01);
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / M;
}

// T113-s3
//	PLL_PERI(2X) = 24 MHz*N/M/P0
//	PLL_PERI(1X) = 24 MHz*N/M/P0/2
//	PLL_PERI(800M) = 24 MHz*N/M/P1
// The output clock of PLL_PERI(2X) is fixed to 1.2 GHz and not suggested to change the parameter.
uint_fast32_t allwnrt113_get_peripll2x_freq(void)
{
	const uint_fast32_t reg = CCU->PLL_PERI_CTRL_REG;
	const uint_fast32_t P1 = UINT32_C(1) + ((reg >> 20) & 0x07);
	const uint_fast32_t P0 = UINT32_C(1) + ((reg >> 16) & 0x07);
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0xFF);
	const uint_fast32_t M = UINT32_C(1) + ((reg >> 1) & 0x01);
	return allwnrt113_get_pll_peri_freq() / P0;
}

// T113-s3
//	PLL_PERI(2X) = 24 MHz*N/M/P0
//	PLL_PERI(1X) = 24 MHz*N/M/P0/2
//	PLL_PERI(800M) = 24 MHz*N/M/P1
uint_fast32_t allwnrt113_get_pll_peri_800M_freq(void)
{
	const uint_fast32_t reg = CCU->PLL_PERI_CTRL_REG;
	const uint_fast32_t P1 = UINT32_C(1) + ((reg >> 20) & 0x07);
	return allwnrt113_get_pll_peri_freq() / P1;
}

uint_fast32_t allwnrt113_get_peripll1x_freq(void)
{
	return allwnrt113_get_peripll2x_freq() / 2;
}

// When the HOSC is 24 MHz, the default frequency of PLL_VIDEO0(4X) is 1188 MHz.
uint_fast32_t allwnrt113_get_video0pllx4_freq(void)
{
	const uint_fast32_t reg = CCU->PLL_VIDEO0_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0xFF);	// >= 12
	const uint_fast32_t M1 = UINT32_C(1) + ((reg >> 1) & 0x01);	// Input divider
	//const uint_fast32_t M0 = UINT32_C(1) + ((reg >> 0) & 0x01);	// PLL Output Div D.(The factor is only for testing)
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / M1;
}

// When the HOSC is 24 MHz, the default frequency of PLL_VIDEO1(4X) is 1188 MHz.
uint_fast32_t allwnrt113_get_video1pllx4_freq(void)
{
	const uint_fast32_t reg = CCU->PLL_VIDEO1_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0xFF);	// >= 12
	const uint_fast32_t M1 = UINT32_C(1) + ((reg >> 1) & 0x01);	// Input divider
	//const uint_fast32_t M0 = UINT32_C(1) + ((reg >> 0) & 0x01);	// PLL Output Div D.(The factor is only for testing)
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / M1;
}

uint_fast32_t allwnrt113_get_vepll_freq(void)
{
	const uint_fast32_t reg = CCU->PLL_VE_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0xFF);	// >= 12
	const uint_fast32_t M1 = UINT32_C(1) + ((reg >> 1) & 0x01);
	const uint_fast32_t M0 = UINT32_C(1) + ((reg >> 0) & 0x01);
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / M1 / M0;
}

//	By default, PLL_AUDIO0(1X) is 24.5714 MHz, and PLL_AUDIO0(4X) is 98.2856 MHz.

uint_fast32_t allwnrt113_get_audio0pll4x_freq(void)
{
	//PLL_AUDIO0(4X) = 24MHz*N/M1/M0/P
	const uint_fast32_t reg = CCU->PLL_AUDIO0_CTRL_REG;
	const uint_fast32_t pllPostDivP = 1 + ((reg >> 16) & 0x3F);	// PLL_POST_DIV_P
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0xFF);
	const uint_fast32_t M1 = UINT32_C(1) + ((reg >> 1) & 0x01);
	const uint_fast32_t M0 = UINT32_C(1) + ((reg >> 0) & 0x01);
	return (uint_fast64_t) allwnrt113_get_hosc_freq() * N / M0 / M1 / pllPostDivP;
}

// By default,
//	PLL_AUDIO1 is 3072 MHz,
// PLL_AUDIO1(DIV2) is 1536 MHz, and
// PLL_AUDIO1(DIV5) is 614.4 MHz (24.576 MHz*25).

uint_fast32_t allwnrt113_get_audio1pll1x_freq(void)
{
	const uint_fast32_t reg = CCU->PLL_AUDIO1_CTRL_REG;
	const uint_fast32_t N = UINT32_C(1) + ((reg >> 8) & 0xFF);
	const uint_fast32_t M1 = UINT32_C(1) + ((reg >> 1) & 0x01);
	const uint_fast32_t M0 = UINT32_C(1) + ((reg >> 0) & 0x01);
	return (uint_fast64_t) allwnrt113_get_hosc_freq() / M0 * N / M1;
}

uint_fast32_t allwnrt113_get_audio0pll1x_freq(void)
{
	return allwnrt113_get_audio0pll4x_freq() / 4;
}

uint_fast32_t allwnrt113_get_audio1pll_div2_freq(void)
{
	return allwnrt113_get_audio1pll1x_freq() / 2;
}

uint_fast32_t allwnrt113_get_audio1pll_div5_freq(void)
{
	return allwnrt113_get_audio1pll1x_freq() / 5;
}

uint_fast32_t allwnrt113_get_video0_x2_freq(void)
{
	return allwnrt113_get_video0pllx4_freq() / 2;
}

uint_fast32_t allwnrt113_get_video0_x1_freq(void)
{
	return allwnrt113_get_video0pllx4_freq() / 4;
}

uint_fast32_t allwnrt113_get_video1_x2_freq(void)
{
	return allwnrt113_get_video1pllx4_freq() / 2;
}

uint_fast32_t allwnrt113_get_video1_x1_freq(void)
{
	return allwnrt113_get_video1pllx4_freq() / 4;
}

uint_fast32_t allwnrt113_get_dram_freq(void)
{
	const uint_fast32_t clkreg = CCU->DRAM_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x03);
	const uint_fast32_t pgdiv = M * N;
	switch ((clkreg >> 24) & 0x03)	/* DRAM_CLK_SEL */
	{
	default:
	case 0x00:	/* 00: PLL_DDR */
		return allwnrt113_get_pll_ddr_freq() / pgdiv;
	case 0x01:	/* 01: PLL_AUDIO1(DIV2) */
		return allwnrt113_get_audio1pll1x_freq() / pgdiv;	// todo: check selected source
	case 0x02:	/* 10: PLL_PERI(2X) */
		return allwnrt113_get_peripll2x_freq() / pgdiv;
	case 0x03: /* 11: PLL_PERI(800M) */
		return allwnrt113_get_pll_peri_800M_freq() / pgdiv;
	}
}

uint_fast32_t allwnrt113_get_i2s1_freq(void)
{
	const uint_fast32_t clkreg = CCU->I2S1_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x1F);
	const uint_fast32_t pgdiv = M * N;
	// I2S/PCM1_CLK = Clock Source/M/N
	switch ((clkreg >> 24) & 0x03)	/* I2S1_CLK_SEL */
	{
	default:
	case 0x00:	/* 00: PLL_AUDIO0(1X) */
		return allwnrt113_get_audio0pll1x_freq() / pgdiv;
	case 0x01:	/* 01: PLL_AUDIO0(4X) */
		return allwnrt113_get_audio0pll4x_freq() / pgdiv;
	case 0x02:	/* 10: PLL_AUDIO1(DIV2) */
		return allwnrt113_get_audio1pll_div2_freq() / pgdiv;
	case 0x03: /* 11: PLL_AUDIO1(DIV5) */
		return allwnrt113_get_audio1pll_div5_freq() / pgdiv;
	}
}

uint_fast32_t allwnrt113_get_i2s2_freq(void)
{
	const uint_fast32_t clkreg = CCU->I2S2_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x1F);
	const uint_fast32_t pgdiv = M * N;
	// I2S/PCM2_CLK = Clock Source/M/N
	switch ((clkreg >> 24) & 0x03)	/* I2S2_CLK_SEL */
	{
	default:
	case 0x00:	/* 00: PLL_AUDIO0(1X) */
		return allwnrt113_get_audio0pll1x_freq() / pgdiv;
	case 0x01:	/* 01: PLL_AUDIO0(4X) */
		return allwnrt113_get_audio0pll4x_freq() / pgdiv;
	case 0x02:	/* 10: PLL_AUDIO1(DIV2) */
		return allwnrt113_get_audio1pll_div2_freq() / pgdiv;
	case 0x03: /* 11: PLL_AUDIO1(DIV5) */
		return allwnrt113_get_audio1pll_div5_freq() / pgdiv;
	}
}

uint_fast32_t allwnrt113_get_audio_codec_dac_freq(void)
{
	const uint_fast32_t clkreg = CCU->AUDIO_CODEC_DAC_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x1F);
	const uint_fast32_t pgdiv = M * N;
	// AUDIO_CODEC_DAC_CLK = Clock Source/M/N.
	//	Clock Source Select
	//	00: PLL_AUDIO0(1X)
	//	01: PLL_AUDIO1(DIV2)
	//	10: PLL_AUDIO1(DIV5)
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:	/* 00: PLL_AUDIO0(1X) */
		return allwnrt113_get_audio0pll1x_freq() / pgdiv;
	case 0x01:	/* 10: PLL_AUDIO1(DIV2) */
		return allwnrt113_get_audio1pll_div2_freq() / pgdiv;
	case 0x02: /* 11: PLL_AUDIO1(DIV5) */
		return allwnrt113_get_audio1pll_div5_freq() / pgdiv;
	}
}

uint_fast32_t allwnrt113_get_audio_codec_adc_freq(void)
{
	const uint_fast32_t clkreg = CCU->AUDIO_CODEC_ADC_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x1F);
	const uint_fast32_t pgdiv = M * N;
	// AUDIO_CODEC_ADC_CLK = Clock Source/M/N.
	//	Clock Source Select
	//	00: PLL_AUDIO0(1X)
	//	01: PLL_AUDIO1(DIV2)
	//	10: PLL_AUDIO1(DIV5)
	switch ((clkreg >> 24) & 0x03)	/* CLK_SRC_SEL */
	{
	default:
	case 0x00:	/* 00: PLL_AUDIO0(1X) */
		return allwnrt113_get_audio0pll1x_freq() / pgdiv;
	case 0x01:	/* 10: PLL_AUDIO1(DIV2) */
		return allwnrt113_get_audio1pll_div2_freq() / pgdiv;
	case 0x02: /* 11: PLL_AUDIO1(DIV5) */
		return allwnrt113_get_audio1pll_div5_freq() / pgdiv;
	}
}

// Graphic 2D (G2D)
uint_fast32_t allwnrt113_get_g2d_freq(void)
{
	const uint_fast32_t clkreg = CCU->G2D_CLK_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x1F);	/* M=FACTOR_M+1 */
	switch ((clkreg >> 24) & 0x07)	/* G2D_CLK_REG */
	{
	default:
	case 0x00:	/* 000: PLL_PERI(2X) */
		return allwnrt113_get_peripll2x_freq() / M;
	case 0x01:	/* 001: PLL_VIDEO0(4X) */
		return allwnrt113_get_video0pllx4_freq() / M;
	case 0x02:	/* 001: PLL_VIDEO1(4X) */
		return allwnrt113_get_video1pllx4_freq() / M;
	case 0x03: /* 011: PLL_AUDIO1(DIV2) */
		return allwnrt113_get_audio1pll_div2_freq() / M;
	}
}

// Display Engine (DE)
uint_fast32_t allwnrt113_get_de_freq(void)
{
	const uint_fast32_t clkreg = CCU->DE_CLK_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x1F);	/* M=FACTOR_M+1 */
	switch ((clkreg >> 24) & 0x07)	/* G2D_CLK_REG */
	{
	default:
	case 0x00:	/* 000: PLL_PERI(2X) */
		return allwnrt113_get_peripll2x_freq() / M;
	case 0x01:	/* 001: PLL_VIDEO0(4X) */
		return allwnrt113_get_video0pllx4_freq() / M;
	case 0x02:	/* 001: PLL_VIDEO1(4X) */
		return allwnrt113_get_video1pllx4_freq() / M;
	case 0x03: /* 011: PLL_AUDIO1(DIV2) */
		return allwnrt113_get_audio1pll_div2_freq() / M;
	}
}

// De-interlacer (DI)
// T113-S3_User_Manual_V1.5.pdf, page 20:
// Performance: module clock 600M for 1080p@60Hz YUV420

uint_fast32_t allwnrt113_get_di_freq(void)
{
	const uint_fast32_t clkreg = CCU->DI_CLK_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x1F);	/* M=FACTOR_M+1 */
	switch ((clkreg >> 24) & 0x07)	/* G2D_CLK_REG */
	{
	default:
	case 0x00:	/* 000: PLL_PERI(2X) */
		return allwnrt113_get_peripll2x_freq() / M;
	case 0x01:	/* 001: PLL_VIDEO0(4X) */
		return allwnrt113_get_video0pllx4_freq() / M;
	case 0x02:	/* 001: PLL_VIDEO1(4X) */
		return allwnrt113_get_video1pllx4_freq() / M;
	case 0x03: /* 011: PLL_AUDIO1(DIV2) */
		return allwnrt113_get_audio1pll_div2_freq() / M;
	}
}

// Crypto Engine (CE)
uint_fast32_t allwnrt113_get_ce_freq(void)
{
	const uint_fast32_t clkreg = CCU->CE_CLK_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x1F);	/* M=FACTOR_M+1 */
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t pgdiv = M * N;
	switch ((clkreg >> 24) & 0x07)	/* G2D_CLK_REG */
	{
	default:
	case 0x00:	/* 000: HOSC */
		return allwnrt113_get_vepll_freq() / pgdiv;
	case 0x01:	/* 001: PLL_PERI(2X) */
		return allwnrt113_get_peripll2x_freq() / pgdiv;
	case 0x02:	/* 010: PLL_PERI(1X) */
		return allwnrt113_get_peripll1x_freq() / pgdiv;
	}
}

// T113
uint_fast32_t allwnrt113_get_ve_freq(void)
{
	const uint_fast32_t clkreg = CCU->CE_CLK_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x1F);	/* M=FACTOR_M+1 */
	switch ((clkreg >> 24) & 0x07)	/* G2D_CLK_REG */
	{
	default:
	case 0x00:	/* 000: PLL_VE */
		return allwnrt113_get_vepll_freq() / M;
	case 0x01:	/* 001: PLL_PERI(2X) */
		return allwnrt113_get_peripll2x_freq() / M;
	}
}

// T113
uint_fast32_t allwnrt113_get_psi_freq(void)
{
	const uint_fast32_t clkreg = CCU->PSI_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x03);
	const uint_fast32_t pgdiv = M * N;
	switch ((clkreg >> 24) & 0x03)
	{
	default:
	case 0x00:
		/* 00: HOSC */
		return allwnrt113_get_hosc_freq() / pgdiv;
	case 0x01:
		/* 01: CLK32K */
		return allwnrt113_get_32k_freq() / pgdiv;
	case 0x02:
		/* 010: CLK16M_RC */
		return allwnrt113_get_16M_freq() / pgdiv;
	case 0x03:
		/* 11: PLL_PERI(1X) */
		return allwnrt113_get_peripll1x_freq() / pgdiv;
	}
}

// T113
uint_fast32_t allwnrt113_get_apb0_freq(void)
{
	const uint_fast32_t clkreg = CCU->APB0_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x1F);
	const uint_fast32_t pgdiv = M * N;
	switch ((clkreg >> 24) & 0x03)
	{
	default:
	case 0x00:
		/* 00: HOSC */
		return allwnrt113_get_hosc_freq() / pgdiv;
	case 0x01:
		/* 01: CLK32K */
		return allwnrt113_get_32k_freq() / pgdiv;
	case 0x02:
		/* 10: PSI_CLK */
		return allwnrt113_get_psi_freq() / pgdiv;
	case 0x03:
		/* 11: PLL_PERI(1X) */
		return allwnrt113_get_peripll1x_freq() / pgdiv;
	}
}

// T113
uint_fast32_t allwnrt113_get_apb1_freq(void)
{
	const uint_fast32_t clkreg = CCU->APB1_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x1F);
	const uint_fast32_t pgdiv = M * N;
	switch ((clkreg >> 24) & 0x03)
	{
	default:
	case 0x00:
		/* 00: HOSC */
		return allwnrt113_get_hosc_freq() / pgdiv;
	case 0x01:
		/* 01: CLK32K */
		return allwnrt113_get_32k_freq() / pgdiv;
	case 0x02:
		/* 10: PSI_CLK */
		return allwnrt113_get_psi_freq() / pgdiv;
	case 0x03:
		/* 11: PLL_PERI(1X) */
		return allwnrt113_get_peripll1x_freq() / pgdiv;
	}
}

// Прямого указания, откуда берётся AHB0, в документации нет.
// DMAC, HSTIMER, MSGBOX тактируются этой частотой.
// В документации Allwinner V853 AHB соответствует PSI
// T113-s3
uint_fast32_t allwnrt113_get_ahb0_freq(void)
{
	return allwnrt113_get_psi_freq();
}

// T113-s3
uint_fast32_t allwnrt113_get_pl1_timer_freq(void)
{
	return allwnrt113_get_ahb0_freq();
}

// T113-s3
// The clock of the UART is from APB1.
uint_fast32_t allwnrt113_get_uart_freq(void)
{
	return allwnrt113_get_apb1_freq();
}

// T113-s3
// The clock of the CAN is from APB1.
uint_fast32_t allwnrt113_get_can_freq(void)
{
	return allwnrt113_get_apb1_freq();
}

// T113
// The clock of the TWI is from APB1.
uint_fast32_t allwnrt113_get_twi_freq(void)
{
	return allwnrt113_get_apb1_freq();
}

// T113
uint_fast32_t allwnrt113_get_spi0_freq(void)
{
	const uint_fast32_t clkreg = CCU->SPI0_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	// SCLK = Clock Source/M/N.
	const uint_fast32_t pgdiv = M * N;
	switch ((clkreg >> 24) & 0x07)
	{
	default:
	case 0x00:
		/* 000: HOSC */
		return allwnrt113_get_hosc_freq() / pgdiv;
	case 0x01:
		/* 001: PLL_PERI(1X) */
		return allwnrt113_get_peripll1x_freq() / pgdiv;
	case 0x02:
		/* 010: PLL_PERI(2X) */
		return allwnrt113_get_peripll2x_freq() / pgdiv;
	case 0x03:
		/* 011: PLL_AUDIO1(DIV2) */
		return allwnrt113_get_audio1pll_div2_freq() / pgdiv;
	case 0x04:
		/* 100: PLL_AUDIO1(DIV5) */
		return allwnrt113_get_audio1pll_div5_freq() / pgdiv;
	}
}

uint_fast32_t allwnrt113_get_spi1_freq(void)
{
	const uint_fast32_t clkreg = CCU->SPI1_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	// SCLK = Clock Source/M/N.
	const uint_fast32_t pgdiv = M * N;
	switch ((clkreg >> 24) & 0x07)
	{
	default:
	case 0x00:
		/* 000: HOSC */
		return allwnrt113_get_hosc_freq() / pgdiv;
	case 0x01:
		/* 001: PLL_PERI(1X) */
		return allwnrt113_get_peripll1x_freq() / pgdiv;
	case 0x02:
		/* 010: PLL_PERI(2X) */
		return allwnrt113_get_peripll2x_freq() / pgdiv;
	case 0x03:
		/* 011: PLL_AUDIO1(DIV2) */
		return allwnrt113_get_audio1pll_div2_freq() / pgdiv;
	case 0x04:
		/* 100: PLL_AUDIO1(DIV5) */
		return allwnrt113_get_audio1pll_div5_freq() / pgdiv;
	}
}

// T113-s3
uint_fast32_t allwnrt113_get_smhc0_freq(void)
{
	const uint_fast32_t clkreg = CCU->SMHC0_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	// SMHC0_CLK = Clock Source/M/N.
	const uint_fast32_t pgdiv = M * N;	// post-gate dividers: clkdiv4 and clkdiv2y
	switch ((clkreg >> 24) & 0x07)
	{
	default:
	case 0x00:
		/* 000: HOSC */
		return allwnrt113_get_hosc_freq() / pgdiv;
	case 0x01:
		/* 001: PLL_PERI(1X) */
		return allwnrt113_get_peripll1x_freq() / pgdiv;
	case 0x02:
		/* 010: PLL_PERI(2X) */
		return allwnrt113_get_peripll2x_freq() / pgdiv;
	case 0x03:
		/* 011: PLL_AUDIO1(DIV2) */
		return allwnrt113_get_audio1pll_div2_freq() / pgdiv;
	}
}

// T113-s3
uint_fast32_t allwnrt113_get_smhc1_freq(void)
{
	const uint_fast32_t clkreg = CCU->SMHC1_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	// SMHC1_CLK = Clock Source/M/N.
	const uint_fast32_t pgdiv = M * N;
	switch ((clkreg >> 24) & 0x07)
	{
	default:
	case 0x00:
		/* 000: HOSC */
		return allwnrt113_get_hosc_freq() / pgdiv;
	case 0x01:
		/* 001: PLL_PERI(1X) */
		return allwnrt113_get_peripll1x_freq() / pgdiv;
	case 0x02:
		/* 010: PLL_PERI(2X) */
		return allwnrt113_get_peripll2x_freq() / pgdiv;
	case 0x03:
		/* 011: PLL_AUDIO1(DIV2) */
		return allwnrt113_get_audio1pll_div2_freq() / pgdiv;
	}
}

// T113-s3
uint_fast32_t allwnrt113_get_smhc2_freq(void)
{
	const uint_fast32_t clkreg = CCU->SMHC2_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	// SMHC2_CLK = Clock Source/M/N.
	const uint_fast32_t pgdiv = M * N;
	switch ((clkreg >> 24) & 0x07)
	{
	default:
	case 0x00:
		/* 000: HOSC */
		return allwnrt113_get_hosc_freq() / pgdiv;
	case 0x01:
		/* 001: PLL_PERI(1X) */
		return allwnrt113_get_peripll1x_freq() / pgdiv;
	case 0x02:
		/* 010: PLL_PERI(2X) */
		return allwnrt113_get_peripll2x_freq() / pgdiv;
	case 0x03:
		/* 011: PLL_PERI(800M) */
		return allwnrt113_get_pll_peri_800M_freq() / pgdiv;
	case 0x04:
		/* 100: PLL_AUDIO1(DIV2) */
		return allwnrt113_get_audio1pll_div2_freq() / pgdiv;
	}
}

// CPU Clock = Clock Source
// CPU_AXI Clock = Clock Source/M

// T113-s3
uint_fast32_t allwnrt113_get_arm_freq(void)
{
	const uint_fast32_t clkreg = CCU->CPU_AXI_CFG_REG;
	const uint_fast32_t P = UINT32_C(1) << ((clkreg >> 16) & 0x03); // PLL Output External Divider P
	switch ((clkreg >> 24) & 0x07)
	{
	default:
	case 0x00:	// 000: HOSC
		return allwnrt113_get_hosc_freq();
	case 0x01:	// 001: CLK32K
		return allwnrt113_get_32k_freq();
	case 0x02:	// 010: CLK16M_RC
		return allwnrt113_get_16M_freq();
	case 0x03:	// 011: PLL_CPU/P
		return allwnrt113_get_pll_cpu_freq() / P;
	case 0x04:	// 100: PLL_PERI(1X)
		return allwnrt113_get_peripll1x_freq();
	case 0x05:	//101: PLL_PERI(2X)
		return allwnrt113_get_peripll2x_freq();
	case 0x06:	// 110: PLL_PERI(800M)
		return allwnrt113_get_pll_peri_800M_freq();
	}
}

// CPU Clock = Clock Source
// CPU_AXI Clock = Clock Source/M
// T113-s3
uint_fast32_t allwnrt113_get_axi_freq(void)
{
	const uint_fast32_t clkreg = CCU->CPU_AXI_CFG_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x03);
	const uint_fast32_t N = UINT32_C(1) + ((clkreg >> 8) & 0x03);	// для чего?
	return allwnrt113_get_arm_freq() / M;
}

// The MBUS clock is from the 4 frequency-division of PLL_DDR, and it has the same source with the DRAM clock
// T113-s3
uint_fast32_t allwnrt113_get_mbus_freq(void)
{
	return allwnrt113_get_pll_ddr_freq() / 4;
}

uint_fast32_t allwnrt113_get_tconlcd_freq(void)
{
	const uint_fast32_t clkreg = CCU->TCONLCD_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	const uint_fast32_t pgdiv = M * N;

	// TCONLCD_CLK = Clock Source/M/N.
	switch ((clkreg >> 24) & 0x07)	// CLK_SRC_SEL
	{
	default:
	case 0x00:
		// 000: PLL_VIDEO0(1X)
		return allwnrt113_get_video0_x1_freq() / pgdiv;
	case 0x01:
		// 001: PLL_VIDEO0(4X)
		return allwnrt113_get_video0pllx4_freq() / pgdiv;
	case 0x02:
		// 010: PLL_VIDEO1(1X)
		return allwnrt113_get_video1_x1_freq() / pgdiv;
	case 0x03:
		// 011: PLL_VIDEO1(4X)
		return allwnrt113_get_video1pllx4_freq() / pgdiv;
	case 0x04:
		// 100: PLL_PERI(2X)
		return allwnrt113_get_peripll2x_freq() / pgdiv;
	case 0x05:
		// 101: PLL_AUDIO1(DIV2)
		return allwnrt113_get_audio1pll_div2_freq() / pgdiv;
	}
}

uint_fast32_t allwnrt113_get_tcontv_freq(void)
{
	const uint_fast32_t clkreg = CCU->TCONTV_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) << ((clkreg >> 8) & 0x03);
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);
	const uint_fast32_t pgdiv = M * N;

	// TCONTV_CLK = Clock Source/M/N.
	switch ((clkreg >> 24) & 0x07)	// CLK_SRC_SEL
	{
	default:
	case 0x00:
		// 000: PLL_VIDEO0(1X)
		return allwnrt113_get_video0_x1_freq() / pgdiv;
	case 0x01:
		// 001: PLL_VIDEO0(4X)
		return allwnrt113_get_video0pllx4_freq() / pgdiv;
	case 0x02:
		// 010: PLL_VIDEO1(1X)
		return allwnrt113_get_video1_x1_freq() / pgdiv;
	case 0x03:
		// 011: PLL_VIDEO1(4X)
		return allwnrt113_get_video1pllx4_freq() / pgdiv;
	case 0x04:
		// 100: PLL_PERI(2X)
		return allwnrt113_get_peripll2x_freq() / pgdiv;
	case 0x05:
		// 101: PLL_AUDIO1(DIV2)
		return allwnrt113_get_audio1pll_div2_freq() / pgdiv;
	}
}

uint_fast32_t allwnrt113_get_dsi_freq(void)
{
	const uint_fast32_t clkreg = CCU->DSI_CLK_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x0F);

	// DSI_CLK = Clock Source/M.
	switch ((clkreg >> 24) & 0x07)	// CLK_SRC_SEL
	{
	default:
	case 0x00:
		/* 000: HOSC */
		return allwnrt113_get_hosc_freq() / M;
	case 0x01:
		/* 001: PLL_PERI(1X) */
		return allwnrt113_get_peripll1x_freq() / M;
	case 0x02:
		// 010: PLL_VIDEO0(2X)
		return allwnrt113_get_video0_x2_freq() / M;
	case 0x03:
		// 011: PLL_VIDEO1(2X)
		return allwnrt113_get_video1_x2_freq() / M;
	case 0x04:
		/* 100: PLL_AUDIO1(DIV2) */
		return allwnrt113_get_audio1pll_div2_freq() / M;
	}
}

uint_fast32_t allwnrf133_get_riscv_freq(void)
{
	const uint_fast32_t clkreg = CCU->RISC_CLK_REG;
	//const uint_fast32_t N = UINT32_C(1) + ((clkreg >> 8) & 0x03);	// RISC_AXI_DIV_CFG
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x1F);	// RISC_DIV_CFG
	switch ((clkreg >> 24) & 0x07)
	{
	default:
	case 0x00:
		/* 000: HOSC */
		return allwnrt113_get_hosc_freq() / M;
	case 0x01:
		/* 001: CLK32K */
		return allwnrt113_get_32k_freq() / M;
	case 0x02:
		/* 010: CLK16M_RC */
		return allwnrt113_get_16M_freq() / M;
	case 0x03:
		/* 011: PLL_PERI(800M) */
		return allwnrt113_get_pll_peri_800M_freq() / M;
	case 0x04:
		/* 100: PLL_PERI(1X) */
		return allwnrt113_get_peripll1x_freq() / M;
	case 0x05:
		/* 101: PLL_CPU */
		return allwnrt113_get_pll_cpu_freq() / M;
	case 0x06:
		/*110: PLL_AUDIO1(DIV2) */
		return allwnrt113_get_audio1pll_div2_freq() / M;

	}
}

uint_fast32_t allwnrf133_get_riscv_axi_freq(void)
{
	const uint_fast32_t clkreg = CCU->RISC_CLK_REG;
	const uint_fast32_t N = UINT32_C(1) + ((clkreg >> 8) & 0x03);	// RISC_AXI_DIV_CFG
	//const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x1F);	// RISC_DIV_CFG
	return allwnrf133_get_riscv_freq() / N;
}

// DSP (HiFi4)
uint_fast32_t allwnrt113_get_dsp_freq(void)
{
	const uint_fast32_t clkreg = CCU->DSP_CLK_REG;
	const uint_fast32_t M = UINT32_C(1) + ((clkreg >> 0) & 0x1F);	/* M=FACTOR_M+1 */
	switch ((clkreg >> 24) & 0x07)	/* G2D_CLK_REG */
	{
	default:
	case 0x00:
		/* 000: HOSC */
		return allwnrt113_get_hosc_freq() / M;
	case 0x01:
		/* 001: CLK32K */
		return allwnrt113_get_32k_freq() / M;
	case 0x02:
		/* 010: CLK16M_RC */
		return allwnrt113_get_16M_freq() / M;
	case 0x03:
		/* 010: PLL_PERI(2X) */
		return allwnrt113_get_peripll2x_freq() / M;
	case 0x04:
		/*110: PLL_AUDIO1(DIV2) */
		return allwnrt113_get_audio1pll_div2_freq() / M;
	}
}


void allwnrt113_pll_initialize(void)
{
#if CPUSTYLE_T113
	set_pll_cpux_axi(PLL_CPU_N);	// see sdram.c
#endif
#if (CPUSTYLE_F133 || CPUSTYLE_T113_S4)
	set_pll_riscv_axi(RV_PLL_CPU_N);	// see sdram.c
#endif

	//set_pll_periph0();
	set_ahb();
	//set_apb();	// УБрал для того, чтобы инициализация ddr3 продолжала выводить текстовый лог
	set_mbus();
	set_dma();

	allwnrt113_module_pll_enable(& CCU->PLL_PERI_CTRL_REG);
	allwnrt113_module_pll_enable(& CCU->PLL_VIDEO0_CTRL_REG);
	allwnrt113_module_pll_enable(& CCU->PLL_VIDEO1_CTRL_REG);
	allwnrt113_module_pll_enable(& CCU->PLL_VE_CTRL_REG);
	allwnrt113_module_pll_enable(& CCU->PLL_AUDIO0_CTRL_REG);
	allwnrt113_module_pll_enable(& CCU->PLL_AUDIO1_CTRL_REG);

	// APB1 frequency set
	{
		// Automatic divisors calculation
		unsigned clksrc = 3;	// 11: PLL_PERI(1X)
		uint_fast32_t needfreq = UINT32_C(200) * 1000 * 1000;
		unsigned dvalue;
		unsigned prei = calcdivider(calcdivround2(allwnrt113_get_peripll1x_freq(), needfreq), 5, (8 | 4 | 1 | 2), & dvalue, 1);
		CCU->APB1_CLK_REG =
			(UINT32_C(1) << 31) |
			clksrc * (UINT32_C(1) << 24) |
			prei * (UINT32_C(1) << 8) |
			dvalue * (UINT32_C(1) << 0) |
			0;
	}

}

#endif /* CPUSTYLE_STM32MP1 */


#if CPUSTYLE_VM14

#if 1
// 1892ВМ14Я ELVEES multicore.ru

/********MCOM-02 REGMAP DEFINE*******************************************/
/***************************System Registers*****************************/

#define DEFAULT_XTI_CLOCK (24)
#define MIN_CPU_FREQ_MHZ DEFAULT_XTI_CLOCK
#define MAX_CPU_FREQ_MHZ (912)
#define MIN_DSP_FREQ_MHZ DEFAULT_XTI_CLOCK
#define MAX_DSP_FREQ_MHZ (720)

#define SYSFREQ 96

//#define CMCTR_BASE 0x38094000
#define CLK_I2C2_EN (1 << 18)
#define CLK_I2C1_EN (1 << 17)
#define CLK_I2C0_EN (1 << 16)

#define DSPENC_EN (1 << 3)
#define DSPEXT_EN (1 << 2)
#define DSP1_EN (1 << 1)
#define DSP0_EN (1 << 0)
//#define SEL_APLL (*(volatile uint32_t *)(CMCTR_BASE + 0x100))
//#define SEL_CPLL (*(volatile uint32_t *)(CMCTR_BASE + 0x104))
//#define SEL_DPLL (*(volatile uint32_t *)(CMCTR_BASE + 0x108))
//#define SEL_SPLL (*(volatile uint32_t *)(CMCTR_BASE + 0x10c))
//#define SEL_VPLL (*(volatile uint32_t *)(CMCTR_BASE + 0x110))
#define PLL_LOCK_BIT (1 << 31)

/***************************GPIO******************************************/
//#define GPIO0_BASE 0x38034000
//#define GPIO0(a) (*(volatile uint32_t *)(GPIO0_BASE + (a)))
//#define SWPORTA_DR 0x00
//#define SWPORTA_DDR 0x04
//#define SWPORTA_CTL 0x08
//#define SWPORTB_DR 0x0c
//#define SWPORTB_DDR 0x10
//#define SWPORTB_CTL 0x14
//#define SWPORTC_DR 0x18
//#define SWPORTC_DDR 0x1c
//#define SWPORTC_CTL 0x20
//#define SWPORTD_DR 0x24
//#define SWPORTD_DDR 0x28
//#define SWPORTD_CTL 0x2c
//
//#define GPIOA29_I2C0_SDA (1 << 29)
//#define GPIOA29_I2C0_SCL (1 << 30)
//#define GPIOD22_I2C1_SDA (1 << 22)
//#define GPIOD23_I2C1_SCL (1 << 23)
//#define GPIOD24_I2C2_SDA (1 << 24)
//#define GPIOD25_I2C2_SCL (1 << 25)

/***FAN53555**************************************************************/
/***5 A, 2.4MHz, Digitally Programmable TinyBuck Regulator****************/

// в загрузчике пока нельзя использовать - память не инициализированна
///***CPU FREQ TABLE********************************************************/
//#define CPU_FREQ_VOLTAGE_SIZE 3
//static const unsigned int CPU_FREQ_VOLTAGE[CPU_FREQ_VOLTAGE_SIZE][2]
//    __attribute__((aligned(8))) = {
//        //   MHz,   mV
//        {816, 1040},
//        {912, 1103},
//        {1008, 1205}};
//
///***DSP FREQ TABLE********************************************************/
//#define DSP_FREQ_VOLTAGE_SIZE 3
//
//static const unsigned int DSP_FREQ_VOLTAGE[CPU_FREQ_VOLTAGE_SIZE][2]
//    __attribute__((aligned(8))) = {
//        //   MHz,   mV
//        {696, 1040},
//        {768, 1103},
//        {888, 1205}};


static unsigned int getCurrentCPUFreq(void)
{
    int sel = (CMCTR->SEL_APLL & 0xFF);
    if (sel > 0x3D)
        sel = 0x3D;
    return (sel + 1) * DEFAULT_XTI_CLOCK;
}
static unsigned int getCurrentDSPFreq(void)
{
    int sel = (CMCTR->SEL_DPLL & 0xFF);
    if (sel > 0x3D)
        sel = 0x3D;
    return (sel + 1) * DEFAULT_XTI_CLOCK;
}

static void setCPUPLLFreq(unsigned int MHz)
{
    CMCTR->SEL_APLL = 0;
    (void) CMCTR->SEL_APLL;
    int sel = (MHz / DEFAULT_XTI_CLOCK) - 1;
    if (sel < 0)
        sel = 0;
    CMCTR->SEL_APLL = sel;
    (void) CMCTR->SEL_APLL;
    while (!(CMCTR->SEL_APLL & PLL_LOCK_BIT))
        ;
}

static void setDSPPLLFreq(unsigned int MHz)
{
    int sel = (MHz / DEFAULT_XTI_CLOCK) - 1;
    if (sel < 0)
        sel = 0;
    CMCTR->SEL_DPLL = sel;
    while (!(CMCTR->SEL_DPLL & PLL_LOCK_BIT))
        ;
}

static int setSystemFreq(unsigned int MHz)
{
    int sel = (MHz / DEFAULT_XTI_CLOCK) - 1;
    if (sel < 0)
        sel = 0;

    if (sel > 5) {
        /* L3_PCLK can't be above 144 MHz */
    	CMCTR->DIV_SYS1_CTR = 1;
    }

    if ((CMCTR->SEL_SPLL & 0xFF) != sel) {
    	CMCTR->SEL_SPLL = sel;
        while (!(CMCTR->SEL_SPLL & PLL_LOCK_BIT))
            ;
    }

    if (sel <= 5) {
    	CMCTR->DIV_SYS1_CTR = 0;
    }

    return 0;
}


static int setCPUFreq(unsigned int MHz)
{
    unsigned int last_freq;

    if (MHz < MIN_CPU_FREQ_MHZ || MHz > MAX_CPU_FREQ_MHZ )
    {
        return 1;
    }
    // Save current CPU freq
    last_freq = getCurrentCPUFreq();
    // Set default CPU freq
    setCPUPLLFreq(DEFAULT_XTI_CLOCK);

    // Set CPU freq
    setCPUPLLFreq(MHz);

    (void) last_freq;
    return 0;
}

static int setDSPFreq(unsigned int MHz)
{
    unsigned int last_freq;

    if (MHz < MIN_DSP_FREQ_MHZ || MHz > MAX_DSP_FREQ_MHZ)
    {
        return 1;
    }
    // Enable DSP_CLK
    CMCTR->GATE_DSP_CTR |= DSP0_EN | DSP1_EN | DSPEXT_EN | DSPENC_EN;

    // Save current DSP freq
    last_freq = getCurrentDSPFreq();
    // Set default DSP freq
    setDSPPLLFreq(DEFAULT_XTI_CLOCK);

    // Set DSP freq
    setDSPPLLFreq(MHz);

    (void) last_freq;
    return 0;
}
#endif

uint_fast32_t elveesvm14_get_arm_freq(void)
{
	return getCurrentCPUFreq() * 1000 * 1000;
}

static uint32_t elveesvm14_get_xtal_freq(void)
{
	return WITHCPUXTAL;

}

uint_fast32_t elveesvm14_get_usart_freq(void)
{
	return SYSFREQ * 1000 * 1000; // elveesvm14_get_xtal_freq();
	//return 24000000;//SYSFREQ * 1000 * 1000; // elveesvm14_get_xtal_freq();
}


//    write32(0x38094068, 0xf);  //# CMCTR.GATE_DSP_CTR
//    write32(0x3809410c, 0x1);  //# CMCTR.SEL_SPLL
//    write32(0x38094100, 0x2);  //# CMCTR.SEL_APLL
//    write32(0x38094104, 0x2);  //# CMCTR.SEL_CPLL
//    write32(0x38094114, 0x2);  //# CMCTR.SEL_UPLL
//    write32(0x38094108, 0x4);  //# CMCTR.SEL_DPLL
//    write32(0x38094048, 0x21); //# CMCTR.GATE_CORE_CTR
//    //write32(0x3809404c, 0xffffffff); //# CMCTR.GATE_SYS_CTR
//    //# setup registers to allow high frequences
//    write32(0x38094004, 0x1);   //# DIV_MPU_CTR = 1;
//    write32(0x38094008, 0x3);   //# DIV_ATB_CTR = 3;
//    write32(0x3809400c, 0x1);   //# DIV_APB_CTR = 1;
//    write32(0x38094014, 0x1);   //# GATE_MPU_CTR = 1;

void vm14_pll_initialize(void)
{
	CMCTR->DIV_MPU_CTR = 0x01;
	CMCTR->DIV_ATB_CTR = 0x03;
	CMCTR->DIV_APB_CTR = 0x01;
	CMCTR->DIV_MPU_CTR = 0x01;
	CMCTR->GATE_MPU_CTR |= 0x01;
	setSystemFreq(SYSFREQ);
#if WITHISBOOTLOADER
	setCPUFreq(768);
#else
	setCPUFreq(768);
#endif
}

#endif /* CPUSTYLE_VM14 */

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
	uint_fast32_t
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
	    CPG.FRQCR =
	    	CPG_FRQCR_CKOEN |	/* CKIO pin: Output off (Hi-Z) */
	    	0x35 |	/* reserved bits state */
			0;
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
uint_fast8_t
calcdivider(
	uint_fast32_t divisor, // ожидаемый коэффициент деления всей системы
	uint_fast8_t width,			// количество разрядов в счётчике
	uint_fast16_t taps,			// маска битов - выходов прескалера. 0x01 - означает bypass, 0x02 - делитель на 2... 0x400 - делитель на 1024
	unsigned * dvalue,		// Значение для записи в регистр сравнения делителя
	uint_fast8_t substract)
{
	const uint_fast8_t rbmax = 16; //позиция старшего значащего бита в маске TAPS
	uint_fast8_t rb, rbi;
	uint_fast32_t prescaler = 1;

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

	PRINTF("calcdivider: no parameters for divisor=%lu, width=%u, taps=%08X\n", (unsigned long) divisor, (unsigned) width, (unsigned) taps);

	// Не подобрать комбинацию прескалера и делителя для ожидаемого коэффициента деления.
	* dvalue = (UINT32_C(1) << width) - 1;	// просто пустышка - максимальный делитель
	return (rbi - 1);	// если надо обраьатывать невозможность подбора - возврат rbmax
}

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
#elif _WIN32
#elif ! LINUX_SUBSYSTEM
	// Cortex A7, Cortex A9
	local_delay_us(5);
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

	static volatile uint_fast32_t gtimloadvalue;

	void
	SecurePhysicalTimer_IRQHandler(void)
	{
		//IRQ_ClearPending (SecurePhysicalTimer_IRQn);
		PL1_SetLoadValue(gtimloadvalue);

		spool_systimerbundle1();	// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
		spool_systimerbundle2();	// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
	}


#elif CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616 || CPUSTYLE_V3S

	// Таймер электронного ключа
	void TIMER0_IRQHandler(void)
	{
		enum { IX = 0 };
		const portholder_t st = TIMER->TMR_IRQ_STA_REG;
		if ((st & (UINT32_C(1) << IX)) != 0)	// TMR0_IRQ_PEND
		{
			// Таймер электронного ключа
			// 1/20 dot length interval timer
			spool_elkeybundle();

			TIMER->TMR_IRQ_STA_REG = (UINT32_C(1) << IX);	// TMR0_IRQ_PEND
		}
	}

	// Таймер "тиков"
	void TIMER1_IRQHandler(void)
	{
		enum { IX = 1 };
		const portholder_t st = TIMER->TMR_IRQ_STA_REG;
		if ((st & (UINT32_C(1) << IX)) != 0)	// TMR1_IRQ_PEND
		{
			// timebase
			spool_systimerbundle1();	// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
			spool_systimerbundle2();	// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.

			TIMER->TMR_IRQ_STA_REG = (UINT32_C(1) << IX);	// TMR1_IRQ_PEND
		}
	}

#elif CPUSTYLE_R7S721

	// Таймер "тиков"
	void OSTMI0TINT_IRQHandler(void)
	{
		spool_systimerbundle1();	// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
		spool_systimerbundle2();	// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
	}

	// Таймер электронного ключа
	void OSTMI1TINT_IRQHandler(void)
	{
		spool_elkeybundle();
	}

#elif CPUSTYLE_XC7Z  && ! LINUX_SUBSYSTEM

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
		spool_encinterrupts(& encoder1);	/* прерывание по изменению сигнала на входах от валкодера */
	}

	ISR(PORTC_INT1_vect)
	{
		spool_encinterrupts(& encoder1);	/* прерывание по изменению сигнала на входах от валкодера */
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

#elif CPUSTYLE_VM14
	// Private timer use
	void
	PTIM_Handler(void)
	{
		PTIM_ClearEventFlag();
		spool_systimerbundle1();	// При возможности вызываются столько раз, сколько произошло таймерных прерываний.
		spool_systimerbundle2();	// Если пропущены прерывания, компенсировать дополнительными вызовами нет смысла.
	}

#elif CPUSTYLE_STM32F30X
	#warning TODO: Add code for STM32F30X support


#endif

// Инициализация таймера, вызывающего прерывания с частотой TICKS_FREQUENCY.
// Должо вызываться позже настройки ADC
void
hardware_timer_initialize(uint_fast32_t ticksfreq)
{
	adcdones_initialize();
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
	TIMSK2 |= (UINT32_C(1) << OCIE2A);	//0x02;	// enable interrupt from Timer/Counter 2 - use TIMER2_COMP_vect

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
	TIMSK0 |= (UINT32_C(1) << OCIE0A);	// enable interrupt from Timer/Counter 0 - use TIMER0_COMPA_vect

	TCNT0 = 0x00;

#elif CPUSTYLE_ATMEGA128
	// ATMega128/ATMega64

	// Timer/Counter 0 initialization
	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, ticksfreq), ATMEGA128_TIMER0_WIDTH, ATMEGA128_TIMER0_TAPS, & value, 1);
	TCCR0 = (UINT32_C(1) << WGM01) | (prei + 1);	// прескалер
	OCR0 = value;	// делитель - программирование полного периода

	TIMSK |= (UINT32_C(1) << OCIE0);	// enable interrupt from Timer/Counter 0 - use TIMER0_COMP_vect

	TCNT0 = 0x00;

#elif CPUSTYLE_ATMEGA32
	// Classic ATMega32

	// Timer/Counter 0 initialization
	// Использование автоматического расчёта предделителя
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, ticksfreq), ATMEGA_TIMER0_WIDTH, ATMEGA_TIMER0_TAPS, & value, 1);
	TCCR0 = (UINT32_C(1) << WGM01) | (prei + 1);	// прескалер
	OCR0 = value;	// делитель - программирование полного периода

	TIMSK |= (UINT32_C(1) << OCIE0);	// enable interrupt from Timer/Counter 0 - use TIMER0_COMP_vect

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
    AT91C_BASE_AIC->AIC_IDCR = (UINT32_C(1) << AT91C_ID_SYS);		// disable interrupt
    AT91C_BASE_AIC->AIC_SVR [AT91C_ID_SYS] = (AT91_REG) AT91F_SYS_IRQHandler;
	AT91C_BASE_AIC->AIC_SMR [AT91C_ID_SYS] =
		(AT91C_AIC_SRCTYPE & AT91C_AIC_SRCTYPE_HIGH_LEVEL) |
		(AT91C_AIC_PRIOR & AT91C_AIC_PRIOR_LOWEST);
	AT91C_BASE_AIC->AIC_ICCR = (UINT32_C(1) << AT91C_ID_SYS);		// clear pending interrupt
    AT91C_BASE_AIC->AIC_IECR = (UINT32_C(1) << AT91C_ID_SYS);	// enable inerrupt

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
		0 * (UINT32_C(1) << 1) |	// Interval Timer Mode
		1 * (UINT32_C(1) << 0) |	// Enables the interrupts when counting starts.
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

	TIM5->PSC = ((UINT32_C(1) << prei) - 1);
	TIM5->ARR = value;
	TIM5->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости - если счётчик успевал превысить значение ARR - считал до конца */

	arm_hardware_set_handler_system(TIM5_IRQn, TIM5_IRQHandler);

	// Prepare funcionality: use CNTP
	const uint_fast32_t gtimfreq = stm32mp1_get_hsi_freq();

	PL1_SetCounterFrequency(gtimfreq);	// CNTFRQ

	gtimloadvalue = calcdivround2(gtimfreq, ticksfreq) - 1;
	// Private timer use
	// Disable Private Timer and set load value
	PL1_SetControl(0);	// CNTP_CTL
	PL1_SetLoadValue(gtimloadvalue);	// CNTP_TVAL

	//arm_hardware_set_handler_system(SecurePhysicalTimer_IRQn, SecurePhysicalTimer_IRQHandler);

	// Enable timer control
	PL1_SetControl(1);

#elif CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616 || CPUSTYLE_V3S

	// timebase timer
	const unsigned ix = 1;
	TIMER->TMR [ix].CTRL_REG = 0;
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(allwnrt113_get_hosc_freq(), ticksfreq), ALLWNR_TIMER_WIDTH, ALLWNR_TIMER_TAPS, & value, 0);

	TIMER->TMR [ix].INTV_VALUE_REG = value;
	TIMER->TMR [ix].CTRL_REG =
		0 * (UINT32_C(1) << 7) |	// TMR1_MODE 0: Periodic mode.
		prei * (UINT32_C(1) << 4) |
		0x01 * (UINT32_C(1) << 2) |	// TMR1_CLK_SRC 01: OSC24M
		(UINT32_C(1) << 0) | // TMR1_EN
		0;

	while ((TIMER->TMR [ix].CTRL_REG & (UINT32_C(1) << 1)) != 0)
		;
	TIMER->TMR [ix].CTRL_REG |= (UINT32_C(1) << 1);	// TMR1_RELOAD

	TIMER->TMR_IRQ_EN_REG |= (UINT32_C(1) << (0 + ix));	// TMR1_IRQ_EN

	arm_hardware_set_handler_system(TIMER1_IRQn, TIMER1_IRQHandler);	// timebase timer

#elif CPUSTYLE_XC7Z  && ! LINUX_SUBSYSTEM

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
#elif CPUSTYLE_XCZU && LINUX_SUBSYSTEM
#elif CPUSTYLE_XC7Z && LINUX_SUBSYSTEM

#elif CPUSTYLE_T507
	#warning Undefined CPUSTYLE_T507

#elif CPUSTYLE_VM14
	// Private timer use
	// Disable Private Timer and set load value
	PTIM_SetControl(0);
	PTIM_SetCurrentValue(0);
	PTIM_SetLoadValue(calcdivround2(CPU_FREQ, ticksfreq * 2));	// Private Timer runs with the system frequency / 2
	// Set bits: IRQ enable and Auto reload
	PTIM_SetControl(0x06U);

	arm_hardware_set_handler_system(SecurePhysicalTimer_IRQn, PTIM_Handler);

	// Start the Private Timer
	PTIM_SetControl(PTIM_GetControl() | 0x01);

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
			//(UINT32_C(1) << SYSCFG_CMPCR_SW_CTRL_Pos) |	// 1: IO compensation values come from RANSRC[3:0] and RAPSRC[3:0]
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

	// AXI, AHB5 and AHB6 clock divisor (up to 266 MHz)
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
    AT91C_BASE_PMC->PMC_PCER = UINT32_C(1) << AT91C_ID_UDP;


*/

// отключение неиспользуемого USB порта.
static void usb_disable(void)
{
#if defined(AT91C_ID_UDP)
    AT91C_BASE_PMC->PMC_SCER = AT91C_PMC_UDP;
	AT91C_BASE_PMC->PMC_PCER = UINT32_C(1) << AT91C_ID_UDP; // разрешить тактированние этого блока

	AT91C_BASE_UDP->UDP_TXVC = AT91C_UDP_TXVDIS;	// запрет usb приемо-передатчика

	AT91C_BASE_PMC->PMC_PCDR = UINT32_C(1) << AT91C_ID_UDP; // запретить тактированние этого блока
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
	const uint_fast32_t mor = PMC->CKGR_MOR & ~ CKGR_MOR_KEY_Msk;
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
	const uint_fast32_t mor = PMC->CKGR_MOR & ~ (CKGR_MOR_KEY_Msk | CKGR_MOR_MOSCRCEN);
    PMC->CKGR_MOR = mor | CKGR_MOR_KEY(0x37);
}

static void program_disable_xtal(void)
{
	const uint_fast32_t mor = PMC->CKGR_MOR & ~ (CKGR_MOR_KEY_Msk | CKGR_MOR_MOSCXTEN);
    PMC->CKGR_MOR = mor | CKGR_MOR_KEY(0x37);
}

// Enable high-frequency XTAL oscillator
static void program_enable_xtal(void)
{
	const uint_fast32_t mor = PMC->CKGR_MOR & ~ (CKGR_MOR_KEY_Msk | CKGR_MOR_MOSCXTST_Msk);
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
uint_fast32_t stm32f7xx_pllq_initialize(void)
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
uint_fast32_t stm32f7xx_pllq_initialize(void)
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
		arm_hardware_pioa_altfn50(UINT32_C(1) << 8, AF_SYSTEM);	// PA8, AF=0: MCO1
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
		arm_hardware_pioc_altfn50(UINT32_C(1) << 9, AF_SYSTEM);	// PC9, AF=0: MCO2
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
		arm_hardware_pioa_altfn50(UINT32_C(1) << 8, AF_SYSTEM);			/* PA0 MCO1 */
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
uint_fast32_t stm32f7xx_pllq_initialize(void)
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
	arm_hardware_pioa_altfn50(UINT32_C(1) << 8, AF_SYSTEM);	// PA8, AF=0: MCO

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
	arm_hardware_pioa_altfn50(UINT32_C(1) << 8, AF_SYSTEM);	// PA8, AF=0: MCO

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
	const uint_fast32_t mor = PMC->CKGR_MOR & ~ CKGR_MOR_KEY_Msk;
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
	const uint_fast32_t mor = PMC->CKGR_MOR & ~ (CKGR_MOR_KEY_Msk | CKGR_MOR_MOSCRCEN);
    PMC->CKGR_MOR = mor | CKGR_MOR_KEY_PASSWD;
}

static void program_disable_xtal(void)
{
	const uint_fast32_t mor = PMC->CKGR_MOR & ~ (CKGR_MOR_KEY_Msk | CKGR_MOR_MOSCXTEN);
    PMC->CKGR_MOR = mor | CKGR_MOR_KEY_PASSWD;
}

// Enable high-frequency XTAL oscillator
static void program_enable_xtal(void)
{
	const uint_fast32_t mor = PMC->CKGR_MOR & ~ (CKGR_MOR_KEY_Msk | CKGR_MOR_MOSCXTST_Msk);
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
	arm_hardware_pioa_altfn50(UINT32_C(1) << 8, AF_SYSTEM);	// PA8, AF=0: MCO

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
	arm_hardware_pioa_altfn50(UINT32_C(1) << 8, AF_SYSTEM);	// PA8, AF=0: MCO

#endif
}

#endif /* CPUSTYLE_STM32L0XX */

#if CPUSTYLE_XC7Z && ! LINUX_SUBSYSTEM

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
	while ((SCLR->PLL_STATUS & (UINT32_C(1) << 0)) == 0)	// ARM_PLL_LOCK
		;
	//	EMIT_MASKWRITE(0XF8000100, 0x00000010U ,0x00000000U),	// ARM_PLL_CTRL
	SCLR->ARM_PLL_CTRL &= ~ 0x0010uL;	// PLL_BYPASS_FORCE

	//	////EMIT_MASKWRITE(0XF8000120, 0x1F003F30U ,0x1F000200U),	// ARM_CLK_CTRL
	SCLR->ARM_CLK_CTRL = (SCLR->ARM_CLK_CTRL & ~ (0x1F003F30U)) |
			(UINT32_C(1) << 28) |	// CPU_PERI_CLKACT
			(UINT32_C(1) << 27) |	// CPU_1XCLKACT
			(UINT32_C(1) << 26) |	// CPU_2XCLKACT
			(UINT32_C(1) << 25) |	// CPU_3OR2XCLKACT
			(UINT32_C(1) << 24) |	// CPU_6OR4XCLKACT
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
	while ((SCLR->PLL_STATUS & (UINT32_C(1) << 1)) == 0)	// DDR_PLL_LOCK
		;
	//	EMIT_MASKWRITE(0XF8000104, 0x00000010U ,0x00000000U),	// DDR_PLL_CTRL
	SCLR->DDR_PLL_CTRL = (SCLR->DDR_PLL_CTRL & ~ (0x00000010U)) |
			0x00000000U |
			0;
	//	EMIT_MASKWRITE(0XF8000124, 0xFFF00003U ,0x0C200003U),	// DDR_CLK_CTRL
	SCLR->DDR_CLK_CTRL = (SCLR->DDR_CLK_CTRL & ~ (0xFFF00003U)) |
			((uint_fast32_t) DDR_2XCLK_DIVISOR << 26) |	// DDR_2XCLK_DIVISOR
			((uint_fast32_t) DDR_3XCLK_DIVISOR << 20) |	// DDR_3XCLK_DIVISOR (only even)
			(UINT32_C(1) << 1) |	// DDR_2XCLKACT
			(UINT32_C(1) << 0) | // DDR_3XCLKACT
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
	while ((SCLR->PLL_STATUS & (UINT32_C(1) << 2)) == 0)	// IO_PLL_LOCK
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

#endif /* CPUSTYLE_XC7Z  && ! LINUX_SUBSYSTEM */

uint32_t SystemCoreClock;     /*!< System Clock Frequency (Core Clock)  */

// PLL initialize
void
sysinit_pll_initialize(int forced)
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
			enum { WORKMASK = UINT32_C(1) << 11 };	/* PA11 */
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
	//while ((SPIBSC0.CMNSR & (UINT32_C(1) << 0)) == 0)	// TEND bit
	//	;
	//SPIBSC0.SSLDR = 0x00;
	//SPIBSC0.SPBCR = 0x200;
	//SPIBSC0.DRCR = 0x0100;

//	/* Установить скорость обмена с SERIAL FLASH повыше */
//	if ((CPG.STBCR9 & CPG_STBCR9_BIT_MSTP93) == 0)
//	{
//		SPIBSC0.SPBCR = (SPIBSC0.SPBCR & ~ (SPIBSC_SPBCR_BRDV | SPIBSC_SPBCR_SPBR)) |
//			(0 << SPIBSC_SPBCR_BRDV_SHIFT) |	// 0..3
//			(2 << SPIBSC_SPBCR_SPBR_SHIFT) |	// 0..255
//			0;
//	}

#if WITHISBOOTLOADER
	{
		/* ----  Writing to On-Chip Data-Retention RAM is enabled. ---- */
		// Нельзя отключить - т.к. r7s721_ttb_map работает со страницами по 1 мегабайту
		// Нельзя отключить - botloader не может загрузить программу на выполнение по DFU.
		// Странно, почему? Судя по описанию, области перекрываются...
		CPG.SYSCR3 |= (CPG_SYSCR3_RRAMWE3 | CPG_SYSCR3_RRAMWE2 | CPG_SYSCR3_RRAMWE1 | CPG_SYSCR3_RRAMWE0);
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

		// Нельзя - перестает работать DEBUG
		//SCLR->APER_CLK_CTRL = 0;	// All AMBA Clock control disable

		// PLL только в bootloader.
		// посеольку программа выполняется из DDR RAM, перерпрограммировать PLL нельзя.

		xc7z_arm_pll_initialize();
		xc7z_ddr_pll_initialize();
		xc7z_io_pll_initialize();


		SCLR->SLCR_UNLOCK = 0x0000DF0DU;
		XDCFG->CTRL &= ~ (UINT32_C(1) << 29);	// PCFG_POR_CNT_4K

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

		SCLR->APER_CLK_CTRL |= (UINT32_C(1) << 22);	/* APER_CLK_CTRL.GPIO_CPU_1XCLKACT */

	#endif /* WITHISBOOTLOADER */

#elif CPUSTYLE_A64 && ! WITHISBOOTLOADER_DDR

	/* Off bootloader USB */
	CCU->BUS_SOFT_RST_REG0 &= ~ (UINT32_C(1) << 29);	// USB-OHCI0_RST.
	CCU->BUS_SOFT_RST_REG0 &= ~ (UINT32_C(1) << 25);	// USB-EHCI0_RST.

	CCU->BUS_SOFT_RST_REG0 &= ~ (UINT32_C(1) << 28);	// USB-OTG-OHCI_RST.
	CCU->BUS_SOFT_RST_REG0 &= ~ (UINT32_C(1) << 24);	// USB-OTG-EHCI_RST
	CCU->BUS_SOFT_RST_REG0 &= ~ (UINT32_C(1) << 23);	// USB-OTG-Device_RST.

	/* Off host-only USB */
	CCU->BUS_CLK_GATING_REG0 &= ~ (UINT32_C(1) << 29);	// USBOHCI0_GATING.
	CCU->BUS_CLK_GATING_REG0 &= ~ (UINT32_C(1) << 25);	// USBEHCI0_GATING.

	CCU->BUS_CLK_GATING_REG0 &= ~ (UINT32_C(1) << 28);	// USB-OTG-OHCI_GATING.
	CCU->BUS_CLK_GATING_REG0 &= ~ (UINT32_C(1) << 24);	// USB-OTG-EHCI_GATING.
	CCU->BUS_CLK_GATING_REG0 &= ~ (UINT32_C(1) << 23);	// USB-OTG-Device_GATING.

	CCU->USBPHY_CFG_REG = 0;

	USBPHY0->HCI_ICR = 0;
	USBPHY1->HCI_ICR = 0;

	allwnr_a64_pll_initialize();

	//	The PLL_PERIPH0(1X) = 24MHz*N*K/2.
	//	The PLL_PERIPH0(2X) = 24MHz*N*K.
	allwnr_a64_module_pll_enable(& CCU->PLL_PERIPH0_CTRL_REG);
	//	The PLL_PERIPH1(1X) = 24MHz*N*K/2.
	//	The PLL_PERIPH1(2X) = 24MHz*N*K.
	allwnr_a64_module_pll_enable(& CCU->PLL_PERIPH1_CTRL_REG);

	allwnr_a64_module_pll_enable(& CCU->PLL_VIDEO0_CTRL_REG);
	allwnr_a64_module_pll_enable(& CCU->PLL_VIDEO1_CTRL_REG);
	allwnr_a64_module_pll_enable(& CCU->PLL_VE_CTRL_REG);

	//allwnr_a64_module_pllaudio_enable();
	allwnr_a64_module_pll_enable(& CCU->PLL_AUDIO_CTRL_REG);

	allwnr_a64_module_pll_enable(& CCU->PLL_HSIC_CTRL_REG);

	//CCU->APB2_CFG_REG = 0x02000303;	// PLL_PERIPH0(2X) / 8 / 4 - allwnr_a64_get_apb2_freq()=300 MHz
	CCU->APB2_CFG_REG = 0x02000304;	// PLL_PERIPH0(2X) / 8 / 5- allwnr_a64_get_apb2_freq()=240 MHz
	allwnr_a64_mbus_initialize();

#elif CPUSTYLE_T113

	/* Off bootloader USB */
	if (1)
	{
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 16);	// USBOHCI0_RST
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 20);	// USBEHCI0_RST
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 24);	// USBOTG0_RST

		CCU->USB0_CLK_REG &= ~ (UINT32_C(1) << 31);	// USB0_CLKEN - Gating Special Clock For OHCI0
		CCU->USB0_CLK_REG &= ~ (UINT32_C(1) << 30);	// USBPHY0_RSTN
	}
	allwnrt113_pll_initialize();

#elif CPUSTYLE_F133

	/* Off bootloader USB */
	if (1)
	{
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 16);	// USBOHCI0_RST
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 20);	// USBEHCI0_RST
		CCU->USB_BGR_REG &= ~  (UINT32_C(1) << 24);	// USBOTG0_RST

		CCU->USB0_CLK_REG &= ~  (UINT32_C(1) << 31);	// USB0_CLKEN - Gating Special Clock For OHCI0
		CCU->USB0_CLK_REG &= ~  (UINT32_C(1) << 30);	// USBPHY0_RSTN
	}

	CCU->MBUS_MAT_CLK_GATING_REG |= (UINT32_C(1) << 11);	// RISC_MCLK_EN
	allwnrt113_pll_initialize();
//
	set_pll_riscv_axi(RV_PLL_CPU_N);

	CCU->RISC_CFG_BGR_REG |= (UINT32_C(1) << 16) | (UINT32_C(1) << 0);	// не проищзволит видимого эффекта

	//CCU->RISC_GATING_REG = (1 * 1u << 31) | (0x16AA << 0);

#elif CPUSTYLE_V3S

	/* Off bootloader USB */
	if (1)
	{
//		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 16);	// USBOHCI0_RST
//		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 20);	// USBEHCI0_RST
//		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 24);	// USBOTG0_RST
//
//		CCU->USB0_CLK_REG &= ~ (UINT32_C(1) << 31);	// USB0_CLKEN - Gating Special Clock For OHCI0
//		CCU->USB0_CLK_REG &= ~ (UINT32_C(1) << 30);	// USBPHY0_RSTN
	}
	allwnr_v3s_pll_initialize();


#elif CPUSTYLE_T507 && ! WITHISBOOTLOADER_DDR

	{
		// Disable SD hosts

		CCU->SMHC0_CLK_REG = 0;
		CCU->SMHC1_CLK_REG = 0;
		CCU->SMHC2_CLK_REG = 0;
		CCU->SMHC_BGR_REG = 0;
	}

	{
		// Disable XFEL enabled device

		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 24);	// USBOTG_RST
		CCU->USB0_CLK_REG &= ~ (UINT32_C(1) << 30);	// USBPHY0_RST
		CCU->USB_BGR_REG &= ~ (UINT32_C(1) << 8);	// USBOTG_GATING
	}

	/* IOMMU off */
	{
		IOMMU->IOMMU_RESET_REG &= ~ (UINT32_C(1) << 31);	// IOMMU_RESET
		IOMMU->IOMMU_ENABLE_REG &= ~ (UINT32_C(1) << 0);	// ENABLE
		CCU->IOMMU_BGR_REG &= ~ (UINT32_C(1) << 0);
	}

	CCU->PSI_AHB1_AHB2_CFG_REG = 0;

	set_t507_pll_cpux_axi(forced ? PLL_CPU_N : 17, PLL_CPU_P_POW);

	allwnr_t507_module_pll_enable(& CCU->PLL_PERI0_CTRL_REG);
	allwnr_t507_module_pll_enable(& CCU->PLL_PERI1_CTRL_REG);
	allwnr_t507_module_pll_enable(& CCU->PLL_DE_CTRL_REG);
	allwnr_t507_module_pll_enable(& CCU->PLL_VIDEO0_CTRL_REG);
	allwnr_t507_module_pll_enable(& CCU->PLL_VIDEO1_CTRL_REG);
	allwnr_t507_module_pll_enable(& CCU->PLL_AUDIO_CTRL_REG);
#if WITHGPUHW
	allwnr_t507_module_pll_enable(& CCU->PLL_GPU0_CTRL_REG);
#endif /* WITHGPUHW */

	// [02.507]CPU=1008 MHz,PLL6=600 Mhz,AHB=200 Mhz, APB1=100Mhz  MBus=400Mhz


	// PSI_AHB1_AHB2 CLK = Clock Source/M/N
	// old default=0x03000102
	// The default value of PLL_PERI0(2X) is 1.2 GHz. It is not recommended to modify the value
	// allwnr_t507_get_psi_ahb1_ahb2_freq()=300 MHz
	// is a peripheral bus interconnect device based on AHB and APB protocol
	CCU->PSI_AHB1_AHB2_CFG_REG =
		0x03 * (UINT32_C(1) << 24) |	// 11: PLL_PERI0(1X)
		(1) * (UINT32_C(1) << 8) |		// FACTOR_N (1/2/4/8)
		//(3 - 1) * (UINT32_C(1) << 0) |		// FACTOR_M - AXI divider 1..4
		0;
	//CCU->PSI_AHB1_AHB2_CFG_REG = 0x03000102;	// allwnr_t507_get_psi_ahb1_ahb2_freq()=100 MHz
	CCU->APB2_CFG_REG = 0x03000002;	// allwnr_t507_get_apb2_freq()=200 MHz
	//CCU->APB2_CFG_REG = 0x03000001;	// allwnr_t507_get_apb2_freq()=300 MHz

//	CCU->MBUS_CFG_REG = 0;
//	CCU->MBUS_CFG_REG |= (UINT32_C(1) << 31);	// CLK_GATING
//	CCU->MBUS_CFG_REG |= (UINT32_C(1) << 30);	// MBUS_RST
	CCU->MBUS_CFG_REG = 0xC1000002;	// MBUS freq = 400 MHz (01: PLL_PERI0(2X) / 3)
	//CCU->MBUS_CFG_REG = 0xC0000000;

	CCU->APB1_CFG_REG =
		0x02 * (UINT32_C(1) << 24) |	// 10: PSI
		//(1) * (UINT32_C(1) << 8) |		// FACTOR_N (1/2/4/8)
		(3 - 1) * (UINT32_C(1) << 0) |		// FACTOR_M 1..4
		0;

#if CPUSTYLE_H616
	C0_CPUX_CFG_H616->C0_CTRL_REG0 &= ~ (UINT32_C(1) << 7);	// AXI to MBUS Clock Gating disable, the priority of this bit is higher than bit[6]
	C0_CPUX_CFG_H616->C0_CTRL_REG0 |= (UINT32_C(1) << 6);	// AXI to MBUS Clock Gating enable
#else /* CPUSTYLE_H616 */
	C0_CPUX_CFG_T507->C0_CTRL_REG0 &= ~ (UINT32_C(1) << 7);	// AXI to MBUS Clock Gating disable, the priority of this bit is higher than bit[6]
	C0_CPUX_CFG_T507->C0_CTRL_REG0 |= (UINT32_C(1) << 6);	// AXI to MBUS Clock Gating enable
#endif /* CPUSTYLE_H616 */

#elif CPUSTYLE_VM14
	/* 1892ВМ14Я */

	PMCTR->CORE_PWR_UP = 1;
	CMCTR->GATE_CORE_CTR |= (UINT32_C(1) << 0);	// L0_EN
	CMCTR->GATE_SYS_CTR |= (UINT32_C(1) << 0);	// SYS_EN - Разрешение для тактовых частот L1_HCLK, L3_PCLK и связанных с ними частот

	vm14_pll_initialize();

#else
	//#warning Undefined CPUSTYLE_xxx

#endif

	SystemCoreClock = CPU_FREQ;
}

void SystemCoreClockUpdate(void)
{
	SystemCoreClock = CPU_FREQ;
}


#if (WITHDCDCFREQCTL || WITHBLPWMCTL) && ! LINUX_SUBSYSTEM

//static uint_fast16_t dcdcrefdiv = 62;	/* делится частота внутреннего генератора 48 МГц */
#if (CPUSTYLE_T507 || CPUSTYLE_H616)
	// Note: The working clock of PWM is from APB1 or OSC24M.
	#define PWMTICKSFREQ (allwnr_t507_get_apb1_freq() / 2)	/* Allwinner t507 / H616 */

#elif (CPUSTYLE_T113 || CPUSTYLE_F133)
	#define PWMTICKSFREQ (allwnrt113_get_apb0_freq() / 2)	/* Allwinner t113-s3 */

#elif CPUSTYLE_R7S721
	#define PWMTICKSFREQ (P0CLOCK_FREQ / 2)	/* Renesas RZ-A1x */

#elif CPUSTYLE_STM32MP1
	#define PWMTICKSFREQ 32000000

#else
	//#error Wrong CPUSTYLE_xxx

#endif

struct DCDCFREQ
{
	uint32_t dcdcdiv;
	uint32_t fmin;
	uint32_t fmax;
};

#if CPUSTYLE_R7S721

/* fsync=15000000, wflwidth=96000 */
/* number of dividers=25 13..38 */
/* Analyze up to 50 harmonics. */
#define BOARDDCDCSYNC 15000000 /* DCDC clock frequency */
static const FLASHMEM struct DCDCFREQ dcdcfreqtable [] = {
	{ 13 , 30000   , 1134000 , },	/* dcdc=1153846 Hz */
	{ 14 , 1134000 , 1230000 , },	/* dcdc=1071428 Hz */
	{ 13 , 1230000 , 2286000 , },	/* dcdc=1153846 Hz */
	{ 14 , 2286000 , 2382000 , },	/* dcdc=1071428 Hz */
	{ 13 , 2382000 , 3438000 , },	/* dcdc=1153846 Hz */
	{ 14 , 3438000 , 3534000 , },	/* dcdc=1071428 Hz */
	{ 13 , 3534000 , 4590000 , },	/* dcdc=1153846 Hz */
	{ 14 , 4590000 , 4686000 , },	/* dcdc=1071428 Hz */
	{ 13 , 4686000 , 5742000 , },	/* dcdc=1153846 Hz */
	{ 14 , 5742000 , 5838000 , },	/* dcdc=1071428 Hz */
	{ 13 , 5838000 , 6894000 , },	/* dcdc=1153846 Hz */
	{ 14 , 6894000 , 6990000 , },	/* dcdc=1071428 Hz */
	{ 13 , 6990000 , 8046000 , },	/* dcdc=1153846 Hz */
	{ 14 , 8046000 , 8142000 , },	/* dcdc=1071428 Hz */
	{ 13 , 8142000 , 9198000 , },	/* dcdc=1153846 Hz */
	{ 14 , 9198000 , 9294000 , },	/* dcdc=1071428 Hz */
	{ 13 , 9294000 , 10350000, },	/* dcdc=1153846 Hz */
	{ 14 , 10350000, 10446000, },	/* dcdc=1071428 Hz */
	{ 13 , 10446000, 11502000, },	/* dcdc=1153846 Hz */
	{ 14 , 11502000, 11598000, },	/* dcdc=1071428 Hz */
	{ 13 , 11598000, 12654000, },	/* dcdc=1153846 Hz */
	{ 14 , 12654000, 12750000, },	/* dcdc=1071428 Hz */
	{ 13 , 12750000, 13806000, },	/* dcdc=1153846 Hz */
	{ 14 , 13806000, 13902000, },	/* dcdc=1071428 Hz */
	{ 13 , 13902000, 14958000, },	/* dcdc=1153846 Hz */
	{ 31 , 15047971, 15047971, },	/* dcdc=483870 Hz */
	{ 13 , 15095971, 16151971, },	/* dcdc=1153846 Hz */
	{ 14 , 16151971, 16247971, },	/* dcdc=1071428 Hz */
	{ 13 , 16247971, 17303971, },	/* dcdc=1153846 Hz */
	{ 14 , 17303971, 17399971, },	/* dcdc=1071428 Hz */
	{ 13 , 17399971, 18455971, },	/* dcdc=1153846 Hz */
	{ 14 , 18455971, 18551971, },	/* dcdc=1071428 Hz */
	{ 13 , 18551971, 19607971, },	/* dcdc=1153846 Hz */
	{ 14 , 19607971, 19703971, },	/* dcdc=1071428 Hz */
	{ 13 , 19703971, 20759971, },	/* dcdc=1153846 Hz */
	{ 14 , 20759971, 20855971, },	/* dcdc=1071428 Hz */
	{ 13 , 20855971, 21911971, },	/* dcdc=1153846 Hz */
	{ 14 , 21911971, 22007971, },	/* dcdc=1071428 Hz */
	{ 13 , 22007971, 23063971, },	/* dcdc=1153846 Hz */
	{ 14 , 23063971, 23159971, },	/* dcdc=1071428 Hz */
	{ 13 , 23159971, 24215971, },	/* dcdc=1153846 Hz */
	{ 14 , 24215971, 24311971, },	/* dcdc=1071428 Hz */
	{ 13 , 24311971, 25367971, },	/* dcdc=1153846 Hz */
	{ 14 , 25367971, 25463971, },	/* dcdc=1071428 Hz */
	{ 13 , 25463971, 26519971, },	/* dcdc=1153846 Hz */
	{ 14 , 26519971, 26615971, },	/* dcdc=1071428 Hz */
	{ 13 , 26615971, 27671971, },	/* dcdc=1153846 Hz */
	{ 14 , 27671971, 27767971, },	/* dcdc=1071428 Hz */
	{ 13 , 27767971, 28823971, },	/* dcdc=1153846 Hz */
	{ 14 , 28823971, 28919971, },	/* dcdc=1071428 Hz */
	{ 13 , 28919971, 29975971, },	/* dcdc=1153846 Hz */
	{ 25 , 29975971, 30071971, },	/* dcdc=600000 Hz */
	{ 13 , 30071971, 31127971, },	/* dcdc=1153846 Hz */
	{ 14 , 31127971, 31223971, },	/* dcdc=1071428 Hz */
	{ 13 , 31223971, 32279971, },	/* dcdc=1153846 Hz */
	{ 14 , 32279971, 32375971, },	/* dcdc=1071428 Hz */
	{ 13 , 32375971, 33431971, },	/* dcdc=1153846 Hz */
	{ 14 , 33431971, 33527971, },	/* dcdc=1071428 Hz */
	{ 13 , 33527971, 34583971, },	/* dcdc=1153846 Hz */
	{ 14 , 34583971, 34679971, },	/* dcdc=1071428 Hz */
	{ 13 , 34679971, 35735971, },	/* dcdc=1153846 Hz */
	{ 14 , 35735971, 35831971, },	/* dcdc=1071428 Hz */
	{ 13 , 35831971, 36887971, },	/* dcdc=1153846 Hz */
	{ 14 , 36887971, 36983971, },	/* dcdc=1071428 Hz */
	{ 13 , 36983971, 38039971, },	/* dcdc=1153846 Hz */
	{ 14 , 38039971, 38135971, },	/* dcdc=1071428 Hz */
	{ 13 , 38135971, 39191971, },	/* dcdc=1153846 Hz */
	{ 14 , 39191971, 39287971, },	/* dcdc=1071428 Hz */
	{ 13 , 39287971, 40343971, },	/* dcdc=1153846 Hz */
	{ 14 , 40343971, 40439971, },	/* dcdc=1071428 Hz */
	{ 13 , 40439971, 41495971, },	/* dcdc=1153846 Hz */
	{ 14 , 41495971, 41591971, },	/* dcdc=1071428 Hz */
	{ 13 , 41591971, 42647971, },	/* dcdc=1153846 Hz */
	{ 14 , 42647971, 42743971, },	/* dcdc=1071428 Hz */
	{ 13 , 42743971, 43799971, },	/* dcdc=1153846 Hz */
	{ 14 , 43799971, 43895971, },	/* dcdc=1071428 Hz */
	{ 13 , 43895971, 44999971, },	/* dcdc=1153846 Hz */
	{ 17 , 44999971, 45095971, },	/* dcdc=882352 Hz */
	{ 13 , 45095971, 46151971, },	/* dcdc=1153846 Hz */
	{ 14 , 46151971, 46247971, },	/* dcdc=1071428 Hz */
	{ 13 , 46247971, 47303971, },	/* dcdc=1153846 Hz */
	{ 14 , 47303971, 47399971, },	/* dcdc=1071428 Hz */
	{ 13 , 47399971, 48455971, },	/* dcdc=1153846 Hz */
	{ 14 , 48455971, 48551971, },	/* dcdc=1071428 Hz */
	{ 13 , 48551971, 49607971, },	/* dcdc=1153846 Hz */
	{ 14 , 49607971, 49703971, },	/* dcdc=1071428 Hz */
	{ 13 , 49703971, 50759971, },	/* dcdc=1153846 Hz */
	{ 14 , 50759971, 50855971, },	/* dcdc=1071428 Hz */
	{ 13 , 50855971, 51911971, },	/* dcdc=1153846 Hz */
	{ 14 , 51911971, 52007971, },	/* dcdc=1071428 Hz */
	{ 13 , 52007971, 53063971, },	/* dcdc=1153846 Hz */
	{ 14 , 53063971, 53159971, },	/* dcdc=1071428 Hz */
	{ 13 , 53159971, 54000000, },	/* dcdc=1153846 Hz */
};

#elif CPUSTYLE_STM32H7XX
// пока для проверки работоспособности. Таблицу надо расчитать.
#define BOARDDCDCSYNC 333333 /* DCDC clock frequency */
static const FLASHMEM struct DCDCFREQ dcdcfreqtable [] = {
  { 63, 26900000uL,  UINT32_MAX },
  { 63, 6900000uL,  26900000uL },
  { 62, 0,		6900000uL },
};

#elif WITHPS7BOARD_MYC_Y7Z020

/* fsync=61440000, wflwidth=96000 */
/* number of dividers=104 51..154 */
/* Analyze up to 50 harmonics. */
#define BOARDDCDCSYNC 61440000 /* DCDC clock frequency */
static const FLASHMEM struct DCDCFREQ dcdcfreqtable [] = {
        { 51 , 30000   , 1182000 , },   /* dcdc=1204705 Hz visible=no */
        { 55 , 1182000 , 1230000 , },   /* dcdc=1117090 Hz visible=no */
        { 52 , 1230000 , 1278000 , },   /* dcdc=1181538 Hz visible=no */
        { 51 , 1278000 , 2382000 , },   /* dcdc=1204705 Hz visible=no */
        { 53 , 2382000 , 2430000 , },   /* dcdc=1159245 Hz visible=no */
        { 52 , 2430000 , 2478000 , },   /* dcdc=1181538 Hz visible=no */
        { 51 , 2478000 , 3582000 , },   /* dcdc=1204705 Hz visible=no */
        { 53 , 3582000 , 3630000 , },   /* dcdc=1159245 Hz visible=no */
        { 52 , 3630000 , 3678000 , },   /* dcdc=1181538 Hz visible=no */
        { 51 , 3678000 , 4782000 , },   /* dcdc=1204705 Hz visible=no */
        { 52 , 4782000 , 4878000 , },   /* dcdc=1181538 Hz visible=no */
        { 51 , 4878000 , 5982000 , },   /* dcdc=1204705 Hz visible=no */
        { 52 , 5982000 , 6078000 , },   /* dcdc=1181538 Hz visible=no */
        { 51 , 6078000 , 7182000 , },   /* dcdc=1204705 Hz visible=no */
        { 52 , 7182000 , 7278000 , },   /* dcdc=1181538 Hz visible=no */
        { 51 , 7278000 , 8430000 , },   /* dcdc=1204705 Hz visible=no */
        { 52 , 8430000 , 8526000 , },   /* dcdc=1181538 Hz visible=no */
        { 51 , 8526000 , 9630000 , },   /* dcdc=1204705 Hz visible=no */
        { 52 , 9630000 , 9726000 , },   /* dcdc=1181538 Hz visible=no */
        { 51 , 9726000 , 10830000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 10830000, 10926000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 10926000, 12030000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 12030000, 12126000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 12126000, 13230000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 13230000, 13326000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 13326000, 14430000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 14430000, 14526000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 14526000, 15630000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 15630000, 15726000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 15726000, 16830000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 16830000, 16926000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 16926000, 18030000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 18030000, 18126000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 18126000, 19230000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 19230000, 19326000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 19326000, 20478000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 20478000, 20574000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 20574000, 21678000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 21678000, 21774000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 21774000, 22878000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 22878000, 22974000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 22974000, 24078000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 24078000, 24174000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 24174000, 25278000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 25278000, 25374000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 25374000, 26478000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 26478000, 26574000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 26574000, 27678000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 27678000, 27774000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 27774000, 28878000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 28878000, 28974000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 28974000, 30078000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 30078000, 30174000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 30174000, 31278000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 31278000, 31374000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 31374000, 32526000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 32526000, 32622000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 32622000, 33726000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 33726000, 33822000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 33822000, 34926000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 34926000, 35022000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 35022000, 36126000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 36126000, 36222000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 36222000, 37326000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 37326000, 37422000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 37422000, 38526000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 38526000, 38622000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 38622000, 39726000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 39726000, 39822000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 39822000, 40926000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 40926000, 41022000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 41022000, 42126000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 42126000, 42222000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 42222000, 43326000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 43326000, 43422000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 43422000, 44574000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 44574000, 44670000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 44670000, 45774000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 45774000, 45870000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 45870000, 46974000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 46974000, 47070000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 47070000, 48174000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 48174000, 48270000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 48270000, 49374000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 49374000, 49470000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 49470000, 50574000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 50574000, 50670000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 50670000, 51774000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 51774000, 51870000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 51870000, 52974000, },   /* dcdc=1204705 Hz visible=no */
        { 52 , 52974000, 53070000, },   /* dcdc=1181538 Hz visible=no */
        { 51 , 53070000, 54000000, },   /* dcdc=1204705 Hz visible=no */
};


#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_T507 || CPUSTYLE_H616)
/* fsync=50000000, wflwidth=96000 */
/* number of dividers=84 42..125 */
/* Analyze up to 50 harmonics. */
#define BOARDDCDCSYNC 50000000 /* DCDC clock frequency */
static const FLASHMEM struct DCDCFREQ dcdcfreqtable [] = {
        { 42 , 30000   , 1182000 , },   /* dcdc=1190476 Hz visible=no */
        { 45 , 1182000 , 1230000 , },   /* dcdc=1111111 Hz visible=no */
        { 43 , 1230000 , 1278000 , },   /* dcdc=1162790 Hz visible=no */
        { 42 , 1278000 , 2334000 , },   /* dcdc=1190476 Hz visible=no */
        { 44 , 2334000 , 2382000 , },   /* dcdc=1136363 Hz visible=no */
        { 43 , 2382000 , 2430000 , },   /* dcdc=1162790 Hz visible=no */
        { 42 , 2430000 , 3534000 , },   /* dcdc=1190476 Hz visible=no */
        { 44 , 3534000 , 3582000 , },   /* dcdc=1136363 Hz visible=no */
        { 43 , 3582000 , 3630000 , },   /* dcdc=1162790 Hz visible=no */
        { 42 , 3630000 , 4734000 , },   /* dcdc=1190476 Hz visible=no */
        { 43 , 4734000 , 4830000 , },   /* dcdc=1162790 Hz visible=no */
        { 42 , 4830000 , 5934000 , },   /* dcdc=1190476 Hz visible=no */
        { 43 , 5934000 , 6030000 , },   /* dcdc=1162790 Hz visible=no */
        { 42 , 6030000 , 7134000 , },   /* dcdc=1190476 Hz visible=no */
        { 43 , 7134000 , 7230000 , },   /* dcdc=1162790 Hz visible=no */
        { 42 , 7230000 , 8286000 , },   /* dcdc=1190476 Hz visible=no */
        { 43 , 8286000 , 8382000 , },   /* dcdc=1162790 Hz visible=no */
        { 42 , 8382000 , 9486000 , },   /* dcdc=1190476 Hz visible=no */
        { 43 , 9486000 , 9582000 , },   /* dcdc=1162790 Hz visible=no */
        { 42 , 9582000 , 10686000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 10686000, 10782000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 10782000, 11886000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 11886000, 11982000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 11982000, 13086000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 13086000, 13182000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 13182000, 14238000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 14238000, 14334000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 14334000, 15438000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 15438000, 15534000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 15534000, 16638000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 16638000, 16734000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 16734000, 17838000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 17838000, 17934000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 17934000, 19038000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 19038000, 19134000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 19134000, 20238000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 20238000, 20334000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 20334000, 21390000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 21390000, 21486000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 21486000, 22590000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 22590000, 22686000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 22686000, 23790000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 23790000, 23886000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 23886000, 24990000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 24990000, 25086000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 25086000, 26190000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 26190000, 26286000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 26286000, 27342000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 27342000, 27438000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 27438000, 28542000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 28542000, 28638000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 28638000, 29742000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 29742000, 29838000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 29838000, 30942000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 30942000, 31038000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 31038000, 32142000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 32142000, 32238000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 32238000, 33294000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 33294000, 33390000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 33390000, 34494000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 34494000, 34590000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 34590000, 35694000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 35694000, 35790000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 35790000, 36894000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 36894000, 36990000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 36990000, 38094000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 38094000, 38190000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 38190000, 39246000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 39246000, 39342000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 39342000, 40446000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 40446000, 40542000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 40542000, 41646000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 41646000, 41742000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 41742000, 42846000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 42846000, 42942000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 42942000, 44046000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 44046000, 44142000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 44142000, 45198000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 45198000, 45294000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 45294000, 46398000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 46398000, 46494000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 46494000, 47598000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 47598000, 47646000, },   /* dcdc=1162790 Hz visible=no */
        { 44 , 47646000, 47694000, },   /* dcdc=1136363 Hz visible=no */
        { 42 , 47694000, 48798000, },   /* dcdc=1190476 Hz visible=no */
        { 44 , 48798000, 48846000, },   /* dcdc=1136363 Hz visible=no */
        { 46 , 48846000, 48894000, },   /* dcdc=1086956 Hz visible=no */
        { 42 , 48894000, 49998000, },   /* dcdc=1190476 Hz visible=no */
        { 50 , 49998000, 50094000, },   /* dcdc=1000000 Hz visible=no */
        { 42 , 50094000, 51150000, },   /* dcdc=1190476 Hz visible=no */
        { 46 , 51150000, 51198000, },   /* dcdc=1086956 Hz visible=no */
        { 44 , 51198000, 51246000, },   /* dcdc=1136363 Hz visible=no */
        { 42 , 51246000, 52350000, },   /* dcdc=1190476 Hz visible=no */
        { 44 , 52350000, 52398000, },   /* dcdc=1136363 Hz visible=no */
        { 43 , 52398000, 52446000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 52446000, 53550000, },   /* dcdc=1190476 Hz visible=no */
        { 43 , 53550000, 53646000, },   /* dcdc=1162790 Hz visible=no */
        { 42 , 53646000, 54000000, },   /* dcdc=1190476 Hz visible=no */
};

#elif CPUSTYLE_STM32MP1
//const uint_fast32_t ifreq = stm32mp1_get_timg2_freq();	// TIM17 это timg2
//PRINTF("hardware_dcdc_calcdivider: ifreq=%lu\n", ifreq);
// сейчас делит 32 MHz
/* fsync=32000000, wflwidth=96000 */
/* number of dividers=53 27..80 */
/* Analyze up to 50 harmonics. */
#define BOARDDCDCSYNC 32000000 /* DCDC clock frequency */
static const FLASHMEM struct DCDCFREQ dcdcfreqtable [] = {
	{ 27 , 30000   , 1182000 , },	/* dcdc=1185185 Hz */
	{ 29 , 1182000 , 1230000 , },	/* dcdc=1103448 Hz */
	{ 28 , 1230000 , 1278000 , },	/* dcdc=1142857 Hz */
	{ 27 , 1278000 , 2334000 , },	/* dcdc=1185185 Hz */
	{ 28 , 2334000 , 2430000 , },	/* dcdc=1142857 Hz */
	{ 27 , 2430000 , 3534000 , },	/* dcdc=1185185 Hz */
	{ 28 , 3534000 , 3630000 , },	/* dcdc=1142857 Hz */
	{ 27 , 3630000 , 4734000 , },	/* dcdc=1185185 Hz */
	{ 28 , 4734000 , 4830000 , },	/* dcdc=1142857 Hz */
	{ 27 , 4830000 , 5886000 , },	/* dcdc=1185185 Hz */
	{ 28 , 5886000 , 5982000 , },	/* dcdc=1142857 Hz */
	{ 27 , 5982000 , 7086000 , },	/* dcdc=1185185 Hz */
	{ 28 , 7086000 , 7182000 , },	/* dcdc=1142857 Hz */
	{ 27 , 7182000 , 8286000 , },	/* dcdc=1185185 Hz */
	{ 28 , 8286000 , 8382000 , },	/* dcdc=1142857 Hz */
	{ 27 , 8382000 , 9438000 , },	/* dcdc=1185185 Hz */
	{ 28 , 9438000 , 9534000 , },	/* dcdc=1142857 Hz */
	{ 27 , 9534000 , 10638000, },	/* dcdc=1185185 Hz */
	{ 28 , 10638000, 10734000, },	/* dcdc=1142857 Hz */
	{ 27 , 10734000, 11838000, },	/* dcdc=1185185 Hz */
	{ 28 , 11838000, 11934000, },	/* dcdc=1142857 Hz */
	{ 27 , 11934000, 12990000, },	/* dcdc=1185185 Hz */
	{ 28 , 12990000, 13086000, },	/* dcdc=1142857 Hz */
	{ 27 , 13086000, 14190000, },	/* dcdc=1185185 Hz */
	{ 28 , 14190000, 14286000, },	/* dcdc=1142857 Hz */
	{ 27 , 14286000, 15390000, },	/* dcdc=1185185 Hz */
	{ 28 , 15390000, 15486000, },	/* dcdc=1142857 Hz */
	{ 27 , 15486000, 16590000, },	/* dcdc=1185185 Hz */
	{ 28 , 16590000, 16686000, },	/* dcdc=1142857 Hz */
	{ 27 , 16686000, 17742000, },	/* dcdc=1185185 Hz */
	{ 28 , 17742000, 17838000, },	/* dcdc=1142857 Hz */
	{ 27 , 17838000, 18942000, },	/* dcdc=1185185 Hz */
	{ 28 , 18942000, 19038000, },	/* dcdc=1142857 Hz */
	{ 27 , 19038000, 20142000, },	/* dcdc=1185185 Hz */
	{ 28 , 20142000, 20238000, },	/* dcdc=1142857 Hz */
	{ 27 , 20238000, 21294000, },	/* dcdc=1185185 Hz */
	{ 28 , 21294000, 21390000, },	/* dcdc=1142857 Hz */
	{ 27 , 21390000, 22494000, },	/* dcdc=1185185 Hz */
	{ 28 , 22494000, 22590000, },	/* dcdc=1142857 Hz */
	{ 27 , 22590000, 23694000, },	/* dcdc=1185185 Hz */
	{ 28 , 23694000, 23790000, },	/* dcdc=1142857 Hz */
	{ 27 , 23790000, 24846000, },	/* dcdc=1185185 Hz */
	{ 28 , 24846000, 24942000, },	/* dcdc=1142857 Hz */
	{ 27 , 24942000, 26046000, },	/* dcdc=1185185 Hz */
	{ 28 , 26046000, 26142000, },	/* dcdc=1142857 Hz */
	{ 27 , 26142000, 27246000, },	/* dcdc=1185185 Hz */
	{ 28 , 27246000, 27342000, },	/* dcdc=1142857 Hz */
	{ 27 , 27342000, 28398000, },	/* dcdc=1185185 Hz */
	{ 28 , 28398000, 28494000, },	/* dcdc=1142857 Hz */
	{ 27 , 28494000, 29598000, },	/* dcdc=1185185 Hz */
	{ 28 , 29598000, 29694000, },	/* dcdc=1142857 Hz */
	{ 27 , 29694000, 30798000, },	/* dcdc=1185185 Hz */
	{ 28 , 30798000, 30846000, },	/* dcdc=1142857 Hz */
	{ 29 , 30846000, 30894000, },	/* dcdc=1103448 Hz */
	{ 27 , 30894000, 31998000, },	/* dcdc=1185185 Hz */
	{ 50 , 31998000, 32094000, },	/* dcdc=640000 Hz */
	{ 27 , 32094000, 33150000, },	/* dcdc=1185185 Hz */
	{ 30 , 33150000, 33198000, },	/* dcdc=1066666 Hz */
	{ 28 , 33198000, 33246000, },	/* dcdc=1142857 Hz */
	{ 27 , 33246000, 34350000, },	/* dcdc=1185185 Hz */
	{ 28 , 34350000, 34446000, },	/* dcdc=1142857 Hz */
	{ 27 , 34446000, 35550000, },	/* dcdc=1185185 Hz */
	{ 28 , 35550000, 35646000, },	/* dcdc=1142857 Hz */
	{ 27 , 35646000, 36702000, },	/* dcdc=1185185 Hz */
	{ 28 , 36702000, 36798000, },	/* dcdc=1142857 Hz */
	{ 27 , 36798000, 37902000, },	/* dcdc=1185185 Hz */
	{ 28 , 37902000, 37998000, },	/* dcdc=1142857 Hz */
	{ 27 , 37998000, 39102000, },	/* dcdc=1185185 Hz */
	{ 28 , 39102000, 39198000, },	/* dcdc=1142857 Hz */
	{ 27 , 39198000, 40254000, },	/* dcdc=1185185 Hz */
	{ 28 , 40254000, 40350000, },	/* dcdc=1142857 Hz */
	{ 27 , 40350000, 41454000, },	/* dcdc=1185185 Hz */
	{ 28 , 41454000, 41550000, },	/* dcdc=1142857 Hz */
	{ 27 , 41550000, 42654000, },	/* dcdc=1185185 Hz */
	{ 28 , 42654000, 42750000, },	/* dcdc=1142857 Hz */
	{ 27 , 42750000, 43806000, },	/* dcdc=1185185 Hz */
	{ 28 , 43806000, 43902000, },	/* dcdc=1142857 Hz */
	{ 27 , 43902000, 45006000, },	/* dcdc=1185185 Hz */
	{ 28 , 45006000, 45102000, },	/* dcdc=1142857 Hz */
	{ 27 , 45102000, 46206000, },	/* dcdc=1185185 Hz */
	{ 28 , 46206000, 46302000, },	/* dcdc=1142857 Hz */
	{ 27 , 46302000, 47406000, },	/* dcdc=1185185 Hz */
	{ 28 , 47406000, 47502000, },	/* dcdc=1142857 Hz */
	{ 27 , 47502000, 48558000, },	/* dcdc=1185185 Hz */
	{ 28 , 48558000, 48654000, },	/* dcdc=1142857 Hz */
	{ 27 , 48654000, 49758000, },	/* dcdc=1185185 Hz */
	{ 28 , 49758000, 49854000, },	/* dcdc=1142857 Hz */
	{ 27 , 49854000, 50958000, },	/* dcdc=1185185 Hz */
	{ 28 , 50958000, 51054000, },	/* dcdc=1142857 Hz */
	{ 27 , 51054000, 52110000, },	/* dcdc=1185185 Hz */
	{ 28 , 52110000, 52206000, },	/* dcdc=1142857 Hz */
	{ 27 , 52206000, 53310000, },	/* dcdc=1185185 Hz */
	{ 28 , 53310000, 53406000, },	/* dcdc=1142857 Hz */
	{ 27 , 53406000, 54000000, },	/* dcdc=1185185 Hz */
};
#endif

#if WITHDCDCFREQCTL
/*
	получение делителя частоты для синхронизации DC-DC конверторов
	для исключения попадания в полосу обзора панорамы гармоник этой частоты.
*/
uint_fast32_t
hardware_dcdc_calcdivider(
	uint_fast32_t freq
	)
{
	//PRINTF("hardware_dcdc_calcdivider: PWMTICKSFREQ=%u\n", (unsigned) PWMTICKSFREQ);
	//ASSERT(BOARDDCDCSYNC == PWMTICKSFREQ);	/* проверка что таблица расчитывалась именно для этой входной частоты PWM */
//	#define WITHHWDCDCFREQMIN 400000L
//	#define WITHHWDCDCFREQMAX 1200000L
//	#define DCDC_FREQ_0 960000
//	#define DCDC_FREQ_1 1200000
//		// set DC-DC Sync freq
//		uint32_t dcdc_offset_0 = abs((int32_t)DCDC_FREQ_0 / 2 - freq % (int32_t)DCDC_FREQ_0);
//		uint32_t dcdc_offset_1 = abs((int32_t)DCDC_FREQ_1 / 2 - freq % (int32_t)DCDC_FREQ_1);
//		if (dcdc_offset_0 > dcdc_offset_1)
//			TRX_DCDC_Freq = 1;
//		else
//			TRX_DCDC_Freq = 0;
#if WITHPS7BOARD_MYC_Y7Z020

	return 1;
#elif CPUSTYLE_R7S721
	// пока до проверки работоспособности.
	return calcdivround_p0clock(760000 * 2);    // на выходе формирователя делитель на 2 - требуемую частоту умножаем на два
#endif

	uint_fast8_t high = ARRAY_SIZE(dcdcfreqtable);
	uint_fast8_t low = 0;
	uint_fast8_t middle = 0;	// результат поиска

	// Двоичный поиск
	while (low < high)
	{
		middle = (high - low) / 2 + low;
		if (freq < dcdcfreqtable [middle].fmin)	// нижняя граница не включается - для обеспечения формального попадания частоты DCO в рабочий диапазон
			low = middle + 1;
		else if (freq >= dcdcfreqtable [middle].fmax)
			high = middle;		// переходим к поиску в меньших индексах
		else
			goto found;
	}

found:
	return dcdcfreqtable [middle].dcdcdiv;
}
#endif /* WITHDCDCFREQCTL */

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
		TIM16->PSC = ((UINT32_C(1) << prei) - 1) & TIM_PSC_PSC;

		TIM16->ARR = value;
		//TIM16->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости - если счётчик успевал превысить значение ARR - считал до конца */

		//TIM16->CCR1 = (value / 2) & TIM_CCR1_CCR1;	// TIM16_CH1 - wave output
		//TIM16->ARR = value;
		TIM16->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE;	/* разрешить перезагрузку и включить таймер */
	}

#elif CPUSTYLE_STM32MP1

	void hardware_dcdcfreq_tim17_ch1_initialize(void)
	{
		const uint_fast32_t ifreq = stm32mp1_get_timg2_freq();	// TIM17 это timg2
		PRINTF("hardware_dcdcfreq_tim17_ch1_initialize: ifreq=%u\n", ifreq);
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
		TIM17->PSC = ((UINT32_C(1) << prei) - 1) & TIM_PSC_PSC;

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

#elif (CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_T507)

	void hardware_dcdcfreq_pwm_initialize(unsigned pwmch)
	{
		//PRINTF("hardware_dcdcfreq_pwm_initialize: pwmch=%u\n", pwmch);
	#if CPUSTYLE_T507 || CPUSTYLE_H616
		unsigned value;
		const uint_fast8_t prei = calcdivider(calcdivround2(allwnr_t507_get_apb1_freq(), PWMTICKSFREQ), ALLWNR_PWM_WIDTH, ALLWNR_PWM_TAPS, & value, 1);
		//PRINTF("hardware_dcdcfreq_pwm_initialize: allwnrt113_get_apb0_freq()=%lu, prei=%u, divider=%u\n", allwnrt113_get_apb0_freq(), prei, value);
		CCU->PWM_BGR_REG |= (UINT32_C(1) << 0);	// PWM_GATING
		CCU->PWM_BGR_REG |= (UINT32_C(1) << 16);	// PWM_RST

		//PRINTF("prei=%u, value=%u\n", prei, value);
		// 9.10.4.1 Configuring Clock
		//	Step 1 PWM gating: When using PWM, write 1 to PCGR[PWMx_CLK_GATING].
		PWM->PCCR [pwmch / 2] = 0;
		PWM->PCCR [pwmch / 2] |= 0x01 * (UINT32_C(1) << 7);	// APB1
		PWM->PCCR [pwmch / 2] |= prei * (UINT32_C(1) << 0);	// PWM01_CLK_DIV_M
		//PWM->PCCR [pwmch / 2] |= (UINT32_C(1) << (5 + (pwmch % 2)));	/* PWM01_CLK_SRC_BYPASS_TO_PWM0 - не использовать делитель */
		PWM->PCCR [pwmch / 2] |= (UINT32_C(1) << 4);	/* PWM01_CLK_GATING */
		PWM->CH [pwmch].PCR = (PWM->CH [pwmch].PCR & ~ (UINT32_C(0xFF) << 0) & ~ (UINT32_C(1) << 9)) |
			0 * (UINT32_C(1) << 9) | /* PWM_MODE 0: Cycle mode */
			value * (UINT32_C(1) << 0) | /* PWM_PRESCAL_K */
			0;

	#elif (CPUSTYLE_T113 || CPUSTYLE_F133)
		unsigned value;
		const uint_fast8_t prei = calcdivider(calcdivround2(allwnrt113_get_apb0_freq(), PWMTICKSFREQ), ALLWNR_PWM_WIDTH, ALLWNR_PWM_TAPS, & value, 1);
		//PRINTF("hardware_dcdcfreq_pwm_initialize: allwnrt113_get_apb0_freq()=%lu, prei=%u, divider=%u\n", allwnrt113_get_apb0_freq(), prei, value);
		CCU->PWM_BGR_REG |= (UINT32_C(1) << 0);	// PWM_GATING
		CCU->PWM_BGR_REG |= (UINT32_C(1) << 16);	// PWM_RST
		// 9.10.4.1 Configuring Clock
		//	Step 1 PWM gating: When using PWM, write 1 to PCGR[PWMx_CLK_GATING].
		PWM->PCGR |= (UINT32_C(1) << (0 + pwmch));	/* PWM5_CLK_GATING */

		//	Step 2 PWM clock source select: Set PCCR01[PWM01_CLK_SRC] to select HOSC or APB0 clock.
		//	Step 3 PWM clock divider: Set PCCR01[PWM01_CLK_DIV_M] to select different frequency division coefficient (1/2/4/8/16/32/64/128/256).
		PWM->PCCR [pwmch / 2] = (PWM->PCCR [pwmch / 2] & ~ ((0x03u << 7) | (0x0Fu << 0))) |
			0x01 * (UINT32_C(1) << 7) |	/* 00: HOSC 01: APB0 */
			(1 << prei) * (UINT32_C(1) << 0) | /* Clock Divide M */
			0;
		//	Step 4 PWM clock bypass: Set PCGR[PWM_CLK_SRC_BYPASS_TO_PWM] to 1, output the PWM clock after the secondary frequency division to the corresponding PWM output pin.
		//PWM->PCGR |= (UINT32_C(1) << (16 + IX));	/* PWM5_CLK_BYPASS */
		//	Step 5 PWM internal clock configuration: Set PCR[PWM_PRESCAL_K] to select any frequency division coefficient from 1 to 256.
		PWM->CH [pwmch].PCR = (PWM->CH [pwmch].PCR & ~ ((UINT32_C(0xFF) << 0) | (UINT32_C(1) << 9))) |
			0 * (UINT32_C(1) << 9) | /* PWM_MODE 0: Cycle mode */
			value * (UINT32_C(1) << 0) | /* PWM_PRESCAL_K */
			0;
		// 9.10.4.2 Configuring PWM
		//	Step 1 PWM mode: Set PCR[PWM_MODE] to select cycle mode or pulse mode, if pulse mode, PCR[PWM_PUL_NUM] needs to be configured.
		//	Step 2 PWM active level: Set PCR[PWM_ACT_STA] to select a low level or high level.
		//	Step 3 PWM duty-cycle: Configure PPR[PWM_ENTIRE_CYCLE] and PPR[PWM_ACT_CYCLE] after clock gating is opened.
//		unsigned pwmoutfreq = 1200000;
//		unsigned cycle = calcdivround2(pwm5ticksfreq, pwmoutfreq);
//		PWM->CH [IX].PPR =
//			(cycle - 1) * (UINT32_C(1) << 16) |	/* PWM_ENTIRE_CYCLE */
//			(cycle / 2) * (UINT32_C(1) << 0) |	/* PWM_ACT_CYCLE */
//			0;
		//	Step 4 PWM starting/stoping phase: Configure PCNTR[PWM_COUNTER_START] after the clock gating is enabled and before the PWM is enabled. You can verify whether the configuration was successful by reading back PCNTR[PWM_COUNTER_STATUS].
		//	Step 5 Enable PWM: Configure PER to select the corresponding PWM enable bit; when selecting pulse mode, PCR[PWM_PUL_START] needs to be enabled.
//		PWM->PER |= (UINT32_C(1) << (0 + IX));
	#else
		#error Wrong CPUSTYLE_xxx
	#endif
	}

	void hardware_dcdcfreq_pwm_setdiv(unsigned pwmch, uint_fast32_t cycle)
	{
		//PRINTF("hardware_dcdcfreq_pwm_setdiv: pwmch=%u, cycle=%u (PER=%08u)\n", pwmch, (unsigned) cycle, (unsigned) PWM->PER);
		PWM->PER |= (UINT32_C(1) << (0 + pwmch));
		PWM->CH [pwmch].PPR =
			(cycle - 1) * (UINT32_C(1) << 16) |	/* PWM_ENTIRE_CYCLE */
			(cycle / 2) * (UINT32_C(1) << 0) |	/* PWM_ACT_CYCLE */
			0;
		// 0: PWM period register is ready to write
		while ((PWM->CH [pwmch].PCR & (UINT32_C(1) << 11)) != 0)	/* PWM_PERIOD_RDY */
			;
	}

	// d: 0..100 - требуемое заполнение выходного сигнала в проценнтах
	void hardware_bl_pwm_set_duty(unsigned pwmch, uint_fast32_t freq, uint_fast32_t d)
	{
		const unsigned cycle = calcdivround2(PWMTICKSFREQ, freq);
		const unsigned duty = cycle - cycle * d / 100;
		//PRINTF("hardware_bl_pwm_set_duty: pwmch=%u, cycle=%u, duty=%u\n", pwmch, (unsigned) cycle, (unsigned) duty);
		PWM->PER |= (UINT32_C(1) << (0 + pwmch));
		PWM->CH [pwmch].PPR =
			(cycle - 1) * (UINT32_C(1) << 16) |	/* PWM_ENTIRE_CYCLE */
			(duty) * (UINT32_C(1) << 0) |	/* PWM_ACT_CYCLE */
			0;
		// 0: PWM period register is ready to write
		while ((PWM->CH [pwmch].PCR & (UINT32_C(1) << 11)) != 0)	/* PWM_PERIOD_RDY */
			;
	}


#endif

#endif /* WITHDCDCFREQCTL */



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
	PRINTF(PSTR("hardware_adc_initialize\n"));

#if CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S

    PMC->PMC_PCER0 = (UINT32_C(1) << ID_ADC);		// разрешить тактовую для ADC
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
	const uint_fast32_t tADCnS = (1000000000UL + (ADC_FREQ / 2)) / ADC_FREQ;	// Количество наносекунд в периоде частоты ADC_FREQ
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

	AT91C_BASE_PMC->PMC_PCER = (UINT32_C(1) << AT91C_ID_ADC);		// разрешить тактовую для ADC
	AT91C_BASE_ADC->ADC_CR = AT91C_ADC_SWRST;	// reset ADC

	// adc characteristics: in 10 bit mode ADC clock max is 5 MHz, in 8 bit mode - 8 MHz
	// Track and Hold Acquisition Time - 600 nS

	unsigned prescal;
	calcdivider(calcdivround2(CPU_FREQ, ADC_FREQ), AT91SAM7_ADC_PRESCAL_WIDTH, AT91SAM7_ADC_PRESCAL_TAPS, & prescal, 1);
	const uint_fast32_t tADCnS = (1000000000UL + (ADC_FREQ / 2)) / ADC_FREQ;	// Количество наносекунд в периоде частоты ADC_FREQ
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
	AT91C_BASE_AIC->AIC_IDCR = (UINT32_C(1) << AT91C_ID_ADC);
	AT91C_BASE_AIC->AIC_SVR [AT91C_ID_ADC] = (AT91_REG) AT91F_ADC_IRQHandler;
	AT91C_BASE_AIC->AIC_SMR [AT91C_ID_ADC] =
		(AT91C_AIC_SRCTYPE & AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL) |
		(AT91C_AIC_PRIOR & AT91C_AIC_PRIOR_LOWEST);
	AT91C_BASE_AIC->AIC_ICCR = (UINT32_C(1) << AT91C_ID_ADC);		// clear pending interrupt
	AT91C_BASE_AIC->AIC_IECR = (UINT32_C(1) << AT91C_ID_ADC);	// enable inerrupt

#elif CPUSTYLE_ATMEGA

	// Использование автоматического расчёта предделителя
	// Хотя, 128 (prei = 6) годится для всех частот - 8 МГц и выше. Ниже - уменьшаем.
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, ADC_FREQ), ATMEGA_ADPS_WIDTH, ATMEGA_ADPS_TAPS, & value, 1);

	#if CPUSTYLE_ATMEGA_XXX4

		DIDR0 = build_adc_mask();	// запретить цифровые входы на входах АЦП
		ADCSRA = (UINT32_C(1) << ADEN) | (UINT32_C(1) << ADIE ) | prei;

	#else /* CPUSTYLE_ATMEGA_XXX4 */

		ADCSRA = (UINT32_C(1) << ADEN) | (UINT32_C(1) << ADIE ) | prei;


	#endif	/* CPUSTYLE_ATMEGA_XXX4 */

#elif CPUSTYLE_ATXMEGAXXXA4

	#warning TODO: write atxmega code - ADC init
	// Использование автоматического расчёта предделителя
	// Хотя, 128 (prei = 6) годится для всех частот - 8 МГц и выше. Ниже - уменьшаем.
	////unsigned value;
	////const uint_fast8_t prei = calcdivider(calcdivround2(CPU_FREQ, ADC_FREQ), ATXMEGA_ADPS_WIDTH, ATXMEGA_ADPS_TAPS, & value, 1);

	////ADCA.PRESCALER = prei;
	//DIDR0 = build_adc_mask();	// запретить цифровые входы на входах АЦП
	//ADCSRA = (UINT32_C(1) << ADEN) | (UINT32_C(1) << ADIE );

#if 0
		// код из electronix.ru
	/* инициализация АЦП */
	ADCA.CTRLA = 0x05;
	ADCA.CTRLB = 0x00;
	ADCA.PRESCALER = ADC_PRESCALER_DIV256_gc; // 0x6; // ADC_PRESCALER_DIV256_gc
	ADCA.CH1.CTRL =	ADC_CH_INPUTMODE_SINGLEENDED_gc; // 0x01;


	uint16_t get_adc()
	{
		ADCA.REFCTRL |= (UINT32_C(1) << 5); // подаем смещение с пина AREFA
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

		adc->PCSEL |= ((UINT32_C(1) << adcmap->ch) << ADC_PCSEL_PCSEL_Pos) & ADC_PCSEL_PCSEL_Msk;

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

	CPG.STBCR6 &= ~ (UINT32_C(1) << CPG_STBCR6_MSTP67_SHIFT);	// Module Stop 67 0: The A/D converter runs.
	(void) CPG.STBCR6;			/* Dummy read */

	const uint_fast32_t ainmask = build_adc_mask();

	ADC.ADCSR =
		1 * (UINT32_C(1) << ADC_SR_ADIE_SHIFT) |	// ADIE - 1: A/D conversion end interrupt (ADI) request is enabled
		3 * (UINT32_C(1) << ADC_SR_CKS_SHIFT) |	// CKS[2..0] - Clock Select - 011: Conversion time = 382 tcyc (maximum)
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

#elif CPUSTYLE_T113 || CPUSTYLE_F133

	//#warning GPADC need to implement at CPUSTYLE_T113


	(void) GPADC;

#elif CPUSTYLE_A64
	//#warning Undefined CPUSTYLE_A64

#elif CPUSTYLE_T507
	//#warning Undefined CPUSTYLE_T507


#elif CPUSTYLE_V3S
	//#warning Undefined CPUSTYLE_V3S

#else
	#warning Undefined CPUSTYLE_XXX
#endif

	//PRINTF(PSTR("hardware_adc_initialize done\n"));
}

#endif /* WITHCPUADCHW */


#if SIDETONE_TARGET_BIT != 0


#if CPUSTYLE_ATMEGA

	#if CPUSTYLE_ATMEGA_XXX4
		//enum { TCCR2A_WORK = (UINT32_C(1) << COM2A0) | (UINT32_C(1) << WGM21), TCCR2A_DISCONNECT = (UINT32_C(1) << WGM21) };
	#elif CPUSTYLE_ATMEGA328
	#else
		enum { TCCR2WGM = (UINT32_C(1) << WGM21) | (UINT32_C(1) << COM20) };	// 0x18
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
	TCCR2 = TCCR2WGM;	// (UINT32_C(1) << WGM21) | (UINT32_C(1) << COM20)

#elif CPUSTYLE_ATMEGA32
	// генерация сигнала самоконтроля на PD7(OC2)

	// 8-bit таймер должен сгенерировать переключения выхода с частотой минимум 800 герц (для получения тона 400 герц).
	TCCR2 = TCCR2WGM;	// (UINT32_C(1) << WGM21) | (UINT32_C(1) << COM20)

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
	// TCCR2WGM = (UINT32_C(1) << WGM21) | (UINT32_C(1) << COM20)
	const uint_fast8_t tccrXBval = TCCR2WGM | (prei + 1);
	if ((TCCR2 != tccrXBval) || (OCR2 > value))		// таймер может отработать до максимального значения счётчика, если уменьшаем TOP
	{
		TCCR2 = TCCR2WGM;	// (UINT32_C(1) << WGM21) | (UINT32_C(1) << COM20) останавливаем таймер
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
	// TCCR2WGM = (UINT32_C(1) << WGM21) | (UINT32_C(1) << COM20)
	const uint_fast8_t tccrXBval = TCCR2WGM | (prei + 1);
	if ((TCCR2 != tccrXBval) || (OCR2 > value))		// таймер может отработать до максимального значения счётчика, если уменьшаем TOP
	{
		TCCR2 = TCCR2WGM;	// (UINT32_C(1) << WGM21) | (UINT32_C(1) << COM20) останавливаем таймер
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

	TIM4->PSC = ((UINT32_C(1) << prei) - 1) & TIM_PSC_PSC;

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
	PMC->PMC_PCER0 = (UINT32_C(1) << ID_TC1);	 // разрешить тактированние этого блока (ID_TC0..ID_TC5 avaliable)

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
	AT91C_BASE_PMC->PMC_PCER = (UINT32_C(1) << AT91C_ID_TC1); // разрешить тактированние этого блока (AT91C_ID_TC0..AT91C_ID_TC2 avaliable)

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
	TCCR2A = (UINT32_C(1) << COM2A0) | (UINT32_C(1) << WGM21);	// 0x42, (UINT32_C(1) << WGM21) only - OC2A disconnected
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
	TCCR2 = TCCR2WGM;	// (UINT32_C(1) << WGM21) | (UINT32_C(1) << COM20)
	// обязательно - настройка вывода процессора как выхода.
	SIDETONE_TARGET_DDR |= SIDETONE_TARGET_BIT; // (UINT32_C(1) << DDD7);	// output pin connection - test without this string need.
	SIDETONE_TARGET_PORT &= ~ SIDETONE_TARGET_BIT; // (UINT32_C(1) << PD7);	// disable pull-up

#elif CPUSTYLE_ATMEGA32
	// Timer/Counter 2 initialization
	// Clock source: System Clock
	// Clock value: Timer 2 Stopped
	// Mode: CTC top=OCR2
	// OC2 output: Toggle on compare match
	ASSR = 0x00;
	TCCR2 = TCCR2WGM;	// (UINT32_C(1) << WGM21) | (UINT32_C(1) << COM20)
	// обязательно - настройка вывода процессора как выхода.
	SIDETONE_TARGET_DDR |= SIDETONE_TARGET_BIT; // (UINT32_C(1) << DDD7);	// output pin connection - test without this string need.
	SIDETONE_TARGET_PORT &= ~ SIDETONE_TARGET_BIT; // (UINT32_C(1) << PD7);	// disable pull-up

#elif CPUSTYLE_ATXMEGAXXXA4

	TCD1.CTRLB = (TC1_CCAEN_bm | TC_WGMODE_FRQ_gc);
	SIDETONE_TARGET_DDR |= SIDETONE_TARGET_BIT; // (UINT32_C(1) << DDD7);	// output pin connection - test without this string need.

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
	//// TCCR1B = (UINT32_C(1) << WGM12) | (UINT32_C(1) << CS12) | (UINT32_C(1) << CS10);		// CTC mode (mode4) and 1/1024 prescaler
	//// OCR1A = 0xffff;

	#if CPUSTYLE_ATMEGA_XXX4
		// Timer/Counter 1 Interrupt(s) initialization
		TIMSK1 |= (UINT32_C(1) << OCIE1A);	// Timer/Counter 1 interrupt enable
	#elif CPUSTYLE_ATMEGA328
		// Timer/Counter 1 Interrupt(s) initialization
		TIMSK1 |= (UINT32_C(1) << OCIE1A);	// Timer/Counter 1 interrupt enable
	#else /* CPUSTYLE_ATMEGA_XXX4 */
		TIMSK |= (UINT32_C(1) << OCIE1A);	// Timer/Counter 1 interrupt enable
		//TIMSK |= 0x10;	// Timer/Counter 1 interrupt enable
	#endif /* CPUSTYLE_ATMEGA_XXX4 */

#elif CPUSTYLE_ATSAM3S || CPUSTYLE_ATSAM4S
	// TC2 used for electronic key synchronisation with 1/20 of dot length
	// TC2 used for generate 1/20 of morse dot length intervals
	//
	PMC->PMC_PCER0 = (UINT32_C(1) << ID_TC2);	// разрешить тактированние этого блока (ID_TC0..ID_TC5 avaliable)

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
	AT91C_BASE_PMC->PMC_PCER = (UINT32_C(1) << AT91C_ID_TC2); // разрешить тактированние этого блока (AT91C_ID_TC0..AT91C_ID_TC2 avaliable)

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

		AT91C_BASE_AIC->AIC_IDCR = (UINT32_C(1) << irqID);		// disable interrupt
		AT91C_BASE_AIC->AIC_SVR [irqID] = (AT91_REG) AT91F_TC2_IRQHandler;
		AT91C_BASE_AIC->AIC_SMR [irqID] =
			(AT91C_AIC_SRCTYPE & AT91C_AIC_SRCTYPE_HIGH_LEVEL) |
			(AT91C_AIC_PRIOR & AT91C_AIC_PRIOR_HIGHEST);
		AT91C_BASE_AIC->AIC_ICCR = (UINT32_C(1) << irqID);		// clear pending interrupt
		AT91C_BASE_AIC->AIC_IECR = (UINT32_C(1) << irqID);	// enable interrupt
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

#elif CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616

	// elkey timer
	const unsigned ix = 0;
	TIMER->TMR[ix].CTRL_REG = 0;

	TIMER->TMR_IRQ_EN_REG |= (UINT32_C(1) << (0 + ix));	// TMR0_IRQ_EN

	arm_hardware_set_handler_system(TIMER0_IRQn, TIMER0_IRQHandler);	// elkey timer

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
	// (UINT32_C(1) << WGM12) - mode4: CTC
	TCCR1B = (UINT32_C(1) << WGM12) | (prei + 1);	// прескалер
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

	TIM3->PSC = ((UINT32_C(1) << prei) - 1) & TIM_PSC_PSC;
	TIM3->ARR = value;
	TIM3->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости - если счётчик успевал превысить значение ARR - считал до конца */


#elif CPUSTYLE_STM32H7XX
	// TIM2 & TIM5 on CPUSTYLE_STM32F4XX have 32-bit CNT and ARR registers
	// TIM7 located on APB1
	// TIM7 on APB1
	// Use basic timer
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(BOARD_TIM3_FREQ, ticksfreq), STM32F_TIM3_TIMER_WIDTH, STM32F_TIM3_TIMER_TAPS, & value, 1);

	TIM3->PSC = ((UINT32_C(1) << prei) - 1) & TIM_PSC_PSC;
	TIM3->ARR = value;
	TIM3->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости - если счётчик успевал превысить значение ARR - считал до конца */

#elif CPUSTYLE_STM32F
	// TIM2 & TIM5 on CPUSTYLE_STM32F4XX have 32-bit CNT and ARR registers
	// TIM7 located on APB1
	// TIM7 on APB1
	// Use basic timer
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(BOARD_TIM3_FREQ, ticksfreq), STM32F_TIM3_TIMER_WIDTH, STM32F_TIM3_TIMER_TAPS, & value, 1);

	TIM3->PSC = ((UINT32_C(1) << prei) - 1) & TIM_PSC_PSC;
	TIM3->ARR = value;
	TIM3->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE; /* разрешить перезагрузку и включить таймер = перенесено в установку скорости - если счётчик успевал превысить значение ARR - считал до конца */

#elif CPUSTYLE_R7S721

	// OSTM1
	OSTM1.OSTMnCMP = calcdivround_p0clock(ticksfreq) - 1;

	OSTM1.OSTMnCTL = (OSTM1.OSTMnCTL & ~ 0x03) |
		0 * (UINT32_C(1) << 1) |	// Interval Timer Mode
		1 * (UINT32_C(1) << 0) |	// Enables the interrupts when counting starts.
		0;

#elif CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_A64 || CPUSTYLE_T507 || CPUSTYLE_H616

	const unsigned ix = 0;
	unsigned value;
	const uint_fast8_t prei = calcdivider(calcdivround2(allwnrt113_get_hosc_freq(), ticksfreq), ALLWNR_TIMER_WIDTH, ALLWNR_TIMER_TAPS, & value, 0);

	TIMER->TMR [ix].INTV_VALUE_REG = value;
	TIMER->TMR [ix].CTRL_REG =
		0 * (UINT32_C(1) << 7) |	// TMR0_MODE 0: Periodic mode.
		prei * (UINT32_C(1) << 4) |
		0x01 * (UINT32_C(1) << 2) |	// TMR1_CLK_SRC 01: OSC24M
		(UINT32_C(1) << 0) | // TMR0_EN
		0;

	while ((TIMER->TMR [ix].CTRL_REG & (UINT32_C(1) << 1)) != 0)
		;
	TIMER->TMR [ix].CTRL_REG |= UINT32_C(1) << 1;	// TMR0_RELOAD

#else
	#warning Undefined CPUSTYLE_XXX
#endif
}

#endif /* WITHELKEY */
