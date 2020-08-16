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

#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_WM8994)
//
// Управление кодеком CIRRUS LOGIC WM8994ECS/R
// https://raw.githubusercontent.com/jaz303/stm32f746g-discovery-gcc/master/lib/Drivers/BSP/Components/wm8994/wm8994.c
//
#include "audio.h"
#include "wm8994.h"

// Clock period, SCLK no less then 80 nS (не выше 12.5 МГц)
#define WM8994_SPIMODE		SPIC_MODE3
#define WM8994_ADDRESS_W	0x34	// I2C address (на платеSTM32F746G-DISCO): 0x34

// Условие использования оптимизированных функций обращения к SPI
#define WITHSPIEXT16 (WITHSPIHW && WITHSPI16BIT)

/* data is
*   D15..D9 WM8994 register offset
*   D8...D0 register data
*/

// The WM8994L can function only as a slave device when in the 2-wire interface configuration

// SPI 3-Wire Write Operation
/*
 Whenever the MODE pin on the WM8994L is in the logic HIGH condition,
 the device control interface will operate in the 3-Wire Write mode. 
 This is a write-only mode that does not require the fourth wire normally 
 used to read data from a device on an SPI bus implementation. 
 This mode is a 16-bit transaction consisting of a 7-bit Control Register Address, 
 and 9-bits of control register data.
*/
static void wm8994_setreg(
	uint_fast16_t regv,			/* 15-bit control register address */
	uint_fast16_t datav			/* 16-bit control register data */
	)
{
#if CODEC_TYPE_WM8994_USE_SPI
	// кодек управляется по SPI
	const spitarget_t target = targetcodec1;	/* addressing to chip */

	#if WITHSPIEXT16

		hardware_spi_connect_b16(SPIC_SPEEDFAST, WM8994_SPIMODE);
		prog_select(target);	/* start sending data to target chip */
		hardware_spi_b16_p1(regv & 0x7FFF);	// b15==0: write to register
		hardware_spi_b16_p2(datav);
		hardware_spi_complete_b16();
		prog_unselect(target);
		hardware_spi_disconnect();

	#else /* WITHSPIEXT16 */

		spi_select(target, WM8994_SPIMODE);
		spi_progval8_p1(target, (regv >> 8) & 0x7F);	// b15==0: write to register
		spi_progval8_p2(target, regv >> 0);
		spi_progval8_p2(target, datav >> 8);
		spi_progval8_p2(target, datav >> 0);
		spi_complete(target);
		spi_unselect(target);

	#endif /* WITHSPIEXT16 */

#else /* CODEC_TYPE_WM8994_USE_SPI */

	// кодек управляется по I2C
	i2c_start(WM8994_ADDRESS_W);
	i2c_write(regv >> 8);
	i2c_write(regv >> 0);
	i2c_write(datav >> 8);
	i2c_write(datav >> 0);
	i2c_waitsend();
	i2c_stop();

#endif /* CODEC_TYPE_WM8994_USE_SPI */
}

/* Установка громкости на наушники */
static void wm8994_setvolume(uint_fast16_t gain, uint_fast8_t mute, uint_fast8_t mutespk)
{
	// 0x3F: +6 dB
	// 0x39: 0 dB: 2.7..2.8 volt p-p at each SPK output
	// 0x00: -57 dB
	const uint_fast8_t levelhp = (gain - BOARD_AFGAIN_MIN) * (WM8994L_OUT_HP_MAX - WM8994L_OUT_HP_MIN) / (BOARD_AFGAIN_MAX - BOARD_AFGAIN_MIN) + WM8994L_OUT_HP_MIN;
	const uint_fast8_t levelspk = (gain - BOARD_AFGAIN_MIN) * (WM8994L_OUT_SPK_MAX - WM8994L_OUT_SPK_MIN) / (BOARD_AFGAIN_MAX - BOARD_AFGAIN_MIN) + WM8994L_OUT_SPK_MIN;

	const uint_fast16_t mutehpmask = mute || (gain == BOARD_AFGAIN_MIN) ? 0x000 : 0x040;
	const uint_fast16_t mutespkmask = mute || mutespk || (gain == BOARD_AFGAIN_MIN) ? 0x000 : 0x040;

	// Установка уровня вывода на наушники
	/* Left Headphone Volume */
	wm8994_setreg(WM8994_LEFT_OUTPUT_VOLUME, levelhp | 0x000 | mutehpmask);	// pending update

	/* Right Headphone Volume */
	wm8994_setreg(WM8994_RIGHT_OUTPUT_VOLUME, levelhp | 0x100 | mutehpmask);

	// Установка уровня вывода на динамики
	/* Left Speaker Volume */
	wm8994_setreg(WM8994_SPEAKER_VOLUME_LEFT, levelspk | 0x000 | mutespkmask);	// pending update

	/* Right Speaker Volume */
	wm8994_setreg(WM8994_SPEAKER_VOLUME_RIGHT, levelspk | 0x100 | mutespkmask);
}

/* Выбор LINE IN как источника для АЦП вместо микрофона */
static void wm8994_lineinput(uint_fast8_t linein, uint_fast8_t mikebust20db, uint_fast16_t mikegain, uint_fast16_t linegain)
{
	// PGA codes:
	// 0x3F: +6 dB
	// 0x39: 0 dB
	// 0x00: -57 dB
	//const uint_fast8_t auxinpgaval = (linegain - WITHLINEINGAINMIN) * (0x07 - 0x01) / (WITHLINEINGAINMAX - WITHLINEINGAINMIN) + 0x01;
	 /* Input volume */
	if (linein != 0)
	{
		// переключение на линейный вход
	}
	else
	{
		// переключение на микрофон
		const uint_fast8_t mikepgaval = (mikegain - WITHMIKEINGAINMIN) * (0x3f) / (WITHMIKEINGAINMAX - WITHMIKEINGAINMIN) + 0x00;
		/* Left AIF1 ADC1 volume */
		wm8994_setreg(WM8994_AIF1_ADC1_LEFT_VOLUME, mikepgaval | 0x000);	// pending gain changes

		/* Right AIF1 ADC1 volume */
		wm8994_setreg(WM8994_AIF1_ADC1_RIGHT_VOLUME, mikepgaval | 0x100);

		/* Left AIF1 ADC2 volume */
		wm8994_setreg(WM8994_AIF1_ADC2_LEFT_VOLUME, mikepgaval | 0x000);	// pending gain changes

		/* Right AIF1 ADC2 volume */
		wm8994_setreg(WM8994_AIF1_ADC2_RIGHT_VOLUME, mikepgaval | 0x100);
	}

#if 0
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
		wm8994_setreg(WM8994_LEFT_INP_PGA_GAIN, 0x40 | 0);	// PGA muted
		wm8994_setreg(WM8994_RIGHT_INP_PGA_GAIN, 0x40 | 0x100);	// write both valuse simultaneously
		//
		wm8994_setreg(WM8994_LEFT_ADC_BOOST_CONTROL, auxinpgaval);	// LLINEIN disconnected, LAUXIN connected w/o gain
		wm8994_setreg(WM8994_RIGHT_ADC_BOOST_CONTROL, auxinpgaval);	// RLINEIN disconnected, RAUXIN connected w/o gain
		//
	}
	else
	{
		// переключение на микрофон
		// Микрофон подключен к LMICN, LMICP=common
		//const uint_fast8_t mikepgaval = 0x10;	// 0x10 - default, 0x00..0x3f mean -12 db..+35.25 dB in 0.75 dB step
		const uint_fast8_t mikepgaval = (mikegain - WITHMIKEINGAINMIN) * (0x3f) / (WITHMIKEINGAINMAX - WITHMIKEINGAINMIN) + 0x00;
		//
		wm8994_setreg(WM8994_LEFT_INP_PGA_GAIN, mikepgaval | 0);	// PGA volume control setting = 0.0dB
		wm8994_setreg(WM8994_RIGHT_INP_PGA_GAIN, 0x40 | mikepgaval | 0x100);	// 0x40 = PGA in muted condition not connected to RADC Mix/Boost stage
		// 
		wm8994_setreg(WM8994_LEFT_ADC_BOOST_CONTROL, 0x000 | 0x100 * (mikebust20db != 0));	// 0x100 - 20 dB boost ON
		wm8994_setreg(WM8994_RIGHT_ADC_BOOST_CONTROL, 0x000);	// RLINEIN disconnected, RAUXIN disconnected
		//
	}

	// Установка чувствительность АЦП не требуется - стоит максимальная после сброса
	//uint_fast8_t level = 255;
	//wm8994_setreg(WM8994_LEFT_ADC_DIGITAL_VOLUME, level | 0);
	//wm8994_setreg(WM8994_RIGHT_ADC_DIGITAL_VOLUME, level | 0x100);
