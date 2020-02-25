/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "display.h"
#include <stdint.h>

#include "../inc/spi.h"
#include "display.h"
#include "formats.h"

#if LCDMODE_ST7565S || LCDMODE_PTE1206

#include "st7565s.h"


/* растровые шрифты */
#include "./fonts/uc1601s_font_small.h"
#if FONTSTYLE_ITALIC
	#include "./fonts/uc1601s_ifont_half.h"
	#include "./fonts/uc1601s_ifont_big.h"
#else /* FONTSTYLE_ITALIC */
	#include "./fonts/uc1601s_font_half.h"
	#include "./fonts/uc1601s_font_big.h"
#endif /* FONTSTYLE_ITALIC */



#define CMD_DISPLAY_OFF   0xAE
#define CMD_DISPLAY_ON    0xAF

#define CMD_SET_DISP_START_LINE  0x40
#define CMD_SET_PAGE  0xB0

#define CMD_SET_COLUMN_UPPER  0x10
#define CMD_SET_COLUMN_LOWER  0x00

#define CMD_SET_ADC_NORMAL  0xA0
#define CMD_SET_ADC_REVERSE 0xA1

#define CMD_SET_DISP_NORMAL 0xA6
#define CMD_SET_DISP_REVERSE 0xA7

#define CMD_SET_ALLPTS_NORMAL 0xA4
#define CMD_SET_ALLPTS_ON  0xA5
#define CMD_SET_BIAS_9 0xA2
#define CMD_SET_BIAS_7 0xA3

#define CMD_RMW  0xE0
#define CMD_RMW_CLEAR 0xEE
#define CMD_INTERNAL_RESET  0xE2
#define CMD_SET_COM_NORMAL  0xC0
#define CMD_SET_COM_REVERSE  0xC8
#define CMD_SET_POWER_CONTROL  0x28
#define CMD_SET_RESISTOR_RATIO  0x20
#define CMD_SET_VOLUME_FIRST  0x81
#define CMD_SET_STATIC_OFF  0xAC
#define  CMD_SET_STATIC_ON  0xAD
#define CMD_SET_STATIC_REG  0x0
#define CMD_SET_BOOSTER_FIRST  0xF8
#define  CMD_SET_BOOSTER_234  0
#define  CMD_SET_BOOSTER_5  1
#define  CMD_SET_BOOSTER_6  3
#define CMD_NOP  0xE3
#define CMD_TEST  0xF0

#define ST7565S_SPIMODE		SPIC_MODE3 /* mode3 & mode0 работает, mode2 не работает */
#define ST7565S_SPIC_SPEED	SPIC_SPEED100k //SPIC_SPEED4M	// st7565s. st7565v: 10 MHz max

#define ST7565S_CTRL() do { board_lcd_rs(0); } while (0)	/* RS: Low: CONTROL */
#define ST7565S_DATA() do { board_lcd_rs(1); } while (0)	/* RS: High: DISPLAY DATA TRANSFER */


static void
st7565s_reset(void)
{
	ST7565S_CTRL();	/* RS: Low: select an index or status register */

	local_delay_ms(5);		// delay 5 mS
	board_lcd_reset(0); 	// Pull RST pin down
	board_update();
	local_delay_ms(10);		// delay 10 mS (for LPH88, 5 for LS020).
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(5);		// delay 5 mS
}

// S8 SPI mode (not S8uc) used.

static void st7565s_write_cmd(uint_fast8_t v1)
{
	ST7565S_CTRL();	/* сделано состоянием по умолчанию RS: Low: select an index or status register */
	/* Enable SPI */
	spi_select2(targetlcd, ST7565S_SPIMODE, ST7565S_SPIC_SPEED);	/* Enable SPI */

	/* Transfer cmd */
	spi_progval8_p1(targetlcd, v1);
	spi_complete(targetlcd);

	/* Disable SPI */
	spi_unselect(targetlcd);
}

// В контроллере ST7565s команды нормально исполняются и если снимать /CS между ними - но для ускорения выдачи сделаю такую функцию
static void st7565s_write_cmd2(uint_fast8_t v1, uint_fast8_t v2)
{
	ST7565S_CTRL();	/* сделано состоянием по умолчанию RS: Low: select an index or status register */
	/* Enable SPI */
	spi_select2(targetlcd, ST7565S_SPIMODE, ST7565S_SPIC_SPEED);	/* Enable SPI */

	/* Transfer cmd */
	spi_progval8_p1(targetlcd, v1);
	spi_progval8_p2(targetlcd, v2);
	spi_complete(targetlcd);

	/* Disable SPI */
	spi_unselect(targetlcd);
}


