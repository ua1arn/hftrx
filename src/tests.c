//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "synthcalcs.h"
#include "board.h"
#include "audio.h"
#include "keyboard.h"
#include "encoder.h"

#include "display/display.h"
#include "display2.h"
#include "formats.h"
#include "codecs.h"
#include "spi.h"
#include "gpio.h"

#if WITHUSEFATFS
	#include "fatfs/ff.h"
#endif /* WITHUSEFATFS */
#if WITHUSEFATFS
	#include "sdcard.h"
#endif /* WITHUSEFATFS */

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
//#include <stdbool.h>


#if 0
// Печать параметров на экране
static void showpos(uint_fast8_t pos)
{
	//static unsigned  count;
	//count ++;
	char buff [22];

	local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
		PSTR("POS %2u"), (unsigned) pos
		 );
	display_gotoxy(0, 0);
	display_at(buff, 0);
}

#endif

#if ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED

// e-inc support functions
static uint_fast8_t eink_lcd_busy(void)
{
	enum { WORKMASK = 1uL << 1 };	/* PB1 */
	arm_hardware_piob_inputs(WORKMASK);
	return (GPIOB->IDR & WORKMASK) != 0;
}
static void eink_lcd_discharge(uint_fast8_t state)
{
	enum { WORKMASK = 1uL << 3 };	/* PB3 */
	arm_hardware_piob_outputs(WORKMASK, WORKMASK * (state != 0));

}
static void eink_lcd_backlight(uint_fast8_t state)
{
	enum { WORKMASK = 1uL << 4 };	/* PB4 */
	arm_hardware_piob_outputs(WORKMASK, WORKMASK * (state != 0));

}
static void eink_lcd_on_border(uint_fast8_t state)
{
	enum { WORKMASK = 1uL << 4 };	/* PB4 */
	arm_hardware_piob_outputs(WORKMASK, WORKMASK * (state != 0));

}
static void eink_lcd_toggle_border(void)
{
	enum { WORKMASK = 1uL << 4 };	/* PB4 */
	GPIOB->ODR ^= WORKMASK;			// border bit
}
static void eink_lcd_pwron(uint_fast8_t state)
{
	enum { WORKMASK = 1uL << 5 };	/* PB5 */
	arm_hardware_piob_outputs(WORKMASK, WORKMASK * (state != 0));

}

static void test_cpu_pwron(uint_fast8_t state)
{
	enum { WORKMASK = 1uL << 11 };	/* PA11 */
	arm_hardware_pioa_outputs(WORKMASK, WORKMASK * (state != 0));

}


static void check_poweroff(void)
{
	uint_fast8_t kbch, repeat;

	if ((repeat = kbd_scan(& kbch)) != 0)
	{
		switch (kbch)
		{
		case KBD_CODE_0:
			test_cpu_pwron(1);
			eink_lcd_backlight(1);	// backlight ON
			break;

		case KBD_CODE_1:
			eink_lcd_backlight(0);	// backlight off
			test_cpu_pwron(0);
			break;

		}
		return;
	}

}


static void local_delay_ms_spool(int ms)
{
	int v = ms / 5;
	while (v --)
	{
		local_delay_ms(5);
		check_poweroff();
	}
}

enum { EINC_SPIMODE = SPIC_MODE0 };

static void
eink_reset(void)
{
	local_delay_ms(25);		// delay after power-up
	board_lcd_reset(0); 	// Pull RST pin down
	board_update();
	local_delay_ms(25);		// local_delay_ms 5 mS (minimum by documentation - 1 mS).
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(25);		// local_delay_ms 5 mS (minimum by documentation - 1 mS).
}

static uint_fast8_t eink_read(uint_fast8_t addr)
{
	uint_fast8_t v;
	const spitarget_t target = targetlcd;
	//
	spi_select(target, EINC_SPIMODE);

	spi_progval8_p1(target, 0x70);
	spi_progval8_p2(target, addr);
	spi_complete(target);
	spi_unselect(target);

	local_delay_us(10);

	spi_select(target, EINC_SPIMODE);

	spi_progval8_p1(target, 0x73);
	spi_complete(target);
	v =  spi_read_byte(target, 0x00);

	spi_unselect(target);
	return v;
}

static uint_fast8_t eink_read_id(void)
{
	uint_fast8_t v;
	const spitarget_t target = targetlcd;
	//
	local_delay_us(15);
	spi_select(target, EINC_SPIMODE);
	spi_progval8_p1(target, 0x71);
	spi_complete(target);

	v =  spi_read_byte(target, 0x00);

	spi_unselect(target);
	return v;
}

static void eink_write(uint_fast8_t addr, uint_fast8_t v)
{
	const spitarget_t target = targetlcd;
	//
	spi_select(target, EINC_SPIMODE);

	spi_progval8_p1(target, 0x70);
	spi_progval8_p2(target, addr);
	spi_complete(target);

	spi_unselect(target);

	local_delay_us(10);

	spi_select(target, EINC_SPIMODE);

	spi_progval8_p1(target, 0x72);
	spi_progval8_p2(target, v);

	spi_complete(target);

	spi_unselect(target);
}

void epd_spi_send (unsigned char register_index, const unsigned char *register_data,
               unsigned length) {
	const spitarget_t target = targetlcd;
	//
	spi_select(target, EINC_SPIMODE);

	spi_progval8_p1(target, 0x70);
	spi_progval8_p2(target, register_index);
	spi_complete(target);

	spi_unselect(target);

	local_delay_us(10);

	spi_select(target, EINC_SPIMODE);

	spi_progval8_p1(target, 0x72);
	while (length --)
		spi_progval8_p2(target, * register_data ++);
	spi_complete(target);

	spi_unselect(target);
}

uint8_t SPI_R(uint8_t Register, uint8_t Data) 
{
	uint_fast8_t v;
	const spitarget_t target = targetlcd;
	//
	spi_select(target, EINC_SPIMODE);

	spi_progval8_p1(target, 0x70);
	spi_progval8_p2(target, Register);
	spi_complete(target);
	spi_unselect(target);

	local_delay_us(10);

	spi_select(target, EINC_SPIMODE);

	spi_progval8_p1(target, 0x73);
	spi_complete(target);
	v =  spi_read_byte(target, Data);

	spi_unselect(target);
	return v;
}

static void eink_write8bytes(uint_fast8_t addr, 
	uint_fast16_t v1, 
	uint_fast16_t v2, 
	uint_fast16_t v3, 
	uint_fast16_t v4)
{
	const spitarget_t target = targetlcd;
	//
	spi_select(target, EINC_SPIMODE);

	spi_progval8_p1(target, 0x70);
	spi_progval8_p2(target, addr);
	spi_complete(target);

	spi_unselect(target);

	spi_select(target, EINC_SPIMODE);

	spi_progval8_p1(target, 0x72);
	spi_progval8_p2(target, v1 >> 8);
	spi_progval8_p2(target, v1 >> 0);
	spi_progval8_p2(target, v2 >> 8);
	spi_progval8_p2(target, v2 >> 0);
	spi_progval8_p2(target, v3 >> 8);
	spi_progval8_p2(target, v3 >> 0);
	spi_progval8_p2(target, v4 >> 8);
	spi_progval8_p2(target, v4 >> 0);
	spi_complete(target);

	spi_unselect(target);
}

// return: 3: scan on, 0: scan off
static uint_fast8_t getscan4bytes(uint_fast8_t row, uint_fast8_t line, uint_fast8_t stage)
{
	switch (stage)
	{
	case 0: return 0xff;
	case 1: return 0xff;
	}
	uint_fast8_t r = 0;
	r = (r << 2) | ((row == line) ? 0x03 : 0x00); -- row;
	r = (r << 2) | ((row == line) ? 0x03 : 0x00); -- row;
	r = (r << 2) | ((row == line) ? 0x03 : 0x00); -- row;
	r = (r << 2) | ((row == line) ? 0x03 : 0x00);
	return r;
}

#include "./display/fonts/ls020_font_small.h"

static const char eitext [] = "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
//static const char eitext [] = "#######################################################################################-------------------------------------";

static uint_fast8_t
ascii_decode(uint_fast8_t c)
{
	return c - ' ';
}

// Выборка значений из буфера изображения
// значения:
// 3 - black
// 2 - while
// 0 - nothing (не модифицируется) - если вне окна, разрешённого к обновлению.
// col: 0..199. row: 0..95
static uint_fast8_t getbufferbit(uint_fast8_t col, uint_fast8_t row, uint_fast8_t stage)
{
	static const uint_fast8_t pattern [16] =
	{
		0xc0, 0x60, 0x30, 0x18, 0x06, 0x06, 0x03, 0x01,
		0xc0, 0x60, 0x30, 0x18, 0x06, 0x06, 0x03, 0x01,
	};

	switch (stage)
	{
	case 0: return 3;
	case 1: return 2;
	}

	//PRINTF(PSTR("getbufferbit: col=%d, row=%d\n"), col, row);

	uint_fast8_t charrow = row % SMALLCHARH;
	uint_fast8_t charcol = col % 8;
	uint_fast8_t textcol = col / 8;
	//if (textcol >= strlen(eitext))
	//	return 0;

#if 0
	const uint_fast8_t c = ascii_decode((unsigned char) eitext [textcol]);
	enum { NBYTES = (sizeof ls020_smallfont [0] / sizeof ls020_smallfont [0][0]) };
	const FLASHMEM uint8_t * p = & ls020_smallfont [c][0];
	return (p [SMALLCHARH - 1 - charrow] & (128U >> charcol)) ? 3 : 2;
#else
	const uint_fast8_t * p = pattern;
	return (p [charrow] & (1U << charcol)) ? 3 : 2;
#endif
}

// формирование байтов для передачи в дисплей
	// col code 198, 196..0
static uint_fast8_t getrow4byteseven(uint_fast8_t col, uint_fast8_t row, uint_fast8_t stage)
{
	uint_fast8_t r = 0;
	// row/col - координаты пикселя
	r = (r << 2) | getbufferbit(col, row, stage); col -= 2;
	r = (r << 2) | getbufferbit(col, row, stage); col -= 2;
	r = (r << 2) | getbufferbit(col, row, stage); col -= 2;
	r = (r << 2) | getbufferbit(col, row, stage);;
	return r;
}

// формирование байтов для передачи в дисплей
// col code 1, 3..199
static uint_fast8_t getrow4bytesodd(uint_fast8_t col, uint_fast8_t row, uint_fast8_t stage)
{
	uint_fast8_t r = 0;
	// row/col - координаты пикселя
	r = (r << 2) | getbufferbit(col, row, stage); col += 2;
	r = (r << 2) | getbufferbit(col, row, stage); col += 2;
	r = (r << 2) | getbufferbit(col, row, stage); col += 2;
	r = (r << 2) | getbufferbit(col, row, stage);;
	return r;
}

static void eink_BWdummyline(uint_fast8_t bv)
{
	const spitarget_t target = targetlcd;
	spi_select(target, EINC_SPIMODE);

	spi_progval8_p1(target, 0x70);
	spi_progval8_p2(target, 0x0a);		// 75 байт данных
	spi_complete(target);
	spi_unselect(target);

	spi_select(target, EINC_SPIMODE);
	spi_progval8_p1(target, 0x72);

	spi_progval8_p2(target, bv);
	int n;
	for (n = 0; n < 74; ++ n)
		spi_progval8_p2(target, 0x00);
	spi_complete(target);
	spi_unselect(target);

	eink_write(0x02, 0x07);	// Output data from COG driver to panel.
}

static void eink_dummyline(void)
{
	const spitarget_t target = targetlcd;
	spi_select(target, EINC_SPIMODE);

	spi_progval8_p1(target, 0x70);
	spi_progval8_p2(target, 0x0a);		// 75 байт данных
	spi_complete(target);
	spi_unselect(target);

	spi_select(target, EINC_SPIMODE);
	spi_progval8_p1(target, 0x72);

	while (eink_lcd_busy())
		PRINTF(PSTR("eink: wait busy\n"));
	spi_progval8_p2(target, 0x00);
	int n;
	for (n = 0; n < 74; ++ n)
	{
		while (eink_lcd_busy())
			PRINTF(PSTR("eink: wait busy\n"));
		spi_progval8_p2(target, 0x00);
	}
	spi_complete(target);
	spi_unselect(target);

	eink_write(0x02, 0x07);	// Output data from COG driver to panel.
}


static void eink_displayimage(uint_fast8_t stage)
{
	


	{
		uint_fast8_t state = eink_read(0x0f);		// Check DC/DC
		if ((state & 0x40) != 0)
		{
		}
		else
		{
			PRINTF(PSTR("eink: Feedback to System (14) - dc/dc is not started, state=%02x\n"), state);
		}
	}	

	// 200 x 96
	const spitarget_t target = targetlcd;
	//
	int row;
	for (row = 0; row < 96; ++ row)
	{
		//eink_lcd_toggle_border();
		int col8;
		int row4;
		// send each line
		spi_select(target, EINC_SPIMODE);

		spi_progval8_p1(target, 0x70);
		spi_progval8_p2(target, 0x0a);		// 75 байт данных
		spi_complete(target);
		spi_unselect(target);

		spi_select(target, EINC_SPIMODE);
		spi_progval8_p1(target, 0x72);

		spi_progval8_p2(target, 0x00);	// border byte


		// send 1..25 (50 total) data byte
		// lines 199, 197.. 1
		// line code 198, 196..0
		// Data Bytes
		for (col8 = 0; col8 < 25; ++ col8)
		{
			while (eink_lcd_busy())
				PRINTF(PSTR("eink: wait busy\n"));
			spi_progval8_p2(target, getrow4byteseven(198 - col8 * 8, row, stage));
		}
		// Scan bytes
		// send 1..24 (24 total) scan byte
		for (row4 = 0; row4 < 24; ++ row4)
		{
			while (eink_lcd_busy())
				PRINTF(PSTR("eink: wait busy\n"));
			spi_progval8_p2(target, getscan4bytes(95 - row4 * 4, row, stage));
		}
		// send 26..50 (50 total) data byte
		// lines 2, 4, .. 200
		// line code 1, 3..199
		// Data Bytes
		for (col8 = 0; col8 < 25; ++ col8)
		{
			while (eink_lcd_busy())
				PRINTF(PSTR("eink: wait busy\n"));
			spi_progval8_p2(target, getrow4bytesodd(col8 * 8 + 1, row, stage));
		}
		//spi_progval8_p2(target, 0x00);	// border byte
		spi_complete(target);
		spi_unselect(target);
		//local_delay_ms(20);
		eink_write(0x02, 0x07);	// Output data from COG driver to panel.
		if (stage == 2)
			local_delay_ms(200);	// delay for show results of patern putput
	}
}

void eink_initialize(void)
{
	PRINTF(PSTR("eink test, SPISPEED=%ld\n"), (long) SPISPEED);

	eink_lcd_on_border(1);
	// Section 3: Power On G2 COG Driver
	board_lcd_reset(0); 	// Pull RST pin down
	board_update();
	local_delay_ms(50);
	eink_lcd_pwron(1);
	eink_lcd_discharge(0);		// 1=discharge

	local_delay_ms(50);

	eink_reset();

	int w = 0;

#if 1
	// Section 4: Initialize G2 COG Driver
	PRINTF(PSTR("eink: Section 4: Initialize G2 COG Driver\n"));
	while (eink_lcd_busy() != 0)
	{
		PRINTF(PSTR("eink: wait busy\n"));
		//eink_lcd_toggle_border();
		local_delay_ms(500);
		++ w;
	}
	uint_fast8_t id = eink_read_id();
	if ((id & 0x0f) == 0x02)
	{
		eink_write(0x02, 0x40);	// Disable OE
		const uint_fast8_t status = eink_read(0x0f);	// Check Breakage
		if ((status & 0x80) != 0)
		{
			PRINTF(PSTR("eink: Check Breakage pass\n"));
			eink_write(0x0b, 0x02);	// Power Saving Mode

			// Channel Select
#if 1
			// 2": SPI(0x01, (0x0000,0x0000,0x01FF,0xE000))
			eink_write8bytes(0x01,		// Channel Select
					0x0000,
					0x0000,
					0x01FF,
					0xE000
					);
#else
			// 2.7": SPI(0x01, (0x0000,0x007F,0xFFFE,0x0000))
			eink_write8bytes(0x01,		// Channel Select
					0x0000,
					0x007F,
					0xFFFE,
					0x0000
					);
#endif

			eink_write(0x07, 0xd1);	// High Power Mode Osc Setting
			eink_write(0x08, 0x02);	// Power Setting
			eink_write(0x09, 0xc2);	// Set Vcom level
			eink_write(0x04, 0x03);	// Power Setting
			eink_write(0x03, 0x01);	// Driver latch on
			eink_write(0x03, 0x00);	// Driver latch off
			local_delay_ms(10);		// Delay >= 5ms

			uint_fast8_t i;
			for (i = 0; i < 4; ++ i)
			{
				PRINTF(PSTR("eink: Start chargepump\n"));
				eink_write(0x05, 0x01);	// Start chargepump positive V
				eink_write(0x05, 0x03);	// Start chargepump neg voltage
				eink_write(0x05, 0x0f);	// Set chargepump Vcom_Driver to ON
				local_delay_ms(300);
				const uint_fast8_t status = eink_read(0x0f);	// Check DC/DC
				if ((status & 0x40) != 0)
				{
					eink_write(0x02, 0x40);	// Output enable to disable
					goto sect4done;
				}
			}
			PRINTF(PSTR("eink: Feedback to System (1), status=%02x\n"), status);
		}
		else
		{
			PRINTF(PSTR("eink: Feedback to System (2)\n"));
		}
	}
	else
	{
		PRINTF(PSTR("eink: Feedback to System (3)\n"));
	}
#endif
sect4done:
	eink_write(0x02, 0x40);	// Output enable to disable
	PRINTF(PSTR("eink: Section 4: Initialize G2 COG Driver done\n"));
	eink_lcd_on_border(1);

	for (;;)
	{
		eink_displayimage(0);
		eink_displayimage(1);
		eink_displayimage(2);
	}

	/*
	eink_BWdummyline(0xff);
	eink_BWdummyline(0xaa);
	eink_dummyline();
	*/
	PRINTF(PSTR("eink reset done, w = %d\n"), w);
	for (;;)
	{
		//eink_lcd_toggle_border();
		local_delay_ms(500);
		uint_fast8_t id = eink_read_id();

		uint_fast8_t v2 = eink_read(0x02);
		const uint_fast8_t status = eink_read(0x0f);	// Check DC/DC

		PRINTF(PSTR("eink id=%02x.v2=%02x,status=%02x\n"), id, v2, status);
		if (id != 0xff)
			break;
	}
	// 6 Power off G2 COG Driver
	{
		uint_fast8_t state = eink_read(0x0f);		// Check DC/DC
		if ((state & 0x40) != 0)
		{
			eink_write(0x03, 0x01);	// Turn on Latch Reset
			eink_write(0x02, 0x05);	// Turn off OE
			eink_write(0x05, 0x0e);	// Power off positive charge pump
			eink_write(0x05, 0x02);	// Power off charge pump Vcom
			eink_write(0x05, 0x00);	// Turn off all charge pumps
			eink_write(0x07, 0x0d);	// Turn off OSC
			eink_write(0x04, 0x83);	// Discharge internal
			local_delay_ms(150);	// Delay >= 120ms
			eink_write(0x04, 0x00);	// Turn off discharge internal
		}
		else
		{
			PRINTF(PSTR("eink: Feedback to System (4) - dc/dc is not started\n"));
		}
	}
	PRINTF(PSTR("eink test done, w = %d\n"), w);
	board_lcd_reset(0); 	// Pull RST pin down
	board_update();
	eink_lcd_discharge(1);		// 1=discharge
	eink_lcd_pwron(0);
	PRINTF(PSTR("eink reset\n"));
}


#endif /* ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED */

#if 1 && ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED
/**
* \file
*
* \brief The waveform driving processes and updating stages of G2 COG with V230 EPD
*
* Copyright (c) 2012-2014 Pervasive Displays Inc. All rights reserved.
*
* \asf_license_start
*
* \page License
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. The name of Atmel may not be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* 4. This software may only be redistributed and used in connection with an
*    Atmel microcontroller product.
*
* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* \asf_license_stop
**/

//#include "EPD_COG_Process.h"

#define COG_V230_G2 1


#define USE_EPD144 0 /**< 1.44 inch PDi EPD */
#define USE_EPD200 1 /**< 2 inch PDi EPD */
#define USE_EPD270 2 /**< 2.7 inch PDi EPD */

/* Using which EPD size for demonstration. 
 * Change to USE_EPD270 if you are connecting with 2.7 inch EPD. USE_EPD144 for 1.44 inch. */
#define USE_EPD_Type USE_EPD200 /**< default is connecting with 2 inch EPD */

#if (USE_EPD_Type==USE_EPD144) // array size=96*16, (128/8=16)
	extern uint8_t const image_array_144_1[]; /**< use 1.44" image array #1 */
	extern uint8_t const image_array_144_2[]; /**< use 1.44" image array #2 */
#elif (USE_EPD_Type==USE_EPD200) // array size=96*25, (200/8=25)
	extern uint8_t const image_array_200_1[]; /**< use 2" image array #1 */
	extern uint8_t const image_array_200_2[]; /**< use 2" image array #2 */
#elif (USE_EPD_Type==USE_EPD270) // array size=176*33, (264/8=33)
	extern uint8_t const image_array_270_1[]; /**< use 2.7" image array #1 */
	extern uint8_t const image_array_270_2[]; /**< use 2.7" image array #2 */
#endif


#if (USE_EPD_Type==USE_EPD144)
#define image_width_144 128 /**< the resolution of 1.44 inch is 128*96 */
#define image_height_144 96 /**< the image array size of 1.44 inch is 96*16 where 16=128/8 */
uint8_t const image_array_144_1[] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 1
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 2
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x3C, 0xFF, 0x00, 0x03,  // 3
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x3C, 0xFF, 0x00, 0x03,  // 4
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0xFF, 0x33, 0xFF, 0x3F, 0xF3,  // 5
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0xFF, 0x33, 0xFF, 0x3F, 0xF3,  // 6
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0x03, 0x33, 0x0F, 0x30, 0x33,  // 7
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0x03, 0x33, 0x0F, 0x30, 0x33,  // 8
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0x03, 0x3C, 0xCF, 0x30, 0x33,  // 9
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0x03, 0x3C, 0xCF, 0x30, 0x33,  // 10
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0x03, 0x30, 0x0F, 0x30, 0x33,  // 11
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0x03, 0x30, 0x0F, 0x30, 0x33,  // 12
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0xFF, 0x30, 0x0F, 0x3F, 0xF3,  // 13
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0xFF, 0x30, 0x0F, 0x3F, 0xF3,  // 14
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x33, 0x33, 0x00, 0x03,  // 15
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x33, 0x33, 0x00, 0x03,  // 16
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0xCF, 0xFF, 0xFF,  // 17
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0xCF, 0xFF, 0xFF,  // 18
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xCF, 0x0F, 0x03, 0x03, 0x0F,  // 19
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xCF, 0x0F, 0x03, 0x03, 0x0F,  // 20
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0C, 0xFC, 0x3C, 0xC0, 0xFF,  // 21
  0xFF, 0xFF, 0x1E, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0xFF, 0x0C, 0xFC, 0x3C, 0xC0, 0xFF,  // 22
  0xFF, 0xFE, 0x1E, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0xFF, 0x0F, 0x3F, 0xC3, 0x0F, 0x33,  // 23
  0xFF, 0xFE, 0x1E, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0xFF, 0x0F, 0x3F, 0xC3, 0x0F, 0x33,  // 24
  0xFF, 0xFC, 0x1E, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0xF0, 0x0F, 0xCF, 0x3F, 0x0C, 0xF3,  // 25
  0xFF, 0xF8, 0x1E, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0xF0, 0x0F, 0xCF, 0x3F, 0x0C, 0xF3,  // 26
  0xFF, 0xF0, 0x9E, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0xFC, 0x0C, 0x0F, 0xF3, 0xCF, 0x03,  // 27
  0xFF, 0xE1, 0x9C, 0x01, 0xF0, 0x38, 0x3F, 0xE0, 0x3F, 0x1F, 0xFC, 0x0C, 0x0F, 0xF3, 0xCF, 0x03,  // 28
  0xFF, 0xC3, 0x9C, 0x01, 0xE0, 0x00, 0x0F, 0x80, 0x0F, 0x1F, 0xFF, 0xFF, 0xF0, 0x03, 0x0F, 0xFF,  // 29
  0xFF, 0xC7, 0x9C, 0x01, 0xC0, 0x00, 0x07, 0x80, 0x07, 0x1F, 0xFF, 0xFF, 0xF0, 0x03, 0x0F, 0xFF,  // 30
  0xFF, 0x0F, 0x9E, 0x3F, 0x8F, 0x87, 0xC3, 0x0F, 0xC3, 0x1F, 0xF0, 0x00, 0x30, 0xC0, 0xC0, 0x0F,  // 31
  0xFF, 0x0F, 0x9E, 0x3F, 0x8F, 0xC7, 0xC3, 0x1F, 0xC3, 0x1F, 0xF0, 0x00, 0x30, 0xC0, 0xC0, 0x0F,  // 32
  0xFE, 0x03, 0x9E, 0x3F, 0x8F, 0xC7, 0xC3, 0x00, 0x03, 0x1F, 0xF3, 0xFF, 0x3F, 0xCF, 0xCC, 0xCF,  // 33
  0xFE, 0x03, 0x9E, 0x3F, 0x8F, 0xC7, 0xE3, 0x00, 0x03, 0x1F, 0xF3, 0xFF, 0x3F, 0xCF, 0xCC, 0xCF,  // 34
  0xF8, 0x03, 0x9E, 0x3F, 0x8F, 0xC7, 0xE3, 0x00, 0x03, 0x1F, 0xF3, 0x03, 0x3C, 0x00, 0xFF, 0xC3,  // 35
  0xF0, 0xFF, 0x9E, 0x3F, 0x8F, 0xC7, 0xE3, 0x1F, 0xFF, 0x1F, 0xF3, 0x03, 0x3C, 0x00, 0xFF, 0xC3,  // 36
  0xE1, 0xFF, 0x9E, 0x3F, 0x8F, 0xC7, 0xE3, 0x1F, 0xFF, 0x1F, 0xF3, 0x03, 0x30, 0xF3, 0xF3, 0xF3,  // 37
  0xE3, 0xFF, 0x9F, 0x0F, 0x8F, 0xC7, 0xE3, 0x0F, 0xC3, 0x0F, 0xF3, 0x03, 0x30, 0xF3, 0xF3, 0xF3,  // 38
  0xC7, 0xFF, 0x9F, 0x01, 0x8F, 0xC7, 0xE3, 0x87, 0x87, 0x81, 0xF3, 0x03, 0x3C, 0xC3, 0xC3, 0x33,  // 39
  0x87, 0xFF, 0x9F, 0x81, 0x8F, 0xC7, 0xE3, 0x80, 0x0F, 0xC1, 0xF3, 0x03, 0x3C, 0xC3, 0xC3, 0x33,  // 40
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0xFF, 0xFF, 0xF3, 0xFF, 0x33, 0x3C, 0xC3, 0xFF,  // 41
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0xFF, 0x33, 0x3C, 0xC3, 0xFF,  // 42
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x30, 0xF3, 0x3F, 0x0F,  // 43
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x30, 0xF3, 0x3F, 0x0F,  // 44
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 45
  0xFF, 0xFF, 0xFF, 0xFE, 0x0F, 0x83, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 46
  0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 47
  0xFF, 0xFF, 0xFF, 0xFF, 0x86, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 48
  0xFF, 0xFF, 0xFF, 0xFF, 0x82, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 49
  0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 50
  0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 51
  0x81, 0x81, 0xC0, 0xFF, 0xE0, 0x3E, 0x07, 0x3F, 0xC7, 0x86, 0x7D, 0x81, 0x81, 0xFF, 0xFF, 0xFF,  // 52
  0x9F, 0x9C, 0xCE, 0x7F, 0xF0, 0x7E, 0x73, 0x3F, 0xC7, 0xCE, 0x3D, 0x9F, 0x9C, 0xFF, 0xFF, 0xFF,  // 53
  0x9F, 0x9C, 0xCF, 0x3F, 0xE0, 0x3E, 0x73, 0x3F, 0x93, 0xCE, 0x9D, 0x9F, 0x9E, 0x7F, 0xFF, 0xFF,  // 54
  0x9F, 0x9C, 0xCF, 0x3F, 0xE0, 0x3E, 0x73, 0x3F, 0x93, 0xCE, 0x9D, 0x9F, 0x9E, 0x7C, 0x30, 0xE3,  // 55
  0x81, 0x9C, 0xCF, 0x3F, 0xC0, 0x1E, 0x73, 0x3F, 0x93, 0xCE, 0xCD, 0x81, 0x9E, 0x7D, 0xB6, 0x59,  // 56
  0x9F, 0x81, 0xCF, 0x3F, 0x82, 0x0E, 0x07, 0x3F, 0x93, 0xCE, 0xE5, 0x9F, 0x9E, 0x7D, 0xB6, 0x9D,  // 57
  0x9F, 0x9F, 0xCF, 0x3F, 0x87, 0x0E, 0x7F, 0x3F, 0x01, 0xCE, 0xE5, 0x9F, 0x9E, 0x7C, 0x31, 0x9D,  // 58
  0x9F, 0x9F, 0xCE, 0x7F, 0x07, 0x06, 0x7F, 0x3F, 0x39, 0xCE, 0xF1, 0x9F, 0x9C, 0xFD, 0xF4, 0xD9,  // 59
  0x81, 0x9F, 0xC0, 0xFE, 0x0F, 0x82, 0x7F, 0x01, 0x39, 0x86, 0xF9, 0x81, 0x81, 0xFD, 0xF6, 0x43,  // 60
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 61
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 62
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 63
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 64
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 65
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 66
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 67
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 68
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 69
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 70
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 71
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 72
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 73
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 74
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 75
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 76
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 77
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 78
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 79
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xFF, 0xFF, 0xFF, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 80
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCF, 0xFF, 0xFF, 0xFF, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 81
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 82
  0xF0, 0x60, 0xC0, 0xCF, 0xBB, 0xE0, 0xC9, 0xF2, 0x0E, 0x03, 0x9C, 0x30, 0x67, 0xF9, 0x9F, 0x63,  // 83
  0xF3, 0x27, 0xCC, 0x47, 0x33, 0xCC, 0xCD, 0xF6, 0x7E, 0x79, 0x99, 0xF3, 0x27, 0xF9, 0xCE, 0x5D,  // 84
  0xF3, 0xA7, 0xCE, 0x47, 0x31, 0xCF, 0xCC, 0xE6, 0x7E, 0x7C, 0x9B, 0xF3, 0xA7, 0xF8, 0xCE, 0xDF,  // 85
  0xF3, 0xA7, 0xCE, 0x67, 0x61, 0xC7, 0xCC, 0xE6, 0x7E, 0x7C, 0x99, 0xF3, 0xA7, 0xF6, 0xE4, 0xCF,  // 86
  0xF3, 0x20, 0xCC, 0xE2, 0x68, 0xE3, 0xCE, 0x6E, 0x0E, 0x7E, 0x9C, 0x72, 0x27, 0xF6, 0x71, 0xE3,  // 87
  0xF0, 0x67, 0xC1, 0xF2, 0xCC, 0xF1, 0xCE, 0x6E, 0x7E, 0x7E, 0x9E, 0x30, 0xE7, 0xE0, 0x7B, 0xF9,  // 88
  0xF3, 0xE7, 0xC0, 0xF0, 0xC0, 0xF9, 0xCE, 0x4E, 0x7E, 0x7C, 0x9F, 0x93, 0xE7, 0xEE, 0x7B, 0xFD,  // 89
  0xF3, 0xE7, 0xCC, 0xF0, 0x9E, 0x7C, 0xCF, 0x1E, 0x7E, 0x7C, 0x9F, 0x93, 0xE7, 0xCF, 0x3B, 0xFD,  // 90
  0xF3, 0xE7, 0xCC, 0x79, 0x9E, 0x4C, 0xCF, 0x1E, 0x7E, 0x79, 0x9B, 0xB3, 0xE7, 0xDF, 0x3B, 0xDD,  // 91
  0xF3, 0xE0, 0xCE, 0x79, 0xBF, 0x61, 0xCF, 0xBE, 0x0E, 0x03, 0x98, 0x73, 0xE0, 0x5F, 0x9B, 0xE3,  // 92
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 93
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCF, 0xFF, 0xFF, 0xFF, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 94
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 95
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF   // 96
};
//2013/04/03 Resolution: 128 x 96


uint8_t const image_array_144_2[] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 1
  0xFF, 0xFF, 0xFF, 0xFF, 0xE3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 2
  0xFF, 0xFF, 0xC0, 0x3C, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 3
  0xFF, 0xFE, 0x1E, 0x10, 0x78, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 4
  0xFF, 0xFC, 0x30, 0x7F, 0x0C, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 5
  0xFF, 0xF8, 0x67, 0xFF, 0xE2, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 6
  0xFF, 0xF8, 0x4F, 0xFF, 0xFA, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 7
  0xFF, 0xFA, 0x1F, 0xFF, 0xFC, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 8
  0xFF, 0xF8, 0x3F, 0xFF, 0xBC, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 9
  0xFF, 0xF8, 0x3B, 0xFF, 0x99, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 10
  0xFF, 0xFD, 0xF9, 0xFF, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 11
  0xFF, 0xFF, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 12
  0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 13
  0xFF, 0xFF, 0xFE, 0x07, 0x80, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 14
  0xFF, 0xFF, 0xF9, 0xF3, 0x23, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 15
  0xFF, 0xFF, 0xF3, 0xC0, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 16
  0xFF, 0xBB, 0xE6, 0x00, 0x1C, 0x2D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 17
  0xFF, 0x73, 0x98, 0x1C, 0x3F, 0xCD, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x1F, 0xFF, 0xE0, 0x1F, 0xFF,  // 18
  0xFF, 0x71, 0x30, 0xFC, 0x41, 0x0D, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x07, 0xFF, 0x80, 0x07, 0xFF,  // 19
  0xFF, 0x70, 0x03, 0xC0, 0x60, 0x1D, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x07, 0xFE, 0x00, 0x07, 0xFF,  // 20
  0xFE, 0x70, 0x07, 0x0C, 0x1F, 0xBD, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0x07, 0xFC, 0x00, 0x07, 0xFF,  // 21
  0xFE, 0x73, 0x3E, 0x3C, 0xC0, 0x3D, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x07, 0xF8, 0x00, 0x07, 0xFF,  // 22
  0xFE, 0x78, 0xF8, 0x79, 0xE0, 0xFB, 0xFF, 0xFF, 0xFF, 0xF0, 0x0F, 0xC7, 0xF0, 0x0F, 0xC7, 0xFF,  // 23
  0xFE, 0x78, 0x01, 0xF1, 0xFF, 0xFB, 0xFF, 0xFF, 0xFF, 0xF0, 0x3F, 0xFF, 0xF0, 0x3F, 0xFF, 0xFF,  // 24
  0xFE, 0x7C, 0x0F, 0xE3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x3F, 0xFF, 0xF0, 0x3F, 0xFF, 0xFF,  // 25
  0xFF, 0x3F, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x7F, 0xFF, 0xE0, 0x7F, 0xFF, 0xFF,  // 26
  0xFF, 0x3F, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF,  // 27
  0xFF, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF,  // 28
  0xFF, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xE0, 0x3F, 0xC0, 0xE0, 0x3F, 0xFF,  // 29
  0xFF, 0xC7, 0xFF, 0xFF, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x0F, 0xC0, 0x00, 0x0F, 0xFF,  // 30
  0xFF, 0xF3, 0xF9, 0xFF, 0xE7, 0x87, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x07, 0xC0, 0x00, 0x07, 0xFF,  // 31
  0xFF, 0xF8, 0xF8, 0x3F, 0xC7, 0x87, 0xFF, 0xFF, 0xEF, 0xC0, 0x00, 0x03, 0xC0, 0x00, 0x03, 0xFF,  // 32
  0xFF, 0xFF, 0xFC, 0x07, 0x07, 0xB7, 0xFF, 0xFF, 0xEF, 0xC0, 0x00, 0x03, 0xC0, 0x00, 0x03, 0xFF,  // 33
  0xFF, 0xFF, 0xFC, 0x00, 0x07, 0xB7, 0xFF, 0xFF, 0x83, 0xC0, 0x3E, 0x01, 0xC0, 0x3E, 0x01, 0xFF,  // 34
  0xFF, 0xFF, 0xFC, 0x00, 0x07, 0x37, 0xFF, 0xFF, 0x39, 0xC0, 0xFF, 0x01, 0xC0, 0xFF, 0x01, 0xFF,  // 35
  0xFF, 0xFF, 0xFC, 0x00, 0x07, 0x77, 0xFF, 0xFE, 0x7F, 0xC0, 0xFF, 0x81, 0xC0, 0xFF, 0x81, 0xFF,  // 36
  0xFF, 0xFF, 0xFE, 0x01, 0x07, 0x77, 0xFF, 0xFE, 0x7F, 0xC0, 0xFF, 0x81, 0xC0, 0xFF, 0x81, 0xFF,  // 37
  0xFF, 0xFF, 0xFE, 0x03, 0xE4, 0x76, 0x1F, 0xFE, 0x7F, 0xC0, 0xFF, 0x81, 0xC0, 0xFF, 0x81, 0xFF,  // 38
  0xFF, 0xFF, 0xFE, 0x0F, 0xE0, 0x70, 0x9F, 0xFF, 0x1F, 0xC0, 0xFF, 0x81, 0xC0, 0xFF, 0x81, 0xFF,  // 39
  0xFF, 0x87, 0xFE, 0x1F, 0xCF, 0x73, 0xC0, 0x7F, 0x83, 0xC0, 0xFF, 0x81, 0xC0, 0xFF, 0x81, 0xFF,  // 40
  0xFE, 0x03, 0xF8, 0x30, 0x1F, 0x73, 0xE7, 0x3F, 0xF1, 0xE0, 0x7F, 0x01, 0xE0, 0x7F, 0x01, 0xFF,  // 41
  0xFC, 0x79, 0xF3, 0x87, 0x1F, 0x77, 0xEF, 0x3F, 0xF9, 0xE0, 0x7F, 0x03, 0xE0, 0x7F, 0x03, 0xFF,  // 42
  0xF9, 0xFC, 0xF3, 0x9F, 0x3F, 0x66, 0xEF, 0x7F, 0xFD, 0xE0, 0x3E, 0x03, 0xE0, 0x3E, 0x03, 0xFF,  // 43
  0xF3, 0xFE, 0xF3, 0xBF, 0xB7, 0x66, 0xFC, 0x7F, 0xF9, 0xF0, 0x00, 0x07, 0xF0, 0x00, 0x07, 0xFF,  // 44
  0xE7, 0xFE, 0x70, 0x3F, 0xB7, 0x6E, 0xF8, 0xFE, 0xF9, 0xF0, 0x00, 0x07, 0xF0, 0x00, 0x07, 0xFF,  // 45
  0xEF, 0x8F, 0x61, 0x9F, 0xB7, 0x6F, 0xD9, 0xFF, 0x07, 0xF8, 0x00, 0x0F, 0xF8, 0x00, 0x0F, 0xFF,  // 46
  0xCF, 0x8F, 0x09, 0x99, 0xB7, 0x6F, 0x9D, 0xFF, 0xDF, 0xFC, 0x00, 0x3F, 0xFC, 0x00, 0x3F, 0xFF,  // 47
  0x8F, 0x87, 0x0D, 0xD9, 0xF7, 0x6F, 0x1D, 0xFF, 0xDF, 0xFF, 0x80, 0xFF, 0xFF, 0x80, 0xFF, 0xFF,  // 48
  0x8F, 0xC7, 0xFD, 0xD9, 0xF7, 0x6E, 0x1D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 49
  0xCF, 0xC7, 0xF9, 0xDD, 0xF7, 0x6E, 0x0D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 50
  0xC1, 0xE7, 0xF9, 0xDD, 0xFF, 0x6F, 0x0D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 51
  0xE1, 0xE3, 0xF9, 0xDD, 0x9F, 0xEF, 0xBD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 52
  0xE1, 0xE7, 0xB8, 0xDD, 0x9F, 0x67, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 53
  0xF0, 0xE7, 0x38, 0xDD, 0xCF, 0x00, 0x7B, 0xFF, 0xFF, 0xFF, 0xFF, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF,  // 54
  0xFC, 0xF7, 0x38, 0xED, 0x87, 0x00, 0x63, 0xFC, 0xF1, 0xE3, 0xFF, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF,  // 55
  0xFC, 0xFF, 0x38, 0xEC, 0x06, 0x7E, 0x07, 0xFE, 0xEE, 0xDD, 0xFF, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF,  // 56
  0xFC, 0x7F, 0x1C, 0xFC, 0x0E, 0xFF, 0x9F, 0xFE, 0xFE, 0xDD, 0xC3, 0xEF, 0x9F, 0x87, 0x8E, 0x3F,  // 57
  0xFE, 0x7E, 0x1C, 0xE0, 0xDE, 0xFF, 0xFF, 0xFE, 0xF1, 0xDD, 0xB7, 0xDF, 0xDF, 0xBB, 0x7D, 0xFF,  // 58
  0xFE, 0x7E, 0x9C, 0x43, 0xDC, 0xFF, 0xFF, 0xFE, 0xFE, 0xDD, 0xB7, 0xDF, 0xDF, 0xBB, 0x7C, 0xFF,  // 59
  0xFE, 0x3C, 0x9C, 0x0F, 0x89, 0xFF, 0xFF, 0xFE, 0xFE, 0xDD, 0xCF, 0xDF, 0xDF, 0xBB, 0x7F, 0x3F,  // 60
  0xFF, 0x3D, 0xCE, 0x1F, 0x81, 0xFF, 0xFF, 0xFE, 0xEE, 0xDD, 0xBF, 0xBF, 0xDF, 0xBB, 0x7F, 0xBF,  // 61
  0xFF, 0x1C, 0xCC, 0x7F, 0xC7, 0xFF, 0xFF, 0xFC, 0x71, 0xE3, 0xC7, 0xBF, 0xDF, 0x87, 0x8C, 0x7F,  // 62
  0xFF, 0x1E, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBB, 0x7F, 0x8F, 0xBF, 0xFF, 0xFF,  // 63
  0xFF, 0x9E, 0xC3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0x7F, 0xFF, 0xBF, 0xFF, 0xFF,  // 64
  0xFF, 0x8E, 0x67, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 65
  0xFF, 0xCF, 0x6F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 66
  0xFF, 0xCF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 67
  0xFF, 0xC7, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 68
  0xFF, 0xE6, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 69
  0xFF, 0xE0, 0xFC, 0xB1, 0xA6, 0x37, 0x4E, 0x3A, 0x4F, 0x59, 0x09, 0x61, 0xA7, 0x14, 0xFF, 0xFF,  // 70
  0xFF, 0xE1, 0xFC, 0xB1, 0xA6, 0x37, 0x4E, 0x3A, 0x4F, 0x59, 0x09, 0x61, 0xA7, 0x14, 0xFF, 0xFF,  // 71
  0xFF, 0xF3, 0xFC, 0xB1, 0xA6, 0x37, 0x4E, 0x3A, 0x4F, 0x59, 0x09, 0x61, 0xA7, 0x14, 0xFF, 0xFF,  // 72
  0xFF, 0xF3, 0xFC, 0xB1, 0xA6, 0x37, 0x4E, 0x3A, 0x4F, 0x59, 0x09, 0x61, 0xA7, 0x14, 0xFF, 0xFF,  // 73
  0xFF, 0xFF, 0xFC, 0xB1, 0xA6, 0x37, 0x4E, 0x3A, 0x4F, 0x59, 0x09, 0x61, 0xA7, 0x14, 0xFF, 0xFF,  // 74
  0xFF, 0xFF, 0xFC, 0xB1, 0xA6, 0x37, 0x4E, 0x3A, 0x4F, 0x59, 0x09, 0x61, 0xA7, 0x14, 0xFF, 0xFF,  // 75
  0xFF, 0xFF, 0xFC, 0xB1, 0xA6, 0x37, 0x4E, 0x3A, 0x4F, 0x59, 0x09, 0x61, 0xA7, 0x14, 0xFF, 0xFF,  // 76
  0xFF, 0xFF, 0xFC, 0xB1, 0xA6, 0x37, 0x4E, 0x3A, 0x4F, 0x59, 0x09, 0x61, 0xA7, 0x14, 0xFF, 0xFF,  // 77
  0xFF, 0xFF, 0xFC, 0xB1, 0xA6, 0x37, 0x4E, 0x3A, 0x4F, 0x59, 0x09, 0x61, 0xA7, 0x14, 0xFF, 0xFF,  // 78
  0xFF, 0xFF, 0xFC, 0xB1, 0xA6, 0x37, 0x4E, 0x3A, 0x4F, 0x59, 0x09, 0x61, 0xA7, 0x14, 0xFF, 0xFF,  // 79
  0xFF, 0xFF, 0xFC, 0xB1, 0xA6, 0x37, 0x4E, 0x3A, 0x4F, 0x59, 0x09, 0x61, 0xA7, 0x14, 0xFF, 0xFF,  // 80
  0xFF, 0xFF, 0xFC, 0xB1, 0xA6, 0x37, 0x4E, 0x3A, 0x4F, 0x59, 0x09, 0x61, 0xA7, 0x14, 0xFF, 0xFF,  // 81
  0xFF, 0xFF, 0xFC, 0xB1, 0xA6, 0x37, 0x4E, 0x3A, 0x4F, 0x59, 0x09, 0x61, 0xA7, 0x14, 0xFF, 0xFF,  // 82
  0xFF, 0xFF, 0xFC, 0xB1, 0xA6, 0x37, 0x4E, 0x3A, 0x4F, 0x59, 0x09, 0x61, 0xA7, 0x14, 0xFF, 0xFF,  // 83
  0xFF, 0xFF, 0xFC, 0xB1, 0xA6, 0x37, 0x4E, 0x3A, 0x4F, 0x59, 0x09, 0x61, 0xA7, 0x14, 0xFF, 0xFF,  // 84
  0xFF, 0xFF, 0xFC, 0xB1, 0xA6, 0x37, 0x4E, 0x3A, 0x4F, 0x59, 0x09, 0x61, 0xA7, 0x14, 0xFF, 0xFF,  // 85
  0xFF, 0xFF, 0xFC, 0xB1, 0xA6, 0x37, 0x4E, 0x3A, 0x4F, 0x59, 0x09, 0x61, 0xA7, 0x14, 0xFF, 0xFF,  // 86
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 87
  0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xC7, 0x1F, 0x70, 0xC6, 0x0C, 0x71, 0xC7, 0xFF, 0xFF, 0xFF, 0xFF,  // 88
  0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0xBA, 0xEE, 0x77, 0xBB, 0xDB, 0xAE, 0xBB, 0xFF, 0xFF, 0xFF, 0xFF,  // 89
  0xFF, 0xFF, 0xFF, 0xFF, 0xEB, 0xFB, 0xED, 0x6F, 0xBF, 0xDB, 0xAE, 0xBB, 0xFF, 0xFF, 0xFF, 0xFF,  // 90
  0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xFB, 0x9D, 0x61, 0x87, 0xBC, 0x6E, 0xBB, 0xFF, 0xFF, 0xFF, 0xFF,  // 91
  0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xF7, 0xEB, 0x7E, 0xBB, 0xBB, 0xB0, 0xBB, 0xFF, 0xFF, 0xFF, 0xFF,  // 92
  0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xEF, 0xE8, 0x3E, 0xBB, 0x7B, 0xBE, 0xBB, 0xFF, 0xFF, 0xFF, 0xFF,  // 93
  0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xDE, 0xEF, 0x6E, 0xBB, 0x7B, 0xAE, 0xBB, 0xFF, 0xFF, 0xFF, 0xFF,  // 94
  0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0x83, 0x1F, 0x71, 0xC7, 0x7C, 0x71, 0xC7, 0xFF, 0xFF, 0xFF, 0xFF,  // 95
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF   // 96
};
//2013/04/03 Resolution: 128 x 96


#elif (USE_EPD_Type==USE_EPD200)

#define image_width_200 200 /**< the resolution of 2 inch is 200*96 */
#define image_height_200 96 /**< the image array size of 2 inch is 96*25 where 25=200/8 */
uint8_t const image_array_200_1[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xfe,0x00,0x00,0x00,0xc0,0x00,0x00,0x00,0x00,
0x60,0x00,0x00,0x7f,0xff,0x80,0x00,0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x00,0x3f,
0xff,0x00,0x00,0x01,0xe0,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0xff,0xff,0xc0,0x00,
0x00,0x00,0x00,0x0f,0x00,0x00,0x00,0x00,0x3f,0xff,0x80,0x00,0x01,0xe0,0x00,0x00,
0x00,0x00,0xf0,0x00,0x00,0x7f,0xff,0x80,0x00,0x00,0x00,0x00,0x0f,0x00,0x00,0x00,
0x00,0x3c,0x0f,0xc0,0x00,0x01,0xe0,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x01,0xe0,
0x00,0x00,0x00,0x00,0x00,0x0f,0x00,0x00,0x00,0x00,0x3c,0x07,0xc0,0x00,0x01,0xe0,
0x00,0x00,0x00,0x00,0xf0,0x00,0x00,0x01,0xe0,0x00,0x00,0x00,0x00,0x00,0x0f,0x00,
0x00,0x00,0x00,0x3c,0x03,0xc0,0x00,0x01,0xe0,0x00,0x00,0x00,0x00,0xf0,0x00,0x00,
0x01,0xe0,0x00,0x00,0x00,0x00,0x00,0x0f,0x00,0x00,0x00,0x00,0x3c,0x03,0xc0,0x7e,
0x07,0xfc,0x67,0xc0,0x7e,0x00,0xf0,0x63,0xe0,0x01,0xe0,0x06,0x7c,0x0f,0xe0,0x07,
0xcf,0x00,0xfc,0x00,0x00,0x3c,0x03,0xc1,0xff,0x87,0xfc,0xff,0xe1,0xff,0x80,0xf0,
0xf7,0xf8,0x01,0xe0,0x0f,0xfe,0x3f,0xf8,0x1f,0xef,0x03,0xff,0x00,0x00,0x3c,0x07,
0xc3,0xff,0xc7,0xfc,0xff,0xe3,0xff,0xc0,0xf0,0xff,0xf8,0x01,0xe0,0x0f,0xfe,0x7f,
0xf8,0x3f,0xff,0x07,0xff,0x80,0x00,0x3c,0x0f,0x87,0x83,0xc1,0xe0,0xf8,0x47,0xc3,
0xe0,0xf0,0xfc,0x7c,0x01,0xe0,0x0f,0x84,0x78,0x7c,0x3e,0x3f,0x0f,0x07,0x80,0x00,
0x3f,0xff,0x8f,0x01,0xe1,0xe0,0xf8,0x07,0x81,0xe0,0xf0,0xf8,0x3c,0x01,0xe0,0x0f,
0x80,0x70,0x3c,0x7c,0x1f,0x1e,0x03,0xc0,0x00,0x3f,0xff,0x0f,0x01,0xe1,0xe0,0xf0,
0x0f,0x00,0xf0,0xf0,0xf0,0x3c,0x01,0xe0,0x0f,0x00,0x00,0x7c,0x78,0x1f,0x1e,0x03,
0xc0,0x00,0x3f,0xfc,0x0f,0xff,0xe1,0xe0,0xf0,0x0f,0x00,0xf0,0xf0,0xf0,0x3c,0x01,
0xe0,0x0f,0x00,0x07,0xfc,0x78,0x0f,0x1f,0xff,0xc0,0x00,0x3c,0x00,0x0f,0xff,0xc1,
0xe0,0xf0,0x0f,0x00,0xf0,0xf0,0xf0,0x3c,0x01,0xe0,0x0f,0x00,0x3f,0xfc,0x78,0x0f,
0x1f,0xff,0x80,0x00,0x3c,0x00,0x0f,0x00,0x01,0xe0,0xf0,0x0f,0x00,0xf0,0xf0,0xf0,
0x3c,0x01,0xe0,0x0f,0x00,0x7f,0x3c,0x78,0x0f,0x1e,0x00,0x00,0x00,0x3c,0x00,0x0f,
0x00,0x41,0xe0,0xf0,0x0f,0x00,0xf0,0xf0,0xf0,0x3c,0x01,0xe0,0x0f,0x00,0xf0,0x3c,
0x78,0x0f,0x1e,0x00,0x80,0x00,0x3c,0x00,0x0f,0x81,0xe1,0xe0,0xf0,0x07,0x81,0xe0,
0xf0,0xf0,0x3c,0x01,0xe0,0x0f,0x00,0xf0,0x3c,0x7c,0x1f,0x1f,0x03,0xc0,0x00,0x3c,
0x00,0x07,0xc3,0xc1,0xe0,0xf0,0x07,0xc3,0xe0,0xf0,0xf0,0x3c,0x01,0xe0,0x0f,0x00,
0xf8,0xfc,0x3e,0x3f,0x0f,0x87,0x80,0x00,0x3c,0x00,0x03,0xff,0xc1,0xfe,0xf0,0x03,
0xff,0xc0,0xf0,0xf0,0x3c,0x01,0xe0,0x0f,0x00,0xff,0xfe,0x3f,0xff,0x07,0xff,0x80,
0x00,0x3c,0x00,0x01,0xff,0x80,0xfe,0xf0,0x01,0xff,0x80,0xf0,0xf0,0x3c,0x01,0xe0,
0x0f,0x00,0x7f,0x9e,0x1f,0xef,0x03,0xff,0x00,0x00,0x18,0x00,0x00,0x7e,0x00,0x7c,
0x60,0x00,0x7e,0x00,0x60,0x60,0x18,0x00,0xc0,0x06,0x00,0x3e,0x0c,0x07,0x86,0x00,
0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7e,0x00,0x3e,0x00,
0x03,0x00,0x3f,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x01,0xff,0x80,0xff,0x80,0x07,0x80,0x7f,0xf8,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff,0xc1,
0xff,0xc0,0x07,0x80,0x7f,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xc3,0xc3,0xe3,0xe0,0x0f,0x80,0x70,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0x81,0xe3,0xc1,0xe0,0x3f,0x80,0x70,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x81,0xe3,0xc0,0xe0,0x7f,0x80,0x70,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x07,0x01,0xe7,0x80,0xf1,0xf7,0x80,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x01,0xe7,0x80,0xf1,0xe7,
0x80,0xf7,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x03,0xe7,0x80,0xf1,0xc7,0x80,0xff,0xf0,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xc7,0x80,
0xf0,0x07,0x80,0xff,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x87,0x80,0xf0,0x07,0x80,0xf0,0xf8,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,
0x87,0x80,0xf0,0x07,0x80,0xe0,0x7c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x07,0x80,0xf0,0x07,0x80,0x00,0x3c,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x7c,0x07,0x80,0xf0,0x07,0x80,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0x07,0x80,0xf0,0x07,0x80,
0xc0,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x01,0xf0,0x03,0xc1,0xe0,0x07,0x81,0xe0,0x3c,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xe0,0x03,0xc1,0xe0,
0x07,0x81,0xe0,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x07,0xc0,0x03,0xe3,0xe0,0x07,0x80,0xf0,0xf8,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xff,0xe1,
0xff,0xc0,0x07,0x80,0xff,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xff,0xe0,0xff,0x80,0x07,0x80,0x7f,0xe0,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,
0xff,0xe0,0x3e,0x00,0x03,0x00,0x1f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
//2013/04/03 Resolution: 200 x 96

uint8_t const image_array_200_2[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xff,0x80,0x00,0x60,0x70,0x01,0x81,0x80,0x1c,
0x00,0x00,0x06,0x06,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x3f,
0xff,0x80,0x00,0xf0,0xf8,0x03,0xc3,0xc0,0x3c,0x00,0x00,0x0f,0x0f,0x00,0x00,0x00,
0x00,0x03,0xc0,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0x80,0x00,0xf0,0xfc,0x03,0xc3,
0xc0,0x7c,0x00,0x00,0x0f,0x0f,0x00,0x00,0x00,0x00,0x03,0xc0,0x00,0x00,0x00,0x00,
0x00,0x3c,0x00,0x00,0x00,0xf0,0xfc,0x03,0xc3,0xc0,0xfc,0x00,0x00,0x0f,0x06,0x00,
0x00,0x00,0x00,0x03,0xc0,0x00,0x00,0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0xf0,0xfe,
0x03,0xc3,0xc1,0xf0,0x00,0x00,0x0f,0x00,0x00,0x00,0x00,0x00,0x03,0xc0,0x00,0x00,
0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0xf0,0xff,0x03,0xc3,0xc3,0xe0,0x00,0x00,0x0f,
0x00,0x00,0x00,0x00,0x00,0x03,0xc0,0x00,0x00,0x00,0x00,0x00,0x3c,0x00,0x00,0x00,
0xf0,0xff,0x03,0xc3,0xc7,0xc0,0x00,0x07,0xcf,0x06,0x03,0xf8,0x06,0x3e,0x03,0xc0,
0x7f,0x01,0x80,0x70,0x00,0x3c,0x00,0x00,0x00,0xf0,0xff,0x83,0xc3,0xcf,0x80,0x00,
0x1f,0xef,0x0f,0x0f,0xfe,0x0f,0x7f,0x83,0xc1,0xff,0xc3,0xc0,0x70,0x00,0x3c,0x00,
0x00,0x00,0xf0,0xf7,0xc3,0xc3,0xdf,0x00,0x00,0x3f,0xff,0x0f,0x1f,0xff,0x0f,0xff,
0xc3,0xc3,0xff,0xc3,0xc0,0xf0,0x00,0x3f,0xff,0x00,0x00,0xf0,0xf3,0xc3,0xc3,0xff,
0x80,0x00,0x3e,0x3f,0x0f,0x1f,0x0f,0x0f,0xc7,0xc3,0xc3,0xc3,0xe1,0xc0,0xe0,0x00,
0x3f,0xff,0x80,0x00,0xf0,0xf3,0xe3,0xc3,0xff,0xc0,0x00,0x7c,0x1f,0x0f,0x1e,0x07,
0x0f,0x83,0xe3,0xc3,0x81,0xe1,0xe1,0xe0,0x00,0x3f,0xff,0x00,0x00,0xf0,0xf1,0xf3,
0xc3,0xfb,0xe0,0x00,0x78,0x1f,0x0f,0x1f,0x02,0x0f,0x01,0xe3,0xc0,0x03,0xe1,0xe1,
0xe0,0x00,0x3c,0x00,0x1f,0xf0,0xf0,0xf0,0xf3,0xc3,0xf3,0xe0,0x00,0x78,0x0f,0x0f,
0x0f,0xf8,0x0f,0x01,0xe3,0xc0,0x3f,0xe0,0xe1,0xc0,0x00,0x3c,0x00,0x1f,0xf0,0xf0,
0xf0,0x7b,0xc3,0xe1,0xf0,0x00,0x78,0x0f,0x0f,0x03,0xfe,0x0f,0x01,0xe3,0xc1,0xff,
0xe0,0xf3,0xc0,0x00,0x3c,0x00,0x1f,0xf0,0xf0,0xf0,0x7f,0xc3,0xc0,0xf8,0x00,0x78,
0x0f,0x0f,0x00,0x7f,0x8f,0x01,0xe3,0xc3,0xf9,0xe0,0x73,0xc0,0x00,0x3c,0x00,0x00,
0x00,0xf0,0xf0,0x3f,0xc3,0xc0,0xf8,0x00,0x78,0x0f,0x0f,0x0c,0x0f,0x8f,0x01,0xe3,
0xc7,0x81,0xe0,0x7b,0x80,0x00,0x3c,0x00,0x00,0x00,0xf0,0xf0,0x1f,0xc3,0xc0,0x7c,
0x00,0x7c,0x1f,0x0f,0x1e,0x07,0x8f,0x83,0xe3,0xc7,0x81,0xe0,0x7f,0x80,0x00,0x3c,
0x00,0x00,0x00,0xf0,0xf0,0x1f,0xc3,0xc0,0x3e,0x00,0x3e,0x3f,0x0f,0x1f,0x0f,0x8f,
0xc7,0xc3,0xc7,0xc7,0xe0,0x3f,0x00,0x00,0x3f,0xff,0x80,0x00,0xf0,0xf0,0x0f,0xc3,
0xc0,0x3e,0x00,0x3f,0xff,0x0f,0x1f,0xff,0x0f,0xff,0xc3,0xc7,0xff,0xf0,0x3f,0x00,
0x00,0x3f,0xff,0xc0,0x00,0xf0,0xf0,0x07,0xc3,0xc0,0x1e,0x00,0x1f,0xef,0x0f,0x0f,
0xfe,0x0f,0x7f,0x83,0xc3,0xfc,0xf0,0x1f,0x00,0x00,0x1f,0xff,0x80,0x00,0x60,0x60,
0x03,0x81,0x80,0x0e,0x00,0x07,0x86,0x06,0x03,0xf8,0x0f,0x3e,0x01,0x81,0xf0,0x60,
0x1e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x0f,0x00,0x00,0x00,0x00,0x00,0x1e,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x00,0x00,0x00,
0x00,0x00,0x3c,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x0f,0x00,0x00,0x00,0x00,0x00,0x3c,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x00,
0x00,0x00,0x00,0x03,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x00,0x00,0x00,0x00,0x03,0xf8,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x06,0x00,0x00,0x00,0x00,0x03,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x3f,0x01,0xe7,0x80,0x00,0xfc,0x00,0x7c,0x00,0x3e,0x00,0x00,
0x00,0x7e,0x00,0x1f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xc1,0xe7,
0x80,0x03,0xff,0x01,0xff,0x00,0xff,0x80,0x00,0x00,0xff,0x80,0x7f,0xe0,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0xff,0xe1,0xe7,0x80,0x07,0xff,0x83,0xff,0x81,0xff,
0xc0,0x00,0x01,0xff,0xc0,0xff,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xe1,
0xe1,0xe7,0x80,0x0f,0x87,0x87,0xc7,0xc3,0xe3,0xe0,0x00,0x03,0xe3,0xe0,0xf0,0xf0,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xc0,0xf1,0xe7,0x80,0x0f,0x03,0xc7,0x83,
0xc3,0xc1,0xe0,0x00,0x07,0xc1,0xe1,0xe0,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x03,0xc0,0xf1,0xe7,0x80,0x0f,0x03,0xc7,0x81,0xc3,0xc0,0xe0,0x00,0x07,0x80,0xe1,
0xc0,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x80,0xf1,0xe7,0x80,0x0e,0x03,
0xcf,0x01,0xe7,0x80,0xf1,0x80,0xc7,0x80,0xf3,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x03,0x00,0xf0,0xc3,0x00,0x0c,0x03,0xcf,0x01,0xe7,0x80,0xf3,0xc1,0xe7,
0x80,0xf3,0xcf,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xf0,0x00,0x00,
0x00,0x07,0xcf,0x01,0xe7,0x80,0xf1,0xe3,0xc7,0x80,0xf3,0xdf,0xe0,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0xe0,0x00,0x00,0x00,0x07,0x8f,0x01,0xe7,0x80,0xf1,
0xe3,0xc7,0xc1,0xf3,0xff,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xc0,
0x00,0x00,0x00,0x0f,0x0f,0x01,0xe7,0x80,0xf0,0xf7,0x83,0xe3,0xf3,0xf1,0xf0,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xc0,0x00,0x00,0x00,0x3f,0x0f,0x01,0xe7,
0x80,0xf0,0x7f,0x03,0xff,0xf3,0xe0,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x1f,0x00,0x00,0x00,0x00,0x7c,0x0f,0x01,0xe7,0x80,0xf0,0x7f,0x01,0xfe,0xf3,0xc0,
0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3e,0x00,0x00,0x00,0x00,0xf8,0x0f,
0x01,0xe7,0x80,0xf0,0x3e,0x00,0x7c,0xf3,0xc0,0x78,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x7c,0x00,0x00,0x00,0x01,0xf0,0x0f,0x01,0xe7,0x80,0xf0,0x7f,0x00,0x00,
0xf3,0xc0,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0x00,0x00,0x00,0x03,
0xe0,0x07,0x83,0xc3,0xc1,0xe0,0x7f,0x01,0x00,0xe1,0xc0,0x78,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0xf0,0x00,0x00,0x00,0x07,0xc0,0x07,0x83,0xc3,0xc1,0xe0,0xf7,
0x83,0x81,0xe1,0xe0,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xe0,0x00,0x00,
0x00,0x0f,0x80,0x07,0xc7,0xc3,0xe3,0xe1,0xe3,0xc3,0xc3,0xc1,0xf1,0xf0,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x03,0xff,0xf0,0x00,0x00,0x0f,0xff,0xc3,0xff,0x81,0xff,
0xc3,0xe3,0xe3,0xff,0xc0,0xff,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff,
0xf0,0x00,0x00,0x0f,0xff,0xc1,0xff,0x00,0xff,0x83,0xc1,0xe1,0xff,0x80,0x7f,0xc0,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff,0xf0,0x00,0x00,0x0f,0xff,0xc0,0x7c,
0x00,0x3e,0x03,0x80,0xe0,0x7e,0x00,0x1f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
//2013/04/03 Resolution: 200 x 96


#elif (USE_EPD_Type==USE_EPD270)

#define image_width_270 264 /**< The resolution of 2 inch is 264*176 */
#define image_height_270 176 /**< the image array size of 2.7 inch is 176*33 where 33=264/8 */
uint8_t const image_array_270_1[] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 1
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 2
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 3
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 4
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 5
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 6
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 7
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 8
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x1E, 0x7E, 0x07, 0x80, 0x01, 0xFF,  // 9
  0xFF, 0xFF, 0xFF, 0xE0, 0xFC, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x1E, 0x7E, 0x07, 0x80, 0x01, 0xFF,  // 10
  0xFF, 0xFF, 0xFF, 0xC0, 0xFC, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFF, 0x98, 0x61, 0xF9, 0x9F, 0xF9, 0xFF,  // 11
  0xFF, 0xFF, 0xFF, 0x80, 0xFC, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFF, 0x98, 0x61, 0xF9, 0x9F, 0xF9, 0xFF,  // 12
  0xFF, 0xFF, 0xFF, 0x80, 0xFC, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x81, 0x9E, 0x07, 0x9F, 0x98, 0x19, 0xFF,  // 13
  0xFF, 0xFF, 0xFF, 0x00, 0xFC, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x81, 0x9E, 0x07, 0x9F, 0x98, 0x19, 0xFF,  // 14
  0xFF, 0xFF, 0xFE, 0x00, 0xFC, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x81, 0x98, 0x67, 0x99, 0x98, 0x19, 0xFF,  // 15
  0xFF, 0xFF, 0xFC, 0x00, 0xFC, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x81, 0x98, 0x67, 0x99, 0x98, 0x19, 0xFF,  // 16
  0xFF, 0xFF, 0xF8, 0x10, 0xFC, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x81, 0x9F, 0x98, 0x01, 0x98, 0x19, 0xFF,  // 17
  0xFF, 0xFF, 0xF0, 0x30, 0xFC, 0x1F, 0xFF, 0xF8, 0xFF, 0xE3, 0xFF, 0xFF, 0xC1, 0xFF, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x81, 0x9F, 0x98, 0x01, 0x98, 0x19, 0xFF,  // 18
  0xFF, 0xFF, 0xE0, 0x70, 0xF0, 0x00, 0x3F, 0xC0, 0x1F, 0x00, 0x7F, 0xFE, 0x00, 0x3F, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFF, 0x99, 0x9F, 0xE7, 0x9F, 0xF9, 0xFF,  // 19
  0xFF, 0xFF, 0xE0, 0xF0, 0xF0, 0x00, 0x3F, 0x00, 0x0E, 0x00, 0x1F, 0xF8, 0x00, 0x0F, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFF, 0x99, 0x9F, 0xE7, 0x9F, 0xF9, 0xFF,  // 20
  0xFF, 0xFF, 0xC1, 0xF0, 0xF0, 0x00, 0x3F, 0x00, 0x04, 0x00, 0x0F, 0xF0, 0x00, 0x07, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x19, 0x99, 0x99, 0x80, 0x01, 0xFF,  // 21
  0xFF, 0xFF, 0x83, 0xF0, 0xF0, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x08, 0x03, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x19, 0x99, 0x99, 0x80, 0x01, 0xFF,  // 22
  0xFF, 0xFF, 0x03, 0xF0, 0xFC, 0x1F, 0xFC, 0x07, 0x00, 0x1C, 0x07, 0xE0, 0x7F, 0x03, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x9F, 0xFF, 0xFF, 0xFF,  // 23
  0xFF, 0xFE, 0x07, 0xF0, 0xFC, 0x1F, 0xFC, 0x1F, 0xC0, 0x7F, 0x03, 0xC0, 0xFF, 0x81, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x9F, 0xFF, 0xFF, 0xFF,  // 24
  0xFF, 0xFC, 0x0F, 0xF0, 0xFC, 0x1F, 0xF8, 0x3F, 0xE0, 0x7F, 0x83, 0xC1, 0xFF, 0xC1, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x01, 0x80, 0x19, 0xF8, 0x66, 0x67, 0xFF,  // 25
  0xFF, 0xF8, 0x1F, 0xF0, 0xFC, 0x1F, 0xF8, 0x3F, 0xE0, 0xFF, 0x83, 0x81, 0xFF, 0xC1, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x01, 0x80, 0x19, 0xF8, 0x66, 0x67, 0xFF,  // 26
  0xFF, 0xF0, 0x00, 0xF0, 0xFC, 0x1F, 0xF8, 0x3F, 0xE0, 0xFF, 0x83, 0x80, 0x00, 0x01, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xE6, 0x7E, 0x60, 0x07, 0x9F, 0xFF,  // 27
  0xFF, 0xF0, 0x00, 0xF0, 0xFC, 0x1F, 0xF8, 0x3F, 0xE0, 0xFF, 0xC3, 0x80, 0x00, 0x00, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xE6, 0x7E, 0x60, 0x07, 0x9F, 0xFF,  // 28
  0xFF, 0xE0, 0x00, 0xF0, 0xFC, 0x1F, 0xF8, 0x3F, 0xE0, 0xFF, 0xC3, 0x80, 0x00, 0x00, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE6, 0x18, 0x61, 0xE7, 0xF9, 0xE1, 0xFF,  // 29
  0xFF, 0xC0, 0x00, 0xF0, 0xFC, 0x1F, 0xF8, 0x3F, 0xE0, 0xFF, 0xC3, 0x80, 0x00, 0x00, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE6, 0x18, 0x61, 0xE7, 0xF9, 0xE1, 0xFF,  // 30
  0xFF, 0x80, 0x00, 0xF0, 0xFC, 0x1F, 0xF8, 0x3F, 0xE0, 0xFF, 0xC3, 0x80, 0x07, 0xCE, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x01, 0xF8, 0x07, 0xE6, 0x01, 0xF9, 0xFF,  // 31
  0xFF, 0x03, 0xFF, 0xF0, 0xFC, 0x1F, 0xF8, 0x3F, 0xE0, 0xFF, 0xC3, 0x83, 0xFF, 0xFF, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x01, 0xF8, 0x07, 0xE6, 0x01, 0xF9, 0xFF,  // 32
  0xFE, 0x07, 0xFF, 0xF0, 0xFC, 0x1F, 0xF8, 0x3F, 0xE0, 0xFF, 0xC3, 0xC1, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE6, 0x00, 0x67, 0xF8, 0x7E, 0x01, 0xFF,  // 33
  0xFE, 0x0F, 0xFF, 0xF0, 0xFE, 0x0F, 0xF8, 0x3F, 0xE0, 0xFF, 0xC3, 0xC1, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE6, 0x00, 0x67, 0xF8, 0x7E, 0x01, 0xFF,  // 34
  0xFC, 0x1F, 0xFF, 0xF0, 0xFE, 0x07, 0xF8, 0x3F, 0xE0, 0xFF, 0xC3, 0xC0, 0xFF, 0xC1, 0xE0, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x9E, 0x61, 0x99, 0x86, 0x19, 0xE7, 0xFF,  // 35
  0xF8, 0x3F, 0xFF, 0xF0, 0xFF, 0x00, 0x38, 0x3F, 0xE0, 0xFF, 0xC3, 0xE0, 0x7F, 0x03, 0xF0, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x9E, 0x61, 0x99, 0x86, 0x19, 0xE7, 0xFF,  // 36
  0xF0, 0x3F, 0xFF, 0xF0, 0xFF, 0x00, 0x38, 0x3F, 0xE0, 0xFF, 0xC3, 0xF0, 0x00, 0x03, 0xF8, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xE1, 0x9E, 0x7F, 0x80, 0x19, 0xE1, 0xFF,  // 37
  0xE0, 0x7F, 0xFF, 0xF0, 0xFF, 0x80, 0x38, 0x3F, 0xE0, 0xFF, 0xC3, 0xF0, 0x00, 0x07, 0xFC, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xE1, 0x9E, 0x7F, 0x80, 0x19, 0xE1, 0xFF,  // 38
  0xC0, 0xFF, 0xFF, 0xF0, 0xFF, 0xE0, 0x38, 0x3F, 0xE0, 0xFF, 0xC3, 0xFC, 0x00, 0x1F, 0xFE, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x87, 0xF8, 0x19, 0xFE, 0x1F, 0xE1, 0xFF,  // 39
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x87, 0xF8, 0x19, 0xFE, 0x1F, 0xE1, 0xFF,  // 40
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFE, 0x19, 0x99, 0xF8, 0x00, 0x19, 0xFF,  // 41
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFE, 0x19, 0x99, 0xF8, 0x00, 0x19, 0xFF,  // 42
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x1E, 0x19, 0xF9, 0x99, 0xFF,  // 43
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x1E, 0x19, 0xF9, 0x99, 0xFF,  // 44
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x19, 0x81, 0x81, 0x98, 0x61, 0xFF,  // 45
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x19, 0x81, 0x81, 0x98, 0x61, 0xFF,  // 46
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFF, 0x9E, 0x06, 0x61, 0xF8, 0x61, 0xFF,  // 47
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFF, 0x9E, 0x06, 0x61, 0xF8, 0x61, 0xFF,  // 48
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x1F, 0xFF, 0x80, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x81, 0x98, 0x61, 0xE0, 0x00, 0x01, 0xFF,  // 49
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x0F, 0xFF, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x81, 0x98, 0x61, 0xE0, 0x00, 0x01, 0xFF,  // 50
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x07, 0xFE, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x81, 0x98, 0x18, 0x07, 0x98, 0x79, 0xFF,  // 51
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x07, 0xFE, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x81, 0x98, 0x18, 0x07, 0x98, 0x79, 0xFF,  // 52
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x03, 0xFC, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x81, 0x99, 0xF8, 0x79, 0x86, 0x79, 0xFF,  // 53
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x01, 0xF8, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0x81, 0x99, 0xF8, 0x79, 0x86, 0x79, 0xFF,  // 54
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x01, 0xF8, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFF, 0x99, 0x98, 0x1F, 0x98, 0x79, 0xFF,  // 55
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0xF0, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFF, 0x99, 0x98, 0x1F, 0x98, 0x79, 0xFF,  // 56
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0xE0, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x18, 0x19, 0x99, 0xE7, 0x81, 0xFF,  // 57
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0x60, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x18, 0x19, 0x99, 0xE7, 0x81, 0xFF,  // 58
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 59
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 60
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 61
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 62
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 63
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 64
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 65
  0xE0, 0x01, 0xE0, 0x03, 0xF8, 0x00, 0xFF, 0xFF, 0xFF, 0xF0, 0x01, 0xFF, 0xFC, 0x00, 0x7F, 0x07, 0xFF, 0xF0, 0x7F, 0x80, 0x0E, 0x07, 0xE1, 0xE0, 0x01, 0xE0, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 66
  0xE0, 0x01, 0xE0, 0x00, 0xF8, 0x00, 0x3F, 0xFF, 0xFF, 0xE0, 0x00, 0xFF, 0xFC, 0x00, 0x1F, 0x07, 0xFF, 0xE0, 0x3F, 0x80, 0x0E, 0x07, 0xE1, 0xE0, 0x01, 0xE0, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 67
  0xE0, 0x01, 0xE0, 0x00, 0x78, 0x00, 0x1F, 0xFF, 0xFF, 0xE0, 0x00, 0x7F, 0xFC, 0x00, 0x0F, 0x07, 0xFF, 0xE0, 0x3F, 0x80, 0x0E, 0x03, 0xE1, 0xE0, 0x01, 0xE0, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 68
  0xE0, 0xFF, 0xE0, 0xF0, 0x38, 0x3C, 0x0F, 0xFF, 0xFF, 0xC0, 0x00, 0x7F, 0xFC, 0x1E, 0x07, 0x07, 0xFF, 0xC0, 0x1F, 0xF0, 0x7E, 0x01, 0xE1, 0xE0, 0xFF, 0xE0, 0xF0, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 69
  0xE0, 0xFF, 0xE0, 0xF8, 0x38, 0x3F, 0x07, 0xFF, 0xFF, 0x80, 0x00, 0x3F, 0xFC, 0x1F, 0x07, 0x07, 0xFF, 0xC0, 0x1F, 0xF0, 0x7E, 0x01, 0xE1, 0xE0, 0xFF, 0xE0, 0xFC, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 70
  0xE0, 0xFF, 0xE0, 0xF8, 0x38, 0x3F, 0x07, 0xFF, 0xFF, 0x80, 0x00, 0x1F, 0xFC, 0x1F, 0x07, 0x07, 0xFF, 0xC0, 0x1F, 0xF0, 0x7E, 0x00, 0xE1, 0xE0, 0xFF, 0xE0, 0xFC, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 71
  0xE0, 0xFF, 0xE0, 0xF8, 0x38, 0x3F, 0x83, 0xFF, 0xFF, 0x00, 0x00, 0x1F, 0xFC, 0x1F, 0x07, 0x07, 0xFF, 0x86, 0x0F, 0xF0, 0x7E, 0x10, 0x61, 0xE0, 0xFF, 0xE0, 0xFE, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 72
  0xE0, 0x03, 0xE0, 0xF8, 0x38, 0x3F, 0x83, 0xFF, 0xFE, 0x00, 0x00, 0x0F, 0xFC, 0x1F, 0x07, 0x07, 0xFF, 0x86, 0x0F, 0xF0, 0x7E, 0x10, 0x61, 0xE0, 0x03, 0xE0, 0xFE, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 73
  0xE0, 0x03, 0xE0, 0xF0, 0x78, 0x3F, 0x83, 0xFF, 0xFE, 0x00, 0xC0, 0x07, 0xFC, 0x1E, 0x0F, 0x07, 0xFF, 0x86, 0x0F, 0xF0, 0x7E, 0x18, 0x21, 0xE0, 0x03, 0xE0, 0xFE, 0x0F, 0xFC, 0x0F, 0x01, 0xFC, 0x3F,  // 74
  0xE0, 0x03, 0xE0, 0x00, 0x78, 0x3F, 0x83, 0xFF, 0xFC, 0x01, 0xE0, 0x07, 0xFC, 0x00, 0x0F, 0x07, 0xFF, 0x0F, 0x07, 0xF0, 0x7E, 0x18, 0x21, 0xE0, 0x03, 0xE0, 0xFE, 0x0F, 0xFC, 0xE7, 0x3C, 0xF3, 0xCF,  // 75
  0xE0, 0xFF, 0xE0, 0x00, 0xF8, 0x3F, 0x83, 0xFF, 0xF8, 0x01, 0xE0, 0x03, 0xFC, 0x00, 0x1F, 0x07, 0xFF, 0x0F, 0x07, 0xF0, 0x7E, 0x1C, 0x01, 0xE0, 0xFF, 0xE0, 0xFE, 0x0F, 0xFC, 0xF3, 0x3E, 0x73, 0xCF,  // 76
  0xE0, 0xFF, 0xE0, 0x03, 0xF8, 0x3F, 0x83, 0xFF, 0xF8, 0x03, 0xF0, 0x01, 0xFC, 0x00, 0x7F, 0x07, 0xFE, 0x00, 0x03, 0xF0, 0x7E, 0x1E, 0x01, 0xE0, 0xFF, 0xE0, 0xFE, 0x0F, 0xFC, 0xF3, 0x3E, 0x67, 0xE7,  // 77
  0xE0, 0xFF, 0xE0, 0xFF, 0xF8, 0x3F, 0x07, 0xFF, 0xF0, 0x07, 0xF8, 0x01, 0xFC, 0x1F, 0xFF, 0x07, 0xFE, 0x00, 0x03, 0xF0, 0x7E, 0x1E, 0x01, 0xE0, 0xFF, 0xE0, 0xFC, 0x1F, 0xFC, 0xF3, 0x3C, 0xE7, 0xE7,  // 78
  0xE0, 0xFF, 0xE0, 0xFF, 0xF8, 0x3F, 0x07, 0xFF, 0xE0, 0x07, 0xF8, 0x00, 0xFC, 0x1F, 0xFF, 0x07, 0xFE, 0x00, 0x03, 0xF0, 0x7E, 0x1F, 0x01, 0xE0, 0xFF, 0xE0, 0xFC, 0x1F, 0xFC, 0xE7, 0x01, 0xE7, 0xE7,  // 79
  0xE0, 0xFF, 0xE0, 0xFF, 0xF8, 0x3C, 0x0F, 0xFF, 0xC0, 0x0F, 0xFC, 0x00, 0x7C, 0x1F, 0xFF, 0x07, 0xFC, 0x1F, 0x81, 0xF0, 0x7E, 0x1F, 0x81, 0xE0, 0xFF, 0xE0, 0xF0, 0x3F, 0xFC, 0x0F, 0x39, 0xE7, 0xE7,  // 80
  0xE0, 0x01, 0xE0, 0xFF, 0xF8, 0x00, 0x1F, 0xFF, 0xC0, 0x1F, 0xFC, 0x00, 0x7C, 0x1F, 0xFF, 0x00, 0x04, 0x3F, 0xC1, 0x80, 0x0E, 0x1F, 0x81, 0xE0, 0x01, 0xE0, 0x00, 0x7F, 0xFC, 0xFF, 0x3C, 0xF3, 0xCF,  // 81
  0xE0, 0x01, 0xE0, 0xFF, 0xF8, 0x00, 0x3F, 0xFF, 0x80, 0x1F, 0xFE, 0x00, 0x3C, 0x1F, 0xFF, 0x00, 0x04, 0x3F, 0xC1, 0x80, 0x0E, 0x1F, 0xC1, 0xE0, 0x01, 0xE0, 0x00, 0xFF, 0xFC, 0xFF, 0x3E, 0x73, 0xCF,  // 82
  0xE0, 0x01, 0xE0, 0xFF, 0xF8, 0x00, 0xFF, 0xFF, 0x00, 0x3F, 0xFF, 0x00, 0x1C, 0x1F, 0xFF, 0x00, 0x00, 0x7F, 0xC0, 0x80, 0x0E, 0x1F, 0xC1, 0xE0, 0x01, 0xE0, 0x03, 0xFF, 0xFC, 0xFF, 0x3F, 0x3C, 0x3F,  // 83
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 84
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 85
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 86
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 87
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 88
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 89
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 90
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 91
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 92
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 93
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 94
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 95
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 96
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 97
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 98
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 99
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 100
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 101
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 102
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 103
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 104
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 105
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 106
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 107
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 108
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 109
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 110
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 111
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 112
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 113
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 114
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 115
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 116
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 117
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 118
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 119
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 120
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 121
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 122
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 123
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 124
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 125
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 126
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 127
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 128
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 129
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 130
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 131
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 132
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 133
  0xFF, 0xFF, 0xFF, 0xE5, 0xBD, 0xCB, 0x88, 0x94, 0xEE, 0x4C, 0xC7, 0x4A, 0xC3, 0x35, 0xED, 0x3D, 0xE5, 0x9E, 0xB4, 0xDE, 0xF6, 0x49, 0x98, 0x89, 0x6C, 0x2E, 0x15, 0x0B, 0x9C, 0x53, 0xFF, 0xFF, 0xFF,  // 134
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 135
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 136
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 137
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 138
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 139
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 140
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 141
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 142
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 143
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 144
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 145
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 146
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 147
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 148
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 149
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 150
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 151
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 152
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x7F, 0xFF, 0xFF, 0xFF, 0xEF, 0xFF, 0xFF, 0x8F,  // 153
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x1C, 0x03, 0x80, 0x38, 0x7F, 0x3C, 0xFF, 0x80, 0xE3, 0x0F, 0xF2, 0x00, 0xF8, 0x00, 0xF8, 0xF1, 0x1C, 0x01, 0xC7, 0xFF, 0xCF, 0x1F, 0xCE, 0x63,  // 154
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0x0C, 0x03, 0x8E, 0x18, 0x7F, 0x3C, 0x7F, 0x1D, 0xE3, 0x0F, 0xE6, 0x00, 0xF9, 0xFE, 0x78, 0xF7, 0xDC, 0x78, 0xC7, 0xFF, 0xCF, 0x8F, 0xCC, 0xFB,  // 155
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0x0C, 0x7F, 0x8F, 0x18, 0x7E, 0x38, 0x7E, 0x3D, 0xE3, 0x87, 0xE6, 0x1F, 0xF9, 0xFF, 0x38, 0xE7, 0xFC, 0x7C, 0xC7, 0xFF, 0xC7, 0xCF, 0xDC, 0xFF,  // 156
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0x8C, 0x7F, 0x8F, 0x1C, 0x7E, 0x78, 0x7E, 0x3F, 0xE3, 0x87, 0xE6, 0x1F, 0xF9, 0xFF, 0x18, 0xE7, 0xFC, 0x7C, 0xC7, 0xFF, 0xA7, 0xC7, 0xBC, 0xFF,  // 157
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0x8C, 0x7F, 0x8F, 0x1C, 0x3E, 0x70, 0x3E, 0x1F, 0xE3, 0xC3, 0xCE, 0x1F, 0xF9, 0xFF, 0x98, 0xE7, 0xFC, 0x7C, 0xC7, 0xFF, 0xB7, 0xE7, 0xBC, 0xFF,  // 158
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0x0C, 0x7F, 0x8E, 0x1E, 0x3C, 0xF2, 0x3E, 0x07, 0xE3, 0xC3, 0xCE, 0x1F, 0xF9, 0xFF, 0x98, 0xF1, 0xFC, 0x78, 0xC7, 0xFF, 0x73, 0xF3, 0x7C, 0x3F,  // 159
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC6, 0x0C, 0x03, 0x8C, 0x3E, 0x1C, 0xE6, 0x1F, 0x03, 0xE3, 0xC3, 0x9E, 0x00, 0xF9, 0xFF, 0x98, 0xF0, 0x7C, 0x71, 0xC7, 0xFF, 0x73, 0xF0, 0x7E, 0x0F,  // 160
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x3C, 0x03, 0x80, 0xFF, 0x19, 0xE6, 0x1F, 0x80, 0xE3, 0xE1, 0x9E, 0x00, 0xF9, 0xFF, 0x98, 0xFC, 0x1C, 0x03, 0xC7, 0xFE, 0x79, 0xF8, 0xFF, 0x83,  // 161
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xFC, 0x7F, 0x80, 0x7F, 0x09, 0xE7, 0x1F, 0xC0, 0xE3, 0xE1, 0x9E, 0x1F, 0xF9, 0xFF, 0x98, 0xFF, 0x1C, 0x7F, 0xC7, 0xFE, 0x01, 0xF8, 0xFF, 0xE1,  // 162
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xFC, 0x7F, 0x88, 0x7F, 0x09, 0xC0, 0x0F, 0xF0, 0xE3, 0xF0, 0x3E, 0x1F, 0xF9, 0xFF, 0x98, 0xFF, 0x8C, 0x7F, 0xC7, 0xFC, 0xF8, 0xF8, 0xFF, 0xF1,  // 163
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xFC, 0x7F, 0x8C, 0x3F, 0x83, 0xCF, 0x8F, 0xF8, 0xE3, 0xF0, 0x3E, 0x1F, 0xF9, 0xFF, 0x18, 0xFF, 0xCC, 0x7F, 0xC7, 0xFD, 0xFC, 0xF8, 0xFF, 0xF9,  // 164
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xFC, 0x7F, 0x8E, 0x1F, 0x83, 0x9F, 0x86, 0xF8, 0xE3, 0xF8, 0x7E, 0x1F, 0xF9, 0xFF, 0x38, 0xFF, 0xCC, 0x7F, 0xC7, 0xF9, 0xFC, 0x78, 0xFF, 0xF9,  // 165
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xFC, 0x7F, 0x8E, 0x0F, 0x87, 0x9F, 0x86, 0x78, 0xE3, 0xF8, 0x7E, 0x1F, 0xF9, 0xFE, 0x78, 0xEF, 0x9C, 0x7F, 0xC7, 0xFB, 0xFE, 0x78, 0xFD, 0xF3,  // 166
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xFC, 0x03, 0x8F, 0x0F, 0xC7, 0x1F, 0xC2, 0x11, 0xE3, 0xF8, 0x7E, 0x00, 0xF9, 0xF8, 0xF8, 0xE7, 0x1C, 0x7F, 0xC0, 0x33, 0xFE, 0x38, 0xF8, 0xE3,  // 167
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xFC, 0x03, 0x8F, 0x07, 0xC7, 0x3F, 0xC2, 0x03, 0xE3, 0xFC, 0xFE, 0x00, 0xF8, 0x03, 0xF8, 0xF0, 0x7C, 0x7F, 0xC0, 0x33, 0xFE, 0x38, 0xFC, 0x0F,  // 168
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 169
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 170
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 171
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 172
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 173
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 174
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 175
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF   // 176
};
//2013/04/03 Resolution: 264 x 176

uint8_t const image_array_270_2[] = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 1
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 2
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 3
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 4
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 5
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 6
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 7
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 8
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 9
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 10
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 11
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 12
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 13
  0xFF, 0xFF, 0xF1, 0xFF, 0xFF, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0xFE, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x18, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 14
  0xFF, 0xFF, 0x80, 0x3F, 0xFF, 0x87, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0x87, 0xFC, 0x3F, 0xFF, 0xFF, 0xFF, 0xE0, 0x7E, 0x18, 0x7F, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 15
  0xFF, 0xFF, 0x00, 0x3F, 0xFF, 0x87, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0x87, 0xFC, 0x3F, 0xFF, 0xC3, 0xFF, 0xE0, 0x7E, 0x18, 0x7F, 0xF0, 0xFF, 0xFF, 0xFF, 0xC0, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 16
  0xFF, 0xFE, 0x00, 0x3F, 0xFF, 0x87, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0x87, 0xFE, 0x7F, 0xFF, 0xC3, 0xFF, 0xC0, 0x7E, 0x18, 0x7F, 0xF0, 0xFF, 0xFF, 0xFF, 0xC0, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 17
  0xFF, 0xFC, 0x1F, 0xFF, 0xFF, 0x87, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0x87, 0xFF, 0xFF, 0xFF, 0xC3, 0xFF, 0xC0, 0x3E, 0x18, 0x7F, 0xF0, 0xFF, 0xFF, 0xFF, 0x87, 0xC3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 18
  0xFF, 0xF8, 0x3F, 0xFE, 0x0F, 0x87, 0xE0, 0xFC, 0x67, 0xF0, 0xE3, 0x0F, 0x87, 0x1E, 0x3F, 0x0F, 0xC0, 0xFF, 0xC4, 0x3E, 0x18, 0x7F, 0xF0, 0xE3, 0x0F, 0xFF, 0x07, 0xC1, 0xC6, 0x1F, 0xE1, 0xFF, 0xFF,  // 19
  0xFF, 0xF8, 0x7F, 0xF8, 0x03, 0x87, 0x80, 0x3C, 0x47, 0xF0, 0xE0, 0x07, 0x86, 0x1C, 0x3C, 0x03, 0x00, 0xFF, 0xC4, 0x3E, 0x18, 0x7F, 0xF0, 0xE0, 0x07, 0xFF, 0x0F, 0xE1, 0xC0, 0x0F, 0x80, 0x7F, 0xFF,  // 20
  0xFF, 0xF8, 0x7F, 0xF0, 0x03, 0x87, 0x00, 0x3C, 0x07, 0xF0, 0xE0, 0x03, 0x84, 0x1C, 0x38, 0x01, 0x00, 0xFF, 0x86, 0x3E, 0x18, 0x7F, 0xF0, 0xE0, 0x03, 0xFF, 0x0F, 0xE1, 0xC0, 0x07, 0x00, 0x3F, 0xFF,  // 21
  0xFF, 0xF8, 0x7F, 0xF0, 0xE1, 0x87, 0x0E, 0x1C, 0x07, 0xF0, 0xE0, 0x83, 0x84, 0x3C, 0x38, 0xE1, 0xC3, 0xFF, 0x86, 0x1E, 0x18, 0x7F, 0xF0, 0xE0, 0x83, 0xFF, 0x0F, 0xE1, 0xC1, 0x07, 0x1C, 0x3F, 0xFF,  // 22
  0xFF, 0xF8, 0x7F, 0xE1, 0xF0, 0x86, 0x1F, 0x0C, 0x1F, 0xF0, 0xE1, 0xC3, 0x80, 0x7C, 0x30, 0xF1, 0xC3, 0xFF, 0x8E, 0x1E, 0x18, 0x60, 0x70, 0xE1, 0xC3, 0x03, 0x0F, 0xE1, 0xC3, 0x86, 0x1E, 0x3F, 0xFF,  // 23
  0xFF, 0xF8, 0x7F, 0xE1, 0xF0, 0x86, 0x1F, 0x0C, 0x3F, 0xF0, 0xE1, 0xC3, 0x80, 0x7C, 0x30, 0x00, 0xC3, 0xFF, 0x00, 0x1E, 0x18, 0x60, 0x30, 0xE1, 0xC3, 0x01, 0x0F, 0xE1, 0xC3, 0x86, 0x00, 0x1F, 0xFF,  // 24
  0xFF, 0xF8, 0x7F, 0xE1, 0xF0, 0x86, 0x1F, 0x0C, 0x3F, 0xF0, 0xE1, 0xC3, 0x80, 0x7C, 0x30, 0x00, 0xC3, 0xFF, 0x00, 0x0E, 0x18, 0x60, 0x30, 0xE1, 0xC3, 0x01, 0x0F, 0xE1, 0xC3, 0x86, 0x00, 0x1F, 0xFF,  // 25
  0xFF, 0xF8, 0x3F, 0xE1, 0xF0, 0x86, 0x1F, 0x0C, 0x3F, 0xF0, 0xE1, 0xC3, 0x80, 0x3C, 0x30, 0xFF, 0xC3, 0xFF, 0x00, 0x0E, 0x18, 0x7F, 0xF0, 0xE1, 0xC3, 0xFF, 0x07, 0xC1, 0xC3, 0x86, 0x1F, 0xFF, 0xFF,  // 26
  0xFF, 0xFC, 0x1F, 0xF0, 0xE1, 0x87, 0x0E, 0x1C, 0x3F, 0xF0, 0xE1, 0xC3, 0x84, 0x1C, 0x30, 0xFF, 0xC3, 0xFE, 0x1F, 0x0E, 0x18, 0x7F, 0xF0, 0xE1, 0xC3, 0xFF, 0x83, 0x83, 0xC3, 0x86, 0x1F, 0xFF, 0xFF,  // 27
  0xFF, 0xFE, 0x00, 0x70, 0x01, 0x87, 0x00, 0x1C, 0x3F, 0xF0, 0xE1, 0xC3, 0x86, 0x1C, 0x38, 0x01, 0xC0, 0xFE, 0x1F, 0x06, 0x18, 0x7F, 0xF0, 0xE1, 0xC3, 0xFF, 0xC0, 0x03, 0xC3, 0x87, 0x00, 0x3F, 0xFF,  // 28
  0xFF, 0xFF, 0x00, 0x78, 0x03, 0x87, 0x80, 0x3C, 0x3F, 0xF0, 0xE1, 0xC3, 0x86, 0x0C, 0x3C, 0x01, 0xC0, 0xFE, 0x1F, 0x86, 0x18, 0x7F, 0xF0, 0xE1, 0xC3, 0xFF, 0xC0, 0x07, 0xC3, 0x87, 0x80, 0x3F, 0xFF,  // 29
  0xFF, 0xFF, 0x80, 0x7C, 0x07, 0x87, 0xC0, 0x7C, 0x3F, 0xF0, 0xE1, 0xC3, 0x87, 0x0C, 0x3E, 0x01, 0xE0, 0xFC, 0x1F, 0x86, 0x18, 0x7F, 0xF0, 0xE1, 0xC3, 0xFF, 0xF0, 0x1F, 0xC3, 0x87, 0xC0, 0x3F, 0xFF,  // 30
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 31
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 32
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 33
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 34
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 35
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 36
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 37
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 38
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 39
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 40
  0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 41
  0xFF, 0xFF, 0xF8, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0x0E, 0x3F, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 42
  0xFF, 0xFC, 0x00, 0xCF, 0x0E, 0x3F, 0xFF, 0xF0, 0x3F, 0x08, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 43
  0xFF, 0x00, 0x3F, 0xE0, 0x00, 0x0F, 0xFF, 0x03, 0xFF, 0xC0, 0x03, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 44
  0xE0, 0x1F, 0xFF, 0xF3, 0xC3, 0xE7, 0xF0, 0x1F, 0xFF, 0xF0, 0x08, 0xC7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 45
  0xE7, 0xFF, 0xFF, 0xFC, 0xF0, 0xE0, 0x01, 0xFF, 0xFF, 0xFC, 0x7E, 0x31, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 46
  0xE7, 0xFF, 0xFF, 0xFE, 0x38, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0x3F, 0x98, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 47
  0xE3, 0xFF, 0xFF, 0xFF, 0x9E, 0x19, 0xCF, 0xFF, 0xFF, 0xFF, 0x8F, 0xE6, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 48
  0xE1, 0xFF, 0xFF, 0xFF, 0xCF, 0x84, 0xE3, 0xFF, 0xFF, 0xFF, 0xE3, 0xF9, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 49
  0xE4, 0x7F, 0xFF, 0xFF, 0xE3, 0xC0, 0x79, 0xFF, 0xFF, 0xFF, 0xF8, 0xFE, 0x63, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 50
  0xE7, 0x3F, 0xFF, 0xFF, 0xF9, 0xF0, 0x3C, 0x7F, 0xFF, 0xFF, 0xFE, 0x7F, 0x98, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 51
  0xE7, 0x9F, 0xFF, 0xFF, 0xFC, 0x78, 0x0F, 0x3F, 0xFF, 0xFF, 0xFF, 0x1F, 0xE6, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 52
  0xE7, 0xCF, 0xFF, 0xFF, 0xFF, 0x39, 0xA7, 0x8F, 0xFF, 0xFF, 0xFF, 0xCF, 0xF1, 0x8F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 53
  0xE7, 0xE7, 0xFF, 0xFF, 0xFF, 0x83, 0xD1, 0xE7, 0xFF, 0xFF, 0xFF, 0xEF, 0xFC, 0x63, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 54
  0xE7, 0xF3, 0xFF, 0xFF, 0xFF, 0xE7, 0xEC, 0xF1, 0xFF, 0xFF, 0xFF, 0xE7, 0xFF, 0x18, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 55
  0xE7, 0xF9, 0xFF, 0xFF, 0xFF, 0xF3, 0xF6, 0x3C, 0xFF, 0xFF, 0xFF, 0xE7, 0xFF, 0xC4, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 56
  0xE7, 0xFC, 0xFF, 0xFF, 0xFF, 0xF9, 0xFB, 0x9E, 0x3F, 0xFF, 0xFF, 0xC7, 0xFF, 0xF7, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 57
  0xE7, 0xFE, 0x7F, 0xFF, 0xFF, 0xFD, 0xF9, 0xCF, 0x9F, 0xFF, 0xFF, 0x8F, 0xFF, 0xE7, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 58
  0xF7, 0xFF, 0x3F, 0xFF, 0xFF, 0xFE, 0xFC, 0xF0, 0x07, 0xFF, 0xFE, 0x0F, 0xFF, 0xCE, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 59
  0xF7, 0xFF, 0x9F, 0xFF, 0xFF, 0xFE, 0x7E, 0x80, 0x01, 0xFF, 0xC0, 0x3F, 0xFC, 0x10, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 60
  0xF3, 0xFF, 0xCF, 0xFF, 0xFF, 0xFF, 0x7E, 0x07, 0xE4, 0xF8, 0x00, 0xFF, 0x80, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 61
  0xF1, 0xFF, 0xE7, 0xFF, 0xFF, 0xFF, 0x3F, 0x3F, 0xE6, 0x00, 0x0F, 0xE0, 0x00, 0x06, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 62
  0xF0, 0xFF, 0xF7, 0xFF, 0xFF, 0xFF, 0xBF, 0xBF, 0xE0, 0x00, 0x7F, 0x00, 0x00, 0x38, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 63
  0xF2, 0x7F, 0xF3, 0xFF, 0xFF, 0xFF, 0x9F, 0x9E, 0x03, 0x07, 0xE0, 0x00, 0x00, 0x02, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x1F, 0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xF0, 0x7F,  // 64
  0xF3, 0x3F, 0xF9, 0xFF, 0xFF, 0xFF, 0xDF, 0xC0, 0x60, 0x7C, 0x00, 0x00, 0x0F, 0xDE, 0x7F, 0xFF, 0xFF, 0xF0, 0x1F, 0xFC, 0x00, 0x00, 0x3F, 0xE0, 0x03, 0xFF, 0xFF, 0xFF, 0x00, 0x1F, 0xFF, 0x80, 0x1F,  // 65
  0xF3, 0x9F, 0xFD, 0xFF, 0xF9, 0xFF, 0xDF, 0xE6, 0x0F, 0x80, 0x00, 0x00, 0xCF, 0x4E, 0x7F, 0xFF, 0xFF, 0xC0, 0x1F, 0xF8, 0x00, 0x00, 0x3F, 0x80, 0x01, 0xFF, 0xFF, 0xFC, 0x00, 0x0F, 0xFF, 0x00, 0x0F,  // 66
  0xF3, 0xCF, 0xFC, 0xFF, 0xE1, 0xFF, 0xCF, 0xF0, 0x70, 0x01, 0x00, 0x1E, 0x07, 0xEE, 0x7F, 0xFF, 0xFF, 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x3F, 0x00, 0x00, 0xFF, 0xFF, 0xF8, 0x00, 0x07, 0xFC, 0x00, 0x07,  // 67
  0xF3, 0xE7, 0xFE, 0xFF, 0xED, 0xFF, 0xCF, 0xFC, 0x00, 0xD8, 0x07, 0xC8, 0x0E, 0x06, 0x7F, 0xFF, 0xFE, 0x00, 0x3F, 0xF8, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7F, 0xFF, 0xF0, 0x00, 0x03, 0xF8, 0x00, 0x07,  // 68
  0xF1, 0xF3, 0xFE, 0xFF, 0xE1, 0xFF, 0xCF, 0xFF, 0x0F, 0x00, 0x18, 0x00, 0xF0, 0x86, 0x7F, 0xFF, 0xFC, 0x00, 0x3F, 0xF8, 0x00, 0x00, 0x7C, 0x07, 0xE0, 0x7F, 0xFF, 0xE0, 0x3F, 0x03, 0xF8, 0x1E, 0x03,  // 69
  0xF0, 0xF9, 0xFE, 0x7F, 0xC7, 0xFF, 0xCF, 0xFF, 0xE0, 0x00, 0x00, 0x06, 0x00, 0x82, 0xFF, 0xFF, 0xFE, 0x00, 0x3F, 0xFF, 0xFF, 0xC0, 0xFC, 0x0F, 0xE0, 0x7F, 0xFF, 0xE0, 0x7F, 0x03, 0xF0, 0x3F, 0x03,  // 70
  0xFA, 0xFC, 0xFF, 0x7F, 0xFF, 0xFF, 0xEF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x06, 0xC0, 0xFF, 0xFF, 0xFE, 0x30, 0x3F, 0xFF, 0xFF, 0xC1, 0xF8, 0x1F, 0xF0, 0x3F, 0xFF, 0xC0, 0xFF, 0x81, 0xE0, 0x7F, 0x83,  // 71
  0xFA, 0x7F, 0x7F, 0x7F, 0xFF, 0xFF, 0xEF, 0x80, 0x00, 0x07, 0xA8, 0x02, 0x78, 0x40, 0xFF, 0xFF, 0xFE, 0xF0, 0x3F, 0xFF, 0xFF, 0x81, 0xF8, 0x1F, 0xF0, 0x3F, 0xFF, 0xC0, 0xFF, 0x81, 0xE0, 0x7F, 0x81,  // 72
  0xFB, 0x3F, 0xBF, 0x3F, 0xFF, 0xFF, 0xE0, 0x01, 0xE0, 0xBF, 0xC0, 0x01, 0xC0, 0x04, 0xFF, 0xFF, 0xFF, 0xF0, 0x7F, 0xFF, 0xFF, 0x03, 0xF8, 0x1F, 0xF0, 0x3F, 0xFF, 0xC0, 0xFF, 0x81, 0xC0, 0xFF, 0x81,  // 73
  0xFB, 0x9F, 0xDF, 0x3F, 0xFF, 0xFF, 0x80, 0x1F, 0xE3, 0xFF, 0x94, 0x01, 0x00, 0xB8, 0xFF, 0xCF, 0xFF, 0xF0, 0x7F, 0xFF, 0xFF, 0x03, 0xF0, 0x3F, 0xF0, 0x3F, 0xFF, 0x81, 0xFF, 0x81, 0xC0, 0xFF, 0x81,  // 74
  0xFB, 0xCF, 0xEF, 0x3F, 0xFF, 0xF0, 0x01, 0xF0, 0xF3, 0xF0, 0x30, 0x00, 0x07, 0x80, 0xFF, 0x9F, 0xFF, 0xF0, 0x7F, 0xFF, 0xFE, 0x07, 0xF0, 0x3F, 0xF0, 0x3F, 0xFF, 0x81, 0xFF, 0x81, 0xC0, 0xFF, 0x81,  // 75
  0xF9, 0xE7, 0xF7, 0x3F, 0xFF, 0xE4, 0x7E, 0x10, 0x71, 0x02, 0x7A, 0x00, 0xF8, 0x00, 0xFF, 0x8F, 0xFF, 0xE0, 0x7F, 0xFF, 0xFC, 0x0F, 0xF0, 0x3F, 0xF0, 0x3F, 0xFF, 0x81, 0xFF, 0x81, 0x81, 0xFF, 0x81,  // 76
  0xF8, 0xF3, 0xFB, 0xBF, 0xFF, 0x0C, 0xC2, 0x1F, 0x38, 0x70, 0x22, 0x07, 0x80, 0x3D, 0xFE, 0x03, 0xFF, 0xE0, 0x7F, 0xFF, 0xFC, 0x0F, 0xF0, 0x3F, 0xE0, 0x3F, 0xFF, 0x81, 0xFF, 0x01, 0x81, 0xFF, 0x83,  // 77
  0xF8, 0x79, 0xFD, 0xBF, 0xF0, 0x2C, 0xC3, 0xBC, 0x3B, 0xE7, 0xA0, 0x7C, 0x07, 0x7D, 0xF8, 0x03, 0xFF, 0xE0, 0x7F, 0xFF, 0xF8, 0x1F, 0xF0, 0x1F, 0xE0, 0x3F, 0xFF, 0x80, 0xFF, 0x01, 0x81, 0xFF, 0x83,  // 78
  0xF9, 0x3C, 0xFE, 0xBF, 0x03, 0xEC, 0x63, 0xF8, 0x39, 0xF2, 0x21, 0xC0, 0x3F, 0x79, 0xF8, 0xFB, 0xFF, 0xE0, 0xFF, 0xFF, 0xF0, 0x3F, 0xF8, 0x1F, 0xC0, 0x7F, 0xFF, 0xC0, 0xFE, 0x03, 0x03, 0xFF, 0x03,  // 79
  0xF9, 0x9E, 0x7F, 0x30, 0x3F, 0xED, 0x7F, 0xF8, 0xFC, 0x30, 0xF0, 0x03, 0x7F, 0x79, 0xF1, 0xFF, 0xFF, 0xE0, 0xFF, 0xFF, 0xF0, 0x3F, 0xF8, 0x06, 0x00, 0x7F, 0xFF, 0xC0, 0x30, 0x03, 0x03, 0xFF, 0x03,  // 80
  0xF9, 0xCF, 0x3F, 0x87, 0xFF, 0xED, 0x7F, 0xFF, 0xC0, 0x03, 0x00, 0x1F, 0x7F, 0x79, 0xF1, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xE0, 0x7F, 0xF8, 0x00, 0x00, 0x7F, 0xFF, 0xC0, 0x00, 0x03, 0x03, 0xFF, 0x03,  // 81
  0xF9, 0xE7, 0x9F, 0xBF, 0xFF, 0xED, 0x3F, 0xFE, 0x02, 0x30, 0x00, 0x3F, 0x7E, 0x79, 0xF1, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xC0, 0xFF, 0xFC, 0x00, 0x00, 0xFF, 0xFF, 0xE0, 0x00, 0x07, 0x03, 0xFF, 0x03,  // 82
  0xF9, 0xF3, 0xCF, 0xBF, 0xFF, 0xED, 0xBF, 0xC2, 0x06, 0x00, 0x00, 0x07, 0x7E, 0x79, 0xF0, 0xFF, 0xFF, 0xC0, 0xFF, 0xFF, 0xC0, 0xFF, 0xFE, 0x00, 0x40, 0xFF, 0xFF, 0xF0, 0x02, 0x07, 0x03, 0xFE, 0x07,  // 83
  0xF9, 0xF9, 0xE7, 0xBF, 0xFF, 0xED, 0x98, 0x52, 0x3E, 0x00, 0x00, 0x00, 0x7E, 0x79, 0xF8, 0x7F, 0xFF, 0xC1, 0xFF, 0xFF, 0x81, 0xFF, 0xFF, 0x80, 0xC0, 0xFF, 0xFF, 0xFC, 0x06, 0x07, 0x03, 0xFE, 0x07,  // 84
  0xF9, 0xFC, 0xF3, 0x9F, 0xFF, 0xED, 0xDB, 0x47, 0xE0, 0x00, 0x00, 0x18, 0x7E, 0x73, 0xFC, 0x1F, 0xFF, 0xC1, 0xFF, 0xFF, 0x01, 0xFF, 0xFF, 0xFF, 0x81, 0xFF, 0xFF, 0xFF, 0xFC, 0x0F, 0x03, 0xFE, 0x07,  // 85
  0xF9, 0xFE, 0xF9, 0xDF, 0xFF, 0xED, 0xC8, 0x7E, 0x00, 0x00, 0x00, 0x0F, 0x7E, 0x73, 0xFE, 0x0F, 0xFF, 0x81, 0xFF, 0xFF, 0x03, 0xFF, 0xFF, 0xFF, 0x01, 0xFF, 0xFF, 0xFF, 0xF8, 0x0F, 0x03, 0xFC, 0x0F,  // 86
  0xFD, 0xFE, 0x7C, 0xDF, 0xFF, 0xED, 0xE3, 0xE0, 0x00, 0x00, 0x00, 0x03, 0x7E, 0xF3, 0xFF, 0x8F, 0xFF, 0x81, 0xFF, 0xFE, 0x07, 0xFF, 0xFF, 0xFE, 0x03, 0xFF, 0xFF, 0xFF, 0xF0, 0x1F, 0x03, 0xFC, 0x0F,  // 87
  0xFD, 0xFF, 0x3E, 0x5F, 0xFF, 0xE0, 0x78, 0x00, 0x00, 0x00, 0x3E, 0x01, 0x7C, 0xF3, 0xFF, 0xC7, 0xFF, 0x81, 0xFF, 0xFC, 0x07, 0xFF, 0xFF, 0xFC, 0x07, 0xFF, 0xFF, 0xFF, 0xE0, 0x3F, 0x03, 0xFC, 0x0F,  // 88
  0xFC, 0xFF, 0x9F, 0x18, 0x3F, 0x87, 0x0C, 0x00, 0x00, 0x03, 0xFF, 0x80, 0x79, 0xC3, 0xFF, 0xC7, 0xFF, 0x83, 0xFF, 0xFC, 0x0F, 0xFF, 0xFF, 0xF8, 0x07, 0xF8, 0x7F, 0xFF, 0xC0, 0x3F, 0x03, 0xF8, 0x1F,  // 89
  0xFC, 0xFF, 0xCF, 0x81, 0x9C, 0x0F, 0xE0, 0x00, 0x00, 0x3F, 0xFF, 0xE0, 0x63, 0x13, 0xFF, 0xC7, 0xFF, 0x83, 0xFF, 0xF8, 0x1F, 0xFF, 0xFF, 0xE0, 0x0F, 0xF0, 0x3F, 0xFF, 0x00, 0x7F, 0x01, 0xF0, 0x3F,  // 90
  0xFE, 0xFF, 0xE7, 0xDF, 0xC1, 0xEF, 0xE0, 0x00, 0x03, 0xFF, 0xF8, 0x30, 0x00, 0x73, 0xFF, 0xC7, 0xFF, 0x03, 0xFF, 0xF0, 0x1F, 0xFF, 0xFF, 0x00, 0x1F, 0xE0, 0x3F, 0xF8, 0x00, 0xFF, 0x81, 0xE0, 0x3F,  // 91
  0xFE, 0x7F, 0xF3, 0xDF, 0xDF, 0xEF, 0xE0, 0x00, 0x3F, 0xFF, 0xC0, 0x3C, 0x01, 0xF3, 0xDF, 0x8F, 0xFF, 0x03, 0xFF, 0xF0, 0x3F, 0xFF, 0xF0, 0x00, 0x3F, 0xE0, 0x3F, 0x80, 0x01, 0xFF, 0x80, 0x00, 0x7F,  // 92
  0xFF, 0x3F, 0xF9, 0xDF, 0xDF, 0xEF, 0xE0, 0x03, 0xFF, 0xFC, 0x01, 0xFF, 0x1C, 0x67, 0xC0, 0x0F, 0xFF, 0x03, 0xFF, 0xE0, 0x7F, 0xFF, 0xF0, 0x00, 0xFF, 0xE0, 0x3F, 0x80, 0x07, 0xFF, 0xC0, 0x00, 0xFF,  // 93
  0xFF, 0x3F, 0xFD, 0xDF, 0xDF, 0xEF, 0xE0, 0x1F, 0xFF, 0xF8, 0x1F, 0xE7, 0xC7, 0x27, 0xC0, 0x3F, 0xFF, 0x03, 0xFF, 0xC0, 0x7F, 0xFF, 0xF0, 0x01, 0xFF, 0xE0, 0x3F, 0x80, 0x0F, 0xFF, 0xE0, 0x01, 0xFF,  // 94
  0xFF, 0x9F, 0xFE, 0xDF, 0xDF, 0xEF, 0xE1, 0xFF, 0xFE, 0x1E, 0xFF, 0xFD, 0xE1, 0x87, 0xF8, 0xFF, 0xFF, 0x07, 0xFF, 0xC0, 0xFF, 0xFF, 0xF0, 0x0F, 0xFF, 0xF0, 0x7F, 0x80, 0x7F, 0xFF, 0xF0, 0x07, 0xFF,  // 95
  0xFF, 0x8F, 0xFE, 0xDF, 0xDF, 0xEF, 0xE7, 0xFF, 0xFC, 0x0F, 0xF7, 0xFF, 0xF8, 0xE7, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0xFF, 0xFF, 0xF9, 0xFF, 0x8F, 0xFF, 0xFF, 0xFE, 0x3F, 0xFF,  // 96
  0xFF, 0xCF, 0xFE, 0x5F, 0xDF, 0xEF, 0xE7, 0xFF, 0xFE, 0x3F, 0x7F, 0xFF, 0xBE, 0x31, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 97
  0xFF, 0xE7, 0xFF, 0x5F, 0xDF, 0xEF, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x08, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 98
  0xFF, 0xF3, 0xFF, 0x5F, 0xDF, 0xEF, 0xE7, 0xFF, 0xFF, 0xFF, 0xDF, 0xEF, 0xEF, 0xC6, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 99
  0xFF, 0xF9, 0xFF, 0x5F, 0xDF, 0xEF, 0xE7, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xF1, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 100
  0xFF, 0xFC, 0xFF, 0x5F, 0xDF, 0xEF, 0xE7, 0xFF, 0x9F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 101
  0xFF, 0xFE, 0x7F, 0x5F, 0xDF, 0xE7, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xBE, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 102
  0xFF, 0xFF, 0x3F, 0x5F, 0xDF, 0xF7, 0x87, 0xFF, 0xE3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 103
  0xFF, 0xFF, 0x9F, 0x5F, 0xDF, 0xF0, 0x23, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 104
  0xFF, 0xFF, 0x8F, 0x5F, 0xCF, 0xFD, 0xE0, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 105
  0xFF, 0xFF, 0xCF, 0x5F, 0xE7, 0xFE, 0x00, 0x7F, 0xFF, 0x9F, 0xFF, 0xFF, 0xFB, 0xFC, 0xFC, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 106
  0xFF, 0xFF, 0xE7, 0x5F, 0xF3, 0xE0, 0x34, 0x1F, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 107
  0xFF, 0xFF, 0xF3, 0x1F, 0xF8, 0x00, 0x16, 0x0F, 0xFF, 0xE3, 0xFF, 0xFD, 0xFE, 0xFF, 0xFF, 0x8F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 108
  0xFF, 0xFF, 0xF3, 0x1F, 0xFF, 0x03, 0xD7, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xFF, 0xF8, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 109
  0xFF, 0xFF, 0xF9, 0x1F, 0xF0, 0x3F, 0xD3, 0x80, 0xFF, 0xFC, 0xFF, 0xFF, 0x3F, 0xFF, 0xC0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 110
  0xFF, 0xFF, 0xFC, 0x1F, 0x80, 0x7F, 0x91, 0xC0, 0x7F, 0xFF, 0xFF, 0xFB, 0xFF, 0xFE, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 111
  0xFF, 0xFF, 0xFE, 0x4F, 0x30, 0xDF, 0x00, 0xE0, 0x1F, 0xFF, 0x9F, 0xFF, 0xFF, 0xF0, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 112
  0xFF, 0xFF, 0xFF, 0x2F, 0x68, 0x10, 0x08, 0x70, 0x8F, 0xFF, 0xF1, 0xFF, 0xFF, 0x81, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 113
  0xFF, 0xFF, 0xFF, 0x0F, 0x40, 0x00, 0xFC, 0x38, 0x63, 0xFF, 0xE7, 0xFF, 0xFC, 0x0F, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 114
  0xFF, 0xFF, 0xFF, 0x8F, 0x00, 0x0F, 0xFE, 0x1C, 0x01, 0xFF, 0xFF, 0xFF, 0xE0, 0xFF, 0xE7, 0xC1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 115
  0xFF, 0xFF, 0xFF, 0xC7, 0x00, 0x7F, 0xFF, 0x06, 0x2C, 0x7F, 0xFF, 0xFF, 0x07, 0xFF, 0xE3, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 116
  0xFF, 0xFF, 0xFF, 0xE0, 0x07, 0xFF, 0xFF, 0x83, 0x1C, 0x3F, 0xFF, 0xF0, 0x3F, 0xFF, 0x00, 0x3C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 117
  0xFF, 0xFF, 0xFF, 0xF0, 0x3F, 0xFF, 0xFF, 0xC1, 0x80, 0x0F, 0xFF, 0x81, 0xFF, 0xF8, 0x60, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 118
  0xFF, 0xFF, 0xFF, 0xFB, 0xFF, 0xFF, 0xFF, 0xE0, 0x8E, 0x03, 0xFC, 0x0F, 0xFF, 0xE3, 0x07, 0x83, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 119
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x2F, 0x01, 0xE0, 0x7F, 0xFE, 0x10, 0x3C, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 120
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x07, 0xC0, 0x03, 0xFF, 0xF8, 0xE1, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 121
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x0F, 0xC0, 0x13, 0xFF, 0xC3, 0x0F, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 122
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x1E, 0x01, 0xF9, 0xFE, 0x00, 0x3C, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 123
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x0F, 0xFC, 0xF8, 0xC1, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 124
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x7F, 0xFE, 0x02, 0x0F, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 125
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x7C, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 126
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x83, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 127
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 128
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x3C, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 129
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 130
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 131
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 132
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 133
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 134
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 135
  0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 136
  0xFF, 0xFE, 0x0E, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0x8F, 0xFF, 0xFF, 0xFF, 0x83, 0xCE, 0xBF, 0xFF, 0xFF, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0x3F, 0x8F, 0x86, 0xBD, 0xF8, 0x60, 0xFF,  // 137
  0xFF, 0xFE, 0xF7, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0x7F, 0xFF, 0xFF, 0xDF, 0xFB, 0xB6, 0xBD, 0xFF, 0xFF, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xBE, 0xDF, 0xF7, 0xBE, 0xBD, 0xF7, 0xEF, 0x7F,  // 138
  0xFE, 0x3E, 0xFA, 0xE6, 0x9D, 0x0D, 0xDF, 0x4C, 0x3A, 0x70, 0xDF, 0x7D, 0x22, 0x1C, 0x0F, 0xF7, 0xB6, 0xB8, 0xC3, 0xBB, 0x9A, 0x7E, 0x98, 0x74, 0xE1, 0xBE, 0xDF, 0xF7, 0xBE, 0xBD, 0xEF, 0xEF, 0xBF,  // 139
  0xFE, 0x3E, 0xFA, 0xDE, 0x6D, 0xF6, 0xBF, 0x37, 0xD9, 0xAF, 0x5F, 0xBC, 0xDD, 0xED, 0xDF, 0xF7, 0xCF, 0xFD, 0xBD, 0xBA, 0x79, 0xBE, 0x6F, 0xB3, 0x5E, 0xBF, 0x3F, 0xCF, 0x8F, 0xFD, 0xEF, 0xEF, 0xBF,  // 140
  0xFE, 0x3E, 0xFA, 0xCE, 0xED, 0x86, 0xBF, 0x76, 0x1B, 0xA0, 0x5F, 0xDD, 0xDD, 0x0D, 0xDF, 0xEF, 0xB7, 0xFD, 0xBD, 0xBA, 0xFB, 0xBE, 0xEC, 0x37, 0x40, 0xBE, 0xB7, 0xE7, 0xF7, 0xFD, 0xEF, 0xEF, 0xBF,  // 141
  0xFF, 0xFE, 0xFA, 0xF6, 0xED, 0x76, 0xBF, 0x75, 0xDB, 0xAF, 0xDF, 0xED, 0xDC, 0xED, 0xDF, 0xEF, 0x7B, 0xFD, 0xBD, 0xBA, 0xFB, 0xBE, 0xEB, 0xB7, 0x5F, 0xBD, 0xCF, 0xF7, 0xF7, 0xFD, 0xEF, 0xEF, 0xBF,  // 142
  0xFF, 0xFE, 0xE6, 0xF6, 0xED, 0x77, 0x3F, 0x75, 0xDB, 0xB7, 0xDF, 0xED, 0xDC, 0xED, 0xDF, 0xDF, 0x7B, 0xFD, 0xB9, 0xB2, 0x7B, 0xBE, 0xEB, 0xB7, 0x6F, 0xBD, 0xCF, 0xF6, 0xF7, 0xFD, 0xF7, 0xEE, 0x7F,  // 143
  0xFF, 0xFE, 0x1E, 0xCE, 0x1D, 0x87, 0x7F, 0x0E, 0x1B, 0xB8, 0x5F, 0x1D, 0xDD, 0x0D, 0xEF, 0x9F, 0x87, 0xFE, 0xC7, 0xCB, 0x9B, 0xBE, 0x1C, 0x37, 0x70, 0xBE, 0x27, 0x0E, 0x0F, 0xFC, 0x38, 0x61, 0xFF,  // 144
  0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0x7F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 145
  0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFE, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 146
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 147
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 148
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 149
  0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xFF, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBE, 0xFF,  // 150
  0xFE, 0x1F, 0xFB, 0x7F, 0xEF, 0xFF, 0xFF, 0xBF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xE1, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFB, 0xFF, 0xFD, 0xE1, 0xFF, 0xBF, 0xBE, 0xFF,  // 151
  0xC6, 0xEF, 0xFF, 0x6F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xEF, 0xBB, 0xFF, 0xFF, 0xFF, 0xEF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xDE, 0xFF, 0xFF, 0xBE, 0xFF,  // 152
  0xC6, 0xED, 0xDB, 0x47, 0xED, 0x3E, 0xED, 0xB1, 0x0D, 0x87, 0x39, 0xE1, 0xD3, 0xCB, 0xEF, 0x1A, 0x70, 0xC5, 0x38, 0x47, 0xD3, 0x84, 0x5D, 0xB0, 0xE2, 0xDB, 0x4F, 0x2F, 0xBF, 0x5D, 0xB9, 0xB6, 0xBB,  // 153
  0xC6, 0x1D, 0xDB, 0x6F, 0xEC, 0xDE, 0xCD, 0xB6, 0xF5, 0x7A, 0xF7, 0xFE, 0xCD, 0x33, 0xE1, 0xB9, 0xAF, 0x5C, 0xD7, 0xAF, 0xCD, 0x7A, 0xD9, 0xAF, 0x6E, 0xBB, 0x34, 0xCF, 0xBF, 0x5D, 0xA7, 0xAE, 0xD7,  // 154
  0xFE, 0xED, 0xDB, 0x6C, 0x6D, 0xDF, 0x4B, 0xB6, 0x05, 0x02, 0x73, 0xF0, 0xDD, 0x7B, 0xEF, 0xBB, 0xA0, 0x5D, 0xD0, 0x2F, 0xDD, 0x02, 0xE9, 0x6F, 0x6E, 0x7B, 0x75, 0xEF, 0xBF, 0x5D, 0xAF, 0x9E, 0xD7,  // 155
  0xFE, 0xED, 0xDB, 0x6F, 0xED, 0xDF, 0x2B, 0xB6, 0xFD, 0x7F, 0xBD, 0xEE, 0xDD, 0x7B, 0xEF, 0xBB, 0xAF, 0xDD, 0xD7, 0xEF, 0xDD, 0x7E, 0xE5, 0x6F, 0x6E, 0xBB, 0x75, 0xEF, 0xBF, 0x5D, 0xAF, 0xAE, 0xD7,  // 156
  0xFE, 0xED, 0x9B, 0x6F, 0xED, 0xDF, 0x33, 0xB7, 0x7D, 0xBF, 0xBD, 0xEE, 0xDD, 0x73, 0xEF, 0xBB, 0xB7, 0xDD, 0xDB, 0xEF, 0xDD, 0xBE, 0xE6, 0x6E, 0x6E, 0xDB, 0x75, 0xCF, 0xDE, 0xD9, 0xA7, 0xB6, 0xE7,  // 157
  0xFE, 0x1E, 0x5B, 0x77, 0xED, 0xDF, 0xB7, 0xB7, 0x85, 0xC2, 0x73, 0xF0, 0xDD, 0x8B, 0xE1, 0xDB, 0xB8, 0x5D, 0xDC, 0x37, 0xDD, 0xC3, 0x76, 0xF1, 0xEE, 0xCB, 0x76, 0x2F, 0xE1, 0xE5, 0xB9, 0xB2, 0xEF,  // 158
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xFE, 0x7F, 0xFF, 0xFF, 0xEF,  // 159
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF,  // 160
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF,  // 161
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 162
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 163
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xFB, 0xFF, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 164
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xDB, 0xFF, 0xFB, 0xFF, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 165
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF7, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xFB, 0xFF, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 166
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x9C, 0x5A, 0x63, 0xC6, 0x21, 0xD2, 0x3D, 0xD8, 0x77, 0x63, 0xD2, 0x30, 0xE9, 0xDB, 0x0F, 0xCB, 0x0B, 0xB7, 0x30, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 167
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1E, 0x6D, 0xD9, 0xB7, 0xEE, 0xDE, 0xCD, 0xDE, 0xB7, 0xB7, 0x6F, 0xCD, 0xCF, 0x66, 0xDA, 0xF7, 0x32, 0xF5, 0x74, 0xEF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 168
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1E, 0xED, 0xDB, 0xB7, 0xEE, 0xDE, 0xDD, 0xDE, 0xB7, 0xB7, 0x6F, 0xDD, 0xCF, 0x6E, 0xDA, 0x07, 0x7A, 0x05, 0x75, 0xE0, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 169
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x1E, 0xED, 0xDB, 0xB7, 0xEE, 0xDE, 0xDD, 0xDE, 0xB7, 0xB7, 0x6F, 0xDD, 0xCF, 0x6E, 0xDA, 0xFF, 0x7A, 0xFD, 0x75, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 170
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xED, 0xDB, 0xB7, 0xEE, 0xDC, 0xDD, 0xDF, 0x37, 0x36, 0x6F, 0xDD, 0xCE, 0x6E, 0xDB, 0x7F, 0x73, 0x7D, 0x74, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 171
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x1D, 0xDB, 0xBB, 0xEE, 0xE3, 0xDD, 0xDF, 0x78, 0xF9, 0x6F, 0xDD, 0xD1, 0xE1, 0xDB, 0x87, 0x8B, 0x86, 0xF7, 0x38, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 172
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 173
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 174
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 175
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF   // 176
};
//2013/04/03 Resolution: 264 x 176
#endif


#define LINE_SIZE	64  /**< maximum data line size */

/**
 * \brief Support 1.44", 2" and 2.7" three type EPD */
enum EPD_SIZE {
	EPD_144,
	EPD_200,
	EPD_270
};

/**
 * \brief The definition for driving stage to compare with for getting Odd and Even data  */
#define BLACK0   (uint8_t)(0x03) /**< getting bit1 or bit0 as black color(11) */
#define BLACK1   (uint8_t)(0x0C) /**< getting bit3 or bit2 as black color(11) */
#define BLACK2   (uint8_t)(0x30) /**< getting bit5 or bit4 as black color(11) */
#define BLACK3   (uint8_t)(0xC0) /**< getting bit7 or bit6 as black color(11) */
#define WHITE0   (uint8_t)(0x02) /**< getting bit1 or bit0 as white color(10) */
#define WHITE1   (uint8_t)(0x08) /**< getting bit3 or bit2 as white color(10) */
#define WHITE2   (uint8_t)(0x20) /**< getting bit5 or bit4 as white color(10) */
#define WHITE3   (uint8_t)(0x80) /**< getting bit7 or bit6 as white color(10) */
#define NOTHING0 (uint8_t)(0x01) /**< getting bit1 or bit0 as nothing input(01) */
#define NOTHING1 (uint8_t)(0x04) /**< getting bit3 or bit2 as nothing input(01) */
#define NOTHING2 (uint8_t)(0x10) /**< getting bit5 or bit4 as nothing input(01) */
#define NOTHING3 (uint8_t)(0x40) /**< getting bit7 or bit6 as nothing input(01) */
#define NOTHING  (uint8_t)(0x00) /**< sending Nothing frame, 01=Nothing, 0101=0x5 */


#define ALL_BLACK	 	 (uint8_t)(0xFF)
#define ALL_WHITE		 (uint8_t)(0xAA)
#define BORDER_BYTE_B    (uint8_t)(0xFF)
#define BORDER_BYTE_W    (uint8_t)(0xAA)
#define ERROR_BUSY       (uint8_t)(0xF0)
#define ERROR_COG_ID     (uint8_t)(0xF1)
#define ERROR_BREAKAGE   (uint8_t)(0xF2)
#define ERROR_DC         (uint8_t)(0xF3)
#define ERROR_CHARGEPUMP (uint8_t)(0xF4)
#define RES_OK           (uint8_t)(0x00)

/**
 * \brief The COG Driver uses a buffer to update the EPD line by line.
   \note Use the 2.7" maximum data(66)+scan(44)+dummy(1) bytes as line buffer size=111.*/
#define LINE_BUFFER_DATA_SIZE 111

/**
 * \brief Support 1.44", 2" and 2.7" three type EPD currently */
#define COUNT_OF_EPD_TYPE 3

/**
 * \brief Four driving stages */
enum Stage {
	Stage1, /**< Inverse previous image */
	Stage2, /**< White */
	Stage3, /**< Inverse new image */
	Stage4  /**< New image */
};

#if (defined COG_V110_G1)
/** 
 * \brief Line data structure of 1.44 inch EPD
 * \note 
 * 1.44 inch needs to put border_control byte at the front of data line. 
 * Refer to COG document Section 5.2 - 1.44" Input Data Order. 
 */
struct COG_144_line_data_t {
	uint8_t border_byte;  /**< Internal border_control, for 1.44" EPD only */
	uint8_t even[16]; /**< 1.44" even byte array */
	uint8_t scan[24]; /**< 1.44" scan byte array */
	uint8_t odd [16]; /**< 1.44" odd byte array */
} ATTRPACKED;

/** 
 * \brief Line data structure of 2 inch EPD
 * \note
 * Add one byte x00 to complete a line
 * Refer to COG document Section 5.2 - 2" Input Data Order. 
 */
struct COG_200_line_data_t {
	uint8_t even[25]; /**< 2" even byte array */
	uint8_t scan[24]; /**< 2" scan byte array */
	uint8_t odd [25]; /**< 2" odd byte array */
	uint8_t dummy_data;	/**< dummy byte 0x00 */
} ATTRPACKED;

/** 
 * \brief Line data structure of 2.7 inch EPD
 * \note
 * Add one byte x00 to complete a line
 * Refer to COG document Section 5.2 - 2.7" Input Data Order. 
 */
struct COG_270_line_data_t {
	uint8_t even[33]; /**< 2.7" even byte array */
	uint8_t scan[44]; /**< 2.7" scan byte array */
	uint8_t odd [33]; /**< 2.7" odd byte array */
	uint8_t dummy_data;	/**< dummy byte 0x00 */
} ATTRPACKED;

#elif (defined COG_V110_G2) 

/** 
 * \brief Line data structure of 1.44 inch EPD with G2 COG
 * \note 
 * 1.44 inch needs to put border_control byte at the front of data line. 
 * Refer to COG document Section 5.2 - 1.44" Input Data Order. 
 */
struct COG_144_line_data_t {
	uint8_t even[16]; /**< 1.44" even byte array */
	uint8_t scan[24]; /**< 1.44" scan byte array */
	uint8_t odd [16]; /**< 1.44" odd byte array */
	uint8_t border_byte; /**< Internal border_control*/
} ATTRPACKED;

/** 
 * \brief Line data structure of 2 inch EPD with G2 COG
 * \note
 * Add one byte x00 to complete a line
 * Refer to COG document Section 5.2 - 2" Input Data Order. 
 */
struct COG_200_line_data_t {
    uint8_t border_byte; /**< Internal border_control*/
	uint8_t even[25]; /**< 2" even byte array */
	uint8_t scan[24]; /**< 2" scan byte array */
	uint8_t odd [25]; /**< 2" odd byte array */
} ATTRPACKED;
    
/** 
 * \brief Line data structure of 2.7 inch EPD with G2 COG
 * \note
 * Add one byte x00 to complete a line
 * Refer to COG document Section 5.2 - 2.7" Input Data Order. 
 */
struct COG_270_line_data_t {
    uint8_t border_byte; /**< Internal border_control*/
	uint8_t even[33]; /**< 2.7" even byte array */
	uint8_t scan[44]; /**< 2.7" scan byte array */
	uint8_t odd [33]; /**< 2.7" odd byte array */
} ATTRPACKED;

#elif (defined COG_V230_G2)
/** 
 * \brief Line data structure of 1.44 inch V230 EPD with G2 COG
 * \note 
 * 1.44 inch needs to put border_control byte at the front of data line. 
 * Refer to COG document Section 5.2 - 1.44" Input Data Order. 
 */
struct COG_144_line_data_t {
	uint8_t even[16]; /**< 1.44" even byte array */
	uint8_t scan[24]; /**< 1.44" scan byte array */
	uint8_t odd [16]; /**< 1.44" odd byte array */
	uint8_t border_byte; /**< Internal border_control*/
} ATTRPACKED;

/** 
 * \brief Line data structure of 2 inch V230 EPD with G2 COG
 * \note
 * Add one byte x00 to complete a line
 * Refer to COG document Section 5.2 - 2" Input Data Order. 
 */
struct COG_200_line_data_t {
    uint8_t border_byte; /**< Internal border_control*/
	uint8_t even[25]; /**< 2" even byte array */
	uint8_t scan[24]; /**< 2" scan byte array */
	uint8_t odd [25]; /**< 2" odd byte array */
} ATTRPACKED;
    
/** 
 * \brief Line data structure of 2.7 inch V230 EPD with G2 COG
 * \note
 * Add one byte x00 to complete a line
 * Refer to COG document Section 5.2 - 2.7" Input Data Order. 
 */
struct COG_270_line_data_t {
    uint8_t border_byte; /**< Internal border_control*/
	uint8_t even[33]; /**< 2.7" even byte array */
	uint8_t scan[44]; /**< 2.7" scan byte array */
	uint8_t odd [33]; /**< 2.7" odd byte array */
} ATTRPACKED;

struct EPD_WaveformTable_Struct
{
	uint8_t stage1_frame1;
	uint8_t stage1_block1;
	uint8_t stage1_step1;
	
	uint16_t stage2_t1;
	uint16_t stage2_t2;
	uint8_t stage2_cycle;
	
	uint8_t stage3_frame3;
	uint8_t stage3_block3;
	uint8_t stage3_step3;
};
struct EPD_V230_G2_Struct
{
	 int16_t frame_y0;
	 int16_t frame_y1;
	 int16_t block_y0;
	 int16_t block_y1;
	 int16_t block_size;
	 int16_t step_size;
	 int16_t frame_cycle;
	 int16_t step_y0;
	 int16_t step_y1;
	 int16_t number_of_steps;
};
#else
#error "ERROR: The EPD's COG type is not defined."
#endif

/** 
 * \brief Packet structure of a line data */
typedef union {
	union {
		struct COG_144_line_data_t line_data_for_144; /**< line data structure of 1.44" EPD */
		struct COG_200_line_data_t line_data_for_200; /**< line data structure of 2" EPD */
		struct COG_270_line_data_t line_data_for_270; /**< line data structure of 2.7" EPD */
	} line_data_by_size; /**< the line data of specific EPD size */
	uint8_t uint8[LINE_BUFFER_DATA_SIZE]; /**< the maximum line buffer data size as length */
} COG_line_data_packet_type;

/** 
 * \brief Define the COG driver's parameters */
struct COG_parameters_t {
	uint8_t   channel_select[8]; /**< the SPI register data of Channel Select */
	uint8_t   voltage_level;     /**< the SPI register data of Voltage Level */
	uint16_t  horizontal_size;   /**< the bytes of width of EPD */
	uint16_t  vertical_size;     /**< the bytes of height of EPD */
	uint8_t   data_line_size;    /**< Data + Scan + Dummy bytes */
	uint16_t  frame_time_offset; /**< the rest of frame time in a stage */
	uint16_t  stage_time;        /**< defined stage time */
} ;


/** 
 * \brief Developer needs to create an external function if wants to read flash */
typedef void (*EPD_read_flash_handler)(long flash_address,uint8_t *target_buffer, 
		uint8_t byte_length);

extern const struct COG_parameters_t  COG_parameters[COUNT_OF_EPD_TYPE];
extern const uint8_t   SCAN_TABLE[4];
void EPD_init(void);
void EPD_power_on (void);
uint8_t EPD_initialize_driver (uint8_t EPD_type_index);
void EPD_display_from_flash_prt (uint8_t EPD_type_index, long previous_image_flash_address,
	long new_image_flash_address,EPD_read_flash_handler On_EPD_read_flash);
uint8_t EPD_power_off (uint8_t EPD_type_index);
void COG_driver_EPDtype_select(uint8_t EPD_type_index);



void EPD_display_init(void);
void EPD_power_init(uint8_t EPD_type_index);
void EPD_display_from_flash(uint8_t EPD_type_index,long previous_image_address,
	long new_image_address,EPD_read_flash_handler On_EPD_read_flash);
void EPD_display_from_flash_Ex(uint8_t EPD_type_index,long previous_image_address,
	long new_image_address,EPD_read_flash_handler On_EPD_read_flash);


static int get_temperature(void)
{
	return 30;
}

static void initialize_temperature(void)
{

}

#ifdef COG_V230_G2

#define ADDRESS_NULL		0xffffffff
//EPD Panel parameters
const struct COG_parameters_t COG_parameters[COUNT_OF_EPD_TYPE]  = {
	{
		// FOR 1.44"
		{0x00,0x00,0x00,0x00,0x00,0x0F,0xFF,0x00},
		0x03,
		(128/8),
		96,
		((((128+96)*2)/8)+1),
		0,
		480
	},
	{
		// For 2.0"
		{0x00,0x00,0x00,0x00,0x01,0xFF,0xE0,0x00},
		0x03,
		(200/8),
		96,
		((((200+96)*2)/8)+1),
		0,
		480
	},
	{
		// For 2.7"
		{0x00,0x00,0x00,0x7F,0xFF,0xFE,0x00,0x00},
		0x00,
		(264/8),
		176,
		((((264+176)*2)/8)+1),
		0,
		630
	}
};

//EPD Waveform parameters
 const struct EPD_WaveformTable_Struct E_Waveform[COUNT_OF_EPD_TYPE][2]  = {
		{// FOR 1.44"
			{//50 ?? T ?? 40
				4,				//stage1_frame1
				16,				//stage1_block1
				2,				//stage1_step1				
				155,			//stage2_t1
				155,			//stage2_t2
				4,				//stage2_cycle
				4,				//stage3_frame3
				16,				//stage3_block3
				2				//stage3_step3
			}
			,{//40 ?? T ?? 10
				4,				//stage1_frame1
				16,				//stage1_block1
				2,				//stage1_step1				
				155,			//stage2_t1
				155,			//stage2_t2
				4,				//stage2_cycle
				4,				//stage3_frame3
				16,				//stage3_block3
				2				//stage3_step3
			}
			
		},
		{// For 2.0"
			{//50 ?? T ?? 40
				4,				//stage1_frame1
				48,				//stage1_block1
				2,				//stage1_step1				
				155,			//stage2_t1
				155,			//stage2_t2
				4,				//stage2_cycle
				4,				//stage3_frame3
				48,				//stage3_block3
				2				//stage3_step3
			}
			,{//40 ?? T ?? 10
				2,				//stage1_frame1
				48,				//stage1_block1
				2,				//stage1_step1				
				155,			//stage2_t1
				155,			//stage2_t2
				4,				//stage2_cycle
				2,				//stage3_frame3
				48,				//stage3_block3
				2				//stage3_step3
			}
		},
		{// For 2.7"
			{//50 ?? T ?? 40
				4,				//stage1_frame1
				22,				//stage1_block1
				2,				//stage1_step1				
				155,			//stage2_t1
				155,			//stage2_t2
				4,				//stage2_cycle
				4,				//stage3_frame3
				22,				//stage3_block3
				2				//stage3_step3
			}
			,{//40 ?? T ?? 10
				2,				//stage1_frame1
				22,				//stage1_block1
				2,				//stage1_step1				
				155,			//stage2_t1
				155,			//stage2_t2
				4,				//stage2_cycle
				2,				//stage3_frame3
				22,				//stage3_block3
				2				//stage3_step3
			}
		},
	 
 };

const uint8_t   SCAN_TABLE[4] = {0xC0,0x30,0x0C,0x03};
	
static const struct EPD_WaveformTable_Struct *action__Waveform_param;
static COG_line_data_packet_type COG_Line;
static EPD_read_flash_handler _On_EPD_read_flash=NULL;
static uint16_t current_frame_time;
static uint8_t  *data_line_even;
static uint8_t  *data_line_odd;
static uint8_t  *data_line_scan;
static uint8_t  *data_line_border_byte;

/**
* \brief According to EPD size and temperature to get stage_time
* \note Refer to COG document Section 5.3 for more details
*
* \param EPD_type_index The defined EPD size
*/
static void set_temperature_factor(uint8_t EPD_type_index) {
	int8_t temperature;
	temperature = get_temperature();	
        if (50 >= temperature  && temperature > 40){
			action__Waveform_param=&E_Waveform[EPD_type_index][0];
		}else if (40 >= temperature  && temperature > 10){
			action__Waveform_param=&E_Waveform[EPD_type_index][1];
		}else action__Waveform_param=&E_Waveform[EPD_type_index][1]; //Default
}


/**
* \brief Set EPD_CS pin to high
*/
void EPD_cs_high (void) {
}

/**
* \brief Set EPD_CS pin to low
*/
void EPD_cs_low (void) {
}

/**
* \brief Set Flash_CS pin to high
*/
void EPD_flash_cs_high(void) {
	//set_gpio_high(Flash_CS_PIN);
}

/**
* \brief Set Flash_CS pin to low
*/
void EPD_flash_cs_low (void) {
	//set_gpio_low(Flash_CS_PIN);
}

/**
* \brief Set /RESET pin to high
*/
void EPD_rst_high (void) {
	board_lcd_reset(1); 	// Pull RST pin high
	board_update();
}

/**
* \brief Set /RESET pin to low
*/
void EPD_rst_low (void) {
	board_lcd_reset(0); 	// Pull RST pin down
	board_update();
}

/**
* \brief Set DISCHARGE pin to high
*/
void EPD_discharge_high (void) {
	eink_lcd_discharge(1);
}

/**
* \brief Set DISCHARGE pin to low
*/
void EPD_discharge_low (void) {
	eink_lcd_discharge(0);
}

/**
* \brief Set Vcc (PANEL_ON) to high
*/
void EPD_Vcc_turn_on (void) {
	eink_lcd_pwron(1);
}

/**
* \brief Set Vcc (PANEL_ON) to low
*/
void EPD_Vcc_turn_off (void) {
	eink_lcd_pwron(0);
}

/**
* \brief Set BORDER_CONTROL pin to high
*/
void EPD_border_high(void) {
	eink_lcd_on_border(1);
}

/**
* \brief Set BORDER_CONTROL pin to low
*/
void EPD_border_low (void) {
	eink_lcd_on_border(0);
}

/**
* \brief Set PWM pin to high
*/
void EPD_pwm_high(void) {
	//set_gpio_high(PWM_PIN);
}

/**
* \brief Set PWM pin to low
*/
void EPD_pwm_low (void) {
	//set_gpio_low(PWM_PIN);
}

/**
* \brief Set MISO pin of SPI to low
*/
void SPIMISO_low(void) {
	//config_gpio_dir_o(SPIMISO_PIN);
	//set_gpio_low(SPIMISO_PIN);
}

/**
* \brief Set MOSI pin of SPI to low
*/
void SPIMOSI_low(void) {
	//set_gpio_low(SPIMOSI_PIN);
}

/**
* \brief Set Clock of SPI to low
*/
void SPICLK_low(void) {
	//set_gpio_low(SPICLK_PIN);
}

/**
* \brief Get BUSY pin status
*/
int EPD_IsBusy(void) {
	return eink_lcd_busy();
}

/**
* \brief Configure GPIO
*/
void EPD_initialize_gpio(void) {
	//config_gpio_dir_i( EPD_BUSY_PIN);
	//config_gpio_dir_o( EPD_CS_PIN);
	//config_gpio_dir_o( EPD_RST_PIN);
	//config_gpio_dir_o( EPD_PANELON_PIN);
	//config_gpio_dir_o( EPD_DISCHARGE_PIN);
	//config_gpio_dir_o( EPD_BORDER_PIN);
	//config_gpio_dir_o( Flash_CS_PIN);
	//config_gpio_dir_i( Temper_PIN);
	EPD_flash_cs_high();
	EPD_border_low();
}




/**
* \brief Initialize the EPD hardware setting
*/
void EPD_display_hardware_init (void) {
	EPD_initialize_gpio();
	EPD_Vcc_turn_off();
	//epd_spi_init();
	initialize_temperature();
	//EPD_cs_low();
	EPD_pwm_low();
	EPD_rst_low();
	EPD_discharge_low();
	EPD_border_low();
	//initialize_EPD_timer();
}



/**
* \brief Initialize the EPD hardware setting
*/
void EPD_init(void) {
	EPD_display_hardware_init();
	EPD_cs_low();
	EPD_rst_low();
	EPD_discharge_low();
	EPD_border_low();
}

/**
* \brief Select the EPD size to get line data array for driving COG
*
* \param EPD_type_index The defined EPD size
*/
void COG_driver_EPDtype_select(uint8_t EPD_type_index) {
	switch(EPD_type_index) {
		case EPD_144:
		data_line_even = &COG_Line.line_data_by_size.line_data_for_144.even[0];
		data_line_odd  = &COG_Line.line_data_by_size.line_data_for_144.odd[0];
		data_line_scan = &COG_Line.line_data_by_size.line_data_for_144.scan[0];
		data_line_border_byte = &COG_Line.line_data_by_size.line_data_for_144.border_byte;
		break;
		case EPD_200:
		data_line_even = &COG_Line.line_data_by_size.line_data_for_200.even[0];
		data_line_odd  = &COG_Line.line_data_by_size.line_data_for_200.odd[0];
		data_line_scan = &COG_Line.line_data_by_size.line_data_for_200.scan[0];
		data_line_border_byte = &COG_Line.line_data_by_size.line_data_for_200.border_byte;
		break;
		case EPD_270:
		data_line_even = &COG_Line.line_data_by_size.line_data_for_270.even[0];
		data_line_odd  = &COG_Line.line_data_by_size.line_data_for_270.odd[0];
		data_line_scan = &COG_Line.line_data_by_size.line_data_for_270.scan[0];
		data_line_border_byte = &COG_Line.line_data_by_size.line_data_for_270.border_byte;
		break;
	}
}

/**
* \brief Power on COG Driver
* \note For detailed flow and description, please refer to the COG G2 document Section 3.
*/
void EPD_power_on (void) {	
	//epd_spi_init_2M();
	/* Initial state */
	EPD_Vcc_turn_on(); //Vcc and Vdd >= 2.7V	
	EPD_cs_high();
	EPD_border_high();
	EPD_rst_high();
	local_delay_ms(5);	
	EPD_rst_low();
	local_delay_ms(5);
	EPD_rst_high();
	local_delay_ms(5);
}

void epd_spi_send_byte(uint8_t register_index, uint8_t register_data) 
{
	eink_write(register_index, register_data);
}
/**
* \brief Initialize COG Driver
* \note For detailed flow and description, please refer to the COG G2 document Section 4.
*
* \param EPD_type_index The defined EPD size
*/
uint8_t EPD_initialize_driver (uint8_t EPD_type_index) {
	
	uint16_t i;
	// Empty the Line buffer
	//for (i = 0; i < LINE_BUFFER_DATA_SIZE; i ++) {
	//	COG_Line.uint8[i] = 0x00;
	//}
	memset(COG_Line.uint8, 0x00, sizeof COG_Line.uint8);
	// Determine the EPD size for driving COG
	COG_driver_EPDtype_select(EPD_type_index);

	// Sense temperature to determine Temperature Factor
	set_temperature_factor(EPD_type_index);
	i = 0;
	
	while (EPD_IsBusy()) {
		if ((i++) >= 0x0FFF) return ERROR_BUSY;
	}
	
	//Check COG ID
	if ((SPI_R(0x72,0x00) & 0x0f) !=0x02) return ERROR_COG_ID;

	//Disable OE
	epd_spi_send_byte(0x02,0x40);	

	//Check Breakage
	if ((SPI_R(0x0F,0x00) & 0x80) != 0x80) return ERROR_BREAKAGE;
	
	//Power Saving Mode
 	epd_spi_send_byte(0x0B, 0x02);

	//Channel Select
	epd_spi_send (0x01, COG_parameters[EPD_type_index].channel_select, 8);

	//High Power Mode Osc Setting
	epd_spi_send_byte(0x07,0xD1);

	//Power Setting
	epd_spi_send_byte(0x08,0x02);

	//Set Vcom level
	epd_spi_send_byte(0x09,0xC2);

	//Power Setting
	epd_spi_send_byte(0x04,0x03);

	//Driver latch on
	epd_spi_send_byte(0x03,0x01);

	//Driver latch off
	epd_spi_send_byte(0x03,0x00);

	local_delay_ms(5);

	//Chargepump Start
	i=0;
	do {
		//Start chargepump positive V
		//VGH & VDH on
		epd_spi_send_byte(0x05,0x01);

		local_delay_ms_spool(240);

		//Start chargepump neg voltage
		//VGL & VDL on
		epd_spi_send_byte(0x05,0x03);

		local_delay_ms_spool(40);

		//Set chargepump
		//Vcom_Driver to ON
		//Vcom_Driver on
		epd_spi_send_byte(0x05,0x0F);

		local_delay_ms_spool(40);

		//Check DC/DC
		if ((SPI_R(0x0F,0x00) & 0x40) != 0x00) break;	
		
	}while((i++) != 4);
	
	if (i>=4) 
	{
		//Output enable to disable
		epd_spi_send_byte(0x02,0x40);
		return ERROR_CHARGEPUMP;
	}
	else  return RES_OK;
}

/**
* \brief Initialize the parameters of Block type stage 
*
* \param EPD_type_index The defined EPD size
* \param EPD_V230_G2_Struct The Block type waveform structure
* \param block_size The width of Block size
* \param step_size The width of Step size
* \param frame_cycle The width of Step size
*/
void stage_init(uint8_t EPD_type_index,struct EPD_V230_G2_Struct *S_epd_v230,
				uint8_t block_size,uint8_t step_size,
				uint8_t frame_cycle)
{
	S_epd_v230->frame_y0 = 0;
	S_epd_v230->frame_y1 = 176;
	S_epd_v230->block_y0 = 0;
	S_epd_v230->block_y1 = 0;
	S_epd_v230->step_y0 = 0;
	S_epd_v230->step_y1 = 0;
	S_epd_v230->block_size = action__Waveform_param->stage1_block1;
	S_epd_v230->step_size =action__Waveform_param->stage1_step1;
	S_epd_v230->frame_cycle = action__Waveform_param->stage1_frame1;
	S_epd_v230->number_of_steps = (COG_parameters[EPD_type_index].vertical_size / S_epd_v230->step_size) + (action__Waveform_param->stage1_block1 / action__Waveform_param->stage1_step1) -1;
	
}

static int EPD_timer;
static void start_EPD_timer(void)
{
	EPD_timer = 0;
}
static void stop_EPD_timer(void)
{

}
static uint32_t get_current_time_tick(void)
{
	return EPD_timer += 50;
}

/**
* \brief For Frame type waveform to update all black/white pattern
*
* \param EPD_type_index The defined EPD size
* \param bwdata Black or White color to whole screen
* \param work_time The working time
*/
static inline void same_data_frame (uint8_t EPD_type_index, uint8_t bwdata, uint32_t work_time) {
	uint16_t i;
	for (i = 0; i <  COG_parameters[EPD_type_index].horizontal_size; i++) {
		data_line_even[i]=bwdata;
		data_line_odd[i]=bwdata;
	}
	start_EPD_timer();
	do 
	{	
		for (i = 0; i < COG_parameters[EPD_type_index].vertical_size; i++) {
			
			/* Scan byte shift per data line */
			data_line_scan[(i>>2)]=SCAN_TABLE[(i%4)];
			
			/* Sending data */
			epd_spi_send (0x0A, (uint8_t *)&COG_Line.uint8, COG_parameters[EPD_type_index].data_line_size);
		 
			/* Turn on Output Enable */
			epd_spi_send_byte(0x02, 0x07);
		
			data_line_scan[(i>>2)]=0;
			
		}
	} while (get_current_time_tick()<work_time);
		/* Stop system timer */
	stop_EPD_timer();
}

/**
* \brief Write nothing Line to COG
* \note A line whose all Scan Bytes are 0x00
*
* \param EPD_type_index The defined EPD size
*/
void nothing_line(uint8_t EPD_type_index) {
	uint16_t i;
	for (i = 0; i <  COG_parameters[EPD_type_index].horizontal_size; i++) {
		data_line_even[i]	=	NOTHING;
		data_line_odd[i]	=	NOTHING;
	}
}


/**
* \brief Get line data of Stage 1 and 3
*
* \note
* - One dot/pixel is comprised of 2 bits which are White(10), Black(11) or Nothing(01).
*   The image data bytes must be divided into Odd and Even bytes.
* - The COG driver uses a buffer to write one line of data (FIFO) - interlaced
*   It's different order from COG_G1
*   Odd byte {D(199,y),D(197,y), D(195,y), D(193,y)}, ... ,{D(7,y),D(5,y),D(3,y), D(1,y)}
*   Scan byte {S(96), S(95)...}
*   Odd byte  {D(2,y),D(4,y), D(6,y), D(8,y)}, ... ,{D(194,y),D(196,y),D(198,y), D(200,y)}
* - For more details on the driving stages, please refer to the COG G2 document Section 5.
*
* \param EPD_type_index The defined EPD size
* \param image_ptr The pointer of memory that stores image that will send to COG
* \param stage_no The assigned stage number that will proceed
*/
void read_line_data_handle(uint8_t EPD_type_index, const uint8_t *image_ptr,uint8_t stage_no)
{
	int16_t x,y,k;
	uint8_t	temp_byte; // Temporary storage for image data check
	k=COG_parameters[EPD_type_index].horizontal_size-1;	
	for (x =0 ; x < COG_parameters[EPD_type_index].horizontal_size ; x++) {
				temp_byte = *image_ptr++;				
				switch(stage_no) {
					case Stage1: // Inverse image
					/* Example at stage 1 to get Even and Odd data. It's different order from G1.
					* +---------+----+----+----+----+----+----+----+----+
					* |         |bit7|bit6|bit5|bit4|bit3|bit2|bit1|bit0|
					* |temp_byte+----+----+----+----+----+----+----+----+
					* |         |  1 |  0 |  1 |  1 |  0 |  1 |  0 |  0 |
					* +---------+----+----+----+----+----+----+----+----+ */
					data_line_odd[x]       = ((temp_byte & 0x40) ? BLACK3  : WHITE3); // WHITE3 = 0x80 = 1000 0000
					data_line_odd[x]      |= ((temp_byte & 0x10) ? BLACK2  : WHITE2); // BLACK2 = 0x30 = 0011 0000
					data_line_odd[x]      |= ((temp_byte & 0x04) ? BLACK1  : WHITE1); // BLACK1 = 0x0C = 0000 1100
					data_line_odd[x]	  |= ((temp_byte & 0x01) ? BLACK0  : WHITE0); // WHITE0 = 0x02 = 0000 0010
					/* data_line_odd[x] = 1000 0000 | 0011 0000 | 0000 1100 | 0000 0010 = 1011 1110 ==> 1011 1110
					* See Even data row at the table below*/
					
					data_line_even[k]    = ((temp_byte & 0x80) ? BLACK0  : WHITE0); // BLACK0 = 0x03 = 0000 0011
					data_line_even[k]   |= ((temp_byte & 0x20) ? BLACK1  : WHITE1); // BLACK1 = 0x0C = 0000 1100
					data_line_even[k]   |= ((temp_byte & 0x08) ? BLACK2  : WHITE2); // WHITE2 = 0x20 = 0010 0000
					data_line_even[k--] |= ((temp_byte & 0x02) ? BLACK3  : WHITE3); // WHITE3 = 0x80 = 1000 0000
					/* data_line_even[k] = 0000 0011 | 0000 1100 | 0010 0000 | 1000 0000 = 1010 1111 ==> 1111 1010
					* See Odd data row at the table below
					* +---------+----+----+----+----+----+----+----+----+
					* |         |bit7|bit6|bit5|bit4|bit3|bit2|bit1|bit0|
					* |temp_byte+----+----+----+----+----+----+----+----+
					* |         |  1 |  0 |  1 |  1 |  0 |  1 |  0 |  0 |
					* +---------+----+----+----+----+----+----+----+----+
					* | Color   |  W |  B |  W |  W |  B |  W |  B |  B | W=White, B=Black, N=Nothing
					* +---------+----+----+----+----+----+----+----+----+
					* | Stage 1 |  B |  W |  B |  B |  W |  B |  W |  W | Inverse
					* +---------+----+----+----+----+----+----+----+----+
					* | Input   | 11 | 10 | 11 | 11 | 10 | 11 | 10 | 10 | W=10, B=11, N=01
					* +---------+----+----+----+----+----+----+----+----+
					* |Even data| 11 |    | 11 |    | 10 |    | 10 |    | = 1111 1010
					* +---------+----+----+----+----+----+----+----+----+
					* |Odd data |    | 10 |    | 11 |    | 11 |    | 10 | = 1011 1110
					* +---------+----+----+----+----+----+----+----+----+ */
					break;				
					case Stage3: // New image
						data_line_odd[x]		 = ((temp_byte & 0x40) ? WHITE3  : BLACK3 );
						data_line_odd[x]		|= ((temp_byte & 0x10) ? WHITE2  : BLACK2 );
						data_line_odd[x]		|= ((temp_byte & 0x04) ? WHITE1  : BLACK1 );
						data_line_odd[x]		|= ((temp_byte & 0x01) ? WHITE0  : BLACK0 );

						data_line_even[k]		 = ((temp_byte & 0x80) ? WHITE0  : BLACK0 );
						data_line_even[k]		|= ((temp_byte & 0x20) ? WHITE1  : BLACK1 );
						data_line_even[k]		|= ((temp_byte & 0x08) ? WHITE2  : BLACK2 );
						data_line_even[k--]		|= ((temp_byte & 0x02) ? WHITE3  : BLACK3 );
					break;
				}
		}	
}


/**
* \brief The base function to handle the driving stages for Frame and Block type
*
* \note
* - There are 3 stages to complete an image update on COG_V230_G2 type EPD.
* - For more details on the driving stages, please refer to the COG G2 document Section 5.4
*
* \param EPD_type_index The defined EPD size
* \param image_ptr The pointer of image array that stores image that will send to COG
* \param image_data_address The address of memory that stores image
* \param stage_no The assigned stage number that will proceed
* \param lineoffset Line data offset
*/
void stage_handle_Base(uint8_t EPD_type_index, const uint8_t *image_ptr,long image_data_address,
						uint8_t stage_no,uint8_t lineoffset)
{	
	struct EPD_V230_G2_Struct S_epd_v230;
	uint16_t cycle,m,i; //m=number of steps
	uint8_t isLastframe = 0;	//If it is the last frame to send Nothing at the fist scan line
	uint8_t isLastBlock=0;		//If the beginning line of block is in active range of EPD
	//uint8_t isScanOn=0;
	long action_block_address;
	uint32_t line_time=8;
	uint8_t byte_array[LINE_BUFFER_DATA_SIZE];
	
	PRINTF(PSTR("stage_handle_Base: 1\n"));
								/** Stage 2: BLACK/WHITE image, Frame type */
	if (stage_no==Stage2)
	{
		for(i=0;i<action__Waveform_param->stage2_cycle;i++)
		{
			same_data_frame (EPD_type_index,ALL_BLACK,action__Waveform_param->stage2_t1);
			same_data_frame (EPD_type_index,ALL_WHITE,action__Waveform_param->stage2_t2);
		}
		return;
	}
	PRINTF(PSTR("stage_handle_Base: 2\n"));
	/** Stage 1 & 3, Block type */
	// The frame/block/step of Stage1 and Stage3 are default the same.
	stage_init(EPD_type_index,
				&S_epd_v230,
				action__Waveform_param->stage1_block1,
				action__Waveform_param->stage1_step1,
				action__Waveform_param->stage1_frame1);
	PRINTF(PSTR("stage_handle_Base: 3\n"));
	 /* Repeat number of frames */
   	 for (cycle = 0; cycle < (S_epd_v230.frame_cycle ); cycle++)
   	 {
	     if (cycle == (S_epd_v230.frame_cycle - 1)) isLastframe = 1;
		 
		 isLastBlock = 0;
		 
		 /* Move number of steps */
	   	 for (m = 0; m < S_epd_v230.number_of_steps; m++)	 
	   	 {
		   	 S_epd_v230.step_y0 = (S_epd_v230.step_size * m) + S_epd_v230.frame_y0;
		   	 S_epd_v230.step_y1 = (S_epd_v230.step_size * (m + 1)) + S_epd_v230.frame_y0;
		   	 S_epd_v230.block_y0 = S_epd_v230.step_y1 - S_epd_v230.block_size;
		   	 S_epd_v230.block_y1 = S_epd_v230.step_y1;
			
			/* reset block_y0=frame_y0 if block is not in active range of EPD */
		   	 if (S_epd_v230.block_y0 < S_epd_v230.frame_y0) S_epd_v230.block_y0 = S_epd_v230.frame_y0;
			
			/* if the beginning line of block is in active range of EPD */
			 if (S_epd_v230.block_y1 == S_epd_v230.block_size) 
				 isLastBlock = 1;
			 	
			const uint8_t *action_block_ptr;
			 if (image_ptr != NULL)
			 {
				 action_block_ptr = (image_ptr + (int) (S_epd_v230.block_y0 * lineoffset));	
			 }
			 else if (_On_EPD_read_flash!=NULL)	//Read line data in range of block, read first
			 {
				action_block_address = image_data_address+(int)(S_epd_v230.block_y0*lineoffset);
				_On_EPD_read_flash(action_block_address,(uint8_t *)&byte_array,
									COG_parameters[EPD_type_index].horizontal_size);
				action_block_ptr=byte_array;
			 }
			
			/* Update line data */
		   	 for (i = S_epd_v230.block_y0; i < S_epd_v230.block_y1; i++)
		   	 {
				if (i >= COG_parameters[EPD_type_index].vertical_size) 
				 break;
				if (  isLastBlock &&(i < (S_epd_v230.step_size + S_epd_v230.block_y0)))//isLastframe &&
				{
				  nothing_line(EPD_type_index);					
				}
				else	 
				{		
				  // Get line data of Stage 1 and 3
				  read_line_data_handle(EPD_type_index,action_block_ptr,stage_no);					
				}
			   		
				if (_On_EPD_read_flash!=NULL)	//Read line data in range of block
				{
					action_block_address += lineoffset;
					_On_EPD_read_flash(action_block_address,(uint8_t *)&byte_array,
					COG_parameters[EPD_type_index].horizontal_size);
					action_block_ptr=byte_array;
				}
				else 	
					action_block_ptr+=lineoffset;
				
				const int16_t scanline_no = (COG_parameters[EPD_type_index].vertical_size-1)-i;
				
				/* Scan byte shift per data line */
				data_line_scan[(scanline_no / 4)] = SCAN_TABLE[(scanline_no % 4)];
			   
				/*  the border uses the internal signal control byte. */
				*data_line_border_byte=0x00;
				   
				/* Sending data */
				epd_spi_send(0x0A, COG_Line.uint8, COG_parameters[EPD_type_index].data_line_size);
				
				 
				/* Turn on Output Enable */
				epd_spi_send_byte(0x02, 0x07);
				   
				data_line_scan[(scanline_no>>2)]=0;

				check_poweroff();
		   	 }
		check_poweroff();
	   	 }
   	 }		
	PRINTF(PSTR("stage_handle_Base: 4\n"));
}

/**
* \brief The driving stages from image array (image_data.h) to COG
*
* \param EPD_type_index The defined EPD size
* \param image_ptr The pointer of image array that stores image that will send to COG
* \param stage_no The assigned stage number that will proceed
* \param lineoffset Line data offset
*/
void stage_handle(uint8_t EPD_type_index, const uint8_t *image_ptr,uint8_t stage_no,uint8_t lineoffset)
{
	stage_handle_Base(EPD_type_index,image_ptr,ADDRESS_NULL,stage_no,lineoffset);	
}

/**
* \brief The driving stages from memory to COG
*
* \note
* - This function is additional added here for developer if the image data
*   is stored in Flash memory.
*
* \param EPD_type_index The defined EPD size
* \param image_data_address The address of flash memory that stores image
* \param stage_no The assigned stage number that will proceed
* \param lineoffset Line data offset
*/
static void stage_handle_ex(uint8_t EPD_type_index,long image_data_address,uint8_t stage_no,uint8_t lineoffset) {
	stage_handle_Base(EPD_type_index,NULL,image_data_address,stage_no,lineoffset);	
}

/**
* \brief Write image data from memory array (image_data.h) to the EPD
*
* \param EPD_type_index The defined EPD size
* \param previous_image_ptr The pointer of memory that stores previous image
* \param new_image_ptr The pointer of memory that stores new image
*/
void EPD_display_from_array_ptr (
	uint8_t EPD_type_index, 
	const uint8_t *previous_image_ptr,
	const uint8_t *new_image_ptr) 
{	
	_On_EPD_read_flash=NULL;		

	PRINTF(PSTR("EPD_display_from_array_ptr: 1\n"));
	stage_handle(EPD_type_index,new_image_ptr,Stage1,COG_parameters[EPD_type_index].horizontal_size);	
	PRINTF(PSTR("EPD_display_from_array_ptr: 2\n"));
	stage_handle(EPD_type_index,new_image_ptr,Stage2,COG_parameters[EPD_type_index].horizontal_size);	
	PRINTF(PSTR("EPD_display_from_array_ptr: 3\n"));
	stage_handle(EPD_type_index,new_image_ptr,Stage3,COG_parameters[EPD_type_index].horizontal_size);	
	PRINTF(PSTR("EPD_display_from_array_ptr: 4\n"));
}

/**
* \brief Write image data from Flash memory to the EPD
* \note This function is additional added here for developer if the image data
* is stored in Flash.
*
* \param EPD_type_index The defined EPD size
* \param previous_image_flash_address The start address of memory that stores previous image
* \param new_image_flash_address The start address of memory that stores new image
* \param On_EPD_read_flash Developer needs to create an external function to read flash
*/
void EPD_display_from_flash_prt (uint8_t EPD_type_index, long previous_image_flash_address,
    long new_image_flash_address,EPD_read_flash_handler On_EPD_read_flash) {
		
	uint8_t line_len=LINE_SIZE;
	if (line_len==0) line_len=COG_parameters[EPD_type_index].horizontal_size;
		
	_On_EPD_read_flash=On_EPD_read_flash;	
	stage_handle_ex(EPD_type_index,new_image_flash_address,Stage1,line_len);
	stage_handle_ex(EPD_type_index,new_image_flash_address,Stage2,line_len);
	stage_handle_ex(EPD_type_index,new_image_flash_address,Stage3,line_len);	
}


/**
* \brief Write Dummy Line to COG
* \note A line whose all Scan Bytes are 0x00
*
* \param EPD_type_index The defined EPD size
*/
static inline void dummy_line(uint8_t EPD_type_index) {
	uint8_t	i;
	for (i = 0; i < (COG_parameters[EPD_type_index].vertical_size/8); i++) {
		switch(EPD_type_index) {
			case EPD_144:
			COG_Line.line_data_by_size.line_data_for_144.scan[i]=0x00;
			break;
			case EPD_200:
			COG_Line.line_data_by_size.line_data_for_200.scan[i]=0x00;
			break;
			case EPD_270:
			COG_Line.line_data_by_size.line_data_for_270.scan[i]=0x00;
			break;
		}
	}
	/* Set charge pump voltage level reduce voltage shift */
	epd_spi_send_byte(0x04, COG_parameters[EPD_type_index].voltage_level);
	
	/* Sending data */
	epd_spi_send (0x0A, (uint8_t *)&COG_Line.uint8, COG_parameters[EPD_type_index].data_line_size);

	/* Turn on Output Enable */
	epd_spi_send_byte(0x02, 0x07);
}


/**
* \brief Write Border(Input) Dummy Line
* \note Set Border byte 0xFF to write Black and set 0xAA to write White
*
* \param EPD_type_index The defined EPD size
*/
static void border_dummy_line(uint8_t EPD_type_index)
{
	uint16_t	i;
	for (i = 0; i < COG_parameters[EPD_type_index].data_line_size; i++)
	{
		COG_Line.uint8[i] = 0x00;
	}
	
	*data_line_border_byte=BORDER_BYTE_B;
	//Write a Border(B) Dummy Line
	epd_spi_send (0x0a, (uint8_t *)&COG_Line.uint8, COG_parameters[EPD_type_index].data_line_size);
	//Turn on OE
	epd_spi_send_byte(0x02, 0x07);
	
	local_delay_ms_spool(40);
	
	*data_line_border_byte=BORDER_BYTE_W;
	//Write a Borde(B) Dummy Line
	epd_spi_send (0x0a, (uint8_t *)&COG_Line.uint8, COG_parameters[EPD_type_index].data_line_size);
	//Turn on OE
	epd_spi_send_byte(0x02, 0x07);

	local_delay_ms_spool(200);
	
	
}


/**
* \brief Power Off COG Driver
* \note For detailed flow and description, please refer to the COG G2 document Section 6.
*
* \param EPD_type_index The defined EPD size
*/
uint8_t EPD_power_off (uint8_t EPD_type_index) {

	if (EPD_type_index==EPD_144 || EPD_type_index==EPD_200) 	{
		border_dummy_line(EPD_type_index);
		dummy_line(EPD_type_index);
	}

	local_delay_ms_spool (25);
	if (EPD_type_index==EPD_270)	{
		EPD_border_low();
		local_delay_ms_spool (200);
		EPD_border_high();
	}
	//Check DC/DC
	if ((SPI_R(0x0F,0x00) & 0x40) == 0x00) return ERROR_DC;
	// Latch reset turn on
	epd_spi_send_byte(0x03, 0x01);
	
	// Output enable off
	epd_spi_send_byte(0x02, 0x05);
	
	// Power off charge pump Vcom
	epd_spi_send_byte(0x05, 0x0E);
	
	// Power off charge negative voltage
	epd_spi_send_byte(0x05, 0x02);
	
	// Discharge
	epd_spi_send_byte(0x04, 0x0C);
	local_delay_ms_spool (120);
	
	// Turn off all charge pumps
	epd_spi_send_byte(0x05, 0x00);
	
	// Turn off osc
	epd_spi_send_byte(0x07, 0x0D);
	
	// Discharge internal
	epd_spi_send_byte(0x04, 0x50);
	local_delay_ms_spool (40);
	
	// Discharge internal
	epd_spi_send_byte(0x04, 0xA0);
	local_delay_ms_spool (40);
	
	// Discharge internal
	epd_spi_send_byte(0x04, 0x00);
	
	// Set power and signals = 0
	EPD_rst_low ();
	////epd_spi_detach ();
	////EPD_cs_low ();
	EPD_Vcc_turn_off ();
	EPD_border_low();
	
	// External discharge = 1
	EPD_discharge_high ();
	local_delay_ms_spool (150);
	
	// External discharge = 0
	EPD_discharge_low ();
	
	return RES_OK;
}

/**
* \brief Power Off COG Driver
* \note For detailed flow and description, please refer to the COG G2 document Section 6.
*
* \param EPD_type_index The defined EPD size
*/
uint8_t EPD_power_off2 (uint8_t EPD_type_index) {
	uint8_t y;		

	if (EPD_type_index==EPD_144 || EPD_type_index==EPD_200) 	{
		border_dummy_line(EPD_type_index);
		dummy_line(EPD_type_index);
	}

	local_delay_ms_spool (25);
	if (EPD_type_index==EPD_270)	{
		EPD_border_low();
		local_delay_ms_spool (200);
		EPD_border_high();
	}

	//Check DC/DC
	if ((SPI_R(0x0F,0x00) & 0x40) == 0x00) return ERROR_DC;
	
	//Turn on Latch Reset
	epd_spi_send_byte(0x03, 0x01);
	//Turn off OE
	epd_spi_send_byte(0x02, 0x05);
	//Power off charge pump Vcom
	epd_spi_send_byte(0x05, 0x03);
	//Power off charge pump neg voltage
	epd_spi_send_byte(0x05, 0x01);
	local_delay_ms_spool(250);
	//Turn off all charge pump 
	epd_spi_send_byte(0x05, 0x00);
	local_delay_ms_spool(300);
	//Turn off OSC
	epd_spi_send_byte(0x07, 0x05);
	
	////epd_spi_detach ();
	////EPD_cs_low();
	EPD_rst_low();
	EPD_Vcc_turn_off ();
	EPD_border_low();
	local_delay_ms_spool (10);
		
	for(y=0;y<10;y++)
	{
		EPD_discharge_high ();		
		local_delay_ms_spool (10);		
		EPD_discharge_low ();	
		local_delay_ms_spool (10);	
	}
	return RES_OK;
}

#endif /* COG_V230_G2 */

/**
 * \brief Initialize the EPD hardware setting 
 */
void EPD_display_init(void) {
	EPD_init();
}

/**
 * \brief Show image from the pointer of memory array
 *
 * \param EPD_type_index The defined EPD size
 * \param previous_image_ptr The pointer of memory that stores previous image
 * \param new_image_ptr The pointer of memory that stores new image
 */
void EPD_display_from_pointer(
	uint8_t EPD_type_index, 
	const uint8_t *previous_image_ptr,
	const uint8_t *new_image_ptr) {
	/* Initialize EPD hardware */
	PRINTF(PSTR("EPD_display_from_pointer: 1\n"));
	EPD_init();
	
	/* Power on COG Driver */
	PRINTF(PSTR("EPD_display_from_pointer: 2\n"));
	EPD_power_on();
	
	/* Initialize COG Driver */
	PRINTF(PSTR("EPD_display_from_pointer: 3\n"));
	EPD_initialize_driver(EPD_type_index);
	
	/* Display image data on EPD from image array */
	PRINTF(PSTR("EPD_display_from_pointer: 4\n"));
	EPD_display_from_array_ptr(EPD_type_index,previous_image_ptr,new_image_ptr);
	
	/* Power off COG Driver */
	PRINTF(PSTR("EPD_display_from_pointer: 5\n"));
	EPD_power_off (EPD_type_index);
	PRINTF(PSTR("EPD_display_from_pointer: 6\n"));
}

/**
 * \brief Show image from Flash memory
 *
 * \param EPD_type_index The defined EPD size
 * \param previous_image_address The address of memory that stores previous image
 * \param new_image_address The address of memory that stores new image
 * \param On_EPD_read_flash Developer needs to create an external function to read flash
 */
void EPD_display_from_flash(uint8_t EPD_type_index,long previous_image_address,
long new_image_address,EPD_read_flash_handler On_EPD_read_flash) {
	/* Initialize EPD hardware */
	EPD_init();
	
	/* Power on COG Driver */
	EPD_power_on();
	
	/* Initialize COG Driver */
	EPD_initialize_driver(EPD_type_index);
	
	/* Display image data on EPD from Flash memory */
	EPD_display_from_flash_prt(EPD_type_index,previous_image_address,
	    new_image_address,On_EPD_read_flash);
	
	/* Power off COG Driver */
	EPD_power_off (EPD_type_index);
}
/**
 * \brief Initialize the EPD hardware setting and COG driver
 *
 * \param EPD_type_index The defined EPD size 
 */
void EPD_power_init(uint8_t EPD_type_index) {
	EPD_init();
	EPD_power_on ();
	EPD_initialize_driver (EPD_type_index);
}

/**
 * \brief Show image from Flash memory when SPI is common used with COG and Flash
 *
 * \note 
 * - This function must work with EPD_power_init when SPI is common used with 
 *   COG and Flash, or the charge pump doesn't work correctly.
 * - EPD_power_init -> write data to flash (switch SPI) -> EPD_display_from_flash_Ex
 *
 * \param EPD_type_index The defined EPD size
 * \param previous_image_address The address of memory that stores previous image
 * \param new_image_address The address of memory that stores new image
 * \param On_EPD_read_flash Developer needs to create an external function to read flash
 */
void EPD_display_from_flash_Ex(uint8_t EPD_type_index,long previous_image_address,
	long new_image_address,EPD_read_flash_handler On_EPD_read_flash) {
	
	/* Display image data on EPD from Flash memory */
	EPD_display_from_flash_prt(EPD_type_index,previous_image_address,
	    new_image_address,On_EPD_read_flash);
	
	/* Power off COG Driver */
	EPD_power_off (EPD_type_index);
}


#endif /* ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED */


#if ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED
// M9546 (PCF8576C) support functions

/**********************************************************************
*
*    Author:		shondll (shondll_yahoo.com)
*    Company:		Olimex Ltd.
*    Date:			07/02/2011
*    File Version:	1.00
*    Description:	LCD 1x9 drtiver
*
**********************************************************************/
//#include "Compiler.h"
//#include "GenericTypeDefs.h"
//#include "LCD1x9.h"
//#include "LCD1x9_mapping.h"

/* DEFINE LOCAL TYPES HERE */

/* DEFINE LOCAL CONSTANTS HERE */
#define LCD1x9_SDA_TRIS TRISCbits.TRISC4
#define LCD1x9_SDA_LAT LATCbits.LATC4
#define LCD1x9_SDA_PORT PORTCbits.RC4

#define LCD1x9_SCL_TRIS TRISCbits.TRISC3
#define LCD1x9_SCL_LAT LATCbits.LATC3
#define LCD1x9_SCL_PORT PORTCbits.RC3

#define LCD1x9_SLAVE_ADDR 0x70

#define ACK 0
#define NACK 1

/**********************************************************************
*
*    Author:		Energy Micro AS
*    Modified for the MOD-LCD1x9 by shondll (shondll_yahoo.com)
*    Company:		Olimex Ltd.
*    Date:		07/02/2011
*    File Version:	1.00
*    Description:	LCD mapping and font
*
**********************************************************************/

/**************************************************************************//**
 * @brief
 * Defines each text symbol's segment in terms of COM and NAND_BIT numbers,
 * in a way that we can enumerate each bit for each text segment in the
 * following bit pattern:
 * @verbatim
 *  -------0------
 *
 * |   \7  |8  /9 |
 * |5   \  |  /   |1
 *
 *  --6---  ---10--
 *
 * |    /  |  \11 |
 * |4  /13 |12 \  |2
 *
 *  -------3------  .(14)
 *  ------15------
 *
 * @endverbatim
 * E.g.: First text character bit pattern #3 (above) is
 *  Segment 1D for Display
 *  Location COM 3, NAND_BIT 0
 *****************************************************************************/
typedef struct
{
  uint8_t com[16]; /**< LCD COM line (for multiplexing) */
  uint8_t bit[16]; /**< LCD bit number */
} CHAR_TypeDef;

/**************************************************************************//**
 * @brief Defines prototype for all segments in display
 *****************************************************************************/
typedef struct
{
  CHAR_TypeDef			Text[9];
} MCU_DISPLAY;

/**************************************************************************//**
 * @brief Working instance of LCD display
 *****************************************************************************/
static const FLASHMEM MCU_DISPLAY LCD1x9 = {
  {
    { /* 1 */
	{       3,       3,       0,       0,       2,       1,       1,       3,       1,       3,       1,       2,       2,       2,       0,       0}, // com
	{  34 - 0,  32 - 0,  32 - 0,  34 - 0,  35 - 0,  35 - 0,  34 - 0,  35 - 0,  33 - 0,  33 - 0,  32 - 0,  32 - 0,  33 - 0,  34 - 0,  33 - 0,  35 - 0}  // bit    
    },
    { /* 2 */
	{       3,       3,       0,       0,       2,       1,       1,       3,       1,       3,       1,       2,       2,       2,       0,       0}, // com
	{  34 - 4,  32 - 4,  32 - 4,  34 - 4,  35 - 4,  35 - 4,  34 - 4,  35 - 4,  33 - 4,  33 - 4,  32 - 4,  32 - 4,  33 - 4,  34 - 4,  33 - 4,  35 - 4}  // bit    
    },
    { /* 3 */
	{       3,       3,       0,       0,       2,       1,       1,       3,       1,       3,       1,       2,       2,       2,       0,       0}, // com
	{  34 - 8,  32 - 8,  32 - 8,  34 - 8,  35 - 8,  35 - 8,  34 - 8,  35 - 8,  33 - 8,  33 - 8,  32 - 8,  32 - 8,  33 - 8,  34 - 8,  33 - 8,  35 - 8}  // bit    
    },
    { /* 4 */
	{       3,       3,       0,       0,       2,       1,       1,       3,       1,       3,       1,       2,       2,       2,       0,       0}, // com
	{ 34 - 12, 32 - 12, 32 - 12, 34 - 12, 35 - 12, 35 - 12, 34 - 12, 35 - 12, 33 - 12, 33 - 12, 32 - 12, 32 - 12, 33 - 12, 34 - 12, 33 - 12, 35 - 12}  // bit    
    },
    { /* 5 */
	{       3,       3,       0,       0,       2,       1,       1,       3,       1,       3,       1,       2,       2,       2,       0,       0}, // com
	{ 34 - 16, 32 - 16, 32 - 16, 34 - 16, 35 - 16, 35 - 16, 34 - 16, 35 - 16, 33 - 16, 33 - 16, 32 - 16, 32 - 16, 33 - 16, 34 - 16, 33 - 16, 35 - 16}  // bit    
    },
    { /* 6 */
	{       3,       3,       0,       0,       2,       1,       1,       3,       1,       3,       1,       2,       2,       2,       0,       0}, // com
	{ 34 - 20, 32 - 20, 32 - 20, 34 - 20, 35 - 20, 35 - 20, 34 - 20, 35 - 20, 33 - 20, 33 - 20, 32 - 20, 32 - 20, 33 - 20, 34 - 20, 33 - 20, 35 - 20}  // bit    
    },
    { /* 7 */
	{       3,       3,       0,       0,       2,       1,       1,       3,       1,       3,       1,       2,       2,       2,       0,       0}, // com
	{ 34 - 24, 32 - 24, 32 - 24, 34 - 24, 35 - 24, 35 - 24, 34 - 24, 35 - 24, 33 - 24, 33 - 24, 32 - 24, 32 - 24, 33 - 24, 34 - 24, 33 - 24, 35 - 24}  // bit    
    },
    { /* 8 */
	{       3,       3,       0,       0,       2,       1,       1,       3,       1,       3,       1,       2,       2,       2,       0,       0}, // com
	{ 34 - 28, 32 - 28, 32 - 28, 34 - 28, 35 - 28, 35 - 28, 34 - 28, 35 - 28, 33 - 28, 33 - 28, 32 - 28, 32 - 28, 33 - 28, 34 - 28, 33 - 28, 35 - 28}  // bit    
    },
    { /* 9 */
	{       3,       3,       0,       0,       2,       1,       1,       3,       1,       3,       1,       2,       2,       2,       0,       0}, // com
	{ 34 - 32, 32 - 32, 32 - 32, 34 - 32, 35 - 32, 35 - 32, 34 - 32, 35 - 32, 33 - 32, 33 - 32, 32 - 32, 32 - 32, 33 - 32, 34 - 32, 33 - 32, 35 - 32}  // bit    
    }
  }
};

/**************************************************************************//**
 * @brief
 * Defines higlighted segments for the alphabet, starting from "blank" (SPACE)
 * Uses bit pattern as defined for text segments above.
 * E.g. a capital O, would have bits 0 1 2 3 4 5 => 0x003f defined
 *****************************************************************************/
static const FLASHMEM uint16_t LCDAlphabet[] = {
  0x0000, /* space */
  0x1100, /* ! */
  0x0280, /* " */
  0x0000, /* # */
  0x0000, /* $ */
  0x0000, /* % */
  0x0000, /* & */
  0x0000, /* ? */
  0x0039, /* ( */
  0x000f, /* ) */
  0x0463, /* * ->> modified to a degree symbol */ 
  0x1540, /* + */
  0x0000, /* , */
  0x0440, /* - */
  0x1000, /* . */
  0x2200, /* / */

  0x003f, /* 0 */
  0x0006, /* 1 */
  0x045b, /* 2 */
  0x044f, /* 3 */
  0x0466, /* 4 */
  0x046d, /* 5 */
  0x047d, /* 6 */
  0x0007, /* 7 */
  0x047f, /* 8 */
  0x046f, /* 9 */

  0x0000, /* : */
  0x0000, /* ; */
  0x0a00, /* < */
  0x0000, /* = */
  0x2080, /* > */
  0x0000, /* ? */
  0xffff, /* @ */

  0x0477, /* A */
  0x0a79, /* B */
  0x0039, /* C */
  0x20b0, /* D */
  0x0079, /* E */
  0x0071, /* F */
  0x047d, /* G */
  0x0476, /* H */
//  0x0006, /* I */
  0x0030, /* I edit */
  0x000e, /* J */
  0x0a70, /* K */
  0x0038, /* L */
  0x02b6, /* M */
  0x08b6, /* N */
  0x003f, /* O */
  0x0473, /* P */
  0x083f, /* Q */
  0x0c73, /* R */
  0x046d, /* S */
  0x1101, /* T */
  0x003e, /* U */
  0x2230, /* V */
  0x2836, /* W */
  0x2a80, /* X */
  0x046e, /* Y */
  0x2209, /* Z */

  0x0039, /* [ */
  0x0880, /* backslash */
  0x000f, /* ] */
  0x0001, /* ^ */
  0x0008, /* _ */
  0x0100, /* ` */

  0x1058, /* a */
  0x047c, /* b */
  0x0058, /* c */
  0x045e, /* d */
  0x2058, /* e */
  0x0471, /* f */
  0x0c0c, /* g */
  0x0474, /* h */
  0x0004, /* i */
  0x000e, /* j */
  0x0c70, /* k */
  0x0038, /* l */
  0x1454, /* m */
  0x0454, /* n */
  0x045c, /* o */
  0x0473, /* p */
  0x0467, /* q */
  0x0450, /* r */
  0x0c08, /* s */
  0x0078, /* t */
  0x001c, /* u */
  0x2010, /* v */
  0x2814, /* w */
  0x2a80, /* x */
  0x080c, /* y */
  0x2048, /* z */

  0x0000,
};


/* DECLARE EXTERNAL VARIABLES HERE */

/* DEFINE LOCAL MACROS HERE */

/* DEFINE LOCAL VARIABLES HERE */
static uint8_t lcdBitmap[20]; // 40segments * 4 = 160px, 160 / 8 = 20bytes

/* DECLARE EXTERNAL VARIABLES HERE */


/* DEFINE FUNCTIONS HERE */

/******************************************************************************
* Description: LCD1x9_Initialize(..) - initializes pins and registers of the LCD1x9
*				Also lights up all segments
* Input: 	none
* Output: 	none
* Return:	0 if sucessfully initialized, -1 if error occured 
*******************************************************************************/
static void LCD1x9_Initialize(void)
{
	int i;

	i2c_start(LCD1x9_SLAVE_ADDR | 0x00);
	i2c_write(0b11001000); // 0xc8 mode register
	i2c_write(0b11110000); // 0xf0 blink register
	i2c_write(0b11100000); // 0xe0 device select register
	i2c_write(0b00000000); // 0x00 pointer register
	
	// light up all the segments, initialize the local display buffer as well
	for(i = 0; i < 20; i++) {
		i2c_write(0x00);
		lcdBitmap[i] = 0; 
	}	
	
	i2c_waitsend();
	i2c_stop();
}


/******************************************************************************
* Description: LCD1x9_enableSegment(..) - enables a segment in the display buffer
*		Note: Does not actually light up the segment, have to call the 'LCD1x9_Update(..)'
* Input: 	comIndex - backplate index
*			bitIndex - segment index
* Output: 	none
* Return:	none
*******************************************************************************/
static void LCD1x9_enableSegment(uint_fast8_t comIndex, uint_fast8_t bitIndex)
{
	if (bitIndex >= 40)
		return;
		
	comIndex &= 0x3;
	
	if (bitIndex & 0x1)
		comIndex |= 0x4;
		
	bitIndex >>= 1;
	
	lcdBitmap[bitIndex] |= 0x80 >> comIndex;
}

/******************************************************************************
* Description: LCD1x9_disableSegment(..) - disables a segment in the display buffer
*		Note: Does not actually lights off the segment, have to call the 'LCD1x9_Update(..)'
* Input: 	comIndex - backplate index
*			bitIndex - segment index
* Output: 	none
* Return:	none
*******************************************************************************/
static void LCD1x9_disableSegment(uint_fast8_t comIndex, uint_fast8_t bitIndex)
{
	if (bitIndex >= 40)
		return;
		
	comIndex &= 0x3;
	
	if (bitIndex & 0x1)
		comIndex |= 0x4;
		
	bitIndex >>= 1;
	
	lcdBitmap[bitIndex] &= ~(0x80 >> comIndex);
}

/******************************************************************************
* Description: LCD1x9_Update(..) - disables a segment in the display buffer
*		Note: Does not actually lights off the segment, have to call the 'LCD1x9_Update(..)'
* Input: 	comIndex - backplate index
*			bitIndex - segment index
* Output: 	none
* Return:	none
*******************************************************************************/
static void LCD1x9_Update(void)
{
	uint_fast8_t i;
	
	i2c_start(LCD1x9_SLAVE_ADDR | 0x00);
	i2c_write(0b11100000); // device select register
	i2c_write(0b00000000); // pointer register
	
	// send the local buffer to the device
	for(i = 0; i < 20; i++)
	{
		check_poweroff();
		i2c_write(lcdBitmap[i]);
	}
		
	i2c_waitsend();
	i2c_stop();
}

/******************************************************************************
* Description: LCD1x9_Write(..) - writes a string to the display
* Input: 	string - the string to write, no more than 9 characters
*			bitIndex - segment index
* Output: 	none
* Return:	none
*******************************************************************************/
static void LCD1x9_Write(const char *string)
{
	uint8_t data, length, index, i;
	uint16_t bitfield;
	uint8_t com, bit;
	
	length = strlen(string);
	if (length > 9)
		return;
	
	index  = 0;
	/* fill out all characters on display */
	for (index = 0; index < 9; index++) {
		if (index < length) {
			data = (uint8_t)string[index];
		} else {
			data = 0x20; // fill with spaces if string is shorter than display
		}

		data -= 0x20;
		bitfield = LCDAlphabet[data];
	
		for (i = 0; i < 16; i++) {
			bit = LCD1x9.Text[index].bit[i];
			com = LCD1x9.Text[index].com[i];
		
			if (bitfield & ((uint16_t)1 << i)) {
				LCD1x9_enableSegment(com, bit);
			} else {
				LCD1x9_disableSegment(com, bit);
			}
		}
	}

	LCD1x9_Update();
}

#define MAXSEGMENT 160
static void LCD1x9_clear(void)
{
	memset(lcdBitmap, 0, sizeof lcdBitmap);
	LCD1x9_Update();
}

static void LCD1x9_seg(int i, uint_fast8_t state)
{
	div_t place = div(i, 4);
	if (state)
		LCD1x9_enableSegment(place.rem, place.quot);
	else
		LCD1x9_disableSegment(place.rem, place.quot);
	LCD1x9_Update();
}

static void lcd_outarray(const uint8_t * a, uint_fast8_t n)
{
	memset(lcdBitmap, 0, sizeof lcdBitmap);
	while (n --)
		LCD1x9_seg(* a ++, 1);
	LCD1x9_Update();
}


static const uint8_t pe2014 [] =
{
	24, 25, 22, 21, 23,	// P
	29, 31, 32, 30, 35,	// E

	56, 57, 53, 54, 59,				// 2
	62, 60, 64, 65, 66, 67,			// 0
	73, 74,							// 1
	76, 77, 81, 82,					// 14

};


#endif /* ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED */

#if 1 && WITHDEBUG && WITHUSEFATFS

struct fb
{
#if _USE_LFN
    char lfn [FF_MAX_LFN + 1];   /* Buffer to store the LFN */
#endif
	FILINFO fno;					/* File information object */
};

void fb_initialize(struct fb * p)
{
	//p->lfnamebuff [0] = '\0';
	//p->fno.lfname = p->lfnamebuff;
    //char *fn;   /* This function is assuming non-Unicode cfg. */
#if 0// _USE_LFN
    ///*static */char lfn[FF_MAX_LFN + 1];   /* Buffer to store the LFN */
    p->fno.lfname = p->lfn;
    p->fno.lfsize = sizeof p->lfn;
#endif
}

static RAMNOINIT_D1 FATFSALIGN_BEGIN uint8_t rbwruff [FF_MAX_SS * 32] FATFSALIGN_END;		// буфер записи - при совпадении с _MAX_SS нельзя располагать в Cortex-M4 CCM


static void showprogress(
	unsigned long filepos,
	unsigned long total
	)
{
}

static void printtextfile(const char * filename)
{
	unsigned long filepos = 0;	// количество выданных байтов
	unsigned long fulllength = 0;	// размер файла
	UINT br = 0;		//  количество считанных байтов
	UINT i = 0;			// номер выводимого байта
	
	FRESULT rc;				/* Result code */
	static RAMNOINIT_D1 FIL Fil;			/* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */
	// чтение файла
	rc = f_open(& Fil, filename, FA_READ);
	if (rc) 
	{
		PRINTF(PSTR("Can not open file '%s'\n"), filename);
		PRINTF(PSTR("Failed with rc=%u.\n"), rc);
		return;
	}
	
	// печать тестового файла
	PRINTF(PSTR("Type the file content: '%s'\n"), filename);
	for (;;)
	{
		char kbch;
#if WITHUSBHW
		board_usbh_polling();     // usb device polling
#endif /* WITHUSBHW */

		if (dbg_getchar(& kbch) != 0)
		{
			if (kbch == 0x1b)
				break;
		}

		if (i >= br)
		{
			// если буфер не заполнен - читаем
			rc = f_read(& Fil, rbwruff, sizeof rbwruff, &br);	/* Read a chunk of file */
			if (rc != FR_OK || !br)
				break;			/* Error or end of file */
			i = 0;		// начальное положение указателя в буфере для вывода данных
			showprogress(filepos, fulllength);
		}
		else if (0)	// "проглатываем" символы без отображения
		{
			++ i;
			showprogress(++ filepos, fulllength);
		}
		else if (dbg_putchar(rbwruff[i]))
		{
			++ i;
			showprogress(++ filepos, fulllength);
		}
	}

	PRINTF(PSTR("read complete: %lu bytes\n"), filepos);

	if (rc) 
	{
		TP();
		PRINTF(PSTR("Failed with rc=%u.\n"), rc);
		return;
	}

	//PRINTF("\nClose the file.\n");
	rc = f_close(& Fil);
	if (rc) 
	{
		TP();
		PRINTF(PSTR("Failed with rc=%u.\n"), rc);
		return;
	}
}

static void rxqclear(void)
{

}

static uint_fast8_t rxqpeek(char * ch)
{
	static int i = ' ';

	* ch = i;
	if (++ i == 0x80)
		i = ' ';
	return 1;
}

// сохранение потока данных с CNC на флэшке
static void dosaveserialport(const char * fname)
{
	static RAMNOINIT_D1 FIL Fil;			/* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */
	unsigned i;
	FRESULT rc;				/* Result code */

	rc = f_open(& Fil, fname, FA_WRITE | FA_CREATE_ALWAYS);
	if (rc)
	{
		PRINTF(PSTR("can not start recording\n"));
		return;	//die(rc);
	}
	PRINTF(PSTR("Write the file content: '%s'\n"), fname);

	rxqclear();	// очистить буфер принятых символов

	unsigned long filepos;	// количество сохранённых байтов
	filepos = 0;
	i = 0;
	showprogress(filepos, 0);
	for (;;)
	{
		char kbch;
		char c;

#if WITHUSBHW
		board_usbh_polling();     // usb device polling
#endif /* WITHUSBHW */
	if (dbg_getchar(& kbch) != 0)
		{
			if (kbch == 0x1b)
			{
				PRINTF(PSTR("break recording\n"));
				break;
			}
		}
		if (rxqpeek(& c) != 0)
		{
			// получен очередной символ из порта - сохраняем в буфер
			rbwruff [i ++] = c;
			showprogress(++ filepos, 0);
			if (i >= (sizeof rbwruff / sizeof rbwruff [0]))
			{
				UINT bw;
				rc = f_write(& Fil, rbwruff, i, & bw);
				if (rc != 0 || bw == 0)
					break;
				i = 0;
			}
		}
	}

	if (i != 0)
	{
		UINT bw;
		rc = f_write(& Fil, rbwruff, i, & bw);
		if (rc != 0 || bw != i)
		{
			TP();
			PRINTF(PSTR("Failed with rc=%u.\n"), rc);
			return;
		}
	}
	rc = f_close(& Fil);
	if (rc) 
	{
		TP();
		PRINTF(PSTR("Failed with rc=%u.\n"), rc);
		return;
	}
}

static volatile unsigned long recticks;
static volatile int recstop;

static void test_recodspool(void * ctx)
{
	if (recticks < NTICKS(60000))
	{
		++ recticks;
	}
	else
	{
		recstop = 1;
	}
}

static void test_recodstart(void)
{
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	recticks = 0;
	recstop = 0;
	LowerIrql(oldIrql);
}

#if 1
/* записать в буфер для ответа 32-бит значение */
unsigned USBD_poke_u32X(uint8_t * buff, uint_fast32_t v)
{
	buff [0] = v >> 0;
	buff [1] = v >> 8;
	buff [2] = v >> 16;
	buff [3] = v >> 24;

	return 4;
}

// сохранение потока данных большими блоками
// 1 - конец циклпа проверок
static int dosaveblocks(const char * fname)
{
	int manualstop = 0;
	unsigned long long kbs = 0;
	static RAMNOINIT_D1 FATFS Fatfs;		/* File system object  - нельзя располагать в Cortex-M4 CCM */
	static RAMNOINIT_D1 FIL Fil;			/* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */
	FRESULT rc;				/* Result code */

	PRINTF(PSTR("FAT FS test - write file '%s'.\n"), fname);
	f_mount(& Fatfs, "", 0);		/* Register volume work area (never fails) */

	/* формирование сигнатуры в данных = для конроля достоверности записи */
	unsigned j;
	static unsigned i;
	static unsigned q;
	memset(rbwruff, 0x20, sizeof rbwruff);
	for (j = 0; j < ARRAY_SIZE(rbwruff) - 16; j += 80)
	{
		USBD_poke_u32_BE(rbwruff + j + 16 - 4, ++ q);
	}
	USBD_poke_u32_BE(rbwruff, ++ i);
	rc = f_open(& Fil, fname, FA_WRITE | FA_CREATE_ALWAYS);
	if (rc)
	{
		PRINTF("can not create file, rc=0x%02X\n", (unsigned) rc);
		return 1;	//die(rc);
	}

#if 1
	//409,337,856
	rc = f_expand(& Fil, 1uLL * 1024 * 1024 * 1024, 0);
	if (rc)
	{
		PRINTF("f_expand: rc=0x%02X\n", (unsigned) rc);
		return 1;	//die(rc);
	}
	else
	{
		//PRINTF("f_expand: rc=0x%02X\n", (unsigned) rc);
	}
#endif

#if 0
	enum { SZ_TBL = 8192 };
	static DWORD clmt [SZ_TBL];                    /* Cluster link map table buffer */
	Fil.cltbl = clmt;
	clmt [0] = SZ_TBL;                      /* Set table size */
	rc = f_lseek(& Fil, CREATE_LINKMAP);     /* Create CLMT */
	if (rc)
	{
		PRINTF("can not set clusters map recording, rc=0x%02X\n", (unsigned) rc);
		return;	//die(rc);
	}
	else
	{
		PRINTF("f_lseek info, clmt [0]=%u\n", (unsigned) clmt [0]);
	}
#endif

	test_recodstart();

	for (;;)
	{
#if WITHUSBHW
		board_usbh_polling();     // usb device polling
#endif /* WITHUSBHW */
		char kbch;
		char c;

		if (recstop != 0)
		{
			PRINTF("end of timed recording\n");
			break;
		}
		if (dbg_getchar(& kbch) != 0)
		{
			if (kbch == 0x1b)
			{
				PRINTF("break recording\n");
				manualstop = 1;
				break;
			}
		}
		UINT bw;
		rc = f_write(& Fil, rbwruff, sizeof rbwruff, & bw);
		if (rc != 0 || bw != sizeof rbwruff)
		{
			PRINTF("write fail, rc=0x%02X\n", (unsigned) rc);
			break;
		}
		kbs += bw;
	}
	rc = f_truncate(& Fil);
	if (rc)
	{
		PRINTF("f_truncate: rc=0x%02X\n", (unsigned) rc);
	}
	rc = f_close(& Fil);
	f_mount(NULL, "", 0);		/* Unregister volume work area (never fails) */
	if (rc)
	{
		TP();
		PRINTF("f_close failed, rc=0x%02X\n", (unsigned) rc);
		return 1;
	}
	else
	{
		PRINTF("Write speed %ld kB/S\n", (long) (kbs / 1000 / 60));
	}
	return manualstop;
}
#endif

#if 0

static const char * fb_getname(const struct fb * p)
{
	const char * fn;
	const FILINFO * pfno = & p->fno;
	#if _USE_LFN
		fn = *pfno->lfname ? pfno->lfname : pfno->fname;
	#else
		fn = pfno->fname;
	#endif

	return fn;
}

static void fb_print(const struct fb * p, int x, int y, int selected)
{
	char buff [100 + FF_MAX_LFN + 1];
	const char * fn = fb_getname(p);
	if (p->fno.fattrib & AM_DIR)
	{
		colmain_setcolors(COLOR_GOLD, selected ? COLOR_BLUE: COLOR_BLACK);
		local_snprintf_P(buff, sizeof buff / sizeof buff [0], "%c   <dir>  %s", selected ? 'X' : ' ',
			fn);
		PRINTF(PSTR("   <dir>  %s\n"), p->fno.fname);
	}
	else
	{
		colmain_setcolors(COLOR_GREEN, selected ? COLOR_BLUE: COLOR_BLACK);
		local_snprintf_P(
			buff,						// куда форматировать строку
			sizeof buff / sizeof buff [0],	// размер буфера
			"%c%8lu  %s", selected ? '>' : ' ',
			p->fno.fsize, fn);
		PRINTF(PSTR("%8lu  %s\n"), p->fno.fsize, p->fno.fname);
	}

	//uint_fast8_t lowhalf = HALFCOUNT_SMALL2 - 1;
	//do
	//{
	//	display_gotoxy(x, y + lowhalf);
	//	display_string2(buff, lowhalf);		// печать буфера - маленьким шрифтом. 0=всегда для данного дисплея
	//} while (lowhalf --);
}
#endif

// Печать имен файлов корневого каталога.
void displfiles_buff(const char* path)
{
    FRESULT res;
    //FILINFO fno;
    DIR dir;
#if _USE_LFN
    ///*static */char lfn[FF_MAX_LFN + 1];   /* Buffer to store the LFN */
    //fno.lfname = lfn;
    //fno.lfsize = sizeof lfn;
#endif

	PRINTF(PSTR("Open root directory.\n"));
    res = f_opendir(& dir, path);                       /* Open the directory */
    if (res == FR_OK) 
	{
		for (;;)
		{
			char *fn;   /* This function is assuming non-Unicode cfg. */
			struct fb fbt;
			FILINFO * pfno = & fbt.fno;
 			fb_initialize(& fbt);		// подготовка к использованию элемента буфера
            res = f_readdir(& dir, pfno);                   /* Read a directory item */
            if (res != FR_OK || pfno->fname[0] == 0) break;  /* Break on error or end of dir */
            if (pfno->fname[0] == '.') continue;             /* Ignore dot entry */
#if 0//_USE_LFN
            fn = *pfno->lfname ? pfno->lfname : pfno->fname;
#else
            fn = pfno->fname;
#endif
            if (pfno->fattrib & AM_DIR)                    /* It is a directory */
			{
                //sprintf(&path[i], "/%s", fn);
                //res = scan_files(path);
                //if (res != FR_OK) break;
                //path[i] = 0;
            } 
			else                                        /* It is a file. */
			{
                //PRINTF("%s/%s\n", path, fn);
 				PRINTF(PSTR("displfiles_buff: %9lu '%s'\n"), (unsigned long) pfno->fsize,  fn);
			}
        }
        //f_closedir(&dir);
    }
}

static BYTE targetdrv = 0;

static char mmcInitialize(void)
{
	DSTATUS st = disk_initialize (targetdrv);				/* Physical drive nmuber (0..) */
	//PRINTF(PSTR("disk_initialize code=%02X\n"), st);
	return st != RES_OK;
}

// read a size Byte big block beginning at the address.
//char mmcReadBlock(uint_fast32_t address, unsigned long count, unsigned char *pBuffer);
static char mmcReadSector(uint_fast32_t sector, unsigned char *pBuffer)
{
	DSTATUS st = disk_read(targetdrv, pBuffer, sector, 1);
	//PRINTF(PSTR("disk_read code=%02X\n"), st);
	return st != RES_OK;
}
//#define 

// write a 512 Byte big block beginning at the (aligned) address
//char mmcWriteBlock (uint_fast32_t address, unsigned long count, const unsigned char *pBuffer);
static char mmcWriteSector(uint_fast32_t sector, const unsigned char *pBuffer)
{
	DSTATUS st = disk_write(targetdrv, pBuffer, sector, 1);
	//PRINTF(PSTR("disk_write code=%02X\n"), st);
	return st != RES_OK;
}

static uint_fast64_t mmcCardSize(void)
{
	DWORD v;
	DSTATUS st = disk_ioctl(targetdrv, GET_SECTOR_COUNT, & v);
	return st != RES_OK ? 0 : (uint_fast64_t) v * MMC_SECTORSIZE;
}

static void diskio_test(void)
{
	const unsigned long MMC_SUCCESS2 = 0x00;
	unsigned long lba_sector = 0;
	static RAMNOINIT_D1 FATFSALIGN_BEGIN unsigned char sectbuffr [MMC_SECTORSIZE] FATFSALIGN_END;
	static RAMNOINIT_D1 FATFSALIGN_BEGIN unsigned char sectbuffw [MMC_SECTORSIZE] FATFSALIGN_END;

	PRINTF(PSTR("Test SD card\n"));
	mmcInitialize();

	PRINTF(PSTR("Enter command:\n"));
//test_disk();
//print_opened_files();
// SD CARD initializations done.
// тест записи/чтения на SD CARD

	static const uint8_t sectproto [MMC_SECTORSIZE] =
	{
			0xEB, 0xFE, 0x90, 0x4D, 0x53, 0x44, 0x4F, 0x53, 0x35, 0x2E, 0x30, 0x00, 0x02, 0x40, 0x86, 0x78,
			0x01, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x3F, 0x00, 0xFF, 0x00, 0x3F, 0x00, 0x00, 0x00,
			0xC1, 0x3F, 0xE7, 0x00, 0x3B, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
			0x01, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x80, 0x00, 0x29, 0x87, 0x4C, 0x4E, 0x57, 0x4E, 0x4F, 0x20, 0x4E, 0x41, 0x4D, 0x45, 0x20, 0x20,
			0x20, 0x20, 0x46, 0x41, 0x54, 0x33, 0x32, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA,

	};
	for (;;)
	{
		static int testdataval;
		int i;

#if WITHUSBHW
		board_usbh_polling();     // usb device polling
#endif /* WITHUSBHW */
		char c;
		if (dbg_getchar(& c))
		{
			switch (c)
			{
			default:
				PRINTF(PSTR("Undefined command letter with code 0x%02X\n"), (unsigned char) c);
				break;

			case 'x':
				PRINTF(PSTR("SD CARD test done.\n"));
				return;

			case '1':
				/* подготовка тестовых данных */
				PRINTF(PSTR("Fill write buffer by 0xff\n"));
				memset(sectbuffw, 0xff, MMC_SECTORSIZE);
				break;

			case '2':
				/* подготовка тестовых данных */
				PRINTF(PSTR("Fill write buffer by 0x00\n"));
				memset(sectbuffw, 0x00, MMC_SECTORSIZE);
				break;

			case '3':
				/* подготовка тестовых данных */
				PRINTF(PSTR("Fill write buffer by 0x55\n"));
				memset(sectbuffw, 0x55, MMC_SECTORSIZE);
				break;

			case '4':
				/* подготовка тестовых данных */
				PRINTF(PSTR("Fill write buffer by 0xaa\n"));
				memset(sectbuffw, 0xaa, MMC_SECTORSIZE);
				break;

			case 'c':
				/* копирование данных */
				PRINTF(PSTR("Copy read buffer to write buffer\n"));
				memcpy(sectbuffw, sectbuffr, MMC_SECTORSIZE);
				break;

			case '5':
				/* подготовка тестовых данных */
				PRINTF(PSTR("Fill write buffer by text data\n"));
				for (i = 0; i < 16; ++ i)
				{
					local_snprintf_P((char *) sectbuffw + i * 32, 32, PSTR("TEST DATA %9d"), testdataval ++);
				}
				local_snprintf_P((char *) sectbuffw, MMC_SECTORSIZE, PSTR("Sector %9lu " __TIME__ " " __DATE__ "  "), lba_sector);
				break;

			case '6':
				/* подготовка даннных как при форматировании FatFS */
				PRINTF(PSTR("Fill write buffer by FatFS data\n"));
				memcpy(sectbuffw, sectproto, MMC_SECTORSIZE);
				break;

			case 'r':
				PRINTF(PSTR("Read SD card, sector = %lu\n"), lba_sector);
				if (mmcReadSector(lba_sector, sectbuffr) != MMC_SUCCESS2)
					PRINTF("Read error\n");
				else
					printhex(0, sectbuffr, MMC_SECTORSIZE);
				break;

			case 'w':
				/* запись тестовых данных */
				PRINTF(PSTR("Write SD card, sector = %lu\n"), lba_sector);
				if (mmcWriteSector(lba_sector, sectbuffw) != MMC_SUCCESS2)
					PRINTF(PSTR("Write error\n"));
				else
					PRINTF(PSTR("Write Okay\n"));
				break;

			case 'v':
				PRINTF(PSTR("Verify SD card, sector = %lu\n"), lba_sector);
				if (mmcReadSector(lba_sector, sectbuffr) != MMC_SUCCESS2)
					PRINTF(PSTR("Read error\n"));
				else if (memcmp(sectbuffr, sectbuffw, MMC_SECTORSIZE) == 0)
					PRINTF(PSTR("No errors\n"));
				else
					PRINTF(PSTR("Different data\n"));
				break;

			case 'n':
				PRINTF(PSTR("Next sector (%lu) on SD card\n"), ++ lba_sector);
				break;

			case 'p':
				PRINTF(PSTR("Previous sector (%lu) on SD card\n"), -- lba_sector);
				break;

			//case 'Q':
			//	PRINTF(PSTR("Wait for ready\n"));
			//	while (SD_ReadCardSize() == 0 && dbg_getchar(& c) == 0)
					;
			case 'q':
				{
					uint_fast64_t v = mmcCardSize();
					PRINTF(PSTR("SD Card size = %lu KB (%lu MB) (%08lx:%08lx bytes)\n"),
						(unsigned long) (v / 1024), 
						(unsigned long) (v / 1024 / 1024), 
						(unsigned long) (v >> 32), 
						(unsigned long) (v >> 0));

				}
				break;
			//case 'D':
			//	PRINTF(PSTR("Wait for ready\n"));
			//	while (mmcCardSize() == 0 && dbg_getchar(& c) == 0)
			//		;
			case 'd':
				{
					uint_fast64_t pos;
					const uint_fast64_t v = mmcCardSize();
					PRINTF(PSTR("Dump SD Card with size = %lu KB (%lu MB) (%08lx:%08lx bytes)\n"),
						(unsigned long) (v / 1024), 
						(unsigned long) (v / 1024 / 1024), 
						(unsigned long) (v >> 32), 
						(unsigned long) (v >> 0));

					for (pos = 0; pos < v; )
					{
						#if WITHUSBHW
								board_usbh_polling();     // usb device polling
						#endif /* WITHUSBHW */
						// проверка прерывания работы с клавиатуры
						char c;
						if (dbg_getchar(& c))
						{
							dbg_putchar(c);
							if (c == 0x1b)
								break;
						}
						// работа
						const unsigned long sector = pos / MMC_SECTORSIZE;
						if (mmcReadSector(sector, sectbuffr) != MMC_SUCCESS2)
						{
							PRINTF(PSTR("Read error\n"));
						}
						else
						{
							printhex(pos, sectbuffr, MMC_SECTORSIZE);
							pos += MMC_SECTORSIZE;
						}
					}
					PRINTF(PSTR("Done dumping.\n"));
				}
				break;

			case 'W':
				{
					unsigned nsect = 1024;
					// Wipe SD
					PRINTF(PSTR("Wipe SD card - first %u sectors. Press 'y' for proceed\n"), nsect);
					char c = 0;
					for (;;)
					{
						if (dbg_getchar(& c))
						{
								break;
						}
					}
					if (c == 'y')
					{
						unsigned sector;

						for (sector = 0; sector < nsect; )
						{
							#if WITHUSBHW
									board_usbh_polling();     // usb device polling
							#endif /* WITHUSBHW */
							// проверка прерывания работы с клавиатуры
							char c;
							if (dbg_getchar(& c))
							{
								dbg_putchar(c);
								if (c == 0x1b)
									break;
							}
							// работа
							if (mmcWriteSector(sector, sectbuffr) != MMC_SUCCESS2)
							{
								PRINTF(PSTR("Write error ar sector %u\n"), sector);
								break;
							}
							else
							{
								++ sector;
							}
						}
						PRINTF(PSTR("Done erasing.\n"));
					}
				}
				break;

			case 'z':
				mmcInitialize();
				PRINTF(PSTR("mmcInitialize.\n"));
				break;
				
			//case 't':
			//	PRINTF("Card %s\n", mmcPing() ? "present" : "missing");
			//	break;
			}
		}
	}
}

static void fatfs_filesystest(int speedtest)
{
	FATFSALIGN_BEGIN BYTE work [FF_MAX_SS] FATFSALIGN_END;
	FRESULT rc;  
	static RAMNOINIT_D1 FATFS Fatfs;		/* File system object  - нельзя располагать в Cortex-M4 CCM */
	static const char testfile [] = "readme.txt";
	char testlog [FF_MAX_LFN + 1];
	//int nlog = 0;

	mmcInitialize();
	PRINTF(PSTR("FAT FS test.\n"));
	f_mount(& Fatfs, "", 0);		/* Register volume work area (never fails) */

	for (;;)
	{
#if WITHUSBHW
		board_usbh_polling();     // usb device polling
#endif /* WITHUSBHW */
		char c;
		if (dbg_getchar(& c))
		{
			switch (c)
			{
			default:
				PRINTF(PSTR("Undefined command letter with code 0x%02X\n"), (unsigned char) c);
				break;

			case 'q':
				{
					uint_fast64_t v = mmcCardSize();
					PRINTF(PSTR("SD Card size = %lu KB (%lu MB) (%08lx:%08lx bytes)\n"),
						(unsigned long) (v / 1024), 
						(unsigned long) (v / 1024 / 1024), 
						(unsigned long) (v >> 32), 
						(unsigned long) (v >> 0));

				}
				break;

			case 'z':
				mmcInitialize();
				break;
				
			case 'x':
				f_mount(NULL, "", 0);		/* Unregister volume work area (never fails) */
				PRINTF(PSTR("FAT FS test done.\n"));
				return;

			case 'd':
				PRINTF(PSTR("FAT FS test - display root directory.\n"));
				f_mount(NULL, "", 0);		/* Unregister volume work area (never fails) */
				f_mount(& Fatfs, "", 0);		/* Register volume work area (never fails) */
				displfiles_buff("");	// Заполнение буфера имён файлов в памяти
				break;

			case 't':
				PRINTF(PSTR("FAT FS test - print file '%s'.\n"), testfile);
				f_mount(NULL, "", 0);		/* Unregister volume work area (never fails) */
				f_mount(& Fatfs, "", 0);		/* Register volume work area (never fails) */
				printtextfile(testfile);
				break;

			case 'F':
				PRINTF(PSTR("FAT FS formatting.\n"));
				f_mount(NULL, "", 0);		/* Unregister volume work area (never fails) */
				rc = f_mkfs("0:", NULL, work, sizeof (work));
				if (rc != FR_OK)
				{
					PRINTF(PSTR("sdcardformat: f_mkfs failure (rc=%d)\n"), rc);
				}
				else
				{
					PRINTF(PSTR("sdcardformat: f_mkfs okay\n"));
				}
				f_mount(& Fatfs, "", 0);		/* Register volume work area (never fails) */
				break;

			case 'w':
				{
					uint_fast16_t year;
					uint_fast8_t month, day;
					uint_fast8_t hour, minute, seconds;
					board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & seconds);
					static unsigned ser;
					local_snprintf_P(testlog, sizeof testlog / sizeof testlog [0],
						PSTR("rec_%04d-%02d-%02d_%02d%02d%02d_%08X_%u.txt"),
						year, month, day,
						hour, minute, seconds,
						(unsigned) hardware_get_random(),
						++ ser
						);
					PRINTF(PSTR("FAT FS test - write file '%s'.\n"), testlog);
					f_mount(NULL, "", 0);		/* Unregister volume work area (never fails) */
					f_mount(& Fatfs, "", 0);		/* Register volume work area (never fails) */
					dosaveserialport(testlog);
				}
				break;
#if 0
			case 'W':
				if (speedtest)
				{
					uint_fast16_t year;
					uint_fast8_t month, day;
					uint_fast8_t hour, minute, seconds;
					board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & seconds);
					static unsigned ser;
					local_snprintf_P(testlog, sizeof testlog / sizeof testlog [0],
						PSTR("rec_%04d-%02d-%02d_%02d%02d%02d_%08lX_%u.txt"),
						year, month, day,
						hour, minute, seconds,
						hardware_get_random(),
						++ ser
						);
					f_mount(NULL, "", 0);		/* Unregister volume work area (never fails) */
					dosaveblocks(testlog);
				}
				break;
#endif
			}
		}
	}
}

#if 1
static int fatfs_filesyspeedstest(void)
{
	uint_fast16_t year;
	uint_fast8_t month, day;
	uint_fast8_t hour, minute, seconds;
	board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & seconds);
	static unsigned ser;
	static const char testfile [] = "readme.txt";
	char testlog [FF_MAX_LFN + 1];
	//int nlog = 0;


	mmcInitialize();
	local_snprintf_P(testlog, sizeof testlog / sizeof testlog [0],
		PSTR("rec_%04d-%02d-%02d_%02d%02d%02d_%08X_%u.txt"),
		year, month, day,
		hour, minute, seconds,
		(unsigned) hardware_get_random(),
		++ ser
		);
	return dosaveblocks(testlog);
}
#endif

#endif /* WITHDEBUG && WITHUSEAUDIOREC */

//HARDWARE_SPI_HANGON()	- поддержка SPI программатора - подключение к программируемому устройству
//HARDWARE_SPI_HANGOFF() - поддержка SPI программатора - отключение от программируемого устройства

#if 0 && defined(targetdataflash)

static void spi_hangon(void)
{
	HARDWARE_SPI_HANGON();
	local_delay_ms(200);
}

static void spi_hangoff(void)
{
	HARDWARE_SPI_HANGOFF();
}

/////////

#define SPIMODE_AT26DF081A	SPIC_MODE3

static unsigned char dataflash_read_status(
	spitarget_t target	/* addressing to chip */
	)
{
	unsigned char v;

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x05);		/* read status register */

	spi_to_read(target);
	v = spi_read_byte(target, 0xff);
	spi_to_write(target);

	spi_unselect(target);	/* done sending data to target chip */

	return v;
}

static int timed_dataflash_read_status(
	spitarget_t target
	)
{
	unsigned long w = 40000;
	while (w --)
	{
		if ((dataflash_read_status(target) & 0x01) == 0)
			return 0;
	}
	PRINTF(PSTR("DATAFLASH timeout error\n"));
	return 1;
}

static int largetimed_dataflash_read_status(
	spitarget_t target
	)
{
	unsigned long w = 40000000;
	while (w --)
	{
		if ((dataflash_read_status(target) & 0x01) == 0)
			return 0;
	}
	PRINTF(PSTR("DATAFLASH erase timeout error\n"));
	return 1;
}

static int testchipDATAFLASH(void)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	unsigned char mf_id;	// Manufacturer ID
	unsigned char mf_devid1;	// device ID (part 1)
	unsigned char mf_devid2;	// device ID (part 2)
	unsigned char mf_dlen;	// Extended Device Information String Length


	/* Ожидание бита ~RDY в слове состояния. Для FRAM не имеет смысла.
	Вставлено для возможности использования DATAFLASH */

	if (timed_dataflash_read_status(target))
		return 1;

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x9f);		/* read id register */

	spi_to_read(target);

	//prog_spi_to_read();
	mf_id = spi_read_byte(target, 0xff);
	mf_devid1 = spi_read_byte(target, 0xff);
	mf_devid2 = spi_read_byte(target, 0xff);
	mf_dlen = spi_read_byte(target, 0xff);

	spi_to_write(target);

	spi_unselect(target);	/* done sending data to target chip */

	PRINTF(PSTR("Read: ID = 0x%02X devId = 0x%02X%02X, mf_dlen=0x%02X\n"), mf_id, mf_devid1, mf_devid2, mf_dlen);
	//PRINTF(PSTR("Need: ID = 0x%02X devId = 0x%02X%02X, mf_dlen=0x%02X\n"), 0x1f, 0x45, 0x01, 0x00);
	return mf_id != 0x1f || mf_devid1 != 0x45 || mf_devid2 != 0x01 || mf_dlen != 0;
}

static int eraseDATAFLASH(void)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x06);		/* write enable */
	spi_unselect(target);	/* done sending data to target chip */

	if (timed_dataflash_read_status(target))
		return 1;

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x60);		/* chip erase */
	spi_unselect(target);	/* done sending data to target chip */

	if (largetimed_dataflash_read_status(target))
		return 1;

	if ((dataflash_read_status(target) & (0x01 << 5)) != 0)	// write error
	{
		PRINTF(PSTR("Erase error\n"));
		return 1;
	}
	return 0;
}

static int prepareDATAFLASH(void)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x06);		/* write enable */
	spi_unselect(target);	/* done sending data to target chip */

	// Write Status Register
	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x01);		/* write status register */
	spi_progval8(target, 0x00);		/* write status register */
	spi_unselect(target);	/* done sending data to target chip */

	return 0;
}

static int writeEnableDATAFLASH(void)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x06);		/* write enable */
	spi_unselect(target);	/* done sending data to target chip */

	return 0;
}

static int writeDisableDATAFLASH(void)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x04);		/* write disable */
	spi_unselect(target);	/* done sending data to target chip */

	return 0;
}


static int writesinglepageDATAFLASH(unsigned long flashoffset, const unsigned char * data, unsigned long len)
{
	spitarget_t target = targetdataflash;	/* addressing to chip */

	//PRINTF(PSTR(" Prog to address %08lX %02X\n"), flashoffset, len);

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x06);		/* write enable */
	spi_unselect(target);	/* done sending data to target chip */

	// start byte programm
	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8_p1(target, 0x02);				/* Page Program */

	spi_progval8_p2(target, flashoffset >> 16);
	spi_progval8_p2(target, flashoffset >> 8);
	spi_progval8_p2(target, flashoffset >> 0);

	while (len --)
		spi_progval8_p2(target, (unsigned char) * data ++);	// data

	spi_complete(target);	/* done sending data to target chip */

	spi_unselect(target);	/* done sending data to target chip */

	/* Ожидание бита ~RDY в слове состояния. Для FRAM не имеет смысла.
	Вставлено для возможности использования DATAFLASH */

	if (timed_dataflash_read_status(target))
		return 1;

	//PRINTF(PSTR("Done programming\n"));
	return 0;
}

static unsigned long ulmin(
	unsigned long a,
	unsigned long b)
{
	return a < b ? a : b;
}

static int writeDATAFLASH(unsigned long flashoffset, const unsigned char * data, unsigned long len)
{
	//PRINTF(PSTR("Write to address %08lX %02X\n"), flashoffset, len);
	while (len != 0)
	{
		unsigned long offset = flashoffset & 0xFF;
		unsigned long part = ulmin(len, ulmin(256, 256 - offset));

		int status = writesinglepageDATAFLASH(flashoffset, data, part);
		if (status != 0)
			return status;
		len -= part;
		flashoffset += part;
		data += part;
	}
	return 0;
}

static int verifyDATAFLASH(unsigned long flashoffset, const unsigned char * data, unsigned long len)
{
	unsigned long count;
	unsigned long err = 0;
	unsigned char v;
	spitarget_t target = targetdataflash;	/* addressing to chip */

	//PRINTF(PSTR("Compare from address %08lX\n"), flashoffset);

	spi_select(target, SPIMODE_AT26DF081A);	/* start sending data to target chip */
	spi_progval8(target, 0x03);		/* sequential read block */

	spi_progval8(target, flashoffset >> 16);
	spi_progval8(target, flashoffset >> 8);
	spi_progval8(target, flashoffset >> 0);

	spi_to_read(target);

	for (count = 0; count < len; ++ count)
	{
		v = spi_read_byte(target, 0xff);
		if (v != data [count])
		{
			PRINTF(PSTR("Data mismatch at %08lx: read=%02x, expected=%02x\n"), flashoffset + count, v, data [count]);
			err = 1;
			break;
		}
	}

	spi_to_write(target);

	spi_unselect(target);	/* done sending data to target chip */

	if (err)
		PRINTF(PSTR("Done compare, have errors\n"));

	return err;
}



/////////
static int
toprintcFLASH(int c)
{
	if (c < 0x20 || c >= 0x7f)
		return '.';
	return c;
}

static int
printhexDATAFLASH(unsigned long voffs, const unsigned char * buff, unsigned long length)
{
	unsigned i, j;
	unsigned rows = (length + 15) / 16;

	for (i = 0; i < rows; ++ i)
	{
		const int trl = ((length - 1) - i * 16) % 16 + 1;
		PRINTF(PSTR("%08lX "), voffs + i * 16);
		for (j = 0; j < trl; ++ j)
			PRINTF(PSTR(" %02X"), buff [i * 16 + j]);

		PRINTF(PSTR("%*s"), (16 - trl) * 3, "");

		PRINTF(PSTR("  "));
		for (j = 0; j < trl; ++ j)
			PRINTF(PSTR("%c"), toprintcFLASH(buff [i * 16 + j]));

		PRINTF(PSTR("\n"));
	}
	return 0;
}

//static unsigned long flashbase = 0x18000000ul;

enum
{
	HSINIT,
	HSPARSEADDR1, HSPARSEADDR2, HSPARSEADDR3, HSPARSEADDR4,
	HSPARSELEN1, HSPARSELEN2,
	HSPARSETYPE1, HSPARSETYPE2,
	HSPARSEDATA1, HSPARSEDATA2,
	HSPARSEBODY1, HSPARSEBODY2,
	HSPARSECKS1, HSPARSECKS2,
};

static int ascii2nibble(int c)
{
	if (isdigit(c))
		return c - '0';
	if (isupper(c))
		return c - 'A' + 10;
	if (islower(c))
		return c - 'a' + 10;
	return 0;
}

static int parsehex(const TCHAR * filename, int (* usedata)(unsigned long addr, const unsigned char * data, unsigned long length))
{
	int hexstate = HSINIT;
	unsigned long rowaddress = 0;
	unsigned long segaddress = 0;
	unsigned long length;
	unsigned long address;
	unsigned long type;
	unsigned long body2, body32;
	unsigned long count;
	unsigned long cksread;
	unsigned long ckscalc;
	unsigned char body [256];

	//unsigned long filepos = 0;	// количество выданных байтов
	UINT br = 0;		//  количество считанных байтов
	UINT i = 0;			// номер выводимого байта
	
	FRESULT rc;				/* Result code */
	static RAMNOINIT_D1 FIL Fil;			/* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */
	// чтение файла
	rc = f_open(& Fil, filename, FA_READ);
	if (rc) 
	{
		PRINTF(PSTR("Can not open file '%s'\n"), filename);
		PRINTF(PSTR("Failed with rc=%u.\n"), rc);
		return 1;
	}
	for (;;)
	{
		if (i >= br)
		{
			// если буфер не заполнен - читаем
			rc = f_read(& Fil, rbwruff, sizeof rbwruff, &br);	/* Read a chunk of file */
			if (rc || !br) 
				break;			/* Error or end of file */
			i = 0;		// начальное положение указателя в буфере для вывода данных
			//showprogress(filepos, fulllength);
		}
		else 
		{
			int c = rbwruff [i ++];
			if (c == EOF)
			{
				if (hexstate != HSINIT)
					PRINTF(PSTR("Incomplete file '%s'\n"), filename);
				break;
			}
			if (c == '\n' || c == '\r')
			{
				if (hexstate != HSINIT)
				{
					PRINTF(PSTR("Incomplete file '%s'\n"), filename);
					break;
				}
				continue;
			}

			switch (hexstate)
			{
			case HSINIT:
				if (c != ':')
				{
					PRINTF(PSTR("Incomplete file '%s'\n"), filename);
					break;
				}
				hexstate = HSPARSELEN1;
				address = 0;
				type = 0;
				count = 0;
				ckscalc = 0;
				length = 0;
				continue;

			case HSPARSELEN1:
				body2 = body2 * 16 + ascii2nibble(c);
				hexstate = HSPARSELEN2;
				continue;

			case HSPARSELEN2:
				body2 = body2 * 16 + ascii2nibble(c);
				ckscalc += (body2 & 0xff);
				length = body2 & 0x00ff;
				hexstate = HSPARSEADDR1;
				continue;

			case HSPARSEADDR1:
				body2 = body2 * 16 + ascii2nibble(c);
				hexstate = HSPARSEADDR2;
				continue;

			case HSPARSEADDR2:
				body2 = body2 * 16 + ascii2nibble(c);
				ckscalc += (body2 & 0xff);
				hexstate = HSPARSEADDR3;
				continue;

			case HSPARSEADDR3:
				body2 = body2 * 16 + ascii2nibble(c);
				hexstate = HSPARSEADDR4;
				continue;

			case HSPARSEADDR4:
				body2 = body2 * 16 + ascii2nibble(c);
				ckscalc += (body2 & 0xff);
				address = body2 & 0xffff;
				hexstate = HSPARSETYPE1;
				continue;

			case HSPARSETYPE1:
				body2 = body2 * 16 + ascii2nibble(c);
				hexstate = HSPARSETYPE2;
				continue;

			case HSPARSETYPE2:
				body2 = body2 * 16 + ascii2nibble(c);
				ckscalc += (body2 & 0xff);
				type = body2 & 0x00ff;

				if (length != 0)
				{
					hexstate = HSPARSEBODY1;
				}
				else
				{
					hexstate = HSPARSECKS1;
				}
				continue;

			case HSPARSEBODY1:
				body2 = body2 * 16 + ascii2nibble(c);
				hexstate = HSPARSEBODY2;
				continue;

			case HSPARSEBODY2:
				body2 = body2 * 16 + ascii2nibble(c);
				ckscalc += (body2 & 0xff);

				if (count < (sizeof body / sizeof body [0]))
					body [count ++] = body2 & 0xff;

				if (count >= length)
				{
					// last byte in body
					hexstate = HSPARSECKS1;
					body32 = body2;
				}
				else
					hexstate = HSPARSEBODY1;
				continue;

			case HSPARSECKS1:
				cksread = cksread * 16 + ascii2nibble(c);
				hexstate = HSPARSECKS2;
				continue;

			case HSPARSECKS2:
				cksread = cksread * 16 + ascii2nibble(c);
				ckscalc += (cksread & 0xff);
				hexstate = HSINIT;

				if ((ckscalc & 0xff) != 0)
				{
					PRINTF(PSTR("Wrong checksum in file '%s'\n"), filename);
					break;
				}
				// Use data
				switch (type)
				{
				case 0:
					// Data
					if (usedata(segaddress + address, body, count))
						break;
					continue;

				case 4:
					// Extended Linear Address
					segaddress = (0xffff & body32) << 16;
					continue;

				case 5:
					// Start Linear Address
					PRINTF(PSTR("Start Linear Address: %08lX\n"), body32);
					continue;

				case 1:
					//printf("End of file record\n");
					continue;

				default:
					PRINTF(PSTR("record with code %02X\n"), (unsigned) type);
					break;
				}
				break;
			}
			break;
		}
	} // for each char in file

	if (rc) 
	{
		rc = f_close(& Fil);
		return 1;
	}

	//PRINTF("\nClose the file.\n");
	rc = f_close(& Fil);
	if (rc) 
	{
		return 1;
	}
	return 0;
}

static void
fatfs_proghexfile(const char * hexfile)
{
	spi_hangon();
	PRINTF(PSTR("SPI FLASH programmer\n"));
	//parsehex(hexfile, printhexDATAFLASH);

	// AT26DF081A chip write
	if (testchipDATAFLASH() == 0)
	{
		//testchipDATAFLASH();
		//parsehex(hexfile, printhexDATAFLASH);

		do 
		{
			PRINTF(PSTR("Prepare...\n"));
			if (prepareDATAFLASH()) break;
			PRINTF(PSTR("Erase...\n"));
			if (eraseDATAFLASH()) break;
			if (writeEnableDATAFLASH()) break;
			if (parsehex(hexfile, writeDATAFLASH)) break;
			PRINTF(PSTR("Programming...\n"));
			if (writeDisableDATAFLASH()) break;
			PRINTF(PSTR("Verify...\n"));
			if (parsehex(hexfile, verifyDATAFLASH)) break;
		} while (0);
		PRINTF(PSTR("SPI FLASH programmer done\n"));
	}
	spi_hangoff();	// после этого ничего не делаем - так как может опять включиться SPI - для работы с SD картой
	for (;;)
		;
}

static void
fatfs_progspi(void)
{
	static RAMNOINIT_D1 FATFS Fatfs;		/* File system object  - нельзя располагать в Cortex-M4 CCM */
	f_mount(& Fatfs, "", 0);		/* Register volume work area (never fails) */
	fatfs_proghexfile("tc1_r7s721_rom.hex");
	f_mount(NULL, "", 0);		/* Unregister volume work area (never fails) */

	for (;;)
		;
}
#endif /* defined(targetdataflash) */

#if 0

// показ цифровых значений параметров.
static void 
display_debug_digit(
	uint_fast32_t value,
	uint_fast8_t width,
	uint_fast8_t comma,
	uint_fast8_t rj
	)
{
	enum { col = 0, row = 0 };
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;

	colmain_setcolors(MNUVALCOLOR, BGCOLOR);
	do
	{
		display_gotoxy(col, row + lowhalf);		// курсор в начало первой строки
		display2_menu_value(value, width, comma, rj, lowhalf);
	} while (lowhalf --);
}

#endif

#if 0 && CPUSTYLE_R7S721

/////////////////////
static void leds(uint_fast8_t v)
{
	i2c_start(0x40);	// CAT9554 address
	i2c_write(0x01);	// #1 - output port address
	i2c_write(0x07 & ~ v);
	i2c_waitsend();
	i2c_stop();
}

static void ledsinit(void)
{
	i2c_start(0x40);	// CAT9554 address
	i2c_write(0x03);	// #3 - Configuration register
	i2c_write(~ 0x07);	// "0" - output, "1" - input
	i2c_waitsend();
	i2c_stop();
}

#endif

#define LED_TARGET_PORT_S(v) do { R7S721_TARGET_PORT_S(7, v); } while (0)
#define LED_TARGET_PORT_C(v) do { R7S721_TARGET_PORT_C(7, v); } while (0)

enum { LEDBIT = 1uL << 1 }; // P7_1
enum { SW1BIT = 1uL << 9 }; // P1_9

#if 0 && WITHDEBUG && WITHUART1HW
// "трассировка" с помощью выдачи на SPI порт информации о сработавших прерываниях
static void test_spi_trace(uint_fast8_t v)
{
	const spitarget_t target = targetctl1;
	//
	spi_select(target, SPIC_MODE3);
	spi_progval8_p1(target, v);
	spi_complete(target);
	spi_unselect(target);
}
//++++++++++++++++

enum { qSZ = 8 };
static volatile uint_fast8_t queue [qSZ];
static volatile uint_fast8_t qp, qg;
static void	qput(uint_fast8_t c)
{
	uint_fast8_t next = (qp + 1) % qSZ;
	if (next != qg)
	{
		queue [qp] = c;
		qp = next;
	}
}

static uint_fast8_t qget(uint_fast8_t * pc)
{
	if (qp != qg)
	{
		* pc = queue [qg];
		qg = (qg + 1) % qSZ;
		return 1;
	}
	return 0;
}

static uint_fast8_t qempty(void)
{
	return qp == qg;
}

// ---------
static volatile uint_fast8_t rxcount, txcount, rxerrcount;

static int cat3_puts_impl_P(const FLASHMEM char * s)
{
	char c;
	while ((c = * s ++) != '\0')
	{
		while (HARDWARE_DEBUGSIRQ_PUTCHAR(c) == 0)
			;
	}
	return 0;
}

/* вызывается из обработчика прерываний */
// произошла потеря символа (символов) при получении данных с CAT компорта
void cat3_rxoverflow(void)
{
	++ rxerrcount;
}

/* вызывается из обработчика прерываний */
void cat3_disconnect(void)
{
}

void cat3_parsechar(uint_fast8_t c)
{
	//while (hardware_usart1_putchar(c) == 0)
	//	;
	//return;
	++ rxcount;
	//test_spi_trace((rxcount & 0x0f) * 16 + (txcount & 0x0f));

	if (c == 0x1b)
	{
		qput('<');
		qput('e');
		qput('s');
		qput('c');
		qput('>');
	}
	else if (c == 0x0d)
	{
		qput('<');
		qput('c');
		qput('r');
		qput('>');
	}
	else if (c == 0x0a)
	{
		qput('<');
		qput('l');
		qput('f');
		qput('>');
	}
	else
		qput(c);
	if (! qempty())
		HARDWARE_DEBUGSIRQ_ENABLETX(1);
}

void cat3_sendchar(void * ctx)
{
	++ txcount;
	//test_spi_trace((rxcount & 0x0f) * 16 + (txcount & 0x0f));

	uint_fast8_t c;
	if (qget(& c))
	{
		HARDWARE_DEBUGSIRQ_TX(ctx, c);
		if (qempty())
			HARDWARE_DEBUGSIRQ_ENABLETX(0);
	}
	else
	{
		HARDWARE_DEBUGSIRQ_ENABLETX(0);
	}
}

static void serial_irq_loopback_test(void)
{
	//test_spi_trace((rxcount & 0x0f) * 16 + (txcount & 0x0f));

	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	HARDWARE_DEBUGSIRQ_INITIALIZE();
	HARDWARE_DEBUGSIRQ_SET_SPEED(DEBUGSPEED);
	HARDWARE_DEBUGSIRQ_ENABLERX(1);
	LowerIrql(oldIrql);
	cat3_puts_impl_P(PSTR("Serial port ECHO test (with IRQ).\r\n"));
	for (;;)
	{
	}

}

#endif /* WITHDEBUG && WITHUART1HW */

// Периодически вызывается в главном цикле
void looptests(void)
{
#if 0 && WITHFQMETER
	{
		const uint32_t v = board_get_fqmeter();
		PRINTF("fq=%08" PRIX32 " (%" PRIu32 ")\n", v, v);
	}
#endif
#if 0
	{
		// Display SOF frequency on USB device
		// Also set:
		//	hpcd_USB_OTG.Init.Sof_enable = DISABLE;
		unsigned v = hamradio_get__getsoffreq();
		PRINTF("SofFreq=%u\n", v);
	}
#endif
#if 0 && defined (KI_LIST) // && WITHKEYBOARD && KEYBOARD_USE_ADC
	{
		// Тестирование АЦП клавиатуры
		static const uint8_t adcinputs [] =
		{
			KI_LIST
		};
		unsigned i;

		for (i = 0; i < ARRAY_SIZE(adcinputs); ++ i)
		{
			const uint_fast8_t adci = adcinputs [i];

			PRINTF("%2u adci%2d %08lX\n", i, (unsigned) adci, (unsigned long) board_getadc_unfiltered_1 /* true */value(adci));
		}
	}
#endif
#if 0
	{
		// вычисления с плавающей точкой
		//
		//				   1.4142135623730950488016887242096981L
		// #define M_SQRT2  1.41421356237309504880

		//original: sqrt(2)=1.41421356237309514547462
		//double:   sqrt(2)=1.41421356237309514547462
		//float:    sqrt(2)=1.41421353816986083984375
		char b [64];

		//snprintf(b, sizeof b / sizeof b [0], "%u\n", (unsigned) SCB_GetFPUType());
		//PRINTF(PSTR("SCB_GetFPUType: %s"), b);

		snprintf(b, sizeof b / sizeof b [0], "sqrt(2)=%1.23f\n", (double) 1.41421356237309504880);
		PRINTF(PSTR("original: %s"), b);

		volatile double d0 = 2;
		volatile double d = sqrt(d0);
		snprintf(b, sizeof b / sizeof b [0], "sqrt(2)=%1.23f\n", d);
		PRINTF(PSTR("double:   %s"), b);

		volatile float f0 = 2;
		volatile float f = sqrtf(f0);
		snprintf(b, sizeof b / sizeof b [0], "sqrt(2)=%1.23f\n", f);
		PRINTF(PSTR("float:    %s"), b);
	}
#endif
#if 0
	// Failt handlers test
	// Data abort test
	* (int volatile *) 0x00000100 = 44;

	// Preefetch abort test
	typedef void (* pfn)(void);
	const pfn p = (pfn) 0x30000000uL;
	(p)();

#endif
#if WITHDTMFPROCESSING
	dtmftest();
#endif
#if 0 && WITHINTEGRATEDDSP && WITHDEBUG
	{
		dsp_speed_diagnostics();	// печать в последовательный порт результатов диагностики
	}
#endif
#if 1 && WITHINTEGRATEDDSP && WITHDEBUG
	{
		// See buffers.c - WITHBUFFERSDEBUG
		buffers_diagnostics();
	}
#endif
#if 0 && WITHCURRLEVEL
	{
		// Тестирование датчика тока
		(void) hamradio_get_pacurrent_value();
	}
#endif
#if 0 && WITHVOLTLEVEL
	{
		// Тестирование датчика напряжния
		(void) hamradio_get_volt_value();
	}
#endif
#if 0 && WITHTEMPSENSOR
	{
		// Тестирование датчика температуры
		// TODO: пока не показывает температуру!
		const uint_fast8_t tempi = TEMPIX;
		const adcvalholder_t v = board_getadc_unfiltered_1 /* true */value(tempi);
		// Измерение опрного напряжения
		const uint_fast8_t vrefi = VREFIX;
		const adcvalholder_t ref = board_getadc_unfiltered_1 /* true */value(vrefi);	// текущее значение данного АЦП
		if (ref != 0)
		{
			const long Vref_mV = (uint_fast32_t) board_getadc_fsval(vrefi) * WITHREFSENSORVAL / ref;
			// Temperature (in °C) = {(V25 - VSENSE) / Avg_Slope} + 25.
			// Average slope = 4.3
			// Voltage at 25 °C = 1.43 V
			const long celsius = (1430 - (v * Vref_mV / board_getadc_fsval(tempi))) / 430 + 2500;
			PRINTF(PSTR("celsius=%3ld.%02ld\n"), celsius / 100, celsius % 100);
		}
	}
#endif
}

#if 0 && CTLSTYLE_V1V
// "прерыватель"

static void showstate(
	unsigned offtime,
	unsigned ontime
	)
{
	char buff [32];


	local_snprintf_P(buff, 32, PSTR(" ON: %4u0 mS"), ontime);
	display_gotoxy(0, 0);
	display_at(buff, 0);


	local_snprintf_P(buff, 32, PSTR("OFF: %4u0 mS"), offtime);
	display_gotoxy(0, 1);
	display_at(buff, 0);

}


static unsigned modif (unsigned v, unsigned pos, unsigned dd, unsigned width)
{
	unsigned d = (dd % width);
	unsigned leftpos = pos * width;
	unsigned left = v / leftpos;
	unsigned right = v % pos;
	return left * leftpos + (((v / pos) + d) % width) * pos + right;

}

#endif /* 1 && CTLSTYLE_V1V */

#if 0

// PG7: fpga ok
// PG6: sys ok
// PG3: dsp ok
// PG2: res ok
enum { MFPGA = 1ul << 7 };
enum { MSYS = 1ul << 6 };
enum { MDSP = 1ul << 3 };
enum { MRES = 1ul << 2 };

static void sdfault(void)
{
	PRINTF(PSTR("sdfault\n"));
	arm_hardware_piog_outputs(MRES, MRES);
	for (;;)
		;
}

static void sdtick(void)
{
	GPIOG->ODR ^= MDSP;	// Debug LEDs
}
#endif

#if LCDMODE_COLORED && ! DSTYLE_G_DUMMY



// Рисуем на основном экране цветной прямоугольник.
// x2, y2 - координаты второго угла (не входящие в закрашиваемый прямоугольник)
static void display_solidbar(
	uint_fast16_t x,
	uint_fast16_t y,
	uint_fast16_t x2,
	uint_fast16_t y2,
	COLORPIP_T color
	)
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
	display_fillrect(x, y, x2 - x, y2 - y, color);
}


// Получение псевдослучайныз чисел.
// 0 .. num-1
static int local_randomgr(unsigned long num)
{

	static unsigned long rand_val = 123456uL;

	if (rand_val & 0x80000000UL)
		rand_val = (rand_val << 1);
	else	rand_val = (rand_val << 1) ^ 0x201051uL;

	return (rand_val % num);

}


/*                                                                      */
/*      RANDOMBARS: Display random bars                                 */
/*                                                                      */

static void BarTest(void)
{
	//PRINTF("BarTest\n");
	int forever = 0;
	unsigned n = 20000;
	for (;forever || n --;)
	{                    /* Until user enters a key...   */
		const int r = local_randomgr(256);
		const int g = local_randomgr(256);
		const int b = local_randomgr(256);

		const COLORPIP_T color = TFTRGB(r, g, b);

		int x = local_randomgr(DIM_X);
		int y = local_randomgr(DIM_Y);
		int x2 = local_randomgr(DIM_X);
		int y2 = local_randomgr(DIM_Y);

		display_solidbar(x, y, x2, y2, color);	// MDMA работает минуя кеш-память

		display_flush();
		//local_delay_ms(5);
	}

	//getch();             /* Pause for user's response    */
}

static  void
GridTest(void)
{
	PRINTF("GridTest\n");
	board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
	board_update();

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


	xm = DIM_X - 1;
	ym = DIM_Y - 1;
	xm4 = xm / 4;
	ym4 = ym / 4;
	xm1 = xm / 40;
	ym1 = ym / 40;

	/* Filled rectangle - all screen. */
	display_solidbar(0, 0, xm, ym, col1);
	/* Filled rectangle at right-down corner. */
	display_solidbar(xm4 * 3 + xm1, ym4 * 3 + ym1, xm4 * 4 - xm1, ym4 * 4 - ym1, col20);
	/* Filled rectangle at right-upper corner. */
	display_solidbar(xm4 * 3 + xm1, ym1, xm4 * 4 - xm1, ym4 - ym1, col21);
	/* Filled rectangle at left - down corner. */
	display_solidbar(xm1, ym4 * 3 + ym1, xm4 - xm1, ym4 * 4 - ym1, col22);
	/* Filled rectangle at center. */
	display_solidbar(xm4 + xm1, ym4 + ym1, xm4 * 3 - xm1, ym4 * 3 - ym1, col23);


	// Тест порядка цветов в пикселе
	const unsigned yrct0 = DIM_Y / 4;
	const unsigned xrct0 = DIM_X / 4;
	display_fillrect(xrct0, yrct0 * 1, xrct0, yrct0, COLORMAIN_RED);
	display_fillrect(xrct0, yrct0 * 2, xrct0, yrct0, COLORMAIN_GREEN);
	display_fillrect(xrct0, yrct0 * 3, xrct0, yrct0, COLORMAIN_BLUE);
/*
	const unsigned yg0 = DIM_Y / 24;
	const unsigned xg0 = DIM_X / 30;
	for (k = 0; k < 16; ++ k)
		for (n = 0; n < 16; ++ n)
			display_solidbar(n * xg0,
				 k * yg0,
				 n * xg0 + xg0,
				 k * yg0 + yg0,
				 TFTRGB(n * 16, k * 16, 255 - (n * 8 + k * 8) )
				 );
*/

	display_fillrect(xm * 4 / 10, 0, xm * 3 / 10, ym * 2 / 10, COLORMAIN_WHITE);
	display_line(xm * 6 / 10,  0, xm * 6 / 10, ym,  COLORMAIN_RED);

	/* Interlase clocke test.	*/
	display_line(10,  0,  xm, 10 + 1,  col3);
	display_line(10,  0,  xm, 10 + 3,  col3);
	display_line(10,  0,  xm, 10 + 5,  col3);
	display_line(10,  0,  xm, 10 + 7,  col3);

	/* diagonales test.	*/
	display_line(xm, 0,  xm, ym, col3);
	display_line(xm, ym, 0,  ym, col3);
	display_line(0,  ym, 0,  0,  col3);
	display_line(0,  0,  xm, ym, col3);
	display_line(0,  ym, xm, 0,  col3);

	// тест перестановки байтов при выборке видеоконтроллером
	const unsigned rctx = DIM_X / 3;
	const unsigned rcty = DIM_Y / 3;
	display_line(rctx, rcty,  rctx * 2 - 1, rcty * 2 - 1, COLORMAIN_BLACK);
	display_line(rctx, rcty * 2 - 1, rctx * 2 - 1,  rcty, COLORMAIN_BLACK);

	display_flush();

	for (;;)
		;
}

#endif /* LCDMODE_COLORED && ! DSTYLE_G_DUMMY */

#if 0
// MCU_AHB_SRAM - 96k

static void memfill(unsigned k)
{
	unsigned long offset = k * 1024uL;

	((volatile uint8_t *) 0x20000000) [offset + 4] = 0xAA;
	((volatile uint8_t *) 0x20000000) [offset + 5] = 0x55;
	((volatile uint32_t *) 0x20000000) [offset + 0] = offset;
}

static int memprobe(unsigned k)
{
	unsigned long offset = k * 1024uL;

	const uint_fast8_t ok1 = ((volatile uint8_t *) 0x20000000) [offset + 4] == 0xAA;
	const uint_fast8_t ok2 = ((volatile uint8_t *) 0x20000000) [offset + 5] == 0x55;
	const uint_fast8_t ok3 = ((volatile uint32_t *) 0x20000000) [offset + 0] == offset;
	return ok1 && ok2 && ok3;
}
#endif


#if 0 && WITHDEBUG
// FPU speed test

#define FFTZS 8192
typedef struct
{
	float r;
	float i;
} cplxf;

static RAMFRAMEBUFF ALIGNX_BEGIN cplxf src [FFTZS] ALIGNX_END;
static RAMFRAMEBUFF ALIGNX_BEGIN cplxf dst [FFTZS] ALIGNX_END;
static RAMDTCM ALIGNX_BEGIN cplxf refv [FFTZS] ALIGNX_END;

static void RAMFUNC_NONILINE cplxmla(cplxf *s, cplxf *d, cplxf *ref, int len) {
	while (len--) {
		d->r += s->r * ref->r - s->i * ref->i;
		d->i += s->i * ref->r + s->r * ref->i;
		++s;  ++d; ++ref;
	}
}

static void RAMFUNC_NONILINE cplxmlafast(cplxf *s, cplxf *d, cplxf *ref, int len) {
	int i;
	for (i = 0; i < len; ++ i) {
		d [i].r += s [i].r * ref [i].r - s [i].i * ref [i].i;
		d [i].i += s [i].i * ref [i].r + s [i].r * ref [i].i;
	}
}

static void RAMFUNC_NONILINE cplxmlasave(cplxf *d, int len) {
	while (len--) {
		volatile float t;
		t = d->r;
		t = d->i;
	}
}

#endif

#if defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U)

static void disableAllIRQs(void)
{
	IRQ_Disable(43);	// DMA1_Stream0_IRQn
	IRQ_Disable(47);	// DMA1_Stream4_IRQn
	IRQ_Disable(89);	// DMA2_Stream1_IRQn
	IRQ_Disable(100);	// DMA2_Stream5_IRQn
//	IRQ_Disable(106);	// USBH_OHCI_IRQn
//	IRQ_Disable(107);	// USBH_EHCI_IRQn
	IRQ_Disable(82);	// TIM5_IRQn systick
//	IRQ_Disable(61);	// TIM3_IRQn elkey
	IRQ_Disable(99);	// EXTI9_IRQn
	IRQ_Disable(109);	// EXTI13_IRQn
	IRQ_Disable(50);	// ADC1_IRQn
	IRQ_Disable(122);	// ADC2_IRQn
	IRQ_Disable(130);	// OTG_IRQn

	// Get ITLinesNumber
	const unsigned n = ((GIC_DistributorInfo() & 0x1f) + 1) * 32;
	unsigned i;
	// 32 - skip SGI handlers (keep enabled for CPU1 start).
	for (i = 32; i < n; ++ i)
	{
		if (IRQ_GetEnableState(i))
			PRINTF("disableAllIRQs: active=%u // IRQ_Disable(%u); \n", i, i);
		IRQ_Disable(i);
	}
	PRINTF("disableAllIRQs: n=%u\n", n);

}

#endif /* defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U) */

#if WITHOPENVG
/*------------------------------------------------------------------------
 *
 * OpenVG 1.0.1 Reference Implementation sample code
 * -------------------------------------------------
 *
 * Copyright (c) 2007 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and /or associated documentation files
 * (the "Materials "), to deal in the Materials without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Materials,
 * and to permit persons to whom the Materials are furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE MATERIALS OR
 * THE USE OR OTHER DEALINGS IN THE MATERIALS.
 *
 *//**
 * \file
 * \brief	Tiger sample application. Resizing the application window
 *			rerenders the tiger in the new resolution. Pressing 1,2,3
 *			or 4 sets pixel zoom factor, mouse moves inside the zoomed
 *			image (mouse move works on OpenGL >= 1.2).
 * \note
 *//*-------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#define UNREF(X) ((void)(X))

#ifdef HG_FLAT_INCLUDES
#	include "openvg.h"
#	include "vgu.h"
//#	include "egl.h"
#else
#	include "VG/openvg.h"
#	include "VG/vgu.h"
//#	include "EGL/egl.h"
#endif


//#if WITHOPENVG && 1

#include "tiger.h"

/*--------------------------------------------------------------*/

//static const float			aspectRatio = 612.0f / 792.0f;

/*--------------------------------------------------------------*/

typedef struct
{
	VGFillRule		m_fillRule;
	VGPaintMode		m_paintMode;
	VGCapStyle		m_capStyle;
	VGJoinStyle		m_joinStyle;
	float			m_miterLimit;
	float			m_strokeWidth;
	VGPaint			m_fillPaint;
	VGPaint			m_strokePaint;
	VGPath			m_path;
} PathData;

typedef struct
{
	PathData*			m_paths;
	int					m_numPaths;
} PS;

static PS* PS_construct(const char* commands, int commandCount, const float* points, int pointCount)
{
	PS* ps = (PS*)malloc(sizeof(PS));
	int p = 0;
	int c = 0;
	int i = 0;
	int paths = 0;
	int maxElements = 0;
	unsigned char* cmd;
	UNREF(pointCount);

	while(c < commandCount)
	{
		int elements, e;
		c += 4;
		p += 8;
		elements = (int)points[p++];
		ASSERT(elements > 0);
		if(elements > maxElements)
			maxElements = elements;
		for(e=0;e<elements;e++)
		{
			switch(commands[c])
			{
			case 'M': p += 2; break;
			case 'L': p += 2; break;
			case 'C': p += 6; break;
			case 'E': break;
			default:
				ASSERT(0);		//unknown command
			}
			c++;
		}
		paths++;
	}

	ps->m_numPaths = paths;
	ps->m_paths = (PathData*)malloc(paths * sizeof(PathData));
	cmd = (unsigned char*)malloc(maxElements);

	i = 0;
	p = 0;
	c = 0;
	while(c < commandCount)
	{
		int elements, startp, e;
		float color[4];

		//fill type
		int paintMode = 0;
		ps->m_paths[i].m_fillRule = VG_NON_ZERO;
		switch( commands[c] )
		{
		case 'N':
			break;
		case 'F':
			ps->m_paths[i].m_fillRule = VG_NON_ZERO;
			paintMode |= VG_FILL_PATH;
			break;
		case 'E':
			ps->m_paths[i].m_fillRule = VG_EVEN_ODD;
			paintMode |= VG_FILL_PATH;
			break;
		default:
			ASSERT(0);		//unknown command
		}
		c++;

		//stroke
		switch( commands[c] )
		{
		case 'N':
			break;
		case 'S':
			paintMode |= VG_STROKE_PATH;
			break;
		default:
			ASSERT(0);		//unknown command
		}
		ps->m_paths[i].m_paintMode = (VGPaintMode)paintMode;
		c++;

		//line cap
		switch( commands[c] )
		{
		case 'B':
			ps->m_paths[i].m_capStyle = VG_CAP_BUTT;
			break;
		case 'R':
			ps->m_paths[i].m_capStyle = VG_CAP_ROUND;
			break;
		case 'S':
			ps->m_paths[i].m_capStyle = VG_CAP_SQUARE;
			break;
		default:
			ASSERT(0);		//unknown command
		}
		c++;

		//line join
		switch( commands[c] )
		{
		case 'M':
			ps->m_paths[i].m_joinStyle = VG_JOIN_MITER;
			break;
		case 'R':
			ps->m_paths[i].m_joinStyle = VG_JOIN_ROUND;
			break;
		case 'B':
			ps->m_paths[i].m_joinStyle = VG_JOIN_BEVEL;
			break;
		default:
			ASSERT(0);		//unknown command
		}
		c++;

		//the rest of stroke attributes
		ps->m_paths[i].m_miterLimit = points[p++];
		ps->m_paths[i].m_strokeWidth = points[p++];

		//paints
		color[0] = points[p++];
		color[1] = points[p++];
		color[2] = points[p++];
		color[3] = 1.0f;
		ps->m_paths[i].m_strokePaint = vgCreatePaint();
		vgSetParameteri(ps->m_paths[i].m_strokePaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
		vgSetParameterfv(ps->m_paths[i].m_strokePaint, VG_PAINT_COLOR, 4, color);

		color[0] = points[p++];
		color[1] = points[p++];
		color[2] = points[p++];
		color[3] = 1.0f;
		ps->m_paths[i].m_fillPaint = vgCreatePaint();
		vgSetParameteri(ps->m_paths[i].m_fillPaint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
		vgSetParameterfv(ps->m_paths[i].m_fillPaint, VG_PAINT_COLOR, 4, color);

		//read number of elements

		elements = (int)points[p++];
		ASSERT(elements > 0);
		startp = p;
		for(e=0;e<elements;e++)
		{
			switch( commands[c] )
			{
			case 'M':
				cmd[e] = VG_MOVE_TO | VG_ABSOLUTE;
				p += 2;
				break;
			case 'L':
				cmd[e] = VG_LINE_TO | VG_ABSOLUTE;
				p += 2;
				break;
			case 'C':
				cmd[e] = VG_CUBIC_TO | VG_ABSOLUTE;
				p += 6;
				break;
			case 'E':
				cmd[e] = VG_CLOSE_PATH;
				break;
			default:
				ASSERT(0);		//unknown command
			}
			c++;
		}

		ps->m_paths[i].m_path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, (unsigned int)VG_PATH_CAPABILITY_ALL);
		vgAppendPathData(ps->m_paths[i].m_path, elements, cmd, points + startp);
		i++;
	}
	free(cmd);
	return ps;
}

static void PS_destruct(PS* ps)
{
	int i;
	ASSERT(ps);
	for(i=0;i<ps->m_numPaths;i++)
	{
		vgDestroyPaint(ps->m_paths[i].m_fillPaint);
		vgDestroyPaint(ps->m_paths[i].m_strokePaint);
		vgDestroyPath(ps->m_paths[i].m_path);
	}
	free(ps->m_paths);
	free(ps);
}

static void PS_render(PS* ps)
{
	int i;
	ASSERT(ps);
	vgSeti(VG_BLEND_MODE, VG_BLEND_SRC_OVER);

	for(i=0;i<ps->m_numPaths;i++)
	{
		vgSeti(VG_FILL_RULE, ps->m_paths[i].m_fillRule);
		vgSetPaint(ps->m_paths[i].m_fillPaint, VG_FILL_PATH);

		if(ps->m_paths[i].m_paintMode & VG_STROKE_PATH)
		{
			vgSetf(VG_STROKE_LINE_WIDTH, ps->m_paths[i].m_strokeWidth);
			vgSeti(VG_STROKE_CAP_STYLE, ps->m_paths[i].m_capStyle);
			vgSeti(VG_STROKE_JOIN_STYLE, ps->m_paths[i].m_joinStyle);
			vgSetf(VG_STROKE_MITER_LIMIT, ps->m_paths[i].m_miterLimit);
			vgSetPaint(ps->m_paths[i].m_strokePaint, VG_STROKE_PATH);
		}

		vgDrawPath(ps->m_paths[i].m_path, ps->m_paths[i].m_paintMode);
	}
	ASSERT(vgGetError() == VG_NO_ERROR);
}

/*--------------------------------------------------------------*/

static void rendertiger(PS* const tiger, int w, int h)
{
	float clearColor[4] = {1,1,1,1};
	float scaleX = w / (tigerMaxX - tigerMinX);
	float scaleY = h / (tigerMaxY - tigerMinY);
	float scale = fminf(scaleX, scaleY);

	//vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_BETTER);
	//vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_NONANTIALIASED);

	vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
	ASSERT(vgGetError() == VG_NO_ERROR);
	vgClear(0, 0, w, h);
	ASSERT(vgGetError() == VG_NO_ERROR);

	// normal on Window (top-down mirror on Storch)
//	vgLoadIdentity();
//	vgScale(scale, scale);
//	vgTranslate(-tigerMinX, -tigerMinY + 0.5f * (h / scale - (tigerMaxY - tigerMinY)));	// all parameters are zeroes

	// top-down mirror
	vgLoadIdentity();
	vgScale(scale, -scale);
	vgTranslate(-tigerMinX, - (tigerMaxY - tigerMinY));
	ASSERT(vgGetError() == VG_NO_ERROR);

	PS_render(tiger);
	ASSERT(vgGetError() == VG_NO_ERROR);

}

//#endif /* WITHOPENVG */
/*--------------------------------------------------------------*/

// See https://github.com/Ajou-Khronies/OpenVG_tutorital_examples/blob/14d30f9a26cb5ed70ccb136bef7b229c8a51c444/samples/Chapter13/Sample_13_03/Sample_13_03.c
#if 0
{
    VGPath path;

    VGfloat clear[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    vgSetfv( VG_CLEAR_COLOR, 4, clear );
    vgClear( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );

    path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1, 0, 0, 0, VG_PATH_CAPABILITY_ALL );

    vguRect( path, 40.5f, 40.5f, 160.0f, 100.0f );
    vguRect( path, 40.0f, 180.0f, 160.0f, 100.0f );

    vgDrawPath( path, VG_STROKE_PATH );

    vgDestroyPath( path );
}
#endif

static void rendertest1(int w, int h)
{
	//		float scaleX = w / (tigerMaxX - tigerMinX);
	//		float scaleY = h / (tigerMaxY - tigerMinY);
	//		float scale = fminf(scaleX, scaleY);
	//		PRINTF("render: scaleX=%f, scaleY=%f\n", scaleX, scaleY);

	vgLoadIdentity();

	vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_BETTER);
	//vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_NONANTIALIASED);
	//vgSeti(VG_FILL_RULE, VG_NON_ZERO);

	vgSeti(VG_PIXEL_LAYOUT, VG_PIXEL_LAYOUT_RGB_HORIZONTAL);	// для работы антииалиасинга треьуется знать расположение пикселей
	//vgSeti(VG_SCREEN_LAYOUT, );

	VGPath path;
    VGPaint fillPaint, strokePaint;
    static const VGubyte segments[] = { VG_MOVE_TO_ABS, VG_LINE_TO_ABS, VG_LINE_TO_ABS,
                           VG_LINE_TO_ABS, VG_LINE_TO_ABS, VG_CLOSE_PATH };
    static const VGfloat coords [] = { 120.0f, 260.0f, 61.2f, 79.1f, 215.1f, 190.9f, 24.8f, 190.9f, 178.8f, 79.1f };
    static const VGfloat clearColor [4] = { 1.0f, 0.0f, 0.0f, 1.0f };
    static const VGfloat fillColor [4] = { 0.0f, 1.0f, 0.0f, 1.0f };
    static const VGfloat strokeColor [4] = { 0.0f, 0.0f, 1.0f, 1.0f };

    // top-down mirror and back...
	vgLoadIdentity();
//	vgScale(1, -1);
//	vgScale(1, -1);
//	vgTranslate(0, - h);
//	vgTranslate(0, + h);

    vgSetfv( VG_CLEAR_COLOR, 4, clearColor );
    vgClear( 0, 0, w, h );
#if 1

    vgSeti( VG_STROKE_LINE_WIDTH, 3 );		// толщина лини
    fillPaint = vgCreatePaint();
    strokePaint = vgCreatePaint();

    path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);

    // 1) перечисляем (добавляем) фигуры
    vgAppendPathData( path, 6, segments, coords );
	VERIFY(VGU_NO_ERROR == vguRoundRect(path, 300, 100, 100, 100, 10, 10));

    // 2) правила черчения / заполнения
   vgSeti( VG_FILL_RULE, VG_EVEN_ODD ); // OR VG_NON_ZERO
    vgSetPaint(fillPaint, VG_FILL_PATH );
    vgSetPaint(strokePaint, VG_STROKE_PATH );

    // 3) Цвета
   vgSetParameterfv( fillPaint, VG_PAINT_COLOR, 4, fillColor);
    vgSetParameterfv( strokePaint, VG_PAINT_COLOR, 4, strokeColor);

    // 4) рисуем фигуры
   vgDrawPath( path, (VG_FILL_PATH | VG_STROKE_PATH) );

	// 5) Освобожлаем память
   vgDestroyPath( path );
    vgDestroyPaint( fillPaint );
    vgDestroyPaint( strokePaint );
#endif
}

static void rendertest2(int w, int h)
{
	static const float clearColor[4] = {0,1,0,1};
	//		float scaleX = w / (tigerMaxX - tigerMinX);
	//		float scaleY = h / (tigerMaxY - tigerMinY);
	//		float scale = fminf(scaleX, scaleY);
	//		PRINTF("render: scaleX=%f, scaleY=%f\n", scaleX, scaleY);

	vgLoadIdentity();
	////eglSwapBuffers(egldisplay, eglsurface);	//force EGL to recognize resize

	vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_BETTER);
	//vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_NONANTIALIASED);
	//vgSeti(VG_FILL_RULE, VG_NON_ZERO);

	vgSeti(VG_PIXEL_LAYOUT, VG_PIXEL_LAYOUT_RGB_HORIZONTAL);
	//vgSeti(VG_SCREEN_LAYOUT, );

	vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
	vgClear(0, 0, w, h);
#if 1

	VGPaint paint = vgCreatePaint();

	static const float drawColorRed[4] = {0,0,0,1};
	vgSetParameterfv(paint, VG_PAINT_COLOR, 4, drawColorRed);
	//vgSetColor(paint, VGuint rgba)

	VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F /*VG_PATH_DATATYPE_F */, 1.0f, 0.0f, 0, 0, (unsigned int)VG_PATH_CAPABILITY_ALL);

	//static const float drawColorRed[4] = {1,0,0,1};
	vgSetfv(VG_TILE_FILL_COLOR, 4, drawColorRed);
	VERIFY(VGU_NO_ERROR == vguRect(path, 0, 0, 100, 100));

	static const float drawColorGreen[4] = {0,0,0,1};
	vgSetfv(VG_TILE_FILL_COLOR, 4, drawColorGreen);
	VERIFY(VGU_NO_ERROR == vguRoundRect(path, 100, 100, 100, 100, 10, 10));

	vgDrawPath(path, VG_STROKE_PATH);	// VG_STROKE_PATH - линиями
	vgDrawPath(path, VG_FILL_PATH);	// VG_FILL_PATH - заполняя

	vgDestroyPath(path);

	vgDestroyPaint(paint);
	//		vgLoadIdentity();
	//		vgScale(scale, scale);
	//		vgTranslate(- tigerMinX, -tigerMinY + 0.5f * (h / scale - (tigerMaxY - tigerMinY)));
	//		//vgTranslate(-tigerMinX + 0.5f * (w / scale - (tigerMaxX - tigerMinX)), -tigerMinY + 0.5f * (h / scale - (tigerMaxY - tigerMinY)));
	//		//vgTranslate(-tigerMinX, tigerMinY);
	//		//vgRotate(30);
	//		PS_render(tiger);
	//		ASSERT(vgGetError() == VG_NO_ERROR);
#endif
}

static void rendertest3(int w, int h)
{
	static const float clearColor[4] = {0,0,1,1};
	//		float scaleX = w / (tigerMaxX - tigerMinX);
	//		float scaleY = h / (tigerMaxY - tigerMinY);
	//		float scale = fminf(scaleX, scaleY);
	//		PRINTF("render: scaleX=%f, scaleY=%f\n", scaleX, scaleY);

	vgLoadIdentity();
	////eglSwapBuffers(egldisplay, eglsurface);	//force EGL to recognize resize

	vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_BETTER);
	//vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_NONANTIALIASED);
	//vgSeti(VG_FILL_RULE, VG_NON_ZERO);

	vgSeti(VG_PIXEL_LAYOUT, VG_PIXEL_LAYOUT_RGB_HORIZONTAL);
	//vgSeti(VG_SCREEN_LAYOUT, );

	vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
	vgClear(0, 0, w, h);
#if 1
	VGPaint paint = vgCreatePaint();

	static const float drawColorRed[4] = {0,0,0,1};
	vgSetParameterfv(paint, VG_PAINT_COLOR, 4, drawColorRed);
	//vgSetColor(paint, VGuint rgba)

	VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F /*VG_PATH_DATATYPE_F */, 1.0f, 0.0f, 0, 0, (unsigned int)VG_PATH_CAPABILITY_ALL);

	//static const float drawColorRed[4] = {1,0,0,1};
	vgSetfv(VG_TILE_FILL_COLOR, 4, drawColorRed);
	VERIFY(VGU_NO_ERROR == vguRect(path, 0, 0, 100, 100));

	static const float drawColorGreen[4] = {0,0,0,1};
	vgSetfv(VG_TILE_FILL_COLOR, 4, drawColorGreen);
	VERIFY(VGU_NO_ERROR == vguRoundRect(path, 100, 100, 100, 100, 10, 10));

	vgDrawPath(path, VG_STROKE_PATH);	// VG_STROKE_PATH - линиями
	vgDrawPath(path, VG_FILL_PATH);	// VG_FILL_PATH - заполняя

	vgDestroyPath(path);

	vgDestroyPaint(paint);
	//		vgLoadIdentity();
	//		vgScale(scale, scale);
	//		vgTranslate(- tigerMinX, -tigerMinY + 0.5f * (h / scale - (tigerMaxY - tigerMinY)));
	//		//vgTranslate(-tigerMinX + 0.5f * (w / scale - (tigerMaxX - tigerMinX)), -tigerMinY + 0.5f * (h / scale - (tigerMaxY - tigerMinY)));
	//		//vgTranslate(-tigerMinX, tigerMinY);
	//		//vgRotate(30);
	//		PS_render(tiger);
	//		ASSERT(vgGetError() == VG_NO_ERROR);
#endif
}


static void rendertestdynamic(int w, int h, int pos, int total)
{
	static const float clearColor[4] = {1.0f, 1.0f, 1.0f, 1.0f };

	////eglSwapBuffers(egldisplay, eglsurface);	//force EGL to recognize resize

	vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_BETTER);
	//vgSeti(VG_RENDERING_QUALITY, VG_RENDERING_QUALITY_NONANTIALIASED);
	//vgSeti(VG_FILL_RULE, VG_NON_ZERO);

	vgSeti(VG_PIXEL_LAYOUT, VG_PIXEL_LAYOUT_RGB_HORIZONTAL);
	//vgSeti(VG_SCREEN_LAYOUT, );

	vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
	vgClear(0, 0, w, h);
#if 1

    VGPaint fillPaint = vgCreatePaint();
    VGPaint strokePaint = vgCreatePaint();

    static const VGfloat fillColor [4] = { 0.0f, 0.0f, 1.0f, 1.0f };
    static const VGfloat strokeColor [4] = { 0.0f, 1.0f, 0.0f, 1.0f };
    static const VGfloat fillColor2 [4] = { 1.0f, 0.0f, 1.0f, 1.0f };
    static const VGfloat strokeColor2 [4] = { 0.0f, 0.0f, 0.0f, 1.0f };


	VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F /*VG_PATH_DATATYPE_F */, 1.0f, 0.0f, 0, 0, (unsigned int)VG_PATH_CAPABILITY_ALL);

	// 0) сброс масштабирования
	vgLoadIdentity();

	// 1) перечисляем (добавляем) фигуры
	VERIFY(VGU_NO_ERROR == vguRect(path, 0, 0, 100, 100));
	VERIFY(VGU_NO_ERROR == vguRoundRect(path, 100, 100, 100 + pos * 5, 10, 10, 10));

	// 2) правила черчения / заполнения
	vgSeti( VG_STROKE_LINE_WIDTH, 3 );		// толщина лини
	vgSeti( VG_FILL_RULE, VG_EVEN_ODD ); // OR VG_NON_ZERO
	vgSetPaint(fillPaint, VG_FILL_PATH );
	vgSetPaint(strokePaint, VG_STROKE_PATH );

	// 3) Цвета
	vgSetParameterfv( fillPaint, VG_PAINT_COLOR, 4, fillColor);
	vgSetParameterfv( strokePaint, VG_PAINT_COLOR, 4, strokeColor);

	// 4) рисуем фигуры
	vgDrawPath( path, (VG_FILL_PATH | VG_STROKE_PATH) );

	vgClearPath(path, VG_PATH_CAPABILITY_ALL);

	// 1a) перечисляем (добавляем) фигуры
	VERIFY(VGU_NO_ERROR == vguRect(path, 50, 50, 100, 100));
	VERIFY(VGU_NO_ERROR == vguRoundRect(path, 150, 150, 100 + pos * 5, 10, 10, 10));

	// 2a) правила черчения / заполнения
	vgSeti( VG_STROKE_LINE_WIDTH, 1 );		// толщина лини
	vgSeti( VG_FILL_RULE, VG_EVEN_ODD ); // OR VG_NON_ZERO
	vgSetPaint(fillPaint, VG_FILL_PATH );
	vgSetPaint(strokePaint, VG_STROKE_PATH );

	// 3a) Цвета
	vgSetParameterfv( fillPaint, VG_PAINT_COLOR, 4, fillColor2);
	vgSetParameterfv( strokePaint, VG_PAINT_COLOR, 4, strokeColor2);

	// 4a) рисуем фигуры
	vgDrawPath( path, (VG_FILL_PATH | VG_STROKE_PATH) );
	vgClearPath(path, VG_PATH_CAPABILITY_ALL);

	// 5a) Освобожлаем память
	vgDestroyPath(path);

	vgDestroyPaint(strokePaint);
	vgDestroyPaint(fillPaint);

#endif
}

/*--------------------------------------------------------------*/

#endif /* WITHOPENVG */

#if 1

void setnormal(float * p, int state)
{
	int dir = state % 2;
	int ix = state / 3;
	p [0] = 0;
	p [1] = 0;
	p [2] = 0;

	p [ix] = dir ? 1 : -1;
}

//
//	Нормали Треугольников
//	Нормаль к плоскости — это единичный вектор который направлен перпендикулярно к этой плоскости.
//	Нормаль к треугольнику — это единичный вектор направленный перпендикулярно к треугольнику.
//	Нормаль очень просто рассчитывается с помощью векторного произведения двух сторон
//	треугольника(если вы помните, векторное произведение двух векторов дает нам
//	перпендикулярный вектор к обоим) и нормализованный: его длина устанавливается в единицу.
//	Вот псевдокод вычисления нормали:
//
//	треугольник( v1, v2, v3 )
//	сторона1 = v2-v1
//	сторона2 = v3-v1
//	треугольник.нормаль = вектПроизведение(сторона1, сторона2).нормализировать()

void calcnormaface(const float * model, int * faces, float normal [3])
{
	int nvertixes = model [0];
	int nfaces = model [1];
	const float * const bufvertixes = model + 2;
	const float * const buffaces = bufvertixes + (nvertixes * 6);

	int i;
	for (i = 0; i < 3; ++ i)
	{
		int face = faces [i];
	}
}

void calcnormaltriangle(const float * model)
{
	int nvertixes = model [0];
	int nfaces = model [1];
	const float * const bufvertixes = model + 2;
	const float * const buffaces = bufvertixes + (nvertixes * 6);
	int i;
	int faces [nvertixes][3];
	int foundfaces = 0;

	for (i = 0; i < nvertixes; ++ i)
	{
		int j;
		// Ищем faces, имеющие в себе данную вершину.
		for (j = 0; j < nfaces && foundfaces < 3; ++ j)
		{
			if (
					buffaces [j * 6 + 0] == i ||
					buffaces [j * 6 + 1] == i ||
					buffaces [j * 6 + 2] == i ||
					0)
			{
				faces [i][foundfaces ++] = j;	// Нашли треугольник содержащий вершину
			}
		}
		if (foundfaces != 3)
		{
			PRINTF("nvertixes = %d: faces should be 3 (foundfaces=%d)\n", nvertixes, foundfaces);
			return;
		}

		//calcfacesnormal(model, )
	}
	for (i = 0; i < nvertixes; ++ i)
	{
		float normal [3][3];
		int j;
		// расчитать нормаль треугольников
		for (j = 0; j < 3; ++ j)
		{
			calcnormaface(model, faces [i], normal [j]);
		}
	}
//	int x1 = verticexs [0];
//	int y1 = verticexs [1];
//	int z1 = verticexs [3];
}
//	Вершинная Нормаль
//	Это нормаль введенная для удобства вычислений.
//	Это комбинированная нормаль от нормалей окружающих данную вершину треугольников.
//	Это очень удобно, так как в вершинных шейдерах мы имеем дело с вершинами,
//	а не с треугольниками.
//	В любом случае в OpenGL у мы почти никогда и не имеем дела с треугольниками.
//
//	вершина v1, v2, v3, ....
//	треугольник tr1, tr2, tr3 // они все используют вершину v1
//	v1.нормаль = нормализовать( tr1.нормаль + tr2.нормаль + tr3.нормаль)
#endif

#if 0 && WITHDEBUG && WITHSMPSYSTEM

static void SecondCPUTaskSGI13(void)
{
	const int cpu = __get_MPIDR() & 0x03;
	PRINTF("mSGI13 Run debug thread test: I am CPU=%d\n", cpu);
}

static void SecondCPUTaskSGI15(void)
{
	const int cpu = __get_MPIDR() & 0x03;
	PRINTF("mSGI15 Run debug thread test: I am CPU=%d\n", cpu);
}

#endif

#if 0
// See also https://www.analog.com/media/en/technical-documentation/data-sheets/ADIS16137.pdf
static void adis161xx_write16(unsigned page, unsigned addr, unsigned value)
{
	enum { WRITEFLAG = 0x80 };
	const uint_fast8_t spispeedindex = SPIC_SPEED1M;
	const spi_modes_t spimode = SPIC_MODE3;
	const unsigned wrriteflag = 1;
	const spitarget_t cs = targetext2;
	unsigned v1, v2;

	spi_select2(cs, spimode, spispeedindex);
	spi_read_byte(cs, WRITEFLAG | 0x00);	// 0x80: write, addr=0x00: page_id register
	spi_read_byte(cs, page);	// page value
	spi_unselect(cs);

	spi_select2(cs, spimode, spispeedindex);
	spi_read_byte(cs, WRITEFLAG | (addr & 0x7F));
	spi_read_byte(cs, value >> 0);
	spi_unselect(cs);

	spi_select2(cs, spimode, spispeedindex);
	spi_read_byte(cs, WRITEFLAG | ((addr & 0x7F) + 1));
	spi_read_byte(cs, value >> 8);
	spi_unselect(cs);

	//local_delay_ms(10);

}

// See also https://www.analog.com/media/en/technical-documentation/data-sheets/ADIS16137.pdf
static unsigned adis161xx_read16(unsigned page, unsigned addr)
{
	enum { WRITEFLAG = 0x80 };
	const uint_fast8_t spispeedindex = SPIC_SPEED1M;
	const spi_modes_t spimode = SPIC_MODE3;
	const unsigned wrriteflag = 0;
	const spitarget_t cs = targetext2;
	unsigned v1, v2;

	spi_select2(cs, spimode, spispeedindex);
	spi_read_byte(cs, WRITEFLAG | 0x00);	// 0x80: write, addr=0x00: page_id register
	spi_read_byte(cs, page);	// page value
	spi_unselect(cs);

	spi_select2(cs, spimode, spispeedindex);
	spi_read_byte(cs, addr & 0x7F);
	spi_read_byte(cs, 0);
	spi_unselect(cs);

	spi_select2(cs, spimode, spispeedindex);
	v1 = spi_read_byte(cs, 0);
	v2 = spi_read_byte(cs, 0);
	spi_unselect(cs);

	return v1 * 256 + v2;
}

// See also https://www.analog.com/media/en/technical-documentation/data-sheets/ADIS16137.pdf
static uint_fast32_t adis161xx_read32(unsigned page, unsigned addr)
{
	enum { WRITEFLAG = 0x80 };
	const uint_fast8_t spispeedindex = SPIC_SPEED1M;
	const spi_modes_t spimode = SPIC_MODE3;
	const unsigned wrriteflag = 0;
	const spitarget_t cs = targetext2;
	unsigned v1, v2, v3, v4;

	spi_select2(cs, spimode, spispeedindex);
	spi_read_byte(cs, WRITEFLAG | 0x00);	// 0x80: write, addr=0x00: page_id register
	spi_read_byte(cs, page);	// page value
	spi_unselect(cs);

	// LOW part of pair
	spi_select2(cs, spimode, spispeedindex);
	spi_read_byte(cs, (addr & 0x7F) + 0);
	spi_read_byte(cs, 0);
	spi_unselect(cs);

	spi_select2(cs, spimode, spispeedindex);
	v1 = spi_read_byte(cs, 0);
	v2 = spi_read_byte(cs, 0);
	spi_unselect(cs);

	// OUT part of pair
	spi_select2(cs, spimode, spispeedindex);
	spi_read_byte(cs, (addr & 0x7F) + 2);
	spi_read_byte(cs, 0);
	spi_unselect(cs);


	spi_select2(cs, spimode, spispeedindex);
	v3 = spi_read_byte(cs, 0);
	v4 = spi_read_byte(cs, 0);
	spi_unselect(cs);

	return
			((uint_fast32_t) v1) << 8 |
			((uint_fast32_t) v2) << 0 |
			((uint_fast32_t) v3) << 24 |
			((uint_fast32_t) v4) << 16 |
			0;
}

#endif

#if CPUSTYLE_F133

// https://github.com/bluespec/CLINT/blob/main/src/CLINT_AXI4.bsv

//#define RISCV_MSIP0 (CLINT_BASE  + 0x0000)
#define RISCV_MTIMECMP_ADDR (CLINT_BASE  + 0x4000)

// На Allwinner F133-A доступ к регистрам таймера только 32-х битный
static uint64_t mtimer_get_raw_time_cmp(void) {
#if 0//( __riscv_xlen == 64)
    // Directly read 64 bit value
    volatile uint64_t *mtime = (volatile uint64_t *)(RISCV_MTIMECMP_ADDR);
    return *mtime;
#elif 1//( __riscv_xlen == 32)
    volatile uint32_t * mtimel = (volatile uint32_t *)(RISCV_MTIMECMP_ADDR);
    volatile uint32_t * mtimeh = (volatile uint32_t *)(RISCV_MTIMECMP_ADDR+4);
    uint32_t mtimeh_val;
    uint32_t mtimel_val;
    do {
        // There is a small risk the mtimeh will tick over after reading mtimel
        mtimeh_val = *mtimeh;
        mtimel_val = *mtimel;
        // Poll mtimeh to ensure it's consistent after reading mtimel
        // The frequency of mtimeh ticking over is low
    } while (mtimeh_val != *mtimeh);
    return (uint64_t) ( ( ((uint64_t)mtimeh_val)<<32) | mtimel_val);
#else
    return 888;
#endif
}

// На Allwinner F133-A доступ к регистрам таймера только 32-х битный
static void mtimer_set_raw_time_cmp(uint64_t new_mtimecmp) {
#if 0//(__riscv_xlen == 64)
    // Single bus access
    volatile uint64_t *mtimecmp = (volatile uint64_t*)(RISCV_MTIMECMP_ADDR);
    *mtimecmp = new_mtimecmp;
#elif 1//( __riscv_xlen == 32)
    volatile uint32_t *mtimecmpl = (volatile uint32_t *)(RISCV_MTIMECMP_ADDR);
    volatile uint32_t *mtimecmph = (volatile uint32_t *)(RISCV_MTIMECMP_ADDR+4);
    // AS we are doing 32 bit writes, an intermediate mtimecmp value may cause spurious interrupts.
    // Prevent that by first setting the dummy MSB to an unacheivable value
    *mtimecmph = 0xFFFFFFFF;  // cppcheck-suppress redundantAssignment
    // set the LSB
    *mtimecmpl = (uint32_t)(new_mtimecmp & 0x0FFFFFFFFUL);
    // Set the correct MSB
    *mtimecmph = (uint32_t)(new_mtimecmp >> 32); // cppcheck-suppress redundantAssignment
#else
#endif
}
#endif /* CPUSTYLE_F133 */

#if 0

// PNG files test

#include "lupng.h"

// PNG files test
void testpng(const void * pngbuffer)
{
	PACKEDCOLORPIP_T * const fb = colmain_fb_draw();
	LuImage * png = luPngReadMemory((char *) pngbuffer);

	PACKEDCOLORPIP_T * const fbpic = (PACKEDCOLORPIP_T *) png->data;
	const COLORPIP_T keycolor = TFTRGB(png->data [0], png->data [1], png->data [2]);	/* угловой пиксель - надо правильно преобразовать из ABGR*/
	const unsigned picdx = png->width;//GXADJ(png->width);
	const unsigned picw = png->width;
	const unsigned pich = png->height;

	PRINTF("testpng: sz=%u data=%p, dataSize=%u, depth=%u, w=%u, h=%u\n", (unsigned) sizeof fbpic [0], png, (unsigned) png->dataSize,  (unsigned) png->depth, (unsigned) png->width, (unsigned) png->height);

	colpip_fillrect(fb, DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, COLORMAIN_GRAY);

	colpip_stretchblt(
		(uintptr_t) fb, GXSIZE(DIM_X, DIM_Y) * sizeof fb [0],
		fb, DIM_X, DIM_Y,
		0, 0, picw / 4, pich / 4,		/* позиция и размеры прямоугольника - получателя */
		(uintptr_t) fbpic, GXSIZE(picw, pich) * sizeof fbpic [0],
		fbpic, picdx, pich,
		0, 0, picdx, pich,
		BITBLT_FLAG_NONE | BITBLT_FLAG_CKEY | 1*BITBLT_FLAG_SRC_ABGR8888, keycolor
		);

	colpip_stretchblt(
		(uintptr_t) fb, GXSIZE(DIM_X, DIM_Y) * sizeof fb [0],
		fb, DIM_X, DIM_Y,
		30, 0, picw / 2, pich / 2,		/* позиция и размеры прямоугольника - получателя */
		(uintptr_t) fbpic, GXSIZE(picw, pich) * sizeof fbpic [0],
		fbpic, picdx, pich,
		0, 0, picdx, pich,
		BITBLT_FLAG_NONE | 0*BITBLT_FLAG_CKEY | 1*BITBLT_FLAG_SRC_ABGR8888, keycolor
		);

	colpip_stretchblt(
		(uintptr_t) fb, GXSIZE(DIM_X, DIM_Y) * sizeof fb [0],
		fb, DIM_X, DIM_Y,
		30, pich / 2, picw / 2, pich / 2,		/* позиция и размеры прямоугольника - получателя */
		(uintptr_t) fbpic, GXSIZE(picw, pich) * sizeof fbpic [0],
		fbpic, picdx, pich,
		0, 0, picdx, pich,
		BITBLT_FLAG_NONE | 1*BITBLT_FLAG_CKEY | 1*BITBLT_FLAG_SRC_ABGR8888, keycolor
		);

	colpip_stretchblt(
		(uintptr_t) fb, GXSIZE(DIM_X, DIM_Y) * sizeof fb [0],
		fb, DIM_X, DIM_Y,
		300, 100, picw / 1, pich / 1,		/* позиция и размеры прямоугольника - получателя */
		(uintptr_t) fbpic, GXSIZE(picw, pich) * sizeof fbpic [0],
		fbpic, picdx, pich,
		0, 0, picdx, pich,
		BITBLT_FLAG_NONE | BITBLT_FLAG_CKEY | 1*BITBLT_FLAG_SRC_ABGR8888, keycolor
		);

	luImageRelease(png, NULL);
	for (;;)
		;
}

#endif

#if (__CORTEX_A == 53U) && CPUSTYLE_CA53

// 4.5.80 Configuration Base Address Register
/** \brief  Get CBAR
    \return               Configuration Base Address Register
    MRC p15, 1, <Rt>, c15, c3, 0; Read CBAR into Rt
 */
__STATIC_FORCEINLINE uint32_t __get_CA53_CBAR(void)
{
	uint32_t result;
  __get_CP(15, 1, result, 15, 3, 0);
  return(result);
}

#endif /* (__CORTEX_A == 53U) && CPUSTYLE_CA53 */

#if (CPUSTYLE_T113 || CPUSTYLE_F133) && 0

#define GPASDC_Vref 1800.f

///GPADC_DATA=Vin/Vref*4095

void gpadc_initialize(void)
{

	CCU->GPADC_BGR_REG |= (UINT32_C(1) << 16); // 1: De-assert reset  HOSC
	CCU->GPADC_BGR_REG |= (UINT32_C(1) << 0); // 1: Pass clock

	GPADC->GP_SR_CON |= (0x2fu << 0);
	GPADC->GP_CTRL |= (0x2u << 18); // continuous mode

	GPADC->GP_CS_EN |= (UINT32_C(1) << 0); // enable

	GPADC->GP_CTRL |= (UINT32_C(1) << 17); // calibration

	GPADC->GP_CTRL |= (UINT32_C(1) << 16);

	while ((GPADC->GP_DATA_INTS) & (1uL << 0))	///if 1 complete
		;

}

void gpadc_test(void)
{
	float Vin;
	Vin = GPADC->GP_CH0_DATA * GPASDC_Vref / 4095.f * 2.8f; ///2.8f my divide resistors

	PRINTF("5V= %d \n", (int) Vin);
}

void gpadc_inthandler(void)
{
	PRINTF("%ld\n", GPADC->GP_CH0_DATA);
}

#endif /* (CPUSTYLE_T113 || CPUSTYLE_F133) */

#if CPUSTYLE_T113

/* HiFI4 DSP-viewed address offset translate to host cpu viewwed */
static ptrdiff_t xlate_dsp2mpu(ptrdiff_t a)
{
	const ptrdiff_t BANKSIZE = 0x08000u;
	const ptrdiff_t CELLBASE = 0x10000u;
	const ptrdiff_t CELLSIZE = 16;
	const ptrdiff_t cellbank = (a - CELLBASE) / BANKSIZE;
	const ptrdiff_t cellrow = (a - CELLBASE) % BANKSIZE / CELLSIZE;	/* гранулярность 16 байт */
	const unsigned cellpos = (a % CELLSIZE);	/* гранулярность 16 байт */

	if (a < CELLBASE)
		return a;	/* translation not needed. */

	return CELLBASE +
			cellbank * BANKSIZE +
			CELLSIZE * ((cellrow % 2) ? (cellrow / 2) + (BANKSIZE / CELLSIZE / 2) : cellrow / 2) +
			cellpos;
}

/* memcpy replacement for Allwinner T113-s3 dsp memory */
static void copy2dsp(uint8_t * pdspmap, const uint8_t * pcpu, unsigned offs, unsigned size)
{
	for (; size --; ++ offs)
	{
		pdspmap [xlate_dsp2mpu(offs)] = pcpu [offs];
	}
}

/* memset replacement for Allwinner T113-s3 dsp memory */
static void zero2dsp(uint8_t * pdspmap, unsigned offs, unsigned size)
{
	for (; size --; ++ offs)
	{
		pdspmap [xlate_dsp2mpu(offs)] = 0x00;	/* fill by zero */
	}
}

//static void xtest(void)
//{
//	unsigned mpu;
//	unsigned dsp;
//
//	dsp = 0x10000;
//	mpu = xlate_dsp2mpu(dsp);
//	PRINTF("dsp=%08X, mpu=%08X\n", dsp, mpu);
//
//	dsp = 0x10010;
//	mpu = xlate_dsp2mpu(dsp);
//	PRINTF("dsp=%08X, mpu=%08X\n", dsp, mpu);
//
//	dsp = 0x10020;
//	mpu = xlate_dsp2mpu(dsp);
//	PRINTF("dsp=%08X, mpu=%08X\n", dsp, mpu);
//
//	dsp = 0x10030;
//	mpu = xlate_dsp2mpu(dsp);
//	PRINTF("dsp=%08X, mpu=%08X\n", dsp, mpu);
//
//	dsp = 0x18020;
//	mpu = xlate_dsp2mpu(dsp);
//	PRINTF("dsp=%08X, mpu=%08X\n", dsp, mpu);
//
//	dsp = 0x18030;
//	mpu = xlate_dsp2mpu(dsp);
//	PRINTF("dsp=%08X, mpu=%08X\n", dsp, mpu);
//}

#endif /* CPUSTYLE_T113 */

#if CPUSTYLE_STM32MP1 && WITHETHHW && 1

void ethhw_initialize(void)
{
	RCC->ETHCKSELR =
			0 |
			0;

	RCC->MP_AHB6ENSETR = RCC_MP_AHB6ENSETR_ETHMACEN_Msk;
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6ENSETR = RCC_MP_AHB6ENSETR_ETHTXEN_Msk;
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6ENSETR = RCC_MP_AHB6ENSETR_ETHRXEN_Msk;
	(void) RCC->MP_AHB6ENSETR;

	RCC->AHB6RSTSETR = RCC_AHB6RSTSETR_ETHMACRST_Msk;	// assert reset
	(void) RCC->AHB6RSTSETR;
	RCC->AHB6RSTCLRR = RCC_AHB6RSTCLRR_ETHMACRST_Msk;	// de-assert reset
	(void) RCC->AHB6RSTCLRR;

	HARDWARE_ETH_INITIALIZE();
}

void ethhw_deinitialize(void)
{
	RCC->AHB6RSTSETR = RCC_AHB6RSTSETR_ETHMACRST_Msk;	// assert reset
	(void) RCC->AHB6RSTSETR;

	RCC->MP_AHB6ENCLRR = RCC_MP_AHB6ENCLRR_ETHTXEN_Msk;
	(void) RCC->MP_AHB6ENCLRR;
	RCC->MP_AHB6ENCLRR = RCC_MP_AHB6ENCLRR_ETHRXEN_Msk;
	(void) RCC->MP_AHB6ENCLRR;
	RCC->MP_AHB6ENCLRR = RCC_MP_AHB6ENCLRR_ETHMACEN_Msk;
	(void) RCC->MP_AHB6ENCLRR;
}

#define ETHHW_BUFFSIZE 4096

void ethhw_filldesc(volatile uint32_t * desc, uint8_t * buff1, uint8_t * buff2)
{
	desc [0] = (uintptr_t) buff1;
	desc [1] = (uintptr_t) buff2;
	desc [2] =
		0 * (UINT32_C(1) << 31) | // IOC
		0 * (UINT32_C(1) << 30) | // TTSE
		ETHHW_BUFFSIZE * (UINT32_C(1) << 16) | // B2L = buffer 2 length
		ETHHW_BUFFSIZE * (UINT32_C(1) << 0) | // B1L = buffer 1 length
		0;

	desc [3] =
		1 * (UINT32_C(1) << 29) | // First Descriptor
		1 * (UINT32_C(1) << 28) | // Last Descriptor
		0;
}

#endif

#if CPUSTYLE_VM14

/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright © 1999-2010 David Woodhouse <dwmw2@infradead.org> et al.
 *
 */

#ifndef __MTD_MTD_H__
#define __MTD_MTD_H__

//#ifndef __UBOOT__
//#include <linux/types.h>
//#include <linux/uio.h>
//#include <linux/notifier.h>
//#include <linux/device.h>
//
//#include <mtd/mtd-abi.h>
//
//#include <asm/div64.h>
//#else
//#include <linux/compat.h>
//#include <mtd/mtd-abi.h>
//#include <linux/errno.h>
//#include <linux/list.h>
//#include <div64.h>
//#if IS_ENABLED(CONFIG_DM)
//#include <dm/device.h>
//#endif

#define MAX_MTD_DEVICES 32
//#endif

#define MTD_ERASE_PENDING	0x01
#define MTD_ERASING		0x02
#define MTD_ERASE_SUSPEND	0x04
#define MTD_ERASE_DONE		0x08
#define MTD_ERASE_FAILED	0x10

#define MTD_FAIL_ADDR_UNKNOWN -1LL

/*
 * If the erase fails, fail_addr might indicate exactly which block failed. If
 * fail_addr = MTD_FAIL_ADDR_UNKNOWN, the failure was not at the device level
 * or was not specific to any particular block.
 */
struct erase_info {
	struct mtd_info *mtd;
	uint64_t addr;
	uint64_t len;
	uint64_t fail_addr;
	u_long time;
	u_long retries;
	unsigned dev;
	unsigned cell;
	void (*callback) (struct erase_info *self);
	u_long priv;
	u_char state;
	struct erase_info *next;
	int scrub;
};

struct mtd_erase_region_info {
	uint64_t offset;		/* At which this region starts, from the beginning of the MTD */
	uint32_t erasesize;		/* For this region */
	uint32_t numblocks;		/* Number of blocks of erasesize in this region */
	unsigned long *lockmap;		/* If keeping bitmap of locks */
};

/**
 * struct mtd_oob_ops - oob operation operands
 * @mode:	operation mode
 *
 * @len:	number of data bytes to write/read
 *
 * @retlen:	number of data bytes written/read
 *
 * @ooblen:	number of oob bytes to write/read
 * @oobretlen:	number of oob bytes written/read
 * @ooboffs:	offset of oob data in the oob area (only relevant when
 *		mode = MTD_OPS_PLACE_OOB or MTD_OPS_RAW)
 * @datbuf:	data buffer - if NULL only oob data are read/written
 * @oobbuf:	oob data buffer
 */
struct mtd_oob_ops {
	unsigned int	mode;
	size_t		len;
	size_t		retlen;
	size_t		ooblen;
	size_t		oobretlen;
	uint32_t	ooboffs;
	uint8_t		*datbuf;
	uint8_t		*oobbuf;
};

#ifdef CONFIG_SYS_NAND_MAX_OOBFREE
#define MTD_MAX_OOBFREE_ENTRIES_LARGE	CONFIG_SYS_NAND_MAX_OOBFREE
#else
#define MTD_MAX_OOBFREE_ENTRIES_LARGE	32
#endif

#ifdef CONFIG_SYS_NAND_MAX_ECCPOS
#define MTD_MAX_ECCPOS_ENTRIES_LARGE	CONFIG_SYS_NAND_MAX_ECCPOS
#else
#define MTD_MAX_ECCPOS_ENTRIES_LARGE	680
#endif
/**
 * struct mtd_oob_region - oob region definition
 * @offset: region offset
 * @length: region length
 *
 * This structure describes a region of the OOB area, and is used
 * to retrieve ECC or free bytes sections.
 * Each section is defined by an offset within the OOB area and a
 * length.
 */
struct mtd_oob_region {
	uint32_t offset;
	uint32_t length;
};

/*
 * struct mtd_ooblayout_ops - NAND OOB layout operations
 * @ecc: function returning an ECC region in the OOB area.
 *	 Should return -ERANGE if %section exceeds the total number of
 *	 ECC sections.
 * @free: function returning a free region in the OOB area.
 *	  Should return -ERANGE if %section exceeds the total number of
 *	  free sections.
 */
struct mtd_ooblayout_ops {
	int (*ecc)(struct mtd_info *mtd, int section,
		   struct mtd_oob_region *oobecc);
	int (*free)(struct mtd_info *mtd, int section,
		    struct mtd_oob_region *oobfree);
};

typedef signed long loff_t;
typedef signed long resource_size_t;

struct nand_oobfree {
	uint32_t offset;
	uint32_t length;
};


struct otp_info {
	uint32_t start;
	uint32_t length;
	uint32_t locked;
};

/*
 * Internal ECC layout control structure. For historical reasons, there is a
 * similar, smaller struct nand_ecclayout_user (in mtd-abi.h) that is retained
 * for export to user-space via the ECCGETLAYOUT ioctl.
 * nand_ecclayout should be expandable in the future simply by the above macros.
 */
struct nand_ecclayout {
	uint32_t eccbytes;
	uint32_t eccpos[MTD_MAX_ECCPOS_ENTRIES_LARGE];
	uint32_t oobavail;
	struct nand_oobfree oobfree[MTD_MAX_OOBFREE_ENTRIES_LARGE];
};

struct module;	/* only needed for owner field in mtd_info */

struct mtd_info {
	u_char type;
	uint32_t flags;
	uint64_t size;	 // Total size of the MTD

	/* "Major" erase size for the device. Naïve users may take this
	 * to be the only erase size available, or may use the more detailed
	 * information below if they desire
	 */
	uint32_t erasesize;
	/* Minimal writable flash unit size. In case of NOR flash it is 1 (even
	 * though individual bits can be cleared), in case of NAND flash it is
	 * one NAND page (or half, or one-fourths of it), in case of ECC-ed NOR
	 * it is of ECC block size, etc. It is illegal to have writesize = 0.
	 * Any driver registering a struct mtd_info must ensure a writesize of
	 * 1 or larger.
	 */
	uint32_t writesize;

	/*
	 * Size of the write buffer used by the MTD. MTD devices having a write
	 * buffer can write multiple writesize chunks at a time. E.g. while
	 * writing 4 * writesize bytes to a device with 2 * writesize bytes
	 * buffer the MTD driver can (but doesn't have to) do 2 writesize
	 * operations, but not 4. Currently, all NANDs have writebufsize
	 * equivalent to writesize (NAND page size). Some NOR flashes do have
	 * writebufsize greater than writesize.
	 */
	uint32_t writebufsize;

	uint32_t oobsize;   // Amount of OOB data per block (e.g. 16)
	uint32_t oobavail;  // Available OOB bytes per block

	/*
	 * If erasesize is a power of 2 then the shift is stored in
	 * erasesize_shift otherwise erasesize_shift is zero. Ditto writesize.
	 */
	unsigned int erasesize_shift;
	unsigned int writesize_shift;
	/* Masks based on erasesize_shift and writesize_shift */
	unsigned int erasesize_mask;
	unsigned int writesize_mask;

	/*
	 * read ops return -EUCLEAN if max number of bitflips corrected on any
	 * one region comprising an ecc step equals or exceeds this value.
	 * Settable by driver, else defaults to ecc_strength.  User can override
	 * in sysfs.  N.B. The meaning of the -EUCLEAN return code has changed;
	 * see Documentation/ABI/testing/sysfs-class-mtd for more detail.
	 */
	unsigned int bitflip_threshold;

	// Kernel-only stuff starts here.
#ifndef __UBOOT__
	const char *name;
#else
	char *name;
#endif
	int index;

	/* OOB layout description */
	const struct mtd_ooblayout_ops *ooblayout;

	/* ECC layout structure pointer - read only! */
	struct nand_ecclayout *ecclayout;

	/* the ecc step size. */
	unsigned int ecc_step_size;

	/* max number of correctible bit errors per ecc step */
	unsigned int ecc_strength;

	/* Data for variable erase regions. If numeraseregions is zero,
	 * it means that the whole device has erasesize as given above.
	 */
	int numeraseregions;
	struct mtd_erase_region_info *eraseregions;

	/*
	 * Do not call via these pointers, use corresponding mtd_*()
	 * wrappers instead.
	 */
	int (*_erase) (struct mtd_info *mtd, struct erase_info *instr);
#ifndef __UBOOT__
	int (*_point) (struct mtd_info *mtd, loff_t from, size_t len,
		       size_t *retlen, void **virt, resource_size_t *phys);
	int (*_unpoint) (struct mtd_info *mtd, loff_t from, size_t len);
#endif
	unsigned long (*_get_unmapped_area) (struct mtd_info *mtd,
					     unsigned long len,
					     unsigned long offset,
					     unsigned long flags);
	int (*_read) (struct mtd_info *mtd, loff_t from, size_t len,
		      size_t *retlen, u_char *buf);
	int (*_write) (struct mtd_info *mtd, loff_t to, size_t len,
		       size_t *retlen, const u_char *buf);
	int (*_panic_write) (struct mtd_info *mtd, loff_t to, size_t len,
			     size_t *retlen, const u_char *buf);
	int (*_read_oob) (struct mtd_info *mtd, loff_t from,
			  struct mtd_oob_ops *ops);
	int (*_write_oob) (struct mtd_info *mtd, loff_t to,
			   struct mtd_oob_ops *ops);
	int (*_get_fact_prot_info) (struct mtd_info *mtd, size_t len,
				    size_t *retlen, struct otp_info *buf);
	int (*_read_fact_prot_reg) (struct mtd_info *mtd, loff_t from,
				    size_t len, size_t *retlen, u_char *buf);
	int (*_get_user_prot_info) (struct mtd_info *mtd, size_t len,
				    size_t *retlen, struct otp_info *buf);
	int (*_read_user_prot_reg) (struct mtd_info *mtd, loff_t from,
				    size_t len, size_t *retlen, u_char *buf);
	int (*_write_user_prot_reg) (struct mtd_info *mtd, loff_t to,
				     size_t len, size_t *retlen, u_char *buf);
	int (*_lock_user_prot_reg) (struct mtd_info *mtd, loff_t from,
				    size_t len);
#ifndef __UBOOT__
	int (*_writev) (struct mtd_info *mtd, const struct kvec *vecs,
			unsigned long count, loff_t to, size_t *retlen);
#endif
	void (*_sync) (struct mtd_info *mtd);
	int (*_lock) (struct mtd_info *mtd, loff_t ofs, uint64_t len);
	int (*_unlock) (struct mtd_info *mtd, loff_t ofs, uint64_t len);
	int (*_is_locked) (struct mtd_info *mtd, loff_t ofs, uint64_t len);
	int (*_block_isreserved) (struct mtd_info *mtd, loff_t ofs);
	int (*_block_isbad) (struct mtd_info *mtd, loff_t ofs);
	int (*_block_markbad) (struct mtd_info *mtd, loff_t ofs);
#ifndef __UBOOT__
	int (*_suspend) (struct mtd_info *mtd);
	void (*_resume) (struct mtd_info *mtd);
	void (*_reboot) (struct mtd_info *mtd);
#endif
	/*
	 * If the driver is something smart, like UBI, it may need to maintain
	 * its own reference counting. The below functions are only for driver.
	 */
	int (*_get_device) (struct mtd_info *mtd);
	void (*_put_device) (struct mtd_info *mtd);

#ifndef __UBOOT__
	/* Backing device capabilities for this device
	 * - provides mmap capabilities
	 */
	struct backing_dev_info *backing_dev_info;

	struct notifier_block reboot_notifier;  /* default mode before reboot */
#endif

	/* ECC status information */
	struct mtd_ecc_stats ecc_stats;
	/* Subpage shift (NAND) */
	int subpage_sft;

	void *priv;

	struct module *owner;
#ifndef __UBOOT__
	struct device dev;
#else
	struct udevice *dev;
#endif
	int usecount;

	/* MTD devices do not have any parent. MTD partitions do. */
	struct mtd_info *parent;

	/*
	 * Offset of the partition relatively to the parent offset.
	 * Is 0 for real MTD devices (ie. not partitions).
	 */
	u64 offset;

	/*
	 * List node used to add an MTD partition to the parent
	 * partition list.
	 */
	struct list_head node;

	/*
	 * List of partitions attached to this MTD device (the parent
	 * MTD device can itself be a partition).
	 */
	struct list_head partitions;
};

#if IS_ENABLED(CONFIG_DM)
static inline void mtd_set_of_node(struct mtd_info *mtd,
				   const struct device_node *np)
{
	mtd->dev->node.np = np;
}

static inline const struct device_node *mtd_get_of_node(struct mtd_info *mtd)
{
	return mtd->dev->node.np;
}
#else
struct device_node;

static inline void mtd_set_of_node(struct mtd_info *mtd,
				   const struct device_node *np)
{
}

static inline const struct device_node *mtd_get_of_node(struct mtd_info *mtd)
{
	return NULL;
}
#endif

static inline bool mtd_is_partition(const struct mtd_info *mtd)
{
	return mtd->parent;
}

static inline bool mtd_has_partitions(const struct mtd_info *mtd)
{
	return !list_empty(&mtd->partitions);
}

bool mtd_partitions_used(struct mtd_info *master);

int mtd_ooblayout_ecc(struct mtd_info *mtd, int section,
		      struct mtd_oob_region *oobecc);
int mtd_ooblayout_find_eccregion(struct mtd_info *mtd, int eccbyte,
				 int *section,
				 struct mtd_oob_region *oobregion);
int mtd_ooblayout_get_eccbytes(struct mtd_info *mtd, u8 *eccbuf,
			       const u8 *oobbuf, int start, int nbytes);
int mtd_ooblayout_set_eccbytes(struct mtd_info *mtd, const u8 *eccbuf,
			       u8 *oobbuf, int start, int nbytes);
int mtd_ooblayout_free(struct mtd_info *mtd, int section,
		       struct mtd_oob_region *oobfree);
int mtd_ooblayout_get_databytes(struct mtd_info *mtd, u8 *databuf,
				const u8 *oobbuf, int start, int nbytes);
int mtd_ooblayout_set_databytes(struct mtd_info *mtd, const u8 *databuf,
				u8 *oobbuf, int start, int nbytes);
int mtd_ooblayout_count_freebytes(struct mtd_info *mtd);
int mtd_ooblayout_count_eccbytes(struct mtd_info *mtd);

static inline void mtd_set_ooblayout(struct mtd_info *mtd,
				     const struct mtd_ooblayout_ops *ooblayout)
{
	mtd->ooblayout = ooblayout;
}

static inline uint32_t mtd_oobavail(struct mtd_info *mtd, struct mtd_oob_ops *ops)
{
	return ops->mode == MTD_OPS_AUTO_OOB ? mtd->oobavail : mtd->oobsize;
}

int mtd_erase(struct mtd_info *mtd, struct erase_info *instr);
#ifndef __UBOOT__
int mtd_point(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen,
	      void **virt, resource_size_t *phys);
int mtd_unpoint(struct mtd_info *mtd, loff_t from, size_t len);
#endif
unsigned long mtd_get_unmapped_area(struct mtd_info *mtd, unsigned long len,
				    unsigned long offset, unsigned long flags);
int mtd_read(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen,
	     u_char *buf);
int mtd_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen,
	      const u_char *buf);
int mtd_panic_write(struct mtd_info *mtd, loff_t to, size_t len, size_t *retlen,
		    const u_char *buf);

int mtd_read_oob(struct mtd_info *mtd, loff_t from, struct mtd_oob_ops *ops);
int mtd_write_oob(struct mtd_info *mtd, loff_t to, struct mtd_oob_ops *ops);

int mtd_get_fact_prot_info(struct mtd_info *mtd, size_t len, size_t *retlen,
			   struct otp_info *buf);
int mtd_read_fact_prot_reg(struct mtd_info *mtd, loff_t from, size_t len,
			   size_t *retlen, u_char *buf);
int mtd_get_user_prot_info(struct mtd_info *mtd, size_t len, size_t *retlen,
			   struct otp_info *buf);
int mtd_read_user_prot_reg(struct mtd_info *mtd, loff_t from, size_t len,
			   size_t *retlen, u_char *buf);
int mtd_write_user_prot_reg(struct mtd_info *mtd, loff_t to, size_t len,
			    size_t *retlen, u_char *buf);
int mtd_lock_user_prot_reg(struct mtd_info *mtd, loff_t from, size_t len);

#ifndef __UBOOT__
int mtd_writev(struct mtd_info *mtd, const struct kvec *vecs,
	       unsigned long count, loff_t to, size_t *retlen);
#endif

static inline void mtd_sync(struct mtd_info *mtd)
{
	if (mtd->_sync)
		mtd->_sync(mtd);
}

int mtd_lock(struct mtd_info *mtd, loff_t ofs, uint64_t len);
int mtd_unlock(struct mtd_info *mtd, loff_t ofs, uint64_t len);
int mtd_is_locked(struct mtd_info *mtd, loff_t ofs, uint64_t len);
int mtd_block_isreserved(struct mtd_info *mtd, loff_t ofs);
int mtd_block_isbad(struct mtd_info *mtd, loff_t ofs);
int mtd_block_markbad(struct mtd_info *mtd, loff_t ofs);

#ifndef __UBOOT__
static inline int mtd_suspend(struct mtd_info *mtd)
{
	return mtd->_suspend ? mtd->_suspend(mtd) : 0;
}

static inline void mtd_resume(struct mtd_info *mtd)
{
	if (mtd->_resume)
		mtd->_resume(mtd);
}
#endif

static inline uint32_t mtd_div_by_eb(uint64_t sz, struct mtd_info *mtd)
{
	if (mtd->erasesize_shift)
		return sz >> mtd->erasesize_shift;
	do_div(sz, mtd->erasesize);
	return sz;
}

static inline uint32_t mtd_mod_by_eb(uint64_t sz, struct mtd_info *mtd)
{
	if (mtd->erasesize_shift)
		return sz & mtd->erasesize_mask;
	return do_div(sz, mtd->erasesize);
}

static inline uint32_t mtd_div_by_ws(uint64_t sz, struct mtd_info *mtd)
{
	if (mtd->writesize_shift)
		return sz >> mtd->writesize_shift;
	do_div(sz, mtd->writesize);
	return sz;
}

static inline uint32_t mtd_mod_by_ws(uint64_t sz, struct mtd_info *mtd)
{
	if (mtd->writesize_shift)
		return sz & mtd->writesize_mask;
	return do_div(sz, mtd->writesize);
}

static inline int mtd_has_oob(const struct mtd_info *mtd)
{
	return mtd->_read_oob && mtd->_write_oob;
}

static inline int mtd_type_is_nand(const struct mtd_info *mtd)
{
	return mtd->type == MTD_NANDFLASH || mtd->type == MTD_MLCNANDFLASH;
}

static inline int mtd_can_have_bb(const struct mtd_info *mtd)
{
	return !!mtd->_block_isbad;
}

	/* Kernel-side ioctl definitions */

struct mtd_partition;
struct mtd_part_parser_data;

extern int mtd_device_parse_register(struct mtd_info *mtd,
				     const char * const *part_probe_types,
				     struct mtd_part_parser_data *parser_data,
				     const struct mtd_partition *defparts,
				     int defnr_parts);
#define mtd_device_register(master, parts, nr_parts)	\
	mtd_device_parse_register(master, NULL, NULL, parts, nr_parts)
extern int mtd_device_unregister(struct mtd_info *master);
extern struct mtd_info *get_mtd_device(struct mtd_info *mtd, int num);
extern int __get_mtd_device(struct mtd_info *mtd);
extern void __put_mtd_device(struct mtd_info *mtd);
extern struct mtd_info *get_mtd_device_nm(const char *name);
extern void put_mtd_device(struct mtd_info *mtd);


#ifndef __UBOOT__
struct mtd_notifier {
	void (*add)(struct mtd_info *mtd);
	void (*remove)(struct mtd_info *mtd);
	struct list_head list;
};


extern void register_mtd_user (struct mtd_notifier *new);
extern int unregister_mtd_user (struct mtd_notifier *old);
#endif
void *mtd_kmalloc_up_to(const struct mtd_info *mtd, size_t *size);

#ifdef CONFIG_MTD_PARTITIONS
void mtd_erase_callback(struct erase_info *instr);
#else
static inline void mtd_erase_callback(struct erase_info *instr)
{
	if (instr->callback)
		instr->callback(instr);
}
#endif

static inline int mtd_is_bitflip(int err) {
	return err == -EUCLEAN;
}

static inline int mtd_is_eccerr(int err) {
	return err == -EBADMSG;
}

static inline int mtd_is_bitflip_or_eccerr(int err) {
	return mtd_is_bitflip(err) || mtd_is_eccerr(err);
}

unsigned mtd_mmap_capabilities(struct mtd_info *mtd);

#ifdef __UBOOT__
/* drivers/mtd/mtdcore.h */
int add_mtd_device(struct mtd_info *mtd);
int del_mtd_device(struct mtd_info *mtd);

#ifdef CONFIG_MTD_PARTITIONS
int add_mtd_partitions(struct mtd_info *, const struct mtd_partition *, int);
int del_mtd_partitions(struct mtd_info *);
#else
static inline int add_mtd_partitions(struct mtd_info *mtd,
				     const struct mtd_partition *parts,
				     int nparts)
{
	return 0;
}

static inline int del_mtd_partitions(struct mtd_info *mtd)
{
	return 0;
}
#endif

struct mtd_info *__mtd_next_device(int i);
#define mtd_for_each_device(mtd)			\
	for ((mtd) = __mtd_next_device(0);		\
	     (mtd) != NULL;				\
	     (mtd) = __mtd_next_device(mtd->index + 1))

int mtd_arg_off(const char *arg, int *idx, loff_t *off, loff_t *size,
		loff_t *maxsize, int devtype, uint64_t chipsize);
int mtd_arg_off_size(int argc, char *const argv[], int *idx, loff_t *off,
		     loff_t *size, loff_t *maxsize, int devtype,
		     uint64_t chipsize);

/* drivers/mtd/mtdcore.c */
void mtd_get_len_incl_bad(struct mtd_info *mtd, uint64_t offset,
			  const uint64_t length, uint64_t *len_incl_bad,
			  int *truncated);
bool mtd_dev_list_updated(void);

/* drivers/mtd/mtd_uboot.c */
int mtd_search_alternate_name(const char *mtdname, char *altname,
			      unsigned int max_len);

#endif
#endif /* __MTD_MTD_H__ */
/* SPDX-License-Identifier: GPL-2.0+ */
/*
 *  Copyright © 2000-2010 David Woodhouse <dwmw2@infradead.org>
 *                        Steven J. Hill <sjhill@realitydiluted.com>
 *		          Thomas Gleixner <tglx@linutronix.de>
 *
 * Info:
 *	Contains standard defines and IDs for NAND flash devices
 *
 * Changelog:
 *	See git changelog.
 */
#ifndef __LINUX_MTD_RAWNAND_H
#define __LINUX_MTD_RAWNAND_H

//#include <config.h>
//
//#include <dm/device.h>
//#include <linux/compat.h>
//#include <linux/mtd/mtd.h>
//#include <linux/mtd/flashchip.h>
//#include <linux/mtd/bbm.h>
//#include <asm/cache.h>

struct mtd_info;
struct nand_chip;
struct nand_flash_dev;
struct device_node;

/* Get the flash and manufacturer id and lookup if the type is supported. */
struct nand_flash_dev *nand_get_flash_type(struct mtd_info *mtd,
					   struct nand_chip *chip,
					   int *maf_id, int *dev_id,
					   struct nand_flash_dev *type);

/* Scan and identify a NAND device */
int nand_scan(struct mtd_info *mtd, int max_chips);
/*
 * Separate phases of nand_scan(), allowing board driver to intervene
 * and override command or ECC setup according to flash type.
 */
int nand_scan_ident(struct mtd_info *mtd, int max_chips,
			   struct nand_flash_dev *table);
int nand_scan_tail(struct mtd_info *mtd);

/* Free resources held by the NAND device */
void nand_release(struct mtd_info *mtd);

/* Internal helper for board drivers which need to override command function */
void nand_wait_ready(struct mtd_info *mtd);

/*
 * This constant declares the max. oobsize / page, which
 * is supported now. If you add a chip with bigger oobsize/page
 * adjust this accordingly.
 */
#define NAND_MAX_OOBSIZE       1664
#define NAND_MAX_PAGESIZE      16384
#define ARCH_DMA_MINALIGN 4

#define ALIGN(n, g) (n)
/*
 * Constants for hardware specific CLE/ALE/NCE function
 *
 * These are bits which can be or'ed to set/clear multiple
 * bits in one go.
 */
/* Select the chip by setting nCE to low */
#define NAND_NCE		0x01
/* Select the command latch by setting CLE to high */
#define NAND_CLE		0x02
/* Select the address latch by setting ALE to high */
#define NAND_ALE		0x04

#define NAND_CTRL_CLE		(NAND_NCE | NAND_CLE)
#define NAND_CTRL_ALE		(NAND_NCE | NAND_ALE)
#define NAND_CTRL_CHANGE	0x80

/*
 * Standard NAND flash commands
 */
#define NAND_CMD_READ0		0
#define NAND_CMD_READ1		1
#define NAND_CMD_RNDOUT		5
#define NAND_CMD_PAGEPROG	0x10
#define NAND_CMD_READOOB	0x50
#define NAND_CMD_ERASE1		0x60
#define NAND_CMD_STATUS		0x70
#define NAND_CMD_SEQIN		0x80
#define NAND_CMD_RNDIN		0x85
#define NAND_CMD_READID		0x90
#define NAND_CMD_ERASE2		0xd0
#define NAND_CMD_PARAM		0xec
#define NAND_CMD_GET_FEATURES	0xee
#define NAND_CMD_SET_FEATURES	0xef
#define NAND_CMD_RESET		0xff

#define NAND_CMD_LOCK		0x2a
#define NAND_CMD_UNLOCK1	0x23
#define NAND_CMD_UNLOCK2	0x24

/* Extended commands for large page devices */
#define NAND_CMD_READSTART	0x30
#define NAND_CMD_RNDOUTSTART	0xE0
#define NAND_CMD_CACHEDPROG	0x15

/* Extended commands for AG-AND device */
/*
 * Note: the command for NAND_CMD_DEPLETE1 is really 0x00 but
 *       there is no way to distinguish that from NAND_CMD_READ0
 *       until the remaining sequence of commands has been completed
 *       so add a high order bit and mask it off in the command.
 */
#define NAND_CMD_DEPLETE1	0x100
#define NAND_CMD_DEPLETE2	0x38
#define NAND_CMD_STATUS_MULTI	0x71
#define NAND_CMD_STATUS_ERROR	0x72
/* multi-bank error status (banks 0-3) */
#define NAND_CMD_STATUS_ERROR0	0x73
#define NAND_CMD_STATUS_ERROR1	0x74
#define NAND_CMD_STATUS_ERROR2	0x75
#define NAND_CMD_STATUS_ERROR3	0x76
#define NAND_CMD_STATUS_RESET	0x7f
#define NAND_CMD_STATUS_CLEAR	0xff

#define NAND_CMD_NONE		-1

/* Status bits */
#define NAND_STATUS_FAIL	0x01
#define NAND_STATUS_FAIL_N1	0x02
#define NAND_STATUS_TRUE_READY	0x20
#define NAND_STATUS_READY	0x40
#define NAND_STATUS_WP		0x80

#define NAND_DATA_IFACE_CHECK_ONLY	-1

/*
 * Constants for ECC_MODES
 */
typedef enum {
	NAND_ECC_NONE,
	NAND_ECC_SOFT,
	NAND_ECC_HW,
	NAND_ECC_HW_SYNDROME,
	NAND_ECC_HW_OOB_FIRST,
	NAND_ECC_SOFT_BCH,
} nand_ecc_modes_t;

enum nand_ecc_algo {
	NAND_ECC_UNKNOWN,
	NAND_ECC_HAMMING,
	NAND_ECC_BCH,
};

/*
 * Constants for Hardware ECC
 */
/* Reset Hardware ECC for read */
#define NAND_ECC_READ		0
/* Reset Hardware ECC for write */
#define NAND_ECC_WRITE		1
/* Enable Hardware ECC before syndrome is read back from flash */
#define NAND_ECC_READSYN	2

/*
 * Enable generic NAND 'page erased' check. This check is only done when
 * ecc.correct() returns -EBADMSG.
 * Set this flag if your implementation does not fix bitflips in erased
 * pages and you want to rely on the default implementation.
 */
#define NAND_ECC_GENERIC_ERASED_CHECK	NAND_BIT(0)
#define NAND_ECC_MAXIMIZE		NAND_BIT(1)
/*
 * If your controller already sends the required NAND commands when
 * reading or writing a page, then the framework is not supposed to
 * send READ0 and SEQIN/PAGEPROG respectively.
 */
#define NAND_ECC_CUSTOM_PAGE_ACCESS	NAND_BIT(2)

/* Bit mask for flags passed to do_nand_read_ecc */
#define NAND_GET_DEVICE		0x80


/*
 * Option constants for bizarre disfunctionality and real
 * features.
 */
/* Buswidth is 16 bit */
#define NAND_BUSWIDTH_16	0x00000002
/* Device supports partial programming without padding */
#define NAND_NO_PADDING		0x00000004
/* Chip has cache program function */
#define NAND_CACHEPRG		0x00000008
/* Chip has copy back function */
#define NAND_COPYBACK		0x00000010
/*
 * Chip requires ready check on read (for auto-incremented sequential read).
 * True only for small page devices; large page devices do not support
 * autoincrement.
 */
#define NAND_NEED_READRDY	0x00000100

/* Chip does not allow subpage writes */
#define NAND_NO_SUBPAGE_WRITE	0x00000200

/* Device is one of 'new' xD cards that expose fake nand command set */
#define NAND_BROKEN_XD		0x00000400

/* Device behaves just like nand, but is readonly */
#define NAND_ROM		0x00000800

/* Device supports subpage reads */
#define NAND_SUBPAGE_READ	0x00001000

/*
 * Some MLC NANDs need data scrambling to limit bitflips caused by repeated
 * patterns.
 */
#define NAND_NEED_SCRAMBLING	0x00002000

/* Device needs 3rd row address cycle */
#define NAND_ROW_ADDR_3		0x00004000

/* Options valid for Samsung large page devices */
#define NAND_SAMSUNG_LP_OPTIONS NAND_CACHEPRG

/* Macros to identify the above */
#define NAND_HAS_CACHEPROG(chip) ((chip->options & NAND_CACHEPRG))
#define NAND_HAS_SUBPAGE_READ(chip) ((chip->options & NAND_SUBPAGE_READ))
#define NAND_HAS_SUBPAGE_WRITE(chip) !((chip)->options & NAND_NO_SUBPAGE_WRITE)

/* Non chip related options */
/* This option skips the bbt scan during initialization. */
#define NAND_SKIP_BBTSCAN	0x00010000
/*
 * This option is defined if the board driver allocates its own buffers
 * (e.g. because it needs them DMA-coherent).
 */
#define NAND_OWN_BUFFERS	0x00020000
/* Chip may not exist, so silence any errors in scan */
#define NAND_SCAN_SILENT_NODEV	0x00040000
/*
 * Autodetect nand buswidth with readid/onfi.
 * This suppose the driver will configure the hardware in 8 bits mode
 * when calling nand_scan_ident, and update its configuration
 * before calling nand_scan_tail.
 */
#define NAND_BUSWIDTH_AUTO      0x00080000
/*
 * This option could be defined by controller drivers to protect against
 * kmap'ed, vmalloc'ed highmem buffers being passed from upper layers
 */
#define NAND_USE_BOUNCE_BUFFER	0x00100000

/* Options set by nand scan */
/* bbt has already been read */
#define NAND_BBT_SCANNED	0x40000000
/* Nand scan has allocated controller struct */
#define NAND_CONTROLLER_ALLOC	0x80000000

/* Cell info constants */
#define NAND_CI_CHIPNR_MSK	0x03
#define NAND_CI_CELLTYPE_MSK	0x0C
#define NAND_CI_CELLTYPE_SHIFT	2

/* ONFI features */
#define ONFI_FEATURE_16_BIT_BUS		(1 << 0)
#define ONFI_FEATURE_EXT_PARAM_PAGE	(1 << 7)

/* ONFI timing mode, used in both asynchronous and synchronous mode */
#define ONFI_TIMING_MODE_0		(1 << 0)
#define ONFI_TIMING_MODE_1		(1 << 1)
#define ONFI_TIMING_MODE_2		(1 << 2)
#define ONFI_TIMING_MODE_3		(1 << 3)
#define ONFI_TIMING_MODE_4		(1 << 4)
#define ONFI_TIMING_MODE_5		(1 << 5)
#define ONFI_TIMING_MODE_UNKNOWN	(1 << 6)

/* ONFI feature address */
#define ONFI_FEATURE_ADDR_TIMING_MODE	0x1

/* Vendor-specific feature address (Micron) */
#define ONFI_FEATURE_ADDR_READ_RETRY	0x89

/* ONFI subfeature parameters length */
#define ONFI_SUBFEATURE_PARAM_LEN	4

/* ONFI optional commands SET/GET FEATURES supported? */
#define ONFI_OPT_CMD_SET_GET_FEATURES	(1 << 2)

// Taken from
// https://github.com/elvees/u-boot/blob/1ad4d225d4b37bf562cc410ad9db87ce44628c44/include/linux/mtd/rawnand.h
// https://github.com/elvees/u-boot/blob/1ad4d225d4b37bf562cc410ad9db87ce44628c44/drivers/mtd/nand/raw/mcom02-nand.c

typedef uint16_t __le16;
typedef uint32_t __le32;
#define NAND_BIT(pos) (UINT32_C(1) << (pos))

struct nand_onfi_params {
	/* rev info and features block */
	/* 'O' 'N' 'F' 'I'  */
	uint8_t sig[4];
	__le16 revision;
	__le16 features;
	__le16 opt_cmd;
	uint8_t reserved0[2];
	__le16 ext_param_page_length; /* since ONFI 2.1 */
	uint8_t num_of_param_pages;        /* since ONFI 2.1 */
	uint8_t reserved1[17];

	/* manufacturer information block */
	char manufacturer[12];
	char model[20];
	uint8_t jedec_id;
	__le16 date_code;
	uint8_t reserved2[13];

	/* memory organization block */
	__le32 byte_per_page;
	__le16 spare_bytes_per_page;
	__le32 data_bytes_per_ppage;
	__le16 spare_bytes_per_ppage;
	__le32 pages_per_block;
	__le32 blocks_per_lun;
	uint8_t lun_count;
	uint8_t addr_cycles;
	uint8_t bits_per_cell;
	__le16 bb_per_lun;
	__le16 block_endurance;
	uint8_t guaranteed_good_blocks;
	__le16 guaranteed_block_endurance;
	uint8_t programs_per_page;
	uint8_t ppage_attr;
	uint8_t ecc_bits;
	uint8_t interleaved_bits;
	uint8_t interleaved_ops;
	uint8_t reserved3[13];

	/* electrical parameter block */
	uint8_t io_pin_capacitance_max;
	__le16 async_timing_mode;
	__le16 program_cache_timing_mode;
	__le16 t_prog;
	__le16 t_bers;
	__le16 t_r;
	__le16 t_ccs;
	__le16 src_sync_timing_mode;
	uint8_t src_ssync_features;
	__le16 clk_pin_capacitance_typ;
	__le16 io_pin_capacitance_typ;
	__le16 input_pin_capacitance_typ;
	uint8_t input_pin_capacitance_max;
	uint8_t driver_strength_support;
	__le16 t_int_r;
	__le16 t_adl;
	uint8_t reserved4[8];

	/* vendor */
	__le16 vendor_revision;
	uint8_t vendor[88];

	__le16 crc;
} __packed;

#define ONFI_CRC_BASE	0x4F4E

/* Extended ECC information Block Definition (since ONFI 2.1) */
struct onfi_ext_ecc_info {
	uint8_t ecc_bits;
	uint8_t codeword_size;
	__le16 bb_per_lun;
	__le16 block_endurance;
	uint8_t reserved[2];
} __packed;

#define ONFI_SECTION_TYPE_0	0	/* Unused section. */
#define ONFI_SECTION_TYPE_1	1	/* for additional sections. */
#define ONFI_SECTION_TYPE_2	2	/* for ECC information. */
struct onfi_ext_section {
	uint8_t type;
	uint8_t length;
} __packed;

#define ONFI_EXT_SECTION_MAX 8

/* Extended Parameter Page Definition (since ONFI 2.1) */
struct onfi_ext_param_page {
	__le16 crc;
	uint8_t sig[4];             /* 'E' 'P' 'P' 'S' */
	uint8_t reserved0[10];
	struct onfi_ext_section sections[ONFI_EXT_SECTION_MAX];

	/*
	 * The actual size of the Extended Parameter Page is in
	 * @ext_param_page_length of nand_onfi_params{}.
	 * The following are the variable length sections.
	 * So we do not add any fields below. Please see the ONFI spec.
	 */
} __packed;

struct nand_onfi_vendor_micron {
	uint8_t two_plane_read;
	uint8_t read_cache;
	uint8_t read_unique_id;
	uint8_t dq_imped;
	uint8_t dq_imped_num_settings;
	uint8_t dq_imped_feat_addr;
	uint8_t rb_pulldown_strength;
	uint8_t rb_pulldown_strength_feat_addr;
	uint8_t rb_pulldown_strength_num_settings;
	uint8_t otp_mode;
	uint8_t otp_page_start;
	uint8_t otp_data_prot_addr;
	uint8_t otp_num_pages;
	uint8_t otp_feat_addr;
	uint8_t read_retry_options;
	uint8_t reserved[72];
	uint8_t param_revision;
} __packed;

struct jedec_ecc_info {
	uint8_t ecc_bits;
	uint8_t codeword_size;
	__le16 bb_per_lun;
	__le16 block_endurance;
	uint8_t reserved[2];
} __packed;

/* JEDEC features */
#define JEDEC_FEATURE_16_BIT_BUS	(1 << 0)

struct nand_jedec_params {
	/* rev info and features block */
	/* 'J' 'E' 'S' 'D'  */
	uint8_t sig[4];
	__le16 revision;
	__le16 features;
	uint8_t opt_cmd[3];
	__le16 sec_cmd;
	uint8_t num_of_param_pages;
	uint8_t reserved0[18];

	/* manufacturer information block */
	char manufacturer[12];
	char model[20];
	uint8_t jedec_id[6];
	uint8_t reserved1[10];

	/* memory organization block */
	__le32 byte_per_page;
	__le16 spare_bytes_per_page;
	uint8_t reserved2[6];
	__le32 pages_per_block;
	__le32 blocks_per_lun;
	uint8_t lun_count;
	uint8_t addr_cycles;
	uint8_t bits_per_cell;
	uint8_t programs_per_page;
	uint8_t multi_plane_addr;
	uint8_t multi_plane_op_attr;
	uint8_t reserved3[38];

	/* electrical parameter block */
	__le16 async_sdr_speed_grade;
	__le16 toggle_ddr_speed_grade;
	__le16 sync_ddr_speed_grade;
	uint8_t async_sdr_features;
	uint8_t toggle_ddr_features;
	uint8_t sync_ddr_features;
	__le16 t_prog;
	__le16 t_bers;
	__le16 t_r;
	__le16 t_r_multi_plane;
	__le16 t_ccs;
	__le16 io_pin_capacitance_typ;
	__le16 input_pin_capacitance_typ;
	__le16 clk_pin_capacitance_typ;
	uint8_t driver_strength_support;
	__le16 t_adl;
	uint8_t reserved4[36];

	/* ECC and endurance block */
	uint8_t guaranteed_good_blocks;
	__le16 guaranteed_block_endurance;
	struct jedec_ecc_info ecc_info[4];
	uint8_t reserved5[29];

	/* reserved */
	uint8_t reserved6[148];

	/* vendor */
	__le16 vendor_rev_num;
	uint8_t reserved7[88];

	/* CRC for Parameter Page */
	__le16 crc;
} __packed;

/**
 * struct nand_hw_control - Control structure for hardware controller (e.g ECC generator) shared among independent devices
 * @lock:               protection lock
 * @active:		the mtd device which holds the controller currently
 * @wq:			wait queue to sleep on if a NAND operation is in
 *			progress used instead of the per chip wait queue
 *			when a hw controller is available.
 */
//struct nand_hw_control {
//	spinlock_t lock;
//	struct nand_chip *active;
//};
//
//static inline void nand_hw_control_init(struct nand_hw_control *nfc)
//{
//	nfc->active = NULL;
//	spin_lock_init(&nfc->lock);
//	init_waitqueue_head(&nfc->wq);
//}

/**
 * struct nand_ecc_step_info - ECC step information of ECC engine
 * @stepsize: data bytes per ECC step
 * @strengths: array of supported strengths
 * @nstrengths: number of supported strengths
 */
struct nand_ecc_step_info {
	int stepsize;
	const int *strengths;
	int nstrengths;
};

/**
 * struct nand_ecc_caps - capability of ECC engine
 * @stepinfos: array of ECC step information
 * @nstepinfos: number of ECC step information
 * @calc_ecc_bytes: driver's hook to calculate ECC bytes per step
 */
struct nand_ecc_caps {
	const struct nand_ecc_step_info *stepinfos;
	int nstepinfos;
	int (*calc_ecc_bytes)(int step_size, int strength);
};

/* a shorthand to generate struct nand_ecc_caps with only one ECC stepsize */
#define NAND_ECC_CAPS_SINGLE(__name, __calc, __step, ...)	\
static const int __name##_strengths[] = { __VA_ARGS__ };	\
static const struct nand_ecc_step_info __name##_stepinfo = {	\
	.stepsize = __step,					\
	.strengths = __name##_strengths,			\
	.nstrengths = ARRAY_SIZE(__name##_strengths),		\
};								\
static const struct nand_ecc_caps __name = {			\
	.stepinfos = &__name##_stepinfo,			\
	.nstepinfos = 1,					\
	.calc_ecc_bytes = __calc,				\
}

/**
 * struct nand_ecc_ctrl - Control structure for ECC
 * @mode:	ECC mode
 * @algo:	ECC algorithm
 * @steps:	number of ECC steps per page
 * @size:	data bytes per ECC step
 * @bytes:	ECC bytes per step
 * @strength:	max number of correctible bits per ECC step
 * @total:	total number of ECC bytes per page
 * @prepad:	padding information for syndrome based ECC generators
 * @postpad:	padding information for syndrome based ECC generators
 * @options:	ECC specific options (see NAND_ECC_XXX flags defined above)
 * @layout:	ECC layout control struct pointer
 * @priv:	pointer to private ECC control data
 * @hwctl:	function to control hardware ECC generator. Must only
 *		be provided if an hardware ECC is available
 * @calculate:	function for ECC calculation or readback from ECC hardware
 * @correct:	function for ECC correction, matching to ECC generator (sw/hw).
 *		Should return a positive number representing the number of
 *		corrected bitflips, -EBADMSG if the number of bitflips exceed
 *		ECC strength, or any other error code if the error is not
 *		directly related to correction.
 *		If -EBADMSG is returned the input buffers should be left
 *		untouched.
 * @read_page_raw:	function to read a raw page without ECC. This function
 *			should hide the specific layout used by the ECC
 *			controller and always return contiguous in-band and
 *			out-of-band data even if they're not stored
 *			contiguously on the NAND chip (e.g.
 *			NAND_ECC_HW_SYNDROME interleaves in-band and
 *			out-of-band data).
 * @write_page_raw:	function to write a raw page without ECC. This function
 *			should hide the specific layout used by the ECC
 *			controller and consider the passed data as contiguous
 *			in-band and out-of-band data. ECC controller is
 *			responsible for doing the appropriate transformations
 *			to adapt to its specific layout (e.g.
 *			NAND_ECC_HW_SYNDROME interleaves in-band and
 *			out-of-band data).
 * @read_page:	function to read a page according to the ECC generator
 *		requirements; returns maximum number of bitflips corrected in
 *		any single ECC step, 0 if bitflips uncorrectable, -EIO hw error
 * @read_subpage:	function to read parts of the page covered by ECC;
 *			returns same as read_page()
 * @write_subpage:	function to write parts of the page covered by ECC.
 * @write_page:	function to write a page according to the ECC generator
 *		requirements.
 * @write_oob_raw:	function to write chip OOB data without ECC
 * @read_oob_raw:	function to read chip OOB data without ECC
 * @read_oob:	function to read chip OOB data
 * @write_oob:	function to write chip OOB data
 */
struct nand_ecc_ctrl {
	nand_ecc_modes_t mode;
	enum nand_ecc_algo algo;
	int steps;
	int size;
	int bytes;
	int total;
	int strength;
	int prepad;
	int postpad;
	unsigned int options;
	struct nand_ecclayout	*layout;
	void *priv;
	void (*hwctl)(struct mtd_info *mtd, int mode);
	int (*calculate)(struct mtd_info *mtd, const uint8_t *dat,
			uint8_t *ecc_code);
	int (*correct)(struct mtd_info *mtd, uint8_t *dat, uint8_t *read_ecc,
			uint8_t *calc_ecc);
	int (*read_page_raw)(struct mtd_info *mtd, struct nand_chip *chip,
			uint8_t *buf, int oob_required, int page);
	int (*write_page_raw)(struct mtd_info *mtd, struct nand_chip *chip,
			const uint8_t *buf, int oob_required, int page);
	int (*read_page)(struct mtd_info *mtd, struct nand_chip *chip,
			uint8_t *buf, int oob_required, int page);
	int (*read_subpage)(struct mtd_info *mtd, struct nand_chip *chip,
			uint32_t offs, uint32_t len, uint8_t *buf, int page);
	int (*write_subpage)(struct mtd_info *mtd, struct nand_chip *chip,
			uint32_t offset, uint32_t data_len,
			const uint8_t *data_buf, int oob_required, int page);
	int (*write_page)(struct mtd_info *mtd, struct nand_chip *chip,
			const uint8_t *buf, int oob_required, int page);
	int (*write_oob_raw)(struct mtd_info *mtd, struct nand_chip *chip,
			int page);
	int (*read_oob_raw)(struct mtd_info *mtd, struct nand_chip *chip,
			int page);
	int (*read_oob)(struct mtd_info *mtd, struct nand_chip *chip, int page);
	int (*write_oob)(struct mtd_info *mtd, struct nand_chip *chip,
			int page);
};

static inline int nand_standard_page_accessors(struct nand_ecc_ctrl *ecc)
{
	return !(ecc->options & NAND_ECC_CUSTOM_PAGE_ACCESS);
}

/**
 * struct nand_buffers - buffer structure for read/write
 * @ecccalc:	buffer pointer for calculated ECC, size is oobsize.
 * @ecccode:	buffer pointer for ECC read from flash, size is oobsize.
 * @databuf:	buffer pointer for data, size is (page size + oobsize).
 *
 * Do not change the order of buffers. databuf and oobrbuf must be in
 * consecutive order.
 */
struct nand_buffers {
	uint8_t	ecccalc[ALIGN(NAND_MAX_OOBSIZE, ARCH_DMA_MINALIGN)];
	uint8_t	ecccode[ALIGN(NAND_MAX_OOBSIZE, ARCH_DMA_MINALIGN)];
	uint8_t databuf[ALIGN(NAND_MAX_PAGESIZE + NAND_MAX_OOBSIZE,
			      ARCH_DMA_MINALIGN)];
};

/**
 * struct nand_sdr_timings - SDR NAND chip timings
 *
 * This struct defines the timing requirements of a SDR NAND chip.
 * These information can be found in every NAND datasheets and the timings
 * meaning are described in the ONFI specifications:
 * www.onfi.org/~/media/ONFI/specs/onfi_3_1_spec.pdf (chapter 4.15 Timing
 * Parameters)
 *
 * All these timings are expressed in picoseconds.
 *
 * @tBERS_max: Block erase time
 * @tCCS_min: Change column setup time
 * @tPROG_max: Page program time
 * @tR_max: Page read time
 * @tALH_min: ALE hold time
 * @tADL_min: ALE to data loading time
 * @tALS_min: ALE setup time
 * @tAR_min: ALE to RE# delay
 * @tCEA_max: CE# access time
 * @tCEH_min: CE# high hold time
 * @tCH_min:  CE# hold time
 * @tCHZ_max: CE# high to output hi-Z
 * @tCLH_min: CLE hold time
 * @tCLR_min: CLE to RE# delay
 * @tCLS_min: CLE setup time
 * @tCOH_min: CE# high to output hold
 * @tCS_min: CE# setup time
 * @tDH_min: Data hold time
 * @tDS_min: Data setup time
 * @tFEAT_max: Busy time for Set Features and Get Features
 * @tIR_min: Output hi-Z to RE# low
 * @tITC_max: Interface and Timing Mode Change time
 * @tRC_min: RE# cycle time
 * @tREA_max: RE# access time
 * @tREH_min: RE# high hold time
 * @tRHOH_min: RE# high to output hold
 * @tRHW_min: RE# high to WE# low
 * @tRHZ_max: RE# high to output hi-Z
 * @tRLOH_min: RE# low to output hold
 * @tRP_min: RE# pulse width
 * @tRR_min: Ready to RE# low (data only)
 * @tRST_max: Device reset time, measured from the falling edge of R/B# to the
 *	      rising edge of R/B#.
 * @tWB_max: WE# high to SR[6] low
 * @tWC_min: WE# cycle time
 * @tWH_min: WE# high hold time
 * @tWHR_min: WE# high to RE# low
 * @tWP_min: WE# pulse width
 * @tWW_min: WP# transition to WE# low
 */
struct nand_sdr_timings {
	uint64_t tBERS_max;
	uint32_t tCCS_min;
	uint64_t tPROG_max;
	uint64_t tR_max;
	uint32_t tALH_min;
	uint32_t tADL_min;
	uint32_t tALS_min;
	uint32_t tAR_min;
	uint32_t tCEA_max;
	uint32_t tCEH_min;
	uint32_t tCH_min;
	uint32_t tCHZ_max;
	uint32_t tCLH_min;
	uint32_t tCLR_min;
	uint32_t tCLS_min;
	uint32_t tCOH_min;
	uint32_t tCS_min;
	uint32_t tDH_min;
	uint32_t tDS_min;
	uint32_t tFEAT_max;
	uint32_t tIR_min;
	uint32_t tITC_max;
	uint32_t tRC_min;
	uint32_t tREA_max;
	uint32_t tREH_min;
	uint32_t tRHOH_min;
	uint32_t tRHW_min;
	uint32_t tRHZ_max;
	uint32_t tRLOH_min;
	uint32_t tRP_min;
	uint32_t tRR_min;
	uint64_t tRST_max;
	uint32_t tWB_max;
	uint32_t tWC_min;
	uint32_t tWH_min;
	uint32_t tWHR_min;
	uint32_t tWP_min;
	uint32_t tWW_min;
};

#define EINVAL (1)
#define ENODEV (2)
#define ENXIO	(3)
#define ETIMEDOUT (4)

#define ERR_PTR(p) ((void *) (p))
/**
 * enum nand_data_interface_type - NAND interface timing type
 * @NAND_SDR_IFACE:	Single Data Rate interface
 */
enum nand_data_interface_type {
	NAND_SDR_IFACE,
};

/**
 * struct nand_data_interface - NAND interface timing
 * @type:	type of the timing
 * @timings:	The timing, type according to @type
 */
struct nand_data_interface {
	enum nand_data_interface_type type;
	union {
		struct nand_sdr_timings sdr;
	} timings;
};

/**
 * nand_get_sdr_timings - get SDR timing from data interface
 * @conf:	The data interface
 */
static inline const struct nand_sdr_timings *
nand_get_sdr_timings(const struct nand_data_interface *conf)
{
	if (conf->type != NAND_SDR_IFACE)
		return ERR_PTR(-EINVAL);

	return &conf->timings.sdr;
}

/**
 * struct nand_chip - NAND Private Flash Chip Data
 * @mtd:		MTD device registered to the MTD framework
 * @IO_ADDR_R:		[BOARDSPECIFIC] address to read the 8 I/O lines of the
 *			flash device
 * @IO_ADDR_W:		[BOARDSPECIFIC] address to write the 8 I/O lines of the
 *			flash device.
 * @flash_node:		[BOARDSPECIFIC] device node describing this instance
 * @read_byte:		[REPLACEABLE] read one byte from the chip
 * @read_word:		[REPLACEABLE] read one word from the chip
 * @write_byte:		[REPLACEABLE] write a single byte to the chip on the
 *			low 8 I/O lines
 * @write_buf:		[REPLACEABLE] write data from the buffer to the chip
 * @read_buf:		[REPLACEABLE] read data from the chip into the buffer
 * @select_chip:	[REPLACEABLE] select chip nr
 * @block_bad:		[REPLACEABLE] check if a block is bad, using OOB markers
 * @block_markbad:	[REPLACEABLE] mark a block bad
 * @cmd_ctrl:		[BOARDSPECIFIC] hardwarespecific function for controlling
 *			ALE/CLE/nCE. Also used to write command and address
 * @dev_ready:		[BOARDSPECIFIC] hardwarespecific function for accessing
 *			device ready/busy line. If set to NULL no access to
 *			ready/busy is available and the ready/busy information
 *			is read from the chip status register.
 * @cmdfunc:		[REPLACEABLE] hardwarespecific function for writing
 *			commands to the chip.
 * @waitfunc:		[REPLACEABLE] hardwarespecific function for wait on
 *			ready.
 * @setup_read_retry:	[FLASHSPECIFIC] flash (vendor) specific function for
 *			setting the read-retry mode. Mostly needed for MLC NAND.
 * @ecc:		[BOARDSPECIFIC] ECC control structure
 * @buffers:		buffer structure for read/write
 * @buf_align:		minimum buffer alignment required by a platform
 * @hwcontrol:		platform-specific hardware control structure
 * @erase:		[REPLACEABLE] erase function
 * @scan_bbt:		[REPLACEABLE] function to scan bad block table
 * @chip_delay:		[BOARDSPECIFIC] chip dependent delay for transferring
 *			data from array to read regs (tR).
 * @state:		[INTERN] the current state of the NAND device
 * @oob_poi:		"poison value buffer," used for laying out OOB data
 *			before writing
 * @page_shift:		[INTERN] number of address bits in a page (column
 *			address bits).
 * @phys_erase_shift:	[INTERN] number of address bits in a physical eraseblock
 * @bbt_erase_shift:	[INTERN] number of address bits in a bbt entry
 * @chip_shift:		[INTERN] number of address bits in one chip
 * @options:		[BOARDSPECIFIC] various chip options. They can partly
 *			be set to inform nand_scan about special functionality.
 *			See the defines for further explanation.
 * @bbt_options:	[INTERN] bad block specific options. All options used
 *			here must come from bbm.h. By default, these options
 *			will be copied to the appropriate nand_bbt_descr's.
 * @badblockpos:	[INTERN] position of the bad block marker in the oob
 *			area.
 * @badblockbits:	[INTERN] minimum number of set bits in a good block's
 *			bad block marker position; i.e., BBM == 11110111b is
 *			not bad when badblockbits == 7
 * @bits_per_cell:	[INTERN] number of bits per cell. i.e., 1 means SLC.
 * @ecc_strength_ds:	[INTERN] ECC correctability from the datasheet.
 *			Minimum amount of bit errors per @ecc_step_ds guaranteed
 *			to be correctable. If unknown, set to zero.
 * @ecc_step_ds:	[INTERN] ECC step required by the @ecc_strength_ds,
 *                      also from the datasheet. It is the recommended ECC step
 *			size, if known; if unknown, set to zero.
 * @onfi_timing_mode_default: [INTERN] default ONFI timing mode. This field is
 *			      set to the actually used ONFI mode if the chip is
 *			      ONFI compliant or deduced from the datasheet if
 *			      the NAND chip is not ONFI compliant.
 * @numchips:		[INTERN] number of physical chips
 * @chipsize:		[INTERN] the size of one chip for multichip arrays
 * @pagemask:		[INTERN] page number mask = number of (pages / chip) - 1
 * @pagebuf:		[INTERN] holds the pagenumber which is currently in
 *			data_buf.
 * @pagebuf_bitflips:	[INTERN] holds the bitflip count for the page which is
 *			currently in data_buf.
 * @subpagesize:	[INTERN] holds the subpagesize
 * @onfi_version:	[INTERN] holds the chip ONFI version (BCD encoded),
 *			non 0 if ONFI supported.
 * @jedec_version:	[INTERN] holds the chip JEDEC version (BCD encoded),
 *			non 0 if JEDEC supported.
 * @onfi_params:	[INTERN] holds the ONFI page parameter when ONFI is
 *			supported, 0 otherwise.
 * @jedec_params:	[INTERN] holds the JEDEC parameter page when JEDEC is
 *			supported, 0 otherwise.
 * @read_retries:	[INTERN] the number of read retry modes supported
 * @onfi_set_features:	[REPLACEABLE] set the features for ONFI nand
 * @onfi_get_features:	[REPLACEABLE] get the features for ONFI nand
 * @setup_data_interface: [OPTIONAL] setup the data interface and timing. If
 *			  chipnr is set to %NAND_DATA_IFACE_CHECK_ONLY this
 *			  means the configuration should not be applied but
 *			  only checked.
 * @bbt:		[INTERN] bad block table pointer
 * @bbt_td:		[REPLACEABLE] bad block table descriptor for flash
 *			lookup.
 * @bbt_md:		[REPLACEABLE] bad block table mirror descriptor
 * @badblock_pattern:	[REPLACEABLE] bad block scan pattern used for initial
 *			bad block scan.
 * @controller:		[REPLACEABLE] a pointer to a hardware controller
 *			structure which is shared among multiple independent
 *			devices.
 * @priv:		[OPTIONAL] pointer to private chip data
 * @write_page:		[REPLACEABLE] High-level page write function
 */

struct nand_chip {
	struct mtd_info mtd;
	void /* __iomem */ *IO_ADDR_R;
	void /* __iomem */ *IO_ADDR_W;

	int flash_node;

	uint8_t (*read_byte)(struct mtd_info *mtd);
	uint16_t (*read_word)(struct mtd_info *mtd);
	void (*write_byte)(struct mtd_info *mtd, uint8_t byte);
	void (*write_buf)(struct mtd_info *mtd, const uint8_t *buf, int len);
	void (*read_buf)(struct mtd_info *mtd, uint8_t *buf, int len);
	void (*select_chip)(struct mtd_info *mtd, int chip);
	int (*block_bad)(struct mtd_info *mtd, loff_t ofs);
	int (*block_markbad)(struct mtd_info *mtd, loff_t ofs);
	void (*cmd_ctrl)(struct mtd_info *mtd, int dat, unsigned int ctrl);
	int (*dev_ready)(struct mtd_info *mtd);
	void (*cmdfunc)(struct mtd_info *mtd, unsigned command, int column,
			int page_addr);
	int(*waitfunc)(struct mtd_info *mtd, struct nand_chip *this);
	int (*erase)(struct mtd_info *mtd, int page);
	int (*scan_bbt)(struct mtd_info *mtd);
	int (*write_page)(struct mtd_info *mtd, struct nand_chip *chip,
			uint32_t offset, int data_len, const uint8_t *buf,
			int oob_required, int page, int raw);
	int (*onfi_set_features)(struct mtd_info *mtd, struct nand_chip *chip,
			int feature_addr, uint8_t *subfeature_para);
	int (*onfi_get_features)(struct mtd_info *mtd, struct nand_chip *chip,
			int feature_addr, uint8_t *subfeature_para);
	int (*setup_read_retry)(struct mtd_info *mtd, int retry_mode);
	int (*setup_data_interface)(struct mtd_info *mtd, int chipnr,
				    const struct nand_data_interface *conf);


	int chip_delay;
	unsigned int options;
	unsigned int bbt_options;

	int page_shift;
	int phys_erase_shift;
	int bbt_erase_shift;
	int chip_shift;
	int numchips;
	uint64_t chipsize;
	int pagemask;
	int pagebuf;
	unsigned int pagebuf_bitflips;
	int subpagesize;
	uint8_t bits_per_cell;
	uint16_t ecc_strength_ds;
	uint16_t ecc_step_ds;
	int onfi_timing_mode_default;
	int badblockpos;
	int badblockbits;

	int onfi_version;
	int jedec_version;
	struct nand_onfi_params	onfi_params;
	struct nand_jedec_params jedec_params;

	struct nand_data_interface *data_interface;

	int read_retries;

	flstate_t state;

	uint8_t *oob_poi;
	struct nand_hw_control *controller;
	struct nand_ecclayout *ecclayout;

	struct nand_ecc_ctrl ecc;
	struct nand_buffers *buffers;
	unsigned long buf_align;
	struct nand_hw_control hwcontrol;

	uint8_t *bbt;
	struct nand_bbt_descr *bbt_td;
	struct nand_bbt_descr *bbt_md;

	struct nand_bbt_descr *badblock_pattern;

	void *priv;
};

static inline void nand_set_flash_node(struct nand_chip *chip,
				       ofnode node)
{
	chip->flash_node = ofnode_to_offset(node);
}

static inline ofnode nand_get_flash_node(struct nand_chip *chip)
{
	return offset_to_ofnode(chip->flash_node);
}

static inline struct nand_chip *mtd_to_nand(struct mtd_info *mtd)
{
	return container_of(mtd, struct nand_chip, mtd);
}

static inline struct mtd_info *nand_to_mtd(struct nand_chip *chip)
{
	return &chip->mtd;
}

static inline void *nand_get_controller_data(struct nand_chip *chip)
{
	return chip->priv;
}

static inline void nand_set_controller_data(struct nand_chip *chip, void *priv)
{
	chip->priv = priv;
}

/*
 * NAND Flash Manufacturer ID Codes
 */
#define NAND_MFR_TOSHIBA	0x98
#define NAND_MFR_SAMSUNG	0xec
#define NAND_MFR_FUJITSU	0x04
#define NAND_MFR_NATIONAL	0x8f
#define NAND_MFR_RENESAS	0x07
#define NAND_MFR_STMICRO	0x20
#define NAND_MFR_HYNIX		0xad
#define NAND_MFR_MICRON		0x2c
#define NAND_MFR_AMD		0x01
#define NAND_MFR_MACRONIX	0xc2
#define NAND_MFR_EON		0x92
#define NAND_MFR_SANDISK	0x45
#define NAND_MFR_INTEL		0x89
#define NAND_MFR_ATO		0x9b

/* The maximum expected count of bytes in the NAND ID sequence */
#define NAND_MAX_ID_LEN 8

/*
 * A helper for defining older NAND chips where the second ID byte fully
 * defined the chip, including the geometry (chip size, eraseblock size, page
 * size). All these chips have 512 bytes NAND page size.
 */
#define LEGACY_ID_NAND(nm, devid, chipsz, erasesz, opts)          \
	{ .name = (nm), {{ .dev_id = (devid) }}, .pagesize = 512, \
	  .chipsize = (chipsz), .erasesize = (erasesz), .options = (opts) }

/*
 * A helper for defining newer chips which report their page size and
 * eraseblock size via the extended ID bytes.
 *
 * The real difference between LEGACY_ID_NAND and EXTENDED_ID_NAND is that with
 * EXTENDED_ID_NAND, manufacturers overloaded the same device ID so that the
 * device ID now only represented a particular total chip size (and voltage,
 * buswidth), and the page size, eraseblock size, and OOB size could vary while
 * using the same device ID.
 */
#define EXTENDED_ID_NAND(nm, devid, chipsz, opts)                      \
	{ .name = (nm), {{ .dev_id = (devid) }}, .chipsize = (chipsz), \
	  .options = (opts) }

#define NAND_ECC_INFO(_strength, _step)	\
			{ .strength_ds = (_strength), .step_ds = (_step) }
#define NAND_ECC_STRENGTH(type)		((type)->ecc.strength_ds)
#define NAND_ECC_STEP(type)		((type)->ecc.step_ds)

/**
 * struct nand_flash_dev - NAND Flash Device ID Structure
 * @name: a human-readable name of the NAND chip
 * @dev_id: the device ID (the second byte of the full chip ID array)
 * @mfr_id: manufecturer ID part of the full chip ID array (refers the same
 *          memory address as @id[0])
 * @dev_id: device ID part of the full chip ID array (refers the same memory
 *          address as @id[1])
 * @id: full device ID array
 * @pagesize: size of the NAND page in bytes; if 0, then the real page size (as
 *            well as the eraseblock size) is determined from the extended NAND
 *            chip ID array)
 * @chipsize: total chip size in MiB
 * @erasesize: eraseblock size in bytes (determined from the extended ID if 0)
 * @options: stores various chip bit options
 * @id_len: The valid length of the @id.
 * @oobsize: OOB size
 * @ecc: ECC correctability and step information from the datasheet.
 * @ecc.strength_ds: The ECC correctability from the datasheet, same as the
 *                   @ecc_strength_ds in nand_chip{}.
 * @ecc.step_ds: The ECC step required by the @ecc.strength_ds, same as the
 *               @ecc_step_ds in nand_chip{}, also from the datasheet.
 *               For example, the "4bit ECC for each 512Byte" can be set with
 *               NAND_ECC_INFO(4, 512).
 * @onfi_timing_mode_default: the default ONFI timing mode entered after a NAND
 *			      reset. Should be deduced from timings described
 *			      in the datasheet.
 *
 */
struct nand_flash_dev {
	char *name;
	union {
		struct {
			uint8_t mfr_id;
			uint8_t dev_id;
		};
		uint8_t id[NAND_MAX_ID_LEN];
	};
	unsigned int pagesize;
	unsigned int chipsize;
	unsigned int erasesize;
	unsigned int options;
	uint16_t id_len;
	uint16_t oobsize;
	struct {
		uint16_t strength_ds;
		uint16_t step_ds;
	} ecc;
	int onfi_timing_mode_default;
};

/**
 * struct nand_manufacturers - NAND Flash Manufacturer ID Structure
 * @name:	Manufacturer name
 * @id:		manufacturer ID code of device.
*/
struct nand_manufacturers {
	int id;
	char *name;
};

extern struct nand_flash_dev nand_flash_ids[];
extern struct nand_manufacturers nand_manuf_ids[];

int nand_default_bbt(struct mtd_info *mtd);
int nand_markbad_bbt(struct mtd_info *mtd, loff_t offs);
int nand_isreserved_bbt(struct mtd_info *mtd, loff_t offs);
int nand_isbad_bbt(struct mtd_info *mtd, loff_t offs, int allowbbt);
int nand_erase_nand(struct mtd_info *mtd, struct erase_info *instr,
			   int allowbbt);
int nand_do_read(struct mtd_info *mtd, loff_t from, size_t len,
			size_t *retlen, uint8_t *buf);

/*
* Constants for oob configuration
*/
#define NAND_SMALL_BADBLOCK_POS		5
#define NAND_LARGE_BADBLOCK_POS		0

/**
 * struct platform_nand_chip - chip level device structure
 * @nr_chips:		max. number of chips to scan for
 * @chip_offset:	chip number offset
 * @nr_partitions:	number of partitions pointed to by partitions (or zero)
 * @partitions:		mtd partition list
 * @chip_delay:		R/B delay value in us
 * @options:		Option flags, e.g. 16bit buswidth
 * @bbt_options:	BBT option flags, e.g. NAND_BBT_USE_FLASH
 * @part_probe_types:	NULL-terminated array of probe types
 */
struct platform_nand_chip {
	int nr_chips;
	int chip_offset;
	int nr_partitions;
	struct mtd_partition *partitions;
	int chip_delay;
	unsigned int options;
	unsigned int bbt_options;
	const char **part_probe_types;
};

/* Keep gcc happy */
struct platform_device;

/**
 * struct platform_nand_ctrl - controller level device structure
 * @probe:		platform specific function to probe/setup hardware
 * @remove:		platform specific function to remove/teardown hardware
 * @hwcontrol:		platform specific hardware control structure
 * @dev_ready:		platform specific function to read ready/busy pin
 * @select_chip:	platform specific chip select function
 * @cmd_ctrl:		platform specific function for controlling
 *			ALE/CLE/nCE. Also used to write command and address
 * @write_buf:		platform specific function for write buffer
 * @read_buf:		platform specific function for read buffer
 * @read_byte:		platform specific function to read one byte from chip
 * @priv:		private data to transport driver specific settings
 *
 * All fields are optional and depend on the hardware driver requirements
 */
struct platform_nand_ctrl {
	int (*probe)(struct platform_device *pdev);
	void (*remove)(struct platform_device *pdev);
	void (*hwcontrol)(struct mtd_info *mtd, int cmd);
	int (*dev_ready)(struct mtd_info *mtd);
	void (*select_chip)(struct mtd_info *mtd, int chip);
	void (*cmd_ctrl)(struct mtd_info *mtd, int dat, unsigned int ctrl);
	void (*write_buf)(struct mtd_info *mtd, const uint8_t *buf, int len);
	void (*read_buf)(struct mtd_info *mtd, uint8_t *buf, int len);
	unsigned char (*read_byte)(struct mtd_info *mtd);
	void *priv;
};

/**
 * struct platform_nand_data - container structure for platform-specific data
 * @chip:		chip level chip structure
 * @ctrl:		controller level device structure
 */
struct platform_nand_data {
	struct platform_nand_chip chip;
	struct platform_nand_ctrl ctrl;
};

#ifdef CONFIG_SYS_NAND_ONFI_DETECTION
/* return the supported features. */
static inline int onfi_feature(struct nand_chip *chip)
{
	return chip->onfi_version ? le16_to_cpu(chip->onfi_params.features) : 0;
}

/* return the supported asynchronous timing mode. */
static inline int onfi_get_async_timing_mode(struct nand_chip *chip)
{
	if (!chip->onfi_version)
		return ONFI_TIMING_MODE_UNKNOWN;
	return le16_to_cpu(chip->onfi_params.async_timing_mode);
}

/* return the supported synchronous timing mode. */
static inline int onfi_get_sync_timing_mode(struct nand_chip *chip)
{
	if (!chip->onfi_version)
		return ONFI_TIMING_MODE_UNKNOWN;
	return le16_to_cpu(chip->onfi_params.src_sync_timing_mode);
}
#else
static inline int onfi_feature(struct nand_chip *chip)
{
	return 0;
}

static inline int onfi_get_async_timing_mode(struct nand_chip *chip)
{
	return ONFI_TIMING_MODE_UNKNOWN;
}

static inline int onfi_get_sync_timing_mode(struct nand_chip *chip)
{
	return ONFI_TIMING_MODE_UNKNOWN;
}
#endif

int onfi_init_data_interface(struct nand_chip *chip,
			     struct nand_data_interface *iface,
			     enum nand_data_interface_type type,
			     int timing_mode);

/*
 * Check if it is a SLC nand.
 * The !nand_is_slc() can be used to check the MLC/TLC nand chips.
 * We do not distinguish the MLC and TLC now.
 */
static inline int nand_is_slc(struct nand_chip *chip)
{
	return chip->bits_per_cell == 1;
}

/**
 * Check if the opcode's address should be sent only on the lower 8 bits
 * @command: opcode to check
 */
static inline int nand_opcode_8bits(unsigned int command)
{
	switch (command) {
	case NAND_CMD_READID:
	case NAND_CMD_PARAM:
	case NAND_CMD_GET_FEATURES:
	case NAND_CMD_SET_FEATURES:
		return 1;
	default:
		break;
	}
	return 0;
}

/* return the supported JEDEC features. */
static inline int jedec_feature(struct nand_chip *chip)
{
	return chip->jedec_version ? le16_to_cpu(chip->jedec_params.features)
		: 0;
}

/* Standard NAND functions from nand_base.c */
void nand_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len);
void nand_write_buf16(struct mtd_info *mtd, const uint8_t *buf, int len);
void nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len);
void nand_read_buf16(struct mtd_info *mtd, uint8_t *buf, int len);
uint8_t nand_read_byte(struct mtd_info *mtd);

/* get timing characteristics from ONFI timing mode. */
const struct nand_sdr_timings *onfi_async_timing_mode_to_sdr_timings(int mode);
/* get data interface from ONFI timing mode 0, used after reset. */
const struct nand_data_interface *nand_get_default_data_interface(void);

int nand_check_erased_ecc_chunk(void *data, int datalen,
				void *ecc, int ecclen,
				void *extraoob, int extraooblen,
				int threshold);

int nand_check_ecc_caps(struct nand_chip *chip,
			const struct nand_ecc_caps *caps, int oobavail);

int nand_match_ecc_req(struct nand_chip *chip,
		       const struct nand_ecc_caps *caps,  int oobavail);

int nand_maximize_ecc(struct nand_chip *chip,
		      const struct nand_ecc_caps *caps, int oobavail);

/* Reset and initialize a NAND device */
int nand_reset(struct nand_chip *chip, int chipnr);

/* NAND operation helpers */
int nand_reset_op(struct nand_chip *chip);
int nand_readid_op(struct nand_chip *chip, uint8_t addr, void *buf,
		   unsigned int len);
int nand_status_op(struct nand_chip *chip, uint8_t *status);
int nand_exit_status_op(struct nand_chip *chip);
int nand_erase_op(struct nand_chip *chip, unsigned int eraseblock);
int nand_read_page_op(struct nand_chip *chip, unsigned int page,
		      unsigned int offset_in_page, void *buf, unsigned int len);
int nand_change_read_column_op(struct nand_chip *chip,
			       unsigned int offset_in_page, void *buf,
			       unsigned int len, int force_8bit);
int nand_read_oob_op(struct nand_chip *chip, unsigned int page,
		     unsigned int offset_in_page, void *buf, unsigned int len);
int nand_prog_page_begin_op(struct nand_chip *chip, unsigned int page,
			    unsigned int offset_in_page, const void *buf,
			    unsigned int len);
int nand_prog_page_end_op(struct nand_chip *chip);
int nand_prog_page_op(struct nand_chip *chip, unsigned int page,
		      unsigned int offset_in_page, const void *buf,
		      unsigned int len);
int nand_change_write_column_op(struct nand_chip *chip,
				unsigned int offset_in_page, const void *buf,
				unsigned int len, int force_8bit);
int nand_read_data_op(struct nand_chip *chip, void *buf, unsigned int len,
		      int force_8bit);
int nand_write_data_op(struct nand_chip *chip, const void *buf,
		       unsigned int len, int force_8bit);

#endif /* __LINUX_MTD_RAWNAND_H */

// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2014 - 2015 Xilinx, Inc.
 * Copyright (C) 2015 ELVEES NeoTek, CJSC
 * Copyright 2018 RnD Center "ELVEES", JSC
 * Based on the Linux version of Arasan NFC driver.
 *
 */

//#include <asm/io.h>
//#include <common.h>
//#include <dm.h>
//#include <fdtdec.h>
//#include <nand.h>
//#include <linux/errno.h>

/* Register offsets */
#define PKT_OFST			0x00
#define MEM_ADDR1_OFST			0x04
#define MEM_ADDR2_OFST			0x08
#define CMD_OFST			0x0C
#define PROG_OFST			0x10
#define INTR_STS_EN_OFST		0x14
#define INTR_SIG_EN_OFST		0x18
#define INTR_STS_OFST			0x1C
#define ID1_OFST			0x20
#define ID2_OFST			0x24
#define FLASH_STS_OFST			0x28
#define DATA_PORT_OFST			0x30
#define ECC_OFST			0x34
#define ECC_ERR_CNT_OFST		0x38
#define ECC_SPR_CMD_OFST		0x3C
#define ECC_ERR_CNT_1BIT_OFST		0x40
#define ECC_ERR_CNT_2BIT_OFST		0x44

#define PKT_CNT_SHIFT			12

#define ECC_ENABLE			NAND_BIT(31)
#define PAGE_SIZE_MASK			GENMASK(25, 23)
#define PAGE_SIZE_SHIFT			23
#define PAGE_SIZE_512			0
#define PAGE_SIZE_2K			1
#define PAGE_SIZE_4K			2
#define PAGE_SIZE_8K			3
#define CMD2_SHIFT			8
#define ADDR_CYCLES_SHIFT		28

#define XFER_COMPLETE			NAND_BIT(2)
#define READ_READY			NAND_BIT(1)
#define WRITE_READY			NAND_BIT(0)
#define MBIT_ERROR			NAND_BIT(3)
#define ERR_INTRPT			NAND_BIT(4)

#define PROG_PGRD			NAND_BIT(0)
#define PROG_ERASE			NAND_BIT(2)
#define PROG_STATUS			NAND_BIT(3)
#define PROG_PGPROG			NAND_BIT(4)
#define PROG_RDID			NAND_BIT(6)
#define PROG_RDPARAM			NAND_BIT(7)
#define PROG_RST			NAND_BIT(8)

#define ONFI_STATUS_FAIL		NAND_BIT(0)
#define ONFI_STATUS_READY		NAND_BIT(6)

#define PG_ADDR_SHIFT			16
#define BCH_MODE_SHIFT			25
#define BCH_EN_SHIFT			25
#define ECC_SIZE_SHIFT			16

#define MEM_ADDR_MASK			GENMASK(7, 0)
#define BCH_MODE_MASK			GENMASK(27, 25)

#define CS_MASK				GENMASK(31, 30)
#define CS_SHIFT			30

#define PAGE_ERR_CNT_MASK		GENMASK(16, 8)
#define PAGE_ERR_CNT_SHIFT              8
#define PKT_ERR_CNT_MASK		GENMASK(7, 0)

#define ONFI_ID_ADDR			0x20
#define ONFI_ID_LEN			4
#define MAF_ID_LEN			5
#define DMA_BUFSIZE			SZ_64K
#define TEMP_BUF_SIZE			512
#define SPARE_ADDR_CYCLES		NAND_BIT(29)

#define ARASAN_NAND_POLL_TIMEOUT	1000000
#define STATUS_TIMEOUT			2000

DECLARE_GLOBAL_DATA_PTR;

struct mcom02_nand_priv {
	struct udevice *dev;
	struct nand_chip nand;

	void /* __iomem */ *regs;

	uint8_t buf[TEMP_BUF_SIZE];

	int bch;
	int err;
	int iswriteoob;
	uint16_t raddr_cycles;
	uint16_t caddr_cycles;
	uint32_t page;
	uint32_t bufshift;
	uint32_t rdintrmask;
	uint32_t pktsize;
	uint32_t ecc_regval;

	int curr_cmd;
	struct nand_ecclayout ecclayout;
};

struct mcom02_nand_ecc_matrix {
	uint32_t pagesize;
	uint32_t codeword_size;
	uint8_t eccbits;
	uint8_t bch;
	uint16_t eccsize;
};

static const struct mcom02_nand_ecc_matrix ecc_matrix[] = {
	{512,	512,	1,	0,	0x3},
	{512,	512,	4,	1,	0x7},
	{512,	512,	8,	1,	0xD},
	/* 2K byte page */
	{2048,	512,	1,	0,	0xC},
	{2048,	512,	8,	1,	0x34},
	{2048,	512,	12,	1,	0x4E},
	{2048,	512,	16,	1,	0x68},
	{2048,	1024,	24,	1,	0x54},
	/* 4K byte page */
	{4096,	512,	1,	0,	0x18},
	{4096,	512,	8,	1,	0x68},
	{4096,	512,	16,	1,	0xD0},
	/* 8K byte page */
	{8192,	512,	1,	0,	0x30},
	{8192,	512,	8,	1,	0xD0},
	{8192,	512,	12,	1,	0x138},
	{8192,	512,	16,	1,	0x1A0},
};

static uint8_t mcom02_nand_page(uint32_t pagesize)
{
	switch (pagesize) {
	case 512:
		return PAGE_SIZE_512;
	case 2048:
		return PAGE_SIZE_2K;
	case 4096:
		return PAGE_SIZE_4K;
	case 8192:
		return PAGE_SIZE_8K;
	default:
		printf("Unsupported page size: %#x\n", pagesize);
		break;
	}

	return 0;
}

static void mcom02_nand_prepare_cmd(struct mcom02_nand_priv *priv, uint8_t cmd1,
				    uint8_t cmd2, uint32_t pagesize, uint8_t addrcycles)
{
	uint32_t regval;

	regval = cmd1 | (cmd2 << CMD2_SHIFT);
	if (addrcycles)
		regval |= addrcycles << ADDR_CYCLES_SHIFT;
	if (pagesize)
		regval |= mcom02_nand_page(pagesize) << PAGE_SIZE_SHIFT;
	writel(regval, priv->regs + CMD_OFST);
}

static void mcom02_nand_setpagecoladdr(struct mcom02_nand_priv *priv,
				       uint32_t page, uint16_t col)
{
	uint32_t val;

	writel(col | (page << PG_ADDR_SHIFT), priv->regs + MEM_ADDR1_OFST);

	val = readl(priv->regs + MEM_ADDR2_OFST);
	val = (val & ~MEM_ADDR_MASK) |
	      ((page >> PG_ADDR_SHIFT) & MEM_ADDR_MASK);
	writel(val, priv->regs + MEM_ADDR2_OFST);
}

static inline void mcom02_nand_setpktszcnt(struct mcom02_nand_priv *priv,
					   uint32_t pktsize, uint32_t pktcount)
{
	writel(pktsize | (pktcount << PKT_CNT_SHIFT), priv->regs + PKT_OFST);
}

static inline void mcom02_nand_set_irq_masks(struct mcom02_nand_priv *priv,
					     uint32_t val)
{
	writel(val, priv->regs + INTR_STS_EN_OFST);
}

static void mcom02_nand_wait_for_event(struct mcom02_nand_priv *priv,
				       uint32_t event)
{
	uint32_t timeout = ARASAN_NAND_POLL_TIMEOUT;

	while (!(readl(priv->regs + INTR_STS_OFST) & event) && timeout) {
		udelay(1);
		timeout--;
	}

	if (!timeout)
		printf("Event waiting timeout, %d\n", event);

	writel(event, priv->regs + INTR_STS_OFST);
}

static void mcom02_nand_readfifo(struct mcom02_nand_priv *priv, uint32_t prog,
				 uint32_t size)
{
	uint32_t i, *bufptr = (uint32_t *)&priv->buf[0];

	mcom02_nand_set_irq_masks(priv, READ_READY);

	writel(prog, priv->regs + PROG_OFST);
	mcom02_nand_wait_for_event(priv, READ_READY);

	mcom02_nand_set_irq_masks(priv, XFER_COMPLETE);

	for (i = 0; i < size / 4; i++)
		bufptr[i] = readl(priv->regs + DATA_PORT_OFST);

	mcom02_nand_wait_for_event(priv, XFER_COMPLETE);
}

static void mcom02_nand_cmdfunc(struct mtd_info *mtd, unsigned int cmd,
				int column, int page_addr)
{
	struct nand_chip *nand = mtd_to_nand(mtd);
	struct mcom02_nand_priv *priv = nand_get_controller_data(nand);
	int wait = 0 /* false */, read = 0 /* false */, read_id = 0 /* false */;
	uint32_t addrcycles, prog;
	uint32_t *bufptr = (uint32_t *)&priv->buf[0];

	priv->bufshift = 0;
	priv->curr_cmd = cmd;

	if (page_addr == -1)
		page_addr = 0;
	if (column == -1)
		column = 0;

	switch (cmd) {
	case NAND_CMD_RESET:
		mcom02_nand_prepare_cmd(priv, cmd, 0, 0, 0);
		prog = PROG_RST;
		wait = 1 /* true */;
		break;
	case NAND_CMD_SEQIN:
		addrcycles = priv->raddr_cycles + priv->caddr_cycles;
		priv->page = page_addr;
		mcom02_nand_prepare_cmd(priv, cmd, NAND_CMD_PAGEPROG,
					mtd->writesize, addrcycles);
		mcom02_nand_setpagecoladdr(priv, page_addr, column);
		break;
	case NAND_CMD_READOOB:
		column += mtd->writesize;
	case NAND_CMD_READ0:
	case NAND_CMD_READ1:
		addrcycles = priv->raddr_cycles + priv->caddr_cycles;
		mcom02_nand_prepare_cmd(priv, NAND_CMD_READ0,
					NAND_CMD_READSTART,
					mtd->writesize, addrcycles);
		mcom02_nand_setpagecoladdr(priv, page_addr, column);
		break;
	case NAND_CMD_RNDOUT:
		mcom02_nand_prepare_cmd(priv, cmd, NAND_CMD_RNDOUTSTART,
					mtd->writesize, 2);
		mcom02_nand_setpagecoladdr(priv, page_addr, column);
		priv->rdintrmask = READ_READY;
		break;
	case NAND_CMD_PARAM:
		mcom02_nand_prepare_cmd(priv, cmd, 0, 0, 1);
		mcom02_nand_setpagecoladdr(priv, page_addr, column);
		mcom02_nand_setpktszcnt(priv, sizeof(struct nand_onfi_params),
					1);
		mcom02_nand_readfifo(priv, PROG_RDPARAM,
				     sizeof(struct nand_onfi_params));
		break;
	case NAND_CMD_READID:
		mcom02_nand_prepare_cmd(priv, cmd, 0, 0, 1);
		mcom02_nand_setpagecoladdr(priv, page_addr, column);
		if (column == ONFI_ID_ADDR)
			mcom02_nand_setpktszcnt(priv, ONFI_ID_LEN, 1);
		else
			mcom02_nand_setpktszcnt(priv, MAF_ID_LEN, 1);
		prog = PROG_RDID;
		wait = 1 /* true */;
		read_id = 1 /* true */;
		break;
	case NAND_CMD_ERASE1:
		addrcycles = priv->raddr_cycles;
		mcom02_nand_prepare_cmd(priv, cmd, NAND_CMD_ERASE2,
					0, addrcycles);
		column = page_addr & 0xffff;
		page_addr = (page_addr >> PG_ADDR_SHIFT) & 0xffff;
		mcom02_nand_setpagecoladdr(priv, page_addr, column);
		prog = PROG_ERASE;
		wait = 1 /* true */;
		break;
	case NAND_CMD_STATUS:
		mcom02_nand_prepare_cmd(priv, cmd, 0, 0, 0);
		mcom02_nand_setpktszcnt(priv, 1, 1);
		mcom02_nand_setpagecoladdr(priv, page_addr, column);
		prog = PROG_STATUS;
		wait = 1 /* true */;
		read = 1 /* true */;
		break;
	default:
		return;
	}

	if (wait) {
		mcom02_nand_set_irq_masks(priv, XFER_COMPLETE);
		writel(prog, priv->regs + PROG_OFST);
		mcom02_nand_wait_for_event(priv, XFER_COMPLETE);
	}

	if (read)
		bufptr[0] = readl(priv->regs + FLASH_STS_OFST);
	if (read_id) {
		bufptr[0] = readl(priv->regs + ID1_OFST);
		bufptr[1] = readl(priv->regs + ID2_OFST);
		if (column == ONFI_ID_ADDR)
			bufptr[0] = ((bufptr[0] >> 8) | (bufptr[1] << 24));
	}
}

static void mcom02_nand_select_chip(struct mtd_info *mtd, int chip)
{
	struct nand_chip *nand = mtd_to_nand(mtd);
	struct mcom02_nand_priv *priv = nand_get_controller_data(nand);
	uint32_t val;

	if (chip == -1)
		return;

	val = readl(priv->regs + MEM_ADDR2_OFST);
	val = (val & ~(CS_MASK)) | (chip << CS_SHIFT);
	writel(val, priv->regs + MEM_ADDR2_OFST);
}

static uint8_t mcom02_nand_read_byte(struct mtd_info *mtd)
{
	struct nand_chip *nand = mtd_to_nand(mtd);
	struct mcom02_nand_priv *priv = nand_get_controller_data(nand);

	return priv->buf[priv->bufshift++];
}

static void mcom02_nand_read_buf(struct mtd_info *mtd, uint8_t *buf, int size)
{
	struct nand_chip *nand = mtd_to_nand(mtd);
	struct mcom02_nand_priv *priv = nand_get_controller_data(nand);
	uint32_t i, pktcount, buf_rd_cnt = 0, pktsize;
	uint32_t *bufptr = (uint32_t *)buf;

	priv->rdintrmask |= READ_READY;

	if (priv->curr_cmd == NAND_CMD_READ0) {
		pktsize = priv->pktsize;
		if (mtd->writesize % pktsize)
			pktcount = mtd->writesize / pktsize + 1;
		else
			pktcount = mtd->writesize / pktsize;
	} else {
		pktsize = size;
		pktcount = 1;
	}

	mcom02_nand_setpktszcnt(priv, pktsize, pktcount);

	mcom02_nand_set_irq_masks(priv, priv->rdintrmask);
	writel(PROG_PGRD, priv->regs + PROG_OFST);

	while (buf_rd_cnt < pktcount) {
		mcom02_nand_wait_for_event(priv, READ_READY);
		buf_rd_cnt++;

		if (buf_rd_cnt == pktcount)
			mcom02_nand_set_irq_masks(priv, XFER_COMPLETE);

		for (i = 0; i < pktsize / 4; i++)
			bufptr[i] = readl(priv->regs + DATA_PORT_OFST);

		bufptr += (pktsize / 4);

		if (buf_rd_cnt < pktcount)
			mcom02_nand_set_irq_masks(priv, priv->rdintrmask);
	}

	mcom02_nand_wait_for_event(priv, XFER_COMPLETE);
	priv->rdintrmask = 0;
}

static inline void mcom02_nand_set_eccsparecmd(struct mcom02_nand_priv *priv,
					       uint8_t cmd1, uint8_t cmd2)
{
	writel(cmd1 | (cmd2 << CMD2_SHIFT) |
	      (priv->caddr_cycles << ADDR_CYCLES_SHIFT) | SPARE_ADDR_CYCLES,
	       priv->regs + ECC_SPR_CMD_OFST);
}

static int mcom02_nand_read_page_hwecc(struct mtd_info *mtd,
				       struct nand_chip *nand, uint8_t *buf,
				       int oob_required, int page)
{
	struct mcom02_nand_priv *priv = nand_get_controller_data(nand);
	uint32_t val;

	mcom02_nand_set_eccsparecmd(priv, NAND_CMD_RNDOUT,
				    NAND_CMD_RNDOUTSTART);
	writel(priv->ecc_regval, priv->regs + ECC_OFST);

	val = readl(priv->regs + CMD_OFST);
	val = val | ECC_ENABLE;
	writel(val, priv->regs + CMD_OFST);

	if (!priv->bch)
		priv->rdintrmask = MBIT_ERROR;

	nand->read_buf(mtd, buf, mtd->writesize);

	val = readl(priv->regs + ECC_ERR_CNT_OFST);
	if (priv->bch) {
		mtd->ecc_stats.corrected +=
			(val & PAGE_ERR_CNT_MASK) >> PAGE_ERR_CNT_SHIFT;
	} else {
		val = readl(priv->regs + ECC_ERR_CNT_1BIT_OFST);
		mtd->ecc_stats.corrected += val;
		val = readl(priv->regs + ECC_ERR_CNT_2BIT_OFST);
		mtd->ecc_stats.failed += val;
		/* clear ecc error count register 1Bit, 2Bit */
		writel(0x0, priv->regs + ECC_ERR_CNT_1BIT_OFST);
		writel(0x0, priv->regs + ECC_ERR_CNT_2BIT_OFST);
	}
	priv->err = 0 /* false */;

	if (oob_required)
		nand->ecc.read_oob(mtd, nand, page);

	return 0;
}

static int mcom02_nand_device_ready(struct mtd_info *mtd,
				    struct nand_chip *nand)
{
	uint8_t status;
	uint32_t timeout = STATUS_TIMEOUT;

	while (timeout--) {
		nand->cmdfunc(mtd, NAND_CMD_STATUS, 0, 0);
		status = nand->read_byte(mtd);

		if (status & ONFI_STATUS_READY) {
			if (status & ONFI_STATUS_FAIL)
				return NAND_STATUS_FAIL;

			return 0;
		}
	}

	printf("Device ready timedout\n");

	return -ETIMEDOUT;
}

static void mcom02_nand_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	struct nand_chip *nand = mtd_to_nand(mtd);
	struct mcom02_nand_priv *priv = nand_get_controller_data(nand);
	uint32_t buf_wr_cnt = 0, pktcount = 1, i, pktsize;
	uint32_t *bufptr = (uint32_t *)buf;

	if (priv->iswriteoob) {
		pktsize = len;
		pktcount = 1;
	} else {
		pktsize = priv->pktsize;
		pktcount = mtd->writesize / pktsize;
	}

	mcom02_nand_setpktszcnt(priv, pktsize, pktcount);

	mcom02_nand_set_irq_masks(priv, WRITE_READY);
	writel(PROG_PGPROG, priv->regs + PROG_OFST);

	while (buf_wr_cnt < pktcount) {
		mcom02_nand_wait_for_event(priv, WRITE_READY);

		buf_wr_cnt++;
		if (buf_wr_cnt == pktcount)
			mcom02_nand_set_irq_masks(priv, XFER_COMPLETE);

		for (i = 0; i < (pktsize / 4); i++)
			writel(bufptr[i], priv->regs + DATA_PORT_OFST);

		bufptr += (pktsize / 4);

		if (buf_wr_cnt < pktcount)
			mcom02_nand_set_irq_masks(priv, WRITE_READY);
	}

	mcom02_nand_wait_for_event(priv, XFER_COMPLETE);
}

static int mcom02_nand_write_page_hwecc(struct mtd_info *mtd,
					struct nand_chip *nand,
					const uint8_t *buf,
					int oob_required, int page)
{
	struct mcom02_nand_priv *priv = nand_get_controller_data(nand);
	uint8_t *ecc_calc = nand->buffers->ecccalc;
	uint32_t *eccpos = nand->ecc.layout->eccpos;
	uint32_t val, i;

	mcom02_nand_set_eccsparecmd(priv, NAND_CMD_RNDIN, 0);
	writel(priv->ecc_regval, priv->regs + ECC_OFST);

	val = readl(priv->regs + CMD_OFST);
	val = val | ECC_ENABLE;
	writel(val, priv->regs + CMD_OFST);

	nand->write_buf(mtd, buf, mtd->writesize);

	if (oob_required) {
		mcom02_nand_device_ready(mtd, nand);
		nand->cmdfunc(mtd, NAND_CMD_READOOB, 0, page);
		nand->read_buf(mtd, ecc_calc, mtd->oobsize);
		for (i = 0; i < nand->ecc.total; i++)
			nand->oob_poi[eccpos[i]] = ecc_calc[eccpos[i]];
		nand->ecc.write_oob(mtd, nand, page);
	}

	return 0;
}

static int mcom02_nand_read_oob(struct mtd_info *mtd, struct nand_chip *nand,
				int page)
{
	nand->cmdfunc(mtd, NAND_CMD_READOOB, 0, page);
	nand->read_buf(mtd, nand->oob_poi, mtd->oobsize);

	return 0;
}

static int mcom02_nand_write_oob(struct mtd_info *mtd, struct nand_chip *nand,
				 int page)
{
	struct mcom02_nand_priv *priv = nand_get_controller_data(nand);

	priv->iswriteoob = 1 /* true */;
	nand->cmdfunc(mtd, NAND_CMD_SEQIN, mtd->writesize, page);
	nand->write_buf(mtd, nand->oob_poi, mtd->oobsize);
	priv->iswriteoob = 0 /* false */;

	return 0;
}

static int mcom02_nand_ecc_init(struct mtd_info *mtd)
{
	struct nand_chip *nand = mtd_to_nand(mtd);
	struct mcom02_nand_priv *priv = nand_get_controller_data(nand);
	uint32_t oob_index, i, regval, eccaddr, bchmode = 0;
	int found = -1;

	nand->ecc.mode = NAND_ECC_HW;
	nand->ecc.read_page = mcom02_nand_read_page_hwecc;
	nand->ecc.write_page = mcom02_nand_write_page_hwecc;
	nand->ecc.write_oob = mcom02_nand_write_oob;
	nand->ecc.read_oob = mcom02_nand_read_oob;

	for (i = 0; i < ARRAY_SIZE(ecc_matrix); i++) {
		if (ecc_matrix[i].pagesize == mtd->writesize) {
			if (ecc_matrix[i].eccbits >=
			    nand->ecc_strength_ds) {
				found = i;
				break;
			}
			found = i;
		}
	}

	if (found < 0) {
		printf("ECC scheme not supported\n");
		return 1;
	}

	if (ecc_matrix[found].bch) {
		switch (ecc_matrix[found].eccbits) {
		case 12:
			bchmode = 0x1;
			break;
		case 8:
			bchmode = 0x2;
			break;
		case 4:
			bchmode = 0x3;
			break;
		case 24:
			bchmode = 0x4;
			break;
		default:
			bchmode = 0x0;
		}
	}

	nand->ecc.strength = ecc_matrix[found].eccbits;
	nand->ecc.size = ecc_matrix[found].codeword_size;

	nand->ecc.steps = ecc_matrix[found].pagesize /
			       ecc_matrix[found].codeword_size;

	nand->ecc.bytes = ecc_matrix[found].eccsize /
			       nand->ecc.steps;

	priv->ecclayout.eccbytes = ecc_matrix[found].eccsize;
	priv->bch = ecc_matrix[found].bch;

	if (mtd->oobsize < ecc_matrix[found].eccsize + 2) {
		printf("OOB too small for ECC scheme\n");
		return 1;
	}
	oob_index = mtd->oobsize - priv->ecclayout.eccbytes;
	eccaddr = mtd->writesize + oob_index;

	for (i = 0; i < nand->ecc.size; i++)
		priv->ecclayout.eccpos[i] = oob_index + i;

	priv->ecclayout.oobfree->offset = 2;
	priv->ecclayout.oobfree->length = oob_index -
		priv->ecclayout.oobfree->offset;

	nand->ecc.layout = &priv->ecclayout;

	regval = eccaddr |
		(ecc_matrix[found].eccsize << ECC_SIZE_SHIFT) |
		(ecc_matrix[found].bch << BCH_EN_SHIFT);
	priv->ecc_regval = regval;
	writel(regval, priv->regs + ECC_OFST);

	regval = readl(priv->regs + MEM_ADDR2_OFST);
	regval = (regval & ~(BCH_MODE_MASK)) | (bchmode << BCH_MODE_SHIFT);
	writel(regval, priv->regs + MEM_ADDR2_OFST);

	if (nand->ecc.size >= 1024)
		priv->pktsize = 1024;
	else
		priv->pktsize = 512;

	return 0;
}

static int mcom02_nand_probe(struct udevice *dev)
{
	struct mcom02_nand_priv *priv = dev_get_priv(dev);
	struct nand_chip *nand = &priv->nand;
	struct mtd_info *mtd;
	int ret;

	priv->regs = (void *)devfdt_get_addr(dev);
	nand_set_controller_data(nand, priv);

	/* Set the driver entry points for MTD */
	nand->cmdfunc = mcom02_nand_cmdfunc;
	nand->select_chip = mcom02_nand_select_chip;
	nand->read_byte = mcom02_nand_read_byte;
	nand->waitfunc = mcom02_nand_device_ready;
	nand->chip_delay = 30;

	/* Buffer read/write routines */
	nand->read_buf = mcom02_nand_read_buf;
	nand->write_buf = mcom02_nand_write_buf;
	nand->options = NAND_BUSWIDTH_AUTO
			| NAND_NO_SUBPAGE_WRITE
			| NAND_USE_BOUNCE_BUFFER;
	nand->bbt_options = NAND_BBT_USE_FLASH;

	priv->rdintrmask = 0;

	mtd = nand_to_mtd(nand);
	ret = nand_scan_ident(mtd, 1, NULL);
	if (ret)
		return ret;

	if (mtd->writesize > SZ_8K) {
		printf("Page size too big for controller\n");
		return -EINVAL;
	}

	if (!nand->onfi_params.addr_cycles) {
		/* Good estimate in case ONFI ident doesn't work */
		priv->raddr_cycles = 3;
		priv->caddr_cycles = 2;
	} else {
		priv->raddr_cycles = nand->onfi_params.addr_cycles & 0xF;
		priv->caddr_cycles =
			(nand->onfi_params.addr_cycles >> 4) & 0xF;
	}

	if (mcom02_nand_ecc_init(mtd))
		return -ENXIO;

	ret = nand_scan_tail(mtd);
	if (ret)
		return ret;

	ret = nand_register(0, mtd);
	if (ret)
		return ret;

	return 0;
}
//
//static const struct udevice_id mcom02_nand_dt_ids[] = {
//	{ .compatible = "arasan,nfc-v2p99" },
//	{ },
//};
//
//U_BOOT_DRIVER(mcom02_nand) = {
//	.name = "mcom02-nand",
//	.id = UCLASS_MTD,
//	.of_match = mcom02_nand_dt_ids,
//	.probe = mcom02_nand_probe,
//	.priv_auto_alloc_size = sizeof(struct mcom02_nand_priv),
//};

//void board_nand_init(void)
//{
//	struct udevice *dev;
//	int ret;
//
//	ret = uclass_get_device_by_driver(UCLASS_MTD,
//					  DM_GET_DRIVER(mcom02_nand), &dev);
//	if (ret && ret != -ENODEV)
//		printf("Failed to initialize %s, error %d\n", dev->name, ret);
//}

static void vm14nand_command(unsigned command, unsigned nbumaddr)
{
	NANDMPORT->COMMAND =
			(command & 0xFFFF) |    // код команды
			(nbumaddr << 28) |
			(1u << 23) |	// page sie = 2048
			0;
}

static void vm14nand_sendcommand(unsigned command, unsigned programm, unsigned nbumaddr)
{
	NANDMPORT->INTERRUPT_STATUS = 0xFF;          // для сброса необходимо записать 1 в соответствующее поле (0xFF тоже сработает)
	NANDMPORT->INTERRUPT_STATUS_EN = 0xFF;      // разрешаем статус прерывания все
	vm14nand_command(command, nbumaddr);
	NANDMPORT->PROGRAM = programm;                 // команда
	while ((NANDMPORT->INTERRUPT_STATUS & 0x4) == 0)  // ждем пока установится статус прерывания Transfer Complete
	{
//		PRINTF("vm14nand_sendcommand: NANDMPORT->INTERRUPT_STATU=%08x\n", (unsigned) NANDMPORT->INTERRUPT_STATUS);
//		if (NANDMPORT->INTERRUPT_STATUS & 0x1)
//			NANDMPORT->BUFFER_DATA = 0x00;
//		if (NANDMPORT->INTERRUPT_STATUS & 0x2)
//			PRINTF("DATA=%08X\n", (unsigned) NANDMPORT->BUFFER_DATA);
	}
	//TP();

	while ((NANDMPORT->INTERRUPT_STATUS & 0x4) != 0)  // сбрасываем статус прерывания Transfer Complete
	{
		NANDMPORT->INTERRUPT_STATUS = 0x4;          // для сброса необходимо записать 1 в соответствующее поле (0xFF тоже сработает)
	}
	// после этого команда reset выполнена (статус прерывания сброшен в 0)
	//TP();

}

// READ
static void vm14nand_sendcommand2(unsigned command, unsigned programm, unsigned nbumaddr)
{
	NANDMPORT->INTERRUPT_STATUS = 0xFF;          // для сброса необходимо записать 1 в соответствующее поле (0xFF тоже сработает)
	NANDMPORT->INTERRUPT_STATUS_EN = 0xFF;      // разрешаем статус прерывания все
	vm14nand_command(command, nbumaddr);
	NANDMPORT->PROGRAM = programm;                 // команда
//	TP();
//	while (NANDMPORT->INTERRUPT_STATUS != (1u << 1))  // buff_rd_rdy_reg
//		;
//	TP();
//
}

void vm14nand_reset(void)
{
	// RESET
	vm14nand_sendcommand(0xFF, 1u << 8, 0);	// 0xFF: код команды reset, 0x100 - команда reset
}

void vm14nand_readid(unsigned address)
{
	NANDMPORT->MEMADDR1 = address;
	NANDMPORT->MEMADDR2 = 0;
	// READ ID
	vm14nand_sendcommand(0x90, 1u << 6, 1);	// 0x90: код команды readID, 0x40 - команда read id
	// id1=0xA927 id2=0
	PRINTF("vm14nand_readid(%04X): ID1=%08x, ID2=%08x\n", address, (unsigned) NANDMPORT->ID1, (unsigned) NANDMPORT->ID2);
}

unsigned vm14nand_readstatus(void)
{
	// READ STATUS
	vm14nand_sendcommand(0x70, 1u << 3, 0);
	PRINTF("vm14nand_readstatus: FLASH_STATUS=%08x\n", (unsigned) NANDMPORT->FLASH_STATUS);

	return NANDMPORT->FLASH_STATUS;

}

void vm14nand_readdata(unsigned address, uint8_t * p, int len)
{
	unsigned long columnaddr = 0;
	unsigned long blockaddr = 0;	// 0..2047
	unsigned long pageaddr = 0;		// 0..31

	NANDMPORT->MEMADDR1 = 0;
	NANDMPORT->MEMADDR2 = 0;
	// PAGE READ
	TP();
	vm14nand_sendcommand2(0x00, (1u << 0), 5);	// 0x00: код команды READ MODE, 0x40 - команда read mode
	local_delay_ms(100);
//	{
//		TP();
//		const unsigned status = (unsigned) NANDMPORT->INTERRUPT_STATUS;
//		PRINTF("Wait NANDMPORT->INTERRUPT_STATU=%08x\n", status);
//
//	}
	vm14nand_sendcommand2(0x30, (1u << 0), 0);	// 0x00: код команды READ MODE, 0x40 - команда read mode
	local_delay_ms(100);
//	{
//		TP();
//		const unsigned status = (unsigned) NANDMPORT->INTERRUPT_STATUS;
//		PRINTF("Wait NANDMPORT->INTERRUPT_STATU=%08x\n", status);
//
//	}
	unsigned i = 0;
//	for (;;)
//	{
//		const unsigned status = (unsigned) NANDMPORT->INTERRUPT_STATUS;
//		//local_delay_ms(100);
//		//PRINTF("Wait NANDMPORT->INTERRUPT_STATU=%08x\n", status);
//		if (status & 0x2)	// buff_rd_rdy_reg
//			PRINTF("[%4u] %08X", i ++, (unsigned) NANDMPORT->BUFFER_DATA);
//		if (status & 0x1)	// buff_wr_rdy_reg
//			NANDMPORT->BUFFER_DATA = 0x00;
//	}
	TP();
	//vm14nand_sendcommand(0x30, 1u << 0, 0);	// 0x00: код команды READ MODE, 0x40 - команда read mode
	//TP();
	// READ
	//vm14nand_sendcommand2(0x30, 1u << 0);	// 0x30: код команды READ, 0x01 - команда read
	// id1=0xA927 id2=0
	//PRINTF("vm14nand_readid(%04X): ID1=%08x, ID2=%08x\n", address, (unsigned) NANDMPORT->ID1, (unsigned) NANDMPORT->ID2);

	while (len >= 4)
	{
		const unsigned status = (unsigned) NANDMPORT->INTERRUPT_STATUS;
		if (status & 0x1)	// buff_wr_rdy_reg
		{
			//PRINTF("W\n");
			NANDMPORT->BUFFER_DATA = 0x00;
		}
		if (status & 0x2)	// buff_rd_rdy_reg
		{
			__UNALIGNED_UINT32_WRITE(p, NANDMPORT->BUFFER_DATA);
			p += 4;
			len -= 4;
		}
	}
}

void vm14nand_writedata(unsigned address, uint8_t * p, int len)
{
	unsigned long columnaddr = 0;
	unsigned long blockaddr = 0;	// 0..2047
	unsigned long pageaddr = 0;		// 0..31

	NANDMPORT->MEMADDR1 = address;
	NANDMPORT->MEMADDR2 = 0;
	// PAGE READ
	TP();
	vm14nand_sendcommand2(0x80, (1u << 4), 5);	// program page
	local_delay_ms(100);
	unsigned i = 0;
//	for (;;)
//	{
//		const unsigned status = (unsigned) NANDMPORT->INTERRUPT_STATUS;
//		//local_delay_ms(100);
//		//PRINTF("Wait NANDMPORT->INTERRUPT_STATU=%08x\n", status);
//		if (status & 0x2)	// buff_rd_rdy_reg
//			PRINTF("[%4u] %08X", i ++, (unsigned) NANDMPORT->BUFFER_DATA);
//		if (status & 0x1)	// buff_wr_rdy_reg
//			NANDMPORT->BUFFER_DATA = 0x00;
//	}
	TP();
	//vm14nand_sendcommand(0x30, 1u << 0, 0);	// 0x00: код команды READ MODE, 0x40 - команда read mode
	//TP();
	// READ
	//vm14nand_sendcommand2(0x30, 1u << 0);	// 0x30: код команды READ, 0x01 - команда read
	// id1=0xA927 id2=0
	//PRINTF("vm14nand_readid(%04X): ID1=%08x, ID2=%08x\n", address, (unsigned) NANDMPORT->ID1, (unsigned) NANDMPORT->ID2);
	int n = 0;
	while (len >= 4)
	{
		//local_delay_ms(1);
		const unsigned status = (unsigned) NANDMPORT->INTERRUPT_STATUS;
		//PRINTF("Wait NANDMPORT->INTERRUPT_STATU=%08x (%u)\n", status, n ++);
		if (status & 0x1)	// buff_wr_rdy_reg
		{
			NANDMPORT->BUFFER_DATA = __UNALIGNED_UINT32_READ(p);
			p += 4;
			len -= 4;
		}
//		if (status & 0x2)	// buff_rd_rdy_reg
//		{
//			__UNALIGNED_UINT32_WRITE(p, NANDMPORT->BUFFER_DATA);
//			p += 4;
//			len -= 4;
//		}
	}
	local_delay_ms(100);
	vm14nand_sendcommand2(0xD0, (1u << 0), 0);	// 0x00: код команды READ MODE, 0x40 - команда read mode
	local_delay_ms(100);
}


void vm14nand_eraseblock(unsigned baddress)
{
//	unsigned long columnaddr = 0;
//	unsigned long blockaddr = 0;	// 0..2047
//	unsigned long pageaddr = 0;		// 0..31

	NANDMPORT->MEMADDR1 = baddress;
	NANDMPORT->MEMADDR2 = 0;
	// PAGE ERASE
	TP();
	vm14nand_sendcommand2(0x60, (1u << 2), 3);
	local_delay_ms(100);
	vm14nand_sendcommand2(0xD0, (1u << 2), 0);
	local_delay_ms(100);

//	vm14nand_sendcommand2(0xD060, (1u << 2), 3);
//	local_delay_ms(100);
}
// MT29F32G08AFACBWP-ITZ
#define SYSTEM_BUS_CLK 144
#define NAND_BUS_FREQ  96

void vm41nandtest(void)
{
	CMCTR->GATE_SYS_CTR &= ~(1u << 21);
	CMCTR->DIV_NFC_CTR = SYSTEM_BUS_CLK / NAND_BUS_FREQ;
	//setSystemFreq(SYSTEM_BUS_CLK);
	CMCTR->GATE_SYS_CTR |= (1u << 21);
	PRINTF("NAND test commands: z, i, n, p, e, r, w\n");
	unsigned sector = 0;
	uint8_t buff [2048] = { 0 };
	for (;;)
	{
		char c;
		if (dbg_getchar(& c))
		{
			switch (c)
			{
			case 'z':
				PRINTF("Reset\n");
				vm14nand_reset();
				break;
			case 'i':
				PRINTF("Read IDs\n");
				vm14nand_readid(0x00);
				vm14nand_readid(0x20);
				vm14nand_readstatus();
				break;
			case 'n':
				sector += 1;
				PRINTF("sector = %u\n", sector);
				break;
			case 'p':
				if (sector)
					sector -= 1;
				PRINTF("sector = %u\n", sector);
				break;
			case 'e':
				PRINTF("Erase block %u\n", sector);
				vm14nand_eraseblock(sector);
				break;
			case 'r':
				PRINTF("Read sector = %u\n", sector);
				vm14nand_readdata(sector, buff, sizeof buff);
				printhex(0, buff, sizeof buff);
				break;
			case 'w':
				PRINTF("Write sector = %u\n", sector);
				memset(buff, 0xE5, sizeof buff);
				buff [0] = sector >> 8;
				buff [1] = sector;
				//memset(b, 0xE5, sizeof b);
				vm14nand_writedata(sector, buff, sizeof buff);
				break;
			default:
				PRINTF("Undefined command %02X\n", c);
				break;
			}
		}
	}
}

#endif /* CPUSTYLE_VM14 */

#if CPUSTYLE_ALLWINNER && 0


static void de_dump(void)
{
	memset(DE_BASE, 0xFF, 4 * 1024 * 1024);
	int skip = 0;
	static uint8_t pattern [256];
	unsigned offs;
	for (offs = 0; offs < 4 * 1024 * 1024; offs += 4096)
	{
		* (volatile uint32_t *) (DE_BASE + offs) |= 1;
		if (memcmp(pattern, (void *) (DE_BASE + offs), sizeof pattern) == 0)
		{
			++ skip;
			PRINTF(".");
			continue;
		}
		if (skip)
		{
			skip = 0;
			PRINTF("\n");
		}
		printhex32(DE_BASE + offs, (void *) (DE_BASE + offs), 256);
		PRINTF("---\n");
	}
}

int wrongbase(uintptr_t base)
{
//	if (base >= 0x05102800 && base < 0x05200000)
//		return 1;
//	if (base < 0x01300000)
//		return 0;
//	if (base < (0x01300000 + 0xA0000))
//		return 1;
//	if (base >= (0x01300000 + 0))
//		return 1;
//	PRINTF("pr=0x%08X ", base);
	return 0;
}

static DE_BLD_TypeDef * blds [32];
unsigned nblds = 0;
void testde(void)
{
//	{
//		uint32_t volatile * const p = (uint32_t volatile *) DE_VI1_BASE;
//		uint32_t v = * p;
//		* p = ~ 0u;
//		PRINTF("VI SIGN=%08X\n", * p);
//		* p = v;
//	}
//	{
//		uint32_t volatile * const p = (uint32_t volatile *) DE_UI1_BASE;
//		uint32_t v = * p;
//		* p = ~ 0u;
//		PRINTF("UI SIGN=%08X\n", * p);
//		* p = v;
//	}
//	{
//		uint32_t volatile * const p = (uint32_t volatile *) (DE_BLD1_BASE + 0x80);
//		//uint32_t v = * p;
//		//* p = ~ 0u;
//		PRINTF("BLD SIGN=%08X\n", * p);
//		//* p = v;
//	}
	PRINTF("UI1:\n");
	memset(DE_UI1_BASE, 255, 256);
	printhex32(DE_UI1_BASE, DE_UI1_BASE, 256);
//	PRINTF("UI2:\n");
//	memset(DE_UI2_BASE, 255, 256);
//	printhex32(DE_UI2_BASE, DE_UI2_BASE, 256);
//	PRINTF("UI3:\n");
//	memset(DE_UI3_BASE, 255, 256);
//	printhex32(DE_UI3_BASE, DE_UI3_BASE, 256);
	PRINTF("VI1:\n");
	memset(DE_VI1_BASE, 255, 256);
	printhex32(DE_VI1_BASE, DE_VI1_BASE, 256);

	//printhex32(DE_BASE, DE_BASE, 4 * 1024 * 1024);
//	const uint32_t signUI = 0xFFB31F17;	// UI signature
//	const uint32_t signVI = 0xFFB39F17;	// VI signature
	const uint32_t signUI = 0xFFB3BF1F;	// UI signature 113
	const uint32_t signVI = 0xFFB3BF1F;	// VI signature 113
	uintptr_t base = DE_BASE;
	uintptr_t top = base + 4 * 1024 * 1024;
	unsigned bld = 0;
	for (;base < top; base += 4096)
	{
		if (wrongbase(base))
			continue;
		uint32_t volatile * const p = (uint32_t volatile *) base;
//		uint32_t v = * p;
//		* p = ~ 0u;
//		uint32_t dprobe = * p;
//		* p = v;
//		TP();
		if (* (uint32_t volatile *) (base + 0x80) == 0x00543210)
		{
			++ bld;
			PRINTF("found BLD6 at 0x%08X\n", (unsigned) base);
			((DE_BLD_TypeDef *) base)->BKCOLOR = bld;
			blds [nblds ++] = ((DE_BLD_TypeDef *) base);
		}
		else if (* (uint32_t volatile *) (base + 0x80) == 0x00003210)
		{
			++ bld;
			PRINTF("found BLD4 at 0x%08X\n", (unsigned) base);
			((DE_BLD_TypeDef *) base)->BKCOLOR = bld;
			blds [nblds ++] = ((DE_BLD_TypeDef *) base);
		}
//		if (bld == 4)
//			break;
//		else if (signUI == dprobe)
//		{
//			PRINTF("found UI at 0x%08X\n", (unsigned) base);
//		}
//		else if (signVI == dprobe)
//		{
//			PRINTF("found VI at 0x%08X\n", (unsigned) base);
//		}
//		else if ((dprobe & 0xFFF00000) == 0xFFB00000)
//		{
//			PRINTF("found XX (%08X) at 0x%08X\n", (unsigned) dprobe, (unsigned) base);
//		}
//		TP();
	}
	unsigned i;
	for (i = 0; i < nblds; ++ i)
	{
		PRINTF("naskr bld%u (%p)= %08X\n", i, blds [i], blds [i]->BKCOLOR);
	}
}
#endif

// p15, 1, <Rt>, c15, c3, 0; -> __get_CP64(15, 1, result, 15);  Read CBAR into Rt
// p15, 1, <Rt>, <Rt2>, c15; -> __get_CP64(15, 1, result, 15);
void hightests(void)
{
#if WITHLTDCHW && LCDMODE_LTDC
	hardware_ltdc_main_set((uintptr_t) colmain_fb_draw());
#endif /* WITHLTDCHW && LCDMODE_LTDC */
	//hmain();
#if CPUSTYLE_VM14 && 1
	{
		vm41nandtest();
	}
#endif
#if 0
	{
		// cache line size test
		static __ALIGNED(256) uint8_t data [256];
		memset(data, 0xE5, sizeof data);
		dcache_clean_all();
		memset(data, 0x00, sizeof data);
		__DSB();
		__set_DCIMVAC((uintptr_t) data);	// Invalidate data cache line by address.
		//dcache_invalidate((uintptr_t) data, 8);
		//dcache_clean_all();
		printhex((uintptr_t) data, data, sizeof data);
		for (;;)
			;

	}
#endif
#if CPUSTYLE_STM32MP1 && WITHETHHW && 0
	{
		// Ethernet controller tests
		ethhw_initialize();

		TP();

		ETH->MACCR |= ETH_MACCR_PS_Msk | ETH_MACCR_FES_Msk;	// Select 100 Mbps operation
		ETH->MACCR |= ETH_MACCR_DM_Msk;	// Select duplex operation

		PRINTF("ETH->MACCR=%08X\n", (unsigned) ETH->MACCR);
		PRINTF("ETH->DMASBMR=%08X\n", (unsigned) ETH->DMASBMR);

		static __attribute__((aligned(32))) uint8_t  dmac0tx_buff [2][ETHHW_BUFFSIZE];
		static __attribute__((aligned(32))) uint8_t  dmac0rx_buff [2][ETHHW_BUFFSIZE];
		static __attribute__((aligned(32))) uint8_t  dmac1tx_buff [2][ETHHW_BUFFSIZE];

		static __attribute__((aligned(32))) volatile uint32_t  dmac0tx_desc [64];
		static __attribute__((aligned(32))) volatile uint32_t  dmac0rx_desc [64];
		static __attribute__((aligned(32))) volatile uint32_t  dmac1tx_desc [64];

		ethhw_filldesc(dmac0tx_desc, dmac0tx_buff [0], dmac0tx_buff [1]);
		ethhw_filldesc(dmac0rx_desc, dmac0rx_buff [0], dmac0rx_buff [1]);
		ethhw_filldesc(dmac1tx_desc, dmac1tx_buff [0], dmac1tx_buff [1]);

		dcache_clean_invalidate((uintptr_t) dmac0tx_desc, sizeof dmac0tx_desc);
		dcache_clean_invalidate((uintptr_t) dmac0rx_desc, sizeof dmac0rx_desc);
		dcache_clean_invalidate((uintptr_t) dmac1tx_desc, sizeof dmac1tx_desc);

		dcache_clean_invalidate((uintptr_t) dmac0tx_buff, sizeof dmac0tx_buff);
		dcache_clean_invalidate((uintptr_t) dmac0rx_buff, sizeof dmac0rx_buff);
		dcache_clean_invalidate((uintptr_t) dmac1tx_buff, sizeof dmac1tx_buff);

		// CH0: RX & TX
		ETH->DMAC0TXDLAR = (uintptr_t) dmac0tx_desc;	// Channel 0 Tx descriptor list address register
		ETH->DMAC0TXDTPR = (uintptr_t) dmac0tx_desc;	// Channel 0 Tx descriptor tail pointer register
		ETH->DMAC0TXRLR = 1 * (UINT32_C(1) << 0);	// Channel 0 Tx descriptor ring length register
		ETH->DMAC0TXCR |= 0x01;	// Channel 0 transmit control register

		ETH->DMAC0RXDLAR = (uintptr_t) dmac0rx_desc;	// Channel 0 Rx descriptor list address register
		ETH->DMAC0RXDTPR = (uintptr_t) dmac0rx_desc;	// Channel 0 Rx descriptor tail pointer register
		ETH->DMAC0RXRLR = // Channel 0 Rx descriptor ring length register
			0 * (UINT32_C(17) << 0) |	// ARBS
			1 * (UINT32_C(1) << 0) |	// RDRL
			0;
		ETH->DMAC0RXCR |= 0x01;	// Channel 0 receive control register

		// CH1: TX
		ETH->DMAC1TXDLAR = (uintptr_t) dmac1tx_desc;	// Channel 1 Tx descriptor list address register
		ETH->DMAC1TXDTPR = (uintptr_t) dmac1tx_desc;	// Channel 1 Tx descriptor tail pointer register
		ETH->DMAC1TXRLR = 1 * (UINT32_C(1) << 0);	// Channel 1 Tx descriptor ring length register
		ETH->DMAC1TXCR |= 0x01;	// Channel 1 transmit control register

		ETH->MACCR |= ETH_MACCR_TE_Msk;
		ETH->MACCR |= ETH_MACCR_RE_Msk;

		ethhw_deinitialize();
	}
#endif
#if CPUSTYLE_T113 && 0
	{
		// HiFI4 DSP start test

		static const uint8_t dsp_code [] =
		{
		#include "../build/HiFi4/app/main.txt"
		};
		//	SRAM A1			0x00020000---0x00027FFF		32 KB

		//	DSP0 IRAM		0x00028000---0x00037FFF		64 KB		The local sram is switched to system boot.
		//	DSP0 DRAM0		0x00038000---0x0003FFFF		32 KB		The local sram is switched to system boot.
		//	DSP0 DRAM1		0x00040000---0x00047FFF		32 KB		The local sram is switched to system boot.
		//
		//	DSP0 IRAM (local sram)		0x00400000---0x0040FFFF		64 KB		The local sram is switched to DSP.
		//	DSP0 DRAM0 (local sram)		0x00420000---0x00427FFF		32 KB		The local sram is switched to DSP.
		//	DSP0 DRAM1 (local sram)		0x00440000---0x00447FFF		32 KB		The local sram is switched to DSP.

		const uintptr_t remap_cpu = (uintptr_t) 0x00028000;

		enum { M = 2 };
		CCU->DSP_CLK_REG = (CCU->DSP_CLK_REG & ~ ((0x07 << 24) | (0x1F << 0))) |
				(0x04u << 24) |	// src: PLL_AUDIO1(DIV2)
				((M - 1) << 0) |
				0;

		CCU->DSP_CLK_REG |= (UINT32_C(1) << 31);	// DSP_CLK_GATING


		CCU->DSP_BGR_REG |= (UINT32_C(1) << 1);	// DSP_CFG_GATING
		CCU->DSP_BGR_REG |= (UINT32_C(1) << 17);	// DSP_CFG_RST

		CCU->DSP_BGR_REG &= ~ (UINT32_C(1) << 16);	// DSP_RST

		// Map local sram to CPU
		SYS_CFG->DSP_BOOT_RAMMAP_REG = 0x01;	/* DSP BOOT SRAM REMAP ENABLE 1: DSP 128K Local SRAM Remap for System Boot */

		// https://github.com/YuzukiHD/FreeRTOS-HIFI4-DSP/blob/164696d952116d20100daefd7a475d2ede828eb0/host/uboot-driver/dsp/sun8iw20/dsp_reg.h#L33C1-L39C65
		//xtest();
		PRINTF("allwnrt113_get_dsp_freq()=%" PRIuFAST32 "\n", allwnrt113_get_dsp_freq());
		//PRINTF("DSP_ALT_RESET_VEC_REG=%08" PRIX32 "\n", DSP0_CFG->DSP_ALT_RESET_VEC_REG);
		//PRINTF("DSP_STAT_REG=%08" PRIX32 "\n", DSP0_CFG->DSP_STAT_REG);
		//local_delay_ms(300);

		//memset((void *) remap_cpu, 0xE5, 128 * 1024);
		//memcpy((void *) remap_cpu, dsp_code, sizeof dsp_code);
//		for (unsigned i = 0; i < (128 * 1024) / 4; ++ i)
//		{
//			volatile uint32_t * const p = (void *) remap_cpu;
//			p [xlate_dsp2mpu(i * 4) / 4] = i * 4;
//		}
		const size_t dsp_code_size = sizeof dsp_code;
		copy2dsp((void *) remap_cpu, dsp_code, 0, dsp_code_size);
		zero2dsp((void *) remap_cpu, dsp_code_size, (128 * 1024u) - dsp_code_size);
		dcache_clean(remap_cpu, 128 * 1024);
		//printhex(remap_cpu, (void *) dsp_code + (64 * 1024), 256);
		//PRINTF("Map local sram to DSP\n");
		// Map local sram to DSP
		SYS_CFG->DSP_BOOT_RAMMAP_REG = 0x00;	/* DSP BOOT SRAM REMAP ENABLE 0: DSP 128K Local SRAM Remap for DSP_SYS */

		// DSP Start address change
		DSP0_CFG->DSP_ALT_RESET_VEC_REG = 0x00028800; //0x400000 if non-cached need
		//DSP0_CFG->DSP_ALT_RESET_VEC_REG = 0x20028000; //0x400000 if non-cached need
		DSP0_CFG->DSP_CTRL_REG0 |= (UINT32_C(1) << 1);	// BIT_START_VEC_SEL
		//PRINTF("DSP_ALT_RESET_VEC_REG=%08" PRIX32 "\n", DSP0_CFG->DSP_ALT_RESET_VEC_REG);

		DSP0_CFG->DSP_CTRL_REG0 |= (UINT32_C(1) << 0);	// Set runstall

		DSP0_CFG->DSP_CTRL_REG0 |= (UINT32_C(1) << 2);	/* set dsp clken */
		CCU->DSP_BGR_REG |= (UINT32_C(1) << 16);	// DSP_RST
		DSP0_CFG->DSP_CTRL_REG0 &= ~ (UINT32_C(1) << 0);	// Clear runstall
		(void) DSP0_CFG->DSP_CTRL_REG0;

		/*
		 * DSP STAT Register
		 */
//		#define BIT_PFAULT_INFO_VALID (0)
//		#define BIT_PFAULT_ERROR (1)
//		#define BIT_DOUBLE_EXCE_ERROR (2)
//		#define BIT_XOCD_MODE (3)
//		#define BIT_DEBUG_MODE (4)
//		#define BIT_PWAIT_MODE (5)
//		#define BIT_IRAM0_LOAD_STORE (6)

		unsigned sss = DSP0_CFG->DSP_STAT_REG;

		local_delay_ms(1300);
		//PRINTF("DSP_STAT_REG=%08" PRIX32 "\n", DSP0_CFG->DSP_STAT_REG);
		for (;;)
			;
	}
#endif
#if CPUSTYLE_VM14 && 0
	{
		unsigned mask = UINT32_C(1) << 24; // GC24 - DBGLED1
		for (;;)
		{
			arm_hardware_pioc_outputs(mask, 1 * mask);
			local_delay_ms(200);
			arm_hardware_pioc_outputs(mask, 0 * mask);
			local_delay_ms(200);
		}
	}
#endif
#if (CPUSTYLE_T113 || CPUSTYLE_F133) && 0
	{
		CCU->GPADC_BGR_REG |= (UINT32_C(1) << 16); 	// 1: De-assert reset  HOSC
		CCU->GPADC_BGR_REG |= (UINT32_C(1) << 0); 	// 1: Pass clock
		GPADC->GP_SR_CON |= (0x2fu << 0);	// set the acquiring time of ADC
		GPADC->GP_SR_CON |= (0x1dfu << 16);	// set the ADC sample frequency divider
		GPADC->GP_CTRL |= (0x2u << 18); 		// set the continuous conversion mode
		GPADC->GP_CS_EN |= (UINT32_C(1) << 0); 		// enable the analog input channel
		GPADC->GP_DATA_INTC |= (UINT32_C(1) << 0);	// enable the GPADC data interrupt
		arm_hardware_set_handler_system(GPADC_IRQn, gpadc_inthandler);
		GPADC->GP_CTRL |= (UINT32_C(1) << 16);		// enable the ADC function
		for(;;) {}
	}
#endif /* #if (CPUSTYLE_T113 || CPUSTYLE_F133) */
#if 0
	{
//		PRINTF("DE_UI1->OVL_SIZE=%08" PRIX32 "\n", DE_UI1->OVL_SIZE);
//		//PRINTF("DE_UI2->OVL_SIZE=%08" PRIX32 "\n", DE_UI2->OVL_SIZE);
//		//PRINTF("DE_UI3->OVL_SIZE=%08" PRIX32 "\n", DE_UI3->OVL_SIZE);
//		DE_UI1->OVL_SIZE = ~ 0u;
//		//DE_UI2->OVL_SIZE = ~ 0u;
//		//DE_UI3->OVL_SIZE = ~ 0u;
//		PRINTF("DE_UI1->OVL_SIZE=%08" PRIX32 "\n", DE_UI1->OVL_SIZE);
//		//PRINTF("DE_UI2->OVL_SIZE=%08" PRIX32 "\n", DE_UI2->OVL_SIZE);
//		//PRINTF("DE_UI3->OVL_SIZE=%08" PRIX32 "\n", DE_UI3->OVL_SIZE);

		PRINTF("G2D_UI0->UI_ATTR=%08" PRIX32 "\n", G2D_UI0->UI_ATTR);
		PRINTF("G2D_UI1->UI_ATTR=%08" PRIX32 "\n", G2D_UI1->UI_ATTR);
		PRINTF("G2D_UI2->UI_ATTR=%08" PRIX32 "\n", G2D_UI2->UI_ATTR);
		G2D_UI0->UI_ATTR = ~ 0u;
		G2D_UI1->UI_ATTR = ~ 0u;
		G2D_UI2->UI_ATTR = ~ 0u;
		PRINTF("G2D_UI0->UI_ATTR=%08" PRIX32 "\n", G2D_UI0->UI_ATTR);
		PRINTF("G2D_UI1->UI_ATTR=%08" PRIX32 "\n", G2D_UI1->UI_ATTR);
		PRINTF("G2D_UI2->UI_ATTR=%08" PRIX32 "\n", G2D_UI2->UI_ATTR);

		for (;;)
			;
	}
#endif
#if 1 && (CPUSTYLE_T113 || CPUSTYLE_F133) && WITHDEBUG && 0
	{
		//	The Hotplug Flag Register is 0x070005DC.
		//	The Soft Entry Address Register is 0x070005E0.

		//	la	a0, 0x02500000
		//	la	a1, 0x23
		//	sb	a1, 0 (a0)
		//	xxx:	j	xxx
		// Test: write byte to 0x02500000 = UART0 data tegister
		static const uint32_t code [] = {
				0x02500537, // 37 05 50 02
				0x0230059B,	// 9B 05 30 02
				0x00B50023, // 23 00 B5 00
				0x0000006F, // 6F 00 00 00,
		};

		* (volatile uint32_t *) 0x070005DC = 1*UINT32_C(0xFA50392F);	// Hotplug Flag Register
		* (volatile uint32_t *) 0x070005E0 = (uintptr_t) code;	// Soft Entry Address Register

		dcache_clean_all();

		//printhex32(0x070005DC, (void *) 0x070005DC, 4);
		//printhex32(0x070005E0, (void *) 0x070005E0, 4);

		//printhex(0x06000000, (void *) 0x06000000, 0x10000);
		CCU->RISC_GATING_REG = 1*(UINT32_C(1) << 31) | 0x16AA;	/* key required for modifications (d1-h_user_manual_v1.0.pdf, page 152). */
		CCU->RISC_CFG_BGR_REG |= (UINT32_C(1) << 16) | (UINT32_C(1) << 0);

		PRINTF("RISC_CFG->WORK_MODE_REG=%08" PRIX32 "\n", RISC_CFG->WORK_MODE_REG);

		RISC_CFG->RISC_STA_ADD0_REG = (uintptr_t) code;
		RISC_CFG->RISC_STA_ADD1_REG = 0;//(uint32_t) (uintptr_t) code >> 32;
		//memset(RISC_CFG, ~ 0u, sizeof * RISC_CFG);

		PRINTF("allwnrf133_get_riscv_freq()=%" PRIuFAST32 "\n", allwnrf133_get_riscv_freq());
		PRINTF("CCU->RISC_GATING_REG=%08" PRIX32 ", CCU->RISC_CFG_BGR_REG=%08" PRIX32 "\n", CCU->RISC_GATING_REG, CCU->RISC_CFG_BGR_REG);
		PRINTF("RISC_CFG->RISC_STA_ADD0_REG=%08" PRIX32 ", RISC_CFG->RISC_STA_ADD1_REG=%08" PRIX32 "\n", RISC_CFG->RISC_STA_ADD0_REG, RISC_CFG->RISC_STA_ADD1_REG);
		//printhex32(RISC_CFG_BASE, RISC_CFG, sizeof * RISC_CFG);
		local_delay_ms(3000);
		//((void (*) (void)) code)();		/* test code invokation for risc-v here */
		CCU->RISC_RST_REG = (UINT32_C(0x16AA) << 16) | 0x01;	/* tested on Allwinner F133 */
		//TP();
		//printhex32(RISC_CFG_BASE, RISC_CFG, sizeof * RISC_CFG);
		for (;;)
			;
	}
#endif
#if 0 && CPUSTYLE_CA53
	{
		const uint_fast32_t ca53_cbar = __get_CA53_CBAR();
		const uint64_t periphbase = (uint64_t) (ca53_cbar & UINT32_C(0xFFFC0000)) | ((uint64_t) (ca53_cbar & UINT32_C(0xFF)) << 32);

		//PRINTF("__get_CBAR()=%08X\n", (unsigned) __get_CBAR());
		PRINTF("__get_CPUACTLR()=%08X\n", (unsigned) __get_CPUACTLR());
		PRINTF("__get_CPUECTLR()=%08X\n", (unsigned) __get_CPUECTLR());
		PRINTF("__get_CA53_CBAR()=%08X\n", (unsigned) ca53_cbar);			/* SYS_CFG_BASE */
		PRINTF("periphbase=%016" PRIX64 "\n", periphbase);			/* SYS_CFG_BASE */

		PRINTF("GIC_DISTRIBUTOR_BASE=%08X\n", (unsigned) GIC_DISTRIBUTOR_BASE);
		PRINTF("GIC_INTERFACE_BASE=%08X\n", (unsigned) GIC_INTERFACE_BASE);
		//printhex(ca53_cbar, ca53_cbar, 256);

		ASSERT(GIC_DISTRIBUTOR_BASE == (periphbase + 0x81000));
		ASSERT(GIC_INTERFACE_BASE == (periphbase + 0x82000));
	}
#endif
#if 0 && (__CORTEX_A == 7U)
	{
		const uint_fast32_t cbar = __get_CBAR();
		PRINTF("__get_CBAR()=%08X\n", (unsigned) __get_CBAR());

	}
#endif
#if 0 && CPUSTYLE_A64
	{
		PRINTF("C0_CPUX_CFG->C_CTRL_REG0=%08X\n", (unsigned) C0_CPUX_CFG->C_CTRL_REG0);
		PRINTF("C0_CPUX_CFG->GENER_CTRL_REG0=%08X\n", (unsigned) C0_CPUX_CFG->GENER_CTRL_REG0);
		PRINTF("C0_CPUX_CFG->C_CPU_STATUS=%08X\n", (unsigned) C0_CPUX_CFG->C_CPU_STATUS);

		C0_CPUX_CFG->GENER_CTRL_REG0 &= ~ (UINT32_C(1) << 4);	// GICCDISABLE
		C0_CPUX_CFG->C_CPU_STATUS |= (0x0Fu << 24);		// SMP
		C0_CPUX_CFG->C_CTRL_REG0 |= (0x0Fu << 24);		// AA64nAA32 1: AArch64

		PRINTF("C0_CPUX_CFG->C_CTRL_REG0=%08X\n", (unsigned) C0_CPUX_CFG->C_CTRL_REG0);
		PRINTF("C0_CPUX_CFG->GENER_CTRL_REG0=%08X\n", (unsigned) C0_CPUX_CFG->GENER_CTRL_REG0);
		PRINTF("C0_CPUX_CFG->C_CPU_STATUS=%08X\n", (unsigned) C0_CPUX_CFG->C_CPU_STATUS);
	}
#endif
#if 0 && LCDMODE_LTDC
	{
		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
		TP();

		#include "Cobra.png.h"

		testpng(Cobra_png);
		for (;;)
			;
	}
#endif
#if 1 && (CPUSTYLE_T507 || CPUSTYLE_H616)
	{
		PRINTF("PSI_AHB1_AHB2_CFG_REG=%08X\n", (unsigned) CCU->PSI_AHB1_AHB2_CFG_REG);
		PRINTF("CPU_FREQ=%u MHz\n", (unsigned) (CPU_FREQ / 1000 / 1000));
		PRINTF("allwnr_t507_get_axi_freq()=%u MHz\n", (unsigned) (allwnr_t507_get_axi_freq() / 1000 / 1000));
		PRINTF("allwnr_t507_get_mbus_freq()=%u MHz\n", (unsigned) (allwnr_t507_get_mbus_freq() / 1000 / 1000));
		PRINTF("allwnr_t507_get_psi_ahb1_ahb2_freq()=%u MHz\n", (unsigned) (allwnr_t507_get_psi_ahb1_ahb2_freq() / 1000 / 1000));
		PRINTF("allwnr_t507_get_apb1_freq()=%u MHz\n", (unsigned) (allwnr_t507_get_apb1_freq() / 1000 / 1000));
		PRINTF("allwnr_t507_get_apb2_freq()=%u MHz\n", (unsigned) (allwnr_t507_get_apb2_freq() / 1000 / 1000));
	}
#endif
#if 0
	{
		TP();
		//de_dump();
		testde();
		TP();
	}
#endif
#if 0 && LCDMODE_LTDC
	{
		enum { picy = 110, picx = 150 };
		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
		TP();
		static PACKEDCOLORPIP_T layer0_a [GXSIZE(DIM_X, DIM_Y)];
		static PACKEDCOLORPIP_T layer0_b [GXSIZE(DIM_X, DIM_Y)];
		static PACKEDCOLORPIP_T layer1 [GXSIZE(DIM_X, DIM_Y)];
		static PACKEDCOLORPIP_T layer2 [GXSIZE(DIM_X, DIM_Y)];
		static PACKEDCOLORPIP_T layer3 [GXSIZE(DIM_X, DIM_Y)];
		static PACKEDCOLORPIP_T fbpic [GXSIZE(picx, picy)];
		static PACKEDCOLORPIP_T fbpic2 [GXSIZE(picx, picy)];
		static PACKEDCOLORPIP_T fbpic3 [GXSIZE(picx, picy)];

//		dcache_clean_invalidate((uintptr_t) layer0, sizeof layer0);
//		dcache_clean_invalidate((uintptr_t) layer1, sizeof layer1);
//		dcache_clean_invalidate((uintptr_t) fbpic, sizeof fbpic);

		hardware_ltdc_main_set4((uintptr_t) layer0_a, (uintptr_t) layer1, 0*(uintptr_t) layer2, 0*(uintptr_t) layer3);

		/* Тестовое изображение для заполнения с color key (с фоном в этом цвете) */
		COLORPIP_T keycolor = COLORPIP_KEY;
//		PRINTF("test: keycolor=%08X\n", keycolor);
//		PRINTF("test: a=%08X\n", COLORPIP_A(keycolor));
//		PRINTF("test: r=%08X\n", COLORPIP_R(keycolor));
//		PRINTF("test: g=%08X\n", COLORPIP_G(keycolor));
//		PRINTF("test: b=%08X\n", COLORPIP_B(keycolor));

		unsigned picalpha = 128;
		colpip_fillrect(fbpic, picx, picy, 0, 0, picx, picy, TFTALPHA(picalpha, keycolor));	/* при alpha==0 все биты цвета становятся 0 */
		colpip_fillrect(fbpic, picx, picy, picx / 4, picy / 4, picx / 2, picy / 2, TFTALPHA(picalpha, COLORPIP_WHITE));
		colpip_line(fbpic, picx, picy, 0, 0, picx - 1, picy - 1, TFTALPHA(picalpha, COLORPIP_WHITE), 0);
		colpip_line(fbpic, picx, picy, 0, picy - 1, picx - 1, 0, TFTALPHA(picalpha, COLORPIP_WHITE), 0);
		colpip_fillrect(fbpic, picx, picy, picx / 4 + 5, picy / 4 + 5, picx / 2 - 10, picy / 2 - 10, TFTALPHA(0 * picalpha, COLORPIP_WHITE));
		colpip_string_tbg(fbpic, picx, picy, 5, 6, "Hello!", TFTALPHA(picalpha, COLORPIP_WHITE));
		dcache_clean((uintptr_t) fbpic, GXSIZE(picx, picy) * sizeof fbpic [0]);

		unsigned pic2alpha = 44;
		colpip_fillrect(fbpic2, picx, picy, 0, 0, picx, picy, TFTALPHA(pic2alpha, keycolor));	/* при alpha==0 все биты цвета становятся 0 */
		colpip_fillrect(fbpic2, picx, picy, picx / 4, picy / 4, picx / 2, picy / 2, TFTALPHA(pic2alpha, COLORPIP_WHITE));
		colpip_line(fbpic2, picx, picy, 0, 0, picx - 1, picy - 1, TFTALPHA(pic2alpha, COLORPIP_WHITE), 0);
		colpip_line(fbpic2, picx, picy, 0, picy - 1, picx - 1, 0, TFTALPHA(pic2alpha, COLORPIP_WHITE), 0);
		colpip_string_tbg(fbpic2, picx, picy, 5, 6, "LY2", TFTALPHA(pic2alpha, COLORPIP_WHITE));
		dcache_clean((uintptr_t) fbpic2, GXSIZE(picx, picy) * sizeof fbpic2 [0]);

		unsigned pic3alpha = 33;
		colpip_fillrect(fbpic3, picx, picy, 0, 0, picx, picy, TFTALPHA(pic3alpha, keycolor));	/* при alpha==0 все биты цвета становятся 0 */
		colpip_fillrect(fbpic3, picx, picy, picx / 4, picy / 4, picx / 2, picy / 2, TFTALPHA(pic3alpha, COLORPIP_WHITE));
		colpip_line(fbpic3, picx, picy, 0, 0, picx - 1, picy - 1, TFTALPHA(pic3alpha, COLORPIP_WHITE), 0);
		colpip_line(fbpic3, picx, picy, 0, picy - 1, picx - 1, 0, TFTALPHA(pic3alpha, COLORPIP_WHITE), 0);
		colpip_string_tbg(fbpic3, picx, picy, 5, 6, "LY3", TFTALPHA(pic3alpha, COLORPIP_WHITE));

		/* непрозрачный фон */
		unsigned bgalpha = 128;
		colpip_fillrect(layer0_a, DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, TFTALPHA(bgalpha, COLORPIP_BLACK));	/* opaque color transparent black */
		colpip_fillrect(layer0_b, DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, TFTALPHA(bgalpha, COLORPIP_BLACK));	/* opaque color transparent black */
		/* непрозрачный прямоугольник на фоне */
		colpip_fillrect(layer0_a, DIM_X, DIM_Y, 10, 10, 400, 300, TFTALPHA(bgalpha, COLORPIP_RED));	// RED - нижний слой не учитывает прозрачность
		colpip_fillrect(layer0_b, DIM_X, DIM_Y, 10, 10, 400, 300, TFTALPHA(bgalpha, COLORPIP_RED));	// RED - нижний слой не учитывает прозрачность

		/* полупрозрачный фон */
		unsigned fgalpha = 128;
		colpip_fillrect(layer1, DIM_X, DIM_Y, 110, 110, DIM_X - 200, DIM_Y - 200, TFTALPHA(fgalpha, COLORPIP_BLUE));	/* transparent black */
		/* полупрозрачный прямоугольник на фоне */
		colpip_fillrect(layer1, DIM_X, DIM_Y, 120, 120, 200, 200, TFTALPHA(fgalpha, COLORPIP_GREEN));	// GREEN
		/* прозрачный слой */
		unsigned l2alpha = 0;
		colpip_fillrect(layer2, DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, TFTALPHA(l2alpha, COLORPIP_RED));	/* opaque color transparent black */
		/* прозрачный слой */
		unsigned l3alpha = 0;
		colpip_fillrect(layer3, DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, TFTALPHA(l2alpha, COLORPIP_GREEN));	/* opaque color transparent black */

		TP();
		/* копируем изображение в верхний слой с цветовым ключем */
		colpip_bitblt(
				(uintptr_t) layer1, GXSIZE(DIM_X, DIM_Y) * sizeof layer1 [0],
				layer1, DIM_X, DIM_Y,
				220, 220,
				(uintptr_t) fbpic, GXSIZE(picx, picy) * sizeof fbpic [0],
				fbpic, picx, picy,
				0, 0,	// координаты окна источника
				picx, picy, // размер окна источника
				BITBLT_FLAG_NONE | BITBLT_FLAG_CKEY, keycolor
				);

		TP();
		/* копируем изображение в верхний слой БЕЗ цветового ключа */
		colpip_bitblt(
				(uintptr_t) layer1, GXSIZE(DIM_X, DIM_Y) * sizeof layer1 [0],
				layer1, DIM_X, DIM_Y,
				350, 250,
				(uintptr_t) fbpic, GXSIZE(picx, picy) * sizeof fbpic [0],
				fbpic, picx, picy,
				0, 0,	// координаты окна источника
				picx, picy, // размер окна источника
				BITBLT_FLAG_NONE, keycolor
				);

		TP();
		/* копируем изображение в верхний слой БЕЗ цветового ключа */
		colpip_stretchblt(
				(uintptr_t) layer1, GXSIZE(DIM_X, DIM_Y) * sizeof layer1 [0],
				layer1, DIM_X, DIM_Y,
				40, 20, picx * 5 / 2, picy,
				(uintptr_t) fbpic, GXSIZE(picx, picy) * sizeof fbpic [0],
				fbpic, picx, picy,
				0, 0,	// координаты источника
				picx, picy,	// размеры источника
				BITBLT_FLAG_NONE | BITBLT_FLAG_CKEY, keycolor
				);

		TP();
		/* копируем изображение в верхний слой БЕЗ цветового ключа */
		colpip_stretchblt(
				(uintptr_t) layer1, GXSIZE(DIM_X, DIM_Y) * sizeof layer1 [0],
				layer1, DIM_X, DIM_Y,
				450, 250, picx * 3 / 2, picy * 3 / 2,
				(uintptr_t) fbpic, GXSIZE(picx, picy) * sizeof fbpic [0],
				fbpic, picx, picy,
				0, 0,	/* координаты источника */
				picx, picy,	// размеры источника
				BITBLT_FLAG_NONE | BITBLT_FLAG_CKEY, keycolor
				);

		TP();
		/* копируем изображение в верхний слой БЕЗ цветового ключа */
		colpip_stretchblt(
				(uintptr_t) layer1, GXSIZE(DIM_X, DIM_Y) * sizeof layer1 [0],
				layer1, DIM_X, DIM_Y,
				170, 220, picx * 2 / 3, picy * 2 / 3,
				(uintptr_t) fbpic, GXSIZE(picx, picy) * sizeof fbpic [0],
				fbpic, picx, picy,
				0, 0,	/* координаты источника */
				picx, picy,	// размеры источника
				BITBLT_FLAG_NONE | BITBLT_FLAG_CKEY, keycolor
				);

		TP();
		/* копируем изображение в верхний слой с цветовым ключем */
		colpip_bitblt(
				(uintptr_t) layer1, GXSIZE(DIM_X, DIM_Y) * sizeof layer1 [0],
				layer1, DIM_X, DIM_Y,
				90, 90,
				(uintptr_t) fbpic, GXSIZE(picx, picy) * sizeof fbpic [0],
				fbpic, picx, picy,
				0, 0,	// координаты окна источника
				picx, picy, // размер окна источника
				BITBLT_FLAG_NONE | BITBLT_FLAG_CKEY, keycolor
				);

		TP();
		/* копируем изображение в 2-й слой с цветовым ключем */
		colpip_bitblt(
				(uintptr_t) layer1, GXSIZE(DIM_X, DIM_Y) * sizeof layer1 [0],
				layer2, DIM_X, DIM_Y,
				30, 30,
				(uintptr_t) fbpic, GXSIZE(picx, picy) * sizeof fbpic [0],
				fbpic2, picx, picy,
				0, 0,	// координаты окна источника
				picx, picy, // размер окна источника
				BITBLT_FLAG_NONE | BITBLT_FLAG_CKEY, keycolor
				);

		TP();
		/* копируем изображение в 3-й слой с цветовым ключем */
		colpip_bitblt(
				(uintptr_t) layer1, GXSIZE(DIM_X, DIM_Y) * sizeof layer1 [0],
				layer3, DIM_X, DIM_Y,
				370, 20,
				(uintptr_t) fbpic, GXSIZE(picx, picy) * sizeof fbpic [0],
				fbpic3, picx, picy,
				0, 0,	// координаты окна источника
				picx, picy, // размер окна источника
				BITBLT_FLAG_NONE | BITBLT_FLAG_CKEY, keycolor
				);

		// нужно если программно заполняли
		dcache_clean((uintptr_t) layer0_a, sizeof layer0_a);
		dcache_clean((uintptr_t) layer0_b, sizeof layer0_b);
		dcache_clean((uintptr_t) layer1, sizeof layer1);
//
//		printhex32((uintptr_t) layer0, layer0, 64);
//		printhex32((uintptr_t) layer1, layer1, 64);


		TP();
		testde();
		TP();

		int phase = 0;
		unsigned c = 0;
		while(1)
		{
			int y = 50;
			int x0 = 270;
			int h = 120;
			int w = 500;
			int xpos = (c * (w - 1)) / 255;	/* позиция маркера */

			PACKEDCOLORPIP_T * const drawlayer = phase ? layer0_a : layer0_b;

			colpip_fillrect(drawlayer, DIM_X, DIM_Y, x0, y, w, h, TFTALPHA(bgalpha, COLORPIP_BLACK));
			/* линия в один пиксель рисуется прораммно - за ней требуется flush,
			 * поскольку потом меняется еще аппаратурой - invalidate
			 * */
			colpip_fillrect(drawlayer, DIM_X, DIM_Y, x0 + xpos, y, 1, h, TFTALPHA(bgalpha, COLORPIP_WHITE));
			dcache_clean_invalidate((uintptr_t) drawlayer, sizeof * drawlayer * GXSIZE(DIM_X, DIM_Y));

			hardware_ltdc_main_set4((uintptr_t) drawlayer, 1*(uintptr_t) layer1, 0*(uintptr_t) layer2, 0*(uintptr_t) layer3);

			phase = ! phase;
			c = (c + 1) % 256;
		}
		for (;;)
			;
	}
#endif
#if 0 && CPUSTYLE_T113
	{
		//	#define DSP0_IRAM_BASE 			((uintptr_t) 0x00028000)			/* 32KB */
		//	#define DSP0_DRAM_BASE 			((uintptr_t) 0x00030000)			/* 32KB */
		// При 0 видим память DSP
		// При 1 видим память что была при загрузке
		// 0: DSP 128K Local SRAM Remap for DSP_SYS
		// 1: DSP 128K Local SRAM Remap for System Boot
		// After system boots up, this bit must be set to 0 before using DSP
		PRINTF("SYS_CFG->DSP_BOOT_RAMMAP_REG=%08" PRIX32 "\n", SYS_CFG->DSP_BOOT_RAMMAP_REG);
		SYS_CFG->DSP_BOOT_RAMMAP_REG = 1;
		PRINTF("SYS_CFG->DSP_BOOT_RAMMAP_REG=%08" PRIX32 "\n", SYS_CFG->DSP_BOOT_RAMMAP_REG);

		uint8_t * const irambase = (void *) DSP0_IRAM_BASE;
		TP();
		irambase [0] = 0xDE;
		irambase [1] = 0xAD;
		irambase [2] = 0xBE;
		irambase [3] = 0xEF;
		printhex(DSP0_IRAM_BASE, irambase, 64);

		uint8_t * const drambase = (void *) DSP0_DRAM_BASE;
		TP();
		drambase [0] = 0xAB;
		drambase [1] = 0xBA;
		drambase [2] = 0x19;
		drambase [3] = 0x80;
		printhex(DSP0_DRAM_BASE, drambase, 64);
		TP();
		PRINTF("allwnrt113_get_dsp_freq()=%" PRIuFAST32 "\n", allwnrt113_get_dsp_freq());

//		CCU->DSP_BGR_REG |= UINT32_C(1) << 18;	// DSP_DBG_RST 1: De-assert
//		CCU->DSP_BGR_REG |= UINT32_C(1) << 17;	// DSP_CFG_RST 1: De-assert
//		CCU->DSP_BGR_REG |= UINT32_C(1) << 16;	// DSP_RST 1: De-assert
//		CCU->DSP_BGR_REG |= UINT32_C(1) << 1;	// DSP_CFG_GATING 1: Pass
	}
#endif
#if 0 && (CPUSTYLE_T113 || CPUSTYLE_F133)
	{
		PRINTF("allwnrt113_get_pll_cpu_freq()=%" PRIuFAST64 "\n", allwnrt113_get_pll_cpu_freq());
		PRINTF("allwnrt113_get_pll_ddr_freq()=%" PRIuFAST64 "\n", allwnrt113_get_pll_ddr_freq());
		PRINTF("allwnrt113_get_g2d_freq()=%" PRIuFAST32 "\n", allwnrt113_get_g2d_freq());
		PRINTF("allwnrt113_get_de_freq()=%" PRIuFAST32 "\n", allwnrt113_get_de_freq());
		PRINTF("allwnrt113_get_ce_freq()=%" PRIuFAST32 "\n", allwnrt113_get_ce_freq());
		PRINTF("allwnrt113_get_ve_freq()=%" PRIuFAST32 "\n", allwnrt113_get_ve_freq());
		PRINTF("allwnrt113_get_di_freq()=%" PRIuFAST32 "\n", allwnrt113_get_di_freq());
	}
#endif
#if 0 && defined (CLINT) && CPUSTYLE_F133
	{
		TP();
		//csr_set_bits_mcounteren(MCOUNTEREN_CY_BIT_MASK | MCOUNTEREN_TM_BIT_MASK | MCOUNTEREN_IR_BIT_MASK);

		PRINTF("mtimer_get_raw_time_cmp = 0x%" PRIX64 "\n", mtimer_get_raw_time_cmp());
		PRINTF("mtimer_get_raw_time_cmp = %" PRIu64 "\n", mtimer_get_raw_time_cmp());

		mtimer_set_raw_time_cmp(1000);
	    //mtimer_set_raw_time_cmp(mtimer_get_raw_time() + 24000000);
		PRINTF("mtimer_get_raw_time_cmp = 0x%" PRIX64 "\n", mtimer_get_raw_time_cmp());
		PRINTF("mtimer_get_raw_time_cmp = %" PRIu64 "\n", mtimer_get_raw_time_cmp());

		mtimer_set_raw_time_cmp(0x12345678DEADBEEF);
		PRINTF("mtimer_get_raw_time_cmp = 0x%" PRIX64 "\n", mtimer_get_raw_time_cmp());
		PRINTF("mtimer_get_raw_time_cmp = %" PRIu64 "\n", mtimer_get_raw_time_cmp());

		// https://chromitem-soc.readthedocs.io/en/latest/clint.html
		PRINTF("mtimecmp=0x%08" PRIX32 "%08" PRIX32"\n",  CLINT->mtimecmphi,  CLINT->mtimecmplo);

	    mtimer_set_raw_time_cmp(csr_read_mcycle() + 20ll * CPU_FREQ);
		PRINTF("mtimer_get_raw_time_cmp = 0x%" PRIX64 "\n", mtimer_get_raw_time_cmp());
		PRINTF("mtimer_get_raw_time_cmp = %" PRIu64 "\n", mtimer_get_raw_time_cmp());
		TP();
		const uintptr_t a = CLINT_BASE + 0xB000;
		PRINTF("mtimecmp=0x%08" PRIX32 "%08" PRIX32"\n",  CLINT->mtimecmphi,  CLINT->mtimecmplo);
		//printhex32(a, (void *) a, 0x10000);
		// https://chromitem-soc.readthedocs.io/en/latest/clint.html
		PRINTF("mtimecmp=0x%08" PRIX32 "%08" PRIX32"\n",  CLINT->mtimecmphi,  CLINT->mtimecmplo);
		//PRINTF("mtime=0x%08" PRIX32 "%08" PRIX32 "\n",  CLINT->mtimehi,  CLINT->mtimelo);
	}
#endif
#if 0 && (CPUSTYLE_T113 || CPUSTYLE_F133)
	{
		/* Allwinner G2D tests */
		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
		TP();

		display2_bgreset();

		/* Запуск теста одного из */

		void g2d_main_layers_alpha(void);
		void g2d_main0(void);

		//g2d_main0();
		g2d_main_layers_alpha();
	}
#endif
#if 0 && LCDMODE_COLORED && ! DSTYLE_G_DUMMY
	{
		const COLORPIP_T bg = display_getbgcolor();
		PACKEDCOLORPIP_T * const buffer = colmain_fb_draw();

		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
//		TP();
//		colpip_fillrect(buffer, DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, 0xFF00);
//		TP();
//		colpip_fillrect(buffer, DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, 0xFF00);
//		TP();
//		colpip_fillrect(buffer, DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, 0xFF00);
//		for (;;)
//			;

		for (int pos = 0; pos < 24; ++ pos)
		{
			COLORPIP_T c = TFTALPHA(255, UINT32_C(1) << pos);
			colpip_fillrect(buffer, DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, c);
			PRINTF("color=%08X pos=%d\n", (unsigned) c, pos);
			local_delay_ms(2000);
		}
		for (;;)
			;

	}
#endif
#if 0 && WITHDEBUG
	{
		PTT_INITIALIZE();
		for (;;)
		{
			unsigned ptt = (PTT_TARGET_PIN & PTT_BIT_PTT) == 0;
			unsigned ptt2 = (PTT2_TARGET_PIN & PTT2_BIT_PTT) == 0;
			unsigned ptt3 = (PTT3_TARGET_PIN & PTT3_BIT_PTT) == 0;
			PRINTF("ptt=%u ptt2=%u, ptt3=%u\n", ptt, ptt2, ptt3);
		}
	}
#endif
#if 0 && WITHDEBUG && (CPUSTYLE_T113 || CPUSTYLE_F133)
	{
		// Allwinner t113-s3 boot mode display

//		CCU->CE_CLK_REG |= (1uL << 31);	// CE_CLK_GATING
//		CCU->MBUS_MAT_CLK_GATING_REG |= (UINT32_C(1) << 2);	// CE_MCLK_EN
		// bits 27:16: eFUSE boot select status,
		// bit 0: 0: GPIO boot select, 1: eFuse boot select
		// The status of the GPIO boot select pin can be read by the bit[12:11] of the system configuration module (register: 0x03000024).
		PRINTF("SID->BOOT_MODE=0x%08X, SYS_CFG->VER_REG=0x%08X\n", (unsigned) SID->BOOT_MODE, (unsigned) SYS_CFG->VER_REG);
		PRINTF("SID->SID_THS=0x%08X\n", (unsigned) SID->SID_THS);
		PRINTF("BOOT_MODE=%u, BOOT_SEL_PAD_STA=0%u FEL_SEL_PAD_STA=%u\n", (unsigned) (SID->BOOT_MODE >> 0) & 0x01, (unsigned) (SYS_CFG->VER_REG >> 11) & 0x03, (unsigned) (SYS_CFG->VER_REG >> 8) & 0x01);
		//printhex32(SID_BASE, SID, sizeof * SID);
	}
#endif
#if 0 && (CPUSTYLE_T113 || CPUSTYLE_F133)
	{
		RTC->FBOOT_INFO_REG0 = (UINT32_C(1) << 28);	// Try process: SMHC0->SPI NOR->SPI NAND->EMMC2_USER->EMMC2_BOOT.
	}
#endif
#if 0 && (WIHSPIDFSW || WIHSPIDFHW || WIHSPIDFOVERSPI)
	{
		// QSPI test
		unsigned char b [64];

		testchipDATAFLASH();	// устанока кодов опрерации для скоростных режимов

		memset(b, 0xE5, sizeof b);
		readDATAFLASH(0x000000, b, ARRAY_SIZE(b));
		printhex(0, b, ARRAY_SIZE(b));

		testchipDATAFLASH();	// устанока кодов опрерации для скоростных режимов

		memset(b, 0xE5, sizeof b);
		readDATAFLASH(0x00080000, b, ARRAY_SIZE(b));
		printhex(0x00080000, b, ARRAY_SIZE(b));
	}
#endif
#if 0 && WITHTWISW && WITHDEBUG
	{
		PRINTF("I2C wires test\n");
		TWISOFT_INITIALIZE();
		for (;;)
		{
			SET_TWCK();
			SET_TWD();
			local_delay_ms(1000);
			CLR_TWCK();
			SET_TWD();
			local_delay_ms(1000);
			SET_TWCK();
			CLR_TWD();
			local_delay_ms(1000);
			CLR_TWCK();
			CLR_TWD();
			local_delay_ms(1000);
		}
	}
#endif
#if 0
	{
		// Test for ADIS16IMU1/PCB
		// SYS_MODE_CURR (Страница 1, адрес 0x36)

		// Wait for module ready
		for (;;)
		{
			const unsigned SYS_E_FLAG = adis161xx_read16(0x00, 0x08);
			const unsigned PROD_ID = adis161xx_read16(0x00, 0x7E);
			if (PROD_ID == 0x4060 && SYS_E_FLAG == 0)
				break;
			PRINTF("SYS_E_FLAG=%04X, Waiting for PROD_ID=0x4060 (%04X)\n", SYS_E_FLAG, PROD_ID);

		}
		for (;;)
		{
			enum { TEMP_BP = 5 };	// binary point of temperature
			const unsigned TEMP_OUT = 0x297B;//adis161xx_read16(0x00, 0x0E);
			const unsigned SYS_E_FLAG = adis161xx_read16(0x00, 0x08);
			const unsigned FIRM_Y = adis161xx_read16(0x03, 0x7C);
			const unsigned FIRM_DM = adis161xx_read16(0x03, 0x7A);
			const unsigned FIRM_REV = adis161xx_read16(0x03, 0x78);
			const unsigned SERIAL_NUM = adis161xx_read16(0x04, 0x20);

			PRINTF("DECLN_ANGL=%04X, SYS_E_FLAG=%04X, FIRM_Y=%04X, FIRM_DM=%04X, FIRM_REV=%04X, SERIAL_NUM=%04X, TEMP_OUT=%d / 10\n", adis161xx_read16(0x03, 0x54), adis161xx_read16(0x00, 0x08), FIRM_Y, FIRM_DM, FIRM_REV, SERIAL_NUM, TEMP_OUT, ((int16_t) TEMP_OUT + (0*250 << TEMP_BP)) >> TEMP_BP);

			for (;;)
			{
				// Получение компонент кватерниона ориентации
				int16_t Q0_C11_OUT =  adis161xx_read16(0x00, 0x60);	// Компонент λ0 кватерниона ориентации
				int16_t Q1_C12_OUT =  adis161xx_read16(0x00, 0x62);	// Компонент λ1 кватерниона ориентации
				int16_t Q2_C13_OUT =  adis161xx_read16(0x00, 0x64);	// Компонент λ2 кватерниона ориентации
				int16_t Q3_C21_OUT =  adis161xx_read16(0x00, 0x66);	// Компонент λ3 кватерниона ориентации

				int16_t ROLL_C23_OUT = adis161xx_read16(0x00, 0x6A);
				int16_t PITCH_C31_OUT = adis161xx_read16(0x00, 0x6C);
				int16_t YAW_C32_OUT = adis161xx_read16(0x00, 0x6E);

				//PRINTF("Q0=%f, Q1=%f, Q2=%f, Q3=%f\n", Q0_C11_OUT / 32768.0f, Q1_C12_OUT / 32768.0f, Q2_C13_OUT / 32768.0f, Q3_C21_OUT / 32768.0f);

				float x = Q0_C11_OUT / 32768.0f;
				float y = Q1_C12_OUT / 32768.0f;
				float z = Q2_C13_OUT / 32768.0f;
				float w = Q3_C21_OUT / 32768.0f;

				// https://coderoad.ru/53033620/%D0%9A%D0%B0%D0%BA-%D0%BF%D1%80%D0%B5%D0%BE%D0%B1%D1%80%D0%B0%D0%B7%D0%BE%D0%B2%D0%B0%D1%82%D1%8C-%D1%83%D0%B3%D0%BB%D1%8B-%D0%AD%D0%B9%D0%BB%D0%B5%D1%80%D0%B0-%D0%B2-%D0%BA%D0%B2%D0%B0%D1%82%D0%B5%D1%80%D0%BD%D0%B8%D0%BE%D0%BD%D1%8B-%D0%B8-%D0%BF%D0%BE%D0%BB%D1%83%D1%87%D0%B8%D1%82%D1%8C-%D1%82%D0%B5-%D0%B6%D0%B5-%D1%83%D0%B3%D0%BB%D1%8B-%D0%AD%D0%B9%D0%BB%D0%B5%D1%80%D0%B0
		        float t0 = +2.0f * (w * x + y * z);
				float t1 = +1.0f - 2.0f * (x * x + y * y);
		        float X = atan2f(t0, t1);

		        float t2;
		        t2 = +2.0f * (w * y - z * x);
		        t2 = t2 > +1.0f ? +1.0f : t2;
		        t2 = t2 < -1.0f ? -1.0f : t2;
		        float Y = asinf(t2);

		        float t3 = +2.0f * (w * z + x * y);
		        float t4 = +1.0f - 2.0f * (y * y + z * z);
		        float Z = atan2f(t3, t4);

		        //PRINTF("X=%f, Y=%f, Z=%f\n", X * (180 / M_PI), Y * (180 / M_PI), Z * (180 / M_PI));
		        PRINTF("HABS=%f, ROLL=%f, PITCH=%f, YAW=%f, BAROM=%f\n",
						(int32_t) adis161xx_read32(0x01, 0x14) / 65536.0f,
		        		ROLL_C23_OUT / 32768.0f * 180.0f,
						PITCH_C31_OUT / 32768.0f * 180.0f,
						YAW_C32_OUT / 32768.0f * 180.0f,
						(int32_t) adis161xx_read32(0x00, 0x2E) / 65536.0f / 25000
						);

			}
			{
				unsigned PG = 0x01;
				//unsigned AE = 0x10;	// LATITUDE_LOW, LATITUDE_OUT
				unsigned AE = 0x24;	// HABS_LOW, HABS_OUT
//				adis161xx_write16(PG, AE, 0xDEAD);
//				adis161xx_write16(PG, AE + 2, 0xBEEF);
				const unsigned LATITUDE_LOW = adis161xx_read16(PG, AE);
				const unsigned LATITUDE_OUT = adis161xx_read16(PG, AE + 2);
				const unsigned LATITUDE4 = adis161xx_read32(PG, AE);
				PRINTF("LOW=%04X, OUT=%04X, 32W=%08X\n", LATITUDE_LOW, LATITUDE_OUT, LATITUDE4);
			}
			unsigned PG = 0x02;
			for (PG = 0; PG < 4; PG += 1)
			{
				unsigned AE;
				for (AE = 0; AE < 128; AE += 4)
				{
					//unsigned AE = 0x10;	// LATITUDE_LOW, LATITUDE_OUT
	//				adis161xx_write16(PG, AE, 0xDEAD);
	//				adis161xx_write16(PG, AE + 2, 0xBEEF);
					const unsigned LATITUDE_LOW = adis161xx_read16(PG, AE);
					const unsigned LATITUDE_OUT = adis161xx_read16(PG, AE + 2);
					const unsigned LATITUDE4 = adis161xx_read32(PG, AE);
					PRINTF("pg=%02X, ae=%02X: LOW=%04X, OUT=%04X, 32W=%08X\n", PG, AE, LATITUDE_LOW, LATITUDE_OUT, LATITUDE4);
				}
			}

//			PRINTF("Write DECLN_ANGL\n");
//			PRINTF("DECLN_ANGL=%04X, SYS_E_FLAG=%04X\n",  adis161xx_read16(0x03, 0x54), adis161xx_read16(0x00, 0x08));
//			adis161xx_write16(0x03, 0x54, 0x0777);
//			PRINTF("DECLN_ANGL=%04X, SYS_E_FLAG=%04X\n",  adis161xx_read16(0x03, 0x54), adis161xx_read16(0x00, 0x08));
//			adis161xx_write16(0x03, 0x54, 0x0888);
//			PRINTF("DECLN_ANGL=%04X, SYS_E_FLAG=%04X\n",  adis161xx_read16(0x03, 0x54), adis161xx_read16(0x00, 0x08));

			char c;
			while (dbg_getchar(& c) == 0)
				;

//			PRINTF("EXT_DATA_SRC #1=%04X\n", adis161xx_read16(0x01, 0x2C));
//			adis161xx_write16(0x01, 0x2C, 0x01);
//			local_delay_ms(10);
//			PRINTF("EXT_DATA_SRC #2=%04X\n", adis161xx_read16(0x01, 0x2C));
//			adis161xx_write16(0x01, 0x2C, 0x03);
//			local_delay_ms(10);
//			PRINTF("EXT_DATA_SRC #3=%04X\n", adis161xx_read16(0x01, 0x2C));

			for (;;)
				;

//
//
//
			local_delay_ms(500);
		}

	}
#endif
#if 0 && WITHDEBUG && WITHSMPSYSTEM
	{
		PRINTF("main: gARM_BASEPRI_ALL_ENABLED=%02X, %02X, %02X, bpr=%02X\n", gARM_BASEPRI_ALL_ENABLED, GIC_ENCODE_PRIORITY(PRI_USER), GIC_GetInterfacePriorityMask(), GIC_GetBinaryPoint());
		enum { TGCPUMASK1 = UINT32_C(1) << 1 };
		enum { TGCPUMASK0 = UINT32_C(1) << 0 };
		const int cpu = __get_MPIDR() & 0x03;

		PRINTF("Main thread test: I am CPU=%d\n", cpu);
		local_delay_ms(100);

		arm_hardware_set_handler(SGI13_IRQn, SecondCPUTaskSGI13, BOARD_SGI_PRIO, UINT32_C(1) << 1);
		arm_hardware_set_handler(SGI15_IRQn, SecondCPUTaskSGI15, BOARD_SGI_PRIO, UINT32_C(1) << 1);

		for (;;)
		{
			// 0: to cpu1 or CPU0 (в зависимости от указанной маски в GIC_SendSGI)
			// 1: to cpu1
			// 2: to cpu0
			//PRINTF("fltr = %d\n", i);
			GIC_SendSGI(SGI15_IRQn, TGCPUMASK1, 0x00);	// CPU1, filer=0
			GIC_SendSGI(SGI13_IRQn, TGCPUMASK1, 0x00);	// CPU1, filer=0
			local_delay_ms(300);
		}
//
//		PRINTF("Main thread test: I am CPU=%d. halt\n", cpu);
//		for (;;)
//			;

	}
#endif
#if 0 && CPUSTYLE_XC7Z
	{
		PRINTF("XDCFG->MCTRL.PS_VERSION=%02lX\n", (XDCFG->MCTRL >> 28) & 0x0F);
	}
#endif
#if 0 && WITHETHHW && CPUSTYLE_XC7Z
	{
		PRINTF("GEM0 test:\n");

		SCLR->SLCR_UNLOCK = 0x0000DF0DU;
		SCLR->APER_CLK_CTRL |= (0x01uL << 6);	// APER_CLK_CTRL.GEM0_CPU_1XCLKACT
		SCLR->GEM0_CLK_CTRL = //(SCLR->GEM0_CLK_CTRL & ~ (0x00uL)) |
				((uint_fast32_t) 0x08 << 20) |	// DIVISOR1
				((uint_fast32_t) 0x05 << 8) |	// DIVISOR
				((uint_fast32_t) 0x00 << 4) |	// SRCSEL: 00x: IO PLL
				((uint_fast32_t) 0x01 << 0) |	// CLKACT
				0;
		SCLR->GEM0_RCLK_CTRL = 0x0000001uL;

		ASSERT(GEM0->MODULE_ID == 0x00020118uL);
		PRINTF("GEM0 test done\n");

		//	Net:   ZYNQ GEM: e000b000, phyaddr 7, interface rgmii-id
		//	eth0: ethernet@e000b000
		//	U-BOOT for myd_y7z020_10_07

	}
#endif
#if 0
	{
		PRINTF("CPU speed changing test:\n");
		stm32mp1_pll1_slow(1);
		TP();
		local_delay_ms(500);
		stm32mp1_pll1_slow(0);
		TP();
		local_delay_ms(500);
		stm32mp1_pll1_slow(1);
		TP();
		local_delay_ms(500);
		stm32mp1_pll1_slow(0);
		TP();
		local_delay_ms(500);
		stm32mp1_pll1_slow(1);
		TP();
		local_delay_ms(500);

		for (;0;)
		{
			char c;
			if (dbg_getchar(& c))
			{
				switch (c)
				{
				case '1':
					stm32mp1_pll1_slow(1);
					break;
				case '0':
					stm32mp1_pll1_slow(0);
					break;
				}
				dbg_putchar(c);
				if (c == 0x1b)
					break;
			}
		}
		PRINTF("CPU speed changing test done.\n");
	}
#endif
#if 0
	{
		// gnu11 tests
		enum e1 { WORKMASK1 = -1 << 7 };
		uint32_t v1 = WORKMASK1;
		enum e2 { WORKMASK2 = -1 << 15 };
		uint32_t v2 = WORKMASK2;
		PRINTF("v1=%08lX, v2=%08lX, %u, %u\n", v1, v2, sizeof (enum e1), sizeof (enum e2));
		for (;;)
			;

	}
#endif
#if 0 && WITHDEBUG
	{
		const time_t t = time(NULL);

		PRINTF("sizeof (time_t) == %u, t = %lu\n", sizeof (time_t), (unsigned long) t);
	}
#endif
#if 1 && defined (__GNUC__)
	{

		PRINTF(PSTR("__GNUC__=%d, __GNUC_MINOR__=%d\n"), (int) __GNUC__, (int) __GNUC_MINOR__);
	}
#endif
#if 0 && (__CORTEX_A != 0)
	{

		PRINTF(PSTR("FPEXC=%08lX\n"), (unsigned long) __get_FPEXC());
		__set_FPEXC(__get_FPEXC() | 0x80000000uL);
		PRINTF(PSTR("FPEXC=%08lX\n"), (unsigned long) __get_FPEXC());
	}
#endif
#if 0 && (__L2C_PRESENT == 1)
	{
		// Renesas: PL310 as a secondary cache. The IP version is r3p2.
		// ZYNQ: RTL release R3p2
		// RTL release 0x8 denotes r3p2 code of the cache controller
		// RTL release 0x9 denotes r3p3 code of the cache controller.
		PRINTF("L2C_310->CACHE_ID=%08lX\n", L2C_GetID());	// L2C_GetID()
		//PRINTF("L2C_310->CACHE_ID Implementer=%02lX\n", (L2C_GetID() >> 24) & 0xFF);
		//PRINTF("L2C_310->CACHE_ID CACHE ID=%02lX\n", (L2C_GetID() >> 10) & 0x3F);
		//PRINTF("L2C_310->CACHE_ID Part number=%02lX\n", (L2C_GetID() >> 6) & 0x0F);
		PRINTF("L2C_310->CACHE_ID RTL release=%02lX\n", (L2C_GetID() >> 0) & 0x3F);

		PRINTF("L2C Data RAM latencies: %08lX\n", * (volatile uint32_t *) ((uintptr_t) L2C_310 + 0x010C)); // reg1_data_ram_control
		PRINTF("L2C Tag RAM latencies: %08lX\n", * (volatile uint32_t *) ((uintptr_t) L2C_310 + 0x0108)); // reg1_tag_ram_control
	}
#endif
#if 0 && defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U)
	{
		// GIC version diagnostics
		// Renesas: ARM GICv1
		//	GICInterface->IIDR=3901043B, GICDistributor->IIDR=0000043B
		// STM32MP1: ARM GICv2
		//	GICInterface->IIDR=0102143B, GICDistributor->IIDR=0100143B
		// ZINQ XC7Z010: ARM GICv1
		//	GICInterface->IIDR=3901243B, GICDistributor->IIDR=0102043B

		PRINTF("GICInterface->IIDR=%08lX, GICDistributor->IIDR=%08lX\n", (unsigned long) GIC_GetInterfaceId(), (unsigned long) GIC_DistributorImplementer());

//		switch (ICPIDR1 & 0x0F)
//		{
//		case 0x03:	PRINTF("arm_gic_initialize: ARM GICv1\n"); break;
//		case 0x04:	PRINTF("arm_gic_initialize: ARM GICv2\n"); break;
//		default:	PRINTF("arm_gic_initialize: ARM GICv? (code=%08lX @%p)\n", (unsigned long) ICPIDR1, & ICPIDR1); break;
//		}
	}
#endif /* defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U) */
#if 0 && (__CORTEX_A == 9U) && defined (SCU_CONTROL_BASE)
	{
		// SCU registers dump
		// ZYNQ7000:
		//	SCU Control Register=00000002
		//	SCU Configuration Register=00000501
		//	SCU CPU Power Status Register=03030000
		//	Filtering Start Address Register=00100000
		//	Filtering End Address Register=FFE00000
		//PRINTF("SCU_CONTROL_BASE=%08lX\n", SCU_CONTROL_BASE);
		PRINTF("SCU Control Register=%08lX\n", ((volatile uint32_t *) SCU_CONTROL_BASE) [0]);	// 0x00
		PRINTF("SCU Configuration Register=%08lX\n", ((volatile uint32_t *) SCU_CONTROL_BASE) [1]);	// 0x04
		PRINTF("SCU CPU Power Status Register=%08lX\n", ((volatile uint32_t *) SCU_CONTROL_BASE) [2]);	// 0x08
		PRINTF("Filtering Start Address Register=%08lX\n", ((volatile uint32_t *) SCU_CONTROL_BASE) [0x10]);	// 0x40
		PRINTF("Filtering End Address Register=%08lX\n", ((volatile uint32_t *) SCU_CONTROL_BASE) [0x11]);	// 0x44
	}
#endif
#if 0 && CPUSTYLE_STM32MP1 && WITHDEBUG
	{
		PRINTF("stm32mp1_get_mpuss_freq()=%lu (MPU)\n", stm32mp1_get_mpuss_freq());
		PRINTF("stm32mp1_get_per_freq()=%lu\n", stm32mp1_get_per_freq());
		PRINTF("stm32mp1_get_axiss_freq()=%lu\n", stm32mp1_get_axiss_freq());
		PRINTF("stm32mp1_get_pll2_r_freq()=%lu (DDR3)\n", stm32mp1_get_pll2_r_freq());
	}
#endif
#if 1 && defined (DDRPHYC) && WITHDEBUG && CPUSTYLE_STM32MP1
	{
		// Check DQS Gating System Latency (R0DGSL) and DQS Gating Phase Select (R0DGPS)
		PRINTF("stm32mp1_ddr_init results: DX0DQSTR=%08lX, DX1DQSTR=%08lX, DX2DQSTR=%08lX, DX3DQSTR=%08lX\n",
				DDRPHYC->DX0DQSTR, DDRPHYC->DX1DQSTR,
				DDRPHYC->DX2DQSTR, DDRPHYC->DX3DQSTR);

		// 16 bit single-chip DDR3:
		// PanGu board: stm32mp1_ddr_init results: DX0DQSTR=3DB02001, DX1DQSTR=3DB02001, DX2DQSTR=3DB02000, DX3DQSTR=3DB02000
		// board v2: 	stm32mp1_ddr_init results: DX0DQSTR=3DB03001, DX1DQSTR=3DB03001, DX2DQSTR=3DB02000, DX3DQSTR=3DB02000
		// voard v3: 	stm32mp1_ddr_init results: DX0DQSTR=3DB03001, DX1DQSTR=3DB03001, DX2DQSTR=3DB02000, DX3DQSTR=3DB02000

	}
#endif
#if 0
	#include "dsp3D.h"
	{


		static float32_t dsp3dModel [] = {
				12,3,
				//         VERTEXES
				//   coords    normals
				1,	1, 1, 	1,0,0, // 0
				1, 1,-1, 	0,-1,0, // 1
				1,-1, 1, 	-1,0,0, // 2
				1,-1,-1, 	0,0,-1, // 3
				3,	1, 1, 	1,0,0, // 0
				3, 1,-1, 	0,-1,0, // 1
				3,-1, 1, 	-1,0,0, // 2
				3,-1,-1, 	0,0,-1, // 3
				6,	1, 1, 	1,0,0, // 0
				6, 1,-1, 	0,-1,0, // 1
				6,-1, 1, 	-1,0,0, // 2
				6,-1,-1, 	0,0,-1, // 3
//				3,	1, 1, 	1,0,0, // 0
//				3, 1,-1, 	0,-1,0, // 1
//				3,-1, 1, 	-1,0,0, // 2
//				3,-1,-1, 	0,0,-1, // 3

				//         FACES
				//    Indexes     RGB
				0,1,2,   255,0,255,
				//1,2,3,   255,0,255,
				4,5,6,   0,0,255,
				//5,6,7,   0,0,255,
				8,9,10,   0,255,0,
				//9,10,11,   0,255,0,
		};

//		calcnormaltriangle(dsp3dModel);
//		TP();
//		for (;;)
//			;
		dsp3D_init();
		dsp3D_setCameraPosition(0,0,10);
		dsp3D_setLightPosition(0,0,10);
		dsp3D_setCameraTarget(0,0,0);

		int phase = 0;
		unsigned cnt = 0;
		PRINTF("3d: test started, CPU_FREQ=%lu kHz\n", (unsigned long) (CPU_FREQ / 1000));
		for (;;)
		{
			float * const buf = dsp3dModel + 5;
			float * const buf2 = dsp3dModel + 11;
			float * const buf3 = dsp3dModel + 17;
			float * const buf4 = dsp3dModel + 23;
//			setnormal(buf, phase % 6);
//			setnormal(buf2, (phase / 6) % 6);
//			setnormal(buf3, phase / 36);
			//PRINTF("normal : %d, %d, %d", (int) buf [0], (int) buf [1], (int) buf [2]);
			const time_t start = time(NULL);
			meshRotation[0] = 0;
			meshRotation[1] = 0;
			meshRotation[2] = 0;
			float a;
			for (a = 0; a < 0.1; a += 0.001f)
			{
				meshRotation[0]+=a;
				meshRotation[1]+=a;
				//meshRotation[2]+=a;

				//dsp3D_renderGouraud(dsp3dModel);
				dsp3D_renderFlat(dsp3dModel);
				//dsp3D_renderWireframe(dsp3dModel);
//				char buff [64];
//				snprintf(buff, 64, "normal : %d, %d, %d,", (int) buf [0], (int) buf [1], (int) buf [2]);
//				display_at(20, 10, buff);
//				snprintf(buff, 64, "normal : %d, %d, %d,", (int) buf2 [0], (int) buf2 [1], (int) buf2 [2]);
//				display_at(20, 15, buff);
//				snprintf(buff, 64, "normal : %d, %d, %d,", (int) buf3 [0], (int) buf3 [1], (int) buf3 [2]);
//				display_at(20, 20, buff);
				dsp3D_present();
				local_delay_ms(25);
				char c;
				if (0 && dbg_getchar(& c))
				{
					switch (c)
					{
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
						setnormal(buf, c - '0');
					default:
					case ' ':
						PRINTF("normal : %d, %d, %d,\n", (int) buf [0], (int) buf [1], (int) buf [2]);
						break;
					}
				}

			}
			const time_t end = time(NULL);
			PRINTF("3d: cnt=%u, %d S\n", cnt, (int) (end - start));
			phase = phase + 1;
			if (phase >= (6 * 6 * 6))
				phase = 0;
		}
	}
#endif
#if 0
	{
		unsigned pin;
		PRINTF("zynq pin & bank calculations test.\n");
		for (pin = 0; pin < 118; ++ pin)
		{
			uint_fast8_t Bank;
			uint_fast8_t PinNumber;

			GPIO_BANK_DEFINE(pin, Bank, PinNumber);
			ASSERT(Bank == GPIO_PIN2BANK(pin));
			ASSERT(PinNumber == GPIO_PIN2BITPOS(pin));
		}
		PRINTF("zynq pin & bank calculations test passed.\n");
	}
#endif
#if 1 && WITHOPENVG
	{
		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
		//disableAllIRQs();
	#if 1

		PS* const tiger = PS_construct(tigerCommands, tigerCommandCount, tigerPoints, tigerPointCount);
		ASSERT(tiger != NULL);
		unsigned cnt;
		PRINTF("tiger: test started, CPU_FREQ=%lu kHz\n", (unsigned long) (CPU_FREQ / 1000));
		for (cnt = 0;; ++ cnt)
		{
			const time_t start = time(NULL);
			uint_fast8_t kbch, repeat;

			if ((repeat = kbd_scan(& kbch)) != 0)
			{
				break;
			}

			rendertiger(tiger, DIM_X, DIM_Y);
			rendertest2(DIM_X, DIM_Y);
			display_flush();		// наблюдаем процесс
			const time_t end = time(NULL);
#if WITHTHERMOLEVEL
		PRINTF("tiger: cnt=%u, %d s, t=%f\n", cnt, (int) (end - start), GET_CPU_TEMPERATURE());
#else /* WITHTHERMOLEVEL */
		PRINTF("tiger: cnt=%u, %d s\n", cnt, (int) (end - start));
#endif /* WITHTHERMOLEVEL */
		}
		PS_destruct(tiger);

	#elif 1
		{
			int pos;
			int total = 100;
			display_nextfb();
			for (pos = 0; ; pos = (pos + 1) % total)
			{
				uint_fast8_t kbch, repeat;

				if ((repeat = kbd_scan(& kbch)) != 0)
				{
					break;
				}

				//rendertest1(DIM_X, DIM_Y);
				//display_nextfb();

				rendertestdynamic(DIM_X, DIM_Y, pos, total);
				display_nextfb();
			}
		}

	#else

		// wait for press any key
		for (;;)
		{
			uint_fast8_t kbch, repeat;

			if ((repeat = kbd_scan(& kbch)) != 0)
			{
				break;
			}
			rendertest1(DIM_X, DIM_Y);
			//display_fillrect(0, 0, DIM_X, DIM_Y, COLORMAIN_RED);
			display_nextfb();
			//local_delay_ms(300);

			rendertest2(DIM_X, DIM_Y);
			//display_fillrect(0, 0, DIM_X, DIM_Y, COLORMAIN_GREEN);
			display_nextfb();
			//local_delay_ms(300);

			rendertest3(DIM_X, DIM_Y);
			//display_fillrect(0, 0, DIM_X, DIM_Y, COLORMAIN_BLUE);
			display_nextfb();
			//local_delay_ms(300);
		}

	#endif
	}
#endif
#if 0 && (WITHTWIHW || WITHTWISW)
	{
		unsigned i;
		for (i = 1; i < 127; ++ i)
		{
			uint8_t v;
			unsigned addrw = i * 2;
			unsigned addrr = addrw + 1;
			////%%TP();
			i2c_start(addrw);
			i2c_write_withrestart(0x1B);
			i2c_start(addrr);
			i2c_read(& v, I2C_READ_ACK_NACK);
			////%%TP();
			PRINTF("I2C addr=%d (0x%02X): test=0x%02X\n", i, addrw, v);
		}
	}
#endif
#if 0 && (WITHTWIHW || WITHTWISW)
	{
		// i2c bus test i2c test twi bus test twi test

		unsigned i;
		for (i = 1; i < 127; ++ i)
		{
			uint8_t v;
			unsigned addrw = i * 2;
			unsigned addrr = addrw + 1;
			////%%TP();
			i2c2_start(addrw);
			i2c2_write_withrestart(0x1B);
			i2c2_start(addrr);
			i2c2_read(& v, I2C_READ_ACK_NACK);
			////%%TP();
			PRINTF("I2C2 addr=%d (0x%02X): test=0x%02X\n", i, addrw, v);
		}
	}
#endif
#if 0 && (WITHTWIHW || WITHTWISW)
	{
		// i2c bus test i2c test twi bus test twi test

		unsigned i;
		for (i = 1; i < 127; ++ i)
		{
			uint8_t v;
			unsigned addrw = i * 2;
			unsigned addrr = addrw + 1;
			////%%TP();
			i2c_start(addrw);
			i2c_write_withrestart(0x1B);
			i2c_start(addrr);
			i2c_read(& v, I2C_READ_ACK_NACK);
			////%%TP();
			PRINTF("I2C1 addr=%d (0x%02X): test=0x%02X\n", i, addrw, v);
		}
	}
#endif
#if 0 && CPUSTYLE_STM32MP1
	{
		//	This register is used by the MPU to check the reset source. This register is updated by the
		//	BOOTROM code, after a power-on reset (por_rst), a system reset (nreset), or an exit from
		//	Standby or CStandby.
		PRINTF(PSTR("MP_RSTSCLRR=%08lX\n"), (unsigned long) RCC->MP_RSTSCLRR);
		RCC->MP_RSTSCLRR = RCC->MP_RSTSCLRR;
		PRINTF(PSTR("MP_RSTSCLRR=%08lX\n"), (unsigned long) RCC->MP_RSTSCLRR);
		PRINTF(PSTR("ACTLR=%08lX\n"), (unsigned long) __get_ACTLR());
	}
#endif
#if 0 && CPUSTYLE_STM32MP1
	{

		RCC->MP_APB5ENSETR = RCC_MP_APB5ENSETR_BSECEN;
		(void) RCC->MP_APB5ENSETR;
	//	0x24: STM32MP153Cx
	//	0x25: STM32MP153Ax
	//	0xA4: STM32MP153Fx
	//	0xA5: STM32MP153Dx
	//	0x00: STM32MP157Cx
	//	0x01: STM32MP157Ax
	//	0x80: STM32MP157Fx
	//	0x81: STM32MP157Dx
		const unsigned rpn = ((* (volatile uint32_t *) RPN_BASE) & RPN_ID_Msk) >> RPN_ID_Pos;
		switch (rpn)
		{
		case 0x24: PRINTF(PSTR("STM32MP153Cx\n")); break;
		case 0x25: PRINTF(PSTR("STM32MP153Ax\n")); break;
		case 0xA4: PRINTF(PSTR("STM32MP153Fx\n")); break;
		case 0xA5: PRINTF(PSTR("STM32MP153Dx\n")); break;
		case 0x00: PRINTF(PSTR("STM32MP157Cx\n")); break;
		case 0x01: PRINTF(PSTR("STM32MP157Ax\n")); break;
		case 0x80: PRINTF(PSTR("STM32MP157Fx\n")); break;
		case 0x81: PRINTF(PSTR("STM32MP157Dx\n")); break;
		default: PRINTF(PSTR("STN32MP1 RPN=%02X\n"), rpn); break;
		}
	}
#endif
#if 0 && defined (TZC) && WITHDEBUG
	{

        const uint_fast8_t lastregion = TZC->BUILD_CONFIG & 0x1f;
        uint_fast8_t i;
        PRINTF("TZC=%p\n", TZC);
        for (i = 0; i <= lastregion; ++ i)
        {
            volatile uint32_t * const REG_ATTRIBUTESx = & TZC->REG_ATTRIBUTESO + (i * 8);
            volatile uint32_t * const REG_ID_ACCESSx = & TZC->REG_ID_ACCESSO + (i * 8);
            volatile uint32_t * const REG_BASE_LOWx = & TZC->REG_BASE_LOWO + (i * 8);
            volatile uint32_t * const REG_BASE_HIGHx = & TZC->REG_BASE_HIGHO + (i * 8);
            volatile uint32_t * const REG_TOP_LOWx = & TZC->REG_TOP_LOWO + (i * 8);
            volatile uint32_t * const REG_TOP_HIGHx = & TZC->REG_TOP_HIGHO + (i * 8);

            PRINTF("TZC->REG_BASE_LOW%d=%08lX ", i, * REG_BASE_LOWx);
            PRINTF("REG_BASE_HIGH%d=%08lX ", i, * REG_BASE_HIGHx);
            PRINTF("REG_TOP_LOW%d=%08lX ", i, * REG_TOP_LOWx);
            PRINTF("REG_TOP_HIGH%d=%08lX ", i, * REG_TOP_HIGHx);
            PRINTF("REG_ATTRIBUTES%d=%08lX ", i, * REG_ATTRIBUTESx);
            PRINTF("REG_ID_ACCESS%d=%08lX\n", i, * REG_ID_ACCESSx);
        }
	}
#endif
#if 0 && WITHDEBUG
	{
		// FPU speed test
		uint_fast8_t state = 0;
#if defined (BOARD_BLINK_INITIALIZE)
		BOARD_BLINK_INITIALIZE();
#elif CPUSTYLE_R7S721
		const uint_fast32_t mask = (1uL << 10);	// P7_10: RXD0: RX DATA line
		arm_hardware_pio7_outputs(mask, mask);
#else /* CPUSTYLE_R7S721 */
		const uint_fast32_t mask = (1uL << 13);	// PA13
		arm_hardware_pioa_outputs(mask, 1 * mask);
#endif /* CPUSTYLE_R7S721 */
		PRINTF("cplxmla @%p, src @%p, dst @%p. refv @%p, CPU_FREQ=%lu MHz\n", cplxmla, src, dst, refv, CPU_FREQ / 1000000uL);
		global_disableIRQ();
		for (;;)
		{
			// stm32mp1 @800 MHz, 16 bit DDR3 @533 MHz
			//	__GNUC__=10, __GNUC_MINOR__=2
			//	cplxmla @C001D174, src @C0CD06C0, dst @C0CC06C0. refv @C0B01A00, CPU_FREQ=792 MHz
			// -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon
			// cplxmla & cplxmlasave: 2.43 kHz
			// -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=neon-vfpv4
			// cplxmla & cplxmlasave: 2.38 kHz
			// -mcpu=cortex-a7 -mfloat-abi=hard -mfpu=vfpv4
			// cplxmla & cplxmlasave: 1.87 kHz

			// ZYNQ 7000 @666 MHz, 16 bit DDR3 @533 MHz
			//	__GNUC__=10, __GNUC_MINOR__=2
			//	cplxmla @00112798, src @0080DB40, dst @007FDB40. refv @00362E00, CPU_FREQ=666 MHz
			// -mcpu=cortex-a9  -mfloat-abi=hard  -mfpu=vfpv3
			// cplxmla & cplxmlasave: 1.53 kHz
			// -mcpu=cortex-a9  -mfloat-abi=hard  -mfpu=neon-vfpv3
			// cplxmla & cplxmlasave: 1.4 kHz

			// R7S721 @360 MHz
			//	__GNUC__=10, __GNUC_MINOR__=2
			// cplxmla @2001C184, src @2027D780, dst @2026D780. refv @20186B00, CPU_FREQ=360 MHz
			// -mcpu=cortex-a9  -mfloat-abi=hard  -mfpu=vfpv3
			// cplxmla & cplxmlasave: 0.71 kHz
			// -mcpu=cortex-a9  -mfloat-abi=hard  -mfpu=neon-vfpv3
			// cplxmla & cplxmlasave: 0.71 kHz
			// -mcpu=cortex-a9  -mfloat-abi=hard  -mfpu=neon
			// cplxmla & cplxmlasave: 0.71 kHz

			cplxmla(src, dst, refv,  FFTZS);
			cplxmlasave(dst, FFTZS);

			if (state)
			{
				state = 0;
	#if defined (BOARD_BLINK_SETSTATE)
			BOARD_BLINK_SETSTATE(0);
	#elif CPUSTYLE_R7S721
				R7S721_TARGET_PORT_S(7, mask);
	#else /* CPUSTYLE_R7S721 */
				(GPIOA)->BSRR = BSRR_S(mask);
	#endif /* CPUSTYLE_R7S721 */
			}
			else
			{
				state = 1;
		#if defined (BOARD_BLINK_SETSTATE)
				BOARD_BLINK_SETSTATE(1);
		#elif CPUSTYLE_R7S721
				R7S721_TARGET_PORT_C(7, mask);
		#else /* CPUSTYLE_R7S721 */
				(GPIOA)->BSRR = BSRR_C(mask);
		#endif /* CPUSTYLE_R7S721 */
			}
		}

	}
#endif
#if 0
	{
	#if 0
		unsigned k;
		TP();
		for (k = 0; k < 1024; ++ k)
		{
			memfill(k);
		}
		TP();
		for (k = 0; k < 1024; ++ k)
		{
			PRINTF("\r%4d ", k);
			if (!memprobe(k))
				break;
		}
		PRINTF("\n");
		PRINTF("MCU_AHB_SRAM size = %uK\n", k);
		printhex(MCU_AHB_SRAM, (const uint8_t *) MCU_AHB_SRAM, 256);	// /* Cortex-M4 memories */
	#endif
	#if WITHSDRAMHW
		printhex(DRAM_MEM_BASE, (const uint8_t *) DRAM_MEM_BASE, 256);	// DDR3
	#endif /* WITHSDRAMHW */
		//printhex(QSPI_MEM_BASE, (const uint8_t *) QSPI_MEM_BASE, 256);	// QSPI
		//arm_hardware_sdram_initialize();
	}
#endif
#if 0
	{
		PRINTF(PSTR("PLL_FREQ=%lu Hz (%lu MHz)\n"), (unsigned long) PLL_FREQ, (unsigned long) PLL_FREQ / 1000000);
	}
#endif
#if 0
	{
		#define WORKMASK (MFPGA | MSYS | MDSP | MRES)

		arm_hardware_piog_outputs(WORKMASK, 0);

		static RAMNOINIT_D1 FATFSALIGN_BEGIN float Etalon [2048] FATFSALIGN_END;
		static RAMNOINIT_D1 FATFSALIGN_BEGIN float TM [2048] FATFSALIGN_END;
		static RAMNOINIT_D1 FIL WPFile;			/* Описатель открытого файла - нельзя располагать в Cortex-M4 CCM */
		static const char fmname [] = "tstdata.dat";
		static RAMNOINIT_D1 FATFS wave_Fatfs;		/* File system object  - нельзя располагать в Cortex-M4 CCM */
	
		int CountZap=0;
		int LC=2048;
		for (CountZap = 0; CountZap < 5; ++ CountZap)
		{
			GPIOG->ODR ^= WORKMASK;	// Debug LEDs
			hardware_spi_io_delay();
			local_delay_ms(500);
		}
		arm_hardware_piog_outputs(WORKMASK, 0);

		f_mount(& wave_Fatfs, "", 0);
		for (CountZap = 0; CountZap < 1000; ++ CountZap, sdtick())
		{
			PRINTF(PSTR("CountZap %d\n"), CountZap);
			int i;
			FRESULT rc;
			UINT ByteWrite;
			for (i=0;i<LC;i++)
			{
				Etalon[i]=i;
				TM[i]=0;
			}
			rc=f_open(&WPFile, fmname, FA_CREATE_ALWAYS | FA_WRITE);
			if (rc!=FR_OK)
				sdfault();

			rc=f_write (&WPFile, Etalon,LC*4,&ByteWrite );

			if (rc!=FR_OK)
				sdfault();
			rc=f_close (&WPFile);
			if (rc!=FR_OK)
				sdfault();
			rc=f_open(&WPFile, fmname, FA_OPEN_EXISTING | FA_READ);
			if (rc!=FR_OK)
				sdfault();
			rc=f_read (&WPFile, TM,LC*4,&ByteWrite );
			f_sync (&WPFile);
			if (rc!=FR_OK)
				sdfault();
			rc=f_close (&WPFile);
			if (rc!=FR_OK)
				sdfault();
			for (i=0;i<LC;i++)
			{
				if (TM[i]!=Etalon[i])
					sdfault();

			}
		}
		f_mount(NULL, "", 0);

		for (;;)
		{
			GPIOG->ODR ^= WORKMASK;	// Debug LEDs
			hardware_spi_io_delay();
			local_delay_ms(500);
		}
	}
#endif
#if 0 && __MPU_PRESENT
	{
		// Cortex Memory Protection Unit (MPU)
		PRINTF(PSTR("MPU=%p\n"), MPU);
		PRINTF(PSTR("MPU->TYPE=%08lX, MPU->CTRL=%08lX\n"), MPU->TYPE, MPU->CTRL);
		const uint_fast8_t n = (MPU->TYPE & MPU_TYPE_DREGION_Msk) >>MPU_TYPE_DREGION_Pos;
		uint_fast8_t i;
		for (i = 0; i < n; ++ i)
		{
			MPU->RNR = i;
			PRINTF(PSTR("MPU->RNR=%08lX, MPU->RBAR=%08lX, MPU->RASR=%08lX "), MPU->RNR, MPU->RBAR, MPU->RASR);
			const uint_fast32_t rasr = MPU->RASR;
			PRINTF(PSTR("XN=%u,AP=%u,TEX=%u,S=%u,C=%u,B=%u,SRD=%u,SIZE=%u,ENABLE=%u\n"),
				((rasr & MPU_RASR_XN_Msk) >> MPU_RASR_XN_Pos),   	// DisableExec
				((rasr & MPU_RASR_AP_Msk) >> MPU_RASR_AP_Pos),   	// AccessPermission
				((rasr & MPU_RASR_TEX_Msk) >> MPU_RASR_TEX_Pos),  	// TypeExtField
				((rasr & MPU_RASR_S_Msk) >> MPU_RASR_S_Pos),    	// IsShareable
				((rasr & MPU_RASR_C_Msk) >> MPU_RASR_C_Pos),    	// IsCacheable
				((rasr & MPU_RASR_B_Msk) >> MPU_RASR_B_Pos),    	// IsBufferable
				((rasr & MPU_RASR_SRD_Msk) >> MPU_RASR_SRD_Pos),  	// SubRegionDisable
				((rasr & MPU_RASR_SIZE_Msk) >> MPU_RASR_SIZE_Pos), 	// Size 512 kB
				((rasr & MPU_RASR_ENABLE_Msk) >> MPU_RASR_ENABLE_Pos)// Enable
				);
		}
	}
#endif
#if 0
	{
		// Сигналы управления HD44780
		for (;;)
		{
			// Установить
			//LCD_STROBE_PORT_S(LCD_STROBE_BIT);
			LCD_RS_PORT_S(ADDRES_BIT);
			//LCD_WE_PORT_S(WRITEE_BIT);
			local_delay_ms(20);

			// Сбросить
			//LCD_STROBE_PORT_C(LCD_STROBE_BIT);
			LCD_RS_PORT_C(ADDRES_BIT);
			//LCD_WE_PORT_C(WRITEE_BIT);
			local_delay_ms(20);
		}
	}
#endif
#if 0
	{
		// вычисления с плавающей точкой
		//
		//				   1.4142135623730950488016887242096981L
		// #define M_SQRT2  1.41421356237309504880

		//original: sqrt(2)=1.41421356237309514547462
		//double:   sqrt(2)=1.41421356237309514547462
		//float:    sqrt(2)=1.41421353816986083984375
		char b [64];
		
		//snprintf(b, sizeof b / sizeof b [0], "%u\n", (unsigned) SCB_GetFPUType());
		//PRINTF(PSTR("SCB_GetFPUType: %s"), b);

		snprintf(b, sizeof b / sizeof b [0], "sqrt(2)=%1.23f\n", (double) 1.41421356237309504880);
		PRINTF(PSTR("original: %s"), b);

		volatile double d0 = 2;
		volatile double d = sqrt(d0);
		snprintf(b, sizeof b / sizeof b [0], "sqrt(2)=%1.23f\n", d);
		PRINTF(PSTR("double:   %s"), b);

		volatile float f0 = 2;
		volatile float f = sqrtf(f0);
		snprintf(b, sizeof b / sizeof b [0], "sqrt(2)=%1.23f\n", f);
		PRINTF(PSTR("float:    %s"), b);
	}
#endif
#if 0 && CTLSTYLE_V1V
	{
		// "прерыватель"

		#define RELAY_PORT PORTD	// выходы процессора - управление трактом ппередачи и манипуляцией
		#define RELAY_DDR DDRD		// переключение на вывод - управление трактом передачи и манипуляцией

		// Управление передатчиком - сигналы TXPATH_ENABLE (PA11) и TXPATH_ENABLE_CW (PA10) - активны при нуле на выходе.
		#define RELAY_BIT		(1uL << PD5)

		RELAY_DDR |= RELAY_BIT;
		unsigned offtime = 100;
		unsigned ontime = 100;
		showstate(offtime, ontime);
		unsigned offt = offtime;
		unsigned ont = ontime;
		unsigned offphase = 0;
		for (;;)
		{
			uint_fast8_t kbch, repeat;

			if ((repeat = kbd_scan(& kbch)) != 0)
			{
				switch (kbch)
				{
				case KBD_CODE_8:
					ontime = modif (ontime, 100, 1, 100);
					break;
				case KBD_CODE_9:
					ontime = modif (ontime, 100, 99, 100);
					break;

				case KBD_CODE_4:
					ontime = modif (ontime, 10, 1, 10);
					break;
				case KBD_CODE_5:
					ontime = modif (ontime, 10, 99, 10);
					break;

				case KBD_CODE_0:
					ontime = modif (ontime, 1, 1, 10);
					break;
				case KBD_CODE_1:
					ontime = modif (ontime, 1, 99, 10);
					break;
				//
				case KBD_CODE_10:
					offtime = modif (offtime, 100, 1, 100);
					break;
				case KBD_CODE_11:
					offtime = modif (offtime, 100, 99, 100);
					break;

				case KBD_CODE_6:
					offtime = modif (offtime, 10, 1, 10);
					break;
				case KBD_CODE_7:
					offtime = modif (offtime, 10, 99, 10);
					break;

				case KBD_CODE_2:
					offtime = modif (offtime, 1, 1, 10);
					break;
				case KBD_CODE_3:
					offtime = modif (offtime, 1, 99, 10);
					break;


				}
				// Update times with new parameters
				offt = offtime;
				ont = ontime;
				showstate(offtime, ontime);
			}
			{
				if (offphase)
				{
					RELAY_PORT &= ~ RELAY_BIT;
					local_delay_ms(10);
					if (-- offt == 0)
					{
						offphase = 0;
						ont = ontime;
						if (ont == 0)
						{
							offphase = 1;
							offt = offtime;
						}
					}
				}
				else
				{
					RELAY_PORT |= RELAY_BIT;
					local_delay_ms(10);
					if (-- ont == 0)
					{
						offphase = 1;
						offt = offtime;
						if (offt == 0)
						{
							offphase = 0;
							ont = ontime;
						}
					}
				}
			}
		}


	}
#endif /* 1 && CTLSTYLE_V1V */
#if 0
	{
		colmain_setcolors(COLOR_GREEN, COLOR_BLACK);
		display_at_P(5, 0, PSTR("PT-Electronics 2015"));

		colmain_setcolors(COLOR_RED, COLOR_BLACK);
		display_at_P(7, 3, PSTR("RENESAS"));

		colmain_setcolors(COLOR_WHITE, COLOR_BLACK);
		display_at_P(9, 6, PSTR("2.7 inch TFT"));
		
		for (;;)
			;
	}
#endif
#if 0
	{
		// Движущиеся картинки
		enum 
		{ 
			topreserved = 6,
			bufY = DIM_Y - GRID2Y(ROWS2GRID(topreserved)), 
			dx = DIM_X, dy = bufY, 
			// куда выводить
			DBX_0 = CHARS2GRID(0), 
			DBY_1 = ROWS2GRID(topreserved)
		};
		PRINTF(PSTR("test: dx=%d, dy=%d\n"), dx, dy);


		/* отображение надписей самым маленьким шрифтом (8 точек) */
		colmain_setcolors(COLOR_GREEN, COLOR_BLACK);
		uint_fast8_t lowhalf2 = HALFCOUNT_SMALL2 - 1;
		do
		{
		
			display_gotoxy(CHARS2GRID(0), ROWS2GRID(0) + lowhalf2);
			display_string2_P(PSTR("PT-Electronics 2017"), lowhalf2);
			//display_string2_P(PSTR("PT-Electronics 2015 RENESAS 2.7 inch TFT"), lowhalf2);

			//display_gotoxy(CHARS2GRID(0), CHARS2GRID(1) + lowhalf2);
			//display_string2_P(PSTR("PT-Electronics 2014"), lowhalf2);

		} while (lowhalf2 --);
	#if 1
		/* отображение надписей маленьким шрифтом (16 точек) */
		colmain_setcolors(COLOR_GREEN, COLOR_BLACK);
		
		display_at_P(CHARS2GRID(0), ROWS2GRID(1), PSTR("Start "));

		display_at_P(CHARS2GRID(0), ROWS2GRID(2), PSTR("Stop "));

	#endif

	#if 0//LCDMODE_COLORED

		static ALIGNX_BEGIN volatile PACKEDCOLOR565_T scr [GXSIZE(dx, dy)] ALIGNX_END;

		colpip_fill(scr, dx, dy, COLOR_WHITE);
		colpip_copy_to_draw(scr, dx, dy, 0, GRID2Y(topreserved));
		//for (;;)
		//	;
		int phase = 0;
		int count = 0;
		const int top = (DIM_X - bufY);
		unsigned loop;
		for (loop = 0; ;loop = loop < top ? loop + 1 : 0)
		{
		
			// рисование линии
			unsigned i;
			for (i = 0; i < bufY; ++ i)
				colpip_point(scr, dx, dy, i + loop, i, COLOR_BLUE);		// поставить точку

			colpip_copy_to_draw(scr, dx, dy, 0, GRID2Y(topreserved));
			//local_delay_ms(25);
			if (++ count > top)
			{
				count = 0;
				phase = ! phase;
				if (phase)
					colpip_fill(scr, dx, dy, COLOR_YELLOW);
				else
					colpip_fill(scr, dx, dy, COLOR_RED);
			}
			
		}
	#else /* LCDMODE_COLORED */

		static FATFSALIGN_BEGIN GX_t scr [MGSIZE(dx, dy)] FATFSALIGN_END;

		colmain_setcolors(COLOR_WHITE, COLOR_BLACK);
		
		memset(scr, 0xFF, sizeof scr);
		display_showbuffer(scr, dx, dy, DBX_0, DBY_1);
		int phase = 0;
		int count = 0;
		const int top = (DIM_X - bufY);
		unsigned loop;
		for (loop = 0; ;loop = loop < top ? loop + 1 : 0)
		{
		
			// рисование линии
			unsigned i;
			for (i = 0; i < bufY; ++ i)
				display_pixelbuffer(scr, dx, dy, i + loop, i);		// погасить точку

			display_showbuffer(scr, dx, dy, DBX_0, DBY_1);
			//local_delay_ms(25);
			if (++ count > top)
			{
				count = 0;
				phase = ! phase;
				if (phase)
					colmain_setcolors(COLOR_YELLOW, COLOR_BLACK);
				else
					colmain_setcolors(COLOR_RED, COLOR_BLACK);
				memset(scr, 0xFF, sizeof scr);
			}
			
		}
	#endif /* LCDMODE_COLORED */
	}
#endif
#if 0 && WITHDEBUG
	{
		// тестирование приёма и передачи символов
		PRINTF(PSTR("Serial port ECHO test.\n"));
		for (;;)
		{
			char c;
			if (dbg_getchar(& c))
			{
				dbg_putchar(c);
				if (c == 0x1b)
					break;
			}
		}
}
#endif
#if 0
	// Тест для проверки корректности работы последовательного порта с прерываниями.
	// Должно работать просто "эхо" вводимых символов.
	{
		serial_irq_loopback_test();
	}
#endif
#if 0
	{
		PRINTF(PSTR("FPU tests start.\n"));
		local_delay_ms(300);
		//volatile int a = 10, b = 0;
		//volatile int c = a / b;
		unsigned long i;
		for (i = 0x8000000;; ++ i)
		{
			const double a = i ? i : 1;
			//const int ai = (int) (sin(a) * 1000);
			PRINTF(PSTR("Hello! %lu, sqrt(%lu)=%lu\n"), i, (unsigned) a, (unsigned)( sqrt(a)*10));
		}
	}
#endif
#if 0 && defined (RTC1_TYPE)
	{
		board_rtc_settime(10, 8, 0);
		board_rtc_setdate(2015, 9, 14);
	}
#endif
#if 0 && WITHDEBUG && WITHSDHCHW
	{
		// SD card control lines test
		HARDWARE_SDIOPOWER_INITIALIZE();
		HARDWARE_SDIOPOWER_SET(1);

		arm_hardware_piod_outputs((1uL << 2), 1 * (1uL << 2));	/* PD2 - SDIO_CMD	*/
		arm_hardware_pioc_outputs((1uL << 12), 1 * (1uL << 12));	/* PC12 - SDIO_CK	*/
		arm_hardware_pioc_outputs((1uL << 8), 1 * (1uL << 8));	/* PC8 - SDIO_D0	*/
		arm_hardware_pioc_outputs((1uL << 9), 1 * (1uL << 9));	/* PC9 - SDIO_D1	*/
		arm_hardware_pioc_outputs((1uL << 10), 1 * (1uL << 10));	/* PC10 - SDIO_D2	*/
		arm_hardware_pioc_outputs((1uL << 11), 1 * (1uL << 11));	/* PC11 - SDIO_D3	*/

		int i;
		for (i = 0;; ++ i)
		{
			int f0 = (i & (1 << 0)) != 0;
			int f1 = (i & (1 << 1)) != 0;
			int f2 = (i & (1 << 2)) != 0;
			int f3 = (i & (1 << 3)) != 0;
			int f4 = (i & (1 << 4)) != 0;
			int f5 = (i & (1 << 5)) != 0;

			arm_hardware_piod_outputs((1uL << 2), f0 * (1uL << 2));	/* PD2 - SDIO_CMD	*/
			arm_hardware_pioc_outputs((1uL << 12), f1 * (1uL << 12));	/* PC12 - SDIO_CK	*/
			arm_hardware_pioc_outputs((1uL << 8), f2 * (1uL << 8));	/* PC8 - SDIO_D0	*/
			arm_hardware_pioc_outputs((1uL << 9), f3 * (1uL << 9));	/* PC9 - SDIO_D1	*/
			arm_hardware_pioc_outputs((1uL << 10), f4 * (1uL << 10));	/* PC10 - SDIO_D2	*/
			arm_hardware_pioc_outputs((1uL << 11), f5 * (1uL << 11));	/* PC11 - SDIO_D3	*/

		}
	}
#endif
#if 0 && WITHDEBUG
	{
		PRINTF(PSTR("SD sensors test\n"));
		// SD card sensors test
		HARDWARE_SDIOSENSE_INITIALIZE();
		for (;;)
		{
			PRINTF(PSTR("SD sensors: CD=%d, WP=%d\n"), HARDWARE_SDIOSENSE_CD(), HARDWARE_SDIOSENSE_WP());
			local_delay_ms(50);
		}
	}
#endif
#if 0 && WITHDEBUG && WITHUSEFATFS
	// SD CARD low level functions test
	{
		diskio_test();
	}
#endif
#if 0 && WITHDEBUG && WITHUSEFATFS
	// SD CARD FatFs functions test
	{
		fatfs_filesystest(0);
	}
#endif
#if 0 && WITHDEBUG && WITHUSEFATFS
	// SD CARD file system level functions speed test
	// no interactive
	{

		FRESULT rc;
//
//		static const MKFS_PARM defopt = { FM_ANY, 0, 0, 0, 0};	/* Default parameter */
//		defopt.fmt = FM_ANY;	/* Format option (FM_FAT, FM_FAT32, FM_EXFAT and FM_SFD) */
//		defopt.n_fat = 2;		/* Number of FATs */
//		defopt.align = 0;		/* Data area alignment (sector) */
//		defopt.n_root = 128;	/* Number of root directory entries */
//		defopt.au_size = 0;		/* Cluster size (byte) */

		PRINTF("Wait for storage device ready. Press space key\n");
		for (;;)
		{
			char c;
			if (dbg_getchar(& c))
			{
				if (c == 0x1B)
				{
					PRINTF("Skip storage device test\n");
					return;
				}
				if (c == ' ')
					break;
			}
	#if WITHUSBHW
			board_usbh_polling();     // usb device polling
	#endif /* WITHUSBHW */
	#if WITHUSEAUDIOREC
			sdcardbgprocess();
	#endif /* WITHUSEAUDIOREC */
			//local_delay_ms(5);
		}
		PRINTF("Storage device ready\n");
		unsigned t;
//		for (t = 0; t < 7000; t += 5)
//		{
//	#if WITHUSBHW
//			board_usbh_polling();     // usb device polling
//	#endif /* WITHUSBHW */
//	#if WITHUSEAUDIOREC
//			sdcardbgprocess();
//	#endif /* WITHUSEAUDIOREC */
//			//local_delay_ms(5);
//		}
		static ticker_t test_recordticker;
		IRQL_t oldIrql;
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		ticker_initialize(& test_recordticker, 1, test_recodspool, NULL);	// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
		ticker_add(& test_recordticker);
		LowerIrql(oldIrql);
		{
 			f_mount(NULL, "", 0);		/* Unregister volume work area (never fails) */
			rc = f_mkfs("0:", NULL, rbwruff, sizeof (rbwruff));
			if (rc != FR_OK)
			{
				PRINTF(PSTR("sdcardformat: f_mkfs failure, rc=0x%02X\n"), (int) rc);
				return;
			}
			else
			{
				PRINTF(PSTR("sdcardformat: f_mkfs okay\n"));
			}

		}
		for (;;)
		{
			PRINTF(PSTR("Storage device test - %d bytes block.\n"), sizeof rbwruff);
			PRINTF("Storage device test\n");
			if (fatfs_filesyspeedstest())
				break;
			for (t = 0; t < 7000; t += 5)
			{
		#if WITHUSBHW
				board_usbh_polling();     // usb device polling
		#endif /* WITHUSBHW */
		#if WITHUSEAUDIOREC
				sdcardbgprocess();
		#endif /* WITHUSEAUDIOREC */
				//local_delay_ms(5);
			}
		}
		PRINTF("Storage device test done\n");

	}
#endif
#if 1 && (WITHNANDHW || WITHNANDSW)
	// NAND memory test
	// PrimeCell Static Memory Controller (PL353) ARM r2p1
	{
		nand_tests();
	}
#endif
#if 0 && WITHDEBUG && WITHUSEAUDIOREC
	// SD CARD file system level functions test
	{
		static ticker_t test_recordticker;
		IRQL_t oldIrql;
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		ticker_initialize(& test_recordticker, 1, test_recodspool, NULL);	// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
		ticker_add(& test_recordticker);
		LowerIrql(oldIrql);
		fatfs_filesystest(1);
	}
#endif
#if 0 && WITHDEBUG && WITHUSEFATFS
	// Автономный программатор SPI flash memory
	{
		//diskio_test();
		////mmcCardSize();
		////mmcCardSize();
		fatfs_progspi();
	}
#endif
#if 0
	{
		// VFP tests, double
		unsigned long i;
		for (i = 0x8000000;; ++ i)
		{
			const double a = i ? i : 1;
			//const int ai = (int) (sin(a) * 1000);

			PRINTF(PSTR("Hello! %lu, sqrt(%lu)=%lu\n"), i, (unsigned) a, (unsigned) sqrt(a));

		}
	}
#endif
#if 0 && CPUSTYLE_R7S721
	{
		// RZ board leds test
		//i2c_initialize();

		arm_hardware_pio7_outputs(LEDBIT, LEDBIT);	/* ---- P7_1 : LED0 direct connection to IP */
		arm_hardware_pio1_inputs(SW1BIT);	/*  */
		ledsinit();

		for (;;)
		{
			leds(0x01);
			local_delay_ms(100);
			leds(0x02);
			local_delay_ms(100);
			leds(0x04);
			local_delay_ms(100);
	#if 1
			if (GPIO.PPR1 & SW1BIT)
			{
				GPIO.P7 |= LEDBIT;
				leds(1);
			}
			else
			{
				GPIO.P7 &= ~ LEDBIT;
				leds(0);
			}
			continue;
	#endif
			//R_LED_On();
			//GPIO.PNOT7 = LEDBIT;
			LED_TARGET_PORT_S(LEDBIT);	// Led OFF
			__DSB();
			local_delay_ms(1000);
			//R_LED_Off();
			//GPIO.PNOT7 = LEDBIT;
			LED_TARGET_PORT_C(LEDBIT);	// Led ON
			__DSB();
			local_delay_ms(5000);
		}
	}
#endif
#if 0 && ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED
	// note: rdx0154 should be enabled (for I2C functions include).
	{
		test_cpu_pwron(1);
		eink_lcd_backlight(1);
		// M9546 (PCF8576C) support functions
		LCD1x9_Initialize();

		if (0)
		{
			// получение номеров сегментов LCD
			int segment = 0;
			LCD1x9_seg(segment, 1);
			for (;;)
			{

				uint_fast8_t kbch, repeat;

				if ((repeat = kbd_scan(& kbch)) != 0)
				{
					switch (kbch)
					{
					case KBD_CODE_0:
						LCD1x9_seg(segment, 0);
						if (++ segment >= MAXSEGMENT)
							segment = 0;
						LCD1x9_seg(segment, 1);
						PRINTF(PSTR("seg = %d\n"), segment);
						break;

					case KBD_CODE_1:
						LCD1x9_seg(segment, 0);
						if (segment == 0)
							segment = MAXSEGMENT - 1;
						else
							-- segment;
						LCD1x9_seg(segment, 1);
						PRINTF(PSTR("seg = %d\n"), segment);
						break;
					}
				}
			}
		}
		for (;;)
		{

			lcd_outarray(pe2014, sizeof pe2014 / sizeof pe2014 [0]);
			int t;
			for (t = 0; t < 100; ++ t)
			{
				local_delay_ms(50);
				check_poweroff();
			}
			LCD1x9_clear();
			{
				// Зажигаем все сегменты
				uint_fast8_t comIndex;
				for (comIndex = 0; comIndex < 4; ++ comIndex)
				{
					uint_fast8_t bitIndex;
					for (bitIndex = 0; bitIndex < 40; ++ bitIndex)
					{
						check_poweroff();
						LCD1x9_enableSegment(comIndex, bitIndex);
						LCD1x9_Update();
						local_delay_ms(50);
					}
				}
			}
			{
				// Гасим все сегменты
				uint_fast8_t comIndex;
				for (comIndex = 0; comIndex < 4; ++ comIndex)
				{
					uint_fast8_t bitIndex;
					for (bitIndex = 0; bitIndex < 40; ++ bitIndex)
					{
						check_poweroff();
						LCD1x9_disableSegment(comIndex, bitIndex);
						LCD1x9_Update();
						local_delay_ms(50);
					}
				}
			}
		}

		for (;;)
			;
	}
#endif /* ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED */
#if 0 && ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED
	{
		test_cpu_pwron(1);
		// EM027BS013 tests
		eink_initialize();
		for (;;)
			;
	}
#endif /* ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED */
#if 0 && ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED
	{
		// проверка кнопок включения-выключени я питания
		test_cpu_pwron(1);
		for (;;)
		{
			local_delay_ms_spool(1000);
		}
	}
#endif /* ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED */
#if 1 && ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED
	{
		test_cpu_pwron(1);
		//EPD_power_init(EPD_200);

		EPD_display_init();
		for(;;) {
			PRINTF(PSTR("eink: loop start\n"));
			/* User selects which EPD size to run demonstration by changing the
			 * USE_EPD_Type in image_data.h
			 * The Image data arrays for each EPD size are defined at image_data.c */
	#if (USE_EPD_Type==USE_EPD144)
			EPD_display_from_pointer(EPD_144,image_array_144_2,image_array_144_1);
	#elif (USE_EPD_Type==USE_EPD200)
			EPD_display_from_pointer(EPD_200,image_array_200_2,image_array_200_1);
	#elif (USE_EPD_Type==USE_EPD270)
			EPD_display_from_pointer(EPD_270,image_array_270_2,image_array_270_1);
	#endif

			/* The interval of two images alternatively change is 10 seconds */
			//local_delay_ms(10000);
			local_delay_ms_spool(1000);

	#if (USE_EPD_Type==USE_EPD144)
			EPD_display_from_pointer(EPD_144,image_array_144_1,image_array_144_2);
	#elif (USE_EPD_Type==USE_EPD200)
			EPD_display_from_pointer(EPD_200,image_array_200_1,image_array_200_2);
	#elif (USE_EPD_Type==USE_EPD270)
			EPD_display_from_pointer(EPD_270,image_array_270_1,image_array_270_2);
	#endif

			/* The interval of two images alternatively change is 10 seconds */
			//local_delay_ms(10000);
			local_delay_ms_spool(2000);
			PRINTF(PSTR("eink: loop end\n"));
		}
	}
#endif /* ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED */
#if 0
	{
		//hardware_tim21_initialize();
		for (;;)
		{
			PRINTF(PSTR("TIM21 CNT=%08lX\n"), TIM21->CNT);
		}
	}
#endif
#if 0
	{
		// test: initialize TIM2:TIM5
		// TIM5 включён на выход TIM2
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM5EN;   // подаем тактирование на TIM2 & TIM5
		__DSB();
		//TIM3->DIER = TIM_DIER_UIE;        	 // разрешить событие от таймера
		TIM5->PSC = 1;
		TIM2->PSC = 1;

		TIM5->CR1 = TIM_CR1_CEN; /* включить таймер */
		TIM2->CR1 = TIM_CR1_CEN; /* включить таймер */
		for (;;)
		{
			PRINTF(PSTR("TIM2:TIM5 CNT=%08lX:%08lX\n"), TIM2->CNT, TIM5->CNT);
		}
	}
#endif
#if 0 && LCDMODE_COLORED && ! DSTYLE_G_DUMMY
	{

		board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
		board_update();
		TP();
		unsigned cnt;
		display2_bgreset();
		//disableAllIRQs();
		for (cnt = 0; ; ++ cnt)
		{
			const time_t tstart = time(NULL);
			//GridTest();
			BarTest();
			const time_t tend = time(NULL);
			PRINTF("BarTest: %u, %ds, pixelsize=%d @%u MHz\n", cnt, (int) (tend - tstart), LCDMODE_PIXELSIZE, (unsigned) (CPU_FREQ / 1000000));
		}
		// Divide result by 10
		// 800x480, Renesas RZ/A1L, @360 MHz, L8, software (w cache: 5.6s..5.7s)
		// 800x480, Renesas RZ/A1L, @360 MHz, L8, software (no cache: 0.2s)
		// 800x480, STM32MP157, @650 MHz, L8, software (w cache: 0.8s)
		// 800x480, STM32MP157, @650 MHz, L8, hardware MDMA: (no cache - 0.9s..1s)
		// 800x480, STM32MP157, @650 MHz, RGB565, hardware MDMA: (no cache - 1.4s)
		// 800x480, STM32MP157, @650 MHz, ARGB8888, hardware MDMA: (no cache - 2.5s)
		// 800x480, Allwinner t113-s3, @1200 MHz, RGB565, software 0.6s
		// 800x480, Allwinner F133-A, @1200 MHz, RGB565, hardware G2D 0.7s
		// 800x480, Allwinner F133-A, @1200 MHz, XRGB8888, hardware G2D 0.9s
	}
#endif
#if 0
	#include "display/pict.h"
	uint_fast16_t xcoi, ycoi;
	uint_fast32_t linesStart = 0;
	uint_fast32_t pixelIdx = 0;

	PACKEDCOLORPIP_T * frame = colmain_fb_draw();

	memset(frame, 0x00, GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORPIP_T));

	for(ycoi = 0; ycoi < DIM_Y; ycoi++)
	{
		for(xcoi = 0; xcoi < (DIM_X * 4); xcoi+=4)
		{
			frame[linesStart + xcoi    ] = gImage_pict[pixelIdx++];
			frame[linesStart + xcoi + 1] = gImage_pict[pixelIdx++];
			frame[linesStart + xcoi + 2] = gImage_pict[pixelIdx++];
		}
		linesStart += (DIM_X * 4);
	}
	display_flush();
	for (;;)
		;
#endif
#if 0 && WITHLTDCHW && LCDMODE_COLORED && ! DSTYLE_G_DUMMY
	{
		// test: вывод палитры на экран
		display2_bgreset();
		PACKEDCOLORPIP_T * const fr = colmain_fb_draw();
		int sepx = 3, sepy = 3;
		int wx = DIM_X / 16;
		int wy = DIM_Y / 16;
		int x = 0, y = 0;

		for (int i = 0; i <= 255; i++)
		{

			display_fillrect(x, y, wx - sepx, wy - sepy, i << 4);

			if (wx > 24)
			{
				char buf [4];
				local_snprintf_P(buf, sizeof buf / sizeof buf [0], PSTR("%d"), i);
				colpip_string3_tbg(fr, DIM_X, DIM_Y, x, y, buf, COLORMAIN_WHITE);
			}

			x = x + wx;
			if ((i + 1) % 16 == 0)
			{
				x = 0;
				y = y + wy;
			}
		}

		display_flush();
		for (;;)
			;
	}
#endif
#if 0
	// разметка для 9-точечной калибровки тачскрина
	display2_bgreset();
	colmain_setcolors(COLORMAIN_WHITE, COLORMAIN_BLACK);
	PACKEDCOLORPIP_T * const fr = colmain_fb_draw();

	uint8_t c = 35;

	colpip_fillrect(fr, DIM_X, DIM_Y, 0 + c, 0 + c, 3, 3, COLORMAIN_WHITE);			// 1
	colpip_fillrect(fr, DIM_X, DIM_Y, DIM_X / 2, 0 + c, 3, 3, COLORMAIN_WHITE);		// 2
	colpip_fillrect(fr, DIM_X, DIM_Y, DIM_X - c, 0 + c, 3, 3, COLORMAIN_WHITE);		// 3

	colpip_fillrect(fr, DIM_X, DIM_Y, 0 + c, DIM_Y / 2, 3, 3, COLORMAIN_WHITE);		// 4
	colpip_fillrect(fr, DIM_X, DIM_Y, DIM_X - c, DIM_Y / 2, 3, 3, COLORMAIN_WHITE);	// 5

	colpip_fillrect(fr, DIM_X, DIM_Y, 0 + c, DIM_Y - c, 3, 3, COLORMAIN_WHITE);		// 6
	colpip_fillrect(fr, DIM_X, DIM_Y, DIM_X / 2, DIM_Y - c, 3, 3, COLORMAIN_WHITE);	// 7
	colpip_fillrect(fr, DIM_X, DIM_Y, DIM_X - c, DIM_Y - c, 3, 3, COLORMAIN_WHITE);	// 8

	colpip_fillrect(fr, DIM_X, DIM_Y, DIM_X / 2, DIM_Y / 2, 3, 3, COLORMAIN_WHITE);	// 9

	dcache_clean((uintptr_t) fr, (uint_fast32_t) GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORPIP_T));
	hardware_ltdc_main_set((uintptr_t) fr);

	for(;;) {}
#endif
#if 0 && defined (TSC1_TYPE)
#include "touch\touch.h"

	{
		uint_fast16_t gridx = 16;
		uint_fast16_t gridy = 16;
		uint_fast16_t markerx = 0;
		uint_fast16_t markery = 0;

		display2_bgreset();
		colmain_setcolors(COLORMAIN_WHITE, COLORMAIN_BLACK);

		// touch screen test
		PRINTF(PSTR("touch screen test:\n"));
		for (;;)
		{
			PACKEDCOLORPIP_T * const fr = colmain_fb_draw();
			char msg [64];
			uint_fast16_t x, y;
			if (board_tsc_getxy(& x, & y))
			{
				PRINTF(PSTR("board_tsc_getxy: x=%5d, y=%5d\n"), (int) x, (int) y);
				local_snprintf_P(msg, ARRAY_SIZE(msg), PSTR("x=%5d, y=%5d"), (int) x, (int) y);
				colpip_fillrect(fr, DIM_X, DIM_Y, markerx, markery, gridx, gridy, COLORMAIN_BLACK);
				markerx = x / gridx * gridx;
				markery = y / gridy * gridy;
				colpip_fillrect(fr, DIM_X, DIM_Y, markerx, markery, gridx, gridy, COLORMAIN_WHITE);
			} else {
				memset(msg, ' ', 63);
				msg [63] = '\0';
				colpip_fillrect(fr, DIM_X, DIM_Y, markerx, markery, gridx, gridy, COLORMAIN_BLACK);
			}
			display_at(22, 26, msg);
			local_delay_ms(10);

			dcache_clean((uintptr_t) fr, (uint_fast32_t) GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORPIP_T));
			hardware_ltdc_main_set((uintptr_t) fr);
		}
	}
#endif
#if 0 && (CTLSTYLE_V1E || CTLSTYLE_V1F)
	{
		//int n = TIM6_DAC_IRQn;
		unsigned long i = 0;
		colmain_setcolors(COLOR_WHITE, COLOR_BLACK);
		for (;;)
		{
			++ i;
			uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
			do
			{
				char buff [22];

				// IF gain
				local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
					PSTR("CNT=%08lX"), i
					 );

				display_gotoxy(0, 0 + lowhalf);
				colmain_setcolors(COLOR_WHITE, COLOR_BLACK);
				display_at(buff, lowhalf);
			} while (lowhalf --);
			PRINTF(PSTR("CNT=%08lX\n"), i);
		}
	}
	{
		// PB8 signal
		enum { WORKMASK = 1ul << 0 };
		arm_hardware_pioa_outputs(WORKMASK, WORKMASK);

		for (;;)
		{
			GPIOA->ODR ^= WORKMASK;	// Debug LEDs
			//hardware_spi_io_delay();
		}
	}
#endif
#if 0 && WITHNMEA
	// NMEA test
	{
		for (;;)
		{
			char buff [32];
			local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("%02d:%02d:%02d,ky=%d"), 
				th.hours, th.minutes, th.seconds, 
				HARDWARE_NMEA_GET_KEYDOWN());

			uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
			do
			{
				display_gotoxy(0, 0 + lowhalf);

				display_at(buff, lowhalf);
			} while (lowhalf --);
		}
	}
#endif
#if 0 && WITHDEBUG
	// NVRAM test
	{
		PRINTF(PSTR("NVRAM test started...\n"));
		nvram_initialize();
		unsigned char i = 0;
		for (;; ++ i)
		{
			save_i8(10, 0x55);
			save_i8(20, 0xaa);
			save_i8(30, i);
			const uint_fast8_t v1 = restore_i8(10);
			const uint_fast8_t v2 = restore_i8(20);
			const uint_fast8_t v3 = restore_i8(30);
			PRINTF(PSTR("v1=%02x, v2=%02x, v3=%02x (expected=%02x)\n"), v1, v2, v3, i);
			local_delay_ms(50);
		}
		for (;;)
			;
	}
#endif

#if 0 && defined (RTC1_TYPE) && WITHDEBUG

	/* RTC test */
	{
		////board_rtc_initialize(); // already done
		//board_rtc_settime(22, 48, 30);
		//board_rtc_setdate(2015, 5, 3);
		
		for (;;)
		{
			uint_fast16_t year;
			uint_fast8_t month, day;
			uint_fast8_t hour, minute, seconds;

			//board_rtc_getdate(& year, & month, & day);
			//board_rtc_gettime(& hour, & minute, & seconds);

			//PRINTF(PSTR("%04d-%02d-%02d "), year, month, day);
			//PRINTF(PSTR("%02d:%02d:%02d "), hour, minute, seconds);

			board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & seconds);

			PRINTF(PSTR("%04d-%02d-%02d "), year, month, day);
			PRINTF(PSTR("%02d:%02d:%02d\n"), hour, minute, seconds);

			local_delay_ms(1250);
			
		}
	}
#endif
#if 0 && WITHDEBUG
	/* ADC test */
	{

		for (;;)
		{
			uint_fast8_t i;
			for (i = 0; i < 8; ++ i)
			{
				//const uint_fast8_t i = AVOXIX; //KI2;
				PRINTF(PSTR("ADC%d=%3d "), i, board_getadc_unfiltered_u8(i, 0, 255));
				//PRINTF(PSTR("ADC%d=%3d "), i, board_getadc_unfiltered_1 /* true */value(i));
			}
			PRINTF(PSTR("\n"));
		}
	}
#endif
#if 0 && WITHDEBUG
	/* ADC test on screen*/
	{
		// see HARDWARE_ADCINPUTS 
		enum { NADC = 8, COLWIDTH = 8 };
		unsigned long cnt = 0;
		for (;; ++ cnt)
		{
			uint_fast8_t i;
			for (i = 0; i < NADC; ++ i)
			{
				char buff [32];

				local_snprintf_P(buff, sizeof buff / sizeof buff [0],
					PSTR("V%d=%4d"), i, board_getadc_unfiltered_1 /* true */value(i));
				display_at(COLWIDTH * (i % 2), i / 2, buff);
			}
			if (0)
			{
				char buff [32];
				local_snprintf_P(buff, sizeof buff / sizeof buff [0],
					PSTR("CNT=%08lX"), cnt); 
				display_at(8 * (i % 2), i / 2, buff);
			}
		}
	}
#endif
#if 0
	// отображение картинок
	eink_lcd_backlight(1);

	/* буфер размером x=64, y=112 точек */
	enum { bufY = DIM_Y - 8, dx = DIM_X, dy = /*24 */ bufY, DBX_0 = 0, DBY_1 = 1};
	static FATFSALIGN_BEGIN PACKEDCOLORPIP_T scr [GXSIZE(dx, dy)] FATFSALIGN_END;


	/* отображение надписей самым маленьким шрифтом (8 точек) */
	display_at_P(CHARS2GRID(0), ROWS2GRID(0), PSTR("PT-Electronics 2014"));

#if 0
	/* отображение надписей маленьким шрифтом (16 точек) */
	display_at_P(CHARS2GRID(0), ROWS2GRID(2), PSTR("Start "));

	display_at_P(CHARS2GRID(0), ROWS2GRID(3), PSTR("Stop "));
#endif

	colmain_setcolors(COLOR_WHITE, COLOR_BLACK);
	unsigned loop;
	for (loop = 0; ;loop = loop < (DIM_X - bufY) ? loop + 1 : 0)
	{
	
		check_poweroff();
		memset(scr, 0xFF, sizeof scr);
		// рисование линии
		unsigned i;
		for (i = 0; i < bufY; ++ i)
			display_pixelbuffer(scr [0], dx, dy, i + loop, i);		// погасить точку

		display_showbuffer(scr [0], dx, dy, DBX_0, DBY_1);
		local_delay_ms(25);
		
	}
#endif
#if 0 && WITHDEBUG
	// вечный цикл
	for (;;)
	{
		i2c_start(0xaa);// Si570: адрес 0x55, запись = 0
		i2c_write(135);
		i2c_write(0x80);	// RST_REG = 1 - выключает генерацию на выходе Si570
		i2c_waitsend();
		i2c_stop();
			
		_delay_ms(20);
	}
#endif
#if 0 && WITHTX && WITHVOX && WITHDEBUG
	// Отображение значений с выхода DSP модуля - уровень VOX
	{
		updateboard(1, 1);	/* полная перенастройка (как после смены режима) - режим приема */
		updateboard2();			/* настройки валкодера и цветовой схемы дисплея. */
		for (;;)
		{
			//unsigned dsp_getmikev(void);
			//unsigned vox1 = dsp_getmikev();
			unsigned vox2 = board_getvox();
			unsigned avox = board_getavox();
			uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
			do
			{
				char buff [22];

				//////////////////////////
				// VOX level
				local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
					PSTR("vox2=%3d"), vox2
					 );
				display_gotoxy(0, 0 + lowhalf);
				display_at(buff, lowhalf);

				//////////////////////////
				// VOX2 level
				/*
				local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
					PSTR("vox1=%5d"), vox1
					 );
				display_gotoxy(0, 1 + lowhalf);
				display_at(buff, lowhalf);
				*/
				//////////////////////////
				// A-VOX level
				local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
					PSTR("avox=%3d"), avox
					 );
				display_gotoxy(11, 0 + lowhalf);
				display_at(buff, lowhalf);

			} while (lowhalf --);
		}
	}
#endif
#if 0 && WITHDEBUG
	TP();
	// Трансивер с DSPIF4 "Вороненок-DSP"
	// Отображение значений с дополнительных входов АЦП
	for (;;)
	{
//		if (! display_refreshenabled_wpm())
//			continue;
		// подтверждаем, что обновление выполнено
//		display_refreshperformed_wpm();

		//const unsigned potrf = board_getadc_filtered_u8(POTIFGAIN, 0, UINT8_MAX);
		const unsigned potrft = board_getadc_unfiltered_1 /* true */value(POTIFGAIN);
		const unsigned potrf = board_getadc_unfiltered_1 /* true */value(POTIFGAIN);

		//const unsigned potaf = board_getadc_smoothed_u8(POTAFGAIN, BOARD_AFGAIN_MIN, BOARD_AFGAIN_MAX);
		const unsigned potaft = board_getadc_unfiltered_1 /* true */value(POTAFGAIN);
		const unsigned potaf = board_getadc_unfiltered_1 /* true */value(POTAFGAIN);

		//const unsigned aux1 = board_getadc_filtered_u8(POTAUX1, 0, UINT8_MAX);
		//const unsigned aux2 = board_getadc_filtered_u8(POTAUX2, 0, UINT8_MAX);
		//const unsigned aux3 = board_getadc_filtered_u8(POTAUX3, 0, UINT8_MAX);
#if WITHPOTWPM
		const unsigned wpm = board_getpot_filtered_u8(POTWPM, 0, UINT8_MAX);
#endif /* WITHPOTWPM */

		PRINTF("potrft=%u potaft=%u\n", potrf, potaft);
		continue;
		char buff [22];

#if 1
		// сокращённый вариант отображения
		// AF gain
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("af= %4d"), potaf
			 );
		display_at(0, 0 * HALFCOUNT_SMALL, buff);
		// AF gain raw
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("aft=%4d"), potaft
			 );
		display_at(0, 1 * HALFCOUNT_SMALL, buff);
		continue;
#else
		// IF gain
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("rf= %4d"), potrf
			 );
		display_at(0, 0 * HALFCOUNT_SMALL, buff);
		// AF gain
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("af= %4d"), potaf
			 );
		display_at(0, 1 * HALFCOUNT_SMALL, buff);

		// AUX1
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("A1= %4d"), aux1
			 );
		display_gotoxy(14, 0 + lowhalf);
		display_at(buff, lowhalf);

		// AUX2
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("A2= %4d"), aux2
			 );
		display_gotoxy(0, 1 + lowhalf);
		display_at(buff, lowhalf);

		// AUX3
		/*
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("A3=%3d"), aux3
			 );
		display_gotoxy(7, 1 + lowhalf);
		display_at(buff, lowhalf);
		*/
#if WITHPOTWPM
		// WPM
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("cw=%3d"), wpm
			 );
		display_gotoxy(14, 1 + lowhalf);
		display_at(buff, lowhalf);
#endif /* WITHPOTWPM */

		// IF gain raw
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("rft=%4d"), potrft
			 );
		display_gotoxy(0, 2 + lowhalf);
		display_at(buff, lowhalf);
		// AF gain raw
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("aft=%4d"), potaft
			 );
		display_gotoxy(10, 2 + lowhalf);
		display_at(buff, lowhalf);
#endif


	}
#endif
#if 0 && WITHDEBUG
	// тест дисплея - вывод меняющихся цифр
	{
		unsigned long i = 0;
		colmain_setcolors(COLOR_WHITE, COLOR_BLACK);
		for (;;)
		{
			++ i;
			uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
			do
			{
				char buff [22];

				// IF gain
				local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
					PSTR("CNT=%08lX"), i
					 );

				display_gotoxy(0, 0 + lowhalf);
				colmain_setcolors(COLOR_WHITE, COLOR_BLACK);
				display_at(buff, lowhalf);
			} while (lowhalf --);
		}
	}
#endif
#if 0 && LCDMODE_MAIN_RGB565
	board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
	board_update();
	// тест интерфейса дисплея - цветов RGB565
	for (;;)
	{
		char b [32];
		int c;
		// COLORPIP_T bg
//		for (c = 0; c < 256; ++ c)
//		{
//			display_setbgcolor(TFTRGB(c, c, c));
//			display2_bgreset();
//			local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("WHITE %-3d"), c);
//			colmain_setcolors(COLOR_WHITE, COLOR_BLACK);
//			display_at(0, 0, b);
//			display_nextfb();
//			local_delay_ms(50);
//		}
		//for (; c -- > 0; )
		//{
		//	display_setbgcolor(TFTRGB(c, c, c));
		//	display2_bgreset();
		//}
		for (c = 0; c < 5; ++ c)
		{
			display_setbgcolor(TFTRGB(UINT32_C(1) << (c + 3), 0, 0));
			display2_bgreset();
			local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("RED %-3d"), UINT32_C(1) << (c + 3));
			colmain_setcolors(COLOR_WHITE, COLOR_BLACK);
			display_at(0, 0, b);
			display_nextfb();
			local_delay_ms(2000);
		}
		//for (; c -- > 0; )
		//{
		//	display_setbgcolor(TFTRGB(c, 0, 0));
		//	display2_bgreset();
		//}
		for (c = 0; c < 6; ++ c)
		{
			display_setbgcolor(TFTRGB(0, UINT32_C(1) << (c + 2), 0));
			display2_bgreset();
			local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("GREEN %-3d"), UINT32_C(1) << (c + 2));
			colmain_setcolors(COLOR_WHITE, COLOR_BLACK);
			display_at(0, 0, b);
			display_nextfb();
			local_delay_ms(2000);
		}
		//for (; c -- > 0; )
		//{
		//	display_setbgcolor(TFTRGB(0, c, 0));
		//	display2_bgreset();
		//}
		for (c = 0; c < 5; ++ c)
		{
			display_setbgcolor(TFTRGB(0, 0, UINT32_C(1) << (c + 3)));
			display2_bgreset();
			local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("BLUE %-3d"), UINT32_C(1) << (c + 3));
			colmain_setcolors(COLOR_WHITE, COLOR_BLACK);
			display_at(0, 0, b);
			display_nextfb();
			local_delay_ms(2000);
		}
		//for (; c -- > 0; )
		//{
		//	display_setbgcolor(TFTRGB(0, 0, c));
		//	display2_bgreset();
		//}
	}
#endif
#if 0
	board_set_bglight(0, WITHLCDBACKLIGHTMAX);	// включить подсветку
	board_update();
	// тест дисплея - проход по всем возможным уровням основных цветов
	for (;;)
	{
		char b [32];
		int c;
		// COLORPIP_T bg
		for (c = 0; c < 256; ++ c)
		{
			display_setbgcolor(TFTRGB(c, c, c));
			display2_bgreset();
			local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("WHITE %-3d"), c);
			colmain_setcolors(COLOR_WHITE, COLOR_BLACK);
			display_at(0, 0, b);
			display_nextfb();
			local_delay_ms(50);
		}
		//for (; c -- > 0; )
		//{
		//	display_setbgcolor(TFTRGB(c, c, c));
		//	display2_bgreset();
		//}
		for (c = 0; c < 256; ++ c)
		{
			display_setbgcolor(TFTRGB(c, 0, 0));
			display2_bgreset();
			local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("RED %-3d"), c);
			colmain_setcolors(COLOR_WHITE, COLOR_BLACK);
			display_at(0, 0, b);
			display_nextfb();
			local_delay_ms(50);
		}
		//for (; c -- > 0; )
		//{
		//	display_setbgcolor(TFTRGB(c, 0, 0));
		//	display2_bgreset();
		//}
		for (c = 0; c < 256; ++ c)
		{
			display_setbgcolor(TFTRGB(0, c, 0));
			display2_bgreset();
			local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("GREEN %-3d"), c);
			colmain_setcolors(COLOR_WHITE, COLOR_BLACK);
			display_at(0, 0, b);
			display_nextfb();
			local_delay_ms(50);
		}
		//for (; c -- > 0; )
		//{
		//	display_setbgcolor(TFTRGB(0, c, 0));
		//	display2_bgreset();
		//}
		for (c = 0; c < 256; ++ c)
		{
			display_setbgcolor(TFTRGB(0, 0, c));
			display2_bgreset();
			local_snprintf_P(b, sizeof b / sizeof b [0], PSTR("BLUE %-3d"), c);
			colmain_setcolors(COLOR_WHITE, COLOR_BLACK);
			display_at(0, 0, b);
			display_nextfb();
			local_delay_ms(50);
		}
		//for (; c -- > 0; )
		//{
		//	display_setbgcolor(TFTRGB(0, 0, c));
		//	display2_bgreset();
		//}
	}
#endif
#if 0 && WITHDEBUG
	{
		//synth_lo1_setfreq(99000000, getlo1div(tx));
		//for (;;)
		//{
		//}


		unsigned long if1 = 73050000ul;
		unsigned long freq;
		for (freq = 0; freq < (56000000 + 50000); freq += 1)
		//for (freq = 28000000; freq < (29700000); freq += 1000)
		{
			synth_lo1_setfreq(freq + if1, getlo1div(tx));
			///*
			if ((freq % 1000) == 0)
			{
				static int cd;
				char buff [22];
				//unsigned char v1, v2;
				display_gotoxy(0, 4);
				local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("%10lu"), freq );
				display_at(buff, 0);
				ITM_SendChar('a' + (cd ++) % 16);
				//SWO_PrintChar('a' + (cd ++) % 16);

			}
			//*/


		}
		for (;;)
		{
		}

	}

#endif
#if 0
	{
		static char buff [22];
		unsigned char v1, v2;
		//v1 = si570_get_status();
		display_gotoxy(0, 6);
		local_snprintf_P(buff, 22, PSTR("he %08lX:%08lX"), 0x1726354aul, -7ul);
		display_at(buff, 0);

		display_gotoxy(0, 7);
		//unsigned long l1 = (unsigned long) (rftw >> 32);
		//unsigned long l2 = (unsigned long) rftw;
		local_snprintf_P(buff, 22, PSTR("he %08lx"), -4000000l);
		display_at(buff, 0);
		for (;;)
			;
	}
#endif
#if 0
	{
		// Проверка, откуда идут помехи - от I2C или от SPI.
		for (;;)
		{

			uint_fast8_t kbch, repeat;

			if ((repeat = kbd_scan(& kbch)) != 0)
			{
				int i;
				switch (kbch)
				{
				case KBD_CODE_BAND_UP:
					// проверка индикатора
					for (i = 0; i < 20000; ++ i)
					{
						char s [21];
						local_snprintf_P(s, sizeof s / sizeof s [0], PSTR("%08lx    "), (unsigned long) i);
						display_menu_label(s);
					}
					break;
				case KBD_CODE_BAND_DOWN:
					// проверка SPI
					for (i = 0; i < 10000; ++ i)
					{
						synth_lo1_setfreq(i, getlo1div(tx));
					}
					break;
				}
			}
		}
	}
#endif
#if 0
	// Тестирование скорости передачи по SPI. На SCK должна быть частота SPISPEED
	for (;;)
	{
		const spitarget_t cs = SPI_CSEL4;
		spi_select(cs, SPIC_MODE3);
		spi_progval8_p1(cs, 0xff);
		//for (;;)
			spi_progval8_p2(cs, 0x55);
			spi_progval8_p2(cs, 0x55);
			spi_progval8_p2(cs, 0x55);
			spi_progval8_p2(cs, 0x55);
			spi_progval8_p2(cs, 0x55);
			spi_progval8_p2(cs, 0x55);
			spi_progval8_p2(cs, 0x55);
			spi_progval8_p2(cs, 0x55);
			spi_progval8_p2(cs, 0x55);
		spi_complete(cs);
		spi_unselect(cs);
	}
#endif
#if 0
	{
		unsigned phase = 0;
		// тестирование входов манипуляции, ptt, ключа и CAT
		for (;;)
		{
			uint_fast8_t kbch, repeat;

			if ((repeat = kbd_scan(& kbch)) != 0)
			{
				//display_dispfreq(kbch * 1000UL + v);
				//v = (v + 10) % 1000;
			}
			unsigned tune1 = hardware_get_tune();
			unsigned ptt1 = hardware_get_ptt();
			unsigned ptt2 = HARDWARE_CAT_GET_RTS();
			IRQL_t oldIrql;
			RiseIrql(IRQL_SYSTEM, & oldIrql);
			unsigned elkey = hardware_elkey_getpaddle(0);
			unsigned ckey = HARDWARE_CAT_GET_DTR();
			LowerIrql(oldIrql);


			PRINTF(PSTR("tune=%u, ptt=%u, elkey=%u\n"), tune1, ptt1, elkey);
			continue;

			display_gotoxy(0, 0);		// курсор в начало первой строки


			display_at(ptt1 != 0 ? "ptt " : "    ", 0);
			display_at(ptt2 != 0 ? "cptt " : "     ", 0);
			display_at(ckey != 0 ? "ckey " : "     ", 0);

			display_gotoxy(0, 2);		// курсор в начало второй строки
			display_at((elkey & ELKEY_PADDLE_DIT) != 0 ? "dit " : "     ", 0);
			display_gotoxy(0, 4);		// курсор в начало второй строки
			display_at((elkey & ELKEY_PADDLE_DASH) != 0 ? "dash" : "      ", 0);
			display_gotoxy(0, 6);		// курсор в начало второй строки
			display_at((phase = ! phase) ? " test1" : " test2", 0);

		}
	}
#endif
#if 0
	{
		// тестирование валкодера в режиме "интеллектуального ускорения"
		// На индикаторе отображается скорость в оборотах в секунду
		for (;;)
		{
			unsigned speed;
			int nrotate = encoder_get_snapshot(& speed, 1);
			uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
			do
			{
				display_gotoxy(0, 0 + lowhalf);
				display2_menu_value(speed * 100UL / ENCODER_NORMALIZED_RESOLUTION, 7, 2, 0, lowhalf);
				display_gotoxy(0, HALFCOUNT_SMALL + lowhalf);
				display2_menu_value(nrotate, 5 | WSIGNFLAG, 255, 0, lowhalf);
				local_delay_ms(100);
			} while (lowhalf --);
			//(void) nrotate;
			//display_debug_digit(speed * 100UL / ENCODER_NORMALIZED_RESOLUTION, 7, 2, 0);
			//enum { lowhalf = 0 };
			//display_gotoxy(0, 1 + lowhalf);		// курсор в начало второй строки
		}
	}
#endif
#if 0
	{
		// тестирование валкодера в режиме "интеллектуального ускорения"
		// На индикаторе отображается скорость в оборотах в секунду
		for (;;)
		{
			uint_fast8_t jumpsize;
			int_least16_t nrotate = getRotateHiRes(& jumpsize, 1);
			(void) nrotate;
			//display_gotoxy(0, 1);		// курсор в начало второй строки
			display_debug_digit(jumpsize, 7, 0, 0);
		}
	}
#endif
#if 0
	// Показ в одной строке трех значений с АЦП клавиатуры
	for (;;)
	{
		uint_fast8_t row;
		uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;

		do
		{
			display_gotoxy(0, 0 + lowhalf);
			display2_menu_value(board_getadc_unfiltered_u8(KI0, 0, UINT8_MAX), 5, 255, 0, lowhalf);
			display_gotoxy(5, 0 + lowhalf);
			display2_menu_value(board_getadc_unfiltered_u8(KI1, 0, UINT8_MAX), 5, 255, 0, lowhalf);
			display_gotoxy(10, 0 + lowhalf);
			display2_menu_value(board_getadc_unfiltered_u8(KI2, 0, UINT8_MAX), 5, 255, 0, lowhalf);
		} while (lowhalf --);
		//
		local_delay_ms(20);
	}
#endif

#if 0
	for (;;)
	{
		uint_fast8_t row;
		for (row = 0; row < 8; ++ row)
		{
			//
			const uint_fast8_t v0 = board_getadc_unfiltered_u8(row, 0, UINT8_MAX); 
			uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;

			do
			{
				display_gotoxy(0, row * HALFCOUNT_SMALL + lowhalf);		// курсор в начало второй строки
				display_at("ADCx=", lowhalf);
				display_gotoxy(5, row * HALFCOUNT_SMALL + lowhalf);		// курсор в начало второй строки
				display2_menu_value(v0, 5, 255, 0, lowhalf);
			} while (lowhalf --);
		}
		//
		local_delay_ms(20);
	}
#endif

#if 0
	{
		PRINTF(PSTR("kbd_test2:\n"));
		// kbd_test2
		// Показ условных номеров клавиш для создания новых матриц перекодировки
		// и тестирования работоспособности клавиатуры.
		enum { menuset = 0 };
		int v = 0;
		for (;;)
		{
			//PRINTF(PSTR("keyport = %02x\n"), (unsigned) KBD_TARGET_PIN);
			//continue;
			
			uint_fast8_t scancode;
			IRQL_t oldIrql;
			RiseIrql(IRQL_SYSTEM, & oldIrql);
			scancode = board_get_pressed_key();
			LowerIrql(oldIrql);

			if (scancode != KEYBOARD_NOKEY)
			{
				PRINTF(PSTR("keycode = %02x, %d\n"), (unsigned) scancode, v);
				v = (v + 1) % 1000;
			}
			continue;

			if (scancode != KEYBOARD_NOKEY)
				display2_dispfreq_a2(scancode * 1000UL + v, 255, 0, menuset);
			else
			{
				display2_dispfreq_a2(v * 1000UL, 255, 0, menuset);
				v = (v + 1) % 1000;
			}
		}
	}
#endif

#if 0
	{
		// kbd_test1
		// показ кодов клавиш
		enum { menuset = 0 };
		int v = 0;
		PRINTF(PSTR("kbd_test1:\n"));
		for (;;)
		{
			uint_fast8_t kbch, repeat;

			if ((repeat = kbd_scan(& kbch)) != 0)
			{
				PRINTF(PSTR("kbch = %02x\n"), (unsigned) kbch);
				continue;
				display2_dispfreq_a2(kbch * 1000UL + v, 255, 0, menuset);
				v = (v + 10) % 1000;
			}
		}
	}
#endif

#if 0
	// test SPI lines (ATT functions)
	{
		int i = 0;
		for (;;)
		{
			board_set_att(i);
			board_update();		/* вывести забуферированные изменения в регистры */
			i = (i + 1) % ATTMODE_COUNT;
			local_delay_ms(500);
		}
	}
#endif

#if 0
	// test SPI lines (band selection hardware)
	{
		int i = 0;
		for (;;)
		{
			board_set_bandf(i);
			board_update();		/* вывести забуферированные изменения в регистры */
			i = (i + 1) % 16;
			local_delay_ms(500);
		}
	}
#endif

#if 0
	// test relays (preapm)
	{
		for (;;)
		{
			int pre;
			for (pre = 0; pre < PAMPMODE_COUNT; ++ pre)
			{
				board_set_tx(0);	
				board_set_bandf(1);	
				board_set_preamp(pre);
				board_update();		/* вывести забуферированные изменения в регистры */
				local_delay_ms(500);
			}
		}
	}
#endif

#if 0
	// test relays (attenuators, preapm, antenna switch)
	{
		for (;;)
		{
			int att;
			for (att = 0; att < ATTMODE_COUNT; ++ att)
			{
				int pre;
				for (pre = 0; pre < PAMPMODE_COUNT; ++ pre)
				{
					int ant;
					for (ant = 0; ant < ANTMODE_COUNT; ++ ant)
					{
						int bandf;
						for (bandf = 0; bandf < 8; ++ bandf)
						{
							board_set_att(att);
							board_set_antenna(ant);
							board_set_bandf(bandf);	
							board_set_bandf2(bandf);
							board_set_bandf3(bandf);
							board_set_preamp(pre);
							board_update();		/* вывести забуферированные изменения в регистры */
							local_delay_ms(100);
						}
					}
				}
			}
		}
	}
#endif

#if 0
	// тестирование сигналов выбора VFO
	{
		for (;;)
		{
			uint_fast8_t i;
			for (i = 0; i < HYBRID_NVFOS; ++ i)
			{
				board_set_lo1vco(i);
				board_update();		/* вывести забуферированные изменения в регистры */
				local_delay_ms(500);
			}
		}
	}
#endif

#if 0
	enum { ADDR = 0x88 };
	// Test I/O exoanders at STM3210C-EVAL
	// Chip name: STMPE811
	// chip addresses: 0x82 & 0x88
	i2c_start(ADDR | 0x00);
	i2c_write(0x04);	// register to write: 0x17 - GPIO select gpio mode bits
	i2c_write(0x0b);	// value to write: 0x17 - GPIO I/O
	i2c_waitsend();
	i2c_stop();

	i2c_start(ADDR | 0x00);
	i2c_write(0x17);	// register to write: 0x17 - GPIO select gpio mode bits
	i2c_write(0xff);	// value to write: 0x17 - GPIO I/O
	i2c_waitsend();
	i2c_stop();

	#if 0
		// чтение всех регшистров
		for (;;)
		{
			int i;
			for (i = 0; i < 0x63; ++ i)
			{
				uint_fast8_t v1 = 0xaa;

				i2c_start(ADDR | 0x00);
				i2c_write_withrestart(i);	// register to read: 0x12 - GPIO monitor pin state register
				i2c_start(ADDR | 0x01);
				i2c_read(& v1, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */
				
				static const FLASHMEM char fmt_1 [] = "%02X";
				char buff [17];
				local_snprintf_P(buff, 17, fmt_1, v1);

				display_gotoxy((i % 8) * 3, (i / 8) * 2);
				display_at(buff, 0);
			}
		}
	#endif

	for (;;)
	{

		enum { ADDR = 0x88 };
		uint_fast8_t v1 = 0xaa;

		i2c_start(ADDR | 0x00);
		i2c_write_withrestart(0x12);	// register to read: 0x12 - GPIO monitor pin state register
		i2c_start(ADDR | 0x01);
		i2c_read(& v1, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */


		static const FLASHMEM char fmt_1 [] = "v=%02X";
		char buff [17];
		local_snprintf_P(buff, 17, fmt_1, v1);
		display_gotoxy(0, 0);
		display_at(buff, 0);
		local_delay_ms(100);
	}

#endif

#if 0
	// PCF8535 (tic154) address (r/w): 
	//	0x79/0x78, 0x7b/0x7a, 0x7d/0x7c, 0x7f/0x7e
	// PCF8574 (bus expander) address (r/w): 
	//	0x41/0x40, 0x43/0x42, 0x45/0x45, 0x47/0x46, 
	//	0x49/0x48, 0x4b/0x4a, 0x4d/0x4c, 0x4f/0x4e, 
	//
	// Test I2C interface
	i2c_start(0x4e);
	i2c_write(0xff);	// все биты на ввод
	i2c_waitsend();
	i2c_stop();

	for (;;)
	{
		i2c_start(0x4f);
		unsigned char v = i2c_readAck();
		//unsigned char v = i2c_readNak();
		//i2c_waitsend();
		i2c_stop();
		static const FLASHMEM char fmt_1 [] = "v=%02X";
		char buff [17];
		local_snprintf_P(buff, 17, fmt_1, v);
		display_gotoxy(0, 0);
		display_at(buff);
		local_delay_ms(100);
	}
#endif

#if 0
	// test SPI lines (output attenuator bits)
	display_dispfreq(v * 1000UL);
	for (;;)
	{
		int v = 0;
		uint_fast8_t kbch, repeat;

		if ((repeat = kbd_scan(& kbch)) != 0)
		{
			v = (v + 1) % 4;
			display_dispfreq(v * 1000UL);
			board_set_att(v);
			board_update();		/* вывести забуферированные изменения в регистры */
#if 1
			double x = v;
			double v = sin(x);	// проверка работы библиотечных функций с плавающей точкой
			uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;

			do
			{
				display_gotoxy(0, 1 + lowhalf);		// курсор в начало первой строки
				display2_menu_value(v * 1000ul, 7, 2, 1, lowhalf);
			} while (lowhalf --);
#endif
		}
	}
#endif

#if 0
	// test spi target on targetext1
	for (;;)
	{
		const spitarget_t target = targetext1;

		prog_select(target);	/* start sending data to target chip */

		prog_val(target, 0x77, 8);

		spi_to_read(target);
		unsigned char v = prog_read_byte();
		spi_to_write(target);

		prog_unselect(target);	/* done sending data to target chip */


	}
#endif

#if 0
	{
		unsigned long v = 0;
		//i2c_initialize();
		//display_initialize();
		for (;;)
		{	
			static const FLASHMEM char fmt_1 [] = "%08lX";
			char buff [17];

			local_snprintf_P(buff, 17, fmt_1, v ++);
			display_gotoxy(0, 0);
			display_at(buff, 0);
			
		}
	}
#endif

#if 0
	{
		uint_fast8_t pos = 0;
		board_cat_set_speed(9600);
		while (dbg_putchar(pos | 0xf0) == 0)
			;
		showpos(pos);
		for (;;)
		{
			uint_fast8_t kbch, repeat;

			local_delay_ms(500);
			pos = calc_next(pos, 0, 15);
			while (dbg_putchar(pos | 0xf0) == 0)
				continue;			
			;
			showpos(pos);
			continue;

			if ((repeat = kbd_scan(& kbch)) != 0)
			{
				switch (kbch)
				{
				case KBD_CODE_BAND_UP:
					pos = calc_next(pos, 0, 15);
					while (dbg_putchar(pos | 0xf0) == 0)
						;
					break;

				case KBD_CODE_BAND_DOWN:
					pos = calc_prev(pos, 0, 15);
					while (dbg_putchar(pos | 0xf0) == 0)
						;
					break;

				default:
					continue;
				}
				showpos(pos);
			}
			else
			{
				//while (dbg_putchar(pos | 0xf0) == 0)
				//	;
			}

		}
	}
#endif
#if 0 && WITHDSPEXTFIR
	{
		// Тестирование сигналов управления загрузкой параметров фильтров FPGA

		unsigned seq;

		TARGET_FPGA_FIR_INITIALIZE();

		for (seq = 0;; ++ seq)
		{
			if (seq & 0x01)
				TARGET_FPGA_FIR_CS_PORT_S(TARGET_FPGA_FIR_CS_BIT);
			else
				TARGET_FPGA_FIR_CS_PORT_C(TARGET_FPGA_FIR_CS_BIT);

			if (seq & 0x02)
				TARGET_FPGA_FIR1_WE_PORT_S(TARGET_FPGA_FIR1_WE_BIT);
			else
				TARGET_FPGA_FIR1_WE_PORT_C(TARGET_FPGA_FIR1_WE_BIT);

			if (seq & 0x04)
				TARGET_FPGA_FIR2_WE_PORT_S(TARGET_FPGA_FIR2_WE_BIT);
			else
				TARGET_FPGA_FIR2_WE_PORT_C(TARGET_FPGA_FIR2_WE_BIT);

			local_delay_ms(250);
		}
	}
#endif
#if 0
	TP();
	for (;;)
	{
		SPI_CS_ASSERT(targetnvram);
		local_delay_ms(700);
		SPI_CS_DEASSERT(targetnvram);
		local_delay_ms(700);
		TP();
	}
#endif

#if WITHLTDCHW && LCDMODE_LTDC
	display_nextfb();	// Скрыть результаты теста, разнести рисуемый и ообрадаемый буферы
#endif /* WITHLTDCHW && LCDMODE_LTDC */
}

#if WITHRTOS

#include "FreeRTOS.h"
#include "task.h"

#define STACK_SIZE 8129
static TaskHandle_t task_blinky_handle;
static TaskHandle_t task_blinky_handle2;
static TaskHandle_t task_blinky_handle3;
static TaskHandle_t task_gr_handle;

//static QueueHandle_t kbd_queue;

static void task_blinky(void *arg)
{
	(void)arg;
	uint32_t state = 1;

	printf("blinky1\n");

	while (1)
	{
		PRINTF("!");
		vTaskDelay(NTICKS(750));
	}
}

static void task_blinky2(void *arg)
{
	int state = 0;
	(void)arg;

#ifdef BOARD_BLINK_INITIALIZE
	BOARD_BLINK_INITIALIZE();
#endif /* BOARD_BLINK_INITIALIZE */
	while (1)
	{
		//PRINTF("@");
#ifdef BOARD_BLINK_INITIALIZE
		BOARD_BLINK_SETSTATE(state);
#endif /* BOARD_BLINK_INITIALIZE */
		state = ! state;
		vTaskDelay(NTICKS(500));
	}
}

static void task_blinky3(void *arg)
{
	int state = 0;
	(void)arg;

	while (1)
	{
		PRINTF("@");
		vTaskDelay(NTICKS(1500));
	}
}

//void vApplicationFPUSafeIRQHandler( uint32_t ulICCIAR )
void vApplicationIRQHandler( uint32_t ulICCIAR )
{
	const IRQn_ID_t int_id = ulICCIAR & 0x3FFUL;

	// IHI0048B_b_gic_architecture_specification.pdf
	// See ARM IHI 0048B.b 3.4.2 Special interrupt numbers when a GIC supports interrupt grouping

	if (int_id == 1022)
	{
	}

	if (int_id >= 1020)
	{
		//dbg_putchar('2');
		//LCLSPIN_LOCK(& giclock);
		//GIC_SetPriority(0, GIC_GetPriority(0));	// GICD_IPRIORITYRn(0) = GICD_IPRIORITYRn(0);
		//GICDistributor->IPRIORITYR [0] = GICDistributor->IPRIORITYR [0];
		GIC_SetPriority(0, GIC_GetPriority(0));
		//LCLSPIN_UNLOCK(& giclock);

	}
	else if (int_id != 0 /* || GIC_GetIRQStatus(0) != 0 */)
	{
		const IRQHandler_t f = IRQ_GetHandler(int_id);

	#if 0//WITHNESTEDINTERRUPTS

		if (f != (IRQHandler_t) 0)
		{
//			static const char hex [16] = "0123456789ABCDEF";
//			if ((int_id >> 8) & 0x0F)
//				dbg_putchar(hex [(int_id >> 8) & 0x0F]);
//			dbg_putchar(hex [(int_id >> 4) & 0x0F]);
//			dbg_putchar(hex [(int_id >> 0) & 0x0F]);
			__enable_irq();						/* modify I bit in CPSR */
			(* f)();	    /* Call interrupt handler */
			__disable_irq();					/* modify I bit in CPSR */
			//dbg_putchar('_');
		}

	#else /* WITHNESTEDINTERRUPTS */

		if (f != (IRQHandler_t) 0)
		{
			(* f)();	    /* Call interrupt handler */
		}

	#endif /* WITHNESTEDINTERRUPTS */

		//dbg_putchar('5');
	}
	else
	{
		//dbg_putchar('3');
		//LCLSPIN_LOCK(& giclock);
		//GIC_SetPriority(0, GIC_GetPriority(0));	// GICD_IPRIORITYRn(0) = GICD_IPRIORITYRn(0);
		//GICDistributor->IPRIORITYR [0] = GICDistributor->IPRIORITYR [0];
		GIC_SetPriority(0, GIC_GetPriority(0));
		//LCLSPIN_UNLOCK(& giclock);
	}
}


void vApplicationMallocFailedHook(void)
{
	PRINTF("malloc failed\n");

	for (;;)
		;
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
	(void)xTask;

	PRINTF("task stack overflow %s\n", pcTaskName);

	for (;;)
		;
}

void vApplicationIdleHook( void )
{
//	uart_printf("k\n");
}

#if configSUPPORT_STATIC_ALLOCATION

static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
//uart_printf("idle get task memory %x %x %x\n", *ppxIdleTaskTCBBuffer, *ppxIdleTaskStackBuffer, *pulIdleTaskStackSize);
}

static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
//	uart_printf("timer get task memory %x %x %x\n", *ppxTimerTaskTCBBuffer, *ppxTimerTaskStackBuffer, *pulTimerTaskStackSize);
}

#endif /* configSUPPORT_STATIC_ALLOCATION */

void task_init(void *arg)
{
	(void)arg;

	//syscall init
	//syscalls_init();

	//kbd_queue = xQueueCreate(10, sizeof(kbd_event_t));

	BaseType_t ret = xTaskCreate(task_blinky, "led1", STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &task_blinky_handle);
	if (ret != pdTRUE){
		PRINTF("1 not created\n");
		for (;;)
			;
	}

	BaseType_t ret2 = xTaskCreate(task_blinky2, "led2", STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &task_blinky_handle2);
	if (ret2 != pdTRUE){
		PRINTF("2 not created\n");
		for (;;)
			;
	}

	BaseType_t ret3 = xTaskCreate(task_blinky3, "led3", STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &task_blinky_handle3);
	if (ret3 != pdTRUE){
		PRINTF("3 not created\n");
		for (;;)
			;
	}
//
//	ret = xTaskCreate(task_gr, "gr", 1000, NULL, tskIDLE_PRIORITY+2, &task_gr_handle);
//	if (ret != pdTRUE){
//		printf("not created\n");
//		while(1);
//	}
//
//	usb_task_init();

	vTaskDelete(NULL);
}

/* PL1 Physical Timer */
#if (__CORTEX_A == 7U) && (__TIM_PRESENT == 1U)

static volatile uint_fast32_t gtimloadvalue;

void vConfigureTickInterrupt(void)
{
	const uint_fast32_t f = CPU_PL1_FREQ;
	const IRQn_ID_t irqn = SecurePhysicalTimer_IRQn;
	/* Stop Timer */
	PL1_SetControl(0x0);
	PL1_SetCounterFrequency(f);
	gtimloadvalue = f / configTICK_RATE_HZ;
	/* Initialize Counter */
	PL1_SetLoadValue(gtimloadvalue);

	/* Disable corresponding IRQ */
	IRQ_Disable(irqn);
	IRQ_ClearPending(irqn);
	IRQ_SetHandler(irqn, FreeRTOS_Tick_Handler);

	/* Set timer priority to lowest (Only bit 7:3 are implemented in MP1 CA7 GIC) */
	/* TickPriority is based on 16 level priority (from MCUs) so set it in 7:4 and leave bit 3=0 */
//	if (TickPriority < (1UL << 4)) {
//		IRQ_SetPriority(irqn, TickPriority << 4);
//		uwTickPrio = TickPriority;
//	} else {
//		return HAL_ERROR;
//	}
	IRQ_SetPriority(irqn, portLOWEST_USABLE_INTERRUPT_PRIORITY << portPRIORITY_SHIFT);
	/* todo: Use other SMP-related flags */
	/* Set edge-triggered IRQ */
	IRQ_SetMode(irqn, IRQ_MODE_TRIG_EDGE_RISING);

	/* Enable corresponding interrupt */
	IRQ_Enable(irqn);

	/* Kick start Timer */
	PL1_SetControl(0x1);
}

void vClearTickInterrupt(void)
{
	PL1_SetLoadValue(gtimloadvalue);
}

#elif (__CORTEX_A == 5U) || (__CORTEX_A == 9U)
// Private timer use
// Disable Private Timer and set load value
void vConfigureTickInterrupt(void)
{
	PTIM_SetControl(0);
	PTIM_SetCurrentValue(0);
	PTIM_SetLoadValue(calcdivround2(CPU_FREQ, ticksfreq * 2));	// Private Timer runs with the system frequency / 2
	// Set bits: IRQ enable and Auto reload
	PTIM_SetControl(0x06U);

	arm_hardware_set_handler(PrivTimer_IRQn, PTIM_Handler, portLOWEST_USABLE_INTERRUPT_PRIORITY << portPRIORITY_SHIFT, UINT32_C(1) << 0);

	// Start the Private Timer
	PTIM_SetControl(PTIM_GetControl() | 0x01);
}

void vClearTickInterrupt(void)
{
	PTIM_ClearEventFlag();
}

#endif

#elif ! LINUX_SUBSYSTEM //#else /* WITHRTOS */

void __WEAK FreeRTOS_SWI_Handler(void)
{
	ASSERT(0);
}

void __WEAK FreeRTOS_IRQ_Handler(void)
{
	ASSERT(0);
}

#endif /* WITHRTOS */

// Вызывается перед инициализацией NVRAM, но после инициализации SPI
void midtests(void)
{
#if WITHRTOS
	{
		  GIC_SetInterfacePriorityMask(0xffU);
		  __enable_irq();
		// now we switch to freertos
		BaseType_t ret = xTaskCreate(task_init, "init", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL);
		if (ret != pdTRUE){
			PRINTF("not created\n");
			while(1);
		}

		PRINTF("starting scheduler\n");
		vTaskStartScheduler();

		for (;;)
			;

	}
#endif /* WITHRTOS */
#if 0 && CPUSTYLE_R7S721
	// тестирование скорости после инициализации MMU
	{
		const uint32_t mask = (1uL << 13);	// P6_13
		// R7S721 pins
		arm_hardware_pio6_outputs(mask, mask);
		for (;;)
		{
			R7S721_TARGET_PORT_S(6, mask);	/* P6_13=1 */
			local_delay_ms(10);
			R7S721_TARGET_PORT_C(6, mask);	/* P6_13=0 */
			local_delay_ms(10);
		}
	}
#endif
	//hardware_cw_diagnostics(0, 0, 1);	// 'U'
#if 0
	// тестирование управляемого по SPI регистра
	{
		for (;;)
		{
			const spitarget_t target = targetctl1;
			//
			spi_select(target, SPIC_MODE3);
			spi_progval8_p1(target, 0xff);
			spi_progval8_p2(target, 0xff);
			spi_progval8_p2(target, 0xff);
			spi_progval8_p2(target, 0xff);
			spi_complete(target);
			spi_unselect(target);
			local_delay_ms(500);
			//
			spi_select(target, SPIC_MODE3);
			spi_progval8_p1(target, 0x00);
			spi_progval8_p2(target, 0x00);
			spi_progval8_p2(target, 0x00);
			spi_progval8_p2(target, 0x00);
			spi_complete(target);
			spi_unselect(target);
			local_delay_ms(500);
		}

	}
#endif
#if 0
	{
		TP();
//		nvramaddress_t place = 0;
//		// NVRAM test
//		save_i32(place, 0x12345678);
//		const uint_fast32_t v1 = restore_i32(place);
//		save_i32(place, 0xDEADBEEF);
//		const uint_fast32_t v2 = restore_i32(place);
//
//		PRINTF("NVRAM test: v1=%08lX, v2=%07lX\n", v1, v2);
		const uint8_t cmd [] =
		{
			0x03,	// FLASH READ
			(uint_fast8_t) (0 >> 8),
			(uint_fast8_t) (0 >> 0),
		};
		uint8_t data [32];
		prog_spi_io(targetext1, SPIC_SPEEDFAST, SPIC_MODE3, 0, cmd, ARRAY_SIZE(cmd), NULL, 0, data, ARRAY_SIZE(data));
		printhex(0, data, ARRAY_SIZE(data));

	}
#endif
#if 0
	// test SPI speed
	{
		TP();
		const spitarget_t target = targetext1;
		//#if WITHSPIHW
		//	hardware_spi_master_setfreq(SPISPEED);
		//#endif /* WITHSPIHW */
		//arm_hardware_pioc_outputs(SPI_ADDRESS_BITS, target);
		//arm_hardware_pioa_outputs(SPI_NAEN_BIT, SPI_NAEN_BIT);

		//for (;;)
		//{
			//GPIOA->ODR ^= SPI_NAEN_BIT;	// Debug LEDs
		//}
		for (;;)
		{

			// тестирование аппаратного SPI в 16-битном режиме
			unsigned v = 0x1234;
			prog_select(target);
			hardware_spi_connect_b16(SPIC_SPEEDFAST, SPIC_MODE3);		// если есть возможность - работаем в 16-ти битном режиме
			hardware_spi_b16_p1(v);
			unsigned v2 = hardware_spi_complete_b16();
			hardware_spi_disconnect();
			prog_unselect(target);
			local_delay_ms(1);
			PRINTF("v=%04X, v2=%04X\n", v, v2);
			continue;

//			unsigned v = 0x12;
//			unsigned v1 = 0x34;
//			spi_select(target, SPIC_MODE3);
//			spi_progval8_p1(target, v);
//			unsigned v2 = spi_progval8_p2(target, v1);
//			unsigned v3 = spi_complete(target);
//			spi_unselect(target);
//			local_delay_ms(1);
//			//ASSERT(v == v2);
//			PRINTF("v=%04X, v1=%04X v2=%04X, v3=%04X\n", v, v1, v2, v3);
//			continue;

			board_lcd_rs(0);
			spi_select(target, SPIC_MODE3);
			spi_progval8_p1(target, 0xFF);
			spi_progval8_p2(target, 0x00);
			spi_progval8_p2(target, 0xFF);
			spi_progval8_p2(target, 0x00);
			spi_progval8_p2(target, 0xFF);
			spi_progval8_p2(target, 0x00);
			spi_progval8_p2(target, 0xFF);
			spi_progval8_p2(target, 0x00);
			spi_complete(target);
			spi_unselect(target);
			//local_delay_ms(50);

			board_lcd_rs(1);
			spi_select(target, SPIC_MODE3);
			spi_progval8_p1(target, 0xFF);
			spi_progval8_p2(target, 0x00);
			spi_progval8_p2(target, 0xFF);
			spi_progval8_p2(target, 0x00);
			spi_progval8_p2(target, 0xFF);
			spi_progval8_p2(target, 0x00);
			spi_progval8_p2(target, 0xFF);
			spi_progval8_p2(target, 0x00);
			spi_complete(target);
			spi_unselect(target);
			//local_delay_ms(50);
		}
	
	}
#endif

#if 0
	{
		// SPI CS address bus verify
		for (;;)
		{
			static const spitarget_t csa [] =
			{
				//SPI_CSEL0,
				//SPI_CSEL1,
				//SPI_CSEL2,
				//SPI_CSEL3,
				//SPI_CSEL4,
				//SPI_CSEL5,
				SPI_CSEL6,
				SPI_CSEL7,
			};
			uint_fast8_t i;

			for (i = 0; i < sizeof csa / sizeof csa [0]; ++ i)
			{
				const spitarget_t cs = csa [i];
				prog_select(cs);
				local_delay_ms(1000);
				prog_unselect(cs);
				local_delay_ms(1000);
			}
		}

	}
#endif
}
#if STM32F0XX_MD
void hardware_f051_dac_initialize(void)		/* инициализация DAC на STM32F4xx */
{
	RCC->APB1ENR |= RCC_APB1ENR_DACEN; //подать тактирование
	__DSB();

	DAC1->CR = DAC_CR_EN1;
}
// вывод 12-битного значения на ЦАП - канал 1
void hardware_f051_dac_ch1_setvalue(uint_fast16_t v)
{
	DAC1->DHR12R1 = v;
}
#endif /* STM32F0XX_MD */


#if CPUSTYLE_R7S721
/* Nested interrupts testbench */



#if 1 && WITHDEBUG

	typedef struct
	{
		uint8_t lock;
		int line;
		const char * file;
	} LOCK_T;

	static volatile LOCK_T locklist16;
	static volatile LOCK_T locklist32;
	static volatile LOCK_T locklist8;

	static void lock_impl(volatile LOCK_T * p, int line, const char * file, const char * variable)
	{
	#if WITHHARDINTERLOCK
		uint8_t r;
		do
			r = __LDREXB(& p->lock);
		while (__STREXB(1, & p->lock));
		if (r != 0)
		{
			PRINTF(PSTR("LOCK @%p %s already locked at %d in %s by %d in %s\n"), p, variable, line, file, p->line, p->file);
			for (;;)
				;
		}
		else
		{
			p->file = file;
			p->line = line;
		}
	#endif /* WITHHARDINTERLOCK */
	}

	static void unlock_impl(volatile LOCK_T * p, int line, const char * file, const char * variable)
	{
	#if WITHHARDINTERLOCK
		uint8_t r;
		do
			r = __LDREXB(& p->lock);
		while (__STREXB(0, & p->lock));
		if (r == 0)
		{
			PRINTF(PSTR("LOCK @%p %s already unlocked at %d in %s by %d in %s\n"), p, variable, line, file, p->line, p->file);
			for (;;)
				;
		}
		else
		{
			p->file = file;
			p->line = line;
		}
	#endif /* WITHHARDINTERLOCK */
	}

	#define LOCK(p) do { lock_impl((p), __LINE__, __FILE__, # p); } while (0)
	#define UNLOCK(p) do { unlock_impl((p), __LINE__, __FILE__, # p); } while (0)

#else /* WITHDEBUG */

	#define LOCK(p) do {  } while (0)
	#define UNLOCK(p) do {  } while (0)

#endif /* WITHDEBUG */

static void hw_swi(void)
{
	__asm volatile (" SWI  0" : /* no outputs */ : /* no inputs */  );
}

#if WITHDEBUG && 1
// ARM_SYSTEM_PRIORITY function
static void r7s721_ostm0_interrupt_test(void)
{
	LOCK(& locklist8);
	auto int marker;
	global_disableIRQ();
	PRINTF(PSTR("  Sy:@%p INTCICCRPR=%02x cpsr=%08x!\n"), & marker, (unsigned) INTCICCRPR, (unsigned) __get_CPSR());
	global_enableIRQ();

	local_delay_ms(5);
	local_delay_ms(5);

	global_disableIRQ();
	PRINTF(PSTR("  Sy: INTCICCRPR=%02x cpsr=%08x.\n"), (unsigned) INTCICCRPR, (unsigned) __get_CPSR());
	global_enableIRQ();
	UNLOCK(& locklist8);
}

// ARM_REALTIME_PRIORITY function
static void r7s721_ostm1_interrupt_test(void)
{
	LOCK(& locklist16);

	auto int marker;
	global_disableIRQ();
	PRINTF(PSTR("    Rt:@%p INTCICCRPR=%02x cpsr=%08x!\n"), & marker, (unsigned) INTCICCRPR, (unsigned) __get_CPSR());
	global_enableIRQ();

	local_delay_ms(5);

	local_delay_ms(5);

	global_disableIRQ();
	PRINTF(PSTR("    rt: INTCICCRPR=%02x cpsr=%08x.\n"), (unsigned) INTCICCRPR, (unsigned) __get_CPSR());
	global_enableIRQ();

	UNLOCK(& locklist16);
}

// ARM_REALTIME_PRIORITY function
static void spool_encinterruptR(void)
{
	LOCK(& locklist16);

	auto int marker;
	global_disableIRQ();
	PRINTF(PSTR("    E:@%p INTCICCRPR=%02x cpsr=%08x!\n"), & marker, (unsigned) INTCICCRPR, (unsigned) __get_CPSR());
	global_enableIRQ();

	local_delay_ms(25);
	local_delay_ms(5);

	global_disableIRQ();
	PRINTF(PSTR("    e:INTCICCRPR=%02x cpsr=%08x.\n"), (unsigned) INTCICCRPR, (unsigned) __get_CPSR());
	global_enableIRQ();

	UNLOCK(& locklist16);
}

void xSWIHandler(void)
{
	PRINTF(PSTR("SWIHandler trapped.\n"));
	global_enableIRQ();

	for (;;)
	{
		global_disableIRQ();
		PRINTF(PSTR("B: INTCICCRPR=%02x cpsr=%08x*\n"), (unsigned) INTCICCRPR, (unsigned) __get_CPSR());
		global_enableIRQ();

		local_delay_ms(20);
		IRQL_t oldIrql;
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		local_delay_ms(20);
		LowerIrql(oldIrql);
	}
}

/* Главная функция программы */
int 
//__attribute__ ((used))
nestedirqtest(void)
{
	global_disableIRQ();
	cpu_initialize();		// в случае ARM - инициализация прерываний и контроллеров, AVR - запрет JTAG

	//HARDWARE_DEBUG_INITIALIZE();
	//HARDWARE_DEBUG_SET_SPEED(DEBUGSPEED);
	// выдача повторяющегося символа для тестирования скорости передачи, если ошибочная инициализация
	//for (;;)
	//	dbg_putchar(0xff);
	// тестирование приёма и передачи символов
	PRINTF(PSTR("INTCICCPMR=%02X\n"), (unsigned) INTCICCPMR);
	PRINTF(PSTR("ECHO test. Press ESC for done.\n"));
	for (;1;)
	{
		char c;
		if (dbg_getchar(& c))
		{
			if (c == 0x1b)
				break;
			dbg_putchar(c);
		}
	}
	board_initialize();		/* инициализация чипселектов и SPI, I2C, загрузка FPGA */
	hardware_timer_initialize(3);
	arm_hardware_set_handler_system(OSTMI0TINT_IRQn, r7s721_ostm0_interrupt_test);
	hardware_elkey_timer_initialize();
	hardware_elkey_set_speed(4);
	arm_hardware_set_handler_realtime(OSTMI1TINT_IRQn, r7s721_ostm1_interrupt_test);

#if defined (ENCODER_BITS)
	#if CTLSTYLE_RAVENDSP_V9
		do { \
			arm_hardware_pio2_alternative(ENCODER_BITS, R7S721_PIOALT_4); \
			arm_hardware_irqn_interrupt(1, 3, ARM_SYSTEM_PRIORITY, spool_encinterruptR); /* IRQ1, both edges */ \
			arm_hardware_irqn_interrupt(2, 3, ARM_SYSTEM_PRIORITY, spool_encinterruptR); /* IRQ2, both edges */ \
		} while (0);
	#endif
#endif /* defined (ENCODER_BITS) */
	PRINTF(PSTR("INTCICCRPR=%02x cpsr=%08x* \n"), (unsigned) INTCICCRPR, (unsigned) __get_CPSR());
	//hw_swi();
	global_enableIRQ();

	for (;;)
	{
		unsigned iccrpr0 = INTCICCRPR;
		IRQL_t oldIrql;
		RiseIrql(IRQL_SYSTEM, & oldIrql);
		unsigned iccrpr1 = INTCICCRPR;
		local_delay_ms(20);
		LowerIrql(oldIrql);

		global_disableIRQ();
		PRINTF(PSTR("iccrpr0=%02x, iccrpr1=%02x, INTCICCRPR=%02x cpsr=%08x*\n"), (unsigned) iccrpr0, (unsigned) iccrpr1, (unsigned) INTCICCRPR, (unsigned) __get_CPSR());
		global_enableIRQ();

		local_delay_ms(20);

	}
}
#endif /* CPUSTYLE_R7S721 */
#endif /* WITHDEBUG */

static unsigned RAMFUNC_NONILINE testramfunc(void)
{
	return 9;
}

static unsigned RAMFUNC_NONILINE testramfunc2(void)
{
	return 10;
}

// Сразу после начала main

#if 0 && WITHLVGL && LINUX_SUBSYSTEM

#include "linux/linux_subsystem.h"
#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lvgl/demos/lv_demos.h"
#include "lvgl/examples/lv_examples.h"
#include <linux/kd.h>

void board_tsc_initialize(void);
void board_tsc_indev_read(lv_indev_drv_t * drv, lv_indev_data_t * data);
void encoder_indev_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
void indev_enc2_spool(void);

#define DISP_BUF_SIZE	(128 * DIM_X)

pthread_t lv_tick_inc_t;

void thread_lv_tick_inc(void)
{
	while(1) {
		lv_tick_inc(1);
		usleep(1000);
	}
}

void thread_spool_encinterrupt(void)
{
	while(1)
	{
		indev_enc2_spool();
		spool_encinterrupt2();
		usleep(1000);
	}
}

#endif

#if 0 && LINUX_SUBSYSTEM
void signal_handler(int n, siginfo_t *info, void *unused)
{
	PRINTF("received value %d\n", info->si_int);
}
#endif

void lowtests(void)
{
#if 0 && LINUX_SUBSYSTEM
	struct sigaction sig;
	sig.sa_sigaction = signal_handler;
	sig.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sig, NULL);

	const char * argv [3] = { "/sbin/modprobe", "inttest", NULL, };
	linux_run_shell_cmd(argv);
	sleep(1);

	int fs = open("/dev/inttest", O_RDONLY);
	ASSERT(fs > 0);
	int pid = getpid();
	ioctl(fs, 0, pid);
	close(fs);

	for(;;) {}
#endif

#if 0 && WITHLVGL && LINUX_SUBSYSTEM

	int ttyd = open(LINUX_TTY_FILE, O_RDWR);
	if (ttyd)
		ioctl(ttyd, KDSETMODE, KD_GRAPHICS);

	close(ttyd);

	/*LVGL init*/
	lv_init();

	/*Linux frame buffer device init*/
	fbdev_init();

	/*A small buffer for LittlevGL to draw the screen's content*/
	static lv_color_t buf1[DISP_BUF_SIZE];
	static lv_color_t buf2[DISP_BUF_SIZE];

	/*Initialize a descriptor for the buffer*/
	static lv_disp_draw_buf_t disp_buf;
	lv_disp_draw_buf_init(&disp_buf, buf1, buf2, DISP_BUF_SIZE);

	/*Initialize and register a display driver*/
	static lv_disp_drv_t disp_drv;
	lv_disp_drv_init(& disp_drv);
	disp_drv.draw_buf   = & disp_buf;
	disp_drv.flush_cb   = fbdev_flush;
	disp_drv.hor_res    = DIM_X;
	disp_drv.ver_res    = DIM_Y;
	lv_disp_drv_register(& disp_drv);

	i2c_initialize();
	board_tsc_initialize();

	/* Set up touchpad input device interface */
	lv_indev_drv_t touch_drv;
	lv_indev_drv_init(& touch_drv);
	touch_drv.type = LV_INDEV_TYPE_POINTER;
	touch_drv.read_cb = board_tsc_indev_read;
	lv_indev_drv_register(& touch_drv);

	/* Encoder register */
	lv_indev_drv_t enc_drv;
	lv_indev_drv_init(& enc_drv);
	enc_drv.type = LV_INDEV_TYPE_ENCODER;
	enc_drv.read_cb = encoder_indev_read;
	lv_indev_drv_register(& enc_drv);

	linux_create_thread(& lv_tick_inc_t, thread_spool_encinterrupt, 50, 1);
	linux_create_thread(& lv_tick_inc_t, thread_lv_tick_inc, 50, 1);

//	lv_demo_benchmark_run_scene(26);
//	lv_demo_widgets();
	lv_demo_keypad_encoder();

	while(1) {
		lv_task_handler();
		usleep(10000);
	}

#endif
#if 0 && __riscv && defined(__riscv_zicsr)
	{
		unsigned vm = (csr_read_mstatus() >> 24) & 0x1F;
		unsigned sxl = (csr_read_mstatus() >> 34) & 0x03;
		unsigned uxl = (csr_read_mstatus() >> 32) & 0x03;
		unsigned xs = (csr_read_mstatus() >> 15) & 0x03;
		PRINTF("1 mstatus=%08lX, vm=%u, sxl=%u, uxl=%u, xs=%u\n", csr_read_mstatus(), vm, sxl, uxl, xs);
	}
#endif
#if 0 && __riscv && defined(__riscv_zicsr)
	{
		PRINTF("misa=0x%016lX\n", (unsigned long) csr_read_misa());
		PRINTF("mvendorid=0x%08X\n", csr_read_mvendorid());
		PRINTF("marchid=0x%08X\n", csr_read_marchid());
		PRINTF("mimpid=0x%08X\n", csr_read_mimpid());
		PRINTF("mhartid=0x%08X\n", csr_read_mhartid());
	}
#endif
#if 0 && __riscv && defined(__riscv_zicsr)
	{
		// see https://github.com/five-embeddev/riscv-csr-access/blob/master/include/riscv-csr.h

		//	0 A Atomic extension
		//	1 B Reserved
		//	2 C Compressed extension
		//	3 D Double-precision floating-point extension
		//	4 E RV32E base ISA
		//	5 F Single-precision floating-point extension
		//	6 G Reserved
		//	7 H Hypervisor extension
		//	8 I RV32I/64I/128I base ISA
		//	9 J Reserved
		//	10 K Reserved
		//	11 L Reserved
		//	12 M Integer Multiply/Divide extension
		//	13 N Tentatively reserved for User-Level Interrupts extension
		//	14 O Reserved
		//	15 P Tentatively reserved for Packed-SIMD extension
		//	16 Q Quad-precision floating-point extension
		//	17 R Reserved
		//	18 S Supervisor mode implemented
		//	19 T Reserved
		//	20 U User mode implemented
		//	21 V “V” Vector extension implemented
		//	22 W Reserved
		//	23 X Non-standard extensions present
		//	24 Y Reserved
		//	25 Z Reserved

		// Allwinner F133-A
		//	READ_CSR(misa)=00B4112D: --X-VU-S-----M---I--F-DC-A
		const unsigned misa_val = csr_read_misa();
		unsigned i;
		PRINTF("misa=%08X: ", misa_val);
		for (i = 0; i < 26; ++ i)
		{
			const int pos = 25 - i;
			const unsigned mask = UINT32_C(1) << pos;
			PRINTF("%c", (misa_val & mask) ? 'A' + pos : '-');
		}
		PRINTF("\n");

	}
#endif
#if 0 && WITHDEBUG && __riscv
	{
		PRINTF("sqrtf=%d\n", (int) (sqrtf(2) * 10000));
		PRINTF("sqrt=%d\n", (int) (sqrt(2) * 10000));
		PRINTF("sqrtf=%g\n", sqrtf(2));
		PRINTF("sqrt=%g\n", sqrt(2));

	}
#endif
#if 0 && defined (BOARD_BLINK_INITIALIZE)
	{
		// LED blink test
		uint_fast8_t state = 0;
		BOARD_BLINK_INITIALIZE();
		for (;;)
		{
			if (state)
			{
				state = 0;
				BOARD_BLINK_SETSTATE(0);
			}
			else
			{
				state = 1;
				BOARD_BLINK_SETSTATE(1);
			}
			local_delay_ms(250);
		}
	}
#endif
#if 0 && (CPUSTYLE_T113 || CPUSTYLE_F133)
	{
		PRINTF("SYS_CFG->SYS_LDO_CTRL_REG=0x%08X (expected arm: 0x0000190E, risc-v: 0x00002F0F)\n", (unsigned) SYS_CFG->SYS_LDO_CTRL_REG);
	}
#endif
#if 0 && (CPUSTYLE_T113)
	{
		PRINTF("C0_CPUX_CFG->C0_CTRL_REG0=0x%08X (expected 0x80000000)\n", (unsigned) C0_CPUX_CFG->C0_CTRL_REG0);
	}
#endif
#if 0
	{
		// CMSIS RTOS2 test
		blinky_main();
	}
#endif
//	PRINTF("TARGET_UART1_TX_MIO test\n");
//	for (;;)
//	{
//		const portholder_t pinmode = MIO_PIN_VALUE(1, 0, GPIO_IOTYPE_LVCMOS33, 1, 0, 0, 0, 0, 0);
//		gpio_output2(TARGET_UART1_TX_MIO, 1, pinmode);
//		local_delay_ms(200);
//		gpio_output2(TARGET_UART1_TX_MIO, 0, pinmode);
//		local_delay_ms(200);
//	}
#if 0 && CPUSTYLE_XC7Z && defined (ZYNQBOARD_LED_RED)
	{
		// калибровка программной задержки
		for (;;)
		{
			const portholder_t pinmode = MIO_PIN_VALUE(1, 0, GPIO_IOTYPE_LVCMOS33, 1, 0, 0, 0, 0, 0);
			gpio_output2(ZYNQBOARD_LED_RED, 0, pinmode);		// LED_R
			//gpio_output2(ZYNQBOARD_LED_GREEN, 1, pinmode);		// LED_G
			local_delay_ms(50);

			gpio_output2(ZYNQBOARD_LED_RED, 1, pinmode);		// LED_R
			//gpio_output2(ZYNQBOARD_LED_GREEN, 0, pinmode);		// LED_G
			local_delay_ms(50);

		}
	}
#endif
#if 0 && CPUSTYLE_STM32MP1
	{
		// калибровка программной задержки

		//RCC->MP_APB5ENSETR = RCC_MP_APB5ENSETR_TZPCEN;
		//PRINTF("Hello. STM32MP157\n");
		//arm_hardware_pioa_altfn20(1uL << 13, 0);	// DBGTRO
		// LED blinking test
		//const uint_fast32_t mask = (1uL << 14);	// PA14 - GREEN LED LD5 on DK1/DK2 MB1272.pdf
		//const uint_fast32_t maskd = (1uL << 14);	// PD14 - LED on small board
		const uint_fast32_t maska = (1uL << 13);	// PA13 - bootloader status LED
		//arm_hardware_piod_outputs(maskd, 1 * maskd);
		arm_hardware_pioa_outputs(maska, 1 * maska);
		for (;;)
		{
			//dbg_putchar('5');
			//(GPIOD)->BSRR = BSRR_S(maskd);
			(GPIOA)->BSRR = BSRR_S(maska);
			__DSB();
			local_delay_ms(50);
			//dbg_putchar('#');
			//(GPIOD)->BSRR = BSRR_C(maskd);
			(GPIOA)->BSRR = BSRR_C(maska);
			__DSB();
			local_delay_ms(50);

		}
	}
#endif /* CPUSTYLE_STM32MP1 */
#if 0 && WITHDEBUG
	{
		// c++ execution test
		extern void cpptest(void);
		cpptest();
	}
#endif /* WITHDEBUG */
#if 0 && CPUSTYLE_R7S721
	{
		nestedirqtest();
	}
#endif /* CPUSTYLE_R7S721 */
#if 0
	{
		// Multi-regions initialize test
		volatile static unsigned v1;
		volatile static unsigned v2 = 2;
		volatile static unsigned RAMDTCM v3;
		volatile static unsigned RAMDTCM v4 = 4;
		volatile static unsigned RAMBIGDTCM v5;
		volatile static unsigned RAMBIGDTCM v6 = 6;
		volatile static unsigned RAMFRAMEBUFF v7;
		volatile static unsigned RAMBIG v8 = 8;
		volatile unsigned v9;
		volatile unsigned v10 = 10;

		PRINTF(PSTR("Unititilalized static=%08lX @%p\n"), v1, & v1);
		PRINTF(PSTR("Ititilalized static=%08lX @%p\n"), v2, & v2);
		PRINTF(PSTR("Unititilalized RAMDTCM=%08lX @%p\n"), v3, & v3);
		PRINTF(PSTR("Ititilalized RAMDTCM=%08lX @%p\n"), v4, & v4);
		PRINTF(PSTR("Unititilalized RAMBIGDTCM=%08lX @%p\n"), v5, & v5);
		PRINTF(PSTR("Ititilalized RAMBIGDTCM=%08lX @%p\n"), v6, & v6);
		PRINTF(PSTR("Unititilalized RAMFRAMEBUFF=%08lX @%p\n"), v7, & v7);
		PRINTF(PSTR("Ititilalized RAMFRAMEBUFF=%08lX @%p\n"), v8, & v8);
		PRINTF(PSTR("RAMFUNC_NONILINE #1=%08lX @%p\n"), testramfunc(), testramfunc);
		PRINTF(PSTR("RAMFUNC_NONILINE #2=%08lX @%p\n"), testramfunc2(), testramfunc2);
		PRINTF(PSTR("Unititilalized auto=%08lX @%p\n"), v9, & v9);
		PRINTF(PSTR("Ititilalized auto=%08lX @%p\n"), v10, & v10);
	}
#endif
#if 0
	{
		// Калиьбровка задержек для данного процссора
		// See local_delay_uscycles()
		//enum { WORKMASK	 = 1ul << 7 };	// PB7
		enum { WORKMASK	 = 1ul << 10 };	// P7_10
		//arm_hardware_piob_outputs(WORKMASK, WORKMASK);
		arm_hardware_pio7_outputs(WORKMASK, WORKMASK);

		for (;;)
		{
			//(GPIOB)->BSRR = BSRR_S(WORKMASK);
			R7S721_TARGET_PORT_S(7, WORKMASK);
			local_delay_ms(5);
			//(GPIOB)->BSRR = BSRR_C(WORKMASK);
			R7S721_TARGET_PORT_C(7, WORKMASK);
			local_delay_ms(5);
		}
	}
#endif
#if 0
	{
		HARDWARE_DEBUG_INITIALIZE();
		HARDWARE_DEBUG_SET_SPEED(DEBUGSPEED);
		//for (;;)
		dbg_puts_impl_P(PSTR("Version " __DATE__ " " __TIME__ " 2 debug session starts.\n"));
		#if 0
			// выдача повторяющегося символа для тестирования скорости передачи, если ошибочная инициализация
			for (;;)
				dbg_putchar(0xf0);
		#endif
		// тестирование приёма и передачи символов
		for (;1;)
		{
			char c;
			if (dbg_getchar(& c))
			{
				dbg_putchar(c);
				if (c == 0x1b)
					break;
			}
		}
		for (;;)
			;
	}
#endif
#if 0
	{
		// "бегающие огоньки" на светодиодах - 74HC595
		//arm_hardware_pio1_inputs(0xFF00);
		// тестирование SPI
		spi_initialize();

		uint_fast8_t i = 0;
		for (;;)
		{
			const uint_fast8_t timev = 1; //(~ GPIO.PPR1 >> 8) & 0xFF;
			const spitarget_t target = targetext1;
			uint_fast8_t v;
			//
			//TP();
			spi_select2(target, SPIC_MODE3, SPIC_SPEEDFAST);	// В FPGA регистр тактируется не в прямую
			spi_progval8_p1(target, 0xFF);		/* read status register */
			spi_complete(target);

			spi_to_read(target);
			v = spi_read_byte(target, 0xff);
			spi_to_write(target);

			spi_unselect(target);	/* done sending data to target chip */

			//local_delay_ms(10 * timev);
			++ i;
		}
	}
#endif /*  */
#if 0
	{
		// FPU test
		HARDWARE_DEBUG_INITIALIZE();
		HARDWARE_DEBUG_SET_SPEED(DEBUGSPEED);
		//for (;;)
		dbg_puts_impl_P(PSTR("Version " __DATE__ " " __TIME__ " 3 debug session starts.\n"));
		// выдача повторяющегося символа для тестирования скорости передачи, если ошибочная инициализация
		//for (;;)
		//	dbg_putchar(0xff);


		extern unsigned long __isr_vector__;
		PRINTF(PSTR("__isr_vector__=%p\n"), & __isr_vector__);
		volatile int a = 10, b = 0;
		volatile int c = a / b;
		TP();
		unsigned long i;
		for (i = 0x8000000;; ++ i)
		{
			TP();
			const double a = i ? i : 1;
			//const int ai = (int) (sin(a) * 1000);
			TP();
			PRINTF(PSTR("Hello! %lu, sqrt(%lu)=%lu\n"), i, (unsigned) a, (unsigned) sqrt(a));
			TP();
		}
	}
#endif /* CPUSTYLE_R7S721 */
#if 0 && CPUSTYLE_R7S721
	// тестирование скорости до инициализации MMU
	{
		const uint32_t mask = (1uL << 13);	// P6_13
		// R7S721 pins
		arm_hardware_pio6_outputs(mask, mask);
		for (;;)
		{
			R7S721_TARGET_PORT_S(6, mask);	/* P6_13=1 */
			local_delay_ms(10);
			R7S721_TARGET_PORT_C(6, mask);	/* P6_13=0 */
			local_delay_ms(10);
		}
	}
#endif
#if 0
	{
		// Формирование импульсов на выводе процессора
		for (;;)
		{
			//const uint32_t WORKMASK = 1uL << 31;	// PA31
			const uint32_t WORKMASK =
				(1uL << 13) |		// BP13
				(1uL << 14) |		// BP14
				(1uL << 15) |		// BP15
				0;

			arm_hardware_piob_outputs(WORKMASK, WORKMASK * 1);
			hardware_spi_io_delay();
			arm_hardware_piob_outputs(WORKMASK, WORKMASK * 0);
			hardware_spi_io_delay();
		}
	}
#endif
#if 0
	hardware_f051_dac_initialize();
	for (;;)
	{
		unsigned i;
		for (i = 0; i < 4096; ++ i)
		{
			hardware_f051_dac_ch1_setvalue(i);
			arm_hardware_pioa_outputs(0x01, 0x01 * (i % 2));
		}
	}
#endif
#if ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED
	hardware_tim21_initialize();
	local_delay_ms(100);	// пока на выходе выпрямителя появится напряжение */
#endif /* ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED */
}
