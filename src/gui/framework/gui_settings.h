#ifndef GUI_SETTINGS_H_INCLUDED
#define GUI_SETTINGS_H_INCLUDED

#include "src/gui/gui_port_include.h"

#if WITHTOUCHGUI

#include "gui.h"

enum {
	sliders_scale_thickness = 8,// ширина шкалы слайдера
	sliders_w = 12,				// размеры ползунка слайдера
	sliders_h = 18,				// от центра (*2)
	button_round_radius = 3,	// радиус закругления кнопки
	window_title_height = 26,	// высота области заголовка окна
	edge_step = 15,				// отступ от границ окна
	window_close_button_size = window_title_height,
	window_title_indent = 20,	// горизонтальный отступ заголовка
	touch_area_enlarge = 5,		// увеличение области вокруг элементов для упрощения попадания по мелким элементам
	autorepeat_delay = 4,		// задержка автоповтора действий
	footer_buttons_count = 9,
	common_btn_interval = 3,
	common_btn_width = WITHGUIMAXX / 9 + 1 - common_btn_interval,
	common_btn_height = FOOTER_HEIGHT - 6,
};

#define COMMON_BUTTON_STYLE		common_btn_width, common_btn_height
#define SMALL_BUTTON_STYLE		common_btn_height, common_btn_height
#define LONG_BUTTON_STYLE		(common_btn_width + common_btn_width / 3), common_btn_height

#define BUTTONS_FONTP_DEFAULT	msgothic_15x17_prop
#define LABELS_FONT_DEFAULT		msgothic_15x17_mono
#define INFOBAR_FONTP			msgothic_13x16_prop
#define WINDOW_TITLE_FONTP		msgothic_15x17_prop

#endif /* WITHTOUCHGUI */

#endif /* GUI_STRUCTS_H_INCLUDED */