#endif
}


/* Параметры обработки звука с микрофона (эхо, эквалайзер, ...) */
static void wm8994_setprocparams(
	uint_fast8_t procenable,		/* включение обработки */
	const uint_fast8_t * gains		/* массив с параметрами */
	)
{
#if 0
	//debug_printf_P(PSTR("codec: procenable=%d, gains={ %2d,%2d,%2d,%2d,%2d }\n"), procenable, gains [0], gains [1], gains [2], gains [3], gains [4]);
	//enum { wide = 0, freq = 1 }; // default settings
	enum { wide = 1, freq = 0 }; // default settings

	// Смысл значения freq
	//	freq	EQ1 (High Pass) EQ2 (Band Pass) EQ3 (Band Pass) EQ4 (Band Pass) EQ5 (Low Pass) 
	//	0 		80Hz 			230Hz 			650Hz 			1.8kHz 			5.3kHz 
	//	1 		105Hz 			300Hz 			850Hz 			2.4kHz 			6.9kHz 
	//	2 		135Hz 			385Hz 			1.1kHz 			3.2kHz 			9.0kHz 
	//	3 		175Hz 			500Hz 			1.4kHz 			4.1kHz 			11.7kHz 

	// Назначение работы эквалайзера в ртакте ADC делается битом 0x100 в ргистре WM8994_EQ1
	if (procenable == 0)
	{
		// Выключено - все значения по умолчанию
		// digital gain control должно быть в диапазоне 0..24 (+12 db .. -12dB), 12 соответствует 0 dB
		enum { gain = 0 }; // default settings
		wm8994_setreg(WM8994_EQ1, // low cutoff - 0x22C reset value
			1 * (1u << 8) |	// 1 = block operates on digital stream to DAC
			freq * (1u << 5) | // Equalizer band 1 low pass -3dB cut-off frequency selection
			(12 - gain) * (1u << 0) | // EQ Band 1 digital gain control.
			0);
		wm8994_setreg(WM8994_EQ2, // peak 1 - 0x02C reset value
			wide * (1u << 8) |	// 0 = narrow band characteristic
			freq * (1u << 5) | // Equalizer Band 2 center frequency selection
			(12 - gain) * (1u << 0) | // EQ Band 2 digital gain control.
			0);
		wm8994_setreg(WM8994_EQ3, // peak 2 - 0x02C reset value
			wide * (1u << 8) |	// 0 = narrow band characteristic
			freq * (1u << 5) | // Equalizer Band 3 center frequency selection
			(12 - gain) * (1u << 0) | // EQ Band 3 digital gain control.
			0);
		wm8994_setreg(WM8994_EQ4, // peak 3 - 0x02C reset value
			wide * (1u << 8) |	// 0 = narrow band characteristic
			freq * (1u << 5) | // Equalizer Band 4 center frequency selection
			(12 - gain) * (1u << 0) | // EQ Band 4 digital gain control.
			0);
		wm8994_setreg(WM8994_EQ5, // high curoff - 0x02C reset value
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
		wm8994_setreg(WM8994_EQ1, // low cutoff - 0x22C reset value
			0 * (1u << 8) |	// 0 = block operates on digital stream from ADC, 1 = block operates on digital stream to DAC
			freq * (1u << 5) | // Equalizer band 1 low pass -3dB cut-off frequency selection
			(24 - gains [0]) * (1u << 0) | // EQ Band 1 digital gain control.
			0);
		wm8994_setreg(WM8994_EQ2, // peak 1 - 0x02C reset value
			wide * (1u << 8) |	// 0 = narrow band characteristic
			freq * (1u << 5) | // Equalizer Band 2 center frequency selection
			(24 - gains [1]) * (1u << 0) | // EQ Band 2 digital gain control.
			0);
		wm8994_setreg(WM8994_EQ3, // peak 2 - 0x02C reset value
			wide * (1u << 8) |	// 0 = narrow band characteristic
			freq * (1u << 5) | // Equalizer Band 3 center frequency selection
			(24 - gains [2]) * (1u << 0) | // EQ Band 3 digital gain control.
			0);
		wm8994_setreg(WM8994_EQ4, // peak 3 - 0x02C reset value
			wide * (1u << 8) |	// 0 = narrow band characteristic
			freq * (1u << 5) | // Equalizer Band 4 center frequency selection
			(24 - gains [3]) * (1u << 0) | // EQ Band 4 digital gain control.
			0);
		wm8994_setreg(WM8994_EQ5, // high curoff - 0x02C reset value
			freq * (1u << 5) | // Equalizer Band 5 high pass -3dB cut-off frequency selection
			(24 - gains [4]) * (1u << 0) | // EQ Band 5 digital gain control.
			0);
	}
#endif
}

static uint_fast16_t power_mgnt_reg_1 = 0;

static void wm8994_stop(void)
{
	wm8994_setreg(WM8994_SOFTWARE_RESET, 0x0000);	// RESET
}

static void wm8994_initialize_slave_fullduplex(void)
{
	debug_printf_P(PSTR("wm8994_initialize_slave_fullduplex start\n"));

	wm8994_setreg(WM8994_SOFTWARE_RESET, 0x0000);	// RESET


	/* Initialize the Control interface of the Audio Codec */
	//AUDIO_IO_Init();
	/* wm8994 Errata Work-Arounds */
	wm8994_setreg(0x102, 0x0003);
	wm8994_setreg(0x817, 0x0000);
	wm8994_setreg(0x102, 0x0000);

	/* Enable VMID soft start (fast), Start-up Bias Current Enabled */
	wm8994_setreg(WM8994_ANTIPOP_2, 0x006C);

	/* Enable bias generator, Enable VMID */
	wm8994_setreg(WM8994_POWER_MANAGEMENT_1, power_mgnt_reg_1 = 0x0003);
  
	/* Add Delay */
	local_delay_ms(50);

	/* Path Configurations for output */
	if (0)
	{
		//case OUTPUT_DEVICE_SPEAKER:
		/* Enable DAC1 (Left), Enable DAC1 (Right),
		Disable DAC2 (Left), Disable DAC2 (Right)*/
		wm8994_setreg(WM8994_POWER_MANAGEMENT_5, 0x0C0C);

		/* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
		wm8994_setreg(WM8994_DAC1_LEFT_MIXER_ROUTING, 0x0000);

		/* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
		wm8994_setreg(WM8994_DAC1_RIGHT_MIXER_ROUTING, 0x0000);

		/* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
		wm8994_setreg(WM8994_DAC2_LEFT_MIXER_ROUTING, 0x0002);

		/* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
		wm8994_setreg(WM8994_DAC2_RIGHT_MIXER_ROUTING, 0x0002);
		//break;
	}
	if (0)
	{
		//case OUTPUT_DEVICE_HEADPHONE:
		/* Disable DAC1 (Left), Disable DAC1 (Right),
		Enable DAC2 (Left), Enable DAC2 (Right)*/
		wm8994_setreg(WM8994_POWER_MANAGEMENT_5, 0x0303);

		/* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
		wm8994_setreg(WM8994_DAC1_LEFT_MIXER_ROUTING, 0x0001);

		/* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
		wm8994_setreg(WM8994_DAC1_RIGHT_MIXER_ROUTING, 0x0001);

		/* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
		wm8994_setreg(WM8994_DAC2_LEFT_MIXER_ROUTING, 0x0000);

		/* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
		wm8994_setreg(WM8994_DAC2_RIGHT_MIXER_ROUTING, 0x0000);
		//break;
	}
	if (1)
	{
		//case OUTPUT_DEVICE_BOTH:
		/* Enable DAC1 (Left), Enable DAC1 (Right),
		also Enable DAC2 (Left), Enable DAC2 (Right)*/
		wm8994_setreg(WM8994_POWER_MANAGEMENT_5, 0x0303 | 0x0C0C);

		/* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
		wm8994_setreg(WM8994_DAC1_LEFT_MIXER_ROUTING, 0x0001);

		/* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
		wm8994_setreg(WM8994_DAC1_RIGHT_MIXER_ROUTING, 0x0001);

		/* Enable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
		wm8994_setreg(WM8994_DAC2_LEFT_MIXER_ROUTING, 0x0001);

		/* Enable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
		wm8994_setreg(WM8994_DAC2_RIGHT_MIXER_ROUTING, 0x0001);

		// Set AIF1DAC1_MONO
		////wm8994_setreg(WM8994_AIF1_DAC1_FILTERS_1, 0x0080);
		// Set AIF1DAC2_MONO
		////wm8994_setreg(WM8994_AIF1_DAC2_FILTERS_1, 0x0080);
		//break;
	}
	if (0)
	{
		//case OUTPUT_DEVICE_AUTO :
		//default:
		/* Disable DAC1 (Left), Disable DAC1 (Right),
		Enable DAC2 (Left), Enable DAC2 (Right)*/
		wm8994_setreg(WM8994_POWER_MANAGEMENT_5, 0x0303);

		/* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
		wm8994_setreg(0x601, 0x0001);

		/* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
		wm8994_setreg(0x602, 0x0001);

		/* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
		wm8994_setreg(0x604, 0x0000);

		/* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
		wm8994_setreg(0x605, 0x0000);
		//break;
	}

	/* Path Configurations for input */
	if (1)
	{
		//case INPUT_DEVICE_DIGITAL_MICROPHONE_2 :
		/* Enable AIF1ADC2 (Left), Enable AIF1ADC2 (Right)
		* Enable DMICDAT2 (Left), Enable DMICDAT2 (Right)
		* Enable Left ADC, Enable Right ADC */
		wm8994_setreg(WM8994_POWER_MANAGEMENT_4, 0x0C30);

		/* Enable AIF1 DRC2 Signal Detect & DRC in AIF1ADC2 Left/Right Timeslot 1 */
		wm8994_setreg(0x450, 0x00DB);

		/* Disable IN1L, IN1R, IN2L, IN2R, Enable Thermal sensor & shutdown */
		wm8994_setreg(0x02, 0x6000);

		/* Enable the DMIC2(Left) to AIF1 Timeslot 1 (Left) mixer path */
		wm8994_setreg(WM8994_AIF1_ADC2_LEFT_MIXER_ROUTING, 0x0002);

		/* Enable the DMIC2(Right) to AIF1 Timeslot 1 (Right) mixer path */
		wm8994_setreg(WM8994_AIF1_ADC2_RIGHT_MIXER_ROUTING, 0x0002);

		/* GPIO1 pin configuration GP1_DIR = output, GP1_FN = AIF1 DRC2 signal detect */
		wm8994_setreg(0x700, 0x000E);
		//  break;
	}
	if (0)
	{
		//case INPUT_DEVICE_INPUT_LINE_1 :
		/* Enable AIF1ADC1 (Left), Enable AIF1ADC1 (Right)
		* Enable Left ADC, Enable Right ADC */
		wm8994_setreg(WM8994_POWER_MANAGEMENT_4, 0x0303);

		/* Enable AIF1 DRC1 Signal Detect & DRC in AIF1ADC1 Left/Right Timeslot 0 */
		wm8994_setreg(0x440, 0x00DB);

		/* Enable IN1L and IN1R, Disable IN2L and IN2R, Enable Thermal sensor & shutdown */
		wm8994_setreg(0x02, 0x6350);

		/* Enable the ADCL(Left) to AIF1 Timeslot 0 (Left) mixer path */
		wm8994_setreg(0x606, 0x0002);

		/* Enable the ADCR(Right) to AIF1 Timeslot 0 (Right) mixer path */
		wm8994_setreg(0x607, 0x0002);

		/* GPIO1 pin configuration GP1_DIR = output, GP1_FN = AIF1 DRC1 signal detect */
		wm8994_setreg(0x700, 0x000D);
		// break;
	}
  
  /*  Clock Configurations */
#if CODEC_TYPE_WM8994_USE_8KS
    /* AIF1 Sample Rate = 8 (KHz), ratio=256 */ 
    wm8994_setreg(WM8994_AIF1_RATE, 0x0003);
#else /* CODEC_TYPE_WM8994_USE_8KS */
    /* AIF1 Sample Rate = 16 (KHz), ratio=256 */ 
    //wm8994_setreg(WM8994_AIF1_RATE, 0x0033);
    /* AIF1 Sample Rate = 48 (KHz), ratio=256 */ 
    wm8994_setreg(WM8994_AIF1_RATE, 0x0083);
    /* AIF1 Sample Rate = 96 (KHz), ratio=256 */ 
    //wm8994_setreg(WM8994_AIF1_RATE, 0x00A3);
#endif /* CODEC_TYPE_WM8994_USE_8KS */
#if CODEC_TYPE_WM8994_USE_32BIT
	/* AIF1 Word Length = 32-bits, AIF1 Format = I2S (Default Register Value) */
	wm8994_setreg(WM8994_AIF1_CONTROL_1, 0x4010 | 0x0060);
#else /* CODEC_TYPE_WM8994_USE_32BIT */
	/* AIF1 Word Length = 16-bits, AIF1 Format = I2S (Default Register Value) */
	wm8994_setreg(WM8994_AIF1_CONTROL_1, 0x4010 | 0x0000);
#endif /* CODEC_TYPE_WM8994_USE_32BIT */

	/* slave mode */
	wm8994_setreg(WM8994_AIF1_MASTER_SLAVE, 0x0000);

	/* Enable the DSP processing clock for AIF1, Enable the core clock */
	wm8994_setreg(WM8994_CLOCKING_1, 0x000A);

	/* Enable AIF1 Clock, AIF1 Clock Source = MCLK1 pin */
	wm8994_setreg(WM8994_AIF1_CLOCKING_1, 0x0001);

	if (1)  /* Audio output selected */
	{
		/* Analog Output Configuration */

		/* Enable SPKRVOL PGA, Enable SPKMIXR, Enable SPKLVOL PGA, Enable SPKMIXL */
		wm8994_setreg(0x03, 0x0300);
#if 1
		// class AB
		/* Left Speaker Mixer Volume = 0dB */
		wm8994_setreg(WM8994_SPKMIXL_ATTENUATION, 0x0000);	// Reference for Speaker in Class AB mode = SPKVDD/2

		/* Speaker output mode = Class D, 
			Right Speaker Mixer Volume = 0dB 
			((0x23, 0x0100) = class AB)
		*/
		wm8994_setreg(WM8994_SPKMIXR_ATTENUATION, 0x0100);
#else
		// class D
		/* Left Speaker Mixer Volume = 0dB */
		wm8994_setreg(WM8994_SPKMIXL_ATTENUATION, 0x0000);	// Reference for Speaker in Class AB mode = SPKVDD/2

		/* Speaker output mode = Class D, 
			Right Speaker Mixer Volume = 0dB 
			((0x23, 0x0100) = class AB)
		*/
		wm8994_setreg(WM8994_SPKMIXR_ATTENUATION, 0x0000);
#endif

		/* Unmute DAC2 (Left) to Left Speaker Mixer (SPKMIXL) path,
		Unmute DAC2 (Right) to Right Speaker Mixer (SPKMIXR) path */
		wm8994_setreg(0x36, 0x0300);

		/* Enable bias generator, Enable VMID, Enable SPKOUTL, Enable SPKOUTR */
		wm8994_setreg(WM8994_POWER_MANAGEMENT_1, power_mgnt_reg_1 = 0x3003);

		/* Headphone/Speaker Enable */

		/* Enable Class W, Class W Envelope Tracking = AIF1 Timeslot 0 */
		wm8994_setreg(0x51, 0x0005);

		/* Enable bias generator, Enable VMID, Enable HPOUT1 (Left) and Enable HPOUT1 (Right) input stages */
		/* idem for Speaker */
		power_mgnt_reg_1 |= 0x0303 | 0x3003;
		wm8994_setreg(WM8994_POWER_MANAGEMENT_1, power_mgnt_reg_1);

		/* Enable HPOUT1 (Left) and HPOUT1 (Right) intermediate stages */
		wm8994_setreg(0x60, 0x0022);

		/* Enable Charge Pump */
		wm8994_setreg(0x4C, 0x9F25);

		/* Add Delay */
		local_delay_ms(15);

		/* Select DAC1 (Left) to Left Headphone Output PGA (HPOUT1LVOL) path */
		wm8994_setreg(0x2D, 0x0001);

		/* Select DAC1 (Right) to Right Headphone Output PGA (HPOUT1RVOL) path */
		wm8994_setreg(0x2E, 0x0001);

		/* Enable Left Output Mixer (MIXOUTL), Enable Right Output Mixer (MIXOUTR) */
		/* idem for SPKOUTL and SPKOUTR */
		wm8994_setreg(0x03, 0x0030 | 0x0300);

		/* Enable DC Servo and trigger start-up mode on left and right channels */
		wm8994_setreg(0x54, 0x0033);

		/* Add Delay */
		local_delay_ms(250);

		/* Enable HPOUT1 (Left) and HPOUT1 (Right) intermediate and output stages. Remove clamps */
		wm8994_setreg(WM8994_ANALOGUE_HP_1, 0x00EE);

		/* mix left and right channel for speakers */
		wm8994_setreg(WM8994_SPKOUT_MIXERS, 0x18 | 0x03);	// 0x0011 - default value - separated cahnnels	

		/* Unmutes */

		/* Unmute DAC 1 (Left) */
		wm8994_setreg(0x610, 0x00C0);

		/* Unmute DAC 1 (Right) */
		wm8994_setreg(0x611, 0x00C0);

		/* Unmute the AIF1 Timeslot 0 DAC path */
		wm8994_setreg(0x420, 0x0000);

		/* Unmute DAC 2 (Left) */
		wm8994_setreg(0x612, 0x00C0);

		/* Unmute DAC 2 (Right) */
		wm8994_setreg(0x613, 0x00C0);

		/* Unmute the AIF1 Timeslot 1 DAC2 path */
		wm8994_setreg(0x422, 0x0000);

		/* Volume Control */

		//wm8994_SetVolume(DeviceAddr, Volume);
	}

	if (1) /* Audio input selected */
	{
		if (1 /*(input_device == INPUT_DEVICE_DIGITAL_MICROPHONE_1) || (input_device == INPUT_DEVICE_DIGITAL_MICROPHONE_2)*/)
		{
			/* Enable Microphone bias 1 generator, Enable VMID */
			power_mgnt_reg_1 |= 0x0013;
			wm8994_setreg(WM8994_POWER_MANAGEMENT_1, power_mgnt_reg_1);

			/* ADC oversample enable */
			wm8994_setreg(0x620, 0x0002);

			/* AIF ADC2 HPF enable, HPF cut = voice mode 1 fc=127Hz at fs=8kHz */
			wm8994_setreg(0x411, 0x3800);
		}
		else if (0 /*(input_device == INPUT_DEVICE_INPUT_LINE_1) || (input_device == INPUT_DEVICE_INPUT_LINE_2) */)
		{
			/* Enable normal bias generator, Enable VMID */
			power_mgnt_reg_1 |= 0x0003;
			wm8994_setreg(WM8994_POWER_MANAGEMENT_1, power_mgnt_reg_1);

			/* Disable mute on IN1L, IN1L Volume = +0dB */
			wm8994_setreg(0x18, 0x000B);

			/* Disable mute on IN1R, IN1R Volume = +0dB */
			wm8994_setreg(0x1A, 0x000B);

			/* Disable mute on IN1L_TO_MIXINL, Gain = +0dB */
			wm8994_setreg(0x29, 0x0025);

			/* Disable mute on IN1R_TO_MIXINL, Gain = +0dB */
			wm8994_setreg(0x2A, 0x0025);

			/* IN1LN_TO_IN1L, IN1LP_TO_VMID, IN1RN_TO_IN1R, IN1RP_TO_VMID */
			wm8994_setreg(0x28, 0x0011);

			/* AIF ADC1 HPF enable, HPF cut = hifi mode fc=4Hz at fs=48kHz */
			wm8994_setreg(0x410, 0x1800);
		}
		/* Volume Control */
		//wm8994_SetVolume(DeviceAddr, Volume);
	}
	/* Return communication control value */

	debug_printf_P(PSTR("wm8994_initialize_slave_fullduplex done\n"));


}



const codec1if_t *
board_getaudiocodecif(void)
{

	static const char codecname [] = "WM8994";

	/* Интерфейс цправления кодеком */
	static const codec1if_t ifc =
	{
		wm8994_stop,
		wm8994_initialize_slave_fullduplex,
		wm8994_setvolume,		/* Установка громкости на наушники */
		wm8994_lineinput,		/* Выбор LINE IN как источника для АЦП вместо микрофона */
		wm8994_setprocparams,	/* Параметры обработки звука с микрофона (эхо, эквалайзер, ...) */
		codecname				/* Название кодека (всегда последний элемент в структуре) */
	};

	return & ifc;
}

#if 0
//////////////////////////////////
/**
  ******************************************************************************
  * @file    wm8994.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    24-June-2015
  * @brief   This file provides the WM8994 Audio Codec driver.   
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "wm8994.h"

/** @addtogroup BSP
  * @{
  */
  
/** @addtogroup Components
  * @{
  */ 

/** @addtogroup wm8994
  * @brief     This file provides a set of functions needed to drive the 
  *            WM8994 audio codec.
  * @{
  */

/** @defgroup WM8994_Private_Types
  * @{
  */

/**
  * @}
  */ 
  
/** @defgroup WM8994_Private_Defines
  * @{
  */
/* Uncomment this line to enable verifying data sent to codec after each write 
   operation (for debug purpose) */
#if !defined (VERIFY_WRITTENDATA)  
/* #define VERIFY_WRITTENDATA */
#endif /* VERIFY_WRITTENDATA */
/**
  * @}
  */ 

/** @defgroup WM8994_Private_Macros
  * @{
  */

/**
  * @}
  */ 
  
/** @defgroup WM8994_Private_Variables
  * @{
  */

/* Audio codec driver structure initialization */  
AUDIO_DrvTypeDef wm8994_drv = 
{
  wm8994_Init,
  wm8994_DeInit,
  wm8994_ReadID,

  wm8994_Play,
  wm8994_Pause,
  wm8994_Resume,
  wm8994_Stop,  

  wm8994_SetFrequency,
  wm8994_SetVolume,
  wm8994_SetMute,  
  wm8994_SetOutputMode,

  wm8994_Reset
};

static uint32_t outputEnabled = 0;
static uint32_t inputEnabled = 0;
/**
  * @}
  */ 

/** @defgroup WM8994_Function_Prototypes
  * @{
  */
static uint_fast8_t CODEC_IO_Write(uint_fast8_t Addr, uint_fast16_t Reg, uint_fast16_t Value);
/**
  * @}
  */ 

/** @defgroup WM8994_Private_Functions
  * @{
  */ 

/**
  * @brief Initializes the audio codec and the control interface.
  * @param DeviceAddr: Device address on communication Bus.   
  * @param OutputInputDevice: can be OUTPUT_DEVICE_SPEAKER, OUTPUT_DEVICE_HEADPHONE,
  *  OUTPUT_DEVICE_BOTH, OUTPUT_DEVICE_AUTO, INPUT_DEVICE_DIGITAL_MICROPHONE_1,
  *  INPUT_DEVICE_DIGITAL_MICROPHONE_2, INPUT_DEVICE_INPUT_LINE_1 or INPUT_DEVICE_INPUT_LINE_2.
  * @param Volume: Initial volume level (from 0 (Mute) to 100 (Max))
  * @param AudioFreq: Audio Frequency 
  * @retval 0 if correct communication, else wrong communication
  */
static uint_fast32_t wm8994_Init(uint_fast16_t DeviceAddr, uint_fast16_t OutputInputDevice, uint_fast8_t Volume, uint32_t AudioFreq)
{
	uint_fast32_t counter = 0;
	uint_fast16_t output_device = OutputInputDevice & 0xFF;
	uint_fast16_t input_device = OutputInputDevice & 0xFF00;
	//uint16_t power_mgnt_reg_1 = 0;

  /* Initialize the Control interface of the Audio Codec */
  AUDIO_IO_Init();
  /* wm8994 Errata Work-Arounds */
  wm8994_setreg(0x102, 0x0003);
  wm8994_setreg(0x817, 0x0000);
  wm8994_setreg(0x102, 0x0000);
  
  /* Enable VMID soft start (fast), Start-up Bias Current Enabled */
  wm8994_setreg(0x39, 0x006C);
  
  /* Enable bias generator, Enable VMID */
  wm8994_setreg(WM8994_POWER_MANAGEMENT_1, power_mgnt_reg_1 = 0x0003);
  
  /* Add Delay */
  local_delay_ms(50);

  /* Path Configurations for output */
  if (output_device > 0)
  {
    outputEnabled = 1;
    switch (output_device)
    {
    case OUTPUT_DEVICE_SPEAKER:
      /* Enable DAC1 (Left), Enable DAC1 (Right),
      Disable DAC2 (Left), Disable DAC2 (Right)*/
      wm8994_setreg(0x05, 0x0C0C);

      /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
      wm8994_setreg(0x601, 0x0000);

      /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
      wm8994_setreg(0x602, 0x0000);

      /* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
      wm8994_setreg(0x604, 0x0002);

      /* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
      wm8994_setreg(0x605, 0x0002);
      break;

    case OUTPUT_DEVICE_HEADPHONE:
      /* Disable DAC1 (Left), Disable DAC1 (Right),
      Enable DAC2 (Left), Enable DAC2 (Right)*/
      wm8994_setreg(0x05, 0x0303);

      /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
      wm8994_setreg(0x601, 0x0001);

      /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
      wm8994_setreg(0x602, 0x0001);

      /* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
      wm8994_setreg(0x604, 0x0000);

      /* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
      wm8994_setreg(0x605, 0x0000);
      break;

    case OUTPUT_DEVICE_BOTH:
      /* Enable DAC1 (Left), Enable DAC1 (Right),
      also Enable DAC2 (Left), Enable DAC2 (Right)*/
      wm8994_setreg(0x05, 0x0303 | 0x0C0C);

      /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
      wm8994_setreg(0x601, 0x0001);

      /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
      wm8994_setreg(0x602, 0x0001);

      /* Enable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
      wm8994_setreg(0x604, 0x0002);

      /* Enable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
      wm8994_setreg(0x605, 0x0002);
      break;

    case OUTPUT_DEVICE_AUTO :
    default:
      /* Disable DAC1 (Left), Disable DAC1 (Right),
      Enable DAC2 (Left), Enable DAC2 (Right)*/
      wm8994_setreg(0x05, 0x0303);

      /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
      wm8994_setreg(0x601, 0x0001);

      /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
      wm8994_setreg(0x602, 0x0001);

      /* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
      wm8994_setreg(0x604, 0x0000);

      /* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
      wm8994_setreg(0x605, 0x0000);
      break;
    }
  }
  else
  {
    outputEnabled = 0;
  }

  /* Path Configurations for input */
  if (input_device > 0)
  {
    inputEnabled = 1;
    switch (input_device)
    {
    case INPUT_DEVICE_DIGITAL_MICROPHONE_2 :
      /* Enable AIF1ADC2 (Left), Enable AIF1ADC2 (Right)
       * Enable DMICDAT2 (Left), Enable DMICDAT2 (Right)
       * Enable Left ADC, Enable Right ADC */
      wm8994_setreg(0x04, 0x0C30);

      /* Enable AIF1 DRC2 Signal Detect & DRC in AIF1ADC2 Left/Right Timeslot 1 */
      wm8994_setreg(0x450, 0x00DB);

      /* Disable IN1L, IN1R, IN2L, IN2R, Enable Thermal sensor & shutdown */
      wm8994_setreg(0x02, 0x6000);

      /* Enable the DMIC2(Left) to AIF1 Timeslot 1 (Left) mixer path */
      wm8994_setreg(0x608, 0x0002);

      /* Enable the DMIC2(Right) to AIF1 Timeslot 1 (Right) mixer path */
      wm8994_setreg(0x609, 0x0002);

      /* GPIO1 pin configuration GP1_DIR = output, GP1_FN = AIF1 DRC2 signal detect */
      wm8994_setreg(0x700, 0x000E);
      break;

    case INPUT_DEVICE_INPUT_LINE_1 :
      /* Enable AIF1ADC1 (Left), Enable AIF1ADC1 (Right)
       * Enable Left ADC, Enable Right ADC */
      wm8994_setreg(0x04, 0x0303);

      /* Enable AIF1 DRC1 Signal Detect & DRC in AIF1ADC1 Left/Right Timeslot 0 */
      wm8994_setreg(0x440, 0x00DB);

      /* Enable IN1L and IN1R, Disable IN2L and IN2R, Enable Thermal sensor & shutdown */
      wm8994_setreg(0x02, 0x6350);

      /* Enable the ADCL(Left) to AIF1 Timeslot 0 (Left) mixer path */
      wm8994_setreg(0x606, 0x0002);

      /* Enable the ADCR(Right) to AIF1 Timeslot 0 (Right) mixer path */
      wm8994_setreg(0x607, 0x0002);

      /* GPIO1 pin configuration GP1_DIR = output, GP1_FN = AIF1 DRC1 signal detect */
      wm8994_setreg(0x700, 0x000D);
      break;

    case INPUT_DEVICE_DIGITAL_MICROPHONE_1 :
    case INPUT_DEVICE_INPUT_LINE_2 :
    default:
      /* Actually, no other input devices supported */
      counter++;
      break;
    }
  }
  else
  {
    inputEnabled = 0;
  }
  
#if CODEC_TYPE_WM8994_USE_8KS
    /* AIF1 Sample Rate = 8 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x0003);
#else /* CODEC_TYPE_WM8994_USE_8KS */
    /* AIF1 Sample Rate = 48 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x0083);
#endif /* CODEC_TYPE_WM8994_USE_8KS */

#if 0
  /*  Clock Configurations */
  switch (AudioFreq)
  {
  case  AUDIO_FREQUENCY_8K:
    /* AIF1 Sample Rate = 8 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x0003);
    break;
    
  case  AUDIO_FREQUENCY_16K:
    /* AIF1 Sample Rate = 16 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x0033);
    break;
    
  case  AUDIO_FREQUENCY_48K:
    /* AIF1 Sample Rate = 48 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x0083);
    break;
    
  case  AUDIO_FREQUENCY_96K:
    /* AIF1 Sample Rate = 96 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x00A3);
    break;
    
  case  AUDIO_FREQUENCY_11K:
    /* AIF1 Sample Rate = 11.025 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x0013);
    break;
    
  case  AUDIO_FREQUENCY_22K:
    /* AIF1 Sample Rate = 22.050 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x0043);
    break;
    
  case  AUDIO_FREQUENCY_44K:
    /* AIF1 Sample Rate = 44.1 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x0073);
    break; 
    
  default:
    /* AIF1 Sample Rate = 48 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x0083);
    break; 
  }
#endif

  /* AIF1 Word Length = 16-bits, AIF1 Format = I2S (Default Register Value) */
  wm8994_setreg(0x300, 0x4010);
  
	#if CODEC_TYPE_WM8994_MASTER
	  /* master mode */
	  wm8994_setreg(0x302, 1uL << 14);
	#else /* CODEC_TYPE_WM8994_MASTER */
	  /* slave mode */
	  wm8994_setreg(0x302, 0x0000);
	#endif /* CODEC_TYPE_WM8994_MASTER */

  /* Enable the DSP processing clock for AIF1, Enable the core clock */
  wm8994_setreg(0x208, 0x000A);
  
  /* Enable AIF1 Clock, AIF1 Clock Source = MCLK1 pin */
  wm8994_setreg(0x200, 0x0001);

  if (output_device > 0)  /* Audio output selected */
  {
    /* Analog Output Configuration */

    /* Enable SPKRVOL PGA, Enable SPKMIXR, Enable SPKLVOL PGA, Enable SPKMIXL */
    wm8994_setreg(0x03, 0x0300);

    /* Left Speaker Mixer Volume = 0dB */
    wm8994_setreg(WM8994_SPKMIXL_ATTENUATION, 0x0000);

    /* Speaker output mode = Class D, Right Speaker Mixer Volume = 0dB ((0x23, 0x0100) = class AB)*/
    wm8994_setreg(WM8994_SPKMIXR_ATTENUATION, 0x0000);

    /* Unmute DAC2 (Left) to Left Speaker Mixer (SPKMIXL) path,
    Unmute DAC2 (Right) to Right Speaker Mixer (SPKMIXR) path */
    wm8994_setreg(0x36, 0x0300);

    /* Enable bias generator, Enable VMID, Enable SPKOUTL, Enable SPKOUTR */
    wm8994_setreg(WM8994_POWER_MANAGEMENT_1, power_mgnt_reg_1 = 0x3003);

    /* Headphone/Speaker Enable */

    /* Enable Class W, Class W Envelope Tracking = AIF1 Timeslot 0 */
    wm8994_setreg(0x51, 0x0005);

    /* Enable bias generator, Enable VMID, Enable HPOUT1 (Left) and Enable HPOUT1 (Right) input stages */
    /* idem for Speaker */
    power_mgnt_reg_1 |= 0x0303 | 0x3003;
    wm8994_setreg(WM8994_POWER_MANAGEMENT_1, power_mgnt_reg_1);

    /* Enable HPOUT1 (Left) and HPOUT1 (Right) intermediate stages */
    wm8994_setreg(0x60, 0x0022);

    /* Enable Charge Pump */
    wm8994_setreg(0x4C, 0x9F25);

    /* Add Delay */
    local_delay_ms(15);

    /* Select DAC1 (Left) to Left Headphone Output PGA (HPOUT1LVOL) path */
    wm8994_setreg(0x2D, 0x0001);

    /* Select DAC1 (Right) to Right Headphone Output PGA (HPOUT1RVOL) path */
    wm8994_setreg(0x2E, 0x0001);

    /* Enable Left Output Mixer (MIXOUTL), Enable Right Output Mixer (MIXOUTR) */
    /* idem for SPKOUTL and SPKOUTR */
    wm8994_setreg(0x03, 0x0030 | 0x0300);

    /* Enable DC Servo and trigger start-up mode on left and right channels */
    wm8994_setreg(0x54, 0x0033);

    /* Add Delay */
    local_delay_ms(250);

    /* Enable HPOUT1 (Left) and HPOUT1 (Right) intermediate and output stages. Remove clamps */
    wm8994_setreg(0x60, 0x00EE);

    /* Unmutes */

    /* Unmute DAC 1 (Left) */
    wm8994_setreg(0x610, 0x00C0);

    /* Unmute DAC 1 (Right) */
    wm8994_setreg(0x611, 0x00C0);

    /* Unmute the AIF1 Timeslot 0 DAC path */
    wm8994_setreg(0x420, 0x0000);

    /* Unmute DAC 2 (Left) */
    wm8994_setreg(0x612, 0x00C0);

    /* Unmute DAC 2 (Right) */
    wm8994_setreg(0x613, 0x00C0);

    /* Unmute the AIF1 Timeslot 1 DAC2 path */
    wm8994_setreg(0x422, 0x0000);
    
    /* Volume Control */
    wm8994_SetVolume(DeviceAddr, Volume);
  }

  if (input_device > 0) /* Audio input selected */
  {
    if ((input_device == INPUT_DEVICE_DIGITAL_MICROPHONE_1) || (input_device == INPUT_DEVICE_DIGITAL_MICROPHONE_2))
    {
      /* Enable Microphone bias 1 generator, Enable VMID */
      power_mgnt_reg_1 |= 0x0013;
      wm8994_setreg(WM8994_POWER_MANAGEMENT_1, power_mgnt_reg_1);

      /* ADC oversample enable */
      wm8994_setreg(0x620, 0x0002);

      /* AIF ADC2 HPF enable, HPF cut = voice mode 1 fc=127Hz at fs=8kHz */
      wm8994_setreg(0x411, 0x3800);
    }
    else if ((input_device == INPUT_DEVICE_INPUT_LINE_1) || (input_device == INPUT_DEVICE_INPUT_LINE_2))
    {
      /* Enable normal bias generator, Enable VMID */
      power_mgnt_reg_1 |= 0x0003;
      wm8994_setreg(WM8994_POWER_MANAGEMENT_1, power_mgnt_reg_1);

      /* Disable mute on IN1L, IN1L Volume = +0dB */
      wm8994_setreg(0x18, 0x000B);

      /* Disable mute on IN1R, IN1R Volume = +0dB */
      wm8994_setreg(0x1A, 0x000B);

      /* Disable mute on IN1L_TO_MIXINL, Gain = +0dB */
      wm8994_setreg(0x29, 0x0025);

      /* Disable mute on IN1R_TO_MIXINL, Gain = +0dB */
      wm8994_setreg(0x2A, 0x0025);

      /* IN1LN_TO_IN1L, IN1LP_TO_VMID, IN1RN_TO_IN1R, IN1RP_TO_VMID */
      wm8994_setreg(0x28, 0x0011);

      /* AIF ADC1 HPF enable, HPF cut = hifi mode fc=4Hz at fs=48kHz */
      wm8994_setreg(0x410, 0x1800);
    }
    /* Volume Control */
    wm8994_SetVolume(DeviceAddr, Volume);
  }
  /* Return communication control value */
  return counter;  
}

/**
  * @brief  Deinitializes the audio codec.
  * @param  None
  * @retval  None
  */
void wm8994_DeInit(void)
{
  /* Deinitialize Audio Codec interface */
  AUDIO_IO_DeInit();
}

/**
  * @brief  Get the WM8994 ID.
  * @param DeviceAddr: Device address on communication Bus.
  * @retval The WM8994 ID 
  */
uint32_t wm8994_ReadID(uint16_t DeviceAddr)
{
  /* Initialize the Control interface of the Audio Codec */
  AUDIO_IO_Init();

  return ((uint32_t)AUDIO_IO_Read(DeviceAddr, WM8994_CHIPID_ADDR));
}

/**
  * @brief Start the audio Codec play feature.
  * @note For this codec no Play options are required.
  * @param DeviceAddr: Device address on communication Bus.   
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t wm8994_Play(uint16_t DeviceAddr, uint16_t* pBuffer, uint16_t Size)
{
  uint32_t counter = 0;
 
  /* Resumes the audio file playing */  
  /* Unmute the output first */
  wm8994_SetMute(DeviceAddr, AUDIO_MUTE_OFF);
  
  return counter;
}

/**
  * @brief Pauses playing on the audio codec.
  * @param DeviceAddr: Device address on communication Bus. 
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t wm8994_Pause(uint16_t DeviceAddr)
{  
  uint32_t counter = 0;
 
  /* Pause the audio file playing */
  /* Mute the output first */
  wm8994_SetMute(DeviceAddr, AUDIO_MUTE_ON);
  
  /* Put the Codec in Power save mode */
  wm8994_setreg(0x02, 0x01);
 
  return counter;
}

/**
  * @brief Resumes playing on the audio codec.
  * @param DeviceAddr: Device address on communication Bus. 
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t wm8994_Resume(uint16_t DeviceAddr)
{
  uint32_t counter = 0;
 
  /* Resumes the audio file playing */  
  /* Unmute the output first */
  wm8994_SetMute(DeviceAddr, AUDIO_MUTE_OFF);
  
  return counter;
}

/**
  * @brief Stops audio Codec playing. It powers down the codec.
  * @param DeviceAddr: Device address on communication Bus. 
  * @param CodecPdwnMode: selects the  power down mode.
  *          - CODEC_PDWN_SW: only mutes the audio codec. When resuming from this 
  *                           mode the codec keeps the previous initialization
  *                           (no need to re-Initialize the codec registers).
  *          - CODEC_PDWN_HW: Physically power down the codec. When resuming from this
  *                           mode, the codec is set to default configuration 
  *                           (user should re-Initialize the codec in order to 
  *                            play again the audio stream).
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t wm8994_Stop(uint16_t DeviceAddr, uint32_t CodecPdwnMode)
{
  uint32_t counter = 0;

  if (outputEnabled != 0)
  {
    /* Mute the output first */
    wm8994_SetMute(DeviceAddr, AUDIO_MUTE_ON);

    if (CodecPdwnMode == CODEC_PDWN_SW)
    {
       /* Only output mute required*/
    }
    else /* CODEC_PDWN_HW */
    {
      /* Mute the AIF1 Timeslot 0 DAC1 path */
      wm8994_setreg(0x420, 0x0200);

      /* Mute the AIF1 Timeslot 1 DAC2 path */
      wm8994_setreg(0x422, 0x0200);

      /* Disable DAC1L_TO_HPOUT1L */
      wm8994_setreg(0x2D, 0x0000);

      /* Disable DAC1R_TO_HPOUT1R */
      wm8994_setreg(0x2E, 0x0000);

      /* Disable DAC1 and DAC2 */
      wm8994_setreg(0x05, 0x0000);

      /* Reset Codec by writing in 0x0000 address register */
      wm8994_setreg(0x0000, 0x0000);

      outputEnabled = 0;
    }
  }
  return counter;
}

