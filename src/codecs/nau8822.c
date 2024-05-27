/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"


//#include "./formats.h"

#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_NAU8822L)
//
// Управление кодеком NOVOTON NAU8822L
// https://www.nuvoton.com/products/smart-home-audio/audio-converters/audio-codec-series/nau8822lyg/

#include "spi.h"
#include "board.h"
#include "formats.h"	// for debug prints
#include "audio.h"
#include "nau8822.h"

// Clock period, SCLK no less then 80 nS (частота не выше 12.5 МГц)
#define NAU8822_SPIMODE			SPIC_MODE3
#define NAU8822_SPISPEED 		SPIC_SPEED10M
#define NAU8822_ADDRESS_W		0x34	// I2C address: 0x34

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
void nau8822_setreg(
	uint_fast8_t regv,			/* 7 bit register address */
	uint_fast16_t datav			/* 9 bit value */
	)
{
	const uint_fast16_t fulldata = regv * 512 + (datav & 0x1ff);

#if CODEC_TYPE_NAU8822_USE_SPI
	// кодек управляется по SPI
	const spitarget_t target = targetcodec1;	/* addressing to chip */

	#if 1//WITHSPILOWSUPPORTT
		// Работа совместно с фоновым обменом SPI по прерываниям
		uint8_t txbuf [2];

		USBD_poke_u16_BE(txbuf, fulldata);
		prog_spi_io(target, NAU8822_SPISPEED, NAU8822_SPIMODE, txbuf, ARRAY_SIZE(txbuf), NULL, 0, NULL, 0);

	#elif WITHSPIEXT16

		hardware_spi_connect_b16(NAU8822_SPISPEED, NAU8822_SPIMODE);
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

	#if WITHTWIHW
		uint8_t buff [] = { fulldata >> 8, fulldata >> 0, };
		i2chw_write(NAU8822_ADDRESS_W, buff, ARRAY_SIZE(buff));
	#elif WITHTWISW
		// кодек управляется по I2C
		i2c_start(NAU8822_ADDRESS_W);
		i2c_write(fulldata >> 8);
		i2c_write(fulldata >> 0);
		i2c_waitsend();
		i2c_stop();
	#endif

#endif /* CODEC_TYPE_NAU8822_USE_SPI */
}

#ifndef NAU8822_INPUT_CONTROL_VAL
	// Микрофон подключен к LMICN, LMICP=common
	// LLIN отключен от PGA
	// 0x02 = LMICN connected to PGA negative input
	// 0x01 = LMICP connected to PGA positive input
	#define NAU8822_INPUT_CONTROL_VAL 0x03
	//#define NAU8822_INPUT_CONTROL_VAL 0x01
#endif

static void nau8822_input_config(void)
{
	nau8822_setreg(NAU8822_INPUT_CONTROL, NAU8822_INPUT_CONTROL_VAL);
	//nau8822_setreg(NAU8822_INPUT_CONTROL, NAU8822_INPUT_CONTROL_VAL);

}

/* Установка громкости на наушники */
static void nau8822_setvolume(uint_fast16_t gain, uint_fast8_t mute, uint_fast8_t mutespk)
{
	//PRINTF("nau8822_setvolume: gain=%d, mute=%d, mutespk=%d\n", (int) gain, (int) mute, (int) mutespk);
	uint_fast16_t vmutehp = 0;
	uint_fast16_t vmutespk = 0;
	uint_fast16_t vmuteaux12 = 0x01;	// default value
	// 0x3F: +6 dB
	// 0x39: 0 dB: 2.7..2.8 volt p-p at each SPK output
	uint_fast16_t levelhp = (gain - BOARD_AFGAIN_MIN) * (NAU8822L_OUT_HP_MAX - NAU8822L_OUT_HP_MIN) / (BOARD_AFGAIN_MAX - BOARD_AFGAIN_MIN) + NAU8822L_OUT_HP_MIN;
	uint_fast16_t levelspk = (gain - BOARD_AFGAIN_MIN) * (NAU8822L_OUT_SPK_MAX - NAU8822L_OUT_SPK_MIN) / (BOARD_AFGAIN_MAX - BOARD_AFGAIN_MIN) + NAU8822L_OUT_SPK_MIN;
	if (mute)
	{
		vmutehp = 0x40;
		vmutespk = 0x40;
	}
	if (mutespk)
	{
		vmutespk = 0x40;
		vmuteaux12 = 0x40;
	}
	//debug_printf_P(PSTR("nau8822_setvolume: level=%02x start\n"), level);

	// Установка уровня вывода на наушники
	nau8822_setreg(NAU8822_LOUT1_HP_CONTROL, vmutehp | (levelhp & 0x3F) | 0);
	nau8822_setreg(NAU8822_ROUT1_HP_CONTROL, vmutehp | (levelhp & 0x3F) | 0x100);

	// Установка уровня вывода на динамик
	nau8822_setreg(NAU8822_LOUT2_SPK_CONTROL, vmutespk | (levelspk & 0x3F) | 0);
	nau8822_setreg(NAU8822_ROUT2_SPK_CONTROL, vmutespk | (levelspk & 0x3F) | 0x100);

	nau8822_setreg(NAU8822_AUX2_MIXER_CONTROL, vmuteaux12);		// aux2
	nau8822_setreg(NAU8822_AUX1_MIXER_CONTROL, vmuteaux12);		// aux1
}

