/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "../inc/spi.h"
#include "hardware.h"
#include "board.h"
#include "formats.h"	// for debug prints


//#include "./formats.h"

#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_NAU8822L)
//
// Управление кодеком NOVOTON NAU8822L
//
#include "audio.h"
#include "nau8822.h"

// Clock period, SCLK no less then 80 nS (не выше 12.5 МГц)
#define NAU8822_SPIMODE		SPIC_MODE3
#define NAU8822_ADDRESS_W	0x34	// I2C address: 0x34

// Условие использования оптимизированных функций обращения к SPI
#define WITHSPIEXT16 (WITHSPIHW && WITHSPI16BIT)

/* data is
*   D15..D9 NAU8822 register offset
*   D8...D0 register data
*/

// The NAU8822L can function only as a slave device when in the 2-wire interface configuration

// SPI 3-Wire Write Operation
/*
 Whenever the MODE pin on the NAU8822L is in the logic HIGH condition,
 the device control interface will operate in the 3-Wire Write mode. 
 This is a write-only mode that does not require the fourth wire normally 
 used to read data from a device on an SPI bus implementation. 
 This mode is a 16-bit transaction consisting of a 7-bit Control Register Address, 
 and 9-bits of control register data.
*/
static void nau8822_setreg(
	uint_fast8_t regv,			/* 7 bit value */
	uint_fast16_t datav			/* 9 bit value */
	)
{
	const uint_fast16_t fulldata = regv * 512 + (datav & 0x1ff);

#if CODEC_TYPE_NAU8822_USE_SPI
	// кодек управляется по SPI
	const spitarget_t target = targetcodec1;	/* addressing to chip */

	#if WITHSPIEXT16

		hardware_spi_connect_b16(SPIC_SPEEDFAST, NAU8822_SPIMODE);
		prog_select(target);	/* start sending data to target chip */
		hardware_spi_b16_p1(fulldata);
		hardware_spi_complete_b16();
		prog_unselect(target);
		hardware_spi_disconnect();

	#else /* WITHSPIEXT16 */

		spi_select(target, NAU8822_SPIMODE);
		spi_progval8_p1(target, fulldata >> 8);		// LSB=b8 of datav
		spi_progval8_p2(target, fulldata >> 0);
		spi_complete(target);
		spi_unselect(target);

	#endif /* WITHSPIEXT16 */

#else /* CODEC_TYPE_NAU8822_USE_SPI */

	// кодек управляется по I2C
	i2c_start(NAU8822_ADDRESS_W);
	i2c_write(fulldata >> 8);
	i2c_write(fulldata >> 0);
	i2c_waitsend();
	i2c_stop();

#endif /* CODEC_TYPE_NAU8822_USE_SPI */
}

/* Установка громкости на наушники */
static void nau8822_setvolume(uint_fast16_t gain, uint_fast8_t mute, uint_fast8_t mutespk)
{
	//PRINTF("nau8822_setvolume: gain=%d, mute=%d, mutespk=%d\n", (int) gain, (int) mute, (int) mutespk);
	uint_fast8_t vmutehp = 0;
	uint_fast8_t vmutespk = 0;
	// 0x3F: +6 dB
	// 0x39: 0 dB: 2.7..2.8 volt p-p at each SPK output
	uint_fast8_t levelhp = (gain - BOARD_AFGAIN_MIN) * (NAU8822L_OUT_HP_MAX - NAU8822L_OUT_HP_MIN) / (BOARD_AFGAIN_MAX - BOARD_AFGAIN_MIN) + NAU8822L_OUT_HP_MIN;
	uint_fast8_t levelspk = (gain - BOARD_AFGAIN_MIN) * (NAU8822L_OUT_SPK_MAX - NAU8822L_OUT_SPK_MIN) / (BOARD_AFGAIN_MAX - BOARD_AFGAIN_MIN) + NAU8822L_OUT_SPK_MIN;
	if (mute)
	{
		vmutehp = 0x40;
		vmutespk = 0x40;
	}
	if (mutespk)
	{
		vmutespk = 0x40;
	}
	//debug_printf_P(PSTR("nau8822_setvolume: level=%02x start\n"), level);

	// Установка уровня вывода на наушники
	nau8822_setreg(NAU8822_LOUT1_HP_CONTROL, vmutehp | (levelhp & 0x3F) | 0);
	nau8822_setreg(NAU8822_ROUT1_HP_CONTROL, vmutehp | (levelhp & 0x3F) | 0x100);

//{0x34, 0x139},
//{0x35, 0x139},

	// Установка уровня вывода на динамик
	nau8822_setreg(NAU8822_LOUT2_SPK_CONTROL, vmutespk | (levelspk & 0x3F) | 0);
	nau8822_setreg(NAU8822_ROUT2_SPK_CONTROL, vmutespk | (levelspk & 0x3F) | 0x100);
//{0x36, 0x139},
//{0x37, 0x139},

}

