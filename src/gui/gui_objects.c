/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Touch GUI от RA4ASN

#include "hardware.h"
#include "formats.h"
#include "display/display.h"
#include "display/fontmaps.h"

#if WITHTOUCHGUI

#include "gui.h"
#include "gui_user.h"
#include "gui_system.h"
#include "gui_structs.h"
#include "gui_settings.h"
#include "gui_windows.h"

// *************** Buttons ***************

static btn_bg_t btn_bg [] = {
	{ 130, 35, },
	{ 100, 44, },
	{ 86, 44, },
};
enum { BG_COUNT = ARRAY_SIZE(btn_bg) };

static void fill_button_bg_buf(btn_bg_t * v)
{
	const uint_fast16_t w = v->w;
	const uint_fast16_t h = v->h;
	const size_t s = GXSIZE(w, h) * sizeof (PACKEDCOLORPIP_T);

	v->bg_non_pressed = 	(PACKEDCOLORPIP_T *) malloc(s);
	GUI_MEM_ASSERT(v->bg_non_pressed);
	v->bg_pressed = 		(PACKEDCOLORPIP_T *) malloc(s);
	GUI_MEM_ASSERT(v->bg_pressed);
	v->bg_locked = 			(PACKEDCOLORPIP_T *) malloc(s);
	GUI_MEM_ASSERT(v->bg_locked);
	v->bg_locked_pressed = 	(PACKEDCOLORPIP_T *) malloc(s);
	GUI_MEM_ASSERT(v->bg_locked_pressed);
	v->bg_disabled = 		(PACKEDCOLORPIP_T *) malloc(s);
	GUI_MEM_ASSERT(v->bg_disabled);

	{
		gxdrawb_t butdbv;
		gxdrawb_initialize(& butdbv, v->bg_non_pressed, w, h);
	#if GUI_OLDBUTTONSTYLE
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLOR_BUTTON_NON_LOCKED, 1);
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_rect(& butdbv, 2, 2, w - 3, h - 3, COLORPIP_BLACK, 0);
	#else
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, GUI_DEFAULTCOLOR, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_NON_LOCKED, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLORPIP_GRAY, 0);
		colmain_rounded_rect(& butdbv, 2, 2, w - 3, h - 3, button_round_radius, COLORPIP_BLACK, 0);
	#endif /* GUI_OLDBUTTONSTYLE */
	}

	{
		gxdrawb_t butdbv;
		gxdrawb_initialize(& butdbv, v->bg_pressed, w, h);
	#if GUI_OLDBUTTONSTYLE
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLOR_BUTTON_PR_NON_LOCKED, 1);
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_line(& butdbv, 2, 3, w - 3, 3, COLORPIP_BLACK, 0);
		colpip_line(& butdbv, 2, 2, w - 3, 2, COLORPIP_BLACK, 0);
		colpip_line(& butdbv, 3, 3, 3, h - 3, COLORPIP_BLACK, 0);
		colpip_line(& butdbv, 2, 2, 2, h - 2, COLORPIP_BLACK, 0);
	#else
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, GUI_DEFAULTCOLOR, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_PR_NON_LOCKED, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLORPIP_GRAY, 0);
		colmain_rounded_rect(& butdbv, 2, 2, w - 3, h - 3, button_round_radius, COLORPIP_BLACK, 0);
	#endif /* GUI_OLDBUTTONSTYLE */
	}

	{
		gxdrawb_t butdbv;
		gxdrawb_initialize(& butdbv, v->bg_locked, w, h);
	#if GUI_OLDBUTTONSTYLE
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLOR_BUTTON_LOCKED, 1);
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_rect(& butdbv, 2, 2, w - 3, h - 3, COLORPIP_BLACK, 0);
	#else
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, GUI_DEFAULTCOLOR, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_LOCKED, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLORPIP_GRAY, 0);
		colmain_rounded_rect(& butdbv, 2, 2, w - 3, h - 3, button_round_radius, COLORPIP_BLACK, 0);
	#endif /* GUI_OLDBUTTONSTYLE */
	}

	{
		gxdrawb_t butdbv;
		gxdrawb_initialize(& butdbv, v->bg_locked_pressed, w, h);
	#if GUI_OLDBUTTONSTYLE
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLOR_BUTTON_PR_LOCKED, 1);
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_line(& butdbv, 2, 3, w - 3, 3, COLORPIP_BLACK, 0);
		colpip_line(& butdbv, 2, 2, w - 3, 2, COLORPIP_BLACK, 0);
		colpip_line(& butdbv, 3, 3, 3, h - 3, COLORPIP_BLACK, 0);
		colpip_line(& butdbv, 2, 2, 2, h - 2, COLORPIP_BLACK, 0);
	#else
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, GUI_DEFAULTCOLOR, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_PR_LOCKED, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLORPIP_GRAY, 0);
		colmain_rounded_rect(& butdbv, 2, 2, w - 3, h - 3, button_round_radius, COLORPIP_BLACK, 0);
	#endif /* GUI_OLDBUTTONSTYLE */
	}

	{
		gxdrawb_t butdbv;
		gxdrawb_initialize(& butdbv, v->bg_disabled, w, h);
	#if GUI_OLDBUTTONSTYLE
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLOR_BUTTON_DISABLED, 1);
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, COLORPIP_GRAY, 0);
		colpip_rect(& butdbv, 2, 2, w - 3, h - 3, COLORPIP_BLACK, 0);
	#else
		colpip_rect(& butdbv, 0, 0, w - 1, h - 1, GUI_DEFAULTCOLOR, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLOR_BUTTON_DISABLED, 1);
		colmain_rounded_rect(& butdbv, 0, 0, w - 1, h - 1, button_round_radius, COLORPIP_GRAY, 0);
		colmain_rounded_rect(& butdbv, 2, 2, w - 3, h - 3, button_round_radius, COLORPIP_BLACK, 0);
	#endif /* GUI_OLDBUTTONSTYLE */
	}
}

