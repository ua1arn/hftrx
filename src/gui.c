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
	return isin(alpha + 90, r);
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
		int antialiasing,
		int style)			// 1 - растягивание по горизонтали
{
	int     x, y;
	int     x2, y2;

	x = xc + icos(gs, style ? r1 << 1 : r1);
	y = yc + isin(gs, r1);
	x2 = xc + icos(gs, style ? r2 << 1 : r2);
	y2 = yc + isin(gs, r2);

	colmain_line(buffer, bx, by, x, y, x2, y2, color, antialiasing);
}

void
polar_to_dek(
		uint_fast16_t xc,
		uint_fast16_t yc,
		uint_fast16_t gs,
		uint_fast16_t r,
		uint_fast16_t * x,
		uint_fast16_t * y,
		uint_fast8_t style)
{
	* x = xc + icos(gs, style ? r << 1 : r);
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
		int antialiasing,
		int style)			// 1 - растягивание по горизонтали
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
		vcos = icos(gs, style ? r << 1 : r);
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
		vcos = icos(ge, style ? r << 1 : r);
		x = xc + vcos;
		y = yc + vsin;

		colmain_line(buffer, bx, by, xo, yo, x, y, color, antialiasing); // рисовать линию
	}
}

/* Нарисовать прямоугольник со скругленными углами */
void colmain_rounded_rect(
		PACKEDCOLORMAIN_T * buffer,
		uint_fast16_t bx,	// ширина буфера
		uint_fast16_t by,	// высота буфера
		uint_fast16_t x1,
		uint_fast16_t y1,
		uint_fast16_t x2,
		uint_fast16_t y2,
		uint_fast8_t r,		// радиус закругления углов
		COLORMAIN_T color,
		uint_fast8_t fill
		)
{
	if (r == 0)
	{
		colpip_rect(buffer, bx, by, x1, y1, x2, y2, color, fill);
		return;
	}

	ASSERT(r << 1 < x2 - x1);
	ASSERT(r << 1 < y2 - y1);

	display_segm_buf(buffer, bx, by, x1 + r, y1 + r, 180, 270, r, 1, color, 1, 0); // up left
	display_segm_buf(buffer, bx, by, x2 - r, y1 + r, 270, 360, r, 1, color, 1, 0); // up right
	display_segm_buf(buffer, bx, by, x2 - r, y2 - r,   0,  90, r, 1, color, 1, 0); // down right
	display_segm_buf(buffer, bx, by, x1 + r, y2 - r,  90, 180, r, 1, color, 1, 0); // down left

	colmain_line(buffer, bx, by, x1 + r, y1, x2 - r, y1, color, 0); // up
	colmain_line(buffer, bx, by, x1, y1 + r, x1, y2 - r, color, 0); // left
	colmain_line(buffer, bx, by, x1 + r, y2, x2 - r, y2, color, 0); // down
	colmain_line(buffer, bx, by, x2, y1 + r, x2, y2 - r, color, 0); // right

	if (fill)
	{
		PACKEDCOLORMAIN_T * oldColor = colmain_mem_at(buffer, bx, by, x1 + r, y1 + r);
		display_floodfill(buffer, bx, by, x1 + (x2 - x1) / 2, y1 + r, color, * oldColor, 0);
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

static const uint_fast16_t swr_fullscale = (SWRMIN * 40 / 10) - SWRMIN;

uint_fast16_t get_swr(void)
{
	uint_fast16_t swr10; 		// swr10 = 0..30 for swr 1..4
	adcvalholder_t forward, reflected;

	forward = board_getswrmeter_unfiltered(& reflected, swrcalibr);

								// рассчитанное  значение
	if (forward < minforward)
		swr10 = 0;				// SWR=1
	else if (forward <= reflected)
		swr10 = swr_fullscale;		// SWR is infinite
	else
		swr10 = (forward + reflected) * SWRMIN / (forward - reflected) - SWRMIN;
	return swr10;
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
#if WITHTOUCHGUI
//	colmain_rounded_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H - 1, 5, COLORMAIN_WHITE, 0);
#endif

	for (p = 0, i = 0; i < ARRAY_SIZE(markersTX_pwr); ++ i, p += 10)
	{
		if (i % 2 == 0)
		{
			char buf [10];
			uint_fast16_t xx, yy;

			display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markersTX_pwr [i], smeter_params.r1, smeter_params.r1 + 8, smeter, 1, 1);
			polar_to_dek(xb, yb, markersTX_pwr [i], smeter_params.r1 + 6, & xx, & yy, 1);
			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%u"), p);

			colmain_setcolors(COLORMAIN_YELLOW, COLORMAIN_BLACK);
			colmain_string3_at_xy(bg, SM_BG_W, SM_BG_H, xx - strwidth3(buf) / 2, yy - pad2w3 + 1, buf);
		}
		else
			display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markersTX_pwr [i], smeter_params.r1, smeter_params.r1 + 4, smeter, 1, 1);
	}

	for (p = 1, i = 0; i < ARRAY_SIZE(markersTX_swr); ++ i, p += 1)
	{
		char buf [10];
		uint_fast16_t xx, yy;

		display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markersTX_swr [i], smeter_params.r2, smeter_params.r2 - 8, smeter, 1, 1);
		polar_to_dek(xb, yb, markersTX_swr [i], smeter_params.r2 - 16, & xx, & yy, 1);
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%u"), p);

		colmain_setcolors(COLORMAIN_YELLOW, COLORMAIN_BLACK);
		colmain_string3_at_xy(bg, SM_BG_W, SM_BG_H, xx - SMALLCHARW3 / 2, yy - SMALLCHARW3 / 2 + 1, buf);
	}

	display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smeter_params.gs, smeter_params.gm, smeter_params.r1, 1, smeter, 1, 1);
	display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smeter_params.gm, smeter_params.ge, smeter_params.r1, 1, smeter, 1, 1);
	display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smeter_params.gs, smeter_params.ge, smeter_params.r2, 1, COLORMAIN_WHITE, 1, 1);

	bg = smeter_bg [SM_STATE_RX];
	colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H - 1, COLORMAIN_BLACK, 1);
