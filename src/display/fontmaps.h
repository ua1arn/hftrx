/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef DISPLAY_FONTMAPS_H_INCLUDED
#define DISPLAY_FONTMAPS_H_INCLUDED

extern const FLASHMEM uint8_t ILI9225_bigfont [13][5 * 24];
extern const FLASHMEM uint8_t ILI9225_halffont [12][5 * 14];
extern const FLASHMEM uint8_t ILI9225_smallfont [][2 * 8];
extern const FLASHMEM uint8_t ILI9320_bigfont [13][4 * 18];
extern const FLASHMEM uint8_t ILI9320_halffont [12][4 * 14];
extern const FLASHMEM uint8_t ILI9320_smallfont [][16];
extern const FLASHMEM uint8_t ILI9341_bigfont [][120];
extern const FLASHMEM uint8_t ILI9341_halffont [12][5 * 14];
extern const FLASHMEM uint8_t ILI9341_smallfont [][20];
extern const FLASHMEM uint8_t ls020_bigfont [13][4 * 18];
extern const FLASHMEM uint8_t ls020_halffont [12][4 * 14];
extern const FLASHMEM uint8_t ls020_smallfont [][16];
extern const FLASHMEM uint8_t S1D13781_bigfont_LTDC [13][56][5];
extern const FLASHMEM uint8_t S1D13781_halffont_LTDC [12][56][4];
extern const FLASHMEM uint8_t S1D13781_smallfont_LTDC [][15][2];
extern const FLASHMEM uint8_t S1D13781_smallfont2_LTDC [][16][2];
extern const FLASHMEM uint8_t S1D13781_smallfont3_LTDC [][8];

#if WITHALTERNATIVEFONTS
extern const FLASHMEM uint8_t ltdc_CenturyGothic_big [13][56][5];
extern const FLASHMEM uint8_t ltdc_CenturyGothic_half [12][56][4];
#endif /* WITHALTERNATIVEFONTS */

//
#if LCDMODE_LTDC

	#define SMALLCHARH 15 /* Font height */
	#define SMALLCHARW 16 /* Font width */
	#define SMALLCHARH2 16 /* Font height */
	#define SMALLCHARW2 10 /* Font width */
	#define SMALLCHARH3 8 /* Font height */
	#define SMALLCHARW3 8 /* Font width */

#else /* LCDMODE_LTDC */

	#if FONTSTYLE_ITALIC
		extern const FLASHMEM uint8_t uc1601s_bigfont [13][2][12];
		extern const FLASHMEM uint8_t uc1601s_halffont [12][2][12];
	#else /* FONTSTYLE_ITALIC */
		extern const FLASHMEM uint8_t uc1601s_bigfont [13][2][12];
		extern const FLASHMEM uint8_t uc1601s_halffont [12][2][10];
	#endif /* FONTSTYLE_ITALIC */
	extern const FLASHMEM uint8_t uc1601s_font [] [6];

	extern const FLASHMEM uint8_t uc1608_bigfont [13][5][24];
	extern const FLASHMEM uint8_t uc1608_halffont [12][5][14];
	extern const FLASHMEM uint8_t uc1608_smallfont [][2][8];

	extern const FLASHMEM uint8_t uc1608_bigfont [13][5][24];
	extern const FLASHMEM uint8_t uc1608_halffont [12][5][14];
	extern const FLASHMEM uint8_t uc1608_smallfont [][2][8];

#endif /* LCDMODE_LTDC */

#if  LCDMODE_ST7735 || LCDMODE_ILI9163 || LCDMODE_ILI9341

	#if DSTYLE_G_X320_Y240
		// LCDMODE_ILI9341

		// Для дисплеев 320 * 240
		#include "./fonts/ILI9341_font_small.h"
		#include "./fonts/ILI9341_font_half.h"
		#include "./fonts/ILI9341_font_big.h"

		#define    ls020_smallfont    ILI9341_smallfont
		#define    ls020_halffont    ILI9341_halffont
		#define    ls020_bigfont    ILI9341_bigfont


		#define BIGCHARH xx40	// st7735
		#define SMALLCHARH xx40	// st7735


	#else /* DSTYLE_G_X320_Y240 */
		// LCDMODE_ST7735 - 160 * 128
		// LCDMODE_ILI9163 - 176 * 132

		//#include "./fonts/ls020_font_small.h"
		//#include "./fonts/ls020_font_half.h"
		//#include "./fonts/ls020_font_big.h"
		#define BIGCHARH 32 /* Font height */
		#define SMALLCHARH 16 /* Font height */

	#endif /* DSTYLE_G_X320_Y240 */

