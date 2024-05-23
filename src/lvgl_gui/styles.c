#include "hardware.h"

#if WITHLVGL

#include "styles.h"

lv_style_t style_mainscreen;
lv_style_t style_footer_button;
lv_style_t style_freq_main;
lv_style_t style_label_btn;
lv_style_t style_window;

void styles_init(void)
{
	lv_style_init(& style_mainscreen);
	lv_style_set_bg_color(& style_mainscreen, lv_palette_main(LV_PALETTE_GREY));
	lv_style_set_text_color(& style_mainscreen, lv_color_white());
	lv_style_set_border_width(& style_mainscreen, 0);
	lv_style_set_pad_all(& style_mainscreen, 0);
	lv_style_set_radius(& style_mainscreen, 0);

	lv_style_init(& style_footer_button);
	lv_style_set_bg_color(& style_footer_button, lv_palette_main(LV_PALETTE_BLUE));
	lv_style_set_border_width(& style_footer_button, 2);
	lv_style_set_border_color(& style_footer_button, lv_color_black());
	lv_style_set_radius(& style_footer_button, 5);
	lv_style_set_size(& style_footer_button, 86);
	lv_style_set_height(& style_footer_button, 44);
	lv_style_set_bg_opa(& style_footer_button, LV_OPA_COVER);
	lv_style_set_pad_all(& style_footer_button, 0);

	lv_style_init(& style_label_btn);
	lv_style_set_text_color(& style_label_btn, lv_color_black());
	lv_style_set_align(& style_label_btn, LV_ALIGN_CENTER);

    lv_style_init(& style_freq_main);
    lv_style_set_text_color(& style_freq_main, lv_color_black());
    lv_style_set_text_font(& style_freq_main, & lv_font_montserrat_38);
    lv_style_set_pad_ver(& style_freq_main, 5);

    lv_style_init(& style_window);
    lv_style_set_align(& style_window, LV_ALIGN_CENTER);
    lv_style_set_bg_opa(& style_window, LV_OPA_50);
    lv_style_set_radius(& style_window, 3);
}

#endif /* WITHLVGL */
