/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
/* Индикатор 176*132 Sharp LS020B8UD06 с контроллером LR38826 */


#ifndef LS020_H_INCLUDED
#define LS020_H_INCLUDED

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if LCDMODE_LS020

#define DISPLAYMODES_FPS 5	/* количество обновлений отображения режимов работы */
#define DISPLAY_FPS	10	/* обновление показаний частоты за секунду */
#define DISPLAYSWR_FPS 5	/* количество обновлений SWR за секунду */

// 132 x 176 pixel
/*
	Active Display Area 31.284 x 41.712 mmІ (W x H)
	Blackmatrix area 34,2 x 44,9 mmІ (W x H)
	Outline Dimensions panel 35.9 x 52,0 mmІ (W x H)
	Design Viewing Direction 6 o’clock display
	Controller: 12 o’clock of the display module
*/

typedef uint_fast8_t COLOR_T;
typedef uint8_t PACKEDCOLOR_T;


// RRRGGGBB
#define TFTRGB(red, green, blue) \
	(  (unsigned char) \
		(	\
			(((red) >> 0) & 0xe0)  | \
			(((green) >> 3) & 0x1c) | \
			(((blue) >> 6) & 0x03) \
		) \
	)

typedef PACKEDCOLOR_T PACKEDCOLORPIP_T;
typedef COLOR_T COLORPIP_T;

#endif /* LCDMODE_LS020 */

#endif /* LS020_H_INCLUDED */

