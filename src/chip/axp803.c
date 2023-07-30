/*
 * axp803.c
 *
 *  Created on: 7 февр. 2023 г.
 *      Author: User
 *
 *      Taken from https://github.com/jernejsk/u-boot/blob/25fa60a9ee1a92d751c0c3a4a8d4acc2495800cc/drivers/power/axp803.c
 */


/*
 * AXP803 driver based on AXP818 driver
 *
 * Based on axp818.c
 * (C) Copyright 2015 Vishnu Patekar <vishnuptekar0510@gmail.com>
 *
 * Based on axp221.c
 * (C) Copyright 2014 Hans de Goede <hdegoede@redhat.com>
 * (C) Copyright 2013 Oliver Schinagl <oliver@schinagl.nl>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

//#include <common.h>
//#include <errno.h>
//#include <asm/arch/gpio.h>
//#include <asm/arch/pmic_bus.h>
//#include <axp803_pmic.h>


#include "hardware.h"

#if WITHSDRAM_AXP803 || WITHSDRAM_AXP305 || WITHSDRAM_AXP853

#include "gpio.h"
#include "formats.h"

#include "axp803.h"

static i2cp_t pmic_i2cp;	/* параметры для обмена по I2C. Поскольку работаем до инициализации кеша-памяти, надо учитывать медленную работу процессора */

static int pmic_bus_init(void)
{
#if WITHTWISW
	i2cp_intiialize(& pmic_i2cp, I2CP_I2C1, 100000000);


	TWISOFT_INITIALIZE();
#elif WITHTWIHW
	i2c_initialize();

#endif /* WITHTWISW */

//	unsigned addr;
//	for (addr = 1; addr < 127; ++ addr)
//	{
//		uint8_t v;
//		unsigned addrw = addr * 2;
//		unsigned addrr = addrw | 0x01;
//		i2c_start(addrw);
//		i2c_write_withrestart(0x1B);
//		i2c_start(addrr);
//		i2c_read(& v, I2C_READ_ACK_NACK);
//		PRINTF("I2C 0x%02X: test=0x%02X\n", addrw, v);
//	}

	return 0;
}

static int pmic_bus_read(uint8_t reg, uint8_t * data)
{
#if WITHTWIHW
	uint8_t bufw = reg;
	i2chw_write(PMIC_I2C_W, & bufw, 1);
	i2chw_read(PMIC_I2C_R, data, 1);
#elif WITHTWISW
	unsigned addrw = PMIC_I2C_W;
	unsigned addrr = PMIC_I2C_R;

	i2cp_start(& pmic_i2cp, addrw);
	i2cp_write_withrestart(& pmic_i2cp, reg);
	i2cp_start(& pmic_i2cp, addrr);
	i2cp_read(& pmic_i2cp, data, I2C_READ_ACK_NACK);
#endif
	return 0;
}

static int pmic_bus_write(uint8_t reg, uint8_t data)
{
#if WITHTWIHW
	uint8_t bufw [] = { reg, data };
	i2chw_write(PMIC_I2C_W, bufw, ARRAY_SIZE(bufw));
#elif WITHTWISW
	unsigned addrw = PMIC_I2C_W;
	unsigned addrr = PMIC_I2C_R;

	i2cp_start(& pmic_i2cp, addrw);
	i2cp_write(& pmic_i2cp, reg);
	i2cp_write(& pmic_i2cp, data);
	i2cp_waitsend(& pmic_i2cp);
	i2cp_stop(& pmic_i2cp);
#endif
	return 0;
}

static int pmic_bus_setbits(uint8_t reg, uint8_t bits)
{
	int ret;
	uint8_t val;

	ret = pmic_bus_read(reg, &val);
	if (ret)
		return ret;

	if ((val & bits) == bits)
		return 0;

	val |= bits;
	return pmic_bus_write(reg, val);
}

static int pmic_bus_clrbits(uint8_t reg, uint8_t bits)
{
	int ret;
	uint8_t val;

	ret = pmic_bus_read(reg, &val);
	if (ret)
		return ret;

	if (!(val & bits))
		return 0;

	val &= ~bits;
	return pmic_bus_write(reg, val);
}

static uint8_t axp803_mvolt_to_cfg(int mvolt, int min, int max, int div)
{
	if (mvolt < min)
		mvolt = min;
	else if (mvolt > max)
		mvolt = max;

	return  (mvolt - min) / div;
}

static int axp803_set_dcdc1(unsigned int mvolt)
{
	int ret;
	uint8_t cfg = axp803_mvolt_to_cfg(mvolt, 1600, 3400, 100);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP803_OUTPUT_CTRL1,
					AXP803_OUTPUT_CTRL1_DCDC1_EN);

	ret = pmic_bus_write(AXP803_DCDC1_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP803_OUTPUT_CTRL1,
				AXP803_OUTPUT_CTRL1_DCDC1_EN);
}

