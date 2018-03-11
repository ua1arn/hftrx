/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// TFT панель 160 * 128 HY-1.8-SPI
// Кроме того, этот файл для обслуживания LCDMODE_ILI9163
/* Индикатор 160*128 с контроллером Sitronix ST7735 */
/* Индикатор 320*240 с контроллером ILITEK ILI9341 */

// Original copyright:
/***************************************************
  This is a library for the Adafruit 1.8" SPI display.
  This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
  as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618
 
  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#include "hardware.h"
#include "board.h"
#include "display.h"
#include <stdint.h>

#include "spifuncs.h"

#include "formats.h"	// for debug prints

#if LCDMODE_ST7735 || LCDMODE_ILI9163 || LCDMODE_ILI9341

//#define WITHFONTSMOOTHING 1
// LCDMODE_ILI9341 - 320 * 240
// LCDMODE_ST7735 - 160 * 128
// LCDMODE_ILI9163 - 176 * 132

#include "st7735.h"

////#include "./byte2crun.h"
////static const FLASHMEM PACKEDCOLOR_T (* byte2run) [256][8] = & byte2run_COLOR_WHITE_COLOR_BLACK;

#if DSTYLE_G_X320_Y240
	// LCDMODE_ILI9341

	// Для дисплеев 320 * 240
	#include "./fonts/ILI9341_font_small.c"
	#include "./fonts/ILI9341_font_half.c"
	#include "./fonts/ILI9341_font_big.c"

	#define	ls020_smallfont	ILI9341_smallfont
	#define	ls020_halffont	ILI9341_halffont
	#define	ls020_bigfont	ILI9341_bigfont


#else /* DSTYLE_G_X320_Y240 */
	// LCDMODE_ST7735 - 160 * 128
	// LCDMODE_ILI9163 - 176 * 132

	#include "./fonts/ls020_font_small.c"
	#include "./fonts/ls020_font_half.c"
	#include "./fonts/ls020_font_big.c"

#endif /* DSTYLE_G_X320_Y240 */


#define ST7735_SPIMODE		SPIC_MODE3
//#define ST7735_SPISPEED		SPIC_SPEED10M	// в описнии контроллера ILI9341 минимальный период указан 100 nS
//#define ST7735_SPISPEED		SPIC_SPEED10M	// в описнии контроллера ST7735 минимальный период указан 150 nS
#define ST7735_SPISPEED		SPIC_SPEED25M

// Условие использования оптимизированных функций обращения к SPI
#define WITHSPIEXT16 (WITHSPIHW && WITHSPI16BIT)

#define ST7735_CMND() do { board_lcd_rs(0); } while (0)	/* RS: Low: command data */
#define ST7735_DATA() do { board_lcd_rs(1); } while (0)	/* RS: High: display data or parameter */

static uint_fast8_t colstart, rowstart, tabcolor; // May be overridden in init func


// в режим передачи данных переводим сразу по окончании команд.
static void st7735_put_char_begin(void)
{
#if WITHSPIEXT16
	
	hardware_spi_connect_b16(ST7735_SPISPEED, ST7735_SPIMODE);	/* Enable SPI */
	prog_select(targetlcd);	/* start sending data to target chip */
	ST7735_CMND();	/* RS: Low: select an index or status register */
	hardware_spi_b16_p1(ST7735_RAMWR);	// старшая половина - 0 - 'NOP' command
	hardware_spi_complete_b16();
	ST7735_DATA();	/* RS: High: select a control register */

#else /* WITHSPIEXT16 */

	spi_select2(targetlcd, ST7735_SPIMODE, ST7735_SPISPEED);	/* Enable SPI */
	ST7735_CMND();	/* RS: Low: select an index or status register */
  	spi_progval8_p1(targetlcd, ST7735_RAMWR); // write to RAM
	spi_complete(targetlcd);
	ST7735_DATA();	/* RS: High: select a control register */

#endif /* WITHSPIEXT16 */
}

static void st7735_put_char_end(void)
{
#if WITHSPIEXT16

	prog_unselect(targetlcd);			/* Disable SPI */
  #if WITHSPISW
	hardware_spi_disconnect();
  #endif /* WITHSPISW */
#else /* WITHSPIEXT16 */

	spi_unselect(targetlcd);		/* Disable SPI */

#endif /* WITHSPIEXT16 */
}


#if WITHSPIEXT16
	static COLOR_T fgcolor, bkcolor, halfcolor;
#else /* WITHSPIEXT16 */
	static struct { uint_fast8_t first, second; } fgcolor, bkcolor, halfcolor;
#endif

static void st7735_setcolor(COLOR_T fg, COLOR_T bg, COLOR_T transient)
{
#if WITHSPIEXT16
	fgcolor = fg;
	bkcolor = bg;
	halfcolor = transient;
#else /* WITHSPIEXT16 */
	fgcolor.first = fg >> 8;
	fgcolor.second = fg >> 0;
	bkcolor.first = bg >> 8;
	bkcolor.second = bg >> 0;
	halfcolor.first = transient >> 8;
	halfcolor.second = transient >> 0;
#endif /* WITHSPIEXT16 */

#if 0

	if (fg == COLOR_YELLOW && bg == COLOR_BLACK)
		byte2run = & byte2run_COLOR_YELLOW_COLOR_BLACK;
	else if (fg == COLOR_BLACK && bg == COLOR_RED)
		byte2run = & byte2run_COLOR_BLACK_COLOR_RED;
	else if (fg == COLOR_BLACK && bg == COLOR_GREEN)
		byte2run = & byte2run_COLOR_BLACK_COLOR_GREEN;
	else if (fg == COLOR_RED && bg == COLOR_BLACK)
		byte2run = & byte2run_COLOR_RED_COLOR_BLACK;
	else if (fg == COLOR_GREEN && bg == COLOR_BLACK)
		byte2run = & byte2run_COLOR_GREEN_COLOR_BLACK;
	else if (fg == COLOR_WHITE && bg == COLOR_DARKGREEN)
		byte2run = & byte2run_COLOR_WHITE_COLOR_DARKGREEN;
	else
		byte2run = & byte2run_COLOR_WHITE_COLOR_BLACK;

#endif
}

