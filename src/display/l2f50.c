/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Siemens S65 Display Control
/* Индикатор 176*132 с контроллером Epson L2F50126 */

#include "hardware.h"
#include "board.h"

#if LCDMODE_L2F50

	#if	LCDMODE_L2F50_TOPDOWN
		#error LCDMODE_L2F50_TOPDOWN not implemented
	#endif

#include "display.h"
#include <stdint.h>

#include "spi.h"
#include "l2f50.h"

/* растровые шрифты */
#include "fontmaps.h"



#define DATCTL 0xBC         // Data Control (data handling in RAM)
#define DISCTL 0xCA         // Display Control
#define GCP64 0xCB          // pulse set for 64 gray scale
#define GCP16 0xCC          // pulse set for 16 gray scale
#define OSSEL 0xD0          // Oscillator select
#define GSSET 0xCD          // set for gray scales
#define ASCSET 0xAA         // aerea scroll setting
#define SCSTART 0xAB        // scroll start setting
#define DISON 0xAF          // Display ON (no parameter)
#define DISOFF 0xAE         // Display OFF (no parameter)
#define DISINV 0xA7         // Display Invert (no parameter)
#define DISNOR 0xA6         // Display Normal (no parameter)
#define SLPIN  0x95         // Display Sleep (no parameter)
#define SLPOUT 0x94         // Display out of sleep (no parameter)
#define RAMWR 0x5C          // Display Memory write
#define PTLIN 0xA8          // partial screen write

#define SD_CSET 0x15        // column address setting
#define SD_PSET 0x75        // page address setting



#define L2F50_CMND() do { board_lcd_rs(0); } while (0)	/* RS: LOW: command data */
#define L2F50_DATA() do { board_lcd_rs(1); } while (0)	/* RS: HIGH: display data or parameter */

#define L2F50_SPIMODE SPIC_MODE3

// Условие использования оптимизированных функций обращения к SPI
#define WITHSPIEXT16 (WITHSPIHW && WITHSPI16BIT)

static void
l2f50_reset(void)
{
	L2F50_DATA(); //HIGH   // RS is set to high

	local_delay_ms(5);		// delay 5 mS
	board_lcd_reset(0); 	// Pull RST pin down
	board_update();
	local_delay_ms(10);		// delay 10 mS (for LPH88, 5 for LS020).
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(5);		// delay 5 mS
}

//  Запись данных без управлениея CS
static void 
l2f50_writeData(uint_fast8_t b)
{
	spi_progval8_p1(targetlcd, b);
	spi_progval8_p2(targetlcd, 0x00);

	spi_complete(targetlcd);
}

static void 
l2f50_writeCmd(uint_fast8_t b)
{
	L2F50_CMND();		// Индикатор в режим приема команд

	spi_progval8_p1(targetlcd, b);
	spi_progval8_p2(targetlcd, 0x00);

	spi_complete(targetlcd);

	L2F50_DATA(); //HIGH   // RS is set to high
}



#if WITHSPIEXT16
	static COLORMAIN_T fgcolor, bkcolor;
#else /* WITHSPIEXT16 */
	static struct { uint_fast8_t first, second; } fgcolor, bkcolor;
#endif

static void l2f50_setcolor(COLORMAIN_T acolor, COLORMAIN_T abkcolor)
{
#if WITHSPIEXT16
	fgcolor = acolor;
	bkcolor = abkcolor;
#else /* WITHSPIEXT16 */
	fgcolor.first = acolor >> 8;
	fgcolor.second = acolor >> 0;
	bkcolor.first = abkcolor >> 8;
	bkcolor.second = abkcolor >> 0;
#endif /* WITHSPIEXT16 */
}

// 176*132
// нормально смотрим - выводы дисплея слева
// l2f50_windowy = (y * CHAR_H);
// l2f50_windowx = (DIM_X - 1 - x * CHAR_W);

static uint_fast8_t l2f50_windowx;
static uint_fast8_t l2f50_windowy;

