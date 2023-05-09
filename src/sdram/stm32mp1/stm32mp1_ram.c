/*
 * Copyright (C) 2018-2022, STMicroelectronics - All Rights Reserved
 *
 * SPDX-License-Identifier: GPL-2.0+ OR BSD-3-Clause
 */


#include "hardware.h"

#if WITHSDRAMHW && CPUSTYLE_STM32MP1

#include "platform_def.h"
#include "stm32mp1_ddr.h"
#include "stm32mp1_ddr_helpers.h"
#include "stm32mp1_ram.h"
#include "stm32mp_ddr.h"
#include "stm32mp_ddr_test.h"
#include "stm32mp_ram.h"

static struct stm32mp_ddr_priv ddr_priv_data;

int stm32mp1_ddr_clk_enable(struct stm32mp_ddr_priv *priv, uint32_t mem_speed)
{
	unsigned long ddrphy_clk, ddr_clk, mem_speed_hz;

	ddr_enable_clock();

//	ddrphy_clk = clk_get_rate(DDRPHYC);
//
//	VERBOSE("DDR: mem_speed (%u kHz), RCC %lu kHz\n",
//		mem_speed, ddrphy_clk / 1000U);

	mem_speed_hz = mem_speed * 1000U;

	/* Max 10% frequency delta */
//	if (ddrphy_clk > mem_speed_hz) {
//		ddr_clk = ddrphy_clk - mem_speed_hz;
//	} else {
//		ddr_clk = mem_speed_hz - ddrphy_clk;
//	}
//	if (ddr_clk > (mem_speed_hz / 10)) {
//		ERROR("DDR expected freq %u kHz, current is %lu kHz\n",
//		      mem_speed, ddrphy_clk / 1000U);
//		return -1;
//	}
	return 0;
}

static int stm32mp1_ddr_setup(void)
{
	struct stm32mp_ddr_priv *priv = &ddr_priv_data;
	int ret;
	struct stm32mp_ddr_config config;
	int node;
	uint32_t uret;
	void *fdt;

	const struct stm32mp_ddr_param param[] = {
		CTL_PARAM(reg),
		CTL_PARAM(timing),
		CTL_PARAM(map),
		CTL_PARAM(perf),
		PHY_PARAM(reg),
		PHY_PARAM(timing),
	};

#if 1


	priv->ctl = (struct stm32mp1_ddrctl *) DDRCTRL_BASE;
	priv->phy = (struct stm32mp1_ddrphy *) DDRPHYC_BASE;
	priv->pwr = PWR_BASE;
	priv->rcc = RCC_BASE;
	priv->info.base = STM32MP_DDR_BASE;
	priv->info.size = DDR_MEM_SIZE;

	stm32mp1_ddr_get_config(& config);

#else
	if (fdt_get_address(&fdt) == 0) {
		return -ENOENT;
	}

	node = fdt_node_offset_by_compatible(fdt, -1, DT_DDR_COMPAT);
	if (node < 0) {
		ERROR("%s: Cannot read DDR node in DT\n", __func__);
		return -EINVAL;
	}

	ret = stm32mp_ddr_dt_get_info(fdt, node, &config.info);
	if (ret < 0) {
		return ret;
	}

	ret = stm32mp_ddr_dt_get_param(fdt, node, param, ARRAY_SIZE(param), (uintptr_t)&config);
	if (ret < 0) {
		return ret;
	}
#endif

	/* Disable axidcg clock gating during init */
	mmio_clrbits_32((uintptr_t) & RCC->DDRITFCR, RCC_DDRITFCR_AXIDCGEN);

	stm32mp1_ddr_init(priv, &config);

	/* Enable axidcg clock gating */
	mmio_setbits_32((uintptr_t) & RCC->DDRITFCR, RCC_DDRITFCR_AXIDCGEN);

	priv->info.size = config.info.size;

	VERBOSE("%s : ram size(%x, %x)\n", __func__,
		(uint32_t)priv->info.base, (uint32_t)priv->info.size);

//	if (stm32mp_map_ddr_non_cacheable() != 0) {
//		panic();
//	}

	uret = stm32mp_ddr_test_data_bus();
	if (uret != 0U) {
		ERROR("DDR data bus test: can't access memory @ 0x%x\n",
		      uret);
		panic();
	}

	uret = stm32mp_ddr_test_addr_bus(config.info.size);
	if (uret != 0U) {
		ERROR("DDR addr bus test: can't access memory @ 0x%x\n",
		      uret);
		panic();
	}
//
//	uret = stm32mp_ddr_check_size();
//	if (uret < config.info.size) {
//		ERROR("DDR size: 0x%x does not match DT config: 0x%x\n",
//		      uret, config.info.size);
//		panic();
//	}

//	if (stm32mp_unmap_ddr() != 0) {
//		panic();
//	}

	VERBOSE("%s : ram size(%x, %x) tests passed\n", __func__,
		(uint32_t)priv->info.base, (uint32_t)priv->info.size);
	return 0;
}

int stm32mp1_ddr_probe(void)
{
	struct stm32mp_ddr_priv *priv = &ddr_priv_data;

	VERBOSE("STM32MP DDR probe\n");

//	priv->ctl = (struct stm32mp_ddrctl *)stm32mp_ddrctrl_base();
//	priv->phy = (struct stm32mp_ddrphy *)stm32mp_ddrphyc_base();
//	priv->pwr = stm32mp_pwr_base();
//	priv->rcc = stm32mp_rcc_base();
	priv->ctl = (struct stm32mp_ddrctl *) DDRCTRL_BASE;
	priv->phy = (struct stm32mp_ddrphy *) DDRPHYC_BASE;
	priv->pwr = PWR_BASE;
	priv->rcc = RCC_BASE;

	priv->info.base = STM32MP_DDR_BASE;
	priv->info.size = 0;

	return stm32mp1_ddr_setup();
}
#endif
