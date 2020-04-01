/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// TFT панель 220 * 176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15
// Управление в 8-битном режиме. IM0=1: 8 bit, 4 wires, serial (SDI, SCL, CS, A0)
// SF-TC220B-9230A-T (возможно).


#include "hardware.h"
#include "board.h"
#include "display.h"
#include <stdint.h>

#include "../inc/spi.h"
#include "board.h"

//#include "formats.h"	// for debug prints

#if LCDMODE_ILI9225 || LCDMODE_ST7781

// LCDMODE_ILI9225 - 220*176
// LCDMODE_ST7781 - 320*240

#include "ili9225.h"
/* растровые шрифты */
#include "fontmaps.h"

#define ILI9225_CMND() do { board_lcd_rs(0); } while (0)	/* RS: Low: command */
#define ILI9225_DATA() do { board_lcd_rs(1); } while (0)	/* RS: High: data */


#if LCDMODE_ST7781
	#define LCDMODE_PARALEAL	1
#endif /* LCDMODE_ST7781 */


#if ! LCDMODE_PARALEAL
	#define ILI9225_SPIMODE		SPIC_MODE3	// без переключения в software spi работает и mode2. В HW не работает SPIC_MODE1
	// Условие использования оптимизированных функций обращения к SPI
	#define WITHSPIEXT16 (WITHSPIHW && WITHSPI16BIT)
#endif

/* Enable SPI */
// выставить чипселект
static void ili9225_cs_activate(void)
{
#if LCDMODE_PARALEAL
	prog_select(targetlcd);	/* start sending data to target chip */
#elif WITHSPIEXT16
	hardware_spi_connect_b16(SPIC_SPEEDFAST, ILI9225_SPIMODE);
	prog_select(targetlcd);	/* start sending data to target chip */
#else
	spi_select(targetlcd, ILI9225_SPIMODE);
#endif
}

/* Disable SPI */
static void ili9225_cs_deactivate(void)
{
#if LCDMODE_PARALEAL
	prog_unselect(targetlcd);
#elif WITHSPIEXT16
	prog_unselect(targetlcd);
	hardware_spi_disconnect();
#else
	spi_unselect(targetlcd);
#endif
}

	// для данной микросхемы не требуется снятие CS между командой и данными
// состояние RS защёлкивается с последним битом передающегося сейчас слова
/**
 * \brief Send command to LCD controller.
 *
 * \param cmdv command.
 */
static void ili9225_write_cmd(uint_fast8_t cmdv)
{
	ILI9225_CMND();	/* RS: Low: select an index or status register */

#if LCDMODE_PARALEAL

	DISPLAY_BUS_DATA_SET(cmdv);
	DISPLAY_BUS_NWR_C(DISPLAY_BUS_NWR);
	DISPLAY_BUS_NWR_S(DISPLAY_BUS_NWR);

#elif WITHSPIEXT16

	hardware_spi_b16_p1(cmdv);	// старшая половина - 0 (NOP)
	hardware_spi_complete_b16();

#else

	spi_progval8_p1(targetlcd, cmdv);
	spi_complete(targetlcd);

#endif
}

/**
 * \brief Write data to LCD GRAM.
 *
 * \param datav data.
 */
static void ili9225_write_data(uint_fast16_t datav)
{
	ILI9225_DATA();	/* RS: High: select a control register */

#if LCDMODE_PARALEAL

	DISPLAY_BUS_DATA_SET(datav >> 8);
	DISPLAY_BUS_NWR_C(DISPLAY_BUS_NWR);
	DISPLAY_BUS_NWR_S(DISPLAY_BUS_NWR);

	DISPLAY_BUS_DATA_SET(datav >> 0);
	DISPLAY_BUS_NWR_C(DISPLAY_BUS_NWR);
	DISPLAY_BUS_NWR_S(DISPLAY_BUS_NWR);

#elif WITHSPIEXT16

	hardware_spi_b16_p1(datav);
	hardware_spi_complete_b16();

#else

	spi_progval8_p1(targetlcd, datav >> 8);
	spi_progval8_p2(targetlcd, datav >> 0);
	spi_complete(targetlcd);

#endif
}



// запись в регистр, CS выбран ранее
static void
LCD_CtrlWrite_ILI9225fast(
	uint_fast8_t addr,	
	uint_fast16_t data
	)
{
	ili9225_write_cmd(addr);
	ili9225_write_data(data);
}


// используется в инициализации
static void
LCD_CtrlWrite(
	uint_fast8_t addr,	
	uint_fast16_t data
	)
{
	ili9225_cs_activate();
	ili9225_write_cmd(addr);
	ili9225_write_data(data);
	ili9225_cs_deactivate();
}