/**
  * @brief Sets higher or lower the codec volume level.
  * @param DeviceAddr: Device address on communication Bus.
  * @param Volume: a byte value from 0 to 255 (refer to codec registers 
  *         description for more details).
  * @retval 0 if correct communication, else wrong communication
  */
uint_fast32_t wm8994_SetVolume(uint_fast16_t DeviceAddr, uint_fast8_t Volume)
{
	uint_fast32_t counter = 0;
	uint_fast8_t convertedvol = VOLUME_CONVERT(Volume);

  /* Output volume */
  if (outputEnabled != 0)
  {
    if(convertedvol > 0x3E)
    {
      /* Unmute audio codec */
      wm8994_SetMute(DeviceAddr, AUDIO_MUTE_OFF);

      /* Left Headphone Volume */
      wm8994_setreg(0x1C, 0x3F | 0x140);

      /* Right Headphone Volume */
      wm8994_setreg(0x1D, 0x3F | 0x140);

      /* Left Speaker Volume */
      wm8994_setreg(0x26, 0x3F | 0x140);

      /* Right Speaker Volume */
      wm8994_setreg(0x27, 0x3F | 0x140);
    }
    else if (Volume == 0)
    {
      /* Mute audio codec */
      wm8994_SetMute(DeviceAddr, AUDIO_MUTE_ON);
    }
    else
    {
      /* Unmute audio codec */
      wm8994_SetMute(DeviceAddr, AUDIO_MUTE_OFF);

      /* Left Headphone Volume */
      wm8994_setreg(0x1C, convertedvol | 0x140);

      /* Right Headphone Volume */
      wm8994_setreg(0x1D, convertedvol | 0x140);

      /* Left Speaker Volume */
      wm8994_setreg(0x26, convertedvol | 0x140);

      /* Right Speaker Volume */
      wm8994_setreg(0x27, convertedvol | 0x140);
    }
  }

  /* Input volume */
  if (inputEnabled != 0)
  {
    convertedvol = VOLUME_IN_CONVERT(Volume);

    /* Left AIF1 ADC1 volume */
    wm8994_setreg(0x400, convertedvol | 0x100);

    /* Right AIF1 ADC1 volume */
    wm8994_setreg(0x401, convertedvol | 0x100);

    /* Left AIF1 ADC2 volume */
    wm8994_setreg(0x404, convertedvol | 0x100);

    /* Right AIF1 ADC2 volume */
    wm8994_setreg(0x405, convertedvol | 0x100);
  }
  return counter;
}

