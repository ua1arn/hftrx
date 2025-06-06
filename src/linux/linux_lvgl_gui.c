/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN

#include "hardware.h"
#include "formats.h"
#include "display2.h"

#if WITHLVGL && LINUX_SUBSYSTEM

#include "lvgl.h"

lv_obj_t * main_gui_obj = NULL;

static void event_handler(lv_event_t * e)
{
    lv_obj_t * obj = (lv_obj_t *) lv_event_get_target(e);
    LV_UNUSED(obj);
    LV_LOG_USER("Button %d clicked", (int)lv_obj_get_index(obj));
}

void lvgl_gui_init(lv_obj_t * parent)
{
	TP();
	//main_gui_obj = lv_screen_active();
	main_gui_obj = parent;

    static int32_t col_dsc[] = {70, 70, 70, LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {50, 50, 50, LV_GRID_TEMPLATE_LAST};

    /*Create a container with grid*/
    lv_obj_t * cont = lv_obj_create(main_gui_obj);
    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
    lv_obj_set_size(cont, 300, 220);
    lv_obj_center(cont);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);

    lv_obj_t * label;
    lv_obj_t * obj;

    uint32_t i;
    for(i = 0; i < 9; i++) {
        uint8_t col = i % 3;
        uint8_t row = i / 3;

        obj = lv_button_create(cont);
        lv_obj_add_event_cb(obj, event_handler, LV_EVENT_CLICKED, NULL);
        /*Stretch the cell horizontally and vertically too
         *Set span to 1 to make the cell 1 column/row sized*/
        lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, col, 1,
                             LV_GRID_ALIGN_STRETCH, row, 1);

        label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "c%d, r%d", col, row);
        lv_obj_center(label);
    }

//    static lv_obj_t * win = lv_win_create(main_gui_obj);
//    static lv_obj_t * btn = lv_win_add_button(win, LV_SYMBOL_LEFT, 40);
//    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
//
//    lv_win_add_title(win, "A title");
//
//    btn = lv_win_add_button(win, LV_SYMBOL_RIGHT, 40);
//    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
//
//    btn = lv_win_add_button(win, LV_SYMBOL_CLOSE, 60);
//    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
}

#endif /* WITHLVGL && LINUX_SUBSYSTEM */