static void 
//NOINLINEAT
st7735_pixel_p1(
	uint_fast8_t fg
	)
{
#if WITHSPIEXT16

	hardware_spi_b16_p1(fg ? fgcolor : bkcolor);

#else /* WITHSPIEXT16 */

	if (fg)
	{
		spi_progval8_p1(targetlcd, fgcolor.first);	
		spi_progval8_p2(targetlcd, fgcolor.second);	
	}
	else
	{
		spi_progval8_p1(targetlcd, bkcolor.first);	
		spi_progval8_p2(targetlcd, bkcolor.second);	
	}

#endif /* WITHSPIEXT16 */
}

static void 
//NOINLINEAT
st7735_pixel_p2(
	uint_fast8_t fg
	)
{
#if WITHSPIEXT16

	hardware_spi_b16_p2(fg ? fgcolor : bkcolor);

#else /* WITHSPIEXT16 */

	if (fg)
	{
		spi_progval8_p2(targetlcd, fgcolor.first);	
		spi_progval8_p2(targetlcd, fgcolor.second);	
	}
	else
	{
		spi_progval8_p2(targetlcd, bkcolor.first);	
		spi_progval8_p2(targetlcd, bkcolor.second);	
	}

#endif /* WITHSPIEXT16 */
}

static void 
//NOINLINEAT
st7735_pixel_p3(
	uint_fast8_t fg
	)
{
#if WITHSPIEXT16

	hardware_spi_b16_p2(fg ? fgcolor : bkcolor);
	hardware_spi_complete_b16();

#else /* WITHSPIEXT16 */

	if (fg)
	{
		spi_progval8_p2(targetlcd, fgcolor.first);	
		spi_progval8_p2(targetlcd, fgcolor.second);	
		spi_complete(targetlcd);
	}
	else
	{
		spi_progval8_p2(targetlcd, bkcolor.first);	
		spi_progval8_p2(targetlcd, bkcolor.second);	
		spi_complete(targetlcd);
	}

#endif /* WITHSPIEXT16 */
}

static void 
//NOINLINEAT
st7735_colorpixel_p1(
	COLOR_T color
	)
{
#if WITHSPIEXT16

	hardware_spi_b16_p1(color);

#else /* WITHSPIEXT16 */

	spi_progval8_p1(targetlcd, color >> 8);	
	spi_progval8_p2(targetlcd, color >> 0);	

#endif /* WITHSPIEXT16 */
}

static void 
//NOINLINEAT
st7735_colorpixel_p2(
	COLOR_T color
	)
{
#if WITHSPIEXT16

	hardware_spi_b16_p2(color);

#else /* WITHSPIEXT16 */

	spi_progval8_p2(targetlcd, color >> 8);	
	spi_progval8_p2(targetlcd, color >> 0);	

#endif /* WITHSPIEXT16 */
}

static void 
//NOINLINEAT
st7735_colorpixel_p3(
	COLOR_T color
	)
{
#if WITHSPIEXT16

	hardware_spi_b16_p2(color);
	hardware_spi_complete_b16();

#else /* WITHSPIEXT16 */

	spi_progval8_p2(targetlcd, color >> 8);	
	spi_progval8_p2(targetlcd, color >> 0);	
	spi_complete(targetlcd);

#endif /* WITHSPIEXT16 */
}

static uint_fast8_t 
//NOINLINEAT
st7735_pixelsmooth_p1(
	uint_fast8_t fg,
	uint_fast8_t prev
	)
{
#if WITHSPIEXT16

	if ((fg != 0) != (prev != 0))
		hardware_spi_b16_p1(halfcolor);
	else if (fg)
		hardware_spi_b16_p1(fgcolor);
	else
		hardware_spi_b16_p1(bkcolor);

#else /* WITHSPIEXT16 */

	if ((fg != 0) != (prev != 0))
	{
		spi_progval8_p1(targetlcd, halfcolor.first);	
		spi_progval8_p1(targetlcd, halfcolor.second);	
	}
	else if (fg)
	{
		spi_progval8_p1(targetlcd, fgcolor.first);	
		spi_progval8_p1(targetlcd, fgcolor.second);	
	}
	else
	{
		spi_progval8_p1(targetlcd, bkcolor.first);	
		spi_progval8_p1(targetlcd, bkcolor.second);	
	}

#endif /* WITHSPIEXT16 */
	return fg;
}

static uint_fast8_t 
//NOINLINEAT
st7735_pixelsmooth_p2(
	uint_fast8_t fg,
	uint_fast8_t prev
	)
{
#if WITHSPIEXT16

	if ((fg != 0) != (prev != 0))
		hardware_spi_b16_p2(halfcolor);
	else if (fg)
		hardware_spi_b16_p2(fgcolor);
	else
		hardware_spi_b16_p2(bkcolor);

#else /* WITHSPIEXT16 */

	if ((fg != 0) != (prev != 0))
	{
		spi_progval8_p2(targetlcd, halfcolor.first);	
		spi_progval8_p2(targetlcd, halfcolor.second);	
	}
	else if (fg)
	{
		spi_progval8_p2(targetlcd, fgcolor.first);	
		spi_progval8_p2(targetlcd, fgcolor.second);	
	}
	else
	{
		spi_progval8_p2(targetlcd, bkcolor.first);	
		spi_progval8_p2(targetlcd, bkcolor.second);	
	}

#endif /* WITHSPIEXT16 */
	return fg;
}

