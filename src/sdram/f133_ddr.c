//------------------------------------------------------------------------------------------------

/*
 Author: Rep Stosw <repstosw2018@gmail.com>
 27.06.2022

 This file was made from disassembled and decompiled \tina-t113\lichee\brandy-2.0\spl-pub\board\t113\libsun8iw20p1_fes.a
 IDA Pro 7.5 SP3 + Hex-Rays decompiler

 Just add in your code:
 extern void sys_dram_init(void);
 and use it :)
*/

//------------------------------------------------------------------------------------------------

#include "hardware.h"

#if WITHSDRAMHW
#if 0///*CPUSTYLE_T113 || */CPUSTYLE_F133

#include "formats.h"
#include "clocks.h"

// Sources taker from:
//	https://raw.githubusercontent.com/szemzoa/awboot/main/arch/arm32/mach-t113s3/mctl_hal.c
//	https://github.com/szemzoa/awboot/blob/main/arch/arm32/mach-t113s3/mctl_hal.c

typedef uintptr_t virtual_addr_t;

// SPDX-License-Identifier: GPL-2.0+

static void sdelay(unsigned us)
{
	local_delay_us(us * 2);
}

static void udelay(unsigned us)
{
	local_delay_us(us * 2);
}

static uint32_t read32(uintptr_t addr)
{
	//__DSB();
	return * (volatile uint32_t *) addr;
}

static void write32(uintptr_t addr, uint32_t value)
{
	* (volatile uint32_t *) addr = value;
	//__DSB();
}

static void write32ptr(void * addr, uint32_t value)
{
	write32((uintptr_t) addr, value);
}

static uint32_t read32ptr(void * addr)
{
	return read32((uintptr_t) addr);
}


#define clrbits_le32(addr, clear) \
		write32((addr), read32(addr) & ~(clear))

#define setbits_le32(addr, set) \
		write32((addr), read32(addr) | (set))

#define clrsetbits_le32(addr, clear, set) \
		write32((addr), (read32(addr) & ~(clear)) | (set))

#define writel(data, addr) do { write32((addr), (data)); } while (0)
#define readl(ddr) (read32(addr))
#define BIT_U32(pos) ((uint32_t) 1 << (pos))

enum sunxi_dram_type {
	SUNXI_DRAM_TYPE_DDR2 = 2,
	SUNXI_DRAM_TYPE_DDR3 = 3,
	SUNXI_DRAM_TYPE_LPDDR2 = 6,
	SUNXI_DRAM_TYPE_LPDDR3 = 7,
};

#define CONFIG_SUNXI_DRAM_TYPE SUNXI_DRAM_TYPE_DDR2

#define DIV_ROUND_UP(n, d)	(((n) + (d) - 1) / (d))

#define CONFIG_DRAM_CLK 528

static inline int ns_to_t(int nanoseconds)
{
	const unsigned int ctrl_freq = CONFIG_DRAM_CLK / 2;

	return DIV_ROUND_UP(ctrl_freq * nanoseconds, 1000);
}
#define DDRMXV(a, b) ((a) > (b) ? (a) : (b))

///////////////////////////////////////
///

#define CONFIG_SYS_SDRAM_BASE 0x40000000

#ifndef SUNXI_SID_BASE
#define SUNXI_SID_BASE	0x3006200
#endif

static void sid_read_ldoB_cal(dram_para_t *para)
{
	uint32_t reg;

	reg = (read32(SUNXI_SID_BASE + 0x1c) & 0xff00) >> 8;

	if (reg == 0)
		return;

	switch (para->dram_type) {
	case SUNXI_DRAM_TYPE_DDR2:
		break;
	case SUNXI_DRAM_TYPE_DDR3:
		if (reg > 0x20)
			reg -= 0x16;
		break;
	default:
		reg = 0;
		break;
	}

	clrsetbits_le32(0x3000150, 0xff00, reg << 8);
}

static void dram_voltage_set(dram_para_t *para)
{
	int vol;

	switch (para->dram_type) {
	case SUNXI_DRAM_TYPE_DDR2:
		vol = 47;
		break;
	case SUNXI_DRAM_TYPE_DDR3:
		vol = 25;
		break;
	default:
		vol = 0;
		break;
	}

	clrsetbits_le32(0x3000150, 0x20ff00, vol << 8);

	udelay(1);

	sid_read_ldoB_cal(para);
}

static void dram_enable_all_master(void)
{
	writel(~0, 0x3102020);
	writel(0xff, 0x3102024);
	writel(0xffff, 0x3102028);
	udelay(10);
}

static void dram_disable_all_master(void)
{
	writel(1, 0x3102020);
	writel(0, 0x3102024);
	writel(0, 0x3102028);
	udelay(10);
}

static void eye_delay_compensation(dram_para_t *para) // s1
{
	uint32_t delay;
	unsigned long ptr;

	// DATn0IOCR, n =  0...7
	delay = (para->dram_tpr11 & 0xf) << 9;
	delay |= (para->dram_tpr12 & 0xf) << 1;
	for (ptr = 0x3103310; ptr < 0x3103334; ptr += 4)
		setbits_le32(ptr, delay);

	// DATn1IOCR, n =  0...7
	delay = (para->dram_tpr11 & 0xf0) << 5;
	delay |= (para->dram_tpr12 & 0xf0) >> 3;
	for (ptr = 0x3103390; ptr != 0x31033b4; ptr += 4)
		setbits_le32(ptr, delay);

	// PGCR0: assert AC loopback FIFO reset
	clrbits_le32(0x3103100, 0x04000000);

	// ??

	delay = (para->dram_tpr11 & 0xf0000) >> 7;
	delay |= (para->dram_tpr12 & 0xf0000) >> 15;
	setbits_le32(0x3103334, delay);
	setbits_le32(0x3103338, delay);

	delay = (para->dram_tpr11 & 0xf00000) >> 11;
	delay |= (para->dram_tpr12 & 0xf00000) >> 19;
	setbits_le32(0x31033b4, delay);
	setbits_le32(0x31033b8, delay);

	setbits_le32(0x310333c, (para->dram_tpr11 & 0xf0000) << 9);
	setbits_le32(0x31033bc, (para->dram_tpr11 & 0xf00000) << 5);

	// PGCR0: release AC loopback FIFO reset
	setbits_le32(0x3103100, BIT_U32(26));

	udelay(1);

	delay = (para->dram_tpr10 & 0xf0) << 4;
	for (ptr = 0x3103240; ptr != 0x310327c; ptr += 4)
		setbits_le32(ptr, delay);
	for (ptr = 0x3103228; ptr != 0x3103240; ptr += 4)
		setbits_le32(ptr, delay);

	setbits_le32(0x3103218, (para->dram_tpr10 & 0x0f) << 8);
	setbits_le32(0x310321c, (para->dram_tpr10 & 0x0f) << 8);

	setbits_le32(0x3103280, (para->dram_tpr10 & 0xf00) >> 4);
}