void objects_init(void)
{
	// Buttons background init
	for (int i = 0; i < BG_COUNT; i ++)
		fill_button_bg_buf(& btn_bg [i]);
}

void draw_button(const button_t * const bh)
{
	PACKEDCOLORPIP_T * bg = NULL;
	window_t * win = get_win(bh->parent);
	const gxdrawb_t * gdb = gui_get_drawbuf();
	uint_fast8_t i = 0;
	static const char delimeters [] = "|";
	uint_fast16_t x1 = win->x1 + bh->x1;
	uint_fast16_t y1 = win->y1 + bh->y1;

	if ((x1 + bh->w >= WITHGUIMAXX) || (y1 + bh->h >= WITHGUIMAXY))
	{
		PRINTF("%s %s x+w: %d y+h: %d\n", bh->name, bh->text, x1 + bh->w, y1 + bh->h);
		ASSERT(0);
	}

	btn_bg_t * b1 = NULL;
	do {
		if (bh->h == btn_bg [i].h && bh->w == btn_bg [i].w)
		{
			b1 = & btn_bg [i];
			break;
		}
	} while ( ++i < BG_COUNT);

	// если не найден заполненный буфер фона по размерам, программная отрисовка
	if (b1 == NULL)
	{
		PACKEDCOLORPIP_T c1, c2;
		c1 = bh->state == DISABLED ? COLOR_BUTTON_DISABLED : (bh->is_locked ? COLOR_BUTTON_LOCKED : COLOR_BUTTON_NON_LOCKED);
		c2 = bh->state == DISABLED ? COLOR_BUTTON_DISABLED : (bh->is_locked ? COLOR_BUTTON_PR_LOCKED : COLOR_BUTTON_PR_NON_LOCKED);

		colpip_rectangle(gdb, x1 + 1, y1 + 1, bh->w - 1, bh->h - 1, GUI_DEFAULTCOLOR, FILL_FLAG_NONE);
		colmain_rounded_rect(gdb, x1, y1, x1 + bh->w, y1 + bh->h - 2, button_round_radius, bh->state == PRESSED ? c2 : c1, 1);
		colmain_rounded_rect(gdb, x1, y1, x1 + bh->w, y1 + bh->h - 1, button_round_radius, COLORPIP_GRAY, 0);
		colmain_rounded_rect(gdb, x1 + 2, y1 + 2, x1 + bh->w - 2, y1 + bh->h - 3, button_round_radius, COLORPIP_BLACK, 0);
	}
	else
	{
		if (bh->state == DISABLED)
			bg = b1->bg_disabled;
		else if (bh->is_locked && bh->state == PRESSED)
			bg = b1->bg_locked_pressed;
		else if (bh->is_locked && bh->state != PRESSED)
			bg = b1->bg_locked;
		else if (! bh->is_locked && bh->state == PRESSED)
			bg = b1->bg_pressed;
		else if (! bh->is_locked && bh->state != PRESSED)
			bg = b1->bg_non_pressed;
		ASSERT(bg != NULL);

		gxdrawb_t bgv;
		gxdrawb_initialize(& bgv, bg, bh->w, bh->h);
		colpip_bitblt(
				gdb->cachebase, gdb->cachesize,	// cache parameters
				gdb, 	// target window
				x1, y1,	// target position
				bgv.cachebase, bgv.cachesize, 	// cache parameters
				& bgv, 0, 0, bh->w, bh->h, BITBLT_FLAG_NONE, 0);
	}

	const uint_fast16_t shiftX = bh->state == PRESSED ? 1 : 0;
	const uint_fast16_t shiftY = bh->state == PRESSED ? 1 : 0;
	const COLORPIP_T textcolor = COLORPIP_BLACK;

	if (strchr(bh->text, delimeters [0]) == NULL)
	{
		/* Однострочная надпись */
#if WITHALTERNATIVEFONTS
		UB_Font_DrawPString(gdb, shiftX + x1 + (bh->w - (getwidth_Pstring(bh->text, & FONT_BUTTONS))) / 2,
				shiftY + y1 + (bh->h - FONT_BUTTONS.height) / 2, bh->text, & FONT_BUTTONS, textcolor);
#else
		colpip_string2_tbg(gdb, shiftX + x1 + (bh->w - (strwidth2(bh->text))) / 2,
				shiftY + y1 + (bh->h - SMALLCHARH2) / 2, bh->text, textcolor);
#endif /* WITHALTERNATIVEFONTS */
	}
	else
	{
		char * next;
		/* Двухстрочная надпись */
		uint_fast8_t j = (bh->h - SMALLCHARH2 * 2) / 2;
		char buf [TEXT_ARRAY_SIZE];
		strcpy(buf, bh->text);
		char * text2 = strtok_r(buf, delimeters, & next);
#if WITHALTERNATIVEFONTS
		UB_Font_DrawPString(gdb,
				shiftX + x1 + (bh->w - (getwidth_Pstring(text2, & FONT_BUTTONS))) / 2,
				shiftY + y1 + j,
				text2, & FONT_BUTTONS, textcolor
				);

		text2 = strtok_r(NULL, delimeters, & next);
		UB_Font_DrawPString(gdb,
				shiftX + x1 + (bh->w - (getwidth_Pstring(text2, & FONT_BUTTONS))) / 2,
				shiftY + bh->h + y1 - FONT_BUTTONS.height - j,
				text2, & FONT_BUTTONS, textcolor
				);
#else
		colpip_string2_tbg(gdb, shift + x1 + (bh->w - (strwidth2(text2))) / 2,
				shift + y1 + j, text2, COLORPIP_BLACK);

		text2 = strtok_r(NULL, delimeters, & next);
		colpip_string2_tbg(gdb, shift + x1 + (bh->w - (strwidth2(text2))) / 2,
				shift + bh->h + y1 - SMALLCHARH2 - j, text2, COLORPIP_BLACK);
#endif /* WITHALTERNATIVEFONTS */
	}
}



