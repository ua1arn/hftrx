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

#include "dspdefines.h"

#if WITHALTERNATIVEFONTS
	#include "display/fonts/ub_fonts.h"
#endif /* WITHALTERNATIVEFONTS */

#if WITHRLEDECOMPRESS
	#include "display/pictures_RLE.h"
#endif /* WITHRLEDECOMPRESS */

#define WITHPLACEHOLDERS 1	//  отображение макета с еще незанятыми полями

#if LCDMODE_LTDC

	#include "display/fontmaps.h"

	static PACKEDCOLORPIP_T * getscratchwnd(void);

#elif WITHSPECTRUMWF

	static PACKEDCOLORPIP_T * getscratchwnd(void);

#endif /* LCDMODE_LTDC */

static const COLORPIP_T colors_2state_alt [2] = { COLORPIP_GRAY, COLORPIP_WHITE, };
static const COLORPIP_T color_alt_red = COLORPIP_RED;

void layout_label1_medium(uint_fast8_t xgrid, uint_fast8_t ygrid, const char * str, size_t slen, uint_fast8_t chars_W2, COLORPIP_T color_fg, COLORPIP_T color_bg);

#if WITHALTERNATIVELAYOUT

#if SMALLCHARW2

typedef struct {
	uint_fast8_t chars_W2;
	const COLORPIP_T * pcolor;
	PACKEDCOLORPIP_T * label_bg;	/* буфер */
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
		lbl->size = GXSIZE(lbl->w, lbl->h) * sizeof (PACKEDCOLORPIP_T);
		lbl->label_bg = (PACKEDCOLORPIP_T *) malloc(lbl->size);
		ASSERT(lbl->label_bg != NULL);
		colpip_fillrect(lbl->label_bg, lbl->w, lbl->h, 0, 0, lbl->w, lbl->h, COLORPIP_BLACK);
		colmain_rounded_rect(lbl->label_bg, lbl->w, lbl->h, 0, 0, lbl->w - 1, lbl->h - 1, 5, * lbl->pcolor, 1);
	} while (++ i < ARRAY_SIZE(label_bg));
}

void layout_label1_medium(uint_fast8_t xgrid, uint_fast8_t ygrid, const char * str, size_t slen, uint_fast8_t chars_W2, COLORPIP_T color_fg, COLORPIP_T color_bg)
{
	PACKEDCOLORPIP_T * const fr = colmain_fb_draw();
	uint_fast16_t xx = GRID2X(xgrid);
	uint_fast16_t yy = GRID2Y(ygrid);
	label_bg_t * lbl_bg = NULL;
	uint_fast8_t i = 0;
	char buf [slen + 1];
	strcpy(buf, str);
	strtrim(buf);
#if WITHALTERNATIVEFONTS
	const uint_fast16_t width_str = getwidth_Pstring(buf, & gothic_12x16_p);
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

	if (lbl_bg != NULL && lbl_bg->label_bg != NULL)
	{
		colpip_bitblt((uintptr_t) fr, GXSIZE(DIM_X, DIM_Y), fr, DIM_X, DIM_Y, xx, yy,
				(uintptr_t) lbl_bg->label_bg, lbl_bg->size, lbl_bg->label_bg, lbl_bg->w, lbl_bg->h,
				0, 0,	// координаты окна источника
				lbl_bg->w, lbl_bg->h, //размер окна источника
				BITBLT_FLAG_NONE, 0);
	}
	else
	{
		colmain_rounded_rect(fr, DIM_X, DIM_Y, xx, yy, xx + width_p, yy + SMALLCHARH2 + 5, 5, color_bg, 1);
	}

#if WITHALTERNATIVEFONTS
	//PRINTF("%s: xx=%d, width_p=%d, width_str=%d, buf='%s'\n", __func__, xx, width_p, width_str, buf);
	UB_Font_DrawPString(fr, DIM_X, DIM_Y, xx + (width_p - width_str) / 2 , yy + 2, buf, & gothic_12x16_p, color_fg);
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
#elif COLORSTYLE_GREEN && 0
	static uint_fast8_t glob_colorstyle = GRADIENT_BLACK_GREEN;
#else /* */
	static uint_fast8_t glob_colorstyle = GRADIENT_BLUE_YELLOW_RED;
#endif /* COLORSTYLE_RED */

#if WITHSPECTRUMWF

#if WITHGRADIENT_FIXED
    #include "pancolor.h"
#endif /* WITHGRADIENT_FIXED */

// get color from signal strength
// Get FFT color warmth (blue to red)
COLOR24_T colorgradient(unsigned pos, unsigned maxpos)
{
#if WITHGRADIENT_FIXED
	// построение цветных градиентов по готовой таблице.
	const COLOR24_T c = pancolor [ARRAY_SIZE(pancolor) - 1 - normalize(pos, 0, maxpos, ARRAY_SIZE(pancolor) - 1)];
	return COLOR24(COLOR24_R(c), COLOR24_G(c), COLOR24_B(c));
#endif
	// построение цветных градиентов от UA3REO
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
	COLORPIP_T fg, bg;
} COLORPAIR_T;

// todo: учесть LCDMODE_COLORED

// Параметры отображения состояния прием/пеердача
static const COLORPAIR_T colors_2rxtx [2] =
{
	{	COLORPIP_GREEN,	COLORPIP_BLACK,	},	// RX
	{	COLORPIP_RED,		COLORPIP_BLACK,	},	// TX
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

// Параметры отображения состояний из двух вариантов (активный - на красном фонк)
static const COLORPAIR_T colors_2state_rec [2] =
{
	{	LABELINACTIVETEXT,	LABELINACTIVEBACK,	},
	{	COLORPIP_RED,	COLORPIP_BLACK,	},
};

// Параметры отображения текстов без вариантов
static const COLORPAIR_T colors_1state [1] =
{
	{	LABELTEXT,	LABELBACK,	},
};

// Параметры отображения текстов без вариантов
static const COLORPAIR_T colors_1statevoltage [1] =
{
	{	DESIGNCOLORSTATETEXT,	DESIGNCOLORSTATEBACK,	},
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
static const COLORPAIR_T colors_2freqB [2] =
{
	{	DESIGNBIGCOLORBINACTIVE,	LABELBACK,	},
	{	DESIGNBIGCOLORB,	LABELBACK,	},
};
// Параметры отображения режима дополнительного приемника
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

#if (WITHSPECTRUMWF && ! LCDMODE_HD44780 && ! LCDMODE_DUMMY) || (WITHAFSPECTRE && ! LCDMODE_DUMMY)

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
	//colpip_fillrect(colmain_fb_draw(), DIM_X, DIM_Y, x, y, 100, 100, COLORPIP_GREEN);

	colpip_string_tbg(colmain_fb_draw(), DIM_X, DIM_Y, x, y + 0, "Hello", COLORPIP_WHITE);
	colpip_string_x2_tbg(colmain_fb_draw(), DIM_X, DIM_Y, x, y + 20, "Test", COLORPIP_WHITE);
	colpip_string_tbg(colmain_fb_draw(), DIM_X, DIM_Y, x, y + 50, "Test", COLORPIP_WHITE);

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
static uint_fast8_t glob_rxbwsatu = 100;	// 0..100 - насыщнность цвета заполнения "шторки" - индикатор полосы пропускания примника на спкктре.

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

static void fftzoom_af(FLOAT_t * buffer, unsigned zoompow2, unsigned normalFFT);

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

#if LCDMODE_LTDC && WITHBARS

enum {
	SM_STATE_RX,
	SM_STATE_TX,
	SM_STATE_COUNT
};
enum { SM_BG_W = GRID2X(15), SM_BG_H = GRID2Y(20) };

typedef ALIGNX_BEGIN PACKEDCOLORPIP_T smeter_bg_t [GXSIZE(SM_BG_W, SM_BG_H)] ALIGNX_END;
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
#if WITHRLEDECOMPRESS
			smpr->gs = 0;
			smpr->gm = 108;
			smpr->ge = 206;
#else
			smpr->gm = 270;
			smpr->gs = smpr->gm - halfsect;
			smpr->ge = smpr->gm + halfsect;
#endif /* WITHRLEDECOMPRESS */
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

	const uint_fast16_t markersTX_pwr [] =
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
	const uint_fast16_t markersTX_swr [] =
	{
		smpr->gs,
		smpr->gs + smpr->step3,
		smpr->gs + 2 * smpr->step3,
		smpr->gs + 3 * smpr->step3,
	};
	const uint_fast16_t markers [] =
	{
		//smpr->gs + 0 * smpr->step1,
		smpr->gs + 2 * smpr->step1,		// S1
		smpr->gs + 4 * smpr->step1,		// S3
		smpr->gs + 6 * smpr->step1,		// S5
		smpr->gs + 8 * smpr->step1,		// S7
		smpr->gs + 10 * smpr->step1,	// S9
	};
	const uint_fast16_t markersR [] =
	{
		smpr->gm + 2 * smpr->step2,	//
		smpr->gm + 4 * smpr->step2,
		smpr->gm + 6 * smpr->step2,
	};
	const uint_fast16_t markers2 [] =
	{
		//smpr->gs + 1 * smpr->step1,
		smpr->gs + 3 * smpr->step1,		// S2
		smpr->gs + 5 * smpr->step1,		// S4
		smpr->gs + 7 * smpr->step1,		// S6
		smpr->gs + 9 * smpr->step1,		// S8
	};
	const uint_fast16_t markers2R [] =
	{
		smpr->gm + 1 * smpr->step2,
		smpr->gm + 3 * smpr->step2,
		smpr->gm + 5 * smpr->step2,
	};

	const COLORPIP_T smeter = COLORPIP_WHITE;
	const COLORPIP_T smeterplus = COLORPIP_DARKRED;
	const uint_fast16_t pad2w3 = strwidth3("ZZ");

	PACKEDCOLORPIP_T * bg;
	uint_fast8_t xb = 120, yb = 120;
	unsigned p;
	unsigned i;

	switch (smetertype)
	{

	case SMETER_TYPE_DIAL:

		bg = smeter_bg [SMETER_TYPE_DIAL][SM_STATE_TX];
		ASSERT(bg != NULL);
#if WITHRLEDECOMPRESS
		graw_picture_RLE_buf(bg, SM_BG_W, SM_BG_H, 0, 0, & smeter_bg_new, COLORPIP_BLACK);
#else
		colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H - 1, COLORPIP_BLACK, 1);

		for (p = 0, i = 0; i < ARRAY_SIZE(markersTX_pwr) - 1; ++ i, p += 10)
		{
			if (i % 2 == 0)
			{
				char buf2 [10];
				uint_fast16_t xx, yy;

				colpip_radius(bg, SM_BG_W, SM_BG_H, xb, yb, markersTX_pwr [i], smpr->r1, smpr->r1 + 8, smeter, 1, 1);
				polar_to_dek(xb, yb, markersTX_pwr [i], smpr->r1 + 6, & xx, & yy, 1);
				local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%u"), p);
				colpip_string3_tbg(bg, SM_BG_W, SM_BG_H, xx - strwidth3(buf2) / 2, yy - pad2w3 + 1, buf2, COLORPIP_YELLOW);
			}
			else
				colpip_radius(bg, SM_BG_W, SM_BG_H, xb, yb, markersTX_pwr [i], smpr->r1, smpr->r1 + 4, smeter, 1, 1);
		}

		for (p = 1, i = 0; i < ARRAY_SIZE(markersTX_swr); ++ i, p += 1)
		{
			char buf2 [10];
			uint_fast16_t xx, yy;

			colpip_radius(bg, SM_BG_W, SM_BG_H, xb, yb, markersTX_swr [i], smpr->r2, smpr->r2 - 8, smeter, 1, 1);
			polar_to_dek(xb, yb, markersTX_swr [i], smpr->r2 - 16, & xx, & yy, 1);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%u"), p);
			colpip_string3_tbg(bg, SM_BG_W, SM_BG_H, xx - SMALLCHARW3 / 2, yy - SMALLCHARW3 / 2 + 1, buf2, COLORPIP_YELLOW);
		}
		colpip_segm(bg, SM_BG_W, SM_BG_H, xb, yb, smpr->gs, smpr->gm, smpr->r1, 1, smeter, 1, 1);
		colpip_segm(bg, SM_BG_W, SM_BG_H, xb, yb, smpr->gm, smpr->ge, smpr->r1, 1, smeter, 1, 1);
		colpip_segm(bg, SM_BG_W, SM_BG_H, xb, yb, smpr->gs, smpr->ge, smpr->r2, 1, COLORPIP_WHITE, 1, 1);
#endif /* WITHRLEDECOMPRESS */

		bg = smeter_bg [SMETER_TYPE_DIAL][SM_STATE_RX];
		ASSERT(bg != NULL);
#if WITHRLEDECOMPRESS
		graw_picture_RLE_buf(bg, SM_BG_W, SM_BG_H, 0, 0, & smeter_bg_new, COLORPIP_BLACK);
#else
		colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H - 1, COLORPIP_BLACK, 1);

		for (p = 1, i = 0; i < ARRAY_SIZE(markers); ++ i, p += 2)
		{
			char buf2 [10];
			uint_fast16_t xx, yy;

			colpip_radius(bg, SM_BG_W, SM_BG_H, xb, yb, markers [i], smpr->r1, smpr->r1 + 8, smeter, 1, 1);
			polar_to_dek(xb, yb, markers [i], smpr->r1 + 6, & xx, & yy, 1);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%u"), p);
			colpip_string3_tbg(bg, SM_BG_W, SM_BG_H, xx - SMALLCHARW3 / 2, yy - pad2w3 + 1, buf2, COLORPIP_YELLOW);
		}
		for (i = 0; i < ARRAY_SIZE(markers2); ++ i)
		{
			colpip_radius(bg, SM_BG_W, SM_BG_H, xb, yb, markers2 [i], smpr->r1, smpr->r1 + 4, smeter, 1, 1);
		}

		for (p = 20, i = 0; i < ARRAY_SIZE(markersR); ++ i, p += 20)
		{
			char buf2 [10];
			uint_fast16_t xx, yy;

			colpip_radius(bg, SM_BG_W, SM_BG_H, xb, yb, markersR [i], smpr->r1, smpr->r1 + 8, smeterplus, 1, 1);
			polar_to_dek(xb, yb, markersR [i], smpr->r1 + 6, & xx, & yy, 1);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("+%u"), p);
			colpip_string3_tbg(bg, SM_BG_W, SM_BG_H, xx - strwidth3(buf2) / 2, yy - pad2w3 + 1, buf2, COLORPIP_RED);
		}
		for (i = 0; i < ARRAY_SIZE(markers2R); ++ i)
		{
			colpip_radius(bg, SM_BG_W, SM_BG_H, xb, yb, markers2R [i], smpr->r1, smpr->r1 + 4, smeterplus, 1, 1);
		}
		colpip_segm(bg, SM_BG_W, SM_BG_H, xb, yb, smpr->gs, smpr->gm, smpr->r1, 1, smeter, 1, 1);
		colpip_segm(bg, SM_BG_W, SM_BG_H, xb, yb, smpr->gm, smpr->ge, smpr->r1, 1, smeterplus, 1, 1);
		colpip_segm(bg, SM_BG_W, SM_BG_H, xb, yb, smpr->gs, smpr->ge, smpr->r2, 1, COLORPIP_WHITE, 1, 1);
#endif /* WITHRLEDECOMPRESS */
		break;

	default:
	case SMETER_TYPE_BARS:

		bg = smeter_bg [SMETER_TYPE_BARS][SM_STATE_TX];
		ASSERT(bg != NULL);
		colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H - 1, COLORPIP_BLACK, 1);
