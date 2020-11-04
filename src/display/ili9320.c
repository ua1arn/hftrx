/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// TFT панель 320 * 240 ADI_3.2_AM-240320D4TOQW-T00H(R)
// Только работает медленно.


#include "hardware.h"
#include "board.h"
#include "display.h"
#include <stdint.h>

#include "../inc/spi.h"
#include "formats.h"	// for debug prints

#if LCDMODE_ILI9320

#include "ili9320.h"

#if DSTYLE_G_X320_Y240
	// Для дисплеев 320 * 240

	#include "./fonts/ILI9341_font_small.h"
	#include "./fonts/ILI9341_font_half.h"
	#include "./fonts/ILI9341_font_big.h"

	#define	ILI9320_smallfont	ILI9341_smallfont
	#define	ILI9320_halffont	ILI9341_halffont
	#define	ILI9320_bigfont		ILI9341_bigfont

#else /* DSTYLE_G_X320_Y240 */

	#error Undefined display layout
	#include "./fonts/ILI9320_font_small.h"	// similar to ls020_small_font
	#include "./fonts/ILI9320_font_half.h"
	#include "./fonts/ILI9320_font_big.h"	// BIGCHARH 32

#endif /* DSTYLE_G_X320_Y240 */

#define ILI9320_SPIMODE		SPIC_MODE3



/*
	RS	R/W Function
	0	0		Set an index register
	0	1		Read a status
	1	0		Write a register or GRAM data
	1	1		Read a register or GRAM data

*/

/* SPI interface definitions */

#define ILI9320_SPI_IDCODE		(0x70)		// постоянная часть первого байта
#define ILI9320_SPI_ID(x)		((x) << 2)
#define ILI9320_SPI_READ		(0x01)		// WR = 1
#define ILI9320_SPI_WRITE		(0x00)		// WR = 0
#define ILI9320_SPI_DATA		(0x02)		// RS = 1
#define ILI9320_SPI_INDEX		(0x00)		// RS = 0


static void
LCD_CtrlWrite_ILI9320(
	uint_fast8_t addr,	
	uint_fast16_t data
	)
{
	const uint_fast8_t v1 = 
		ILI9320_SPI_WRITE	| 
		ILI9320_SPI_INDEX	|	// RS = 0
		ILI9320_SPI_ID(0)	|
		ILI9320_SPI_IDCODE;	//0x70		// постоянная часть первого байта

	const uint_fast8_t v2 = 
		ILI9320_SPI_WRITE	| 
		ILI9320_SPI_DATA	|	// RS = 1
		ILI9320_SPI_ID(0)	|
		ILI9320_SPI_IDCODE;	//0x70		// постоянная часть первого байта

	spi_select(targetlcd, ILI9320_SPIMODE);
	spi_progval8_p1(targetlcd, v1);
	spi_progval8_p2(targetlcd, 0x00);	// high byte of address
	spi_progval8_p2(targetlcd, addr >> 0);	// low byte of address
	spi_complete(targetlcd);
	spi_unselect(targetlcd);

	spi_select(targetlcd, ILI9320_SPIMODE);
	spi_progval8_p1(targetlcd, v2);
	spi_progval8_p2(targetlcd, data >> 8);
	spi_progval8_p2(targetlcd, data >> 0);
	spi_complete(targetlcd);
	spi_unselect(targetlcd);
}


static uint_fast16_t
LCD_Read_ILI9320(
	uint_fast16_t addr
	)
{
	uint_fast8_t r1, r2;

	enum { v1 = 
		ILI9320_SPI_WRITE	| 
		ILI9320_SPI_INDEX	|	// rs = 0
		ILI9320_SPI_ID(0)	|
		ILI9320_SPI_IDCODE };	//0x70		// постоянная часть первого байта

	enum { v2 = 
		ILI9320_SPI_READ	| 
		ILI9320_SPI_DATA	|	// rs = 1
		ILI9320_SPI_ID(0)	|
		ILI9320_SPI_IDCODE };	//0x70		// постоянная часть первого байта

	spi_select(targetlcd, ILI9320_SPIMODE);
	spi_progval8_p1(targetlcd, v1);
	spi_progval8_p2(targetlcd, addr >> 8);	// high byte of address
	spi_progval8_p2(targetlcd, addr >> 0);	// low byte of address
	spi_complete(targetlcd);
	spi_unselect(targetlcd);

	spi_select(targetlcd, ILI9320_SPIMODE);
	spi_progval8_p1(targetlcd, v2);
	spi_progval8_p2(targetlcd, 0);		// dummy
	spi_complete(targetlcd);
	r1 = spi_read_byte(targetlcd, 0xff);
	r2 = spi_read_byte(targetlcd, 0xff);
	spi_unselect(targetlcd);

	return r1 * 256 + r2;
}


