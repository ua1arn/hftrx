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
colmain_setcolors(COLORPIP_T fg, COLORPIP_T bg)
{
}

uint_fast16_t display_wrdata_begin(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast16_t * yp)
{
	* yp = GRID2Y(y);
	return GRID2X(x);
}

void
display_wrdata_end(const gxdrawb_t * db)
{
}

uint_fast16_t display_wrdatabar_begin(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast16_t * yp)
{
	* yp = GRID2Y(y);
	return GRID2X(x);
}

void
display_wrdatabar_end(const gxdrawb_t * db)
{
}


/* вызывается между вызовами display_wrdatabig_begin() и display_wrdatabig_end() */
uint_fast16_t
display_put_char_big_tbg(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char c)
{
	return xpix + 1;
}

/* вызывается между вызовами display_wrdatabig_begin() и display_wrdatabig_end() */
uint_fast16_t
display_put_char_half_tbg(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char c)
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


// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
uint_fast16_t
display_put_char_small2(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc)
{
	(void) cc;
	return xpix;
}

#endif /* LCDMODE_DUMMY */


