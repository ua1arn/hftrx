// SPDX-License-Identifier: GPL-2.0+
/*
 * drivers/dsp/sun8iw20/dsp.c
 *
 * Copyright (c) 2007-2025 Allwinnertech Co., Ltd.
 * Author: wujiayi <wujiayi@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details
 *
 */

/*
#include <asm/arch-sunxi/cpu_ncat_v2.h>
#include <asm/io.h>
#include <common.h>
#include <sunxi_image_verifier.h>
#include <sys_config.h>
*/

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "Type.h"

#include "LowLevel.h"

#include "Gate.h"

#include "timer.h"

#include "dsp_reg.h"
//#include "elf.h"
//#include "fdt_support.h"
//#include "platform.h"

/*
#include "../common/dsp_fdt.h"
#include "../common/dsp_ic.h"
#include "../common/dsp_img.h"
*/
//#include "platform.h"
//#include "imgdts.h"
//#include "dsp_img.h"
//#include "dsp_fdt.h"

//#define ulong unsigned int

//#define pr_err printf

//#define readl_dsp(addr) readl((const volatile void *)(addr))
//#define writel_dsp(val, addr) writel((u32)(val), (volatile void *)(addr))

#define readl_dsp(addr) (*((IO u32*)(addr)))
#define writel_dsp(val, addr) (*((IO u32*)(addr)))=((u32)(val))

//#define ROUND_DOWN(a, b) ((a) & ~((b)-1))
//#define ROUND_UP(a, b) (((a) + (b)-1) & ~((b)-1))

//#define ROUND_DOWN_CACHE(a) ROUND_DOWN(a, CONFIG_SYS_CACHELINE_SIZE)
//#define ROUND_UP_CACHE(a) ROUND_UP(a, CONFIG_SYS_CACHELINE_SIZE)

#define DSP0_IRAM /*0x00400000*/ 0x00028000

#define SUNXI_CCM_BASE   CCU_BASE
#define SUNXI_SRAMC_BASE 0x03000000

#define CCU_DSP_CLK_REG (*(IO u32*)(CCU_BASE+0x0C70)) //OK
#define CCU_DSP_BGR_REG (*(IO u32*)(CCU_BASE+0x0C7C)) //OK

#define SYS_CFG_DSP_BOOT_RAMMAP_REG    (*(IO u32*)(SRAMC_BASE+0x0008)) //OK

#define DSP0_CFG_DSP_ALT_RESET_VEC_REG (*(IO u32*)(DSP0_BASE+0x0000))  //OK
#define DSP0_CFG_DSP_CTRL_REG0         (*(IO u32*)(DSP0_BASE+0x0004))  //OK
#define DSP0_CFG_DSP_STAT_REG          (*(IO u32*)(DSP0_BASE+0x0010))  //OK


/*
 * dsp need to remap addresses for some addr.
 */
/*
static struct vaddr_range_t addr_mapping[] = {
    {0x10000000, 0x1fffffff, 0x30000000},
    {0x30000000, 0x3fffffff, 0x10000000},
};
*/

/* HiFI4 DSP-viewed address offset translate to host cpu viewwed */
static ptrdiff_t xlate_dsp2mpu(ptrdiff_t a)
{
	const ptrdiff_t BANKSIZE = 0x08000u;
	const ptrdiff_t CELLBASE = 0x10000u;
	const ptrdiff_t CELLSIZE = 16;
	const ptrdiff_t cellbank = (a - CELLBASE) / BANKSIZE;
	const ptrdiff_t cellrow = (a - CELLBASE) % BANKSIZE / CELLSIZE;	/* гранулярность 16 байт */
	const unsigned cellpos = (a % CELLSIZE);	/* гранулярность 16 байт */

	if (a < CELLBASE)
		return a;	/* translation not needed. */

	return CELLBASE +
			cellbank * BANKSIZE +
			CELLSIZE * ((cellrow % 2) ? (cellrow / 2) + (BANKSIZE / CELLSIZE / 2) : cellrow / 2) +
			cellpos;
}

/* memcpy replacement for Allwinner T113-s3 dsp memory */
static void copy2dsp(uint8_t * pdspmap, const uint8_t * pcpu, unsigned offs, unsigned size)
{
	for (; size --; ++ offs)
	{
		pdspmap [xlate_dsp2mpu(offs)] = pcpu [offs];
	}
}

/* memset replacement for Allwinner T113-s3 dsp memory */
static void zero2dsp(uint8_t * pdspmap, unsigned offs, unsigned size)
{
	for (; size --; ++ offs)
	{
		pdspmap [xlate_dsp2mpu(offs)] = 0x00;	/* fill by zero */
	}
}