// в режим передачи данных переводим сразу по окончании команд.
static void ili9320_put_char_begin(void)
{
	uint_fast16_t addr = 0x22;	
	const uint_fast8_t v1 = 
		ILI9320_SPI_WRITE	| 
		ILI9320_SPI_INDEX	|	// RS = 0
		ILI9320_SPI_ID(0)	|
		ILI9320_SPI_IDCODE;	//0x70		// постоянная часть первого байта

	const uint_fast8_t v2 = 
		ILI9320_SPI_WRITE	| 
		ILI9320_SPI_DATA	|	// RS = 1
		ILI9320_SPI_ID(0)	|
		ILI9320_SPI_IDCODE;	//0x70		// постоянная часть первого байта

	spi_select(targetlcd, ILI9320_SPIMODE);
	spi_progval8_p1(targetlcd, v1);
	spi_progval8_p2(targetlcd, 0x00);	// high byte of address
	spi_progval8_p2(targetlcd, addr >> 0);	// low byte of address
	spi_complete(targetlcd);
	spi_unselect(targetlcd);

	spi_select(targetlcd, ILI9320_SPIMODE);
	spi_progval8(targetlcd, v2);
}

static void ili9320_put_char_end(void)
{
	spi_unselect(targetlcd);
}



static void
ili9320_put_pixel_p1(	
	COLORMAIN_T color
	)
{
	spi_progval8_p1(targetlcd, color >> 8);	// смотреть бит TRI а регистре 03
	spi_progval8_p2(targetlcd, color >> 0);	// смотреть бит TRI а регистре 03
}
static void
ili9320_put_pixel_p2(	
	COLORMAIN_T color
	)
{
	spi_progval8_p2(targetlcd, color >> 8);	// смотреть бит TRI а регистре 03
	spi_progval8_p2(targetlcd, color >> 0);	// смотреть бит TRI а регистре 03
}
static void
ili9320_put_pixel_p3(	
	COLORMAIN_T color
	)
{
	spi_progval8_p2(targetlcd, color >> 8);	// смотреть бит TRI а регистре 03
	spi_progval8_p2(targetlcd, color >> 0);	// смотреть бит TRI а регистре 03
	spi_complete(targetlcd);
}


static COLORMAIN_T color, bkcolor;

static void ili9320_setcolor(COLORMAIN_T acolor, COLORMAIN_T abkcolor)
{
	color = acolor;
	bkcolor = abkcolor;
}

static void ili9320_pixel_p1(
	uint_fast8_t fg
	)
{
	const COLORMAIN_T cl = fg ? color : bkcolor;
	ili9320_put_pixel_p1(cl);
}

static void ili9320_pixel_p2(
	uint_fast8_t fg
	)
{
	const COLORMAIN_T cl = fg ? color : bkcolor;
	ili9320_put_pixel_p2(cl);
}

static void ili9320_pixel_p3(
	uint_fast8_t fg
	)
{
	const COLORMAIN_T cl = fg ? color : bkcolor;
	ili9320_put_pixel_p3(cl);
}

static void 
//NOINLINEAT
ili9320_pix8(
	uint_fast8_t v
	)
{
	ili9320_pixel_p1(v & 0x01);
	ili9320_pixel_p2(v & 0x02);
	ili9320_pixel_p2(v & 0x04);
	ili9320_pixel_p2(v & 0x08);
	ili9320_pixel_p2(v & 0x10);
	ili9320_pixel_p2(v & 0x20);
	ili9320_pixel_p2(v & 0x40);
	ili9320_pixel_p3(v & 0x80);
}

