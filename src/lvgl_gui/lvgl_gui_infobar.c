/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN

#include "hardware.h"
#include "formats.h"

#if WITHLVGL && LINUX_SUBSYSTEM

#include "lvgl_gui.h"
#include "windows.h"

static infobar_t infobar;

const uint8_t infobar_places[infobar_count] = {
		INFOBAR_AF,
		INFOBAR_AF_VOLUME,
		INFOBAR_ATT,
		INFOBAR_DNR | infobar_switch,
		INFOBAR_TX_POWER | infobar_switch,
		INFOBAR_DUMMY,
		INFOBAR_CPU_TEMP | infobar_noaction,
		INFOBAR_DUMMY
};

static void infobar_action(uint8_t place)
{
	switch(place)
	{
	case INFOBAR_DNR:
	{
		hamradio_change_nr(1);
		break;
	}

	case INFOBAR_TX_POWER:
	{
		win_open(WIN_TX_POWER);
		break;
	}

	default:
		break;
	}
}

static void infobar_lock(lv_obj_t * btn, uint8_t v)
{
	if (v)
		lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_ORANGE), 0);
	else
		lv_obj_set_style_bg_color(btn, lv_color_black(), 0);
}

static void popup_close(void)
{
	infobar.active_popup_index = 0xFF;

	lv_obj_t * cont = infobar.main;
	uint32_t child_cnt = lv_obj_get_child_count(cont);
	for(uint32_t i = 0; i < child_cnt; i ++)
	{
		lv_obj_t * child = lv_obj_get_child(cont, i);
		if (lv_obj_check_type(child, &lv_button_class))
			lv_obj_add_flag(child, LV_OBJ_FLAG_CLICKABLE);
	}

	lv_obj_del(infobar.popup);
	infobar.popup = NULL;
}

static void popup_buttons_cb(lv_event_t * e)
{
    lv_obj_t * btn = lv_event_get_target(e);
    user_t * ext = lv_obj_get_user_data(btn);
    int8_t payload = ext->payload;
    uint8_t place = ext->index;

	switch(place)
	{
	case INFOBAR_AF:
	{
		hamradio_set_bw(payload);
		popup_close();
		break;
	}

	case INFOBAR_ATT:
	{
		hamradio_set_att_db(payload);
		popup_close();
		break;
	}

	case INFOBAR_AF_VOLUME:
	{
		if (payload == 20)
		{
			hamradio_set_gmutespkr(! hamradio_get_gmutespkr());
			button_set_lock(btn, hamradio_get_gmutespkr());
		}
		else if (! hamradio_get_gmutespkr())
			hamradio_set_afgain(hamradio_get_afgain() + payload);

		break;
	}

	default:
		break;
	}
}

static lv_obj_t * popup_dummy(void)
{
	lv_obj_t * p = lv_obj_create(gui_get_main());

	lv_obj_set_style_bg_color(p, lv_color_hex(0x333333), 0);
	lv_obj_set_style_border_color(p, lv_color_white(), 0);
	lv_obj_set_style_border_width(p, 1, 0);

	char txt[32];
	snprintf(txt, sizeof(txt), "Popup\ndummy");
	lv_obj_t * label = lv_label_create(p);
	lv_label_set_text(label, txt);
	lv_obj_add_style(label, & winlblst, 0);

	return p;
}

