/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "audio.h"

#include "display/display.h"
#include "formats.h"

#include <string.h>
#include <math.h>
#include "gui.h"
#include "touch/touch.h"

#if (DIM_X < GUIMINX || DIM_Y < GUIMINY) && WITHTOUCHGUI	// не соблюдены минимальные требования к разрешению экрана
#undef WITHTOUCHGUI											// для функционирования touch GUI
#endif

#if LCDMODE_LTDC

#include "keyboard.h"
#include "./display/fontmaps.h"

/*-----------------------------------------------------  V_Bre
 * void V_Bre (int xn, int yn, int xk, int yk)
 *
 * Подпрограмма иллюстрирующая построение вектора из точки
 * (xn,yn) в точку (xk, yk) методом Брезенхема.
 *
 * Построение ведется от точки с меньшими  координатами
 * к точке с большими координатами с единичным шагом по
 * координате с большим приращением.
 *
 * В общем случае исходный вектор проходит не через вершины
 * растровой сетки, а пересекает ее стороны.
 * Пусть приращение по X больше приращения по Y и оба они > 0.
 * Для очередного значения X нужно выбрать одну двух ближайших
 * координат сетки по Y.
 * Для этого проверяется как проходит  исходный  вектор - выше
 * или ниже середины расстояния между ближайшими значениями Y.
 * Если выше середины,  то Y-координату  надо  увеличить на 1,
 * иначе оставить прежней.
 * Для этой проверки анализируется знак переменной s,
 * соответствующей разности между истинным положением и
 * серединой расстояния между ближайшими Y-узлами сетки.
 */

void colmain_line(
	PACKEDCOLORMAIN_T * buffer,
	uint_fast16_t bx,	// ширина буфера
	uint_fast16_t by,	// высота буфера
	int xn, int yn,
	int xk, int yk,
	COLORMAIN_T color,
	int antialiasing
	)
{
	int  dx, dy, s, sx, sy, kl, incr1, incr2;
	char swap;

	/* Вычисление приращений и шагов */
	if ((dx = xk - xn) < 0)
	{
		dx = - dx;
		sx = - 1;
	}
	else if (dx > 0)
		sx = + 1;
	else
		sx = 0;

	if ((dy = yk - yn) < 0)
	{
		dy = - dy;
		sy = - 1;
	}
	else if (dy > 0)
		sy = + 1;
	else
		sy = 0;

	/* Учет наклона */
	if ((kl = dx) < (s = dy))
	{
		dx = s;  dy = kl;  kl = s; swap = 1;
	}
	else
	{
		swap = 0;
	}

	s = (incr1 = 2 * dy) - dx; /* incr1 - констан. перевычисления */
	/* разности если текущее s < 0  и  */
	/* s - начальное значение разности */
	incr2 = 2 * dx;         /* Константа для перевычисления    */
	/* разности если текущее s >= 0    */
	colmain_putpixel(buffer, bx, by, xn, yn, color); /* Первый  пиксел вектора       */

	static uint_fast16_t xold, yold;
	xold = xn;
	yold = yn;
	while (-- kl >= 0)
	{
		if (s >= 0)
		{
			if (swap)
				xn += sx;
			else
				yn += sy;
			s-= incr2;
		}
		if (swap)
			yn += sy;
		else
			xn += sx;
		s += incr1;
		colmain_putpixel(buffer, bx, by, xn, yn, color); /* Текущая  точка  вектора   */

		if (antialiasing)
		{
			if (((xold == xn - 1) || (xold == xn + 1)) && ((yold == yn - 1) || (yold == yn + 1)))
			{
				PACKEDCOLORMAIN_T * a;
				a = colmain_mem_at(buffer, bx, by, xn, yold);
				* a = color | COLORPIP_SHADED;
				a = colmain_mem_at(buffer, bx, by, xold, yn);
				* a = color | COLORPIP_SHADED;
//				a = colmain_mem_at(buffer, bx, by, xn, yn);		// нужны дополнительные цвета для этих 2х точек
//				* a = color | COLORPIP_SHADED;
//				a = colmain_mem_at(buffer, bx, by, xold, yold);
//				* a = color | COLORPIP_SHADED;
			}
			xold = xn;
			yold = yn;
		}
	}
}  /* V_Bre */

const int sin90 [91] =
{
	0, 175, 349, 523, 698, 872,1045,1219,1392,   /*  0..8        */
	1564,1736,1908,2079,2250,2419,2588,2756,2924,   /*  9..17       */
	3090,3256,3420,3584,3746,3907,4067,4226,4384,   /* 18..26       */
	4540,4695,4848,5000,5150,5299,5446,5592,5736,
	5878,6018,6157,6293,6428,6561,6691,6820,6947,
	7071,7193,7314,7431,7547,7660,7771,7880,7986,
	8090,8192,8290,8387,8480,8572,8660,8746,8829,
	8910,8988,9063,9135,9205,9272,9336,9397,9455,
	9511,9563,9613,9659,9703,9744,9781,9816,9848,
	9877,9903,9925,9945,9962,9976,9986,9994,9998,   /* 81..89       */
	10000                                           /* 90           */
};


static int muldiv(int a, int b, unsigned c)
{
	return  (unsigned) ((a * (long) b + (c / 2)) / c);
}

static int isin(unsigned alpha, unsigned r)
{
	while (alpha >= 360)
		alpha -= 360;

	if (alpha < 90)         /* 0..3 hours   */
		return muldiv(sin90 [ alpha ], r, 10000);
	if (alpha < 180)        /* 9..0 hours   */
		return muldiv(sin90 [ 180 - alpha ], r, 10000);
	if (alpha < 270)        /* 6..9 hours   */
		return - muldiv(sin90 [ alpha - 180], r, 10000);
				/* 3..6 hours   */
	return - muldiv(sin90 [ 360 - alpha ], r, 10000);
}

static  int icos(unsigned alpha, unsigned r)
{
	return isin(alpha + 90, r * 20 / 10);
}


// Рисование радиусов
void
display_radius_buf(
		PACKEDCOLORMAIN_T * buffer,
		uint_fast16_t bx,	// ширина буфера
		uint_fast16_t by,	// высота буфера
		int xc, int yc,
		unsigned gs,
		unsigned r1, unsigned r2,
		COLORMAIN_T color,
		int antialiasing)
{
	int     x, y;
	int     x2, y2;

	x = xc + icos(gs, r1);
	y = yc + isin(gs, r1);
	x2 = xc + icos(gs, r2);
	y2 = yc + isin(gs, r2);

	colmain_line(buffer, bx, by, x, y, x2, y2, color, antialiasing);

}

void polar_to_dek(uint_fast16_t xc, uint_fast16_t yc, uint_fast16_t gs, uint_fast16_t r, uint_fast16_t * x, uint_fast16_t * y)
{
	* x = xc + icos(gs, r);
	* y = yc + isin(gs, r);
}

// круговой интерполятор
// нач.-x, нач.-y, градус начала, градус конуа, радиус, шаг приращения угла
void
display_segm_buf(
		PACKEDCOLORMAIN_T * buffer,
		uint_fast16_t bx,	// ширина буфера
		uint_fast16_t by,	// высота буфера
		int xc, int yc,
		unsigned gs, unsigned ge,
		unsigned r, int step,
		COLORMAIN_T color,
		int antialiasing)
{
	int     x, y;
	int     xo, yo;
	char     first;
	int     vcos, vsin;

	if (gs == ge)   return;
	first = 1;
	while (gs != ge)
	{
		vsin = isin(gs, r);
		vcos = icos(gs, r);
		x = xc + vcos;
		y = yc + vsin;

		if (first != 0) // 1-я точка
		{
			// переместить к началу рисования
			xo = x, yo = y;
			first = 0;
		}
		else
		{  // рисовать элемент окружности
			colmain_line(buffer, bx, by, xo, yo, x, y, color, antialiasing);
			xo = x, yo = y;
		}
		if (ge == 360)
			ge = 0;
		if (step < 0)
		{
			gs += step;
			if (gs >= 360)
				gs += 360;
		}
		else
		{
			gs += step;
			if (gs >= 360)
				gs -= 360;
		}
	}

	if (first == 0)
	{
		// завершение окружности
		vsin = isin(ge, r);
		vcos = icos(ge, r);
		x = xc + vcos;
		y = yc + vsin;

		colmain_line(buffer, bx, by, xo, yo, x, y, color, antialiasing); // рисовать линию
	}

}

#endif /* LCDMODE_LTDC */

#if LCDMODE_LTDC

#include "./display/fontmaps.h"

static
uint_fast16_t normalize(
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

static
uint_fast16_t normalize3(
	uint_fast16_t raw,
	uint_fast16_t rawmin,
	uint_fast16_t rawmid,
	uint_fast16_t rawmax,
	uint_fast16_t range1,
	uint_fast16_t range2
	)
{
	if (raw < rawmid)
		return normalize(raw, rawmin, rawmid, range1);
	else
		return normalize(raw - rawmid, 0, rawmax - rawmid, range2 - range1) + range1;
}

enum {
	SM_STATE_RX,
	SM_STATE_TX,
	SM_STATE_COUNT
};
enum { SM_BG_W = 240, SM_BG_H = 70 };
typedef ALIGNX_BEGIN PACKEDCOLORMAIN_T smeter_bg_t [GXSIZE(SM_BG_W, SM_BG_H)] ALIGNX_END;
static smeter_bg_t smeter_bg[SM_STATE_COUNT]; 	// 0 - rx, 1 - tx

typedef struct {
	uint_fast16_t gs;
	uint_fast16_t gm;
	uint_fast16_t ge;
	uint_fast16_t r1;
	uint_fast16_t r2;
	uint_fast16_t rv1;
	uint_fast16_t rv2;
} smeter_params_t;
static smeter_params_t smeter_params;

void
display2_smeter15_init(
	uint_fast8_t xgrid,
	uint_fast8_t ygrid,
	dctx_t * pctx
	)
{
	const uint_fast8_t halfsect = 30;

	smeter_params.gm = 270;
	smeter_params.gs = smeter_params.gm - halfsect;
	smeter_params.ge = smeter_params.gm + halfsect;
	smeter_params.rv1 = 7 * GRID2Y(3);
	smeter_params.rv2 = smeter_params.rv1 - 3 * GRID2Y(3);

	const int stripewidth = 12; //16;
	smeter_params.r1 = 7 * GRID2Y(3) - 8;	//350;
	smeter_params.r2 = smeter_params.r1 - stripewidth;

	enum { step1 = 3 };		// шаг для оцифровки S
	enum { step2 = 4 };		// шаг для оцифровки плюсов
	enum { step3 = 20 };	// swr
	const int markersTX_pwr [] =
	{
		smeter_params.gs,
		smeter_params.gs + 2 * step1,
		smeter_params.gs + 4 * step1,
		smeter_params.gs + 6 * step1,
		smeter_params.gs + 8 * step1,
		smeter_params.gs + 10 * step1,
		smeter_params.gs + 12 * step1,
		smeter_params.gs + 14 * step1,
		smeter_params.gs + 16 * step1,
		smeter_params.gs + 18 * step1,
		smeter_params.gs + 20 * step1,
	};
	const int markersTX_swr [] =
	{
		smeter_params.gs,
		smeter_params.gs + step3,
		smeter_params.gs + 2 * step3,
		smeter_params.gs + 3 * step3,
	};
	const int markers [] =
	{
		//smeter_params.gs + 0 * step1,
		smeter_params.gs + 2 * step1,		// S1
		smeter_params.gs + 4 * step1,		// S3
		smeter_params.gs + 6 * step1,		// S5
		smeter_params.gs + 8 * step1,		// S7
		smeter_params.gs + 10 * step1,	// S9
	};
	const int markersR [] =
	{
		smeter_params.gm + 2 * step2,	//
		smeter_params.gm + 4 * step2,
		smeter_params.gm + 6 * step2,
	};
	const int markers2 [] =
	{
		//smeter_params.gs + 1 * step1,
		smeter_params.gs + 3 * step1,		// S2
		smeter_params.gs + 5 * step1,		// S4
		smeter_params.gs + 7 * step1,		// S6
		smeter_params.gs + 9 * step1,		// S8
	};
	const int markers2R [] =
	{
		smeter_params.gm + 1 * step2,
		smeter_params.gm + 3 * step2,
		smeter_params.gm + 5 * step2,
	};

	const COLORMAIN_T smeter = COLORMAIN_WHITE;
	const COLORMAIN_T smeterplus = COLORMAIN_DARKRED;
	const uint_fast16_t pad2w3 = strwidth3("ZZ");

	PACKEDCOLORMAIN_T * bg = smeter_bg [SM_STATE_TX];
	uint_fast8_t xb = 120, yb = 120;
	unsigned p;
	unsigned i;

	colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H - 1, COLORMAIN_BLACK, 1);

	for (p = 0, i = 0; i < ARRAY_SIZE(markersTX_pwr); ++ i, p += 10)
	{
		if (i % 2 == 0)
		{
			char buf [10];
			uint_fast16_t xx, yy;

			display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markersTX_pwr [i], smeter_params.r1, smeter_params.r1 + 8, smeter, 1);
			polar_to_dek(xb, yb, markersTX_pwr [i], smeter_params.r1 + 8, & xx, & yy);
			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%u"), p);

			colmain_setcolors(COLORMAIN_YELLOW, COLORMAIN_BLACK);
			colmain_string3_at_xy(bg, SM_BG_W, SM_BG_H, xx - strwidth3(buf) / 2, yy - pad2w3 + 1, buf);
		}
		else
			display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markersTX_pwr [i], smeter_params.r1, smeter_params.r1 + 4, smeter, 1);
	}

	for (p = 1, i = 0; i < ARRAY_SIZE(markersTX_swr); ++ i, p += 1)
	{
		char buf [10];
		uint_fast16_t xx, yy;

		display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markersTX_swr [i], smeter_params.r2, smeter_params.r2 - 8, smeter, 1);
		polar_to_dek(xb, yb, markersTX_swr [i], smeter_params.r2 - 16, & xx, & yy);
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%u"), p);

		colmain_setcolors(COLORMAIN_YELLOW, COLORMAIN_BLACK);
		colmain_string3_at_xy(bg, SM_BG_W, SM_BG_H, xx - SMALLCHARW3 / 2, yy - SMALLCHARW3 / 2 + 1, buf);
	}

	display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smeter_params.gs, smeter_params.gm, smeter_params.r1, 1, smeter, 1);
	display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smeter_params.gm, smeter_params.ge, smeter_params.r1, 1, smeter, 1);
	display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smeter_params.gs, smeter_params.ge, smeter_params.r2, 1, COLORMAIN_WHITE, 1);

	bg = smeter_bg [SM_STATE_RX];
	colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H - 1, COLORMAIN_BLACK, 1);

	for (p = 1, i = 0; i < ARRAY_SIZE(markers); ++ i, p += 2)
	{
		char buf [10];
		uint_fast16_t xx, yy;

		display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markers [i], smeter_params.r1, smeter_params.r1 + 8, smeter, 1);
		polar_to_dek(xb, yb, markers [i], smeter_params.r1 + 8, & xx, & yy);
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%u"), p);

		colmain_setcolors(COLORMAIN_YELLOW, COLORMAIN_BLACK);
		colmain_string3_at_xy(bg, SM_BG_W, SM_BG_H, xx - SMALLCHARW3 / 2, yy - pad2w3 + 1, buf);
	}
	for (i = 0; i < ARRAY_SIZE(markers2); ++ i)
	{
		display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markers2 [i], smeter_params.r1, smeter_params.r1 + 4, smeter, 1);
	}

	for (p = 20, i = 0; i < ARRAY_SIZE(markersR); ++ i, p += 20)
	{
		char buf [10];
		uint_fast16_t xx, yy;

		display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markersR [i], smeter_params.r1, smeter_params.r1 + 8, smeterplus, 1);
		polar_to_dek(xb, yb, markersR [i], smeter_params.r1 + 8, & xx, & yy);
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("+%u"), p);

		colmain_setcolors(COLORMAIN_RED, COLORMAIN_BLACK);
		colmain_string3_at_xy(bg, SM_BG_W, SM_BG_H, xx - strwidth3(buf) / 2, yy - pad2w3 + 1, buf);
	}
	for (i = 0; i < ARRAY_SIZE(markers2R); ++ i)
	{
		display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markers2R [i], smeter_params.r1, smeter_params.r1 + 4, smeterplus, 1);
	}
	display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smeter_params.gs, smeter_params.gm, smeter_params.r1, 1, smeter, 1);
	display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smeter_params.gm, smeter_params.ge, smeter_params.r1, 1, smeterplus, 1);
	display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smeter_params.gs, smeter_params.ge, smeter_params.r2, 1, COLORMAIN_WHITE, 1);
}

