#ifndef GUI_SYSTEM_H_INCLUDED
#define GUI_SYSTEM_H_INCLUDED

#include "hardware.h"

#if WITHTOUCHGUI
#include "gui_structs.h"

#define GUI_MEM_ASSERT(v) do { if (((v) == NULL)) { \
		PRINTF(PSTR("%s: %d ('%s') - memory allocate failed!\n"), __FILE__, __LINE__, (# v)); \
		for (;;) ; \
		} } while (0)

void gui_obj_create(const char * obj_name, ...);
void * find_gui_obj(obj_type_t type, window_t * win, const char * name);
uint_fast8_t get_label_width(const label_t * const lh);
uint_fast8_t get_label_height(const label_t * const lh);
uint_fast8_t get_label_width2(const char * name);
uint_fast8_t get_label_height2(const char * name);
void label_set_coords(const char * name, uint16_t x, uint16_t y);
void label_set_visible(const char * name, uint8_t v);
void label_set_text(const char * name, const char * text);
void label_set_pos_x(const char * name, uint16_t x);
void label_set_pos_y(const char * name, uint16_t y);
uint16_t label_get_pos_x(const char * name);
uint16_t label_get_pos_y(const char * name);
void objects_state (window_t * win);
void remove_end_line_spaces(char * str);
const char * remove_start_line_spaces(const char * str);
uint_fast8_t get_parent_window(void);
void set_parent_window(uint8_t p);
void get_gui_tracking(int_fast16_t * x, int_fast16_t * y);
void textfield_add_string(text_field_t * tf, const char * str, COLORPIP_T color);
void textfield_update_size(text_field_t * tf);
void textfield_clean(text_field_t * tf);

uint_fast8_t put_to_wm_queue(window_t * win, wm_message_t message, ...);
wm_message_t get_from_wm_queue(window_t * win, uint_fast8_t * type, uintptr_t * ptr, int_fast8_t * action);
void dump_queue(window_t * win);
gui_t * get_gui_ptr(void);

uint_fast16_t gui_get_window_draw_width(void);
uint_fast16_t gui_get_window_draw_height(void);
void gui_drawstring(uint_fast16_t x, uint_fast16_t y, const char * str, font_size_t font, COLORPIP_T color);
void gui_drawline(uint_fast16_t x1, uint_fast16_t y1, uint_fast16_t x2, uint_fast16_t y2, COLORPIP_T color);
void gui_drawpoint(uint_fast16_t x1, uint_fast16_t y1, COLORPIP_T color);
void gui_drawrect(uint_fast16_t x1, uint_fast16_t y1, uint_fast16_t x2, uint_fast16_t y2, COLORPIP_T color, uint_fast8_t fill);
void gui_print_UB(uint16_t x, uint16_t y, const char * text, const UB_Font * font, COLORPIP_T color);
const gxdrawb_t * gui_get_drawbuf(void);

#endif /* WITHTOUCHGUI */
#endif /* GUI_USER_H_INCLUDED */