/*
 * Main purpose of the auto_set_timing routine seems to be to calculate all
 * timing settings for the specific type of sdram used. Read together with
 * an sdram datasheet for context on the various variables.
 */
static void mctl_set_timing_params(dram_para_t *para)
{
	/* DRAM_TPR0 */
	uint8_t tccd		= 2;
	uint8_t tfaw;
	uint8_t trrd;
	uint8_t trcd;
	uint8_t trc;

	/* DRAM_TPR1 */
	uint8_t txp;
	uint8_t twtr;
	uint8_t trtp		= 4;
	uint8_t twr;
	uint8_t trp;
	uint8_t tras;

	/* DRAM_TPR2 */
	uint16_t trefi;
	uint16_t trfc;

	uint8_t tcksrx;
	uint8_t tckesr;
	uint8_t trd2wr;
	uint8_t twr2rd;
	uint8_t trasmax;
	uint8_t twtp;
	uint8_t tcke;
	uint8_t tmod;
	uint8_t tmrd;
	uint8_t tmrw;

	uint8_t tcl;
	uint8_t tcwl;
	uint8_t t_rdata_en;
	uint8_t wr_latency;

	uint32_t mr0;
	uint32_t mr1;
	uint32_t mr2;
	uint32_t mr3;

	uint32_t tdinit0;
	uint32_t tdinit1;
	uint32_t tdinit2;
	uint32_t tdinit3;

	switch (CONFIG_SUNXI_DRAM_TYPE) {
	case SUNXI_DRAM_TYPE_DDR2:
		/* DRAM_TPR0 */
		tfaw		= ns_to_t(50);
		trrd		= ns_to_t(10);
		trcd		= ns_to_t(20);
		trc		= ns_to_t(65);

		/* DRAM_TPR1 */
		txp		= 2;
		twtr		= ns_to_t(8);
		twr		= ns_to_t(15);
		trp		= ns_to_t(15);
		tras		= ns_to_t(45);

		/* DRAM_TRP2 */
		trfc		= ns_to_t(328);
		trefi		= ns_to_t(7800) / 32;

		trasmax		= CONFIG_DRAM_CLK / 30;
		if (CONFIG_DRAM_CLK < 409) {
			t_rdata_en	= 1;
			tcl		= 3;
			mr0		= 0x06a3;
		} else {
			t_rdata_en	= 2;
			tcl		= 4;
			mr0		= 0x0e73;
		}
		tmrd		= 2;
		twtp		= twr + 5;
		tcksrx		= 5;
		tckesr		= 4;
		trd2wr		= 4;
		tcke		= 3;
		tmod		= 12;
		wr_latency	= 1;
		tmrw		= 0;
		twr2rd		= twtr + 5;
		tcwl		= 0;

		mr1		= para->dram_mr1;
		mr2		= 0;
		mr3		= 0;

		tdinit0		= 200 * CONFIG_DRAM_CLK + 1;
		tdinit1		= 100 * CONFIG_DRAM_CLK / 1000 + 1;
		tdinit2		= 200 * CONFIG_DRAM_CLK + 1;
		tdinit3		= 1 * CONFIG_DRAM_CLK + 1;

		break;
	case SUNXI_DRAM_TYPE_DDR3:
		trfc		= ns_to_t(350);
		trefi		= ns_to_t(7800) / 32 + 1;	// XXX

		twtr		= ns_to_t(8) + 2;		// + 2 ? XXX
		/* Only used by trd2wr calculation, which gets discard below */
//		twr		= DDRMXV(ns_to_t(15), 2);
		trrd		= DDRMXV(ns_to_t(10), 2);
		txp		= DDRMXV(ns_to_t(10), 2);

		if (CONFIG_DRAM_CLK <= 800) {
			tfaw		= ns_to_t(50);
			trcd		= ns_to_t(15);
			trp		= ns_to_t(15);
			trc		= ns_to_t(53);
			tras		= ns_to_t(38);

			mr0		= 0x1c70;
			mr2		= 0x18;
			tcl		= 6;
			wr_latency	= 2;
			tcwl		= 4;
			t_rdata_en	= 4;
		} else {
			tfaw		= ns_to_t(35);
			trcd		= ns_to_t(14);
			trp		= ns_to_t(14);
			trc		= ns_to_t(48);
			tras		= ns_to_t(34);

			mr0		= 0x1e14;
			mr2		= 0x20;
			tcl		= 7;
			wr_latency	= 3;
			tcwl		= 5;
			t_rdata_en	= 5;
		}

		trasmax		= CONFIG_DRAM_CLK / 30;
		twtp		= tcwl + 2 + twtr;		// WL+BL/2+tWTR
		/* Gets overwritten below */
//		trd2wr		= tcwl + 2 + twr;		// WL+BL/2+tWR
		twr2rd		= tcwl + twtr;			// WL+tWTR

		tdinit0		= 500 * CONFIG_DRAM_CLK + 1;	// 500 us
		tdinit1		= 360 * CONFIG_DRAM_CLK / 1000 + 1;   // 360 ns
		tdinit2		= 200 * CONFIG_DRAM_CLK + 1;	// 200 us
		tdinit3		= 1 * CONFIG_DRAM_CLK + 1;	//   1 us

		mr1		= para->dram_mr1;
		mr3		= 0;
		tcke		= 3;
		tcksrx		= 5;
		tckesr		= 4;
		if (((para->dram_tpr13 & 0xc) == 0x04) || CONFIG_DRAM_CLK < 912)
			trd2wr	   = 5;
		else
			trd2wr	   = 6;

		tmod		= 12;
		tmrd		= 4;
		tmrw		= 0;

		break;
	case SUNXI_DRAM_TYPE_LPDDR2:
		tfaw		= DDRMXV(ns_to_t(50), 4);
		trrd		= DDRMXV(ns_to_t(10), 1);
		trcd		= DDRMXV(ns_to_t(24), 2);
		trc		= ns_to_t(70);
		txp		= ns_to_t(8);
		if (txp < 2) {
			txp++;
			twtr	= 2;
		} else {
			twtr	= txp;
		}
		twr		= DDRMXV(ns_to_t(15), 2);
		trp		= ns_to_t(17);
		tras		= ns_to_t(42);
		trefi		= ns_to_t(3900) / 32;
		trfc		= ns_to_t(210);

		trasmax		= CONFIG_DRAM_CLK / 60;
		mr3		= para->dram_mr3;
		twtp		= twr + 5;
		mr2		= 6;
		mr1		= 5;
		tcksrx		= 5;
		tckesr		= 5;
		trd2wr		= 10;
		tcke		= 2;
		tmod		= 5;
		tmrd		= 5;
		tmrw		= 3;
		tcl		= 4;
		wr_latency	= 1;
		t_rdata_en	= 1;

		tdinit0		= 200 * CONFIG_DRAM_CLK + 1;
		tdinit1		= 100 * CONFIG_DRAM_CLK / 1000 + 1;
		tdinit2		= 11 * CONFIG_DRAM_CLK + 1;
		tdinit3		= 1 * CONFIG_DRAM_CLK + 1;
		twr2rd		= twtr + 5;
		tcwl		= 2;
		mr1		= 195;
		mr0		= 0;

		break;
	case SUNXI_DRAM_TYPE_LPDDR3:
		tfaw		= DDRMXV(ns_to_t(50), 4);
		trrd		= DDRMXV(ns_to_t(10), 1);
		trcd		= DDRMXV(ns_to_t(24), 2);
		trc		= ns_to_t(70);
		twtr		= DDRMXV(ns_to_t(8), 2);
		twr		= DDRMXV(ns_to_t(15), 2);
		trp		= ns_to_t(17);
		tras		= ns_to_t(42);
		trefi		= ns_to_t(3900) / 32;
		trfc		= ns_to_t(210);
		txp		= twtr;

		trasmax		= CONFIG_DRAM_CLK / 60;
		if (CONFIG_DRAM_CLK < 800) {
			tcwl	   = 4;
			wr_latency = 3;
			t_rdata_en = 6;
			mr2		   = 12;
		} else {
			tcwl	   = 3;
			tcke	   = 6;
			wr_latency = 2;
			t_rdata_en = 5;
			mr2		   = 10;
		}
		twtp		= tcwl + 5;
		tcl		= 7;
		mr3		= para->dram_mr3;
		tcksrx		= 5;
		tckesr		= 5;
		trd2wr		= 13;
		tcke		= 3;
		tmod		= 12;
		tdinit0		= 400 * CONFIG_DRAM_CLK + 1;
		tdinit1		= 500 * CONFIG_DRAM_CLK / 1000 + 1;
		tdinit2		= 11 * CONFIG_DRAM_CLK + 1;
		tdinit3		= 1 * CONFIG_DRAM_CLK + 1;
		tmrd		= 5;
		tmrw		= 5;
		twr2rd		= tcwl + twtr + 5;
		mr1		= 195;
		mr0		= 0;

		break;
	default:
		trfc		= 128;
		trp		= 6;
		trefi		= 98;
		txp		= 10;
		twr		= 8;
		twtr		= 3;
		tras		= 14;
		tfaw		= 16;
		trc		= 20;
		trcd		= 6;
		trrd		= 3;

		twr2rd		= 8; // 48(sp)
		tcksrx		= 4; // t1
		tckesr		= 3; // t4
		trd2wr		= 4; // t6
		trasmax		= 27; // t3
		twtp		= 12; // s6
		tcke		= 2; // s8
		tmod		= 6; // t0
		tmrd		= 2; // t5
		tmrw		= 0; // a1
		tcwl		= 3; // a5
		tcl		= 3; // a0
		wr_latency	= 1; // a7
		t_rdata_en	= 1; // a4
		mr3		= 0; // s0
		mr2		= 0; // t2
		mr1		= 0; // s1
		mr0		= 0; // a3
		tdinit3		= 0; // 40(sp)
		tdinit2		= 0; // 32(sp)
		tdinit1		= 0; // 24(sp)
		tdinit0		= 0; // 16(sp)

		break;
	}

	/* Set mode registers */
	writel(mr0, 0x3103030);
	writel(mr1, 0x3103034);
	writel(mr2, 0x3103038);
	writel(mr3, 0x310303c);
	/* TODO: dram_odt_en is either 0x0 or 0x1, so right shift looks weird */
	writel((para->dram_odt_en >> 4) & 0x3, 0x310302c);

	/* Set dram timing DRAMTMG0 - DRAMTMG5 */
	writel((twtp << 24) | (tfaw << 16) | (trasmax << 8) | (tras << 0),
		0x3103058);
	writel((txp << 16) | (trtp << 8) | (trc << 0),
		0x310305c);
	writel((tcwl << 24) | (tcl << 16) | (trd2wr << 8) | (twr2rd << 0),
		0x3103060);
	writel((tmrw << 16) | (tmrd << 12) | (tmod << 0),
		0x3103064);
	writel((trcd << 24) | (tccd << 16) | (trrd << 8) | (trp << 0),
		0x3103068);
	writel((tcksrx << 24) | (tcksrx << 16) | (tckesr << 8) | (tcke << 0),
		0x310306c);

	/* Set dual rank timing */
	clrsetbits_le32(0x3103078, 0xf000ffff,
			(CONFIG_DRAM_CLK < 800) ? 0xf0006610 : 0xf0007610);

	/* Set phy interface time PITMG0, PTR3, PTR4 */
	writel((0x2 << 24) | (t_rdata_en << 16) | BIT_U32(8) | (wr_latency << 0),
		0x3103080);
	writel(((tdinit0 << 0) | (tdinit1 << 20)), 0x3103050);
	writel(((tdinit2 << 0) | (tdinit3 << 20)), 0x3103054);

	/* Set refresh timing and mode */
	writel((trefi << 16) | (trfc << 0), 0x3103090);
	writel((trefi << 15) & 0x0fff0000, 0x3103094);
}