// в режим передачи данных переводим сразу по окончании команд.
// вызывается всегда после ili9225_set_strype - потому есть возможность использовать ili9225_cs_activate

static void ili9225_put_char_begin(void)
{
	ili9225_cs_activate();
	ili9225_write_cmd(ILI9225_GRAM_DATA_REG);

	ILI9225_DATA();	/* RS: High: select a control register */
}

static void ili9225_put_char_end(void)
{
	ili9225_cs_deactivate();
}

#if WITHSPIEXT16 || LCDMODE_PARALEAL
	static COLORMAIN_T fgcolor, bkcolor;
#else
	static struct { uint_fast8_t first, second; } fgcolor, bkcolor;
#endif

static void ili9225_setcolor(COLORMAIN_T acolor, COLORMAIN_T abkcolor)
{
#if WITHSPIEXT16 || LCDMODE_PARALEAL
	fgcolor = acolor;
	bkcolor = abkcolor;
#else
	fgcolor.first = acolor >> 8;
	fgcolor.second = acolor >> 0;
	bkcolor.first = abkcolor >> 8;
	bkcolor.second = abkcolor >> 0;
#endif
}

static void ili9225_pixel_p1(
	uint_fast8_t fg	// 0 - background color, other - foreground color
	)
{
#if LCDMODE_PARALEAL
	ili9225_write_data(fg ? fgcolor : bkcolor);
#elif WITHSPIEXT16
	hardware_spi_b16_p1(fg ? fgcolor : bkcolor);
#else
	if (fg)
	{
		spi_progval8_p1(targetlcd, fgcolor.first);	// смотреть бит TRI а регистре 03
		spi_progval8_p2(targetlcd, fgcolor.second);	// смотреть бит TRI а регистре 03
	}
	else
	{
		spi_progval8_p1(targetlcd, bkcolor.first);	// смотреть бит TRI а регистре 03
		spi_progval8_p2(targetlcd, bkcolor.second);	// смотреть бит TRI а регистре 03
	}
#endif
}

static void ili9225_pixel_p2(
	uint_fast8_t fg	// 0 - background color, other - foreground color
	)
{
#if LCDMODE_PARALEAL
	ili9225_write_data(fg ? fgcolor : bkcolor);
#elif WITHSPIEXT16
	hardware_spi_b16_p2(fg ? fgcolor : bkcolor);
#else
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
#endif
}

static void ili9225_pixel_p3(
	uint_fast8_t fg	// 0 - background color, other - foreground color
	)
{
#if LCDMODE_PARALEAL
	ili9225_write_data(fg ? fgcolor : bkcolor);
#elif WITHSPIEXT16
	hardware_spi_b16_p2(fg ? fgcolor : bkcolor);
	hardware_spi_complete_b16();
#else
	if (fg)
	{
		spi_progval8_p2(targetlcd, fgcolor.first);	// смотреть бит TRI а регистре 03
		spi_progval8_p2(targetlcd, fgcolor.second);	// смотреть бит TRI а регистре 03
		spi_complete(targetlcd);
	}
	else
	{
		spi_progval8_p2(targetlcd, bkcolor.first);	// смотреть бит TRI а регистре 03
		spi_progval8_p2(targetlcd, bkcolor.second);	// смотреть бит TRI а регистре 03
		spi_complete(targetlcd);
	}
#endif
}

static void ili9225_colorpixel_p1(
	COLORMAIN_T color
	)
{
#if LCDMODE_PARALEAL
	ili9225_write_data(color);
#elif WITHSPIEXT16
	hardware_spi_b16_p1(color);
#else
	spi_progval8_p1(targetlcd, color >> 8);	// смотреть бит TRI а регистре 03
	spi_progval8_p2(targetlcd, color >> 0);	// смотреть бит TRI а регистре 03
#endif
}

static void ili9225_colorpixel_p2(
	COLORMAIN_T color
	)
{
#if LCDMODE_PARALEAL
	ili9225_write_data(color);
#elif WITHSPIEXT16
	hardware_spi_b16_p2(color);
#else
	spi_progval8_p2(targetlcd, color >> 8);	// смотреть бит TRI а регистре 03
	spi_progval8_p2(targetlcd, color >> 0);	// смотреть бит TRI а регистре 03
#endif
}

