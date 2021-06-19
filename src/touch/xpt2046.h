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

// https://github.com/MarlinFirmware/Marlin/blob/2.0.x/Marlin/src/lcd/touch/touch_buttons.cpp

#define XPT2046_DFR_MODE 0x00
#define XPT2046_SER_MODE 0x04
#define XPT2046_CONTROL  0x80
#define XPT2046_PD0  	0x01	// full-power (PD0 = 1)
#define XPT2046_PD1  	0x02	

enum XPTCoordinate {
  XPT2046_X  = 1 * 0x10,	// Длинная сторона на 320x240
  XPT2046_Y  = 5 * 0x10,	// Короткая сторона на 320x240
  XPT2046_Z1 = 3 * 0x10,
  XPT2046_Z2 = 4 * 0x10
};

#ifndef XPT2046_Z1_THRESHOLD
  #define XPT2046_Z1_THRESHOLD 10
#endif

void xpt2046_initialize(void);
uint_fast8_t xpt2046_getxy(uint_fast16_t * xr, uint_fast16_t * yr);


#endif /* SRC_TOUCH_XPT2046_H_ */
