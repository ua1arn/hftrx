/*
 * Copyright (c) 2017-2022, STMicroelectronics - All Rights Reserved
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "hardware.h"

#if WITHSDRAMHW && CPUSTYLE_STM32MP1

#include "platform_def.h"
#include "stm32mp1_ddr_helpers.h"

void ddr_enable_clock(void)
{
	//stm32mp1_clk_rcc_regs_lock();

	mmio_setbits_32((uintptr_t) & RCC->DDRITFCR,
			RCC_DDRITFCR_DDRC1EN |
#if STM32MP_DDR_DUAL_AXI_PORT
			RCC_DDRITFCR_DDRC2EN |
#endif
			RCC_DDRITFCR_DDRPHYCEN |
			RCC_DDRITFCR_DDRPHYCAPBEN |
			RCC_DDRITFCR_DDRCAPBEN);

	//stm32mp1_clk_rcc_regs_unlock();
}
#endif
