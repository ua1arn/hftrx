#include "hardware.h"

// TODO: move to config file

//	#define WITHSDRAM_AXP313 1
//	#define PMIC_I2C_W 0x6C  // 7bit: 0x36
//	#define PMIC_I2C_R (PMIC_I2C_W | 0x01)
//	int axp313_initialize(void)

#if WITHSDRAM_AXP313

#include "gpio.h"
#include "formats.h"

//#include "axp313.h"

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

#if WITHSDRAM_AXP313				/* AXP313 */

#define BIT(x)              (1 << (x))

#define AXP_CHIP_VERSION	0x3
#define AXP_CHIP_VERSION_MASK	0xc8
#define AXP_CHIP_ID		0x48
//#define AXP_SHUTDOWN_REG	0x1a
//#define AXP_SHUTDOWN_MASK	BIT(7)

 #else

 	#error "Please define the regulator registers in axp_spl_regulators[]."

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
///PRINTF("START PMIC \n");
	uint8_t axp313_chip_id;
	int ret;

	ret = pmic_bus_init();
	if (ret)
		return ret;

	ret = pmic_bus_read(AXP313_CHIP_VERSION, &axp313_chip_id);
	if (ret)
		return ret;

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

	PRINTF("axp313_chip_id=0x%02X (expected 0x4B)\n", axp313_chip_id);
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
		return ret;

	ret = pmic_bus_read(AXP313_CHIP_VERSION, &axp313_chip_id);
	if (ret)
		return ret;

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

	PRINTF("axp313_chip_id=0x%02X (expected 0x4B)\n", axp313_chip_id);
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

#endif
