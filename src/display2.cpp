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
#include "keyboard.h"
#include "encoder.h"
#include "formats.h"

#include <string.h>
#include <math.h>
#include "src/gui/gui.h"
#include "src/touch/touch.h"

#include "dspdefines.h"

//#define WITHSECTRUMPEAKS 1	// Рисование peak value

#if LVGL_VERSION_MAJOR == 9 && LVGL_VERSION_MINOR < 3
void lv_obj_set_flag(lv_obj_t * obj, lv_obj_flag_t f, bool v)
{
    if(v) lv_obj_add_flag(obj, f);
    else lv_obj_remove_flag(obj, f);
}

void lv_style_set_margin_all(lv_style_t * style, int32_t value)
{
    lv_style_set_margin_left(style, value);
    lv_style_set_margin_right(style, value);
    lv_style_set_margin_top(style, value);
    lv_style_set_margin_bottom(style, value);
}
#endif /* LVGL_VERSION_MAJOR == 9 && LVGL_VERSION_MINOR < 3 */

/* struct dzone subset field values */

#define PAGELATCH 12	// bit position
#define PAGEINIT 13		// bit position
#define PAGESLEEP 14	// bit position
#define PAGEMENU 15		// bit position

#define PAGEBITS 16

#define REDRSUBSET(page)		(UINT16_C(1) << (page))	// сдвиги соответствуют номеру отображаемого набора элементов

#define REDRSUBSET_MENU		REDRSUBSET(PAGEMENU)
#define REDRSUBSET_SLEEP	REDRSUBSET(PAGESLEEP)
/* специальные биты */
#define REDRSUBSET_INIT		REDRSUBSET(PAGEINIT)
#define REDRSUBSET_LATCH	REDRSUBSET(PAGELATCH)
#define REDRSUBSET_SHOW		((uint16_t) (~ REDRSUBSET(PAGELATCH) & ~ REDRSUBSET(PAGEINIT)))	// кроме специальных

struct dzone;
typedef struct dzitem
{
	void (* draw)(struct dzone * dzp);
	void (* onclick)(struct dzone * dzp);
#if WITHLVGL
	lv_obj_t * (* lvelementcreate)(lv_obj_t * parent, const struct dzone * dzp, const struct dzitem * dzip, unsigned i);
#else /* WITHLVGL */
	void * lvelementcreate;
#endif /* WITHLVGL */
	const char * id;	// html id
} dzitem_t;

typedef struct dzone
{
	uint8_t x; // левый верхний угол
	uint8_t y;
	uint8_t colspan;
	uint8_t rowspan;
	void (* redraw)(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t colspan, uint_fast8_t rowspan, dctx_t * pctx);	// функция отображения элемента
	const dzitem_t * dzip;
	uint16_t subset;	// битовая маска страниц
} dzone_t;

#if WITHLVGL

static lv_style_t xxmainstyle;
static lv_style_t xxfreqastyle;
static lv_style_t xxfreqbstyle;
static lv_style_t xxdivstyle;
static lv_style_t xxcompatstyle;	// отрисовка элементов через функции совместимости
static lv_style_t xxscopestyle;
static lv_style_t xxtxrxstyle;
//static lv_style_t xxsmeterstyle;
static lv_style_t xxcellstyle;		// объекты, выполняющие разметку для вложенных объектов - без зазоров, без рамки, с прозрачным фоном.
static lv_style_t xxmenustyle;

static lv_obj_t * xxmainwnds [PAGEBITS];	// разные экраны (основной, меню, sleep */
static lv_group_t * xxgroup;

// преобразование цвета в тип LVGL
lv_color_t display_lvlcolor(COLORPIP_T c)
{
	return lv_color_make(
			COLORPIP_R(c),
			COLORPIP_G(c),
			COLORPIP_B(c));
}

static void lvstales_initialize(void)
{
    ASSERT(lv_screen_active());

	lv_obj_clear_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);

	{
		lv_style_t * const s = & xxmainstyle;

		// стиль окна
		lv_style_init(s);
		lv_style_set_bg_color(s, display_lvlcolor(display2_getbgcolor()));
		lv_style_set_border_width(s, 0);
		lv_style_set_pad_all(s, 0);
		lv_style_set_radius(s, 0);
	//	lv_style_set_grid_cell_row_span(s, 5);
	//	lv_style_set_grid_cell_column_span(s, 16);
		lv_style_set_align(s, LV_ALIGN_TOP_LEFT);
	}

//	{
//		// s-meter
//		lv_style_t * const s = & xxsmeterstyle;
//		//
//		lv_style_init(s);
//	}
	{
		// стиль элемента
		lv_style_t * const s = & xxdivstyle;
		//
		lv_style_init(s);
		//lv_style_set_bg_color(s, display_lvlcolor(COLORPIP_GREEN));
		lv_style_set_bg_color(s, lv_palette_main(LV_PALETTE_GREY));

		lv_style_set_text_color(s, display_lvlcolor(DSGN_LABELACTIVETEXT));
	    lv_style_set_bg_color(s, display_lvlcolor(DSGN_LABELACTIVEBACK));
		lv_style_set_bg_opa(s, LV_OPA_COVER);
		lv_style_set_border_color(s, lv_palette_main(LV_PALETTE_LIGHT_BLUE));
		lv_style_set_border_width(s, 1);
		lv_style_set_pad_all(s, 0);
		lv_style_set_radius(s, 4);
		lv_style_set_text_align(s, LV_TEXT_ALIGN_CENTER);
		lv_style_set_text_opa(s, LV_OPA_COVER);
	    lv_style_set_text_font(s, & Roboto_Medium_18_w1);	// was: Rubik_Medium_18_w2
	    //lv_style_set_text_letter_space(s, 3);
	}
	{
		// стиль меню настроек
		lv_style_t * const s = & xxmenustyle;
		//
		lv_style_init(s);
		//lv_style_set_bg_color(s, display_lvlcolor(COLORPIP_GREEN));
		lv_style_set_bg_color(s, lv_palette_main(LV_PALETTE_GREY));

		lv_style_set_text_color(s, display_lvlcolor(DSGN_LABELACTIVETEXT));
	    lv_style_set_bg_color(s, display_lvlcolor(display2_getbgcolor()));
		lv_style_set_bg_opa(s, LV_OPA_COVER);
		lv_style_set_border_color(s, lv_palette_main(LV_PALETTE_LIGHT_BLUE));
		lv_style_set_border_width(s, 0);
		lv_style_set_pad_all(s, 0);
		lv_style_set_radius(s, 0);
		lv_style_set_text_align(s, LV_TEXT_ALIGN_LEFT);
		lv_style_set_text_opa(s, LV_OPA_COVER);
	    lv_style_set_text_font(s, & Rubik_Medium_18_w2);
	    //lv_style_set_text_letter_space(s, 3);
	}

	{
		// частота основного приемникв
		lv_style_t * const s = & xxfreqastyle;

	    lv_style_init(s);
	    lv_style_set_text_color(s, display_lvlcolor(DSGN_BIGCOLOR));
	    lv_style_set_bg_color(s, display_lvlcolor(display2_getbgcolor()));
	    lv_style_set_text_align(s, LV_TEXT_ALIGN_RIGHT);
	    lv_style_set_pad_all(s, 0);
	    lv_style_set_text_font(s, & Epson_LTDC_big);
	    //lv_style_set_text_letter_space(s, 5);
		lv_style_set_border_width(s, 0);

	}

	{
		// частота второго приемникв
		lv_style_t * const s = & xxfreqbstyle;

	    lv_style_init(s);
//	    lv_style_set_text_color(s, display_lvlcolor(DSGN_BIGCOLORB));
//	    lv_style_set_bg_color(s, display_lvlcolor(display2_getbgcolor()));
	    lv_style_set_text_align(s, LV_TEXT_ALIGN_RIGHT);
	    //lv_style_set_pad_ver(s, 15);
	    //lv_style_set_text_font(s, & eurostyle_56w);
	    //lv_style_set_text_letter_space(s, 5);
		////lv_style_set_border_width(s, 0);

	}

	{
		// водопад/спектроаналихатор/3dss
		lv_style_t * const s = & xxscopestyle;
	    lv_style_init(s);
		lv_style_set_border_width(s, 0);
		lv_style_set_radius(s, 0);

	//    static lv_anim_t a;
	//    lv_anim_set_repeat_delay(& a, 20);
	//    lv_anim_set_path_cb(&a, lvanim_path_ticks);
	//    lv_style_set_anim(s, & a);
	}

	{
		// отрисовка элементов через функции совместимости
		lv_style_t * const s = & xxcompatstyle;
	    lv_style_init(s);
		lv_style_set_border_width(s, 0);
		lv_style_set_radius(s, 0);
	    lv_style_set_text_color(s, display_lvlcolor(COLOR_WHITE));
	    lv_style_set_bg_color(s, display_lvlcolor(display2_getbgcolor()));
	}

	{
		// TX/RX indicator
		lv_style_t * const s = & xxtxrxstyle;
	    lv_style_init(s);
//		lv_style_set_border_width(s, 0);
//		lv_style_set_radius(s, 8);
	}

	{
		// cells
		// объекты, выполняющие разметку для вложенных объектов - без зазоров, без рамки, с прозрачным фоном.
		lv_style_t * const s = & xxcellstyle;
	    lv_style_init(s);

	    //lv_style_set_text_align(s, LV_TEXT_ALIGN_CENTER);
		lv_style_set_border_width(s, 0);
		lv_style_set_radius(s, 0);

		lv_style_set_pad_column(s, 0);
		lv_style_set_pad_row(s, 0);

		lv_style_set_margin_all(s, 0);

		lv_style_set_pad_top(s, 0);
		lv_style_set_pad_bottom(s, 0);
		lv_style_set_pad_left(s, 0);
		lv_style_set_pad_right(s, 0);

		lv_style_set_bg_opa(s, LV_OPA_0);
	}
}

#if defined (TSC1_TYPE)
static void input_tsc_read_cb(lv_indev_t * drv, lv_indev_data_t * data)
{
	uint_fast16_t x, y;

	data->state = board_tsc_getxy(& x, & y) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
	data->point.x = x;
	data->point.y = y;
}
#endif /* defined (TSC1_TYPE) */

#if WITHKEYBOARD
static void input_keypad_read_cb(lv_indev_t * drv, lv_indev_data_t * data)
{
#if WITHLVGLINDEV
	uint_fast8_t kbch;

	data->state = kbd_scan(& kbch) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
	// lv_key_t (заняты коды до 127)
	data->key = kbch;
#else
	// stub
	data->state = LV_INDEV_STATE_RELEASED;
	// lv_key_t (заняты коды до 127)
	data->key = 0;
#endif /*  */
}
#endif /* WITHKEYBOARD */

#if WITHENCODER
static void input_encoder_read_cb(lv_indev_t * drv, lv_indev_data_t * data)
{
#if WITHLVGLINDEV
	encoder_t * const e = (encoder_t *) lv_indev_get_driver_data(drv);
	LV_ASSERT_NULL(e);
	data->state = LV_INDEV_STATE_RELEASED;
	data->enc_diff = encoder_get_delta(e);
//	if (data->enc_diff)
//	{
//		PRINTF("input_encoder_read_cb: delta=%d\n", (int) data->enc_diff);
//	}
#else
	// stub
	data->state = LV_INDEV_STATE_RELEASED;
	data->enc_diff = 0;
#endif /*  */
}
#endif /* WITHENCODER */

//#include "misc/lv_event_private.h"
//extern "C" const char * lv_event_code_get_name(lv_event_code_t code);

static int demostate;

static void mainwndkeyhandler(lv_event_t * e)
{
	lv_obj_t * const obj = (lv_obj_t *) lv_event_get_target(e);
//	PRINTF("ev: %s\n", lv_event_code_get_name(lv_event_get_code(e)));
	switch (lv_event_get_code(e))
	{
	case LV_EVENT_KEY:
		PRINTF("main event key=%u, obj=%p\n", (unsigned) lv_event_get_key(e), lv_event_get_target(e));
		if (lv_event_get_key(e) == KBD_CODE_DISPMODE)
		{
			demostate = ! demostate;
		}
		break;
	case LV_EVENT_CLICKED:
		PRINTF("main clicked , obj=%p\n", lv_event_get_target(e));
		break;
	case LV_EVENT_ROTARY:
		PRINTF("main rotary, diff=%d, obj=%p\n", (int) lv_event_get_rotary_diff(e), lv_event_get_target(e));
		break;
	default:
		break;
	}
}

static void sleepwndkeyhandler(lv_event_t * e)
{
	lv_obj_t * const obj = (lv_obj_t *) lv_event_get_target(e);
//	PRINTF("ev: %s\n", lv_event_code_get_name(lv_event_get_code(e)));
	switch (lv_event_get_code(e))
	{
	case LV_EVENT_KEY:
		PRINTF("sleep event key=%u, obj=%p\n", (unsigned) lv_event_get_key(e), lv_event_get_target(e));
		if (lv_event_get_key(e) == KBD_CODE_DISPMODE)
		{
			demostate = ! demostate;
		}
		break;
	case LV_EVENT_CLICKED:
		PRINTF("sleep clicked , obj=%p\n", lv_event_get_target(e));
		break;
	case LV_EVENT_ROTARY:
		PRINTF("sleep rotary, diff=%d, obj=%p\n", (int) lv_event_get_rotary_diff(e), lv_event_get_target(e));
		break;
	default:
		break;
	}
}

static lv_ll_t updateslist;

static void updateslist_init(void)
{
	lv_ll_init(& updateslist, sizeof (lv_obj_t * *));
}

// Включить элемент в список требующиз обновления
static void uiupdates(lv_obj_t * obj)
{
//	lv_obj_t * * palce = (lv_obj_t * *) lv_ll_ins_tail(& updateslist);
//	LV_ASSERT_MALLOC(palce);
//	* palce = obj;
}

// Заставить перерисоваться объекты из списка обновления
static void sendupdates(void)
{
//	void * it;
//    LV_LL_READ(& updateslist, it)
//	{
//    	lv_obj_invalidate(* (lv_obj_t * *) it);
//	}
}

static lv_obj_t * dzi_create_GUI(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	// Тут создать все интересующие жлементы - размер в экран у parent ставится
	lv_obj_t * const lbl = lv_label_create(parent);

	lv_obj_add_style(lbl, & xxdivstyle, 0);
	//lv_label_set_text_fmt(lbl, "el%u", i);
	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_default(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_label_create(parent);

	lv_obj_add_style(lbl, & xxdivstyle, 0);
	lv_label_set_text_fmt(lbl, "el%u", i);

	return lbl;
}

static dctx_t * compat_pctx;	// хак, эта переменная заполняется перед вызовом lv_timer_handler()
void dzi_compat_draw_callback(lv_layer_t * layer, const void * dzpv, dctx_t * pctx)
{
	const struct dzone * const dzp = (const struct dzone *) dzpv;

	gxdrawb_t dbv;
	gxdrawb_initlvgl(& dbv, layer);
	(* dzp->redraw)(& dbv, dzp->x, dzp->y, dzp->colspan, dzp->rowspan, compat_pctx);

}
// для быстрого перехода на систему ввода LVGL отрисовка этих widgets
// производится вызовом старых функций
static lv_obj_t * dzi_create_compat(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_compat_create(parent, dzp);

	lv_obj_add_style(lbl, & xxcompatstyle, 0);

	uiupdates(lbl);
	return lbl;
}




static lv_obj_t * dzi_create_menu(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const menu = lv_hamradiomenu_create(parent);

	lv_obj_add_style(menu, & xxmenustyle, 0);
	return menu;
}

static lv_obj_t * dzi_create_modea(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_modea);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_modeb(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_modeb);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_antenna(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_ant5);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_preamp_ovf(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_preamp_ovf);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_tune(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_tune);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_bypass(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_bypass);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_rxbw(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_rxbw);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_rxbwval(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_rxbwval);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_voltlevel(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_voltlevel);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

// Печать времени - только часы и минуты, без секунд
// Jan-01 13:40
static lv_obj_t * dzi_create_datetime12(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_datetime12);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_rec(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_rec);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_agc(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_agc);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_notch(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_notch);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_lock(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_lock);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_spk(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_spk);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_wpm(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_wpm);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_bkin(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_bkin);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_usbact(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_usbact);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_btact(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_btact);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_vfomode(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_vfomode);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_nr(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_nr);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_classa(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_classa);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_datamode(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_datamode);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_voxtune(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_voxtune);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_siglevel(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_siglevel);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_currlevel(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_currlevel);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_thermo(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_thermo);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_attenuator(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_attenuator);

	lv_obj_add_style(lbl, & xxdivstyle, 0);

	uiupdates(lbl);
	return lbl;
}


static lv_obj_t * dzi_create_freqa(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_freqa);

	lv_obj_add_style(lbl, & xxdivstyle, 0);
	lv_obj_add_style(lbl, & xxfreqastyle, LV_PART_MAIN);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_freqb(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_freqb);

	lv_obj_add_style(lbl, & xxdivstyle, 0);
	lv_obj_add_style(lbl, & xxfreqbstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_txrx(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_info_create(parent, infocb_txrx);

	lv_obj_add_style(lbl, & xxdivstyle, 0);
	lv_obj_add_style(lbl, & xxtxrxstyle, 0);

	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_smtr2(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_smtr2_create(parent);

	lv_obj_add_style(lbl, & xxdivstyle, 0);
	//lv_obj_add_style(lbl, & xxscopestyle, 0);
//	ASSERT(lv_obj_is_editable(lbl));
//	lv_obj_add_event_cb(lbl, sleepwndkeyhandler, LV_EVENT_ROTARY, NULL);	// Объект должен быть editable

	uiupdates(lbl);
	return lbl;
}

// отображение 3DSS
static lv_obj_t * dzi_create_3dss(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	lv_obj_t * const lbl = lv_sscp3dss_create(parent);

	lv_obj_add_style(lbl, & xxdivstyle, 0);
	lv_obj_add_style(lbl, & xxscopestyle, 0);

	uiupdates(lbl);
	return lbl;
}

// отображение водопада/спектра/3DSS
static lv_obj_t * dzi_create_gcombo(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
#if 0
	lv_obj_t * const lbl = lv_wtrf_create(parent);

	lv_obj_add_style(lbl, & xxdivstyle, 0);
	lv_obj_add_style(lbl, & xxscopestyle, 0);

	uiupdates(lbl);
	return lbl;
#else

    static int32_t col_dsc[] = { LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };				// занимаем всю ширину родителя
    static int32_t row_dsc[] = { LV_GRID_FR(2), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST };	// занимаем 1/2 высоты родителя

    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);

	lv_obj_add_style(cont, & xxcellstyle, 0);


	lv_obj_t * const upper = lv_sscp2_create(cont);
	lv_obj_set_grid_cell(upper, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
	lv_obj_add_style(upper, & xxcellstyle, 0);

	lv_obj_t * const lower = lv_wtrf2_create(cont);
	lv_obj_set_grid_cell(lower, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
	lv_obj_add_style(lower, & xxcellstyle, 0);

	uiupdates(cont);
	return cont;
#endif
}

static lv_obj_t * dzi_create_middlemenu(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
    static int32_t col_dsc0 [] =
    {
		LV_GRID_FR(8), LV_GRID_FR(8), LV_GRID_FR(8), LV_GRID_FR(8),
		LV_GRID_FR(8), LV_GRID_FR(8), LV_GRID_FR(8), LV_GRID_FR(8),
		LV_GRID_TEMPLATE_LAST
    };
    static int32_t row_dsc0 [] =
    {
    	LV_GRID_FR(1),		// занимаем всю высоту родителя
		LV_GRID_TEMPLATE_LAST
    };

    lv_obj_t * cont0 = lv_obj_create(parent);
    lv_obj_set_layout(cont0, LV_LAYOUT_GRID);
    lv_obj_set_style_grid_column_dsc_array(cont0, col_dsc0, 0);
    lv_obj_set_style_grid_row_dsc_array(cont0, row_dsc0, 0);

	lv_obj_add_style(cont0, & xxcellstyle, 0);

	unsigned col;
	for (col = 0; col < ARRAY_SIZE(col_dsc0) - 1; ++ col)
	{

	    static int32_t col_dsc[] = { LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };				// занимаем всю ширину родителя
	    static int32_t row_dsc[] = { LV_GRID_FR(2), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST };	// занимаем 1/2 высоты родителя

	    lv_obj_t * cont = lv_obj_create(cont0);

		lv_obj_add_style(cont, & xxcellstyle, 0);

	    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
	    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
	    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
		lv_obj_set_grid_cell(cont, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

		// нижняя и верхняя строки в элементе горизонтального меню
		lv_obj_t * const upper = lv_label_create(cont);
		lv_obj_set_grid_cell(upper, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
		lv_obj_add_style(upper, & xxdivstyle, 0);
		{
			uint_fast8_t active;
			const char * const label = hamradio_midlabel5(col, & active);
			lv_label_set_text(upper, label);
			//lv_label_set_text_fmt(upper, "up%u", col);
		}

		lv_obj_t * const lower = lv_label_create(cont);
		lv_obj_set_grid_cell(lower, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
		lv_obj_add_style(lower, & xxdivstyle, 0);
		{
			uint_fast8_t active;
			const char * const label = hamradio_midvalue5(col, & active);
			lv_label_set_text(lower, label);
			//lv_label_set_text_fmt(lower, "dn%u", col);
		}

	}
	uiupdates(cont0);
	return cont0;
}

static lv_obj_t * dzi_create_encf1(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	char buf [32];
	hamradio_get_label_ENC1F(0, buf, ARRAY_SIZE(buf));
	lv_obj_t * lbl = lv_label_create(parent);
	lv_obj_add_style(lbl, & xxdivstyle, 0);
	lv_label_set_text(lbl, buf);
	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_encf2(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	char buf [32];
	hamradio_get_label_ENC2F(0, buf, ARRAY_SIZE(buf));
	lv_obj_t * lbl = lv_label_create(parent);
	lv_obj_add_style(lbl, & xxdivstyle, 0);
	lv_label_set_text(lbl, buf);
	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_encf3(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	char buf [32];
	hamradio_get_label_ENC3F(0, buf, ARRAY_SIZE(buf));
	lv_obj_t * lbl = lv_label_create(parent);
	lv_obj_add_style(lbl, & xxdivstyle, 0);
	lv_label_set_text(lbl, buf);
	uiupdates(lbl);
	return lbl;
}

static lv_obj_t * dzi_create_encf4(lv_obj_t * parent, const struct dzone * dzp, const dzitem_t * dzip, unsigned i)
{
	char buf [32];
	hamradio_get_label_ENC4F(0, buf, ARRAY_SIZE(buf));
	lv_obj_t * lbl = lv_label_create(parent);
	lv_obj_add_style(lbl, & xxdivstyle, 0);
	lv_label_set_text(lbl, buf);
	uiupdates(lbl);
	return lbl;
}

#define LVCREATE(fn) (fn)

#else /* WITHLVGL */

#define LVCREATE(fn) (NULL)

#endif /* WITHLVGL */


#if ! WITHTOUCHGUI && ! LINUX_SUBSYSTEM

void gui_update(void)
{

}

#endif

static const dzitem_t dzi_default =
{
	.lvelementcreate = LVCREATE(dzi_create_default),
	.id = "default"
};

// для быстрого перехода на систему ввода LVGL отрисовка этих widgets
// производится вызовом старых функций
static const dzitem_t dzi_compat =
{
	.lvelementcreate = LVCREATE(dzi_create_compat),
	//.id = "compat"
};

static const dzitem_t dzi_menu =
{
	.lvelementcreate = LVCREATE(dzi_create_menu),
	.id = "compmenuat"
};

static const dzitem_t dzi_middlemenu =
{
	.lvelementcreate = LVCREATE(dzi_create_middlemenu),
	.id = "middlemenu"
};

static const dzitem_t dzi_encf1 =
{
	.lvelementcreate = LVCREATE(dzi_create_encf1),
	.id = "encf1"
};

static const dzitem_t dzi_encf2 =
{
	.lvelementcreate = LVCREATE(dzi_create_encf2),
	.id = "encf2"
};

static const dzitem_t dzi_encf3 =
{
	.lvelementcreate = LVCREATE(dzi_create_encf3),
	.id = "encf3"
};

static const dzitem_t dzi_encf4 =
{
	.lvelementcreate = LVCREATE(dzi_create_encf4),
	.id = "encf4"
};

static const dzitem_t dzi_GUI =
{
	.lvelementcreate = LVCREATE(dzi_create_GUI),
	.id = "GUI"
};

static const dzitem_t dzi_freqa =
{
	.lvelementcreate = LVCREATE(dzi_create_freqa),
	.id = "freq-a"
};

static const dzitem_t dzi_freqb =
{
	.lvelementcreate = LVCREATE(dzi_create_freqb),
	.id = "freq-b"
};

static const dzitem_t dzi_txrx =
{
	.lvelementcreate = LVCREATE(dzi_create_txrx),
	.id = "txrx"
};

static const dzitem_t dzi_smtr2 =
{
	.lvelementcreate = LVCREATE(dzi_create_smtr2),
	.id = "smeter"
};

static const dzitem_t dzi_gcombo =
{
	.lvelementcreate = LVCREATE(dzi_create_gcombo),
	.id = "gcombo"
};

static const dzitem_t dzi_3dss =
{
	.lvelementcreate = LVCREATE(dzi_create_3dss),
	.id = "3dss"
};

static const dzitem_t dzi_modea =
{
	.lvelementcreate = LVCREATE(dzi_create_modea),
	.id = "mode-a"
};

static const dzitem_t dzi_modeb =
{
	.lvelementcreate = LVCREATE(dzi_create_modeb),
	.id = "mode-b"
};

static const dzitem_t dzi_antenna =
{
	.lvelementcreate = LVCREATE(dzi_create_antenna),
	.id = "ant"
};

static const dzitem_t dzi_attenuator =
{
	.lvelementcreate = LVCREATE(dzi_create_attenuator),
	.id = "ant"
};

static const dzitem_t dzi_preamp_ovf =
{
	.lvelementcreate = LVCREATE(dzi_create_preamp_ovf),
	.id = "ant"
};

static const dzitem_t dzi_tune =
{
	.lvelementcreate = LVCREATE(dzi_create_tune),
	.id = "ant"
};

static const dzitem_t dzi_bypass =
{
	.lvelementcreate = LVCREATE(dzi_create_bypass),
	.id = "byp"
};

// nar/wid/nor
static const dzitem_t dzi_rxbw =
{
	.lvelementcreate = LVCREATE(dzi_create_rxbw),
	.id = "rxbw"
};

// 3.1k
static const dzitem_t dzi_rxbwval =
{
	.lvelementcreate = LVCREATE(dzi_create_rxbwval),
	.id = "rxbwval"
};

// 3.1k
static const dzitem_t dzi_siglevel =
{
	.lvelementcreate = LVCREATE(dzi_create_siglevel),
	.id = "siglevel"
};

// 12.7V
static const dzitem_t dzi_voltlevel =
{
	.lvelementcreate = LVCREATE(dzi_create_voltlevel),
	.id = "voltlevel"
};

// 12.7V
static const dzitem_t dzi_datetime12 =
{
	.lvelementcreate = LVCREATE(dzi_create_datetime12),
	.id = "datetime12"
};

// 6.3A
static const dzitem_t dzi_currlevel =
{
	.lvelementcreate = LVCREATE(dzi_create_currlevel),
	.id = "currlevel"
};
static const dzitem_t dzi_thermo =
{
	.lvelementcreate = LVCREATE(dzi_create_thermo),
	.id = "thermo"
};

static const dzitem_t dzi_rec =
{
	.lvelementcreate = LVCREATE(dzi_create_rec),
	.id = "rec"
};

static const dzitem_t dzi_lock =
{
	.lvelementcreate = LVCREATE(dzi_create_lock),
	.id = "lock"
};

static const dzitem_t dzi_notch =
{
	.lvelementcreate = LVCREATE(dzi_create_notch),
	.id = "notch"
};

static const dzitem_t dzi_agc =
{
	.lvelementcreate = LVCREATE(dzi_create_agc),
	.id = "agc"
};

static const dzitem_t dzi_spk =
{
	.lvelementcreate = LVCREATE(dzi_create_spk),
	.id = "spk"
};

static const dzitem_t dzi_bkin =
{
	.lvelementcreate = LVCREATE(dzi_create_bkin),
	.id = "bkin"
};

static const dzitem_t dzi_usbact =
{
	.lvelementcreate = LVCREATE(dzi_create_usbact),
	.id = "usbact"
};

static const dzitem_t dzi_btact =
{
	.lvelementcreate = LVCREATE(dzi_create_btact),
	.id = "bt"
};

static const dzitem_t dzi_wpm =
{
	.lvelementcreate = LVCREATE(dzi_create_wpm),
	.id = "wpm"
};

static const dzitem_t dzi_vfomode =
{
	.lvelementcreate = LVCREATE(dzi_create_vfomode),
	.id = "vfomode"
};

static const dzitem_t dzi_nr =
{
	.lvelementcreate = LVCREATE(dzi_create_nr),
	.id = "nr"
};

static const dzitem_t dzi_classa =
{
	.lvelementcreate = LVCREATE(dzi_create_classa),
	.id = "classa"
};

static const dzitem_t dzi_datamode =
{
	.lvelementcreate = LVCREATE(dzi_create_datamode),
	.id = "datamode"
};

static const dzitem_t dzi_voxtune =
{
	.lvelementcreate = LVCREATE(dzi_create_voxtune),
	.id = "voxtune"
};


#if WITHALTERNATIVEFONTS
	#include "display/fonts/ub_fonts.h"
#endif /* WITHALTERNATIVEFONTS */

#if WITHRLEDECOMPRESS
	#include "display/pictures_RLE.h"
#endif /* WITHRLEDECOMPRESS */

#if LCDMODE_LTDC

	#include "display/fontmaps.h"

	// получить адрес в видеобуфере, соответствующий ячейке
	static PACKEDCOLORPIP_T * getscratchwnd(
		const gxdrawb_t * db,
		uint_fast8_t x0,
		uint_fast8_t y0
	);

#elif WITHSPECTRUMWF

	// получить адрес в видеобуфере, соответствующий ячейке
	static PACKEDCOLORPIP_T * getscratchwnd(
		const gxdrawb_t * db,
		uint_fast8_t x0,
		uint_fast8_t y0
	);

#endif /* LCDMODE_LTDC */

static const COLORPIP_T colors_2state_alt [2] = { COLORPIP_GRAY, COLORPIP_WHITE, };
static const COLORPIP_T color_alt_red = COLORPIP_RED;

// Стирание фона (REDRSUBSET_SHOW)
static void display2_preparebg(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast8_t colspan, uint_fast8_t rowspan, dctx_t * pctx)
{
#if WITHLVGL
	return;
#endif /* WITHLVGL */
#if WITHLTDCHW
	colpip_fillrect(db, 0, 0, DIM_X, DIM_Y, display2_getbgcolor());
#endif
}

// запись подготовленного изображения на главный дисплей (REDRSUBSET_SHOW)
static void display2_showmain(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t colspan, uint_fast8_t rowspan, dctx_t * pctx)
{
#if WITHLVGL
	return;
#endif /* WITHLVGL */
#if WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC
	display_snapshot(db);	/* запись видимого изображения */
#endif /* WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC */
#if WITHLTDCHW
	colmain_nextfb();
#endif
}

// запись подготовленного изображения на второй дисплей (REDRSUBSET_SHOW)
static void display2_showhdmi(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast8_t colspan, uint_fast8_t rowspan, dctx_t * pctx)
{
#if WITHLVGL
	return;
#endif /* WITHLVGL */
#if WITHLTDCHW
	colmain_nextfb_sub();
#endif
}

void layout_label1_medium(const gxdrawb_t * db, uint_fast8_t xgrid, uint_fast8_t ygrid, const char * str, size_t slen, uint_fast8_t chars_W2, COLORPIP_T color_fg, COLORPIP_T color_bg);


#if WITHALTERNATIVELAYOUT

#if SMALLCHARW2

typedef struct {
	uint_fast8_t chars_W2;
	const COLORPIP_T * pcolor;
	PACKEDCOLORPIP_T * label_bg;	/* буфер */
	size_t size;					/* размер для cache flush */
	uint_fast16_t w;
	uint_fast16_t h;
	gxdrawb_t dbv;
} label_bg_t;

static label_bg_t label_bg [] = {
		{ 5, & colors_2state_alt [0], },
		{ 5, & colors_2state_alt [1], },
		{ 5, & color_alt_red, 		  },
		{ 7, & colors_2state_alt [0], },
		{ 7, & colors_2state_alt [1], },
};

void layout_init(const gxdrawb_t * db, uint_fast8_t xgrid, uint_fast8_t ygrid, uint_fast8_t colspan, uint_fast8_t rowspan, dctx_t * pctx)
{
	uint_fast8_t i = 0;

	do {
		label_bg_t * const lbl = & label_bg [i];
		lbl->w = lbl->chars_W2 * SMALLCHARW2;
		lbl->h = SMALLCHARH2 + 6;
		lbl->size = GXSIZE(lbl->w, lbl->h) * sizeof (PACKEDCOLORPIP_T);
		lbl->label_bg = (PACKEDCOLORPIP_T *) malloc(lbl->size);
		ASSERT(lbl->label_bg != NULL);
		gxdrawb_initialize(& lbl->dbv, lbl->label_bg, lbl->w, lbl->h);
		colpip_fillrect(& lbl->dbv, 0, 0, lbl->w, lbl->h, COLORPIP_BLACK);
		colmain_rounded_rect(& lbl->dbv, 0, 0, lbl->w - 1, lbl->h - 1, 5, * lbl->pcolor, 1);
	} while (++ i < ARRAY_SIZE(label_bg));
}

void layout_label1_medium(const gxdrawb_t * db, uint_fast8_t xgrid, uint_fast8_t ygrid, const char * str, size_t slen, uint_fast8_t chars_W2, COLORPIP_T color_fg, COLORPIP_T color_bg)
{
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
		colpip_bitblt(db->cachebase, db->cachesize, db,
				xx, yy,	// олучаткль - позиция
				lbl_bg->dbv.cachebase, lbl_bg->dbv.cachesize,
				& lbl_bg->dbv,
				0, 0,	// координаты окна источника
				lbl_bg->w, lbl_bg->h, //размер окна источника
				BITBLT_FLAG_NONE, 0);
	}
	else
	{
		colmain_rounded_rect(db, xx, yy, xx + width_p, yy + SMALLCHARH2 + 5, 5, color_bg, 1);
	}

	//ASSERT(width_p >= width_str);
	if (width_p >= width_str)
	{
#if WITHALTERNATIVEFONTS
		//PRINTF("%s: xx=%d, width_p=%d, width_str=%d, buf='%s'\n", __func__, xx, width_p, width_str, buf);
		UB_Font_DrawPString(db, xx + (width_p - width_str) / 2 , yy + 2, buf, & gothic_12x16_p, color_fg);
#else
		colpip_string2_tbg(db, xx + (width_p - width_str) / 2 , yy + 4, buf, color_fg);
#endif /* WITHALTERNATIVEFONTS */
	}

}
#endif /* SMALLCHARW2 */
#else
void layout_label1_medium(const gxdrawb_t * db, uint_fast8_t xgrid, uint_fast8_t ygrid, const char * str, size_t slen, uint_fast8_t chars_W2, COLORPIP_T color_fg, COLORPIP_T color_bg)
{
}
#endif /* WITHALTERNATIVELAYOUT */


// todo: switch off -Wunused-function

// формирование данных спектра для последующего отображения
// спектра или водопада
static void display2_gcombo(const gxdrawb_t * db, uint_fast8_t xgrid, uint_fast8_t ygrid, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx);
static void display2_latchcombo(const gxdrawb_t * db, uint_fast8_t xgrid, uint_fast8_t ygrid, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx);
static void display2_wfl_init(const gxdrawb_t * db, uint_fast8_t xgrid, uint_fast8_t ygrid, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx);
static void display2_spectrum(const gxdrawb_t * db, uint_fast8_t xgrid, uint_fast8_t ygrid, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx);
static void display2_waterfall(const gxdrawb_t * db, uint_fast8_t xgrid, uint_fast8_t ygrid, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx);
// Отображение шкалы S-метра и других измерителей
static void display2_legend(const gxdrawb_t * db, uint_fast8_t xgrid, uint_fast8_t ygrid, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx);
// Отображение шкалы S-метра
static void display2_legend_rx(const gxdrawb_t * db, uint_fast8_t xgrid, uint_fast8_t ygrid, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx);
// Отображение шкалы SWR-метра и других измерителе
static void display2_legend_tx(const gxdrawb_t * db, uint_fast8_t xgrid, uint_fast8_t ygrid, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx);

static void display2_af_spectre15_init(const gxdrawb_t * db, uint_fast8_t xgrid, uint_fast8_t ygrid, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx);		// вызывать после display2_smeter15_init
static void display2_af_spectre15_latch(const gxdrawb_t * db, uint_fast8_t xgrid, uint_fast8_t ygrid, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx);
static void display2_af_spectre15(const gxdrawb_t * db, uint_fast8_t xgrid, uint_fast8_t ygrid, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx);

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
#endif /* WITHGRADIENT_FIXED */
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

// Параметры отображения состояний из двух вариантов (RX/TX).
static gxstyle_t dbstylev_2rxtx [2];
// Параметры отображения состояний из четырех вариантов
static gxstyle_t dbstylev_4state [4];
// Параметры отображения состояний из двух вариантов
static gxstyle_t dbstylev_2state [2];
// Параметры отображения состояний из двух вариантов
static gxstyle_t dbstylev_2stateSmall [2];
// Параметры отображения состояний из двух вариантов (активный - на красном фонк)
static gxstyle_t dbstylev_2state_rec [2];
// Параметры отображения текстов без вариантов
static gxstyle_t dbstylev_1state;
// Параметры отображения текстов без вариантов
static gxstyle_t dbstylev_1statePSU;
// Параметры отображения состояний FUNC MENU из двух вариантов
static gxstyle_t dbstylev_2fmenu [2];
// Параметры отображения текстов без вариантов
static gxstyle_t dbstylev_1fmenu;
// Параметры отображения текстов без вариантов (синий)
static gxstyle_t dbstylev_1stateTime;
// Параметры отображения частоты и режима дополнительного приемника
static gxstyle_t dbstylev_2rxB [2];
// Параметры отображения частоты основного приемника
static gxstyle_t dbstylev_1freqv;

// Параметры отображения спектра и водопада - устанавливаются в display2_stylesupdate().
static COLORPIP_T colorcmarker = DSGN_GRIDCOLOR0;	// Цвет макркера на центре
static COLORPIP_T colorgridlines = DSGN_GRIDCOLOR2;	// Цвет линий сетки
static COLORPIP_T colorgridlines3dss = COLORPIP_GREEN;
static COLORPIP_T colordigits = DSGN_GRIDDIGITS;	// Цвет текста частот сетки

#if 1
	// полностью частота до килогерц
	static int_fast16_t glob_gridstep = 10000; //1 * glob_griddigit;	// 10, 20. 50 kHz - шаг сетки для рисования
	static const int_fast16_t glob_griddigit = 1000;	// 1 kHz - точность отображения частоты на сетке
	static const int glob_gridwc = 1;
	static const int_fast32_t glob_gridmod = INT32_MAX;	// 10 ^ glob_gridwc
	static const char  gridfmt_2 [] = "%*ld";
#else
	// Дестки килогерц без мегагерц
	static int_fast16_t glob_gridstep = 10000; //1 * glob_griddigit;	// 10, 20. 50 kHz - шаг сетки для рисования
	static const int_fast16_t glob_griddigit = 10000;	// 10 kHz - точность отображения частоты на сетке
	static const int glob_gridwc = 2;
	static const int_fast32_t glob_gridmod = 1;	// 10 ^ glob_gridwc
	static const char  gridfmt_2 [] = ".%0*ld";
#endif
	static unsigned gbwalpha = 128;	/* Полупрозрачность при отрисовке прямоугольникв ("шторки") полосы пропускания на водопаде. */

// waterfall/spectrum parameters
static uint_fast8_t glob_view_style = VIEW_COLOR;		/* стиль отображения спектра и панорамы */
static uint_fast8_t gview3dss_mark;			/* Для VIEW_3DSS - индикация полосы пропускания на спектре */
static uint_fast8_t glob_rxbwsatu = 100;	// 0..100 - насыщнность цвета заполнения "шторки" - индикатор полосы пропускания примника на спкктре.

static int_fast16_t glob_topdb = - 30;		/* absolute power - верхний предел FFT */
static int_fast16_t glob_bottomdb = - 130;	/* absolute power - нижний предел FFT */
static uint_fast8_t glob_zoomxpow2;			/* уменьшение отображаемого участка спектра - horizontal magnification power of two */

static uint_fast8_t glob_showdbm = 1;		// Отображение уровня сигнала в dBm или S-memter (в зависимости от настроек)

static uint_fast8_t glob_smetertype = SMETER_TYPE_DIAL;	/* выбор внешнего вида прибора - стрелочный или градусник */

static int_fast16_t glob_afspeclow = 300;	// нижняя частота отображения спектроанализатора
static int_fast16_t glob_afspechigh = 3400;	// верхняя частота отображения спектроанализатора

static int glob_lvlgridstep = 12;	// Шаг сетки уровней в децибелах. (0-отключаем отображение сетки уровней)

static uint_fast8_t glob_spectrumpart = 50;

static uint_fast8_t glob_displayfps = 15;

//#define WIDEFREQ (TUNE_TOP > 100000000L)

static void fftzoom_af(FLOAT_t * buffer, unsigned zoompow2, unsigned normalFFT);

uint_fast16_t normalize(
	uint_fast16_t raw,
	uint_fast16_t rawmin,	// включает интервал входного raw
	uint_fast16_t rawmax,	// включает интервал входного raw
	uint_fast16_t range		// включает выходное значение
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


uint_fast16_t approximate(
	const int16_t * points,		// массив позиций входных значений
	const uint_fast16_t * angles,		// массив позицый выходных значений
	unsigned n,					// размерность массивов
	int_fast16_t v				// значение для анализа
	)
{
	unsigned i;
	if (n < 2)
		return 0;
	for (i = 0; i < (n - 1); ++ i)
	{
		const int_fast16_t left = points [i];
		const int_fast16_t right = points [i + 1];
		const uint_fast16_t minimal = angles [i];
		const uint_fast16_t maximal = angles [i + 1];
		ASSERT(left < right);
		ASSERT(minimal < maximal);
		if (v < left)
			return minimal;
		if (v > right)
			continue;
		const uint_fast16_t offset = v - left;
		const uint_fast16_t range = right - left + 1;	// диапазон входных значение на данном участке
		const uint_fast16_t delta = maximal - minimal + 1;	// диапазон выходных значение на данном участке
		return (uint_fast32_t) offset * delta / range + minimal;
	}
	return angles [n - 1];	// при выходе за максимальное значение - уприраемся в правое значение
}

static uint_fast8_t first_tx = 0;

#if LCDMODE_LTDC && WITHBARS

enum {
	SM_STATE_RX,
	SM_STATE_TX,
	//
	SM_STATE_COUNT
};

// Размеры S-метра
#define SM_BG_W_CELLS 15
#define SM_BG_H_CELLS 18
#define SM_YCENTEROFFS 120	// Расстояние от верха поля до оси стрелки S-метра

enum { SM_BG_W = GRID2X(SM_BG_W_CELLS), SM_BG_H = GRID2Y(SM_BG_H_CELLS) };


/* точки на шкале s-метра, к которым надо привязать измеренное значение */
static const int16_t smeterpointsRX [] =
{
	- 1270,	// S0 level -127.0 dBm
	//- 1090,	// S3 level -109.0 dBm
	- 730,	// S9 level -73.0 dBm
	- 130,	// S9+60 level -13.0 dBm
};

typedef PACKEDCOLORPIP_T smeter_bg_t [GXSIZE(SM_BG_W, SM_BG_H)];

typedef struct smeter_params_tag
{
	ALIGNX_BEGIN smeter_bg_t smeter_bg ALIGNX_END;
	uint_fast16_t gs;
	uint_fast16_t gm;
	uint_fast16_t ge;
	uint_fast16_t smeterangles [ARRAY_SIZE(smeterpointsRX)];	// gs, gm, ge
	gxdrawb_t smbgdb;

	uint_fast16_t r1;
	uint_fast16_t r2;
	uint_fast16_t rv1;
	uint_fast16_t rv2;
	uint_fast8_t step1;
	uint_fast8_t step2;
	uint_fast8_t step3;
} smeter_params_t;

static smeter_params_t smprms [SMETER_TYPE_COUNT] [SM_STATE_COUNT];

static void
display2_smeter15_layout_dial(smeter_params_t * smpr)
{
	const uint_fast8_t halfsect = 30;	// Градусы
	const int stripewidth = 12; //16;
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

static void
display2_smeter15_layout_bars(smeter_params_t * smpr)
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

static void
display2_smeter15_layout_tx(
	smeter_params_t * const smpr,
	uint_fast8_t smetertype
	)
{
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

	const COLORPIP_T bgcolor = display2_getbgcolor();
	const COLORPIP_T smetercolor = COLORPIP_WHITE;
	const COLORPIP_T smeterpluscolor = COLORPIP_DARKRED;
	const uint_fast16_t pad2w3 = strwidth3("ZZ");

	// SMETER_TYPE_DIAL data
	// координаты оси стрелки
	const uint_fast16_t xb = SM_BG_W / 2, yb = SM_YCENTEROFFS;	// In pixels
	unsigned p;
	unsigned i;

	// Raster buffers
	gxdrawb_t * const db = & smpr->smbgdb;
	gxdrawb_initialize(db, smpr->smeter_bg, SM_BG_W, SM_BG_H);
	// Angles (positions)
	uint_fast16_t * const smeteranglesTX = smpr->smeterangles;
	switch (smetertype)
	{

	case SMETER_TYPE_DIAL:
		// Фон для TX
#if WITHRLEDECOMPRESS
		graw_picture_RLE_buf(db, 0, 0, & smeter_bg_new, bgcolor);
#else
		colpip_fillrect(db, 0, 0, SM_BG_W, SM_BG_H, bgcolor);

		for (p = 0, i = 0; i < ARRAY_SIZE(markersTX_pwr) - 1; ++ i, p += 10)
		{
			if (i % 2 == 0)
			{
				char buf2 [10];
				uint_fast16_t xx, yy;

				colpip_radius(db, xb, yb, markersTX_pwr [i], smpr->r1, smpr->r1 + 8, smetercolor, 1, 1);
				polar_to_dek(xb, yb, markersTX_pwr [i], smpr->r1 + 6, & xx, & yy, 1);
				local_snprintf_P(buf2, ARRAY_SIZE(buf2), "%u", p);
				colpip_string3_tbg(db, xx - strwidth3(buf2) / 2, yy - pad2w3 + 1, buf2, COLORPIP_YELLOW);
			}
			else
				colpip_radius(db, xb, yb, markersTX_pwr [i], smpr->r1, smpr->r1 + 4, smetercolor, 1, 1);
		}

		for (p = 1, i = 0; i < ARRAY_SIZE(markersTX_swr); ++ i, p += 1)
		{
			char buf2 [10];
			uint_fast16_t xx, yy;

			colpip_radius(db, xb, yb, markersTX_swr [i], smpr->r2, smpr->r2 - 8, smetercolor, 1, 1);
			polar_to_dek(xb, yb, markersTX_swr [i], smpr->r2 - 16, & xx, & yy, 1);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), "%u", p);
			colpip_string3_tbg(db, xx - SMALLCHARW3 / 2, yy - SMALLCHARW3 / 2 + 1, buf2, COLORPIP_YELLOW);
		}
		colpip_segm(db, xb, yb, smpr->gs, smpr->gm, smpr->r1, 1, smetercolor, 1, 1);
		colpip_segm(db, xb, yb, smpr->gm, smpr->ge, smpr->r1, 1, smetercolor, 1, 1);
		colpip_segm(db, xb, yb, smpr->gs, smpr->ge, smpr->r2, 1, COLORPIP_WHITE, 1, 1);
#endif /* WITHRLEDECOMPRESS */
		break;

	default:
	case SMETER_TYPE_BARS:
		// Фон для TX
		colpip_rect(db, 0, 0, SM_BG_W - 1, SM_BG_H - 1, bgcolor, 1);
//		colpip_rect(db, 0, 0, SM_BG_W - 1, SM_BG_H - 1, COLORPIP_WHITE, 0);
//		colpip_rect(db, 0, 0, SM_BG_W - 1, SM_BG_H / 2, COLORPIP_WHITE, 0);

		colpip_line(db, smpr->gs, smpr->r1, smpr->ge, smpr->r1, COLORPIP_WHITE, 0);
		for (p = 0, i = 0; i < ARRAY_SIZE(markersTX_pwr); ++ i, p += 10)
		{
			if (i % 2 == 0)
			{
				char buf2 [10];
				colpip_line(db, markersTX_pwr [i], smpr->r1, markersTX_pwr [i], smpr->r1 - 10, COLORPIP_WHITE, 0);
				local_snprintf_P(buf2, ARRAY_SIZE(buf2), "%u", p);
				colpip_string3_tbg(db, markersTX_pwr [i] - strwidth3(buf2) / 2, smpr->r1 - 10 - SMALLCHARH3 - 2, buf2, COLORPIP_YELLOW);
			}
			else
				colpip_line(db, markersTX_pwr [i], smpr->r1, markersTX_pwr [i], smpr->r1 - 5, COLORPIP_WHITE, 0);
		}

		colpip_line(db, smpr->gs, smpr->r2, smpr->ge, smpr->r2, COLORPIP_WHITE, 0);
		for (p = 1, i = 0; i < ARRAY_SIZE(markersTX_swr); ++ i, p += 1)
		{
			char buf2 [10];
			colpip_line(db, markersTX_swr [i], smpr->r2, markersTX_swr [i], smpr->r2 + 10, COLORPIP_WHITE, 0);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), "%u", p);
			colpip_string3_tbg(db, markersTX_swr [i] - strwidth3(buf2) / 2, smpr->r2 + 12, buf2, COLORPIP_YELLOW);
		}
		break;
	}
	dcache_clean(db->cachebase, db->cachesize);
	smeteranglesTX [0] = smpr->gs;
	smeteranglesTX [1] = smpr->gm;
	smeteranglesTX [2] = smpr->ge;
}

static void
display2_smeter15_layout_rx(
	smeter_params_t * const smpr,
	uint_fast8_t smetertype
	)
{
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

	const COLORPIP_T bgcolor = display2_getbgcolor();
	const COLORPIP_T smetercolor = COLORPIP_WHITE;
	const COLORPIP_T smeterpluscolor = COLORPIP_DARKRED;
	const uint_fast16_t pad2w3 = strwidth3("ZZ");

	// SMETER_TYPE_DIAL data
	// координаты оси стрелки
	const uint_fast16_t xb = SM_BG_W / 2, yb = SM_YCENTEROFFS;	// In pixels
	unsigned p;
	unsigned i;

	// Raster buffers
	gxdrawb_t * const db = & smpr->smbgdb;
	gxdrawb_initialize(db, smpr->smeter_bg, SM_BG_W, SM_BG_H);
	// Angles (positions)
	uint_fast16_t * const smeteranglesRX = smpr->smeterangles;
	switch (smetertype)
	{

	case SMETER_TYPE_DIAL:
		// Фон для RX
#if WITHRLEDECOMPRESS
		graw_picture_RLE_buf(db, 0, 0, & smeter_bg_new, bgcolor);
#else
		colpip_fillrect(db, 0, 0, SM_BG_W, SM_BG_H, bgcolor);

		for (p = 1, i = 0; i < ARRAY_SIZE(markers); ++ i, p += 2)
		{
			char buf2 [10];
			uint_fast16_t xx, yy;

			colpip_radius(db, xb, yb, markers [i], smpr->r1, smpr->r1 + 8, smetercolor, 1, 1);
			polar_to_dek(xb, yb, markers [i], smpr->r1 + 6, & xx, & yy, 1);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), "%u", p);
			colpip_string3_tbg(db, xx - SMALLCHARW3 / 2, yy - pad2w3 + 1, buf2, COLORPIP_YELLOW);
		}
		for (i = 0; i < ARRAY_SIZE(markers2); ++ i)
		{
			colpip_radius(db, xb, yb, markers2 [i], smpr->r1, smpr->r1 + 4, smetercolor, 1, 1);
		}

		for (p = 20, i = 0; i < ARRAY_SIZE(markersR); ++ i, p += 20)
		{
			char buf2 [10];
			uint_fast16_t xx, yy;

			colpip_radius(db, xb, yb, markersR [i], smpr->r1, smpr->r1 + 8, smeterpluscolor, 1, 1);
			polar_to_dek(xb, yb, markersR [i], smpr->r1 + 6, & xx, & yy, 1);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), "+%u", p);
			colpip_string3_tbg(db, xx - strwidth3(buf2) / 2, yy - pad2w3 + 1, buf2, COLORPIP_RED);
		}
		for (i = 0; i < ARRAY_SIZE(markers2R); ++ i)
		{
			colpip_radius(db, xb, yb, markers2R [i], smpr->r1, smpr->r1 + 4, smeterpluscolor, 1, 1);
		}
		colpip_segm(db, xb, yb, smpr->gs, smpr->gm, smpr->r1, 1, smetercolor, 1, 1);
		colpip_segm(db, xb, yb, smpr->gm, smpr->ge, smpr->r1, 1, smeterpluscolor, 1, 1);
		colpip_segm(db, xb, yb, smpr->gs, smpr->ge, smpr->r2, 1, COLORPIP_WHITE, 1, 1);
#endif /* WITHRLEDECOMPRESS */
		break;

	default:
	case SMETER_TYPE_BARS:
		// Фон для RX

		colpip_rect(db, 0, 0, SM_BG_W - 1, SM_BG_H - 1, bgcolor, 1);
//		colpip_rect(db, 0, 0, SM_BG_W - 1, SM_BG_H - 1, COLORPIP_WHITE, 0);
//		colpip_rect(db, 0, 0, SM_BG_W - 1, SM_BG_H / 2, COLORPIP_WHITE, 0);

		colpip_line(db, smpr->gs, smpr->r1, smpr->gm, smpr->r1, COLORPIP_WHITE, 0);
		colpip_line(db, smpr->gm, smpr->r1, smpr->ge, smpr->r1, COLORPIP_RED, 0);
		colpip_string2_tbg(db, smpr->gs - SMALLCHARW2, smpr->r1 - SMALLCHARH2 - 2, "Sm", COLORPIP_YELLOW);

		for (p = 1, i = 0; i < ARRAY_SIZE(markers); ++ i, p += 2)
		{
			char buf2 [10];
			uint_fast16_t xx, yy;
			colpip_line(db, markers [i], smpr->r1, markers [i], smpr->r1 - 10, COLORPIP_WHITE, 0);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), "%u", p);
			colpip_string3_tbg(db, markers [i] - SMALLCHARW3 / 2, smpr->r1 - 10 - SMALLCHARH3 - 2, buf2, COLORPIP_YELLOW);
		}
		for (i = 0; i < ARRAY_SIZE(markers2); ++ i)
		{
			colpip_line(db, markers2 [i], smpr->r1, markers2 [i], smpr->r1 - 5, COLORPIP_WHITE, 0);
		}

		for (p = 20, i = 0; i < ARRAY_SIZE(markersR); ++ i, p += 20)
		{
			char buf2 [10];
			colpip_line(db, markersR [i], smpr->r1, markersR [i], smpr->r1 - 10, COLORPIP_RED, 0);
			local_snprintf_P(buf2, ARRAY_SIZE(buf2), "+%u", p);
			colpip_string3_tbg(db, markersR [i] - strwidth3(buf2) / 2, smpr->r1 - 10 - SMALLCHARH3 - 2, buf2, COLORPIP_YELLOW);
		}
		for (i = 0; i < ARRAY_SIZE(markers2R); ++ i)
		{
			colpip_line(db, markers2R [i], smpr->r1, markers2R [i], smpr->r1 - 5, COLORPIP_RED, 0);
		}
		break;
	}

	dcache_clean(db->cachebase, db->cachesize);
	smeteranglesRX [0] = smpr->gs;
	smeteranglesRX [1] = smpr->gm;
	smeteranglesRX [2] = smpr->ge;
}