static uint_fast8_t st7565s_started;


// начало выдаче байтов (записи в видеопамять)
// Вызывается в начале выдачи строки
static void st7565s_put_char_begin(void)
{
    ST7565S_DATA();
	/* Enable SPI */
	spi_select2(targetlcd, ST7565S_SPIMODE, ST7565S_SPIC_SPEED);	/* Enable SPI */
	st7565s_started = 1;		/* первый символ выдаём без ожидания готовности */
}

static void st7565s_put_char_end(void)
{
	if (st7565s_started == 0)		// Если выдадли хоть один символ
		spi_complete(targetlcd);		/* ожидаем завершения передачи последнего байта */
	/* Disable SPI */
	spi_unselect(targetlcd);
	ST7565S_CTRL();	/* сделано состоянием по умолчанию RS: Low: select an index or status register */
}



// lower bit placed near to bottom of screen
static void
//NOINLINEAT
st7565s_pix8(
	uint_fast8_t v
	)
{
	if (st7565s_started != 0)
	{
		spi_progval8_p1(targetlcd, v);	/* первый байт выдаём без ожидания готовности */
		st7565s_started = 0;
	}
	else
	{
		spi_progval8_p2(targetlcd, v);	/* последующие байты передаются с предварительным ожиданием готовности */
	}
}

/*
 Функция установки курсора в позицию x,y
 X - координата по горизонтали в пределах 9..127 .0-132,
 Y - координата по вертикали (строка, Page) в пределах 0-7
*/ 
static void st7565s_set_addr_column(uint_fast16_t x, uint_fast16_t y)		//
{
	ST7565S_CTRL();	/* сделано состоянием по умолчанию RS: Low: select an index or status register */
	/* Enable SPI */
	spi_select2(targetlcd, ST7565S_SPIMODE, ST7565S_SPIC_SPEED);	/* Enable SPI */

	/* Transfer cmd */
	spi_progval8_p1(targetlcd, CMD_SET_PAGE | (y & 0x0f));
	spi_progval8_p2(targetlcd, CMD_SET_COLUMN_LOWER | (x & 0x0f));
	spi_progval8_p2(targetlcd, CMD_SET_COLUMN_UPPER | (x >> 4));
	spi_complete(targetlcd);

	/* Disable SPI */
	spi_unselect(targetlcd);
}




static void st7565s_clear(void)
{
    uint_fast8_t cnt_y;

	for (cnt_y = 0; cnt_y < ((DIM_Y + 7) / 8); ++ cnt_y)
	{
	    uint_fast8_t cnt_x;

		display_gotoxy(0, cnt_y);

		st7565s_put_char_begin();

	    for (cnt_x = 0; cnt_x < DIM_X; ++ cnt_x)
		{
			st7565s_pix8(0x00);
		}
	    st7565s_put_char_end();
	}
}


// st7565s_bigfont & st7565s_halffont decode
static uint_fast8_t 
NOINLINEAT
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


// Вызовы этой функции (или группу вызовов) требуется "обрамить" парой вызовов
// st7565s_put_char_begin() и st7565s_put_char_end().
//
static void st7565s_put_char_small(char cc)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	enum { NCOLS = (sizeof uc1601s_font [0] / sizeof uc1601s_font [0][0]) };
	const FLASHMEM uint8_t * const p = & uc1601s_font [c][0];

	for (; i < NCOLS; ++ i)
    	st7565s_pix8(p [i]);
}


/* Исключающее ИЛИ с точкой в растре */
// use display_pixelbuffer_xor
uint_fast16_t display_pixelbuffer_char(
	GX_t * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	char cc
	)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	enum { NCOLS = (sizeof uc1601s_font [0] / sizeof uc1601s_font [0][0]) };
	const FLASHMEM uint8_t * const p = & uc1601s_font [c][0];

	for (; i < NCOLS; ++ i)
	{
		const uint_fast8_t v = p [i];

		if (v & 0x01)	display_pixelbuffer_xor(buffer, dx, dy, col + i, row + 7);
		if (v & 0x02)	display_pixelbuffer_xor(buffer, dx, dy, col + i, row + 6);
		if (v & 0x04)	display_pixelbuffer_xor(buffer, dx, dy, col + i, row + 5);
		if (v & 0x08)	display_pixelbuffer_xor(buffer, dx, dy, col + i, row + 4);
		if (v & 0x10)	display_pixelbuffer_xor(buffer, dx, dy, col + i, row + 3);
		if (v & 0x20)	display_pixelbuffer_xor(buffer, dx, dy, col + i, row + 2);
		if (v & 0x40)	display_pixelbuffer_xor(buffer, dx, dy, col + i, row + 1);
		if (v & 0x80)	display_pixelbuffer_xor(buffer, dx, dy, col + i, row + 0);
	}
	return NCOLS;
}