/* Выбор LINE IN как источника для АЦП вместо микрофона */
static void nau8822_lineinput(uint_fast8_t linein, uint_fast8_t mikebust20db, uint_fast16_t mikegain, uint_fast16_t linegain)
{
	// Микрофон подключен к LMICN, LMICP=common
	// Входы RMICN, RMICP никуда не подключены
	// Line input подключены к LAUXIN, RAUXIN
//{0x0f, 0x1ff},
//{0x10, 0x1ff},
	if (linein != 0)
	{
		// переключение на линейный вход
		// Line input подключены к LAUXIN, RAUXIN
		//const uint_fast8_t auxinpgaval = 0x1; // 1..7: -12..+6 dB
		const uint_fast8_t auxinpgaval = (linegain - WITHLINEINGAINMIN) * (0x07 - 0x01) / (WITHLINEINGAINMAX - WITHLINEINGAINMIN) + 0x01;
		//const uint_fast8_t auxinpgaval = 0x10;	// 0x10 - default, 0x00..0x3f mean -12 db..+35.25 dB in 0.75 dB step
		nau8822_setreg(NAU8822_LEFT_INP_PGA_GAIN, 0x40 | 0);	// PGA muted
		nau8822_setreg(NAU8822_RIGHT_INP_PGA_GAIN, 0x40 | 0x100);	// write both valuse simultaneously
		//
		nau8822_setreg(NAU8822_LEFT_ADC_BOOST_CONTROL, auxinpgaval);	// LLINEIN disconnected, LAUXIN connected w/o gain
		nau8822_setreg(NAU8822_RIGHT_ADC_BOOST_CONTROL, auxinpgaval);	// RLINEIN disconnected, RAUXIN connected w/o gain
		//
	}
	else
	{
		// переключение на микрофон
		// Микрофон подключен к LMICN, LMICP=common
		//const uint_fast8_t mikepgaval = 0x10;	// 0x10 - default, 0x00..0x3f mean -12 db..+35.25 dB in 0.75 dB step
		const uint_fast8_t mikepgaval = (mikegain - WITHMIKEINGAINMIN) * (0x3f) / (WITHMIKEINGAINMAX - WITHMIKEINGAINMIN) + 0x00;
		//
		nau8822_setreg(NAU8822_LEFT_INP_PGA_GAIN, mikepgaval | 0);	// PGA volume control setting = 0.0dB
		nau8822_setreg(NAU8822_RIGHT_INP_PGA_GAIN, 0x40 | mikepgaval | 0x100);	// 0x40 = PGA in muted condition not connected to RADC Mix/Boost stage
		// 
		nau8822_setreg(NAU8822_LEFT_ADC_BOOST_CONTROL, 0x000 | 0x100 * (mikebust20db != 0));	// 0x100 - 20 dB boost ON
		nau8822_setreg(NAU8822_RIGHT_ADC_BOOST_CONTROL, 0x000);	// RLINEIN disconnected, RAUXIN disconnected
		//
	}
}


