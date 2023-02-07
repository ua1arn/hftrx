/**
 * Taken from https://github.com/jernejsk/u-boot/blob/25fa60a9ee1a92d751c0c3a4a8d4acc2495800cc/include/axp803.h
 *
 */

/*
 * (C) Copyright 2016 Icenowy Zheng <icenowy@aosc.xyz>
 *
 * Based on axp818.h, which is:
 * (C) Copyright 2015 Vishnu Patekar <vishnupatekar0510@gmail.com>
 *
 * X-Powers AXP803 Power Management IC driver
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#define AXP803_CHIP_ID		0x03

#define AXP803_OUTPUT_CTRL1	0x10
#define AXP803_OUTPUT_CTRL1_DCDC1_EN	(1 << 0)
#define AXP803_OUTPUT_CTRL1_DCDC2_EN	(1 << 1)
#define AXP803_OUTPUT_CTRL1_DCDC3_EN	(1 << 2)
#define AXP803_OUTPUT_CTRL1_DCDC4_EN	(1 << 3)
#define AXP803_OUTPUT_CTRL1_DCDC5_EN	(1 << 4)
#define AXP803_OUTPUT_CTRL1_DCDC6_EN	(1 << 5)
#define AXP803_OUTPUT_CTRL2	0x12
#define AXP803_OUTPUT_CTRL2_ELDO1_EN	(1 << 0)
#define AXP803_OUTPUT_CTRL2_ELDO2_EN	(1 << 1)
#define AXP803_OUTPUT_CTRL2_ELDO3_EN	(1 << 2)
#define AXP803_OUTPUT_CTRL2_DLDO1_EN	(1 << 3)
#define AXP803_OUTPUT_CTRL2_DLDO2_EN	(1 << 4)
#define AXP803_OUTPUT_CTRL2_DLDO3_EN	(1 << 5)
#define AXP803_OUTPUT_CTRL2_DLDO4_EN	(1 << 6)
#define AXP803_OUTPUT_CTRL2_SW_EN	(1 << 7)
#define AXP803_OUTPUT_CTRL3	0x13
#define AXP803_OUTPUT_CTRL3_FLDO1_EN	(1 << 2)
#define AXP803_OUTPUT_CTRL3_FLDO2_EN	(1 << 3)
#define AXP803_OUTPUT_CTRL3_ALDO1_EN	(1 << 5)
#define AXP803_OUTPUT_CTRL3_ALDO2_EN	(1 << 6)
#define AXP803_OUTPUT_CTRL3_ALDO3_EN	(1 << 7)

#define AXP803_DLDO1_CTRL	0x15
#define AXP803_DLDO2_CTRL	0x16
#define AXP803_DLDO3_CTRL	0x17
#define AXP803_DLDO4_CTRL	0x18
#define AXP803_ELDO1_CTRL	0x19
#define AXP803_ELDO2_CTRL	0x1a
#define AXP803_ELDO3_CTRL	0x1b
#define AXP803_FLDO1_CTRL	0x1c
#define AXP803_FLDO2_CTRL	0x1d
#define AXP803_DCDC1_CTRL	0x20
#define AXP803_DCDC2_CTRL	0x21
#define AXP803_DCDC3_CTRL	0x22
#define AXP803_DCDC4_CTRL	0x23
#define AXP803_DCDC5_CTRL	0x24
#define AXP803_DCDC6_CTRL	0x25

#define AXP803_ALDO1_CTRL	0x28
#define AXP803_ALDO2_CTRL	0x29
#define AXP803_ALDO3_CTRL	0x2a

#define AXP803_SHUTDOWN		0x32
#define AXP803_SHUTDOWN_POWEROFF	(1 << 7)

/* For axp_gpio.c */
#define AXP_POWER_STATUS		0x00
#define AXP_POWER_STATUS_VBUS_PRESENT		(1 << 5)
#define AXP_VBUS_IPSOUT			0x30
#define AXP_VBUS_IPSOUT_DRIVEBUS		(1 << 2)
#define AXP_MISC_CTRL			0x8f
#define AXP_MISC_CTRL_N_VBUSEN_FUNC		(1 << 4)
#define AXP_GPIO0_CTRL			0x90
#define AXP_GPIO1_CTRL			0x92
#define AXP_GPIO_CTRL_OUTPUT_LOW	0x00 /* Drive pin low */
#define AXP_GPIO_CTRL_OUTPUT_HIGH	0x01 /* Drive pin high */
#define AXP_GPIO_CTRL_INPUT		0x02 /* Input */
#define AXP_GPIO_STATE			0x94
#define AXP_GPIO_STATE_OFFSET		0
