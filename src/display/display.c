/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Доработки для LS020 Василий Линывый, livas60@mail.ru
//

#include "src/gui/gui.h"
#include "hardware.h"
#include "board.h"
#include "display.h"
#include "formats.h"
#include "spi.h"	// hardware_spi_master_send_frame
#include "display2.h"
#include <string.h>

#if LCDMODE_LTDC

#include "fontmaps.h"
//
//#if ! LCDMODE_LTDC_L24
//#include "./byte2crun.h"
//#endif /* ! LCDMODE_LTDC_L24 */

typedef PACKEDCOLORMAIN_T FRAMEBUFF_T [LCDMODE_MAIN_PAGES] [GXSIZE(DIM_SECOND, DIM_FIRST)];

#if defined (SDRAM_BANK_ADDR) && LCDMODE_LTDCSDRAMBUFF && LCDMODE_LTDC
	#define framebuff (* (FRAMEBUFF_T *) SDRAM_BANK_ADDR)
#else /* defined (SDRAM_BANK_ADDR) && LCDMODE_LTDCSDRAMBUFF && LCDMODE_LTDC */
	//#define framebuff (framebuff0)
	//extern FRAMEBUFF_T framebuff0;	//L8 (8-bit Luminance or CLUT)
#endif /* defined (SDRAM_BANK_ADDR) && LCDMODE_LTDCSDRAMBUFF && LCDMODE_LTDC */

#if ! defined (SDRAM_BANK_ADDR) && LCDMODE_MAIN_PAGES == 3
	// буфер экрана
	//RAMFRAMEBUFF ALIGNX_BEGIN FRAMEBUFF_T framebuff0 ALIGNX_END;
	RAMFRAMEBUFF ALIGNX_BEGIN PACKEDCOLORMAIN_T fbf0 [GXSIZE(DIM_SECOND, DIM_FIRST)] ALIGNX_END;
	RAMFRAMEBUFF ALIGNX_BEGIN PACKEDCOLORMAIN_T fbf1 [GXSIZE(DIM_SECOND, DIM_FIRST)] ALIGNX_END;
	RAMFRAMEBUFF ALIGNX_BEGIN PACKEDCOLORMAIN_T fbf2 [GXSIZE(DIM_SECOND, DIM_FIRST)] ALIGNX_END;
	static PACKEDCOLORMAIN_T * const fbfs [LCDMODE_MAIN_PAGES] =
	{
			fbf0, fbf1, fbf2,
	};

	static uint_fast8_t mainphase;

	void colmain_fb_next(void)
	{
		mainphase = (mainphase + 1) % LCDMODE_MAIN_PAGES;
	}

	PACKEDCOLORMAIN_T *
	colmain_fb_draw(void)
	{
		return fbfs [(mainphase + 1) % LCDMODE_MAIN_PAGES];
	}

	PACKEDCOLORMAIN_T *
	colmain_fb_show(void)
	{
		return fbfs [(mainphase + 0) % LCDMODE_MAIN_PAGES];
	}

#elif WITHSDRAMHW && LCDMODE_LTDCSDRAMBUFF

	void colmain_fb_next(void)
	{
	}

	PACKEDCOLORMAIN_T *
	colmain_fb_draw(void)
	{
		return & framebuff[0][0];
	}


	PACKEDCOLORMAIN_T *
	colmain_fb_show(void)
	{
		return & framebuff[0][0];
	}

#else
	RAMFRAMEBUFF ALIGNX_BEGIN PACKEDCOLORMAIN_T fbf [GXSIZE(DIM_SECOND, DIM_FIRST)] ALIGNX_END;

	void colmain_fb_next(void)
	{
	}

	PACKEDCOLORMAIN_T *
	colmain_fb_draw(void)
	{
		return fbf;
	}


	PACKEDCOLORMAIN_T *
	colmain_fb_show(void)
	{
		return fbf;
	}
#endif /* LCDMODE_LTDC */


// Установить прозрачность для прямоугольника
void display_transparency(
	uint_fast16_t x1, uint_fast16_t y1,
	uint_fast16_t x2, uint_fast16_t y2,
	uint_fast8_t alpha	// на сколько затемнять цвета (0 - чёрный, 255 - без изменений)
	)
{
	PACKEDCOLORMAIN_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
#if 1
	uint_fast16_t y;

	for (y = y1; y <= y2; y ++)
	{
		uint_fast16_t x;
		const uint_fast32_t yt = (uint_fast32_t) GXADJ(dx) * y;
		//ASSERT(y < dy);
		for (x = x1; x <= x2; x ++)
		{
			//ASSERT(x < dx);
			buffer [yt + x] = getshadedcolor(buffer [yt + x], alpha);
		}
	}
#else
	uint_fast16_t y;

	for (y = y1; y <= y2; y ++)
	{
		for (uint_fast16_t x = x1; x <= x2; x ++)
		{
			PACKEDCOLORMAIN_T * const p = colmain_mem_at(buffer, dx, dy, x, y);
			* p = getshadedcolor(* p, alpha);
		}
	}
#endif
}


static uint_fast8_t scalecolor(
	uint_fast8_t cv,	// color component value
	uint_fast8_t maxv,	// maximal color component value
	uint_fast8_t rmaxv	// resulting maximal color component value
	)
{
	return (cv * rmaxv) / maxv;
}

/* модифицировать цвет в RGB24 */
static COLOR24_T
color24_shaded(
	COLOR24_T dot,
	uint_fast8_t alpha	// на сколько затемнять цвета (0 - чёрный, 255 - без изменений)
	)
{
	//return dot;	// test
	if (dot == 0)
		return COLOR24(alpha >> 2, alpha >> 2, alpha >> 2);
	const uint_fast8_t r = scalecolor((dot >> 16) & 0xFF, 255, alpha);
	const uint_fast8_t g = scalecolor((dot >> 8) & 0xFF, 255, alpha);
	const uint_fast8_t b = scalecolor((dot >> 0) & 0xFF, 255, alpha);
	return COLOR24(r, g, b);
}

/* модифицировать цвет в RGB24 */
static COLOR24_T
color24_aliased(
	COLOR24_T dot
	)
{
	return color24_shaded(dot, DEFAULT_ALPHA);	// test
}


/* модифицировать цвет */
COLORPIP_T getshadedcolor(
	COLORPIP_T dot, // исходный цвет
	uint_fast8_t alpha	// на сколько затемнять цвета (0 - чёрный, 255 - без изменений)
	)
{
#if defined (COLORPIP_SHADED)

	return dot |= COLORPIP_SHADED;

#elif LCDMODE_PIP_RGB565

	if (dot == COLORPIP_BLACK)
	{
		return TFTRGB565(alpha, alpha, alpha); // back gray
	}
	else
	{
		const uint_fast8_t r = ((dot >> 11) & 0x001f) * 8;	// result in 0..255
		const uint_fast8_t g = ((dot >> 5) & 0x003f) * 4;	// result in 0..255
		const uint_fast8_t b = ((dot >> 0) & 0x001f) * 8;	// result in 0..255

		const COLOR24_T c = color24_shaded(COLOR24(r, g, b), alpha);
		return TFTRGB565((c >> 16) & 0xFF, (c >> 8) & 0xFF, (c >> 0) & 0xFF);
	}

#elif LCDMODE_PIP_RGB24

	if (dot == COLORPIP_BLACK)
	{
		return COLOR24(alpha, alpha, alpha); // back gray
	}
	else
	{
		return color24_shaded(dot, alpha);
	}



#else /*  */
	//#warning LCDMODE_PIP_L8 or LCDMODE_PIP_RGB565 not defined
	return dot;

#endif /* LCDMODE_PIP_L8 */
}

