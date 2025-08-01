#ifndef GUI_WINDOWS_H
#define GUI_WINDOWS_H

#include "hardware.h"

#if WITHTOUCHGUI

#include "src/gui/gui_structs.h"

window_t * get_win(uint8_t window_id);
void move_window(window_t * win, int_fast16_t ax, int_fast16_t ay);
void window_set_title_align(window_t * win, title_align_t align);
void enable_window_move(window_t * win);
void close_window(uint_fast8_t parent);
void open_window(window_t * win);
void close_all_windows(void);
void calculate_window_position(window_t * win, uint_fast8_t mode, ...);

typedef enum {
    #define X(window_id, parent_id, align_mode, title, is_close, onVisibleProcess) WINDOW_##window_id,
	#include "window_list.h"
    WINDOW_LIST(X)
    #undef X
	#undef WINDOW_LIST
    WINDOWS_COUNT
} window_index_t;

#undef WINDOW_LIST

#endif /* WITHTOUCHGUI */
#endif /* GUI_WINDOWS_H */