static int axp803_set_dcdc2(unsigned int mvolt)
{
	int ret;
	uint8_t cfg;

	if (mvolt >= 1220)
		cfg = 70 + axp803_mvolt_to_cfg(mvolt, 1220, 1300, 20);
	else
		cfg = axp803_mvolt_to_cfg(mvolt, 500, 1200, 10);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP803_OUTPUT_CTRL1,
					AXP803_OUTPUT_CTRL1_DCDC2_EN);

	ret = pmic_bus_write(AXP803_DCDC2_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP803_OUTPUT_CTRL1,
				AXP803_OUTPUT_CTRL1_DCDC2_EN);
}

static int axp803_set_dcdc3(unsigned int mvolt)
{
	int ret;
	uint8_t cfg;

	if (mvolt >= 1220)
		cfg = 70 + axp803_mvolt_to_cfg(mvolt, 1220, 1300, 20);
	else
		cfg = axp803_mvolt_to_cfg(mvolt, 500, 1200, 10);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP803_OUTPUT_CTRL1,
					AXP803_OUTPUT_CTRL1_DCDC3_EN);

	ret = pmic_bus_write(AXP803_DCDC3_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP803_OUTPUT_CTRL1,
				AXP803_OUTPUT_CTRL1_DCDC3_EN);
}

static int axp803_set_dcdc5(unsigned int mvolt)
{
	int ret;
	uint8_t cfg;

	if (mvolt >= 1140)
		cfg = 32 + axp803_mvolt_to_cfg(mvolt, 1140, 1840, 20);
	else
		cfg = axp803_mvolt_to_cfg(mvolt, 800, 1120, 10);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP803_OUTPUT_CTRL1,
					AXP803_OUTPUT_CTRL1_DCDC5_EN);

	ret = pmic_bus_write(AXP803_DCDC5_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP803_OUTPUT_CTRL1,
				AXP803_OUTPUT_CTRL1_DCDC5_EN);
}

static int axp803_set_aldo(int aldo_num, unsigned int mvolt)
{
	int ret;
	uint8_t cfg;

//	if (aldo_num < 1 || aldo_num > 3)
//		return -EINVAL;

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP803_OUTPUT_CTRL3,
				AXP803_OUTPUT_CTRL3_ALDO1_EN << (aldo_num - 1));

	cfg = axp803_mvolt_to_cfg(mvolt, 700, 3300, 100);
	ret = pmic_bus_write(AXP803_ALDO1_CTRL + (aldo_num - 1), cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP803_OUTPUT_CTRL3,
				AXP803_OUTPUT_CTRL3_ALDO1_EN << (aldo_num - 1));
}

/* TODO: re-work other AXP drivers to consolidate ALDO functions. */
static int axp803_set_aldo1(unsigned int mvolt)
{
	return axp803_set_aldo(1, mvolt);
}

static int axp803_set_aldo2(unsigned int mvolt)
{
	return axp803_set_aldo(2, mvolt);
}

static int axp803_set_aldo3(unsigned int mvolt)
{
	return axp803_set_aldo(3, mvolt);
}

static int axp803_set_dldo(int dldo_num, unsigned int mvolt)
{
	int ret;
	uint8_t cfg;

//	if (dldo_num < 1 || dldo_num > 4)
//		return -EINVAL;

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP803_OUTPUT_CTRL2,
				AXP803_OUTPUT_CTRL2_DLDO1_EN << (dldo_num - 1));

	cfg = axp803_mvolt_to_cfg(mvolt, 700, 3300, 100);
	if (dldo_num == 2 && mvolt > 3300)
		cfg += 1 + axp803_mvolt_to_cfg(mvolt, 3400, 4200, 200);
	ret = pmic_bus_write(AXP803_DLDO1_CTRL + (dldo_num - 1), cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP803_OUTPUT_CTRL2,
				AXP803_OUTPUT_CTRL2_DLDO1_EN << (dldo_num - 1));
}

static int axp803_set_eldo(int eldo_num, unsigned int mvolt)
{
	int ret;
	uint8_t cfg;

//	if (eldo_num < 1 || eldo_num > 3)
//		return -EINVAL;

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP803_OUTPUT_CTRL2,
				AXP803_OUTPUT_CTRL2_ELDO1_EN << (eldo_num - 1));

	cfg = axp803_mvolt_to_cfg(mvolt, 700, 1900, 50);
	ret = pmic_bus_write(AXP803_ELDO1_CTRL + (eldo_num - 1), cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP803_OUTPUT_CTRL2,
				AXP803_OUTPUT_CTRL2_ELDO1_EN << (eldo_num - 1));
}