#if defined (COLORPIP_SHADED)

static void fillpair_xltrgb24(COLOR24_T * xltable, unsigned i, COLOR24_T c)
{
	ASSERT(i < 128);
	xltable [i] = c;
	xltable [i | COLORPIP_SHADED] = color24_shaded(c, DEFAULT_ALPHA);
}

static void fillfour_xltrgb24(COLOR24_T * xltable, unsigned i, COLOR24_T c)
{
	ASSERT(i < 128 - 16);
	xltable [i] = c;
	xltable [i | COLORPIP_SHADED] = color24_shaded(c, DEFAULT_ALPHA);
	xltable [i | COLORPIP_ALIASED] =  color24_aliased(c);
	xltable [i | COLORPIP_SHADED | COLORPIP_ALIASED] = color24_aliased(color24_shaded(c, DEFAULT_ALPHA));
}

#endif /* defined (COLORPIP_SHADED) */


void display2_xltrgb24(COLOR24_T * xltable)
{
#if defined (COLORPIP_SHADED)
	int i;

	PRINTF("display2_xltrgb24: init indexed colors\n");

	for (i = 0; i < 256; ++ i)
	{
		xltable [i] = COLOR24(255, 0, 0);
	}

	// часть цветов с 0-го индекса используется в отображении водопада
	// остальные в дизайне
	// PALETTESIZE == 96
	int a = 0;
	// a = 0
	for (i = 0; i < 16; ++ i)
	{
		fillpair_xltrgb24(xltable, a + i, COLOR24(0, 0, scalecolor(i, 15, 255)));	// проверить результат перед попыткой применить целочисленные вычисления!
	}
	a += i;
	// a = 16
	for (i = 0; i < 16; ++ i)
	{
		fillpair_xltrgb24(xltable, a + i, COLOR24(0, scalecolor(i, 15, 255), 255));
	}
	a += i;
	// a = 32
	for (i = 0; i < 16; ++ i)
	{
		fillpair_xltrgb24(xltable, a + i, COLOR24(0, 255, 255 - scalecolor(i, 15, 255)));
	}
	a += i;
	// a = 48
	for (i = 0; i < 16; ++ i)
	{
		fillpair_xltrgb24(xltable, a + i, COLOR24(scalecolor(i, 15, 255), 255, 0));
	}
	a += i;
	// a = 64
	for (i = 0; i < 16; ++ i)
	{
		fillpair_xltrgb24(xltable, a + i, COLOR24(255, 255 - scalecolor(i, 15, 255), 0));
	}
	a += i;
	// a = 80
	for (i = 0; i < 16; ++ i)
	{
		fillpair_xltrgb24(xltable, a + i, COLOR24(255, 0, scalecolor(i, 15, 255)));
	}
	a += i;
	// a = 96
	ASSERT(a == COLORPIP_BASE);

#if 0
	{
		/* тестовое заполнение палитры для проверки целостности сигналов к TFT
		 * Используется совместно с test: вывод палитры на экран
		 */
		enum { TESTSIZE = 64 };
		for (i = 0; i < 256; ++ i)
		{
			xltable [i] = COLOR24(0, 0, 0);
		}

	#if 0
		/* RED */
		for (i = 0; i < TESTSIZE; ++ i)
		{
			uint_fast8_t c = scalecolor(i, TESTSIZE - 1, 255);
			fillpair_xltrgb24(xltable, i, COLOR24(1 * c, 0 * c, 0 * c));	// проверить результат перед попыткой применить целочисленные вычисления!
		}
	#elif 0
		/* GREEN */
		for (i = 0; i < TESTSIZE; ++ i)
		{
			uint_fast8_t c = scalecolor(i, TESTSIZE - 1, 255);
			fillpair_xltrgb24(xltable, i, COLOR24(0 * c, 1 * c, 0 * c));	// проверить результат перед попыткой применить целочисленные вычисления!
		}
	#else
		/* BLUE */
		for (i = 0; i < TESTSIZE; ++ i)
		{
			uint_fast8_t c = scalecolor(i, TESTSIZE - 1, 255);
			fillpair_xltrgb24(xltable, i, COLOR24(0 * c, 0 * c, 1 * c));	// проверить результат перед попыткой применить целочисленные вычисления!
		}
	#endif
	}
#endif
	// Цвета используемые в дизайне

	fillfour_xltrgb24(xltable, COLORPIP_YELLOW    	, COLOR24(0xFF, 0xFF, 0x00));
	fillfour_xltrgb24(xltable, COLORPIP_ORANGE    	, COLOR24(0xFF, 0xA5, 0x00));
	fillfour_xltrgb24(xltable, COLORPIP_BLACK     	, COLOR24(0x00, 0x00, 0x00));
	fillfour_xltrgb24(xltable, COLORPIP_WHITE     	, COLOR24(0xFF, 0xFF, 0xFF));
	fillfour_xltrgb24(xltable, COLORPIP_GRAY      	, COLOR24(0x60, 0x60, 0x60));
	fillfour_xltrgb24(xltable, COLORPIP_DARKGREEN 	, COLOR24(0x00, 0x80, 0x00));
	fillfour_xltrgb24(xltable, COLORPIP_BLUE      	, COLOR24(0x00, 0x00, 0xFF));
	fillfour_xltrgb24(xltable, COLORPIP_GREEN     	, COLOR24(0x00, 0xFF, 0x00));
	fillfour_xltrgb24(xltable, COLORPIP_RED       	, COLOR24(0xFF, 0x00, 0x00));

	fillfour_xltrgb24(xltable, COLORPIP_LOCKED	  	, COLOR24(0x3C, 0x3C, 0x00));
	// код (COLORPIP_BASE + 15) освободися. GUI_MENUSELECTCOLOR?

#if COLORSTYLE_ATS52
	// new (for ats52)
	fillfour_xltrgb24(xltable, COLORPIP_GRIDCOLOR		, COLOR24(0x80, 0x00, 0x00));		//COLOR_GRAY - center marker
	fillfour_xltrgb24(xltable, COLORPIP_GRIDCOLOR2		, COLOR24(0x60, 0x60, 0x60));		//COLOR_DARKRED - other markers
	fillfour_xltrgb24(xltable, COLORPIP_SPECTRUMBG		, COLOR24(0x00, 0x40, 0x40));		// фон спектра вне полосы пропускания
	fillfour_xltrgb24(xltable, COLORMAIN_SPECTRUMBG2	, COLOR24(0x00, 0x80, 0x80));		// фон спектра - полоса пропускания приемника
	fillfour_xltrgb24(xltable, COLORPIP_SPECTRUMFG		, COLOR24(0x00, 0xFF, 0x00));		//COLOR_GREEN

#else /* COLORSTYLE_ATS52 */
	// old
	fillfour_xltrgb24(xltable, COLORPIP_GRIDCOLOR      	, COLOR24(0x80, 0x80, 0x00));        //COLOR_GRAY - center marker
	fillfour_xltrgb24(xltable, COLORPIP_GRIDCOLOR2     	, COLOR24(0x80, 0x00, 0x00));        //COLOR_DARKRED - other markers
	fillfour_xltrgb24(xltable, COLORPIP_SPECTRUMBG     	, COLOR24(0x00, 0x00, 0x00));            // фон спектра вне полосы пропускания
	fillfour_xltrgb24(xltable, COLORMAIN_SPECTRUMBG2   	, COLOR24(0x00, 0x80, 0x80));        // фон спектра - полоса пропускания приемника
	fillfour_xltrgb24(xltable, COLORPIP_SPECTRUMFG		, COLOR24(0x00, 0xFF, 0x00));		//COLOR_GREEN

#endif /* COLORSTYLE_ATS52 */

#elif LCDMODE_COLORED && ! LCDMODE_DUMMY	/* LCDMODE_MAIN_L8 && LCDMODE_PIP_L8 */
	PRINTF("display2_xltrgb24: init RRRRRGGG GGGBBBBB colos\n");
	// Обычная таблица - все цвета могут быть использованы как индекс
	// Водопад отображается без использования инлдексов цветов
	int i;

	for (i = 0; i < 256; ++ i)
	{
		uint_fast8_t r = ((i & 0xe0) << 0) | ((i & 0xe0) >> 3) | ((i & 0xe0) >> 6);		// 3 bit red
		uint_fast8_t g = ((i & 0x1c) << 3) | ((i & 0x1c) << 0) | ((i & 0x1c) >> 3) ;	// 3 bit green
		uint_fast8_t b = ((i & 0x03) << 6) | ((i & 0x03) << 4) | ((i & 0x03) << 2) | ((i & 0x03) << 0);	// 2 bit blue
		xltable [i] = COLOR24(r, g, b);
	}

#else
	#warning Monochrome display without indexing colors
#endif /* LCDMODE_MAIN_L8 && LCDMODE_PIP_L8 */
}

