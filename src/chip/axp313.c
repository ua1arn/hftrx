/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//


#include "hardware.h"

#if WITHSDRAM_AXP313 || WITHSDRAM_AXP318

#include "axp_regulator.h"
#include "formats.h"
#include <string.h>

enum axp313_reg {
	AXP313_CHIP_VERSION	= 0x03,
	AXP313_OUTPUT_CTRL	= 0x10,
    AXP313_DCDC1_CTRL	= 0x13,
	AXP313_SHUTDOWN		= 0x1a,
};

#define AXP313A_CHIP_VERSION_MASK	0xcf
#define AXP313A_CHIP_VERSION_AXP1530	0x48
#define AXP313A_CHIP_VERSION_AXP313A	0x4b
#define AXP313A_CHIP_VERSION_AXP313B	0x4c

enum axp313a_reg {
	AXP313A_POWER_STATUS = 0x00,
	AXP313A_CHIP_VERSION = 0x03,
	AXP313A_OUTPUT_CTRL = 0x10,
	AXP313A_DCDC1_CTRL = 0x13,
	AXP313A_DCDC2_CTRL = 0x14,
	AXP313A_DCDC3_CTRL = 0x15,
	AXP313A_ALDO1_CTRL = 0x16,
	AXP313A_DLDO1_CTRL = 0x17,
	AXP313A_SHUTDOWN = 0x1a,
	AXP313A_IRQ_ENABLE = 0x20,
	AXP313A_IRQ_STATUS = 0x21,
};

#define AXP313A_OUTPUT_CTRL_DCDC1	BIT(0)
#define AXP313A_OUTPUT_CTRL_DCDC2	BIT(1)
#define AXP313A_OUTPUT_CTRL_DCDC3	BIT(2)
#define AXP313A_OUTPUT_CTRL_ALDO1	BIT(3)
#define AXP313A_OUTPUT_CTRL_DLDO1	BIT(4)

#define AXP313A_POWEROFF		BIT(7)
///-----------------
#define AXP313_CHIP_VERSION_MASK	0xcf
#define AXP313_CHIP_VERSION_AXP1530	0x48
#define AXP313_CHIP_VERSION_AXP313A	0x4b
#define AXP313_CHIP_VERSION_AXP313B	0x4c

#define AXP313_DCDC_SPLIT_OFFSET	71
#define AXP313_DCDC_SPLIT_MVOLT		1200

#define AXP313_POWEROFF			BIT(7)

// #define AXP_SHUTDOWN_REG	0x27
// #define AXP_SHUTDOWN_MASK	BIT(0)
#ifndef BIT
#define BIT(x)              (1 << (x))
#endif

#if WITHSDRAM_AXP313				/* AXP313 */


#define AXP_CHIP_VERSION	0x3
#define AXP_CHIP_VERSION_MASK	0xc8
#define AXP_CHIP_ID		0x48
//#define AXP_SHUTDOWN_REG	0x1a
//#define AXP_SHUTDOWN_MASK	BIT(7)

 #else

 	//#error "Please define the regulator registers in axp_spl_regulators[]."

 #endif


// https://github.com/u-boot/u-boot/blob/1de103fc29761fa729dffaa15d0cfb2766be05e4/drivers/power/regulator/axp_regulator.c#L198

//struct axp_regulator_plat {
//	const char	*name;
//	uint8_t		enable_reg;
//	uint8_t		enable_mask;
//	uint8_t		volt_reg;
//	uint8_t		volt_mask;
//	uint16_t		min_mV;
//	uint16_t		max_mV;
//	uint8_t		step_mV;
//	uint8_t		split;
//	const uint16_t	*table;
//};

/*
 * The "dcdc1" regulator has another range, beyond 1.54V up to 3.4V, in
 * steps of 100mV. We cannot model this easily, but also don't need that,
 * since it's typically only used for ~1.1V anyway, so just ignore it.
 * Also the DCDC3 regulator is described wrongly in the (available) manual,
 * experiments show that the split point is at 1200mV, as for DCDC1/2.
 */