// Purpose of this routine seems to be to initialize the PLL driving
// the MBUS and sdram.
//
static int ccu_set_pll_ddr_clk(int index, dram_para_t *para)
{
	unsigned int val, clk, n;

	if (para->dram_tpr13 & BIT_U32(6))
		clk = para->dram_tpr9;
	else
		clk = para->dram_clk;

	// set VCO clock divider
	n = (clk * 2) / 24;

	val = read32(0x2001010);
	val &= 0xfff800fc; // clear dividers
	val |= (n - 1) << 8; // set PLL division
	val |= 0xc0000000; // enable PLL and LDO
	val &= 0xdfffffff;
	writel(val | 0x20000000, 0x2001010);

	// wait for PLL to lock
	while ((read32(0x2001010) & 0x10000000) == 0) {
		;
	}

	udelay(20);

	// enable PLL output
	val = read32(0x2001000);
	val |= 0x08000000;
	writel(val, 0x2001000);

	// turn clock gate on
	val = read32(0x2001800);
	val &= 0xfcfffcfc; // select DDR clk source, n=1, m=1
	val |= 0x80000000; // turn clock on
	writel(val, 0x2001800);

	return n * 24;
}

// Main purpose of sys_init seems to be to initalise the clocks for
// the sdram controller.
//
static void mctl_sys_init(dram_para_t *para)
{
	// assert MBUS reset
	clrbits_le32(0x2001540, BIT_U32(30));

	// turn off sdram clock gate, assert sdram reset
	clrbits_le32(0x200180c, 0x10001);
	clrsetbits_le32(0x2001800, BIT_U32(31) | BIT_U32(30), BIT_U32(27));
	udelay(10);

	// set ddr pll clock
	para->dram_clk = ccu_set_pll_ddr_clk(0, para) / 2;
	udelay(100);
	dram_disable_all_master();

	// release sdram reset
	setbits_le32(0x200180c, BIT_U32(16));

	// release MBUS reset
	setbits_le32(0x2001540, BIT_U32(30));
	setbits_le32(0x2001800, BIT_U32(30));

	udelay(5);

	// turn on sdram clock gate
	setbits_le32(0x200180c, BIT_U32(0));

	// turn dram clock gate on, trigger sdr clock update
	setbits_le32(0x2001800, BIT_U32(31) | BIT_U32(27));
	udelay(5);

	// mCTL clock enable
	writel(0x8000, 0x310300c);
	udelay(10);
}