#else

#endif

#if FONTSHERE

#if LCDMODE_DUMMY || LCDMODE_HD44780
	// Дисплеи, не требующие растров знакогенераторов

#elif LCDMODE_UC1601

	/* растровые шрифты */
	#include "./fonts/uc1601s_font_small.h"

	#if FONTSTYLE_ITALIC
		#include "./fonts/uc1601s_ifont_half.h"
		#include "./fonts/uc1601s_ifont_big.h"
	#else /* FONTSTYLE_ITALIC */
		#include "./fonts/uc1601s_font_half.h"
		#include "./fonts/uc1601s_font_big.h"
	#endif /* FONTSTYLE_ITALIC */

#elif DSTYLE_G_X176_Y132

	#include "./fonts/ls020_font_small.h"
	#include "./fonts/ls020_font_half.h"
	#include "./fonts/ls020_font_big.h"

#elif DSTYLE_G_X320_Y240
	// в знакогенераторе изображения символов "по вертикалти"
	// Для дисплеев 320 * 240
	#include "./fonts/ILI9341_font_small.h"
	#include "./fonts/ILI9341_font_half.h"
	#include "./fonts/ILI9341_font_big.h"
	/* шрифты для работы с буером в памяти */
	#include "./fonts/S1D13781_font_small3_LTDC.h"
	#include "./fonts/S1D13781_font_small2_LTDC.h"

	#define	ILI9320_smallfont	ILI9341_smallfont
	#define	ILI9320_halffont	ILI9341_halffont
	#define	ILI9320_bigfont		ILI9341_bigfont

	#define	ls020_smallfont	ILI9341_smallfont
	#define	ls020_halffont	ILI9341_halffont
	#define	ls020_bigfont	ILI9341_bigfont

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

#elif DSTYLE_G_X800_Y480
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

#elif DSTYLE_G_X240_Y128

	#include "./fonts/uc1601s_font_small.h"	// в одну полосу- small2

	#if FONTSTYLE_ITALIC
		#include "./fonts/uc1608_font_small.h"
		#include "./fonts/uc1608_font_half.h"
		#include "./fonts/uc1608_font_big.h"
	#else
		#include "./fonts/uc1608_sphm_font_small.h"
		#include "./fonts/uc1608_sphm_font_half.h"
		#include "./fonts/uc1608_sphm_font_big.h"
	#endif

#elif LCDMODE_ST7735 || LCDMODE_ILI9163 || LCDMODE_ILI9341

	#if DSTYLE_G_X320_Y240
		// LCDMODE_ILI9341

		// Для дисплеев 320 * 240
		#include "./fonts/ILI9341_font_small.h"
		#include "./fonts/ILI9341_font_half.h"
		#include "./fonts/ILI9341_font_big.h"

		#define    ls020_smallfont    ILI9341_smallfont
		#define    ls020_halffont    ILI9341_halffont
		#define    ls020_bigfont    ILI9341_bigfont


	#else /* DSTYLE_G_X320_Y240 */
		// LCDMODE_ST7735 - 160 * 128
		// LCDMODE_ILI9163 - 176 * 132

		#include "./fonts/ls020_font_small.h"
		#include "./fonts/ls020_font_half.h"
		#include "./fonts/ls020_font_big.h"

	#endif /* DSTYLE_G_X320_Y240 */

#elif DSTYLE_G_X220_Y176 || DSTYLE_G_X160_Y128
	// Индикатор 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 с контроллером ILI9225С

	#include "./fonts/ILI9225_font_small.c"
	#include "./fonts/ILI9225_font_half.c"
	#include "./fonts/ILI9225_font_big.c"

#else /*  */
	// в знакогенераторе изображения символов "по вертикалти"
	//#error Undefined display layout

	#if FONTSTYLE_ITALIC
		#include "./fonts/uc1601s_ifont_half.h"
		#include "./fonts/uc1601s_ifont_big.h"
	#else /* FONTSTYLE_ITALIC */
		#include "./fonts/uc1601s_font_half.h"
		#include "./fonts/uc1601s_font_big.h"
	#endif /* FONTSTYLE_ITALIC */

	#include "./fonts/ls020_font_small.h"
#endif /* DSTYLE_G_X320_Y240 */

#endif /* FONTSHERE */

#endif /* DISPLAY_FONTMAPS_H_INCLUDED */
