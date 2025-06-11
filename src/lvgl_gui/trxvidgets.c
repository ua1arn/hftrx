/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if WITHLVGL

#include "formats.h"
#include "board.h"
#include "display2.h"
#include "audio.h"

#include "lvgl.h"
#include "core/lv_obj_private.h"
#include "core/lv_obj_class_private.h"
#include "widgets/label/lv_label_private.h"
#include "widgets/image/lv_image_private.h"
#include "misc/lv_area_private.h"

#include "styles.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS_SMTR2 (& lv_smtr2_class)	// собственный renderer
#define MY_CLASS_TXRX (& lv_txrx_class)		// собственный renderer
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

static void lv_txrx_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
//static void lv_txrx_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_txrx_event(const lv_obj_class_t * class_p, lv_event_t * e);

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
    .constructor_cb = lv_smtr2_constructor,
//    .destructor_cb = lv_smtr2_destructor,
    .event_cb = lv_smtr2_event,
    .base_class = & lv_obj_class,
    .instance_size = sizeof (lv_smtr2_t),
    .name = "hmr_smtr2",
};

static const lv_obj_class_t lv_txrx_class  = {
    .constructor_cb = lv_txrx_constructor,
//    .destructor_cb = lv_txrx_destructor,
    .event_cb = lv_txrx_event,
    .base_class = & lv_label_class,
    .instance_size = sizeof (lv_txrx_t),
    .name = "hmr_txrx",
};

static const lv_obj_class_t lv_info_class  = {
    .constructor_cb = lv_info_constructor,
//    .destructor_cb = lv_info_destructor,
    .event_cb = lv_info_event,
    .base_class = & lv_label_class,
    .instance_size = sizeof (lv_info_t),
    .name = "hmr_nfo",
};

static const lv_obj_class_t lv_wtrf2_class  = {
    .constructor_cb = lv_wtrf2_constructor,
//    .destructor_cb = lv_wtrf2_destructor,
    .event_cb = lv_wtrf2_event,
    .base_class = & lv_obj_class,
    .instance_size = sizeof (lv_wtrf2_t),
    .name = "hmr_wtrf2",
};

static const lv_obj_class_t lv_compat_class  = {
    .constructor_cb = lv_compat_constructor,
//    .destructor_cb = lv_compat_destructor,
    .event_cb = lv_compat_event,
    .base_class = & lv_obj_class,
    .instance_size = sizeof (lv_compat_t),
    .name = "hmr_compat",
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

lv_obj_t * lv_txrx_create(lv_obj_t * parent) {
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_TXRX, parent);
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

static void lv_txrx_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_txrx_t * const cp = (lv_txrx_t *) obj;

    const int state = hamradio_get_tx();
    lv_snprintf(cp->text, ARRAY_SIZE(cp->text), "%s", "--");
    lv_label_set_text_static(obj, cp->text);
    LV_TRACE_OBJ_CREATE("finished");
}

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


static void lv_txrx_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);
    lv_obj_t * const obj = (lv_obj_t *) lv_event_get_target(e);
	const lv_event_code_t code = lv_event_get_code(e);
    LV_ASSERT_OBJ(obj, MY_CLASS_TXRX);

    // текст обновляем перед отрисовкой
    if (LV_EVENT_DRAW_MAIN_BEGIN == code)
    {
    	lv_txrx_t   * const cp = (lv_txrx_t *) obj;
        const int state = hamradio_get_tx();
        lv_snprintf(cp->text, ARRAY_SIZE(cp->text), "%s", state ? "TX" : "RX");
    }

    lv_res_t res = lv_obj_event_base(MY_CLASS_TXRX, e);	// обработчик родительского клвсса
    if (res != LV_RES_OK) return;
}

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
    	int seelctor = 0;	// вариант стиля отображения если надо менять в зависимости от ситуации
        int len = (* cp->infocb)(cp->infotext, ARRAY_SIZE(cp->infotext) - 1, & seelctor);
        if (len > 0)
        	cp->infotext [len] = '\0';
        else
        	cp->infotext [0] = '\0';
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

