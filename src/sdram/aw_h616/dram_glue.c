/*
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
 */

// https://github.com/apritzel/u-boot/blob/3aaabfe9ff4bbcd11096513b1b28d1fb0a40800f/arch/arm/mach-sunxi/dram_sun50i_h616.c#L3
// https://github.com/iuncuim/u-boot/blob/t507-lpddr4/arch/arm/mach-sunxi/dram_sun50i_h616.c

#include "hardware.h"

#if WITHSDRAMHW && (CONFIG_SUNXI_DRAM_H618_LPDDR4 || CONFIG_SUNXI_DRAM_H616_LPDDR4 || CONFIG_SUNXI_DRAM_T507_LPDDR4)

#include "formats.h"
#include "clocks.h"
#include <string.h>
#include "dram_glue.h"


static int xdramc_simple_wr_test(unsigned int mem_mb, int len)
{
	unsigned int  offs	= (mem_mb >> 1) << 18; // half of memory size
	const uint32_t  patt1 = 0x01234567;
	const uint32_t  patt2 = 0xfedcba98;
	uint32_t *addr, v1, v2, i;

	addr = (uint32_t *)CONFIG_SYS_SDRAM_BASE;
	for (i = 0; i != len; i++, addr++) {
		write32((virtual_addr_t)addr, patt1 + i);
		write32((virtual_addr_t)(addr + offs), patt2 + i);
	}

	addr = (uint32_t *)CONFIG_SYS_SDRAM_BASE;
	for (i = 0; i != len; i++) {
		v1 = read32((virtual_addr_t)(addr + i));
		v2 = patt1 + i;
		if (v1 != v2) {
			PRINTF("DRAM simple test FAIL.\n");
			PRINTF("%x != %x at address %p\n", (unsigned) v1, (unsigned) v2, (void *) (addr + i));
			return 1;
		}
		v1 = read32((virtual_addr_t)(addr + offs + i));
		v2 = patt2 + i;
		if (v1 != v2) {
			PRINTF("DRAM simple test FAIL.\n");
			PRINTF("%x != %x at address %p\n", (unsigned) v1, (unsigned) v2, (void *) (addr + offs + i));
			return 1;
		}
	}
	PRINTF("DRAM simple test OK.\n");
	return 0;
}


static unsigned long rand_val = 123456UL;

static void local_random_init(void)
{
	rand_val = 123456UL;
}

static unsigned long local_random(void)
{


	if (rand_val & 0x80000000UL)
		rand_val = (rand_val << 1);
	else
		rand_val = (rand_val << 1) ^ 0x201051UL;

	return (rand_val);
}

static uint32_t ddr_check_rand(unsigned long sizeee)
{
	typedef uint16_t test_t;
	const uint32_t sizeN = sizeee / sizeof (test_t);
	volatile test_t * const p = (volatile test_t *) CONFIG_SYS_SDRAM_BASE;
	uint32_t i;
	uint32_t uret;

	// fill
	//local_random_init();
	uint32_t seed = rand_val;

	for (i = 0; i < sizeN; ++ i)
	{
		//ddr_check_progress(i);
		p [i] = local_random();
	}
	// compare
	//local_random_init();
	rand_val = seed;
	//p = (volatile uint16_t *) STM32MP_DDR_BASE;
	for (i = 0; i < sizeN; ++ i)
	{
		//ddr_check_progress(i);
		if (p [i] != (test_t) local_random())
			return i * sizeof (test_t);
	}

	return sizeee;	// OK
}
/*
 * Test if memory at offset offset matches memory at begin of DRAM
 *
 * Note: dsb() is not available on ARMv5 in Thumb mode
 */
#ifndef CONFIG_MACH_SUNIV
static int mctl_mem_matches0(uint64_t offset, uint32_t value)
{
	/* Try to write different values to RAM at two addresses */
	writel(0, CONFIG_SYS_SDRAM_BASE);
	writel(value, (uintptr_t) CONFIG_SYS_SDRAM_BASE + offset);
	__DSB();
	/* Check if the same value is actually observed when reading back */
	return readl(CONFIG_SYS_SDRAM_BASE + 0) ==
	       readl((uintptr_t)CONFIG_SYS_SDRAM_BASE + offset);
}

// return 1: wrapped at offset
// Test if memory at offset offset matches memory at begin of DRAM
int mctl_mem_matches(uint64_t offset)
{
	return mctl_mem_matches0(offset, 0xaa55aa55) || mctl_mem_matches0(offset, 0xdeadbeef);
}
// return 1: wrapped at offset
// Test if memory at offset offset matches memory at begin of DRAM
int mctl_mem_matches_original(uint64_t offset)
{
	/* Try to write different values to RAM at two addresses */
	writel(0, CONFIG_SYS_SDRAM_BASE);
	writel(0xaa55aa55, (unsigned long)CONFIG_SYS_SDRAM_BASE + offset);
	__DSB();
	/* Check if the same value is actually observed when reading back */
	return readl(CONFIG_SYS_SDRAM_BASE) ==
	       readl((unsigned long)CONFIG_SYS_SDRAM_BASE + offset);
}
#endif

int arm_hardware_sdram_initialize(void)
{
	uint64_t memsizeB;
	unsigned memsizeMB;
	// https://artmemtech.com/
	// artmem atl4b0832
	PRINTF("arm_hardware_sdram_initialize start, cpux=%u MHz\n", (unsigned) (allwnr_t507_get_cpux_freq() / 1000 / 1000));
	PRINTF("arm_hardware_sdram_initialize, ddr=%u MHz\n", (unsigned) (allwnr_t507_get_dram_freq() / 1000 / 1000));

	memsizeB = sunxi_dram_init();
	memsizeMB = memsizeB / 1024 / 1024;
	//memsize =  dram_power_up_process(& lpddr4);
	//dbp();
	PRINTF("arm_hardware_sdram_initialize: result=%u MB\n", memsizeMB);
	(void) memsizeMB;

#if 0
	if (xdramc_simple_wr_test(memsizeMB, 64))
	{
		PRINTF("xdramc_simple_wr_test failed\n");
	}
	else
	{
		PRINTF("xdramc_simple_wr_test passed\n");
	}

	{
		int e;
		for (e = 0; e < 4; ++ e)
		{
			unsigned uret;
			unsigned size = memsizeMB * 1024 * 1024;
			int partfortest = 4;
			uret = ddr_check_rand(size / partfortest);
			if (uret != (size / partfortest)) {
				PRINTF("DDR random test: 0x%08x does not match DT config: 0x%08x\n",
				      uret, size / partfortest);
				for(;;)
					;
			}
			//TP();
		}
	}
#endif
	//	memset((void *) CONFIG_SYS_SDRAM_BASE, 0, 128u << 20);
	//	memset((void *) CONFIG_SYS_SDRAM_BASE + 0x00, 0xE5, 0x80);
	//	memset((void *) CONFIG_SYS_SDRAM_BASE + 0x80, 0xDF, 0x80);
	//printhex32(CONFIG_SYS_SDRAM_BASE, (void *) CONFIG_SYS_SDRAM_BASE, 2 * 0x80);

	PRINTF("arm_hardware_sdram_initialize done, ddr=%u MHz\n", (unsigned) (allwnr_t507_get_dram_freq() / 1000 / 1000));
	return ! memsizeMB;
}

#endif /* WITHSDRAMHW && CPUSTYLE_T507 */
