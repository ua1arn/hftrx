/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Copyright (c) 2022-2024 Belousov Oleg aka R1CBU
 */

/*********************
 *      INCLUDES
 *********************/

#include "hardware.h"
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
#define MY_CLASS_TXRX (& lv_txrx_class)
#define MY_CLASS_WTFL (& lv_wtfl_class)

/**********************
 *  STATIC PROTOTYPES
 **********************/
 
static void lv_smtr_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
//static void lv_smtr_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_smtr_event(const lv_obj_class_t * class_p, lv_event_t * e);

static void lv_txrx_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
//static void lv_txrx_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_txrx_event(const lv_obj_class_t * class_p, lv_event_t * e);

static void lv_wtfl_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
//static void lv_wtfl_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
//static void lv_wtfl_event(const lv_obj_class_t * class_p, lv_event_t * e);

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
	lv_obj_t obj;
} lv_smtr_t;

typedef struct
{
	lv_label_t label;
	char text [32];
} lv_txrx_t;

typedef struct
{
	lv_image_t img;
	//char text [32];
} lv_wtfl_t;

/**********************
 *  STATIC VARIABLES
 **********************/

static const lv_obj_class_t lv_smtr_class  = {
    .constructor_cb = lv_smtr_constructor,
//    .destructor_cb = lv_smtr_destructor,
//    .event_cb = lv_smtr_event,
    .base_class = & lv_obj_class,
    .instance_size = sizeof (lv_smtr_t),
    .name = "lv_smtr",
};

static const lv_obj_class_t lv_txrx_class  = {
    .constructor_cb = lv_txrx_constructor,
//    .destructor_cb = lv_txrx_destructor,
    .event_cb = lv_txrx_event,
    .base_class = & lv_label_class,
    .instance_size = sizeof (lv_txrx_t),
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .name = "lv_txrx",
};

static const lv_obj_class_t lv_wtfl_class  = {
    .constructor_cb = lv_wtfl_constructor,
//    .destructor_cb = lv_wtfl_destructor,
//    .event_cb = lv_wtfl_event,
    .base_class = & lv_label_class,
    .instance_size = sizeof (lv_wtfl_t),
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .name = "lv_wtfl",
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_smtr_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_SMTR, parent);
    lv_obj_class_init_obj(obj);

	return obj;
}



lv_obj_t * lv_wtrf_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_WTFL, parent);
    lv_obj_class_init_obj(obj);

	return obj;
}
static void value_changed_event_cb(lv_event_t * e)
{
	TP();
}

lv_obj_t * lv_txrx_create(lv_obj_t * parent) {
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS_TXRX, parent);
    lv_obj_class_init_obj(obj);

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

static void lv_wtfl_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_wtfl_t * const cp = (lv_wtfl_t *) obj;


#if WITHLVGL && WITHSPECTRUMWF
	lv_image_set_src(obj, wfl_get_draw_buff());	// src_type=LV_IMAGE_SRC_VARIABLE

#endif /* WITHLVGL && WITHSPECTRUMWF */

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

//static void lv_smtr_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
//{
//    LV_UNUSED(class_p);
//    lv_smtr_t * smtr = (lv_smtr_t *)obj;
//
//}


static void lv_txrx_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);
    lv_obj_t  * const obj = (lv_obj_t *) lv_event_get_target(e);
	lv_layer_t * const layer = lv_event_get_layer(e);
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

static void lv_smtr_event(const lv_obj_class_t * class_p, lv_event_t * e) {
    LV_UNUSED(class_p);

    lv_res_t res = lv_obj_event_base(MY_CLASS_SMTR, e);	// обработчик родительского клвсса

    if (res != LV_RES_OK) return;

    lv_obj_t  * const obj = (lv_obj_t *) lv_event_get_target(e);
	lv_layer_t * const layer = lv_event_get_layer(e);
	const lv_event_code_t code = lv_event_get_code(e);
    LV_ASSERT_OBJ(obj, MY_CLASS_SMTR);

    if (code == LV_EVENT_DRAW_MAIN_END)
    {
        lv_smtr_t   * const smtr = (lv_smtr_t *) obj;
        lv_draw_buf_t * const smeterdesc = smtr_get_draw_buff();	// изображение s-метра

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

    	lv_draw_rect(layer, & rect, & coords);

//        {
//        	lv_draw_image_dsc_t img;
//        	lv_draw_image_dsc_init(& img);
//
//        	lv_area_t srcrect;
//        	lv_area_set_pos(& srcrect, 0, 0);
//        	lv_area_set_width(& srcrect, smeterdesc->header.w);
//        	lv_area_set_height(& srcrect, smeterdesc->header.h);
//        	lv_draw_image(layer, smeterdesc, & srcrect);
//        }

		{
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


//        lv_draw_line_dsc_t  main_line_dsc;
//        lv_draw_line_dsc_t  peak_line_dsc;
//
//        /* Lines */
//
//        lv_draw_line_dsc_init(&main_line_dsc);
//        lv_obj_init_draw_line_dsc(obj, LV_PART_INDICATOR, &main_line_dsc);
//
//        if (smtr->peak_on) {
//            lv_draw_line_dsc_init(&peak_line_dsc);
//            lv_obj_init_draw_line_dsc(obj, LV_PART_TICKS, &peak_line_dsc);
//        }
//
//        lv_coord_t x1 = obj->coords.x1;
//        lv_coord_t y1 = obj->coords.y1;
//
//        lv_coord_t w = lv_obj_get_width(obj);
//        lv_coord_t h = lv_obj_get_height(obj);
//
//        lv_point_t main_a, main_b;
//        lv_point_t peak_a, peak_b;
//
//        if (!smtr->filled) {
//            main_b.x = x1;
//            main_b.y = y1 + h;
//        }
//
//        peak_b.x = x1;
//        peak_b.y = y1 + h;
//
//        float range = smtr->max - smtr->min;
//
//        for (uint16_t i = 0; i < smtr->data_size; i++) {
//            float       v = (smtr->data_buf[i] - smtr->min) / range;
//            int32_t     x = i * w / smtr->data_size;
//
//            /* Peak */
//
//            if (smtr->peak_on) {
//                float v_peak = (smtr->peak_buf[i].val - smtr->min) / range;
//
//                peak_a.x = x1 + x;
//                peak_a.y = y1 + (1.0f - v_peak) * h;
//
//                lv_draw_line(draw_ctx, &peak_line_dsc, &peak_a, &peak_b);
//
//                peak_b = peak_a;
//            }
//
//            /* Main */
//
//            main_a.x = x1 + x;
//            main_a.y = y1 + (1.0f - v) * h;
//
//            if (smtr->filled) {
//                main_b.x = main_a.x;
//                main_b.y = y1 + h;
//            }
//
//            lv_draw_line(draw_ctx, &main_line_dsc, &main_a, &main_b);
//
//            if (!smtr->filled) {
//                main_b = main_a;
//            }
//        }
    }
}