// Вызовы этой функции (или группу вызовов) требуется "обрамить" парой вызовов
// st7565s_put_char_begin() и st7565s_put_char_end().
//
static void st7565s_neg_put_char_small(char cc)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	enum { NCOLS = (sizeof uc1601s_font [0] / sizeof uc1601s_font [0][0]) };
	const FLASHMEM uint8_t * const p = & uc1601s_font [c][0];

	for (; i < NCOLS; ++ i)
    	st7565s_pix8(~ p [i]);
}

// многополосный вывод символов - за несколько горизонтальных проходов.
// Нумерация полос - сверху вниз, начиная с 0

// Вызовы этой функции (или группу вызовов) требуется "обрамить" парой вызовов
// st7565s_put_char_begin() и st7565s_put_char_end().
//
static void st7565s_put_char_big(char cc, uint_fast8_t lowhalf)
{
	// '#' - узкий пробел
	enum { NBV = (BIGCHARH / 8) }; // сколько байтов в одной вертикали
	uint_fast8_t i = 1 * ((cc == '.' || cc == '#') ? 6 : 0);	// начальная колонка знакогенератора, откуда начинать.
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NCOLS = (sizeof uc1601s_bigfont [0][0] / sizeof uc1601s_bigfont [0][0][0]) };
	const FLASHMEM uint8_t * const p = & uc1601s_bigfont [c][lowhalf][0];

	for (; i < NCOLS; ++ i)
    	st7565s_pix8(p [i]);
}

// многополосный вывод символов - за несколько горизонтальных проходов.
// Нумерация полос - сверху вниз, начиная с 0

// Вызовы этой функции (или группу вызовов) требуется "обрамить" парой вызовов
// st7565s_put_char_begin() и st7565s_put_char_end().
//
static void st7565s_put_char_half(char cc, uint_fast8_t lowhalf)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NCOLS = (sizeof uc1601s_halffont [c][lowhalf] / sizeof uc1601s_halffont [c][lowhalf] [0]) };
	const FLASHMEM uint8_t * const p = & uc1601s_halffont [c][lowhalf][0];

	for (; i < NCOLS; ++ i)
    	st7565s_pix8(p [i]);

}

/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
static void 
st7565s_bar_column(uint_fast8_t pattern)
{
	st7565s_pix8(pattern);
}

#if 0

static uint_fast8_t vbias = DEFAULT_LCD_CONTRAST;

static void st7565s_set_contrast(uint_fast8_t v)
{
	if (vbias != v)
	{
		vbias = v;
	}
}

#endif

#if 0

// Выключение дисплея (и включение его).
// TODO: перенести сюда управление питанием LCD
static void st7565s_disable(uint_fast8_t state)
{
	if (state == 0)
	{
		// Enable LCD
		st7565s_write_cmd(CMD_DISPLAY_ON);
	}
	else
	{
		// Disable LCD
		st7565s_write_cmd(CMD_DISPLAY_OFF);
	}
}
#endif
#if 0
static void DisplayOn(void)   
{   
	st7565s_write_cmd(0x2c);//booster on   
	local_delay_ms(20);   
	st7565s_write_cmd(0x2e);//regulator on   
	local_delay_ms(20);   
	st7565s_write_cmd(0x2f);//follow on   
	local_delay_ms(20);   

	st7565s_write_cmd(0xaf);//display on   
}   
#endif

