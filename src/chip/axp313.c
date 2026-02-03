#include "hardware.h"

// TODO: move to config file

//	#define WITHSDRAM_AXP313 1
//	#define PMIC_I2C_W 0x6C  // 7bit: 0x36
//	#define PMIC_I2C_R (PMIC_I2C_W | 0x01)
//	int axp313_initialize(void)

#if WITHSDRAM_AXP313 || WITHSDRAM_AXP318

#include "gpio.h"
#include "formats.h"
#include <string.h>

//#include "axp313.h"

#define NA 0xff

struct axp_regulator_plat {
	const char	*name;
	uint8_t		enable_reg;
	uint8_t		enable_mask;
	uint8_t		volt_reg;
	uint8_t		volt_mask;
	uint16_t		min_mV;
	uint16_t		max_mV;
	uint8_t		step_mV;
	uint8_t		split;
	const uint16_t	*table;
};

#include <errno.h>

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
#define BIT(x)              (1 << (x))

#if WITHSDRAM_AXP313				/* AXP313 */


#define AXP_CHIP_VERSION	0x3
#define AXP_CHIP_VERSION_MASK	0xc8
#define AXP_CHIP_ID		0x48
//#define AXP_SHUTDOWN_REG	0x1a
//#define AXP_SHUTDOWN_MASK	BIT(7)

 #else

 	//#error "Please define the regulator registers in axp_spl_regulators[]."

 #endif

static i2cp_t pmic_i2cp;	/* ��������� ��� ������ �� I2C. ��������� �������� �� ������������� ����-������, ���� ��������� ��������� ������ ���������� */

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
	uint8_t bufw = reg;
	return i2chwx_exchange(TWIHARD_S_PTR, PMIC_I2C_W, & bufw, 1, data, 1);
}

static int pmic_bus_write(uint8_t reg, uint8_t data)
{
	uint8_t bufw [] = { reg, data };
	return i2chwx_write(TWIHARD_S_PTR, PMIC_I2C_W, bufw, ARRAY_SIZE(bufw));
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
///-------------


static uint8_t mvolt_to_cfg(int mvolt, int min, int max, int div)
{
	if (mvolt < min)
		mvolt = min;
	else if (mvolt > max)
		mvolt = max;

	return (mvolt - min) / div;
}

static int axp_set_dcdc(int dcdc_num, unsigned int mvolt)
{
	int ret;
	uint8_t cfg, enable_mask = 1U << (dcdc_num - 1);
	int volt_reg = AXP313_DCDC1_CTRL + dcdc_num - 1;
	int max_mV;

	switch (dcdc_num) {
	case 1:
	case 2:
		max_mV	= 1540;
		break;
	case 3:
		/*
		 * The manual defines a different split point, but tests
		 * show that it's the same 1200mV as for DCDC1/2.
		 */
		max_mV	= 1840;
		break;
	default:
		return -EINVAL;
	}

	if (mvolt > AXP313_DCDC_SPLIT_MVOLT)
		cfg = AXP313_DCDC_SPLIT_OFFSET + mvolt_to_cfg(mvolt,
				AXP313_DCDC_SPLIT_MVOLT + 20, max_mV, 20);
	else
		cfg = mvolt_to_cfg(mvolt, 500, AXP313_DCDC_SPLIT_MVOLT, 10);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP313_OUTPUT_CTRL, enable_mask);

	PRINTF("DCDC%d: writing 0x%x to reg 0x%x\n", dcdc_num, cfg, volt_reg);
	ret = pmic_bus_write(volt_reg, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP313_OUTPUT_CTRL, enable_mask);
}

//static int do_poweroff(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
//{
//	pmic_bus_write(AXP313_SHUTDOWN, AXP313_POWEROFF);
//
//	/* infinite loop during shutdown */
//	while (1) {}
//
//	/* not reached */
//	return 0;
//}