#if LCDMODE_LTDC

void display_putpixel(
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORMAIN_T color
	)
{
	PACKEDCOLORMAIN_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	colmain_putpixel(buffer, dx, dy, x, y, color);
}

/* заполнение прямоугольника на основном экране произвольным цветом
*/
void
display_fillrect(
	uint_fast16_t x, uint_fast16_t y, 	// координаты в пикселях
	uint_fast16_t w, uint_fast16_t h, 	// размеры в пикселях
	COLORMAIN_T color
	)
{
	PACKEDCOLORMAIN_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;

	colmain_fillrect(buffer, dx, dy, x, y, w, h, color);
}

#endif /* LCDMODE_LTDC */

/* копирование содержимого окна с перекрытием для водопада */
void
display_scroll_down(
	uint_fast16_t x0,	// левый верхний угол окна
	uint_fast16_t y0,	// левый верхний угол окна
	uint_fast16_t w, 	// до 65535 пикселей - ширина окна
	uint_fast16_t h, 	// до 65535 пикселей - высота окна
	uint_fast16_t n,	// количество строк прокрутки
	int_fast16_t hshift	// количество пиксеелей для сдвига влево (отрицательное число) или вправо (положительное).
	)
{
	PACKEDCOLORMAIN_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;

#if WITHDMA2DHW && LCDMODE_LTDC

#if LCDMODE_HORFILL && defined (DMA2D_FGPFCCR_CM_VALUE_MAIN)
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	/* TODO: В DMA2D нет средств управления направлением пересылки, потому данный код копирует сам на себя данные (размножает) */
	/* исходный растр */
	DMA2D->FGMAR = (uintptr_t) colmain_mem_at(buffer, dx, dy, y0 + 0, x0);
	DMA2D->FGOR = (DMA2D->FGOR & ~ (DMA2D_FGOR_LO)) |
		((DIM_X - w) << DMA2D_FGOR_LO_Pos) |
		0;
	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) colmain_mem_at(buffer, dx, dy, y0 + n, x0);
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((DIM_X - w) << DMA2D_OOR_LO_Pos) |
		0;
	/* размер пересылаемого растра */
	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		((h - n) << DMA2D_NLR_NL_Pos) |
		(w << DMA2D_NLR_PL_Pos) |
		0;
	/* формат пикселя */
	DMA2D->FGPFCCR = (DMA2D->FGPFCCR & ~ (DMA2D_FGPFCCR_CM)) |
		DMA2D_FGPFCCR_CM_VALUE_MAIN |	/* Color mode - framebuffer pixel format */
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		0 * DMA2D_CR_MODE_0 |	// 00: Memory-to-memory (FG fetch only)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		hardware_nonguiyield();
	__DMB();

	ASSERT((DMA2D->ISR & DMA2D_ISR_CEIF) == 0);	// Configuration Error
	ASSERT((DMA2D->ISR & DMA2D_ISR_TEIF) == 0);	// Transfer Error

#else /* LCDMODE_HORFILL */
#endif /* LCDMODE_HORFILL */

#endif /* WITHDMA2DHW && LCDMODE_LTDC */
}

/* копирование содержимого окна с перекрытием для водопада */
void
display_scroll_up(
	uint_fast16_t x0,	// левый верхний угол окна
	uint_fast16_t y0,	// левый верхний угол окна
	uint_fast16_t w, 	// до 65535 пикселей - ширина окна
	uint_fast16_t h, 	// до 65535 пикселей - высота окна
	uint_fast16_t n,	// количество строк прокрутки
	int_fast16_t hshift	// количество пиксеелей для сдвига влево (отрицательное число) или вправо (положительное).
	)
{
	PACKEDCOLORMAIN_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;

#if WITHDMA2DHW && LCDMODE_LTDC
#if LCDMODE_HORFILL && defined (DMA2D_FGPFCCR_CM_VALUE_MAIN)
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд

	/* исходный растр */
	DMA2D->FGMAR = (uintptr_t) colmain_mem_at(buffer, dx, dy, y0 + n, x0);
	DMA2D->FGOR = (DMA2D->FGOR & ~ (DMA2D_FGOR_LO)) |
		((DIM_X - w) << DMA2D_FGOR_LO_Pos) |
		0;
	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) colmain_mem_at(buffer, dx, dy, y0 + 0, x0);
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((DIM_X - w) << DMA2D_OOR_LO_Pos) |
		0;
	/* размер пересылаемого растра */
	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		((h - n) << DMA2D_NLR_NL_Pos) |
		(w << DMA2D_NLR_PL_Pos) |
		0;
	/* формат пикселя */
	DMA2D->FGPFCCR = (DMA2D->FGPFCCR & ~ (DMA2D_FGPFCCR_CM)) |
		DMA2D_FGPFCCR_CM_VALUE_MAIN |	/* Color mode - framebuffer pixel format */
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		0 * DMA2D_CR_MODE_0 |	// 00: Memory-to-memory (FG fetch only)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		hardware_nonguiyield();
	__DMB();

	ASSERT((DMA2D->ISR & DMA2D_ISR_CEIF) == 0);	// Configuration Error
	ASSERT((DMA2D->ISR & DMA2D_ISR_TEIF) == 0);	// Transfer Error

#else /* LCDMODE_HORFILL */
#endif /* LCDMODE_HORFILL */
#endif /* WITHDMA2DHW && LCDMODE_LTDC */
}


#if ! LCDMODE_LTDC_L24
#include "./byte2crun.h"
#endif /* ! LCDMODE_LTDC_L24 */

static PACKEDCOLORMAIN_T ltdc_fg = COLORMAIN_WHITE, ltdc_bg = COLORMAIN_BLACK;

#if ! LCDMODE_LTDC_L24
static const FLASHMEM PACKEDCOLORMAIN_T (* byte2runmain) [256][8] = & byte2runmain_COLORMAIN_WHITE_COLORMAIN_BLACK;
//static const FLASHMEM PACKEDCOLORPIP_T (* byte2runpip) [256][8] = & byte2runpip_COLORPIP_WHITE_COLORPIP_BLACK;
#endif /* ! LCDMODE_LTDC_L24 */

