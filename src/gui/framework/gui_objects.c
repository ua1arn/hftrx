/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN

#include "../gui_port_include.h"

#if WITHTOUCHGUI

#include "gui.h"
#include "gui_system.h"
#include "gui_structs.h"
#include "gui_settings.h"
#include "gui_windows.h"
#include "../gui_user.h"

const label_t label_default = 	{ 0, CANCELLED, 0, NON_VISIBLE, "", "", COLORPIP_WHITE, };
const button_t button_default = { 0, 0, CANCELLED, BUTTON_NON_LOCKED, 0, 1, 0, 0, NON_VISIBLE, INT32_MAX, "", "", };
const text_field_t tf_default = { 0, 0, CANCELLED, 0, NON_VISIBLE, UP, NULL, "", };
const touch_area_t ta_default = { 0, 0, 0, 0, 0, "", 0, 0, 0, 0, 0, };

const gui_color_t btn_bg_colors[BG_COUNT] =
		{
				COLOR_BUTTON_NON_LOCKED,
				COLOR_BUTTON_PR_NON_LOCKED,
				COLOR_BUTTON_LOCKED,
				COLOR_BUTTON_PR_LOCKED,
				COLOR_BUTTON_DISABLED,
		};

static btn_bg_t btn_bg[] = {
	{ COMMON_BUTTON_STYLE, },
	{ SMALL_BUTTON_STYLE, },
	{ LONG_BUTTON_STYLE, },
};
enum { BG_DEF_COUNT = ARRAY_SIZE(btn_bg) };

// *************** Labels ***************

/* Получение ширины метки в пикселях  */
uint16_t get_label_width(const label_t * const lh)
{
	return lh->width_pix;
}

/* Получение высоты метки в пикселях  */
uint16_t get_label_height(const label_t * const lh)
{
	return lh->height_pix;
}

uint16_t get_label_width2(const char * name)
{
	window_t * win = get_win(get_parent_window());
	label_t * lh = (label_t *) find_gui_obj(TYPE_LABEL, win, name);

	return lh->width_pix;
}

uint16_t get_label_height2(const char * name)
{
	window_t * win = get_win(get_parent_window());
	label_t * lh = (label_t *) find_gui_obj(TYPE_LABEL, win, name);

	return lh->height_pix;
}

static void __draw_label(label_t * lh, uint16_t x, uint16_t y, const gui_drawbuf_t * db)
{
	__gui_print_mono(db, x, y, lh->text, lh->font, lh->color);
}

void draw_label(label_t * lh)
{
	window_t * win = get_win(lh->parent);
	const gui_drawbuf_t * gdb = __gui_get_drawbuf();
	uint16_t x = win->x1 + lh->x;
	uint16_t y = win->y1 + lh->y;

#if GUI_USE_CACHE
#if DEBUG_LABELS_CACHE
	static uint32_t cache_hits = 0, cache_misses = 0;

	if (lh->cache != NULL && ! gui_objects_cache_needs_render(lh->cache, 0, 0, lh->text))
		cache_hits++;
	else
		cache_misses++;

	if ((cache_hits + cache_misses) % 60 == 0)
	{
		printf("Labels cache: hits=%u, misses=%u, hit_rate=%.1f%%\n", cache_hits, cache_misses,
				100.0f * cache_hits / (cache_hits + cache_misses));
		cache_hits = 0;
		cache_misses = 0;
	}
 #endif /* DEBUG_LABELS_CACHE */

	if (lh->cache != NULL && ! gui_objects_cache_needs_render(lh->cache, 0, 0, lh->text))
	{
		/* Кэш действителен - копируем готовую текстуру */
		if (gui_objects_cache_draw(lh->cache, x, y)) goto fallback_render;
		return;
	}

	/* Кэш недействителен - создаём/обновляем */
	if (lh->cache == NULL)
	{
		lh->cache = gui_objects_cache_create(lh->width_pix, lh->height_pix, GUI_CACHE_TYPE_LABEL);
		if (lh->cache == NULL) goto fallback_render;
	}

	/* Рендерим в кэш */
	if (gui_objects_cache_begin_render(lh->cache))
	{
		const gui_drawbuf_t * cache_db = __gui_get_drawbuf();

		__draw_label(lh, 0, 0, cache_db);

		gui_objects_cache_end_render(lh->cache, 0, 0, lh->text);
	}

	/* Копируем из кэша на экран */
	if (gui_objects_cache_draw(lh->cache, x, y)) goto fallback_render;
	return;

fallback_render:
#endif /* GUI_USE_CACHE */

	__draw_label(lh, x, y, gdb);
}

// *************** Buttons ****************

static void fill_button_bg_buf(btn_bg_t * v)
{
	const uint16_t w = v->w;
	const uint16_t h = v->h;

	for (int i = 0; i < BG_COUNT; i ++)
	{
		v->bgs[i] = __gui_object_bgbuf_init(w, h);

		gui_drawbuf_t butdbv;
		__gui_drawbuf_init(& butdbv, v->bgs[i], w, h);
		__gui_draw_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLORPIP_GRAY, 0);
		__gui_draw_rounded_rect(& butdbv, 1, 1, w - 3, h - 3, button_round_radius, COLORPIP_BLACK, 0);
		__gui_draw_rounded_rect(& butdbv, 2, 2, w - 5, h - 5, button_round_radius, btn_bg_colors[i], 1);
		__gui_drawbuf_end(& butdbv);
	}
}