static void sunxi_dsp_set_runstall(u32 dsp_id, u32 value)
{
  u32 reg_val;

  if (dsp_id == 0) /* DSP0 */
  {
    reg_val = readl_dsp(DSP0_CFG_BASE + DSP_CTRL_REG0);
    reg_val &= ~(1 << BIT_RUN_STALL);
    reg_val |= (value << BIT_RUN_STALL);
    writel_dsp(reg_val, (DSP0_CFG_BASE + DSP_CTRL_REG0));
  }
}

#if 0
static int update_reset_vec(u32 img_addr, u32 *run_addr) {

#if 0
  Elf32_Ehdr *ehdr; /* Elf header structure pointer */

  ehdr = (Elf32_Ehdr *)(ADDR_TPYE)img_addr;
  if (!*run_addr)
    *run_addr = ehdr->e_entry;
#endif

  *run_addr=img_addr; //run address = image address

  return 0;
}
#endif

//static int load_image(u32 img_addr, u32 dsp_id) {
void load_image(const void *img_addr, u32 img_size) { //добавил размер

//  Elf32_Ehdr *ehdr; /* Elf header structure pointer */
//  Elf32_Phdr *phdr; /* Program header structure pointer */
/*
  void *dst = NULL;
  void *src = NULL;
  int i = 0;
  int size = sizeof(addr_mapping) / sizeof(struct vaddr_range_t);
  ulong mem_start = 0;
  u32 mem_size = 0;
*/
//  ehdr = (Elf32_Ehdr *)(ADDR_TPYE)img_addr;
//  phdr = (Elf32_Phdr *)(ADDR_TPYE)(img_addr + ehdr->e_phoff);

#if 0
  /* Load each program header */
  for (i = 0; i < ehdr->e_phnum; ++i) {

    // remap addresses
    dst = (void *)(ADDR_TPYE)set_img_va_to_pa((unsigned long)phdr->p_paddr,
                                              addr_mapping, size);

    src = (void *)(ADDR_TPYE)img_addr + phdr->p_offset;
    DSP_DEBUG("Loading phdr %i from 0x%x to 0x%p (%i bytes)\n", i,
              phdr->p_paddr, dst, phdr->p_filesz);

    if (phdr->p_filesz)
      memcpy(dst, src, phdr->p_filesz);
    if (phdr->p_filesz != phdr->p_memsz)
      memset(dst + phdr->p_filesz, 0x00, phdr->p_memsz - phdr->p_filesz);
    if (i == 0)
      show_img_version((char *)dst + 896, dsp_id);

    // flush_cache(ROUND_DOWN_CACHE((unsigned long)dst),
    //	    ROUND_UP_CACHE(phdr->p_filesz));
    ++phdr;
  }
#endif

/*
  dts_get_dsp_memory(&mem_start, &mem_size, dsp_id);

  if (!mem_start || !mem_size)
  {
    pr_err("dts_get_dsp_memory fail\n");
  }
  else
  {
    flush_cache(ROUND_DOWN_CACHE(mem_start), ROUND_UP_CACHE(mem_size));
  }
*/
/*
  memcpy((void*)DSP0_IRAM,img_addr,img_size);
  cache_flush_range(DSP0_IRAM,DSP0_IRAM+img_size);
*/

  zero2dsp((void*)DSP0_IRAM,0,128*1024u);

  copy2dsp((void*)DSP0_IRAM,img_addr,0,img_size);
//  memcpy((void*)DSP0_IRAM,img_addr,img_size);

//  zero2dsp((void*)DSP0_IRAM,img_size,(128*1024u)-img_size);

  cache_flush_range(DSP0_IRAM,DSP0_IRAM+(128*1024u));
}

static void dsp_freq_default_set(void)
{
  u32 val = DSP_CLK_SRC_PERI2X | DSP_CLK_FACTOR_M(2) | (1 << BIT_DSP_SCLK_GATING);
  writel_dsp(val, (SUNXI_CCM_BASE + CCMU_DSP_CLK_REG));
}

static void sram_remap_set(int value) {
  u32 val = 0;

  val = readl_dsp(SUNXI_SRAMC_BASE + SRAMC_SRAM_REMAP_REG);
  val &= ~(1 << BIT_SRAM_REMAP_ENABLE);
  val |= (value << BIT_SRAM_REMAP_ENABLE);
  writel_dsp(val, SUNXI_SRAMC_BASE + SRAMC_SRAM_REMAP_REG);
}