/* Выбор LINE IN как источника для АЦП вместо микрофона */
static void nau8822_lineinput(uint_fast8_t linein, uint_fast8_t mikeboost20db, uint_fast16_t mikegain, uint_fast16_t linegain)
{
	//PRINTF("nau8822_lineinput: linein=%d, mikeboost20db=%d, mikegain=%d, linegain=%d\n", (int) linein, (int) mikeboost20db, (int) mikegain, (int) linegain);
	// Микрофон подключен к LMICP, LMICN=common
	// Входы RMICN, RMICP никуда не подключены
	// Line input подключены к LAUXIN, RAUXIN
//{0x0f, 0x1ff},
//{0x10, 0x1ff},
	if (linein != 0)
	{
		// переключение на линейный вход
		// Line input подключены к LAUXIN, RAUXIN
		const uint_fast8_t inppgagain = 0x40;	// PGA muted
		nau8822_setreg(NAU8822_LEFT_INP_PGA_GAIN, inppgagain | 0);
		nau8822_setreg(NAU8822_RIGHT_INP_PGA_GAIN, inppgagain | 0x100);	// write both valuse simultaneously
		//
		// 1..7: -12..+6 dB, 5: 0 dB, 0: off
		const uint_fast8_t adcboostcontrol = (linegain - WITHLINEINGAINMIN) * (0x07) / (WITHLINEINGAINMAX - WITHLINEINGAINMIN) + 0x00;
		nau8822_setreg(NAU8822_LEFT_ADC_BOOST_CONTROL, adcboostcontrol);	// LLINEIN disconnected, LAUXIN connected w/o gain
		nau8822_setreg(NAU8822_RIGHT_ADC_BOOST_CONTROL, adcboostcontrol);	// RLINEIN disconnected, RAUXIN connected w/o gain
		nau8822_input_config();
	}
	else
	{
		// переключение на микрофон
		// Микрофон подключен к LMICP, LMICN=common
		//
		const uint_fast8_t inppgagain = (mikegain - WITHMIKEINGAINMIN) * (0x3F) / (WITHMIKEINGAINMAX - WITHMIKEINGAINMIN) + 0x00;
		nau8822_setreg(NAU8822_LEFT_INP_PGA_GAIN, inppgagain | 0);
		nau8822_setreg(NAU8822_RIGHT_INP_PGA_GAIN, inppgagain | 0x100);	// Write both valuse simultaneously
		// 
		nau8822_setreg(NAU8822_LEFT_ADC_BOOST_CONTROL, 0x000 | 0x100 * (mikeboost20db != 0));	// 0x100 - 20 dB boost ON
		nau8822_setreg(NAU8822_RIGHT_ADC_BOOST_CONTROL, 0x000);	// RLINEIN disconnected, RAUXIN disconnected
		nau8822_input_config();
	}
}

//
// 01100 = 0.0dB default unity gain value
// 00000 = +12dB
// 00001 = +11dB
// 00010 = +10dB
// 00011 = +9dB
// 00100 = +8dB
// 00101 = +7dB
// 00110 = +6dB
// 00111 = +5dB
// 01000 = +4dB
// 01001 = +3dB
// 01010 = +2dB
// 01010 = +1dB
// 01100 = +0dB