//		colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H - 1, COLORPIP_WHITE, 0);
//		colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H / 2, COLORPIP_WHITE, 0);

		colpip_line(bg, SM_BG_W, SM_BG_H, smpr->gs, smpr->r1, smpr->ge, smpr->r1, COLORPIP_WHITE, 0);
		for (p = 0, i = 0; i < ARRAY_SIZE(markersTX_pwr); ++ i, p += 10)
		{
			if (i % 2 == 0)
			{
				char buf2 [10];
				colpip_line(bg, SM_BG_W, SM_BG_H, markersTX_pwr [i], smpr->r1, markersTX_pwr [i], smpr->r1 - 10, COLORPIP_WHITE, 0);
				local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%u"), p);
				colpip_string3_tbg(bg, SM_BG_W, SM_BG_H, markersTX_pwr [i] - strwidth3(buf2) / 2, smpr->r1 - 10 - SMALLCHARH3 - 2, buf2, COLORPIP_YELLOW);
			}
			else
				colpip_line(bg, SM_BG_W, SM_BG_H, markersTX_pwr [i], smpr->r1, markersTX_pwr [i], smpr->r1 - 5, COLORPIP_WHITE, 0);
		}

		colpip_line(bg, SM_BG_W, SM_BG_H, smpr->gs, smpr->r2, smpr->ge, smpr->r2, COLORPIP_WHITE, 0);
		for (p = 1, i = 0; i < ARRAY_SIZE(markersTX_swr); ++ i, p += 1)
		{
			char buf2 [10];
			colpip_line(bg, SM_BG_W, SM_BG_H, markersTX_swr [i], smpr->r2, markersTX_swr [i], smpr->r2 + 10, COLORPIP_WHITE, 0);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%u"), p);
			colpip_string3_tbg(bg, SM_BG_W, SM_BG_H, markersTX_swr [i] - strwidth3(buf2) / 2, smpr->r2 + 12, buf2, COLORPIP_YELLOW);
		}

		bg = smeter_bg [SMETER_TYPE_BARS][SM_STATE_RX];
		colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H - 1, COLORPIP_BLACK, 1);
//		colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H - 1, COLORPIP_WHITE, 0);
//		colpip_rect(bg, SM_BG_W, SM_BG_H, 0, 0, SM_BG_W - 1, SM_BG_H / 2, COLORPIP_WHITE, 0);

		colpip_line(bg, SM_BG_W, SM_BG_H, smpr->gs, smpr->r1, smpr->gm, smpr->r1, COLORPIP_WHITE, 0);
		colpip_line(bg, SM_BG_W, SM_BG_H, smpr->gm, smpr->r1, smpr->ge, smpr->r1, COLORPIP_RED, 0);
		colpip_string2_tbg(bg, SM_BG_W, SM_BG_H, smpr->gs - SMALLCHARW2, smpr->r1 - SMALLCHARH2 - 2, "Sm", COLORPIP_YELLOW);

		for (p = 1, i = 0; i < ARRAY_SIZE(markers); ++ i, p += 2)
		{
			char buf2 [10];
			uint_fast16_t xx, yy;
			colpip_line(bg, SM_BG_W, SM_BG_H, markers [i], smpr->r1, markers [i], smpr->r1 - 10, COLORPIP_WHITE, 0);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%u"), p);
			colpip_string3_tbg(bg, SM_BG_W, SM_BG_H, markers [i] - SMALLCHARW3 / 2, smpr->r1 - 10 - SMALLCHARH3 - 2, buf2, COLORPIP_YELLOW);
		}
		for (i = 0; i < ARRAY_SIZE(markers2); ++ i)
		{
			colpip_line(bg, SM_BG_W, SM_BG_H, markers2 [i], smpr->r1, markers2 [i], smpr->r1 - 5, COLORPIP_WHITE, 0);
		}

		for (p = 20, i = 0; i < ARRAY_SIZE(markersR); ++ i, p += 20)
		{
			char buf2 [10];
			colpip_line(bg, SM_BG_W, SM_BG_H, markersR [i], smpr->r1, markersR [i], smpr->r1 - 10, COLORPIP_RED, 0);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("+%u"), p);
			colpip_string3_tbg(bg, SM_BG_W, SM_BG_H, markersR [i] - strwidth3(buf2) / 2, smpr->r1 - 10 - SMALLCHARH3 - 2, buf2, COLORPIP_YELLOW);
		}
		for (i = 0; i < ARRAY_SIZE(markers2R); ++ i)
		{
			colpip_line(bg, SM_BG_W, SM_BG_H, markers2R [i], smpr->r1, markers2R [i], smpr->r1 - 5, COLORPIP_RED, 0);
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

static void smeter_arrow(uint_fast16_t target_pixel_x, uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, COLORPIP_T color)
{
	if (target_pixel_x > 220)
		target_pixel_x = 220;
	float32_t x0 = x + w / 2 + 2;
	float32_t y0 = y + h + 140;
	float32_t x1 = x + target_pixel_x;
	float32_t y1 = y;

	PACKEDCOLORPIP_T * const fr = colmain_fb_draw();

	// length cut
	const uint32_t max_length = 220;
	float32_t x_diff = 0;
	float32_t y_diff = 0;
	float32_t length = sqrtf((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
	if (length > max_length)
	{
		float32_t coeff = (float32_t) max_length / length;
		x_diff = (x1 - x0) * coeff;
		y_diff = (y1 - y0) * coeff;
		x1 = x0 + x_diff;
		y1 = y0 + y_diff;
	}
	// right cut
	uint_fast16_t tryes = 0;
	while ((x1 > x + w) && tryes < 100)
	{
		x_diff = (x1 - x0) * 0.99f;
		y_diff = (y1 - y0) * 0.99f;
		x1 = x0 + x_diff;
		y1 = y0 + y_diff;
		tryes ++;
	}
	// left cut
	tryes = 0;
	while ((x1 < x) && tryes < 100)
	{
		x_diff = (x1 - x0) * 0.99f;
		y_diff = (y1 - y0) * 0.99f;
		x1 = x0 + x_diff;
		y1 = y0 + y_diff;
		tryes ++;
	}
	// start cut
	tryes = 0;
	while ((y0 > y + h) && tryes < 150)
	{
		x_diff = (x1 - x0) * 0.99f;
		y_diff = (y1 - y0) * 0.99f;
		x0 = x1 - x_diff;
		y0 = y1 - y_diff;
		tryes ++;
	}

	// draw
	if (x1 < x0)
	{
		colpip_line(fr, DIM_X, DIM_Y, x0, y0, x1, y1, color, 1);
		colpip_line(fr, DIM_X, DIM_Y, x0 + 1, y0, x1 + 1, y1, color, 1);
	}
	else
	{
		colpip_line(fr, DIM_X, DIM_Y, x0, y0, x1, y1, color, 1);
		colpip_line(fr, DIM_X, DIM_Y, x0 - 1, y0, x1 - 1, y1, color, 1);
	}
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
	PACKEDCOLORPIP_T * const fr = colmain_fb_draw();
	static uint_fast8_t first_tx = 0;

	int gp = smpr->gs, gv = smpr->gs, gv_trace = smpr->gs, gswr = smpr->gs;

	//colpip_rect(colmain_fb_draw(), DIM_X, DIM_Y, x0, y0, x0 + width - 1, y0 + height - 1, COLORPIP_GREEN, 1);

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
#if WITHRLEDECOMPRESS
		uint_fast16_t tracemax;
		uint_fast16_t value = dsp_getsmeter10(& tracemax, 0, UINT8_MAX * 10, 0);
		tracemax = value > tracemax ? value : tracemax;	// защита от рассогласования значений

		gv = normalize3(value, (s9level - s9delta) * 10, s9level * 10, (s9level + s9_60_delta) * 10, smpr->gm - smpr->gs, smpr->ge - smpr->gs);
#else
		uint_fast8_t tracemax;
		uint_fast8_t value = board_getsmeter(& tracemax, 0, UINT8_MAX, 0);
		tracemax = value > tracemax ? value : tracemax;	// защита от рассогласования значений

		gv =
			smpr->gs + normalize3(value, 	s9level - s9delta, s9level, s9level + s9_60_delta, smpr->gm - smpr->gs, smpr->ge - smpr->gs);
		gv_trace =
			smpr->gs + normalize3(tracemax, s9level - s9delta, s9level, s9level + s9_60_delta, smpr->gm - smpr->gs, smpr->ge - smpr->gs);
#endif

		first_tx = 1;
	}

	switch (glob_smetertype)
	{

	case SMETER_TYPE_DIAL:

		if (is_tx)
		{
			// TX state
			colpip_bitblt(
					(uintptr_t) fr, GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORPIP_T),
					fr, DIM_X, DIM_Y, x0, y0 + dial_shift,
					(uintptr_t) smeter_bg [SMETER_TYPE_DIAL][SM_STATE_TX], GXSIZE(SM_BG_W, SM_BG_H) * sizeof (PACKEDCOLORPIP_T),
					smeter_bg [SMETER_TYPE_DIAL][SM_STATE_TX], SM_BG_W, SM_BG_H,
					0, 0,	// координаты окна источника
					SM_BG_W, SM_BG_H - dial_shift, // размер окна источника
					BITBLT_FLAG_NONE, 0);
#if WITHRLEDECOMPRESS
			smeter_arrow(gp, x0, y0 + dial_shift, smeter_bg_new.width, smeter_bg_new.height, COLOR_GRAY);
#else
			if (gswr > smpr->gs)
			{
				uint_fast16_t xx, yy;
				const COLORPIP_T color = COLORPIP_YELLOW;

				colpip_segm(fr, DIM_X, DIM_Y, xc, yc, smpr->gs, gswr, smpr->r2 + 2, 1, color, 0, 1);
				colpip_segm(fr, DIM_X, DIM_Y, xc, yc, smpr->gs, gswr, smpr->r1 - 2, 1, color, 0, 1);
				colpip_radius(fr, DIM_X, DIM_Y, xc, yc, smpr->gs, smpr->r1 - 2, smpr->r2 + 2, color, 0, 1);
				colpip_radius(fr, DIM_X, DIM_Y, xc, yc, gswr, smpr->r1 - 2, smpr->r2 + 2, color, 0, 1);
				polar_to_dek(xc, yc, gswr - 1, smpr->r1 - 4, & xx, & yy, 1);
				display_floodfill(fr, DIM_X, DIM_Y, xx, yy, color, COLORPIP_BLACK, 1);
			}

			const COLORPIP_T color = COLORPIP_GREEN;
			colpip_radius(fr, DIM_X, DIM_Y, xc - 1, yc, gp, smpr->rv1, smpr->rv2, color, 0, 1);
			colpip_radius(fr, DIM_X, DIM_Y, xc, yc, gp, smpr->rv1, smpr->rv2, color, 0, 1);
			colpip_radius(fr, DIM_X, DIM_Y, xc + 1, yc, gp, smpr->rv1, smpr->rv2, color, 0, 1);
#endif /* WITHRLEDECOMPRESS */
		}

		else
		{
			// RX state
			colpip_bitblt(
					(uintptr_t) fr, GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORPIP_T),
					fr, DIM_X, DIM_Y, x0, y0 + dial_shift,
					(uintptr_t) smeter_bg [SMETER_TYPE_DIAL][SM_STATE_RX], GXSIZE(SM_BG_W, SM_BG_H) * sizeof (PACKEDCOLORPIP_T),
					smeter_bg [SMETER_TYPE_DIAL][SM_STATE_RX], SM_BG_W, SM_BG_H,
					0, 0,	// координаты окна источника
					SM_BG_W, SM_BG_H - dial_shift, // размер окна источника
					BITBLT_FLAG_NONE, 0);
#if WITHRLEDECOMPRESS
			smeter_arrow(gv, x0, y0 + dial_shift, smeter_bg_new.width, smeter_bg_new.height, COLOR_GRAY);
#else
			{
				// Рисование peak value (риска)
				const COLORPIP_T color = COLORPIP_YELLOW;
				colpip_radius(fr, DIM_X, DIM_Y, xc - 1, yc, gv_trace, smpr->r1 - 2, smpr->r2 + 2, color, 0, 1);
				colpip_radius(fr, DIM_X, DIM_Y, xc, yc, gv_trace, smpr->r1 - 2, smpr->r2 + 2, color, 0, 1);
				colpip_radius(fr, DIM_X, DIM_Y, xc + 1, yc, gv_trace, smpr->r1 - 2, smpr->r2 + 2, color, 0, 1);
			}

			{
				// Рисование стрелки
				const COLORPIP_T color = COLORPIP_GREEN;
				colpip_radius(fr, DIM_X, DIM_Y, xc - 1, yc, gv, smpr->rv1, smpr->rv2, color, 0, 1);
				colpip_radius(fr, DIM_X, DIM_Y, xc, yc, gv, smpr->rv1, smpr->rv2, color, 0, 1);
				colpip_radius(fr, DIM_X, DIM_Y, xc + 1, yc, gv, smpr->rv1, smpr->rv2, color, 0, 1);
			}
#endif /* WITHRLEDECOMPRESS */
		}

#if WITHAA
		display_do_AA(fr, DIM_X, DIM_Y, x0, y0, SM_BG_W, SM_BG_H);
#endif /* WITHAA */

		break;

	case SMETER_TYPE_BARS:

		if (is_tx)
		{
			colpip_bitblt(
					(uintptr_t) fr, GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORPIP_T),
					fr, DIM_X, DIM_Y, x0, y0,
					(uintptr_t) smeter_bg [SMETER_TYPE_BARS][SM_STATE_TX], GXSIZE(SM_BG_W, SM_BG_H) * sizeof (PACKEDCOLORPIP_T),
					smeter_bg [SMETER_TYPE_BARS][SM_STATE_TX], SM_BG_W, SM_BG_H,
					0, 0,	// координаты окна источника
					SM_BG_W, SM_BG_H, // размер окна источника
					BITBLT_FLAG_NONE, 0);

			if(gp > smpr->gs)
				colpip_rect(fr, DIM_X, DIM_Y, x0 + smpr->gs, y0 + smpr->r1 + 5, x0 + gp, y0 + smpr->r1 + 20, COLORPIP_GREEN, 1);

			if(gswr > smpr->gs)
				colpip_rect(fr, DIM_X, DIM_Y, x0 + smpr->gs, y0 + smpr->r2 - 20, x0 + gswr, y0 + smpr->r2 - 5, COLORPIP_GREEN, 1);
		}
		else
		{
			colpip_bitblt(
					(uintptr_t) fr, GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORPIP_T),
					fr, DIM_X, DIM_Y, x0, y0,
					(uintptr_t) smeter_bg [SMETER_TYPE_BARS][SM_STATE_RX], GXSIZE(SM_BG_W, SM_BG_H) * sizeof (PACKEDCOLORPIP_T),
					smeter_bg [SMETER_TYPE_BARS][SM_STATE_RX], SM_BG_W, SM_BG_H,
					0, 0,	// координаты окна источника
					SM_BG_W, SM_BG_H,	// размер окна источника
					BITBLT_FLAG_NONE, 0
					);

			if(gv > smpr->gs)
				colpip_rect(fr, DIM_X, DIM_Y, x0 + smpr->gs, y0 + smpr->r1 + 5, x0 + gv, y0 + smpr->r1 + 20, COLORPIP_GREEN, 1);

			if(gv_trace > smpr->gs)
				colpip_line(fr, DIM_X, DIM_Y, x0 + gv_trace, y0 + smpr->r1 + 5, x0 + gv_trace, y0 + smpr->r1 + 20, COLORPIP_YELLOW, 0);
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
#if LCDMODE_LTDC

	colpip_fillrect(colmain_fb_draw(), DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, display_getbgcolor());

#endif /* LCDMODE_LTDC */
}

