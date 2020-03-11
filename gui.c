/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "audio.h"

#include "display/display.h"
#include "formats.h"

#include <string.h>
#include <math.h>

#if WITHTOUCHGUI
	#include "gui.h"
	#include "list.h"

	uint_fast8_t touch_list_count = 0;

	void gui_initialize (void)
	{
		pipparams_t pip;
		uint_fast8_t i = 1, j = 0;
		InitializeListHead(& touch_list);
		do {
			touch_elements[touch_list_count].x1 = button_handlers[i].x1;
			touch_elements[touch_list_count].x2 = button_handlers[i].x2;
			touch_elements[touch_list_count].y1 = button_handlers[i].y1;
			touch_elements[touch_list_count].y2 = button_handlers[i].y2;
			touch_elements[touch_list_count].state = button_handlers[i].state;
			touch_elements[touch_list_count].id = i;
			touch_elements[touch_list_count].is_trackable = button_handlers[i].is_trackable;
			touch_elements[touch_list_count].type = TYPE_BUTTON;
			InsertHeadList(& touch_list, & touch_elements[touch_list_count].item);
			touch_list_count++;
		} while (button_handlers[++i].parent == FOOTER);
		display2_getpipparams(& pip);
		gui.pip_width = pip.w;
		gui.pip_height = pip.h;
	}

	void draw_rect_pip(uint_fast16_t x1, uint_fast16_t y1, uint_fast16_t x2, uint_fast16_t y2, PACKEDCOLOR565_T color, uint8_t fill)
	{
		PACKEDCOLORPIP_T * const colorpip = getscratchpip();
		if (fill)
		{
			for (uint_fast16_t i = y1; i <= y2; i++)
				display_colorbuffer_line_set(colorpip, gui.pip_width, gui.pip_height, x1, i, x2, i, color);
		} else
		{
			display_colorbuffer_line_set(colorpip, gui.pip_width, gui.pip_height, x1, y1, x2, y1, color);
			display_colorbuffer_line_set(colorpip, gui.pip_width, gui.pip_height, x1, y1, x1, y2, color);
			display_colorbuffer_line_set(colorpip, gui.pip_width, gui.pip_height, x1, y2, x2, y2, color);
			display_colorbuffer_line_set(colorpip, gui.pip_width, gui.pip_height, x2, y1, x2, y2, color);
		}
	}

	static uint_fast16_t normalize(uint_fast16_t raw, uint_fast16_t rawmin,	uint_fast16_t rawmax, uint_fast16_t range)
	{
		if (rawmin < rawmax)
		{
			// Normal direction
			const uint_fast16_t distance = rawmax - rawmin;
			if (raw < rawmin)
				return 0;
			raw = raw - rawmin;
			if (raw > distance)
				return range;
			return (uint_fast32_t) raw * range / distance;
		}
		else
		{
			// reverse direction
			const uint_fast16_t distance = rawmin - rawmax;
			if (raw >= rawmin)
				return 0;
			raw = rawmin - raw;
			if (raw > distance)
				return range;
			return (uint_fast32_t) raw * range / distance;
		}
	}

	uint_fast8_t find_button (uint_fast8_t id_window, char * val)				// возврат id кнопки окна по ее названию
	{
		for (uint_fast8_t i = 1; i < button_handlers_count; i++)
		{
			if (button_handlers[i].parent == id_window && strcmp(button_handlers[i].text, val) == 0)
				return i;
		}
		return 0;
	}

	uint_fast8_t find_label (uint_fast8_t id_window, char * val)				// возврат id метки окна по ее названию
	{
		for (uint_fast8_t i = 1; i < labels_count; i++)
		{
			if (labels[i].parent == id_window && strcmp(labels[i].name, val) == 0)
				return i;
		}
		return 0;
	}

	void footer_buttons_state (uint_fast8_t state, char * name)					// блокируются все, кроме name == text
	{
		static uint_fast8_t id = 0;
		if (state == DISABLED)
		{
			id = find_button(FOOTER, name);
			button_handlers[id].is_locked = BUTTON_LOCKED;
		} else
			button_handlers[id].is_locked = BUTTON_NON_LOCKED;

		for (uint_fast8_t i = 1; i < button_handlers_count; i++)
		{
			if (button_handlers[i].parent != FOOTER)
				break;
			button_handlers[i].state = button_handlers[i].text == name ? DISABLED : state;
		}
	}

	void set_window(uint_fast8_t parent, uint_fast8_t value)
	{
		PLIST_ENTRY p;
		for (uint_fast8_t i = 1; i < button_handlers_count; i++)
		{
			if (button_handlers[i].parent == parent)
			{
				button_handlers[i].visible = value ? VISIBLE : NON_VISIBLE;
				button_handlers[i].is_locked = 0;
				if (value)
				{
					touch_elements[touch_list_count].x1 = button_handlers[i].x1;
					touch_elements[touch_list_count].x2 = button_handlers[i].x2;
					touch_elements[touch_list_count].y1 = button_handlers[i].y1;
					touch_elements[touch_list_count].y2 = button_handlers[i].y2;
					touch_elements[touch_list_count].state = button_handlers[i].state;
					touch_elements[touch_list_count].id = i;
					touch_elements[touch_list_count].is_trackable = button_handlers[i].is_trackable;
					touch_elements[touch_list_count].type = TYPE_BUTTON;
					InsertHeadList(& touch_list, & touch_elements[touch_list_count].item);
					touch_list_count++;
				}
				else
				{
					p = RemoveHeadList(& touch_list);
					touch_list_count--;
				}
			}
		}
		for (uint_fast8_t i = 1; i < labels_count; i++)
		{
			if (labels[i].parent == parent)
			{
				labels[i].visible = value ? VISIBLE : NON_VISIBLE;
				if (value && labels[i].onClickHandler != 0)
				{
					labels[i].x2 = labels[i].x1 + strlen(labels[i].text) * 16;
					labels[i].y2 = labels[i].y1 + 15;

					touch_elements[touch_list_count].x1 = labels[i].x1;
					touch_elements[touch_list_count].x2 = labels[i].x2;
					touch_elements[touch_list_count].y1 = labels[i].y1;
					touch_elements[touch_list_count].y2 = labels[i].y2;
					touch_elements[touch_list_count].state = labels[i].state;
					touch_elements[touch_list_count].id = i;
					touch_elements[touch_list_count].is_trackable = labels[i].is_trackable;
					touch_elements[touch_list_count].type = TYPE_LABEL;
					InsertHeadList(& touch_list, & touch_elements[touch_list_count].item);
					touch_list_count++;
				}
				if(! value && labels[i].onClickHandler != 0)
				{
					p = RemoveHeadList(& touch_list);
					touch_list_count--;
				}
			}
		}
		windows[gui.window_to_draw].is_show = value ? VISIBLE : NON_VISIBLE;
		windows[gui.window_to_draw].first_call = 0;
		gui.window_to_draw = value ? gui.window_to_draw : 0;
		(void) p;
	}

	void window_bp_process (void)
	{
		PACKEDCOLORPIP_T * const colorpip = getscratchpip();
		static uint_fast8_t val_high, val_low, val_c, val_w, bw_type;
		static uint_fast16_t x_h, x_l, x_c;
		char buf[10];
		static uint_fast8_t id_button_high, id_button_low, id_button_width, id_button_pitch, id_lbl_high, id_lbl_low;

		if (windows[WINDOW_BP].first_call == 1)
		{
			windows[WINDOW_BP].first_call = 0;

			id_lbl_low = find_label(WINDOW_BP, "lbl_low");
			id_lbl_high = find_label(WINDOW_BP, "lbl_high");
			id_button_high = find_button(WINDOW_BP, "High cut");
			id_button_low = find_button(WINDOW_BP, "Low cut");
			id_button_width = find_button(WINDOW_BP, "Width");
			id_button_pitch = find_button(WINDOW_BP, "Pitch");

			bw_type = get_bp_type();
			if (bw_type)	// BWSET_WIDE
			{
				button_handlers[id_button_high].visible = VISIBLE;
				button_handlers[id_button_low].visible = VISIBLE;
				button_handlers[id_button_width].visible = NON_VISIBLE;
				button_handlers[id_button_pitch].visible = NON_VISIBLE;
				button_handlers[id_button_high].is_locked = 1;

				val_high = get_high_bp(0);
				val_low = get_low_bp(0);

				local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("%d"), val_high * 100);
				strcpy (labels[id_lbl_high].text, buf);
				x_h = normalize(val_high, 0, 50, 290) + 290;
				labels[id_lbl_high].x1 = x_h + 64 > 550 ? 486 : x_h;

				local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("%d"), val_low * 10);
				strcpy (labels[id_lbl_low].text, buf);
				x_l = normalize(val_low, 0, 500, 290) + 290;
				labels[id_lbl_low].x1 = x_l - strlen(buf) * 16;
			}
			else			// BWSET_NARROW
			{
				button_handlers[id_button_high].visible = NON_VISIBLE;
				button_handlers[id_button_low].visible = NON_VISIBLE;
				button_handlers[id_button_width].visible = VISIBLE;
				button_handlers[id_button_pitch].visible = VISIBLE;

				button_handlers[id_button_width].is_locked = 1;

				val_c = get_high_bp(0);
				x_c = 400; //normalize(val_c, 0, 500, 290) + 290;
				val_w = get_low_bp(0) / 2;
				x_l = normalize(190 - val_w , 0, 500, 290) + 290;
				x_h = normalize(190 + val_w , 0, 500, 290) + 290;

				local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("%d"), val_w * 20);
				labels[id_lbl_high].x1 = x_c - strlen(buf) * 8;
				strcpy (labels[id_lbl_high].text, buf);
				local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("P %d"), val_c * 10);
				strcpy (labels[id_lbl_low].text, buf);
				labels[id_lbl_low].x1 = 550 - strlen(buf) * 16;
			}
		}

		if (encoder2.rotate != 0)
		{
			bw_type = get_bp_type();
			if (bw_type)	// BWSET_WIDE
			{
				if (button_handlers[id_button_high].is_locked == 1)
				{
					val_high = get_high_bp(encoder2.rotate);
					encoder2.rotate_done = 1;
					local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("%d"), val_high * 100);
					strcpy (labels[id_lbl_high].text, buf);
					x_h = normalize(val_high, 0, 50, 290) + 290;
					labels[id_lbl_high].x1 = x_h + 64 > 550 ? 486 : x_h;
				}
				else if (button_handlers[id_button_low].is_locked == 1)
				{
					val_low = get_low_bp(encoder2.rotate * 10);
					encoder2.rotate_done = 1;
					local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("%d"), val_low * 10);
					strcpy (labels[id_lbl_low].text, buf);
					x_l = normalize(val_low / 10, 0, 50, 290) + 290;
					labels[id_lbl_low].x1 = x_l - strlen(buf) * 16;
				}
			}
			else				// BWSET_NARROW
			{
				if (button_handlers[id_button_pitch].is_locked == 1)
				{
					val_c = get_high_bp(encoder2.rotate);
					val_w = get_low_bp(0) / 2;
				}
				else if (button_handlers[id_button_width].is_locked == 1)
				{
					val_c = get_high_bp(0);
					val_w = get_low_bp(encoder2.rotate) / 2;
				}
				encoder2.rotate_done = 1;
				x_c = 400;
				x_l = normalize(190 - val_w , 0, 500, 290) + 290;
				x_h = normalize(190 + val_w , 0, 500, 290) + 290;

				local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("%d"), val_w * 20);
				labels[id_lbl_high].x1 = x_c - strlen(buf) * 8;
				strcpy (labels[id_lbl_high].text, buf);
				local_snprintf_P(buf, sizeof buf / sizeof buf[0], PSTR("P %d"), val_c * 10);
				strcpy (labels[id_lbl_low].text, buf);
				labels[id_lbl_low].x1 = 550 - strlen(buf) * 16;
			}
		}

		display_colorbuffer_line_set(colorpip, gui.pip_width, gui.pip_height, 251, 110, 549, 110, COLORPIP_GRAY);
		display_colorbuffer_line_set(colorpip, gui.pip_width, gui.pip_height, 290, 70, 290, 120, COLORPIP_GRAY);
		draw_rect_pip(x_l, 70, x_h, 108, COLORPIP_YELLOW, 1);
		if (! bw_type)
			display_colorbuffer_line_set(colorpip, gui.pip_width, gui.pip_height, x_c, 60, x_c, 120, COLORPIP_RED);
}

	void window_freq_process (void)
	{
		if (windows[WINDOW_FREQ].first_call == 1)
		{
			windows[WINDOW_FREQ].first_call = 0;
			button_handlers[find_button(WINDOW_FREQ, "OK")].is_locked = BUTTON_LOCKED;
		}
	}

	void window_menu_process(void)
	{
		PACKEDCOLORPIP_T * const colorpip = getscratchpip();
		static uint_fast8_t menu_level;

		if (windows[WINDOW_MENU].first_call == 1)
		{
			windows[WINDOW_MENU].first_call = 0;
			set_menu_cond(VISIBLE);

			menu[MENU_GROUPS].add_id = 0;
			menu[MENU_GROUPS].selected_str = 0;
			menu[MENU_GROUPS].selected_label = 0;
			menu[MENU_PARAMS].add_id = 0;
			menu[MENU_PARAMS].selected_str = 0;
			menu[MENU_PARAMS].selected_label = 0;
			menu[MENU_VALS].add_id = 0;
			menu[MENU_VALS].selected_str = 0;
			menu[MENU_VALS].selected_label = 0;

			menu[MENU_GROUPS].first_id = 1;
			while (labels[++menu[MENU_GROUPS].first_id].parent != WINDOW_MENU);				// первое вхождение метки group
			menu[MENU_GROUPS].last_id = menu[MENU_GROUPS].first_id;
			while (strcmp(labels[++menu[MENU_GROUPS].last_id].name, "lbl_group") == 0);
			menu[MENU_GROUPS].last_id--;													// последнее вхождение метки group
			menu[MENU_GROUPS].num_rows = menu[MENU_GROUPS].last_id - menu[MENU_GROUPS].first_id;

			menu[MENU_PARAMS].first_id = menu[MENU_GROUPS].last_id + 1;						// первое вхождение метки params
			menu[MENU_PARAMS].last_id = menu[MENU_PARAMS].first_id;
			while (strcmp(labels[++menu[MENU_PARAMS].last_id].name, "lbl_params") == 0);
			menu[MENU_PARAMS].last_id--;													// последнее вхождение метки params
			menu[MENU_PARAMS].num_rows = menu[MENU_PARAMS].last_id - menu[MENU_PARAMS].first_id;

			menu[MENU_VALS].first_id = menu[MENU_PARAMS].last_id + 1;						// первое вхождение метки vals
			menu[MENU_VALS].last_id = menu[MENU_VALS].first_id;
			while (strcmp(labels[++menu[MENU_VALS].last_id].name, "lbl_vals") == 0);
			menu[MENU_VALS].last_id--;														// последнее вхождение метки vals
			menu[MENU_VALS].num_rows = menu[MENU_VALS].last_id - menu[MENU_VALS].first_id;

			menu[MENU_GROUPS].count = get_multilinemenu_block_groups(menu[MENU_GROUPS].menu_block) - 1;
			for(uint_fast8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
			{
				strcpy(labels[menu[MENU_GROUPS].first_id + i].text, menu[MENU_GROUPS].menu_block[i + menu[MENU_GROUPS].add_id].name);
				labels[menu[MENU_GROUPS].first_id + i].visible = VISIBLE;
				labels[menu[MENU_GROUPS].first_id + i].color = COLORPIP_WHITE;
			}

			menu[MENU_PARAMS].count = get_multilinemenu_block_params(menu[MENU_PARAMS].menu_block, menu[MENU_GROUPS].menu_block[menu[MENU_GROUPS].selected_str].index) - 1;
			for(uint_fast8_t i = 0; i <= menu[MENU_PARAMS].num_rows; i++)
			{
				labels[menu[MENU_PARAMS].first_id + i].visible = NON_VISIBLE;
				labels[menu[MENU_PARAMS].first_id + i].color = COLORPIP_WHITE;
				if (menu[MENU_PARAMS].count < i)
					continue;
				strcpy(labels[menu[MENU_PARAMS].first_id + i].text, menu[MENU_PARAMS].menu_block[i + menu[MENU_PARAMS].add_id].name);
				labels[menu[MENU_PARAMS].first_id + i].visible = VISIBLE;
			}

			menu[MENU_PARAMS].selected_str = 0;
			menu[MENU_VALS].count = menu[MENU_PARAMS].count < menu[MENU_VALS].num_rows ? menu[MENU_PARAMS].count :  menu[MENU_VALS].num_rows;
			get_multilinemenu_block_vals(menu[MENU_VALS].menu_block, menu[MENU_PARAMS].menu_block[menu[MENU_PARAMS].selected_str].index, menu[MENU_VALS].count);
			for(uint_fast8_t i = 0; i <= menu[MENU_VALS].num_rows; i++)
			{
				labels[menu[MENU_VALS].first_id + i].visible = NON_VISIBLE;
				labels[menu[MENU_VALS].first_id + i].color = COLORPIP_WHITE;
				if (menu[MENU_VALS].count < i)
					continue;
				strcpy(labels[menu[MENU_VALS].first_id + i].text, menu[MENU_VALS].menu_block[i + menu[MENU_VALS].add_id].name);
				labels[menu[MENU_VALS].first_id + i].visible = VISIBLE;
			}
			menu_level = MENU_GROUPS;
		}

		if (! encoder2.press_done)
		{
			// выход из режима редактирования параметра  - краткое или длинное нажатие на энкодер
			if (encoder2.press && menu_level == MENU_VALS)
			{
				menu_level = MENU_PARAMS;
				encoder2.press = 0;
			}
			if (encoder2.press)
				menu_level = ++menu_level > MENU_VALS ? MENU_VALS : menu_level;
			if (encoder2.hold)
			{
				menu_level = --menu_level == MENU_OFF ? MENU_OFF : menu_level;
				if (menu_level == MENU_GROUPS)
				{
					menu[MENU_PARAMS].add_id = 0;
					menu[MENU_PARAMS].selected_str = 0;
					menu[MENU_PARAMS].selected_label = 0;
					menu[MENU_VALS].add_id = 0;
					menu[MENU_VALS].selected_str = 0;
					menu[MENU_VALS].selected_label = 0;
				}
			}

			// при переходе на следующий уровень пункт меню подсвечивается
			if (menu_level == MENU_VALS)
				for (uint8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
				{
					labels[i + menu[MENU_PARAMS].first_id].color = i == menu[MENU_PARAMS].selected_label ? COLORPIP_YELLOW : COLORPIP_DARKGRAY;
					labels[i + menu[MENU_VALS].first_id].color = i == menu[MENU_PARAMS].selected_label ? COLORPIP_YELLOW : COLORPIP_DARKGRAY;
				}
			if (menu_level == MENU_PARAMS)
			{
				for (uint8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
				{
					labels[i + menu[MENU_GROUPS].first_id].color = i == menu[MENU_GROUPS].selected_label ? COLORPIP_YELLOW : COLORPIP_DARKGRAY;
					labels[i + menu[MENU_PARAMS].first_id].color = COLORPIP_WHITE;
					labels[i + menu[MENU_VALS].first_id].color = COLORPIP_WHITE;
				}
			}
			if (menu_level == MENU_GROUPS)
				for (uint8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
					labels[i + menu[MENU_GROUPS].first_id].color = COLORPIP_WHITE;

			encoder2.press = 0;
			encoder2.hold = 0;
			encoder2.press_done = 1;
			PRINTF("menu level %d\n", menu_level);
		}

		if (menu_level == MENU_OFF)
		{
			set_window(WINDOW_MENU, NON_VISIBLE);
			encoder2.busy = 0;
			footer_buttons_state(CANCELLED, "");
			set_menu_cond(NON_VISIBLE);
			return;
		}

		if (encoder2.rotate != 0 && menu_level == MENU_VALS)
		{
			encoder2.rotate_done = 1;
			strcpy(labels[menu[MENU_VALS].first_id + menu[MENU_PARAMS].selected_label].text, gui_edit_menu_item(menu[MENU_PARAMS].menu_block[menu[MENU_PARAMS].selected_str].index, encoder2.rotate));
		}

		if (encoder2.rotate != 0 && menu_level != MENU_VALS)
		{
			encoder2.rotate_done = 1;

			menu[menu_level].selected_str = (menu[menu_level].selected_str + encoder2.rotate) <= 0 ? 0 : menu[menu_level].selected_str + encoder2.rotate;
			menu[menu_level].selected_str = menu[menu_level].selected_str > menu[menu_level].count ? menu[menu_level].count : menu[menu_level].selected_str;

			menu[MENU_PARAMS].count = get_multilinemenu_block_params(menu[MENU_PARAMS].menu_block, menu[MENU_GROUPS].menu_block[menu[MENU_GROUPS].selected_str].index) - 1;

			if (encoder2.rotate > 0)
			{
				// указатель подошел к нижней границе списка
				if (++menu[menu_level].selected_label > (menu[menu_level].count < menu[menu_level].num_rows ? menu[menu_level].count : menu[menu_level].num_rows))
				{
					menu[menu_level].selected_label = (menu[menu_level].count < menu[menu_level].num_rows ? menu[menu_level].count : menu[menu_level].num_rows);
					menu[menu_level].add_id = menu[menu_level].selected_str - menu[menu_level].selected_label;
				}
			}
			if (encoder2.rotate < 0)
			{
				// указатель подошел к верхней границе списка
				if (--menu[menu_level].selected_label < 0)
				{
					menu[menu_level].selected_label = 0;
					menu[menu_level].add_id = menu[menu_level].selected_str;
				}
			}

			if (menu_level == MENU_GROUPS)
				for(uint_fast8_t i = 0; i <= menu[MENU_GROUPS].num_rows; i++)
					strcpy(labels[menu[MENU_GROUPS].first_id + i].text, menu[MENU_GROUPS].menu_block[i + menu[MENU_GROUPS].add_id].name);

			menu[MENU_VALS].count = menu[MENU_PARAMS].count < menu[MENU_VALS].num_rows ? menu[MENU_PARAMS].count : menu[MENU_VALS].num_rows;
			get_multilinemenu_block_vals(menu[MENU_VALS].menu_block,  menu[MENU_PARAMS].menu_block[menu[MENU_PARAMS].add_id].index,
										 menu[MENU_VALS].count);

			for(uint_fast8_t i = 0; i <= menu[MENU_PARAMS].num_rows; i++)
			{
				labels[menu[MENU_PARAMS].first_id + i].visible = NON_VISIBLE;
				labels[menu[MENU_VALS].first_id + i].visible = NON_VISIBLE;
				if (i > menu[MENU_PARAMS].count)
					continue;
				strcpy(labels[menu[MENU_PARAMS].first_id + i].text, menu[MENU_PARAMS].menu_block[i + menu[MENU_PARAMS].add_id].name);
				strcpy(labels[menu[MENU_VALS].first_id + i].text,   menu[MENU_VALS].menu_block[i].name);
				labels[menu[MENU_PARAMS].first_id + i].visible = VISIBLE;
				labels[menu[MENU_VALS].first_id + i].visible = VISIBLE;
			}
			PRINTF("%d %s %d\n", menu[menu_level].selected_str, menu[menu_level].menu_block[menu[menu_level].selected_str].name, menu[menu_level].add_id);
		}
		if (menu_level != MENU_VALS)
			display_colorbuff_string_tbg(colorpip, gui.pip_width, gui.pip_height, labels[menu[menu_level].selected_label + menu[menu_level].first_id].x1 - 16,
										 labels[menu[menu_level].selected_label + menu[menu_level].first_id].y1, ">", COLORPIP_GREEN);
	}

	void buttons_menu_handler(void)
	{

	}

	uint_fast8_t check_encoder2 (int_least16_t rotate)
	{
		if (encoder2.rotate_done || encoder2.rotate == 0)
		{
			encoder2.rotate = rotate;
			encoder2.rotate_done = 0;
		}
		return encoder2.busy;
	}

	void set_encoder2_state (uint_fast8_t code)
	{
		if (code == KBD_ENC2_PRESS)
			encoder2.press = 1;
		if (code == KBD_ENC2_HOLD)
			encoder2.hold = 1;
		encoder2.press_done = 0;
	}

	void remove_end_line_spaces(char * str)
	{
		uint_fast8_t i;
		char c[1], outstr[20];
		for(i = strlen(str) - 1; i > 0; i--)
		{
			strncpy(c, & str[i], 1);
			if(strcmp(c, " ") != 0)
				break;
		}
		str[i + 1] = 0;
	}

	void encoder2_menu (enc2_menu_t * enc2_menu)
	{
		uint_fast8_t id_lbl_param = find_label(WINDOW_ENC2, "lbl_enc2_param");
		uint_fast8_t id_lbl_val = find_label(WINDOW_ENC2, "lbl_enc2_val");
		gui.window_to_draw = WINDOW_ENC2;
		set_window(gui.window_to_draw, enc2_menu->state != 0);
		if (windows[gui.window_to_draw].is_show == VISIBLE)
		{
			footer_buttons_state(DISABLED, "");
			strcpy(labels[id_lbl_param].text, enc2_menu->param);
			remove_end_line_spaces(labels[id_lbl_param].text);
			PRINTF("%d\n", strlen(labels[id_lbl_param].text));
			strcpy(labels[id_lbl_val].text, enc2_menu->val);
			labels[id_lbl_val].color = enc2_menu->state == 2 ? COLORPIP_YELLOW : COLORPIP_WHITE;
			labels[id_lbl_val].x1 = windows[WINDOW_ENC2].x1 + ((windows[WINDOW_ENC2].x2 - windows[WINDOW_ENC2].x1) - (strlen (labels[id_lbl_val].text) * 16)) / 2;
			labels[id_lbl_param].x1 = windows[WINDOW_ENC2].x1 + ((windows[WINDOW_ENC2].x2 - windows[WINDOW_ENC2].x1) - (strlen (labels[id_lbl_param].text) * 16)) / 2;
		} else
			footer_buttons_state(CANCELLED, "");
	}

	void buttons_mode_handler(void)
	{
		if (windows[WINDOW_MODES].is_show && button_handlers[gui.selected_id].parent == WINDOW_MODES)
		{
			if (button_handlers[gui.selected_id].payload != UINTPTR_MAX)
				change_submode(button_handlers[gui.selected_id].payload);

			set_window(WINDOW_MODES, NON_VISIBLE);
			footer_buttons_state(CANCELLED, "");
		}
	}

	void buttons_bp_handler (void) // переделать
	{
		if (gui.selected_id == find_button(WINDOW_BP, "Low cut"))
		{
			button_handlers[find_button(WINDOW_BP, "High cut")].is_locked = 0;
			button_handlers[find_button(WINDOW_BP, "Low cut")].is_locked = 1;
		}
		else if (gui.selected_id == find_button(WINDOW_BP, "High cut"))
		{
			button_handlers[find_button(WINDOW_BP, "High cut")].is_locked = 1;
			button_handlers[find_button(WINDOW_BP, "Low cut")].is_locked = 0;
		}
		else if (gui.selected_id == find_button(WINDOW_BP, "Width"))
		{
			button_handlers[find_button(WINDOW_BP, "Width")].is_locked = 1;
			button_handlers[find_button(WINDOW_BP, "Pitch")].is_locked = 0;
		}
		else if (gui.selected_id == find_button(WINDOW_BP, "Pitch"))
		{
			button_handlers[find_button(WINDOW_BP, "Width")].is_locked = 0;
			button_handlers[find_button(WINDOW_BP, "Pitch")].is_locked = 1;
		}
		else if (gui.selected_id == find_button(WINDOW_BP, "OK"))
		{
			set_window(WINDOW_BP, NON_VISIBLE);
			encoder2.busy = 0;
			footer_buttons_state(CANCELLED, "");
		}
	}

	void buttons_freq_handler (void)
	{
		uint_fast8_t editfreqmode = 0;
		if (gui.window_to_draw == WINDOW_FREQ)
			editfreqmode = send_key_code(button_handlers[gui.selected_id].payload);
		if (editfreqmode == 0)
		{
			set_window(WINDOW_FREQ, NON_VISIBLE);
			footer_buttons_state(CANCELLED, "");
		}
	}

	void button1_handler(void)
	{
		if (gui.window_to_draw == 0) gui.window_to_draw = WINDOW_MODES;

		if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
		{
			set_window(WINDOW_MODES, VISIBLE);
//			button_handlers[find_button(WINDOW_MODES, "Mode")].is_locked = 1;
			windows[gui.window_to_draw].first_call = 1;
			footer_buttons_state(DISABLED, button_handlers[gui.selected_id].text);
		}
		else
		{
			set_window(WINDOW_MODES, NON_VISIBLE);
//			button_handlers[find_button(WINDOW_MODES, "Mode")].is_locked = 0;
			footer_buttons_state(CANCELLED, "");
		}
	}

	void button2_handler(void)
	{
		if (gui.window_to_draw == 0) gui.window_to_draw = WINDOW_BP;

		if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
		{
			encoder2.busy = 1;
			set_window(WINDOW_BP, VISIBLE);
			windows[gui.window_to_draw].first_call = 1;
			footer_buttons_state(DISABLED, button_handlers[gui.selected_id].text);
		}
		else
		{
			set_window(WINDOW_BP, NON_VISIBLE);
			encoder2.busy = 0;
			footer_buttons_state(CANCELLED, "");
		}
	}

	void button3_handler(void)
	{
		if (gui.window_to_draw == 0) gui.window_to_draw = WINDOW_AGC;

		if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
		{
			set_window(WINDOW_AGC, VISIBLE);
			windows[gui.window_to_draw].first_call = 1;
			footer_buttons_state(DISABLED, button_handlers[gui.selected_id].text);
		}
		else
		{
			set_window(WINDOW_AGC, NON_VISIBLE);
			footer_buttons_state(CANCELLED, "");
		}
	}

	void button4_handler(void)
	{
		if (gui.window_to_draw == 0) gui.window_to_draw = WINDOW_FREQ;

		if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
		{
			set_window(WINDOW_FREQ, VISIBLE);
			windows[gui.window_to_draw].first_call = 1;
			send_key_code(KBD_CODE_ENTERFREQ);
			footer_buttons_state(DISABLED, button_handlers[gui.selected_id].text);
		}
		else
		{
			set_window(WINDOW_FREQ, NON_VISIBLE);
			send_key_code(KBD_CODE_ENTERFREQDONE);
			footer_buttons_state(CANCELLED, "");
		}
	}

	void button5_handler(void)
	{

	}

	void button6_handler(void)
	{

	}

	void button7_handler(void)
	{

	}

	void button8_handler(void)
	{
		if (gui.window_to_draw == 0) gui.window_to_draw = WINDOW_TEST_TRACKING;

		if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
		{
			set_window(WINDOW_TEST_TRACKING, VISIBLE);
			windows[gui.window_to_draw].first_call = 1;
			footer_buttons_state(DISABLED, button_handlers[gui.selected_id].text);
		}
		else
		{
			set_window(WINDOW_TEST_TRACKING, NON_VISIBLE);
			footer_buttons_state(CANCELLED, "");
		}
	}

	void window_tracking_process(void)
	{
		if (windows[WINDOW_TEST_TRACKING].first_call)
		{
			windows[WINDOW_TEST_TRACKING].first_call = 0;
		}

		if(gui.is_tracking && gui.selected_type == TYPE_BUTTON)				// добавить проверку границ окна
		{
			button_handlers[gui.selected_id].x1 += gui.vector_move_x;
			button_handlers[gui.selected_id].x2 += gui.vector_move_x;
			button_handlers[gui.selected_id].y1 += gui.vector_move_y;
			button_handlers[gui.selected_id].y2 += gui.vector_move_y;
			gui.vector_move_x = 0;
			gui.vector_move_y = 0;
		}
	}

	void button_move_handler(void)
	{

	}

	void labels_test_handler(void)
	{
		PRINTF("Label touched\n");
		labels[gui.selected_id].color = labels[gui.selected_id].color == COLORPIP_WHITE ? COLORPIP_YELLOW : COLORPIP_WHITE;
	}

	void button9_handler(void)
	{
		if (gui.window_to_draw == 0) gui.window_to_draw = WINDOW_MENU;

		if (windows[gui.window_to_draw].is_show == NON_VISIBLE)
		{
			windows[gui.window_to_draw].is_show = VISIBLE;
			windows[gui.window_to_draw].first_call = 1;
			footer_buttons_state(DISABLED, button_handlers[gui.selected_id].text);
			encoder2.busy = 1;
		}
		else
		{
			for (uint_fast8_t i = 1; i < button_handlers_count; i++)
			{
				if (button_handlers[i].parent == gui.window_to_draw)
				{
					button_handlers[i].visible = NON_VISIBLE;
					button_handlers[i].is_locked = 0;
				}
			}
			windows[gui.window_to_draw].is_show = NON_VISIBLE;
			windows[gui.window_to_draw].first_call = 0;
			gui.window_to_draw = 0;
			footer_buttons_state(CANCELLED, "");
			encoder2.busy = 0;
			set_menu_cond(NON_VISIBLE);
		}
	}

	void draw_button_pip(uint_fast16_t x1, uint_fast16_t y1, uint_fast16_t x2, uint_fast16_t y2, uint_fast8_t pressed, uint_fast8_t is_locked, uint_fast8_t is_disabled) // pressed = 0
	{
		PACKEDCOLOR565_T c1, c2;
		c1 = is_disabled ? COLOR_BUTTON_DISABLED : (is_locked ? COLOR_BUTTON_LOCKED : COLOR_BUTTON_NON_LOCKED);
		c2 = is_disabled ? COLOR_BUTTON_DISABLED : (is_locked ? COLOR_BUTTON_PR_LOCKED : COLOR_BUTTON_PR_NON_LOCKED);
		draw_rect_pip(x1, 	y1,	  x2,   y2,   pressed ? c1 : c2, 1);
		draw_rect_pip(x1, 	y1,   x2,   y2,   COLORPIP_GRAY, 0);
		draw_rect_pip(x1+2, y1+2, x2-2, y2-2, COLORPIP_BLACK, 0);
	}

	// FIXME: доелать модификацию цвета для LCDMODE_LTDC_L8
	static PACKEDCOLORPIP_T getshadedcolor(
			PACKEDCOLORPIP_T dot, // исходный цвет
			uint_fast8_t alpha	// на сколько затемнять цвета
			)
	{
	#if LCDMODE_LTDC_L8
		return dot ^ 0x80;	// FIXME: use indexed color
	#else /* LCDMODE_LTDC_L8 */
		PACKEDCOLORPIP_T color_bg = TFTRGB565 (alpha, alpha, alpha);
		if (dot == COLORPIP_BLACK)
			return color_bg; // back gray
		else // RRRR.RGGG.GGGB.BBBB
		{
			PACKEDCOLORPIP_T color_red, color_green, color_blue;
			color_red = dot >> 11;
			color_green = (dot >> 5) & 0x003f;
			color_blue = dot & 0x001f;

			return
				(normalize(color_red, 0, 32, alpha) << 11) |
				(normalize(color_green, 0, 64, alpha) << 5) |
				(normalize(color_blue, 0, 32, alpha));
		}
	#endif /* LCDMODE_LTDC_L8 */
	}

	void pip_transparency_rect(uint_fast16_t x1, uint_fast16_t y1, uint_fast16_t x2, uint_fast16_t y2, uint_fast8_t alpha)
	{
		PACKEDCOLORPIP_T * const colorpip = getscratchpip();

		for (uint_fast16_t y = y1; y <= y2; y++)
		{
			const uint_fast16_t yt = gui.pip_width * y;
			for (uint_fast16_t x = x1; x <= x2; x++)
			{
				colorpip[yt + x] = getshadedcolor(colorpip[yt + x], alpha);
			}
		}
	}

	void display_pip_update(uint_fast8_t x, uint_fast8_t y, void * pv)
	{
		PACKEDCOLORPIP_T * const colorpip = getscratchpip();
		uint_fast16_t yt, xt;
		uint_fast8_t alpha = 10; // на сколько затемнять цвета
		//PACKEDCOLORPIP_T dot, color_red, color_green, color_blue, color_bg = TFTRGB565 (alpha, alpha, alpha);
		char buff [30];
		char * text2 = NULL;
		uint_fast8_t str_len = 0;

		// вывод на PIP служебной информации
	#if WITHTHERMOLEVEL
//		if (hamradio_get_tx())
//		{
			int_fast16_t temp = hamradio_get_temperature_value();
			str_len += local_snprintf_P(&buff[str_len], sizeof buff / sizeof buff [0] - str_len, PSTR("%d.%dC "), temp / 10, temp % 10);
//		}
	#endif /* WITHTHERMOLEVEL */
	#if WITHCURRLEVEL && WITHCPUADCHW	// ток PA (при передаче)
		if (hamradio_get_tx())
		{
			int_fast16_t drain = hamradio_get_pacurrent_value();
			if (drain < 0) drain = 0;
			str_len += local_snprintf_P(&buff[str_len], sizeof buff / sizeof buff [0] - str_len, PSTR("%d.%02dA "), drain / 100, drain % 100);
		}
	#endif /* WITHCURRLEVEL && WITHCPUADCHW */
	#if WITHVOLTLEVEL && WITHCPUADCHW	// напряжение питания
		str_len += local_snprintf_P(&buff[str_len], sizeof buff / sizeof buff [0] - str_len,
									PSTR("%d.%1dV "), hamradio_get_volt_value() / 10, hamradio_get_volt_value() % 10);
	#endif /* WITHVOLTLEVEL && WITHCPUADCHW */
	#if WITHIF4DSP						// ширина панорамы
		str_len += local_snprintf_P(&buff[str_len], sizeof buff / sizeof buff [0] - str_len, PSTR("SPAN:%3dk"), (int) ((display_zoomedbw() + 0) / 1000));
	#endif /* WITHIF4DSP */
		xt = gui.pip_width - 10 - str_len * 10;
		pip_transparency_rect(xt - 5, 225, gui.pip_width - 5, 248, alpha);
		display_colorbuff_string2_tbg(colorpip, gui.pip_width, gui.pip_height, xt, 230, buff, COLORPIP_YELLOW);

	#if defined (RTC1_TYPE)				// текущее время
		uint_fast16_t year;
		uint_fast8_t month, day, hour, minute, secounds;
		str_len = 0;
		board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & secounds);
		str_len += local_snprintf_P(&buff[str_len], sizeof buff / sizeof buff [0] - str_len,
									PSTR("%02d.%02d.%04d %02d%c%02d"), day, month, year, hour, ((secounds & 1) ? ' ' : ':'), minute);
		pip_transparency_rect(5, 225, str_len * 10 + 15, 248, alpha);
		display_colorbuff_string2_tbg(colorpip, gui.pip_width, gui.pip_height, 10, 230, buff, COLORPIP_YELLOW);
	#endif 	/* defined (RTC1_TYPE) */

		if (windows[gui.window_to_draw].is_show)
		{
			pip_transparency_rect(windows[gui.window_to_draw].x1, windows[gui.window_to_draw].y1,
								  windows[gui.window_to_draw].x2, windows[gui.window_to_draw].y2, alpha);

			if (windows[gui.window_to_draw].onVisibleProcess != 0)							// запуск процедуры фоновой обработки для окна, если есть
				windows[gui.window_to_draw].onVisibleProcess();

			// вывод заголовка окна
			display_colorbuff_string_tbg(colorpip, gui.pip_width, gui.pip_height,
										 windows[gui.window_to_draw].x1 + 20,
										 windows[gui.window_to_draw].y1 + 10,
										 windows[gui.window_to_draw].title,
										 COLORPIP_YELLOW);
			// отрисовка принадлежащих окну элементов

			for (uint_fast8_t i = 1; i < labels_count; i++)
			{
				if (labels[i].parent == gui.window_to_draw && labels[i].visible == VISIBLE)	// метки
					display_colorbuff_string_tbg(colorpip, gui.pip_width, gui.pip_height, labels[i].x1, labels[i].y1, labels[i].text, labels[i].color);
			}
		}
		for (uint_fast8_t i = 1; i < button_handlers_count; i++)
		{
			if ((button_handlers[i].parent == gui.window_to_draw && button_handlers[i].visible == VISIBLE && windows[gui.window_to_draw].is_show)
					|| button_handlers[i].parent == FOOTER)									// кнопки
			{
				draw_button_pip(button_handlers[i].x1, button_handlers[i].y1,
								button_handlers[i].x2, button_handlers[i].y2, button_handlers[i].state, button_handlers[i].is_locked,
								button_handlers[i].state == DISABLED ? 1 : 0);


				if (strchr(button_handlers[i].text, ' ') == NULL)
				{
					display_colorbuff_string2_tbg(colorpip, gui.pip_width, gui.pip_height, button_handlers[i].x1 + ((button_handlers[i].x2 - button_handlers[i].x1) -
							(strlen (button_handlers[i].text) * 10)) / 2, button_handlers[i].y1 + 17, button_handlers[i].text, COLORPIP_BLACK);
				} else
				{
					strcpy (buff, button_handlers[i].text);
					text2 = strtok(buff, " ");
					display_colorbuff_string2_tbg(colorpip, gui.pip_width, gui.pip_height, button_handlers[i].x1 + ((button_handlers[i].x2 - button_handlers[i].x1) -
							(strlen (text2) * 10)) / 2, button_handlers[i].y1 + 10, text2, COLORPIP_BLACK);
					text2 = strtok(NULL, '\0');
					display_colorbuff_string2_tbg(colorpip, gui.pip_width, gui.pip_height, button_handlers[i].x1 + ((button_handlers[i].x2 - button_handlers[i].x1) -
							(strlen (text2) * 10)) / 2, button_handlers[i].y1 + 25, text2, COLORPIP_BLACK);
				}
			}
		}
	}

	void set_state_record(list_template_t * val)
	{
		gui.selected_id = val->id;								// добавить везде проверку на gui.selected_type
		switch (val->type)
		{
			case TYPE_BUTTON:
				gui.selected_type = TYPE_BUTTON;
				button_handlers[val->id].state = val->state;
				if (button_handlers[val->id].onClickHandler && button_handlers[val->id].state == RELEASED)
					button_handlers[val->id].onClickHandler();
				break;

			case TYPE_LABEL:
				gui.selected_type = TYPE_LABEL;
				labels[val->id].state = val->state;
				if (labels[val->id].onClickHandler && labels[val->id].state == RELEASED)
					labels[val->id].onClickHandler();
				break;
		}
	}

	void process_gui(void)
	{
		uint_fast16_t tx, ty;
		static uint_fast16_t x_old = 0, y_old = 0;
		pipparams_t pipparam;
		PLIST_ENTRY t;
		static list_template_t * p = NULL;

		if (board_tsc_is_pressed())
		{
			board_tsc_getxy(& tx, & ty);
			if (gui.fix)			// первые координаты после нажатия от контролера тачскрина приходят старые, пропускаем
			{
				display2_getpipparams(&pipparam);
				gui.last_pressed_x = tx;
				gui.last_pressed_y = ty - pipparam.y;
				gui.is_touching_screen = 1;
				debug_printf_P(PSTR("pip x: %d, pip y: %d %d\n"), gui.last_pressed_x, gui.last_pressed_y, gui.is_tracking);
			}
			gui.fix = 1;
		}
		else
		{
			gui.is_touching_screen = 0;
			gui.is_after_touch = 0;
			gui.fix = 0;
		}

		if (gui.state == CANCELLED && gui.is_touching_screen && ! gui.is_after_touch)
		{
			for (t = touch_list.Blink; t != & touch_list; t = t->Blink)
			{
				p = CONTAINING_RECORD(t, list_template_t, item);

				if (p->x1 < gui.last_pressed_x && p->x2 > gui.last_pressed_x
				 && p->y1 < gui.last_pressed_y && p->y2 > gui.last_pressed_y && p->state != DISABLED)
				{
					gui.state = PRESSED;
					break;
				}
			}
		}

		if (gui.is_tracking && ! gui.is_touching_screen)
		{
			gui.is_tracking = 0;
			gui.vector_move_x = 0;
			gui.vector_move_y = 0;
			x_old = 0;
			y_old = 0;
		}

		if (gui.state == PRESSED)
		{
			if (p->is_trackable && gui.is_touching_screen)
			{
				gui.is_tracking = 1;
				gui.vector_move_x = x_old ? gui.vector_move_x + gui.last_pressed_x - x_old : 0; // т.к. process_gui и display_pip_update
				gui.vector_move_y = y_old ? gui.vector_move_y + gui.last_pressed_y - y_old : 0; // вызываются с разной частотой, необходимо
				p->state = PRESSED;																// накопление вектора перемещения точки
				set_state_record(p);
				x_old = gui.last_pressed_x;
				y_old = gui.last_pressed_y;
				debug_printf_P(PSTR("move x: %d, move y: %d\n"), gui.vector_move_x, gui.vector_move_y);
			}
			else if (p->x1 < gui.last_pressed_x && p->x2 > gui.last_pressed_x
			 && p->y1 < gui.last_pressed_y && p->y2 > gui.last_pressed_y && ! gui.is_after_touch)
			{
				if (gui.is_touching_screen)
				{
					p->state = PRESSED;
					set_state_record(p);
				}
				else
					gui.state = RELEASED;
			}
			else
			{
				gui.state = CANCELLED;
				p->state = CANCELLED;
				set_state_record(p);
				gui.is_after_touch = 1; 	// точка непрерывного нажатия вышла за пределы выбранного элемента, не поддерживающего tracking
			}
		}
		if (gui.state == RELEASED)
		{
			p->state = RELEASED;			// для запуска обработчика нажатия
			set_state_record(p);
			p->state = CANCELLED;
			set_state_record(p);
			gui.is_after_touch = 0;
			gui.state = CANCELLED;
			gui.is_tracking = 0;
		}
	}
#endif /* WITHTOUCHGUI */
