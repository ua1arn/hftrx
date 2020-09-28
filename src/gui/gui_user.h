#ifndef GUI_USER_H_INCLUDED
#define GUI_USER_H_INCLUDED

#include "hardware.h"

#if WITHTOUCHGUI

#include "src/gui/gui.h"
#include "src/gui/gui_structs.h"

void gui_user_actions_after_close_window(void);

#if WITHGUISTYLE_MINI

enum {
	WINDOW_MAIN,

	WINDOWS_COUNT
};

#elif WITHGUISTYLE_COMMON

enum {
	WINDOW_MAIN,					// постоянно отображаемые кнопки внизу экрана
	WINDOW_MODES,					// переключение режимов работы, видов модуляции
	WINDOW_AF,						// регулировка полосы пропускания фильтров выбранного режима
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
	WINDOW_OPTIONS,					// различные настройки
	WINDOW_UTILS,					// измерения и т.д.
	WINDOW_BANDS,					// выбор диапазона
	WINDOW_MEMORY,					// ячейки памяти
	WINDOW_DISPLAY,					// настройки отображения
	WINDOW_RECEIVE,					// настройки приема

	WINDOWS_COUNT
};

#endif

typedef struct {
	uint8_t press;			// короткое нажание
	uint8_t hold;			// длинное нажатие
	uint8_t press_done;		// событие нажатия от энкодера обработано, можно получать новые данные
} enc2_t;

#define ENC2_STACK_SIZE	10

typedef struct enc2_stack_tag {
    int_fast8_t data[ENC2_STACK_SIZE];
    size_t size;
} enc2_stack_t;

enum {
	MENU_OFF,
	MENU_GROUPS,
	MENU_PARAMS,
	MENU_VALS,
	MENU_COUNT
};

typedef struct {
	uint8_t first_id;			// первое вхождение номера метки уровня
	uint8_t last_id;			// последнее вхождение номера метки уровня
	uint8_t num_rows;			// число меток уровня
	uint8_t count;				// число значений уровня
	int8_t selected_str;		// выбранная строка уровня
	int8_t selected_label;		// выбранная метка уровня
	uint8_t add_id;				// номер строки уровня, отображаемой первой
	menu_names_t menu_block [MENU_ARRAY_SIZE];	// массив значений уровня меню
} menu_t;

typedef struct {
	char name [TEXT_ARRAY_SIZE];
	uint16_t menupos;
	uint8_t exitkey;
} menu_by_name_t;

typedef enum {
	TYPE_BP_LOW,
	TYPE_BP_HIGH,
	TYPE_IF_SHIFT
} bp_select_t;

typedef struct {
	uint_fast8_t updated;
	bp_select_t select;
	int8_t change;
} bp_var_t;

#endif /* WITHTOUCHGUI */
#endif /* GUI_USER_H_INCLUDED */