static int axp803_set_fldo(int fldo_num, unsigned int mvolt)
{
	int ret;
	uint8_t cfg;

//	if (fldo_num < 1 || fldo_num > 2)
//		return -EINVAL;

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP803_OUTPUT_CTRL3,
				AXP803_OUTPUT_CTRL3_FLDO1_EN << (fldo_num - 1));

	cfg = axp803_mvolt_to_cfg(mvolt, 700, 1450, 50);
	ret = pmic_bus_write(AXP803_FLDO1_CTRL + (fldo_num - 1), cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP803_OUTPUT_CTRL3,
				AXP803_OUTPUT_CTRL3_FLDO1_EN << (fldo_num - 1));
}

static int axp803_set_sw(int on)
{
	if (on)
		return pmic_bus_setbits(AXP803_OUTPUT_CTRL2,
					AXP803_OUTPUT_CTRL2_SW_EN);

	return pmic_bus_clrbits(AXP803_OUTPUT_CTRL2,
				AXP803_OUTPUT_CTRL2_SW_EN);
}

/**
 *
	&reg_aldo2 {
		regulator-always-on;
		regulator-min-microvolt = <1800000>;
		regulator-max-microvolt = <3300000>;
		regulator-name = "vcc-pl";
	};

	&reg_aldo3 {
		regulator-always-on;
		regulator-min-microvolt = <3000000>;
		regulator-max-microvolt = <3000000>;
		regulator-name = "vcc-pll-avcc";
	};

	&reg_dc1sw {
		regulator-name = "vcc-phy";
	};

	&reg_dcdc1 {
		regulator-always-on;
		regulator-min-microvolt = <3300000>;
		regulator-max-microvolt = <3300000>;
		regulator-name = "vcc-3v3";
	};

	&reg_dcdc2 {
		regulator-always-on;
		regulator-min-microvolt = <1040000>;
		regulator-max-microvolt = <1300000>;
		regulator-name = "vdd-cpux";
	};

	/ * DCDC3 is polyphased with DCDC2 * /

	&reg_dcdc5 {
		regulator-always-on;
		regulator-min-microvolt = <1500000>;
		regulator-max-microvolt = <1500000>;
		regulator-name = "vcc-dram";
	};

	&reg_dcdc6 {
		regulator-always-on;
		regulator-min-microvolt = <1100000>;
		regulator-max-microvolt = <1100000>;
		regulator-name = "vdd-sys";
	};

	&reg_dldo1 {
		regulator-min-microvolt = <3300000>;
		regulator-max-microvolt = <3300000>;
		regulator-name = "vcc-hdmi-dsi";
	};

	&reg_dldo2 {
		regulator-min-microvolt = <3300000>;
		regulator-max-microvolt = <3300000>;
		regulator-name = "vcc-wifi";
	};

	&reg_dldo4 {
		regulator-min-microvolt = <1800000>;
		regulator-max-microvolt = <3300000>;
		regulator-name = "vcc-wifi-io";
	};

	&reg_eldo1 {
		regulator-min-microvolt = <1800000>;
		regulator-max-microvolt = <1800000>;
		regulator-name = "cpvdd";
	};

	&reg_fldo1 {
		regulator-min-microvolt = <1200000>;
		regulator-max-microvolt = <1200000>;
		regulator-name = "vcc-1v2-hsic";
	};

	/ *
	 * The A64 chip cannot work without this regulator off, although
	 * it seems to be only driving the AR100 core.
	 * Maybe we don't still know well about CPUs domain.
	 * /
	&reg_fldo2 {
		regulator-always-on;
		regulator-min-microvolt = <1100000>;
		regulator-max-microvolt = <1100000>;
		regulator-name = "vdd-cpus";
	};

	&reg_rtc_ldo {
		regulator-name = "vcc-rtc";
	};
 *
 * @return
 */