// 11000 = -12dB
// 11001 and larger values are reserved
static uint_fast8_t getbandgain(const uint_fast8_t * p, uint_fast8_t procenable)
{
#if 0
	static const uint8_t gains [25] =
	{
			0x18,	// -12dB
			0x17,	// -11dB
			0x16,	// -10dB
			0x15,	// -9dB
			0x14,	// -8dB
			0x13,	// -7dB
			0x12,	// -6dB
			0x11,	// -5dB
			0x10,	// -4dB
			0x0F,	// -3dB
			0x0E,	// -2dB
			0x0D,	// -1dB
			0x0C,	// +0dB
			0x0B,	// +1dB
			0x0A,	// +2dB
			0x09,	// +3dB
			0x08,	// +4dB
			0x07,	// +5dB
			0x06,	// +6dB
			0x05,	// +7dB
			0x04,	// +8dB
			0x03,	// +9dB
			0x02,	// +10dB
			0x01,	// +11dB
			0x00,	// +12dB
	};
	return procenable ? gains [p [0]] : 12;
#else
	ASSERT(p [0] <= 24);
	return procenable ? 24 - p [0] : 12;
#endif
}

/* Параметры обработки звука с микрофона (эхо, эквалайзер, ...) */
static void nau8822_setprocparams(
	uint_fast8_t procenable,		/* включение обработки */
	const uint_fast8_t * gains		/* массив с параметрами */
	)
{
	//debug_printf_P(PSTR("codec: procenable=%d, gains={ %2d,%2d,%2d,%2d,%2d }\n"), procenable, gains [0], gains [1], gains [2], gains [3], gains [4]);
	//enum { wide = 0, freq = 1 }; // default settings
	enum { wide = 1, freq = 0 };

	// Смысл значения freq
	//	freq	EQ1 (High Pass) EQ2 (Band Pass) EQ3 (Band Pass) EQ4 (Band Pass) EQ5 (Low Pass) 
	//	0 		80Hz 			230Hz 			650Hz 			1.8kHz 			5.3kHz 
	//	1 		105Hz 			300Hz 			850Hz 			2.4kHz 			6.9kHz 
	//	2 		135Hz 			385Hz 			1.1kHz 			3.2kHz 			9.0kHz 
	//	3 		175Hz 			500Hz 			1.4kHz 			4.1kHz 			11.7kHz 

	// Назначение работы эквалайзера в ртакте ADC делается битом 0x100 в ргистре NAU8822_EQ1

	// Выключено - все значения по умолчанию
	// digital gain control должно быть в диапазоне 0..24 (+12 db .. -12dB), 12 соответствует 0 dB
	nau8822_setreg(NAU8822_EQ1, // low cutoff - 0x22C reset value
		0 * (UINT16_C(1) << 8) |	// 0 = block operates on digital stream from ADC
		freq * (UINT16_C(1) << 5) | // EQ1CF Equalizer band 1 low pass -3dB cut-off frequency selection
		getbandgain(gains + 0, procenable) * (UINT16_C(1) << 0) | // EQ Band 1 digital gain control.
		0);
	nau8822_setreg(NAU8822_EQ2, // peak 1 - 0x02C reset value
		wide * (UINT16_C(1) << 8) |	// EQ2BW 0 = narrow band characteristic
		freq * (UINT16_C(1) << 5) | // EQ2CF Equalizer Band 2 center frequency selection
		getbandgain(gains + 1, procenable) * (UINT16_C(1) << 0) | // EQ Band 2 digital gain control.
		0);
	nau8822_setreg(NAU8822_EQ3, // peak 2 - 0x02C reset value
		wide * (UINT16_C(1) << 8) |	// EQ3BW 0 = narrow band characteristic
		freq * (UINT16_C(1) << 5) | // EQ3CF Equalizer Band 3 center frequency selection
		getbandgain(gains + 2, procenable) * (UINT16_C(1) << 0) | // EQ Band 3 digital gain control.
		0);
	nau8822_setreg(NAU8822_EQ4, // peak 3 - 0x02C reset value
		wide * (UINT16_C(1) << 8) |	// EQ4BW 0 = narrow band characteristic
		freq * (UINT16_C(1) << 5) | // EQ4CF Equalizer Band 4 center frequency selection
		getbandgain(gains + 3, procenable) * (UINT16_C(1) << 0) | // EQ Band 4 digital gain control.
		0);
	nau8822_setreg(NAU8822_EQ5, // high curoff - 0x02C reset value
		freq * (UINT16_C(1) << 5) | // EQ5CF Equalizer Band 5 low pass -3dB cut-off frequency selection
		getbandgain(gains + 4, procenable) * (UINT16_C(1) << 0) | // EQ Band 5 digital gain control.
		0);
}