static void
l2f50_set_windowh(uint_fast8_t height)
{
	const uint_fast8_t x0 = l2f50_windowx;
	const uint_fast8_t y0 = l2f50_windowy;
	const uint_fast8_t x1 = DIM_X - 1;
	const uint_fast8_t y1 = y0 + height - 1;

	spi_select(targetlcd, L2F50_SPIMODE);

	#if	LCDMODE_L2F50_TOPDOWN

		l2f50_writeCmd(SD_CSET);                    //column address set 
		l2f50_writeData(0x08 + y0);                //start column (X1,Y1) is the upper left corner,
		l2f50_writeData(0x01);                   //start column
		l2f50_writeData(0x08 + y1);                //end column (бывает 139 = 8 + 131)
		l2f50_writeData(0x01);                   //end column

		l2f50_writeCmd(SD_PSET);                    //page address set 
		l2f50_writeData(x0);                     //start page, начало по оси х
		l2f50_writeData(0);                     //end page, конец по оси х (бывает 175)
	
	#else /* LCDMODE_L2F50_TOPDOWN */

		l2f50_writeCmd(SD_CSET);                    //column address set 
		l2f50_writeData(0x08 + y0);                //start column (X1,Y1) is the upper left corner,
		l2f50_writeData(0x01);                   //start column
		l2f50_writeData(0x08 + y1);                //end column (бывает 139 = 8 + 131)
		l2f50_writeData(0x01);                   //end column

		l2f50_writeCmd(SD_PSET);                    //page address set 
		l2f50_writeData(x0);                     //start page, начало по оси х
		l2f50_writeData(x1);                     //end page, конец по оси х (бывает 175)
	
	#endif /* LCDMODE_L2F50_TOPDOWN */
	spi_unselect(targetlcd);
}

// в режим передачи данных переводим сразу по окончании команд.
static void l2f50_put_char_begin(void)
{
#if WITHSPIEXT16

	hardware_spi_connect_b16(SPIC_SPEEDFAST, L2F50_SPIMODE);	/* Enable SPI */
	prog_select(targetlcd);	/* start sending data to target chip */
	L2F50_CMND();		// Индикатор в режим приема команд
	hardware_spi_b16_p1(RAMWR << 8);
	hardware_spi_complete_b16();
	L2F50_DATA(); //HIGH   // RS is set to high

#else /* WITHSPIEXT16 */

	spi_select(targetlcd, L2F50_SPIMODE);

	l2f50_writeCmd(RAMWR);// Display Memory write

#endif /* WITHSPIEXT16 */
}

static void l2f50_put_char_end(void)
{
#if WITHSPIEXT16

	prog_unselect(targetlcd);			/* Disable SPI */
	hardware_spi_disconnect();

#else /* WITHSPIEXT16 */

	spi_unselect(targetlcd);		/* Disable SPI */

#endif /* WITHSPIEXT16 */
}

static void l2f50_clear(COLORMAIN_T bg)
{
	unsigned i;

	l2f50_setcolor(COLOR_WHITE, bg);
	display_gotoxy(0, 0);
	l2f50_set_windowh(DIM_Y);

	l2f50_put_char_begin();
	spi_progval8_p1(targetlcd, bkcolor.first);    // Заливка цветом фона
	spi_progval8_p2(targetlcd, bkcolor.second);    // Заливка цветом фона
	for (i = 0; i < (DIM_X * DIM_Y) - 1; ++ i)
	{
		spi_progval8_p2(targetlcd, bkcolor.first);    // Заливка цветом фона
		spi_progval8_p2(targetlcd, bkcolor.second);    // Заливка цветом фона
	}
	spi_complete(targetlcd);

	l2f50_put_char_end();
}

static void l2f50_pixel_p1(uint_fast8_t fg)
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

static void l2f50_pixel_p2(uint_fast8_t fg)
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

static void l2f50_pixel_p3(uint_fast8_t fg)
{
#if WITHSPIEXT16

	hardware_spi_b16_p2(fg ? fgcolor : bkcolor);
	spi_complete(targetlcd);

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


static void l2f50_pix8(uint_fast8_t v)
{
	l2f50_pixel_p1(v & 0x01);
	l2f50_pixel_p2(v & 0x02);
	l2f50_pixel_p2(v & 0x04);
	l2f50_pixel_p2(v & 0x08);
	l2f50_pixel_p2(v & 0x10);
	l2f50_pixel_p2(v & 0x20);
	l2f50_pixel_p2(v & 0x40);
	l2f50_pixel_p3(v & 0x80);
}

// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
static void ls020_put_char_small(char cc)
{
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_smallfont [0] / sizeof ls020_smallfont [0][0]) };
	//enum { NCOLS = (sizeof uc1608_smallfont[0][0] / sizeof uc1608_smallfont[0][0][0]) };
	const FLASHMEM uint8_t * const p = & ls020_smallfont [c][0];
	uint_fast8_t i;
	for (i = 0; i < NBYTES; ++ i)
	{
		l2f50_pix8(p [i]);
	}
}


// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();

static void ls020_put_char_big(char cc)
{
	// '#' - узкий пробел
	enum { NBV = (BIGCHARH / 8) }; // сколько байтов в одной вертикали
	uint_fast8_t i = NBV * ((cc == '.' || cc == '#') ? 12 : 0);	// начальная колонка знакогенератора, откуда начинать.
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_bigfont [c] / sizeof ls020_bigfont [0][0]) };
	const FLASHMEM uint8_t * const p  = & ls020_bigfont [c][0];

	for (; i < NBYTES; ++ i)
	{
		l2f50_pix8(p [i]);
	}
}

// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();

static void ls020_put_char_half(char cc)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_halffont [0] / sizeof ls020_halffont [0][0]) };
	const FLASHMEM uint8_t * const p = & ls020_halffont [c][0];
	for (; i < NBYTES; ++ i)
	{
		l2f50_pix8(p [i]);
	}
}


/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */

static void ls020_bar_column(uint_fast8_t pattern)
{
	l2f50_pix8(pattern);
}

static void l2f50_initialize(void)
{
	/*
	  Init the Siemens S65 Display with EPSON controller
	  (c) Christian Kranz, 10/2005
	*/
	static const FLASHMEM uint8_t disctl [] =
	{
		0x4C,0x01,0x53,0x00,0x02,0xB4,0xB0,0x02,
		0x00,
	};
	static const FLASHMEM uint8_t gcp64_0 [] =
	{
		0x11,0x27,0x3C,0x4C,0x5D,0x6C,0x78,0x84,
		0x90,0x99,0xA2,0xAA,0xB2,0xBA,0xC0,0xC7,
		0xCC,0xD2,0xD7,0xDC,0xE0,0xE4,0xE8,0xED,
		0xF0,0xF4,0xF7,0xFB,0xFE,
	};
	static const FLASHMEM uint8_t gcp64_1 [] =
	{
		0x01,0x03,0x06,0x09,0x0B,0x0E,0x10,0x13,
		0x15,0x17,0x19,0x1C,0x1E,0x20,0x22,0x24,
		0x26,0x28,0x2A,0x2C,0x2D,0x2F,0x31,0x33,
		0x35,0x37,0x39,0x3B,0x3D,0x3F,0x42,0x44,
		0x47,0x5E,
	};
	static const FLASHMEM uint8_t gcp16 [] =
	{
		0x13,0x23,0x2D,0x33,0x38,0x3C,0x40,0x43,
		0x46,0x48,0x4A,0x4C,0x4E,0x50,0x64,
	};

	uint_fast8_t i;

	spi_select(targetlcd, L2F50_SPIMODE);

	//l2f50_writeCmd(0xAE); //display off

	l2f50_writeCmd(DATCTL);  //DATCTL Data Control (data handling in RAM)
	#if	LCDMODE_L2F50_TOPDOWN
		l2f50_writeData(0x2A); // RGB565 mode
	#else /* LCDMODE_L2F50_TOPDOWN */
		l2f50_writeData(0x2B); // RGB565 mode
	#endif /* LCDMODE_L2F50_TOPDOWN */

	spi_unselect(targetlcd);


	//565 mode, 0x2B=MIRROR
	// 0x0A=666mode, 0x3A=444mode

	//S65_CS_DISABLE(); // high //HIGH
	//asm volatile("nop");
	//S65_CS_ENABLE(); // low //LOW

	spi_select(targetlcd, L2F50_SPIMODE);

	l2f50_writeCmd(DISCTL);  //display control 
	for (i = 0; i < (sizeof disctl / sizeof disctl [0]); ++ i)
	{
		l2f50_writeData(disctl [i]);
	}

	//----------------------------

	l2f50_writeCmd(GCP64);// 0xCB pulse set for 64 gray scale
	for (i = 0; i < (sizeof gcp64_0 / sizeof gcp64_0 [0]); ++ i)
	{
		l2f50_writeData(gcp64_0 [i]);
		l2f50_writeData(0x00);
	}
	for (i = 0; i < (sizeof gcp64_1 / sizeof gcp64_1 [0]); ++ i)
	{
		l2f50_writeData(gcp64_1 [i]);
		l2f50_writeData(0x01);
	}

	l2f50_writeCmd(GCP16);//0xCC  pulse set for 16 gray scale
	for (i = 0; i < (sizeof gcp16 / sizeof gcp16 [0]); ++ i)
	{
		l2f50_writeData(gcp16 [i]);
	}

	l2f50_writeCmd(GSSET); //0xCD   set for gray scales
	l2f50_writeData(0x00);
	//----------------------------

	//l2f50_writeCmd(OSSEL);     // Oscillator select
	//l2f50_writeData(0x00);

	l2f50_writeCmd(SLPOUT); //sleep out

	local_delay_ms(7);

	//s65_setArea(0, 0, S65_WIDTH, S65_HEIGHT);
	//  l2f50_writeCmd(0x15);                    //column address set 
	//  l2f50_writeData(0x08);                //start column
	///  l2f50_writeData(0x01);                   //start column
	//  l2f50_writeData(0x8B);                //end column
	//  l2f50_writeData(0x01);                   //end column                  

	//  l2f50_writeCmd(0x75);   // page address setting
	//  l2f50_writeData(0x00);        //page address set 
	//  l2f50_writeData(0x8F);    //end page

	//  l2f50_writeCmd(0xAA);  // aerea scroll setting
	//  l2f50_writeData(0x00); //
	//  l2f50_writeData(0xAF); //
	//  l2f50_writeData(0xAF); //
	//  l2f50_writeData(0x03); //

	//  l2f50_writeCmd(0xAB); // scroll start setting
	//  l2f50_writeData(0x00); //


	//L2F50_CMND(); // low //LOW
	l2f50_writeCmd(DISON); //display on

	spi_unselect(targetlcd);

	//S65_CS_DISABLE(); // high //HIGH deselect display 

}