//int sunxi_dsp_init(u32 img_addr, u32 run_ddr, u32 dsp_id) {
void sunxi_dsp_init(const void *img_addr, u32 run_ddr, u32 dsp_id, u32 img_size) {

  u32 reg_val;

//  u32 image_len = 0;
//  struct dts_msg_t dts_msg;
//  unsigned long section_addr = 0;
//  char *str = ".oemhead.text";
//  int ret = 0;

#if 0

#ifdef CONFIG_SUNXI_VERIFY_DSP
  if (sunxi_verify_dsp(img_addr, image_len, dsp_id) < 0) {
    return -1;
  }
#endif

  /* clear dts msg data */
  memset((void *)&dts_msg, 0, sizeof(struct dts_msg_t));

  /* get dsp status */
  ret = dts_dsp_status(&dts_msg, dsp_id);
  if (ret < 0) {
    printf("dsp%d:dsp close in dts\n", dsp_id);
    return 0;
  }

  /* get dts msg about dsp */
  ret = dts_uart_msg(&dts_msg, dsp_id);
  if (ret < 0)
    printf("dsp%d:uart config fail\n", dsp_id);

  /* get gpio interrput about dsp */
  ret = dts_gpio_int_msg(&dts_msg, dsp_id);
  if (ret < 0)
    printf("dsp%d:gpio init config fail\n", dsp_id);

  ret = dts_sharespace_msg(&dts_msg, dsp_id);
  if (ret < 0)
    printf("dsp%d:sharespace config fail\n", dsp_id);
#endif

  /* set uboot use local ram */
  sram_remap_set(1);

#if 0
  /* update run addr */
  update_reset_vec(img_addr, &run_ddr);

  /* find section */
  ret = find_img_section(img_addr, str, &section_addr);
  if (ret < 0) {
    printf("dsp%d:find section err\n", dsp_id);
    return -1;
  }

  /* get img len */
  ret = get_img_len(img_addr, section_addr, &image_len);
  if (ret < 0) {
    printf("dsp%d:get img len err\n", dsp_id);
    return -1;
  }

  /* set img dts */
  ret = set_msg_dts(img_addr, section_addr, &dts_msg);
  if (ret < 0) {
    printf("dsp%d:set img dts err\n", dsp_id);
    return -1;
  }
#endif

  dsp_freq_default_set();

  if (dsp_id == 0) { /* DSP0 */

    /* clock gating */
    reg_val = readl_dsp(SUNXI_CCM_BASE + CCMU_DSP_BGR_REG);
    reg_val |= (1 << BIT_DSP0_CFG_GATING);
    writel_dsp(reg_val, SUNXI_CCM_BASE + CCMU_DSP_BGR_REG);

    /* reset */
    reg_val = readl_dsp(SUNXI_CCM_BASE + CCMU_DSP_BGR_REG);
    reg_val |= (1 << BIT_DSP0_CFG_RST);
    reg_val |= (1 << BIT_DSP0_DBG_RST);
    writel_dsp(reg_val, SUNXI_CCM_BASE + CCMU_DSP_BGR_REG);

    /* set external Reset Vector if needed */
    if (run_ddr != DSP_DEFAULT_RST_VEC)
    {
      writel_dsp(run_ddr, DSP0_CFG_BASE + DSP_ALT_RESET_VEC_REG);

      reg_val = readl_dsp(DSP0_CFG_BASE + DSP_CTRL_REG0);
      reg_val |= (1 << BIT_START_VEC_SEL);
      writel_dsp(reg_val, DSP0_CFG_BASE + DSP_CTRL_REG0);
    }

    /* set runstall */
    sunxi_dsp_set_runstall(dsp_id, 1);

    /* set dsp clken */
    reg_val = readl_dsp(DSP0_CFG_BASE + DSP_CTRL_REG0);
    reg_val |= (1 << BIT_DSP_CLKEN);
    writel_dsp(reg_val, DSP0_CFG_BASE + DSP_CTRL_REG0);

    /* de-assert dsp0 */
    reg_val = readl_dsp(SUNXI_CCM_BASE + CCMU_DSP_BGR_REG);
    reg_val |= (1 << BIT_DSP0_RST);
    writel_dsp(reg_val, SUNXI_CCM_BASE + CCMU_DSP_BGR_REG);

    /* load image to ram */
    load_image(img_addr, img_size); //������� img_size, ����� dsp_id

    /* set dsp use local ram */
    sram_remap_set(0);

    /* clear runstall */
    sunxi_dsp_set_runstall(dsp_id, 0);
  }

//  printf("DSP%d start ok, img length %d, booting from 0x%x\n", dsp_id,
//         /*image_len*/ img_size, run_ddr);
}


