/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Поддержка контроллера TFT панели Epson S1D13781
//


#include "hardware.h"
#include "board.h"
#include "display.h"
#include <stdint.h>

#include "spifuncs.h"
#include "formats.h"		// debug output support

#if LCDMODE_S1D13781

#include "s1d13781.h"

#define REG00_REV_CODE       0x00     // Revision Code Register [READONLY]
#define REG02_PROD_CODE      0x02     // Product Code Register [READONLY]
#define REG04_POWER_SAVE     0x04     // Power Save Register
#define REG06_RESET          0x06     // Software Reset Register
#define REG10_PLL_0          0x10     // PLL Setting Register 0
#define REG12_PLL_1          0x12     // PLL Setting Register 1
#define REG14_PLL_2          0x14     // PLL Setting Register 2
#define REG16_INTCLK         0x16     // Internal Clock Configuration Register
#define REG18_PLL_3          0x18     // PLL Setting Hidden Register 0 [RESERVED]
#define REG1A_PLL_4          0x1A     // PLL Setting Hidden Register 1 [RESERVED]
#define REG1C_PLL_5          0x1C     // PLL Setting Hidden Register 2 [RESERVED]
#define REG20_PANEL_SET      0x20     // Panel Setting Register
#define REG22_DISP_SET       0x22     // Display Setting Register
#define REG24_HDISP          0x24     // Horizontal Display Width Register
#define REG26_HNDP           0x26     // Horizontal Non-Display Period Register
#define REG28_VDISP          0x28     // Vertical Display Height Register
#define REG2A_VNDP           0x2A     // Vertical Non-Display Period Register
#define REG2C_HSW            0x2C     // HS Pulse Width Register
#define REG2E_HPS            0x2E     // HS Pulse Start Position Register
#define REG30_VSW            0x30     // VS Pulse Width Register
#define REG32_VPS            0x32     // VS Pulse Start Position Register
#define REG34_LINE_COUNT     0x34     // TE Line Count Register
#define REG40_MAIN_SET       0x40     // Main Layer Setting Register
#define REG42_MAIN_SADDR_0   0x42     // Main Layer Start Address Register 0
#define REG44_MAIN_SADDR_1   0x44     // Main Layer Start Address Register 1
#define REG46_MAIN_WIDTH     0x46     // Main Layer Width Register [READONLY]
#define REG48_MAIN_HEIGHT    0x48     // Main Layer Height Register [READONLY]
#define REG50_PIP_SET        0x50     // PIP Layer Setting Register
#define REG52_PIP_SADDR_0    0x52     // PIP Layer Start Address Register 0
#define REG54_PIP_SADDR_1    0x54     // PIP Layer Start Address Register 1
#define REG56_PIP_WIDTH      0x56     // PIP Layer Width Register
#define REG58_PIP_HEIGHT     0x58     // PIP Layer Height Register
#define REG5A_PIP_XSTART     0x5A     // PIP Layer X Start Position Register
#define REG5C_PIP_YSTART     0x5C     // PIP Layer Y Start Position Register
#define REG60_PIP_EN         0x60     // PIP Enable Register
#define REG62_ALPHA          0x62     // Alpha Blending Register
#define REG64_TRANS          0x64     // Transparency Register
#define REG66_KEY_0          0x66     // Key Color Register 0
#define REG68_KEY_1          0x68     // Key Color Register 1
#define REG80_BLT_CTRL_0     0x80     // BitBLT Control Register 0 [WRITEONLY]
#define REG82_BLT_CTRL_1     0x82     // BitBLT Control Register 1
#define REG84_BLT_STATUS     0x84     // BitBLT Status Register [READONLY]
#define REG86_BLT_CMD        0x86     // BitBLT Command Register
#define REG88_BLT_SSADDR_0   0x88     // BitBLT Source Start Address Register 0
#define REG8A_BLT_SSADDR_1   0x8A     // BitBLT Source Start Address Register 1
#define REG8C_BLT_DSADDR_0   0x8C     // BitBLT Destination Start Address Register 0
#define REG8E_BLT_DSADDR_1   0x8E     // BitBLT Destination Start Address Register 1
#define REG90_BLT_RECTOFFSET 0x90     // BitBLT Rectangle Offset Register
#define REG92_BLT_WIDTH      0x92     // BitBLT Width Register
#define REG94_BLT_HEIGHT     0x94     // BitBLT Height Register
#define REG96_BLT_BGCOLOR_0  0x96     // BitBLT Background Color Register 0
#define REG98_BLT_BGCOLOR_1  0x98     // BitBLT Background Color Register 1
#define REG9A_BLT_FGCOLOR_0  0x9A     // BitBLT Foreground Color Register 0
#define REG9C_BLT_FGCOLOR_1  0x9C     // BitBLT Foreground Color Register 1
#define REGD0_GPIO_CONFIG    0xD0     // GPIO Configuration Register
#define REGD2_GPIO_STATUS    0xD2     // GPIO Status / Control Register
#define REGD4_GPIO_PULLDOWN  0xD4     // GPIO Pull-Down Control Register

#define REGFLAG_BASE         0xFC     // Special reserved flags beyond this point
#define REGFLAG_DELAY        0xFC     // PLL Register Programming Delay (in us)
#define REGFLAG_OFF_DELAY    0xFD     // LCD Panel Power Off Delay (in ms)
#define REGFLAG_ON_DELAY     0xFE     // LCD Panel Power On Delay (in ms)
#define REGFLAG_END_OF_TABLE 0xFF     // End of Registers Marker


#include "./fonts/S1D13781_font_small.c"
#include "./fonts/S1D13781_font_half.c"
#include "./fonts/S1D13781_font_big.c"

#define S1D13781_SPIMODE SPIC_MODE3		/* допустим только MODE3, MODE2 не работает с этим контроллером */
#define S1D13781_SPIC_SPEED		SPIC_SPEED10M

// Условие использования оптимизированных функций обращения к SPI
#define WITHSPIEXT16 (WITHSPIHW && WITHSPI16BIT)

#if LCDMODE_S1D13781_TOPDOWN
	#define S1D13781_SETFLAGS (0x02 << 3)	// для перевёрнутого изображения
#else
	#define S1D13781_SETFLAGS (0x00 << 3)
#endif

#define BIGCHARWIDTH (sizeof S1D13781_bigfont [0][0] / sizeof S1D13781_bigfont [0][0][0])
#define HALFCHARWIDTH (sizeof S1D13781_halffont [0][0] / sizeof S1D13781_halffont [0][0][0])
#define BIGCHARHEIGHT (8 * (sizeof S1D13781_bigfont [0] / sizeof S1D13781_bigfont [0][0]))

#define SMALLCHARWIDTH SMALLCHARW
#define SMALLCHARHEIGHT	SMALLCHARH

#define SMALLCHARWIDTH2 SMALLCHARW
#define SMALLCHARHEIGHT2 SMALLCHARH

static void
s1d13781_select(void)
{
#if WITHSPIEXT16
	hardware_spi_connect_b16(S1D13781_SPIC_SPEED, S1D13781_SPIMODE);		// если есть возможность - работаем в 16-ти битном режиме
	prog_select(targetlcd);	
#else
	spi_select2(targetlcd, S1D13781_SPIMODE, S1D13781_SPIC_SPEED);	/* Enable SPI */
#endif /* WITHSPIHW */
}

static void
s1d13781_unselect(void)
{
#if WITHSPIEXT16
	prog_unselect(targetlcd);
	hardware_spi_disconnect();
#else
	spi_unselect(targetlcd);
#endif /* WITHSPIHW */
}


// Commands:
// 0x80 - 8 bit write
// 0xc0 - 8 bit read
// 0x88 - 16 bit write
// 0xc8 - 16 bit read

// Sets address for writes and complete spi transfer
static void set_addrwr(
	uint_fast32_t addr
	)
{
#if WITHSPIEXT16
	// 19 bit address
	// 16 bit data write
	const uint_fast32_t v = (addr) | 0x88000000UL;

	hardware_spi_b16_p1(v >> 16);
	hardware_spi_b16_p2(v >> 0);
	hardware_spi_complete_b16();

#else /* WITHSPIEXT16 */
	// 19 bit address
	// 8 bit data write
	spi_progval8_p1(targetlcd, 0x80);		// 8 bit write
	spi_progval8_p2(targetlcd, addr >> 16);
	spi_progval8_p2(targetlcd, addr >> 8);
	spi_progval8_p2(targetlcd, addr >> 0);
	spi_complete(targetlcd);
#endif /* WITHSPIEXT16 */
}

// Commands:
// 0x80 - 8 bit write
// 0xc0 - 8 bit read
// 0x88 - 16 bit write
// 0xc8 - 16 bit read

// Sets address for writes in to controller's registers
static void set_addw_p1p2_registers_nc(
	uint_fast8_t addr
	)
{
#if WITHSPIEXT16
		// 19 bit address
		// 16 bit data write
		//const uint_fast32_t v = ((S1D_PHYSICAL_REG_ADDR + addr) & 0x0007FFFF) | 0x88000000;
		const uint_fast32_t v = (S1D_PHYSICAL_REG_ADDR + addr) | 0x88000000UL;


		hardware_spi_b16_p1(v >> 16);
		hardware_spi_b16_p2(v >> 0);
#else /* WITHSPIEXT16 */
		// 19 bit address
		// 16 bit data write
		spi_progval8_p1(targetlcd, 0x80);		// 8 bit write
		spi_progval8_p2(targetlcd, 0xff & (S1D_PHYSICAL_REG_ADDR >> 16));
		spi_progval8_p2(targetlcd, 0xff & (S1D_PHYSICAL_REG_ADDR >> 8));
		spi_progval8_p2(targetlcd, addr);
#endif /* WITHSPIEXT16 */
}

// Commands:
// 0x80 - 8 bit write
// 0xc0 - 8 bit read
// 0x88 - 16 bit write
// 0xc8 - 16 bit read

// Sets address for writes in to controller's Video RAM
static void set_addrwr_8bit_p1p2_nc(
	uint_fast32_t addr
	)
{
	// 19 bit address
	// 16 bit data write
	spi_progval8_p1(targetlcd, 0x80);		// 8 bit write
	spi_progval8_p2(targetlcd, 0xff & (addr >> 16));
	spi_progval8_p2(targetlcd, 0xff & (addr >> 8));
	spi_progval8_p2(targetlcd, addr);
}


#if WITHSPIEXT16 && S1D_DISPLAY_BPP == 16

// Commands:
// 0x80 - 8 bit write
// 0xc0 - 8 bit read
// 0x88 - 16 bit write
// 0xc8 - 16 bit read

// Sets address for writes in to controller's Video RAM
static void set_addw_16bit_p1p2_nc(
	uint_fast32_t addr
	)
{
	// 19 bit address
	// 16 bit data write
	//const uint_fast32_t v = ((addr) & 0x0007FFFF) | 0x88000000;
	const uint_fast32_t v = (addr) | 0x88000000UL;


	hardware_spi_b16_p1(v >> 16);
	hardware_spi_b16_p2(v >> 0);
}

#endif /* WITHSPIEXT16 */

