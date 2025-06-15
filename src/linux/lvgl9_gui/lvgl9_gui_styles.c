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

lv_style_t mainst, fbtnst, fbtnlockst, fcontst, flbl, winst;

void init_gui_styles(void)
{
	lv_style_init(& mainst);
	lv_style_set_width(& mainst, DIM_X);
	lv_style_set_height(& mainst, DIM_Y);

	lv_style_init(& fbtnst);
	lv_style_set_bg_color(& fbtnst, lv_palette_main(LV_PALETTE_LIGHT_GREEN));
	lv_style_set_border_width(& fbtnst, 2);
	lv_style_set_border_color(& fbtnst, lv_color_black());
	lv_style_set_radius(& fbtnst, 5);
	lv_style_set_bg_opa(& fbtnst, LV_OPA_COVER);
	lv_style_set_pad_all(& fbtnst, 0);

	lv_style_init(& fbtnlockst);
	lv_style_set_bg_color(& fbtnlockst, lv_palette_main(LV_PALETTE_ORANGE));
	lv_style_set_border_width(& fbtnlockst, 2);
	lv_style_set_border_color(& fbtnlockst, lv_color_black());
	lv_style_set_radius(& fbtnlockst, 5);
	lv_style_set_bg_opa(& fbtnlockst, LV_OPA_COVER);
	lv_style_set_pad_all(& fbtnlockst, 0);

	lv_style_init(& flbl);
	lv_style_set_text_color(& flbl, lv_color_black());
	lv_style_set_align(& flbl, LV_ALIGN_CENTER);

	lv_style_init(& fcontst);
	lv_style_set_bg_color(& fcontst, lv_color_black());
	lv_style_set_border_width(& fcontst, 0);
	lv_style_set_pad_column(& fcontst, 3);
	lv_style_set_pad_all(& fcontst, 0);

	lv_style_init(& winst);
	lv_style_set_align(& winst, LV_ALIGN_CENTER);
	lv_style_set_bg_opa(& winst, LV_OPA_30);
}


#endif /* WITHLVGL && LINUX_SUBSYSTEM */