static void ili9225_colorpixel_p3(
	COLORMAIN_T color
	)
{
#if LCDMODE_PARALEAL
	ili9225_write_data(color);
#elif WITHSPIEXT16
	hardware_spi_b16_p2(color);
	hardware_spi_complete_b16();
#else
	spi_progval8_p2(targetlcd, color >> 8);	// смотреть бит TRI а регистре 03
	spi_progval8_p2(targetlcd, color >> 0);	// смотреть бит TRI а регистре 03
	spi_complete(targetlcd);
#endif
}

// Выдать восемь цветных пикселей
static void 
ili9225_pix8(
	uint_fast8_t v
	)
{
	ili9225_pixel_p1(v & 0x01);
	ili9225_pixel_p2(v & 0x02);
	ili9225_pixel_p2(v & 0x04);
	ili9225_pixel_p2(v & 0x08);
	ili9225_pixel_p2(v & 0x10);
	ili9225_pixel_p2(v & 0x20);
	ili9225_pixel_p2(v & 0x40);
	ili9225_pixel_p3(v & 0x80);
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
static void ili9225_put_char_small(char cc)
{
	uint_fast8_t i = 0;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_smallfont [0] / sizeof ls020_smallfont [0][0]) };
	const FLASHMEM uint8_t * p = & ls020_smallfont [c][0];
	
	for (; i < NBYTES; ++ i)
		ili9225_pix8(p [i]);	// Выдать восемь цветных пикселей
}

// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
static void ili9225_put_char_big(char cc)
{
	// '#' - узкий пробел
	enum { NBV = (BIGCHARH / 8) }; // сколько байтов в одной вертикали
	uint_fast8_t i = NBV * ((cc == '.' || cc == '#') ? 12 : 0);	// начальная колонка знакогенератора, откуда начинать.
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ILI9225_bigfont [0] / sizeof ILI9225_bigfont [0][0]) };
	const FLASHMEM uint8_t * p = & ILI9225_bigfont [c][0];
	
	for (; i < NBYTES; ++ i)
		ili9225_pix8(p [i]);	// Выдать восемь цветных пикселей
}

// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
static void ili9225_put_char_half(char cc)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ILI9225_halffont [0] / sizeof ILI9225_halffont [0][0]) };
	const FLASHMEM uint8_t * p = & ILI9225_halffont [c][0];
	
	for (; i < NBYTES; ++ i)
		ili9225_pix8(p [i]);	// Выдать восемь цветных пикселей
}

static uint_fast8_t ili9225_y;	/* в пикселях */

// открыть для записи полосу пикселей высотой как указано в параметре (символ) и шириной до правого края
static void
ili9225_set_strype(uint_fast8_t height)
{
	ili9225_cs_activate();

#if LCDMODE_ILI9225_TOPDOWN
	const uint_fast8_t yc1 = ili9225_y + (height - 1);
	const uint_fast8_t yc2 = ili9225_y;

	LCD_CtrlWrite_ILI9225fast(ILI9225_RAM_ADDR_SET1, yc2);	// Horizontal GRAM Address Set
	// открыть строку для записи текста
	LCD_CtrlWrite_ILI9225fast(ILI9225_HORIZONTAL_WINDOW_ADDR1, yc1); // Horizontal Address Start Position
	LCD_CtrlWrite_ILI9225fast(ILI9225_HORIZONTAL_WINDOW_ADDR2, yc2); // Horizontal Address End Position

#else	/* LCDMODE_ILI9225_TOPDOWN */
	const uint_fast8_t yc1 = (DIM_Y - 1) - ili9225_y;
	const uint_fast8_t yc2 = (DIM_Y - 1) - ili9225_y - (height - 1);

	LCD_CtrlWrite_ILI9225fast(ILI9225_RAM_ADDR_SET1, yc1);	// Horizontal GRAM Address Set
	// открыть строку для записи текста
	LCD_CtrlWrite_ILI9225fast(ILI9225_HORIZONTAL_WINDOW_ADDR1, yc1); // Horizontal Address Start Position
	LCD_CtrlWrite_ILI9225fast(ILI9225_HORIZONTAL_WINDOW_ADDR2, yc2); // Horizontal Address End Position

#endif	/* LCDMODE_ILI9225_TOPDOWN */

	ili9225_cs_deactivate();
}

