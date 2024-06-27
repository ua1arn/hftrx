#include "hardware.h"

#if WITHSDRAMHW && (CPUSTYLE_V3S)

#include "formats.h"
#include "gpio.h"
//#include "t113_dram.h"

int sys_dram_init(void)
{
	return 0;
}

void arm_hardware_sdram_initialize(void)
{
	PRINTF("arm_hardware_sdram_initialize start\n");
//	PRINTF("default: allwnrt113_get_pll_ddr_freq()=%" PRIuFAST64 "\n", allwnrt113_get_pll_ddr_freq());
//	PRINTF("default: allwnrt113_get_dram_freq()=%" PRIuFAST32 "\n", allwnrt113_get_dram_freq());
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
//	PRINTF("settings: allwnrt113_get_pll_ddr_freq()=%" PRIuFAST64 "\n", allwnrt113_get_pll_ddr_freq());
//	PRINTF("settings: allwnrt113_get_dram_freq()=%" PRIuFAST32 "\n", allwnrt113_get_dram_freq());

	PRINTF("arm_hardware_sdram_initialize done\n");
	//local_delay_ms(1000);
}


#endif /* WITHSDRAMHW && (CPUSTYLE_V3S) */

