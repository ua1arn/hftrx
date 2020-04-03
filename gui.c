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

#if LCDMODE_LTDC

#if WITHTOUCHGUI
	#include "keyboard.h"
	#include "list.h"

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
	void * pv
	)
{
	const int xc = GRID2X(xgrid);
	const int yc = GRID2Y(ygrid);

	enum { halfsect = 30 };
	enum { gm = 270 };
	enum { gs = gm - halfsect };
	const int ge = gm + halfsect;
	const int stripewidth = 12; //16;
	const int r1 = 7 * GRID2Y(3) - 8;	//350;
	const int r2 = r1 - stripewidth;

	const uint_fast8_t is_tx = hamradio_get_tx();

	int gv, gv_trace, gswr;
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

	if (is_tx)																	// шкала при передаче
	{
		unsigned p;
		unsigned i;
		for (p = 0, i = 0; i < sizeof markersTX_pwr / sizeof markersTX_pwr [0]; ++ i, p += 10)
		{
			char buf [10];
			uint_fast16_t xx, yy;
			display_radius(xc, yc, markersTX_pwr [i], r1, r1 + 8, smeter);
			polar_to_dek(xc, yc, markersTX_pwr [i], r1 + 8, & xx, & yy);
			local_snprintf_P(buf, sizeof buf / sizeof buf [0], PSTR("%u"), p);
			display_string3_at_xy(xx - strwidth3(buf) / 2, yy - pad2w3, buf, COLORMAIN_YELLOW, COLORMAIN_BLACK);
		}

		for (p = 1, i = 0; i < sizeof markersTX_swr / sizeof markersTX_swr [0]; ++ i, p += 1)
		{
			char buf [10];
			uint_fast16_t xx, yy;
			display_radius(xc, yc, markersTX_swr [i], r2, r2 - 8, smeter);
			polar_to_dek(xc, yc, markersTX_swr [i], r2 - 16, & xx, & yy);
			local_snprintf_P(buf, sizeof buf / sizeof buf [0], PSTR("%u"), p);
			p++;
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
			floodFill_framebuffer(xx, yy, COLORMAIN_YELLOW, COLORMAIN_BLACK);
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

	(void) pv;
}

void button1_handler(void);
	void button2_handler(void);
	void button3_handler(void);
	void button4_handler(void);
	void button5_handler(void);
	void button6_handler(void);
	void button7_handler(void);
	void button8_handler(void);
	void button9_handler(void);
	void labels_menu_handler (void);
	void buttons_mode_handler(void);
	void buttons_bp_handler(void);
	void buttons_freq_handler(void);
	void buttons_menu_handler(void);
	void buttons_uif_handler(void);
	void window_bp_process(void);
	void window_menu_process(void);
	void window_freq_process(void);
	void window_uif_process(void);
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
		LIST_ARRAY_SIZE = 40
	};

	typedef struct {
		uint16_t x1;				// координаты от начала PIP
		uint16_t y1;
		uint16_t x2;
		uint16_t y2;
		void(*onClickHandler) (void);	// обработчик события RELEASED
		uint8_t state;				// текущее состояние кнопки
		uint8_t is_locked;			// признак фиксации кнопки
		uint8_t is_trackable;		// получение относительных координат точки перемещения нажатия, нужно ли для кнопок?
		uint8_t parent;			// индекс окна, в котором будет отображаться кнопка
		uint8_t visible;			// рисовать ли кнопку на экране
		uintptr_t payload;
		char name [NAME_ARRAY_SIZE];
		char text [TEXT_ARRAY_SIZE];					// текст внутри кнопки, разделитель строк |, не более 2х строк
	} button_t;

	static button_t button_handlers [] = {
	//   x1,   y1,  x2,  y2,  onClickHandler,         state,   is_locked, is_trackable,	parent,    visible,      payload,	 name, 		text
		{ },
		{   0, 260,  86, 304, button1_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "btnMode", "Mode", },
		{  89, 260, 175, 304, button2_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "btnAF", "AF|filter", },
		{ 178, 260, 264, 304, button3_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "btnAGC", "AGC", },
		{ 267, 260, 353, 304, button4_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "btn4", "Freq", },
		{ 356, 260, 442, 304, button5_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "btn5", "", },
		{ 445, 260, 531, 304, button6_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "btn6", "", },
		{ 534, 260, 620, 304, button7_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "btn7", "", },
		{ 623, 260, 709, 304, button8_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "btn8", "", },
		{ 712, 260, 798, 304, button9_handler, 	    CANCELLED, BUTTON_NON_LOCKED, 0, FOOTER, 	   VISIBLE,     UINTPTR_MAX, "btnSysMenu", "System|settings", },
		{ 234,  55, 314, 105, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_LSB, "btnModeLSB", "LSB", },
		{ 319,  55, 399, 105, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_CW,  "btnModeCW", "CW", },
		{ 404,  55, 484, 105, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_AM,  "btnModeAM", "AM", },
		{ 489,  55, 569, 105, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_DGL, "btnModeDGL", "DGL", },
		{ 234, 110, 314, 160, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_USB, "btnModeUSB", "USB", },
		{ 319, 110, 399, 160, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_CWR, "btnModeCWR", "CWR", },
		{ 404, 110, 484, 160, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_NFM, "btnModeNFM", "NFM", },
		{ 489, 110, 569, 160, buttons_mode_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MODES, NON_VISIBLE, SUBMODE_DGU, "btnModeDGU", "DGU", },
		{ 251, 155, 337, 195, buttons_bp_handler,	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP,    NON_VISIBLE, UINTPTR_MAX, "btnAF_1", "", },
		{ 357, 155, 443, 195, buttons_bp_handler, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP,    NON_VISIBLE, UINTPTR_MAX, "btnAF_OK", "OK", },
		{ 463, 155, 549, 195, buttons_bp_handler, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_BP,    NON_VISIBLE, UINTPTR_MAX, "btnAF_2", "", },
		{ 251,  70, 337, 110, set_agc_off, 			CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, "btnAGCoff", "AGC|off", },
		{ 357,  70, 443, 110, set_agc_slow, 		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, "btnAGCslow", "AGC|slow", },
		{ 463,  70, 549, 110, set_agc_fast, 		CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_AGC,   NON_VISIBLE, UINTPTR_MAX, "btnAGCfast", "AGC|fast", },
		{ 120,  28, 170,  78, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_SPLIT, 		"btnFreq1", "1", },
		{ 173,  28, 223,  78, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_A_EQ_B, 		"btnFreq2", "2", },
		{ 226,  28, 276,  78, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_A_EX_B, 		"btnFreq3", "3", },
		{ 279,  28, 329,  78, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_BKIN, 			"btnFreqBK", "<-", },
		{ 120,  81, 170, 131, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_ATUBYPASS, 	"btnFreq4", "4", },
		{ 173,  81, 223, 131, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_ANTENNA, 		"btnFreq5", "5", },
		{ 226,  81, 276, 131, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_ATT, 			"btnFreq6", "6", },
		{ 279,  81, 329, 131, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_ENTERFREQDONE, "btnFreqOK", "OK", },
		{ 120, 134, 170, 184, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_MOX, 			"btnFreq7", "7", },
		{ 173, 134, 223, 184, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_RECORDTOGGLE, 	"btnFreq8", "8", },
		{ 226, 134, 276, 184, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_LDSPTGL, 		"btnFreq9", "9", },
		{ 279, 134, 329, 184, buttons_freq_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_FREQ,  NON_VISIBLE, KBD_CODE_VOXTOGGLE, 	"btnFreq0", "0", },
		{   0, 	 0,   0,   0, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU,  NON_VISIBLE, UINTPTR_MAX, 			"btnSysMenu-", "-", },
		{   0, 	 0,   0,   0, buttons_menu_handler, CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_MENU,  NON_VISIBLE, UINTPTR_MAX, 			"btnSysMenu+", "+", },
		{   0, 	 0,   0,   0, buttons_uif_handler, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_UIF,   NON_VISIBLE, UINTPTR_MAX, 			"btnUIF-", "-", },
		{   0, 	 0,   0,   0, buttons_uif_handler, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_UIF,   NON_VISIBLE, UINTPTR_MAX, 			"btnUIF+", "+", },
		{ 375, 120, 425, 150, buttons_uif_handler, 	CANCELLED, BUTTON_NON_LOCKED, 0, WINDOW_UIF,   NON_VISIBLE, UINTPTR_MAX, 			"btnUIF_OK", "OK", },
	};
	enum { button_handlers_count = sizeof button_handlers / sizeof button_handlers[0] };

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
		{ 250, 120, WINDOW_BP,   DISABLED,  0, NON_VISIBLE, "lbl_low",  "", COLORPIP_YELLOW, },
		{ 490, 120, WINDOW_BP,   DISABLED,  0, NON_VISIBLE, "lbl_high", "", COLORPIP_YELLOW, },
		{ 100,  50, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 100,  85, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 100, 120, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 100, 155, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 100, 190, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_group", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 250,  50, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 250,  85, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 250, 120, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 250, 155, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 250, 190, WINDOW_MENU, CANCELLED, 1, NON_VISIBLE, "lbl_params", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 450,  50, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 450,  85, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 450, 120, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 450, 155, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 450, 190, WINDOW_MENU, CANCELLED, 0, NON_VISIBLE, "lbl_vals", "", COLORPIP_WHITE, labels_menu_handler, },
		{ 580,  60, WINDOW_ENC2, DISABLED,  0, NON_VISIBLE, "lbl_enc2_param", "", 	COLORPIP_WHITE, },
		{ 580,  90, WINDOW_ENC2, DISABLED,  0, NON_VISIBLE, "lbl_enc2_val", "", 	COLORPIP_WHITE, },
		{    0,  0,	WINDOW_UIF,  DISABLED,  0, NON_VISIBLE, "lbl_uif_param", "", COLORPIP_WHITE, },
		{    0,  0,	WINDOW_UIF,  DISABLED,  0, NON_VISIBLE, "lbl_uif_val", 	 "", COLORPIP_WHITE, },
};
	enum { labels_count = sizeof labels / sizeof labels[0] };

	typedef struct {
		uint16_t last_pressed_x; 	 // последняя точка касания экрана
		uint16_t last_pressed_y;
		uint8_t kbd_code;
		uint8_t selected_id;		 // индекс последнего выбранного элемента
		uint8_t selected_type;		 // тип последнего выбранного элемента
		uint8_t state;				 // последнее состояние
		uint8_t is_touching_screen; // есть ли касание экрана в данный момент
		uint8_t is_after_touch; 	 // есть ли касание экрана после выхода точки касания из элемента (при is_tracking == 0)
		uint8_t fix;				 // первые координаты после нажатия от контролера тачскрина приходят старые, пропускаем
		uint8_t window_to_draw;	 // индекс записи с описанием запрошенного к отображению окна
		uint16_t pip_width;		 // параметры pip
		uint16_t pip_height;
		uint16_t pip_x;
		uint16_t pip_y;
		uint8_t is_tracking;		 // получение относительных координат точки перемещения нажатия
		int16_t vector_move_x;	 // в т.ч. и за границами элемента, при state == PRESSED
		int16_t vector_move_y;
	} gui_t;

	static gui_t gui = { 0, 0, KBD_CODE_MAX, 0, TYPE_DUMMY, CANCELLED, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, };

	typedef struct {
		uint8_t window_id;			// в окне будут отображаться элементы с соответствующим полем for_window
		uint16_t x1;
		uint16_t y1;
		uint16_t x2;
		uint16_t y2;
		char name[NAME_ARRAY_SIZE];		// текст, выводимый в заголовке окна
		uint8_t is_show;			// запрос на отрисовку окна
		uint8_t first_call;		// признак первого вызова для различных инициализаций
		void (*onVisibleProcess) (void);
	} windowpip_t;

	static windowpip_t windows[] = {
	//     window_id,   x1,  y1, x2,  y2,  title,         is_show, first_call, onVisibleProcess
		{ },
		{ WINDOW_MODES, 214, 20, 586, 175, "Select mode", NON_VISIBLE, 0, },
		{ WINDOW_BP,    214, 20, 586, 225, "Bandpass",    NON_VISIBLE, 0, window_bp_process, },
		{ WINDOW_AGC,   214, 20, 586, 140, "AGC control", NON_VISIBLE, 0, },
		{ WINDOW_FREQ,  100,  0, 350, 200, "Freq", 		  NON_VISIBLE, 0, window_freq_process, },
		{ WINDOW_MENU,   50, 10, 600, 220, "Settings",	  NON_VISIBLE, 0, window_menu_process, },
		{ WINDOW_ENC2, 	550, 15, 735, 120, "Fast menu",   NON_VISIBLE, 0, },
		{ WINDOW_UIF, 	300, 15, 500, 160, "",   		  NON_VISIBLE, 0, window_uif_process, },
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
		uint8_t add_id;			// номер строки уровня, отображаемой первой
		menu_names_t menu_block [MENU_ARRAY_SIZE];	// массив значений уровня меню
	} menu_t;

	menu_t menu[MENU_COUNT];

	typedef struct {
		char name [TEXT_ARRAY_SIZE];
		uint8_t menupos;
		uint8_t exitkey;
	} menu_by_name_t;

	menu_by_name_t menu_uif;

	typedef struct {
		uint_fast16_t type;				// тип элемента, поддерживающего реакцию на касания
		uint8_t state;				// текущее состояние элемента
		uint8_t visible;			// текущая видимость элемента
		uint8_t id;				// номер элемента из структуры описания
		uint8_t is_trackable;		// поддерживает ли элемент возврат относительных координат перемещения точки нажатия
		uint16_t x1;
		uint16_t y1;
		uint16_t x2;
		uint16_t y2;
		LIST_ENTRY item;
	} list_template_t;

	LIST_ENTRY touch_list;
	list_template_t touch_elements[LIST_ARRAY_SIZE];
	uint_fast8_t touch_list_count = 0;
	uint_fast8_t menu_label_touched = 0;
	uint_fast8_t menu_level;

	void gui_initialize (void)
	{
		pipparams_t pip;
		uint_fast8_t i = 1;
		InitializeListHead(& touch_list);
		do {
			touch_elements[touch_list_count].id = i;
			touch_elements[touch_list_count].type = TYPE_BUTTON;
			InsertHeadList(& touch_list, & touch_elements[touch_list_count].item);
			touch_list_count++;
		} while (button_handlers[++i].parent == FOOTER);
		display2_getpipparams(& pip);
		gui.pip_width = pip.w;
		gui.pip_height = pip.h;
		gui.pip_x = pip.x;
		gui.pip_y = pip.y;
	}

	void update_touch_list(void)
	{
		PLIST_ENTRY t;

		for (t = touch_list.Blink; t != & touch_list; t = t->Blink)
		{
			list_template_t * p = CONTAINING_RECORD(t, list_template_t, item);
			if (p->type == TYPE_BUTTON)
			{
				p->x1 = button_handlers[p->id].x1;
				p->x2 = button_handlers[p->id].x2;
				p->y1 = button_handlers[p->id].y1;
				p->y2 = button_handlers[p->id].y2;
				p->state = button_handlers[p->id].state;
				p->visible = button_handlers[p->id].visible;
				p->is_trackable = button_handlers[p->id].is_trackable;
			}
			else if (p->type == TYPE_LABEL)
			{
				p->x1 = labels[p->id].x;
				p->x2 = labels[p->id].x + strwidth(labels[p->id].text);
				p->y1 = labels[p->id].y - 8;
				p->y2 = labels[p->id].y + strwidth(labels[p->id].text) + 8;
				p->state = labels[p->id].state;
				p->visible = labels[p->id].visible;
				p->is_trackable = labels[p->id].is_trackable;
			}
		}
	}

	uint_fast8_t find_button (uint_fast8_t id_window, const char * name)				// возврат id кнопки окна по ее названию
	{
		for (uint_fast8_t i = 1; i < button_handlers_count; i++)
		{
			if (button_handlers[i].parent == id_window && strcmp(button_handlers[i].name, name) == 0)
				return i;
		}
		return 0;
	}

	uint_fast8_t find_label (uint_fast8_t id_window, const char * name)				// возврат id метки окна по ее названию
	{
		for (uint_fast8_t i = 1; i < labels_count; i++)
		{
			if (labels[i].parent == id_window && strcmp(labels[i].name, name) == 0)
				return i;
		}
		return 0;
	}

	void footer_buttons_state (uint_fast8_t state, const char * name)					// блокируются все, кроме name == text
	{
		static uint_fast8_t id = 0;
		if (state == DISABLED)
		{
			id = find_button(FOOTER, name);
			button_handlers[id].is_locked = BUTTON_LOCKED;
		} else
			button_handlers[id].is_locked = BUTTON_NON_LOCKED;

		for (uint_fast8_t i = 1; i < button_handlers_count; i++)
		{
			if (button_handlers[i].parent != FOOTER)
				break;
			button_handlers[i].state = button_handlers[i].text == name ? DISABLED : state;
		}
	}

	void set_window(uint_fast8_t parent, uint_fast8_t value)
	{
		PLIST_ENTRY p;
		for (uint_fast8_t i = 1; i < button_handlers_count; i++)
		{
			if (button_handlers[i].parent == parent)
			{
				button_handlers[i].visible = value ? VISIBLE : NON_VISIBLE;
				button_handlers[i].is_locked = 0;
				if (button_handlers[i].visible)
				{
					touch_elements[touch_list_count].id = i;
					touch_elements[touch_list_count].type = TYPE_BUTTON;
					InsertHeadList(& touch_list, & touch_elements[touch_list_count].item);
					touch_list_count++;
				}
				else
				{
					p = RemoveHeadList(& touch_list);
					touch_list_count--;
				}
			}
		}
		for (uint_fast8_t i = 1; i < labels_count; i++)
		{
			if (labels[i].parent == parent)
			{
				labels[i].visible = value ? VISIBLE : NON_VISIBLE;
				if (labels[i].visible && labels[i].onClickHandler)
				{
					touch_elements[touch_list_count].id = i;
					touch_elements[touch_list_count].type = TYPE_LABEL;
					InsertHeadList(& touch_list, & touch_elements[touch_list_count].item);
					touch_list_count++;
				}
				if(! labels[i].visible && labels[i].onClickHandler)
				{
					p = RemoveHeadList(& touch_list);
					touch_list_count--;
				}
			}
		}
		windows[parent].is_show = value ? VISIBLE : NON_VISIBLE;
		gui.window_to_draw = value ? parent : 0;
		(void) p;
	}

	void window_bp_process (void)
	{
		PACKEDCOLORPIP_T * const colorpip = getscratchpip();
		static uint_fast8_t val_high, val_low, val_c, val_w, bw_type;
		static uint_fast16_t x_h, x_l, x_c;
		char buf[TEXT_ARRAY_SIZE];
		static uint_fast8_t id_button_high, id_button_low, id_button_width, id_button_pitch, id_lbl_high, id_lbl_low;

		if (windows[WINDOW_BP].first_call == 1)
		{
			windows[WINDOW_BP].first_call = 0;

			id_lbl_low = find_label(WINDOW_BP, "lbl_low");
			id_lbl_high = find_label(WINDOW_BP, "lbl_high");
			id_button_high = find_button(WINDOW_BP, "btnAF_2");
			id_button_low = find_button(WINDOW_BP, "btnAF_1");

			bw_type = get_bp_type();
			if (bw_type)	// BWSET_WIDE
			{
				strcpy(button_handlers[id_button_high].text, "High|cut");
				strcpy(button_handlers[id_button_low].text, "Low|cut");
				button_handlers[id_button_high].is_locked = 1;

				val_high = get_high_bp(0);
				val_low = get_low_bp(0);

				local_snprintf_P(labels[id_lbl_high].text, TEXT_ARRAY_SIZE, PSTR("%d"), val_high * 100);
				x_h = normalize(val_high, 0, 50, 290) + 290;
				labels[id_lbl_high].x = x_h + 64 > 550 ? 486 : x_h;

				local_snprintf_P(labels[id_lbl_low].text, TEXT_ARRAY_SIZE, PSTR("%d"), val_low * 10);
				x_l = normalize(val_low, 0, 500, 290) + 290;
				labels[id_lbl_low].x = x_l - strwidth(buf);
			}
			else			// BWSET_NARROW
			{
				strcpy(button_handlers[id_button_high].text, "Pitch");
				strcpy(button_handlers[id_button_low].text, "Width");
				button_handlers[id_button_low].is_locked = 1;

				val_c = get_high_bp(0);
				x_c = 400; //normalize(val_c, 0, 500, 290) + 290;
				val_w = get_low_bp(0) / 2;
				x_l = normalize(190 - val_w , 0, 500, 290) + 290;
				x_h = normalize(190 + val_w , 0, 500, 290) + 290;

				local_snprintf_P(labels[id_lbl_high].text, TEXT_ARRAY_SIZE, PSTR("%d"), val_w * 20);

				local_snprintf_P(labels[id_lbl_low].text, sizeof buf / sizeof buf[0], PSTR("P %d"), val_c * 10);
				labels[id_lbl_low].x = 550 - strwidth(labels[id_lbl_low].text);
			}
		}

		if (encoder2.rotate != 0)
		{
			bw_type = get_bp_type();
			if (bw_type)	// BWSET_WIDE
			{
				if (button_handlers[id_button_high].is_locked == 1)
				{
					val_high = get_high_bp(encoder2.rotate);
					encoder2.rotate_done = 1;
					local_snprintf_P(labels[id_lbl_high].text, TEXT_ARRAY_SIZE, PSTR("%d"), val_high * 100);
					x_h = normalize(val_high, 0, 50, 290) + 290;
					labels[id_lbl_high].x = x_h + 64 > 550 ? 486 : x_h;
				}
				else if (button_handlers[id_button_low].is_locked == 1)
				{
					val_low = get_low_bp(encoder2.rotate * 10);
					encoder2.rotate_done = 1;
					local_snprintf_P(labels[id_lbl_low].text, TEXT_ARRAY_SIZE, PSTR("%d"), val_low * 10);
					x_l = normalize(val_low / 10, 0, 50, 290) + 290;
					labels[id_lbl_low].x = x_l - strwidth(labels[id_lbl_low].text);
				}
			}
			else				// BWSET_NARROW
			{
				if (button_handlers[id_button_high].is_locked == 1)
				{
					val_c = get_high_bp(encoder2.rotate);
					val_w = get_low_bp(0) / 2;
				}
				else if (button_handlers[id_button_low].is_locked == 1)
				{
					val_c = get_high_bp(0);
					val_w = get_low_bp(encoder2.rotate) / 2;
				}
				encoder2.rotate_done = 1;
				x_c = 400;
				x_l = normalize(190 - val_w , 0, 500, 290) + 290;
				x_h = normalize(190 + val_w , 0, 500, 290) + 290;

				local_snprintf_P(labels[id_lbl_high].text, TEXT_ARRAY_SIZE, PSTR("%d"), val_w * 20);
				labels[id_lbl_high].x = x_c - strwidth(labels[id_lbl_high].text) / 2;

				local_snprintf_P(labels[id_lbl_low].text, TEXT_ARRAY_SIZE, PSTR("P %d"), val_c * 10);
				labels[id_lbl_low].x = 550 - strwidth(labels[id_lbl_low].text);
			}
		}

		colpip_line(colorpip, gui.pip_width, gui.pip_height, 251, 110, 549, 110, COLORPIP_GRAY);
		colpip_line(colorpip, gui.pip_width, gui.pip_height, 290, 70, 290, 120, COLORPIP_GRAY);
		colpip_rect(colorpip, gui.pip_width, gui.pip_height, x_l, 70, x_h, 108, COLORPIP_YELLOW, 1);
		if (! bw_type)
			colpip_line(colorpip, gui.pip_width, gui.pip_height, x_c, 60, x_c, 120, COLORPIP_RED);
}

	void window_freq_process (void)
	{
		if (windows[WINDOW_FREQ].first_call == 1)
		{
			windows[WINDOW_FREQ].first_call = 0;
			button_handlers[find_button(WINDOW_FREQ, "btnFreqOK")].is_locked = BUTTON_LOCKED;
		}
	}

	void labels_menu_handler (void)
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

	void gui_uif_editmenu(const char * name, uint_fast8_t menupos, uint_fast8_t exitkey)
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

	void window_uif_process(void)
	{
		static uint_fast8_t id_lbl_uif_param, id_lbl_uif_val, window_half_wight;
		static uint_fast8_t id_button_up = 0, id_button_down = 0;
		uint_fast8_t button_menu_w = 40, button_menu_h = 40;

		if (windows[WINDOW_UIF].first_call == 1)
		{
			windows[WINDOW_UIF].first_call = 0;
			window_half_wight =  windows[WINDOW_UIF].x1 + ((windows[WINDOW_UIF].x2 - windows[WINDOW_UIF].x1) / 2);

			id_lbl_uif_param = find_label(WINDOW_UIF, "lbl_uif_param");
			id_lbl_uif_val = find_label(WINDOW_UIF, "lbl_uif_val");
			strcpy(labels[id_lbl_uif_param].text, menu_uif.name);
			const char * v = gui_edit_menu_item(menu_uif.menupos, 0);
			strcpy(labels[id_lbl_uif_val].text, v);

			labels[id_lbl_uif_param].x = window_half_wight - (strwidth(labels[id_lbl_uif_param].text) / 2);
			labels[id_lbl_uif_param].y = windows[WINDOW_UIF].y1 + SMALLCHARH;
			labels[id_lbl_uif_val].x = window_half_wight - (strwidth(labels[id_lbl_uif_val].text) / 2);
			labels[id_lbl_uif_val].y = windows[WINDOW_UIF].y1 + SMALLCHARH * 4;

			id_button_up = find_button(WINDOW_UIF, "btnUIF+");
			id_button_down = find_button(WINDOW_UIF, "btnUIF-");

			button_handlers[id_button_down].x1 = labels[id_lbl_uif_val].x - button_menu_w - 10;
			button_handlers[id_button_down].x2 = button_handlers[id_button_down].x1 + button_menu_w;
			button_handlers[id_button_down].y1 = (labels[id_lbl_uif_val].y + SMALLCHARH / 2) - (button_menu_h / 2);
			button_handlers[id_button_down].y2 = button_handlers[id_button_down].y1 + button_menu_h;

			button_handlers[id_button_up].x1 = labels[id_lbl_uif_val].x + strwidth(labels[id_lbl_uif_val].text) + 10;
			button_handlers[id_button_up].x2 = button_handlers[id_button_up].x1 + button_menu_w;
			button_handlers[id_button_up].y1 = button_handlers[id_button_down].y1;
			button_handlers[id_button_up].y2 = button_handlers[id_button_down].y2;

			enable_keyboard_redirect();
		}

		if (encoder2.rotate != 0)
		{
			const char * v = gui_edit_menu_item(menu_uif.menupos, encoder2.rotate);
			strcpy(labels[id_lbl_uif_val].text, v);
			labels[id_lbl_uif_val].x = window_half_wight - (strwidth(labels[id_lbl_uif_val].text) / 2);
			encoder2.rotate_done = 1;
			button_handlers[id_button_down].x1 = labels[id_lbl_uif_val].x - button_menu_w - 10;
			button_handlers[id_button_down].x2 = button_handlers[id_button_down].x1 + button_menu_w;
			button_handlers[id_button_up].x1 = labels[id_lbl_uif_val].x + strwidth(labels[id_lbl_uif_val].text) + 10;
			button_handlers[id_button_up].x2 = button_handlers[id_button_up].x1 + button_menu_w;

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

	void buttons_uif_handler(void)
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

	void window_menu_process(void)
	{
		PACKEDCOLORPIP_T * const colorpip = getscratchpip();
		static uint_fast8_t str_step = 0, menu_is_scrolling = 0, start_str_group = 0, start_str_params = 0;
		static uint_fast8_t id_button_up = 0, id_button_down = 0, button_pressed = 0;
		uint_fast8_t button_menu_w = 40, button_menu_h = 40;

		if (windows[WINDOW_MENU].first_call == 1)
		{
			windows[WINDOW_MENU].first_call = 0;
			set_menu_cond(VISIBLE);

			id_button_up = find_button(WINDOW_MENU, "btnSysMenu+");
			id_button_down = find_button(WINDOW_MENU, "btnSysMenu-");
			button_handlers[id_button_up].visible = NON_VISIBLE;
			button_handlers[id_button_down].visible = NON_VISIBLE;

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
			while (strcmp(labels[++menu[MENU_PARAMS].last_id].name, "lbl_params") == 0);
			menu[MENU_PARAMS].last_id--;													// последнее вхождение метки params
			menu[MENU_PARAMS].num_rows = menu[MENU_PARAMS].last_id - menu[MENU_PARAMS].first_id;

			menu[MENU_VALS].first_id = menu[MENU_PARAMS].last_id + 1;						// первое вхождение метки vals
			menu[MENU_VALS].last_id = menu[MENU_VALS].first_id;
			while (strcmp(labels[++menu[MENU_VALS].last_id].name, "lbl_vals") == 0);
			menu[MENU_VALS].last_id--;														// последнее вхождение метки vals
			menu[MENU_VALS].num_rows = menu[MENU_VALS].last_id - menu[MENU_VALS].first_id;

			menu[MENU_GROUPS].count = get_multilinemenu_block_groups(menu[MENU_GROUPS].menu_block) - 1;
			for(uint_fast8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
			{
				strcpy(labels[menu[MENU_GROUPS].first_id + i].text, menu[MENU_GROUPS].menu_block[i + menu[MENU_GROUPS].add_id].name);
				labels[menu[MENU_GROUPS].first_id + i].visible = VISIBLE;
				labels[menu[MENU_GROUPS].first_id + i].color = COLORPIP_WHITE;
			}

			menu[MENU_PARAMS].count = get_multilinemenu_block_params(menu[MENU_PARAMS].menu_block, menu[MENU_GROUPS].menu_block[menu[MENU_GROUPS].selected_str].index) - 1;
			for(uint_fast8_t i = 0; i <= menu[MENU_PARAMS].num_rows; i++)
			{
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
			for(uint_fast8_t i = 0; i <= menu[MENU_VALS].num_rows; i++)
			{
				labels[menu[MENU_VALS].first_id + i].visible = NON_VISIBLE;
				labels[menu[MENU_VALS].first_id + i].color = COLORPIP_WHITE;
				if (menu[MENU_VALS].count < i)
					continue;
				strcpy(labels[menu[MENU_VALS].first_id + i].text, menu[MENU_VALS].menu_block[i + menu[MENU_VALS].add_id].name);
				labels[menu[MENU_VALS].first_id + i].visible = VISIBLE;
			};
			str_step = labels[menu[MENU_GROUPS].first_id + 1].y - labels[menu[MENU_GROUPS].first_id].y;
			menu_level = MENU_GROUPS;
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
				button_handlers[id_button_down].x2 = button_handlers[id_button_down].x1 + button_menu_w;
				button_handlers[id_button_down].y1 = (labels[id_sel_label].y + SMALLCHARH / 2) - (button_menu_h / 2);
				button_handlers[id_button_down].y2 = button_handlers[id_button_down].y1 + button_menu_h;

				button_handlers[id_button_up].visible = VISIBLE;
				button_handlers[id_button_up].x1 = labels[id_sel_label].x + strwidth(labels[id_sel_label].text) + 10;
				button_handlers[id_button_up].x2 = button_handlers[id_button_up].x1 + button_menu_w;
				button_handlers[id_button_up].y1 = button_handlers[id_button_down].y1;
				button_handlers[id_button_up].y2 = button_handlers[id_button_down].y2;
				for (uint8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
				{
					labels[i + menu[MENU_GROUPS].first_id].color = i == menu[MENU_GROUPS].selected_label ? COLORPIP_YELLOW : COLORPIP_DARKGRAY;
					labels[i + menu[MENU_PARAMS].first_id].color = i == menu[MENU_PARAMS].selected_label ? COLORPIP_YELLOW : COLORPIP_DARKGRAY;
					labels[i + menu[MENU_VALS].first_id].color = i == menu[MENU_PARAMS].selected_label ? COLORPIP_YELLOW : COLORPIP_DARKGRAY;
				}
				menu_label_touched = 0;
			}
			if (menu_level == MENU_PARAMS)
			{
				button_handlers[id_button_down].visible = NON_VISIBLE;
				button_handlers[id_button_up].visible = NON_VISIBLE;
				for (uint8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
				{
					labels[i + menu[MENU_GROUPS].first_id].color = i == menu[MENU_GROUPS].selected_label ? COLORPIP_YELLOW : COLORPIP_DARKGRAY;
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
			button_handlers[id_button_up].x2 = button_handlers[id_button_up].x1 + button_menu_w;
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
			colpip_string_tbg(colorpip, gui.pip_width, gui.pip_height, labels[menu[menu_level].selected_label + menu[menu_level].first_id].x - 16,
										 labels[menu[menu_level].selected_label + menu[menu_level].first_id].y, ">", COLORPIP_GREEN);
	}

	void buttons_menu_handler(void)
	{
		if (gui.selected_type == TYPE_BUTTON && gui.selected_id == find_button(WINDOW_MENU, "btnSysMenu+"))
			encoder2.rotate = 1;
		else if (gui.selected_type == TYPE_BUTTON && gui.selected_id == find_button(WINDOW_MENU, "btnSysMenu-"))
			encoder2.rotate = -1;
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

	void remove_end_line_spaces(char * str)
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
		uint_fast8_t id_lbl_param = find_label(WINDOW_ENC2, "lbl_enc2_param");
		uint_fast8_t id_lbl_val = find_label(WINDOW_ENC2, "lbl_enc2_val");
		gui.window_to_draw = WINDOW_ENC2;
		set_window(gui.window_to_draw, enc2_menu->state != 0);
		if (windows[gui.window_to_draw].is_show == VISIBLE)
		{
			footer_buttons_state(DISABLED, "");
			strcpy(labels[id_lbl_param].text, enc2_menu->param);
			remove_end_line_spaces(labels[id_lbl_param].text);
			strcpy(labels[id_lbl_val].text, enc2_menu->val);
			labels[id_lbl_val].color = enc2_menu->state == 2 ? COLORPIP_YELLOW : COLORPIP_WHITE;
			labels[id_lbl_val].x = windows[WINDOW_ENC2].x1 + ((windows[WINDOW_ENC2].x2 - windows[WINDOW_ENC2].x1) - (strwidth(labels[id_lbl_val].text))) / 2;
			labels[id_lbl_param].x = windows[WINDOW_ENC2].x1 + ((windows[WINDOW_ENC2].x2 - windows[WINDOW_ENC2].x1) - (strwidth(labels[id_lbl_param].text))) / 2;
		} else
			footer_buttons_state(CANCELLED, "");
	}

	void buttons_mode_handler(void)
	{
		if (windows[WINDOW_MODES].is_show && button_handlers[gui.selected_id].parent == WINDOW_MODES)
		{
			if (button_handlers[gui.selected_id].payload != UINTPTR_MAX)
				change_submode(button_handlers[gui.selected_id].payload);

			set_window(WINDOW_MODES, NON_VISIBLE);
			footer_buttons_state(CANCELLED, "");
		}
	}

	void buttons_bp_handler (void)
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

	void buttons_freq_handler (void)
	{
		uint_fast8_t editfreqmode = 0;
		if (gui.window_to_draw == WINDOW_FREQ)
			editfreqmode = send_key_code(button_handlers[gui.selected_id].payload);
		if (editfreqmode == 0)
		{
			set_window(WINDOW_FREQ, NON_VISIBLE);
			footer_buttons_state(CANCELLED, "");
			disable_keyboard_redirect();
		}
	}

	void button1_handler(void)
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

	void button2_handler(void)
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

	void button3_handler(void)
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

	void button4_handler(void)
	{
		gui.window_to_draw = WINDOW_FREQ;

		if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
		{
			set_window(gui.window_to_draw, VISIBLE);
			windows[gui.window_to_draw].first_call = 1;
			send_key_code(KBD_CODE_ENTERFREQ);
			enable_keyboard_redirect();
			footer_buttons_state(DISABLED, button_handlers[gui.selected_id].name);
		}
		else
		{
			set_window(gui.window_to_draw, NON_VISIBLE);
			send_key_code(KBD_CODE_ENTERFREQDONE);
			disable_keyboard_redirect();
			footer_buttons_state(CANCELLED, "");
		}
	}

	void button5_handler(void)
	{

	}

	void button6_handler(void)
	{

	}

	void button7_handler(void)
	{

	}

	void button8_handler(void)
	{

	}

	void button9_handler(void)
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

	/* Кнопка */
	void draw_button_pip(
		uint_fast16_t x1, uint_fast16_t y1,
		uint_fast16_t x2, uint_fast16_t y2,
		uint_fast8_t pressed, uint_fast8_t is_locked, uint_fast8_t is_disabled,
		const char * text) // pressed = 0
	{
		PACKEDCOLORPIP_T * const colorpip = getscratchpip();
		PACKEDCOLOR565_T c1, c2;
		static const char delimeters [] = "|";
		c1 = is_disabled ? COLOR_BUTTON_DISABLED : (is_locked ? COLOR_BUTTON_LOCKED : COLOR_BUTTON_NON_LOCKED);
		c2 = is_disabled ? COLOR_BUTTON_DISABLED : (is_locked ? COLOR_BUTTON_PR_LOCKED : COLOR_BUTTON_PR_NON_LOCKED);
		colpip_rect(colorpip, gui.pip_width, gui.pip_height, x1,	y1, x2, y2, pressed ? c1 : c2, 1);
		colpip_rect(colorpip, gui.pip_width, gui.pip_height, x1,	y1, x2, y2, COLORPIP_GRAY, 0);
		colpip_rect(colorpip, gui.pip_width, gui.pip_height, x1 + 2, y1 + 2, x2 - 2, y2 - 2, COLORPIP_BLACK, 0);

		if (strchr(text, delimeters[0]) == NULL)
		{
			/* Однострочная надпись */
			colpip_string2_tbg(colorpip, gui.pip_width, gui.pip_height,
					x1 + ((x2 - x1) - (strwidth2(text))) / 2,
					y1 + ((y2 - y1) - SMALLCHARH2) / 2,
					text, COLORPIP_BLACK);
		} else
		{
			/* Двухстрочная надпись */
			uint_fast8_t j = (y2 - y1 - SMALLCHARH2 * 2) / 2;
			char buf [TEXT_ARRAY_SIZE];
			strcpy(buf, text);
			char * text2 = strtok(buf, delimeters);
			colpip_string2_tbg(colorpip, gui.pip_width, gui.pip_height, x1 +
					((x2 - x1) - (strwidth2(text2))) / 2,
					y1 + j, text2, COLORPIP_BLACK);

			text2 = strtok(NULL, delimeters);
			colpip_string2_tbg(colorpip, gui.pip_width, gui.pip_height, x1 +
					((x2 - x1) - (strwidth2(text2))) / 2,
					y2 - SMALLCHARH2 - j, text2, COLORPIP_BLACK);
		}
	}

	void display_pip_update(uint_fast8_t x, uint_fast8_t y, void * pv)
	{
		PACKEDCOLORPIP_T * const colorpip = getscratchpip();
		uint_fast16_t yt, xt;
		uint_fast8_t alpha = DEFAULT_ALPHA; // на сколько затемнять цвета
		char buf [TEXT_ARRAY_SIZE];
		char * text2 = NULL;
		uint_fast8_t str_len = 0;

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
			xt = gui.pip_width - SMALLCHARW2 - str_len * SMALLCHARW2;
			colpip_transparency(colorpip, gui.pip_width, gui.pip_height, xt - 5, 230, gui.pip_width - 5, 253, alpha);
			colpip_string2_tbg(colorpip, gui.pip_width, gui.pip_height, xt, 235, buf, COLORPIP_YELLOW);
		}

	#if defined (RTC1_TYPE)				// текущее время
		uint_fast16_t year;
		uint_fast8_t month, day, hour, minute, secounds;
		str_len = 0;
		board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & secounds);
		str_len += local_snprintf_P(&buf[str_len], sizeof buf / sizeof buf [0] - str_len,
				PSTR("%02d.%02d.%04d %02d%c%02d"), day, month, year, hour, ((secounds & 1) ? ' ' : ':'), minute);
		colpip_transparency(colorpip, gui.pip_width, gui.pip_height, 5, 230, str_len * SMALLCHARW2 + 15, 253, alpha);
		colpip_string2_tbg(colorpip, gui.pip_width, gui.pip_height, 10, 235, buf, COLORPIP_YELLOW);
	#endif 	/* defined (RTC1_TYPE) */

		if (windows[gui.window_to_draw].is_show)
		{
			colpip_transparency(colorpip, gui.pip_width, gui.pip_height,
					windows[gui.window_to_draw].x1, windows[gui.window_to_draw].y1,
					windows[gui.window_to_draw].x2, windows[gui.window_to_draw].y2, alpha);

			if (windows[gui.window_to_draw].onVisibleProcess)							// запуск процедуры фоновой обработки
				windows[gui.window_to_draw].onVisibleProcess();							// для окна, если есть

			// вывод заголовка окна
			colpip_string_tbg(colorpip, gui.pip_width, gui.pip_height, windows[gui.window_to_draw].x1 + 20,
					windows[gui.window_to_draw].y1 + 10, windows[gui.window_to_draw].name, COLORPIP_YELLOW);

			// отрисовка принадлежащих окну элементов

			// метки
			for (uint_fast8_t i = 1; i < labels_count; i++)
			{
				const label_t * const lh = & labels[i];
				if (lh->parent == gui.window_to_draw && lh->visible == VISIBLE)
					colpip_string_tbg(colorpip, gui.pip_width, gui.pip_height, lh->x, lh->y, lh->text, lh->color);
			}

			// кнопки
			for (uint_fast8_t i = 1; i < button_handlers_count; i++)
			{
				const button_t * const bh = & button_handlers[i];
				if (bh->parent == gui.window_to_draw && bh->visible == VISIBLE)
					draw_button_pip(bh->x1, bh->y1, bh->x2, bh->y2, bh->state, bh->is_locked, bh->state == DISABLED ? 1 : 0, bh->text);
			}
		}

		// кнопки in FOOTER
		for (uint_fast8_t i = 1; i < button_handlers_count; i++)
		{
			const button_t * const bhf = & button_handlers[i];
			if (bhf->parent != FOOTER)
				break;
			draw_button_pip(bhf->x1, bhf->y1, bhf->x2, bhf->y2, bhf->state, bhf->is_locked, bhf->state == DISABLED ? 1 : 0, bhf->text);
		}
	}

	void set_state_record(list_template_t * val)
	{
		gui.selected_id = val->id;								// добавить везде проверку на gui.selected_type
		switch (val->type)
		{
			case TYPE_BUTTON:
				gui.selected_type = TYPE_BUTTON;
				button_handlers[val->id].state = val->state;
				if (button_handlers[val->id].onClickHandler && button_handlers[val->id].state == RELEASED)
					button_handlers[val->id].onClickHandler();
				break;

			case TYPE_LABEL:
				gui.selected_type = TYPE_LABEL;
				labels[val->id].state = val->state;
				if (labels[val->id].onClickHandler && labels[val->id].state == RELEASED)
					labels[val->id].onClickHandler();
				break;
		}
	}

	void process_gui(void)
	{
		uint_fast16_t tx, ty;
		static uint_fast16_t x_old = 0, y_old = 0;
		static list_template_t * p = NULL;

	#if defined (TSC1_TYPE)
		if (board_tsc_is_pressed())
		{
			board_tsc_getxy(& tx, & ty);
			if (gui.fix && ty > gui.pip_y)			// первые координаты после нажатия от контролера тачскрина приходят старые, пропускаем
			{
				gui.last_pressed_x = tx;
				gui.last_pressed_y = ty - gui.pip_y;
				gui.is_touching_screen = 1;
				update_touch_list();
				debug_printf_P(PSTR("pip x: %d, pip y: %d\n"), gui.last_pressed_x, gui.last_pressed_y);
			}
			gui.fix = 1;
		}
		else
	#endif /* defined (TSC1_TYPE) */
		{
			gui.is_touching_screen = 0;
			gui.is_after_touch = 0;
			gui.fix = 0;
		}

		if (gui.state == CANCELLED && gui.is_touching_screen && ! gui.is_after_touch)
		{
			PLIST_ENTRY t;
			for (t = touch_list.Blink; t != & touch_list; t = t->Blink)
			{
				p = CONTAINING_RECORD(t, list_template_t, item);

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
				gui.vector_move_x = x_old ? gui.vector_move_x + gui.last_pressed_x - x_old : 0; // т.к. process_gui и display_pip_update
				gui.vector_move_y = y_old ? gui.vector_move_y + gui.last_pressed_y - y_old : 0; // вызываются с разной частотой, необходимо
				p->state = PRESSED;																// накопление вектора перемещения точки
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
#endif /* WITHTOUCHGUI */

#endif /* LCDMODE_LTDC */