// ширина занимаемого места - 15 ячеек (240/16 = 15)
void
display2_smeter15(
	uint_fast8_t xgrid,
	uint_fast8_t ygrid,
	dctx_t * pctx
	)
{
	/* получение координат прямоугольника с изображением */
	const uint_fast16_t width = GRID2X(15);
	const uint_fast16_t height = GRID2Y(14);
	const uint_fast16_t x0 = GRID2X(xgrid);
	const uint_fast16_t y0 = GRID2Y(ygrid);
	const int xc = x0 + width / 2;
	const int yc = y0 + 120;

	const uint_fast8_t is_tx = hamradio_get_tx();
	PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
	static uint_fast8_t first_tx = 0;

	int gp = smeter_params.gs, gv = smeter_params.gs, gv_trace = smeter_params.gs, gswr = smeter_params.gs;

	//colpip_rect(colmain_fb_draw(), DIM_X, DIM_Y, x0, y0 - 8, x0 + width - 1, y0 + height - 1 + 15, DESIGNCOLORSTATE, 0);

	if (is_tx)
	{
		enum { gx_hyst = 3 };		// гистерезис в градусах
		/* фильтрация - (в градусах) */
		static uint_fast16_t gp_smooth, gswr_smooth;

		if (first_tx)				// сброс при переходе на передачу
		{
			first_tx = 0;
			gp_smooth = smeter_params.gs;
			gswr_smooth = smeter_params.gs;
		}

		uint_fast16_t power, swr10; 		// swr10 = 0..30 for swr 1..4
		adcvalholder_t forward, reflected;

		power = board_getadc_filtered_truevalue(PWRI);
		gp = smeter_params.gs + normalize(power, 0, maxpwrcali << 4, smeter_params.ge - smeter_params.gs);

//		forward = board_getswrmeter(& reflected, swrcalibr);
		reflected = board_getadc_unfiltered_truevalue(REFMRRIX) * (unsigned long) swrcalibr / 100;		// добавить фильтрацию рассчитанного значения
		forward = board_getadc_unfiltered_truevalue(FWDMRRIX);

		const uint_fast16_t fullscale = (SWRMIN * 40 / 10) - SWRMIN;
									// рассчитанное  значение
		if (forward < minforward)
			swr10 = 0;				// SWR=1
		else if (forward <= reflected)
			swr10 = fullscale;		// SWR is infinite
		else
			swr10 = (forward + reflected) * SWRMIN / (forward - reflected) - SWRMIN;
		gswr = smeter_params.gs + normalize(swr10, 0, fullscale, smeter_params.ge - smeter_params.gs);

		if (gp > smeter_params.gs)
			gp_smooth = gp;

		if (gp == smeter_params.gs && gp_smooth > smeter_params.gs)
			gp = (gp_smooth -= gx_hyst) > smeter_params.gs ? gp_smooth : smeter_params.gs;

		if (gswr > smeter_params.gs)
			gswr_smooth = gswr;

		if (gswr == smeter_params.gs && gswr_smooth > smeter_params.gs)
			gswr = (gswr_smooth -= gx_hyst) > smeter_params.gs ? gswr_smooth : smeter_params.gs;
	}
	else
	{
		uint_fast8_t tracemax;
		uint_fast8_t value = board_getsmeter(& tracemax, 0, UINT8_MAX, 0);
		tracemax = value > tracemax ? value : tracemax;	// защита от рассогласования значений

		gv =
			smeter_params.gs + normalize3(value, 	s9level - s9delta, s9level, s9level + s9_60_delta, smeter_params.gm - smeter_params.gs, smeter_params.ge - smeter_params.gs);
		gv_trace =
			smeter_params.gs + normalize3(tracemax, s9level - s9delta, s9level, s9level + s9_60_delta, smeter_params.gm - smeter_params.gs, smeter_params.ge - smeter_params.gs);

		first_tx = 1;
	}

	if (is_tx)
	{
		// TX state
		colpip_plot(fr, DIM_X, DIM_Y, x0, y0, smeter_bg [SM_STATE_TX], SM_BG_W, SM_BG_H);

		if (gswr > smeter_params.gs)
		{
			uint_fast16_t xx, yy;

			display_segm_buf(fr, DIM_X, DIM_Y, xc, yc, smeter_params.gs, gswr, smeter_params.r2 + 2, 1, COLORMAIN_YELLOW, 0);
			display_segm_buf(fr, DIM_X, DIM_Y, xc, yc, smeter_params.gs, gswr, smeter_params.r1 - 2, 1, COLORMAIN_YELLOW, 0);
			display_radius_buf(fr, DIM_X, DIM_Y, xc, yc, smeter_params.gs, smeter_params.r1 - 2, smeter_params.r2 + 2, COLORMAIN_YELLOW, 0);
			display_radius_buf(fr, DIM_X, DIM_Y, xc, yc, gswr, smeter_params.r1 - 2, smeter_params.r2 + 2, COLORMAIN_YELLOW, 0);
			polar_to_dek(xc, yc, gswr - 1, smeter_params.r1 - 4, & xx, & yy);
			display_floodfill(fr, DIM_X, DIM_Y, xx, yy, COLORMAIN_YELLOW, COLORMAIN_BLACK);
		}

		const COLORMAIN_T ct2 = gp > smeter_params.gm ? COLORMAIN_RED : COLORMAIN_GREEN;
		display_radius_buf(fr, DIM_X, DIM_Y, xc - 1, yc, gp, smeter_params.rv1, smeter_params.rv2, ct2, 0);
		display_radius_buf(fr, DIM_X, DIM_Y, xc, yc, gp, smeter_params.rv1, smeter_params.rv2, ct2, 0);
		display_radius_buf(fr, DIM_X, DIM_Y, xc + 1, yc, gp, smeter_params.rv1, smeter_params.rv2, ct2, 0);
	}
	else
	{
		// RX state
		colpip_plot(fr, DIM_X, DIM_Y, x0, y0, smeter_bg [SM_STATE_RX], SM_BG_W, SM_BG_H);

		{
			// Рисование стрелки
			const COLORMAIN_T ct = gv_trace > smeter_params.gm ? COLORMAIN_RED : COLORMAIN_YELLOW;
			display_radius_buf(fr, DIM_X, DIM_Y, xc - 1, yc, gv_trace, smeter_params.r1 - 2, smeter_params.r2 + 2, ct, 0);
			display_radius_buf(fr, DIM_X, DIM_Y, xc, yc, gv_trace, smeter_params.r1 - 2, smeter_params.r2 + 2, ct, 0);
			display_radius_buf(fr, DIM_X, DIM_Y, xc + 1, yc, gv_trace, smeter_params.r1 - 2, smeter_params.r2 + 2, ct, 0);
		}

		{
			// Рисование стрелки (?)
			const COLORMAIN_T ct2 = gv > smeter_params.gm ? COLORMAIN_RED : COLORMAIN_GREEN;
			display_radius_buf(fr, DIM_X, DIM_Y, xc - 1, yc, gv, smeter_params.rv1, smeter_params.rv2, ct2, 0);
			display_radius_buf(fr, DIM_X, DIM_Y, xc, yc, gv, smeter_params.rv1, smeter_params.rv2, ct2, 0);
			display_radius_buf(fr, DIM_X, DIM_Y, xc + 1, yc, gv, smeter_params.rv1, smeter_params.rv2, ct2, 0);
		}
	}
}

#endif /* LCDMODE_LTDC */

// S-METER
/* отображение S-метра на приёме или передаче */
// Функция вызывается из display2.c
void
display2_bars_rx(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHBARS
	uint_fast8_t tracemax;
	uint_fast8_t v = board_getsmeter(& tracemax, 0, UINT8_MAX, 0);
	display_smeter(x, y, v, tracemax, s9level, s9delta, s9_60_delta);
#endif /* WITHBARS */
}

// SWR-METER, POWER-METER
/* отображение P-метра и SWR-метра на приёме или передаче */
// Функция вызывается из display2.c
void
display2_bars_tx(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHBARS
#if WITHTX
	#if (WITHSWRMTR || WITHSHOWSWRPWR)
		uint_fast8_t pwrtrace;
		const uint_fast8_t pwr = board_getpwrmeter(& pwrtrace);
		adcvalholder_t reflected;
		const adcvalholder_t forward = board_getswrmeter(& reflected, swrcalibr);
		#if WITHSHOWSWRPWR
			display_swrmeter(x, y, forward, reflected, minforward);
			display_pwrmeter(x, y, pwr, pwrtrace, maxpwrcali);
		#else
			if (swrmode || getactualtune())
				display_swrmeter(x, y, forward, reflected, minforward);
			else
				display_pwrmeter(x, y, pwr, pwrtrace, maxpwrcali);
		#endif
	#elif WITHPWRMTR
		uint_fast8_t pwrtrace;
		const uint_fast8_t pwr = board_getpwrmeter(& pwrtrace);
		display_pwrmeter(x, y, pwr, pwrtrace, maxpwrcali);
	#endif

#endif /* WITHTX */
#endif /* WITHBARS */
}

#if CTLSTYLE_RA4YBO_AM0

// S-METER, SWR-METER, POWER-METER
/* отображение S-метра или SWR-метра на приёме или передаче */
// Вызывается из display2.c (версия для CTLSTYLE_RA4YBO_AM0)
void
display2_bars_amv0(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHBARS
	if (gtx)
	{
#if WITHTX
	#if (WITHSWRMTR || WITHSHOWSWRPWR)
		uint_fast8_t pwrtrace;
		const uint_fast8_t pwr = board_getpwrmeter(& pwrtrace);
		const uint_fast8_t modulaton = board_getadc_filtered_u8(REF, 0, UINT8_MAX);
		display_modulationmeter_amv0(x, y, modulaton, UINT8_MAX);
		display_pwrmeter_amv0(x, y, pwr, pwrtrace, maxpwrcali);
	#elif WITHPWRMTR
		uint_fast8_t pwrtrace;
		const uint_fast8_t pwr = board_getpwrmeter(& pwrtrace);
		display_pwrmeter_amv0(x, y, pwr, pwrtrace, maxpwrcali);
	#endif

#endif
	}
	else
	{
		uint_fast8_t tracemax;
		uint_fast8_t v = board_getsmeter(& tracemax, 0, UINT8_MAX, 0);
		display_smeter_amv0(x, y, v, tracemax, s9level, s9delta, s9_60_delta);
	}
#endif /* WITHBARS */
}

#endif /* CTLSTYLE_RA4YBO_AM0 */

#ifndef WITHGUIMAXX
#define WITHGUIMAXX		DIM_X
#endif

#ifndef WITHGUIMAXY
#define WITHGUIMAXY		DIM_Y
#endif

#if WITHGUIMAXX < GUIMINX
#undef WITHGUIMAXX
#define WITHGUIMAXX 	GUIMINX
#endif

#if WITHGUIMAXY < GUIMINY
#undef WITHGUIMAXY
#define WITHGUIMAXY 	GUIMINY
#endif

#if WITHTOUCHGUI

