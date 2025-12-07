/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "formats.h"

#if defined (TSC1_TYPE)

#include "touch.h"
#include "board.h"
#include "src/display/display.h"
#include "display2.h"
#include "gpio.h"

uint_fast16_t
tcsnormalize(
		uint_fast16_t raw,
		uint_fast16_t rawmin,
		uint_fast16_t rawmax,
		uint_fast16_t range
		)
{
	if (rawmin < rawmax)
	{
		// Normal direction
		const uint_fast16_t distance = rawmax - rawmin;
		if (raw < rawmin)
			return 0;
		raw = raw - rawmin;
		if (raw > distance)
			return range;
		return (uint_fast32_t) raw * range / distance;
	}
	else
	{
		// reverse direction
		const uint_fast16_t distance = rawmin - rawmax;
		if (raw >= rawmin)
			return 0;
		raw = rawmin - raw;
		if (raw > distance)
			return range;
		return (uint_fast32_t) raw * range / distance;
	}
}

#if WITHTSC5PCALIBRATE

// функции калибровки взяты тут:
// https://github.com/vadrov/stm32-xpt2046-ili9341-dma-irq-spi-temperature-voltage
/*
 * Copyright (C) 2019, VadRov, all right reserved.
 *
 *  https://www.youtube.com/@VadRov
 *  https://dzen.ru/vadrov
 *  https://vk.com/vadrov
 *  https://t.me/vadrov_channel
 *
 */

/* Коэффициенты для преобразования координат тачскрина в дисплейные координаты */
typedef struct
{
	int64_t	Dx1, Dx2, Dx3, Dy1, Dy2, Dy3, D;
} tCoef;

/*
 * Расчет коэффициентов для преобразования координат тачскрина в дисплейные координаты
 */
static void tsc_CoefCalc(const tPoint *p_d, const tPoint *p_t, tCoef *coef, int all_points)
{
	int i;
	int_fast64_t a = 0, b = 0, c = 0, d = 0, e = 0;
	int_fast64_t X1 = 0, X2 = 0, X3 = 0, Y1 = 0, Y2 = 0, Y3 = 0;
	for (i = 0; i < all_points; i ++)
	{
		a += p_t[i].x * p_t[i].x;
		b += p_t[i].y * p_t[i].y;
		c += p_t[i].x * p_t[i].y;
		d += p_t[i].x;
		e += p_t[i].y;
		X1 += p_t[i].x * p_d[i].x;
		X2 += p_t[i].y * p_d[i].x;
		X3 += p_d[i].x;
		Y1 += p_t[i].x * p_d[i].y;
		Y2 += p_t[i].y * p_d[i].y;
		Y3 += p_d[i].y;
	}
	coef->D = all_points * (a * b - c * c) + 2 * c *  d * e - a * e * e - b * d * d;
	coef->Dx1 = all_points * (X1 * b - X2 * c) + e * (X2 * d - X1 * e) + X3 * (c * e - b * d);
	coef->Dx2 = all_points * (X2 * a - X1 * c) + d * (X1 * e - X2 * d) + X3 * (c * d - a * e);
	coef->Dx3 = X3 * (a * b - c * c) + X1 * (c * e - b * d) + X2 * (c * d - a * e);
	coef->Dy1 = all_points * (Y1 * b - Y2 * c) + e * (Y2 * d - Y1 * e) + Y3 * (c * e - b * d);
	coef->Dy2 = all_points * (Y2 * a - Y1 * c) + d * (Y1 * e - Y2 * d) + Y3 * (c * d - a * e);
	coef->Dy3 = Y3 * (a * b - c * c) + Y1 * (c * e - b * d) + Y2 * (c * d -a * e);
}

static tCoef tsccoef;

/* поддержка калибровки */
const void * board_tsc_normparams(void)
{
	return & tsccoef;
}

/*
 * Преобразование координат тачскрина в дисплейные/экранные координаты:
 * - в переменной p_t (тип tPoint) принимает координаты тачскрина;
 * - в переменной coef (тип tCoef) принимает коэффициенты преобразования;
 * - в переменной p_d (тип tPoint) возвращает дисплейные координаты.
 */
