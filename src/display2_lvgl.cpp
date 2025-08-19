/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN

#include "hardware.h"

#if WITHLVGL

#include "formats.h"
#include "radio.h"
#include "display2.h"
#include "lvgl.h"

static const lv_coord_t cols_dsc[4] = { 100, 50, 300, LV_GRID_TEMPLATE_LAST};
static const lv_coord_t rows_dsc[2] = { 30, LV_GRID_TEMPLATE_LAST };

static void parameditor_slider_cb(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code != LV_EVENT_VALUE_CHANGED) return;

	struct paramdefdef * pd = (struct paramdefdef *) lv_event_get_user_data(e);
	lv_obj_t * s = (lv_obj_t *) lv_event_get_target(e);
	lv_obj_t * l = (lv_obj_t *) lv_obj_get_user_data(s);

	int32_t v = lv_slider_get_value(s);
	param_setvalue(pd, v);
	updateboard();

	lv_label_set_text_fmt(l, "%" PRIi32, v);
}

static void parameditor_dropdown_cb(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code != LV_EVENT_VALUE_CHANGED) return;

	struct paramdefdef * pd = (struct paramdefdef *) lv_event_get_user_data(e);
	lv_obj_t * t = (lv_obj_t *) lv_event_get_target(e);

	uint8_t v = lv_dropdown_get_selected(t);
	param_setvalue(pd, v);
	updateboard();
}

static void parameditor_switch_cb(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code != LV_EVENT_VALUE_CHANGED) return;

	struct paramdefdef * pd = (struct paramdefdef *) lv_event_get_user_data(e);
	lv_obj_t * sw = (lv_obj_t *) lv_event_get_target(e);

	uint8_t v = lv_obj_has_state(sw, LV_STATE_CHECKED) ? 1 : 0;
	param_setvalue(pd, v);
	updateboard();
}

lv_obj_t * hamradio_walkmenu_getparameditor(const struct paramdefdef * pd, lv_obj_t * parent)
{
	lv_obj_t * obj = lv_menu_cont_create(parent);
	lv_obj_set_layout(obj, LV_LAYOUT_GRID);
	lv_obj_set_style_grid_column_dsc_array(obj, cols_dsc, 0);
	lv_obj_set_style_grid_row_dsc_array(obj, rows_dsc, 0);

	switch (pd->qrj) {
	case RJ_TXAUDIO:
	case RJ_VIEW:
	case RJ_SMETER: {
		lv_obj_t * label_name = lv_label_create(obj);
		lv_label_set_text_static(label_name, pd->qlabel);
		lv_obj_set_grid_cell(label_name, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);

		lv_obj_t * dd = lv_dropdown_create(obj);
		lv_dropdown_clear_options(dd);

		switch (pd->qrj)
		{
		case RJ_SMETER:
			lv_dropdown_set_options(dd, "BARS\n" "DIAL");
			break;
#if WITHTX && WITHIF4DSP
		case RJ_TXAUDIO:
			for (unsigned int i = 0; i < TXAUDIOSRC_COUNT; i ++)
				lv_dropdown_add_option(dd, txaudiosrcs[i].label, LV_DROPDOWN_POS_LAST);
			break;
#endif /* WITHTX && WITHIF4DSP */
		case RJ_VIEW:
			for (int i = 0; i < VIEW_count; i ++)
				lv_dropdown_add_option(dd, view_types[i], LV_DROPDOWN_POS_LAST);
			break;
		default:
			break;
		}

		lv_dropdown_set_selected(dd, param_getvalue(pd));
		lv_obj_set_grid_cell(dd, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_START, 0, 1);
		lv_obj_add_event_cb(dd, parameditor_dropdown_cb, LV_EVENT_VALUE_CHANGED, (struct paramdefdef *) pd);

		break;
	}
	case RJ_YES:
	case RJ_ON: {
		lv_obj_t * label = lv_label_create(obj);
		lv_label_set_text_static(label, pd->qlabel);
		lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);

		lv_obj_t * sw = lv_switch_create(obj);
		lv_obj_add_state(sw, param_getvalue(pd) ? LV_STATE_CHECKED : LV_STATE_DEFAULT);
		lv_obj_set_grid_cell(sw, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_START, 0, 1);
		lv_obj_add_event_cb(sw, parameditor_switch_cb, LV_EVENT_VALUE_CHANGED, (struct paramdefdef *) pd);

		break;
	}
	default: {
		lv_obj_t * label_name = lv_label_create(obj);
		lv_label_set_text_static(label_name, pd->qlabel);
		lv_obj_set_grid_cell(label_name, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);

		lv_obj_t * label_value = lv_label_create(obj);
		lv_label_set_text_fmt(label_value, "%" PRIiFAST32, param_getvalue(pd));
		lv_obj_set_grid_cell(label_value, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 0, 1);

		lv_obj_t * slider = lv_slider_create(obj);
		lv_slider_set_range(slider, pd->qbottom, pd->qupper);
		lv_slider_set_value(slider, param_getvalue(pd), LV_ANIM_OFF);
		lv_obj_set_user_data(slider, label_value);
		lv_obj_set_grid_cell(slider, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_START, 0, 1);
		lv_obj_add_event_cb(slider, parameditor_slider_cb, LV_EVENT_VALUE_CHANGED, (struct paramdefdef *) pd);

		break;
	}
	}

	return obj;
}