static uint_fast8_t smprmsinited;

static void
display2_smeter15_init(
		const gxdrawb_t * db_unused,	// NULL
		uint_fast8_t xgrid_unused,
		uint_fast8_t ygrid_unused,
		uint_fast8_t xspan_unused,	// 0
		uint_fast8_t yspan_unused,	// 0
		dctx_t * pctx
		)
{
	if (smprmsinited)
			return;

	for (uint_fast8_t i = 0; i < SMETER_TYPE_COUNT; ++ i)
	{
		switch (i)
		{
		case SMETER_TYPE_DIAL:
			display2_smeter15_layout_dial(& smprms [i] [SM_STATE_TX]);
			display2_smeter15_layout_dial(& smprms [i] [SM_STATE_RX]);
			break;

		default:
		case SMETER_TYPE_BARS:
			display2_smeter15_layout_bars(& smprms [i] [SM_STATE_TX]);
			display2_smeter15_layout_bars(& smprms [i] [SM_STATE_RX]);
			break;
		}
		display2_smeter15_layout_tx(& smprms [i] [SM_STATE_TX], i);
		display2_smeter15_layout_rx(& smprms [i] [SM_STATE_RX], i);
	}
	smprmsinited = 1;
}

static void smeter_arrow(const gxdrawb_t * db, uint_fast16_t target_pixel_x, uint_fast16_t x, uint_fast16_t y, uint_fast16_t w, uint_fast16_t h, COLORPIP_T color)
{
	if (target_pixel_x > 220)
		target_pixel_x = 220;
	float32_t x0 = x + w / 2 + 2;
	float32_t y0 = y + h + 140;
	float32_t x1 = x + target_pixel_x;
	float32_t y1 = y;

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
		colpip_line(db, x0, y0, x1, y1, color, 1);
		colpip_line(db, x0 + 1, y0, x1 + 1, y1, color, 1);
	}
	else
	{
		colpip_line(db, x0, y0, x1, y1, color, 1);
		colpip_line(db, x0 - 1, y0, x1 - 1, y1, color, 1);
	}
}

static void
pix_display2_smeter15(const gxdrawb_t * db,
		uint_fast16_t x0,
		uint_fast16_t y0,
		uint_fast16_t width,
		uint_fast16_t height,
		uint_fast8_t pathi	// тракт, испольуемый для показа s-метра
		)
{
	const COLORPIP_T bgcolor = display2_getbgcolor();
	const uint_fast8_t is_tx = hamradio_get_tx();
	const smeter_params_t * const smpr = & smprms [glob_smetertype] [is_tx ? SM_STATE_TX : SM_STATE_RX];
	const uint_fast16_t * const smeterangles = smpr->smeterangles;
	const gxdrawb_t * const smbgdb = & smpr->smbgdb;

	// координаты оси стрелки
	const int xc = x0 + width / 2;
	const int yc = y0 + SM_YCENTEROFFS;


	uint_fast16_t gp = smpr->gs;
	uint_fast16_t gv = smpr->gs;
	uint_fast16_t gv_trace = smpr->gs;
	uint_fast16_t gswr = smpr->gs;

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

		const adcvalholder_t powerV = board_getadc_unfiltered_truevalue(PWRMRRIX);	// без возможных тормозов на SPI при чтении
		gp = smpr->gs + normalize(powerV, 0, maxpwrcali * 16, smpr->ge - smpr->gs);

		// todo: get_swr(swr_fullscale) - использщовать MRRxxx.
		// Для тюнера и измерений не годится, для показа - без торомозов.
		const uint_fast16_t swr_fullscale = (SWRMIN * 40 / 10) - SWRMIN;	// количество рисок в шкале ииндикатора
		const uint_fast16_t swrV = get_swr(swr_fullscale);
		gswr = smpr->gs + normalize(swrV, 0, swr_fullscale, smpr->ge - smpr->gs);

		if (gp > smpr->gs)
			gp_smooth = gp;

		if (gp == smpr->gs && gp_smooth > smpr->gs)
			gp = (gp_smooth -= gx_hyst) > smpr->gs ? gp_smooth : smpr->gs;

		if (gswr > smpr->gs)
			gswr_smooth = gswr;

		if (gswr == smpr->gs && gswr_smooth > smpr->gs && gswr_smooth >= gx_hyst)
			gswr = (gswr_smooth -= gx_hyst) > smpr->gs ? gswr_smooth : smpr->gs;
	}
	else
	{
		int_fast16_t tracemaxi10;
		int_fast16_t rssi10 = dsp_rssi10(& tracemaxi10, pathi);	/* получить значение уровня сигнала для s-метра в 0.1 дБмВт */
		gv = approximate(smeterpointsRX, smeterangles, ARRAY_SIZE(smeterpointsRX), rssi10);
		gv_trace = approximate(smeterpointsRX, smeterangles, ARRAY_SIZE(smeterpointsRX), tracemaxi10);

		first_tx = 1;
	}


	switch (glob_smetertype)
	{

	case SMETER_TYPE_DIAL:

		if (is_tx)
		{
			// TX state
			colpip_bitblt(
					db->cachebase, db->cachesize,
					db, x0, y0,
					smbgdb->cachebase, 0*smbgdb->cachesize,
					smbgdb,
					0, 0,	// координаты окна источника
					SM_BG_W, SM_BG_H, // размер окна источника
					BITBLT_FLAG_NONE, 0);
#if WITHRLEDECOMPRESS
			smeter_arrow(db, gp, x0, y0, smeter_bg_new.width, smeter_bg_new.height, COLOR_GRAY);
#else
			if (gswr > smpr->gs)
			{
				uint_fast16_t xx, yy;
				const COLORPIP_T color = COLORPIP_YELLOW;

				colpip_segm(db, xc, yc, smpr->gs, gswr, smpr->r2 + 2, 1, color, 0, 1);
				colpip_segm(db, xc, yc, smpr->gs, gswr, smpr->r1 - 2, 1, color, 0, 1);
				colpip_radius(db, xc, yc, smpr->gs, smpr->r1 - 2, smpr->r2 + 2, color, 0, 1);
				colpip_radius(db, xc, yc, gswr, smpr->r1 - 2, smpr->r2 + 2, color, 0, 1);
				polar_to_dek(xc, yc, gswr - 1, smpr->r1 - 4, & xx, & yy, 1);
				display_floodfill(db, xx, yy, color, bgcolor);
			}

			const COLORPIP_T color = COLORPIP_GREEN;
			colpip_radius(db, xc - 1, yc, gp, smpr->rv1, smpr->rv2, color, 0, 1);
			colpip_radius(db, xc, yc, gp, smpr->rv1, smpr->rv2, color, 0, 1);
			colpip_radius(db, xc + 1, yc, gp, smpr->rv1, smpr->rv2, color, 0, 1);
#endif /* WITHRLEDECOMPRESS */
		}

		else
		{
			// RX state
			colpip_bitblt(
					db->cachebase, db->cachesize,
					db, x0, y0,
					smbgdb->cachebase, 0*smbgdb->cachesize,
					smbgdb,
					0, 0,	// координаты окна источника
					SM_BG_W, SM_BG_H, // размер окна источника
					BITBLT_FLAG_NONE, 0);
#if WITHRLEDECOMPRESS
			smeter_arrow(db, gv, x0, y0, smeter_bg_new.width, smeter_bg_new.height, COLOR_GRAY);
#else
			{
				// Рисование peak value (риска)
				const COLORPIP_T color = COLORPIP_YELLOW;
				colpip_radius(db, xc - 1, yc, gv_trace, smpr->r1 - 2, smpr->r2 + 2, color, 0, 1);
				colpip_radius(db, xc, yc, gv_trace, smpr->r1 - 2, smpr->r2 + 2, color, 0, 1);
				colpip_radius(db, xc + 1, yc, gv_trace, smpr->r1 - 2, smpr->r2 + 2, color, 0, 1);
			}

			{
				// Рисование стрелки
				const COLORPIP_T color = COLORPIP_GREEN;
				colpip_radius(db, xc - 1, yc, gv, smpr->rv1, smpr->rv2, color, 0, 1);
				colpip_radius(db, xc, yc, gv, smpr->rv1, smpr->rv2, color, 0, 1);
				colpip_radius(db, xc + 1, yc, gv, smpr->rv1, smpr->rv2, color, 0, 1);
			}
#endif /* WITHRLEDECOMPRESS */
		}

#if WITHAA
		display_do_AA(db, x0, y0, SM_BG_W, SM_BG_H);
#endif /* WITHAA */

		break;

	case SMETER_TYPE_BARS:

		if (is_tx)
		{
			colpip_bitblt(
					db->cachebase, db->cachesize,
					db, x0, y0,
					smbgdb->cachebase, 0*smbgdb->cachesize,
					smbgdb,
					0, 0,	// координаты окна источника
					SM_BG_W, SM_BG_H, // размер окна источника
					BITBLT_FLAG_NONE, 0);

			if(gp > smpr->gs)
				colpip_rect(db, x0 + smpr->gs, y0 + smpr->r1 + 5, x0 + gp, y0 + smpr->r1 + 20, COLORPIP_GREEN, 1);

			if(gswr > smpr->gs)
				colpip_rect(db, x0 + smpr->gs, y0 + smpr->r2 - 20, x0 + gswr, y0 + smpr->r2 - 5, COLORPIP_GREEN, 1);
		}
		else
		{
			colpip_bitblt(
					db->cachebase, db->cachesize,
					db, x0, y0,
					smbgdb->cachebase, 0*smbgdb->cachesize,
					smbgdb,
					0, 0,	// координаты окна источника
					SM_BG_W, SM_BG_H,	// размер окна источника
					BITBLT_FLAG_NONE, 0
					);
			// Уровень сигнала
			colpip_rect(db, x0 + smpr->gs, y0 + smpr->r1 + 5, x0 + gv, y0 + smpr->r1 + 20, COLORPIP_GREEN, 1);
			// Пиковый уровень сигнала
			if (gv_trace > gv)
				colpip_line(db, x0 + gv_trace, y0 + smpr->r1 + 5, x0 + gv_trace, y0 + smpr->r1 + 20, COLORPIP_YELLOW, 0);
		}

		break;
	}
}

// ширина занимаемого места - 15 ячеек (240/16 = 15)
static void
display2_smeter15(const gxdrawb_t * db,
		uint_fast8_t xgrid,
		uint_fast8_t ygrid,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	const uint_fast8_t pathi = 0;	// тракт, испольуемый для показа s-метра
	pix_display2_smeter15(db, GRID2X(xgrid), GRID2Y(ygrid), GRID2X(xspan), GRID2Y(yspan), pathi);
}

#endif /* LCDMODE_LTDC */

// Отображение частоты. Герцы так же большим шрифтом.
static void display_freqXa_big(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const uint_fast8_t comma = 3 - rj;

	if (0)
	{

	}
#if WITHDIRECTFREQENER
	else if (pctx != NULL && pctx->type == DCTX_FREQ)
	{
		const editfreq2_t * const efp = (const editfreq2_t *) pctx->pv;


		display_value_big(db, x, y, xspan, yspan, efp->freq, fullwidth, comma, comma + 3, rj, efp->blinkpos + 1, efp->blinkstate, 0, & dbstylev_1freqv);	// отрисовываем верхнюю часть строки
	}
#endif /* WITHDIRECTFREQENER */
	else
	{
		enum { blinkpos = UINT8_MAX, blinkstate = 0 };

		const uint_fast32_t freq = hamradio_get_freq_a();

		display_value_big(db, x, y, xspan, yspan, freq, fullwidth, comma, comma + 3, rj, blinkpos, blinkstate, 0, & dbstylev_1freqv);	// отрисовываем верхнюю часть строки
	}
}


// Подготовка отображения частоты. Герцы маленьким шрифтом.
static void display2_freqX_a_init(
	const gxdrawb_t * db_unused,	// NULL
	uint_fast8_t xcell,
	uint_fast8_t ycell,
	uint_fast8_t xspan,
	uint_fast8_t yspan,
	dctx_t * pctx
	)
{
#if WITHPRERENDER
	/* valid chars: "0123456789 #._" */
	rendered_value_big_initialize(dbstylev_1freqv.textcolor, dbstylev_1freqv.bgcolor);
#endif /* WITHPRERENDER */
}

// Отображение частоты. Герцы маленьким шрифтом.
static void display2_freqX_a(
	const gxdrawb_t * db,
	uint_fast8_t xcell,
	uint_fast8_t ycell,
	uint_fast8_t xspan,
	uint_fast8_t yspan,
	dctx_t * pctx
	)
{
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const uint_fast8_t comma = 3 - rj;

	if (0)
	{

	}
#if WITHDIRECTFREQENER
	else if (pctx != NULL && pctx->type == DCTX_FREQ)
	{
		const editfreq2_t * const efp = (const editfreq2_t *) pctx->pv;
	#if WITHPRERENDER
		rendered_value_big(db, xcell, ycell, xspan, yspan, efp->freq, fullwidth, comma, comma + 3, rj, efp->blinkpos + 1, efp->blinkstate, 1);	// отрисовываем верхнюю часть строки
	#else /* WITHPRERENDER */
		display_value_big(db, xcell, ycell, xspan, yspan, efp->freq, fullwidth, comma, comma + 3, rj, efp->blinkpos + 1, efp->blinkstate, 1, & dbstylev_1freqv);	// отрисовываем верхнюю часть строки
	#endif /* WITHPRERENDER */
	}
#endif /* WITHDIRECTFREQENER */
	else
	{
		enum { blinkpos = UINT8_MAX, blinkstate = 0 };

		const uint_fast32_t freq = hamradio_get_freq_a();

	#if WITHPRERENDER
		rendered_value_big(db, xcell, ycell, xspan, yspan, freq, fullwidth, comma, comma + 3, rj, blinkpos, blinkstate, 1);	// отрисовываем верхнюю часть строки
	#else /* WITHPRERENDER */
		display_value_big(db, xcell, ycell, xspan, yspan, freq, fullwidth, comma, comma + 3, rj, blinkpos, blinkstate, 1, & dbstylev_1freqv);	// отрисовываем верхнюю часть строки
	#endif /* WITHPRERENDER */
	}
}

// Верстия отображения без точки между мегагерцами и сотнями килогерц (для текстовых дисплееев)
// FREQ B
static void display_freqchr_a(
	const gxdrawb_t * db,
	uint_fast8_t xcell,
	uint_fast8_t ycell,
	uint_fast8_t xspan,
	uint_fast8_t yspan,
	dctx_t * pctx
	)
{
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const uint_fast8_t comma = 3 - rj;

	if (0)
	{

	}
#if WITHDIRECTFREQENER
	else if (pctx != NULL && pctx->type == DCTX_FREQ)
	{
		const editfreq2_t * const efp = (const editfreq2_t *) pctx->pv;

		display_value_big(db, xcell, ycell, xspan, yspan, efp->freq, fullwidth, comma, 255, rj, efp->blinkpos + 1, efp->blinkstate, 1, & dbstylev_1freqv);	// отрисовываем верхнюю часть строки
	}
#endif /* WITHDIRECTFREQENER */
	else
	{
		enum { blinkpos = UINT8_MAX, blinkstate = 0 };

		const uint_fast32_t freq = hamradio_get_freq_a();

		display_value_big(db, xcell, ycell, xspan, yspan, freq, fullwidth, comma, 255, rj, blinkpos, blinkstate, 1, & dbstylev_1freqv);	// отрисовываем верхнюю часть строки
	}
}

// Верстия отображения без точки между мегагерцами и сотнями килогерц (для текстовых дисплееев)
// FREQ B
static void display_freqchr_b(const gxdrawb_t * db,
	uint_fast8_t xcell,
	uint_fast8_t ycell,
	uint_fast8_t xspan,
	uint_fast8_t yspan,
	dctx_t * pctx
)
{
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const uint_fast8_t comma = 3 - rj;
	uint_fast8_t state;
	hamradio_get_vfomode3_value(& state);	// state - признак активного SPLIT (0/1)
	const gxstyle_t * const dbstylep = & dbstylev_2rxB [state];
	if (0)
	{

	}
#if WITHDIRECTFREQENER
	else if (pctx != NULL && pctx->type == DCTX_FREQ)
	{
		const editfreq2_t * const efp = (const editfreq2_t *) pctx->pv;

		display_value_big(db, xcell, ycell, xspan, yspan, efp->freq, fullwidth, comma, UINT8_MAX, rj, efp->blinkpos + 1, efp->blinkstate, 1, dbstylep);	// отрисовываем верхнюю часть строки
	}
#endif /* WITHDIRECTFREQENER */
	else
	{
		enum { blinkpos = UINT8_MAX, blinkstate = 0 };

		const uint_fast32_t freq = hamradio_get_freq_b();

		display_value_big(db, xcell, ycell, xspan, yspan, freq, fullwidth, comma, UINT8_MAX, 1, blinkpos, blinkstate, 1, dbstylep);	// отрисовываем верхнюю часть строки
	}
}

static void display2_freqX_b(const gxdrawb_t * db,
		uint_fast8_t xcell,
		uint_fast8_t ycell,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	uint_fast8_t rj;
	uint_fast8_t fullwidth = display_getfreqformat(& rj);
	const uint_fast8_t comma = 3 - rj;
	uint_fast8_t state;
	hamradio_get_vfomode3_value(& state);	// state - признак активного SPLIT (0/1)
	const gxstyle_t * const dbstylep = & dbstylev_2rxB [state];

	const uint_fast32_t freq = hamradio_get_freq_b();

	pix_display_value_small(db, GRID2X(xcell), GRID2Y(ycell), GRID2X(xspan), GRID2Y(yspan), freq, fullwidth, comma, comma + 3, rj, dbstylep);
}

// отладочная функция измерителя опорной частоты
static void display_freqmeter10(const gxdrawb_t * db,
		uint_fast8_t xcell,
		uint_fast8_t ycell,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHFQMETER
	char buf2 [11];

	local_snprintf_P(
		buf2, ARRAY_SIZE(buf2),
		"%010lu",
		(unsigned long) board_get_fqmeter()
		);

	display_text(db, xcell, ycell, buf2, xspan, yspan, & dbstylev_1freqv);
#endif /* WITHFQMETER */
}

// отображение текста с атрибутами по состоянию
static void
display2text_states(const gxdrawb_t * db,
	uint_fast8_t xcell,
	uint_fast8_t ycell,
	const char * const * labels,	// массив указателей на текст
	const gxstyle_t * dbstylep,			// массив стилей
	uint_fast8_t state,
	uint_fast8_t width,				/* ширина поляЮ в которую должно быть вписано значение */
	uint_fast8_t yspan				/* ширина поляЮ в которую должно быть вписано значение */
	)
{
	#if LCDMODE_COLORED
	#else /* LCDMODE_COLORED */
	#endif /* LCDMODE_COLORED */
#if WITHALTERNATIVELAYOUT
	layout_label1_medium(db, xcell, ycell, labels [state], strlen(labels [state]), 5, COLORPIP_BLACK, dbstylep->textcolor);
#else
	display_text(db, xcell, ycell, labels [state], width, yspan, dbstylep + state);
#endif /* WITHALTERNATIVELAYOUT */
}

// Отображение режимов TX / RX
static void display_txrxstate2(const gxdrawb_t * db,
		uint_fast8_t xcell,
		uint_fast8_t ycell,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHTX
	const uint_fast8_t state = hamradio_get_tx();

	static const char text0 [] = "RX";
	static const char text1 [] = "TX";
	const char * const labels [2] = { text0, text1 };
	display2text_states(db, xcell, ycell, labels, dbstylev_2rxtx, state, xspan, yspan);
#endif /* WITHTX */
}

// Отображение режимов TX / RX
static void display_txrxstate5alt(const gxdrawb_t * db,
		uint_fast8_t xcell,
		uint_fast8_t ycell,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHTX
	const uint_fast8_t state = hamradio_get_tx();

	static const char text0 [] = "RX";
	static const char text1 [] = "TX";
	const char * const labels [2] = { text0, text1 };
	layout_label1_medium(db, xcell, ycell, labels [state], 2, 5, state ? COLORPIP_WHITE : COLORPIP_BLACK, state ? COLORPIP_RED : COLORPIP_GRAY);
#endif /* WITHTX */
}

