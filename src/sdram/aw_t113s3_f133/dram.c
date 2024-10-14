/*
 * dram.c
 *
 *  Created on: Jul 30, 2023
 *      Author: Gena
 */




#include "hardware.h"

#if WITHSDRAMHW && (CPUSTYLE_T113 || CPUSTYLE_F133)

#include "formats.h"
#include "t113_dram.h"


#include "spi.h"

/**
 *
	Hi,

	this is the extracted version of the Allwinner D1/D1s/R528/T113-s DRAM
	"driver", to be included into mainline U-Boot at some point. With this
	on top of my previous T113-s3 support series[1], I can boot my MangoPi MQ-R
	without the help of awboot.
	The DRAM init code is based on awboot's version, though has been heavily
	reworked. To show what has been done, I pushed a history branch [2], which
	takes a verbatim copy of awboot's mctl_hal.c, then converts this over the
	course of about 80 patches into the version posted here. The series there
	contains an awboot/U-Boot compat layer, so the file can be used in both
	repositories. This compat layer is dropped here, but can be put back by
	reverting the top patch of [2].

	I was wondering if people could have a look at this version here, to give
	early feedback. I will (re-)post this as part of a proper R528/T113-s
	support series, but first need to sort out some minor issues and address
	Samuel's comments on the previous version.

	If you wonder, the (working!) Kconfig DRAM variables for the T113-s3 are:
	CONFIG_DRAM_CLK=792
	CONFIG_DRAM_ZQ=8092667
	CONFIG_DRAM_SUNXI_ODT_EN=0
	CONFIG_DRAM_SUNXI_TPR0=0x004a2195
	CONFIG_DRAM_SUNXI_TPR11=0x340000
	CONFIG_DRAM_SUNXI_TPR12=0x46
	CONFIG_DRAM_SUNXI_TPR13=0x34000100

	For the D1 with DDR3 chips (most boards?), it should be those values:
	CONFIG_DRAM_CLK=792
	CONFIG_DRAM_SUNXI_ODT_EN=1
	CONFIG_DRAM_SUNXI_TPR0=0x004a2195
	CONFIG_DRAM_SUNXI_TPR11=0x870000
	CONFIG_DRAM_SUNXI_TPR12=0x24
	CONFIG_DRAM_SUNXI_TPR13=0x34050100

	According to the dump of some MangoPi MQ-1 firmware, the D1s should work with:
	CONFIG_SUNXI_DRAM_DDR2=y
	CONFIG_DRAM_CLK=528
	CONFIG_DRAM_ZQ=8092665
	CONFIG_DRAM_SUNXI_ODT_EN=0
	CONFIG_DRAM_SUNXI_TPR0=0x00471992
	CONFIG_DRAM_SUNXI_TPR11=0x30010
	CONFIG_DRAM_SUNXI_TPR12=0x35
	CONFIG_DRAM_SUNXI_TPR13=0x34000000

	Many thanks!
	Andre

	[1] https://lore.kernel.org/u-boot/20221206004549.29015-1-andre.przywara@arm.com/
	[2] https://github.com/apritzel/u-boot/commits/d1_dram_history

 *
 */
#if CPUSTYLE_T113

static dram_para_t t113m4_ddrp3 =
{
	.dram_clk = 936,	// s3: 792
	.dram_type = 3,
	.dram_zq = 0x7b7bfb,	// same as 0x007b7bFb or 8092667
	.dram_odt_en = 0x00,
	.dram_para1 = 0x000010d2,
	.dram_para2 = 0x0000,
	.dram_mr0 = 0x1c70,
	.dram_mr1 = 0x042,
	.dram_mr2 = 0x18,
	.dram_mr3 = 0x0,
	.dram_tpr0 = 0x004a2195,
	.dram_tpr1 = 0x02423190,
	.dram_tpr2 = 0x0008B061,
	.dram_tpr3 = 0xB4787896,
	.dram_tpr4 = 0x0,
	.dram_tpr5 = 0x48484848,
	.dram_tpr6 = 0x00000048,
	.dram_tpr7 = 0x1620121e,
	.dram_tpr8 = 0x0,
	.dram_tpr9 = 0x0,
	.dram_tpr10 = 0x0,
	.dram_tpr11 = 0x00340000,
	.dram_tpr12 = 0x00000046,
	.dram_tpr13 = 0x34000100,
};

