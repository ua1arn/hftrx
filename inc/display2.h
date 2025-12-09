/* $Id$ */
/* Pin manipulation functioms */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef DISPLAY2_H_INCLUDED
#define DISPLAY2_H_INCLUDED

#include "src/display/display.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef struct editfreq_tag
{
	uint32_t val;
	uint8_t num;
	uint8_t key;
	uint8_t error;
} editfreq_t;

typedef struct editfreq2_tag
{
	uint32_t freq;
	uint8_t blinkpos;		// позиция (степень 10) редактируесого символа
	uint8_t blinkstate;	// в месте редактируемого символа отображается подчёркивание (0 - пробел)
} editfreq2_t;

enum DCTX_t { DCTX_FREQ, DCTX_MENU };
typedef struct dctx_tag
{
	enum DCTX_t type;
	const void * pv;
} dctx_t;

// копирование растра в видеобуфер отображения
void display2_vtty(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t colspan, uint_fast8_t rowspan, dctx_t * pctx);
void display2_vtty_init(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t colspan, uint_fast8_t rowspan, dctx_t * pctx);

// FUNC menu item label & value
void display2_fnblock9(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t colspan, uint_fast8_t rowspan, dctx_t * pctx);

void display2_swrsts20(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t colspan, uint_fast8_t rowspan, dctx_t * pctx);

// вызывается по dzones
void display2_multilinemenu_block(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx);

uint_fast8_t display_getpagesmax(void);	// количество разных вариантов отображения (menuset)
uint_fast8_t display_getpagesleep(void);	// номер варианта отображения для "сна"
uint_fast8_t display_getfreqformat(uint_fast8_t * prjv);	// получить параметры отображения частоты (для функции прямого ввода)

void display2_bgprocess(uint_fast8_t inmenu, uint_fast8_t menuset, dctx_t * ctx);	// выполнение шагов state machine отображения дисплея
void display2_fillbg(const gxdrawb_t * db);	// очистить дисплей
void display2_initialize(void);	// проход по элементам с необходимостью инициализации
void display2_latch(void);
uint_fast8_t display2_mouse(uint_fast16_t x, uint_fast16_t y, unsigned evcode, uint_fast8_t inmenu, uint_fast8_t menuset, dctx_t * ctx);	// Обработка событий тачскрина или мыши
void display2_postmortem(void);

void display2_needupdate(void);

// Вызывается из display2.c
void
display2_bars(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t colspan, uint_fast8_t rowspan, dctx_t * pctx);
// Вызывается из display2.c
void
display2_bars_rx(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t colspan, uint_fast8_t rowspan, dctx_t * pctx);
// Вызывается из display2.c
void
display2_bars_tx(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t colspan, uint_fast8_t rowspan, dctx_t * pctx);
// Вызывается из display2.c
void
display2_adctest(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t colspan, uint_fast8_t rowspan, dctx_t * pctx);

void layout_init(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t colspan, uint_fast8_t rowspan, dctx_t * pctx);
void gui_WM_walkthrough(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t colspan, uint_fast8_t rowspan, dctx_t * pctx);

// Параметры отображения многострочного меню для больших экранов
typedef struct multimenuwnd_tag
{
	uint8_t multilinemenu_max_rows;
	uint8_t menurow_count;
	uint8_t ystep;
	uint8_t reverse;	// 0/1
	uint8_t valuew;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	uint8_t xspan;	/* количество знакомест по горизонтали, отдаваемое под меню */
} multimenuwnd_t;

void display2_getmultimenu(multimenuwnd_t * p); /* получение параметров окна для меню */

uint_fast8_t display2_getswrmax(void);

uint_fast16_t normalize(
	uint_fast16_t raw,
	uint_fast16_t rawmin,	// включает интервал входного raw
	uint_fast16_t rawmax,	// включает интервал входного raw
	uint_fast16_t range		// включает максимальное выходное значение
	);

int_fast32_t approximate(
	const int32_t * points,		// массив позиций входных значений
	const int32_t * angles,		// массив позицый выходных значений
	unsigned n,					// размерность массивов
	int_fast16_t v				// значение для анализа
	);

// FUNC menu

void display_2fmenuslines(
	const gxdrawb_t * db,
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t xspan,
	uint_fast8_t yspan,
	uint_fast8_t active,
	const char * label,
	const char * value
	);

void board_set_topdb(int_fast16_t v);			/* верхний предел FFT */
void board_set_bottomdb(int_fast16_t v);		/* нижний предел FFT */
void board_set_zoomxpow2(uint_fast8_t v);		/* уменьшение отображаемого участка спектра */
void board_set_view_style(uint_fast8_t v);		/* стиль отображения спектра и панорамы */
void board_set_view3dss_mark(uint_fast8_t v);	/* Для VIEW_3DSS - индикация полосы пропускания на спектре */
void display2_set_showdbm(uint_fast8_t v);			// Отображение уровня сигнала в dBm или S-memter (в зависимости от настроек)
void board_set_afspeclow(int_fast16_t v);		// нижняя частота отображения спектроанализатора
void board_set_afspechigh(int_fast16_t v);		// верхняя частота отображения спектроанализатора
void display2_set_lvlgridstep(uint_fast8_t v);		/* Шаг сетки уровней в децибелах */
void display2_set_rxbwsatu(uint_fast8_t v);		/* 0..100 - насыщнность цвета заполнения "шторки" - индикатор полосы пропускания примника на спкктре. */
void display2_set_spectrumpart(uint_fast8_t v); /* Часть отведенной под спектр высоты экрана 0..100 */
void display2_set_smetertype(uint_fast8_t v);
void display2_set_filter_spe(uint_fast8_t v);	/* парамеры видеофильтра спектра */
void display2_set_filter_wfl(uint_fast8_t v);	/* парамеры видеофильтра водопада */