static void tsc_ConvertPoint(tPoint *p_d, const tPoint *p_t, const tCoef *coef)
{
	const int_fast32_t x = (int_fast32_t) ((p_t->x * coef->Dx1 + p_t->y * coef->Dx2 + coef->Dx3) / coef->D);
	const int_fast32_t y = (int_fast32_t) ((p_t->x * coef->Dy1 + p_t->y * coef->Dy2 + coef->Dy3) / coef->D);
	p_d->x = (x < 0) ? 0 : (x >= DIM_X) ? (DIM_X - 1) : x;
	p_d->y = (y < 0) ? 0 : (y >= DIM_Y) ? (DIM_Y - 1) : y;
}

/* получение координаты нажатия в пределах 0..DIM_X-1 */
uint_fast16_t board_tsc_normalize_x(uint_fast16_t tx, uint_fast16_t ty, const void * params)
{
	const tCoef * const coef = (const tCoef *) params;
	const int_fast32_t x = (int_fast32_t) ((tx * coef->Dx1 + ty * coef->Dx2 + coef->Dx3) / coef->D);
	return (x < 0) ? 0 : (x >= DIM_X) ? (DIM_X - 1) : x;
}

/* получение координаты нажатия в пределах 0..DIM_Y-1 */
uint_fast16_t board_tsc_normalize_y(uint_fast16_t tx, uint_fast16_t ty, const void * params)
{
	const tCoef * const coef = (const tCoef *) params;
	const int_fast32_t y = (int_fast32_t) ((tx * coef->Dy1 + ty * coef->Dy2 + coef->Dy3) / coef->D);
	return (y < 0) ? 0 : (y >= DIM_Y) ? (DIM_Y - 1) : y;
}

#else /* WITHTSC5PCALIBRATE */

/* поддержка калибровки */
const void * board_tsc_normparams(void)
{
	return NULL;
}

#endif /* WITHTSC5PCALIBRATE */

