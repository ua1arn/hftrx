#ifndef GUI_USER_H_INCLUDED
#define GUI_USER_H_INCLUDED

#include "hardware.h"

#if WITHTOUCHGUI
#include "src/gui/gui_structs.h"

typedef struct {
	int16_t rotate;			// признак поворота второго энкодера
	uint8_t press;			// короткое нажание
	uint8_t hold;			// длинное нажатие
	uint8_t rotate_done;	// событие поворота от энкодера обработано, можно получать новые данные
	uint8_t press_done;		// событие нажатия от энкодера обработано, можно получать новые данные
} enc2_t;

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