// The main purpose of this routine seems to be to copy an address configuration
// from the dram_para1 and dram_para2 fields to the PHY configuration registers
// (0x3102000, 0x3102004).
//
static void mctl_com_init(dram_para_t *para)
{
	uint32_t val, width;
	unsigned long ptr;
	int i;

	// purpose ??
	clrsetbits_le32(0x3102008, 0x3f00, 0x2000);

	// set SDRAM type and word width
	val  = read32(0x3102000) & ~0x00fff000;
	val |= (para->dram_type & 0x7) << 16;		// DRAM type
	val |= (~para->dram_para2 & 0x1) << 12;		// DQ width
	val |= BIT_U32(22);					// ??
	if (para->dram_type == SUNXI_DRAM_TYPE_LPDDR2 ||
	    para->dram_type == SUNXI_DRAM_TYPE_LPDDR3) {
		val |= BIT_U32(19);		// type 6 and 7 must use 1T
	} else {
		if (para->dram_tpr13 & BIT_U32(5))
			val |= BIT_U32(19);
	}
	writel(val, 0x3102000);

	// init rank / bank / row for single/dual or two different ranks
	if ((para->dram_para2 & BIT_U32(8)) &&
	    ((para->dram_para2 & 0xf000) != 0x1000))
		width = 32;
	else
		width = 16;

	ptr = 0x3102000;
	for (i = 0; i < width; i += 16) {
		val = read32(ptr) & 0xfffff000;

		val |= (para->dram_para2 >> 12) & 0x3; // rank
		val |= ((para->dram_para1 >> (i + 12)) << 2) & 0x4; // bank - 2
		val |= (((para->dram_para1 >> (i + 4)) - 1) << 4) & 0xff; // row - 1

		// convert from page size to column addr width - 3
		switch ((para->dram_para1 >> i) & 0xf) {
		case 8: val |= 0xa00; break;
		case 4: val |= 0x900; break;
		case 2: val |= 0x800; break;
		case 1: val |= 0x700; break;
		default: val |= 0x600; break;
		}
		writel(val, ptr);
		ptr += 4;
	}

	// set ODTMAP based on number of ranks in use
	val = (read32(0x3102000) & 0x1) ? 0x303 : 0x201;
	writel(val, 0x3103120);

	// set mctl reg 3c4 to zero when using half DQ
	if (para->dram_para2 & BIT_U32(0))
		writel(0, 0x31033c4);

	// purpose ??
	if (para->dram_tpr4) {
                setbits_le32(0x3102000, (para->dram_tpr4 & 0x3) << 25);
                setbits_le32(0x3102004, (para->dram_tpr4 & 0x7fc) << 10);
	}
}

static const uint8_t ac_remapping_tables[][22] = {
	[0] = { 0 },
	[1] = {  1,  9,  3,  7,  8, 18,  4, 13,  5,  6, 10,
		 2, 14, 12,  0,  0, 21, 17, 20, 19, 11, 22 },
	[2] = {  4,  9,  3,  7,  8, 18,  1, 13,  2,  6, 10,
		 5, 14, 12,  0,  0, 21, 17, 20, 19, 11, 22 },
	[3] = {  1,  7,  8, 12, 10, 18,  4, 13,  5,  6,  3,
		 2,  9,  0,  0,  0, 21, 17, 20, 19, 11, 22 },
	[4] = {  4, 12, 10,  7,  8, 18,  1, 13,  2,  6,  3,
		 5,  9,  0,  0,  0, 21, 17, 20, 19, 11, 22 },
	[5] = { 13,  2,  7,  9, 12, 19,  5,  1,  6,  3,  4,
		 8, 10,  0,  0,  0, 21, 22, 18, 17, 11, 20 },
	[6] = {  3, 10,  7, 13,  9, 11,  1,  2,  4,  6,  8,
		 5, 12,  0,  0,  0, 20,  1,  0, 21, 22, 17 },
	[7] = {  3,  2,  4,  7,  9,  1, 17, 12, 18, 14, 13,
		 8, 15,  6, 10,  5, 19, 22, 16, 21, 20, 11 },
};

