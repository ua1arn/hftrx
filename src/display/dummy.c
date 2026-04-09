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

void pix_display_texts(const gxdrawb_t * db, uint_fast16_t xpixB, uint_fast16_t ypix, uint_fast16_t w, uint_fast16_t h, const gxstyle_t * dbstylep, const char * const * slines, unsigned nlines)
{
}

#endif /* LCDMODE_DUMMY */


