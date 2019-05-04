/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Siemens S65 Display Control
// Индикатор 176*132 LPH8836-2 с контроллером Hitachi HD66773
// проверено с LPH8836-1, LPH8836-2 и LPH8836-4, 

#include "hardware.h"
#include "board.h"
#include "display.h"
#include <stdint.h>

#include "spifuncs.h"

#if LCDMODE_LPH88

#include "lph88.h"

#include "./fonts/ls020_font_small.h"
#include "./fonts/ls020_font_half.h"
#include "./fonts/ls020_font_big.h"

#define LPH88_SPIMODE SPIC_MODE3	// MODE3 only supported. The input level (SDI) is read on the rising edge of the SCL signal.


static void
lph88_reset(void)
{
	local_delay_ms(25);		// delay after power-up
	board_lcd_reset(0); 	// Pull RST pin down
	board_update();
	local_delay_ms(5);		// local_delay_ms 5 mS (minimum by documentation - 1 mS).
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
}

static void lph88_comtype(uint_fast8_t v)
{
	spi_select(targetlcd, LPH88_SPIMODE);
	
	spi_progval8_p1(targetlcd, 0x74);	// indicates command
	spi_progval8_p2(targetlcd, 0x00);	// hi
	spi_progval8_p2(targetlcd, v);		// lo
	spi_complete(targetlcd);
	
	spi_unselect(targetlcd);

}

static void lph88_comdat(uint_fast8_t hi, uint_fast8_t lo)
{
	spi_select(targetlcd, LPH88_SPIMODE);
	
	spi_progval8_p1(targetlcd, 0x76);	// indicates data
	spi_progval8_p2(targetlcd, hi);
	spi_progval8_p2(targetlcd, lo);
	spi_complete(targetlcd);
	
	spi_unselect(targetlcd);

}

static void lph88_comdat16(uint_fast16_t v)
{
	spi_select(targetlcd, LPH88_SPIMODE);
	
	spi_progval8_p1(targetlcd, 0x76);	// indicates data
	spi_progval8_p2(targetlcd, v >> 8);
	spi_progval8_p2(targetlcd, v);
	spi_complete(targetlcd);
	
	spi_unselect(targetlcd);
}

static void lph88_cmd(uint_fast8_t cmd, uint_fast16_t dat)
{
	lph88_comtype(cmd);
	lph88_comdat16(dat);
}

// в режим передачи данных переводим сразу по окончании команд.
static void lph88_put_char_begin(void)
{
	lph88_comtype(0x22);	// Write Data to GRAM (R22h)

	spi_select(targetlcd, LPH88_SPIMODE);
	spi_progval8_p1(targetlcd, 0x76);	// indicates data
}

static void lph88_put_char_end(void)
{
	spi_complete(targetlcd);
	spi_unselect(targetlcd);
}

static struct { uint_fast8_t first, second; } fgcolor, bkcolor;

static void lph88_setcolor(COLOR_T acolor, COLOR_T abkcolor)
{
	fgcolor.first = acolor >> 8;
	fgcolor.second = acolor >> 0;
	bkcolor.first = abkcolor >> 8;
	bkcolor.second = abkcolor >> 0;
}


static void lph88_pixel_p2(
	uint_fast8_t fg	// 0 - background color, other - foreground color
	)
{
	if (fg)
	{
		spi_progval8_p2(targetlcd, fgcolor.first);	// смотреть бит TRI а регистре 03
		spi_progval8_p2(targetlcd, fgcolor.second);	// смотреть бит TRI а регистре 03
	}
	else
	{
		spi_progval8_p2(targetlcd, bkcolor.first);	// смотреть бит TRI а регистре 03
		spi_progval8_p2(targetlcd, bkcolor.second);	// смотреть бит TRI а регистре 03
	}
}

// Выдать восемь цветных пикселей
static void 
lph88_pix8(
	uint_fast8_t v
	)
{
	lph88_pixel_p2(v & 0x01);
	lph88_pixel_p2(v & 0x02);
	lph88_pixel_p2(v & 0x04);
	lph88_pixel_p2(v & 0x08);
	lph88_pixel_p2(v & 0x10);
	lph88_pixel_p2(v & 0x20);
	lph88_pixel_p2(v & 0x40);
	lph88_pixel_p2(v & 0x80);
}

static uint_fast8_t
bigfont_decode(uint_fast8_t c)
{
	// '#' - узкий пробел
	if (c == ' ' || c == '#')
		return 11;
	if (c == '_')
		return 10;		// курсор - позиция редактирвания частоты
	if (c == '.')
		return 12;		// точка
	return c - '0';		// остальные - цифры 0..9
}


static uint_fast8_t
smallfont_decode(uint_fast8_t c)
{
	return c - ' ';
}