// Завершение отрисовки, переключение на следующий фреймбуфер
static void
display2_nextfb(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHLTDCHW && LCDMODE_LTDC

	display_snapshot(colmain_fb_draw(), DIM_X, DIM_Y);	/* запись видимого изображения */
	display_nextfb();
#endif /* WITHLTDCHW && LCDMODE_LTDC */
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


// Подготовка отображения частоты. Герцы маленьким шрифтом.
static void display2_freqX_a_init(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHPRERENDER
	/* valid chars: "0123456789 #._" */
	colmain_setcolors3(colors_1freq [0].fg, colors_1freq [0].bg, colors_1freq [0].fg);
	render_value_big_initialize();
#endif /* WITHPRERENDER */
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
#if ! WITHPRERENDER
	colmain_setcolors3(colors_1freq [0].fg, colors_1freq [0].bg, colors_1freq [0].fg);
#endif /* ! WITHPRERENDER */
	if (pctx != NULL && pctx->type == DCTX_FREQ)
	{
#if WITHDIRECTFREQENER
		const editfreq2_t * const efp = (const editfreq2_t *) pctx->pv;
		uint_fast8_t lowhalf = HALFCOUNT_FREQA - 1;
		do
		{
#if WITHPRERENDER
			render_value_big(x, y + lowhalf, efp->freq, fullwidth, comma, comma + 3, rj, efp->blinkpos + 1, efp->blinkstate, 1, lowhalf);	// отрисовываем верхнюю часть строки
#else /* WITHPRERENDER */
			display_value_big(x, y + lowhalf, efp->freq, fullwidth, comma, comma + 3, rj, efp->blinkpos + 1, efp->blinkstate, 1, lowhalf);	// отрисовываем верхнюю часть строки
#endif /* WITHPRERENDER */
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
#if WITHPRERENDER
			render_value_big(x, y + lowhalf, freq, fullwidth, comma, comma + 3, rj, blinkpos, blinkstate, 1, lowhalf);	// отрисовываем верхнюю часть строки
#else /* WITHPRERENDER */
			display_value_big(x, y + lowhalf, freq, fullwidth, comma, comma + 3, rj, blinkpos, blinkstate, 1, lowhalf);	// отрисовываем верхнюю часть строки
#endif /* WITHPRERENDER */
		} while (lowhalf --);
	}
}

/* заглушка - в 320*200 */
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
	uint_fast8_t xcell,
	uint_fast8_t ycell,
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
			display_value_big(xcell, ycell + lowhalf, efp->freq, fullwidth, comma, 255, rj, efp->blinkpos + 1, efp->blinkstate, 1, lowhalf);	// отрисовываем верхнюю часть строки
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
			display_value_big(xcell, ycell + lowhalf, freq, fullwidth, comma, 255, rj, blinkpos, blinkstate, 1, lowhalf);	// отрисовываем верхнюю часть строки
		} while (lowhalf --);
	}
}

// Верстия отображения без точки между мегагерцами и сотнями килогерц (для текстовых дисплееев)
// FREQ B
static void display_freqchr_b(
	uint_fast8_t xcell,
	uint_fast8_t ycell,
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
			display_value_big(xcell, ycell + lowhalf, efp->freq, fullwidth, comma, 255, rj, efp->blinkpos + 1, efp->blinkstate, 1, lowhalf);	// отрисовываем верхнюю часть строки
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
			display_value_big(xcell, ycell + lowhalf, freq, fullwidth, comma, 255, 1, blinkpos, blinkstate, 1, lowhalf);	// отрисовываем верхнюю часть строки
		} while (lowhalf --);
	}
}

static void display2_freqX_b(
	uint_fast8_t xcell,
	uint_fast8_t ycell,
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
		display_value_small(xcell, ycell + lowhalf, freq, fullwidth, comma, comma + 3, rj, lowhalf);
	} while (lowhalf --);
}

// отладочная функция измерителя опорной частоты
static void display_freqmeter10(
	uint_fast8_t xcell,
	uint_fast8_t ycell,
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
	display_at(xcell, ycell, buf2);
#endif /* WITHFQMETER */
}

// отображение текста (из FLASH) с атрибутами по состоянию
static void
NOINLINEAT
display2_text_P(
	uint_fast8_t xcell,
	uint_fast8_t ycell,
	const FLASHMEM char * const * labels,	// массив указателей на текст
	const COLORPAIR_T * colors,			// массив цветов
	uint_fast8_t state
	)
{
	#if LCDMODE_COLORED
	#else /* LCDMODE_COLORED */
	#endif /* LCDMODE_COLORED */
#if WITHALTERNATIVELAYOUT
	layout_label1_medium(xcell, ycell, labels [state], strlen_P(labels [state]), 5, COLORPIP_BLACK, colors_2state_alt [state]);
#else
	colmain_setcolors(colors [state].fg, colors [state].bg);
	display_at_P(xcell, ycell, labels [state]);
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
	layout_label1_medium(x, y, labels [state], strlen_P(labels [state]), 5, COLORPIP_BLACK, colors_2state_alt [state]);
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
	layout_label1_medium(x, y, labels [state], 2, 5, state ? COLORPIP_WHITE : COLORPIP_BLACK, state ? COLORPIP_RED : COLORPIP_GRAY);
#endif /* WITHTX */
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

	/* формирование мигающей надписи REC */
	display2_text_P(x, y, labels, habradio_get_blinkphase() ? colors_2state_rec : colors_2state, state);

#endif /* WITHUSEAUDIOREC */
}


void display2_swrsts(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{

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
	layout_label1_medium(x, y, label, strlen_P(label), 7, COLORPIP_BLACK, colors_2state_alt [state]);
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
	local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%5u"), (unsigned) freq);
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
	layout_label1_medium(x, y, labels [0], strlen_P(labels [0]), 5, COLORPIP_BLACK, colors_2state_alt [state]);
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

	static const FLASHMEM char text0 [] = "   ";
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

	layout_label1_medium(x, y, fastv ? text2 : text1, 4, chars_W2, COLORPIP_BLACK, colors_2state_alt [lockv || fastv]);
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
static void display2_rxbwval4(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
	const char * const labels [1] = { hamradio_get_rxbw_value4(), };
	ASSERT(strlen(labels [0]) == 4);
	display2_text(x, y, labels, colors_1statevoltage, 0);
}

// RX path bandwidth
static void display2_rxbwval6alt(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
	const char * const labels [1] = { hamradio_get_rxbw_value4(), };
	enum { state = 0 };
	ASSERT(strlen(labels [0]) == 4);
	layout_label1_medium(x, y, labels [state], 4, 6, state ? COLORPIP_WHITE : COLORPIP_BLACK, state ? COLORPIP_RED : COLORPIP_GRAY);
}


// RX path bandwidth name
static void display2_rxbw3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	dctx_t * pctx
	)
{
	const char FLASHMEM * const labels [1] = { hamradio_get_rxbw_label3_P(), };
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
		layout_label1_medium(x, y, str, 3, chars_W2, COLORPIP_WHITE, OVFCOLOR);
	}
	else if (boad_mike_adcoverflow() != 0)
	{
		const char str [] = "MIC";
		layout_label1_medium(x, y, str, 3, chars_W2, COLORPIP_WHITE, OVFCOLOR);
	}
	else
	{
		const char * str = hamradio_get_pre_value_P();
		layout_label1_medium(x, y, str, strlen_P(str), chars_W2, COLORPIP_BLACK, colors_2state_alt [1]);
	}
}

// display antenna
static void display2_ant5(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT2
	const char FLASHMEM * const labels [1] = { hamradio_get_ant5_value_P(), };
	ASSERT(strlen(labels [0]) == 5);
	display2_text_P(x, y, labels, colors_1state, 0);
#elif WITHANTSELECT
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
#if WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT2
	const char FLASHMEM * const labels [1] = { hamradio_get_ant5_value_P(), };
	layout_label1_medium(x, y, labels [0], strlen_P(labels [0]), 7, COLORPIP_BLACK, colors_2state_alt [1]);
#elif WITHANTSELECT
	const char FLASHMEM * const labels [1] = { hamradio_get_ant5_value_P(), };
	layout_label1_medium(x, y, labels [0], strlen_P(labels [0]), 7, COLORPIP_BLACK, colors_2state_alt [1]);
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
	layout_label1_medium(x, y, labels [0], strlen_P(labels [0]), 5, COLORPIP_BLACK, colors_2state_alt [1]);
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
	PACKEDCOLORPIP_T * const fr = colmain_fb_draw();
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
	colmain_setcolors(colors_1statevoltage [0].fg, colors_1statevoltage [0].bg);
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

	colmain_setcolors(colors_1statevoltage [0].fg, colors_1statevoltage [0].bg);
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
	int_fast16_t tempv = hamradio_get_PAtemp_value();	// Градусы в десятых долях

	// 50+ - красный
	// 30+ - желтый
	// ниже 30 зеленый
	if (tempv > 999)
		tempv = 999; //- tempv;

	if (tempv < 0)
	{
		tempv = 999; //- tempv;
		colmain_setcolors(COLORPIP_WHITE, display_getbgcolor());
	}
	else if (tempv >= 500)
		colmain_setcolors(COLORPIP_RED, display_getbgcolor());
	else if (tempv >= 300)
		colmain_setcolors(COLORPIP_YELLOW, display_getbgcolor());
	else
		colmain_setcolors(COLORPIP_GREEN, display_getbgcolor());

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
	int_fast16_t tempv = hamradio_get_PAtemp_value();	// Градусы в десятых долях

	// 50+ - красный
	// 30+ - желтый
	// ниже 30 зеленый
	if (tempv > 999)
		tempv = 999; //- tempv;

	if (tempv < 0)
	{
		tempv = 999; //- tempv;
		colmain_setcolors(COLORPIP_WHITE, display_getbgcolor());
	}
	else if (tempv >= 500)
		colmain_setcolors(COLORPIP_RED, display_getbgcolor());
	else if (tempv >= 300)
		colmain_setcolors(COLORPIP_YELLOW, display_getbgcolor());
	else
		colmain_setcolors(COLORPIP_GREEN, display_getbgcolor());

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

		colmain_setcolors(colors_1statevoltage [0].fg, colors_1statevoltage [0].bg);
		uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
		do
		{
			display_value_small(x + CHARS2GRID(0), y + lowhalf, drain, 3 | WMINUSFLAG, 1, 255, 1, lowhalf);
		} while (lowhalf --);
		display_at_P(x + CHARS2GRID(5), y, PSTR("A"));

	#else /* WITHCURRLEVEL_ACS712_30A */
		// dd.d - 5 places (without "A")
		int_fast16_t drain = hamradio_get_pacurrent_value();	// Ток в десятках милиампер (до 2.55 ампера), может быть отрицательным

		colmain_setcolors(colors_1statevoltage [0].fg, colors_1statevoltage [0].bg);
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

		colmain_setcolors(colors_1statevoltage [0].fg, colors_1statevoltage [0].bg);
		uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
		do
		{
			display_value_small(x + CHARS2GRID(0), y + lowhalf, drain, 3 | WMINUSFLAG, 1, 255, 1, lowhalf);
		} while (lowhalf --);
		//display_at_P(x + CHARS2GRID(5), y, PSTR("A"));

	#else /* WITHCURRLEVEL_ACS712_30A */
		// dd.d - 5 places (without "A")
		int_fast16_t drain = hamradio_get_pacurrent_value();	// Ток в десятках милиампер (до 2.55 ампера), может быть отрицательным

		colmain_setcolors(colors_1statevoltage [0].fg, colors_1statevoltage [0].bg);
		uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
		do
		{
			display_value_small(x + CHARS2GRID(0), y + lowhalf, drain, 3 | WMINUSFLAG, 2, 255, 0, lowhalf);
		} while (lowhalf --);
		//display_at_P(x + CHARS2GRID(5), y, PSTR("A"));

	#endif /* WITHCURRLEVEL_ACS712_30A */
#endif /* WITHCURRLEVEL || WITHCURRLEVEL2 */
}

