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

static void stm32mp1_ddr_get_config(struct stm32mp_ddr_config * cfg)
{
	cfg->info.speed = DDR_MEM_SPEED; // kHz
	cfg->info.size = DDR_MEM_SIZE;
	cfg->info.name = DDR_MEM_NAME;

	cfg->c_reg.mstr = 	 	DDR_MSTR;
	cfg->c_reg.mrctrl0 = 	DDR_MRCTRL0;
	cfg->c_reg.mrctrl1 = 	DDR_MRCTRL1;
	cfg->c_reg.derateen =  	DDR_DERATEEN;
	cfg->c_reg.derateint =	DDR_DERATEINT;
	cfg->c_reg.pwrctl = 	DDR_PWRCTL;
	cfg->c_reg.pwrtmg = 	DDR_PWRTMG;
	cfg->c_reg.hwlpctl = 	DDR_HWLPCTL;
	cfg->c_reg.rfshctl0 =  	DDR_RFSHCTL0;
	cfg->c_reg.rfshctl3 =  	DDR_RFSHCTL3;
	cfg->c_reg.crcparctl0  = DDR_CRCPARCTL0;
	cfg->c_reg.zqctl0 = 	 DDR_ZQCTL0;
	cfg->c_reg.dfitmg0 = 	 DDR_DFITMG0;
	cfg->c_reg.dfitmg1 = 	 DDR_DFITMG1;
	cfg->c_reg.dfilpcfg0 = 	DDR_DFILPCFG0;
	cfg->c_reg.dfiupd0 = 	 DDR_DFIUPD0;
	cfg->c_reg.dfiupd1 = 	 DDR_DFIUPD1;
	cfg->c_reg.dfiupd2 = 	 DDR_DFIUPD2;
	cfg->c_reg.dfiphymstr = DDR_DFIPHYMSTR;
	cfg->c_reg.odtmap = 	DDR_ODTMAP;
	cfg->c_reg.dbg0 = 	 	DDR_DBG0;
	cfg->c_reg.dbg1 = 	 	DDR_DBG1;
	cfg->c_reg.dbgcmd = 	DDR_DBGCMD;
	cfg->c_reg.poisoncfg = DDR_POISONCFG;
	cfg->c_reg.pccfg = 	 DDR_PCCFG;

	cfg->c_timing.rfshtmg = 	 DDR_RFSHTMG;
	cfg->c_timing.dramtmg0 =  DDR_DRAMTMG0;
	cfg->c_timing.dramtmg1 =  DDR_DRAMTMG1;
	cfg->c_timing.dramtmg2 =  DDR_DRAMTMG2;
	cfg->c_timing.dramtmg3 =  DDR_DRAMTMG3;
	cfg->c_timing.dramtmg4 =  DDR_DRAMTMG4;
	cfg->c_timing.dramtmg5 =  DDR_DRAMTMG5;
	cfg->c_timing.dramtmg6 =  DDR_DRAMTMG6;
	cfg->c_timing.dramtmg7 =  DDR_DRAMTMG7;
	cfg->c_timing.dramtmg8 =  DDR_DRAMTMG8;
	cfg->c_timing.dramtmg14 = DDR_DRAMTMG14;
	cfg->c_timing.odtcfg = 	 DDR_ODTCFG;

	cfg->c_perf.sched = 	 DDR_SCHED;
	cfg->c_perf.sched1 = 	 DDR_SCHED1;
	cfg->c_perf.perfhpr1 =  DDR_PERFHPR1;
	cfg->c_perf.perflpr1 =  DDR_PERFLPR1;
	cfg->c_perf.perfwr1 = 	 DDR_PERFWR1;
	cfg->c_perf.pcfgr_0 = 	 DDR_PCFGR_0;
	cfg->c_perf.pcfgw_0 = 	 DDR_PCFGW_0;
	cfg->c_perf.pcfgqos0_0  = DDR_PCFGQOS0_0;
	cfg->c_perf.pcfgqos1_0  = DDR_PCFGQOS1_0;
	cfg->c_perf.pcfgwqos0_0 = DDR_PCFGWQOS0_0;
	cfg->c_perf.pcfgwqos1_0 = DDR_PCFGWQOS1_0;
	cfg->c_perf.pcfgr_1 = 	 DDR_PCFGR_1;
	cfg->c_perf.pcfgw_1 = 	 DDR_PCFGW_1;
	cfg->c_perf.pcfgqos0_1  = DDR_PCFGQOS0_1;
	cfg->c_perf.pcfgqos1_1  = DDR_PCFGQOS1_1;
	cfg->c_perf.pcfgwqos0_1 = DDR_PCFGWQOS0_1;
	cfg->c_perf.pcfgwqos1_1 = DDR_PCFGWQOS1_1;

	cfg->c_map.addrmap1 =  DDR_ADDRMAP1;
	cfg->c_map.addrmap2 =  DDR_ADDRMAP2;
	cfg->c_map.addrmap3 =  DDR_ADDRMAP3;
	cfg->c_map.addrmap4 =  DDR_ADDRMAP4;
	cfg->c_map.addrmap5 =  DDR_ADDRMAP5;
	cfg->c_map.addrmap6 =  DDR_ADDRMAP6;
	cfg->c_map.addrmap9 =  DDR_ADDRMAP9;
	cfg->c_map.addrmap10 = DDR_ADDRMAP10;
	cfg->c_map.addrmap11 = DDR_ADDRMAP11;

	cfg->p_reg.pgcr = 	 DDR_PGCR;
	cfg->p_reg.aciocr = 	 (DDR_ACIOCR & ~ (DDRPHYC_ACIOCR_ACSR_Msk)) | (0x02uL << DDRPHYC_ACIOCR_ACSR_Pos);
	cfg->p_reg.dxccr = 	 DDR_DXCCR;
	cfg->p_reg.dsgcr = 	 DDR_DSGCR;
	cfg->p_reg.dcr = 		 DDR_DCR;
	cfg->p_reg.odtcr = 	 DDR_ODTCR;

	cfg->p_timing.ptr0 = 	 DDR_PTR0;
	cfg->p_timing.ptr1 = 	 DDR_PTR1;
	cfg->p_timing.ptr2 = 	 DDR_PTR2;
	cfg->p_timing.dtpr0 = 	 DDR_DTPR0;
	cfg->p_timing.dtpr1 = 	 DDR_DTPR1;
	cfg->p_timing.dtpr2 = 	 DDR_DTPR2;
	cfg->p_timing.mr0 = 		 DDR_MR0;
	cfg->p_timing.mr1 = 		 DDR_MR1;
	cfg->p_timing.mr2 = 		 DDR_MR2;
	cfg->p_timing.mr3 = 		 DDR_MR3;

	cfg->p_reg.zq0cr1 = 	 DDR_ZQ0CR1;
	cfg->p_reg.dx0gcr = 	 DDR_DX0GCR;
	//cfg->p_cal.dx0dllcr =  DDR_DX0DLLCR;
	//cfg->p_cal.dx0dqtr = 	 DDR_DX0DQTR;
	//cfg->p_cal.dx0dqstr =  DDR_DX0DQSTR;
	cfg->p_reg.dx1gcr = 	 DDR_DX1GCR;
	//cfg->p_cal.dx1dllcr =  DDR_DX1DLLCR;
	//cfg->p_cal.dx1dqtr = 	 DDR_DX1DQTR;
	//cfg->p_cal.dx1dqstr =  	DDR_DX1DQSTR;
	cfg->p_reg.dx2gcr = 	DDR_DX2GCR;
	//cfg->p_cal.dx2dllcr =  	DDR_DX2DLLCR;
	//cfg->p_cal.dx2dqtr = 	DDR_DX2DQTR;
	//cfg->p_cal.dx2dqstr =  	DDR_DX2DQSTR;
	cfg->p_reg.dx3gcr = 	DDR_DX3GCR;
	//cfg->p_cal.dx3dllcr =  	DDR_DX3DLLCR;
	//cfg->p_cal.dx3dqtr = 	DDR_DX3DQTR;
	//cfg->p_cal.dx3dqstr =  	DDR_DX3DQSTR;
}

static int stm32mp1_ddr_setup(void)
{
	struct stm32mp_ddr_priv *priv = &ddr_priv_data;
	int ret;
	struct stm32mp_ddr_config config;
	int node;
	unsigned uret;
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


	priv->ctl = (struct stm32mp_ddrctl *) DDRCTRL_BASE;
	priv->phy = (struct stm32mp_ddrphy *) DDRPHYC_BASE;
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
		(unsigned)priv->info.base, (unsigned)priv->info.size);

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
		(unsigned)priv->info.base, (unsigned)priv->info.size);
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