#if WITHTOUCHGUI
//	colmain_rounded_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H - 1, 5, COLORMAIN_WHITE, 0);
#endif

	for (p = 1, i = 0; i < ARRAY_SIZE(markers); ++ i, p += 2)
	{
		char buf [10];
		uint_fast16_t xx, yy;

		display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markers [i], smeter_params.r1, smeter_params.r1 + 8, smeter, 1, 1);
		polar_to_dek(xb, yb, markers [i], smeter_params.r1 + 6, & xx, & yy, 1);
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%u"), p);

		colmain_setcolors(COLORMAIN_YELLOW, COLORMAIN_BLACK);
		colmain_string3_at_xy(bg, SM_BG_W, SM_BG_H, xx - SMALLCHARW3 / 2, yy - pad2w3 + 1, buf);
	}
	for (i = 0; i < ARRAY_SIZE(markers2); ++ i)
	{
		display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markers2 [i], smeter_params.r1, smeter_params.r1 + 4, smeter, 1, 1);
	}

	for (p = 20, i = 0; i < ARRAY_SIZE(markersR); ++ i, p += 20)
	{
		char buf [10];
		uint_fast16_t xx, yy;

		display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markersR [i], smeter_params.r1, smeter_params.r1 + 8, smeterplus, 1, 1);
		polar_to_dek(xb, yb, markersR [i], smeter_params.r1 + 6, & xx, & yy, 1);
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("+%u"), p);

		colmain_setcolors(COLORMAIN_RED, COLORMAIN_BLACK);
		colmain_string3_at_xy(bg, SM_BG_W, SM_BG_H, xx - strwidth3(buf) / 2, yy - pad2w3 + 1, buf);
	}
	for (i = 0; i < ARRAY_SIZE(markers2R); ++ i)
	{
		display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markers2R [i], smeter_params.r1, smeter_params.r1 + 4, smeterplus, 1, 1);
	}
	display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smeter_params.gs, smeter_params.gm, smeter_params.r1, 1, smeter, 1, 1);
	display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smeter_params.gm, smeter_params.ge, smeter_params.r1, 1, smeterplus, 1, 1);
	display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smeter_params.gs, smeter_params.ge, smeter_params.r2, 1, COLORMAIN_WHITE, 1, 1);
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

		uint_fast16_t power;

		power = board_getadc_filtered_truevalue(PWRI);
		gp = smeter_params.gs + normalize(power, 0, maxpwrcali << 4, smeter_params.ge - smeter_params.gs);

		gswr = smeter_params.gs + normalize(get_swr(), 0, swr_fullscale, smeter_params.ge - smeter_params.gs);

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
			const COLORMAIN_T color = COLORMAIN_YELLOW;

			display_segm_buf(fr, DIM_X, DIM_Y, xc, yc, smeter_params.gs, gswr, smeter_params.r2 + 2, 1, color, 0, 1);
			display_segm_buf(fr, DIM_X, DIM_Y, xc, yc, smeter_params.gs, gswr, smeter_params.r1 - 2, 1, color, 0, 1);
			display_radius_buf(fr, DIM_X, DIM_Y, xc, yc, smeter_params.gs, smeter_params.r1 - 2, smeter_params.r2 + 2, color, 0, 1);
			display_radius_buf(fr, DIM_X, DIM_Y, xc, yc, gswr, smeter_params.r1 - 2, smeter_params.r2 + 2, color, 0, 1);
			polar_to_dek(xc, yc, gswr - 1, smeter_params.r1 - 4, & xx, & yy, 1);
			display_floodfill(fr, DIM_X, DIM_Y, xx, yy, color, COLORMAIN_BLACK, 1);
		}

		const COLORMAIN_T color = COLORMAIN_GREEN;
		display_radius_buf(fr, DIM_X, DIM_Y, xc - 1, yc, gp, smeter_params.rv1, smeter_params.rv2, color, 0, 1);
		display_radius_buf(fr, DIM_X, DIM_Y, xc, yc, gp, smeter_params.rv1, smeter_params.rv2, color, 0, 1);
		display_radius_buf(fr, DIM_X, DIM_Y, xc + 1, yc, gp, smeter_params.rv1, smeter_params.rv2, color, 0, 1);
	}
	else
	{
		// RX state
		colpip_plot(fr, DIM_X, DIM_Y, x0, y0, smeter_bg [SM_STATE_RX], SM_BG_W, SM_BG_H);

		{
			// Рисование peak value (риска)
			const COLORMAIN_T color = COLORMAIN_YELLOW;
			display_radius_buf(fr, DIM_X, DIM_Y, xc - 1, yc, gv_trace, smeter_params.r1 - 2, smeter_params.r2 + 2, color, 0, 1);
			display_radius_buf(fr, DIM_X, DIM_Y, xc, yc, gv_trace, smeter_params.r1 - 2, smeter_params.r2 + 2, color, 0, 1);
			display_radius_buf(fr, DIM_X, DIM_Y, xc + 1, yc, gv_trace, smeter_params.r1 - 2, smeter_params.r2 + 2, color, 0, 1);
		}

		{
			// Рисование стрелки
			const COLORMAIN_T color = COLORMAIN_GREEN;
			display_radius_buf(fr, DIM_X, DIM_Y, xc - 1, yc, gv, smeter_params.rv1, smeter_params.rv2, color, 0, 1);
			display_radius_buf(fr, DIM_X, DIM_Y, xc, yc, gv, smeter_params.rv1, smeter_params.rv2, color, 0, 1);
			display_radius_buf(fr, DIM_X, DIM_Y, xc + 1, yc, gv, smeter_params.rv1, smeter_params.rv2, color, 0, 1);
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

#include "gui_structs.h"
#include "list.h"
#include "codecs/nau8822.h"

static void btn_main_handler(void);
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
static void window_audiosettings_process(void);
static void buttons_audioparams_process(void);
static void window_ap_mic_eq_process(void);
static void gui_main_process(void);
static void update_touch_list(void);
static void window_ap_reverb_process(void);
static void window_ap_mic_process(void);
static void buttons_ap_mic_process(void);
static void window_tx_process(void);
static void window_tx_vox_process(void);
static void buttons_tx_sett_process(void);
static void window_swrscan_process(void);
static void buttons_swrscan_process(void);
static void window_tx_power_process(void);
static void window_ap_mic_prof_process(void);
static void buttons_ap_mic_prof_process(void);

static btn_bg_t btn_bg [] = {
	{ 100, 44, },
	{ 86, 44, },
	{ 50, 50, },
	{ 40, 40, },
};
enum { BG_COUNT = ARRAY_SIZE(btn_bg) };

static button_t buttons [] = {
//   x1, y1, w, h,  onClickHandler,        			state,   	is_locked, is_trackable, parent,   			 visible,      payload,	          name, 					text
	{ },
//		{ 0, 0, 86, 44, button1_handler, 	  			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN,  			NON_VISIBLE, UINTPTR_MAX,		"btnMode", 					"Mode", },
//		{ 0, 0, 86, 44, button2_handler, 	  			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN,  			NON_VISIBLE, UINTPTR_MAX,		"btnAF", 					"AF|filter", },
//		{ 0, 0, 86, 44, button3_handler, 	  			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN,  			NON_VISIBLE, UINTPTR_MAX,		"btnAGC", 					"AGC", },
//		{ 0, 0, 86, 44, button4_handler, 	  			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN,  			NON_VISIBLE, UINTPTR_MAX,		"btn4", 					"Freq|enter", },
//		{ 0, 0, 86, 44, button5_handler, 	  			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, 			NON_VISIBLE, UINTPTR_MAX,		"btn5", 					"", },
//		{ 0, 0, 86, 44, button6_handler, 	  			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN,  			NON_VISIBLE, UINTPTR_MAX,		"btnSWRscan",				"SWR|scanner", },
//		{ 0, 0, 86, 44, button7_handler, 	  			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN,  			NON_VISIBLE, UINTPTR_MAX,  		"btn7", 					"Transmit|settings", },
//		{ 0, 0, 86, 44, button8_handler, 	  			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN,  			NON_VISIBLE, UINTPTR_MAX,		"btn8", 					"Audio|settings", },
//		{ 0, 0, 86, 44, button9_handler, 	  			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN,  			NON_VISIBLE, UINTPTR_MAX,		"btnSysMenu", 				"System|settings", },
//		{ 0, 0, 86, 44, buttons_mode_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, 			NON_VISIBLE, SUBMODE_LSB, 		"btnModeLSB", 				"LSB", },
//		{ 0, 0, 86, 44, buttons_mode_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, 			NON_VISIBLE, SUBMODE_CW,  		"btnModeCW", 				"CW", },
//		{ 0, 0, 86, 44, buttons_mode_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES,			NON_VISIBLE, SUBMODE_AM,  		"btnModeAM", 				"AM", },
//		{ 0, 0, 86, 44, buttons_mode_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, 			NON_VISIBLE, SUBMODE_DGL, 		"btnModeDGL", 				"DGL", },
//		{ 0, 0, 86, 44, buttons_mode_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, 			NON_VISIBLE, SUBMODE_USB, 		"btnModeUSB", 				"USB", },
//		{ 0, 0, 86, 44, buttons_mode_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, 			NON_VISIBLE, SUBMODE_CWR, 		"btnModeCWR", 				"CWR", },
//		{ 0, 0, 86, 44, buttons_mode_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, 			NON_VISIBLE, SUBMODE_NFM, 		"btnModeNFM", 				"NFM", },
//		{ 0, 0, 86, 44, buttons_mode_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, 			NON_VISIBLE, SUBMODE_DGU, 		"btnModeDGU", 				"DGU", },
//		{ 0, 0, 86, 44, buttons_bp_handler,   			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP,   			NON_VISIBLE, UINTPTR_MAX, 		"btnAF_1", 					"", },
//		{ 0, 0, 86, 44, buttons_bp_handler,   			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP,    			NON_VISIBLE, UINTPTR_MAX, 		"btnAF_OK", 				"OK", },
//		{ 0, 0, 86, 44, buttons_bp_handler,   			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP,    			NON_VISIBLE, UINTPTR_MAX, 		"btnAF_2", 					"", },
//		{ 0, 0, 86, 44, hamradio_set_agc_off, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   			NON_VISIBLE, UINTPTR_MAX, 		"btnAGCoff", 				"AGC|off", },
//		{ 0, 0, 86, 44, hamradio_set_agc_slow,			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   			NON_VISIBLE, UINTPTR_MAX, 		"btnAGCslow", 				"AGC|slow", },
//		{ 0, 0, 86, 44, hamradio_set_agc_fast,			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   			NON_VISIBLE, UINTPTR_MAX, 		"btnAGCfast", 				"AGC|fast", },
//		{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 1, 		 		"btnFreq1", 				"1", },
//		{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 2, 		 		"btnFreq2", 				"2", },
//		{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 3, 		 		"btnFreq3", 				"3", },
//		{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, BUTTON_CODE_BK, 	"btnFreqBK", 				"<-", },
//		{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 4, 	 			"btnFreq4", 				"4", },
//		{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ, 			NON_VISIBLE, 5, 				"btnFreq5", 				"5", },
//		{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 6, 				"btnFreq6", 				"6", },
//		{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, BUTTON_CODE_OK, 	"btnFreqOK", 				"OK", },
//		{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 7, 				"btnFreq7", 				"7", },
//		{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 8,  				"btnFreq8", 				"8", },
//		{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 9, 		 		"btnFreq9", 				"9", },
//		{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 0, 	 			"btnFreq0", 				"0", },
		{ 0, 0, 40, 40, buttons_menu_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU,  			NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu-",				"-", },
		{ 0, 0, 40, 40, buttons_menu_handler,			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU,  			NON_VISIBLE, UINTPTR_MAX, 		"btnSysMenu+",				"+", },
		{ 0, 0, 40, 40, buttons_uif_handler,  			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_UIF,   			NON_VISIBLE, UINTPTR_MAX, 		"btnUIF-", 					"-", },
		{ 0, 0, 40, 40, buttons_uif_handler,  			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_UIF,   			NON_VISIBLE, UINTPTR_MAX, 		"btnUIF+", 					"+", },
//		{ 0, 0, 86, 44, buttons_swrscan_process,		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_SWR_SCANNER, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_swr_start", 	 		"Start", },
//		{ 0, 0, 86, 44, buttons_swrscan_process, 	  	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_SWR_SCANNER, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_swr_OK", 	 	 		"OK", },
		{ 0, 0, 100, 44, buttons_audioparams_process, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_reverb", 				"Reverb|OFF", },
		{ 0, 0, 100, 44, buttons_audioparams_process, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_reverb_settings", 		"Reverb|settings", },
		{ 0, 0, 100, 44, buttons_audioparams_process, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_monitor", 				"Monitor|disabled", },
		{ 0, 0, 100, 44, buttons_audioparams_process, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_mic_eq", 				"MIC EQ|OFF", },
		{ 0, 0, 100, 44, buttons_audioparams_process,	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_mic_eq_settings", 		"MIC EQ|settings", },
		{ 0, 0, 100, 44, buttons_audioparams_process, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_mic_settings", 		"MIC|settings", },
		{ 0, 0, 100, 44, buttons_audioparams_process, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AUDIOSETTINGS, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_mic_profiles", 		"MIC|profiles", },
		{ 0, 0,  40, 40, buttons_audioparams_process, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AP_MIC_EQ, 	 	NON_VISIBLE, UINTPTR_MAX, 		"btn_EQ_ok", 				"OK", },
		{ 0, 0,  40, 40, buttons_audioparams_process, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AP_REVERB_SETT,	NON_VISIBLE, UINTPTR_MAX, 		"btn_REVs_ok", 				"OK", },
		{ 0, 0,  86, 44, buttons_ap_mic_process, 		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AP_MIC_SETT,  	NON_VISIBLE, UINTPTR_MAX, 		"btn_mic_agc", 				"AGC|OFF", },
		{ 0, 0,  86, 44, buttons_ap_mic_process, 		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AP_MIC_SETT,  	NON_VISIBLE, UINTPTR_MAX, 		"btn_mic_boost", 			"Boost|OFF", },
		{ 0, 0,  86, 44, buttons_ap_mic_process, 		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AP_MIC_SETT,  	NON_VISIBLE, UINTPTR_MAX,		"btn_mic_OK", 	 			"OK", },
//		{ 0, 0, 100, 44, buttons_tx_sett_process, 		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_SETTINGS, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_tx_vox", 	 			"VOX|OFF", },
//		{ 0, 0, 100, 44, buttons_tx_sett_process, 		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_SETTINGS, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_tx_vox_settings", 		"VOX|settings", },
//		{ 0, 0, 100, 44, buttons_tx_sett_process, 		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_SETTINGS, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_tx_power", 	 		"TX power", },
//		{ 0, 0,  44, 44, buttons_tx_sett_process, 		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_VOX_SETT, 	NON_VISIBLE, UINTPTR_MAX,		"btn_tx_vox_OK", 	 		"OK", },
//		{ 0, 0,  44, 44, buttons_tx_sett_process, 		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_POWER,    	NON_VISIBLE, UINTPTR_MAX, 		"btn_tx_pwr_OK", 	 		"OK", },
		{ 0, 0, 100, 44, buttons_ap_mic_prof_process, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AP_MIC_PROF, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_mic_profile_1_load", 	"Profile 1|load", },
		{ 0, 0, 100, 44, buttons_ap_mic_prof_process, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AP_MIC_PROF, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_mic_profile_2_load", 	"Profile 2|load", },
		{ 0, 0, 100, 44, buttons_ap_mic_prof_process, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AP_MIC_PROF, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_mic_profile_3_load", 	"Profile 3|load", },
		{ 0, 0, 100, 44, buttons_ap_mic_prof_process, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AP_MIC_PROF, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_mic_profile_1_save", 	"Profile 1|save", },
		{ 0, 0, 100, 44, buttons_ap_mic_prof_process, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AP_MIC_PROF, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_mic_profile_2_save", 	"Profile 2|save", },
		{ 0, 0, 100, 44, buttons_ap_mic_prof_process, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AP_MIC_PROF, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_mic_profile_3_save", 	"Profile 3|save", },
//		{ 375, 120, buttons_uif_handler, 	NULL, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_UIF,   NON_VISIBLE, UINTPTR_MAX, 			"btnUIF_OK", "OK", },
};
enum { BUTTONS_COUNT = ARRAY_SIZE(buttons) };

static label_t labels[] = {
//    x, y,  parent,     		state, is_trackable, visible,   name,       		Text, font_size, 	color, 			onClickHandler
	{ },
//		{ 0, 0, WINDOW_BP,   			DISABLED,  0, NON_VISIBLE, "lbl_low",  	  			"", FONT_LARGE, COLORMAIN_YELLOW, },
//		{ 0, 0, WINDOW_BP,   			DISABLED,  0, NON_VISIBLE, "lbl_high", 	  			"", FONT_LARGE, COLORMAIN_YELLOW, },
		{ 0, 0, WINDOW_MENU, 			CANCELLED, 1, NON_VISIBLE, "lbl_group", 	  		"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, 			CANCELLED, 1, NON_VISIBLE, "lbl_group", 	  		"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, 			CANCELLED, 1, NON_VISIBLE, "lbl_group", 	  		"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, 			CANCELLED, 1, NON_VISIBLE, "lbl_group", 	  		"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU,			CANCELLED, 1, NON_VISIBLE, "lbl_group", 	  		"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, 			CANCELLED, 1, NON_VISIBLE, "lbl_group", 	  		"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, 			CANCELLED, 1, NON_VISIBLE, "lbl_params", 	  		"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, 			CANCELLED, 1, NON_VISIBLE, "lbl_params", 	  		"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU,			CANCELLED, 1, NON_VISIBLE, "lbl_params", 	  		"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, 			CANCELLED, 1, NON_VISIBLE, "lbl_params", 	  		"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, 			CANCELLED, 1, NON_VISIBLE, "lbl_params", 	  		"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, 			CANCELLED, 1, NON_VISIBLE, "lbl_params", 	  		"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, 			CANCELLED, 0, NON_VISIBLE, "lbl_vals", 	  			"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, 			CANCELLED, 0, NON_VISIBLE, "lbl_vals", 	  			"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, 			CANCELLED, 0, NON_VISIBLE, "lbl_vals", 	  			"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, 			CANCELLED, 0, NON_VISIBLE, "lbl_vals", 	  			"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, 			CANCELLED, 0, NON_VISIBLE, "lbl_vals", 	  			"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, 			CANCELLED, 0, NON_VISIBLE, "lbl_vals", 	  			"", FONT_LARGE, COLORMAIN_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_ENC2, 			DISABLED,  0, NON_VISIBLE, "lbl_enc2_param", 		"", FONT_LARGE, COLORMAIN_WHITE, },
		{ 0, 0, WINDOW_ENC2, 			DISABLED,  0, NON_VISIBLE, "lbl_enc2_val",	  		"", FONT_LARGE, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_UIF,  			DISABLED,  0, NON_VISIBLE, "lbl_uif_param",  		"", FONT_LARGE, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_UIF,  			DISABLED,  0, NON_VISIBLE, "lbl_uif_val", 	  		"", FONT_LARGE, COLORMAIN_WHITE, },
//		{ 0, 0,	WINDOW_FREQ, 			DISABLED,  0, NON_VISIBLE, "lbl_freq_val",  		"", FONT_LARGE, COLORMAIN_YELLOW, },
		{ 0, 0,	WINDOW_AP_MIC_EQ, 		DISABLED,  0, NON_VISIBLE, "lbl_eq0.08_val",  		"", FONT_LARGE, COLORMAIN_YELLOW, },
		{ 0, 0,	WINDOW_AP_MIC_EQ, 		DISABLED,  0, NON_VISIBLE, "lbl_eq0.23_val",  		"", FONT_LARGE, COLORMAIN_YELLOW, },
		{ 0, 0,	WINDOW_AP_MIC_EQ, 		DISABLED,  0, NON_VISIBLE, "lbl_eq0.65_val",  		"", FONT_LARGE, COLORMAIN_YELLOW, },
		{ 0, 0,	WINDOW_AP_MIC_EQ, 		DISABLED,  0, NON_VISIBLE, "lbl_eq1.8_val",   		"", FONT_LARGE, COLORMAIN_YELLOW, },
		{ 0, 0,	WINDOW_AP_MIC_EQ, 		DISABLED,  0, NON_VISIBLE, "lbl_eq5.3_val",   		"", FONT_LARGE, COLORMAIN_YELLOW, },
		{ 0, 0,	WINDOW_AP_MIC_EQ, 		DISABLED,  0, NON_VISIBLE, "lbl_eq0.08_name", 		"", FONT_MEDIUM, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_MIC_EQ, 		DISABLED,  0, NON_VISIBLE, "lbl_eq0.23_name", 		"", FONT_MEDIUM, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_MIC_EQ, 		DISABLED,  0, NON_VISIBLE, "lbl_eq0.65_name", 		"", FONT_MEDIUM, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_MIC_EQ, 		DISABLED,  0, NON_VISIBLE, "lbl_eq1.8_name",  		"", FONT_MEDIUM, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_MIC_EQ, 		DISABLED,  0, NON_VISIBLE, "lbl_eq5.3_name",  		"", FONT_MEDIUM, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbDelay",		"", FONT_MEDIUM, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbLoss", 		"", FONT_MEDIUM, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbDelay_min", 	"", FONT_SMALL, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbDelay_max", 	"", FONT_SMALL, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbLoss_min", 	"", FONT_SMALL, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_REVERB_SETT,  DISABLED,  0, NON_VISIBLE, "lbl_reverbLoss_max", 	"", FONT_SMALL, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micLevel", 			"", FONT_MEDIUM, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micClip",  			"", FONT_MEDIUM, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micAGC",   			"", FONT_MEDIUM, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micLevel_min", 		"", FONT_SMALL, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micLevel_max", 		"", FONT_SMALL, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micClip_min",  		"", FONT_SMALL, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micClip_max",  		"", FONT_SMALL, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micAGC_min",   		"", FONT_SMALL, COLORMAIN_WHITE, },
		{ 0, 0,	WINDOW_AP_MIC_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_micAGC_max",   		"", FONT_SMALL, COLORMAIN_WHITE, },
//		{ 0, 0,	WINDOW_TX_VOX_SETT, 	DISABLED,  0, NON_VISIBLE, "lbl_vox_delay",    		"", FONT_MEDIUM, COLORMAIN_WHITE, },
//		{ 0, 0,	WINDOW_TX_VOX_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_vox_level",    		"", FONT_MEDIUM, COLORMAIN_WHITE, },
//		{ 0, 0,	WINDOW_TX_VOX_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_avox_level",   		"", FONT_MEDIUM, COLORMAIN_WHITE, },
//		{ 0, 0,	WINDOW_TX_VOX_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_vox_delay_min", 	"", FONT_SMALL, COLORMAIN_WHITE, },
//		{ 0, 0,	WINDOW_TX_VOX_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_vox_delay_max", 	"", FONT_SMALL, COLORMAIN_WHITE, },
//		{ 0, 0,	WINDOW_TX_VOX_SETT, 	DISABLED,  0, NON_VISIBLE, "lbl_vox_level_min",  	"", FONT_SMALL, COLORMAIN_WHITE, },
//		{ 0, 0,	WINDOW_TX_VOX_SETT, 	DISABLED,  0, NON_VISIBLE, "lbl_vox_level_max",  	"", FONT_SMALL, COLORMAIN_WHITE, },
//		{ 0, 0,	WINDOW_TX_VOX_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_avox_level_min", 	"", FONT_SMALL, COLORMAIN_WHITE, },
//		{ 0, 0,	WINDOW_TX_VOX_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_avox_level_max", 	"", FONT_SMALL, COLORMAIN_WHITE, },
//		{ 0, 0,	WINDOW_SWR_SCANNER,  	DISABLED,  0, NON_VISIBLE, "lbl_swr_bottom", 		"", FONT_SMALL, COLORMAIN_WHITE, },
//		{ 0, 0,	WINDOW_SWR_SCANNER,  	DISABLED,  0, NON_VISIBLE, "lbl_swr_top", 			"", FONT_SMALL, COLORMAIN_WHITE, },
//		{ 0, 0,	WINDOW_SWR_SCANNER,  	DISABLED,  0, NON_VISIBLE, "lbl_swr_error", 		"", FONT_MEDIUM, COLORMAIN_WHITE, },
//		{ 0, 0,	WINDOW_TX_POWER,  		DISABLED,  0, NON_VISIBLE, "lbl_tx_power",    		"", FONT_MEDIUM, COLORMAIN_WHITE, },
//		{ 0, 0,	WINDOW_TX_POWER,  		DISABLED,  0, NON_VISIBLE, "lbl_tune_power",  		"", FONT_MEDIUM, COLORMAIN_WHITE, },
		};
enum { LABELS_COUNT = ARRAY_SIZE(labels) };

static slider_t sliders[] = {
		{ },
		{ 0, 0, 0, 0, 0, 0, ORIENTATION_VERTICAL, 	WINDOW_AP_MIC_EQ, 		"eq0.08", 				CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		{ 0, 0, 0, 0, 0, 0, ORIENTATION_VERTICAL, 	WINDOW_AP_MIC_EQ, 		"eq0.23", 				CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		{ 0, 0, 0, 0, 0, 0, ORIENTATION_VERTICAL, 	WINDOW_AP_MIC_EQ, 		"eq0.65", 				CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		{ 0, 0, 0, 0, 0, 0, ORIENTATION_VERTICAL, 	WINDOW_AP_MIC_EQ, 		"eq1.8",  				CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		{ 0, 0, 0, 0, 0, 0, ORIENTATION_VERTICAL, 	WINDOW_AP_MIC_EQ, 		"eq5.3",  				CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_AP_REVERB_SETT, 	"reverbDelay", 			CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_AP_REVERB_SETT, 	"reverbLoss",  			CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_AP_MIC_SETT, 	"sl_micLevel", 			CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_AP_MIC_SETT, 	"sl_micClip",  			CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_AP_MIC_SETT, 	"sl_micAGC",   			CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
//			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_VOX_SETT, 	"sl_vox_delay",   		CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
//			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_VOX_SETT, 	"sl_vox_level",  		CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
//			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_VOX_SETT, 	"sl_avox_level",  		CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
//			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_POWER, 		"sl_pwr_level",   		CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
//			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_POWER, 		"sl_pwr_tuner_level",  	CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
};
enum { SLIDERS_COUNT = ARRAY_SIZE(sliders) };

static window_t windows[] = {
//     window_id,   		 parent_id, 			align_mode,     x1, y1, w, h,   title,     			 is_show, first_call, onVisibleProcess
	{ WINDOW_MAIN, 			 UINT8_MAX, 			ALIGN_LEFT_X,	0, 0, 0, 0, "",  	   	   			 NON_VISIBLE, 0, gui_main_process, },
	{ WINDOW_MODES, 		 UINT8_MAX, 			ALIGN_CENTER_X, 0, 0, 0, 0, "Select mode", 			 NON_VISIBLE, 0, window_mode_process, },
	{ WINDOW_BP,    		 UINT8_MAX, 			ALIGN_CENTER_X, 0, 0, 0, 0, "Bandpass",    			 NON_VISIBLE, 0, window_bp_process, },
	{ WINDOW_AGC,   		 UINT8_MAX, 			ALIGN_CENTER_X, 0, 0, 0, 0, "AGC control", 			 NON_VISIBLE, 0, window_agc_process, },
	{ WINDOW_FREQ,  		 UINT8_MAX, 			ALIGN_CENTER_X, 0, 0, 0, 0, "Freq:", 	   			 NON_VISIBLE, 0, window_freq_process, },
	{ WINDOW_MENU,  		 UINT8_MAX, 			ALIGN_CENTER_X, 0, 0, 0, 0, "Settings",	   		 	 NON_VISIBLE, 0, window_menu_process, },
	{ WINDOW_ENC2, 			 UINT8_MAX, 			ALIGN_RIGHT_X, 	0, 0, 0, 0, "",  			 		 NON_VISIBLE, 0, window_enc2_process, },
	{ WINDOW_UIF, 			 UINT8_MAX, 			ALIGN_LEFT_X, 	0, 0, 0, 0, "",   		   	 		 NON_VISIBLE, 0, window_uif_process, },
	{ WINDOW_SWR_SCANNER,	 UINT8_MAX, 			ALIGN_CENTER_X, 0, 0, 0, 0, "SWR band scanner",		 NON_VISIBLE, 0, window_swrscan_process, },
	{ WINDOW_AUDIOSETTINGS,  UINT8_MAX, 			ALIGN_CENTER_X, 0, 0, 0, 0, "Audio settings", 		 NON_VISIBLE, 0, window_audiosettings_process, },
	{ WINDOW_AP_MIC_EQ, 	 WINDOW_AUDIOSETTINGS, 	ALIGN_CENTER_X, 0, 0, 0, 0, "MIC TX equalizer",		 NON_VISIBLE, 0, window_ap_mic_eq_process, },
	{ WINDOW_AP_REVERB_SETT, WINDOW_AUDIOSETTINGS, 	ALIGN_CENTER_X, 0, 0, 0, 0, "Reverberator settings", NON_VISIBLE, 0, window_ap_reverb_process, },
	{ WINDOW_AP_MIC_SETT, 	 WINDOW_AUDIOSETTINGS, 	ALIGN_CENTER_X, 0, 0, 0, 0, "Microphone settings", 	 NON_VISIBLE, 0, window_ap_mic_process, },
	{ WINDOW_AP_MIC_PROF, 	 WINDOW_AUDIOSETTINGS, 	ALIGN_CENTER_X, 0, 0, 0, 0, "Microphone profiles", 	 NON_VISIBLE, 0, window_ap_mic_prof_process, },
	{ WINDOW_TX_SETTINGS, 	 UINT8_MAX, 			ALIGN_CENTER_X, 0, 0, 0, 0, "Transmit settings", 	 NON_VISIBLE, 0, window_tx_process, },
	{ WINDOW_TX_VOX_SETT, 	 WINDOW_TX_SETTINGS, 	ALIGN_CENTER_X, 0, 0, 0, 0, "VOX settings", 	 	 NON_VISIBLE, 0, window_tx_vox_process, },
	{ WINDOW_TX_POWER, 		 WINDOW_TX_SETTINGS, 	ALIGN_CENTER_X, 0, 0, 0, 0, "TX power", 	 	 	 NON_VISIBLE, 0, window_tx_power_process, },
};
enum { windows_count = ARRAY_SIZE(windows) };

static enc2_t encoder2 = { 0, 0, 0, 0, 1, 1, };
static editfreq_t editfreq;
static menu_t menu[MENU_COUNT];
static menu_by_name_t menu_uif;
static gui_t gui = { 0, 0, KBD_CODE_MAX, TYPE_DUMMY, NULL, CANCELLED, 0, 0, 0, 0, 0, 1, };
static LIST_ENTRY windows_list;
static touch_t touch_elements[TOUCH_ARRAY_SIZE];
static uint_fast8_t touch_count = 0;
static uint_fast8_t menu_label_touched = 0;
static uint_fast8_t menu_level;
static uint_fast8_t swr_scan_enable = 0;		// флаг разрешения сканирования КСВ
static uint_fast8_t swr_scan_stop = 0;
static uint_fast8_t * y_vals;					// массив КСВ
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
		for (uint_fast8_t i = 1; i < win->bh_count; i++)
		{
			button_t * bh = & win->bh_ptr[i];
			if (!strcmp(bh->name, name))
				return (button_t *) bh;
		}
		PRINTF("find_gui_element_ref: button '%s' not found\n", name);
		ASSERT(0);
		return NULL;
		break;

	case TYPE_LABEL:
		for (uint_fast8_t i = 1; i < win->lh_count; i++)
		{
			label_t * lh = & win->lh_ptr[i];
			if (!strcmp(lh->name, name))
				return (label_t *) lh;
		}
		PRINTF("find_gui_element_ref: label '%s' not found\n", name);
		ASSERT(0);
		return NULL;
		break;

	case TYPE_SLIDER:
		for (uint_fast8_t i = 1; i < win->sh_count; i++)
		{
			slider_t * sh = & win->sh_ptr[i];
			if (!strcmp(sh->name, name))
				return (slider_t *) sh;
		}
		PRINTF("find_gui_element_ref: slider '%s' not found\n", name);
		ASSERT(0);
		return NULL;
		break;

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
		if (touch_elements[i].type != type || touch_elements[i].win != win)
			break;
	}
	* id_end = i - 1;
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
	else
	{ 	// Очистка оконного стека
		PLIST_ENTRY savedFlink;
		for (PLIST_ENTRY t = windows_list.Flink; t != & windows_list; t = savedFlink)
		{
			const window_t * const w = CONTAINING_RECORD(t, window_t, item);
			savedFlink = t->Flink;
			if (w != win)
				RemoveEntryList(t);
		}
		touch_count = id_end + 1;
	}

	for (uint_fast8_t i = id_start; i <= id_end; i++)
	{
		button_t * bh = (button_t *) touch_elements[i].link;
		if (state == DISABLED)
		{
			bh->state = strcmp(bh->name, name) ? DISABLED : CANCELLED;
			bh->is_locked = bh->state == CANCELLED ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
		}
		else if (state == CANCELLED)
		{
			bh->state = CANCELLED;
			bh->is_locked = BUTTON_NON_LOCKED;
		}
	}
}

/* Установка признака видимости окна */
static void set_window(window_t * win, uint_fast8_t value)
{
	PLIST_ENTRY p;
	uint_fast8_t j = 0;
	win->state = value;
	if (value)
	{
		win->first_call = 1;

		if (win->parent_id != UINT8_MAX)	// Есть есть parent window, закрыть его и оставить child window
		{
			PLIST_ENTRY savedFlink;
			for (PLIST_ENTRY t = windows_list.Flink; t != & windows_list; t = savedFlink)
			{
				const window_t * const w = CONTAINING_RECORD(t, window_t, item);
				savedFlink = t->Flink;
				if (w->window_id == win->parent_id)
				{
					RemoveEntryList(t);
					break;
				}
			}
		}
		InsertHeadList(& windows_list, & win->item);
	}
	else
	{
		if(win->bh_count)
		{
			free(win->bh_ptr);
			win->bh_count = 0;
		}
		if(win->lh_count)
		{
			free(win->lh_ptr);
			win->lh_count = 0;
		}
		if(win->sh_count)
		{
			free(win->sh_ptr);
			win->sh_count = 0;
		}
		PLIST_ENTRY savedFlink;
		for (PLIST_ENTRY t = windows_list.Flink; t != & windows_list; t = savedFlink)
		{
			const window_t * const w = CONTAINING_RECORD(t, window_t, item);
			savedFlink = t->Flink;
			if (w == win)
			{
				RemoveEntryList(t);
				break;
			}
		}
		if (win->parent_id != UINT8_MAX)	// При закрытии child window открыть parent window, если есть
		{
			window_t * r = & windows[win->parent_id];
			InsertHeadList(& windows_list, & r->item);
		}
	}
(void) p;
}

/* Расчет экранных координат окна */
static void calculate_window_position(window_t * win, uint16_t xmax, uint16_t ymax)
{
	uint_fast8_t edge_step = 20;
	win->w = xmax > (strlen(win->name) * SMALLCHARW) ? (xmax + edge_step) : (strlen(win->name) * SMALLCHARW + edge_step * 2);
	win->h = ymax + edge_step;

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

static void elements_state (window_t * win)
{
	uint_fast8_t j = 0;
	button_t * b = win->bh_ptr;
	if (b != NULL)
	{
		j = 0;
		for (uint_fast8_t i = 1; i < win->bh_count; i++)
		{
			button_t * bh = & b[i];
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

	label_t * l = win->lh_ptr;
	if(l != NULL)
	{
		j = 0;
		for (uint_fast8_t i = 1; i < win->lh_count; i++)
		{
			label_t * lh = & l[i];
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

	slider_t * s = win->sh_ptr;
	if(s != NULL)
	{
		j = 0;
		for (uint_fast8_t i = 1; i < win->sh_count; i++)
		{
			slider_t * sh = & s[i];
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

// FIXME: Доделать автовыравнивание
static void window_bp_process(void)
{
	static uint_fast8_t val_high, val_low, val_c, val_w;
	static uint_fast16_t x_h, x_l, x_c;
	window_t * win = & windows[WINDOW_BP];
	uint_fast16_t x_size = 290, x_0 = 50, y_0 = 90;
	static label_t * lbl_low, * lbl_high;
	static button_t * button_high, * button_low;

	if (win->first_call)
	{
		uint_fast16_t id = 0, x, y, xmax = 0, ymax = 0;
		uint_fast8_t interval = 20, col1_int = 35, row1_int = window_title_height + 20;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0, 86, 44, buttons_bp_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP, NON_VISIBLE, UINTPTR_MAX, "btnAF_1", "", },
			{ 0, 0, 86, 44, buttons_bp_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP, NON_VISIBLE, UINTPTR_MAX, "btnAF_OK", "OK", },
			{ 0, 0, 86, 44, buttons_bp_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP, NON_VISIBLE, UINTPTR_MAX, "btnAF_2", "", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		label_t labels[] = {
		//    x, y,  parent,     		state, is_trackable, visible,   name,       		Text, font_size, 	color, 			onClickHandler
			{ },
			{ 0, 0, WINDOW_BP, DISABLED,  0, NON_VISIBLE, "lbl_low", "", FONT_LARGE, COLORMAIN_YELLOW, },
			{ 0, 0, WINDOW_BP, DISABLED,  0, NON_VISIBLE, "lbl_high", "", FONT_LARGE, COLORMAIN_YELLOW, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		memcpy(win->lh_ptr, labels, labels_size);

		button_high = find_gui_element_ref(TYPE_BUTTON, win, "btnAF_2");
		button_low = find_gui_element_ref(TYPE_BUTTON, win, "btnAF_1");

		lbl_low = find_gui_element_ref(TYPE_LABEL, win, "lbl_low");
		lbl_high = find_gui_element_ref(TYPE_LABEL, win, "lbl_high");

		lbl_low->y = y_0 + get_label_height(lbl_low);
		lbl_high->y = lbl_low->y;

		lbl_low->visible = VISIBLE;
		lbl_high->visible = VISIBLE;

		x = col1_int;
		y = lbl_high->y + get_label_height(lbl_high) * 2;
		for (uint_fast8_t id = 1; id < win->bh_count; id ++)
		{
			button_t * bh = & win->bh_ptr[id];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;
			x = x + interval + bh->w;
		}

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

		xmax = button_high->x1 + button_high->w;
		ymax = button_high->y1 + button_high->h;
		calculate_window_position(win, xmax, ymax);
		elements_state(win);
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
	colmain_line(fr, DIM_X, DIM_Y, win->x1 + x_0 - 10, win->y1 + y_0, win->x1 + x_0 + x_size, win->y1 + y_0, COLORMAIN_WHITE, 0);
	colmain_line(fr, DIM_X, DIM_Y, win->x1 + x_0, win->y1 + y_0 - 45, win->x1 + x_0, win->y1 + y_0 + 5, COLORMAIN_WHITE, 0);
	colmain_line(fr, DIM_X, DIM_Y, win->x1 + x_l, win->y1 + y_0 - 40, win->x1 + x_l - 4, win->y1 + y_0 - 3, COLORMAIN_YELLOW, 1);
	colmain_line(fr, DIM_X, DIM_Y, win->x1 + x_h, win->y1 + y_0 - 40, win->x1 + x_h + 4, win->y1 + y_0 - 3, COLORMAIN_YELLOW, 1);
	colmain_line(fr, DIM_X, DIM_Y, win->x1 + x_l, win->y1 + y_0 - 40, win->x1 + x_h, win->y1 + y_0 - 40, COLORMAIN_YELLOW, 0);
	colmain_line(fr, DIM_X, DIM_Y, win->x1 + x_c, win->y1 + y_0 - 45, win->x1 + x_c, win->y1 + y_0 + 5, COLORMAIN_RED, 0);
}

static void buttons_freq_handler (void)
{
	button_t * bh =  gui.selected_link->link;
	if (bh->parent == WINDOW_FREQ && editfreq.key == BUTTON_CODE_DONE)
		editfreq.key = bh->payload;
}

static void window_freq_process (void)
{
	static label_t * lbl_freq;
	window_t * win = & windows[WINDOW_FREQ];

	if (win->first_call)
	{
		uint_fast16_t x, y, xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 4;
		win->first_call = 0;
		button_t * bh = NULL;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 1, 		 		"btnFreq1", 				"1", },
			{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 2, 		 		"btnFreq2", 				"2", },
			{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 3, 		 		"btnFreq3", 				"3", },
			{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, BUTTON_CODE_BK, 	"btnFreqBK", 				"<-", },
			{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 4, 	 			"btnFreq4", 				"4", },
			{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ, 			NON_VISIBLE, 5, 				"btnFreq5", 				"5", },
			{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 6, 				"btnFreq6", 				"6", },
			{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, BUTTON_CODE_OK, 	"btnFreqOK", 				"OK", },
			{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 7, 				"btnFreq7", 				"7", },
			{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 8,  				"btnFreq8", 				"8", },
			{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 9, 		 		"btnFreq9", 				"9", },
			{ 0, 0, 50, 50, buttons_freq_handler, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  			NON_VISIBLE, 0, 	 			"btnFreq0", 				"0", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);

		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		label_t labels[] = {
		//    x, y,  parent,     		state, is_trackable, visible,   name,       		Text, font_size, 	color, 			onClickHandler
			{ },
			{ 0, 0,	WINDOW_FREQ, 			DISABLED,  0, NON_VISIBLE, "lbl_freq_val",  		"", FONT_LARGE, COLORMAIN_YELLOW, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);

		win->lh_ptr = malloc(labels_size);
		memcpy(win->lh_ptr, labels, labels_size);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 1, r = 1; i < win->bh_count; i ++, r ++)
		{
			bh = & win->bh_ptr[i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = col1_int;
				y = y + bh->h + interval;
			}
			xmax = (xmax > bh->x1 + bh->w) ? xmax : (bh->x1 + bh->w);
			ymax = (ymax > bh->y1 + bh->h) ? ymax : (bh->y1 + bh->h);
		}

		bh = find_gui_element_ref(TYPE_BUTTON, win, "btnFreqOK");
		bh->is_locked = BUTTON_LOCKED;

		lbl_freq = find_gui_element_ref(TYPE_LABEL, win, "lbl_freq_val");
		lbl_freq->x = strwidth(win->name) + strwidth(" ") + 20;
		lbl_freq->y = 5;
		strcpy(lbl_freq->text, "     0 k");
		lbl_freq->color = COLORMAIN_YELLOW;
		lbl_freq->visible = VISIBLE;

		editfreq.val = 0;
		editfreq.num = 0;
		editfreq.key = BUTTON_CODE_DONE;

		calculate_window_position(win, xmax, ymax);
		elements_state(win);
		return;
	}

	if (editfreq.key != BUTTON_CODE_DONE)
	{
		lbl_freq->color = COLORMAIN_YELLOW;
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
				lbl_freq->color = COLORMAIN_RED;
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
	if (gui.selected_type == TYPE_BUTTON)
	{
		button_t * bh = gui.selected_link->link;
		if (bh == find_gui_element_ref(TYPE_BUTTON, win, "btnUIF+"))
			encoder2.rotate = 1;
		else if (bh == find_gui_element_ref(TYPE_BUTTON, win, "btnUIF-"))
			encoder2.rotate = -1;
		else if (bh == find_gui_element_ref(TYPE_BUTTON, win, "btnUIF_OK"))
		{
			hamradio_disable_keyboard_redirect();
			set_window(win, NON_VISIBLE);
			footer_buttons_state(CANCELLED);
		}
	}
}

static void window_uif_process(void)
{
	static label_t * lbl_uif_val;
	static button_t * button_up, * button_down;
	static uint_fast16_t window_center_x;
	static uint_fast8_t reinit = 0;
	window_t * win = & windows[WINDOW_UIF];

	if (win->first_call || reinit)
	{
		win->first_call = 0;
		reinit = 0;
		uint_fast16_t xmax = 0, ymax = 0;
		uint_fast8_t col1_int = 20, row1_int = window_title_height + 20;

		strcpy(win->name, menu_uif.name);

		button_down = find_gui_element_ref(TYPE_BUTTON, win, "btnUIF-");
		button_up = find_gui_element_ref(TYPE_BUTTON, win, "btnUIF+");
		lbl_uif_val = find_gui_element_ref(TYPE_LABEL, win, "lbl_uif_val");

		const char * v = hamradio_gui_edit_menu_item(menu_uif.menupos, 0);
		strcpy(lbl_uif_val->text, v);

		button_down->x1 = col1_int;
		button_down->y1 = row1_int;
		button_down->visible = VISIBLE;

		button_up->x1 = button_down->x1 + button_down->w + 30 + get_label_width(lbl_uif_val);
		button_up->y1 = button_down->y1;
		button_up->visible = VISIBLE;

		window_center_x = (col1_int + button_up->x1 + button_up->w) / 2;

		lbl_uif_val->x = window_center_x - get_label_width(lbl_uif_val) / 2;
		lbl_uif_val->y = row1_int + button_up->w / 2 - get_label_height(lbl_uif_val) / 2;
		lbl_uif_val->visible = VISIBLE;

		xmax = button_up->x1 + button_up->w;
		ymax = button_up->y1 + button_up->h;
		calculate_window_position(win, xmax, ymax);

		hamradio_enable_keyboard_redirect();
		return;
	}

	if (encoder2.rotate != 0)
	{
		hamradio_gui_edit_menu_item(menu_uif.menupos, encoder2.rotate);

		reinit = 1;
		encoder2.rotate_done = 1;
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
	static uint_fast8_t menu_is_scrolling = 0;
	uint_fast8_t int_cols = 200, int_rows = 35;
	static button_t * button_up = NULL, * button_down = NULL;
	window_t * win = & windows[WINDOW_MENU];

	if (win->first_call)
	{
		uint_fast16_t xmax = 0, ymax = 0;
		win->first_call = 0;
		win->align_mode = ALIGN_CENTER_X;						// выравнивание окна системных настроек всегда по центру

		hamradio_set_menu_cond(VISIBLE);

		uint_fast8_t col1_int = 20, row1_int = window_title_height + 20, id_start, id_end, i = 0;
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
		xn = col1_int;
		yn = row1_int;
		for(i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
		{
			lh = touch_elements[menu[MENU_GROUPS].first_id + i].link;
			strcpy(lh->text, menu[MENU_GROUPS].menu_block[i + menu[MENU_GROUPS].add_id].name);
			lh->visible = VISIBLE;
			lh->color = i == menu[MENU_GROUPS].selected_label ? COLORMAIN_BLACK : COLORMAIN_WHITE;
			lh->x = xn;
			lh->y = yn;
			yn += int_rows;
		}

		menu[MENU_PARAMS].count = hamradio_get_multilinemenu_block_params(menu[MENU_PARAMS].menu_block, menu[MENU_GROUPS].menu_block[menu[MENU_GROUPS].selected_str].index) - 1;
		xn += int_cols;
		yn = row1_int;
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
		yn = row1_int;
		for(lh = NULL, i = 0; i <= menu[MENU_VALS].num_rows; i ++)
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

		ASSERT(lh != NULL);
		xmax = lh->x + 100;
		ymax = lh->y + get_label_height(lh);
		calculate_window_position(win, xmax, ymax);
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

		// при переходе между уровнями пункты меню выделяется цветом
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
			for (uint8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
			{
				lh = touch_elements[menu[MENU_GROUPS].first_id + i].link;
				lh->color = i == menu[MENU_GROUPS].selected_label ? COLORMAIN_BLACK : COLORMAIN_GRAY;

				lh = touch_elements[menu[MENU_PARAMS].first_id + i].link;
				lh->color = i == menu[MENU_PARAMS].selected_label ? COLORMAIN_BLACK : COLORMAIN_GRAY;

				lh = touch_elements[menu[MENU_VALS].first_id + i].link;
				lh->color = i == menu[MENU_PARAMS].selected_label ? COLORMAIN_YELLOW : COLORMAIN_GRAY;
			}
			menu_label_touched = 0;
		}
		else if (menu_level == MENU_PARAMS)
		{
			button_down->visible = NON_VISIBLE;
			button_up->visible = NON_VISIBLE;
			for (uint8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
			{
				lh = touch_elements[menu[MENU_GROUPS].first_id + i].link;
				lh->color = i == menu[MENU_GROUPS].selected_label ? COLORMAIN_BLACK : COLORMAIN_GRAY;

				lh = touch_elements[menu[MENU_PARAMS].first_id + i].link;
				lh->color = i == menu[MENU_PARAMS].selected_label ? COLORMAIN_BLACK : COLORMAIN_WHITE;

				lh = touch_elements[menu[MENU_VALS].first_id + i].link;
				lh->color = COLORMAIN_WHITE;
			}
		}
		else if (menu_level == MENU_GROUPS)
		{
			button_down->visible = NON_VISIBLE;
			button_up->visible = NON_VISIBLE;
			for (uint8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
			{
				lh = touch_elements[menu[MENU_GROUPS].first_id + i].link;
				lh->color = i == menu[MENU_GROUPS].selected_label ? COLORMAIN_BLACK : COLORMAIN_WHITE;

				lh = touch_elements[menu[MENU_PARAMS].first_id + i].link;
				lh->color = COLORMAIN_WHITE;

				lh = touch_elements[menu[MENU_VALS].first_id + i].link;
				lh->color = COLORMAIN_WHITE;
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
				l->color = i == menu[MENU_GROUPS].selected_label ? COLORMAIN_BLACK : COLORMAIN_WHITE;
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
			strcpy(lv->text, menu[MENU_VALS].menu_block[i].name);
			lp->color = i == menu[MENU_PARAMS].selected_label && menu_level > MENU_GROUPS ? COLORMAIN_BLACK : COLORMAIN_WHITE;
			lp->visible = VISIBLE;
			lp->state = CANCELLED;
			lv->visible = VISIBLE;
			lv->state = CANCELLED;
		}
		menu_label_touched = 0;
	}

	label_t * lh;
	switch (menu_level)
	{
	case MENU_PARAMS:
	case MENU_VALS:
		lh = touch_elements[menu[MENU_PARAMS].first_id + menu[MENU_PARAMS].selected_label].link;
		colpip_rect(colmain_fb_draw(), DIM_X, DIM_Y, win->x1 + lh->x - 5, win->y1 + lh->y - 5, win->x1 + lh->x + int_cols - 20,
				win->y1 + lh->y + get_label_height(lh) + 5, GUI_MENUSELECTCOLOR, 1);

	case MENU_GROUPS:
		lh = touch_elements[menu[MENU_GROUPS].first_id + menu[MENU_GROUPS].selected_label].link;
		colpip_rect(colmain_fb_draw(), DIM_X, DIM_Y, win->x1 + lh->x - 5, win->y1 + lh->y - 5, win->x1 + lh->x + int_cols - 20,
				win->y1 + lh->y + get_label_height(lh) + 5, GUI_MENUSELECTCOLOR, 1);
	}

}

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
	window_t * win = & windows[WINDOW_ENC2];
	uint_fast8_t col1_int = 20, row1_int = 20;

	if (win->first_call == 1 || gui_enc2_menu->updated)
	{
		uint_fast16_t xmax = 0, ymax = 0;

		win->first_call = 0;
		lbl_param = find_gui_element_ref(TYPE_LABEL, win, "lbl_enc2_param");
		lbl_param->x = col1_int;
		lbl_param->y = row1_int;
		strcpy(lbl_param->text, gui_enc2_menu->param);
		remove_end_line_spaces(lbl_param->text);
		lbl_param->visible = VISIBLE;

		lbl_val = find_gui_element_ref(TYPE_LABEL, win, "lbl_enc2_val");
		lbl_val->y = lbl_param->y + get_label_height(lbl_val) * 2;
		lbl_val->visible = VISIBLE;
		strcpy(lbl_val->text, gui_enc2_menu->val);
		lbl_val->color = gui_enc2_menu->state == 2 ? COLORMAIN_YELLOW : COLORMAIN_WHITE;
		lbl_val->x = lbl_param->x + get_label_width(lbl_param) / 2 - get_label_width(lbl_val) / 2;

		xmax = lbl_param->x + get_label_width(lbl_param);
		ymax = lbl_val->y + get_label_height(lbl_val);
		calculate_window_position(win, xmax, ymax);
		gui_enc2_menu->updated = 0;
		gui.timer_1sec_updated = 1;
	}
}

static void buttons_mode_handler(void)
{
	window_t * win = & windows[WINDOW_MODES];
	if(gui.selected_type == TYPE_BUTTON)
	{
		button_t * bh = (button_t *)gui.selected_link->link;
		if (win->state && bh->parent == win->window_id)
		{
			if (bh->payload != UINTPTR_MAX)
				hamradio_change_submode(bh->payload);

			set_window(win, NON_VISIBLE);
			footer_buttons_state(CANCELLED);
			gui.timer_1sec_updated = 1;
		}
	}
}

static void window_mode_process(void)
{
	window_t * win = & windows[WINDOW_MODES];
	if (win->first_call)
	{
		uint_fast16_t x, y;
		uint_fast16_t xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 4;
		uint_fast8_t id_start, id_end;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_LSB, "btnModeLSB", "LSB", },
			{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_CW, "btnModeCW", "CW", },
			{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_AM, "btnModeAM", "AM", },
			{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_DGL, "btnModeDGL", "DGL", },
			{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_USB, "btnModeUSB", "USB", },
			{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_CWR, "btnModeCWR", "CWR", },
			{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_NFM, "btnModeNFM", "NFM", },
			{ 0, 0, 86, 44, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_DGU,  "btnModeDGU", "DGU", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);

		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);


		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 1, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr[i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;
			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = col1_int;
				y = row1_int + bh->h + interval;
			}
			xmax = (xmax > bh->x1 + bh->w) ? xmax : (bh->x1 + bh->w);
			ymax = (ymax > bh->y1 + bh->h) ? ymax : (bh->y1 + bh->h);
		}
		calculate_window_position(win, xmax, ymax);
		elements_state(win);
		return;
	}
}

static void window_agc_process(void)
{
	window_t * win = & windows[WINDOW_AGC];
	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 4, id_start, id_end;
		win->first_call = 0;
		button_t * bh = NULL;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0, 86, 44, hamradio_set_agc_off, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   			NON_VISIBLE, UINTPTR_MAX, 		"btnAGCoff", 				"AGC|off", },
			{ 0, 0, 86, 44, hamradio_set_agc_slow,			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   			NON_VISIBLE, UINTPTR_MAX, 		"btnAGCslow", 				"AGC|slow", },
			{ 0, 0, 86, 44, hamradio_set_agc_fast,			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   			NON_VISIBLE, UINTPTR_MAX, 		"btnAGCfast", 				"AGC|fast", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 1, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr[i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = col1_int;
				y = y + bh->h + interval;
			}
			xmax = (xmax > bh->x1 + bh->w) ? xmax : (bh->x1 + bh->w);
			ymax = (ymax > bh->y1 + bh->h) ? ymax : (bh->y1 + bh->h);
		}
		calculate_window_position(win, xmax, ymax);
		elements_state(win);
		return;
	}
}

static void buttons_audioparams_process(void)
{
	if(gui.selected_type == TYPE_BUTTON)
	{
		window_t * winAP = & windows[WINDOW_AUDIOSETTINGS];
		window_t * winEQ = & windows[WINDOW_AP_MIC_EQ];
		window_t * winRS = & windows[WINDOW_AP_REVERB_SETT];
		window_t * winMIC = & windows[WINDOW_AP_MIC_SETT];
		window_t * winMICpr = & windows[WINDOW_AP_MIC_PROF];
		button_t * btn_reverb = find_gui_element_ref(TYPE_BUTTON, winAP, "btn_reverb");						// reverb on/off
		button_t * btn_reverb_settings = find_gui_element_ref(TYPE_BUTTON, winAP, "btn_reverb_settings");	// reverb settings
		button_t * btn_monitor = find_gui_element_ref(TYPE_BUTTON, winAP, "btn_monitor");					// monitor on/off
		button_t * btn_mic_eq = find_gui_element_ref(TYPE_BUTTON, winAP, "btn_mic_eq");						// MIC EQ on/off
		button_t * btn_mic_eq_settings = find_gui_element_ref(TYPE_BUTTON, winAP, "btn_mic_eq_settings");	// MIC EQ settingss
		button_t * btn_mic_settings = find_gui_element_ref(TYPE_BUTTON, winAP, "btn_mic_settings");			// mic settings
		button_t * btn_mic_profiles = find_gui_element_ref(TYPE_BUTTON, winAP, "btn_mic_profiles");			// mic profiles
		button_t * btn_EQ_ok = find_gui_element_ref(TYPE_BUTTON, winEQ, "btn_EQ_ok");
		button_t * btn_REVs_ok = find_gui_element_ref(TYPE_BUTTON, winRS, "btn_REVs_ok");


#if WITHREVERB
		if (gui.selected_link->link == btn_reverb)
		{
			btn_reverb->is_locked = hamradio_get_greverb() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			local_snprintf_P(btn_reverb->text, ARRAY_SIZE(btn_reverb->text), PSTR("Reverb|%s"), btn_reverb->is_locked ? "ON" : "OFF");
			hamradio_set_greverb(btn_reverb->is_locked);
			btn_reverb_settings->state = btn_reverb->is_locked ? CANCELLED : DISABLED;

		}
		else if (gui.selected_link->link == btn_reverb_settings)
		{
			set_window(winRS, VISIBLE);
		}
		else if (gui.selected_link->link == btn_REVs_ok)
		{
			set_window(winRS, NON_VISIBLE);
		}
		else
#endif /* WITHREVERB */
		if (gui.selected_link->link == btn_monitor)
		{
			btn_monitor->is_locked = hamradio_get_gmoniflag() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			local_snprintf_P(btn_monitor->text, ARRAY_SIZE(btn_monitor->text), PSTR("Monitor|%s"), btn_monitor->is_locked ? "enabled" : "disabled");
			hamradio_set_gmoniflag(btn_monitor->is_locked);
		}
		else if (gui.selected_link->link == btn_mic_eq)
		{
			btn_mic_eq->is_locked = hamradio_get_gmikeequalizer() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			local_snprintf_P(btn_mic_eq->text, ARRAY_SIZE(btn_mic_eq->text), PSTR("MIC EQ|%s"), btn_mic_eq->is_locked ? "ON" : "OFF");
			hamradio_set_gmikeequalizer(btn_mic_eq->is_locked);
			btn_mic_eq_settings->state = btn_mic_eq->is_locked ? CANCELLED : DISABLED;
		}
		else if (gui.selected_link->link == btn_mic_eq_settings)
		{
			set_window(winEQ, VISIBLE);
		}
		else if (gui.selected_link->link == btn_mic_settings)
		{
			set_window(winMIC, VISIBLE);
		}
		else if (gui.selected_link->link == btn_mic_profiles)
		{
			set_window(winMICpr, VISIBLE);
		}
		else if (gui.selected_link->link == btn_EQ_ok)
		{
			set_window(winEQ, NON_VISIBLE);
		}
	}
}

static void window_audiosettings_process(void)
{
	window_t * win = & windows[WINDOW_AUDIOSETTINGS];

	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 3, id_start, id_end;
		button_t * bh = NULL;
		win->first_call = 0;

		find_entry_area_elements(TYPE_BUTTON, win, & id_start, & id_end);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = id_start, r = 1; i <= id_end; i ++, r ++)
		{
			bh = (button_t *) touch_elements[i].link;
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = col1_int;
				y = y + bh->h + interval;
			}
			xmax = (xmax > bh->x1 + bh->w) ? xmax : (bh->x1 + bh->w);
			ymax = (ymax > bh->y1 + bh->h) ? ymax : (bh->y1 + bh->h);
		}
		calculate_window_position(win, xmax, ymax);

#if WITHREVERB
		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_reverb"); 						// reverb on/off
		bh->is_locked = hamradio_get_greverb() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("Reverb|%s"), hamradio_get_greverb() ? "ON" : "OFF");

		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_reverb_settings");				// reverb settings
		bh->state = hamradio_get_greverb() ? CANCELLED : DISABLED;
#else
		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_reverb");						// reverb on/off disable
		bh->state = DISABLED;

		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_reverb_settings"); 			// reverb settings disable
		bh->state = DISABLED;
#endif /* WITHREVERB */

		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_monitor");						// monitor on/off
		bh->is_locked = hamradio_get_gmoniflag() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("Monitor|%s"), bh->is_locked ? "enabled" : "disabled");

#if WITHAFCODEC1HAVEPROC
		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_mic_eq");						// MIC EQ on/off
		bh->is_locked = hamradio_get_gmikeequalizer() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("MIC EQ|%s"), bh->is_locked ? "ON" : "OFF");

		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_mic_eq_settings");				// MIC EQ settings
		bh->state = hamradio_get_gmikeequalizer() ? CANCELLED : DISABLED;
#else
		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_mic_eq");						// MIC EQ on/off disable
		bh->state = DISABLED;

		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_mic_eq_settings"); 			// MIC EQ settings disable
		bh->state = DISABLED;
#endif /* WITHAFCODEC1HAVEPROC */
		return;
	}
}

static void window_ap_mic_eq_process(void)
{
	PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
	window_t * win = & windows[WINDOW_AP_MIC_EQ];
	slider_t * sl = NULL;
	label_t * lbl = NULL;
	static uint_fast8_t eq_limit, eq_base = 0;
	char buf[TEXT_ARRAY_SIZE];
	static int_fast16_t mid_y = 0;
	static uint_fast8_t id = 0, id_start, id_end;
	static button_t * btn_EQ_ok;

	if (win->first_call)
	{
		uint_fast16_t x, y, mid_w;
		uint_fast16_t xmax = 0, ymax = 0;
		uint_fast8_t interval = 70, col1_int = 70, row1_int = window_title_height + 20;
		win->first_call = 0;

		find_entry_area_elements(TYPE_SLIDER, win, & id_start, & id_end);

		eq_base = hamradio_getequalizerbase();
		eq_limit = abs(eq_base) * 2;

		x = col1_int;
		y = row1_int;

		for (id = id_start; id <= id_end; id++)
		{
			sl = (slider_t *) touch_elements[id].link;

			sl->x = x;
			sl->size = 200;
			sl->step = 2;
			sl->value = normalize(hamradio_get_gmikeequalizerparams(id - id_start), eq_limit, 0, 100);
			sl->visible = VISIBLE;

			mid_w = sl->x + sliders_width / 2;		// центр шкалы слайдера по x

			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("lbl_%s_name"), sl->name);
			lbl = find_gui_element_ref(TYPE_LABEL, win, buf);
			local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%sk"), strchr(sl->name, 'q') + 1);
			lbl->x = mid_w - get_label_width(lbl) / 2;
			lbl->y = y;
			lbl->visible = VISIBLE;

			y = lbl->y + get_label_height(lbl) * 2;

			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("lbl_%s_val"), sl->name);
			lbl = find_gui_element_ref(TYPE_LABEL, win, buf);
			local_snprintf_P(lbl->text, ARRAY_SIZE(lbl->text), PSTR("%d"), hamradio_get_gmikeequalizerparams(id - id_start) + eq_base);
			lbl->x = mid_w - get_label_width(lbl) / 2;
			lbl->y = y;
			lbl->visible = VISIBLE;

			sl->y = lbl->y + get_label_height(lbl) * 2 + 10;

			x = x + interval;
			y = row1_int;
		}

		btn_EQ_ok = find_gui_element_ref(TYPE_BUTTON, win, "btn_EQ_ok");
		btn_EQ_ok->x1 = sl->x + sliders_width + btn_EQ_ok->w;
		btn_EQ_ok->y1 = sl->y + sl->size - btn_EQ_ok->h;
		btn_EQ_ok->visible = VISIBLE;

		xmax = btn_EQ_ok->x1 + btn_EQ_ok->w;
		ymax = btn_EQ_ok->y1 + btn_EQ_ok->h;
		calculate_window_position(win, xmax, ymax);

		mid_y = win->y1 + sl->y + sl->size / 2;

		return;
	}

	if (gui.selected_type == TYPE_SLIDER && gui.is_tracking)
	{
		/* костыль через костыль */
		sl = gui.selected_link->link;
		uint_fast8_t id = gui.selected_link->pos;

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
		colmain_line(fr, DIM_X, DIM_Y, win->x1 + 50, mid_y + yy, win->x1 + win->w - (btn_EQ_ok->w << 1), mid_y + yy, GUI_SLIDERLAYOUTCOLOR, 0);
		local_snprintf_P(buf, ARRAY_SIZE(buf), i == 0 ? PSTR("%d") : PSTR("-%d"), i);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, win->x1 + 50 - strwidth2(buf) - 5, mid_y + yy - SMALLCHARH2 / 2, buf, COLORMAIN_WHITE);

		if (i == 0)
			continue;
		colmain_line(fr, DIM_X, DIM_Y, win->x1 + 50, mid_y - yy, win->x1 + win->w - (btn_EQ_ok->w << 1), mid_y - yy, GUI_SLIDERLAYOUTCOLOR, 0);
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), i);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, win->x1 + 50 - strwidth2(buf) - 5, mid_y - yy - SMALLCHARH2 / 2, buf, COLORMAIN_WHITE);
	}
}

static void window_ap_reverb_process(void)
{
	window_t * win = & windows[WINDOW_AP_REVERB_SETT];
	static label_t * lbl_reverbDelay = NULL, * lbl_reverbLoss = NULL;
	static slider_t * sl_reverbDelay = NULL, * sl_reverbLoss = NULL;
	static uint_fast16_t delay_min, delay_max, loss_min, loss_max;
	slider_t * sl = NULL;

	if (win->first_call)
	{
		uint_fast8_t interval = 60, col1_int = 20;
		uint_fast16_t xmax = 0, ymax = 0;
		win->first_call = 0;

		label_t * lbl_reverbDelay_min = find_gui_element_ref(TYPE_LABEL, win, "lbl_reverbDelay_min");
		label_t * lbl_reverbDelay_max = find_gui_element_ref(TYPE_LABEL, win, "lbl_reverbDelay_max");
		label_t * lbl_reverbLoss_min = find_gui_element_ref(TYPE_LABEL, win, "lbl_reverbLoss_min");
		label_t * lbl_reverbLoss_max = find_gui_element_ref(TYPE_LABEL, win, "lbl_reverbLoss_max");
		lbl_reverbDelay = find_gui_element_ref(TYPE_LABEL, win, "lbl_reverbDelay");
		lbl_reverbLoss = find_gui_element_ref(TYPE_LABEL, win, "lbl_reverbLoss");
		sl_reverbDelay = find_gui_element_ref(TYPE_SLIDER, win, "reverbDelay");
		sl_reverbLoss = find_gui_element_ref(TYPE_SLIDER, win, "reverbLoss");

		hamradio_get_reverb_delay_limits(& delay_min, & delay_max);
		hamradio_get_reverb_loss_limits(& loss_min, & loss_max);

		lbl_reverbDelay->x = col1_int;
		lbl_reverbDelay->y = interval;
		lbl_reverbDelay->visible = VISIBLE;
		local_snprintf_P(lbl_reverbDelay->text, ARRAY_SIZE(lbl_reverbDelay->text), PSTR("Delay: %3d ms"), hamradio_get_reverb_delay());

		lbl_reverbLoss->x = lbl_reverbDelay->x;
		lbl_reverbLoss->y = lbl_reverbDelay->y + interval;
		lbl_reverbLoss->visible = VISIBLE;
		local_snprintf_P(lbl_reverbLoss->text, ARRAY_SIZE(lbl_reverbLoss->text), PSTR("Loss :  %2d dB"), hamradio_get_reverb_loss());

		sl_reverbDelay->x = lbl_reverbDelay->x + interval * 3;
		sl_reverbDelay->y = lbl_reverbDelay->y;
		sl_reverbDelay->visible = VISIBLE;
		sl_reverbDelay->size = 300;
		sl_reverbDelay->step = 3;
		sl_reverbDelay->value = normalize(hamradio_get_reverb_delay(), delay_min, delay_max, 100);

		local_snprintf_P(lbl_reverbDelay_min->text, ARRAY_SIZE(lbl_reverbDelay_min->text), PSTR("%d ms"), delay_min);
		lbl_reverbDelay_min->x = sl_reverbDelay->x - get_label_width(lbl_reverbDelay_min) / 2;
		lbl_reverbDelay_min->y = sl_reverbDelay->y + get_label_height(lbl_reverbDelay_min) * 3;
		lbl_reverbDelay_min->visible = VISIBLE;

		local_snprintf_P(lbl_reverbDelay_max->text, ARRAY_SIZE(lbl_reverbDelay_max->text), PSTR("%d ms"), delay_max);
		lbl_reverbDelay_max->x = sl_reverbDelay->x + sl_reverbDelay->size - get_label_width(lbl_reverbDelay_max) / 2;
		lbl_reverbDelay_max->y = sl_reverbDelay->y + get_label_height(lbl_reverbDelay_max) * 3;
		lbl_reverbDelay_max->visible = VISIBLE;

		sl_reverbLoss->x = lbl_reverbLoss->x + interval * 3;
		sl_reverbLoss->y = lbl_reverbLoss->y;
		sl_reverbLoss->visible = VISIBLE;
		sl_reverbLoss->size = 300;
		sl_reverbLoss->step = 3;
		sl_reverbLoss->value = normalize(hamradio_get_reverb_loss(), loss_min, loss_max, 100);

		local_snprintf_P(lbl_reverbLoss_min->text, ARRAY_SIZE(lbl_reverbLoss_min->text), PSTR("%d dB"), loss_min);
		lbl_reverbLoss_min->x = sl_reverbLoss->x - get_label_width(lbl_reverbLoss_min) / 2;
		lbl_reverbLoss_min->y = sl_reverbLoss->y + get_label_height(lbl_reverbLoss_min) * 3;
		lbl_reverbLoss_min->visible = VISIBLE;

		local_snprintf_P(lbl_reverbLoss_max->text, ARRAY_SIZE(lbl_reverbLoss_max->text), PSTR("%d dB"), loss_max);
		lbl_reverbLoss_max->x = sl_reverbLoss->x + sl_reverbLoss->size - get_label_width(lbl_reverbLoss_max) / 2;
		lbl_reverbLoss_max->y = sl_reverbLoss->y + get_label_height(lbl_reverbLoss_max) * 3;
		lbl_reverbLoss_max->visible = VISIBLE;

		button_t * bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_REVs_ok");
		bh->x1 = sl_reverbLoss->x + sl_reverbLoss->size + interval / 2;
		bh->y1 = lbl_reverbLoss->y;
		bh->visible = VISIBLE;

		xmax = bh->x1 + bh->w;
		ymax = bh->y1 + bh->h;
		calculate_window_position(win, xmax, ymax);

		return;
	}

	if (gui.selected_type == TYPE_SLIDER && gui.is_tracking)
	{
		char buf[TEXT_ARRAY_SIZE];

		/* костыль через костыль */
		sl = (slider_t *) gui.selected_link->link;

		if(sl == sl_reverbDelay)
		{
			uint_fast16_t delay = delay_min + normalize(sl->value, 0, 100, delay_max - delay_min);
			local_snprintf_P(lbl_reverbDelay->text, ARRAY_SIZE(lbl_reverbDelay->text), PSTR("Delay: %3d ms"), delay);
			hamradio_set_reverb_delay(delay);
		}
		else if (sl == sl_reverbLoss)
		{
			uint_fast16_t loss = loss_min + normalize(sl->value, 0, 100, loss_max - loss_min);
			local_snprintf_P(lbl_reverbLoss->text, ARRAY_SIZE(lbl_reverbLoss->text), PSTR("Loss :  %2d dB"), loss);
			hamradio_set_reverb_loss(loss);
		}
	}
}

static void window_ap_mic_process(void)
{
	window_t * win = & windows[WINDOW_AP_MIC_SETT];
	static slider_t * sl_micLevel = NULL, * sl_micClip = NULL, * sl_micAGC = NULL;
	static label_t * lbl_micLevel = NULL, * lbl_micClip = NULL, * lbl_micAGC = NULL;
	static uint_fast16_t level_min, level_max, clip_min, clip_max, agc_min, agc_max;
	slider_t * sl;

	if (win->first_call)
	{
		uint_fast8_t interval = 50, col1_int = 20;
		uint_fast16_t xmax = 0, ymax = 0;
		win->first_call = 0;

		hamradio_get_mic_level_limits(& level_min, & level_max);
		hamradio_get_mic_clip_limits(& clip_min, & clip_max);
		hamradio_get_mic_agc_limits(& agc_min, & agc_max);

		sl_micLevel = find_gui_element_ref(TYPE_SLIDER, win, "sl_micLevel");
		sl_micClip = find_gui_element_ref(TYPE_SLIDER, win, "sl_micClip");
		sl_micAGC = find_gui_element_ref(TYPE_SLIDER, win, "sl_micAGC");
		lbl_micLevel = find_gui_element_ref(TYPE_LABEL, win, "lbl_micLevel");
		lbl_micClip = find_gui_element_ref(TYPE_LABEL, win, "lbl_micClip");
		lbl_micAGC = find_gui_element_ref(TYPE_LABEL, win, "lbl_micAGC");

		lbl_micLevel->x = col1_int;
		lbl_micLevel->y = interval;
		lbl_micLevel->visible = VISIBLE;
		local_snprintf_P(lbl_micLevel->text, ARRAY_SIZE(lbl_micLevel->text), PSTR("Level: %3d"), hamradio_get_mik1level());

		lbl_micClip->x = lbl_micLevel->x;
		lbl_micClip->y = lbl_micLevel->y + interval;
		lbl_micClip->visible = VISIBLE;
		local_snprintf_P(lbl_micClip->text, ARRAY_SIZE(lbl_micClip->text), PSTR("Clip : %3d"), hamradio_get_gmikehclip());

		lbl_micAGC->x = lbl_micClip->x;
		lbl_micAGC->y = lbl_micClip->y + interval;
		lbl_micAGC->visible = VISIBLE;
		local_snprintf_P(lbl_micAGC->text, ARRAY_SIZE(lbl_micAGC->text), PSTR("AGC  : %3d"), hamradio_get_gmikeagcgain());

		sl_micLevel->x = lbl_micLevel->x + interval * 2 + interval / 2;
		sl_micLevel->y = lbl_micLevel->y;
		sl_micLevel->visible = VISIBLE;
		sl_micLevel->size = 300;
		sl_micLevel->step = 3;
		sl_micLevel->value = normalize(hamradio_get_mik1level(), level_min, level_max, 100);

		sl_micClip->x = sl_micLevel->x;
		sl_micClip->y = lbl_micClip->y;
		sl_micClip->visible = VISIBLE;
		sl_micClip->size = 300;
		sl_micClip->step = 3;
		sl_micClip->value = normalize(hamradio_get_gmikehclip(), clip_min, clip_max, 100);

		sl_micAGC->x = sl_micLevel->x;
		sl_micAGC->y = lbl_micAGC->y;
		sl_micAGC->visible = VISIBLE;
		sl_micAGC->size = 300;
		sl_micAGC->step = 3;
		sl_micAGC->value = normalize(hamradio_get_gmikeagcgain(), clip_min, clip_max, 100);

		button_t * bh2 = find_gui_element_ref(TYPE_BUTTON, win, "btn_mic_boost");
		bh2->x1 = (sl_micLevel->x + sl_micLevel->size + col1_int * 2) / 2 - (bh2->w / 2);
		bh2->y1 = lbl_micAGC->y + interval;
		bh2->is_locked = hamradio_get_gmikebust20db() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh2->text, ARRAY_SIZE(bh2->text), PSTR("Boost|%s"), bh2->is_locked ? "ON" : "OFF");
		bh2->visible = VISIBLE;

		button_t * bh1 = find_gui_element_ref(TYPE_BUTTON, win, "btn_mic_agc");
		bh1->x1 = bh2->x1 - bh1->w - interval;
		bh1->y1 = bh2->y1;
		bh1->is_locked = hamradio_get_gmikeagc() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh1->text, ARRAY_SIZE(bh1->text), PSTR("AGC|%s"), bh1->is_locked ? "ON" : "OFF");
		bh1->visible = VISIBLE;

		bh1 = find_gui_element_ref(TYPE_BUTTON, win, "btn_mic_OK");
		bh1->x1 = bh2->x1 + bh2->w + interval;
		bh1->y1 = bh2->y1;
		bh1->visible = VISIBLE;

		label_t * lbl_micLevel_min = find_gui_element_ref(TYPE_LABEL, win, "lbl_micLevel_min");
		local_snprintf_P(lbl_micLevel_min->text, ARRAY_SIZE(lbl_micLevel_min->text), PSTR("%d"), level_min);
		lbl_micLevel_min->x = sl_micLevel->x - get_label_width(lbl_micLevel_min) / 2;
		lbl_micLevel_min->y = sl_micLevel->y + get_label_height(lbl_micLevel_min) * 3;
		lbl_micLevel_min->visible = VISIBLE;

		label_t * lbl_micLevel_max = find_gui_element_ref(TYPE_LABEL, win, "lbl_micLevel_max");
		local_snprintf_P(lbl_micLevel_max->text, ARRAY_SIZE(lbl_micLevel_max->text), PSTR("%d"), level_max);
		lbl_micLevel_max->x = sl_micLevel->x + sl_micLevel->size - get_label_width(lbl_micLevel_max) / 2;
		lbl_micLevel_max->y = sl_micLevel->y + get_label_height(lbl_micLevel_max) * 3;
		lbl_micLevel_max->visible = VISIBLE;

		label_t * lbl_micClip_min = find_gui_element_ref(TYPE_LABEL, win, "lbl_micClip_min");
		local_snprintf_P(lbl_micClip_min->text, ARRAY_SIZE(lbl_micClip_min->text), PSTR("%d"), clip_min);
		lbl_micClip_min->x = sl_micClip->x - get_label_width(lbl_micClip_min) / 2;
		lbl_micClip_min->y = sl_micClip->y + get_label_height(lbl_micClip_min) * 3;
		lbl_micClip_min->visible = VISIBLE;

		label_t * lbl_micClip_max = find_gui_element_ref(TYPE_LABEL, win, "lbl_micClip_max");
		local_snprintf_P(lbl_micClip_max->text, ARRAY_SIZE(lbl_micClip_max->text), PSTR("%d"), clip_max);
		lbl_micClip_max->x = sl_micClip->x + sl_micClip->size - get_label_width(lbl_micClip_max) / 2;
		lbl_micClip_max->y = sl_micClip->y + get_label_height(lbl_micClip_max) * 3;
		lbl_micClip_max->visible = VISIBLE;

		label_t * lbl_micAGC_min = find_gui_element_ref(TYPE_LABEL, win, "lbl_micAGC_min");
		local_snprintf_P(lbl_micAGC_min->text, ARRAY_SIZE(lbl_micAGC_min->text), PSTR("%d"), agc_min);
		lbl_micAGC_min->x = sl_micAGC->x - get_label_width(lbl_micAGC_min) / 2;
		lbl_micAGC_min->y = sl_micAGC->y + get_label_height(lbl_micAGC_min) * 3;
		lbl_micAGC_min->visible = VISIBLE;

		label_t * lbl_micAGC_max = find_gui_element_ref(TYPE_LABEL, win, "lbl_micAGC_max");
		local_snprintf_P(lbl_micAGC_max->text, ARRAY_SIZE(lbl_micAGC_max->text), PSTR("%d"), agc_max);
		lbl_micAGC_max->x = sl_micAGC->x + sl_micClip->size - get_label_width(lbl_micAGC_max) / 2;
		lbl_micAGC_max->y = sl_micAGC->y + get_label_height(lbl_micAGC_max) * 3;
		lbl_micAGC_max->visible = VISIBLE;

		xmax = sl_micAGC->x + sl_micAGC->size + col1_int;
		ymax = bh1->y1 + bh1->h;
		calculate_window_position(win, xmax, ymax);
		return;
	}

	if (gui.selected_type == TYPE_SLIDER && gui.is_tracking)
	{
		char buf[TEXT_ARRAY_SIZE];

		/* костыль через костыль */
		sl = (slider_t *) gui.selected_link->link;

		if(sl == sl_micLevel)
		{
			uint_fast16_t level = level_min + normalize(sl->value, 0, 100, level_max - level_min);
			local_snprintf_P(lbl_micLevel->text, ARRAY_SIZE(lbl_micLevel->text), PSTR("Level: %3d"), level);
			hamradio_set_mik1level(level);
		}
		else if (sl == sl_micClip)
		{
			uint_fast16_t clip = clip_min + normalize(sl->value, 0, 100, clip_max - clip_min);
			local_snprintf_P(lbl_micClip->text, ARRAY_SIZE(lbl_micClip->text), PSTR("Clip : %3d"), clip);
			hamradio_set_gmikehclip(clip);
		}
		else if (sl == sl_micAGC)
		{
			uint_fast16_t agc = agc_min + normalize(sl->value, 0, 100, agc_max - agc_min);
			local_snprintf_P(lbl_micAGC->text, ARRAY_SIZE(lbl_micAGC->text), PSTR("AGC  : %3d"), agc);
			hamradio_set_gmikeagc(agc);
		}
	}
}

static void buttons_ap_mic_process(void)
{
	if(gui.selected_type == TYPE_BUTTON)
	{
		window_t * win = & windows[WINDOW_AP_MIC_SETT];
		button_t * btn_mic_boost = find_gui_element_ref(TYPE_BUTTON, win, "btn_mic_boost");
		button_t * btn_mic_agc = find_gui_element_ref(TYPE_BUTTON, win, "btn_mic_agc");
		button_t * btn_mic_OK = find_gui_element_ref(TYPE_BUTTON, win, "btn_mic_OK");

		if (gui.selected_link->link == btn_mic_boost)
		{
			btn_mic_boost->is_locked = hamradio_get_gmikebust20db() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			local_snprintf_P(btn_mic_boost->text, ARRAY_SIZE(btn_mic_boost->text), PSTR("Boost|%s"), btn_mic_boost->is_locked ? "ON" : "OFF");
			hamradio_set_gmikebust20db(btn_mic_boost->is_locked);
		}
		else if (gui.selected_link->link == btn_mic_agc)
		{
			btn_mic_agc->is_locked = hamradio_get_gmikeagc() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			local_snprintf_P(btn_mic_agc->text, ARRAY_SIZE(btn_mic_agc->text), PSTR("AGC|%s"), btn_mic_agc->is_locked ? "ON" : "OFF");
			hamradio_set_gmikeagc(btn_mic_agc->is_locked);
		}
		else if (gui.selected_link->link == btn_mic_OK)
		{
			set_window(win, NON_VISIBLE);
		}
	}
}

static void window_tx_process(void)
{
	window_t * win = & windows[WINDOW_TX_SETTINGS];

	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 3, id_start, id_end;
		button_t * bh = NULL;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0, 100, 44, buttons_tx_sett_process, 		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_SETTINGS, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_tx_vox", 	 			"VOX|OFF", },
			{ 0, 0, 100, 44, buttons_tx_sett_process, 		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_SETTINGS, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_tx_vox_settings", 		"VOX|settings", },
			{ 0, 0, 100, 44, buttons_tx_sett_process, 		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_SETTINGS, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_tx_power", 	 		"TX power", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = 1, r = 1; i < win->bh_count; i ++, r ++)
		{
			button_t * bh = & win->bh_ptr[i];
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = col1_int;
				y = y + bh->h + interval;
			}
			xmax = (xmax > bh->x1 + bh->w) ? xmax : (bh->x1 + bh->w);
			ymax = (ymax > bh->y1 + bh->h) ? ymax : (bh->y1 + bh->h);
		}
		calculate_window_position(win, xmax, ymax);

#if WITHVOX
		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_tx_vox"); 						// vox on/off
		bh->is_locked = hamradio_get_gvoxenable() ? BUTTON_LOCKED : BUTTON_NON_LOCKED;
		local_snprintf_P(bh->text, ARRAY_SIZE(bh->text), PSTR("VOX|%s"), hamradio_get_gvoxenable() ? "ON" : "OFF");

		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_tx_vox_settings");				// vox settings
		bh->state = hamradio_get_gvoxenable() ? CANCELLED : DISABLED;
#else
		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_tx_vox");						// reverb on/off disable
		bh->state = DISABLED;

		bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_tx_vox_settings"); 			// reverb settings disable
		bh->state = DISABLED;
#endif /* WITHVOX */
		elements_state(win);
		return;
	}

}

static void buttons_tx_vox_process(void)
{
	if(gui.selected_type == TYPE_BUTTON)
	{
		window_t * win = & windows[WINDOW_TX_VOX_SETT];
		button_t * btn_tx_vox_OK = find_gui_element_ref(TYPE_BUTTON, win, "btn_tx_vox_OK");
		if (gui.selected_link->link == btn_tx_vox_OK)
			set_window(win, NON_VISIBLE);
	}
}

static void window_tx_vox_process(void)
{
	window_t * win = & windows[WINDOW_TX_VOX_SETT];
	static slider_t * sl_vox_delay = NULL, * sl_vox_level = NULL, * sl_avox_level = NULL;
	static label_t * lbl_vox_delay = NULL, * lbl_vox_level = NULL, * lbl_avox_level = NULL;
	static uint_fast16_t delay_min, delay_max, level_min, level_max, alevel_min, alevel_max;
	slider_t * sl;

	if (win->first_call)
	{
		uint_fast8_t interval = 50, col1_int = 20;
		uint_fast16_t xmax = 0, ymax = 0;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0,  44, 44, buttons_tx_vox_process, 		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_VOX_SETT, 	NON_VISIBLE, UINTPTR_MAX,		"btn_tx_vox_OK", 	 		"OK", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		slider_t sliders [] = {
			{ },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_VOX_SETT, 	"sl_vox_delay",   		CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_VOX_SETT, 	"sl_vox_level",  		CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_VOX_SETT, 	"sl_avox_level",  		CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		};
		win->sh_count = ARRAY_SIZE(sliders);
		uint_fast16_t sliders_size = sizeof(sliders);
		win->sh_ptr = malloc(sliders_size);
		memcpy(win->sh_ptr, sliders, sliders_size);

		label_t labels[] = {
		//    x, y,  parent,     		state, is_trackable, visible,   name,       		Text, font_size, 	color, 			onClickHandler
			{ },
			{ 0, 0,	WINDOW_TX_VOX_SETT, 	DISABLED,  0, NON_VISIBLE, "lbl_vox_delay",    		"", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_VOX_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_vox_level",    		"", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_VOX_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_avox_level",   		"", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_VOX_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_vox_delay_min", 	"", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_VOX_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_vox_delay_max", 	"", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_VOX_SETT, 	DISABLED,  0, NON_VISIBLE, "lbl_vox_level_min",  	"", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_VOX_SETT, 	DISABLED,  0, NON_VISIBLE, "lbl_vox_level_max",  	"", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_VOX_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_avox_level_min", 	"", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_VOX_SETT,  	DISABLED,  0, NON_VISIBLE, "lbl_avox_level_max", 	"", FONT_SMALL, COLORMAIN_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		memcpy(win->lh_ptr, labels, labels_size);

		hamradio_get_vox_delay_limits(& delay_min, & delay_max);
		hamradio_get_vox_level_limits(& level_min, & level_max);
		hamradio_get_antivox_delay_limits(& alevel_min, & alevel_max);

		sl_vox_delay = find_gui_element_ref(TYPE_SLIDER, win, "sl_vox_delay");
		sl_vox_level = find_gui_element_ref(TYPE_SLIDER, win, "sl_vox_level");
		sl_avox_level = find_gui_element_ref(TYPE_SLIDER, win, "sl_avox_level");
		lbl_vox_delay = find_gui_element_ref(TYPE_LABEL, win, "lbl_vox_delay");
		lbl_vox_level = find_gui_element_ref(TYPE_LABEL, win, "lbl_vox_level");
		lbl_avox_level = find_gui_element_ref(TYPE_LABEL, win, "lbl_avox_level");

		ldiv_t d = ldiv(hamradio_get_vox_delay(), 100);
		lbl_vox_delay->x = col1_int;
		lbl_vox_delay->y = interval;
		lbl_vox_delay->visible = VISIBLE;
		local_snprintf_P(lbl_vox_delay->text, ARRAY_SIZE(lbl_vox_delay->text), PSTR("Delay: %d.%d"), d.quot, d.rem / 10);

		lbl_vox_level->x = lbl_vox_delay->x;
		lbl_vox_level->y = lbl_vox_delay->y + interval;
		lbl_vox_level->visible = VISIBLE;
		local_snprintf_P(lbl_vox_level->text, ARRAY_SIZE(lbl_vox_level->text), PSTR("Level: %3d"), hamradio_get_vox_level());

		lbl_avox_level->x = lbl_vox_level->x;
		lbl_avox_level->y = lbl_vox_level->y + interval;
		lbl_avox_level->visible = VISIBLE;
		local_snprintf_P(lbl_avox_level->text, ARRAY_SIZE(lbl_avox_level->text), PSTR("AVOX : %3d"), hamradio_get_antivox_level());

		sl_vox_delay->x = lbl_vox_delay->x + interval * 2 + interval / 2;
		sl_vox_delay->y = lbl_vox_delay->y;
		sl_vox_delay->visible = VISIBLE;
		sl_vox_delay->size = 300;
		sl_vox_delay->step = 3;
		sl_vox_delay->value = normalize(hamradio_get_vox_delay(), delay_min, delay_max, 100);

		sl_vox_level->x = sl_vox_delay->x;
		sl_vox_level->y = lbl_vox_level->y;
		sl_vox_level->visible = VISIBLE;
		sl_vox_level->size = 300;
		sl_vox_level->step = 3;
		sl_vox_level->value = normalize(hamradio_get_vox_level(), level_min, level_max, 100);

		sl_avox_level->x = sl_vox_delay->x;
		sl_avox_level->y = lbl_avox_level->y;
		sl_avox_level->visible = VISIBLE;
		sl_avox_level->size = 300;
		sl_avox_level->step = 3;
		sl_avox_level->value = normalize(hamradio_get_antivox_level(), alevel_min, alevel_max, 100);

		button_t * bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_tx_vox_OK");
		bh->x1 = (sl_vox_delay->x + sl_vox_delay->size + col1_int * 2) / 2 - (bh->w / 2);
		bh->y1 = lbl_avox_level->y + interval;
		bh->visible = VISIBLE;

		d = ldiv(delay_min, 100);
		label_t * lbl_vox_delay_min = find_gui_element_ref(TYPE_LABEL, win, "lbl_vox_delay_min");
		local_snprintf_P(lbl_vox_delay_min->text, ARRAY_SIZE(lbl_vox_delay_min->text), PSTR("%d.%d sec"), d.quot, d.rem / 10);
		lbl_vox_delay_min->x = sl_vox_delay->x - get_label_width(lbl_vox_delay_min) / 2;
		lbl_vox_delay_min->y = sl_vox_delay->y + get_label_height(lbl_vox_delay_min) * 3;
		lbl_vox_delay_min->visible = VISIBLE;

		d = ldiv(delay_max, 100);
		label_t * lbl_vox_delay_max = find_gui_element_ref(TYPE_LABEL, win, "lbl_vox_delay_max");
		local_snprintf_P(lbl_vox_delay_max->text, ARRAY_SIZE(lbl_vox_delay_max->text), PSTR("%d.%d sec"), d.quot, d.rem / 10);
		lbl_vox_delay_max->x = sl_vox_delay->x + sl_vox_delay->size - get_label_width(lbl_vox_delay_max) / 2;
		lbl_vox_delay_max->y = sl_vox_delay->y + get_label_height(lbl_vox_delay_max) * 3;
		lbl_vox_delay_max->visible = VISIBLE;

		label_t * lbl_vox_level_min = find_gui_element_ref(TYPE_LABEL, win, "lbl_vox_level_min");
		local_snprintf_P(lbl_vox_level_min->text, ARRAY_SIZE(lbl_vox_level_min->text), PSTR("%d"), level_min);
		lbl_vox_level_min->x = sl_vox_level->x - get_label_width(lbl_vox_level_min) / 2;
		lbl_vox_level_min->y = sl_vox_level->y + get_label_height(lbl_vox_level_min) * 3;
		lbl_vox_level_min->visible = VISIBLE;

		label_t * lbl_vox_level_max = find_gui_element_ref(TYPE_LABEL, win, "lbl_vox_level_max");
		local_snprintf_P(lbl_vox_level_max->text, ARRAY_SIZE(lbl_vox_level_max->text), PSTR("%d"), level_max);
		lbl_vox_level_max->x = sl_vox_level->x + sl_vox_level->size - get_label_width(lbl_vox_level_max) / 2;
		lbl_vox_level_max->y = sl_vox_level->y + get_label_height(lbl_vox_level_max) * 3;
		lbl_vox_level_max->visible = VISIBLE;

		label_t * lbl_avox_level_min = find_gui_element_ref(TYPE_LABEL, win, "lbl_avox_level_min");
		local_snprintf_P(lbl_avox_level_min->text, ARRAY_SIZE(lbl_avox_level_min->text), PSTR("%d"), alevel_min);
		lbl_avox_level_min->x = sl_avox_level->x - get_label_width(lbl_avox_level_min) / 2;
		lbl_avox_level_min->y = sl_avox_level->y + get_label_height(lbl_avox_level_min) * 3;
		lbl_avox_level_min->visible = VISIBLE;

		label_t * lbl_avox_level_max = find_gui_element_ref(TYPE_LABEL, win, "lbl_avox_level_max");
		local_snprintf_P(lbl_avox_level_max->text, ARRAY_SIZE(lbl_avox_level_max->text), PSTR("%d"), alevel_max);
		lbl_avox_level_max->x = sl_avox_level->x + sl_vox_level->size - get_label_width(lbl_avox_level_max) / 2;
		lbl_avox_level_max->y = sl_avox_level->y + get_label_height(lbl_avox_level_max) * 3;
		lbl_avox_level_max->visible = VISIBLE;

		xmax = sl_avox_level->x + sl_avox_level->size + col1_int;
		ymax = bh->y1 + bh->h;
		calculate_window_position(win, xmax, ymax);
		elements_state(win);
		return;
	}

	if (gui.selected_type == TYPE_SLIDER && gui.is_tracking)
	{
		char buf[TEXT_ARRAY_SIZE];

		/* костыль через костыль */
		sl = (slider_t *) gui.selected_link->link;

		if(sl == sl_vox_delay)
		{
			uint_fast16_t delay = delay_min + normalize(sl->value, 0, 100, delay_max - delay_min);
			ldiv_t d = ldiv(delay, 100);
			local_snprintf_P(lbl_vox_delay->text, ARRAY_SIZE(lbl_vox_delay->text), PSTR("Delay: %d.%d"), d.quot, d.rem / 10);
			hamradio_set_vox_delay(delay);
		}
		else if (sl == sl_vox_level)
		{
			uint_fast16_t level = level_min + normalize(sl->value, 0, 100, level_max - level_min);
			local_snprintf_P(lbl_vox_level->text, ARRAY_SIZE(lbl_vox_level->text), PSTR("Level: %3d"), level);
			hamradio_set_vox_level(level);
		}
		else if (sl == sl_avox_level)
		{
			uint_fast16_t alevel = alevel_min + normalize(sl->value, 0, 100, alevel_max - alevel_min);
			local_snprintf_P(lbl_avox_level->text, ARRAY_SIZE(lbl_avox_level->text), PSTR("AVOX : %3d"), alevel);
			hamradio_set_antivox_level(alevel);
		}
	}
}

static void buttons_tx_sett_process(void)
{
	if(gui.selected_type == TYPE_BUTTON)
	{
		window_t * winTX = & windows[WINDOW_TX_SETTINGS];
		window_t * winPower = & windows[WINDOW_TX_POWER];
		window_t * winVOX = & windows[WINDOW_TX_VOX_SETT];
		button_t * btn_tx_vox = find_gui_element_ref(TYPE_BUTTON, winTX, "btn_tx_vox");
		button_t * btn_tx_power = find_gui_element_ref(TYPE_BUTTON, winTX, "btn_tx_power");
		button_t * btn_tx_vox_settings = find_gui_element_ref(TYPE_BUTTON, winTX, "btn_tx_vox_settings");
		if (gui.selected_link->link == btn_tx_vox)
		{
			btn_tx_vox->is_locked = hamradio_get_gvoxenable() ? BUTTON_NON_LOCKED : BUTTON_LOCKED;
			local_snprintf_P(btn_tx_vox->text, ARRAY_SIZE(btn_tx_vox->text), PSTR("VOX|%s"), btn_tx_vox->is_locked ? "ON" : "OFF");
			hamradio_set_gvoxenable(btn_tx_vox->is_locked);
			btn_tx_vox_settings->state = hamradio_get_gvoxenable() ? CANCELLED : DISABLED;
		}
		else if (gui.selected_link->link == btn_tx_vox_settings)
		{
			set_window(winVOX, VISIBLE);
		}
		else if (gui.selected_link->link == btn_tx_power)
		{
			set_window(winPower, VISIBLE);
		}
	}
}

static void window_swrscan_process(void)
{
	PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
	uint_fast16_t gr_w = 500, gr_h = 250;												// размеры области графика
	uint_fast8_t interval = 20, col1_int = 20, row1_int = 30;
	uint_fast16_t x0 = col1_int + interval * 2, y0 = row1_int + gr_h - interval * 2;	// нулевые координаты графика
	uint_fast16_t x1 = x0 + gr_w - interval * 4, y1 = gr_h - y0 + interval * 2;			// размеры осей графика
	static uint_fast16_t mid_w = 0, freq_step = 0;
	static uint_fast16_t i, current_freq_x;
	static uint_fast32_t lim_bottom, lim_top, swr_freq, backup_freq;
	static label_t * lbl_swr_error;
	static button_t * btn_swr_start, * btnSWRscan, * btn_swr_OK;
	static uint_fast8_t backup_power;
	static uint_fast8_t swr_scan_done = 0, is_swr_scanning = 0;
	window_t * win = & windows[WINDOW_SWR_SCANNER];
	uint_fast8_t averageFactor = 3;

	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		win->first_call = 0;
		button_t * bh = NULL;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0, 86, 44, buttons_swrscan_process,		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_SWR_SCANNER, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_swr_start", 	 		"Start", },
			{ 0, 0, 86, 44, buttons_swrscan_process, 	  	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_SWR_SCANNER, 	NON_VISIBLE, UINTPTR_MAX, 		"btn_swr_OK", 	 	 		"OK", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		label_t labels[] = {
		//    x, y,  parent,     		state, is_trackable, visible,   name,       		Text, font_size, 	color, 			onClickHandler
			{ },
			{ 0, 0,	WINDOW_SWR_SCANNER,  	DISABLED,  0, NON_VISIBLE, "lbl_swr_bottom", 		"", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_SWR_SCANNER,  	DISABLED,  0, NON_VISIBLE, "lbl_swr_top", 			"", FONT_SMALL, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_SWR_SCANNER,  	DISABLED,  0, NON_VISIBLE, "lbl_swr_error", 		"", FONT_MEDIUM, COLORMAIN_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		memcpy(win->lh_ptr, labels, labels_size);

		mid_w = col1_int + gr_w / 2;
		btn_swr_start = find_gui_element_ref(TYPE_BUTTON, win, "btn_swr_start");
		btn_swr_start->x1 = mid_w - btn_swr_start->w - interval;
		btn_swr_start->y1 = row1_int + gr_h + col1_int;
		strcpy(btn_swr_start->text, "Start");
		btn_swr_start->visible = VISIBLE;

		btn_swr_OK = find_gui_element_ref(TYPE_BUTTON, win, "btn_swr_OK");
		btn_swr_OK->x1 = mid_w + interval;
		btn_swr_OK->y1 = btn_swr_start->y1;
		btn_swr_OK->visible = VISIBLE;

		lbl_swr_error = find_gui_element_ref(TYPE_LABEL, win, "lbl_swr_error");
		btnSWRscan = find_gui_element_ref(TYPE_BUTTON, & windows[WINDOW_MAIN], "btn_SWRscan");

		backup_freq = hamradio_get_freq_rx();
		if(hamradio_verify_freq_bands(backup_freq, & lim_bottom, & lim_top))
		{
			label_t * lbl_swr_bottom = find_gui_element_ref(TYPE_LABEL, win, "lbl_swr_bottom");
			local_snprintf_P(lbl_swr_bottom->text, ARRAY_SIZE(lbl_swr_bottom->text), PSTR("%dk"), lim_bottom / 1000);
			lbl_swr_bottom->x = x0 - get_label_width(lbl_swr_bottom) / 2;
			lbl_swr_bottom->y = y0 + get_label_height(lbl_swr_bottom) * 2;
			lbl_swr_bottom->visible = VISIBLE;

			label_t * lbl_swr_top = find_gui_element_ref(TYPE_LABEL, win, "lbl_swr_top");
			local_snprintf_P(lbl_swr_top->text, ARRAY_SIZE(lbl_swr_top->text), PSTR("%dk"), lim_top / 1000);
			lbl_swr_top->x = x1 - get_label_width(lbl_swr_bottom) / 2;
			lbl_swr_top->y = lbl_swr_bottom->y;
			lbl_swr_top->visible = VISIBLE;

			btn_swr_start->state = CANCELLED;
			swr_freq = lim_bottom;
			freq_step = (lim_top - lim_bottom) / (x1 - x0);
			current_freq_x = normalize(backup_freq / 1000, lim_bottom / 1000, lim_top / 1000, x1 - x0);
//				backup_power = hamradio_get_tx_power();
		}
		else
		{	// если текущая частота не входит ни в один из диапазонов, вывод сообщения об ошибке
			local_snprintf_P(lbl_swr_error->text, ARRAY_SIZE(lbl_swr_error->text), PSTR("%dk not into HAM bands"), backup_freq / 1000);
			lbl_swr_error->x = mid_w - get_label_width(lbl_swr_error) / 2;
			lbl_swr_error->y = (row1_int + gr_h) / 2;
			lbl_swr_error->visible = VISIBLE;
			btn_swr_start->state = DISABLED;
		}

		xmax = col1_int + gr_w;
		ymax = btn_swr_OK->y1 + btn_swr_OK->h;
		calculate_window_position(win, xmax, ymax);
		elements_state(win);

		i = 0;
		y_vals = calloc(x1 - x0, sizeof(uint_fast8_t));
		swr_scan_done = 0;
		is_swr_scanning = 0;
		swr_scan_stop = 0;
		return;
	}

	if (swr_scan_enable)						// нажата кнопка Start
	{
		swr_scan_enable = 0;
		strcpy(btn_swr_start->text, "Stop");
		btnSWRscan->state = DISABLED;
		btn_swr_OK->state = DISABLED;
//		hamradio_set_tx_power(50);
		hamradio_set_tune(1);
		is_swr_scanning = 1;
		i = 0;
		swr_freq = lim_bottom;
		memset(y_vals, 0, x1 - x0);
	}

	if (is_swr_scanning)						// сканирование
	{
		hamradio_set_freq(swr_freq);
		swr_freq += freq_step;
		if (swr_freq >= lim_top || swr_scan_stop)
		{										// нажата кнопка Stop или сканируемая частота выше границы диапазона
			swr_scan_done = 1;
			is_swr_scanning = 0;
			swr_scan_stop = 0;
			strcpy(btn_swr_start->text, "Start");
			btnSWRscan->state = CANCELLED;
			btn_swr_OK->state = CANCELLED;
			hamradio_set_tune(0);
			hamradio_set_freq(backup_freq);
//			hamradio_set_tx_power(backup_power);
		}
		y_vals[i] = normalize(get_swr(), 0, swr_fullscale, y0 - y1);
		if (i)
			y_vals[i] = (y_vals[i - 1] * (averageFactor - 1) + y_vals[i]) / averageFactor;
		i++;
	}

	if (! win->first_call)
	{
		// отрисовка фона графика и разметки
		uint_fast16_t gr_x = win->x1 + x0, gr_y = win->y1 + y0;
		colpip_fillrect(fr, DIM_X, DIM_Y, win->x1 + col1_int, win->y1 + row1_int, gr_w, gr_h, COLORMAIN_BLACK);
		colmain_line(fr, DIM_X, DIM_Y, gr_x, gr_y, gr_x, win->y1 + y1, COLORMAIN_WHITE, 0);
		colmain_line(fr, DIM_X, DIM_Y, gr_x, gr_y, win->x1 + x1, gr_y, COLORMAIN_WHITE, 0);

		char buf[5];
		uint_fast8_t l = 1, row_step = round((y0 - y1) / 3);
		local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), l++);
		colpip_string3_tbg(fr, DIM_X, DIM_Y, gr_x - SMALLCHARW3 * 2, gr_y - SMALLCHARH3 / 2, buf, COLORMAIN_WHITE);
		for(int_fast16_t yy = y0 - row_step; yy > y1; yy -= row_step)
		{
			if(yy < 0)
				break;

			colmain_line(fr, DIM_X, DIM_Y, gr_x, win->y1 + yy, win->x1 + x1, win->y1 + yy, COLORMAIN_DARKGREEN, 0);
			local_snprintf_P(buf, ARRAY_SIZE(buf), PSTR("%d"), l++);
			colpip_string3_tbg(fr, DIM_X, DIM_Y, gr_x - SMALLCHARW3 * 2, win->y1 + yy - SMALLCHARH3 / 2, buf, COLORMAIN_WHITE);
		}

		if (lbl_swr_error->visible)				// фон сообщения об ошибке
		{
			colpip_fillrect(fr, DIM_X, DIM_Y, win->x1 + col1_int, win->y1 + lbl_swr_error->y - 5, gr_w, get_label_height(lbl_swr_error) + 5, COLORMAIN_RED);
		}
		else									// маркер текущей частоты
		{
			colmain_line(fr, DIM_X, DIM_Y, gr_x + current_freq_x, gr_y, gr_x + current_freq_x, win->y1 + y1, COLORMAIN_RED, 0);
		}

		if (is_swr_scanning || swr_scan_done)	// вывод графика во время сканирования и по завершении
		{
			for(uint_fast16_t j = 2; j <= i; j ++)
				colmain_line(fr, DIM_X, DIM_Y, gr_x + j - 2, gr_y - y_vals[j - 2], gr_x + j - 1, gr_y - y_vals[j - 1], COLORMAIN_YELLOW, 1);
		}
	}
}

static void buttons_swrscan_process(void)
{
	if(gui.selected_type == TYPE_BUTTON)
	{
		window_t * win = & windows[WINDOW_SWR_SCANNER];
		button_t * btn_swr_start = find_gui_element_ref(TYPE_BUTTON, win, "btn_swr_start");
		button_t * btn_swr_OK = find_gui_element_ref(TYPE_BUTTON, win, "btn_swr_OK");

		if (gui.selected_link->link == btn_swr_start && ! strcmp(btn_swr_start->text, "Start"))
		{
			swr_scan_enable = 1;
		}
		else if (gui.selected_link->link == btn_swr_start && ! strcmp(btn_swr_start->text, "Stop"))
		{
			swr_scan_stop = 1;
		}
		else if (gui.selected_link->link == btn_swr_OK)
		{
			set_window(win, NON_VISIBLE);
			footer_buttons_state(CANCELLED);
			hamradio_set_lockmode(0);
			hamradio_disable_keyboard_redirect();
			free(y_vals);
		}
	}
}

static void buttons_tx_power_process(void)
{
	if(gui.selected_type == TYPE_BUTTON)
	{
		window_t * win = & windows[WINDOW_TX_POWER];
		button_t * btn_tx_pwr_OK = find_gui_element_ref(TYPE_BUTTON, win, "btn_tx_pwr_OK");
		if (gui.selected_link->link == btn_tx_pwr_OK)
			set_window(win, NON_VISIBLE);
	}
}

static void window_tx_power_process(void)
{
	window_t * win = & windows[WINDOW_TX_POWER];
	static slider_t * sl_pwr_level = NULL, * sl_pwr_tuner_level = NULL;
	static label_t * lbl_tx_power = NULL, * lbl_tune_power = NULL;
	static uint_fast16_t power_min, power_max;
	slider_t * sl;

	if (win->first_call)
	{
		uint_fast8_t interval = 50, col1_int = 20;
		uint_fast16_t xmax = 0, ymax = 0;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0,  44, 44, buttons_tx_power_process, 		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_TX_POWER,    	NON_VISIBLE, UINTPTR_MAX, 		"btn_tx_pwr_OK", 	 		"OK", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);
		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		slider_t sliders [] = {
			{ },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_POWER, 		"sl_pwr_level",   		CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
			{ 0, 0, 0, 0, 0, 0, ORIENTATION_HORIZONTAL, WINDOW_TX_POWER, 		"sl_pwr_tuner_level",  	CANCELLED, NON_VISIBLE, 0, 50, 255, 0, 0, },
		};
		win->sh_count = ARRAY_SIZE(sliders);
		uint_fast16_t sliders_size = sizeof(sliders);
		win->sh_ptr = malloc(sliders_size);
		memcpy(win->sh_ptr, sliders, sliders_size);

		label_t labels[] = {
		//    x, y,  parent,     		state, is_trackable, visible,   name,       		Text, font_size, 	color, 			onClickHandler
			{ },
			{ 0, 0,	WINDOW_TX_POWER,  		DISABLED,  0, NON_VISIBLE, "lbl_tx_power",    		"", FONT_MEDIUM, COLORMAIN_WHITE, },
			{ 0, 0,	WINDOW_TX_POWER,  		DISABLED,  0, NON_VISIBLE, "lbl_tune_power",  		"", FONT_MEDIUM, COLORMAIN_WHITE, },
		};
		win->lh_count = ARRAY_SIZE(labels);
		uint_fast16_t labels_size = sizeof(labels);
		win->lh_ptr = malloc(labels_size);
		memcpy(win->lh_ptr, labels, labels_size);

		sl_pwr_level = find_gui_element_ref(TYPE_SLIDER, win, "sl_pwr_level");
		sl_pwr_tuner_level = find_gui_element_ref(TYPE_SLIDER, win, "sl_pwr_tuner_level");

		lbl_tx_power = find_gui_element_ref(TYPE_LABEL, win, "lbl_tx_power");
		lbl_tune_power = find_gui_element_ref(TYPE_LABEL, win, "lbl_tune_power");

		hamradio_get_tx_power_limits(& power_min, & power_max);
		uint_fast8_t power = hamradio_get_tx_power();
		uint_fast8_t tune_power = hamradio_get_tx_tune_power();

		lbl_tx_power->x = col1_int;
		lbl_tx_power->y = interval;
		lbl_tx_power->visible = VISIBLE;
		local_snprintf_P(lbl_tx_power->text, ARRAY_SIZE(lbl_tx_power->text), PSTR("TX power  : %3d"), power);

		lbl_tune_power->x = lbl_tx_power->x;
		lbl_tune_power->y = lbl_tx_power->y + interval;
		lbl_tune_power->visible = VISIBLE;
		local_snprintf_P(lbl_tune_power->text, ARRAY_SIZE(lbl_tune_power->text), PSTR("Tune power: %3d"), tune_power);

		sl_pwr_level->x = lbl_tx_power->x + interval * 3 + interval / 2;
		sl_pwr_level->y = lbl_tx_power->y;
		sl_pwr_level->visible = VISIBLE;
		sl_pwr_level->size = 300;
		sl_pwr_level->step = 3;
		sl_pwr_level->value = normalize(power, power_min, power_max, 100);

		sl_pwr_tuner_level->x = sl_pwr_level->x;
		sl_pwr_tuner_level->y = lbl_tune_power->y;
		sl_pwr_tuner_level->visible = VISIBLE;
		sl_pwr_tuner_level->size = 300;
		sl_pwr_tuner_level->step = 3;
		sl_pwr_tuner_level->value = normalize(tune_power, power_min, power_max, 100);

		button_t * bh = find_gui_element_ref(TYPE_BUTTON, win, "btn_tx_pwr_OK");
		bh->x1 = (sl_pwr_level->x + sl_pwr_level->size + col1_int * 2) / 2 - (bh->w / 2);
		bh->y1 = lbl_tune_power->y + interval;
		bh->visible = VISIBLE;

		xmax = sl_pwr_tuner_level->x + sl_pwr_tuner_level->size + col1_int;
		ymax = bh->y1 + bh->h;
		calculate_window_position(win, xmax, ymax);
		elements_state(win);
		return;
	}

	if (gui.selected_type == TYPE_SLIDER && gui.is_tracking)
	{
		char buf[TEXT_ARRAY_SIZE];

		/* костыль через костыль */
		sl = (slider_t *) gui.selected_link->link;

		if(sl == sl_pwr_level)
		{
			uint_fast8_t power = power_min + normalize(sl->value, 0, 100, power_max - power_min);
			local_snprintf_P(lbl_tx_power->text, ARRAY_SIZE(lbl_tx_power->text), PSTR("TX power  : %3d"),power);
			hamradio_set_tx_power(power);
		}
		else if (sl == sl_pwr_tuner_level)
		{
			uint_fast8_t power = power_min + normalize(sl->value, 0, 100, power_max - power_min);
			local_snprintf_P(lbl_tune_power->text, ARRAY_SIZE(lbl_tune_power->text), PSTR("Tune power: %3d"),power);
			hamradio_set_tx_tune_power(power);
		}
	}
}

static void window_ap_mic_prof_process(void)
{
	window_t * win = & windows[WINDOW_AP_MIC_PROF];

	if (win->first_call)
	{
		uint_fast16_t x = 0, y = 0, xmax = 0, ymax = 0;
		uint_fast8_t interval = 6, col1_int = 20, row1_int = window_title_height + 20, row_count = 3, id_start, id_end;
		button_t * bh = NULL;
		win->first_call = 0;

		find_entry_area_elements(TYPE_BUTTON, win, & id_start, & id_end);

		x = col1_int;
		y = row1_int;

		for (uint_fast8_t i = id_start, r = 1; i <= id_end; i ++, r ++)
		{
			bh = (button_t *) touch_elements[i].link;
			bh->x1 = x;
			bh->y1 = y;
			bh->visible = VISIBLE;

			x = x + interval + bh->w;
			if (r >= row_count)
			{
				r = 0;
				x = col1_int;
				y = y + bh->h + interval;
			}
			xmax = (xmax > bh->x1 + bh->w) ? xmax : (bh->x1 + bh->w);
			ymax = (ymax > bh->y1 + bh->h) ? ymax : (bh->y1 + bh->h);
		}
		calculate_window_position(win, xmax, ymax);
		return;
	}
}

static void buttons_ap_mic_prof_process(void)
{

}

void gui_open_sys_menu(void)
{
	button9_handler();
}

static void btn_main_handler(void)
{
	if(gui.selected_type == TYPE_BUTTON)
	{
		window_t * winMain = & windows[WINDOW_MAIN];
		button_t * btn_Mode = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_Mode");
		button_t * btn_AF = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_AF");
		button_t * btn_AGC = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_AGC");
		button_t * btn_Freq = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_Freq");
		button_t * btn_5 = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_5");
		button_t * btn_SWRscan = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_SWRscan");
		button_t * btn_TXsett = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_TXsett");
		button_t * btn_AUDsett = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_AUDsett");
		button_t * btn_SysMenu = find_gui_element_ref(TYPE_BUTTON, winMain, "btn_SysMenu");

		if (gui.selected_link->link == btn_Mode)
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
		else if (gui.selected_link->link == btn_AF)
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
		else if (gui.selected_link->link == btn_AGC)
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
		else if (gui.selected_link->link == btn_Freq)
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
		else if (gui.selected_link->link == btn_5)
		{

		}
		else if (gui.selected_link->link == btn_SWRscan)
		{
			window_t * win = & windows[WINDOW_SWR_SCANNER];
			if (win->state == NON_VISIBLE)
			{
				set_window(win, VISIBLE);
				footer_buttons_state(DISABLED, ((button_t *)gui.selected_link)->name);
				hamradio_set_lockmode(1);
				hamradio_enable_keyboard_redirect();
			}
			else
			{
				set_window(win, NON_VISIBLE);
				footer_buttons_state(CANCELLED);
				if(swr_scan_enable)
				{
					swr_scan_enable = 0;
					hamradio_set_tune(0);
				}
				free(y_vals);
				hamradio_set_lockmode(0);
				hamradio_disable_keyboard_redirect();
			}
		}
		else if (gui.selected_link->link == btn_TXsett)
		{
			window_t * win = & windows[WINDOW_TX_SETTINGS];
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
		else if (gui.selected_link->link == btn_AUDsett)
		{

		}
		else if (gui.selected_link->link == btn_SysMenu)
		{

		}
	}
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
		window_t * win = & windows[WINDOW_SWR_SCANNER];
		if (win->state == NON_VISIBLE)
		{
			set_window(win, VISIBLE);
			footer_buttons_state(DISABLED, ((button_t *)gui.selected_link)->name);
			hamradio_set_lockmode(1);
			hamradio_enable_keyboard_redirect();
		}
		else
		{
			set_window(win, NON_VISIBLE);
			footer_buttons_state(CANCELLED);
			if(swr_scan_enable)
			{
				swr_scan_enable = 0;
				hamradio_set_tune(0);
			}
			free(y_vals);
			hamradio_set_lockmode(0);
			hamradio_disable_keyboard_redirect();
		}
	}
}

static void button7_handler(void)
{
	if(gui.selected_type == TYPE_BUTTON)
	{
		window_t * win = & windows[WINDOW_TX_SETTINGS];
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

static void button8_handler(void)
{
	if(gui.selected_type == TYPE_BUTTON)
	{
		window_t * win = & windows[WINDOW_AUDIOSETTINGS];
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
	uint_fast16_t yt, xt, y1 = 125, y2 = 145, current_place = 0, xx;
	uint_fast8_t num_places = 8, lbl_place_width = 100;

	if (win->first_call)
	{
		uint_fast8_t interval_btn = 3, id_start, id_end;
		uint_fast16_t x = 0;
		win->first_call = 0;

		button_t buttons [] = {
		//   x1, y1, w, h,  onClickHandler,   state,   	is_locked, is_trackable, parent,   	visible,      payload,	 name, 		text
			{ },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_Mode", 	 "Mode", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_AF",  	 "AF|filter", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_AGC",  	 "AGC", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_Freq",    "Freq|enter", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_5",  	 "", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_SWRscan", "SWR|scanner", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_TXsett",  "Transmit|settings", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_AUDsett", "Audio|settings", },
			{ 0, 0, 86, 44, btn_main_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MAIN, NON_VISIBLE, UINTPTR_MAX, "btn_SysMenu", "System|settings", },
		};
		win->bh_count = ARRAY_SIZE(buttons);
		uint_fast16_t buttons_size = sizeof(buttons);

		win->bh_ptr = malloc(buttons_size);
		memcpy(win->bh_ptr, buttons, buttons_size);

		for (uint_fast8_t id = 1; id < win->bh_count; id ++)
		{
			button_t * bh = & win->bh_ptr[id];
			bh->x1 = x;
			bh->y1 = WITHGUIMAXY - bh->h;
			bh->visible = VISIBLE;
			x = x + interval_btn + bh->w;
		}

		elements_state(win);
		return;
	}

	// разметка
	for(uint_fast8_t i = 1; i < num_places; i++)
	{
		uint_fast16_t x = lbl_place_width * i;
		colmain_line(fr, DIM_X, DIM_Y, x, y1, x, y2 + SMALLCHARH2, COLORMAIN_GREEN, 0);
	}

	// текущее время
#if defined (RTC1_TYPE)
	static uint_fast16_t year;
	static uint_fast8_t month, day, hour, minute, secounds;
	if(gui.timer_1sec_updated)
		board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & secounds);
	local_snprintf_P(buf, buflen, PSTR("%02d.%02d"), day, month);
	xx = current_place * lbl_place_width + lbl_place_width / 2;
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y1, buf, COLORMAIN_WHITE);
	local_snprintf_P(buf, buflen, PSTR("%02d%c%02d"), hour, ((secounds & 1) ? ' ' : ':'), minute);
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y2, buf, COLORMAIN_WHITE);
#endif 	/* defined (RTC1_TYPE) */

	current_place++;

	// напряжение питания
#if WITHVOLTLEVEL
	static ldiv_t v;
	if(gui.timer_1sec_updated)
		v = ldiv(hamradio_get_volt_value(), 10);
	local_snprintf_P(buf, buflen, PSTR("%d.%1dV"), v.quot, v.rem);
	xx = current_place * lbl_place_width + lbl_place_width / 2;
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, hamradio_get_tx() ? y1 : (y1 + (y2 - y1) / 2), buf, COLORMAIN_WHITE);
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

		colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y2, buf, COLORMAIN_WHITE);
	}
#endif /* WITHCURRLEVEL */

	current_place++;

	// ширина панорамы
#if WITHIF4DSP
	static int_fast32_t z;
	if(gui.timer_1sec_updated)
		z = display_zoomedbw() / 1000;
	local_snprintf_P(buf, buflen, PSTR("SPAN"));
	xx = current_place * lbl_place_width + lbl_place_width / 2;
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y1, buf, COLORMAIN_WHITE);
	local_snprintf_P(buf, buflen, PSTR("%dk"), z);
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y2, buf, COLORMAIN_WHITE);
#endif /* WITHIF4DSP */

	current_place++;

	// параметры полосы пропускания фильтра
	static uint_fast8_t bp_type, bp_low, bp_high;
	if(gui.timer_1sec_updated)
	{
		bp_high = hamradio_get_high_bp(0);
		bp_low = hamradio_get_low_bp(0) * 10;
		bp_type = hamradio_get_bp_type();
		bp_high = bp_type ? bp_high * 100 : bp_high * 10;
	}
	local_snprintf_P(buf, buflen, PSTR("AF"));
	xx = current_place * lbl_place_width + 7;
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx, y1 + (y2 - y1) / 2, buf, COLORMAIN_WHITE);
	xx += SMALLCHARW2 * 3;
	local_snprintf_P(buf, buflen, bp_type ? (PSTR("L %d")) : (PSTR("W %d")), bp_low);
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx, y1, buf, COLORMAIN_WHITE);
	local_snprintf_P(buf, buflen, bp_type ? (PSTR("H %d")) : (PSTR("P %d")), bp_high);
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx, y2, buf, COLORMAIN_WHITE);

	current_place++;

	// значение сдвига частоты
	static int_fast16_t if_shift;
	if (gui.timer_1sec_updated)
		if_shift = hamradio_get_if_shift();
	xx = current_place * lbl_place_width + lbl_place_width / 2;
	if (if_shift)
	{
		local_snprintf_P(buf, buflen, PSTR("IF shift"));
		colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y1, buf, COLORMAIN_WHITE);
		local_snprintf_P(buf, buflen, if_shift == 0 ? PSTR("%d") : PSTR("%+dk"), if_shift);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y2, buf, COLORMAIN_WHITE);
	}
	else
	{
		local_snprintf_P(buf, buflen, PSTR("IF shift"));
		colpip_string2_tbg(fr, DIM_X, DIM_Y, xx - strwidth2(buf) / 2, y1 + (y2 - y1) / 2, buf, COLORMAIN_GRAY);
	}

//	#if WITHTHERMOLEVEL	// температура выходных транзисторов (при передаче)
//		static ldiv_t t;
//		if (hamradio_get_tx())// && gui.timer_1sec_updated)
//		{
//			t = ldiv(hamradio_get_temperature_value(), 10);
//			local_snprintf_P(buf, buflen, PSTR("%d.%dC "), t.quot, t.rem);
//			PRINTF("%s\n", buf);		// пока вывод в консоль
//		}
//	#endif /* WITHTHERMOLEVEL */

	gui.timer_1sec_updated = 0;
}

/* Кнопка */
static void draw_button(const button_t * const bh)
{
	PACKEDCOLORMAIN_T * bg = NULL;
	PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
	window_t * win = & windows[bh->parent];
	uint_fast8_t i = 0;
	static const char delimeters [] = "|";
	uint_fast16_t x1 = win->x1 + bh->x1;
	uint_fast16_t y1 = win->y1 + bh->y1;


	btn_bg_t * b1 = NULL;
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
#if GUI_OLDBUTTONSTYLE
		colpip_rect(fr, DIM_X, DIM_Y, x1, y1, x1 + bh->w, y1 + bh->h - 2, bh->state == PRESSED ? c2 : c1, 1);
		colpip_rect(fr, DIM_X, DIM_Y, x1, y1, x1 + bh->w, y1 + bh->h - 1, COLORMAIN_GRAY, 0);
		colpip_rect(fr, DIM_X, DIM_Y, x1 + 2, y1 + 2, x1 + bh->w - 2, y1 + bh->h - 3, COLORMAIN_BLACK, 0);
#else
		colmain_rounded_rect(fr, DIM_X, DIM_Y, x1, y1, x1 + bh->w, y1 + bh->h - 2, button_round_radius, bh->state == PRESSED ? c2 : c1, 1);
		colmain_rounded_rect(fr, DIM_X, DIM_Y, x1, y1, x1 + bh->w, y1 + bh->h - 1, button_round_radius, COLORMAIN_GRAY, 0);
		colmain_rounded_rect(fr, DIM_X, DIM_Y, x1 + 2, y1 + 2, x1 + bh->w - 2, y1 + bh->h - 3, button_round_radius, COLORMAIN_BLACK, 0);
#endif /* GUI_OLDBUTTONSTYLE */
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
#if GUI_OLDBUTTONSTYLE
		colpip_plot(fr, DIM_X, DIM_Y, x1, y1, bg, bh->w, bh->h);
#else
		PACKEDCOLORMAIN_T * src = NULL, * dst = NULL, * row = NULL;
		for (uint16_t yy = y1, yb = 0; yy < y1 + bh->h; yy++, yb++)
		{
			row = colmain_mem_at(bg, b1->w, b1->h, 0, yb);
			if (* row == GUI_DEFAULTCOLOR)										// если в первой позиции строки буфера не прозрачный цвет,
			{																	// скопировать ее целиком, иначе попиксельно с проверкой
				for (uint16_t xx = x1, xb = 0; xx < x1 + bh->w; xx++, xb++)
				{
					src = colmain_mem_at(bg, b1->w, b1->h, xb, yb);
					if (* src == GUI_DEFAULTCOLOR)
						continue;
					dst = colmain_mem_at(fr, DIM_X, DIM_Y, xx, yy);
					memcpy(dst, src, sizeof(PACKEDCOLORMAIN_T));
				}
			}
			else
			{
				dst = colmain_mem_at(fr, DIM_X, DIM_Y, x1, yy);
				memcpy(dst, row, b1->w * sizeof(PACKEDCOLORMAIN_T));
			}
		}
#endif /* GUI_OLDBUTTONSTYLE */
	}

#if GUI_OLDBUTTONSTYLE
	uint_fast8_t shift = bh->state == PRESSED ? 1 : 0;
#else
	uint_fast8_t shift = 0;
#endif /* GUI_OLDBUTTONSTYLE */

	if (strchr(bh->text, delimeters[0]) == NULL)
	{
		/* Однострочная надпись */
		colpip_string2_tbg(fr, DIM_X, DIM_Y, shift + x1 + (bh->w - (strwidth2(bh->text))) / 2,
				shift + y1 + (bh->h - SMALLCHARH2) / 2, bh->text, COLORMAIN_BLACK);
	}
	else
	{
		/* Двухстрочная надпись */
		uint_fast8_t j = (bh->h - SMALLCHARH2 * 2) / 2;
		char buf [TEXT_ARRAY_SIZE];
		strcpy(buf, bh->text);
		char * text2 = strtok(buf, delimeters);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, shift + x1 + (bh->w - (strwidth2(text2))) / 2,
				shift + y1 + j, text2, COLORMAIN_BLACK);

		text2 = strtok(NULL, delimeters);
		colpip_string2_tbg(fr, DIM_X, DIM_Y, shift + x1 + (bh->w - (strwidth2(text2))) / 2,
				shift + bh->h + y1 - SMALLCHARH2 - j, text2, COLORMAIN_BLACK);
	}
}

static void draw_slider(slider_t * sl)
{
	PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
	window_t * win = &windows[sl->parent];

	if (sl->orientation)		// ORIENTATION_HORIZONTAL
	{
		if (sl->value_old != sl->value)
		{
			uint_fast16_t mid_w = sl->y + sliders_width / 2;
			sl->value_p = sl->x + sl->size * sl->value / 100;
			sl->y1_p = mid_w - sliders_w;
			sl->x1_p = sl->value_p - sliders_h;
			sl->y2_p = mid_w + sliders_w;
			sl->x2_p = sl->value_p + sliders_h;
			sl->value_old = sl->value;
		}
		colpip_rect(fr, DIM_X, DIM_Y, win->x1 + sl->x, win->y1 + sl->y,  win->x1 + sl->x + sl->size, win->y1 + sl->y + sliders_width, COLORMAIN_BLACK, 1);
		colpip_rect(fr, DIM_X, DIM_Y, win->x1 + sl->x, win->y1 + sl->y,  win->x1 + sl->x + sl->size, win->y1 + sl->y + sliders_width, COLORMAIN_WHITE, 0);
		colpip_rect(fr, DIM_X, DIM_Y, win->x1 + sl->x1_p, win->y1 + sl->y1_p,  win->x1 + sl->x2_p, win->y1 + sl->y2_p, sl->state == PRESSED ? COLOR_BUTTON_PR_NON_LOCKED : COLOR_BUTTON_NON_LOCKED, 1);
		colmain_line(fr, DIM_X, DIM_Y, win->x1 + sl->value_p, win->y1 + sl->y1_p,  win->x1 + sl->value_p, win->y1 + sl->y2_p, COLORMAIN_WHITE, 0);
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
		colpip_rect(fr, DIM_X, DIM_Y, win->x1 + sl->x + 1, win->y1 + sl->y + 1, win->x1 + sl->x + sliders_width - 1, win->y1 + sl->y + sl->size - 1, COLORMAIN_BLACK, 1);
		colpip_rect(fr, DIM_X, DIM_Y, win->x1 + sl->x, win->y1 + sl->y, win->x1 + sl->x + sliders_width, win->y1 + sl->y + sl->size, COLORMAIN_WHITE, 0);
		colpip_rect(fr, DIM_X, DIM_Y, win->x1 + sl->x1_p, win->y1 + sl->y1_p, win->x1 + sl->x2_p, win->y1 + sl->y2_p, sl->state == PRESSED ? COLOR_BUTTON_PR_NON_LOCKED : COLOR_BUTTON_NON_LOCKED, 1);
		colmain_line(fr, DIM_X, DIM_Y, win->x1 + sl->x1_p, win->y1 + sl->value_p, win->x1 + sl->x2_p, win->y1 + sl->value_p, COLORMAIN_WHITE, 0);
	}
}

static void fill_button_bg_buf(btn_bg_t * v)
{
	PACKEDCOLORMAIN_T * buf;
	uint_fast8_t w, h;

	w = v->w;
	h = v->h;
	size_t s = GXSIZE(w, h) * sizeof (PACKEDCOLORMAIN_T);

	v->bg_non_pressed = 	(PACKEDCOLORMAIN_T *) calloc(1, s);
	v->bg_pressed = 		(PACKEDCOLORMAIN_T *) calloc(1, s);
	v->bg_locked = 			(PACKEDCOLORMAIN_T *) calloc(1, s);
	v->bg_locked_pressed = 	(PACKEDCOLORMAIN_T *) calloc(1, s);
	v->bg_disabled = 		(PACKEDCOLORMAIN_T *) calloc(1, s);

	buf = v->bg_non_pressed;
	ASSERT(buf != NULL);
#if GUI_OLDBUTTONSTYLE
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_NON_LOCKED, 1);
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORMAIN_GRAY, 0);
	colpip_rect(buf, w, h, 2, 2, w - 3, h - 3, COLORMAIN_BLACK, 0);
#else
	memset(buf, GUI_DEFAULTCOLOR, s);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_NON_LOCKED, 1);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLORMAIN_GRAY, 0);
	colmain_rounded_rect(buf, w, h, 2, 2, w - 3, h - 3, button_round_radius, COLORMAIN_BLACK, 0);
#endif /* GUI_OLDBUTTONSTYLE */

	buf = v->bg_pressed;
	ASSERT(buf != NULL);
#if GUI_OLDBUTTONSTYLE
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_PR_NON_LOCKED, 1);
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORMAIN_GRAY, 0);
	colmain_line(buf, w, h, 2, 3, w - 3, 3, COLORMAIN_BLACK, 0);
	colmain_line(buf, w, h, 2, 2, w - 3, 2, COLORMAIN_BLACK, 0);
	colmain_line(buf, w, h, 3, 3, 3, h - 3, COLORMAIN_BLACK, 0);
	colmain_line(buf, w, h, 2, 2, 2, h - 2, COLORMAIN_BLACK, 0);
#else
	memset(buf, GUI_DEFAULTCOLOR, s);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_PR_NON_LOCKED, 1);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLORMAIN_GRAY, 0);
	colmain_rounded_rect(buf, w, h, 2, 2, w - 3, h - 3, button_round_radius, COLORMAIN_BLACK, 0);
