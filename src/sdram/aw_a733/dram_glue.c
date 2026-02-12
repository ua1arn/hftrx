/*
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
 */

// https://github.com/apritzel/u-boot/blob/3aaabfe9ff4bbcd11096513b1b28d1fb0a40800f/arch/arm/mach-sunxi/dram_sun50i_h616.c#L3
// https://github.com/iuncuim/u-boot/blob/t507-lpddr4/arch/arm/mach-sunxi/dram_sun50i_h616.c

#include "hardware.h"

#if WITHSDRAMHW && (CONFIG_SUNXI_DRAM_A733_LPDDR5 || CONFIG_SUNXI_DRAM_A733_LPDDR4)

#include "formats.h"
#include "clocks.h"
#include <string.h>

#if CONFIG_SUNXI_DRAM_A733_LPDDR5
uint32_t dram_para [128] = {
		CONFIG_DRAM_CLK, //2400,	   // dram_clk
		9,		   // dram_type
		0x0e0e0e0e,// dram_dx_odt
		0x0f0f0f0f,// dram_dx_dri
		0xec030e0f,// dram_ca_dri
		0,		   // dram_para0
		0xa10a,	   // dram_para1
		0x1001,	   // dram_para2
		0,		   // dram_mr0
		0,		   // dram_mr1
		0,		   // dram_mr2
		0x6,	   // dram_mr3
		0,		   // dram_mr4
		0,		   // dram_mr5
		0,		   // dram_mr6
		0x12,	   // dram_mr11
		0x44,	   // dram_mr12
		0,		   // dram_mr13
		0x34,	   // dram_mr14
		0,		   // dram_mr16
		0x06,	   // dram_mr17
		0,		   // dram_mr22
		0x4040,	   // dram_tpr0
		0,		   // dram_tpr1
		0x170b070, // dram_tpr2
		0x3800,	   // dram_tpr3
		0x3514,	   // dram_tpr6
		0x325f0000,// dram_tpr10
		0,		   // dram_tpr11
		0,		   // dram_tpr12
		0x10061,   // dram_tpr13
		0		   // dram_tpr14
};
#endif /* CONFIG_SUNXI_DRAM_A733_LPDDR5 */


#if CONFIG_SUNXI_DRAM_A733_LPDDR4
uint32_t dram_para [128] = {
		CONFIG_DRAM_CLK, //800,	   // dram_clk
		8,		   // dram_type
		0x0e0e0e0e,// dram_dx_odt
		0x0f0f0f0f,// dram_dx_dri
		0xec030e0f,// dram_ca_dri
		0,		   // dram_para0
		0xa10a,	   // dram_para1
		0x1001,	   // dram_para2
		0,		   // dram_mr0
		0,		   // dram_mr1
		0,		   // dram_mr2
		0x6,	   // dram_mr3
		0,		   // dram_mr4
		0,		   // dram_mr5
		0,		   // dram_mr6
		0x12,	   // dram_mr11
		0x44,	   // dram_mr12
		0,		   // dram_mr13
		0x34,	   // dram_mr14
		0,		   // dram_mr16
		0x06,	   // dram_mr17
		0,		   // dram_mr22
		0x4040,	   // dram_tpr0
		0,		   // dram_tpr1
		0x170b070, // dram_tpr2
		0x3800,	   // dram_tpr3
		0x3514,	   // dram_tpr6
		0x325f0000,// dram_tpr10
		0,		   // dram_tpr11
		0,		   // dram_tpr12
		0x10061,   // dram_tpr13
		0		   // dram_tpr14
};
#endif /* CONFIG_SUNXI_DRAM_A733_LPDDR4 */

void sunxi_smc_en_with_glitch_workaround(void) {
	return;
}

//static uint32_t dram_size;
//
//uint32_t sunxi_get_dram_size() {
//	return dram_size;
//}

extern int init_DRAM(int type, void *buff);

void udelay(uint32_t us)
{
	local_delay_us(us);
}

int printf_dram(const char *format, ...)
{
	char b [256];	// see stack sizes for interrupt handlers
	va_list	ap;

//	IRQL_t irql;
//	IRQLSPIN_LOCK(& printflock, & irql, IRQL_IPC_ONLY);

	va_start(ap, format);
	int n = vsnprintf(b, sizeof b / sizeof b [0], format, ap);
	va_end(ap);
	b [ARRAY_SIZE(b) - 1] = '\0';

	dbg_puts_impl(b);
//	IRQLSPIN_UNLOCK(& printflock, irql);
	return n;
}
void arm_hardware_sdram_initialize(void)
{
	uint64_t memsizeB ;

	//sunxi_dram_init(NULL);

	PRINTF("arm_hardware_sdram_initialize start\n");
	int memsizeMB;
	memsizeMB = init_DRAM(0, dram_para);
	PRINTF("arm_hardware_sdram_initialize: result=%d MB\n", memsizeMB);
	PRINTF("arm_hardware_sdram_initialize done\n");
}

#endif