// Class-A power amplifier
static void display2_classa7(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
#if WITHPACLASSA
	const uint_fast8_t active = habradio_get_classa();
	#if LCDMODE_COLORED
		static const char classa_text [] = "CLASS A";
		static const char classa_null [] = "       ";
		display_2states_P(x, y, active, classa_text, classa_text);
	#else /* LCDMODE_COLORED */
		display_at_P(x, y, active ? classa_text : classa_null);
	#endif /* LCDMODE_COLORED */
#endif /* WITHPACLASSA */
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
	display2_text(x, y, labels, colors_1statevoltage, 0);
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
	display2_text(x, y, labels, colors_1statevoltage, 0);
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
	display2_text(x, y, labels, colors_1statevoltage, 0);

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
	uint_fast8_t hour, minute, seconds;
	char buf2 [9];

	board_rtc_cached_gettime(& hour, & minute, & seconds);
	local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%02d:%02d:%02d"),
			(int) hour, (int) minute, (int) seconds
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
	uint_fast8_t hour, minute, seconds;
	char buf2 [6];

	board_rtc_cached_gettime(& hour, & minute, & seconds);
	local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%02d%c%02d"),
		(int) hour,
		((seconds & 1) ? ' ' : ':'),	// мигающее двоеточие с периодом две секунды
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
	uint_fast8_t hour, minute, seconds;
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

	board_rtc_cached_getdatetime(& year, & month, & day, & hour, & minute, & seconds);

	local_snprintf_P(buf2, ARRAY_SIZE(buf2), PSTR("%s-%02d %02d%c%02d"),
		months [month - 1],
		(int) day,
		(int) hour,
		((seconds & 1) ? ' ' : ':'),	// мигающее двоеточие с периодом две секунды
		(int) minute
		);

	const char * const labels [1] = { buf2, };
	ASSERT(strlen(buf2) == 12);
	display2_text(x, y, labels, colors_1stateBlue, 0);
#endif /* defined (RTC1_TYPE) */
}

static void display2_dummy(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{

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

#if LINUX_SUBSYSTEM
	enum
	{
		REDRM_ALL,
		REDRM_FREQ = REDRM_ALL,
		REDRM_KEYB = REDRM_ALL,
		REDRM_MODE = REDRM_ALL,
		REDRM_INIS,
		REDRM_count
	};
#else
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
		REDRM_KEYB,		// обработчик клавиатуры для указанного display layout
		REDRM_count
	};
#endif /* LINUX_SUBSYSTEM */

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


/* Описания расположения элементов на дисплеях */

#include "dstyles/dstyles.h"

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
#if WITHBARS && WITHTX

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

#endif /* WITHBARS && WITHTX */
}

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

static const uint_fast8_t BDCO_WFLRX = BDCV_SPMRX;	// смещение водопада по вертикали в ячейках от начала общего поля
#if WITHTOUCHGUI
static const uint_fast8_t BDCV_WFLRX = BDCV_ALLRX - BDCV_SPMRX - 10;    // вертикальный размер водопада в ячейках - GUI version
#else /* WITHTOUCHGUI */
static const uint_fast8_t BDCV_WFLRX = BDCV_ALLRX - BDCV_SPMRX;	// вертикальный размер водопада в ячейках
#endif /* WITHTOUCHGUI */
static const uint_fast16_t WFDY = GRID2Y(BDCV_WFLRX);				// размер по вертикали в пикселях части отведенной водопаду
static const uint_fast16_t WFY0 = GRID2Y(BDCO_WFLRX);				// смещение по вертикали в пикселях части отведенной водопаду
static const uint_fast16_t SPY0 = GRID2Y(BDCO_SPMRX);				// смещение по вертикали в пикселях части отведенной спектру

#define ALLDX 	(GRID2X(CHARS2GRID(BDTH_ALLRX)))
#define ALLDY 	(GRID2Y(BDCV_ALLRX))
#define SPDY 	(GRID2Y(BDCV_SPMRX))				// размер по вертикали в пикселях части отведенной спектру

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
	FFTOVERLAP = 1,
	NOVERLAP = 1 << WITHFFTOVERLAPPOW2,		// Количество перекрывающися буферов FFT спектра
	BOARD_FFTZOOM_MAX = (1 << BOARD_FFTZOOM_POW2MAX),

	LARGEFFT = WITHFFTSIZEWIDE / FFTOVERLAP * BOARD_FFTZOOM_MAX,	// размер буфера для децимации
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
	FLOAT_t raw_buf [WITHFFTSIZEAF * AFSP_DECIMATION];		// Для последующей децимации /2
	FLOAT_t fft_buf [WITHFFTSIZEAF * 2];		// комплексные числа
	uint_fast8_t is_ready;
	uint_fast16_t x;
	uint_fast16_t y;
	uint_fast16_t w;
	uint_fast16_t h;
	FLOAT_t max_val;
	FLOAT_t val_array [DIM_X];
	ARM_MORPH(arm_rfft_fast_instance) rfft_instance;
} afsp_t;

#endif /* WITHAFSPECTRE */


#if defined (COLORPIP_SHADED)

	/* быстрое отображение водопада (но требует больше памяти) */
	enum { WFROWS = ALLDY };
	enum { PALETTESIZE = COLORPIP_BASE };
	static uint_fast16_t wfrow;		// строка, в которую последней занесены данные

#elif WITHFASTWATERFLOW && WITHGRADIENT_FIXED

	/* быстрое отображение водопада (но требует больше памяти) */
	enum { WFROWS = ALLDY };	// буфер больше чем WFDY - для возможности динамисеского изменения высоты отображаемого водопада
	enum { PALETTESIZE = ARRAY_SIZE(pancolor) };
	static PACKEDCOLORPIP_T wfpalette [PALETTESIZE];
	static uint_fast16_t wfrow;		// строка, в которую последней занесены данные

#elif WITHFASTWATERFLOW

	/* быстрое отображение водопада (но требует больше памяти) */
	enum { WFROWS = ALLDY };	// буфер больше чем WFDY - для возможности динамисеского изменения высоты отображаемого водопада
	enum { PALETTESIZE = 256 };

	static PACKEDCOLORPIP_T wfpalette [PALETTESIZE];
	static uint_fast16_t wfrow;		// строка, в которую последней занесены данные

#elif (! LCDMODE_S1D13781_NHWACCEL && LCDMODE_S1D13781)

	enum { WFROWS = 1 };
	enum { wfrow = 0 };				// строка, в которую последней занесены данные

	enum { PALETTESIZE = 256 };
	static RAMBIGDTCM PACKEDCOLOR565_T wfpalette [PALETTESIZE];

#else

	enum { WFROWS = ALLDY };
	static uint_fast16_t wfrow;		// строка, в которую последней занесены данные

	enum { PALETTESIZE = 256 };
	static RAMBIGDTCM PACKEDCOLOR565_T wfpalette [PALETTESIZE];

#endif


#if WITHVIEW_3DSS
enum {
#if CPUSTYLE_XC7Z || CPUSTYLE_XCZU || CPUSTYLE_STM32MP1 || CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_T507
	MAX_3DSS_STEP = 70,
#else
	MAX_3DSS_STEP = 42,
#endif /* #if CPUSTYLE_XC7Z || CPUSTYLE_XC7Z || CPUSTYLE_STM32MP1 || CPUSTYLE_T113 */
	Y_STEP = 2,
	DEPTH_ATTENUATION = 2,
	MAX_DELAY_3DSS = 1,
	HALF_ALLDX = ALLDX / 2,
	SPY_3DSS = SPDY,
	SPY_3DSS_H = SPY_3DSS / 4
};

#endif /* WITHVIEW_3DSS */

typedef int16_t WFL3DSS_T;

struct ustates
{
#if defined(ARM_MATH_NEON)
	FLOAT_t iir_state [ZOOMFFT_DECIM_STAGES_IIR * 8];
#else /* defined(ARM_MATH_NEON) */
	FLOAT_t iir_state [ZOOMFFT_DECIM_STAGES_IIR * 4];
#endif /* defined(ARM_MATH_NEON) */
	FLOAT_t fir_state [ZOOMFFT_DECIM_STAGES_FIR + LARGEFFT - 1];

	FLOAT_t cmplx_sig [NORMALFFT * 2];
	FLOAT_t ifspec_wndfn [NORMALFFT];

	FLOAT_t spavgarray [ALLDX];	// массив входных данных для отображения (через фильтры).
	FLOAT_t Yold_wtf [ALLDX];
	FLOAT_t Yold_spe [ALLDX];
	union
	{
		PACKEDCOLORPIP_T wfjarray [GXSIZE(ALLDX, WFROWS)];	// массив "водопада"
#if WITHVIEW_3DSS
		WFL3DSS_T wfj3dss [MAX_3DSS_STEP] [ALLDX];
#endif /* WITHVIEW_3DSS */
	} u;
	PACKEDCOLORPIP_T color_scale [SPDY];	/* массив значений для раскраски спектра */

#if WITHAFSPECTRE
	FLOAT_t afspec_wndfn [WITHFFTSIZEAF];
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

// Получить цвет запослнен6ия водопада при перестройке
static COLORPIP_T display2_bgcolorwfl(void)
{
	return gvars.color_scale [0];
}


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

// RT context function
/* вызывается при запрещённых глобальных прерываниях */
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

#include "dsp/window_functions.h"

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

	VERIFY(ARM_MATH_SUCCESS == ARM_MORPH(arm_rfft_fast_init)(& gvars.afsp.rfft_instance, WITHFFTSIZEAF));
	ARM_MORPH(arm_nuttall4b)(gvars.afspec_wndfn, WITHFFTSIZEAF);	/* оконная функция для показа звукового спектра */

#if 0 && CTLSTYLE_V3D
	subscribefloat(& afdemodoutfloat, & afspectreregister, NULL, afsp_save_sample);
#else
	subscribefloat(& speexoutfloat, & afspectreregister, NULL, afsp_save_sample);	// выход speex и фильтра
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

		ARM_MORPH(arm_mult)(gvars.afsp.raw_buf, gvars.afspec_wndfn, gvars.afsp.raw_buf, WITHFFTSIZEAF); // apply window function
		ARM_MORPH(arm_rfft_fast)(& gvars.afsp.rfft_instance, gvars.afsp.raw_buf, gvars.afsp.fft_buf, 0); // 0-прямое, 1-обратное
		gvars.afsp.is_ready = 0;	// буфер больше не нужен... но он заполняется так же в user mode
		ARM_MORPH(arm_cmplx_mag)(gvars.afsp.fft_buf, gvars.afsp.fft_buf, WITHFFTSIZEAF);

		ASSERT(gvars.afsp.w <= ARRAY_SIZE(gvars.afsp.val_array));
		for (unsigned x = 0; x < gvars.afsp.w; x ++)
		{
			const uint_fast16_t fftpos = raster2fftsingle(x, gvars.afsp.w, leftfftpos, rightfftpos);
			ASSERT(fftpos < ARRAY_SIZE(gvars.afsp.fft_buf));
			// filterig
			gvars.afsp.val_array [x] = gvars.afsp.val_array [x] * (FLOAT_t) 0.6 + (FLOAT_t) 0.4 * gvars.afsp.fft_buf [fftpos];
		}
		ARM_MORPH(arm_max_no_idx)(gvars.afsp.val_array, gvars.afsp.w, & gvars.afsp.max_val);	// поиск в отображаемой части
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
				PACKEDCOLORPIP_T * const fr = colmain_fb_draw();

				ASSERT(gvars.afsp.w <= ARRAY_SIZE(gvars.afsp.val_array));
				for (unsigned x = 0; x < gvars.afsp.w; x ++)
				{
					//const uint_fast16_t y_norm = normalize(gvars.afsp.val_array [x], 0, gvars.afsp.max_val, gvars.afsp.h - 2) + 1;
					const uint_fast16_t y_norm = normalize(gvars.afsp.val_array [x] * 4096, 0, gvars.afsp.max_val * 4096, gvars.afsp.h - 2) + 1;
					ASSERT(y_norm <= gvars.afsp.h);
					ASSERT(gvars.afsp.y >= y_norm);
					if (gvars.afsp.y >= y_norm)
					{
						colpip_set_vline(fr, DIM_X, DIM_Y,
								gvars.afsp.x + x, gvars.afsp.y - y_norm, y_norm,
								AFSPECTRE_COLOR);
					}
				}
#if WITHAA
				display_do_AA(fr, DIM_X, DIM_Y, GRID2X(xgrid), GRID2Y(ygrid), SM_BG_W, SM_BG_H);
#endif /* WITHAA */
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
void display2_set_filter_wfl(uint_fast8_t v)
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
	const FLOAT_t * pFIRCoeffs;
	const FLOAT_t * pIIRCoeffs;
};

static const struct zoom_param zoom_params [] =
{
	// x2 zoom lowpass
	{
		.zoom = 2,
		.numTaps = ZOOMFFT_DECIM_STAGES_FIR,
		.pFIRCoeffs = (const FLOAT_t [ZOOMFFT_DECIM_STAGES_FIR]){-0.05698952454792, 0.5574889164132, 0.5574889164132, -0.05698952454792},
		.pIIRCoeffs = (const FLOAT_t [ZOOMFFT_DECIM_STAGES_IIR * 5]){0.8384843639921, 0, 0, 0, 0, 1, 0.5130084793341, 1, 0.1784114407685, -0.6967733943344, 0.8744089756375, 0, 0, 0, 0, 1, 1.046379755684, 1, 0.3420998857106, -0.3982809814397, 1.83222755502, 0, 0, 0, 0, 1, 1.831496024383, 1, 0.5072844084012, -0.1179052535088, 0.01953722920982, 0, 0, 0, 0, 1, 0.3029841730578, 1, 0.09694668293684, -0.9095549467394, 1, 0, 0, 0, 0},
	},
	// x4 zoom lowpass
	{
		.zoom = 4,
		.numTaps = ZOOMFFT_DECIM_STAGES_FIR,
		.pFIRCoeffs = (const FLOAT_t [ZOOMFFT_DECIM_STAGES_FIR]){-0.05698952454792, 0.5574889164132, 0.5574889164132, -0.05698952454792},
		.pIIRCoeffs = (const FLOAT_t [ZOOMFFT_DECIM_STAGES_IIR * 5]){0.6737499659657, 0, 0, 0, 0, 1, -1.102065194995, 1, 1.353694541279, -0.7896377861467, 0.53324811147, 0, 0, 0, 0, 1, -0.5853766477218, 1, 1.289175897987, -0.5882714065646, 0.6143152247695, 0, 0, 0, 0, 1, 1.182778527244, 1, 1.236309127239, -0.4063767082903, 0.01708381580242, 0, 0, 0, 0, 1, -1.245590418009, 1, 1.418191929315, -0.9374008035325, 1, 0, 0, 0, 0},
	},
	// x8 zoom lowpass
	{
		.zoom = 8,
		.numTaps = ZOOMFFT_DECIM_STAGES_FIR,
		.pFIRCoeffs = (const FLOAT_t [ZOOMFFT_DECIM_STAGES_FIR]){-0.05698952454792, 0.5574889164132, 0.5574889164132, -0.05698952454792},
		.pIIRCoeffs = (const FLOAT_t [ZOOMFFT_DECIM_STAGES_IIR * 5]){0.6469981129046, 0, 0, 0, 0, 1, -1.750671284068, 1, 1.766710155669, -0.8829517893283, 0.4645312725883, 0, 0, 0, 0, 1, -1.553480572725, 1, 1.681513354365, -0.7637556184482, 0.2925692260954, 0, 0, 0, 0, 1, -0.1114766808264, 1, 1.601891439147, -0.6499504503566, 0.01652325734055, 0, 0, 0, 0, 1, -1.797298202754, 1, 1.831125104215, -0.9660534813317, 1, 0, 0, 0, 0},
	},
	// x16 zoom lowpass
	{
		.zoom = 16,
		.numTaps = ZOOMFFT_DECIM_STAGES_FIR,
		.pFIRCoeffs = (const FLOAT_t [ZOOMFFT_DECIM_STAGES_FIR]){-0.05698952454792, 0.5574889164132, 0.5574889164132, -0.05698952454792},
		.pIIRCoeffs = (const FLOAT_t [ZOOMFFT_DECIM_STAGES_IIR * 5]){0.6500044972642, 0, 0, 0, 0, 1, -1.935616780918, 1, 1.908632776595, -0.9387888949475, 0.4599444315799, 0, 0, 0, 0, 1, -1.880017827578, 1, 1.851418291083, -0.8732990221737, 0.2087317940803, 0, 0, 0, 0, 1, -1.278402634611, 1, 1.794539349192, -0.80764043772, 0.01645106748385, 0, 0, 0, 0, 1, -1.948135342532, 1, 1.948194658987, -0.9825675157696, 1, 0, 0, 0, 0},
	},
};

