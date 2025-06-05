#ifndef STYLES_H_INCLUDED
#define STYLES_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl/lvgl.h"

void styles_init(void);	// инициализация стилей, используемых объектами главного окна
void lvgl_test(void);	// создание элементов на главном окне
void lvgl_dev_init(void);
lv_color_t display_lvlcolor(COLORPIP_T c);	// преобразование цвета в тип LVGL

lv_draw_buf_t * smtr_get_draw_buff(void);	// подготовка lv_draw_buf_t с изображением s-метра
void smtr_proccess(void);	/* Обновить содержимое lv_draw_buf_t - s-meter */

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

#ifdef __cplusplus
}
#endif

#endif /* STYLES_H_INCLUDED */