static void pte1206_initialize(void)
{
	debug_printf_P(PSTR("pte1206_initialize()\n"));
	//st7565s_write_cmd(0xE2);//reset   
	//st7565s_write_cmd(0x40);//start line   
	//st7565s_write_cmd(0xA1);//seg direction: normal   
	//st7565s_write_cmd(CMD_SET_DISP_NORMAL);//display reverse: normal   
#if LCDMODE_PTE1206_TOPDOWN
	st7565s_write_cmd(CMD_SET_ADC_REVERSE);		// left-right reverse (on 132 columns basis)
	st7565s_write_cmd(CMD_SET_COM_REVERSE);		// top-down reverse (по узкой стороне меняется порядок строк)
#else /* LCDMODE_ST7565S_TOPDOWN */
	st7565s_write_cmd(CMD_SET_ADC_NORMAL);
	st7565s_write_cmd(CMD_SET_COM_NORMAL);
#endif
	st7565s_write_cmd(CMD_SET_DISP_START_LINE);

	st7565s_write_cmd(CMD_SET_ALLPTS_NORMAL);
	st7565s_write_cmd(CMD_SET_BIAS_9);		//Bias: 1/6   
	//st7565s_write_cmd(0xC8);//com direction: normal  c8 or c0   
	st7565s_write_cmd(0x25);//resistor ratio   
	st7565s_write_cmd2(0x81, 0x18);//64 levels   
	st7565s_write_cmd2(0xF3, 0x00);//reduce frame frequence   

	st7565s_write_cmd(CMD_SET_POWER_CONTROL | 0x4 );
	local_delay_ms(50);   
	st7565s_write_cmd(CMD_SET_POWER_CONTROL | 0x6);
	local_delay_ms(50);   
	st7565s_write_cmd(CMD_SET_POWER_CONTROL | 0x7);
	local_delay_ms(50);   
	st7565s_write_cmd(CMD_DISPLAY_ON);//display on   

	st7565s_write_cmd2(CMD_SET_VOLUME_FIRST, 0x14);
	debug_printf_P(PSTR("pte1206_initialize() done\n"));
}

static void st7565s_initialize_bad(void)
{
	 st7565s_write_cmd(0xa2);//set seg direct
	 st7565s_write_cmd(0xc8);//set com direct
	 st7565s_write_cmd(0x40);//set com direct
	 st7565s_write_cmd(0xA2);//set lcd bias
	 st7565s_write_cmd(0x25);//set internal rb/ra
	 st7565s_write_cmd(0x2f);//all power on
	 st7565s_write_cmd(0x81);//set contrast
	 st7565s_write_cmd(0x23);//
	 st7565s_write_cmd(0xf8);
	 st7565s_write_cmd(0x00);
	 st7565s_write_cmd(0x2f);
	 st7565s_write_cmd(0xaf);//dispaly on


}
static void st7565s_initialize(void)
{
	debug_printf_P(PSTR("st7565s_initialize()\n"));
	
	local_delay_ms(10);			// delay required after [power on] reset

	st7565s_write_cmd(CMD_SET_BIAS_7);

#if LCDMODE_ST7565S_TOPDOWN
	st7565s_write_cmd(CMD_SET_ADC_REVERSE);		// left-right reverse (on 132 columns basis)
	st7565s_write_cmd(CMD_SET_COM_REVERSE);		// top-down reverse (по узкой стороне меняется порядок строк)
#else /* LCDMODE_ST7565S_TOPDOWN */
	st7565s_write_cmd(CMD_SET_ADC_NORMAL);
	st7565s_write_cmd(CMD_SET_COM_NORMAL);
#endif
	st7565s_write_cmd(CMD_SET_DISP_START_LINE);

	st7565s_write_cmd(CMD_SET_POWER_CONTROL | 0x4 );
	local_delay_ms(50);
	st7565s_write_cmd(CMD_SET_POWER_CONTROL | 0x7);
	local_delay_ms(50);

	//st7565s_write_cmd2(CMD_SET_BOOSTER_FIRST, CMD_SET_BOOSTER_6);	// данная команда не требуется
	st7565s_write_cmd2(CMD_SET_BOOSTER_FIRST, CMD_SET_BOOSTER_234);	// если внешнее питание данная команда не требуется


	st7565s_write_cmd(CMD_DISPLAY_ON);
	st7565s_write_cmd(CMD_SET_ALLPTS_NORMAL);

	st7565s_write_cmd2(CMD_SET_VOLUME_FIRST, 0x24);
	debug_printf_P(PSTR("st7565s_initialize() done\n"));
}

void display_testframe(uint_fast8_t state)
{
	st7565s_write_cmd(state ? CMD_SET_ALLPTS_ON : CMD_SET_ALLPTS_NORMAL);
}
/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{
	enum { BLADDR_W = 0x5E };	// MCP4017
	{
		// backlight control
		i2c_start(BLADDR_W);
	    i2c_write(0x60);	// 0x7F: off, 0x70: minimal
	    i2c_waitsend();
	    i2c_stop();

	}

	#if LCDMODE_PTE1206
	pte1206_initialize();
	#else /* LCDMODE_PTE1206 */
	st7565s_initialize();
	#endif /* LCDMODE_PTE1206 */
}