static void button1_handler(void);
static void button2_handler(void);
static void button3_handler(void);
static void button4_handler(void);
static void button5_handler(void);
static void button6_handler(void);
static void button7_handler(void);
static void button8_handler(void);
static void button9_handler(void);
static void labels_menu_handler (void);
static void buttons_mode_handler(void);
static void buttons_bp_handler(void);
static void buttons_freq_handler(void);
static void buttons_menu_handler(void);
static void buttons_uif_handler(void);
static void window_bp_process(void);
static void window_menu_process(void);
static void window_freq_process(void);
static void window_uif_process(void);
static void window_mode_process(void);
static void window_agc_process(void);
static void window_enc2_process(void);
static void window_audioparams_process(void);
static void gui_main_process(void);

	typedef enum {
		TYPE_DUMMY,
		TYPE_BUTTON,
		TYPE_LABEL,
		TYPE_SLIDER
	} element_type_t;

	enum {
		PRESSED,						// нажато
		RELEASED,						// отпущено после нажатия внутри элемента
		CANCELLED,						// первоначальное состояние или отпущено после нажатия вне элемента
		DISABLED						// заблокировано для нажатия
	};

	enum {
		BUTTON_NON_LOCKED,
		BUTTON_LOCKED
	};

	enum {
		BUTTON_CODE_BK = 20,
		BUTTON_CODE_OK = 30,
		BUTTON_CODE_DONE = 99
	};

	enum {								// button_handler.visible & windowpip.is_show
		NON_VISIBLE,					// parent window закрыто, кнопка не отрисовывается
		VISIBLE							// parent window на экране, кнопка отрисовывается
	};

	enum {								// button_handler.parent & windowpip.window_id
		WINDOW_MAIN,					// постоянно отображаемые кнопки внизу экрана
		WINDOW_MODES,					// переключение режимов работы, видов модуляции
		WINDOW_BP,						// регулировка полосы пропускания фильтров выбранного режима
		WINDOW_AGC,						// выбор пресетов настроек АРУ для текущего режима модуляции
		WINDOW_FREQ,
		WINDOW_MENU,
		WINDOW_ENC2,
		WINDOW_UIF,
		WINDOW_AUDIO
	};

	enum {
		NAME_ARRAY_SIZE = 20,
		TEXT_ARRAY_SIZE = 20,
		MENU_ARRAY_SIZE = 30,
		TOUCH_ARRAY_SIZE = 100
	};

	enum {
		BTN_BUF_W = 128,
		BTN_BUF_H = 64
	};

	typedef ALIGNX_BEGIN PACKEDCOLORMAIN_T bg_t [GXSIZE(BTN_BUF_W, BTN_BUF_H)] ALIGNX_END;

	typedef ALIGNX_BEGIN struct {
		uint8_t w;
		uint8_t h;
		bg_t bg_non_pressed;
		bg_t bg_pressed;
		bg_t bg_locked;
		bg_t bg_locked_pressed;
		bg_t bg_disabled;
	} ALIGNX_END btn_bg_t;

	static btn_bg_t btn_bg [] = {
			{ 100, 44, },
			{ 86, 44, },
			{ 50, 50, },
			{ 40, 40, },
	};

	enum { BG_COUNT = ARRAY_SIZE(btn_bg) };

	typedef struct {
		uint16_t x1;				// координаты от начала экрана
		uint16_t y1;
		uint8_t w;
		uint8_t h;
		void(*onClickHandler) (void);	// обработчик события RELEASED
		uint8_t state;				// текущее состояние кнопки
		uint8_t is_locked;			// признак фиксации кнопки
		uint8_t is_trackable;		// получение относительных координат точки перемещения нажатия, нужно ли для кнопок?
		uint8_t parent;				// индекс окна, в котором будет отображаться кнопка
		uint8_t visible;			// рисовать ли кнопку на экране
		uintptr_t payload;
		char name [NAME_ARRAY_SIZE];
		char text [TEXT_ARRAY_SIZE]; // текст внутри кнопки, разделитель строк |, не более 2х строк
	} button_t;

	static button_t buttons [] = {
	//   x1, y1, w, h,  onClickHandler,        state,   is_locked, is_trackable, parent,    visible,      payload,	              name, 		text
		{ },
		{ 0, 0, 86, 44, button1_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN,  VISIBLE,     UINTPTR_MAX, 		"btnMode", 		"Mode", },
		{ 0, 0, 86, 44, button2_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN,  VISIBLE,     UINTPTR_MAX, 		"btnAF", 		"AF|filter", },
		{ 0, 0, 86, 44, button3_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN,  VISIBLE,     UINTPTR_MAX, 		"btnAGC", 		"AGC", },
		{ 0, 0, 86, 44, button4_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN,  VISIBLE,     UINTPTR_MAX, 		"btn4", 		"Freq", },
		{ 0, 0, 86, 44, button5_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN,  VISIBLE,     UINTPTR_MAX, 		"btn5", 		"", },
		{ 0, 0, 86, 44, button6_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN,  VISIBLE,     UINTPTR_MAX, 		"btn6", 		"", },
		{ 0, 0, 86, 44, button7_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN,  VISIBLE,     UINTPTR_MAX, 		"btn7", 		"", },
		{ 0, 0, 86, 44, button8_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN,  VISIBLE,     UINTPTR_MAX, 		"btn8", 		"Audio|params", },
		{ 0, 0, 86, 44, button9_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN,  VISIBLE,     UINTPTR_MAX, 		"btnSysMenu", 	"System|settings", },
		{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_LSB, 		"btnModeLSB", 	"LSB", },
		{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_CW,  		"btnModeCW", 	"CW", },
		{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_AM,  		"btnModeAM", 	"AM", },
		{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_DGL, 		"btnModeDGL", 	"DGL", },
		{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_USB, 		"btnModeUSB", 	"USB", },
		{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_CWR, 		"btnModeCWR", 	"CWR", },
		{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_NFM, 		"btnModeNFM", 	"NFM", },
		{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_DGU, 		"btnModeDGU", 	"DGU", },
		{ 0, 0, 86, 44, buttons_bp_handler,	  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP,    NON_VISIBLE, UINTPTR_MAX, 		"btnAF_1", 		"", },
		{ 0, 0, 86, 44, buttons_bp_handler,   CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP,    NON_VISIBLE, UINTPTR_MAX, 		"btnAF_OK", 	"OK", },
		{ 0, 0, 86, 44, buttons_bp_handler,   CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP,    NON_VISIBLE, UINTPTR_MAX, 		"btnAF_2", 		"", },
		{ 0, 0, 86, 44, hamradio_set_agc_off, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, 		"btnAGCoff", 	"AGC|off", },
		{ 0, 0, 86, 44, hamradio_set_agc_slow,CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, 		"btnAGCslow", 	"AGC|slow", },
		{ 0, 0, 86, 44, hamradio_set_agc_fast,CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, 		"btnAGCfast", 	"AGC|fast", },
		{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, 1, 		 		"btnFreq1", 	"1", },
		{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, 2, 		 		"btnFreq2", 	"2", },
		{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, 3, 		 		"btnFreq3", 	"3", },
		{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, BUTTON_CODE_BK, 	"btnFreqBK", 	"<-", },
		{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, 4, 	 			"btnFreq4", 	"4", },
		{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, 5, 				"btnFreq5", 	"5", },
		{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, 6, 				"btnFreq6", 	"6", },
		{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, BUTTON_CODE_OK, 	"btnFreqOK", 	"OK", },
		{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, 7, 				"btnFreq7", 	"7", },
		{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, 8,  				"btnFreq8", 	"8", },
		{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, 9, 		 		"btnFreq9", 	"9", },
		{ 0, 0, 50, 50, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, 0, 	 			"btnFreq0", 	"0", },
		{ 0, 0, 40, 40, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU,  NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu-",	"-", },
		{ 0, 0, 40, 40, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU,  NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu+",	"+", },
		{ 0, 0, 40, 40, buttons_uif_handler,  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_UIF,   NON_VISIBLE, UINTPTR_MAX, 		"btnUIF-", 		"-", },
		{ 0, 0, 40, 40, buttons_uif_handler,  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_UIF,   NON_VISIBLE, UINTPTR_MAX, 		"btnUIF+", 		"+", },
#if ! WITHOLDMENUSTYLE
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu1",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu2",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu3",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu4",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu5",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu6",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu7",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu8",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu9",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu10",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu11",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu12",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu13",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu14",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu15",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu16",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu17",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu18",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu19",	"", },
		{ 0, 0, 100, 44, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU, NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu20",	"", },
#endif

//		{ 375, 120, buttons_uif_handler, 	NULL, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_UIF,   NON_VISIBLE, UINTPTR_MAX, 			"btnUIF_OK", "OK", },
	};
	enum { BUTTONS_COUNT = ARRAY_SIZE(buttons) };

	typedef enum {
		FONT_LARGE,		// S1D13781_smallfont_LTDC
		FONT_MEDIUM,	// S1D13781_smallfont2_LTDC
		FONT_SMALL		// S1D13781_smallfont3_LTDC
	} font_size_t;

	typedef struct {
		uint16_t x;
		uint16_t y;
		uint8_t parent;
		uint8_t state;
		uint8_t is_trackable;
		uint8_t visible;
		char name [NAME_ARRAY_SIZE];
		char text [TEXT_ARRAY_SIZE];
		font_size_t font_size;
		PACKEDCOLORMAIN_T color;
		void (*onClickHandler) (void);
	} label_t;

	static label_t labels[] = {
	//    x, y,  parent,     state, is_trackable, visible, name,       Text, font_size, color, onClickHandler
		{ },
		{ 0, 0, WINDOW_BP,   DISABLED,  0, NON_VISIBLE, "lbl_low",  	  "", FONT_LARGE, COLORPIP_YELLOW, },
		{ 0, 0, WINDOW_BP,   DISABLED,  0, NON_VISIBLE, "lbl_high", 	  "", FONT_LARGE, COLORPIP_YELLOW, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", 	  "", FONT_LARGE, COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_ENC2, DISABLED,  0, NON_VISIBLE, "lbl_enc2_param", "", FONT_LARGE, COLORPIP_WHITE, },
		{ 0, 0, WINDOW_ENC2, DISABLED,  0, NON_VISIBLE, "lbl_enc2_val",	  "", FONT_LARGE, COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_UIF,  DISABLED,  0, NON_VISIBLE, "lbl_uif_param",  "", FONT_LARGE, COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_UIF,  DISABLED,  0, NON_VISIBLE, "lbl_uif_val", 	  "", FONT_LARGE, COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_FREQ, DISABLED,  0, NON_VISIBLE, "lbl_freq_val",   "", FONT_LARGE, COLORPIP_YELLOW, },
		{ 0, 0,	WINDOW_AUDIO, DISABLED, 0, NON_VISIBLE, "lbl_eq0.08_val",  "", FONT_MEDIUM, COLORPIP_YELLOW, },
		{ 0, 0,	WINDOW_AUDIO, DISABLED, 0, NON_VISIBLE, "lbl_eq0.23_val",  "", FONT_MEDIUM, COLORPIP_YELLOW, },
		{ 0, 0,	WINDOW_AUDIO, DISABLED, 0, NON_VISIBLE, "lbl_eq0.65_val",  "", FONT_MEDIUM, COLORPIP_YELLOW, },
		{ 0, 0,	WINDOW_AUDIO, DISABLED, 0, NON_VISIBLE, "lbl_eq1.8_val",   "", FONT_MEDIUM, COLORPIP_YELLOW, },
		{ 0, 0,	WINDOW_AUDIO, DISABLED, 0, NON_VISIBLE, "lbl_eq5.3_val",   "", FONT_MEDIUM, COLORPIP_YELLOW, },
		{ 0, 0,	WINDOW_AUDIO, DISABLED, 0, NON_VISIBLE, "lbl_eq0.08_name", "", FONT_MEDIUM, COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_AUDIO, DISABLED, 0, NON_VISIBLE, "lbl_eq0.23_name", "", FONT_MEDIUM, COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_AUDIO, DISABLED, 0, NON_VISIBLE, "lbl_eq0.65_name", "", FONT_MEDIUM, COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_AUDIO, DISABLED, 0, NON_VISIBLE, "lbl_eq1.8_name",  "", FONT_MEDIUM, COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_AUDIO, DISABLED, 0, NON_VISIBLE, "lbl_eq5.3_name",  "", FONT_MEDIUM, COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_MAIN,  DISABLED, 0, NON_VISIBLE, "lbl_infobar_1_1", "", FONT_MEDIUM, COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_MAIN,  DISABLED, 0, NON_VISIBLE, "lbl_infobar_1_2", "", FONT_MEDIUM, COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_MAIN,  DISABLED, 0, NON_VISIBLE, "lbl_infobar_2_1", "", FONT_MEDIUM, COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_MAIN,  DISABLED, 0, NON_VISIBLE, "lbl_infobar_2_2", "", FONT_MEDIUM, COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_MAIN,  DISABLED, 0, NON_VISIBLE, "lbl_infobar_3_1", "", FONT_MEDIUM, COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_MAIN,  DISABLED, 0, NON_VISIBLE, "lbl_infobar_3_2", "", FONT_MEDIUM, COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_MAIN,  DISABLED, 0, NON_VISIBLE, "lbl_infobar_4_1", "", FONT_MEDIUM, COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_MAIN,  DISABLED, 0, NON_VISIBLE, "lbl_infobar_4_2", "", FONT_MEDIUM, COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_MAIN,  DISABLED, 0, NON_VISIBLE, "lbl_infobar_5_1", "", FONT_MEDIUM, COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_MAIN,  DISABLED, 0, NON_VISIBLE, "lbl_infobar_5_2", "", FONT_MEDIUM, COLORPIP_WHITE, },

		};
	enum { LABELS_COUNT = ARRAY_SIZE(labels) };

	typedef enum  {
		ORIENTATION_VERTICAL,
		ORIENTATION_HORIZONTAL
	} orientation_t;

	enum {
		sliders_width = 8,		// ширина шкалы
		sliders_w = 12,			// размеры ползунка
		sliders_h = 18			// от центра (*2)
	};

	typedef struct {
		uint16_t x;
		uint16_t y;
		uint16_t x1_p;			// координаты ползунка
		uint16_t y1_p;			// для update_touch_list
		uint16_t x2_p;
		uint16_t y2_p;
		orientation_t orientation;
		uint8_t parent;
		char name [NAME_ARRAY_SIZE];
		uint8_t state;
		uint8_t visible;
		uint16_t size;			// длина шкалы в пикселях
		uint8_t value;			// 0..100 %
		uint8_t value_old;		// для перерасчетов при изменении значения
		uint16_t value_p;		// в пикселях от начала шкалы
		float step;
	} slider_t;

	static slider_t sliders[] = {
			{ },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_VERTICAL, WINDOW_AUDIO, "eq0.08", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_VERTICAL, WINDOW_AUDIO, "eq0.23", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_VERTICAL, WINDOW_AUDIO, "eq0.65", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_VERTICAL, WINDOW_AUDIO, "eq1.8", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_VERTICAL, WINDOW_AUDIO, "eq5.3", CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
	};
	enum { SLIDERS_COUNT = ARRAY_SIZE(sliders) };

	typedef enum {
		ALIGN_LEFT_X 	= WITHGUIMAXX >> 2,					// вертикальное выравнивание по центру левой половины экрана
		ALIGN_CENTER_X 	= WITHGUIMAXX >> 1,					// вертикальное выравнивание по центру экрана
		ALIGN_RIGHT_X 	= ALIGN_LEFT_X + ALIGN_CENTER_X,	// вертикальное выравнивание по центру правой половины экрана
		ALIGN_Y 		= WITHGUIMAXY >> 1					// горизонтальное выравнивание всегда по центру экрана
	} align_t;

	typedef struct {
		uint8_t window_id;			// в окне будут отображаться элементы с соответствующим полем for_window
		align_t align_mode;			// вертикаль выравнивания окна
		uint16_t x1;
		uint16_t y1;
		uint16_t w;
		uint16_t h;
		char name[NAME_ARRAY_SIZE];	// текст, выводимый в заголовке окна
		uint8_t state;
		uint8_t first_call;			// признак первого вызова для различных инициализаций
		void (*onVisibleProcess) (void);
	} window_t;

	static window_t windows[] = {
	//     window_id,   align_mode,     x1, y1, w, h,   title,      is_show, first_call, onVisibleProcess
		{ WINDOW_MAIN, 	ALIGN_LEFT_X,	0, 0,   0,   0, "",  	   	   NON_VISIBLE,	0, gui_main_process, },
		{ WINDOW_MODES, ALIGN_CENTER_X, 0, 0, 402, 150, "Select mode", NON_VISIBLE, 0, window_mode_process, },
		{ WINDOW_BP,    ALIGN_CENTER_X, 0, 0, 372, 205, "Bandpass",    NON_VISIBLE, 0, window_bp_process, },
		{ WINDOW_AGC,   ALIGN_CENTER_X, 0, 0, 372, 110, "AGC control", NON_VISIBLE, 0, window_agc_process, },
		{ WINDOW_FREQ,  ALIGN_CENTER_X, 0, 0, 250, 215, "Freq:", 	   NON_VISIBLE, 0, window_freq_process, },
		{ WINDOW_MENU,  ALIGN_CENTER_X, 0, 0, 550, 240, "Settings",	   NON_VISIBLE, 0, window_menu_process, },
		{ WINDOW_ENC2, 	ALIGN_RIGHT_X, 	0, 0, 185, 105, "Fast menu",   NON_VISIBLE, 0, window_enc2_process, },
		{ WINDOW_UIF, 	ALIGN_LEFT_X, 	0, 0, 200, 145, "",   		   NON_VISIBLE, 0, window_uif_process, },
		{ WINDOW_AUDIO, ALIGN_CENTER_X, 0, 0, 430, 350, "MIC params",  NON_VISIBLE, 0, window_audioparams_process, },
	};
	enum { windows_count = ARRAY_SIZE(windows) };

	typedef struct {
		int16_t rotate;			// признак поворота второго энкодера
		uint8_t press;			// короткое нажание
		uint8_t hold;			// длинное нажатие
		uint8_t busy;			// второй энкодер выделен для обработки данных окна
		uint8_t rotate_done;	// событие поворота от энкодера обработано, можно получать новые данные
		uint8_t press_done;		// событие нажатия от энкодера обработано, можно получать новые данные
	} enc2_t;

	static enc2_t encoder2 = { 0, 0, 0, 0, 1, 1, };

	static editfreq_t editfreq;

	enum {
		MENU_OFF,
		MENU_GROUPS,
		MENU_PARAMS,
		MENU_VALS,
		MENU_COUNT
	};

#if WITHOLDMENUSTYLE
	typedef struct {
		uint8_t first_id;			// первое вхождение номера метки уровня
		uint8_t last_id;			// последнее вхождение номера метки уровня
		uint8_t num_rows;			// число меток уровня
		uint8_t count;				// число значений уровня
		int8_t selected_str;		// выбранная строка уровня
		int8_t selected_label;		// выбранная метка уровня
		uint8_t add_id;				// номер строки уровня, отображаемой первой
		menu_names_t menu_block [MENU_ARRAY_SIZE];	// массив значений уровня меню
	} menu_t;

	static menu_t menu[MENU_COUNT];
#else

#endif

	typedef struct {
		char name [TEXT_ARRAY_SIZE];
		uint16_t menupos;
		uint8_t exitkey;
	} menu_by_name_t;

	menu_by_name_t menu_uif;

	typedef struct {
		element_type_t type;		// тип элемента, поддерживающего реакцию на касания
		window_t * win;
		void * link;
		uint8_t pos;
		uint8_t state;				// текущее состояние элемента
		uint8_t visible;			// текущая видимость элемента
		uint8_t is_trackable;		// поддерживает ли элемент возврат относительных координат перемещения точки нажатия
		uint16_t x1;				// координаты окна
		uint16_t y1;
		uint16_t x2;
		uint16_t y2;
	} touch_t;

	typedef struct {
		uint16_t last_pressed_x; 	  // последняя точка касания экрана
		uint16_t last_pressed_y;
		uint8_t kbd_code;
		element_type_t selected_type; // тип последнего выбранного элемента
		touch_t * selected_link;	  // ссылка на выбранный элемент
		uint8_t state;				  // последнее состояние
		uint8_t is_touching_screen;   // есть ли касание экрана в данный момент
		uint8_t is_after_touch; 	  // есть ли касание экрана после выхода точки касания из элемента (при is_tracking == 0)
		uint8_t is_tracking;		  // получение относительных координат точки перемещения нажатия
		int16_t vector_move_x;	 	  // в т.ч. и за границами элемента, при state == PRESSED
		int16_t vector_move_y;
		uint8_t timer_1sec_updated;	  // для периодических обновлений состояния
	} gui_t;

	static gui_t gui = { 0, 0, KBD_CODE_MAX, TYPE_DUMMY, NULL, CANCELLED, 0, 0, 0, 0, 0, 1, };

	static touch_t touch_elements[TOUCH_ARRAY_SIZE];
	static uint_fast8_t touch_count = 0;
	static uint_fast8_t menu_label_touched = 0;
	static uint_fast8_t menu_level;
	static enc2_menu_t * gui_enc2_menu;

	void gui_timer_update(void * arg)
	{
		gui.timer_1sec_updated = 1;
	}

	static void reset_tracking(void)
	{
		gui.vector_move_x = 0;
		gui.vector_move_y = 0;
	}

	/* Возврат ссылки на запись в структуре по названию и типу окна */
	static void * find_gui_element_ref(element_type_t type, window_t * win, const char * name)
	{
		switch (type)
		{
		case TYPE_BUTTON:
			for (uint_fast8_t i = 1; i < BUTTONS_COUNT; i++)
			{
				button_t * bh = & buttons[i];
				if (bh->parent == win->window_id && strcmp(bh->name, name) == 0)
					return (button_t *) bh;
			}
			return NULL;

		case TYPE_LABEL:
			for (uint_fast8_t i = 1; i < LABELS_COUNT; i++)
			{
				label_t * lh = & labels[i];
				if (lh->parent == win->window_id && strcmp(lh->name, name) == 0)
					return (label_t *) lh;
			}
			return NULL;

		case TYPE_SLIDER:
			for (uint_fast8_t i = 1; i < SLIDERS_COUNT; i++)
			{
				slider_t * sh = & sliders[i];
				if (sh->parent == win->window_id && strcmp(sh->name, name) == 0)
					return (slider_t *) sh;
			}
			return NULL;

		default:
			ASSERT(0);
			return NULL;
		}
	}

	/* Поиск области вхождения (включительно) записей в структуре по типу элемента */
	static void find_entry_area_elements(element_type_t type, window_t * win, uint_fast8_t * id_start, uint_fast8_t * id_end)
	{
		uint_fast8_t i = 0;

		for(; i < touch_count; i++)
		{
			if (touch_elements[i].type == type && touch_elements[i].win == win)
				break;
		}
		* id_start = i;

		for(; i < touch_count; i++)
		{
			if (touch_elements[i].type != type)
				break;
		}
		* id_end = i - 1;

//		uint_fast8_t i = 1;
//		switch (type)
//				{
//				case TYPE_BUTTON:
//
//					for(; i < BUTTONS_COUNT; i ++)
//					{
//						button_t * bh = & buttons[i];
//						if (bh->parent == win->window_id)
//							break;
//					}
//					* id_start = i;
//
//					for(; i < BUTTONS_COUNT; i ++)
//					{
//						button_t * bh = & buttons[i];
//						if (bh->parent != win->window_id)
//							break;
//					}
//					* id_end = i - 1;
//
//					break;
//
//				case TYPE_LABEL:
//
//					for(; i < BUTTONS_COUNT; i ++)
//					{
//						label_t * lh = & labels[i];
//						if (lh->parent == win->window_id)
//							break;
//					}
//					* id_start = i;
//
//					for(; i < BUTTONS_COUNT; i ++)
//					{
//						label_t * lh = & labels[i];
//						if (lh->parent != win->window_id)
//							break;
//					}
//					* id_end = i - 1;
//					break;
//
//				case TYPE_SLIDER:
//
//					for(; i < BUTTONS_COUNT; i ++)
//					{
//						slider_t * sh = & sliders[i];
//						if (sh->parent == win->window_id)
//							break;
//					}
//					* id_start = i;
//
//					for(; i < BUTTONS_COUNT; i ++)
//					{
//						slider_t * sh = & sliders[i];
//						if (sh->parent != win->window_id)
//							break;
//					}
//					* id_end = i - 1;
//					break;
//
//				default:
//					ASSERT(0);
//					break;
//				}

	}

	/* Получение ширины метки в пикселях  */
	static uint_fast8_t get_label_width(const label_t * const lh)
	{
		if (lh->font_size == FONT_LARGE)
			return strlen(lh->text) * SMALLCHARW;
		else if (lh->font_size == FONT_MEDIUM)
			return strlen(lh->text) * SMALLCHARW2;
		else if (lh->font_size == FONT_SMALL)
			return strlen(lh->text) * SMALLCHARW3;
		return 0;
	}

	/* Получение высоты метки в пикселях  */
	static uint_fast8_t get_label_height(const label_t * const lh)
	{
		if (lh->font_size == FONT_LARGE)
			return SMALLCHARH;
		else if (lh->font_size == FONT_MEDIUM)
			return SMALLCHARH2;
		else if (lh->font_size == FONT_SMALL)
			return SMALLCHARH3;
		return 0;
	}

	/* Установки статуса основных кнопок */
	/* При DISABLED в качестве необязательного параметра передать name активной кнопки или "" для блокирования всех */
	static void footer_buttons_state (uint_fast8_t state, ...)
	{
		window_t * win = & windows[WINDOW_MAIN];
		va_list arg;
		char * name = NULL;
		uint_fast8_t id_start, id_end, is_name;
		find_entry_area_elements(TYPE_BUTTON, win, & id_start, & id_end);

		if (state == DISABLED)
		{
			va_start(arg, state);
			name = va_arg(arg, char *);
			va_end(arg);
		}

		for (uint_fast8_t i = id_start; i <= id_end; i++)
		{
			button_t * bh = (button_t *) touch_elements[i].link;
			if (state == DISABLED)
			{
				bh->state = strcmp(bh->name, name) ? DISABLED : CANCELLED;
				bh->is_locked = strcmp(bh->name, name) ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			}
			else if (state == CANCELLED)
			{
				bh->state = CANCELLED;
				bh->is_locked = BUTTON_NON_LOCKED;
			}
		}
	}

	/* Установка признака видимости окна и его элементов */
	static void set_window(window_t * win, uint_fast8_t value)
	{
		uint_fast8_t j = 0;
		win->state = value;
		if (value)
			win->first_call = 1;


		for (uint_fast8_t i = 1; i < BUTTONS_COUNT; i++)
		{
			button_t * bh = & buttons[i];
			if (bh->parent == win->window_id)
			{
				bh->is_locked = 0;
				if (win->state)
				{
					touch_elements[touch_count].link = bh;
					touch_elements[touch_count].win = win;
					touch_elements[touch_count].type = TYPE_BUTTON;
					touch_elements[touch_count].pos = j++;
					touch_count++;
				}
				else
				{
					touch_count--;
					bh->visible = NON_VISIBLE;
				}
			}
		}

		j = 0;
		for (uint_fast8_t i = 1; i < LABELS_COUNT; i++)
		{
			label_t * lh = & labels[i];
			if (lh->parent == win->window_id)
			{
				if (win->state)
				{
					touch_elements[touch_count].link = lh;
					touch_elements[touch_count].win = win;
					touch_elements[touch_count].type = TYPE_LABEL;
					touch_elements[touch_count].pos = j++;
					touch_count++;
				}
				else
				{
					touch_count--;
					lh->visible = NON_VISIBLE;
				}
			}
		}

		j = 0;
		for (uint_fast8_t i = 1; i < SLIDERS_COUNT; i++)
		{
			slider_t * sh = & sliders[i];
			if (sh->parent == win->window_id)
			{
				if (win->state)
				{
					touch_elements[touch_count].link = (slider_t *) sh;
					touch_elements[touch_count].win = win;
					touch_elements[touch_count].type = TYPE_SLIDER;
					touch_elements[touch_count].pos = j++;
					touch_count++;
				}
				else
				{
					touch_count--;
					sh->visible = NON_VISIBLE;
				}

			}
		}
	}

	/* Расчет экранных координат окна */
	static void calculate_window_position(window_t * win)
	{
		win->y1 = ALIGN_Y - win->h / 2;

		switch (win->align_mode)
		{
		case ALIGN_LEFT_X:
			if (ALIGN_LEFT_X - win->w / 2 < 0)
				win->x1 = 0;
			else
				win->x1 = ALIGN_LEFT_X - win->w / 2;
			break;

		case ALIGN_RIGHT_X:
			if (ALIGN_RIGHT_X + win->w / 2 > WITHGUIMAXX)
				win->x1 = WITHGUIMAXX - win->w;
			else
				win->x1 = ALIGN_RIGHT_X - win->w / 2;
			break;

		case ALIGN_CENTER_X:
		default:
			win->x1 = ALIGN_CENTER_X - win->w / 2;
			break;
		}
	}

	static void buttons_bp_handler(void)
	{
		if(gui.selected_type == TYPE_BUTTON)
		{
			window_t * win = & windows[WINDOW_BP];
			button_t * button_high = find_gui_element_ref(TYPE_BUTTON, win, "btnAF_2");
			button_t * button_low = find_gui_element_ref(TYPE_BUTTON, win, "btnAF_1");
			button_t * button_OK = find_gui_element_ref(TYPE_BUTTON, win, "btnAF_OK");

			if (gui.selected_link->link == button_low)
			{
				button_high->is_locked = 0;
				button_low->is_locked = 1;
			}
			else if (gui.selected_link->link == button_high)
			{
				button_high->is_locked = 1;
				button_low->is_locked = 0;
			}
			else if (gui.selected_link->link == button_OK)
			{
				set_window(win, NON_VISIBLE);
				encoder2.busy = 0;
				footer_buttons_state(CANCELLED);
				hamradio_disable_keyboard_redirect();
			}
		}
	}

	static void window_bp_process(void)
	{
		static uint_fast8_t val_high, val_low, val_c, val_w;
		static uint_fast16_t x_h, x_l, x_c, x_0, y_0;
		window_t * win = & windows[WINDOW_BP];
		uint_fast16_t x_size = 290;
		static label_t * lbl_low, * lbl_high;
		static button_t * button_high, * button_low;

		if (win->first_call == 1)
		{
			uint_fast16_t id = 0, x, y;
			uint_fast8_t interval = 20, col1_int = 35, row1_int = 20, id_start, id_end;
			calculate_window_position(win);
			x_0 = win->x1 + 50;											// координаты нулевой точки графика
			y_0 = win->y1 + 90;

			find_entry_area_elements(TYPE_BUTTON, win, & id_start, & id_end);

			x = win->x1 + col1_int;
			y = win->y1 + win->h - ((button_t *) touch_elements[id_start].link)->h - row1_int;
			for (uint_fast8_t id = id_start; id <= id_end; id++)
			{
				button_t * bh = (button_t *) touch_elements[id].link;;
				bh->x1 = x;
				bh->y1 = y;
				bh->visible = VISIBLE;
				x = x + interval + bh->w;
			}

			button_high = find_gui_element_ref(TYPE_BUTTON, win, "btnAF_2");
			button_low = find_gui_element_ref(TYPE_BUTTON, win, "btnAF_1");

			lbl_low = find_gui_element_ref(TYPE_LABEL, win, "lbl_low");
			lbl_high = find_gui_element_ref(TYPE_LABEL, win, "lbl_high");

			lbl_low->y = y_0 + get_label_height(lbl_low);
			lbl_high->y = y_0 + get_label_height(lbl_high);

			lbl_low->visible = VISIBLE;
			lbl_high->visible = VISIBLE;

			val_high = hamradio_get_high_bp(0);
			val_low = hamradio_get_low_bp(0);
			if (hamradio_get_bp_type())			// BWSET_WIDE
			{
				strcpy(button_high->text, "High|cut");
				strcpy(button_low->text, "Low|cut");
				button_high->is_locked = 1;
			}
			else								// BWSET_NARROW
			{
				strcpy(button_high->text, "Pitch");
				strcpy(button_low->text, "Width");
				button_low->is_locked = 1;
			}
		}

		if (encoder2.rotate != 0 || win->first_call)
		{
			char buf[TEXT_ARRAY_SIZE];

			if (win->first_call)
				win->first_call = 0;

			if (hamradio_get_bp_type())			// BWSET_WIDE
			{
				if (button_high->is_locked == 1)
					val_high = hamradio_get_high_bp(encoder2.rotate);
				else if (button_low->is_locked == 1)
					val_low = hamradio_get_low_bp(encoder2.rotate * 10);
				encoder2.rotate_done = 1;

				x_h = x_0 + normalize(val_high, 0, 50, x_size);
				x_l = x_0 + normalize(val_low / 10, 0, 50, x_size);
				x_c = x_l + (x_h - x_l) / 2;

				local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), val_high * 100);
				strcpy(lbl_high->text, buf);
				lbl_high->x = (x_h + get_label_width(lbl_high) > x_0 + x_size) ?
						(x_0 + x_size - get_label_width(lbl_high)) : x_h;

				local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), val_low * 10);
				strcpy(lbl_low->text, buf);
				lbl_low->x = (x_l - get_label_width(lbl_low) < x_0 - 10) ? (x_0 - 10) : (x_l - get_label_width(lbl_low));
			}
			else						// BWSET_NARROW
			{
				if (button_high->is_locked == 1)
				{
					val_c = hamradio_get_high_bp(encoder2.rotate);
					val_w = hamradio_get_low_bp(0) / 2;
				}
				else if (button_low->is_locked == 1)
				{
					val_c = hamradio_get_high_bp(0);
					val_w = hamradio_get_low_bp(encoder2.rotate) / 2;
				}
				encoder2.rotate_done = 1;
				x_c = x_0 + x_size / 2;
				x_l = x_c - normalize(val_w , 0, 500, x_size);
				x_h = x_c + normalize(val_w , 0, 500, x_size);

				local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), val_w * 20);
				strcpy(lbl_high->text, buf);
				lbl_high->x = x_c - get_label_width(lbl_high) / 2;

				local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("P %d"), val_c * 10);
				strcpy(lbl_low->text, buf);
				lbl_low->x = x_0 + x_size - get_label_width(lbl_low);
			}
			gui.timer_1sec_updated = 1;
		}
		PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
		colpip_line(fr, DIM_X, DIM_Y, x_0 - 10, y_0, x_0 + x_size, y_0, COLORPIP_WHITE);
		colpip_line(fr, DIM_X, DIM_Y, x_0, y_0 - 45, x_0, y_0 + 5, COLORPIP_WHITE);
		colpip_line(fr, DIM_X, DIM_Y, x_l, y_0 - 40, x_l - 4, y_0 - 3, COLORPIP_YELLOW);
		colpip_line(fr, DIM_X, DIM_Y, x_h, y_0 - 40, x_h + 4, y_0 - 3, COLORPIP_YELLOW);
		colpip_line(fr, DIM_X, DIM_Y, x_l, y_0 - 40, x_h, y_0 - 40, COLORPIP_YELLOW);
		colpip_line(fr, DIM_X, DIM_Y, x_c, y_0 - 45, x_c, y_0 + 5, COLORPIP_RED);
	}

	static void buttons_freq_handler (void)
	{
		if (((button_t *) gui.selected_link)->parent == WINDOW_FREQ && editfreq.key == BUTTON_CODE_DONE)
			editfreq.key = ((button_t *) gui.selected_link)->payload;
	}

	static void window_freq_process (void)
	{
		static label_t * lbl_freq;
		window_t * win = & windows[WINDOW_FREQ];

		if (win->first_call == 1)
		{
			uint_fast16_t x, y;
			uint_fast8_t interval = 6, col1_int = 20, row1_int = 40, id_start, id_end;
			button_t * bh = NULL;
			win->first_call = 0;
			calculate_window_position(win);

			find_entry_area_elements(TYPE_BUTTON, win, & id_start, & id_end);

			x = win->x1 + col1_int;
			y = win->y1 + row1_int;

			for (uint_fast8_t i = id_start; i <= id_end; i ++)
			{
				bh = (button_t *) touch_elements[i].link;
				bh->x1 = x;
				bh->y1 = y;
				bh->visible = VISIBLE;

				x = x + interval + bh->w;
				if (x + bh->w > win->x1 + win->w)
				{
					x = win->x1 + col1_int;
					y = y + bh->h + interval;
				}
			}

			bh = find_gui_element_ref(TYPE_BUTTON, win, "btnFreqOK");
			bh->is_locked = BUTTON_LOCKED;

			lbl_freq = find_gui_element_ref(TYPE_LABEL, win, "lbl_freq_val");
			lbl_freq->x = win->x1 + strwidth(win->name) + strwidth(" ") + 20;
			lbl_freq->y = win->y1 + 10;
			strcpy(lbl_freq->text, "     0 k");
			lbl_freq->color = COLORPIP_YELLOW;
			lbl_freq->visible = VISIBLE;

			editfreq.val = 0;
			editfreq.num = 0;
			editfreq.key = BUTTON_CODE_DONE;

			return;
		}

		if (editfreq.key != BUTTON_CODE_DONE)
		{
			lbl_freq->color = COLORPIP_YELLOW;
			char buf[TEXT_ARRAY_SIZE];
			switch (editfreq.key)
			{
			case BUTTON_CODE_BK:
				if (editfreq.num > 0)
				{
					editfreq.val /= 10;
					editfreq.num --;
				}
				break;

			case BUTTON_CODE_OK:
				if(hamradio_set_freq(editfreq.val * 1000))
				{
					set_window(win, NON_VISIBLE);
					footer_buttons_state(CANCELLED);
					hamradio_set_lockmode(0);
					hamradio_disable_keyboard_redirect();
				} else
					lbl_freq->color = COLORPIP_RED;
				break;

			default:
				if (editfreq.num < 6)
				{
					editfreq.val  = editfreq.val * 10 + editfreq.key;
					if (editfreq.val)
						editfreq.num ++;
				}
			}
			editfreq.key = BUTTON_CODE_DONE;
			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%6d k"), editfreq.val);
			strcpy(lbl_freq->text, buf);
		}
	}

	void gui_uif_editmenu(const char * name, uint_fast16_t menupos, uint_fast8_t exitkey)
	{
		window_t * win = & windows[WINDOW_UIF];
		if (win->state == NON_VISIBLE)
		{
			set_window(win, VISIBLE);
			footer_buttons_state(DISABLED, "");
			strcpy(menu_uif.name, name);
			menu_uif.menupos = menupos;
			menu_uif.exitkey = exitkey;
		}
		else
		{
			set_window(win, NON_VISIBLE);
			footer_buttons_state(CANCELLED);
		}
	}

	void gui_put_keyb_code (uint_fast8_t kbch)
	{
		// После обработки события по коду кнопки
		// сбрасывать gui.kbd_code в KBD_CODE_MAX.
		gui.kbd_code = gui.kbd_code == KBD_CODE_MAX ? kbch : gui.kbd_code;
	}

	static void buttons_uif_handler(void)
	{
		window_t * win = & windows[WINDOW_UIF];
		if (gui.selected_type == TYPE_BUTTON && gui.selected_link == find_gui_element_ref(TYPE_BUTTON, win, "btnUIF+"))
			encoder2.rotate = 1;
		else if (gui.selected_type == TYPE_BUTTON && gui.selected_link == find_gui_element_ref(TYPE_BUTTON, win, "btnUIF-"))
			encoder2.rotate = -1;
		else if (gui.selected_type == TYPE_BUTTON && gui.selected_link == find_gui_element_ref(TYPE_BUTTON, win, "btnUIF_OK"))
		{
			hamradio_disable_keyboard_redirect();
			set_window(win, NON_VISIBLE);
			footer_buttons_state(CANCELLED);
		}
	}

	static void window_uif_process(void)
	{
		static label_t * lbl_uif_param, * lbl_uif_val;
		static button_t * button_up, * button_down;
		static uint_fast16_t window_center_x;
		window_t * win = & windows[WINDOW_UIF];

		if (win->first_call)
		{
			win->first_call = 0;
			calculate_window_position(win);
			window_center_x = win->x1 + win->w / 2;

			lbl_uif_param = find_gui_element_ref(TYPE_LABEL, win, "lbl_uif_param");
			strcpy(lbl_uif_param->text, menu_uif.name);
			lbl_uif_param->x = window_center_x - (get_label_width(lbl_uif_param) / 2);
			lbl_uif_param->y = win->y1 + get_label_height(lbl_uif_param);
			lbl_uif_param->visible = VISIBLE;

			lbl_uif_val = find_gui_element_ref(TYPE_LABEL, win, "lbl_uif_val");
			const char * v = hamradio_gui_edit_menu_item(menu_uif.menupos, 0);
			strcpy(lbl_uif_val->text, v);
			lbl_uif_val->x = window_center_x - (get_label_width(lbl_uif_val) / 2);
			lbl_uif_val->y = win->y1 + get_label_height(lbl_uif_param) * 4;
			lbl_uif_val->visible = VISIBLE;

			button_down = find_gui_element_ref(TYPE_BUTTON, win, "btnUIF-");
			button_down->x1 = lbl_uif_val->x - button_down->w - 10;
			button_down->y1 = (lbl_uif_val->y + get_label_height(lbl_uif_param) / 2) - (button_down->h / 2);
			button_down->visible = VISIBLE;

			button_up = find_gui_element_ref(TYPE_BUTTON, win, "btnUIF+");
			button_up->x1 = lbl_uif_val->x + get_label_width(lbl_uif_val) + 10;
			button_up->y1 = button_down->y1;
			button_up->visible = VISIBLE;

			hamradio_enable_keyboard_redirect();
			return;
		}

		if (encoder2.rotate != 0)
		{
			const char * v = hamradio_gui_edit_menu_item(menu_uif.menupos, encoder2.rotate);
			strcpy(lbl_uif_val->text, v);
			lbl_uif_val->x = window_center_x - (get_label_width(lbl_uif_val) / 2);
			encoder2.rotate_done = 1;

			button_down->x1 = lbl_uif_val->x - button_down->w - 10;
			button_up->x1 = lbl_uif_val->x + get_label_width(lbl_uif_val) + 10;

			gui.timer_1sec_updated = 1;
		}

		if (gui.kbd_code != KBD_CODE_MAX)
		{
			if (gui.kbd_code == menu_uif.exitkey)
			{
				hamradio_disable_keyboard_redirect();
				set_window(win, NON_VISIBLE);
				footer_buttons_state(CANCELLED);
			}
			gui.kbd_code = KBD_CODE_MAX;
		}
	}