COLORPIP_T display2_getbgcolor(void);
void display2_setbgcolor(COLORPIP_T c);

COLORPIP_T display2_get_spectrum(int x);

int_fast32_t display2_zoomedbw(void);
const char * display2_gethtml(uint_fast8_t page);

unsigned display2_gettileradius(void);

void display2_set_page_temp(uint_fast8_t page);
uint_fast8_t display_getpage0(void);
uint_fast8_t display_getpagegui(void);

#define SWRMIN 10	// минимум - соответствует SWR = 1.0, точность = 0.1


#if WITHLVGL

#include "lvgl.h"
#include "../demos/lv_demos.h"
#include "layouts/grid/lv_grid.h"
//#include "../demos/vector_graphic/lv_demo_vector_graphic.h"

#include "core/lv_obj_private.h"
#include "core/lv_obj_class_private.h"
#include "widgets/label/lv_label_private.h"
#include "widgets/image/lv_image_private.h"
#include "layouts/grid/lv_grid.h"
#include "misc/lv_event.h"

void styles_init(void);	// инициализация стилей, используемых объектами главного окна
void lvgl_test(void);	// создание элементов на главном окне
void lvgl_dev_init(void);

lv_color_t display_lvlcolor(COLORPIP_T c);	// преобразование цвета в тип LVGL

// Полные шрифты

lv_obj_t * lv_smtr2_create(lv_obj_t * parent);	// s-meter (own draw)
lv_obj_t * lv_wtrf_create(lv_obj_t * parent);	// waterfall
lv_obj_t * lv_wtrf2_create(lv_obj_t * parent);	// waterfall (own draw)
lv_obj_t * lv_sscp2_create(lv_obj_t * parent);	// RF specter (own draw)
lv_obj_t * lv_info_create(lv_obj_t * parent, int (* infocb)(char * b, size_t len, int * selector));
lv_obj_t * lv_sscp3dss_create(lv_obj_t * parent);	// 3dss waterfall

lv_obj_t * lv_compat_create(lv_obj_t * parent, const void * param);
void dzi_compat_draw_callback(lv_layer_t * layer, const void * dzpv, dctx_t * pctx);

void lv_wtrf2_draw(lv_layer_t * layer, const lv_area_t * coords);
lv_obj_t * lv_hamradiomenu_create(lv_obj_t * parent);


typedef struct
{
	lv_obj_t obj;

	lv_style_t stdigits;
	lv_style_t stlines;
    lv_draw_rect_dsc_t gradrect;
    uint8_t * gbuf1pix;	// с шириной в 1 пиксель
    lv_draw_buf_t gdrawb;
    lv_draw_rect_dsc_t grect_dsc;	// из этого прямоугольника будем брать для отрисовки вертикальных линий

    uint8_t * gbuftmp;	// буфер для отрисовки виджета в draw
    lv_draw_buf_t gdrawbtmp;

} lv_sscp2_t;


typedef struct
{
	lv_obj_t obj;

	lv_style_t stdigits;
	lv_style_t stlines;
    lv_draw_rect_dsc_t gradrect;
    uint8_t * gbuf1pix;	// с шириной в 1 пиксель
    lv_draw_buf_t gdrawb;
    lv_draw_rect_dsc_t grect_dsc;	// из этого прямоугольника будем брать для отрисовки вертикальных линий

    uint8_t * gbuftmp;	// буфер для отрисовки виджета в draw
    lv_draw_buf_t gdrawbtmp;

} lv_sscp3dss_t;


typedef struct
{
	lv_obj_t obj;
	//
} lv_smtr2_t;

typedef struct
{
	lv_label_t label;
	char infotext [32];
	int (* infocb)(char * b, size_t len, int * state);
} lv_info_t;

typedef struct
{
	lv_obj_t obj;
	lv_style_t stdigits;
	lv_style_t stlines;
} lv_wtrf2_t;

typedef struct
{
	lv_obj_t obj;
	const void * dzpv;
} lv_compat_t;

#if 0//defined (G2D_ROT) && ! LINUX_SUBSYSTEM

	#include "misc/lv_types.h"
	#include "misc/lv_color.h"
	#include "misc/lv_area.h"
	lv_result_t lv_draw_sw_image_awrot(
											bool is_transform,
											lv_color_format_t src_cf,
											const uint8_t *src_buf,
											const lv_area_t * coords,
											int32_t src_stride,
											const lv_area_t * des_area,
											const lv_draw_task_t * draw_task,
											const lv_draw_image_dsc_t * draw_dsc);

		#define LV_DRAW_SW_IMAGE(__transformed,                                     \
							 __cf,                                                  \
							 __src_buf,                                             \
							 __img_coords,                                          \
							 __src_stride,                                          \
							 __blend_area,                                          \
							 __draw_task,                                           \
							 __draw_dsc)                                            \
				lv_draw_sw_image_awrot(   (__transformed),                            \
										(__cf),                                     \
										(uint8_t *)(__src_buf),                     \
										(__img_coords),                             \
										(__src_stride),                             \
										(__blend_area),                             \
										(__draw_task),                              \
										(__draw_dsc))