static void __draw_button(button_t * bh, uint16_t x, uint16_t y, const gui_drawbuf_t * db)
{
	window_t * win = get_win(bh->parent);

	/* Поиск кэшированного фона по размерам */
	btn_bg_t * b1 = NULL;
#if ! GUI_USE_CACHE
	uint8_t i = 0;
	do {
		if (bh->h == btn_bg[i].h && bh->w == btn_bg[i].w)
		{
			b1 = & btn_bg[i];
			break;
		}
	} while (++i < BG_DEF_COUNT);
#endif /* ! GUI_USE_CACHE */

	if (b1 == NULL)
	{
		/* Программная отрисовка фона */
		gui_color_t c1, c2;
		c1 = bh->state == DISABLED ? COLOR_BUTTON_DISABLED : (bh->is_locked ? COLOR_BUTTON_LOCKED : COLOR_BUTTON_NON_LOCKED);
		c2 = bh->state == DISABLED ? COLOR_BUTTON_DISABLED : (bh->is_locked ? COLOR_BUTTON_PR_LOCKED : COLOR_BUTTON_PR_NON_LOCKED);

		__gui_draw_rect(db, x, y, bh->w - 1, bh->h - 1, GUI_DEFAULTCOLOR, 1);
		__gui_draw_rounded_rect(db, x, y, bh->w - 1, bh->h - 1, button_round_radius, COLORPIP_GRAY, 0);
		__gui_draw_rounded_rect(db, x + 1, y + 1, bh->w - 3, bh->h - 3, button_round_radius, COLORPIP_BLACK, 0);
		__gui_draw_rounded_rect(db, x + 2, y + 2, bh->w - 5, bh->h - 5, button_round_radius, bh->state == PRESSED ? c2 : c1, 1);
	} else
	{
		/* Копирование кэшированного фона */
		gui_objbgbuf_t * bg = NULL;

		if (bh->state == DISABLED) bg = b1->bgs[BG_DISABLED];
		else if (bh->is_locked && bh->state == PRESSED) bg = b1->bgs[BG_LOCKED_PRESED];
		else if (bh->is_locked && bh->state != PRESSED) bg = b1->bgs[BG_LOCKED];
		else if (!bh->is_locked && bh->state == PRESSED) bg = b1->bgs[BG_PRESSED];
		else if (!bh->is_locked && bh->state != PRESSED) bg = b1->bgs[BG_NON_PRESSED];

		if (bg != NULL)
		{
			gui_drawbuf_t bgv;
			__gui_drawbuf_init(& bgv, bg, bh->w, bh->h);
			__gui_drawbuf_copy(db, & bgv, x, y, bh->w, bh->h);
			__gui_drawbuf_end(& bgv);
		}
	}

	/* Отрисовка текста кнопки */
	const uint16_t shiftX = bh->state == PRESSED ? 1 : 0;
	const uint16_t shiftY = bh->state == PRESSED ? 1 : 0;
	const gui_color_t textcolor = COLORPIP_BLACK;
	static const char delimeters[] = "|";

	if (strchr(bh->text, delimeters[0]) == NULL)
	{
		/* Однострочная надпись */
		int strlenP = get_strwidth_prop(bh->text, bh->font);
		__gui_print_prop(db, shiftX + x + (bh->w - strlenP) / 2, shiftY + y + (bh->h - bh->font->height) / 2,
				bh->text, bh->font, textcolor);
	} else
	{
		/* Двухстрочная надпись */
		char * next;
		uint8_t j = (bh->h - bh->font->height * 2) / 2;
		char buf[TEXT_ARRAY_SIZE];
		strcpy(buf, bh->text);
		char * text2 = strtok_r(buf, delimeters, & next);

		int strlenP = get_strwidth_prop(text2, bh->font);
		__gui_print_prop(db, shiftX + x + (bh->w - strlenP) / 2, shiftY + y + j, text2, bh->font, textcolor);

		text2 = strtok_r(NULL, delimeters, & next);
		strlenP = get_strwidth_prop(text2, bh->font);
		__gui_print_prop(db, shiftX + x + (bh->w - strlenP) / 2, shiftY + bh->h + y - bh->font->height - j,
				text2, bh->font, textcolor);
	}

	if (bh->is_focus)
		gui_drawDashedRectangle(x + 4, y + 4, bh->w - 8, bh->h - 8, 4, COLOR_BLACK);
}

void draw_button(button_t * bh)
{
	window_t * win = get_win(bh->parent);
	const gui_drawbuf_t * gdb = __gui_get_drawbuf();
	uint16_t x1 = win->x1 + bh->x1;
	uint16_t y1 = win->y1 + bh->y1;

#if GUI_USE_CACHE
#if DEBUG_BUTTONS_CACHE
	static uint32_t cache_hits = 0, cache_misses = 0;

	if (bh->cache != NULL && ! gui_objects_cache_needs_render(bh->cache, bh->state, bh->is_locked, bh->text))
		cache_hits++;
	else
		cache_misses++;

	if ((cache_hits + cache_misses) % 60 == 0)
	{
		printf("Buttons cache: hits=%u, misses=%u, hit_rate=%.1f%%\n", cache_hits, cache_misses,
				100.0f * cache_hits / (cache_hits + cache_misses));
		cache_hits = 0;
		cache_misses = 0;
	}
 #endif /* DEBUG_BUTTONS_CACHE */

	if (bh->cache != NULL && ! gui_objects_cache_needs_render(bh->cache, bh->state, bh->is_locked, bh->text))
	{
		/* Кэш действителен - копируем готовую текстуру */
		if (gui_objects_cache_draw(bh->cache, x1, y1)) goto fallback_render;
		return;
	}

	/* Кэш недействителен - создаём/обновляем */
	if (bh->cache == NULL)
	{
		bh->cache = gui_objects_cache_create(bh->w, bh->h, GUI_CACHE_TYPE_BUTTON);
		if (bh->cache == NULL) goto fallback_render;
	}

	/* Рендерим в кэш */
	if (gui_objects_cache_begin_render(bh->cache))
	{
		const gui_drawbuf_t * cache_db = __gui_get_drawbuf();

		__draw_button(bh, 0, 0, cache_db);

		gui_objects_cache_end_render(bh->cache, bh->state, bh->is_locked, bh->text);
	}

	/* Копируем из кэша на экран */
	if (gui_objects_cache_draw(bh->cache, x1, y1)) goto fallback_render;
	return;

fallback_render:
#endif /* defined(GUI_USE_CACHE) */

	__draw_button(bh, x1, y1, gdb);
}

static void __draw_close_button(button_t * bh, uint16_t x, uint16_t y, const gui_drawbuf_t * db)
{
	uint16_t w = bh->w;
	uint16_t h = bh->h;

	__gui_draw_rect(db, x, y, w,  h, COLORPIP_BLACK, 0);
	__gui_draw_line(db, x, y, x + w, y + h, COLORPIP_BLACK);
	__gui_draw_line(db, x, y + h, x + w, y, COLORPIP_BLACK);
}

