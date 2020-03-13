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
#include "gui.h"
#include <stdint.h>

#include "spi.h"
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


#include "./fonts/S1D13781_font_small_LTDC.h"
#include "./fonts/S1D13781_font_half_LTDC.h"
#include "./fonts/S1D13781_font_big_LTDC.h"

#define S1D13781_SPIMODE SPIC_MODE3		/* допустим только MODE3, MODE2 не работает с этим контроллером */
#define S1D13781_SPIC_SPEEDSLOW		SPIC_SPEED10M
#define S1D13781_SPIC_SPEED		SPIC_SPEED25M

// Условие использования оптимизированных функций обращения к SPI
#define WITHSPIEXT16 (WITHSPIHW && WITHSPI16BIT)

#if LCDMODE_S1D13781_TOPDOWN
	#define S1D13781_SETFLAGS (0x02 << 3)	// для перевёрнутого изображения
#else
	#define S1D13781_SETFLAGS (0x00 << 3)
#endif

#define BIGCHARWIDTH BIGCHARW
#define HALFCHARWIDTH HALFCHARW
#define BIGCHARHEIGHT BIGCHARH

#define SMALLCHARWIDTH SMALLCHARW
#define SMALLCHARHEIGHT	SMALLCHARH

#define SMALLCHARWIDTH2 SMALLCHARW
#define SMALLCHARHEIGHT2 SMALLCHARH

// For SPI write operations
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

// For SPI read operations
static void
s1d13781_selectslow(void)
{
#if WITHSPIEXT16
	hardware_spi_connect_b16(S1D13781_SPIC_SPEEDSLOW, S1D13781_SPIMODE);		// если есть возможность - работаем в 16-ти битном режиме
	prog_select(targetlcd);	
#else
	spi_select2(targetlcd, S1D13781_SPIMODE, S1D13781_SPIC_SPEEDSLOW);	/* Enable SPI */
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
// 0x88 - 16 bit write
// 0xc0 - 8 bit read
// 0xc8 - 16 bit read

// Sets address for writes and complete spi transfer
static void set_addrwr(
	uint_fast32_t addr
	)
{
#if WITHSPIEXT16
	// 19 bit address
	// 16 bit data write
	const uint_fast32_t v = (addr) | 0x88000000uL;

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
static void set_addrwr_p1p2_registers_nc(
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
	//const uint_fast32_t v = ((addr) & 0x0007FFFFuL) | 0x88000000uL;
	const uint_fast32_t v = (addr) | 0x88000000uL;

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

	set_addrwr_p1p2_registers_nc(reg);
	#if WITHSPIEXT16
		hardware_spi_b16_p2(val);
		hardware_spi_complete_b16();
	#else /* WITHSPIEXT16 */
		spi_progval8_p2(targetlcd, val);
		spi_complete(targetlcd);
	#endif /* WITHSPIEXT16 */

	s1d13781_unselect();
}

static void s1d13781_wrcmd8slow(uint_fast8_t reg, uint_fast8_t val)
{
	s1d13781_selectslow();

	set_addrwr_p1p2_registers_nc(reg);
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

	set_addrwr_p1p2_registers_nc(reg);
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

	set_addrwr_p1p2_registers_nc(reg);
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

	set_addrwr_p1p2_registers_nc(reg);
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

	s1d13781_selectslow();
	v = set_addr_p1p2_registers_getval8(REG84_BLT_STATUS);
	s1d13781_unselect();

	return (v & 0x01) != 0;
}

static uint_fast16_t getprodcode(void)
{
	uint_fast16_t v;

	s1d13781_selectslow();
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
	return 1;

	while (bitblt_getbusyflag() != 0)
		;
	return 1;
}

static void
bitblt_waitbusy2(void)
{
	if (s1d13781_missing != 0)
		return;

	while (bitblt_getbusyflag() != 0)
		;
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
		bitblt_waitbusy2();
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
	bitblt_waitbusy2();
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
		bitblt_waitbusy2();
	}
}

/* копирование содержимого окна с перекрытием для водопада */
void
display_scroll_down(
	uint_fast16_t x,	// левый верзний угол окна
	uint_fast16_t y,	// левый верзний угол окна
	uint_fast16_t w, 	// до 65535 пикселей - ширина окна
	uint_fast16_t h, 	// до 65535 пикселей - высота окна
	uint_fast16_t n,	// количество строк прокрутки
	int_fast16_t hshift	// количество пиксеелей для сдвига влево (отрицательное число) или вправо (положительное).
	)
{
	const COLOR_T fillnewcolor = COLOR_BLACK;	// цвет, которым заполняется свободное место при сдвиге старого изобажения
	enum { WC = (S1D_DISPLAY_BPP / 8) };		// количество байтов на пиксель
	const int_fast16_t adjw = hshift < 0 ?
				w + hshift 	// сдвиг окна влево
				:
				w - hshift	// сдвиг окна вправо
				;
	const int_fast16_t onlypos = hshift < 0 ? 0 : hshift;
	const int_fast16_t onlyneg = hshift < 0 ? hshift : 0;
	// Parameters validation
	if ((hshift < 0 && - hshift >= w) || (hshift > 0 && hshift >= w) || (n >= h))
	{
		// очистка используемой зоны экрана
		bitblt_fill(x, y, w, h, fillnewcolor);
		return;
	}

	// вычисление начального адреса в видеопамяти
	const uint_fast32_t srcaddr = 
			S1D_PHYSICAL_VMEM_ADDR + 
			(uint_fast32_t) (x + w - onlypos) * WC +
			(uint_fast32_t) (y + h - 1 - n) * S1D_DISPLAY_SCANLINE_BYTES;

	// вычисление конечного адреса в видеопамяти
	// Здесь используется информация о горизонтальном сдвиге растра
	const uint_fast32_t dstaddr = 
			S1D_PHYSICAL_VMEM_ADDR + 
			(uint_fast32_t) (x + w + onlyneg) * WC +
			(uint_fast32_t) (y + h - 1) * S1D_DISPLAY_SCANLINE_BYTES;

	// дождаться выполнения предидущей команды BitBlt engine.
	// копирование
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
		s1d13781_wrcmd32_pair(REG92_BLT_WIDTH, h - n, adjw);
		s1d13781_wrcmd8(REG86_BLT_CMD, 0x01);	// 0x01 - move negative
		s1d13781_wrcmd8(REG80_BLT_CTRL_0, 0x01);	// BitBlt start
		bitblt_waitbusy2();

		// очистка края экрана
		if (hshift < 0)
		{
			// очистка правого края экрана
			bitblt_fill(x + w + hshift, y + n, - hshift, h - n, fillnewcolor);
		}
		else if (hshift > 0)
		{
			// очистка левого края экрана
			bitblt_fill(x, y + n, hshift, h - n, fillnewcolor);
		}
	}
}
	
