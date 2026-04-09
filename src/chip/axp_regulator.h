/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef SRC_CHIP_AXP_REGULATOR_H_
#define SRC_CHIP_AXP_REGULATOR_H_

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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
#ifndef BIT
#define BIT(x)              (1 << (x))
#endif

int pmic_bus_read(uint8_t reg, uint8_t * data);

int axpXXX_bus_init(void);
void axpXXX_print(const struct axp_regulator_plat * regs);
int axpXXX_set_value(const struct axp_regulator_plat * regs, const char * name, unsigned mvolt);
int axpXXX_set_enable(const struct axp_regulator_plat * regs, const char * name, unsigned state);

int set_ddr_voltage_ext(char *name, int set_vol, int on);	// SysterKit callback

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SRC_CHIP_AXP_REGULATOR_H_ */