#endif /* defined (G2D_ROT) && ! LINUX_SUBSYSTEM */

#endif /* WITHLVGL */

#ifdef __cplusplus
}
#endif /* __cplusplus */


#if defined (COLORPIP_SHADED)
	// LCDMODE_MAIN_L8 also defied

	// цвета
	// 0..COLORPIP_BASE-1 - волопад
	// COLORPIP_BASE..127 - надписи и элементы дизайна
	// то же с кодом больше на 128 - затененные цвета для получения полупрозрачности
	// 0..95 - палитра водопада
	// 96..111 - норм цвета
	// 112..127 - первая степень AA
	// Заполнение палитры производится в display2_xltrgb24()

	#define COLORPIP_YELLOW      (COLORPIP_BASE + 0) // TFTRGB(0xFF, 0xFF, 0x00)
	#define COLORPIP_ORANGE      (COLORPIP_BASE + 1) // TFTRGB(0xFF, 0xA5, 0x00)
	#define COLORPIP_BLACK       (COLORPIP_BASE + 2) // TFTRGB(0x00, 0x00, 0x00)
	#define COLORPIP_WHITE       (COLORPIP_BASE + 3) // TFTRGB(0xFF, 0xFF, 0xFF)
	#define COLORPIP_GRAY        (COLORPIP_BASE + 4) // TFTRGB(0x80, 0x80, 0x80)
	#define COLORPIP_DARKGREEN   (COLORPIP_BASE + 5) // TFTRGB(0x70, 0x70, 0x70) FIXME: use right value
	#define COLORPIP_BLUE        (COLORPIP_BASE + 6) // TFTRGB(0x00, 0x00, 0xFF)
	#define COLORPIP_GREEN       (COLORPIP_BASE + 7) // TFTRGB(0x00, 0xFF, 0x00)
	#define COLORPIP_RED         (COLORPIP_BASE + 8) // TFTRGB(0xFF, 0x00, 0x00)

	#define DSGN_LOCKED  	 	(COLORPIP_BASE + 9) // TFTRGB(0x3C, 0x3C, 0x00)
	#define DSGN_GRIDCOLOR0		(COLORPIP_BASE + 10) // TFTRGB(128, 0, 0)		//COLOR_GRAY - center marker
	#define DSGN_GRIDCOLOR2		(COLORPIP_BASE + 11) // TFTRGB(96, 96, 96)		//COLOR_DARKRED - other markers
	#define DSGN_SPECTRUMBG		(COLORPIP_BASE + 12) // TFTRGB(0, 64, 24)			//
	#define DSGN_SPECTRUMBG2	(COLORPIP_BASE + 13) // TFTRGB(0, 24, 8)		//COLOR_xxx - полоса пропускания приемника
	#define DSGN_SPECTRUMBG2RX2	(COLORPIP_BASE + 13) // TFTRGB(0, 24, 8)		//COLOR_xxx - полоса пропускания приемника
	#define DSGN_SPECTRUMFG		(COLORPIP_BASE + 14) // TFTRGB(0, 255, 0)		// цвет спектра при сполошном заполнении
	#define COLORPIP_DARKGRAY   (COLORPIP_BASE + 15) // TFTRGB(0x00, 0x64, 0x00)

	#define DSGN_BIGCOLOR 			COLORPIP_YELLOW 	// цвет частоты и режима основного приемника
	#define DSGN_BIGCOLORBACK 		COLORPIP_BLACK

	#define DSGN_SPECTRUMPEAKS 	COLORPIP_DARKGRAY
	#define DSGN_SPECTRUMLINE	COLORPIP_YELLOW
	#define DSGN_SPECTRUMFENCE	COLORPIP_WHITE

	#define DSGN_FMENUACTIVETEXT	COLORPIP_WHITE
	#define DSGN_FMENUACTIVEBACK	COLORPIP_DARKGREEN

	#define DSGN_FMENUINACTIVETEXT	COLORPIP_BLACK
	#define DSGN_FMENUINACTIVEBACK	COLORPIP_DARKGREEN

	#define DSGN_FMENUTEXT            COLORPIP_GREEN
	#define DSGN_FMENUBACK            COLORPIP_BLACK
	#define DSGN_SMLABELTEXT        COLORPIP_GREEN
	#define DSGN_SMLABELBACK        COLORPIP_BLACK
	#define DSGN_SMLABELPLKUSTEXT        COLORPIP_RED
	#define DSGN_SMLABELPLKUSBACK        COLORPIP_BLACK

	#define DSGN_LABELACTIVETEXT	COLORPIP_WHITE
	#define DSGN_LABELACTIVEBACK	COLORPIP_DARKRED
	#define DSGN_LABELINACTIVETEXT	COLORPIP_BLACK
	#define DSGN_LABELINACTIVEBACK	COLORPIP_RED

	// поля отображения, не имеющие вариантов по состоянию вкл/выкл
	#define DSGN_LABELTEXT			COLORPIP_RED
	#define DSGN_LABELBACK			COLORPIP_BLACK

	#define DSGN_PSUSTATETEXT			COLORPIP_WHITE	// температура, напряжение - was DSGN_LABELTEXT
	#define DSGN_PSUSTATEBACK			COLORPIP_BLACK

	/* обычный текст в меню */
	#define DSGN_MENUCOLOR 		COLORPIP_WHITE
	#define DSGN_MENUBGCOLOR		COLORPIP_BLACK
	/* выделенный текст в меню */
	#define DSGN_MENUSELCOLOR	COLORPIP_BLACK
	#define DSGN_MENUSELBGCOLOR	COLORPIP_DARKCYAN
	/* выделенный текст в значениях */
	#define DSGN_MENUVALSELCOLOR	COLORPIP_BLACK
	#define DSGN_MENUVALSELBGCOLOR	COLORPIP_GRAY

	#define DSGN_SMLCOLOR	COLORPIP_GREEN		// цвет левой половины S-метра
	#define DSGN_SMRCOLOR	COLORPIP_RED			// цвет правой половины S-метра
	#define DSGN_PWRCOLOR	COLORPIP_RED		// цвет измерителя мощности
	#define DSGN_SWRCOLOR	COLORPIP_YELLOW		// цвет SWR-метра

	#if LCDMODE_PALETTE256
		// Цвета, используемые на основном экране
		#define COLORPIP_DARKCYAN   COLORPIP_DARKGREEN
		#define COLORPIP_CYAN       COLORPIP_GREEN

		#define DSGN_GRIDDIGITS  DSGN_GRIDCOLOR2
		#define COLORPIP_DARKRED  DSGN_GRIDCOLOR2

	#endif /* LCDMODE_PALETTE256 */

	#define DSGN_OVFCOLOR COLORPIP_RED
	#define DSGN_LOCKCOLOR COLORPIP_RED
	#define DSGN_BGCOLOR (display2_getbgcolor())

