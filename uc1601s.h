/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef UC1601S_H_INCLUDED
#define UC1601S_H_INCLUDED

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if LCDMODE_UC1601

#define DISPLAYMODES_FPS 5	/* количество обновлений отображения режимов работы */
#define DISPLAY_FPS	10	/* обновление показаний частоты за секунду */
#define DISPLAYSWR_FPS 10	/* количество обновлений SWR за секунду */

// Цветное изображение не поддерживается на этом дисплее (заглушка).
#define TFTRGB(r, g, b)	(0)
typedef uint_fast8_t COLOR_T;	/* цвета не поддерживаются - заглушка */
typedef uint8_t PACKEDCOLOR_T;	/* цвета не поддерживаются - заглушка */

#endif /* LCDMODE_UC1601 */

#endif /* UC1601S_H_INCLUDED */
