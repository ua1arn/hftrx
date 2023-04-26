/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Доработки для LS020 Василий Линывый, livas60@mail.ru
//

#include "hardware.h"
#include "board.h"
#include "display.h"
#include "formats.h"
#include "spi.h"	// hardware_spi_master_send_frame
#include "display2.h"
#include <string.h>


const char * savestring = "no data";
const char * savewhere = "no func";

#if LCDMODE_LTDC

#define FONTSHERE 1

#include "fontmaps.h"

#if WITHALTERNATIVEFONTS
	uint8_t const * font_big = ltdc_CenturyGothic_big [0] [0];
	uint8_t const * font_half = ltdc_CenturyGothic_half [0] [0];
	const size_t size_bigfont = sizeof ltdc_CenturyGothic_big [0] [0];
	const size_t size_halffont = sizeof ltdc_CenturyGothic_half [0] [0];
#else
	uint8_t const * font_big = S1D13781_bigfont_LTDC [0] [0];
	uint8_t const * font_half = S1D13781_halffont_LTDC [0] [0];
	const size_t size_bigfont = sizeof S1D13781_bigfont_LTDC [0] [0];
	const size_t size_halffont = sizeof S1D13781_halffont_LTDC [0] [0];
#endif /* WITHALTERNATIVEFONTS */

//
//#if ! LCDMODE_LTDC_L24
//#include "./byte2crun.h"
//#endif /* ! LCDMODE_LTDC_L24 */

typedef PACKEDCOLORPIP_T FRAMEBUFF_T [LCDMODE_MAIN_PAGES] [GXSIZE(DIM_SECOND, DIM_FIRST)];

#if defined (SDRAM_BANK_ADDR) && LCDMODE_LTDCSDRAMBUFF && LCDMODE_LTDC
	#define framebuff (* (FRAMEBUFF_T *) SDRAM_BANK_ADDR)
#else /* defined (SDRAM_BANK_ADDR) && LCDMODE_LTDCSDRAMBUFF && LCDMODE_LTDC */
	//#define framebuff (framebuff0)
	//extern FRAMEBUFF_T framebuff0;	//L8 (8-bit Luminance or CLUT)
#endif /* defined (SDRAM_BANK_ADDR) && LCDMODE_LTDCSDRAMBUFF && LCDMODE_LTDC */

#if ! defined (SDRAM_BANK_ADDR)
	// буфер экрана
	RAMFRAMEBUFF ALIGNX_BEGIN FRAMEBUFF_T fbfX ALIGNX_END;

	static uint_fast8_t drawframe;

	// переключиться на использование для DRAW следующего фреймбуфера (его номер возвращается)
	uint_fast8_t colmain_fb_next(void)
	{
		drawframe = (drawframe + 1) % LCDMODE_MAIN_PAGES;
		return drawframe;
	}

	PACKEDCOLORPIP_T *
	colmain_fb_draw(void)
	{
		return fbfX [drawframe];
	}

	void colmain_fb_initialize(void)
	{
//		uint_fast8_t i;
//		for (i = 0; i < LCDMODE_MAIN_PAGES; ++ i)
//			memset(fbfX [i], 0, sizeof fbfX [0]);
	}

	uint_fast8_t colmain_getindexbyaddr(uintptr_t addr)
	{
		uint_fast8_t i;
		for (i = 0; i < LCDMODE_MAIN_PAGES; ++ i)
		{
			if ((uintptr_t) fbfX [i] == addr)
				return i;
		}
		ASSERT(0);
		return 0;
	}

#elif WITHSDRAMHW && LCDMODE_LTDCSDRAMBUFF

	// переключиться на использование для DRAW следующего фреймбуфера (его номер возвращается)
	uint_fast8_t colmain_fb_next(void)
	{
		return 0;
	}

	PACKEDCOLORPIP_T *
	colmain_fb_draw(void)
	{
		return & framebuff[0][0];
	}

	void colmain_fb_initialize(void)
	{
		//memset(framebuff, 0, sizeof framebuff);
	}

	uint_fast8_t colmain_getindexbyaddr(uintptr_t addr)
	{
		uint_fast8_t i;
		for (i = 0; i < LCDMODE_MAIN_PAGES; ++ i)
		{
			if ((uintptr_t) framebuff [i] == addr)
				return i;
		}
		ASSERT(0);
		return 0;
	}

#else
	RAMFRAMEBUFF ALIGNX_BEGIN PACKEDCOLORPIP_T fbf [GXSIZE(DIM_SECOND, DIM_FIRST)] ALIGNX_END;

	// переключиться на использование для DRAW следующего фреймбуфера (его номер возвращается)
	uint_fast8_t colmain_fb_next(void)
	{
		return 0;
	}

	PACKEDCOLORPIP_T *
	colmain_fb_draw(void)
	{
		return fbf;
	}

	void colmain_fb_initialize(void)
	{
		//memset(fbf, 0, sizeof fbf);
	}

	uint_fast8_t colmain_getindexbyaddr(uintptr_t addr)
	{
		return 0;
	}


#endif /* LCDMODE_LTDC */

#if LCDMODE_LTDC

void display_putpixel(
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	)
{
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	colpip_putpixel(buffer, dx, dy, x, y, color);
}

/* заполнение прямоугольника на основном экране произвольным цветом
*/
void
display_fillrect(
	uint_fast16_t x, uint_fast16_t y, 	// координаты в пикселях
	uint_fast16_t w, uint_fast16_t h, 	// размеры в пикселях
	COLORPIP_T color
	)
{
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;

	colpip_fillrect(buffer, dx, dy, x, y, w, h, color);
}