void draw_close_button(button_t * bh)
{
	window_t * win = get_win(bh->parent);
	const gui_drawbuf_t * gdb = __gui_get_drawbuf();
	uint16_t x = win->x1 + bh->x1;
	uint16_t y = win->y1 + bh->y1;

#if GUI_USE_CACHE
	if (bh->cache != NULL && ! gui_objects_cache_needs_render(bh->cache, bh->state, bh->is_locked, bh->text))
	{
		/* Кэш действителен - копируем готовую текстуру */
		if (gui_objects_cache_draw(bh->cache, x, y)) goto fallback_render;
		return;
	}

	/* Кэш недействителен - создаём/обновляем */
	if (bh->cache == NULL)
	{
		bh->cache = gui_objects_cache_create(bh->w, bh->h, GUI_CACHE_TYPE_BUTTON);
		if (bh->cache == NULL) goto fallback_render;
	}

	/* Рендерим в кэш */
	if (gui_objects_cache_begin_render(bh->cache))
	{
		const gui_drawbuf_t * cache_db = __gui_get_drawbuf();

		__draw_close_button(bh, 0, 0, cache_db);

		gui_objects_cache_end_render(bh->cache, bh->state, bh->is_locked, bh->text);
	}

	/* Копируем из кэша на экран */
	if (gui_objects_cache_draw(bh->cache, x, y)) goto fallback_render;
	return;

fallback_render:
#endif /* GUI_USE_CACHE */

	__draw_close_button(bh, x, y, gdb);
}

// *************** Text fields ***************

/* Рассчитать размеры текстового поля */
void textfield_update_size(text_field_t * tf)
{
	GUI_ASSERT(tf != NULL);

	tf->w = tf->font->width * tf->w_sim;
	tf->h = tf->font->height * tf->h_str;

	GUI_ASSERT(tf->w < WITHGUIMAXX);
	GUI_ASSERT(tf->h < WITHGUIMAXY - window_title_height);
}

/* Добавить строку в текстовое поле */
void textfield_add_string_old(text_field_t * tf, const char * str, gui_color_t color)
{
	GUI_ASSERT(tf != NULL);

	tf_entry_t * rec = &  tf->string[tf->index];
	strncpy(rec->text, str, TEXT_ARRAY_SIZE - 1);
	rec->color_line = color;
	tf->index ++;
	tf->index = tf->index >= tf->h_str ? 0 : tf->index;
}

void textfield_add_string(const char * name, const char * str, gui_color_t color)
{
	window_t * win = get_win(get_parent_window());
	text_field_t * tf = (text_field_t *) find_gui_obj(TYPE_TEXT_FIELD, win, name);

	tf_entry_t * rec = &  tf->string[tf->index];
	GUI_ASSERT(rec);
	strncpy(rec->text, str, TEXT_ARRAY_SIZE - 1);
	rec->color_line = color;
	tf->index ++;
	tf->index = tf->index >= tf->h_str ? 0 : tf->index;
}

/* Очистить текстовое поле */
void textfield_clean(const char * name)
{
	window_t * win = get_win(get_parent_window());
	text_field_t * tf = (text_field_t *) find_gui_obj(TYPE_TEXT_FIELD, win, name);

	tf->index = 0;
	memset(tf->string, 0, tf->h_str * sizeof(tf_entry_t));
}

static void __draw_textfield(text_field_t * tf, uint16_t x, uint16_t y, const gui_drawbuf_t * db)
{
	int_fast8_t j = tf->index - 1;

	for (uint8_t i = 0; i < tf->h_str; i ++)
	{
		uint8_t pos = tf->direction ? i : (tf->h_str - i - 1);
		j = j < 0 ? (tf->h_str - 1) : j;

		__gui_print_mono(drawbuf, x, y + tf->font->height * pos,
				tf->string[j].text, tf->font, tf->string[j].color_line);

		j --;
	}
}

void draw_textfield(text_field_t * tf)
{
	window_t * win = get_win(tf->parent);
	const gui_drawbuf_t * gdb = __gui_get_drawbuf();
	uint16_t x = win->x1 + tf->x1;
	uint16_t y = win->y1 + tf->y1;

#if GUI_USE_CACHE
#if DEBUG_TFS_CACHE
	static uint32_t cache_hits = 0, cache_misses = 0;

	if (tf->cache != NULL && ! gui_objects_cache_needs_render(tf->cache, 0, tf->cache->flags, ""))
		cache_hits++;
	else
		cache_misses++;

	if ((cache_hits + cache_misses) % 60 == 0)
	{
		printf("TFs cache: hits=%u, misses=%u, hit_rate=%.1f%%\n", cache_hits, cache_misses,
				100.0f * cache_hits / (cache_hits + cache_misses));
		cache_hits = 0;
		cache_misses = 0;
	}
 #endif /* DEBUG_TFS_CACHE */

	if (tf->cache != NULL && ! gui_objects_cache_needs_render(tf->cache, 0, tf->cache->flags, ""))
	{
		/* Кэш действителен - копируем готовую текстуру */
		if (gui_objects_cache_draw(tf->cache, x, y)) goto fallback_render;
		return;
	}

	/* Кэш недействителен - создаём/обновляем */
	if (tf->cache == NULL)
	{
		tf->cache = gui_objects_cache_create(tf->w, tf->h, GUI_CACHE_TYPE_TF);
		if (tf->cache == NULL) goto fallback_render;
		tf->cache->flags = 0;
	}

	/* Рендерим в кэш */
	if (gui_objects_cache_begin_render(tf->cache))
	{
		const gui_drawbuf_t * cache_db = __gui_get_drawbuf();

		__draw_textfield(tf, 0, 0, cache_db);

		gui_objects_cache_end_render(tf->cache, 0, tf->cache->flags, "");
	}

	/* Копируем из кэша на экран */
	if (gui_objects_cache_draw(tf->cache, x, y)) goto fallback_render;
	return;

fallback_render:
#endif /* GUI_USE_CACHE */

__draw_textfield(tf, x, y, gdb);
}

// *************** Sliders ****************

static void slider_update(slider_t * sl, uint16_t x, uint16_t y)
{
	if (sl->orientation == ORIENTATION_HORIZONTAL)
	{
		sl->value_p = sl->scale_x + sl->scale_size * sl->value / 100;
		sl->x1_p = sl->value_p - sliders_w;
		sl->y1_p = 0;
		sl->x2_p = sl->value_p + sliders_w;
		sl->y2_p = sliders_h * 2;
	}
	else if (sl->orientation == ORIENTATION_VERTICAL)
	{
		sl->value_p = sl->scale_y + sl->scale_size * sl->value / 100;
		sl->x1_p = 0;
		sl->y1_p = sl->value_p - sliders_w;
		sl->x2_p = sliders_h * 2;
		sl->y2_p = sl->value_p + sliders_w;
	}
}