// Отображение режима записи аудио фрагмента
static void display2_rec3(const gxdrawb_t * db,
		uint_fast8_t xcell,
		uint_fast8_t ycell,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHUSEAUDIOREC

	const uint_fast8_t state = hamradio_get_rec_value();
	static const char text_pau [] = "PAU";
	static const char text_rec [] = "REC";
	const char * const labels [2] = { text_pau, text_rec };

	/* формирование мигающей надписи REC */
	display2text_states(db, xcell, ycell, labels, hamradio_get_blinkphase() ? dbstylev_2state_rec : dbstylev_2state, state, xspan, yspan);

#endif /* WITHUSEAUDIOREC */
}

static void display_2states(const gxdrawb_t * db,
	uint_fast8_t xcell,
	uint_fast8_t ycell,
	uint_fast8_t state,
	const char * state1,	// активное
	const char * state0,
	uint_fast8_t xspan,
	uint_fast8_t yspan
	)
{
#if LCDMODE_COLORED
	const char * const labels [2] = { state1, state1, };
#else /* LCDMODE_COLORED */
	const char * const labels [2] = { state0, state1, };
#endif /* LCDMODE_COLORED */
#if 0
	const uint_fast16_t x = GRID2X(xcell);
	const uint_fast16_t y = GRID2Y(ycell);
	const uint_fast16_t w = SMALLCHARW * xspan;
	const uint_fast16_t h = SMALLCHARH;
	const gxstyle_t * const dbstylep = & dbstylev_2state [1],			// массив стилей

	display2text_states(db, xcell, ycell, labels, dbstylev_2state, 1, xspan, yspan);

	colmain_rounded_rect(
			db,
			x, y, x + w - 1, y + h - 1,
			5,
			state ? COLORPIP_WHITE : dbstylev_2state [1].bgcolor,
			dbstylep->bgradius,
			dbstylep->bgfilled
			);
#else
	display2text_states(db, xcell, ycell, labels, dbstylev_2state, state, xspan, yspan);
#endif
}

// Параметры, не меняющие состояния цветом
static void display_1state(const gxdrawb_t * db,
	uint_fast8_t x, 
	uint_fast8_t y, 
	const char * label,
	uint_fast8_t xspan,
	uint_fast8_t yspan
	)
{
	display2text_states(db, x, y, & label, & dbstylev_1state, 0, xspan, yspan);
}

void display_2fmenuslines(
	const gxdrawb_t * db,
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t xspan,
	uint_fast8_t yspan,
	uint_fast8_t active,
	const char * label,
	const char * value
	)
{
	const char * strings [] = { label, value, };
	pix_display_texts(db, GRID2X(x), GRID2Y(y), GRID2X(xspan), GRID2Y(yspan), & dbstylev_2fmenu [active], strings,ARRAY_SIZE(strings));
}

/* Middle bar rendering */
void display2_midbar(const gxdrawb_t * db,
	uint_fast8_t x0,
	uint_fast8_t y0,
	uint_fast8_t xspan,
	uint_fast8_t yspan,
	dctx_t * pctx
	)
{
	const uint_fast16_t x0pix = GRID2X(x0);
	const uint_fast16_t y0pix = GRID2Y(y0);				// смещение по вертикали в пикселях части отведенной спектру
	const uint_fast16_t alldx = GRID2X(xspan);
	const uint_fast16_t alldy = GRID2Y(yspan);				// размер по вертикали в пикселях части отведенной спектру
	const uint_fast16_t cellwidth = alldx / MIDCELLS;

	uint_fast8_t section;
	for (section = 0; section < MIDCELLS; ++ section)
	{
		const uint_fast16_t xpos = cellwidth * section;
		uint_fast8_t active;
		const char * const label = hamradio_midlabel5(section, & active);
		const char * const value = hamradio_midvalue5(section, & active);
		const char * const texts [] = { label, value, };
		pix_display_texts(db, xpos, y0pix, cellwidth, alldy, & dbstylev_2stateSmall [active], texts, ARRAY_SIZE(texts));

	}
}

// отображение состояния CTCSS
void display2_rxctcss5(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHSUBTONES
	uint_fast32_t freq;
	const uint_fast8_t active = hamradio_get_ctcss_active(& freq);
	if (active)
	{
		char b [32];
		//local_snprintf_P(b, ARRAY_SIZE(b), "%d", (int) (freq));
		local_snprintf_P(b, ARRAY_SIZE(b), "%d.%d", (int) (freq / 10), (int) (freq % 10));
		display_2states(db, x, y, active, b, b, xspan, yspan);
	}
	else
	{
		display_2states(db, x, y, active, "-", "-", xspan, yspan);
	}
#endif /* WITHSUBTONES */
}

// отображение состояния USB BT
static void display2_btsts2(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHUSEUSBBT && (defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI))
	const uint_fast8_t active = hamradio_get_usbbth_active();
	#if LCDMODE_COLORED
		static const char text_bt [] = "BT";
		display_2states(db, x, y, active, text_bt, text_bt, xspan, yspan);
	#else /* LCDMODE_COLORED */
		display_text(db, x, y, active ? "BT" : "", xspan, yspan, & dbstylev);
	#endif /* LCDMODE_COLORED */
#endif /* WITHUSEUSBBT && (defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI)) */
}

// отображение состояния USB HOST
static void display2_usbsts3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI)
	const uint_fast8_t active = hamradio_get_usbmsc_active();
	#if LCDMODE_COLORED
		static const char text_usb [] = "USB";
		display_2states(db, x, y, active, text_usb, text_usb, xspan, yspan);
	#else /* LCDMODE_COLORED */
		display_text(db, x, y, active ? "USB" : "", xspan, yspan, & dbstylev);
	#endif /* LCDMODE_COLORED */
#endif /* defined (WITHUSBHW_HOST) || defined (WITHUSBHW_EHCI) */
}

//////////////

// Отображение остояния ENC1F
static void display2_ENC1F(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	char buf2 [10];
	const uint_fast8_t active = hamradio_get_bringENC1F();
	(void) pctx;

	hamradio_get_label_ENC1F(active, buf2, ARRAY_SIZE(buf2));
	display_2states(db, x, y, active, buf2, buf2, xspan, yspan);
}

// Отображение остояния ENC2F
static void display2_ENC2F(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	char buf2 [10];
	const uint_fast8_t active = hamradio_get_bringENC2F();
	(void) pctx;

	hamradio_get_label_ENC2F(active, buf2, ARRAY_SIZE(buf2));
	display_2states(db, x, y, active, buf2, buf2, xspan, yspan);
}

// Отображение остояния ENC4F
static void display2_ENC3F(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	char buf2 [10];
	const uint_fast8_t active = hamradio_get_bringENC3F();
	(void) pctx;

	hamradio_get_label_ENC3F(active, buf2, ARRAY_SIZE(buf2));
	display_2states(db, x, y, active, buf2, buf2, xspan, yspan);
}

// Отображение остояния ENC4F
static void display2_ENC4F(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	char buf2 [10];
	const uint_fast8_t active = hamradio_get_bringENC4F();
	(void) pctx;

	hamradio_get_label_ENC4F(active, buf2, ARRAY_SIZE(buf2));
	display_2states(db, x, y, active, buf2, buf2, xspan, yspan);
}

/////////////

static const char text_nul1_P [] = " ";
static const char text_nul2_P [] = "  ";
static const char text_nul3_P [] = "   ";
static const char text_nul4_P [] = "    ";
static const char text_nul5_P [] = "     ";
//static const char text_nul9_P [] = "         ";
static const char text_nul3 [] = "   ";
static const char text_nul5 [] = "     ";

// Отображение режима NR ON/OFF
static void display2_nr3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHIF4DSP
	int_fast32_t grade;
	const uint_fast8_t state = hamradio_get_nrvalue(& grade);
	display_2states(db, x, y, state, "NR", "", xspan, yspan);
#endif /* WITHIF4DSP */
}

/* Отображение включенного режима CW BREAK-IN */
static void display2_bkin3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHELKEY
	const uint_fast8_t state = hamradio_get_bkin_value();
	display_2states(db, x, y, state, "BKN", "", xspan, yspan);
	(void) pctx;
#endif /* WITHELKEY */
}

/* Отображение включенного динамика */
static void display2_spk3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHSPKMUTE
	static const char text_spk [] = "SPK";
	const uint_fast8_t state = hamradio_get_spkon_value();	// не-0: динамик включен
	display_2states(db, x, y, state, text_spk, text_spk, xspan, yspan);
	(void) pctx;
#endif /* WITHSPKMUTE */
}

static void display2_wpm5(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHELKEY
	const uint_fast8_t value = hamradio_get_cw_wpm();	// не-0: динамик включен
	char buf2 [6];
	const char * const labels [1] = { buf2, };

	local_snprintf_P(buf2, ARRAY_SIZE(buf2), "%2dwpm", (int) value);
	display2text_states(db, x, y, labels, & dbstylev_1state, 0, xspan, yspan);
	(void) pctx;
#endif /* WITHELKEY */
}

// Отображение типа режима NOCH и ON/OFF
static void display2_notch5(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHNOTCHONOFF || WITHNOTCHFREQ
	int_fast32_t freq;
	const uint_fast8_t state = hamradio_get_notchvalue(& freq);
	const char * const label = hamradio_get_notchtype5_P();
	const char * const labels [2] = { label, label, };
	display2text_states(db, x, y, labels, dbstylev_2state, state, xspan, yspan);
#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
}

// Отображение типа режима NOCH и ON/OFF
static void display2_notch7alt(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHNOTCHONOFF || WITHNOTCHFREQ
	int_fast32_t freq;
	const uint_fast8_t state = hamradio_get_notchvalue(& freq);
	const char * const label = hamradio_get_notchtype5_P();
	const char * const labels [2] = { label, label, };
	layout_label1_medium(db, x, y, label, strlen_P(label), 7, COLORPIP_BLACK, colors_2state_alt [state]);
#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
}

// Отображение частоты NOCH
static void display2_notchfreq5(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHNOTCHONOFF || WITHNOTCHFREQ
	int_fast32_t freq;
	const uint_fast8_t state = hamradio_get_notchvalue(& freq);
	char buf2 [6];
	local_snprintf_P(buf2, ARRAY_SIZE(buf2), "%5u", (unsigned) freq);
	display_2states(db, x, y, state, buf2, "", xspan, yspan);
#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
}

// Отображение режима NOCH ON/OFF
static void display_notch3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHNOTCHONOFF || WITHNOTCHFREQ
	int_fast32_t freq;
	const uint_fast8_t state = hamradio_get_notchvalue(& freq);
	display_2states(db, x, y, state, "NCH", "", xspan, yspan);
#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
}


// VFO mode
static void display2_vfomode3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	uint_fast8_t state;	// state - признак активного SPLIT (0/1)
	const char * const labels [1] = { hamradio_get_vfomode3_value(& state), };
	display2text_states(db, x, y, labels, & dbstylev_1state, 0, xspan, yspan);
}

// VFO mode
static void display2_vfomode5alt(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	uint_fast8_t state;	// state - признак активного SPLIT (0/1)
	const char * const labels [1] = { hamradio_get_vfomode3_value(& state), };
	layout_label1_medium(db, x, y, labels [0], strlen_P(labels [0]), 5, COLORPIP_BLACK, colors_2state_alt [state]);
}

// VFO mode with memory ch info
static void display_vfomode5(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	uint_fast8_t state;	// state - признак активного SPLIT (0/1)
	const char * const labels [1] = { hamradio_get_vfomode5_value(& state), };
	display2text_states(db, x, y, labels, & dbstylev_1state, 0, xspan, yspan);
}

// Отображение режима передачи аудио с USB
static void display_datamode4(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHTX
	#if WITHIF4DSP && WITHUSBUAC && WITHDATAMODE
		const uint_fast8_t state = hamradio_get_datamode();
		display_2states(db, x, y, state, "DATA", "", xspan, yspan);
	#endif /* WITHIF4DSP && WITHUSBUAC && WITHDATAMODE */
#endif /* WITHTX */
}

// Отображение режима передачи аудио с USB
static void display2_datamode3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHTX
	#if WITHIF4DSP && WITHUSBUAC && WITHDATAMODE
		const uint_fast8_t state = hamradio_get_datamode();
		display_2states(db, x, y, state, "DAT", "", xspan, yspan);
	#endif /* WITHIF4DSP && WITHUSBUAC && WITHDATAMODE */
#endif /* WITHTX */
}

// Отображение режима автонастройки
static void display2_atu3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHTX
	#if WITHAUTOTUNER
		const uint_fast8_t state = hamradio_get_atuvalue();
		display_2states(db, x, y, state, "ATU", "", xspan, yspan);
	#endif /* WITHAUTOTUNER */
#endif /* WITHTX */
}

// Отображение режима автонастройки
static void display2_atu4alt(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHTX
	#if WITHAUTOTUNER
		const uint_fast8_t state = hamradio_get_atuvalue();
		display_2states(db, x, y, state, "ATU", "", xspan, yspan);
	#endif /* WITHAUTOTUNER */
#endif /* WITHTX */
}


// Отображение режима General Coverage / HAM bands
static void display2_genham1(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHBCBANDS

	const uint_fast8_t state = hamradio_get_genham_value();

	display_2states(db, x, y, state, "G", "H", xspan, yspan);

#endif /* WITHBCBANDS */
}

// Отображение режима обхода тюнера
static void display2_byp3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHTX
	#if WITHAUTOTUNER
	const uint_fast8_t state = hamradio_get_bypvalue();
	display_2states(db, x, y, state, "BYP", "", xspan, yspan);
	#endif /* WITHAUTOTUNER */
#endif /* WITHTX */
}

// Отображение режима обхода тюнера
static void display2_byp4alt(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHTX
	#if WITHAUTOTUNER
	const uint_fast8_t state = hamradio_get_bypvalue();
	display_2states(db, x, y, state, "BYP", "", xspan, yspan);
	#endif /* WITHAUTOTUNER */
#endif /* WITHTX */
}

// Отображение режима VOX
static void display_vox3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHTX
	#if WITHVOX
		const uint_fast8_t state = hamradio_get_voxvalue();
		display_2states(db, x, y, state, "VOX", "", xspan, yspan);
	#endif /* WITHVOX */
#endif /* WITHTX */
}

// Отображение режимов VOX и TUNE
// Если VOX не предусмотрен, только TUNE
static void display2_voxtune3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHTX

	static const char text_vox [] = "VOX";
	static const char text_tun [] = "TUN";
	static const char text_nul [] = "";

#if WITHVOX

	const uint_fast8_t tunev = hamradio_get_tunemodevalue();
	const uint_fast8_t voxv = hamradio_get_voxvalue();

	#if LCDMODE_COLORED
		const char * const labels [4] = { text_vox, text_vox, text_tun, text_tun, };
	#else /* LCDMODE_COLORED */
		const char * const labels [4] = { text_nul, text_vox, text_tun, text_tun, };
	#endif /* LCDMODE_COLORED */

	display2text_states(db, x, y, labels, dbstylev_4state, tunev * 2 + voxv, xspan, yspan);

#else /* WITHVOX */

	const uint_fast8_t state = hamradio_get_tunemodevalue();

	display_2states(db, x, y, state, "TUN", "", xspan, yspan);

#endif /* WITHVOX */
#endif /* WITHTX */
}

// Отображение режимов VOX и TUNE
// Длинный текст
// Если VOX не предусмотрен, только TUNE
static void display_voxtune4(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHTX
#if WITHVOX

	const uint_fast8_t tunev = hamradio_get_tunemodevalue();
	const uint_fast8_t voxv = hamradio_get_voxvalue();
	static const char text0 [] = "VOX ";
	static const char text1 [] = "TUNE";
	const char * const labels [4] = { text0, text0, text1, text1, };
	display2text_states(db, x, y, labels, dbstylev_4state, tunev * 2 + voxv, xspan, yspan);

#else /* WITHVOX */

	const uint_fast8_t state = hamradio_get_tunemodevalue();
	display_2states(db, x, y, state, "TUNE", "", xspan, yspan);

#endif /* WITHVOX */
#endif /* WITHTX */
}


static void display_lockstate3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	const uint_fast8_t lockv = hamradio_get_lockvalue();
	const uint_fast8_t fastv = hamradio_get_usefastvalue();

	static const char text0 [] = "";
	static const char text1 [] = "LCK";
	static const char text2 [] = "FST";
#if LCDMODE_COLORED
	const char * const labels [4] = { text1, text2, text1, text1, };
#else /* LCDMODE_COLORED */
	const char * const labels [4] = { text0, text2, text1, text1, };
#endif
	display2text_states(db, x, y, labels, dbstylev_4state, lockv * 2 + fastv, xspan, yspan);
}

static void display2_lockstate4(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	const uint_fast8_t lockv = hamradio_get_lockvalue();
	const uint_fast8_t fastv = hamradio_get_usefastvalue();

	static const char text0 [] = "";
	static const char text1 [] = "LOCK";
	static const char text2 [] = "FAST";
#if LCDMODE_COLORED
	const char * const labels [4] = { text1, text2, text1, text1, };
#else /* LCDMODE_COLORED */
	const char * const labels [4] = { text0, text2, text1, text1, };
#endif
	display2text_states(db, x, y, labels, dbstylev_4state, lockv * 2 + fastv, xspan, yspan);
}

static void display2_lockstate5alt(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	enum { chars_W2 = 5 };
	const uint_fast8_t lockv = hamradio_get_lockvalue();
	const uint_fast8_t fastv = hamradio_get_usefastvalue();

	static const char text0 [] = "";
	static const char text1 [] = "LOCK";
	static const char text2 [] = "FAST";

	layout_label1_medium(db, x, y, fastv ? text2 : text1, 4, chars_W2, COLORPIP_BLACK, colors_2state_alt [lockv || fastv]);
}


static void display_lockstate1(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	gxstyle_t dbstylev;
	gxstyle_initialize(& dbstylev);
	gxstyle_textcolor(& dbstylev, DSGN_LOCKCOLOR, DSGN_BGCOLOR);
	display_text(db, x, y, hamradio_get_lockvalue() ? "*" : "", xspan, yspan, & dbstylev);
}

// RX path bandwidth
static void display2_rxbwval4(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	const char * const labels [1] = { hamradio_get_rxbw_value4(), };
	display2text_states(db, x, y, labels, & dbstylev_1statePSU, 0, xspan, yspan);
}

// RX path bandwidth
static void display2_rxbwval6alt(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	const char * const labels [1] = { hamradio_get_rxbw_value4(), };
	enum { state = 0 };
	layout_label1_medium(db, x, y, labels [state], 4, 6, state ? COLORPIP_WHITE : COLORPIP_BLACK, state ? COLORPIP_RED : COLORPIP_GRAY);
}


// RX path bandwidth name
static void display2_rxbw3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	const char * const labels [1] = { hamradio_get_rxbw_label3_P(), };
	display2text_states(db, x, y, labels, & dbstylev_1state, 0, xspan, yspan);
}

// текущее состояние DUAL WATCH
static void display2_mainsub3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHUSEDUALWATCH
	uint_fast8_t state;	// state - признак активного SPLIT (0/1)
	hamradio_get_vfomode5_value(& state);
	const char * const label = hamradio_get_mainsubrxmode3_value_P();
	display_2states(db, x, y, state, label, label, xspan, yspan);
#endif /* WITHUSEDUALWATCH */
}


// RX preamplifier
static void display_pre3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	const char * const labels [1] = { hamradio_get_pre_value_P(), };
	display2text_states(db, x, y, labels, & dbstylev_1state, 0, xspan, yspan);
}

// переполнение АЦП (надо показывать как REDRM_BARS - с таймерным обновлением)
static void display2_ovf3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHDSPEXTDDC
	gxstyle_t dbstylev;
	gxstyle_initialize(& dbstylev);
	//const char * const labels [1] = { hamradio_get_pre_value_P(), };
	//display2text_states(db, x, y, labels, & dbstylev_1state, 0, xspan, yspan);

	const char * labels [1];
	if (hamradio_get_bringSWR(labels))
	{
 		gxstyle_textcolor(& dbstylev, DSGN_BGCOLOR, DSGN_OVFCOLOR);
		display_text(db, x, y, labels [0], xspan, yspan, & dbstylev);
	}
	else if (boad_fpga_adcoverflow() != 0)
	{
		gxstyle_textcolor(& dbstylev, DSGN_BGCOLOR, DSGN_OVFCOLOR);
		display_text(db, x, y, "OVF", xspan, yspan, & dbstylev);
	}
	else if (boad_mike_adcoverflow() != 0)
	{
		gxstyle_textcolor(& dbstylev, DSGN_BGCOLOR, DSGN_OVFCOLOR);
		display_text(db, x, y, "MIK", xspan, yspan, & dbstylev);
	}
	else
	{
		gxstyle_textcolor(& dbstylev, DSGN_BGCOLOR, DSGN_BGCOLOR);
		display_text(db, x, y, "", xspan, yspan, & dbstylev);
	}
#endif /* WITHDSPEXTDDC */
}

// RX preamplifier или переполнение АЦП (надо показывать как REDRM_BARS - с таймерным обновлением)
static void display2_preovf3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	const char * labels [1];
	gxstyle_t dbstylev;
	gxstyle_initialize(& dbstylev);
	if (hamradio_get_bringSWR(labels))
	{
 		gxstyle_textcolor(& dbstylev, DSGN_BGCOLOR, DSGN_OVFCOLOR);
		display_text(db, x, y, labels [0], xspan, yspan, & dbstylev);
	}
	else if (boad_fpga_adcoverflow() != 0)
	{
		gxstyle_textcolor(& dbstylev, DSGN_BGCOLOR, DSGN_OVFCOLOR);
		display_text(db, x, y, "OVF", xspan, yspan, & dbstylev);
	}
	else if (boad_mike_adcoverflow() != 0)
	{
		gxstyle_textcolor(& dbstylev, DSGN_BGCOLOR, DSGN_OVFCOLOR);
		display_text(db, x, y, "MIK", xspan, yspan, & dbstylev);
	}
	else
	{
		// поля отображения, не имеющие вариантов по состоянию вкл/выкл
		gxstyle_textcolor(& dbstylev, DSGN_LABELTEXT, DSGN_LABELBACK);
		display_text(db, x, y, hamradio_get_pre_value_P(), xspan, yspan, & dbstylev);
	}
}

static void display2_preovf5alt(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	enum { chars_W2 = 5 };

	if (boad_fpga_adcoverflow() != 0)
	{
		const char str [] = "OVF";
		layout_label1_medium(db, x, y, str, 3, chars_W2, COLORPIP_WHITE, DSGN_OVFCOLOR);
	}
	else if (boad_mike_adcoverflow() != 0)
	{
		const char str [] = "MIC";
		layout_label1_medium(db, x, y, str, 3, chars_W2, COLORPIP_WHITE, DSGN_OVFCOLOR);
	}
	else
	{
		const char * str = hamradio_get_pre_value_P();
		layout_label1_medium(db, x, y, str, strlen_P(str), chars_W2, COLORPIP_BLACK, colors_2state_alt [1]);
	}
}

// display antenna
static void display2_ant5(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT2
	const char * const labels [1] = { hamradio_get_ant5_value_P(), };
	display2text_states(db, x, y, labels, & dbstylev_1state, 0, xspan, yspan);
#elif WITHANTSELECT
	const char * const labels [1] = { hamradio_get_ant5_value_P(), };
	display2text_states(db, x, y, labels, & dbstylev_1state, 0, xspan, yspan);
#endif /* WITHANTSELECT */
}

// display antenna
static void display2_ant7alt(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHANTSELECTRX || WITHANTSELECT1RX || WITHANTSELECT2
	const char * const labels [1] = { hamradio_get_ant5_value_P(), };
	layout_label1_medium(db, x, y, labels [0], strlen_P(labels [0]), 7, COLORPIP_BLACK, colors_2state_alt [1]);
#elif WITHANTSELECT
	const char * const labels [1] = { hamradio_get_ant5_value_P(), };
	layout_label1_medium(db, x, y, labels [0], strlen_P(labels [0]), 7, COLORPIP_BLACK, colors_2state_alt [1]);
#endif /* WITHANTSELECT */
}

// RX att (or att/pre)
static void display2_att4(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	const char * const labels [1] = { hamradio_get_att_value_P(), };
	display2text_states(db, x, y, labels, & dbstylev_1state, 0, xspan, yspan);
}

// RX att (or att/pre)
static void display2_att5alt(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	const char * const labels [1] = { hamradio_get_att_value_P(), };
	layout_label1_medium(db, x, y, labels [0], strlen_P(labels [0]), 5, COLORPIP_BLACK, colors_2state_alt [1]);
}

// RX att, при передаче показывает TX
static void display_att_tx3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	const uint_fast8_t tx = hamradio_get_tx();
	const char * text = tx ? "TX" : hamradio_get_att_value_P();

	display_text(db, x, y, text, xspan, yspan, & dbstylev_1state);
}

// RX agc
static void display2_agc3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	display_1state(db, x, y, hamradio_get_agc3_value_P(), xspan, yspan);
}

// RX agc
static void display_agc4(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	display_1state(db, x, y, hamradio_get_agc4_value_P(), xspan, yspan);
}

// SSB/CW/AM/FM/...
static void display2_mode3_a(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	const char * const labels [1] = { hamradio_get_mode_a_value_P(), };
	display2text_states(db, x, y, labels, & dbstylev_1state, 0, xspan, yspan);
}

#if WITHTOUCHGUI

static void display2_mode_lower_a(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	char labels[5];
	local_snprintf_P(labels, ARRAY_SIZE(labels), " %s", hamradio_get_mode_a_value_P());
	colpip_string2_tbg(db, GRID2X(x), GRID2Y(y), labels, dbstylev_1state.textcolor);
}

#endif /* WITHTOUCHGUI */

// SSB/CW/AM/FM/...
static void display2_mode3_b(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	const char * const label = hamradio_get_mode_b_value_P();
	const char * const labels [2] = { label, label };
	uint_fast8_t state;	// state - признак активного SPLIT (0/1)
	hamradio_get_vfomode3_value(& state);
	display2text_states(db, x, y, labels, dbstylev_2rxB, state, xspan, yspan);
}


static void display2_demorect(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	colpip_fillrect(db, 0, 0, DIM_X / 2, DIM_Y, COLORPIP_RED);
	colpip_fillrect(db, DIM_X / 2, 0, DIM_X / 2, DIM_Y, COLORPIP_BLUE);
}

// dd.dV - 5 places
static void display2_voltlevelV(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHVOLTLEVEL
	uint_fast8_t volt = hamradio_get_volt_value();	// Напряжение в сотнях милливольт т.е. 151 = 15.1 вольта
	//PRINTF("display2_voltlevelV: volt=%u\n", volt);
	char b [xspan + 1];
	const div_t v = div(volt, 10);
	local_snprintf_P(b, ARRAY_SIZE(b), "%d.%dV", v.quot, v.rem);
	display_text(db, x, y, b, xspan, yspan, & dbstylev_1statePSU);
#endif /* WITHVOLTLEVEL */
}

// dd.d - 4 places
static void display_voltlevel(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHVOLTLEVEL
	const uint_fast8_t volt = hamradio_get_volt_value();	// Напряжение в сотнях милливольт т.е. 151 = 15.1 вольта
	//PRINTF("display_voltlevel: volt=%u\n", volt);
	char b [xspan + 1];
	const div_t v = div(volt, 10);
	local_snprintf_P(b, ARRAY_SIZE(b), "%d.%d", v.quot, v.rem);
	display_text(db, x, y, b, xspan, yspan, & dbstylev_1statePSU);
#endif /* WITHVOLTLEVEL */
}

// отображение градусов с десятыми долями без отрицательных температур
static void display2_thermo(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if (WITHTHERMOLEVEL || WITHTHERMOLEVEL2)
	int_fast16_t tempv = hamradio_get_PAtemp_value();	// Градусы в десятых долях
	gxstyle_t dbstylev;
	gxstyle_initialize(& dbstylev);
	char b [xspan + 1];

	// 50+ - красный
	// 30+ - желтый
	// ниже 30 зеленый
	if (tempv > 999)
		tempv = 999;
	else if (tempv < - 999)
		tempv = - 999;

	const div_t d = div(tempv, 10);
	local_snprintf_P(b, ARRAY_SIZE(b), "%+d.%1d", d.quot, iabs(d.rem));
	if (tempv >= 500)
		gxstyle_textcolor(& dbstylev, COLORPIP_RED, display2_getbgcolor());
	else if (tempv >= 300)
		gxstyle_textcolor(& dbstylev, COLORPIP_YELLOW, display2_getbgcolor());
	else
		gxstyle_textcolor(& dbstylev, dbstylev_1statePSU.textcolor, dbstylev_1statePSU.bgcolor);

	display_text(db, x , y, b, xspan, yspan, & dbstylev);
#endif /* (WITHTHERMOLEVEL || WITHTHERMOLEVEL2) */
}

// отображение градусов с десятыми долями и "C" без отрицательных температур
static void display2_thermoC(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if (WITHTHERMOLEVEL || WITHTHERMOLEVEL2)
	int_fast16_t tempv = hamradio_get_PAtemp_value();	// Градусы в десятых долях
	gxstyle_t dbstylev;
	gxstyle_initialize(& dbstylev);
	char b [xspan + 1];

	// 50+ - красный
	// 30+ - желтый
	// ниже 30 зеленый
	if (tempv > 999)
		tempv = 999;
	else if (tempv < - 999)
		tempv = - 999;

	const div_t d = div(tempv, 10);
	local_snprintf_P(b, ARRAY_SIZE(b), "%+d.%1dC", d.quot, iabs(d.rem));
	if (tempv >= 500)
		gxstyle_textcolor(& dbstylev, COLORPIP_RED, display2_getbgcolor());
	else if (tempv >= 300)
		gxstyle_textcolor(& dbstylev, COLORPIP_YELLOW, display2_getbgcolor());
	else
		gxstyle_textcolor(& dbstylev, dbstylev_1statePSU.textcolor, dbstylev_1statePSU.bgcolor);

	display_text(db, x , y, b, xspan, yspan, & dbstylev);
#endif /* (WITHTHERMOLEVEL || WITHTHERMOLEVEL2) */
}

// Signed value display
// +d.ddA - 6 places (with "A")
// +dd.dA - 6 places (with "A")
static void display2_currlevelA(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHCURRLEVEL || WITHCURRLEVEL2
	#if (WITHCURRLEVEL_ACS712_30A || WITHCURRLEVEL_ACS712_20A)
		// dd.d
		int_fast16_t drain = hamradio_get_pacurrent_value() / 10;	// Ток в десятках милиампер), может быть отрицательным
		const div_t v = div(drain, 10);
		char b [xspan + 1];
		local_snprintf_P(b, ARRAY_SIZE(b), "%d.%01dA", v.quot, iabs(v.rem));
		display_text(db, x, y, b, xspan, yspan, & dbstylev_1statePSU);

	#else /* WITHCURRLEVEL_ACS712_30A */
		// d.dd
		int_fast16_t drain = hamradio_get_pacurrent_value();	// Ток в десятках милиампер, может быть отрицательным
		const div_t v = div(drain, 100);
		char b [xspan + 1];
		local_snprintf_P(b, ARRAY_SIZE(b), "%d.%02dA", v.quot, iabs(v.rem));
		display_text(db, x, y, b, xspan, yspan, & dbstylev_1statePSU);

	#endif /* WITHCURRLEVEL_ACS712_30A */
#endif /* WITHCURRLEVEL || WITHCURRLEVEL2 */
}

// +d.dd - 5 places (without "A")
// +dd.d - 5 places (without "A")
static void display2_currlevel(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHCURRLEVEL || WITHCURRLEVEL2
	#if (WITHCURRLEVEL_ACS712_30A || WITHCURRLEVEL_ACS712_20A)
		// dd.d
		int_fast16_t drain = hamradio_get_pacurrent_value() / 10;	// Ток в десятках милиампер), может быть отрицательным
		const div_t v = div(drain, 10);
		char b [xspan + 1];
		local_snprintf_P(b, ARRAY_SIZE(b), "%d.%01d", v.quot, iabs(v.rem));
		display_text(db, x, y, b, xspan, yspan, & dbstylev_1statePSU);

	#else /* WITHCURRLEVEL_ACS712_30A */
		// d.dd
		int_fast16_t drain = hamradio_get_pacurrent_value();	// Ток в десятках милиампер, может быть отрицательным
		const div_t v = div(drain, 100);
		char b [xspan + 1];
		local_snprintf_P(b, ARRAY_SIZE(b), "%d.%02d", v.quot, iabs(v.rem));
		display_text(db, x, y, b, xspan, yspan, & dbstylev_1statePSU);

	#endif /* WITHCURRLEVEL_ACS712_30A */
#endif /* WITHCURRLEVEL || WITHCURRLEVEL2 */
}

// Class-A power amplifier
static void display2_classa7(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHPACLASSA
	const uint_fast8_t active = hamradio_get_classa();
	#if LCDMODE_COLORED
		static const char classa_text [] = "CLASS A";
		static const char classa_null [] = "";
		display_2states(db, x, y, active, classa_text, classa_text, xspan, yspan);
	#else /* LCDMODE_COLORED */
		display_text(db, x, y, active ? classa_text : classa_null, xspan, yspan, & dbstylev);
	#endif /* LCDMODE_COLORED */
#endif /* WITHPACLASSA */
}

// Class-A power amplifier
static void display2_classa3(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHPACLASSA
	const uint_fast8_t active = hamradio_get_classa();
	#if LCDMODE_COLORED
		static const char classa_text [] = "CLA";
		static const char classb_text [] = "CLB";
		static const char classa_null [] = "";
		//display_2states(db, x, y, active, classa_text, classb_text);
		display_2states(db, x, y, 1, active ? classa_text : classb_text, classa_null, xspan, yspan);
	#else /* LCDMODE_COLORED */
		display_text(db, x, y, active ? classa_text : classa_null, xspan, yspan, & dbstylev);
	#endif /* LCDMODE_COLORED */
#endif /* WITHPACLASSA */
}

// Отображение уровня сигнала в dBm
static void display_siglevel7(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHIF4DSP
	int_fast16_t tracemaxi10;
	int_fast16_t rssi = dsp_rssi10(& tracemaxi10, 0);	/* получить значение уровня сигнала для s-метра в 0.1 дБмВт */

	char buf2 [8];
	// в формате при наличии знака числа ширина формата отностися ко всему полю вместе со знаком
	local_snprintf_P(buf2, ARRAY_SIZE(buf2), "%-+4d" "dBm", (int) (tracemaxi10 / 10));
	const char * const labels [1] = { buf2, };
	display2text_states(db, x, y, labels, & dbstylev_1statePSU, 0, xspan, yspan);
#endif /* WITHIF4DSP */
}

// Отображение уровня сигнала в dBm
static void display2_siglevel4(
	const gxdrawb_t * db,
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t xspan,
	uint_fast8_t yspan,
	dctx_t * pctx
	)
{
#if WITHIF4DSP
	int_fast16_t tracemaxi10;
	int_fast16_t rssi10 = dsp_rssi10(& tracemaxi10, 0);	/* получить значение уровня сигнала для s-метра в 0.1 дБмВт */

	char buf2 [5];
	// в формате при наличии знака числа ширина формата отностися ко всему полю вместе со знаком
	int j = local_snprintf_P(buf2, ARRAY_SIZE(buf2), "%-+4d", (int) (tracemaxi10 / 10));
	const char * const labels [1] = { buf2, };
	display2text_states(db, x, y, labels, & dbstylev_1statePSU, 0, xspan, yspan);
#endif /* WITHIF4DSP */
}

// Отображение КСВ в меню
void display2_swrsts20(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx)
{
	adcvalholder_t r;
	adcvalholder_t f;
	const uint_fast16_t swr = tuner_get_swr0(TUS_SWRMAX, & r, & f);
	char b [xspan + 1];

	local_snprintf_P(b, ARRAY_SIZE(b), "%u.%02u f=%-5u r=%-5u",
		(unsigned) (swr + TUS_SWRMIN) / TUS_SWRMIN,
		(unsigned) (swr + TUS_SWRMIN) % TUS_SWRMIN,
		f,
		r);
	display_text(db, x, y, b, xspan, yspan, & dbstylev_1statePSU);
}

#if WITHIF4DSP
int_fast32_t display2_zoomedbw(void)
{
	return ((int_fast64_t) dsp_get_samplerateuacin_rts() * SPECTRUMWIDTH_MULT / SPECTRUMWIDTH_DENOM) >> glob_zoomxpow2;
}
#endif /* WITHIF4DSP */

static void display2_span9(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHIF4DSP

	char buf2 [10];

	local_snprintf_P(buf2, ARRAY_SIZE(buf2), "SPAN:%3dk", (int) ((display2_zoomedbw() + 0) / 1000));
	const char * const labels [1] = { buf2, };
	display2text_states(db, x, y, labels, & dbstylev_1statePSU, 0, xspan, yspan);

#endif /* WITHIF4DSP */
}
// Отображение уровня сигнала в баллах шкалы S
// S9+60
static void display_smeter5(
	const gxdrawb_t * db,
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t xspan,
	uint_fast8_t yspan,
	dctx_t * pctx
	)
{
#if WITHIF4DSP
	int_fast16_t tracemaxi10;
	int_fast16_t rssi10 = dsp_rssi10(& tracemaxi10, 0);	/* получить значение уровня сигнала для s-метра в 0.1 дБмВт */

	const int v = rssi10 / 10;
	char buf2 [6];
	const int s9level = - 73;
	const int s9step = 6;

	if (v < - 121)
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), "S0");
	}
	else if (v < - 115)
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), "S1");
	}
	else if (v < - 109)
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), "S2");
	}
	else if (v < - 103)
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), "S3");
	}
	else if (v < - 97)
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), "S4");
	}
	else if (v < - 91)
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), "S5");
	}
	else if (v < - 85)
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), "S6");
	}
	else if (v < - 79)
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), "S7");
	}
	else if (v < - 73)
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), "S8");
	}
	else
	{
		local_snprintf_P(buf2, ARRAY_SIZE(buf2), "S9+%02d", v - (- 73));
	}
	const char * const labels [1] = { buf2, };
	display2text_states(db, x, y, labels, & dbstylev_1state, 0, xspan, yspan);
#endif /* WITHIF4DSP */
}

// Отображение уровня сигнала в dBm или S-memter (в зависимости от настроек)
static void display2_smeors5(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	if (glob_showdbm != 0)
	{
		display2_siglevel4(db, x, y, xspan, yspan, pctx);
	}
	else
	{
		display_smeter5(db, x, y, xspan, yspan, pctx);
	}
}

