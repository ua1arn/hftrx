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

#if LCDMODE_LTDC && WITHTOUCHGUI

#include "keyboard.h"
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

void
display_smeter2(
	uint_fast8_t xgrid,
	uint_fast8_t ygrid,
	dctx_t * pctx
	)
{
	const int xc = GRID2X(xgrid) + 120;
	const int yc = GRID2Y(ygrid) + 120;
	enum { halfsect = 30 };
	enum { gm = 270 };
	enum { gs = gm - halfsect };
	const int ge = gm + halfsect;
	const int stripewidth = 12; //16;
	const int r1 = 7 * GRID2Y(3) - 8;	//350;
	const int r2 = r1 - stripewidth;

	const uint_fast8_t is_tx = hamradio_get_tx();

	int gv, gv_trace = gs, gswr = gs;
	uint_fast16_t swr10; 														// swr10 = 0..30 for swr 1..4

	if (is_tx)																	// угол поворота стрелки; 246 минимум, 270 середина, 294 максимум
	{																			// добавить учет калибровок
		enum { gx_hyst = 3 };		// гистерезис в градусах
		/* фильтрация - (в градусах) */
		static uint_fast16_t gv_smooth = gs;
		static uint_fast16_t gswr_smooth = gs;

		uint_fast8_t tracemax;
		adcvalholder_t forward, reflected;

		gv = gs + normalize(board_getpwrmeter(& tracemax), 0, 200, ge - gs);

		forward = board_getswrmeter(& reflected, get_swrcalibr());
		const uint_fast16_t fullscale = (SWRMIN * 40 / 10) - SWRMIN;
									// рассчитанное  значение
		if (forward < get_minforward())
			swr10 = 0;				// SWR=1
		else if (forward <= reflected)
			swr10 = fullscale;		// SWR is infinite
		else
			swr10 = (forward + reflected) * SWRMIN / (forward - reflected) - SWRMIN;
		gswr = gs + normalize(swr10, 0, 30, ge - gs);

		if (gv > gs)
			gv_smooth = gv;

		if (gv == gs && gv_smooth > gs)
			gv = (gv_smooth -= gx_hyst) > gs ? gv_smooth : gs;

		if (gswr > gs)
			gswr_smooth = gswr;

		if (gswr == gs && gswr_smooth > gs)
			gswr = (gswr_smooth -= gx_hyst) > gs ? gswr_smooth : gs;
	}
	else
	{
		uint_fast8_t tracemax;

		gv = gs + normalize(board_getsmeter(& tracemax, 0, UINT8_MAX, 0), 120, 250, ge - gs); //270 + 24;
		gv_trace = gs + normalize(tracemax, 120, 250, ge - gs);
	}

	int rv1 = 7 * GRID2Y(3);	//350;
	int rv2 = rv1 - 3 * GRID2Y(3);
	enum { step1 = 3 };		// шаг для оцифровки S
	enum { step2 = 4 };		// шаг для оцифровки плюсов
	enum { step3 = 20 };	// swr
	static const int markersTX_pwr [] =
	{
		gs,
		gs + 2 * step1,
		gs + 4 * step1,
		gs + 6 * step1,
		gs + 8 * step1,
		gs + 10 * step1,
		gs + 12 * step1,
		gs + 14 * step1,
		gs + 16 * step1,
		gs + 18 * step1,
		gs + 20 * step1,
	};
	static const int markersTX_swr [] =
	{
		gs,
		gs + step3,
		gs + 2 * step3,
		gs + 3 * step3,
	};
	static const int markers [] =
	{
		//gs + 0 * step1,
		gs + 2 * step1,		// S1
		gs + 4 * step1,		// S3
		gs + 6 * step1,		// S5
		gs + 8 * step1,		// S7
		gs + 10 * step1,	// S9
	};
	static const int markersR [] =
	{
		gm + 2 * step2,	//
		gm + 4 * step2,
		gm + 6 * step2,
	};
	static const int markers2 [] =
	{
		//gs + 1 * step1,
		gs + 3 * step1,		// S2
		gs + 5 * step1,		// S4
		gs + 7 * step1,		// S6
		gs + 9 * step1,		// S8
	};
	static const int markers2R [] =
	{
		gm + 1 * step2,
		gm + 3 * step2,
		gm + 5 * step2,
	};

	const COLORMAIN_T smeter = COLORMAIN_WHITE;
	const COLORMAIN_T smeterplus = COLORMAIN_DARKRED;
	const uint_fast16_t pad2w3 = strwidth3("ZZ");

//	static uint_fast8_t first_run = 1;
//	static uint_fast16_t x1, y1, x2, y2;
//
//	if(first_run)
//	{
//		uint_fast16_t xx, yy;
//		first_run = 0;
//		polar_to_dek(xc, yc, gm, rv1 + 8 + SMALLCHARH3, & xx, & yy);
//		y1 = yy;
//		polar_to_dek(xc, yc, gs, rv2, & xx, & yy);
//		y2 = yy;
//		polar_to_dek(xc, yc, gs, r1 + 8 + SMALLCHARW3 * 2, & xx, & yy);
//		x1 = xx;
//		polar_to_dek(xc, yc, ge, r1 + 8 + SMALLCHARW3 * 2, & xx, & yy);
//		x2 = xx;
//		PRINTF("xc/yc - %d/%d\n", xc, yc);
//		PRINTF("x1/y1 - %d/%d\n", x1, y1);
//		PRINTF("x2/y2 - %d/%d\n", x2, y2);
//	}
//	colpip_rect(colmain_fb_draw(), DIM_X, DIM_Y, x1, y1, x2, y2, COLORPIP_WHITE, 0);

	if (is_tx)																	// шкала при передаче
	{
		unsigned p;
		unsigned i;
		for (p = 0, i = 0; i < sizeof markersTX_pwr / sizeof markersTX_pwr [0]; ++ i, p += 10)
		{
			char buf [10];
			uint_fast16_t xx, yy;
			if (i % 2 == 0)
			{
				display_radius(xc, yc, markersTX_pwr [i], r1, r1 + 8, smeter);
				polar_to_dek(xc, yc, markersTX_pwr [i], r1 + 8, & xx, & yy);
				local_snprintf_P(buf, sizeof buf / sizeof buf [0], PSTR("%u"), p);
				display_string3_at_xy(xx - strwidth3(buf) / 2, yy - pad2w3, buf, COLORMAIN_YELLOW, COLORMAIN_BLACK);
			}
			else
				display_radius(xc, yc, markersTX_pwr [i], r1, r1 + 4, smeter);
		}

		for (p = 1, i = 0; i < sizeof markersTX_swr / sizeof markersTX_swr [0]; ++ i, p += 1)
		{
			char buf [10];
			uint_fast16_t xx, yy;
			display_radius(xc, yc, markersTX_swr [i], r2, r2 - 8, smeter);
			polar_to_dek(xc, yc, markersTX_swr [i], r2 - 16, & xx, & yy);
			local_snprintf_P(buf, sizeof buf / sizeof buf [0], PSTR("%u"), p);
			display_string3_at_xy(xx - SMALLCHARW3 / 2, yy - SMALLCHARW3 / 2, buf, COLORMAIN_YELLOW, COLORMAIN_BLACK);
		}
	}
	else																		// шкала при приеме
	{
		unsigned p;
		unsigned i;
		for (p = 1, i = 0; i < sizeof markers / sizeof markers [0]; ++ i, p += 2)
		{
			char buf [10];
			uint_fast16_t xx, yy;
			display_radius(xc, yc, markers [i], r1, r1 + 8, smeter);
			polar_to_dek(xc, yc, markers [i], r1 + 8, & xx, & yy);
			local_snprintf_P(buf, sizeof buf / sizeof buf [0], PSTR("%u"), p);
			display_string3_at_xy(xx - SMALLCHARW3 / 2, yy - pad2w3, buf, COLORMAIN_YELLOW, COLORMAIN_BLACK);
		}
		for (i = 0; i < sizeof markers2 / sizeof markers2 [0]; ++ i)
		{
			display_radius(xc, yc, markers2 [i], r1, r1 + 4, smeter);
		}

		for (p = 20, i = 0; i < sizeof markersR / sizeof markersR [0]; ++ i, p += 20)
		{
			char buf [10];
			uint_fast16_t xx, yy;
			display_radius(xc, yc, markersR [i], r1, r1 + 8, smeterplus);
			polar_to_dek(xc, yc, markersR [i], r1 + 8, & xx, & yy);
			local_snprintf_P(buf, sizeof buf / sizeof buf [0], PSTR("+%u"), p);
			display_string3_at_xy(xx - strwidth3(buf) / 2, yy - pad2w3, buf, COLORMAIN_RED, COLORMAIN_BLACK);
		}
		for (i = 0; i < sizeof markers2R / sizeof markers2R [0]; ++ i)
		{
			display_radius(xc, yc, markers2R [i], r1, r1 + 4, smeterplus);
		}
	}

	display_segm(xc, yc, gs, gm, r1, 1, smeter);
	display_segm(xc, yc, gm, ge, r1, 1, is_tx ? smeter : smeterplus);
	display_segm(xc, yc, gs, ge, r2, 1, COLORMAIN_WHITE);

	if (is_tx)
	{
		// TX state
		if (gswr > gs)
		{
			uint_fast16_t xx, yy;
			display_segm(xc, yc, gs, gswr, r2 + 2, 1, COLORMAIN_YELLOW);
			display_segm(xc, yc, gs, gswr, r1 - 2, 1, COLORMAIN_YELLOW);
			display_radius(xc, yc, gs, r1 - 2, r2 + 2, COLORMAIN_YELLOW);
			display_radius(xc, yc, gswr, r1 - 2, r2 + 2, COLORMAIN_YELLOW);
			polar_to_dek(xc, yc, gswr - 1, r1 - 4, & xx, & yy);
			display_floodfill(xx, yy, COLORMAIN_YELLOW, COLORMAIN_BLACK);
		}

	}
	else
	{
		// RX state
		const COLORMAIN_T ct = gv_trace > gm ? COLORMAIN_RED : COLORMAIN_YELLOW;
		display_radius(xc - 1, yc, gv_trace, r1 - 2, r2 + 2, ct);
		display_radius(xc, yc, gv_trace, r1 - 2, r2 + 2, ct);
		display_radius(xc + 1, yc, gv_trace, r1 - 2, r2 + 2, ct);
	}

	{
		const COLORMAIN_T ct = gv > gm ? COLORMAIN_RED : COLORMAIN_GREEN;
		display_radius(xc - 1, yc, gv, rv1, rv2, ct);
		display_radius(xc, yc, gv, rv1, rv2, ct);
		display_radius(xc + 1, yc, gv, rv1, rv2, ct);
	}
}

