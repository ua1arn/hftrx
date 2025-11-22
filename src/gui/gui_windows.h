#ifndef GUI_WINDOWS_H
#define GUI_WINDOWS_H

#include "gui/gui_port_include.h"

#if WITHTOUCHGUI

#include "src/gui/gui_structs.h"

window_t * get_win(uint8_t window_id);
void move_window(window_t * win, int_fast16_t ax, int_fast16_t ay);
void window_set_title_align(title_align_t align);
void window_set_title(const char * text);
void enable_window_move(void);
void close_window(uint_fast8_t parent);
void open_window(window_t * win);
void close_all_windows(void);
void calculate_window_position(uint_fast8_t mode, ...);
uint8_t is_win_init(void);
uint8_t is_winmain_init(void);

typedef enum {
    #define X(window_id, parent_id, align_mode, title, is_close, onVisibleProcess) WINDOW_##window_id,
	#include "window_list.h"
    WINDOW_LIST(X)
    #undef X
	#undef WINDOW_LIST
    WINDOWS_COUNT
} window_index_t;

#endif /* WITHTOUCHGUI */
#endif /* GUI_WINDOWS_H */
