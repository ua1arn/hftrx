/*
 * xpt2046.h
 *
 *  Created on: Jun 18, 2021
 *      Author: gena
 */

#ifndef SRC_TOUCH_XPT2046_H_
#define SRC_TOUCH_XPT2046_H_

/*
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 */
									  
// Relies on XPT2046-compatible mode of ADS7843,
// hence no Z1 / Z2 measurements are possible.

void xpt2046_initialize(void);
uint_fast8_t xpt2046_getxy(uint_fast16_t * xr, uint_fast16_t * yr);


#endif /* SRC_TOUCH_XPT2046_H_ */