/*
 * This routine chooses one of several remapping tables for 22 lines.
 * It is unclear which lines are being remapped. It seems to pick
 * table cfg7 for the Nezha board.
 */
static void mctl_phy_ac_remapping(dram_para_t *para)
{
	const uint8_t *cfg;
	uint32_t fuse, val;

	/*
	 * It is unclear whether the LPDDRx types don't need any remapping,
	 * or whether the original code just didn't provide tables.
	 */
	if (para->dram_type != SUNXI_DRAM_TYPE_DDR2 &&
	    para->dram_type != SUNXI_DRAM_TYPE_DDR3)
		return;

	fuse = (read32(SUNXI_SID_BASE + 0x28) & 0xf00) >> 8;
	PRINTF("DDR efuse: 0x%x\n", fuse);

	if (para->dram_type == SUNXI_DRAM_TYPE_DDR2) {
		if (fuse == 15)
			return;
		cfg = ac_remapping_tables[6];
	} else {
		if (para->dram_tpr13 & 0xc0000) {
			cfg = ac_remapping_tables[7];
		} else {
			switch (fuse) {
			case 8: cfg = ac_remapping_tables[2]; break;
			case 9: cfg = ac_remapping_tables[3]; break;
			case 10: cfg = ac_remapping_tables[5]; break;
			case 11: cfg = ac_remapping_tables[4]; break;
			default:
			case 12: cfg = ac_remapping_tables[1]; break;
			case 13:
			case 14: cfg = ac_remapping_tables[0]; break;
			}
		}
	}

	val = (cfg[4] << 25) | (cfg[3] << 20) | (cfg[2] << 15) |
	      (cfg[1] << 10) | (cfg[0] << 5);
	writel(val, 0x3102500);

	val = (cfg[10] << 25) | (cfg[9] << 20) | (cfg[8] << 15) |
	      (cfg[ 7] << 10) | (cfg[6] <<  5) | cfg[5];
	writel(val, 0x3102504);

	val = (cfg[15] << 20) | (cfg[14] << 15) | (cfg[13] << 10) |
	      (cfg[12] <<  5) | cfg[11];
	writel(val, 0x3102508);

	val = (cfg[21] << 25) | (cfg[20] << 20) | (cfg[19] << 15) |
	      (cfg[18] << 10) | (cfg[17] <<  5) | cfg[16];
	writel(val, 0x310250c);

	val = (cfg[4] << 25) | (cfg[3] << 20) | (cfg[2] << 15) |
	      (cfg[1] << 10) | (cfg[0] <<  5) | 1;
	writel(val, 0x3102500);
}

// Init the controller channel. The key part is placing commands in the main
// command register (PIR, 0x3103000) and checking command status (PGSR0, 0x3103010).
//
static unsigned int mctl_channel_init(unsigned int ch_index, dram_para_t *para)
{
	unsigned int val, dqs_gating_mode;

	dqs_gating_mode = (para->dram_tpr13 & 0xc) >> 2;

	// set DDR clock to half of CPU clock
	clrsetbits_le32(0x310200c, 0xfff, (para->dram_clk / 2) - 1);

	// MRCTRL0 nibble 3 undocumented
	clrsetbits_le32(0x3103108, 0xf00, 0x300);

	if (para->dram_odt_en)
		val = 0;
	else
		val = BIT_U32(5);

	// DX0GCR0
	if (para->dram_clk > 672)
		clrsetbits_le32(0x3103344, 0xf63e, val);
	else
		clrsetbits_le32(0x3103344, 0xf03e, val);

	// DX1GCR0
	if (para->dram_clk > 672) {
                setbits_le32(0x3103344, 0x400);
		clrsetbits_le32(0x31033c4, 0xf63e, val);
	} else {
		clrsetbits_le32(0x31033c4, 0xf03e, val);
	}

	// 0x3103208 undocumented
	setbits_le32(0x3103208, BIT_U32(1));

	eye_delay_compensation(para);

	// set PLL SSCG ?
	val = read32(0x3103108);
	if (dqs_gating_mode == 1) {
		clrsetbits_le32(0x3103108, 0xc0, 0);
		clrbits_le32(0x31030bc, 0x107);
	} else if (dqs_gating_mode == 2) {
		clrsetbits_le32(0x3103108, 0xc0, 0x80);

		clrsetbits_le32(0x31030bc, 0x107,
				(((para->dram_tpr13 >> 16) & 0x1f) - 2) | 0x100);
		clrsetbits_le32(0x310311c, BIT_U32(31), BIT_U32(27));
	} else {
		clrbits_le32(0x3103108, 0x40);
		udelay(10);
		setbits_le32(0x3103108, 0xc0);
	}

	if (para->dram_type == SUNXI_DRAM_TYPE_LPDDR2 ||
	    para->dram_type == SUNXI_DRAM_TYPE_LPDDR3) {
		if (dqs_gating_mode == 1)
			clrsetbits_le32(0x310311c, 0x080000c0, 0x80000000);
		else
			clrsetbits_le32(0x310311c, 0x77000000, 0x22000000);
	}

	clrsetbits_le32(0x31030c0, 0x0fffffff,
			(para->dram_para2 & BIT_U32(12)) ? 0x03000001 : 0x01000007);

	if (read32(0x70005d4) & (1 << 16)) {
		clrbits_le32(0x7010250, 0x2);
		udelay(10);
	}

	// Set ZQ config
	clrsetbits_le32(0x3103140, 0x3ffffff,
			(para->dram_zq & 0x00ffffff) | BIT_U32(25));

	// Initialise DRAM controller
	if (dqs_gating_mode == 1) {
		//writel(0x52, 0x3103000); // prep PHY reset + PLL init + z-cal
		writel(0x53, 0x3103000); // Go

		while ((read32(0x3103010) & 0x1) == 0) {
		} // wait for IDONE
		udelay(10);

		// 0x520 = prep DQS gating + DRAM init + d-cal
		if (para->dram_type == SUNXI_DRAM_TYPE_DDR3)
			writel(0x5a0, 0x3103000);		// + DRAM reset
		else
			writel(0x520, 0x3103000);
	} else {
		if ((read32(0x70005d4) & (1 << 16)) == 0) {
			// prep DRAM init + PHY reset + d-cal + PLL init + z-cal
			if (para->dram_type == SUNXI_DRAM_TYPE_DDR3)
				writel(0x1f2, 0x3103000);	// + DRAM reset
			else
				writel(0x172, 0x3103000);
		} else {
			// prep PHY reset + d-cal + z-cal
			writel(0x62, 0x3103000);
		}
	}

	setbits_le32(0x3103000, 0x1);		 // GO

	udelay(10);
	while ((read32(0x3103010) & 0x1) == 0) {
	} // wait for IDONE

	if (read32(0x70005d4) & (1 << 16)) {
		clrsetbits_le32(0x310310c, 0x06000000, 0x04000000);
		udelay(10);

		setbits_le32(0x3103004, 0x1);

		while ((read32(0x3103018) & 0x7) != 0x3) {
		}

		clrbits_le32(0x7010250, 0x1);
		udelay(10);

		clrbits_le32(0x3103004, 0x1);

		while ((read32(0x3103018) & 0x7) != 0x1) {
		}

		udelay(15);

		if (dqs_gating_mode == 1) {
			clrbits_le32(0x3103108, 0xc0);
			clrsetbits_le32(0x310310c, 0x06000000, 0x02000000);
			udelay(1);
			writel(0x401, 0x3103000);

			while ((read32(0x3103010) & 0x1) == 0) {
			}
		}
	}

	// Check for training error
	if (read32(0x3103010) & BIT_U32(20)) {
		printf("ZQ calibration error, check external 240 ohm resistor\n");
		return 0;
	}

	// STATR = Zynq STAT? Wait for status 'normal'?
	while ((read32(0x3103018) & 0x1) == 0) {
	}

	setbits_le32(0x310308c, BIT_U32(31));
	udelay(10);
	clrbits_le32(0x310308c, BIT_U32(31));
	udelay(10);
	setbits_le32(0x3102014, BIT_U32(31));
	udelay(10);

	clrbits_le32(0x310310c, 0x06000000);

	if (dqs_gating_mode == 1)
		clrsetbits_le32(0x310311c, 0xc0, 0x40);

	return 1;
}