// Сэмплы для децимации
typedef struct fftbuff_tag
{
	LIST_ENTRY item;
	FLOAT_t largebuffI [LARGEFFT];
	FLOAT_t largebuffQ [LARGEFFT];
} fftbuff_t;

static LIST_ENTRY fftbuffree;
static LIST_ENTRY fftbufready;
static IRQLSPINLOCK_t fftlock;

// realtime-mode function
uint_fast8_t allocate_fftbuffer(fftbuff_t * * dest)
{
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& fftlock, & oldIrql);
	if (! IsListEmpty(& fftbuffree))
	{
		const PLIST_ENTRY t = RemoveTailList(& fftbuffree);
		IRQLSPIN_UNLOCK(& fftlock, oldIrql);
		fftbuff_t * const p = CONTAINING_RECORD(t, fftbuff_t, item);
		* dest = p;
		return 1;
	}
	/* Начинаем отбрасывать самые старые в очереди готовых. */
	if (! IsListEmpty(& fftbufready))
	{
		const PLIST_ENTRY t = RemoveTailList(& fftbufready);
		IRQLSPIN_UNLOCK(& fftlock, oldIrql);
		fftbuff_t * const p = CONTAINING_RECORD(t, fftbuff_t, item);
		* dest = p;
		return 1;
	}
	IRQLSPIN_UNLOCK(& fftlock, oldIrql);
	return 0;
}

// realtime-mode function
void saveready_fftbuffer(fftbuff_t * p)
{
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& fftlock, & oldIrql);
	while (! IsListEmpty(& fftbufready))
	{
		const PLIST_ENTRY t = RemoveTailList(& fftbufready);
		InsertHeadList(& fftbuffree, t);
	}
	InsertHeadList(& fftbufready, & p->item);
	IRQLSPIN_UNLOCK(& fftlock, oldIrql);
}

// user-mode function
void release_fftbuffer(fftbuff_t * p)
{
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& fftlock, & oldIrql);
	InsertHeadList(& fftbuffree, & p->item);
	IRQLSPIN_UNLOCK(& fftlock, oldIrql);
}

// user-mode function
uint_fast8_t  getfilled_fftbuffer(fftbuff_t * * dest)
{
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& fftlock, & oldIrql);
	if (! IsListEmpty(& fftbufready))
	{
		const PLIST_ENTRY t = RemoveTailList(& fftbufready);
		IRQLSPIN_UNLOCK(& fftlock, oldIrql);
		fftbuff_t * const p = CONTAINING_RECORD(t, fftbuff_t, item);
		* dest = p;
		return 1;
	}
	IRQLSPIN_UNLOCK(& fftlock, oldIrql);
	return 0;
}

static fftbuff_t * pfill [NOVERLAP];
static unsigned filleds [NOVERLAP]; // 0..LARGEFFT-1

// сохранение сэмпла для отображения спектра
// rt-context function
static void
saveIQRTSxx(void * ctx, int_fast32_t iv, int_fast32_t qv)
{
	const FLOAT_t ivf = adpt_input(& ifspectrumin96, qv);	// нормализованное к -1..+1
	const FLOAT_t qvf = adpt_input(& ifspectrumin96, iv);	// нормализованное к -1..+1
	unsigned i;
	for (i = 0; i < NOVERLAP; ++ i)
	{
		fftbuff_t * * ppf = & pfill [i];

		if (* ppf == NULL)
		{
			if (allocate_fftbuffer(ppf) == 0)
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
			saveready_fftbuffer(pf);
			* ppf = NULL;
		}
	}
}

// вызывается при запрещённых прерываниях.
void fftbuffer_initialize(void)
{
	static RAMBIG fftbuff_t fftbuffersarray [NOVERLAP * 3 + 1 * 0];
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
		VERIFY(allocate_fftbuffer(ppf) != 0);
		/* установка начальной позиции для заполнения со сдвигом. */
		const unsigned filled = (i * LARGEFFT / NOVERLAP);
		filleds [i] = filled;
		ARM_MORPH(arm_fill)(0, (* ppf)->largebuffI, filled);
		ARM_MORPH(arm_fill)(0, (* ppf)->largebuffQ, filled);
	}
	IRQLSPINLOCK_INITIALIZE(& fftlock, IRQL_REALTIME);
}

#if (__ARM_FP & 0x08) || __riscv_d


#if defined(ARM_MATH_NEON)
/**
  @brief         Compute new coefficient arrays for use in vectorized filter (Neon only).
  @param[in]     numStages         number of 2nd order stages in the filter.
  @param[in]     pCoeffs           points to the original filter coefficients.
  @param[in]     pComputedCoeffs   points to the new computed coefficients for the vectorized Neon version.
  @return        none

  @par   Size of coefficient arrays:
            pCoeffs has size 5 * numStages

            pComputedCoeffs has size 8 * numStages

            pComputedCoeffs is the array to be used in arm_biquad_cascade_df2T_init_f32.

*/
void arm_biquad_cascade_df2T_compute_coefs_f64(
  uint8_t numStages,
  const float64_t * pCoeffs,
  float64_t * pComputedCoeffs)
{
   uint8_t cnt;
   float64_t b0[4],b1[4],b2[4],a1[4],a2[4];

   cnt = numStages >> 2;
   while(cnt > 0)
   {
      for(int i=0;i<4;i++)
      {
        b0[i] = pCoeffs[0];
        b1[i] = pCoeffs[1];
        b2[i] = pCoeffs[2];
        a1[i] = pCoeffs[3];
        a2[i] = pCoeffs[4];
        pCoeffs += 5;
      }

      /* Vec 1 */
      *pComputedCoeffs++ = 0;
      *pComputedCoeffs++ = b0[1];
      *pComputedCoeffs++ = b0[2];
      *pComputedCoeffs++ = b0[3];

      /* Vec 2 */
      *pComputedCoeffs++ = 0;
      *pComputedCoeffs++ = 0;
      *pComputedCoeffs++ = b0[1] * b0[2];
      *pComputedCoeffs++ = b0[2] * b0[3];

      /* Vec 3 */
      *pComputedCoeffs++ = 0;
      *pComputedCoeffs++ = 0;
      *pComputedCoeffs++ = 0;
      *pComputedCoeffs++ = b0[1] * b0[2] * b0[3];

      /* Vec 4 */
      *pComputedCoeffs++ = b0[0];
      *pComputedCoeffs++ = b0[0] * b0[1];
      *pComputedCoeffs++ = b0[0] * b0[1] * b0[2];
      *pComputedCoeffs++ = b0[0] * b0[1] * b0[2] * b0[3];

      /* Vec 5 */
      *pComputedCoeffs++ = b1[0];
      *pComputedCoeffs++ = b1[1];
      *pComputedCoeffs++ = b1[2];
      *pComputedCoeffs++ = b1[3];

      /* Vec 6 */
      *pComputedCoeffs++ = b2[0];
      *pComputedCoeffs++ = b2[1];
      *pComputedCoeffs++ = b2[2];
      *pComputedCoeffs++ = b2[3];

      /* Vec 7 */
      *pComputedCoeffs++ = a1[0];
      *pComputedCoeffs++ = a1[1];
      *pComputedCoeffs++ = a1[2];
      *pComputedCoeffs++ = a1[3];

      /* Vec 8 */
      *pComputedCoeffs++ = a2[0];
      *pComputedCoeffs++ = a2[1];
      *pComputedCoeffs++ = a2[2];
      *pComputedCoeffs++ = a2[3];

      cnt--;
   }

   cnt = numStages & 0x3;
   while(cnt > 0)
   {
      *pComputedCoeffs++ = *pCoeffs++;
      *pComputedCoeffs++ = *pCoeffs++;
      *pComputedCoeffs++ = *pCoeffs++;
      *pComputedCoeffs++ = *pCoeffs++;
      *pComputedCoeffs++ = *pCoeffs++;
      cnt--;
   }

}
#endif

void arm_cmplx_mult_real_f64(
  const float64_t * pSrcCmplx,
  const float64_t * pSrcReal,
  	  	  float64_t * pCmplxDst,
        uint32_t numSamples)
{
        uint32_t blkCnt;                               /* Loop counter */
        float64_t in;                                  /* Temporary variable */

#if 0//defined(ARM_MATH_NEON) && !defined(ARM_MATH_AUTOVECTORIZE)
    float32x4_t r;
    float32x4x2_t ab,outCplx;

    /* Compute 4 outputs at a time */
    blkCnt = numSamples >> 2U;

    while (blkCnt > 0U)
    {
        ab = vld2q_f32(pSrcCmplx);  // load & separate real/imag pSrcA (de-interleave 2)
        r = vld1q_f32(pSrcReal);  // load & separate real/imag pSrcB

	/* Increment pointers */
        pSrcCmplx += 8;
        pSrcReal += 4;

        outCplx.val[0] = vmulq_f32(ab.val[0], r);
        outCplx.val[1] = vmulq_f32(ab.val[1], r);

        vst2q_f32(pCmplxDst, outCplx);
        pCmplxDst += 8;

        blkCnt--;
    }

    /* Tail */
    blkCnt = numSamples & 3;
#else
#if defined (ARM_MATH_LOOPUNROLL) && !defined(ARM_MATH_AUTOVECTORIZE)

  /* Loop unrolling: Compute 4 outputs at a time */
  blkCnt = numSamples >> 2U;

  while (blkCnt > 0U)
  {
    /* C[2 * i    ] = A[2 * i    ] * B[i]. */
    /* C[2 * i + 1] = A[2 * i + 1] * B[i]. */

    in = *pSrcReal++;
    /* store result in destination buffer. */
    *pCmplxDst++ = *pSrcCmplx++ * in;
    *pCmplxDst++ = *pSrcCmplx++ * in;

    in = *pSrcReal++;
    *pCmplxDst++ = *pSrcCmplx++ * in;
    *pCmplxDst++ = *pSrcCmplx++ * in;

    in = *pSrcReal++;
    *pCmplxDst++ = *pSrcCmplx++ * in;
    *pCmplxDst++ = *pSrcCmplx++ * in;

    in = *pSrcReal++;
    *pCmplxDst++ = *pSrcCmplx++* in;
    *pCmplxDst++ = *pSrcCmplx++ * in;

    /* Decrement loop counter */
    blkCnt--;
  }

  /* Loop unrolling: Compute remaining outputs */
  blkCnt = numSamples % 0x4U;

#else

  /* Initialize blkCnt with number of samples */
  blkCnt = numSamples;

#endif /* #if defined (ARM_MATH_LOOPUNROLL) */
#endif /* #if defined(ARM_MATH_NEON) */

  while (blkCnt > 0U)
  {
    /* C[2 * i    ] = A[2 * i    ] * B[i]. */
    /* C[2 * i + 1] = A[2 * i + 1] * B[i]. */

    in = *pSrcReal++;
    /* store result in destination buffer. */
    *pCmplxDst++ = *pSrcCmplx++ * in;
    *pCmplxDst++ = *pSrcCmplx++ * in;

    /* Decrement loop counter */
    blkCnt--;
  }

}

/**
  @brief Instance structure for floating-point FIR decimator.
 */
