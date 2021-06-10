#ifndef GUI_SETTINGS_H_INCLUDED
#define GUI_SETTINGS_H_INCLUDED

#include "hardware.h"
#include "src/gui/gui.h"

#if WITHTOUCHGUI

#define GUI_WINDOWBGCOLOR			2
#define GUI_SLIDERLAYOUTCOLOR		100
#define GUI_MENUSELECTCOLOR			109
#define GUI_DEFAULTCOLOR			COLORMAIN_BLACK

#if CPUSTYLE_R7S721
	#define GUI_WINDOWTITLECOLOR		20
#elif CPUSTYLE_XC7Z || CPUSTYLE_STM32MP1
	#define GUI_WINDOWTITLECOLOR		COLORPIP_BLUE
#endif

#if CTLSTYLE_V3D
	#define GUI_SOFTWARE_RENDERING		1 			// Не использовать DMA при отрисовке
#else
	#define GUI_SOFTWARE_RENDERING		0
#endif

enum {
	sliders_width = 8,			// ширина шкалы слайдера
	sliders_w = 12,				// размеры ползунка слайдера
	sliders_h = 18,				// от центра (*2)
	button_round_radius = 5,	// радиус закругления кнопки
	window_title_height = 26,	// высота области заголовка окна
	edge_step = 15,				// отступ от границ окна
	window_close_button_size = window_title_height,
	window_title_indent = 20,	// горизонтальный отступ заголовка
	touch_area_enlarge = 5,		// увеличение области вокруг элементов для упрощения попадания по мелким элементам
};

#endif /* WITHTOUCHGUI */
#endif /* GUI_STRUCTS_H_INCLUDED */
