#ifndef GUI_SETTINGS_H_INCLUDED
#define GUI_SETTINGS_H_INCLUDED

#include "hardware.h"
#include "src/gui/gui.h"

#if WITHTOUCHGUI

#define GUI_WINDOWBGCOLOR			2
#define GUI_SLIDERLAYOUTCOLOR		100
#define GUI_MENUSELECTCOLOR			109
#define GUI_DEFAULTCOLOR			COLORMAIN_BLACK

#if CTLSTYLE_V3D
	#define GUI_SOFTWARE_RENDERING		1 			// Не использовать DMA при отрисовке
	#define GUI_OLDBUTTONSTYLE			1
#else
	#define GUI_TRANSPARENT_WINDOWS		1			// Прозрачный фон окон
	#define GUI_SOFTWARE_RENDERING		0
#endif

enum {
	sliders_width = 8,			// ширина шкалы слайдера
	sliders_w = 12,				// размеры ползунка слайдера
	sliders_h = 18,				// от центра (*2)
	button_round_radius = 5,	// радиус закругления кнопки
	window_title_height = 26,	// высота области заголовка окна
	window_close_button_size = window_title_height,
	window_title_indent = 20,	// горизонтальный отступ заголовка
	footer_buttons_count = 9,
	touch_area_enlarge = 5,		// увеличение области вокруг элементов для упрощения попадания по мелким элементам
};

#endif /* WITHTOUCHGUI */
#endif /* GUI_STRUCTS_H_INCLUDED */