// возврат степени 2 от числа (не являющиеся 1 2 4 8... округляются до ближайшего меньшего).
static uint_fast8_t
nau8822_ilog2(
	uint_fast32_t v		// число на анализ
	)
{
	uint_fast8_t n;
	ASSERT(v != 0);

	for (n = 0; v != 1; ++ n)
		v >>= 1;

	return n;
}

static void nau8822_pll(
	unsigned div2,
	uint_fast32_t N
	)
{
	nau8822_setreg(NAU8822_PLL_N,
		(!! div2 << 4) | // 0 - mclk divide by 1
		((0x0F & (N >> 24)) << 0) |	// integer portion of N
		0);

	nau8822_setreg(NAU8822_PLL_K1, 0x03F & (N >> 18));
	nau8822_setreg(NAU8822_PLL_K2, 0x01FF & (N >> 9));
	nau8822_setreg(NAU8822_PLL_K3, 0x01FF & (N >> 0));
}

static void nau8822_initialize_fullduplex(void (* io_control)(uint_fast8_t on), uint_fast8_t master)
{
	//debug_printf_P(PSTR("nau8822_initialize_fullduplex start\n"));
	uint_fast16_t NAU8822_AUDIO_INTERFACE_WLEN_val;
	uint_fast16_t NAU8822_MISC_8B_val;	// When in 8-bit mode, the Register 4 word length control (WLEN) is ignored.
	switch (WITHADAPTERCODEC1WIDTH)
	{
	default:
	case 32: NAU8822_AUDIO_INTERFACE_WLEN_val = 0x060; NAU8822_MISC_8B_val = 0x00; break;
	case 24: NAU8822_AUDIO_INTERFACE_WLEN_val = 0x040; NAU8822_MISC_8B_val = 0x00; break;
	case 20: NAU8822_AUDIO_INTERFACE_WLEN_val = 0x020; NAU8822_MISC_8B_val = 0x00; break;
	case 16: NAU8822_AUDIO_INTERFACE_WLEN_val = 0x000; NAU8822_MISC_8B_val = 0x00; break;
	case 8: NAU8822_AUDIO_INTERFACE_WLEN_val = 0x00; NAU8822_MISC_8B_val = 0x040; break;
	}

#if CODEC_TYPE_NAU8822_USE_8KS
	const uint_fast16_t NAU8822_ADDITIONAL_CONTROL_SMPLR_val = 0x05u * (UINT16_C(1) << 2); // SMPLR=0x05 (8 kHz)
	const uint_fast16_t NAU8822_CLOCKING_MCLKSEL_val = 0x05u * (UINT16_C(1) << 5);	// 0x05: divide by 6 MCLKSEL master clock prescaler
	const uint_fast32_t ws = 8000;
#else /* CODEC_TYPE_NAU8822_USE_8KS */
	const uint_fast16_t NAU8822_ADDITIONAL_CONTROL_SMPLR_val = 0x00u * (UINT16_C(1) << 2); // SMPLR=0x00 (48kHz)
	const uint_fast16_t NAU8822_CLOCKING_MCLKSEL_val = 0x00u * (UINT16_C(1) << 5);	// Scaling of master clock source for internal 256fs rate divide by 1
	const uint_fast16_t NAU8822_CLOCKING_MCLKSEL_PLL_val = (UINT16_C(1) << 8) | 2 * (UINT16_C(1) << 5);	// PLL Scaling of master clock source for internal 256fs rate divide by 1
	const uint_fast32_t ws = 48000;
#endif /* CODEC_TYPE_NAU8822_USE_8KS */

	const uint_fast32_t mclk = 12288000;
	const uint_fast32_t framebits = CODEC1_FRAMEBITS;
	const uint_fast32_t bclk = ws * framebits;
	const unsigned divider = mclk / bclk;
	//debug_printf_P(PSTR("nau8822_initialize_fullduplex: mclk=%lu, bclk=%lu, divider=%lu, nau8822_ilog2=%u\n"), mclk, bclk, divider, nau8822_ilog2(divider));

	const uint_fast32_t imclk = 256 * ws;

	io_control(0);

	nau8822_setreg(NAU8822_RESET, 0x00);	// RESET
	nau8822_setreg(11, 0xFF);	// RESET off (write value ignored)
	nau8822_setreg(11, 0x00);	// RESET off (write value ignored)

	nau8822_pll(0, 8u << 24);

	// R1 Bit 8, DCBUFEN, set to logic = 1 if setting up for greater than 3.60V operation
	nau8822_setreg(NAU8822_POWER_MANAGEMENT_1, 0x1cd); // was: 0x1cd - pll off, input to internal bias buffer in high-Z floating condition
	nau8822_setreg(NAU8822_POWER_MANAGEMENT_2, 0x1bf); // was: 0x1bf - right pga off - 0x1b7
	nau8822_setreg(NAU8822_POWER_MANAGEMENT_3, 0x1ef); // was: 0x1ff - reserved=0

//	nau8822_setreg(NAU8822_POWER_MANAGEMENT_1, 0x1FF); // was: 0x1cd - pll off, input to internal bias buffer in high-Z floating condition
//	nau8822_setreg(NAU8822_POWER_MANAGEMENT_2, 0x1FF); // was: 0x1bf - right pga off - 0x1b7
//	nau8822_setreg(NAU8822_POWER_MANAGEMENT_3, 0x1FF); // was: 0x1ff - reserved=0

	nau8822_setreg(NAU8822_CLOCKING,	// reg 0x06
		//NAU8822_CLOCKING_MCLKSEL_PLL_val |	// Scaling of master clock source for internal 256fs rate divide by 1
		NAU8822_CLOCKING_MCLKSEL_val |	// Scaling of master clock source for internal 256fs rate divide by 1
		nau8822_ilog2(divider) * (UINT16_C(1) << 2) |	// BCLKSEL: Scaling of output frequency at BCLK pin#8 when chip is in master mode
		master * (UINT16_C(1) << 0) |	// 1 = FS and BCLK are driven as outputs by internally generated clocks
		0);

#if CODEC1_FORMATI2S_PHILIPS
	// I2S mode
	// When in 8-bit mode, the Register 4 word length control (WLEN) is ignored.
	nau8822_setreg(NAU8822_AUDIO_INTERFACE, 0x010 | NAU8822_AUDIO_INTERFACE_WLEN_val);	// reg 0x04, I2S

#else /* CODEC1_FORMATI2S_PHILIPS */
	// LJ mode
	// When in 8-bit mode, the Register 4 word length control (WLEN) is ignored.
	nau8822_setreg(NAU8822_AUDIO_INTERFACE, 0x008 | NAU8822_AUDIO_INTERFACE_WLEN_val);	// reg 0x04, LJ

#endif /* CODEC1_FORMATI2S_PHILIPS */

	nau8822_setreg(NAU8822_COMPANDING_CONTROL, 0x000);	// reg 0x05 = 0 reset state

	nau8822_setreg(NAU8822_MISC, 	// reg 0x3C,
		0x20 |				// ADCOUT output driver enable control
		NAU8822_MISC_8B_val |			// 8-bit word length enable
		0);

	nau8822_setreg(NAU8822_ADDITIONAL_CONTROL, 	// reg 0x07,
		NAU8822_ADDITIONAL_CONTROL_SMPLR_val |			// SMPLR=0x05 (8 kHz)
		0);

	io_control(1);

	const uint_fast8_t level = 0;	// До инициализации тишина
	// Установка уровня вывода на наушники
	nau8822_setreg(NAU8822_LOUT1_HP_CONTROL, level | 0);
	nau8822_setreg(NAU8822_ROUT1_HP_CONTROL, level | 0x100);

	// Установка уровня вывода на динамик
	nau8822_setreg(NAU8822_LOUT2_SPK_CONTROL, level | 0);
	nau8822_setreg(NAU8822_ROUT2_SPK_CONTROL, level | 0x100);

	// Установка параметров умножителя за ЦАП не требуется - всегда максимальный уровень.
	nau8822_setreg(NAU8822_LEFT_DAC_DIGITAL_VOLUME, 255 | 0);
	nau8822_setreg(NAU8822_RIGHT_DAC_DIGITAL_VOLUME, 255 | 0x100);

	nau8822_setreg(NAU8822_DAC_DITHER, 0x000);	// dither off
	nau8822_setreg(NAU8822_DAC_CONTROL, 0x008);	// was: 0x00c - removed automute

	#if CODEC_TYPE_NAU8822_NO_BTL
		// Выходы SPK кодека не используются как мостовой выход, а идут к следующему каскаду усиления (отключаем инверсию правого канала)
		nau8822_setreg(NAU8822_RIGHT_SPK_SUBMIXER, 0x00);
	#else /* CODEC_TYPE_NAU8822_NO_BTL */
		nau8822_setreg(NAU8822_RIGHT_SPK_SUBMIXER, 0x10);	// RSUBBYP: use RMIX as BTL channel
	#endif /* CODEC_TYPE_NAU8822_NO_BTL */

//{0xb , 0x1ff},
//{0xc , 0x1ff},

	//[AA_AUXIN_HP/Audio Control] 
//Noise gate
//{0x23,0x18);

//[AA_AUXIN_HP/Input Output Mixer] 

	//nau8822_setreg(NAU8822_OUTPUT_CONTROL, 0x063 | 0x01c); // AUXOUT1, AUXOUT2, LSPKOUT and RSPKOUT x1.5 gain
	//nau8822_setreg(NAU8822_OUTPUT_CONTROL, 0x01e); // AUXOUT1, AUXOUT2, LSPKOUT and RSPKOUT x1.5 gain
	nau8822_setreg(NAU8822_OUTPUT_CONTROL,
			(UINT16_C(1) << 4) |	// AUX1BST
			(UINT16_C(1) << 3) |	// AUX2BST
			(UINT16_C(1) << 2) |	// SPKBST: LSPKOUT and RSPKOUT speaker amplifier gain boost control
			(UINT16_C(1) << 1) |	// TSEN
			0
			); // AUXOUT1, AUXOUT2, LSPKOUT and RSPKOUT x1.5 gain

	nau8822_setreg(NAU8822_AUX2_MIXER_CONTROL, 0x001);	// 0x40 - AUX2 muted, 0x01 (default) - connected to LEFT DAC
	nau8822_setreg(NAU8822_AUX1_MIXER_CONTROL, 0x001);	// 0x40 - AUX1 muted, 0x01 (default) - connected to RIGHT DAC

	nau8822_setreg(NAU8822_LEFT_MIXER_CONTROL, 0x001);
	nau8822_setreg(NAU8822_RIGHT_MIXER_CONTROL, 0x001);

	nau8822_input_config();

	// Установка чувствительность АЦП не требуется - стоит максимальная после сброса
	// но на всякий случай для понятности програмируем.
	const uint_fast8_t adcdigvol = 255;
	nau8822_setreg(NAU8822_LEFT_ADC_DIGITAL_VOLUME, adcdigvol | 0);
	nau8822_setreg(NAU8822_RIGHT_ADC_DIGITAL_VOLUME, adcdigvol | 0x100);

	nau8822_setreg(NAU8822_ADC_CONTROL, 0x108);	// HP filter enable, 128x oversampling for better SNR

	//debug_printf_P(PSTR("nau8822_initialize_fullduplex done\n"));
}

static void nau8822_stop(void)
{
	nau8822_setreg(NAU8822_RESET, 0x00);	// RESET
	nau8822_setreg(NAU8822_GPIO_CONTROL, 0x08);	// RESET off (write value ignored)
	nau8822_setreg(NAU8822_GPIO_CONTROL, 0x00);	// RESET off (write value ignored)
}

/* требуется ли подача тактирования для инициадизации кодека */
static uint_fast8_t nau8822_clocksneed(void)
{
	return 1;
}

const codec1if_t *
board_getaudiocodecif(void)
{

	static const char codecname [] = "NAU8822";

	/* Интерфейс управления кодеком */
	static const codec1if_t ifc =
	{
		nau8822_clocksneed,
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