// Внимание, следом обязательно должно быть ожидание готовности SPI
// Sets address for reads from comntroller's registes. It includes a dummy reads.
// Commands:
// 0x80 - 8 bit write
// 0xc0 - 8 bit read
// 0x88 - 16 bit write
// 0xc8 - 16 bit read
static uint_fast8_t set_addr_p1p2_registers_getval8(
	uint_fast8_t addr
	)
{
	#if WITHSPIEXT16
		// 19 bit address
		// 16 bit data read
		//const uint_fast32_t v = ((S1D_PHYSICAL_REG_ADDR + addr) & 0x0007FFFF) | 0xC8000000;
		const uint_fast32_t v = (S1D_PHYSICAL_REG_ADDR + addr) | 0xC0000000uL;

		hardware_spi_b16_p1(v >> 16);
		hardware_spi_b16_p2(v >> 0);
		hardware_spi_b16_p2(0xffff);	// dummy read & read status (8-but value read)
		return hardware_spi_complete_b16() & 0xFF;	// read status

	#else /* WITHSPIEXT16 */
		// 19 bit address
		// 8 bit data read
		spi_progval8_p1(targetlcd, 0xc0);		// 8 bit read
		spi_progval8_p2(targetlcd, 0xff & (S1D_PHYSICAL_REG_ADDR >> 16));
		spi_progval8_p2(targetlcd, 0xff & (S1D_PHYSICAL_REG_ADDR >> 8));
		spi_progval8_p2(targetlcd, addr);
		spi_progval8_p2(targetlcd, 0xff);		// dummy read
		return spi_progval8_p3(targetlcd, 0xff);		// read status register

	#endif /* WITHSPIEXT16 */
}

// Commands:
// 0x80 - 8 bit write
// 0xc0 - 8 bit read
// 0x88 - 16 bit write
// 0xc8 - 16 bit read
static uint_fast16_t set_addr_p1p2_registers_getval16(
	uint_fast8_t addr
	)
{
	#if WITHSPIEXT16
		// 19 bit address
		// 16 bit data read
		//const uint_fast32_t v = ((S1D_PHYSICAL_REG_ADDR + addr) & 0x0007FFFF) | 0xC8000000;
		const uint_fast32_t v = (S1D_PHYSICAL_REG_ADDR + addr) | 0xC8000000UL;

		hardware_spi_b16_p1(v >> 16);
		hardware_spi_b16_p2(v >> 0);
		hardware_spi_b16_p2(0xffff);	// dummy read
		hardware_spi_b16_p2(0xffff);	// read status
		return hardware_spi_complete_b16();	// read status

	#else /* WITHSPIEXT16 */
		// 19 bit address
		// 8 bit data read
		spi_progval8_p1(targetlcd, 0xc0);		// 8 bit read
		spi_progval8_p2(targetlcd, 0xff & (S1D_PHYSICAL_REG_ADDR >> 16));
		spi_progval8_p2(targetlcd, 0xff & (S1D_PHYSICAL_REG_ADDR >> 8));
		spi_progval8_p2(targetlcd, addr);
		spi_progval8_p2(targetlcd, 0xff);		// dummy read

		const uint_fast8_t v = spi_progval8_p3(targetlcd, 0xff);	// low byte
		return spi_read_byte(targetlcd, 0xff) * 256 + v;		// high byte
	#endif /* WITHSPIEXT16 */
}

// используется при заполнении знакогенератора - скорость работы не критична
static void set_data16(
	uint_fast16_t data
	)
{
	#if WITHSPIEXT16
		hardware_spi_b16(data);
	#else /* WITHSPIEXT16 */
		spi_progval8(targetlcd, data >> 0);
		spi_progval8(targetlcd, data >> 8);
	#endif /* WITHSPIEXT16 */
}

// используется при выводе растрового изображения
static void set_data16_p1(
	uint_fast16_t data
	)
{
	#if WITHSPIEXT16
		hardware_spi_b16_p1(data);
	#else /* WITHSPIEXT16 */
		spi_progval8_p1(targetlcd, data >> 0);
		spi_progval8_p2(targetlcd, data >> 8);
	#endif /* WITHSPIEXT16 */
}
// используется при выводе растрового изображения
static void set_data16_p2(
	uint_fast16_t data
	)
{
	#if WITHSPIEXT16
		hardware_spi_b16_p2(data);
	#else /* WITHSPIEXT16 */
		spi_progval8_p2(targetlcd, data >> 0);
		spi_progval8_p2(targetlcd, data >> 8);
	#endif /* WITHSPIEXT16 */
}
// используется при выводе растрового изображения
static void set_data16complete(void)
{
	#if WITHSPIEXT16
		hardware_spi_complete_b16();
	#else /* WITHSPIEXT16 */
		spi_complete(targetlcd);
	#endif /* WITHSPIEXT16 */
}

static void s1d13781_wrcmd8(uint_fast8_t reg, uint_fast8_t val)
{
	s1d13781_select();

	set_addw_p1p2_registers_nc(reg);
	#if WITHSPIEXT16
		hardware_spi_b16_p2(val);
		hardware_spi_complete_b16();
	#else /* WITHSPIEXT16 */
		spi_progval8_p2(targetlcd, val);
		spi_complete(targetlcd);
	#endif /* WITHSPIEXT16 */

	s1d13781_unselect();
}
static void s1d13781_wrcmd16(uint_fast8_t reg, uint_fast16_t val)
{
	s1d13781_select();

	set_addw_p1p2_registers_nc(reg);
	#if WITHSPIEXT16
		hardware_spi_b16_p2(val);
		hardware_spi_complete_b16();
	#else /* WITHSPI16BIT */
		spi_progval8_p2(targetlcd, val >> 0);
		spi_progval8_p2(targetlcd, val >> 8);
		spi_complete(targetlcd);
	#endif /* WITHSPI16BIT */

	s1d13781_unselect();
}

static void s1d13781_wrcmd32(uint_fast8_t reg, uint_fast32_t val)
{
	s1d13781_select();

	set_addw_p1p2_registers_nc(reg);
	#if WITHSPIEXT16
		hardware_spi_b16_p2(val >> 0);
		hardware_spi_b16_p2(val >> 16);
		hardware_spi_complete_b16();
	#else /* WITHSPIEXT16 */
		spi_progval8_p2(targetlcd, val >> 0);
		spi_progval8_p2(targetlcd, val >> 8);
		spi_progval8_p2(targetlcd, val >> 16);
		spi_progval8_p2(targetlcd, val >> 24);
		spi_complete(targetlcd);
	#endif /* WITHSPIEXT16 */

	s1d13781_unselect();
}

// функция передачи двух 16-ти битных слов подряд
static void s1d13781_wrcmd32_pair(uint_fast8_t reg, uint_fast16_t high, uint_fast16_t low)
{
	s1d13781_select();

	set_addw_p1p2_registers_nc(reg);
	#if WITHSPIEXT16
		hardware_spi_b16_p2(low);
		hardware_spi_b16_p2(high);
		hardware_spi_complete_b16();
	#else /* WITHSPIEXT16 */
		spi_progval8_p2(targetlcd, low >> 0);
		spi_progval8_p2(targetlcd, low >> 8);
		spi_progval8_p2(targetlcd, high >> 0);
		spi_progval8_p2(targetlcd, high >> 8);
		spi_complete(targetlcd);
	#endif /* WITHSPIEXT16 */

	s1d13781_unselect();
}

static void s1d13781_wrcmdcolor(uint_fast8_t reg, COLOR_T val)
{
#if S1D_DISPLAY_BPP == 24
	s1d13781_wrcmd32(reg, val);
#elif S1D_DISPLAY_BPP == 16
	s1d13781_wrcmd16(reg, val);		// сделано 16, так как цвет 16-ти битный
#elif S1D_DISPLAY_BPP == 8
	s1d13781_wrcmd8(reg, val);		// сделано 8, так как цвет 8-ти битный
#endif
}

// настройка bitblt на копирование с расширением цветов
static void s1d13781_colorexpand(void)
{
#if S1D_DISPLAY_BPP == 24

	s1d13781_wrcmd16(REG82_BLT_CTRL_1,
			(0x02 << 2)	| // формат дисплейной памяти: 24 bpb
			(0x00 << 1) | // destiantion linear select
			(0x01 << 0) | // source linear select (for chargen array of images required)
			0
		);

#elif S1D_DISPLAY_BPP == 16

	s1d13781_wrcmd16(REG82_BLT_CTRL_1,
			(0x01 << 2)	| // формат дисплейной памяти: 16 bpb
			(0x00 << 1) | // destiantion linear select
			(0x01 << 0) | // source linear select (for chargen array of images required)
			0
		);

#elif S1D_DISPLAY_BPP == 8

	s1d13781_wrcmd16(REG82_BLT_CTRL_1,
			(0x00 << 2)	| // формат дисплейной памяти: 8 bpb
			(0x00 << 1) | // destiantion linear select
			(0x01 << 0) | // source linear select (for chargen array of images required)
			0
		);

#endif
}

static void s1d13781_screencopy(void)
{
#if S1D_DISPLAY_BPP == 24

	s1d13781_wrcmd16(REG82_BLT_CTRL_1,
			(0x02 << 2)	| // формат дисплейной памяти: 24 bpb
			(0x00 << 1) | // destiantion linear select
			(0x00 << 0) | // source linear select
			0
		);

#elif S1D_DISPLAY_BPP == 16

	s1d13781_wrcmd16(REG82_BLT_CTRL_1,
			(0x01 << 2)	| // формат дисплейной памяти: 16 bpb
			(0x00 << 1) | // destiantion linear select
			(0x00 << 0) | // source linear select
			0
		);

#elif S1D_DISPLAY_BPP == 8

	s1d13781_wrcmd16(REG82_BLT_CTRL_1,
			(0x00 << 2)	| // формат дисплейной памяти: 8 bpb
			(0x00 << 1) | // destiantion linear select
			(0x00 << 0) | // source linear select
			0
		);

#endif
}

static uint_fast8_t bitblt_getbusyflag(void)
{
	uint_fast8_t v;

	s1d13781_select();
	v = set_addr_p1p2_registers_getval8(REG84_BLT_STATUS);
	s1d13781_unselect();

	return (v & 0x01) != 0;
}

static uint_fast16_t getprodcode(void)
{
	uint_fast16_t v;

	s1d13781_select();
	v = set_addr_p1p2_registers_getval16(REG02_PROD_CODE);
	s1d13781_unselect();

	return v;
}

static uint_fast8_t s1d13781_missing;	// не-ноль, если дисплей не обнаружен

// дождаться выполнения предидущей команды BitBlt engine.
// если не дождались - возврат 0
static uint_fast8_t
bitblt_waitbusy(void)
{
	if (s1d13781_missing != 0)
		return 0;

	bitblt_getbusyflag();
	bitblt_getbusyflag();
	while (bitblt_getbusyflag() != 0)
		;
	return 1;
}