static void __draw_slider(slider_t * sl, uint16_t x, uint16_t y, const gui_drawbuf_t * db)
{
	if (sl->orientation == ORIENTATION_HORIZONTAL)
	{
		// temp background
		//__gui_draw_rect(db, x, y,  sl->size, sliders_h * 2, COLORPIP_YELLOW, 1);

		// scale
		__gui_draw_rect(db, x + sl->scale_x, y + sl->scale_y, sl->scale_size, sliders_scale_thickness, COLORPIP_WHITE, 0);
		__gui_draw_rect(db, x + sl->scale_x + 1, y + sl->scale_y + 1, sl->scale_size - 2, sliders_scale_thickness - 2, COLORPIP_BLACK, 1);

		// handle
		__gui_draw_rect(db, sl->x1_p, sl->y1_p,  sl->x2_p - sl->x1_p, sl->y2_p - sl->y1_p, sl->state == PRESSED ? COLOR_BUTTON_PR_NON_LOCKED : COLOR_BUTTON_NON_LOCKED, 1);
		__gui_draw_line(db, x + sl->value_p, sl->y1_p, x + sl->value_p, sl->y2_p - 1, COLORPIP_WHITE);
	}
	else if (sl->orientation == ORIENTATION_VERTICAL)
	{
		// temp background
		//__gui_draw_rect(db, x, y, sliders_h * 2, sl->size, COLORPIP_YELLOW, 1);

		// scale
		__gui_draw_rect(db, x + sl->scale_x, y + sl->scale_y, sliders_scale_thickness, sl->scale_size, COLORPIP_WHITE, 0);
		__gui_draw_rect(db, x + sl->scale_x + 1, y + sl->scale_y + 1, sliders_scale_thickness - 2, sl->scale_size - 2, COLORPIP_BLACK, 1);

		// handle
		__gui_draw_rect(db, sl->x1_p, sl->y1_p,  sl->x2_p - sl->x1_p, sl->y2_p - sl->y1_p, sl->state == PRESSED ? COLOR_BUTTON_PR_NON_LOCKED : COLOR_BUTTON_NON_LOCKED, 1);
		__gui_draw_line(db, sl->x1_p, y + sl->value_p, sl->x2_p - 1, y + sl->value_p, COLORPIP_WHITE);
	}
}

/* Отрисовка слайдера */
void draw_slider(slider_t * sl)
{
	window_t * win = get_win(sl->parent);
	const gui_drawbuf_t * gdb = __gui_get_drawbuf();

	uint16_t x = win->x1 + sl->x;
	uint16_t y = win->y1 + sl->y;

	slider_update(sl, x, y);

#if GUI_USE_CACHE
#if DEBUG_SLIDERS_CACHE
	static uint32_t cache_hits = 0, cache_misses = 0;

	if (sl->cache != NULL && ! gui_objects_cache_needs_render(sl->cache, sl->state, sl->value, ""))
		cache_hits++;
	else
		cache_misses++;

	if ((cache_hits + cache_misses) % 60 == 0)
	{
		printf("Sliders cache: hits=%u, misses=%u, hit_rate=%.1f%%\n", cache_hits, cache_misses,
				100.0f * cache_hits / (cache_hits + cache_misses));
		cache_hits = 0;
		cache_misses = 0;
	}
 #endif /* DEBUG_BUTTONS_CACHE */

	if (sl->cache != NULL && ! gui_objects_cache_needs_render(sl->cache, sl->state, sl->value, ""))
	{
		/* Кэш действителен - копируем готовую текстуру */
		if (gui_objects_cache_draw(sl->cache, x, y)) goto fallback_render;
		return;
	}

	/* Кэш недействителен - создаём/обновляем */
	if (sl->cache == NULL)
	{
		sl->cache = gui_objects_cache_create(sl->width, sl->height, GUI_CACHE_TYPE_SLIDER);
		if (sl->cache == NULL) goto fallback_render;
	}

	/* Рендерим в кэш */
	if (gui_objects_cache_begin_render(sl->cache))
	{
		const gui_drawbuf_t * cache_db = __gui_get_drawbuf();

		__draw_slider(sl, 0, 0, cache_db);

		gui_objects_cache_end_render(sl->cache, sl->state, sl->value, "");
	}

	/* Копируем из кэша на экран */
	if (gui_objects_cache_draw(sl->cache, x, y)) goto fallback_render;
	return;

fallback_render:
#endif /* defined(GUI_USE_CACHE) */

	__draw_slider(sl, x, y, gdb);
}

// *************** Common ***************

static obj_type_t parse_obj_name(const char * name)
{
	GUI_ASSERT(name);

	if (! strncmp(name, "btn_", 4))
		return TYPE_BUTTON;
	else if (! strncmp(name, "lbl_", 4))
		return TYPE_LABEL;
	else if (! strncmp(name, "sl_", 3))
		return TYPE_SLIDER;
	else if (! strncmp(name, "btc_", 4))
		return TYPE_CLOSE_BUTTON;
	else if (! strncmp(name, "ta_", 3))
		return TYPE_TOUCH_AREA;
	else if (! strncmp(name, "tf_", 3))
		return TYPE_TEXT_FIELD;
	else
	{
		PRINTF("Unrecognized GUI object type: %s\n", name);
		GUI_ASSERT(0);
		return TYPE_DUMMY;
	}
}

static void obj_name_user(char * name)
{
	char * r = strrchr(name, '#');
	name[r - name] = '\0';
}

// label: color, width_by_symbols
// button: w, h, is_repeating, is_long_press, text,
// text_field: w_sim, h_str, direction, font *
// touch area: x, y, w, h, is_trackable
// возвращает индекс созданного объекта