/* Параметры обработки звука с микрофона (эхо, эквалайзер, ...) */
static void nau8822_setprocparams(
	uint_fast8_t procenable,		/* включение обработки */
	const uint_fast8_t * gains		/* массив с параметрами */
	)
{
	//debug_printf_P(PSTR("codec: procenable=%d, gains={ %2d,%2d,%2d,%2d,%2d }\n"), procenable, gains [0], gains [1], gains [2], gains [3], gains [4]);
	//enum { wide = 0, freq = 1 }; // default settings
	enum { wide = 1, freq = 0 }; // default settings

	// Смысл значения freq
	//	freq	EQ1 (High Pass) EQ2 (Band Pass) EQ3 (Band Pass) EQ4 (Band Pass) EQ5 (Low Pass) 
	//	0 		80Hz 			230Hz 			650Hz 			1.8kHz 			5.3kHz 
	//	1 		105Hz 			300Hz 			850Hz 			2.4kHz 			6.9kHz 
	//	2 		135Hz 			385Hz 			1.1kHz 			3.2kHz 			9.0kHz 
	//	3 		175Hz 			500Hz 			1.4kHz 			4.1kHz 			11.7kHz 

	// Назначение работы эквалайзера в ртакте ADC делается битом 0x100 в ргистре NAU8822_EQ1
	if (procenable == 0)
	{
		// Выключено - все значения по умолчанию
		// digital gain control должно быть в диапазоне 0..24 (+12 db .. -12dB), 12 соответствует 0 dB
		enum { gain = 0 }; // default settings
		nau8822_setreg(NAU8822_EQ1, // low cutoff - 0x22C reset value
			1 * (1u << 8) |	// 1 = block operates on digital stream to DAC
			freq * (1u << 5) | // Equalizer band 1 low pass -3dB cut-off frequency selection
			(12 - gain) * (1u << 0) | // EQ Band 1 digital gain control.
			0);
		nau8822_setreg(NAU8822_EQ2, // peak 1 - 0x02C reset value
			wide * (1u << 8) |	// 0 = narrow band characteristic
			freq * (1u << 5) | // Equalizer Band 2 center frequency selection
			(12 - gain) * (1u << 0) | // EQ Band 2 digital gain control.
			0);
		nau8822_setreg(NAU8822_EQ3, // peak 2 - 0x02C reset value
			wide * (1u << 8) |	// 0 = narrow band characteristic
			freq * (1u << 5) | // Equalizer Band 3 center frequency selection
			(12 - gain) * (1u << 0) | // EQ Band 3 digital gain control.
			0);
		nau8822_setreg(NAU8822_EQ4, // peak 3 - 0x02C reset value
			wide * (1u << 8) |	// 0 = narrow band characteristic
			freq * (1u << 5) | // Equalizer Band 4 center frequency selection
			(12 - gain) * (1u << 0) | // EQ Band 4 digital gain control.
			0);
		nau8822_setreg(NAU8822_EQ5, // high curoff - 0x02C reset value
			freq * (1u << 5) | // Equalizer Band 5 high pass -3dB cut-off frequency selection
			(12 - gain) * (1u << 0) | // EQ Band 5 digital gain control.
			0);
	}
	else
	{
#if HARDWARE_CODEC1_NPROCPARAMS != 5
	#error Wrong value of HARDWARE_CODEC1_NPROCPARAMS
#endif
		// digital gain control должно быть в диапазоне 0..24 (+12 db .. -12dB), 12 соответствует 0 dB
		// Включено
		nau8822_setreg(NAU8822_EQ1, // low cutoff - 0x22C reset value
			0 * (1u << 8) |	// 0 = block operates on digital stream from ADC, 1 = block operates on digital stream to DAC
			freq * (1u << 5) | // Equalizer band 1 low pass -3dB cut-off frequency selection
			(24 - gains [0]) * (1u << 0) | // EQ Band 1 digital gain control.
			0);
		nau8822_setreg(NAU8822_EQ2, // peak 1 - 0x02C reset value
			wide * (1u << 8) |	// 0 = narrow band characteristic
			freq * (1u << 5) | // Equalizer Band 2 center frequency selection
			(24 - gains [1]) * (1u << 0) | // EQ Band 2 digital gain control.
			0);
		nau8822_setreg(NAU8822_EQ3, // peak 2 - 0x02C reset value
			wide * (1u << 8) |	// 0 = narrow band characteristic
			freq * (1u << 5) | // Equalizer Band 3 center frequency selection
			(24 - gains [2]) * (1u << 0) | // EQ Band 3 digital gain control.
			0);
		nau8822_setreg(NAU8822_EQ4, // peak 3 - 0x02C reset value
			wide * (1u << 8) |	// 0 = narrow band characteristic
			freq * (1u << 5) | // Equalizer Band 4 center frequency selection
			(24 - gains [3]) * (1u << 0) | // EQ Band 4 digital gain control.
			0);
		nau8822_setreg(NAU8822_EQ5, // high curoff - 0x02C reset value
			freq * (1u << 5) | // Equalizer Band 5 high pass -3dB cut-off frequency selection
			(24 - gains [4]) * (1u << 0) | // EQ Band 5 digital gain control.
			0);
	}
}

// возврат степени 2 от числа (не являющиеся 1 2 4 8... округляются до ближайшего меньшего).
static uint_fast8_t
nau8822_ilog2(
	unsigned long v		// число на анализ
	)
{
	uint_fast8_t n;
	ASSERT(v != 0);

	for (n = 0; v != 1; ++ n)
		v >>= 1;

	return n;
}