static void display2_freqdelta8(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHINTEGRATEDDSP
	int_fast32_t deltaf;
	const uint_fast8_t f = dsp_getfreqdelta10(& deltaf, 0);		/* Получить значение отклонения частоты с точностью 0.1 герца для приемника A */
	deltaf = - deltaf;	// ошибка по частоте преобразуется в расстройку
	if (f != 0)
	{
		char b [xspan + 1];
		local_snprintf_P(b, ARRAY_SIZE(b), "%+" PRIiFAST32 "", deltaf);
		display_text(db, x, y, b, xspan, yspan, & dbstylev_1state);
	}
	else
	{
		display_text(db, x, y, "", xspan, yspan, & dbstylev_1state);
	}
#endif /* WITHINTEGRATEDDSP */
}

/* Получить информацию об ошибке настройки в режиме SAM */
static void display_samfreqdelta8(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHINTEGRATEDDSP
	int_fast32_t deltaf;
	const uint_fast8_t f = hamradio_get_samdelta10(& deltaf, 0);		/* Получить значение отклонения частоты с точностью 0.1 герца для приемника A */
	deltaf = - deltaf;	// ошибка по частоте преобразуется в расстройку
	if (f != 0)
	{
		char b [xspan + 1];
		local_snprintf_P(b, ARRAY_SIZE(b), "%+" PRIiFAST32 "", deltaf);
		display_text(db, x, y, b, xspan, yspan, & dbstylev_1state);
	}
	else
	{
		display_text(db, x, y, "", xspan, yspan, & dbstylev_1state);
	}
#endif /* WITHINTEGRATEDDSP */
}

// d.d - 3 places
// текущее значение верхней частоты среза НЧ фильтра АМ/ЧМ
static void display_amfmhighcut4(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHINTEGRATEDDSP
	uint_fast8_t flag;
	const uint_fast8_t v = hamradio_get_amfm_highcut10_value(& flag);	// текущее значение верхней частоты среза НЧ фильтра АМ/ЧМ (в десятках герц)
	const div_t d = div(v, 10);
	char b [xspan + 1];
	local_snprintf_P(b, ARRAY_SIZE(b), "%d.%d", d.quot, d.rem);
	const char * const labels [2] = { b, b, };
	display2text_states(db, x, y, labels, dbstylev_2state, flag, xspan, yspan);
#endif /* WITHINTEGRATEDDSP */
}

// dd.d - 4 places
// текущее значение верхней частоты среза НЧ фильтра АМ/ЧМ
static void display_amfmhighcut5(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if WITHINTEGRATEDDSP
	uint_fast8_t flag;
	const uint_fast8_t v = hamradio_get_amfm_highcut10_value(& flag);	// текущее значение верхней частоты среза НЧ фильтра АМ/ЧМ (в десятках герц)
	const div_t d = div(v, 10);
	char b [xspan + 1];
	local_snprintf_P(b, ARRAY_SIZE(b), "%d.%d", d.quot, d.rem);
	const char * const labels [2] = { b, b, };
	display2text_states(db, x, y, labels, dbstylev_2state, flag, xspan, yspan);
#endif /* WITHINTEGRATEDDSP */
}

// Печать времени - часы, минуты и секунды
static void display_time8(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if defined (RTC1_TYPE)
	uint_fast8_t hour, minute, seconds;
	char buf2 [xspan + 1];

	board_rtc_cached_gettime(& hour, & minute, & seconds);
	local_snprintf_P(buf2, ARRAY_SIZE(buf2), "%02d:%02d:%02d",
			(int) hour, (int) minute, (int) seconds
		);

	const char * const labels [1] = { buf2, };
	display2text_states(db, x, y, labels, & dbstylev_1state, 0, xspan, yspan);
#endif /* defined (RTC1_TYPE) */
}

// Печать времени - только часы и минуты, без секунд
static void display_time5(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if defined (RTC1_TYPE)
	uint_fast8_t hour, minute, seconds;
	char buf2 [6];

	board_rtc_cached_gettime(& hour, & minute, & seconds);
	local_snprintf_P(buf2, ARRAY_SIZE(buf2), "%02d%c%02d",
		(int) hour,
		((seconds & 1) ? ' ' : ':'),	// мигающее двоеточие с периодом две секунды
		(int) minute
		);

	const char * const labels [1] = { buf2, };
	display2text_states(db, x, y, labels, & dbstylev_1stateTime, 0, xspan, yspan);

#endif /* defined (RTC1_TYPE) */
}


static volatile unsigned sofcount;
static uint32_t softicks;

unsigned hamradio_get_getsoffreq(void)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	unsigned count = sofcount;
	sofcount = 0;
	LowerIrql(oldIrql);

	uint32_t nowticks = sys_now();
	uint32_t difftime = nowticks - softicks;
	softicks = nowticks;
	unsigned n = count * 1000 / difftime;
	return n;
}

void hamradio_tick_sof(void)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	++ sofcount;
	LowerIrql(oldIrql);
}

// Печать частоты SOF пакетов USB device.
static void display2_freqsof9(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	char buf2 [13];
	unsigned v = hamradio_get_getsoffreq();
	v = ulmin(v, 99999);
	local_snprintf_P(buf2, ARRAY_SIZE(buf2), "Sof:%5u",
			v
		);

	const char * const labels [1] = { buf2, };
	display2text_states(db, x, y, labels, & dbstylev_1state, 0, xspan, yspan);
}

// Печать времени - только часы и минуты, без секунд
// Jan-01 13:40
static void display2_datetime12(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
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

	local_snprintf_P(buf2, ARRAY_SIZE(buf2), "%s-%02d %02d%c%02d",
		months [month - 1],
		(int) day,
		(int) hour,
		((seconds & 1) ? ' ' : ':'),	// мигающее двоеточие с периодом две секунды
		(int) minute
		);

	const char * const labels [1] = { buf2, };
	display2text_states(db, x, y, labels, & dbstylev_1stateTime, 0, xspan, yspan);
#endif /* defined (RTC1_TYPE) */
}

// Отображение места для какого-либо элемента
static void display2_dummy(
	const gxdrawb_t * db,
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t xspan,
	uint_fast8_t yspan,
	dctx_t * pctx
	)
{
	const uint_fast8_t state = 0;
	display_text(db, x, y, "", xspan, yspan, & dbstylev_1state);
}


//+++ bars

static int_fast16_t display_mapbar(
	int_fast16_t val,
	int_fast16_t bottom, int_fast16_t top,
	int_fast16_t mapleft,
	int_fast16_t mapinside,
	int_fast16_t mapright
	)
{
	if (val < bottom)
		return mapleft;
	if (val < top)
		return mapinside;
	return mapright;
}


void wait_iq(const gxdrawb_t * db,
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t xspan,
	uint_fast8_t yspan,
	dctx_t * pctx
	)
{
#if LINUX_SUBSYSTEM
	linux_wait_iq();
#endif
}

/* Описания расположения элементов на дисплеях */

#include "dstyles/dstyles.h"

#if WITHBARS

// количество точек в отображении мощности на диспле
static uint_fast16_t display_getpwrfullwidth(void)
{
	return GRID2X(CHARS2GRID(BDTH_ALLPWR));
}

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
void display_swrmeter(const gxdrawb_t * db,
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t colspan,
	uint_fast8_t rowspan,
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

	//PRINTF("swr10=%d, mapleftval=%d, fs=%d\n", swr10, mapleftval, display_getmaxswrlimb());

	gxstyle_t dbstylev;
	gxstyle_initialize(& dbstylev);
	gxstyle_textcolor(& dbstylev, DSGN_SWRCOLOR, DSGN_BGCOLOR);

	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdata_begin(display_bars_x_swr(x, CHARS2GRID(0)), y, & ypix);
	display_bar(db, xpix, ypix, BDTH_ALLSWR, rowspan, mapleftval, fullscale, fullscale, PATTERN_BAR_FULL, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYFULL, & dbstylev);

	if (BDTH_SPACESWR != 0)
	{
		// заполняем пустое место за индикаторм КСВ
		uint_fast16_t ypix;
		uint_fast16_t xpix = display_wrdata_begin(display_bars_x_swr(x, CHARS2GRID(BDTH_ALLSWR)), y, & ypix);
		display_bar(db, xpix, ypix, BDTH_SPACESWR, rowspan, 0, 1, 1, PATTERN_SPACE, PATTERN_SPACE, PATTERN_SPACE, & dbstylev);
	}

#endif /* WITHBARS && WITHTX */
}

// координаьы для общего блока PWR & SWR
static void display_pwrmeter(const gxdrawb_t * db,
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t colspan,
	uint_fast8_t rowspan,
	uint_fast8_t value,			// текущее значение
	uint_fast8_t tracemax,		// max hold значение
	uint_fast8_t maxpwrcali		// значение для отклонения на всю шкалу
	)
{
#if WITHBARS
	const uint_fast16_t fullscale = display_getpwrfullwidth();	// количество точек в отображении мощности на диспле
	uint_fast8_t v = (uint_fast32_t) value * value * fullscale / ((uint_fast32_t) maxpwrcali * maxpwrcali);
	uint_fast8_t t = (uint_fast32_t) tracemax * tracemax * fullscale / ((uint_fast32_t) maxpwrcali * maxpwrcali);
	const uint_fast8_t mapleftval = display_mapbar(v, 0, fullscale, 0, v, fullscale);
	const uint_fast8_t mapleftmax = display_mapbar(t, 0, fullscale, fullscale, t, fullscale); // fullscale - invisible

	gxstyle_t dbstylev;
	gxstyle_initialize(& dbstylev);
	gxstyle_textcolor(& dbstylev, DSGN_PWRCOLOR, DSGN_BGCOLOR);

	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdata_begin(display_bars_x_pwr(x, CHARS2GRID(0)), y, & ypix);
	display_bar(db, xpix, ypix, BDTH_ALLPWR, rowspan, mapleftval, mapleftmax, fullscale, PATTERN_BAR_HALF, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYHALF, & dbstylev);

	if (BDTH_SPACEPWR != 0)
	{
		// заполняем пустое место за индикаторм мощности
		uint_fast16_t ypix;
		uint_fast16_t xpix = display_wrdata_begin(display_bars_x_pwr(x, CHARS2GRID(BDTH_ALLPWR)), y, & ypix);
		display_bar(db, xpix, ypix, BDTH_SPACEPWR, rowspan, 0, 1, 1, PATTERN_SPACE, PATTERN_SPACE, PATTERN_SPACE, & dbstylev);
	}

#endif /* WITHBARS */
}

static void display_smeter(const gxdrawb_t * db,
	uint_fast8_t x,
	uint_fast8_t y,
	int_fast16_t value10,		// текущее значение в dBm
	int_fast16_t tracemax10,	// метка запомненного максимума в dBm
	uint_fast8_t colspan,
	uint_fast8_t rowspan
	)
{
#if WITHBARS
	gxstyle_t dbstylev;
	gxstyle_initialize(& dbstylev);
	//tracemax = value > tracemax ? value : tracemax;	// защита от рассогласования значений
	//delta1 = delta1 > level9 ? level9 : delta1;

	const int_fast16_t leftmin = - 109 * 10;	// S3 = -109.0 dBm
	const int_fast16_t delta1 = 6 * 10;	// S0..S9
	const int_fast16_t level9 = - 73 * 10;	// S9 = -73.0 dBm
	const int_fast16_t delta2 = 60 * 10;	// to S9+60
	const int_fast16_t mapleftval = display_mapbar(value10, leftmin, level9, 0, value10 - leftmin, delta1);
	const int_fast16_t mapleftmax = display_mapbar(tracemax10, leftmin, level9, delta1, tracemax10 - leftmin, delta1); // delta1 - invisible
	const int_fast16_t maprightval = display_mapbar(value10, level9, level9 + delta2, 0, value10 - level9, delta2);
	const int_fast16_t maprightmax = display_mapbar(tracemax10, level9, level9 + delta2, delta2, tracemax10 - level9, delta2); // delta2 - invisible

	gxstyle_textcolor(& dbstylev, DSGN_SMLCOLOR, DSGN_BGCOLOR);
	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdata_begin(display_bars_x_rx(x, CHARS2GRID(0)), y, & ypix);
	display_bar(db, xpix, ypix, BDTH_LEFTRX, rowspan, mapleftval, mapleftmax, delta1, PATTERN_BAR_HALF, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYHALF, & dbstylev);		//ниже 9 баллов ничего
	//
	gxstyle_textcolor(& dbstylev, DSGN_SMRCOLOR, DSGN_BGCOLOR);
	uint_fast16_t ypix2;
	uint_fast16_t xpix2 = display_wrdata_begin(display_bars_x_rx(x, CHARS2GRID(BDTH_LEFTRX)), y, & ypix2);
	display_bar(db, xpix2, ypix2, BDTH_RIGHTRX, rowspan, maprightval, maprightmax, delta2, PATTERN_BAR_FULL, PATTERN_BAR_FULL, PATTERN_BAR_EMPTYFULL, & dbstylev);		// выше 9 баллов ничего нет.

	if (BDTH_SPACERX != 0)
	{
		uint_fast16_t ypix;
		uint_fast16_t xpix = display_wrdata_begin(display_bars_x_pwr(x, CHARS2GRID(BDTH_ALLRX)), y, & ypix);
		display_bar(db, xpix, ypix, BDTH_SPACERX, rowspan, 0, 1, 1, PATTERN_SPACE, PATTERN_SPACE, PATTERN_SPACE, & dbstylev);
	}

#endif /* WITHBARS */
}
//--- bars

// Отображение шкалы S-метра и других измерителей
static void display2_legend_rx(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if defined(SMETERMAP)
	gxstyle_t dbstylev;
	gxstyle_initialize(& dbstylev);
	gxstyle_textcolor(& dbstylev, DSGN_SMLABELTEXT, DSGN_SMLABELBACK);
	display_text(db, x + 0, y, SMETERMAP, BDTH_LEFTRX, yspan, & dbstylev);
	gxstyle_textcolor(& dbstylev, DSGN_SMLABELPLKUSTEXT, DSGN_SMLABELPLKUSBACK);
	display_text(db, x + BDTH_LEFTRX, y, SMETERMAP + BDTH_LEFTRX, xspan - BDTH_LEFTRX, yspan, & dbstylev);
#endif /* defined(SMETERMAP) */
}

// Отображение шкалы SWR-метра и других измерителей
static void display2_legend_tx(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
#if defined(SWRPWRMAP) && WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR)
	gxstyle_t dbstylev;
	gxstyle_initialize(& dbstylev);
	gxstyle_textcolor(& dbstylev, DSGN_SMLABELTEXT, DSGN_SMLABELBACK);
	#if WITHSWRMTR
		#if WITHSHOWSWRPWR /* на дисплее одновременно отображаются SWR-meter и PWR-meter */
				display_text(db, x, y, SWRPWRMAP, xspan, yspan, & dbstylev);
		#else
				if (swrmode) 	// Если TUNE то показываем шкалу КСВ
					display_text(db, x, y, SWRMAP, xspan, yspan, & dbstylev);
				else
					display_text(db, x, y, POWERMAP, xspan, yspan, & dbstylev);
		#endif
	#else
		#warning No TX indication
	#endif

#endif /* defined(SWRPWRMAP) && WITHTX && (WITHSWRMTR || WITHSHOWSWRPWR) */
}


// Отображение шкалы S-метра и других измерителей
static void display2_legend(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	if (hamradio_get_tx())
		display2_legend_tx(db, x, y, xspan, yspan, pctx);
	else
		display2_legend_rx(db, x, y, xspan, yspan, pctx);
}


static uint_fast16_t calcprev16(uint_fast16_t v, uint_fast16_t lim)
{
	if (v)
		return -- v;
	else
		return lim - 1;
}

static uint_fast16_t calcnext16(uint_fast16_t v, uint_fast16_t lim)
{
	v ++;
	if (v >= lim)
		return 0;
	else
		return v;
}

#if ! LCDMODE_DUMMY

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

#endif /* ! LCDMODE_DUMMY */

#if (WITHSPECTRUMWF && ! LCDMODE_DUMMY) || WITHAFSPECTRE

enum { ALLDX = GRID2X(CHARS2GRID(BDTH_ALLRX)) };

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
#define ZOOMFFT_LPF_STAGES_IIR 9

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

enum { NROWSWFL = GRID2Y(BDCV_ALLRX) };

#if defined (COLORPIP_SHADED)

	/* быстрое отображение водопада (но требует больше памяти) */
	enum { PALETTESIZE = COLORPIP_BASE };

#elif LCDMODE_LTDC

	/* быстрое отображение водопада (но требует больше памяти) */
	#if WITHGRADIENT_FIXED
		enum { PALETTESIZE = ARRAY_SIZE(pancolor) };
	#else /* WITHGRADIENT_FIXED */
		enum { PALETTESIZE = 256 };
	#endif /* WITHGRADIENT_FIXED */

	static uint_fast16_t row3dss;		// строка, в которую последней занесены данные

#endif


// Получить цвет заполнения водопада при перестройке
static COLORPIP_T display2_bgcolorwfl(void);

#if CPUSTYLE_XC7Z || CPUSTYLE_STM32MP1 || CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_T507 || CPUSTYLE_RK356X
	enum { MAX_3DSS_STEP = 70 };
#else
	enum { MAX_3DSS_STEP = 42 };
#endif /* #if CPUSTYLE_XC7Z || CPUSTYLE_XC7Z || CPUSTYLE_STM32MP1 || CPUSTYLE_T113 */
	enum { Z_STEP_3DSS = 2 };

typedef int16_t WFL3DSS_T;
typedef int16_t SCAPEJVAL_T;

template<typename pixelt, uint_fast16_t argdx> class scrollb
{
	typedef pixelt element_t;
	const uint_fast16_t & vx;	// координаты левого верхнего угла видимой области
	const uint_fast16_t & vy;	// координаты левого верхнего угла видимой области - самые свежие данные водопада
	const uint_fast16_t dy;
	element_t * m_buffer;
	// получить адрес по невиртуальным координатам буфера
public:
	element_t * bufferat(uint_fast16_t dx, uint_fast16_t rx, uint_fast16_t ry)  const;	/* получить адрес в памяти элемента с координатами x/y */

public:
	element_t * bf() const { return m_buffer; }	// пока для переноса
	scrollb(const uint_fast16_t & scrollh, const uint_fast16_t & scrollv, element_t * buffer, uint_fast16_t ady) :
		vx(scrollh),
		vy(scrollv),
		dy(ady),
		m_buffer(buffer)
	{
	}
	// стереть содержимое
	void setupnew(uint_fast16_t dx, const pixelt & value) const;
	// частота увеличилась - надо сдвигать картинку влево
	// нужно сохрянять часть старого изображения
	void shiftleft(uint_fast16_t dx, uint_fast16_t pixels, const element_t & value) const;
	// частота уменьшилась - надо сдвигать картинку вправо
	// нужно сохрянять часть старого изображения
	void shiftright(uint_fast16_t dx, uint_fast16_t pixels, const element_t & value) const;
	// получить значение по виртуальной позиции
	element_t peek(uint_fast16_t x, uint_fast16_t y) const { return * bufferat(argdx, (vx + x) % argdx, (vy + y) % dy); }
	// записать значение по виртуальной позиции
	void poke(uint_fast16_t x, uint_fast16_t y, const element_t & value) const { * bufferat(argdx, (vx + x) % argdx, (vy + y) % dy) = value; }
};
// Специализация. стереть содержимое
template <> void scrollb<int16_t, ALLDX>::setupnew(uint_fast16_t dx, const int16_t & v) const
{
	arm_fill_q15(v, m_buffer, dx * dy);
}
// Специализация. получить адрес по невиртуальным координатам буфера
template <> int16_t * scrollb<int16_t, ALLDX>::bufferat(uint_fast16_t dx, uint_fast16_t rx, uint_fast16_t ry) const
{
	return & m_buffer [dx * ry + rx];
}
// Специализация
// частота увеличилась - надо сдвигать картинку влево
// нужно сохрянять часть старого изображения
template <> void scrollb<int16_t, ALLDX>::shiftleft(uint_fast16_t dx, uint_fast16_t pixels, const int16_t & value) const
{
	uint_fast16_t y;
    for (y = 0; y < dy; ++ y)
 	{
 		memmove(
 				bufferat(dx, 0, y),			// to
				bufferat(dx, pixels, y),		// from
 				(dx - pixels) * sizeof (element_t)
 		);
 		arm_fill_q15(value, bufferat(dx, dx - pixels, y), pixels);
	}
}
// Специализация
// частота уменьшилась - надо сдвигать картинку вправо
// нужно сохрянять часть старого изображения
template <> void scrollb<int16_t, ALLDX>::shiftright(uint_fast16_t dx, uint_fast16_t pixels, const int16_t & value) const
{
	uint_fast16_t y;
   	for (y = 0; y < dy; ++ y)
	{
   		memmove(
   				bufferat(dx, pixels, y),	// to
				bufferat(dx, 0, y),		// from
				(dx - pixels) * sizeof (element_t)
    		);
   		arm_fill_q15(value, bufferat(dx, 0, y), pixels);
   	}
}

///////////////////////////////
// Специализация. стереть содержимое
template <> void scrollb<PACKEDCOLORPIP_T, ALLDX>::setupnew(uint_fast16_t dx, const PACKEDCOLORPIP_T & value) const
{
	// todo: use accelerated graphic functions
#if LCDMODE_PIXELSIZE == 2
	arm_fill_q15(value, (q15_t *) m_buffer, GXSIZE(dx, dy));
#elif LCDMODE_PIXELSIZE == 4
	arm_fill_q31(value, (q31_t *) m_buffer, GXSIZE(dx, dy));
#elif LCDMODE_PIXELSIZE == 1
	arm_fill_q7(value, (q7_t *) m_buffer, GXSIZE(dx, dy));
#endif
}
// Специализация. получить адрес по невиртуальным координатам буфера
template <> PACKEDCOLORPIP_T * scrollb<PACKEDCOLORPIP_T, ALLDX>::bufferat(uint_fast16_t dx, uint_fast16_t rx, uint_fast16_t ry) const
{
	return & m_buffer [GXADJ(dx) * ry + rx];
}
// Специализация
// частота увеличилась - надо сдвигать картинку влево
// нужно сохрянять часть старого изображения
template <> void scrollb<PACKEDCOLORPIP_T, ALLDX>::shiftleft(uint_fast16_t dx, uint_fast16_t pixels, const PACKEDCOLORPIP_T & value) const
{
	uint_fast16_t y;
    for (y = 0; y < dy; ++ y)
 	{
 		memmove(
 				bufferat(dx, 0, y),			// to
				bufferat(dx, pixels, y),		// from
 				(dx - pixels) * sizeof (element_t)
 		);
#if LCDMODE_PIXELSIZE == 2
		arm_fill_q15(value, (q15_t *) bufferat(dx, dx - pixels, y), pixels);
#elif LCDMODE_PIXELSIZE == 4
		arm_fill_q31(value, (q31_t *) bufferat(dx, dx - pixels, y), pixels);
#elif LCDMODE_PIXELSIZE == 1
		arm_fill_q7(value, (q7_t *) bufferat(dx, dx - pixels, y), pixels);
#endif
	}
}
// Специализация
// частота уменьшилась - надо сдвигать картинку вправо
// нужно сохрянять часть старого изображения
template <> void scrollb<PACKEDCOLORPIP_T, ALLDX>::shiftright(uint_fast16_t dx, uint_fast16_t pixels, const PACKEDCOLORPIP_T & value) const
{
	uint_fast16_t y;
    for (y = 0; y < dy; ++ y)
    {
   		memmove(
   				bufferat(dx, pixels, y),	// to
				bufferat(dx, 0, y),		// from
				(ALLDX - pixels) * sizeof (element_t)
    		);
#if LCDMODE_PIXELSIZE == 2
   		arm_fill_q15(value, (q15_t *) bufferat(dx, 0, y), pixels);
#elif LCDMODE_PIXELSIZE == 4
   		arm_fill_q31(value, (q31_t *) bufferat(dx, 0, y), pixels);
#elif LCDMODE_PIXELSIZE == 1
   		arm_fill_q7(value, (q7_t *) bufferat(dx, 0, y), pixels);
#endif
   	}

}
///////////////////////////////

// Специализация. стереть содержимое
template <> void scrollb<FLOAT_t, ALLDX>::setupnew(uint_fast16_t dx, const FLOAT_t & v) const
{
	ARM_MORPH(arm_fill)(v, m_buffer, dx * dy);
}
// Специализация. получить адрес по невиртуальным координатам буфера
template <> FLOAT_t * scrollb<FLOAT_t, ALLDX>::bufferat(uint_fast16_t dx, uint_fast16_t x, uint_fast16_t y) const
{
	return & m_buffer [dx * y + x];
}
// Специализация
// частота увеличилась - надо сдвигать картинку влево
// нужно сохрянять часть старого изображения
template <> void scrollb<FLOAT_t, ALLDX>::shiftleft(uint_fast16_t dx, uint_fast16_t pixels, const FLOAT_t & value) const
{
	uint_fast16_t y;
    for (y = 0; y < dy; ++ y)
    {
 		memmove(
 				bufferat(dx, 0, y),			// to
				bufferat(dx, pixels, y),		// from
 				(dx - pixels) * sizeof (element_t)
 		);
 		ARM_MORPH(arm_fill)(value, bufferat(dx, dx - pixels, y), pixels);
	}
}
// Специализация
// частота уменьшилась - надо сдвигать картинку вправо
// нужно сохрянять часть старого изображения
template <> void scrollb<FLOAT_t, ALLDX>::shiftright(uint_fast16_t dx, uint_fast16_t pixels, const FLOAT_t & value) const
{
	uint_fast16_t y;
    for (y = 0; y < dy; ++ y)
	{
   		memmove(
   				bufferat(dx, pixels, y),	// to
				bufferat(dx, 0, y),		// from
				(dx - pixels) * sizeof (element_t)
    		);
 		ARM_MORPH(arm_fill)(value, bufferat(dx, 0, y), pixels);
   	}
}
///////////////////////////////

// Специализация. стереть содержимое
template <> void scrollb<agcstate_t, ALLDX>::setupnew(uint_fast16_t dx, const agcstate_t & v) const
{
	unsigned x;
	for (x = 0; x < dx; ++ x)
	{
		m_buffer [x] = v;
	}
}
// Специализация. получить адрес по невиртуальным координатам буфера
template <> agcstate_t * scrollb<agcstate_t, ALLDX>::bufferat(uint_fast16_t dx, uint_fast16_t x, uint_fast16_t y) const
{
	return & m_buffer [x];
}
// Специализация
// частота увеличилась - надо сдвигать картинку влево
// нужно сохрянять часть старого изображения
template <> void scrollb<agcstate_t, ALLDX>::shiftleft(uint_fast16_t dx, uint_fast16_t pixels, const agcstate_t & value) const
{
	const uint_fast16_t y = 0;
 	memmove(
		bufferat(dx, 0, y),			// to
		bufferat(dx, pixels, y),		// from
		(dx - pixels) * sizeof (element_t)
	);
	unsigned x;
	for (x = 0; x < pixels; ++ x)
	{
		* bufferat(dx, dx - pixels + x, y) = value;
	}
}
// Специализация
// частота уменьшилась - надо сдвигать картинку вправо
// нужно сохрянять часть старого изображения
template <> void scrollb<agcstate_t, ALLDX>::shiftright(uint_fast16_t dx, uint_fast16_t pixels, const agcstate_t & value) const
{
	const uint_fast16_t y = 0;
	memmove(
		bufferat(dx, pixels, y),	// to
		bufferat(dx, 0, y),		// from
		(dx - pixels) * sizeof (element_t)
	);
	unsigned x;
	for (x = 0; x < pixels; ++ x)
	{
		* bufferat(dx, x, y) = value;
	}
}

///////////////////////////////

template<typename pixelt, uint_fast16_t argdx> class scrollb1h : public scrollb<pixelt, argdx>
{
	typedef scrollb<pixelt, argdx> parent;
public:
	scrollb1h(const uint_fast16_t & scrollh, const uint_fast16_t & scrollv, pixelt * buffer) :
		scrollb<pixelt, argdx>(scrollh, scrollv, buffer, 1)
	{
	}
};

////////////////////////////////
///

template<uint_fast16_t w, uint_fast16_t h> class scrollbf
{
	uint_fast16_t centerx;
	uint_fast16_t centery;
	const uint_fast16_t centeryzero = 0;

	__ALIGNED(64) PACKEDCOLORPIP_T m_buffscrollcolor [GXSIZE(w, h)];
	int16_t m_buffscrollpwr [GXSIZE(w, h)];

	FLOAT_t m_spavgarray [w * 1];	// h == 1
	FLOAT_t m_yoldwfl [w * 1];	// h == 1
	FLOAT_t m_yoldspe [w * 1];	// h == 1
#if WITHSECTRUMPEAKS
	agcstate_t m_ypeakspe [w * 1];	// h == 1
#endif /* WITHSECTRUMPEAKS */
	FLOAT_t m_yold3dss [w * 1];	// h == 1

public:
	agcparams_t peakparams;
	uint_fast16_t getwfrow() const { return centery; }
	scrollb<PACKEDCOLORPIP_T, w>  scrollcolor;	// Водопад (можно использовать как источник данных для 3DSS)
	scrollb<int16_t, w>  scrollpwr;				// мощности для 3DSS
	/* one-row objects */
	scrollb1h<FLOAT_t, w>  spavgarray;	// строка принятая из DSP части в последний раз
	scrollb1h<FLOAT_t, w>  yoldwfl;		// фильтр водопада
	scrollb1h<FLOAT_t, w>  yoldspe;		// фильтр спектра
#if WITHSECTRUMPEAKS
	scrollb1h<agcstate_t, w>  ypeakspe;		// пиковые значения спектра
#endif /* WITHSECTRUMPEAKS */
	scrollb1h<FLOAT_t, w>  yold3dss;	// фильтр 3DSS

public:
	scrollbf() :
		scrollcolor(centerx, centery, m_buffscrollcolor, NROWSWFL),
		scrollpwr(centerx, centery, m_buffscrollpwr, NROWSWFL),
		/* one-row objects */
		spavgarray(centerx, centeryzero, m_spavgarray),
		yoldwfl(centerx, centeryzero, m_yoldwfl),
		yoldspe(centerx, centeryzero, m_yoldspe),
#if WITHSECTRUMPEAKS
		ypeakspe(centerx, centeryzero, m_ypeakspe),
#endif /* WITHSECTRUMPEAKS */
		yold3dss(centerx, centeryzero, m_yold3dss)
	{
		agc_parameters_peaks_initialize(& peakparams, glob_displayfps);	// частота latch
	}
	/* + стереть содержимое */
	void setupnew()
	{
#if WITHSECTRUMPEAKS
		agcstate_t agc0;
		agc_state_initialize(& agc0, & peakparams);
		ypeakspe.setupnew(w, agc0);
#endif /* WITHSECTRUMPEAKS */
		scrollcolor.setupnew(w, display2_bgcolorwfl());
		scrollpwr.setupnew(w, 0);
		/* one-row objects */
		spavgarray.setupnew(w, 0);
		yoldwfl.setupnew(w, 0);
		yoldspe.setupnew(w, 0);
		yold3dss.setupnew(w, 0);
	}
	/* + продвижение по истории */
	void shiftrows()
	{
		centery = (centery + h - 1) % h;
	}
	// частота увеличилась - надо сдвигать картинку влево
	// нужно сохрянять часть старого изображения
	// в строке centery - новое
	void shiftleft(uint_fast16_t pixels)
	{
		if (pixels)
		{
			//centerx = (centerx + w + pixels) % w;	// корректировка горизонтальной позиции воображаемого левого края
			// TODO: очистить освобождающиеся зоны
#if WITHSECTRUMPEAKS
			agcstate_t agc0;
			agc_state_initialize(& agc0, & peakparams);
			ypeakspe.shiftleft(w, pixels, agc0);
#endif /* WITHSECTRUMPEAKS */
			scrollcolor.shiftleft(w, pixels, display2_bgcolorwfl());
			scrollpwr.shiftleft(w, pixels, 0);
			/* one-row objects */
			spavgarray.shiftleft(w, pixels, 0);
			yoldwfl.shiftleft(w, pixels, 0);
			yoldspe.shiftleft(w, pixels, 0);
			yold3dss.shiftleft(w, pixels, 0);
		}
	}
	// частота уменьшилась - надо сдвигать картинку вправо
	// нужно сохрянять часть старого изображения
	// в строке centery - новое
	void shiftright(uint_fast16_t pixels)
	{
		if (pixels)
		{
			//centerx = (centerx + w - pixels) % w;	// корректировка горизонтальной позиции воображаемого левого края
			// TODO: очистить освобождающиеся зоны
#if WITHSECTRUMPEAKS
			agcstate_t agc0;
			agc_state_initialize(& agc0, & peakparams);
			ypeakspe.shiftright(w, pixels, agc0);
#endif /* WITHSECTRUMPEAKS */
			scrollcolor.shiftright(w, pixels, display2_bgcolorwfl());
			scrollpwr.shiftright(w, pixels, 0);
			/* one-row objects */
			spavgarray.shiftright(w, pixels, 0);
			yoldwfl.shiftright(w, pixels, 0);
			yoldspe.shiftright(w, pixels, 0);
			yold3dss.shiftright(w, pixels, 0);
		}
	}

private:
	void filtering_waterfall(uint_fast16_t x)
	{
		const FLOAT_t val = spavgarray.peek(x, 0);	// массив входных данных
		const FLOAT_t Y = yoldwfl.peek(x, 0) * waterfall_alpha + waterfall_beta * val;
		yoldwfl.poke(x, 0, Y);
	}

public:
	FLOAT_t filtered_waterfall(uint_fast16_t vx, uint_fast16_t alldx)
	{
		const uint_fast16_t bx = normalize(vx, 0, alldx - 1, ALLDX - 1);
		return yoldwfl.peek(bx, 0);
	}

private:
	void filtering_spectrum(uint_fast16_t x)
	{
		const FLOAT_t val = spavgarray.peek(x, 0);	// массив входных данных
		const FLOAT_t Y = yoldspe.peek(x, 0) * spectrum_alpha + spectrum_beta * val;
		yoldspe.poke(x, 0, Y);

#if WITHSECTRUMPEAKS
		//const FLOAT_t val = Y;//spavgarray.peek(x, 0);	// массив входных данных
		// Работа с пиковым детектором
		agcstate_t * const st = ypeakspe.bufferat(ALLDX, x, 0);
		agc_perform(& peakparams, st, val);
#endif /* WITHSECTRUMPEAKS */
	}

public:
	FLOAT_t filtered_spectrum(uint_fast16_t vx, uint_fast16_t alldx)
	{
		const uint_fast16_t bx = normalize(vx, 0, alldx - 1, ALLDX - 1);
		return yoldspe.peek(bx, 0);
//		agcstate_t * const st = ypeakspe.bufferat(ALLDX, bx, 0);
//		return agc_result_fast(st);
	}

	FLOAT_t peaks_spectrum(uint_fast16_t vx, uint_fast16_t alldx)
	{
#if WITHSECTRUMPEAKS
		const uint_fast16_t bx = normalize(vx, 0, alldx - 1, ALLDX - 1);
		agcstate_t * const st = ypeakspe.bufferat(ALLDX, bx, 0);
		return agc_result_slow(st);
#else /* WITHSECTRUMPEAKS */
		return filtered_spectrum(vx, alldx);
#endif /* WITHSECTRUMPEAKS */
	}

	// todo: свои параметры фильтрации?
private:
	void filtering_3dss(uint_fast16_t x)
	{
		const FLOAT_t val = spavgarray.peek(x, 0);	// массив входных данных
		const FLOAT_t Y = yold3dss.peek(x, 0) * waterfall_alpha + waterfall_beta * val;
		yold3dss.poke(x, 0, Y);
	}

public:
	// todo: свои параметры фильтрации?
	FLOAT_t filtered_3dss(uint_fast16_t vx, uint_fast16_t alldx)
	{
		const uint_fast16_t bx = normalize(vx, 0, alldx - 1, ALLDX - 1);
		return yold3dss.peek(bx, 0);
	}
public:
	void filtering()
	{
		uint_fast16_t x;
		for (x = 0; x < w; ++ x)
		{
			filtering_waterfall(x);
			filtering_spectrum(x);
			filtering_3dss(x);
		}
	}

};

typedef scrollbf<ALLDX, NROWSWFL> scbfi_t;
///////////////////
///
///

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


struct ustatesx
{
#if defined(ARM_MATH_NEON)
	FLOAT_t iir_state [ZOOMFFT_LPF_STAGES_IIR * 8];
#else /* defined(ARM_MATH_NEON) */
	FLOAT_t iir_state [ZOOMFFT_LPF_STAGES_IIR * 4];
#endif /* defined(ARM_MATH_NEON) */
	FLOAT_t fir_state [ZOOMFFT_DECIM_STAGES_FIR + LARGEFFT - 1];

	FLOAT_t cmplx_sig [NORMALFFT * 2];
	FLOAT_t ifspec_wndfn [NORMALFFT];

#if WITHVIEW_3DSS

	WFL3DSS_T wfj3dss [MAX_3DSS_STEP] [ALLDX];
	uint16_t depth_map_3dss [MAX_3DSS_STEP][ALLDX];
	uint16_t envelope_y [ALLDX];

#endif /* WITHVIEW_3DSS */

#if WITHVIEW_3DSS

	PACKEDCOLORPIP_T hist3dss [GXSIZE(ALLDX, MAX_3DSS_STEP)];		// массив цветных пикселей ландшавта
	SCAPEJVAL_T hist3dssvals [MAX_3DSS_STEP][ALLDX];	// массив высот (0..PALETTESIZE - 1)

#endif /* WITHVIEW_3DSS */

#if WITHAFSPECTRE
	FLOAT_t afspec_wndfn [WITHFFTSIZEAF];
	afsp_t afsp;
#endif /* WITHAFSPECTRE */

	PACKEDCOLORPIP_T wfpaletteI [PALETTESIZE];

};

static scbfi_t scbf;

#if CPUSTYLE_R7S721
#warning To be done
static union states
{
	struct ustatesx gvarsv;
	uint8_t rbfimage0 [758554];
	//uint8_t rbfimage0 [sizeof (struct ustatesx)];
} uu =
{
	.rbfimage0 = {
	#include BOARD_BITIMAGE_NAME
	}
};