static lv_obj_t * popup_create(uint8_t place)
{
	lv_obj_t * p = lv_obj_create(gui_get_main());

	lv_obj_set_style_bg_color(p, lv_color_hex(0x333333), 0);
	lv_obj_set_style_border_color(p, lv_color_white(), 0);
	lv_obj_set_style_border_width(p, 1, 0);
	lv_obj_set_style_min_width(p, 50, 0);
	lv_obj_set_style_min_height(p, 50, 0);
	lv_obj_set_style_pad_all(p, 10, 0);
	lv_obj_set_size(p, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_obj_clear_flag(p, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_layout(p, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(p, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_style_pad_row(p, 5, 0);

	switch(place)
	{
	case INFOBAR_AF:
	{
		static user_t ext[5];
		bws_t bws;
		const int count = hamradio_get_bws(& bws, 5);

		for (int i = 0; i < count; i ++)
		{
			lv_obj_t * btn = lv_obj_create(p);
			lv_obj_add_style(btn, & popupbtnst, 0);
			lv_obj_t * lbl = lv_label_create(btn);
			lv_obj_add_style(lbl, & lblst, 0);
			lv_label_set_text(lbl, bws.label[i]);
			ext[i].payload = i;
			ext[i].index = place;
			lv_obj_set_user_data(btn, & ext[i]);
			lv_obj_add_event_cb(btn, popup_buttons_cb, LV_EVENT_CLICKED, NULL);
		}

		break;
	}

	case INFOBAR_ATT:
	{
		static user_t ext[6];
		uint_fast8_t atts [6];
		unsigned count = hamradio_get_att_dbs(atts, 6);

		for (int i = 0; i < count; i ++)
		{
			lv_obj_t * btn = lv_obj_create(p);
			lv_obj_add_style(btn, & popupbtnst, 0);
			lv_obj_t * lbl = lv_label_create(btn);
			lv_obj_add_style(lbl, & lblst, 0);
			lv_label_set_text_fmt(lbl, "%d db", atts[i] / 10);
			ext[i].payload = i;
			ext[i].index = place;
			lv_obj_set_user_data(btn, & ext[i]);
			lv_obj_add_event_cb(btn, popup_buttons_cb, LV_EVENT_CLICKED, NULL);
		}

		break;
	}

	case INFOBAR_AF_VOLUME:
	{
		enum { btn_num = 3 };

		static user_t btns [btn_num] = {
				{ "+",     1, },
				{ "-", 	  -1, },
				{ "Mute", 20, },
		};

		for (int i = 0; i < btn_num; i ++)
		{
			lv_obj_t * btn = lv_obj_create(p);
			lv_obj_add_style(btn, & popupbtnst, 0);
			lv_obj_t * lbl = lv_label_create(btn);
			lv_obj_add_style(lbl, & lblst, 0);
			lv_label_set_text_fmt(lbl, "%s", btns[i].text);
			btns[i].index = place;
			lv_obj_set_user_data(btn, & btns[i]);
			lv_obj_add_event_cb(btn, popup_buttons_cb, LV_EVENT_CLICKED, NULL);

			if (i == 2) button_set_lock(btn, hamradio_get_gmutespkr());
		}

		break;
	}

	default:
	{
		char txt[32];
		snprintf(txt, sizeof(txt), "Popup2\n %d", index + 1);
		lv_obj_t * label = lv_label_create(p);
		lv_label_set_text(label, txt);
		lv_obj_add_style(label, & winlblst, 0);

		break;
	}
	}

	return p;
}

static void info_button_event_cb(lv_event_t * e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code != LV_EVENT_CLICKED) return;

    lv_obj_t * btn = lv_event_get_target(e);
    user_t * ext = lv_obj_get_user_data(btn);
    uint8_t btn_index = ext->index;
    uint8_t sw = (infobar_places[btn_index] >> infobar_switch_pos) & 1;

    if (sw)
    	infobar_action(ext->payload);
    else
    {
		if (infobar.active_popup_index == btn_index)
			popup_close();
		else
		{
			lv_obj_t * cont = infobar.main;
			uint32_t child_cnt = lv_obj_get_child_count(cont);
			for(uint32_t i = 0; i < child_cnt; i ++)
			{
				lv_obj_t * child = lv_obj_get_child(cont, i);
				if (lv_obj_check_type(child, & lv_button_class))
					lv_obj_clear_flag(child, LV_OBJ_FLAG_CLICKABLE);
			}

			lv_obj_add_flag(btn, LV_OBJ_FLAG_CLICKABLE);

			infobar.active_popup_index = btn_index;

			if (ext->payload)
				infobar.popup = popup_create(ext->payload);
			else
				infobar.popup = popup_dummy();

			if (btn_index == infobar_count - 1)
				lv_obj_align_to(infobar.popup, btn, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 5);
			else
				lv_obj_align_to(infobar.popup, btn, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
		}
    }
}

void infobar_init(lv_obj_t * p)
{
	infobar.active_popup_index = 0xFF;
	infobar.update_count = 0;

	lv_obj_t * cont = lv_obj_create(p);
	infobar.main = cont;
	lv_obj_set_size(cont, DIM_X, 40);
	lv_obj_set_pos(cont, 0, 130);

	lv_obj_set_style_pad_all(cont, 0, 0);
	lv_obj_set_style_pad_gap(cont, 0, 0);
	lv_obj_set_style_bg_color(cont, lv_color_black(), 0);
	lv_obj_set_style_border_width(cont, 0, 0);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_layout(cont, LV_LAYOUT_GRID);

	static lv_coord_t col_dsc[] = {
			LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
			LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1),
			LV_GRID_TEMPLATE_LAST };
	static lv_coord_t row_dsc[] = { LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };

	lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
	lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

	static user_t ext[infobar_count];

	for (int i = 0; i < infobar_count; i++)
	{
		lv_obj_t * btn = lv_button_create(cont);
		lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_STRETCH, i, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

		lv_obj_set_style_bg_color(btn, lv_color_black(), 0);
		lv_obj_set_style_border_width(btn, 2, 0);
		lv_obj_set_style_border_color(btn, lv_palette_main(LV_PALETTE_GREEN), 0);
		lv_obj_set_style_radius(btn, 0, 0);

		if(i == 0)
			lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_RIGHT, 0);
		else if(i == 7)
			lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_LEFT, 0);
		else
			lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT, 0);

		char txt[32];
		lv_obj_t * label = lv_label_create(btn);

		uint8_t place = infobar_places[i] & infobar_valid_mask;

		infobar.to_update[infobar.update_count] = btn;
		infobar.update_count ++;
		ASSERT(infobar.update_count < max_updating_objects);

		if (place)
			ext[i].payload = place;
		else
		{
			snprintf(txt, sizeof(txt), "Btn\n%d", i + 1);
			lv_label_set_text(label, txt);
			lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
			lv_obj_center(label);
		}

		uint8_t clickable = ! ((infobar_places[i] >> infobar_noaction_pos) & 1);
		if (clickable)
		{
			ext[i].index = i;
			lv_obj_add_event_cb(btn, info_button_event_cb, LV_EVENT_CLICKED, NULL);
		}

		lv_obj_set_user_data(btn, & ext[i]);
	}

	gui_update();
}

