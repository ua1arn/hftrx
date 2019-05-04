/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Поддержка контроллера TFT панели Epson S1D13781
//



#include "hardware.h"
#include "display.h"
#include "spifuncs.h"

//#include "formats.h"	// for debug prints

#if LCDMODE_ILI8961

// LCDMODE_ILI8961 - 320 * 240, only RGB interface

#include "ili8961.h"


	// Для дисплеев 320 * 240
	#include "./fonts/ILI9341_font_small.h"
	#include "./fonts/ILI9341_font_half.h"
	#include "./fonts/ILI9341_font_big.h"

	#define	ls020_smallfont	ILI9341_smallfont
	#define	ls020_halffont	ILI9341_halffont
	#define	ls020_bigfont	ILI9341_bigfont


#if SPI_BIDIRECTIONAL

static uint_fast8_t ili8961_read(
	uint_fast8_t addrv	// address value, A6,R/W,A5..A0
	)
{
	uint_fast8_t datav;	// data value, D7..D0

	if (addrv & 0x40)
		addrv |= 0x80;	// переместить A6 в 7-й бит
	addrv |= 0x40;	// D6=1: read

	spi_select(targetlcd, SPIC_MODE3);	/* Enable SPI */
  	spi_progval8_p1(targetlcd, addrv); 
	spi_complete(targetlcd);
	spi_to_read(targetlcd);
	datav = spi_read_byte(targetlcd, 0xFF);
	spi_to_write(targetlcd);
	spi_unselect(targetlcd);		/* Disable SPI */

	return datav;
}
#endif /* SPI_BIDIRECTIONAL */

static void ili8961_write(
	uint_fast8_t addrv,	// address value, A6,R/W,A5..A0
	uint_fast8_t datav	// data value, D7..D0
	)
{
	if (addrv & 0x40)
		addrv |= 0x80;	// переместить A6 в 7-й бит
	addrv &= ~ 0x40;	// D6=0: write

	spi_select(targetlcd, SPIC_MODE3);	/* Enable SPI */
  	spi_progval8_p1(targetlcd, addrv); 
  	spi_progval8_p2(targetlcd, datav); 
	spi_complete(targetlcd);
	spi_unselect(targetlcd);		/* Disable SPI */
}

#if SPI_BIDIRECTIONAL

static void ili8961_printstate(void)
{
	static const uint8_t as [] =
	{
		0x00, 0x01, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
		0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 
		0x16, 0x17, 0x18, 0x19, 0x1a,
		0x2b, 0x2f,
		0x55, 0x57, 0x5a, 0x61,
	};

	unsigned i;
	for (i = 0; i < sizeof as / sizeof as [0]; ++ i)
	{
		const uint_fast8_t addr = as [i];
		debug_printf_P(PSTR("r%02X=%02X "), addr, ili8961_read(addr));
	}
	debug_printf_P(PSTR("\n"));
}

#endif /* SPI_BIDIRECTIONAL */

static void ili8961_initialize(void)
{
	// Load defaults
	ili8961_write(0x00, 0x07);
	ili8961_write(0x01, 0x9C);
	ili8961_write(0x03, 0x40);
	ili8961_write(0x04, 0x0B);
	ili8961_write(0x05, 0x5E);
	ili8961_write(0x06, 0x15);
	ili8961_write(0x07, 0x46);
	ili8961_write(0x08, 0x00);
	ili8961_write(0x0b, 0x00);
	ili8961_write(0x0c, 0x06);
	ili8961_write(0x0d, 0x40);
	ili8961_write(0x0e, 0x40);
	ili8961_write(0x0f, 0x40);
	ili8961_write(0x10, 0x40);
	ili8961_write(0x11, 0x40);
	ili8961_write(0x12, 0x00);
	ili8961_write(0x16, 0x04);
	ili8961_write(0x17, 0x57);
	ili8961_write(0x18, 0x75);
	ili8961_write(0x19, 0x44);
	ili8961_write(0x1a, 0x54);
	ili8961_write(0x2b, 0x00);
	ili8961_write(0x2f, 0x61);
	ili8961_write(0x55, 0x00);
	ili8961_write(0x57, 0x00);
	ili8961_write(0x5a, 0x02);
	ili8961_write(0x61, 0x00);

	local_delay_ms(100); 

	//ili8961_write( 0x2F, 0x09 );	//VGH, LC_SEL (R2Fh[3:2]): 0b10 - normal voltage
	//ili8961_write( 0x2B, 0x01 );	// Standbay -> Normal Mode
	//return;

#if 0
	ili8961_write(0x05, 0x1e);
	local_delay_ms(100);
	ili8961_write(0x05, 0x5e);
	local_delay_ms(100);

	ili8961_write(0x2b, 0x01);
	ili8961_write(0x00, 0x09);
	ili8961_write(0x01, 0x9f);
	ili8961_write(0x03, 0x68);
	ili8961_write(0x0d, 0x40);
#if (FLIP_ILI8961 == 0)
	ili8961_write(0x04, 0x18/*0x1b*/);
#else
	ili8961_write(0x04, 0x1b);
#endif
	ili8961_write(0x16, 0x04);
	ili8961_write(0x2f, 0x61/*0x71*/);
	//return;
#endif

	// Custom programming

	ili8961_write( 0x04, 0x07 );	// flp control
	ili8961_write( 0x06, 0x84 );	// VBLK
	ili8961_write( 0x07, 0xfd );// HBLK
	ili8961_write( 0x00, 0x03 );// Set VCAC

	local_delay_ms(100); 

	ili8961_write( 0x0b, 0x80 ); // VCOMDC out voltage sourse select
	ili8961_write( 0x01, 0x9f );	//0x8F:VCOM=0.8V, 0x9f:VCOM=1.1V, 0xBF:VCOM=1.7V

	local_delay_ms(100);

	ili8961_write( 0x0c, 0x06 ); 
	ili8961_write( 0x16, 0x04 );	//GAMMA
	ili8961_write( 0x17, 0xa7 );	
	ili8961_write( 0x18, 0x78 );	
	ili8961_write( 0x19, 0x67 );	
	ili8961_write( 0x1a, 0x65 );	
	ili8961_write( 0x1b, 0x74 );	

	ili8961_write( 0x2f, 0x09 );	//VGH, LC_SEL (R2Fh[3:2]): 0b10 - normal voltage


	ili8961_write( 0x55, 0x00 );	//0x00-line inversion, 0x40-column inversion 
	ili8961_write( 0x5a, 0x03 );	//VGL
	ili8961_write( 0x00, 0x07 );	
	ili8961_write( 0x2b, 0x01 );	// Standbay -> Normal Mode

	local_delay_ms(400);


}

/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{
	ili8961_initialize();
	//ili8961_printstate();	// Use with SPI_BIDIRECTIONAL
}

void display_set_contrast(uint_fast8_t v)
{
}

/* аппаратный сброс дисплея - перед инициализаций */
/* вызывается при разрешённых прерываниях. */
void display_reset(void)
{
}



/* Разряжаем конденсаторы питания */
void display_discharge(void)
{
}

uint_fast8_t
display_getreadystate(void)
{
	return 1;
}

#endif /* LCDMODE_ILI8961 */