static uint_fast8_t 
//NOINLINEAT
st7735_pixelsmooth_p3(
	uint_fast8_t fg,
	uint_fast8_t prev
	)
{
#if WITHSPIEXT16

	if ((fg != 0) != (prev != 0))
		hardware_spi_b16_p2(halfcolor);
	else if (fg)
		hardware_spi_b16_p2(fgcolor);
	else
		hardware_spi_b16_p2(bkcolor);
	hardware_spi_complete_b16();

#else /* WITHSPIEXT16 */

	if ((fg != 0) != (prev != 0))
	{
		spi_progval8_p2(targetlcd, halfcolor.first);	
		spi_progval8_p2(targetlcd, halfcolor.second);	
	}
	else if (fg)
	{
		spi_progval8_p2(targetlcd, fgcolor.first);	
		spi_progval8_p2(targetlcd, fgcolor.second);	
	}
	else
	{
		spi_progval8_p2(targetlcd, bkcolor.first);	
		spi_progval8_p2(targetlcd, bkcolor.second);	
	}
	spi_complete(targetlcd);

#endif /* WITHSPIEXT16 */
	return fg;
}

// Выдать восемь цветных пикселей
static void 
//NOINLINEAT
st7735_pix8(
	uint_fast8_t v
	)
{
	st7735_pixel_p1(v & 0x01);
	st7735_pixel_p2(v & 0x02);
	st7735_pixel_p2(v & 0x04);
	st7735_pixel_p2(v & 0x08);
	st7735_pixel_p2(v & 0x10);
	st7735_pixel_p2(v & 0x20);
	st7735_pixel_p2(v & 0x40);
	st7735_pixel_p3(v & 0x80);
}
// Выдать восемь цветных пикселей
static uint_fast8_t 
//NOINLINEAT
st7735_pix8smooth(
	uint_fast8_t v,
	uint_fast8_t prev
	)
{
	prev = st7735_pixelsmooth_p1(v & 0x01, prev);
	prev = st7735_pixelsmooth_p2(v & 0x02, prev);
	prev = st7735_pixelsmooth_p2(v & 0x04, prev);
	prev = st7735_pixelsmooth_p2(v & 0x08, prev);
	prev = st7735_pixelsmooth_p2(v & 0x10, prev);
	prev = st7735_pixelsmooth_p2(v & 0x20, prev);
	prev = st7735_pixelsmooth_p2(v & 0x40, prev);
	prev = st7735_pixelsmooth_p3(v & 0x80, prev);

	return prev;
}

/*
static void 
ZZZst7735_pix8(
	uint_fast8_t v
	)
{
	prog_spi_send_frame(targetlcd, (* byte2run) [v], sizeof (* byte2run) [v]);
}
*/

static uint_fast8_t
//NOINLINEAT
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
ascii_decode(uint_fast8_t c)
{
	return c - ' ';
}

// Вызов этой функции только внутри display_wrdata_begin() и 	display_wrdata_end();
static void st7735_put_char_small(char cc)
{
	uint_fast8_t i = 0;
	const uint_fast8_t c = ascii_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_smallfont [0] / sizeof ls020_smallfont [0][0]) };
	const FLASHMEM uint8_t * p = & ls020_smallfont [c][0];
	
	for (; i < NBYTES; ++ i)
		st7735_pix8(p [i]);	// Выдать восемь цветных пикселей
}


// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
static void st7735_put_char_big(char cc)
{
	// '#' - узкий пробел
	enum { NBV = (BIGCHARH / 8) }; // сколько байтов в одной вертикали
	uint_fast8_t i = NBV * ((cc == '.' || cc == '#') ? 12 : 0);	// начальная колонка знакогенератора, откуда начинать.
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_bigfont [0] / sizeof ls020_bigfont [0][0]) };
	const FLASHMEM uint8_t * p = & ls020_bigfont [c][0];
	
#if WITHFONTSMOOTHING
	uint_fast8_t col;
	for (col = i / NBV; col < NBYTES / NBV; ++ col)
	{
		uint_fast8_t last = 0;	// Фон
		uint_fast8_t row;
		for (row = 0; row < NBV; ++ row)
			last = st7735_pix8smooth(p [i ++], last);

	}
#else /* WITHFONTSMOOTHING */
	for (; i < NBYTES; ++ i)
		st7735_pix8(p [i]);	// Выдать восемь цветных пикселей
#endif /* WITHFONTSMOOTHING */
}


// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
static void st7735_put_char_half(char cc)
{
	enum { NBV = (BIGCHARH / 8) }; // сколько байтов в одной вертикали
	uint_fast8_t i = 0;
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_halffont [0] / sizeof ls020_halffont [0][0]) };
	const FLASHMEM uint8_t * p = & ls020_halffont [c][0];
	
#if WITHFONTSMOOTHING
	uint_fast8_t col;
	for (col = 0; col < NBYTES / NBV; ++ col)
	{
		uint_fast8_t last = 0;	// Фон
		uint_fast8_t row;
		for (row = 0; row < NBV; ++ row)
			last = st7735_pix8smooth(p [i ++], last);

	}
#else /* WITHFONTSMOOTHING */
	for (; i < NBYTES; ++ i)
		st7735_pix8(p [i]);	// Выдать восемь цветных пикселей
#endif /* WITHFONTSMOOTHING */
}

static uint_fast8_t st7735_y;	/* в пикселях */

