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
#define MY_CLASS_SMTR (& lv_smtr_class)
#define MY_CLASS_SMTR2 (& lv_smtr2_class)
#define MY_CLASS_TXRX (& lv_txrx_class)
#define MY_CLASS_WTRF2 (& lv_wtrf2_class)
#define MY_CLASS_SSCP2 (& lv_sscp2_class)
#define MY_CLASS_WTRF (& lv_wtrf_class)
#define MY_CLASS_INFO (& lv_info_class)
#define MY_CLASS_COMPAT (& lv_compat_class)

/**********************
 *  STATIC PROTOTYPES
 **********************/
 
static void lv_smtr_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
//static void lv_smtr2_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_smtr_event(const lv_obj_class_t * class_p, lv_event_t * e);

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

static void lv_sscp2_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
//static void lv_sscp2_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_sscp2_event(const lv_obj_class_t * class_p, lv_event_t * e);

static void lv_compat_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
//static void lv_compat_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_compat_event(const lv_obj_class_t * class_p, lv_event_t * e);

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
	lv_image_t img;
	//
} lv_smtr_t;

typedef struct
{
	lv_obj_t obj;
	//
} lv_smtr2_t;

typedef struct
{
	lv_label_t label;
	//
	char text [32];
} lv_txrx_t;

typedef struct
{
	lv_label_t label;
	char infotext [32];
	int (* infocb)(char * b, size_t len);
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
	lv_style_t stdigits;
	lv_style_t stlines;
} lv_sscp2_t;

typedef struct
{
	lv_obj_t obj;
	const void * dzpv;
} lv_compat_t;

/**********************
 *  STATIC VARIABLES
 **********************/

static const lv_obj_class_t lv_smtr_class  = {
    .constructor_cb = lv_smtr_constructor,
//    .destructor_cb = lv_smtr_destructor,
    .event_cb = lv_smtr_event,
    .base_class = & lv_image_class,
    .instance_size = sizeof (lv_smtr_t),
    .name = "hmr_smtr",
};

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

static const lv_obj_class_t lv_sscp2_class  = {
    .constructor_cb = lv_sscp2_constructor,
//    .destructor_cb = lv_sscp2_destructor,
    .event_cb = lv_sscp2_event,
    .base_class = & lv_obj_class,
    .instance_size = sizeof (lv_sscp2_t),
    .name = "hmr_sscp2",
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

lv_obj_t * lv_smtr_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_SMTR, parent);
    lv_obj_class_init_obj(obj);

#if WITHLVGL && WITHBARS
	lv_image_set_src(obj, smtr_get_draw_buff());	// src_type=LV_IMAGE_SRC_VARIABLE
#endif /* WITHLVGL && WITHBARS */

	return obj;
}

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

lv_obj_t * lv_sscp2_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_SSCP2, parent);
    lv_obj_class_init_obj(obj);

	return obj;
}

lv_obj_t * lv_txrx_create(lv_obj_t * parent) {
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_TXRX, parent);
    lv_obj_class_init_obj(obj);

    return obj;
}

lv_obj_t * lv_info_create(lv_obj_t * parent, int (* infocb)(char * b, size_t len))
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

static void lv_sscp2_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_sscp2_t * const cp = (lv_sscp2_t *) obj;

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

#if WITHLVGL && WITHBARS && 0
	lv_img_set_src(obj, smtr_get_draw_buff());	// src_type=LV_IMAGE_SRC_VARIABLE
#endif /* WITHLVGL && WITHBARS */

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_smtr_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_smtr_t * const cp = (lv_smtr_t *) obj;

#if WITHLVGL && WITHBARS
	lv_img_set_src(obj, smtr_get_draw_buff());	// src_type=LV_IMAGE_SRC_VARIABLE
#endif /* WITHLVGL && WITHBARS */

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
    lv_obj_t  * const obj = (lv_obj_t *) lv_event_get_target(e);
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
        int len = (* cp->infocb)(cp->infotext, ARRAY_SIZE(cp->infotext) - 1);
        if (len > 0)
        	cp->infotext [len] = '\0';
        else
        	cp->infotext [0] = '\0';
    }

    lv_res_t res = lv_obj_event_base(MY_CLASS_INFO, e);	// обработчик родительского клвсса
    if (res != LV_RES_OK) return;
}

