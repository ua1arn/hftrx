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

#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_CS42L51)

#include "audio.h"

// https://github.com/STMicroelectronics/STM32CubeL4/blob/master/Drivers/BSP/Components/cs42l51/cs42l51.c

#define CS42L51_ADDRESS_W 0x94	// AD0 (pin 04) tied low.
#define CS42L51_ADDRESS_R (CS42L51_ADDRESS_W | 0x01)

void cs42l51_setreg(
	uint_fast8_t regv,			/* 8 bit map byte value */
	uint_fast8_t datav			/* 8 bit value */
	)
{
	const uint_fast16_t fulldata = regv * 256 + (datav & 0xff);

#if CODEC_TYPE_CS42L51_USE_SPI
	// кодек управляется по SPI
	const spitarget_t target = targetcodec1;	/* addressing to chip */

	#if WITHSPILOWSUPPORTT || 1
		// Работа совместно с фоновым обменом SPI по прерываниям
		uint8_t txbuf [2];

		USBD_poke_u16_BE(txbuf, fulldata);
		prog_spi_io(target, SPIC_SPEEDFAST, CS42L51_SPIMODE, txbuf, ARRAY_SIZE(txbuf), NULL, 0);

	#elif WITHSPIEXT16

		hardware_spi_connect_b16(SPIC_SPEEDFAST, CS42L51_SPIMODE);
		prog_select(target);	/* start sending data to target chip */
		hardware_spi_b16_p1(fulldata);
		hardware_spi_complete_b16();
		prog_unselect(target);
		hardware_spi_disconnect();

	#else /* WITHSPIEXT16 */

		spi_select(target, CS42L51_SPIMODE);
		spi_progval8_p1(target, fulldata >> 8);		// LSB=b8 of datav
		spi_progval8_p2(target, fulldata >> 0);
		spi_complete(target);
		spi_unselect(target);

	#endif /* WITHSPIEXT16 */

#else /* CODEC_TYPE_CS42L51_USE_SPI */

	// кодек управляется по I2C
	i2c2_start(CS42L51_ADDRESS_W);
	i2c2_write(regv);
	i2c2_write(datav);
	i2c2_waitsend();
	i2c2_stop();

#endif /* CODEC_TYPE_CS42L51_USE_SPI */
}


static void cs42l51_initialize_fullduplex(void (* io_control)(uint_fast8_t on), uint_fast8_t master)
{
}

/* Установка громкости на наушники */
static void cs42l51_setvolume(uint_fast16_t gain, uint_fast8_t mute, uint_fast8_t mutespk)
{
}

/* Выбор LINE IN как источника для АЦП вместо микрофона */
static void cs42l51_lineinput(uint_fast8_t linein, uint_fast8_t mikeboost20db, uint_fast16_t mikegain, uint_fast16_t linegain)
{
}

/* Параметры обработки звука с микрофона (эхо, эквалайзер, ...) */
static void cs42l51_setprocparams(
	uint_fast8_t procenable,		/* включение обработки */
	const uint_fast8_t * gains		/* массив с параметрами */
	)
{
}

static void cs42l51_stop(void)
{
#if CODEC_TYPE_CS42L51_MASTER
	// после RESET кодек при подаче MCLK формирует WS и BCLK... конфликт с выходами FPGA, если тактирование от неё.
	cs42l51_setreg(CS42L51_RESET, 0x00);	// RESET
#endif /* CODEC_TYPE_CS42L51_MASTER */
}

/* требуется ли подача тактирования для инициадизации кодека */
static uint_fast8_t cs42l51_clocksneed(void)
{
	return 0;
}

const codec1if_t *
board_getaudiocodecif(void)
{

	static const char codecname [] = "CS42L51";

	/* Интерфейс управления кодеком */
	static const codec1if_t ifc =
	{
		cs42l51_clocksneed,
		cs42l51_stop,
		cs42l51_initialize_fullduplex,
		cs42l51_setvolume,		/* Установка громкости на наушники */
		cs42l51_lineinput,		/* Выбор LINE IN как источника для АЦП вместо микрофона */
		cs42l51_setprocparams,	/* Параметры обработки звука с микрофона (эхо, эквалайзер, ...) */
		codecname				/* Название кодека (всегда последний элемент в структуре) */
	};

	return & ifc;
}

//
// Управление кодеком CS42L51
#endif /* defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_CS42L51) */