/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{
	
	l2f50_initialize();
}

void display_set_contrast(uint_fast8_t v)
{
	//ls020_set_contrast(v);
}

void 
display_clear(void)
{
	const COLORMAIN_T bg = display_getbgcolor();

	l2f50_clear(bg);
}

void
//NOINLINEAT
colmain_setcolors(COLORMAIN_T fg, COLORMAIN_T bg)
{
	l2f50_setcolor(fg, bg);
}

void colmain_setcolors3(COLORMAIN_T fg, COLORMAIN_T bg, COLORMAIN_T fgbg)
{
	colmain_setcolors(fg, bg);
}

void
display_wrdata_begin(void)
{
	l2f50_set_windowh(SMALLCHARH);
	l2f50_put_char_begin();
}

void
display_wrdata_end(void)
{
	l2f50_put_char_end();
}

void
display_wrdatabar_begin(void)
{
	l2f50_set_windowh(CHAR_H * 1);
	l2f50_put_char_begin();
}

void
display_wrdatabar_end(void)
{
	l2f50_put_char_end();
}


void
display_wrdatabig_begin(void)
{
	l2f50_set_windowh(BIGCHARH);	// same as HALFCHARH
	l2f50_put_char_begin();
}


void
display_wrdatabig_end(void)
{
	l2f50_put_char_end();
}

/* отображение одной вертикальной полосы на графическом индикаторе */
/* старшие биты соответствуют верхним пикселям изображения */
/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
void 
display_barcolumn(uint_fast8_t pattern)
{
	ls020_bar_column(pattern);
}

/* вызывается между вызовами display_wrdatabig_begin() и display_wrdatabig_end() */
void
display_put_char_big(uint_fast8_t c, uint_fast8_t lowhalf)
{
	ls020_put_char_big(c);
}

/* вызывается между вызовами display_wrdatabig_begin() и display_wrdatabig_end() */
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

void
display_gotoxy(uint_fast8_t x, uint_fast8_t y)
{
	#if	LCDMODE_L2F50_TOPDOWN
		// нормально смотрим - выводы дисплея справа
		l2f50_windowy = (DIM_Y - 1) - CHAR_H * y;
		l2f50_windowx = (DIM_X - 1 - x * CHAR_W);
	#else /* LCDMODE_L2F50_TOPDOWN */
		// нормально смотрим - выводы дисплея слева
		l2f50_windowy = (y * CHAR_H);
		l2f50_windowx = (x * CHAR_W);
	#endif /* LCDMODE_L2F50_TOPDOWN */
}

// Координаты в пикселях
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
	#if	LCDMODE_L2F50_TOPDOWN
		// нормально смотрим - выводы дисплея справа
		l2f50_windowy = (DIM_Y - 1) - y;
		l2f50_windowx = (DIM_X - 1 - x);
	#else /* LCDMODE_L2F50_TOPDOWN */
		// нормально смотрим - выводы дисплея слева
		l2f50_windowy = (y);
		l2f50_windowx = (x);
	#endif /* LCDMODE_L2F50_TOPDOWN */
}

void display_plotstart(
	uint_fast16_t dy	// Высота окна в пикселях
	)
{
	l2f50_set_windowh(dy);
	l2f50_put_char_begin();
}

void display_plotstop(void)
{
	l2f50_put_char_end();
}

void display_plot(
	const PACKEDCOLORMAIN_T * buffer, 
	uint_fast16_t dx,	// Размеры окна в пикселях
	uint_fast16_t dy,
	uint_fast16_t xpix,	// начало области рисования
	uint_fast16_t ypix
	)
{

}


/* аппаратный сброс дисплея - перед инициализаций */
/* вызывается при разрешённых прерываниях. */
void
display_reset(void)
{
	l2f50_reset();
}

/* Разряжаем конденсаторы питания */
void display_discharge(void)
{
}

#endif /* LCDMODE_LPH88 */
