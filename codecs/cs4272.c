/* $Id$ */
//
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"
#include "board.h"

#include "./formats.h"
#include "../inc/spi.h"

#if defined(CODEC2_TYPE) && (CODEC2_TYPE == CODEC_TYPE_CS4272)

#include "audio.h"

//
// Управление кодеком CS4272
//
// В slave mode данный кодек изменяет выдаваетмые данные по спадающем фронту сигнала SCLK.
// Судя по документации, ожидает что мастер так же изменяет выдаваемые данные по спадающему фронту SCLK.
//

#define CS4272_SPIMODE		SPIC_MODE3
#define CS4272_SPIC_SPEED	SPIC_SPEED4M	// No more then 6 MHz

#define CS4272_ADDRESS_W	0x20	// I2C address: 0x20 or 0x22	- depend on adress pin state
									// Also used as SPI prefix byte
#define CS4272_ADDRESS_R	(CS4272_ADDRESS_W + 1)

//memory address pointers
#define MODE_CONTROL_1 0x1 //memory address pointer for modeControl1
#define DAC_CONTROL 0x2 //memory address pointer for DACControl
#define DAC_VOLUME_AND_MIXING 0x3 //memory address pointer for DACVolume and Mixing control
#define DAC_VOLUME_A 0x4 //memory address pointer for DACVolumeA
#define DAC_VOLUME_B 0x5 //memory address pointer for DACVolumeB
#define ADC_CONTROL 0x6 //memory address pointer for ADC Control
#define MODE_CONTROL_2 0x7 //memory address pointer for ModeControlTwo


#define ADC_CONTROL_HPF_DISABLE_B 0x01
#define ADC_CONTROL_HPF_DISABLE_A 0x02
#define ADC_CONTROL_I2S 0x10
#define ADC_CONTROL_DITHER16 0x20

#define MODE_CONTROL_2_PDN 0x01
#define MODE_CONTROL_2_CPEN 0x02
#define MODE_CONTROL_2_LOOP 0x10

// так как на приёме используется 32-битный фрейм, можно использовать только
// формат left justified - так как кодек "не знает" когда закончится фрейм.
enum { USE_I2S = 0 };	// 0 = left justified, 1 = phillips I2S


static void cs4272_setreg(
	uint_fast8_t mapv,	// Memory Address Pointer
	uint_fast8_t datav
	)
{
#if CODEC_TYPE_CS4272_STANDALONE
	return;
#elif CODEC_TYPE_CS4272_USE_SPI
	// кодек управляется по SPI
	const spitarget_t target = targetcodec2;	/* addressing to chip */

	spi_select2(target, CS4272_SPIMODE, CS4272_SPIC_SPEED);	/* Enable SPI */
	spi_progval8_p1(target, CS4272_ADDRESS_W);		// Chip Aaddress, D0=0: write
	spi_progval8_p2(target, mapv);
	spi_progval8_p2(target, datav);
	spi_complete(target);
	spi_unselect(target);

#else
	// кодек управляется по I2C
	i2c_start(CS4272_ADDRESS_W);
	i2c_write(mapv);
	i2c_write(datav);
	i2c_waitsend();
	i2c_stop();

	// resd back for test
	/*
	uint_fast8_t v1;

	i2c_start(CS4272_ADDRESS_R);
	//i2c_write(mapv);
	i2c_read(& v1, I2C_READ_ACK_NACK);

	debug_printf_P(PSTR("cs4272: a=%02x, v1=%02x (%02x)\n"), mapv, v1, datav);
	*/
#endif
}

// MCLK должен уже подаваться в момент инициализации
static void cs4272_initialize_fullduplex(void)
{
	board_codec2_nreset(1);	// Выставить сигнал сброса
	board_update();
	local_delay_ms(2);
	board_codec2_nreset(0);	// снять сигнал сброса
	board_update();

#if CODEC_TYPE_CS4272_STANDALONE
	return;
#endif /* CODEC_TYPE_CS4272_STANDALONE */

	    //CODEC START SEQUENCE
    cs4272_setreg(MODE_CONTROL_2, MODE_CONTROL_2_PDN | MODE_CONTROL_2_CPEN);// write 0x03 to register 0x07 within 10ms of bringing RST_bar high
    cs4272_setreg(MODE_CONTROL_1, 
		(USE_I2S << 0) |		/* DAC_DIF2:DAC_DIF1:DAC_DIF0 format LJ/I2S */
		(0U << 4) |		/* ratio select: Slave Mode, MCLK/LRCK=256, SCLK/LRCK=32, 64, 128 */
		0x00
		);

    cs4272_setreg(DAC_CONTROL, 
		//(2U << 4) |	// De-Emphasis Control (Bits 5:4), 2: 48 kHz de-emphasis
		//(1U << 6) |	// Interpolation Filter Select (Bit 6), 0: fast roll off filter
		0);
    cs4272_setreg(DAC_VOLUME_AND_MIXING,	// DACVolume and Mixing control
		(1U << 6) |	// A=B
		(6U << 0) |	// ATAPI mixing control
		//(9U << 0) |	// ATAPI mixing control reverse
		0);
    cs4272_setreg(DAC_VOLUME_A, 0);	// set attenuation
    cs4272_setreg(DAC_VOLUME_B, 0);	// set attenuation

    cs4272_setreg(ADC_CONTROL, 
		(USE_I2S << 4) |		/* ADC_DIF: format LJ/I2S */
		//(1 * ADC_CONTROL_HPF_DISABLE_B) |
		//(1 * ADC_CONTROL_HPF_DISABLE_A) |
		//0x04 |	// MUTE B
		//0x08 |	// MUTE A
		0);

    cs4272_setreg(MODE_CONTROL_2, MODE_CONTROL_2_CPEN);
	
#if 1
	// calibration
	local_delay_ms(200);
    cs4272_setreg(ADC_CONTROL, 
		(USE_I2S << 4) |		/* ADC_DIF: format LJ/I2S */
		(1 * ADC_CONTROL_HPF_DISABLE_B) |
		(1 * ADC_CONTROL_HPF_DISABLE_A) |
		//0x04 |	// MUTE B
		//0x08 |	// MUTE A
		0);
#endif
}


const codec2if_t * board_getfpgacodecif(void)
{
	static const char codecname [] = "CS4272";

	/* Интерфейс цправления кодеком */
	static const codec2if_t ifc =
	{
		cs4272_initialize_fullduplex,
		codecname
	};

	return & ifc;
}

#endif /* defined(CODEC2_TYPE) && (CODEC2_TYPE == CODEC_TYPE_CS4272) */
