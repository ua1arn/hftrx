/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef DISPLAY_FONTMAPS_H_INCLUDED
#define DISPLAY_FONTMAPS_H_INCLUDED

extern const FLASHMEM uint8_t ILI9225_bigfont [13][5 * 24];
extern const FLASHMEM uint8_t ILI9225_halffont [13][5 * 14];
extern const FLASHMEM uint8_t ILI9225_smallfont [][2 * 8];
extern const FLASHMEM uint8_t ILI9320_bigfont [13][4 * 18];
extern const FLASHMEM uint8_t ILI9320_halffont [13][4 * 14];
extern const FLASHMEM uint8_t ILI9320_smallfont [][16];
extern const FLASHMEM uint8_t ILI9341_bigfont [][120];
extern const FLASHMEM uint8_t ILI9341_halffont [13][5 * 14];
extern const FLASHMEM uint8_t ILI9341_smallfont [][20];
extern const FLASHMEM uint8_t ls020_bigfont [13][4 * 18];
extern const FLASHMEM uint8_t ls020_halffont [13][4 * 14];
extern const FLASHMEM uint8_t ls020_smallfont [][16];
extern const FLASHMEM uint8_t S1D13781_bigfont_LTDC [13][56][5];
extern const FLASHMEM uint8_t S1D13781_halffont_LTDC [13][56][4];
extern const FLASHMEM uint8_t S1D13781_smallfont_LTDC [][15][2];
extern const FLASHMEM uint8_t S1D13781_smallfont2_LTDC [][16][2];
extern const FLASHMEM uint8_t S1D13781_smallfont3_LTDC [][8];

#if WITHALTERNATIVEFONTS
extern const FLASHMEM uint8_t ltdc_CenturyGothic_big [13][56][5];
extern const FLASHMEM uint8_t ltdc_CenturyGothic_half [13][56][4];
#endif /* WITHALTERNATIVEFONTS */

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

#elif DSTYLE_G_X480_Y272
	// в знакогенераторе изображения символов "по горизонтали"
	#include "./fonts/S1D13781_font_small3_LTDC.h"
	#include "./fonts/S1D13781_font_small2_LTDC.h"
	#include "./fonts/S1D13781_font_small_LTDC.h"
	#include "./fonts/S1D13781_font_half_LTDC.h"
	#include "./fonts/S1D13781_font_big_LTDC.h"
	#if WITHALTERNATIVEFONTS
		#include "./fonts/ltdc_CenturyGothic_big.h"
		#include "./fonts/ltdc_CenturyGothic_half.h"
	#endif /* WITHALTERNATIVEFONTS */

#elif DSTYLE_G_X800_Y480 || DSTYLE_G_X1024_Y600 || DSTYLE_G_X1280_Y720
	// в знакогенераторе изображения символов "по горизонтали"
	#include "./fonts/S1D13781_font_small3_LTDC.h"
	#include "./fonts/S1D13781_font_small2_LTDC.h"
	#include "./fonts/S1D13781_font_small_LTDC.h"
	#include "./fonts/S1D13781_font_half_LTDC.h"
	#include "./fonts/S1D13781_font_big_LTDC.h"
	#if WITHALTERNATIVEFONTS
		#include "./fonts/ltdc_CenturyGothic_big.h"
		#include "./fonts/ltdc_CenturyGothic_half.h"
	#endif /* WITHALTERNATIVEFONTS */

#endif /* DSTYLE_G_X320_Y240 */

#endif /* FONTSHERE */

#endif /* DISPLAY_FONTMAPS_H_INCLUDED */
