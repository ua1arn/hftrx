/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef UC1608_H_INCLUDED
#define UC1608_H_INCLUDED

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if LCDMODE_UC1608

	#define DISPLAYMODES_FPS 5	/* количество обновлений отображения режимов работы */
	#define DISPLAY_FPS	10	/* обновление показаний частоты десять раз в секунду */
	#define DISPLAYSWR_FPS 5	/* количество обновлений SWR за секунду */

	// Цветное изображение не поддерживается на этом дисплее (заглушка).
	#define TFTRGB(r, g, b)	(0)
	typedef uint_fast8_t COLORMAIN_T;	/* цвета не поддерживаются - заглушка */
	typedef uint8_t PACKEDCOLORMAIN_T;	/* цвета не поддерживаются - заглушка */

	#define TFTRGB565 TFTRGB

#endif /* LCDMODE_UC1608 */

#endif /* UC1608_H_INCLUDED */