// открыть для записи полосу высотой с символ и шириной до правого края
static void
st7735_set_strype(uint_fast8_t height)
{
	const uint_fast8_t xs = st7735_y + colstart;
	const uint_fast8_t xe = st7735_y + height - 1 + colstart;

#if WITHSPIEXT16

	hardware_spi_connect_b16(ST7735_SPISPEED, ST7735_SPIMODE);	/* Enable SPI */
	prog_select(targetlcd);	/* start sending data to target chip */
	ST7735_CMND();	/* RS: Low: select an index or status register */
	hardware_spi_b16_p1(ST7735_CASET);	// старшая половина - 0 - 'NOP' command
	hardware_spi_complete_b16();
	ST7735_DATA();	/* RS: High: select a control register */
	#if LCDMODE_ILI9163
		hardware_spi_b16_p1(xs * 256 + xe);
	#else
		hardware_spi_b16_p1(xs);     // XSTART
		hardware_spi_b16_p2(xe);     // XEND
	#endif
	hardware_spi_complete_b16();
	prog_unselect(targetlcd);
  #if WITHSPISW
	hardware_spi_disconnect();
  #endif /* WITHSPISW */

#else /* WITHSPIEXT16 */

	spi_select2(targetlcd, ST7735_SPIMODE, ST7735_SPISPEED);	/* Enable SPI */
	ST7735_CMND();	/* RS: Low: select an index or status register */
	spi_progval8_p1(targetlcd, ST7735_CASET); // Column addr set (0..127)
	spi_complete(targetlcd);
	ST7735_DATA();	/* RS: High: select a control register */
	#if LCDMODE_ILI9163
		spi_progval8_p1(targetlcd, xs);     // XSTART
		spi_progval8_p2(targetlcd, xe);     // XEND
	#else
		spi_progval8_p1(targetlcd, 0x00);				// xs15:xs8
		spi_progval8_p2(targetlcd, xs);     // XSTART
		spi_progval8_p2(targetlcd, 0x00);				// xe7:xe0
		spi_progval8_p2(targetlcd, xe);     // XEND
	#endif
	spi_complete(targetlcd);
	spi_unselect(targetlcd);	/* Disable SPI */

#endif /* WITHSPIEXT16 */
}
// Тип для хранения горизонтальной координаты
#if DIM_X > 254
	typedef uint_fast16_t xholder_t;
#else
	typedef uint_fast8_t xholder_t;
#endif

/*
 Функция установки курсора в позицию x,y
 X - координата по горизонтали в пределах 0-175 (0-319),
 Y - координата по вертикали (строка) в пределах 8 бит
*/
static void 
//NOINLINEAT
st7735_set_addr_column(
	xholder_t x
	)
{
	const xholder_t ys = x + rowstart;
	const xholder_t ye = DIM_X - 1 + rowstart;

#if WITHSPIEXT16

	/* Enable SPI */
	hardware_spi_connect_b16(ST7735_SPISPEED, ST7735_SPIMODE);
	prog_select(targetlcd);	/* start sending data to target chip */
	ST7735_CMND();	/* RS: Low: select an index or status register */
	hardware_spi_b16_p1(ST7735_RASET);	// старшая половина - 0 - 'NOP' command
	hardware_spi_complete_b16();
	ST7735_DATA();	/* RS: High: select a control register */
	#if LCDMODE_ILI9163
		hardware_spi_b16_p1(ys * 256 + ye);
	#else
		hardware_spi_b16_p1(ys);     // ye15:ys0 YSTART
		hardware_spi_b16_p2(ye);     // ye15:ye0 YEND
	#endif
	hardware_spi_complete_b16();
	prog_unselect(targetlcd);
  #if WITHSPISW
	hardware_spi_disconnect();
  #endif /* WITHSPISW */

#else /* WITHSPIEXT16 */

	spi_select2(targetlcd, ST7735_SPIMODE, ST7735_SPISPEED);		/* Enable SPI */
	ST7735_CMND();	/* RS: Low: select an index or status register */
	spi_progval8_p1(targetlcd, ST7735_RASET); // Row addr set (1..160)
	spi_complete(targetlcd);
	ST7735_DATA();	/* RS: High: select a control register */
	#if LCDMODE_ILI9163
		spi_progval8_p1(targetlcd, ys);     // ys7:ys0 YSTART
		spi_progval8_p2(targetlcd, ye);     // ye7:ye0 YEND
	#else
		spi_progval8_p1(targetlcd, ys >> 8);			// ys15:ys8
		spi_progval8_p2(targetlcd, ys);     // ys7:ys0 YSTART
		spi_progval8_p2(targetlcd, ye >> 8);				//ye15:ye8
		spi_progval8_p2(targetlcd, ye);     // ye7:ye0 YEND
	#endif
	spi_complete(targetlcd);
	spi_unselect(targetlcd);		/* Disable SPI */

#endif /* WITHSPIEXT16 */
}

static void st7735_clear(COLOR_T bg)
{
	unsigned long i;
	
	display_gotoxy(0, 0);
	st7735_set_strype(DIM_Y);	// установить окно высотой в весь экран

	st7735_setcolor(COLOR_WHITE, bg, bg);
	st7735_put_char_begin();
	for (i = 0; i < ((unsigned long) DIM_Y * DIM_X) / 8; ++ i)
	{
		st7735_pix8(0x00);	// Выдать восемь пикселей цвета фона
	}
	st7735_put_char_end();

}

// MY = Mirror Y-axis (Row address direction parameter), D7 parameter of MADCTL command
// MX = Mirror X-axis (Column address direction parameter), D6 parameter of MADCTL command
// ML = Scan direction parameter, D4 parameter of MADCTL command
// RGB = Red, Green and Blue pixel position change, D3 parameter of MADCTL command
// In vertical addressing mode (MV=1), the Y-address increments after each byte, after the last Y-address (Y=YE),
// MH Horizontal Refresh ORDER
// d7 d6 d5 d4 d3 d2 d1 d0
// MY MX MV ML RGB MH 00 00

#define useSTinit 1

#if LCDMODE_ST7735
	#if LCDMODE_ST7735_TOPDOWN
		#define MADCTL_VALUE (0x80)
	#else
		#define MADCTL_VALUE (0x40)
	#endif