static dram_para_t t113s3_ddrp3 =
{
	.dram_clk = 792,
	.dram_type = 3,
	.dram_zq = 0x007b7bFb,	// same as 0x007b7bFb or 8092667
	.dram_odt_en = 0x00,
	.dram_para1 = 0x000010d2,
	.dram_para2 = 0x0000,
	.dram_mr0 = 0x1c70,
	.dram_mr1 = 0x042,
	.dram_mr2 = 0x18,
	.dram_mr3 = 0x0,
	.dram_tpr0 = 0x004a2195,
	.dram_tpr1 = 0x02423190,
	.dram_tpr2 = 0x0008B061,
	.dram_tpr3 = 0xB4787896,
	.dram_tpr4 = 0x0,
	.dram_tpr5 = 0x48484848,
	.dram_tpr6 = 0x00000048,
	.dram_tpr7 = 0x1620121e,
	.dram_tpr8 = 0x0,
	.dram_tpr9 = 0x0,
	.dram_tpr10 = 0x0,
	.dram_tpr11 = 0x00340000,
	.dram_tpr12 = 0x00000046,
	.dram_tpr13 = 0x34000100,
};

int sys_dram_init(void)
{
	if (allwnr_t113_get_chipid() == CHIPID_T113M4020DC0)
	{
		return init_DRAM(0, & t113m4_ddrp3) != 0;
	}
	else
	{
		return init_DRAM(0, & t113s3_ddrp3) != 0;
	}
}

#elif CPUSTYLE_F133

/*
 *
CONFIG_SUNXI_DRAM_DDR2=y
CONFIG_DRAM_CLK=528
CONFIG_DRAM_ZQ=8092665
CONFIG_DRAM_SUNXI_ODT_EN=0
CONFIG_DRAM_SUNXI_TPR0=0x00471992
CONFIG_DRAM_SUNXI_TPR11=0x30010
CONFIG_DRAM_SUNXI_TPR12=0x35
CONFIG_DRAM_SUNXI_TPR13=0x34000000

 */
static dram_para_t ddrp2 = {
	.dram_clk = 528,
	.dram_type = 2,
	.dram_zq = 0x007B7BF9,	// same as 0x007B7BF9 or 8092665
	.dram_odt_en = 0x00,
	.dram_para1 = 0x000000d2,
	.dram_para2 = 0x00000000,
	.dram_mr0 = 0x00000e73,
	.dram_mr1 = 0x02,
	.dram_mr2 = 0x0,
	.dram_mr3 = 0x0,
	.dram_tpr0 = 0x00471992,
	.dram_tpr1 = 0x0131a10c,
	.dram_tpr2 = 0x00057041,
	.dram_tpr3 = 0xb4787896,
	.dram_tpr4 = 0x0,
	.dram_tpr5 = 0x48484848,
	.dram_tpr6 = 0x48,
	.dram_tpr7 = 0x1621121e,
	.dram_tpr8 = 0x0,
	.dram_tpr9 = 0x0,
	.dram_tpr10 = 0x00000000,
	.dram_tpr11 = 0x00030010,
	.dram_tpr12 = 0x00000035,
	.dram_tpr13 = 0x34000000,
};

int sys_dram_init(void)
{
	return init_DRAM(0, & ddrp2) != 0;
}

#endif /* CPUSTYLE_T113, CPUSTYLE_F133 */

void arm_hardware_sdram_initialize(void)
{
	PRINTF("arm_hardware_sdram_initialize start\n");
	PRINTF("default: allwnr_t113_get_pll_ddr_freq()=%ukHz\n", (unsigned) (allwnr_t113_get_pll_ddr_freq() / 1000));
	PRINTF("default: allwnr_t113_get_dram_freq()=%ukHz\n", (unsigned) (allwnr_t113_get_dram_freq() / 1000));
	if (sys_dram_init() == 0)
	{
		PRINTF("No external memory");
#ifdef BOARD_BLINK_INITIALIZE
		BOARD_BLINK_INITIALIZE();
		for (;;)
		{
			BOARD_BLINK_SETSTATE(1);
			local_delay_ms(100);
			BOARD_BLINK_SETSTATE(0);
			local_delay_ms(100);
		}
#endif
		for (;;)
			;
	}
	PRINTF("settings: allwnr_t113_get_pll_ddr_freq()=%ukHz\n", (unsigned) (allwnr_t113_get_pll_ddr_freq() / 1000));
	PRINTF("settings: allwnr_t113_get_dram_freq()=%ukHz\n", (unsigned) (allwnr_t113_get_dram_freq() / 1000));

	PRINTF("arm_hardware_sdram_initialize done\n");
	//local_delay_ms(1000);
}


#endif /* WITHSDRAMHW && (CPUSTYLE_T113 || CPUSTYLE_F133) */

