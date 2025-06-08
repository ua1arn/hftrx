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

void colmain_setcolors3(COLORPIP_T fg, COLORPIP_T bg, COLORPIP_T fgbg)
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

/* отображение одной вертикальной полосы на графическом индикаторе */
/* старшие биты соответствуют верхним пикселям изображения */
/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
uint_fast16_t
display_barcolumn(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, uint_fast8_t pattern)
{
	return xpix + 1;
}

/* вызывается между вызовами display_wrdatabig_begin() и display_wrdatabig_end() */
uint_fast16_t
display_put_char_big(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char c, uint_fast8_t lowhalf)
{
	return xpix + 1;
}

/* вызывается между вызовами display_wrdatabig_begin() и display_wrdatabig_end() */
uint_fast16_t
display_put_char_half(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char c, uint_fast8_t lowhalf)
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

void
display_wrdata2_end(const gxdrawb_t * db)
{
}


// большие и средние цифры (частота)
// большие и средние цифры (частота)
uint_fast16_t display_wrdatabig_begin(const gxdrawb_t * db, uint_fast8_t x, uint_fast8_t y, uint_fast16_t * yp)
{
	* yp = GRID2Y(y);
	return GRID2X(x);
}


void
display_wrdatabig_end(const gxdrawb_t * db)
{
}

// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
uint_fast16_t
display_put_char_small2(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc, uint_fast8_t lowhalf)
{
	(void) lowhalf;
	(void) cc;
	return xpix;
}

#endif /* LCDMODE_DUMMY */