int axp803_initialize(void)
{
	uint8_t axp803_chip_id;
	int ret;

	ret = pmic_bus_init();
	if (ret)
		return ret;

	ret = pmic_bus_read(AXP803_CHIP_ID, &axp803_chip_id);
	if (ret)
		return ret;

	if (0)
	{
		unsigned reg;
		for (reg = 0; reg <= 0xED; ++ reg)
		{
			uint8_t v;
			pmic_bus_read(reg, & v);
			PRINTF("axp803 reg%02X=0x%02X\n", reg, v);
		}
	}

	PRINTF("axp803_chip_id=0x%02X (expected 0x51)\n", axp803_chip_id);
	if (!(axp803_chip_id == 0x51))
		return -1;

	axp803_set_aldo1(3300);	// VCC-PE
	axp803_set_aldo2(3300);	// VCC-PL
	axp803_set_aldo3(3000);	// AVCC, VCC-PLL

//	&reg_dc1sw {
//		regulator-name = "vcc-phy";
//	};

	axp803_set_dcdc1(3300);	// VCC-CARD, VCC-PC, ...

	// plyphased
	axp803_set_dcdc2(1100);	// VDD-CPUX
	axp803_set_dcdc3(1100);	// VDD-CPUX

	axp803_set_dcdc5(1500);	// VCC-DRAM

	//axp803_set_dcdc6(1100);	// VDD-SYS

	axp803_set_dldo(1, 3300);
	axp803_set_dldo(2, 3300);
	axp803_set_dldo(4, 3300);

	axp803_set_eldo(1, 1800);	// CPVDD (analog power of CPU)
	//axp803_set_eldo(2, 1800);	// not uised in banana pi M64
	axp803_set_eldo(3, 1800);	// CSI

	axp803_set_fldo(1, 1200);	// VCC1V2-HSIC
	axp803_set_fldo(2, 1100);	// VDD-CPUX

	axp803_set_sw(1);	// reg 12h, bit 7

	return 0;
}

//int do_poweroff(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
//{
//	pmic_bus_write(AXP803_SHUTDOWN, AXP803_SHUTDOWN_POWEROFF);
//
//	/* infinite loop during shutdown */
//	while (1) {}
//
//	/* not reached */
//	return 0;
//}

// https://github.com/renesas-rcar/arm-trusted-firmware/blob/b5ad4738d907ce3e98586b453362db767b86f45d/plat/allwinner/sun50i_h616/sunxi_power.c#L23
// https://github.com/apritzel/u-boot/blob/3aaabfe9ff4bbcd11096513b1b28d1fb0a40800f/include/axp305.h#L6

// SPDX-License-Identifier: GPL-2.0+
/*
 * AXP305 driver
 *
 * (C) Copyright 2020 Jernej Skrabec <jernej.skrabec@siol.net>
 *
 * Based on axp221.c
 * (C) Copyright 2014 Hans de Goede <hdegoede@redhat.com>
 * (C) Copyright 2013 Oliver Schinagl <oliver@schinagl.nl>
 */

#define AXP305_DCDC4_1600MV_OFFSET 46

static uint8_t axp305_mvolt_to_cfg(int mvolt, int min, int max, int div)
{
	if (mvolt < min)
		mvolt = min;
	else if (mvolt > max)
		mvolt = max;

	return  (mvolt - min) / div;
}

static int axp_set_dcdcd(unsigned int mvolt)
{
	int ret;
	uint8_t cfg;

	if (mvolt >= 1600)
		cfg = AXP305_DCDC4_1600MV_OFFSET +
			axp305_mvolt_to_cfg(mvolt, 1600, 3300, 100);
	else
		cfg = axp305_mvolt_to_cfg(mvolt, 600, 1500, 20);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP305_OUTPUT_CTRL1,
					AXP305_OUTPUT_CTRL1_DCDCD_EN);

	ret = pmic_bus_write(AXP305_DCDCD_VOLTAGE, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP305_OUTPUT_CTRL1,
				AXP305_OUTPUT_CTRL1_DCDCD_EN);
}


//#if !CONFIG_IS_ENABLED(ARM_PSCI_FW) && !IS_ENABLED(CONFIG_SYSRESET_CMD_POWEROFF)
//int do_poweroff(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
//{
//	pmic_bus_write(AXP305_SHUTDOWN, AXP305_POWEROFF);
//
//	/* infinite loop during shutdown */
//	while (1) {}
//
//	/* not reached */
//	return 0;
//}
//#endif

// AXP 305 - контроллер питания производства Allwinner - аналог Datasheet AXP805

