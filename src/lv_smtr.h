/*
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 *
 *  Copyright (c) 2022-2024 Belousov Oleg aka R1CBU
 */

#ifndef LV_smtr_H
#define LV_smtr_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "hardware.h"
#include "lvgl.h"
#include "core/lv_obj_private.h"
#include "core/lv_obj_class_private.h"

/**********************
 *      TYPEDEFS
 **********************/


typedef struct {
    lv_obj_t            obj;
} lv_smtr_t;

extern const lv_obj_class_t lv_smtr_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * lv_smtr_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

void lv_smtr_set_data_size(lv_obj_t * obj, uint16_t size);
void lv_smtr_clear_data(lv_obj_t * obj);
void lv_smtr_scroll_data(lv_obj_t * obj, int32_t df);
void lv_smtr_add_data(lv_obj_t * obj, float * data, size_t size);
void lv_smtr_set_span(lv_obj_t * obj, int32_t hz);
void lv_smtr_set_max(lv_obj_t * obj, int16_t db);
void lv_smtr_set_min(lv_obj_t * obj, int16_t db);
void lv_smtr_set_filled(lv_obj_t * obj, bool on);
void lv_smtr_set_peak(lv_obj_t * obj, bool on);
void lv_smtr_set_peak_hold(lv_obj_t * obj, uint16_t ms);
void lv_smtr_set_peak_speed(lv_obj_t * obj, float db);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
