/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "audio.h"
#include "display2.h"

#include "formats.h"

#include <string.h>
#include <math.h>
#include "src/gui/gui.h"

#if WITHALTERNATIVEFONTS
	#include "../display/fonts/ub_fonts.h"
#endif /* WITHALTERNATIVEFONTS */

#define WITHPLACEHOLDERS 1	//  отображение макета с еще незанятыми полями

#if 0

	#undef PRINTF
	#undef ASSERT
	#undef VERIFY

	#define PRINTF(...)	do {} while (0)
	#define ASSERT(v) ((void) (0))
	#define VERIFY(v) ((void) (v))

//	#define TRACE0(f)			do {} while (0)
//	#define TRACE1(f,a1)			do {} while (0)
//	#define TRACE2(f,a1,a2)			do {} while (0)
//	#define TRACE3(f,a1,a2,a3)		do {} while (0)
//	#define TRACE4(f,a1,a2,a3,a4)		do {} while (0)
//	#define TRACE5(f,a1,a2,a3,a4,a5)		do {} while (0)

#endif

#if LCDMODE_LTDC

#include "display/fontmaps.h"

	static PACKEDCOLORMAIN_T * getscratchwnd(void);

#elif WITHSPECTRUMWF

	static PACKEDCOLORMAIN_T * getscratchwnd(void);

#endif /* LCDMODE_LTDC */

static const COLORMAIN_T colors_2state_alt [2] = { COLORPIP_GRAY, COLORPIP_WHITE, };
static const COLORMAIN_T color_alt_red = COLORMAIN_RED;

void layout_label1_medium(uint_fast8_t xgrid, uint_fast8_t ygrid, const char * str, size_t slen, uint_fast8_t chars_W2, COLORMAIN_T color_fg, COLORMAIN_T color_bg);

#if WITHALTERNATIVELAYOUT

#if SMALLCHARW2

typedef struct {
	uint_fast8_t chars_W2;
	const COLORMAIN_T * pcolor;
	PACKEDCOLORMAIN_T * label_bg;	/* буфер */
	size_t size;					/* размер для cache flush */
	uint_fast16_t w;
	uint_fast16_t h;
} label_bg_t;

static label_bg_t label_bg [] = {
		{ 5, & colors_2state_alt [0], },
		{ 5, & colors_2state_alt [1], },
		{ 5, & color_alt_red, 		  },
		{ 7, & colors_2state_alt [0], },
		{ 7, & colors_2state_alt [1], },
};

static void layout_init(uint_fast8_t xgrid, uint_fast8_t ygrid, dctx_t * pctx)
{
	uint_fast8_t i = 0;

	do {
		label_bg_t * const lbl = & label_bg [i];
		lbl->w = lbl->chars_W2 * SMALLCHARW2;
		lbl->h = SMALLCHARH2 + 6;
		lbl->size = GXSIZE(lbl->w, lbl->h) * sizeof (PACKEDCOLORMAIN_T);
		lbl->label_bg = (PACKEDCOLORMAIN_T *) malloc(lbl->size);
		ASSERT(lbl->label_bg != NULL);
		colmain_fillrect(lbl->label_bg, lbl->w, lbl->h, 0, 0, lbl->w, lbl->h, COLORMAIN_BLACK);
		colmain_rounded_rect(lbl->label_bg, lbl->w, lbl->h, 0, 0, lbl->w - 1, lbl->h - 1, 5, * lbl->pcolor, 1);
	} while (++ i < ARRAY_SIZE(label_bg));
}

void layout_label1_medium(uint_fast8_t xgrid, uint_fast8_t ygrid, const char * str, size_t slen, uint_fast8_t chars_W2, COLORMAIN_T color_fg, COLORMAIN_T color_bg)
{
	PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
	uint_fast16_t xx = GRID2X(xgrid);
	uint_fast16_t yy = GRID2Y(ygrid);
	label_bg_t * lbl_bg = NULL;
	uint_fast8_t i = 0;
	char buf [slen + 1];
	strcpy(buf, str);
	strtrim(buf);
#if WITHALTERNATIVEFONTS
	const uint_fast16_t width_str = getwidth_Pstring(buf, & gothic_12x16);
#else
	const uint_fast16_t width_str = strwidth2(buf);
#endif /* WITHALTERNATIVEFONTS */

	const uint_fast16_t width_p = chars_W2 * SMALLCHARW2;

	if (! width_str)
		return;

	do {
		label_bg_t * const lbl = & label_bg [i];
		if (lbl->chars_W2 == chars_W2 && * lbl->pcolor == color_bg)
		{
			lbl_bg = lbl;
			break;
		}
	} while (++ i < ARRAY_SIZE(label_bg));

	if (lbl_bg != NULL)
		colpip_plot((uintptr_t) fr, GXSIZE(DIM_X, DIM_Y), fr, DIM_X, DIM_Y, xx, yy,
				(uintptr_t) lbl_bg->label_bg, lbl_bg->size, lbl_bg->label_bg, lbl_bg->w, lbl_bg->h);
	else
		colmain_rounded_rect(fr, DIM_X, DIM_Y, xx, yy, xx + width_p, yy + SMALLCHARH2 + 5, 5, color_bg, 1);

#if WITHALTERNATIVEFONTS
	UB_Font_DrawPString(fr, DIM_X, DIM_Y, xx + (width_p - width_str) / 2 , yy + 2, buf, & gothic_12x16, color_fg);
#else
	colpip_string2_tbg(fr, DIM_X, DIM_Y, xx + (width_p - width_str) / 2 , yy + 4, buf, color_fg);
#endif /* WITHALTERNATIVEFONTS */

}
#endif /* SMALLCHARW2 */
#endif /* WITHALTERNATIVELAYOUT */

static void display2_af_spectre15_init(uint_fast8_t xgrid, uint_fast8_t ygrid, dctx_t * pctx);		// вызывать после display2_smeter15_init
static void display2_af_spectre15_latch(uint_fast8_t xgrid, uint_fast8_t ygrid, dctx_t * pctx);
static void display2_af_spectre15(uint_fast8_t xgrid, uint_fast8_t ygrid, dctx_t * pctx);

#if COLORSTYLE_RED
	static uint_fast8_t glob_colorstyle = GRADIENT_BLACK_RED;
#else /* COLORSTYLE_RED */
	static uint_fast8_t glob_colorstyle = GRADIENT_BLUE_YELLOW_RED;
#endif /* COLORSTYLE_RED */

#if WITHSPECTRUMWF
// построение цветных градиентов от UA3REO
// get color from signal strength
// Get FFT color warmth (blue to red)
COLOR24_T colorgradient(unsigned pos, unsigned maxpos)
{
	uint_fast8_t red = 0;
	uint_fast8_t green = 0;
	uint_fast8_t blue = 0;
	//blue -> yellow -> red
	if (glob_colorstyle == GRADIENT_BLUE_YELLOW_RED)
	{
		// r g b
		// 0 0 0
		// 0 0 255
		// 255 255 0
		// 255 0 0
		// contrast of each of the 3 zones, the total should be 1.0f
		const float32_t contrast1 = 0.1f;
		const float32_t contrast2 = 0.4f;
		const float32_t contrast3 = 0.5f;

		if (pos < maxpos * contrast1)
		{
			blue = (uint_fast8_t) (pos * 255 / (maxpos * contrast1));
		}
		else if (pos < maxpos * (contrast1 + contrast2))
		{
			green = (uint_fast8_t) ((pos - maxpos * contrast1) * 255 / ((maxpos - maxpos * contrast1) * (contrast1 + contrast2)));
			red = green;
			blue = 255 - green;
		}
		else
		{
			red = 255;
			blue = 0;
			green = (uint_fast8_t) (255 - (pos - (maxpos * (contrast1 + contrast2))) * 255 / ((maxpos - (maxpos * (contrast1 + contrast2))) * (contrast1 + contrast2 + contrast3)));
		}
		return COLOR24(red, green, blue);
	}
	//black -> yellow -> red
	if (glob_colorstyle == GRADIENT_BLACK_YELLOW_RED)
	{
		// r g b
		// 0 0 0
		// 255 255 0
		// 255 0 0
		// contrast of each of the 2 zones, the total should be 1.0f
		const float32_t contrast1 = 0.5f;
		const float32_t contrast2 = 0.5f;

		if (pos < maxpos * contrast1)
		{
			red = (uint_fast8_t) (pos * 255 / (maxpos * contrast1));
			green = (uint_fast8_t) (pos * 255 / (maxpos * contrast1));
			blue = 0;
		}
		else
		{
			red = 255;
			blue = 0;
			green = (uint_fast8_t) (255 - (pos - (maxpos * (contrast1))) * 255 / ((maxpos - (maxpos * (contrast1))) * (contrast1 + contrast2)));
		}
		return COLOR24(red, green, blue);
	}
	//black -> yellow -> green
	if (glob_colorstyle == GRADIENT_BLACK_YELLOW_GREEN)
	{
		// r g b
		// 0 0 0
		// 255 255 0
		// 0 255 0
		// contrast of each of the 2 zones, the total should be 1.0f
		const float32_t contrast1 = 0.5f;
		const float32_t contrast2 = 0.5f;

		if (pos < maxpos * contrast1)
		{
			red = (uint_fast8_t) (pos * 255 / (maxpos * contrast1));
			green = (uint_fast8_t) (pos * 255 / (maxpos * contrast1));
			blue = 0;
		}
		else
		{
			green = 255;
			blue = 0;
			red = (uint_fast8_t) (255 - (pos - (maxpos * (contrast1))) * 255 / ((maxpos - (maxpos * (contrast1))) * (contrast1 + contrast2)));
		}
		return COLOR24(red, green, blue);
	}
	//black -> red
	if (glob_colorstyle == GRADIENT_BLACK_RED)
	{
		// r g b
		// 0 0 0
		// 255 0 0

		if (pos <= maxpos)
		{
			red = (uint_fast8_t) (pos * 255 / maxpos);
		}
		return COLOR24(red, green, blue);
	}
	//black -> green
	if (glob_colorstyle == GRADIENT_BLACK_GREEN)
	{
		// r g b
		// 0 0 0
		// 0 255 0

		if (pos <= maxpos)
		{
			green = (uint_fast8_t) (pos * 255 / maxpos);
		}
		return COLOR24(red, green, blue);
	}
	//black -> blue
	if (glob_colorstyle == GRADIENT_BLACK_BLUE)
	{
		// r g b
		// 0 0 0
		// 0 0 255

		if (pos <= maxpos)
		{
			blue = (uint_fast8_t) (pos * 255 / maxpos);
		}
		return COLOR24(red, green, blue);
	}
	//black -> white
	if (glob_colorstyle == GRADIENT_BLACK_WHITE)
	{
		// r g b
		// 0 0 0
		// 255 255 255

		if (pos <= maxpos)
		{
			red = (uint_fast8_t) (pos * 255 / maxpos);
			green = red;
			blue = red;
		}
		return COLOR24(red, green, blue);
	}
	//unknown
	return COLOR24(255, 255, 255);
}
#else /* WITHSPECTRUMWF */
COLOR24_T colorgradient(unsigned pos, unsigned maxpos)
{
	return COLOR24(255, 255, 255);
}
#endif /* WITHSPECTRUMWF */


/* стркутура хранит цвета элементов дизайна. Возможно третье поле - для анталиасингового формирования изображения */
typedef struct colorpair_tag
{
	COLORMAIN_T fg, bg;
} COLORPAIR_T;

// todo: учесть LCDMODE_COLORED

// Параметры отображения состояния прием/пеердача
static const COLORPAIR_T colors_2rxtx [2] =
{
	{	COLORMAIN_GREEN,	COLORMAIN_BLACK,	},	// RX
	{	COLORMAIN_RED,		COLORMAIN_BLACK,	},	// TX
};

// Параметры отображения состояний из трех вариантов
static const COLORPAIR_T colors_4state [4] =
{
	{	LABELINACTIVETEXT,	LABELINACTIVEBACK,	},
	{	LABELACTIVETEXT,	LABELACTIVEBACK,	},
	{	LABELACTIVETEXT,	LABELACTIVEBACK,	},
	{	LABELACTIVETEXT,	LABELACTIVEBACK,	},
};

// Параметры отображения состояний из двух вариантов
static const COLORPAIR_T colors_2state [2] =
{
	{	LABELINACTIVETEXT,	LABELINACTIVEBACK,	},
	{	LABELACTIVETEXT,	LABELACTIVEBACK,	},
};

// Параметры отображения текстов без вариантов
static const COLORPAIR_T colors_1state [1] =
{
	{	LABELTEXT,	LABELBACK,	},
};

// Параметры отображения состояний FUNC MENU из двух вариантов
static const COLORPAIR_T colors_2fmenu [2] =
{
	{	FMENUINACTIVETEXT,	FMENUINACTIVEBACK,	},
	{	FMENUACTIVETEXT,	FMENUACTIVEBACK,	},
};

// Параметры отображения текстов без вариантов
static const COLORPAIR_T colors_1fmenu [1] =
{
	{	FMENUTEXT,	FMENUBACK,	},
};

// Параметры отображения текстов без вариантов
// синий
static const COLORPAIR_T colors_1stateBlue [1] =
{
	{	DESIGNBIGCOLORB,	LABELBACK,	},
};

// Параметры отображения частоты дополнительного приемника
// синий
static const COLORPAIR_T colors_2freqB [2] =
{
	{	DESIGNBIGCOLORBINACTIVE,	LABELBACK,	},
	{	COLORMAIN_YELLOW,	LABELBACK,	},
};
// Параметры отображения режима дополнительного приемника
// синий
static const COLORPAIR_T colors_2modeB [2] =
{
	{	DESIGNBIGCOLORBINACTIVE,	LABELBACK,	},
	{	DESIGNBIGCOLORB,	LABELBACK,	},
};

// Параметры отображения частоты основного приемника
static const COLORPAIR_T colors_1freq [1] =
{
	{	DESIGNBIGCOLOR,	LABELBACK,	},
};

// Параметры отображения режима основного приемника
static const COLORPAIR_T colors_1mode [1] =
{
	{	DESIGNBIGCOLOR,	LABELBACK,	},
};

#if (WITHSPECTRUMWF && ! LCDMODE_HD44780 && ! LCDMODE_DUMMY) || WITHAFSPECTRE

// Тестовая функция - прототип для элементов отображения
static void
display2_testvidget(
	uint_fast8_t xcell,
	uint_fast8_t ycell,
	dctx_t * pctx
	)
{
	const uint_fast16_t x = GRID2X(xcell);
	const uint_fast16_t y = GRID2X(ycell);
	//colmain_fillrect(colmain_fb_draw(), DIM_X, DIM_Y, x, y, 100, 100, COLORMAIN_GREEN);

	colpip_string_tbg(colmain_fb_draw(), DIM_X, DIM_Y, x, y + 0, "Hello", COLORMAIN_WHITE);
	colpip_string_x2_tbg(colmain_fb_draw(), DIM_X, DIM_Y, x, y + 20, "Test", COLORMAIN_WHITE);
	colpip_string_tbg(colmain_fb_draw(), DIM_X, DIM_Y, x, y + 50, "Test", COLORMAIN_WHITE);

}
#endif /* (WITHSPECTRUMWF && ! LCDMODE_HD44780 && ! LCDMODE_DUMMY) || WITHAFSPECTRE */

// todo: switch off -Wunused-function

// формирование данных спектра для последующего отображения
// спектра или водопада
static void display2_latchwaterfall(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	);
static void display2_wfl_init(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);
static void display2_spectrum(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	);
static void display2_waterfall(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	);
static void display2_colorbuff(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	);
// Отображение шкалы S-метра и других измерителей
static void display2_legend(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	);
// Отображение шкалы S-метра
static void display2_legend_rx(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	);
// Отображение шкалы SWR-метра и других измерителе
static void display2_legend_tx(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	);

// Параметры отображения спектра и водопада

#if 1
	// полностью частота до килогерц
	static int_fast16_t glob_gridstep = 10000; //1 * glob_griddigit;	// 10, 20. 50 kHz - шаг сетки для рисования
	static const int_fast16_t glob_griddigit = 1000;	// 1 kHz - точность отображения частоты на сетке
	static const int glob_gridwc = 1;
	static const int_fast32_t glob_gridmod = INT32_MAX;	// 10 ^ glob_gridwc
	static const char FLASHMEM  gridfmt_2 [] = "%*ld";
#else
	// Дестки килогерц без мегагерц
	static int_fast16_t glob_gridstep = 10000; //1 * glob_griddigit;	// 10, 20. 50 kHz - шаг сетки для рисования
	static const int_fast16_t glob_griddigit = 10000;	// 10 kHz - точность отображения частоты на сетке
	static const int glob_gridwc = 2;
	static const int_fast32_t glob_gridmod = 1;	// 10 ^ glob_gridwc
	static const char FLASHMEM  gridfmt_2 [] = ".%0*ld";
#endif

// waterfall/spectrum parameters
static uint_fast8_t glob_view_style;		/* стиль отображения спектра и панорамы */
static uint_fast8_t gview3dss_mark;			/* Для VIEW_3DSS - индикация полосы пропускания на спектре */

static int_fast16_t glob_topdb = 30;		/* верхний предел FFT */
static int_fast16_t glob_bottomdb = 130;	/* нижний предел FFT */

static int_fast16_t glob_topdbwf = 0;		/* верхний предел FFT */
static int_fast16_t glob_bottomdbwf = 137;	/* нижний предел FFT */
static uint_fast8_t glob_wflevelsep;		/* чувствительность водопада регулируется отдельной парой параметров */
static uint_fast8_t glob_zoomxpow2;			/* уменьшение отображаемого участка спектра - horisontal magnification power of two */

static uint_fast8_t glob_showdbm = 1;		// Отображение уровня сигнала в dBm или S-memter (в зависимости от настроек)

static uint_fast8_t glob_smetertype = SMETER_TYPE_DIAL;	/* выбор внешнего вида прибора - стрелочный или градусник */

static int_fast16_t glob_afspeclow = 300;	// нижняя частота отображения спектроанализатора
static int_fast16_t glob_afspechigh = 3400;	// верхняя частота отображения спектроанализатора

static uint_fast8_t glob_lvlgridstep = 12;	// Шаг сетки уровней в децибелах. (0-отключаем отображение сетки уровней)

//#define WIDEFREQ (TUNE_TOP > 100000000L)

#if WITHSPECTRUMWF || WITHAFSPECTRE
static void fftzoom_af(float32_t * buffer, unsigned zoompow2, unsigned normalFFT);
#endif /* WITHSPECTRUMWF */

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

#if WITHTX

uint_fast16_t get_swr(uint_fast16_t swr_fullscale)
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
#else
uint_fast16_t get_swr(uint_fast16_t swr_fullscale)
{
	return 0;
}
#endif /* WITHTX */

#if LCDMODE_LTDC && WITHBARS

enum {
	SM_STATE_RX,
	SM_STATE_TX,
	SM_STATE_COUNT
};
enum { SM_BG_W = GRID2X(15), SM_BG_H = GRID2Y(20) };

typedef ALIGNX_BEGIN PACKEDCOLORMAIN_T smeter_bg_t [GXSIZE(SM_BG_W, SM_BG_H)] ALIGNX_END;
static smeter_bg_t smeter_bg [SMETER_TYPE_COUNT][SM_STATE_COUNT];

typedef struct {
	uint_fast16_t gs;
	uint_fast16_t gm;
	uint_fast16_t ge;
	uint_fast16_t r1;
	uint_fast16_t r2;
	uint_fast16_t rv1;
	uint_fast16_t rv2;
	uint_fast8_t step1;
	uint_fast8_t step2;
	uint_fast8_t step3;
} smeter_params_t;

static smeter_params_t smprms [SMETER_TYPE_COUNT];

static void
display2_smeter15_layout(
	uint_fast8_t xgrid,
	uint_fast8_t ygrid,
	uint_fast8_t smetertype
	)
{
	const uint_fast8_t halfsect = 30;
	const int stripewidth = 12; //16;

	smeter_params_t * const smpr = & smprms [smetertype];

	switch (smetertype)
	{
	case SMETER_TYPE_DIAL:
		{
			smpr->gm = 270;
			smpr->gs = smpr->gm - halfsect;
			smpr->ge = smpr->gm + halfsect;
			smpr->rv1 = 7 * GRID2Y(3);
			smpr->rv2 = smpr->rv1 - 3 * GRID2Y(3);
			smpr->r1 = 7 * GRID2Y(3) - 8;	//350;
			smpr->r2 = smpr->r1 - stripewidth;
			smpr->step1 = 3;	// шаг для оцифровки S
			smpr->step2 = 4;	// шаг для оцифровки плюсов
			smpr->step3 = 20;	// swr
		}
		break;

	default:
	case SMETER_TYPE_BARS:
		{
			smpr->gs = 20;
			smpr->ge = 220;
			smpr->gm = 240 / 2;
			smpr->step1 = 10;	// шаг для оцифровки S
			smpr->step2 = 14;	// шаг для оцифровки плюсов
			smpr->step3 = 67;	// swr
			smpr->r1 = SM_BG_H / 4;					// горизонталь первой шкалы
			smpr->r2 = SM_BG_H - smpr->r1; 	// горизонталь второй шкалы
		}
		break;
	}

	const int markersTX_pwr [] =
	{
		smpr->gs,
		smpr->gs + 2 * smpr->step1,
		smpr->gs + 4 * smpr->step1,
		smpr->gs + 6 * smpr->step1,
		smpr->gs + 8 * smpr->step1,
		smpr->gs + 10 * smpr->step1,
		smpr->gs + 12 * smpr->step1,
		smpr->gs + 14 * smpr->step1,
		smpr->gs + 16 * smpr->step1,
		smpr->gs + 18 * smpr->step1,
		smpr->gs + 20 * smpr->step1,
	};
	const int markersTX_swr [] =
	{
		smpr->gs,
		smpr->gs + smpr->step3,
		smpr->gs + 2 * smpr->step3,
		smpr->gs + 3 * smpr->step3,
	};
	const int markers [] =
	{
		//smpr->gs + 0 * smpr->step1,
		smpr->gs + 2 * smpr->step1,		// S1
		smpr->gs + 4 * smpr->step1,		// S3
		smpr->gs + 6 * smpr->step1,		// S5
		smpr->gs + 8 * smpr->step1,		// S7
		smpr->gs + 10 * smpr->step1,	// S9
	};
	const int markersR [] =
	{
		smpr->gm + 2 * smpr->step2,	//
		smpr->gm + 4 * smpr->step2,
		smpr->gm + 6 * smpr->step2,
	};
	const int markers2 [] =
	{
		//smpr->gs + 1 * smpr->step1,
		smpr->gs + 3 * smpr->step1,		// S2
		smpr->gs + 5 * smpr->step1,		// S4
		smpr->gs + 7 * smpr->step1,		// S6
		smpr->gs + 9 * smpr->step1,		// S8
	};
	const int markers2R [] =
	{
		smpr->gm + 1 * smpr->step2,
		smpr->gm + 3 * smpr->step2,
		smpr->gm + 5 * smpr->step2,
	};

	const COLORMAIN_T smeter = COLORMAIN_WHITE;
	const COLORMAIN_T smeterplus = COLORMAIN_DARKRED;
	const uint_fast16_t pad2w3 = strwidth3("ZZ");

	PACKEDCOLORMAIN_T * bg;
	uint_fast8_t xb = 120, yb = 120;
	unsigned p;
	unsigned i;

	switch (smetertype)
	{

	case SMETER_TYPE_DIAL:

		bg = smeter_bg [SMETER_TYPE_DIAL][SM_STATE_TX];
		colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H - 1, COLORMAIN_BLACK, 1);

		for (p = 0, i = 0; i < ARRAY_SIZE(markersTX_pwr) - 1; ++ i, p += 10)
		{
			if (i % 2 == 0)
			{
				char buf2 [10];
				uint_fast16_t xx, yy;

				display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markersTX_pwr [i], smpr->r1, smpr->r1 + 8, smeter, 1, 1);
				polar_to_dek(xb, yb, markersTX_pwr [i], smpr->r1 + 6, & xx, & yy, 1);
				local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%u"), p);
				colpip_string3_tbg(bg, SM_BG_W, SM_BG_H, xx - strwidth3(buf2) / 2, yy - pad2w3 + 1, buf2, COLORMAIN_YELLOW);
			}
			else
				display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markersTX_pwr [i], smpr->r1, smpr->r1 + 4, smeter, 1, 1);
		}

		for (p = 1, i = 0; i < ARRAY_SIZE(markersTX_swr); ++ i, p += 1)
		{
			char buf2 [10];
			uint_fast16_t xx, yy;

			display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markersTX_swr [i], smpr->r2, smpr->r2 - 8, smeter, 1, 1);
			polar_to_dek(xb, yb, markersTX_swr [i], smpr->r2 - 16, & xx, & yy, 1);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%u"), p);
			colpip_string3_tbg(bg, SM_BG_W, SM_BG_H, xx - SMALLCHARW3 / 2, yy - SMALLCHARW3 / 2 + 1, buf2, COLORMAIN_YELLOW);
		}
		display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smpr->gs, smpr->gm, smpr->r1, 1, smeter, 1, 1);
		display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smpr->gm, smpr->ge, smpr->r1, 1, smeter, 1, 1);
		display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smpr->gs, smpr->ge, smpr->r2, 1, COLORMAIN_WHITE, 1, 1);
		bg = smeter_bg [SMETER_TYPE_DIAL][SM_STATE_RX];
		colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H - 1, COLORMAIN_BLACK, 1);

		for (p = 1, i = 0; i < ARRAY_SIZE(markers); ++ i, p += 2)
		{
			char buf2 [10];
			uint_fast16_t xx, yy;

			display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markers [i], smpr->r1, smpr->r1 + 8, smeter, 1, 1);
			polar_to_dek(xb, yb, markers [i], smpr->r1 + 6, & xx, & yy, 1);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%u"), p);
			colpip_string3_tbg(bg, SM_BG_W, SM_BG_H, xx - SMALLCHARW3 / 2, yy - pad2w3 + 1, buf2, COLORMAIN_YELLOW);
		}
		for (i = 0; i < ARRAY_SIZE(markers2); ++ i)
		{
			display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markers2 [i], smpr->r1, smpr->r1 + 4, smeter, 1, 1);
		}

		for (p = 20, i = 0; i < ARRAY_SIZE(markersR); ++ i, p += 20)
		{
			char buf2 [10];
			uint_fast16_t xx, yy;

			display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markersR [i], smpr->r1, smpr->r1 + 8, smeterplus, 1, 1);
			polar_to_dek(xb, yb, markersR [i], smpr->r1 + 6, & xx, & yy, 1);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("+%u"), p);
			colpip_string3_tbg(bg, SM_BG_W, SM_BG_H, xx - strwidth3(buf2) / 2, yy - pad2w3 + 1, buf2, COLORMAIN_RED);
		}
		for (i = 0; i < ARRAY_SIZE(markers2R); ++ i)
		{
			display_radius_buf(bg, SM_BG_W, SM_BG_H, xb, yb, markers2R [i], smpr->r1, smpr->r1 + 4, smeterplus, 1, 1);
		}
		display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smpr->gs, smpr->gm, smpr->r1, 1, smeter, 1, 1);
		display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smpr->gm, smpr->ge, smpr->r1, 1, smeterplus, 1, 1);
		display_segm_buf(bg, SM_BG_W, SM_BG_H, xb, yb, smpr->gs, smpr->ge, smpr->r2, 1, COLORMAIN_WHITE, 1, 1);

		break;

	default:
	case SMETER_TYPE_BARS:

		bg = smeter_bg [SMETER_TYPE_BARS][SM_STATE_TX];
		colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H - 1, COLORMAIN_BLACK, 1);
//		colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H - 1, COLORMAIN_WHITE, 0);
//		colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H / 2, COLORMAIN_WHITE, 0);

		colmain_line(bg, SM_BG_W, SM_BG_H, smpr->gs, smpr->r1, smpr->ge, smpr->r1, COLORMAIN_WHITE, 0);
		for (p = 0, i = 0; i < ARRAY_SIZE(markersTX_pwr); ++ i, p += 10)
		{
			if (i % 2 == 0)
			{
				char buf2 [10];
				colmain_line(bg, SM_BG_W, SM_BG_H, markersTX_pwr [i], smpr->r1, markersTX_pwr [i], smpr->r1 - 10, COLORMAIN_WHITE, 0);
				local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%u"), p);
				colpip_string3_tbg(bg, SM_BG_W, SM_BG_H, markersTX_pwr [i] - strwidth3(buf2) / 2, smpr->r1 - 10 - SMALLCHARH3 - 2, buf2, COLORMAIN_YELLOW);
			}
			else
				colmain_line(bg, SM_BG_W, SM_BG_H, markersTX_pwr [i], smpr->r1, markersTX_pwr [i], smpr->r1 - 5, COLORMAIN_WHITE, 0);
		}

		colmain_line(bg, SM_BG_W, SM_BG_H, smpr->gs, smpr->r2, smpr->ge, smpr->r2, COLORMAIN_WHITE, 0);
		for (p = 1, i = 0; i < ARRAY_SIZE(markersTX_swr); ++ i, p += 1)
		{
			char buf2 [10];
			colmain_line(bg, SM_BG_W, SM_BG_H, markersTX_swr [i], smpr->r2, markersTX_swr [i], smpr->r2 + 10, COLORMAIN_WHITE, 0);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%u"), p);
			colpip_string3_tbg(bg, SM_BG_W, SM_BG_H, markersTX_swr [i] - strwidth3(buf2) / 2, smpr->r2 + 12, buf2, COLORMAIN_YELLOW);
		}

		bg = smeter_bg [SMETER_TYPE_BARS][SM_STATE_RX];
		colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H - 1, COLORMAIN_BLACK, 1);
//		colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H - 1, COLORMAIN_WHITE, 0);
//		colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H / 2, COLORMAIN_WHITE, 0);

		colmain_line(bg, SM_BG_W, SM_BG_H, smpr->gs, smpr->r1, smpr->gm, smpr->r1, COLORMAIN_WHITE, 0);
		colmain_line(bg, SM_BG_W, SM_BG_H, smpr->gm, smpr->r1, smpr->ge, smpr->r1, COLORMAIN_RED, 0);
		colpip_string2_tbg(bg, SM_BG_W, SM_BG_H, smpr->gs - SMALLCHARW2, smpr->r1 - SMALLCHARH2 - 2, "Sm", COLORMAIN_YELLOW);

		for (p = 1, i = 0; i < ARRAY_SIZE(markers); ++ i, p += 2)
		{
			char buf2 [10];
			uint_fast16_t xx, yy;
			colmain_line(bg, SM_BG_W, SM_BG_H, markers [i], smpr->r1, markers [i], smpr->r1 - 10, COLORMAIN_WHITE, 0);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%u"), p);
			colpip_string3_tbg(bg, SM_BG_W, SM_BG_H, markers [i] - SMALLCHARW3 / 2, smpr->r1 - 10 - SMALLCHARH3 - 2, buf2, COLORMAIN_YELLOW);
		}
		for (i = 0; i < ARRAY_SIZE(markers2); ++ i)
		{
			colmain_line(bg, SM_BG_W, SM_BG_H, markers2 [i], smpr->r1, markers2 [i], smpr->r1 - 5, COLORMAIN_WHITE, 0);
		}

		for (p = 20, i = 0; i < ARRAY_SIZE(markersR); ++ i, p += 20)
		{
			char buf2 [10];
			colmain_line(bg, SM_BG_W, SM_BG_H, markersR [i], smpr->r1, markersR [i], smpr->r1 - 10, COLORMAIN_RED, 0);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("+%u"), p);
			colpip_string3_tbg(bg, SM_BG_W, SM_BG_H, markersR [i] - strwidth3(buf2) / 2, smpr->r1 - 10 - SMALLCHARH3 - 2, buf2, COLORMAIN_YELLOW);
		}
		for (i = 0; i < ARRAY_SIZE(markers2R); ++ i)
		{
			colmain_line(bg, SM_BG_W, SM_BG_H, markers2R [i], smpr->r1, markers2R [i], smpr->r1 - 5, COLORMAIN_RED, 0);
		}

		break;
	}
}

static uint_fast8_t smprmsinited;

static void
display2_smeter15_init(
	uint_fast8_t xgrid,
	uint_fast8_t ygrid,
	dctx_t * pctx
	)
{
	if (smprmsinited)
			return;

	for (uint_fast8_t i = 0; i < SMETER_TYPE_COUNT; ++ i)
	{
		display2_smeter15_layout(xgrid, ygrid, i);
	}
	smprmsinited = 1;
}

// ширина занимаемого места - 15 ячеек (240/16 = 15)
static void
display2_smeter15(
	uint_fast8_t xgrid,
	uint_fast8_t ygrid,
	dctx_t * pctx
	)
{
	smeter_params_t * const smpr = & smprms [glob_smetertype];

	/* получение координат прямоугольника с изображением */
	const uint_fast16_t width = SM_BG_W;
	const uint_fast16_t height = SM_BG_H;
	const uint_fast16_t x0 = GRID2X(xgrid);
	const uint_fast16_t y0 = GRID2Y(ygrid);
	const int dial_shift = GRID2Y(2);
	const int xc = x0 + width / 2;
	const int yc = y0 + 120 + dial_shift;

	const uint_fast8_t is_tx = hamradio_get_tx();
	PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
	static uint_fast8_t first_tx = 0;

	int gp = smpr->gs, gv = smpr->gs, gv_trace = smpr->gs, gswr = smpr->gs;

	//colpip_rect(colmain_fb_draw(), DIM_X, DIM_Y, x0, y0, x0 + width - 1, y0 + height - 1, COLORMAIN_GREEN, 1);

	if (is_tx)
	{
		enum { gx_hyst = 3 };		// гистерезис в градусах
		/* фильтрация - (в градусах) */
		static uint_fast16_t gp_smooth, gswr_smooth;

		if (first_tx)				// сброс при переходе на передачу
		{
			first_tx = 0;
			gp_smooth = smpr->gs;
			gswr_smooth = smpr->gs;
		}

		const adcvalholder_t power = board_getadc_unfiltered_truevalue(PWRMRRIX);	// без возможных тормозов на SPI при чтении
		gp = smpr->gs + normalize(power, 0, maxpwrcali * 16, smpr->ge - smpr->gs);

		// todo: get_swr(swr_fullscale) - использщовать MRRxxx.
		// Для тюнера и измерений не годится, для показа - без торомозов.
		const uint_fast16_t swr_fullscale = (SWRMIN * 40 / 10) - SWRMIN;	// количество рисок в шкале ииндикатора
		gswr = smpr->gs + normalize(get_swr(swr_fullscale), 0, swr_fullscale, smpr->ge - smpr->gs);

		if (gp > smpr->gs)
			gp_smooth = gp;

		if (gp == smpr->gs && gp_smooth > smpr->gs)
			gp = (gp_smooth -= gx_hyst) > smpr->gs ? gp_smooth : smpr->gs;

		if (gswr > smpr->gs)
			gswr_smooth = gswr;

		if (gswr == smpr->gs && gswr_smooth > smpr->gs)
			gswr = (gswr_smooth -= gx_hyst) > smpr->gs ? gswr_smooth : smpr->gs;
	}
	else
	{
		uint_fast8_t tracemax;
		uint_fast8_t value = board_getsmeter(& tracemax, 0, UINT8_MAX, 0);
		tracemax = value > tracemax ? value : tracemax;	// защита от рассогласования значений

		gv =
			smpr->gs + normalize3(value, 	s9level - s9delta, s9level, s9level + s9_60_delta, smpr->gm - smpr->gs, smpr->ge - smpr->gs);
		gv_trace =
			smpr->gs + normalize3(tracemax, s9level - s9delta, s9level, s9level + s9_60_delta, smpr->gm - smpr->gs, smpr->ge - smpr->gs);

		first_tx = 1;
	}

	switch (glob_smetertype)
	{

	case SMETER_TYPE_DIAL:

		if (is_tx)
		{
			// TX state
			colpip_plot(
					(uintptr_t) fr, GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORMAIN_T),
					fr, DIM_X, DIM_Y, x0, y0 + dial_shift,
					(uintptr_t) smeter_bg [SMETER_TYPE_DIAL][SM_STATE_TX], GXSIZE(SM_BG_W, SM_BG_H) * sizeof (PACKEDCOLORMAIN_T),
					smeter_bg [SMETER_TYPE_DIAL][SM_STATE_TX], SM_BG_W, SM_BG_H - dial_shift);

			if (gswr > smpr->gs)
			{
				uint_fast16_t xx, yy;
				const COLORMAIN_T color = COLORMAIN_YELLOW;

				display_segm_buf(fr, DIM_X, DIM_Y, xc, yc, smpr->gs, gswr, smpr->r2 + 2, 1, color, 0, 1);
				display_segm_buf(fr, DIM_X, DIM_Y, xc, yc, smpr->gs, gswr, smpr->r1 - 2, 1, color, 0, 1);
				display_radius_buf(fr, DIM_X, DIM_Y, xc, yc, smpr->gs, smpr->r1 - 2, smpr->r2 + 2, color, 0, 1);
				display_radius_buf(fr, DIM_X, DIM_Y, xc, yc, gswr, smpr->r1 - 2, smpr->r2 + 2, color, 0, 1);
				polar_to_dek(xc, yc, gswr - 1, smpr->r1 - 4, & xx, & yy, 1);
				display_floodfill(fr, DIM_X, DIM_Y, xx, yy, color, COLORMAIN_BLACK, 1);
			}

			const COLORMAIN_T color = COLORMAIN_GREEN;
			display_radius_buf(fr, DIM_X, DIM_Y, xc - 1, yc, gp, smpr->rv1, smpr->rv2, color, 0, 1);
			display_radius_buf(fr, DIM_X, DIM_Y, xc, yc, gp, smpr->rv1, smpr->rv2, color, 0, 1);
			display_radius_buf(fr, DIM_X, DIM_Y, xc + 1, yc, gp, smpr->rv1, smpr->rv2, color, 0, 1);
		}
		else
		{
			// RX state
			colpip_plot(
					(uintptr_t) fr, GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORMAIN_T),
					fr, DIM_X, DIM_Y, x0, y0 + dial_shift,
					(uintptr_t) smeter_bg [SMETER_TYPE_DIAL][SM_STATE_RX], GXSIZE(SM_BG_W, SM_BG_H) * sizeof (PACKEDCOLORMAIN_T),
					smeter_bg [SMETER_TYPE_DIAL][SM_STATE_RX], SM_BG_W, SM_BG_H - dial_shift);

			{
				// Рисование peak value (риска)
				const COLORMAIN_T color = COLORMAIN_YELLOW;
				display_radius_buf(fr, DIM_X, DIM_Y, xc - 1, yc, gv_trace, smpr->r1 - 2, smpr->r2 + 2, color, 0, 1);
				display_radius_buf(fr, DIM_X, DIM_Y, xc, yc, gv_trace, smpr->r1 - 2, smpr->r2 + 2, color, 0, 1);
				display_radius_buf(fr, DIM_X, DIM_Y, xc + 1, yc, gv_trace, smpr->r1 - 2, smpr->r2 + 2, color, 0, 1);
			}

			{
				// Рисование стрелки
				const COLORMAIN_T color = COLORMAIN_GREEN;
				display_radius_buf(fr, DIM_X, DIM_Y, xc - 1, yc, gv, smpr->rv1, smpr->rv2, color, 0, 1);
				display_radius_buf(fr, DIM_X, DIM_Y, xc, yc, gv, smpr->rv1, smpr->rv2, color, 0, 1);
				display_radius_buf(fr, DIM_X, DIM_Y, xc + 1, yc, gv, smpr->rv1, smpr->rv2, color, 0, 1);
			}
		}

		break;

	case SMETER_TYPE_BARS:

		if (is_tx)
		{
			colpip_plot(
					(uintptr_t) fr, GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORMAIN_T),
					fr, DIM_X, DIM_Y, x0, y0,
					(uintptr_t) smeter_bg [SMETER_TYPE_BARS][SM_STATE_TX], GXSIZE(SM_BG_W, SM_BG_H) * sizeof (PACKEDCOLORMAIN_T),
					smeter_bg [SMETER_TYPE_BARS][SM_STATE_TX], SM_BG_W, SM_BG_H);

			if(gp > smpr->gs)
				colpip_rect(fr, DIM_X, DIM_Y, x0 + smpr->gs, y0 + smpr->r1 + 5, x0 + gp, y0 + smpr->r1 + 20, COLORMAIN_GREEN, 1);

			if(gswr > smpr->gs)
				colpip_rect(fr, DIM_X, DIM_Y, x0 + smpr->gs, y0 + smpr->r2 - 20, x0 + gswr, y0 + smpr->r2 - 5, COLORMAIN_GREEN, 1);
		}
		else
		{
			colpip_plot(
					(uintptr_t) fr, GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORMAIN_T),
					fr, DIM_X, DIM_Y, x0, y0,
					(uintptr_t) smeter_bg [SMETER_TYPE_BARS][SM_STATE_RX], GXSIZE(SM_BG_W, SM_BG_H) * sizeof (PACKEDCOLORMAIN_T),
					smeter_bg [SMETER_TYPE_BARS][SM_STATE_RX], SM_BG_W, SM_BG_H
					);

			if(gv > smpr->gs)
				colpip_rect(fr, DIM_X, DIM_Y, x0 + smpr->gs, y0 + smpr->r1 + 5, x0 + gv, y0 + smpr->r1 + 20, COLORMAIN_GREEN, 1);

			if(gv_trace > smpr->gs)
				colmain_line(fr, DIM_X, DIM_Y, x0 + gv_trace, y0 + smpr->r1 + 5, x0 + gv_trace, y0 + smpr->r1 + 20, COLORMAIN_YELLOW, 0);
		}

		break;
	}
}

#endif /* LCDMODE_LTDC */

// очистка фона
static void
display2_clearbg(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if LCDMODE_LTDC && ! (LCDMODE_PIP_RGB565 || LCDMODE_PIP_L8)

	colmain_fillrect(colmain_fb_draw(), DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, display_getbgcolor());

#endif /* LCDMODE_LTDC && ! (LCDMODE_PIP_RGB565 || LCDMODE_PIP_L8) */
}

// Завершение отрисовки, переключение на следующий фреймбуфер
static void
display2_nextfb(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHLTDCHW && LCDMODE_LTDC && ! (LCDMODE_PIP_RGB565 || LCDMODE_PIP_L8)

	display_nextfb();
#endif /* WITHLTDCHW && LCDMODE_LTDC && ! (LCDMODE_PIP_RGB565 || LCDMODE_PIP_L8) */
}

// Отображение частоты. Герцы так же большим шрифтом.
static void display_freqXbig_a(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const uint_fast8_t comma = 3 - rj;

	colmain_setcolors3(colors_1freq [0].fg, colors_1freq [0].bg, colors_1freq [0].fg);
	if (pctx != NULL && pctx->type == DCTX_FREQ)
	{
#if WITHDIRECTFREQENER
		const editfreq2_t * const efp = (const editfreq2_t *) pctx->pv;


		uint_fast8_t lowhalf = HALFCOUNT_FREQA - 1;
		do
		{
			display_value_big(x, y + lowhalf, efp->freq, fullwidth, comma, comma + 3, rj, efp->blinkpos + 1, efp->blinkstate, 0, lowhalf);	// отрисовываем верхнюю часть строки
		} while (lowhalf --);
#endif /* WITHDIRECTFREQENER */
	}
	else
	{
		enum { blinkpos = 255, blinkstate = 0 };

		const uint_fast32_t freq = hamradio_get_freq_a();

		uint_fast8_t lowhalf = HALFCOUNT_FREQA - 1;
		do
		{
			display_value_big(x, y + lowhalf, freq, fullwidth, comma, comma + 3, rj, blinkpos, blinkstate, 0, lowhalf);	// отрисовываем верхнюю часть строки
		} while (lowhalf --);
	}
}

// Отображение частоты. Герцы маленьким шрифтом.
static void display2_freqX_a(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const uint_fast8_t comma = 3 - rj;

	colmain_setcolors3(colors_1freq [0].fg, colors_1freq [0].bg, colors_1freq [0].fg);
	if (pctx != NULL && pctx->type == DCTX_FREQ)
	{
#if WITHDIRECTFREQENER
		const editfreq2_t * const efp = (const editfreq2_t *) pctx->pv;
		uint_fast8_t lowhalf = HALFCOUNT_FREQA - 1;
		do
		{
			display_value_big(x, y + lowhalf, efp->freq, fullwidth, comma, comma + 3, rj, efp->blinkpos + 1, efp->blinkstate, 1, lowhalf);	// отрисовываем верхнюю часть строки
		} while (lowhalf --);
#endif /* WITHDIRECTFREQENER */
	}
	else
	{
		enum { blinkpos = 255, blinkstate = 0 };

		const uint_fast32_t freq = hamradio_get_freq_a();

		uint_fast8_t lowhalf = HALFCOUNT_FREQA - 1;
		do
		{
			display_value_big(x, y + lowhalf, freq, fullwidth, comma, comma + 3, rj, blinkpos, blinkstate, 1, lowhalf);	// отрисовываем верхнюю часть строки
		} while (lowhalf --);
	}
}

static void display2_freqx_a(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const uint_fast8_t comma = 3 - rj;
	const uint_fast32_t freq = hamradio_get_freq_a();

	colmain_setcolors3(colors_1freq [0].fg, colors_1freq [0].bg, colors_1freq [0].fg);
	display_value_lower(x, y, freq, fullwidth, comma, rj);
}

// Верстия отображения без точки между мегагерцами и сотнями килогерц (для текстовых дисплееев)
// FREQ B
static void display_freqchr_a(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const uint_fast8_t comma = 3 - rj;

	colmain_setcolors3(colors_1freq [0].fg, colors_1freq [0].bg, colors_1freq [0].fg);
	if (pctx != NULL && pctx->type == DCTX_FREQ)
	{
#if WITHDIRECTFREQENER
		const editfreq2_t * const efp = (const editfreq2_t *) pctx->pv;

		uint_fast8_t lowhalf = HALFCOUNT_FREQA - 1;
		do
		{
			display_value_big(x, y + lowhalf, efp->freq, fullwidth, comma, 255, rj, efp->blinkpos + 1, efp->blinkstate, 1, lowhalf);	// отрисовываем верхнюю часть строки
		} while (lowhalf --);
#endif /* WITHDIRECTFREQENER */
	}
	else
	{
		enum { blinkpos = 255, blinkstate = 0 };

		const uint_fast32_t freq = hamradio_get_freq_a();

		uint_fast8_t lowhalf = HALFCOUNT_FREQA - 1;
		do
		{
			display_value_big(x, y + lowhalf, freq, fullwidth, comma, 255, rj, blinkpos, blinkstate, 1, lowhalf);	// отрисовываем верхнюю часть строки
		} while (lowhalf --);
	}
}

// Верстия отображения без точки между мегагерцами и сотнями килогерц (для текстовых дисплееев)
// FREQ B
static void display_freqchr_b(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const uint_fast8_t comma = 3 - rj;
	uint_fast8_t state;
	hamradio_get_vfomode3_value(& state);	// state - признак активного SPLIT (0/1)

	colmain_setcolors3(colors_2freqB [state].fg, colors_2freqB [state].bg, colors_2freqB [state].fg);
	if (pctx != NULL && pctx->type == DCTX_FREQ)
	{
#if WITHDIRECTFREQENER
		const editfreq2_t * const efp = (const editfreq2_t *) pctx->pv;

		uint_fast8_t lowhalf = HALFCOUNT_FREQA - 1;
		do
		{
			display_value_big(x, y + lowhalf, efp->freq, fullwidth, comma, 255, rj, efp->blinkpos + 1, efp->blinkstate, 1, lowhalf);	// отрисовываем верхнюю часть строки
		} while (lowhalf --);
#endif /* WITHDIRECTFREQENER */
	}
	else
	{
		enum { blinkpos = 255, blinkstate = 0 };

		const uint_fast32_t freq = hamradio_get_freq_b();

		uint_fast8_t lowhalf = HALFCOUNT_FREQA - 1;
		do
		{
			display_value_big(x, y + lowhalf, freq, fullwidth, comma, 255, 1, blinkpos, blinkstate, 1, lowhalf);	// отрисовываем верхнюю часть строки
		} while (lowhalf --);
	}
}

static void display2_freqX_b(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const uint_fast8_t comma = 3 - rj;
	uint_fast8_t state;
	hamradio_get_vfomode3_value(& state);	// state - признак активного SPLIT (0/1)

	const uint_fast32_t freq = hamradio_get_freq_b();

	colmain_setcolors(colors_2freqB [state].fg, colors_2freqB [state].bg);
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_value_small(x, y + lowhalf, freq, fullwidth, comma, comma + 3, rj, lowhalf);
	} while (lowhalf --);
}

// отладочная функция измерителя опорной частоты
static void display_freqmeter10(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHFQMETER
	char buf2 [11];

	local_snprintf_P(
		buf2, ARRAY_SIZE(buf2),
		PSTR("%010lu"),
		(unsigned long) board_get_fqmeter()
		);

	colmain_setcolors(colors_1freq [0].fg, colors_1freq [0].bg);
	display_at(x, y, buf2);
#endif /* WITHFQMETER */
}

// отображение текста (из FLASH) с атрибутами по состоянию
static void 
NOINLINEAT
display2_text_P(
	uint_fast8_t x, 
	uint_fast8_t y, 
	const FLASHMEM char * const * labels,	// массив указателей на текст
	const COLORPAIR_T * colors,			// массив цветов
	uint_fast8_t state
	)
{
	#if LCDMODE_COLORED
	#else /* LCDMODE_COLORED */
	#endif /* LCDMODE_COLORED */
#if WITHALTERNATIVELAYOUT
	layout_label1_medium(x, y, labels [state], strlen_P(labels [state]), 5, COLORMAIN_BLACK, colors_2state_alt [state]);
#else
	colmain_setcolors(colors [state].fg, colors [state].bg);
	display_at_P(x, y, labels [state]);
#endif /* WITHALTERNATIVELAYOUT */
}

// отображение текста (из FLASH) с атрибутами по состоянию
static void
NOINLINEAT
display2_text_alt_P(
	uint_fast8_t x,
	uint_fast8_t y,
	const FLASHMEM char * const * labels,	// массив указателей на текст
	const COLORPAIR_T * colors,			// массив цветов
	uint_fast8_t state
	)
{
	layout_label1_medium(x, y, labels [state], strlen_P(labels [state]), 5, COLORMAIN_BLACK, colors_2state_alt [state]);
}

// отображение текста с атрибутами по состоянию
static void 
NOINLINEAT
display2_text(
	uint_fast8_t x, 
	uint_fast8_t y, 
	const char * const * labels,	// массив указателей на текст
	const COLORPAIR_T * colors,			// массив цветов
	uint_fast8_t state
	)
{
	#if LCDMODE_COLORED
	#else /* LCDMODE_COLORED */
	#endif /* LCDMODE_COLORED */

	colmain_setcolors(colors [state].fg, colors [state].bg);
	display_at(x, y, labels [state]);
}

// Отображение режимов TX / RX
static void display_txrxstatecompact(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHTX
	const uint_fast8_t tx = hamradio_get_tx();
	colmain_setcolors(TXRXMODECOLOR, tx ? MODECOLORBG_TX : MODECOLORBG_RX);
	display_at_P(x, y, tx ? PSTR("T") : PSTR(" "));
#endif /* WITHTX */
}

// Отображение режимов TX / RX
static void display_txrxstate2(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHTX
	const uint_fast8_t state = hamradio_get_tx();

	static const FLASHMEM char text0 [] = "RX";
	static const FLASHMEM char text1 [] = "TX";
	const FLASHMEM char * const labels [2] = { text0, text1 };
	display2_text_P(x, y, labels, colors_2rxtx, state);
#endif /* WITHTX */
}

// Отображение режимов TX / RX
static void display_txrxstate5alt(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHTX
	const uint_fast8_t state = hamradio_get_tx();

	static const FLASHMEM char text0 [] = "RX";
	static const FLASHMEM char text1 [] = "TX";
	const FLASHMEM char * const labels [2] = { text0, text1 };
	layout_label1_medium(x, y, labels [state], 2, 5, state ? COLORMAIN_WHITE : COLORMAIN_BLACK, state ? COLORMAIN_RED : COLORMAIN_GRAY);
#endif /* WITHTX */
}

static void display_recstatus(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHUSEAUDIOREC
	unsigned long hamradio_get_recdropped(void);
	int hamradio_get_recdbuffered(void);

	char buf2 [12];
	local_snprintf_P(
		buf2,
		ARRAY_SIZE(buf2),
		PSTR("%08lx %2d"), 
		(unsigned long) hamradio_get_recdropped(),
		(int) hamradio_get_recdbuffered()
		);
		
	colmain_setcolors(LABELTEXT, LABELBACK);
	display_at(x, y, buf2);

#endif /* WITHUSEAUDIOREC */
}

// Отображение режима записи аудио фрагмента
static void display2_rec3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHUSEAUDIOREC

	const uint_fast8_t state = hamradio_get_rec_value();

	static const FLASHMEM char text_pau [] = "PAU";
	static const FLASHMEM char text_rec [] = "REC";
	const FLASHMEM char * const labels [2] = { text_pau, text_rec };
	display2_text_P(x, y, labels, colors_2state, state);

#endif /* WITHUSEAUDIOREC */
}

// отображение состояния USB HOST
static void display2_usbsts3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI)
	const uint_fast8_t active = hamradio_get_usbh_active();
	#if LCDMODE_COLORED
		static const FLASHMEM char text_usb [] = "USB";
		display_2states_P(x, y, active, text_usb, text_usb);
	#else /* LCDMODE_COLORED */
		display_at_P(x, y, active ? PSTR("USB") : PSTR("   "));
	#endif /* LCDMODE_COLORED */
#endif /* defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) */
}

void display_2states(
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t state,
	const char * state1,	// активное
	const char * state0
	)
{
	#if LCDMODE_COLORED
		const char * const labels [2] = { state1, state1, };
	#else /* LCDMODE_COLORED */
		const char * const labels [2] = { state0, state1, };
	#endif /* LCDMODE_COLORED */
	display2_text(x, y, labels, colors_2state, state);
}

void display_2states_P(
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t state,
	const FLASHMEM char * state1,	// активное
	const FLASHMEM char * state0
	)
{
	#if LCDMODE_COLORED
		const FLASHMEM char * const labels [2] = { state1, state1, };
	#else /* LCDMODE_COLORED */
		const FLASHMEM char * const labels [2] = { state0, state1, };
	#endif /* LCDMODE_COLORED */
	display2_text_P(x, y, labels, colors_2state, state);
}

// Параметры, не меняющие состояния цветом
void display_1state_P(
	uint_fast8_t x, 
	uint_fast8_t y, 
	const FLASHMEM char * label
	)
{
	display2_text_P(x, y, & label, colors_1state, 0);
}


void display_2fmenus(
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t state,
	const char * state1,	// активное
	const char * state0
	)
{
	#if LCDMODE_COLORED
		const char * const labels [2] = { state1, state1, };
	#else /* LCDMODE_COLORED */
		const char * const labels [2] = { state0, state1, };
	#endif /* LCDMODE_COLORED */
	display2_text(x, y, labels, colors_2fmenu, state);
}

void display_2fmenus_P(
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t state,
	const FLASHMEM char * state1,	// активное
	const FLASHMEM char * state0
	)
{
	#if LCDMODE_COLORED
		const FLASHMEM char * const labels [2] = { state1, state1, };
	#else /* LCDMODE_COLORED */
		const FLASHMEM char * const labels [2] = { state0, state1, };
	#endif /* LCDMODE_COLORED */
	display2_text_P(x, y, labels, colors_2fmenu, state);
}

// Параметры, не меняющие состояния цветом
void display_1fmenu(
	uint_fast8_t x, 
	uint_fast8_t y, 
	const char * label
	)
{
	display2_text(x, y, & label, colors_1fmenu, 0);
}

// Параметры, не меняющие состояния цветом
void display_1fmenu_P(
	uint_fast8_t x,
	uint_fast8_t y,
	const FLASHMEM char * label
	)
{
	display2_text_P(x, y, & label, colors_1fmenu, 0);
}

static const FLASHMEM char text_nul1_P [] = " ";
static const FLASHMEM char text_nul2_P [] = "  ";
static const FLASHMEM char text_nul3_P [] = "   ";
static const FLASHMEM char text_nul4_P [] = "    ";
static const FLASHMEM char text_nul5_P [] = "     ";
//static const FLASHMEM char text_nul9_P [] = "         ";
static const char text_nul3 [] = "   ";
static const char text_nul5 [] = "     ";

// Отображение режима NR ON/OFF
static void display2_nr3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHIF4DSP
	int_fast32_t grade;
	const uint_fast8_t state = hamradio_get_nrvalue(& grade);
	display_2states_P(x, y, state, PSTR("NR "), text_nul3_P);
#endif /* WITHIF4DSP */
}

/* Отображение включенного режима CW BREAK-IN */
static void display2_bkin3(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHELKEY
	const uint_fast8_t state = hamradio_get_bkin_value();
	display_2states_P(x, y, state, PSTR("BKN"), text_nul3_P);
	(void) pctx;
#endif /* WITHELKEY */
}

/* Отображение включенного динамика */
static void display2_spk3(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHSPKMUTE
	static const FLASHMEM char text_spk [] = "SPK";
	const uint_fast8_t state = hamradio_get_spkon_value();	// не-0: динамик включен
	display_2states_P(x, y, state, text_spk, text_spk);
	(void) pctx;
#endif /* WITHSPKMUTE */
}

static void display2_wpm5(
		uint_fast8_t x,
		uint_fast8_t y,
		dctx_t * pctx
		)
{
#if WITHELKEY
	const uint_fast8_t value = hamradio_get_cw_wpm();	// не-0: динамик включен
	char buf2 [6];
	const char * const labels [1] = { buf2, };

	local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%2dwpm"), (int) value);
	display2_text(x, y, labels, colors_1state, 0);
	(void) pctx;
#endif /* WITHELKEY */
}

// Отображение типа режима NOCH и ON/OFF
static void display2_notch5(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHNOTCHONOFF || WITHNOTCHFREQ
	int_fast32_t freq;
	const uint_fast8_t state = hamradio_get_notchvalue(& freq);
	const char FLASHMEM * const label = hamradio_get_notchtype5_P();
	const char FLASHMEM * const labels [2] = { label, label, };
	display2_text_P(x, y, labels, colors_2state, state);
#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
}

// Отображение типа режима NOCH и ON/OFF
static void display2_notch7alt(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHNOTCHONOFF || WITHNOTCHFREQ
	int_fast32_t freq;
	const uint_fast8_t state = hamradio_get_notchvalue(& freq);
	const char FLASHMEM * const label = hamradio_get_notchtype5_P();
	const char FLASHMEM * const labels [2] = { label, label, };
	layout_label1_medium(x, y, label, strlen_P(label), 7, COLORMAIN_BLACK, colors_2state_alt [state]);
#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
}

// Отображение частоты NOCH
static void display2_notchfreq5(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHNOTCHONOFF || WITHNOTCHFREQ
	int_fast32_t freq;
	const uint_fast8_t state = hamradio_get_notchvalue(& freq);
	char buf2 [6];
	local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%5lu"), freq);
	display_2states(x, y, state, buf2, text_nul5);
#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
}

// Отображение режима NOCH ON/OFF
static void display_notch3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHNOTCHONOFF || WITHNOTCHFREQ
	int_fast32_t freq;
	const uint_fast8_t state = hamradio_get_notchvalue(& freq);
	static const FLASHMEM char text_nch [] = "NCH";
	display_2states_P(x, y, state, PSTR("NCH"), text_nul3_P);
#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
}


// VFO mode
static void display2_vfomode3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	uint_fast8_t state;	// state - признак активного SPLIT (0/1)
	const char * const labels [1] = { hamradio_get_vfomode3_value(& state), };
	display2_text(x, y, labels, colors_1state, 0);
}

// VFO mode
static void display2_vfomode5alt(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
	uint_fast8_t state;	// state - признак активного SPLIT (0/1)
	const char * const labels [1] = { hamradio_get_vfomode3_value(& state), };
	layout_label1_medium(x, y, labels [0], strlen_P(labels [0]), 5, COLORMAIN_BLACK, colors_2state_alt [state]);
}

// VFO mode with memory ch info
static void display_vfomode5(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	uint_fast8_t state;	// state - признак активного SPLIT (0/1)
	const char * const labels [1] = { hamradio_get_vfomode5_value(& state), };
	display2_text(x, y, labels, colors_1state, 0);
}

static void display_XXXXX3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHPLACEHOLDERS
	const uint_fast8_t state = 0;
	display_2states_P(x, y, state, text_nul3_P, text_nul3_P);
#endif /* WITHPLACEHOLDERS */
}

static void display_XXXXX5(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHPLACEHOLDERS
	const uint_fast8_t state = 0;
	display_2states_P(x, y, state, text_nul5_P, text_nul5_P);
#endif /* WITHPLACEHOLDERS */
}

// Отображение режима передачи аудио с USB
static void display_datamode4(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHTX
	#if WITHIF4DSP && WITHUSBUAC && WITHDATAMODE
		const uint_fast8_t state = hamradio_get_datamode();
		display_2states_P(x, y, state, PSTR("DATA"), text_nul4_P);
	#endif /* WITHIF4DSP && WITHUSBUAC && WITHDATAMODE */
#endif /* WITHTX */
}

// Отображение режима передачи аудио с USB
static void display2_datamode3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHTX
	#if WITHIF4DSP && WITHUSBUAC && WITHDATAMODE
		const uint_fast8_t state = hamradio_get_datamode();
		display_2states_P(x, y, state, PSTR("DAT"), text_nul3_P);
	#endif /* WITHIF4DSP && WITHUSBUAC && WITHDATAMODE */
#endif /* WITHTX */
}

// Отображение режима автонастройки
static void display2_atu3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHTX
	#if WITHAUTOTUNER
		const uint_fast8_t state = hamradio_get_atuvalue();
		display_2states_P(x, y, state, PSTR("ATU"), text_nul3_P);
	#endif /* WITHAUTOTUNER */
#endif /* WITHTX */
}

// Отображение режима автонастройки
static void display2_atu4alt(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHTX
	#if WITHAUTOTUNER
		const uint_fast8_t state = hamradio_get_atuvalue();
		display_2states_P(x, y, state, PSTR("ATU"), text_nul3_P);
	#endif /* WITHAUTOTUNER */
#endif /* WITHTX */
}


// Отображение режима General Coverage / HAM bands
static void display2_genham1(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHBCBANDS

	const uint_fast8_t state = hamradio_get_genham_value();

	display_2states_P(x, y, state, PSTR("G"), PSTR("H"));

#endif /* WITHBCBANDS */
}

// Отображение режима обхода тюнера
static void display2_byp3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHTX
	#if WITHAUTOTUNER
		const uint_fast8_t state = hamradio_get_bypvalue();
		display_2states_P(x, y, state, PSTR("BYP"), text_nul3_P);
	#endif /* WITHAUTOTUNER */
#endif /* WITHTX */
}

// Отображение режима обхода тюнера
static void display2_byp4alt(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHTX
	#if WITHAUTOTUNER
		const uint_fast8_t state = hamradio_get_bypvalue();
		display_2states_P(x, y, state, PSTR("BYP"), text_nul3_P);
	#endif /* WITHAUTOTUNER */
#endif /* WITHTX */
}

// Отображение режима VOX
static void display_vox3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHTX
	#if WITHVOX
		const uint_fast8_t state = hamradio_get_voxvalue();
		display_2states_P(x, y, state, PSTR("VOX"), text_nul3_P);
	#endif /* WITHVOX */
#endif /* WITHTX */
}

// Отображение режимов VOX и TUNE
// Если VOX не предусмотрен, только TUNE
static void display2_voxtune3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHTX

	static const FLASHMEM char text_vox [] = "VOX";
	static const FLASHMEM char text_tun [] = "TUN";
	static const FLASHMEM char text_nul [] = "   ";

#if WITHVOX

	const uint_fast8_t tunev = hamradio_get_tunemodevalue();
	const uint_fast8_t voxv = hamradio_get_voxvalue();

	#if LCDMODE_COLORED
		const FLASHMEM char * const labels [4] = { text_vox, text_vox, text_tun, text_tun, };
	#else /* LCDMODE_COLORED */
		const FLASHMEM char * const labels [4] = { text_nul, text_vox, text_tun, text_tun, };
	#endif /* LCDMODE_COLORED */

	display2_text_P(x, y, labels, colors_4state, tunev * 2 + voxv);

#else /* WITHVOX */

	const uint_fast8_t state = hamradio_get_tunemodevalue();

	display_2states_P(x, y, state, PSTR("TUN"), text_nul3_P);

#endif /* WITHVOX */
#endif /* WITHTX */
}

// Отображение режимов VOX и TUNE
// Длинный текст
// Если VOX не предусмотрен, только TUNE
static void display_voxtune4(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHTX
#if WITHVOX

	const uint_fast8_t tunev = hamradio_get_tunemodevalue();
	const uint_fast8_t voxv = hamradio_get_voxvalue();
	static const FLASHMEM char text0 [] = "VOX ";
	static const FLASHMEM char text1 [] = "TUNE";
	const FLASHMEM char * const labels [4] = { text0, text0, text1, text1, };
	display2_text_P(x, y, labels, colors_4state, tunev * 2 + voxv);

#else /* WITHVOX */

	const uint_fast8_t state = hamradio_get_tunemodevalue();
		display_2states_P(x, y, state, PSTR("TUNE"), text_nul4_P);

#endif /* WITHVOX */
#endif /* WITHTX */
}

// Отображение режимов VOX и TUNE
// Однобуквенные обозначения
// Если VOX не предусмотрен, только TUNE
static void display_voxtune1(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHTX
#if WITHVOX

	const uint_fast8_t tunev = hamradio_get_tunemodevalue();
	const uint_fast8_t voxv = hamradio_get_voxvalue();
	static const FLASHMEM char textx [] = " ";
	static const FLASHMEM char text0 [] = "V";
	static const FLASHMEM char text1 [] = "U";
	const FLASHMEM char * const labels [4] = { textx, text0, text1, text1, };
	display2_text_P(x, y, labels, colors_4state, tunev * 2 + voxv);

#else /* WITHVOX */

	const uint_fast8_t state = hamradio_get_tunemodevalue();
	display_2states_P(x, y, state, PSTR("U"), text_nul1_P);

#endif /* WITHVOX */
#endif /* WITHTX */
}


static void display_lockstate3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	const uint_fast8_t lockv = hamradio_get_lockvalue();
	const uint_fast8_t fastv = hamradio_get_usefastvalue();

	static const FLASHMEM char text0 [] = "    ";
	static const FLASHMEM char text1 [] = "LCK";
	static const FLASHMEM char text2 [] = "FST";
#if LCDMODE_COLORED
	const FLASHMEM char * const labels [4] = { text1, text2, text1, text1, };
#else /* LCDMODE_COLORED */
	const FLASHMEM char * const labels [4] = { text0, text2, text1, text1, };
#endif
	display2_text_P(x, y, labels, colors_4state, lockv * 2 + fastv);
}

static void display2_lockstate4(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	const uint_fast8_t lockv = hamradio_get_lockvalue();
	const uint_fast8_t fastv = hamradio_get_usefastvalue();

	static const FLASHMEM char text0 [] = "    ";
	static const FLASHMEM char text1 [] = "LOCK";
	static const FLASHMEM char text2 [] = "FAST";
#if LCDMODE_COLORED
	const FLASHMEM char * const labels [4] = { text1, text2, text1, text1, };
#else /* LCDMODE_COLORED */
	const FLASHMEM char * const labels [4] = { text0, text2, text1, text1, };
#endif
	display2_text_P(x, y, labels, colors_4state, lockv * 2 + fastv);
}

static void display2_lockstate5alt(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
	enum { chars_W2 = 5 };
	const uint_fast8_t lockv = hamradio_get_lockvalue();
	const uint_fast8_t fastv = hamradio_get_usefastvalue();

	static const FLASHMEM char text0 [] = "    ";
	static const FLASHMEM char text1 [] = "LOCK";
	static const FLASHMEM char text2 [] = "FAST";

	layout_label1_medium(x, y, fastv ? text2 : text1, 4, chars_W2, COLORMAIN_BLACK, colors_2state_alt [lockv || fastv]);
}


static void display_lockstate1(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	colmain_setcolors(LOCKCOLOR, BGCOLOR);
	display_at_P(x, y, hamradio_get_lockvalue() ? PSTR("*") : PSTR(" "));
}

// Отображение PBT
static void display_pbt(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHPBT
	const int_fast32_t pbt = hamradio_get_pbtvalue();
	display_at_P(x, y, PSTR("PBT "), lowhalf);

	//colmain_setcolors(LOCKCOLOR, BGCOLOR);
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display2_menu_value(x + 4, y + lowhalf, pbt, 4 | WSIGNFLAG, 2, 1, lowhalf);
	} while (lowhalf --);
#endif /* WITHPBT */
}

// RX path bandwidth
static void display2_rxbw3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	const char FLASHMEM * const labels [1] = { hamradio_get_rxbw_value_P(), };
	ASSERT(strlen(labels [0]) == 3);
	display2_text_P(x, y, labels, colors_1state, 0);
}


// текущее состояние DUAL WATCH
static void display2_mainsub3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHUSEDUALWATCH
	uint_fast8_t state;	// state - признак активного SPLIT (0/1)
	hamradio_get_vfomode5_value(& state);
	const char FLASHMEM * const label = hamradio_get_mainsubrxmode3_value_P();
	ASSERT(strlen(label) == 3);
	display_2states_P(x, y, state, label, label);
#endif /* WITHUSEDUALWATCH */
}


// RX preamplifier
static void display_pre3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	const char FLASHMEM * const labels [1] = { hamradio_get_pre_value_P(), };
	ASSERT(strlen(labels [0]) == 3);
	display2_text_P(x, y, labels, colors_1state, 0);
}

// переполнение АЦП (надо показывать как REDRM_BARS - с таймерным обновлением)
static void display_ovf3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHDSPEXTDDC
	//const char FLASHMEM * const labels [1] = { hamradio_get_pre_value_P(), };
	//display2_text_P(x, y, labels, colors_1state, 0);

	if (boad_fpga_adcoverflow() != 0)
	{
		colmain_setcolors(BGCOLOR, OVFCOLOR);
		display_at_P(x, y, PSTR("OVF"));
	}
	else if (boad_mike_adcoverflow() != 0)
	{
		colmain_setcolors(BGCOLOR, OVFCOLOR);
		display_at_P(x, y, PSTR("MIK"));
	}
	else
	{
		colmain_setcolors(BGCOLOR, BGCOLOR);
		display_at_P(x, y, PSTR("   "));
	}
#endif /* WITHDSPEXTDDC */
}

// RX preamplifier или переполнение АЦП (надо показывать как REDRM_BARS - с таймерным обновлением)
static void display2_preovf3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	if (boad_fpga_adcoverflow() != 0)
	{
		colmain_setcolors(OVFCOLOR, BGCOLOR);
		display_at_P(x, y, PSTR("OVF"));
	}
	else if (boad_mike_adcoverflow() != 0)
	{
		colmain_setcolors(BGCOLOR, OVFCOLOR);
		display_at_P(x, y, PSTR("MIK"));
	}
	else
	{
		colmain_setcolors(LABELTEXT, LABELBACK);
		display_at_P(x, y, hamradio_get_pre_value_P());
	}
}

static void display2_preovf5alt(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
	enum { chars_W2 = 5 };

	if (boad_fpga_adcoverflow() != 0)
	{
		const char str [] = "OVF";
		layout_label1_medium(x, y, str, 3, chars_W2, COLORMAIN_WHITE, OVFCOLOR);
	}
	else if (boad_mike_adcoverflow() != 0)
	{
		const char str [] = "MIC";
		layout_label1_medium(x, y, str, 3, chars_W2, COLORMAIN_WHITE, OVFCOLOR);
	}
	else
	{
		const char * str = hamradio_get_pre_value_P();
		layout_label1_medium(x, y, str, strlen_P(str), chars_W2, COLORMAIN_BLACK, colors_2state_alt [1]);
	}
}

// display antenna
static void display2_ant5(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHANTSELECT
	const char FLASHMEM * const labels [1] = { hamradio_get_ant5_value_P(), };
	ASSERT(strlen(labels [0]) == 5);
	display2_text_P(x, y, labels, colors_1state, 0);
#endif /* WITHANTSELECT */
}

// display antenna
static void display2_ant7alt(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHANTSELECT
	const char FLASHMEM * const labels [1] = { hamradio_get_ant5_value_P(), };
	layout_label1_medium(x, y, labels [0], strlen_P(labels [0]), 7, COLORMAIN_BLACK, colors_2state_alt [1]);
#endif /* WITHANTSELECT */
}

// RX att (or att/pre)
static void display2_att4(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	const char FLASHMEM * const labels [1] = { hamradio_get_att_value_P(), };
	ASSERT(strlen(labels [0]) == 4);
	display2_text_P(x, y, labels, colors_1state, 0);
}

// RX att (or att/pre)
static void display2_att5alt(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
	const char FLASHMEM * const labels [1] = { hamradio_get_att_value_P(), };
	layout_label1_medium(x, y, labels [0], strlen_P(labels [0]), 5, COLORMAIN_BLACK, colors_2state_alt [1]);
}

// HP/LP
static void display_hplp2(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHPOWERLPHP
	const char FLASHMEM * const labels [1] = { hamradio_get_hplp_value_P(), };
	ASSERT(strlen(labels [0]) == 2);
	display2_text_P(x, y, labels, colors_1state, 0);
#endif /* WITHPOWERLPHP */
}

// RX att, при передаче показывает TX
static void display_att_tx3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	const uint_fast8_t tx = hamradio_get_tx();
	const FLASHMEM char * text = tx ? PSTR("TX  ") : hamradio_get_att_value_P();

	colmain_setcolors(LABELTEXT, LABELBACK);
	ASSERT(strlen(text) == 3);
	display_at_P(x, y, text);
}

// RX agc
static void display2_agc3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	ASSERT(strlen(hamradio_get_agc3_value_P()) == 3);
	display_1state_P(x, y, hamradio_get_agc3_value_P());
}

// RX agc
static void display_agc4(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	ASSERT(strlen(hamradio_get_agc4_value_P()) == 4);
	display_1state_P(x, y, hamradio_get_agc4_value_P());
}

// VFO mode - одним символом (первым от слова SPLIT или пробелом)
static void display_vfomode1(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	uint_fast8_t state;	// state - признак активного SPLIT (0/1)
	const char * const label = hamradio_get_vfomode3_value(& state);

	colmain_setcolors(LABELTEXT, LABELBACK);
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		uint_fast16_t ypix;
		uint_fast16_t xpix = display_wrdata_begin(x, y, & ypix);
		display_put_char_small(xpix, ypix, label [0], lowhalf);
		display_wrdata_end();
	} while (lowhalf --);
}

// SSB/CW/AM/FM/...
static void display2_mode3_a(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	const char FLASHMEM * const labels [1] = { hamradio_get_mode_a_value_P(), };
	ASSERT(strlen(labels [0]) == 3);
	display2_text_P(x, y, labels, colors_1mode, 0);
}

#if WITHTOUCHGUI

static void display2_mode_lower_a(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
	char labels[5];
	local_snprintf_P(labels, ARRAY_SIZE(labels), PSTR(" %s"), hamradio_get_mode_a_value_P());
	PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();
	colpip_string2_tbg(fr, DIM_X, DIM_Y, GRID2X(x), GRID2Y(y), labels, colors_1mode [0].fg);
}

#endif /* WITHTOUCHGUI */

// SSB/CW/AM/FM/...
static void display2_mode3_b(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	const char FLASHMEM * const label = hamradio_get_mode_b_value_P();
	const char FLASHMEM * const labels [2] = { label, label };
	uint_fast8_t state;	// state - признак активного SPLIT (0/1)
	hamradio_get_vfomode3_value(& state);
	ASSERT(strlen(labels [0]) == 3);
	display2_text_P(x, y, labels, colors_2modeB, state);
}

// dd.dV - 5 places
static void display2_voltlevelV5(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHVOLTLEVEL
	uint_fast8_t volt = hamradio_get_volt_value();	// Напряжение в сотнях милливольт т.е. 151 = 15.1 вольта
	//PRINTF("display2_voltlevelV5: volt=%u\n", volt);
	colmain_setcolors(colors_1state [0].fg, colors_1state [0].bg);
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_value_small(x + CHARS2GRID(0), y + lowhalf, volt, 3, 1, 255, 0, lowhalf);
	} while (lowhalf --);
	display_at_P(x + CHARS2GRID(4), y, PSTR("V"));
#endif /* WITHVOLTLEVEL */
}

// dd.d - 4 places
static void display_voltlevel4(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHVOLTLEVEL
	const uint_fast8_t volt = hamradio_get_volt_value();	// Напряжение в сотнях милливольт т.е. 151 = 15.1 вольта
	//PRINTF("display_voltlevel4: volt=%u\n", volt);

	colmain_setcolors(colors_1state [0].fg, colors_1state [0].bg);
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_value_small(x, y + lowhalf, volt, 3, 1, 255, 0, lowhalf);
	} while (lowhalf --);
#endif /* WITHVOLTLEVEL */
}

// отображение градусов с десятыми долями
static void display2_thermo4(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHTHERMOLEVEL
	int_fast16_t tempv = hamradio_get_temperature_value();	// Градусы в десятых долях

	// 50+ - красный
	// 30+ - желтый
	// ниже 30 зеленый
	if (tempv < 0)
	{
		tempv = - tempv;
		colmain_setcolors(COLORMAIN_WHITE, display_getbgcolor());
	}
	else if (tempv >= 500)
		colmain_setcolors(COLORMAIN_RED, display_getbgcolor());
	else if (tempv >= 300)
		colmain_setcolors(COLORMAIN_YELLOW, display_getbgcolor());
	else
		colmain_setcolors(COLORMAIN_GREEN, display_getbgcolor());

	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_value_small(x + CHARS2GRID(0), y + lowhalf, tempv, 3, 1, 255, 0, lowhalf);
	} while (lowhalf --);
#endif /* WITHTHERMOLEVEL */
}

// отображение градусов с десятыми долями и "C"
static void display2_thermo5(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHTHERMOLEVEL
	int_fast16_t tempv = hamradio_get_temperature_value();	// Градусы в десятых долях

	// 50+ - красный
	// 30+ - желтый
	// ниже 30 зеленый
	if (tempv < 0)
	{
		tempv = - tempv;
		colmain_setcolors(COLORMAIN_WHITE, display_getbgcolor());
	}
	else if (tempv >= 500)
		colmain_setcolors(COLORMAIN_RED, display_getbgcolor());
	else if (tempv >= 300)
		colmain_setcolors(COLORMAIN_YELLOW, display_getbgcolor());
	else
		colmain_setcolors(COLORMAIN_GREEN, display_getbgcolor());

	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_value_small(x + CHARS2GRID(0), y + lowhalf, tempv, 3, 1, 255, 0, lowhalf);
	} while (lowhalf --);
	display_at_P(x + CHARS2GRID(4), y, PSTR("C"));
#endif /* WITHTHERMOLEVEL */
}

// +d.ddA - 5 places (with "A")
// +dd.dA - 5 places (with "A")
static void display2_currlevelA6(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHCURRLEVEL || WITHCURRLEVEL2
	#if (WITHCURRLEVEL_ACS712_30A || WITHCURRLEVEL_ACS712_20A)

		int_fast16_t drain = hamradio_get_pacurrent_value();	// Ток в десятках милиампер (до 2.55 ампера), может быть отрицательным

		colmain_setcolors(colors_1state [0].fg, colors_1state [0].bg);
		uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
		do
		{
			display_value_small(x + CHARS2GRID(0), y + lowhalf, drain, 3 | WMINUSFLAG, 1, 255, 1, lowhalf);
		} while (lowhalf --);
		display_at_P(x + CHARS2GRID(5), y, PSTR("A"));

	#else /* WITHCURRLEVEL_ACS712_30A */
		// dd.d - 5 places (without "A")
		int_fast16_t drain = hamradio_get_pacurrent_value();	// Ток в десятках милиампер (до 2.55 ампера), может быть отрицательным

		colmain_setcolors(colors_1state [0].fg, colors_1state [0].bg);
		uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
		do
		{
			display_value_small(x + CHARS2GRID(0), y + lowhalf, drain, 3 | WMINUSFLAG, 2, 255, 0, lowhalf);
		} while (lowhalf --);
		display_at_P(x + CHARS2GRID(5), y, PSTR("A"));

	#endif /* WITHCURRLEVEL_ACS712_30A */
#endif /* WITHCURRLEVEL || WITHCURRLEVEL2 */
}

// +d.dd - 5 places (without "A")
// +dd.d - 5 places (without "A")
static void display2_currlevel5(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHCURRLEVEL || WITHCURRLEVEL2
	#if (WITHCURRLEVEL_ACS712_30A || WITHCURRLEVEL_ACS712_20A)

		int_fast16_t drain = hamradio_get_pacurrent_value();	// Ток в десятках милиампер (до 2.55 ампера), может быть отрицательным

		colmain_setcolors(colors_1state [0].fg, colors_1state [0].bg);
		uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
		do
		{
			display_value_small(x + CHARS2GRID(0), y + lowhalf, drain, 3 | WMINUSFLAG, 1, 255, 1, lowhalf);
		} while (lowhalf --);
		//display_at_P(x + CHARS2GRID(5), y, PSTR("A"));

	#else /* WITHCURRLEVEL_ACS712_30A */
		// dd.d - 5 places (without "A")
		int_fast16_t drain = hamradio_get_pacurrent_value();	// Ток в десятках милиампер (до 2.55 ампера), может быть отрицательным

		colmain_setcolors(colors_1state [0].fg, colors_1state [0].bg);
		uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
		do
		{
			display_value_small(x + CHARS2GRID(0), y + lowhalf, drain, 3 | WMINUSFLAG, 2, 255, 0, lowhalf);
		} while (lowhalf --);
		//display_at_P(x + CHARS2GRID(5), y, PSTR("A"));

	#endif /* WITHCURRLEVEL_ACS712_30A */
#endif /* WITHCURRLEVEL || WITHCURRLEVEL2 */
}

// Отображение уровня сигнала в dBm
static void display_siglevel7(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHIF4DSP
	uint_fast8_t tracemax;
	uint_fast8_t v = board_getsmeter(& tracemax, 0, UINT8_MAX, 0);

	char buf2 [8];
	// в формате при наличии знака числа ширина формата отностися ко всему полю вместе со знаком
	local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%-+4d" "dBm"), tracemax - UINT8_MAX);
	(void) v;
	const char * const labels [1] = { buf2, };
	ASSERT(strlen(buf2) == 7);
	display2_text(x, y, labels, colors_1state, 0);
#endif /* WITHIF4DSP */
}

// Отображение уровня сигнала в dBm
static void display2_siglevel4(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHIF4DSP
	uint_fast8_t tracemax;
	uint_fast8_t v = board_getsmeter(& tracemax, 0, UINT8_MAX, 0);

	char buf2 [5];
	// в формате при наличии знака числа ширина формата отностися ко всему полю вместе со знаком
	int j = local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%-+4d"), (int) (tracemax - UINT8_MAX));
	(void) v;
	const char * const labels [1] = { buf2, };
	ASSERT(strlen(buf2) == 4);
	display2_text(x, y, labels, colors_1state, 0);
#endif /* WITHIF4DSP */
}

#if WITHIF4DSP
int_fast32_t display_zoomedbw(void)
{
	return ((int_fast64_t) dsp_get_samplerateuacin_rts() * SPECTRUMWIDTH_MULT / SPECTRUMWIDTH_DENOM) >> glob_zoomxpow2;
}
#endif /* WITHIF4DSP */

static void display2_span9(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHIF4DSP

	char buf2 [10];

	local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("SPAN:%3dk"), (int) ((display_zoomedbw() + 0) / 1000));
	const char * const labels [1] = { buf2, };
	ASSERT(strlen(buf2) == 9);
	display2_text(x, y, labels, colors_1state, 0);

#endif /* WITHIF4DSP */
}
// Отображение уровня сигнала в баллах шкалы S
// S9+60
static void display_smeter5(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHIF4DSP
	uint_fast8_t tracemax;
	uint_fast8_t v = board_getsmeter(& tracemax, 0, UINT8_MAX, 0);

	char buf2 [6];
	const int s9level = - 73;
	const int s9step = 6;
	const int alevel = tracemax - UINT8_MAX;

	(void) v;
	if (alevel < (s9level - s9step * 9))
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("S0   "));
	}
	else if (alevel < (s9level - s9step * 7))
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("S1   "));
	}
	else if (alevel < (s9level - s9step * 6))
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("S2   "));
	}
	else if (alevel < (s9level - s9step * 5))
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("S3   "));
	}
	else if (alevel < (s9level - s9step * 4))
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("S4   "));
	}
	else if (alevel < (s9level - s9step * 3))
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("S5   "));
	}
	else if (alevel < (s9level - s9step * 2))
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("S6   "));
	}
	else if (alevel < (s9level - s9step * 1))
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("S7   "));
	}
	else if (alevel < (s9level - s9step * 0))
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("S8   "));
	}
	else
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("S9+%02d"), alevel - s9level);
	}
	const char * const labels [1] = { buf2, };
	ASSERT(strlen(buf2) == 5);
	display2_text(x, y, labels, colors_1state, 0);
#endif /* WITHIF4DSP */
}

// Отображение уровня сигнала в dBm или S-memter (в зависимости от настроек)
static void display2_smeors5(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
	if (glob_showdbm != 0)
	{
		display2_siglevel4(x, y, pctx);
	}
	else
	{
		display_smeter5(x, y, pctx);
	}
}

static void display2_freqdelta8(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHINTEGRATEDDSP
	int_fast32_t deltaf;
	const uint_fast8_t f = dsp_getfreqdelta10(& deltaf, 0);		/* Получить значение отклонения частоты с точностью 0.1 герца для приемника A */
	deltaf = - deltaf;	// ошибка по частоте преобразуется в расстройку
	colmain_setcolors(colors_1state [0].fg, colors_1state [0].bg);
	if (f != 0)
	{
		uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
		do
		{
			display_value_small(x, y + lowhalf, deltaf, 6 | WSIGNFLAG, 1, 255, 0, lowhalf);
		} while (lowhalf --);
	}
	else
	{
		display_at_P(x, y, PSTR("        "));
	}
#endif /* WITHINTEGRATEDDSP */
}

/* Получить информацию об ошибке настройки в режиме SAM */
static void display_samfreqdelta8(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHINTEGRATEDDSP
	int_fast32_t deltaf;
	const uint_fast8_t f = hamradio_get_samdelta10(& deltaf, 0);		/* Получить значение отклонения частоты с точностью 0.1 герца для приемника A */
	deltaf = - deltaf;	// ошибка по частоте преобразуется в расстройку
	colmain_setcolors(colors_1state [0].fg, colors_1state [0].bg);
	if (f != 0)
	{
		uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
		do
		{
			display_value_small(x, y + lowhalf, deltaf, 6 | WSIGNFLAG, 1, 255, 0, lowhalf);
		} while (lowhalf --);
	}
	else
	{
		display_at_P(x, y, PSTR("        "));
	}
#endif /* WITHINTEGRATEDDSP */
}

// d.d - 3 places
// текущее значение верхней частоты среза НЧ фильтра АМ/ЧМ
static void display_amfmhighcut4(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if WITHAMHIGHKBDADJ
	uint_fast8_t flag;
	const uint_fast8_t v = hamradio_get_amfm_highcut10_value(& flag);	// текущее значение верхней частоты среза НЧ фильтра АМ/ЧМ (в десятках герц)

	colmain_setcolors(colors_2state [flag].fg, colors_2state [flag].bg);
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_value_small(x, y, v, 3, 2, 255, 0, lowhalf);
	} while (lowhalf --);
#endif /* WITHAMHIGHKBDADJ */
}

// dd.d - 4 places
// текущее значение верхней частоты среза НЧ фильтра АМ/ЧМ
static void display_amfmhighcut5(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHAMHIGHKBDADJ
	uint_fast8_t flag;
	const uint_fast8_t v = hamradio_get_amfm_highcut10_value(& flag);	// текущее значение верхней частоты среза НЧ фильтра АМ/ЧМ (в десятках герц)

	colmain_setcolors(colors_2state [flag].fg, colors_2state [flag].bg);
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_value_small(x, y, v, 4, 2, 255, 0, lowhalf);
	} while (lowhalf --);
#endif /* WITHAMHIGHKBDADJ */
}

// Печать времени - часы, минуты и секунды
static void display_time8(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if defined (RTC1_TYPE)
	uint_fast8_t hour, minute, secounds;
	char buf2 [9];

	board_rtc_gettime(& hour, & minute, & secounds);
	local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%02d:%02d:%02d"),
			(int) hour, (int) minute, (int) secounds
		);
	
	const char * const labels [1] = { buf2, };
	display2_text(x, y, labels, colors_1state, 0);
#endif /* defined (RTC1_TYPE) */
}

// Печать времени - только часы и минуты, без секунд
static void display_time5(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if defined (RTC1_TYPE)
	uint_fast8_t hour, minute, secounds;
	char buf2 [6];

	board_rtc_gettime(& hour, & minute, & secounds);
	local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%02d%c%02d"),
		(int) hour,
		((secounds & 1) ? ' ' : ':'),	// мигающее двоеточие с периодом две секунды
		(int) minute
		);

	ASSERT(strlen(buf2) == 5);
	const char * const labels [1] = { buf2, };
	display2_text(x, y, labels, colors_1stateBlue, 0);

#endif /* defined (RTC1_TYPE) */
}

// Печать частоты SOF пакетов USB device.
static void display2_freqsof9(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHUSBHW && defined (WITHUSBHW_DEVICE)
	char buf2 [13];
	unsigned v = hamradio_get__getsoffreq();
	v = ulmin(v, 99999);
	local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("Sof:%5u"),
			v
		);

	const char * const labels [1] = { buf2, };
	ASSERT(strlen(buf2) == 9);
	display2_text(x, y, labels, colors_1stateBlue, 0);
#endif /*  WITHUSBHW && defined (WITHUSBHW_DEVICE) */
}

// Печать времени - только часы и минуты, без секунд
// Jan-01 13:40
static void display2_datetime12(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if defined (RTC1_TYPE)
	char buf2 [13];

	uint_fast16_t year;
	uint_fast8_t month, day;
	uint_fast8_t hour, minute, secounds;
	static const char months [12] [4] =
	{
		"JAN",
		"FEB",
		"MAR",
		"APR",
		"MAY",
		"JUN",
		"JUL",
		"AUG",
		"SEP",
		"OCT",
		"NOV",
		"DEC",
	};

	board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & secounds);

	local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%s-%02d %02d%c%02d"),
		months [month - 1],
		(int) day,
		(int) hour,
		((secounds & 1) ? ' ' : ':'),	// мигающее двоеточие с периодом две секунды
		(int) minute
		);

	const char * const labels [1] = { buf2, };
	ASSERT(strlen(buf2) == 12);
	display2_text(x, y, labels, colors_1stateBlue, 0);
#endif /* defined (RTC1_TYPE) */
}

struct dzone
{
	uint8_t x; // левый верхний угол
	uint8_t y;
	void (* redraw)(uint_fast8_t x, uint_fast8_t y, dctx_t * pctx);	// функция отображения элемента
	uint8_t key;		// при каких обновлениях перерисовывается этот элемент
	uint8_t subset;
};

/* struct dzone subset field values */

#define PAGEINIT 6
#define PAGESLEEP 7

#define REDRSUBSET(page)		(1U << (page))	// сдвиги соответствуют номеру отображаемого набора элементов

#define REDRSUBSET_ALL ( \
		REDRSUBSET(0) | \
		REDRSUBSET(1) | \
		REDRSUBSET(2) | \
		REDRSUBSET(3) | \
		0)

#define REDRSUBSET_MENU		REDRSUBSET(4)
#define REDRSUBSET_MENU2	REDRSUBSET(5)
#define REDRSUBSET_SLEEP	REDRSUBSET(PAGESLEEP)
#define REDRSUBSET_INIT		REDRSUBSET(PAGEINIT)

enum
{
	REDRM_MODE,		// поля меняющиемя при изменении режимов работы, LOCK state
	REDRM_FREQ,		// индикаторы частоты
	REDRM_FRQB,	// индикаторы частоты
	REDRM_BARS,		// S-meter, SWR-meter, voltmeter
	REDRM_VOLT,		// вольтметр (редко меняющиеся параметры)

	REDRM_MFXX,		// код редактируемого параметра
	REDRM_MLBL,		// название редактируемого параметра
	REDRM_MVAL,		// значение параметра меню
	REDRM_BUTTONS,  // область отображения экранных кнопок
	REDRM_INIS,  	// инициализирующие процедцры экранных элементоы
	REDRM_count
};

/* Описания расположения элементов на дисплеях */

#if DSTYLE_T_X20_Y4

	enum
	{
		BDTH_ALLRX = 14,	// ширина зоны для отображение полосы на индикаторе
		BDTH_RIGHTRX = 5,	// ширина индикатора плюсов
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
		BDTH_SPACERX = 0,
		//
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 9,
		BDTH_SPACESWR = 1,
		BDTH_ALLPWR = 4,
		BDTH_SPACEPWR = 0
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

	enum {
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DPAGE1,					// Страница, в которой отображаются основные (или все)
		DISPLC_MODCOUNT,
	};
	enum {
		DPAGEEXT = REDRSUBSET(DPAGE0) | REDRSUBSET(DPAGE1)
	};
	#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
	#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0, 0,	display_freqchr_a,	REDRM_FREQ, DPAGEEXT, },	// частота для символьных дисплеев
		//{	0, 0,	display_txrxstate2,	REDRM_MODE, DPAGEEXT, },
		{	9, 0,	display2_mode3_a,	REDRM_MODE,	DPAGEEXT, },	// SSB/CW/AM/FM/...
		{	12, 0,	display_lockstate1,	REDRM_MODE, DPAGEEXT, },
		{	13, 0,	display2_rxbw3,		REDRM_MODE, DPAGEEXT, },
		{	17, 0,	display2_vfomode3,	REDRM_MODE, DPAGEEXT, },	// SPLIT

		{	0, 1,	display_att_tx3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
		{	4, 1,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
	#if defined (RTC1_TYPE)
		{	8, 1,	display_time8,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME
	#endif /* defined (RTC1_TYPE) */
	#if WITHUSEDUALWATCH
		{	0, 1,	display_freqchr_b,	REDRM_FREQ, REDRSUBSET(DPAGE1), },	// частота для символьных дисплеев
		{	17, 1,	display2_mainsub3, REDRM_MODE, DPAGEEXT, },	// main/sub RX
	#endif /* WITHUSEDUALWATCH */

		{	0, 2,	display2_bars,		REDRM_BARS, DPAGEEXT, },	// S-METER, SWR-METER, POWER-METER
		{	17, 2,	display2_voxtune3,	REDRM_MODE, DPAGEEXT, },

	#if WITHVOLTLEVEL && WITHCURRLEVEL
		{	0, 3,	display_voltlevel4, REDRM_VOLT, DPAGEEXT, },	// voltmeter with "V"
		{	5, 3,	display2_currlevel5, REDRM_VOLT, DPAGEEXT, },	// without "A"
	#endif /* WITHVOLTLEVEL && WITHCURRLEVEL */
#if WITHIF4DSP
	#if WITHUSEAUDIOREC
		{	13, 3,	display2_rec3,		REDRM_BARS, DPAGEEXT, },	// Отображение режима записи аудио фрагмента
	#endif /* WITHUSEAUDIOREC */
#endif /* WITHIF4DSP */
		{	17, 3,	display2_agc3,		REDRM_MODE, DPAGEEXT, },

	#if WITHMENU 
		{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
		{	0, 1,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
		{	4, 1,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
		{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
	#endif /* WITHMENU */
	};
	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 1;
			p->ystep = 1;	// количество ячеек разметки на одну строку меню
			p->reverse = 0;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

#elif DSTYLE_T_X20_Y2
	/*
		Вот то, как сейчас описан дисплей 2*20, предлагаю внести в это
		изменения (вместо рисования картинок) - это будет отдельный,
		твой персональный формат.
		Можно указывать, какой элемент в каком из наборов отображаемых символов находится.
		Если есть желание для приёма и передачи сделать разные элементы - укажи это в комментарии,
		я дополню. Просто эти замещающие друг друга элементы должны занимать одинаковое
		место на экране, затирая старое изображение при переключении режима отображения.
		То, что переключается по MENU (REDRSUBSET(0)/REDRSUBSET(1)) - перерисовывается со стиранием экрана.

	*/
	enum
	{
		BDTH_ALLRX = 12,	// ширина зоны для отображение полосы на индикаторе
		BDTH_RIGHTRX = 4,	// ширина индикатора плюсов
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
		BDTH_SPACERX = 1,		/* количество позиций, затираемых справа от полосы S-метра */
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 6,
		BDTH_SPACESWR = 1,	/* количество позиций, затираемых справа от полосы SWR-метра */
		BDTH_ALLPWR = 5,
		BDTH_SPACEPWR = 0	/* количество позиций, затираемых справа от полосы PWR-метра */
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

	enum {
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DPAGE_SMETER,
		DPAGE_TIME,
		DISPLC_MODCOUNT
	};
	#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
	#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
	static const FLASHMEM struct dzone dzones [] =
	{
		// строка 0
		{	0, 0,	display2_vfomode3,	REDRM_MODE, REDRSUBSET_ALL, },	// SPLIT
		{	4, 0,	display_freqchr_a,	REDRM_FREQ, REDRSUBSET_ALL, },	// частота для символьных дисплеев
		{	12, 0,	display_lockstate1,	REDRM_MODE, REDRSUBSET_ALL, },
		{	13, 0,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET_ALL, },	// SSB/CW/AM/FM/...
		{	17, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET_ALL, },
		// строка 1 - постоянные элементы
		{	0, 1,	display2_voxtune3,	REDRM_MODE, REDRSUBSET_ALL, },
		// строка 1
		{	4, 1,	display_att_tx3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
	#if WITHDSPEXTDDC
		{	9, 1,	display2_preovf3,	REDRM_BARS, REDRSUBSET(DPAGE0), },	// ovf/pre
	#else /* WITHDSPEXTDDC */
		{	9, 1,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// pre
	#endif /* WITHDSPEXTDDC */
		// строка 1
		{	4, 1,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE_SMETER), },	// S-METER, SWR-METER, POWER-METER
		{	4, 1,	display_time8,		REDRM_BARS, REDRSUBSET(DPAGE_TIME), },	// TIME
		// строка 1 - постоянные элементы
		{	17, 1,	display2_agc3,		REDRM_MODE, REDRSUBSET_ALL, },

		//{	0, 0,	display_txrxstate2,	REDRM_MODE, REDRSUBSET_ALL, },
	#if WITHMENU
		{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
		{	0, 1,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
		{	4, 1,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
		{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
	#endif /* WITHMENU */
	};
#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		p->multilinemenu_max_rows = 1;
		p->ystep = 1;	// количество ячеек разметки на одну строку меню
		p->reverse = 0;
		p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

#elif DSTYLE_T_X20_Y2_IGOR

	enum
	{
		BDTH_ALLRX = 12,	// ширина зоны для отображение полосы на индикаторе
		BDTH_RIGHTRX = 4,	// ширина индикатора плюсов
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
		BDTH_SPACERX = 1,		/* количество позиций, затираемых справа от полосы S-метра */
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 6,
		BDTH_SPACESWR = 1,	/* количество позиций, затираемых справа от полосы SWR-метра */
		BDTH_ALLPWR = 5,
		BDTH_SPACEPWR = 0	/* количество позиций, затираемых справа от полосы PWR-метра */
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

	enum {
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DPAGE1 = 0,					// Страница, в которой отображаются основные (или все)
		DISPLC_MODCOUNT
	};
	#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
	#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0, 0,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0) | REDRSUBSET(DPAGE1), },	// SPLIT
		{	4, 0,	display_freqchr_a,	REDRM_FREQ, REDRSUBSET(DPAGE0) | REDRSUBSET(DPAGE1), },	// частота для символьных дисплеев
		{	12, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0) | REDRSUBSET(DPAGE1), },
		{	4, 1,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
		//{	0, 1, display_pbt,		REDRM_BARS, REDRSUBSET(DPAGE1), },	// PBT +00.00
	#if WITHMENU
		{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
		{	0, 1,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
		{	4, 1,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
		{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
	#endif /* WITHMENU */
	};
#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		p->multilinemenu_max_rows = 1;
		p->ystep = 1;	// количество ячеек разметки на одну строку меню
		p->reverse = 0;
		p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

#elif DSTYLE_T_X16_Y2 && DSTYLE_SIMPLEFREQ

	enum
	{
		BDTH_ALLRX = 12,	// ширина зоны для отображение полосы на индикаторе
		BDTH_RIGHTRX = 4,	// ширина индикатора плюсов
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
		BDTH_SPACERX = 0,
		//
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 7,
		BDTH_SPACESWR = 1,
		BDTH_ALLPWR = 4,
		BDTH_SPACEPWR = 0
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	40	// 4.0 - значение на полной шкале
	enum {
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		//
		DISPLC_MODCOUNT
	};
	#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
	#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0, 0,	display_freqchr_a,	REDRM_FREQ, REDRSUBSET_ALL, },	// частота для символьных дисплеев
	};
#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		p->multilinemenu_max_rows = 1;
		p->ystep = 1;	// количество ячеек разметки на одну строку меню
		p->reverse = 0;
		p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

#elif DSTYLE_T_X16_Y2 && CTLSTYLE_RA4YBO_AM0

	enum
	{
		BDTH_ALLRX = 15,	// ширина зоны для отображение полосы на индикаторе
		BDTH_RIGHTRX = 5,	// ширина индикатора плюсов
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
		BDTH_SPACERX = 0,
		//
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 7,
		BDTH_SPACESWR = 1,
		BDTH_ALLPWR = 7,
		BDTH_SPACEPWR = 0
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	40	// 4.0 - значение на полной шкале
	enum {
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		//
		DISPLC_MODCOUNT
	};

	#define DISPLC_WIDTH	6	// количество цифр в отображении частоты
	#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0, 0,	display_freqchr_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },	// частота для символьных дисплеев
		{	8, 0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
		{	13, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
		{	1, 1,	display2_bars_amv0,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
	#if WITHMENU 
		{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
		{	0, 1,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
		{	4, 1,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
	#endif /* WITHMENU */
	};

#elif DSTYLE_T_X16_Y2

	enum
	{
		BDTH_ALLRX = 16,	// ширина зоны для отображение полосы на индикаторе
		BDTH_RIGHTRX = 5,	// ширина индикатора плюсов
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
		BDTH_SPACERX = 0,
		//
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 7,
		BDTH_SPACESWR = 1,
		BDTH_ALLPWR = 8,
		BDTH_SPACEPWR = 0
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	40	// 4.0 - значение на полной шкале
	enum {
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DPAGE_SMETER,			// Страница с отображением S-метра, SWR-метра
	#if WITHUSEAUDIOREC
		//DPAGE_SDCARD,
	#endif /* WITHUSEAUDIOREC */
	#if WITHUSEDUALWATCH
		DPAGE_SUBRX,
	#endif /* WITHUSEDUALWATCH */
	#if defined (RTC1_TYPE)
		DPAGE_TIME,
	#endif /* defined (RTC1_TYPE) */
	#if WITHMODEM
		DPAGE_BPSK,
	#endif /* WITHMODEM */
	#if WITHVOLTLEVEL && WITHCURRLEVEL 
		DPAGE_VOLTS,
	#endif /* WITHVOLTLEVEL && WITHCURRLEVEL */
		//
		DISPLC_MODCOUNT
	};
	#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
	#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0, 0,	display_freqchr_a,	REDRM_FREQ, REDRSUBSET_ALL, },	// частота для символьных дисплеев
		{	8, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET_ALL, },
		{	9, 0,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET_ALL, },	// SSB/CW/AM/FM/...
		{	13, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET_ALL, },

		{	0, 1,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
		{	4, 1,	display_att_tx3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// при скрытом s-metre, при передаче показывает TX
	#if WITHDSPEXTDDC
		{	9, 1,	display2_preovf3,	REDRM_BARS, REDRSUBSET(DPAGE0), },	// ovf/pre
	#else /* WITHDSPEXTDDC */
		{	9, 1,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// pre
	#endif /* WITHDSPEXTDDC */
#if WITHIF4DSP
	#if WITHUSEAUDIOREC
		{	13, 1,	display2_rec3,		REDRM_BARS, REDRSUBSET(DPAGE0) /*| REDRSUBSET(DPAGE_SMETER)*/, },	// Отображение режима записи аудио фрагмента
	#endif /* WITHUSEAUDIOREC */
#else /* WITHIF4DSP */
		{	13, 1,	display2_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0) /*| REDRSUBSET(DPAGE_SMETER)*/, },
#endif /* WITHIF4DSP */

		{	0, 1,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE_SMETER), },	// S-METER, SWR-METER, POWER-METER

	#if WITHVOLTLEVEL && WITHCURRLEVEL
		{	0, 1,	display2_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE_VOLTS), },	// voltmeter with "V"
		{	6, 1,	display2_currlevelA6, REDRM_VOLT, REDRSUBSET(DPAGE_VOLTS), },	// amphermeter with "A"
	#endif /* WITHVOLTLEVEL && WITHCURRLEVEL */
	#if WITHMODEM
		{	0, 1,	display2_freqdelta8, REDRM_BARS, REDRSUBSET(DPAGE_BPSK), },	// выход ЧМ демодулятора
	#endif /* WITHMODEM */
	#if WITHUSEDUALWATCH
		{	0, 1,	display_freqchr_b,	REDRM_FREQ, REDRSUBSET(DPAGE_SUBRX), },	// FREQ B
		{	9, 1,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE_SUBRX), },	// SPLIT
		{	13, 1,	display2_mainsub3, REDRM_MODE, REDRSUBSET(DPAGE_SUBRX), },	// main/sub RX
	#endif /* WITHUSEDUALWATCH */

	//#if WITHUSEAUDIOREC
	//	{	0, 1,	display_recstatus,	REDRM_BARS, REDRSUBSET(DPAGE_SDCARD), },	// recording debug information
	//	{	13, 1,	display2_rec3,		REDRM_BARS, REDRSUBSET(DPAGE_SDCARD), },	// Отображение режима записи аудио фрагмента
	//#endif /* WITHUSEAUDIOREC */

	#if defined (RTC1_TYPE)
		{	0, 1,	display_time8,		REDRM_BARS, REDRSUBSET(DPAGE_TIME), },	// TIME
	#if WITHUSEDUALWATCH
		{	9, 1,	display2_mainsub3,	REDRM_BARS, REDRSUBSET(DPAGE_TIME), },	// main/sub RX
	#endif /* WITHUSEDUALWATCH */
	#if WITHUSEAUDIOREC
		{	13, 1,	display2_rec3,		REDRM_BARS, REDRSUBSET(DPAGE_TIME), },	// Отображение режима записи аудио фрагмента
	#endif /* WITHUSEAUDIOREC */
	#endif /* defined (RTC1_TYPE) */


		//{	0, 0,	display_txrxstate2,	REDRM_MODE, REDRSUBSET_ALL, },
		//{	0, 0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET_ALL, },
	#if WITHMENU
		{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
		{	0, 1,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
		{	4, 1,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
		{	15, 0,	display_lockstate1,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
	#endif /* WITHMENU */
	};
#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		p->multilinemenu_max_rows = 1;
		p->ystep = 1;	// количество ячеек разметки на одну строку меню
		p->reverse = 0;
		p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

#elif DSTYLE_T_X16_Y4

	enum
	{
		BDTH_ALLRX = 14,	// ширина зоны для отображение полосы на индикаторе
		BDTH_RIGHTRX = 5,	// ширина индикатора плюсов
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
		BDTH_SPACERX = 0,
		//
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 9,
		BDTH_SPACESWR = 1,
		BDTH_ALLPWR = 4,
		BDTH_SPACEPWR = 0
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	40	// 4.0 - значение на полной шкале

	enum {
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DISPLC_MODCOUNT
	};
	#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
	#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0, 0,	display_freqchr_a,	REDRM_FREQ, REDRSUBSET_ALL, },	// частота для символьных дисплеев
		{	8, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET_ALL, },
		{	9, 0,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET_ALL, },	// SSB/CW/AM/FM/...
		{	13, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET_ALL, },

		{	0, 1,	display2_vfomode3,	REDRM_MODE, REDRSUBSET_ALL, },	// SPLIT
		{	4, 1,	display_att_tx3,	REDRM_MODE, REDRSUBSET_ALL, },		// при передаче показывает TX
	#if WITHDSPEXTDDC
		{	9, 1,	display2_preovf3,	REDRM_BARS, REDRSUBSET_ALL, },	// ovf/pre
	#else /* WITHDSPEXTDDC */
		{	9, 1,	display_pre3,		REDRM_MODE, REDRSUBSET_ALL, },	// pre
	#endif /* WITHDSPEXTDDC */

		{	0, 2,	display2_bars,		REDRM_BARS, REDRSUBSET_ALL, },	// S-METER, SWR-METER, POWER-METER

		{	0, 3,	display2_voxtune3,	REDRM_MODE, REDRSUBSET_ALL, },
	#if defined (RTC1_TYPE)
		{	4, 3,	display_time8,		REDRM_BARS, REDRSUBSET_ALL, },	// TIME
	#endif /* defined (RTC1_TYPE) */
		{	13, 3,	display2_agc3,		REDRM_MODE, REDRSUBSET_ALL, },

		//{	0, 0,	display_txrxstate2,	REDRM_MODE, REDRSUBSET_ALL, },
		//{	0, 0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET_ALL, },

	#if WITHMENU
		{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра параметра
		{	0, 1,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
		{	4, 1,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
		{	15, 0,	display_lockstate1,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
	#endif /* WITHMENU */
	};
#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		p->multilinemenu_max_rows = 1;
		p->ystep = 1;	// количество ячеек разметки на одну строку меню
		p->reverse = 0;
		p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

#elif DSTYLE_G_X64_Y32
	// RDX0120
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */

	enum
	{
		BDTH_ALLRX = 9,	// ширина зоны для отображение полосы на индикаторе
		BDTH_RIGHTRX = 3,	// ширина индикатора плюсов
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
		BDTH_SPACERX = 0,
		//
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	};
	enum
	{
		PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
		PATTERN_BAR_FULL = 0x7e,	//0x7C,	// правый бит - нижний
		PATTERN_BAR_HALF = 0x1e,	//0x38,	// правый бит - нижний
		PATTERN_BAR_EMPTYFULL = 0x42,	//0x00
		PATTERN_BAR_EMPTYHALF = 0x12	//0x00
	};
	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

	enum {
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DISPLC_MODCOUNT
	};
	#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
	#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
		{	3, 0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
		{	7, 0,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

		{	0, 1,	display2_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },

		{	9, 2,	display_vfomode1,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT - одним символом
		{	0, 2,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER

		{	6, 3,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
		{	9, 3,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
		{	0, 3,	display2_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
	#if WITHMENU 
		{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
		{	0, 3,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
		{	0, 2,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
		{	9, 3,	display_lockstate1,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
	#endif /* WITHMENU */
	};
#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		p->multilinemenu_max_rows = 1;
		p->ystep = 1;	// количество ячеек разметки на одну строку меню
		p->reverse = 0;
		p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

#elif DSTYLE_G_X128_Y64
	// RDX0077
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP

		enum
		{
			BDTH_ALLRX = 21,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 11,	// ширина индикатора плюсов
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 11,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//					  "012345678901234567890"
		#define SMETERMAP 	  " 1 3 5 7 9 + 20 40 60"	//
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			#define SWRPWRMAP "1 | 2 | 3 0%   | 100%" 
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		#else
			#define POWERMAP  " 0 10 20 40 60 80 100"
			#define SWRMAP    "1    |    2    |   3 "	// 
			#define SWRMAX	(SWRMIN * 31 / 10)	// 3.1 - значение на полной шкале
		#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
		#if 1
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x7e,
			PATTERN_BAR_EMPTYFULL = 0x42,
			PATTERN_BAR_EMPTYHALF = 0x42
		#else
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x42,
			PATTERN_BAR_EMPTYHALF = 0x24
		#endif
		};

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },	// TX/RX
			{	3, 0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },		// VOX/TUN
			{	7, 0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	10, 0,	display_hplp2,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	13, 0,	display_voltlevel4, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter without "V"
			{	18, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },


			{	0,	1,	display2_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	18, 2,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	2, 4,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	7, 4,	display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },
			{	18, 4,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...


			{	2, 5,	display2_lockstate4, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7, 5,	display2_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	11, 5,	display2_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0, 6,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
			{	0, 7,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },// Отображение оцифровки шкалы S-метра
		#if WITHMENU
			{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
			{	0, 1,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
			{	4, 1,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
			{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
		#endif /* WITHMENU */
		};
	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 1;
			p->ystep = 1;	// количество ячеек разметки на одну строку меню
			p->reverse = 0;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#else /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */

		enum
		{
			BDTH_ALLRX = 17,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 6,	// ширина индикатора плюсов
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 10,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 6,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//#define SMETERMAP "1 3 5 7 9 + 20 40 60"
		//#define POWERMAP  "0 10 20 40 60 80 100"
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x42,
			PATTERN_BAR_EMPTYHALF = 0x24
		};

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0,	0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3,	0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7,	0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	16, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	18, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	12, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	0,	2,	display2_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },		// col = 1 for !WIDEFREQ
			{	18, 3,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			{	18, 5,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			{	0,	5,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	7,	5,	display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },		// x=8 then !WIDEFREQ
			{	18, 7,	display2_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	0,	7,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
		#if WITHMENU
			{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
			{	0, 1,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
			{	4, 1,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
			{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
		#endif /* WITHMENU */
		};
	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 1;
			p->ystep = 1;	// количество ячеек разметки на одну строку меню
			p->reverse = 0;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#endif /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */

#elif DSTYLE_G_X132_Y64
	// RDX0154
	// по горизонтали 22 знакоместа (x=0..21)
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP
		// SW20XXX 
		enum
		{
			BDTH_ALLRX = 21,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 11,	// ширина индикатора плюсов
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 11,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//					  "0123456789012345678901"
		#define SMETERMAP 	  " 1 3 5 7 9 + 20 40 60 "	//
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			#define SWRPWRMAP "1 | 2 | 3 0%   | 100% " 
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		#else
			#define POWERMAP  " 0 10 20 40 60 80 100 "
			#define SWRMAP    "1    |    2    |   3  "	// 
			#define SWRMAX	(SWRMIN * 32 / 10)	// 3.2 - значение на полной шкале
		#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
		#if 1
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x7e,
			PATTERN_BAR_EMPTYFULL = 0x42,
			PATTERN_BAR_EMPTYHALF = 0x42
		#else
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x42,
			PATTERN_BAR_EMPTYHALF = 0x24
		#endif
		};

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		// по горизонтали 22 знакоместа (x=0..21)
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0,	0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },	// TX/RX
			{	3,	0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// VOX/TUN
			{	7,	0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// PRE/ATT/___
			{	11, 0,	display2_lockstate4, REDRM_MODE, REDRSUBSET(DPAGE0), },	// LOCK
			{	19, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// 3.1

			{	0,	1,	display2_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	19, 2,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/..
			//
			{	2,	4,	display_vfomode5,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	8,	4,	display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },
			{	19, 4,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0,	5,	display_hplp2,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// HP/LP
			{	3,	5,	display2_voltlevelV5,REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter with "V"
		#if WITHANTSELECT
			{	9,	5,	display2_ant5,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// ANTENNA
		#endif /* WITHANTSELECT */
			{	9,	5,	display_time5,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME
		#if WITHTX && WITHAUTOTUNER
			{	15, 5,	display2_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// ATU
			{	19, 5,	display2_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// BYP
		#endif /* WITHTX && WITHAUTOTUNER */

			{	0,	6,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
			{	0,	7,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// Отображение оцифровки шкалы S-метра
		#if WITHMENU
			{	0,	0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
			{	0,	1,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
			{	4,	1,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
			{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
		#endif /* WITHMENU */
		};
	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 1;
			p->ystep = 1;	// количество ячеек разметки на одну строку меню
			p->reverse = 0;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#else /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */

		enum
		{
			BDTH_ALLRX = 17,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 6,	// ширина индикатора плюсов
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 8,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 8,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
			PATTERN_BAR_EMPTYHALF = 0x00	//0x00
		};
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3, 0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7, 0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	12, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	16, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	19, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0, 2,	display2_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },	// x=1 then !WIDEFREQ
			{	19, 3,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 5,	display_voltlevel4, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter without "V"
			{	0, 6,	display2_currlevelA6, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// amphermeter with "A"
			{	13, 5,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	16, 5,	display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },	// x=9 then !WIDEFREQ
			{	19, 5,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 6,	display2_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	4, 6,	display2_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },

	#if WITHIF4DSP
		#if WITHUSEAUDIOREC
			{	19, 7,	display2_rec3,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// Отображение режима записи аудио фрагмента
		#endif /* WITHUSEAUDIOREC */
	#else /* WITHIF4DSP */
			{	19, 7,	display2_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
	#endif /* WITHIF4DSP */
	#if WITHUSEDUALWATCH
			{	19, 6,	display2_mainsub3, REDRM_BARS, REDRSUBSET(DPAGE0), },	// main/sub RX
	#endif /* WITHUSEDUALWATCH */

			{	0, 7,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER

		#if WITHMENU
			{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
			{	0, 1,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
			{	4, 1,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
			{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
		#endif /* WITHMENU */
		};
	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 1;
			p->ystep = 1;	// количество ячеек разметки на одну строку меню
			p->reverse = 0;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#endif /* DSTYLE_UR3LMZMOD */

#elif DSTYLE_G_X160_Y128
	// TFT-1.8, LCDMODE_ST7735 for example
	// CHAR_W=8, CHAR_H=8, SMALLCHARH=16
	#define CHAR_W	8
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP
		// используется в послдних версиях sw2013 mini - CTLSTYLE_SW2012CN и CTLSTYLE_SW2012CN5

		enum
		{
			BDTH_ALLRX = 20,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 11,	// ширина индикатора плюсов
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 10,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		#define SMETERMAP "1 3 5 7 9 + 20 40 60"	//
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			//                "01234567890123456789"
			#define SWRPWRMAP "1 | 2 | 3 0%  | 100%"
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		#else /* WITHSHOWSWRPWR */
			#define POWERMAP  "0 10 20 40 60 80 100"
			#define SWRMAP    "1    |    2    |   3"	// 
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		#endif /* WITHSHOWSWRPWR */
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
		#if 1
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x81,
			PATTERN_BAR_EMPTYHALF = 0x81
		#else
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x42,
			PATTERN_BAR_EMPTYHALF = 0x24
		#endif
		};

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3, 0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7, 0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	12, 0,	display2_lockstate4, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	17, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			//{	0, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			//{	0, 0,	display2_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0,	3,	display2_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	17, 5,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0,	8,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	7,	8,	display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },
			{	17, 8,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0,	11,	display_time5,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME
			//{	0,	11,	display_time8,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME
			{	0,	11,	display2_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	4,	11,	display2_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	15, 11,	display2_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter with "V"

			{	0, 13,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
			{	0, 14,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// Отображение оцифровки шкалы S-метра

		#if WITHMENU
			{	4, 0,	display2_menu_group,	REDRM_MLBL, REDRSUBSET_MENU, },	// название группы
			{	0, 2,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
			{	4, 2,	display2_menu_lblng,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
			{	0, 4,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
			{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
		#endif /* WITHMENU */

		};
#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		p->multilinemenu_max_rows = 1;
		p->ystep = 1;	// количество ячеек разметки на одну строку меню
		p->reverse = 0;
		p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

	#else /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */

		enum
		{
			BDTH_ALLRX = 15,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 5,	// ширина индикатора плюсов
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 7,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 7,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x81,
			PATTERN_BAR_EMPTYHALF = 0x24
		};
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0,	0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3,	0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7,	0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	12, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	17, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	16, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0,	4,	display2_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	17, 6,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0,	9,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	7,	9,	display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },
			{	17, 9,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0,	12,	display_time5,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME
			//{	0,	12,	display_time8,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME
			{	0,	12,	display2_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	4,	12,	display2_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	8,	12,	display2_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter with "V"

			{	16, 13,	display_agc4,		REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0, 14,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
		#if WITHMENU
			{	4, 0,	display2_menu_group,	REDRM_MLBL, REDRSUBSET_MENU, },	// название группы
			{	0, 2,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
			{	4, 2,	display2_menu_lblng,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
			{	0, 4,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
			{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
		#endif /* WITHMENU */
		};
#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		p->multilinemenu_max_rows = 1;
		p->ystep = 1;	// количество ячеек разметки на одну строку меню
		p->reverse = 0;
		p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

	#endif /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */

#elif DSTYLE_G_X176_Y132
	// Siemens S65 LS020, Siemens S65 LPH88
	#define CHAR_W	8
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP
		// SW20XXX - только одно положение аттенюатора и УВЧ - "по кругу"
		enum
		{
			BDTH_ALLRX = 20,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 11,	// ширина индикатора плюсов
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 10,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		#define SMETERMAP     "1 3 5 7 9 + 20 40 60"
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			//                "01234567890123456789"
			#define SWRPWRMAP "1 | 2 | 3 0%  | 100%"
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		#else /* WITHSHOWSWRPWR */
			#define SWRMAP    "1    |    2    |   3"	// 
			#define POWERMAP  "0 10 20 40 60 80 100"
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		#endif /* WITHSHOWSWRPWR */
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0x3c,
			//PATTERN_BAR_FULL = 0x7e,
			//PATTERN_BAR_HALF = 0x7e /* 0x3c */,
			PATTERN_BAR_EMPTYFULL = 0x00,
			PATTERN_BAR_EMPTYHALF = 0x00
		};

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			//{	0, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3, 0,	display_hplp2,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	6, 0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	10, 0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	14, 0,	display2_lockstate4, REDRM_MODE, REDRSUBSET(DPAGE0), },
			//{	0, 0,	display2_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	19, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	0, 2,	display2_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter with "V"
			{	6, 2,	display2_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	10, 2,	display2_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	17, 3,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			{	0,	5,	display2_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	0, 10,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	17, 10,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			{	5, 10,	display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },
			{	0, 13,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },		// S-METER, SWR-METER, POWER-METER
			{	1, 14,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// Отображение оцифровки шкалы S-метра
		#if WITHMENU
			{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
			{	0, 2,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
			{	4, 2,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
			{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
		#endif /* WITHMENU */
		};
#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		p->multilinemenu_max_rows = 1;
		p->ystep = 1;	// количество ячеек разметки на одну строку меню
		p->reverse = 0;
		p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

	#elif DSTYLE_UR3LMZMOD
		// Управление УВЧ и двухкаскадным аттенюатором
		// отображение АРУ
		// Для CTLSTYLE_RA4YBO_V1

		enum
		{
			BDTH_ALLRX = 20,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 11,	// ширина индикатора плюсов
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 10,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		#define SMETERMAP     "1 3 5 7 9 + 20 40 60"
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			//                "01234567890123456789"
			#define SWRPWRMAP "1 | 2 | 3 0%  | 100%"
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		#else /* WITHSHOWSWRPWR */
			#define SWRMAP    "1    |    2    |   3"	// 
			#define POWERMAP  "0 10 20 40 60 80 100"
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		#endif /* WITHSHOWSWRPWR */
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0x3c,
			//PATTERN_BAR_FULL = 0x7e,
			//PATTERN_BAR_HALF = 0x7e /* 0x3c */,
			PATTERN_BAR_EMPTYFULL = 0x00,
			PATTERN_BAR_EMPTYHALF = 0x00
		};

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3, 0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7, 0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },		// 4 chars - 12dB, 18dB
			{	12, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	16, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	18, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0, 2,	display2_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter with "V"
			{	6, 2,	display2_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	10, 2,	display2_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	18, 2,	display_agc4,		REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0,	6,	display2_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	18, 4,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 10,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	4, 10,	display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },
			{	16, 10,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 13,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },		// S-METER, SWR-METER, POWER-METER
			{	1, 14,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// Отображение оцифровки шкалы S-метра
		#if WITHMENU
			{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
			{	0, 2,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
			{	4, 2,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
			{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
		#endif /* WITHMENU */
		};
	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 1;
			p->ystep = 1;	// количество ячеек разметки на одну строку меню
			p->reverse = 0;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#else /* DSTYLE_UR3LMZMOD */

		enum
		{
			BDTH_ALLRX = 17,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 6,	// ширина индикатора плюсов
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 8,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 8,
			BDTH_SPACEPWR = 0,
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX,
		#endif /* WITHSHOWSWRPWR */
			BDCV_ALLRX = ROWS2GRID(1),		// количество ячееек, отведенное под S-метр, панораму, иные отображения
			BDCV_SPMRX = BDCV_ALLRX,	// вертикальный размер спектра в ячейках		};
			BDCV_WFLRX = BDCV_ALLRX,	// вертикальный размер водопада в ячейках		};
			BDCO_SPMRX = 0,	// смещение спектра по вертикали в ячейках от начала общего поля
			BDCO_WFLRX = 0	// смещение водопада по вертикали в ячейках от начала общего поля
		};
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
			PATTERN_BAR_EMPTYHALF = 0x00	//0x00
		};
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0,	0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3,	0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7,	0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	12, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	16, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	19, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	0,	2,	display2_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter with "V"
			{	6,	2,	display2_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	10, 2,	display2_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	0,	4,	display2_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	19, 4,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			{	0,	9,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	8,	9,	display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },
			{	19, 9,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			{	0,	14,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
			{	18, 14,	display2_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
		#if WITHMENU
			{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
			{	0, 2,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
			{	4, 2,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
			{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
		#endif /* WITHMENU */
		};
	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 1;
			p->ystep = 1;	// количество ячеек разметки на одну строку меню
			p->reverse = 0;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#endif /* DSTYLE_UR3LMZMOD */

#elif DSTYLE_G_X220_Y176
	// Индикатор 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 с контроллером ILI9225С
	// x= 27 characters
	#define CHAR_W	8
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP

		enum
		{
			BDTH_ALLRX = 27,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 14,	// ширина индикатора плюсов
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 16,
			BDTH_SPACEPWR = 1
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		// 27 horisontal places
		//                    "012345678901234567890123456"
			#define SMETERMAP "1  3  5  7  9 + 20  40  60 "
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
			#define SWRPWRMAP "1 | 2 | 3 0%     |    100% "
		#else
			#define POWERMAP  "0  10  20  40  60  80  100"
			#define SWRMAP    "1    -    2    -    3    -"	// 
			#define SWRMAX	(SWRMIN * 36 / 10)	// 3.6 - значение на полной шкале
		#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x81,	//0x00
			PATTERN_BAR_EMPTYHALF = 0x81	//0x00
		};

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3, 0,	display_hplp2,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			//{	6, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	10, 0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			//{	0, 0,	display2_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	20, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	24, 0,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			
			{	0, 5,	display_freqXbig_a, REDRM_FREQ, REDRSUBSET(DPAGE0), },

			{	0, 11,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	11, 11, display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },
			{	24, 11,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 14,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
			{	0, 17,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// Отображение оцифровки шкалы S-метра

			{	0, 20,	display2_lockstate4, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	9, 20,	display_voxtune4,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	14, 20,	display_time5,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME HH:MM
			{	14, 20,	display2_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	18, 20,	display2_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	22, 20,	display2_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter
		#if WITHMENU
			{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
			{	0, 2,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
			{	4, 2,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
			{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
		#endif /* WITHMENU */
		};
	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 1;
			p->ystep = 1;	// количество ячеек разметки на одну строку меню
			p->reverse = 0;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#elif DSTYLE_UR3LMZMOD
		// KEYBSTYLE_SW2013SF_US2IT
		enum
		{
			BDTH_ALLRX = 27,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 14,	// ширина индикатора плюсов
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 16,
			BDTH_SPACEPWR = 1
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		// 27 horisontal places
		//                    "012345678901234567890123456"
			#define SMETERMAP "1  3  5  7  9 + 20  40  60 "
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
			#define SWRPWRMAP "1 | 2 | 3 0%     |    100% "
		#else
			#define POWERMAP  "0  10  20  40  60  80  100"
			#define SWRMAP    "1    -    2    -    3    -"	// 
			#define SWRMAX	(SWRMIN * 36 / 10)	// 3.6 - значение на полной шкале
		#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x81,	//0x00
			PATTERN_BAR_EMPTYHALF = 0x81	//0x00
		};

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3, 0,	display_hplp2,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	6, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	10, 0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			//{	0, 0,	display2_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	20, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	24, 0,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			
			{	0, 5,	display_freqXbig_a, REDRM_FREQ, REDRSUBSET(DPAGE0), },

			{	0, 11,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	11, 11, display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },
			{	24, 11,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 14,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
			{	0, 16,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// Отображение оцифровки шкалы S-метра

			{	0, 18,	display2_datetime12,	REDRM_BARS, REDRSUBSET(DPAGE0), },	// DATE&TIME Jan-01 13:40
			//{	0, 18,	display_time5,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME HH:MM
			{	0, 20,	display2_lockstate4, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	9, 20,	display_voxtune4,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	14, 20,	display2_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	18, 20,	display2_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	22, 20,	display2_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter
		#if WITHMENU
			{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
			{	0, 2,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
			{	4, 2,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
			{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
		#endif /* WITHMENU */
		};
	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 1;
			p->ystep = 1;	// количество ячеек разметки на одну строку меню
			p->reverse = 0;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#else /* DSTYLE_UR3LMZMOD */

		enum
		{
			BDTH_ALLRX = 17,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 6,	// ширина индикатора плюсов
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 8,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 8,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
			PATTERN_BAR_EMPTYHALF = 0x00	//0x00
		};
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	4, 0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	10, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	19, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	15, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	23, 0,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 4,	display2_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	19, 10,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			{	0, 10,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	8, 10,	display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },
			{	19, 13,	display2_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	0, 14,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER

		#if defined (RTC1_TYPE)
			{	0, 20,	display_time8,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME
		#endif /* defined (RTC1_TYPE) */
			{	10, 20,	display_voxtune4,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	15, 20,	display2_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	19, 20,	display2_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	23, 20,	display_voltlevel4, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter without "V"
		#if WITHMENU
			{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
			{	0, 2,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
			{	4, 2,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
			{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
		#endif /* WITHMENU */
		};
	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 1;
			p->ystep = 1;	// количество ячеек разметки на одну строку меню
			p->reverse = 0;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#endif /* DSTYLE_UR3LMZMOD */

#elif DSTYLE_G_X240_Y128
	// WO240128A
	#define CHAR_W	8
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP
		// x=30, y=16

		enum
		{
			BDTH_ALLRX = 30,	// ширина зоны для отображение полосы на индикаторе
			BDTH_LEFTRX = 15,	// ширина индикатора баллов
			BDTH_RIGHTRX = BDTH_ALLRX - BDTH_LEFTRX,	// ширина индикатора плюсов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 20,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//					  "012345678901234567890123456789"
		#define SMETERMAP 	  "S 1  3  5  7  9  +20  +40  +60"	//
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			#define SWRPWRMAP "1 | 2 | 3 0%      50%     100%" 
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		#else
			#error Not designed for work DSTYLE_UR3LMZMOD without WITHSHOWSWRPWR
			//#define POWERMAP  " 0 10 20 40 60 80 100"
			//#define SWRMAP    "1    |    2    |   3 "	// 
			//#define SWRMAX	(SWRMIN * 31 / 10)	// 3.1 - значение на полной шкале
		#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x81,
			PATTERN_BAR_EMPTYHALF = 0x81
		};
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
		/* ---------------------------------- */
			{	0,	0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3,	0,	display2_ant5,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	9,	0,	display_hplp2,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	16, 0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// attenuator state
			{	21, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	25,	0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	27, 0,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	2,	display_freqXbig_a, REDRM_FREQ, REDRSUBSET(DPAGE0), },	// fullwidth = 8 constantly
		/* ---------------------------------- */
			//{	0,	8,	display2_mainsub3, REDRM_MODE, REDRSUBSET(DPAGE0), },	// main/sub RX
			{	4,	8,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPL
			{	12, 8,	display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },
			{	27, 8,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	11,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
			{	0,	12,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// Отображение оцифровки шкалы S-метра
			/* ---------------------------------- */
			{	0,	14,	display2_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
	#if WITHNOTCHONOFF || WITHNOTCHFREQ
			// see next {	4,	14,	display2_notch5,		REDRM_MODE, REDRSUBSET(DPAGE0), },
	#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ  */
	#if WITHAUTOTUNER
			{	4,	14,	display2_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	8,	14,	display2_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
	#endif /* WITHAUTOTUNER  */
		/* ---------------------------------- */
	#if WITHVOLTLEVEL
			{	12,	14,	display2_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter with "V"
	#endif /* WITHVOLTLEVEL  */
	#if defined (RTC1_TYPE)
			//{	18,	14,	display_time8,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME
			{	18,	14,	display2_datetime12,	REDRM_BARS, REDRSUBSET(DPAGE0), },	// DATE&TIME Jan-01 13:40
	#endif /* defined (RTC1_TYPE) */
		#if WITHNOTCHONOFF || WITHNOTCHFREQ
			{	27, 14,	display_notch3, REDRM_MODE, REDRSUBSET(DPAGE0), },	// 3.7
		#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
		/* ---------------------------------- */
	#if WITHMENU
			{	1,	0,	display2_multilinemenu_block_groups,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
			{	LABELW + 2,	0,	display2_multilinemenu_block_params,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
			{	LABELW*2 + 3,	0,	display2_multilinemenu_block_vals,	REDRM_MVAL, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
		#if WITHVOLTLEVEL && WITHCURRLEVEL
			{	0,	9,	display2_voltlevelV5, REDRM_VOLT, REDRSUBSET_MENU, },	// voltmeter with "V"
			{	6,	9,	display2_currlevelA6, REDRM_VOLT, REDRSUBSET_MENU, },	// amphermeter with "A"
		#endif /* WITHVOLTLEVEL && WITHCURRLEVEL */
	#endif /* WITHMENU */
		};
	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 8;
			p->ystep = 2;	// количество ячеек разметки на одну строку меню
			p->reverse = 1;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#elif DSTYLE_UR3LMZMOD
		// Version with specreum display
		// x=30, y=16
		enum
		{
			BDCV_ALLRX = 7,			// количество ячееек, отведенное под S-метр, панораму, иные отображения
			BDCV_SPMRX = BDCV_ALLRX,	// вертикальный размер спектра в ячейках		};
			BDCV_WFLRX = BDCV_ALLRX,	// вертикальный размер водопада в ячейках		};
			BDCO_SPMRX = 0,	// смещение спектра по вертикали в ячейках от начала общего поля
			BDCO_WFLRX = 0,	// смещение водопада по вертикали в ячейках от начала общего поля

			BDTH_ALLRX = 30,	// ширина зоны для отображение полосы на индикаторе
			BDTH_LEFTRX = 15,	// ширина индикатора баллов
			BDTH_RIGHTRX = BDTH_ALLRX - BDTH_LEFTRX,	// ширина индикатора плюсов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 20,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//					  "012345678901234567890123456789"
		#define SMETERMAP 	  "S 1  3  5  7  9  +20  +40  +60"	//
		//#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			#define SWRPWRMAP "1 | 2 | 3 0%      50%     100%" 
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		//#else
		//	#error Not designed for work DSTYLE_UR3LMZMOD without WITHSHOWSWRPWR
			//#define POWERMAP  " 0 10 20 40 60 80 100"
			//#define SWRMAP    "1    |    2    |   3 "	// 
			//#define SWRMAX	(SWRMIN * 31 / 10)	// 3.1 - значение на полной шкале
		//#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x81,
			PATTERN_BAR_EMPTYHALF = 0x81
		};
		//#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DPAGE1,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		enum
		{
			PG0 = REDRSUBSET(DPAGE0),
			PG1 = REDRSUBSET(DPAGE1),
			PGALL = PG0 | PG1 | REDRSUBSET_MENU,
			PGLATCH = PGALL | REDRSUBSET_SLEEP,
			PGINI = REDRSUBSET_INIT,
			PGunused
		};
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
		/* ---------------------------------- */
			{	0,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
			{	3,	0,	display2_ant5,		REDRM_MODE, PGALL, },
			{	9,	0,	display_ovf3,		REDRM_BARS, PGALL, },	// ovf
			{	14,	0,	display_pre3,		REDRM_MODE, PGALL, },	// pre
			{	18, 0,	display2_att4,		REDRM_MODE, PGALL, },	// attenuator state
			{	22,	0,	display_lockstate1, REDRM_MODE, PGALL, },
			{	23, 0,	display2_rxbw3,		REDRM_MODE, PGALL, },
			{	27, 0,	display2_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	2,	display_freqXbig_a, REDRM_FREQ, PGALL, },	// fullwidth = 8 constantly
		#if ! WITHDSPEXTDDC
			{	27, 2,	display2_agc3,		REDRM_MODE, PGALL, },
		#endif /* ! WITHDSPEXTDDC */
			{	27, 4,	display2_voxtune3,	REDRM_MODE, PGALL, },
		/* ---------------------------------- */
		#if WITHUSEAUDIOREC
			{	0,	7,	display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента
		#endif /* WITHUSEAUDIOREC */
			{	4,	7,	display2_mainsub3, REDRM_MODE, PGALL, },	// main/sub RX
			{	8,	7,	display2_vfomode3,	REDRM_MODE, PGALL, },	// SPL
			{	16, 7,	display2_freqX_b,	REDRM_FRQB, PGALL, },
			{	27, 7,	display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	9,	display2_bars,		REDRM_BARS, PG0, },	// S-METER, SWR-METER, POWER-METER
			{	0,	10,	display2_legend,	REDRM_MODE, PG0, },	// Отображение оцифровки шкалы S-метра
			/* ---------------------------------- */
			{	0,	9,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
			{	0,	9,	display2_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
			{	0,	9,	display2_spectrum,	REDRM_BARS, PG1, },// подготовка изображения спектра
			{	0,	9,	display2_colorbuff,	REDRM_BARS,	PG1, },// Отображение водопада и/или спектра
			/* ---------------------------------- */
			{	0,	14,	display_time5,		REDRM_BARS, PG0, },	// TIME
		#if WITHVOLTLEVEL
			{	6,	14,	display2_voltlevelV5, REDRM_VOLT, PG0, },	// voltmeter with "V"
		#endif /* WITHVOLTLEVEL  */
		#if WITHCURRLEVEL || WITHCURRLEVEL2
			{	11, 14,	display2_currlevelA6, REDRM_VOLT, PG0, },	// amphermeter with "A"
		#endif /*  WITHCURRLEVEL || WITHCURRLEVEL2 */
		#if WITHAMHIGHKBDADJ
			{	6, 14,	display_amfmhighcut4,REDRM_MODE, PG0, },	// 3.70
		#endif /* WITHAMHIGHKBDADJ */
			{	19, 14, display_siglevel7, 	REDRM_BARS, PG0, },		// signal level dBm
			//{	18, 14,	display_samfreqdelta8, REDRM_BARS, PG0, },	/* Получить информацию об ошибке настройки в режиме SAM */
		#if WITHNOTCHONOFF || WITHNOTCHFREQ
			{	27, 14,	display_notch3, REDRM_MODE, PG0, },	// 3.7
		#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
		/* ---------------------------------- */
	#if WITHMENU
			{	1,	9,	display2_multilinemenu_block_groups,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
			{	LABELW + 2,	9,	display2_multilinemenu_block_params,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
			{	LABELW*2 + 3,	9,	display2_multilinemenu_block_vals,	REDRM_MVAL, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
		#if WITHVOLTLEVEL && WITHCURRLEVEL
			//{	0,	9,	display2_voltlevelV5, REDRM_VOLT, REDRSUBSET_MENU, },	// voltmeter with "V"
			//{	6,	9,	display2_currlevelA6, REDRM_VOLT, REDRSUBSET_MENU, },	// amphermeter with "A"
		#endif /* WITHVOLTLEVEL && WITHCURRLEVEL */
	#endif /* WITHMENU */
		};
	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 3;
			p->ystep = 2;	// количество ячеек разметки на одну строку меню
			p->reverse = 1;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#else /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */
		// RA1AGO version
		// x=30, y=16

		enum
		{
			BDCV_ALLRX = 7,			// количество ячееек, отведенное под S-метр, панораму, иные отображения
			BDCV_SPMRX = BDCV_ALLRX,	// вертикальный размер спектра в ячейках		};
			BDCV_WFLRX = BDCV_ALLRX,	// вертикальный размер водопада в ячейках		};
			BDCO_SPMRX = 0,	// смещение спектра по вертикали в ячейках от начала общего поля
			BDCO_WFLRX = 0,	// смещение водопада по вертикали в ячейках от начала общего поля
			BDTH_ALLRX = 30,	// ширина зоны для отображение полосы на индикаторе
			BDTH_LEFTRX = 15,	// ширина индикатора баллов
			BDTH_RIGHTRX = BDTH_ALLRX - BDTH_LEFTRX,	// ширина индикатора плюсов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 20,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//					  "012345678901234567890123456789"
		#define SMETERMAP 	  "S 1  3  5  7  9  +20  +40  +60"	//
		//#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			#define SWRPWRMAP "1 | 2 | 3 0%      50%     100%" 
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		//#else
		//	#error Not designed for work DSTYLE_UR3LMZMOD without WITHSHOWSWRPWR
			//#define POWERMAP  " 0 10 20 40 60 80 100"
			//#define SWRMAP    "1    |    2    |   3 "	// 
			//#define SWRMAX	(SWRMIN * 31 / 10)	// 3.1 - значение на полной шкале
		//#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x81,
			PATTERN_BAR_EMPTYHALF = 0x81
		};
		//#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
		#if WITHDSPEXTDDC
			DPAGE1,					// Страница, в которой отображаются основные (или все)
		#endif /* WITHDSPEXTDDC */
			DISPLC_MODCOUNT
		};
		enum
		{
			PG0 = REDRSUBSET(DPAGE0),
		#if WITHDSPEXTDDC
			PG1 = REDRSUBSET(DPAGE1),
			PGALL = PG0 | PG1 | REDRSUBSET_MENU,
		#else /* WITHDSPEXTDDC */
			PGALL = PG0 | REDRSUBSET_MENU,
		#endif /* WITHDSPEXTDDC */
			PGLATCH = PGALL | REDRSUBSET_SLEEP,
			PGINI = REDRSUBSET_INIT,
			PGunused
		};
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
		/* ---------------------------------- */
			{	0,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
			{	3,	0,	display2_ant5,		REDRM_MODE, PGALL, },
		#if WITHDSPEXTDDC
			{	9,	0,	display_ovf3,		REDRM_BARS, PGALL, },	// ovf
			{	14,	0,	display_pre3,		REDRM_MODE, PGALL, },	// pre
			{	18, 0,	display2_att4,		REDRM_MODE, PGALL, },	// attenuator state
			{	22,	0,	display_lockstate1, REDRM_MODE, PGALL, },
		#else /* WITHDSPEXTDDC */
			{	9,	0,	display_pre3,		REDRM_MODE, PGALL, },	// pre
			{	13, 0,	display2_att4,		REDRM_MODE, PGALL, },	// attenuator state
			{	18,	0,	display2_lockstate4, REDRM_MODE, PGALL, },
		#endif /* WITHDSPEXTDDC */
			{	23, 0,	display2_rxbw3,		REDRM_MODE, PGALL, },
			{	27, 0,	display2_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	2,	display_freqXbig_a, REDRM_FREQ, PGALL, },	// fullwidth = 8 constantly
		#if ! WITHDSPEXTDDC
			{	27, 2,	display2_agc3,		REDRM_MODE, PGALL, },
		#endif /* ! WITHDSPEXTDDC */
			{	27, 4,	display2_voxtune3,	REDRM_MODE, PGALL, },
		/* ---------------------------------- */
		#if WITHUSEAUDIOREC
			{	0,	7,	display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента
		#endif /* WITHUSEAUDIOREC */
			{	4,	7,	display2_mainsub3, REDRM_MODE, PGALL, },	// main/sub RX
			{	8,	7,	display2_vfomode3,	REDRM_MODE, PGALL, },	// SPL
			{	16, 7,	display2_freqX_b,	REDRM_FRQB, PGALL, },
			{	27, 7,	display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	9,	display2_bars,		REDRM_BARS, PG0 | REDRSUBSET_MENU, },	// S-METER, SWR-METER, POWER-METER
			{	0,	10,	display2_legend,	REDRM_MODE, PG0 | REDRSUBSET_MENU, },	// Отображение оцифровки шкалы S-метра
			/* ---------------------------------- */
		#if WITHDSPEXTDDC

			{	0,	9,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
			{	0,	9,	display2_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
			{	0,	9,	display2_spectrum,	REDRM_BARS, PG1, },// подготовка изображения спектра
			{	0,	9,	display2_colorbuff,	REDRM_BARS,	PG1, },// Отображение водопада и/или спектра
		#else /* WITHDSPEXTDDC */
			{	27, 12,	display2_atu3,		REDRM_MODE, PGALL, },	// ATU
			{	27, 14,	display2_byp3,		REDRM_MODE, PGALL, },	// BYP
		#endif /* WITHDSPEXTDDC */
			/* ---------------------------------- */
			{	0,	14,	display_time5,		REDRM_BARS, PG0, },	// TIME
		#if WITHVOLTLEVEL
			{	6,	14,	display2_voltlevelV5, REDRM_VOLT, PG0, },	// voltmeter with "V"
		#endif /* WITHVOLTLEVEL  */
		#if WITHCURRLEVEL || WITHCURRLEVEL2
			{	11, 14,	display2_currlevelA6, REDRM_VOLT, PG0, },	// amphermeter with "A"
		#endif /*  WITHCURRLEVEL || WITHCURRLEVEL2 */
		#if WITHAMHIGHKBDADJ
			{	6, 14,	display_amfmhighcut4,REDRM_MODE, PG0, },	// 3.70
		#endif /* WITHAMHIGHKBDADJ */
			{	18, 14,	display_samfreqdelta8, REDRM_BARS, PG0 | REDRSUBSET_MENU, },	/* Получить информацию об ошибке настройки в режиме SAM */
		#if WITHNOTCHONOFF || WITHNOTCHFREQ
			{	27, 14,	display_notch3, REDRM_MODE, PG0, },	// 3.7
		#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
		/* ---------------------------------- */
	#if WITHMENU
			{	0,	12,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
			{	0,	14,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
			{	4,	14,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
			//{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
		#if WITHVOLTLEVEL && (WITHCURRLEVEL || WITHCURRLEVEL2)
			//{	0,	14,	display2_voltlevelV5, REDRM_VOLT, REDRSUBSET_MENU, },	// voltmeter with "V"
			//{	6,	14,	display2_currlevelA6, REDRM_VOLT, REDRSUBSET_MENU, },	// amphermeter with "A"
		#endif /* WITHVOLTLEVEL && (WITHCURRLEVEL || WITHCURRLEVEL2) */
	#endif /* WITHMENU */
		};

	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 1;
			p->ystep = 2;	// количество ячеек разметки на одну строку меню
			p->reverse = 0;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#endif /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */

#elif DSTYLE_G_X320_Y240 && WITHSPECTRUMWF
	// DSP version - with spectrum scope
	// TFT панель 320 * 240 ADI_3.2_AM-240320D4TOQW-T00H(R)
	// 320*240 SF-TC240T-9370-T с контроллером ILI9341
	// 32*15 знакомест 10*16
	#define CHAR_W	10
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */

	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		//					"012345678901234567890123"
		#define SWRPWRMAP	"1   2   3   4  0% | 100%"
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
	#else
		//					"012345678901234567890123"
		#define POWERMAP	"0    25    50   75   100"
		#define SWRMAP		"1   |   2  |   3   |   4"	//
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
	#endif
	//						"012345678901234567890123"
	#define SMETERMAP		"1  3  5  7  9 +20 +40 60"

	enum
	{
		BDCV_ALLRX = 10,	// общая высота, отведенная под панораму, водопад и иные отображения

		BDCO_SPMRX = 0,	// смещение спектра по вертикали в ячейках от начала общего поля
		BDCV_SPMRX = 5,	// вертикальный размер спектра в ячейках

		BDCO_WFLRX = 4,	// смещение водопада по вертикали в ячейках от начала общего поля
		BDCV_WFLRX = 5,	// вертикальный размер водопада в ячейках

		BDTH_ALLRX = 24,	// ширина зоны для отображение полосы на индикаторе
		BDTH_RIGHTRX = 11,	// ширина индикатора плюсов
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
		BDTH_SPACERX = 0,
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 13,
		BDTH_SPACESWR = 2,
		BDTH_ALLPWR = 9,
		BDTH_SPACEPWR = 0
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	enum
	{
		PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
		PATTERN_BAR_FULL = 0x7e,
		PATTERN_BAR_HALF = 0x3c,
		PATTERN_BAR_EMPTYFULL = 0x00,
		PATTERN_BAR_EMPTYHALF = 0x00
	};
	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

	enum
	{
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DISPLC_MODCOUNT
	};
	enum {
		PG0 = REDRSUBSET(DPAGE0),
		PGALL = PG0 | REDRSUBSET_MENU,
		PGNOMEMU = PG0,
		PGLATCH = PGALL | REDRSUBSET_SLEEP,	// страницы, на которых возможно отображение водопада или панорамы.
		PGINI = REDRSUBSET_INIT,
		PGunused
	};
	#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
	#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0,	0,	display2_clearbg, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
		{	0,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
		{	3,	0,	display2_voxtune3,	REDRM_MODE, PGALL, },
		{	7,	0,	display2_att4,		REDRM_MODE, PGALL, },
		{	12, 0,	display_pre3,		REDRM_MODE, PGALL, },
	#if WITHDSPEXTDDC
		{	16, 0,	display_ovf3,		REDRM_BARS, PGALL, },	// ovf/pre
	#endif /* WITHDSPEXTDDC */
		{	20, 0,	display2_lockstate4, REDRM_MODE, PGALL, },
		{	25, 0,	display2_agc3,		REDRM_MODE, PGALL, },
		{	29, 0,	display2_rxbw3,		REDRM_MODE, PGALL, },

		{	0,	3,	display_freqXbig_a, REDRM_FREQ, PGALL, },
		{	25, 3,	display2_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		{	29, 3,	display2_nr3,		REDRM_MODE,	PGALL, },	// NR
		//---
		{	0,	9,	display_vfomode5,	REDRM_MODE, PGALL, },	// SPLIT
		{	6,	9,	display2_freqX_b,	REDRM_FRQB, PGALL, },
	#if WITHUSEDUALWATCH
		{	29, 9,	display2_mainsub3,	REDRM_MODE, PGNOMEMU, },	// main/sub RX
	#endif /* WITHUSEDUALWATCH */
		{	25, 9,	display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		//---
		{	0,	12,	display2_legend,	REDRM_MODE, PG0, },	// Отображение оцифровки шкалы S-метра, PWR & SWR-метра
		{	0,	15,	display2_bars,		REDRM_BARS, PG0, },	// S-METER, SWR-METER, POWER-METER
		{	27, 15,	display_smeter5,	REDRM_BARS, PG0, },	// signal level

		{	0,	18,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
		{	0,	18,	display2_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
		{	0,	18,	display2_spectrum,	REDRM_BARS, PG0, },// подготовка изображения спектра
		{	0,	18,	display2_waterfall,	REDRM_BARS, PG0, },// подготовка изображения водопада
		{	0,	18,	display2_colorbuff,	REDRM_BARS,	PG0, },// Отображение водопада и/или спектра

		//---
		//{	22, 25,	display_samfreqdelta8, REDRM_BARS, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */

	#if WITHVOLTLEVEL
		{	0, 28,	display2_voltlevelV5, REDRM_VOLT, PGALL, },	// voltmeter with "V"
	#endif /* WITHVOLTLEVEL */
	#if WITHCURRLEVEL || WITHCURRLEVEL2
		{	6, 28,	display2_currlevelA6, REDRM_VOLT, PGALL, },	// amphermeter with "A"
	#endif /* WITHCURRLEVEL || WITHCURRLEVEL2 */
	#if defined (RTC1_TYPE)
		{	13, 28,	display_time8,		REDRM_BARS, PGALL, },	// TIME
	#endif /* defined (RTC1_TYPE) */
	#if WITHUSEAUDIOREC
		{	25, 28,	display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента
	#endif /* WITHUSEAUDIOREC */
	#if WITHMENU
		{	1 + LABELW * 0 + 0,	18,	display2_multilinemenu_block_groups,	REDRM_MLBL, 	REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
		{	1 + LABELW * 1 + 1,	18,	display2_multilinemenu_block_params,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
		{	1 + LABELW * 2 + 2,	18,	display2_multilinemenu_block_vals,	REDRM_MVAL, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
	#endif /* WITHMENU */
		{	0,	0,	display2_nextfb, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
	};

	/* получить координаты окна с панорамой и/или водопадом. */
	void display2_getpipparams(pipparams_t * p)
	{
		p->x = 0; //GRID2X(0);	// позиция верхнего левого угла в пикселях
		p->y = GRID2Y(18);	// позиция верхнего левого угла в пикселях
		p->w = DIM_X; //GRID2X(CHARS2GRID(BDTH_ALLRX));	// размер по горизонтали в пикселях
		p->h = GRID2Y(BDCV_ALLRX);				// размер по вертикали в пикселях
	}

	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 5;
			p->ystep = 2;	// количество ячеек разметки на одну строку меню
			p->reverse = 1;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

#elif DSTYLE_G_X320_Y240
	// No DSP version - no spectrum scope
	// TFT панель 320 * 240 ADI_3.2_AM-240320D4TOQW-T00H(R)
	// 320*240 SF-TC240T-9370-T с контроллером ILI9341
	// 32*15 знакомест 10*16
	#define CHAR_W	10
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP
		// TFT панель 320 * 240
		enum
		{
			BDTH_ALLRX = 20,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 11,	// ширина индикатора плюсов
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 5,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 10,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		#define SMETERMAP "1 3 5 7 9 + 20 40 60"
		#define SWRPWRMAP "1 2 3  0%   |   100%" 
		#define POWERMAP  "0 10 20 40 60 80 100"
		#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x00,
			PATTERN_BAR_EMPTYHALF = 0x00
		};

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0,	0,	display2_clearbg, 	REDRM_MODE, REDRSUBSET(DPAGE0) | REDRSUBSET_MENU | REDRSUBSET_SLEEP, },
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3, 0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7, 0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	12, 0,	display2_preovf3,	REDRM_BARS, REDRSUBSET(DPAGE0), },
			{	16, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	19, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0, 8,	display_freqXbig_a, REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	19, 8,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 18,	display_vfomode5,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	5, 18,	display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },
			{	19, 18,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	1, 24,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
		#if defined (RTC1_TYPE)
			{	0, 28,	display_time8,		REDRM_BARS, REDRSUBSET_MENU | REDRSUBSET(DPAGE0), },	// TIME
		#endif /* defined (RTC1_TYPE) */
			{	18, 28,	display2_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
		#if WITHMENU
			{	1 + LABELW * 0 + 0,	18,	display2_multilinemenu_block_groups,	REDRM_MLBL, sREDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
			{	1 + LABELW * 1 + 1,	18,	display2_multilinemenu_block_params,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
			{	1 + LABELW * 2 + 2,	18,	display2_multilinemenu_block_vals,	REDRM_MVAL, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
		#endif /* WITHMENU */
			{	0,	0,	display2_nextfb, 	REDRM_MODE, REDRSUBSET(DPAGE0) | REDRSUBSET_MENU | REDRSUBSET_SLEEP, },
		};

	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 5;
			p->ystep = 3;	// количество ячеек разметки на одну строку меню
			p->reverse = 1;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#else /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */
		// TFT панель 320 * 240
		// для Аиста
		enum
		{
			BDCV_ALLRX = 9,	// общая высота, отведенная под S-метр, панораму, водопад и иные отображения

			BDCO_SPMRX = 0,	// смещение спектра по вертикали в ячейках от начала общего поля
			BDCV_SPMRX = 4,	// вертикальный размер спектра в ячейках

			BDCO_WFLRX = 4,	// смещение водопада по вертикали в ячейках от начала общего поля
			BDCV_WFLRX = 5,	// вертикальный размер водопада в ячейках

			BDTH_ALLRX = 26,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 16,	// ширина индикатора плюсов
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 8,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 8,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x00,
			PATTERN_BAR_EMPTYHALF = 0x00
		};
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

		enum
		{
			DPAGE0,					// Страница, в которой отображаются основные (или все)
		#if WITHIF4DSP
			DPAGE1,					// Страница, в которой отображается спектр иводопад
		#endif /* WITHIF4DSP */
			DISPLC_MODCOUNT
		};
		enum {
			PG0 = REDRSUBSET(DPAGE0),
		#if WITHIF4DSP
			PG1 = REDRSUBSET(DPAGE1),
			PGALL = PG0 | PG1 | REDRSUBSET_MENU,
			PGNOMEMU = PG0 | PG1,
		#else /* WITHIF4DSP */
			PGALL = PG0 | REDRSUBSET_MENU,
			PGNOMEMU = PG0,
		#endif /* WITHIF4DSP */
			PGLATCH = PGALL | REDRSUBSET_SLEEP,	// страницы, на которых возможно отображение водопада или панорамы.
			PGINI = REDRSUBSET_INIT,
			PGunused
		};
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
			{	3,	0,	display2_voxtune3,	REDRM_MODE, PGALL, },
			{	7,	0,	display2_att4,		REDRM_MODE, PGALL, },
			{	12, 0,	display_pre3,		REDRM_MODE, PGALL, },
		#if WITHDSPEXTDDC
			{	16, 0,	display_ovf3,		REDRM_BARS, PGALL, },	// ovf/pre
		#endif /* WITHDSPEXTDDC */
			{	20, 0,	display2_lockstate4, REDRM_MODE, PGALL, },
			{	25, 0,	display2_agc3,		REDRM_MODE, PGALL, },
			{	29, 0,	display2_rxbw3,		REDRM_MODE, PGALL, },

			{	0,	8,	display_freqXbig_a, REDRM_FREQ, PGALL, },
			{	29, 8,	display2_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
			//---
			{	0,	15,	display_vfomode5,	REDRM_MODE, PGALL, },	// SPLIT
			{	6,	15,	display2_freqX_b,	REDRM_FRQB, PGALL, },
		#if WITHUSEDUALWATCH
			{	25, 15,	display2_mainsub3,	REDRM_MODE, PGNOMEMU, },	// main/sub RX
		#endif /* WITHUSEDUALWATCH */
			{	29, 15,	display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
			//---
			{	0,	18,	display2_bars,		REDRM_BARS, PG0, },	// S-METER, SWR-METER, POWER-METER
		#if WITHIF4DSP
			{	0,	18,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
			{	0,	18,	display2_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
			{	0,	18,	display2_spectrum,	REDRM_BARS, PG1, },// подготовка изображения спектра
			{	0,	18,	display2_waterfall,	REDRM_BARS, PG1, },// подготовка изображения водопада
			{	0,	18,	display2_colorbuff,	REDRM_BARS,	PG1, },// Отображение водопада и/или спектра

			{	27, 18,	display_smeter5,	REDRM_BARS, PGNOMEMU, },	// signal level
		#endif /* WITHIF4DSP */
			//---
			//{	22, 25,	display_samfreqdelta8, REDRM_BARS, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */

		#if WITHVOLTLEVEL
			{	0, 28,	display2_voltlevelV5, REDRM_VOLT, PGALL, },	// voltmeter with "V"
		#endif /* WITHVOLTLEVEL */
		#if WITHCURRLEVEL || WITHCURRLEVEL2
			{	6, 28,	display2_currlevelA6, REDRM_VOLT, PGALL, },	// amphermeter with "A"
		#endif /* WITHCURRLEVEL || WITHCURRLEVEL2 */
		#if defined (RTC1_TYPE)
			{	13, 28,	display_time8,		REDRM_BARS, PGALL, },	// TIME
		#endif /* defined (RTC1_TYPE) */
		#if WITHUSEAUDIOREC
			{	25, 28,	display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента
		#endif /* WITHUSEAUDIOREC */
		#if WITHMENU
			{	1 + LABELW * 0 + 0,	18,	display2_multilinemenu_block_groups,	REDRM_MLBL, 	REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
			{	1 + LABELW * 1 + 1,	18,	display2_multilinemenu_block_params,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
			{	1 + LABELW * 2 + 2,	18,	display2_multilinemenu_block_vals,	REDRM_MVAL, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
		#endif /* WITHMENU */
		};

		/* получить координаты окна с панорамой и/или водопадом. */
		void display2_getpipparams(pipparams_t * p)
		{
			p->x = 0; //GRID2X(0);	// позиция верхнего левого угла в пикселях
			p->y = GRID2Y(18);	// позиция верхнего левого угла в пикселях
			p->w = DIM_X; //GRID2X(CHARS2GRID(BDTH_ALLRX));	// размер по горизонтали в пикселях
			p->h = GRID2Y(BDCV_ALLRX);				// размер по вертикали в пикселях
		}

	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 5;
			p->ystep = 2;	// количество ячеек разметки на одну строку меню
			p->reverse = 1;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#endif /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */

#elif DSTYLE_G_X480_Y272 && WITHSPECTRUMWF && (WITHTOUCHGUI && WITHGUISTYLE_MINI)

	// TFT панель SONY PSP-1000
	// 272/5 = 54, 480/16=30

	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		//					"012345678901234567890123"
		#define SWRPWRMAP	"1   2   3   4  0% | 100%"
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
	#else
		//					"012345678901234567890123"
		#define POWERMAP	"0    25    50   75   100"
		#define SWRMAP		"1   |   2  |   3   |   4"	//
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
	#endif
	//						"012345678901234567890123"
	#define SMETERMAP		"1  3  5  7  9 +20 +40 60"
	enum
	{
		BDTH_ALLRX = 30,	// ширина зоны для отображение полосы на индикаторе
		BDTH_ALLRXBARS = 24,	// ширина зоны для отображение полосы на индикаторе
		BDTH_LEFTRX = 12,	// ширина индикатора баллов
		BDTH_RIGHTRX = BDTH_ALLRXBARS - BDTH_LEFTRX,	// ширина индикатора плюсов
		BDTH_SPACERX = 0,
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 13,
		BDTH_SPACESWR = 2,
		BDTH_ALLPWR = 9,
		BDTH_SPACEPWR = 0,
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRXBARS,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRXBARS,
		BDTH_SPACEPWR = BDTH_SPACERX,
	#endif /* WITHSHOWSWRPWR */

		BDCV_ALLRX = ROWS2GRID(22),	// количество строк (ячееек), отведенное под S-метр, панораму, иные отображения

		/* совмещение на одном экрание водопада и панорамы */
		BDCO_SPMRX = ROWS2GRID(0),	// смещение спектра по вертикали в ячейках от начала общего поля
		BDCV_SPMRX = ROWS2GRID(12),	// вертикальный размер спектра в ячейках		};
		BDCO_WFLRX = BDCV_SPMRX,	// смещение водопада по вертикали в ячейках от начала общего поля
		BDCV_WFLRX = BDCV_ALLRX - BDCO_WFLRX	// вертикальный размер водопада в ячейках		};
	};

	enum
	{
		PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
		PATTERN_BAR_FULL = 0xFF,
		PATTERN_BAR_HALF = 0x3c,
		PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
		PATTERN_BAR_EMPTYHALF = 0x00	//0x00
	};

	/* совмещение на одном экрание водопада и панорамы */
	enum
	{
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DISPLC_MODCOUNT
	};

	enum
	{
		PG0 = REDRSUBSET(DPAGE0),
		PGNOMEMU = PG0,
		PGALL = PG0 | REDRSUBSET_MENU,
		PGWFL = PG0,	// страница отображения водопада
		PGSPE = PG0,	// страница отображения панорамы
		PGSWR = PG0,	// страница отоюражения S-meter и SWR-meter
		PGLATCH = PGALL | REDRSUBSET_SLEEP,	// страницы, на которых возможно отображение водопада или панорамы.
		PGSLP = REDRSUBSET_SLEEP,
		PGINI = REDRSUBSET_INIT,
		PGunused
	};

	#if TUNE_TOP > 100000000uL
		#define DISPLC_WIDTH	9	// количество цифр в отображении частоты
	#else
		#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
	#endif
	#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты

	#define MENU1ROW 20

	// 272/5 = 54, 480/16=30
	// Main frequency indicator 56 lines height = 12 cells
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0,	0,	display2_clearbg, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
//		{	0,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
//		{	3,	0,	display2_ant5,		REDRM_MODE, PGALL, },
//		{	9,	0,	display2_att4,		REDRM_MODE, PGALL, },
//		{	14,	0,	display2_preovf3,	REDRM_BARS, PGALL, },
//		{	18,	0,	display_lockstate1,	REDRM_BARS, PGALL, },	// LOCK (*)
#if WITHBARS
		{    0, 0,  display2_smeter15_init,	REDRM_INIS, PGINI, },	// Инициализация стрелочного прибора
		{    0, 0,  display2_smeter15, 		REDRM_BARS, PGALL, },	// Изображение стрелочного прибора
#endif /* WITHBARS */
#if WITHAFSPECTRE
		{	0,	0,	display2_af_spectre15_init,	 REDRM_INIS, PGINI, },
		{	0,	0,	display2_af_spectre15_latch, REDRM_BARS, PGLATCH, },
		{	0,	0,	display2_af_spectre15,		 REDRM_BARS, PGSPE, },
#endif /* WITHAFSPECTRE */

//	#if WITHENCODER2
//		{	21, 0,	display2_fnlabel9,	REDRM_MODE, PGALL, },	// FUNC item label
//		{	21,	4,	display2_fnvalue9,	REDRM_MODE, PGALL, },	// FUNC item value
//		{	25, 12,	display2_notch5,		REDRM_MODE, PGALL, },	// NOTCH on/off
//	#else /* WITHENCODER2 */
//		{	25, 0,	display2_notch5,		REDRM_MODE, PGALL, },	// FUNC item label
//		{	25,	4,	display2_notchfreq5,	REDRM_BARS, PGALL, },	// FUNC item value
//	#endif /* WITHENCODER2 */

//		{	26, 16,	display2_nr3,		REDRM_MODE, PGALL, },	// NR
//		{	26,	16,	display2_agc3,		REDRM_MODE, PGALL, },	// AGC mode
//		{	26,	20,	display2_voxtune3,	REDRM_MODE, PGNOMEMU, },	// VOX

		{	13,	0,	display2_freqx_a,	REDRM_FREQ, PGALL, },	// MAIN FREQ Частота (малые цифры)
		{	27,	8,	display2_mode_lower_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
//		{	21,	12,	display2_rxbw3,		REDRM_MODE, PGALL, },	// 3.1 / 0,5 / WID / NAR
//		{	26,	8,	display2_datamode3,	REDRM_MODE, PGALL, },	// DATA mode indicator

//		{	0,	16,	display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента
//		{	0,	16,	display2_mainsub3,	REDRM_MODE, PGALL, },	// main/sub RX: A/A, A/B, B/A, etc

//		{	5,	16,	display2_vfomode3,	REDRM_MODE, PGALL, },	// SPLIT - не очень нужно при наличии индикации на A/B (display2_mainsub3) яркостью.
//		{	9,	16,	display2_freqX_b,	REDRM_FRQB, PGALL, },	// SUB FREQ
//		{	21,	16,	display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...

#if 1
//		{	0,	20,	display2_legend,	REDRM_MODE, PGSWR, },	// Отображение оцифровки шкалы S-метра, PWR & SWR-метра
//		{	0,	24,	display2_bars,		REDRM_BARS, PGSWR, },	// S-METER, SWR-METER, POWER-METER
//		{	25, 24, display2_smeors5, 	REDRM_BARS, PGSWR, },	// уровень сигнала в баллах S или dBm

		{	0,	27,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
		{	0,	27,	display2_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
		{	0,	27,	display2_spectrum,	REDRM_BARS, PGSPE, },// подготовка изображения спектра
		{	0,	27,	display2_waterfall,	REDRM_BARS, PGWFL, },// подготовка изображения водопада
		{	0,	0,	gui_WM_walktrough,	REDRM_BARS,	PGALL, },
		{	0,	27,	display2_colorbuff,	REDRM_BARS,	PGWFL | PGSPE, },// Отображение водопада и/или спектра
#else
		{	0,	20,	display2_adctest,	REDRM_BARS, PGSWR, },	// ADC raw data print
#endif

		//{	0,	51,	display_samfreqdelta8, REDRM_BARS, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */
//		{	0,	51,	display_time5,		REDRM_BARS, PGALL,	},	// TIME
//		{	6, 	51,	display2_atu3,		REDRM_MODE, PGALL, },	// TUNER state (optional)
//		{	10, 51,	display2_byp3,		REDRM_MODE, PGALL, },	// TUNER BYPASS state (optional)
//		{	14, 51,	display2_thermo5,	REDRM_VOLT, PGALL, },	// thermo sensor 20.7C
//		{	19, 51,	display2_currlevel5, REDRM_VOLT, PGALL, },	// PA drain current d.dd without "A"
//		{	25, 51,	display2_voltlevelV5, REDRM_VOLT, PGALL, },	// voltmeter with "V"
	#if WITHAMHIGHKBDADJ
		{	25, 51,	display_amfmhighcut5,REDRM_MODE, PGALL, },	// 13.70
	#endif /* WITHAMHIGHKBDADJ */

//		// sleep mode display
//		{	5,	24,	display2_datetime12,	REDRM_BARS, PGSLP, },	// DATE & TIME // DATE&TIME Jan-01 13:40
//		{	20, 24,	display2_voltlevelV5, REDRM_VOLT, PGSLP, },	// voltmeter with "V"

//	#if WITHMENU
//		{	1,	MENU1ROW,	display2_multilinemenu_block_groups,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
//		{	LABELW + 3,	MENU1ROW,	display2_multilinemenu_block_params,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
//		{	LABELW * 2 + 4,	MENU1ROW,	display2_multilinemenu_block_vals,	REDRM_MVAL, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
//	#endif /* WITHMENU */
		{	0,	0,	display2_nextfb, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
	};

#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		enum { YSTEP = 4 };		// количество ячеек разметки на одну строку меню
		p->multilinemenu_max_rows = (51 - MENU1ROW) / YSTEP;
		p->ystep = YSTEP;	// количество ячеек разметки на одну строку меню
		p->reverse = 1;
		p->valuew = 10;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

	/* получить координаты окна с панорамой и/или водопадом. */
	void display2_getpipparams(pipparams_t * p)
	{
		p->x = 0; //GRID2X(0);	// позиция верхнего левого угла в пикселях
		p->y = GRID2Y(26);	// позиция верхнего левого угла в пикселях
		p->w = DIM_X; //GRID2X(CHARS2GRID(BDTH_ALLRX));	// размер по горизонтали в пикселях
		p->h = GRID2Y(BDCV_ALLRX);				// размер по вертикали в пикселях
	}


#elif DSTYLE_G_X480_Y272 && WITHSPECTRUMWF && ! WITHTOUCHGUI

	// TFT панель SONY PSP-1000
	// 272/5 = 54, 480/16=30

	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		//					"012345678901234567890123"
		#define SWRPWRMAP	"1   2   3   4  0% | 100%" 
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
	#else
		//					"012345678901234567890123"
		#define POWERMAP	"0    25    50   75   100"
		#define SWRMAP		"1   |   2  |   3   |   4"	// 
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
	#endif
	//						"012345678901234567890123"
	#define SMETERMAP		"1  3  5  7  9 +20 +40 60"
	enum
	{
		BDTH_ALLRX = 30,	// ширина зоны для отображение полосы на индикаторе
		BDTH_ALLRXBARS = 24,	// ширина зоны для отображение полосы на индикаторе
		BDTH_LEFTRX = 12,	// ширина индикатора баллов
		BDTH_RIGHTRX = BDTH_ALLRXBARS - BDTH_LEFTRX,	// ширина индикатора плюсов
		BDTH_SPACERX = 0,
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 13,
		BDTH_SPACESWR = 2,
		BDTH_ALLPWR = 9,
		BDTH_SPACEPWR = 0,
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRXBARS,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRXBARS,
		BDTH_SPACEPWR = BDTH_SPACERX,
	#endif /* WITHSHOWSWRPWR */

		BDCV_ALLRX = ROWS2GRID(22),	// количество строк (ячееек), отведенное под S-метр, панораму, иные отображения

		/* совмещение на одном экрание водопада и панорамы */
		BDCO_SPMRX = ROWS2GRID(0),	// смещение спектра по вертикали в ячейках от начала общего поля
		BDCV_SPMRX = ROWS2GRID(12),	// вертикальный размер спектра в ячейках		};
		BDCO_WFLRX = BDCV_SPMRX,	// смещение водопада по вертикали в ячейках от начала общего поля
		BDCV_WFLRX = BDCV_ALLRX - BDCO_WFLRX	// вертикальный размер водопада в ячейках		};
	};

	enum
	{
		PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
		PATTERN_BAR_FULL = 0xFF,
		PATTERN_BAR_HALF = 0x3c,
		PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
		PATTERN_BAR_EMPTYHALF = 0x00	//0x00
	};

	/* совмещение на одном экрание водопада и панорамы */
	enum
	{
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DISPLC_MODCOUNT
	};

	enum
	{
		PG0 = REDRSUBSET(DPAGE0),
		PGNOMEMU = PG0,
		PGALL = PG0 | REDRSUBSET_MENU,
		PGWFL = PG0,	// страница отображения водопада
		PGSPE = PG0,	// страница отображения панорамы
		PGSWR = PGALL,	// страница отоюражения S-meter и SWR-meter
		PGLATCH = PGALL | REDRSUBSET_SLEEP,	// страницы, на которых возможно отображение водопада или панорамы.
		PGSLP = REDRSUBSET_SLEEP,
		PGINI = REDRSUBSET_INIT,
		PGunused
	};

	#if 1//TUNE_TOP > 100000000uL
		#define DISPLC_WIDTH	9	// количество цифр в отображении частоты
	#else
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
	#endif
	#define DISPLC_RJ		0	// количество скрытых справа цифр в отображении частоты

	#define MENU1ROW 28

	// 272/5 = 54, 480/16=30
	// Main frequency indicator 56 lines height = 12 cells
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0,	0,	display2_clearbg, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
		{	0,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
		{	3,	0,	display2_ant5,		REDRM_MODE, PGALL, },
		{	9,	0,	display2_att4,		REDRM_MODE, PGALL, },
		{	14,	0,	display2_preovf3,	REDRM_BARS, PGALL, },
		{	18,	0,	display_lockstate1,	REDRM_BARS, PGALL, },	// LOCK (*)

	#if WITHENCODER2
		{	21, 0,	display2_fnlabel9,	REDRM_MODE, PGALL, },	// FUNC item label
		{	21,	4,	display2_fnvalue9,	REDRM_MODE, PGALL, },	// FUNC item value
		{	25, 12,	display2_notch5,		REDRM_MODE, PGALL, },	// NOTCH on/off
	#else /* WITHENCODER2 */
		{	25, 0,	display2_notch5,		REDRM_MODE, PGALL, },	// FUNC item label
		{	25,	4,	display2_notchfreq5,	REDRM_BARS, PGALL, },	// FUNC item value
	#endif /* WITHENCODER2 */

		{	26, 16,	display2_nr3,		REDRM_MODE, PGALL, },	// NR
//		{	26,	16,	display2_agc3,		REDRM_MODE, PGALL, },	// AGC mode
		{	26,	20,	display2_voxtune3,	REDRM_MODE, PGNOMEMU, },	// VOX

		{	0,	4,	display2_freqX_a,	REDRM_FREQ, PGALL, },	// MAIN FREQ Частота (большие цифры)
		{	21,	8,	display2_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		{	21,	12,	display2_rxbw3,		REDRM_MODE, PGALL, },	// 3.1 / 0,5 / WID / NAR
		{	26,	8,	display2_datamode3,	REDRM_MODE, PGALL, },	// DATA mode indicator

		{	0,	16,	display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента
		{	0,	16,	display2_mainsub3,	REDRM_MODE, PGALL, },	// main/sub RX: A/A, A/B, B/A, etc

		{	5,	16,	display2_vfomode3,	REDRM_MODE, PGALL, },	// SPLIT - не очень нужно при наличии индикации на A/B (display2_mainsub3) яркостью.
		{	9,	16,	display2_freqX_b,	REDRM_FRQB, PGALL, },	// SUB FREQ
		{	21,	16,	display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...

#if 1
		{	0,	20,	display2_legend,	REDRM_MODE, PGSWR, },	// Отображение оцифровки шкалы S-метра, PWR & SWR-метра
		{	0,	24,	display2_bars,		REDRM_BARS, PGSWR, },	// S-METER, SWR-METER, POWER-METER
		{	25, 24, display2_smeors5, 	REDRM_BARS, PGSWR, },	// уровень сигнала в баллах S или dBm

		{	0,	28,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
		{	0,	28,	display2_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
		{	0,	28,	display2_spectrum,	REDRM_BARS, PGSPE, },// подготовка изображения спектра
		{	0,	28,	display2_waterfall,	REDRM_BARS, PGWFL, },// подготовка изображения водопада
		{	0,	28,	display2_colorbuff,	REDRM_BARS,	PGWFL | PGSPE, },// Отображение водопада и/или спектра
#else
		{	0,	20,	display2_adctest,	REDRM_BARS, PGSWR, },	// ADC raw data print
#endif

		//{	0,	51,	display_samfreqdelta8, REDRM_BARS, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */
		{	0,	51,	display_time5,		REDRM_BARS, PGALL,	},	// TIME
		{	6, 	51,	display2_atu3,		REDRM_MODE, PGALL, },	// TUNER state (optional)
		{	10, 51,	display2_byp3,		REDRM_MODE, PGALL, },	// TUNER BYPASS state (optional)
		{	14, 51,	display2_thermo5,	REDRM_VOLT, PGALL, },	// thermo sensor 20.7C
		{	19, 51,	display2_currlevel5, REDRM_VOLT, PGALL, },	// PA drain current d.dd without "A"
		{	25, 51,	display2_voltlevelV5, REDRM_VOLT, PGALL, },	// voltmeter with "V"
	#if WITHAMHIGHKBDADJ
		{	25, 51,	display_amfmhighcut5,REDRM_MODE, PGALL, },	// 13.70
	#endif /* WITHAMHIGHKBDADJ */

		// sleep mode display
		{	5,	24,	display2_datetime12,	REDRM_BARS, PGSLP, },	// DATE & TIME // DATE&TIME Jan-01 13:40
		{	20, 24,	display2_voltlevelV5, REDRM_VOLT, PGSLP, },	// voltmeter with "V"

	#if WITHMENU
		{	1,	MENU1ROW,	display2_multilinemenu_block_groups,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
		{	LABELW + 3,	MENU1ROW,	display2_multilinemenu_block_params,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
		{	LABELW * 2 + 4,	MENU1ROW,	display2_multilinemenu_block_vals,	REDRM_MVAL, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
	#endif /* WITHMENU */
		{	0,	0,	display2_nextfb, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
	};

#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		enum { YSTEP = 4 };		// количество ячеек разметки на одну строку меню
		p->multilinemenu_max_rows = (51 - MENU1ROW) / YSTEP;
		p->ystep = YSTEP;	// количество ячеек разметки на одну строку меню
		p->reverse = 1;
		p->valuew = 10;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

	/* получить координаты окна с панорамой и/или водопадом. */
	void display2_getpipparams(pipparams_t * p)
	{
		p->x = 0; //GRID2X(0);	// позиция верхнего левого угла в пикселях
		p->y = GRID2Y(28);	// позиция верхнего левого угла в пикселях
		p->w = DIM_X; //GRID2X(CHARS2GRID(BDTH_ALLRX));	// размер по горизонтали в пикселях
		p->h = GRID2Y(BDCV_ALLRX);				// размер по вертикали в пикселях
	}

#elif DSTYLE_G_X480_Y272

	// TFT панель SONY PSP-1000
	// 272/5 = 54, 480/16=30
	// без панорамы и водопада

	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		//					"012345678901234567890123"
		#define SWRPWRMAP	"1   2   3   4  0% | 100%" 
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
	#else
		//					"012345678901234567890123"
		#define POWERMAP	"0    25    50   75   100"
		#define SWRMAP		"1   |   2  |   3   |   4"	// 
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
	#endif
	//						"012345678901234567890123"
	#define SMETERMAP		"1  3  5  7  9 +20 +40 60"
	enum
	{
		BDTH_ALLRX = 24,	// ширина зоны для отображение полосы на индикаторе
		BDTH_LEFTRX = 12,	// ширина индикатора баллов
		BDTH_RIGHTRX = BDTH_ALLRX - BDTH_LEFTRX,	// ширина индикатора плюсов
		BDTH_SPACERX = 0,
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 13,
		BDTH_SPACESWR = 2,
		BDTH_ALLPWR = 9,
		BDTH_SPACEPWR = 0,
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX,
	#endif /* WITHSHOWSWRPWR */

		BDCV_ALLRX = ROWS2GRID(20),	// количество ячееек, отведенное под S-метр, панораму, иные отображения
	};

	enum
	{
		PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
		PATTERN_BAR_FULL = 0xFF,
		PATTERN_BAR_HALF = 0x3c,
		PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
		PATTERN_BAR_EMPTYHALF = 0x00	//0x00
	};

		/* совмещение на одном экрание водопада и панорамы */
		enum 
		{
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};

		enum
		{
			PG0 = REDRSUBSET(DPAGE0),
			PGNOMEMU = PG0,
			PGSWR = PG0,	// страница отоюражения S-meter и SWR-meter
			PGALL = PG0 | REDRSUBSET_MENU,
			PGSLP = REDRSUBSET_SLEEP,
			PGINI = REDRSUBSET_INIT,
			PGunused
		};

	#if 1//TUNE_TOP > 100000000uL
		#define DISPLC_WIDTH	9	// количество цифр в отображении частоты
	#else
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
	#endif
	#define DISPLC_RJ		0	// количество скрытых справа цифр в отображении частоты

	static const FLASHMEM struct dzone dzones [] =
	{
			{	0,	0,	display2_clearbg, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
			{	0,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
			{	3,	0,	display2_ant5,		REDRM_MODE, PGALL, },
			{	9,	0,	display2_att4,		REDRM_MODE, PGALL, },
			{	14,	0,	display2_preovf3,	REDRM_BARS, PGALL, },
			{	18,	0,	display_lockstate1,	REDRM_BARS, PGALL, },	// LOCK (*)

		#if WITHENCODER2
			{	21, 0,	display2_fnlabel9,	REDRM_MODE, PGALL, },	// FUNC item label
			{	21,	4,	display2_fnvalue9,	REDRM_MODE, PGALL, },	// FUNC item value
			{	25, 12,	display2_notch5,		REDRM_MODE, PGALL, },	// NOTCH on/off
		#else /* WITHENCODER2 */
			{	25, 0,	display2_notch5,		REDRM_MODE, PGALL, },	// FUNC item label
			{	25,	4,	display2_notchfreq5,	REDRM_BARS, PGALL, },	// FUNC item value
		#endif /* WITHENCODER2 */

			{	26, 16,	display2_nr3,		REDRM_MODE, PGALL, },	// NR
	//		{	26,	16,	display2_agc3,		REDRM_MODE, PGALL, },	// AGC mode
			{	26,	20,	display2_voxtune3,	REDRM_MODE, PGALL, },	// VOX

			{	0,	4,	display2_freqX_a,	REDRM_FREQ, PGALL, },	// MAIN FREQ Частота (большие цифры)
			{	21,	8,	display2_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
			{	21,	12,	display2_rxbw3,		REDRM_MODE, PGALL, },	// 3.1 / 0,5 / WID / NAR
			{	26,	8,	display2_datamode3,	REDRM_MODE, PGALL, },	// DATA mode indicator

			{	0,	16,	display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента
			{	0,	16,	display2_mainsub3,	REDRM_MODE, PGALL, },	// main/sub RX: A/A, A/B, B/A, etc

			{	5,	16,	display2_vfomode3,	REDRM_MODE, PGALL, },	// SPLIT - не очень нужно при наличии индикации на A/B (display2_mainsub3) яркостью.
			{	9,	16,	display2_freqX_b,	REDRM_FRQB, PGALL, },	// SUB FREQ
			{	21,	16,	display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...

	#if 1
	        {	0,  20,	display2_legend_rx, REDRM_MODE, PGSWR, },    // Отображение оцифровки шкалы S-метра
	        {	0,  24,	display2_bars_rx,   REDRM_BARS, PGSWR, },    // S-METER, SWR-METER, POWER-METER
			{	25, 24, display2_smeors5, 	REDRM_BARS, PGSWR, },	 // уровень сигнала в баллах S или dBm
	        {	0,  28,	display2_legend_tx, REDRM_MODE, PGSWR, },    // Отображение оцифровки шкалы PWR & SWR-метра
	        {	0,  32,	display2_bars_tx,   REDRM_BARS, PGSWR, },    // S-METER, SWR-METER, POWER-METER

//			{	0,	28,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
//			{	0,	28,	display2_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
//			{	0,	28,	display2_spectrum,	REDRM_BARS, PGSPE, },// подготовка изображения спектра
//			{	0,	28,	display2_waterfall,	REDRM_BARS, PGWFL, },// подготовка изображения водопада
//			{	0,	28,	display2_colorbuff,	REDRM_BARS,	PGWFL | PGSPE, },// Отображение водопада и/или спектра
	#else
			{	0,	20,	display2_adctest,	REDRM_BARS, PGSWR, },	// ADC raw data print
	#endif

			//{	0,	51,	display_samfreqdelta8, REDRM_BARS, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */
			{	0,	51,	display_time5,		REDRM_BARS, PGALL,	},	// TIME
			{	6, 	51,	display2_atu3,		REDRM_MODE, PGALL, },	// TUNER state (optional)
			{	10, 51,	display2_byp3,		REDRM_MODE, PGALL, },	// TUNER BYPASS state (optional)
			{	14, 51,	display2_thermo5,	REDRM_VOLT, PGALL, },	// thermo sensor 20.7C
			{	19, 51,	display2_currlevel5, REDRM_VOLT, PGALL, },	// PA drain current d.dd without "A"
			{	25, 51,	display2_voltlevelV5, REDRM_VOLT, PGALL, },	// voltmeter with "V"
		#if WITHAMHIGHKBDADJ
			{	25, 51,	display_amfmhighcut5,REDRM_MODE, PGALL, },	// 13.70
		#endif /* WITHAMHIGHKBDADJ */

			// sleep mode display
			{	5,	24,	display2_datetime12,	REDRM_BARS, PGSLP, },	// DATE & TIME // DATE&TIME Jan-01 13:40
			{	20, 24,	display2_voltlevelV5, REDRM_VOLT, PGSLP, },	// voltmeter with "V"

		#if WITHMENU
			{	1,	25,	display2_multilinemenu_block_groups,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
			{	LABELW + 3,	25,	display2_multilinemenu_block_params,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
			{	LABELW * 2 + 4,	25,	display2_multilinemenu_block_vals,	REDRM_MVAL, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
		#endif /* WITHMENU */
			{	0,	0,	display2_nextfb, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
		};

	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 6;
			p->ystep = 4;	// количество ячеек разметки на одну строку меню
			p->reverse = 1;
			p->valuew = 10;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

		/* получить координаты окна с панорамой и/или водопадом. */
		void display2_getpipparams(pipparams_t * p)
		{
			p->x = 0; //GRID2X(0);	// позиция верхнего левого угла в пикселях
			p->y = GRID2Y(28);	// позиция верхнего левого угла в пикселях
			p->w = DIM_X; //GRID2X(CHARS2GRID(BDTH_ALLRX));	// размер по горизонтали в пикселях
			p->h = GRID2Y(BDCV_ALLRX);				// размер по вертикали в пикселях
		}

#elif DSTYLE_G_X800_Y480 && WITHTOUCHGUI && WITHALTERNATIVELAYOUT
	// Вариант с сенсорным экраном
	// TFT панель AT070TN90
	// 480/5 = 96, 800/16=50

	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		//					"012345678901234567890123456789"
		#define SWRPWRMAP	"1    2    3    4  0%   |  100%"
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
	#else
		#warning Should be defined WITHSHOWSWRPWR
	#endif
	//						"012345678901234567890123456789"
	#define SMETERMAP		"1   3   5   7   9  +20 +40 +60"
	enum
	{
		BDTH_ALLRXBARS = 30,	// ширина зоны для отображение барграфов на индикаторе
		BDTH_ALLRX = 50, //DIM_X / GRID2X(1),	// ширина зоны для отображение графического окна на индикаторе
		BDCV_ALLRX = ROWS2GRID(61),	// количество строк, отведенное под S-метр, панораму, иные отображения

		BDTH_LEFTRX = 17,	// ширина индикатора баллов (без плюсов)
		BDTH_RIGHTRX = BDTH_ALLRXBARS - BDTH_LEFTRX,	// ширина индикатора плюсов
		BDTH_SPACERX = 0,
		BDTH_ALLSWR = 16,
		BDTH_SPACESWR = 2,
		BDTH_ALLPWR = 12,
		BDTH_SPACEPWR = 0,

		/* совмещение на одном экрание водопада и панорамы */
		BDCO_SPMRX = ROWS2GRID(0),	// смещение спектра по вертикали в ячейках от начала общего поля
		BDCV_SPMRX = ROWS2GRID(27),	// вертикальный размер спектра в ячейках
		BDCO_WFLRX = BDCV_SPMRX,	// смещение водопада по вертикали в ячейках от начала общего поля
		BDCV_WFLRX = BDCV_ALLRX - BDCV_SPMRX - 10	// вертикальный размер водопада в ячейках
	};
	enum {
		DLES = 35,		// spectrum window upper line
        DLE1 = 93,		//
		DLE_unused
	};


	enum
	{
		PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
		PATTERN_BAR_FULL = 0xFF,
		PATTERN_BAR_HALF = 0x3c,
		PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
		PATTERN_BAR_EMPTYHALF = 0x00	//0x00
	};

	enum
	{
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DISPLC_MODCOUNT
	};

	enum
	{
		PG0 = REDRSUBSET(DPAGE0),
		PGALL = PG0 | REDRSUBSET_MENU,
		PGWFL = PG0,	// страница отображения водопада
		PGSPE = PG0,	// страница отображения панорамы
		PGSWR = PG0,	// страница отоюражения S-meter и SWR-meter
		PGLATCH = PGALL | REDRSUBSET_SLEEP,	// страницы, на которых возможно отображение водопада или панорамы.
		PGSLP = REDRSUBSET_SLEEP,
		PGINI = REDRSUBSET_INIT,
		PGunused
	};

	#if 1//TUNE_TOP > 100000000uL
		#define DISPLC_WIDTH	9	// количество цифр в отображении частоты
	#else
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
	#endif
	#define DISPLC_RJ		0	// количество скрытых справа цифр в отображении частоты

	// 480/5 = 96, 800/16=50
	// 272/5 = 54, 480/16=30 (old)
	//#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	//#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */
	//#define SMALLCHARH 15 /* Font height */
	//#define SMALLCHARW 16 /* Font width */
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0,	0,	display2_clearbg, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
		{	17,	0,	display_txrxstate5alt, REDRM_MODE, PGALL, },
		{	21,	0,	display2_ant7alt,		REDRM_MODE, PGALL, },
		{	26,	0,	display2_att5alt,		REDRM_MODE, PGALL, },
		{	31,	0,	display2_preovf5alt,	REDRM_BARS, PGALL, },
		{	35,	0,	display2_genham1,	REDRM_BARS, PGALL, },	// Отображение режима General Coverage / HAM bands
		{	38,	0,	display2_lockstate5alt, REDRM_MODE, PGALL, },	// LOCK
		{	42, 0,	display2_notch7alt,	REDRM_MODE, PGALL, },	// NOTCH on/off
		{	42, 5,	display2_voxtune3,	REDRM_MODE, PGALL, },	// VOX
		{	46, 5,	display2_datamode3,	REDRM_MODE, PGALL, },	// DATA mode indicator
		{	46, 15,	display2_usbsts3,	REDRM_BARS, PGALL, },	// USB host status
		{	46, 20,	display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента
#if WITHBARS
		{    0, 4,  display2_smeter15_init,REDRM_INIS, PGINI, },	//  Инициализация стрелочного прибора
		{    0, 4,  display2_smeter15, 	REDRM_BARS, PGALL, },	// Изображение стрелочного прибора
#endif /* WITHBARS */
#if WITHAFSPECTRE
		{	0,	4,	display2_af_spectre15_init,	REDRM_INIS, PGINI, },
		{	0,	4,	display2_af_spectre15_latch,	REDRM_BARS,	PGLATCH, },
		{	0,	4,	display2_af_spectre15,		REDRM_BARS, PGSPE, },
#endif /* WITHAFSPECTRE */
		{   0,  0,  layout_init,		REDRM_INIS, PGINI, },
//		{   47, 20, display2_bkin3,		REDRM_MODE, PGALL, },
//		{	46, 20,	display2_agc3,		REDRM_MODE, PGALL, },	// AGC mode
//		{	46, 25,	display2_voxtune3,	REDRM_MODE, PGALL, },	// VOX
//		{	46, 30,	display2_datamode3,	REDRM_MODE, PGALL, },	// DATA mode indicator
//		{	46, 35,	display2_nr3,		REDRM_MODE, PGALL, },	// NR
		{	42, 15,	display2_atu4alt,		REDRM_MODE, PGALL, },	// TUNER state (optional)
		{	42, 20,	display2_byp4alt,		REDRM_MODE, PGALL, },	// TUNER BYPASS state (optional)

		{	17,	7,	display2_freqX_a,	REDRM_FREQ, PGALL, },	// MAIN FREQ Частота (большие цифры)
		{	38, 10,	display2_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		{	42, 10,	display2_rxbw3,		REDRM_MODE, PGALL, },	// 3.1 / 0,5 / WID / NAR
		{	46, 10,	display2_nr3,		REDRM_MODE, PGALL, },	// NR : was: AGC
		{	38, 15,	display2_mainsub3,	REDRM_MODE, PGALL, },	// main/sub RX: A/A, A/B, B/A, etc

		{	42,	15,	display2_vfomode5alt,	REDRM_MODE, PGALL, },	// SPLIT
		{	26,	20,	display2_freqX_b,	REDRM_FRQB, PGALL, },	// SUB FREQ
		{	38, 20,	display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...

#if 1
		// на освободившеемя место добавить статусную строку с различной информацией
//		{	0,	25,	display2_legend,	REDRM_MODE, PGSWR, },	// Отображение оцифровки шкалы S-метра
//		{	0,	30,	display2_bars,		REDRM_BARS, PGSWR, },	// S-METER, SWR-METER, POWER-METER
		{	0,	0, display2_siglevel4, 	REDRM_BARS, PGSWR, },	// signal level dBm
//		{	36, 30,	display2_freqdelta8, REDRM_BARS, PGSWR, },	// выход ЧМ демодулятора
	#if WITHSPECTRUMWF || WITHAFSPECTRE
		{	0,	DLES,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
		{	0,	DLES,	display2_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
		{	0,	DLES,	display2_spectrum,	REDRM_BARS, PGSPE, },// подготовка изображения спектра
		{	0,	DLES,	display2_waterfall,	REDRM_BARS, PGWFL, },// подготовка изображения водопада
		{	0,	DLES,	display2_colorbuff,	REDRM_BARS,	PGWFL | PGSPE, },// Отображение водопада и/или спектра
	#endif /* WITHSPECTRUMWF */
#else
		{	0,	25,	display2_adctest,	REDRM_BARS, PGSWR, },	// ADC raw data print
#endif
	#if WITHAMHIGHKBDADJ
		//{	XX, DLE1,	display_amfmhighcut4,REDRM_MODE, PGALL, },	// 3.70
	#endif /* WITHAMHIGHKBDADJ */
		//{	XX,	DLE1,	display_samfreqdelta8, REDRM_BARS, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */
		// sleep mode display
		{	5,	25,	display2_datetime12,	REDRM_BARS, PGSLP, },	// DATE & TIME // DATE&TIME Jan-01 13:40
		{	20, 25,	display2_voltlevelV5, REDRM_VOLT, PGSLP, },	// voltmeter with "V"
		{	0,	DLES,	gui_WM_walktrough,	REDRM_BARS, PGWFL | PGSPE, },

		{	0,	0,	display2_nextfb, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
	};

#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		p->multilinemenu_max_rows = 15;
		p->ystep = 4;	// количество ячеек разметки на одну строку меню
		p->reverse = 1;
		p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

	/* получить координаты окна с панорамой и/или водопадом. */
	void display2_getpipparams(pipparams_t * p)
	{
		p->x = 0; //GRID2X(0);	// позиция верхнего левого угла в пикселях
		p->y = GRID2Y(DLES);	// позиция верхнего левого угла в пикселях
		p->w = DIM_X; //GRID2X(CHARS2GRID(BDTH_ALLRX));	// размер по горизонтали в пикселях
		p->h = GRID2Y(BDCV_ALLRX);				// размер по вертикали в пикселях
	}

#elif DSTYLE_G_X800_Y480 && WITHTOUCHGUI //&& WITHSPECTRUMWF
	// Вариант с сенсорным экраном
	// TFT панель AT070TN90
	// 480/5 = 96, 800/16=50

	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		//					"012345678901234567890123456789"
		#define SWRPWRMAP	"1    2    3    4  0%   |  100%"
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
	#else
		#warning Should be defined WITHSHOWSWRPWR
	#endif
	//						"012345678901234567890123456789"
	#define SMETERMAP		"1   3   5   7   9  +20 +40 +60"
	enum
	{
		BDTH_ALLRXBARS = 30,	// ширина зоны для отображение барграфов на индикаторе
		BDTH_ALLRX = 50, //DIM_X / GRID2X(1),	// ширина зоны для отображение графического окна на индикаторе
		BDCV_ALLRX = ROWS2GRID(61),	// количество строк, отведенное под S-метр, панораму, иные отображения

		BDTH_LEFTRX = 17,	// ширина индикатора баллов (без плюсов)
		BDTH_RIGHTRX = BDTH_ALLRXBARS - BDTH_LEFTRX,	// ширина индикатора плюсов
		BDTH_SPACERX = 0,
		BDTH_ALLSWR = 16,
		BDTH_SPACESWR = 2,
		BDTH_ALLPWR = 12,
		BDTH_SPACEPWR = 0,

		/* совмещение на одном экрание водопада и панорамы */
		BDCO_SPMRX = ROWS2GRID(0),	// смещение спектра по вертикали в ячейках от начала общего поля
		BDCV_SPMRX = ROWS2GRID(27),	// вертикальный размер спектра в ячейках
		BDCO_WFLRX = BDCV_SPMRX,	// смещение водопада по вертикали в ячейках от начала общего поля
		BDCV_WFLRX = BDCV_ALLRX - BDCV_SPMRX - 10	// вертикальный размер водопада в ячейках
	};
	enum {
		DLES = 35,		// spectrum window upper line
        DLE1 = 93,		//
		DLE_unused
	};


	enum
	{
		PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
		PATTERN_BAR_FULL = 0xFF,
		PATTERN_BAR_HALF = 0x3c,
		PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
		PATTERN_BAR_EMPTYHALF = 0x00	//0x00
	};

	enum
	{
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DISPLC_MODCOUNT
	};

	enum
	{
		PG0 = REDRSUBSET(DPAGE0),
		PGALL = PG0 | REDRSUBSET_MENU,
		PGWFL = PG0,	// страница отображения водопада
		PGSPE = PG0,	// страница отображения панорамы
		PGSWR = PG0,	// страница отоюражения S-meter и SWR-meter
		PGLATCH = PGALL | REDRSUBSET_SLEEP,	// страницы, на которых возможно отображение водопада или панорамы.
		PGSLP = REDRSUBSET_SLEEP,
		PGINI = REDRSUBSET_INIT,
		PGunused
	};

	#if 1//TUNE_TOP > 100000000uL
		#define DISPLC_WIDTH	9	// количество цифр в отображении частоты
	#else
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
	#endif
	#define DISPLC_RJ		0	// количество скрытых справа цифр в отображении частоты

	// 480/5 = 96, 800/16=50
	// 272/5 = 54, 480/16=30 (old)
	//#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	//#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */
	//#define SMALLCHARH 15 /* Font height */
	//#define SMALLCHARW 16 /* Font width */
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0,	0,	display2_clearbg, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
		{	17,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
		{	20,	0,	display2_ant5,		REDRM_MODE, PGALL, },
		{	26,	0,	display2_att4,		REDRM_MODE, PGALL, },
		{	31,	0,	display2_preovf3,	REDRM_BARS, PGALL, },
		{	35,	0,	display2_genham1,	REDRM_BARS, PGALL, },	// Отображение режима General Coverage / HAM bands
		{	38,	0,	display2_lockstate4, REDRM_MODE, PGALL, },	// LOCK
		{	43, 0,	display2_notch5,	REDRM_MODE, PGALL, },	// NOTCH on/off
		{	43, 5,	display2_voxtune3,	REDRM_MODE, PGALL, },	// VOX
		{	47, 5,	display2_datamode3,	REDRM_MODE, PGALL, },	// DATA mode indicator
		{	47, 15,	display2_usbsts3,	REDRM_BARS, PGALL, },	// USB host status
		{	47, 20,	display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента
#if WITHBARS
		{    0, 4,  display2_smeter15_init,REDRM_INIS, PGINI, },	//  Инициализация стрелочного прибора
		{    0, 4,  display2_smeter15, 	REDRM_BARS, PGALL, },	// Изображение стрелочного прибора
#endif /* WITHBARS */
#if WITHAFSPECTRE
		{	0,	4,	display2_af_spectre15_init,	REDRM_INIS, PGINI, },
		{	0,	4,	display2_af_spectre15_latch,	REDRM_BARS,	PGLATCH, },
		{	0,	4,	display2_af_spectre15,		REDRM_BARS, PGSPE, },
#endif /* WITHAFSPECTRE */

//		{   47, 20, display2_bkin3,		REDRM_MODE, PGALL, },
	#if WITHENCODER2
//		{	41, 0,	display2_fnlabel9,	REDRM_MODE, PGALL, },	// FUNC item label
//		{	41,	4,	display2_fnvalue9,	REDRM_MODE, PGALL, },	// FUNC item value
//		{	45, 15,	display2_notch5,		REDRM_MODE, PGALL, },	// NOTCH on/off
	#else /* WITHENCODER2 */
		{	45, 0,	display2_notch5,		REDRM_MODE, PGALL, },	// FUNC item label
		{	45,	4,	display2_notchfreq5,	REDRM_BARS, PGALL, },	// FUNC item value
	#endif /* WITHENCODER2 */

//		{	46, 20,	display2_agc3,		REDRM_MODE, PGALL, },	// AGC mode
//		{	46, 25,	display2_voxtune3,	REDRM_MODE, PGALL, },	// VOX
//		{	46, 30,	display2_datamode3,	REDRM_MODE, PGALL, },	// DATA mode indicator
//		{	46, 35,	display2_nr3,		REDRM_MODE, PGALL, },	// NR
		{	43, 15,	display2_atu3,		REDRM_MODE, PGALL, },	// TUNER state (optional)
		{	43, 20,	display2_byp3,		REDRM_MODE, PGALL, },	// TUNER BYPASS state (optional)
		
		{	17,	7,	display2_freqX_a,	REDRM_FREQ, PGALL, },	// MAIN FREQ Частота (большие цифры)
		{	38, 10,	display2_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		{	43, 10,	display2_rxbw3,		REDRM_MODE, PGALL, },	// 3.1 / 0,5 / WID / NAR
		{	47, 10,	display2_nr3,		REDRM_MODE, PGALL, },	// NR : was: AGC
		{	38, 15,	display2_mainsub3,	REDRM_MODE, PGALL, },	// main/sub RX: A/A, A/B, B/A, etc

		{	43,	15,	display2_vfomode3,	REDRM_MODE, PGALL, },	// SPLIT
		{	26,	20,	display2_freqX_b,	REDRM_FRQB, PGALL, },	// SUB FREQ
		{	38, 20,	display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...

#if 1
		// на освободившеемя место добавить статусную строку с различной информацией
//		{	0,	25,	display2_legend,	REDRM_MODE, PGSWR, },	// Отображение оцифровки шкалы S-метра
//		{	0,	30,	display2_bars,		REDRM_BARS, PGSWR, },	// S-METER, SWR-METER, POWER-METER
		{	0,	0, display2_siglevel4, 	REDRM_BARS, PGSWR, },	// signal level dBm
//		{	36, 30,	display2_freqdelta8, REDRM_BARS, PGSWR, },	// выход ЧМ демодулятора
	#if WITHSPECTRUMWF || WITHAFSPECTRE
		{	0,	DLES,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
		{	0,	DLES,	display2_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
		{	0,	DLES,	display2_spectrum,	REDRM_BARS, PGSPE, },// подготовка изображения спектра
		{	0,	DLES,	display2_waterfall,	REDRM_BARS, PGWFL, },// подготовка изображения водопада
		{	0,	DLES,	display2_colorbuff,	REDRM_BARS,	PGWFL | PGSPE, },// Отображение водопада и/или спектра
	#endif /* WITHSPECTRUMWF */
#else
		{	0,	25,	display2_adctest,	REDRM_BARS, PGSWR, },	// ADC raw data print
#endif
	#if WITHAMHIGHKBDADJ
		//{	XX, DLE1,	display_amfmhighcut4,REDRM_MODE, PGALL, },	// 3.70
	#endif /* WITHAMHIGHKBDADJ */
		//{	XX,	DLE1,	display_samfreqdelta8, REDRM_BARS, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */
		// sleep mode display
		{	5,	25,	display2_datetime12,	REDRM_BARS, PGSLP, },	// DATE & TIME // DATE&TIME Jan-01 13:40
		{	20, 25,	display2_voltlevelV5, REDRM_VOLT, PGSLP, },	// voltmeter with "V"
		{	0,	DLES,	gui_WM_walktrough,	REDRM_BARS, PGWFL | PGSPE, },

		{	0,	0,	display2_nextfb, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
	};

#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		p->multilinemenu_max_rows = 15;
		p->ystep = 4;	// количество ячеек разметки на одну строку меню
		p->reverse = 1;
		p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

	/* получить координаты окна с панорамой и/или водопадом. */
	void display2_getpipparams(pipparams_t * p)
	{
		p->x = 0; //GRID2X(0);	// позиция верхнего левого угла в пикселях
		p->y = GRID2Y(DLES);	// позиция верхнего левого угла в пикселях
		p->w = DIM_X; //GRID2X(CHARS2GRID(BDTH_ALLRX));	// размер по горизонтали в пикселях
		p->h = GRID2Y(BDCV_ALLRX);				// размер по вертикали в пикселях
	}

#elif DSTYLE_G_X800_Y480 // && (LCDMODE_MAIN_PAGES > 1)	//&& WITHSPECTRUMWF
	// вариант без сенсорного экрана
	// стрелочный S-метр
	// TFT панель AT070TN90
	// 480/5 = 96, 800/16=50

	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале (на этом дизайне нет, просто для того чтобы компилировлось)

	enum {
		DLES = 35,		// spectrum window upper line
        DLE1 = 91,		// 96-5
		DLE_unused
	};

	enum
	{
		BDTH_ALLRXBARS = 30,	// ширина зоны для отображение барграфов на индикаторе

		BDTH_ALLRX = 50, 		// ширина зоны для отображение графического окна на индикаторе
		BDCV_ALLRX = ROWS2GRID(55 /* DLE1 - DLES */),	// количество строк, отведенное под панораму и волопад.

		BDTH_LEFTRX = 17,	// ширина индикатора баллов (без плюсов)
		BDTH_RIGHTRX = BDTH_ALLRXBARS - BDTH_LEFTRX,	// ширина индикатора плюсов
		BDTH_SPACERX = 0,
		BDTH_ALLSWR = 16,
		BDTH_SPACESWR = 2,
		BDTH_ALLPWR = 12,
		BDTH_SPACEPWR = 0,

		/* совмещение на одном экрание водопада и панорамы */
		BDCO_SPMRX = ROWS2GRID(0),	// смещение спектра по вертикали в ячейках от начала общего поля
		BDCV_SPMRX = ROWS2GRID(32),	// вертикальный размер спектра в ячейках
		BDCO_WFLRX = BDCV_SPMRX,	// смещение водопада по вертикали в ячейках от начала общего поля
		BDCV_WFLRX = BDCV_ALLRX - BDCV_SPMRX	// вертикальный размер водопада в ячейках
	};


	enum
	{
		PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
		PATTERN_BAR_FULL = 0xFF,
		PATTERN_BAR_HALF = 0x3c,
		PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
		PATTERN_BAR_EMPTYHALF = 0x00	//0x00
	};

	enum
	{
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DISPLC_MODCOUNT
	};

	enum
	{
		PG0 = REDRSUBSET(DPAGE0),
		PGALL = PG0 | REDRSUBSET_MENU,
		PGWFL = PG0,	// страница отображения водопада
		PGSPE = PG0,	// страница отображения панорамы
		PGSWR = PG0,	// страница отоюражения S-meter и SWR-meter
		PGLATCH = PGALL | REDRSUBSET_SLEEP,	// страницы, на которых возможно отображение водопада или панорамы.
		PGSLP = REDRSUBSET_SLEEP,
		PGINI = REDRSUBSET_INIT,
		PGunused
	};

	#if 1//TUNE_TOP > 100000000uL
		#define DISPLC_WIDTH	9	// количество цифр в отображении частоты
	#else
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
	#endif
	#define DISPLC_RJ		0	// количество скрытых справа цифр в отображении частоты

	// 480/5 = 96, 800/16=50
	// 272/5 = 54, 480/16=30 (old)
	//#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	//#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */
	//#define SMALLCHARH 15 /* Font height */
	//#define SMALLCHARW 16 /* Font width */
	static const FLASHMEM struct dzone dzones [] =
	{
		/* общий для всех режимов элемент */
		{	0,	0,	display2_clearbg, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },

		{	0,	0, 	display_siglevel7, 	REDRM_BARS, PGALL, },	// signal level dBm
		//{	0,	0, 	display2_smeors5, 	REDRM_BARS, PGALL, },	// уровень сигнала в баллах S или dBm
		{	15,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
		{	18, 0,	display2_atu3,		REDRM_MODE, PGALL, },	// TUNER state (optional)
		{	22, 0,	display2_byp3,		REDRM_MODE, PGALL, },	// TUNER BYPASS state (optional)
		{	26,	0,	display2_ant5,		REDRM_MODE, PGALL, },
		{	32,	0,	display2_att4,		REDRM_MODE, PGALL, },
		{	37,	0,	display2_preovf3,	REDRM_BARS, PGALL, },
	#if WITHBARS
		{   0, 	4,  display2_smeter15_init,REDRM_INIS, PGINI, },	//  Инициализация стрелочного прибора
		{   0, 	4,	display2_smeter15, 	REDRM_BARS, PGALL, },	// Изображение стрелочного прибора
	#endif /* WITHBARS */
	#if WITHAFSPECTRE
		{	0,	4,	display2_af_spectre15_init,	REDRM_INIS, PGINI, },
		{	0,	4,	display2_af_spectre15_latch,	REDRM_BARS,	PGLATCH, },
		{	0,	4,	display2_af_spectre15,		REDRM_BARS, PGSPE, },
	#endif /* WITHAFSPECTRE */

		{	15,	6,	display2_freqX_a,	REDRM_FREQ, PGALL, },	// MAIN FREQ Частота (большие цифры)

		{	41, 0,	display2_fnlabel9,	REDRM_MODE, PGALL, },	// FUNC item label
		{	41,	4,	display2_fnvalue9,	REDRM_MODE, PGALL, },	// FUNC item value

		{	37, 10,	display2_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		{	41, 10,	display2_rxbw3,		REDRM_MODE, PGALL, },	// 3.1 / 0,5 / WID / NAR
		{	46, 10,	display2_agc3,		REDRM_MODE, PGALL, },	// AGC mode

		{	37, 15,	display2_nr3,		REDRM_MODE, PGALL, },	// NR : was: AGC
		{	41, 15,	display2_datamode3,	REDRM_MODE, PGALL, },	// DATA mode indicator
		{	45, 15,	display2_notch5,		REDRM_MODE, PGALL, },	// NOTCH on/off

		{	15, 20,	display2_mainsub3,	REDRM_MODE, PGALL, },	// main/sub RX: A/A, A/B, B/A, etc
		{	20,	20,	display2_vfomode3,	REDRM_MODE, PGALL, },	// SPL
		{	24,	20,	display2_freqX_b,	REDRM_FRQB, PGALL, },	// SUB FREQ
		{	37, 20,	display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		{	41, 20,	display2_voxtune3,	REDRM_MODE, PGALL, },	// VOX
		{	45,	20,	display2_lockstate4, REDRM_MODE, PGALL, },	// LOCK

		// размещены под S-метром (15 ячеек)
		{	1, 	25,	display2_voltlevelV5, REDRM_VOLT, PGALL, },	// voltmeter with "V"
		{	7, 	25,	display2_currlevelA6, REDRM_VOLT, PGALL, },	// PA drain current d.dd with "A"

		//{	14, 25,
		//{	19, 25,
		//{	23, 25,
		//{	27, 25,
		{	33, 25,	display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента
		{	37, 25,	display2_spk3,		REDRM_MODE, PGALL, },	// оьображение признака включения динамика
		{	41, 25, display2_bkin3,		REDRM_MODE, PGALL, },	// BREAK-IN
		{	45,	25,	display2_wpm5, 		REDRM_BARS, PGALL, },	// 22WPM

		//{	24, 30,	display_freqmeter10, REDRM_BARS, PGALL, },	// измеренная частота опоры
		{	37, 30,	display2_freqdelta8, REDRM_BARS, PGALL, },	// выход ЧМ демодулятора

	#if WITHSPECTRUMWF
		{	0,	DLES,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
		{	0,	DLES,	display2_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
		{	0,	DLES,	display2_spectrum,	REDRM_BARS, PGSPE, },// подготовка изображения спектра
		{	0,	DLES,	display2_waterfall,	REDRM_BARS, PGWFL, },// подготовка изображения водопада
		{	0,	DLES,	display2_colorbuff,	REDRM_BARS,	PGWFL | PGSPE, },// Отображение водопада и/или спектра
	#endif /* WITHSPECTRUMWF */

		{	0,	DLE1,	display2_datetime12,	REDRM_BARS, PGALL,	},	// DATE&TIME Jan-01 13:40
		{	13,	DLE1,	display2_span9,		REDRM_MODE, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */
		{	23, DLE1,	display2_thermo4,	REDRM_VOLT, PGALL, },	// thermo sensor
		{	28, DLE1,	display2_usbsts3,		REDRM_BARS, PGALL, },	// USB host status
		//{	28, DLE1,	display_freqmeter10, REDRM_BARS, PGALL, },	// измеренная частота опоры

	#if WITHMENU
		{	3,				DLES,	display2_multilinemenu_block_groups,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
		{	LABELW + 5,		DLES,	display2_multilinemenu_block_params,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
		{	LABELW*2 + 6,	DLES,	display2_multilinemenu_block_vals,	REDRM_MVAL, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
	#endif /* WITHMENU */

		// sleep mode display
		{	5,	25,	display2_datetime12,	REDRM_BARS, PGSLP, },	// DATE & TIME // DATE&TIME Jan-01 13:40
		{	20, 25,	display2_voltlevelV5, REDRM_VOLT, PGSLP, },	// voltmeter with "V"
	#if WITHMENU
		{	15, 25,	display2_popup,		REDRM_MODE, PG0, },	// Всплывающие меню. В конце массива для перекрытия всего что под ним
	#endif /* WITHMENU */

	#if 0
		{	0,	0,	display2_vtty_init,	REDRM_INIS,	PGINI, },	// Подготовка видеобуфера окна протокола
		{	0,	0, display2_vtty,	REDRM_BARS, PG0, },		// Вывод текущего состояния протокола
		{	0,	(DIM_Y - GRID2Y(5)) / 5, display2_freqsof9,	REDRM_BARS, PG0, },		// Вывод текущего состояния протокола
	#endif

		/* общий для всех режимов элемент */
		{	0,	0,	display2_nextfb, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
	};

#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		enum { YSTEP = 5 };		// количество ячеек разметки на одну строку меню
		p->multilinemenu_max_rows = (DLE1 - DLES) / YSTEP;
		p->ystep = YSTEP;	// количество ячеек разметки на одну строку меню
		p->reverse = 1;
		p->valuew = 10;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

	/* получить координаты окна с панорамой и/или водопадом. */
	void display2_getpipparams(pipparams_t * p)
	{
		p->x = GRID2X(0);	// позиция верхнего левого угла в пикселях
		p->y = GRID2Y(DLES);	// позиция верхнего левого угла в пикселях
		p->w = GRID2X(CHARS2GRID(BDTH_ALLRX));	// размер по горизонтали в пикселях
		p->h = GRID2Y(BDCV_ALLRX);				// размер по вертикали в пикселях
	}


#elif DSTYLE_G_X800_Y480 && 1	//&& WITHSPECTRUMWF
	// вариант без сенсорного экрана и без offscreen composition
	// Для тестирования на минимальном объеме памяти.
	// TFT панель AT070TN90
	// 480/5 = 96, 800/16=50

	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		//					"012345678901234567890123456789"
		#define SWRPWRMAP	"1    2    3    4  0%   |  100%"
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
	#else
		#warning Should be defined WITHSHOWSWRPWR
	#endif
	//						"012345678901234567890123456789"
	#define SMETERMAP		"1   3   5   7   9  +20 +40 +60"
	enum
	{
		BDTH_ALLRXBARS = 30,	// ширина зоны для отображение барграфов на индикаторе
#if 1
		BDTH_ALLRX = 50, //DIM_X / GRID2X(1),	// ширина зоны для отображение графического окна на индикаторе
		BDCV_ALLRX = ROWS2GRID(49),	// количество строк, отведенное под S-метр, панораму, иные отображения
#else
		BDTH_ALLRX = 40,	// ширина зоны для отображение графического окна на индикаторе
		BDCV_ALLRX = ROWS2GRID(55),	// количество строк, отведенное под S-метр, панораму, иные отображения
#endif

		BDTH_LEFTRX = 17,	// ширина индикатора баллов (без плюсов)
		BDTH_RIGHTRX = BDTH_ALLRXBARS - BDTH_LEFTRX,	// ширина индикатора плюсов
		BDTH_SPACERX = 0,
		BDTH_ALLSWR = 16,
		BDTH_SPACESWR = 2,
		BDTH_ALLPWR = 12,
		BDTH_SPACEPWR = 0,

		/* совмещение на одном экрание водопада и панорамы */
		BDCO_SPMRX = ROWS2GRID(0),	// смещение спектра по вертикали в ячейках от начала общего поля
		BDCV_SPMRX = ROWS2GRID(27),	// вертикальный размер спектра в ячейках
		BDCO_WFLRX = BDCV_SPMRX,	// смещение водопада по вертикали в ячейках от начала общего поля
		BDCV_WFLRX = BDCV_ALLRX - BDCV_SPMRX	// вертикальный размер водопада в ячейках
	};
	enum {
		DLES = 35,		// spectrum window upper line
        DLE1 = 93,		//
		DLE_unused
	};


	enum
	{
		PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
		PATTERN_BAR_FULL = 0xFF,
		PATTERN_BAR_HALF = 0x3c,
		PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
		PATTERN_BAR_EMPTYHALF = 0x00	//0x00
	};

	enum
	{
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DISPLC_MODCOUNT
	};

	enum
	{
		PG0 = REDRSUBSET(DPAGE0),
		PGALL = PG0 | REDRSUBSET_MENU,
		PGWFL = PG0,	// страница отображения водопада
		PGSPE = PG0,	// страница отображения панорамы
		PGSWR = PG0,	// страница отоюражения S-meter и SWR-meter
		PGLATCH = PGALL | REDRSUBSET_SLEEP,	// страницы, на которых возможно отображение водопада или панорамы.
		PGSLP = REDRSUBSET_SLEEP,
		PGunused
	};

	#if 1//TUNE_TOP > 100000000uL
		#define DISPLC_WIDTH	9	// количество цифр в отображении частоты
	#else
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
	#endif
	#define DISPLC_RJ		0	// количество скрытых справа цифр в отображении частоты

	// 480/5 = 96, 800/16=50
	// 272/5 = 54, 480/16=30 (old)
	//#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	//#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */
	//#define SMALLCHARH 15 /* Font height */
	//#define SMALLCHARW 16 /* Font width */
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0,	0,	display2_clearbg, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
		{	0,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
		{	3,	0,	display2_ant5,		REDRM_MODE, PGALL, },
		{	9,	0,	display2_att4,		REDRM_MODE, PGALL, },
		{	14,	0,	display2_preovf3,	REDRM_BARS, PGALL, },
		{	18,	0,	display2_genham1,	REDRM_BARS, PGALL, },	// Отображение режима General Coverage / HAM bands
		{	21,	0,	display2_lockstate4, REDRM_MODE, PGALL, },	// LOCK

	#if WITHENCODER2
		{	41, 0,	display2_fnlabel9,	REDRM_MODE, PGALL, },	// FUNC item label
		{	41,	4,	display2_fnvalue9,	REDRM_MODE, PGALL, },	// FUNC item value
		{	45, 15,	display2_notch5,		REDRM_MODE, PGALL, },	// NOTCH on/off
	#else /* WITHENCODER2 */
		{	45, 0,	display2_notch5,		REDRM_MODE, PGALL, },	// FUNC item label
		{	45,	4,	display2_notchfreq5,	REDRM_BARS, PGALL, },	// FUNC item value
	#endif /* WITHENCODER2 */

		{	34, 20,	display2_atu3,		REDRM_MODE, PGALL, },	// TUNER state (optional)
		{	38, 20,	display2_byp3,		REDRM_MODE, PGALL, },	// TUNER BYPASS state (optional)
		{   42, 20, display2_bkin3,		REDRM_MODE, PGALL, },
		{	46, 20,	display2_voxtune3,	REDRM_MODE, PGALL, },	// VOX

		{	38, 25,	display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента

		{	42, 25,	display2_datamode3,	REDRM_MODE, PGALL, },	// DATA mode indicator
		{	46, 25,	display2_nr3,		REDRM_MODE, PGALL, },	// NR : was: AGC

		{	0,	7,	display2_freqX_a,	REDRM_FREQ, PGALL, },	// MAIN FREQ Частота (большие цифры)
		{	21, 10,	display2_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		{	26, 10,	display2_rxbw3,		REDRM_MODE, PGALL, },	// 3.1 / 0,5 / WID / NAR
		{	21, 15,	display2_mainsub3,	REDRM_MODE, PGALL, },	// main/sub RX: A/A, A/B, B/A, etc

		{	26,	15,	display2_vfomode3,	REDRM_MODE, PGALL, },	// SPLIT
		{	9,	20,	display2_freqX_b,	REDRM_FRQB, PGALL, },	// SUB FREQ
		{	21, 20,	display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...

#if 1
		{	0,	25,	display2_legend,	REDRM_MODE, PGSWR, },	// Отображение оцифровки шкалы S-метра
		{	0,	30,	display2_bars,		REDRM_BARS, PGSWR, },	// S-METER, SWR-METER, POWER-METER
		//{	0,	35,	display2_legend_tx,	REDRM_MODE, PGSWR, },	// Отображение оцифровки шкалы PWR & SWR-метра
		//{	0,	40,	display2_bars_tx,	REDRM_BARS, PGSWR, },	// S-METER, SWR-METER, POWER-METER
		{	31,	30, display2_siglevel4, 	REDRM_BARS, PGSWR, },	// signal level dBm
		{	36, 30,	display2_freqdelta8, REDRM_BARS, PGSWR, },	// выход ЧМ демодулятора
		{	46, 30,	display2_agc3,		REDRM_MODE, PGALL, },	// AGC mode
	#if WITHSPECTRUMWF
		{	0,	DLES,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
		{	0,	DLES,	display2_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
		{	0,	DLES,	display2_spectrum,	REDRM_BARS, PGSPE, },// подготовка изображения спектра
		{	0,	DLES,	display2_waterfall,	REDRM_BARS, PGWFL, },// подготовка изображения водопада
		{	0,	DLES,	display2_colorbuff,	REDRM_BARS,	PGWFL | PGSPE, },// Отображение водопада и/или спектра
	#endif /* WITHSPECTRUMWF */
#else
		{	0,	25,	display2_adctest,	REDRM_BARS, PGSWR, },	// ADC raw data print
#endif



		{	0,	DLE1,	display2_datetime12,	REDRM_BARS, PGALL,	},	// DATE&TIME Jan-01 13:40
		{	13,	DLE1,	display2_span9,		REDRM_MODE, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */
		{	23, DLE1,	display2_thermo4,	REDRM_VOLT, PGALL, },	// thermo sensor
		{	28, DLE1,	display2_usbsts3,		REDRM_BARS, PGALL, },	// USB host status
		//{	28, DLE1,	display_freqmeter10, REDRM_BARS, PGALL, },	// измеренная частота опоры

		{	39, DLE1,	display2_currlevel5, REDRM_VOLT, PGALL, },	// PA drain current d.dd without "A"
		{	45, DLE1,	display2_voltlevelV5, REDRM_VOLT, PGALL, },	// voltmeter with "V"
	#if WITHAMHIGHKBDADJ
		//{	XX, DLE1,	display_amfmhighcut4,REDRM_MODE, PGALL, },	// 3.70
	#endif /* WITHAMHIGHKBDADJ */
		//{	XX,	DLE1,	display_samfreqdelta8, REDRM_BARS, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */

	#if WITHMENU
		{	3,				30,	display2_multilinemenu_block_groups,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
		{	LABELW + 5,		30,	display2_multilinemenu_block_params,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
		{	LABELW*2 + 6,	30,	display2_multilinemenu_block_vals,	REDRM_MVAL, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
	#endif /* WITHMENU */

		// sleep mode display
		{	5,	25,	display2_datetime12,	REDRM_BARS, PGSLP, },	// DATE & TIME // DATE&TIME Jan-01 13:40
		{	20, 25,	display2_voltlevelV5, REDRM_VOLT, PGSLP, },	// voltmeter with "V"

		/* общий для всех режимов элемент */
		{	0,	0,	display2_nextfb, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
	};

#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		p->multilinemenu_max_rows = 15;
		p->ystep = 4;	// количество ячеек разметки на одну строку меню
		p->reverse = 1;
		p->valuew = 10;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

	/* получить координаты окна с панорамой и/или водопадом. */
	void display2_getpipparams(pipparams_t * p)
	{
		p->x = 0; //GRID2X(0);	// позиция верхнего левого угла в пикселях
		p->y = GRID2Y(DLES);	// позиция верхнего левого угла в пикселях
		p->w = DIM_X; //GRID2X(CHARS2GRID(BDTH_ALLRX));	// размер по горизонтали в пикселях
		p->h = GRID2Y(BDCV_ALLRX);				// размер по вертикали в пикселях
	}

#elif DSTYLE_G_DUMMY

#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
	//					"012345678901234567890123"
	#define SWRPWRMAP	"1   2   3   4  0% | 100%"
	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
#else
	//					"012345678901234567890123"
	#define POWERMAP	"0    25    50   75   100"
	#define SWRMAP		"1   |   2  |   3   |   4"	//
	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
#endif
//						"012345678901234567890123"
#define SMETERMAP		"1  3  5  7  9 +20 +40 60"
enum
{
	BDTH_ALLRXBARS = 24,	// ширина зоны для отображение барграфов на индикаторе
	BDTH_ALLRX = 40,	// ширина зоны для отображение графического окна на индикаторе

	BDTH_LEFTRX = 12,	// ширина индикатора баллов (без плюслв)
	BDTH_RIGHTRX = BDTH_ALLRXBARS - BDTH_LEFTRX,	// ширина индикатора плюсов
	BDTH_SPACERX = 0,
	BDTH_ALLSWR = 13,
	BDTH_SPACESWR = 2,
	BDTH_ALLPWR = 9,
	BDTH_SPACEPWR = 0,

	BDCV_ALLRX = ROWS2GRID(55),	// количество строк, отведенное под S-метр, панораму, иные отображения
	/* совмещение на одном экрание водопада и панорамы */
	BDCO_SPMRX = ROWS2GRID(0),	// смещение спектра по вертикали в ячейках от начала общего поля
	BDCV_SPMRX = ROWS2GRID(27),	// вертикальный размер спектра в ячейках
	BDCO_WFLRX = BDCV_SPMRX,	// смещение водопада по вертикали в ячейках от начала общего поля
	BDCV_WFLRX = BDCV_ALLRX - BDCV_SPMRX	// вертикальный размер водопада в ячейках
};
enum {
	DLES = 35	// spectrum window upper line
};


enum
{
	PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
	PATTERN_BAR_FULL = 0xFF,
	PATTERN_BAR_HALF = 0x3c,
	PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
	PATTERN_BAR_EMPTYHALF = 0x00	//0x00
};

enum
{
	DPAGE0,					// Страница, в которой отображаются основные (или все)
	DISPLC_MODCOUNT
};

#if 1//TUNE_TOP > 100000000uL
	#define DISPLC_WIDTH	9	// количество цифр в отображении частоты
#else
	#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
#endif
#define DISPLC_RJ		0	// количество скрытых справа цифр в отображении частоты


static void display2_dummy(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{

}
// 480/5 = 96, 800/16=50
// 272/5 = 54, 480/16=30 (old)
//#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
//#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */
//#define SMALLCHARH 15 /* Font height */
//#define SMALLCHARW 16 /* Font width */
static const FLASHMEM struct dzone dzones [] =
{
	{	0,	0,	display2_dummy,	REDRM_MODE,	REDRSUBSET_SLEEP | REDRSUBSET_MENU },	// Выключить PIP если на данной странице не требуется
};

#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		p->multilinemenu_max_rows = 15;
		p->ystep = 4;	// количество ячеек разметки на одну строку меню
		p->reverse = 1;
		p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

	/* получить координаты окна с панорамой и/или водопадом. */
	void display2_getpipparams(pipparams_t * p)
	{
		p->x = 0; //GRID2X(0);	// позиция верхнего левого угла в пикселях
		p->y = 0;	// позиция верхнего левого угла в пикселях
		p->w = DIM_X; //GRID2X(CHARS2GRID(BDTH_ALLRX));	// размер по горизонтали в пикселях
		p->h = DIM_Y;				// размер по вертикали в пикселях
		//p->frame = (uintptr_t) 0;
	}

#else
	#error TODO: to be implemented
#endif /* LCDMODE_LS020 */

void
//NOINLINEAT
display2_menu_value(
	uint_fast8_t x,
	uint_fast8_t y,
	int_fast32_t value,
	uint_fast8_t width,	// full width (if >= 128 - display with sign)
	uint_fast8_t comma,		// comma position (from right, inside width)
	uint_fast8_t rj,		// right truncated
	uint_fast8_t lowhalf
	)
{
	display_value_small(x, y, value, width, comma, 255, rj, lowhalf);
}

//+++ bars
		
static uint_fast8_t display_mapbar(
	uint_fast8_t val, 
	uint_fast8_t bottom, uint_fast8_t top, 
	uint_fast8_t mapleft, 
	uint_fast8_t mapinside, 
	uint_fast8_t mapright
	)
{
	if (val < bottom)
		return mapleft;
	if (val < top)
		return mapinside;
	return mapright;
}

#if WITHBARS

// количество точек в отображении мощности на диспле
static uint_fast16_t display_getpwrfullwidth(void)
{
	return GRID2X(CHARS2GRID(BDTH_ALLPWR));
}

#if LCDMODE_LTDC
	// Используеся frame buffer - свои оптимизированные функции рисования

#elif LCDMODE_HD44780
	// На HD44780 используется псевдографика

#elif LCDMODE_S1D13781 && ! LCDMODE_LTDC


#else /* LCDMODE_HD44780 */

// Вызовы этой функции (или группу вызовов) требуется "обрамить" парой вызовов
// display_wrdatabar_begin() и display_wrdatabar_end().
//
void 
//NOINLINEAT
display_bar(
	uint_fast16_t xpix,
	uint_fast16_t ypix,
	uint_fast8_t width,	/* количество знакомест, занимаемых индикатором */
	uint_fast8_t value,		/* значение, которое надо отобразить */
	uint_fast8_t tracevalue,		/* значение маркера, которое надо отобразить */
	uint_fast8_t topvalue,	/* значение, соответствующее полностью заполненному индикатору */
	uint_fast8_t vpattern,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL */
	uint_fast8_t vpatternmax,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL - для отображения запомненного значения */
	uint_fast8_t vemptyp			/* паттерн для заполнения между штрихами */
	)
{
	//enum { DISPLAY_BAR_LEVELS = 6 };	// количество градаций в одном знакоместе

	//value = value < 0 ? 0 : value;
	const uint_fast16_t wfull = GRID2X(width);
	const uint_fast16_t wpart = (uint_fast32_t) wfull * value / topvalue;
	const uint_fast16_t wmark = (uint_fast32_t) wfull * tracevalue / topvalue;
	uint_fast8_t i = 0;

	for (; i < wpart; ++ i)
	{
		if (i == wmark)
		{
			xpix = display_barcolumn(xpix, ypix, vpatternmax);
			continue;
		}
#if (DSTYLE_G_X132_Y64 || DSTYLE_G_X128_Y64) && DSTYLE_UR3LMZMOD
		xpix = display_barcolumn(xpix, ypix, vpattern);
#elif DSTYLE_G_X64_Y32
		xpix = display_barcolumn(xpix, ypix, (i % 6) != 5 ? vpattern : vemptyp);
#else
		xpix = display_barcolumn(xpix, ypix, (i % 2) == 0 ? vpattern : PATTERN_SPACE);
#endif
	}

	for (; i < wfull; ++ i)
	{
		if (i == wmark)
		{
			xpix = display_barcolumn(xpix, ypix, vpatternmax);
			continue;
		}
#if (DSTYLE_G_X132_Y64 || DSTYLE_G_X128_Y64) && DSTYLE_UR3LMZMOD
		xpix = display_barcolumn(xpix, ypix, vemptyp);
#elif DSTYLE_G_X64_Y32
		xpix = display_barcolumn(xpix, ypix, (i % 6) == 5 ? vpattern : vemptyp);
#else
		xpix = display_barcolumn(xpix, ypix, (i % 2) == 0 ? vemptyp : PATTERN_SPACE);
#endif
	}
}
#endif /* LCDMODE_HD44780 */

#endif /* WITHBARS */

// Адресация для s-meter
static uint_fast8_t
display_bars_x_rx(
	uint_fast8_t x, 
	uint_fast8_t xoffs	// grid
	)
{
	return x + xoffs;
}

// Адресация для swr-meter
static uint_fast8_t
display_bars_x_swr(
	uint_fast8_t x, 
	uint_fast8_t xoffs	// grid
	)
{
	return display_bars_x_rx(x, xoffs);
}

// Адресация для pwr-meter
static uint_fast8_t
display_bars_x_pwr(
	uint_fast8_t x, 
	uint_fast8_t xoffs	// grid
	)
{
#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
	return display_bars_x_rx(x, xoffs + CHARS2GRID(BDTH_ALLSWR + BDTH_SPACESWR));
#else
	return display_bars_x_rx(x, xoffs);
#endif
}

// координаьы для общего блока PWR & SWR
void display_swrmeter(  
	uint_fast8_t x, 
	uint_fast8_t y, 
	adcvalholder_t f,	// forward, 
	adcvalholder_t r,	// reflected (скорректированное)
	uint_fast16_t minforward
	)
{
#if WITHBARS

	// SWRMIN - значение 10 - соответствует SWR = 1.0, точность = 0.1
	// SWRMAX - какая цифра стоит в конце шкалы SWR-метра (30 = КСВ 3.0)
	const uint_fast16_t fullscale = SWRMAX - SWRMIN;
	uint_fast16_t swr10;		// рассчитанное  значение
	if (f < minforward)
		swr10 = 0;	// SWR=1
	else if (f <= r)
		swr10 = fullscale;		// SWR is infinite
	else
		swr10 = (f + r) * SWRMIN / (f - r) - SWRMIN;
	// v = 10..40 for swr 1..4
	// swr10 = 0..30 for swr 1..4
	const uint_fast8_t mapleftval = display_mapbar(swr10, 0, fullscale, 0, swr10, fullscale);

	//PRINTF(PSTR("swr10=%d, mapleftval=%d, fs=%d\n"), swr10, mapleftval, display_getmaxswrlimb());

	colmain_setcolors(SWRCOLOR, BGCOLOR);

	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdatabar_begin(display_bars_x_swr(x, CHARS2GRID(0)), y, & ypix);
	display_bar(xpix, ypix, BDTH_ALLSWR, mapleftval, fullscale, fullscale, PATTERN_BAR_FULL, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYFULL);
	display_wrdatabar_end();

	if (BDTH_SPACESWR != 0)
	{
		// заполняем пустое место за индикаторм КСВ
		uint_fast16_t ypix;
		uint_fast16_t xpix = display_wrdatabar_begin(display_bars_x_swr(x, CHARS2GRID(BDTH_ALLSWR)), y, & ypix);
		display_bar(xpix, ypix, BDTH_SPACESWR, 0, 1, 1, PATTERN_SPACE, PATTERN_SPACE, PATTERN_SPACE);
		display_wrdatabar_end();
	}

#endif /* WITHBARS */
}

#if CTLSTYLE_RA4YBO_AM0


// Вызывается из display2_bars_amv0 (версия для CTLSTYLE_RA4YBO_AM0)
// координаьы для общего блока PWR & SWR
// используется место для SWR
void display_modulationmeter_amv0(  
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t value,
	uint_fast8_t fullscale
	)
{
#if WITHBARS
	const uint_fast8_t mapleftval = display_mapbar(value, 0, fullscale, 0, value, fullscale);

	//PRINTF(PSTR("swr10=%d, mapleftval=%d, fs=%d\n"), swr10, mapleftval, display_getmaxswrlimb());

	colmain_setcolors(SWRCOLOR, BGCOLOR);
	display_at_P(display_bars_x_swr(x - 1, CHARS2GRID(0)), y, PSTR("M"));

	colmain_setcolors(SWRCOLOR, BGCOLOR);

	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdatabar_begin(stored_xcell, stored_ycell, & ypix);
	display_bar(xpix, ypix, BDTH_ALLSWR, mapleftval, fullscale, fullscale, PATTERN_BAR_FULL, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYFULL);
	display_wrdatabar_end();

	if (BDTH_SPACESWR != 0)
	{
		// заполняем пустое место за индикаторм КСВ
		display_bars_x_swr(x, y, CHARS2GRID(BDTH_ALLSWR));
		uint_fast16_t ypix;
		uint_fast16_t xpix = display_wrdatabar_begin(stored_xcell, stored_ycell, & ypix);
		display_bar(xpix, ypix, BDTH_SPACESWR, 0, 1, 1, PATTERN_SPACE, PATTERN_SPACE, PATTERN_SPACE);
		display_wrdatabar_end();
	}

#endif /* WITHBARS */
}
// координаьы для общего блока PWR & SWR
// Вызывается из display2_bars_amv0 (версия для CTLSTYLE_RA4YBO_AM0)
void display_pwrmeter_amv0(  
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t value,			// текущее значение
	uint_fast8_t tracemax,		// max hold значение
	uint_fast8_t maxpwrcali		// значение для отклонения на всю шкалу
	)
{
#if WITHBARS
	const uint_fast16_t fullscale = display_getpwrfullwidth();	// количество точек в отображении мощности на диспле
#if WITHPWRLIN
	uint_fast8_t v = (uint_fast32_t) value * fullscale / ((uint_fast32_t) maxpwrcali);
	uint_fast8_t t = (uint_fast32_t) tracemax * fullscale / ((uint_fast32_t) maxpwrcali);
#else /* WITHPWRLIN */
	uint_fast8_t v = (uint_fast32_t) value * value * fullscale / ((uint_fast32_t) maxpwrcali * maxpwrcali);
	uint_fast8_t t = (uint_fast32_t) tracemax * tracemax * fullscale / ((uint_fast32_t) maxpwrcali * maxpwrcali);
#endif /* WITHPWRLIN */
	const uint_fast8_t mapleftval = display_mapbar(v, 0, fullscale, 0, v, fullscale);
	const uint_fast8_t mapleftmax = display_mapbar(t, 0, fullscale, fullscale, t, fullscale); // fullscale - invisible

	colmain_setcolors(PWRCOLOR, BGCOLOR);
	display_bars_x_pwr(x - 1, y, CHARS2GRID(0));
	display_string_P(PSTR("P"), 0);

	display_bars_x_pwr(x, y, CHARS2GRID(0));

	colmain_setcolors(PWRCOLOR, BGCOLOR);

	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdatabar_begin(stored_xcell, stored_ycell, & ypix);
	display_bar(xpix, ypix, BDTH_ALLPWR, mapleftval, mapleftmax, fullscale, PATTERN_BAR_HALF, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYHALF);
	display_wrdatabar_end();

	if (BDTH_SPACEPWR != 0)
	{
		// заполняем пустое место за индикаторм мощности
		display_bars_x_pwr(x, y, CHARS2GRID(BDTH_ALLPWR));
		uint_fast16_t ypix;
		uint_fast16_t xpix = display_wrdatabar_begin(stored_xcell, stored_ycell, & ypix);
		display_bar(xpix, ypix, BDTH_SPACEPWR, 0, 1, 1, PATTERN_SPACE, PATTERN_SPACE, PATTERN_SPACE);
		display_wrdatabar_end();
	}

#endif /* WITHBARS */
}

void display_smeter_amv0(
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t value,		// текущее значение
	uint_fast8_t tracemax,	// метка запомненного максимума
	uint_fast8_t level9,	// s9 level
	uint_fast8_t delta1,	// s9 - s0 delta
	uint_fast8_t delta2)	// s9+50 - s9 delta
{
#if WITHBARS
	tracemax = value > tracemax ? value : tracemax;	// защита от рассогласования значений
	//delta1 = delta1 > level9 ? level9 : delta1;
	
	const uint_fast8_t leftmin = level9 - delta1;
	const uint_fast8_t mapleftval = display_mapbar(value, leftmin, level9, 0, value - leftmin, delta1);
	const uint_fast8_t mapleftmax = display_mapbar(tracemax, leftmin, level9, delta1, tracemax - leftmin, delta1); // delta1 - invisible
	const uint_fast8_t maprightval = display_mapbar(value, level9, level9 + delta2, 0, value - level9, delta2);
	const uint_fast8_t maprightmax = display_mapbar(tracemax, level9, level9 + delta2, delta2, tracemax - level9, delta2); // delta2 - invisible

	colmain_setcolors(LCOLOR, BGCOLOR);
	display_bars_x_rx(x - 1, y, CHARS2GRID(0));
	display_string_P(PSTR("S"), 0);

	display_bars_x_rx(x, y, CHARS2GRID(0));
	colmain_setcolors(LCOLOR, BGCOLOR);
	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdatabar_begin(stored_xcell, stored_ycell, & ypix);
	display_bar(xpix, ypix, BDTH_LEFTRX, mapleftval, mapleftmax, delta1, PATTERN_BAR_HALF, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYHALF);		//ниже 9 баллов ничего
	display_wrdatabar_end();
	//
	display_bars_x_rx(x, y, CHARS2GRID(BDTH_LEFTRX));
	colmain_setcolors(RCOLOR, BGCOLOR);
	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdatabar_begin(stored_xcell, stored_ycell, & ypix);
	display_bar(xpix, ypix, BDTH_RIGHTRX, maprightval, maprightmax, delta2, PATTERN_BAR_FULL, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYFULL);		// выше 9 баллов ничего нет.
	display_wrdatabar_end();

	if (BDTH_SPACERX != 0)
	{
		display_bars_x_pwr(x, y, CHARS2GRID(BDTH_ALLRX));
		uint_fast16_t ypix;
		uint_fast16_t xpix = display_wrdatabar_begin(stored_xcell, stored_ycell, & ypix);
		display_bar(xpix, ypix, BDTH_SPACERX, 0, 1, 1, PATTERN_SPACE, PATTERN_SPACE, PATTERN_SPACE);
		display_wrdatabar_end();
	}

#endif /* WITHBARS */
}
#endif /* CTLSTYLE_RA4YBO_AM0 */

// координаьы для общего блока PWR & SWR
void display_pwrmeter(  
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t value,			// текущее значение
	uint_fast8_t tracemax,		// max hold значение
	uint_fast8_t maxpwrcali		// значение для отклонения на всю шкалу
	)
{
#if WITHBARS
	const uint_fast16_t fullscale = display_getpwrfullwidth();	// количество точек в отображении мощности на диспле
#if WITHPWRLIN
	uint_fast8_t v = (uint_fast32_t) value * fullscale / ((uint_fast32_t) maxpwrcali);
	uint_fast8_t t = (uint_fast32_t) tracemax * fullscale / ((uint_fast32_t) maxpwrcali);
#else /* WITHPWRLIN */
	uint_fast8_t v = (uint_fast32_t) value * value * fullscale / ((uint_fast32_t) maxpwrcali * maxpwrcali);
	uint_fast8_t t = (uint_fast32_t) tracemax * tracemax * fullscale / ((uint_fast32_t) maxpwrcali * maxpwrcali);
#endif /* WITHPWRLIN */
	const uint_fast8_t mapleftval = display_mapbar(v, 0, fullscale, 0, v, fullscale);
	const uint_fast8_t mapleftmax = display_mapbar(t, 0, fullscale, fullscale, t, fullscale); // fullscale - invisible

	colmain_setcolors(PWRCOLOR, BGCOLOR);

	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdatabar_begin(display_bars_x_pwr(x, CHARS2GRID(0)), y, & ypix);
	display_bar(xpix, ypix, BDTH_ALLPWR, mapleftval, mapleftmax, fullscale, PATTERN_BAR_HALF, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYHALF);
	display_wrdatabar_end();

	if (BDTH_SPACEPWR != 0)
	{
		// заполняем пустое место за индикаторм мощности
		uint_fast16_t ypix;
		uint_fast16_t xpix = display_wrdatabar_begin(display_bars_x_pwr(x, CHARS2GRID(BDTH_ALLPWR)), y, & ypix);
		display_bar(xpix, ypix, BDTH_SPACEPWR, 0, 1, 1, PATTERN_SPACE, PATTERN_SPACE, PATTERN_SPACE);
		display_wrdatabar_end();
	}

#endif /* WITHBARS */
}

void display_smeter(
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t value,		// текущее значение
	uint_fast8_t tracemax,	// метка запомненного максимума
	uint_fast8_t level9,	// s9 level
	uint_fast8_t delta1,	// s9 - s0 delta
	uint_fast8_t delta2)	// s9+50 - s9 delta
{
#if WITHBARS
	tracemax = value > tracemax ? value : tracemax;	// защита от рассогласования значений
	//delta1 = delta1 > level9 ? level9 : delta1;
	
	const uint_fast8_t leftmin = level9 - delta1;
	const uint_fast8_t mapleftval = display_mapbar(value, leftmin, level9, 0, value - leftmin, delta1);
	const uint_fast8_t mapleftmax = display_mapbar(tracemax, leftmin, level9, delta1, tracemax - leftmin, delta1); // delta1 - invisible
	const uint_fast8_t maprightval = display_mapbar(value, level9, level9 + delta2, 0, value - level9, delta2);
	const uint_fast8_t maprightmax = display_mapbar(tracemax, level9, level9 + delta2, delta2, tracemax - level9, delta2); // delta2 - invisible

	colmain_setcolors(LCOLOR, BGCOLOR);
	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdatabar_begin(display_bars_x_rx(x, CHARS2GRID(0)), y, & ypix);
	display_bar(xpix, ypix, BDTH_LEFTRX, mapleftval, mapleftmax, delta1, PATTERN_BAR_HALF, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYHALF);		//ниже 9 баллов ничего
	display_wrdatabar_end();
	//
	colmain_setcolors(RCOLOR, BGCOLOR);
	uint_fast16_t ypix2;
	uint_fast16_t xpix2 = display_wrdatabar_begin(display_bars_x_rx(x, CHARS2GRID(BDTH_LEFTRX)), y, & ypix2);
	display_bar(xpix2, ypix2, BDTH_RIGHTRX, maprightval, maprightmax, delta2, PATTERN_BAR_FULL, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYFULL);		// выше 9 баллов ничего нет.
	display_wrdatabar_end();

	if (BDTH_SPACERX != 0)
	{
		uint_fast16_t ypix;
		uint_fast16_t xpix = display_wrdatabar_begin(display_bars_x_pwr(x, CHARS2GRID(BDTH_ALLRX)), y, & ypix);
		display_bar(xpix, ypix, BDTH_SPACERX, 0, 1, 1, PATTERN_SPACE, PATTERN_SPACE, PATTERN_SPACE);
		display_wrdatabar_end();
	}

#endif /* WITHBARS */
}
//--- bars

// Отображение шкалы S-метра и других измерителей
static void display2_legend_rx(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if defined(SMETERMAP)
	colmain_setcolors(LABELTEXT, LABELBACK);
	display_at_P(x, y, PSTR(SMETERMAP));
#endif /* defined(SMETERMAP) */
}

// Отображение шкалы SWR-метра и других измерителей
static void display2_legend_tx(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
#if defined(SWRPWRMAP) && WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR)
	colmain_setcolors(LABELTEXT, LABELBACK);
	#if WITHSWRMTR
		#if WITHSHOWSWRPWR /* на дисплее одновременно отображаются SWR-meter и PWR-meter */
				display_at_P(x, y, PSTR(SWRPWRMAP));
		#else
				if (swrmode) 	// Если TUNE то показываем шкалу КСВ
					display_string_P(x, y, PSTR(SWRMAP));
				else
					display_string_P(x, y, PSTR(POWERMAP));
		#endif
	#elif WITHPWRMTR
				display_string_P(x, y, PSTR(POWERMAP));
	#else
		#warning No TX indication
	#endif

#endif /* defined(SWRPWRMAP) && WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR) */
}


// Отображение шкалы S-метра и других измерителей
static void display2_legend(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	if (hamradio_get_tx())
		display2_legend_tx(x, y, pctx);
	else
		display2_legend_rx(x, y, pctx);
}


#if (WITHSPECTRUMWF && ! LCDMODE_HD44780 && ! LCDMODE_DUMMY) || WITHAFSPECTRE

enum 
{
	ALLDX = GRID2X(CHARS2GRID(BDTH_ALLRX)),	// размер по горизонтали в пикселях
	ALLDY = GRID2Y(BDCV_ALLRX),				// размер по вертикали в пикселях части отведенной водопаду
	WFDY = GRID2Y(BDCV_WFLRX),				// размер по вертикали в пикселях части отведенной водопаду
	WFY0 = GRID2Y(BDCO_WFLRX),				// смещение по вертикали в пикселях части отведенной водопаду
	SPDY = GRID2Y(BDCV_SPMRX),				// размер по вертикали в пикселях части отведенной спектру
	SPY0 = GRID2Y(BDCO_SPMRX)				// смещение по вертикали в пикселях части отведенной спектру
};

// Параметры фильтров данных спектра и водопада
// устанавливаются через меню
#define DISPLAY_SPECTRUM_BETA (0.25)
#define DISPLAY_WATERFALL_BETA (0.5)

static FLOAT_t spectrum_beta = (FLOAT_t) DISPLAY_SPECTRUM_BETA;					// incoming value coefficient
static FLOAT_t spectrum_alpha = 1 - (FLOAT_t) DISPLAY_SPECTRUM_BETA;	// old value coefficient

static FLOAT_t waterfall_beta = (FLOAT_t) DISPLAY_WATERFALL_BETA;					// incoming value coefficient
static FLOAT_t waterfall_alpha = 1 - (FLOAT_t) DISPLAY_WATERFALL_BETA;	// old value coefficient

#ifndef WITHFFTOVERLAPPOW2
	#define WITHFFTOVERLAPPOW2	(BOARD_FFTZOOM_POW2MAX + 1)	/* Количество перекрывающися буферов FFT спектра (2^param). */
#endif

enum
{
	NOVERLAP = 1 << WITHFFTOVERLAPPOW2,		// Количество перекрывающися буферов FFT спектра
	BOARD_FFTZOOM_MAX = (1 << BOARD_FFTZOOM_POW2MAX),

	LARGEFFT = WITHFFTSIZEWIDE * BOARD_FFTZOOM_MAX,	// размер буфера для децимации
	NORMALFFT = WITHFFTSIZEWIDE				// размер буфера для отображения
};


// параметры масштабирования спектра


// IIR filter before decimation
#define ZOOMFFT_DECIM_STAGES_IIR 9

// Дециматор для Zoom FFT
#define ZOOMFFT_DECIM_STAGES_FIR 4	// Maximum taps from all zooms


#if WITHAFSPECTRE

enum
{
	AFSP_DECIMATIONPOW2 = 1,		// x2
	AFSP_DECIMATION = (1 << AFSP_DECIMATIONPOW2)
};

typedef struct {
	float32_t raw_buf [WITHFFTSIZEAF * AFSP_DECIMATION];		// Для последующей децимации /2
	float32_t fft_buf [WITHFFTSIZEAF * 2];		// комплексные числа
	uint_fast8_t is_ready;
	uint_fast16_t x;
	uint_fast16_t y;
	uint_fast16_t w;
	uint_fast16_t h;
	float32_t max_val;
	float32_t val_array [DIM_X];
	arm_rfft_fast_instance_f32 rfft_instance;
} afsp_t;

#endif /* WITHAFSPECTRE */


#if defined (COLORPIP_SHADED)

	/* быстрое отображение водопада (но требует больше памяти) */
	enum { WFROWS = WFDY };
	enum { PALETTESIZE = COLORPIP_BASE };
	static PACKEDCOLORMAIN_T wfpalette [1];
	static uint_fast16_t wfrow;		// строка, в которую последней занесены данные

#elif WITHFASTWATERFLOW && LCDMODE_PIP_RGB565

	/* быстрое отображение водопада (но требует больше памяти) */
	enum { WFROWS = WFDY };
	enum { PALETTESIZE = 256 };
	static PACKEDCOLORMAIN_T wfpalette [PALETTESIZE];
	static uint_fast16_t wfrow;		// строка, в которую последней занесены данные

#elif LCDMODE_PIP_L8 || (! LCDMODE_PIP_L8 && LCDMODE_MAIN_L8)

	enum { PALETTESIZE = COLORPIP_BASE };
	enum { WFROWS = WFDY };
	static uint_fast16_t wfrow;		// строка, в которую последней занесены данные

#elif WITHFASTWATERFLOW && 1

	/* быстрое отображение водопада (но требует больше памяти) */
	enum { WFROWS = WFDY };
	// https://www.mischianti.org/images-to-byte-array-online-converter-cpp-arduino/
	const unsigned long pancolor [] FLASHMEM = {
		0x00891127,
		0x008a1127,
		0x008c1227,
		0x008c1226,
		0x008d1226,
		0x008e1326,
		0x008f1326,
		0x00911325,
		0x00921425,
		0x00921425,
		0x00931426,
		0x00951426,
		0x00961425,
		0x00971525,
		0x00981525,
		0x00991524,
		0x009a1524,
		0x009c1624,
		0x009d1624,
		0x009e1724,
		0x009f1723,
		0x00a01723,
		0x00a11722,
		0x00a31822,
		0x00a41823,
		0x00a51823,
		0x00a71924,
		0x00a81924,
		0x00a91923,
		0x00a91922,
		0x00ab1a22,
		0x00ac1a23,
		0x00ae1a23,
		0x00af1b23,
		0x00af1b23,
		0x00b11b23,
		0x00b21b23,
		0x00b31c22,
		0x00b51c23,
		0x00b61c23,
		0x00b71d23,
		0x00b81d22,
		0x00b91d23,
		0x00bb1d23,
		0x00bb1e23,
		0x00bd1e23,
		0x00be1e24,
		0x00bf1e23,
		0x00c01f23,
		0x00c11f23,
		0x00c21f23,
		0x00c42023,
		0x00c52023,
		0x00c62023,
		0x00c72023,
		0x00c72024,
		0x00c82024,
		0x00ca2124,
		0x00cb2124,
		0x00cd2124,
		0x00cd2124,
		0x00cd2224,
		0x00ce2224,
		0x00cf2224,
		0x00d02224,
		0x00d22324,
		0x00d32325,
		0x00d42325,
		0x00d52326,
		0x00d62425,
		0x00d72426,
		0x00d82426,
		0x00d82426,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92425,
		0x00d92525,
		0x00d92525,
		0x00d92525,
		0x00d92525,
		0x00d92525,
		0x00d92525,
		0x00d92525,
		0x00d92525,
		0x00d92525,
		0x00d92525,
		0x00d92525,
		0x00d92524,
		0x00d92524,
		0x00d92524,
		0x00d92524,
		0x00d92524,
		0x00d92524,
		0x00d92524,
		0x00d92524,
		0x00d92524,
		0x00d92524,
		0x00d92524,
		0x00d82424,
		0x00d82424,
		0x00d82424,
		0x00d82424,
		0x00d82424,
		0x00d82424,
		0x00d82424,
		0x00d82424,
		0x00d82424,
		0x00d82424,
		0x00d82424,
		0x00d82424,
		0x00d92524,
		0x00d82524,
		0x00d82524,
		0x00d92524,
		0x00d92524,
		0x00d92524,
		0x00d92624,
		0x00d92624,
		0x00d92624,
		0x00d82724,
		0x00d72724,
		0x00d82824,
		0x00d92825,
		0x00d92925,
		0x00d92925,
		0x00d92b25,
		0x00d92b25,
		0x00d92c24,
		0x00d92e25,
		0x00d92f25,
		0x00d93025,
		0x00d93225,
		0x00d93225,
		0x00d93425,
		0x00d93525,
		0x00d93825,
		0x00d93925,
		0x00d93b25,
		0x00da3d26,
		0x00da3f26,
		0x00d94126,
		0x00d94126,
		0x00da4426,
		0x00da4627,
		0x00da4727,
		0x00da4927,
		0x00da4b27,
		0x00db4d27,
		0x00db5027,
		0x00db5228,
		0x00db5428,
		0x00db5728,
		0x00dc5928,
		0x00db5b29,
		0x00db5e29,
		0x00db6029,
		0x00dc6329,
		0x00dc662a,
		0x00dc672a,
		0x00dc6a2a,
		0x00dd6c2a,
		0x00dd6e2b,
		0x00de702b,
		0x00de742c,
		0x00df772c,
		0x00df782c,
		0x00de7a2d,
		0x00de7c2d,
		0x00df7f2d,
		0x00df812d,
		0x00df842e,
		0x00e0862e,
		0x00e0892f,
		0x00df8a2f,
		0x00e18d2f,
		0x00e29030,
		0x00e29230,
		0x00e29431,
		0x00e39731,
		0x00e39931,
		0x00e49c32,
		0x00e49f32,
		0x00e4a033,
		0x00e5a334,
		0x00e5a534,
		0x00e6a835,
		0x00e6aa35,
		0x00e7ac35,
		0x00e7af36,
		0x00e7b136,
		0x00e8b437,
		0x00e8b637,
		0x00e9b837,
		0x00e9bb38,
		0x00e9bd38,
		0x00e9bf39,
		0x00ebc139,
		0x00ebc43a,
		0x00ecc63b,
		0x00ecc83b,
		0x00edca3b,
		0x00edcc3c,
		0x00eecf3c,
		0x00edd03c,
		0x00eed33d,
		0x00eed53d,
		0x00efd73e,
		0x00f0da3f,
		0x00f0db3f,
		0x00f1dd3f,
		0x00f1df40,
		0x00f2e140,
		0x00f2e340,
		0x00f3e541,
		0x00f3e641,
		0x00f3e841,
		0x00f4ea42,
		0x00f5ec42,
		0x00f5ed43,
		0x00f5ee43,
		0x00f6f043,
		0x00f6f244,
		0x00f7f444,
		0x00f6f544,
		0x00f7f745,
		0x00f7f845,
		0x00f6f945,
		0x00f8fb46,
		0x00f8fd46,
		0x00f8fd46,
		0x00f9fe46,
		0x00f9ff47,
		0x00faff47,
		0x00fbff48,
		0x00faff48,
		0x00faff48,
		0x00faff48,
		0x00f9ff48,
		0x00f9ff48,
		0x00f8ff48,
		0x00f7ff47,
		0x00f7ff48,
		0x00f6ff48,
		0x00f4ff47,
		0x00f3ff47,
		0x00f1ff47,
		0x00f0ff47,
		0x00efff47,
		0x00efff47,
		0x00eeff47,
		0x00ecff46,
		0x00eaff46,
		0x00e9ff46,
		0x00e7ff46,
		0x00e6ff45,
		0x00e5ff45,
		0x00e2ff45,
		0x00e0ff45,
		0x00deff45,
		0x00ddff44,
		0x00dcff44,
		0x00dbff44,
		0x00d9ff44,
		0x00d6ff44,
		0x00d4ff43,
		0x00d2ff43,
		0x00d0ff43,
		0x00cfff43,
		0x00cdff43,
		0x00cbff43,
		0x00c9ff42,
		0x00c7ff42,
		0x00c5ff42,
		0x00c3ff42,
		0x00c1ff42,
		0x00bfff41,
		0x00beff41,
		0x00bcff41,
		0x00b9ff41,
		0x00b6ff41,
		0x00b4ff40,
		0x00b2ff40,
		0x00b0ff40,
		0x00aeff40,
		0x00acff40,
		0x00aaff3f,
		0x00a8ff3f,
		0x00a6ff3f,
		0x00a4ff3f,
		0x00a1ff3f,
		0x009fff3e,
		0x009dff3e,
		0x009aff3e,
		0x0098ff3e,
		0x0096ff3e,
		0x0094ff3e,
		0x0092ff3e,
		0x0090ff3d,
		0x0090ff3e,
		0x008eff3d,
		0x008eff3d,
		0x008eff3d,
		0x008fff3d,
		0x008fff3d,
		0x008fff3d,
		0x008fff3d,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3d,
		0x008fff3d,
		0x008fff3d,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3d,
		0x008fff3d,
		0x008fff3d,
		0x008fff3d,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3e,
		0x008fff3d,
		0x008fff3d,
		0x008fff3d,
		0x008fff3f,
		0x008fff41,
		0x008fff43,
		0x008fff45,
		0x008fff47,
		0x008fff49,
		0x008eff4b,
		0x008eff4e,
		0x008fff4f,
		0x0090ff51,
		0x0090ff52,
		0x0090ff55,
		0x0090ff58,
		0x008fff59,
		0x0090ff5b,
		0x0090ff5d,
		0x0090ff5f,
		0x0090ff62,
		0x008fff64,
		0x0090ff66,
		0x0090ff69,
		0x0090ff6b,
		0x0090ff6d,
		0x0090ff6f,
		0x0090ff71,
		0x0090ff73,
		0x0090ff75,
		0x0091ff78,
		0x0091ff7a,
		0x0091ff7d,
		0x0091ff7f,
		0x0091ff80,
		0x0091ff83,
		0x0091ff86,
		0x0091ff89,
		0x0091ff8b,
		0x0091ff8c,
		0x0091ff8d,
		0x0091ff90,
		0x0091ff92,
		0x0091ff95,
		0x0092ff97,
		0x0092ff98,
		0x0092ff9b,
		0x0092ff9d,
		0x0092ff9f,
		0x0092ffa1,
		0x0092ffa4,
		0x0092ffa6,
		0x0092ffa8,
		0x0092ffaa,
		0x0092ffac,
		0x0093ffae,
		0x0093ffb0,
		0x0093ffb2,
		0x0093ffb4,
		0x0093ffb6,
		0x0093ffb7,
		0x0093ffb8,
		0x0093ffbb,
		0x0093ffbe,
		0x0093ffc1,
		0x0094ffc3,
		0x0094ffc4,
		0x0094ffc5,
		0x0094ffc8,
		0x0093ffca,
		0x0093ffcd,
		0x0094ffce,
		0x0094ffce,
		0x0094ffcf,
		0x0094ffd2,
		0x0094ffd5,
		0x0095ffd6,
		0x0095ffd7,
		0x0095ffd7,
		0x0094fed9,
		0x0094fedc,
		0x0095fedf,
		0x0095fee0,
		0x0095fee2,
		0x0095fde3,
		0x0095fde4,
		0x0096fee5,
		0x0096fee7,
		0x0096fee8,
		0x0096fee9,
		0x0096feea,
		0x0096feec,
		0x0096feed,
		0x0096feee,
		0x0096feef,
		0x0096fdf0,
		0x0096fef2,
		0x0096fef4,
		0x0096fef5,
		0x0096fcf6,
		0x0096fcf7,
		0x0096fcf8,
		0x0096fcfa,
		0x0096fcfb,
		0x0096fcfb,
		0x0096fcfc,
		0x0097fdfd,
		0x0097fcfd,
		0x0096fbfd,
		0x0095fafd,
		0x0095f9fd,
		0x0094f8fd,
		0x0094f7fc,
		0x0093f5fc,
		0x0092f4fc,
		0x0092f4fc,
		0x0091f2fc,
		0x0090f0fc,
		0x0090effc,
		0x008fedfc,
		0x008eebfb,
		0x008deafb,
		0x008de9fb,
		0x008ce7fc,
		0x008be5fc,
		0x008ae3fc,
		0x0088e0fb,
		0x0087defb,
		0x0087ddfb,
		0x0086dbfc,
		0x0085dafc,
		0x0084d8fc,
		0x0083d6fb,
		0x0081d3fb,
		0x0081d1fb,
		0x0080cffa,
		0x007fcdfa,
		0x007ecbfa,
		0x007dc9fa,
		0x007cc7fb,
		0x007ac4fb,
		0x0079c1fb,
		0x0079c0fb,
		0x0077bdfb,
		0x0076bbfb,
		0x0075b9fa,
		0x0073b5fa,
		0x0072b3fa,
		0x0071b0fa,
		0x0070aefa,
		0x006facfa,
		0x006ea9fa,
		0x006da7fa,
		0x006ba4fa,
		0x006aa2fa,
		0x00699efa,
		0x00689cfa,
		0x006699fa,
		0x006597fa,
		0x006493f9,
		0x006391f9,
		0x00618df9,
		0x00608af9,
		0x005f88f9,
		0x005d84f9,
		0x005c82f9,
		0x005b80f9,
		0x005a7bf9,
		0x005979f8,
		0x005876f9,
		0x005774f9,
		0x005671f9,
		0x00556ff9,
		0x00546bf8,
		0x005369f8,
		0x005165f9,
		0x005061f9,
		0x005060f9,
		0x004f5cf8,
		0x004d58f8,
		0x004c54f8,
		0x004b51f8,
		0x004a4df8,
		0x004949f8,
		0x004844f8,
		0x004741f8,
		0x00463cf8,
		0x004539f7,
		0x004535f8,
		0x004430f8,
		0x00432af8,
		0x004225f8,
		0x004120f8,
		0x004019f7,
		0x003f12f7,
		0x003e02f7,
		0x003e00f7,
		0x003d00f8,
		0x003d00f8,
		0x003c00f8,
		0x003b00f7,
		0x003a00f6,
		0x003a00f7,
		0x003900f7,
		0x003900f7,
		0x003800f8,
		0x003800f8,
		0x003800f8,
		0x003700f8,
		0x003700f8,
		0x003600f8,
		0x003600f8,
		0x003600f8,
		0x003500f8,
		0x003500f8,
		0x003500f8,
		0x003500f8,
		0x003500f8,
		0x003500f8,
		0x003400f8,
		0x003400f8,
		0x003400f8,
		0x003300f7,
		0x003400f8,
		0x003400f8,
		0x003400f8,
		0x003400f8,
		0x003300f8,
		0x003300f7,
		0x003300f7,
		0x003300f6,
		0x003300f6,
		0x003300f7,
		0x003300f7,
		0x003300f7,
		0x003300f6,
		0x003300f6,
		0x003300f5,
		0x003300f5,

	};
	enum { PALETTESIZE = ARRAY_SIZE(pancolor) };
	static PACKEDCOLORMAIN_T wfpalette [PALETTESIZE];
	static uint_fast16_t wfrow;		// строка, в которую последней занесены данные

#elif WITHFASTWATERFLOW

	/* быстрое отображение водопада (но требует больше памяти) */
	enum { WFROWS = WFDY };
	enum { PALETTESIZE = 256 };

	static PACKEDCOLORMAIN_T wfpalette [PALETTESIZE];
	static uint_fast16_t wfrow;		// строка, в которую последней занесены данные

#elif (! LCDMODE_S1D13781_NHWACCEL && LCDMODE_S1D13781)

	enum { WFROWS = 1 };
	enum { wfrow = 0 };				// строка, в которую последней занесены данные

	enum { PALETTESIZE = 256 };
	static RAMBIGDTCM PACKEDCOLOR565_T wfpalette [PALETTESIZE];

#elif LCDMODE_PIP_L8

#else

	enum { WFROWS = WFDY };
	static uint_fast16_t wfrow;		// строка, в которую последней занесены данные

	enum { PALETTESIZE = 256 };
	static RAMBIGDTCM PACKEDCOLOR565_T wfpalette [PALETTESIZE];

#endif


#if WITHVIEW_3DSS
enum {
#if CPUSTYLE_XC7Z
	MAX_3DSS_STEP = WFROWS,
	Y_STEP = 1,
	DEPTH_ATTENUATION = 4,
#else
	MAX_3DSS_STEP = 42,
	Y_STEP = 2,
	DEPTH_ATTENUATION = 2,
#endif /* #if CPUSTYLE_XC7Z */
	MAX_DELAY_3DSS = 1,
	HALF_ALLDX = ALLDX / 2,
	SPY_3DSS = SPDY,
	SPY_3DSS_H = SPY_3DSS / 4
};

#endif /* WITHVIEW_3DSS */

struct ustates
{
#if defined(ARM_MATH_NEON)
	float32_t iir_state [ZOOMFFT_DECIM_STAGES_IIR * 8];
#else /* defined(ARM_MATH_NEON) */
	float32_t iir_state [ZOOMFFT_DECIM_STAGES_IIR * 4];
#endif /* defined(ARM_MATH_NEON) */
	float32_t fir_state [ZOOMFFT_DECIM_STAGES_FIR + LARGEFFT - 1];

	float32_t cmplx_sig [NORMALFFT * 2];
	float32_t ifspec_wndfn [NORMALFFT];

	FLOAT_t spavgarray [ALLDX];	// массив входных данных для отображения (через фильтры).
	FLOAT_t Yold_wtf [ALLDX];
	FLOAT_t Yold_spe [ALLDX];
	PACKEDCOLORMAIN_T wfjarray [GXSIZE(ALLDX, WFROWS)];	// массив "водопада"
	PACKEDCOLORMAIN_T color_scale [SPDY];	/* массив значений для раскраски спектра */

#if WITHAFSPECTRE
	float32_t afspec_wndfn [WITHFFTSIZEAF];
	afsp_t afsp;
#endif /* WITHAFSPECTRE */

#if WITHVIEW_3DSS
	uint16_t depth_map_3dss [MAX_3DSS_STEP][ALLDX];
	uint16_t envelope_y [ALLDX];
#endif /* WITHVIEW_3DSS */
};

union states
{
	struct ustates data;
	uint16_t rbfimage_dummy [1];	// для предотвращения ругани компилятора на приведение типов
};


#if (CPUSTYLE_R7S721 || 0)

static uint16_t rbfimage0 [] =
{
#include BOARD_BITIMAGE_NAME
};

/* получить расположение в памяти и количество элементов в массиве для загрузки FPGA */
const uint16_t * getrbfimage(size_t * count)
{
	ASSERT(sizeof rbfimage0 >= sizeof (union states));

	* count = sizeof rbfimage0 / sizeof rbfimage0 [0];
	return & rbfimage0 [0];
}

#define gvars ((* (union states *) rbfimage0).data)

#else /* (CPUSTYLE_R7S721 || 0) */

static RAMBIGDTCM struct ustates gvars;

#endif /* (CPUSTYLE_R7S721 || 0) */


#if (WITHSPECTRUMWF && ! LCDMODE_HD44780 && ! LCDMODE_DUMMY) || WITHAFSPECTRE

static void printsigwnd(void)
{
	int i;

	PRINTF(PSTR("static const FLASHMEM FLOAT_t gvars.ifspec_wndfn [%u] =\n"), (unsigned) NORMALFFT);
	PRINTF(PSTR("{\n"));
	for (i = 0; i < NORMALFFT; ++ i)
	{
		int el = ((i + 1) % 4) == 0;
		PRINTF(PSTR("\t" "%+1.20f%s"), gvars.ifspec_wndfn [i], el ? ",\n" : ", ");
	}
	PRINTF(PSTR("};\n"));
}

#endif /*  (WITHSPECTRUMWF && ! LCDMODE_HD44780 && ! LCDMODE_DUMMY) || WITHAFSPECTRE */

#if WITHAFSPECTRE

// перевести частоту в позицию бина результата FFT децимированного спектра
static int freq2fft_af(int freq)
{
	return AFSP_DECIMATION * freq * WITHFFTSIZEAF / dsp_get_samplerateuacin_audio48();
}

// перевод позиции в окне в номер бина - отображение с нулевой частотой в левой стороне окна
static int raster2fftsingle(
	int x,	// window pos
	int dx,	// width
	int leftfft,	// начало буфера FFT, отобрааемое на экране (в бинах)
	int rightfft	// последний бин буфера FFT, отобрааемый на экране (включитеоьно)
	)
{
	ASSERT(leftfft < rightfft);
	return x * (rightfft - leftfft) / dx + leftfft;
}

// user-mode context function
static void
afsp_save_sample(void * ctx, FLOAT_t ch0, FLOAT_t ch1)
{
	static uint_fast16_t i = 0;

	ASSERT(i < ARRAY_SIZE(gvars.afsp.raw_buf));
	if (gvars.afsp.is_ready == 0)
	{
		gvars.afsp.raw_buf [i] = ch0;
		i ++;
	}

	if (i >= ARRAY_SIZE(gvars.afsp.raw_buf))
	{
		gvars.afsp.is_ready = 1;
		i = 0;
	}
}

static void
display2_af_spectre15_init(uint_fast8_t xgrid, uint_fast8_t ygrid, dctx_t * pctx)		// вызывать после display2_smeter15_init
{
	static subscribefloat_t afspectreregister;
	smeter_params_t * const smpr = & smprms [SMETER_TYPE_BARS];		// отображение НЧ спектра только для режима s-метра BARS

	gvars.afsp.x = GRID2X(xgrid) + smpr->gs;
	gvars.afsp.y = GRID2Y(ygrid) + SM_BG_H - 10;
	gvars.afsp.w = smpr->ge - smpr->gs;
	gvars.afsp.h = 40;
	gvars.afsp.is_ready = 0;

	VERIFY(ARM_MATH_SUCCESS == arm_rfft_fast_init_f32(& gvars.afsp.rfft_instance, WITHFFTSIZEAF));

	int i;
	for (i = 0; i < WITHFFTSIZEAF; ++ i)
	{
		gvars.afspec_wndfn [i] = fir_design_window(i, WITHFFTSIZEAF, BOARD_WTYPE_SPECTRUM);
	}
#if 0 && CTLSTYLE_V3D
	// делать так не стоит - afsp_save_sample функция работающая в user mode, из real time контекста её вызывать нельзя
	// возможно нужен "переходник", выкачивающий из real time очереди для показа спектра
	subscribefloat_user(& afdemodoutfloat_rt, & afspectreregister, NULL, afsp_save_sample);
#else
	subscribefloat_user(& speexoutfloat_user, & afspectreregister, NULL, afsp_save_sample);	// выход sppeex и фильтра
#endif /* CTLSTYLE_V3D */
}


static void
display2_af_spectre15_latch(uint_fast8_t xgrid, uint_fast8_t ygrid, dctx_t * pctx)
{
	if (gvars.afsp.is_ready)
	{
		const unsigned leftfftpos = freq2fft_af(glob_afspeclow);	// нижняя частота (номер бина) отлбражаемая на экране
		const unsigned rightfftpos = freq2fft_af(glob_afspechigh);	// последний бин буфера FFT, отобрааемый на экране (включитеоьно)

		fftzoom_af(gvars.afsp.raw_buf, AFSP_DECIMATIONPOW2, WITHFFTSIZEAF);
		// осталась половина буфера

		arm_mult_f32(gvars.afsp.raw_buf, gvars.afspec_wndfn, gvars.afsp.raw_buf, WITHFFTSIZEAF); // apply window function
		arm_rfft_fast_f32(& gvars.afsp.rfft_instance, gvars.afsp.raw_buf, gvars.afsp.fft_buf, 0); // 0-прямое, 1-обратное
		gvars.afsp.is_ready = 0;	// буфер больше не нужен... но он заполняется так же в user mode
		arm_cmplx_mag_f32(gvars.afsp.fft_buf, gvars.afsp.fft_buf, WITHFFTSIZEAF);

		ASSERT(gvars.afsp.w <= ARRAY_SIZE(gvars.afsp.val_array));
		for (unsigned x = 0; x < gvars.afsp.w; x ++)
		{
			const uint_fast16_t fftpos = raster2fftsingle(x, gvars.afsp.w, leftfftpos, rightfftpos);
			ASSERT(fftpos < ARRAY_SIZE(gvars.afsp.fft_buf));
			// filterig
			gvars.afsp.val_array [x] = gvars.afsp.val_array [x] * (FLOAT_t) 0.6 + (FLOAT_t) 0.4 * gvars.afsp.fft_buf [fftpos];
		}
		arm_max_no_idx_f32(gvars.afsp.val_array, gvars.afsp.w, & gvars.afsp.max_val);	// поиск в отображаемой части
		gvars.afsp.max_val = FMAXF(gvars.afsp.max_val, (FLOAT_t) 0.001);
	}
}

static void
display2_af_spectre15(uint_fast8_t xgrid, uint_fast8_t ygrid, dctx_t * pctx)
{
	switch (glob_smetertype)
	{
	case SMETER_TYPE_BARS:
		{
			if (! hamradio_get_tx())
			{
				PACKEDCOLORMAIN_T * const fr = colmain_fb_draw();

				ASSERT(gvars.afsp.w <= ARRAY_SIZE(gvars.afsp.val_array));
				for (unsigned x = 0; x < gvars.afsp.w; x ++)
				{
					//const uint_fast16_t y_norm = normalize(gvars.afsp.val_array [x], 0, gvars.afsp.max_val, gvars.afsp.h - 2) + 1;
					const uint_fast16_t y_norm = normalize(gvars.afsp.val_array [x] * 4096, 0, gvars.afsp.max_val * 4096, gvars.afsp.h - 2) + 1;
					ASSERT(y_norm <= gvars.afsp.h);
					ASSERT(gvars.afsp.y >= y_norm);
					if (gvars.afsp.y >= y_norm)
					{
						display_colorbuf_set_vline(fr, DIM_X, DIM_Y,
								gvars.afsp.x + x, gvars.afsp.y - y_norm, y_norm,
								AFSPECTRE_COLOR);
					}
				}
			}
		}
		break;

	default:
		break;
	}
}

#endif /* WITHAFSPECTRE */

static FLOAT_t filter_waterfall(
	uint_fast16_t x
	)
{
	ASSERT(x < ARRAY_SIZE(gvars.spavgarray));
	ASSERT(x < ARRAY_SIZE(gvars.Yold_wtf));
	const FLOAT_t val = gvars.spavgarray [x];
	const FLOAT_t Y = gvars.Yold_wtf [x] * waterfall_alpha + waterfall_beta * val;
	gvars.Yold_wtf [x] = Y;
	return Y;
}

static FLOAT_t filter_spectrum(
	uint_fast16_t x
	)
{
	ASSERT(x < ARRAY_SIZE(gvars.spavgarray));
	ASSERT(x < ARRAY_SIZE(gvars.Yold_spe));
	const FLOAT_t val = gvars.spavgarray [x];
	const FLOAT_t Y = gvars.Yold_spe [x] * spectrum_alpha + spectrum_beta * val;
	gvars.Yold_spe [x] = Y;
	return Y;
}

/* парамеры видеофильтра спектра */
void display2_set_filter_spe(uint_fast8_t v)
{
	ASSERT(v <= 100);
	const FLOAT_t val = (int) v / (FLOAT_t) 100;
	spectrum_beta = val;
	spectrum_alpha = 1 - val;
}

/* парамеры видеофильтра водопада */
void display2_set_filter_wtf(uint_fast8_t v)
{
	ASSERT(v <= 100);
	const FLOAT_t val = (int) v / (FLOAT_t) 100;
	waterfall_beta = val;
	waterfall_alpha = 1 - val;
}

// Поддержка панорпамы и водопада



// параметры масштабирования спектра

struct zoom_param
{
	unsigned zoom;
	unsigned numTaps;
	const float32_t * pFIRCoeffs;
	const float32_t * pIIRCoeffs;
};

static const struct zoom_param zoom_params [] =
{
	// x2 zoom lowpass
	{
		.zoom = 2,
		.numTaps = ZOOMFFT_DECIM_STAGES_FIR,
		.pFIRCoeffs = (const float32_t [ZOOMFFT_DECIM_STAGES_FIR]){-0.05698952454792, 0.5574889164132, 0.5574889164132, -0.05698952454792},
		.pIIRCoeffs = (const float32_t [ZOOMFFT_DECIM_STAGES_IIR * 5]){0.8384843639921, 0, 0, 0, 0, 1, 0.5130084793341, 1, 0.1784114407685, -0.6967733943344, 0.8744089756375, 0, 0, 0, 0, 1, 1.046379755684, 1, 0.3420998857106, -0.3982809814397, 1.83222755502, 0, 0, 0, 0, 1, 1.831496024383, 1, 0.5072844084012, -0.1179052535088, 0.01953722920982, 0, 0, 0, 0, 1, 0.3029841730578, 1, 0.09694668293684, -0.9095549467394, 1, 0, 0, 0, 0},
	},
	// x4 zoom lowpass
	{
		.zoom = 4,
		.numTaps = ZOOMFFT_DECIM_STAGES_FIR,
		.pFIRCoeffs = (const float32_t [ZOOMFFT_DECIM_STAGES_FIR]){-0.05698952454792, 0.5574889164132, 0.5574889164132, -0.05698952454792},
		.pIIRCoeffs = (const float32_t [ZOOMFFT_DECIM_STAGES_IIR * 5]){0.6737499659657, 0, 0, 0, 0, 1, -1.102065194995, 1, 1.353694541279, -0.7896377861467, 0.53324811147, 0, 0, 0, 0, 1, -0.5853766477218, 1, 1.289175897987, -0.5882714065646, 0.6143152247695, 0, 0, 0, 0, 1, 1.182778527244, 1, 1.236309127239, -0.4063767082903, 0.01708381580242, 0, 0, 0, 0, 1, -1.245590418009, 1, 1.418191929315, -0.9374008035325, 1, 0, 0, 0, 0},
	},
	// x8 zoom lowpass
	{
		.zoom = 8,
		.numTaps = ZOOMFFT_DECIM_STAGES_FIR,
		.pFIRCoeffs = (const float32_t [ZOOMFFT_DECIM_STAGES_FIR]){-0.05698952454792, 0.5574889164132, 0.5574889164132, -0.05698952454792},
		.pIIRCoeffs = (const float32_t [ZOOMFFT_DECIM_STAGES_IIR * 5]){0.6469981129046, 0, 0, 0, 0, 1, -1.750671284068, 1, 1.766710155669, -0.8829517893283, 0.4645312725883, 0, 0, 0, 0, 1, -1.553480572725, 1, 1.681513354365, -0.7637556184482, 0.2925692260954, 0, 0, 0, 0, 1, -0.1114766808264, 1, 1.601891439147, -0.6499504503566, 0.01652325734055, 0, 0, 0, 0, 1, -1.797298202754, 1, 1.831125104215, -0.9660534813317, 1, 0, 0, 0, 0},
	},
	// x16 zoom lowpass
	{
		.zoom = 16,
		.numTaps = ZOOMFFT_DECIM_STAGES_FIR,
		.pFIRCoeffs = (const float32_t [ZOOMFFT_DECIM_STAGES_FIR]){-0.05698952454792, 0.5574889164132, 0.5574889164132, -0.05698952454792},
		.pIIRCoeffs = (const float32_t [ZOOMFFT_DECIM_STAGES_IIR * 5]){0.6500044972642, 0, 0, 0, 0, 1, -1.935616780918, 1, 1.908632776595, -0.9387888949475, 0.4599444315799, 0, 0, 0, 0, 1, -1.880017827578, 1, 1.851418291083, -0.8732990221737, 0.2087317940803, 0, 0, 0, 0, 1, -1.278402634611, 1, 1.794539349192, -0.80764043772, 0.01645106748385, 0, 0, 0, 0, 1, -1.948135342532, 1, 1.948194658987, -0.9825675157696, 1, 0, 0, 0, 0},
	},
};

// Сэмплы для децимации
typedef struct fftbuff_tag
{
	LIST_ENTRY item;
	float32_t largebuffI [LARGEFFT];
	float32_t largebuffQ [LARGEFFT];
} fftbuff_t;

static LIST_ENTRY fftbuffree;
static LIST_ENTRY fftbufready;
static SPINLOCK_t fftlock;

// realtime-mode function
uint_fast8_t allocate_fftbuffer_low(fftbuff_t * * dest)
{
	SPIN_LOCK(& fftlock);
	if (! IsListEmpty(& fftbuffree))
	{
		const PLIST_ENTRY t = RemoveTailList(& fftbuffree);
		SPIN_UNLOCK(& fftlock);
		fftbuff_t * const p = CONTAINING_RECORD(t, fftbuff_t, item);
		* dest = p;
		return 1;
	}
	/* Начинаем отбрасывать самые старые в очереди готовых. */
	if (! IsListEmpty(& fftbufready))
	{
		const PLIST_ENTRY t = RemoveTailList(& fftbufready);
		SPIN_UNLOCK(& fftlock);
		fftbuff_t * const p = CONTAINING_RECORD(t, fftbuff_t, item);
		* dest = p;
		return 1;
	}
	return 0;
}

// realtime-mode function
void saveready_fftbuffer_low(fftbuff_t * p)
{
	SPIN_LOCK(& fftlock);
	while (! IsListEmpty(& fftbufready))
	{
		const PLIST_ENTRY t = RemoveTailList(& fftbufready);
		InsertHeadList(& fftbuffree, t);
	}
	InsertHeadList(& fftbufready, & p->item);
	SPIN_UNLOCK(& fftlock);
}

// user-mode function
void release_fftbuffer(fftbuff_t * p)
{
	global_disableIRQ();
	SPIN_LOCK(& fftlock);
	InsertHeadList(& fftbuffree, & p->item);
	SPIN_UNLOCK(& fftlock);
	global_enableIRQ();
}

// user-mode function
uint_fast8_t  getfilled_fftbuffer(fftbuff_t * * dest)
{
	global_disableIRQ();
	SPIN_LOCK(& fftlock);
	if (! IsListEmpty(& fftbufready))
	{
		const PLIST_ENTRY t = RemoveTailList(& fftbufready);
		SPIN_UNLOCK(& fftlock);
		global_enableIRQ();
		fftbuff_t * const p = CONTAINING_RECORD(t, fftbuff_t, item);
		* dest = p;
		return 1;
	}
	SPIN_UNLOCK(& fftlock);
	global_enableIRQ();
	return 0;
}

static fftbuff_t * pfill [NOVERLAP];
static unsigned filleds [NOVERLAP]; // 0..LARGEFFT-1

// сохранение сэмпла для отображения спектра
static void
saveIQRTSxx(void * ctx, int_fast32_t iv, int_fast32_t qv)
{
	const FLOAT_t ivf = adpt_input(& ifspectrumin, qv);	// нормализованное к -1..+1
	const FLOAT_t qvf = adpt_input(& ifspectrumin, iv);	// нормализованное к -1..+1
	unsigned i;
	for (i = 0; i < NOVERLAP; ++ i)
	{
		fftbuff_t * * ppf = & pfill [i];

		if (* ppf == NULL)
		{
			if (allocate_fftbuffer_low(ppf) == 0)
			{
				TP();
				continue;	/* обшибочная ситуация, нарушает фиксированный сдвиг перекрытия буферов */
			}
			filleds [i] = 0;
		}
		fftbuff_t * const pf = * ppf;

		pf->largebuffI [filleds [i]] = ivf;
		pf->largebuffQ [filleds [i]] = qvf;

		if (++ filleds [i] >= LARGEFFT)
		{
			saveready_fftbuffer_low(pf);
			* ppf = NULL;
		}
	}
}

// вызывается при запрещенных прерываниях.
void fftbuffer_initialize(void)
{
	static RAMBIG fftbuff_t fftbuffersarray [NOVERLAP * 3];
	unsigned i;

	InitializeListHead(& fftbuffree);	// Свободные
	for (i = 0; i < (sizeof fftbuffersarray / sizeof fftbuffersarray [0]); ++ i)
	{
		fftbuff_t * const p = & fftbuffersarray [i];
		InsertHeadList(& fftbuffree, & p->item);
	}
	InitializeListHead(& fftbufready);	// Для выдачи на дисплей
	/* начальный запрос буферов заполнение выборок. */
	for (i = 0; i < NOVERLAP; ++ i)
	{
		fftbuff_t * * const ppf = & pfill [i];
		VERIFY(allocate_fftbuffer_low(ppf) != 0);
		/* установка начальной позиции для заполнения со сдвигом. */
		const unsigned filled = (i * LARGEFFT / NOVERLAP);
		filleds [i] = filled;
		arm_fill_f32(0, (* ppf)->largebuffI, filled);
		arm_fill_f32(0, (* ppf)->largebuffQ, filled);
	}
	SPINLOCK_INITIALIZE(& fftlock);
}

static void fftzoom_filer_decimate_ifspectrum(
	const struct zoom_param * const prm,
	float32_t * buffer
	)
{
	union configs
	{
		arm_biquad_cascade_df2T_instance_f32 iir_config;
		arm_fir_decimate_instance_f32 fir_config;
	} c;
	const unsigned usedSize = NORMALFFT * prm->zoom;

	// Biquad LPF фильтр
#if defined (ARM_MATH_NEON)
	float32_t IIRCoeffs_NEON [ZOOMFFT_DECIM_STAGES_IIR * 8];

	// Initialize floating-point Biquad cascade filter.
    arm_biquad_cascade_df2T_compute_coefs_f32(ZOOMFFT_DECIM_STAGES_IIR, prm->pIIRCoeffs, IIRCoeffs_NEON);
    arm_biquad_cascade_df2T_init_f32(& c.iir_config, ZOOMFFT_DECIM_STAGES_IIR, IIRCoeffs_NEON, gvars.iir_state);

#else /* defined (ARM_MATH_NEON) */
	// Initialize floating-point Biquad cascade filter.
	arm_biquad_cascade_df2T_init_f32(& c.iir_config, ZOOMFFT_DECIM_STAGES_IIR, prm->pIIRCoeffs, gvars.iir_state);
	arm_biquad_cascade_df2T_f32(& c.iir_config, buffer, buffer, usedSize);

#endif /* defined (ARM_MATH_NEON) */

	// Дециматор
	VERIFY(ARM_MATH_SUCCESS == arm_fir_decimate_init_f32(& c.fir_config,
						prm->numTaps,
						prm->zoom,          // Decimation factor
						prm->pFIRCoeffs,
						gvars.fir_state,            // Filter state variables
						usedSize));
	arm_fir_decimate_f32(& c.fir_config, buffer, buffer, usedSize);
}

// децимация НЧ спектра для увеличения разрешения
static void fftzoom_af(float32_t * buffer, unsigned zoompow2, unsigned normalFFT)
{
	if (zoompow2 != 0)
	{
		ASSERT(ARRAY_SIZE(zoom_params) >= zoompow2);
		const struct zoom_param * const prm = & zoom_params [zoompow2 - 1];
		arm_fir_decimate_instance_f32 fir_config;
		const unsigned usedSize = normalFFT * prm->zoom;

		VERIFY(ARM_MATH_SUCCESS == arm_fir_decimate_init_f32(& fir_config,
							prm->numTaps,
							prm->zoom,          // Decimation factor
							prm->pFIRCoeffs,
							gvars.fir_state,       	// Filter state variables
							usedSize));

		arm_fir_decimate_f32(& fir_config, buffer, buffer, usedSize);
	}
}

static void
make_cmplx(
	float32_t * dst,
	uint_fast16_t size,
	const float32_t * realv,
	const float32_t * imgev
	)
{
	while (size --)
	{
		dst [0] = * realv ++;
		dst [1] = * imgev ++;
		dst += 2;
	}
}

// перевод позиции в окне в номер бина - отображение с нулевой частотой в центре окна
static int raster2fft(
	int x,	// window pos
	int dx,	// width
	int fftsize,	// размер буфера FFT (в бинах)
	int visiblefftsize	// Часть буфера FFT, отобрааемая на экране (в бинах)
	)
{
	const int xm = dx / 2;	// middle
	const int delta = x - xm;	// delta in pixels
	const int fftoffset = delta * (visiblefftsize / 2 - 1) / xm;
	return fftoffset < 0 ? (fftsize + fftoffset) : fftoffset;
}

// Копрование информации о спектре с текущую строку буфера
// преобразование к пикселям растра
static uint_fast8_t
dsp_getspectrumrow(
	FLOAT_t * const hbase,	// Буфер амплитуд
	uint_fast16_t dx,		// X width (pixels) of display window
	uint_fast8_t zoompow2	// horisontal magnification power of two
	)
{
	const uint_fast32_t needsize = ((uint_fast32_t) NORMALFFT << zoompow2);
	uint_fast16_t i;
	uint_fast16_t x;
	arm_cfft_instance_f32 fftinstance;

	// проверка, есть ли накопленный буфер для формирования спектра
	fftbuff_t * pf;
	if (getfilled_fftbuffer(& pf) == 0)
		return 0;

	const unsigned usedsize = NORMALFFT << zoompow2;
	float32_t * const largesigI = pf->largebuffI + LARGEFFT - usedsize;
	float32_t * const largesigQ = pf->largebuffQ + LARGEFFT - usedsize;
	float32_t * const fftinpt = gvars.cmplx_sig;

	//beginstamp();	// performance diagnostics
	// ARM_MATH_LOOPUNROLL=1, ARM_MATH_NEON=1
	// STM32MP1: -Og -mfpu=neon-vfpv4 : dtmax=286188, dtlast=237885,
	// STM32MP1: -Ofast -flto -mfpu=neon-vfpv4 : dtmax=224095, dtlast=174349
	// STM32MP1: -Ofast -flto -mfpu=vfpv4 : dtmax=236905, dtlast=183697
	// ARM_MATH_NEON=1
	// STM32MP1: -Ofast -flto -mfpu=neon-vfpv4 : dtmax=226855, dtlast=200977

	if (zoompow2 > 0)
	{
		ASSERT(ARRAY_SIZE(zoom_params) >= zoompow2);
		const struct zoom_param * const prm = & zoom_params [zoompow2 - 1];

		fftzoom_filer_decimate_ifspectrum(prm, largesigI);
		fftzoom_filer_decimate_ifspectrum(prm, largesigQ);
	}

	// Подготовить массив комплексных чисел для преобразования в частотную область
	make_cmplx(fftinpt, NORMALFFT, largesigQ, largesigI);

	release_fftbuffer(pf);

	arm_cmplx_mult_real_f32(fftinpt, gvars.ifspec_wndfn, fftinpt,  NORMALFFT);	// Применить оконную функцию к IQ буферу
	VERIFY(ARM_MATH_SUCCESS == arm_cfft_init_f32(& fftinstance, NORMALFFT));
	arm_cfft_f32(& fftinstance, fftinpt, 0, 1);	// forward transform
	arm_cmplx_mag_f32(fftinpt, fftinpt, NORMALFFT);	/* Calculate magnitudes */

	//endstamp();	// performance diagnostics

	enum { visiblefftsize = (int_fast64_t) NORMALFFT * SPECTRUMWIDTH_MULT / SPECTRUMWIDTH_DENOM };
	enum { fftsize = NORMALFFT };
	static const FLOAT_t fftcoeff = (FLOAT_t) 1 / (int32_t) (NORMALFFT / 2);
	for (x = 0; x < dx; ++ x)
	{
		const int fftpos = raster2fft(x, dx, fftsize, visiblefftsize);
		hbase [x] = fftinpt [fftpos] * fftcoeff;
	}
	return 1;
}

enum { BUFDIM_X = DIM_X, BUFDIM_Y = DIM_Y };
//enum { BUFDIM_X = ALLDX, BUFDIM_Y = ALLDY };

static uint_fast32_t wffreqpix;			// глобальный пиксель по x центра спектра, для которой в последной раз отрисовали.
static uint_fast8_t wfzoompow2;				// масштаб, с которым выводили спектр
static int_fast16_t wfhscroll;			// сдвиг по шоризонтали (отрицаельный - влево) для водопада.
static uint_fast16_t wfvscroll;			// сдвиг по вертикали (в рабочем направлении) для водопада.
static uint_fast8_t wfclear;			// стирание всей областии отображение водопада.


// стираем целиком старое изображение водопада
// в строке wfrow - новое
static void wflclear(void)
{
	uint_fast16_t y;
#if WITHVIEW_3DSS
	uint_fast8_t rows = glob_view_style == VIEW_3DSS ? MAX_3DSS_STEP : WFROWS;
#else
	uint_fast8_t rows = WFROWS;
#endif /* WITHVIEW_3DSS */

	for (y = 0; y < rows; ++ y)
	{
		if (y == wfrow)
			continue;
		// TODO: use minimal level color gvars.color_scale [0] instead of binary zeroes
		memset(
				colmain_mem_at(gvars.wfjarray, ALLDX, rows, 0, y),
				0x00,
				ALLDX * sizeof gvars.wfjarray [0]
		);
	}
}


// стираем целиком старое изображение водопада
// Очистка водопада (без учета последней записаной строки)
static void wflclear0(void)
{
	uint_fast16_t y;
#if WITHVIEW_3DSS
	uint_fast8_t rows = glob_view_style == VIEW_3DSS ? MAX_3DSS_STEP : WFROWS;
#else
	uint_fast8_t rows = WFROWS;
#endif /* WITHVIEW_3DSS */

	for (y = 0; y < rows; ++ y)
	{
		// TODO: use minimal level color gvars.color_scale [0] instead of binary zeroes
		memset(
				colmain_mem_at(gvars.wfjarray, ALLDX, rows, 0, y),
				0x00,
				ALLDX * sizeof gvars.wfjarray [0]
		);
	}
}

// стираем буфер усреднения FFT
static void fft_avg_clear(void)
{
	memset(gvars.Yold_spe, 0x00, sizeof gvars.Yold_spe);
}

// стираем буфер усреднения водопада
static void wfl_avg_clear(void)
{
	memset(gvars.Yold_wtf, 0x00, sizeof gvars.Yold_wtf);
}

// частота увеличилась - надо сдвигать картинку влево
// нужно сохрянять часть старого изображения
// в строке wfrow - новое
static void wflshiftleft(uint_fast16_t pixels)
{
	uint_fast16_t y;
#if WITHVIEW_3DSS
	uint_fast8_t rows = glob_view_style == VIEW_3DSS ? MAX_3DSS_STEP : WFROWS;
#else
	uint_fast8_t rows = WFROWS;
#endif /* WITHVIEW_3DSS */

	if (pixels == 0)
		return;

	// двигаем буфер усреднения значений WTF и FFT
	memmove(& gvars.Yold_spe [0], & gvars.Yold_spe [pixels], (ALLDX - pixels) * sizeof gvars.Yold_spe [0]);
	memset(& gvars.Yold_spe [ALLDX - pixels], 0x00, pixels * sizeof gvars.Yold_spe[0]);

	memmove(& gvars.Yold_wtf [0], & gvars.Yold_wtf [pixels], (ALLDX - pixels) * sizeof gvars.Yold_wtf [0]);
	memset(& gvars.Yold_wtf [ALLDX - pixels], 0x00, pixels * sizeof gvars.Yold_wtf[0]);

	for (y = 0; y < rows; ++ y)
	{
//		if (y == wfrow)
//		{
//			continue;
//		}
		memmove(
				colmain_mem_at(gvars.wfjarray, ALLDX, rows, 0, y),
				colmain_mem_at(gvars.wfjarray, ALLDX, rows, pixels, y),
				(ALLDX - pixels) * sizeof gvars.wfjarray [0]
		);
		// TODO: use minimal level color gvars.color_scale [0] instead of binary zeroes
		memset(
				colmain_mem_at(gvars.wfjarray, ALLDX, rows, ALLDX - pixels, y),
				0x00,
				pixels * sizeof gvars.wfjarray [0]
		);
	}
}

// частота уменьшилась - надо сдвигать картинку вправо
// нужно сохрянять часть старого изображения
// в строке wfrow - новое
static void wflshiftright(uint_fast16_t pixels)
{
	uint_fast16_t y;
#if WITHVIEW_3DSS
	uint_fast8_t rows = glob_view_style == VIEW_3DSS ? MAX_3DSS_STEP : WFROWS;
#else
	uint_fast8_t rows = WFROWS;
#endif /* WITHVIEW_3DSS */

	if (pixels == 0)
		return;

	// двигаем буфер усреднения значений WTF и FFT
	memmove(& gvars.Yold_spe [pixels], & gvars.Yold_spe [0], (ALLDX - pixels) * sizeof gvars.Yold_spe [0]);
	memset(& gvars.Yold_spe [0], 0x00, pixels * sizeof gvars.Yold_spe [0]);

	memmove(& gvars.Yold_wtf [pixels], & gvars.Yold_wtf [0], (ALLDX - pixels) * sizeof gvars.Yold_wtf [0]);
	memset(& gvars.Yold_wtf [0], 0x00, pixels * sizeof gvars.Yold_wtf [0]);


	for (y = 0; y < rows; ++ y)
	{
		memmove(
				colmain_mem_at(gvars.wfjarray, ALLDX, rows, pixels, y),
				colmain_mem_at(gvars.wfjarray, ALLDX, rows, 0, y),
				(ALLDX - pixels) * sizeof gvars.wfjarray [0]
		);
		// TODO: use minimal level color gvars.color_scale [0] instead of binary zeroes
		memset(
				colmain_mem_at(gvars.wfjarray, ALLDX, rows, 0, y),
				0x00,
				pixels * sizeof gvars.wfjarray [0]
		);
	}
}

// стираем целиком старое изображение водопада
static void wfsetupnew0(void)
{
	wflclear0();
	fft_avg_clear(); // очищаем буфер усреднения FFT
	wfl_avg_clear(); // очищаем буфер усреднения водопада
}

// стираем целиком старое изображение водопада
// в строке 0 - новое
static void wfsetupnew(void)
{
	wflclear();
	fft_avg_clear(); // очищаем буфер усреднения FFT
	wfl_avg_clear(); // очищаем буфер усреднения водопада
}

// отрисовка вновь появившихся данных на водопаде (в случае использования аппаратного scroll видеопамяти).
static void display_wfputrow(uint_fast16_t x, uint_fast16_t y, const PACKEDCOLORMAIN_T * p)
{
	colpip_to_main(
			(uintptr_t) p, GXSIZE(ALLDX, 1) * sizeof * p,
			p, ALLDX, 1, x, y);
}

static void
display2_wfl_init(
	uint_fast8_t xgrid,
	uint_fast8_t ygrid,
	dctx_t * pctx
	)
{
	static subscribeint32_t rtsregister;
	int i;
	for (i = 0; i < WITHFFTSIZEWIDE; ++ i)
	{
		gvars.ifspec_wndfn [i] = fir_design_window(i, WITHFFTSIZEWIDE, BOARD_WTYPE_SPECTRUM);
	}
	//printsigwnd();	// печать оконных коэффициентов для формирования таблицы во FLASH
	//toplogdb = LOG10F((FLOAT_t) INT32_MAX / waterfalrange);
	fftbuffer_initialize();

	subscribeint_user(& rtstargetsint, & rtsregister, NULL, saveIQRTSxx);

	// Код взят из проекта Malamute (R3DI)
	//PRINTF("wfpalette_initialize: main=%d, pip=%d, PALETTESIZE=%d, LCDMODE_MAIN_PAGES=%d\n", sizeof (PACKEDCOLORMAIN_T), sizeof (PACKEDCOLORMAIN_T), PALETTESIZE, LCDMODE_MAIN_PAGES);
	if (PALETTESIZE == 256)
	{
#if ! defined (COLORPIP_SHADED)
		int i;
		// Init 256 colors palette
		ASSERT(PALETTESIZE == 256);
		for (i = 0; i < PALETTESIZE; ++ i)
		{
			const COLOR24_T c = colorgradient(i, PALETTESIZE - 1);
			wfpalette [i] = TFTRGB565(COLOR24_R(c), COLOR24_G(c), COLOR24_B(c));
		}
#endif /* !  defined (COLORPIP_SHADED) */
	}
	else
	{
#if WITHFASTWATERFLOW && 1
		int i;
		for (i = 0; i < PALETTESIZE; ++ i)
		{
			const COLOR24_T c = pancolor [PALETTESIZE - 1 - i];
			wfpalette [i] = TFTRGB565(COLOR24_R(c), COLOR24_G(c), COLOR24_B(c));
		}
#endif /* WITHFASTWATERFLOW */
	}

	/* массив значений для раскраски спектра */
	for (i = 0; i < SPDY; ++ i)
	{
#if LCDMODE_MAIN_L8
		gvars.color_scale [i] = normalize(i, 0, SPDY - 1, PALETTESIZE - 1);
#else /* LCDMODE_MAIN_L8 */
		gvars.color_scale [i] = wfpalette [normalize(i, 0, SPDY - 1, PALETTESIZE - 1)];
#endif /* LCDMODE_MAIN_L8 */
	}

	fft_avg_clear();	// Сброс фильтра
	wfl_avg_clear();	// Сброс фильтра

	wflclear0();	// Очистка водопада (без учета последней записаной строки)
	uint_fast16_t y;
	for (y = 0; y < WFROWS; ++ y)
	{
		uint_fast16_t x;
		for (x = 0; x < ALLDX; ++ x)
			colmain_putpixel(gvars.wfjarray, ALLDX, WFROWS, x, y, gvars.color_scale [0]);	// wfpalette [0]
	}
}

// получить горизонтальную позицию для заданного отклонения в герцах
// Использовать для "статической" разметки дисплея - полоса пропускания, маркер частоты приема.
static uint_fast16_t
deltafreq2x(
	int_fast32_t fc,	// центральная частота
	int_fast16_t delta,	// отклонение от центральной частоты в герцах
	int_fast32_t bw,	// полоса обзора
	uint_fast16_t width	// ширина экрана
	)
{
	const int_fast32_t dp = (delta + bw / 2) * (width - 1) / bw;
	return dp;
}

// Получить абсолюьный пиксель горизонтальной позиции для заданой частоты
// Значения в пикселях меньше чем частота в герцах - тип шире, чем uint_fast32_t не требуется
static
uint_fast32_t
deltafreq2abspix(
	int_fast32_t f,	// частота в герцах
	int_fast32_t bw,	// полоса обзора в герцах
	uint_fast16_t width	// ширина экрана
	)
{
	const uint_fast32_t pc = ((int_fast64_t) f * width) / bw;	// абсолютный пиксель соответствующий частоте

	return pc;
}

// получить горизонтальную позицию для заданного отклонения в герцах
// Использовать для "динамической" разметки дисплея - риски, кратные 10 кГц.
// Возврат UINT16_MAX при невозможности отобразить запрошенную частоту в указанном окне
static
uint_fast16_t
deltafreq2x_abs(
	int_fast32_t fc,	// центральная частота
	int_fast16_t delta,	// отклонение от центральной частоты в герцах
	int_fast32_t bw,	// полоса обзора
	uint_fast16_t width	// ширина экрана
	)
{
	const int_fast32_t fm = fc + delta;	// частота маркера
	const uint_fast32_t pm = deltafreq2abspix(fm, bw, width);	// абсолютный пиксель маркера
	const int_fast32_t f0 = fc - bw / 2;	// частота левого края окна
	const uint_fast32_t p0 = deltafreq2abspix(f0, bw, width);	// абсолютный пиксель левого края окна

	if (fm < 0)
		return UINT16_MAX;	// частота маркера края выходит за пределы представимого
	if (f0 < 0)
		return UINT16_MAX;	// частота левого края выходит за пределы представимого
	if (pm < p0)
		return UINT16_MAX;	// Левее левого края окна
	if (pm - p0 >= width)
		return UINT16_MAX;	// Правее правого края окна
	return pm - p0;
}

#define MARKERH 10

static uint_fast8_t
isvisibletext(
	uint_fast16_t dx, // ширина буфера
	uint_fast16_t x, // начало строки
	uint_fast16_t w	// ширина строки со значением частоты
	)
{
	return (x + w) <= dx;
}

// отрисовка маркеров частот
static
void
display_colorgrid_xor(
	PACKEDCOLORMAIN_T * buffer,
	uint_fast16_t row0,	// вертикальная координата начала занимаемой области (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	int_fast32_t f0,	// center frequency
	int_fast32_t bw		// span
	)
{
	const COLORPIP_T color0 = COLORPIP_GRIDCOLOR;	// макркер на центре
	const COLORPIP_T color = COLORPIP_GRIDCOLOR2;	// макркеры частот сетки
	//
	const int_fast32_t go = f0 % (int) glob_gridstep;	// шаг сетки
	const int_fast32_t gs = (int) glob_gridstep;	// шаг сетки
	const int_fast32_t halfbw = bw / 2;
	int_fast32_t df;	// кратное сетке значение
	for (df = - halfbw / gs * gs - go; df < halfbw; df += gs)
	{
		if (df > - halfbw)
		{
			uint_fast16_t xmarker;
			// Маркер частоты кратной glob_gridstep - XOR линию
			xmarker = deltafreq2x_abs(f0, df, bw, ALLDX);
			if (xmarker != UINT16_MAX)
			{
				char buf2 [16];
				uint_fast16_t freqw;	// ширина строки со значением частоты
				local_snprintf_P(buf2, ARRAY_SIZE(buf2), gridfmt_2, glob_gridwc, (long) ((f0 + df) / glob_griddigit % glob_gridmod));
				freqw = strwidth3(buf2);
				uint_fast16_t xtext = xmarker >= (freqw + 1) / 2 ? xmarker - (freqw + 1) / 2 : UINT16_MAX;
				if (isvisibletext(BUFDIM_X, xtext, freqw))
				{
					colpip_string3_tbg(buffer, BUFDIM_X, BUFDIM_Y, xtext, row0, buf2, COLORPIP_YELLOW);
					display_colorbuf_xor_vline(buffer, BUFDIM_X, BUFDIM_Y, xmarker, row0 + MARKERH, h - MARKERH, color);
				}
				else
					display_colorbuf_xor_vline(buffer, BUFDIM_X, BUFDIM_Y, xmarker, row0, h, color);
			}
		}
	}
	display_colorbuf_xor_vline(buffer, BUFDIM_X, BUFDIM_Y, ALLDX / 2, row0, h, color0);	// center frequency marker
}

// Преобразовать отношение выраженное в децибелах к "разам" отношения напряжений.

static FLOAT_t db2ratio(FLOAT_t valueDBb)
{
	return POWF(10, valueDBb / 20);
}

// отрисовка маркеров частот
static
void
display_colorgrid_set(
	PACKEDCOLORMAIN_T * buffer,
	uint_fast16_t row0,	// вертикальная координата начала занимаемой области (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	int_fast32_t f0,	// center frequency
	int_fast32_t bw		// span
	)
{
	const COLORPIP_T color0 = COLORPIP_GRIDCOLOR;	// макркер на центре
	const COLORPIP_T color = COLORPIP_GRIDCOLOR2;
	const uint_fast8_t markerh = 10;
	const int_fast32_t go = f0 % (int) glob_gridstep;	// шаг сетки
	const int_fast32_t gs = (int) glob_gridstep;	// шаг сетки
	const int_fast32_t halfbw = bw / 2;
	int_fast32_t df;	// кратное сетке значение

	// Маркеры уровней сигналов
	if (glob_lvlgridstep != 0)
	{
		int_fast16_t lvl;
		for (lvl = glob_topdb / glob_lvlgridstep * glob_lvlgridstep; lvl < glob_bottomdb; lvl += glob_lvlgridstep)
		{
			const int valy = dsp_mag2y(db2ratio(- lvl), h - 1, glob_topdb, glob_bottomdb);

			display_colorbuf_set_hline(buffer, BUFDIM_X, BUFDIM_Y, 0, valy, ALLDX, color);	// Level marker
		}
	}

	for (df = - halfbw / gs * gs - go; df < halfbw; df += gs)
	{
		if (df > - halfbw)
		{
			// Маркер частоты кратной glob_gridstep - XOR линию
			const uint_fast16_t xmarker = deltafreq2x_abs(f0, df, bw, ALLDX);
			if (xmarker != UINT16_MAX)
			{
				char buf2 [16];
				uint_fast16_t freqw;	// ширина строки со значением частоты
				local_snprintf_P(buf2, ARRAY_SIZE(buf2), gridfmt_2, glob_gridwc, (long) ((f0 + df) / glob_griddigit % glob_gridmod));
				freqw = strwidth3(buf2);
				uint_fast16_t xtext = xmarker >= (freqw + 1) / 2 ? xmarker - (freqw + 1) / 2 : UINT16_MAX;
				if (isvisibletext(BUFDIM_X, xtext, freqw))
				{
					colpip_string3_tbg(buffer, BUFDIM_X, BUFDIM_Y, xtext, row0, buf2, COLORPIP_YELLOW);
					display_colorbuf_set_vline(buffer, BUFDIM_X, BUFDIM_Y, xmarker, row0 + markerh, h - markerh, color);
				}
				else
					display_colorbuf_set_vline(buffer, BUFDIM_X, BUFDIM_Y, xmarker, row0, h, color);
			}
		}
	}
	display_colorbuf_set_vline(buffer, BUFDIM_X, BUFDIM_Y, ALLDX / 2, row0, h, color0);	// center frequency marker
}

// отрисовка маркеров частот для 3DSS
static
void
display_colorgrid_3dss(
	PACKEDCOLORMAIN_T * buffer,
	uint_fast16_t row0,	// вертикальная координата начала занимаемой области (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	int_fast32_t f0,	// center frequency
	int_fast32_t bw		// span
	)
{
	const COLORPIP_T color0 = COLORPIP_GRIDCOLOR;	// макркер на центре
	const COLORPIP_T color = COLORPIP_GREEN;
	const uint_fast16_t row = row0 + h + 3;
	const int_fast32_t go = f0 % (int) glob_gridstep;	// шаг сетки
	const int_fast32_t gs = (int) glob_gridstep;	// шаг сетки
	const int_fast32_t halfbw = bw / 2;
	int_fast32_t df;	// кратное сетке значение
	for (df = - halfbw / gs * gs - go; df < halfbw; df += gs)
	{
		uint_fast16_t xmarker;
		if (df > - halfbw)
		{
			// Маркер частоты кратной glob_gridstep - XOR линию
			xmarker = deltafreq2x_abs(f0, df, bw, ALLDX);
			if (xmarker != UINT16_MAX)
			{
				char buf2 [16];
				uint_fast16_t freqw;	// ширина строки со значением частоты
				local_snprintf_P(buf2, ARRAY_SIZE(buf2), gridfmt_2, glob_gridwc, (long) ((f0 + df) / glob_griddigit % glob_gridmod));
				freqw = strwidth3(buf2);
				uint_fast16_t xtext = xmarker >= (freqw + 1) / 2 ? xmarker - (freqw + 1) / 2 : UINT16_MAX;
				if (isvisibletext(BUFDIM_X, xtext, freqw))
					colpip_string3_tbg(buffer, BUFDIM_X, BUFDIM_Y, xtext, row, buf2, COLORPIP_YELLOW);

				display_colorbuf_set_vline(buffer, BUFDIM_X, BUFDIM_Y, xmarker, row0, h, color);
			}
		}
	}
	display_colorbuf_set_vline(buffer, BUFDIM_X, BUFDIM_Y, ALLDX / 2, row0, h, color0);	// center frequency marker
}

// Спектр на монохромных дисплеях
// или на цветных, где есть возможность раскраски растровой картинки.

#define HHWMG ((! LCDMODE_S1D13781_NHWACCEL && LCDMODE_S1D13781) || LCDMODE_UC1608 || LCDMODE_UC1601)


#if WITHVIEW_3DSS

static void init_depth_map_3dss(void)
{
	for (int_fast8_t i = 0; i < MAX_3DSS_STEP; i ++)
	{
		uint_fast16_t range = HALF_ALLDX - 1 - i * Y_STEP;

		for (uint_fast16_t x = 0; x < ALLDX; ++ x)
		{
			uint_fast16_t x1;

			if (x <= HALF_ALLDX)
				x1 = HALF_ALLDX - normalize(HALF_ALLDX - x, 0, HALF_ALLDX, range);
			else
				x1 = HALF_ALLDX + normalize(x, HALF_ALLDX, ALLDX - 1, range);

			gvars.depth_map_3dss [i][x] = x1;
		}
	}
}
#endif /* WITHVIEW_3DSS */

#if HHWMG
static ALIGNX_BEGIN GX_t spectmonoscr [MGSIZE(ALLDX, SPDY)] ALIGNX_END;
#endif /* HHWMG */

uint_fast16_t calcprev(uint_fast16_t v, uint_fast16_t lim)
{
	if (v)
		return -- v;
	else
		return lim - 1;
}

uint_fast16_t calcnext(uint_fast16_t v, uint_fast16_t lim)
{
	v ++;
	if (v >= lim)
		return 0;
	else
		return v;
}

// подготовка изображения спектра
static void display2_spectrum(
	uint_fast8_t x0, 
	uint_fast8_t y0, 
	dctx_t * pctx
	)
{
#if HHWMG
	// Спектр на монохромных дисплеях
	// или на цветных, где есть возможность раскраски растровой картинки.

	if (1 || hamradio_get_tx() == 0)
	{
		const GTG_t gtg = { spectmonoscr, ALLDX, SPDY, };
		uint_fast16_t x;
		uint_fast16_t y;
		const uint_fast8_t pathi = 0;	// RX A
		const uint_fast32_t f0 = hamradio_get_freq_pathi(pathi);	/* frequency at middle of spectrum */
		const int_fast32_t bw = display_zoomedbw();
		uint_fast16_t xleft = deltafreq2x(f0, hamradio_getleft_bp(pathi), bw, ALLDX);	// левый край шторуи
		uint_fast16_t xright = deltafreq2x(f0, hamradio_getright_bp(pathi), bw, ALLDX);	// правый край шторки
		uint_fast16_t xmarker = deltafreq2x(f0, 0, bw, ALLDX);	// центральная частота

		if (xleft > xright)
			xleft = 0;
		if (xright == xleft)
			xright = xleft + 1;

		// формирование растра
		display_pixelbuffer_clear(spectmonoscr, ALLDX, SPDY);

		if (glob_fillspect == 0)
		{
			/* рисуем спектр ломанной линией */
			uint_fast16_t ylast = 0;
			// отображение спектра
			for (x = 0; x < ALLDX; ++ x)
			{
				// логарифм - в вертикальную координату
				const uint_fast16_t ynew = SPDY - 1 - dsp_mag2y(filter_spectrum(x), SPDY - 1, glob_topdb, glob_bottomdb);
				if (x != 0)
					display_pixelbuffer_line(spectmonoscr, ALLDX, SPDY, x - 1, ylast, x, ynew);
				ylast = ynew;
			}
		}
		else
		{
			// отображение спектра заполненной зоной
			for (x = 0; x < ALLDX; ++ x)
			{
				// логарифм - в вертикальную координату
				const int yv = SPDY - 1 - dsp_mag2y(filter_spectrum(x), SPDY - 1, glob_topdb, glob_bottomdb);	//отображаемый уровень, yv = 0..SPDY
				for (y = yv; y < SPDY; ++ y)
					display_pixelbuffer(spectmonoscr, ALLDX, SPDY, x, SPY0 + y);	// set точку
			}
		}
		// формирование изображения шторки (XOR).
		for (x = xleft; x <= xright; ++ x)
		{
			for (y = 0; y < SPDY; ++ y)
				display_pixelbuffer_xor(spectmonoscr, ALLDX, SPDY, x, SPY0 + y);	// xor точку
		}
		// формирование маркера центральной частоты (XOR).
		if (xmarker < ALLDX && xmarker != xleft && xmarker != xright)
		{
			for (y = 0; y < SPDY; ++ y)
				display_pixelbuffer_xor(spectmonoscr, ALLDX, SPDY, xmarker, SPY0 + y);	// xor точку
		}
	}
	else
	{
		display_pixelbuffer_clear(spectmonoscr, ALLDX, SPDY);
	}
	colmain_setcolors(COLORPIP_SPECTRUMBG, COLORPIP_SPECTRUMFG);

#else /* */
	PACKEDCOLORMAIN_T * const colorpip = getscratchwnd();
	(void) x0;
	(void) y0;
	(void) pctx;

#if WITHTOUCHGUI
	const uint_fast16_t spy = ALLDY - FOOTER_HEIGHT - 15;
#else
	const uint_fast16_t spy = ALLDY - 15;
#endif

	// Спектр на цветных дисплеях, не поддерживающих ускоренного
	// построения изображения по bitmap с раскрашиванием
	if (1 || hamradio_get_tx() == 0)
	{
		const uint_fast8_t pathi = 0;	// RX A
		const uint_fast32_t f0 = hamradio_get_freq_pathi(pathi);	/* frequency at middle of spectrum */
		const int_fast32_t bw = display_zoomedbw();
		uint_fast16_t xleft = deltafreq2x(f0, hamradio_getleft_bp(pathi), bw, ALLDX);	// левый край шторки
		uint_fast16_t xright = deltafreq2x(f0, hamradio_getright_bp(pathi), bw, ALLDX);	// правый край шторки

		if (xleft > xright)
			xleft = 0;
		if (xright == xleft)
			xright = xleft + 1;
		if (xright >= ALLDX)
			xright = ALLDX - 1;

		const uint_fast16_t xrightv = xright + 1;	// рисуем от xleft до xright включительно

#if WITHVIEW_3DSS
		if (glob_view_style == VIEW_3DSS)
		{
			static uint_fast8_t current_3dss_step = 0;
			static uint_fast8_t delay_3dss = MAX_DELAY_3DSS;

			uint_fast8_t draw_step = calcprev(current_3dss_step, MAX_3DSS_STEP);
			uint_fast16_t ylast_sp = 0;
			int i;

			for (int_fast8_t i = 0; i < MAX_3DSS_STEP - 1; i ++)
			{
				uint_fast16_t y0 = spy - 5 - i * Y_STEP;
				uint_fast16_t x;

				for (x = 0; x < ALLDX; ++ x)
				{
					if (i == 0)
					{
						const int val = dsp_mag2y(filter_spectrum(x), SPY_3DSS - 1, glob_topdb, glob_bottomdb);
						uint_fast16_t ynew = spy - 1 - val;
						uint_fast16_t dy, j;
						* colmain_mem_at(gvars.wfjarray, ALLDX, MAX_3DSS_STEP, x, current_3dss_step) = val;

						for (dy = spy - 1, j = 0; dy > ynew; dy --, j ++)
						{
							if (x > xleft && x < xrightv && gview3dss_mark)
								colpip_point(colorpip, BUFDIM_X, BUFDIM_Y, x, dy, COLORPIP_SPECTRUMFG);
							else
								colpip_point(colorpip, BUFDIM_X, BUFDIM_Y, x, dy, gvars.color_scale [j]);
						}

						if (x)
						{
							colmain_line(colorpip, BUFDIM_X, BUFDIM_Y, x - 1, ylast_sp, x, ynew, COLORMAIN_WHITE, 1);
						}

						gvars.envelope_y [x] = ynew - 2;
						ylast_sp = ynew;
					}
					else
					{
						static uint_fast16_t x_old = UINT16_MAX;
						uint_fast16_t x_d = gvars.depth_map_3dss [i - 1][x];

						if (x_d >= ALLDX)
							return;

						if (x_old != x_d)
						{
							uint_fast16_t y1 = y0 - * colmain_mem_at(gvars.wfjarray, ALLDX, MAX_3DSS_STEP, x, draw_step);
							int_fast16_t h = y0 - y1 - i / DEPTH_ATTENUATION;		// высота пика
							h = h < 0 ? 0 : h;

							for (; h > 0; h --)
							{
								/* предотвращение отрисовки по ранее закрашенной области*/
								if (* colmain_mem_at(colorpip, BUFDIM_X, BUFDIM_Y, x_d, y0 - h) != COLORMAIN_BLACK)
									break;

								colpip_point(colorpip, BUFDIM_X, BUFDIM_Y, x_d, y0 - h, gvars.color_scale [h]);
							}
							x_old = x_d;
						}
					}
				}
				draw_step = calcprev(draw_step, MAX_3DSS_STEP);
			}
			delay_3dss = calcnext(delay_3dss, MAX_DELAY_3DSS);

			if (! delay_3dss)
				current_3dss_step = calcnext(current_3dss_step, MAX_3DSS_STEP);

			// увеличение контрастности спектра на фоне панорамы
			ylast_sp = spy;
			for (uint_fast16_t x = 0; x < ALLDX; ++ x)
			{
				uint_fast16_t y1 = gvars.envelope_y [x];

				if (y1 >= BUFDIM_Y)
					return;

				if (x)
					colmain_line(colorpip, BUFDIM_X, BUFDIM_Y, x - 1, ylast_sp, x, y1, COLORMAIN_BLACK, 0);

				ylast_sp = y1;
			}

			display_colorgrid_3dss(colorpip, spy - SPY_3DSS_H + 3, SPY_3DSS_H, f0, bw);
		}
		else
#endif /* WITHVIEW_3DSS */
		{
			/* рисуем спектр ломанной линией */
			/* стираем старый фон, рисуем прямоугольник полосы пропускания */
			if (ALLDX / (xrightv - xleft) > 8)
			{
				colmain_fillrect(colorpip, BUFDIM_X, BUFDIM_Y, 0, SPY0, ALLDX, SPDY, COLORPIP_SPECTRUMBG);
			}
			else
			{
				if (xleft > 0)
				{
					colmain_fillrect(colorpip, BUFDIM_X, BUFDIM_Y, 0, SPY0, xleft, SPDY, COLORPIP_SPECTRUMBG);
				}
				if (xrightv < ALLDX)
				{
					colmain_fillrect(colorpip, BUFDIM_X, BUFDIM_Y, xrightv, SPY0, ALLDX - xrightv, SPDY, COLORPIP_SPECTRUMBG);
				}
			}
			// Изображение "шторки".
			if (xleft < xrightv)
			{
				colmain_fillrect(colorpip, BUFDIM_X, BUFDIM_Y, xleft, SPY0, xrightv - xleft, SPDY, COLORMAIN_SPECTRUMBG2);
			}

			uint_fast16_t ylast = 0;
			display_colorgrid_set(colorpip, SPY0, SPDY, f0, bw);	// отрисовка маркеров частот

			for (uint_fast16_t x = 0; x < ALLDX; ++ x)
			{
				// ломанная
				const int val = dsp_mag2y(filter_spectrum(x), SPDY - 1, glob_topdb, glob_bottomdb);
				uint_fast16_t ynew = SPY0 + SPDY - 1 - val;

				if (glob_view_style == VIEW_COLOR) 		// раскрашенный цветовым градиентом спектр
				{
					for (uint_fast16_t dy = SPY0 + SPDY - 1, i = 0; dy > ynew; dy --, i ++)
					{
						colpip_point(colorpip, BUFDIM_X, BUFDIM_Y, x, dy, gvars.color_scale [i]);
					}
				}
				else if (glob_view_style == VIEW_FILL) // залитый зеленым спектр
				{
					display_colorbuf_set_vline(colorpip, BUFDIM_X, BUFDIM_Y, x, ynew + SPY0, SPDY - ynew, COLORPIP_SPECTRUMFG);
				}

				if (x)
				{
					colmain_line(colorpip, BUFDIM_X, BUFDIM_Y, x - 1, ylast, x, ynew, COLORPIP_SPECTRUMLINE, 1);
				}
				ylast = ynew;
			}
		}
	}

#endif
}

// формирование данных спектра для последующего отображения
// спектра или водопада
static void display2_latchwaterfall(
	uint_fast8_t x0,
	uint_fast8_t y0,
	dctx_t * pctx
	)
{
	uint_fast16_t x, y;
	(void) x0;
	(void) y0;
	(void) pctx;

	// Сдвиг изображения при необходимости (перестройка/переклбчение диапащонов или масштаба).
	const uint_fast8_t pathi = 0;	// RX A
	const int_fast32_t bw = display_zoomedbw();
	const uint_fast32_t f0pix = deltafreq2abspix(hamradio_get_freq_pathi(pathi), bw, ALLDX);	/* pixel of frequency at middle of spectrum */

	int_fast16_t hscroll = 0;
	uint_fast8_t hclear = 0;

	if (wffreqpix == 0 || wfzoompow2 != glob_zoomxpow2)
	{
		wfsetupnew(); // стираем целиком старое изображение водопада. в строке 0 - новое
		hclear = 1;
	}
	else if (wffreqpix == f0pix)
	{
		// не менялась частота (в видимых пикселях)
	}
	else if (wffreqpix > f0pix)
	{
		// частота уменьшилась - надо сдвигать картинку вправо
		const uint_fast32_t deltapix = wffreqpix - f0pix;
		if (deltapix < ALLDX / 2)
		{
			hscroll = (int_fast16_t) deltapix;
			// нужно сохрянять часть старого изображения
			// в строке wfrow - новое
			wflshiftright(hscroll);
		}
		else
		{
			wfsetupnew(); // стираем целиком старое изображение водопада. в строке 0 - новое
			hclear = 1;
		}
	}
	else if (wffreqpix < f0pix)
	{
		// частота увеличилась - надо сдвигать картинку влево
		const uint_fast32_t deltapix = f0pix - wffreqpix;
		if (deltapix < ALLDX / 2)
		{
			hscroll = - (int_fast16_t) deltapix;
			// нужно сохрянять часть старого изображения
			// в строке wfrow - новое
			wflshiftleft(- hscroll);
		}
		else
		{
			wfsetupnew(); // стираем целиком старое изображение водопада. в строке 0 - новое
			hclear = 1;
		}
	}

	// запоминание информации спектра для спектрограммы
	if (! dsp_getspectrumrow(gvars.spavgarray, ALLDX, glob_zoomxpow2))
		return;	// еще нет новых данных.

#if (! LCDMODE_S1D13781_NHWACCEL && LCDMODE_S1D13781)
#else
	wfrow = (wfrow == 0) ? (WFDY - 1) : (wfrow - 1);
#endif

#if WITHVIEW_3DSS
	if (glob_view_style != VIEW_3DSS)
#endif /* WITHVIEW_3DSS */
	{
		// запоминание информации спектра для водопада
		for (x = 0; x < ALLDX; ++ x)
		{
			// для водопада
			const int val = dsp_mag2y(filter_waterfall(x), PALETTESIZE - 1, glob_wflevelsep ? glob_topdbwf : glob_topdb, glob_wflevelsep ? glob_bottomdbwf : glob_bottomdb); // возвращает значения от 0 до dy включительно
		#if LCDMODE_MAIN_L8
			colmain_putpixel(gvars.wfjarray, ALLDX, WFROWS, x, wfrow, val);	// запись в буфер водопада индекса палитры
		#else /* LCDMODE_MAIN_L8 */
			ASSERT(val >= 0);
			ASSERT(val < ARRAY_SIZE(wfpalette));
			colmain_putpixel(gvars.wfjarray, ALLDX, WFROWS, x, wfrow, wfpalette [val]);	// запись в буфер водопада цветовой точки
		#endif /* LCDMODE_MAIN_L8 */
		}
	}

	wffreqpix = f0pix;
	wfzoompow2 = glob_zoomxpow2;
	wfhscroll += hscroll;
	wfvscroll = wfvscroll < WFDY ? wfvscroll + 1 : WFDY;
	wfclear = hclear;
}

// Подготовка изображения водопада
static void display2_waterfall(
	uint_fast8_t x0, 
	uint_fast8_t y0, 
	dctx_t * pctx
	)
{
#if (! LCDMODE_S1D13781_NHWACCEL && LCDMODE_S1D13781)

		const uint_fast8_t pathi = 0;	// RX A
		const uint_fast32_t f0 = hamradio_get_freq_pathi(pathi);	/* frequency at middle of spectrum */
		const int_fast32_t bw = display_zoomedbw();
		uint_fast16_t x, y;
		const uint_fast16_t xm = deltafreq2x(f0, 0, bw, ALLDX);
		int_fast16_t hscroll = wfclear ? ALLDX : wfhscroll;
		(void) pctx;

	#if 1
		// следы спектра ("водопад")
		// сдвигаем вниз, отрисовываем только верхнюю строку
		display_scroll_down(GRID2X(x0), GRID2Y(y0) + WFY0, ALLDX, WFDY, wfvscroll, hscroll);
		x = 0;
		display_wfputrow(GRID2X(x0) + x, GRID2Y(y0) + 0 + WFY0, & wfarray [wfrow] [0]);	// display_plot inside for one row
	#elif 1
		// следы спектра ("фонтан")
		// сдвигаем вверх, отрисовываем только нижнюю строку
		display_scroll_up(GRID2X(x0), GRID2Y(y0) + WFY0, ALLDX, WFDY, wfvscroll, hscroll);
		x = 0;
		display_wfputrow(GRID2X(x0) + x, GRID2Y(y0) + WFDY - 1 + WFY0, & wfarray [wfrow] [0]);	// display_plot inside for one row
	#else
		// следы спектра ("водопад")
		// отрисовываем весь экран
		for (y = 0; y < WFDY; ++ y)
		{
			// отрисовка горизонтальными линиями
			x = 0;
			display_wfputrow(GRID2X(x0) + x, GRID2Y(y0) + y + WFY0, & wfarray [(wfrow + y) % WFDY] [0]);	// display_plot inside for one row
		}
	#endif
		// Запрос на сдвиг исполнен
		wfhscroll = 0;
		wfvscroll = 0;
		wfclear = 0;

#elif HHWMG
	// Спектр на монохромных дисплеях
	// или на цветных, где есть возможность раскраски растровой картинки.

	// следы спектра ("водопад") на монохромных дисплеях

#elif WITHFASTWATERFLOW || LCDMODE_PIP_L8 || (! LCDMODE_PIP_L8 && LCDMODE_MAIN_L8)
	// следы спектра ("водопад") на цветных дисплеях
	/* быстрое отображение водопада (но требует больше памяти) */

	#if ! LCDMODE_HORFILL
		#error LCDMODE_HORFILL must be defined
	#endif /* ! LCDMODE_HORFILL */

#if WITHVIEW_3DSS
	if (glob_view_style != VIEW_3DSS)
#endif /* WITHVIEW_3DSS */
	{
		PACKEDCOLORMAIN_T * const colorpip = getscratchwnd();
		const uint_fast16_t p1h = WFDY - wfrow;	// высота верхней части в результируюшем изображении
		const uint_fast16_t p2h = wfrow;		// высота нижней части в результируюшем изображении
		const uint_fast16_t p1y = WFY0;
		const uint_fast16_t p2y = WFY0 + p1h;

		{
			/* перенос свежей части растра */
			colpip_plot(
					(uintptr_t) colorpip, GXSIZE(BUFDIM_X, BUFDIM_Y) * sizeof (PACKEDCOLORMAIN_T),
					colorpip, BUFDIM_X, BUFDIM_Y, 0, p1y,
					(uintptr_t) gvars.wfjarray, sizeof (* gvars.wfjarray) * GXSIZE(ALLDX, WFROWS),	// папаметры для clean
					colmain_mem_at(gvars.wfjarray, ALLDX, WFROWS, 0, wfrow),	// начальный адрес источника
					ALLDX, p1h);	// размеры источника
		}
		if (p2h != 0)
		{
			/* перенос старой части растра */
			colpip_plot(
					(uintptr_t) colorpip, 0 * sizeof (PACKEDCOLORMAIN_T),
					colorpip, BUFDIM_X, BUFDIM_Y, 0, p2y,
					(uintptr_t) gvars.wfjarray, 0 * sizeof (* gvars.wfjarray) * GXSIZE(ALLDX, WFROWS),	// размер области 0 - ранее уже вызывали clean
					colmain_mem_at(gvars.wfjarray, ALLDX, WFROWS, 0, 0),	// начальный адрес источника
					ALLDX, p2h);	// размеры источника
		}
	}

#else /* */

	// следы спектра ("водопад") на цветных дисплеях
	PACKEDCOLORMAIN_T * const colorpip = getscratchwnd();
	uint_fast16_t y;

	// формирование растра
	// следы спектра ("водопад")
	for (y = 0; y < WFDY; ++ y)
	{
		uint_fast16_t x;
		for (x = 0; x < ALLDX; ++ x)
		{
			colpip_point(colorpip, BUFDIM_X, BUFDIM_Y, x, y + WFY0, wfpalette [* colmain_mem_at(gvars.wfjarray, ALLDX, WFDY, x, (wfrow + y) % WFDY)]);
		}
	}

#endif /*  */
	(void) x0;
	(void) y0;
	(void) pctx;
}

static void display2_colorbuff(
	uint_fast8_t x0,
	uint_fast8_t y0,
	dctx_t * pctx
	)
{
#if HHWMG
	const GTG_t gtg = { spectmonoscr, ALLDX, SPDY, };
	// Спектр на монохромных дисплеях
	// или на цветных, где есть возможность раскраски растровой картинки.
	display_showbuffer(spectmonoscr, ALLDX, SPDY, x0, y0);

#else /* */

	#if (LCDMODE_LTDC)

	#else /* LCDMODE_LTDC */
	colpip_to_main(
			(uintptr_t) getscratchwnd(), sizeof (PACKEDCOLORMAIN_T) * GXSIZE(BUFDIM_X, BUFDIM_Y),
			getscratchwnd(), BUFDIM_X, BUFDIM_Y, GRID2X(x0), GRID2Y(y0));
	#endif /* LCDMODE_LTDC */

#endif /* LCDMODE_S1D13781 */
}

static
PACKEDCOLORMAIN_T * getscratchwnd(void)
{
#if HHWMG
	// Спектр на монохромных дисплеях
	// или на цветных, где есть возможность раскраски растровой картинки.

	return NULL;	//spectmonoscr;

#else /* */

	#if (LCDMODE_LTDC)

		pipparams_t pip;
		display2_getpipparams(& pip);
		return colmain_mem_at(colmain_fb_draw(), DIM_X, DIM_Y, pip.x, pip.y);

	#else /* LCDMODE_LTDC */

		static PACKEDCOLORMAIN_T tbuff0 [GXSIZE(BUFDIM_X, BUFDIM_Y)];
		return tbuff0;

	#endif /* LCDMODE_LTDC */

#endif /* LCDMODE_S1D13781 */
}


#else /* WITHSPECTRUMWF && ! LCDMODE_HD44780 && ! LCDMODE_DUMMY */

static
PACKEDCOLORMAIN_T * getscratchwnd(void)
{
	return NULL;
}

// Stub
static void display2_latchwaterfall(
	uint_fast8_t x0, 
	uint_fast8_t y0, 
	dctx_t * pctx
	)
{
}

// Stub
static void display2_spectrum(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
}

// Stub
static void display2_waterfall(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
}

// Stub
static void display2_colorbuff(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{

}

// Stub
static void display2_wfl_init(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{

}

#endif /* WITHSPECTRUMWF && ! LCDMODE_HD44780 && ! LCDMODE_DUMMY */

#if WITHSHOWSWRPWR
uint_fast8_t display2_getswrmax(void)
{
	return SWRMAX;
}
#endif /* WITHSHOWSWRPWR */

#define STMD 1

#if STMD

static uint_fast8_t
validforredraw(
	const FLASHMEM struct dzone * const p,
	uint_fast8_t key,
	uint_fast8_t subset
	)
{
#if LCDMODE_MAIN_PAGES > 1
	/* про off-screen composition отрисовываем все элементы вне
	 * зависимости от группы обновления, но для подходящей страницы.
	 */
	if (/*(p->key != key) || */(p->subset & subset) == 0)
		return 0;
#else /* LCDMODE_MAIN_PAGES > 1 */
	if ((p->key != key) || (p->subset & subset) == 0)
		return 0;
#endif /* LCDMODE_MAIN_PAGES > 1 */
	return 1;
}

// Параметры state machine отображения
static uint8_t reqs [REDRM_count];		// запросы на отображение
static uint8_t subsets [REDRM_count];	// параметр прохода по списку отображения.
static uint8_t walkis [REDRM_count];	// индекс в списке параметров отображения в данном проходе
static uint_fast8_t keyi;					// запрос на отображение, выполняющийся сейчас.

#endif /* STMD */

static uint_fast8_t
getsubset(
	uint_fast8_t menuset,	/* индекс режима отображения (0..DISPLC_MODCOUNT - 1) */
	uint_fast8_t extra		/* находимся в режиме отображения настроек */
	)
{
	return extra ? REDRSUBSET_MENU : REDRSUBSET(menuset);
}

// выполнение отрисовки всех элементов за раз.
// Например при работе в меню
static void 
display_walktrough(
	uint_fast8_t key,
	uint_fast8_t subset,
	dctx_t * pctx
	)
{
	enum { WALKCOUNT = sizeof dzones / sizeof dzones [0] };
	uint_fast8_t i;

	for (i = 0; i < WALKCOUNT; ++ i)
	{
		const FLASHMEM struct dzone * const p = & dzones [i];

		if (validforredraw(p, key, subset) == 0)
			continue;
		(* p->redraw)(p->x, p->y, pctx);
	#if WITHINTEGRATEDDSP
		audioproc_spool_user();		// решение проблем с прерыванием звука при стирании экрана
	#endif /* WITHINTEGRATEDDSP */
	}
}


// заказ на выполнение отрисовки всех элементов через state machine.
static void 
display_walktroughsteps(
	uint_fast8_t key,
	uint_fast8_t subset
	)
{
	ASSERT(key < REDRM_count);
#if STMD

	#if LCDMODE_MAIN_PAGES > 1

		key = 0;
		if (reqs [key] != 0)
		{
			// уже идет отрисовка
			if ((subsets [key] & subset) == 0)
			{
				// начинаем снова - другой subset
				reqs [key] = 1;
				subsets [key] = subset;
				walkis [key] = 0;
			}
		}
		else
		{
			// начинаем снова
			reqs [key] = 1;
			subsets [key] = subset;
			walkis [key] = 0;
		}

	#else /* LCDMODE_MAIN_PAGES > 1 */

		reqs [key] = 1;
		subsets [key] = subset;
		walkis [key] = 0;

	#endif /* LCDMODE_MAIN_PAGES > 1 */

#else /* STMD */

	#if LCDMODE_MAIN_PAGES > 1
		key = 0;
	#endif /* LCDMODE_MAIN_PAGES > 1 */

	display_walktrough(key, subset, display2_getcontext());

#endif /* STMD */
}

// Interface functions
// выполнение шагов state machine отображения дисплея
void display2_bgprocess(void)
{
#if STMD
	enum { WALKCOUNT = sizeof dzones / sizeof dzones [0] };
	const uint_fast8_t keyi0 = keyi;

	for (;;)
	{
		ASSERT(keyi < REDRM_count);
		if (reqs [keyi] != 0)
			break;
		keyi = (keyi == (REDRM_count - 1)) ? 0 : (keyi + 1);
		if (keyi == keyi0)
			return;			// не нашли ни одного запроса
	}

	ASSERT(keyi < REDRM_count);
	//return;
	for (; walkis [keyi] < WALKCOUNT; ++ walkis [keyi])
	{
		const FLASHMEM struct dzone * const p = & dzones [walkis [keyi]];

		if (validforredraw(p, keyi, subsets [keyi]) == 0)
			continue;
		(* p->redraw)(p->x, p->y, display2_getcontext());
		walkis [keyi] += 1;
		break;
	}
	ASSERT(keyi < REDRM_count);
	if (walkis [keyi] >= WALKCOUNT)
	{
		reqs [keyi] = 0;	// снять запрос на отображение данного типа элементов
		keyi = (keyi == (REDRM_count - 1)) ? 0 : (keyi + 1);
		ASSERT(keyi < REDRM_count);
	}

#endif /* STMD */
}

// Interface functions
// сброс state machine отображения дисплея и очистить дисплей
void display2_bgreset(void)
{
	uint_fast8_t i;

	// очистить дисплей.
	display_clear();	

#if STMD
	// сброс state machine отображения дисплея
	for (i = 0; i < REDRM_count; ++ i)
	{
		reqs [i] = 0;
		//walkis [keyi] = 0;
	}
	keyi = 0;
#endif /* STMD */
}

void display2_initialize(void)
{

#if WITHDEBUG
	{
		static int inited;
		ASSERT(inited == 0);	// Only one pass supported
		inited = 1;
	}
#endif /* WITHDEBUG */

	// параметр key игнорируеся обычно, но для случая старых дисплеев выделен особенный
	display_walktrough(REDRM_INIS, REDRSUBSET_INIT, NULL);
}

// Interface functions
void display2_mode_subset(
	uint_fast8_t menuset	/* индекс режима отображения (0..DISPLC_MODCOUNT - 1) */
	)
{
	//TP();
	display_walktroughsteps(REDRM_MODE, getsubset(menuset, 0));
}

void display2_barmeters_subset(
	uint_fast8_t menuset,	/* индекс режима отображения (0..3) */
	uint_fast8_t extra		/* находимся в режиме отображения настроек */
	)
{
#if LCDMODE_MAIN_PAGES > 1
	if (extra)
		return;
	//TP();
	display_walktroughsteps(0, getsubset(menuset, extra));
#else /* LCDMODE_MAIN_PAGES > 1 */
	display_walktroughsteps(REDRM_BARS, getsubset(menuset, extra));
#endif /* LCDMODE_MAIN_PAGES > 1 */
}

void display2_volts(
	uint_fast8_t menuset,	/* индекс режима отображения (0..DISPLC_MODCOUNT - 1) */
	uint_fast8_t extra		/* находимся в режиме отображения настроек */
	)
{
#if LCDMODE_MAIN_PAGES > 1
	if (extra)
		return;
	//TP();
	display_walktroughsteps(0, getsubset(menuset, extra));
#else /* LCDMODE_MAIN_PAGES > 1 */
	display_walktroughsteps(REDRM_VOLT, getsubset(menuset, extra));
#endif /* LCDMODE_MAIN_PAGES > 1 */
}

void display2_dispfreq_ab(
	uint_fast8_t menuset	/* индекс режима отображения (0..DISPLC_MODCOUNT - 1) */
	)
{
	//TP();
#if LCDMODE_MAIN_PAGES > 1
	display_walktroughsteps(0, getsubset(menuset, 0));

#else /* LCDMODE_MAIN_PAGES > 1 */
	display_walktroughsteps(REDRM_FREQ, getsubset(menuset, 0));
	display_walktroughsteps(REDRM_FRQB, getsubset(menuset, 0));

#endif /* LCDMODE_MAIN_PAGES > 1 */
}

void display2_dispfreq_a2(
	uint_fast32_t freq,
	uint_fast8_t blinkpos,		// позиция (степень 10) редактируесого символа
	uint_fast8_t blinkstate,	// в месте редактируемого символа отображается подчёркивание (0 - пробел)
	uint_fast8_t menuset	/* индекс режима отображения (0..DISPLC_MODCOUNT - 1) */
	)
{
	//TP();
#if WITHDIRECTFREQENER

	editfreq2_t ef;
	dctx_t ctx;

	ef.freq = freq;
	ef.blinkpos = blinkpos;
	ef.blinkstate = blinkstate;

	ctx.type = DCTX_FREQ;
	ctx.pv = & ef;

	display_walktrough(REDRM_FREQ,  getsubset(menuset, 0), & ctx);

#else	/* WITHDIRECTFREQENER */

	display_walktroughsteps(REDRM_FREQ,  getsubset(menuset, 0));

#endif /* WITHDIRECTFREQENER */
}

// Обновление изоражения экрана при нахождении в режиме меню
// Замена группы вызовов следующих трех функций:
//	display2_menuitemlabel(mp, byname);
//	display2_menuitemvalue(mp);
//	display2_redrawbars(1, 1);		/* обновление динамической части отображения - обновление S-метра или SWR-метра и volt-метра. */
//
// Если указывает на элемент value - на обычных дисплеях жквивалентна вызову display2_menuitemvalue
//
void display2_menu(
	const FLASHMEM struct menudef * mp,
	uint_fast8_t byname			/* был выполнен прямой вход в меню */
	)
{
	//PRINTF("display2_menu: mp=%p\n", mp);

	dctx_t dctx;
	dctx.type = DCTX_MENU;
	dctx.pv = mp;

#if LCDMODE_MAIN_PAGES > 1

	display_walktrough(0, REDRSUBSET_MENU, & dctx);

#else /* LCDMODE_MAIN_PAGES > 1 */

	display_walktrough(REDRM_FREQ, REDRSUBSET_MENU, NULL);
	display_walktrough(REDRM_FRQB, REDRSUBSET_MENU, NULL);
	display_walktrough(REDRM_MODE, REDRSUBSET_MENU, NULL);
	if (byname == 0)
	{
		display_walktrough(REDRM_MFXX, REDRSUBSET_MENU, & dctx);
	}
	display_walktrough(REDRM_MLBL, REDRSUBSET_MENU, & dctx);
	display_walktrough(REDRM_MVAL, REDRSUBSET_MENU, & dctx);

#endif /* LCDMODE_MAIN_PAGES > 1 */
}

// последний номер варианта отображения (menuset)
uint_fast8_t display_getpagesmax(void)
{
	return DISPLC_MODCOUNT - 1;
}

// номер варианта отображения для "сна"
uint_fast8_t display_getpagesleep(void)
{
	return PAGESLEEP;
}

// получить параметры отображения частоты (для функции прямого ввода)
uint_fast8_t display_getfreqformat(
	uint_fast8_t * prjv
	)
{
	* prjv = DISPLC_RJ;
	return DISPLC_WIDTH;
}

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
	if (hamradio_get_tx())
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

#if LCDMODE_LTDC

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

// Установка параметров отображения
/* дизайн спектра под 3DSS Yaesu */
void
board_set_view_style(uint_fast8_t v)
{
#if WITHINTEGRATEDDSP
	const uint_fast8_t n = v;

	if (glob_view_style != n)
	{
		glob_view_style = n;
#if WITHVIEW_3DSS
		wfsetupnew0();	// при переключении стилей отображения очищать общий буфер

		if (glob_view_style == VIEW_3DSS)
			init_depth_map_3dss();
#endif /* WITHVIEW_3DSS */
	}
#endif /* WITHINTEGRATEDDSP */
}

/* Для VIEW_3DSS - индикация полосы пропускания на спектре */
void
board_set_view3dss_mark(int_fast16_t v)
{
	gview3dss_mark = v;
}

/* верхний предел FFT - spectrum */
void
board_set_topdb(int_fast16_t v)
{
	glob_topdb = v;
}

/* нижний предел FFT - spectrum */
void
board_set_bottomdb(int_fast16_t v)
{
	glob_bottomdb = v;
}

/* Шаг сетки уровней в децибелах. (0-отключаем отображение сетки уровней) */
void
board_set_lvlgridstep(uint_fast8_t v)
{
	glob_lvlgridstep = v;
}

/* верхний предел FFT - waterflow */
void
board_set_topdbwf(int_fast16_t v)
{
	glob_topdbwf = v;
}

/* нижний предел FFT - waterflow*/
void
board_set_bottomdbwf(int_fast16_t v)
{
	glob_bottomdbwf = v;
}

/* уменьшение отображаемого участка спектра */
// horisontal magnification power of two
void
board_set_zoomxpow2(uint_fast8_t v)
{
	glob_zoomxpow2 = v;
}

/* чувствительность водопада регулируется отдельной парой параметров */
void
board_set_wflevelsep(uint_fast8_t v)
{
	glob_wflevelsep = v != 0;
}

// Отображение уровня сигнала в dBm или S-memter (в зависимости от настроек)
void
board_set_showdbm(uint_fast8_t v)
{
	glob_showdbm = v != 0;
}

// нижняя частота отображения спектроанализатора
void
board_set_afspeclow(int_fast16_t v)
{
	glob_afspeclow = v;
}

// верхняя частота отображения спектроанализатора
void
board_set_afspechigh(int_fast16_t v)
{
	glob_afspechigh = v;
}

void display2_set_smetertype(uint_fast8_t v)
{
	ASSERT(v < SMETER_TYPE_COUNT);
	glob_smetertype = v;
}
