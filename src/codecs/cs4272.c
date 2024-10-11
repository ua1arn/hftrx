/* $Id$ */
//
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"

#include "board.h"

#include "formats.h"
#include "spi.h"
#include "audio.h"

//
// Управление кодеком CS4272
//
// В slave mode данный кодек изменяет выдаваетмые данные по спадающем фронту сигнала SCLK.
// Судя по документации, ожидает что мастер так же изменяет выдаваемые данные по спадающему фронту SCLK.
//

#define CS4272_SPIMODE		SPIC_MODE3
#define CS4272_SPIC_SPEED	SPIC_SPEED4M	// No more then 6 MHz

#if defined (CS4272_ADDRESS_7BITS)
	#define CS4272_ADDRESS_W(tg)	((CS4272_ADDRESS_7BITS) << 2)	// I2C address: 0x20 or 0x22	- depend on adress pin state
										// Also used as SPI prefix byte
	#define CS4272_ADDRESS_R(tg)	(CS4272_ADDRESS_W(tg) + 1)

#else
	#define CS4272_ADDRESS_W(tg)	((tg) ? 0x22 : 0x20)	// I2C address: 0x20 or 0x22	- depend on adress pin state
										// Also used as SPI prefix byte
	#define CS4272_ADDRESS_R(tg)	(CS4272_ADDRESS_W(tg) + 1)

#endif

//memory address pointers
#define MODE_CONTROL_1 0x1 //memory address pointer for modeControl1
#define DAC_CONTROL 0x2 //memory address pointer for DACControl
#define DAC_VOLUME_AND_MIXING 0x3 //memory address pointer for DACVolume and Mixing control
#define DAC_VOLUME_A 0x4 //memory address pointer for DACVolumeA
#define DAC_VOLUME_B 0x5 //memory address pointer for DACVolumeB
#define ADC_CONTROL 0x6 //memory address pointer for ADC Control
#define MODE_CONTROL_2 0x7 //memory address pointer for ModeControlTwo
#define CHIP_ID 0x8	// I2C only

#define ADC_CONTROL_HPF_DISABLE_B 0x01
#define ADC_CONTROL_HPF_DISABLE_A 0x02
#define ADC_CONTROL_I2S 0x10
#define ADC_CONTROL_DITHER16 0x20

#define MODE_CONTROL_2_PDN 0x01
#define MODE_CONTROL_2_CPEN 0x02
#define MODE_CONTROL_2_LOOP 0x10

// так как на приёме используется 32-битный фрейм, можно использовать только
// формат left justified - так как кодек "не знает" когда закончится фрейм.
enum { USE_I2S = 1 };	// 0 = left justified, 1 = Philips I2S

//CODEC_TYPE_CS4272_USE_32BIT
//enum { USE_32BIT = 1 };	// 0 = 16 bit stereo, 1 = 32 bit stereo
// NOT USED AT ALL