int axp305_initialize(void)
{
	uint8_t axp305_chip_id;
	int ret;

	ret = pmic_bus_init();
	if (ret)
		return ret;

	ret = pmic_bus_read(AXP803_CHIP_ID, &axp305_chip_id);
	if (ret)
		return ret;

	ret = pmic_bus_init();
	if (ret)
		return ret;

	if (0)
	{
		unsigned reg;
		for (reg = 0; reg <= 0xED; ++ reg)
		{
			uint8_t v;
			pmic_bus_read(reg, & v);
			PRINTF("axp305 reg%02X=0x%02X\n", reg, v);
		}
	}

	PRINTF("axp305_chip_id=0x%02X (expected 0x60)\n", axp305_chip_id);
	if (!(axp305_chip_id == 0x60))
		return -1;

	return 0;

	// dcdc/a: 0.9V
	// dcdc/b: NC
	// DCDC/C: 0.9V (GPU)
	// DCDC/D: 1.2V (DRM & DDR4*2)
	// DCDC/E: 3.3V
	// ALDO1: 3.3V (already started)
	// ALDO2&3: 3.3V
	// BLDO1: 1.8V
	// CLDO1: 2.5V (DDR4*2)

//
//	axp305_set_aldo1(3300);	// VCC-PE
//	axp305_set_aldo2(3300);	// VCC-PL
//	axp305_set_aldo3(3000);	// AVCC, VCC-PLL
//
////	&reg_dc1sw {
////		regulator-name = "vcc-phy";
////	};
//
//	axp305_set_dcdc1(3300);	// VCC-CARD, VCC-PC, ...
//
//	// plyphased
//	axp305_set_dcdc2(1100);	// VDD-CPUX
//	axp305_set_dcdc3(1100);	// VDD-CPUX
//
//	axp305_set_dcdc5(1500);	// VCC-DRAM
//
//	//axp305_set_dcdc6(1100);	// VDD-SYS
//
//	axp305_set_dldo(1, 3300);
//	axp305_set_dldo(2, 3300);
//	axp305_set_dldo(4, 3300);
//
//	axp305_set_eldo(1, 1800);	// CPVDD (analog power of CPU)
//	//axp305_set_eldo(2, 1800);	// not uised in banana pi M64
//	axp305_set_eldo(3, 1800);	// CSI
//
//	axp305_set_fldo(1, 1200);	// VCC1V2-HSIC
//	axp305_set_fldo(2, 1100);	// VDD-CPUX
//
//	axp305_set_sw(1);	// reg 12h, bit 7

	return 0;
}

/////////////
///
/// https://github.com/iuncuim/u-boot/blob/4faa8b19966e3741dcf6bbb398262c746a9c9782/drivers/power/axp858.c#L259
///
///
// SPDX-License-Identifier: GPL-2.0+
/*
 * AXP858 driver based on AXP221 driver
 *
 *
 * (C) Copyright 2015 Vishnu Patekar <vishnuptekar0510@gmail.com>
 *
 * Based on axp221.c
 * (C) Copyright 2014 Hans de Goede <hdegoede@redhat.com>
 * (C) Copyright 2013 Oliver Schinagl <oliver@schinagl.nl>
 */
//
//#include <common.h>
//#include <command.h>
//#include <errno.h>
//#include <asm/arch/gpio.h>
//#include <asm/arch/pmic_bus.h>
//#include <axp_pmic.h>


#define AXP858_CHIP_ID		    0x03

#define AXP858_OUTPUT_CTRL1	    0x10
#define AXP858_OUTPUT_CTRL1_DCDC1_EN	(1 << 0)
#define AXP858_OUTPUT_CTRL1_DCDC2_EN	(1 << 1)
#define AXP858_OUTPUT_CTRL1_DCDC3_EN	(1 << 2)
#define AXP858_OUTPUT_CTRL1_DCDC4_EN	(1 << 3)
#define AXP858_OUTPUT_CTRL1_DCDC5_EN	(1 << 4)
#define AXP858_OUTPUT_CTRL1_DCDC6_EN	(1 << 5)
#define AXP858_OUTPUT_CTRL2	    0x11
#define AXP858_OUTPUT_CTRL2_ALDO1_EN	(1 << 0)
#define AXP858_OUTPUT_CTRL2_ALDO2_EN	(1 << 1)
#define AXP858_OUTPUT_CTRL2_ALDO3_EN	(1 << 2)
#define AXP858_OUTPUT_CTRL2_ALDO4_EN	(1 << 3)
#define AXP858_OUTPUT_CTRL2_ALDO5_EN	(1 << 4)
#define AXP858_OUTPUT_CTRL2_BLDO1_EN	(1 << 5)
#define AXP858_OUTPUT_CTRL2_BLDO2_EN	(1 << 6)
#define AXP858_OUTPUT_CTRL2_BLDO3_EN	(1 << 7)
#define AXP858_OUTPUT_CTRL3	    0x12
#define AXP858_OUTPUT_CTRL3_BLDO4_EN	(1 << 0)
#define AXP858_OUTPUT_CTRL3_BLDO5_EN	(1 << 1)
#define AXP858_OUTPUT_CTRL3_CLDO1_EN	(1 << 2)
#define AXP858_OUTPUT_CTRL3_CLDO2_EN	(1 << 3)
#define AXP858_OUTPUT_CTRL3_CLDO3_EN	(1 << 4)
#define AXP858_OUTPUT_CTRL3_CLDO4_EN	(1 << 5)
#define AXP858_OUTPUT_CTRL3_CPUSLDO4_EN	(1 << 6)
#define AXP858_OUTPUT_CTRL3_SW_EN	    (1 << 7)

