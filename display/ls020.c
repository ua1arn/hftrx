/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Доработки для LS020 Василий Линывый, livas60@mail.ru
//
// Siemens S65 Display Control

/* Индикатор 176*132 Sharp LS020B8UD06 с контроллером LR38826 */


#include "hardware.h"
#include "board.h"

#include "display.h"
#include <stdint.h>

#include "spi.h"

#if LCDMODE_LS020

#include "ls020.h"
/* растровые шрифты */
#include "fontmaps.h"

#define LS020_SPIMODE SPIC_MODE3


static void
ls020_reset(void)
{
	//spi_select(targetlcd, LS020_SPIMODE);
	//spi_unselect(targetlcd);

	local_delay_ms(5);		// delay 5 mS
	board_lcd_reset(0); 	// Pull RST pin down
	board_update();
	local_delay_ms(10);		// delay 10 mS (for LPH88, 5 for LS020).
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(5);		// delay 5 mS
}


#define LS020_CMND() do { board_lcd_rs(1); } while (0)	/* RS: High: command data */
#define LS020_DATA() do { board_lcd_rs(0); } while (0)	/* RS: Low: display data or parameter */

static void 
//NOINLINEAT
ls020_wrcmd16(uint_fast8_t hi, uint_fast8_t lo)
{
	LS020_CMND();		// Индикатор в режим приема команд

	spi_select(targetlcd, LS020_SPIMODE);
	spi_progval8_p1(targetlcd, hi);
	spi_progval8_p2(targetlcd, lo);
	spi_complete(targetlcd);
	spi_unselect(targetlcd);

}

// в режим передачи данных переводим сразу по окончании команд.
static void ls020_put_char_begin(void)
{
	LS020_DATA();		// Индикатор в режим приема данных
	spi_select(targetlcd, LS020_SPIMODE);
}

static void ls020_put_char_end(void)
{
	spi_unselect(targetlcd);
}

static uint_fast8_t color, bkcolor;

static void ls020_setcolor(uint_fast8_t acolor, uint_fast8_t abkcolor)
{
	color = acolor;
	bkcolor = abkcolor;
}

static void ls020_pixel_p1(
	uint_fast8_t fg
	)
{
	spi_progval8_p1(targetlcd, fg ? color : bkcolor);
}

static void ls020_pixel_p2(
	uint_fast8_t fg
	)
{
	spi_progval8_p2(targetlcd, fg ? color : bkcolor);
}

static void ls020_pixel_p3(
	uint_fast8_t fg
	)
{
	spi_progval8_p2(targetlcd, fg ? color : bkcolor);
	spi_complete(targetlcd);
}


static void 
//NOINLINEAT
ls020_pix8(
	uint_fast8_t v
	)
{
	ls020_pixel_p1(v & 0x01);
	ls020_pixel_p2(v & 0x02);
	ls020_pixel_p2(v & 0x04);
	ls020_pixel_p2(v & 0x08);
	ls020_pixel_p2(v & 0x10);
	ls020_pixel_p2(v & 0x20);
	ls020_pixel_p2(v & 0x40);
	ls020_pixel_p3(v & 0x80);
}



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

// Вызов этой функции только внутри display_wrdata_begin() и 	display_wrdata_end();
static void ls020_put_char_small(char cc)
{
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_smallfont [0] / sizeof ls020_smallfont [0][0]) };
	//enum { NCOLS = (sizeof uc1608_smallfont[0][0] / sizeof uc1608_smallfont[0][0][0]) };
	const FLASHMEM uint8_t * const p = & ls020_smallfont [c][0];
	uint_fast8_t i;
	for (i = 0; i < NBYTES; ++ i)
	{
		ls020_pix8(p [i]);
	}
}


// Вызов этой функции только внутри display_wrdata_begin() и 	display_wrdata_end();

static void ls020_put_char_big(char cc)
{
	enum { NBV = (BIGCHARH / 8) }; // сколько байтов в одной вертикали
	uint_fast8_t i = NBV * ((cc == '.' || cc == '#') ? 12 : 0);	// начальная колонка знакогенератора, откуда начинать.
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_bigfont [c] / sizeof ls020_bigfont [0][0]) };
	const FLASHMEM uint8_t * const p  = & ls020_bigfont [c][0];

	for (; i < NBYTES; ++ i)
	{
		ls020_pix8(p [i]);
	}
}

// Вызов этой функции только внутри display_wrdata_begin() и 	display_wrdata_end();

static void ls020_put_char_half(char cc)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_halffont [0] / sizeof ls020_halffont [0][0]) };
	const FLASHMEM uint8_t * const p = & ls020_halffont [c][0];
	for (; i < NBYTES; ++ i)
	{
		ls020_pix8(p [i]);
	}
}