static void ili9225_set_addr_column(uint_fast8_t x)
{
	ili9225_cs_activate();

#if LCDMODE_ILI9225_TOPDOWN
	LCD_CtrlWrite_ILI9225fast(ILI9225_RAM_ADDR_SET2, (DIM_X - 1) - x);	// Vertical GRAM Address Set
	// постоянная часть выностится в инициализацию
	LCD_CtrlWrite_ILI9225fast(ILI9225_VERTICAL_WINDOW_ADDR1, (DIM_X - 1) - x); // Vertical GRAM End Address
	//LCD_CtrlWrite(ILI9225_VERTICAL_WINDOW_ADDR2, 0); // Vertical GRAM Start Address
#else	/* LCDMODE_ILI9225_TOPDOWN */
	LCD_CtrlWrite_ILI9225fast(ILI9225_RAM_ADDR_SET2, x);	// Vertical GRAM Address Set
	// постоянная часть выностится в инициализацию
	//LCD_CtrlWrite(ILI9225_VERTICAL_WINDOW_ADDR1, DIM_X - 1); // Vertical GRAM End Address
	LCD_CtrlWrite_ILI9225fast(ILI9225_VERTICAL_WINDOW_ADDR2, x); // Vertical GRAM Start Address
#endif	/* LCDMODE_ILI9225_TOPDOWN */

	ili9225_cs_deactivate();
}

static void ili9225_clear(COLORMAIN_T bg)
{
	unsigned i;
	
	display_gotoxy(0, 0);
	ili9225_setcolor(COLOR_WHITE, bg);

	ili9225_set_strype(DIM_Y);	// установить окно высотой в весь экран
	ili9225_put_char_begin();
	for (i = 0; i < ((unsigned long) DIM_Y * DIM_X) / 8; ++ i)
	{
		ili9225_pix8(0x00);	// Выдать восемь цветных пикселей
	}
	ili9225_put_char_end();

}

#if LCDMODE_ST7781