#define AXP858_DCDC1_CTRL	    0x13
#define AXP858_DCDC2_CTRL	    0x14
#define AXP858_DCDC3_CTRL	    0x15
#define AXP858_DCDC4_CTRL	    0x16
#define AXP858_DCDC5_CTRL	    0x17
#define AXP858_DCDC6_CTRL	    0x18

#define AXP858_ALDO1_CTRL	    0x19
#define AXP858_ALDO2_CTRL	    0x20
#define AXP858_ALDO3_CTRL	    0x21
#define AXP858_ALDO4_CTRL	    0x22
#define AXP858_ALDO5_CTRL	    0x23

#define AXP858_BLDO1_CTRL	    0x24
#define AXP858_BLDO2_CTRL	    0x25
#define AXP858_BLDO3_CTRL	    0x26
#define AXP858_BLDO4_CTRL	    0x27
#define AXP858_BLDO5_CTRL	    0x28

#define AXP858_CLDO1_CTRL	    0x24
#define AXP858_CLDO2_CTRL	    0x25
#define AXP858_CLDO3_CTRL	    0x26
#define AXP858_CLDO4_GPIO1_CTRL	0x27
#define AXP858_CLDO4_GPIO2_CTRL	0x28

#define AXP858_SHUTDOWN		0x32
#define AXP858_SHUTDOWN_POWEROFF	(1 << 7)

/* For axp_gpio.c */
// #define AXP_POWER_STATUS		0x00
// #define AXP_POWER_STATUS_VBUS_PRESENT		(1 << 5)
// #define AXP_VBUS_IPSOUT			0x30
// #define AXP_VBUS_IPSOUT_DRIVEBUS		(1 << 2)
// #define AXP_MISC_CTRL			0x8f
// #define AXP_MISC_CTRL_N_VBUSEN_FUNC		(1 << 4)
// #define AXP_GPIO0_CTRL			0x90
// #define AXP_GPIO1_CTRL			0x92
// #define AXP_GPIO_CTRL_OUTPUT_LOW	0x00 /* Drive pin low */
// #define AXP_GPIO_CTRL_OUTPUT_HIGH	0x01 /* Drive pin high */
// #define AXP_GPIO_CTRL_INPUT		0x02 /* Input */
// #define AXP_GPIO_STATE			0x94
// #define AXP_GPIO_STATE_OFFSET		0
static uint8_t axp858_mvolt_to_cfg(int mvolt, int min, int max, int div)
{
	if (mvolt < min)
		mvolt = min;
	else if (mvolt > max)
		mvolt = max;

	return  (mvolt - min) / div;
}

static int axp858_set_dcdc1(unsigned int mvolt)
{
	int ret;
	uint8_t cfg = axp858_mvolt_to_cfg(mvolt, 1500, 3400, 100);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP858_OUTPUT_CTRL1,
					AXP858_OUTPUT_CTRL1_DCDC1_EN);

	ret = pmic_bus_write(AXP858_DCDC1_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP858_OUTPUT_CTRL1,
				AXP858_OUTPUT_CTRL1_DCDC1_EN);
}

static int axp858_set_dcdc2(unsigned int mvolt)
{
	int ret;
	uint8_t cfg;

	if (mvolt >= 1220)
		//cfg = 70 + axp858_mvolt_to_cfg(mvolt, 1220, 1540, 20);
		cfg = 70 + axp858_mvolt_to_cfg(mvolt, 1220, 1300, 20);
	else
		cfg = axp858_mvolt_to_cfg(mvolt, 500, 1200, 10);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP858_OUTPUT_CTRL1,
					AXP858_OUTPUT_CTRL1_DCDC2_EN);

	ret = pmic_bus_write(AXP858_DCDC2_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP858_OUTPUT_CTRL1,
				AXP858_OUTPUT_CTRL1_DCDC2_EN);
}

static int axp858_set_dcdc3(unsigned int mvolt)
{
	int ret;
	uint8_t cfg;

	if (mvolt >= 1220)
		//cfg = 70 + axp858_mvolt_to_cfg(mvolt, 1220, 1540, 20);
		cfg = 70 + axp858_mvolt_to_cfg(mvolt, 1220, 1300, 20);
	else
		cfg = axp858_mvolt_to_cfg(mvolt, 500, 1200, 10);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP858_OUTPUT_CTRL1,
					AXP858_OUTPUT_CTRL1_DCDC3_EN);

	ret = pmic_bus_write(AXP858_DCDC3_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP858_OUTPUT_CTRL1,
				AXP858_OUTPUT_CTRL1_DCDC3_EN);
}