/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */

static void ls020_bar_column(uint_fast8_t pattern)
{
	ls020_pix8(pattern);
}


static void ls020_set_contrast(uint_fast8_t v) //display contrast
{

  ls020_wrcmd16(0xef, 0x90);	// set page 0x90
  ls020_wrcmd16(0x24, v);
  ls020_wrcmd16(0x00, 0x00);
}

#define ASX 0x12
#define ASY 0x13
#define APX 0x14
#define AEX 0x15
#define AEY 0x16
#define APY 0x17

static uint_fast8_t ls020_windowx;
static uint_fast8_t ls020_windowy;

static void
ls020_set_windowh(uint_fast8_t height)
{
	const uint_fast8_t x1 = ls020_windowx;
	const uint_fast8_t y1 = ls020_windowy;

#if	LCDMODE_LS020_TOPDOWN
	// Перевёрнутое изображение
	// 180 degree rotation:  AEx < ASx
	ls020_wrcmd16(0xEF, 0x80); 	// Set page 0x80

	ls020_wrcmd16(ASX, y1); // window bottom
	ls020_wrcmd16(AEX, y1 - (height - 1)); 	// window top

	ls020_wrcmd16(ASY, x1); 	// window left
	// перенесено в инициализацию - постоянный параметр
	//ls020_wrcmd16(AEY, DIM_X - 1);	// window right


#else /* LCDMODE_LS020_TOPDOWN */
	
	ls020_wrcmd16(0xEF, 0x80); 	// Set page 0x80

	// прямое изображение
	// x-axis line symmetry (mirror image): AEx > ASx
	ls020_wrcmd16(ASX, y1); // window top
	ls020_wrcmd16(AEX, y1 + (height - 1)); 	// window bottom

	ls020_wrcmd16(ASY, x1); 	// window left
	// перенесено в инициализацию - постоянный параметр
	//ls020_wrcmd16(AEY, 0);			// window right
#endif /* LCDMODE_LS020_TOPDOWN */
}

/*
 Функция установки курсора в позицию x,y
 X - координата по горизонтали в пределах 0-175,
 Y - координата по вертикали (строка) в пределах 8 бит
*/
static void ls020_set_addr_column(uint_fast8_t x1, uint_fast8_t y1)
{
	ls020_windowx = x1;
	ls020_windowy = y1;
}


static void ls020_clear(COLORMAIN_T bg)
{
	unsigned i;

	ls020_setcolor(COLOR_WHITE, bg);
	display_gotoxy(0, 0);
	ls020_set_windowh(DIM_Y);

	ls020_put_char_begin();
	// ускоренный вариант
	spi_progval8_p1(targetlcd, bg);    // Заливка цветом фона
	for (i = 0; i < (DIM_X * DIM_Y) - 1; ++ i)
	{
		spi_progval8_p2(targetlcd, bg);    // Заливка цветом фона
	}
	spi_complete(targetlcd);
	ls020_put_char_end();
}

#if 1
// процедура подготовки дисплея к выключению
static void ls020_poweroff(void)
{
  ls020_wrcmd16(0xEF, 0x00); ls020_wrcmd16(0x7E, 0x04); ls020_wrcmd16(0xEF, 0xB0); ls020_wrcmd16(0x5A, 0x48); 
  ls020_wrcmd16(0xEF, 0x00); ls020_wrcmd16(0x7F, 0x01); ls020_wrcmd16(0xEF, 0xB0); ls020_wrcmd16(0x64, 0xFF);
  ls020_wrcmd16(0x65, 0x00); ls020_wrcmd16(0xEF, 0x00); ls020_wrcmd16(0x7F, 0x01); ls020_wrcmd16(0xE2, 0x62);
  ls020_wrcmd16(0xE2, 0x02); ls020_wrcmd16(0xEF, 0xB0); ls020_wrcmd16(0xBC, 0x02); ls020_wrcmd16(0xEF, 0x00);
  ls020_wrcmd16(0x7F, 0x01); ls020_wrcmd16(0xE2, 0x00); ls020_wrcmd16(0x80, 0x00); ls020_wrcmd16(0xE2, 0x04);
  ls020_wrcmd16(0xE2, 0x00); ls020_wrcmd16(0xE1, 0x00); ls020_wrcmd16(0xEF, 0xB0); ls020_wrcmd16(0xBC, 0x00);
  ls020_wrcmd16(0xEF, 0x00); ls020_wrcmd16(0x7F, 0x01);
}
#endif