static void cs4272_setreg(
	uint_fast8_t tg,
	uint_fast8_t mapv,	// Memory Address Pointer
	uint_fast8_t datav
	)
{
#if CODEC_TYPE_CS4272_STANDALONE
	return;
#elif CODEC_TYPE_CS4272_USE_SPI
	// кодек управляется по SPI
	const spitarget_t target = tg ? targetcodec2 : targetcodec2;	/* addressing to chip */

	#if WITHSPILOWSUPPORTT || 1
		// Работа совместно с фоновым обменом SPI по прерываниям
		const uint8_t txbuf [3] =
		{
			CS4272_ADDRESS_W(tg),
			mapv,
			datav,
		};

		prog_spi_io(target, CS4272_SPIC_SPEED, CS4272_SPIMODE, txbuf, ARRAY_SIZE(txbuf), NULL, 0, NULL, 0);

	#else /* WITHSPILOWSUPPORTT */

		spi_select2(target, CS4272_SPIMODE, CS4272_SPIC_SPEED);	/* Enable SPI */
		spi_progval8_p1(target, CS4272_ADDRESS_W(tg));		// Chip Aaddress, D0=0: write
		spi_progval8_p2(target, mapv);
		spi_progval8_p2(target, datav);
		spi_complete(target);
		spi_unselect(target);

	#endif /* WITHSPILOWSUPPORTT */

#else

	#if WITHTWIHW
		uint8_t buff [] = { mapv, datav, };
		i2chw_write(CS4272_ADDRESS_W(tg), buff, ARRAY_SIZE(buff));
	#elif WITHTWISW
		// кодек управляется по I2C
		i2c_start(CS4272_ADDRESS_W(tg));
		i2c_write(mapv);
		i2c_write(datav);
		i2c_waitsend();
		i2c_stop();
	#endif


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

#if ! CODEC_TYPE_CS4272_USE_SPI
static uint_fast8_t cs4272_getreg(
	uint_fast8_t tg,
	uint_fast8_t mapv	// Memory Address Pointer
	)
{
	uint8_t v = 0xFF;
#if CODEC_TYPE_CS4272_STANDALONE
	return v;
#else

	#if WITHTWIHW
		uint8_t buff [] = { mapv, };
		i2chw_write(CS4272_ADDRESS_W(tg), buff, ARRAY_SIZE(buff));
		i2chw_read(CS4272_ADDRESS_R(tg), & v, 1);
	#elif WITHTWISW

		// кодек управляется по I2C
		i2c_start(CS4272_ADDRESS_W(tg));
		i2c_write(mapv);
		i2c_waitsend();
		i2c_stop();

		i2c_start(CS4272_ADDRESS_R(tg));
		i2c_read(& v, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */
	#endif
	// resd back for test
	/*
	uint_fast8_t v1;

	i2c_start(CS4272_ADDRESS_R);
	//i2c_write(mapv);
	i2c_read(& v1, I2C_READ_ACK_NACK);

	debug_printf_P(PSTR("cs4272: a=%02x, v1=%02x (%02x)\n"), mapv, v1, datav);
	*/
#endif
	return v;
}
#endif

// MCLK должен уже подаваться в момент инициализации
static void cs4272_initialize_fullduplex_addr(uint_fast8_t tg, uint_fast8_t master)
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
    cs4272_setreg(tg, MODE_CONTROL_2,
    	MODE_CONTROL_2_PDN | MODE_CONTROL_2_CPEN	// write 0x03 to register 0x07 within 10ms of bringing RST_bar high
    	);
    cs4272_setreg(tg, MODE_CONTROL_1,
		((USE_I2S ? 0x01 : 0x00) << 0) |		/* DAC_DIF2:DAC_DIF1:DAC_DIF0 format LJ/I2S */
		0x00 * (UINT32_C(1) << 4) |				/* ratio select: MCLK/LRCK=256, SCLK/LRCK=64 */
		!! master * (UINT32_C(1) << 3) |		/* master/slave mode */
		0x00
		);

    cs4272_setreg(tg, DAC_CONTROL,
		//(2U << 4) |	// De-Emphasis Control (Bits 5:4), 2: 48 kHz de-emphasis
		//(1U << 6) |	// Interpolation Filter Select (Bit 6), 0: fast roll off filter
		0);
    cs4272_setreg(tg, DAC_VOLUME_AND_MIXING,	// DACVolume and Mixing control
		//(1U << 6) |	// A=B
		(0x09 << 0) |	// ATAPI mixing control: aL & bR
		//(0x06 << 0) |	// ATAPI mixing control reverse
		0);
    cs4272_setreg(tg, DAC_VOLUME_A,
    	0);	// set attenuation
    cs4272_setreg(tg, DAC_VOLUME_B,
    	0);	// set attenuation

    cs4272_setreg(tg, ADC_CONTROL,
		((USE_I2S ? 0x01 : 0x00) * ADC_CONTROL_I2S) |		/* ADC_DIF: format LJ/I2S */
		//(1 * ADC_CONTROL_HPF_DISABLE_B) |
		//(1 * ADC_CONTROL_HPF_DISABLE_A) |
		//0x04 |	// MUTE B
		//0x08 |	// MUTE A
		0);

    cs4272_setreg(tg, MODE_CONTROL_2,
    	MODE_CONTROL_2_CPEN
		);
	
#if 1
	// calibration
	local_delay_ms(200);
    cs4272_setreg(tg, ADC_CONTROL,	/* ADC Control - Address 06h */
		((USE_I2S ? 0x01 : 0x00) << 4) |		/* ADC_DIF: 0: Left Justified, up to 24-bit data, 1: I2S, up to 24-bit data */
		(1 * ADC_CONTROL_HPF_DISABLE_B) |
		(1 * ADC_CONTROL_HPF_DISABLE_A) |
		//0x04 |	// MUTE B
		//0x08 |	// MUTE A
		0);
#endif

#if ! CODEC_TYPE_CS4272_USE_SPI
	{
		const uint_fast8_t chip_id = cs4272_getreg(tg, CHIP_ID);
		PRINTF("cs4272_initialize_fullduplex_addr: tg=%d, chip_id=0x%02X\n", (int) tg, (unsigned) chip_id);

//	    cs4272_setreg(tg, DAC_VOLUME_A, 0x55);	// set attenuation
//		const uint_fast8_t volume_a1 = cs4272_getreg(tg, DAC_VOLUME_A);
//		PRINTF("cs4272_initialize_fullduplex_addr: tg=%d, volume_a1=0x%02X\n", (int) tg, (unsigned) volume_a1);
//
//	    cs4272_setreg(tg, DAC_VOLUME_A, 0xAA);	// set attenuation
//		const uint_fast8_t volume_a2 = cs4272_getreg(tg, DAC_VOLUME_A);
//		PRINTF("cs4272_initialize_fullduplex_addr: tg=%d, volume_a2=0x%02X\n", (int) tg, (unsigned) volume_a2);
	}
#endif /* ! CODEC_TYPE_CS4272_USE_SPI */
}


// MCLK должен уже подаваться в момент инициализации
static void cs4272_initialize_fullduplex(void (* io_control)(uint_fast8_t on), uint_fast8_t master)
{
	cs4272_initialize_fullduplex_addr(0, master);
}

/* требуется ли подача тактирования для инициадизации кодека */
static uint_fast8_t cs4272_clocksneed(void)
{
	return 1;
}

#if defined(CODEC2_TYPE) && (CODEC2_TYPE == CODEC_TYPE_CS4272)

const codec2if_t * board_getfpgacodecif(void)
{
	static const char codecname [] = "CS4272";

	/* Интерфейс управления кодеком */
	static const codec2if_t ifc =
	{
		cs4272_clocksneed
		cs4272_initialize_fullduplex,
		codecname
	};

	return & ifc;
}

#endif /* defined(CODEC2_TYPE) && (CODEC2_TYPE == CODEC_TYPE_CS4272) */

#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_CS4272)

/* требуется ли подача тактирования для инициадизации кодека */
static uint_fast8_t nau8822_clocksneed(void)
{
	return 1;
}

static void cs4272_stop(void)
{
}


/* Установка громкости на наушники */
static void cs4272_setvolume(uint_fast16_t gainL, uint_fast16_t gainR, uint_fast8_t mute, uint_fast8_t mutespk)
{
}


/* Выбор LINE IN как источника для АЦП вместо микрофона */
static void cs4272_lineinput(uint_fast8_t linein, uint_fast8_t mikeboost20db, uint_fast16_t mikegain, uint_fast16_t linegain)
{
}


/* Параметры обработки звука с микрофона (эхо, эквалайзер, ...) */
static void cs4272_setprocparams(
	uint_fast8_t procenable,		/* включение обработки */
	const uint_fast8_t * gains		/* массив с параметрами */
	)
{
}

const codec1if_t *
board_getaudiocodecif(void)
{

	static const char codecname [] = "CS4272";

	/* Интерфейс управления кодеком */
	static const codec1if_t ifc =
	{
		cs4272_clocksneed,
		cs4272_stop,
		cs4272_initialize_fullduplex,
		cs4272_setvolume,		/* Установка громкости на наушники */
		cs4272_lineinput,		/* Выбор LINE IN как источника для АЦП вместо микрофона */
		cs4272_setprocparams,	/* Параметры обработки звука с микрофона (эхо, эквалайзер, ...) */
		codecname				/* Название кодека (всегда последний элемент в структуре) */
	};

	return & ifc;
}

#endif /* defined(CODEC2_TYPE) && (CODEC2_TYPE == CODEC_TYPE_CS4272) */