void colmain_setcolors(COLORMAIN_T fg, COLORMAIN_T bg)
{

#if ! LCDMODE_LTDC_L24
	ltdc_fg = fg;
	ltdc_bg = bg;
#else /* ! LCDMODE_LTDC_L24 */

	ltdc_fg.r = fg >> 16;
	ltdc_fg.g = fg >> 8;
	ltdc_fg.b = fg >> 0;
	ltdc_bg.r = bg >> 16;
	ltdc_bg.g = bg >> 8;
	ltdc_bg.b = bg >> 0;

#endif /* ! LCDMODE_LTDC_L24 */

#if ! LCDMODE_LTDC_L24

	COLORMAIN_SELECTOR(byte2runmain);

#endif /* ! LCDMODE_LTDC_L24 */

	//COLORPIP_SELECTOR(byte2runpip);

}

void colmain_setcolors3(COLORMAIN_T fg, COLORMAIN_T bg, COLORMAIN_T fgbg)
{
	colmain_setcolors(fg, bg);
}

/* индивидуальные функции драйвера дисплея - реализованы в соответствующем из файлов */
void display_clear(void)
{
	const COLORMAIN_T bg = display_getbgcolor();
	display_fillrect(0, 0, DIM_X, DIM_Y, bg);
}

void display_plotstart(
	uint_fast16_t dy	// Высота окна источника в пикселях
	)
{

}

void display_plotstop(void)
{

}

// Вызовы этой функции (или группу вызовов) требуется "обрамить" парой вызовов
// display_wrdatabar_begin() и display_wrdatabar_end().
void display_bar(
	uint_fast16_t x,
	uint_fast16_t y,
	uint_fast8_t width,	/* количество знакомест, занимаемых индикатором */
	uint_fast8_t value,		/* значение, которое надо отобразить */
	uint_fast8_t tracevalue,		/* значение маркера, которое надо отобразить */
	uint_fast8_t topvalue,	/* значение, соответствующее полностью заполненному индикатору */
	uint_fast8_t vpattern,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL */
	uint_fast8_t patternmax,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL - для отображения запомненного значения */
	uint_fast8_t emptyp			/* паттерн для заполнения между штрихами */
	)
{
	PACKEDCOLORMAIN_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	ASSERT(value <= topvalue);
	ASSERT(tracevalue <= topvalue);
	const uint_fast16_t wfull = GRID2X(width);
	const uint_fast16_t h = SMALLCHARH; //GRID2Y(1);
	const uint_fast16_t wpart = (uint_fast32_t) wfull * value / topvalue;
	const uint_fast16_t wmark = (uint_fast32_t) wfull * tracevalue / topvalue;
	const uint_fast8_t hpattern = 0x33;

	colmain_fillrect(buffer, dx, dy, 	x, y, 			wpart, h, 			ltdc_fg);
	colmain_fillrect(buffer, dx, dy, 	x + wpart, y, 	wfull - wpart, h, 	ltdc_bg);
	if (wmark < wfull && wmark >= wpart)
		colmain_fillrect(buffer, dx, dy, x + wmark, y, 	1, h, 				ltdc_fg);
}

// самый маленький шрифт
uint_fast16_t display_wrdata2_begin(uint_fast8_t x, uint_fast8_t y, uint_fast16_t * yp)
{
	//ltdc_secondoffs = 0;
	//ltdc_h = SMALLCHARH;

	* yp = GRID2Y(y);
	return GRID2X(x);
}

void display_wrdata2_end(void)
{
}


// Выдать один цветной пиксель
static void
ltdc_pix1color(
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	PACKEDCOLORMAIN_T color
	)
{
	PACKEDCOLORMAIN_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	volatile PACKEDCOLORMAIN_T * const tgr = colmain_mem_at(buffer, dx, dy, x, y);
	* tgr = color;
	//arm_hardware_flush((uintptr_t) tgr, sizeof * tgr);
}


// Выдать один цветной пиксель (фон/символ)
static void
ltdc_pixel(
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	uint_fast8_t v			// 0 - цвет background, иначе - foreground
	)
{
	ltdc_pix1color(x, y, v ? ltdc_fg : ltdc_bg);
}


// Выдать восемь цветных пикселей, младший бит - самый верхний в растре
static void
ltdc_vertical_pixN(
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	uint_fast8_t pattern,		// pattern
	uint_fast8_t w		// number of lower bits used in pattern
	)
{

#if LCDMODE_LTDC_L24 || LCDMODE_HORFILL

	// TODO: для паттернов шире чем восемь бит, повторить нужное число раз.
	ltdc_pixel(x, y + 0, pattern & 0x01);
	ltdc_pixel(x, y + 1, pattern & 0x02);
	ltdc_pixel(x, y + 2, pattern & 0x04);
	ltdc_pixel(x, y + 3, pattern & 0x08);
	ltdc_pixel(x, y + 4, pattern & 0x10);
	ltdc_pixel(x, y + 5, pattern & 0x20);
	ltdc_pixel(x, y + 6, pattern & 0x40);
	ltdc_pixel(x, y + 7, pattern & 0x80);

	// сместить по вертикали?
	//ltdc_secondoffs ++;

#else /* LCDMODE_LTDC_L24 */
	PACKEDCOLORMAIN_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	PACKEDCOLORMAIN_T * const tgr = colmain_mem_at(buffer, dx, dy, x, y);
	// размещаем пиксели по горизонтали
	// TODO: для паттернов шире чем восемь бит, повторить нужное число раз.
	const FLASHMEM PACKEDCOLORMAIN_T * const pcl = (* byte2runmain) [pattern];
	memcpy(tgr, pcl, sizeof (* pcl) * w);
	//arm_hardware_flush((uintptr_t) tgr, sizeof (PACKEDCOLORMAIN_T) * w);
#endif /* LCDMODE_LTDC_L24 */
}

#if LCDMODE_HORFILL

// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
void RAMFUNC ltdc_horizontal_pixels(
	PACKEDCOLORMAIN_T * tgr,		// target raster
	const FLASHMEM uint8_t * raster,
	uint_fast16_t width	// number of bits (start from LSB first byte in raster)
	)
{
	uint_fast16_t col;
	uint_fast16_t w = width;

	for (col = 0; w >= 8; col += 8, w -= 8)
	{
		const FLASHMEM PACKEDCOLORMAIN_T * const pcl = (* byte2runmain) [* raster ++];
		memcpy(tgr + col, pcl, sizeof (* tgr) * 8);
	}
	if (w != 0)
	{
		const FLASHMEM PACKEDCOLORMAIN_T * const pcl = (* byte2runmain) [* raster ++];
		memcpy(tgr + col, pcl, sizeof (* tgr) * w);
	}
	//arm_hardware_flush((uintptr_t) tgr, sizeof (* tgr) * width);
}

// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// return new x
static uint_fast16_t
RAMFUNC_NONILINE
ltdc_horizontal_put_char_small(uint_fast16_t x, uint_fast16_t y, char cc)
{
	PACKEDCOLORMAIN_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	const uint_fast8_t width = SMALLCHARW;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH; ++ cgrow)
	{
		PACKEDCOLORMAIN_T * const tgr = colmain_mem_at(buffer, dx, dy, x, y + cgrow);
		ltdc_horizontal_pixels(tgr, S1D13781_smallfont_LTDC [c] [cgrow], width);
	}
	return x + width;
}

// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
// return new x coordinate
static uint_fast16_t RAMFUNC_NONILINE ltdc_horizontal_put_char_big(uint_fast16_t x, uint_fast16_t y, char cc)
{
	PACKEDCOLORMAIN_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	const uint_fast8_t width = ((cc == '.' || cc == '#') ? BIGCHARW_NARROW  : BIGCHARW);	// полнаяширина символа в пикселях
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < BIGCHARH; ++ cgrow)
	{
		PACKEDCOLORMAIN_T * const tgr = colmain_mem_at(buffer, dx, dy, x, y + cgrow);
		ltdc_horizontal_pixels(tgr, S1D13781_bigfont_LTDC [c] [cgrow], width);
	}
	return x + width;
}

// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
// return new x coordinate
static uint_fast16_t RAMFUNC_NONILINE ltdc_horizontal_put_char_half(uint_fast16_t x, uint_fast16_t y, char cc)
{
	PACKEDCOLORMAIN_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	const uint_fast8_t width = HALFCHARW;
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < HALFCHARH; ++ cgrow)
	{
		PACKEDCOLORMAIN_T * const tgr = colmain_mem_at(buffer, dx, dy, x, y + cgrow);
		ltdc_horizontal_pixels(tgr, S1D13781_halffont_LTDC [c] [cgrow], width);
	}
	return x + width;
}

#else /* LCDMODE_HORFILL */

// Вызов этой функции только внутри display_wrdata_begin() и 	display_wrdata_end();
static uint_fast16_t RAMFUNC_NONILINE ltdc_vertical_put_char_small(uint_fast16_t x, uint_fast16_t y, char cc)
{
	uint_fast8_t i = 0;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_smallfont [0] / sizeof ls020_smallfont [0] [0]) };
	const FLASHMEM uint8_t * const p = & ls020_smallfont [c] [0];

	for (; i < NBYTES; ++ i)
		ltdc_vertical_pixN(x ++, y, p [i], 8);	// Выдать восемь цветных пикселей, младший бит - самый верхний в растре
	return x;
}

// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
static uint_fast16_t RAMFUNC_NONILINE ltdc_vertical_put_char_big(uint_fast16_t x, uint_fast16_t y, char cc)
{
	// '#' - узкий пробел
	enum { NBV = (BIGCHARH / 8) }; // сколько байтов в одной вертикали
	uint_fast8_t i = NBV * ((cc == '.' || cc == '#') ? 12 : 0);	// начальная колонка знакогенератора, откуда начинать.
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_bigfont [0] / sizeof ls020_bigfont [0] [0]) };
	const FLASHMEM uint8_t * const p = & ls020_bigfont [c] [0];

	for (; i < NBYTES; ++ i)
		ltdc_vertical_pixN(x ++, y, p [i], 8);	// Выдать восемь цветных пикселей, младший бит - самый верхний в растре
	return x;
}

// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
static uint_fast16_t RAMFUNC_NONILINE ltdc_vertical_put_char_half(uint_fast16_t x, uint_fast16_t y, char cc)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_halffont [0] / sizeof ls020_halffont [0] [0]) };
	const FLASHMEM uint8_t * const p = & ls020_halffont [c] [0];

	for (; i < NBYTES; ++ i)
		ltdc_vertical_pixN(x ++, y, p [i], 8);	// Выдать восемь цветных пикселей, младший бит - самый верхний в растре
	return x;
}


#endif /* LCDMODE_HORFILL */

#if 0
uint_fast16_t display_put_char_small2(uint_fast16_t x, uint_fast16_t y, uint_fast8_t c, uint_fast8_t lowhalf)
{
#if LCDMODE_HORFILL
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	return ltdc_horizontal_put_char_small(x, y, c);
#else /* LCDMODE_HORFILL */
	return ltdc_vertical_put_char_small(x, y, c);
#endif /* LCDMODE_HORFILL */
}
#endif

// полоса индикатора
uint_fast16_t display_wrdatabar_begin(uint_fast8_t x, uint_fast8_t y, uint_fast16_t * yp)
{
//	ltdc_secondoffs = 0;
//	ltdc_h = 8;

	* yp = GRID2Y(y);
	return GRID2X(x);
}

// Выдать восемь цветных пикселей, младший бит - самый верхний в растре
uint_fast16_t
display_barcolumn(uint_fast16_t xpix, uint_fast16_t ypix, uint_fast8_t pattern)
{
//	ltdc_vertical_pixN(pattern, 8);	// Выдать восемь цветных пикселей, младший бит - самый верхний в растре
	return xpix + 1;
}

void display_wrdatabar_end(void)
{
}

// большие и средние цифры (частота)
uint_fast16_t display_wrdatabig_begin(uint_fast8_t x, uint_fast8_t y, uint_fast16_t * yp)
{
	//ltdc_secondoffs = 0;
	//ltdc_h = BIGCHARH;

	* yp = GRID2Y(y);
	return GRID2X(x);
}

uint_fast16_t display_put_char_big(uint_fast16_t x, uint_fast16_t y, uint_fast8_t c, uint_fast8_t lowhalf)
{
#if LCDMODE_HORFILL
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	return ltdc_horizontal_put_char_big(x, y, c);
#else /* LCDMODE_HORFILL */
	return ltdc_vertical_put_char_big(x, y, c);
#endif /* LCDMODE_HORFILL */
}

uint_fast16_t display_put_char_half(uint_fast16_t x, uint_fast16_t y, uint_fast8_t c, uint_fast8_t lowhalf)
{
#if LCDMODE_HORFILL
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	return ltdc_horizontal_put_char_half(x, y, c);
#else /* LCDMODE_HORFILL */
	return ltdc_vertical_put_char_half(x, y, c);
#endif /* LCDMODE_HORFILL */
}

void display_wrdatabig_end(void)
{
}

// обычный шрифт
uint_fast16_t display_wrdata_begin(uint_fast8_t x, uint_fast8_t y, uint_fast16_t * yp)
{
//	ltdc_secondoffs = 0;
//	ltdc_h = SMALLCHARH;

	* yp = GRID2Y(y);
	return GRID2X(x);
}

uint_fast16_t display_put_char_small(uint_fast16_t x, uint_fast16_t y, uint_fast8_t c, uint_fast8_t lowhalf)
{
#if LCDMODE_HORFILL
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	return ltdc_horizontal_put_char_small(x, y, c);
#else /* LCDMODE_HORFILL */
	return ltdc_vertical_put_char_small(x, y, c);
#endif /* LCDMODE_HORFILL */
}

void display_wrdata_end(void)
{
}


#if LCDMODE_LQ043T3DX02K || LCDMODE_AT070TN90 || LCDMODE_AT070TNA2

// заглушки

/* аппаратный сброс дисплея - перед инициализаций */
/* вызывается при разрешённых прерываниях. */
void
display_reset(void)
{
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(1); // Delay 1ms
	board_lcd_reset(0); 	// Pull RST pin down
	board_update();
	local_delay_ms(10); // Delay 10ms
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(50); // Delay 50 ms
}
/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{
}

void display_set_contrast(uint_fast8_t v)
{
}

/* Разряжаем конденсаторы питания */
void display_discharge(void)
{
}

#endif /* LCDMODE_LQ043T3DX02K */
#endif /* LCDMODE_LTDC */

uint_fast8_t
bigfont_decode(uint_fast8_t c)
{
	// '#' - узкий пробел
	if (c == ' ' || c == '#')
		return 11;
	if (c == '_')
		return 10;		// курсор - позиция редактирвания частоты
	if (c == '.')
		return 12;		// точка
	if (c > '9')
		return 10;		// ошибка - курсор - позиция редактирвания частоты
	return c - '0';		// остальные - цифры 0..9
}

uint_fast8_t
smallfont_decode(uint_fast8_t c)
{
	if (c < ' ' || c > 0x7F)
		return '$' - ' ';
	return c - ' ';
}


#if 0
// Используется при выводе на графический индикатор,
// самый маленький шрифт
static void
display_string2(uint_fast8_t xcell, uint_fast8_t ycell, const char * s, uint_fast8_t lowhalf)
{
	char c;
	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdata2_begin(xcell, ycell, & ypix);
	while((c = * s ++) != '\0')
		xpix = display_put_char_small2(xpix, ypix, c, lowhalf);
	display_wrdata2_end();
}



