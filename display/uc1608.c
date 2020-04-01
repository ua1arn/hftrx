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
//NOINLINEAT
smallfont_decode(uint_fast8_t c)
{
	return c - ' ';
}

// многополосный вывод символов - за несколько горизонтальных проходов.
// Нумерация полос - сверху вниз, начиная с 0
// Вызов этой функции только внутри display_wrdata_begin() и 	display_wrdata_end();
static void uc1608_put_char_small(char cc, uint_fast8_t lowhalf)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	//enum { NBYTES = (sizeof uc1608_smallfont / sizeof uc1608_smallfont[0]) };
	enum { NCOLS = (sizeof uc1608_smallfont[0][0] / sizeof uc1608_smallfont[0][0][0]) };
	const FLASHMEM uint8_t * const p = & uc1608_smallfont[c][lowhalf][0];

	for (; i < NCOLS; ++ i)
    	uc1608_putoctet(p [i]);
}

// многополосный вывод символов - за несколько горизонтальных проходов.
// Нумерация полос - сверху вниз, начиная с 0
// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
static void uc1608_put_char_big(char cc, uint_fast8_t lowhalf)
{
	// '#' - узкий пробел
	enum { NBV = (BIGCHARH / 8) }; // сколько байтов в одной вертикали
	uint_fast8_t i = 1 * ((cc == '.' || cc == '#') ? 14 : 0);	// начальная колонка знакогенератора, откуда начинать.
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	//enum { NBYTES = (sizeof uc1608_bigfont / sizeof uc1608_bigfont[0]) };
	enum { NCOLS = (sizeof uc1608_bigfont [0][0] / sizeof uc1608_bigfont [0][0][0]) };
	const FLASHMEM uint8_t * const p = & uc1608_bigfont [c][lowhalf][0];

	for (; i < NCOLS; ++ i)
		uc1608_putoctet(p [i]);
}

// многополосный вывод символов - за несколько горизонтальных проходов.
// Нумерация полос - сверху вниз, начиная с 0
// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
static void uc1608_put_char_half(char cc, uint_fast8_t lowhalf)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	//enum { NBYTES = (sizeof uc1608_halffont / sizeof uc1608_halffont[0]) };
	enum { NCOLS = (sizeof uc1608_halffont [c][lowhalf] / sizeof uc1608_halffont [c][lowhalf] [0]) };
	const FLASHMEM uint8_t * const p = & uc1608_halffont [c][lowhalf][0];

	for (; i < NCOLS; ++ i)
		uc1608_putoctet(p [i]);
}

static void uc1608_put_char_small2(char cc)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	enum { NCOLS = (sizeof uc1601s_font[0] / sizeof uc1601s_font[0][0]) };
	const FLASHMEM uint8_t * const p = & uc1601s_font[c][0];

	for (; i < NCOLS; ++ i)
    	uc1608_putoctet(p [i]);
}

/*
 Функция установки курсора в позицию x,y
 X - координата по горизонтали в пределах 0-239,
 Y - координата по вертикали (строка) из восьми пикселов в пределах 0..15
*/
static void uc1608_set_addr_column(uint_fast8_t col, uint_fast8_t row)
{
/*
	if (row > 15)
	{
		global_disableIRQ();
		for (;;)
			;
	}
	if (col > 239)
	{
		global_disableIRQ();
		for (;;)
			;
	}
*/
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
	//const COLORMAIN_T bg = display_getbgcolor();
	unsigned i;
	
	display_gotoxy(0, 0);

	uc1608_put_char_begin();
	for (i = 0; i < ((unsigned long) DIM_Y * DIM_X) / 8; ++ i)
	{
		uc1608_putoctet(0x00);		// lower bit placed near to bottom of screen
	}
	uc1608_put_char_end();
}

void
display_setcolors(COLORMAIN_T fg, COLORMAIN_T bg)
{
	(void) fg;
	(void) bg;
}

void display_setcolors3(COLORMAIN_T fg, COLORMAIN_T bg, COLORMAIN_T fgbg)
{
	display_setcolors(fg, bg);
}

void
display_wrdata_begin(void)
{
	uc1608_put_char_begin();
}

void
display_wrdata_end(void)
{
	uc1608_put_char_end();
}

// начало вывода сиволов big и half
void
display_wrdatabig_begin(void)
{
	uc1608_put_char_begin();
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
void 
display_barcolumn(uint_fast8_t pattern)
{
	uc1608_putoctet(pattern);	// lower bit placed near to bottom of screen
}

void
display_wrdatabar_begin(void)
{
	uc1608_put_char_begin();
}

void
display_wrdatabar_end(void)
{
	uc1608_put_char_end();
}


void
display_put_char_big(uint_fast8_t c, uint_fast8_t lowhalf)
{
	uc1608_put_char_big(c, lowhalf);
}

void
display_put_char_half(uint_fast8_t c, uint_fast8_t lowhalf)
{
	uc1608_put_char_half(c, lowhalf);
}


// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
void
display_put_char_small(uint_fast8_t c, uint_fast8_t lowhalf)
{
	uc1608_put_char_small(c, lowhalf);
}

void
display_wrdata2_begin(void)
{
	uc1608_put_char_begin();
}

void
display_wrdata2_end(void)
{
	uc1608_put_char_end();
}

// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
void
display_put_char_small2(uint_fast8_t c, uint_fast8_t lowhalf)
{
	uc1608_put_char_small2(c);
}

void
display_gotoxy(uint_fast8_t x, uint_fast8_t y)
{
	uc1608_set_addr_column(x * CHAR_W, y);
}

// Координаты в пикселях
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
	uc1608_set_addr_column(x, y / CHAR_H);
}


void display_plotstart(
	uint_fast16_t height	// Высота окна в пикселях
	)
{

}

void display_plot(
	const PACKEDCOLORMAIN_T * buffer, 
	uint_fast16_t dx,	// Размеры окна в пикселях
	uint_fast16_t dy
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
void display_discharge(void)
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

#endif /* LCDMODE_UC1608 */