// Вызов этой функции только внутри display_wrdata_begin() и 	display_wrdata_end();
static void ili9320_put_char_fast(char cc, uint_fast8_t lowhalf)
{
	uint_fast8_t i = 0;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ILI9320_smallfont [0] / sizeof ILI9320_smallfont [0][0]) };
	const FLASHMEM uint8_t * const p = & ILI9320_smallfont [c][0];
	for (; i < NBYTES; ++ i)
	{
		ili9320_pix8(p [i]);
	}
}


// Вызов этой функции только внутри display_wrdata_begin() и 	display_wrdata_end();

static void ili9320_put_char_big(char cc, uint_fast8_t lowhalf)
{
	// '#' - узкий пробел
	enum { NBV = (BIGCHARH / 8) }; // сколько байтов в одной вертикали
	uint_fast8_t i = NBV * ((cc == '.' || cc == '#') ? 12 : 0);	// начальная колонка знакогенератора, откуда начинать.
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ILI9320_bigfont [0] / sizeof ILI9320_bigfont [0][0]) };
	const FLASHMEM uint8_t * const p = & ILI9320_bigfont [c][0];

	for (; i < NBYTES; ++ i)
	{
		ili9320_pix8(p [i]);
	}
}

// Вызов этой функции только внутри display_wrdata_begin() и 	display_wrdata_end();

static void ili9320_put_char_half(char cc, uint_fast8_t lowhalf)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ILI9320_halffont [0] / sizeof ILI9320_halffont [0][0]) };
	const FLASHMEM uint8_t * const p = & ILI9320_halffont [c][0];
	for (; i < NBYTES; ++ i)
	{
		ili9320_pix8(p [i]);
	}
}


/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
static void ili9320_bar_column(uint_fast8_t pattern)
{
	ili9320_pix8(pattern);
}

static uint_fast8_t saved_y;

static void ili9320_set_windowh(uint_fast8_t height)
{
#if LCDMODE_ILI9320_TOPDOWN
	/* перевёрнутое изображение */
	// открыть строку для записи текста
	LCD_CtrlWrite_ILI9320(0x0050, saved_y - (height - 1)); // Horizontal GRAM Start Address
	LCD_CtrlWrite_ILI9320(0x0051, saved_y /* + (height - 1) */ ); // Horizontal GRAM End Address
	LCD_CtrlWrite_ILI9320(0x0020, saved_y);
#else /* LCDMODE_ILI9320_TOPDOWN */
	/* нормальное изображение */
	// открыть строку для записи текста
	LCD_CtrlWrite_ILI9320(0x0050, saved_y); // Horizontal GRAM Start Address
	LCD_CtrlWrite_ILI9320(0x0051, saved_y + (height - 1)); // Horizontal GRAM End Address
	LCD_CtrlWrite_ILI9320(0x0020, saved_y);
#endif	/* LCDMODE_ILI9320_TOPDOWN */
}



/*
 Функция установки курсора в позицию x,y
 X - координата по горизонтали в пределах 0-175,
 Y - координата по вертикали (строка) в пределах 8 бит
*/
static void ili9320_set_graddr(uint_fast16_t x)
{
#if LCDMODE_ILI9320_TOPDOWN
	/* перевёрнутое изображение */
	LCD_CtrlWrite_ILI9320(0x0021, x);
#else /* LCDMODE_ILI9320_TOPDOWN */
	/* нормальное изображение */
	LCD_CtrlWrite_ILI9320(0x0021, x);
#endif	/* LCDMODE_ILI9320_TOPDOWN */
}


static void ili9320_clear(COLORMAIN_T bg)
{
	unsigned long i;

	colmain_setcolors(COLOR_WHITE, bg);
	display_gotoxy(0, 0);

	ili9320_set_windowh(DIM_Y);

	ili9320_put_char_begin();

	for (i = 0; i < ((unsigned long) DIM_Y * DIM_X) / 8; ++ i)
	{
		ili9320_pix8(0x00);
	}
	ili9320_put_char_end();

}