/* вызывается при разрешённых прерываниях. */
/* st7781 - 320*240 2.8" panel with praralel 8 bit interface */
void display_initialize(void)
{
#if 1
	ili9225_cs_activate();
	//--------------------------------ST7781 Internal Register Initial---------------------------------------//
	ili9225_write_cmd(0x00ff);ili9225_write_data(0x0001);
	ili9225_write_cmd(0x00f3);ili9225_write_data(0x0008);
	//LCD_CtrlRead_ST7781 (0x00f3); //Read Parameter
	//---------------------------End ST7781 Internal Register Initial--------------------------------------//
	//------------------------------------Display Control Setting----------------------------------------------//
	ili9225_write_cmd(0x0001);ili9225_write_data(0x0100); //Output Direct
	ili9225_write_cmd(0x0002);ili9225_write_data(0x0700); //Line Inversion
	ili9225_write_cmd(0x0003);ili9225_write_data(0x1030); //Entry Mode (65K, BGR)
	ili9225_write_cmd(0x0008);ili9225_write_data(0x0807); // Porch Setting
	ili9225_write_cmd(0x0009);ili9225_write_data(0x0000); //Scan Cycle
	ili9225_write_cmd(0x000A);ili9225_write_data(0x0000); //FMARK off
	//-----------------------------------End Display Control setting-----------------------------------------//
	//-------------------------------- Power Control Registers Initial --------------------------------------//
	ili9225_write_cmd(0x0010);ili9225_write_data(0x0790); //Power Control1
	ili9225_write_cmd(0x0011);ili9225_write_data(0x0005); //Power Control2
	ili9225_write_cmd(0x0012);ili9225_write_data(0x0000); //Power Control3
	ili9225_write_cmd(0x0013);ili9225_write_data(0x0000); //Power Control4
	//---------------------------------End Power Control Registers Initial -------------------------------//
	local_delay_ms(100);; //Delay 100ms
	//--------------------------------- Power Supply Startup 1 Setting------------------------------------//
	ili9225_write_cmd(0x0010);ili9225_write_data(0x1490); //Power Control1
	local_delay_ms(50); //Delay 50ms
	ili9225_write_cmd(0x0011);ili9225_write_data(0x0227); //Power Control2
	//--------------------------------- End Power Supply Startup 1 Setting------------------------------//
	local_delay_ms(50); //Delay 50ms
	//--------------------------------- Power Supply Startup 2 Setting------------------------------------//
	ili9225_write_cmd(0x0012);ili9225_write_data(0x0089); //Power Control3
	ili9225_write_cmd(0x0013);ili9225_write_data(0x1900); //Power Control4
	ili9225_write_cmd(0x0029);ili9225_write_data(0x0021); //VCOMH setting
	//--------------------------------- End Power Supply Startup 2 Setting------------------------------//
	local_delay_ms(50); //Delay 50ms
	//-------------------------------------Gamma Cluster Setting-------------------------------------------//
	ili9225_write_cmd(0x0030);ili9225_write_data(0x0000);
	ili9225_write_cmd(0x0031);ili9225_write_data(0x0006);
	ili9225_write_cmd(0x0032);ili9225_write_data(0x0100);
	ili9225_write_cmd(0x0035);ili9225_write_data(0x0001);
	ili9225_write_cmd(0x0036);ili9225_write_data(0x0000);
	ili9225_write_cmd(0x0037);ili9225_write_data(0x0000);
	ili9225_write_cmd(0x0038);ili9225_write_data(0x0406);
	ili9225_write_cmd(0x0039);ili9225_write_data(0x0202);
	ili9225_write_cmd(0x003c);ili9225_write_data(0x0001);
	ili9225_write_cmd(0x003d);ili9225_write_data(0x0000);
	//---------------------------------------End Gamma Setting---------------------------------------------//
	//----------------------------------Display Windows 240 X 320----------------------------------------//
	ili9225_write_cmd(0x0050);ili9225_write_data(0x0000); // Horizontal Address Start Position
	ili9225_write_cmd(0x0051);ili9225_write_data(0x00ef); // Horizontal Address End Position
	ili9225_write_cmd(0x0052);ili9225_write_data(0x0000); // Vertical Address Start Position
	ili9225_write_cmd(0x0053);ili9225_write_data(0x013f); // Vertical Address End Position
	//----------------------------------End Display Windows 240 X 320----------------------------------//
	//-------------------------------------------Frame Rate Setting-------------------------------------------//
	ili9225_write_cmd(0x0060);ili9225_write_data(0xa700); //Gate scan control
	ili9225_write_cmd(0x0061);ili9225_write_data(0x0001); //Non-display Area setting
	ili9225_write_cmd(0x0090);ili9225_write_data(0x0033); //RTNI setting
	//-------------------------------------------END Frame Rate setting------------------------------------//
	ili9225_write_cmd(0x0007);ili9225_write_data(0x0133); //Display Control1
	local_delay_ms(50); //Delay 50ms
	
	ili9225_write_cmd(0x0022);	
	ili9225_cs_deactivate();

#elif 1
		/* Start Initial Sequence */
		LCD_CtrlWrite(0xFF,0x0001);
		LCD_CtrlWrite(0xF3,0x0008);
		LCD_CtrlWrite(0x01,0x0100);
		LCD_CtrlWrite(0x02,0x0700);
		LCD_CtrlWrite(0x03,0x1030);  
		LCD_CtrlWrite(0x08,0x0302);
		LCD_CtrlWrite(0x08,0x0207);
		LCD_CtrlWrite(0x09,0x0000);
		LCD_CtrlWrite(0x0A,0x0000);
		LCD_CtrlWrite(0x10,0x0000);  
		LCD_CtrlWrite(0x11,0x0005);
		LCD_CtrlWrite(0x12,0x0000);
		LCD_CtrlWrite(0x13,0x0000);
		local_delay_ms(50);
		LCD_CtrlWrite(0x10,0x12B0);
		local_delay_ms(50);
		LCD_CtrlWrite(0x11,0x0007);
		local_delay_ms(50);
		LCD_CtrlWrite(0x12,0x008B);
		local_delay_ms(50);	
		LCD_CtrlWrite(0x13,0x1700);
		local_delay_ms(50);	
		LCD_CtrlWrite(0x29,0x0022);		
		LCD_CtrlWrite(0x30,0x0000);
		LCD_CtrlWrite(0x31,0x0707);
		LCD_CtrlWrite(0x32,0x0505);
		LCD_CtrlWrite(0x35,0x0107);
		LCD_CtrlWrite(0x36,0x0008);
		LCD_CtrlWrite(0x37,0x0000);
		LCD_CtrlWrite(0x38,0x0202);
		LCD_CtrlWrite(0x39,0x0106);
		LCD_CtrlWrite(0x3C,0x0202);
		LCD_CtrlWrite(0x3D,0x0408);
		local_delay_ms(50);				
		LCD_CtrlWrite(0x50,0x0000);		
		LCD_CtrlWrite(0x51,0x00EF);		
		LCD_CtrlWrite(0x52,0x0000);		
		LCD_CtrlWrite(0x53,0x013F);		
		LCD_CtrlWrite(0x60,0xA700);		
		LCD_CtrlWrite(0x61,0x0001);
		LCD_CtrlWrite(0x90,0x0033);				
		LCD_CtrlWrite(0x2B,0x000B);		
		LCD_CtrlWrite(0x07,0x0133);
#else

	enum { bgr_and_ss = 1 };

	LCD_CtrlWrite(ILI9225_DRIVER_OUTPUT_CTRL, (bgr_and_ss * ILI9225_DRIVER_OUTPUT_CTRL_SS)); // set SS=1 and NL=1 bit
	LCD_CtrlWrite(ILI9225_LCD_AC_DRIVING_CTRL, ILI9225_LCD_AC_DRIVING_CTRL_D10);
	LCD_CtrlWrite(ILI9225_BLANK_PERIOD_CTRL1, 0x0808); // set BP and FP

	//// ?? LCD_CtrlWrite(ILI9225_FRAME_CYCLE_CTRL, 0x0101); //ADD  Set frame cycle

	//LCD_CtrlWrite(ILI9225_OSC_CTRL, 0x0C01); // Set frame rate  0x0801 !!!!!! 2.0 use 0x0b01
	LCD_CtrlWrite(ILI9225_OSC_CTRL, 0x0701); // Set frame rate  07xx = 66 Hz

	////LCD_CtrlWrite(0x0020,0x0000); // Set GRAM Address
	////LCD_CtrlWrite(0x0021,0x0000); // Set GRAM Address

	//--------Power On sequence ----------//
	local_delay_ms(50); // Delay 50ms
	LCD_CtrlWrite(ILI9225_POWER_CTRL1, 0x0F00); // Set SAP,DSTB,STB ,set the driving cabality of source driver, 0x0A00

	LCD_CtrlWrite(ILI9225_POWER_CTRL2, 0x173B); // Set APON,PON,AON,VCI1EN,VC 
	local_delay_ms(50); // Delay 50ms

	LCD_CtrlWrite(ILI9225_POWER_CTRL3, 0x6231); // Internal reference voltage= Vci; 

	LCD_CtrlWrite(ILI9225_POWER_CTRL4, 0x006D); // Set GVDD 0x006D

	//LCD_CtrlWrite(ILI9225_POWER_CTRL5, 0x404E); // Set VCOMH/VCOML voltage 0x4350 T=1.0 mm
	LCD_CtrlWrite(ILI9225_POWER_CTRL5, 0x454E); // Set VCOMH/VCOML voltage 0x4350 T = 0.6 mm

	//---------- Set GRAM area -------------//
	#if 0
	// not used
	LCD_CtrlWrite(ILI9225_GATE_SCAN_CTRL, 0);
	LCD_CtrlWrite(ILI9225_VERTICAL_SCROLL_CTRL1, DIM_X - 1);	// 220 - 1	- Vertical Scroll Control 1 Register
	LCD_CtrlWrite(ILI9225_VERTICAL_SCROLL_CTRL2, 0);
	LCD_CtrlWrite(ILI9225_VERTICAL_SCROLL_CTRL3, 0);
	LCD_CtrlWrite(ILI9225_PARTIAL_DRIVING_POS1, DIM_X - 1);	// 220 - 1
	LCD_CtrlWrite(ILI9225_PARTIAL_DRIVING_POS2, 0);
	#endif

	#if 0
	// ------- Adjust the Gamma Curve ------//
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL1, 0x0000);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL2, 0x0808);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL3, 0x080A);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL4, 0x000A);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL5, 0x0A08);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL6, 0x0808);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL7, 0x0000);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL8, 0x0A00);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL9, 0x1007);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL10, 0x0710);
	#endif

	local_delay_ms(50); // Delay 50ms

	/* check args */ LCD_CtrlWrite(ILI9225_DISP_CTRL1, 0x1017); //Turn on Display
	//WriteCommand(0x00,0x22);

	// +++ запись константной части адресации курсора здесь (заполнение до правой части экрана).
	#if LCDMODE_ILI9225_TOPDOWN
		LCD_CtrlWrite(ILI9225_ENTRY_MODE, (bgr_and_ss * ILI9225_ENTRY_MODE_BGR) | ILI9225_ENTRY_MODE_ID(1)); // set GRAM write direction and BGR=1.
		LCD_CtrlWrite(ILI9225_VERTICAL_WINDOW_ADDR2, 0); // Vertical GRAM Start Address
	#else	/* LCDMODE_ILI9225_TOPDOWN */
		LCD_CtrlWrite(ILI9225_ENTRY_MODE, (bgr_and_ss * ILI9225_ENTRY_MODE_BGR) | ILI9225_ENTRY_MODE_ID(2)); // set GRAM write direction and BGR=1.
		LCD_CtrlWrite(ILI9225_VERTICAL_WINDOW_ADDR1, DIM_X - 1); // Vertical GRAM End Address
	#endif	/* LCDMODE_ILI9225_TOPDOWN */