#elif LCDMODE_ILI9341 || LCDMODE_ILI9163
	#if LCDMODE_ILI9341_TOPDOWN
		#define MADCTL_VALUE (0x48)
	#else
		#define MADCTL_VALUE (0x88)
	#endif
#else
	#error Wrong LCDMODE_XXX definition
#endif

// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.
#define DELAY_FLAG 0x80

#if LCDMODE_ST7735 || LCDMODE_ILI9163

static const FLASHMEM uint8_t
Bcmd [] = {                  // Initialization commands for 7735B screens
    14, 			     	      // 14 commands in list:
    ST7735_SWRESET, 0 | DELAY_FLAG,  //  1: Software reset, no args, w/local_delay_ms
      150,                     //     50 ms local_delay_ms
    ST7735_SLPOUT , 0 | DELAY_FLAG,  //  2: Out of sleep mode, no args, w/local_delay_ms
      150,                    //     255 = 500 ms local_delay_ms
    ST7735_COLMOD , 1 | DELAY_FLAG,  //  3: Set color mode, 1 arg + local_delay_ms:
      0x05,                   //     16-bit color
      10,                     //     10 ms local_delay_ms
    ST7735_FRMCTR1, 3 | DELAY_FLAG,  //  4: Frame rate control, 3 args + local_delay_ms:
      0x00,                   //     fastest refresh
      0x06,                   //     6 lines front porch
      0x03,                   //     3 lines back porch
      10,                     //     10 ms local_delay_ms
    ST7735_MADCTL , 1      ,  //  5: Memory access ctrl (directions), 1 arg:
      MADCTL_VALUE,	// was: 0x08,                   //     Row addr/col addr, bottom to top refresh
    ST7735_DISSET5, 2      ,  //  6: Display settings #5, 2 args, no local_delay_ms:
      0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate
                              //     rise, 3 cycle osc equalize
      0x02,                   //     Fix on VTL
    ST7735_INVCTR , 1      ,  //  7: Display inversion control, 1 arg:
      0x0,                    //     Line inversion
    ST7735_PWCTR1 , 2 | DELAY_FLAG,  //  8: Power control, 2 args + local_delay_ms:
      0x02,                   //     GVDD = 4.7V
      0x70,                   //     1.0uA
      10,                     //     10 ms local_delay_ms
    ST7735_PWCTR2 , 1      ,  //  9: Power control, 1 arg, no local_delay_ms:
      0x05,                   //     VGH = 14.7V, VGL = -7.35V
    ST7735_PWCTR3 , 2      ,  // 10: Power control, 2 args, no local_delay_ms:
      0x01,                   //     Opamp current small
      0x02,                   //     Boost frequency
    ST7735_VMCTR1 , 2 | DELAY_FLAG,  // 10: Power control, 2 args + local_delay_ms:
      0x3C,                   //     VCOMH = 4V
      0x38,                   //     VCOML = -1.1V
      10,                     //     10 ms local_delay_ms
    ST7735_PWCTR6 , 2      ,  // 11: Power control, 2 args, no local_delay_ms:
      0x11, 0x15,
    ST7735_GMCTRP1, 16      ,  // 12: Magical unicorn dust, 16 args, no local_delay_ms:
      0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
      0x21, 0x1B, 0x13, 0x19, //      these config values represent)
      0x17, 0x15, 0x1E, 0x2B,
      0x04, 0x05, 0x02, 0x0E,
    ST7735_GMCTRN1, 16 | DELAY_FLAG,  // 13: Sparkles and rainbows, 16 args + local_delay_ms:
      0x0B, 0x14, 0x08, 0x1E, //     (ditto)
      0x22, 0x1D, 0x18, 0x1E,
      0x1B, 0x1A, 0x24, 0x2B,
      0x06, 0x06, 0x02, 0x0F,
      10,                     //     10 ms local_delay_ms
	ST7735_DISPON, 0,			// 14: Main screen turn on, no args, w/local_delay_ms
 };

static const FLASHMEM uint8_t
Rcmd1 [] = {                 // Init for 7735R, part 1 (red or green tab)
    13,                       // 13 commands in list:
    ST7735_SWRESET,   DELAY_FLAG,  //  1: Software reset, 0 args, w/local_delay_ms
      150,                    //     150 ms local_delay_ms
    ST7735_SLPOUT,   DELAY_FLAG,  //  2: Out of sleep mode, 0 args, w/local_delay_ms
      150,                    //     500 ms local_delay_ms
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_PWCTR1 , 3      ,  //  6: Power control, 3 args, no local_delay_ms:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  7: Power control, 1 arg, no local_delay_ms:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  8: Power control, 2 args, no local_delay_ms:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 9: Power control, 2 args, no local_delay_ms:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735_PWCTR5 , 2      ,  // 10: Power control, 2 args, no local_delay_ms:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 11: Power control, 1 arg, no local_delay_ms:
      0x0E,
    ST7735_MADCTL , 1      ,  // 12: Memory access control (directions), 1 arg:
      MADCTL_VALUE, // was: 0xC8,                   //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 13: set color mode, 1 arg, no local_delay_ms:
      0x05,                 //     16-bit color
};            

static const FLASHMEM uint8_t
Rcmd3 [] = {                 // Init for 7735R, part 3 (red or green tab)
    3,                         //  3 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no local_delay_ms:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no local_delay_ms:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
	ST7735_DISPON, 0 | DELAY_FLAG,  				// 5: Main screen turn on, no args, w/local_delay_ms
		150,
	},

	ili9163_initcmds [] = {
    5,                      		 // 5 commands in list:
    ST7735_SWRESET,   DELAY_FLAG,  	//  1: Software reset, 0 args, w/local_delay_ms
      150,                    		//     150 ms local_delay_ms
    ST7735_SLPOUT,   DELAY_FLAG,  	//  2: Out of sleep mode, 0 args, w/local_delay_ms
      150,                    		//     500 ms local_delay_ms
    ST7735_MADCTL, 1,  				// 3: Memory access control (directions), 1 arg:
      MADCTL_VALUE, 				//  row addr/col addr, bottom to top refresh
    ST7735_COLMOD, 1,  				// 4: set color mode, 1 arg, no local_delay_ms:
      0x05,                 		//     16-bit color
	ST7735_DISPON, 0 | DELAY_FLAG,  				// 5: Main screen turn on, no args, w/local_delay_ms
		150,
};

