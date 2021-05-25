#include "hardware.h"

#define SOFTW_I2C_ADDR	8

enum {
	SOFTTSC_INIT = 1,
	SOFTTSC_READ
};

uint_fast8_t softtsc_getXY(uint_fast16_t * xr, uint_fast16_t * yr);
uint_fast8_t softtsc_initialize(void);
