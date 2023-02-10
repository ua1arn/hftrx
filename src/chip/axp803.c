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
#include "gpio.h"
#include "formats.h"

#include "axp803.h"

#define PMIC_I2C_W 0x68
#define PMIC_I2C_R (PMIC_I2C_W | 0x01)

static i2cp_t pmic_i2cp;	/* параметры для обмена по I2C. Поскольку работаем до инициализации кеша-памяти, надо учитывать медленную работу процессора */

int pmic_bus_init(void)
{
	i2cp_intiialize(& pmic_i2cp, I2CP_I2C1, 100000000);
	TWISOFT_INITIALIZE();
//
//
//	uint8_t v;
//	unsigned addrw = PMIC_I2C_W;
//	unsigned addrr = PMIC_I2C_R;
//	////%%TP();
//	i2c_start(addrw);
//	i2c_write_withrestart(0x1B);
//	i2c_start(addrr);
//	i2c_read(& v, I2C_READ_ACK_NACK);
//	////%%TP();
//	PRINTF("I2C 0x%02X: test=0x%02X\n", addrw, v);

	return 0;
}

int pmic_bus_read(uint8_t reg, uint8_t * data)
{
	unsigned addrw = PMIC_I2C_W;
	unsigned addrr = PMIC_I2C_R;

	i2cp_start(& pmic_i2cp, addrw);
	i2cp_write_withrestart(& pmic_i2cp, reg);
	i2cp_start(& pmic_i2cp, addrr);
	i2cp_read(& pmic_i2cp, data, I2C_READ_ACK_NACK);

	return 0;
}

int pmic_bus_write(uint8_t reg, uint8_t data)
{
	unsigned addrw = PMIC_I2C_W;
	unsigned addrr = PMIC_I2C_R;

	i2cp_start(& pmic_i2cp, addrw);
	i2cp_write(& pmic_i2cp, reg);
	i2cp_write(& pmic_i2cp, data);
	i2cp_waitsend(& pmic_i2cp);
	i2cp_stop(& pmic_i2cp);

	return 0;
}

int pmic_bus_setbits(uint8_t reg, uint8_t bits)
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

int pmic_bus_clrbits(uint8_t reg, uint8_t bits)
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

int axp803_set_dcdc1(unsigned int mvolt)
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

int axp803_set_dcdc2(unsigned int mvolt)
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

int axp803_set_dcdc3(unsigned int mvolt)
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

int axp803_set_dcdc5(unsigned int mvolt)
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

int axp803_set_aldo(int aldo_num, unsigned int mvolt)
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
int axp803_set_aldo1(unsigned int mvolt)
{
	return axp803_set_aldo(1, mvolt);
}

int axp803_set_aldo2(unsigned int mvolt)
{
	return axp803_set_aldo(2, mvolt);
}

int axp803_set_aldo3(unsigned int mvolt)
{
	return axp803_set_aldo(3, mvolt);
}

int axp803_set_dldo(int dldo_num, unsigned int mvolt)
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

int axp803_set_eldo(int eldo_num, unsigned int mvolt)
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

int axp803_set_fldo(int fldo_num, unsigned int mvolt)
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

int axp803_set_sw(int on)
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

	//PRINTF("axp803_chip_id=0x%02X (expected 0x51)\n", axp803_chip_id);
//	if (!(axp803_chip_id == 0x51))
//		return -ENODEV;
//	else
//		return ret;

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

	//axp803_set_dcdc6(1100);	// VDD-SYS

	axp803_set_dldo(1, 3300);
	axp803_set_dldo(2, 3300);
	axp803_set_dldo(4, 3300);

	axp803_set_eldo(1, 1800);

	axp803_set_fldo(1, 1200);
	axp803_set_fldo(2, 1100);

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