void display_set_contrast(uint_fast8_t v)
{
	//st7565s_set_contrast(v);
}

void 
display_clear(void)
{
	st7565s_clear();
}

void
display_setcolors(COLOR_T fg, COLOR_T bg)
{
	(void) fg;
	(void) bg;
}

void display_setcolors3(COLOR_T fg, COLOR_T bg, COLOR_T fgbg)
{
	display_setcolors(fg, bg);
}

void
display_wrdata_begin(void)
{
	st7565s_put_char_begin();
}

void
display_wrdata_end(void)
{
	st7565s_put_char_end();
}

void
display_wrdatabar_begin(void)
{
	st7565s_put_char_begin();
}

void
display_wrdatabar_end(void)
{
	st7565s_put_char_end();
}


void
display_wrdatabig_begin(void)
{
	st7565s_put_char_begin();
}


void
display_wrdatabig_end(void)
{
	st7565s_put_char_end();
}

/* отображение одной вертикальной полосы на графическом индикаторе */
/* старшие биты соответствуют верхним пикселям изображения */
/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
void 
display_barcolumn(uint_fast8_t pattern)
{
	st7565s_bar_column(pattern);
}

void
display_put_char_big(uint_fast8_t c, uint_fast8_t lowhalf)
{
	st7565s_put_char_big(c, lowhalf);
}

void
display_put_char_half(uint_fast8_t c, uint_fast8_t lowhalf)
{
	st7565s_put_char_half(c, lowhalf);
}


// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
void
display_put_char_small(uint_fast8_t c, uint_fast8_t lowhalf)
{
	(void) lowhalf;
	st7565s_put_char_small(c);
}

// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
void
display_neg_put_char_small(uint_fast8_t c, uint_fast8_t lowhalf)
{
	(void) lowhalf;
	st7565s_neg_put_char_small(c);
}

// самый маленький шрифт
// stub function
void display_wrdata2_begin(void)
{
	display_wrdata_begin();
}
// stub function
void display_wrdata2_end(void)
{
	display_wrdata_end();
}
// stub function
void display_put_char_small2(uint_fast8_t c, uint_fast8_t lowhalf)
{
	display_put_char_small(c, lowhalf);
}

void
display_gotoxy(uint_fast8_t x, uint_fast8_t y)
{
#if LCDMODE_ST7565S_TOPDOWN || LCDMODE_PTE1206_TOPDOWN
	st7565s_set_addr_column(x * CHAR_W + (132 - DIM_X), DIM_Y / 8 - 1 - y);
	st7565s_set_addr_column(x * CHAR_W + (132 - DIM_X), DIM_Y / 8 - 1 - y);
#else /* LCDMODE_ST7565S_TOPDOWN */
	st7565s_set_addr_column(x * CHAR_W, DIM_Y / 8 - 1 - y);
	st7565s_set_addr_column(x * CHAR_W, DIM_Y / 8 - 1 - y);
#endif /* LCDMODE_ST7565S_TOPDOWN */
}

// Координаты в пикселях
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
#if LCDMODE_ST7565S_TOPDOWN || LCDMODE_PTE1206_TOPDOWN
	st7565s_set_addr_column(x + (132 - DIM_X), DIM_Y / 8 - 1 - (y / 8));
	st7565s_set_addr_column(x + (132 - DIM_X), DIM_Y / 8 - 1 - (y / 8));
#else /* LCDMODE_ST7565S_TOPDOWN */
	st7565s_set_addr_column(x, DIM_Y / 8 - 1 - (y / 8));
	st7565s_set_addr_column(x, DIM_Y / 8 - 1 - (y / 8));
#endif /* LCDMODE_ST7565S_TOPDOWN */
}


void display_plotstart(
	uint_fast16_t height	// Высота окна в пикселях
	)
{

}

void display_plot(
	const PACKEDCOLOR_T * buffer, 
	uint_fast16_t dx,	// Размеры окна в пикселях
	uint_fast16_t dy
	)
{

}

void display_plotstop(void)
{

}

/* аппаратный сброс дисплея - перед инициализаций */
/* вызывается при разрешённых прерываниях. */
void
display_reset(void)
{
	st7565s_reset();
}

/* Разряжаем конденсаторы питания */
void display_discharge(void)
{
	st7565s_write_cmd(CMD_DISPLAY_OFF);
}

#endif /* LCDMODE_ST7565S || LCDMODE_PTE1206 */
