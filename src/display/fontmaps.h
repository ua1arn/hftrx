/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef DISPLAY_FONTMAPS_H_INCLUDED
#define DISPLAY_FONTMAPS_H_INCLUDED

//extern const uint8_t S1D13781_smallfont_LTDC [];
//extern const uint8_t S1D13781_smallfont_RU_LTDC [];
extern const uint8_t S1D13781_smallfont2_LTDC [];
extern const uint8_t S1D13781_smallfont3_LTDC [];

//extern const uint8_t S1D13781_bigfont_LTDC [];
//extern const uint8_t S1D13781_halffont_LTDC [];
extern const uint8_t ltdc_CenturyGothic_big [];
extern const uint8_t ltdc_CenturyGothic_half [];

//
#if LCDMODE_LTDC

	#define SMALLCHARH 15 /* Font height */
	#define SMALLCHARW 16 /* Font width */
	#define SMALLCHARH2 16 /* Font height */
	#define SMALLCHARW2 10 /* Font width */
	#define SMALLCHARH3 8 /* Font height */
	#define SMALLCHARW3 8 /* Font width */

#endif /* LCDMODE_LTDC */

#if FONTSHERE

#if LCDMODE_DUMMY
	// Дисплеи, не требующие растров знакогенераторов
#else
	// в знакогенераторе изображения символов "по горизонтали"
	#include "./fonts/S1D13781_font_small3_LTDC.h"
	#include "./fonts/S1D13781_font_small2_LTDC.h"
#if WITHALTERNATIVEFONTS
	#include "./fonts/ltdc_CenturyGothic_big.h"
	#include "./fonts/ltdc_CenturyGothic_half.h"
#else /* WITHALTERNATIVEFONTS */
	#include "./fonts/S1D13781_font_half_LTDC.h"
	#include "./fonts/S1D13781_font_big_LTDC.h"
#endif /* WITHALTERNATIVEFONTS */

#endif

#endif /* FONTSHERE */

#endif /* DISPLAY_FONTMAPS_H_INCLUDED */
