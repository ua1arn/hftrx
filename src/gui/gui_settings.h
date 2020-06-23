#ifndef GUI_SETTINGS_H_INCLUDED
#define GUI_SETTINGS_H_INCLUDED

#include "hardware.h"

#if WITHTOUCHGUI

#define GUI_WINDOWBGCOLOR			2
#define GUI_SLIDERLAYOUTCOLOR		100
#define GUI_MENUSELECTCOLOR			109
#define GUI_TRANSPARENT_WINDOWS		1		/* Прозрачный фон окон */
//#define GUI_OLDBUTTONSTYLE			1   	/* Кнопки без закругления углов */

enum {
	sliders_width = 8,			// ширина шкалы слайдера
	sliders_w = 12,				// размеры ползунка слайдера
	sliders_h = 18,				// от центра (*2)
	button_round_radius = 5,	// радиус закругления кнопки
	window_title_height = 26,	// высота области заголовка окна
	footer_buttons_count = 9,
};

typedef enum {
	WINDOW_MAIN,					// постоянно отображаемые кнопки внизу экрана
	WINDOW_MODES,					// переключение режимов работы, видов модуляции
	WINDOW_BP,						// регулировка полосы пропускания фильтров выбранного режима
	WINDOW_AGC,						// выбор пресетов настроек АРУ для текущего режима модуляции
	WINDOW_FREQ,					// прямой ввод частоты
	WINDOW_MENU,					// системное меню
	WINDOW_ENC2,					// быстрое меню по нажатию 2-го энкодера
	WINDOW_UIF,						// быстрое меню по нажатию заранее определенных кнопок
	WINDOW_SWR_SCANNER,				// сканер КСВ по диапазону
	WINDOW_AUDIOSETTINGS,			// настройки аудиопараметров
	WINDOW_AP_MIC_EQ,				// эквалайзер микрофона
	WINDOW_AP_REVERB_SETT,			// параметры ревербератора
	WINDOW_AP_MIC_SETT,				// настройки микрофона
	WINDOW_AP_MIC_PROF,				// профили микрофона (заготовка окна)
	WINDOW_TX_SETTINGS,				// настройки, относящиеся к режиму передачи
	WINDOW_TX_VOX_SETT,				// настройки VOX
	WINDOW_TX_POWER,				// выходная мощность
	WINDOW_OPTIONS,
	WINDOW_UTILS,
	WINDOWS_COUNT
} window_id_t;


#if defined (GUI_OLDBUTTONSTYLE)
#define GUI_DEFAULTCOLOR		COLORMAIN_BLACK
#else
#define GUI_DEFAULTCOLOR		255
#endif

#endif /* WITHTOUCHGUI */
#endif /* GUI_STRUCTS_H_INCLUDED */
