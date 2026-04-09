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
// Orange pi 4 pro
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
// https://github.com/chainsx/u-boot-sun60i/blob/8ffffe4f175ffa7591965075c351070967b1e86a/device-a733/configs/cubie_a7z/sys_config.fex#L461

// Radaxa Cubie A7Z
// DRAM SIZE =1024 MBytes, para1 = 30fa, para2 = 4000001, dram_tpr13 = 6063

uint32_t dram_para [128] = {
	CONFIG_DRAM_CLK, // CONFIG_DRAM_CLK, //800, // dram_clk
	8, 			// dram_type
	0x08080808, // dram_dx_odt
	0x0e0e0e0e, // dram_dx_dri
	0x88030e0e, // dram_ca_dri
	0, 			// dram_para0
	0x30fa, //0x311a, 	// dram_para1
	0x4000001, //0x1001, 	// dram_para2
	0x0, 		// dram_mr0
	0x8c, 		// dram_mr1
	0, 			// dram_mr2
	0x33, 		// dram_mr3
	0x0, 		// dram_mr4
	0, 			// dram_mr5
	0, 			// dram_mr6
	0x4, 		// dram_mr11
	0x72, 		// dram_mr12
	0x8, 		// dram_mr13
	0x1d, 		// dram_mr14
	0, 			// dram_mr16
	0, 			// dram_mr17
	0x24, 		// dram_mr22
	0, 			// dram_tpr0
	0, 			// dram_tpr1
	0x11080503, // dram_tpr2
	0x200000, 	// dram_tpr3
	0x402a, 	// dram_tpr6
	0x721f0000, // dram_tpr10
	0, 			// dram_tpr11
	0, 			// dram_tpr12
	0x6063, //0x60, 		// dram_tpr13
	0, 			// dram_tpr14
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
int arm_hardware_sdram_initialize(void)
{
	uint64_t memsizeB ;

	//sunxi_dram_init(NULL);

	PRINTF("arm_hardware_sdram_initialize start\n");
	int memsizeMB;
	memsizeMB = init_DRAM(0, dram_para);
	PRINTF("arm_hardware_sdram_initialize: result=%d MB\n", memsizeMB);
	PRINTF("arm_hardware_sdram_initialize done\n");
	return ! memsizeMB;
}

#endif