typedef struct
  {
          uint8_t M;                  /**< decimation factor. */
          uint16_t numTaps;           /**< number of coefficients in the filter. */
    const float64_t *pCoeffs;         /**< points to the coefficient array. The array is of length numTaps.*/
    	float64_t *pState;          /**< points to the state variable array. The array is of length numTaps+blockSize-1. */
  } arm_fir_decimate_instance_f64;


  arm_status arm_fir_decimate_init_f64(
          arm_fir_decimate_instance_f64 * S,
          uint16_t numTaps,
          uint8_t M,
    const float64_t * pCoeffs,
	float64_t * pState,
          uint32_t blockSize)
  {
    arm_status status;

    /* The size of the input block must be a multiple of the decimation factor */
    if ((blockSize % M) != 0U)
    {
      /* Set status as ARM_MATH_LENGTH_ERROR */
      status = ARM_MATH_LENGTH_ERROR;
    }
    else
    {
      /* Assign filter taps */
      S->numTaps = numTaps;

      /* Assign coefficient pointer */
      S->pCoeffs = pCoeffs;

      /* Clear the state buffer. The size is always (blockSize + numTaps - 1) */
      memset(pState, 0, (numTaps + (blockSize - 1U)) * sizeof(float64_t));

      /* Assign state pointer */
      S->pState = pState;

      /* Assign Decimation Factor */
      S->M = M;

      status = ARM_MATH_SUCCESS;
    }

    return (status);

  }

  void arm_fir_decimate_f64(
    const arm_fir_decimate_instance_f64 * S,
    const float64_t * pSrc,
			float64_t * pDst,
          uint32_t blockSize)
  {
	  	  float64_t *pState = S->pState;                 /* State pointer */
    const float64_t *pCoeffs = S->pCoeffs;               /* Coefficient pointer */
		float64_t *pStateCur;                          /* Points to the current sample of the state */
		float64_t *px0;                                /* Temporary pointer for state buffer */
    const float64_t *pb;                                 /* Temporary pointer for coefficient buffer */
		float64_t x0, c0;                              /* Temporary variables to hold state and coefficient values */
		float64_t acc0;                                /* Accumulator */
          uint32_t numTaps = S->numTaps;                 /* Number of filter coefficients in the filter */
          uint32_t i, tapCnt, blkCnt, outBlockSize = blockSize / S->M;  /* Loop counters */

  #if defined (ARM_MATH_LOOPUNROLL)
          float64_t *px1, *px2, *px3;
          float64_t x1, x2, x3;
          float64_t acc1, acc2, acc3;
  #endif

    /* S->pState buffer contains previous frame (numTaps - 1) samples */
    /* pStateCur points to the location where the new input data should be written */
    pStateCur = S->pState + (numTaps - 1U);

  #if defined (ARM_MATH_LOOPUNROLL)

      /* Loop unrolling: Compute 4 samples at a time */
    blkCnt = outBlockSize >> 2U;

    /* Samples loop unrolled by 4 */
    while (blkCnt > 0U)
    {
      /* Copy 4 * decimation factor number of new input samples into the state buffer */
      i = S->M * 4;

      do
      {
        *pStateCur++ = *pSrc++;

      } while (--i);

      /* Set accumulators to zero */
      acc0 = 0.0f;
      acc1 = 0.0f;
      acc2 = 0.0f;
      acc3 = 0.0f;

      /* Initialize state pointer for all the samples */
      px0 = pState;
      px1 = pState + S->M;
      px2 = pState + 2 * S->M;
      px3 = pState + 3 * S->M;

      /* Initialize coeff pointer */
      pb = pCoeffs;

      /* Loop unrolling: Compute 4 taps at a time */
      tapCnt = numTaps >> 2U;

      while (tapCnt > 0U)
      {
        /* Read the b[numTaps-1] coefficient */
        c0 = *(pb++);

        /* Read x[n-numTaps-1] sample for acc0 */
        x0 = *(px0++);
        /* Read x[n-numTaps-1] sample for acc1 */
        x1 = *(px1++);
        /* Read x[n-numTaps-1] sample for acc2 */
        x2 = *(px2++);
        /* Read x[n-numTaps-1] sample for acc3 */
        x3 = *(px3++);

        /* Perform the multiply-accumulate */
        acc0 += x0 * c0;
        acc1 += x1 * c0;
        acc2 += x2 * c0;
        acc3 += x3 * c0;

        /* Read the b[numTaps-2] coefficient */
        c0 = *(pb++);

        /* Read x[n-numTaps-2] sample for acc0, acc1, acc2, acc3 */
        x0 = *(px0++);
        x1 = *(px1++);
        x2 = *(px2++);
        x3 = *(px3++);

        /* Perform the multiply-accumulate */
        acc0 += x0 * c0;
        acc1 += x1 * c0;
        acc2 += x2 * c0;
        acc3 += x3 * c0;

        /* Read the b[numTaps-3] coefficient */
        c0 = *(pb++);

        /* Read x[n-numTaps-3] sample acc0, acc1, acc2, acc3 */
        x0 = *(px0++);
        x1 = *(px1++);
        x2 = *(px2++);
        x3 = *(px3++);

        /* Perform the multiply-accumulate */
        acc0 += x0 * c0;
        acc1 += x1 * c0;
        acc2 += x2 * c0;
        acc3 += x3 * c0;

        /* Read the b[numTaps-4] coefficient */
        c0 = *(pb++);

        /* Read x[n-numTaps-4] sample acc0, acc1, acc2, acc3 */
        x0 = *(px0++);
        x1 = *(px1++);
        x2 = *(px2++);
        x3 = *(px3++);

        /* Perform the multiply-accumulate */
        acc0 += x0 * c0;
        acc1 += x1 * c0;
        acc2 += x2 * c0;
        acc3 += x3 * c0;

        /* Decrement loop counter */
        tapCnt--;
      }

      /* Loop unrolling: Compute remaining taps */
      tapCnt = numTaps % 0x4U;

      while (tapCnt > 0U)
      {
        /* Read coefficients */
        c0 = *(pb++);

        /* Fetch state variables for acc0, acc1, acc2, acc3 */
        x0 = *(px0++);
        x1 = *(px1++);
        x2 = *(px2++);
        x3 = *(px3++);

        /* Perform the multiply-accumulate */
        acc0 += x0 * c0;
        acc1 += x1 * c0;
        acc2 += x2 * c0;
        acc3 += x3 * c0;

        /* Decrement loop counter */
        tapCnt--;
      }

      /* Advance the state pointer by the decimation factor
       * to process the next group of decimation factor number samples */
      pState = pState + S->M * 4;

      /* The result is in the accumulator, store in the destination buffer. */
      *pDst++ = acc0;
      *pDst++ = acc1;
      *pDst++ = acc2;
      *pDst++ = acc3;

      /* Decrement loop counter */
      blkCnt--;
    }

    /* Loop unrolling: Compute remaining samples */
    blkCnt = outBlockSize % 0x4U;

  #else

    /* Initialize blkCnt with number of samples */
    blkCnt = outBlockSize;

  #endif /* #if defined (ARM_MATH_LOOPUNROLL) */

    while (blkCnt > 0U)
    {
      /* Copy decimation factor number of new input samples into the state buffer */
      i = S->M;

      do
      {
        *pStateCur++ = *pSrc++;

      } while (--i);

      /* Set accumulator to zero */
      acc0 = 0.0f;

      /* Initialize state pointer */
      px0 = pState;

      /* Initialize coeff pointer */
      pb = pCoeffs;

  #if defined (ARM_MATH_LOOPUNROLL)

      /* Loop unrolling: Compute 4 taps at a time */
      tapCnt = numTaps >> 2U;

      while (tapCnt > 0U)
      {
        /* Read the b[numTaps-1] coefficient */
        c0 = *pb++;

        /* Read x[n-numTaps-1] sample */
        x0 = *px0++;

        /* Perform the multiply-accumulate */
        acc0 += x0 * c0;

        /* Read the b[numTaps-2] coefficient */
        c0 = *pb++;

        /* Read x[n-numTaps-2] sample */
        x0 = *px0++;

        /* Perform the multiply-accumulate */
        acc0 += x0 * c0;

        /* Read the b[numTaps-3] coefficient */
        c0 = *pb++;

        /* Read x[n-numTaps-3] sample */
        x0 = *px0++;

        /* Perform the multiply-accumulate */
        acc0 += x0 * c0;

        /* Read the b[numTaps-4] coefficient */
        c0 = *pb++;

        /* Read x[n-numTaps-4] sample */
        x0 = *px0++;

        /* Perform the multiply-accumulate */
        acc0 += x0 * c0;

        /* Decrement loop counter */
        tapCnt--;
      }

      /* Loop unrolling: Compute remaining taps */
      tapCnt = numTaps % 0x4U;

  #else

      /* Initialize tapCnt with number of taps */
      tapCnt = numTaps;

  #endif /* #if defined (ARM_MATH_LOOPUNROLL) */

      while (tapCnt > 0U)
      {
        /* Read coefficients */
        c0 = *pb++;

        /* Fetch 1 state variable */
        x0 = *px0++;

        /* Perform the multiply-accumulate */
        acc0 += x0 * c0;

        /* Decrement loop counter */
        tapCnt--;
      }

      /* Advance the state pointer by the decimation factor
       * to process the next group of decimation factor number samples */
      pState = pState + S->M;

      /* The result is in the accumulator, store in the destination buffer. */
      *pDst++ = acc0;

      /* Decrement loop counter */
      blkCnt--;
    }

    /* Processing is complete.
       Now copy the last numTaps - 1 samples to the satrt of the state buffer.
       This prepares the state buffer for the next function call. */

    /* Points to the start of the state buffer */
    pStateCur = S->pState;

  #if defined (ARM_MATH_LOOPUNROLL)

    /* Loop unrolling: Compute 4 taps at a time */
    tapCnt = (numTaps - 1U) >> 2U;

    /* Copy data */
    while (tapCnt > 0U)
    {
      *pStateCur++ = *pState++;
      *pStateCur++ = *pState++;
      *pStateCur++ = *pState++;
      *pStateCur++ = *pState++;

      /* Decrement loop counter */
      tapCnt--;
    }

    /* Loop unrolling: Compute remaining taps */
    tapCnt = (numTaps - 1U) % 0x04U;

  #else

    /* Initialize tapCnt with number of taps */
    tapCnt = (numTaps - 1U);

  #endif /* #if defined (ARM_MATH_LOOPUNROLL) */

    /* Copy data */
    while (tapCnt > 0U)
    {
      *pStateCur++ = *pState++;

      /* Decrement loop counter */
      tapCnt--;
    }

  }

#endif

static void fftzoom_filer_decimate_ifspectrum(
	const struct zoom_param * const prm,
	FLOAT_t * buffer,
	unsigned usedSizeChk
	)
{
	union configs
	{
		ARM_MORPH(arm_biquad_cascade_df2T_instance) iir_config;
		ARM_MORPH(arm_fir_decimate_instance) fir_config;
	} c;
	const unsigned usedSize = NORMALFFT / FFTOVERLAP * prm->zoom;
	ASSERT(usedSize == usedSizeChk);

	// Biquad LPF фильтр
#if defined (ARM_MATH_NEON)
	FLOAT_t IIRCoeffs_NEON [ZOOMFFT_DECIM_STAGES_IIR * 8];

	// Initialize floating-point Biquad cascade filter.
	ARM_MORPH(arm_biquad_cascade_df2T_compute_coefs)(ZOOMFFT_DECIM_STAGES_IIR, prm->pIIRCoeffs, IIRCoeffs_NEON);
	ARM_MORPH(arm_biquad_cascade_df2T_init)(& c.iir_config, ZOOMFFT_DECIM_STAGES_IIR, IIRCoeffs_NEON, gvars.iir_state);

#else /* defined (ARM_MATH_NEON) */
	// Initialize floating-point Biquad cascade filter.
	ARM_MORPH(arm_biquad_cascade_df2T_init)(& c.iir_config, ZOOMFFT_DECIM_STAGES_IIR, prm->pIIRCoeffs, gvars.iir_state);
	ARM_MORPH(arm_biquad_cascade_df2T)(& c.iir_config, buffer, buffer, usedSize);

#endif /* defined (ARM_MATH_NEON) */

	// Дециматор
	VERIFY(ARM_MATH_SUCCESS == ARM_MORPH(arm_fir_decimate_init)(& c.fir_config,
						prm->numTaps,
						prm->zoom,          // Decimation factor
						prm->pFIRCoeffs,
						gvars.fir_state,            // Filter state variables
						usedSize));
	ARM_MORPH(arm_fir_decimate)(& c.fir_config, buffer, buffer, usedSize);
}

// децимация НЧ спектра для увеличения разрешения
static void fftzoom_af(FLOAT_t * buffer, unsigned zoompow2, unsigned normalFFT)
{
	if (zoompow2 != 0)
	{
		ASSERT(ARRAY_SIZE(zoom_params) >= zoompow2);
		const struct zoom_param * const prm = & zoom_params [zoompow2 - 1];
		ARM_MORPH(arm_fir_decimate_instance) fir_config;
		const unsigned usedSize = normalFFT * prm->zoom;

		VERIFY(ARM_MATH_SUCCESS == ARM_MORPH(arm_fir_decimate_init)(& fir_config,
							prm->numTaps,
							prm->zoom,          // Decimation factor
							prm->pFIRCoeffs,
							gvars.fir_state,       	// Filter state variables
							usedSize));

		ARM_MORPH(arm_fir_decimate)(& fir_config, buffer, buffer, usedSize);
	}
}

static void
make_cmplx(
	FLOAT_t * dst,
	uint_fast16_t size,
	const FLOAT_t * realv,
	const FLOAT_t * imgev
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
	uint_fast16_t i;
	uint_fast16_t x;
	ARM_MORPH(arm_cfft_instance) fftinstance;

	// проверка, есть ли накопленный буфер для формирования спектра
	//static fftbuff_t * prevpf = NULL;
	fftbuff_t * pf;
	if (getfilled_fftbuffer(& pf) == 0)
		return 0;

	const unsigned usedsize = (NORMALFFT / FFTOVERLAP) << zoompow2;
	FLOAT_t * const largesigI = pf->largebuffI + LARGEFFT - usedsize;
	FLOAT_t * const largesigQ = pf->largebuffQ + LARGEFFT - usedsize;


	if (zoompow2 > 0)
	{
		ASSERT(ARRAY_SIZE(zoom_params) >= zoompow2);
		const struct zoom_param * const prm = & zoom_params [zoompow2 - 1];

		fftzoom_filer_decimate_ifspectrum(prm, largesigI, usedsize);
		fftzoom_filer_decimate_ifspectrum(prm, largesigQ, usedsize);
	}

//	if (prevpf == NULL)
//	{
//		prevpf = pf;
//		return 0;
//	}

	FLOAT_t * const fftinpt = gvars.cmplx_sig;
//	FLOAT_t * const prevLargesigI = prevpf->largebuffI + LARGEFFT - usedsize;
//	FLOAT_t * const prevLargesigQ = prevpf->largebuffQ + LARGEFFT - usedsize;

	// Подготовить массив комплексных чисел для преобразования в частотную область
	make_cmplx(fftinpt + NORMALFFT * 0, NORMALFFT, largesigQ, largesigI);
//	make_cmplx(fftinpt + NORMALFFT * 1, NORMALFFT / FFTOVERLAP, prevLargesigQ, prevLargesigI);
//	make_cmplx(fftinpt + NORMALFFT * 0, NORMALFFT / FFTOVERLAP, largesigQ, largesigI);


	ARM_MORPH(arm_cmplx_mult_real)(fftinpt, gvars.ifspec_wndfn, fftinpt,  NORMALFFT);	// Применить оконную функцию к IQ буферу
	VERIFY(ARM_MATH_SUCCESS == ARM_MORPH(arm_cfft_init)(& fftinstance, NORMALFFT));
	ARM_MORPH(arm_cfft)(& fftinstance, fftinpt, 0, 1);	// forward transform
	ARM_MORPH(arm_cmplx_mag)(fftinpt, fftinpt, NORMALFFT);	/* Calculate magnitudes */

	//endstamp();	// performance diagnostics

	enum { visiblefftsize = (int_fast64_t) NORMALFFT * SPECTRUMWIDTH_MULT / SPECTRUMWIDTH_DENOM };
	enum { fftsize = NORMALFFT };
	static const FLOAT_t fftcoeff = (FLOAT_t) 1 / (int32_t) (NORMALFFT / 2);
	for (x = 0; x < dx; ++ x)
	{
		const int fftpos = raster2fft(x, dx, fftsize, visiblefftsize);
		hbase [x] = fftinpt [fftpos] * fftcoeff;
	}

//	prevpf = pf;
//	release_fftbuffer(prevpf);
	release_fftbuffer(pf);
	return 1;
}