#endif /* LCDMODE_ST7735 || LCDMODE_ILI9163 */

///ILI9341_TM2.4  ????????9229?? 	V=2.8V  OK 2011-07-05  //
static const FLASHMEM uint8_t
ili9341_initcmds [] = {                  // Initialization commands for ilitek ili9341 screens
    25, 			     	      // 25 commands in list:
    LCD_SLEEP_OUT, 0 | DELAY_FLAG,  //  1: Sleep Out (11h)
      150,                     //     150 ms delay
    LCD_POWERB, 3,  //  2 undoc
		0x0000,
		0x00aa,
		0x00b0,
	0x00EF, 3,	// 3 undoc
		0x0003,//
		0x0080,//
		0x0002,//

	LCD_POWER_SEQ, 4,	// 4 undoc
		0x0067,//
		0x0003,//
		0x0012,//
		0x0081,//

	LCD_DTCA, 3,	// 5 undoc
		0x0083,//
		0x0011,//
		0x0078,//

	LCD_POWERA, 5,	// 6 undoc
		0x0039,//AP[2:0]
		0x002c,//
		0x0000,//
		0x0034,//
		0x0002,//

	LCD_DTCB, 2,	// 7 undoc
		0x0000,//
		0x0000,

	LCD_PRC, 1,	// 8 undoc
		0x0020,//

	0x00CA, 3,	// 9 undoc
		0x0083, 	
		0x0002,
		0x0000,

	LCD_DTCB, 2,	// 10 undoc
		0x0000,//
		0x0000,//

	LCD_POWER1, 1, //11: Set GVDD  Power Control 1
		0x0018, //VRH[5:0]	21

	LCD_POWER2, 1, //12: Set power step-up Power Control 2
		0x0011, //SAP[2:0];BT[3:0]   04                             

	LCD_VCOM1, 2, //13: set vcom VCOM Control 1(
		0x0024, //VCM control 0x34   36   
		0x003c, // 0x40              40

	LCD_MAC, 1,	// 14 Memory Access Control
		MADCTL_VALUE,// Memory Access Control // was: 0x0048

	LCD_FRC, 2,  // 15: Frame Rate Control (In Normal Mode/Full Colors)
		0x0000, //fosc as a source for divider
		0x001b,     //70Hz(default)

	0x00B2, 2,  // 16:Frame Rate Control (In Idle Mode/8 colors)
		0x0000, //fosc as a source for divider
		0x001b,     //70Hz(default)

	0x00B3, 2,  // 17: Frame Rate control (In Partial Mode/Full Colors)
		0x0000, //fosc as a source for divider
		0x001b,     //70Hz(default)

	LCD_DFC, 4,		// 18 Display Function Control (B6h)
		0x0A, //Display Function Control  0x000a   
		0xA2, //0x0082,
		0x27,	// 320 lines добавлено из переключения в RGB MODE - у команды вообще0то четыре параметра
		0x04,	// добавлено из переключения в RGB MODE - у команды вообще0то четыре параметра

	LCD_VCOM2, 1, //19:Set VCOMH/VCOML VCOM Control 2
		0xb8, //VCOM Control , VMF[6:0]   0x00BC b8

	LCD_GAMMA, 1, // 18: Gamma set
		0x01,
	LCD_3GAMMA_EN, 1,
		0x00, //19: 3Gamma Function Disable     
	LCD_PGAMMA, 15,	// #20 Positive Gamma Correction (E0h)
		0x1F,0x1B,0x19,0x0C,0x0D,0x07,0x44,0xA9,0x2F,0x08,0x0D,0x13,0x10,0x0B,0x00,
	LCD_NGAMMA, 15,		// #21 Negative Gamma Correction (E1h)
		0x00,0x24,0x26,0x03,0x12,0x08,0x3C,0x56,0x50,0x07,0x12,0x0C,0x2F,0x34,0x0F,
	LCD_PIXEL_FORMAT, 1,	// #22
		0x65,		// RGB color format = 18 bit/pixel, MCU color format = 16 bit/pixel
	LCD_DISPLAY_ON, 0, // #23: display on
};