static int axp858_set_dcdc4(unsigned int mvolt)
{
	int ret;
	uint8_t cfg;

	if (mvolt >= 1140)
		cfg = 32 + axp858_mvolt_to_cfg(mvolt, 1140, 1840, 20);
	else
		cfg = axp858_mvolt_to_cfg(mvolt, 800, 1120, 10);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP858_OUTPUT_CTRL1,
					AXP858_OUTPUT_CTRL1_DCDC4_EN);

	ret = pmic_bus_write(AXP858_DCDC4_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP858_OUTPUT_CTRL1,
				AXP858_OUTPUT_CTRL1_DCDC4_EN);
}

static int axp858_set_dcdc5(unsigned int mvolt)
{
	int ret;
	uint8_t cfg;

	if (mvolt >= 1140)
		cfg = 32 + axp858_mvolt_to_cfg(mvolt, 1140, 1840, 20);
	else
		cfg = axp858_mvolt_to_cfg(mvolt, 800, 1120, 10);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP858_OUTPUT_CTRL1,
					AXP858_OUTPUT_CTRL1_DCDC5_EN);

	ret = pmic_bus_write(AXP858_DCDC5_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP858_OUTPUT_CTRL1,
				AXP858_OUTPUT_CTRL1_DCDC5_EN);
}

/* Fix fornon-sequential register address */
static int axp858_getaldor(int aldo_num)
{
	switch (aldo_num)
	{
	case 1:	return AXP858_ALDO1_CTRL;
	case 2:	return AXP858_ALDO2_CTRL;
	case 3:	return AXP858_ALDO3_CTRL;
	case 4:	return AXP858_ALDO4_CTRL;
	case 5:	return AXP858_ALDO5_CTRL;
	default:
		ASSERT(0);
		for (;;)
			;
	}
}

static int axp858_set_aldo(int aldo_num, unsigned int mvolt)
{
	int ret;
	uint8_t cfg;

	if (aldo_num < 1 || aldo_num > 5)
		return -1;

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP858_OUTPUT_CTRL2,
				AXP858_OUTPUT_CTRL2_ALDO1_EN << (aldo_num - 1));

	cfg = axp858_mvolt_to_cfg(mvolt, 700, 3300, 100);
	ret = pmic_bus_write(axp858_getaldor(aldo_num), cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP858_OUTPUT_CTRL2,
				AXP858_OUTPUT_CTRL2_ALDO1_EN << (aldo_num - 1));
}

/* TODO: re-work other AXP drivers to consolidate ALDO functions. */
static int axp858_set_aldo1(unsigned int mvolt)
{
	return axp858_set_aldo(1, mvolt);
}

static int axp858_set_aldo2(unsigned int mvolt)
{
	return axp858_set_aldo(2, mvolt);
}

static int axp858_set_aldo3(unsigned int mvolt)
{
	return axp858_set_aldo(3, mvolt);
}

static int axp858_set_aldo4(unsigned int mvolt)
{
	return axp858_set_aldo(4, mvolt);
}

static int axp858_set_aldo5(unsigned int mvolt)
{
	return axp858_set_aldo(5, mvolt);
}

// int axp858_set_dldo(int dldo_num, unsigned int mvolt)
// {
// 	int ret;
// 	uint8_t cfg;

// 	if (dldo_num < 1 || dldo_num > 4)
// 		return -EINVAL;

// 	if (mvolt == 0)
// 		return pmic_bus_clrbits(AXP858_OUTPUT_CTRL2,
// 				AXP858_OUTPUT_CTRL2_DLDO1_EN << (dldo_num - 1));

// 	cfg = axp858_mvolt_to_cfg(mvolt, 700, 3300, 100);
// 	if (dldo_num == 2 && mvolt > 3300)
// 		cfg += 1 + axp858_mvolt_to_cfg(mvolt, 3400, 4200, 200);
// 	ret = pmic_bus_write(AXP858_DLDO1_CTRL + (dldo_num - 1), cfg);
// 	if (ret)
// 		return ret;

// 	return pmic_bus_setbits(AXP858_OUTPUT_CTRL2,
// 				AXP858_OUTPUT_CTRL2_DLDO1_EN << (dldo_num - 1));
// }

// int axp858_set_eldo(int eldo_num, unsigned int mvolt)
// {
// 	int ret;
// 	uint8_t cfg;

// 	if (eldo_num < 1 || eldo_num > 3)
// 		return -EINVAL;

// 	if (mvolt == 0)
// 		return pmic_bus_clrbits(AXP858_OUTPUT_CTRL2,
// 				AXP858_OUTPUT_CTRL2_ELDO1_EN << (eldo_num - 1));

// 	cfg = axp858_mvolt_to_cfg(mvolt, 700, 1900, 50);
// 	ret = pmic_bus_write(AXP858_ELDO1_CTRL + (eldo_num - 1), cfg);
// 	if (ret)
// 		return ret;