/* заполнение прямоугольника произвольным цветом с помощью BitBlt engine
   Например, очистка дисплея.
*/
static void 
bitblt_fill(
	uint_fast16_t x, uint_fast16_t y, 	// координаты в пикселях
	uint_fast16_t w, uint_fast16_t h, 	// размеры в пикселях
	COLOR_T color)
{
	// дождаться выполнения предидущей команды BitBlt engine.
	if (bitblt_waitbusy() != 0)
	{
		s1d13781_wrcmdcolor(REG9A_BLT_FGCOLOR_0, color);	// сделано 16, так как цвет 16-ти битный
		s1d13781_colorexpand();

		// PIPMEMSTART
		// вычисление начального адреса в видеопамяти
		const uint_fast32_t dstaddr = S1D_PHYSICAL_VMEM_ADDR + x * (S1D_DISPLAY_BPP / 8) + (uint_fast32_t) y * S1D_DISPLAY_SCANLINE_BYTES;
		//const uint_fast32_t dstaddr = PIPMEMSTART + x * (S1D_DISPLAY_BPP / 8) + (uint_fast32_t) y * S1D_DISPLAY_SCANLINE_BYTES;
		// set destination address
		s1d13781_wrcmd32(REG8C_BLT_DSADDR_0, dstaddr);		// 32 bits of address

		// set bitblt rectangle width and height (pixels) registers.
		s1d13781_wrcmd32_pair(REG92_BLT_WIDTH, h, w);

		s1d13781_wrcmd8(REG86_BLT_CMD, 0x02);	// 0x02 - solid fill
		s1d13781_wrcmd8(REG80_BLT_CTRL_0, 0x01);	// BitBlt start
	}

}

static COLOR_T stored_fgcolor, stored_bgcolor;

static void s1d13781_setcolor(COLOR_T fgcolor, COLOR_T bgcolor)
{
	stored_fgcolor = fgcolor;
	stored_bgcolor = bgcolor;
}


/*	Функция установки курсора в позицию x,y
*/

static uint_fast32_t shadow_dstaddr;



static void s1d13781_gotoxy(
	uint_fast32_t x,	// горизонтальная координата в пикселях
	uint_fast32_t y		// вертикальная координата в пикселях
	)
{
	// PIPMEMSTART
	// вычисление начального адреса в видеопамяти
	shadow_dstaddr = S1D_PHYSICAL_VMEM_ADDR + x * (S1D_DISPLAY_BPP / 8) + y * S1D_DISPLAY_SCANLINE_BYTES;
	//shadow_dstaddr = PIPMEMSTART + x * (S1D_DISPLAY_BPP / 8) + y) * S1D_DISPLAY_SCANLINE_BYTES;
}


static void s1d13781_next_column(
	uint_fast8_t w			// width in pixels of element
	)
{
	shadow_dstaddr += w * (S1D_DISPLAY_BPP / 8);
}

/* заполнение символами с помощью BitBlt engine
*/
static void 
bitblt_chargen_big(
	uint_fast8_t w, 	// до 255 * 255 пикселей -
	uint_fast32_t srcaddr								// откуда брать битмап
	)
{
	// set source address
	s1d13781_wrcmd32(REG88_BLT_SSADDR_0, srcaddr);		// bits of address
	// set bitblt rectangle width and height (pixels) registers.
	s1d13781_wrcmd16(REG92_BLT_WIDTH, w);
	s1d13781_wrcmd8(REG80_BLT_CTRL_0, 0x01);	// BitBlt start
	bitblt_waitbusy();
}

/* заполнение картинкой с помощью BitBlt engine
*/
static void 
bitblt_picture(
	uint_fast16_t w, 	// до 65535 пикселей -
	uint_fast16_t h, 	// до 65535 пикселей -
	uint_fast32_t dstaddr,								// куда копировать битмап
	uint_fast32_t srcaddr								// откуда брать битмап
	)
{
	// дождаться выполнения предидущей команды BitBlt engine.
	if (bitblt_waitbusy() != 0)
	{
		s1d13781_wrcmdcolor(REG96_BLT_BGCOLOR_0, stored_bgcolor);
		s1d13781_wrcmdcolor(REG9A_BLT_FGCOLOR_0, stored_fgcolor);
		s1d13781_colorexpand();
		//s1d13781_wrcmd8(REG80_BLT_CTRL_0, 0x80);	// BitBlt reset
		// set source address
		s1d13781_wrcmd32(REG88_BLT_SSADDR_0, srcaddr);		// bits of address
		s1d13781_wrcmd32(REG8C_BLT_DSADDR_0, dstaddr);		// bits of address
		// set bitblt rectangle width and height (pixels) registers.
		s1d13781_wrcmd32_pair(REG92_BLT_WIDTH, h, w);
		s1d13781_wrcmd8(REG86_BLT_CMD, 0x04);	// 0x04 - move with color expand
		s1d13781_wrcmd8(REG80_BLT_CTRL_0, 0x01);	// BitBlt start
		bitblt_waitbusy();
	}
}

/* копирование содержимого окна с перекрытием для водопада */
void
display_scroll_down(
	uint_fast16_t x,	// левый верзний угол окна
	uint_fast16_t y,	// левый верзний угол окна
	uint_fast16_t w, 	// до 65535 пикселей - ширина окна
	uint_fast16_t h, 	// до 65535 пикселей - высота окна
	uint_fast16_t n		// количество строк прокрутки
	)
{
	enum { WC = (S1D_DISPLAY_BPP / 8) };	// количество байтов на пиксель

	// вычисление начального адреса в видеопамяти
	const uint_fast32_t srcaddr = 
			S1D_PHYSICAL_VMEM_ADDR + 
			(uint_fast32_t) (x + w) * WC + 
			(uint_fast32_t) (y + h - 1 - n) * S1D_DISPLAY_SCANLINE_BYTES;

	// вычисление конечного адреса в видеопамяти
	const uint_fast32_t dstaddr = 
			S1D_PHYSICAL_VMEM_ADDR + 
			(uint_fast32_t) (x + w) * WC + 
			(uint_fast32_t) (y + h - 1) * S1D_DISPLAY_SCANLINE_BYTES;

	// дождаться выполнения предидущей команды BitBlt engine.
	if (bitblt_waitbusy() != 0)
	{
		//s1d13781_wrcmdcolor(REG96_BLT_BGCOLOR_0, stored_bgcolor);
		//s1d13781_wrcmdcolor(REG9A_BLT_FGCOLOR_0, stored_fgcolor);
		s1d13781_screencopy();
		// пересылаем с последнего бацта последнего пикселя окна
		// set source address
		s1d13781_wrcmd32(REG88_BLT_SSADDR_0, srcaddr - 1);		// last byte-alligned address
		s1d13781_wrcmd32(REG8C_BLT_DSADDR_0, dstaddr - 1);		// last byte-alligned address
		// set bitblt rectangle width and height (pixels) registers.
		s1d13781_wrcmd32_pair(REG92_BLT_WIDTH, h - n, w);
		s1d13781_wrcmd8(REG86_BLT_CMD, 0x01);	// 0x01 - move negative
		s1d13781_wrcmd8(REG80_BLT_CTRL_0, 0x01);	// BitBlt start
		bitblt_waitbusy();
	}
}
	
/* копирование содержимого окна с перекрытием для водопада */
void
display_scroll_up(
	uint_fast16_t x,	// левый верзний угол окна
	uint_fast16_t y,	// левый верзний угол окна
	uint_fast16_t w, 	// до 65535 пикселей - ширина окна
	uint_fast16_t h, 	// до 65535 пикселей - высота окна
	uint_fast16_t n		// количество строк прокрутки
	)
{
	enum { WC = (S1D_DISPLAY_BPP / 8) };	// количество байтов на пиксель

	// вычисление начального адреса в видеопамяти
	const uint_fast32_t srcaddr = 
			S1D_PHYSICAL_VMEM_ADDR + 
			(uint_fast32_t) (x) * WC + 
			(uint_fast32_t) (y + n) * S1D_DISPLAY_SCANLINE_BYTES;

	// вычисление конечного адреса в видеопамяти
	const uint_fast32_t dstaddr = 
			S1D_PHYSICAL_VMEM_ADDR + 
			(uint_fast32_t) (x) * WC + 
			(uint_fast32_t) (y + 0) * S1D_DISPLAY_SCANLINE_BYTES;

	// дождаться выполнения предидущей команды BitBlt engine.
	if (bitblt_waitbusy() != 0)
	{
		//s1d13781_wrcmdcolor(REG96_BLT_BGCOLOR_0, stored_bgcolor);
		//s1d13781_wrcmdcolor(REG9A_BLT_FGCOLOR_0, stored_fgcolor);
		s1d13781_screencopy();
		// пересылаем с первого пикселя окна
		// set source address
		s1d13781_wrcmd32(REG88_BLT_SSADDR_0, srcaddr);		// bits of address
		s1d13781_wrcmd32(REG8C_BLT_DSADDR_0, dstaddr);		// bits of address
		// set bitblt rectangle width and height (pixels) registers.
		s1d13781_wrcmd32_pair(REG92_BLT_WIDTH, h - n, w);
		s1d13781_wrcmd8(REG86_BLT_CMD, 0x00);	// 0x00 - move positive
		s1d13781_wrcmd8(REG80_BLT_CTRL_0, 0x01);	// BitBlt start
		bitblt_waitbusy();
	}
}
	


// загрузка в память видеоконтроллера битовых картинок знакогенераторов
// Каждый символ - последовательность битов с младшего бита, каждый ситвол выровнен на 16 бит.

static uint_fast8_t chargen_bitpos;
static uint_fast16_t chargen_bitacc;
static uint_fast32_t chargen_addr;

static void chargen_beginofchar(void)
{
	chargen_bitpos = 0;
	chargen_bitacc = 0;

	s1d13781_select();
	set_addrwr(chargen_addr);	// Sets address for writes and complete spi transfer

}

static void chargen_putbit(uint_fast8_t f)
{
	if (chargen_addr >= S1D_PHYSICAL_VMEM_SIZE)
		return;

	if (f != 0)
	{
		chargen_bitacc |= (1U << chargen_bitpos);
	}

	if (++ chargen_bitpos >= 16)
	{
		set_data16(chargen_bitacc);
		chargen_addr += 2;
		chargen_bitpos = 0;
		chargen_bitacc = 0;
	}	

}


static void chargen_endofchar(void)
{
	if ((chargen_addr < S1D_PHYSICAL_VMEM_SIZE) && (chargen_bitpos != 0))
	{
		set_data16(chargen_bitacc);
		chargen_addr += 2;
		chargen_bitpos = 0;
		chargen_bitacc = 0;
	}	

	s1d13781_unselect();
}