static void nau8822_initialize_fullduplex(void)
{
	//debug_printf_P(PSTR("nau8822_initialize_fullduplex start\n"));

#if CODEC_TYPE_NAU8822_MASTER
	const uint_fast8_t master = 1;	// кодек формирует I2S синхронизацию
#else /* CODEC_TYPE_NAU8822_MASTER */
	const uint_fast8_t master = 0;
#endif /* CODEC_TYPE_NAU8822_MASTER */
#if CODEC_TYPE_NAU8822_USE_8KS
	const unsigned long NAU8822_ADDITIONAL_CONTROL_SMPLR_val = 0x05uL * (1 << 2); // SMPLR=0x05 (8 kHz)
	const unsigned long NAU8822_CLOCKING_MCLKSEL_val = 0x05uL * (1 << 5);	// 0x05: divide by 6 MCLKSEL master clock prescaler
	const unsigned long ws = 8000;
#else /* CODEC_TYPE_NAU8822_USE_8KS */
	const unsigned long NAU8822_ADDITIONAL_CONTROL_SMPLR_val = 0x00uL * (1 << 2); // SMPLR=0x00 (48kHz)
	const unsigned long NAU8822_CLOCKING_MCLKSEL_val = 0x00uL * (1 << 5);	// Scaling of master clock source for internal 256fs rate divide by 1
	const unsigned long ws = 48000;
#endif /* CODEC_TYPE_NAU8822_USE_8KS */

	const unsigned long mclk = 12288000;
	const unsigned long framebits = WITHI2S_FRAMEBITS;
	const unsigned long bclk = ws * framebits;
	const unsigned divider = mclk / bclk;
	//debug_printf_P(PSTR("nau8822_initialize_fullduplex: mclk=%lu, bclk=%lu, divider=%lu, nau8822_ilog2=%u\n"), mclk, bclk, divider, nau8822_ilog2(divider));

	nau8822_setreg(NAU8822_RESET, 0x00);	// RESET

	const uint_fast8_t level = 0;	// До инициализации тишина
	// Установка уровня вывода на наушники
	nau8822_setreg(NAU8822_LOUT1_HP_CONTROL, level | 0);
	nau8822_setreg(NAU8822_ROUT1_HP_CONTROL, level | 0x100);

	// Установка уровня вывода на динамик
	nau8822_setreg(NAU8822_LOUT2_SPK_CONTROL, level | 0);
	nau8822_setreg(NAU8822_ROUT2_SPK_CONTROL, level | 0x100);

	nau8822_setreg(NAU8822_POWER_MANAGEMENT_1, 0x1cd); // was: 0x1cd - pll off, input to internal bias buffer in high-Z floating condition
	nau8822_setreg(NAU8822_POWER_MANAGEMENT_2, 0x1bf); // was: 0x1bf - right pga off - 0x1b7
	nau8822_setreg(NAU8822_POWER_MANAGEMENT_3, 0x1ef); // was: 0x1ff - reserved=0

#if WITHI2S_FORMATI2S_PHILIPS
	// I2S mode
	#if WITHI2S_FRAMEBITS == 64
		nau8822_setreg(NAU8822_AUDIO_INTERFACE, 0x010 | 0x060);	// reg 0x04, I2S, 32 bit
	#else /* WITHI2S_FRAMEBITS == 64 */
		nau8822_setreg(NAU8822_AUDIO_INTERFACE, 0x010 | 0x000);	// reg 0x04, I2S, 16 bit
	#endif /* WITHI2S_FRAMEBITS == 64 */

#else /* WITHI2S_FORMATI2S_PHILIPS */
	// LJ mode
	#if WITHI2S_FRAMEBITS == 64
		nau8822_setreg(NAU8822_AUDIO_INTERFACE, 0x008 | 0x060);	// reg 0x04, LJ, 32 bit
	#else /* WITHI2S_FRAMEBITS == 64 */
		nau8822_setreg(NAU8822_AUDIO_INTERFACE, 0x008 | 0x000);	// reg 0x04, LJ, 16 bit
	#endif /* WITHI2S_FRAMEBITS == 64 */

#endif /* WITHI2S_FORMATI2S_PHILIPS */

	//nau8822_setreg(NAU8822_COMPANDING_CONTROL, 0x000);	// reg 0x05 = 0 reset state

	nau8822_setreg(NAU8822_ADDITIONAL_CONTROL, 	// reg 0x07,
		NAU8822_ADDITIONAL_CONTROL_SMPLR_val |			// SMPLR=0x05 (8 kHz)
		0);

	nau8822_setreg(NAU8822_CLOCKING,	// reg 0x06
		NAU8822_CLOCKING_MCLKSEL_val |	// Scaling of master clock source for internal 256fs rate divide by 1
		nau8822_ilog2(divider) * (0x01uL << 2) |	// BCLKSEL: Scaling of output frequency at BCLK pin#8 when chip is in master mode
		master * (0x01uL << 0) |	// 1 = FS and BCLK are driven as outputs by internally generated clocks
		0);

	// Установка параметров умножителя за ЦАП не требуется - всегда максимальный уровень.
	//nau8822_setreg(NAU8822_LEFT_DAC_DIGITAL_VOLUME, 255 | 0);
	//nau8822_setreg(NAU8822_RIGHT_DAC_DIGITAL_VOLUME, 255 | 0x100);
//{0xb , 0x1ff},
//{0xc , 0x1ff},

	//[AA_AUXIN_HP/Audio Control] 
	nau8822_setreg(NAU8822_DAC_CONTROL, 0x008);	// was: 0x00c - removed automute
	nau8822_setreg(NAU8822_ADC_CONTROL, 0x108);
//Noise gate
//{0x23,0x18);

//[AA_AUXIN_HP/Input Output Mixer] 
	nau8822_setreg(NAU8822_RIGHT_SPK_SUBMIXER, 0x10);	// use RMIX as BTL channel

	//nau8822_setreg(NAU8822_OUTPUT_CONTROL, 0x063 | 0x01c); // AUXOUT1, AUXOUT2, LSPKOUT and RSPKOUT x1.5 gain
	nau8822_setreg(NAU8822_OUTPUT_CONTROL, 0x01e); // AUXOUT1, AUXOUT2, LSPKOUT and RSPKOUT x1.5 gain

	//nau8822_setreg(NAU8822_AUX2_MIXER_CONTROL, 0x040);	// 0x40 - AUX2 muted, 0x01 (default) - connected to LEFT DAC
	//nau8822_setreg(NAU8822_AUX1_MIXER_CONTROL, 0x040);	// 0x40 - AUX1 muted, 0x01 (default) - connected to RIGHT DAC

	nau8822_setreg(NAU8822_LEFT_MIXER_CONTROL, 0x001);
	nau8822_setreg(NAU8822_RIGHT_MIXER_CONTROL, 0x001);

	// Микрофон подключен к LMICN, LMICP=common
	nau8822_setreg(NAU8822_INPUT_CONTROL, 0x003);

	// Установка чувствительность АЦП не требуется - стоит максимальная после сброса
	// но на всякий слуяай для понятности програмируем.
	const uint_fast8_t mklevel = 255;
	nau8822_setreg(NAU8822_LEFT_ADC_DIGITAL_VOLUME, mklevel | 0);
	nau8822_setreg(NAU8822_RIGHT_ADC_DIGITAL_VOLUME, mklevel | 0x100);

	nau8822_setreg(NAU8822_DAC_DITHER, 0x000);	// dither off

	//debug_printf_P(PSTR("nau8822_initialize_fullduplex done\n"));
}

static void nau8822_stop(void)
{
#if CODEC_TYPE_NAU8822_MASTER
	// после RESET кодек при подаче MCLK формирует WS и BCLK... конфликт с выходами FPGA, если тактирование от неё.
	nau8822_setreg(NAU8822_RESET, 0x00);	// RESET
#endif /* CODEC_TYPE_NAU8822_MASTER */
}

const codec1if_t *
board_getaudiocodecif(void)
{

	static const char codecname [] = "NAU8822";

	/* Интерфейс цправления кодеком */
	static const codec1if_t ifc =
	{
		nau8822_stop,
		nau8822_initialize_fullduplex,
		nau8822_setvolume,		/* Установка громкости на наушники */
		nau8822_lineinput,		/* Выбор LINE IN как источника для АЦП вместо микрофона */
		nau8822_setprocparams,	/* Параметры обработки звука с микрофона (эхо, эквалайзер, ...) */
		codecname				/* Название кодека (всегда последний элемент в структуре) */
	};

	return & ifc;
}

#endif /* (CODEC1_TYPE == CODEC_TYPE_NAU8822L) */