#endif /* GUI_OLDBUTTONSTYLE */

	buf = v->bg_locked;
	ASSERT(buf != NULL);
#if GUI_OLDBUTTONSTYLE
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_LOCKED, 1);
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORMAIN_GRAY, 0);
	colpip_rect(buf, w, h, 2, 2, w - 3, h - 3, COLORMAIN_BLACK, 0);
#else
	memset(buf, GUI_DEFAULTCOLOR, s);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_LOCKED, 1);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLORMAIN_GRAY, 0);
	colmain_rounded_rect(buf, w, h, 2, 2, w - 3, h - 3, button_round_radius, COLORMAIN_BLACK, 0);
#endif /* GUI_OLDBUTTONSTYLE */

	buf = v->bg_locked_pressed;
	ASSERT(buf != NULL);
#if GUI_OLDBUTTONSTYLE
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_PR_LOCKED, 1);
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORMAIN_GRAY, 0);
	colmain_line(buf, w, h, 2, 3, w - 3, 3, COLORMAIN_BLACK, 0);
	colmain_line(buf, w, h, 2, 2, w - 3, 2, COLORMAIN_BLACK, 0);
	colmain_line(buf, w, h, 3, 3, 3, h - 3, COLORMAIN_BLACK, 0);
	colmain_line(buf, w, h, 2, 2, 2, h - 2, COLORMAIN_BLACK, 0);