// Используется при выводе на графический индикатор,
// самый маленький шрифт
static void
display_string2_P(uint_fast8_t xcell, uint_fast8_t ycell, const FLASHMEM  char * s, uint_fast8_t lowhalf)
{
	char c;

	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdata2_begin(xcell, ycell, & ypix);
	while((c = * s ++) != '\0')
		xpix = display_put_char_small2(xpix, xpix, c, lowhalf);
	display_wrdata2_end();
}
#endif

const char * savestring = "no data";
// Используется при выводе на графический индикатор,
static void
display_string(uint_fast8_t xcell, uint_fast8_t ycell, const char * s, uint_fast8_t lowhalf)
{
	savestring = s;
	char c;

	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdata_begin(xcell, ycell, & ypix);
	while((c = * s ++) != '\0')
		xpix = display_put_char_small(xpix, ypix, c, lowhalf);
	display_wrdata_end();
}

// Выдача строки из ОЗУ в указанное место экрана.
void
//NOINLINEAT
display_at(uint_fast8_t x, uint_fast8_t y, const char * s)
{
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_string(x, y + lowhalf, s, lowhalf);

	} while (lowhalf --);
}

// Используется при выводе на графический индикатор,
static void
display_string_P(uint_fast8_t xcell, uint_fast8_t ycell, const FLASHMEM  char * s, uint_fast8_t lowhalf)
{
	char c;

	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdata_begin(xcell, ycell, & ypix);
	while((c = * s ++) != '\0')
		xpix = display_put_char_small(xpix, ypix, c, lowhalf);
	display_wrdata_end();
}

// Выдача строки из ПЗУ в указанное место экрана.
void
//NOINLINEAT
display_at_P(uint_fast8_t x, uint_fast8_t y, const FLASHMEM char * s)
{
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		display_string_P(x, y + lowhalf, s, lowhalf);

	} while (lowhalf --);
}

/* выдать на дисплей монохромный буфер с размерами dx * dy битов */
void display_showbuffer(
	const GX_t * buffer,
	unsigned dx,	// пиксели
	unsigned dy,	// пиксели
	uint_fast8_t xcell,	// сетка
	uint_fast8_t ycell	// сетка
	)
{
#if 1
#if LCDMODE_S1D13781

	s1d13781_showbuffer(buffer, dx, dy, xcell, ycell);

#else /* LCDMODE_S1D13781 */

	#if WITHSPIHWDMA && (LCDMODE_UC1608 | 0)
		// на LCDMODE_S1D13781 почему-то DMA сбивает контроллер
		arm_hardware_flush((uintptr_t) buffer, sizeof (* buffer) * MGSIZE(dx, dy));	// количество байтов
	#endif

	uint_fast8_t lowhalf = (dy) / 8 - 1;
	if (lowhalf == 0)
		return;
	do
	{
		uint_fast8_t pos;
		const GX_t * const p = buffer + lowhalf * MGADJ(dx);	// начало данных горизонтальной полосы в памяти
		//debug_printf_P(PSTR("display_showbuffer: col=%d, row=%d, lowhalf=%d\n"), col, row, lowhalf);
		display_plotfrom(GRID2X(ycell), GRID2Y(xcell) + lowhalf * 8);		// курсор в начало первой строки
		// выдача горизонтальной полосы
		uint_fast16_t ypix;
		uint_fast16_t xpix = display_wrdatabar_begin(xcell, ycell, & ypix);
	#if WITHSPIHWDMA && (0)
		// на LCDMODE_S1D13781 почему-то DMA сбивает контроллер
		// на LCDMODE_UC1608 портит мохранене теузей частоты и режима работы (STM32F746xx)
		hardware_spi_master_send_frame(p, dx);
	#else
		for (pos = 0; pos < dx; ++ pos)
			xpix = display_barcolumn(xpix, ypix, p [pos]);	// Выдать восемь цветных пикселей, младший бит - самый верхний в растре
	#endif
		display_wrdatabar_end();
	} while (lowhalf --);

#endif /* LCDMODE_S1D13781 */
#endif
}

#if LCDMODE_S1D13781

	// младший бит левее
	static const uint_fast16_t mapcolumn [16] =
	{
		0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080, // биты для манипуляций с видеобуфером
		0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000,
	};

#elif LCDMODE_UC1608 || LCDMODE_UC1601

	/* старшие биты соответствуют верхним пикселям изображения */
	// млдший бит ниже в растре
	static const uint_fast8_t mapcolumn [8] =
	{
		0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, // биты для манипуляций с видеобуфером
	};
#else /* LCDMODE_UC1608 || LCDMODE_UC1601 */

	/* младшие биты соответствуют верхним пикселям изображения */
	// млдший бит выше в растре
	static const uint_fast8_t mapcolumn [8] =
	{
		0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, // биты для манипуляций с видеобуфером
	};

#endif /* LCDMODE_UC1608 || LCDMODE_UC1601 */


// погасить точку
void display_pixelbuffer(
	GX_t * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row	// вертикальная координата пикселя (0..dy-1) сверху вниз
	)
{
#if LCDMODE_S1D13781

	//row = (dy - 1) - row;		// смена направления
	GX_t * const rowstart = buffer + row * MGADJ(dx);	// начало данных строки растра в памяти
	GX_t * const p = rowstart + col / 16;
	//* p |= mapcolumn [col % 16];	// установить точку
	* p &= ~ mapcolumn [col % 16];	// погасить точку
	//* p ^= mapcolumn [col % 16];	// инвертировать точку

#else /* LCDMODE_S1D13781 */

	//row = (dy - 1) - row;		// смена направления
	GX_t * const p = buffer + (row / 8) * MGADJ(dx) + col;	// начало данных горизонтальной полосы в памяти
	//* p |= mapcolumn [row % 8];	// установить точку
	* p &= ~ mapcolumn [row % 8];	// погасить точку
	//* p ^= mapcolumn [row % 8];	// инвертировать точку

#endif /* LCDMODE_S1D13781 */
}

/* Исключающее ИЛИ с точкой в растре */
void display_pixelbuffer_xor(
	GX_t * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row	// вертикальная координата пикселя (0..dy-1) сверху вниз
	)
{
#if LCDMODE_S1D13781
	//row = (dy - 1) - row;		// смена направления
	GX_t * const rowstart = buffer + row * MGADJ(dx);	// начало данных строки растра в памяти
	GX_t * const p = rowstart + col / 16;
	//* p |= mapcolumn [col % 16];	// установить точку
	//* p &= ~ mapcolumn [col % 16];	// погасить точку
	* p ^= mapcolumn [col % 16];	// инвертировать точку

#else /* LCDMODE_S1D13781 */

	//row = (dy - 1) - row;		// смена направления
	GX_t * const p = buffer + (row / 8) * MGADJ(dx);	// начало данных горизонтальной полосы в памяти
	//* p |= mapcolumn [row % 8];	// установить точку
	//* p &= ~ mapcolumn [row % 8];	// погасить точку
	* p ^= mapcolumn [row % 8];	// инвертировать точку

#endif /* LCDMODE_S1D13781 */
}

void display_pixelbuffer_clear(
	GX_t * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy
	)
{
	memset(buffer, 0xFF, (size_t) MGSIZE(dx, dy) * (sizeof * buffer));			// рисование способом погасить точку
}

