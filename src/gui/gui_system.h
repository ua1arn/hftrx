#ifndef GUI_SYSTEM_H_INCLUDED
#define GUI_SYSTEM_H_INCLUDED

#include "hardware.h"

#if WITHTOUCHGUI
#include "src/gui/gui_structs.h"

#define GUI_MEM_ASSERT(v) do { if (((v) == NULL)) { \
		PRINTF(PSTR("%s: %d ('%s') - memory allocate failed!\n"), __FILE__, __LINE__, (# v)); \
		for (;;) ; \
		} } while (0)

void * find_gui_element(element_type_t type, window_t * win, const char * name);
uint_fast8_t get_label_width(const label_t * const lh);
uint_fast8_t get_label_height(const label_t * const lh);
void footer_buttons_state (uint_fast8_t state, ...);
void close_window(uint_fast8_t parent);
void open_window(window_t * win);
void open_window_id(uint_fast8_t win_id);
void window_set_parent(uint_fast8_t win_id, uint_fast8_t parent_win_id);
uint_fast8_t win_first_call(uint_fast8_t clear);
void close_all_windows(void);
void calculate_window_position(window_t * win, uint_fast8_t mode, ...);
void calculate_window_position_by_id(uint8_t win_id, uint_fast8_t mode, ...);
void elements_state (window_t * win);
void remove_end_line_spaces(char * str);
const char * remove_start_line_spaces(const char * str);
void reset_tracking(void);
uint_fast8_t check_for_parent_window(void);
void get_gui_tracking(int_fast16_t * x, int_fast16_t * y);
void textfield_add_string(text_field_t * tf, const char * str, COLORMAIN_T color);
void textfield_update_size(text_field_t * tf);
void textfield_clean(text_field_t * tf);
void window_set_title_align(window_t * win, title_align_t align);
void set_property(const uint8_t window_id, const char * name, uint_fast8_t operation, ...);
retval_t get_property(const uint8_t window_id, const char * name, uint_fast8_t operation);
void add_element(const uint8_t window_id, const char * element_name, ...);

uint_fast8_t put_to_wm_queue(window_t * win, wm_message_t message, ...);
wm_message_t get_from_wm_queue(window_t * win, uint_fast8_t * type, uintptr_t * ptr, int_fast8_t * action);
wm_message_t get_from_wm_queue_by_id(uint8_t win_id, uint_fast8_t * type, int_fast8_t * action, char * name);

uint_fast16_t gui_get_window_draw_width(window_t * win);
uint_fast16_t gui_get_window_draw_height(window_t * win);
void gui_drawstring(window_t * win, uint_fast16_t x, uint_fast16_t y, const char * str, font_size_t font, COLORMAIN_T color);
void gui_drawline(window_t * win, uint_fast16_t x1, uint_fast16_t y1, uint_fast16_t x2, uint_fast16_t y2, COLORMAIN_T color);

#endif /* WITHTOUCHGUI */
#endif /* GUI_USER_H_INCLUDED */
