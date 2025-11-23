#ifndef GUI_SETTINGS_H_INCLUDED
#define GUI_SETTINGS_H_INCLUDED

#include "src/gui/gui_port_include.h"

#if WITHTOUCHGUI

#include "src/gui/gui.h"

#if LCDMODE_MAIN_L8
	// Indexed, one byte colors
	#define GUI_WINDOWTITLECOLOR		20
	#define GUI_WINDOWBGCOLOR			2
	#define GUI_SLIDERLAYOUTCOLOR		100
	#define GUI_MENUSELECTCOLOR			109
	#define GUI_DEFAULTCOLOR			COLORPIP_BLACK
	#define COLOR_BUTTON_NON_LOCKED		COLORPIP_GREEN
	#define COLOR_BUTTON_PR_NON_LOCKED	COLORPIP_DARKGREEN	// was: COLORPIP_DARKGREEN2
	#define COLOR_BUTTON_LOCKED			COLORPIP_YELLOW
	#define COLOR_BUTTON_PR_LOCKED		DSGN_LOCKED // TFTRGB(0x3C, 0x3C, 0x00)
	#define COLOR_BUTTON_DISABLED		COLORPIP_GRAY // TFTRGB(0x50, 0x50, 0x50) FIXME: use right value

#elif 0
	// Can be used TFTRGB(r, g, b) macro
	// Experemental

	#define GUI_WINDOWTITLECOLOR		COLOR_SKYBLUE
	#define GUI_WINDOWBGCOLOR			COLOR_DARKGRAY
	#define GUI_SLIDERLAYOUTCOLOR		COLOR_GREEN
	#define GUI_MENUSELECTCOLOR			COLOR_GREEN
	#define GUI_DEFAULTCOLOR			COLOR_BLACK
    #define COLOR_BUTTON_NON_LOCKED        COLOR_WHITE
    #define COLOR_BUTTON_PR_NON_LOCKED    COLOR_WHITE
	#define COLOR_BUTTON_LOCKED			COLOR_YELLOW
	#define COLOR_BUTTON_PR_LOCKED		DSGN_LOCKED
	#define COLOR_BUTTON_DISABLED		COLOR_GRAY

#else /* LCDMODE_MAIN_L8 */
	// Can be used TFTRGB(r, g, b) macro
	#define GUI_WINDOWTITLECOLOR		COLOR_SKYBLUE
	#define GUI_WINDOWBGCOLOR			COLOR_DARKGRAY
	#define GUI_SLIDERLAYOUTCOLOR		COLOR_GREEN
	#define GUI_MENUSELECTCOLOR			COLOR_GREEN
	#define GUI_DEFAULTCOLOR			COLOR_BLACK
	#define COLOR_BUTTON_NON_LOCKED		COLOR_GREEN
	#define COLOR_BUTTON_PR_NON_LOCKED	COLOR_DARKGREEN
	#define COLOR_BUTTON_LOCKED			COLOR_YELLOW
	#define COLOR_BUTTON_PR_LOCKED		DSGN_LOCKED
	#define COLOR_BUTTON_DISABLED		COLOR_GRAY

#endif /* LCDMODE_MAIN_L8 */

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
	footer_buttons_count = 9
};

#if 1 //WITHALTERNATIVEFONTS
	#define FONT_BUTTONS	arial_14x13_prop
#endif /* WITHALTERNATIVEFONTS */

#endif /* WITHTOUCHGUI */

#endif /* GUI_STRUCTS_H_INCLUDED */