enum { BUFDIM_X = DIM_X, BUFDIM_Y = DIM_Y };
//enum { BUFDIM_X = ALLDX, BUFDIM_Y = ALLDY };

static int_fast32_t wffreqpix;			// глобальный пиксель по x центра спектра, для которой в последной раз отрисовали.
static uint_fast8_t wfzoompow2;				// масштаб, с которым выводили спектр
static int_fast16_t wfhscroll;			// сдвиг по шоризонтали (отрицаельный - влево) для водопада.
static uint_fast16_t wfvscroll;			// сдвиг по вертикали (в рабочем направлении) для водопада.
static uint_fast8_t wfclear;			// стирание всей областии отображение водопада.


#if WITHVIEW_3DSS

#define SIZEOF_WFL3DSS (sizeof gvars.u.wfj3dss)
#define ADDR_WFL3DSS (gvars.u.wfj3dss)

static WFL3DSS_T * atwfj3dss(uint_fast16_t x, uint_fast16_t y)
{
	ASSERT(glob_view_style == VIEW_3DSS);
	ASSERT(x < ALLDX);
	ASSERT(y < MAX_3DSS_STEP);
	return & gvars.u.wfj3dss [y][x];
}

static uint_fast16_t wfj3dss_peek(uint_fast16_t x, uint_fast16_t y)
{
	return * atwfj3dss(x, y);
}

static void wfj3dss_poke(uint_fast16_t x, uint_fast16_t y, WFL3DSS_T val)
{
	* atwfj3dss(x, y) = val;
}
#endif /* WITHVIEW_3DSS */

static
PACKEDCOLORPIP_T *
atwflj(uint_fast16_t x, uint_fast16_t y)
{
#if WITHVIEW_3DSS
	ASSERT(glob_view_style != VIEW_3DSS);
#endif /* WITHVIEW_3DSS */
	return colpip_mem_at(gvars.u.wfjarray, ALLDX, WFROWS, x, y);
}

// стираем буфер усреднения FFT
static void fft_avg_clear(void)
{
	ARM_MORPH(arm_fill)(0, gvars.Yold_spe, ALLDX);
}

// стираем буфер усреднения водопада
static void wfl_avg_clear(void)
{
	ARM_MORPH(arm_fill)(0, gvars.Yold_wtf, ALLDX);
}

// стираем целиком старое изображение водопада
static void wflclear(void)
{
    switch (glob_view_style)
    {
#if WITHVIEW_3DSS
    case VIEW_3DSS:
     	memset(ADDR_WFL3DSS, 0, SIZEOF_WFL3DSS);
    	break;
#endif /* WITHVIEW_3DSS */

    default:
    	colpip_fillrect(gvars.u.wfjarray, ALLDX, WFROWS, 0, 0, ALLDX, WFROWS, display2_bgcolorwfl());
    	break;
    }
}

// частота увеличилась - надо сдвигать картинку влево
// нужно сохрянять часть старого изображения
static void wflshiftleft(uint_fast16_t pixels)
{
	uint_fast16_t y;

	if (pixels == 0)
		return;

	// двигаем буфер усреднения значений WTF и FFT
	memmove(& gvars.Yold_spe [0], & gvars.Yold_spe [pixels], (ALLDX - pixels) * sizeof gvars.Yold_spe [0]);
	ARM_MORPH(arm_fill)(0, & gvars.Yold_spe [ALLDX - pixels], pixels);

	memmove(& gvars.Yold_wtf [0], & gvars.Yold_wtf [pixels], (ALLDX - pixels) * sizeof gvars.Yold_wtf [0]);
	ARM_MORPH(arm_fill)(0, & gvars.Yold_wtf [ALLDX - pixels], pixels);

    switch (glob_view_style)
    {
#if WITHVIEW_3DSS
    case VIEW_3DSS:
         for (y = 0; y < MAX_3DSS_STEP; ++ y)
     	{
     		memmove(
     				atwfj3dss(0, y),			// to
					atwfj3dss(pixels, y),		// from
     				(ALLDX - pixels) * sizeof (WFL3DSS_T)
     		);
     		memset(atwfj3dss(ALLDX - pixels, y), 0, pixels * sizeof (WFL3DSS_T));
    	}
 		break;
#endif /* WITHVIEW_3DSS */

    default:
        for (y = 0; y < WFROWS; ++ y)
    	{
    		memmove(
    				atwflj(0, y),		// to
    				atwflj(pixels, y),	// from
    				(ALLDX - pixels) * sizeof (PACKEDCOLORPIP_T)
    		);
    	}
        // заполнение вновь появившегося прямоугольника
    	colpip_fillrect(gvars.u.wfjarray, ALLDX, WFROWS, ALLDX - pixels, 0, pixels, WFROWS, display2_bgcolorwfl());
     	break;
    }
}

// частота уменьшилась - надо сдвигать картинку вправо
// нужно сохрянять часть старого изображения
// в строке wfrow - новое
static void wflshiftright(uint_fast16_t pixels)
{
	uint_fast16_t y;

	if (pixels == 0)
		return;
	// двигаем буфер усреднения значений WTF и FFT
	memmove(& gvars.Yold_spe [pixels], & gvars.Yold_spe [0], (ALLDX - pixels) * sizeof gvars.Yold_spe [0]);
	ARM_MORPH(arm_fill)(0, & gvars.Yold_spe [0], pixels);

	memmove(& gvars.Yold_wtf [pixels], & gvars.Yold_wtf [0], (ALLDX - pixels) * sizeof gvars.Yold_wtf [0]);
	ARM_MORPH(arm_fill)(0, & gvars.Yold_wtf [0], pixels);


    switch (glob_view_style)
    {
#if WITHVIEW_3DSS
    case VIEW_3DSS:
       	for (y = 0; y < MAX_3DSS_STEP; ++ y)
		{
       		memmove(
       				atwfj3dss(pixels, y),	// to
					atwfj3dss(0, y),		// from
					(ALLDX - pixels) * sizeof (WFL3DSS_T)
        		);
     		memset(atwfj3dss(0, y), 0, pixels * sizeof (WFL3DSS_T));
       	}
        break;
#endif /* WITHVIEW_3DSS */

    default:
    	for (y = 0; y < WFROWS; ++ y)
    	{
			memmove(
					atwflj(pixels, y),	// to
					atwflj(0, y),		// from
					(ALLDX - pixels) * sizeof (PACKEDCOLORPIP_T)
				);
    	}
        // заполнение вновь появившегося прямоугольника
    	colpip_fillrect(gvars.u.wfjarray, ALLDX, WFROWS, 0, 0, pixels, WFROWS, display2_bgcolorwfl());
     	break;
    }
}

// при смене диапазона или частот  при отсутствии необзодимости сохранять часть старого изображения водопада
// стираем целиком старое изображение водопада
// в строке 0 - новое
static void wfsetupnew(void)
{
	wflclear();	// Очистка водопада
	fft_avg_clear(); // очищаем буфер усреднения FFT
	wfl_avg_clear(); // очищаем буфер усреднения водопада
}

// отрисовка вновь появившихся данных на водопаде (в случае использования аппаратного scroll видеопамяти).
static void display_wfputrow(uint_fast16_t x, uint_fast16_t y, const PACKEDCOLORPIP_T * p)
{
	colpip_copy_to_draw(
			(uintptr_t) p, GXSIZE(ALLDX, 1) * sizeof * p,
			p, ALLDX, 1, x, y);
}

#if ! LINUX_SUBSYSTEM
	#include "dsp/window_functions.h"
#endif /* ! LINUX_SUBSYSTEM */

static void
display2_wfl_init(
	uint_fast8_t xgrid,
	uint_fast8_t ygrid,
	dctx_t * pctx
	)
{
	static subscribeint32_t rtsregister;

    ARM_MORPH(arm_nuttall4b)(gvars.ifspec_wndfn, WITHFFTSIZEWIDE);	/* оконная функция для показа радиоспектра */

	//printsigwnd();	// печать оконных коэффициентов для формирования таблицы во FLASH
	//toplogdb = LOG10F((FLOAT_t) INT32_MAX / waterfalrange);
	fftbuffer_initialize();

	subscribeint32(& rtstargetsint, & rtsregister, NULL, saveIQRTSxx);

#if ! defined (COLORPIP_SHADED)
	{
		int i;
		// Init 256 colors palette
		for (i = 0; i < PALETTESIZE; ++ i)
		{
			const COLOR24_T c = colorgradient(i, PALETTESIZE - 1);
			wfpalette [i] = TFTRGB(COLOR24_R(c), COLOR24_G(c), COLOR24_B(c));
		}
	}
#endif /* !  defined (COLORPIP_SHADED) */

	{
		int i;
		/* массив значений для раскраски спектра */
		for (i = 0; i < SPDY; ++ i)
		{
	#if LCDMODE_MAIN_L8
			gvars.color_scale [i] = normalize(i, 0, SPDY - 1, PALETTESIZE - 1);
	#else /* LCDMODE_MAIN_L8 */
			gvars.color_scale [i] = wfpalette [normalize(i, 0, SPDY - 1, PALETTESIZE - 1)];
	#endif /* LCDMODE_MAIN_L8 */
		}
	}

	wflclear();	// Очистка водопада
	fft_avg_clear();	// Сброс фильтра
	wfl_avg_clear();	// Сброс фильтра
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
int_fast32_t
deltafreq2abspix(
	int_fast32_t f,	// частота в герцах
	int_fast32_t bw,	// полоса обзора в герцах
	uint_fast16_t width	// ширина экрана
	)
{
	const int_fast32_t pc = ((int_fast64_t) f * width) / bw;	// абсолютный пиксель соответствующий частоте

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
	const int_fast32_t pm = deltafreq2abspix(fm, bw, width);	// абсолютный пиксель маркера
	const int_fast32_t f0 = fc - bw / 2;	// частота левого края окна
	const int_fast32_t p0 = deltafreq2abspix(f0, bw, width);	// абсолютный пиксель левого края окна

//	if (fm < 0)
//		return UINT16_MAX;	// частота маркера края выходит за пределы представимого
//	if (f0 < 0)
//		return UINT16_MAX;	// частота левого края выходит за пределы представимого
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
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t row0,	// вертикальная координата начала занимаемой области (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	int_fast32_t f0,	// center frequency
	int_fast32_t bw		// span
	)
{
	const COLORPIP_T color0 = COLORPIP_GRIDCOLOR0;	// макркер на центре
	const COLORPIP_T color = COLORPIP_GRIDCOLOR2;	// макркеры частот сетки
	const COLORPIP_T colordigits = COLORPIP_GRIDDIGITS;	// макркеры частот сетки

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
					colpip_string3_tbg(buffer, BUFDIM_X, BUFDIM_Y, xtext, row0, buf2, colordigits);
					colpip_xor_vline(buffer, BUFDIM_X, BUFDIM_Y, xmarker, row0 + MARKERH, h - MARKERH, color);
				}
				else
					colpip_xor_vline(buffer, BUFDIM_X, BUFDIM_Y, xmarker, row0, h, color);
			}
		}
	}
	colpip_xor_vline(buffer, BUFDIM_X, BUFDIM_Y, ALLDX / 2, row0, h, color0);	// center frequency marker
}

// Преобразовать отношение выраженное в децибелах к "разам" отношения напряжений.

static FLOAT_t db2ratio(FLOAT_t valueDBb)
{
	return POWF(10, valueDBb / 20);
}

// отрисовка маркеров частот
static void
display_colorgrid_set(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t row0,	// вертикальная координата начала занимаемой области (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	int_fast32_t f0,	// center frequency
	int_fast32_t bw		// span
	)
{
	const COLORPIP_T color0 = COLORPIP_GRIDCOLOR0;	// макркер на центре
	const COLORPIP_T color = COLORPIP_GRIDCOLOR2;
	const COLORPIP_T colordigits = COLORPIP_GRIDDIGITS;	// макркеры частот сетки
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

			colpip_set_hline(buffer, BUFDIM_X, BUFDIM_Y, 0, valy, ALLDX, color);	// Level marker
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
					colpip_string3_tbg(buffer, BUFDIM_X, BUFDIM_Y, xtext, row0, buf2, colordigits);
					colpip_set_vline(buffer, BUFDIM_X, BUFDIM_Y, xmarker, row0 + markerh, h - markerh, color);
				}
				else
					colpip_set_vline(buffer, BUFDIM_X, BUFDIM_Y, xmarker, row0, h, color);
			}
		}
	}
	colpip_set_vline(buffer, BUFDIM_X, BUFDIM_Y, ALLDX / 2, row0, h, color0);	// center frequency marker
}

// отрисовка маркеров частот для 3DSS
static
void
display_colorgrid_3dss(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t row0,	// вертикальная координата начала занимаемой области (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	int_fast32_t f0,	// center frequency
	int_fast32_t bw		// span
	)
{
	const COLORPIP_T colorcenter = COLORPIP_GRIDCOLOR0;	// макркер на центре
	const COLORPIP_T colorgrid = COLORPIP_GREEN;
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

				colpip_set_vline(buffer, BUFDIM_X, BUFDIM_Y, xmarker, row0, h, colorgrid);
			}
		}
	}
	colpip_set_vline(buffer, BUFDIM_X, BUFDIM_Y, ALLDX / 2, row0, h, colorcenter);	// center frequency marker
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

static int scalecolor(int component, int m, int delta)
{
	return component * m / delta;
}

