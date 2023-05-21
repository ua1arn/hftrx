/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "display.h"
#include "formats.h"
#include "spi.h"

#include <stdint.h>

#if LCDMODE_UC1608

#include "uc1608.h"

/* растровые шрифты */
#define FONTSHERE 1
#include "fontmaps.h"



#define UC1608_SPIMODE		SPIC_MODE3
#if CTLSTYLE_SW2011ALL
	#define SPIC_SPEEDUC1608		SPIC_SPEED400k	// was: 200k SPISPEED for UC1608 should be less then 7.1 MHzs
#else /* CTLSTYLE_SW2011ALL */
	#define SPIC_SPEEDUC1608		SPIC_SPEED4M	// SPISPEED for UC1608 should be less then 7.1 MHzs
#endif /* CTLSTYLE_SW2011ALL */

#define tglcd	targetuc1608


#define UC1608_CTRL() do { board_lcd_rs(0); } while (0)	/* RS: Low: CONTROL */
#define UC1608_DATA() do { board_lcd_rs(1); } while (0)	/* RS: High: DISPLAY DATA TRANSFER */

void uc1608_gotoxy(uint_fast8_t x, uint_fast8_t y);

// S8 SPI mode (not S8uc) used.

static void uc1608_write_cmd(uint_fast8_t v1)
{
	UC1608_CTRL();	/* RS: Low: select an index or status register */
	/* Enable SPI */
	spi_select2(tglcd, UC1608_SPIMODE, SPIC_SPEEDUC1608);

	/* Transfer cmd */
	spi_progval8_p1(tglcd, v1);
	spi_complete(tglcd);

	/* Disable SPI */
	spi_unselect(tglcd);
}

static void uc1608_write_cmd2(uint_fast8_t v1, uint_fast8_t v2)
{
	UC1608_CTRL();	/* RS: Low: select an index or status register */
	/* Enable SPI */
	spi_select2(tglcd, UC1608_SPIMODE, SPIC_SPEEDUC1608);

	/* Transfer cmd */
	spi_progval8_p1(tglcd, v1);
	spi_progval8_p2(tglcd, v2);
	spi_complete(tglcd);

	/* Disable SPI */
	spi_unselect(tglcd);
}

static uint_fast8_t uc1608_started;

// в режим передачи данных переводим сразу по окончании команд.
static void 
//NOINLINEAT
uc1608_put_char_begin(void)
{
	uc1608_started = 1;		/* первый символ выдаём без ожидания готовности */
	UC1608_DATA();	/* RS: High: select a control register */
	spi_select2(tglcd, UC1608_SPIMODE, SPIC_SPEEDUC1608);
}



// lower bit placed near to bottom of screen
static void 
//NOINLINEAT
uc1608_putoctet(
	uint_fast8_t v
	)
{
	if (uc1608_started != 0)
	{
		spi_progval8_p1(tglcd, v);	/* первый байт выдаём без ожидания готовности */
		uc1608_started = 0;
	}
	else
	{
		spi_progval8_p2(tglcd, v);	/* последующие байты передаются с предварительным ожиданием готовности */
	}
}


static void 
//NOINLINEAT
uc1608_put_char_end(void)
{
	if (uc1608_started == 0)	// Если выдадли хоть один символ
		spi_complete(tglcd);		/* ожидаем завершения передачи последнего байта */
	spi_unselect(tglcd);
}

// многополосный вывод символов - за несколько горизонтальных проходов.
// Нумерация полос - сверху вниз, начиная с 0
// Вызов этой функции только внутри display_wrdata_begin() и 	display_wrdata_end();
static uint_fast16_t uc1608_put_char_small(uint_fast16_t xpix, char cc, uint_fast8_t lowhalf)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = smallfont_decode(cc);
	//enum { NBYTES = (sizeof uc1608_smallfont / sizeof uc1608_smallfont[0]) };
	enum { NCOLS = (sizeof uc1608_smallfont[0][0] / sizeof uc1608_smallfont[0][0][0]) };
	const FLASHMEM uint8_t * const p = & uc1608_smallfont[c][lowhalf][0];

	for (; i < NCOLS; ++ i)
    	uc1608_putoctet(p [i]);
	return xpix + NCOLS;
}

