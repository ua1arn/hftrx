#ifndef GUI_SYSTEM_H_INCLUDED
#define GUI_SYSTEM_H_INCLUDED

#include "hardware.h"

#if WITHTOUCHGUI
#include "gui_structs.h"

#define GUI_MEM_ASSERT(v) do { if (((v) == NULL)) { \
		PRINTF(PSTR("%s: %d ('%s') - memory allocate failed!\n"), __FILE__, __LINE__, (# v)); \
		for (;;) ; \
		} } while (0)

void * find_gui_obj(obj_type_t type, window_t * win, const char * name);
void objects_state (window_t * win);
void remove_end_line_spaces(char * str);
const char * remove_start_line_spaces(const char * str);
uint_fast8_t get_parent_window(void);
void set_parent_window(uint8_t p);
void get_gui_tracking(int_fast16_t * x, int_fast16_t * y);

uint_fast8_t put_to_wm_queue(window_t * win, wm_message_t message, ...);
wm_message_t get_from_wm_queue(uint8_t win_id, uint8_t * type, int8_t * action, char * name);
void dump_queue(window_t * win);

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