typedef enum {
    LV_MENU_ITEM_BUILDER_VARIANT_1,
    LV_MENU_ITEM_BUILDER_VARIANT_2
} lv_menu_builder_variant_t;

static lv_obj_t * create_text(lv_obj_t * parent, const char * icon, const char * txt,
                              lv_menu_builder_variant_t builder_variant)
{
    lv_obj_t * obj = lv_menu_cont_create(parent);

    lv_obj_t * img = NULL;
    lv_obj_t * label = NULL;

    if(icon) {
        img = lv_image_create(obj);
        lv_image_set_src(img, icon);
    }

    if(txt) {
        label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
    }

    if(builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 && icon && txt) {
        lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_swap(img, label);
    }

    return obj;
}

static lv_obj_t * create_slider(lv_obj_t * parent, const char * icon, const char * txt, int32_t min, int32_t max,
                                int32_t val)
{
    lv_obj_t * obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t * slider = lv_slider_create(obj);
    lv_obj_set_flex_grow(slider, 1);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, val, LV_ANIM_OFF);

    if(icon == NULL) {
        lv_obj_add_flag(slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    }

    return obj;
}

static lv_obj_t * create_switch(lv_obj_t * parent, const char * icon, const char * txt, bool chk)
{
    lv_obj_t * obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_obj_t * sw = lv_switch_create(obj);
    lv_obj_add_state(sw, chk ? LV_STATE_CHECKED : LV_STATE_DEFAULT);

    return obj;
}

static void back_event_handler(lv_event_t * e)
{
    lv_obj_t * const obj = lv_event_get_target_obj(e);
    lv_obj_t * const menu = (lv_obj_t *) lv_event_get_user_data(e);

    if (lv_menu_back_button_is_root(menu, obj)) {
        lv_obj_t * mbox1 = lv_msgbox_create(NULL);
        lv_msgbox_add_title(mbox1, "Hello");
        lv_msgbox_add_text(mbox1, "Root back btn click.");
        lv_msgbox_add_close_button(mbox1);
    }
}

static lv_obj_t * groot_page;