static const struct axp_regulator_plat axp313_regulators[] = {
	{ "dcdc1", 0x10, BIT(0), 0x13, 0x7f,  500, 1540,  10, 70 },
	{ "dcdc2", 0x10, BIT(1), 0x14, 0x7f,  500, 1540,  10, 70 },
	{ "dcdc3", 0x10, BIT(2), 0x15, 0x7f,  500, 1840,  10, 70 },
	{ "aldo1", 0x10, BIT(3), 0x16, 0x1f,  500, 3500, 100, NA },
	{ "dldo1", 0x10, BIT(4), 0x17, 0x1f,  500, 3500, 100, NA },
	{ }
};

static void axp313_set(const char * name, unsigned mvolt)
{
	const int ec = axpXXX_set_value(axp313_regulators, name, mvolt);
	ASSERT(ec >= 0);
	if (ec < 0)
		return;
	const int ec2 = axpXXX_set_enable(axp313_regulators, name, 1);
	ASSERT(ec2 >= 0);
}

static void axp313_setstate(const char * name, unsigned state)
{
	const int ec = axpXXX_set_enable(axp313_regulators, name, state);
	ASSERT(ec >= 0);
}

int board_orangepi_zero3_axp313_initialize(void)
{
	PRINTF("START PMIC \n");
	uint8_t axp313_chip_id;
	int ret;

	ret = axpXXX_bus_init();
	if (ret)
	{
		PRINTF("axpXXX_bus_init() failure.\n");
			dbg_flush();
		return ret;
	}

	ret = pmic_bus_read(AXP313_CHIP_VERSION, &axp313_chip_id);
	if (ret)
	{
		PRINTF("pmic_bus_read() failure.\n");
			dbg_flush();
		return ret;
	}

	if (0)
	{
		uint8_t registers [0xEE];
		unsigned reg;
		for (reg = 0; reg < ARRAY_SIZE(registers); ++ reg)
		{
			if (pmic_bus_read(reg, & registers [reg]))
				break;
		}
		PRINTF("axp313 registers (count = 0x%02X):\n", reg);
		printhex(0, registers, reg);
	}

	PRINTF("axp313_chip_id=0x%02X (expected 0x%02X)\n", axp313_chip_id, 0x4B);

	dbg_flush();
	if (!(axp313_chip_id == 0x4B))
		return -1;
    PRINTF("axp313_chip_id=OK\n");

	//axpXXX_print(axp313_regulators);

	axp313_set("aldo1", 1800);	///VCC 1V8
	axp313_set("dldo1", 3300);	///VCC3V3
	axp313_set("dcdc1", 970);	///810-990 VDD-GPU-SYS
	axp313_set("dcdc2", 970);	///810-1100 VDD-CPU
	axp313_set("dcdc3", 1100);	///VCC-DRAM - 1.1V for LPDDR4

	PRINTF("axp313 INIT END\n");
	//axpXXX_print(axp313_regulators);
	return 0;
}