static void button1_handler(void);
static void button2_handler(void);
static void button3_handler(void);
static void button4_handler(void);
static void button5_handler(void);
static void button6_handler(void);
static void button7_handler(void);
static void button8_handler(void);
void button9_handler(void);
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
void encoder2_menu (enc2_menu_t * enc2_menu);

	enum {
		TYPE_DUMMY,
		TYPE_BUTTON,
		TYPE_LABEL
	};

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
		FOOTER,							// постоянно отображаемые кнопки внизу экрана
		WINDOW_MODES,					// переключение режимов работы, видов модуляции
		WINDOW_BP,						// регулировка полосы пропускания фильтров выбранного режима
		WINDOW_AGC,						// выбор пресетов настроек АРУ для текущего режима модуляции
		WINDOW_FREQ,
		WINDOW_MENU,
		WINDOW_ENC2,
		WINDOW_UIF
	};

	enum {
		NAME_ARRAY_SIZE = 20,
		TEXT_ARRAY_SIZE = 20,
		MENU_ARRAY_SIZE = 30,
		TOUCH_ARRAY_SIZE = 40
	};

	enum {
		BTN_BUF_W = 96,
		BTN_BUF_H = 64
	};

	typedef ALIGNX_BEGIN PACKEDCOLORPIP_T bg_t [BTN_BUF_W][BTN_BUF_H] ALIGNX_END;

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
			{ 86, 44, },
			{ 50, 50, },
			{ 40, 40, },
	};

	enum { BG_COUNT = sizeof btn_bg / sizeof btn_bg[0] };

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

	static button_t button_handlers [] = {
	//   x1, y1, w, h,  onClickHandler,        state,   is_locked, is_trackable, parent,    visible,      payload,	              name, 		text
		{ },
		{ 0, 0, 86, 44, button1_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	  	 VISIBLE,     UINTPTR_MAX, 		"btnMode", 		"Mode", },
		{ 0, 0, 86, 44, button2_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	  	 VISIBLE,     UINTPTR_MAX, 		"btnAF", 		"AF|filter", },
		{ 0, 0, 86, 44, button3_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	  	 VISIBLE,     UINTPTR_MAX, 		"btnAGC", 		"AGC", },
		{ 0, 0, 86, 44, button4_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	  	 VISIBLE,     UINTPTR_MAX, 		"btn4", 		"Freq", },
		{ 0, 0, 86, 44, button5_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	 	 VISIBLE,     UINTPTR_MAX, 		"btn5", 		"", },
		{ 0, 0, 86, 44, button6_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	  	 VISIBLE,     UINTPTR_MAX, 		"btn6", 		"", },
		{ 0, 0, 86, 44, button7_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	  	 VISIBLE,     UINTPTR_MAX, 		"btn7", 		"", },
		{ 0, 0, 86, 44, button8_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	  	 VISIBLE,     UINTPTR_MAX, 		"btn8", 		"", },
		{ 0, 0, 86, 44, button9_handler, 	  CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	  	 VISIBLE,     UINTPTR_MAX, 		"btnSysMenu", 	"System|settings", },
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
		{ 0, 0, 86, 44, set_agc_off, 		  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, 		"btnAGCoff", 	"AGC|off", },
		{ 0, 0, 86, 44, set_agc_slow, 		  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, 		"btnAGCslow", 	"AGC|slow", },
		{ 0, 0, 86, 44, set_agc_fast, 		  CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, 		"btnAGCfast", 	"AGC|fast", },
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
//		{ 375, 120, buttons_uif_handler, 	NULL, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_UIF,   NON_VISIBLE, UINTPTR_MAX, 			"btnUIF_OK", "OK", },
	};
	enum { BUTTON_HANDLERS_COUNT = sizeof button_handlers / sizeof button_handlers[0] };

	typedef struct {
		uint16_t x;
		uint16_t y;
		uint8_t parent;
		uint8_t state;
		uint8_t is_trackable;
		uint8_t visible;
		char name [NAME_ARRAY_SIZE];
		char text [TEXT_ARRAY_SIZE];
		PACKEDCOLORPIP_T color;
		void (*onClickHandler) (void);
	} label_t;

	static label_t labels[] = {
	//     x,   y,  parent,      state,        visible,     name,   Text,  color
		{ },
		{ 0, 0, WINDOW_BP,   DISABLED,  0, NON_VISIBLE, "lbl_low",  "", COLORPIP_YELLOW, },
		{ 0, 0, WINDOW_BP,   DISABLED,  0, NON_VISIBLE, "lbl_high", "", COLORPIP_YELLOW, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 0, 0, WINDOW_ENC2, DISABLED,  0, NON_VISIBLE, "lbl_enc2_param", "", COLORPIP_WHITE, },
		{ 0, 0, WINDOW_ENC2, DISABLED,  0, NON_VISIBLE, "lbl_enc2_val",	  "", COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_UIF,  DISABLED,  0, NON_VISIBLE, "lbl_uif_param",  "", COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_UIF,  DISABLED,  0, NON_VISIBLE, "lbl_uif_val", 	  "", COLORPIP_WHITE, },
		{ 0, 0,	WINDOW_FREQ, DISABLED,  0, NON_VISIBLE, "lbl_freq_val",   "", COLORPIP_YELLOW, },
};
	enum { LABELS_COUNT = sizeof labels / sizeof labels[0] };

	typedef struct {
		uint16_t last_pressed_x; 	 // последняя точка касания экрана
		uint16_t last_pressed_y;
		uint8_t kbd_code;
		uint8_t selected_id;		 // индекс последнего выбранного элемента
		uint8_t selected_type;		 // тип последнего выбранного элемента
		uint8_t state;				 // последнее состояние
		uint8_t is_touching_screen; // есть ли касание экрана в данный момент
		uint8_t is_after_touch; 	 // есть ли касание экрана после выхода точки касания из элемента (при is_tracking == 0)
		uint8_t window_to_draw;	 // индекс записи с описанием запрошенного к отображению окна
		uint8_t is_tracking;		 // получение относительных координат точки перемещения нажатия
		int16_t vector_move_x;	 // в т.ч. и за границами элемента, при state == PRESSED
		int16_t vector_move_y;
	} gui_t;

	static gui_t gui = { 0, 0, KBD_CODE_MAX, 0, TYPE_DUMMY, CANCELLED, 0, 0, 0, 0, 0, 0, };

	enum align_t {
		ALIGN_LEFT_X 	= DIM_X >> 2,						// вертикальное выравнивание по центру левой половины экрана
		ALIGN_CENTER_X 	= DIM_X >> 1,						// вертикальное выравнивание по центру экрана
		ALIGN_RIGHT_X 	= ALIGN_LEFT_X + ALIGN_CENTER_X,	// вертикальное выравнивание по центру правой половины экрана
		ALIGN_Y 		= DIM_Y >> 1						// горизонтальное выравнивание всегда по центру экрана
	};

	typedef struct {
		uint8_t window_id;			// в окне будут отображаться элементы с соответствующим полем for_window
		enum align_t align_mode;	// вертикаль выравнивания окна
		uint16_t x1;
		uint16_t y1;
		uint16_t w;
		uint16_t h;
		char name[NAME_ARRAY_SIZE];	// текст, выводимый в заголовке окна
		uint8_t is_show;			// запрос на отрисовку окна
		uint8_t first_call;			// признак первого вызова для различных инициализаций
		void (*onVisibleProcess) (void);
	} window_t;

	static window_t windows[] = {
	//     window_id,   align_mode,     x1, y1, w, h,   title,         is_show, first_call, onVisibleProcess
		{ },
		{ WINDOW_MODES, ALIGN_CENTER_X, 0, 0, 402, 150, "Select mode", NON_VISIBLE, 0, window_mode_process},
		{ WINDOW_BP,    ALIGN_CENTER_X, 0, 0, 372, 205, "Bandpass",    NON_VISIBLE, 0, window_bp_process, },
		{ WINDOW_AGC,   ALIGN_CENTER_X, 0, 0, 372, 110, "AGC control", NON_VISIBLE, 0, window_agc_process},
		{ WINDOW_FREQ,  ALIGN_CENTER_X, 0, 0, 250, 215, "Freq:", 	   NON_VISIBLE, 0, window_freq_process, },
		{ WINDOW_MENU,  ALIGN_CENTER_X, 0, 0, 550, 240, "Settings",	   NON_VISIBLE, 0, window_menu_process, },
		{ WINDOW_ENC2, 	ALIGN_RIGHT_X, 	0, 0, 185, 105, "Fast menu",   NON_VISIBLE, 0, window_enc2_process},
		{ WINDOW_UIF, 	ALIGN_LEFT_X, 	0, 0, 200, 145, "",   		   NON_VISIBLE, 0, window_uif_process, },
	};
	enum { windows_count = sizeof windows / sizeof windows[0] };

	typedef struct {
		int16_t rotate;			// признак поворота второго энкодера
		uint8_t press;			// короткое нажание
		uint8_t hold;			// длинное нажатие
		uint8_t busy;			// второй энкодер выделен для обработки данных окна
		uint8_t rotate_done;	// событие поворота от энкодера обработано, можно получать новые данные
		uint8_t press_done;		// событие нажатия от энкодера обработано, можно получать новые данные
	} enc2_t;

	static enc2_t encoder2 = { 0, 0, 0, 0, 1, 1, };

	typedef struct {
		uint32_t val;
		uint8_t num;
		uint8_t key;
	} editfreq_t;

	static editfreq_t editfreq;

	enum {
		MENU_OFF,
		MENU_GROUPS,
		MENU_PARAMS,
		MENU_VALS,
		MENU_COUNT
	};

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

	typedef struct {
		char name [TEXT_ARRAY_SIZE];
		uint16_t menupos;
		uint8_t exitkey;
	} menu_by_name_t;

	menu_by_name_t menu_uif;

	typedef struct {
		uint_fast16_t type;			// тип элемента, поддерживающего реакцию на касания
		uint8_t state;				// текущее состояние элемента
		uint8_t visible;			// текущая видимость элемента
		uint8_t id;					// номер элемента из структуры описания
		uint8_t is_trackable;		// поддерживает ли элемент возврат относительных координат перемещения точки нажатия
		uint16_t x1;				// координаты окна
		uint16_t y1;
		uint16_t x2;
		uint16_t y2;
	} touch_t;

	static touch_t touch_elements[TOUCH_ARRAY_SIZE];
	static uint_fast8_t touch_count = 0;
	static uint_fast8_t menu_label_touched = 0;
	static uint_fast8_t menu_level;
	static enc2_menu_t * gui_enc2_menu;

	static uint_fast8_t find_button (uint_fast8_t id_window, const char * name)				// возврат id кнопки окна по ее названию
	{
		for (uint_fast8_t i = 1; i < BUTTON_HANDLERS_COUNT; i++)
		{
			if (button_handlers[i].parent == id_window && strcmp(button_handlers[i].name, name) == 0)
				return i;
		}
		return 0;
	}

	static uint_fast8_t find_label (uint_fast8_t id_window, const char * name)				// возврат id метки окна по ее названию
	{
		for (uint_fast8_t i = 1; i < LABELS_COUNT; i++)
		{
			if (labels[i].parent == id_window && strcmp(labels[i].name, name) == 0)
				return i;
		}
		return 0;
	}

	static void footer_buttons_state (uint_fast8_t state, const char * name)					// блокируются все, кроме name == text
	{
		static uint_fast8_t id = 0;
		if (state == DISABLED)
		{
			id = find_button(FOOTER, name);
			button_handlers[id].is_locked = BUTTON_LOCKED;
		} else
			button_handlers[id].is_locked = BUTTON_NON_LOCKED;

		for (uint_fast8_t i = 1; i < BUTTON_HANDLERS_COUNT; i++)
		{
			if (button_handlers[i].parent != FOOTER)
				break;
			button_handlers[i].state = button_handlers[i].text == name ? DISABLED : state;
		}
	}

	static void set_window(uint_fast8_t parent, uint_fast8_t value)
	{
		for (uint_fast8_t i = 1; i < BUTTON_HANDLERS_COUNT; i++)
		{
			if (button_handlers[i].parent == parent)
			{
				button_handlers[i].visible = value ? VISIBLE : NON_VISIBLE;
				button_handlers[i].is_locked = 0;
				if (button_handlers[i].visible)
				{
					touch_elements[touch_count].id = i;
					touch_elements[touch_count].type = TYPE_BUTTON;
					touch_count++;
				}
				else
					touch_count--;
			}
		}
		for (uint_fast8_t i = 1; i < LABELS_COUNT; i++)
		{
			if (labels[i].parent == parent)
			{
				labels[i].visible = value ? VISIBLE : NON_VISIBLE;
				if (labels[i].visible && labels[i].onClickHandler)
				{
					touch_elements[touch_count].id = i;
					touch_elements[touch_count].type = TYPE_LABEL;
					touch_count++;
				}
				if(! labels[i].visible && labels[i].onClickHandler)
					touch_count--;
			}
		}
		windows[parent].is_show = value ? VISIBLE : NON_VISIBLE;
		gui.window_to_draw = value ? parent : 0;
	}

	static void calculate_window_position(uint_fast8_t window_id)
	{
		window_t * win = & windows[window_id];
		win->y1 = ALIGN_Y - win->h / 2;

		switch (windows[window_id].align_mode)
		{
		case ALIGN_LEFT_X:
			if (ALIGN_LEFT_X - win->w / 2 < 0)
				win->x1 = 0;
			else
				win->x1 = ALIGN_LEFT_X - win->w / 2;
			break;

		case ALIGN_RIGHT_X:
			if (ALIGN_RIGHT_X + win->w / 2 > DIM_X)
				win->x1 = DIM_X - win->w;
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
			uint_fast8_t id_button_high = find_button(WINDOW_BP, "btnAF_2");
			uint_fast8_t id_button_low = find_button(WINDOW_BP, "btnAF_1");
			uint_fast8_t id_button_OK = find_button(WINDOW_BP, "btnAF_OK");

			if (gui.selected_id == id_button_low)
			{
				button_handlers[id_button_high].is_locked = 0;
				button_handlers[id_button_low].is_locked = 1;
			}
			else if (gui.selected_id == id_button_high)
			{
				button_handlers[id_button_high].is_locked = 1;
				button_handlers[id_button_low].is_locked = 0;
			}
			else if (gui.selected_id == id_button_OK)
			{
				set_window(WINDOW_BP, NON_VISIBLE);
				encoder2.busy = 0;
				footer_buttons_state(CANCELLED, "");
			}
		}
	}

	static void window_bp_process(void)
	{
		static uint_fast8_t val_high, val_low, val_c, val_w;
		static uint_fast16_t x_h, x_l, x_c, x_0, y_0;
		static window_t * win = & windows[WINDOW_BP];
		uint_fast16_t x_size = 290;
		static label_t * lbl_low, * lbl_high;
		static button_t * button_high, * button_low;

		if (win->first_call == 1)
		{
			uint_fast8_t interval = 20, id = 0, x, y;
			uint_fast8_t col1_int = 35, row1_int = 20;
			calculate_window_position(WINDOW_BP);
			x_0 = win->x1 + 50;											// оконные координаты нулевой точки графика
			y_0 = win->y1 + 90;

			while(button_handlers[++id].parent != WINDOW_BP)			// первое вхождение кнопки WINDOW_BP
				;
			x = win->x1 + col1_int;
			y = win->y1 + win->h - button_handlers[id].h - row1_int;
			do {
				button_handlers[id].x1 = x;
				button_handlers[id].y1 = y;
				x = x + interval + button_handlers[id].w;
				if (x + button_handlers[id].w > win->x1 + win->w)
				{
					x = win->x1 + col1_int;
					y = y + button_handlers[id].h + interval;
				}
			} while (button_handlers[++id].parent == WINDOW_BP);

			button_high = & button_handlers[find_button(WINDOW_BP, "btnAF_2")];
			button_low = & button_handlers[find_button(WINDOW_BP, "btnAF_1")];

			lbl_low = & labels[find_label(WINDOW_BP, "lbl_low")];
			lbl_high = & labels[find_label(WINDOW_BP, "lbl_high")];

			lbl_low->y = y_0 + SMALLCHARH;
			lbl_high->y = y_0 + SMALLCHARH;

			val_high = get_high_bp(0);
			val_low = get_low_bp(0);
			if (get_bp_type())			// BWSET_WIDE
			{
				strcpy(button_high->text, "High|cut");
				strcpy(button_low->text, "Low|cut");
				button_high->is_locked = 1;
			}
			else						// BWSET_NARROW
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

			if (get_bp_type())			// BWSET_WIDE
			{
				if (button_high->is_locked == 1)
					val_high = get_high_bp(encoder2.rotate);
				else if (button_low->is_locked == 1)
					val_low = get_low_bp(encoder2.rotate * 10);
				encoder2.rotate_done = 1;

				x_h = x_0 + normalize(val_high, 0, 50, x_size);
				x_l = x_0 + normalize(val_low / 10, 0, 50, x_size);
				x_c = x_l + (x_h - x_l) / 2;

				local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("%d"), val_high * 100);
				strcpy(lbl_high->text, buf);
				lbl_high->x = (x_h + strwidth(lbl_high->text) > x_0 + x_size) ?
						(x_0 + x_size - strwidth(lbl_high->text)) : x_h;

				local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("%d"), val_low * 10);
				strcpy(lbl_low->text, buf);
				lbl_low->x = (x_l - strwidth(lbl_low->text) < x_0 - 10) ? (x_0 - 10) : (x_l - strwidth(lbl_low->text));
			}
			else						// BWSET_NARROW
			{
				if (button_high->is_locked == 1)
				{
					val_c = get_high_bp(encoder2.rotate);
					val_w = get_low_bp(0) / 2;
				}
				else if (button_low->is_locked == 1)
				{
					val_c = get_high_bp(0);
					val_w = get_low_bp(encoder2.rotate) / 2;
				}
				encoder2.rotate_done = 1;
				x_c = x_0 + x_size / 2;
				x_l = x_c - normalize(val_w , 0, 500, x_size);
				x_h = x_c + normalize(val_w , 0, 500, x_size);

				local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("%d"), val_w * 20);
				strcpy(lbl_high->text, buf);
				lbl_high->x = x_c - strwidth(lbl_high->text) / 2;

				local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("P %d"), val_c * 10);
				strcpy(lbl_low->text, buf);
				lbl_low->x = x_0 + x_size - strwidth(lbl_low->text);
			}
		}
		PACKEDCOLORMAIN_T * fr = colmain_fb_draw();
		colpip_line(fr, DIM_X, DIM_Y, x_0 - 10, y_0, x_0 + x_size, y_0, COLORPIP_WHITE);
		colpip_line(fr, DIM_X, DIM_Y, x_0, y_0 - 45, x_0, y_0 + 5, COLORPIP_WHITE);
		colpip_line(fr, DIM_X, DIM_Y, x_l, y_0 - 40, x_l - 4, y_0 - 3, COLORPIP_YELLOW);
		colpip_line(fr, DIM_X, DIM_Y, x_h, y_0 - 40, x_h + 4, y_0 - 3, COLORPIP_YELLOW);
		colpip_line(fr, DIM_X, DIM_Y, x_l, y_0 - 40, x_h, y_0 - 40, COLORPIP_YELLOW);
		colpip_line(fr, DIM_X, DIM_Y, x_c, y_0 - 45, x_c, y_0 + 5, COLORPIP_RED);
	}

	static void buttons_freq_handler (void)
	{
		if (gui.window_to_draw == WINDOW_FREQ && editfreq.key == BUTTON_CODE_DONE)
			editfreq.key = button_handlers[gui.selected_id].payload;
	}

	static void window_freq_process (void)
	{
		static uint_fast8_t lbl_id;
		static window_t * win = & windows[WINDOW_FREQ];

		if (win->first_call == 1)
		{
			uint_fast8_t interval = 6, id = 0, x, y;
			uint_fast8_t col1_int = 20, row1_int = 40;
			win->first_call = 0;
			calculate_window_position(WINDOW_FREQ);

			while(button_handlers[++id].parent != WINDOW_FREQ)			// первое вхождение кнопки WINDOW_FREQ
				;
			x = win->x1 + col1_int;
			y = win->y1 + row1_int;
			do {
				button_handlers[id].x1 = x;
				button_handlers[id].y1 = y;
				x = x + interval + button_handlers[id].w;
				if (x + button_handlers[id].w > win->x1 + win->w)
				{
					x = win->x1 + col1_int;
					y = y + button_handlers[id].h + interval;
				}
			} while (button_handlers[++id].parent == WINDOW_FREQ);
			button_handlers[find_button(WINDOW_FREQ, "btnFreqOK")].is_locked = BUTTON_LOCKED;

			lbl_id = find_label(WINDOW_FREQ, "lbl_freq_val");
			labels[lbl_id].x = win->x1 + strwidth(win->name) + strwidth(" ") + 20;
			labels[lbl_id].y = win->y1 + 10;
			strcpy(labels[lbl_id].text, "     0 k");
			labels[lbl_id].color = COLORPIP_YELLOW;

			editfreq.val = 0;
			editfreq.num = 0;
			editfreq.key = BUTTON_CODE_DONE;

			return;
		}

		if (editfreq.key != BUTTON_CODE_DONE)
		{
			labels[lbl_id].color = COLORPIP_YELLOW;
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
				if(gui_set_freq(editfreq.val * 1000))
				{
					set_window(WINDOW_FREQ, NON_VISIBLE);
					footer_buttons_state(CANCELLED, "");
					gui_set_lockmode(0);
					disable_keyboard_redirect();
				} else
					labels[lbl_id].color = COLORPIP_RED;
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
			local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("%6d k"), editfreq.val);
			strcpy(labels[lbl_id].text, buf);
		}
	}

	void gui_uif_editmenu(const char * name, uint_fast16_t menupos, uint_fast8_t exitkey)
	{
		gui.window_to_draw = WINDOW_UIF;

		if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
		{
			set_window(gui.window_to_draw, VISIBLE);
			windows[gui.window_to_draw].first_call = 1;
			footer_buttons_state(DISABLED, "");
			strcpy(menu_uif.name, name);
			menu_uif.menupos = menupos;
			menu_uif.exitkey = exitkey;
		}
		else
		{
			set_window(gui.window_to_draw, NON_VISIBLE);
			footer_buttons_state(CANCELLED, "");
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
		if (gui.selected_type == TYPE_BUTTON && gui.selected_id == find_button(WINDOW_UIF, "btnUIF+"))
			encoder2.rotate = 1;
		else if (gui.selected_type == TYPE_BUTTON && gui.selected_id == find_button(WINDOW_UIF, "btnUIF-"))
			encoder2.rotate = -1;
		else if (gui.selected_type == TYPE_BUTTON && gui.selected_id == find_button(WINDOW_UIF, "btnUIF_OK"))
		{
			disable_keyboard_redirect();
			set_window(WINDOW_UIF, NON_VISIBLE);
			footer_buttons_state(CANCELLED, "");
		}
	}

	static void window_uif_process(void)
	{
		static uint_fast8_t id_lbl_uif_param, id_lbl_uif_val, window_center_x;
		static uint_fast8_t id_button_up = 0, id_button_down = 0;
		uint_fast8_t button_menu_w = 40, button_menu_h = 40;

		if (windows[WINDOW_UIF].first_call == 1)
		{
			windows[WINDOW_UIF].first_call = 0;
			calculate_window_position(WINDOW_UIF);
			window_center_x =  windows[WINDOW_UIF].x1 + windows[WINDOW_UIF].w / 2;

			id_lbl_uif_param = find_label(WINDOW_UIF, "lbl_uif_param");
			id_lbl_uif_val = find_label(WINDOW_UIF, "lbl_uif_val");
			strcpy(labels[id_lbl_uif_param].text, menu_uif.name);
			const char * v = gui_edit_menu_item(menu_uif.menupos, 0);
			strcpy(labels[id_lbl_uif_val].text, v);

			labels[id_lbl_uif_param].x = window_center_x - (strwidth(labels[id_lbl_uif_param].text) / 2);
			labels[id_lbl_uif_param].y = windows[WINDOW_UIF].y1 + SMALLCHARH;
			labels[id_lbl_uif_val].x = window_center_x - (strwidth(labels[id_lbl_uif_val].text) / 2);
			labels[id_lbl_uif_val].y = windows[WINDOW_UIF].y1 + SMALLCHARH * 4;

			id_button_up = find_button(WINDOW_UIF, "btnUIF+");
			id_button_down = find_button(WINDOW_UIF, "btnUIF-");

			button_handlers[id_button_down].x1 = labels[id_lbl_uif_val].x - button_menu_w - 10;
			button_handlers[id_button_down].y1 = (labels[id_lbl_uif_val].y + SMALLCHARH / 2) - (button_menu_h / 2);
			button_handlers[id_button_up].x1 = labels[id_lbl_uif_val].x + strwidth(labels[id_lbl_uif_val].text) + 10;
			button_handlers[id_button_up].y1 = button_handlers[id_button_down].y1;

			enable_keyboard_redirect();
			return;
		}

		if (encoder2.rotate != 0)
		{
			const char * v = gui_edit_menu_item(menu_uif.menupos, encoder2.rotate);
			strcpy(labels[id_lbl_uif_val].text, v);
			labels[id_lbl_uif_val].x = window_center_x - (strwidth(labels[id_lbl_uif_val].text) / 2);
			encoder2.rotate_done = 1;

			button_handlers[id_button_down].x1 = labels[id_lbl_uif_val].x - button_menu_w - 10;
			button_handlers[id_button_up].x1 = labels[id_lbl_uif_val].x + strwidth(labels[id_lbl_uif_val].text) + 10;
		}

		if (gui.kbd_code != KBD_CODE_MAX)
		{
			if (gui.kbd_code == menu_uif.exitkey)
			{
				disable_keyboard_redirect();
				set_window(WINDOW_UIF, NON_VISIBLE);
				footer_buttons_state(CANCELLED, "");
			}
			gui.kbd_code = KBD_CODE_MAX;
		}
	}

	static void labels_menu_handler (void)
	{
		if (gui.selected_type == TYPE_LABEL)
		{
			if(strcmp(labels[gui.selected_id].name, "lbl_group") == 0)
			{
				menu[MENU_GROUPS].selected_label = gui.selected_id - menu[MENU_GROUPS].first_id;
				menu_label_touched = 1;
				menu_level = MENU_GROUPS;
			}
			else if(strcmp(labels[gui.selected_id].name, "lbl_params") == 0)
			{
				menu[MENU_PARAMS].selected_label = gui.selected_id - menu[MENU_PARAMS].first_id;
				menu_label_touched = 1;
				menu_level = MENU_PARAMS;
			}
			else if(strcmp(labels[gui.selected_id].name, "lbl_vals") == 0)
			{
				menu[MENU_VALS].selected_label = gui.selected_id - menu[MENU_VALS].first_id;
				menu[MENU_PARAMS].selected_label = menu[MENU_VALS].selected_label;
				menu_label_touched = 1;
				menu_level = MENU_VALS;
			}
		}
	}

	static void buttons_menu_handler(void)
	{
		if (gui.selected_type == TYPE_BUTTON && gui.selected_id == find_button(WINDOW_MENU, "btnSysMenu+"))
			encoder2.rotate = 1;
		else if (gui.selected_type == TYPE_BUTTON && gui.selected_id == find_button(WINDOW_MENU, "btnSysMenu-"))
			encoder2.rotate = -1;
	}

	static void window_menu_process(void)
	{
		static uint_fast8_t str_step = 0, menu_is_scrolling = 0, start_str_group = 0, start_str_params = 0;
		static uint_fast8_t id_button_up = 0, id_button_down = 0, button_pressed = 0;
		static uint_fast8_t button_menu_w = 0, button_menu_h = 0;

		if (windows[WINDOW_MENU].first_call == 1)
		{
			windows[WINDOW_MENU].align_mode = ALIGN_CENTER_X;						// выравнивание окна системных настроек только по центру
			calculate_window_position(WINDOW_MENU);

			windows[WINDOW_MENU].first_call = 0;
			set_menu_cond(VISIBLE);

			uint_fast8_t int_cols = 200, int_rows = 35;
			uint_fast8_t col1_int = 50, row1_int = 40;
			uint_fast8_t xn, yn;

			id_button_up = find_button(WINDOW_MENU, "btnSysMenu+");
			id_button_down = find_button(WINDOW_MENU, "btnSysMenu-");
			button_handlers[id_button_up].visible = NON_VISIBLE;
			button_handlers[id_button_down].visible = NON_VISIBLE;

			button_menu_w = button_handlers[id_button_up].w;
			button_menu_h = button_handlers[id_button_up].h;

			menu[MENU_GROUPS].add_id = 0;
			menu[MENU_GROUPS].selected_str = 0;
			menu[MENU_GROUPS].selected_label = 0;
			menu[MENU_PARAMS].add_id = 0;
			menu[MENU_PARAMS].selected_str = 0;
			menu[MENU_PARAMS].selected_label = 0;
			menu[MENU_VALS].add_id = 0;
			menu[MENU_VALS].selected_str = 0;
			menu[MENU_VALS].selected_label = 0;

			menu[MENU_GROUPS].first_id = 1;
			while (labels[++menu[MENU_GROUPS].first_id].parent != WINDOW_MENU);				// первое вхождение метки group
			menu[MENU_GROUPS].last_id = menu[MENU_GROUPS].first_id;
			while (strcmp(labels[++menu[MENU_GROUPS].last_id].name, "lbl_group") == 0);
			menu[MENU_GROUPS].last_id--;													// последнее вхождение метки group
			menu[MENU_GROUPS].num_rows = menu[MENU_GROUPS].last_id - menu[MENU_GROUPS].first_id;

			menu[MENU_PARAMS].first_id = menu[MENU_GROUPS].last_id + 1;						// первое вхождение метки params
			menu[MENU_PARAMS].last_id = menu[MENU_PARAMS].first_id;
			while (strcmp(labels[++menu[MENU_PARAMS].last_id].name, "lbl_params") == 0)
				;
			menu[MENU_PARAMS].last_id--;													// последнее вхождение метки params
			menu[MENU_PARAMS].num_rows = menu[MENU_PARAMS].last_id - menu[MENU_PARAMS].first_id;

			menu[MENU_VALS].first_id = menu[MENU_PARAMS].last_id + 1;						// первое вхождение метки vals
			menu[MENU_VALS].last_id = menu[MENU_VALS].first_id;
			while (strcmp(labels[++menu[MENU_VALS].last_id].name, "lbl_vals") == 0)
				;
			menu[MENU_VALS].last_id--;														// последнее вхождение метки vals
			menu[MENU_VALS].num_rows = menu[MENU_VALS].last_id - menu[MENU_VALS].first_id;

			menu[MENU_GROUPS].count = get_multilinemenu_block_groups(menu[MENU_GROUPS].menu_block) - 1;
			xn = windows[WINDOW_MENU].x1 + col1_int;
			yn = windows[WINDOW_MENU].y1 + row1_int;
			for(uint_fast8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
			{
				strcpy(labels[menu[MENU_GROUPS].first_id + i].text, menu[MENU_GROUPS].menu_block[i + menu[MENU_GROUPS].add_id].name);
				labels[menu[MENU_GROUPS].first_id + i].visible = VISIBLE;
				labels[menu[MENU_GROUPS].first_id + i].color = COLORPIP_WHITE;
				labels[menu[MENU_GROUPS].first_id + i].x = xn;
				labels[menu[MENU_GROUPS].first_id + i].y = yn;
				yn += int_rows;
			}

			menu[MENU_PARAMS].count = get_multilinemenu_block_params(menu[MENU_PARAMS].menu_block, menu[MENU_GROUPS].menu_block[menu[MENU_GROUPS].selected_str].index) - 1;
			xn += int_cols;
			yn = windows[WINDOW_MENU].y1 + row1_int;
			for(uint_fast8_t i = 0; i <= menu[MENU_PARAMS].num_rows; i++)
			{
				labels[menu[MENU_PARAMS].first_id + i].x = xn;
				labels[menu[MENU_PARAMS].first_id + i].y = yn;
				yn += int_rows;
				labels[menu[MENU_PARAMS].first_id + i].visible = NON_VISIBLE;
				labels[menu[MENU_PARAMS].first_id + i].color = COLORPIP_WHITE;
				if (menu[MENU_PARAMS].count < i)
					continue;
				strcpy(labels[menu[MENU_PARAMS].first_id + i].text, menu[MENU_PARAMS].menu_block[i + menu[MENU_PARAMS].add_id].name);
				labels[menu[MENU_PARAMS].first_id + i].visible = VISIBLE;
			}

			menu[MENU_PARAMS].selected_str = 0;
			menu[MENU_VALS].count = menu[MENU_PARAMS].count < menu[MENU_VALS].num_rows ? menu[MENU_PARAMS].count :  menu[MENU_VALS].num_rows;
			get_multilinemenu_block_vals(menu[MENU_VALS].menu_block, menu[MENU_PARAMS].menu_block[menu[MENU_PARAMS].selected_str].index, menu[MENU_VALS].count);
			xn += int_cols;
			yn = windows[WINDOW_MENU].y1 + row1_int;
			for(uint_fast8_t i = 0; i <= menu[MENU_VALS].num_rows; i++)
			{
				labels[menu[MENU_VALS].first_id + i].x = xn;
				labels[menu[MENU_VALS].first_id + i].y = yn;
				yn += int_rows;
				labels[menu[MENU_VALS].first_id + i].visible = NON_VISIBLE;
				labels[menu[MENU_VALS].first_id + i].color = COLORPIP_WHITE;
				if (menu[MENU_VALS].count < i)
					continue;
				strcpy(labels[menu[MENU_VALS].first_id + i].text, menu[MENU_VALS].menu_block[i + menu[MENU_VALS].add_id].name);
				labels[menu[MENU_VALS].first_id + i].visible = VISIBLE;
			};
			str_step = labels[menu[MENU_GROUPS].first_id + 1].y - labels[menu[MENU_GROUPS].first_id].y;
			menu_level = MENU_GROUPS;
			return;
		}

		if(gui.is_tracking && gui.selected_type == TYPE_LABEL && gui.vector_move_y != 0)
		{
			if (! menu_is_scrolling)
			{
				start_str_group = menu[MENU_GROUPS].add_id;
				start_str_params = menu[MENU_PARAMS].add_id;
			}
			ldiv_t r = ldiv(gui.vector_move_y, str_step);
			if(strcmp(labels[gui.selected_id].name, "lbl_group") == 0)
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
			else if(strcmp(labels[gui.selected_id].name, "lbl_params") == 0 &&
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
			gui.vector_move_y = 0;
			gui.vector_move_x = 0;
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
			if (menu_level == MENU_VALS)
			{
				menu[MENU_VALS].selected_label = menu[MENU_PARAMS].selected_label;
				uint_fast8_t id_sel_label = menu[MENU_VALS].first_id + menu[MENU_VALS].selected_label;

				button_handlers[id_button_down].visible = VISIBLE;
				button_handlers[id_button_down].x1 = labels[id_sel_label].x - button_menu_w - 10;
				button_handlers[id_button_down].y1 = (labels[id_sel_label].y + SMALLCHARH / 2) - (button_menu_h / 2);

				button_handlers[id_button_up].visible = VISIBLE;
				button_handlers[id_button_up].x1 = labels[id_sel_label].x + strwidth(labels[id_sel_label].text) + 10;
				button_handlers[id_button_up].y1 = button_handlers[id_button_down].y1;
				for (uint8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
				{
					labels[i + menu[MENU_GROUPS].first_id].color = i == menu[MENU_GROUPS].selected_label ? COLORPIP_YELLOW : COLORPIP_GRAY;
					labels[i + menu[MENU_PARAMS].first_id].color = i == menu[MENU_PARAMS].selected_label ? COLORPIP_YELLOW : COLORPIP_GRAY;
					labels[i + menu[MENU_VALS].first_id].color = i == menu[MENU_PARAMS].selected_label ? COLORPIP_YELLOW : COLORPIP_GRAY;
				}
				menu_label_touched = 0;
			}
			if (menu_level == MENU_PARAMS)
			{
				button_handlers[id_button_down].visible = NON_VISIBLE;
				button_handlers[id_button_up].visible = NON_VISIBLE;
				for (uint8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
				{
					labels[i + menu[MENU_GROUPS].first_id].color = i == menu[MENU_GROUPS].selected_label ? COLORPIP_YELLOW : COLORPIP_GRAY;
					labels[i + menu[MENU_PARAMS].first_id].color = COLORPIP_WHITE;
					labels[i + menu[MENU_VALS].first_id].color = COLORPIP_WHITE;
				}
			}
			if (menu_level == MENU_GROUPS)
			{
				button_handlers[id_button_down].visible = NON_VISIBLE;
				button_handlers[id_button_up].visible = NON_VISIBLE;
				for (uint8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
				{
					labels[i + menu[MENU_GROUPS].first_id].color = COLORPIP_WHITE;
					labels[i + menu[MENU_PARAMS].first_id].color = COLORPIP_WHITE;
					labels[i + menu[MENU_VALS].first_id].color = COLORPIP_WHITE;
				}
			}

			encoder2.press = 0;
			encoder2.hold = 0;
			encoder2.press_done = 1;
		}

		if (menu_level == MENU_OFF)
		{
			set_window(WINDOW_MENU, NON_VISIBLE);
			encoder2.busy = 0;
			footer_buttons_state(CANCELLED, "");
			set_menu_cond(NON_VISIBLE);
			return;
		}

		if (encoder2.rotate != 0 && menu_level == MENU_VALS)
		{
			encoder2.rotate_done = 1;
			menu[MENU_PARAMS].selected_str = menu[MENU_PARAMS].selected_label + menu[MENU_PARAMS].add_id;
			strcpy(labels[menu[MENU_VALS].first_id + menu[MENU_PARAMS].selected_label].text,
					gui_edit_menu_item(menu[MENU_PARAMS].menu_block[menu[MENU_PARAMS].selected_str].index, encoder2.rotate));

			uint_fast8_t id_sel_label = menu[MENU_VALS].first_id + menu[MENU_VALS].selected_label;
			button_handlers[id_button_up].x1 = labels[id_sel_label].x + strwidth(labels[id_sel_label].text) + 10;
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

			menu[MENU_PARAMS].count = get_multilinemenu_block_params(menu[MENU_PARAMS].menu_block, menu[MENU_GROUPS].menu_block[menu[MENU_GROUPS].selected_str].index) - 1;

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
					strcpy(labels[menu[MENU_GROUPS].first_id + i].text, menu[MENU_GROUPS].menu_block[i + menu[MENU_GROUPS].add_id].name);

			menu[MENU_VALS].count = menu[MENU_PARAMS].count < menu[MENU_VALS].num_rows ? menu[MENU_PARAMS].count : menu[MENU_VALS].num_rows;
			get_multilinemenu_block_vals(menu[MENU_VALS].menu_block,  menu[MENU_PARAMS].menu_block[menu[MENU_PARAMS].add_id].index, menu[MENU_VALS].count);

			for(uint_fast8_t i = 0; i <= menu[MENU_PARAMS].num_rows; i++)
			{
				labels[menu[MENU_PARAMS].first_id + i].visible = NON_VISIBLE;
				labels[menu[MENU_PARAMS].first_id + i].state = DISABLED;
				labels[menu[MENU_VALS].first_id + i].visible = NON_VISIBLE;
				labels[menu[MENU_VALS].first_id + i].state = DISABLED;
				if (i > menu[MENU_PARAMS].count)
					continue;
				strcpy(labels[menu[MENU_PARAMS].first_id + i].text, menu[MENU_PARAMS].menu_block[i + menu[MENU_PARAMS].add_id].name);
				strcpy(labels[menu[MENU_VALS].first_id + i].text,   menu[MENU_VALS].menu_block[i].name);
				labels[menu[MENU_PARAMS].first_id + i].visible = VISIBLE;
				labels[menu[MENU_PARAMS].first_id + i].state = CANCELLED;
				labels[menu[MENU_VALS].first_id + i].visible = VISIBLE;
				labels[menu[MENU_VALS].first_id + i].state = CANCELLED;
			}
			menu_label_touched = 0;
		}
		if (menu_level != MENU_VALS)
			colpip_string_tbg(colmain_fb_draw(), DIM_X, DIM_Y, labels[menu[menu_level].selected_label + menu[menu_level].first_id].x - SMALLCHARW,
					labels[menu[menu_level].selected_label + menu[menu_level].first_id].y, ">", COLORPIP_YELLOW);
	}

	uint_fast8_t check_encoder2 (int_least16_t rotate)
	{
		if (encoder2.rotate_done || encoder2.rotate == 0)
		{
			encoder2.rotate = rotate;
			encoder2.rotate_done = 0;
		}
		return encoder2.busy;
	}

	void set_encoder2_state (uint_fast8_t code)
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

	void encoder2_menu (enc2_menu_t * enc2_menu)
	{
		gui.window_to_draw = WINDOW_ENC2;

		if (windows[gui.window_to_draw].is_show == NON_VISIBLE && enc2_menu->state != 0)
		{
			set_window(gui.window_to_draw, VISIBLE);
			windows[gui.window_to_draw].first_call = 1;
			footer_buttons_state(DISABLED, "");
			gui_enc2_menu = enc2_menu;
		}
		else if (windows[gui.window_to_draw].is_show == VISIBLE && enc2_menu->state == 0)
		{
			set_window(gui.window_to_draw, NON_VISIBLE);
			gui_enc2_menu = NULL;
			footer_buttons_state(CANCELLED, "");
		}
	}

	static void window_enc2_process(void)
	{
		static uint_fast8_t id_lbl_param, id_lbl_val;
		static uint_fast16_t window_center_x;

		if (windows[WINDOW_ENC2].first_call == 1)
		{
			calculate_window_position(WINDOW_ENC2);
			window_center_x =  windows[WINDOW_ENC2].x1 + windows[WINDOW_ENC2].w / 2;
			windows[WINDOW_ENC2].first_call = 0;
			id_lbl_param = find_label(WINDOW_ENC2, "lbl_enc2_param");
			id_lbl_val = find_label(WINDOW_ENC2, "lbl_enc2_val");
			labels[id_lbl_param].y = windows[WINDOW_ENC2].y1 + SMALLCHARH * 3;
			labels[id_lbl_val].y = labels[id_lbl_param].y + SMALLCHARH * 2;
			return;
		}
		if(gui_enc2_menu->updated)
		{
			strcpy(labels[id_lbl_param].text, gui_enc2_menu->param);
			remove_end_line_spaces(labels[id_lbl_param].text);
			strcpy(labels[id_lbl_val].text, gui_enc2_menu->val);
			labels[id_lbl_val].color = gui_enc2_menu->state == 2 ? COLORPIP_YELLOW : COLORPIP_WHITE;

			labels[id_lbl_param].x = window_center_x - strwidth(labels[id_lbl_param].text) / 2;
			labels[id_lbl_val].x = window_center_x - strwidth(labels[id_lbl_val].text) / 2;

			gui_enc2_menu->updated = 0;
		}
	}

	static void buttons_mode_handler(void)
	{
		if(gui.selected_type == TYPE_BUTTON)
		{
			if (windows[WINDOW_MODES].is_show && button_handlers[gui.selected_id].parent == WINDOW_MODES)
			{
				if (button_handlers[gui.selected_id].payload != UINTPTR_MAX)
					change_submode(button_handlers[gui.selected_id].payload);

				set_window(WINDOW_MODES, NON_VISIBLE);
				footer_buttons_state(CANCELLED, "");
			}
		}
	}

	static void window_mode_process(void)
	{
		if (windows[WINDOW_MODES].first_call == 1)
		{
			uint_fast8_t interval = 6, id = 0, x, y;
			uint_fast8_t col1_int = 20, row1_int = 40;
			windows[WINDOW_MODES].first_call = 0;
			calculate_window_position(WINDOW_MODES);

			while(button_handlers[++id].parent != WINDOW_MODES)			// первое вхождение кнопки WINDOW_MODES
				;
			x = windows[WINDOW_MODES].x1 + col1_int;
			y = windows[WINDOW_MODES].y1 + row1_int;
			do {
				button_handlers[id].x1 = x;
				button_handlers[id].y1 = y;
				x = x + interval + button_handlers[id].w;
				if (x + button_handlers[id].w > windows[WINDOW_MODES].x1 + windows[WINDOW_MODES].w)
				{
					x = windows[WINDOW_MODES].x1 + col1_int;
					y = windows[WINDOW_MODES].y1 + row1_int + button_handlers[id].h + interval;
				}
			} while (button_handlers[++id].parent == WINDOW_MODES);
			return;
		}
	}

	void window_agc_process(void)
	{
		if (windows[WINDOW_AGC].first_call == 1)
		{
			uint_fast8_t interval = 40, id = 0, x, y;
			uint_fast8_t col1_int = 20, row1_int = 40;
			windows[WINDOW_AGC].first_call = 0;
			calculate_window_position(WINDOW_AGC);

			while(button_handlers[++id].parent != WINDOW_AGC)			// первое вхождение кнопки WINDOW_AGC
				;
			x = windows[WINDOW_AGC].x1 + col1_int;
			y = windows[WINDOW_AGC].y1 + row1_int;
			do {
				button_handlers[id].x1 = x;
				button_handlers[id].y1 = y;
				x = x + interval + button_handlers[id].w;

			} while (button_handlers[++id].parent == WINDOW_AGC);
			return;
		}
	}

	static void button1_handler(void)
	{
		if(gui.selected_type == TYPE_BUTTON)
		{
			gui.window_to_draw = WINDOW_MODES;

			if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
			{
				set_window(gui.window_to_draw, VISIBLE);
				windows[gui.window_to_draw].first_call = 1;
				footer_buttons_state(DISABLED, button_handlers[gui.selected_id].name);
			}
			else
			{
				set_window(gui.window_to_draw, NON_VISIBLE);
				footer_buttons_state(CANCELLED, "");
			}
		}
	}

	static void button2_handler(void)
	{
		if(gui.selected_type == TYPE_BUTTON)
		{
			gui.window_to_draw = WINDOW_BP;

			if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
			{
				encoder2.busy = 1;
				set_window(gui.window_to_draw, VISIBLE);
				windows[gui.window_to_draw].first_call = 1;
				footer_buttons_state(DISABLED, button_handlers[gui.selected_id].name);
			}
			else
			{
				set_window(gui.window_to_draw, NON_VISIBLE);
				encoder2.busy = 0;
				footer_buttons_state(CANCELLED, "");
			}
		}
	}

	static void button3_handler(void)
	{
		if(gui.selected_type == TYPE_BUTTON)
		{
			gui.window_to_draw = WINDOW_AGC;

			if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
			{
				set_window(gui.window_to_draw, VISIBLE);
				windows[gui.window_to_draw].first_call = 1;
				footer_buttons_state(DISABLED, button_handlers[gui.selected_id].name);
			}
			else
			{
				set_window(gui.window_to_draw, NON_VISIBLE);
				footer_buttons_state(CANCELLED, "");
			}
		}
	}

	static void button4_handler(void)
	{
		if(gui.selected_type == TYPE_BUTTON)
		{
			gui.window_to_draw = WINDOW_FREQ;

			if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
			{
				set_window(gui.window_to_draw, VISIBLE);
				windows[gui.window_to_draw].first_call = 1;
				gui_set_lockmode(1);
				enable_keyboard_redirect();
				footer_buttons_state(DISABLED, button_handlers[gui.selected_id].name);
			}
			else
			{
				set_window(gui.window_to_draw, NON_VISIBLE);
				gui_set_lockmode(0);
				disable_keyboard_redirect();
				footer_buttons_state(CANCELLED, "");
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

		}
	}

	void button9_handler(void)
	{
		if(gui.selected_type == TYPE_BUTTON)
		{
			gui.window_to_draw = WINDOW_MENU;

			if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
			{
				set_window(gui.window_to_draw, VISIBLE);
				windows[gui.window_to_draw].first_call = 1;
				footer_buttons_state(DISABLED, button_handlers[gui.selected_id].name);
				encoder2.busy = 1;
			}
			else
			{
				set_window(gui.window_to_draw, NON_VISIBLE);
				footer_buttons_state(CANCELLED, "");
				encoder2.busy = 0;
				set_menu_cond(NON_VISIBLE);
			}
		}
	}

	/* Кнопка */
	void draw_button_pip(uint_fast8_t id)
	{
		PACKEDCOLORPIP_T * bg = NULL;
		btn_bg_t * b1 = NULL;
		uint_fast8_t i = 0;
		static const char delimeters [] = "|";
		const button_t * const bh = & button_handlers[id];

		do {
			if (bh->h == btn_bg[i].h && bh->w == btn_bg[i].w)
			{
				b1 = & btn_bg[i];
				break;
			}
		} while (++i < BG_COUNT);

		if (b1 == NULL)				// если не найден заполненный буфер фона по размерам, программная отрисовка
		{
			PACKEDCOLORPIP_T c1, c2;
			c1 = bh->state == DISABLED ? COLOR_BUTTON_DISABLED : (bh->is_locked ? COLOR_BUTTON_LOCKED : COLOR_BUTTON_NON_LOCKED);
			c2 = bh->state == DISABLED ? COLOR_BUTTON_DISABLED : (bh->is_locked ? COLOR_BUTTON_PR_LOCKED : COLOR_BUTTON_PR_NON_LOCKED);
			colpip_rect(colmain_fb_draw(), DIM_X, DIM_Y, bh->x1, bh->y1, bh->x1 + bh->w, bh->y1 + bh->h - 2, bh->state == PRESSED ? c2 : c1, 1);
			colpip_rect(colmain_fb_draw(), DIM_X, DIM_Y, bh->x1, bh->y1, bh->x1 + bh->w, bh->y1 + bh->h - 1, COLORPIP_GRAY, 0);
			colpip_rect(colmain_fb_draw(), DIM_X, DIM_Y, bh->x1 + 2, bh->y1 + 2, bh->x1 + bh->w - 2, bh->y1 + bh->h - 3, COLORPIP_BLACK, 0);
		}
		else
		{
			if (bh->state == DISABLED)
				bg = (PACKEDCOLORPIP_T *) b1->bg_disabled;
			else if (bh->is_locked && bh->state == PRESSED)
				bg = (PACKEDCOLORPIP_T *) b1->bg_locked_pressed;
			else if (bh->is_locked && bh->state != PRESSED)
				bg = (PACKEDCOLORPIP_T *) b1->bg_locked;
			else if (! bh->is_locked && bh->state == PRESSED)
				bg = (PACKEDCOLORPIP_T *) b1->bg_pressed;
			else if (! bh->is_locked && bh->state != PRESSED)
				bg = (PACKEDCOLORPIP_T *) b1->bg_non_pressed;

			colpip_plot(colmain_fb_draw(), DIM_X, DIM_Y, bh->x1, bh->y1, bg, bh->w, bh->h);
		}

		if (strchr(bh->text, delimeters[0]) == NULL)
		{
			/* Однострочная надпись */
			colpip_string2_tbg(colmain_fb_draw(), DIM_X, DIM_Y, bh->x1 + (bh->w - (strwidth2(bh->text))) / 2,
					bh->y1 + (bh->h - SMALLCHARH2) / 2, bh->text, COLORPIP_BLACK);
		} else
		{
			/* Двухстрочная надпись */
			uint_fast8_t j = (bh->h - SMALLCHARH2 * 2) / 2;
			char buf [TEXT_ARRAY_SIZE];
			strcpy(buf, bh->text);
			char * text2 = strtok(buf, delimeters);
			colpip_string2_tbg(colmain_fb_draw(), DIM_X, DIM_Y, bh->x1 + (bh->w - (strwidth2(text2))) / 2,
					bh->y1 + j, text2, COLORPIP_BLACK);

			text2 = strtok(NULL, delimeters);
			colpip_string2_tbg(colmain_fb_draw(), DIM_X, DIM_Y, bh->x1 + (bh->w - (strwidth2(text2))) / 2,
					bh->h + bh->y1 - SMALLCHARH2 - j, text2, COLORPIP_BLACK);
		}
	}

	void fill_button_bg_buf(btn_bg_t * v)
	{
		PACKEDCOLORPIP_T * buf;
		uint_fast8_t w, h;

		w = v->w;
		h = v->h;
		ASSERT(w < BTN_BUF_W);
		ASSERT(h < BTN_BUF_H);

		buf = * v->bg_non_pressed;
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_NON_LOCKED, 1);
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_rect(buf, w, h, 2, 2, w - 3, h - 3, COLORPIP_BLACK, 0);

		buf = * v->bg_pressed;
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_PR_NON_LOCKED, 1);
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_rect(buf, w, h, 2, 2, w - 3, h - 3, COLORPIP_BLACK, 0);

		buf = * v->bg_locked;
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_LOCKED, 1);
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_rect(buf, w, h, 2, 2, w - 3, h - 3, COLORPIP_BLACK, 0);

		buf = * v->bg_locked_pressed;
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_PR_LOCKED, 1);
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_rect(buf, w, h, 2, 2, w - 3, h - 3, COLORPIP_BLACK, 0);

		buf = * v->bg_disabled;
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLOR_BUTTON_DISABLED, 1);
		colpip_rect(buf, w, h, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_rect(buf, w, h, 2, 2, w - 3, h - 3, COLORPIP_BLACK, 0);
	}

	void gui_initialize (void)
	{
		uint_fast8_t i = 1, x = 0;

		do {
			touch_elements[touch_count].id = i;
			touch_elements[touch_count].type = TYPE_BUTTON;
			touch_count++;
			button_handlers[i].x1 = x;
			button_handlers[i].y1 = DIM_Y - button_handlers[i].h;
			x = x + button_handlers[i].w + 3;
		} while (button_handlers[++i].parent == FOOTER);

		i = 0;
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
				const button_t * const bh = & button_handlers[p->id];
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
				const label_t * const lh = & labels[p->id];
				p->x1 = lh->x;
				p->x2 = lh->x + strwidth(lh->text);
				p->y1 = lh->y - SMALLCHARH;
				p->y2 = lh->y + SMALLCHARH * 2;
				p->state = lh->state;
				p->visible = lh->visible;
				p->is_trackable = lh->is_trackable;
			}
		}
	}

	static void set_state_record(touch_t * val)
	{
		ASSERT(val != NULL);
		gui.selected_id = val->id;								// добавить везде проверку на gui.selected_type
		switch (val->type)
		{
			case TYPE_BUTTON:
				ASSERT(val->id < BUTTON_HANDLERS_COUNT);
				gui.selected_type = TYPE_BUTTON;
				button_handlers[val->id].state = val->state;
				if (button_handlers[val->id].onClickHandler && button_handlers[val->id].state == RELEASED)
					button_handlers[val->id].onClickHandler();
				break;

			case TYPE_LABEL:
				ASSERT(val->id < LABELS_COUNT);
				gui.selected_type = TYPE_LABEL;
				labels[val->id].state = val->state;
				if (labels[val->id].onClickHandler && labels[val->id].state == RELEASED)
					labels[val->id].onClickHandler();
				break;
			default:
				ASSERT(0);
				break;
		}
	}

	void process_gui(void)
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
			debug_printf_P(PSTR("last x/y=%d/%d\n"), gui.last_pressed_x, gui.last_pressed_y);
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
			gui.vector_move_x = 0;
			gui.vector_move_y = 0;
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
				p->state = PRESSED;
				set_state_record(p);
				if (gui.vector_move_x != 0 || gui.vector_move_y != 0)
				{
					gui.is_tracking = 1;
//					debug_printf_P(PSTR("move x: %d, move y: %d\n"), gui.vector_move_x, gui.vector_move_y);
				}
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

	void display_pip_update(uint_fast8_t x, uint_fast8_t y, dctx_t * pctx)
	{
		uint_fast16_t yt, xt;
		uint_fast8_t alpha = DEFAULT_ALPHA; // на сколько затемнять цвета
		char buf [TEXT_ARRAY_SIZE];
		char * text2 = NULL;
		uint_fast8_t str_len = 0;

		process_gui();

		// вывод на PIP служебной информации
	#if WITHTHERMOLEVEL	// температура выходных транзисторов (при передаче)
		if (hamradio_get_tx())
		{
			ldiv_t t = ldiv(hamradio_get_temperature_value(), 10);
			str_len += local_snprintf_P(&buf[str_len], sizeof buf / sizeof buf [0] - str_len, PSTR("%d.%dC "),
					t.quot, t.rem);
		}
	#endif /* WITHTHERMOLEVEL */
	#if WITHCURRLEVEL	// ток PA (при передаче)
		if (hamradio_get_tx())
		{
			int_fast16_t drain = hamradio_get_pacurrent_value();
			if (drain < 0) drain = 0;
			ldiv_t t = ldiv(drain, 100);
			str_len += local_snprintf_P(&buf[str_len], sizeof buf / sizeof buf [0] - str_len, PSTR("%d.%02dA "),
					t.quot, t.rem);
		}
	#endif /* WITHCURRLEVEL */
	#if WITHVOLTLEVEL	// напряжение питания
		ldiv_t t = ldiv(hamradio_get_volt_value(), 10);
		str_len += local_snprintf_P(&buf[str_len], sizeof buf / sizeof buf [0] - str_len,
									PSTR("%d.%1dV "), t.quot, t.rem);
	#endif /* WITHVOLTLEVEL */
	#if WITHIF4DSP						// ширина панорамы
		str_len += local_snprintf_P(&buf[str_len], sizeof buf / sizeof buf [0] - str_len, PSTR("SPAN:%3dk"),
				(int) ((display_zoomedbw() + 0) / 1000));
	#endif /* WITHIF4DSP */
		if (str_len > 0)
		{
			xt = DIM_X - SMALLCHARW2 - str_len * SMALLCHARW2;
			display_transparency(xt - 5, 405, DIM_X - 5, 428, alpha);
			colpip_string2_tbg(colmain_fb_draw(), DIM_X, DIM_Y, xt, 410, buf, COLORPIP_YELLOW);
		}

	#if defined (RTC1_TYPE)				// текущее время
		uint_fast16_t year;
		uint_fast8_t month, day, hour, minute, secounds;
		str_len = 0;
		board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & secounds);
		str_len += local_snprintf_P(&buf[str_len], sizeof buf / sizeof buf [0] - str_len,
				PSTR("%02d.%02d.%04d %02d%c%02d"), day, month, year, hour, ((secounds & 1) ? ' ' : ':'), minute);
		display_transparency(5, 405, str_len * SMALLCHARW2 + 15, 428, alpha);
		colpip_string2_tbg(colmain_fb_draw(), DIM_X, DIM_Y, 10, 410, buf, COLORPIP_YELLOW);
	#endif 	/* defined (RTC1_TYPE) */

		if (windows[gui.window_to_draw].is_show)
		{
			// при открытии окна рассчитываются экранные координаты самого окна и его child элементов
			if (windows[gui.window_to_draw].first_call == 0)
			{
				display_transparency(
						windows[gui.window_to_draw].x1, windows[gui.window_to_draw].y1,
						windows[gui.window_to_draw].x1 + windows[gui.window_to_draw].w,
						windows[gui.window_to_draw].y1 + windows[gui.window_to_draw].h, alpha);
			}

			// запуск процедуры фоновой обработки для окна
			windows[gui.window_to_draw].onVisibleProcess();

			if (windows[gui.window_to_draw].first_call == 0)
			{
				// вывод заголовка окна
				colpip_string_tbg(colmain_fb_draw(), DIM_X, DIM_Y, windows[gui.window_to_draw].x1 + 20,
						windows[gui.window_to_draw].y1 + 10, windows[gui.window_to_draw].name, COLORPIP_YELLOW);

				// отрисовка принадлежащих окну элементов

				// метки
				for (uint_fast8_t i = 1; i < LABELS_COUNT; i++)
				{
					const label_t * const lh = & labels[i];
					if (lh->parent == gui.window_to_draw && lh->visible == VISIBLE)
						colpip_string_tbg(colmain_fb_draw(), DIM_X, DIM_Y, lh->x, lh->y, lh->text, lh->color);
				}

				// кнопки
				for (uint_fast8_t i = 1; i < BUTTON_HANDLERS_COUNT; i++)
				{
					const button_t * const bh = & button_handlers[i];
					if (bh->parent == gui.window_to_draw && bh->visible == VISIBLE)
						draw_button_pip(i);
				}
			}
		}

		// кнопки in FOOTER
		for (uint_fast8_t i = 1; i < BUTTON_HANDLERS_COUNT; i++)
		{
			const button_t * const bhf = & button_handlers[i];
			if (bhf->parent != FOOTER)
				break;
			draw_button_pip(i);
		}
	}
#endif /* LCDMODE_LTDC && WITHTOUCHGUI */