///-------------
static uint8_t axp313a_mvolt_to_cfg(int mvolt, int min, int max, int div)
{
	if (mvolt < min)
		mvolt = min;
	else if (mvolt > max)
		mvolt = max;

	return (mvolt - min) / div;
}

static int axp_set_dcdc1(unsigned int mvolt)
{
	int ret;
	uint8_t cfg;

	if (mvolt >= 1600)
		cfg = 88 + axp313a_mvolt_to_cfg(mvolt, 1600, 3400, 100);
	else if (mvolt >= 1220)
		cfg = 71 + axp313a_mvolt_to_cfg(mvolt, 1220, 1540, 20);
	else
		cfg = axp313a_mvolt_to_cfg(mvolt, 500, 1200, 10);


	if (mvolt == 0)
		return pmic_bus_clrbits(AXP313A_OUTPUT_CTRL,
					AXP313A_OUTPUT_CTRL_DCDC1);

	ret = pmic_bus_write(AXP313A_DCDC1_CTRL, cfg);

	if (ret)
		return ret;

	return pmic_bus_setbits(AXP313A_OUTPUT_CTRL,
				AXP313A_OUTPUT_CTRL_DCDC1);
}

static int axp_set_dcdc2(unsigned int mvolt)
{
	int ret;
	uint8_t cfg;

	if (mvolt >= 1220)
		cfg = 71 + axp313a_mvolt_to_cfg(mvolt, 1220, 1540, 20);
	else
		cfg = axp313a_mvolt_to_cfg(mvolt, 500, 1200, 10);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP313A_OUTPUT_CTRL,
					AXP313A_OUTPUT_CTRL_DCDC2);

	ret = pmic_bus_write(AXP313A_DCDC2_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP313A_OUTPUT_CTRL,
				AXP313A_OUTPUT_CTRL_DCDC2);
}

static int axp_set_dcdc3(unsigned int mvolt)
{
	int ret;
	uint8_t cfg;

	if (mvolt >= 1220)
		cfg = 71 + axp313a_mvolt_to_cfg(mvolt, 1220, 1840, 20);
	else
		cfg = axp313a_mvolt_to_cfg(mvolt, 500, 1200, 10);

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP313A_OUTPUT_CTRL,
					AXP313A_OUTPUT_CTRL_DCDC3);

	ret = pmic_bus_write(AXP313A_DCDC3_CTRL, cfg);
	if (ret)
		return ret;

	return pmic_bus_setbits(AXP313A_OUTPUT_CTRL,
				AXP313A_OUTPUT_CTRL_DCDC3);
}

static int axp_set_aldo1(unsigned int mvolt)
{
	int ret;
	uint8_t cfg = axp313a_mvolt_to_cfg(mvolt, 500, 3500, 100);


	if (mvolt == 0)
		return pmic_bus_clrbits(AXP313A_OUTPUT_CTRL,
					AXP313A_OUTPUT_CTRL_ALDO1);


	ret = pmic_bus_write(AXP313A_ALDO1_CTRL, cfg);

	if (cfg)
		return ret;

	return pmic_bus_setbits(AXP313A_OUTPUT_CTRL,
				AXP313A_OUTPUT_CTRL_ALDO1);


}

static int axp_set_dldo1(int dldo_num, unsigned int mvolt)
{
	int ret;
	uint8_t cfg = axp313a_mvolt_to_cfg(mvolt, 500, 3500, 100);

	if (dldo_num != 1)
		return -EINVAL;

	if (mvolt == 0)
		return pmic_bus_clrbits(AXP313A_OUTPUT_CTRL,
					AXP313A_OUTPUT_CTRL_DLDO1);


	ret = pmic_bus_write(AXP313A_DLDO1_CTRL, cfg);

	if (cfg)
		return ret;

	return pmic_bus_setbits(AXP313A_OUTPUT_CTRL,
				AXP313A_OUTPUT_CTRL_DLDO1);
}
///-------------