uint8_t gui_obj_create(const char * name, ...)
{
	uint8_t idx, window_id = get_parent_window();
	window_t * win = get_win(window_id);
	va_list arg;
	va_start(arg, name);

	char obj_name[NAME_ARRAY_SIZE] = { 0 };
	local_snprintf_P(obj_name, NAME_ARRAY_SIZE, "%s#%02d", name, win->window_id);
	obj_type_t type = parse_obj_name(obj_name);

	switch (type)
	{
	case TYPE_LABEL:
	{
		win->lh_ptr = (label_t *) realloc(win->lh_ptr, sizeof(label_t) * (win->lh_count + 1));
		GUI_MEM_ASSERT(win->lh_ptr);

		label_t * lh = & win->lh_ptr[win->lh_count];
		memcpy(lh, & label_default, sizeof(label_t));

		lh->parent = window_id;
		lh->color = va_arg(arg, gui_color_t);
		lh->visible = 1;
		lh->index = win->lh_count;
		lh->x = 0;
		lh->y = 0;
		lh->font = & LABELS_FONT_DEFAULT;
		lh->height_pix = lh->font->height;

		strncpy(lh->name, obj_name, NAME_ARRAY_SIZE);
		lh->width = va_arg(arg, uint32_t);
		memset(lh->text, '*', lh->width);		// для совместимости, потом убрать
		lh->width_pix = get_strwidth_mono(" ", lh->font) * lh->width;
#if GUI_USE_CACHE
		lh->cache = NULL;
#endif /* GUI_USE_CACHE */

		idx = win->lh_count;
		win->lh_count ++;
		break;
	}

	case TYPE_BUTTON:
	{
		win->bh_ptr = (button_t *) realloc(win->bh_ptr, sizeof(button_t) * (win->bh_count + 1));
		GUI_MEM_ASSERT(win->bh_ptr);

		button_t * bh = & win->bh_ptr[win->bh_count];
		memcpy(bh, & button_default, sizeof(button_t));

		bh->parent = window_id;
		bh->w = va_arg(arg, int);
		bh->h = va_arg(arg, int);
		bh->is_repeating = va_arg(arg, uint32_t);
		bh->is_long_press = va_arg(arg, uint32_t);
		strncpy(bh->name, obj_name, NAME_ARRAY_SIZE);
		strncpy(bh->text, va_arg(arg, char *), TEXT_ARRAY_SIZE - 1);
		bh->visible = 1;
		bh->index = win->bh_count;
		bh->x1 = 0;
		bh->y1 = 0;
		bh->font = & BUTTONS_FONTP_DEFAULT;
#if GUI_USE_CACHE
		bh->cache = NULL;
#endif /* GUI_USE_CACHE */

		idx = win->bh_count;
		win->bh_count ++;
		break;
	}

	case TYPE_TEXT_FIELD:
	{
		win->tf_ptr = (text_field_t *) realloc(win->tf_ptr, sizeof(text_field_t) * (win->tf_count + 1));
		GUI_MEM_ASSERT(win->tf_ptr);

		text_field_t * tf = & win->tf_ptr[win->tf_count];
		memcpy(tf, & tf_default, sizeof(text_field_t));

		tf->parent = window_id;
		tf->w_sim = va_arg(arg, uint32_t);
		tf->h_str = va_arg(arg, uint32_t);
		tf->direction = (tf_direction_t) va_arg(arg, uint32_t);
		tf->font = va_arg(arg, gui_mono_font_t *);
		strncpy(tf->name, obj_name, NAME_ARRAY_SIZE);
		tf->visible = 1;
		tf->index = win->tf_count;
		tf->x1 = 0;
		tf->y1 = 0;
#if GUI_USE_CACHE
		tf->cache = NULL;
#endif /* GUI_USE_CACHE */

		tf->string = (tf_entry_t *) calloc(tf->h_str, sizeof(tf_entry_t));
		GUI_MEM_ASSERT(tf->string);
		tf->index = 0;

		textfield_update_size(tf);

		idx = win->tf_count;
		win->tf_count ++;
		break;
	}

	case TYPE_TOUCH_AREA:
	{
		win->ta_ptr = (touch_area_t *) realloc(win->ta_ptr, sizeof(touch_area_t) * (win->ta_count + 1));
		GUI_MEM_ASSERT(win->ta_ptr);

		touch_area_t * ta = & win->ta_ptr[win->ta_count];
		memcpy(ta, & ta_default, sizeof(touch_area_t));

		ta->parent = window_id;
		ta->x1 = va_arg(arg, int);
		ta->y1 = va_arg(arg, int);
		ta->w = va_arg(arg, int);
		ta->h = va_arg(arg, int);
		ta->is_trackable = va_arg(arg, int);
		strncpy(ta->name, obj_name, NAME_ARRAY_SIZE);
		ta->visible = 1;
		ta->index = win->ta_count;

		idx = win->ta_count;
		win->ta_count ++;
		break;
	}

	case TYPE_SLIDER:
	{
		win->sh_ptr = (slider_t *) realloc(win->sh_ptr, sizeof(slider_t) * (win->sh_count + 1));
		GUI_MEM_ASSERT(win->sh_ptr);

		slider_t * sh = & win->sh_ptr[win->sh_count];
		memset(sh, 0, sizeof(slider_t));

		sh->parent = window_id;
		sh->orientation = va_arg(arg, int);
		strncpy(sh->name, obj_name, NAME_ARRAY_SIZE);
		sh->state = CANCELLED;
		sh->visible = 1;
		sh->size = va_arg(arg, int);
		sh->step = va_arg(arg, int);
		sh->value = 0;
		sh->value_old = 255;
		sh->index = win->sh_count;

		if (sh->orientation)	// ORIENTATION_HORIZONTAL
		{
			sh->width = sh->size;
			sh->height = sliders_h * 2;
			sh->scale_x = sliders_w;
			sh->scale_y = sliders_h - sliders_scale_thickness / 2;
			sh->scale_size = sh->size - sliders_w * 2;
		}
		else					// ORIENTATION_VERTICAL
		{
			sh->width = sliders_h * 2;
			sh->height = sh->size;
			sh->scale_x = sliders_h - sliders_scale_thickness / 2;
			sh->scale_y = sliders_w;
			sh->scale_size = sh->size - sliders_h * 2;
		}

#if GUI_USE_CACHE
		sh->cache = NULL;
#endif /* GUI_USE_CACHE */

		idx = win->sh_count;
		win->sh_count ++;
		break;
	}

	default:
		idx = 0;
		break;
	}

	va_end(arg);
	return idx;
}