// использовать результаты калибровки
void board_tsc_calibration(void)
{
#if WITHTSC5PCALIBRATE

	const uint_fast16_t xstep = DIM_X / 6;
	const uint_fast16_t ystep = DIM_Y / 6;
	tPoint p_display [TSCCALIBPOINTS];
	tPoint * const p_touch = board_tsc_getcalpoints();

	unsigned i = 0;
	p_display [i].x = xstep * 1;	// левый верхний
	p_display [i].y = ystep * 1;
	++ i;
	p_display [i].x = xstep * 3;	// центр верхний
	p_display [i].y = ystep * 1;
	++ i;
	p_display [i].x = xstep * 5;	// правый верхний
	p_display [i].y = ystep * 1;
	++ i;
	p_display [i].x = xstep * 1;	// левый средний
	p_display [i].y = ystep * 3;
	++ i;
	p_display [i].x = xstep * 3;	// центр средний
	p_display [i].y = ystep * 3;
	++ i;
	p_display [i].x = xstep * 5;	// правый средний
	p_display [i].y = ystep * 3;
	++ i;
	p_display [i].x = xstep * 1;	// левый нижний
	p_display [i].y = ystep * 5;
	++ i;
	p_display [i].x = xstep * 3;	// центр нижний
	p_display [i].y = ystep * 5;
	++ i;
	p_display [i].x = xstep * 5;	// правый нижний
	p_display [i].y = ystep * 5;
	ASSERT(TSCCALIBPOINTS == (i + 1));

	// Выполнение калибровки тач сенсора
	if (0)
	{
		enum { r0 = 15 };
		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
		gxdrawb_t dbv;	// framebuffer для выдачи диагностических сообщений


		uint_fast8_t tg;	// получение калибровочных значений для данной точки
		for (tg = 0; tg < TSCCALIBPOINTS; ++ tg)
		{
			//PRINTF("tsc: calibrate target %u\n", (unsigned) tg);

			gxdrawb_initialize(& dbv, colmain_fb_draw(), DIM_X, DIM_Y);
			// стереть фон
			colpip_fillrect(& dbv, 0, 0, DIM_X, DIM_Y, COLOR_BLACK);
			// нарисовать мишени для калибровки
			uint_fast8_t i;
			for (i = 0; i < TSCCALIBPOINTS; ++ i)
			{
				const uint_fast16_t xg = DIM_X / 32;
				const uint_fast16_t yg = DIM_Y / 20;
				colpip_line(& dbv, p_display [i].x - xg, p_display [i].y - 0, p_display [i].x + xg, p_display [i].y + 0, COLOR_WHITEALL, 0);
				colpip_line(& dbv, p_display [i].x - 0, p_display [i].y - yg, p_display [i].x + 0, p_display [i].y + yg, COLOR_WHITEALL, 0);
				if (i == tg)
				{
					colpip_segm(& dbv, p_display [i].x, p_display [i].y, 0, 360, 15, r0, COLOR_WHITEALL, 0, 0);
				}
			}
			colpip_string_tbg(& dbv, xstep * 2, ystep * 5, "CALIBRATE", COLOR_WHITEALL);
			colmain_nextfb();
			// wait answer
			unsigned as;
			for (as = 0; as < 5000; /*++ as */)
			{
				uint_fast16_t x, y, z;
				if (board_tsc_getraw(& x, & y, & z))
				{
					p_touch [tg].x = x;
					p_touch [tg].y = y;
					//PRINTF("tsc: calibrate target %u: x=%-5u, y=%-5u , z=%-5u\n", tg, x, y, z);
					PRINTF("{ %u, %u, }, /* point %u */\n", x, y, tg);
					break;
				}
				//local_delay_ms(1);
			}
			gxdrawb_initialize(& dbv, colmain_fb_draw(), DIM_X, DIM_Y);
			// стереть фон
			colpip_fillrect(& dbv, 0, 0, DIM_X, DIM_Y, COLOR_BLACK);
			colpip_string_tbg(& dbv, xstep * 2, ystep * 5, "CALIBRATE DONE", COLOR_WHITEALL);
			colmain_nextfb();
			//PRINTF("tsc: calibrate target %u done\n", (unsigned) tg);
		}

	}

	//Раcсчитываем коэффициенты для перехода от координат тачскрина в дисплейные координаты.
	tsc_CoefCalc(p_display, p_touch, & tsccoef, TSCCALIBPOINTS);
#endif /* WITHTSC5PCALIBRATE */
}

#if ! LINUX_SUBSYSTEM && defined (TSC1_TYPE) && TSC1_TYPE != TSC_TYPE_EVDEV

uint_fast8_t
board_tsc_getxy(uint_fast16_t * xr, uint_fast16_t * yr)
{
	uint_fast16_t x, y, z;
	if (board_tsc_getraw(& x, & y, & z))
	{
		const void * const param = board_tsc_normparams();	/* поддержка калибровки */

		* xr = board_tsc_normalize_x(x, y, param);
		* yr = board_tsc_normalize_y(x, y, param);
		//PRINTF("board_tsc_getxy: x=%-5u, y=%-5u -> xc=%-5u, yc=%-5u\n", x, y, * xr, * yr);
		return 1;
	}
	* xr = 0;	/* зачем ? */
	* yr = 0;	/* зачем ? */
	return 0;
}

#endif /* ! LINUX_SUBSYSTEM && defined (TSC1_TYPE) && TSC1_TYPE != TSC_TYPE_EVDEV */

#if WITHLVGL && 0

#include "lv_drivers/indev/evdev.h"

void board_tsc_indev_read(lv_indev_drv_t * drv, lv_indev_data_t * data)
{
	uint_fast16_t x, y;

	if(drv->type == LV_INDEV_TYPE_POINTER)
	{
		if (board_tsc_getxy(& x, & y))
			data->state = LV_INDEV_STATE_PR;
		else
			data->state = LV_INDEV_STATE_REL;

		data->point.x = x;
		data->point.y = y;
	}
}

#endif /* WITHLVGL */

#endif /* defined (TSC1_TYPE) */