// Вызов этой функции только внутри display_wrdata_begin() и 	display_wrdata_end();
static void lph88_put_char_fast(char cc)
{
	uint_fast8_t i = 0;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_smallfont [0] / sizeof ls020_smallfont [0][0]) };
	const FLASHMEM uint8_t * const p = & ls020_smallfont [c][0];

	for (; i < NBYTES; ++ i)
	{
		lph88_pix8(p [i]);	// Выдать восемь цветных пикселей
	}
}


// Вызов этой функции только внутри display_wrdatabig_begin() и 	display_wrdatabig_end();

static void lph88_put_char_big(char cc)
{
	// '#' - узкий пробел
	enum { NBV = (BIGCHARH / 8) }; // сколько байтов в одной вертикали
	uint_fast8_t i = NBV * ((cc == '.' || cc == '#') ? 12 : 0);	// начальная колонка знакогенератора, откуда начинать.
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_bigfont [0] / sizeof ls020_bigfont [0][0]) };
	const FLASHMEM uint8_t * const p  = & ls020_bigfont [c][0];

	for (; i < NBYTES; ++ i)
	{
		lph88_pix8(p [i]);	// Выдать восемь цветных пикселей
	}
}

// Вызов этой функции только внутри display_wrdata_begin() и 	display_wrdata_end();

static void lph88_put_char_half(char cc)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_halffont [0] / sizeof ls020_halffont [0][0]) };
	const FLASHMEM uint8_t * const p = & ls020_halffont [c][0];

	for (; i < NBYTES; ++ i)
	{
		lph88_pix8(p [i]);	// Выдать восемь цветных пикселей
	}
}

/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
static void lph88_bar_column(uint_fast8_t v)
{
	lph88_pix8(v);	// Выдать восемь цветных пикселей
}

static uint_fast8_t lph88_windowy;
static uint_fast8_t lph88_windowx;


/*
 * установить высоту окна перед выводом текста
 */
static void lph88_set_windowh(uint_fast8_t height)
{
#if	LCDMODE_LPH88_TOPDOWN 
	uint_fast8_t y = lph88_windowy;
	uint_fast8_t x = lph88_windowx;
	// "коридор" до правого края дисплея
	lph88_comtype(0x17);		// Vertical RAM Address Position (R17h)
	lph88_comdat(DIM_X - 1, x);	// vea7..vea0, vsa7..vsa0

	lph88_comtype(0x16);		// Horizontal RAM Address Position (R16h)
	lph88_comdat(y, y - (height - 1));	// hea7..hea0, hsa7..hsa0


	lph88_comtype(0x21);		// RAM Address Set (R21h)
	lph88_comdat(x, y);		// ad15..ad8, ad7..ad0
#else /* LCDMODE_LPH88_TOPDOWN */
	uint_fast8_t y = lph88_windowy;
	uint_fast8_t x = lph88_windowx;

	// "коридор" до правого края дисплея
	lph88_comtype(0x17);		// Vertical RAM Address Position (R17h)
	lph88_comdat(x, 0);	// vea7..vea0, vsa7..vsa0

	lph88_comtype(0x16);		// Horizontal RAM Address Position (R16h)
	lph88_comdat(y + height - 1, y);	// hea7..hea0, hsa7..hsa0


	lph88_comtype(0x21);		// RAM Address Set (R21h)
	lph88_comdat(x, y /* + height - 1 */);		// ad15..ad8, ad7..ad0
#endif /* LCDMODE_LPH88_TOPDOWN */
}

static void lph88_clear(COLOR_T bg)
{
	uint_fast16_t i;
	const uint_fast16_t n = ((unsigned long) DIM_Y * DIM_X / 8 / 4);

	display_gotoxy(0, 0);
	lph88_set_windowh(DIM_Y);
	lph88_setcolor(COLOR_WHITE, bg);

	lph88_put_char_begin();
	for (i = 0; i < n; i++)	// DIM_Y = 132
	{
		lph88_pix8(0x00);
		lph88_pix8(0x00);
		lph88_pix8(0x00);
		lph88_pix8(0x00);
	}
	lph88_put_char_end();
}

/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{
	local_delay_ms(50);

	lph88_cmd(0x07, 0x0000); //display off
	local_delay_ms(10);

	//power on sequence
	lph88_cmd(0x02, 0x0400); //lcd drive control
	lph88_cmd(0x0C, 0x0001); //power control 3: VC        //step 1
	lph88_cmd(0x0D, 0x0006); //power control 4: VRH
	lph88_cmd(0x04, 0x0000); //power control 2: CAD
	lph88_cmd(0x0D, 0x0616); //power control 4: VRL
	lph88_cmd(0x0E, 0x0010); //power control 5: VCM
	lph88_cmd(0x0E, 0x1010); //power control 5: VDV
	lph88_cmd(0x03, 0x0000); //power control 1: BT        //step 2
	lph88_cmd(0x03, 0x0000); //power control 1: DC
	lph88_cmd(0x03, 0x000C); //power control 1: AP
	local_delay_ms(40);
	lph88_cmd(0x0E, 0x2D1F); //power control 5: VCOMG     //step 3
	local_delay_ms(40);
	lph88_cmd(0x0D, 0x0616); //power control 4: PON       //step 4
	local_delay_ms(100);

	//display options
#if	LCDMODE_LPH88_TOPDOWN
	lph88_cmd(0x05, 0x0020);		// I/D1:0 = 11b, am=0 was: 0x0030
#else /* LCDMODE_LPH88_TOPDOWN */
	lph88_cmd(0x05, 0x0010);		// I/D1:0 = 00b, am=0 was: 0x0000
#endif /* LCDMODE_LPH88_TOPDOWN */
	//s65_setArea(0, 0, (S65_WIDTH-1), (S65_HEIGHT-1));
	lph88_cmd(0x01, 0x0215);		// set gate scan direction and panel size

	//display on sequence (bit2 = reversed colors)
	lph88_cmd(0x07, 0x0005); //display control: D0
	lph88_cmd(0x07, 0x0025); //display control: GON
	lph88_cmd(0x07, 0x0027); //display control: D1
	lph88_cmd(0x07, 0x0037); //display control: DTE
	local_delay_ms(10);
}