void gui_obj_align_to(const char * name1, const char * name2, object_alignment_t align, uint16_t offset)
{
	window_t * win = get_win(get_parent_window());
	obj_type_t type1 = parse_obj_name(name1);
	obj_type_t type2 = parse_obj_name(name2);
	void * oh1 = find_gui_obj(type1, win, name1);
	void * oh2 = find_gui_obj(type2, win, name2);

	if (oh1 == oh2)
		return;

	uint16_t x2 = 0, y2 = 0, w2 = 0, h2 = 0;

	switch(type2)
	{
	case TYPE_LABEL:
		label_t * lh2 = (label_t *) oh2;
		x2 = lh2->x;
		y2 = lh2->y;
		w2 = get_label_width(lh2);
		h2 = get_label_height(lh2);
		break;

	case TYPE_BUTTON:
		button_t * bh2 = (button_t *) oh2;
		x2 = bh2->x1;
		y2 = bh2->y1;
		w2 = bh2->w;
		h2 = bh2->h;
		break;

	case TYPE_TEXT_FIELD:
		text_field_t * tf2 = (text_field_t *) oh2;
		x2 = tf2->x1;
		y2 = tf2->y1;
		w2 = tf2->w;
		h2 = tf2->h;
		break;

	case TYPE_SLIDER:
		slider_t * sh2 = (slider_t *) oh2;
		x2 = sh2->x;
		y2 = sh2->y;
		w2 = sh2->width;
		h2 = sh2->height;

	default:
		break;
	}

	switch(type1)
	{
	case TYPE_LABEL:
		label_t * lh1 = (label_t *) oh1;

		if (align == ALIGN_RIGHT_UP) { lh1->x = x2 + w2 + offset; lh1->y = y2; }
		else if (align == ALIGN_RIGHT_UP_MID) { lh1->x = x2 + w2 + offset; lh1->y = y2 + (h2 / 2 - get_label_height(lh1) / 2); }
		else if (align == ALIGN_LEFT_UP)  { lh1->x = x2 - get_label_width(lh1) - offset; lh1->y = y2; }
		else if (align == ALIGN_DOWN_LEFT) { lh1->x = x2; lh1->y = y2 + h2 + offset; }
		else if (align == ALIGN_DOWN_MID) { lh1->x = x2 + w2 / 2 - get_label_width(lh1) / 2; lh1->y = y2 + h2 + offset; }
		else if (align == ALIGN_DOWN_RIGHT) { lh1->x = x2 + w2 - get_label_width(lh1); lh1->y = y2 + h2 + offset; }
		break;

	case TYPE_BUTTON:
		button_t * bh1 = (button_t *) oh1;

		if (align == ALIGN_RIGHT_UP) { bh1->x1 = x2 + w2 + offset; bh1->y1 = y2; }
		else if (align == ALIGN_RIGHT_UP_MID) { bh1->x1 = x2 + w2 + offset; bh1->y1 = y2 + (h2 / 2 - bh1->h / 2); }
		else if (align == ALIGN_LEFT_UP)  { bh1->x1 = x2 - bh1->w - offset; bh1->y1 = y2; }
		else if (align == ALIGN_DOWN_LEFT) { bh1->x1 = x2; bh1->y1 = y2 + h2 + offset; }
		else if (align == ALIGN_DOWN_MID) { bh1->x1 = x2 + w2 / 2 - bh1->w / 2; bh1->y1 = y2 + h2 + offset; }
		else if (align == ALIGN_DOWN_RIGHT) { bh1->x1 = x2 + w2 - bh1->w; bh1->y1 = y2 + h2 + offset; }
		break;

	case TYPE_SLIDER:
		slider_t * sh1 = (slider_t *) oh1;

		if (align == ALIGN_RIGHT_UP) { sh1->x = x2 + w2 + offset; sh1->y = y2; }
		else if (align == ALIGN_RIGHT_UP_MID) { sh1->x = x2 + w2 + offset; sh1->y = y2 + (h2 / 2 - sh1->height / 2); }
		else if (align == ALIGN_LEFT_UP)  { sh1->x = x2 - sh1->width - offset; sh1->y = y2; }
		else if (align == ALIGN_DOWN_LEFT) { sh1->x = x2; sh1->y = y2 + h2 + offset; }
		else if (align == ALIGN_DOWN_MID) { sh1->x = x2 + w2 / 2 - sh1->width / 2; sh1->y = y2 + h2 + offset; }
		else if (align == ALIGN_DOWN_RIGHT) { sh1->x = x2 + w2 - sh1->width; sh1->y = y2 + h2 + offset; }
		break;

	default:
		break;
	}
}

char * gui_obj_get_string_prop(const char * name, object_prop_t prop)
{
	window_t * win = get_win(get_parent_window());
	obj_type_t type = parse_obj_name(name);
	void * obj = find_gui_obj(type, win, name);

	switch(type)
	{
	case TYPE_LABEL:
		label_t * lh = (label_t *) obj;
		if (prop == GUI_OBJ_TEXT) return lh->text;
		break;
	case TYPE_BUTTON:
		button_t * bh = (button_t *) obj;
		if (prop == GUI_OBJ_TEXT) return bh->text;
		break;
	default:
		break;
	}

	return NULL;
}

int gui_obj_get_int_prop(const char * name, object_prop_t prop)
{
	window_t * win = get_win(get_parent_window());
	obj_type_t type = parse_obj_name(name);
	void * obj = find_gui_obj(type, win, name);

	switch(type)
	{
	case TYPE_LABEL:
		label_t * lh = (label_t *) obj;
		if (prop == GUI_OBJ_VISIBLE) return lh->visible;
		else if (prop == GUI_OBJ_POS_X) return lh->x;
		else if (prop == GUI_OBJ_POS_Y) return lh->y;
		else if (prop == GUI_OBJ_PAYLOAD) return lh->payload;
		else if (prop == GUI_OBJ_STATE) return lh->state;
		else if (prop == GUI_OBJ_WIDTH) return lh->width_pix;
		else if (prop == GUI_OBJ_HEIGHT) return lh->height_pix;
		else if (prop == GUI_OBJ_INDEX) return lh->index;
		break;

	case TYPE_BUTTON:
		button_t * bh = (button_t *) obj;
		if (prop == GUI_OBJ_VISIBLE) return bh->visible;
		else if (prop == GUI_OBJ_POS_X) return bh->x1;
		else if (prop == GUI_OBJ_POS_Y) return bh->y1;
		else if (prop == GUI_OBJ_PAYLOAD) return bh->payload;
		else if (prop == GUI_OBJ_STATE) return bh->state;
		else if (prop == GUI_OBJ_LOCK) return bh->is_locked;
		else if (prop == GUI_OBJ_WIDTH) return bh->w;
		else if (prop == GUI_OBJ_HEIGHT) return bh->h;
		else if (prop == GUI_OBJ_INDEX) return bh->index;
		break;

	case TYPE_SLIDER:
		slider_t * sh = (slider_t *) obj;
		if (prop == GUI_OBJ_VISIBLE) return sh->visible;
		else if (prop == GUI_OBJ_POS_X) return sh->x;
		else if (prop == GUI_OBJ_POS_Y) return sh->y;
		else if (prop == GUI_OBJ_STATE) return sh->state;
		else if (prop == GUI_OBJ_WIDTH) return sh->width;
		else if (prop == GUI_OBJ_HEIGHT) return sh->height;
		else if (prop == GUI_OBJ_PAYLOAD) return sh->value;
		else if (prop == GUI_OBJ_SIZE) return sh->size;
		else if (prop == GUI_OBJ_INDEX) return sh->index;
		break;

	case TYPE_TOUCH_AREA:
		touch_area_t * ta = (touch_area_t *) obj;
		if (prop == GUI_OBJ_VISIBLE) return ta->visible;
		else if (prop == GUI_OBJ_POS_X) return ta->x1;
		else if (prop == GUI_OBJ_POS_Y) return ta->y1;
		else if (prop == GUI_OBJ_PAYLOAD) return ta->payload;
		else if (prop == GUI_OBJ_INDEX) return ta->index;
		break;

	case TYPE_TEXT_FIELD:
		text_field_t * tf = (text_field_t *) obj;
		if (prop == GUI_OBJ_VISIBLE) return tf->visible;
		else if (prop == GUI_OBJ_POS_X) return tf->x1;
		else if (prop == GUI_OBJ_POS_Y) return tf->y1;
		else if (prop == GUI_OBJ_WIDTH) return tf->w;
		else if (prop == GUI_OBJ_HEIGHT) return tf->h;
		else if (prop == GUI_OBJ_INDEX) return tf->index;
		break;

	default:
		break;
	}
	return 0;
}