#else /* */

	// определение основных цветов
	///


	// Заполнение палитры производится в display2_xltrgb24()

	#define COLOR_bgXXO         TFTRGB(0x01, 0x24, 0x37)	// flex radio BG color

	#define COLORPIP_YELLOW      COLOR_YELLOW
	#define COLORPIP_ORANGE      COLOR_ORANGE
	#define COLORPIP_BLACK       COLOR_BLACK
	#define COLORPIP_WHITE       COLOR_WHITEALL
	#define COLORPIP_GRAY        COLOR_GRAY
	#define COLORPIP_CYAN        COLOR_CYAN
	#define COLORPIP_OLIVE		 COLOR_OLIVE
	#define COLORPIP_DARKGRAY    COLOR_DARKGRAY
	#define COLORPIP_DARKGREEN   COLOR_DARKGREEN
	#define COLORPIP_DARKGRED    COLOR_DARKRED
	#define COLORPIP_DARKCYAN    COLOR_DARKCYAN
	#define COLORPIP_DARKBLUE    COLOR_DARKBLUE
	#define COLORPIP_BLUE        COLOR_BLUE
	#define COLORPIP_GREEN       COLOR_GREEN
	#define COLORPIP_RED         COLOR_RED
	#define COLORPIP_DARKRED   	 COLOR_DARKRED

	#if COLORSTYLE_RED
		// Spectrum/waterfall colors
		#define DSGN_GRIDCOLOR0     	COLORPIP_RED        	// center marker
		#define DSGN_GRIDCOLOR2     	COLORPIP_DARKRED      // other markers
		#define DSGN_GRIDDIGITS 		COLORPIP_RED

		#define DSGN_SPECTRUMBG     	COLORPIP_BLACK
		#define DSGN_SPECTRUMBG2   		COLORPIP_DARKRED      // полоса пропускания приемника
		#define DSGN_SPECTRUMBG2RX2   	COLORPIP_DARKGREEN      	// полоса пропускания приемника RX2
		#define DSGN_SPECTRUMFG			COLORPIP_RED			// цвет спектра при сполошном заполнении
		#define DSGN_SPECTRUMFENCE	COLOR_WHITEALL
		#define DSGN_SPECTRUMLINE		COLORPIP_RED
		#define DSGN_LOCKED  	 TFTRGB(0x3C, 0x3C, 0x00)
		#define DSGN_SPECTRUMPEAKS 		COLORPIP_DARKGRAY

	#elif COLORSTYLE_GREEN
		// Spectrum/waterfall colors
		#define DSGN_GRIDCOLOR0     	COLORPIP_GREEN        	// center marker
		#define DSGN_GRIDCOLOR2     	COLORPIP_DARKGREEN      // other markers
		#define DSGN_GRIDDIGITS 		COLORPIP_GREEN

		#define DSGN_SPECTRUMBG     	COLORPIP_BLACK
		#define DSGN_SPECTRUMBG2   		COLORPIP_DARKGREEN      // полоса пропускания приемника
		#define DSGN_SPECTRUMBG2RX2   	COLORPIP_DARKGRED      	// полоса пропускания приемника RX2
		#define DSGN_SPECTRUMFG			COLORPIP_GREEN			// цвет спектра при сполошном заполнении
		#define DSGN_SPECTRUMFENCE		COLOR_WHITEALL
		#define DSGN_SPECTRUMLINE		COLORPIP_GREEN
		#define DSGN_LOCKED  	 		TFTRGB(0x3C, 0x3C, 0x00)
		#define DSGN_SPECTRUMPEAKS 		COLORPIP_DARKGRAY


	#elif COLORSTYLE_BLUE
		// Spectrum/waterfall colors
		#define DSGN_GRIDCOLOR      	COLORPIP_OLIVE        // center marker
		#define DSGN_GRIDCOLOR0     	COLORPIP_DARKRED        // other markers
		#define DSGN_GRIDCOLOR2     	COLORPIP_DARKRED        // other markers

		#define DSGN_SPECTRUMBG     	COLORPIP_BLACK
		#define DSGN_SPECTRUMBG2    	COLORPIP_DARKCYAN        //  полоса пропускания приемника
		#define DSGN_SPECTRUMBG2RX2    	COLORPIP_DARKRED        //  полоса пропускания приемника RX2
		#define DSGN_SPECTRUMFG			COLORPIP_GREEN
		#define DSGN_SPECTRUMFENCE		COLORPIP_WHITE
		#define DSGN_SPECTRUMLINE		COLORPIP_YELLOW
		#define DSGN_SPECTRUMPEAKS 		COLORPIP_DARKGRAY
		#define DSGN_GRIDDIGITS 		COLORPIP_YELLOW

	#elif COLORSTYLE_BLUE2
		// Spectrum/waterfall colors
		#define DSGN_GRIDCOLOR0     	COLORPIP_GREEN        	// center marker
		#define DSGN_GRIDCOLOR2     	COLORPIP_DARKGREEN      // other markers
		#define DSGN_GRIDDIGITS 		COLORPIP_GREEN

		#define DSGN_SPECTRUMBG     	COLORPIP_BLACK
		#define DSGN_SPECTRUMBG2   		COLORPIP_DARKGREEN      // полоса пропускания приемника
		#define DSGN_SPECTRUMBG2RX2   	COLORPIP_DARKGRED      	// полоса пропускания приемника RX2
		#define DSGN_SPECTRUMFG			COLORPIP_GREEN			// цвет спектра при сполошном заполнении
		#define DSGN_SPECTRUMFENCE		COLOR_WHITEALL
		#define DSGN_SPECTRUMLINE		COLORPIP_GREEN
		#define DSGN_LOCKED  	 		TFTRGB(0x3C, 0x3C, 0x00)
		#define DSGN_SPECTRUMPEAKS 		COLORPIP_DARKGRAY

	#elif COLORSTYLE_WHITE
		// Spectrum/waterfall colors
		#define DSGN_GRIDCOLOR0        COLOR_OLIVE
		#define DSGN_GRIDCOLOR2     COLOR_GRAY        //COLOR_DARKRED - other markers
		#define DSGN_GRIDDIGITS     COLOR_YELLOW
		#define DSGN_SPECTRUMBG     COLOR_BLACK
		#define DSGN_SPECTRUMBG2    COLOR_CYAN                //COLOR_CYAN - полоса пропускания приемника
		#define DSGN_SPECTRUMBG2RX2    COLOR_DARKRED                //COLOR_CYAN - полоса пропускания приемника
		#define DSGN_SPECTRUMFG        COLOR_GREEN        // цвет спектра при сполошном заполнении
		#define DSGN_SPECTRUMFENCE    COLOR_WHITEALL    //COLOR_WHITEALL
		//#define DSGN_SPECTRUMLINE    COLORPIP_GREEN
		#define DSGN_SPECTRUMLINE    COLORPIP_YELLOW
		#define DSGN_LOCKED       TFTRGB(0x3C, 0x3C, 0x00)
		#define DSGN_SPECTRUMPEAKS         COLORPIP_DARKGRAY
	#else
		//#error Undefined color scheme - set COLORSTYLE_xxx

	#endif