/**
  * @brief Enables or disables the mute feature on the audio codec.
  * @param DeviceAddr: Device address on communication Bus.   
  * @param Cmd: AUDIO_MUTE_ON to enable the mute or AUDIO_MUTE_OFF to disable the
  *             mute mode.
  * @retval 0 if correct communication, else wrong communication
  */
uint_fast32_t wm8994_SetMute(uint_fast16_t DeviceAddr, uint_fast32_t Cmd)
{
	uint_fast32_t counter = 0;
  
  if (outputEnabled != 0)
  {
    /* Set the Mute mode */
    if(Cmd == AUDIO_MUTE_ON)
    {
      /* Soft Mute the AIF1 Timeslot 0 DAC1 path L&R */
      wm8994_setreg(0x420, 0x0200);

      /* Soft Mute the AIF1 Timeslot 1 DAC2 path L&R */
      wm8994_setreg(0x422, 0x0200);
    }
    else /* AUDIO_MUTE_OFF Disable the Mute */
    {
      /* Unmute the AIF1 Timeslot 0 DAC1 path L&R */
      wm8994_setreg(0x420, 0x0000);

      /* Unmute the AIF1 Timeslot 1 DAC2 path L&R */
      wm8994_setreg(0x422, 0x0000);
    }
  }
  return counter;
}