// Initialization commands for ilitek ili9341 screens
static const FLASHMEM uint8_t
ili9341_initcmdsST [] = 
{
	21,			// Number of commands
	0xCA, 3,
		0xC3, 0x08, 0x50, 
	LCD_POWERB, 3,
		0x00, 0xC1, 0x30, 
	LCD_POWER_SEQ, 4,
		0x64, 0x03, 0x12, 0x81, 
	LCD_DTCA, 3,
		0x85, 0x00, 0x78, 
	LCD_POWERA, 5,
		0x39, 0x2C, 0x00, 0x34, 0x02, 
	LCD_PRC, 1,
		0x20, 
	LCD_DTCB, 2,
		0x00, 0x00, 
	LCD_FRC, 2,
		0x00, 0x1B, 
	LCD_DFC, 4,
		0x0A, 
		0xA2, 	// добавлено из переключения в RGB MODE - у команды вообще0то четыре параметра
		0x27, 	// 320 lines добавлено из переключения в RGB MODE - у команды вообще0то четыре параметра
		0x04, 
	LCD_POWER1, 1,
		0x10, 
	LCD_POWER2, 1,
		0x10, 
	LCD_VCOM1, 2,
		0x45, 0x15, 
	LCD_VCOM2, 1,
		0x90, 
	LCD_MAC, 1,
		MADCTL_VALUE, 	// was from ST: 0xC8

	LCD_3GAMMA_EN, 1, 
		0x00, 

	LCD_GAMMA, 1,
		0x01, 

	LCD_PGAMMA, 15,
		0x0F, 0x29, 0x24, 0x0C, 0x0E, 0x09, 0x4E, 0x78, 0x3C, 0x09, 0x13, 0x05, 0x17, 0x11, 0x00, 
	LCD_NGAMMA, 15,
		0x00, 0x16, 0x1B, 0x04,	0x11, 0x07, 0x31, 0x33, 0x42, 0x05, 0x0C, 0x0A, 0x28, 0x2F, 0x0F, 
	// added by ua1arn
	LCD_PIXEL_FORMAT, 1,
		0x0065,		// RGB color format = 18 bit/pixel, MCU color format = 16 bit/pixel
	LCD_SLEEP_OUT, 0 | DELAY_FLAG,
		150, 
	LCD_DISPLAY_ON, 0,
};
/* переход в RGB */
static const FLASHMEM uint8_t
ili9341_switchtorgb_cmds_ST [] = 
{
3,	// Number of commands
LCD_RGB_INTERFACE, 1,
	0xC2, 	//bypass, rcm=2
LCD_DFC, 4,
	0x0A, 
	0xA7, 
	0x27, 
	0x04, 
/* переход в RGB */
LCD_INTERFACE, 3, // | DELAY_FLAG,
	0x01, 
	0x00, 
	0x06, 
	//150,
};
// Команды перехода в RGB режим интерфейса (обновление изображения с внешнего источника).
static const FLASHMEM uint8_t
ili9341_switchtorgb_cmds [] = {                  // commands for ilitek ili9341 screens
	3, 			     	      // 14 commands in list:
	LCD_RGB_INTERFACE, 1,	// RGB Interface Signal Control (B0h)
		0x40, //40 42 60 62

	ST7735_COLMOD, 1 | DELAY_FLAG,	// #22
		0x0065,		// MCU color format = 16 bit/pixel, RGB color format = 18 bit/pixel
		150,

	LCD_INTERFACE, 3,	// Interface Control (F6h)
		0x01,
		0x03,
		0x06, //06 08
};

/**
 * \brief Send command to LCD controller.
 *
 * \param cmdv command.
 */
static void 
//NOINLINEAT
writecommand(uint_fast8_t cmdv)
{
	ST7735_CMND();	/* RS: Low: select an index or status register */
	/* Transfer cmd */
	spi_progval8_p1(targetlcd, cmdv);
	spi_complete(targetlcd);
}


/**
 * \brief Write data to LCD GRAM.
 *
 * \param datav data.
 */
static void writedata(uint_fast8_t datav)
{
	ST7735_DATA();	/* RS: High: select a control register */
	/* Transfer data */
	spi_progval8_p1(targetlcd, datav);
	spi_complete(targetlcd);
}

// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
static void 
commandList(const FLASHMEM uint8_t * addr) 
{

	uint_fast8_t  numCommands, numArgs;
	uint_fast8_t ms;

	spi_select2(targetlcd, ST7735_SPIMODE, ST7735_SPISPEED);		/* Enable SPI */

	numCommands = * addr ++;   // Number of commands to follow
	while (numCommands --) {                 // For each command...
		writecommand(* addr ++); //   Read, issue command
		numArgs  = * addr ++;    //   Number of args to follow
		ms       = numArgs & DELAY_FLAG;          //   If hibit set, local_delay_ms follows args
		numArgs &= ~ DELAY_FLAG;                   //   Mask out local_delay_ms bit
		while (numArgs--) {                   //   For each argument...
			writedata(* addr ++);  //     Read, issue argument
		}

		if (ms) {
			ms = * addr ++; // Read post-command local_delay_ms time (ms)
			if (ms == 255)     // If 255, local_delay_ms for 500 ms
				local_delay_ms(500);
			else if (ms == 10)
				local_delay_ms(10);
			else if (ms == 100)
				local_delay_ms(100);
			else if (ms == 150)
				local_delay_ms(150);
			else
				local_delay_ms(500);
		}
	}

	spi_unselect(targetlcd);	/* Disable SPI */
}


// Initialization code common to both 'B' and 'R' type displays
static void commonInit(const FLASHMEM uint8_t *cmdList) {

  colstart  = rowstart = 0; // May be overridden in init func
  if(cmdList) commandList(cmdList);
}

#if LCDMODE_ST7735

// Initialization for ST7735B screens
static void initB(void) {
  commonInit(Bcmd);
}

// Initialization for ST7735R screens (green or red tabs)
static void initR(uint8_t options) {
  commonInit(Rcmd1);
  if(options == INITR_GREENTAB) {
    colstart = 2;
    rowstart = 1;
  } else {
    // colstart, rowstart left at default '0' values
  }
  commandList(Rcmd3);
  tabcolor = options;
}

#endif /* LCDMODE_ST7735 */

static void st7735_initialize(void)
{
#if LCDMODE_ILI9341
    // colstart, rowstart left at default '0' values
	#if useSTinit
		commandList(ili9341_initcmdsST);	// Эта строка используется в случае применения инициализации от ST
	#else
		commandList(ili9341_initcmds);
	#endif

#elif LCDMODE_ILI9163
    // colstart, rowstart left at default '0' values
	commandList(ili9163_initcmds);
#else
  // If your TFT's plastic wrap has a Black Tab, use the following:
  initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  // If your TFT's plastic wrap has a Red Tab, use the following:
  //initR(INITR_REDTAB);   // initialize a ST7735R chip, red tab
  // If your TFT's plastic wrap has a Green Tab, use the following:
  //initR(INITR_GREENTAB); // initialize a ST7735R chip, green tab
#endif

}