// ***********************************************

void infobar_update(void)
{
	char buf[32];

	for (int i = 0; i < infobar.update_count; i ++)
	{
		lv_obj_t * p = infobar.to_update[i];
		user_t * ext = lv_obj_get_user_data(p);
		int index = ext->index;

		switch(ext->payload)
		{
		case INFOBAR_CPU_TEMP:
		{
#if defined (GET_CPU_TEMPERATURE)
			float cpu_temp = GET_CPU_TEMPERATURE();
			snprintf(buf, sizeof(buf), "CPU temp\n%2.1f", cpu_temp);
			button_set_text(p, buf);
#endif /* defined (GET_CPU_TEMPERATURE) */
			break;
		}

		case INFOBAR_DNR:
		{
			uint8_t s = hamradio_change_nr(0);
			snprintf(buf, sizeof(buf), "DNR\n%s", s ? "on" : "off");
			button_set_text(p, buf);
			infobar_lock(p, s);
			break;
		}

		case INFOBAR_AF:
		{
			int bp_wide = hamradio_get_bp_type_wide();
			int bp_high = hamradio_get_high_bp(0);
			int bp_low = hamradio_get_low_bp(0) * 10;
			bp_high = bp_wide ? (bp_high * 100) : (bp_high * 10);

			snprintf(buf, sizeof(buf), "AF\n%d-%d", bp_low, bp_high);
			button_set_text(p, buf);

			break;
		}

		case INFOBAR_ATT:
		{
			uint8_t att = hamradio_get_att_db();

			if (att)
				snprintf(buf, sizeof(buf), "ATT\n%d db", att);
			else
				snprintf(buf, sizeof(buf), "ATT\noff");

			button_set_text(p, buf);

			break;
		}

		case INFOBAR_AF_VOLUME:
		{
			uint8_t v = hamradio_get_afgain();
			uint8_t m = hamradio_get_gmutespkr();

			if (m)
				snprintf(buf, sizeof(buf), "AF gain\nmuted");
			else
				snprintf(buf, sizeof(buf), "AF gain\n%d", v);

			button_set_text(p, buf);

			break;
		}

		case INFOBAR_TX_POWER:
		{
			uint8_t tx_pwr = hamradio_get_tx_power();
			uint8_t tune_pwr = hamradio_get_tx_tune_power();

			snprintf(buf, sizeof(buf), "TX %d\%%\nTune %d\%%", tx_pwr, tune_pwr);
			button_set_text(p, buf);

			break;
		}

		default:
			break;
		}
	}
}

#endif /* WITHLVGL && LINUX_SUBSYSTEM */