// пробел - 0, узкий пробел - 0. точка - 1
static uint_fast8_t
//NOINLINEAT
narrowfont_decode(uint_fast8_t c)
{
	if (c == ' ' || c == '#')
		return 0;
	return 1;		// точка
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
ascii_decode(uint_fast8_t c)
{
	return c - ' ';
}

static uint_fast32_t narrowchargenbase;	// VA места в памяти видеоконтроллера, где располагается знакогенератор узких символов
static uint_fast32_t narrowchargenstep;	// количество байт на символ в знакогенераторе

static uint_fast32_t bigchargenbase;	// VA места в памяти видеоконтроллера, где располагается знакогенератор больших символов
static uint_fast32_t bigchargenstep;	// количество байт на символ в знакогенераторе

static uint_fast32_t halfchargenbase;	// VA места в памяти видеоконтроллера, где располагается знакогенератор средних символов
static uint_fast32_t halfchargenstep;	// количество байт на символ в знакогенераторе

static uint_fast32_t smallchargenbase;	// VA места в памяти видеоконтроллера, где располагается знакогенератор больших символов
static uint_fast32_t smallchargenstep;	// количество байт на символ в знакогенераторе

static uint_fast32_t smallchargenbase2;	// VA места в памяти видеоконтроллера, где располагается знакогенератор больших символов
static uint_fast32_t smallchargenstep2;	// количество байт на символ в знакогенераторе

static uint_fast32_t scratchbufbase;	// VA места в памяти видеоконтроллера, где располагается буфер монохромного растра

// загрузка в память видеоконтроллера битовых картинок знакогенераторов
// Каждый символ - последовательность битов с младшего бита, каждый ситвол выровнен на 16 бит.
static void loadchargens(void)
{
	chargen_addr = FREEMEMSTART;	// начало свободной памяти в видеоконтроллере
	uint_fast8_t c;	// номер символа, для которого заполняем буфер знакогенератора.

	// выделение в видеопамяти буфера для отрисовки монохромного изображения с последующим отоюражением в соответствии с установленными цветами
	scratchbufbase = chargen_addr;
	chargen_addr += (S1D_DISPLAY_WIDTH + 15) / 16 * 2 * S1D_DISPLAY_HEIGHT;	// размер кратен 16-ти битам.

	// Заполнение знакогенератора зауженных символов ('#' - узкий пробел. Точка всегда узкая. ).
	ASSERT(chargen_addr <= S1D_PHYSICAL_VMEM_SIZE);
	narrowchargenbase = chargen_addr;

	for (c = 0; c < 2; ++ c)
	{
		enum { NHALFS = (sizeof S1D13781_bigfont [0] / sizeof S1D13781_bigfont [0][0]) };
		const uint_fast8_t cv = bigfont_decode(c ? '.' : '#');
		uint_fast32_t a = chargen_addr;
		chargen_beginofchar();
		uint_fast8_t lowhalf;
		for (lowhalf = 0; lowhalf < NHALFS; ++ lowhalf)
		{
			uint_fast8_t cgrow;
			for (cgrow = 0; cgrow < 8; ++ cgrow)
			{
				// NARROWCHARSTARTCOLUMN - начальная колонка в большом знакогенераторе (исходном битмапе в ПЗУ)
				uint_fast8_t i = NARROWCHARSTARTCOLUMN;
				//uint_fast8_t i = (cc == '.' || cc == '#') ? NARROWCHARSTARTCOLUMN : 0;	// начальная колонка знакогенератора, откуда начинать. '#' - узкий пробел. Точка всегда узкая
				//const uint_fast8_t c = bigfont_decode((unsigned char) cc);
				enum { NCOLS = (sizeof S1D13781_bigfont [cv][0] / sizeof S1D13781_bigfont [cv][0][0]) };
				const FLASHMEM uint8_t * p = & S1D13781_bigfont [cv][lowhalf][0];

				for (; i < NCOLS; ++ i)
				{
					const uint_fast8_t v = p [i] & (1U << cgrow);
					chargen_putbit(v);
				}
			}
		}
		chargen_endofchar();
		narrowchargenstep = chargen_addr - a;
	}
	
	// Заполнение знакогенератора больших символов
	ASSERT(chargen_addr <= S1D_PHYSICAL_VMEM_SIZE);
	bigchargenbase = chargen_addr;

	for (c = 0; c < (sizeof S1D13781_bigfont / sizeof S1D13781_bigfont [0]); ++ c)
	{
		enum { NHALFS = (sizeof S1D13781_bigfont [0] / sizeof S1D13781_bigfont [0][0]) };
		uint_fast32_t a = chargen_addr;
		chargen_beginofchar();
		uint_fast8_t lowhalf;
		for (lowhalf = 0; lowhalf < NHALFS; ++ lowhalf)
		{
			uint_fast8_t cgrow;
			for (cgrow = 0; cgrow < 8 && (lowhalf * 8 + cgrow) < BIGCHARH; ++ cgrow)
			{
				uint_fast8_t i = 0;
				//uint_fast8_t i = (cc == '.' || cc == '#') ? NARROWCHARSTARTCOLUMN : 0;	// начальная колонка знакогенератора, откуда начинать. '#' - узкий пробел. Точка всегда узкая
				//const uint_fast8_t c = bigfont_decode((unsigned char) cc);
				enum { NCOLS = (sizeof S1D13781_bigfont [0][0] / sizeof S1D13781_bigfont [0][0][0]) };
				const FLASHMEM uint8_t * p = & S1D13781_bigfont [c][lowhalf][0];

				for (; i < NCOLS; ++ i)
				{
					const uint_fast8_t v = p [i] & (1U << cgrow);
					chargen_putbit(v);
				}
			}
		}
		chargen_endofchar();
		bigchargenstep = chargen_addr - a;
	}

	// Заполнение знакогенератора средних символов
	ASSERT(chargen_addr <= S1D_PHYSICAL_VMEM_SIZE);
	halfchargenbase = chargen_addr;

	for (c = 0; c < (sizeof S1D13781_halffont / sizeof S1D13781_halffont [0]); ++ c)
	{
		enum { NHALFS = (sizeof S1D13781_halffont [0] / sizeof S1D13781_halffont [0][0]) };
		const uint_fast32_t a = chargen_addr;
		chargen_beginofchar();
		uint_fast8_t lowhalf;
		for (lowhalf = 0; lowhalf < NHALFS; ++ lowhalf)
		{
			uint_fast8_t cgrow;
			for (cgrow = 0; cgrow < 8 && (lowhalf * 8 + cgrow) < HALFCHARH; ++ cgrow)
			{
				uint_fast8_t i = 0;
				//uint_fast8_t i = (cc == '.' || cc == '#') ? 12 : 0;	// начальная колонка знакогенератора, откуда начинать. '#' - узкий пробел. Точка всегда узкая
				//const uint_fast8_t c = halffont_decode((unsigned char) cc);
				enum { NCOLS = (sizeof S1D13781_halffont [0][0] / sizeof S1D13781_halffont [0][0] [0]) };
				const FLASHMEM uint8_t * p = & S1D13781_halffont [c][lowhalf][0];

				for (; i < NCOLS; ++ i)
				{
					const uint_fast8_t v = p [i] & (1U << cgrow);
					chargen_putbit(v);
				}
			}
		}
		chargen_endofchar();
		halfchargenstep = chargen_addr - a;
	}

	// Заполнение знакогенератора маленьких символов
	ASSERT(chargen_addr <= S1D_PHYSICAL_VMEM_SIZE);
	smallchargenbase = chargen_addr;

	for (c = 0; c < (0x80 - 0x20); ++ c)
	{
		const uint_fast32_t a = chargen_addr;
		chargen_beginofchar();
		uint_fast8_t cgrow;
		for (cgrow = 0; cgrow < SMALLCHARH; ++ cgrow)
		{
			uint_fast8_t cgcol;
			for (cgcol = 0; cgcol < SMALLCHARW; ++ cgcol)
			{
				//uint_fast8_t i = 0;
				//uint_fast8_t i = (cc == '.' || cc == '#') ? 12 : 0;	// начальная колонка знакогенератора, откуда начинать.
				//const uint_fast8_t c = halffont_decode((unsigned char) cc);
				const FLASHMEM uint8_t * p = & S1D13781_smallfont [c][cgrow][cgcol / 8];

				const uint_fast8_t v = * p & (1U << (cgcol % 8));
				chargen_putbit(v);
			}
		}
		chargen_endofchar();
		smallchargenstep = chargen_addr - a;
	}
#if 0
	ASSERT(chargen_addr <= S1D_PHYSICAL_VMEM_SIZE);
	// Заполнение знакогенератора маленьких "2" символов
	smallchargenbase2 = chargen_addr;

	for (c = 0; c < (0x80 - 0x20); ++ c)
	{
		const uint_fast32_t a = chargen_addr;
		chargen_beginofchar();
		uint_fast8_t cgrow;
		for (cgrow = 0; cgrow < SMALLCHARH; ++ cgrow)
		{
			uint_fast8_t cgcol;
			for (cgcol = 0; cgcol < SMALLCHARW; ++ cgcol)
			{
				//uint_fast8_t i = 0;
				//uint_fast8_t i = (cc == '.' || cc == '#') ? 12 : 0;	// начальная колонка знакогенератора, откуда начинать.
				//const uint_fast8_t c = halffont_decode((unsigned char) cc);
				const FLASHMEM uint8_t * p = & S1D13781_smallfont [c][cgrow][cgcol / 8];

				const uint_fast8_t v = * p & (1U << (cgcol % 8));
				chargen_putbit(v);
			}
		}
		chargen_endofchar();
		smallchargenstep2 = chargen_addr - a;
	}
#endif
	
	ASSERT(chargen_addr <= S1D_PHYSICAL_VMEM_SIZE);
	if (chargen_addr > S1D_PHYSICAL_VMEM_SIZE)
	{
		for (;;)
			;
	}
}

static uint_fast32_t getnarrowcharbase(
	char cc	// символ для отображения
	)
{
	return narrowchargenbase + narrowfont_decode(cc) * narrowchargenstep;
}

static uint_fast32_t getbigcharbase(
	char cc	// символ для отображения
	)
{
	return bigchargenbase + bigfont_decode(cc) * bigchargenstep;
}

static uint_fast32_t gethalfcharbase(
	char cc	// символ для отображения
	)
{
	return halfchargenbase + bigfont_decode(cc) * halfchargenstep;
}

static uint_fast32_t getsmallcharbase(
	char cc	// символ для отображения
	)
{
	return smallchargenbase + ascii_decode(cc) * smallchargenstep;
}

static uint_fast32_t getsmallcharbase2(
	char cc	// символ для отображения
	)
{
	return smallchargenbase2 + ascii_decode(cc) * smallchargenstep2;
}





// загрузка таблицы цветов для
// RRRGGGBB
// В остальных режимах работы просто бредит.

static void
loadlut(
	uint_fast32_t addr		// LUT base address
	)
{
#if S1D_DISPLAY_BPP == 8
	uint_fast16_t color;

	s1d13781_select();
	set_addrwr(addr);	// Sets address for writes and complete spi transfer

	for (color = 0; color < S1D_PALETTE_SIZE; ++ color)
	{
#define ARGB(zr,zg,zb) do { r = (zr), g = (zg), b = (zb); } while (0)
		uint_fast8_t r, g, b;

		switch (color)
		{
		case COLOR_BLACK:			ARGB(0, 0, 0);			break;	// 0x00 черный
		case COLOR_RED:		ARGB(255, 0, 0);		break; 	// 0xE0 красный
		case COLOR_GREEN:			ARGB(0, 255, 0);		break; 	// 0x1C зеленый
		case COLOR_BLUE:			ARGB(0, 0, 255);		break; 	// 0x03 синий
		case COLOR_DARKRED:		ARGB(128, 0, 0);		break; 	// 
		case COLOR_DARKGREEN:		ARGB(0, 128, 0);		break; 	// 
		case COLOR_DARKBLUE:		ARGB(0, 0, 128);		break; 	// 
		case COLOR_YELLOW:		ARGB(255, 255, 0);		break; 	// 0xFC желтый
		case COLOR_MAGENTA:		ARGB(255, 0, 255);		break; 	// 0x83 пурпурный
		case COLOR_CYAN:			ARGB(0, 255, 255);		break; 	// 0x1F голубой
		case COLOR_WHITE:	ARGB(255, 255, 255);	break;  // 0xff	белый
		case COLOR_GRAY:			ARGB(128, 128, 128);	break; 	// серый
		case COLOR_BROWN:			ARGB(0xa5, 0x2a, 0x2a);	break; 	// 0x64 коричневый
		case COLOR_GOLD:			ARGB(0xff, 0xd7, 0x00);	break; 	// 0xF4 золото
		case COLOR_PEAR:			ARGB(0xd1, 0xe2, 0x31);	break; 	// 0xDC грушевый
#undef ARGB
		default:
			r = ((color & 0xe0) << 0) | ((color & 0x80) ? 0x1f : 0);	// red
			g = ((color & 0x1c) << 3) | ((color & 0x10) ? 0x1f : 0);	// green
			b = ((color & 0x03) << 6) | ((color & 0x02) ? 0x3f : 0);	// blue
			break;
		}
		/* запись значений в регистры палитры */
		#if WITHSPIEXT16
			hardware_spi_b16_p1((g << 8) | b);
			hardware_spi_b16_p2(r);
			hardware_spi_complete_b16();
		#else /* WITHSPIEXT16 */
			spi_progval8_p1(targetlcd, b);
			spi_progval8_p2(targetlcd, g);
			spi_progval8_p2(targetlcd, r);
			spi_progval8_p2(targetlcd, 0);		// dummy byte for align
			spi_complete(targetlcd);			// dummy byte for align
		#endif /* WITHSPIEXT16 */
	}
	s1d13781_unselect();
#endif /* S1D_DISPLAY_BPP == 8 */
}


static void
bitblt_setdstaddr(
	uint_fast32_t addr		/* адрес в памяти для заполнения с помощью BitBlt engine */
	)
{
	s1d13781_wrcmd32(REG8C_BLT_DSADDR_0, addr);		// bits of address
}


// Установка постоянных параметров для BitBlt при отображении строки
// маленьких символов
static void s1d13781_put_char_begin(void)
{
	// дождаться выполнения предидущей команды BitBlt engine.
	if (bitblt_waitbusy() != 0)
	{
		s1d13781_wrcmdcolor(REG96_BLT_BGCOLOR_0, stored_bgcolor);
		s1d13781_wrcmdcolor(REG9A_BLT_FGCOLOR_0, stored_fgcolor);
		s1d13781_colorexpand();
		// todo: разобраться с ошибкой в формате знакогенератора
		s1d13781_wrcmd32_pair(REG92_BLT_WIDTH, SMALLCHARHEIGHT, SMALLCHARWIDTH);	// set bitblt rectangle width and height (pixels) registers.
		//s1d13781_wrcmd32_pair(REG92_BLT_WIDTH, 15, 16);	// set bitblt rectangle width and height (pixels) registers.
		s1d13781_wrcmd8(REG86_BLT_CMD, 0x04);	// команда для bitblt - 0x04 - move with color expand
	}
}

// Установка постоянных параметров для BitBlt при отображении строки
// маленьких символов
static void s1d13781_put_char_begin2(void)
{
	// дождаться выполнения предидущей команды BitBlt engine.
	if (bitblt_waitbusy() != 0)
	{
		s1d13781_wrcmdcolor(REG96_BLT_BGCOLOR_0, stored_bgcolor);
		s1d13781_wrcmdcolor(REG9A_BLT_FGCOLOR_0, stored_fgcolor);
		s1d13781_colorexpand();
		s1d13781_wrcmd32_pair(REG92_BLT_WIDTH, SMALLCHARHEIGHT2, SMALLCHARWIDTH2);	// set bitblt rectangle width and height (pixels) registers.
		s1d13781_wrcmd8(REG86_BLT_CMD, 0x04);	// команда для bitblt - 0x04 - move with color expand
	}
}

/* заполнение символами с помощью BitBlt engine
   Версия функции для фиксированного прямоугольника - размер устанавливается
   перед началом выдачи последовательности символов
*/
static void 
bitblt_chargen_small(
	uint_fast32_t srcaddr								// откуда брать битмап
	)
{
	// set source address
	s1d13781_wrcmd32(REG88_BLT_SSADDR_0, srcaddr);		// bits of address
	s1d13781_wrcmd8(REG80_BLT_CTRL_0, 0x01);	// BitBlt start
	bitblt_waitbusy();
}


// Установка постоянных параметров для BitBlt при отображении строки
// больших символов
static void s1d13781_put_charbig_begin(void)
{
	// дождаться выполнения предидущей команды BitBlt engine.
	if (bitblt_waitbusy() != 0)
	{
		s1d13781_wrcmdcolor(REG96_BLT_BGCOLOR_0, stored_bgcolor);
		s1d13781_wrcmdcolor(REG9A_BLT_FGCOLOR_0, stored_fgcolor);
		s1d13781_colorexpand();
		s1d13781_wrcmd16(REG94_BLT_HEIGHT, BIGCHARHEIGHT);
		s1d13781_wrcmd8(REG86_BLT_CMD, 0x04);	// команда для bitblt - 0x04 - move with color expand
	}
}

// Вызов этой функции только внутри s1d13781_put_char_begin() и s1d13781_put_char_end();
static void s1d13781_put_char_small(char cc)
{
	// дождаться выполнения предидущей команды BitBlt engine.
	if (bitblt_waitbusy() != 0)
	{
		bitblt_setdstaddr(shadow_dstaddr);
		bitblt_chargen_small(getsmallcharbase(cc));
		s1d13781_next_column(CHAR_W);
	}
}

// Вызов этой функции только внутри s1d13781_put_char_begin() и s1d13781_put_char_end();
static void s1d13781_put_char_small2(char cc)
{
	// дождаться выполнения предидущей команды BitBlt engine.
	if (bitblt_waitbusy() != 0)
	{
		//s1d13781_wrcmd8(REG80_BLT_CTRL_0, 0x80);	// BitBlt reset
		bitblt_setdstaddr(shadow_dstaddr);
		bitblt_chargen_small(getsmallcharbase2(cc));
		s1d13781_next_column(SMALLCHARWIDTH2);
	}
}

// Вызов этой функции только внутри display_wrdata_begin() и 	display_wrdata_end();

static void s1d13781_put_char_big(char cc)
{
	// дождаться выполнения предидущей команды BitBlt engine.
	if (bitblt_waitbusy() != 0)
	{
		bitblt_setdstaddr(shadow_dstaddr);

		// '#' - узкий пробел
		if (cc == '.' || cc == '#')
		{
			const unsigned NARROWCHARWIDTH = (BIGCHARWIDTH - NARROWCHARSTARTCOLUMN);
			bitblt_chargen_big(NARROWCHARWIDTH, getnarrowcharbase(cc));
			s1d13781_next_column(NARROWCHARWIDTH);
		}
		else
		{
			bitblt_chargen_big(BIGCHARWIDTH, getbigcharbase(cc));
			s1d13781_next_column(BIGCHARWIDTH);
		}
	}
}

// Вызов этой функции только внутри display_wrdatabig_begin() и 	display_wrdatabig_end();

static void s1d13781_put_char_half(char cc)
{
	// дождаться выполнения предидущей команды BitBlt engine.
	if (bitblt_waitbusy() != 0)
	{
		bitblt_setdstaddr(shadow_dstaddr);
		bitblt_chargen_big(HALFCHARWIDTH, gethalfcharbase(cc));
		s1d13781_next_column(HALFCHARWIDTH);
	}
}

// рисование незаполненного прямоугольника
static void rectangle(
	uint_fast16_t x,
	uint_fast16_t y,
	uint_fast16_t w,
	uint_fast16_t h,
	COLOR_T color1,
	COLOR_T color2
	)
{
	enum { thickness = 1 };

	bitblt_fill(x, y, w, thickness, color1);			// horizontal top line
	bitblt_fill(x, y + h - thickness, w, thickness, color1);	// horizontal bottom line
	bitblt_fill(x, y + thickness, thickness, h - 2 * thickness, color1);	// vertical left line
	bitblt_fill(x + w - thickness, y + thickness, thickness, h - 2 * thickness, color1);	// vertical right line
	bitblt_fill(x + thickness, y + thickness, w - 2 * thickness, h - 2 * thickness, color2);							// central panel
}

// рисование 3D прямоугольника
static void rectangle3d(
	uint_fast16_t x,
	uint_fast16_t y,
	uint_fast16_t w,
	uint_fast16_t h,
	COLOR_T color1,
	COLOR_T color2
	)
{
	enum { thickness = 2 };
	enum { state = 0 };

	const uint_fast16_t wi = w - 2 * thickness;		// внутренняя ширина
	const uint_fast16_t hi = h - 2 * thickness;		// внутренняя высота
	const uint_fast16_t x1 = x + thickness;			// коордитата внутреннего угла
	const uint_fast16_t x2 = x + thickness + wi;	// коордитата внутреннего угла
	const uint_fast16_t y1 = y + thickness;			// коордитата внутреннего угла
	const uint_fast16_t y2 = y + thickness + hi;	// коордитата внутреннего угла
	
	if (state != 0)
	{
		// нажатое состояние
	}
	else
	{
		// отпущенное состояние
		bitblt_fill(x, y, w, thickness, color1);						// horizontal top line
		bitblt_fill(x1, y2, w - thickness, thickness, color2);			// horizontal bottom line
		bitblt_fill(x, y + thickness, thickness, h - thickness, color1);	// vertical left line
		bitblt_fill(x2, y1, thickness, h - thickness, color2);			// vertical right line
		//bitblt_fill(x1, y1, wi, hi, color1);							// central panel
	}
}

uint_fast8_t
display_getreadystate(void)
{
	return bitblt_getbusyflag() == 0;
}

static void display_putpixel(
	uint_fast16_t x,
	uint_fast16_t y,
	COLOR_T color
	)
{
	// вычисление начального адреса в видеопамяти
	const uint_fast32_t dstaddr = S1D_PHYSICAL_VMEM_ADDR + x * (S1D_DISPLAY_BPP / 8) + (uint_fast32_t) y * S1D_DISPLAY_SCANLINE_BYTES;


#if S1D_DISPLAY_BPP == 24

	spi_select2(targetlcd, S1D13781_SPIMODE, S1D13781_SPIC_SPEED);	/* Enable SPI */
	set_addrwr_8bit_p1p2_nc(dstaddr);
	spi_progval8_p2(targetlcd, color >> 0);
	spi_progval8_p2(targetlcd, color >> 8);
	spi_progval8_p2(targetlcd, color >> 16);
	spi_complete(targetlcd);
	spi_unselect(targetlcd);			// или в байтовом (8-битном) режиме.

#elif S1D_DISPLAY_BPP == 16

	#if WITHSPIEXT16

		hardware_spi_connect_b16(S1D13781_SPIC_SPEED, S1D13781_SPIMODE);		// если есть возможность - работаем в 16-ти битном режиме
		prog_select(targetlcd);
		set_addw_16bit_p1p2_nc(dstaddr);
		hardware_spi_b16_p2(color);
		hardware_spi_complete_b16();
		prog_unselect(targetlcd);
		hardware_spi_disconnect();

	#else /* WITHSPI16BIT */

		spi_select2(targetlcd, S1D13781_SPIMODE, S1D13781_SPIC_SPEED);	/* Enable SPI */
		set_addrwr_8bit_p1p2_nc(dstaddr);
		spi_progval8_p2(targetlcd, color >> 0);
		spi_progval8_p2(targetlcd, color >> 8);
		spi_complete(targetlcd);
		spi_unselect(targetlcd);

	#endif /* WITHSPI16BIT */

#elif S1D_DISPLAY_BPP == 8

	spi_select2(targetlcd, S1D13781_SPIMODE, S1D13781_SPIC_SPEED);	/* Enable SPI */
	set_addrwr_8bit_p1p2_nc(dstaddr);
	spi_progval8_p2(targetlcd, color >> 0);
	spi_complete(targetlcd);
	spi_unselect(targetlcd);

#endif

}

static void display_putpixel_1(
	COLOR_T color
	)
{
#if S1D_DISPLAY_BPP == 24

	spi_progval8_p1(targetlcd, color >> 0);
	spi_progval8_p2(targetlcd, color >> 8);
	spi_progval8_p2(targetlcd, color >> 16);

#elif S1D_DISPLAY_BPP == 16

	#if WITHSPIEXT16

		hardware_spi_b16_p1(color);

	#else /* WITHSPI16BIT */

		spi_progval8_p1(targetlcd, color >> 0);
		spi_progval8_p2(targetlcd, color >> 8);

	#endif /* WITHSPI16BIT */

#elif S1D_DISPLAY_BPP == 8

	spi_progval8_p1(targetlcd, color >> 0);

#endif

}

static void display_putpixel_2(
	COLOR_T color
	)
{
#if S1D_DISPLAY_BPP == 24

	spi_progval8_p2(targetlcd, color >> 0);
	spi_progval8_p2(targetlcd, color >> 8);
	spi_progval8_p2(targetlcd, color >> 16);

#elif S1D_DISPLAY_BPP == 16

	#if WITHSPIEXT16

		hardware_spi_b16_p2(color);

	#else /* WITHSPI16BIT */

		spi_progval8_p2(targetlcd, color >> 0);
		spi_progval8_p2(targetlcd, color >> 8);

	#endif /* WITHSPI16BIT */

#elif S1D_DISPLAY_BPP == 8

	spi_progval8_p2(targetlcd, color >> 0);

#endif

}

static void display_putpixel_complete(void)
{
#if S1D_DISPLAY_BPP == 24

	spi_complete(targetlcd);

#elif S1D_DISPLAY_BPP == 16

	#if WITHSPIEXT16

		hardware_spi_complete_b16();

	#else /* WITHSPI16BIT */

		spi_complete(targetlcd);

	#endif /* WITHSPI16BIT */

#elif S1D_DISPLAY_BPP == 8

	spi_complete(targetlcd);

#endif
}




/*-----------------------------------------------------  V_Bre
 * void V_Bre (int xn, int yn, int xk, int yk)
 *
 * Подпрограмма иллюстрирующая построение вектора из точки
 * (xn,yn) в точку (xk, yk) методом Брезенхема.
 *
 * Построение ведется от точки с меньшими  координатами
 * к точке с большими координатами с единичным шагом по
 * координате с большим приращением.
 *
 * В общем случае исходный вектор проходит не через вершины
 * растровой сетки, а пересекает ее стороны.
 * Пусть приращение по X больше приращения по Y и оба они > 0.
 * Для очередного значения X нужно выбрать одну двух ближайших
 * координат сетки по Y.
 * Для этого проверяется как проходит  исходный  вектор - выше
 * или ниже середины расстояния между ближайшими значениями Y.
 * Если выше середины,  то Y-координату  надо  увеличить на 1,
 * иначе оставить прежней.
 * Для этой проверки анализируется знак переменной s,
 * соответствующей разности между истинным положением и
 * серединой расстояния между ближайшими Y-узлами сетки.
 */

static void display_line(int xn, int yn, int xk, int yk, COLOR_T color)
{  
	int  dx, dy, s, sx, sy, kl, incr1, incr2;
	char swap;

	/* Вычисление приращений и шагов */
	sx = 0;
	if ((dx= xk-xn) < 0) 
	{
		dx = - dx;
		-- sx;
	} 
	else if (dx > 0) 
		++ sx;
	sy = 0;

	if ((dy= yk-yn) < 0) 
	{
		dy = - dy; 
		-- sy;
	} 
	else if (dy>0) 
		++ sy;
	/* Учет наклона */
	swap = 0;
	if ((kl= dx) < (s= dy)) 
	{
		dx= s;  dy= kl;  kl= s; ++swap;
	}
	s = (incr1= 2 * dy) - dx; /* incr1 - констан. перевычисления */
	/* разности если текущее s < 0  и  */
	/* s - начальное значение разности */
	incr2 = 2 * dx;         /* Константа для перевычисления    */
	/* разности если текущее s >= 0    */
	display_putpixel(xn, yn, color); /* Первый  пиксел вектора       */

	while (--kl >= 0) 
	{
		if (s >= 0) 
		{
			if (swap) xn+= sx; 
			else yn+= sy;
			s-= incr2;
		}
		if (swap) 
			yn+= sy; 
		else 
			xn+= sx;
		s += incr1;
		display_putpixel(xn, yn, color); /* Текущая  точка  вектора   */
	}
}  /* V_Bre */


static const int sin90 [91] =
{          
	0, 175, 349, 523, 698, 872,1045,1219,1392,   /*  0..8        */
	1564,1736,1908,2079,2250,2419,2588,2756,2924,   /*  9..17       */
	3090,3256,3420,3584,3746,3907,4067,4226,4384,   /* 18..26       */
	4540,4695,4848,5000,5150,5299,5446,5592,5736,
	5878,6018,6157,6293,6428,6561,6691,6820,6947,
	7071,7193,7314,7431,7547,7660,7771,7880,7986,
	8090,8192,8290,8387,8480,8572,8660,8746,8829,
	8910,8988,9063,9135,9205,9272,9336,9397,9455,
	9511,9563,9613,9659,9703,9744,9781,9816,9848,
	9877,9903,9925,9945,9962,9976,9986,9994,9998,   /* 81..89       */
	10000                                           /* 90           */
};


static int muldiv(int a, int b, unsigned c)
{       
	return  (unsigned) ((a * (long) b + 5000) / c);
}

static  int isin(unsigned alpha, unsigned r)
{       
	while (alpha >= 360)
		alpha -= 360;

	if (alpha < 90)         /* 0..3 hours   */
		return muldiv(sin90 [ alpha ], r, 10000);
	if (alpha < 180)        /* 9..0 hours   */
		return muldiv(sin90 [ 180 - alpha ], r, 10000);
	if (alpha < 270)        /* 6..9 hours   */
		return - muldiv(sin90 [ alpha - 180], r, 10000);
				/* 3..6 hours   */
	return - muldiv(sin90 [ 360 - alpha ], r, 10000);
}

static  int icos(unsigned alpha, unsigned r)
{
	return isin(alpha + 90, r * 20 / 10);
}


// Рисование радиусов
static void
display_radius(int xc, int yc, unsigned gs, unsigned r1, unsigned r2, COLOR_T color)
{
	int     x, y;
	int     x2, y2;

	x = xc + icos(gs, r1);
	y = yc + isin(gs, r1);
	x2 = xc + icos(gs, r2);
	y2 = yc + isin(gs, r2);

	display_line(x, y, x2, y2, color);

}

// круговой интерполятор
// нач.-x, нач.-y, градус начала, градус конуа, радиус, шаг приращения угла
static void
display_segm(int xc, int yc, unsigned gs, unsigned ge, unsigned r, int step, COLOR_T color)
{
	int     x, y;
	int     xo, yo;
	char     first;
	int     vcos, vsin;

	if (gs == ge)   return;
	first = 1;
	while (gs != ge)
	{
		vsin = isin(gs, r);
		vcos = icos(gs, r);
		x = xc + vcos;
		y = yc + vsin;

		if (first != 0) // 1-я точка
		{
			// переместить к началу рисования
			xo = x, yo = y;
			first = 0;
		}
		else
		{  // рисовать элемент окружности
			display_line(xo, yo, x, y, color);
			xo = x, yo = y;
		}
		if (ge == 360)  
			ge = 0;
		if (step < 0)
		{
			gs += step;
			if (gs >= 360)
				gs += 360;
		}
		else
		{
			gs += step;
			if (gs >= 360)
				gs -= 360;
		}
	}

	if (first == 0)
	{
		// завершение окружности
		vsin = isin(ge, r);
		vcos = icos(ge, r);
		x = xc + vcos;
		y = yc + vsin;

		display_line(xo, yo, x, y, color); // рисовать линию
	}

}



static void
display_limb(void)
{
	enum { ADDRCELLHEIGHT = 15 };
	int i;
	enum { halfsect = 30 };
	enum { gm = 270 };
	enum { gs = gm - halfsect };
	int ge = gm + halfsect;
	int stripewidth = 12; //16;
	int r1 = 7 * ADDRCELLHEIGHT - 8;	//350;
	int r2 = r1 - stripewidth;
	int yc = 9 * ADDRCELLHEIGHT;	//560;
	int xc = 9 * ADDRCELLHEIGHT;	//120;
	
	int gv = 270 - 20;		// угол поворота стрелки
	int rv1 = 8 * ADDRCELLHEIGHT;	//350;
	int rv2 = rv1 - 5 * ADDRCELLHEIGHT;
	enum { step1 = 3 };		// шаг для оцифровки S
	enum { step2 = 4 };		// шаг для оцифровки плюсов
	static const int markers [] =
	{
		//gs + 0 * step1,
		gs + 2 * step1,		// S1
		gs + 4 * step1,		// S3
		gs + 6 * step1,		// S5
		gs + 8 * step1,		// S7
		gs + 10 * step1,	// S9
	};
	static const int markersR [] =
	{
		gm + 2 * step2,	// 
		gm + 4 * step2,
		gm + 6 * step2,
	};
	static const int markers2 [] =
	{
		//gs + 1 * step1,		
		gs + 3 * step1,		// S2	
		gs + 5 * step1,		// S4
		gs + 7 * step1,		// S6
		gs + 9 * step1,		// S8
	};
	static const int markers2R [] =
	{
		gm + 1 * step2,
		gm + 3 * step2,
		gm + 5 * step2,
	};

	const COLOR_T smeter = COLOR_WHITE;
	const COLOR_T smeterplus = COLOR_GRAY;

	if (bitblt_waitbusy())	// перед рисованием прямым доступом к видеопамяти дождаться конца работы BitBlt
	{
		//s1d13781_wrcmd8(REG80_BLT_CTRL_0, 0x80);	// BitBlt reset

		display_segm(xc, yc, gs, gm, r1, 1, smeter);
		display_segm(xc, yc, gm, ge, r1, 1, smeterplus);

		display_segm(xc, yc, gs, ge, r2, 1, COLOR_WHITE);		// POWER
		//display_radius(xc, yc, gs, r1, r2, COLOR_RED);
		//display_radius(xc, yc, ge, r1, r2, COLOR_RED);
		for (i = 0; i < sizeof markers / sizeof markers [0]; ++ i)
		{
			display_radius(xc, yc, markers [i], r1, r1 + 8, smeter);
		}
		for (i = 0; i < sizeof markers2 / sizeof markers2 [0]; ++ i)
		{
			display_radius(xc, yc, markers2 [i], r1, r1 + 4, smeter);
		}
		for (i = 0; i < sizeof markersR / sizeof markersR [0]; ++ i)
		{
			display_radius(xc, yc, markersR [i], r1, r1 + 8, smeterplus);
		}
		for (i = 0; i < sizeof markers2R / sizeof markers2R [0]; ++ i)
		{
			display_radius(xc, yc, markers2R [i], r1, r1 + 4, smeterplus);
		}

		//display_segm(xc, yc, gs, ge, rv1, 1, BLUE);	// показывает зоны перемещения стрелки
		//display_segm(xc, yc, gs, ge, rv2, 1, BLUE);	// показывает зоны перемещения стрелки
		display_radius(xc - 2, yc, gv, rv1, rv2, COLOR_RED);
		display_radius(xc - 1, yc, gv, rv1, rv2, COLOR_RED);
		display_radius(xc, yc, gv, rv1, rv2, COLOR_RED);
		display_radius(xc + 1, yc, gv, rv1, rv2, COLOR_RED);
		display_radius(xc + 2, yc, gv, rv1, rv2, COLOR_RED);
	}
}


// val = 0..S1D_DISPLAY_HEIGHT
void drawfftbar(int x, int val, int maxval)
{
	const int wb = 2;
	const int xb = 50;
	const int hb = 50;
	const int he = 100;
	const int v = (uint_fast64_t) val * he / maxval;
	bitblt_fill(xb + x * wb, hb, wb, he - v,	COLOR_GRAY);	// inactive part
	bitblt_fill(xb + x * wb, hb + he - v, wb, v, COLOR_GREEN);	// active part
}

static void s1d13781_clear(COLOR_T bg)
{
	bitblt_fill(0, 0, S1D_DISPLAY_WIDTH, S1D_DISPLAY_HEIGHT, bg);
	s1d13781_setcolor(COLOR_WHITE, bg);
}

static void 
s1d13781_pip_init(void)
{
	s1d13781_wrcmd16(REG50_PIP_SET, 0x0006 | S1D13781_SETFLAGS); // 8 bpp+LUT2
	s1d13781_wrcmd32(REG52_PIP_SADDR_0, PIPMEMSTART); 
	s1d13781_wrcmd16(REG56_PIP_WIDTH, S1D_DISPLAY_WIDTH); 
	s1d13781_wrcmd16(REG58_PIP_HEIGHT, S1D_DISPLAY_HEIGHT); 
	s1d13781_wrcmd32_pair(REG5A_PIP_XSTART, 0, 0);		// Y, X, где в основном окнео отображается  PIP

	s1d13781_wrcmd16(REG62_ALPHA, 0x0040);		// 0x40 - сквозь PIP не видно MAIN, 0x00 - только MAIN

	s1d13781_wrcmd16(REG64_TRANS, 0x0000);		// 0x01 - transparency enabled 
	s1d13781_wrcmd32(REG66_KEY_0, 0xffd700UL);	// GOLD, key color (uncompresssed - after LUT2)	цвет в распакованном виде

	s1d13781_wrcmd16(REG60_PIP_EN, 0x0000);		// 0x01 - normal, 0x00 - off
}

#if ! defined (LCDMODE_S1D13781_REFOSC_MHZ)
	#define LCDMODE_S1D13781_REFOSC_MHZ	25
#endif /* ! defined (LCDMODE_S1D13781_REFOSC_MHZ) */

static void s1d13781_initialize(void)
{
	/* параметры PLL и тактирования панели. */
	enum
	{
		PLLDIV = LCDMODE_S1D13781_REFOSC_MHZ,	// делитель опорной частоты
		PLLMUL = 63,			// множитель опорной частоты
		CLKDIV = 7				// деление рабочей частоты для получения тактовой частоты для панели
	};

	/* 13781 init data file */
	const static struct {
		uint8_t reg;
		uint16_t val;
	} FLASHMEM initdata [] =
    {  
        { REG06_RESET,                          0x0100 },

        { REGFLAG_ON_DELAY,                     0x2710 },

        { REG04_POWER_SAVE,                     0x0000 },
        { REG10_PLL_0,                          0x0000 },
        { REG12_PLL_1,                          PLLDIV - 1 },	// divide - 1
        { REG14_PLL_2,                          PLLMUL - 1 },	// multiple - 1
        { REG10_PLL_0,                          0x0001 },

        { REGFLAG_DELAY,                        0x09C4 },

        { REG16_INTCLK,                         CLKDIV - 1 },	// clkdiv - 1
        { REG04_POWER_SAVE,                     0x0002 },
        { REG20_PANEL_SET,                      0x00CD },		// 18 bit panel, DE fixed high
        { REG22_DISP_SET,                       0x0001 | 0x0100 },	// TE pin disable

        { REG24_HDISP,                          0x003C },
        { REG26_HNDP,                           0x002D },
        { REG28_VDISP,                          0x0110 },
        { REG2A_VNDP,                           0x000E },
        { REG2C_HSW,                            0x0029 },
        { REG2E_HPS,                            0x0002 },
        { REG30_VSW,                            0x000A },
        { REG32_VPS,                            0x0002 },

        { REG42_MAIN_SADDR_0,                   0xffff & (S1D_PHYSICAL_VMEM_ADDR >> 0) },
        { REG44_MAIN_SADDR_1,                   0xffff & (S1D_PHYSICAL_VMEM_ADDR >> 16) },

		// Инициализация BitBlt engine постоянными параметрами
		{ REG90_BLT_RECTOFFSET,				S1D_DISPLAY_WIDTH },

#if S1D_DISPLAY_BPP == 24

        { REG40_MAIN_SET,                       0x0000 | S1D13781_SETFLAGS },

#elif S1D_DISPLAY_BPP == 16

        { REG40_MAIN_SET,                       0x0001 | S1D13781_SETFLAGS },	// 0/180 deg. rotation 

#elif S1D_DISPLAY_BPP == 8

        { REG40_MAIN_SET,                       0x0006 | S1D13781_SETFLAGS },

#endif

        { REGD0_GPIO_CONFIG,                    0x0001 },
        { REGD2_GPIO_STATUS,                    0x0001 },
        { REGD4_GPIO_PULLDOWN,                  0x0000 },

		{ REG80_BLT_CTRL_0,						0x0080 },	// BitBlt reset

    };


	uint_fast8_t i;
	for (i = 0; i < (sizeof initdata / sizeof initdata [0]); ++ i)  
	{ 
		const uint_fast8_t reg = initdata [i].reg;
		const uint_fast16_t value = initdata [i].val;

		switch (reg)
		{
		case REGFLAG_OFF_DELAY:
			local_delay_ms(10);
			break;
		case REGFLAG_ON_DELAY:
			local_delay_ms(250);
			break;
		case REGFLAG_DELAY:
			local_delay_ms(3);
			break;

		default:
			s1d13781_wrcmd16(reg, value);
			break;
		};
	}

	// подождать один кадр или больше
	local_delay_ms(250);	

	loadlut(0x60000UL);				// LUT1 загрузка таблицы цветов
	loadlut(0x60400UL);				// LUT2 загрузка таблицы цветов

	loadchargens();

	s1d13781_pip_init();


	//s1d13781_clear();

	//for (;;)
	//	;
	s1d13781_setcolor(COLOR_WHITE, display_getbgcolor());
}





/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{
	hardware_spi_master_setfreq(SPIC_SPEED10M, 10000000uL);
	hardware_spi_master_setfreq(SPIC_SPEED25M, 25000000uL);

	const uint_fast16_t prodcode = getprodcode();
	debug_printf_P(PSTR("display_initialize: product code = 0x%04x (expected 0x0050)\n"), (unsigned) prodcode);
	if (prodcode != 0x0050)
	{
		s1d13781_missing = 1;
	}
	s1d13781_initialize();
}

void display_set_contrast(uint_fast8_t v)
{
}

static int vesa_getmaxx(void)
{
	return DIM_X - 1;
}
static int vesa_getmaxy(void)
{
	return DIM_Y - 1;
}


static void graph_bar(int x, int y, int x2, int y2, COLOR_T color)
{
	if (x2 < x)
	{
		int t = x;
		x = x2, x2 = t;
	}
	if (y2 < y)
	{
		int t = y;
		y = y2, y2 = t;
	}
	bitblt_fill(x, y, x2 - x, y2 - y, color);
}


static  void
GrideTest(void)
{
	int     xm, ym, xm4, ym4;
	int xm1, ym1;
	unsigned long col1, col20, col21, col22, col23, col3;
	int     n, k;

	col1 = TFTRGB(192,192,192);

	col20 = TFTRGB(64,128,128);
	col21 = TFTRGB(128,64,128);
	col22 = TFTRGB(128,128,64);
	col23 = TFTRGB(64,64,64);

	col3 = TFTRGB(0,192,192);


	xm = vesa_getmaxx();
	ym = vesa_getmaxy();
	xm4 = xm / 4;
	ym4 = ym / 4;
	xm1 = xm / 40;
	ym1 = ym / 40;

	/* Filled rectangle - all screen. */
	graph_bar(0, 0, xm, ym, col1);

	/* Filled rectangle at right-down corner. */
	graph_bar(xm4 * 3 + xm1, ym4 * 3 + ym1, xm4 * 4 - xm1, ym4 * 4 - ym1, col20);
	/* Filled rectangle at right-upper corner. */
	graph_bar(xm4 * 3 + xm1, ym1, xm4 * 4 - xm1, ym4 - ym1, col21);
	/* Filled rectangle at left - down corner. */
	graph_bar(xm1, ym4 * 3 + ym1, xm4 - xm1, ym4 * 4 - ym1, col22);
	/* Filled rectangle at center. */
	graph_bar(xm4 + xm1, ym4 + ym1, xm4 * 3 - xm1, ym4 * 3 - ym1, col23);

	for (k = 0; k < 16; ++ k)
		for (n = 0; n < 16; ++ n)
			graph_bar(n * 18 + 1,
				 k * 10 + 3,
				 n * 18 + 16,
				 k * 10 + 9,
				 TFTRGB(n * 16, k * 16, 255 - (n * 8 + k * 8) )
				 );

	/* Interlase test.	*/
	display_line(0,  0,  xm, 1,  col3);
	display_line(0,  0,  xm, 3,  col3);
	display_line(0,  0,  xm, 5,  col3);

	/* diagonales test.	*/
	display_line(xm, 0,  xm, ym, col3);
	display_line(xm, ym, 0,  ym, col3);
	display_line(0,  ym, 0,  0,  col3);
	display_line(0,  0,  xm, ym, col3);
	display_line(0,  ym, xm, 0,  col3);

	//getch();

}

static int local_random( int num )
{

	static unsigned long rand_val = 123456UL;

	if (rand_val & 0x80000000UL)
		rand_val = (rand_val << 1);
	else	rand_val = (rand_val << 1) ^0x201051UL;

	return (rand_val % num);

}


/*                                                                      */
/*      RANDOMBARS: Display local_random bars                                 */
/*                                                                      */

static void RandomBars(void)
{
	unsigned n = 20000;
	for (;n --;)
	{                    /* Until user enters a key...   */
		int r = local_random(255);
		int g = local_random(255);
		int b = local_random(255);

		const COLOR_T color = TFTRGB(r, g, b);

		int x = local_random(vesa_getmaxx());
		int y = local_random(vesa_getmaxy());
		int x2 = local_random(vesa_getmaxx());
		int y2 = local_random(vesa_getmaxy());

		graph_bar(x, y, x2, y2, color);
		//_delay_ms(10);
	}

	//getch();             /* Pause for user's response    */
}

#if ! LCDMODE_LTDC

void 
display_clear(void)
{
	const COLOR_T bg = display_getbgcolor();

	s1d13781_clear(bg);

	//display_limb();
#if 0
	GrideTest();
	display_string2_P(PSTR("test done"), 0);
	for (;;)
		;
#elif 0
	RandomBars();
	display_gotoxy(0, 0);
	display_setcolors(COLOR_WHITE, COLOR_BLACK);
	display_string2_P(PSTR("test done"), 0);
	for (;;)
		;
#endif
}

void
//NOINLINEAT
display_setcolors(COLOR_T fg, COLOR_T bg)
{
	s1d13781_setcolor(fg, bg);
}

void display_setcolors3(COLOR_T fg, COLOR_T bg, COLOR_T fgbg)
{
	display_setcolors(fg, bg);
}

void
display_wrdata_begin(void)
{
	s1d13781_put_char_begin();
}

void
display_wrdata_end(void)
{
}


void
display_wrdata2_begin(void)
{
	s1d13781_put_char_begin2();
}

void
display_wrdata2_end(void)
{
}


void
display_wrdatabig_begin(void)
{
	s1d13781_put_charbig_begin();
}


void
display_wrdatabig_end(void)
{
}


/* отображение одной вертикальной полосы на графическом индикаторе */
/* старшие биты соответствуют верхним пикселям изображения */
/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */

// работа с буфером в видеопамяти с начальным смещением scratchbufbase
void 
display_barcolumn(uint_fast8_t pattern)
{
}

void
display_wrdatabar_begin(void)
{
}

void
display_wrdatabar_end(void)
{
}

// отрисовать буфер на дисплее.
// работа с буфером в видеопамяти с начальным смещением scratchbufbase
/* выдать на дисплей монохромный буфер с размерами dx * dy битов */
void display_showbufferXXX(
	const GX_t * buffer,
	unsigned dx,	// пиксели
	unsigned dy,	// пиксели
	uint_fast8_t x,	// сетка
	uint_fast8_t y	// сетка
	)
{
	ASSERT(dx <= S1D_DISPLAY_WIDTH);
	ASSERT(dy <= S1D_DISPLAY_HEIGHT);

	// дождаться выполнения предидущей команды BitBlt engine.
	if (bitblt_waitbusy() != 0)
	{
		// вычисление начального адреса в видеопамяти
		const uint_fast32_t dstaddr = S1D_PHYSICAL_VMEM_ADDR + (uint_fast32_t) GRID2X(x) * (S1D_DISPLAY_BPP / 8) + (uint_fast32_t) GRID2Y(y) * S1D_DISPLAY_SCANLINE_BYTES;

		ASSERT((dx % 16) == 0);

		if ((dx % 16) == 0 && sizeof (* buffer) == 2)
		{

			uint_fast8_t init = 1;
			// перенос монохромного растра в видеопамять
			s1d13781_select();
			set_addrwr(scratchbufbase);// Sets address for writes and complete spi transfer

		#if 0//WITHSPIEXT16 && WITHSPIHWDMA
			// Обратить внимание, передается растр, где младшицй бит левее.
		// Передача в индикатор по DMA	
			const uint_fast32_t len = GXSIZE(dx / 16, dy);
			arm_hardware_flush((uintptr_t) buffer, len * sizeof (* buffer));	// количество байтов
			hardware_spi_master_send_frame_16b(buffer, len);

		#else /* WITHSPIEXT16 && WITHSPIHWDMA */

				uint_fast16_t row, col;
				for (row = 0; row < dy; ++ row)
				{
					const GX_t * const rowstart = buffer + row * ((dx + 15) / 16);	// начало данных строки растра в памяти
					for (col = 0; col < dx; col += 16)
					{
						const uint_fast16_t v = rowstart [col / 16];
						if (init)
						{
							set_data16_p1(v);
							init = 0;
						}
						else
							set_data16_p2(v);
					}
				}
				set_data16complete();

		#endif /* WITHSPIEXT16 && WITHSPIHWDMA */
			s1d13781_unselect();
		}

		s1d13781_wrcmdcolor(REG96_BLT_BGCOLOR_0, stored_bgcolor);
		s1d13781_wrcmdcolor(REG9A_BLT_FGCOLOR_0, stored_fgcolor);
		s1d13781_colorexpand();

		//s1d13781_wrcmd8(REG80_BLT_CTRL_0, 0x80);	// BitBlt reset
		bitblt_setdstaddr(dstaddr);

		// set source address
		s1d13781_wrcmd32(REG88_BLT_SSADDR_0, scratchbufbase);		// bits of address
		// set bitblt rectangle width and height (pixels) registers.
		s1d13781_wrcmd32_pair(REG92_BLT_WIDTH, dy, dx);	// set bitblt rectangle width and height (pixels) registers.
		//s1d13781_wrcmd8(REG86_BLT_CMD, 0x04);	// перенесено в 'begin' - 0x04 - move with color expand
		s1d13781_wrcmd8(REG80_BLT_CTRL_0, 0x01);	// BitBlt start
		bitblt_waitbusy();
	}
}


/*****************/
void
display_put_char_big(uint_fast8_t c, uint_fast8_t lowhalf)
{
	(void) lowhalf;
	s1d13781_put_char_big(c);
}

void
display_put_char_half(uint_fast8_t c, uint_fast8_t lowhalf)
{
	(void) lowhalf;
	s1d13781_put_char_half(c);
}


// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
void
display_put_char_small(uint_fast8_t c, uint_fast8_t lowhalf)
{
	(void) lowhalf;
	s1d13781_put_char_small(c);
}


// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
void
display_put_char_small2(uint_fast8_t c, uint_fast8_t lowhalf)
{
	(void) lowhalf;
	s1d13781_put_char_small(c);
}

	//uint8_t x = h * CHAR_W;
	//uint8_t y = Y_DIM - 5 - (v * CHAR_H);

static uint_fast8_t stored_xgrid, stored_ygrid;	// используется в display_dispbar

void
display_gotoxy(uint_fast8_t x, uint_fast8_t y)
{
	stored_xgrid = x;	// используется в display_dispbar
	stored_ygrid = y;	// используется в display_dispbar

	s1d13781_gotoxy(GRID2X(x), GRID2Y(y));		// устанавливаем позицию в символьных координатах
}

/* работа с цветным буфером */
void display_plotfrom(
	uint_fast16_t x, 
	uint_fast16_t y	// Координаты в пикселях
	)
{
	// дождаться выполнения предидущей команды BitBlt engine.
	if (bitblt_waitbusy() != 0)
	{
		// вычисление начального адреса в видеопамяти
		const uint_fast32_t dstaddr = S1D_PHYSICAL_VMEM_ADDR + x * (S1D_DISPLAY_BPP / 8) + (uint_fast32_t) y * S1D_DISPLAY_SCANLINE_BYTES;


	#if S1D_DISPLAY_BPP == 24

		spi_select2(targetlcd, S1D13781_SPIMODE, S1D13781_SPIC_SPEED);	/* Enable SPI */
		set_addrwr_8bit_p1p2_nc(dstaddr);
		spi_complete();

	#elif S1D_DISPLAY_BPP == 16

		#if WITHSPIEXT16

			hardware_spi_connect_b16(S1D13781_SPIC_SPEED, S1D13781_SPIMODE);		// если есть возможность - работаем в 16-ти битном режиме
			prog_select(targetlcd);
			set_addw_16bit_p1p2_nc(dstaddr);
			hardware_spi_complete_b16();

		#else /* WITHSPI16BIT */

			spi_select2(targetlcd, S1D13781_SPIMODE, S1D13781_SPIC_SPEED);	/* Enable SPI */
			set_addrwr_8bit_p1p2_nc(dstaddr);
			spi_complete();

		#endif /* WITHSPI16BIT */

	#elif S1D_DISPLAY_BPP == 8

		spi_select2(targetlcd, S1D13781_SPIMODE, S1D13781_SPIC_SPEED);	/* Enable SPI */
		set_addrwr_8bit_p1p2_nc(dstaddr);
		spi_complete();

	#endif
	}

}


void display_plotstart(
	uint_fast16_t height	// Высота окна в пикселях
	)
{
	// дождаться выполнения предидущей команды BitBlt engine.
	if (bitblt_waitbusy() != 0)
	{
	}
}

// сейчас только одна строка работает
void display_plot(
	const PACKEDCOLOR_T * buffer, 
	uint_fast16_t dx,	// Размеры окна в пикселях
	uint_fast16_t dy
	)
{
	uint_fast32_t len = GXSIZE(dx, dy);	// количество элементов
	ASSERT(dy == 1);
#if 0//WITHSPIEXT16 && WITHSPIHWDMA
	// Передача в индикатор по DMA	
	arm_hardware_flush((uintptr_t) buffer, len * sizeof (* buffer));	// количество байтов
	hardware_spi_master_send_frame_16b(buffer, len);
#else /* WITHSPIEXT16 && WITHSPIHWDMA */
	if (len >= 2)
	{
		display_putpixel_1(* buffer ++);
		len -= 1;
		while (len --)
			display_putpixel_2(* buffer ++);
		display_putpixel_complete();
	}
	else
	{
		display_putpixel_1(* buffer ++);
		display_putpixel_complete();
	}
#endif /* WITHSPIEXT16 && WITHSPIHWDMA */
}

void display_plotstop(void)
{
#if S1D_DISPLAY_BPP == 24

	spi_unselect(targetlcd);			// или в байтовом (8-битном) режиме.

#elif S1D_DISPLAY_BPP == 16

	#if WITHSPIEXT16

		prog_unselect(targetlcd);
		hardware_spi_disconnect();

	#else /* WITHSPI16BIT */

		spi_unselect(targetlcd);

	#endif /* WITHSPI16BIT */

#elif S1D_DISPLAY_BPP == 8

	spi_unselect(targetlcd);

#endif
}

// Вызовы этой функции (или группу вызовов) требуется "обрамить" парой вызовов
// display_wrdatabar_begin() и display_wrdatabar_end().
void display_dispbar(
	uint_fast8_t width,	/* количество знакомест, занимаемых индикатором */
	uint_fast8_t value,		/* значение, которое надо отобразить */
	uint_fast8_t tracevalue,		/* значение маркера, которое надо отобразить */
	uint_fast8_t topvalue,	/* значение, соответствующее полностью заполненному индикатору */
	uint_fast8_t pattern,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL */
	uint_fast8_t patternmax,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL - для отображения запомненного значения */
	uint_fast8_t emptyp			/* паттерн для заполнения между штрихами */
	)
{
	const uint_fast16_t h = GRID2Y(1);
	const uint_fast16_t x = GRID2X(stored_xgrid);
	const uint_fast16_t y = GRID2Y(stored_ygrid);
	const uint_fast16_t w = GRID2X(width);
	const uint_fast16_t t = value * w / topvalue;

	bitblt_fill(x, y, t, h, stored_fgcolor);
	bitblt_fill(x + t, y, w - t, h, stored_bgcolor);
}

#endif /* ! LCDMODE_LTDC */

void
display_panel(
	uint_fast8_t x, // левый верхний угод
	uint_fast8_t y,
	uint_fast8_t w, // ширина и высота в знакоместах
	uint_fast8_t h
	)
{
	rectangle3d(GRID2X(x), GRID2Y(y), GRID2X(w) - 3, GRID2Y(h) - 3, COLOR_GRAY, COLOR_GRAY);
}

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


/* Разряжаем конденсаторы питания */
void display_discharge(void)
{
}

#endif /* LCDMODE_S1D13781 */