void gui_obj_set_prop(const char * name, object_prop_t prop, ...)
{
	window_t * win = get_win(get_parent_window());
	obj_type_t type = parse_obj_name(name);
	void * obj = find_gui_obj(type, win, name);

	va_list arg;
	va_start(arg, prop);

	switch(type)
	{
	case TYPE_LABEL:
		label_t * lh = (label_t *) obj;
		if (prop == GUI_OBJ_VISIBLE) lh->visible = !! va_arg(arg, int);
		else if (prop == GUI_OBJ_POS_X) lh->x = va_arg(arg, int);
		else if (prop == GUI_OBJ_POS_Y) lh->y = va_arg(arg, int);
		else if (prop == GUI_OBJ_POS) { lh->x = va_arg(arg, int); lh->y = va_arg(arg, int); }
		else if (prop == GUI_OBJ_PAYLOAD) lh->payload = va_arg(arg, int);
		else if (prop == GUI_OBJ_TEXT) {
			strncpy(lh->text, va_arg(arg, char *), TEXT_ARRAY_SIZE - 1);
			//lh->width_pix = get_strwidth_mono(lh->text, lh->font); // todo: разделить свойства ширины метки и ширины текста в ней
		}
		else if (prop == GUI_OBJ_TEXT_FMT) vsnprintf(lh->text, TEXT_ARRAY_SIZE - 1, va_arg(arg, char *), arg);
		else if (prop == GUI_OBJ_STATE) lh->state = va_arg(arg, int);
		else if (prop == GUI_OBJ_COLOR) lh->color = va_arg(arg, gui_color_t);
		else if (prop == GUI_OBJ_FONT) {
			lh->font = va_arg(arg, gui_mono_font_t *);
			lh->height_pix = lh->font->height;
			lh->width_pix = get_strwidth_mono(" ", lh->font) * lh->width;
		}

#if GUI_USE_CACHE
		if (prop & NEED_INVALIDATION_MASK) gui_objects_cache_invalidate(lh->cache);
#endif /* GUI_USE_CACHE */

		break;

	case TYPE_BUTTON:
		button_t * bh = (button_t *) obj;
		if (prop == GUI_OBJ_VISIBLE) bh->visible = !! va_arg(arg, int);
		else if (prop == GUI_OBJ_POS_X) bh->x1 = va_arg(arg, int);
		else if (prop == GUI_OBJ_POS_Y) bh->y1 = va_arg(arg, int);
		else if (prop == GUI_OBJ_POS) { bh->x1 = va_arg(arg, int); bh->y1 = va_arg(arg, int); }
		else if (prop == GUI_OBJ_PAYLOAD) bh->payload = va_arg(arg, int);
		else if (prop == GUI_OBJ_TEXT) strncpy(bh->text, va_arg(arg, char *), TEXT_ARRAY_SIZE - 1);
		else if (prop == GUI_OBJ_TEXT_FMT) vsnprintf(bh->text, TEXT_ARRAY_SIZE - 1, va_arg(arg, char *), arg);
		else if (prop == GUI_OBJ_STATE) bh->state = va_arg(arg, int);
		else if (prop == GUI_OBJ_LOCK) bh->is_locked = !! va_arg(arg, int);
		else if (prop == GUI_OBJ_WIDTH) bh->w = va_arg(arg, int);
		else if (prop == GUI_OBJ_HEIGHT) bh->h = va_arg(arg, int);
		else if (prop == GUI_OBJ_SIZE) { bh->w = va_arg(arg, int); bh->h = va_arg(arg, int); }
		else if (prop == GUI_OBJ_REPEAT) bh->is_repeating = !! va_arg(arg, int);
		else if (prop == GUI_OBJ_LONG_PRESS) bh->is_long_press = !! va_arg(arg, int);
		else if (prop == GUI_OBJ_FONT) bh->font = va_arg(arg, gui_prop_font_t *);

#if GUI_USE_CACHE
		if (prop & NEED_INVALIDATION_MASK) gui_objects_cache_invalidate(bh->cache);
#endif /* GUI_USE_CACHE */

		break;

	case TYPE_SLIDER:
		slider_t * sh = (slider_t *) obj;
		if (prop == GUI_OBJ_VISIBLE) sh->visible = !! va_arg(arg, int);
		else if (prop == GUI_OBJ_POS_X) sh->x = va_arg(arg, int);
		else if (prop == GUI_OBJ_POS_Y) sh->y = va_arg(arg, int);
		else if (prop == GUI_OBJ_POS) { sh->x = va_arg(arg, int); sh->y = va_arg(arg, int); }
		else if (prop == GUI_OBJ_PAYLOAD) sh->value = va_arg(arg, int);
		break;

	case TYPE_TEXT_FIELD:
		text_field_t * tf = (text_field_t *) obj;
		if (prop == GUI_OBJ_VISIBLE) tf->visible = !! va_arg(arg, int);
		else if (prop == GUI_OBJ_POS_X) tf->x1 = va_arg(arg, int);
		else if (prop == GUI_OBJ_POS_Y) tf->y1 = va_arg(arg, int);
		else if (prop == GUI_OBJ_POS) { tf->x1 = va_arg(arg, int); tf->y1 = va_arg(arg, int); }
		else if (prop == GUI_OBJ_TEXT) {
			tf_entry_t * rec = &  tf->string[tf->index];
			strncpy(rec->text, va_arg(arg, char *), TEXT_ARRAY_SIZE - 1);
			rec->color_line = va_arg(arg, int);
			tf->index ++;
			tf->index = tf->index >= tf->h_str ? 0 : tf->index;
		}
		else if (prop == GUI_OBJ_TEXT_FMT) {
			tf_entry_t * rec = &  tf->string[tf->index];
			vsnprintf(rec->text, TEXT_ARRAY_SIZE - 1, va_arg(arg, char *), arg);
			rec->color_line = va_arg(arg, int);
			tf->index ++;
			tf->index = tf->index >= tf->h_str ? 0 : tf->index;
		}
		else if (prop == GUI_OBJ_CLEAN) {
			tf->index = 0;
			memset(tf->string, 0, tf->h_str * sizeof(tf_entry_t));
		}

#if GUI_USE_CACHE
		if (prop & NEED_INVALIDATION_MASK)
		{
			gui_objects_cache_invalidate(tf->cache);
			tf->cache->flags = 1;
		}
#endif /* GUI_USE_CACHE */
		break;

	default:
		break;
	}

	va_end(arg);
}