// custom draw widget
static void lv_smtr2_event(const lv_obj_class_t * class_p, lv_event_t * e) {
    LV_UNUSED(class_p);

    lv_res_t res = lv_obj_event_base(MY_CLASS_SMTR2, e);	// обработчик родительского клвсса

    if (res != LV_RES_OK) return;

    lv_obj_t  * const obj = (lv_obj_t *) lv_event_get_target(e);
	const lv_event_code_t code = lv_event_get_code(e);
    LV_ASSERT_OBJ(obj, MY_CLASS_SMTR2);

    if (LV_EVENT_DRAW_MAIN == code)
    {
		lv_layer_t * const layer = lv_event_get_layer(e);
		lv_smtr2_t * const smtr2 = (lv_smtr2_t *) obj;

        lv_area_t coords;
        lv_obj_get_coords(obj, & coords);	// координаты объекта
        const int_fast32_t h = lv_area_get_height(& coords);
        const int_fast32_t w = lv_area_get_width(& coords);

        lv_area_t smeterbar;
        lv_area_set(& smeterbar, coords.x1, coords.y1 + h / 3, coords.x2, coords.y1 + h * 2 / 3);

        int_fast32_t gs = 0;
        int_fast32_t gm = lv_area_get_width(& coords) / 2;
        int_fast32_t ge = lv_area_get_width(& coords) - 1;

		const adcvalholder_t power = board_getadc_unfiltered_truevalue(PWRMRRIX);	// без возможных тормозов на SPI при чтении
		uint_fast8_t tracemax;
		uint_fast8_t smtrvalue = board_getsmeter(& tracemax, 0, UINT8_MAX, 0);
		int_fast32_t gv_pos = gs + normalize31(smtrvalue, s9level - s9delta, s9level, s9level + s9_60_delta, gm - gs, ge - gs);
		int_fast32_t gv_trace = gs + normalize31(tracemax, s9level - s9delta, s9level, s9level + s9_60_delta, gm - gs, ge - gs);

        if (1)
        {

            lv_draw_rect_dsc_t rect;
            lv_draw_rect_dsc_init(& rect);

            rect.bg_color = lv_palette_main(LV_PALETTE_GREY);
            rect.bg_image_opa = LV_OPA_COVER;

        	lv_draw_rect(layer, & rect, & coords);

        	if (gv_trace > gv_pos)
        	{
        		lv_area_t cplus;
        		lv_area_set(& cplus, smeterbar.x1 + gv_pos, smeterbar.y1 + 0, smeterbar.x1 + gv_trace, smeterbar.y2 + 0);
        		rect.bg_color = lv_palette_main(LV_PALETTE_RED);
            	lv_draw_rect(layer, & rect, & cplus);
        	}
        	if (gv_pos > 0)
        	{
        		lv_area_t cplus;
        		lv_area_set(& cplus, smeterbar.x1 + 0, smeterbar.y1 + 0, smeterbar.x1 + gv_pos, smeterbar.y2 + 0);
        		rect.bg_color = lv_palette_main(LV_PALETTE_YELLOW);
            	lv_draw_rect(layer, & rect, & cplus);
        	}
        }

    	if (0)
    	{
            // градиент
            lv_draw_rect_dsc_t rect;
            lv_draw_rect_dsc_init(& rect);

            rect.bg_color = lv_palette_main(LV_PALETTE_BLUE);
            rect.bg_image_opa = LV_OPA_COVER;

            // Update LV_GRADIENT_MAX_STOPS in lv_conf.h
            static const lv_color_t grad_colors [] =
            {
                LV_COLOR_MAKE(0xff, 0x00, 0x00),
                LV_COLOR_MAKE(0x00, 0xff, 0x00),
                LV_COLOR_MAKE(0x00, 0x00, 0x7f),
            };

    		lv_grad_init_stops(& rect.bg_grad, grad_colors, NULL, NULL, ARRAY_SIZE(grad_colors));
    		lv_grad_vertical_init(& rect.bg_grad);
    		//lv_grad_conical_init(& rect.bg_grad, lv_pct(50), lv_pct(50), 0, 180, LV_GRAD_EXTEND_PAD);

    	   	lv_draw_rect(layer, & rect, & coords);
    	}

    	if (0)
    	{
    		// lines test
            lv_draw_line_dsc_t dsc;
            lv_draw_line_dsc_init(& dsc);

            dsc.width = 1;
            dsc.round_end = 0;
            dsc.round_start = 0;

            // диагональ
            dsc.color = lv_palette_main(LV_PALETTE_YELLOW);
            lv_point_precise_set(& dsc.p1, coords.x1, coords.y1);
            lv_point_precise_set(& dsc.p2, coords.x2, coords.y2);
            lv_draw_line(layer, & dsc);

            dsc.color = lv_palette_main(LV_PALETTE_RED);
            lv_point_precise_set(& dsc.p1, coords.x1 + gv_pos, coords.y1);
            lv_point_precise_set(& dsc.p2, coords.x1 + gv_pos, coords.y2);
            lv_draw_line(layer, & dsc);

            dsc.color = lv_palette_main(LV_PALETTE_LIGHT_GREEN);
            lv_point_precise_set(& dsc.p1, coords.x1 + gv_trace, coords.y1);
            lv_point_precise_set(& dsc.p2, coords.x1 + gv_trace, coords.y2);
            lv_draw_line(layer, & dsc);
    	}
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
        lv_obj_get_coords(obj, & coords);	// координаты объекта

        lv_wtrf2_draw(layer, & coords);
     }
}



#endif /* WITHLVGL */