/* получить расположение в памяти и количество элементов в массиве для загрузки FPGA */
const uint8_t * getrbfimage(size_t * count)
{
//	* count = 0;
//	return NULL;
	* count = sizeof uu.rbfimage0 / sizeof uu.rbfimage0 [0];
	return & uu.rbfimage0 [0];
}

#define gvars (uu.gvarsv)

#else /* (CPUSTYLE_R7S721 || 0) */

static struct ustatesx gvars;

#endif /* (CPUSTYLE_R7S721 || 0) */


#define SIZEOF_WFL3DSS (sizeof gvars.wfj3dss)
#define ADDR_WFL3DSS (gvars.wfj3dss)
#define ADDR_SCAPEARRAY (gvars.hist3dss)
#define ADDR_SCAPEARRAYVALS (gvars.hist3dssvals)
#define wfpalette (gvars.wfpaletteI)

// Получить цвет заполнения водопада при перестройке
static COLORPIP_T display2_bgcolorwfl(void)
{
	return wfpalette [0];
}

#if (WITHSPECTRUMWF && ! LCDMODE_DUMMY) || WITHAFSPECTRE

static void printsigwnd(void)
{
	int i;

	PRINTF("static const FLOAT_t gvars.ifspec_wndfn [%u] =\n", (unsigned) NORMALFFT);
	PRINTF("{\n");
	for (i = 0; i < NORMALFFT; ++ i)
	{
		int el = ((i + 1) % 4) == 0;
		PRINTF("\t" "%+1.20f%s", gvars.ifspec_wndfn [i], el ? ",\n" : ", ");
	}
	PRINTF("};\n");
}

#endif /*  (WITHSPECTRUMWF && ! LCDMODE_DUMMY) || WITHAFSPECTRE */

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
	return x * (rightfft - leftfft) / (dx - 1) + leftfft;
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
display2_af_spectre15_init(
		const gxdrawb_t * db_unused,	// NULL
		uint_fast8_t xgrid,
		uint_fast8_t ygrid,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)		// вызывать после display2_smeter15_init
{
	static subscribefloat_t afspectreregister;
	smeter_params_t * const smpr = & smprms [SMETER_TYPE_BARS][SM_STATE_RX];		// отображение НЧ спектра только для режима s-метра BARS

	gvars.afsp.x = GRID2X(xgrid) + smpr->gs;
	gvars.afsp.y = GRID2Y(ygrid) + SM_BG_H - 10;
	gvars.afsp.w = smpr->ge - smpr->gs;
	gvars.afsp.h = 40;
	gvars.afsp.is_ready = 0;

	VERIFY(ARM_MATH_SUCCESS == ARM_MORPH(arm_rfft_fast_init)(& gvars.afsp.rfft_instance, WITHFFTSIZEAF));
	ARM_MORPH(arm_nuttall4b)(gvars.afspec_wndfn, WITHFFTSIZEAF);	/* оконная функция для показа звукового спектра */

	subscribefloat(& speexoutfloat, & afspectreregister, NULL, afsp_save_sample);	// выход speex и фильтра
}


static void
display2_af_spectre15_latch(
		const gxdrawb_t * db_unused,	// NULL
		uint_fast8_t xgrid,
		uint_fast8_t ygrid,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
{
	if (gvars.afsp.is_ready)
	{
		const FLOAT_t m1 = 0.6;
		const FLOAT_t m2 = 1 - m1;
		const unsigned leftfftpos = freq2fft_af(glob_afspeclow);	// нижняя частота (номер бина) отлбражаемая на экране
		const unsigned rightfftpos = freq2fft_af(glob_afspechigh);	// последний бин буфера FFT, отобрааемый на экране (включитеоьно)

		fftzoom_af(gvars.afsp.raw_buf, AFSP_DECIMATIONPOW2, WITHFFTSIZEAF);
		// осталась половина буфера

		ARM_MORPH(arm_mult)(gvars.afsp.raw_buf, gvars.afspec_wndfn, gvars.afsp.raw_buf, WITHFFTSIZEAF); // apply window function
		//VERIFY(ARM_MATH_SUCCESS == ARM_MORPH(arm_rfft_fast)(& gvars.afsp.rfft_instance, gvars.afsp.raw_buf, gvars.afsp.fft_buf, 0)); // 0-прямое, 1-обратное
		ARM_MORPH(arm_rfft_fast)(& gvars.afsp.rfft_instance, gvars.afsp.raw_buf, gvars.afsp.fft_buf, 0); // 0-прямое, 1-обратное
		gvars.afsp.is_ready = 0;	// буфер больше не нужен... но он заполняется так же в user mode
		ARM_MORPH(arm_cmplx_mag)(gvars.afsp.fft_buf, gvars.afsp.fft_buf, WITHFFTSIZEAF);

		ASSERT(gvars.afsp.w <= ARRAY_SIZE(gvars.afsp.val_array));
		for (unsigned x = 0; x < gvars.afsp.w; x ++)
		{
			const uint_fast16_t fftpos = raster2fftsingle(x, gvars.afsp.w, leftfftpos, rightfftpos);
			ASSERT(fftpos < ARRAY_SIZE(gvars.afsp.fft_buf));
			// filterig
			gvars.afsp.val_array [x] = gvars.afsp.val_array [x] * m1 + m2 * gvars.afsp.fft_buf [fftpos];
		}
		ARM_MORPH(arm_max_no_idx)(gvars.afsp.val_array, gvars.afsp.w, & gvars.afsp.max_val);	// поиск в отображаемой части
		gvars.afsp.max_val = FMAXF(gvars.afsp.max_val, (FLOAT_t) 0.001);
	}
}

static void
display2_af_spectre15(const gxdrawb_t * db,
		uint_fast8_t xgrid,
		uint_fast8_t ygrid,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		)
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
						colpip_set_vline(db,
								gvars.afsp.x + x, gvars.afsp.y - y_norm, y_norm,
								DSGN_AFSPECTRE_COLOR);
					}
				}
#if WITHAA
				display_do_AA(db, GRID2X(xgrid), GRID2Y(ygrid), SM_BG_W, SM_BG_H);
#endif /* WITHAA */
			}
		}
		break;

	default:
		break;
	}
}

#endif /* WITHAFSPECTRE */


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
		.pIIRCoeffs = (const FLOAT_t [ZOOMFFT_LPF_STAGES_IIR * 5]){0.8384843639921, 0, 0, 0, 0, 1, 0.5130084793341, 1, 0.1784114407685, -0.6967733943344, 0.8744089756375, 0, 0, 0, 0, 1, 1.046379755684, 1, 0.3420998857106, -0.3982809814397, 1.83222755502, 0, 0, 0, 0, 1, 1.831496024383, 1, 0.5072844084012, -0.1179052535088, 0.01953722920982, 0, 0, 0, 0, 1, 0.3029841730578, 1, 0.09694668293684, -0.9095549467394, 1, 0, 0, 0, 0},
	},
	// x4 zoom lowpass
	{
		.zoom = 4,
		.numTaps = ZOOMFFT_DECIM_STAGES_FIR,
		.pFIRCoeffs = (const FLOAT_t [ZOOMFFT_DECIM_STAGES_FIR]){-0.05698952454792, 0.5574889164132, 0.5574889164132, -0.05698952454792},
		.pIIRCoeffs = (const FLOAT_t [ZOOMFFT_LPF_STAGES_IIR * 5]){0.6737499659657, 0, 0, 0, 0, 1, -1.102065194995, 1, 1.353694541279, -0.7896377861467, 0.53324811147, 0, 0, 0, 0, 1, -0.5853766477218, 1, 1.289175897987, -0.5882714065646, 0.6143152247695, 0, 0, 0, 0, 1, 1.182778527244, 1, 1.236309127239, -0.4063767082903, 0.01708381580242, 0, 0, 0, 0, 1, -1.245590418009, 1, 1.418191929315, -0.9374008035325, 1, 0, 0, 0, 0},
	},
	// x8 zoom lowpass
	{
		.zoom = 8,
		.numTaps = ZOOMFFT_DECIM_STAGES_FIR,
		.pFIRCoeffs = (const FLOAT_t [ZOOMFFT_DECIM_STAGES_FIR]){-0.05698952454792, 0.5574889164132, 0.5574889164132, -0.05698952454792},
		.pIIRCoeffs = (const FLOAT_t [ZOOMFFT_LPF_STAGES_IIR * 5]){0.6469981129046, 0, 0, 0, 0, 1, -1.750671284068, 1, 1.766710155669, -0.8829517893283, 0.4645312725883, 0, 0, 0, 0, 1, -1.553480572725, 1, 1.681513354365, -0.7637556184482, 0.2925692260954, 0, 0, 0, 0, 1, -0.1114766808264, 1, 1.601891439147, -0.6499504503566, 0.01652325734055, 0, 0, 0, 0, 1, -1.797298202754, 1, 1.831125104215, -0.9660534813317, 1, 0, 0, 0, 0},
	},
	// x16 zoom lowpass
	{
		.zoom = 16,
		.numTaps = ZOOMFFT_DECIM_STAGES_FIR,
		.pFIRCoeffs = (const FLOAT_t [ZOOMFFT_DECIM_STAGES_FIR]){-0.05698952454792, 0.5574889164132, 0.5574889164132, -0.05698952454792},
		.pIIRCoeffs = (const FLOAT_t [ZOOMFFT_LPF_STAGES_IIR * 5]){0.6500044972642, 0, 0, 0, 0, 1, -1.935616780918, 1, 1.908632776595, -0.9387888949475, 0.4599444315799, 0, 0, 0, 0, 1, -1.880017827578, 1, 1.851418291083, -0.8732990221737, 0.2087317940803, 0, 0, 0, 0, 1, -1.278402634611, 1, 1.794539349192, -0.80764043772, 0.01645106748385, 0, 0, 0, 0, 1, -1.948135342532, 1, 1.948194658987, -0.9825675157696, 1, 0, 0, 0, 0},
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
static IRQLSPINLOCK_t fftlock = IRQLSPINLOCK_INIT;
#define FFTLOCL_IRQL IRQL_REALTIME

// realtime-mode function
uint_fast8_t allocate_fftbuffer(fftbuff_t * * dest)
{
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& fftlock, & oldIrql, FFTLOCL_IRQL);
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
	IRQLSPIN_LOCK(& fftlock, & oldIrql, FFTLOCL_IRQL);
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
	IRQLSPIN_LOCK(& fftlock, & oldIrql, FFTLOCL_IRQL);
	InsertHeadList(& fftbuffree, & p->item);
	IRQLSPIN_UNLOCK(& fftlock, oldIrql);
}

// user-mode function
uint_fast8_t  getfilled_fftbuffer(fftbuff_t * * dest)
{
	IRQL_t oldIrql;
	IRQLSPIN_LOCK(& fftlock, & oldIrql, FFTLOCL_IRQL);
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
	IRQLSPINLOCK_INITIALIZE(& fftlock);
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

#if 0
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
	FLOAT_t IIRCoeffs_NEON [ZOOMFFT_LPF_STAGES_IIR * 8];

	// Initialize floating-point Biquad cascade filter.
	ARM_MORPH(arm_biquad_cascade_df2T_compute_coefs)(ZOOMFFT_LPF_STAGES_IIR, prm->pIIRCoeffs, IIRCoeffs_NEON);
	ARM_MORPH(arm_biquad_cascade_df2T_init)(& c.iir_config, ZOOMFFT_LPF_STAGES_IIR, IIRCoeffs_NEON, gvars.iir_state);

#else /* defined (ARM_MATH_NEON) */
	// Initialize floating-point Biquad cascade filter.
	ARM_MORPH(arm_biquad_cascade_df2T_init)(& c.iir_config, ZOOMFFT_LPF_STAGES_IIR, prm->pIIRCoeffs, gvars.iir_state);
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
	uint_fast8_t zoompow2	// horizontal magnification power of two
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
	dsp_cfft(& fftinstance, fftinpt, 0);	// forward transform
	ARM_MORPH(arm_cmplx_mag)(fftinpt, fftinpt, NORMALFFT);	/* Calculate magnitudes */

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

/* координаты в пикселях информации на спектре/вожопаде */
struct dispmap
{
	uint_fast16_t xcenter;
	uint_fast16_t xleft [2];	// Для каждого тракта приёма
	uint_fast16_t xright [2];	// Для каждого тракта приёма
	int_fast32_t f0pix;
	int_fast32_t bw;
	int_fast32_t f0;	// частота центра экрана
};

static int_fast32_t wffreqpix;			// глобальный пиксель по x центра спектра, для которой в последной раз отрисовали.
static uint_fast8_t wfzoompow2;				// масштаб, с которым выводили спектр
//static int_fast16_t wfhscroll;			// сдвиг по шоризонтали (отрицаельный - влево) для водопада.
//static uint_fast16_t wfvscroll;			// сдвиг по вертикали (в рабочем направлении) для водопада.
//static uint_fast8_t wfclear;			// стирание всей областии отображение водопада.
struct dispmap latched_dm;

#if WITHVIEW_3DSS
enum
{
	DEPTH_ATTENUATION = 2,
	MAX_DELAY_3DSS = 1
};

static uint_fast16_t current_3dss_step = 0;
static uint_fast16_t delay_3dss = MAX_DELAY_3DSS;

static WFL3DSS_T * atwfj3dss(uint_fast16_t x, uint_fast16_t y)
{
	ASSERT(x < ALLDX);
	ASSERT(y < MAX_3DSS_STEP);
	return & ADDR_WFL3DSS [y][x];
}

static uint_fast16_t wfj3dss_peek(uint_fast16_t x, uint_fast16_t y)
{
	return * atwfj3dss(x, y);
}

static void wfj3dss_poke(uint_fast16_t x, uint_fast16_t y, WFL3DSS_T val)
{
	* atwfj3dss(x, y) = val;
}

static void init_depth_map_3dss(void)
{
	const uint_fast16_t HALF_ALLDX = ALLDX / 2;

	for (int_fast16_t i = 0; i < MAX_3DSS_STEP; i ++)
	{
		uint_fast16_t range = HALF_ALLDX - 1 - i * Z_STEP_3DSS;

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

/* получить адрес пикселя из массива истории ландшафта */
static
PACKEDCOLORPIP_T *
atskapej(uint_fast16_t x, uint_fast16_t y)
{
	gxdrawb_t scapedbv;
	gxdrawb_initialize(& scapedbv, ADDR_SCAPEARRAY, ALLDX, MAX_3DSS_STEP);
	return colpip_mem_at(& scapedbv, x, y);
}

/* получить адрес значения высоты из массива истории ландшафта */
static
SCAPEJVAL_T *
atskapejval(uint_fast16_t x, uint_fast16_t y)
{
	return & ADDR_SCAPEARRAYVALS [y] [x];
}

#endif /* WITHVIEW_3DSS */


// стираем целиком старое изображение водопада
static void wflclear(void)
{
#if WITHVIEW_3DSS
	memset(ADDR_WFL3DSS, 0, SIZEOF_WFL3DSS);
#endif /* WITHVIEW_3DSS */

#if WITHVIEW_3DSS
	gxdrawb_t scapedbv;
	gxdrawb_initialize(& scapedbv, ADDR_SCAPEARRAY, ALLDX, MAX_3DSS_STEP);
	// стирание прямоугольника
	colpip_fillrect(& scapedbv, 0, 0, ALLDX, MAX_3DSS_STEP, display2_bgcolorwfl());
	arm_fill_q15(0, atskapejval(0, 0), ARRAY_SIZE(ADDR_SCAPEARRAYVALS));
#endif /* WITHVIEW_3DSS */

}

// частота увеличилась - надо сдвигать картинку влево
// нужно сохрянять часть старого изображения
static void wflshiftleft(uint_fast16_t pixels)
{
	uint_fast16_t y;

	if (pixels == 0)
		return;

#if WITHVIEW_3DSS
	gxdrawb_t scapedbv;
	gxdrawb_initialize(& scapedbv, ADDR_SCAPEARRAY, ALLDX, MAX_3DSS_STEP);
    for (y = 0; y < MAX_3DSS_STEP; ++ y)
 	{
 		memmove(
 				atwfj3dss(0, y),			// to
				atwfj3dss(pixels, y),		// from
 				(ALLDX - pixels) * sizeof (WFL3DSS_T)
 		);
 		memset(atwfj3dss(ALLDX - pixels, y), 0, pixels * sizeof (WFL3DSS_T));
	}

    // ландшафт
    for (y = 0; y < MAX_3DSS_STEP; ++ y)
 	{
 		memmove(
 				atskapej(0, y),		// to
 				atskapej(pixels, y),	// from
 				(ALLDX - pixels) * sizeof (PACKEDCOLORPIP_T)
 		);
 		memmove(
 				atskapejval(0, y),		// to
 				atskapejval(pixels, y),	// from
 				(ALLDX - pixels) * sizeof (SCAPEJVAL_T)
 		);
 	}
     // заполнение вновь появившегося прямоугольника
 	colpip_fillrect(& scapedbv, scapedbv.dx - pixels, 0, pixels, MAX_3DSS_STEP, display2_bgcolorwfl());
#endif /* WITHVIEW_3DSS */
}

// частота уменьшилась - надо сдвигать картинку вправо
// нужно сохрянять часть старого изображения
// в строке wfrow - новое
static void wflshiftright(uint_fast16_t pixels)
{
	uint_fast16_t y;

	if (pixels == 0)
		return;

#if WITHVIEW_3DSS
	gxdrawb_t scapedbv;
	gxdrawb_initialize(& scapedbv, ADDR_SCAPEARRAY, ALLDX, MAX_3DSS_STEP);
   	for (y = 0; y < MAX_3DSS_STEP; ++ y)
	{
   		memmove(
   				atwfj3dss(pixels, y),	// to
				atwfj3dss(0, y),		// from
				(ALLDX - pixels) * sizeof (WFL3DSS_T)
    		);
 		memset(atwfj3dss(0, y), 0, pixels * sizeof (WFL3DSS_T));
   	}

    // ландшафт
	for (y = 0; y < MAX_3DSS_STEP; ++ y)
	{
		memmove(
				atskapej(pixels, y),	// to
				atskapej(0, y),		// from
				(ALLDX - pixels) * sizeof (PACKEDCOLORPIP_T)
			);
		memmove(
				atskapejval(pixels, y),	// to
				atskapejval(0, y),		// from
				(ALLDX - pixels) * sizeof (SCAPEJVAL_T)
			);
	}
	// заполнение вновь появившегося прямоугольника
	colpip_fillrect(& scapedbv, 0, 0, pixels, MAX_3DSS_STEP, display2_bgcolorwfl());
#endif /* WITHVIEW_3DSS */
}

// при смене диапазона или частот  при отсутствии необзодимости сохранять часть старого изображения водопада
// стираем целиком старое изображение водопада
// в строке 0 - новое
static void wfsetupnew(void)
{
	wflclear();	// Очистка водопада
}

#if ! LINUX_SUBSYSTEM
	#include "dsp/window_functions.h"
#endif /* ! LINUX_SUBSYSTEM */


static void
display2_wfl_init(
		const gxdrawb_t * db_unused, 	// NULL
		uint_fast8_t x0, uint_fast8_t y0, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx)
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

	wflclear();	// Очистка водопада
//	avg_clear_spe();	// Сброс фильтра
//	avg_clear_wfl();	// Сброс фильтра
//	avg_clear_3dss();	// Сброс фильтра 3dss
#if WITHVIEW_3DSS
	init_depth_map_3dss();
#endif /* WITHVIEW_3DSS */
	scbf.setupnew();
}

// Получить абсолюьный пиксель горизонтальной позиции для заданой частоты
// Значения в пикселях меньше, чем частота в герцах - тип шире, чем uint_fast32_t не требуется
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

// получить горизонтальную позицию в окне для заданного отклонения в герцах
// Использовать для "динамической" разметки дисплея - риски, кратные 10 кГц.
// Возврат UINT16_MAX при невозможности отобразить запрошенную частоту в указанном окне
static
uint_fast16_t
deltafreq2x_abs(
	int_fast32_t f0,	// центральная частота в герцах
	int_fast16_t delta,	// отклонение от центральной частоты в герцах
	int_fast32_t bw,	// полоса обзора в герцах
	uint_fast16_t width	// ширина экрана в пикселях
	)
{
	const int_fast32_t fm = f0 + delta;	// частота маркера
	const int_fast32_t pm = deltafreq2abspix(fm, bw, width);	// абсолютный пиксель маркера
	const int_fast32_t freqleft = f0 - bw / 2;	// частота левого края окна
	const int_fast32_t pixleft = deltafreq2abspix(freqleft, bw, width);	// абсолютный пиксель левого края окна

	if (pm < pixleft)
		return UINT16_MAX;	// Левее левого края окна
	if ((pm - pixleft) >= (int) width)
		return UINT16_MAX;	// Правее правого края окна
	return pm - pixleft;
}

/* получение оконных координат границ полосы пропускания и центра спектра */
static void display2_getdispmap(struct dispmap * p)
{
	const int_fast32_t bw = display2_zoomedbw();
	const int_fast32_t f0 = hamradio_get_freq_pathi(0);	// частота центра экрана
	const int_fast32_t fz = 10000000;	// расчёт положений отногсительно одной и той же частоты - изюежать "прыгания" ихображения при перестройке
	unsigned pathi;

	for (pathi = 0; pathi < 2; ++ pathi)
	{
		const int_fast32_t df = hamradio_get_freq_pathi(pathi) - f0;
		p->xleft [pathi] = deltafreq2x_abs(fz, hamradio_getleft_bp(pathi) + df, bw, ALLDX);	// левый край шторки
		p->xright [pathi] = deltafreq2x_abs(fz, hamradio_getright_bp(pathi) + df, bw, ALLDX);	// правый край шторки
	}
	p->xcenter = deltafreq2x_abs(fz, 0, bw, ALLDX);	// маркер центральной частоты
	p->f0pix = deltafreq2abspix(f0, bw, ALLDX);	/* pixel of frequency at middle of spectrum */
	p->bw = bw;
	p->f0 = f0;
}

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
static void
display_colorgrid_xor(
	const gxdrawb_t * db,
	uint_fast16_t row0,	// вертикальная координата начала занимаемой области (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	int_fast32_t f0,	// center frequency
	int_fast32_t bw		// span
	)
{
	const int MARKERH = 10;

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
				if (isvisibletext(DIM_X, xtext, freqw))
				{
					colpip_string3_tbg(db, xtext, row0, buf2, colordigits);
					colpip_xor_vline(db, xmarker, row0 + MARKERH, h - MARKERH, colorgridlines);
				}
				else
					colpip_xor_vline(db, xmarker, row0, h, colorgridlines);
			}
		}
	}
	colpip_xor_vline(db, ALLDX / 2, row0, h, colorcmarker);	// center frequency marker
}

// отрисовка маркеров частот
static void
display_colorgrid_set(
	const gxdrawb_t * db,
	uint_fast16_t x,
	uint_fast16_t y,	// вертикальная координата начала занимаемой области (0..dy-1) сверху вниз
	uint_fast16_t w,	// ширина
	uint_fast16_t h,	// высота
	int_fast32_t f0,	// center frequency
	int_fast32_t bw,		// span
	const struct dispmap * dm
	)
{
	const uint_fast8_t markerh = 10;
	const int_fast32_t go = f0 % (int) glob_gridstep;	// шаг сетки
	const int_fast32_t gs = (int) glob_gridstep;	// шаг сетки
	const int_fast32_t halfbw = bw / 2;
	int_fast32_t df;	// кратное сетке значение

	// Маркеры уровней сигналов
	if (glob_lvlgridstep != 0)
	{
		int lvl;
		for (lvl = glob_topdb / glob_lvlgridstep * glob_lvlgridstep; lvl >= glob_bottomdb; lvl -= glob_lvlgridstep)
		{
			const int yval = dsp_mag2y(db2ratio(lvl), h - 1, glob_topdb, glob_bottomdb);
			if (yval > 0 && yval < (int) h)
				colpip_set_hline(db, x, y + yval, w, colorgridlines);	// Level marker
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
				if (isvisibletext(DIM_X, xtext, freqw))
				{
					colpip_string3_tbg(db, xtext, y, buf2, colordigits);
					colpip_set_vline(db, xmarker, y + markerh, h - markerh, colorgridlines);
				}
				else
					colpip_set_vline(db, xmarker, y, h, colorgridlines);
			}
		}
	}
	if (dm->xcenter != UINT16_MAX)
	{
		colpip_set_vline(db, x + dm->xcenter, y, h, colorcmarker);	// center frequency marker
	}
}

// отрисовка маркеров частот для 3DSS
static
void
display_colorgrid_3dss(
	const gxdrawb_t * db,
	uint_fast16_t row0,	// вертикальная координата начала занимаемой области (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	int_fast32_t f0,	// center frequency
	int_fast32_t bw		// span
	)
{
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
				if (isvisibletext(DIM_X, xtext, freqw))
					colpip_string3_tbg(db, xtext, row, buf2, colordigits);

				colpip_set_vline(db, xmarker, row0, h, colorgridlines3dss);
			}
		}
	}
	colpip_set_vline(db, ALLDX / 2, row0, h, colorcmarker);	// center frequency marker
}


// формирование данных спектра для последующего отображения
// спектра или водопада
static void display2_latchcombo(
		const gxdrawb_t * db_unused, 	// NULL
		uint_fast8_t x0, uint_fast8_t y0, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx)
{
	uint_fast16_t x, y;
	const uint_fast16_t alldx = GRID2X(xspan);

	// Сдвиг изображения при необходимости (перестройка/переклбчение диапащонов или масштаба).
	const uint_fast8_t pathi = 0;	// RX A
	display2_getdispmap(& latched_dm);

	int_fast16_t hscroll = 0;
//	uint_fast8_t hclear = 0;

	if (wffreqpix == 0 || wfzoompow2 != glob_zoomxpow2)
	{
		wfsetupnew(); // стираем целиком старое изображение водопада. в строке 0 - новое
		scbf.setupnew();
//		hclear = 1;
	}
	else if (wffreqpix == latched_dm.f0pix)
	{
		// не менялась частота (в видимых пикселях)
	}
	else if (wffreqpix > latched_dm.f0pix)
	{
		// частота уменьшилась - надо сдвигать картинку вправо
		const uint_fast32_t deltapix = wffreqpix - latched_dm.f0pix;
		if (deltapix < alldx / 2)
		{
			hscroll = (int_fast16_t) deltapix;
			// нужно сохрянять часть старого изображения
			wflshiftright(hscroll);
			scbf.shiftright(hscroll);
		}
		else
		{
			wfsetupnew(); // стираем целиком старое изображение водопада. в строке 0 - новое
			scbf.setupnew();
//			hclear = 1;
		}
	}
	else if (wffreqpix < latched_dm.f0pix)
	{
		// частота увеличилась - надо сдвигать картинку влево
		const uint_fast32_t deltapix = latched_dm.f0pix - wffreqpix;
		if (deltapix < alldx / 2)
		{
			hscroll = - (int_fast16_t) deltapix;
			// нужно сохрянять часть старого изображения
			wflshiftleft(- hscroll);
			scbf.shiftleft(- hscroll);
		}
		else
		{
			wfsetupnew(); // стираем целиком старое изображение водопада. в строке 0 - новое
			scbf.setupnew();
//			hclear = 1;
		}
	}

	// запоминание информации спектра для спектрограммы и 3DSS
	if (! dsp_getspectrumrow(scbf.spavgarray.bf(), alldx, glob_zoomxpow2))
		return;	// еще нет новых данных.

	scbf.shiftrows();	/* + продвижение по истории */
	scbf.filtering();

#if WITHVIEW_3DSS
	row3dss = (row3dss == 0) ? (MAX_3DSS_STEP - 1) : (row3dss - 1);
#endif /* WITHVIEW_3DSS */

#if 0 //WITHVIEW_3DSS
	// продвижение по истории
	delay_3dss = calcnext16(delay_3dss, MAX_DELAY_3DSS);
	if (! delay_3dss)
		current_3dss_step = calcnext16(current_3dss_step, MAX_3DSS_STEP);
#endif /* WITHVIEW_3DSS */

#if WITHVIEW_3DSS
	gxdrawb_t scapedbv;
	gxdrawb_initialize(& scapedbv, ADDR_SCAPEARRAY, ALLDX, MAX_3DSS_STEP);
#endif
	// формирование строки водопада
	for (x = 0; x < ALLDX; ++ x)
	{
		// для водопада
		const int valwfl = dsp_mag2y(scbf.filtered_waterfall(x, ALLDX), PALETTESIZE - 1, glob_topdb, glob_bottomdb); // возвращает значения от 0 до dy включительно
		const int val3dss = dsp_mag2y(scbf.filtered_3dss(x, ALLDX), INT16_MAX, glob_topdb, glob_bottomdb); // возвращает значения от 0 до dy включительно
		scbf.scrollpwr.poke(x, 0, val3dss);
	#if LCDMODE_MAIN_L8
		//colpip_putpixel(& wfjdbv, x, wfrow, valwfl);	// запись в буфер водопада индекса палитры
		scbf.scrollcolor.poke(x, 0, valwfl);
		#if WITHVIEW_3DSS
			colpip_putpixel(ADDR_SCAPEARRAY, ALLDX, NROWSWFL, x, row3dss, val3dss);	// запись в буфер водопада индекса палитры
			* atskapejval(x, row3dss) = val3dss;
		#endif /* WITHVIEW_3DSS */
		#if WITHVIEW_3DSS
			* atskapejval(x, row3dss) = val3dss;
		#endif /* WITHVIEW_3DSS */
	#else /* LCDMODE_MAIN_L8 */
		ASSERT(valwfl >= 0);
		ASSERT(valwfl < (int) ARRAY_SIZE(wfpalette));
		//colpip_putpixel(& wfjdbv, x, wfrow, wfpalette [valwfl]);	// запись в буфер водопада цветовой точки
		scbf.scrollcolor.poke(x, 0, wfpalette [valwfl]);
		#if WITHVIEW_3DSS
			ASSERT(val3dss >= 0);
			ASSERT(val3dss <= INT16_MAX);
			colpip_putpixel(& scapedbv, x, row3dss, wfpalette [valwfl]);	// запись в буфер водопада цветовой точки
			* atskapejval(x, row3dss) = val3dss;
		#endif /* WITHVIEW_3DSS */
	#endif /* LCDMODE_MAIN_L8 */
	}

	wffreqpix = latched_dm.f0pix;
	wfzoompow2 = glob_zoomxpow2;
//	wfhscroll += hscroll;
//	wfvscroll = wfvscroll < NROWSWFL ? wfvscroll + 1 : NROWSWFL;
//	wfclear = hclear;
	(void) x0;
	(void) y0;
	(void) yspan;
	(void) pctx;
}
// подготовка изображения спектра
static void display2_spectrum(const gxdrawb_t * db, uint_fast8_t x0, uint_fast8_t y0, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx)
{
	const uint_fast16_t x0pix = GRID2X(x0);
	const uint_fast16_t y0pix = GRID2Y(y0);				// смещение по вертикали в пикселях части отведенной спектру
	const uint_fast16_t alldx = GRID2X(xspan);
	const uint_fast16_t alldy = GRID2Y(yspan);				// размер по вертикали в пикселях части отведенной спектру
	// Спектр на цветных дисплеях, не поддерживающих ускоренного
	// построения изображения по bitmap с раскрашиванием

	uint_fast8_t pathi = 0;	// RX A

	if (yspan == 0)
		return;

	const uint_fast32_t f0 = latched_dm.f0;	/* frequency at middle of spectrum */
	const int_fast32_t bw = latched_dm.bw;
	/* рисуем спектр ломанной линией */
	/* стираем старый фон */
	colpip_fillrect(db, x0pix, y0pix, alldx, alldy, DSGN_SPECTRUMBG);
	// сохранияем дянные для отображения
	int vals [alldx];
	int peaks [alldx];
	for (uint_fast16_t x = 0; x < alldx; ++ x)
	{
#if WITHSPECTRUMWF
		vals [x] = dsp_mag2y(scbf.filtered_spectrum(x, alldx), alldy - 1, glob_topdb, glob_bottomdb);
		peaks [x] = dsp_mag2y(scbf.peaks_spectrum(x, alldx), alldy - 1, glob_topdb, glob_bottomdb);
#else
		vals [x] = (x * (alldy - 1) / (alldx - 1));	// debug
		peaks [x] = (x * (alldy - 1) / (alldx - 1));	// debug
#endif /* WITHSPECTRUMWF */
	}

	if (! colpip_hasalpha())
	{
		// Изображение "шторки" под спектром.
		uint_fast8_t splitflag;
		uint_fast8_t pathi;
		hamradio_get_vfomode3_value(& splitflag);
		for (pathi = 0; pathi < (splitflag ? 2 : 1); ++ pathi)
		{
			const COLORPIP_T rxbwcolor = display2_rxbwcolor(pathi ? DSGN_SPECTRUMBG2RX2 : DSGN_SPECTRUMBG2, DSGN_SPECTRUMBG);
			uint_fast16_t xleft = latched_dm.xleft [pathi];		// левый край шторки
			uint_fast16_t xright = latched_dm.xright [pathi];	// правый край шторки
			if (xleft == UINT16_MAX || xright == UINT16_MAX)
				continue;
			if (xleft > xright)
				xleft = 0;
			if (xright == xleft)
				xright = xleft + 1;
			if (xright >= alldx)
				xright = alldx - 1;

			const uint_fast16_t xrightv = xright + 1;	// рисуем от xleft до xright включительно

			if (xleft < xrightv)
			{
				colpip_fillrect(db, x0pix + xleft, y0pix, xrightv - xleft, alldy, rxbwcolor);
			}
		}
	}

	uint_fast16_t ylast = 0;
	display_colorgrid_set(db, x0pix, y0pix, alldx, alldy, f0, bw, & latched_dm);	// отрисовка маркеров частот

	if (1)
	{
		// пиковые значения спектра
		for (uint_fast16_t x = 0; x < alldx; ++ x)
		{
			int val;
			for (val = peaks [x]; val > vals [x]; -- val)
			{
				uint_fast16_t ynew = y0pix + alldy - 1 - val;
				colpip_point(db, x0pix + x, ynew, DSGN_SPECTRUMPEAKS);
				break;	// только одна точка
    		}
		}
	}
	if (1)
	{
		for (uint_fast16_t x = 0; x < alldx; ++ x)
		{
			// ломанная
			const int val = vals [x];
			uint_fast16_t ynew = y0pix + alldy - 1 - val;

			if (glob_view_style == VIEW_COLOR) 		// раскрашенный цветовым градиентом спектр
			{
				for (uint_fast16_t y = y0pix + alldy - 1, i = 0; y > ynew; y --, i ++)
				{
					const uint_fast16_t ix = normalize(i, 0, alldy - 1, PALETTESIZE - 1);
					colpip_point(db, x0pix + x, y, wfpalette [ix]);
				}
			}
			else if (glob_view_style == VIEW_FILL) // залитый зеленым спектр
			{
				colpip_set_vline(db, x0pix + x, ynew, alldy + y0pix - ynew, DSGN_SPECTRUMFG);
			}
			else if (glob_view_style == VIEW_DOTS) // Отдельные точки
			{
				colpip_point(db, x0pix + x, ynew, DSGN_SPECTRUMFG);
			}

			if (x && glob_view_style != VIEW_DOTS)
			{
				colpip_line(db, x0pix + x - 1, ylast, x0pix + x, ynew, DSGN_SPECTRUMLINE, 1);
			}
			ylast = ynew;
		}
	}

	if (colpip_hasalpha())
	{
		// Изображение "шторки" на спектре.
		uint_fast8_t splitflag;
		uint_fast8_t pathi;
		hamradio_get_vfomode3_value(& splitflag);
		for (pathi = 0; pathi < (splitflag ? 2 : 1); ++ pathi)
		{
			uint_fast16_t xleft = latched_dm.xleft [pathi];		// левый край шторки
			uint_fast16_t xright = latched_dm.xright [pathi];	// правый край шторки
			if (xleft == UINT16_MAX || xright == UINT16_MAX)
				continue;
			if (xleft > xright)
				xleft = 0;
			if (xright == xleft)
				xright = xleft + 1;
			if (xright >= alldx)
				xright = alldx - 1;
			const uint_fast16_t xrightv = xright + 1;	// рисуем от xleft до xright включительно
			if (xleft < xrightv)
			{
				colpip_rectangle(
						db,
						xleft + x0pix, y0pix,
						xrightv - xleft, alldy, // размер окна заполнения
						pathi ? DSGN_SPECTRUMBG2RX2 : DSGN_SPECTRUMBG2,
						FILL_FLAG_MIXBG, gbwalpha
					);
			}
		}
	}
	(void) pctx;
}

#if WITHVIEW_3DSS

// координаты наблюдателя относительно левого верхнего угла параллелепипеда с 3dss историей
typedef struct mapscene_view
{
	int_fast16_t x;
	int_fast16_t y;
	int_fast16_t z;
} mapview_t;

// Возвращает параметры для преобразования координат
static int mapscene_calc(
	int_fast16_t x, 	// координата слева направо (0..box->y-1)
	int_fast16_t y, 	// координата сверзу вниз (0..box->y-1)
	int_fast16_t z,		// удаление от передней стенки (0..box->z-1)
	const mapview_t * vp,	// координаты наблюдателя
	const mapview_t * box, 	// размеры пространства исходных точек
	int * pdiv
)
{
	const int dx = vp->x - x;
	const int dy = vp->y - y;
	const int dz = vp->z - z;	// дальность
	// расчёт масштаба - пока по расстоянию только одной координаты z
	const int multiplier = (- vp->z - 1);
	const int divisor = (dz - 1);

	* pdiv = divisor;
	return multiplier;
}

/*
 * Получить горизонтальную координату отображения на передннюю стенку точки с координатами x, y, z в параллелепипеде
 * находящиеся на переднем плане (z == 0) не корректируются
 */
static int_fast16_t
mapscene_x(
	int_fast16_t x, 	// координата слева направо (0..box->y-1)
	int_fast16_t y, 	// координата сверзу вниз (0..box->y-1)
	int_fast16_t z,		// удаление от передней стенки (0..box->z-1)
	const mapview_t * vp,	// координаты наблюдателя
	const mapview_t * box, 	// размеры пространства исходных точек
	int multiplier,
	int divisor
	)
{
	return vp->x + (vp->x - x) * multiplier / divisor;	// скорректированная координата
}