#else
	memset(buf, GUI_DEFAULTCOLOR, s);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_PR_LOCKED, 1);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLORMAIN_GRAY, 0);
	colmain_rounded_rect(buf, w, h, 2, 2, w - 3, h - 3, button_round_radius, COLORMAIN_BLACK, 0);
#endif /* GUI_OLDBUTTONSTYLE */

	buf = v->bg_disabled;
	ASSERT(buf != NULL);
#if GUI_OLDBUTTONSTYLE
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_DISABLED, 1);
	colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORMAIN_GRAY, 0);
	colpip_rect(buf, w, h, 2, 2, w - 3, h - 3, COLORMAIN_BLACK, 0);
#else
	memset(buf, GUI_DEFAULTCOLOR, s);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_DISABLED, 1);
	colmain_rounded_rect(buf, w, h, 0, 0, w - 1, h - 1, button_round_radius, COLORMAIN_GRAY, 0);
	colmain_rounded_rect(buf, w, h, 2, 2, w - 3, h - 3, button_round_radius, COLORMAIN_BLACK, 0);
#endif /* GUI_OLDBUTTONSTYLE */
}

void gui_initialize (void)
{
	uint_fast8_t i = 0;
	window_t * win = & windows[WINDOW_MAIN];

	InitializeListHead(& windows_list);

	set_window(win, VISIBLE);

	do {
		fill_button_bg_buf(& btn_bg[i]);
	} while (++i < BG_COUNT) ;
}

