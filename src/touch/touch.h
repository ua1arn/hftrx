#ifndef TOUCH_H_INCLUDED
#define TOUCH_H_INCLUDED

#include "hardware.h"

#if defined (TSC1_TYPE)

void board_tsc_initialize(void);
uint_fast8_t board_tsc_getxy(uint_fast16_t * x, uint_fast16_t * y);	/* touch screen interface */
uint_fast8_t board_tsc_is_pressed (void); 	/* Return 1 if touch detection */

#endif /* defined (TSC1_TYPE) */

#endif /* TOUCH_H_INCLUDED */