// 	return pmic_bus_setbits(AXP858_OUTPUT_CTRL2,
// 				AXP858_OUTPUT_CTRL2_ELDO1_EN << (eldo_num - 1));
// }

// int axp858_set_fldo(int fldo_num, unsigned int mvolt)
// {
// 	int ret;
// 	uint8_t cfg;

// 	if (fldo_num < 1 || fldo_num > 3)
// 		return -EINVAL;

// 	if (mvolt == 0)
// 		return pmic_bus_clrbits(AXP858_OUTPUT_CTRL3,
// 				AXP858_OUTPUT_CTRL3_FLDO1_EN << (fldo_num - 1));

// 	if (fldo_num < 3) {
// 		cfg = axp858_mvolt_to_cfg(mvolt, 700, 1450, 50);
// 		ret = pmic_bus_write(AXP858_FLDO1_CTRL + (fldo_num - 1), cfg);
// 	} else {
// 		/*
// 		 * Special case for FLDO3, which is DCDC5 / 2 or FLDOIN / 2
// 		 * Since FLDOIN is unknown, test against DCDC5.
// 		 */
// 		if (mvolt * 2 == CONFIG_AXP_DCDC5_VOLT)
// 			ret = pmic_bus_clrbits(AXP858_FLDO2_3_CTRL,
// 					       AXP858_FLDO2_3_CTRL_FLDO3_VOL);
// 		else
// 			ret = pmic_bus_setbits(AXP858_FLDO2_3_CTRL,
// 					       AXP858_FLDO2_3_CTRL_FLDO3_VOL);
// 	}
// 	if (ret)
// 		return ret;

// 	return pmic_bus_setbits(AXP858_OUTPUT_CTRL3,
// 				AXP858_OUTPUT_CTRL3_FLDO1_EN << (fldo_num - 1));
// }

static int axp858_set_sw(int on)
{
	if (on)
		return pmic_bus_setbits(AXP858_OUTPUT_CTRL3,
					AXP858_OUTPUT_CTRL3_SW_EN);

	return pmic_bus_clrbits(AXP858_OUTPUT_CTRL3,
				AXP858_OUTPUT_CTRL3_SW_EN);
}

static int pmu_axp858_ap_reset_enable(void)
{
	uint8_t reg_value;

	if (pmic_bus_read(AXP858_SHUTDOWN,  &reg_value))
		return -1;

	reg_value |= 1 << 4;
	if (pmic_bus_write(AXP858_SHUTDOWN, reg_value))
		return -1;

	return 0;
}

int axp853_initialize(void)
{
	uint8_t axp_chip_id;
	int ret;
	uint8_t reg_value;
    PRINTF("PMIC: AXP853T/AXP858\n");
	ret = pmic_bus_init();
	if (ret)
		return ret;

	ret = pmic_bus_read(AXP858_CHIP_ID, &axp_chip_id);

	PRINTF("axp_chip_id=0x%02X (expected 0x54)\n", axp_chip_id);
	if (!(axp_chip_id == 0x54))
		return -1;

	if (0)
	{
		unsigned reg;
		for (reg = 0; reg <= 0xED; ++ reg)
		{
			uint8_t v;
			pmic_bus_read(reg, & v);
			PRINTF("axp853 reg%02X=0x%02X\n", reg, v);
		}
	}

	pmu_axp858_ap_reset_enable();	// без этой строчки не инициалищируется после reset
	axp858_set_sw(0);

	// https://artmemtech.com/
	// artmem atl4b0832

	// VDD2, VDDQ = 1.06–1.17V; VDD1 = 1.70–1.95V; TC = 0°C to +85°C

	// F1 ball VDD1: 1.8
	// A4 ball VDD2: vcc_dram 1.1

	VERIFY(0 == axp858_set_dcdc1(3300));
	VERIFY(0 == axp858_set_dcdc2(900));		// CPU
	VERIFY(0 == axp858_set_dcdc3(900));
	//VERIFY(0 == axp858_set_dcdc4(900));	// VDD-GPU
	VERIFY(0 == axp858_set_dcdc5(1100));		// VCC-DRAM - 1.1V for LPDDR4
	VERIFY(0 == axp858_set_aldo1(1800));
	VERIFY(0 == axp858_set_aldo2(1800));
	VERIFY(0 == axp858_set_aldo3(2500));		// VPP DRAM
	VERIFY(0 == axp858_set_aldo4(1800));		// 1.8V for LPDDR4
	VERIFY(0 == axp858_set_aldo5(3300));		// VCC-PE

	axp858_set_sw(1);
	//local_delay_ms(100);

	return 0;
}
#endif /* WITHSDRAM_AXP803 || WITHSDRAM_AXP305 || WITHSDRAM_AXP853 */