static
uint_fast16_t normalize31(
	uint_fast16_t raw,
	uint_fast16_t rawmin,
	uint_fast16_t rawmid,
	uint_fast16_t rawmax,
	uint_fast16_t range1,
	uint_fast16_t range2
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

        uint_fast16_t gs = 0;
        uint_fast16_t gm = lv_area_get_width(& coords) / 2;
        uint_fast16_t ge = lv_area_get_width(& coords) - 1;

		const adcvalholder_t power = board_getadc_unfiltered_truevalue(PWRMRRIX);	// без возможных тормозов на SPI при чтении
		uint_fast8_t tracemax;
		uint_fast8_t smtrvalue = board_getsmeter(& tracemax, 0, UINT8_MAX, 0);
		uint_fast16_t gv_pos = gs + normalize31(smtrvalue, s9level - s9delta, s9level, s9level + s9_60_delta, gm - gs, ge - gs);
		uint_fast16_t gv_trace = gs + normalize31(tracemax, s9level - s9delta, s9level, s9level + s9_60_delta, gm - gs, ge - gs);

        lv_draw_rect_dsc_t rect;
        lv_draw_rect_dsc_init(& rect);

        //rect.bg_color = state ? lv_color_make(255, 255, 0) : lv_color_make(0, 0, 255);
        rect.bg_color = lv_color_make(0, 0, 255);
        rect.bg_image_opa = LV_OPA_COVER;

    	lv_draw_rect(layer, & rect, & coords);


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

// custom draw widget
static void lv_compat_event(const lv_obj_class_t * class_p, lv_event_t * e) {
    LV_UNUSED(class_p);

    lv_res_t res = lv_obj_event_base(MY_CLASS_SMTR2, e);	// обработчик родительского клвсса

    if (res != LV_RES_OK) return;

    lv_obj_t  * const obj = (lv_obj_t *) lv_event_get_target(e);
	const lv_event_code_t code = lv_event_get_code(e);
    LV_ASSERT_OBJ(obj, MY_CLASS_SMTR2);

    if (LV_EVENT_DRAW_MAIN == code)
    {
		lv_layer_t * const layer = lv_event_get_layer(e);
		lv_compat_t * const cp = (lv_compat_t *) obj;

        lv_area_t coords;
        lv_obj_get_coords(obj, & coords);	// координаты объекта

        dzi_compat_draw_callback(layer, cp->dzpv, NULL);
     }
}

static void lv_smtr_event(const lv_obj_class_t * class_p, lv_event_t * e) {
    LV_UNUSED(class_p);

    lv_obj_t  * const obj = (lv_obj_t *) lv_event_get_target(e);
	const lv_event_code_t code = lv_event_get_code(e);
    LV_ASSERT_OBJ(obj, MY_CLASS_SMTR);

    if (LV_EVENT_DRAW_MAIN_BEGIN == code)
    {
		lv_smtr_t * const smtr = (lv_smtr_t *) obj;

		smtr_proccess();
    }

    lv_res_t res = lv_obj_event_base(MY_CLASS_SMTR, e);	// обработчик родительского клвсса
    if (res != LV_RES_OK) return;
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
        lv_obj_get_coords(obj, & coords);	// координаты объекта

        lv_sscp2_draw(layer, & coords);
     }
}

typedef struct
{
	lv_image_t img;
	//char text [32];
} lv_wtrf_t;

static const lv_obj_class_t lv_wtrf_class  = {
    .constructor_cb = lv_wtrf_constructor,
//    .destructor_cb = lv_wtrf_destructor,
    .event_cb = lv_wtrf_event,
    .base_class = & lv_image_class,
    .instance_size = sizeof (lv_wtrf_t),
    .name = "hmr_wtrf",
};

lv_obj_t * lv_wtrf_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_WTRF, parent);
    lv_obj_class_init_obj(obj);

	return obj;
}

static void lv_wtrf_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_wtrf_t * const cp = (lv_wtrf_t *) obj;

#if WITHLVGL && WITHSPECTRUMWF
	lv_image_set_src(obj, wfl_get_draw_buff());	// src_type=LV_IMAGE_SRC_VARIABLE
#endif /* WITHLVGL && WITHSPECTRUMWF */

	LV_TRACE_OBJ_CREATE("finished");
}

// custom draw widget
static void lv_wtrf_event(const lv_obj_class_t * class_p, lv_event_t * e) {
    LV_UNUSED(class_p);

    lv_res_t res = lv_obj_event_base(MY_CLASS_WTRF, e);	// обработчик родительского клвсса

    if (res != LV_RES_OK) return;

    lv_obj_t  * const obj = (lv_obj_t *) lv_event_get_target(e);
	const lv_event_code_t code = lv_event_get_code(e);
    LV_ASSERT_OBJ(obj, MY_CLASS_WTRF);

    if (LV_EVENT_DRAW_MAIN_BEGIN == code)
    {
		lv_layer_t * const layer = lv_event_get_layer(e);
		lv_wtrf_t * const smtr = (lv_wtrf_t *) obj;

		wfl_proccess();	// обновить в ранее назначенном буфере изображение
      }
}


#endif /* WITHLVGL */