#if WITHOLDMENUSTYLE

	static void labels_menu_handler (void)
	{
		if (gui.selected_type == TYPE_LABEL)
		{
			label_t * lh = gui.selected_link->link;
			if(strcmp(lh->name, "lbl_group") == 0)
			{
				menu[MENU_GROUPS].selected_label = gui.selected_link->pos % (menu[MENU_GROUPS].num_rows + 1);
				menu_label_touched = 1;
				menu_level = MENU_GROUPS;
			}
			else if(strcmp(lh->name, "lbl_params") == 0)
			{
				menu[MENU_PARAMS].selected_label = gui.selected_link->pos % (menu[MENU_GROUPS].num_rows + 1);
				menu_label_touched = 1;
				menu_level = MENU_PARAMS;
			}
			else if(strcmp(lh->name, "lbl_vals") == 0)
			{
				menu[MENU_VALS].selected_label = gui.selected_link->pos % (menu[MENU_GROUPS].num_rows + 1);
				menu[MENU_PARAMS].selected_label = menu[MENU_VALS].selected_label;
				menu_label_touched = 1;
				menu_level = MENU_VALS;
			}
		}
	}

	static void buttons_menu_handler(void)
	{
		if (! strcmp(((button_t *) gui.selected_link->link)->name, "btnSysMenu+"))
			encoder2.rotate = 1;
		else if (! strcmp(((button_t *) gui.selected_link->link)->name, "btnSysMenu-"))
			encoder2.rotate = -1;
	}

	static void window_menu_process(void)
	{
		static uint_fast8_t menu_is_scrolling = 0, int_rows = 35;
		static button_t * button_up = NULL, * button_down = NULL;
		window_t * win = & windows[WINDOW_MENU];

		if (win->first_call == 1)
		{
			win->first_call = 0;
			win->align_mode = ALIGN_CENTER_X;						// выравнивание окна системных настроек всегда по центру
			calculate_window_position(win);

			hamradio_set_menu_cond(VISIBLE);

			uint_fast8_t col1_int = 50, row1_int = 40, int_cols = 200, id_start, id_end, i = 0;
			uint_fast16_t xn, yn;
			label_t * lh;

			find_entry_area_elements(TYPE_LABEL, win, & id_start, & id_end);

			button_up = find_gui_element_ref(TYPE_BUTTON, win, "btnSysMenu+");
			button_down = find_gui_element_ref(TYPE_BUTTON, win, "btnSysMenu-");
			button_up->visible = NON_VISIBLE;
			button_down->visible = NON_VISIBLE;

			menu[MENU_GROUPS].add_id = 0;
			menu[MENU_GROUPS].selected_str = 0;
			menu[MENU_GROUPS].selected_label = 0;
			menu[MENU_PARAMS].add_id = 0;
			menu[MENU_PARAMS].selected_str = 0;
			menu[MENU_PARAMS].selected_label = 0;
			menu[MENU_VALS].add_id = 0;
			menu[MENU_VALS].selected_str = 0;
			menu[MENU_VALS].selected_label = 0;

			menu[MENU_GROUPS].first_id = id_start;
			for (i = id_start; i <= id_end; i++)
			{
				lh = touch_elements[i].link;
				if(strcmp(lh->name, "lbl_group"))
					break;
			}

			menu[MENU_GROUPS].last_id = --i;
			menu[MENU_GROUPS].num_rows = menu[MENU_GROUPS].last_id - menu[MENU_GROUPS].first_id;

			menu[MENU_PARAMS].first_id = ++i;
			for (; i <= id_end; i++)
			{
				lh = touch_elements[i].link;
				if(strcmp(lh->name, "lbl_params"))
					break;
			}
			menu[MENU_PARAMS].last_id = --i;
			menu[MENU_PARAMS].num_rows = menu[MENU_PARAMS].last_id - menu[MENU_PARAMS].first_id;

			menu[MENU_VALS].first_id = ++i;
			for (; i <= id_end; i++)
			{
				lh = touch_elements[i].link;
				if(strcmp(lh->name, "lbl_vals"))
					break;
			}
			menu[MENU_VALS].last_id = --i;
			menu[MENU_VALS].num_rows = menu[MENU_VALS].last_id - menu[MENU_VALS].first_id;

			menu[MENU_GROUPS].count = hamradio_get_multilinemenu_block_groups(menu[MENU_GROUPS].menu_block) - 1;
			xn = win->x1 + col1_int;
			yn = win->y1 + row1_int;
			for(i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
			{
				lh = touch_elements[menu[MENU_GROUPS].first_id + i].link;
				strcpy(lh->text, menu[MENU_GROUPS].menu_block[i + menu[MENU_GROUPS].add_id].name);
				lh->visible = VISIBLE;
				lh->color = COLORMAIN_WHITE;
				lh->x = xn;
				lh->y = yn;
				yn += int_rows;
			}

			menu[MENU_PARAMS].count = hamradio_get_multilinemenu_block_params(menu[MENU_PARAMS].menu_block, menu[MENU_GROUPS].menu_block[menu[MENU_GROUPS].selected_str].index) - 1;
			xn += int_cols;
			yn = win->y1 + row1_int;
			for(i = 0; i <= menu[MENU_PARAMS].num_rows; i++)
			{
				lh = touch_elements[menu[MENU_PARAMS].first_id + i].link;
				strcpy(lh->text, menu[MENU_PARAMS].menu_block[i + menu[MENU_PARAMS].add_id].name);
				lh->visible = VISIBLE;
				lh->color = COLORMAIN_WHITE;
				lh->x = xn;
				lh->y = yn;
				yn += int_rows;
			}

			menu[MENU_VALS].count = menu[MENU_PARAMS].count < menu[MENU_VALS].num_rows ? menu[MENU_PARAMS].count :  menu[MENU_VALS].num_rows;
			hamradio_get_multilinemenu_block_vals(menu[MENU_VALS].menu_block, menu[MENU_PARAMS].menu_block[menu[MENU_PARAMS].selected_str].index, menu[MENU_VALS].count);
			xn += int_cols;
			yn = win->y1 + row1_int;
			for(i = 0; i <= menu[MENU_VALS].num_rows; i++)
			{
				lh = touch_elements[menu[MENU_VALS].first_id + i].link;
				lh->x = xn;
				lh->y = yn;
				yn += int_rows;
				lh->visible = NON_VISIBLE;
				lh->color = COLORMAIN_WHITE;
				if (menu[MENU_VALS].count < i)
					continue;
				strcpy(lh->text, menu[MENU_VALS].menu_block[i + menu[MENU_VALS].add_id].name);
				lh->visible = VISIBLE;
			}

			menu_level = MENU_GROUPS;
			return;
		}

		if(gui.is_tracking && gui.selected_type == TYPE_LABEL && gui.vector_move_y != 0)
		{
			static uint_fast8_t start_str_group = 0, start_str_params = 0;
			if (! menu_is_scrolling)
			{
				start_str_group = menu[MENU_GROUPS].add_id;
				start_str_params = menu[MENU_PARAMS].add_id;
			}
			ldiv_t r = ldiv(gui.vector_move_y, int_rows);
			if(strcmp(((label_t *) gui.selected_link->link)->name, "lbl_group") == 0)
			{
				int_fast8_t q = start_str_group - r.quot;
				menu[MENU_GROUPS].add_id = q <= 0 ? 0 : q;
				menu[MENU_GROUPS].add_id = (menu[MENU_GROUPS].add_id + menu[MENU_GROUPS].num_rows) > menu[MENU_GROUPS].count ?
						(menu[MENU_GROUPS].count - menu[MENU_GROUPS].num_rows) : menu[MENU_GROUPS].add_id;
				menu[MENU_GROUPS].selected_str = menu[MENU_GROUPS].selected_label + menu[MENU_GROUPS].add_id;
				menu_level = MENU_GROUPS;
				menu[MENU_PARAMS].add_id = 0;
				menu[MENU_PARAMS].selected_str = 0;
				menu[MENU_PARAMS].selected_label = 0;
				menu[MENU_VALS].add_id = 0;
				menu[MENU_VALS].selected_str = 0;
				menu[MENU_VALS].selected_label = 0;
			}
			else if(strcmp(((label_t *) gui.selected_link->link)->name, "lbl_params") == 0 &&
					menu[MENU_PARAMS].count > menu[MENU_PARAMS].num_rows)
			{
				int_fast8_t q = start_str_params - r.quot;
				menu[MENU_PARAMS].add_id = q <= 0 ? 0 : q;
				menu[MENU_PARAMS].add_id = (menu[MENU_PARAMS].add_id + menu[MENU_PARAMS].num_rows) > menu[MENU_PARAMS].count ?
						(menu[MENU_PARAMS].count - menu[MENU_PARAMS].num_rows) : menu[MENU_PARAMS].add_id;
				menu[MENU_PARAMS].selected_str = menu[MENU_PARAMS].selected_label + menu[MENU_PARAMS].add_id;
				menu[MENU_VALS].add_id = menu[MENU_PARAMS].add_id;
				menu[MENU_VALS].selected_str = menu[MENU_PARAMS].selected_str;
				menu[MENU_VALS].selected_label = menu[MENU_PARAMS].selected_label;
				menu_level = MENU_PARAMS;
			}
			menu_is_scrolling = 1;
		}

		if(! gui.is_tracking && menu_is_scrolling)
		{
			menu_is_scrolling = 0;
			reset_tracking();
		}

		if (! encoder2.press_done || menu_label_touched || menu_is_scrolling)
		{
			// выход из режима редактирования параметра  - краткое или длинное нажатие на энкодер
			if (encoder2.press && menu_level == MENU_VALS)
			{
				menu_level = MENU_PARAMS;
				encoder2.press = 0;
			}
			if (encoder2.press)
				menu_level = ++menu_level > MENU_VALS ? MENU_VALS : menu_level;
			if (encoder2.hold)
			{
				menu_level = --menu_level == MENU_OFF ? MENU_OFF : menu_level;
				if (menu_level == MENU_GROUPS)
				{
					menu[MENU_PARAMS].add_id = 0;
					menu[MENU_PARAMS].selected_str = 0;
					menu[MENU_PARAMS].selected_label = 0;
					menu[MENU_VALS].add_id = 0;
					menu[MENU_VALS].selected_str = 0;
					menu[MENU_VALS].selected_label = 0;
				}
			}

			// при переходе на следующий уровень пункт меню подсвечивается
			label_t * lh = NULL;
			if (menu_level == MENU_VALS)
			{
				menu[MENU_VALS].selected_label = menu[MENU_PARAMS].selected_label;
				lh = touch_elements[menu[MENU_VALS].first_id + menu[MENU_VALS].selected_label].link;

				button_down->visible = VISIBLE;
				button_down->x1 = lh->x - button_down->w - 10;
				button_down->y1 = (lh->y + get_label_height(lh) / 2) - (button_down->h / 2);

				button_up->visible = VISIBLE;
				button_up->x1 = lh->x + get_label_width(lh) + 10;
				button_up->y1 = button_down->y1;
				for (uint8_t i = 0; i < menu[MENU_GROUPS].num_rows; i++)
				{
					lh = touch_elements[menu[MENU_GROUPS].first_id + i].link;
					lh->color = i == menu[MENU_GROUPS].selected_label ? COLORPIP_YELLOW : COLORPIP_GRAY;

					lh = touch_elements[menu[MENU_PARAMS].first_id + i].link;
					lh->color = i == menu[MENU_PARAMS].selected_label ? COLORPIP_YELLOW : COLORPIP_GRAY;

					lh = touch_elements[menu[MENU_VALS].first_id + i].link;
					lh->color = i == menu[MENU_PARAMS].selected_label ? COLORPIP_YELLOW : COLORPIP_GRAY;
				}
				menu_label_touched = 0;
			}
			else if (menu_level == MENU_PARAMS)
			{
				button_down->visible = NON_VISIBLE;
				button_up->visible = NON_VISIBLE;
				for (uint8_t i = 0; i < menu[MENU_GROUPS].num_rows; i++)
				{
					lh = touch_elements[menu[MENU_GROUPS].first_id + i].link;
					lh->color = i == menu[MENU_GROUPS].selected_label ? COLORPIP_YELLOW : COLORPIP_GRAY;

					lh = touch_elements[menu[MENU_PARAMS].first_id + i].link;
					lh->color = COLORPIP_WHITE;

					lh = touch_elements[menu[MENU_VALS].first_id + i].link;
					lh->color = COLORPIP_WHITE;
				}
			}
			else if (menu_level == MENU_GROUPS)
			{
				button_down->visible = NON_VISIBLE;
				button_up->visible = NON_VISIBLE;
				for (uint8_t i = 0; i < menu[MENU_GROUPS].num_rows; i++)
				{
					lh = touch_elements[menu[MENU_GROUPS].first_id + i].link;
					lh->color = COLORPIP_WHITE;

					lh = touch_elements[menu[MENU_PARAMS].first_id + i].link;
					lh->color = COLORPIP_WHITE;

					lh = touch_elements[menu[MENU_VALS].first_id + i].link;
					lh->color = COLORPIP_WHITE;
				}
			}

			encoder2.press = 0;
			encoder2.hold = 0;
			encoder2.press_done = 1;
		}

		if (menu_level == MENU_OFF)
		{
			set_window(win, NON_VISIBLE);
			encoder2.busy = 0;
			footer_buttons_state(CANCELLED);
			hamradio_set_menu_cond(NON_VISIBLE);
			return;
		}

		if (encoder2.rotate != 0 && menu_level == MENU_VALS)
		{
			encoder2.rotate_done = 1;
			menu[MENU_PARAMS].selected_str = menu[MENU_PARAMS].selected_label + menu[MENU_PARAMS].add_id;
			label_t * lh = touch_elements[menu[MENU_VALS].first_id + menu[MENU_PARAMS].selected_label].link;
			strcpy(lh->text, hamradio_gui_edit_menu_item(menu[MENU_PARAMS].menu_block[menu[MENU_PARAMS].selected_str].index, encoder2.rotate));

			lh = touch_elements[menu[MENU_VALS].first_id + menu[MENU_VALS].selected_label].link;
			button_up->x1 = lh->x + get_label_width(lh) + 10;
		}

		if ((menu_label_touched || menu_is_scrolling || encoder2.rotate != 0) && menu_level != MENU_VALS)
		{
			encoder2.rotate_done = 1;

			if (encoder2.rotate != 0)
			{
				menu[menu_level].selected_str = (menu[menu_level].selected_str + encoder2.rotate) <= 0 ? 0 : (menu[menu_level].selected_str + encoder2.rotate);
				menu[menu_level].selected_str = menu[menu_level].selected_str > menu[menu_level].count ? menu[menu_level].count : menu[menu_level].selected_str;
			}
			else if (menu_label_touched)
				menu[menu_level].selected_str = menu[menu_level].selected_label + menu[menu_level].add_id;

			menu[MENU_PARAMS].count = hamradio_get_multilinemenu_block_params(menu[MENU_PARAMS].menu_block, menu[MENU_GROUPS].menu_block[menu[MENU_GROUPS].selected_str].index) - 1;

			if (encoder2.rotate > 0)
			{
				// указатель подошел к нижней границе списка
				if (++menu[menu_level].selected_label > (menu[menu_level].count < menu[menu_level].num_rows ? menu[menu_level].count : menu[menu_level].num_rows))
				{
					menu[menu_level].selected_label = (menu[menu_level].count < menu[menu_level].num_rows ? menu[menu_level].count : menu[menu_level].num_rows);
					menu[menu_level].add_id = menu[menu_level].selected_str - menu[menu_level].selected_label;
				}
			}
			if (encoder2.rotate < 0)
			{
				// указатель подошел к верхней границе списка
				if (--menu[menu_level].selected_label < 0)
				{
					menu[menu_level].selected_label = 0;
					menu[menu_level].add_id = menu[menu_level].selected_str;
				}
			}

			if (menu_level == MENU_GROUPS)
				for(uint_fast8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
				{
					label_t * l = touch_elements[menu[MENU_GROUPS].first_id + i].link;
					strcpy(l->text, menu[MENU_GROUPS].menu_block[i + menu[MENU_GROUPS].add_id].name);
				}

			menu[MENU_VALS].count = menu[MENU_PARAMS].count < menu[MENU_VALS].num_rows ? menu[MENU_PARAMS].count : menu[MENU_VALS].num_rows;
			hamradio_get_multilinemenu_block_vals(menu[MENU_VALS].menu_block,  menu[MENU_PARAMS].menu_block[menu[MENU_PARAMS].add_id].index, menu[MENU_VALS].count);

			for(uint_fast8_t i = 0; i <= menu[MENU_PARAMS].num_rows; i++)
			{
				label_t * lp = touch_elements[menu[MENU_PARAMS].first_id + i].link;
				label_t * lv = touch_elements[menu[MENU_VALS].first_id + i].link;

				lp->visible = NON_VISIBLE;
				lp->state = DISABLED;
				lv->visible = NON_VISIBLE;
				lv->state = DISABLED;
				if (i > menu[MENU_PARAMS].count)
					continue;
				strcpy(lp->text, menu[MENU_PARAMS].menu_block[i + menu[MENU_PARAMS].add_id].name);
				strcpy(lv->text,   menu[MENU_VALS].menu_block[i].name);
				lp->visible = VISIBLE;
				lp->state = CANCELLED;
				lv->visible = VISIBLE;
				lv->state = CANCELLED;
			}
			menu_label_touched = 0;
		}
		if (menu_level != MENU_VALS)
		{
			label_t * l = touch_elements[menu[menu_level].selected_label + menu[menu_level].first_id].link;
			colpip_string_tbg(colmain_fb_draw(), DIM_X, DIM_Y, l->x - SMALLCHARW, l->y, ">", COLORPIP_YELLOW);
		}
	}

#else
	static void labels_menu_handler (void)
	{

	}

	static void buttons_menu_handler(void)
	{

	}

	static void window_menu_process(void)
	{
		window_t * win = & windows[WINDOW_MENU];
		if (win->first_call == 1)
		{
			win->align_mode = ALIGN_CENTER_X;						// выравнивание окна системных настроек только по центру
			calculate_window_position(win);

			win->first_call = 0;
		}

	}
#endif
	uint_fast8_t gui_check_encoder2 (int_least16_t rotate)
	{
		if (encoder2.rotate_done || encoder2.rotate == 0)
		{
			encoder2.rotate = rotate;
			encoder2.rotate_done = 0;
		}
		return encoder2.busy;
	}

	void gui_set_encoder2_state (uint_fast8_t code)
	{
		if (code == KBD_ENC2_PRESS)
			encoder2.press = 1;
		if (code == KBD_ENC2_HOLD)
			encoder2.hold = 1;
		encoder2.press_done = 0;
	}

	static void remove_end_line_spaces(char * str)
	{
		size_t i = strlen(str);
		if (i == 0)
			return;
		for (; -- i > 0;)
		{
			if (str [i] != ' ')
				break;
		}
		str [i + 1] = '\0';
	}

	void gui_encoder2_menu (enc2_menu_t * enc2_menu)
	{
		window_t * win = & windows[WINDOW_ENC2];
		if (win->state == NON_VISIBLE && enc2_menu->state != 0)
		{
			set_window(win, VISIBLE);
			footer_buttons_state(DISABLED, "");
			gui_enc2_menu = enc2_menu;
		}
		else if (win->state == VISIBLE && enc2_menu->state == 0)
		{
			set_window(win, NON_VISIBLE);
			gui_enc2_menu = NULL;
			footer_buttons_state(CANCELLED);
		}
	}

	static void window_enc2_process(void)
	{
		static label_t * lbl_param,  * lbl_val;
		static uint_fast16_t window_center_x;
		window_t * win = & windows[WINDOW_ENC2];


		if (win->first_call == 1)
		{
			calculate_window_position(win);
			window_center_x =  win->x1 + win->w / 2;
			win->first_call = 0;
			lbl_param = find_gui_element_ref(TYPE_LABEL, win, "lbl_enc2_param");
			lbl_val = find_gui_element_ref(TYPE_LABEL, win, "lbl_enc2_val");
			lbl_param->y = win->y1 + get_label_height(lbl_param) * 3;
			lbl_val->y = lbl_param->y + get_label_height(lbl_val) * 2;
			lbl_param->visible = VISIBLE;
			lbl_val->visible = VISIBLE;
			return;
		}
		if(gui_enc2_menu->updated)
		{
			strcpy(lbl_param->text, gui_enc2_menu->param);
			remove_end_line_spaces(lbl_param->text);
			strcpy(lbl_val->text, gui_enc2_menu->val);
			lbl_val->color = gui_enc2_menu->state == 2 ? COLORPIP_YELLOW : COLORPIP_WHITE;

			lbl_param->x = window_center_x - get_label_width(lbl_param) / 2;
			lbl_val->x = window_center_x - get_label_width(lbl_val) / 2;

			gui_enc2_menu->updated = 0;
			gui.timer_1sec_updated = 1;
		}
	}

	static void buttons_mode_handler(void)
	{
		window_t * win = & windows[WINDOW_MODES];
		if(gui.selected_type == TYPE_BUTTON)
		{
			if (win->state && ((button_t *)gui.selected_link)->parent == win->window_id)
			{
				if (((button_t *)gui.selected_link)->payload != UINTPTR_MAX)
					hamradio_change_submode(((button_t *)gui.selected_link)->payload);

				set_window(win, NON_VISIBLE);
				footer_buttons_state(CANCELLED);
			}
		}
	}

	static void window_mode_process(void)
	{
		window_t * win = & windows[WINDOW_MODES];
		if (win->first_call == 1)
		{
			uint_fast16_t x, y;
			uint_fast8_t interval = 6, id = 0, col1_int = 20, row1_int = 40;
			uint_fast8_t id_start, id_end;
			win->first_call = 0;
			calculate_window_position(win);
			find_entry_area_elements(TYPE_BUTTON, win, & id_start, & id_end);

			x = win->x1 + col1_int;
			y = win->y1 + row1_int;
			for (uint_fast8_t id = id_start; id <= id_end; id++)
			{
				button_t * bh = (button_t *) touch_elements[id].link;
				bh->x1 = x;
				bh->y1 = y;
				bh->visible = VISIBLE;
				x = x + interval + bh->w;
				if (x + bh->w > win->x1 + win->w)
				{
					x = win->x1 + col1_int;
					y = win->y1 + row1_int + bh->h + interval;
				}
			}
			return;
		}
	}

	static void window_agc_process(void)
	{
		window_t * win = & windows[WINDOW_AGC];
		if (win->first_call == 1)
		{
			uint_fast16_t x, y;
			uint_fast8_t interval = 40, col1_int = 20, row1_int = 40;
			uint_fast8_t id = 0, id_start, id_end;
			win->first_call = 0;
			calculate_window_position(win);

			find_entry_area_elements(TYPE_BUTTON, win, & id_start, & id_end);

			x = win->x1 + col1_int;
			y = win->y1 + row1_int;
			for (uint_fast8_t id = id_start; id <= id_end; id++)
			{
				button_t * bh = (button_t *) touch_elements[id].link;
				bh->x1 = x;
				bh->y1 = y;
				bh->visible = VISIBLE;
				x = x + interval + bh->w;
				if (x + bh->w > win->x1 + win->w)
				{
					x = win->x1 + col1_int;
					y = win->y1 + row1_int + bh->h + interval;
				}
			}
			return;
		}
	}

	static void window_audioparams_process(void)
	{
		PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
		window_t * win = & windows[WINDOW_AUDIO];
		slider_t * sl = NULL;
		label_t * lbl = NULL;
		static uint_fast8_t eq_limit, eq_base = 0;
		char buf[TEXT_ARRAY_SIZE];
		static int_fast16_t mid_y = 0, start_y = 0;
		static uint_fast8_t id = 0, id_start, id_end;

		if (win->first_call == 1)
		{
			uint_fast16_t x, y, mid_w;
			uint_fast8_t interval = 70, col1_int = 70, row1_int = 120;
			win->first_call = 0;
			calculate_window_position(win);
			find_entry_area_elements(TYPE_SLIDER, win, & id_start, & id_end);

			eq_base = hamradio_getequalizerbase();
			eq_limit = abs(eq_base) * 2;

			x = win->x1 + col1_int;
			y = win->y1 + row1_int;

			for (id = id_start; id <= id_end; id++)
			{
				sl = (slider_t *) touch_elements[id].link;

				sl->x = x;
				sl->y = y;
				sl->size = 200;
				sl->step = 2;
				sl->value = normalize(hamradio_get_gmikeequalizerparams(id - id_start), eq_limit, 0, 100);
				sl->visible = VISIBLE;

				mid_w = sl->x + sliders_width / 2;		// центр шкалы слайдера по x

				local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("lbl_%s_name"), sl->name);
				lbl = find_gui_element_ref(TYPE_LABEL, win, buf);
				local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%sk"), strchr(sl->name, 'q') + 1);
				lbl->x = mid_w - get_label_width(lbl) / 2;
				lbl->y = sl->y - get_label_height(lbl) * 4;
				lbl->visible = VISIBLE;

				local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("lbl_%s_val"), sl->name);
				lbl = find_gui_element_ref(TYPE_LABEL, win, buf);
				local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%d"), hamradio_get_gmikeequalizerparams(id - id_start) + eq_base);
				lbl->x = mid_w - get_label_width(lbl) / 2;
				lbl->y = sl->y - get_label_height(lbl) * 3 + get_label_height(lbl) / 2;
				lbl->visible = VISIBLE;

				x = x + interval;
			}
			mid_y = sl->y + sl->size / 2;
			return;
		}

		if (gui.selected_type == TYPE_SLIDER)
		{
			/* костыль через костыль */
			sl = gui.selected_link->link;
			uint_fast8_t id = gui.selected_link->pos;
			PRINTF("%d\n", id);

			hamradio_set_gmikeequalizerparams(id, normalize(sl->value, 100, 0, eq_limit));

			uint_fast16_t mid_w = sl->x + sliders_width / 2;
			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("lbl_%s_val"), sl->name);
			lbl = find_gui_element_ref(TYPE_LABEL, win, buf);
			local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%d"), hamradio_get_gmikeequalizerparams(id) + eq_base);
			lbl->x = mid_w - get_label_width(lbl) / 2;
		}

		for (uint_fast16_t i = 0; i <= abs(eq_base); i += 3)
		{
			uint_fast16_t yy = normalize(i, 0, abs(eq_base), 100);
			colmain_line(fr, DIM_X, DIM_Y, win->x1 + 50, mid_y + yy, win->x1 + win->w - 50, mid_y + yy, 225, 0);
			local_snprintf_P(buf, ARRAY_SIZE(buf), i == 0 ? PSTR("%d") : PSTR("-%d"), i);
			colpip_string2_tbg(fr, DIM_X, DIM_Y, win->x1 + 50 - strwidth2(buf) - 5, mid_y + yy - SMALLCHARH2 / 2, buf, COLORMAIN_WHITE);

			if (i == 0)
				continue;
			colmain_line(fr, DIM_X, DIM_Y, win->x1 + 50, mid_y - yy, win->x1 + win->w - 50, mid_y - yy, 225, 0);
			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), i);
			colpip_string2_tbg(fr, DIM_X, DIM_Y, win->x1 + 50 - strwidth2(buf) - 5, mid_y - yy - SMALLCHARH2 / 2, buf, COLORMAIN_WHITE);
		}
	}

	void gui_open_sys_menu(void)
	{
		button9_handler();
	}

	static void button1_handler(void)
	{
		if(gui.selected_type == TYPE_BUTTON)
		{
			window_t * win = & windows[WINDOW_MODES];
			if (win->state == NON_VISIBLE)
			{
				set_window(win, VISIBLE);
				footer_buttons_state(DISABLED, ((button_t *)gui.selected_link)->name);
			}
			else
			{
				set_window(win, NON_VISIBLE);
				footer_buttons_state(CANCELLED);
			}
		}
	}

	static void button2_handler(void)
	{
		if(gui.selected_type == TYPE_BUTTON)
		{
			window_t * win = & windows[WINDOW_BP];
			if (win->state == NON_VISIBLE)
			{
				encoder2.busy = 1;
				set_window(win, VISIBLE);
				footer_buttons_state(DISABLED, ((button_t *)gui.selected_link)->name);
				hamradio_enable_keyboard_redirect();
			}
			else
			{
				set_window(win, NON_VISIBLE);
				encoder2.busy = 0;
				footer_buttons_state(CANCELLED);
				hamradio_disable_keyboard_redirect();
			}
		}
	}

	static void button3_handler(void)
	{
		if(gui.selected_type == TYPE_BUTTON)
		{
			window_t * win = & windows[WINDOW_AGC];
			if (win->state == NON_VISIBLE)
			{
				set_window(win, VISIBLE);
				footer_buttons_state(DISABLED, ((button_t *)gui.selected_link)->name);
			}
			else
			{
				set_window(win, NON_VISIBLE);
				footer_buttons_state(CANCELLED);
			}
		}
	}

	static void button4_handler(void)
	{
		if(gui.selected_type == TYPE_BUTTON)
		{
			window_t * win = & windows[WINDOW_FREQ];
			if (win->state == NON_VISIBLE)
			{
				set_window(win, VISIBLE);
				hamradio_set_lockmode(1);
				hamradio_enable_keyboard_redirect();
				footer_buttons_state(DISABLED, ((button_t *)gui.selected_link)->name);
			}
			else
			{
				set_window(win, NON_VISIBLE);
				hamradio_set_lockmode(0);
				hamradio_disable_keyboard_redirect();
				footer_buttons_state(CANCELLED);
			}
		}
	}

	static void button5_handler(void)
	{
		if(gui.selected_type == TYPE_BUTTON)
		{

		}

	}

	static void button6_handler(void)
	{
		if(gui.selected_type == TYPE_BUTTON)
		{

		}

	}

	static void button7_handler(void)
	{
		if(gui.selected_type == TYPE_BUTTON)
		{

		}
	}

	static void button8_handler(void)
	{
		if(gui.selected_type == TYPE_BUTTON)
		{
			window_t * win = & windows[WINDOW_AUDIO];
			if (win->state == NON_VISIBLE)
			{
				set_window(win, VISIBLE);
				footer_buttons_state(DISABLED, ((button_t *)gui.selected_link)->name);
			}
			else
			{
				set_window(win, NON_VISIBLE);
				footer_buttons_state(CANCELLED);
				hamradio_set_menu_cond(NON_VISIBLE);
			}
		}
	}

	static void button9_handler(void)
	{
		if(gui.selected_type == TYPE_BUTTON)
		{
			window_t * win = & windows[WINDOW_MENU];
			if (win->state == NON_VISIBLE)
			{
				set_window(win, VISIBLE);
				footer_buttons_state(DISABLED, ((button_t *)gui.selected_link)->name);
				encoder2.busy = 1;
			}
			else
			{
				set_window(win, NON_VISIBLE);
				footer_buttons_state(CANCELLED);
				encoder2.busy = 0;
				hamradio_set_menu_cond(NON_VISIBLE);
			}
		}
	}

	static void gui_main_process(void)
	{
		window_t * win = & windows[WINDOW_MAIN];
		PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
		char buf [TEXT_ARRAY_SIZE];
		const uint_fast8_t buflen = ARRAY_SIZE(buf);
		uint_fast16_t yt, xt;

		if (win->first_call == 1)
		{
			uint_fast8_t interval = 3, id_start, id_end;
			uint_fast16_t x = 0;
			win->first_call = 0;

			find_entry_area_elements(TYPE_BUTTON, win, & id_start, & id_end);
			for (uint_fast8_t id = id_start; id <= id_end; id ++)
			{
				button_t * bh = (button_t *) touch_elements[id].link;
				bh->x1 = x;
				bh->y1 = WITHGUIMAXY - bh->h;
				bh->visible = VISIBLE;
				x = x + interval + bh->w;
			}
		}

		uint_fast8_t interval = 15, len1 = 0, len2 = 0;
		uint_fast16_t x = 10, y1 = 125, y2 = 145;			// пока абсолютные, переделать на относительные

		// текущее время
	#if defined (RTC1_TYPE)
		static uint_fast16_t year;
		static uint_fast8_t month, day, hour, minute, secounds;
		if(gui.timer_1sec_updated)
			board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & secounds);
		local_snprintf_P(buf, buflen, PSTR("%02d.%02d"), day, month);
		len1 = strwidth2(buf);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, x, y1, buf, COLORPIP_WHITE);
		local_snprintf_P(buf, buflen, PSTR("%02d%c%02d"), hour, ((secounds & 1) ? ' ' : ':'), minute);
		len2 = strwidth2(buf);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, x, y2, buf, COLORPIP_WHITE);
	#endif 	/* defined (RTC1_TYPE) */

		x = x + (len1 > len2 ? len1 : len2) + interval;		// при изменении ширины выводимой строки infobar может дергаться, переделать
		len1 = len2 = 0;									// добавить выравнивание по ячейкам

		// напряжение питания
	#if WITHVOLTLEVEL
		static ldiv_t v;
		if(gui.timer_1sec_updated)
			v = ldiv(hamradio_get_volt_value(), 10);
		local_snprintf_P(buf, buflen, PSTR("%d.%1dV"), v.quot, v.rem);
		len1 = strwidth2(buf);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, x, y1, buf, COLORPIP_WHITE);
	#endif /* WITHVOLTLEVEL */

		// ток PA (при передаче)
	#if WITHCURRLEVEL
		if (hamradio_get_tx())
		{
			static int_fast16_t drain;
			if (gui.timer_1sec_updated)
			{
				drain = hamradio_get_pacurrent_value();	// Ток в десятках милиампер (может быть отрицательным)
				if (drain < 0)
				{
					drain = 0;	// FIXME: без калибровки нуля (как у нас сейчас) могут быть ошибки установки тока
				}
			}

		#if (WITHCURRLEVEL_ACS712_30A || WITHCURRLEVEL_ACS712_20A)
			// для больших токов (более 9 ампер)
			ldiv_t t = ldiv(drain / 10, 10);
			local_snprintf_P(buf, buflen, PSTR("%2d.%01dA"), t.quot, t.rem);

		#else /* (WITHCURRLEVEL_ACS712_30A || WITHCURRLEVEL_ACS712_20A) */
			// Датчик тока до 5 ампер
			ldiv_t t = ldiv(drain, 100);
			local_snprintf_P(buf, buflen, PSTR("%d.%02dA"), t.quot, t.rem);

		#endif /* (WITHCURRLEVEL_ACS712_30A || WITHCURRLEVEL_ACS712_20A) */

			len2 = strwidth2(buf);
			colpip_string2_tbg(fr, DIM_X, DIM_Y, x, y2, buf, COLORPIP_WHITE);
		}
	#endif /* WITHCURRLEVEL */

		x = x + (len1 > len2 ? len1 : len2) + interval;
		len1 = len2 = 0;

		// ширина панорамы
	#if WITHIF4DSP
		static int_fast32_t z;
		if(gui.timer_1sec_updated)
			z = display_zoomedbw() / 1000;
		local_snprintf_P(buf, buflen, PSTR("SPAN"));
		len1 = strwidth2(buf);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, x, y1, buf, COLORPIP_WHITE);
		local_snprintf_P(buf, buflen, PSTR("%dk"), z);
		len2 = strwidth2(buf);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, x, y2, buf, COLORPIP_WHITE);
	#endif /* WITHIF4DSP */

		x = x + (len1 > len2 ? len1 : len2) + interval;
		len1 = len2 = 0;

		// параметры полосы пропускания фильтра
		static uint_fast8_t bp_type, bp_low, bp_high;
		if(gui.timer_1sec_updated)
		{
			bp_high = hamradio_get_high_bp(0);
			bp_low = hamradio_get_low_bp(0) * 10;
			bp_type = hamradio_get_bp_type();
			bp_high = bp_type ? bp_high * 100 : bp_high * 10;
		}
		local_snprintf_P(buf, buflen, PSTR("AF filter"));
		len1 = strwidth2(buf);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, x, y1, buf, COLORPIP_WHITE);
		local_snprintf_P(buf, buflen, bp_type ? (PSTR("%d..%d")) : (PSTR("W%d P%d")), bp_low, bp_high);
		len2 = strwidth2(buf);
		if (len2 > len1)
			colpip_string3_tbg(fr, DIM_X, DIM_Y, x, y2 , buf, COLORPIP_WHITE);
		else
			colpip_string2_tbg(fr, DIM_X, DIM_Y, x, y2, buf, COLORPIP_WHITE);
		len2 = len1;

		x = x + (len1 > len2 ? len1 : len2) + interval;
		len1 = len2 = 0;

		// значение сдвига частоты
		static int_fast16_t if_shitf;
		if (gui.timer_1sec_updated)
			if_shitf = hamradio_get_if_shift();
		local_snprintf_P(buf, buflen, PSTR("IF shift"));
		len1 = strwidth2(buf);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, x, y1, buf, COLORPIP_WHITE);
		local_snprintf_P(buf, buflen, if_shitf == 0 ? PSTR("%d") : PSTR("%+dk"), if_shitf);
		len2 = strwidth2(buf);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, x, y2, buf, COLORPIP_WHITE);

	#if WITHTHERMOLEVEL	// температура выходных транзисторов (при передаче)
		static ldiv_t t;
		if (hamradio_get_tx())// && gui.timer_1sec_updated)
		{
			t = ldiv(hamradio_get_temperature_value(), 10);
			local_snprintf_P(buf, buflen, PSTR("%d.%dC "), t.quot, t.rem);
			PRINTF("%s\n", buf);		// пока вывод в консоль
		}
	#endif /* WITHTHERMOLEVEL */

		gui.timer_1sec_updated = 0;
	}

	/* Кнопка */
	static void draw_button(const button_t * const bh)
	{
		PACKEDCOLORMAIN_T * bg = NULL;
		PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
		btn_bg_t * b1 = NULL;
		uint_fast8_t i = 0;
		static const char delimeters [] = "|";

		do {
			if (bh->h == btn_bg[i].h && bh->w == btn_bg[i].w)
			{
				b1 = & btn_bg[i];
				break;
			}
		} while (++i < BG_COUNT);

		if (b1 == NULL)				// если не найден заполненный буфер фона по размерам, программная отрисовка
		{
			PACKEDCOLORMAIN_T c1, c2;
			c1 = bh->state == DISABLED ? COLOR_BUTTON_DISABLED : (bh->is_locked ? COLOR_BUTTON_LOCKED : COLOR_BUTTON_NON_LOCKED);
			c2 = bh->state == DISABLED ? COLOR_BUTTON_DISABLED : (bh->is_locked ? COLOR_BUTTON_PR_LOCKED : COLOR_BUTTON_PR_NON_LOCKED);
			colpip_rect(fr, DIM_X, DIM_Y, bh->x1, bh->y1, bh->x1 + bh->w, bh->y1 + bh->h - 2, bh->state == PRESSED ? c2 : c1, 1);
			colpip_rect(fr, DIM_X, DIM_Y, bh->x1, bh->y1, bh->x1 + bh->w, bh->y1 + bh->h - 1, COLORPIP_GRAY, 0);
			colpip_rect(fr, DIM_X, DIM_Y, bh->x1 + 2, bh->y1 + 2, bh->x1 + bh->w - 2, bh->y1 + bh->h - 3, COLORPIP_BLACK, 0);
		}
		else
		{
			if (bh->state == DISABLED)
				bg = b1->bg_disabled;
			else if (bh->is_locked && bh->state == PRESSED)
				bg = b1->bg_locked_pressed;
			else if (bh->is_locked && bh->state != PRESSED)
				bg = b1->bg_locked;
			else if (! bh->is_locked && bh->state == PRESSED)
				bg = b1->bg_pressed;
			else if (! bh->is_locked && bh->state != PRESSED)
				bg = b1->bg_non_pressed;

			colpip_plot(fr, DIM_X, DIM_Y, bh->x1, bh->y1, bg, bh->w, bh->h);
		}

		if (strchr(bh->text, delimeters[0]) == NULL)
		{
			/* Однострочная надпись */
			colpip_string2_tbg(fr, DIM_X, DIM_Y, bh->x1 + (bh->w - (strwidth2(bh->text))) / 2,
					bh->y1 + (bh->h - SMALLCHARH2) / 2, bh->text, COLORPIP_BLACK);
		} else
		{
			/* Двухстрочная надпись */
			uint_fast8_t j = (bh->h - SMALLCHARH2 * 2) / 2;
			char buf [TEXT_ARRAY_SIZE];
			strcpy(buf, bh->text);
			char * text2 = strtok(buf, delimeters);
			colpip_string2_tbg(fr, DIM_X, DIM_Y, bh->x1 + (bh->w - (strwidth2(text2))) / 2,
					bh->y1 + j, text2, COLORPIP_BLACK);

			text2 = strtok(NULL, delimeters);
			colpip_string2_tbg(fr, DIM_X, DIM_Y, bh->x1 + (bh->w - (strwidth2(text2))) / 2,
					bh->h + bh->y1 - SMALLCHARH2 - j, text2, COLORPIP_BLACK);
		}
	}

	static void draw_slider(slider_t * sl)
	{
		PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();

		if (sl->orientation)		// ORIENTATION_HORIZONTAL
		{
			colpip_rect(fr, DIM_X, DIM_Y, sl->x, sl->y, sl->x + sl->size, sl->y + sliders_width, COLORMAIN_WHITE, 0);		// в процессе
		}
		else						// ORIENTATION_VERTICAL
		{
			if (sl->value_old != sl->value)
			{
				uint_fast16_t mid_w = sl->x + sliders_width / 2;
				sl->value_p = sl->y + sl->size * sl->value / 100;
				sl->x1_p = mid_w - sliders_w;
				sl->y1_p = sl->value_p - sliders_h;
				sl->x2_p = mid_w + sliders_w;
				sl->y2_p = sl->value_p + sliders_h;
				sl->value_old = sl->value;
			}
			colpip_rect(fr, DIM_X, DIM_Y, sl->x + 1, sl->y + 1, sl->x + sliders_width - 1, sl->y + sl->size - 1, 242, 1);
			colpip_rect(fr, DIM_X, DIM_Y, sl->x, sl->y, sl->x + sliders_width, sl->y + sl->size, COLORMAIN_WHITE, 0);
			colpip_rect(fr, DIM_X, DIM_Y, sl->x1_p, sl->y1_p, sl->x2_p, sl->y2_p, sl->state == PRESSED ? COLOR_BUTTON_PR_NON_LOCKED : COLOR_BUTTON_NON_LOCKED, 1);
			colmain_line(fr, DIM_X, DIM_Y,  sl->x1_p, sl->value_p, sl->x2_p, sl->value_p, COLORMAIN_WHITE, 0);
		}
	}

	static void fill_button_bg_buf(btn_bg_t * v)
	{
		PACKEDCOLORMAIN_T * buf;
		uint_fast8_t w, h;

		w = v->w;
		h = v->h;
		ASSERT(w < BTN_BUF_W);
		ASSERT(h < BTN_BUF_H);

		buf = v->bg_non_pressed;
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_NON_LOCKED, 1);
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_rect(buf, w, h, 2, 2, w - 3, h - 3, COLORPIP_BLACK, 0);

		buf = v->bg_pressed;
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_PR_NON_LOCKED, 1);
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_rect(buf, w, h, 2, 2, w - 3, h - 3, COLORPIP_BLACK, 0);

		buf = v->bg_locked;
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_LOCKED, 1);
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_rect(buf, w, h, 2, 2, w - 3, h - 3, COLORPIP_BLACK, 0);

		buf = v->bg_locked_pressed;
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_PR_LOCKED, 1);
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_rect(buf, w, h, 2, 2, w - 3, h - 3, COLORPIP_BLACK, 0);

		buf = v->bg_disabled;
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_DISABLED, 1);
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_rect(buf, w, h, 2, 2, w - 3, h - 3, COLORPIP_BLACK, 0);
	}

	void gui_initialize (void)
	{
		uint_fast8_t i = 0;
		window_t * win = & windows[WINDOW_MAIN];
		set_window(win, VISIBLE);

		do {
			fill_button_bg_buf(& btn_bg[i]);
		} while (++i < BG_COUNT) ;
	}

	static void update_touch(void)
	{
		for (uint_fast8_t i = 0; i < touch_count; i++)
		{
			touch_t * p = & touch_elements[i];
			if (p->type == TYPE_BUTTON)
			{
				button_t * bh = (button_t *) p->link;
				p->x1 = bh->x1;
				p->x2 = bh->x1 + bh->w;
				p->y1 = bh->y1;
				p->y2 = bh->y1 + bh->h;
				p->state = bh->state;
				p->visible = bh->visible;
				p->is_trackable = bh->is_trackable;
			}
			else if (p->type == TYPE_LABEL)
			{
				label_t * lh = (label_t *) p->link;
				p->x1 = lh->x;
				p->x2 = lh->x + get_label_width(lh);
				p->y1 = lh->y - get_label_height(lh);
				p->y2 = lh->y + get_label_height(lh) * 2;
				p->state = lh->state;
				p->visible = lh->visible;
				p->is_trackable = lh->is_trackable;
			}
			else if (p->type == TYPE_SLIDER)
			{
				slider_t * sh = (slider_t *) p->link;
				p->x1 = sh->x1_p;
				p->x2 = sh->x2_p;
				p->y1 = sh->y1_p;
				p->y2 = sh->y2_p;
				p->state = sh->state;
				p->visible = sh->visible;
				p->is_trackable = 1;
			}
		}
	}

	static void slider_process(slider_t * sl)
	{
		uint16_t v = sl->value + round(gui.vector_move_y / sl->step);
		if (v >= 0 && v <= sl->size / sl->step)
			sl->value = v;
		reset_tracking();
	}

	static void set_state_record(touch_t * val)
	{
		ASSERT(val != NULL);
		switch (val->type)
		{
			case TYPE_BUTTON:
				ASSERT(val->link != NULL);
				button_t * bh = (button_t *) val->link;
				gui.selected_type = TYPE_BUTTON;
				gui.selected_link = val;
				bh->state = val->state;
				if (bh->state == RELEASED)
					bh->onClickHandler();
				break;

			case TYPE_LABEL:
				ASSERT(val->link != NULL);
				label_t * lh = (label_t *) val->link;
				gui.selected_type = TYPE_LABEL;
				gui.selected_link = val;
				lh->state = val->state;
				if (lh->onClickHandler && lh->state == RELEASED)
					lh->onClickHandler();
				break;

			case TYPE_SLIDER:
				ASSERT(val->link != NULL);
				slider_t * sh = (slider_t *) val->link;
				gui.selected_type = TYPE_SLIDER;
				gui.selected_link = val;
				sh->state = val->state;
				if (sh->state == PRESSED)
					slider_process(sh);
				break;

			default:
				ASSERT(0);
				break;
		}
	}

	static void process_gui(void)
	{
		uint_fast16_t tx, ty;
		static uint_fast16_t x_old = 0, y_old = 0;
		static touch_t * p = NULL;

	#if defined (TSC1_TYPE)
		if (board_tsc_getxy(& tx, & ty))
		{
			gui.last_pressed_x = tx;
			gui.last_pressed_y = ty;
			gui.is_touching_screen = 1;
//			debug_printf_P(PSTR("last x/y=%d/%d\n"), gui.last_pressed_x, gui.last_pressed_y);
			update_touch();
		}
		else
	#endif /* defined (TSC1_TYPE) */
		{
			gui.is_touching_screen = 0;
			gui.is_after_touch = 0;
		}

		if (gui.state == CANCELLED && gui.is_touching_screen && ! gui.is_after_touch)
		{
			for (uint_fast8_t i = 0; i < touch_count; i++)
			{
				p = & touch_elements[i];

				if (p->x1 < gui.last_pressed_x && p->x2 > gui.last_pressed_x
				 && p->y1 < gui.last_pressed_y && p->y2 > gui.last_pressed_y
				 && p->state != DISABLED && p->visible == VISIBLE)
				{
					gui.state = PRESSED;
					break;
				}
			}
		}

		if (gui.is_tracking && ! gui.is_touching_screen)
		{
			gui.is_tracking = 0;
			reset_tracking();
			x_old = 0;
			y_old = 0;
		}

		if (gui.state == PRESSED)
		{
			ASSERT(p != NULL);
			if (p->is_trackable && gui.is_touching_screen)
			{
				gui.vector_move_x = x_old ? gui.vector_move_x + gui.last_pressed_x - x_old : 0; // проверить, нужно ли оставить накопление
				gui.vector_move_y = y_old ? gui.vector_move_y + gui.last_pressed_y - y_old : 0;

				if (gui.vector_move_x != 0 || gui.vector_move_y != 0)
				{
					gui.is_tracking = 1;
//					debug_printf_P(PSTR("move x: %d, move y: %d\n"), gui.vector_move_x, gui.vector_move_y);
				}
				p->state = PRESSED;
				set_state_record(p);

				x_old = gui.last_pressed_x;
				y_old = gui.last_pressed_y;
			}
			else if (p->x1 < gui.last_pressed_x && p->x2 > gui.last_pressed_x
			 && p->y1 < gui.last_pressed_y && p->y2 > gui.last_pressed_y && ! gui.is_after_touch)
			{
				if (gui.is_touching_screen)
				{
					p->state = PRESSED;
					set_state_record(p);
				}
				else
					gui.state = RELEASED;
			}
			else
			{
				gui.state = CANCELLED;
				p->state = CANCELLED;
				set_state_record(p);
				gui.is_after_touch = 1; 	// точка непрерывного нажатия вышла за пределы выбранного элемента, не поддерживающего tracking
			}
		}
		if (gui.state == RELEASED)
		{
			p->state = RELEASED;			// для запуска обработчика нажатия
			set_state_record(p);
			p->state = CANCELLED;
			set_state_record(p);
			gui.is_after_touch = 0;
			gui.state = CANCELLED;
			gui.is_tracking = 0;
		}
	}

	void gui_WM_walktrough(uint_fast8_t x, uint_fast8_t y, dctx_t * pctx)
	{
		uint_fast8_t alpha = DEFAULT_ALPHA; // на сколько затемнять цвета
		char buf [TEXT_ARRAY_SIZE];
		char * text2 = NULL;
		uint_fast8_t str_len = 0;
		PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();

		process_gui();

		for (uint_fast8_t i = 0; i < windows_count; i ++)
		{
			const window_t * const win = & windows[i];
			uint_fast8_t f = win->first_call;
			if (win->state == VISIBLE)
			{
				// при открытии окна рассчитываются экранные координаты самого окна и его child элементов
				if (! f)
				{
					display_transparency(win->x1, win->y1, win->x1 + win->w, win->y1 + win->h, alpha);
				}

				// запуск процедуры фоновой обработки для окна
				win->onVisibleProcess();

				if (! f)
				{
					// вывод заголовка окна
					colpip_string_tbg(fr, DIM_X, DIM_Y, win->x1 + 20, win->y1 + 10, win->name, COLORPIP_YELLOW);

					// отрисовка принадлежащих окну элементов

					for (uint_fast8_t i = 0; i < touch_count; i++)
					{
						touch_t * p = & touch_elements[i];

						if (p->type == TYPE_BUTTON)
						{
							button_t * bh = (button_t *) p->link;
							if (bh->visible && bh->parent == win->window_id)
								draw_button(bh);
						}
						else if (p->type == TYPE_LABEL)
						{
							label_t * lh = (label_t *) p->link;
							if (lh->visible && lh->parent == win->window_id)
							{
								if (lh->font_size == FONT_LARGE)
									colpip_string_tbg(fr, DIM_X, DIM_Y, lh->x, lh->y, lh->text, lh->color);
								else if (lh->font_size == FONT_MEDIUM)
									colpip_string2_tbg(fr, DIM_X, DIM_Y, lh->x, lh->y, lh->text, lh->color);
								else if (lh->font_size == FONT_SMALL)
									colpip_string3_tbg(fr, DIM_X, DIM_Y, lh->x, lh->y, lh->text, lh->color);
							}
						}
						else if (p->type == TYPE_SLIDER)
						{
							slider_t * sh = (slider_t *) p->link;
							if (sh->visible && sh->parent == win->window_id)
								draw_slider(sh);
						}
					}
				}
			}
		}
	}
#endif /* WITHTOUCHGUI */
