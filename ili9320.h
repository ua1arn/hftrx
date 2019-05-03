/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// TFT панель 320 * 240 ADI_3.2_AM-240320D4TOQW-T00H(R)
// Только работает медленно.

#ifndef ILI9320_H_INCLUDED
#define ILI9320_H_INCLUDED

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if LCDMODE_ILI9320

#define DISPLAYMODES_FPS 5	/* количество обновлений отображения режимов работы */
#define DISPLAY_FPS	10	/* обновление показаний частоты за секунду */
#define DISPLAYSWR_FPS 5	/* количество обновлений SWR за секунду */

typedef uint_fast16_t COLOR_T;
typedef uint16_t PACKEDCOLOR_T;

// RRRR.RGGG.GGGB.BBBB
#define TFTRGB(red, green, blue) \
	(  (uint_fast16_t) \
		(	\
			(((uint_fast16_t) (red) << 8) &   0xf800)  | \
			(((uint_fast16_t) (green) << 3) & 0x07e0) | \
			(((uint_fast16_t) (blue) >> 3) &  0x001f) \
		) \
	)


#endif /* LCDMODE_ILI9320 */
#endif /* ILI9320_H_INCLUDED */

