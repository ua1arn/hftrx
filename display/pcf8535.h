/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//



#ifndef PCF8535_H_INCLUDED
#define PCF8535_H_INCLUDED

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if LCDMODE_PCF8535 || LCDMODE_PCF8531

#define DISPLAYMODES_FPS 4	/* количество обновлений отображения режимов работы */
#define DISPLAY_FPS	4	/* обновление показаний частоты за секунду */
#define DISPLAYSWR_FPS 4	/* количество обновлений SWR за секунду */



// Цветное изображение не поддерживается на этом дисплее (заглушка).
#define TFTRGB(r, g, b)	(0)
typedef uint_fast8_t COLOR_T;	/* цвета не поддерживаются - заглушка */
typedef uint8_t PACKEDCOLOR_T;	/* цвета не поддерживаются - заглушка */

typedef PACKEDCOLOR_T PACKEDCOLORPIP_T;
typedef COLOR_T COLORPIP_T;

#endif /* LCDMODE_PCF8535 || LCDMODE_PCF8531 */

#endif /* PCF8535_H_INCLUDED */