/* рисование линии на основном экране произвольным цветом
*/
void
display_line(
	int x1, int y1,
	int x2, int y2,
	COLORPIP_T color
	)
{
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;

	colpip_line(buffer, dx, dy, x1, y1, x2, y2, color, 0);
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
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;

#if WITHDMA2DHW && LCDMODE_LTDC

#if LCDMODE_HORFILL && defined (DMA2D_FGPFCCR_CM_VALUE_MAIN)
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	/* TODO: В DMA2D нет средств управления направлением пересылки, потому данный код копирует сам на себя данные (размножает) */
	/* исходный растр */
	DMA2D->FGMAR = (uintptr_t) colpip_mem_at(buffer, dx, dy, y0 + 0, x0);
	DMA2D->FGOR = (DMA2D->FGOR & ~ (DMA2D_FGOR_LO)) |
		((DIM_X - w) << DMA2D_FGOR_LO_Pos) |
		0;
	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) colpip_mem_at(buffer, dx, dy, y0 + n, x0);
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
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;

#if WITHDMA2DHW && LCDMODE_LTDC
#if LCDMODE_HORFILL && defined (DMA2D_FGPFCCR_CM_VALUE_MAIN)
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд

	/* исходный растр */
	DMA2D->FGMAR = (uintptr_t) colpip_mem_at(buffer, dx, dy, y0 + n, x0);
	DMA2D->FGOR = (DMA2D->FGOR & ~ (DMA2D_FGOR_LO)) |
		((DIM_X - w) << DMA2D_FGOR_LO_Pos) |
		0;
	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) colpip_mem_at(buffer, dx, dy, y0 + 0, x0);
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

static PACKEDCOLORPIP_T ltdc_fg = COLORMAIN_WHITE, ltdc_bg = COLORMAIN_BLACK;

#if ! LCDMODE_LTDC_L24
static const FLASHMEM PACKEDCOLORPIP_T (* byte2runmain) [256][8] = & byte2runmain_COLORMAIN_WHITE_COLORMAIN_BLACK;
//static const FLASHMEM PACKEDCOLORPIP_T (* byte2runpip) [256][8] = & byte2runpip_COLORPIP_WHITE_COLORPIP_BLACK;
#endif /* ! LCDMODE_LTDC_L24 */

void colmain_setcolors(COLORPIP_T fg, COLORPIP_T bg)
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

void colmain_setcolors3(COLORPIP_T fg, COLORPIP_T bg, COLORPIP_T fgbg)
{
	colmain_setcolors(fg, bg);
}

/* индивидуальные функции драйвера дисплея - реализованы в соответствующем из файлов */
void display_clear(void)
{
	const COLORPIP_T bg = display_getbgcolor();
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();

	colpip_fillrect(buffer, DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, bg);
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
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	ASSERT(value <= topvalue);
	ASSERT(tracevalue <= topvalue);
	const uint_fast16_t wfull = GRID2X(width);
	const uint_fast16_t h = SMALLCHARH; //GRID2Y(1);
	const uint_fast16_t wpart = (uint_fast32_t) wfull * value / topvalue;
	const uint_fast16_t wmark = (uint_fast32_t) wfull * tracevalue / topvalue;
	const uint_fast8_t hpattern = 0x33;

	colpip_fillrect(buffer, dx, dy, 	x, y, 			wpart, h, 			ltdc_fg);
	colpip_fillrect(buffer, dx, dy, 	x + wpart, y, 	wfull - wpart, h, 	ltdc_bg);
	if (wmark < wfull && wmark >= wpart)
		colpip_fillrect(buffer, dx, dy, x + wmark, y, 	1, h, 				ltdc_fg);
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
	PACKEDCOLORPIP_T color
	)
{
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	volatile PACKEDCOLORPIP_T * const tgr = colpip_mem_at(buffer, dx, dy, x, y);
	* tgr = color;
	//dcache_clean((uintptr_t) tgr, sizeof * tgr);
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
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	PACKEDCOLORPIP_T * const tgr = colpip_mem_at(buffer, dx, dy, x, y);
	// размещаем пиксели по горизонтали
	// TODO: для паттернов шире чем восемь бит, повторить нужное число раз.
	const FLASHMEM PACKEDCOLORPIP_T * const pcl = (* byte2runmain) [pattern];
	memcpy(tgr, pcl, sizeof (* pcl) * w);
	//dcache_clean((uintptr_t) tgr, sizeof (PACKEDCOLORPIP_T) * w);
#endif /* LCDMODE_LTDC_L24 */
}

/* использование альманахов отображаемых шрифтов */
static void colmain_fontbuff_draw(
	PACKEDCOLORPIP_T * const __restrict buffer,
	const uint_fast16_t dx, const uint_fast16_t dy,
	uint_fast16_t x, uint_fast16_t y,	/* координаты в буфере */
	uint_fast8_t c	/* отображаемый символ */
	)
{

}

/* предварительное посторение альманахов отображаемых шрифтов */
static void colmain_fontbuff_initialize(void)
{

}

#if LCDMODE_HORFILL

// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
void RAMFUNC ltdc_horizontal_pixels(
	PACKEDCOLORPIP_T * tgr,		// target raster
	const FLASHMEM uint8_t * raster,
	uint_fast16_t width	// number of bits (start from LSB first byte in raster)
	)
{
	uint_fast16_t col;
	uint_fast16_t w = width;

	for (col = 0; w >= 8; col += 8, w -= 8)
	{
		const FLASHMEM PACKEDCOLORPIP_T * const pcl = (* byte2runmain) [* raster ++];
		memcpy(tgr + col, pcl, sizeof (* tgr) * 8);
	}
	if (w != 0)
	{
		const FLASHMEM PACKEDCOLORPIP_T * const pcl = (* byte2runmain) [* raster ++];
		memcpy(tgr + col, pcl, sizeof (* tgr) * w);
	}
	//dcache_clean((uintptr_t) tgr, sizeof (* tgr) * width);
}