// Routine to draw a line in the RGB565 color to the LCD.
// The line is drawn from (xmin,ymin) to (xmax,ymax).
// The algorithm used to draw the line is "Bresenham's line
// algorithm".
#define SWAP(a, b)  do { (a) ^= (b); (b) ^= (a); (a) ^= (b); } while (0)
// Нарисовать линию указанным цветом
void display_pixelbuffer_line(
	GX_t * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x0,
	uint_fast16_t y0,
	uint_fast16_t x1,
	uint_fast16_t y1
	)
{
	int xmin = x0;
	int xmax = x1;
	int ymin = y0;
	int ymax = y1;
   int Dx = xmax - xmin;
   int Dy = ymax - ymin;
   int steep = (abs(Dy) >= abs(Dx));
   if (steep) {
	   SWAP(xmin, ymin);
	   SWAP(xmax, ymax);
	   // recompute Dx, Dy after swap
	   Dx = xmax - xmin;
	   Dy = ymax - ymin;
   }
   int xstep = 1;
   if (Dx < 0) {
	   xstep = -1;
	   Dx = -Dx;
   }
   int ystep = 1;
   if (Dy < 0) {
	   ystep = -1;
	   Dy = -Dy;
   }
   int TwoDy = 2*Dy;
   int TwoDyTwoDx = TwoDy - 2*Dx; // 2*Dy - 2*Dx
   int E = TwoDy - Dx; //2*Dy - Dx
   int y = ymin;
   int xDraw, yDraw;
   int x;
   for (x = xmin; x != xmax; x += xstep) {
	   if (steep) {
		   xDraw = y;
		   yDraw = x;
	   } else {
		   xDraw = x;
		   yDraw = y;
	   }
	   // plot
	   //LCD_PlotPoint(xDraw, yDraw, color);
	   display_pixelbuffer(buffer, dx, dy, xDraw, yDraw);
	   // next
	   if (E > 0) {
		   E += TwoDyTwoDx; //E += 2*Dy - 2*Dx;
		   y = y + ystep;
	   } else {
		   E += TwoDy; //E += 2*Dy;
	   }
   }
}
#undef SWAP

static const FLASHMEM int32_t vals10 [] =
{
	1000000000UL,
	100000000UL,
	10000000UL,
	1000000UL,
	100000UL,
	10000UL,
	1000UL,
	100UL,
	10UL,
	1UL,
};

// Отображение цифр в поле "больших цифр" - индикатор основной частоты настройки аппарата.
void
NOINLINEAT
display_value_big(
	uint_fast8_t x,	// x координата начала вывода значения
	uint_fast8_t y,	// y координата начала вывода значения
	uint_fast32_t freq,
	uint_fast8_t width, // = 8;	// full width
	uint_fast8_t comma, // = 2;	// comma position (from right, inside width)
	uint_fast8_t comma2,	// = comma + 3;		// comma position (from right, inside width)
	uint_fast8_t rj,	// = 1;		// right truncated
	uint_fast8_t blinkpos,		// позиция, где символ заменён пробелом
	uint_fast8_t blinkstate,	// 0 - пробел, 1 - курсор
	uint_fast8_t withhalf,		// 0 - только большие цифры
	uint_fast8_t lowhalf		// lower half
	)
{
	//const uint_fast8_t comma2 = comma + 3;		// comma position (from right, inside width)
	const uint_fast8_t j = (sizeof vals10 /sizeof vals10 [0]) - rj;
	uint_fast8_t i = (j - width);
	uint_fast8_t z = 1;	// only zeroes
	uint_fast8_t half = 0;	// отображаем после второй запатой - маленьким шрифтом

	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdatabig_begin(x, y, & ypix);
	for (; i < j; ++ i)
	{
		const ldiv_t res = ldiv(freq, vals10 [i]);
		const uint_fast8_t g = (j - i);		// десятичная степень текущего разряда на отображении

		// разделитель десятков мегагерц
		if (comma2 == g)
		{
			xpix = display_put_char_big(xpix, ypix, (z == 0) ? '.' : '#', lowhalf);	// '#' - узкий пробел. Точка всегда узкая
		}
		else if (comma == g)
		{
			z = 0;
			half = withhalf;
			xpix = display_put_char_big(xpix, ypix, '.', lowhalf);
		}

		if (blinkpos == g)
		{
			const uint_fast8_t bc = blinkstate ? '_' : ' ';
			// эта позиция редактирования частоты. Справа от неё включаем все нули
			z = 0;
			if (half)
				xpix = display_put_char_half(xpix, ypix, bc, lowhalf);

			else
				xpix = display_put_char_big(xpix, ypix, bc, lowhalf);
		}
		else if (z == 1 && (i + 1) < j && res.quot == 0)
			xpix = display_put_char_big(xpix, ypix, ' ', lowhalf);	// supress zero
		else
		{
			z = 0;
			if (half)
				xpix = display_put_char_half(xpix, ypix, '0' + res.quot, lowhalf);

			else
				xpix = display_put_char_big(xpix, ypix, '0' + res.quot, lowhalf);
		}
		freq = res.rem;
	}
	display_wrdatabig_end();
}


void
NOINLINEAT
display_value_small(
	uint_fast8_t x,	// x координата начала вывода значения
	uint_fast8_t y,	// y координата начала вывода значения
	int_fast32_t freq,
	uint_fast8_t width,	// full width (if >= 128 - display with sign)
	uint_fast8_t comma,		// comma position (from right, inside width)
	uint_fast8_t comma2,
	uint_fast8_t rj,		// right truncated
	uint_fast8_t lowhalf
	)
{
	const uint_fast8_t wsign = (width & WSIGNFLAG) != 0;
	const uint_fast8_t wminus = (width & WMINUSFLAG) != 0;
	const uint_fast8_t j = (sizeof vals10 /sizeof vals10 [0]) - rj;
	uint_fast8_t i = j - (width & WWIDTHFLAG);	// Номер цифры по порядку
	uint_fast8_t z = 1;	// only zeroes

	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdata_begin(x, y, & ypix);
	if (wsign || wminus)
	{
		// отображение со знаком.
		z = 0;
		if (freq < 0)
		{
			xpix = display_put_char_small(xpix, ypix, '-', lowhalf);
			freq = - freq;
		}
		else if (wsign)
			xpix = display_put_char_small(xpix, ypix, '+', lowhalf);
		else
			xpix = display_put_char_small(xpix, ypix, ' ', lowhalf);
	}
	for (; i < j; ++ i)
	{
		const ldiv_t res = ldiv(freq, vals10 [i]);
		const uint_fast8_t g = (j - i);
		// разделитель десятков мегагерц
		if (comma2 == g)
		{
			xpix = display_put_char_small(xpix, ypix, (z == 0) ? '.' : ' ', lowhalf);
		}
		else if (comma == g)
		{
			z = 0;
			xpix = display_put_char_small(xpix, ypix, '.', lowhalf);
		}

		if (z == 1 && (i + 1) < j && res.quot == 0)
			xpix = display_put_char_small(xpix, ypix, ' ', lowhalf);	// supress zero
		else
		{
			z = 0;
			xpix = display_put_char_small(xpix, ypix, '0' + res.quot, lowhalf);
		}
		freq = res.rem;
	}
	display_wrdata_end();
}

#if LCDMODE_COLORED
static COLORMAIN_T bgcolor = COLORMAIN_BLACK;
#endif /* LCDMODE_COLORED */

void
display_setbgcolor(COLORMAIN_T c)
{
#if LCDMODE_COLORED
	bgcolor = c;
#endif /* LCDMODE_COLORED */
}

COLORMAIN_T
display_getbgcolor(void)
{
#if LCDMODE_COLORED
	return bgcolor;
#else /* LCDMODE_COLORED */
	return COLOR_BLACK;
#endif /* LCDMODE_COLORED */
}


#if LCDMODE_LTDC && (LCDMODE_MAIN_L8 && LCDMODE_PIP_RGB565) || (! LCDMODE_MAIN_L8 && LCDMODE_PIP_L8)