// скопировано из даташита.
// под разные виды панелей там различные инициализациолнные последовательностти.
// CPT QVGA 3.2" Panel
static
void ILI9320_CPT32_Initial(void)
{
	// VCI=2.8V

	//************* Start Initial Sequence **********//
	LCD_CtrlWrite_ILI9320(0x00E5, 0x8000); // Set the internal vcore voltage
	LCD_CtrlWrite_ILI9320(0x0000, 0x0001); // Start internal OSC.
	LCD_CtrlWrite_ILI9320(0x0001, 0x0100); // set SS and SM bit
	LCD_CtrlWrite_ILI9320(0x0002, 0x0700); // set 1 line inversion
#if LCDMODE_ILI9320_TOPDOWN
	/* перевёрнутое изображение */
	LCD_CtrlWrite_ILI9320(0x0003, 0x1020 /* 0x1030 */); // set GRAM write direction and BGR=1.
#else /* LCDMODE_ILI9320_TOPDOWN */
	/* нормальное изображение */
	LCD_CtrlWrite_ILI9320(0x0003, 0x1010 /* 0x1030 */); // set GRAM write direction and BGR=1.
#endif	/* LCDMODE_ILI9320_TOPDOWN */
	LCD_CtrlWrite_ILI9320(0x0004, 0x0000); // Resize register
	LCD_CtrlWrite_ILI9320(0x0008, 0x0202); // set the back porch and front porch
	LCD_CtrlWrite_ILI9320(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
	LCD_CtrlWrite_ILI9320(0x000A, 0x0000); // FMARK function
	LCD_CtrlWrite_ILI9320(0x000C, 0x0000); // RGB interface setting
	LCD_CtrlWrite_ILI9320(0x000D, 0x0000); // Frame marker Position
	LCD_CtrlWrite_ILI9320(0x000F, 0x0000); // RGB interface polarity
	//*************Power On sequence ****************//
	LCD_CtrlWrite_ILI9320(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
	LCD_CtrlWrite_ILI9320(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
	LCD_CtrlWrite_ILI9320(0x0012, 0x0000); // VREG1OUT voltage
	LCD_CtrlWrite_ILI9320(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
	local_delay_ms(200); // Dis-charge capacitor power voltage
	LCD_CtrlWrite_ILI9320(0x0010, 0x17B0); // SAP, BT[3:0], AP, DSTB, SLP, STB
	LCD_CtrlWrite_ILI9320(0x0011, 0x0147); // DC1[2:0], DC0[2:0], VC[2:0]
	local_delay_ms(50); // Delay 50ms
	LCD_CtrlWrite_ILI9320(0x0012, 0x013C); // VREG1OUT voltage
	local_delay_ms(50); // Delay 50ms
	LCD_CtrlWrite_ILI9320(0x0013, 0x0E00); // VDV[4:0] for VCOM amplitude
	LCD_CtrlWrite_ILI9320(0x0029, 0x0009); // VCM[4:0] for VCOMH
	local_delay_ms(50);
	//LCD_CtrlWrite_ILI9320(0x0020, 0x0000); // GRAM horizontal Address
	//LCD_CtrlWrite_ILI9320(0x0021, 0x0000); // GRAM Vertical Address
	// ----------- Adjust the Gamma Curve ----------//
	LCD_CtrlWrite_ILI9320(0x0030, 0x0207);
	LCD_CtrlWrite_ILI9320(0x0031, 0x0505);
	LCD_CtrlWrite_ILI9320(0x0032, 0x0102);
	LCD_CtrlWrite_ILI9320(0x0035, 0x0006);
	LCD_CtrlWrite_ILI9320(0x0036, 0x0606);
	LCD_CtrlWrite_ILI9320(0x0037, 0x0707);
	LCD_CtrlWrite_ILI9320(0x0038, 0x0506);
	LCD_CtrlWrite_ILI9320(0x0039, 0x0407);
	LCD_CtrlWrite_ILI9320(0x003C, 0x0106);
	LCD_CtrlWrite_ILI9320(0x003D, 0x0601);
	//------------------ Set GRAM area ---------------//
	LCD_CtrlWrite_ILI9320(0x0050, 0x0000); // Horizontal GRAM Start Address
	LCD_CtrlWrite_ILI9320(0x0051, 0x00EF); // Horizontal GRAM End Address
	LCD_CtrlWrite_ILI9320(0x0052, 0x0000); // Vertical GRAM Start Address
	LCD_CtrlWrite_ILI9320(0x0053, 0x013F); // Vertical GRAM End Address
	LCD_CtrlWrite_ILI9320(0x0060, 0x2700); // Gate Scan Line
	LCD_CtrlWrite_ILI9320(0x0061, 0x0001); // NDL,VLE, REV
	LCD_CtrlWrite_ILI9320(0x006A, 0x0000); // set scrolling line
	//-------------- Partial Display Control ---------//
	LCD_CtrlWrite_ILI9320(0x0080, 0x0000);
	LCD_CtrlWrite_ILI9320(0x0081, 0x0000);
	LCD_CtrlWrite_ILI9320(0x0082, 0x0000);
	LCD_CtrlWrite_ILI9320(0x0083, 0x0000);
	LCD_CtrlWrite_ILI9320(0x0084, 0x0000);
	LCD_CtrlWrite_ILI9320(0x0085, 0x0000);
	//-------------- Panel Control -------------------//
	LCD_CtrlWrite_ILI9320(0x0090, 0x0010);
	LCD_CtrlWrite_ILI9320(0x0092, 0x0000);
	LCD_CtrlWrite_ILI9320(0x0093, 0x0003);
	LCD_CtrlWrite_ILI9320(0x0095, 0x0110);
	LCD_CtrlWrite_ILI9320(0x0097, 0x0000);
	LCD_CtrlWrite_ILI9320(0x0098, 0x0000);
	LCD_CtrlWrite_ILI9320(0x0007, 0x0173); // 262K color and display ON
}
static
void LCD_ExitSleep_ILI9320(void)
{
	//*************Power On sequence ******************//
	LCD_CtrlWrite_ILI9320(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
	LCD_CtrlWrite_ILI9320(0x0011, 0x0000); // DC1[2:0], DC0[2:0], VC[2:0]
	LCD_CtrlWrite_ILI9320(0x0012, 0x0000); // VREG1OUT voltage
	LCD_CtrlWrite_ILI9320(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
	local_delay_ms(200); // Dis-charge capacitor power voltage
	LCD_CtrlWrite_ILI9320(0x0010, 0x17B0); // SAP, BT[3:0], AP, DSTB, SLP, STB
	LCD_CtrlWrite_ILI9320(0x0011, 0x0147); // DC1[2:0], DC0[2:0], VC[2:0]
	local_delay_ms(50); // Delay 50ms
	LCD_CtrlWrite_ILI9320(0x0012, 0x013C); // VREG1OUT voltage
	local_delay_ms(50); // Delay 50ms
	LCD_CtrlWrite_ILI9320(0x0013, 0x0E00); // VDV[4:0] for VCOM amplitude
	LCD_CtrlWrite_ILI9320(0x0029, 0x0009); // VCM[4:0] for VCOMH
	local_delay_ms(50);
	LCD_CtrlWrite_ILI9320(0x0007, 0x0173); // 262K color and display ON
}
static
void LCD_EnterSleep_ILI9320(void)
{
	LCD_CtrlWrite_ILI9320(0x0007, 0x0000); // display OFF
	//************* Power OFF sequence **************//
	LCD_CtrlWrite_ILI9320(0x0010, 0x0000); // SAP, BT[3:0], APE, AP, DSTB, SLP
	LCD_CtrlWrite_ILI9320(0x0011, 0x0000); // DC1[2:0], DC0[2:0], VC[2:0]
	LCD_CtrlWrite_ILI9320(0x0012, 0x0000); // VREG1OUT voltage
	LCD_CtrlWrite_ILI9320(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
	local_delay_ms(200); // Dis-charge capacitor power voltage
	LCD_CtrlWrite_ILI9320(0x0010, 0x0002); // SAP, BT[3:0], APE, AP, DSTB, SLP
}

static void ili9320_disable(uint_fast8_t state)
{
	if (state != 0)
		LCD_EnterSleep_ILI9320();
	else
		LCD_ExitSleep_ILI9320();
}


static void ili9320_initialize(void)
{

	ILI9320_CPT32_Initial();

	//ili9320_set_contrast(255);
	//ili9320_clear();

#if 0
	// Печать параметров на экране
	for (;;)
	{
		//static unsigned  count;
		//count ++;
		char buff [22];
		const unsigned long n = 0; //LCD_Read_ILI9320(0x0000);	// 0x9242 expected
		const unsigned long n2 = LCD_Status_ILI9320();

		local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
			PSTR("%04lX,%04lX"),
				n,
				n2
			 );
		display_at(0, 0, buff);
	}
#endif


	//for (;;)
	//	;
}

/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{

	ili9320_initialize();
#if 0
	ili9320_clear(BLACK);
	// Печать параметров на экране
	for (;;)
	{
		//static unsigned  count;
		//count ++;
		char buff [22];
		const unsigned long n = LCD_Read_ILI9320(0x0000);	// 0x9320 expected
		//const unsigned long n2 = LCD_Status_ILI9320();

		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("%04lX"),
				n
			 );
		display_at(0, 0, buff);
	}
#endif
}

void display_set_contrast(uint_fast8_t v)
{
}

void 
display_clear(void)
{
	const COLORMAIN_T bg = display_getbgcolor();

	ili9320_clear(bg);
}

// для framebufer дисплеев - вытолкнуть кэш память
void display_flush(void)
{
}

void
//NOINLINEAT
colmain_setcolors(COLORMAIN_T fg, COLORMAIN_T bg)
{
	ili9320_setcolor(fg, bg);
}

void colmain_setcolors3(COLORMAIN_T fg, COLORMAIN_T bg, COLORMAIN_T fgbg)
{
	colmain_setcolors(fg, bg);
}

void
display_wrdata_begin(void)
{
	ili9320_set_windowh(SMALLCHARH);
	ili9320_put_char_begin();
}

void
display_wrdata_end(void)
{
	ili9320_put_char_end();
}

void
display_wrdatabar_begin(void)
{
	ili9320_set_windowh(CHAR_H);
	ili9320_put_char_begin();
}

void
display_wrdatabar_end(void)
{
	ili9320_put_char_end();
}


void
display_wrdatabig_begin(void)
{
	ili9320_set_windowh(BIGCHARH);	// same as HALFCHARH
	ili9320_put_char_begin();
}


void
display_wrdatabig_end(void)
{
	ili9320_put_char_end();
}

/* отображение одной вертикальной полосы на графическом индикаторе */
/* старшие биты соответствуют верхним пикселям изображения */
/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
uint_fast16_t
display_barcolumn(uint_fast16_t xpix, uint_fast16_t ypix, uint_fast8_t pattern)
{
	ili9320_bar_column(pattern);
	return xpix + 1;
}

void
display_put_char_big(uint_fast8_t c, uint_fast8_t lowhalf)
{
	ili9320_put_char_big(c, lowhalf);
}

void
display_put_char_half(uint_fast8_t c, uint_fast8_t lowhalf)
{
	ili9320_put_char_half(c, lowhalf);
}


// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
void
display_put_char_small(uint_fast8_t c, uint_fast8_t lowhalf)
{
	ili9320_put_char_fast(c, lowhalf);
}

void
display_gotoxy(uint_fast8_t x, uint_fast8_t y)
{
#if LCDMODE_ILI9320_TOPDOWN
	/* перевёрнутое изображение */
	saved_y = (DIM_Y - 1) - (y * CHAR_H);
	ili9320_set_graddr((x * CHAR_W));
#else /* LCDMODE_ILI9320_TOPDOWN */
	/* нормальное изображение */
	saved_y = (y * CHAR_H);
	ili9320_set_graddr((DIM_X - 1) - (x * CHAR_W));
#endif	/* LCDMODE_ILI9320_TOPDOWN */
}

// Координаты в пикселях
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
#if LCDMODE_ILI9320_TOPDOWN
	/* перевёрнутое изображение */
	saved_y = (DIM_Y - 1) - (y);
	ili9320_set_graddr((x));
#else /* LCDMODE_ILI9320_TOPDOWN */
	/* нормальное изображение */
	saved_y = (y);
	ili9320_set_graddr((DIM_X - 1) - (x));
#endif	/* LCDMODE_ILI9320_TOPDOWN */
}

void display_plotstart(
	uint_fast16_t dy	// Высота окна в пикселях
	)
{
	ili9320_set_windowh(dy);
	ili9320_put_char_begin();
}

void display_plotstop(void)
{
	ili9320_put_char_end();
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
void display_reset(void)
{
	//************* Reset LCD Driver ****************//
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

/* Разряжаем конденсаторы питания */
void display_discharge(void)
{
}

#endif /* LCDMODE_ILI9320 */