#endif /* */

#if COLORSTYLE_RED
	// "All-in-red": FT1000 inspired color scheme
	#define DSGN_BIGCOLOR 			COLORPIP_RED 	// цвет частоты и режима основного приемника
	#define DSGN_BIGCOLORBACK 		COLORPIP_BLACK

	#define DSGN_FMENUACTIVETEXT	COLORPIP_WHITE
	#define DSGN_FMENUACTIVEBACK	COLORPIP_DARKRED
	#define DSGN_FMENUINACTIVETEXT	COLORPIP_BLACK
	#define DSGN_FMENUINACTIVEBACK	COLORPIP_RED
	#define DSGN_FMENUTEXT			COLORPIP_RED
	#define DSGN_FMENUBACK			COLORPIP_BLACK

	#define DSGN_LABELACTIVETEXT	COLORPIP_WHITE
	#define DSGN_LABELACTIVEBACK	COLORPIP_DARKRED
	#define DSGN_LABELINACTIVETEXT	COLORPIP_BLACK
	#define DSGN_LABELINACTIVEBACK	COLORPIP_RED
	// поля отображения, не имеющие вариантов по состоянию вкл/выкл
	#define DSGN_LABELTEXT			COLORPIP_RED
	#define DSGN_LABELBACK			COLORPIP_BLACK

	#define DSGN_PSUSTATETEXT			COLORPIP_RED	// температура, напряжение - was DSGN_LABELTEXT
	#define DSGN_PSUSTATEBACK			COLORPIP_BLACK

	/* обычный текст в меню */
	#define DSGN_MENUCOLOR 		COLORPIP_WHITE
	#define DSGN_MENUBGCOLOR		COLORPIP_BLACK
	/* выделенный текст в меню */
	#define DSGN_MENUSELCOLOR	COLORPIP_BLACK
	#define DSGN_MENUSELBGCOLOR	COLORPIP_DARKCYAN
	/* выделенный текст в значениях */
	#define DSGN_MENUVALSELCOLOR	COLORPIP_BLACK
	#define DSGN_MENUVALSELBGCOLOR	COLORPIP_GRAY

	// Text-like S-meter bar
	#define DSGN_SMLABELTEXT		COLORPIP_GREEN
	#define DSGN_SMLABELBACK		COLORPIP_BLACK
	#define DSGN_SMLABELPLKUSTEXT		COLORPIP_RED
	#define DSGN_SMLABELPLKUSBACK		COLORPIP_BLACK

	// Цвета используемые для отображения
	// различных элементов на основном экране.

	#define DSGN_BGCOLOR (display2_getbgcolor())

	#define DSGN_SMLCOLOR	COLORPIP_GREEN		// цвет левой половины S-метра
	#define DSGN_SMRCOLOR	COLORPIP_RED			// цвет правой половины S-метра
	#define DSGN_PWRCOLOR	COLORPIP_RED		// цвет измерителя мощности
	#define DSGN_SWRCOLOR	COLORPIP_YELLOW		// цвет SWR-метра

	#define DSGN_OVFCOLOR COLORPIP_RED
	#define DSGN_LOCKCOLOR COLORPIP_RED
	#define DSGN_TXRXMODECOLOR COLORPIP_BLACK
	#define DSGN_MODECOLORBG_TX COLORPIP_RED
	#define DSGN_MODECOLORBG_RX	COLORPIP_GREEN

	//#define DSGN_AFSPECTRE_COLOR COLORPIP_YELLOW
	#define DSGN_AFSPECTRE_COLOR DSGN_SPECTRUMBG2

