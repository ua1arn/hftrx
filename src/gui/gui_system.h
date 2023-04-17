#ifndef GUI_SYSTEM_H_INCLUDED
#define GUI_SYSTEM_H_INCLUDED

#include "hardware.h"

#if WITHTOUCHGUI
#include "gui_structs.h"

#define GUI_MEM_ASSERT(v) do { if (((v) == NULL)) { \
		PRINTF(PSTR("%s: %d ('%s') - memory allocate failed!\n"), __FILE__, __LINE__, (# v)); \
		for (;;) ; \
		} } while (0)

void add_element(const char * element_name, ...);
void * find_gui_element(element_type_t type, window_t * win, const char * name);
uint_fast8_t get_label_width(const label_t * const lh);
uint_fast8_t get_label_height(const label_t * const lh);
void footer_buttons_state (uint_fast8_t state, ...);
void close_window(uint_fast8_t parent);
void open_window(window_t * win);
void close_all_windows(void);
void calculate_window_position(window_t * win, uint_fast8_t mode, ...);
void elements_state (window_t * win);
void remove_end_line_spaces(char * str);
const char * remove_start_line_spaces(const char * str);
void reset_tracking(void);
uint_fast8_t check_for_parent_window(void);
void get_gui_tracking(int_fast16_t * x, int_fast16_t * y);
void textfield_add_string(text_field_t * tf, const char * str, COLORPIP_T color);
void textfield_update_size(text_field_t * tf);
void textfield_clean(text_field_t * tf);
void window_set_title_align(window_t * win, title_align_t align);
void enable_window_move(window_t * win);

uint_fast8_t put_to_wm_queue(window_t * win, wm_message_t message, ...);
wm_message_t get_from_wm_queue(window_t * win, uint_fast8_t * type, uintptr_t * ptr, int_fast8_t * action);

uint_fast16_t gui_get_window_draw_width(void);
uint_fast16_t gui_get_window_draw_height(void);
void gui_drawstring(uint_fast16_t x, uint_fast16_t y, const char * str, font_size_t font, COLORPIP_T color);
void gui_drawline(uint_fast16_t x1, uint_fast16_t y1, uint_fast16_t x2, uint_fast16_t y2, COLORPIP_T color);
void gui_drawpoint(uint_fast16_t x1, uint_fast16_t y1, COLORPIP_T color);

#endif /* WITHTOUCHGUI */
#endif /* GUI_USER_H_INCLUDED */