void tututu(void)
{
// HiFI4 DSP start test

		static const uint8_t dsp_code [] =
		{
		#include "main.txt"
		};
		//	SRAM A1			0x00020000---0x00027FFF		32 KB

		//	DSP0 IRAM		0x00028000---0x00037FFF		64 KB		The local sram is switched to system boot.
		//	DSP0 DRAM0		0x00038000---0x0003FFFF		32 KB		The local sram is switched to system boot.
		//	DSP0 DRAM1		0x00040000---0x00047FFF		32 KB		The local sram is switched to system boot.
		//
		//	DSP0 IRAM (local sram)		0x00400000---0x0040FFFF		64 KB		The local sram is switched to DSP.
		//	DSP0 DRAM0 (local sram)		0x00420000---0x00427FFF		32 KB		The local sram is switched to DSP.
		//	DSP0 DRAM1 (local sram)		0x00440000---0x00447FFF		32 KB		The local sram is switched to DSP.

		const uintptr_t remap_cpu = (uintptr_t) 0x20028000; //0x00028000;

		enum { M = 2 };
		CCU_DSP_CLK_REG = (CCU_DSP_CLK_REG & ~ ((0x07 << 24) | (0x1F << 0))) |
				(0x04u << 24) |	// src: PLL_AUDIO1(DIV2)
				((M - 1) << 0) |
				0;

		CCU_DSP_CLK_REG |= (UINT32_C(1) << 31);	// DSP_CLK_GATING


		CCU_DSP_BGR_REG |= (UINT32_C(1) << 1);	// DSP_CFG_GATING
		CCU_DSP_BGR_REG |= (UINT32_C(1) << 17);	// DSP_CFG_RST

		CCU_DSP_BGR_REG &= ~ (UINT32_C(1) << 16);	// DSP_RST

		// Map local sram to CPU
		SYS_CFG_DSP_BOOT_RAMMAP_REG = 0x01;	/* DSP BOOT SRAM REMAP ENABLE 1: DSP 128K Local SRAM Remap for System Boot */

		//https://github.com/YuzukiHD/FreeRTOS-HIFI4-DSP/blob/164696d952116d20100daefd7a475d2ede828eb0/host/uboot-driver/dsp/sun8iw20/dsp_reg.h#L33C1-L39C65

		const size_t dsp_code_size = sizeof dsp_code;

		copy2dsp((void *) remap_cpu, dsp_code, 0, dsp_code_size);
		zero2dsp((void *) remap_cpu, dsp_code_size, (128 * 1024u) - dsp_code_size);

//		dcache_clean(remap_cpu, 128 * 1024);
		cache_flush_range((u32)remap_cpu,((u32)remap_cpu)+(128*1024));

		// Map local sram to DSP
		SYS_CFG_DSP_BOOT_RAMMAP_REG = 0x00;	/* DSP BOOT SRAM REMAP ENABLE 0: DSP 128K Local SRAM Remap for DSP_SYS */

		// DSP Start address change
		DSP0_CFG_DSP_ALT_RESET_VEC_REG = 0x20028000; //0x400000 if non-cached need
		DSP0_CFG_DSP_CTRL_REG0 |= (UINT32_C(1) << 1);	// BIT_START_VEC_SEL

		DSP0_CFG_DSP_CTRL_REG0 |= (UINT32_C(1) << 0);	// Set runstall

		DSP0_CFG_DSP_CTRL_REG0 |= (UINT32_C(1) << 2);	/* set dsp clken */
		CCU_DSP_BGR_REG |= (UINT32_C(1) << 16);	// DSP_RST
		DSP0_CFG_DSP_CTRL_REG0 &= ~ (UINT32_C(1) << 0);	// Clear runstall
		(void) DSP0_CFG_DSP_CTRL_REG0;

		/*
		 * DSP STAT Register
		 */
//		#define BIT_PFAULT_INFO_VALID (0)
//		#define BIT_PFAULT_ERROR (1)
//		#define BIT_DOUBLE_EXCE_ERROR (2)
//		#define BIT_XOCD_MODE (3)
//		#define BIT_DEBUG_MODE (4)
//		#define BIT_PWAIT_MODE (5)
//		#define BIT_IRAM0_LOAD_STORE (6)

		unsigned sss = DSP0_CFG_DSP_STAT_REG;

		DelayMS(1300);
		for (;;);
}