/* получение цвета заполнения "шторки" - индикатор полосы пропускания примника на спкктре. */
static COLORPIP_T display2_rxbwcolor(COLORPIP_T colorfg, COLORPIP_T colorbg)
{
#if defined (COLORPIP_SHADED)
	return colorfg;
#else
	const int fg_a = COLORPIP_A(colorfg);
	const int fg_r = COLORPIP_R(colorfg);
	const int fg_g = COLORPIP_G(colorfg);
	const int fg_b = COLORPIP_B(colorfg);

	const int bg_a = COLORPIP_A(colorbg);
	const int bg_r = COLORPIP_R(colorbg);
	const int bg_g = COLORPIP_G(colorbg);
	const int bg_b = COLORPIP_B(colorbg);

	const int delta_r = fg_r - bg_r;
	const int delta_g = fg_g - bg_g;
	const int delta_b = fg_b - bg_b;

	unsigned m = glob_rxbwsatu;	// 0..100
	return TFTALPHA(fg_a, TFTRGB(scalecolor(delta_r, m, 100) + bg_r, scalecolor(delta_g, m, 100) + bg_g, scalecolor(delta_b, m, 100) + bg_b));
#endif
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
	colmain_setcolors(COLORPIP_SPECTRUMBG, COLORPIP_SPECTRUMFG); // цвет спектра при сполошном заполнении

#else /* */
	PACKEDCOLORPIP_T * const colorpip = getscratchwnd();
	(void) x0;
	(void) y0;
	(void) pctx;

#if WITHTOUCHGUI
	const uint_fast16_t spy = ALLDY - FOOTER_HEIGHT - 15;
#else
	const uint_fast16_t spy = ALLDY - 15;
#endif
	const COLORPIP_T rxbwcolor = display2_rxbwcolor(COLORPIP_SPECTRUMBG2, COLORPIP_SPECTRUMBG);

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
			const COLORPIP_T bgcolor = display_getbgcolor();
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
						wfj3dss_poke(x, current_3dss_step, val);

						for (dy = spy - 1, j = 0; dy > ynew; dy --, j ++)
						{
							if (x > xleft && x < xrightv && gview3dss_mark)
								colpip_point(colorpip, BUFDIM_X, BUFDIM_Y, x, dy, COLORPIP_SPECTRUMFG);
							else
								colpip_point(colorpip, BUFDIM_X, BUFDIM_Y, x, dy, gvars.color_scale [j]);
						}

						if (x)
						{
							colpip_line(colorpip, BUFDIM_X, BUFDIM_Y, x - 1, ylast_sp, x, ynew, COLORPIP_WHITE, 1);
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
							const uint_fast16_t t0 = wfj3dss_peek(x, draw_step);
							uint_fast16_t y1 = y0 - t0;
							int_fast16_t h = y0 - y1 - i / DEPTH_ATTENUATION;		// высота пика
							h = h < 0 ? 0 : h;
							h = h > y0 ? y0 : h;

							for (; h > 0; h --)
							{
								ASSERT(y0 >= h);
								/* предотвращение отрисовки по ранее закрашенной области*/
								if (* colpip_mem_at(colorpip, BUFDIM_X, BUFDIM_Y, x_d, y0 - h) != bgcolor)
									break;

								colpip_point(colorpip, BUFDIM_X, BUFDIM_Y, x_d, y0 - h, gvars.color_scale [h]);
							}
							x_old = x_d;
						}
					}
				}
				draw_step = calcprev(draw_step, MAX_3DSS_STEP);
			}

			// прожвижение по истории (должно быть в latch)
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
					colpip_line(colorpip, BUFDIM_X, BUFDIM_Y, x - 1, ylast_sp, x, y1, COLORPIP_BLACK, 0);

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
				colpip_fillrect(colorpip, BUFDIM_X, BUFDIM_Y, 0, SPY0, ALLDX, SPDY, COLORPIP_SPECTRUMBG);
			}
			else
			{
				if (xleft > 0)
				{
					colpip_fillrect(colorpip, BUFDIM_X, BUFDIM_Y, 0, SPY0, xleft, SPDY, COLORPIP_SPECTRUMBG);
				}
				if (xrightv < ALLDX)
				{
					colpip_fillrect(colorpip, BUFDIM_X, BUFDIM_Y, xrightv, SPY0, ALLDX - xrightv, SPDY, COLORPIP_SPECTRUMBG);
				}
			}
			// Изображение "шторки".
			if (xleft < xrightv)
			{
				colpip_fillrect(colorpip, BUFDIM_X, BUFDIM_Y, xleft, SPY0, xrightv - xleft, SPDY, rxbwcolor);
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
					colpip_set_vline(colorpip, BUFDIM_X, BUFDIM_Y, x, ynew + SPY0, SPDY - ynew, COLORPIP_SPECTRUMFG);
				}

				if (x)
				{
					colpip_line(colorpip, BUFDIM_X, BUFDIM_Y, x - 1, ylast, x, ynew, COLORPIP_SPECTRUMLINE, 1);
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
	const int_fast32_t f0pix = deltafreq2abspix(hamradio_get_freq_pathi(pathi), bw, ALLDX);	/* pixel of frequency at middle of spectrum */

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
	wfrow = (wfrow == 0) ? (WFROWS - 1) : (wfrow - 1);
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
			colpip_putpixel(gvars.u.wfjarray, ALLDX, WFROWS, x, wfrow, val);	// запись в буфер водопада индекса палитры
		#else /* LCDMODE_MAIN_L8 */
			ASSERT(val >= 0);
			ASSERT(val < ARRAY_SIZE(wfpalette));
			colpip_putpixel(gvars.u.wfjarray, ALLDX, WFROWS, x, wfrow, wfpalette [val]);	// запись в буфер водопада цветовой точки
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

#elif WITHFASTWATERFLOW || ! LCDMODE_MAIN_L8
	// следы спектра ("водопад") на цветных дисплеях
	/* быстрое отображение водопада (но требует больше памяти) */

	#if ! LCDMODE_HORFILL
		#error LCDMODE_HORFILL must be defined
	#endif /* ! LCDMODE_HORFILL */

#if WITHVIEW_3DSS
	if (glob_view_style != VIEW_3DSS)
#endif /* WITHVIEW_3DSS */
	{

		PACKEDCOLORPIP_T * const colorpip = getscratchwnd();
		const uint_fast16_t p1h = ulmin16(WFROWS - wfrow, WFDY);	// высота верхней части в результируюшем изображении
		const uint_fast16_t p2h = ulmin16(wfrow, WFDY - p1h);		// высота нижней части в результируюшем изображении
		const uint_fast16_t p1y = WFY0;
		const uint_fast16_t p2y = WFY0 + p1h;

		{
			/* перенос свежей части растра */
			ASSERT(atwflj(0, wfrow) != NULL);
			colpip_bitblt(
					(uintptr_t) colorpip, GXSIZE(BUFDIM_X, BUFDIM_Y) * sizeof (PACKEDCOLORPIP_T),
					colorpip, BUFDIM_X, BUFDIM_Y,
					0, p1y,	// координаты получателя
					(uintptr_t) gvars.u.wfjarray, sizeof (* gvars.u.wfjarray) * GXSIZE(ALLDX, WFROWS),	// папаметры для clean
					atwflj(0, 0),	// начальный адрес источника
					ALLDX, WFROWS, 	// размеры источника
					0, wfrow,	// координаты окна источника
					ALLDX, p1h, 	// размеры окна источника
					BITBLT_FLAG_NONE, 0);
		}
		if (p2h != 0)
		{
			ASSERT(atwflj(0, 0) != NULL);
			/* перенос старой части растра */
			colpip_bitblt(
					(uintptr_t) colorpip, 0 * sizeof (PACKEDCOLORPIP_T),
					colorpip, BUFDIM_X, BUFDIM_Y,
					0, p2y,		// координаты получателя
					(uintptr_t) gvars.u.wfjarray, 0 * sizeof (* gvars.u.wfjarray) * GXSIZE(ALLDX, WFROWS),	// размер области 0 - ранее уже вызывали clean
					atwflj(0, 0),	// начальный адрес источника
					ALLDX, WFROWS, 	// размеры источника
					0, 0,	// координаты окна источника
					ALLDX, p2h, 	// размеры окна источника
					BITBLT_FLAG_NONE, 0);
		}
	}

#else /* */

	// следы спектра ("водопад") на цветных дисплеях
	PACKEDCOLORPIP_T * const colorpip = getscratchwnd();
	uint_fast16_t y;

	// формирование растра
	// следы спектра ("водопад")
	for (y = 0; y < WFDY; ++ y)
	{
		uint_fast16_t x;
		for (x = 0; x < ALLDX; ++ x)
		{
			colpip_point(colorpip, BUFDIM_X, BUFDIM_Y, x, y + WFY0, wfpalette [* atwflj(x, (wfrow + y) % WFDY)]);
		}
	}

#endif /*  */
	(void) x0;
	(void) y0;
	(void) pctx;
}

// // подготовка изображения спектра и волрада
static void display2_gcombo(
	uint_fast8_t x0,
	uint_fast8_t y0,
	dctx_t * pctx
	)
{
	display2_spectrum(x0, y0, pctx);
	display2_waterfall(x0, y0, pctx);
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
	colpip_copy_to_draw(
			(uintptr_t) getscratchwnd(), sizeof (PACKEDCOLORPIP_T) * GXSIZE(BUFDIM_X, BUFDIM_Y),
			getscratchwnd(), BUFDIM_X, BUFDIM_Y, GRID2X(x0), GRID2Y(y0));
	#endif /* LCDMODE_LTDC */

#endif /* LCDMODE_S1D13781 */
}

static
PACKEDCOLORPIP_T * getscratchwnd(void)
{
#if HHWMG
	// Спектр на монохромных дисплеях
	// или на цветных, где есть возможность раскраски растровой картинки.

	return NULL;	//spectmonoscr;

#else /* */

	#if (LCDMODE_LTDC)

		pipparams_t pip;
		display2_getpipparams(& pip);
		return colpip_mem_at(colmain_fb_draw(), DIM_X, DIM_Y, pip.x, pip.y);

	#else /* LCDMODE_LTDC */

		static PACKEDCOLORPIP_T tbuff0 [GXSIZE(BUFDIM_X, BUFDIM_Y)];
		return tbuff0;

	#endif /* LCDMODE_LTDC */

#endif /* LCDMODE_S1D13781 */
}


#else /* WITHSPECTRUMWF && ! LCDMODE_HD44780 && ! LCDMODE_DUMMY */

static
PACKEDCOLORPIP_T * getscratchwnd(void)
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
#if LINUX_SUBSYSTEM
	if (key != REDRM_INIS)
		return;
#endif /* LINUX_SUBSYSTEM */

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
#if LINUX_SUBSYSTEM
		return;

#elif STMD
	ASSERT(key < REDRM_count);

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
#if LINUX_SUBSYSTEM
	enum { WALKCOUNT = sizeof dzones / sizeof dzones [0] };
	uint8_t dpage = REDRSUBSET(amenuset());

	for (int i = 0; i < WALKCOUNT; i ++)
	{
		const struct dzone * const p = & dzones [i];
		if (p->subset >= dpage && p->key == REDRM_ALL)
			(* p->redraw)(p->x, p->y, display2_getcontext());
	}

#elif STMD
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

// Обработка клавиатуры и валкодеров
void display2_mode_keyboard(
	uint_fast8_t menuset	/* индекс режима отображения (0..DISPLC_MODCOUNT - 1) */
	)
{
	//TP();
	display_walktroughsteps(REDRM_KEYB, getsubset(menuset, 0));
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
colpip_radius(
		PACKEDCOLORPIP_T * buffer,
		uint_fast16_t bx,	// ширина буфера
		uint_fast16_t by,	// высота буфера
		int xc, int yc,
		unsigned gs,
		unsigned r1, unsigned r2,
		COLORPIP_T color,
		int antialiasing,
		int style)			// 1 - растягивание по горизонтали
{
	int     x, y;
	int     x2, y2;

	x = xc + icos(gs, style ? r1 << 1 : r1);
	y = yc + isin(gs, r1);
	x2 = xc + icos(gs, style ? r2 << 1 : r2);
	y2 = yc + isin(gs, r2);

	colpip_line(buffer, bx, by, x, y, x2, y2, color, antialiasing);
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
colpip_segm(
		PACKEDCOLORPIP_T * buffer,
		uint_fast16_t bx,	// ширина буфера
		uint_fast16_t by,	// высота буфера
		int xc, int yc,
		unsigned gs, unsigned ge,
		unsigned r, int step,
		COLORPIP_T color,
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
			colpip_line(buffer, bx, by, xo, yo, x, y, color, antialiasing);
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

		colpip_line(buffer, bx, by, xo, yo, x, y, color, antialiasing); // рисовать линию
	}
}

/* Нарисовать прямоугольник со скругленными углами */
void colmain_rounded_rect(
		PACKEDCOLORPIP_T * buffer,
		uint_fast16_t bx,	// ширина буфера
		uint_fast16_t by,	// высота буфера
		uint_fast16_t x1,
		uint_fast16_t y1,
		uint_fast16_t x2,
		uint_fast16_t y2,
		uint_fast8_t r,		// радиус закругления углов
		COLORPIP_T color,
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

	colpip_segm(buffer, bx, by, x1 + r, y1 + r, 180, 270, r, 1, color, 1, 0); // up left
	colpip_segm(buffer, bx, by, x2 - r, y1 + r, 270, 360, r, 1, color, 1, 0); // up right
	colpip_segm(buffer, bx, by, x2 - r, y2 - r,   0,  90, r, 1, color, 1, 0); // down right
	colpip_segm(buffer, bx, by, x1 + r, y2 - r,  90, 180, r, 1, color, 1, 0); // down left

	colpip_line(buffer, bx, by, x1 + r, y1, x2 - r, y1, color, 0); // up
	colpip_line(buffer, bx, by, x1, y1 + r, x1, y2 - r, color, 0); // left
	colpip_line(buffer, bx, by, x1 + r, y2, x2 - r, y2, color, 0); // down
	colpip_line(buffer, bx, by, x2, y1 + r, x2, y2 - r, color, 0); // right

	if (fill)
	{
		PACKEDCOLORPIP_T * oldColor = colpip_mem_at(buffer, bx, by, x1 + r, y1 + r);
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
		wfsetupnew();	// при переключении стилей отображения очищать общий буфер

		if (glob_view_style == VIEW_3DSS)
			init_depth_map_3dss();
#endif /* WITHVIEW_3DSS */
	}
#endif /* WITHINTEGRATEDDSP */
}

/* Для VIEW_3DSS - индикация полосы пропускания на спектре */
void
board_set_view3dss_mark(uint_fast8_t v)
{
	gview3dss_mark = v != 0;
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
display2_set_lvlgridstep(uint_fast8_t v)
{
	glob_lvlgridstep = v;
}

/* 0..100 - насыщнность цвета заполнения "шторки" - индикатор полосы пропускания примника на спкктре. */
void
display2_set_rxbwsatu(uint_fast8_t v)
{
	glob_rxbwsatu = v;
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
display2_set_showdbm(uint_fast8_t v)
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

#if WITHTOUCHGUI

uint_fast8_t display_getpagegui(void)
{
	return DPAGE1;
}

uint_fast8_t display_getpage0(void)
{
	return DPAGE0;
}

COLORPIP_T display2_get_spectrum(int x)
{
	//int v = dsp_mag2y(filter_spectrum(x), SPDY - 1, glob_topdb, glob_bottomdb);
	int v = dsp_mag2y(gvars.spavgarray [x], SPDY - 1, glob_topdb, glob_bottomdb);
	return gvars.color_scale [v];
}

#endif /* WITHTOUCHGUI */
