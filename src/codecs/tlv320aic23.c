/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#include "hardware.h"

#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_TLV320AIC23B)


#include "board.h"
#include "formats.h"
#include "spi.h"

//
// Управление кодеком TLV320AIC23B (WM8731)
//
// В slave mode данный кодек изменяет выдаваетмые данные по спадающем фронту сигнала BCLK.
// Судя по документации, ожидает что мастер так же изменяет выдаваемые данные по спадающему фронту BCLK.
//
#include "audio.h"
#include "tlv320aic23.h"

// Clock period, SCLK no less then 80 nS (не выше 12.5 МГц)
#define TLV320AIC23_SPIMODE		SPIC_MODE3	// Linux initialize in mode 0
#define TLV320AIC23_SPISPEED 	SPIC_SPEEDFAST

// Условие использования оптимизированных функций обращения к SPI
#define WITHSPIEXT16 (WITHSPIHW && WITHSPI16BIT)

// The TLV320AIC23B is a write only device and responds only if R/W is 0.
#define TLV320AIC23_ADDRESS_W	0x34	// I2C address: 0x34 or 0x36	- depend on adress pin state

static void tlv320aic23_setreg(
	uint_fast8_t regv,			/* 7 bit value */
	uint_fast16_t datav			/* 9 bit value */
	)
{
	const uint_fast16_t fulldata = regv * 512 + (datav & 0x1ff);

#if CODEC_TYPE_TLV320AIC23B_USE_SPI
	// кодек управляется по SPI
	const spitarget_t target = targetcodec1;	/* addressing to chip */

	#if WITHSPILOWSUPPORTT || 1
		// Работа совместно с фоновым обменом SPI по прерываниям
		uint8_t txbuf [2];

		USBD_poke_u16_BE(txbuf, fulldata);
		prog_spi_io(target, TLV320AIC23_SPISPEED, TLV320AIC23_SPIMODE, 0, txbuf, ARRAY_SIZE(txbuf), NULL, 0, NULL, 0);

	#elif WITHSPIEXT16

		hardware_spi_connect_b16(TLV320AIC23_SPISPEED, TLV320AIC23_SPIMODE);
		prog_select(target);	/* start sending data to target chip */
		hardware_spi_b16_p1(fulldata);
		hardware_spi_complete_b16();
		prog_unselect(target);	/* done sending data to target chip */
		hardware_spi_disconnect();

	#else /* WITHSPIEXT16 */

		spi_select(target, TLV320AIC23_SPIMODE);
		spi_progval8_p1(target, fulldata >> 8);		// LSB=b8 of datav
		spi_progval8_p2(target, fulldata >> 0);
		spi_complete(target);
		spi_unselect(target);

	#endif /* WITHSPIEXT16 */

#else /* CODEC_TYPE_TLV320AIC23B_USE_SPI */

	// кодек управляется по I2C

#if WITHTWISW

	i2c_start(TLV320AIC23_ADDRESS_W);
	i2c_write(fulldata >> 8);
	i2c_write(fulldata >> 0);
	i2c_waitsend();
	i2c_stop();

#elif WITHTWIHW

	uint8_t buf[2] = { (fulldata >> 8), (fulldata & 0xFF), };
	i2chw_write(TLV320AIC23_ADDRESS_W, buf, 2);

#endif /* WITHTWISW */

#endif /* CODEC_TYPE_TLV320AIC23B_USE_SPI */
}

static void tlv320aic23_stop(void)
{
	tlv320aic23_setreg(TLV320AIC23_RESET, 0x00);	// RESET
}