// *************** Labels ***************

/* Получение ширины метки в пикселях  */
uint_fast8_t get_label_width(const label_t * const lh)
{
	if (lh->font_size == FONT_LARGE)
		return strlen(lh->text) * SMALLCHARW;
	else if (lh->font_size == FONT_MEDIUM)
		return strlen(lh->text) * SMALLCHARW2;
	else if (lh->font_size == FONT_SMALL)
		return strlen(lh->text) * SMALLCHARW3;
	return 0;
}

/* Получение высоты метки в пикселях  */
uint_fast8_t get_label_height(const label_t * const lh)
{
	if (lh->font_size == FONT_LARGE)
		return SMALLCHARH;
	else if (lh->font_size == FONT_MEDIUM)
		return SMALLCHARH2;
	else if (lh->font_size == FONT_SMALL)
		return SMALLCHARH3;
	return 0;
}

uint_fast8_t get_label_width2(const char * name)
{
	window_t * win = get_win(get_parent_window());
	label_t * lh = (label_t *) find_gui_obj(TYPE_LABEL, win, name);

	if (lh->font_size == FONT_LARGE)
		return strlen(lh->text) * SMALLCHARW;
	else if (lh->font_size == FONT_MEDIUM)
		return strlen(lh->text) * SMALLCHARW2;
	else if (lh->font_size == FONT_SMALL)
		return strlen(lh->text) * SMALLCHARW3;
	return 0;
}