#endif
}

#else /* LCDMODE_ST7781 */

/* вызывается при разрешённых прерываниях. */
/* ili9225 */
void display_initialize(void)
{
	enum { bgr_and_ss = 1 };

	LCD_CtrlWrite(ILI9225_DRIVER_OUTPUT_CTRL, (bgr_and_ss * ILI9225_DRIVER_OUTPUT_CTRL_SS) | ILI9225_DRIVER_OUTPUT_CTRL_NL(0x1C)); // set SS=1 and NL=1 bit
	LCD_CtrlWrite(ILI9225_LCD_AC_DRIVING_CTRL, 0x0100); // set 1 line inversion
	LCD_CtrlWrite(ILI9225_BLANK_PERIOD_CTRL1, 0x0808); // set BP and FP
	//LCD_CtrlWrite(ILI9225_INTERFACE_CTRL, 0x0000); // RGB interface setting R0Ch=0x0110 for RGB 18Bit and R0Ch=0111 for RGB16Bit

	LCD_CtrlWrite(ILI9225_FRAME_CYCLE_CTRL, 0x0101); //ADD  Set frame cycle

	//LCD_CtrlWrite(ILI9225_OSC_CTRL, 0x0C01); // Set frame rate  0x0801 !!!!!! 2.0 use 0x0b01
	LCD_CtrlWrite(ILI9225_OSC_CTRL, 0x0701); // Set frame rate  07xx = 66 Hz

	////LCD_CtrlWrite(0x0020,0x0000); // Set GRAM Address
	////LCD_CtrlWrite(0x0021,0x0000); // Set GRAM Address

	//--------Power On sequence ----------//
	local_delay_ms(50); // Delay 50ms
	LCD_CtrlWrite(ILI9225_POWER_CTRL1, 0x0F00); // Set SAP,DSTB,STB ,set the driving cabality of source driver, 0x0A00

	LCD_CtrlWrite(ILI9225_POWER_CTRL2, 0x173B); // Set APON,PON,AON,VCI1EN,VC 
	local_delay_ms(50); // Delay 50ms

	LCD_CtrlWrite(ILI9225_POWER_CTRL3, 0x6231); // Internal reference voltage= Vci; 

	LCD_CtrlWrite(ILI9225_POWER_CTRL4, 0x006D); // Set GVDD 0x006D

	//LCD_CtrlWrite(ILI9225_POWER_CTRL5, 0x404E); // Set VCOMH/VCOML voltage 0x4350 T=1.0 mm
	LCD_CtrlWrite(ILI9225_POWER_CTRL5, 0x454E); // Set VCOMH/VCOML voltage 0x4350 T = 0.6 mm

	//---------- Set GRAM area -------------//
	#if 0
	// not used
	LCD_CtrlWrite(ILI9225_GATE_SCAN_CTRL, 0);
	LCD_CtrlWrite(ILI9225_VERTICAL_SCROLL_CTRL1, DIM_X - 1);	// 220 - 1	- Vertical Scroll Control 1 Register
	LCD_CtrlWrite(ILI9225_VERTICAL_SCROLL_CTRL2, 0);
	LCD_CtrlWrite(ILI9225_VERTICAL_SCROLL_CTRL3, 0);
	LCD_CtrlWrite(ILI9225_PARTIAL_DRIVING_POS1, DIM_X - 1);	// 220 - 1
	LCD_CtrlWrite(ILI9225_PARTIAL_DRIVING_POS2, 0);
	#endif

	#if 0
	// ------- Adjust the Gamma Curve ------//
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL1, 0x0000);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL2, 0x0808);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL3, 0x080A);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL4, 0x000A);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL5, 0x0A08);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL6, 0x0808);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL7, 0x0000);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL8, 0x0A00);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL9, 0x1007);
	LCD_CtrlWrite(ILI9225_GAMMA_CTRL10, 0x0710);
	#endif

	local_delay_ms(50); // Delay 50ms

	LCD_CtrlWrite(ILI9225_DISP_CTRL1, 0x1017); //Turn on Display
	//WriteCommand(0x00,0x22);

	// +++ запись константной части адресации курсора здесь (заполнение до правой части экрана).
	#if LCDMODE_ILI9225_TOPDOWN
		LCD_CtrlWrite(ILI9225_ENTRY_MODE, (bgr_and_ss * ILI9225_ENTRY_MODE_BGR) | ILI9225_ENTRY_MODE_ID(1)); // set GRAM write direction and BGR=1.
		LCD_CtrlWrite(ILI9225_VERTICAL_WINDOW_ADDR2, 0); // Vertical GRAM Start Address
	#else	/* LCDMODE_ILI9225_TOPDOWN */
		LCD_CtrlWrite(ILI9225_ENTRY_MODE, (bgr_and_ss * ILI9225_ENTRY_MODE_BGR) | ILI9225_ENTRY_MODE_ID(2)); // set GRAM write direction and BGR=1.
		LCD_CtrlWrite(ILI9225_VERTICAL_WINDOW_ADDR1, DIM_X - 1); // Vertical GRAM End Address
	#endif	/* LCDMODE_ILI9225_TOPDOWN */


}