static void tlv320aic23_initialize_fullduplex(void (* io_control)(uint_fast8_t on), uint_fast8_t master)
{
	PRINTF("tlv320aic23_initialize_fullduplex\n");
	const unsigned long framebits = CODEC1_FRAMEBITS;

	tlv320aic23_setreg(TLV320AIC23_RESET, 0x00);	// RESET

	tlv320aic23_setreg(TLV320AIC23_PWR,
		TLV320AIC23_CLK_OFF |	// Выключаем выход тактовой частоты (вывод 02) - можно использовать для тестирования - на незапрограммированной микросхеме сигнал присутствует
		TLV320AIC23_OSC_OFF |	// Выключаем кварцевый генератор
		//TLV320AIC23_OUT_OFF |	// Выключаем усилитель наушников
		//TLV320AIC23_ADC_OFF |	// Выключаем АЦП
		//TLV320AIC23_LINE_OFF |	// Выключаем Line input
		0
		);

	//	It is recommended that between changing any content of Digital Audio Interface or Sampling Control
	//	Register that the active bit is reset then set.

	unsigned iwl;
	switch (WITHADAPTERCODEC1WIDTH)
	{
	default:
	case 16: iwl = TLV320AIC23_IWL_16; break;
	case 20: iwl = TLV320AIC23_IWL_20; break;
	case 24: iwl = TLV320AIC23_IWL_24; break;
	case 32: iwl = TLV320AIC23_IWL_32; break;
	}

	tlv320aic23_setreg(TLV320AIC23_DIGT_FMT, 
			master * TLV320AIC23_MS_MASTER | /* 0 - operate in slave mode */
			iwl |
#if CODEC1_FORMATI2S_PHILIPS
		TLV320AIC23_FOR_I2S |
#else /* CODEC1_FORMATI2S_PHILIPS */
		TLV320AIC23_FOR_LJUST |
#endif /* CODEC1_FORMATI2S_PHILIPS */
		0
		);

	// Из-за ограничений SPI/I2S контроллера на STM32 возможно
	// использовать только режим с MCLK=256*Fs
	// Внутренний делитель от MCLK до битовой синхронизации - /8
	// На MCLK ожидаем 12.288 МГц
	// See TI SLWS106H table 3.3.2.2 Normal-Mode Sampling Rates

#if CODEC_TYPE_TLV320AIC23B_USE_8KS

	// BCLK = 0.256 MHz (stereo, 16 bit/ch)
	tlv320aic23_setreg(TLV320AIC23_SRATE,			// Sample Rate Control
		3 * (1U << TLV320AIC23_SR_SHIFT) |	// BOSR = 0, SR[3:0] bits = 3: 12.288 / 256 / 6 = 8 kHz DAC and ADC sampling rate
		0 * (1U << TLV320AIC23_BOSR_SHIFT) |
		0	
		);	

#else /* CODEC_TYPE_TLV320AIC23B_USE_8KS */

	// BCLK = 1.536 MHz (stereo, 16 bit/ch)
	tlv320aic23_setreg(TLV320AIC23_SRATE,			// Sample Rate Control
		0 * (1U << TLV320AIC23_SR_SHIFT) |	// BOSR = 0, SR[3:0] bits = 0: 12.288 / 256 = 48 kHz DAC and ADC sampling rate
		0 * (1U << TLV320AIC23_BOSR_SHIFT) |
		0	
		);	

#endif /* CODEC_TYPE_TLV320AIC23B_USE_8KS */

#if 1 //WITHDEBUG
	// Введено для того, чтобы при ошибках в настройке кодек был настроен на выдачу звука.
	// Выбираем вход с микрофона, включить ЦАП
	// mic input
	tlv320aic23_setreg(TLV320AIC23_ANLG, 
		TLV320AIC23_DAC_SELECTED |
		//TLV320AIC23_MICM_MUTED |
		1 * TLV320AIC23_MICB_20DB |			// 1 - включение предусилителя микрофона
		TLV320AIC23_INSEL_MIC |			// Оцифровка с микрофона а не с line in
		0
		);
#endif /* WITHDEBUG */

	tlv320aic23_setreg(TLV320AIC23_DIGT,
		0 * TLV320AIC23_ADCHP_ONFF |			/* ФВЧ перед АЦП - наличие бита означает ВЫКЛЮЧИТЬ */
		0 * TLV320AIC23_DACM_MUTE | 			/* 0 - выключаем DAC MUTE */
		0
		);

	tlv320aic23_setreg(TLV320AIC23_ACTIVE,
		TLV320AIC23_ACT_ON |		// Digital Interface Activation
		0
		);
}