static unsigned int calculate_rank_size(uint32_t regval)
{
	unsigned int bits;

	bits = (regval >> 8) & 0xf;	/* page size - 3 */
	bits += (regval >> 4) & 0xf;	/* row width - 1 */
	bits += (regval >> 2) & 0x3;	/* bank count - 2 */
	bits -= 14;			/* 1MB = 20 bits, minus above 6 = 14 */

	return 1U << bits;
}

/*
 * The below routine reads the dram config registers and extracts
 * the number of address bits in each rank available. It then calculates
 * total memory size in MB.
 */
static unsigned int DRAMC_get_dram_size(void)
{
	uint32_t val;
	unsigned int size;

	val = read32(0x3102000);		/* MC_WORK_MODE0 */
	size = calculate_rank_size(val);
	if ((val & 0x3) == 0)		/* single rank? */
		return size;

	val = read32(0x3102004);		/* MC_WORK_MODE1 */
	if ((val & 0x3) == 0)		/* two identical ranks? */
		return size * 2;

	/* add sizes of both ranks */
	return size + calculate_rank_size(val);
}

/*
 * The below routine reads the command status register to extract
 * DQ width and rank count. This follows the DQS training command in
 * channel_init. If error bit 22 is reset, we have two ranks and full DQ.
 * If there was an error, figure out whether it was half DQ, single rank,
 * or both. Set bit 12 and 0 in dram_para2 with the results.
 */
static int dqs_gate_detect(dram_para_t *para)
{
	uint32_t dx0 = 0, dx1 = 0;

	if ((read32(0x3103010) & BIT_U32(22)) == 0) {
		para->dram_para2 = (para->dram_para2 & ~0xf) | BIT_U32(12);
		PRINTF("dual rank and full DQ\n");

		return 1;
	}

	dx0 = (read32(0x03103348) & 0x3000000) >> 24;
	if (dx0 == 0) {
		para->dram_para2 = (para->dram_para2 & ~0xf) | 0x1001;
		PRINTF("dual rank and half DQ\n");

		return 1;
	}

	if (dx0 == 2) {
		dx1 = (read32(0x031033c8) & 0x3000000) >> 24;
		if (dx1 == 2) {
			para->dram_para2 = para->dram_para2 & ~0xf00f;
			PRINTF("single rank and full DQ\n");
		} else {
			para->dram_para2 = (para->dram_para2 & ~0xf00f) | BIT_U32(0);
			PRINTF("single rank and half DQ\n");
		}

		return 1;
	}

	if ((para->dram_tpr13 & BIT_U32(29)) == 0)
		return 0;

	PRINTF("DX0 state: %d\n", dx0);
	PRINTF("DX1 state: %d\n", dx1);

	return 0;
}

static int dramc_simple_wr_test(unsigned int mem_mb, int len)
{
	unsigned int  offs	= (mem_mb / 2) << 18; // half of memory size
	unsigned int  patt1 = 0x01234567;
	unsigned int  patt2 = 0xfedcba98;
	unsigned int *addr, v1, v2, i;

	addr = (unsigned int *)CONFIG_SYS_SDRAM_BASE;
	for (i = 0; i != len; i++, addr++) {
		writel(patt1 + i, (unsigned long)addr);
		writel(patt2 + i, (unsigned long)(addr + offs));
	}

	addr = (unsigned int *)CONFIG_SYS_SDRAM_BASE;
	for (i = 0; i != len; i++) {
		v1 = read32((unsigned long)(addr + i));
		v2 = patt1 + i;
		if (v1 != v2) {
			printf("DRAM: simple test FAIL\n");
			printf("%x != %x at address %p\n", v1, v2, addr + i);
			return 1;
		}
		v1 = read32((unsigned long)(addr + offs + i));
		v2 = patt2 + i;
		if (v1 != v2) {
			printf("DRAM: simple test FAIL\n");
			printf("%x != %x at address %p\n", v1, v2, addr + offs + i);
			return 1;
		}
	}

	PRINTF("DRAM: simple test OK\n");
	return 0;
}

