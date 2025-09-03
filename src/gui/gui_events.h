#ifndef GUI_EVENTS_H_INCLUDED
#define GUI_EVENTS_H_INCLUDED

#include "hardware.h"

typedef enum {
	CODE_CURSOR_LEFT,
	CODE_CURSOR_RIGHT,
	CODE_KEY_ENTER,
	CODE_KEY_ESCAPE,
} gui_event_code;

typedef enum {
	EVENT_TYPE_CONTROL,
	EVENT_TYPE_KEY,
} gui_event_type;

void gui_put_event(gui_event_type type, uint16_t code);

#endif /* GUI_EVENTS_H_INCLUDED */