// Выдать буфер на дисплей
// В случае фреймбуфеных дисплеев - формат цвета и там и там одинаковый
// если разный - то заглушка

//#warning colpip_to_main is dummy for this LCDMODE_LTDC combination

void colpip_to_main(
	const PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата левого верхнего угла на экране (0..dx-1) слева направо
	uint_fast16_t row	// вертикальная координата левого верхнего угла на экране (0..dy-1) сверху вниз
	)
{
	ASSERT(0);
}


// Координаты в пикселях
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
}

#elif LCDMODE_LTDC

// Выдать буфер на дисплей. Функции бывают только для не L8 режимов
// В случае фреймбуфеных дисплеев - формат цвета и там и там одинаковый
void colpip_to_main(
	uintptr_t srcinvalidateaddr,	// параметры clean источника
	int_fast32_t srcinvalidatesize,
	const PACKEDCOLORPIP_T * buffer,	// источник
	uint_fast16_t dx,	// ширина буфера источника
	uint_fast16_t dy,	// высота буфера источника
	uint_fast16_t col,	// целевая горизонтальная координата левого верхнего угла на экране (0..dx-1) слева направо
	uint_fast16_t row	// целевая вертикальная координата левого верхнего угла на экране (0..dy-1) сверху вниз
	)
{
	ASSERT(dx <= DIM_X);
	ASSERT(dy <= DIM_Y);
	ASSERT(((uintptr_t) buffer % DCACHEROWSIZE) == 0);
#if LCDMODE_HORFILL
	hwaccel_copy(
		(uintptr_t) colmain_fb_draw(), sizeof (PACKEDCOLORPIP_T) * GXSIZE(DIM_X, DIM_Y),	// target area invalidate parameters
		colmain_mem_at(colmain_fb_draw(), DIM_X, DIM_Y, col, row), DIM_X, DIM_Y,
		srcinvalidateaddr, srcinvalidatesize,	// параметры clean источника
		buffer, dx, dy
		);
#else /* LCDMODE_HORFILL */
	hwaccel_copy(
		(uintptr_t) colmain_fb_draw(), sizeof (PACKEDCOLORPIP_T) * GXSIZE(DIM_X, DIM_Y),	// target area invalidate parameters
		colmain_mem_at(colmain_fb_draw(), DIM_X, DIM_Y, col, row), DIM_X, DIM_Y,
		srcinvalidateaddr, srcinvalidatesize,	// параметры clean источника
		buffer, dx, dy
		);
#endif /* LCDMODE_HORFILL */
}

// Координаты в пикселях
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
}

#else

// Выдать буфер на дисплей. Функции бывают только для не L8 режимов
// В случае фреймбуфеных дисплеев - формат цвета и там и там одинаковый
void colpip_to_main(
	uintptr_t srcinvalidateaddr,	// параметры clean источника
	int_fast32_t srcinvalidatesize,
	const PACKEDCOLORPIP_T * buffer,	// источник
	uint_fast16_t dx,	// ширина буфера источника
	uint_fast16_t dy,	// высота буфера источника
	uint_fast16_t xpix,	// горизонтальная координата левого верхнего угла на экране (0..dx-1) слева направо
	uint_fast16_t ypix	// вертикальная координата левого верхнего угла на экране (0..dy-1) сверху вниз
	)
{
#if LCDMODE_COLORED
	display_plotfrom(xpix, ypix);
	display_plotstart(dy);
	display_plot(buffer, dx, dy, xpix, ypix);
	display_plotstop();
#endif
}

#endif /*  */

/*
 * настройка портов для последующей работы с дополнительными (кроме последовательного канала)
 * сигналами дисплея.
 */
/* вызывается при запрещённых прерываниях. */
void display_hardware_initialize(void)
{
	debug_printf_P(PSTR("display_hardware_initialize start\n"));


#if WITHDMA2DHW
	// Image construction hardware
	arm_hardware_dma2d_initialize();

#endif /* WITHDMA2DHW */
#if WITHMDMAHW
	// Image construction hardware
	arm_hardware_mdma_initialize();

#endif /* WITHMDMAHW */
#if WITHLTDCHW
	// STM32xxx LCD-TFT Controller (LTDC)
	// RENESAS Video Display Controller 5
	arm_hardware_ltdc_initialize();
	colmain_setcolors(COLORMAIN_WHITE, COLORMAIN_BLACK);
	arm_hardware_ltdc_main_set((uintptr_t) colmain_fb_draw());

#endif /* WITHLTDCHW */

#if LCDMODE_HARD_SPI
#elif LCDMODE_HARD_I2C
#elif LCDMODE_LTDC
#else
	#if LCDMODE_HD44780 && (LCDMODE_SPI == 0)
		hd44780_io_initialize();
	#else /* LCDMODE_HD44780 && (LCDMODE_SPI == 0) */
		DISPLAY_BUS_INITIALIZE();	// see LCD_CONTROL_INITIALIZE, LCD_DATA_INITIALIZE_WRITE
	#endif /* LCDMODE_HD44780 && (LCDMODE_SPI == 0) */
#endif
	debug_printf_P(PSTR("display_hardware_initialize done\n"));
}

// https://habr.com/ru/post/166317/

//	Hue — тон, цикличная угловая координата.
//	Value, Brightness — яркость, воспринимается как альфа-канал, при v=0 пиксель не светится,
//	при v=17 — светится максимально ярко, в зависимости от H и S.
//	Saturation. С отсутствием фона, значения  дадут не серый цвет, а белый разной яркости,
//	поэтому параметр W=Smax-S можно называть Whiteness — он отражает степень «белизны» цвета.
//	При W=0, S=Smax=15 цвет полностью определяется Hue, при S=0, W=Wmax=15 цвет пикселя
//	будет белым.

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} RGB_t;

typedef struct {
    uint8_t h;
    uint8_t s;
    uint8_t v;
} HSV_t;

const uint8_t max_whiteness = 15;
const uint8_t max_value = 17;

enum
{
	sixth_hue = 16,
	third_hue = sixth_hue * 2,
	half_hue = sixth_hue * 3,
	two_thirds_hue = sixth_hue * 4,
	five_sixths_hue = sixth_hue * 5,
	full_hue = sixth_hue * 6
};

RGB_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return (RGB_t) { r, g, b };
}

HSV_t hsv(uint8_t h, uint8_t s, uint8_t v)
{
    return (HSV_t) { h, s, v };
}

RGB_t hsv2rgb(HSV_t hsv)
{
	const RGB_t black = { 0, 0, 0 };

    if (hsv.v == 0) return black;

    uint8_t high = hsv.v * max_whiteness;//channel with max value
    if (hsv.s == 0) return rgb(high, high, high);

    uint8_t W = max_whiteness - hsv.s;
    uint8_t low = hsv.v * W;//channel with min value
    uint8_t rising = low;
    uint8_t falling = high;

    const uint8_t h_after_sixth = hsv.h % sixth_hue;
    if (h_after_sixth > 0)
    {
    	//not at primary color? ok, h_after_sixth = 1..sixth_hue - 1
        const uint8_t z = hsv.s * (uint8_t) (hsv.v * h_after_sixth) / sixth_hue;
        rising += z;
        falling -= z + 1;//it's never 255, so ok
    }

    uint8_t H = hsv.h;
    while (H >= full_hue)
    	H -= full_hue;

    if (H < sixth_hue) return rgb(high, rising, low);
    if (H < third_hue) return rgb(falling, high, low);
    if (H < half_hue) return rgb(low, high, rising);
    if (H < two_thirds_hue) return rgb(low, falling, high);
    if (H < five_sixths_hue) return rgb(rising, low, high);
    return rgb(high, low, falling);
}