#endif /* LCDMODE_ST7781 */


// В данном дисплее игнорируется
void display_set_contrast(uint_fast8_t v)
{
}

void 
display_clear(void)
{
	const COLORMAIN_T bg = display_getbgcolor();

	ili9225_clear(bg);
}

void
display_setcolors(COLORMAIN_T fg, COLORMAIN_T bg)
{
	ili9225_setcolor(fg, bg);
}


void display_setcolors3(COLORMAIN_T fg, COLORMAIN_T bg, COLORMAIN_T fgbg)
{
	display_setcolors(fg, bg);
}


void
display_wrdata_begin(void)
{
	ili9225_set_strype(SMALLCHARH);
	ili9225_put_char_begin();
}

void
display_wrdata_end(void)
{
	ili9225_put_char_end();
}

void
display_wrdatabig_begin(void)
{
	ili9225_set_strype(BIGCHARH);	// same as HALFCHARH
	ili9225_put_char_begin();
}


void
display_wrdatabig_end(void)
{
	ili9225_put_char_end();
}

/* отображение одной вертикальной полосы на графическом индикаторе */
/* старшие биты соответствуют верхним пикселям изображения */
/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
void 
display_barcolumn(uint_fast8_t pattern)
{
	ili9225_pix8(pattern);	// Выдать восемь цветных пикселей
}

