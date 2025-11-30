#ifndef GUI_OBJECTS_H_INCLUDED
#define GUI_OBJECTS_H_INCLUDED

#include "../gui_port_include.h"

#if WITHTOUCHGUI

#include "gui_structs.h"

uint16_t get_label_width(const label_t * const lh);
uint16_t get_label_height(const label_t * const lh);
uint16_t get_label_height2(const char * name);
uint16_t get_label_width2(const char * name);

void textfield_update_size(text_field_t * tf);
void textfield_add_string(const char * name, const char * str, gui_color_t color);
void textfield_add_string_old(text_field_t * tf, const char * str, gui_color_t color);
void textfield_clean(const char * name);

uint8_t gui_obj_create(const char * obj_name, ...);
void gui_obj_align_to(const char * name1, const char * name2, object_alignment_t align, uint16_t offset);
void gui_obj_set_prop(const char * name, object_prop_t prop, ...);
int gui_obj_get_int_prop(const char * name, object_prop_t prop);
char * gui_obj_get_string_prop(const char * name, object_prop_t prop);
uint8_t gui_check_obj(const char * name1, const char * name2);
void gui_arrange_objects(const char names[][NAME_ARRAY_SIZE], uint8_t count, uint8_t cols, uint8_t interval);
void gui_arrange_objects_from(const char * name, uint8_t count, uint8_t cols, uint8_t interval);
char * get_obj_name_by_idx(obj_type_t type, uint8_t idx);

#endif /* WITHTOUCHGUI */
#endif /* GUI_OBJECTS_H_INCLUDED */
