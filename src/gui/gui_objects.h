#ifndef GUI_OBJECTS_H_INCLUDED
#define GUI_OBJECTS_H_INCLUDED

#include "hardware.h"

#if WITHTOUCHGUI

#include "gui_structs.h"

uint16_t get_label_width(const label_t * const lh);
uint16_t get_label_height(const label_t * const lh);
uint16_t get_label_height2(const char * name);
uint16_t get_label_width2(const char * name);

void textfield_update_size(text_field_t * tf);
void textfield_add_string(text_field_t * tf, const char * str, COLORPIP_T color);
void textfield_clean(text_field_t * tf);

void gui_obj_align_to(const char * name1, const char * name2, object_alignment_t align, uint16_t offset);
void gui_obj_set_prop(const char * name, object_prop_t prop, ...);

#endif /* WITHTOUCHGUI */
#endif /* GUI_OBJECTS_H_INCLUDED */