void
display_wrdatabar_begin(void)
{
	ili9225_set_strype(CHAR_H);
	ili9225_put_char_begin();
}

void
display_wrdatabar_end(void)
{
	ili9225_put_char_end();
}

void
display_put_char_big(uint_fast8_t c, uint_fast8_t lowhalf)
{
	ili9225_put_char_big(c);
}

void
display_put_char_half(uint_fast8_t c, uint_fast8_t lowhalf)
{
	ili9225_put_char_big(c);
	//ili9225_put_char_half(c);
}


// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
void
display_put_char_small(uint_fast8_t c, uint_fast8_t lowhalf)
{
	ili9225_put_char_small(c);
}

	//uint8_t x = h * CHAR_W;
	//uint8_t y = DIM_Y - 5 - (v * CHAR_H);

void
display_gotoxy(uint_fast8_t x, uint_fast8_t y)
{
	ili9225_y = y * CHAR_H;		/* переход от символьных координат к экранным */
	ili9225_set_addr_column(x * CHAR_W);
}

// Координаты в пикселях
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
	ili9225_y = y;
	ili9225_set_addr_column(x);
}


void display_plotstart(
	uint_fast16_t height	// Высота окна в пикселях
	)
{
	ili9225_set_strype(height);
	ili9225_put_char_begin();
}

void display_plotstop(void)
{
	ili9225_put_char_end();
}

void display_plot(
	const PACKEDCOLORMAIM_T * buffer, 
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
		ili9225_colorpixel_p1(* buffer ++);
		len -= 2;
		while (len --)
			ili9225_colorpixel_p2(* buffer ++);
		ili9225_colorpixel_p3(* buffer ++);
	}
#endif /* WITHSPIEXT16 */
}


static void display_lcd_reset(uint_fast8_t v)
{
	board_lcd_reset(v); 	// Pull RST pin up/down
	board_update();
}
 
/* аппаратный сброс дисплея - перед инициализаций */
/* вызывается при разрешённых прерываниях. */
void display_reset(void)
{
	//************* Reset LCD Driver ****************//
	display_lcd_reset(1); 	// Pull RST pin up
	local_delay_ms(1); // Delay 1ms
	display_lcd_reset(0); 	// Pull RST pin down
	local_delay_ms(10); // Delay 10ms
	display_lcd_reset(1); 	// Pull RST pin up
	local_delay_ms(50); // Delay 50 ms
}

/* Разряжаем конденсаторы питания */
void display_discharge(void)
{
}

#endif /* LCDMODE_ILI9225 */
