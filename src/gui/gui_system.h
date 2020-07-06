#ifndef GUI_SYSTEM_H_INCLUDED
#define GUI_SYSTEM_H_INCLUDED

#include "hardware.h"

#if WITHTOUCHGUI
#include "src/gui/gui_structs.h"

/* Возврат ссылки на запись в структуре по названию и типу окна */
void * find_gui_element(element_type_t type, window_t * win, const char * name);

/* Получение ширины метки в пикселях  */
uint_fast8_t get_label_width(const label_t * const lh);

/* Получение высоты метки в пикселях  */
uint_fast8_t get_label_height(const label_t * const lh);

/* Установки статуса основных кнопок */
/* При DISABLED в качестве необязательного параметра передать name активной кнопки или "" для блокирования всех */
void footer_buttons_state (uint_fast8_t state, ...);

/* Установка признака видимости окна */
void close_window(uint_fast8_t parent);
void open_window(window_t * win);
void close_all_windows(void);

/* Расчет экранных координат окна */
void calculate_window_position(window_t * win, uint16_t xmax, uint16_t ymax);

/* Установка статуса элементов после инициализации */
void elements_state (window_t * win);

/* Удаление пробелов в конце строки */
void remove_end_line_spaces(char * str);

/* Сброс данных трекинга тачскрина */
void reset_tracking(void);

/* Возврат признака нажатия кнопки*/
uint_fast8_t is_short_pressed(void);
uint_fast8_t is_long_pressed(void);

uint_fast8_t is_moving_label(void);
uint_fast8_t is_moving_slider(void);
void * get_selected_element(void);
uint_fast8_t check_for_parent_window(void);
uint_fast8_t get_gui_1sec_timer(void);
uint_fast8_t get_selected_element_pos(void);
uint_fast8_t get_gui_keyb_code(void);
void get_gui_tracking(int_fast8_t * x, int_fast8_t * y);

#endif /* WITHTOUCHGUI */
#endif /* GUI_USER_H_INCLUDED */