/*
 * Получить вертикальную координату отображения на передннюю стенку точки с координатами x, y, z в параллелепипеде
 * находящиеся на переднем плане (z == 0) не корректируются
 */
static int_fast16_t
mapscene_y(
	int_fast16_t x, 	// координата слева направо (0..box->y-1)
	int_fast16_t y, 	// координата сверзу вниз (0..box->y-1)
	int_fast16_t z,		// удаление от передней стенки (0..box->z-1)
	const mapview_t * vp,	// координаты наблюдателя
	const mapview_t * box, 	// размеры пространства исходных точек
	int multiplier,
	int divisor
	)
{
	return vp->y + (vp->y - y) * multiplier / divisor;	// скорректированная координата
}

// отрисовка изображения спектра в 3D проекции
static void display2_3dss_alt(const gxdrawb_t * db, uint_fast8_t x0, uint_fast8_t y0, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx)
{
	const int_fast16_t x0pix = GRID2X(x0);
	const int_fast16_t y0pix = GRID2Y(y0);
	const int_fast16_t alldx = GRID2X(xspan);
	const int_fast16_t alldy = GRID2Y(yspan);
	const uint_fast32_t f0 = latched_dm.f0;	/* frequency at middle of spectrum */
	const int_fast32_t bw = latched_dm.bw;
	// координаты наблюдателя относительно левого верхнего угла параллелепипеда с 3dss историей
	const mapview_t vp =
	{
		.x = alldx / 2,	// смотрим с середины окна
		.y = 0,			// От верхнего края
		.z = - 500 //- MAX_3DSS_STEP * 2		// пока от балды - удаление от передней стенки паралеепипеда
	};
	// размеры пространства исходных точек
	static const mapview_t box =
	{
		.x = ALLDX,
		.y = PALETTESIZE,
		.z = MAX_3DSS_STEP
	};
	int_fast16_t zfofward;

	//colpip_fillrect(db, x0pix, y0pix, alldx, alldy, display2_getbgcolor());
	for (zfofward = 0; zfofward < MAX_3DSS_STEP; ++ zfofward)
	{
		const int_fast16_t z = zfofward;//MAX_3DSS_STEP - 1 - zfofward;	// начинаем рисовать с самой дальней строки истории
		const uint_fast16_t zrow = (row3dss + zfofward) % MAX_3DSS_STEP;	// строка в буфере - c "заворотом"
		int_fast16_t xw;	// позиция в окне слева направо
		for (xw = 0; xw < alldx; ++ xw)
		{
			const int_fast16_t x = normalize(xw, 0, alldx - 1, ALLDX - 1);
			const SCAPEJVAL_T val3dss = * atskapejval(x, zrow);	// (0..INT16_MAX)
			const int_fast16_t y = alldy - 1 - normalize(val3dss, 0, INT16_MAX, alldy - 1);
			int divisor;
			const int multiplier = mapscene_calc(x, y, z, & vp, & box, & divisor);
			const int_fast16_t xmap = mapscene_x(x, y, z, & vp, & box, multiplier, divisor);
			const int_fast16_t ymap = mapscene_y(x, y, z, & vp, & box, multiplier, divisor);
			// координаты выходят за границы окна - не рисуем
			if (xmap < 0 || xmap >= alldx || ymap < 0 || ymap >= alldy)
				continue;
			const COLORPIP_T color = * atskapej(x, zrow);
			colpip_point(db, x0pix + xmap, y0pix + ymap, color);
			//colpip_set_vline(db, x0pix + xmap, y0pix + ymap, alldy - 1 - ymap, color);
		}
	}
	{
		// Отрисовать передний план линией
		const uint_fast16_t zrow = row3dss;	// строка в буфере - откуда берём информацию о мощности
		int_fast16_t xw;	// позиция в окне слева направо
		int_fast16_t oldx, oldy;
		for (xw = 0; xw < alldx; ++ xw)
		{
			const int_fast16_t x = normalize(xw, 0, alldx - 1, ALLDX - 1);
			const SCAPEJVAL_T val3dss = * atskapejval(x, zrow);	// (0..INT16_MAX)
			const int_fast16_t y = alldy - 1 - normalize(val3dss, 0, INT16_MAX, alldy - 1);
			if (xw == 0)
			{
				oldx = x0pix + x;
				oldy = y0pix + y;
			}
			else
			{
				colpip_line(db, oldx, oldy, x0pix + x, y0pix + y, COLORPIP_YELLOW, 0);
				oldx = x0pix + x;
				oldy = y0pix + y;
			}
		}
	}
	// todo: сделать так,, чтобы вписывалось в разрешенный прямоугольник
	//display_colorgrid_3dss(colorpip, y0pix, alldy, f0, bw);
}
#endif /* WITHVIEW_3DSS */

#if WITHVIEW_3DSS

// подготовка изображения спектра
static void display2_3dss(const gxdrawb_t * db0, uint_fast8_t x0, uint_fast8_t y0, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx)
{
	const uint_fast16_t x0pix = GRID2X(x0);
	const uint_fast16_t y0pix = GRID2Y(y0);
	PACKEDCOLORPIP_T * const colorpip = colpip_mem_at(db0, x0pix, y0pix);
	gxdrawb_t dbv;
	gxdrawb_initialize(& dbv, colpip_mem_at(db0, x0pix, y0pix), db0->dx, db0->dy - y0pix);
	const uint_fast16_t alldx = GRID2X(xspan);
	const uint_fast16_t alldy = GRID2Y(yspan);
	const uint_fast16_t SPY_3DSS = GRID2Y(yspan);	// was: SPDY
	const uint_fast16_t SPY_3DSS_H = SPY_3DSS / 4;
	const uint_fast16_t SPY = GRID2Y(yspan) - 15;	// 15 - высота шрифта частотных маркеров
	const uint_fast16_t HORMAX_3DSS = SPY - MAX_3DSS_STEP * Z_STEP_3DSS - 2;
	const uint_fast32_t f0 = latched_dm.f0;	/* frequency at middle of spectrum */
	const int_fast32_t bw = latched_dm.bw;
	uint_fast16_t xleft = latched_dm.xleft [0];		// левый край шторки
	uint_fast16_t xright = latched_dm.xright [0];	// правый край шторки
	if (xleft == UINT16_MAX || xright == UINT16_MAX)
		return;
	if (xleft > xright)
		xleft = 0;
	if (xright == xleft)
		xright = xleft + 1;
	if (xright >= alldx)
		xright = alldx - 1;

	const uint_fast16_t xrightv = xright + 1;	// рисуем от xleft до xright включительно

	uint_fast16_t draw_step = calcprev16(current_3dss_step, MAX_3DSS_STEP);
	uint_fast16_t ylast_sp = 0;
	int i;
	const COLORPIP_T bgcolor = display2_getbgcolor();
	for (int_fast16_t i = 0; i < MAX_3DSS_STEP - 1; i ++)
	{
		uint_fast16_t y0 = SPY - 5 - i * Z_STEP_3DSS;
		uint_fast16_t x;

		uint_fast16_t x_old = UINT16_MAX;
		for (x = 0; x < alldx; ++ x)
		{
			if (i == 0)
			{
				// Самый ближний к зрителю (самый свежий)
				const int val = dsp_mag2y(scbf.filtered_spectrum(x, alldx), HORMAX_3DSS - 1, glob_topdb, glob_bottomdb);
				uint_fast16_t ynew = SPY - 1 - val;
				uint_fast16_t dy, j;
				wfj3dss_poke(x, current_3dss_step, val);

				for (dy = SPY - 1, j = 0; dy > ynew; dy --, j ++)
				{
					if (x > xleft && x < xrightv && gview3dss_mark)
						colpip_point(& dbv, x, dy, DSGN_SPECTRUMFG);
					else
					{
						const uint_fast16_t ix = normalize(j, 0, HORMAX_3DSS - 1, PALETTESIZE - 1);
						colpip_point(& dbv, x, dy, wfpalette [ix]);
					}
				}

				if (x)
				{
					colpip_line(& dbv, x - 1, ylast_sp, x, ynew, COLORPIP_WHITE, 1);
				}

				gvars.envelope_y [x] = ynew - 2;
				ylast_sp = ynew;
			}
			else
			{
				// Не самый ближний к зрителю (самый свежий)
				// i > 0
				uint_fast16_t x_d = gvars.depth_map_3dss [i - 1][x];

				if (x_d >= ALLDX)
					return;

				if (x_old != x_d)
				{
					const uint_fast16_t t0 = wfj3dss_peek(x, draw_step);
					//ASSERT(y0 >= t0);
					if (y0 >= t0)
					{
						uint_fast16_t y1 = y0 - t0;
						int_fast16_t h = y0 - y1 - i / DEPTH_ATTENUATION;		// высота пика
						h = h < 0 ? 0 : h;
						h = h > (int) y0 ? y0 : h;

						for (; h > 0; h --)
						{
							ASSERT((int) y0 >= h);
							/* предотвращение отрисовки по ранее закрашенной области*/
							if (* colpip_mem_at(& dbv, x_d, y0 - h) != bgcolor)
								break;
	                        const uint_fast16_t ix = normalize(h, 0, HORMAX_3DSS - 1, PALETTESIZE - 1);
							colpip_point(& dbv, x_d, y0 - h, wfpalette [ix]);
						}
					}
					x_old = x_d;
				}
			}
		}
		draw_step = calcprev16(draw_step, MAX_3DSS_STEP);
	}

	// увеличение контрастности спектра на фоне панорамы
	ylast_sp = SPY;
	for (uint_fast16_t x = 0; x < alldx; ++ x)
	{
		uint_fast16_t y1 = gvars.envelope_y [x];

		if (y1 >= DIM_Y)
			break;

		if (x)
			colpip_line(& dbv, x - 1, ylast_sp, x, y1, COLORPIP_BLACK, 0);

		ylast_sp = y1;
	}

	display_colorgrid_3dss(& dbv, SPY - SPY_3DSS_H + 3, SPY_3DSS_H, f0, bw);
	(void) pctx;

#if WITHVIEW_3DSS
	// продвижение по истории
	delay_3dss = calcnext16(delay_3dss, MAX_DELAY_3DSS);
	if (! delay_3dss)
		current_3dss_step = calcnext16(current_3dss_step, MAX_3DSS_STEP);
#endif /* WITHVIEW_3DSS */
}
#endif /* WITHVIEW_3DSS */

// Подготовка изображения водопада
static void display2_waterfall(const gxdrawb_t * db, uint_fast8_t x0, uint_fast8_t y0, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx)
{
	const uint_fast16_t x0pix = GRID2X(x0);				// смещение по вертикали в пикселях части отведенной водопаду
	const uint_fast16_t y0pix = GRID2Y(y0);				// смещение по вертикали в пикселях части отведенной водопаду
	const uint_fast8_t xBDCV_WFLRX = yspan;				// вертикальный размер водопада в ячейках
	const uint_fast16_t wfdy = GRID2Y(xBDCV_WFLRX);		// размер по вертикали в пикселях части отведенной водопаду
	const uint_fast16_t wfdx = GRID2X(xspan);
	gxdrawb_t wfjdbv;
	gxdrawb_initialize(& wfjdbv, scbf.scrollcolor.bf(), ALLDX, NROWSWFL);

#if ! LCDMODE_MAIN_L8
	// следы спектра ("водопад") на цветных дисплеях
	/* быстрое отображение водопада (но требует больше памяти) */
	const uint_fast16_t wfrow = scbf.getwfrow();
	const uint_fast16_t p1h = ulmin16(NROWSWFL - wfrow, wfdy);	// высота верхней части в результируюшем изображении
	const uint_fast16_t p2h = ulmin16(wfrow, wfdy - p1h);		// высота нижней части в результируюшем изображении
	const uint_fast16_t p1y = y0pix;
	const uint_fast16_t p2y = y0pix + p1h;

	if (yspan == 0)
		return;
	if (p1h != 0)	// всегда есть хоть одна строка
	{
		/* перенос свежей части растра */
		colpip_bitblt(
				db->cachebase, db->cachesize,
				db,
				0, p1y,	// координаты получателя
				wfjdbv.cachebase, wfjdbv.cachesize,	// папаметры для clean
				& wfjdbv,	// источник
				0, wfrow,	// координаты окна источника
				wfdx, p1h, 	// размеры окна источника
				BITBLT_FLAG_NONE, 0);
	}
	if (p2h != 0)
	{
		/* перенос старой части растра */
		colpip_bitblt(
				db->cachebase, 0 * db->cachesize,
				db,
				0, p2y,		// координаты получателя
				wfjdbv.cachebase, wfjdbv.cachesize,	// папаметры для clean
				& wfjdbv,	// источник
				0, 0,	// координаты окна источника
				wfdx, p2h, 	// размеры окна источника
				BITBLT_FLAG_NONE, 0);
	}

	if (hamradio_get_bringtuneA())
	{
		uint_fast8_t splitflag;
		uint_fast8_t pathi;
		hamradio_get_vfomode3_value(& splitflag);
		for (pathi = 0; pathi < (splitflag ? 2 : 1); ++ pathi)
		{
			uint_fast16_t xleft = latched_dm.xleft [pathi];		// левый край шторки
			uint_fast16_t xright = latched_dm.xright [pathi];	// правый край шторки

			if (xleft != UINT16_MAX && xright != UINT16_MAX)
			{
				if (colpip_hasalpha())
				{
					if (xleft > xright)
						xleft = 0;
					if (xright == xleft)
						xright = xleft + 1;
					if (xright >= wfdx)
						xright = wfdx - 1;

					const uint_fast16_t xrightv = xright + 1;	// рисуем от xleft до xright включительно
					/* Отрисовка прямоугольникв ("шторки") полосы пропускания на водопаде. */
					colpip_rectangle(
							db,
							xleft, y0pix,
							xrightv - xleft, wfdy, // размер окна источника
							pathi ? DSGN_SPECTRUMBG2RX2 : DSGN_SPECTRUMBG2,
							FILL_FLAG_MIXBG, gbwalpha
						);

				}
				else
				{
					const COLORPIP_T rxbwcolor = display2_rxbwcolor(pathi ? DSGN_SPECTRUMBG2RX2 : DSGN_SPECTRUMBG2, DSGN_SPECTRUMBG);
					// Изображение двух вертикальных линий по краям "шторки".
					colpip_set_vline(db, xleft, y0pix, wfdy, rxbwcolor);
					colpip_set_vline(db, xright, y0pix, wfdy, rxbwcolor);
					//colpip_fillrect(db, xleft, y0pix, xrightv - xleft, wfdy, COLORPIP_WHITE);
				}
			}
		}
	}

#else /* */

	// следы спектра ("водопад") на цветных дисплеях
	uint_fast16_t y;

	// формирование растра
	// следы спектра ("водопад")
	for (y = 0; y < wfdy; ++ y)
	{
		uint_fast16_t x;
		for (x = 0; x < wfdx; ++ x)
		{
			colpip_point(db, x0pix + x, y0pix + y, wfpalette [scbf.scrollcolor.peek(x, y)]);
		}
	}

#endif /*  */

	(void) x0;
	(void) y0;
	(void) pctx;
}



// подготовка изображения спектра и волрада
static void display2_gcombo(const gxdrawb_t * db, uint_fast8_t xgrid, uint_fast8_t ygrid, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx)
{
	const uint_fast8_t hspectrum = (uint_fast16_t) yspan * glob_spectrumpart / 100;
	switch (glob_view_style)
	{
#if WITHVIEW_3DSS
	case VIEW_3DSS:
		display2_3dss(db, xgrid, ygrid, xspan, yspan, pctx);
		//display2_3dss_alt(db, xgrid, ygrid, xspan, yspan, pctx);
		break;
#endif /* WITHVIEW_3DSS */
	default:
		// Делим отведённый размер между двумя панелями отображения
		display2_spectrum(db, xgrid, ygrid + 0, xspan, hspectrum, pctx);
		display2_waterfall(db, xgrid, ygrid + hspectrum, xspan, yspan - hspectrum, pctx);
		break;
	}
}

static
PACKEDCOLORPIP_T * getscratchwnd(const gxdrawb_t * db,
		uint_fast8_t x0,
		uint_fast8_t y0
	)
{
    return colpip_mem_at(db, GRID2X(x0), GRID2Y(y0));
}


#else /* WITHSPECTRUMWF && ! LCDMODE_DUMMY */

static
PACKEDCOLORPIP_T * getscratchwnd(const gxdrawb_t * db,
		uint_fast8_t x0,
		uint_fast8_t y0
	)
{
	return NULL;
}

// Stub
static void display2_latchcombo(const gxdrawb_t * db, uint_fast8_t x0, uint_fast8_t y0, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx)
{
}

// Stub
static void display2_spectrum(PACKEDCOLORPIP_T * const colorpipu, int_fast8_t x0, uint_fast8_t y0, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx)
{
}

// Stub
static void display2_waterfall(const gxdrawb_t * db, uint_fast8_t x0, uint_fast8_t y0, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx)
{
}

// Stub
static void display2_gcombo(const gxdrawb_t * db, uint_fast8_t x0, uint_fast8_t y0, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx)
{
}

// Stub
static void display2_wfl_init(const gxdrawb_t * db, uint_fast8_t x0, uint_fast8_t y0, uint_fast8_t xspan, uint_fast8_t yspan, dctx_t * pctx)
{

}

#endif /* WITHSPECTRUMWF && ! LCDMODE_DUMMY */


#if WITHLVGL

// отображение 3DSS

#define MY_CLASS_3DSS (& lv_sscp3dss_class)	// собственный renderer

static void lv_sscp3dss_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_sscp3dss_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_sscp3dss_event(const lv_obj_class_t * class_p, lv_event_t * e);

static const lv_obj_class_t lv_sscp3dss_class  =
{
    .base_class = & lv_obj_class,
    .constructor_cb = lv_sscp3dss_constructor,
    .destructor_cb = lv_sscp3dss_destructor,
    .event_cb = lv_sscp3dss_event,
    .name = "hmr_sscp3dss",
    .instance_size = sizeof (lv_sscp3dss_t),
};

// Рисуем спектр примитивами LVGL
void lv_sscp3dss_draw(lv_sscp3dss_t * const sscp3dss, lv_layer_t * layer, const lv_area_t * coord)
{
    //PRINTF("lv_sscp2_draw: w/h=%d/%d, x/y=%d/%d\n", (int) lv_area_get_width(coords), (int) lv_area_get_height(coords), (int) coords->x1, (int) coords->y1);
	const int32_t alldx = lv_area_get_width(coord);
	const int32_t alldy = lv_area_get_height(coord);
	//lv_draw_buf_t * db = (lv_draw_buf_t *) lv_draw_layer_alloc_buf(layer);
#if WITHSPECTRUMWF
	const struct dispmap * const dm = & latched_dm;
#endif /* WITHSPECTRUMWF */

	// Формирование изображения.
    if (1)
    {
    	// закрасить фон
        lv_draw_rect_dsc_t rect;
        lv_draw_rect_dsc_init(& rect);
        rect.bg_color = display_lvlcolor(DSGN_SPECTRUMBG); //lv_palette_main(LV_PALETTE_GREY);
        rect.bg_opa = LV_OPA_COVER;
    	lv_draw_rect(layer, & rect, coord);
    }

#if WITHSPECTRUMWF
    if (0)
    {
    	// построение 3ss
    }
#endif /* WITHSPECTRUMWF */

#if WITHSPECTRUMWF
    if (1)
    {
    	// сетка
    	const uint_fast32_t f0 = dm->f0;	/* frequency at middle of spectrum */
    	const int_fast32_t bw = dm->bw;

    	const uint_fast8_t markerh = 10;
    	const int_fast32_t go = f0 % (int) glob_gridstep;	// шаг сетки
    	const int_fast32_t gs = (int) glob_gridstep;	// шаг сетки
    	const int_fast32_t halfbw = bw / 2;
    	int_fast32_t df;	// кратное сетке значение

    	// Маркеры уровней сигналов
    	if (0 && glob_lvlgridstep != 0)
    	{
    		int32_t lvl;
    		for (lvl = glob_topdb / glob_lvlgridstep * glob_lvlgridstep; lvl >= glob_bottomdb; lvl -= glob_lvlgridstep)
    		{
    			const int32_t yval = dsp_mag2y(db2ratio(lvl), alldy - 1, glob_topdb, glob_bottomdb);
    			if (yval > 0 && yval < alldy)
    			{
   		            lv_draw_line_dsc_t l;
					lv_draw_line_dsc_init(& l);

					l.width = 1;
					l.round_end = 0;
					l.round_start = 0;
					l.colorgridlines = display_lvlcolor(colorgridlines);

					lv_point_precise_set(& l.p1, coord->x1, coord->y1 + yval);
					lv_point_precise_set(& l.p2, coord->x2, coord->y1 + yval);
					lv_draw_line(layer, & l);

    				//colpip_set_hline(db, x, y + yval, w, colorgridlines);	// Level marker
    			}
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

    				lv_draw_line_dsc_t l;
		            lv_draw_line_dsc_init(& l);

		            l.width = 1;
		            l.round_end = 0;
		            l.round_start = 0;
		            l.colorgridlines = display_lvlcolor(colorgridlines);

		            lv_point_precise_set(& l.p1, coord->x1 + xmarker, coord->y1);
		            lv_point_precise_set(& l.p2, coord->x1 + xmarker, coord->y2);
		            lv_draw_line(layer, & l);
					//colpip_set_vline(db, xmarker, y + markerh, alldy - markerh, colorgridlines);

		            // текст маркера частоты
		    	    lv_draw_label_dsc_t label;
		    	    lv_draw_label_dsc_init(& label);
		            label.colorgridlines = display_lvlcolor(colordigits);
		            label.align = LV_TEXT_ALIGN_CENTER;
		            label.text = buf2;
		            // позиция (текст, выходящий за границы окне отрежется библиотекой)
		            lv_area_t labelcoord;
		            lv_area_set(& labelcoord, coord->x1 + xmarker - 100, coord->y1, coord->x1 + xmarker + 100, coord->y2);
		            lv_draw_label(layer, & label, & labelcoord);
		            //colpip_string3_tbg(db, xtext, y, buf2, colordigits);
    			}
    		}
    	}
    	if (dm->xcenter != UINT16_MAX)
    	{
        	// линия центральной частоты
            lv_draw_line_dsc_t l;
            lv_draw_line_dsc_init(& l);

            l.width = 1;
            l.round_end = 0;
            l.round_start = 0;
            l.colorgridlines = display_lvlcolor(colorcmarker);

            lv_point_precise_set(& l.p1, coord->x1 + dm->xcenter, coord->y1 + 0);
            lv_point_precise_set(& l.p2, coord->x1 + dm->xcenter, coord->y1 + alldy - 1);
            lv_draw_line(layer, & l);
    		//colpip_set_vline(db, x + dm->xcenter, y, h, colorcmarker);	// center frequency marker
    	}

    }
#endif /* WITHSPECTRUMWF */

}

// custom draw widget
static void lv_sscp3dss_event(const lv_obj_class_t * class_p, lv_event_t * e) {
    LV_UNUSED(class_p);

    lv_res_t res = lv_obj_event_base(MY_CLASS_3DSS, e);	// обработчик родительского клвсса

    if (res != LV_RES_OK) return;

    lv_obj_t  * const obj = (lv_obj_t *) lv_event_get_target(e);
	const lv_event_code_t code = lv_event_get_code(e);
    LV_ASSERT_OBJ(obj, MY_CLASS_3DSS);

    if (LV_EVENT_DRAW_MAIN == code)
    {
		lv_layer_t * const layer = lv_event_get_layer(e);
		lv_sscp3dss_t * const sscp3dss = (lv_sscp3dss_t *) obj;

        lv_area_t coords;
    	lv_obj_get_content_coords(obj, &coords); // координаты внутри border
        lv_sscp3dss_draw(sscp3dss, layer, & coords);
     }
}

lv_obj_t * lv_sscp3dss_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_3DSS, parent);
    lv_obj_class_init_obj(obj);

	return obj;
}

static void lv_sscp3dss_size_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * const obj = (lv_obj_t *) lv_event_get_target(e);
	lv_sscp3dss_t * const sscp3dss = (lv_sscp3dss_t *) obj;
    //PRINTF("sscp3dss size changed: w/h=%d/%d\n", (int) lv_area_get_width(& coords), (int) lv_area_get_height(& coords));

    lv_area_t coords;
    lv_obj_get_coords(obj, & coords);	// координаты объекта
	const int_fast32_t h = lv_area_get_height(& coords);

	// Формирование градиента в требуемой высоте
    lv_obj_t * gradcanvas = lv_canvas_create(obj);
    lv_obj_add_flag(gradcanvas, LV_OBJ_FLAG_HIDDEN);

    lv_canvas_set_draw_buf(gradcanvas, & sscp3dss->gdrawb);
    lv_layer_t layer;
    lv_canvas_init_layer(gradcanvas, & layer);

	lv_area_t gradcoords;
	lv_area_set(& gradcoords, 0, 0, 0, h - 1);
	lv_draw_rect(& layer, & sscp3dss->gradrect, & gradcoords);

    lv_canvas_finish_layer(gradcanvas, & layer);	// выполняем отрисовку в sscp3dss->gdrawb
    lv_obj_delete(gradcanvas);
}

static void lv_sscp3dss_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_sscp3dss_t * const sscp3dss = (lv_sscp3dss_t *) obj;
	const lv_color_format_t cf = (lv_color_format_t) display_get_lvformat();
	const int_fast32_t h = lv_display_get_vertical_resolution(lv_display_get_default());
	const int_fast32_t w = lv_display_get_horizontal_resolution(lv_display_get_default());

	lv_obj_add_event_cb(obj, lv_sscp3dss_size_changed_event_cb, LV_EVENT_SIZE_CHANGED, NULL);

	{
		lv_style_t * const s = & sscp3dss->stdigits;

		// стиль текста оцифровки
		lv_style_init(s);
	}
	{
		// Временный буфер для рисования самого виджета
		const int_fast32_t gbufsizetmp = LV_DRAW_BUF_SIZE(w, h, cf);	// размер выделеной памяти
		sscp3dss->gbuftmp = (uint8_t *) lv_malloc(gbufsizetmp);
		LV_ASSERT_MALLOC(sscp3dss->gbuftmp);

		lv_draw_buf_init(& sscp3dss->gdrawbtmp, w, h, cf, LV_DRAW_BUF_STRIDE(w, cf), sscp3dss->gbuftmp, gbufsizetmp);
	    lv_draw_buf_set_flag(& sscp3dss->gdrawbtmp, LV_IMAGE_FLAGS_MODIFIABLE);
	}
	{
        // градиент

		// Буфер для рисования градиента
		const int_fast32_t w = 1;
		const int_fast32_t gbufsize = LV_DRAW_BUF_SIZE(w, h, cf);	// размер выделеной памяти
		sscp3dss->gbuf1pix = (uint8_t *) lv_malloc(gbufsize);
		LV_ASSERT_MALLOC(sscp3dss->gbuf1pix);

		lv_draw_buf_init(& sscp3dss->gdrawb, w, h, cf, LV_DRAW_BUF_STRIDE(w, cf), sscp3dss->gbuf1pix, gbufsize);
	    lv_draw_buf_set_flag(& sscp3dss->gdrawb, LV_IMAGE_FLAGS_MODIFIABLE);

	    lv_draw_rect_dsc_init(& sscp3dss->grect_dsc);
	    sscp3dss->grect_dsc.bg_image_src = & sscp3dss->gdrawb;


        lv_draw_rect_dsc_init(& sscp3dss->gradrect);

        // Update LV_GRADIENT_MAX_STOPS in lv_conf.h
		static const lv_color_t grad_colors [] =
		{
			LV_COLOR_MAKE(0x00, 0x00, 0x7f),	// цвет самого слабого сигнала
			LV_COLOR_MAKE(0x00, 0xff, 0x00),
			LV_COLOR_MAKE(0xff, 0x00, 0x00),	// цвет самого сильного сигнала
		};

		lv_grad_init_stops(& sscp3dss->gradrect.bg_grad, grad_colors, NULL, NULL, ARRAY_SIZE(grad_colors));
		lv_grad_vertical_init(& sscp3dss->gradrect.bg_grad);
	}
	{
		lv_style_t * const s = & sscp3dss->stlines;

		// стиль линий
		lv_style_init(s);
	}


//#if WITHLVGL && WITHSPECTRUMWF
//	lv_image_set_src(obj, wfl_get_draw_buff());	// src_type=LV_IMAGE_SRC_VARIABLE
//#endif /* WITHLVGL && WITHSPECTRUMWF */

	LV_TRACE_OBJ_CREATE("finished");
}

static void lv_sscp3dss_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    lv_sscp3dss_t * const sscp3dss = (lv_sscp3dss_t *) obj;

	lv_free(sscp3dss->gbuf1pix);
	lv_free(sscp3dss->gbuftmp);

}

#endif /* WITHLVGL */

#if WITHLVGL

#include "lvgl.h"
#include "core/lv_obj_private.h"
#include "core/lv_obj_class_private.h"
#include "widgets/label/lv_label_private.h"
#include "widgets/image/lv_image_private.h"
#include "misc/lv_area_private.h"

//#include "styles.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS_SMTR2 (& lv_smtr2_class)	// собственный renderer
#define MY_CLASS_WTRF2 (& lv_wtrf2_class)	// собственный renderer
#define MY_CLASS_WTRF (& lv_wtrf_class)		// использует старый renderer
#define MY_CLASS_INFO (& lv_info_class)		// собственный renderer - нформационная панель
#define MY_CLASS_COMPAT (& lv_compat_class)	// старый renderer без модификаций

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_smtr2_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
//static void lv_smtr2_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_smtr2_event(const lv_obj_class_t * class_p, lv_event_t * e);

static void lv_info_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
//static void lv_info_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_info_event(const lv_obj_class_t * class_p, lv_event_t * e);

static void lv_wtrf_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
//static void lv_wtrf_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_wtrf_event(const lv_obj_class_t * class_p, lv_event_t * e);

static void lv_wtrf2_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
//static void lv_wtrf2_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_wtrf2_event(const lv_obj_class_t * class_p, lv_event_t * e);

static void lv_compat_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
//static void lv_compat_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_compat_event(const lv_obj_class_t * class_p, lv_event_t * e);


static const lv_obj_class_t lv_smtr2_class  = {
    .base_class = & lv_obj_class,
    .constructor_cb = lv_smtr2_constructor,
//    .destructor_cb = lv_smtr2_destructor,
    .event_cb = lv_smtr2_event,
    .name = "hmr_smtr2",
//    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
    .instance_size = sizeof (lv_smtr2_t),
};

static const lv_obj_class_t lv_info_class  = {
    .base_class = & lv_label_class,
    .constructor_cb = lv_info_constructor,
//    .destructor_cb = lv_info_destructor,
    .event_cb = lv_info_event,
    .name = "hmr_nfo",
    .instance_size = sizeof (lv_info_t),
};

static const lv_obj_class_t lv_wtrf2_class  = {
    .base_class = & lv_obj_class,
    .constructor_cb = lv_wtrf2_constructor,
//    .destructor_cb = lv_wtrf2_destructor,
    .event_cb = lv_wtrf2_event,
    .name = "hmr_wtrf2",
    .instance_size = sizeof (lv_wtrf2_t),
};

static const lv_obj_class_t lv_compat_class  = {
    .base_class = & lv_obj_class,
    .constructor_cb = lv_compat_constructor,
//    .destructor_cb = lv_compat_destructor,
    .event_cb = lv_compat_event,
    .name = "hmr_compat",
    .instance_size = sizeof (lv_compat_t),
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_smtr2_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_SMTR2, parent);
    lv_obj_class_init_obj(obj);

	return obj;
}


lv_obj_t * lv_wtrf2_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_WTRF2, parent);
    lv_obj_class_init_obj(obj);

	return obj;
}

lv_obj_t * lv_info_create(lv_obj_t * parent, int (* infocb)(char * b, size_t len, int * selector))
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_INFO, parent);
    lv_obj_class_init_obj(obj);

    lv_info_t * const cp = (lv_info_t *) obj;
    cp->infotext [0] = '\0';
    cp->infocb = infocb;
    return obj;
}

lv_obj_t * lv_compat_create(lv_obj_t * parent, const void * dzp)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_COMPAT, parent);
    lv_obj_class_init_obj(obj);

    lv_compat_t * const cp = (lv_compat_t *) obj;

    cp->dzpv = dzp;

	return obj;

}

/*=====================
 * Setter functions
 *====================*/


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_info_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_info_t * const cp = (lv_info_t *) obj;

    lv_snprintf(cp->infotext, ARRAY_SIZE(cp->infotext), "%s", "..");
    lv_label_set_text_static(obj, cp->infotext);
    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_wtrf2_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_wtrf2_t * const cp = (lv_wtrf2_t *) obj;

	{
		lv_style_t * const s = & cp->stdigits;

		// стиль текста оцифровки
		lv_style_init(s);
	}

	{
		lv_style_t * const s = & cp->stlines;

		// стиль линий
		lv_style_init(s);
	}


//#if WITHLVGL && WITHSPECTRUMWF
//	lv_image_set_src(obj, wfl_get_draw_buff());	// src_type=LV_IMAGE_SRC_VARIABLE
//#endif /* WITHLVGL && WITHSPECTRUMWF */

	LV_TRACE_OBJ_CREATE("finished");
}

//////////////////////

static void lv_compat_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_compat_t * const cp = (lv_compat_t *) obj;

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_smtr2_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_smtr2_t * const cp = (lv_smtr2_t *) obj;

    LV_TRACE_OBJ_CREATE("finished");
}

//static void lv_smtr2_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
//{
//    LV_UNUSED(class_p);
//    lv_smtr2_t * smtr = (lv_smtr2_t *)obj;
//
//}


static void lv_info_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);
    lv_obj_t  * const obj = (lv_obj_t *) lv_event_get_target(e);
	const lv_event_code_t code = lv_event_get_code(e);
    LV_ASSERT_OBJ(obj, MY_CLASS_INFO);

    // текст обновляем перед отрисовкой
    if (LV_EVENT_DRAW_MAIN_BEGIN == code)
    {
    	lv_info_t   * const cp = (lv_info_t *) obj;
    	int state = 0;	// вариант стиля отображения если надо менять в зависимости от ситуации
    	(* cp->infocb)(cp->infotext, ARRAY_SIZE(cp->infotext), & state);
    }

    lv_res_t res = lv_obj_event_base(MY_CLASS_INFO, e);	// обработчик родительского клвсса
    if (res != LV_RES_OK) return;
}

// Исправлено масштабирование к крайнему большему значению
static
int_fast32_t normalize31(
	int_fast32_t raw,
	int_fast32_t rawmin,
	int_fast32_t rawmid,
	int_fast32_t rawmax,
	int_fast32_t range1,
	int_fast32_t range2
	)
{
	if (raw < rawmid)
		return normalize(raw, rawmin, rawmid - 1, range1 - 1);
	else
		return normalize(raw - rawmid, 0, rawmax - rawmid - 1, range2 - range1 - 1) + range1;
}

static void smtr2_draw(lv_smtr2_t * smtr2, const lv_area_t * coords, lv_layer_t * layer)
{
	const int_fast32_t h = lv_area_get_height(coords);
	const int_fast32_t w = lv_area_get_width(coords);
	// Прямоугольник для рисования полосы s-meter
	lv_area_t smeterbar;
	lv_area_set(&smeterbar, coords->x1, coords->y1 + h / 3, coords->x2, coords->y1 + h * 2 / 3);
	int_fast32_t gs = 0;
	int_fast32_t gm = lv_area_get_width(&smeterbar) / 2;
	int_fast32_t ge = lv_area_get_width(&smeterbar) - 1;
	const adcvalholder_t power = board_getadc_unfiltered_truevalue(PWRMRRIX);
	// без возможных тормозов на SPI при чтении
	int_fast16_t tracemaxi10;
	int_fast16_t rssi10 = dsp_rssi10(& tracemaxi10, 0);	/* получить значение уровня сигнала для s-метра в 0.1 дБмВт */
	int_fast32_t gv_pos = gs + normalize31(smtrvalue, s9level - s9delta, s9level, s9level + s9_60_delta, gm - gs, ge - gs);
	int_fast32_t gv_trace = gs + normalize31(tracemax, s9level - s9delta, s9level, s9level + s9_60_delta, gm - gs, ge - gs);
	if (1)
	{
		lv_draw_rect_dsc_t rect;
		lv_draw_rect_dsc_init(&rect);
		// фон
		rect.bg_color = lv_palette_main(LV_PALETTE_DEEP_PURPLE);
		rect.bg_image_opa = LV_OPA_COVER;
		lv_draw_rect(layer, & rect, coords);

		if (gv_pos > 0)
		{
			lv_area_t cmeter;
			lv_area_set(&cmeter, smeterbar.x1 + 0, smeterbar.y1 + 0, smeterbar.x1 + gv_pos, smeterbar.y2 + 0);
			rect.bg_color = lv_palette_main(LV_PALETTE_RED);
			lv_draw_rect(layer, &rect, &cmeter);
		}
		if (gv_trace > gv_pos)
		{
			lv_area_t cplus;
			lv_area_set(&cplus, smeterbar.x1 + gv_pos, smeterbar.y1 + 0, smeterbar.x1 + gv_trace, smeterbar.y2 + 0);
			rect.bg_color = lv_palette_main(LV_PALETTE_YELLOW);
			lv_draw_rect(layer, &rect, &cplus);
			// Заполнение до правого края
			lv_area_t cright;
			lv_area_set(&cright, smeterbar.x1 + gv_trace, smeterbar.y1 + 0, smeterbar.x2, smeterbar.y2 + 0);
			rect.bg_color = lv_palette_main(LV_PALETTE_BLUE_GREY);
			lv_draw_rect(layer, &rect, &cright);
		}
		else
		{
			// Заполнение до правого края
			lv_area_t cright;
			lv_area_set(&cright, smeterbar.x1 + gv_pos, smeterbar.y1 + 0, smeterbar.x2, smeterbar.y2 + 0);
			rect.bg_color = lv_palette_main(LV_PALETTE_BLUE_GREY);
			lv_draw_rect(layer, &rect, &cright);
		}
	}
	if (0)
	{
		// lines test
		lv_draw_line_dsc_t dsc;
		lv_draw_line_dsc_init(&dsc);
		dsc.width = 1;
		dsc.round_end = 0;
		dsc.round_start = 0;
		// диагональ
		dsc.color = lv_palette_main(LV_PALETTE_YELLOW);
		lv_point_precise_set(&dsc.p1, coords->x1, coords->y1);
		lv_point_precise_set(&dsc.p2, coords->x2, coords->y2);
		lv_draw_line(layer, &dsc);
		dsc.color = lv_palette_main(LV_PALETTE_RED);
		lv_point_precise_set(&dsc.p1, coords->x1 + gv_pos, coords->y1);
		lv_point_precise_set(&dsc.p2, coords->x1 + gv_pos, coords->y2);
		lv_draw_line(layer, &dsc);
		dsc.color = lv_palette_main(LV_PALETTE_LIGHT_GREEN);
		lv_point_precise_set(&dsc.p1, coords->x1 + gv_trace, coords->y1);
		lv_point_precise_set(&dsc.p2, coords->x1 + gv_trace, coords->y2);
		lv_draw_line(layer, &dsc);
	}
}