uint_fast16_t
RAMFUNC_NONILINE ltdc_horizontal_put_char_unified(
	const FLASHMEM uint8_t * fontraster,
	uint_fast8_t width,		// пикселей в символе по горизонтали знакогнератора
	uint_fast8_t width2,	// пикселей в символе по горизонтали отображается (для уменьшеных в ширину символов большиз шрифтов)
	uint_fast8_t height,	// строк в символе по вертикали
	uint_fast8_t bytesw,	// байтов в одной строке символа
	PACKEDCOLORPIP_T * const __restrict buffer,
	const uint_fast16_t dx,
	const uint_fast16_t dy,
	uint_fast16_t x, uint_fast16_t y,
	uint_fast8_t c
	)
{
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < height; ++ cgrow)
	{
		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(buffer, dx, dy, x, y + cgrow);
		ltdc_horizontal_pixels(tgr, & fontraster [(c * height + cgrow) * bytesw], width2);
	}
	return x + width2;
}

// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// return new x
static uint_fast16_t
RAMFUNC_NONILINE
ltdc_horizontal_put_char_small(uint_fast16_t x, uint_fast16_t y, char cc)
{
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	return ltdc_horizontal_put_char_unified(S1D13781_smallfont_LTDC [0] [0], SMALLCHARW, SMALLCHARW, SMALLCHARH, sizeof S1D13781_smallfont_LTDC [0] [0], buffer, dx, dy, x, y, c);
//	const uint_fast8_t width = SMALLCHARW;
//	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
//	uint_fast8_t cgrow;
//	for (cgrow = 0; cgrow < SMALLCHARH; ++ cgrow)
//	{
//		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(buffer, dx, dy, x, y + cgrow);
//		ltdc_horizontal_pixels(tgr, S1D13781_smallfont_LTDC [c] [cgrow], width);
//	}
//	return x + width;
}

// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
// return new x coordinate
static uint_fast16_t RAMFUNC_NONILINE ltdc_horizontal_put_char_big(uint_fast16_t x, uint_fast16_t y, char cc)
{
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	const uint_fast8_t width = ((cc == '.' || cc == '#') ? BIGCHARW_NARROW  : BIGCHARW);	// полнаяширина символа в пикселях
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	return ltdc_horizontal_put_char_unified(font_big, BIGCHARW, width, BIGCHARH, size_bigfont, buffer, dx, dy, x, y, c);
//	uint_fast8_t cgrow;
//	for (cgrow = 0; cgrow < BIGCHARH; ++ cgrow)
//	{
//		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(buffer, dx, dy, x, y + cgrow);
//		ltdc_horizontal_pixels(tgr, S1D13781_bigfont_LTDC [c] [cgrow], width);
//	}
//	return x + width;
}

// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
// return new x coordinate
static uint_fast16_t RAMFUNC_NONILINE ltdc_horizontal_put_char_half(uint_fast16_t x, uint_fast16_t y, char cc)
{
	PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	const uint_fast8_t width = HALFCHARW;
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	return ltdc_horizontal_put_char_unified(font_half, HALFCHARW, width, HALFCHARH, size_halffont, buffer, dx, dy, x, y, c);
//	uint_fast8_t cgrow;
//	for (cgrow = 0; cgrow < HALFCHARH; ++ cgrow)
//	{
//		PACKEDCOLORPIP_T * const tgr = colpip_mem_at(buffer, dx, dy, x, y + cgrow);
//		ltdc_horizontal_pixels(tgr, S1D13781_halffont_LTDC [c] [cgrow], width);
//	}
//	return x + width;
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
	savewhere = __func__;
#if LCDMODE_HORFILL
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	return ltdc_horizontal_put_char_big(x, y, c);
#else /* LCDMODE_HORFILL */
	return ltdc_vertical_put_char_big(x, y, c);
#endif /* LCDMODE_HORFILL */
}

uint_fast16_t display_put_char_half(uint_fast16_t x, uint_fast16_t y, uint_fast8_t c, uint_fast8_t lowhalf)
{
	savewhere = __func__;
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


#if LCDMODE_LQ043T3DX02K || LCDMODE_AT070TN90 || LCDMODE_AT070TNA2 || LCDMODE_TCG104XGLPAPNN || LCDMODE_H497TLB01P4

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

void display_set_contrast(uint_fast8_t v)
{
}

// для framebufer дисплеев - вытолкнуть кэш память
// Функция используется только в тестах и для выдачи аварийных сообщений.
// Ждать синхронизации дисплея не требуется.
void display_flush(void)
{
	const uintptr_t frame = (uintptr_t) colmain_fb_draw();
//	char s [32];
//	local_snprintf_P(s, 32, "F=%08lX", (unsigned long) frame);
//	display_at(0, 0, s);
	dcache_clean(frame, (uint_fast32_t) GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORPIP_T));
	hardware_ltdc_main_set_no_vsync(frame);
}

/* переключаем на следующий фреймбуфер. Модификация этой памяти больше производиться не будет. */
void display_nextfb(void)
{
	const uintptr_t frame = (uintptr_t) colmain_fb_draw();	// Тот буфер, в котором рисовали, станет отображаемым
//	char s [32];
//	local_snprintf_P(s, 32, "B=%08lX ", (unsigned long) frame);
//	display_at(0, 0, s);
	ASSERT((frame % DCACHEROWSIZE) == 0);
	dcache_clean_invalidate(frame, (uint_fast32_t) GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORPIP_T));
	hardware_ltdc_main_set(frame);
	const unsigned page = colmain_fb_next();	// возвращает новый индекс страницы отрисовки
#if WITHOPENVG
	openvg_next(page);
#endif /* WITHOPENVG */
}

/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{
#if WITHOPENVG
	PACKEDCOLORPIP_T * frames [LCDMODE_MAIN_PAGES];
	unsigned i;
	for (i = 0; i < LCDMODE_MAIN_PAGES; ++ i)
	{
		frames [i] = fbfX [i];
	}
	openvg_init(frames);
#endif /* WITHOPENVG */
}

