/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN

#include "hardware.h"
#include "formats.h"

#if WITHLVGL && LINUX_SUBSYSTEM

#include "lvgl9_gui.h"

lv_style_t mainst, btnst, fcontst, lblst, winlblst, winst;

void init_gui_styles(void)
{
	lv_style_init(& mainst);
	lv_style_set_width(& mainst, DIM_X);
	lv_style_set_height(& mainst, DIM_Y);

	lv_style_init(& btnst);
	lv_style_set_bg_color(& btnst, lv_palette_main(LV_PALETTE_LIGHT_GREEN));
	lv_style_set_border_width(& btnst, 2);
	lv_style_set_border_color(& btnst, lv_color_black());
	lv_style_set_radius(& btnst, 5);
	lv_style_set_bg_opa(& btnst, LV_OPA_COVER);
	lv_style_set_pad_all(& btnst, 0);

	lv_style_init(& lblst);
	lv_style_set_text_color(& lblst, lv_color_black());
	lv_style_set_align(& lblst, LV_ALIGN_CENTER);

	lv_style_init(& winlblst);
	lv_style_set_text_color(& winlblst, lv_color_white());
	lv_style_set_align(& winlblst, LV_ALIGN_CENTER);

	lv_style_init(& fcontst);
	lv_style_set_bg_color(& fcontst, lv_color_black());
	lv_style_set_border_width(& fcontst, 0);
	lv_style_set_pad_column(& fcontst, 3);
	lv_style_set_pad_all(& fcontst, 0);

	lv_style_init(& winst);
	lv_style_set_align(& winst, LV_ALIGN_CENTER);
	lv_style_set_bg_opa(& winst, LV_OPA_80);
	lv_style_set_bg_color(& winst, lv_palette_main(LV_PALETTE_BROWN));
}


#endif /* WITHLVGL && LINUX_SUBSYSTEM */
