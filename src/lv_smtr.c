/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Copyright (c) 2022-2024 Belousov Oleg aka R1CBU
 */

/*********************
 *      INCLUDES
 *********************/

#include <stdlib.h>
#include "lv_smtr.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_smtr_class

/**********************
 *  STATIC PROTOTYPES
 **********************/
 
static void lv_smtr_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_smtr_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_smtr_event(const lv_obj_class_t * class_p, lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_smtr_class  = {
    .constructor_cb = lv_smtr_constructor,
    .destructor_cb = lv_smtr_destructor,
    .event_cb = lv_smtr_event,
    .base_class = &lv_obj_class,
    .instance_size = sizeof(lv_smtr_t),
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_smtr_create(lv_obj_t * parent) {
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);

    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_smtr_set_data_size(lv_obj_t * obj, uint16_t size) {
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_smtr_t * smtr = (lv_smtr_t *)obj;

//    smtr->data_size = size;
//    smtr->data_buf = lv_mem_realloc(smtr->data_buf, size * sizeof(float));
//    smtr->peak_buf = lv_mem_realloc(smtr->peak_buf, size * sizeof(lv_smtr_peak_t));
}

void lv_smtr_clear_data(lv_obj_t * obj) {
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_smtr_t * smtr = (lv_smtr_t *)obj;

    uint32_t now = lv_tick_get();
    
//    for (uint16_t i = 0; i < smtr->data_size; i++) {
//        smtr->peak_buf[i].val = smtr->min;
//        smtr->peak_buf[i].time = now;
//    }
//
//    smtr->delta_surplus = 0;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_smtr_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj) {
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_smtr_t * smtr = (lv_smtr_t *)obj;
//
//    smtr->filled = false;
//    smtr->peak_on = false;
//    smtr->data_size = 0;
//    smtr->data_buf = NULL;
//    smtr->peak_buf = NULL;
//    smtr->min = -40;
//    smtr->max = 0;
//    smtr->span = 100000;
//    smtr->delta_surplus = 0;

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_smtr_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj) {
    LV_UNUSED(class_p);
    lv_smtr_t * smtr = (lv_smtr_t *)obj;

//    if (smtr->data_buf) lv_mem_free(smtr->data_buf);
//    if (smtr->peak_buf) lv_mem_free(smtr->peak_buf);
}

static void lv_smtr_event(const lv_obj_class_t * class_p, lv_event_t * e) {
    LV_UNUSED(class_p);

    lv_res_t res = lv_obj_event_base(MY_CLASS, e);	// обраьотчик родительского клвсса

    if (res != LV_RES_OK) return;

    lv_obj_t  * const obj = (lv_obj_t *) lv_event_get_target(e);
	lv_layer_t * const layer = lv_event_get_layer(e);
	const lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_DRAW_MAIN_END)
    {
        lv_smtr_t   *smtr = (lv_smtr_t *) obj;

        lv_area_t coords;
        lv_obj_get_coords(obj, & coords);	// координаты объекта

//		lv_coord_t w = lv_obj_get_width(obj);
//		lv_coord_t h = lv_obj_get_height(obj);
		{
            lv_draw_line_dsc_t dsc;
            lv_draw_line_dsc_init(& dsc);
            dsc.color = lv_palette_main(LV_PALETTE_RED);
            dsc.width = 4;
            dsc.round_end = 1;
            dsc.round_start = 1;
            dsc.p1.x = coords.x1;
            dsc.p1.y = coords.y1;
            dsc.p2.x = coords.x2;
            dsc.p2.y = coords.y2;
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