int board_orangepi_zero2w_axp313_initialize(void)
{
	PRINTF("START PMIC \n");
	uint8_t axp313_chip_id;
	int ret;

	ret = axpXXX_bus_init();
	if (ret)
	{
		PRINTF("axpXXX_bus_init() failure.\n");
			dbg_flush();
		return ret;
	}

	ret = pmic_bus_read(AXP313_CHIP_VERSION, &axp313_chip_id);
	if (ret)
	{
		PRINTF("pmic_bus_read() failure.\n");
			dbg_flush();
		return ret;
	}

	if (0)
	{
		uint8_t registers [0xEE];
		unsigned reg;
		for (reg = 0; reg < ARRAY_SIZE(registers); ++ reg)
		{
			if (pmic_bus_read(reg, & registers [reg]))
				break;
		}
		PRINTF("axp313 registers (count = 0x%02X):\n", reg);
		printhex(0, registers, reg);
	}

	PRINTF("axp313_chip_id=0x%02X (expected 0x%02X)\n", axp313_chip_id, 0x4B);

	dbg_flush();
	if (!(axp313_chip_id == 0x4B))
		return -1;
    PRINTF("axp313_chip_id=OK\n");

	//axpXXX_print(axp313_regulators);

	axp313_set("aldo1", 1800);	///VCC 1V8
	axp313_set("dldo1", 3300);	///VCC3V3
	axp313_set("dcdc1", 970);	///810-990 VDD-GPU-SYS
	axp313_set("dcdc2", 970);	///810-1100 VDD-CPU
	axp313_set("dcdc3", 1100);	///VCC-DRAM - 1.1V for LPDDR4

	PRINTF("axp313 INIT END\n");
	//axpXXX_print(axp313_regulators);
	return 0;
}

/*
 * Only two level step tuning is implemented for DCDC6, 8, 9
 * so the voltage below is not support in this driver
 * DCDC6:   20  (v1.8 - 2.4v), 40 (2.44v - 2.76v)
 * DCDC8,9: 100 (1.9v - 3.4v)
 */
static const struct axp_regulator_plat axp318_regulators[] = {
	{  "dcdc1", 0x10, BIT(0), 0x12, 0x1f, 1000, 3400, 100, NA },
	{  "dcdc2", 0x10, BIT(1), 0x13, 0x7f,  500, 1540,  10, 70 },
	{  "dcdc3", 0x10, BIT(2), 0x14, 0x7f,  500, 1540,  10, 70 },
	{  "dcdc4", 0x10, BIT(3), 0x15, 0x7f,  500, 1540,  10, 70 },
	{  "dcdc5", 0x10, BIT(4), 0x16, 0x7f,  500, 1540,  10, 70 },
	{  "dcdc6", 0x10, BIT(5), 0x17, 0x7f,  500, 1540,  10, 70 },
	{  "dcdc7", 0x10, BIT(6), 0x18, 0x7f,  500, 1840,  10, 70 },
	{  "dcdc8", 0x10, BIT(7), 0x19, 0x7f,  500, 1840,  10, 70 },
	{  "dcdc9", 0x11, BIT(0), 0x1a, 0x7f,  500, 1840,  10, 70 },
	{  "aldo1", 0x20, BIT(0), 0x24, 0x1f,  500, 3400, 100, NA },
	{  "aldo2", 0x20, BIT(1), 0x25, 0x1f,  500, 3400, 100, NA },
	{  "aldo3", 0x20, BIT(2), 0x26, 0x1f,  500, 3400, 100, NA },
	{  "aldo4", 0x20, BIT(3), 0x27, 0x1f,  500, 3400, 100, NA },
	{  "aldo5", 0x20, BIT(4), 0x28, 0x1f,  500, 3400, 100, NA },
	{  "aldo6", 0x20, BIT(5), 0x29, 0x1f,  500, 3400, 100, NA },
	{  "bldo1", 0x20, BIT(6), 0x2a, 0x1f,  500, 3400, 100, NA },
	{  "bldo2", 0x20, BIT(7), 0x2b, 0x1f,  500, 3400, 100, NA },
	{  "bldo3", 0x21, BIT(0), 0x2c, 0x1f,  500, 3400, 100, NA },
	{  "bldo4", 0x21, BIT(1), 0x2d, 0x1f,  500, 3400, 100, NA },
	{  "bldo5", 0x21, BIT(2), 0x2e, 0x1f,  500, 3400, 100, NA },
	{  "cldo1", 0x21, BIT(3), 0x2f, 0x1f,  500, 3400, 100, NA },
	{  "cldo2", 0x21, BIT(4), 0x30, 0x1f,  500, 3400, 100, NA },
	{  "cldo3", 0x21, BIT(5), 0x31, 0x1f,  500, 3400, 100, NA },
	{  "cldo4", 0x21, BIT(6), 0x32, 0x1f,  500, 3400, 100, NA },
	{  "cldo5", 0x21, BIT(7), 0x33, 0x1f,  500, 3400, 100, NA },
	{  "dldo1", 0x22, BIT(0), 0x34, 0x1f,  500, 3400, 100, NA },
	{  "dldo2", 0x22, BIT(1), 0x35, 0x1f,  500, 3400, 100, NA },
	{  "dldo3", 0x22, BIT(2), 0x36, 0x1f,  500, 3400, 100, NA },
	{  "dldo4", 0x22, BIT(3), 0x37, 0x1f,  500, 3400, 100, NA },
	{  "dldo5", 0x22, BIT(4), 0x38, 0x1f,  500, 3400, 100, NA },
	{  "dldo6", 0x22, BIT(5), 0x39, 0x1f,  500, 3400, 100, NA },
	{  "eldo1", 0x22, BIT(6), 0x3a, 0x1f,  500, 1500,  25, NA },
	{  "eldo2", 0x22, BIT(7), 0x3b, 0x1f,  500, 1500,  25, NA },
	{  "eldo3", 0x23, BIT(0), 0x3c, 0x1f,  500, 1500,  25, NA },
	{  "eldo4", 0x23, BIT(1), 0x3d, 0x1f,  500, 1500,  25, NA },
	{  "eldo5", 0x23, BIT(2), 0x3e, 0x1f,  500, 1500,  25, NA },
	{  "eldo6", 0x23, BIT(3), 0x3f, 0x1f,  500, 1500,  25, NA },
	{ "swout1", 0x11, BIT(3),   NA,   NA,   NA,   NA,  NA, NA },
	{ "swout2", 0x11, BIT(4),   NA,   NA,   NA,   NA,  NA, NA },
	{ }
};