static void update_touch_list(void)
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
	uint16_t v = sl->value + round((sl->orientation ? gui.vector_move_x : gui.vector_move_y) / sl->step);
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
	static window_t * w = NULL;

#if defined (TSC1_TYPE)
	if (board_tsc_getxy(& tx, & ty))
	{
		gui.last_pressed_x = tx;
		gui.last_pressed_y = ty;
		gui.is_touching_screen = 1;
//		debug_printf_P(PSTR("last x/y=%d/%d\n"), gui.last_pressed_x, gui.last_pressed_y);
		update_touch_list();
	}
	else
#endif /* defined (TSC1_TYPE) */
	{
		gui.is_touching_screen = 0;
		gui.is_after_touch = 0;
	}

	PLIST_ENTRY t = windows_list.Flink;
	const window_t * const win1 = CONTAINING_RECORD(t, window_t, item);
	t = windows_list.Blink;
	const window_t * const win2 = CONTAINING_RECORD(t, window_t, item);

	if (gui.state == CANCELLED && gui.is_touching_screen && ! gui.is_after_touch)
	{
		for (uint_fast8_t i = 0; i < touch_count; i++)
		{
			p = & touch_elements[i];
			w = p->win;
			uint_fast16_t x1 = p->x1 + w->x1, y1 = p->y1 + w->y1;
			uint_fast16_t x2 = p->x2 + w->x1, y2 = p->y2 + w->y1;

			if (x1 < gui.last_pressed_x && x2 > gui.last_pressed_x && y1 < gui.last_pressed_y && y2 > gui.last_pressed_y
					&& p->state != DISABLED && p->visible == VISIBLE && (p->win == win1 || p->win == win2))
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
//				debug_printf_P(PSTR("move x: %d, move y: %d\n"), gui.vector_move_x, gui.vector_move_y);
			}
			p->state = PRESSED;
			set_state_record(p);

			x_old = gui.last_pressed_x;
			y_old = gui.last_pressed_y;
		}
		else if (w->x1 + p->x1 < gui.last_pressed_x && w->x1 + p->x2 > gui.last_pressed_x
				&& w->y1 + p->y1 < gui.last_pressed_y && w->y1 + p->y2 > gui.last_pressed_y && ! gui.is_after_touch)
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

	for (PLIST_ENTRY t = windows_list.Blink; t != & windows_list; t = t->Blink)
	{
		const window_t * const win = CONTAINING_RECORD(t, window_t, item);
		uint_fast8_t f = win->first_call;

		if (win->state == VISIBLE)
		{
			// при открытии окна рассчитываются экранные координаты самого окна и его child элементов
			if (! f)
			{
				if (win->window_id != WINDOW_MAIN)
				{
					ASSERT(win->w > 0 || win->h > 0);
#if GUI_TRANSPARENT_WINDOWS
					display_transparency(win->x1, strcmp(win->name, "") ? (win->y1 + window_title_height) : win->y1, win->x1 + win->w - 1, win->y1 + win->h - 1, alpha);
#else
					colpip_fillrect(fr, DIM_X, DIM_Y, win->x1, strcmp(win->name, "") ? (win->y1 + window_title_height) : win->y1, win->w, win->h, GUI_WINDOWBGCOLOR);
#endif /* GUI_TRANSPARENT_WINDOWS */
				}
			}

			// запуск процедуры фоновой обработки для окна
			win->onVisibleProcess();

			if (! f)
			{
				// вывод заголовка окна
				if (strcmp(win->name, ""))
				{
					colpip_fillrect(fr, DIM_X, DIM_Y, win->x1, win->y1, win->w, window_title_height, 20);
					colpip_string_tbg(fr, DIM_X, DIM_Y, win->x1 + 20, win->y1 + 5, win->name, COLORMAIN_BLACK);
				}

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
								colpip_string_tbg(fr, DIM_X, DIM_Y,  win->x1 +lh->x, win->y1 + lh->y, lh->text, lh->color);
							else if (lh->font_size == FONT_MEDIUM)
								colpip_string2_tbg(fr, DIM_X, DIM_Y, win->x1 +lh->x, win->y1 + lh->y, lh->text, lh->color);
							else if (lh->font_size == FONT_SMALL)
								colpip_string3_tbg(fr, DIM_X, DIM_Y, win->x1 +lh->x, win->y1 + lh->y, lh->text, lh->color);
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
