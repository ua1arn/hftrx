/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef HD44780_H_INCLUDED
#define HD44780_H_INCLUDED

#include "hardware.h"

#if LCDMODE_HD44780

#define CHARS2GRID(columns) ((columns) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */
#define ROWS2GRID(rows) ((rows) * 1)		/* перевести количество символов в ячейкт сетки разметки отображния */

#if 0

	#define LCDMODE_SMETER3	1	/* на каждом знакоместе три градации уровня */
	#define GRID2X(cellsx) ((cellsx) * 3)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */

#else

	#define LCDMODE_SMETER2	1	/* на каждом знакоместе две градации уровня */
	#define GRID2X(cellsx) ((cellsx) * 2)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */

#endif


#define GRID2Y(cellsy) ((cellsy) * 1)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */

#define DISPLAYMODES_FPS 5	/* количество обновлений отображения режимов работы */
#define DISPLAY_FPS	10	/* обновление показаний частоты за секунду */
#define DISPLAYSWR_FPS 5	/* количество обновлений SWR за секунду */

// Цветное изображение не поддерживается на этом дисплее (заглушка).
#define TFTRGB(r, g, b)	(0)
typedef uint_fast8_t COLORMAIN_T;	/* цвета не поддерживаются - заглушка */
typedef uint8_t PACKEDCOLORMAIM_T;


typedef PACKEDCOLORMAIM_T PACKEDCOLORPIP_T;
typedef COLORMAIN_T COLORPIP_T;

// начальные коды символов для отображения псевдографики
#define PATTERN_BAR_FULL 4	/* в случае двух градаций может быть 3 - и будет место ещё для двух специсмволов */
#define PATTERN_BAR_HALF 0
#define PATTERN_BAR_EMPTYHALF 0 // этот код не используется - для совместимости с интерфейсом графических индикаторов
#define PATTERN_BAR_EMPTYFULL 0	// этот код не используется - для совместимости с интерфейсом графических индикаторов
#define PATTERN_SPACE	0x20	/* очищаем место за SWR и PWR метром этим символом */

void hd44780_io_initialize(void);	/* настройка паралельного интерфейса работы с дисплеем, вызывается при запрещённых прерываниях. */


#endif /* LCDMODE_HD44780 */

#endif /* HD44780_H_INCLUDED */