static void switch_handler(lv_event_t * e)
{
	const lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * const menu = (lv_obj_t *) lv_event_get_user_data(e);
    lv_obj_t * const obj = lv_event_get_target_obj(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
    	ASSERT(groot_page);
        if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
            lv_menu_set_page(menu, NULL);
            lv_menu_set_sidebar_page(menu, groot_page);
            lv_obj_send_event(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0), 0), LV_EVENT_CLICKED,
                              NULL);
        }
        else {
            lv_menu_set_sidebar_page(menu, NULL);
            lv_menu_clear_history(menu); /* Clear history because we will be showing the root page later */
            lv_menu_set_page(menu, groot_page);
        }
    }
}

struct menuwalkctx
{
	lv_obj_t * menu;
	lv_obj_t * section_first;
};

static void * dzicreategroup(void * menuwalkctx, const void * groupitem)
{
	char groupname [32];
	hamradio_walkmenu_getgroupanme(groupitem, groupname, ARRAY_SIZE(groupname));
	struct menuwalkctx * ctx = (struct menuwalkctx *) menuwalkctx;

    /*Create sub pages*/
    lv_obj_t * sub_xx_page = lv_menu_page_create(ctx->menu, groupname);
    lv_obj_set_style_pad_hor(sub_xx_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(ctx->menu), LV_PART_MAIN), 0);
    //lv_menu_separator_create(sub_xx_page);
	lv_obj_t * const cont = create_text(ctx->section_first, LV_SYMBOL_SETTINGS, groupname, LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(ctx->menu, cont, sub_xx_page);

	return lv_menu_section_create(sub_xx_page);		// Сюда будут добавляться редактируемые параметры в группе
}

static void dzicreateitem(void * pwalkctx, void * psectionctx, const void * paramitem)
{
//	char b [32];
//	hamradio_walkmenu_getparamanme(paramitem, b, ARRAY_SIZE(b));
//	char v [32];
//	hamradio_walkmenu_getparamvalue(paramitem, v, ARRAY_SIZE(v));
	//PRINTF(" Param: '%s'\n", b);
	//return;

//	struct menuwalkctx * ctx = (struct menuwalkctx *) pwalkctx;
//	lv_obj_t * section2 = (lv_obj_t *) psectionctx;	// Сюда будут добавляться редактируемые параметры в группе

	lv_obj_t * const obj = hamradio_walkmenu_getparameditor((const struct paramdefdef *) paramitem, (lv_obj_t *) psectionctx);
	//create_slider(section2, LV_SYMBOL_SETTINGS, b, 0, 150, 50);
    //create_switch(section_mechanics, LV_SYMBOL_SETTINGS, b, false);
}

lv_obj_t * lv_hamradiomenu_create(lv_obj_t * parent)
{
	   lv_obj_t * const menu = lv_menu_create(parent);

	    lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_ENABLED);
	    lv_obj_add_event_cb(menu, back_event_handler, LV_EVENT_CLICKED, menu);

	    /*Create a root page*/
		lv_obj_t * const root_page = lv_menu_page_create(menu, "Settings");
		lv_obj_set_style_pad_hor(root_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), LV_PART_MAIN), 0);

		lv_obj_t * const section_first = lv_menu_section_create(root_page);


		struct menuwalkctx ctx;
		ctx.menu = menu;
		ctx.section_first = section_first;
		hamradio_walkmenu(& ctx, dzicreategroup, dzicreateitem);
		lv_obj_set_flag(menu, LV_OBJ_FLAG_SCROLLABLE, true);
		lv_menu_set_sidebar_page(menu, root_page);
	    if (1)
	    {

	    	int32_t idx1 = 0;	// Открыть первое подменю (единственное)
	    	int32_t idx2 = 0;	// выбрать первый элемент в нём (номер группы в меню)
	    	lv_obj_t * ch1 = lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), idx1);
	    	ASSERT(ch1);
	        lv_obj_t * ch2 = lv_obj_get_child(ch1, idx2);
	       	ASSERT(ch2);
	       lv_obj_send_event(ch2, LV_EVENT_CLICKED, NULL);
	    }

		groot_page = root_page;
		return menu;
}

#endif /* WITHLVGL */
