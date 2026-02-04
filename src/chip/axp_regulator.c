/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "axp_regulator.h"
#include "formats.h"
#include <string.h>

#if WITHTWIHW

int pmic_bus_read(uint8_t reg, uint8_t * data)
{
	uint8_t bufw = reg;
	return i2chwx_exchange(TWIHARD_S_PTR, PMIC_I2C_W, & bufw, 1, data, 1);
}

static int pmic_read(uint8_t reg, uint8_t * data)
{
	uint8_t bufw = reg;
	return i2chwx_exchange(TWIHARD_S_PTR, PMIC_I2C_W, & bufw, 1, data, 1);
}

static int pmic_bus_write(uint8_t reg, uint8_t data)
{
	const uint8_t bufw [] = { reg, data };
	return i2chwx_write(TWIHARD_S_PTR, PMIC_I2C_W, bufw, ARRAY_SIZE(bufw));
}

static int pmic_write(uint8_t reg, uint8_t data)
{
	const uint8_t bufw [] = { reg, data };
	return i2chwx_write(TWIHARD_S_PTR, PMIC_I2C_W, bufw, ARRAY_SIZE(bufw));
}

static int pmic_reg_read(uint8_t reg)
{
	const uint8_t bufw = reg;
	uint8_t rv;
	int ec = i2chwx_exchange(TWIHARD_S_PTR, PMIC_I2C_W | 0x01, & bufw, 1, & rv, 1);
	return (ec < 0) ? ec : rv;
}

static int pmic_reg_write(uint8_t reg, uint8_t data)
{
	const uint8_t bufw [] = { reg, data };
	return i2chwx_write(TWIHARD_S_PTR, PMIC_I2C_W & ~ 0x01, bufw, ARRAY_SIZE(bufw));
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


int pmic_clrsetbits(unsigned reg, unsigned clr, unsigned set)
{
	//struct uc_pmic_priv *priv = dev_get_uclass_priv(dev);
	uint8_t val = 0;
	int ret;

//	if (priv->trans_len < 1 || priv->trans_len > sizeof(val)) {
//		PRINTF("Wrong transmission size [%d]\n", priv->trans_len);
//		return -EINVAL;
//	}
	ret = pmic_read(reg, &val);
	if (ret < 0)
		return ret;

	val = (val & ~clr) | set;
	return pmic_write(reg, val);
}


int axp_regulator_get_value(const struct axp_regulator_plat *plat)
{
	int mV, sel;

	if (plat->volt_reg == NA)
		return - 1;

	sel = pmic_reg_read(plat->volt_reg);
	if (sel < 0)
		return sel;

	sel &= plat->volt_mask;
	sel >>= ffs(plat->volt_mask) - 1;

	if (plat->table) {
		mV = plat->table[sel];
	} else {
		if (sel > plat->split)
			sel = plat->split + (sel - plat->split) * 2;
		mV = plat->min_mV + sel * plat->step_mV;
	}

	return mV * 1000;
}

int axp_regulator_set_value(const struct axp_regulator_plat *plat, int uV)
{
	int mV = uV / 1000;
	unsigned sel, shift;

	if (plat->volt_reg == NA)
		return - 1;
	if (mV < plat->min_mV || mV > plat->max_mV)
		return - 1;

	shift = ffs(plat->volt_mask) - 1;

	if (plat->table) {
		/*
		 * The table must be monotonically increasing and
		 * have an entry for each possible field value.
		 */
		sel = plat->volt_mask >> shift;
		while (sel && plat->table[sel] > mV)
			sel--;
	} else {
		sel = (mV - plat->min_mV) / plat->step_mV;
		if (sel > plat->split)
			sel = plat->split + (sel - plat->split) / 2;
	}

	return pmic_clrsetbits(plat->volt_reg,
			       plat->volt_mask, sel << shift);
}

int axp_regulator_get_enable(const struct axp_regulator_plat *plat)
{
	int reg;

	reg = pmic_reg_read(plat->enable_reg);
	if (reg < 0)
		return reg;

	return (reg & plat->enable_mask) == plat->enable_mask;
}

int axp_regulator_set_enable(const struct axp_regulator_plat *plat, int enable)
{
	return pmic_clrsetbits(plat->enable_reg,
			       plat->enable_mask,
			       enable ? plat->enable_mask : 0);
}

int axpXXX_set_value(const struct axp_regulator_plat * regs, const char * name, unsigned mvolt)
{
	for (;; ++ regs)
	{
		if (regs->name == NULL)
			break;
		if (strcmp(name, regs->name))
			continue;
		return axp_regulator_set_value(regs, mvolt * 1000);
	}
	PRINTF("axpXXX_set_enable: %s not found\n", name);
	return - 1;
}

int axpXXX_set_enable(const struct axp_regulator_plat * regs, const char * name, unsigned state)
{
	for (;; ++ regs)
	{
		if (regs->name == NULL)
			break;
		if (strcmp(name, regs->name))
			continue;
		return axp_regulator_set_enable(regs, state);
	}
	PRINTF("axpXXX_set_enable: %s not found\n", name);
	return - 1;
}


void axpXXX_print(const struct axp_regulator_plat * regs)
{
	for (;; ++ regs)
	{
		if (regs->name == NULL)
			break;

		int en = axp_regulator_get_enable(regs);
		int voltage = axp_regulator_get_value(regs);

		PRINTF("'%s': %s %d mV\n", regs->name, (en & regs->enable_mask) ? "On" : "Off", voltage / 1000);
	}
}

int axpXXX_bus_init(void)
{
	i2c_initialize();
	return 0;
}

#endif /* WITHTWIHW */