uint8_t gui_check_obj(const char * name1, const char * name2)
{
	return strcmp(name1, name2) == 0;
}

// выравнивание однотипных объектов (кнопка, метка, слайдер) с передачей массива имен объектов
void gui_arrange_objects(const char names[][NAME_ARRAY_SIZE], uint8_t count, uint8_t cols, uint8_t interval)
{
	if (count <= 1) return;

	window_t * win = get_win(get_parent_window());

	obj_type_t type = parse_obj_name(names[0]);
	if (type != TYPE_BUTTON && type != TYPE_LABEL && type != TYPE_SLIDER)
	{
		PRINTF("%s: idx %d unsupported object type to arrange\n", __func__, 0);
		GUI_ASSERT(0);
	}

	uint16_t x = gui_obj_get_int_prop(names[0], GUI_OBJ_POS_X);
	uint16_t y = gui_obj_get_int_prop(names[0], GUI_OBJ_POS_Y);
	uint16_t w = gui_obj_get_int_prop(names[0], GUI_OBJ_WIDTH);
	uint16_t h = gui_obj_get_int_prop(names[0], GUI_OBJ_HEIGHT);

	for (int i = 1; i < count; i ++)
	{
		uint8_t row = i / cols;
		uint8_t col = i % cols;

		const char * obj = names[i];

		obj_type_t typex = parse_obj_name(obj);
		if (typex != type)
		{
			PRINTF("%s: idx %d - arrange various objects not supported\n", __func__, i);
			GUI_ASSERT(0);
		}

		gui_obj_set_prop(obj, GUI_OBJ_POS_X, x + (w + interval) * col);
		gui_obj_set_prop(obj, GUI_OBJ_POS_Y, y + (h + interval) * row);
	}
}

char * get_obj_name_by_idx(obj_type_t type, uint8_t idx)
{
	window_t * win = get_win(get_parent_window());
	static char obj_name[NAME_ARRAY_SIZE] = { 0 };

	if (type == TYPE_BUTTON)
	{
		GUI_ASSERT(idx < win->bh_count);

		strncpy(obj_name, win->bh_ptr[idx].name, NAME_ARRAY_SIZE);
		obj_name_user(obj_name);
		return obj_name;
	}
	else if (type == TYPE_LABEL)
	{
		GUI_ASSERT(idx < win->lh_count);
		strncpy(obj_name, win->lh_ptr[idx].name, NAME_ARRAY_SIZE);
		obj_name_user(obj_name);
		return obj_name;
	}
	else if (type == TYPE_SLIDER)
	{
		GUI_ASSERT(idx < win->sh_count);
		strncpy(obj_name, win->sh_ptr[idx].name, NAME_ARRAY_SIZE);
		obj_name_user(obj_name);
		return obj_name;
	}

	GUI_ASSERT(0);
	return NULL;
}

static uint8_t get_obj_idx_by_name(window_t * win, obj_type_t type, const char * name)
{
	void * p = find_gui_obj(type, win, name);

	if (type == TYPE_BUTTON)
		return ((button_t *) p)->index;
	else if (type == TYPE_LABEL)
		return ((label_t *) p)->index;
	else if (type == TYPE_SLIDER)
		return ((slider_t *) p)->index;

	GUI_ASSERT(0);
	return 0;
}

// выравнивание однотипных объектов (кнопка, метка, слайдер) с передачей имени первого объекта,
// обработка по возрастанию индекса (в порядке создания)
void gui_arrange_objects_from(const char * name, uint8_t count, uint8_t cols, uint8_t interval)
{
	if (count <= 1) return;

	window_t * win = get_win(get_parent_window());

	obj_type_t type = parse_obj_name(name);
	if (type != TYPE_BUTTON && type != TYPE_LABEL && type != TYPE_SLIDER)
	{
		PRINTF("%s: idx %d unsupported object type to arrange\n", __func__, 0);
		GUI_ASSERT(0);
	}

	uint16_t x = gui_obj_get_int_prop(name, GUI_OBJ_POS_X);
	uint16_t y = gui_obj_get_int_prop(name, GUI_OBJ_POS_Y);
	uint16_t w = gui_obj_get_int_prop(name, GUI_OBJ_WIDTH);
	uint16_t h = gui_obj_get_int_prop(name, GUI_OBJ_HEIGHT);
	uint8_t idx = get_obj_idx_by_name(win, type, name) + 1;

	for (int i = 1; i < count; i ++)
	{
		uint8_t row = i / cols;
		uint8_t col = i % cols;

		const char * obj = get_obj_name_by_idx(type, idx ++);

		gui_obj_set_prop(obj, GUI_OBJ_POS_X, x + (w + interval) * col);
		gui_obj_set_prop(obj, GUI_OBJ_POS_Y, y + (h + interval) * row);
	}
}

void gui_objects_init(void)
{
#if ! GUI_USE_CACHE
	// Buttons background init
	for (int i = 0; i < BG_DEF_COUNT; i ++)
		fill_button_bg_buf(& btn_bg[i]);
#endif /* ! GUI_USE_CACHE */
}

#endif /* WITHTOUCHGUI */