/* копирование содержимого окна с перекрытием для водопада */
void
display_scroll_up(
	uint_fast16_t x,	// левый верзний угол окна
	uint_fast16_t y,	// левый верзний угол окна
	uint_fast16_t w, 	// до 65535 пикселей - ширина окна
	uint_fast16_t h, 	// до 65535 пикселей - высота окна
	uint_fast16_t n,	// количество строк прокрутки
	int_fast16_t hshift	// количество пиксеелей для сдвига влево (отрицательное число) или вправо (положительное).
	)
{
	const COLOR_T fillnewcolor = COLOR_BLACK;	// цвет, которым заполняется свободное место при сдвиге старого изобажения
	enum { WC = (S1D_DISPLAY_BPP / 8) };		// количество байтов на пиксель
	const int_fast16_t adjw = hshift < 0 ?
				w + hshift 	// сдвиг окна влево
				:
				w - hshift	// сдвиг окна вправо
				;
	const int_fast16_t onlypos = hshift < 0 ? 0 : hshift;
	const int_fast16_t onlyneg = hshift < 0 ? hshift : 0;
	// Parameters validation
	if ((hshift < 0 && - hshift >= w) || (hshift > 0 && hshift >= w) || (n >= h))
	{
		// очистка используемой зоны экрана
		bitblt_fill(x, y, w, h, fillnewcolor);
		return;
	}

	// вычисление начального адреса в видеопамяти
	const uint_fast32_t srcaddr = 
			S1D_PHYSICAL_VMEM_ADDR + 
			(uint_fast32_t) (x - onlyneg) * WC +
			(uint_fast32_t) (y + n) * S1D_DISPLAY_SCANLINE_BYTES;

	// вычисление конечного адреса в видеопамяти
	const uint_fast32_t dstaddr = 
			S1D_PHYSICAL_VMEM_ADDR + 
			(uint_fast32_t) (x + onlypos) * WC +
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
		bitblt_waitbusy2();

		// очистка края экрана
		if (hshift < 0)
		{
			// очистка правого края экрана
			bitblt_fill(x + w + hshift, y, - hshift, h - n, fillnewcolor);
		}
		else if (hshift > 0)
		{
			// очистка левого края экрана
			bitblt_fill(x, y, hshift, h - n, fillnewcolor);
		}
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

static void chargen_flush(void)
{
	set_data16(chargen_bitacc);
	chargen_addr += 2;
	chargen_bitpos = 0;
	chargen_bitacc = 0;
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
		chargen_flush();
}

static 
void
chargen_putbits(
	const FLASHMEM uint8_t * raster, 
	uint_fast8_t width
	)
{
	uint_fast8_t col;
	for (col = 0; col < width; ++ col)
	{
		chargen_putbit((1uL << (col % 8)) & raster [col / 8]);
	}
}


static void chargen_endofchar(void)
{
	if ((chargen_addr < S1D_PHYSICAL_VMEM_SIZE) && (chargen_bitpos != 0))
		chargen_flush();

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
smallfont_decode(uint_fast8_t c)
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
		const uint_fast32_t a = chargen_addr;
		const uint_fast8_t cv = bigfont_decode(c ? '.' : '#');		
		uint_fast8_t cgrow;

		chargen_beginofchar();
		for (cgrow = 0; cgrow < BIGCHARH; ++ cgrow)
		{
			chargen_putbits(S1D13781_bigfont_LTDC [cv][cgrow], BIGCHARW_NARROW);
		}
		chargen_endofchar();
		narrowchargenstep = chargen_addr - a;
	}
	
	// Заполнение знакогенератора больших символов
	ASSERT(chargen_addr <= S1D_PHYSICAL_VMEM_SIZE);
	bigchargenbase = chargen_addr;

	for (c = 0; c < (sizeof S1D13781_bigfont_LTDC / sizeof S1D13781_bigfont_LTDC [0]); ++ c)
	{
		const uint_fast32_t a = chargen_addr;
		uint_fast8_t cgrow;

		chargen_beginofchar();
		for (cgrow = 0; cgrow < BIGCHARH; ++ cgrow)
		{
			chargen_putbits(S1D13781_bigfont_LTDC [c] [cgrow], BIGCHARW);
		}
		chargen_endofchar();
		bigchargenstep = chargen_addr - a;
	}

	// Заполнение знакогенератора средних символов
	ASSERT(chargen_addr <= S1D_PHYSICAL_VMEM_SIZE);
	halfchargenbase = chargen_addr;

	for (c = 0; c < (sizeof S1D13781_halffont_LTDC / sizeof S1D13781_halffont_LTDC [0]); ++ c)
	{
		const uint_fast32_t a = chargen_addr;
		uint_fast8_t cgrow;

		chargen_beginofchar();
		for (cgrow = 0; cgrow < HALFCHARH; ++ cgrow)
		{
			chargen_putbits(S1D13781_halffont_LTDC [c] [cgrow], HALFCHARW);
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
			chargen_putbits(S1D13781_smallfont_LTDC [c] [cgrow], SMALLCHARW);
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
			chargen_putbits(S1D13781_smallfont_LTDC [c] [cgrow], SMALLCHARW);
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
	return smallchargenbase + smallfont_decode(cc) * smallchargenstep;
}

static uint_fast32_t getsmallcharbase2(
	char cc	// символ для отображения
	)
{
	return smallchargenbase2 + smallfont_decode(cc) * smallchargenstep2;
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
	bitblt_waitbusy2();
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
		s1d13781_next_column(GRID2X(CHARS2GRID(1)));
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
			bitblt_chargen_big(BIGCHARW_NARROW, getnarrowcharbase(cc));
			s1d13781_next_column(BIGCHARW_NARROW);
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

void display_putpixel(
	uint_fast16_t x,
	uint_fast16_t y,
	COLOR_T color
	)
{
	// вычисление начального адреса в видеопамяти
	const uint_fast32_t dstaddr = S1D_PHYSICAL_VMEM_ADDR + x * (S1D_DISPLAY_BPP / 8) + (uint_fast32_t) y * S1D_DISPLAY_SCANLINE_BYTES;

	s1d13781_select();

#if S1D_DISPLAY_BPP == 24

	set_addrwr_8bit_p1p2_nc(dstaddr);
	spi_progval8_p2(targetlcd, color >> 0);
	spi_progval8_p2(targetlcd, color >> 8);
	spi_progval8_p2(targetlcd, color >> 16);
	spi_complete(targetlcd);

#elif S1D_DISPLAY_BPP == 16

	#if WITHSPIEXT16

		set_addw_16bit_p1p2_nc(dstaddr);
		hardware_spi_b16_p2(color);
		hardware_spi_complete_b16();

	#else /* WITHSPI16BIT */

		set_addrwr_8bit_p1p2_nc(dstaddr);
		spi_progval8_p2(targetlcd, color >> 0);
		spi_progval8_p2(targetlcd, color >> 8);
		spi_complete(targetlcd);

	#endif /* WITHSPI16BIT */

#elif S1D_DISPLAY_BPP == 8

	set_addrwr_8bit_p1p2_nc(dstaddr);
	spi_progval8_p2(targetlcd, color >> 0);
	spi_complete(targetlcd);

#endif

	s1d13781_unselect();
}

static void display_putpixel_1(
	PACKEDCOLOR_T color
	)
{
#if S1D_DISPLAY_BPP == 24

	spi_progval8_p1(targetlcd, color.r);
	spi_progval8_p2(targetlcd, color.g);
	spi_progval8_p2(targetlcd, color.b);

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
	PACKEDCOLOR_T color
	)
{
#if S1D_DISPLAY_BPP == 24

	spi_progval8_p2(targetlcd, color.r);
	spi_progval8_p2(targetlcd, color.g);
	spi_progval8_p2(targetlcd, color.b);

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
        { REG20_PANEL_SET,                      0x008D },		// 18 bit panel, DE fixed low
        { REG22_DISP_SET,                       0x0001 | 0x0100 },	// TE pin disable

        { REG24_HDISP,                          0x003C },	// Horizontal Display Width Register
        { REG26_HNDP,                           0x002D },	// Horizontal Non-Display Period Register
        { REG28_VDISP,                          0x0110 },	// Vertical Display Height Register
        { REG2A_VNDP,                           0x000E },	// Vertical Non-Display Period Register
        { REG2C_HSW,                            0x0029 },	// HS Pulse Width Register
        { REG2E_HPS,                            0x0002 },	// HS Pulse Start Position Register
        { REG30_VSW,                            0x000A },	// VS Pulse Width Register
        { REG32_VPS,                            0x0002 },	// VS Pulse Start Position Register

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

/* установить DE в требуемое состояние */
static void
s1d13781_set_DE(uint_fast8_t state)
{
	s1d13781_wrcmd16(REG20_PANEL_SET, state ? 0x00CD : 0x008D);	// 18 bit panel, DE fixed high/fixed low
}


/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{
	//hardware_spi_master_setfreq(SPIC_SPEED10M, 10000000uL);
	//hardware_spi_master_setfreq(SPIC_SPEED25M, 25000000uL);

	const uint_fast16_t prodcode = getprodcode();
	debug_printf_P(PSTR("display_initialize: product code = 0x%04x (expected 0x0050)\n"), (unsigned) prodcode);
	if (prodcode != 0x0050)
	{
		s1d13781_missing = 1;
	}
	s1d13781_initialize();
	s1d13781_set_DE(0);
	local_delay_ms(60);
	s1d13781_set_DE(1);
}

void display_set_contrast(uint_fast8_t v)
{
}

// Рисуем на основном экране цветной прямоугольник.
// x2, y2 - координаты второго угла (не входящие в закрашиваемый прямоугольник)
void display_solidbar(uint_fast16_t x, uint_fast16_t y, uint_fast16_t x2, uint_fast16_t y2, COLOR_T color)
{
	if (x2 < x)
	{
		const uint_fast16_t t = x;
		x = x2, x2 = t;
	}
	if (y2 < y)
	{
		const uint_fast16_t t = y;
		y = y2, y2 = t;
	}
	bitblt_fill(x, y, x2 - x, y2 - y, color);
}


#if ! LCDMODE_LTDC

void 
display_clear(void)
{
	const COLOR_T bg = display_getbgcolor();

	s1d13781_clear(bg);
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
void s1d13781_showbuffer(
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
			const uint_fast32_t len = MGSIZE(dx, dy);
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
		s1d13781_wrcmd8(REG86_BLT_CMD, 0x04);	// 0x04 - move with color expand
		s1d13781_wrcmd8(REG80_BLT_CTRL_0, 0x01);	// BitBlt start
		bitblt_waitbusy2();
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
	s1d13781_gotoxy(x, y);
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

void display_plot(
	const PACKEDCOLOR_T * buffer, 
	const uint_fast16_t dx,	// Размеры окна в пикселях
	uint_fast16_t dy
	)
{
#if 0//WITHSPIEXT16 && WITHSPIHWDMA
	// Передача в индикатор по DMA
	arm_hardware_flush((uintptr_t) buffer, GXSIZE(dx, dy) * sizeof (* buffer));	// количество байтов
#endif /* WITHSPIEXT16 && WITHSPIHWDMA */

	for (; dy --; shadow_dstaddr += S1D_DISPLAY_SCANLINE_BYTES)
	{
		/* начать пересылку по SPI */
		s1d13781_select();

	#if S1D_DISPLAY_BPP == 24
		set_addrwr_8bit_p1p2_nc(shadow_dstaddr);
		spi_complete(targetlcd);
	#elif S1D_DISPLAY_BPP == 16
		#if WITHSPIEXT16
			set_addw_16bit_p1p2_nc(shadow_dstaddr);
			hardware_spi_complete_b16();
		#else /* WITHSPI16BIT */
			set_addrwr_8bit_p1p2_nc(shadow_dstaddr);
			spi_complete(targetlcd);
		#endif /* WITHSPI16BIT */
	#elif S1D_DISPLAY_BPP == 8
		set_addrwr_8bit_p1p2_nc(shadow_dstaddr);
		spi_complete(targetlcd);
	#endif

		/* произвести пересылку по SPI */
		{
			uint_fast32_t len = dx;	// количество элементов
		#if 0//WITHSPIEXT16 && WITHSPIHWDMA
			// Передача в индикатор по DMA
			hardware_spi_master_send_frame_16b(buffer, len);
			buffer += len;
		#else /* WITHSPIEXT16 && WITHSPIHWDMA */
			if (len >= 2)
			{
				display_putpixel_1(* buffer ++);
				len -= 1;
				while (len --)
					display_putpixel_2(* buffer ++);
				display_putpixel_complete();
			}
			else if (len == 1)
			{
				display_putpixel_1(* buffer ++);
				display_putpixel_complete();
			}
		#endif /* WITHSPIEXT16 && WITHSPIHWDMA */
		}

		/* закончить пересылку по SPI */
		s1d13781_unselect();
	}
}

void display_plotstop(void)
{
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
	const uint_fast16_t h = SMALLCHARH; //GRID2Y(1);
	const uint_fast16_t x = GRID2X(stored_xgrid);
	const uint_fast16_t y = GRID2Y(stored_ygrid);
	const uint_fast16_t wfull = GRID2X(width);
	const uint_fast16_t wpart = (uint_fast32_t) wfull * value / topvalue;
	const uint_fast16_t wmark = (uint_fast32_t) wfull * tracevalue / topvalue;

	bitblt_fill(x, y, wpart, h, stored_fgcolor);
	bitblt_fill(x + wpart, y, wfull - wpart, h, stored_bgcolor);
	if (wmark < wfull && wmark >= wpart)
		bitblt_fill(x + wmark, y, 1, h, stored_fgcolor);
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