// custom draw widget
static void lv_smtr2_event(const lv_obj_class_t * class_p, lv_event_t * e) {
    LV_UNUSED(class_p);

    lv_res_t res = lv_obj_event_base(MY_CLASS_SMTR2, e);	// обработчик родительского клвсса

    if (res != LV_RES_OK) return;

    lv_obj_t  * const obj = (lv_obj_t *) lv_event_get_target(e);
	const lv_event_code_t code = lv_event_get_code(e);
    LV_ASSERT_OBJ(obj, MY_CLASS_SMTR2);

    if (LV_EVENT_ROTARY == code)
    {
    	if (lv_event_get_rotary_diff(e))
    	{
    		PRINTF("lv_smtr2_event rotary, diff=%d, obj=%p\n", (int) lv_event_get_rotary_diff(e), lv_event_get_target(e));
    	}
    }
    if (LV_EVENT_DRAW_MAIN_END == code)	// рисуем после отрисовки родителбского класса
    {
    	lv_smtr2_t * const smtr2 = (lv_smtr2_t*) obj;
    	lv_layer_t * const layer = lv_event_get_layer(e);

    	lv_area_t coords;
    	lv_obj_get_content_coords(obj, &coords); // координаты внутри border
		smtr2_draw(smtr2, & coords, layer);
    }
}

// custom draw widget
static void lv_compat_event(const lv_obj_class_t * class_p, lv_event_t * e) {
    LV_UNUSED(class_p);

    lv_res_t res = lv_obj_event_base(MY_CLASS_COMPAT, e);	// обработчик родительского клвсса

    if (res != LV_RES_OK) return;

    lv_obj_t  * const obj = (lv_obj_t *) lv_event_get_target(e);
	const lv_event_code_t code = lv_event_get_code(e);
    LV_ASSERT_OBJ(obj, MY_CLASS_COMPAT);

    if (LV_EVENT_DRAW_MAIN == code)
    {
		lv_layer_t * const layer = lv_event_get_layer(e);
		lv_compat_t * const cp = (lv_compat_t *) obj;

        lv_area_t coords;
        lv_obj_get_coords(obj, & coords);	// координаты объекта

        dzi_compat_draw_callback(layer, cp->dzpv, NULL);
     }
}

// custom draw widget
static void lv_wtrf2_event(const lv_obj_class_t * class_p, lv_event_t * e) {
    LV_UNUSED(class_p);

    lv_res_t res = lv_obj_event_base(MY_CLASS_WTRF2, e);	// обработчик родительского клвсса

    if (res != LV_RES_OK) return;

    lv_obj_t  * const obj = (lv_obj_t *) lv_event_get_target(e);
	const lv_event_code_t code = lv_event_get_code(e);
    LV_ASSERT_OBJ(obj, MY_CLASS_WTRF2);

    if (LV_EVENT_DRAW_MAIN == code)
    {
		lv_layer_t * const layer = lv_event_get_layer(e);
		lv_wtrf2_t * const wtrf2 = (lv_wtrf2_t *) obj;

        lv_area_t coords;
    	lv_obj_get_content_coords(obj, &coords); // координаты внутри border
        lv_wtrf2_draw(layer, & coords);
     }
}



#endif /* WITHLVGL */

#if WITHLVGL

#define MY_CLASS_SSCP2 (& lv_sscp2_class)	// собственный renderer

static void lv_sscp2_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_sscp2_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_sscp2_event(const lv_obj_class_t * class_p, lv_event_t * e);

static const lv_obj_class_t lv_sscp2_class  =
{
    .base_class = & lv_obj_class,
    .constructor_cb = lv_sscp2_constructor,
    .destructor_cb = lv_sscp2_destructor,
    .event_cb = lv_sscp2_event,
    .name = "hmr_sscp2",
    .instance_size = sizeof (lv_sscp2_t),
};

lv_obj_t * lv_sscp2_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_SSCP2, parent);
    lv_obj_class_init_obj(obj);

	return obj;
}

static void lv_sscp2_size_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * const obj = (lv_obj_t *) lv_event_get_target(e);
	lv_sscp2_t * const sscp2 = (lv_sscp2_t *) obj;
    //PRINTF("sscp2 size changed: w/h=%d/%d\n", (int) lv_area_get_width(& coords), (int) lv_area_get_height(& coords));

    lv_area_t coords;
    lv_obj_get_coords(obj, & coords);	// координаты объекта
	const int_fast32_t h = lv_area_get_height(& coords);

	// Формирование градиента в требуемой высоте
    lv_obj_t * gradcanvas = lv_canvas_create(obj);
    lv_obj_add_flag(gradcanvas, LV_OBJ_FLAG_HIDDEN);

    lv_canvas_set_draw_buf(gradcanvas, & sscp2->gdrawb);
    lv_layer_t layer;
    lv_canvas_init_layer(gradcanvas, & layer);

	lv_area_t gradcoords;
	lv_area_set(& gradcoords, 0, 0, 0, h - 1);
	lv_draw_rect(& layer, & sscp2->gradrect, & gradcoords);

    lv_canvas_finish_layer(gradcanvas, & layer);	// выполняем отрисовку в sscp2->gdrawb
    lv_obj_delete(gradcanvas);
}

static void lv_sscp2_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_sscp2_t * const sscp2 = (lv_sscp2_t *) obj;
	const lv_color_format_t cf = (lv_color_format_t) display_get_lvformat();
	const int_fast32_t h = lv_display_get_vertical_resolution(lv_display_get_default());
	const int_fast32_t w = lv_display_get_horizontal_resolution(lv_display_get_default());

	lv_obj_add_event_cb(obj, lv_sscp2_size_changed_event_cb, LV_EVENT_SIZE_CHANGED, NULL);

	{
		lv_style_t * const s = & sscp2->stdigits;

		// стиль текста оцифровки
		lv_style_init(s);
	}
	{
		// Временный буфер для рисования самого виджета
		const int_fast32_t gbufsizetmp = LV_DRAW_BUF_SIZE(w, h, cf);	// размер выделеной памяти
		sscp2->gbuftmp = (uint8_t *) lv_malloc(gbufsizetmp);
		LV_ASSERT_MALLOC(sscp2->gbuftmp);

		lv_draw_buf_init(& sscp2->gdrawbtmp, w, h, cf, LV_DRAW_BUF_STRIDE(w, cf), sscp2->gbuftmp, gbufsizetmp);
	    lv_draw_buf_set_flag(& sscp2->gdrawbtmp, LV_IMAGE_FLAGS_MODIFIABLE);
	}
	{
        // градиент

		// Буфер для рисования градиента
		const int_fast32_t w = 1;
		const int_fast32_t gbufsize = LV_DRAW_BUF_SIZE(w, h, cf);	// размер выделеной памяти
		sscp2->gbuf1pix = (uint8_t *) lv_malloc(gbufsize);
		LV_ASSERT_MALLOC(sscp2->gbuf1pix);

		lv_draw_buf_init(& sscp2->gdrawb, w, h, cf, LV_DRAW_BUF_STRIDE(w, cf), sscp2->gbuf1pix, gbufsize);
	    lv_draw_buf_set_flag(& sscp2->gdrawb, LV_IMAGE_FLAGS_MODIFIABLE);

	    lv_draw_rect_dsc_init(& sscp2->grect_dsc);
	    sscp2->grect_dsc.bg_image_src = & sscp2->gdrawb;


        lv_draw_rect_dsc_init(& sscp2->gradrect);

        // Update LV_GRADIENT_MAX_STOPS in lv_conf.h
		static const lv_color_t grad_colors [] =
		{
			LV_COLOR_MAKE(0x00, 0x00, 0x7f),	// цвет самого слабого сигнала
			LV_COLOR_MAKE(0x00, 0xff, 0x00),
			LV_COLOR_MAKE(0xff, 0x00, 0x00),	// цвет самого сильного сигнала
		};

		lv_grad_init_stops(& sscp2->gradrect.bg_grad, grad_colors, NULL, NULL, ARRAY_SIZE(grad_colors));
		lv_grad_vertical_init(& sscp2->gradrect.bg_grad);
	}
	{
		lv_style_t * const s = & sscp2->stlines;

		// стиль линий
		lv_style_init(s);
	}


//#if WITHLVGL && WITHSPECTRUMWF
//	lv_image_set_src(obj, wfl_get_draw_buff());	// src_type=LV_IMAGE_SRC_VARIABLE
//#endif /* WITHLVGL && WITHSPECTRUMWF */

	LV_TRACE_OBJ_CREATE("finished");
}

static void lv_sscp2_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    lv_sscp2_t * const sscp2 = (lv_sscp2_t *) obj;

	lv_free(sscp2->gbuf1pix);
	lv_free(sscp2->gbuftmp);

}

// Рисуем спектр примитивами LVGL
void lv_sscp2_draw(lv_sscp2_t * const sscp2, lv_layer_t * layer, const lv_area_t * coord)
{
    //PRINTF("lv_sscp2_draw: w/h=%d/%d, x/y=%d/%d\n", (int) lv_area_get_width(coords), (int) lv_area_get_height(coords), (int) coords->x1, (int) coords->y1);
	const int32_t alldx = lv_area_get_width(coord);
	const int32_t alldy = lv_area_get_height(coord);
	//lv_draw_buf_t * db = (lv_draw_buf_t *) lv_draw_layer_alloc_buf(layer);
#if WITHSPECTRUMWF
	const struct dispmap * const dm = & latched_dm;
#endif /* WITHSPECTRUMWF */

	// сохранияем дянные для отображения
	int vals [alldx];
	int peaks [alldx];
    int32_t x;
	for (x = 0; x < alldx; ++ x)
	{
#if WITHSPECTRUMWF
		vals [x] = dsp_mag2y(scbf.filtered_spectrum(x, alldx), alldy - 1, glob_topdb, glob_bottomdb);
		peaks [x] = dsp_mag2y(scbf.peaks_spectrum(x, alldx), alldy - 1, glob_topdb, glob_bottomdb);
#else
		vals [x] = (x * (alldy - 1) / (alldx - 1));	// debug
		peaks [x] = (x * (alldy - 1) / (alldx - 1));	// debug
#endif /* WITHSPECTRUMWF */
	}

	// Формирование изображения.
    if (1)
    {
    	// закрасить фон
        lv_draw_rect_dsc_t rect;
        lv_draw_rect_dsc_init(& rect);
        rect.bg_color = display_lvlcolor(DSGN_SPECTRUMBG); //lv_palette_main(LV_PALETTE_GREY);
        rect.bg_opa = LV_OPA_COVER;
    	lv_draw_rect(layer, & rect, coord);
    }

#if WITHSPECTRUMWF
    if (1)
    {
		// Изображение "шторки" на спектре.
		uint_fast8_t splitflag;
		uint_fast8_t pathi;
		hamradio_get_vfomode3_value(& splitflag);
		for (pathi = 0; pathi < (splitflag ? 2 : 1); ++ pathi)
		{
			const lv_color_t lvrxbwcolor = display_lvlcolor(pathi ? DSGN_SPECTRUMBG2RX2 : DSGN_SPECTRUMBG2);
			int32_t xleft = dm->xleft [pathi];		// левый край шторки
			int32_t xright = dm->xright [pathi];	// правый край шторки
			if (xleft == UINT16_MAX || xright == UINT16_MAX)
				continue;
			// рисуем от xleft до xright включительно
			if (xleft > xright)
				xleft = 0;
			if (xright >= alldx)
				xright = alldx - 1;

			// Изображение "шторки" под спектром.
	    	lv_area_t bwcoords;
	        lv_draw_rect_dsc_t rect;
	        lv_draw_rect_dsc_init(& rect);
	        lv_area_set(& bwcoords, xleft, 0, xright, alldy - 1);
	        lv_area_move(& bwcoords, coord->x1, coord->y1);

	        rect.bg_color = lvrxbwcolor; //display_lvlcolor(rxbwcolor); //lv_palette_main(LV_PALETTE_GREEN);
	        rect.bg_opa = glob_rxbwsatu * (LV_OPA_COVER - LV_OPA_TRANSP) / 100 + LV_OPA_TRANSP;
	    	lv_draw_rect(layer, & rect, & bwcoords);
		}
    }
#endif /* WITHSPECTRUMWF */

    if (1)
    {
    	// пиковые значения спектра
    	const PACKEDCOLORPIP_T colordots = DSGN_SPECTRUMPEAKS;
        int32_t x;
    	for (x = 0; x < alldx - 1; ++ x)
    	{
    		int val;
    		for (val = peaks [x]; val > vals [x]; -- val)
    		{
    			const int32_t ydst = alldy - 1 - val;
    			void * const dst = lv_draw_layer_go_to_xy(layer, coord->x1 + x, coord->y1 + ydst);
    			lv_memcpy(dst, & colordots, LV_COLOR_FORMAT_GET_SIZE(display_get_lvformat()));
				break;	// только одна точка
    		}
    	}
   	}

    if (glob_view_style == VIEW_COLOR)
    {
    	// Градиентное заполнение спектра
        int32_t x;
    	for (x = 0; x < alldx - 1; ++ x)
    	{
    		const int val = vals [x];

    		int32_t level;		// отступ от нижней границы контрола в отображаемой части
			for (level = 0; level <= val; ++ level)
			{
				const int32_t ydst = alldy - 1 - level;
				const int32_t ysrc = level;

				void * const dst = lv_draw_layer_go_to_xy(layer, coord->x1 + x, coord->y1 + ydst);
				void * const src = lv_draw_buf_goto_xy(& sscp2->gdrawb, 0, ysrc);	// одна колонка
				lv_memcpy(dst, src, LV_COLOR_FORMAT_GET_SIZE(display_get_lvformat()));
			}
    	}
   	}

    if (glob_view_style == VIEW_DOTS)
    {
    	// Без заполнения спектра
    	const PACKEDCOLORPIP_T colordots = DSGN_SPECTRUMLINE;
        int32_t x;
    	for (x = 0; x < alldx - 1; ++ x)
    	{
    		const int val = vals [x];
			const int32_t ydst = alldy - 1 - val;

			void * const dst = lv_draw_layer_go_to_xy(layer, coord->x1 + x, coord->y1 + ydst);
			lv_memcpy(dst, & colordots, LV_COLOR_FORMAT_GET_SIZE(display_get_lvformat()));
    	}
   	}

    if (glob_view_style == VIEW_FILL)
    {
    	// Одноцветное заполнение спектра
    	const PACKEDCOLORPIP_T colorfill = DSGN_SPECTRUMLINE;
        int32_t x;
    	for (x = 0; x < alldx - 1; ++ x)
    	{
    		const int val = vals [x];
     		int32_t level;	// отступ от нижней границы контрола в отображаемой части
			for (level = 0; level <= val; ++ level)
			{
				const int32_t ydst = alldy - 1 - level;

				void * const dst = lv_draw_layer_go_to_xy(layer, coord->x1 + x, coord->y1 + ydst);
				lv_memcpy(dst, & colorfill, LV_COLOR_FORMAT_GET_SIZE(display_get_lvformat()));
			}
    	}
   	}

    if (glob_view_style != VIEW_DOTS)
    {
    	// линия спектра
        lv_draw_line_dsc_t l;
        lv_draw_line_dsc_init(& l);

        l.width = 1;
        l.round_end = 0;
        l.round_start = 0;
        l.color = lv_palette_main(LV_PALETTE_YELLOW);

        int32_t x;
    	for (x = 0; x < alldx; ++ x)
    	{
    		// ломанная
    		const int val = vals [x];
    		int32_t y = alldy - 1 - val;
	        lv_point_precise_set(& l.p2, coord->x1 + x, coord->y1 + y);
    		if (x)
    		{
    	        lv_draw_line(layer, & l);
    		}
    		l.p1 = l.p2;
    	}
    }

    if (0)
    {
    	// надписи
	    lv_draw_label_dsc_t label;
	    lv_draw_label_dsc_init(& label);
        label.color = lv_palette_main(LV_PALETTE_YELLOW);
        label.align = LV_TEXT_ALIGN_CENTER;
        label.text = "Test scope";
        lv_draw_label(layer, & label, coord);

    }
#if WITHSPECTRUMWF
    if (1)
    {
    	// сетка и надписи
    	const uint_fast32_t f0 = dm->f0;	/* frequency at middle of spectrum */
    	const int_fast32_t bw = dm->bw;

    	const uint_fast8_t markerh = 10;
    	const int_fast32_t go = f0 % (int) glob_gridstep;	// шаг сетки
    	const int_fast32_t gs = (int) glob_gridstep;	// шаг сетки
    	const int_fast32_t halfbw = bw / 2;
    	int_fast32_t df;	// кратное сетке значение

    	// Маркеры уровней сигналов
    	if (glob_lvlgridstep != 0)
    	{
    		int32_t lvl;
    		for (lvl = glob_topdb / glob_lvlgridstep * glob_lvlgridstep; lvl >= glob_bottomdb; lvl -= glob_lvlgridstep)
    		{
    			const int32_t yval = dsp_mag2y(db2ratio(lvl), alldy - 1, glob_topdb, glob_bottomdb);
    			if (yval > 0 && yval < alldy)
    			{
   		            lv_draw_line_dsc_t l;
					lv_draw_line_dsc_init(& l);

					l.width = 1;
					l.round_end = 0;
					l.round_start = 0;
					l.colorgridlines = display_lvlcolor(colorgridlines);

					lv_point_precise_set(& l.p1, coord->x1, coord->y1 + yval);
					lv_point_precise_set(& l.p2, coord->x2, coord->y1 + yval);
					lv_draw_line(layer, & l);

    				//colpip_set_hline(db, x, y + yval, w, colorgridlines);	// Level marker
    			}
    		}
    	}

    	for (df = - halfbw / gs * gs - go; df < halfbw; df += gs)
    	{
    		if (df > - halfbw)
    		{
    			// Маркер частоты кратной glob_gridstep - линия и надпись
    			const uint_fast16_t xmarker = deltafreq2x_abs(f0, df, bw, ALLDX);
    			if (xmarker != UINT16_MAX)
    			{
    				char buf2 [16];
    				uint_fast16_t freqw;	// ширина строки со значением частоты
    				local_snprintf_P(buf2, ARRAY_SIZE(buf2), gridfmt_2, glob_gridwc, (long) ((f0 + df) / glob_griddigit % glob_gridmod));

    				lv_draw_line_dsc_t l;
		            lv_draw_line_dsc_init(& l);

		            l.width = 1;
		            l.round_end = 0;
		            l.round_start = 0;
		            l.colorgridlines = display_lvlcolor(colorgridlines);

		            lv_point_precise_set(& l.p1, coord->x1 + xmarker, coord->y1);
		            lv_point_precise_set(& l.p2, coord->x1 + xmarker, coord->y2);
		            lv_draw_line(layer, & l);
					//colpip_set_vline(db, xmarker, y + markerh, alldy - markerh, colorgridlines);

		            // текст маркера частоты
		    	    lv_draw_label_dsc_t label;
		    	    lv_draw_label_dsc_init(& label);
		            label.colorgridlines = display_lvlcolor(colordigits);
		            label.align = LV_TEXT_ALIGN_CENTER;
		            label.font = &lv_font_montserrat_14;
		            label.text = buf2;
		            // позиция (текст, выходящий за границы окне отрежется библиотекой)
		            lv_area_t labelcoord;
		            lv_area_set(& labelcoord, coord->x1 + xmarker - 100, coord->y1, coord->x1 + xmarker + 100, coord->y2);
		            lv_draw_label(layer, & label, & labelcoord);
		            //colpip_string3_tbg(db, xtext, y, buf2, colordigits);
    			}
    		}
    	}
    	if (dm->xcenter != UINT16_MAX)
    	{
        	// линия центральной частоты
            lv_draw_line_dsc_t l;
            lv_draw_line_dsc_init(& l);

            l.width = 1;
            l.round_end = 0;
            l.round_start = 0;
            l.colorgridlines = display_lvlcolor(colorcmarker);

            lv_point_precise_set(& l.p1, coord->x1 + dm->xcenter, coord->y1 + 0);
            lv_point_precise_set(& l.p2, coord->x1 + dm->xcenter, coord->y1 + alldy - 1);
            lv_draw_line(layer, & l);
    		//colpip_set_vline(db, x + dm->xcenter, y, h, colorcmarker);	// center frequency marker
    	}

    }
#endif /* WITHSPECTRUMWF */
}

// custom draw widget
static void lv_sscp2_event(const lv_obj_class_t * class_p, lv_event_t * e) {
    LV_UNUSED(class_p);

    lv_res_t res = lv_obj_event_base(MY_CLASS_SSCP2, e);	// обработчик родительского клвсса

    if (res != LV_RES_OK) return;

    lv_obj_t  * const obj = (lv_obj_t *) lv_event_get_target(e);
	const lv_event_code_t code = lv_event_get_code(e);
    LV_ASSERT_OBJ(obj, MY_CLASS_SSCP2);

    if (LV_EVENT_DRAW_MAIN == code)
    {
		lv_layer_t * const layer = lv_event_get_layer(e);
		lv_sscp2_t * const sscp2 = (lv_sscp2_t *) obj;

        lv_area_t coords;
    	lv_obj_get_content_coords(obj, &coords); // координаты внутри border
        lv_sscp2_draw(sscp2, layer, & coords);
     }
}

////////////////////////////

static void wtrf2_fillinfo(lv_draw_image_dsc_t * fd, lv_draw_buf_t * dbf, lv_area_t * area, uint_fast16_t wfdx, uint_fast16_t wfdy, int phase)
{
#if WITHSPECTRUMWF
	gxdrawb_t wfjdbv;
	gxdrawb_initialize(& wfjdbv, scbf.scrollcolor.bf(), ALLDX, NROWSWFL);

	// следы спектра ("водопад") на цветных дисплеях
	/* быстрое отображение водопада (но требует больше памяти) */
	const uint_fast16_t wfrow = scbf.getwfrow();
	const uint_fast16_t p1h = ulmin16(NROWSWFL - wfrow, wfdy);	// высота верхней части в результируюшем изображении
	const uint_fast16_t p2h = ulmin16(wfrow, wfdy - p1h);		// высота нижней части в результируюшем изображении
	const uint_fast16_t p1y = 0;
	const uint_fast16_t p2y = 0 + p1h;

	lv_draw_image_dsc_init(fd);
	lv_draw_buf_init(dbf, ALLDX, NROWSWFL, (lv_color_format_t) display_get_lvformat(), wfjdbv.stride, wfjdbv.buffer, wfjdbv.cachesize);
	fd->src = dbf;

    if (phase == 0)
    {
    	ASSERT(p1h != 0);
		/* отрисовка свежей части растра */

//		0, p1y,	// координаты получателя
    	lv_area_set(area, 0, p1y, wfdx - 1, p1y + p1h - 1);	// куда

    	//		0, wfrow,	// координаты окна источника
    	//		wfdx, p1h, 	// размеры окна источника
    	lv_area_set(& fd->image_area, 0, wfrow, wfdx - 1, wfrow + p1h - 1);	// откуда
    	dbf->data = (uint8_t *) lv_draw_buf_goto_xy(dbf, 0, wfrow);	// хак - надо исправлять
    }
    else
    {
        if (p2h != 0)
        {
    		/* отрисовка старой части растра */
    //		0, p2y,		// координаты получателя
        	lv_area_set(area, 0, p2y, wfdx - 1, p2y + p2h - 1);		// куда

    //		0, 0,	// координаты окна источника
    //		wfdx, p2h, 	// размеры окна источника
        	lv_area_set(& fd->image_area, 0, 0, wfdx - 1, p2h - 1);	// откуда
        	dbf->data = (uint8_t *) lv_draw_buf_goto_xy(dbf, 0, 0);	// хак - надо исправлять
        }
        else
        {
        	// особый случай - всё "новое".
        	lv_area_set(area, 0, 0, 0, 0);				// куда
        	lv_area_set(& fd->image_area, 0, 0, 0, 0);	// откуда
        }
    }
#endif /* WITHSPECTRUMWF */
}

// Рисуем водопад примитивами LVGL
void lv_wtrf2_draw(lv_layer_t * layer, const lv_area_t * coords)
{
    //PRINTF("lv_wtrf2_draw: alldx/alldy=%d/%d, x/y=%d/%d\n", (int) lv_area_get_width(coords), (int) lv_area_get_height(coords), (int) coords->x1, (int) coords->y1);
	const int32_t alldx = lv_area_get_width(coords);
	const int32_t alldy = lv_area_get_height(coords);
	const uint8_t pathi = 0;
    lv_area_t a1;
    lv_area_t a2;
#if WITHSPECTRUMWF
	const struct dispmap * const dm = & latched_dm;
#endif /* WITHSPECTRUMWF */

	const int32_t lbw = alldx / 2 + 15;
	const int32_t rbw = alldx / 2 + 45;

    if (0)
    {
    	// отладка. закрасить зону отображения
        lv_draw_rect_dsc_t rect;
        lv_draw_rect_dsc_init(& rect);
        rect.bg_color = lv_palette_main(LV_PALETTE_RED);
        rect.bg_opa = LV_OPA_COVER;
    	lv_draw_rect(layer, & rect, coords);
    }

#if WITHSPECTRUMWF
    if (1)
    {
    	// водопад
        lv_draw_buf_t b1;
        lv_draw_buf_t b2;

        lv_draw_image_dsc_t fd1;
        lv_draw_image_dsc_t fd2;

        wtrf2_fillinfo(& fd1, & b1, & a1, alldx, alldy, 0);
        wtrf2_fillinfo(& fd2, & b2, & a2, alldx, alldy, 1);

        lv_area_move(& a1, coords->x1, coords->y1);
        lv_area_move(& a2, coords->x1, coords->y1);

        lv_draw_image(layer, & fd1, & a1);
        lv_draw_image(layer, & fd2, & a2);
    }
#endif /* WITHSPECTRUMWF */

#if ! WITHSPECTRUMWF
    if (1)
    {
    	// надписи
	    lv_draw_label_dsc_t label;
	    lv_draw_label_dsc_init(& label);
        label.color = lv_palette_main(LV_PALETTE_YELLOW);
        label.align = LV_TEXT_ALIGN_CENTER;
        label.text = "Test waterfall";
        lv_draw_label(layer, & label, coords);
    }
#endif /* ! WITHSPECTRUMWF */

#if WITHSPECTRUMWF
    if (hamradio_get_bringtuneA())
    {
    	// отладка. закрасить зону полосы пропускания
        lv_draw_rect_dsc_t rect;
        lv_draw_rect_dsc_init(& rect);
		uint_fast8_t splitflag;
		uint_fast8_t pathi;
		hamradio_get_vfomode3_value(& splitflag);
		for (pathi = 0; pathi < (splitflag ? 2 : 1); ++ pathi)
		{
			int_fast32_t xleft = dm->xleft [pathi];		// левый край шторки
			int_fast32_t xright = dm->xright [pathi];	// правый край шторки

			if (xleft != UINT16_MAX && xright != UINT16_MAX)
			{
		    	lv_area_t bwcoords;

				if (xleft > xright)
					xleft = 0;
				if (xright == xleft)
					xright = xleft + 1;
				if (xright >= alldx)
					xright = alldx - 1;

				/* Отрисовка прямоугольникв ("шторки") полосы пропускания на водопаде. */
		        bwcoords.y1 = coords->y1;
		        bwcoords.y2 = coords->y2;
		        bwcoords.x1 = coords->x1 + xleft;
		        bwcoords.x2 = coords->x1 + xright;
		        rect.bg_color = display_lvlcolor(pathi ? DSGN_SPECTRUMBG2RX2 : DSGN_SPECTRUMBG2);
		        rect.bg_opa = glob_rxbwsatu * (LV_OPA_COVER - LV_OPA_TRANSP) / 100 + LV_OPA_TRANSP;
		    	lv_draw_rect(layer, & rect, & bwcoords);
			}
		}
    }
#endif /* WITHSPECTRUMWF */

#if WITHSPECTRUMWF
    if (0)
    {
    	// линии
        lv_draw_line_dsc_t l;
        lv_draw_line_dsc_init(& l);

        l.width = 1;
        l.round_end = 0;
        l.round_start = 0;
        l.color = lv_palette_main(LV_PALETTE_YELLOW);

        // Центральная частота
        lv_point_precise_set(& l.p1, coords->x1 + dm->xcenter, coords->y1);
        lv_point_precise_set(& l.p2, coords->x1 + dm->xcenter, coords->y2);
        lv_draw_line(layer, & l);
    }
#endif /* WITHSPECTRUMWF */
}


#endif /* WITHLVGL */

///////////////////////////////
///



#if WITHRENDERHTML

#include <string.h>
#include <stdexcept>

#include "display/display.h"
#include "display/fontmaps.h"
#include "display2.h"

#include "litehtml.h"
#include <litehtml/encodings.h>
#include <litehtml/el_text.h>
#include <litehtml/render_item.h>


namespace litehtml
{

class hftrxgd: public litehtml::document_container
{
	// call back interface to draw text, images and other elements
public:
	litehtml::uint_ptr create_font(const font_description& descr, const document* doc, litehtml::font_metrics* fm);
	void delete_font(litehtml::uint_ptr hFont);
	int text_width(const char *text, litehtml::uint_ptr hFont);
	void draw_text(litehtml::uint_ptr hdc, const char *text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position &pos);
	int pt_to_px(int pt) const;
	int get_default_font_size() const;
	const char* get_default_font_name() const;
	void draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker &marker);
	void load_image(const char *src, const char *baseurl, bool redraw_on_ready);
	void get_image_size(const char *src, const char *baseurl, litehtml::size &sz);
	void draw_image(litehtml::uint_ptr hdc, const background_layer &layer, const std::string &url, const std::string &base_url);
	void draw_solid_fill(litehtml::uint_ptr hdc, const background_layer &layer, const web_color &color);
	void draw_linear_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::linear_gradient &gradient);
	void draw_radial_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::radial_gradient &gradient);
	void draw_conic_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::conic_gradient &gradient);
	void draw_borders(litehtml::uint_ptr hdc, const litehtml::borders &borders, const litehtml::position &draw_pos, bool root);

	void set_caption(const char *caption);
	void set_base_url(const char *base_url);
	void link(const std::shared_ptr<litehtml::document> &doc, const litehtml::element::ptr &el);
	void on_anchor_click(const char *url, const litehtml::element::ptr &el);
	void on_mouse_event(const litehtml::element::ptr &el, litehtml::mouse_event event);
	void set_cursor(const char *cursor);
	void transform_text(litehtml::string &text, litehtml::text_transform tt);
	void import_css(litehtml::string &text, const litehtml::string &url, litehtml::string &baseurl);
	void set_clip(const litehtml::position &pos, const litehtml::border_radiuses &bdr_radius);
	void del_clip();
	void get_client_rect(litehtml::position &client) const;
	void get_viewport(litehtml::position& viewport) const;
	litehtml::element::ptr create_element(const char *tag_name, const litehtml::string_map &attributes, const std::shared_ptr<litehtml::document> &doc);

	void get_media_features(litehtml::media_features &media) const;
	void get_language(litehtml::string &language, litehtml::string &culture) const;
//	litehtml::string resolve_color(const litehtml::string& /*color*/) const
//	{
//		return litehtml::string();
//	}
//	void split_text(const char *text, const std::function<void(const char*)> &on_word, const std::function<void(const char*)> &on_space);
	hftrxgd(int dx, int dy) :
			m_dx(dx), m_dy(dy)
	{
	}
	virtual ~hftrxgd() = default;

private:
	int m_dx;
	int m_dy;

};

static COLORPIP_T getCOLORPIP(const litehtml::web_color &color)
{
	return TFTALPHA(color.alpha, TFTRGB(color.red, color.green, color.blue));
}

litehtml::uint_ptr hftrxgd::create_font(const font_description& descr, const document* doc, litehtml::font_metrics* fm)
{
	//PRINTF("create_font: faceName='%s', size=%d\n", faceName, size);
	if (fm)
	{
		fm->font_size = 12; //?????size;
		fm->ascent = 0; //PANGO_PIXELS((double)pango_font_metrics_get_ascent(metrics));
		fm->descent = 0; //PANGO_PIXELS((double)pango_font_metrics_get_descent(metrics));
		fm->height = SMALLCHARH; //PANGO_PIXELS((double)pango_font_metrics_get_height(metrics));
		fm->x_height = fm->height;
		fm->draw_spaces = true;
	}
	return 1;
}

void hftrxgd::delete_font(litehtml::uint_ptr hFont)
{
	(void) hFont;
}

int hftrxgd::text_width(const char *text, litehtml::uint_ptr hFont)
{
	//PRINTF("text_width: text='%s'\n", text);
	(void) hFont;
	return SMALLCHARW * strlen(text);
}

void hftrxgd::draw_text(litehtml::uint_ptr hdc, const char *text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position &pos)
{
	const gxdrawb_t * const db = (const gxdrawb_t *) hdc;
	//PRINTF("draw_text: text='%s'\n", text);

	//colpip_fillrect(db, pos.left(), pos.top(), pos.width, pos.height, getCOLORPIP(color));
	colpip_string_tbg(db, pos.left(), pos.top(), text, getCOLORPIP(color));
}

int hftrxgd::pt_to_px(int pt) const
{
	return pt;
}

int hftrxgd::get_default_font_size() const
{
	return 12;
}

const char* hftrxgd::get_default_font_name() const
{
	return "Times New Roman";
}

void hftrxgd::draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker &marker)
{
	const gxdrawb_t * const db = (const gxdrawb_t *) hdc;
	TP();

	colpip_fillrect(db, marker.pos.left(), marker.pos.top(), marker.pos.width, marker.pos.height, getCOLORPIP(marker.color));

//	int top_margin = marker.pos.height / 3;
//	if (top_margin < 4)
//		top_margin = 0;
//
//	int draw_x = marker.pos.left();
//	int draw_y = marker.pos.top() + top_margin;
//	int draw_width = marker.pos.height - top_margin * 2;
//	int draw_height = marker.pos.height - top_margin * 2;
//
//	colpip_fillrect(buffer, m_dx, m_dy, draw_x, draw_y, draw_width, draw_height, getCOLORPIP(marker.color));
}

void hftrxgd::load_image(const char *src, const char *baseurl, bool redraw_on_ready)
{
	PRINTF("load_image: src='%s', baseurl='%s'\n", src, baseurl);
}

void hftrxgd::get_image_size(const char *src, const char *baseurl, litehtml::size &sz)
{
	//TP();
	litehtml::size sss(5, 10);
	sz = sss;
}

void hftrxgd::draw_image(litehtml::uint_ptr hdc, const background_layer &layer, const std::string &url, const std::string &base_url)
{
	const uint_fast8_t pathi = 0;	// тракт, испольуемый для показа s-метра
	const gxdrawb_t * const db = (const gxdrawb_t *) hdc;
	//PRINTF("draw_image: url='%s', base_url='%s'\n", url.c_str(), base_url.c_str());
	if (0)
	{

	}
	else if (! strcmp(url.c_str(), dzi_smtr2.id))
	{
		pix_display2_smeter15(db, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, pathi);
	}
	else if (! strcmp(url.c_str(), dzi_freqa.id))
	{
		uint_fast8_t rj;
		uint_fast8_t fullwidth = display_getfreqformat(& rj);
		const uint_fast8_t comma = 3 - rj;


		enum { blinkpos = UINT8_MAX, blinkstate = 0 };

		const uint_fast32_t freq = hamradio_get_freq_pathi(pathi);

#if WITHPRERENDER
		pix_rendered_value_big(db, layer.border_box.left(), layer.border_box.top(), freq, fullwidth, comma, comma + 3, rj, blinkpos, blinkstate, 1);	// отрисовываем верхнюю часть строки
#else /* WITHPRERENDER */
		pix_display_value_big(db, layer.border_box.left(), layer.border_box.top(), freq, fullwidth, comma, comma + 3, rj, blinkpos, blinkstate, 1);	// отрисовываем верхнюю часть строки
#endif /* WITHPRERENDER */
	}
	else if (! strcmp(url.c_str(), dzi_gcombo.id))
	{
		uint_fast8_t x = layer.border_box.left() / 16;
		uint_fast8_t y = layer.border_box.top() / 5;
		uint_fast8_t w = layer.border_box.width / 16;
		uint_fast8_t h = layer.border_box.height / 5;
		display2_gcombo(db, x, y, w, h, NULL);

	}
	else
	{
		colpip_fillrect(db, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, COLORPIP_RED);
	}
}

void hftrxgd::draw_solid_fill(litehtml::uint_ptr hdc, const background_layer &layer, const web_color &color)
{
	const gxdrawb_t * const db = (const gxdrawb_t *) hdc;
	//PRINTF("draw_solid_fill: bottom_left_x=%d\n", layer.border_radius.bottom_left_x);

	colpip_fillrect(db, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, getCOLORPIP(color));
}

void hftrxgd::draw_linear_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::linear_gradient &gradient)
{
	const gxdrawb_t * const db = (const gxdrawb_t *) hdc;
	//TP();
	COLORPIP_T color = COLORPIP_RED;

	colpip_fillrect(db, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, color);

}
void hftrxgd::draw_radial_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::radial_gradient &gradient)
{
	const gxdrawb_t * const db = (const gxdrawb_t *) hdc;
	//TP();
	COLORPIP_T color = COLORPIP_GREEN;

	colpip_fillrect(db, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, color);

}