#elif COLORSTYLE_GREEN
	#define DSGN_BIGCOLOR 			COLORPIP_YELLOW 	// цвет частоты и режима основного приемника
	#define DSGN_BIGCOLORBACK 		COLORPIP_BLACK

	#define DSGN_FMENUACTIVETEXT	COLORPIP_WHITE
	#define DSGN_FMENUACTIVEBACK	COLORPIP_DARKGREEN

	#define DSGN_FMENUINACTIVETEXT	COLORPIP_BLACK
	#define DSGN_FMENUINACTIVEBACK	COLORPIP_DARKGREEN

	#define DSGN_FMENUTEXT			COLORPIP_GREEN
	#define DSGN_FMENUBACK			COLORPIP_BLACK

	#define DSGN_LABELACTIVETEXT	COLORPIP_WHITE
	#define DSGN_LABELACTIVEBACK	COLORPIP_DARKGREEN

	#define DSGN_LABELINACTIVETEXT	COLORPIP_BLACK
	#define DSGN_LABELINACTIVEBACK	COLORPIP_DARKGREEN
	// поля отображения, не имеющие вариантов по состоянию вкл/выкл
	#define DSGN_LABELTEXT			COLORPIP_WHITE//COLORPIP_GREEN
	#define DSGN_LABELBACK			COLORPIP_DARKGREEN//COLORPIP_BLACK

	#define DSGN_PSUSTATETEXT			COLORPIP_WHITE	// температура, напряжение - was DSGN_LABELTEXT
	#define DSGN_PSUSTATEBACK			COLORPIP_DARKGREEN

	/* обычный текст в меню */
	#define DSGN_MENUCOLOR 		COLORPIP_WHITE
	#define DSGN_MENUBGCOLOR		COLORPIP_BLACK
	/* выделенный текст в меню */
	#define DSGN_MENUSELCOLOR	COLORPIP_BLACK
	#define DSGN_MENUSELBGCOLOR	COLORPIP_DARKCYAN
	/* выделенный текст в значениях */
	#define DSGN_MENUVALSELCOLOR	COLORPIP_BLACK
	#define DSGN_MENUVALSELBGCOLOR	COLORPIP_GRAY

	// Text-like S-meter bar
	#define DSGN_SMLABELTEXT		COLORPIP_GREEN
	#define DSGN_SMLABELBACK		COLORPIP_BLACK
	#define DSGN_SMLABELPLKUSTEXT		COLORPIP_RED
	#define DSGN_SMLABELPLKUSBACK		COLORPIP_BLACK

	// Цвета используемые для отображения
	// различных элементов на основном экране.

	#define DSGN_BGCOLOR (display2_getbgcolor())

	#define DSGN_SMLCOLOR	COLORPIP_GREEN		// цвет левой половины S-метра
	#define DSGN_SMRCOLOR	COLORPIP_RED			// цвет правой половины S-метра
	#define DSGN_PWRCOLOR	COLORPIP_RED		// цвет измерителя мощности
	#define DSGN_SWRCOLOR	COLORPIP_YELLOW		// цвет SWR-метра

	#define DSGN_OVFCOLOR COLORPIP_RED
	#define DSGN_LOCKCOLOR COLORPIP_RED
	#define DSGN_TXRXMODECOLOR COLORPIP_BLACK
	#define DSGN_MODECOLORBG_TX COLORPIP_RED
	#define DSGN_MODECOLORBG_RX	COLORPIP_GREEN

	//#define DSGN_AFSPECTRE_COLOR COLORPIP_YELLOW
	#define DSGN_AFSPECTRE_COLOR DSGN_SPECTRUMBG2