static uint_fast8_t vbias = DEFAULT_LCD_CONTRAST;

void display_set_contrast(uint_fast8_t v)
{
	if (v != vbias)
	{
		vbias = v;
		ls020_set_contrast(vbias);
	}
}

static void ls020_initialize(void)
{
	static const FLASHMEM uint8_t init_array_0[] [2] =
	{
		{	0xEF, 0x00,	},  	// page 0x00
		{	0xEE, 0x04,	}, 
		{	0x1B, 0x04,	}, 

		{	0xFE, 0xFE,	},
		{	0xFE, 0xFE,	}, 

		{	0xEF, 0x90,	},  	// page 0x90
		{	0x4A, 0x04,	}, 
		{	0x7F, 0x3F,	}, 
		{	0xEE, 0x04,	}, 
		{	0x43, 0x06,	},
	};

	static const FLASHMEM uint8_t init_array_1[] [2] =
	{
		{	0xEF, 0x00,	},  	// page 0x00
		{	0xEE, 0x0C,	}, 

		{	0xEF, 0x90,	},  	// page 0x90
		{	0x00, 0x80,	}, 		// WTH bit????

		{	0xEF, 0xB0,	},  	// page 0xB0
		{	0x49, 0x02,	}, 		// positive mode 0x02, negative mode - 0x03

		{	0xEF, 0x00,	},  	// page 0x00

		{	0x7F, 0x01,	},
		{	0xE1, 0x81,	}, 

		{	0xE2, 0x02,	}, 
		{	0xE2, 0x76,	}, 

		{	0xE1, 0x83,	},		// it is not a DIM_Y - 1 value
		//{	0x80, 0x01,	}, 

		//{	0xEF, 0x90,	},  	// page 0x90
		//{	0x00, 0x00,	},
		//{	0xE8, 0x00,	},		// Set 8-bit format (RRRGGGBB)
		//{	0x00, 0x00,	},
	};


	ls020_wrcmd16(0xFD, 0xFD);
	ls020_wrcmd16(0xFD, 0xFD);

	local_delay_ms(100);	// delay 68ms

	uint_fast8_t i;
	for (i=0; i < (sizeof init_array_0 / sizeof init_array_0 [0]); i ++)  
	{ 
		ls020_wrcmd16(
			init_array_0[i] [0],
			init_array_0[i] [1]
				); 
	}
	local_delay_ms(7);	// delay 10ms

	for (i=0; i< (sizeof init_array_1 / sizeof init_array_1 [0]); i ++)  
	{ 
		ls020_wrcmd16(
			init_array_1[i] [0],
			init_array_1[i] [1]
				); 
	}

	local_delay_ms(100);	// delay 10ms

	ls020_wrcmd16(0x80, 0x01);
	ls020_wrcmd16(0xEF, 0x90);
	ls020_wrcmd16(0xE8, 0x00);		// RRRGGGBB
	ls020_wrcmd16(0x00, 0x00);

	/*
		Взято отсюда: http://juras.yourbb.be/viewtopic.php?f=2&t=24&start=0

		ls020_wrcmd16(0xE800); // Set 8-bit host-mode, color format: RRRGGGBB


		Here is a list of possible host-modes I have found:
		24-bit formats:
		0x10: xxB5B4-B3B2B1B0, R3R2R1R0-G5G4G3G2, xxxx-xxR5R4.
		0x50: xxR5R4-R3R2R1R0, B3B2B1B0-G5G4G3G2, xxxx-xxB5B4.
		0x20: xxB5B4-B3B2B1B0, xxG5G4-G3G2G1G0, xxR5R4-R3R2R1R0.
		0x60: xxR5R4-R3R2R1R0, xxG5G4-G3G2G1G0, xxB5B4-B3B2B1B0.
		0x30: R5R4R3R2-R1R0xx, G5G4G3G2-G1G0xx, B5B4B3B2-B1B0xx.
		0x70: B5B4B3B2-B1B0xx, G5G4G3G2-G1G0xx, R5R4R3R2-R1R0xx.

		16-bit formats:
		0x08: G2G1G0B5-B4B3B2B1, R5R4R3R2-R1G5G4G3
		0x48: G2G1G0R5-R4R3R2R1, B5B4B3B2-B1G5G4G3
		0x0C: R5R4R3R2-R1G5G4G3, G2G1G0B5-B4B3B2B1

		8-bit formats:
		0x00: RRRGGGBB
		0x40: BBGGGRRR
		
	*/

	/* установить направление заполнения в окне и постоянные параметры */
#if	LCDMODE_LS020_TOPDOWN

	// Перевёрнутое изображение
	ls020_wrcmd16(0xEF, 0x80); 	// Set page 0x80
	//ls020_wrcmd16(0x18, 0x00); 	// normal mode:  AEx > ASx
	ls020_wrcmd16(0x18, 0x01); 	// y-axis symmetry (mirror image): AEx < ASx 
	// перенесено в инициализацию - постоянный параметр
	ls020_wrcmd16(AEY, DIM_X - 1);	// window right

#else /* LCDMODE_LS020_TOPDOWN */

	// прямое изображение
	ls020_wrcmd16(0xEF, 0x80); 	// Set page 0x80
	//ls020_wrcmd16(0x18, 0x01); 	// y-axis symmetry (mirror image): AEx < ASx 
	ls020_wrcmd16(0x18, 0x02); 	// x-axis line symmetry (mirror image): AEx > ASx
	// перенесено в инициализацию - постоянный параметр
	ls020_wrcmd16(AEY, 0);			// window right

#endif /* LCDMODE_LS020_TOPDOWN */
	ls020_set_contrast(vbias);
}