// Set the Vref mode for the controller
//
static void mctl_vrefzq_init(dram_para_t *para)
{
	if (para->dram_tpr13 & BIT_U32(17))
		return;

	clrsetbits_le32(0x3103110, 0x7f7f7f7f, para->dram_tpr5);

	// IOCVR1
	if ((para->dram_tpr13 & BIT_U32(16)) == 0)
		clrsetbits_le32(0x3103114, 0x7f, para->dram_tpr6 & 0x7f);
}

// Perform an init of the controller. This is actually done 3 times. The first
// time to establish the number of ranks and DQ width. The second time to
// establish the actual ram size. The third time is final one, with the final
// settings.
//
static int mctl_core_init(dram_para_t *para)
{
	mctl_sys_init(para);

	mctl_vrefzq_init(para);

	mctl_com_init(para);

	mctl_phy_ac_remapping(para);

	mctl_set_timing_params(para);

	return mctl_channel_init(0, para);
}

/*
 * This routine sizes a DRAM device by cycling through address lines and
 * figuring out if they are connected to a real address line, or if the
 * address is a mirror.
 * First the column and bank bit allocations are set to low values (2 and 9
 * address lines). Then a maximum allocation (16 lines) is set for rows and
 * this is tested.
 * Next the BA2 line is checked. This seems to be placed above the column,
 * BA0-1 and row addresses. Finally, the column address is allocated 13 lines
 * and these are tested. The results are placed in dram_para1 and dram_para2.
 */
static int auto_scan_dram_size(dram_para_t *para)
{
	unsigned int rval, i, j, rank, maxrank, offs;
	unsigned int shft;
	unsigned long ptr, mc_work_mode, chk;

	if (mctl_core_init(para) == 0) {
		printf("DRAM initialisation error : 0\n");
		return 0;
	}

	maxrank	= (para->dram_para2 & 0xf000) ? 2 : 1;
	mc_work_mode = 0x3102000;
	offs = 0;

	/* write test pattern */
	for (i = 0, ptr = CONFIG_SYS_SDRAM_BASE; i < 64; i++, ptr += 4)
		writel((i & 0x1) ? ptr : ~ptr, ptr);

	for (rank = 0; rank < maxrank;) {
		/* set row mode */
		clrsetbits_le32(mc_work_mode, 0xf0c, 0x6f0);
		udelay(1);

		// Scan per address line, until address wraps (i.e. see shadow)
		for (i = 11; i < 17; i++) {
			chk = CONFIG_SYS_SDRAM_BASE + (1U << (i + 11));
			ptr = CONFIG_SYS_SDRAM_BASE;
			for (j = 0; j < 64; j++) {
				if (read32(chk) != ((j & 1) ? ptr : ~ptr))
					break;
				ptr += 4;
				chk += 4;
			}
			if (j == 64)
				break;
		}
		if (i > 16)
			i = 16;
		PRINTF("rank %d row = %d\n", rank, i);

		/* Store rows in para 1 */
		shft = offs + 4;
		rval = para->dram_para1;
		rval &= ~(0xff << shft);
		rval |= i << shft;
		para->dram_para1 = rval;

		if (rank == 1)		/* Set bank mode for rank0 */
			clrsetbits_le32(0x3102000, 0xffc, 0x6a4);

		/* Set bank mode for current rank */
		clrsetbits_le32(mc_work_mode, 0xffc, 0x6a4);
		udelay(1);

		// Test if bit A23 is BA2 or mirror XXX A22?
		chk = CONFIG_SYS_SDRAM_BASE + (1U << 22);
		ptr = CONFIG_SYS_SDRAM_BASE;
		for (i = 0, j = 0; i < 64; i++) {
			if (read32(chk) != ((i & 1) ? ptr : ~ptr)) {
				j = 1;
				break;
			}
			ptr += 4;
			chk += 4;
		}

		PRINTF("rank %d bank = %d\n", rank, (j + 1) << 2); /* 4 or 8 */

//		[AUTO DEBUG] rank 0 row = 13
//		[AUTO DEBUG] rank 0 bank = 4
//		[AUTO DEBUG] rank 0 page size = 2 KB

		/* Store banks in para 1 */
		shft = 12 + offs;
		rval = para->dram_para1;
		rval &= ~(0xf << shft);
		rval |= j << shft;
		para->dram_para1 = rval;

		if (rank == 1)		/* Set page mode for rank0 */
			clrsetbits_le32(0x3102000, 0xffc, 0xaa0);

		/* Set page mode for current rank */
		clrsetbits_le32(mc_work_mode, 0xffc, 0xaa0);
		udelay(1);

		// Scan per address line, until address wraps (i.e. see shadow)
		for (i = 9; i < 14; i++) {
			chk = CONFIG_SYS_SDRAM_BASE + (1U << i);
			ptr = CONFIG_SYS_SDRAM_BASE;
			for (j = 0; j < 64; j++) {
				if (read32(chk) != ((j & 1) ? ptr : ~ptr))
					break;
				ptr += 4;
				chk += 4;
			}
			if (j == 64)
				break;
		}
		if (i > 13)
			i = 13;

		unsigned int pgsize = (i == 9) ? 0 : (1 << (i - 10));
		PRINTF("rank %d page size = %d KB\n", rank, pgsize);

		/* Store page size */
		shft = offs;
		rval = para->dram_para1;
		rval &= ~(0xf << shft);
		rval |= pgsize << shft;
		para->dram_para1 = rval;

		// Move to next rank
		rank++;
		if (rank != maxrank) {
			if (rank == 1) {
				/* MC_WORK_MODE */
				clrsetbits_le32(0x3202000, 0xffc, 0x6f0);

				/* MC_WORK_MODE2 */
				clrsetbits_le32(0x3202004, 0xffc, 0x6f0);
			}
			/* store rank1 config in upper half of para1 */
			offs += 16;
			mc_work_mode += 4;	/* move to MC_WORK_MODE2 */
		}
	}
	if (maxrank == 2) {
		para->dram_para2 &= 0xfffff0ff;
		/* note: rval is equal to para->dram_para1 here */
		if ((rval & 0xffff) == (rval >> 16)) {
			PRINTF("rank1 config same as rank0\n");
		} else {
			para->dram_para2 |= BIT_U32(8);
			PRINTF("rank1 config different from rank0\n");
		}
	}

	return 1;
}