void display_set_contrast(uint_fast8_t v)
{
}

void 
display_clear(void)
{
	const COLOR_T bg = display_getbgcolor();

	lph88_clear(bg);
}

void
//NOINLINEAT
display_setcolors(COLOR_T fg, COLOR_T bg)
{
	lph88_setcolor(fg, bg);
}

void display_setcolors3(COLOR_T fg, COLOR_T bg, COLOR_T fgbg)
{
	display_setcolors(fg, bg);
}

void
display_wrdata_begin(void)
{
	lph88_set_windowh(SMALLCHARH);
	lph88_put_char_begin();
}

void
display_wrdata_end(void)
{
	lph88_put_char_end();
}



void
display_wrdatabig_begin(void)
{
	lph88_set_windowh(BIGCHARH);	// same as HALFCHARH
	lph88_put_char_begin();
}


void
display_wrdatabig_end(void)
{
	lph88_put_char_end();
}

/* отображение одной вертикальной полосы на графическом индикаторе */
/* старшие биты соответствуют верхним пикселям изображения */
/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */


void
display_wrdatabar_begin(void)
{
	lph88_set_windowh(CHAR_H);
	lph88_put_char_begin();
}

void 
display_barcolumn(uint_fast8_t pattern)
{
	lph88_bar_column(pattern);
}

void
display_wrdatabar_end(void)
{
	lph88_put_char_end();
}

void
display_put_char_big(uint_fast8_t c, uint_fast8_t lowhalf)
{
	lph88_put_char_big(c);
}

void
display_put_char_half(uint_fast8_t c, uint_fast8_t lowhalf)
{
	lph88_put_char_half(c);
}


// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
void
display_put_char_small(uint_fast8_t c, uint_fast8_t lowhalf)
{
	lph88_put_char_fast(c);
	//lph88_put_char_fasto(c, lowhalf);
}

	//uint8_t x = h * CHAR_W;
	//uint8_t y = DIM_Y - 5 - (v * CHAR_H);

void
display_gotoxy(uint_fast8_t x, uint_fast8_t y)
{
#if	LCDMODE_LPH88_TOPDOWN
	// нормально смотрим - выводы дисплея справа
	lph88_windowy = (DIM_Y - 1) - CHAR_H * y;
	lph88_windowx = (x * CHAR_W);
#else /* LCDMODE_LPH88_TOPDOWN */
	// нормально смотрим - выводы дисплея слева
	lph88_windowy = (y * CHAR_H);
	lph88_windowx = (DIM_X - 1 - x * CHAR_W);
#endif /* LCDMODE_LPH88_TOPDOWN */
}

// Координаты в пикселях
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
#if	LCDMODE_LPH88_TOPDOWN
	// нормально смотрим - выводы дисплея справа
	lph88_windowy = (DIM_Y - 1) - y;
	lph88_windowx = (x);
#else /* LCDMODE_LPH88_TOPDOWN */
	// нормально смотрим - выводы дисплея слева
	lph88_windowy = (y);
	lph88_windowx = (DIM_X - 1 - x);
#endif /* LCDMODE_LPH88_TOPDOWN */
}

void display_plotstart(
	uint_fast16_t height	// Высота окна в пикселях
	)
{
	lph88_set_windowh(height);
	lph88_put_char_begin();
}

void display_plotstop(void)
{
	lph88_put_char_end();
}

void display_plot(
	const PACKEDCOLOR_T * buffer, 
	uint_fast16_t dx,	// Размеры окна в пикселях
	uint_fast16_t dy
	)
{

}


/* аппаратный сброс дисплея - перед инициализаций */
/* вызывается при разрешённых прерываниях. */
void
display_reset(void)
{
	lph88_reset();
}


/* Разряжаем конденсаторы питания */
void display_discharge(void)
{
}

uint_fast8_t
display_getreadystate(void)
{
	return 1;
}

#endif /* LCDMODE_LPH88 */