/* Переключение дисплея в режим обновления видеопамяти по параллельному интерфейсу. */
static void st7735_switchtorgb(void)
{
#if LCDMODE_ILI9341 || LCDMODE_ILI9163
	#if useSTinit
		commandList(ili9341_switchtorgb_cmds_ST);

	#else

		commandList(ili9341_switchtorgb_cmds);
	#endif

#endif /* LCDMODE_ILI9341 */
}

/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{
	hardware_spi_master_setfreq(SPIC_SPEED10M, 10000000uL);
	hardware_spi_master_setfreq(SPIC_SPEED25M, 25000000uL);

	st7735_initialize();
	#if LCDMODE_LTDC
		st7735_switchtorgb();	/* Переключение дисплея в режим обновления видеопамяти по параллельному интерфейсу. */
	#endif
}

void display_set_contrast(uint_fast8_t v)
{
}

#if ! LCDMODE_LTDC

void 
display_clear(void)
{
	const COLOR_T bg = display_getbgcolor();

	st7735_clear(bg);
}

void
display_setcolors(COLOR_T fg, COLOR_T bg)
{
	st7735_setcolor(fg, bg, bg);
}

void display_setcolors3(COLOR_T fg, COLOR_T bg, COLOR_T fgbg)
{
	st7735_setcolor(fg, bg, fgbg);
}

void
display_wrdata_begin(void)
{
	st7735_set_strype(SMALLCHARH);
	st7735_put_char_begin();
}

void
display_wrdata_end(void)
{
	st7735_put_char_end();
}

void
display_wrdata2_begin(void)
{
	st7735_set_strype(SMALLCHARH);
	st7735_put_char_begin();
}

void
display_wrdata2_end(void)
{
	st7735_put_char_end();
}

void
display_wrdatabig_begin(void)
{
	st7735_set_strype(BIGCHARH);	// same as HALFCHARH
	st7735_put_char_begin();
}


void
display_wrdatabig_end(void)
{
	st7735_put_char_end();
}

/* отображение одной вертикальной полосы на графическом индикаторе */
/* старшие биты соответствуют верхним пикселям изображения */
/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
void 
display_barcolumn(uint_fast8_t pattern)
{
	st7735_pix8(pattern);
}

void
display_wrdatabar_begin(void)
{
	st7735_set_strype(CHAR_H);
	st7735_put_char_begin();
}

void
display_wrdatabar_end(void)
{
	st7735_put_char_end();
}

void
display_put_char_big(uint_fast8_t c, uint_fast8_t lowhalf)
{
	st7735_put_char_big(c);
}

void
display_put_char_half(uint_fast8_t c, uint_fast8_t lowhalf)
{
	st7735_put_char_half(c);
}


// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
void
display_put_char_small(uint_fast8_t c, uint_fast8_t lowhalf)
{
	st7735_put_char_small(c);
}

// Вызов этой функции только внутри display_wrdata2_begin() и display_wrdata2_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
void
display_put_char_small2(uint_fast8_t c, uint_fast8_t lowhalf)
{
	st7735_put_char_small(c);
}

void
display_gotoxy(uint_fast8_t x, uint_fast8_t y)
{
	st7735_y = y * CHAR_H;		/* переход от символьных координат к экранным */
	st7735_set_addr_column(x * CHAR_W);
}

// Координаты в пикселях
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
	st7735_y = y;		/* переход от символьных координат к экранным */
	st7735_set_addr_column(x);
}

void display_plotstart(
	uint_fast16_t height	// Высота окна в пикселях
	)
{
	st7735_set_strype(height);
	st7735_put_char_begin();
}

void display_plotstop(void)
{
	st7735_put_char_end();
}

void display_plot(
	const PACKEDCOLOR_T * buffer, 
	uint_fast16_t dx,	// Размеры окна в пикселях
	uint_fast16_t dy
	)
{
	uint_fast32_t len = GXSIZE(dx, dy);	// количество элементов
#if WITHSPIEXT16 && WITHSPIHWDMA
	// Передача в индикатор по DMA	
	arm_hardware_flush((uintptr_t) buffer, len * sizeof (* buffer));	// количество байтов
	hardware_spi_master_send_frame_16b(buffer, len);
#else /* WITHSPIEXT16 */
	if (len >= 3)
	{
		st7735_colorpixel_p1(* buffer ++);
		len -= 2;
		while (len --)
			st7735_colorpixel_p2(* buffer ++);
		st7735_colorpixel_p3(* buffer ++);
	}
	else if (len == 2)
	{
		st7735_colorpixel_p1(* buffer ++);
		st7735_colorpixel_p3(* buffer ++);
	}
#endif /* WITHSPIEXT16 */
}

uint_fast8_t
display_getreadystate(void)
{
	return 1;
}

#endif /* ! LCDMODE_LTDC */

/* аппаратный сброс дисплея - перед инициализаций */
/* вызывается при разрешённых прерываниях. */
void display_reset(void)
{
#if LCDMODE_ILI9163

	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(150); // local_delay_ms 150 ms

#else

	//************* Reset LCD Driver ****************//
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(1); // local_delay_ms 1ms
	board_lcd_reset(0); 	// Pull RST pin down
	board_update();
	local_delay_ms(10); // local_delay_ms 10ms
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(100); // local_delay_ms 100 ms

#endif
}



/* Разряжаем конденсаторы питания */
void display_discharge(void)
{
	spi_select2(targetlcd, ST7735_SPIMODE, ST7735_SPISPEED);	/* Enable SPI */
	writecommand(ST7735_DISPOFF);
	writecommand(ST7735_SLPIN);
	spi_unselect(targetlcd);		/* Disable SPI */
}

#endif /* LCDMODE_ST7735 || LCDMODE_ILI9163 || LCDMODE_ILI9341 */