// многополосный вывод символов - за несколько горизонтальных проходов.
// Нумерация полос - сверху вниз, начиная с 0
// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
static uint_fast16_t uc1608_put_char_big(uint_fast16_t xpix, char cc, uint_fast8_t lowhalf)
{
	// '#' - узкий пробел
	enum { NBV = (BIGCHARH / 8) }; // сколько байтов в одной вертикали
	uint_fast8_t i = 1 * ((cc == '.' || cc == '#') ? 14 : 0);	// начальная колонка знакогенератора, откуда начинать.
    const uint_fast8_t width = bigfont_width(cc);
    const uint_fast8_t c = bigfont_decode(cc);
	//enum { NBYTES = (sizeof uc1608_bigfont / sizeof uc1608_bigfont[0]) };
	enum { NCOLS = (sizeof uc1608_bigfont [0][0] / sizeof uc1608_bigfont [0][0][0]) };
	const FLASHMEM uint8_t * const p = & uc1608_bigfont [c][lowhalf][0];

	for (; i < NCOLS; ++ i)
		uc1608_putoctet(p [i]);
	return xpix + NCOLS;
}

// многополосный вывод символов - за несколько горизонтальных проходов.
// Нумерация полос - сверху вниз, начиная с 0
// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
static uint_fast16_t uc1608_put_char_half(uint_fast16_t xpix, char cc, uint_fast8_t lowhalf)
{
	uint_fast8_t i = 0;
    const uint_fast8_t width = halffont_width(cc);
    const uint_fast8_t c = bigfont_decode(cc);
	//enum { NBYTES = (sizeof uc1608_halffont / sizeof uc1608_halffont[0]) };
	enum { NCOLS = (sizeof uc1608_halffont [c][lowhalf] / sizeof uc1608_halffont [c][lowhalf] [0]) };
	const FLASHMEM uint8_t * const p = & uc1608_halffont [c][lowhalf][0];

	for (; i < NCOLS; ++ i)
		uc1608_putoctet(p [i]);
	return xpix + NCOLS;
}

static uint_fast16_t uc1608_put_char_small2(uint_fast16_t xpix, char cc)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = smallfont_decode(cc);
	enum { NCOLS = (sizeof uc1601s_font[0] / sizeof uc1601s_font[0][0]) };
	const FLASHMEM uint8_t * const p = & uc1601s_font[c][0];

	for (; i < NCOLS; ++ i)
    	uc1608_putoctet(p [i]);
	return xpix + NCOLS;
}

/*
 Функция установки курсора в позицию x,y
 X - координата по горизонтали в пределах 0-239,
 Y - координата по вертикали (строка) из восьми пикселов в пределах 0..15
*/
static void uc1608_set_addr_column(uint_fast8_t col, uint_fast8_t row)
{
	UC1608_CTRL();	/* Send commands */
	/* Enable SPI */
	spi_select2(tglcd, UC1608_SPIMODE, SPIC_SPEEDUC1608);
	/* Transfer cmd */
	spi_progval8_p1(tglcd, 0x40);			// set start line = 0
	//spi_progval8_p2(tglcd, 0x88);			// set RAM address control
	spi_progval8_p2(tglcd, 0xb0 | (row & 0x0F));	// ! mask added set page address
	spi_progval8_p2(tglcd, 0x00 | (col & 0x0F));		
	spi_progval8_p2(tglcd, 0x10 | ((col >> 4) & 0x0F));	// ! mask added

	spi_complete(tglcd);

	/* Disable SPI */
	spi_unselect(tglcd);
}

void 
display_clear(void)
{
	//const COLORPIP_T bg = display_getbgcolor();
	unsigned i;
	
	uc1608_gotoxy(0, 0);

	uc1608_put_char_begin();
	for (i = 0; i < ((unsigned long) DIM_Y * DIM_X) / 8; ++ i)
	{
		uc1608_putoctet(0x00);		// lower bit placed near to bottom of screen
	}
	uc1608_put_char_end();
}

// для framebufer дисплеев - вытолкнуть кэш память
void display_flush(void)
{
}

void
colmain_setcolors(COLORPIP_T fg, COLORPIP_T bg)
{
	(void) fg;
	(void) bg;
}

void colmain_setcolors3(COLORPIP_T fg, COLORPIP_T bg, COLORPIP_T fgbg)
{
	colmain_setcolors(fg, bg);
}

uint_fast16_t
display_wrdata_begin(uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp)
{
	uc1608_put_char_begin();
	* yp = GRID2Y(ycell);
	return GRID2X(xcell);
}

void
display_wrdata_end(void)
{
	uc1608_put_char_end();
}

// начало вывода сиволов big и half
uint_fast16_t
display_wrdatabig_begin(uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp)
{
	uc1608_put_char_begin();
	* yp = GRID2Y(ycell);
	return GRID2X(xcell);
}

// завершение вывода сиволов big и half
void
display_wrdatabig_end(void)
{
	uc1608_put_char_end();
}

