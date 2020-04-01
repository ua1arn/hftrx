/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef ILI8961_H_INCLUDED
#define ILI8961_H_INCLUDED

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if LCDMODE_ILI8961

	//#define DISPLAYMODES_FPS 5	/* количество обновлений отображения режимов работы */
	//#define DISPLAY_FPS	10	/* обновление показаний частоты десять раз в секунду */
	//#define DISPLAYSWR_FPS 5	/* количество обновлений SWR за секунду */

	#if LCDMODE_LTDC

		// Определения COLORMAIN_T, PACKEDCOLORMAIM_T и TFTRGB используются из diaplay.h - данный контроллер работает только в ржиме LCDMODE_LTDC

	#else /* LCDMODE_LTDC */

		#error Use LCDMODE_LTDC

	#endif /* LCDMODE_LTDC */

#define LCDMODE_COLORED	1

#endif /* LCDMODE_ILI8961 */

#endif /* ILI8961_H_INCLUDED */