void hftrxgd::draw_conic_gradient(litehtml::uint_ptr hdc, const background_layer &layer, const background_layer::conic_gradient &gradient)
{
	const gxdrawb_t * const db = (const gxdrawb_t *) hdc;
	//TP();
	COLORPIP_T color = COLORPIP_BLUE;

	colpip_fillrect(db, layer.border_box.left(), layer.border_box.top(), layer.border_box.width, layer.border_box.height, color);

}

void hftrxgd::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders &borders, const litehtml::position &draw_pos, bool root)
{
	//PRINTF("draw_borders: bottom_left_x=%d\n", borders.radius.bottom_left_x);
	const gxdrawb_t * const db = (const gxdrawb_t *) hdc;
	COLORPIP_T top_color = getCOLORPIP(borders.top.color);
	COLORPIP_T botom_color = getCOLORPIP(borders.bottom.color);
	COLORPIP_T left_color = getCOLORPIP(borders.left.color);
	COLORPIP_T right_color = getCOLORPIP(borders.right.color);
	colpip_rect(db, draw_pos.left(), draw_pos.top(), draw_pos.right() - 1, draw_pos.bottom() - 1, top_color, 0);
}

void hftrxgd::set_caption(const char *caption)
{
	//PRINTF("set_caption: caption='%s'\n", caption);
}
void hftrxgd::set_base_url(const char *base_url)
{
	//PRINTF("set_base_url: base_url='%s'\n", base_url);
}
void link(const std::shared_ptr<litehtml::document> &doc, const litehtml::element::ptr &el)
{
	PRINTF("link: doc=%pid='%s'\n", (const void*) & doc, (const char*) el->id());

	//TP();
}
void hftrxgd::on_anchor_click(const char *url, const litehtml::element::ptr &el)
{
	TP();
}

// вызывается если попадаем в элемент
void hftrxgd::on_mouse_event(const litehtml::element::ptr &el, litehtml::mouse_event event)
{
	//TP();
	PRINTF("on_mouse_event: id='%d'\n", el->tag());
}
void hftrxgd::set_cursor(const char *cursor)
{
	PRINTF("set_cursor: cursor='%s'\n", cursor);
}

void hftrxgd::transform_text(litehtml::string &text, litehtml::text_transform tt)
{
	TP();
}
void hftrxgd::import_css(litehtml::string &text, const litehtml::string &url, litehtml::string &baseurl)
{
	//TP();
}

void hftrxgd::set_clip(const litehtml::position &pos, const litehtml::border_radiuses &bdr_radius)
{
	TP();
}
void hftrxgd::del_clip()
{
	TP();
}
void hftrxgd::get_client_rect(litehtml::position &client) const
{
	client = litehtml::position(0, 0, DIM_X, DIM_Y);
}
void hftrxgd::get_viewport(litehtml::position& viewport) const
{
	//ASSERT(0);
}

#if 0
class freqel: public litehtml::el_td
{
	virtual void draw(uint_ptr hdc, int x, int y, const position *clip, const std::shared_ptr<render_item> &ri);
	virtual void draw_background(uint_ptr hdc, int x, int y, const position *clip, const std::shared_ptr<render_item> &ri);
	virtual void get_text(string &text);
	virtual void get_content_size(size &sz, int max_width);

	virtual bool on_mouse_over() override;
	virtual bool on_mouse_leave() override;
	virtual bool on_lbutton_down() override;
	virtual bool on_lbutton_up() override;
	virtual void on_click() override;

	int m_rxid;
public:
	freqel(const std::shared_ptr<document> &doc, int rxid) :
			litehtml::el_td(doc), m_rxid(rxid)
	{
		set_data("Hello set data");
	}
	virtual ~ freqel() = default;
};

void freqel::draw(uint_ptr hdc, int x, int y, const position *clip, const std::shared_ptr<render_item> &ri)
{
	PACKEDCOLORPIP_T *const buffer = colmain_fb_draw();
	const int dx = DIM_X;
	const int dy = DIM_Y;
	COLORPIP_T color = COLORPIP_BLUE;

	const position &pos = ri->pos();
	string text;
	get_text(text);
	PRINTF("draw: x=%d, y=%d, text='%s', x=%d, y=%d, w=%d, h=%d, U=%d\n", x, y, text.c_str(), pos.x, pos.y, pos.width, pos.height, ri->margin_top());
	//colpip_fillrect(buffer, dx, dy, x + pos.x, y + pos.y, pos.width, pos.height, color);
	litehtml::el_td::draw(hdc, x, y, clip, ri);
}

void freqel::draw_background(uint_ptr hdc, int x, int y, const position *clip, const std::shared_ptr<render_item> &ri)
{
	PACKEDCOLORPIP_T *const buffer = colmain_fb_draw();
	const int dx = DIM_X;
	const int dy = DIM_Y;
	COLORPIP_T color = COLORPIP_BLUE;

	const position &pos = ri->pos();
	string text;
	get_text(text);
	PRINTF("draw_background: x=%d, y=%d, text='%s', x=%d, y=%d, w=%d, h=%d\n", x, y, text.c_str(), pos.x, pos.y, pos.width, pos.height);
	//colpip_fillrect(buffer, dx, dy, x + pos.x, y + pos.y, pos.width, pos.height, color);
	litehtml::el_td::draw_background(hdc, x, y, clip, ri);
}

void freqel::get_content_size(size &sz, int max_width)
{
	TP();
	sz = size(200, 50);
}

void freqel::get_text(string &text)
{
	//TP();
	text = "14030000";
}

bool freqel::on_mouse_over()
{
	PRINTF("on_mouse_over\n");
	return false;
}
bool freqel::on_mouse_leave()
{
	PRINTF("on_mouse_leave\n");
	return false;
}
bool freqel::on_lbutton_down()
{
	PRINTF("on_lbutton_down\n");
	return false;
}
bool freqel::on_lbutton_up()
{
	PRINTF("on_lbutton_up\n");
	return false;
}
void freqel::on_click()
{
	PRINTF("on_click\n");
}
#endif

litehtml::element::ptr hftrxgd::create_element(const char *tag_name, const litehtml::string_map &attributes, const std::shared_ptr<litehtml::document> &doc)
{
//	PRINTF("create_element: tag_name='%s'\n", tag_name);
//	for (const std::pair<string, string> &p: attributes)
//	{
//		PRINTF(" create_element: '%s'='%s'\n", p.first.c_str(), p.second.c_str());
//	}
	return nullptr;
#if 0
	try
	{
		std::string id = attributes.at("id");
		std::string classname = attributes.at("class");
		PRINTF("create_element: tag_name='%s', id='%s', class='%s'\n", tag_name, id.c_str(), classname.c_str());

		if (id == "FREQ_A")
		{
			auto newTag = std::make_shared<freqel>(doc, 0);
			return newTag;
		}
		if (id == "FREQ_B")
		{
			auto newTag = std::make_shared<freqel>(doc, 1);
			return newTag;
		}

	} catch (const std::out_of_range&)
	{
		//PRINTF("create_element: tag_name='%s', id not found\n", tag_name);
	}
	return nullptr;
#endif
}

void hftrxgd::get_media_features(litehtml::media_features &media) const
{
	position client;
	get_client_rect(client);

	media.type = media_type_screen;
	media.width = client.width;
	media.height = client.height;
	media.color = 8;
	media.monochrome = 0;
	media.color_index = 256;
	media.resolution = 96; //GetDeviceCaps(m_tmp_hdc, LOGPIXELSX);
	media.device_width = client.width; //1000; //GetDeviceCaps(m_tmp_hdc, HORZRES);
	media.device_height = client.height; //1000; //GetDeviceCaps(m_tmp_hdc, VERTRES);
}

void hftrxgd::get_language(litehtml::string &language, litehtml::string &culture) const
{
	language = "en";
	culture = "";
}

//void hftrxgd::split_text(const char *text, const std::function<void(const char*)> &on_word, const std::function<void(const char*)> &on_space)
//{
//	//PRINTF("split_text: text='%s'\n", text);
//	//on_word("zzzzzz");
//	on_word(text);
//}

void hftrxgd::link(const std::shared_ptr<litehtml::document> &doc, const litehtml::element::ptr &el)
{
	//TP();
}


	class el_text2: public litehtml::el_text
	{
	public:
		virtual std::shared_ptr<render_item> create_render_item(const std::shared_ptr<render_item>& parent_ri);
		virtual bool is_replaced() const override { TP(); return true; }
		explicit el_text2(const char* text, const document::ptr& doc) : el_text(text, doc) { }
	//	virtual void draw(uint_ptr hdc, int x, int y, const position *clip, const std::shared_ptr<render_item>& ri);
	//	virtual void draw_background(uint_ptr hdc, int x, int y, const position *clip, const std::shared_ptr<render_item> &ri);
	};

//void el_text2::draw(uint_ptr hdc, int x, int y, const position *clip, const std::shared_ptr<render_item> &ri)
//{
//	TP();
//}
//
//void el_text2::draw_background(uint_ptr hdc, int x, int y, const position *clip, const std::shared_ptr<render_item> &ri)
//{
//	TP();
//}

	class render_item_text2 : public render_item
	{
	protected:
//		int calc_max_height(int image_height, int containing_block_height);
//		int _render(int x, int y, const containing_block_context &containing_block_size, formatting_context* fmt_ctx, bool second_pass) override;

	public:
		explicit render_item_text2(std::shared_ptr<element>  src_el) : render_item(std::move(src_el))
		{}
		std::shared_ptr<render_item> clone() override
		{
			return std::make_shared<render_item_text2>(src_el());
		}
	};

	std::shared_ptr<litehtml::render_item> litehtml::el_text2::create_render_item(const std::shared_ptr<render_item>& parent_ri)
	{
		auto ret = std::make_shared<render_item_text2>(shared_from_this());
		ret->parent(parent_ri);
		return ret;
	}

}

using namespace litehtml;

static litehtml::hftrxgd hfrx_cont(DIM_X, DIM_Y);
static const litehtml::position hfrx_wndclip(0, 0, DIM_X, DIM_Y);
static document::ptr hftrxmain_docs [DISPLC_MODCOUNT];
static litehtml::elements_list hftrx_timeels;

static const char hftrx_css [] =
R"##(
html { display: block; }
head { display: none; }
meta {	display: none; }
title { display: none; }
link {	display: none; }
style { display: none; }
script { display: none; }
img { display: inline-block; }
)##";

#endif /* WITHRENDERHTML */


#if WITHSHOWSWRPWR
uint_fast8_t display2_getswrmax(void)
{
	return SWRMAX;
}
#endif /* WITHSHOWSWRPWR */

unsigned display2_gettileradius(void)
{
	return DISPLC_RADIUS;
}


static uint_fast8_t
validforredraw(
	const struct dzone * const dzp,
	uint_fast16_t subset
	)
{
	/* про off-screen composition отрисовываем все элементы вне
	 * зависимости от группы обновления, но для подходящей страницы.
	 */
	if ((dzp->subset & subset) == 0)
		return 0;
	return 1;
}

enum { WALKCOUNT = sizeof dzones / sizeof dzones [0] };

static uint_fast8_t getpageix(
	uint_fast8_t inmenu,		/* находимся в режиме отображения настроек */
	uint_fast8_t menuset	/* индекс режима отображения (0..DISPLC_MODCOUNT - 1) */
	)
{
	return inmenu ? PAGEMENU : menuset;
}

// выполнение отрисовки всех элементов за раз.
// Например при работе в меню
static void 
display_walktrough(
	const gxdrawb_t * db,
	uint_fast16_t subset,
	dctx_t * pctx
	)
{
	uint_fast8_t i;
	for (i = 0; i < WALKCOUNT; ++ i)
	{
		const struct dzone * const dzp = & dzones [i];

		if (validforredraw(dzp, subset) == 0)
			continue;
		(* dzp->redraw)(db, dzp->x, dzp->y, dzp->colspan, dzp->rowspan, pctx);
	}
}

//static int redrawreq;
//
//void display2_needupdate(void)
//{
//	redrawreq = 1;
//}

// Обработка событий тачскрина или мыши
uint_fast8_t display2_mouse(uint_fast16_t xe, uint_fast16_t ye, unsigned evcode, uint_fast8_t inmenu, uint_fast8_t menuset, dctx_t * ctx)
{

#if WITHLVGL
	return 0;

#elif LINUX_SUBSYSTEM
	return 0;

#elif WITHRENDERHTML
	litehtml::position::vector redraw_boxes;
	hftrxmain_docs [menuset]->on_lbutton_down(xe, ye, 0, 0, redraw_boxes);
	hftrxmain_docs [menuset]->on_lbutton_up(xe, ye, 0, 0, redraw_boxes);
	return 0;

#else
	const uint_fast16_t subset = REDRSUBSET(getpageix(inmenu, menuset));
	uint_fast8_t i;

	for (i = 0; i < WALKCOUNT; ++ i)
	{
		const struct dzone * const dzp = & dzones [i];
		const uint_fast16_t x = GRID2X(dzp->x);
		const uint_fast16_t y = GRID2Y(dzp->y);
		const uint_fast16_t w = GRID2X(dzp->colspan);
		const uint_fast16_t h = GRID2Y(dzp->rowspan);
		if (validforredraw(dzp, subset) == 0)
			continue;
		if (x < xe || (x + w) < xe)
			continue;
		if (y < ye || (y + h) < ye)
			continue;
		// Попали внутрь прямоугольника
		//(* p->redraw)(dzp->x, dzp->y, pctx);
		return 1;
	}
	return 0;
#endif

}

// Interface functions
// выполнение шагов state machine отображения дисплея
void display2_bgprocess(
		uint_fast8_t inmenu,
		uint_fast8_t menuset,	/* индекс режима отображения (0..DISPLC_MODCOUNT - 1) */
		dctx_t * pctx
		)
{
#if WITHLVGL && WITHLVGLINDEV
	inmenu = 0;
	menuset = demostate ? PAGESLEEP : 0;
	lv_group_focus_obj(xxmainwnds [menuset]);
#endif
	const uint_fast8_t ix = getpageix(inmenu, menuset);	// требуемая страница для показа (включая меню и sleep)

//	if (redrawreq == 0)
//		return;
//	redrawreq = 0;

#if WITHLVGL
	// Отрисовка производится диспетчером LVGL
	{
		uint_fast8_t page;
		for (page = 0; page < PAGEBITS; ++ page)
		{
			lv_obj_t * const wnd = xxmainwnds [page];
			if (wnd == NULL)
				continue;
			lv_obj_set_flag(wnd, LV_OBJ_FLAG_HIDDEN, page != ix);
		}
	}
	sendupdates();
	if (xxmainwnds [ix])
	{
		//lv_obj_move_foreground(xxmainwnds [ix]);
		lv_obj_invalidate(xxmainwnds [ix]);
	}
	compat_pctx = pctx;
	lv_timer_handler();
	return;

#elif LINUX_SUBSYSTEM
	gxdrawb_t dbv;
	gxdrawb_initialize(& dbv, colmain_fb_draw(), DIM_X, DIM_Y);
	// may be REDRSUBSET(ix)
	display_walktrough(& dbv, REDRSUBSET(menuset), pctx);

#elif ! LCDMODE_LTDC
	return;

#elif WITHRENDERHTML

	document::ptr doc = hftrxmain_docs [ix];
	if (doc == NULL)
		return;
	if (0)
	{
		litehtml::css_selector sel;
		//sel.parse("[id=FREQ_A]", no_quirks_mode);	// select by id
		sel.parse(".BIG-FREQ", no_quirks_mode);	// Select by class
		litehtml::elements_list testels = doc->root()->select_all(sel);
		//litehtml::element::ptr testel = hftrxmain_docs->root()->select_one(sel);
		for (litehtml::element::ptr& testel : testels)
		{
			//testel->set_data("+");
			testel->set_attr("style", "background-color:red; color:green;");
			testel->compute_styles(false);
			TP();
		}

	}
	gxdrawb_t dbv;
	gxdrawb_initialize(& dbv, colmain_fb_draw(), DIM_X, DIM_Y);
	const litehtml::uint_ptr hftrx_hdc = (uintptr_t) & dbv;
	if (1)
	{

		for (litehtml::element::ptr& el : hftrx_timeels)
		{
//			uint_fast8_t hour, minute, seconds;
//			board_rtc_cached_gettime(& hour, & minute, & seconds);

//			const litehtml::element::ptr & old = el->children().back();	// Единственный элемент
//
//			char s [16];
//			local_snprintf_P(s, sizeof s, "%08X", (unsigned) sys_now());
//
//			litehtml::element::ptr tp(std::make_shared<el_text2>(s, doc));

			const int phase = (sys_now() & 0x200) != 0;
			el->set_class("on", !! phase);
			el->set_class("off", ! phase);
			el->compute_styles(true);
			el->refresh_styles();

//			if (sys_now() & 0x200)
//			{
//				//el->set_attr("style", "background-color:red; color:green;");
//				//el->set_data("1");
//			}
//			else
//			{
//				//el->set_attr("style", "background-color:green; color:red;");
//				//el->set_data("2");
//			}



			//doc->append_children_from_string(* el, s);
//			ASSERT(el->appendChild(tp));
//			ASSERT(el->removeChild(old));

			//PRINTF("***** old = %p\n", old);
			//tp->compute_styles(false);
			//el->compute_styles(true);

			//doc->refreshh();
			//TP();
		}
	}

	doc->render(hfrx_wndclip.width, litehtml::render_all);
	doc->draw(hftrx_hdc, 0, 0, & hfrx_wndclip);
	//TP();
	colmain_nextfb();

#else
	// обычное отображение, без LVGL или litehtml
	gxdrawb_t dbv;
	gxdrawb_initialize(& dbv, colmain_fb_draw(), DIM_X, DIM_Y);
	display_walktrough(& dbv, REDRSUBSET(ix), pctx);
#endif
}

// Заполнение стилей для отображения
static void display2_stylesupdate(void)
{
	// Параметры отображения состояний из двух вариантов (RX/TX).
	// Параметры отображения состояния прием/передача
	gxstyle_initialize(& dbstylev_2rxtx [0]);
	gxstyle_textcolor(& dbstylev_2rxtx [0], COLORPIP_GREEN, COLORPIP_BLACK);	// RX
	gxstyle_initialize(& dbstylev_2rxtx [1]);
	gxstyle_textcolor(& dbstylev_2rxtx [1], COLORPIP_RED, COLORPIP_BLACK);	// TX

	// Параметры отображения состояний из четырех вариантов
	gxstyle_initialize(& dbstylev_4state [0]);
	gxstyle_textcolor(& dbstylev_4state [0], DSGN_LABELINACTIVETEXT, DSGN_LABELINACTIVEBACK);
	gxstyle_initialize(& dbstylev_4state [1]);
	gxstyle_textcolor(& dbstylev_4state [1], DSGN_LABELACTIVETEXT, DSGN_LABELACTIVEBACK);
	gxstyle_initialize(& dbstylev_4state [2]);
	gxstyle_textcolor(& dbstylev_4state [2], DSGN_LABELACTIVETEXT, DSGN_LABELACTIVEBACK);
	gxstyle_initialize(& dbstylev_4state [3]);
	gxstyle_textcolor(& dbstylev_4state [3], DSGN_LABELACTIVETEXT, DSGN_LABELACTIVEBACK);

	// Параметры отображения частоты основного приемника
	gxstyle_initialize(& dbstylev_1freqv);
	gxstyle_textcolor(& dbstylev_1freqv, DSGN_BIGCOLOR,	DSGN_BIGCOLORBACK);

	// Параметры отображения частоты и режима дополнительного приемника
	gxstyle_initialize(& dbstylev_2rxB [0]);
	gxstyle_textcolor(& dbstylev_2rxB [0], DSGN_LABELINACTIVETEXT, DSGN_LABELINACTIVEBACK);
	gxstyle_initialize(& dbstylev_2rxB [1]);
	gxstyle_textcolor(& dbstylev_2rxB [1], DSGN_LABELACTIVETEXT, DSGN_LABELACTIVEBACK);

	// Параметры отображения текстов без вариантов
	gxstyle_initialize(& dbstylev_1statePSU);
	gxstyle_textcolor(& dbstylev_1statePSU, DSGN_PSUSTATETEXT, DSGN_PSUSTATEBACK);

	// Параметры отображения состояний из двух вариантов
	gxstyle_initialize(& dbstylev_2state [0]);
	gxstyle_textcolor(& dbstylev_2state [0], DSGN_LABELINACTIVETEXT, DSGN_LABELINACTIVEBACK);
	gxstyle_initialize(& dbstylev_2state [1]);
	gxstyle_textcolor(& dbstylev_2state [1], DSGN_LABELACTIVETEXT, DSGN_LABELACTIVEBACK);
	// Параметры отображения состояний из двух вариантов

	gxstyle_initialize(& dbstylev_2stateSmall [0]);
	gxstyle_initialize(& dbstylev_2stateSmall [1]);
	gxstyle_textcolor(& dbstylev_2stateSmall [0], DSGN_LABELINACTIVETEXT, DSGN_LABELINACTIVEBACK);
	gxstyle_textcolor(& dbstylev_2stateSmall [1], DSGN_LABELACTIVETEXT, DSGN_LABELACTIVEBACK);
	gxstyle_setsmallfont2(& dbstylev_2stateSmall [0]);
	gxstyle_setsmallfont2(& dbstylev_2stateSmall [1]);
	gxstyle_texthalign(& dbstylev_2stateSmall [0], GXSTYLE_HALIGN_LEFT);
	gxstyle_texthalign(& dbstylev_2stateSmall [1], GXSTYLE_HALIGN_LEFT);

	// Параметры отображения текстов без вариантов
	gxstyle_initialize(& dbstylev_1state);
	gxstyle_textcolor(& dbstylev_1state, DSGN_LABELTEXT, DSGN_LABELBACK);

	// Параметры отображения состояний из двух вариантов (активный - на красном фонк)
	gxstyle_initialize(& dbstylev_2state_rec [0]);
	gxstyle_textcolor(& dbstylev_2state_rec [0], DSGN_LABELINACTIVETEXT, DSGN_LABELINACTIVEBACK);
	gxstyle_initialize(& dbstylev_2state_rec [1]);
	gxstyle_textcolor(& dbstylev_2state_rec [1], COLORPIP_RED, COLORPIP_BLACK);

	// Параметры отображения состояний FUNC MENU из двух вариантов
	gxstyle_initialize(& dbstylev_2fmenu [0]);
	gxstyle_textcolor(& dbstylev_2fmenu [0], DSGN_FMENUINACTIVETEXT, DSGN_FMENUINACTIVEBACK);
	gxstyle_initialize(& dbstylev_2fmenu [1]);
	gxstyle_textcolor(& dbstylev_2fmenu [1], DSGN_FMENUACTIVETEXT, DSGN_FMENUACTIVEBACK);

	// Параметры отображения состояний FUNC MENU без вариантов
	gxstyle_initialize(& dbstylev_1fmenu);
	gxstyle_textcolor(& dbstylev_1fmenu, DSGN_FMENUTEXT, DSGN_FMENUBACK);

	// Параметры отображения текстов без вариантов (время)
	gxstyle_initialize(& dbstylev_1stateTime);
	gxstyle_textcolor(& dbstylev_1stateTime, DSGN_BIGCOLOR, DSGN_BIGCOLORBACK);

	// Параметры отображения спектра и водопада
	colorcmarker = DSGN_GRIDCOLOR0;	// Цвет макркера на центре
	colorgridlines = DSGN_GRIDCOLOR2;	// Цвет линий сетки
	colordigits = DSGN_GRIDDIGITS;	// Цвет текста частот сетки
	colorgridlines3dss = COLORPIP_GREEN;

}

void display2_initialize(void)
{
	display2_stylesupdate();

#if (CPUSTYLE_R7S721 || 0)
	//scbfi_t * p = new (reinterpret_cast<void *>(& scbf)) scbfi_t;
#endif
#if WITHLVGL

	lvstales_initialize();	// эти стили нужны в linux ?
	updateslist_init();
	{
		xxgroup = lv_group_create();
		lv_obj_t * const parent = lv_screen_active();
    	// Всего страниц (включая неотображаемые - PAGEBITS
		uint_fast8_t page;
		for (page = 0; page < PAGEBITS; ++ page)
		{
			const uint_fast16_t subset = REDRSUBSET(page);
			if ((subset & REDRSUBSET_SHOW) == 0)
				continue;	// не треуется создавать страницу

			lv_obj_t * const wnd = lv_obj_create(parent);

			lv_obj_add_style(wnd, & xxmainstyle, 0);
			lv_obj_set_size(wnd, lv_obj_get_width(parent), lv_obj_get_height(parent));
			lv_obj_set_flag(wnd, LV_OBJ_FLAG_SCROLLABLE, false);
			lv_obj_set_style_rotary_sensitivity(wnd, 1, LV_PART_MAIN);	// не помогло. Объект должен быть editable

			unsigned i;
			for (i = 0; i < WALKCOUNT; ++ i)
			{
				const struct dzone * const dzp = & dzones [i];

				if (validforredraw(dzp, subset) == 0 || dzp->colspan == 0 || dzp->rowspan == 0)
					continue;

				const dzitem_t * const dzip = dzp->dzip;	// параметры создания элемента
				lv_obj_t * lbl;
				if (dzip != NULL && dzip->lvelementcreate != NULL)
				{
					lbl = (dzip->lvelementcreate)(wnd, dzp, dzip, i);
				}
				else
				{
					// Для целей отладки создаём видимый элемент
					lbl = lv_label_create(wnd);
					lv_obj_add_style(lbl, & xxdivstyle, 0);
					lv_label_set_text_fmt(lbl, "ix%d", (int) i);
				}

				if (lbl == NULL)
					continue;

				lv_obj_set_pos(lbl, GRID2X(dzp->x), GRID2Y(dzp->y));
				lv_obj_set_size(lbl, GRID2X(dzp->colspan), GRID2Y(dzp->rowspan));

			}

			// Включаем завершённую страницу
			if (lv_obj_get_child_count(wnd) != 0)
			{
				lv_obj_set_flag(wnd, LV_OBJ_FLAG_HIDDEN, page != 0);
#if WITHLVGLINDEV
				switch (page)
				{
				case 0:
					lv_obj_add_event_cb(wnd, mainwndkeyhandler, LV_EVENT_KEY, NULL);		// работает
					//lv_obj_add_event_cb(wnd, mainwndkeyhandler, LV_EVENT_ROTARY, NULL);		// Объект должен быть editable
					lv_obj_add_event_cb(wnd, mainwndkeyhandler, LV_EVENT_CLICKED, NULL);	// работает
					break;
				case PAGESLEEP:
					lv_obj_add_event_cb(wnd, sleepwndkeyhandler, LV_EVENT_KEY, NULL);		// работает
					//lv_obj_add_event_cb(wnd, sleepwndkeyhandler, LV_EVENT_ROTARY, NULL);	// Объект должен быть editable
					lv_obj_add_event_cb(wnd, sleepwndkeyhandler, LV_EVENT_CLICKED, NULL);	// работает
					break;
				}
				lv_group_add_obj(xxgroup, wnd);
#endif /* WITHLVGLINDEV */
				PRINTF("xxmainwnds[%u]=%p\n", (unsigned) page, wnd);
				xxmainwnds [page] = wnd;
			}
			else
			{
				lv_obj_delete(wnd);
				xxmainwnds [page] = NULL;
			}
		}
	}
#if WITHENCODER_1F
	{
		lv_indev_t * indev = lv_indev_create();
		lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);
		lv_indev_set_driver_data(indev, & encoder_ENC1F);
		lv_indev_set_read_cb(indev, input_encoder_read_cb);
#if WITHLVGLINDEV
		if (xxgroup)
		{
			lv_indev_set_group(indev, xxgroup);
		}
#endif /* WITHLVGLINDEV */
	}
#endif /* WITHENCODER_1F */
#if WITHENCODER_2F
	{
		lv_indev_t * indev = lv_indev_create();
		lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);
		lv_indev_set_driver_data(indev, & encoder_ENC2F);
		lv_indev_set_read_cb(indev, input_encoder_read_cb);
#if WITHLVGLINDEV
		if (xxgroup)
		{
			lv_indev_set_group(indev, xxgroup);
		}
#endif /* WITHLVGLINDEV */
	}
#endif /* WITHENCODER_2F */
#if WITHENCODER_3F
	{
		lv_indev_t * indev = lv_indev_create();
		lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);
		lv_indev_set_driver_data(indev, & encoder_ENC3F);
		lv_indev_set_read_cb(indev, input_encoder_read_cb);
#if WITHLVGLINDEV
		if (xxgroup)
		{
			lv_indev_set_group(indev, xxgroup);
		}
#endif /* WITHLVGLINDEV */
	}
#endif /* WITHENCODER_3F */
#if WITHENCODER_4F
	{
		lv_indev_t * indev = lv_indev_create();
		lv_indev_set_type(indev, LV_INDEV_TYPE_ENCODER);
		lv_indev_set_driver_data(indev, & encoder_ENC4F);
		lv_indev_set_read_cb(indev, input_encoder_read_cb);
#if WITHLVGLINDEV
		if (xxgroup)
		{
			lv_indev_set_group(indev, xxgroup);
		}
#endif /* WITHLVGLINDEV */
	}
#endif /* WITHENCODER_4F */

#if defined (TSC1_TYPE)
	{
		lv_indev_t * indev = lv_indev_create();
		lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
		lv_indev_set_read_cb(indev, input_tsc_read_cb);
	}
#endif /* defined (TSC1_TYPE) */
#if WITHKEYBOARD
	{
		lv_indev_t * indev = lv_indev_create();
		lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);
		lv_indev_set_read_cb(indev, input_keypad_read_cb);
#if WITHLVGLINDEV
		if (xxgroup)
		{
			lv_indev_set_group(indev, xxgroup);
		}
#endif /* WITHLVGLINDEV */
	}
#endif /* WITHKEYBOARD) */
	// TODO: add encoder(s)

//	{
//		static lv_timer_t * lvgl_task1;
//		lvgl_task1 = lv_timer_create(lvgl_task1_cb, 1, NULL);
//		lv_timer_set_repeat_count(lvgl_task1, -1);
//	}

#endif /* WITHLVGL */

#if 0
	{
		uint_fast8_t page;
		PRINTF("+++++++++++++++++++++++\n");
		for (page = 0; page < DISPLC_MODCOUNT; ++ page)
		{
			PRINTF("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n");
			PRINTF("<html>\n");
			PRINTF("<head>\n");
			PRINTF("<meta charset=\"utf-8\">\n");
			PRINTF("<title>HF TRX 800x480</title>\n");
			PRINTF("<meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0\">\n");
			PRINTF("<!--link rel=\"stylesheet\" type=\"text/css\" href=\"hftrx.css\"-->\n");
			// Формирование шаблона с html элементами
			const uint_fast16_t subset = REDRSUBSET(page);
			uint_fast8_t i;
			// Styles section
			//		/*----------0---------------*/
			//		#dbm {position: fixed; top:5px;left:10px;width:80px;height:22px;}
			//		#filter {position: fixed; top:5px;left:115px;width:48px;height:22px;}
			//		#smeter {position: fixed; top:25px; left:0px;}
			//		#rxtx {position: fixed; top:5px;left:170px;width:48px;height:22px;}
			PRINTF("<style>\n");
			for (i = 0; i < WALKCOUNT; ++ i)
			{
				const struct dzone * const dzp = & dzones [i];

				if (validforredraw(dzp, subset) == 0)
					continue;
				if (dzp->colspan == 0 || dzp->rowspan == 0)
					continue;
				//
				if (dzp->dzip != NULL && dzp->dzip->id != NULL)
				{
					PRINTF(" #%s { position:absolute; left:%dpx; top:%dpx; width:%dpx; height:%dpx; }\n",
							dzp->dzip->id,
							(int) GRID2X(dzp->x), (int) GRID2Y(dzp->y), (int) GRID2X(dzp->colspan), (int) GRID2Y(dzp->rowspan));
				}
				else
				{
					PRINTF(" #id%d { position:absolute; left:%dpx; top:%dpx; width:%dpx; height:%dpx; }\n",
							(int) i,
							(int) GRID2X(dzp->x), (int) GRID2Y(dzp->y), (int) GRID2X(dzp->colspan), (int) GRID2Y(dzp->rowspan));
				}
			}
			PRINTF("</style>\n");
			PRINTF("</head>\n");

			PRINTF("<body style=\"background-color:orange;\">\n");
			for (i = 0; i < WALKCOUNT; ++ i)
			{
				const struct dzone * const dzp = & dzones [i];

				if (validforredraw(dzp, subset) == 0)
					continue;
				if (dzp->colspan == 0 || dzp->rowspan == 0)
					continue;
				if (dzp->dzip != NULL && dzp->dzip->id != NULL)
				{
					PRINTF(" <div id=\"%s\" style=\"background-color:blue; color:black;\"><img src='%s'/></div>\n",
							dzp->dzip->id,
							dzp->dzip->id
							);
				}
				else
				{
					PRINTF(" <div id=\"id%d\" style=\"background-color:blue; color:black;\">%*.*s</div>\n",
							(int) i,
							dzp->colspan, dzp->colspan, "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"
							);
				}
			}
			PRINTF("</body>\n");

			PRINTF("</html>\n");
			PRINTF("-------------------------\n");
		}
	}
#endif

	display_walktrough(NULL, REDRSUBSET_INIT, NULL);// выполнение отрисовки всех элементов за раз.
	//redrawreq = 0;

#if WITHRENDERHTML
	{
		uint_fast8_t page;
		for (page = 0; page < DISPLC_MODCOUNT; ++ page)
		{
			TP();
			hftrxmain_docs [page] = litehtml::document::createFromString(display2_gethtml(page), & hfrx_cont, hftrx_css);
			TP();
			if (1)
			{
				litehtml::css_selector sel;
				sel.parse("#id3", no_quirks_mode);	// select by id
				//sel.parse(".BIG-FREQ", no_quirks_mode);	// Select by class
				hftrx_timeels = hftrxmain_docs [page]->root()->select_all(sel);
				PRINTF("hftrx_timeels size=%d\n", hftrx_timeels.size());
	//			for (litehtml::element::ptr& el : hftrx_timeels)
	//			{
	//				for (const litehtml::element::ptr& el2 : el->children())
	//				{
	//					string t;
	//					el2->get_text(t);
	//					PRINTF(" children: name='%s'\n", el2->dump_get_name().c_str());
	//					PRINTF(" children: text='%s'\n", t.c_str());
	//				}
	//			}
			}
		}
	}

#endif /* WITHRENDERHTML */
}

#if LCDMODE_COLORED
static COLORPIP_T bgcolor = COLORPIP_BLACK;
#endif /* LCDMODE_COLORED */

void
display2_setbgcolor(COLORPIP_T c)
{
#if LCDMODE_COLORED
	bgcolor = c;
#endif /* LCDMODE_COLORED */
}

COLORPIP_T
display2_getbgcolor(void)
{
#if LCDMODE_COLORED
	return bgcolor;
#else /* LCDMODE_COLORED */
	return COLOR_BLACK;
#endif /* LCDMODE_COLORED */
}

// очистить дисплей
void display2_fillbg(const gxdrawb_t * db)
{
	colpip_fillrect(db, 0, 0, DIM_X, DIM_Y, display2_getbgcolor());
}

void display2_latch(void)
{
	display_walktrough(NULL, REDRSUBSET_LATCH, NULL);// обновление данных всех элементов за раз. draw buffer не требуется
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
display2_bars_rx(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t colspan, uint_fast8_t rowspan, dctx_t * pctx)
{
#if WITHBARS
	int_fast16_t tracemaxi10;
	int_fast16_t rssi10 = dsp_rssi10(& tracemaxi10, 0);	/* получить значение уровня сигнала для s-метра в 0.1 дБмВт */
	display_smeter(db, x, y, rssi10, tracemaxi10, colspan, rowspan);
#endif /* WITHBARS */
}

// SWR-METER, POWER-METER
/* отображение P-метра и SWR-метра на приёме или передаче */
// Функция вызывается из display2.c
void
display2_bars_tx(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast8_t colspan, uint_fast8_t rowspan, dctx_t * pctx)
{
#if WITHBARS
#if WITHTX
	#if (WITHSWRMTR || WITHSHOWSWRPWR)
		uint_fast8_t pwrtrace;
		const uint_fast8_t pwr = board_getpwrmeter(& pwrtrace);
		adcvalholder_t reflected;
		const adcvalholder_t forward = board_getswrmeter_cached(& reflected, swrcalibr);
		#if WITHSHOWSWRPWR
			display_swrmeter(db, x, y, colspan, rowspan, forward, reflected, minforward);
			display_pwrmeter(db, x, y, colspan, rowspan, pwr, pwrtrace, maxpwrcali);
		#else
			if (swrmode || getactualtune())
				display_swrmeter(db, x, y, colspan, rowspan, forward, reflected, minforward);
			else
				display_pwrmeter(db, x, y, colspan, rowspan, pwr, pwrtrace, maxpwrcali);
		#endif
	#endif

#endif /* WITHTX */
#endif /* WITHBARS */
}

// Установка параметров отображения
/* дизайн спектра под 3DSS Yaesu */
void
board_set_view_style(uint_fast8_t v)
{
	const uint_fast8_t n = v;

	if (glob_view_style != n)
	{
		glob_view_style = n;
	}
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

/* Часть отведенной под спектр высоты экрана 0..100 */
void
display2_set_spectrumpart(uint_fast8_t v)
{
	glob_spectrumpart = v;
}

void board_set_displayfps(uint_fast8_t v)
{
#if WITHSPECTRUMWF
	if (glob_displayfps != v && v != 0)
	{
		glob_displayfps = v;
		agc_parameters_peaks_initialize(& scbf.peakparams, glob_displayfps);	// частота latch
	}
#endif /* WITHSPECTRUMWF */
}

/* 0..100 - насыщнность цвета заполнения "шторки" - индикатор полосы пропускания примника на спкктре. */
void
display2_set_rxbwsatu(uint_fast8_t v)
{
	glob_rxbwsatu = v;
}

/* уменьшение отображаемого участка спектра */
// horizontal magnification power of two
void
board_set_zoomxpow2(uint_fast8_t v)
{
	glob_zoomxpow2 = v;
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
	if (glob_smetertype != v)
	{
		glob_smetertype = v;
		first_tx = 1;
	}
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
	return wfpalette [dsp_mag2y(scbf.spavgarray.peek(x, 0), PALETTESIZE - 1, glob_topdb, glob_bottomdb)];
}

#endif /* WITHTOUCHGUI */
