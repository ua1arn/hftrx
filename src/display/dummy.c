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

void display_set_contrast(uint_fast8_t v)
{
}

void
display_clear(void)
{
}

void
colmain_setcolors(COLORMAIN_T fg, COLORMAIN_T bg)
{
}

void colmain_setcolors3(COLORMAIN_T fg, COLORMAIN_T bg, COLORMAIN_T fgbg)
{
}

uint_fast16_t display_wrdata_begin(uint_fast8_t x, uint_fast8_t y, uint_fast16_t * yp)
{
	* yp = GRID2Y(y);
	return GRID2X(x);
}

void
display_wrdata_end(void)
{
}

uint_fast16_t display_wrdatabar_begin(uint_fast8_t x, uint_fast8_t y, uint_fast16_t * yp)
{
	* yp = GRID2Y(y);
	return GRID2X(x);
}

void
display_wrdatabar_end(void)
{
}

/* отображение одной вертикальной полосы на графическом индикаторе */
/* старшие биты соответствуют верхним пикселям изображения */
/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
uint_fast16_t
display_barcolumn(uint_fast16_t xpix, uint_fast16_t ypix, uint_fast8_t pattern)
{
	return xpix + 1;
}

/* вызывается между вызовами display_wrdatabig_begin() и display_wrdatabig_end() */
uint_fast16_t
display_put_char_big(uint_fast16_t x, uint_fast16_t y, uint_fast8_t c, uint_fast8_t lowhalf)
{
	return x + 1;
}

/* вызывается между вызовами display_wrdatabig_begin() и display_wrdatabig_end() */
uint_fast16_t
display_put_char_half(uint_fast16_t x, uint_fast16_t y, uint_fast8_t c, uint_fast8_t lowhalf)
{
	return x + 1;
}


// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
uint_fast16_t
display_put_char_small(uint_fast16_t x, uint_fast16_t y, uint_fast8_t c, uint_fast8_t lowhalf)
{
	return x + 1;
}

// Координаты в пикселях
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
}

void display_plotstart(
	uint_fast16_t dy	// Высота окна в пикселях
	)
{
}

void display_plotstop(void)
{
}

void display_plot(
	const PACKEDCOLORMAIN_T * buffer,
	uint_fast16_t dx,	// Размеры окна в пикселях
	uint_fast16_t dy,
	uint_fast16_t xpix,	// начало области рисования
	uint_fast16_t ypix
	)
{

}


/* аппаратный сброс дисплея - перед инициализаций */
/* вызывается при разрешённых прерываниях. */
void
display_reset(void)
{
}

/* Разряжаем конденсаторы питания */
void display_discharge(void)
{
}

void
display_wrdata2_end(void)
{
}


// большие и средние цифры (частота)
// большие и средние цифры (частота)
uint_fast16_t display_wrdatabig_begin(uint_fast8_t x, uint_fast8_t y, uint_fast16_t * yp)
{
	* yp = GRID2Y(y);
	return GRID2X(x);
}


void
display_wrdatabig_end(void)
{
}

// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
uint_fast16_t
display_put_char_small2(uint_fast16_t x, uint_fast16_t y, uint_fast8_t c, uint_fast8_t lowhalf)
{
	(void) lowhalf;
	(void) c;
	return x;
}

#endif /* LCDMODE_DUMMY */


