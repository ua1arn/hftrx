/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Без вывода изображения

#include "hardware.h"
#include "board.h"
#include "display.h"

#if LCDMODE_DUMMY

/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{
}

// Заполниить цветом фона
void
display_clear(const gxdrawb_t * db)
{
}

void
gxstyle_textcolor(gxstyle_t * dbstyle, COLORPIP_T fg, COLORPIP_T bg)
{
	dbstyle->textfg = fg;
	dbstyle->textbg = bg;
}

uint_fast16_t display_wrdata_begin(uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp)
{
	* yp = 0;
	return 0;
}


uint_fast16_t
display_put_char_big(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc, COLORPIP_T fg)
{
	return xpix + 1;
}

uint_fast16_t
display_put_char_half(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc, COLORPIP_T fg)
{
	return xpix + 1;
}


/* аппаратный сброс дисплея - перед инициализаций */
/* вызывается при разрешённых прерываниях. */
void
display_reset(void)
{
}

/* Разряжаем конденсаторы питания */
void display_uninitialize(void)
{
}

void gxstyle_initialize(gxstyle_t * dbstyle)
{
	gxstyle_texthalign(dbstyle, GXSTYLE_HALIGN_RIGHT);
	gxstyle_textvalign(dbstyle, GXSTYLE_VALIGN_CENTER);
}

void gxstyle_texthalign(gxstyle_t * dbstyle, enum gxstyle_texthalign a)
{
	dbstyle->texthalign = a;
}

void gxstyle_textvalign(gxstyle_t * dbstyle, enum gxstyle_textvalign a)
{
	dbstyle->textvalign = a;
}

uint_fast16_t gxstyle_strwidth(const gxstyle_t * dbstyle, const char * s)
{
	return 0;
}
void gxstyle_setsmallfont(gxstyle_t * dbstyle)
{
}
void gxstyle_setsmallfont2(gxstyle_t * dbstyle)
{
}

#endif /* LCDMODE_DUMMY */