static void axp318w_set(const char * name, unsigned mvolt)
{
	const int ec = axpXXX_set_value(axp318_regulators, name, mvolt);
	ASSERT(ec >= 0);
	if (ec < 0)
		return;
	const int ec2 = axpXXX_set_enable(axp318_regulators, name, 1);
	ASSERT(ec2 >= 0);
}

static void axp318w_setstate(const char * name, unsigned state)
{
	const int ec = axpXXX_set_enable(axp318_regulators, name, state);
	ASSERT(ec >= 0);
}

int board_radaxa_cubie_axp318w_initialize(void)
{

	PRINTF("START PMIC \n");
	uint8_t axp318_chip_id;
	int ret;

	ret = axpXXX_bus_init();
	if (ret)
	{
		PRINTF("axpXXX_bus_init() failure.\n");
			dbg_flush();
		return ret;
	}

	if (0)
	{
		uint8_t registers [0xEE];
		unsigned reg;
		for (reg = 0; reg < ARRAY_SIZE(registers); ++ reg)
		{
			if (pmic_bus_read(reg, & registers [reg]))
				break;
		}
		PRINTF("axp318 registers (count = 0x%02X):\n", reg);
		printhex(0, registers, reg);
	}
//	ret = pmic_bus_read(AXP313_CHIP_VERSION, &axp318_chip_id);
//	if (ret)
//	{
//		PRINTF("pmic_bus_read() failure.\n");
//			dbg_flush();
//		return ret;
//	}

//	PRINTF("axp318_chip_id=0x%02X (expected 0x%02X)\n", axp318_chip_id, 0x4B);
//
//	dbg_flush();
//	if (!(axp318_chip_id == 0x4B))
//		return -1;
//    PRINTF("axp318_chip_id=OK\n");

    // Radaxa Cubie A7Z voltages:
	//	SWOUT1 VCC33-LDO 3.3V with ON/OFF
	//	SWOUT2 VCC-CARD 3.3V with ON/OFF
	//	DCDC2 VDD-SYS, VDD-DRAM, VDD-VE, ... 800 mV
	//	DCDC3 VDD-CPUB
	//	DCDC4 VDD-GPU
	//	DCDC5 VDD-CPUL
	//	DCDC6 VCC-DRAML, VDD-VDDQ [see options]
	//	DCDC7 VDD-DRAM, VCC-VDDQ [seee options]
	//	DCDC8 VCC-UFS-IO,, VCC12-UFS, VCC18-UFS
	//	DCDC9 ELDO-INPUT
	//	ALDO1 VCC-PL, VCC22-18-USB, VCC33-USB, VCC-3V3_TYPEC 3300 mV
	//	BLDO1 VCC-PE, VCC-PK, VCC-MCSI 1800 mV
	//	BLDO2 VCC-PD, VCC-LVDS0, VCC-PJ, VCC18-LCS 1800 mV
	//	BLDO4 VCC18-CODEC 1800 mV
	//	BLDO5 VCC-PG 1800 mV
	//	CLDO1 VCC-PM, ... 1800 mV
	//	CLDO2 VCC18-HDMI 1800 mV
	//	CLDO3 ... 1800 mV
	//	CLDO5 ... 1800 mV
	//	DLDO1 VCC-EFUSE 1800 mV
	//	DLDO6 VCC-UFS 2500 mV
	//	ELDO6 VDD-CPUS, VDD-USB 800 mV
	//	RTCLDO-PMU VCC-RTC
#if 0
	axpXXX_print(axp318_regulators);
#else
    axp318w_set("dcdc2", 800); // VDD-SYS, VDD-DRAM, VDD-VE, ... 800 mV
    axp318w_set("dcdc3", 900); // VDD-CPUB 0.8-1V
    axp318w_set("dcdc4", 900); // VDD-GPU 0.8V-0.96V
    axp318w_set("dcdc5", 900); // VDD-CPUL 0.8V-1V
    axp318w_set("dcdc6", 800); // VCC-DRAML, VDD-VDDQ [see options] 0.8/0.6/0.5V
    axp318w_set("dcdc7", 1100); // VDD-DRAM, VCC-VDDQ [seee options] 1.1/1.05V
    axp318w_set("dcdc8", 1200); // VCC-UFS-IO,, VCC12-UFS, VCC18-UFS 0.8/1.2/1.8V
    axp318w_set("dcdc9", 1200); // ELDO-INPUT 1.25/1.1V
    axp318w_set("aldo1", 3300); // VCC-PL, VCC22-18-USB, VCC33-USB, VCC-3V3_TYPEC 3300 mV
    axp318w_set("bldo1", 1800); // VCC-PE, VCC-PK, VCC-MCSI 1800 mV
    axp318w_set("bldo2", 1800); // VCC-PD, VCC-LVDS0, VCC-PJ, VCC18-LCS 1800 mV
    axp318w_set("bldo4", 1800); // VCC18-CODEC 1800 mV
    axp318w_set("bldo5", 1800); // VCC-PG 1800 mV
    axp318w_set("cldo1", 1800); // VCC-PM, ... 1800 mV
    axp318w_set("cldo2", 1800); // VCC18-HDMI 1800 mV
    axp318w_set("cldo3", 1800); // ... 1800 mV
    axp318w_set("cldo5", 1800); // ... 1800 mV
    axp318w_set("dldo1", 1800); // VCC-EFUSE 1800 mV
    axp318w_set("dldo6", 2500); // VCC-UFS 2500 mV
    axp318w_set("eldo6", 800); // VDD-CPUS, VDD-USB 800 mV
    axp318w_setstate("swout1", 1);
    axp318w_setstate("swout2", 1);
#endif
	//axpXXX_print(axp318_regulators);

	PRINTF("axp318 INIT END\n");
	dbg_flush();
	return 0;

}

#endif
