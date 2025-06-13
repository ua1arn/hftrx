#ifndef WINDOWS_H
#define WINDOWS_H

#include "hardware.h"

#if WITHLVGL && LINUX_SUBSYSTEM

#include <linux/lvgl9_gui/lvgl9_gui.h>

#ifdef __cplusplus
extern "C" {
#endif

void win_modes_handler(lv_event_t * e);
void win_test_handler(lv_event_t * e);


typedef void (* handler_t)(lv_event_t * e);

#define WINDOW_LIST(X) \
    X(MODES, "win_modes", "Modes", win_modes_handler)      \
    X(TEST,  "win_test",  "Test",  win_test_handler)

typedef enum {
    #define X(name, cname, title, h) WIN_##name,
    WINDOW_LIST(X)
    #undef X
    WINDOW_COUNT
} window_index_t;

typedef struct {
    const char name[30];
    const char title[30];
    handler_t h;
} window_t;

static const window_t windows[] = {
    #define X(name, cname, title, h) {cname, title, h},
    WINDOW_LIST(X)
    #undef X
};


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* WITHLVGL && LINUX_SUBSYSTEM */

#endif /*WINDOWS_H*/
