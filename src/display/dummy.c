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

#endif /* LCDMODE_DUMMY */