/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{
	
	ls020_initialize();
}

void 
display_clear(void)
{
	const COLORMAIN_T bg = display_getbgcolor();

	ls020_clear(bg);
}

void
//NOINLINEAT
display_setcolors(COLORMAIN_T fg, COLORMAIN_T bg)
{
	ls020_setcolor(fg, bg);
}

void display_setcolors3(COLORMAIN_T fg, COLORMAIN_T bg, COLORMAIN_T fgbg)
{
	display_setcolors(fg, bg);
}

void
display_wrdata_begin(void)
{
	ls020_set_windowh(SMALLCHARH);
	ls020_put_char_begin();
}

void
display_wrdata_end(void)
{
	ls020_put_char_end();
}

void
display_wrdatabar_begin(void)
{
	ls020_set_windowh(CHAR_H * 1);
	ls020_put_char_begin();
}

void
display_wrdatabar_end(void)
{
	ls020_put_char_end();
}


void
display_wrdatabig_begin(void)
{
	ls020_set_windowh(BIGCHARH);	// same as HALFCHARH
	ls020_put_char_begin();
}


void
display_wrdatabig_end(void)
{
	ls020_put_char_end();
}

/* отображение одной вертикальной полосы на графическом индикаторе */
/* старшие биты соответствуют верхним пикселям изображения */
/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
void 
display_barcolumn(uint_fast8_t pattern)
{
	ls020_bar_column(pattern);
}

void
display_put_char_big(uint_fast8_t c, uint_fast8_t lowhalf)
{
	ls020_put_char_big(c);
}

void
display_put_char_half(uint_fast8_t c, uint_fast8_t lowhalf)
{
	ls020_put_char_half(c);
}


// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
void
display_put_char_small(uint_fast8_t c, uint_fast8_t lowhalf)
{
	ls020_put_char_small(c);
}

	//uint8_t x = h * CHAR_W;
	//uint8_t y = DIM_Y - 5 - (v * CHAR_H);

void
display_gotoxy(uint_fast8_t x, uint_fast8_t y)
{
#if	LCDMODE_LS020_TOPDOWN
	// Перевёрнутое изображение
	ls020_set_addr_column(x * CHAR_W, (DIM_Y - 1) - y * CHAR_H); // Rotate 180 degrees
#else /* LCDMODE_LS020_TOPDOWN */
	// прямое изображение
	ls020_set_addr_column((DIM_X - 1) - (x * CHAR_W), y * CHAR_H); // Rotate 180 degrees
#endif /* LCDMODE_LS020_TOPDOWN */
}
// Координаты в пикселях
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
#if	LCDMODE_LS020_TOPDOWN
	// Перевёрнутое изображение
	ls020_set_addr_column(x, (DIM_Y - 1) - y); // Rotate 180 degrees
#else /* LCDMODE_LS020_TOPDOWN */
	// прямое изображение
	ls020_set_addr_column((DIM_X - 1) - (x), y); // Rotate 180 degrees
#endif /* LCDMODE_LS020_TOPDOWN */
}

void display_plotstart(
	uint_fast16_t dy	// Высота окна в пикселях
	)
{
	ls020_set_windowh(dy);
	ls020_put_char_begin();
}

void display_plotstop(void)
{
	ls020_put_char_end();
}

void display_plot(
	const PACKEDCOLORMAIN_T * buffer, 
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
	ls020_reset();
}


/* Разряжаем конденсаторы питания */
void display_discharge(void)
{
	ls020_poweroff();
}

#endif /* LCDMODE_LS020 */
