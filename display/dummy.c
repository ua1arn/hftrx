/*
 * dummy.c
 *
 *  Created on: Oct 25, 2019
 *      Author: gena
 */



/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Siemens S65 Display Control
/* Индикатор 176*132 с контроллером Epson L2F50126 */

#include "hardware.h"
#include "board.h"

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
display_setcolors(COLOR_T fg, COLOR_T bg)
{
}

void display_setcolors3(COLOR_T fg, COLOR_T bg, COLOR_T fgbg)
{
}

void
display_wrdata_begin(void)
{
}

void
display_wrdata_end(void)
{
}

void
display_wrdatabar_begin(void)
{
}

void
display_wrdatabar_end(void)
{
}


void
display_wrdatabig_begin(void)
{
}


void
display_wrdatabig_end(void)
{
}

/* отображение одной вертикальной полосы на графическом индикаторе */
/* старшие биты соответствуют верхним пикселям изображения */
/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
void
display_barcolumn(uint_fast8_t pattern)
{
}

/* вызывается между вызовами display_wrdatabig_begin() и display_wrdatabig_end() */
void
display_put_char_big(uint_fast8_t c, uint_fast8_t lowhalf)
{
}

/* вызывается между вызовами display_wrdatabig_begin() и display_wrdatabig_end() */
void
display_put_char_half(uint_fast8_t c, uint_fast8_t lowhalf)
{
}


// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
void
display_put_char_small(uint_fast8_t c, uint_fast8_t lowhalf)
{
}

void
display_gotoxy(uint_fast8_t x, uint_fast8_t y)
{
}

// Координаты в пикселях
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
}

void display_plotstart(
	uint_fast16_t height	// Высота окна в пикселях
	)
{
}

void display_plotstop(void)
{
}

void display_plot(
	const PACKEDCOLOR_T * buffer,
	uint_fast16_t dx,	// Размеры окна в пикселях
	uint_fast16_t dy
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

#endif /* LCDMODE_DUMMY */