/* отображение одной вертикальной полосы на графическом индикаторе */
/* старшие биты соответствуют верхним пикселям изображения */
/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
uint_fast16_t
display_barcolumn(uint_fast16_t xpix, uint_fast16_t ypix, uint_fast8_t pattern)

{
	uc1608_putoctet(pattern);	// lower bit placed near to bottom of screen
	return xpix + 1;
}

uint_fast16_t
display_wrdatabar_begin(uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp)
{
	uc1608_put_char_begin();
	* yp = GRID2Y(ycell);
	return GRID2X(xcell);
}

void
display_wrdatabar_end(void)
{
	uc1608_put_char_end();
}


uint_fast16_t
display_put_char_big(uint_fast16_t xpix, uint_fast16_t ypix, uint_fast8_t c, uint_fast8_t lowhalf)
{
	return uc1608_put_char_big(xpix, c, lowhalf);
}

uint_fast16_t
display_put_char_half(uint_fast16_t xpix, uint_fast16_t ypix, uint_fast8_t c, uint_fast8_t lowhalf)
{
	return uc1608_put_char_half(xpix, c, lowhalf);
}


// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
uint_fast16_t
display_put_char_small(uint_fast16_t xpix, uint_fast16_t ypix, char c, uint_fast8_t lowhalf)
{
	return uc1608_put_char_small(xpix, c, lowhalf);
}

uint_fast16_t
display_wrdata2_begin(uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp)
{
	uc1608_put_char_begin();
	* yp = GRID2Y(ycell);
	return GRID2X(xcell);
}

void
display_wrdata2_end(void)
{
	uc1608_put_char_end();
}

// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения

uint_fast16_t
display_put_char_small2(uint_fast16_t xpix, uint_fast16_t ypix, char cc, uint_fast8_t lowhalf)
{
	return uc1608_put_char_small2(xpix, cc);
}

void
uc1608_gotoxy(uint_fast8_t x, uint_fast8_t y)
{
	uc1608_set_addr_column(x * CHAR_W, y);
}

// Координаты в пикселях
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
	uc1608_set_addr_column(x, y / CHAR_H);
}


void display_plotstart(
	uint_fast16_t dy	// Высота окна в пикселях
	)
{

}

void display_plot(
	const PACKEDCOLORPIP_T * buffer, 
	uint_fast16_t dx,	// Размеры окна в пикселях
	uint_fast16_t dy,
	uint_fast16_t xpix,	// начало области рисования
	uint_fast16_t ypix
	)
{

}

void display_plotstop(void)
{

}


static void uc1608_resetdelay(void)
{
	local_delay_ms(150); // Delay 50 ms
}
/* аппаратный сброс дисплея - перед инициализаций */
/* вызывается при разрешённых прерываниях. */
void display_reset(void)
{
	//************* Reset LCD Driver ****************//
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	uc1608_resetdelay();
	board_lcd_reset(0); 	// Pull RST pin down
	board_update();
	local_delay_ms(1); // Delay 1ms
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	uc1608_resetdelay();
}

/* Разряжаем конденсаторы питания */
void display_uninitialize(void)
{
	uc1608_write_cmd(0xe2);	// System reset
	local_delay_ms(5);		// wait draining
}

static uint_fast8_t vbias = DEFAULT_LCD_CONTRAST;

void display_set_contrast(uint_fast8_t v)
{
	if (v != vbias)
	{
		vbias = v;
		uc1608_write_cmd2(0x81, (0x02 << 6) | ((vbias & 0x3F) << 0));	// Set gain and PM
	}
}


/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{
	uc1608_write_cmd(0xe2);	// System reset
	uc1608_resetdelay();


	#if LCDMODE_UC1608_TOPDOWN
		uc1608_write_cmd(0xc5);	// Set Display mapping	MX=0, MY=1, MSF=1
	#else /* LCDMODE_UC1608_TOPDOWN */
		uc1608_write_cmd(0xc9);	// Set Display mapping	MX=1, MY=0, MSF=1
	#endif /* LCDMODE_UC1608_TOPDOWN */
	//uc1608_write_cmd(0x20);	// Set MR & TC
	//uc1608_write_cmd(0xeb);	// Set bias ratio

	uc1608_write_cmd2(0x81, (0x02 << 6) | ((vbias & 0x3F) << 0));	// Set gain and PM

	//uc1608_write_cmd(0x88);			// set RAM address control - старани работает с автоникрементом
	//uc1608_write_cmd(0x40);			// set start line = 0

	uc1608_write_cmd(0xaf);	// Set Display Enable
}

void display_nextfb(void)
{
}

#endif /* LCDMODE_UC1608 */