#elif COLORSTYLE_BLUE

	#define DSGN_BIGCOLOR 			COLORPIP_WHITE 	// цвет частоты и режима основного приемника
	#define DSGN_BIGCOLORBACK 		COLORPIP_BLACK

	#define DSGN_FMENUACTIVETEXT	COLORPIP_WHITE
	#define DSGN_FMENUACTIVEBACK	COLORPIP_BLACK
	#define DSGN_FMENUINACTIVETEXT	COLORPIP_DARKCYAN
	#define DSGN_FMENUINACTIVEBACK	COLORPIP_BLACK
	#define DSGN_FMENUTEXT			COLORPIP_GREEN
	#define DSGN_FMENUBACK			COLORPIP_BLACK

	#define DSGN_LABELACTIVETEXT	COLORPIP_WHITE
	#define DSGN_LABELACTIVEBACK	COLORPIP_BLACK
	#define DSGN_LABELINACTIVETEXT	COLORPIP_DARKCYAN
	#define DSGN_LABELINACTIVEBACK	COLORPIP_BLACK

	// поля отображения, не имеющие вариантов по состоянию вкл/выкл
	#define DSGN_LABELTEXT			COLORPIP_WHITE
	#define DSGN_LABELBACK			COLORPIP_BLACK

	#define DSGN_PSUSTATETEXT			COLORPIP_WHITE	// температура, напряжение - was DSGN_LABELTEXT
	#define DSGN_PSUSTATEBACK			COLORPIP_BLACK

	/* обычный текст в меню */
	#define DSGN_MENUCOLOR 		COLORPIP_WHITE
	#define DSGN_MENUBGCOLOR		COLORPIP_BLACK
	/* выделенный текст в меню */
	#define DSGN_MENUSELCOLOR	COLORPIP_BLACK
	#define DSGN_MENUSELBGCOLOR	COLORPIP_DARKCYAN
	/* выделенный текст в значениях */
	#define DSGN_MENUVALSELCOLOR	COLORPIP_BLACK
	#define DSGN_MENUVALSELBGCOLOR	COLORPIP_GRAY

	// Text-like S-meter bar
	#define DSGN_SMLABELTEXT		COLORPIP_GREEN
	#define DSGN_SMLABELBACK		COLORPIP_BLACK
	#define DSGN_SMLABELPLKUSTEXT		COLORPIP_RED
	#define DSGN_SMLABELPLKUSBACK		COLORPIP_BLACK

	// Цвета используемые для отображения
	// различных элементов на основном экране.

	#define DSGN_BGCOLOR (display2_getbgcolor())

	#define DSGN_SMLCOLOR	COLORPIP_GREEN		// цвет левой половины S-метра
	#define DSGN_SMRCOLOR	COLORPIP_RED			// цвет правой половины S-метра
	#define DSGN_PWRCOLOR	COLORPIP_RED		// цвет измерителя мощности
	#define DSGN_SWRCOLOR	COLORPIP_YELLOW		// цвет SWR-метра

	#define DSGN_OVFCOLOR COLORPIP_RED
	#define DSGN_LOCKCOLOR COLORPIP_RED
	#define DSGN_TXRXMODECOLOR COLORPIP_BLACK
	#define DSGN_MODECOLORBG_TX COLORPIP_RED
	#define DSGN_MODECOLORBG_RX	COLORPIP_GREEN

	//#define DSGN_AFSPECTRE_COLOR COLORPIP_YELLOW
	#define DSGN_AFSPECTRE_COLOR DSGN_SPECTRUMBG2

#elif COLORSTYLE_BLUE2

	#define DSGN_BIGCOLOR 			COLORPIP_DARKCYAN 	// цвет частоты и режима основного приемника
	#define DSGN_BIGCOLORBACK 		COLORPIP_BLACK

	#define DSGN_FMENUACTIVETEXT	COLORPIP_WHITE
	#define DSGN_FMENUACTIVEBACK	COLORPIP_DARKCYAN

	#define DSGN_FMENUINACTIVETEXT	COLORPIP_BLACK
	#define DSGN_FMENUINACTIVEBACK	COLORPIP_DARKCYAN

	#define DSGN_FMENUTEXT			COLORPIP_BLUE
	#define DSGN_FMENUBACK			COLORPIP_BLACK

	#define DSGN_LABELACTIVETEXT	COLORPIP_WHITE
	#define DSGN_LABELACTIVEBACK	COLORPIP_DARKCYAN

	#define DSGN_LABELINACTIVETEXT	COLORPIP_BLACK
	#define DSGN_LABELINACTIVEBACK	COLORPIP_DARKCYAN
	// поля отображения, не имеющие вариантов по состоянию вкл/выкл
	#define DSGN_LABELTEXT			COLORPIP_WHITE//COLORPIP_GREEN
	#define DSGN_LABELBACK			COLORPIP_DARKCYAN//COLORPIP_BLACK

	#define DSGN_PSUSTATETEXT			COLORPIP_WHITE	// температура, напряжение - was DSGN_LABELTEXT
	#define DSGN_PSUSTATEBACK			COLORPIP_DARKCYAN

	/* обычный текст в меню */
	#define DSGN_MENUCOLOR 		COLORPIP_WHITE
	#define DSGN_MENUBGCOLOR		COLORPIP_BLACK
	/* выделенный текст в меню */
	#define DSGN_MENUSELCOLOR	COLORPIP_BLACK
	#define DSGN_MENUSELBGCOLOR	COLORPIP_DARKCYAN
	/* выделенный текст в значениях */
	#define DSGN_MENUVALSELCOLOR	COLORPIP_BLACK
	#define DSGN_MENUVALSELBGCOLOR	COLORPIP_GRAY

	// Text-like S-meter bar
	#define DSGN_SMLABELTEXT		COLORPIP_GREEN
	#define DSGN_SMLABELBACK		COLORPIP_BLACK
	#define DSGN_SMLABELPLKUSTEXT		COLORPIP_RED
	#define DSGN_SMLABELPLKUSBACK		COLORPIP_BLACK

	// Цвета используемые для отображения
	// различных элементов на основном экране.

	#define DSGN_BGCOLOR (display2_getbgcolor())

	#define DSGN_SMLCOLOR	COLORPIP_GREEN		// цвет левой половины S-метра
	#define DSGN_SMRCOLOR	COLORPIP_RED			// цвет правой половины S-метра
	#define DSGN_PWRCOLOR	COLORPIP_RED		// цвет измерителя мощности
	#define DSGN_SWRCOLOR	COLORPIP_YELLOW		// цвет SWR-метра

	#define DSGN_OVFCOLOR COLORPIP_RED
	#define DSGN_LOCKCOLOR COLORPIP_RED
	#define DSGN_TXRXMODECOLOR COLORPIP_BLACK
	#define DSGN_MODECOLORBG_TX COLORPIP_RED
	#define DSGN_MODECOLORBG_RX	COLORPIP_GREEN

	//#define DSGN_AFSPECTRE_COLOR COLORPIP_YELLOW
	#define DSGN_AFSPECTRE_COLOR DSGN_SPECTRUMBG2