/*
 * This routine sets up parameters with dqs_gating_mode equal to 1 and two
 * ranks enabled. It then configures the core and tests for 1 or 2 ranks and
 * full or half DQ width. It then resets the parameters to the original values.
 * dram_para2 is updated with the rank and width findings.
 */
static int auto_scan_dram_rank_width(dram_para_t *para)
{
	unsigned int s1 = para->dram_tpr13;
	unsigned int s2 = para->dram_para1;

	para->dram_para1 = 0x00b000b0;
	para->dram_para2 = (para->dram_para2 & ~0xf) | BIT_U32(12);

	/* set DQS probe mode */
	para->dram_tpr13 = (para->dram_tpr13 & ~0x8) | BIT_U32(2) | BIT_U32(0);

	mctl_core_init(para);

	if (read32(0x3103010) & BIT_U32(20))
		return 0;

	if (dqs_gate_detect(para) == 0)
		return 0;

	para->dram_tpr13 = s1;
	para->dram_para1 = s2;

	return 1;
}

/*
 * This routine determines the SDRAM topology. It first establishes the number
 * of ranks and the DQ width. Then it scans the SDRAM address lines to establish
 * the size of each rank. It then updates dram_tpr13 to reflect that the sizes
 * are now known: a re-init will not repeat the autoscan.
 */
static int auto_scan_dram_config(dram_para_t *para)
{
	if (((para->dram_tpr13 & BIT_U32(14)) == 0) &&
	    (auto_scan_dram_rank_width(para) == 0)) {
		printf("ERROR: auto scan dram rank & width failed\n");
		return 0;
	}

	if (((para->dram_tpr13 & BIT_U32(0)) == 0) &&
	    (auto_scan_dram_size(para) == 0)) {
		printf("ERROR: auto scan dram size failed\n");
		return 0;
	}

	if ((para->dram_tpr13 & BIT_U32(15)) == 0)
		para->dram_tpr13 |= BIT_U32(14) | BIT_U32(13) | BIT_U32(1) | BIT_U32(0);

	return 1;
}

int init_DRAM(int type, dram_para_t *para)
{
	uint32_t rc, mem_size_mb;

	PRINTF("DRAM BOOT DRIVE INFO: %s\n", "V0.24");
	PRINTF("DRAM CLK = %d MHz\n", para->dram_clk);
	PRINTF("DRAM Type = %d (2:DDR2,3:DDR3)\n", para->dram_type);
	if ((para->dram_odt_en & 0x1) == 0)
		PRINTF("DRAMC read ODT off\n");
	else
		PRINTF("DRAMC ZQ value: 0x%x\n", para->dram_zq);

	/* Test ZQ status */
	if (para->dram_tpr13 & BIT_U32(16)) {
		PRINTF("DRAM only have internal ZQ\n");
		setbits_le32(0x3000160, BIT_U32(8));
		writel(0, 0x3000168);
		udelay(10);
	} else {
		clrbits_le32(0x3000160, 0x3);
		writel(para->dram_tpr13 & BIT_U32(16), 0x7010254);
		udelay(10);
		clrsetbits_le32(0x3000160, 0x108, BIT_U32(1));
		udelay(10);
		setbits_le32(0x3000160, BIT_U32(0));
		udelay(20);
		PRINTF("ZQ value = 0x%x\n", read32(0x300016c));
	}

	dram_voltage_set(para);

	/* Set SDRAM controller auto config */
	if ((para->dram_tpr13 & BIT_U32(0)) == 0) {
		if (auto_scan_dram_config(para) == 0) {
			printf("auto_scan_dram_config() FAILED\n");
			return 0;
		}
	}

	/* report ODT */
	rc = para->dram_mr1;
	if ((rc & 0x44) == 0)
		PRINTF("DRAM ODT off\n");
	else
		PRINTF("DRAM ODT value: 0x%x\n", rc);

	/* Init core, final run */
	if (mctl_core_init(para) == 0) {
		printf("DRAM initialisation error: 1\n");
		return 0;
	}

	/* Get SDRAM size */
	/* TODO: who ever puts a negative number in the top half? */
	rc = para->dram_para2;
	if (rc & BIT_U32(31)) {
		rc = (rc >> 16) & ~BIT_U32(15);
	} else {
		rc = DRAMC_get_dram_size();
		PRINTF("DRAM: size = %dMB\n", rc);
		para->dram_para2 = (para->dram_para2 & 0xffffU) | rc << 16;
	}
	mem_size_mb = rc;

	/* Purpose ?? */
	if (para->dram_tpr13 & BIT_U32(30)) {
		rc = para->dram_tpr8;
		if (rc == 0)
			rc = 0x10000200;
		writel(rc, 0x31030a0);
		writel(0x40a, 0x310309c);
		setbits_le32(0x3103004, BIT_U32(0));
		PRINTF("Enable Auto SR\n");
	} else {
		clrbits_le32(0x31030a0, 0xffff);
		clrbits_le32(0x3103004, 0x1);
	}

	/* Purpose ?? */
	if (para->dram_tpr13 & BIT_U32(9)) {
		clrsetbits_le32(0x3103100, 0xf000, 0x5000);
	} else {
		if (para->dram_type != SUNXI_DRAM_TYPE_LPDDR2)
			clrbits_le32(0x3103100, 0xf000);
	}

	setbits_le32(0x3103140, BIT_U32(31));

	/* CHECK: is that really writing to a different register? */
	if (para->dram_tpr13 & BIT_U32(8))
		writel(read32(0x3103140) | 0x300, 0x31030b8);

	if (para->dram_tpr13 & BIT_U32(16))
		clrbits_le32(0x3103108, BIT_U32(13));
	else
		setbits_le32(0x3103108, BIT_U32(13));

	/* Purpose ?? */
	if (para->dram_type == SUNXI_DRAM_TYPE_LPDDR3)
		clrsetbits_le32(0x310307c, 0xf0000, 0x1000);

	dram_enable_all_master();
	if (para->dram_tpr13 & BIT_U32(28)) {
		if ((read32(0x70005d4) & BIT_U32(16)) ||
		    dramc_simple_wr_test(mem_size_mb, 4096))
			return 0;
	}

	return mem_size_mb;
}


#endif /* CPUSTYLE_T113 */
#endif /* WITHSDRAMHW */
