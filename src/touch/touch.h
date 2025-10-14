#ifndef TOUCH_H_INCLUDED
#define TOUCH_H_INCLUDED

#include "hardware.h"

#ifdef __cplusplus
extern "C" {
#endif


enum { TSCCALIBPOINTS = 5 };
/* Данные с координатами точки касания */
typedef struct
{
	int x, y;
} tPoint;

void board_tsc_initialize(void);	/* вызывается при разрешённых прерываниях. */
void board_tsc_calibration(void);	/* использовать результаты калибровки */
uint_fast8_t board_tsc_getxy(uint_fast16_t * x, uint_fast16_t * y);	/* touch screen interface */
uint_fast8_t board_tsc_getraw(uint_fast16_t * xr, uint_fast16_t * yr, uint_fast16_t * zr);	/* touch screen interface */
uint_fast16_t board_tsc_normalize_x(uint_fast16_t x, uint_fast16_t y, const void * params);	/* получение координаты нажатия в пределах 0..DIM_X-1 */
uint_fast16_t board_tsc_normalize_y(uint_fast16_t x, uint_fast16_t y, const void * params);	/* получение координаты нажатия в пределах 0..DIM_Y-1 */
tPoint * board_tsc_getcalpoints(void);	/* поддержка калибровки */
const void * board_tsc_normparams(void);	/* поддержка калибровки */

#ifdef __cplusplus
 }
#endif

#endif /* TOUCH_H_INCLUDED */
