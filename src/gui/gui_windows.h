#ifndef GUI_WINDOWS_H
#define GUI_WINDOWS_H

#include "hardware.h"

#if WITHTOUCHGUI

#include "src/gui/gui_structs.h"
#include "window_list.h"

window_t * get_win(uint8_t window_id);

typedef enum {
    #define X(window_id, parent_id, align_mode, title, is_close, onVisibleProcess) WINDOW_##window_id,
    WINDOW_LIST(X)
    #undef X
    WINDOWS_COUNT
} window_index_t;

#endif /* WITHTOUCHGUI */
#endif /* GUI_WINDOWS_H */