/**
  * @brief Switch dynamically (while audio file is played) the output target 
  *         (speaker or headphone).
  * @param DeviceAddr: Device address on communication Bus.
  * @param Output: specifies the audio output target: OUTPUT_DEVICE_SPEAKER,
  *         OUTPUT_DEVICE_HEADPHONE, OUTPUT_DEVICE_BOTH or OUTPUT_DEVICE_AUTO 
  * @retval 0 if correct communication, else wrong communication
  */
uint_fast32_t wm8994_SetOutputMode(uint_fast16_t DeviceAddr, uint_fast8_t Output)
{
	uint_fast32_t counter = 0;

#if 1
    /* Enable DAC1 (Left), Enable DAC1 (Right), 
    also Enable DAC2 (Left), Enable DAC2 (Right)*/
    wm8994_setreg(0x05, 0x0303 | 0x0C0C);
    
    /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
    wm8994_setreg(0x601, 0x0001);
    
    /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
    wm8994_setreg(0x602, 0x0001);
    
    /* Enable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
    wm8994_setreg(0x604, 0x0002);
    
    /* Enable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
    wm8994_setreg(0x605, 0x0002);
#else
  
  switch (Output) 
  {
  case OUTPUT_DEVICE_SPEAKER:
    /* Enable DAC1 (Left), Enable DAC1 (Right), 
    Disable DAC2 (Left), Disable DAC2 (Right)*/
    wm8994_setreg(0x05, 0x0C0C);
    
    /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
    wm8994_setreg(0x601, 0x0000);
    
    /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
    wm8994_setreg(0x602, 0x0000);
    
    /* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
    wm8994_setreg(0x604, 0x0002);
    
    /* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
    wm8994_setreg(0x605, 0x0002);
    break;
    
  case OUTPUT_DEVICE_HEADPHONE:
    /* Disable DAC1 (Left), Disable DAC1 (Right), 
    Enable DAC2 (Left), Enable DAC2 (Right)*/
    wm8994_setreg(0x05, 0x0303);
    
    /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
    wm8994_setreg(0x601, 0x0001);
    
    /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
    wm8994_setreg(0x602, 0x0001);
    
    /* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
    wm8994_setreg(0x604, 0x0000);
    
    /* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
    wm8994_setreg(0x605, 0x0000);
    break;
    
  case OUTPUT_DEVICE_BOTH:
    /* Enable DAC1 (Left), Enable DAC1 (Right), 
    also Enable DAC2 (Left), Enable DAC2 (Right)*/
    wm8994_setreg(0x05, 0x0303 | 0x0C0C);
    
    /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
    wm8994_setreg(0x601, 0x0001);
    
    /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
    wm8994_setreg(0x602, 0x0001);
    
    /* Enable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
    wm8994_setreg(0x604, 0x0002);
    
    /* Enable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
    wm8994_setreg(0x605, 0x0002);
    break;
    
  default:
    /* Disable DAC1 (Left), Disable DAC1 (Right), 
    Enable DAC2 (Left), Enable DAC2 (Right)*/
    wm8994_setreg(0x05, 0x0303);
    
    /* Enable the AIF1 Timeslot 0 (Left) to DAC 1 (Left) mixer path */
    wm8994_setreg(0x601, 0x0001);
    
    /* Enable the AIF1 Timeslot 0 (Right) to DAC 1 (Right) mixer path */
    wm8994_setreg(0x602, 0x0001);
    
    /* Disable the AIF1 Timeslot 1 (Left) to DAC 2 (Left) mixer path */
    wm8994_setreg(0x604, 0x0000);
    
    /* Disable the AIF1 Timeslot 1 (Right) to DAC 2 (Right) mixer path */
    wm8994_setreg(0x605, 0x0000);
    break;    
  }  