int board_orangepi_zero2w_axp313_initialize(void)
{
	PRINTF("START PMIC \n");
	uint8_t axp313_chip_id;
	int ret;

	ret = pmic_bus_init();
	if (ret)
	{
		PRINTF("pmic_bus_init() failure.\n");
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
		unsigned reg;
		for (reg = 0; reg <= 0xED; ++ reg)
		{
			uint8_t v;
			pmic_bus_read(reg, & v);
			PRINTF("axp313 reg%02X=0x%02X\n", reg, v);
		}
	}

	PRINTF("axp313_chip_id=0x%02X (expected 0x%02X)\n", axp313_chip_id, 0x4B);

	dbg_flush();
	if (!(axp313_chip_id == 0x4B))
		return -1;
    PRINTF("axp313_chip_id=OK\n");

	axp_set_aldo1(1800);///VCC 1V8
	axp_set_dldo1(1,3300);///VCC3V3
	axp_set_dcdc1(970);///810-990 VDD-GPU-SYS
	axp_set_dcdc2(970);///810-1100 VDD-CPU
	axp_set_dcdc3(1100);///VCC-DRAM - 1.1V for LPDDR4

	PRINTF("axp313 INIT END\n");
	dbg_flush();
	return 0;
}

int board_orangepi_zero3_axp313_initialize(void)
{
	PRINTF("START PMIC \n");
	uint8_t axp313_chip_id;
	int ret;

	ret = pmic_bus_init();
	if (ret)
	{
		PRINTF("pmic_bus_init() failure.\n");
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

	axp_set_aldo1(1800);///VCC 1V8
	axp_set_dldo1(1,3300);///VCC3V3
	axp_set_dcdc1(970);///810-990 VDD-GPU-SYS
	axp_set_dcdc2(970);///810-1100 VDD-CPU
	axp_set_dcdc3(1100);///VCC-DRAM - 1.1V for LPDDR4

	PRINTF("axp313 INIT END\n");
	dbg_flush();
	return 0;
}


//void read_reg(void)
//{
//uint8_t data_read;
//pmic_bus_read(AXP313A_DCDC1_CTRL, &data_read);
//PRINTF(" reg=%p data=%p\n",AXP313A_DCDC1_CTRL,data_read);
//pmic_bus_read(AXP313A_DCDC2_CTRL, &data_read);
//PRINTF(" reg=%p data=%p\n",AXP313A_DCDC2_CTRL,data_read);
//pmic_bus_read(AXP313A_DCDC3_CTRL, &data_read);
//PRINTF(" reg=%p data=%p\n",AXP313A_DCDC3_CTRL,data_read);
//pmic_bus_read(AXP313A_ALDO1_CTRL, &data_read);
//PRINTF(" reg=%p data=%p\n",AXP313A_ALDO1_CTRL,data_read);
//pmic_bus_read(AXP313A_DLDO1_CTRL, &data_read);
//PRINTF(" reg=%p data=%p\n",AXP313A_DLDO1_CTRL,data_read);
//
//}

// https://github.com/u-boot/u-boot/blob/1de103fc29761fa729dffaa15d0cfb2766be05e4/drivers/power/regulator/axp_regulator.c#L198

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

static int axpXXX_set(const struct axp_regulator_plat * regs, const char * name, unsigned voltage)
{
	for (;; ++ regs)
	{
		if (regs->name == NULL)
			return - 1;
		if (strcmp(name, regs->name))
			continue;
		PRINTF("Found %s for set %u mV\n", name, voltage);
	}
	PRINTF("axpXXX_set: %s not found\n", name);
	return - 1;
}
static int axp318w_set(const char * name, unsigned voltage)
{
	return axpXXX_set(axp318_regulators, name, voltage);
}

int board_radaxa_cubie_axp318w_initialize(void)
{

	PRINTF("START PMIC \n");
	uint8_t axp318_chip_id;
	int ret;

	ret = pmic_bus_init();
	if (ret)
	{
		PRINTF("pmic_bus_init() failure.\n");
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

	PRINTF("axp318 INIT END\n");
	dbg_flush();
	return 0;

}

#endif