uint_fast8_t get_label_height2(const char * name)
{
	window_t * win = get_win(get_parent_window());
	label_t * lh = (label_t *) find_gui_obj(TYPE_LABEL, win, name);

	if (lh->font_size == FONT_LARGE)
		return SMALLCHARH;
	else if (lh->font_size == FONT_MEDIUM)
		return SMALLCHARH2;
	else if (lh->font_size == FONT_SMALL)
		return SMALLCHARH3;
	return 0;
}

void label_set_coords(const char * name, uint16_t x, uint16_t y)
{
	window_t * win = get_win(get_parent_window());
	label_t * lh = (label_t *) find_gui_obj(TYPE_LABEL, win, name);
	lh->x = x;
	lh->y = y;
}

void label_set_pos_x(const char * name, uint16_t x)
{
	window_t * win = get_win(get_parent_window());
	label_t * lh = (label_t *) find_gui_obj(TYPE_LABEL, win, name);
	lh->x = x;
}

void label_set_pos_y(const char * name, uint16_t y)
{
	window_t * win = get_win(get_parent_window());
	label_t * lh = (label_t *) find_gui_obj(TYPE_LABEL, win, name);
	lh->y = y;
}

uint16_t label_get_pos_x(const char * name)
{
	window_t * win = get_win(get_parent_window());
	label_t * lh = (label_t *) find_gui_obj(TYPE_LABEL, win, name);
	return lh->x;
}

uint16_t label_get_pos_y(const char * name)
{
	window_t * win = get_win(get_parent_window());
	label_t * lh = (label_t *) find_gui_obj(TYPE_LABEL, win, name);
	return lh->y;
}

void label_set_visible(const char * name, uint8_t v)
{
	window_t * win = get_win(get_parent_window());
	label_t * lh = (label_t *) find_gui_obj(TYPE_LABEL, win, name);
	lh->visible = v != 0;
}

void label_set_text(const char * name, const char * text)
{
	window_t * win = get_win(get_parent_window());
	label_t * lh = (label_t *) find_gui_obj(TYPE_LABEL, win, name);
	strncpy(lh->text, text, TEXT_ARRAY_SIZE - 1);
}

// *************** Text fields ***************

/* Рассчитать размеры текстового поля */
void textfield_update_size(text_field_t * tf)
{
	ASSERT(tf != NULL);
	if (tf->font)
	{
		tf->w = tf->font->width * tf->w_sim;
		tf->h = tf->font->height * tf->h_str;
	}
	else
	{
		tf->w = SMALLCHARW2 * tf->w_sim;
		tf->h = SMALLCHARH2 * tf->h_str;
	}
	ASSERT(tf->w < WITHGUIMAXX);
	ASSERT(tf->h < WITHGUIMAXY - window_title_height);
}

/* Добавить строку в текстовое поле */
void textfield_add_string(text_field_t * tf, const char * str, COLORPIP_T color)
{
	ASSERT(tf != NULL);

	tf_entry_t * rec = &  tf->string [tf->index];
	strncpy(rec->text, str, TEXT_ARRAY_SIZE - 1);
	rec->color_line = color;
	tf->index ++;
	tf->index = tf->index >= tf->h_str ? 0 : tf->index;
}

/* Очистить текстовое поле */
void textfield_clean(text_field_t * tf)
{
	ASSERT(tf != NULL);
	tf->index = 0;
	memset(tf->string, 0, tf->h_str * sizeof(tf_entry_t));
}

#endif /* WITHTOUCHGUI */