/* Установка громкости на наушники */
static void tlv320aic23_setvolume(uint_fast16_t gain, uint_fast8_t mute, uint_fast8_t mutespk)
{
	uint_fast8_t level = (gain - BOARD_AFGAIN_MIN) * (TLV320AIC23_OUT_VOL_MAX - TLV320AIC23_OUT_VOL_MIN) / (BOARD_AFGAIN_MAX - BOARD_AFGAIN_MIN) + TLV320AIC23_OUT_VOL_MIN;
	uint_fast8_t mute_all = mute != 0 || mutespk != 0;
	tlv320aic23_setreg(TLV320AIC23_LCHNVOL, 
		(mute_all == 0) * (level & TLV320AIC23_OUT_VOL_MASK) |
		TLV320AIC23_LRS_ENABLED |	/* левый и правый одновременно */
		0 * TLV320AIC23_LZC_ON |	/* синхронизация с переходом через "0" */
		0
		);
}

/* Выбор LINE IN как источника для АЦП вместо микрофона */
static void tlv320aic23_lineinput(uint_fast8_t v, uint_fast8_t mikeboost20db, uint_fast16_t mikegain, uint_fast16_t linegain)
{
	//debug_printf_P(PSTR("tlv320aic23_lineinput: glob_mik1level=%d\n"), mikegain);
	(void) mikegain;	// управления усилением микрофона в этом кодеке нет
	if (v != 0)
	{
		uint_fast8_t level = (linegain - WITHLINEINGAINMIN) * (TLV320AIC23_IN_VOL_MAX - TLV320AIC23_IN_VOL_MIN) / (WITHLINEINGAINMAX - WITHLINEINGAINMIN) + TLV320AIC23_IN_VOL_MIN;
		tlv320aic23_setreg(TLV320AIC23_LINVOL, 
			TLV320AIC23_LRS_ENABLED |	/* левый и правый одновременно */
			(level & TLV320AIC23_IN_VOL_MASK) |
			0
			);
		// вход с line input
		// line input
		tlv320aic23_setreg(TLV320AIC23_ANLG, 
			TLV320AIC23_DAC_SELECTED |
			TLV320AIC23_MICM_MUTED |
			//TLV320AIC23_MICB_20DB |
			//TLV320AIC23_INSEL_MIC |			// Оцифровка с микрофона а не с line in
			0
			);
	}
	else
	{
		// вход с микрофона
		// mic input
		tlv320aic23_setreg(TLV320AIC23_ANLG, 
			TLV320AIC23_DAC_SELECTED |
			//TLV320AIC23_MICM_MUTED |
			(mikeboost20db != 0) * TLV320AIC23_MICB_20DB |			// 1 - включение предусилителя микрофона
			TLV320AIC23_INSEL_MIC |			// Оцифровка с микрофона а не с line in
			0
			);
	}
}


/* Параметры обработки звука с микрофона (эхо, эквалайзер, ...) */
static void tlv320aic23_setprocparams(
	uint_fast8_t procenable,		/* включение обработки */
	const uint_fast8_t * gains		/* массив с параметрами */
	)
{

}

/* требуется ли подача тактирования для инициадизации кодека */
static uint_fast8_t tlv320aic23_clocksneed(void)
{
	return 0;
}

const codec1if_t *
board_getaudiocodecif(void)
{

	static const char codecname [] = "TLV320AIC23";

	/* Интерфейс управления кодеком */
	static const codec1if_t ifc =
	{
		tlv320aic23_clocksneed,
		tlv320aic23_stop,
		tlv320aic23_initialize_fullduplex,
		tlv320aic23_setvolume,	/* Установка громкости на наушники */
		tlv320aic23_lineinput,	/* Выбор LINE IN как источника для АЦП вместо микрофона */
		tlv320aic23_setprocparams,	/* Параметры обработки звука с микрофона (эхо, эквалайзер, ...) */
		codecname				/* Название кодека (всегда последний элемент в структуре) */
	};

	return & ifc;
}

#endif /* (CODEC1_TYPE == CODEC_TYPE_TLV320AIC23B) */
