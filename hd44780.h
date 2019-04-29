/* $Id$ */
//
// ѕроект HF Dream Receiver ( ¬ приЄмник мечты)
// автор √ена «авидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef HD44780_H_INCLUDED
#define HD44780_H_INCLUDED

#include "hardware.h"

#if LCDMODE_HD44780

#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в €чейкт сетки разметки отображни€ */
#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в €чейкт сетки разметки отображни€ */

#if 0

	#define LCDMODE_SMETER3	1	/* на каждом знакоместе три градации уровн€ */
	#define GRID2X(cellsx) ((cellsx) * 3)	/* перевод €чеек сетки разметки в номер пиксел€ по горизонталм */

#else

	#define LCDMODE_SMETER2	1	/* на каждом знакоместе две градации уровн€ */
	#define GRID2X(cellsx) ((cellsx) * 2)	/* перевод €чеек сетки разметки в номер пиксел€ по горизонталм */

#endif


#define GRID2Y(cellsy) ((cellsy) * 1)	/* перевод €чеек сетки разметки в номер пиксел€ по вертикали */

#define DISPLAYMODES_FPS 5	/* количество обновлений отображени€ режимов работы */
#define DISPLAY_FPS	10	/* обновление показаний частоты за секунду */
#define DISPLAYSWR_FPS 5	/* количество обновлений SWR за секунду */

// ÷ветное изображение не поддерживаетс€ на этом дисплее (заглушка).
#define TFTRGB(r, g, b)	(0)
typedef uint_fast8_t COLOR_T;	/* цвета не поддерживаютс€ - заглушка */
typedef uint8_t PACKEDCOLOR_T;


// начальные коды символов дл€ отображени€ псевдографики
#define PATTERN_BAR_FULL 4	/* в случае двух градаций может быть 3 - и будет место ещЄ дл€ двух специсмволов */
#define PATTERN_BAR_HALF 0
#define PATTERN_BAR_EMPTYHALF 0 // этот код не используетс€ - дл€ совместимости с интерфейсом графических индикаторов
#define PATTERN_BAR_EMPTYFULL 0	// этот код не используетс€ - дл€ совместимости с интерфейсом графических индикаторов
#define PATTERN_SPACE	0x20	/* очищаем место за SWR и PWR метром этим символом */

void hd44780_io_initialize(void);	/* настройка паралельного интерфейса работы с дисплеем, вызываетс€ при запрещЄнных прерывани€х. */


#endif /* LCDMODE_HD44780 */

#endif /* HD44780_H_INCLUDED */