/* Разряжаем конденсаторы питания */
void display_uninitialize(void)
{
#if WITHOPENVG
	openvg_deinit();
#endif /* WITHOPENVG */
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
// Используется при выводе на графический индикатор,
static void
display_string(uint_fast8_t xcell, uint_fast8_t ycell, const char * s, uint_fast8_t lowhalf)
{
	savestring = s;
	savewhere = __func__;
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
		dcache_clean((uintptr_t) buffer, sizeof (* buffer) * MGSIZE(dx, dy));	// количество байтов
	#endif

	uint_fast8_t lowhalf = (dy) / 8 - 1;
	if (lowhalf == 0)
		return;
	do
	{
		uint_fast8_t pos;
		const GX_t * const p = buffer + lowhalf * MGADJ(dx);	// начало данных горизонтальной полосы в памяти
		//PRINTF(PSTR("display_showbuffer: col=%d, row=%d, lowhalf=%d\n"), col, row, lowhalf);
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
	1000000000U,
	100000000U,
	10000000U,
	1000000U,
	100000U,
	10000U,
	1000U,
	100U,
	10U,
	1U,
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
//	if (width > ARRAY_SIZE(vals10))
//		width = ARRAY_SIZE(vals10);
	//const uint_fast8_t comma2 = comma + 3;		// comma position (from right, inside width)
	const uint_fast8_t j = ARRAY_SIZE(vals10) - rj;
	uint_fast8_t i = (j - width);
	uint_fast8_t z = blinkpos == 255 ? 1 : 0;	// only zeroes
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
display_value_lower(
	uint_fast8_t x,	// x координата начала вывода значения
	uint_fast8_t y,	// y координата начала вывода значения
	uint_fast32_t freq,
	uint_fast8_t width, // = 8;	// full width
	uint_fast8_t comma, // = 2;	// comma position (from right, inside width)
	uint_fast8_t rj	// = 1;		// right truncated
	)
{
//	if (width > ARRAY_SIZE(vals10))
//		width = ARRAY_SIZE(vals10);
	const uint_fast8_t j = ARRAY_SIZE(vals10) - rj;
	uint_fast8_t i = (j - width);
	uint_fast8_t z = 1;	// only zeroes
	uint_fast8_t half = 0;	// отображаем после второй запатой - маленьким шрифтом

	uint_fast16_t ypix;
	uint_fast16_t xpix = display_wrdatabig_begin(x, y, & ypix);
	for (; i < j; ++ i)
	{
		const ldiv_t res = ldiv(freq, vals10 [i]);
		const uint_fast8_t g = (j - i);		// десятичная степень текущего разряда на отображении

		if (comma == g || comma + 3 == g)
		{
			z = 0;
			xpix = display_put_char_big(xpix, ypix, '.', 0);
		}

		if (z == 1 && (i + 1) < j && res.quot == 0)
			xpix = display_put_char_big(xpix, ypix, ' ', 0);	// supress zero
		else
		{
			z = 0;
			xpix = display_put_char_half(xpix, ypix, '0' + res.quot, 0);
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
//	if (width > ARRAY_SIZE(vals10))
//		width = ARRAY_SIZE(vals10);
	const uint_fast8_t wsign = (width & WSIGNFLAG) != 0;
	const uint_fast8_t wminus = (width & WMINUSFLAG) != 0;
	const uint_fast8_t j = ARRAY_SIZE(vals10) - rj;
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

void display_value_small_xy(
	uint_fast16_t xpix,	// x координата начала вывода значения
	uint_fast16_t ypix,	// y координата начала вывода значения
	int_fast32_t freq,
	COLOR565_T fg
	)
{
	uint_fast8_t width = 9;
	uint_fast8_t comma = 3;
	uint_fast8_t comma2 = 6;
	uint_fast8_t rj = 0;
	uint_fast8_t lowhalf = 0;
	const uint_fast8_t wsign = (width & WSIGNFLAG) != 0;
	const uint_fast8_t wminus = (width & WMINUSFLAG) != 0;
	const uint_fast8_t j = ARRAY_SIZE(vals10) - rj;
	uint_fast8_t i = j - (width & WWIDTHFLAG);	// Номер цифры по порядку
	uint_fast8_t z = 1;	// only zeroes

	if (wsign || wminus)
	{
		// отображение со знаком.
		z = 0;
		if (freq < 0)
		{
			xpix = display_put_char_small_xy(xpix, ypix, '-', fg);
			freq = - freq;
		}
		else if (wsign)
			xpix = display_put_char_small_xy(xpix, ypix, '+', fg);
		else
			xpix = display_put_char_small_xy(xpix, ypix, ' ', fg);
	}
	for (; i < j; ++ i)
	{
		const ldiv_t res = ldiv(freq, vals10 [i]);
		const uint_fast8_t g = (j - i);
		// разделитель десятков мегагерц
		if (comma2 == g)
		{
			xpix = display_put_char_small_xy(xpix, ypix, (z == 0) ? '.' : ' ', fg);
		}
		else if (comma == g)
		{
			z = 0;
			xpix = display_put_char_small_xy(xpix, ypix, '.', fg);
		}

		if (z == 1 && (i + 1) < j && res.quot == 0)
			xpix = display_put_char_small_xy(xpix, ypix, ' ', fg);	// supress zero
		else
		{
			z = 0;
			xpix = display_put_char_small_xy(xpix, ypix, '0' + res.quot, fg);
		}
		freq = res.rem;
	}
}

#if LCDMODE_COLORED
static COLORPIP_T bgcolor = COLORMAIN_BLACK;
#endif /* LCDMODE_COLORED */

void
display_setbgcolor(COLORPIP_T c)
{
#if LCDMODE_COLORED
	bgcolor = c;
#endif /* LCDMODE_COLORED */
}

COLORPIP_T
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

//#warning colpip_copy_to_draw is dummy for this LCDMODE_LTDC combination

void colpip_copy_to_draw(
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

// Скопировать цветной буфр в drqw buffer
void colpip_copy_to_draw(
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
	hwaccel_bitblt(
		(uintptr_t) colmain_fb_draw(), sizeof (PACKEDCOLORPIP_T) * GXSIZE(DIM_X, DIM_Y),	// target area invalidate parameters
		colpip_mem_at(colmain_fb_draw(), DIM_X, DIM_Y, col, row), DIM_X, DIM_Y,
		srcinvalidateaddr, srcinvalidatesize,	// параметры clean источника
		buffer, dx, dy,
		0, 0,
		dx, dy
		);
#else /* LCDMODE_HORFILL */
	hwaccel_bitblt(
		(uintptr_t) colmain_fb_draw(), sizeof (PACKEDCOLORPIP_T) * GXSIZE(DIM_X, DIM_Y),	// target area invalidate parameters
		colpip_mem_at(colmain_fb_draw(), DIM_X, DIM_Y, col, row), DIM_X, DIM_Y,
		srcinvalidateaddr, srcinvalidatesize,	// параметры clean источника
		buffer, dx, dy,
		0, 0,
		dx, dy,
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
void colpip_copy_to_draw(
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

#if WITHLTDCHW

#if LCDMODE_LQ043T3DX02K
	// Sony PSP-1000 display panel
	// LQ043T3DX02K panel (272*480)
	// RK043FN48H-CT672B  panel (272*480) - плата STM32F746G-DISCO
	/**
	  * @brief  RK043FN48H Size
	  */
const videomode_t vdmode0 =
{
	.width = 480,				/* LCD PIXEL WIDTH            */
	.height = 272,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  RK043FN48H Timing
	  */
	.hsync = 41,				/* Horizontal synchronization */
	.hbp = 2,				/* Horizontal back porch      */
	.hfp = 2,				/* Horizontal front porch     */

	.vsync = 10,				/* Vertical synchronization   */
	.vbp = 2,					/* Vertical back porch        */
	.vfp = 4,					/* Vertical front porch       */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 1,		/* требуется формирование сигнала RESET для панели по этому выводу после начала формирования синхронизации */
	//.ltdc_dotclk = 9000000uL,	// частота пикселей при работе с интерфейсом RGB
	.fps = 60	/* frames per second */
};
	/* SONY PSP-1000 display (4.3") required. */
	/* Используется при BOARD_DEMODE = 0 */
	//#define BOARD_DERESET 1		/* требуется формирование сигнала RESET для панели по этому выводу после начала формирования синхронизации */

#elif LCDMODE_AT070TN90

	/* AT070TN90 panel (800*480) - 7" display HV mode */
const videomode_t vdmode0 =
{
	.width = 800,			/* LCD PIXEL WIDTH            */
	.height = 480,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  AT070TN90 Timing
	  * MODE=0 (DE)
	  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
	  * MODE=1 (SYNC)
	  * When selected sync mode, de must be grounded.
	  */
	.hsync = 40,				/* Horizontal synchronization 1..40 */
	.hbp = 6,				/* Horizontal back porch      */
	.hfp = 210,				/* Horizontal front porch  16..354   */

	.vsync = 20,				/* Vertical synchronization 1..20  */
	.vbp = 3,				/* Vertical back porch      */
	.vfp = 22,				/* Vertical front porch  7..147     */

	/* Accumulated parameters for this display */
	//LEFTMARGIN = 46,		/* horizontal blanking EXACTLY */
	//TOPMARGIN = 23,			/* vertical blanking EXACTLY */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	//.ltdc_dotclk = 30000000uL,	// частота пикселей при работе с интерфейсом RGB
	.fps = 60	/* frames per second */
};

#elif 1 && LCDMODE_AT070TNA2

/* AT070TNA2 panel (1024*600) - 7" display HV mode */
// HX8282-A01.pdf, page 38
const videomode_t vdmode0 =
{
	.width = 1024,			/* LCD PIXEL WIDTH            */
	.height = 600,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  AT070TN90 Timing
	  * MODE=0 (DE)
	  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
	  * MODE=1 (SYNC)
	  * When selected sync mode, de must be grounded.
	  */
	/* from r6dan: thb+thpw=160 is fixed */
	/* EK79001.PDF: */
	.hsync = 20,			/* Horizontal synchronization 1..140 */
	.hbp = 140,				/* Horizontal back porch */
	.hfp = 160,				/* Horizontal front porch  16..216  (r6dan: 140-160-180)  */

	/* from r6dan: tvb+tvpw=23 is fixed */
	.vsync = 3,				/* Vertical synchronization 1..20 */
	.vbp = 20,				/* Vertical back porch */
	.vfp = 12,				/* Vertical front porch  1..127  (r6dan: 2-12-22) */

	/* Accumulated parameters for this display */
	//LEFTMARGIN = 160,		/* horizontal blanking EXACTLY */
	//TOPMARGIN = 23,			/* vertical blanking EXACTLY */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	//.ltdc_dotclk = 51200000uL,	// частота пикселей при работе с интерфейсом RGB 40.8..67.2
	.fps = 60	/* frames per second */
};


#elif 0 && LCDMODE_AT070TNA2
	// 1280 * 720
	/* AT070TNA2 panel (1024*600) - 7" display HV mode */
	// HX8282-A01.pdf, page 38
	//	pinfo.xres = 1280;
	//	pinfo.yres = 720;
	//	pinfo.type = HDMI_PANEL;
	//	pinfo.pdest = DISPLAY_1;
	//	pinfo.wait_cycle = 0;
	//	pinfo.bpp = 24;
	//	pinfo.fb_num = 2;
	//	pinfo.clk_rate = 74250000;
	//	pinfo.lcdc.h_back_porch = 124;
	//	pinfo.lcdc.h_front_porch = 110;
	//	pinfo.lcdc.h_pulse_width = 136;
	//	pinfo.lcdc.v_back_porch = 19;
	//	pinfo.lcdc.v_front_porch = 5;
	//	pinfo.lcdc.v_pulse_width = 6;
	//	pinfo.lcdc.border_clr = 0;
	//	pinfo.lcdc.underflow_clr = 0xff;
	//	pinfo.lcdc.hsync_skew = 0;
const videomode_t vdmode0 =
{
	.width = 1280,			/* LCD PIXEL WIDTH            */
	.height = 720,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  AT070TN90 Timing
	  * MODE=0 (DE)
	  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
	  * MODE=1 (SYNC)
	  * When selected sync mode, de must be grounded.
	  */
	.hsync = 136,			/* Horizontal synchronization 1..140 */
	.hbp = xxx,				/* Horizontal back porch  xxx   */
	.hfp = 110,				/* Horizontal front porch  16..216   */

	.vsync = 6,				/* Vertical synchronization 1..20  */
	.vbp = xx,				/* Vertical back porch  xxx   */
	.vfp = 5,				/* Vertical front porch  1..127     */

	/* Accumulated parameters for this display */
	//LEFTMARGIN = 160,		/* horizontal blanking EXACTLY */
	//TOPMARGIN = 23,			/* vertical blanking EXACTLY */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	//.ltdc_dotclk = 74250000uL,	// частота пикселей при работе с интерфейсом RGB
	.fps = 60	/* frames per second */
};

#elif LCDMODE_TCG104XGLPAPNN

/* TCG104XGLPAPNN-AN30 panel (1024*768) - 10.4" display */
// TCG104XGLPAPNN-AN30-1384899.pdf
// horizontal period 1114 / 1344 / 1400
// vertical period 778 / 806 / 845
// Synchronization method should be DE mode
const videomode_t vdmode0 =
{
	.width = 1024,			/* LCD PIXEL WIDTH            */
	.height = 768,			/* LCD PIXEL HEIGHT           */

	.hsync = 120,			/* Horizontal synchronization 1..140 */
	.hbp = 100,				/* Horizontal back porch  xxx   */
	.hfp = 100,				/* Horizontal front porch  16..216   */

	.vsync = 20,				/* Vertical synchronization 1..20  */
	.vbp = 9,				/* Vertical back porch  xxx   */
	.vfp = 9,				/* Vertical front porch  1..127     */

	/* Accumulated parameters for this display */
	//LEFTMARGIN = 160,		/* horizontal blanking EXACTLY */
	//TOPMARGIN = 23,			/* vertical blanking EXACTLY */

	// Synchronization method should be DE mode
	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	//.ltdc_dotclk = 51200000uL,	// частота пикселей при работе с интерфейсом RGB 40.8..67.2
	.fps = 60	/* frames per second 50 60 70 */
};

#elif LCDMODE_ILI8961
	// HHT270C-8961-6A6 (320*240)
const videomode_t vdmode0 =
{
	.width = 320 * 3,				/* LCD PIXEL WIDTH            */
	.height = 240,			/* LCD PIXEL HEIGHT           */

	/**
	  * @brief  RK043FN48H Timing
	  */
	.hsync = 1,				/* Horizontal synchronization */
	.hbp = 2,				/* Horizontal back porch      */
	.hfp = 2,				/* Horizontal front porch     */

	.vsync = 1,				/* Vertical synchronization   */
	.vbp = 2,					/* Vertical back porch        */
	.vfp = 2,					/* Vertical front porch       */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	//.ltdc_dotclk = 24000000u,	// частота пикселей при работе с интерфейсом RGB
	.fps = 60	/* frames per second */
};

#elif LCDMODE_ILI9341
	// SF-TC240T-9370-T (320*240)
static const const videomode_t vdmode0 =
{

	.width = 240,				/* LCD PIXEL WIDTH            */
	.height = 320,			/* LCD PIXEL HEIGHT           */

	/**
	  * @brief  ILI9341 Timing
	  */
	.hsync = 10,				/* Horizontal synchronization */
	.hbp = 20,				/* Horizontal back porch      */
	.hfp = 10,				/* Horizontal front porch     */

	.vsync = 2,				/* Vertical synchronization   */
	.vbp = 2,					/* Vertical back porch        */
	.vfp = 4,					/* Vertical front porch       */

	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	//.ltdc_dotclk = 3000000uL,	// частота пикселей при работе с интерфейсом RGB
	.fps = 60	/* frames per second */
};

#elif LCDMODE_H497TLB01P4
	/* 720xRGBx1280 - 5" AMOELD Panel H497TLB01.4 */
	// See also:
	// https://github.com/bbelos/rk3188-kernel/blob/master/drivers/video/rockchip/transmitter/tc358768.c
	// https://github.com/tanish2k09/venom_kernel_aio_otfp/blob/master/drivers/input/touchscreen/mediatek/S3202/synaptics_dsx_i2c.c
	// https://stash.phytec.com/projects/TIRTOS/repos/vps-phytec/raw/src/boards/src/bsp_boardPriv.h?at=e8b92520f41e6523301d120dae15db975ad6d0da
	//https://code.ihub.org.cn/projects/825/repositories/874/file_edit_page?file_name=am57xx-idk-common.dtsi&path=arch%2Farm%2Fboot%2Fdts%2Fam57xx-idk-common.dtsi&rev=master
const videomode_t vdmode0 =
{
	.width = 720,			/* LCD PIXEL WIDTH            */
	.height = 1280,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  AT070TN90 Timing
	  * MODE=0 (DE)
	  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
	  * MODE=1 (SYNC)
	  * When selected sync mode, de must be grounded.
	  */
	.hsync = 5,				/* Horizontal synchronization 1..40 */
	.hbp = 11,				/* Horizontal back porch      */
	.hfp = 16,				/* Horizontal front porch  16..354   */

	.vsync = 5,				/* Vertical synchronization 1..20  */
	.vbp = 11,					/* Vertical back porch        */
	.vfp = 16,				/* Vertical front porch  7..147     */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	//.ltdc_dotclk = 3000000uL	// частота пикселей при работе с интерфейсом RGB
	.fps = 60	/* frames per second */
};

#elif LCDMODE_TV101WXM
	/* 720xRGBx1280 - 5" AMOELD Panel H497TLB01.4 */
	// See also:
	// https://github.com/bbelos/rk3188-kernel/blob/master/drivers/video/rockchip/transmitter/tc358768.c
	// https://github.com/tanish2k09/venom_kernel_aio_otfp/blob/master/drivers/input/touchscreen/mediatek/S3202/synaptics_dsx_i2c.c
	// https://stash.phytec.com/projects/TIRTOS/repos/vps-phytec/raw/src/boards/src/bsp_boardPriv.h?at=e8b92520f41e6523301d120dae15db975ad6d0da
	//https://code.ihub.org.cn/projects/825/repositories/874/file_edit_page?file_name=am57xx-idk-common.dtsi&path=arch%2Farm%2Fboot%2Fdts%2Fam57xx-idk-common.dtsi&rev=master
const videomode_t vdmode0 =
{
	.width = 800,			/* LCD PIXEL WIDTH            */
	.height = 1280,			/* LCD PIXEL HEIGHT           */
	/**
	  * @brief  AT070TN90 Timing
	  * MODE=0 (DE)
	  * When selected DE mode, VSYNC & HSYNC must pulled HIGH
	  * MODE=1 (SYNC)
	  * When selected sync mode, de must be grounded.
	  */
	.hsync = 5,				/* Horizontal synchronization 1..40 */
	.hbp = 11,				/* Horizontal back porch      */
	.hfp = 16,				/* Horizontal front porch  16..354   */

	.vsync = 5,				/* Vertical synchronization 1..20  */
	.vbp = 11,					/* Vertical back porch        */
	.vfp = 16,				/* Vertical front porch  7..147     */

	// MODE: DE/SYNC mode select.
	// DE MODE: MODE="1", VS and HS must pull high.
	// SYNC MODE: MODE="0". DE must be grounded
	.vsyncneg = 1,			/* Negative polarity required for VSYNC signal */
	.hsyncneg = 1,			/* Negative polarity required for HSYNC signal */
	.deneg = 0,				/* Negative DE polarity: (normal: DE is 0 while sync) */
	.lq43reset = 0,	// LQ043T3DX02K require DE reset
	//.ltdc_dotclk = 3000000uL	// частота пикселей при работе с интерфейсом RGB
	.fps = 60	/* frames per second */
};

#else
	#error Unsupported LCDMODE_xxx

#endif

#endif /* WITHLTDCHW */
/*
 * настройка портов для последующей работы с дополнительными (кроме последовательного канала)
 * сигналами дисплея.
 */

/* вызывается при запрещённых прерываниях. */
void display_hardware_initialize(void)
{
	PRINTF(PSTR("display_hardware_initialize start\n"));


#if WITHDMA2DHW
	// Image construction hardware
	arm_hardware_dma2d_initialize();

#endif /* WITHDMA2DHW */
#if WITHMDMAHW
	// Image construction hardware
	arm_hardware_mdma_initialize();

#endif /* WITHMDMAHW */

#if WITHLTDCHW
	const videomode_t * const vdmode = & vdmode0;
	colmain_fb_initialize();
	uintptr_t frames [LCDMODE_MAIN_PAGES];
	unsigned i;
	for (i = 0; i < LCDMODE_MAIN_PAGES; ++ i)
	{
		frames [i] = (uintptr_t) fbfX [i];
	}
	// STM32xxx LCD-TFT Controller (LTDC)
	// RENESAS Video Display Controller 5
	//PRINTF("display_getdotclock=%lu\n", (unsigned long) display_getdotclock(vdmode));
	hardware_ltdc_initialize(frames, vdmode);
	colmain_setcolors(COLORMAIN_WHITE, COLORMAIN_BLACK);
	colmain_fontbuff_initialize();
	hardware_ltdc_main_set((uintptr_t) colmain_fb_draw());
	hardware_ltdc_L8_palette();
#endif /* WITHLTDCHW */

#if LCDMODETX_TC358778XBG
	tc358768_initialize(vdmode);
	panel_initialize(vdmode);
#endif /* LCDMODETX_TC358778XBG */
#if LCDMODEX_SII9022A
	/* siiI9022A Lattice Semiconductor Corp HDMI Transmitter */
	sii9022x_initialize(vdmode);
#endif /* LCDMODEX_SII9022A */

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
	PRINTF(PSTR("display_hardware_initialize done\n"));
}

void display_hdmi_initialize(void)
{
	const videomode_t * const vdmode = & vdmode0;
//#if LCDMODETX_TC358778XBG
//	tc358768_initialize(vdmode);
//	panel_initialize(vdmode);
//#endif /* LCDMODETX_TC358778XBG */
#if LCDMODEX_SII9022A
	/* siiI9022A Lattice Semiconductor Corp HDMI Transmitter */
	sii9022x_initialize(vdmode);
#endif /* LCDMODEX_SII9022A */
}

void display_wakeup(void)
{
#if WITHLTDCHW
	const videomode_t * const vdmode = & vdmode0;
	colmain_fb_initialize();
	uintptr_t frames [LCDMODE_MAIN_PAGES];
	unsigned i;
	for (i = 0; i < LCDMODE_MAIN_PAGES; ++ i)
	{
		frames [i] = (uintptr_t) fbfX [i];
	}
	// STM32xxx LCD-TFT Controller (LTDC)
	// RENESAS Video Display Controller 5
	//PRINTF("display_getdotclock=%lu\n", (unsigned long) display_getdotclock(vdmode));
    hardware_ltdc_initialize(frames, vdmode);
#endif /* WITHLTDCHW */
#if LCDMODETX_TC358778XBG
    tc358768_wakeup(vdmode);
    panel_wakeup();
#endif /* LCDMODETX_TC358778XBG */
#if LCDMODEX_SII9022A
    // siiI9022A Lattice Semiconductor Corp HDMI Transmitter
    sii9022x_wakeup(vdmode);
#endif /* LCDMODEX_SII9022A */
}

// Palette reload
void display_palette(void)
{
#if WITHLTDCHW
	hardware_ltdc_L8_palette();
#endif /* WITHLTDCHW */
}
// https://habr.com/ru/post/166317/

//	Hue - тон, цикличная угловая координата.
//	Value, Brightness - яркость, воспринимается как альфа-канал, при v=0 пиксель не светится,
//	при v=17 - светится максимально ярко, в зависимости от H и S.
//	Saturation. С отсутствием фона, значения  дадут не серый цвет, а белый разной яркости,
//	поэтому параметр W=Smax-S можно называть Whiteness - он отражает степень "белизны" цвета.
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

#if WITHRLEDECOMPRESS

PACKEDCOLORPIP_T convert_565_to_a888(uint16_t color)
{
	uint8_t b5 = (color & 0x1F) << 3;
	uint8_t g6 = ((color & 0x7E0) >> 5) << 2;
	uint8_t r5 = ((color & 0xF800) >> 11) << 3;

	return TFTRGB(r5, g6, b5);
}

void graw_picture_RLE(uint16_t x, uint16_t y, const picRLE_t * picture, PACKEDCOLORPIP_T bg_color)
{
	uint_fast32_t i = 0;
	uint_fast16_t x1 = x, y1 = y;
	uint_fast16_t transparent_color = 0, count = 0;
	PACKEDCOLORPIP_T * const fr = colmain_fb_draw();

	while (y1 < y + picture->height)
	{
		if ((int16_t)picture->data [i] < 0) // no repeats
		{
			count = (-(int16_t)picture->data [i]);
			i ++;
			for (uint_fast16_t p = 0; p < count; p ++)
			{
				PACKEDCOLORPIP_T point = convert_565_to_a888(picture->data [i]);
				colpip_point(fr, DIM_X, DIM_Y, x1, y1, picture->data [i] == 0 ? bg_color : point);

				x1 ++;
				if (x1 >= x + picture->width)
				{
					x1 = x;
					y1 ++;
				}
				i ++;
			}
		}
		else // repeats
		{
			count = ((int16_t)picture->data [i]);
			i++;

			PACKEDCOLORPIP_T point = convert_565_to_a888(picture->data [i]);
			for (uint_fast16_t p = 0; p < count; p ++)
			{
				colpip_point(fr, DIM_X, DIM_Y, x1, y1, picture->data [i] == 0 ? bg_color : point);

				x1 ++;
				if (x1 >= x + picture->width)
				{
					x1 = x;
					y1 ++;
				}
			}
			i ++;
		}
	}
}

void graw_picture_RLE_buf(PACKEDCOLORPIP_T * const buf, uint_fast16_t dx, uint_fast16_t dy, uint16_t x, uint16_t y, const picRLE_t * picture, PACKEDCOLORPIP_T bg_color)
{
	uint_fast32_t i = 0;
	uint_fast16_t x1 = x, y1 = y;
	uint_fast16_t transparent_color = 0, count = 0;

	while (y1 < y + picture->height)
	{
		if ((int16_t)picture->data [i] < 0) // no repeats
		{
			count = (-(int16_t)picture->data [i]);
			i ++;
			for (uint_fast16_t p = 0; p < count; p++)
			{
				PACKEDCOLORPIP_T point = convert_565_to_a888(picture->data [i]);
				colpip_point(buf, dx, dy, x1, y1, picture->data [i] == transparent_color ? bg_color : point);

				x1 ++;
				if (x1 >= x + picture->width)
				{
					x1 = x;
					y1 ++;
				}
				i ++;
			}
		}
		else // repeats
		{
			count = ((int16_t)picture->data [i]);
			i ++;

			PACKEDCOLORPIP_T point = convert_565_to_a888(picture->data [i]);
			for (uint_fast16_t p = 0; p < count; p++)
			{
				colpip_point(buf, dx, dy, x1, y1, picture->data[i] == transparent_color ? bg_color : point);

				x1 ++;
				if (x1 >= x + picture->width)
				{
					x1 = x;
					y1 ++;
				}
			}
			i ++;
		}
	}
}

#endif /* WITHRLEDECOMPRESS */

void display_do_AA(PACKEDCOLORPIP_T * __restrict buffer,
		uint_fast16_t dx,
		uint_fast16_t dy,
		uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
		uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз)
		uint_fast16_t width,
		uint_fast16_t height)
{
	uint_fast32_t p1_r, p1_g, p1_b, p2_r, p2_g, p2_b, p3_r, p3_g, p3_b, p4_r, p4_g, p4_b, p_r, p_b, p_g;

	for (uint16_t x = col; x < col + width - 1; x ++)
	{
		for (uint16_t y = row; y < row + height - 1; y ++)
		{
			PACKEDCOLORPIP_T * p1 = colpip_mem_at(buffer, dx, dy, x, y);
			PACKEDCOLORPIP_T * p2 = colpip_mem_at(buffer, dx, dy, x + 1, y);
			PACKEDCOLORPIP_T * p3 = colpip_mem_at(buffer, dx, dy, x, y + 1);
			PACKEDCOLORPIP_T * p4 = colpip_mem_at(buffer, dx, dy, x + 1, y + 1);

			 if ((* p1 == * p2) && (* p1 == * p3) && (* p1 == * p4))	// если смежные пиксели одинакового цвета, пропустить расчет
				continue;

			p1_r = COLORPIP_R(* p1);
			p1_g = COLORPIP_G(* p1);
			p1_b = COLORPIP_B(* p1);

			p2_r = COLORPIP_R(* p2);
			p2_g = COLORPIP_G(* p2);
			p2_b = COLORPIP_B(* p2);

			p3_r = COLORPIP_R(* p3);
			p3_g = COLORPIP_G(* p3);
			p3_b = COLORPIP_B(* p3);

			p4_r = COLORPIP_R(* p4);
			p4_g = COLORPIP_G(* p4);
			p4_b = COLORPIP_B(* p4);

			p_r = (p1_r + p2_r + p3_r + p4_r) / 4;
			p_g = (p1_g + p2_g + p3_g + p4_g) / 4;
			p_b = (p1_b + p2_b + p3_b + p4_b) / 4;

			colpip_point(buffer, dx, dy, x, y, TFTRGB(p_r, p_g, p_b));
		}
	}
}