#elif COLORSTYLE_WHITE

	#define DSGN_BIGCOLOR             COLORPIP_WHITE     // цвет частоты и режима основного приемника
	#define DSGN_BIGCOLORBACK         COLORPIP_BLACK

	#define DSGN_FMENUACTIVETEXT    COLORPIP_WHITE
	#define DSGN_FMENUACTIVEBACK    COLORPIP_GRAY
	#define DSGN_FMENUINACTIVETEXT    COLORPIP_BLACK
	#define DSGN_FMENUINACTIVEBACK    COLORPIP_GRAY

	#define DSGN_LABELACTIVETEXT    COLORPIP_WHITE
	#define DSGN_LABELACTIVEBACK    COLORPIP_GRAY

	#define DSGN_LABELINACTIVETEXT    COLORPIP_BLACK
	#define DSGN_LABELINACTIVEBACK    COLORPIP_GRAY

	// поля отображения, не имеющие вариантов по состоянию вкл/выкл
	#define DSGN_LABELTEXT            COLORPIP_WHITE
	#define DSGN_LABELBACK            COLORPIP_BLACK

	#define DSGN_PSUSTATETEXT            COLORPIP_WHITE    // температура, напряжение - was DSGN_LABELTEXT
	#define DSGN_PSUSTATEBACK            COLORPIP_BLACK

	#define DSGN_FMENUTEXT            COLORPIP_GREEN
	#define DSGN_FMENUBACK            COLORPIP_BLACK
	#define DSGN_SMLABELTEXT        COLORPIP_GREEN
	#define DSGN_SMLABELBACK        COLORPIP_BLACK
	#define DSGN_SMLABELPLKUSTEXT        COLORPIP_RED
	#define DSGN_SMLABELPLKUSBACK        COLORPIP_BLACK

	/* обычный текст в меню */
	#define DSGN_MENUCOLOR 		COLORPIP_WHITE
	#define DSGN_MENUBGCOLOR		COLORPIP_BLACK
	/* выделенный текст в меню */
	#define DSGN_MENUSELCOLOR	COLORPIP_BLACK
	#define DSGN_MENUSELBGCOLOR	COLORPIP_DARKCYAN
	/* выделенный текст в значениях */
	#define DSGN_MENUVALSELCOLOR	COLORPIP_BLACK
	#define DSGN_MENUVALSELBGCOLOR	COLORPIP_GRAY

	// Text-like S-meter bar
	#define DSGN_SMLABELTEXT		COLORPIP_GREEN
	#define DSGN_SMLABELBACK		COLORPIP_BLACK
	#define DSGN_SMLABELPLKUSTEXT		COLORPIP_RED
	#define DSGN_SMLABELPLKUSBACK		COLORPIP_BLACK

	// Цвета используемые для отображения
	// различных элементов на основном экране.

	#define DSGN_BGCOLOR (display2_getbgcolor())

	#define DSGN_SMLCOLOR	COLORPIP_GREEN		// цвет левой половины S-метра
	#define DSGN_SMRCOLOR	COLORPIP_RED			// цвет правой половины S-метра
	#define DSGN_PWRCOLOR	COLORPIP_RED		// цвет измерителя мощности
	#define DSGN_SWRCOLOR	COLORPIP_YELLOW		// цвет SWR-метра

	#define DSGN_OVFCOLOR COLORPIP_RED
	#define DSGN_LOCKCOLOR COLORPIP_RED
	#define DSGN_TXRXMODECOLOR COLORPIP_BLACK
	#define DSGN_MODECOLORBG_TX COLORPIP_RED
	#define DSGN_MODECOLORBG_RX	COLORPIP_GREEN

	//#define DSGN_AFSPECTRE_COLOR COLORPIP_YELLOW
	#define DSGN_AFSPECTRE_COLOR DSGN_SPECTRUMBG2

#else /* COLORSTYLE_RED */
	//#error Undefined color scheme - set COLORSTYLE_xxx

#endif /* COLORSTYLE_RED */

#define MIDCELLS 8

#endif /* BOARD_H_INCLUDED */