#endif
  return counter;
}

/**
  * @brief Sets new frequency.
  * @param DeviceAddr: Device address on communication Bus.
  * @param AudioFreq: Audio frequency used to play the audio stream.
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t wm8994_SetFrequency(uint16_t DeviceAddr, uint32_t AudioFreq)
{
  uint32_t counter = 0;
 
  /*  Clock Configurations */
  switch (AudioFreq)
  {
  case  AUDIO_FREQUENCY_8K:
    /* AIF1 Sample Rate = 8 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x0003);
    break;
    
  case  AUDIO_FREQUENCY_16K:
    /* AIF1 Sample Rate = 16 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x0033);
    break;
    
  case  AUDIO_FREQUENCY_48K:
    /* AIF1 Sample Rate = 48 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x0083);
    break;
    
  case  AUDIO_FREQUENCY_96K:
    /* AIF1 Sample Rate = 96 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x00A3);
    break;
    
  case  AUDIO_FREQUENCY_11K:
    /* AIF1 Sample Rate = 11.025 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x0013);
    break;
    
  case  AUDIO_FREQUENCY_22K:
    /* AIF1 Sample Rate = 22.050 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x0043);
    break;
    
  case  AUDIO_FREQUENCY_44K:
    /* AIF1 Sample Rate = 44.1 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x0073);
    break; 
    
  default:
    /* AIF1 Sample Rate = 48 (KHz), ratio=256 */ 
    wm8994_setreg(0x210, 0x0083);
    break; 
  }
  return counter;
}

/**
  * @brief Resets wm8994 registers.
  * @param DeviceAddr: Device address on communication Bus. 
  * @retval 0 if correct communication, else wrong communication
  */
uint32_t wm8994_Reset(uint16_t DeviceAddr)
{
  uint32_t counter = 0;
  
  /* Reset Codec by writing in 0x0000 address register */
  counter = wm8994_setreg(0x0000, 0x0000);
  outputEnabled = 0;
  inputEnabled=0;

  return counter;
}

/**
  * @brief  Writes/Read a single data.
  * @param  Addr: I2C address
  * @param  Reg: Reg address 
  * @param  Value: Data to be written
  * @retval None
  */
static uint_fast8_t CODEC_IO_Write(uint_fast8_t Addr, uint_fast32_t Reg, uint_fast32_t Value)
{
  uint32_t result = 0;
  
 AUDIO_IO_Write(Addr, Reg, Value);
  
#ifdef VERIFY_WRITTENDATA
  /* Verify that the data has been correctly written */
  result = (AUDIO_IO_Read(Addr, Reg) == Value)? 0:1;
#endif /* VERIFY_WRITTENDATA */
  
  return result;
}

#endif

#endif /* (CODEC1_TYPE == CODEC_TYPE_WM8994) */
