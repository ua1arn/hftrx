#ifndef TOUCH_H_INCLUDED
#define TOUCH_H_INCLUDED

#include "hardware.h"

#ifdef __cplusplus
extern "C" {
#endif

void board_tsc_initialize(void);
uint_fast8_t board_tsc_getxy(uint_fast16_t * x, uint_fast16_t * y);	/* touch screen interface */
uint_fast8_t board_tsc_getraw(uint_fast16_t * xr, uint_fast16_t * yr, uint_fast16_t * zr);	/* touch screen interface */
uint_fast16_t board_tsc_normalize_x(uint_fast16_t x, uint_fast16_t y, const void * params);	/* получение координаты нажатия в пределах 0..DIM_X-1 */
uint_fast16_t board_tsc_normalize_y(uint_fast16_t x, uint_fast16_t y, const void * params);	/* получение координаты нажатия в пределах 0..DIM_Y-1 */

#ifdef __cplusplus
 }
#endif

#endif /* TOUCH_H_INCLUDED */
