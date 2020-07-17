/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "audio.h"
#include "gpio.h"
#include "formats.h"
#include "spi.h"
#include "synthcalcs.h"
#include "keyboard.h"
#include "touch/touch.h"
#include "display/display.h"

#include <string.h>
#include <math.h>


#define CTLREG_SPIMODE	SPIC_MODE3

//#include "chip/cmx992.c"
/********************************/

#if defined(PLL1_TYPE)
static phase_t phase_last_n1 = 256;		/* предполагаем, такой делитель N допустим для всех типов микросхем PLL */
static pllhint_t last_hint1 = (pllhint_t) -1;
#endif

// for programming PLL2

const phase_t phase_0 = 0;

#if \
	defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_AD9834) || \
	defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_AD9835)
static uint_fast8_t dds1_profile;		/* информация о последнем использованом профиле при работе с DDS */
#endif

#if \
	defined(DDS2_TYPE) && (DDS2_TYPE == DDS_TYPE_AD9834) || \
	defined(DDS2_TYPE) && (DDS2_TYPE == DDS_TYPE_AD9835)
static uint_fast8_t dds2_profile;		/* информация о последнем использованом профиле при работе с DDS */
#endif

#if \
	defined(DDS3_TYPE) && (DDS3_TYPE == DDS_TYPE_AD9834) || \
	defined(DDS3_TYPE) && (DDS3_TYPE == DDS_TYPE_AD9835)
static uint_fast8_t dds3_profile;		/* информация о последнем использованом профиле при работе с DDS */
#endif
////////////////
// board specific functions

uint_fast8_t 	glob_agc;
static uint_fast8_t 	glob_opowerlevel = WITHPOWERTRIMMAX;	/* WITHPOWERTRIMMIN..WITHPOWERTRIMMAX */
uint_fast8_t	glob_loudspeaker_off;

static uint_fast8_t 	glob_tx;			// находимся в режиме передачи
static uint_fast8_t 	glob_sleep;			// находимся в режиме минимального потребления
static uint_fast8_t 	glob_af_input = BOARD_DETECTOR_SSB;		// код детектора - вид модуляции с которым работаем.
static uint_fast8_t		glob_nfm;			// режим NFM
static uint_fast8_t		glob_nfmnbon;		// режим NFM с шумоподавителем - SW2014FM
static uint_fast8_t 	glob_att;			// код аттенюатора
static uint_fast8_t 	glob_antenna;		// выбор антенны (0 - ANT1, 1 - ANT2)
static uint_fast8_t 	glob_preamp;		// включение предусилителя (УВЧ) приёмника
static uint_fast8_t 	glob_mikemute;		// отключить аудиовход балансного модулятора
static uint_fast8_t 	glob_vox;


static uint_fast8_t 	glob_bglight = WITHLCDBACKLIGHTMIN;	// включаем дисплей для работы в тествх в hightests()
static uint_fast8_t 	glob_bglightoff = 1;	// выключаем дисплей
static uint_fast8_t 	glob_kblight = 1;

//#if WITHKEYBOARD
static uint_fast8_t 	glob_poweron = 1;
//#endif /* WITHKEYBOARD */

static uint_fast8_t		glob_fanflag;	/* включение вентилятора */
#if WITHDCDCFREQCTL
	static uint_fast32_t 	glob_blfreq = UINT32_MAX;	/* DC-DC frequency divider */
#endif /* WITHDCDCFREQCTL */

/* Следующие два параметра совместно выбирают фильтры в случае использования отдельных фильтров для LSB и USB */
static uint_fast16_t 	glob_filter;		// код фильтра ПЧ. 16 бит из-за использования пары ADG714 в одной из конфигураций
static uint_fast8_t 	glob_if4lsb;	/* Выбор фильтра ПЧ или боковой в детекторе приёмника прямого преобразования */

static uint_fast8_t 	glob_notch;
static uint_fast8_t 	glob_notchnarrow;
static uint_fast8_t 	glob_lo1scale = 1;
static uint_fast8_t 	glob_affilter;

static uint_fast8_t 	glob_dac1value [2];
static uint_fast8_t 	glob_txcw;			// находимся в режиме передачи телеграфа
static uint_fast8_t 	glob_txgate = 1;	// разрешение драйвера и оконечного усилителя

static int_fast16_t		glob_adcoffset;		/* смещение для выходного сигнала с АЦП */
static uint_fast8_t		glob_flt_reset_n;	// сброс фильтров в FPGA DSP
static uint_fast8_t		glob_dactest;		/* вместо выхода интерполятора к ЦАП передатчика подключается выход NCO */
static uint_fast8_t		glob_tx_inh_enable;	/* разрешение реакции FPGA на вход tx_inh */
static uint_fast8_t		glob_tx_bpsk_enable;	/* разрешение прямого формирования модуляции в FPGA */
static uint_fast8_t		glob_mode_wfm;
static uint_fast8_t		glob_adcfifo;
static uint_fast8_t		glob_xvrtr;
static uint_fast8_t		glob_dacstraight;	// Требуется формирование кода для ЦАП в режиме беззнакового кода
static uint_fast8_t		glob_dither;		/* управление зашумлением в LTC2208 */
static uint_fast8_t		glob_adcrand;		/* управление рандомизацией выходных данных в LTC2208 */
static uint_fast8_t		glob_firprofile [2];	/* */
static uint_fast8_t 	glob_reset_n;
static uint_fast8_t 	glob_i2s_enable;	// разрешение генерации тактовой частоты для I2S в FPGA
static uint_fast8_t 	glob_lcdreset = 1;	//
static uint_fast8_t 	glob_lctl1;
static uint_fast8_t 	glob_codec2_nreset;

static uint_fast8_t 	glob_dac1;
static uint_fast16_t	glob_maxlabdac;	/* значение на выходе ЦАП для увода частоты опорного генератора PLL */

static uint_fast8_t 	glob_vco;
static uint_fast8_t 	glob_lo2xtal;

static uint_fast8_t 	glob_bandf;		/* код диапазонного фильтра приёмника */
static uint_fast8_t 	glob_bandf2;	/* код диапазонного фильтра передатчика */
static uint_fast8_t 	glob_bandf3;	/* управление через разъем ACC */
static uint_fast8_t		glob_pabias;	/* ток покоя выходного каскада передатчика */
static uint_fast8_t 	glob_bandfonhpf = 5;	/* код диапазонного фильтра передатчика, начиная с которого включается ФВЧ перед УВЧ а SW20xx */
static uint_fast8_t 	glob_bandfonuhf;
static uint_fast16_t 	glob_bcdfreq;	/* отображаемая частота с точностью сотен килогерц */

/* Управление согласующим устройством */
static uint_fast8_t 	glob_tuner_C, glob_tuner_L, glob_tuner_type, glob_tuner_bypass;
static uint_fast8_t 	glob_autotune;	/* Находимся в режиме настройки согласующего устройства */

static uint_fast8_t		glob_stage1level = 2;	/* index of code for A1..A0 of OPA2674I-14D in stage 1 */
static uint_fast8_t		glob_stage2level = 2;	/* index of code for A1..A0 of OPA2674I-14D in stage 2 */

static uint_fast8_t		glob_sdcardpoweron;	/* не-0: включить питание SD CARD */
static uint_fast8_t		glob_usbflashpoweron;/* не-0: включить питание USB FLASH */
static uint_fast8_t 	glob_user1;
static uint_fast8_t 	glob_user2;
static uint_fast8_t 	glob_user3;
static uint_fast8_t 	glob_user4;
static uint_fast8_t 	glob_user5;
static uint_fast8_t		glob_attvalue;	// RF signal gen attenuator value

static void prog_rfadc_update(void);

/**********************/
//#if defined(PLL1_TYPE) && (LO1MODE_DIRECT == 0)
//	#define PLLX_TYPE PLL1_TYPE
//#elif defined(PLL2_TYPE)
//	#define PLLX_TYPE PLL2_TYPE
//#endif

#if defined(PLL1_TYPE)
	#if (PLL1_TYPE == PLL_TYPE_ADF4001)
		#include "chip/adf4001.h"
	#elif (PLL1_TYPE == PLL_TYPE_LMX2306)
		#include "chip/lmx2306.h"
	#elif (PLL1_TYPE == PLL_TYPE_LMX1601)
		#include "chip/lmx1601.h"
	#elif (PLL1_TYPE == PLL_TYPE_MC145170)
		#include "chip/mc145170.h"
	#elif (PLL1_TYPE == PLL_TYPE_ADF4360)
		#include "chip/adf4360.h"
	#elif (PLL1_TYPE == PLL_TYPE_TSA6057)
		#include "chip/tsa6057.h"
	#elif (PLL1_TYPE == PLL_TYPE_UMA1501)
		#include "chip/uma1501.h"
	#elif (PLL1_TYPE == PLL_TYPE_LM7001)
		#include "chip/lm7001.h"
	#elif (PLL1_TYPE == PLL_TYPE_SI570)
		#include "chip/si570.h"
	#elif (PLL1_TYPE == PLL_TYPE_SI5351A)
		#include "chip/si5351a.h"
	#elif (PLL1_TYPE == PLL_TYPE_HMC830)
		#include "chip/hmc830.h"
	#elif (PLL1_TYPE == PLL_TYPE_RFFC5071)
		#include "chip/rffc5071.h"
	#endif
#endif /* defined(PLL1_TYPE) */

#if defined(PLL2_TYPE)
	#if (PLL2_TYPE == PLL_TYPE_ADF4001)
		#include "chip/adf4001.h"
	#elif (PLL2_TYPE == PLL_TYPE_LMX2306)
		#include "chip/lmx2306.h"
	#elif (PLL2_TYPE == PLL_TYPE_LMX1601)
		#include "chip/lmx1601.h"
	#elif (PLL2_TYPE == PLL_TYPE_MC145170)
		#include "chip/mc145170.h"
	#elif (PLL2_TYPE == PLL_TYPE_ADF4360)
		#include "chip/adf4360.h"
	#elif (PLL2_TYPE == PLL_TYPE_CMX992)
		#include "chip/cmx992.h"
	#elif (PLL2_TYPE == PLL_TYPE_UMA1501)
		#include "chip/uma1501.h"
	#elif (PLL2_TYPE == PLL_TYPE_LM7001)
		#include "chip/lm7001.h"
	#elif (PLL2_TYPE == PLL_TYPE_SI5351A)
		#include "chip/si5351a.h"
	#elif (PLL2_TYPE == PLL_TYPE_HMC830)
		#include "chip/hmc830.h"
	#elif (PLL2_TYPE == PLL_TYPE_RFFC5071)
		#include "chip/rffc5071.h"
	#endif
#endif /* defined(PLL2_TYPE) */

#if defined(DDS1_TYPE)
	#if (DDS1_TYPE == DDS_TYPE_AD9852)
		#include "chip/ad9852.h"
	#elif (DDS1_TYPE == DDS_TYPE_AD9857)
		#include "chip/ad9857.h"
	#elif (DDS1_TYPE == DDS_TYPE_AD9951)
		#include "chip/ad9951.h"
	#elif (DDS1_TYPE == DDS_TYPE_AD9834)
		#include "chip/ad9834.h"
	#elif (DDS1_TYPE == DDS_TYPE_AD9851)
		#include "chip/ad9851.h"
	#elif (DDS1_TYPE == DDS_TYPE_AD9835)
		#include "chip/ad9835.h"
	#elif (DDS1_TYPE == DDS_TYPE_FPGAV1)
		#include "chip/fpga_v1.h"
	#elif (DDS1_TYPE == DDS_TYPE_FPGAV2)
		#include "chip/fpga_v2.h"
	#endif
#endif /* defined(DDS1_TYPE) */

#if defined(DDS2_TYPE)
	#if (DDS2_TYPE == DDS_TYPE_AD9852)
		#include "chip/ad9852.h"
	#elif (DDS2_TYPE == DDS_TYPE_AD9857)
		#include "chip/ad9857.h"
	#elif (DDS2_TYPE == DDS_TYPE_AD9951)
		#include "chip/ad9951.h"
	#elif (DDS2_TYPE == DDS_TYPE_AD9834)
		#include "chip/ad9834.h"
	#elif (DDS2_TYPE == DDS_TYPE_AD9851)
		#include "chip/ad9851.h"
	#elif (DDS2_TYPE == DDS_TYPE_AD9835)
		#include "chip/ad9835.h"
	#endif
#endif /* defined(DDS2_TYPE) */

#if defined(DDS3_TYPE)
	#if (DDS3_TYPE == DDS_TYPE_AD9852)
		#include "chip/ad9852.h"
	#elif (DDS3_TYPE == DDS_TYPE_AD9857)
		#include "chip/ad9857.h"
	#elif (DDS3_TYPE == DDS_TYPE_AD9951)
		#include "chip/ad9951.h"
	#elif (DDS3_TYPE == DDS_TYPE_AD9834)
		#include "chip/ad9834.h"
	#elif (DDS3_TYPE == DDS_TYPE_AD9851)
		#include "chip/ad9851.h"
	#elif (DDS3_TYPE == DDS_TYPE_AD9835)
		#include "chip/ad9835.h"
	#endif
#endif /* defined(DDS3_TYPE) */


#if defined(RTC1_TYPE)
	#if (RTC1_TYPE == RTC_TYPE_M41T81)
		#include "chip/m41t81.h"
	#elif (RTC1_TYPE == RTC_TYPE_STM32F4xx)
		#include "chip/rtcstm32f4xx.h"
	#elif (RTC1_TYPE == RTC_TYPE_DS1305)
		#include "chip/ds1305.h"
	#elif (RTC1_TYPE == RTC_TYPE_DS1307)
		#include "chip/ds1307.h"
	#endif /* (RTC1_TYPE == RTC_TYPE_M41T81) */
#endif /* defined(RTC1_TYPE) */

#if XVTR_R820T2
	#include "chip/r820t.h"
#endif /* XVTR_R820T2 */

#if CTLREGMODE_RAVENDSP_V3 || CTLREGMODE_RAVENDSP_V4 || CTLREGMODE_RAVENDSP_V5

	static uint_fast8_t flag_ctldacreg;	/* признак модификации теневых значений. Требуется вывод в регистры */
	static uint_fast8_t flag_ctrlreg;	/* признак модификации теневых значений. Требуется вывод в регистры */

#elif (ATMEGA_CTLSTYLE_V7_H_INCLUDED || ARM_CTLSTYLE_V7_H_INCLUDED || ARM_CTLSTYLE_V7A_H_INCLUDED)

	static uint_fast8_t flag_rxctrlreg;	/* признак модификации теневых значений. Требуется вывод в регистры */
	static uint_fast8_t flag_ctldacreg;	/* признак модификации теневых значений. Требуется вывод в регистры */

#else

	static uint_fast8_t flag_ctrlreg;	/* признак модификации теневых значений. Требуется вывод в регистры */

#endif

#if WITHAUTOTUNER_UA1CEI

#include <ctype.h>

enum nmeaparser_states
{
	NMEAST_INITIALIZED,
	NMEAST_OPENED,	// встретился символ '$'
	NMEAST_CHSHI,	// прём старшего символа контрольной суммы
	NMEAST_CHSLO,	// приём младшего символа контрольной суммы


	//
	NMEAST_COUNTSTATES

};


static uint_fast8_t nmeaparser_state = NMEAST_INITIALIZED;
static uint_fast8_t nmeaparser_checksum;
static uint_fast8_t nmeaparser_chsval;
static uint_fast8_t nmeaparser_param;		// номер принимаемого параметра в строке
static uint_fast8_t nmeaparser_chars;		// количество символов, помещённых в буфер

#define NMEA_CHARSSMALL		24
#define NMEA_CHARSBIG		257
#define NMEA_BIGFIELD		255	// номер большого поля

enum
{
	//	ответ:
	NMF_CODE, //	$ANSW,

	NMF_STATE, //	состояние устройства
	NMF_FWD, //	V_FWD, //ADC датчик апрямой волны
	NMF_REF, //	V_REF, //ADC датчика отраженной волны
	NMF_C_SENS, //	C_SENS, //ADC датчика тока ACS712
	NMF_12V_SENS, //	U_SENS, //ADC входного напряжения питания 12V
	NMF_T_SENS, //	T_SENS, //ADC датчика температуры LM235

	NMEA_PARAMS
};

static char nmeaparser_buffsmall [NMEA_PARAMS] [NMEA_CHARSSMALL];
static char nmeaparser_buffbig [NMEA_CHARSBIG];

static unsigned nmeaparser_get_buffsize(uint_fast8_t field)
{
	switch (field)
	{
	case NMEA_BIGFIELD:
		return NMEA_CHARSBIG;
	default:
		return NMEA_CHARSSMALL;
	}
}

static char * nmeaparser_get_buff(uint_fast8_t field)
{
	switch (field)
	{
	case NMEA_BIGFIELD:
		return nmeaparser_buffbig;
	default:
		return nmeaparser_buffsmall [field];
	}
}

static uint_fast8_t calcxorv(
	const char * s,
	size_t len
	)
{
	unsigned char r = '*';
	while (len --)
		r ^= (unsigned char) * s ++;
	return r & 0xff;
}

static uint_fast8_t hex2int(uint_fast8_t c)
{
	if (isdigit((unsigned char) c))
		return c - '0';
	if (isupper((unsigned char) c))
		return c - 'A' + 10;
	if (islower((unsigned char) c))
		return c - 'a' + 10;
	return 0;
}

/* вызывается из обработчика прерываний */
// принятый символ с последовательного порта
void nmea_parsechar(uint_fast8_t c)
{
	switch (nmeaparser_state)
	{
	case NMEAST_INITIALIZED:
		if (c == '$')
		{
			nmeaparser_checksum = '*';
			nmeaparser_state = NMEAST_OPENED;
			nmeaparser_param = 0;		// номер принимаемого параметра в строке
			nmeaparser_chars = 0;		// количество символов, помещённых в буфер
		}
		break;

	case NMEAST_OPENED:
		nmeaparser_checksum ^= c;
		if (c == ',')
		{
			// закрываем буфер параметра, переходим к следующему параметру
			nmeaparser_get_buff(nmeaparser_param) [nmeaparser_chars] = '\0';
			nmeaparser_param += 1;
			nmeaparser_chars = 0;
		}
		else if (c == '*')
		{
			// закрываем буфер параметра, переходим к следующему параметру
			nmeaparser_get_buff(nmeaparser_param) [nmeaparser_chars] = '\0';
			nmeaparser_param += 1;
			// переходим к приёму контрольной суммы
			nmeaparser_state = NMEAST_CHSHI;
		}
		else if (nmeaparser_param < NMEA_PARAMS && nmeaparser_chars < (nmeaparser_get_buffsize(nmeaparser_param) - 1))
		{
			nmeaparser_get_buff(nmeaparser_param) [nmeaparser_chars] = c;
			nmeaparser_chars += 1;
			//stat_l1 = stat_l1 > nmeaparser_chars ? stat_l1 : nmeaparser_chars;
		}
		else
			nmeaparser_state = NMEAST_INITIALIZED;	// при ошибках формата строки
		break;

	case NMEAST_CHSHI:
		nmeaparser_chsval = hex2int(c) * 16;
		nmeaparser_state = NMEAST_CHSLO;
		break;

	case NMEAST_CHSLO:
		//debugstate();
		nmeaparser_state = NMEAST_INITIALIZED;
		if (nmeaparser_checksum == (nmeaparser_chsval + hex2int(c)))	// для тесто проверка контрольной суммы отключена
		{
			if (strcmp(nmeaparser_get_buff(NMF_CODE), "ANSW") == 0)
			{
				//
				const adcvalholder_t EXTFS = 0x0FFF;	// в тюнере стоит 12-бит АЦП
				// board_adc_store_data
				const adcvalholder_t FS = board_getadc_fsval(FWD);

				board_adc_store_data(FWD, strtoul(nmeaparser_get_buff(NMF_FWD), NULL, 10) * FS / EXTFS);
				board_adc_store_data(REF, strtoul(nmeaparser_get_buff(NMF_REF), NULL, 10) * FS / EXTFS);
				// для WITHTDIRECTDATA -  значения параметров напрямую получаются от контроллера усилителя мощности
				board_adc_store_data(PASENSEIX, strtol(nmeaparser_get_buff(NMF_C_SENS), NULL, 10));
				board_adc_store_data(XTHERMOIX, strtol(nmeaparser_get_buff(NMF_T_SENS), NULL, 10));
				board_adc_store_data(VOLTSOURCE, strtol(nmeaparser_get_buff(NMF_12V_SENS), NULL, 10));
			}
		}
		break;

	default:
		break;
	}
}

static void
ua1ceituner_send(void)
{
	//управление устройством
	//Обмен на скорости 250 kb/s, 8-N-1
	/*
	запрос:
	$COM,
	RX_TX_state,        //0 = RX 1 = TX
	BND_number,       //номер диапазона  1 - 10
	PA_class,         //0 = class A 1 = class AB
	FAN,              //0 = Вентиляторы выключены 1 = FAN1 = ON, 2 = FAN1+FAN2 = ON
	ANT,              //0 = антенна 1, 1 = антенна 2
	SEL_CTUNio,       //0 = конденсатор на входе тюнера 1 = конденсатор на входе тюнера
	SEL_CTUN,         //перебор емкости конденсатора тюнера  0 - 255
	SEL_LTUN,          //перебор ендуктивностей тюнера  0 - 255
	*CS<CR><LF>

	  ответ:
	  $ANSW,
	  state,          //состояние устройства (пока = 0)
	  V_FWD,            //ADC датчик апрямой волны
	  V_REF,            //ADC датчика отраженной волны
	  T_SENS,           //ADC датчика температуры LM235
	  C_SENS,           //ADC датчика тока ACS712
	  U_SENS,           //ADC входного напряжения питания 12V
	  SENS_3V3,         //ADC  напряжения питания 3.3V
	  SENS_5V,          //ADC  напряжения питания 5V
	  VREF              //ADC  измерения опорного напряжения
	  *CS<CR><LF>
	*/
	nmea_format(
			"$COM,"
			"%d,"	// RX_TX_state,        //0 = RX 1 = TX
			"%d,"	// BND_number,       //номер диапазона  1 - 10
			"%d,"	// PA_class,         //0 = class A 1 = class AB
			"%d,"	// FAN,              //0 = Вентиляторы выключены 1 = FAN1 = ON, 2 = FAN1+FAN2 = ON
			"%d,"	// ANT,              //0 = антенна 1, 1 = антенна 2
			"%d,"	// SEL_CTUNio,       //0 = конденсатор на входе тюнера 1 = конденсатор на входе тюнера
			"%d,"	// SEL_CTUN,         //перебор емкости конденсатора тюнера  0 - 255
			"%d,"	// SEL_LTUN,          //перебор ендуктивностей тюнера  0 - 255
			"*FF\r\n",	// *CS<CR><LF>
			glob_tx,
			glob_bandf3,
			1,	// 1=class AB, 0=class A
			glob_fanflag,
			glob_antenna,
			glob_tuner_type,
			glob_tuner_bypass ? 0 : glob_tuner_C,
			glob_tuner_bypass ? 0 : glob_tuner_L
		);
}

#endif /* WITHAUTOTUNER_UA1CEI */


#if WITHCPUDACHW

	//#define HARDWARE_DACBITS 12	/* ЦАП работает с 12-битными значениями */

	// AGC dac constans
	enum
	{
		dacwidth = HARDWARE_DACBITS,
		dacFScode = (1U << dacwidth) - 1,
		dacrefvoltage = DACVREF_CPU * 100,		// 3.3 volt - DAC reference voltage
	};

	enum
	{
	#if (CTLSTYLE_RAVENDSP_V1 || CTLSTYLE_DSPV1A)

		dacXagchighvotage = 2950,	// 2.9 volt - AD605 VGN max
		dacXagclowvoltage = 100,		// 0.1 volt - AD605 VGN min

		dac_agc_highcode = dacFScode * dacXagchighvotage / dacrefvoltage,
		dac_agc_lowcode = dacFScode * dacXagclowvoltage / dacrefvoltage,
		dac_agc_coderange = dac_agc_highcode - dac_agc_lowcode,

		// заглушка
		//dacXagchighvotage = 3300,	// 0.1..1.25 volt - AD9744 REFERENCE INPUT range (after 18k/10k chain).
		//dacXagclowvoltage = 280,

		dac_dacfs_highcode = dacFScode * dacXagchighvotage / dacrefvoltage,
		dac_dacfs_lowcode = dacFScode * dacXagclowvoltage / dacrefvoltage,
		dac_dacfs_coderange = dac_dacfs_highcode - dac_dacfs_lowcode

	#elif \
		CTLSTYLE_RAVENDSP_V3 || \
		CTLSTYLE_RAVENDSP_V4 || \
		CTLSTYLE_RAVENDSP_V5 || \
		CTLSTYLE_RAVENDSP_V6 || \
		CTLSTYLE_RAVENDSP_V7 || \
		CTLSTYLE_RAVENDSP_V8 || \
		CTLSTYLE_STORCH_V2 || \
		CTLSTYLE_STORCH_V3 || \
		CTLSTYLE_STORCH_V4 || \
		CTLSTYLE_STORCH_V5 || \
		CTLSTYLE_STORCH_V6 || \
		CTLSTYLE_STORCH_V7 || \
		CTLSTYLE_OLEG4Z_V1 || \
		CTLSTYLE_NUCLEO_V1 || \
		0

		dacXagchighvotage = 3300,	// 0.1..1.25 volt - AD9744 REFERENCE INPUT range (after 18k/10k chain).
		dacXagclowvoltage = 280,

		dac_dacfs_highcode = dacFScode * dacXagchighvotage / dacrefvoltage,
		dac_dacfs_lowcode = dacFScode * dacXagclowvoltage / dacrefvoltage,
		dac_dacfs_coderange = dac_dacfs_highcode - dac_dacfs_lowcode

	#endif

	};
#endif /* WITHCPUDACHW */

#if 0 && WITHDACOUTDSPAGC

static void setagcattenuation(long code, uint_fast8_t tx)	// в кодах ЦАП
{
	if (tx != 0)
		HARDWARE_DAC_AGC(0);
	else
		HARDWARE_DAC_AGC(dac_agc_highcode - ((code > dac_agc_coderange) ? dac_agc_coderange : code));
}

static void setlevelindicator(long code)	// в кодах ЦАП
{
	hardware_dac_ch2_setvalue((dacFScode - 1) > code ? (dacFScode - 1) : code);
}


#endif /* WITHDACOUTDSPAGC */

/* вывод битов через PIO процессора, если они управляются напрямую без SPI */
static void 
prog_gpioreg(void)
{

	#if LS020_RESET
		LS020_RESET_SET(glob_lcdreset);	// LCD reset bit
	#endif /* LS020_RESET */

	#if TARGET_CS4272_RESET_BIT
		// CODEC2 reset
		if (glob_codec2_nreset == 0)
			TARGET_CS4272_RESET_PORT_S(TARGET_CS4272_RESET_BIT);
		else
			TARGET_CS4272_RESET_PORT_C(TARGET_CS4272_RESET_BIT);
	#endif /* TARGET_CS4272_RESET_BIT */

	#if TARGET_DDSRESET_BIT
		// DDS RESET
		if (glob_reset_n == 0)
			TARGET_DDSRESET_PORT_S(TARGET_DDSRESET_BIT);
		else
			TARGET_DDSRESET_PORT_C(TARGET_DDSRESET_BIT);
	#endif /* TARGET_DDSRESET_BIT */

	#if defined (TARGET_BAND_DATA_SET)
		// Band select
		TARGET_BAND_DATA_SET(glob_bandf);
	#endif /* defined (TARGET_BAND_DATA_SET) */

	#if defined (TARGET_BANDF3_DATA_TX_SET)
		// Band select
		TARGET_BANDF3_DATA_TX_SET(glob_bandf3, glob_tx);
	#endif /* defined (TARGET_BANDF3_DATA_TX_SET) */

	#if defined (HARDWARE_SDIOPOWER_SET)
		// SD CARD POWER ENABLE BIT
		HARDWARE_SDIOPOWER_SET(glob_sdcardpoweron);
	#endif /* defined (HARDWARE_SDIOPOWER_SET) */

	#if defined (TARGET_USBFS_VBUSON_SET) && defined (WITHUSBHW_HOST)
		// USB FLASH POWER ENABLE BIT
		TARGET_USBFS_VBUSON_SET(glob_usbflashpoweron);
	#endif /* defined (TARGET_USBFS_VBUSON_SET) && defined (WITHUSBHW_HOST) */

	#if defined (HARDWARE_BL_SET)
		// яркость подсветки
		HARDWARE_BL_SET(! glob_bglightoff, glob_bglight - WITHLCDBACKLIGHTMIN);
	#endif /* defined (HARDWARE_BL_SET) */

	#if defined (HARDWARE_DAC_ALC)
	//#if WITHCPUDACHW && WITHPOWERTRIM && ! WITHNOTXDACCONTROL
		// ALC
		// регулировка напряжения на REFERENCE INPUT TXDAC AD9744
		//HARDWARE_DAC_ALC((glob_opowerlevel - WITHPOWERTRIMMIN) * dac_dacfs_coderange / (WITHPOWERTRIMMAX - WITHPOWERTRIMMIN) + dac_dacfs_lowcode);
		HARDWARE_DAC_ALC((WITHPOWERTRIMMAX - WITHPOWERTRIMMIN) * dac_dacfs_coderange / (WITHPOWERTRIMMAX - WITHPOWERTRIMMIN) + dac_dacfs_lowcode);
	//#endif /* WITHCPUDACHW && WITHPOWERTRIM && ! WITHNOTXDACCONTROL */
	#endif /* defined (HARDWARE_DAC_ALC) */
}


/* 
	сигналы управления индикатором, требующим кроме SPI ещё двух сигналов - RS (register select) и RST (reset).
	reset может формироваться через общий регистр управления.
*/

// выставить уровень на сигнале lcd register select - не требуется board_update
void board_lcd_rs(uint_fast8_t state)	
{
#if LS020_RS
	LS020_RS_SET(state);	// LCD register address
#endif /* LS020_RS */
}

/* инициализация на вывод битов PIO процессора, если некоторые биты управляются напрямую без SPI */
/* вызывается при запрещённых прерываниях.*/
static void
board_gpio_init(void)
{
	#if defined (HARDWARE_INITIALIZE)
		HARDWARE_INITIALIZE();
	#endif /* defined (HARDWARE_INITIALIZE) */
	///////////////////////
	// CODEC2 reset
	#if TARGET_CS4272_RESET_BIT
		CS4272_RESET_INITIALIZE();
	#endif /* TARGET_CS4272_RESET_BIT */

	///////////////////////
	// LCD RS bit
	#if LS020_RS
		LS020_RS_INITIALIZE();
	#endif /* LS020_RS */

	///////////////////////
	// LCD RST bit
	#if LS020_RESET
		LS020_RESET_INITIALIZE();
	#endif /* LS020_RESET */

	// DDS RESET
	#if TARGET_DDSRESET_BIT
		TARGET_DDSRESET_INITIALIZE();
	#endif /* TARGET_DDSRESET_BIT */

	#if ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED
	{
		enum { WORKMASK = 1U << 4 };	/* PB4 - pte1206 backlight, EM027BS013 border */
		arm_hardware_piob_outputs(WORKMASK, WORKMASK);
	}
	#endif /* ARM_STM32L051_TQFP32_CPUSTYLE_V1_H_INCLUDED */
	///////////////////////
	// SD CARD POWER ENABLE BIT
	#if HARDWARE_SDIOPOWER_BIT
		HARDWARE_SDIOPOWER_INITIALIZE();
	#endif /* HARDWARE_SDIOPOWER_BIT */

	#if defined (HARDWARE_KBD_INITIALIZE)
		HARDWARE_KBD_INITIALIZE();
	#endif /* defined (HARDWARE_KBD_INITIALIZE) */
	/* макроопределение, которое должно включить в себя все инициализации */
	#if defined (TARGET_FPGA_OVF_INITIALIZE)
		TARGET_FPGA_OVF_INITIALIZE();
	#endif /* defined (TARGET_FPGA_OVF_INITIALIZE) */
}

#if WITHSPISLAVE

#elif WITHINTEGRATEDDSP

#elif WITHEXTERNALDDSP

#endif /* (WITHEXTERNALDDSP || WITHINTEGRATEDDSP) && ! WITHSPISLAVE */


#if MULTIVFO

	#if CPUSTYLE_ATMEGA || CPUSTYLE_ATXMEGA

		#define VFODIVPOWER2	16	/* ~65 kHz granulation */
		typedef uint_fast16_t fseltype_t;

	#elif CPUSTYLE_ARM

		#define VFODIVPOWER2	0	/* 1 Hz granulation */
		typedef uint_fast32_t fseltype_t;

	#else

		#error Undefined CPUSTYLE_XXX

	#endif

static fseltype_t board_vcos [HYBRID_NVFOS - 1];

/* 
 * получить по частоте гетеродина номер ГПД.
 */
static uint_fast8_t 
pll1_getvco(
	uint_fast32_t f // 1-st lo freq
	)
{
	const fseltype_t freqloc = (fseltype_t) (f >> VFODIVPOWER2);

	uint_fast8_t bottom = 0;
	uint_fast8_t top = (sizeof board_vcos / sizeof board_vcos [0]) - 1;

	// Двоичный поиск
	while (bottom < top)
	{
		const uint_fast8_t middle = (top - bottom) / 2 + bottom;

		if (board_vcos [middle] > freqloc)
		{
			top = middle;	// нижняя граница диапазона значений текущего элемента сравнения больше значения поиска - продолжаем поиск в нижней половине
			continue;
		}
		if (board_vcos [middle + 1] < freqloc)	
		{
			bottom = middle + 1;
			continue;
		}
		return middle + 1;
	}
	return (bottom != 0) ? (sizeof board_vcos / sizeof board_vcos [0]): 0;
}

/* функция для настройки ГПД */
uint_fast32_t 
getvcoranges(uint_fast8_t vco, uint_fast8_t top)
{
	/* setup VFOs limits */
	uint_fast32_t tmp32, vfopad = 2UL << VFODIVPOWER2;
	fseltype_t tmp16;
	tmp32 = SYNTHLOWFREQ;	/* scale freq before calculate phase */
	uint_fast8_t i;

	for (i = 0; i < HYBRID_NVFOS; ++ i)
	{
		if (vco == i && top == 0)
			break;
		tmp32 = (uint_fast32_t) ((uint_fast64_t) tmp32 * FREQ_SCALE / FREQ_DENOM);
		if (vco == i && top == 1)
			break;
	}
	tmp16 = (tmp32 >> VFODIVPOWER2);
	return ((uint_fast32_t) tmp16 << VFODIVPOWER2) + (top ? - vfopad : + vfopad);
}

static void 
prog_vcodata_init(void)
{
	/* setup VFOs limits */
	uint_fast32_t tmp = SYNTHLOWFREQ;	/* scale freq before calculate phase */;
	uint_fast8_t i;

	for (i = 0; i < sizeof board_vcos / sizeof board_vcos [0]; ++ i)
	{

		tmp = (uint_fast32_t) ((uint_fast64_t)tmp * FREQ_SCALE / FREQ_DENOM);
		board_vcos [i] = (fseltype_t) (tmp >> VFODIVPOWER2);
	}
}

#endif /* MULTIVFO */

#if defined(PLL1_TYPE)

#if LO1DIVIDEVCO	/* используется управляеммый делитель за гетеродином */


struct FREQRZAK {
  uint16_t divider;	// общий делитель
  uint16_t VCO_Reg02h;	// параметры для программирования
  uint32_t fmin;
  uint32_t fmax;			// эта частота не входит в диапазон
};

#if FIXSCALE_48M0_X1_DIV256

static const struct FREQRZAK FLASHMEM freqs [] =
{
    { 4, 0x7D,        17975000UL,   20840000UL, }, /* 17.975000..20.840000 */
    { 5, 0x7C,        14380000UL,   16672000UL, }, /* 14.380000..16.672000 */
    { 6, 0x7B,        11983333UL,   13893333UL, }, /* 11.983333..13.893333 */
    { 7, 0x7A,        10271428UL,   11908571UL, }, /* 10.271428..11.908571 */
    { 8, 0x79,         8987500UL,   10420000UL, }, /* 8.987500..10.420000 */
    { 14, 0x73,        5135714UL,    5954285UL, }, /* 5.135714..5.954285 */
};

#endif /* FIXSCALE_48M0_X1_DIV256 */

static uint_fast16_t 
pll1_getoutdivider(
	uint_fast32_t freq // 1-st lo freq
	)
{
#if FIXSCALE_48M0_X1_DIV256

	uint_fast8_t high = (sizeof freqs / sizeof freqs [0]);
	uint_fast8_t low = 0;
	uint_fast8_t middle;	// результат поиска

	// Двоичный поиск
	while (low < high)
	{
		middle = (high - low) / 2 + low;
		if (freq < freqs [middle].fmin)	// нижняя граница не включается - для обеспечения формального попадания частоты DCO в рабочий диапазон
			low = middle + 1;
		else if (freq >= freqs [middle].fmax)
			high = middle;		// переходим к поиску в меньших индексах
		else
			goto found;
	}
#if 0
	display_at_P(PSTR(0, 0, "[pll1_getoutdivider Err]"));
	return 0;		/* требуемую частоту невозожно получить */
#endif

found: 
	// нужная комбинация делителей найдена.
	;
	return freqs [middle].divider;

#else /* FIXSCALE_48M0_X1_DIV256 */

	uint_fast32_t divider = SYNTHTOPFREQ / f;
	if (divider == 0)	// desired frequency is greater then valid top
		divider = 1;
	return divider;

#endif /* FIXSCALE_48M0_X1_DIV256 */
}

static void pll1_setoutdivider(uint_fast16_t divider)
{
#if defined(PLL2_TYPE)
	const phase_t v = divider;
	prog_pll2_n(& v);
#endif
}

#else /* LO1DIVIDEVCO */

static uint_fast16_t 
pll1_getoutdivider(
	uint_fast32_t f // 1-st lo freq
	)
{
	return 1;
}

#endif /* LO1DIVIDEVCO */

#endif /* defined(PLL1_TYPE) */

#if CTLSTYLE_V5	//  Маленькая плата. На плате синтезатора управление ГУН и четыре свободных выхода. Остальное - в цепочке
/* ctl register interface */

#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated
#if defined (LO1MODE_HYBRID) || defined (LO1MODE_FIXSCALE)
	static FLASHMEM const uint_fast8_t vcos [4] =
	{
		0x01,	// vco 0
		0x02,	// vco 1
		0x04,	// vco 2
		0x08,	// vco 3
	};
#endif

	rbtype_t rbbuff [4] = { 0 };

	/* учет диапазона - на 0-м диапазоне обход УВЧ включается принудительно */

	RBVAL(034, glob_bandf, 4);			// D4..D7: band select код выбора диапазонного фильтра
	RBVAL(032, glob_att, 2);	/* D3:D2: ATTENUATOR RELAYS POWER */
	RBBIT(031, glob_preamp && (glob_bandf != 0));				/* D1: RF amplifier */
	RBBIT(030, glob_tx);					/* D0: TX mode: 1 - TX режим передачи */

	// programming RX control registers
	// filter codes:
	// 0x06 - fil3 3.1 kHz
	// 0x01 - fil2 2.7 kHz
	// 0x00 - fil0 9.0 kHz
	// 0x02 - fil1 6.0 kHz
	// 0x03 - fil5 0.5 kHz
	// 0x04 - fil2 15 kHz (bypass)
	// 0x05 - unused (9 kHz filter in FM strip)
	// 0x07 - unused (6 kHz filter in FM strip)
	RBVAL(025, glob_filter, 3);	/* select IF filter, wrong order of bits */
	RBVAL(020, 0x00, 5);	/* Unused outputs */

	RBBIT(017, (glob_agc == BOARD_AGCCODE_OFF));		/* AGC OFF */
	RBVAL(014, glob_agc, 3);	/* AGC code (delay) */
	RBVAL(012, glob_tx ? BOARD_DETECTOR_MUTE : glob_af_input, 2);	/* AF input selection 0-ssb, 1-am, 2-mute, 3-fm */
	RBBIT(011, glob_tx || fm);		/* AF_IF_FF - IF amp ad605 OFF in TX mode or in FM mode */
	RBBIT(010, fm || am);				/* SSB_DET_DISABLE - switch lo4 off in AM and FM modes */

	/* регистр управления (74HC595), расположенный на плате синтезатора */
	RBVAL(004, 0x00, 4);	/* d4..d7 in control register - spare bits */

#if defined (LO1MODE_HYBRID) || defined (LO1MODE_FIXSCALE)
	RBVAL(000, vcos [glob_vco], 4);	/* d0..d3 in control register */
#else
	RBVAL(000, 0x00, 4);	/* d0..d3 in control register */
#endif

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGSTYLE_SW2012_MINI || CTLREGSTYLE_SW2012C

// управляющие регистры SW2012MINI с RDX0120
// управляющие регистры SW2012MINI c цветным дисплеем и регулировкой выходной мощности
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */


static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

	rbtype_t rbbuff [2] = { 0 };

	/* регистр управления (74HC595), дополнительный. Диапазоны выводятся по просьбе Геннадия из Николаева. */
	RBVAL(014, (glob_bandf - 0), 4);		/* d4..d7 - band selection d0..d7 */
	RBBIT(013, glob_af_input == BOARD_DETECTOR_WFM);	/* pin 07: d3: WFM mode */
	RBVAL(011, 0x00, 2);								/* d2..d1: spare */
	RBBIT(010, glob_lcdreset);			/* pin 15: d0: lctl0 */

	/* регистр управления (74HC595), расположенный на плате синтезатора */
	RBVAL(006, WITHPOWERTRIMMAX - glob_opowerlevel, 2);								/* d6..d7: spare or power level */
	RBBIT(005, glob_tx);				/* pin 05: d5: TX2 */
	RBBIT(004, glob_tx ? glob_txcw : glob_filter);			/* pin 04: d4: CW - на приёме - НЧ фильтр. */
	RBBIT(003, glob_att);				/* pin 03: d3: ATT */
	RBBIT(002, glob_preamp);			/* pin 02: d2 - PRE */
	RBBIT(001, glob_bandf >= glob_bandfonhpf);		/* pin 01: d1 - bnd2 signal */
	RBBIT(000, ! glob_reset_n);		/* pin 15: d0 in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGSTYLE_SW2012CN

#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// управляющие регистры SW2012MINI COLOR 2 c цветным дисплеем ILI9163 и регулировкой выходной мощности
// управляющие регистры SW2016MINI

	#if WITHAUTOTUNER

		#if SHORTSET7 || FULLSET7
			static void 
			prog_ctrlreg(uint_fast8_t plane)
			{
				const spitarget_t target = targetctl1;
				//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
				//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

				rbtype_t rbbuff [3] = { 0 };

			#if 1
				/* +++ Управление согласующим устройством - версия схемы от pedchenk.lena@rambler.ru ы*/
				/* регистр управления массивом конденсаторов */
				RBBIT(027, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 7: TYPE OF TUNER 	*/
				RBVAL(020, glob_tuner_bypass ? 0 : revbits8(glob_tuner_C) >> 1, 7);/* pin 6..1, 15: Capacitors tuner bank 	*/
				/* регистр управления наборной индуктивностью. */
				RBBIT(017, ! glob_tuner_bypass);		// pin 7: обход СУ
				RBVAL(010, glob_tuner_bypass ? 0 : revbits8(glob_tuner_L) >> 1, 7);/* Inductors tuner bank 	*/
				/* --- Управление согласующим устройством */
			#else
				/* +++ Управление согласующим устройством */
				/* регистр управления массивом конденсаторов */
				RBVAL(021, glob_tuner_bypass ? 0 : revbits8(glob_tuner_C) >> 1, 7);/* pin 7..1: Capacitors tuner bank 	*/
				RBBIT(020, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 15: TYPE OF TUNER 	*/
				/* регистр управления наборной индуктивностью. */
				RBBIT(017, glob_tuner_bypass);		// pin 01: обход СУ
				RBVAL(010, glob_tuner_bypass ? 0 : glob_tuner_L, 7);			/* Inductors tuner bank 	*/
				/* --- Управление согласующим устройством */
			#endif
				/* регистр управления (74HC595), расположенный на плате синтезатора */
				RBVAL(006, glob_opowerlevel - WITHPOWERTRIMMIN, 2);								/* d6..d7: spare or power level */
				RBBIT(005, glob_tx);				/* pin 05: d5: TX2 */
				RBBIT(004, glob_tx ? glob_txcw : glob_filter);			/* pin 04: d4: CW - на приёме - НЧ фильтр. */
				RBBIT(003, glob_att);				/* pin 03: d3: ATT */
				RBBIT(002, glob_preamp);			/* pin 02: d2 - PRE */
				RBBIT(001, glob_bandf >= glob_bandfonhpf);		/* pin 01: d1 - bnd2 signal */
				RBBIT(000, ! glob_reset_n);		/* pin 15: d0 in control register - ad9951 RESET */

				spi_select(target, CTLREG_SPIMODE);
				prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
				spi_unselect(target);
			}
		#elif SHORTSET8 || FULLSET8
			#error CTLREGSTYLE_SW2012CN && WITHAUTOTUNER && (SHORTSET8 || FULLSET8) not supported
		#endif
	#else /* WITHAUTOTUNER */

		static void 
		prog_ctrlreg(uint_fast8_t plane)
		{
			const spitarget_t target = targetctl1;
			//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
			//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

			rbtype_t rbbuff [3] = { 0 };

			/* регистр управления (74HC595), дополнительный. Диапазоны выводятся по просьбе Геннадия из Николаева. */
			//RBVAL(014, (glob_bandf - 0), 4);		/* d4..d7 - band selection d0..d7 */
			//RBBIT(013, glob_af_input == BOARD_DETECTOR_WFM);	/* pin 07: d3: WFM mode */
			//RBVAL(011, 0x00, 2);								/* d2..d1: spare */
			//RBBIT(010, glob_lcdreset);			/* pin 15: d0: lctl0 */

			/* регистр управления (74HC595), расположенный на плате синтезатора */
			RBVAL(006, glob_opowerlevel - WITHPOWERTRIMMIN, 2);								/* d6..d7: spare or power level */
			RBBIT(005, glob_tx);				/* pin 05: d5: TX2 */
			RBBIT(004, glob_tx ? glob_txcw : glob_filter);			/* pin 04: d4: CW - на приёме - НЧ фильтр. */
			RBBIT(003, glob_att);				/* pin 03: d3: ATT */
			RBBIT(002, glob_preamp);			/* pin 02: d2 - PRE */
			RBBIT(001, glob_bandf >= glob_bandfonhpf);		/* pin 01: d1 - bnd2 signal */
			RBBIT(000, ! glob_reset_n);		/* pin 15: d0 in control register - ad9951 RESET */

			spi_select(target, CTLREG_SPIMODE);
			prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
			spi_unselect(target);
		}
#endif /* WITHAUTOTUNER */

#elif CTLREGSTYLE_RA4YBO_AM0

	#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

	static void 
	prog_ctrlreg(uint_fast8_t plane)
	{
		const spitarget_t target = targetctl1;
		rbtype_t rbbuff [3] = { 0 };
		
		RBBIT(007, glob_filter);				// полоса
		RBBIT(006, glob_user2);				// ревербератор
		RBBIT(005, glob_user1);				// эквалайзер

		RBBIT(004, glob_bandf);		// 0: меньше 2 МГц, 1 - выше
		RBVAL(002, glob_att, 2);				/* ATT */
		RBBIT(001, glob_tx);
		RBBIT(000, glob_mikemute);

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}

#elif CTLREGSTYLE_SW2012CN_RN3ZOB
// с автотюнером
// управляющие регистры SW2012MINI COLOR 2 c цветным дисплеем ILI9163 и регулировкой выходной мощности

#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

	static void 
	prog_ctrlreg(uint_fast8_t plane)
	{
	#if SHORTSET7 || FULLSET7
		const spitarget_t target = targetctl1;
		//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
		//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

		rbtype_t rbbuff [3] = { 0 };

		/* +++ Управление согласующим устройством */
		/* регистр управления массивом конденсаторов - дальше от процессора */
		RBVAL(021, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_C) >> 1), 7);/* LSB-MSB: pin07-pin01. Capacitors tuner bank 	*/
		RBBIT(020, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 15: TYPE OF TUNER 	*/
		/* регистр управления наборной индуктивностью. - ближе к процессору */
		RBBIT(017, ! glob_tuner_bypass);		// pin 07: обход СУ
		RBVAL(010, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_L) >> 1), 7);	/* LSB-MSB: pin06-pin01,pin15: Inductors tuner bank 	*/
		/* --- Управление согласующим устройством */

		/* регистр управления (74HC595), расположенный на плате синтезатора */
		RBVAL(006, glob_opowerlevel - WITHPOWERTRIMMIN, 2);								/* d6..d7: spare or power level */
		RBBIT(005, glob_tx);				/* pin 05: d5: TX2 */
		RBBIT(004, glob_tx ? glob_txcw : glob_filter);			/* pin 04: d4: CW - на приёме - НЧ фильтр. */
		RBBIT(003, glob_att);				/* pin 03: d3: ATT */
		RBBIT(002, glob_preamp);			/* pin 02: d2 - PRE */
		RBBIT(001, glob_bandf >= glob_bandfonhpf);		/* pin 01: d1 - bnd2 signal */
		RBBIT(000, ! glob_reset_n);		/* pin 15: d0 in control register - ad9951 RESET */

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	#elif SHORTSET8 || FULLSET8
		#error CTLREGSTYLE_SW2012CN_RN3ZOB && WITHAUTOTUNER && (SHORTSET8 || FULLSET8) not supported
	#else
		#error CTLREGSTYLE_SW2012CN_RN3ZOB && WITHAUTOTUNER && (SHORTSET8 || FULLSET8) not supported
	#endif
	}



#elif CTLREGSTYLE_SW2012CN_RN3ZOB_V2
// с автотюнером
// с переключением антенн
// управляющие регистры SW2012MINI COLOR 2 c цветным дисплеем ILI9163 и регулировкой выходной мощности

#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

	static void 
	prog_ctrlreg(uint_fast8_t plane)
	{
		const spitarget_t target = targetctl1;
		//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
		//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

		rbtype_t rbbuff [3] = { 0 };

		/* +++ Управление согласующим устройством */
		/* регистр управления массивом конденсаторов - дальше от процессора */
		RBVAL(021, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_C) >> 1), 7);/* LSB-MSB: pin07-pin01. Capacitors tuner bank 	*/
		RBBIT(020, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 15: TYPE OF TUNER 	*/
		/* регистр управления наборной индуктивностью. - ближе к процессору */
		RBBIT(017, ! glob_tuner_bypass);		// pin 07: обход СУ
		RBVAL(010, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_L) >> 1), 7);	/* LSB-MSB: pin06-pin01,pin15: Inductors tuner bank 	*/
		/* --- Управление согласующим устройством */

		/* регистр управления (74HC595), расположенный на плате синтезатора */
		//RBVAL(006, glob_opowerlevel - WITHPOWERTRIMMIN, 2);								/* d6..d7: spare or power level */
		RBBIT(007, glob_antenna);			/* pin 07: antenna select */
		RBBIT(006, 1);						/* pin 06: FAN */
		RBBIT(005, glob_tx);				/* pin 05: d5: TX2 */
		RBBIT(004, glob_tx ? glob_txcw : glob_filter);			/* pin 04: d4: CW - на приёме - НЧ фильтр. */
		RBBIT(003, glob_att);				/* pin 03: d3: ATT */
		RBBIT(002, glob_preamp);			/* pin 02: d2 - PRE */
		RBBIT(001, glob_bandf >= glob_bandfonhpf);		/* pin 01: d1 - bnd2 signal */
		RBBIT(000, ! glob_reset_n);		/* pin 15: d0 in control register - ad9951 RESET */

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}

#elif CTLREGSTYLE_SW2013SF

#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

#if WITHWFM
	rbtype_t rbbuff [3] = { 0 };
	/* регистр управления (74HC595), дополнительный. */
	RBBIT(027, glob_af_input == BOARD_DETECTOR_WFM);	/* pin 07: d7: WFM mode */
	RBVAL(021, 0x00, 6);								/* d6..d1: spare */
	RBBIT(020, glob_lcdreset);			/* pin 15: d0: lctl0 */
#else
	rbtype_t rbbuff [2] = { 0 };
#endif /* WITHWFM */
	/* регистр управления IC6 (74HC595), расположенный на плате синтезатора */
	RBBIT(017, glob_tx);				/* pin 07: d7: TX2 */
	RBBIT(016, glob_tx ? glob_txcw : (glob_filter != BOARD_FILTER_2P7));			/* pin 06: d6: CW - на приёме - НЧ фильтр. */
	RBBIT(015, glob_att);				/* pin 05: d5: ATT */
	RBBIT(014, glob_preamp);			/* pin 04: d4: PRE */
	RBBIT(013, glob_filter);			/* pin 03: d3: NAR - включение узкополосного фильтра по НЧ */
	RBBIT(012, glob_lo1scale != 1);				/* pin 02: d2: UKV */
	RBBIT(011, glob_mikemute);		/* pin 01: d1: MUTE */
	RBBIT(010, WITHPOWERTRIMMAX - glob_opowerlevel);			/* pin 15: "1" - low power */

	/* регистр управления IC5 (74HC595), расположенный на плате синтезатора */
	RBVAL(004, glob_bandf, 4);			/* pin 04..pin 07 - band selection d0..d7 */
	RBBIT(003, 0);						/* pin 03 in control register - SPARE */
	RBBIT(002, glob_lcdreset);				/* pin 02 in control register - LCD RESET */
	RBBIT(001, glob_bandf >= glob_bandfonhpf);		/* pin 01 - bnd2 signal */
	RBBIT(000, ! glob_reset_n);		/* pin 15: in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGSTYLE_SW2013SF_V1	// For UT4UA - with RDX0154 & LCD backlight level
// For UT4UA управляющие регистры c регулировкой подсветки RDX0154 и регулировкой выходной мощности
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

#if WITHWFM
	rbtype_t rbbuff [3] = { 0 };
	/* регистр управления (74HC595), дополнительный. */
	RBBIT(027, glob_af_input == BOARD_DETECTOR_WFM);	/* pin 07: d7: WFM mode */
	RBVAL(021, 0x00, 6);								/* d6..d1: spare */
	RBBIT(020, glob_lcdreset);			/* pin 15: d0: lctl0 */
#else
	rbtype_t rbbuff [2] = { 0 };
#endif /* WITHWFM */
	/* регистр управления IC6 (74HC595), расположенный на плате синтезатора */
	RBBIT(017, glob_tx);				/* pin 07: d7: TX2 */
	RBBIT(016, glob_tx ? glob_txcw : (glob_filter != BOARD_FILTER_2P7));			/* pin 06: d6: CW - на приёме - НЧ фильтр. */
	RBBIT(015, glob_att);				/* pin 05: d5: ATT */
	RBBIT(014, glob_preamp);			/* pin 04: d4: PRE */
	RBBIT(013, glob_filter);			/* pin 03: d3: NAR - включение узкополосного фильтра по НЧ */
	RBBIT(012, glob_lo1scale != 1);				/* pin 02: d2: UKV */
	RBBIT(011, glob_mikemute);		/* pin 01: d1: MUTE */
	RBBIT(010, WITHPOWERTRIMMAX - glob_opowerlevel);			/* pin 15: "1" - low power */

	/* регистр управления IC5 (74HC595), расположенный на плате синтезатора */
	RBVAL(004, (glob_bandf - 0), 4);	/* pin 04..pin 07 - band selection d0..d7 */
	RBVAL(002, (glob_bglight - 1), 2);		/* d2..d3 - lcd backlight */
	RBBIT(001, glob_bandf >= glob_bandfonhpf);		/* pin 01 - bnd2 signal */
	RBBIT(000, ! glob_reset_n);		/* pin 15: in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGSTYLE_SW2013SF_V2
// for US2IT управляющие регистры c регулировкой подсветки (без управления диапазонами)
// 8 bit tuner
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

	rbtype_t rbbuff [5] = { 0 };
	/* +++ Управление согласующим устройством */
	/* дополнительный регистр */
	RBBIT(043, glob_tx);				/* pin 03:индикатор передачи */
	RBBIT(042, glob_antenna);			// pin 02: выбор антенны (0 - ANT1, 1 - ANT2)
	RBBIT(041, glob_tuner_bypass);		// pin 01: обход СУ
	RBBIT(040, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 15: TYPE OF TUNER 	*/
	/* регистр управления массивом конденсаторов */
	RBVAL8(030, glob_tuner_bypass ? 0 : revbits8(glob_tuner_C));/* Capacitors tuner bank 	*/
	/* регистр управления наборной индуктивностью. */
	RBVAL8(020, glob_tuner_bypass ? 0 : glob_tuner_L);			/* Inductors tuner bank 	*/
	/* --- Управление согласующим устройством */
	/* регистр управления IC6 (74HC595), расположенный на плате синтезатора */
	RBBIT(017, glob_tx);				/* pin 07: d7: TX2 */
	RBBIT(016, glob_tx ? glob_txcw : (glob_filter != BOARD_FILTER_2P7));			/* pin 06: d6: CW - на приёме - НЧ фильтр. */
	RBBIT(015, glob_att);				/* pin 05: d5: ATT */
	RBBIT(014, glob_preamp);			/* pin 04: d4: PRE */
	RBBIT(013, glob_filter);			/* pin 03: d3: NAR - включение узкополосного фильтра по НЧ */
	RBBIT(012, glob_lo1scale != 1);				/* pin 02: d2: UKV */
	RBBIT(011, glob_mikemute);		/* pin 01: d1: MUTE */
	RBBIT(010, WITHPOWERTRIMMAX - glob_opowerlevel);			/* pin 15: "1" - low power */

	/* регистр управления IC5 (74HC595), расположенный на плате синтезатора */
	//RBVAL(004, (glob_bandf - 0), 4);	/* pin 04..pin 07 - band selection d0..d7 */
	RBVAL(006, glob_bandf2, 2);	/* pin 06..pin 07 - spare */
	RBVAL(004, (glob_bglight - WITHLCDBACKLIGHTMIN), 2);	/* pin 04..pin 05 - LCD backlight */
	RBBIT(003, glob_autotune);						/* pin 03 in control register - SPARE */
	RBBIT(002, glob_lcdreset);				/* pin 02 in control register - LCD RESET */
	RBBIT(001, glob_bandf >= glob_bandfonhpf);		/* pin 01 - bnd2 signal */
	RBBIT(000, ! glob_reset_n);		/* pin 15: in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGSTYLE_SW2013SF_V3
// for US2IT управляющие регистры c регулировкой подсветки (без управления диапазонами)
// 7 bit tuner
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

	rbtype_t rbbuff [4] = { 0 };
	/* +++ Управление согласующим устройством */
	/* регистр управления массивом конденсаторов */
	RBVAL(031, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_C) >> 1), 7);/* Capacitors tuner bank 	*/
	RBBIT(030, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 15: TYPE OF TUNER 	*/
	/* регистр управления наборной индуктивностью. */
	RBBIT(027, glob_tuner_bypass);		// pin 01: обход СУ
	RBVAL(020, glob_tuner_bypass ? 0 : glob_tuner_L, 7);			/* Inductors tuner bank 	*/
	/* --- Управление согласующим устройством */
	/* регистр управления IC6 (74HC595), расположенный на плате синтезатора */
	RBBIT(017, glob_tx);				/* pin 07: d7: TX2 */
	RBBIT(016, glob_tx ? glob_txcw : (glob_filter != BOARD_FILTER_2P7));			/* pin 06: d6: CW - на приёме - НЧ фильтр. */
	RBBIT(015, glob_att);				/* pin 05: d5: ATT */
	RBBIT(014, glob_preamp);			/* pin 04: d4: PRE */
	RBBIT(013, glob_filter);			/* pin 03: d3: NAR - включение узкополосного фильтра по НЧ */
	RBBIT(012, glob_lo1scale != 1);				/* pin 02: d2: UKV */
	RBBIT(011, glob_mikemute);		/* pin 01: d1: MUTE */
	RBBIT(010, WITHPOWERTRIMMAX - glob_opowerlevel);			/* pin 15: "1" - low power */

	/* регистр управления IC5 (74HC595), расположенный на плате синтезатора */
	//RBVAL(004, (glob_bandf - 0), 4);	/* pin 04..pin 07 - band selection d0..d7 */
	RBVAL(006, glob_bandf2, 2);	/* pin 06..pin 07 - spare */
	RBVAL(004, (glob_bglight - WITHLCDBACKLIGHTMIN), 2);	/* pin 04..pin 05 - LCD backlight */
	RBBIT(003, glob_autotune);						/* pin 03 in control register - SPARE */
	RBBIT(002, glob_lcdreset);				/* pin 02 in control register - LCD RESET */
	RBBIT(001, glob_bandf >= glob_bandfonhpf);		/* pin 01 - bnd2 signal */
	RBBIT(000, ! glob_reset_n);		/* pin 15: in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGSTYLE_SW2013RDX
// SW2013SF с индикатором RDX154
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;

	rbtype_t rbbuff [4] = { 0 };	// сделано с запасом и на тюнер

#if WITHAUTOTUNER
		/* +++ Управление согласующим устройством */
	#if SHORTSET7 || FULLSET7

		/* +++ Управление согласующим устройством */
		/* регистр управления массивом конденсаторов */
		RBBIT(037, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 7: TYPE OF TUNER 	*/
		RBVAL(030, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_C) >> 1), 7);/* Capacitors tuner bank 	*/
		/* регистр управления наборной индуктивностью. */
		RBBIT(027, ! glob_tuner_bypass);		// pin 7: обход СУ
		RBVAL(020, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_L) >> 1), 7);			/* pin 15, 1..6: Inductors tuner bank 	*/
		/* --- Управление согласующим устройством */

	#elif SHORTSET8 || FULLSET8
		#error CTLREGSTYLE_SW2013RDX && WITHAUTOTUNER && (SHORTSET8 || FULLSET8) not supported
	#endif
		/* --- Управление согласующим устройством */
#endif /* WITHAUTOTUNER */

	/* регистр управления (IC6 74HC595), расположенный на плате трансивера */
	RBBIT(017, glob_tx);				/* pin 07: d7: TX2 */
	RBBIT(016, glob_tx ? glob_txcw : glob_filter);			/* pin 06: d6: CW - на приёме - НЧ фильтр. */
	RBBIT(015, glob_att);				/* pin 05: d5: ATT */
	RBBIT(014, glob_preamp);			/* pin 04: d4: PRE */
	RBBIT(013, glob_filter);			/* pin 03: d3: NAR - включение узкополосного фильтра по НЧ */
	RBBIT(012, glob_lo1scale != 1);				/* pin 02: d2: UKV */
	RBBIT(011, glob_mikemute);		/* pin 01: d1: MUTE */
	RBBIT(010, WITHPOWERTRIMMAX - glob_opowerlevel);			/* pin 15: "1" - low power */

	/* регистр управления (IC5 74HC595), расположенный на плате синтезатора */
	RBVAL(004, (glob_bandf - 0), 4);	/* pin 04..pin 07 - band selection d0..d7 */
	RBVAL(002, (glob_bglight - WITHLCDBACKLIGHTMIN), 2);	/* зшт 02, pin 03 - lcd backlight */
	RBBIT(001, glob_bandf >= glob_bandfonhpf);		/* pin 01 - bnd2 signal */
	RBBIT(000, ! glob_reset_n);		/* pin 15: in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGSTYLE_SW2013RDX_UY5UM_WO240
// SW2013SF с индикатором RDX154
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;

	rbtype_t rbbuff [4] = { 0 };	// сделано с запасом и на тюнер

#if WITHAUTOTUNER
		/* +++ Управление согласующим устройством */
	#if SHORTSET7 || FULLSET7

		/* +++ Управление согласующим устройством */
		/* регистр управления массивом конденсаторов */
		RBBIT(037, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 7: TYPE OF TUNER 	*/
		RBVAL(030, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_C) >> 1), 7);/* Capacitors tuner bank 	*/
		/* регистр управления наборной индуктивностью. */
		RBBIT(027, ! glob_tuner_bypass);		// pin 7: обход СУ
		RBVAL(020, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_L) >> 1), 7);			/* pin 15, 1..6: Inductors tuner bank 	*/
		/* --- Управление согласующим устройством */

	#elif SHORTSET8 || FULLSET8
		#error CTLREGSTYLE_SW2013RDX && WITHAUTOTUNER && (SHORTSET8 || FULLSET8) not supported
	#endif
		/* --- Управление согласующим устройством */
#endif /* WITHAUTOTUNER */

	/* регистр управления (IC6 74HC595), расположенный на плате трансивера */
	RBBIT(017, glob_tx);				/* pin 07: d7: TX2 */
	RBBIT(016, glob_tx ? glob_txcw : glob_filter);			/* pin 06: d6: CW - на приёме - НЧ фильтр. */
	RBBIT(015, glob_att);				/* pin 05: d5: ATT */
	RBBIT(014, glob_preamp);			/* pin 04: d4: PRE */
	RBBIT(013, glob_filter);			/* pin 03: d3: NAR - включение узкополосного фильтра по НЧ */
	RBBIT(012, glob_notch);				/* pin 02: d2: NOTCH ON */
	RBBIT(011, glob_mikemute);		/* pin 01: d1: MUTE */
	RBBIT(010, WITHPOWERTRIMMAX - glob_opowerlevel);			/* pin 15: "1" - low power */

	/* регистр управления (IC5 74HC595), расположенный на плате синтезатора */
	RBVAL(004, (glob_bandf - 0), 4);	/* pin 04..pin 07 - band selection d0..d7 */
	RBVAL(002, (glob_bglight - WITHLCDBACKLIGHTMIN), 2);	/* зшт 02, pin 03 - lcd backlight */
	RBBIT(001, glob_bandf >= glob_bandfonhpf);		/* pin 01 - bnd2 signal */
	RBBIT(000, glob_antenna);		/* pin 15: in control register - antenna select */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGSTYLE_SW2014NFM
// SW2013SF с индикатором RDX154
// дополнительная плата для NFM вместо управления подсветкой
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

#if WITHAUTOTUNER
		rbtype_t rbbuff [4] = { 0 };
		/* +++ Управление согласующим устройством */
	#if SHORTSET7 || FULLSET7

		/* +++ Управление согласующим устройством */
		/* регистр управления массивом конденсаторов */
		RBBIT(037, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 7: TYPE OF TUNER 	*/
		RBVAL(030, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_C) >> 1), 7);/* Capacitors tuner bank 	*/
		/* регистр управления наборной индуктивностью. */
		RBBIT(027, ! glob_tuner_bypass);		// pin 7: обход СУ
		RBVAL(020, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_L) >> 1), 7);			/* pin 15, 1..6: Inductors tuner bank 	*/
		/* --- Управление согласующим устройством */

	#elif SHORTSET8 || FULLSET8
		#error CTLREGSTYLE_SW2013RDX && WITHAUTOTUNER && (SHORTSET8 || FULLSET8) not supported
	#endif
		/* --- Управление согласующим устройством */
#else /* WITHAUTOTUNER */
		rbtype_t rbbuff [2] = { 0 };
#endif /* WITHAUTOTUNER */
	/* регистр управления (74HC595), расположенный дальше от процессора */
	RBBIT(017, glob_tx);				/* pin 07: d7: TX2 */
	RBBIT(016, glob_tx ? glob_txcw : glob_filter);			/* pin 06: d6: CW - на приёме - НЧ фильтр. */
	RBBIT(015, glob_att);				/* pin 05: d5: ATT */
	RBBIT(014, glob_preamp);			/* pin 04: d4: PRE */
	RBBIT(013, glob_filter);			/* pin 03: d3: NAR - включение узкополосного фильтра по НЧ */
	RBBIT(012, glob_bandf >= glob_bandfonuhf);				/* pin 02: d2: UKV */
	RBBIT(011, glob_mikemute);		/* pin 01: d1: MUTE */
	RBBIT(010, WITHPOWERTRIMMAX - glob_opowerlevel);			/* pin 15: "1" - low power */

	/* регистр управления (74HC595), расположенный ближе к процессору */
	RBVAL(004, glob_bandf, 4);	/* pin 04..pin 07 - band selection d0..d7 */
	RBBIT(003, glob_nfm);				/* pin 03 - включение режима NFM */
	RBBIT(002, glob_nfmnbon);			/* pin 02 - "0" - отключение шумоподавителя */
	RBBIT(001, glob_bandf >= glob_bandfonhpf);		/* pin 01 - bnd2 signal */
	RBBIT(000, ! glob_reset_n);		/* pin 15: in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGSTYLE_SW2016VHF
// SW2013SF с индикатором RDX154
// дополнительная плата для NFM вместо управления подсветкой
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

#if WITHAUTOTUNER
		rbtype_t rbbuff [4] = { 0 };
		/* +++ Управление согласующим устройством */
	#if SHORTSET7 || FULLSET7

		/* +++ Управление согласующим устройством */
		/* регистр управления массивом конденсаторов */
		RBBIT(037, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 7: TYPE OF TUNER 	*/
		RBVAL(030, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_C) >> 1), 7);/* Capacitors tuner bank 	*/
		/* регистр управления наборной индуктивностью. */
		RBBIT(027, ! glob_tuner_bypass);		// pin 7: обход СУ
		RBVAL(020, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_L) >> 1), 7);			/* pin 15, 1..6: Inductors tuner bank 	*/
		/* --- Управление согласующим устройством */

	#elif SHORTSET8 || FULLSET8
		#error CTLREGSTYLE_SW2013RDX && WITHAUTOTUNER && (SHORTSET8 || FULLSET8) not supported
	#endif
		/* --- Управление согласующим устройством */
#else /* WITHAUTOTUNER */
		rbtype_t rbbuff [2] = { 0 };
#endif /* WITHAUTOTUNER */

	/* регистр управления (74HC595), расположенный дальше от процессора */
	RBBIT(017, glob_tx);				/* pin 07: d7: TX2 */
	RBBIT(016, glob_tx ? glob_txcw : glob_filter);			/* pin 06: d6: CW - на приёме - НЧ фильтр. */
	RBBIT(015, glob_att);				/* pin 05: d5: ATT */
	RBBIT(014, glob_preamp);			/* pin 04: d4: PRE */
	RBBIT(013, glob_filter);			/* pin 03: d3: NAR - включение узкополосного фильтра по НЧ */
	RBBIT(012, glob_bandf >= glob_bandfonuhf);				/* pin 02: d2: UKV */
	RBBIT(011, glob_mikemute);		/* pin 01: d1: MUTE */
	RBBIT(010, WITHPOWERTRIMMAX - glob_opowerlevel);			/* pin 15: "1" - low power */

	/* регистр управления (74HC595), расположенный ближе к процессору */
	RBVAL(004, glob_bandf, 4);	/* pin 04..pin 07 - band selection d0..d7 */
	RBBIT(003, glob_nfm);				/* pin 03 - включение режима NFM */
	RBBIT(002, glob_nfmnbon);			/* pin 02 - "0" - отключение шумоподавителя */
	RBBIT(001, glob_bandf >= glob_bandfonhpf);		/* pin 01 - bnd2 signal */
	RBBIT(000, glob_antenna);		/* pin 15: in control register - SPARE, antenna for UY5UM */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGSTYLE_SW2013RDX_UY5UM	/* с регулировкой мощности R-2R на сигналах выбора диапазонного фильтра */
// SW2013SF с индикатором RDX154
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

#if WITHWFM
	rbtype_t rbbuff [3] = { 0 };
	/* регистр управления (74HC595), дополнительный. */
	RBBIT(027, glob_af_input == BOARD_DETECTOR_WFM);	/* pin 07: d7: WFM mode */
	RBVAL(021, 0x00, 6);								/* d6..d1: spare */
	RBBIT(020, glob_lcdreset);			/* pin 15: d0: lctl0 */
#else
	rbtype_t rbbuff [2] = { 0 };
#endif /* WITHWFM */
	/* регистр управления (74HC595), расположенный на плате трансивера */
	RBBIT(017, glob_tx);				/* pin 07: d7: TX2 */
	RBBIT(016, glob_tx ? glob_txcw : glob_filter);			/* pin 06: d6: CW - на приёме - НЧ фильтр. */
	RBBIT(015, glob_att);				/* pin 05: d5: ATT */
	RBBIT(014, glob_preamp);			/* pin 04: d4: PRE */
	RBBIT(013, glob_filter);			/* pin 03: d3: NAR - включение узкополосного фильтра по НЧ */
	RBBIT(012, glob_lo1scale != 1);				/* pin 02: d2: UKV */
	RBBIT(011, glob_mikemute);		/* pin 01: d1: MUTE */
	//RBBIT(010, WITHPOWERTRIMMAX - glob_opowerlevel);			/* pin 15: "1" - low power */

	/* регистр управления (74HC595), расположенный на плате синтезатора */
	RBVAL(004, (glob_opowerlevel - WITHPOWERTRIMMIN), 4);		/* pin 04..pin 07 - power level d0..d7 */
	RBVAL(002, (glob_bglight - WITHLCDBACKLIGHTMIN), 2);	/* зшт 02, pin 03 - lcd backlight */
	RBBIT(001, glob_bandf >= glob_bandfonhpf);		/* pin 01 - bnd2 signal */
	RBBIT(000, ! glob_reset_n);		/* pin 15: in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGSTYLE_SW2013RDX_LTIYUR
// SW2013SF с индикатором RDX154
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

	rbtype_t rbbuff [3] = { 0 };

	/* дополнительный регистр LTIYUR */
	RBBIT(024, (glob_agc == BOARD_AGCCODE_OFF));			/* pin 04: AGC off */
	RBBIT(022, glob_loudspeaker_off);	/* pin 02: Speaker off */
	RBBIT(021, glob_notchnarrow);		/* pin 01: notch in CW */
	RBBIT(020, glob_notch);				/* pin 15: notch */

	/* регистр управления (74HC595), расположенный на плате трансивера */
	RBBIT(017, glob_tx);				/* pin 07: d7: TX2 */
	RBBIT(016, glob_tx ? glob_txcw : glob_filter);			/* pin 06: d6: CW - на приёме - НЧ фильтр. */
	RBBIT(015, glob_att);				/* pin 05: d5: ATT */
	RBBIT(014, glob_preamp);			/* pin 04: d4: PRE */
	RBBIT(013, glob_filter);			/* pin 03: d3: NAR - включение узкополосного фильтра по НЧ */
	RBBIT(012, glob_lo1scale != 1);				/* pin 02: d2: UKV */
	RBBIT(011, glob_mikemute);		/* pin 01: d1: MUTE */
	RBBIT(010, WITHPOWERTRIMMAX - glob_opowerlevel);			/* pin 15: "1" - low power */

	/* регистр управления (74HC595), расположенный на плате синтезатора */
	RBVAL(004, (glob_bandf - 0), 4);	/* pin 04..pin 07 - band selection d0..d7 */
	RBVAL(002, (glob_bglight - WITHLCDBACKLIGHTMIN), 2);	/* зшт 02, pin 03 - lcd backlight */
	RBBIT(001, glob_bandf >= glob_bandfonhpf);		/* pin 01 - bnd2 signal */
	RBBIT(000, ! glob_reset_n);		/* pin 15: in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGSTYLE_SW2011 //  плата с ATMega, 32 bits
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// SW2011RDX MAXI (classic)
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

#if WITHWFM
	rbtype_t rbbuff [3] = { 0 };
	/* регистр управления (74HC595), дополнительный. */
	RBBIT(027, glob_af_input == BOARD_DETECTOR_WFM);	/* pin 07: d7: WFM mode */
	RBVAL(021, 0x00, 6);								/* d6..d1: spare */
	RBBIT(020, glob_lcdreset);			/* pin 15: d0: lctl0 */
#else
	rbtype_t rbbuff [2] = { 0 };
#endif /* WITHWFM */
	/* регистр управления (74HC595), расположенный на плате трансивера */
	RBBIT(017, glob_tx);				/* pin 07: d7: TX2 */
	RBBIT(016, glob_tx ? glob_txcw : glob_filter);			/* pin 06: d6: CW - на приёме - НЧ фильтр. */
	RBBIT(015, glob_att);				/* pin 05: d5: ATT */
	RBBIT(014, glob_preamp);			/* pin 04: d4: PRE */
	RBBIT(013, glob_filter);			/* pin 03: d3: NAR - включение узкополосного фильтра по НЧ */
	RBBIT(012, glob_lo1scale != 1);				/* pin 02: d2: UKV */
	RBBIT(011, glob_mikemute);		/* pin 01: d1: MUTE */
	RBBIT(010, glob_lcdreset);			/* pin 15: lctl0 - reset на индикаторы, подключённые на SPI */

	/* регистр управления (74HC595), расположенный на плате синтезатора */
	RBVAL(004, (glob_bandf - 0), 4);		/* d4..d7 - band selection d0..d7 */
	RBVAL(002, (glob_bglight - WITHLCDBACKLIGHTMIN), 2);		/* d2..d3 - lcd backlight */
	RBBIT(001, glob_bandf >= glob_bandfonhpf);		/* d1 - bnd2 signal */
	RBBIT(000, ! glob_reset_n);		/* d0 in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGSTYLE_SW2011N //  плата с ATMega, 32 bits
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// SW2011RDX MAXI (classic)
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

#if WITHWFM
	rbtype_t rbbuff [3] = { 0 };
	/* регистр управления (74HC595), дополнительный. */
	RBBIT(027, glob_af_input == BOARD_DETECTOR_WFM);	/* pin 07: d7: WFM mode */
	RBVAL(021, 0x00, 6);								/* d6..d1: spare */
	RBBIT(020, glob_lcdreset);			/* pin 15: d0: lctl0 */
#else
	rbtype_t rbbuff [2] = { 0 };
#endif /* WITHWFM */
	/* регистр управления (74HC595), расположенный на плате трансивера */
	RBBIT(017, glob_tx);				/* pin 07: d7: TX2 */
	RBBIT(016, glob_tx ? glob_txcw : (glob_filter != BOARD_FILTER_2P7));			/* pin 06: d6: CW - на приёме - НЧ фильтр. */
	RBBIT(015, glob_att);				/* pin 05: d5: ATT */
	RBBIT(014, glob_preamp);			/* pin 04: d4: PRE */
	RBBIT(013, glob_filter != BOARD_FILTER_2P7);			/* pin 03: d3: NAR - включение узкополосного фильтра по НЧ */
	RBBIT(012, glob_lo1scale != 1);				/* pin 02: d2: UKV */
	RBBIT(011, glob_mikemute);		/* pin 01: d1: MUTE */
	RBBIT(010, glob_lcdreset);			/* pin 15: lctl0 - reset на индикаторы, подключённые на SPI */

	/* регистр управления (74HC595), расположенный на плате синтезатора */
	RBVAL(004, (glob_bandf - 0), 4);		/* d4..d7 - band selection d0..d7 */
	RBVAL(002, (glob_bglight - WITHLCDBACKLIGHTMIN), 2);		/* d2..d3 - lcd backlight */
	RBBIT(001, glob_bandf >= glob_bandfonhpf);		/* d1 - bnd2 signal */
	RBBIT(000, ! glob_reset_n);		/* d0 in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE24_UA3DKC
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */
// 24-bit control register for down-conversion UA3DKC
/*
Если все это удастся реализовать, то на вых. 595 мы будем иметь
 {а их у нас будет 24, если отказаться от сигнала DDS-RESET, который в данном проекте не используется}:
 1. 10 вых сигналов переключения диапазонов.
 2. 4 вых сигнала переключения КВФ ПРМ - 0.7, 1.8, 2.7, 3.1 кгц.
 3. 2 вых сигнала переключения КВФ ПРД - 2.4, 3.1 кгц.
 4. 3 вых сигнала переключения AGC - выкл, медл,быстр.
 5. 1 вых сигнала включения PRE.
 6. 2 вых сигнала включения АТТ - 10 дб, 20 дб.
 7 1 вых сигнал включения режима CW/CWR.
 8. 1 вых сигнал включения TX в режиме CW/AM.
 { 7 и 8 пункты включаются также при режиме TUNE}  Здесь отказываемся от
	вых сигнала включения AM/DRM - не будет использоваться и для экономии выходов от сигнала
	включения TX, так как этот сигнал мы имеем на выходе контроллера.
	Итого 24 выхода будут полностью задействованы.
	Что и на каких выходах 595-ых будет присутствовать ни какой разницы нет.
*/

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated
	const uint_fast16_t bandmask = (1U << glob_bandf);

	rbtype_t rbbuff [5] = { 0 };

	/* Управление согласующим устройством */
	RBVAL8(040, revbits8(glob_tuner_L));		/* Inductors tuner bank 	*/
	RBVAL8(030, revbits8(glob_tuner_C));		/* Capacitors tuner bank 	*/

	RBBIT(027, glob_tuner_type);		/* pin 07: TYPE OF TUNER 	*/
	RBBIT(026, glob_loudspeaker_off);	/* pin 06: выключение динамика		*/
	RBBIT(025, (glob_agc == BOARD_AGCCODE_OFF) || glob_tx);/* pin 05: agc off			*/
	RBVAL(023, glob_att, 2);			/* pin 04, 05: 10 dB ATTENUATOR RELAYS POWER */
	RBBIT(022, glob_preamp);			/* pin 03: D1: pin 01: RF amplifier */
	RBBIT(021, glob_tx);				/* pin 01: D0: pin 15: TX mode: 1 - TX режим передачи */
	RBBIT(020, (bandmask >> 9) & 0x01);			// pin 15: D6..D7: pin 06-07: band select band8..band9

	RBVAL(010, bandmask >> 1, 8);				/* pin 15, band select band0..band7 */

	/* регистр управления (74HC595), расположенный на плате синтезатора */
	RBBIT(007, glob_mikemute);			/* pin 07:  */
	RBBIT(006, glob_tx && glob_txcw);	/* pin 06: 	*/
	RBBIT(005, glob_autotune);			/* pin 5: tune mode */
	RBVAL(000, glob_filter, 5);			/* pin 15, 01, 02, 03: 	RX IF FILTER (low 4 bits), pin , 4, TX IF filter (bit 5). */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE24
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// 24-bit control register for down-conversion and direct-conversion RX
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

	rbtype_t rbbuff [3] = { 0 };

	/* учет диапазона - на 0-м диапазоне обход УВЧ включается принудительно */

	RBVAL(024, glob_bandf, 4);			// D4..D7: pin 04-07: band select код выбора диапазонного фильтра
	RBVAL(022, glob_att, 2);			/* D3:D2: pin 03:02: 10 dB ATTENUATOR RELAYS POWER */
	RBBIT(021, ! glob_tx && glob_preamp && (glob_bandf != 0));	/* D1: pin 01: RF amplifier */
	RBBIT(020, glob_tx);							/* D0: pin 15: TX mode: 1 - TX режим передачи */

	RBVAL(010, 0x00, 8);				/* all spare */

	/* регистр управления (74HC595), расположенный на плате синтезатора */
	RBVAL(006, glob_tx ? BOARD_DETECTOR_MUTE : glob_af_input, 2);	/* pin 06-07: AF input selection 0-ssb, 1-am, 2-mute, 3-fm */
	RBBIT(005, ! glob_if4lsb);		/* pin 05: For DC rx: LSB mode */
	RBBIT(004, glob_tx && glob_txcw);	/* pin 04: */
	RBBIT(003, glob_mikemute);		/* pin 03: */
	RBVAL(001, glob_filter, 2);		/* pin 01-02: und1 & und2 */
	RBBIT(000, ! glob_reset_n);		/* d0 pin 15: in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE24_RK4CI
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// Платы "Воробей" и "Колибри" - 24-bit control register - RK4CI version


static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

	// 15 uS полупериод меандра на выходе регистра (ATMega644 @ 10 MHz)
	// 28 uS в случае программного SPI.
	rbtype_t rbbuff [3] = { 0 };
	
	/* учет диапазона - на 0-м диапазоне обход УВЧ включается принудительно */
	RBVAL(024, glob_bandf, 4);			// D4..D7: band select код выбора диапазонного фильтра
	RBVAL(022, glob_att, 2);			/* D3,D2: d0 dB & 10 dB ATTENUATOR RELAYS POWER */
	RBBIT(021, glob_preamp && (glob_bandf != 0));	/* D1: pin 01: RF amplifier */
	RBBIT(020, glob_tx);							/* D0: pin 15: TX mode: 1 - TX режим передачи */

	RBVAL(016, glob_tx ? BOARD_DETECTOR_MUTE : glob_af_input, 2);	/* AF input selection 0-ssb, 1-am, 2-mute, 3-fm */
	RBBIT(015, glob_mikemute);				/* D5 */
	RBBIT(014, glob_tx && glob_txcw);		/* D4 */
	RBVAL(012, glob_agc, 2);				/* D2..D3: 02 03: AGC time */
	RBVAL(010, glob_filter, 2);				/* D0..D1: 15 01: und1 & und2 - номер фильтра ПЧ */

	/* регистр управления (74HC595), расположенный на плате синтезатора */
	RBBIT(007, glob_lcdreset);		// D7 LCD_RESET_PIO
	RBBIT(006, 0x00);				/* D6 in control register - spare bit */
	RBBIT(005, 0x00);				/* D5 in control register - spare bit */
	RBBIT(004, glob_loudspeaker_off);		// d4 выход LSCTL0 (вывод 15 на 34-выводном разъёме) - Отключение динамика
	RBBIT(003, glob_kblight && (glob_bglight != WITHLCDBACKLIGHTMIN));	/* d3 keyboard backlight */
	RBVAL(001, glob_bglight, 2);	/* d1..d2 LCD backlight */
	RBBIT(000, ! glob_reset_n);		/* d0 in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE24_RK4CI_V1	/* управляющий регистр - "Воробей-3" с 3*ULN2003 */
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// Платы "Воробей-3" - 32-bit control register - вариант с позиционными кодами выбора диапазона, фильтра ПЧ и режима работы

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	rbtype_t rbbuff [4] = { 0 };
	const uint_fast16_t bandmask = (1U << glob_bandf);
	
	// DD10
	RBVAL(030, bandmask >> 2, 8);			/* D0..D7: band 2.. band 9 outputs */
	// DD9
	RBVAL(026, bandmask, 2);				/* D6..D7: band select код выбора диапазонного фильтра */
	RBVAL(024, glob_att, 2);				/* D4,D5: d0 dB & 10 dB ATTENUATOR RELAYS POWER */
	RBBIT(023, glob_preamp);				/* D3: RF amplifier */
	RBBIT(022, glob_tx);							/* D2: pin 15: TX mode: 1 - TX режим передачи */
	RBBIT(021, glob_tx && glob_txcw);		/* D1 */
	RBBIT(020, glob_mikemute);				/* D0 */
	// DD8
	RBVAL(016, glob_af_input, 2);			/* d6,d7: AF input selection 0-ssb, 1-am, 2-mute, 3-fm */
	RBVAL(014, glob_agc, 2);				/* D4..D5: 02 03: AGC time */
	RBVAL(012, glob_filter, 2);				/* D2..D3: - номер фильтра ПЧ */
	RBBIT(011, 0x00);						/* D1: LSCTL5 */
	RBBIT(010, 0x00);						/* D0: LSCTL4 */
	// DD7
	RBBIT(007, 0x00);						/* D7: LSCTL3 */
	RBBIT(006, 0x00);						/* D6: LSCTL2 */
	RBBIT(005, 0x00);						/* D5: LSCTL1 */
	RBBIT(004, 0x00);						/* D4: LSCTL0 */
	RBBIT(003, glob_kblight && (glob_bglight != WITHLCDBACKLIGHTMIN));	/* d3 keyboard backlight */
	RBVAL(001, glob_bglight, 2);			/* d1..d2 LCD backlight */
	RBBIT(000, ! glob_reset_n);			/* d0 in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE24_V1
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// Платы "Воробей" и "Колибри" - 24-bit control register

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

	// 15 uS полупериод меандра на выходе регистра (ATMega644 @ 10 MHz)
	// 28 uS в случае программного SPI.
	rbtype_t rbbuff [3] = { 0 };
	
	/* учет диапазона - на 0-м диапазоне обход УВЧ включается принудительно */
	RBVAL(024, glob_bandf, 4);			// D4..D7: band select код выбора диапазонного фильтра
	RBVAL(022, ! glob_tx && glob_att, 2);			/* D3,D2: d0 dB & 10 dB ATTENUATOR RELAYS POWER */
	RBBIT(021, ! glob_tx && glob_preamp && (glob_bandf != 0));	/* D1: pin 01: RF amplifier */
	RBBIT(020, glob_tx);							/* D0: pin 15: TX mode: 1 - TX режим передачи */

#if 1
	RBVAL(016, glob_tx ? BOARD_DETECTOR_MUTE : glob_af_input, 2);	/* AF input selection 0-ssb, 1-am, 2-mute, 3-fm */
	#if WITHDUALBFO || (defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_DCRX))
		RBBIT(015, glob_if4lsb);		/*  D5: For DC rx: LSB mode */
	#else
		RBBIT(015, 0x00);		/* D5: reserved */
	#endif
	RBBIT(014, glob_tx && glob_txcw);	/* */
	RBBIT(013, glob_mikemute);		/* */
	#if WITHAGCMODESLOWFAST
		RBBIT(012, glob_agc);			/* D2: AGC time */
	#elif WITHAGCMODEONOFF
		RBBIT(012, (glob_agc == BOARD_AGCCODE_OFF));			/* D2: AGC OFFs */
	#elif WITHAGCMODENONE
		RBBIT(012, 0);			/* D2: spare */
	#else
		#error WITHAGCMODExxx undefined
	#endif
	RBVAL(010, glob_filter, 2);				/* D0,D1: 15 01: und1 & und2 - номер фильтра ПЧ */
#else
	// eugene.zhebrakoff@gmail.com
	RBVAL(015, glob_agc, 3);		/* AGC delay time */
	RBBIT(014, glob_tx && glob_txcw);	/* TX */
	RBBIT(013, glob_bandf >= 5);			/* PA - bnd2 - signal * und4 */
	RBBIT(012, glob_affilter);			/* Notch Filtr on-off * und3 */
	RBVAL(010, glob_filter, 2);			/* SSB - CW Filtr * und1 & und2 */
#endif

	/* регистр управления (74HC595), расположенный на плате синтезатора */
	RBBIT(007, glob_lcdreset);		// d7 LCD_RESET_PIO
	RBBIT(006, 0x00);				/* d6 in control register - spare bit */
	RBBIT(005, glob_lctl1);			// d5 lctl1 - есть только в версии "колибри"
	RBBIT(004, glob_lcdreset);		// d4 lctl0
	RBBIT(003, glob_kblight && (glob_bglight != WITHLCDBACKLIGHTMIN));	/* d3 keyboard backlight */
	RBVAL(001, glob_bglight, 2);	/* d1..d2 LCD backlight */
	RBBIT(000, ! glob_reset_n);		/* d0 in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE32_V1
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// Платы "Воробей-3" - 32-bit control register - вариант с позиционными кодами выбора диапазона, фильтра ПЧ и режима работы

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	rbtype_t rbbuff [4] = { 0 };
	const uint_fast16_t bandmask = (1U << glob_bandf);
	
	// DD10
	RBVAL(030, bandmask >> 2, 8);			/* D0..D7: band 2.. band 9 outputs */
	// DD9
	RBVAL(026, bandmask, 2);				/* D6..D7: band select код выбора диапазонного фильтра */
	RBVAL(024, glob_att, 2);				/* D4,D5: d0 dB & 10 dB ATTENUATOR RELAYS POWER */
	RBBIT(023, ! glob_tx && glob_preamp);	/* D3: RF amplifier */
	RBBIT(022, glob_tx);							/* D2: pin 15: TX mode: 1 - TX режим передачи */
	RBBIT(021, glob_tx && glob_txcw);		/* D1 */
	RBBIT(020, glob_mikemute);				/* D0 */
	// DD8
	RBVAL(016, glob_af_input, 2);			/* d6,d7: AF input selection 0-ssb, 1-am, 2-mute, 3-fm */
	RBBIT(015, (glob_agc == BOARD_AGCCODE_OFF));				/* D5: AGC OFF */
	RBVAL(012, glob_filter, 3);				/* d2,d3,d4: фильтр ПЧ */
	RBBIT(011, 0x00);						/* D1: LSCTL5 */
	RBBIT(010, 0x00);						/* D0: LSCTL4 */
	// DD7
	RBBIT(007, 0x00);						/* D7: LSCTL3 */
	RBBIT(006, 0x00);						/* D6: LSCTL2 */
	RBBIT(005, 0x00);						/* D5: LSCTL1 */
	RBBIT(004, 0x00);						/* D4: LSCTL0 */
	RBBIT(003, glob_kblight && (glob_bglight != WITHLCDBACKLIGHTMIN));	/* d3 keyboard backlight */
	RBVAL(001, glob_bglight, 2);			/* d1..d2 LCD backlight */
	RBBIT(000, ! glob_reset_n);			/* d0 in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE24_V2
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// Платы "Воробей-2" с кодеком - 24-bit control register

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

	// 15 uS полупериод меандра на выходе регистра (ATMega644 @ 10 MHz)
	// 28 uS в случае программного SPI.
	rbtype_t rbbuff [3] = { 0 };
	
	/* учет диапазона - на 0-м диапазоне обход УВЧ включается принудительно */
	RBVAL(024, glob_bandf, 4);			// D4..D7: band select код выбора диапазонного фильтра
	RBVAL(022, glob_att, 2);			/* D3:D2: 20,10 dB ATTENUATOR RELAYS POWER */
	RBBIT(021, ! glob_tx && glob_preamp && (glob_bandf != 0));	/* D1: pin 01: RF amplifier */
	RBBIT(020, glob_tx);							/* D0: pin 15: TX mode: 1 - TX режим передачи */

	RBVAL(016, glob_tx ? BOARD_DETECTOR_MUTE : glob_af_input, 2);	/* AF input selection 0-ssb, 1-am, 2-mute, 3-fm */
	RBBIT(015, glob_if4lsb);		/* For DC rx: LSB mode */
	RBBIT(014, glob_tx && glob_txcw);	/* */
	RBBIT(013, glob_mikemute);		/* */
	RBBIT(012, glob_affilter);		/* Notch Filtr on-off * und3 */
	RBVAL(010, glob_filter, 2);				/* und1 & und2 */

	/* регистр управления (74HC595), расположенный на плате синтезатора */
	RBBIT(007, glob_lcdreset);		// d7 local LCD reset
	RBBIT(006, 1);		// d6 0 - разрешает ioupdate от кодека
	RBBIT(005, 0);	// d5 was: #CODEC_DOE: 0 - enable data output of codec
	RBBIT(004, glob_lcdreset);		// d4 lctl0 - external LCD reset
	RBBIT(003, glob_kblight && (glob_bglight != WITHLCDBACKLIGHTMIN));	/* d3 keyboard backlight */
	RBVAL(001, glob_bglight, 2);	/* d1..d2 LCD backlight */
	RBBIT(000, ! glob_reset_n);		/* d0 in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE24_IGOR
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// Платы "Воробей" и "Колибри" - 24-bit control register
// Вариант для Игоря

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

	// 15 uS полупериод меандра на выходе регистра (ATMega644 @ 10 MHz)
	// 28 uS в случае программного SPI.
	rbtype_t rbbuff [3] = { 0 };
	const div_t a = div(glob_bcdfreq, 10);
	const div_t b = div(a.quot, 10);
	
	RBVAL(22, b.quot, 2);			// D7,D6: x10 MHz
	RBVAL(18, b.rem, 4);			// D5..D2: x1 MHz
	RBVAL(14, a.rem, 4);			// D1,d0 & D7,D6: x100 kHz

	RBVAL(014, glob_att, 2);			/* D5,D4: 20 dB, 10 dB ATTENUATOR RELAYS POWER */
	RBBIT(013, glob_preamp);			/* D3: pin 01: RF amplifier */
	RBBIT(012, glob_if4lsb);			/* D2: For DC rx: LSB mode */
	RBVAL(010, glob_filter, 2);		/* D1,D0: IF filter code */

	/* регистр управления (74HC595), расположенный на плате синтезатора */
	RBVAL(006, 0x00, 2);	/* d6..d7 in control register - spare bits */
	RBBIT(005, glob_lctl1);		// d5 lctl1 - есть только в версии "колибри"
	RBBIT(004, glob_lcdreset);		// d4 lctl0
	RBBIT(003, glob_kblight && (glob_bglight != WITHLCDBACKLIGHTMIN));	/* d3 keyboard backlight */
	RBVAL(001, glob_bglight, 2);	/* d1..d2 LCD backlight */
	RBBIT(000, ! glob_reset_n);		/* d0 in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE16_GEN500
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// 16-bit control register

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	rbtype_t rbbuff [2] = { 0 };


	RBBIT(017, 0x01);		// ADC RESET
	RBVAL(015, 0x03, 2);	// ADC SMODE1, SMODE0
	RBBIT(014, 0x01);		// ADC HPFE

	RBVAL(012, 0x03, 2);		// ADC DFS1, DFS0
	RBBIT(011, 0x01);		// ADC ZCAL
	RBBIT(010, 0x01);		// CML reset	

	RBVAL(001, 0xff, 7);	// spare
	RBBIT(000, 0x01);		// DAC reset	

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE_RA4YBO

#define BOARD_NPLANES	2	/* в данной конфигурации присутствует цифровой потенциометр со "слоями" */

// 24-bit control register + DAC for RA4YBO
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	const uint_fast8_t wfm = glob_af_input == BOARD_DETECTOR_WFM;	// WFM mode activated
	const uint_fast8_t ssb = glob_af_input == BOARD_DETECTOR_SSB;	// SSB mode activated
	const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

	rbtype_t rbbuff [5] = { 0 };

	/* IC17 AD5262 */
	RBNULL(041, 7);					/* для выравнивания */
	RBBIT(040, plane);			/* DAC target */

	RBVAL(030, glob_dac1value [plane], 8);		/* DAC value */

	/* IC15 74HC595 */
	/* учет диапазона - на 0-м диапазоне обход УВЧ включается принудительно */
	RBVAL(024, glob_bandf, 4);			// D4..D7: pin 4 5 6 7 band select код выбора диапазонного фильтра
	RBVAL(022, glob_att, 2);			/* D3:D2: pin 3,2 20,10 dB ATTENUATOR RELAYS POWER */
	RBBIT(021, glob_preamp && (glob_bandf != 0));	/* D1: pin 01: RF amplifier */
	RBBIT(020, glob_tx);							/* D0: pin 15: TX mode: 1 - TX режим передачи */

	/* IC14 74HC595 */
	RBVAL(016, glob_agc, 2);	/* D7..D6:  AGC code (delay) */
	RBBIT(015, glob_affilter);	/* D5 */
	RBVAL(013, glob_af_input, 2);	/* D4..D3	*/
	RBVAL(010, glob_filter, 3);	/* D2..D0 - IF filters code	*/

	/* IC13 74HC595 */
	RBBIT(007, glob_mikemute);		/* D7 */
	RBBIT(006, 0x00);		/* D6 - und3*/
	RBBIT(005, wfm);		/* D5 */
	RBBIT(004, fm);		/* D4 */
	RBBIT(003, am);		/* D3 */
	RBBIT(002, ssb);		/* D2 */
	RBBIT(001, 0x00);		/* D1 - und2 */
	RBBIT(000, 0x00);		/* D0 - und1 */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE_RA4YBO_V1

#define BOARD_NPLANES	2	/* в данной конфигурации присутствует цифровой потенциометр со "слоями" */

// Разворот битов в регистре управления индуктивностью
static uint_fast8_t revbits7L(uint_fast8_t v)
{
	uint_fast8_t r = 0;

	if ((v & 0x01) != 0)	r |= (1U << 1);		// 0.1 uH
	if ((v & 0x02) != 0)	r |= (1U << 2);		// 0.2 uH	
	if ((v & 0x04) != 0)	r |= (1U << 5);		// 0.5 uH	
	if ((v & 0x08) != 0)	r |= (1U << 6);		// 1.0 uH	
	if ((v & 0x10) != 0)	r |= (1U << 4);		// 2.0 uH
	if ((v & 0x20) != 0)	r |= (1U << 3);		// 5.0 uH	
	if ((v & 0x40) != 0)	r |= (1U << 0);		// 10 uH	

	return r;
}

// 24-bit control register + DAC + tuner for RA4YBO
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	const uint_fast8_t wfm = glob_af_input == BOARD_DETECTOR_WFM;	// WFM mode activated
	const uint_fast8_t ssb = glob_af_input == BOARD_DETECTOR_SSB;	// SSB mode activated
	const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

	rbtype_t rbbuff [7] = { 0 };

	
	/* +++ Управление согласующим устройством */
	/* регистр управления наборной индуктивностью. */
	RBVAL(052, glob_tuner_bypass ? 0 : revbits7L(glob_tuner_L), 7);			/* Inductors tuner bank 	*/
	RBBIT(051, ! glob_tuner_bypass);		// pin 15: обход СУ
	/* регистр управления массивом конденсаторов */
	RBVAL(042, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_C) >> 1), 7);/* Capacitors tuner bank 	*/
	RBBIT(041, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 15: TYPE OF TUNER 	*/
	/* --- Управление согласующим устройством */

	/* IC7 AD5262 */
	RBBIT(040, plane);			/* DAC target */

	RBVAL(030, glob_dac1value [plane], 8);		/* DAC value */

	/* IC6 74HC595 */
	RBBIT(027, am);					/* D7 */
	RBBIT(026, fm);					/* D6 */
	RBBIT(025, ssb);				/* D5 */
	RBVAL(023, glob_agc, 2);		/* D7..D6: AGC code (delay) */
	RBBIT(022, glob_tx);		/* D2 AF mute */
	RBVAL(020, glob_af_input, 2);	/* D0 D1: AF input	*/

	/* IC5 74HC595 */
	RBVAL(016, glob_filter, 2);		/* D6-D7: if filter code */
	RBVAL(012, glob_bandf, 4);		// D2..D5: pin 02 03 04 05 band select код выбора диапазонного фильтра
	RBVAL(010, glob_tx ? 0 : glob_att, 2);		/* D1:D0: pin 01,15 20,10 dB ATTENUATOR RELAYS POWER */

	/* IC4 74HC595 */
	RBBIT(007, glob_tx ? 0 : glob_preamp);	/* D7: pin 07: RF amplifier */
	RBBIT(006, ! wfm);			/* D6: pin 06: wfm */
	RBBIT(005, glob_tx);		/* D5: pin 05: tx mode */
	RBBIT(004, glob_bandf >= glob_bandfonuhf);	/* D4: pin 04: частота больше 111 МГц */
	RBBIT(003, glob_user4);		/* D3: pin 03: und4 */
	RBBIT(002, glob_user3);		/* D2: pin 02: und3 */
	RBBIT(001, glob_user2);		/* D1: pin 01: und2 */
	RBBIT(000, glob_user1);		/* D0: pin 15: und1 */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}


#elif CTLREGMODE_RA4YBO_V2

#define BOARD_NPLANES	2	/* в данной конфигурации присутствует цифровой потенциометр со "слоями" */

// 24-bit control register + DAC for RA4YBO
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	const uint_fast8_t wfm = glob_af_input == BOARD_DETECTOR_WFM;	// WFM mode activated
	const uint_fast8_t ssb = glob_af_input == BOARD_DETECTOR_SSB;	// SSB mode activated
	const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

	rbtype_t rbbuff [5] = { 0 };

	/* IC7 AD5262 */
	RBNULL(041, 7);					/* для выравнивания */
	RBBIT(040, plane);			/* DAC target */

	RBVAL(030, glob_dac1value [plane], 8);		/* DAC value */

	/* IC6 74HC595 */
	RBBIT(027, am);					/* D7 */
	RBBIT(026, fm);					/* D6 */
	RBBIT(025, ssb);				/* D5 */
	RBVAL(023, glob_agc, 2);		/* D7..D6: AGC code (delay) */
	RBBIT(022, glob_af_input == BOARD_DETECTOR_MUTE);		/* D2 */
	RBVAL(020, glob_af_input, 2);	/* D0 D1: AF input	*/

	/* IC5 74HC595 */
	RBVAL(016, glob_filter, 2);		/* D6-D7: if filter code */
	RBVAL(012, glob_bandf, 4);		// D2..D5: pin 02 03 04 05 band select код выбора диапазонного фильтра
	RBVAL(010, glob_tx ? 0 : glob_att, 2);		/* D1:D0: pin 01,15 20,10 dB ATTENUATOR RELAYS POWER */

	/* IC4 74HC595 */
	RBBIT(007, glob_tx ? 0 : glob_preamp);	/* D7: pin 07: RF amplifier */
	RBBIT(006, ! wfm);			/* D6: pin 06: wfm */
	RBBIT(005, glob_tx);		/* D5: pin 05: tx mode */
	RBBIT(004, glob_bandf >= glob_bandfonuhf);	/* D4: pin 04: частота больше 111 МГц */
	RBBIT(003, glob_user4);		/* D3: pin 03: und4 */
	RBBIT(002, glob_user3);		/* D2: pin 02: und3 */
	RBBIT(001, glob_user2);		/* D1: pin 01: und2 */
	RBBIT(000, glob_user1);		/* D0: pin 15: und1 */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE_RA4YBO_V3

#define BOARD_NPLANES	2	/* в данной конфигурации присутствует цифровой потенциометр со "слоями" */

// 24-bit control register + DAC for RA4YBO
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	const uint_fast8_t wfm = glob_af_input == BOARD_DETECTOR_WFM;	// WFM mode activated
	const uint_fast8_t ssb = glob_af_input == BOARD_DETECTOR_SSB;	// SSB mode activated
	const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

	rbtype_t rbbuff [5] = { 0 };

	/* IC7 AD5262 */
	RBNULL(041, 7);					/* для выравнивания */
	RBBIT(040, plane);			/* DAC target */

	RBVAL(030, glob_dac1value [plane], 8);		/* DAC value */

	/* IC6 74HC595 */
	RBBIT(027, am);					/* D7 */
	RBBIT(026, fm);					/* D6 */
	RBBIT(025, ssb);				/* D5 */
	RBVAL(023, glob_agc, 2);		/* D7..D6: AGC code (delay) */
	RBBIT(022, glob_af_input == BOARD_DETECTOR_MUTE);		/* D2 */
	RBVAL(020, glob_af_input, 2);	/* D0 D1: AF input	*/

	/* IC5 74HC595 */
	RBVAL(016, glob_filter, 2);		/* D6-D7: if filter code */
	RBVAL(012, glob_bandf, 4);		// D2..D5: pin 02 03 04 05 band select код выбора диапазонного фильтра
	RBVAL(010, glob_tx ? 0 : glob_att, 2);		/* D1:D0: pin 01,15 20,10 dB ATTENUATOR RELAYS POWER */

	/* IC4 74HC595 */
	RBBIT(007, glob_tx ? 0 : glob_preamp);	/* D7: pin 07: RF amplifier */
	RBBIT(006, ! wfm);			/* D6: pin 06: wfm */
	RBBIT(005, glob_tx);		/* D5: pin 05: tx mode */
	RBBIT(004, glob_bandf >= glob_bandfonuhf);	/* D4: pin 04: частота больше 111 МГц */
	RBBIT(003, glob_user4);		/* D3: pin 03: und4 */
	RBBIT(002, glob_user3);		/* D2: pin 02: und3 */
	RBBIT(001, glob_user2);		/* D1: pin 01: und2 */
	RBBIT(000, glob_user1);		/* D0: pin 15: und1 */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE_RA4YBO_V3A
// Новый вариант - без HMC830 с двумя ADG714

#define BOARD_NPLANES	2	/* в данной конфигурации присутствует цифровой потенциометр со "слоями" */

// 24-bit control register + DAC for RA4YBO
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	const uint_fast8_t wfm = glob_af_input == BOARD_DETECTOR_WFM;	// WFM mode activated
	const uint_fast8_t ssb = glob_af_input == BOARD_DETECTOR_SSB;	// SSB mode activated
	const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

	const uint_fast8_t filtercodein = 0xFF & (glob_filter / 256);
	const uint_fast8_t filtercodeout = 0xFF & glob_filter;

	const uint_fast8_t L10uH = ! glob_tuner_bypass && (glob_tuner_L & 0x80) != 0;
	const uint_fast8_t L5uH = ! glob_tuner_bypass && (glob_tuner_L & 0x40) != 0;
	const uint_fast8_t L2p5uH = ! glob_tuner_bypass && (glob_tuner_L & 0x20) != 0;
	const uint_fast8_t L1p25uH = ! glob_tuner_bypass && (glob_tuner_L & 0x10) != 0;
	const uint_fast8_t L650nH = ! glob_tuner_bypass && (glob_tuner_L & 0x08) != 0;
	const uint_fast8_t L300nH = ! glob_tuner_bypass && (glob_tuner_L & 0x04) != 0;
	const uint_fast8_t L150nH = ! glob_tuner_bypass && (glob_tuner_L & 0x02) != 0;
	const uint_fast8_t L80nH = ! glob_tuner_bypass && (glob_tuner_L & 0x01) != 0;

	const uint_fast8_t cap7 = revbits8(glob_tuner_C >> 1) >> 1;

	rbtype_t rbbuff [10] = { 0 };

	// +++ tuner registers

	RBBIT(0116, L10uH);			// IC3 pin 03
	RBBIT(0115, L5uH);			// IC3 pin 02
	RBBIT(0114, L2p5uH);		// IC3 pin 02
	RBBIT(0113, L1p25uH);		// IC3 pin 01
	//RBBIT(0112, 0);			// IC3 pin 15

	RBBIT(0111, L650nH);				// IC2 pin 07
	RBBIT(0110, L300nH);				// IC2 pin 06
	RBBIT(0107, L150nH);				// IC2 pin 05
	RBBIT(0106, L80nH);					// IC2 pin 04
	RBBIT(0105, glob_antenna);			// IC2 pin 03 yag
	RBBIT(0104, ! glob_tuner_bypass && glob_tuner_type);		// IC2 pin 02 typ
	RBBIT(0103, ! glob_tuner_bypass && (glob_tuner_C & 0x01) != 0);	// IC2 pin 01 5pF
	//RBBIT(0102, 0);			// IC2 pin 15

	RBVAL(0073, ! glob_tuner_bypass ? cap7 : 0, 7);	// IC1 D1..D7: capacitors
	//RBBIT(0072, 0);			// IC1 pin 15

	// -- tuner registers

	// IC2 ADG714 - outputs of IF filters selection
	RBVAL(0062, filtercodeout, 8);		// D0..D7: band select бит выбора фильтра ПЧ
	// IC1 ADG714 - inputs of IF filters selection
	RBVAL(0052, filtercodein, 8);		// D0..D7: band select бит выбора фильтра ПЧ

	/* AD5262 */
	RBBIT(0050, plane);			/* DAC target */

	RBVAL(0040, glob_dac1value [plane], 8);		/* DAC value */

	/* IC7 74HC595 */
	RBBIT(0037, 0);							/* D7 UND */
	RBVAL(0035, glob_af_input, 2);					/* D5,,D6 */
	RBBIT(0034, glob_af_input == BOARD_DETECTOR_MUTE);					/* D4: pin 04: AF_MUTE */
	RBVAL(0032, glob_agc, 2);			/* D2..D3: AGC code (delay) */
	RBBIT(0011, 0);						// D1: pin 01: IF FIL2
	RBBIT(0030, 0);						// D0: pin 15: IF FIL1

	/* IC6 74HC595 */
	RBVAL(0025, glob_bandf, 3);			// D0..D2: pin 15 01 02 band select код выбора диапазонного фильтра
	RBBIT(0024, ssb);					/* D6 SSB */
	RBBIT(0023, 0x00);					/* D5 CW */
	RBBIT(0022, am);						/* D4 AM*/
	RBBIT(0021, fm);						/* D3 NFM */
	RBBIT(0020, 0);						// D0: pin 15: IF FIL0

	/* IC16 74HC595 */
	RBBIT(0017, glob_affilter);							/* D7: pin 07: AF FIL ON */
	RBBIT(0016, glob_bandf >= glob_bandfonuhf);	/* D4: pin 04: частота больше 111 МГц */
	RBVAL(0014, glob_att, 2);	/* D1:D0: pin 01,15 20,10 dB ATTENUATOR RELAYS POWER */
	RBBIT(0013, glob_tx ? 0 : glob_preamp);			/* D3: pin 03: RF_AMP_ON */
	RBBIT(0012, glob_tx);				/* D2: pin 02: TX */
	RBBIT(0011, wfm);					/* D1: pin 01: ~WFM_ON */
	RBBIT(0010, (glob_bandf & 0x08));	/* D0: pin 15: BANDF_3 */

	/* IC15 74HC595 */
	RBBIT(0007, glob_loudspeaker_off);	/* D7: pin 06: speaker */
	RBBIT(0006, 0);						/* D6: pin 05: shift */
	RBBIT(0005, glob_user5);				/* D5: pin 04: rever */
	RBBIT(0004, glob_user4);				/* D4: pin 03: equal */
	RBBIT(0003, glob_user3);				/* D3: pin 03: play */
	RBBIT(0002, glob_user2);				/* D2: pin 02: rec */
	RBBIT(0001, glob_antenna);			/* D1: pin 01: YAGI */
	RBBIT(0000, glob_tuner_bypass);		/* D0: pin 15: byp tun */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE16
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */
/*

	Выходы 595-х

	Ближе к процессору
	15 - RESET AD9951
	01 - AGC OFF
	02 - UNUSED
	03 - MUTE микрофонному усилителю
	04 - "1" - передача в режиме телеграфа (пустить выход DDS на усилитель)
	05 - LSB (CWR) только в прошивках для прямого преобразования.
	06 - 0 (выбор режимов, отличающихся от SSB/CW)
	07 - 0 (выбор режимов, отличающихся от SSB/CW)

	Дальше от процессора
	15 - режим TX
	01 - включить УВЧ
	02 - 6 дБ аттенюатор
	03 - 12 дБ аттенюатор
	04 - A0 дешифратора диапазонов
	05 - A1 дешифратора диапазонов
	06 - A2 дешифратора диапазонов
	07 - A3 дешифратора диапазонов

*/

// 16-bit control register for down-conversion and direct-conversion RX
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated
	rbtype_t rbbuff [2] = { 0 };

	/* учет диапазона - на 0-м диапазоне обход УВЧ включается принудительно */

	/* регистр управления (74HC595), дальше от процессора */
	RBVAL(014, glob_bandf, 4);				/* D4..D7: pin 4 5 6 7 band select код выбора диапазонного фильтра */
	RBVAL(012, glob_att, 2);				/* d3:D2: pin 2 10 dB ATTENUATOR RELAYS POWER */
	RBBIT(011, ! glob_tx && glob_preamp);	/* D1: pin 01: RF amplifier */
	RBBIT(010, glob_tx);					/* D0: pin 15: TX mode: 1 - TX режим передачи */

	/* регистр управления (74HC595), ближе к процессору */
	RBVAL(006, glob_tx ? BOARD_DETECTOR_MUTE : glob_af_input, 2);	/* pin 6 7 AF input selection 0-ssb, 1-am, 2-mute, 3-fm */
	RBBIT(005, glob_if4lsb);							/* pin 5 For DC rx: LSB mode */
	RBBIT(004, glob_tx && glob_txcw);				/* pin 4 */
	RBBIT(003, glob_mikemute);						/* pin 3 */
	RBBIT(002, glob_filter);						/* pin 2: 1: узкий фильтр */
	RBBIT(001, glob_agc);						/* pin 1 AGC OFF */
	RBBIT(000, ! glob_reset_n);					/* pin 15 in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}
#elif CTLREGMODE16_UR3VBM
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */
/*

	Выходы 595-х

	Ближе к процессору
	15 - RESET AD9951
	01 - AGC OFF
	02 - UNUSED
	03 - MUTE микрофонному усилителю
	04 - "1" - передача в режиме телеграфа (пустить выход DDS на усилитель)
	05 - LSB (CWR) только в прошивках для прямого преобразования.
	06 - 0 (выбор режимов, отличающихся от SSB/CW)
	07 - 0 (выбор режимов, отличающихся от SSB/CW)

	Дальше от процессора
	15 - режим TX
	01 - включить УВЧ
	02 - 6 дБ аттенюатор
	03 - 12 дБ аттенюатор
	04 - A0 дешифратора диапазонов
	05 - A1 дешифратора диапазонов
	06 - A2 дешифратора диапазонов
	07 - A3 дешифратора диапазонов

*/

// 16-bit control register for down-conversion and direct-conversion RX
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated
	rbtype_t rbbuff [2] = { 0 };

	/* учет диапазона - на 0-м диапазоне обход УВЧ включается принудительно */

	/* регистр управления (74HC595), дальше от процессора */
	RBVAL(014, glob_bandf, 4);				/* D4..D7: pin 4 5 6 7 band select код выбора диапазонного фильтра */
	RBVAL(012, glob_att, 2);				/* d3:D2: pin 2 10 dB ATTENUATOR RELAYS POWER */
	RBBIT(011, ! glob_tx && glob_preamp);	/* D1: pin 01: RF amplifier */
	RBBIT(010, glob_tx);					/* D0: pin 15: TX mode: 1 - TX режим передачи */

	/* регистр управления (74HC595), ближе к процессору */
	RBVAL(006, glob_tx ? BOARD_DETECTOR_MUTE : glob_af_input, 2);	/* pin 6 7 AF input selection 0-ssb, 1-am, 2-mute, 3-fm */
	RBBIT(005, ! glob_if4lsb);							/* pin 5 For DC rx: LSB mode */
	RBBIT(004, glob_tx && glob_txcw);				/* pin 4 */
	RBBIT(003, glob_mikemute);						/* pin 3 */
	RBBIT(002, glob_filter);						/* pin 2: 1: узкий фильтр */
	RBBIT(001, glob_agc);						/* pin 1 AGC OFF */
	RBBIT(000, ! glob_reset_n);					/* pin 15 in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE8_UR5YFV
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */
/*

	Выходы 74HC595:

	15 - режим TX
	01 - включить УВЧ
	02 - аттенюатор
	03 - "1" - передача в режиме телеграфа (пустить выход DDS на усилитель) или разбалансировка БМ.

	04 - a0 дешифратора диапазонов
	05 - a1 дешифратора диапазонов
	06 - a2 дешифратора диапазонов
	07 - a3 дешифратора диапазонов

*/

// 8-bit control register for down-conversion and direct-conversion RX
static void
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	rbtype_t rbbuff [1] = { 0 };

	/* регистр управления (74HC595) */
    RBVAL(004, glob_bandf, 4);              /* pin 4  - D4..D7: pin 4 5 6 7 band select код выбора диапазонного фильтра  */
    RBBIT(003, glob_tx && glob_txcw);       /* pin 3  - TX_CW */
    RBBIT(002, glob_att);                   /* pin 2  - ATT */
    RBBIT(001, ! glob_tx && glob_preamp);   /* pin 1  - PRE */
    RBBIT(000, glob_tx);                    /* pin 15 - TX_MODE */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE8_UA3RNB
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */
/*

	Выходы 74HC595:

15) TX
1)  AMP
2)  ATT 10db
3)  ATT 20db
4)  BANDSEL0
5)  BANDSEL1
6)  BANDSEL2
7)  BANDSEL3

Геннадий если можно сделать предыдущую мою просьбу.
Вернуть плоское меню, добавить диапазон 50Мгц и управление для него на ДПФ.

*/

// 8-bit control register for down-conversion and direct-conversion RX
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	rbtype_t rbbuff [1] = { 0 };

	/* регистр управления (74HC595) */
    RBVAL(004, glob_bandf, 4);              /* pin 4  - D4..D7: pin 4 5 6 7 band select код выбора диапазонного фильтра  */
    RBVAL(002, glob_att, 2);                /* pin 2 & 3 - ATT */
    RBBIT(001, ! glob_tx && glob_preamp);   /* pin 1  - PRE */
    RBBIT(000, glob_tx);                    /* pin 15 - TX_MODE */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE16_RN3ZOB
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */
// 16-bit control register for down-conversion and direct-conversion RX
// От CTLREGMODE16 отличается позиционным управление диапазонныии фильтрами.
// пят выходов - 160 80 40 30 20
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated
	rbtype_t rbbuff [2] = { 0 };

	/* учет диапазона - на 0-м диапазоне обход УВЧ включается принудительно */

	/* регистр управления (74HC595), дальше от процессора */
	RBVAL(013, 1U << glob_bandf, 5);				/* D3..D7: pin 3 4 5 6 7 band select биты выбора диапазонного фильтра */
	RBVAL(011, glob_att, 2);				/* d1:D2: pin 2 10 dB ATTENUATOR RELAYS POWER */
	RBBIT(010, glob_tx);					/* D0: pin 15: TX mode: 1 - TX режим передачи */

	/* регистр управления (74HC595), ближе к процессору */
	RBVAL(006, glob_tx ? BOARD_DETECTOR_MUTE : glob_af_input, 2);	/* pin 6 7 AF input selection 0-ssb, 1-am, 2-mute, 3-fm */
	RBBIT(005, glob_if4lsb);							/* pin 5 For DC rx: LSB mode */
	RBBIT(004, glob_tx && glob_txcw);				/* pin 4 */
	RBBIT(003, glob_mikemute);						/* pin 3 */
	RBBIT(002, glob_filter);						/* pin 2: 1: узкий фильтр */
	RBBIT(001, glob_agc);						/* pin 1 AGC OFF */
	RBBIT(000, ! glob_reset_n);					/* pin 15 in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}


#elif CTLREGMODE16_DC
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */
/*

	Выходы 595-х

	Ближе к процессору

	15 - RESET AD9951
	01 - "1" - НЧ фильтр SSB
	02 - "1" - НЧ фильтр CW
	03 - MUTE микрофонному усилителю
	04 - "1" - передача в режиме телеграфа (пустить выход DDS на усилитель)
	05 - LSB (CWR)
	06 - 0 (выбор режимов, отличающихся от SSB/CW)
	07 - 0 (выбор режимов, отличающихся от SSB/CW)

	Дальше от процессора

	15 - режим TX
	01 - включить УВЧ
	02 - 10 дБ аттенюатор
	03 - 20 дБ аттенюатор
	04 - a0 дешифратора диапазонов
	05 - a1 дешифратора диапазонов
	06 - a2 дешифратора диапазонов
	07 - a3 дешифратора диапазонов

*/

// 16-bit control register for down-conversion and direct-conversion RX
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	//const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	//const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated
	rbtype_t rbbuff [2] = { 0 };

	/* учет диапазона - на 0-м диапазоне обход УВЧ включается принудительно */

	/* регистр управления (74HC595), дальше от процессора */
	RBVAL(014, glob_bandf, 4);				/* D4..D7: pin 4 5 6 7 band select код выбора диапазонного фильтра */
	RBVAL(012, glob_att, 2);				/* d3:D2: pin 2 10 dB ATTENUATOR RELAYS POWER */
	RBBIT(011, ! glob_tx && glob_preamp);	/* D1: pin 01: RF amplifier */
	RBBIT(010, glob_tx);					/* D0: pin 15: TX mode: 1 - TX режим передачи */

	/* регистр управления (74HC595), ближе к процессору */
	RBBIT(007, glob_mikemute);						/* pin 7 */
	RBBIT(006, glob_tx || ( BOARD_DETECTOR_MUTE == glob_af_input));	/* pin 6: RXAF mute */
	RBBIT(005, glob_if4lsb);						/* pin 5 For DC rx: LSB mode */
	RBBIT(004, glob_tx && glob_txcw);				/* pin 4 */
	RBVAL(001, glob_filter, 3);						/* pin 1 2 3 - filter selection */
	RBBIT(000, ! glob_reset_n);					/* pin 15 in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE16_NIKOLAI
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// 16-bit control register for down-conversion and direct-conversion RX
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	rbtype_t rbbuff [2] = { 0 };

	const uint_fast16_t bandmask = (1U << glob_bandf);

	/* регистр управления (74HC595), дальше от процессора */
	RBVAL(010, bandmask >> 0, 8);			// D0..D7: pin 15, 01-07 выбор 7..0 диапазонного фильтра

	/* регистр управления (74HC595), ближе к процессору */
	RBVAL(006, bandmask >> 8, 2);	/* pin 06,07 выбор 9,8 диапазонного фильтра */
	RBBIT(005, glob_att != 0);	/* pin 05 second stage (20 dB) atteuator on */
	RBBIT(004, glob_preamp != 0);	/* pin 04: RF amplifier */
	RBBIT(003, ! (glob_agc == BOARD_AGCCODE_OFF));	/* pin 03 - AGC ON */
	RBBIT(002, glob_notch);		/* pin 02 */
	RBBIT(001, glob_filter == BOARD_FILTER_6P0);	/* pin 01 6 kHz filter */
	RBBIT(000, ! glob_reset_n);		/* pin 15 in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE_V8A	// приёмник "Воронёнок"
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// плата управления приёмника "Воронёнок"
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
#if WITHIF4DSP
	const uint_fast8_t fm = 0;
	const uint_fast8_t am = 0;
	const uint_fast8_t detector = BOARD_DETECTOR_SSB;
#else /* WITHIF4DSP */
	const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated
	const uint_fast8_t detector = glob_af_input;
#endif /* WITHIF4DSP */

#if defined (LO1MODE_HYBRID) || defined (LO1MODE_FIXSCALE)
	const uint_fast8_t vcomask = (1U << glob_vco);
#else
	const uint_fast8_t vcomask = 0;
#endif

	rbtype_t rbbuff [5] = { 0 };

	RBVAL(040, 1U << glob_bandf, 8);		// D0..D7: band select бит выбора диапазонного фильтра

	// next 74HC596 - DD13
	/* учет диапазона - на 0-м диапазоне обход УВЧ включается принудительно */
	RBVAL(034, glob_bandf, 4);			// в новых версиях не используется D4..D7: band select код выбора диапазонного фильтра
	RBVAL(032, glob_att, 2);				/* D3:D2: 10 dB ATTENUATOR RELAYS POWER */
	RBBIT(031, glob_preamp && (glob_bandf != 0));				/* D1: RF amplifier */
	RBBIT(030, glob_tx);					/* D0: TX mode: 1 - TX режим передачи */

	// next 74HC596 - DD12
	RBVAL(024, glob_tx ? 0x0f : ~ glob_filter, 4);	/* D7..D4: select IF filter, low level selection */
	RBBIT(023, glob_tx);					/* d3: af_if_off  */
	RBBIT(022, glob_tx ? 0x00 : am);		/* D2 AM detector ON */
	RBBIT(021, glob_tx ? 0x00 : fm);		/* D1 FM detector ON */
	RBBIT(020, (glob_agc == BOARD_AGCCODE_OFF));				/* D0: AGC OFF */

	// next 74HC596 - DD11
	RBVAL(015, glob_agc, 3);	/* D7..D5:  AGC code (delay) */
	RBVAL(013, glob_tx ? BOARD_DETECTOR_MUTE : detector, 2);	/* D4..D3: AF input selection 0-ssb, 1-am, 2-mute, 3-fm */
	RBBIT(012, glob_tx && glob_txcw);	 // pin 02: d2: ssb_mod_unbalance
	RBBIT(011, glob_mikemute);		// pin 01: d1: mike_amp_mute
	RBBIT(010, fm || am);	// pin 15: D0: switch lo4 off in AM and FM modes

	/* регистр управления (74HC595 - S2), управляющий DDS RESET и включением VFO. */
	RBVAL(004, vcomask, 4);				/* pin 07,06,05,04: d7..d4 in control register - VCO select */
	RBVAL(002, 0x00, 2);				/* pin 03,02: d3..d2 in control register - spare bits */
	RBBIT(001, 1);			/* pin 01: d1 in control register - 0: ad9951 controlled localy */
	RBBIT(000, ! glob_reset_n);		/* pin 15: d0 in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE_RAVENDSP_V1	// трансивер "Воронёнок" с 12 kHz IF DSP
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */
//
// Проекты:
// production-mainunit_v2a-rezonit
// production-mainunit_v2-rezonit

// плата управления трансивера "Воронёнок"
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
#if defined (LO1MODE_HYBRID) || defined (LO1MODE_FIXSCALE)
	const uint_fast8_t vcomask = ((uint_fast8_t) 1 << glob_vco);
#else
	const uint_fast8_t vcomask = 0;
#endif

	const uint_fast8_t filtercode = glob_tx ? BOARD_FILTERCODE_6P0TX : glob_filter;

	rbtype_t rbbuff [6] = { 0 };

	RBVAL8(050, 1U << glob_bandf);		// D0..D7: band select бит выбора диапазонного фильтра

	// next 74HC596 - DD13
	/* учет диапазона - на 0-м диапазоне обход УВЧ включается принудительно */
	RBBIT(047, glob_tx || ! glob_preamp || (glob_bandf == 0));	/* D7: убрать смещение с транзистора УВЧ */
	RBVAL(044, 0x00, 3);			// spare bits
	RBVAL(042, glob_att, 2);				/* D3:D2: 10 dB ATTENUATOR RELAYS POWER */
	RBBIT(041, glob_preamp && (glob_bandf != 0));				/* D1: RF amplifier */
	RBBIT(040, glob_tx);					/* D0: TX mode: 1 - TX режим передачи */
	// ADG714 - inputs of IF filters selection
	RBVAL8(030, filtercode);		// D0..D7: band select бит выбора фильтра ПЧ
	// ADG714 - outputs of IF filters selection
	RBVAL8(020, filtercode);		// D0..D7: band select бит выбора фильтра ПЧ

	// 74HC596 - DD22
	RBVAL(013, 0x00, 5);	/* D3..D7: spare bits */
	RBBIT(012, ! glob_codec2_nreset);	 // pin 02: d2: CS4272 reset
	RBBIT(011, ! glob_tx);		// pin 01: d1: ~TXMODE
	RBBIT(010, glob_tx);	// pin 15: D0: ~RXMODE

	/* регистр управления (74HC595 - S1), управляющий DDS RESET и включением VFO. */
	RBVAL(004, vcomask, 4);				/* pin 07,06,05,04: d7..d4 in control register - VCO select */
	RBVAL(002, (glob_bglight - WITHLCDBACKLIGHTMIN), 2);	/* pin 02..pin 03 - LCD backlight */
	RBBIT(001, glob_kblight);			/* pin 01: d1 in control register - keyboard backlight */
	RBBIT(000, ! glob_reset_n);		/* pin 15: d0 in control register - ad9951 RESET */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

#elif CTLREGMODE_RAVENDSP_V3	// трансивер "Воронёнок" с DSP и FPGA
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// трансивер "Воронёнок" с DSP и FPGA
static void 
//NOINLINEAT
prog_rxctrlreg(uint_fast8_t plane)
{
#if defined(DDS1_TYPE)
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
#endif
	// registers chain control register
	{
		//Current Output at Full Power A1 = 1, A0 = 1, VO = 0 ±500 ±380 ±350 ±320 mA min A
		//Current Output at Power Cutback A1 = 1, A0 = 0, VO = 0 ±450 ±350 ±320 ±300 mA min A
		//Current Output at Idle Power A1 = 0, A0 = 1, VO = 0 ±100 ±60 ±55 ±50 mA min A

		enum
		{
			HARDWARE_OPA2674I_FULLPOWER = 0x03,
			HARDWARE_OPA2674I_POWERCUTBACK = 0x02,
			HARDWARE_OPA2674I_IDLEPOWER = 0x01,
			HARDWARE_OPA2674I_SHUTDOWN = 0x00
		};
		static const FLASHMEM uint_fast8_t powerxlat [] =
		{
			HARDWARE_OPA2674I_IDLEPOWER,
			HARDWARE_OPA2674I_POWERCUTBACK,
			HARDWARE_OPA2674I_FULLPOWER,
		};

		const spitarget_t target = targetctl1;
		const uint_fast8_t txgated = glob_tx && glob_txgate;

		rbtype_t rbbuff [6] = { 0 };

		// STP08CP05TTR на части передатчика
		RBNULL(054, 4);
		RBVAL(052, ~ (txgated ? powerxlat [glob_stage2level] : HARDWARE_OPA2674I_SHUTDOWN), 2);	// A1..A0 of OPA2674I-14D in stage 2
		RBVAL(050, ~ (txgated ? powerxlat [glob_stage1level] : HARDWARE_OPA2674I_SHUTDOWN), 2);	// A1..A0 of OPA2674I-14D in stage 1

		// STP08CP05TTR на части передатчика
		RBVAL8(040, 1U << glob_bandf2);		// D0..D7: band select бит выбора диапазонного фильтра передатчика

		// STP08CP05TTR в управлении диапазонными фильтрами приёмника
		RBVAL8(030, glob_tx ? 0 : (1U << glob_bandf));		// D0..D7: band select бит выбора диапазонного фильтра приёмника

		// STP08CP05TTR DD3 в управлении диапазонными фильтрами приёмника
		RBVAL(026, glob_att, 2);			/* D7:D6: 12 dB and 6 dB attenuator control */
		RBNULL(024, 2);
		RBBIT(023, glob_tx);				// OUT3 (pin 08): переключение антенного реле - в макетах
		RBNULL(021, 2);
		RBBIT(020, glob_bandf == 0);		// OUT0 (pin 05): средневолновый ФНЧ

		// SN74HC595PW рядом с DIN8
		RBNULL(014, 4);
		RBVAL(010, glob_bandf3, 4);			/* D3:D0: DIN8 PA band select */

		// STP08CP05TTR рядом с DIN8
		RBNULL(007, 1);
		RBBIT(006, glob_tx);				// D6: ext ptt signal
		RBBIT(005, glob_bandf == 0);		// D5: средневолновый ФНЧ - управление реле
		RBNULL(003, 2);
		RBVAL(001, (glob_bglight - WITHLCDBACKLIGHTMIN), 2);	/* D2:D1 - LCD backlight */
		RBBIT(000, glob_kblight);			/* D0: keyboard backlight */

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}
}

static void 
//NOINLINEAT
prog_ctldacreg(void)	// CTLREGMODE_RAVENDSP_V3
{
	const spitarget_t target = targetdac1;
	// Выдача кода на цифровой потенциометр AD5260BRUZ50 в управлении частотой опорного генератора

	spi_select(target, CTLREG_SPIMODE);
	spi_progval8_p1(target, glob_dac1);
	spi_complete(target);
	spi_unselect(target);
}

#elif CTLREGMODE_RAVENDSP_V4	// "Воронёнок" с DSP и FPGA, SD-CARD
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// "Воронёнок" с DSP и FPGA, SD-CARD
static void 
//NOINLINEAT
prog_rxctrlreg(uint_fast8_t plane)
{
#if defined(DDS1_TYPE)
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
#endif

	// registers chain control register
	{
		const uint_fast8_t txgated = glob_tx && glob_txgate;
		const uint_fast8_t lcdblcode = (glob_bglight - WITHLCDBACKLIGHTMIN);
		const uint_fast8_t attvalue2db = glob_attvalue / 2;
		const uint_fast8_t attvalue2dbhalf = attvalue2db / 2;
		// подготовка кодов для управления аттенюаторами
		const uint_fast8_t attvalue1 = (attvalue2dbhalf);
		const uint_fast8_t attvalue2 = (attvalue2db - attvalue2dbhalf);

		//PRINTF(PSTR("a1=%u, a2=%u\n"), attvalue1, attvalue2);

		//Current Output at Full Power A1 = 1, A0 = 1, VO = 0 ±500 ±380 ±350 ±320 mA min A
		//Current Output at Power Cutback A1 = 1, A0 = 0, VO = 0 ±450 ±350 ±320 ±300 mA min A
		//Current Output at Idle Power A1 = 0, A0 = 1, VO = 0 ±100 ±60 ±55 ±50 mA min A

		enum
		{
			HARDWARE_OPA2674I_FULLPOWER = 0x03,
			HARDWARE_OPA2674I_POWERCUTBACK = 0x02,
			HARDWARE_OPA2674I_IDLEPOWER = 0x01,
			HARDWARE_OPA2674I_SHUTDOWN = 0x00
		};
		/*
		static const FLASHMEM uint_fast8_t powerxlat [] =
		{
			HARDWARE_OPA2674I_IDLEPOWER,
			HARDWARE_OPA2674I_POWERCUTBACK,
			HARDWARE_OPA2674I_FULLPOWER,
		};
		*/
		const spitarget_t target = targetctl1;

		rbtype_t rbbuff [6] = { 0 };
		// Полный вариант сборки платы
		// 74HC595 управление программируемым аттенюатором AT-220
		RBVAL(056, 2 - ((attvalue1 & 0x01) != 0), 2);
		RBVAL(054, 2 - ((attvalue1 & 0x02) != 0), 2);
		RBVAL(052, 2 - ((attvalue1 & 0x04) != 0), 2);
		RBVAL(050, 2 - ((attvalue1 & 0x08) != 0), 2);

		// 74HC595 управление программируемым аттенюатором AT-220
		RBVAL(046, 2 - ((attvalue2 & 0x01) != 0), 2);
		RBVAL(044, 2 - ((attvalue2 & 0x02) != 0), 2);
		RBVAL(042, 2 - ((attvalue2 & 0x04) != 0), 2);
		RBVAL(040, 2 - ((attvalue2 & 0x08) != 0), 2);

		// STP08CP05TTR в управлении диапазонными фильтрами приёмника
		RBVAL8(030, glob_tx ? 0 : (1U << glob_bandf));		// D0..D7: band select бит выбора диапазонного фильтра приёмника

		// STP08CP05TTR в управлении диапазонными фильтрами приёмника
		RBVAL(026, glob_att, 2);			/* D7:D6: 12 dB and 6 dB attenuator control */
		RBBIT(021, txgated);				// D1: ~PA BIAS
		RBBIT(020, glob_bandf == 0);		// D0: средневолновый ФНЧ

		// SN74HC595PW рядом с DIN8
		RBNULL(014, 4);
		RBVAL(010, glob_bandf3, 4);			/* D3:D0: DIN8 PA band select */

		// STP08CP05TTR рядом с DIN8
		RBNULL(007, 1);
		RBBIT(006, glob_tx);				// D6: EXT PTT signal
		RBBIT(005, glob_bandf == 0);		// D5: средневолновый ФНЧ - управление реле
		RBBIT(004, glob_sdcardpoweron);		/* D4: SD CARD POWER */
		RBBIT(003, lcdblcode & 0x02);		/* D3	- LCD backlight */
		RBBIT(002, lcdblcode & 0x02);		/* D2	- LCD backlight */
		RBBIT(001, lcdblcode & 0x01);		/* D2:D1 - LCD backlight */
		RBBIT(000, glob_preamp && glob_bandf != 0 /*glob_kblight*/);			/* D0: keyboard backlight заменён на внешнее управление УВЧ */

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}
}

static void 
//NOINLINEAT
prog_ctldacreg(void)	// CTLREGMODE_RAVENDSP_V4
{
	const spitarget_t target = targetdac1;
	// Выдача кода на цифровой потенциометр AD5260BRUZ50 в управлении частотой опорного генератора

	spi_select(target, CTLREG_SPIMODE);
	spi_progval8_p1(target, glob_dac1);
	spi_complete(target);
	spi_unselect(target);
}

#elif CTLREGMODE_RAVENDSP_V5	// "Воронёнок" с DSP и FPGA, DUAL WATCH, SD-CARD & PA on board
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

#if WITHAUTOTUNEROWNSPI && WITHAUTOTUNER

static void 
prog_atuctlreg(const spitarget_t target)
{
	rbtype_t rbbuff [2] = { 0 };

	/* +++ Управление согласующим устройством */
	/* регистр управления массивом конденсаторов - дальше от процессора */
	RBVAL(011, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_C) >> 1), 7);/* LSB-MSB: pin07-pin01. Capacitors tuner bank 	*/
	RBBIT(010, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 15: TYPE OF TUNER 	*/
	/* регистр управления наборной индуктивностью. - ближе к процессору */
	RBBIT(007, ! glob_tuner_bypass);		// pin 07: обход СУ
	//RBVAL(000, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_L) >> 1), 7);	/* LSB-MSB: pin06-pin01,pin15: Inductors tuner bank 	*/
	RBVAL(000, glob_tuner_bypass ? 0 : glob_tuner_L, 7);	/* LSB-MSB: pin06-pin01,pin15: Inductors tuner bank 	*/
	/* --- Управление согласующим устройством */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}
#endif /* WITHAUTOTUNEROWNSPI && WITHAUTOTUNER */

// "Воронёнок" с DSP и FPGA, SD-CARD
static void 
//NOINLINEAT
prog_rxctrlreg(uint_fast8_t plane)
{
#if defined(DDS1_TYPE)
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
#endif
#if WITHAUTOTUNEROWNSPI && WITHAUTOTUNER
	prog_atuctlreg(targetatu1);		// Tuner control regiser
#endif /* WITHAUTOTUNEROWNSPI && WITHAUTOTUNER */

	// registers chain control register
	{
		const uint_fast8_t lcdblcode = (glob_bglight - WITHLCDBACKLIGHTMIN);
		//Current Output at Full Power A1 = 1, A0 = 1, VO = 0 ±500 ±380 ±350 ±320 mA min A
		//Current Output at Power Cutback A1 = 1, A0 = 0, VO = 0 ±450 ±350 ±320 ±300 mA min A
		//Current Output at Idle Power A1 = 0, A0 = 1, VO = 0 ±100 ±60 ±55 ±50 mA min A

		enum
		{
			HARDWARE_OPA2674I_FULLPOWER = 0x03,
			HARDWARE_OPA2674I_POWERCUTBACK = 0x02,
			HARDWARE_OPA2674I_IDLEPOWER = 0x01,
			HARDWARE_OPA2674I_SHUTDOWN = 0x00
		};
		static const FLASHMEM uint_fast8_t powerxlat [] =
		{
			HARDWARE_OPA2674I_IDLEPOWER,
			HARDWARE_OPA2674I_POWERCUTBACK,
			HARDWARE_OPA2674I_FULLPOWER,
		};
		const spitarget_t target = targetctl1;
		const uint_fast8_t txgated = glob_tx && glob_txgate;

#if 0
		rbtype_t rbbuff [9] = { 0 };
		/* +++ Управление согласующим устройством */
		/* дополнительный регистр */
		RBBIT(0103, glob_tx);				/* pin 03:индикатор передачи */
		RBBIT(0102, glob_antenna);			// pin 02: выбор антенны (0 - ANT1, 1 - ANT2)
		RBBIT(0101, ! glob_tuner_bypass);		// pin 01: обход СУ (1 - работа)
		RBBIT(0100, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 15: TYPE OF TUNER 	*/
		/* регистр управления массивом конденсаторов */
		RBVAL8(0070, glob_tuner_bypass ? 0 : glob_tuner_C);			/* Capacitors tuner bank 	*/
		/* регистр управления наборной индуктивностью. */
		RBVAL8(0060, glob_tuner_bypass ? 0 : glob_tuner_L);			/* Inductors tuner bank 	*/
		/* --- Управление согласующим устройством */
#else
		rbtype_t rbbuff [6] = { 0 };

#endif
		// AD5260BRUZ20 - регулировка тока покоя оконечного каскада передатчика
		RBVAL8(0050, txgated ? glob_pabias : 0);

		// STP08CP05TTR на части передатчика
		RBVAL8(0040, 1U << glob_bandf2);		// D0..D7: band select бит выбора диапазонного фильтра передатчика

		// STP08CP05TTR в управлении диапазонными фильтрами приёмника
		RBVAL8(0030, glob_tx ? 0 : (1U << glob_bandf));		// D0: 0, D7..D1: band select бит выбора диапазонного фильтра приёмника

		// STP08CP05TTR в управлении диапазонными фильтрами приёмника
		RBVAL(0026, glob_att, 2);			/* D7:D6: 12 dB and 6 dB attenuator control */
		RBVAL(0024, ~ (txgated ? powerxlat [glob_stage2level] : HARDWARE_OPA2674I_SHUTDOWN), 2);	// A1..A0 of OPA2674I-14D in stage 2
		RBVAL(0022, ~ (txgated ? powerxlat [glob_stage1level] : HARDWARE_OPA2674I_SHUTDOWN), 2);	// A1..A0 of OPA2674I-14D in stage 1
		RBBIT(0021, glob_tx);				// D1: TX ANT relay
		RBBIT(0020, glob_bandf == 0);		// D0: средневолновый ФНЧ

		// SN74HC595PW рядом с DIN8
		RBNULL(0014, 4);
		RBVAL(0010, glob_bandf3, 4);			/* D3:D0: DIN8 PA band select */

		// STP08CP05TTR рядом с DIN8
		RBBIT(0007, glob_fanflag);			// D7: TX FAN
		RBBIT(0006, glob_tx);				// D6: EXT PTT signal
		RBBIT(0005, glob_bandf == 0);		// D5: средневолновый ФНЧ - управление реле
		RBBIT(0004, glob_sdcardpoweron);		/* D4: SD CARD POWER */
		RBBIT(0003, lcdblcode & 0x02);		/* D3	- LCD backlight */
		RBBIT(0002, lcdblcode & 0x02);		/* D2	- LCD backlight */
		RBBIT(0001, lcdblcode & 0x01);		/* D2:D1 - LCD backlight */
		RBBIT(0000, glob_kblight);			/* D0: keyboard backlight */

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}
}

static void 
//NOINLINEAT
prog_ctldacreg(void)	// CTLREGMODE_RAVENDSP_V5
{
	const spitarget_t target = targetdac1;
	// Выдача кода на цифровой потенциометр AD5260BRUZ20 в управлении частотой опорного генератора

	spi_select(target, CTLREG_SPIMODE);
	spi_progval8_p1(target, glob_dac1);
	spi_complete(target);
	spi_unselect(target);
}

#elif CTLREGMODE_RAVENDSP_V6	// "Воронёнок" с DSP и FPGA, DUAL WATCH, SD-CARD & PA on board
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// "Воронёнок" с DSP и FPGA, SD-CARD
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
#if defined(DDS1_TYPE)
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
#endif

	// registers chain control register
	{
		const uint_fast8_t lcdblcode = (glob_bglight - WITHLCDBACKLIGHTMIN);
		//Current Output at Full Power A1 = 1, A0 = 1, VO = 0 ±500 ±380 ±350 ±320 mA min A
		//Current Output at Power Cutback A1 = 1, A0 = 0, VO = 0 ±450 ±350 ±320 ±300 mA min A
		//Current Output at Idle Power A1 = 0, A0 = 1, VO = 0 ±100 ±60 ±55 ±50 mA min A

		enum
		{
			HARDWARE_OPA2674I_FULLPOWER = 0x03,
			HARDWARE_OPA2674I_POWERCUTBACK = 0x02,
			HARDWARE_OPA2674I_IDLEPOWER = 0x01,
			HARDWARE_OPA2674I_SHUTDOWN = 0x00
		};
		static const FLASHMEM uint_fast8_t powerxlat [] =
		{
			HARDWARE_OPA2674I_IDLEPOWER,
			HARDWARE_OPA2674I_POWERCUTBACK,
			HARDWARE_OPA2674I_FULLPOWER,
		};
		const spitarget_t target = targetctl1;


#if 1
		rbtype_t rbbuff [8] = { 0 };
		/* +++ Управление согласующим устройством */
		/* дополнительный регистр */
		RBBIT(0073, glob_tx);				/* pin 03:индикатор передачи */
		RBBIT(0072, glob_antenna);			// pin 02: выбор антенны (0 - ANT1, 1 - ANT2)
		RBBIT(0071, ! glob_tuner_bypass);		// pin 01: обход СУ (1 - работа)
		RBBIT(0070, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 15: TYPE OF TUNER 	*/
		/* регистр управления массивом конденсаторов */
		RBVAL8(0060, glob_tuner_bypass ? 0 : glob_tuner_C);			/* Capacitors tuner bank 	*/
		/* регистр управления наборной индуктивностью. */
		RBVAL8(0050, glob_tuner_bypass ? 0 : glob_tuner_L);			/* Inductors tuner bank 	*/
		/* --- Управление согласующим устройством */
#else
		rbtype_t rbbuff [5] = { 0 };

#endif
		const uint_fast8_t txgated = glob_tx && glob_txgate;
		// STP08CP05TTR на части передатчика
		RBBIT(0047, glob_antenna);		// D7: antenns select бит выбора антенны (0 - ANT1, 1 - ANT2)
		RBVAL(0040, 1U << glob_bandf2, 7);		// D0..D6: band select бит выбора диапазонного фильтра передатчика

		// STP08CP05TTR в управлении диапазонными фильтрами приёмника
		RBVAL(0031, glob_tx ? 0 : (1U << glob_bandf) >> 1, 7);		// D1: 1, D7..D1: band select бит выбора диапазонного фильтра приёмника
		RBBIT(0030, txgated);			// D0: включение подачи смещения на выходной каскад усилителя мощности

		// STP08CP05TTR в управлении диапазонными фильтрами приёмника
		RBVAL(0026, glob_att, 2);			/* D7:D6: 12 dB and 6 dB attenuator control */
		RBVAL(0024, ~ (txgated ? powerxlat [glob_stage2level] : HARDWARE_OPA2674I_SHUTDOWN), 2);	// A1..A0 of OPA2674I-14D in stage 2
		RBVAL(0022, ~ (txgated ? powerxlat [glob_stage1level] : HARDWARE_OPA2674I_SHUTDOWN), 2);	// A1..A0 of OPA2674I-14D in stage 1
		RBBIT(0021, glob_tx);				// D1: TX ANT relay
		RBBIT(0020, glob_bandf == 0);		// D0: средневолновый ФНЧ

		// SN74HC595PW рядом с DIN8
		RBNULL(0014, 4);
		RBVAL(0010, glob_bandf3, 4);			/* D3:D0: DIN8 PA band select */

		// STP08CP05TTR рядом с DIN8
		RBBIT(0007, glob_fanflag);			// D7: TX FAN
		RBBIT(0006, glob_tx);				// D6: EXT PTT signal
		RBBIT(0005, glob_bandf == 0);		// D5: средневолновый ФНЧ - управление реле
		RBBIT(0004, glob_sdcardpoweron);		/* D4: SD CARD POWER */
		RBBIT(0003, lcdblcode & 0x02);		/* D3	- LCD backlight */
		RBBIT(0002, lcdblcode & 0x02);		/* D2	- LCD backlight */
		RBBIT(0001, lcdblcode & 0x01);		/* D2:D1 - LCD backlight */
		RBBIT(0000, glob_kblight);			/* D0: keyboard backlight */

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}
}

#elif CTLREGMODE_RAVENDSP_V7	// V6 fixed "Воронёнок" с DSP и FPGA, DUAL WATCH, SD-CARD & PA on board

#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// "Воронёнок" с DSP и FPGA, SD-CARD
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
#if defined(DDS1_TYPE)
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
#endif

	// registers chain control register
	{
		const uint_fast8_t lcdblcode = (glob_bglight - WITHLCDBACKLIGHTMIN);
		//Current Output at Full Power A1 = 1, A0 = 1, VO = 0 ±500 ±380 ±350 ±320 mA min A
		//Current Output at Power Cutback A1 = 1, A0 = 0, VO = 0 ±450 ±350 ±320 ±300 mA min A
		//Current Output at Idle Power A1 = 0, A0 = 1, VO = 0 ±100 ±60 ±55 ±50 mA min A

		enum
		{
			HARDWARE_OPA2674I_FULLPOWER = 0x03,
			HARDWARE_OPA2674I_POWERCUTBACK = 0x02,
			HARDWARE_OPA2674I_IDLEPOWER = 0x01,
			HARDWARE_OPA2674I_SHUTDOWN = 0x00
		};
		static const FLASHMEM uint_fast8_t powerxlat [] =
		{
			HARDWARE_OPA2674I_IDLEPOWER,
			HARDWARE_OPA2674I_POWERCUTBACK,
			HARDWARE_OPA2674I_FULLPOWER,
		};
		const spitarget_t target = targetctl1;


#if 0
		rbtype_t rbbuff [8] = { 0 };
		/* +++ Управление согласующим устройством */
		/* дополнительный регистр */
		RBBIT(0073, glob_tx);				/* pin 03:индикатор передачи */
		RBBIT(0072, glob_antenna);			// pin 02: выбор антенны (0 - ANT1, 1 - ANT2)
		RBBIT(0071, ! glob_tuner_bypass);		// pin 01: обход СУ (1 - работа)
		RBBIT(0070, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 15: TYPE OF TUNER 	*/
		/* регистр управления массивом конденсаторов */
		RBVAL8(0060, glob_tuner_bypass ? 0 : glob_tuner_C);			/* Capacitors tuner bank 	*/
		/* регистр управления наборной индуктивностью. */
		RBVAL8(0050, glob_tuner_bypass ? 0 : glob_tuner_L);			/* Inductors tuner bank 	*/
		/* --- Управление согласующим устройством */
#else
		rbtype_t rbbuff [5] = { 0 };

#endif
		const uint_fast8_t txgated = glob_tx && glob_txgate;

#if 0//KEYB_FPANEL20_V0A_UY5UM
		RBBIT(0047, glob_antenna);		// D7: antenns select бит выбора антенны (0 - ANT1, 1 - ANT2)
		RBBIT(0046, glob_antenna);		// D6: antenns select бит выбора антенны (0 - ANT1, 1 - ANT2)
		RBVAL(0040, 1U << glob_bandf2, 6);		// D0..D5: band select бит выбора диапазонного фильтра передатчика
#else /* KEYB_FPANEL20_V0A_UY5UM */
		// DD23 STP08CP05TTR на части передатчика
		RBBIT(0047, glob_antenna);		// D7: antenns select бит выбора антенны (0 - ANT1, 1 - ANT2)
		RBVAL(0040, 1U << glob_bandf2, 7);		// D0..D6: band select бит выбора диапазонного фильтра передатчика
#endif /* KEYB_FPANEL20_V0A_UY5UM */

		// DD1 STP08CP05TTR в управлении диапазонными фильтрами приёмника
		RBVAL(0031, glob_tx ? 0 : (1U << glob_bandf) >> 1, 7);		// D1: 1, D7..D1: band select бит выбора диапазонного фильтра приёмника
		RBBIT(0030, txgated);		// D0: включение подачи смещения на выходной каскад усилителя мощности

		// DD3 STP08CP05TTR в управлении диапазонными фильтрами приёмника
		RBVAL(0026, glob_att, 2);			/* D7:D6: 12 dB and 6 dB attenuator control */
		RBVAL(0024, ~ (txgated ? powerxlat [glob_stage2level] : HARDWARE_OPA2674I_SHUTDOWN), 2);	// A1..A0 of OPA2674I-14D in stage 2
		RBVAL(0022, ~ (txgated ? powerxlat [glob_stage1level] : HARDWARE_OPA2674I_SHUTDOWN), 2);	// A1..A0 of OPA2674I-14D in stage 1
		RBBIT(0021, glob_tx);				// D1: TX ANT relay
		RBBIT(0020, glob_bandf == 0);		// D0: средневолновый ФНЧ

		// DD21 SN74HC595PW рядом с DIN8
		RBNULL(0014, 4);
		RBVAL(0010, glob_bandf3, 4);			/* D3:D0: DIN8 PA band select */

		// DD5 STP08CP05TTR рядом с DIN8
		RBBIT(0007, glob_fanflag);			// D7: TX FAN
		RBBIT(0006, glob_tx);				// D6: EXT PTT signal
		RBBIT(0005, glob_bandf == 0);		// D5: средневолновый ФНЧ - управление реле
		RBBIT(0004, glob_sdcardpoweron);		/* D4: SD CARD POWER */
		RBBIT(0003, lcdblcode & 0x02);		/* D3	- LCD backlight */
		RBBIT(0002, lcdblcode & 0x02);		/* D2	- LCD backlight */
		RBBIT(0001, lcdblcode & 0x01);		/* D2:D1 - LCD backlight */
		RBBIT(0000, glob_kblight);			/* D0: keyboard backlight */

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}
}

#elif CTLREGMODE_STORCH_V1	// V6 fixed "Воронёнок" с USB FS, DSP и FPGA, DUAL WATCH, SD-CARD & PA on board

#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// "Storch" с USB, DSP и FPGA, SD-CARD
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
#if defined(DDS1_TYPE)
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
#endif

	// registers chain control register
	{
		const uint_fast8_t lcdblcode = (glob_bglight - WITHLCDBACKLIGHTMIN);
		//Current Output at Full Power A1 = 1, A0 = 1, VO = 0 ±500 ±380 ±350 ±320 mA min A
		//Current Output at Power Cutback A1 = 1, A0 = 0, VO = 0 ±450 ±350 ±320 ±300 mA min A
		//Current Output at Idle Power A1 = 0, A0 = 1, VO = 0 ±100 ±60 ±55 ±50 mA min A

		enum
		{
			HARDWARE_OPA2674I_FULLPOWER = 0x03,
			HARDWARE_OPA2674I_POWERCUTBACK = 0x02,
			HARDWARE_OPA2674I_IDLEPOWER = 0x01,
			HARDWARE_OPA2674I_SHUTDOWN = 0x00
		};
		static const FLASHMEM uint_fast8_t powerxlat [] =
		{
			HARDWARE_OPA2674I_IDLEPOWER,
			HARDWARE_OPA2674I_POWERCUTBACK,
			HARDWARE_OPA2674I_FULLPOWER,
		};
		const spitarget_t target = targetctl1;

		rbtype_t rbbuff [8] = { 0 };

#if 1
		/* +++ Управление согласующим устройством */
		/* дополнительный регистр */
		RBBIT(0073, glob_tx);				/* pin 03:индикатор передачи */
		RBBIT(0072, glob_antenna);			// pin 02: выбор антенны (0 - ANT1, 1 - ANT2)
		RBBIT(0071, ! glob_tuner_bypass);		// pin 01: обход СУ (1 - работа)
		RBBIT(0070, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 15: TYPE OF TUNER 	*/
		/* регистр управления массивом конденсаторов */
		RBVAL8(0060, glob_tuner_bypass ? 0 : glob_tuner_C);			/* Capacitors tuner bank 	*/
		/* регистр управления наборной индуктивностью. */
		RBVAL8(0050, glob_tuner_bypass ? 0 : glob_tuner_L);			/* Inductors tuner bank 	*/
		/* --- Управление согласующим устройством */

#endif
		const uint_fast8_t txgated = glob_tx && glob_txgate;
		// DD17 STP08CP05TTR на разъём управления LPF
		RBBIT(0047, txgated);		// D7 - XS18 PIN 16: PTT
		RBVAL(0040, 1U << glob_bandf2, 7);		// D0..D6: band select бит выбора диапазонного фильтра передатчика

		// DD16 STP08CP05TTR в управлении диапазонными фильтрами приёмника
		RBVAL(0031, glob_tx ? 0 : (1U << glob_bandf) >> 1, 7);		// D1: 1, D7..D1: band select бит выбора диапазонного фильтра приёмника
		RBBIT(0030, txgated);		// D0: включение подачи смещения на выходной каскад усилителя мощности

		// DD15 STP08CP05TTR в управлении диапазонными фильтрами приёмника
		RBVAL(0026, glob_att, 2);			/* D7:D6: 12 dB and 6 dB attenuator control */
		RBVAL(0024, ~ (txgated ? powerxlat [glob_stage1level] : HARDWARE_OPA2674I_SHUTDOWN), 2);	// A1..A0 of OPA2674I-14D in stage 1
		RBBIT(0023, glob_fanflag);			// D3: not used - dedicated to PA FAN
		RBBIT(0022, glob_bandf == 0);		// D2: средневолновый ФНЧ - управление реле на выходе фильтров
		RBBIT(0021, glob_tx);				// D1: TX ANT relay
		RBBIT(0020, glob_bandf == 0);		// D0: средневолновый ФНЧ - управление реле на входе

		// DD18 SN74HC595PW рядом с DIN8
		RBNULL(0014, 4);
		RBVAL(0010, glob_bandf3, 4);			/* D3:D0: DIN8 EXT PA band select */

		// DD14 STP08CP05TTR рядом с DIN8
		//RBBIT(0007, ! glob_reset_n);		// D7: NMEA reset
		RBBIT(0007, glob_fanflag);			// D7: TX FAN
		RBBIT(0006, glob_tx);				// D6: DIN8 EXT PTT signal
		RBBIT(0005, 0);						// D5: not used
		RBBIT(0004, 0);						/* D4: not used */
		RBBIT(0003, lcdblcode & 0x02);		/* D3	- LCD backlight */
		RBBIT(0002, lcdblcode & 0x02);		/* D2	- LCD backlight */
		RBBIT(0001, lcdblcode & 0x01);		/* D2:D1 - LCD backlight */
		RBBIT(0000, glob_kblight);			/* D0: keyboard backlight */

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}
}
#elif CTLREGMODE_STORCH_V1_R4DR	// STORCH_V1 thermo с USB FS, DSP и FPGA, DUAL WATCH, SD-CARD & PA on board
// Сигналы DIN8 дублируются на разъеме управления тюнером
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// "Storch" с USB, DSP и FPGA, SD-CARD
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
#if defined(DDS1_TYPE)
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
#endif

	// registers chain control register
	{
		const uint_fast8_t lcdblcode = (glob_bglight - WITHLCDBACKLIGHTMIN);
		//Current Output at Full Power A1 = 1, A0 = 1, VO = 0 ±500 ±380 ±350 ±320 mA min A
		//Current Output at Power Cutback A1 = 1, A0 = 0, VO = 0 ±450 ±350 ±320 ±300 mA min A
		//Current Output at Idle Power A1 = 0, A0 = 1, VO = 0 ±100 ±60 ±55 ±50 mA min A

		enum
		{
			HARDWARE_OPA2674I_FULLPOWER = 0x03,
			HARDWARE_OPA2674I_POWERCUTBACK = 0x02,
			HARDWARE_OPA2674I_IDLEPOWER = 0x01,
			HARDWARE_OPA2674I_SHUTDOWN = 0x00
		};
		static const FLASHMEM uint_fast8_t powerxlat [] =
		{
			HARDWARE_OPA2674I_IDLEPOWER,
			HARDWARE_OPA2674I_POWERCUTBACK,
			HARDWARE_OPA2674I_FULLPOWER,
		};
		const spitarget_t target = targetctl1;

		rbtype_t rbbuff [8] = { 0 };

#if 1
		/* +++ Управление согласующим устройством */
		/* дополнительный регистр */
		RBBIT(0073, glob_tx);				/* pin 03:индикатор передачи */
		RBBIT(0072, glob_antenna);			// pin 02: выбор антенны (0 - ANT1, 1 - ANT2)
		RBBIT(0071, ! glob_tuner_bypass);		// pin 01: обход СУ (1 - работа)
		RBBIT(0070, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 15: TYPE OF TUNER 	*/
		/* регистр управления массивом конденсаторов */
		RBVAL8(0060, glob_tuner_bypass ? 0 : glob_tuner_C);			/* Capacitors tuner bank 	*/
		/* регистр управления наборной индуктивностью. */
		RBVAL8(0050, glob_tuner_bypass ? 0 : glob_tuner_L);			/* Inductors tuner bank 	*/
		/* --- Управление согласующим устройством */

#endif
		const uint_fast8_t txgated = glob_tx && glob_txgate;
		// DD17 STP08CP05TTR на разъём управления LPF
		RBBIT(0047, glob_antenna);		// 16: D7: antenns select бит выбора антенны (0 - ANT1, 1 - ANT2)
		//RBVAL(0040, 1U << glob_bandf2, 7);		// D0..D6: band select бит выбора диапазонного фильтра передатчика
		RBBIT(0045, glob_fanflag);			// 12: D5: PA FAN
		RBBIT(0044, glob_tx);				// 10: D4: TX ANT relay
		RBVAL(0040, ~ glob_bandf3, 4);		// 02, 04, 06, 08: D0..D3: band select

		// DD16 STP08CP05TTR в управлении диапазонными фильтрами приёмника
		RBVAL(0031, glob_tx ? 0 : (1U << glob_bandf) >> 1, 7);		// D1: 1, D7..D1: band select бит выбора диапазонного фильтра приёмника
		RBBIT(0030, txgated);		// D0: включение подачи смещения на выходной каскад усилителя мощности

		// DD15 STP08CP05TTR в управлении диапазонными фильтрами приёмника
		RBVAL(0026, glob_att, 2);			/* D7:D6: 12 dB and 6 dB attenuator control */
		RBVAL(0024, ~ (txgated ? powerxlat [glob_stage1level] : HARDWARE_OPA2674I_SHUTDOWN), 2);	// A1..A0 of OPA2674I-14D in stage 1
		RBBIT(0023, glob_fanflag);			// D3: not used - dedicated to PA FAN
		RBBIT(0022, glob_bandf == 0);		// D2: средневолновый ФНЧ - управление реле на выходе фильтров
		RBBIT(0021, glob_tx);				// D1: TX ANT relay
		RBBIT(0020, glob_bandf == 0);		// D0: средневолновый ФНЧ - управление реле на входе

		// DD18 SN74HC595PW рядом с DIN8
		RBNULL(0014, 4);
		RBVAL(0010, glob_bandf3, 4);			/* D3:D0: DIN8 EXT PA band select */

		// DD14 STP08CP05TTR рядом с DIN8
		//RBBIT(0007, ! glob_reset_n);		// D7: NMEA reset
		RBBIT(0007, glob_fanflag);			// D7: TX FAN
		RBBIT(0006, glob_tx);				// D6: DIN8 EXT PTT signal
		RBBIT(0005, 0);						// D5: not used
		RBBIT(0004, 0);						/* D4: not used */
		RBBIT(0003, lcdblcode & 0x02);		/* D3	- LCD backlight */
		RBBIT(0002, lcdblcode & 0x02);		/* D2	- LCD backlight */
		RBBIT(0001, lcdblcode & 0x01);		/* D2:D1 - LCD backlight */
		RBBIT(0000, glob_tx);			/* D0: keyboard backlight -> PTT OUT */

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}
}

#elif CTLREGMODE_STORCH_V2	// USB FS, USB HS, DSP и FPGA, DUAL WATCH, SD-CARD & PA on board
// отличается формированием сигналов кода диапазона на заднем разъеме (DIN8)
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// "Storch" с USB, DSP и FPGA, SD-CARD
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
#if defined(DDS1_TYPE)
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
#endif

	// registers chain control register
	{
		const uint_fast8_t lcdblcode = (glob_bglight - WITHLCDBACKLIGHTMIN);
		//Current Output at Full Power A1 = 1, A0 = 1, VO = 0 ±500 ±380 ±350 ±320 mA min A
		//Current Output at Power Cutback A1 = 1, A0 = 0, VO = 0 ±450 ±350 ±320 ±300 mA min A
		//Current Output at Idle Power A1 = 0, A0 = 1, VO = 0 ±100 ±60 ±55 ±50 mA min A

		enum
		{
			HARDWARE_OPA2674I_FULLPOWER = 0x03,
			HARDWARE_OPA2674I_POWERCUTBACK = 0x02,
			HARDWARE_OPA2674I_IDLEPOWER = 0x01,
			HARDWARE_OPA2674I_SHUTDOWN = 0x00
		};
		static const FLASHMEM uint_fast8_t powerxlat [] =
		{
			HARDWARE_OPA2674I_IDLEPOWER,
			HARDWARE_OPA2674I_POWERCUTBACK,
			HARDWARE_OPA2674I_FULLPOWER,
		};
		const spitarget_t target = targetctl1;

		rbtype_t rbbuff [8] = { 0 };
		const uint_fast8_t txgated = glob_tx && glob_txgate;

#if 0
		/* +++ Управление согласующим устройством */
		/* дополнительный регистр */
		RBBIT(0073, glob_tx);				/* pin 03:индикатор передачи */
		RBBIT(0072, glob_antenna);			// pin 02: выбор антенны (0 - ANT1, 1 - ANT2)
		RBBIT(0071, ! glob_tuner_bypass);		// pin 01: обход СУ (1 - работа)
		RBBIT(0070, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 15: TYPE OF TUNER 	*/
		/* регистр управления массивом конденсаторов */
		RBVAL8(0060, glob_tuner_bypass ? 0 : glob_tuner_C);			/* Capacitors tuner bank 	*/
		/* регистр управления наборной индуктивностью. */
		RBVAL8(0050, glob_tuner_bypass ? 0 : glob_tuner_L);			/* Inductors tuner bank 	*/
		/* --- Управление согласующим устройством */

#endif
		// DD21 SN74HC595PW на разъём управления LPF
		RBBIT(0047, txgated);		// D7 - XS18 PIN 16: PTT
		RBVAL(0040, 1U << glob_bandf2, 7);		// D0..D6: band select бит выбора диапазонного фильтра передатчика

		// DD16 STP08CP05TTR в управлении диапазонными фильтрами приёмника
		RBVAL(0031, glob_tx ? 0 : (1U << glob_bandf) >> 1, 7);		// D1: 1, D7..D1: band select бит выбора диапазонного фильтра приёмника
		RBBIT(0030, txgated);		// D0: включение подачи смещения на выходной каскад усилителя мощности

		// DD15 STP08CP05TTR в управлении диапазонными фильтрами приёмника
		RBVAL(0026, glob_att, 2);			/* D7:D6: 12 dB and 6 dB attenuator control */
		RBVAL(0024, ~ (txgated ? powerxlat [glob_stage1level] : HARDWARE_OPA2674I_SHUTDOWN), 2);	// A1..A0 of OPA2674I-14D in stage 1
		RBBIT(0023, glob_fanflag);			/* D3: PA FAN */
		RBBIT(0022, glob_bandf == 0);		// D2: средневолновый ФНЧ - управление реле на выходе фильтров
		RBBIT(0021, glob_tx);				// D1: TX ANT relay
		RBBIT(0020, glob_bandf == 0);		// D0: средневолновый ФНЧ - управление реле на входе

		// DD18 SN74HC595PW рядом с DIN8
		RBNULL(0014, 4);
		RBVAL(0010, glob_bandf3, 4);			/* D3:D0: DIN8 EXT PA band select */

		// DD14 STP08CP05TTR рядом с DIN8
		RBBIT(0007, ! glob_reset_n);		// D7: NMEA reset
		RBBIT(0006, glob_tx);				// D6: DIN8 EXT PTT signal
		RBBIT(0005, 0);						// D5: not used
		RBBIT(0004, 0);						/* D4: not used */
		RBBIT(0003, lcdblcode & 0x02);		/* D3	- LCD backlight */
		RBBIT(0002, lcdblcode & 0x02);		/* D2	- LCD backlight */
		RBBIT(0001, lcdblcode & 0x01);		/* D2:D1 - LCD backlight */
		RBBIT(0000, glob_kblight);			/* D0: keyboard backlight */

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}
}

#elif CTLREGMODE_STORCH_V3	// USB FS, USB HS, DSP и FPGA, DUAL WATCH, SD-CARD & PA on board

#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// "Storch" с USB, DSP и FPGA, SD-CARD
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
#if defined(DDS1_TYPE)
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
#endif

	// registers chain control register
	{
		const uint_fast8_t lcdblcode = (glob_bglight - WITHLCDBACKLIGHTMIN);
		//Current Output at Full Power A1 = 1, A0 = 1, VO = 0 ±500 ±380 ±350 ±320 mA min A
		//Current Output at Power Cutback A1 = 1, A0 = 0, VO = 0 ±450 ±350 ±320 ±300 mA min A
		//Current Output at Idle Power A1 = 0, A0 = 1, VO = 0 ±100 ±60 ±55 ±50 mA min A

		enum
		{
			HARDWARE_OPA2674I_FULLPOWER = 0x03,
			HARDWARE_OPA2674I_POWERCUTBACK = 0x02,
			HARDWARE_OPA2674I_IDLEPOWER = 0x01,
			HARDWARE_OPA2674I_SHUTDOWN = 0x00
		};
		static const FLASHMEM uint_fast8_t powerxlat [] =
		{
			HARDWARE_OPA2674I_IDLEPOWER,
			HARDWARE_OPA2674I_POWERCUTBACK,
			HARDWARE_OPA2674I_FULLPOWER,
		};
		const spitarget_t target = targetctl1;

		rbtype_t rbbuff [8] = { 0 };
		const uint_fast8_t txgated = glob_tx && glob_txgate;

#if 0
		/* +++ Управление согласующим устройством */
		/* дополнительный регистр */
		RBBIT(0073, glob_tx);				/* pin 03:индикатор передачи */
		RBBIT(0072, glob_antenna);			// pin 02: выбор антенны (0 - ANT1, 1 - ANT2)
		RBBIT(0071, ! glob_tuner_bypass);		// pin 01: обход СУ (1 - работа)
		RBBIT(0070, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 15: TYPE OF TUNER 	*/
		/* регистр управления массивом конденсаторов */
		RBVAL8(0060, glob_tuner_bypass ? 0 : glob_tuner_C);			/* Capacitors tuner bank 	*/
		/* регистр управления наборной индуктивностью. */
		RBVAL8(0050, glob_tuner_bypass ? 0 : glob_tuner_L);			/* Inductors tuner bank 	*/
		/* --- Управление согласующим устройством */

#endif
		// DD21 SN74HC595PW + ULN2003APW на разъём управления LPF
		RBBIT(0047, txgated);		// D7 - XS18 PIN 16: PTT
		RBVAL(0040, 1U << glob_bandf2, 7);		// D0..D6: band select бит выбора диапазонного фильтра передатчика

		// DD20 SN74HC595PW в управлении диапазонными фильтрами приёмника
		RBVAL(0031, glob_tx ? 0 : (1U << glob_bandf) >> 1, 7);		// D1: 1, D7..D1: band select бит выбора диапазонного фильтра приёмника
		RBBIT(0030, txgated);		// D0: включение подачи смещения на выходной каскад усилителя мощности

		// DD19 SN74HC595PW в управлении диапазонными фильтрами приёмника
		RBVAL(0026, glob_att, 2);			/* D7:D6: 12 dB and 6 dB attenuator control */
		RBVAL(0024, ~ (txgated ? powerxlat [glob_stage1level] : HARDWARE_OPA2674I_SHUTDOWN), 2);	// A1..A0 of OPA2674I-14D in stage 1
		RBBIT(0023, glob_fanflag);			/* D3: PA FAN */
		RBBIT(0022, glob_bandf == 0);		// D2: средневолновый ФНЧ - управление реле на выходе фильтров
		RBBIT(0021, glob_tx);				// D1: TX ANT relay
		RBBIT(0020, glob_bandf == 0);		// D0: средневолновый ФНЧ - управление реле на входе

		// DD18 SN74HC595PW рядом с DIN8
		RBNULL(0014, 4);
		RBVAL(0010, glob_bandf3, 4);			/* D3:D0: DIN8 EXT PA band select */

		// DD14 STP08CP05TTR рядом с DIN8
		RBBIT(0007, ! glob_reset_n);		// D7: NMEA reset
		RBBIT(0006, glob_tx);				// D6: DIN8 EXT PTT signal
		RBBIT(0005, 0);						// D5: not used
		RBBIT(0004, 0);						/* D4: not used */
		RBBIT(0003, lcdblcode & 0x02);		/* D3	- LCD backlight */
		RBBIT(0002, lcdblcode & 0x02);		/* D2	- LCD backlight */
		RBBIT(0001, lcdblcode & 0x01);		/* D2:D1 - LCD backlight */
		RBBIT(0000, glob_kblight);			/* D0: keyboard backlight */

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}
}

#elif CTLREGMODE_STORCH_V7

/* TFT 4.3" "Аист" с DSP и FPGA STM32H743IIT6 */
/* TFT 4.3", 7" "Аист" с DSP и FPGA R7S721020VCFP */
/* TFT 4.3", 7" "Аист" с DSP и FPGA R7S721020VCFP, LVDS */

#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// "Storch" с USB, DSP и FPGA, SD-CARD, TFT 4.3"
static void
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{

#if defined(DDS1_TYPE)
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
#endif
	prog_rfadc_update();			// AD9246 vref divider update

	// registers chain control register
	{
		const uint_fast8_t lcdblcode = (glob_bglight - WITHLCDBACKLIGHTMIN);
		//Current Output at Full Power A1 = 1, A0 = 1, VO = 0 ±500 ±380 ±350 ±320 mA min A
		//Current Output at Power Cutback A1 = 1, A0 = 0, VO = 0 ±450 ±350 ±320 ±300 mA min A
		//Current Output at Idle Power A1 = 0, A0 = 1, VO = 0 ±100 ±60 ±55 ±50 mA min A

		enum
		{
			HARDWARE_OPA2674I_FULLPOWER = 0x03,
			HARDWARE_OPA2674I_POWERCUTBACK = 0x02,
			HARDWARE_OPA2674I_IDLEPOWER = 0x01,
			HARDWARE_OPA2674I_SHUTDOWN = 0x00
		};
		static const FLASHMEM uint_fast8_t powerxlat [] =
		{
			HARDWARE_OPA2674I_IDLEPOWER,
			HARDWARE_OPA2674I_POWERCUTBACK,
			HARDWARE_OPA2674I_FULLPOWER,
		};
		const spitarget_t target = targetctl1;

		rbtype_t rbbuff [9] = { 0 };
		const uint_fast8_t txgated = glob_tx && glob_txgate;

#if WITHAUTOTUNER
	#if WITHAUTOTUNER_AVBELNN
		// Плата управления LPF и тюнером от avbelnn

		// Геннадий схему брал на краснодарском форуме Аист сообщение 545 от avbelnn.
		// http://www.cqham.ru/forum/showthread.php?36525-QRP-SDR-трансивер-Аист-(Storch)&p=1541543&viewfull=1#post1541543

		RBBIT(0107, 0);	// REZ4
		RBBIT(0106, 0);	// REZ3
		RBBIT(0105, 0);	// REZ2_OC
		RBBIT(0104, glob_antenna);	// REZ1_OC -> antenna switch
		RBBIT(0103, ! (glob_tx && ! glob_autotune));	// HP/LP: 0: high power, 1: low power
		RBBIT(0102, glob_tx);
		RBBIT(0101, glob_fanflag);	// FAN
		// 0100 is a bpf7
		RBVAL(0072, 1U << glob_bandf2, 7);	// BPF7..BPF1 (fences: 2.4 MHz, 3.9 MHz, 7.4 MHz, 14.8 MHz, 22 MHz, 30 MHz, 50 MHz)
		RBBIT(0071, glob_tuner_type);		// TY
		RBBIT(0070, ! glob_tuner_bypass);	// в обесточенном состоянии - режим BYPASS
		RBVAL8(0060, glob_tuner_C);
		RBVAL8(0050, glob_tuner_L);

	#elif SHORTSET8 || FULLSET8

	#elif SHORTSET7 || FULLSET7

		/* +++ Управление согласующим устройством */
		/* регистр управления массивом конденсаторов */
		RBBIT(067, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 7: TYPE OF TUNER 	*/
		RBVAL(060, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_C) >> 1), 7);/* Capacitors tuner bank 	*/
		/* регистр управления наборной индуктивностью. */
		RBBIT(057, ! glob_tuner_bypass);		// pin 7: обход СУ
		RBVAL(050, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_L) >> 1), 7);			/* pin 15, 1..6: Inductors tuner bank 	*/
		/* --- Управление согласующим устройством */

	#else
		#error WITHAUTOTUNER and unknown details
	#endif
#endif /* WITHAUTOTUNER */

		// DD21 SN74HC595PW + ULN2003APW на разъём управления LPF
		RBBIT(0047, txgated);		// D7 - XS18 PIN 16: PTT
		RBVAL(0040, 1U << glob_bandf2, 7);		// D0..D6: band select бит выбора диапазонного фильтра передатчика

		// DD20 SN74HC595PW в управлении диапазонными фильтрами приёмника
		RBVAL(0031, glob_tx ? 0 : (1U << glob_bandf) >> 1, 7);		// D1: 1, D7..D1: band select бит выбора диапазонного фильтра приёмника
		RBBIT(0030, txgated);		// D0: включение подачи смещения на выходной каскад усилителя мощности

		// DD19 SN74HC595PW в управлении диапазонными фильтрами приёмника
		RBVAL(0026, glob_att, 2);			/* D7:D6: 12 dB and 6 dB attenuator control */
		RBVAL(0024, ~ (txgated ? powerxlat [glob_stage1level] : HARDWARE_OPA2674I_SHUTDOWN), 2);	// A1..A0 of OPA2674I-14D in stage 1
		RBBIT(0023, glob_fanflag);			/* D3: PA FAN - removed in LVDS version */
		RBBIT(0022, glob_bandf == 0);		// D2: средневолновый ФНЧ - управление реле на выходе фильтров
		RBBIT(0021, glob_tx);				// D1: TX ANT relay
		RBBIT(0020, glob_bandf == 0);		// D0: средневолновый ФНЧ - управление реле на входе

		// DD18 SN74HC595PW рядом с DIN8
		RBBIT(0017, glob_poweron);			// POWER_HOLD_ON added in next version
		RBBIT(0016, glob_fanflag);			// FAN_CTL added in LVDS version
		RBBIT(0015, glob_tx);				// EXT_PTT2 added in LVDS version
		RBBIT(0014, glob_tx);				// EXT_PTT added in LVDS version
		RBVAL(0010, glob_bandf3, 4);		/* D3:D0: DIN8 EXT PA band select */

		// DD14 STP08CP05TTR рядом с DIN8
		RBBIT(0007, ! glob_reset_n);		// D7: NMEA reset
		RBBIT(0006, glob_tx);				// D6: DIN8 EXT PTT signal - removed in LVDS version
		RBBIT(0005, 0);						// D5: not used
		RBBIT(0004, 0);						/* D4: not used */
		RBBIT(0003, lcdblcode & 0x02);		/* D3	- LCD backlight  - removed in LVDS version*/
		RBBIT(0002, lcdblcode & 0x02);		/* D2	- LCD backlight  - removed in LVDS version*/
		RBBIT(0001, lcdblcode & 0x01);		/* D2:D1 - LCD backlight  - removed in LVDS version*/
#if WITHKBDBACKLIGHT
		RBBIT(0000, glob_kblight);			/* D0: keyboard backlight */
#endif /* WITHKBDBACKLIGHT */

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}
}

#elif CTLREGMODE_STORCH_V9

/* TFT 4.3", 7" "Аист" с DSP и FPGA R7S721020VCFP, LVDS, дополнения для подключения трансвертора */

#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// "Storch" с USB, DSP и FPGA, SD-CARD, TFT 4.3"
static void
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{

#if defined(DDS1_TYPE)
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
#endif
	//prog_rfadc_update();			// AD9246 vref divider update

	// registers chain control register
	{
		const uint_fast8_t lcdblcode = (glob_bglight - WITHLCDBACKLIGHTMIN);
		//Current Output at Full Power A1 = 1, A0 = 1, VO = 0 ±500 ±380 ±350 ±320 mA min A
		//Current Output at Power Cutback A1 = 1, A0 = 0, VO = 0 ±450 ±350 ±320 ±300 mA min A
		//Current Output at Idle Power A1 = 0, A0 = 1, VO = 0 ±100 ±60 ±55 ±50 mA min A

		enum
		{
			HARDWARE_OPA2674I_FULLPOWER = 0x03,
			HARDWARE_OPA2674I_POWERCUTBACK = 0x02,
			HARDWARE_OPA2674I_IDLEPOWER = 0x01,
			HARDWARE_OPA2674I_SHUTDOWN = 0x00
		};
		static const FLASHMEM uint_fast8_t powerxlat [] =
		{
			HARDWARE_OPA2674I_IDLEPOWER,
			HARDWARE_OPA2674I_POWERCUTBACK,
			HARDWARE_OPA2674I_FULLPOWER,
		};
		const spitarget_t target = targetctl1;

		rbtype_t rbbuff [10] = { 0 };
		const uint_fast8_t txgated = glob_tx && glob_txgate;
		const uint_fast8_t xvrtr = bandf_calc_getxvrtr(glob_bandf);
		//PRINTF("prog_ctrlreg: glob_bandf=%d, xvrtr=%d\n", glob_bandf, xvrtr);

#if WITHAUTOTUNER
	#if WITHAUTOTUNER_AVBELNN
		// Плата управления LPF и тюнером от avbelnn

		// Схему брал на краснодарском форуме Аист сообщение 545 от avbelnn.
		// http://www.cqham.ru/forum/showthread.php?36525-QRP-SDR-трансивер-Аист-(Storch)&p=1541543&viewfull=1#post1541543

		RBBIT(0117, 0);	// REZ4
		RBBIT(0116, 0);	// REZ3
		RBBIT(0115, 0);	// REZ2_OC
		RBBIT(0114, glob_antenna);	// REZ1_OC -> antenna switch
		RBBIT(0113, ! (glob_tx && ! glob_autotune));	// HP/LP: 0: high power, 1: low power
		RBBIT(0112, glob_tx);
		RBBIT(0111, glob_fanflag);	// FAN
		// 0110 is a bpf7
		RBVAL(0102, 1U << glob_bandf2, 7);	// BPF7..BPF1 (fences: 2.4 MHz, 3.9 MHz, 7.4 MHz, 14.8 MHz, 22 MHz, 30 MHz, 50 MHz)
		RBBIT(0101, glob_tuner_type);		// TY
		RBBIT(0100, ! glob_tuner_bypass);	// в обесточенном состоянии - режим BYPASS
		RBVAL8(0070, glob_tuner_C);
		RBVAL8(0060, glob_tuner_L);

	#elif SHORTSET8 || FULLSET8

	#elif SHORTSET7 || FULLSET7

		/* +++ Управление согласующим устройством */
		/* регистр управления массивом конденсаторов */
		RBBIT(0077, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 7: TYPE OF TUNER 	*/
		RBVAL(0070, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_C) >> 1), 7);/* Capacitors tuner bank 	*/
		/* регистр управления наборной индуктивностью. */
		RBBIT(0067, ! glob_tuner_bypass);		// pin 7: обход СУ
		RBVAL(0060, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_L) >> 1), 7);			/* pin 15, 1..6: Inductors tuner bank 	*/
		/* --- Управление согласующим устройством */

	#else
		#error WITHAUTOTUNER and unknown details
	#endif
#endif /* WITHAUTOTUNER */

		// DD23 SN74HC595PW + ULN2003APW на разъём управления LPF
		RBBIT(0057, ! xvrtr && txgated);		// D7 - XS18 PIN 16: PTT
		RBVAL(0050, 1U << glob_bandf2, 7);		// D0..D6: band select бит выбора диапазонного фильтра передатчика

		// DD42 SN74HC595PW
		RBBIT(0047, xvrtr && ! glob_tx);	// D7 - XVR_RXMODE
		RBBIT(0046, xvrtr && glob_tx);		// D6 - XVR_TXMODE
		RBBIT(0045, 0);			// D5: CTLSPARE2
		RBBIT(0044, 0);			// D4: CTLSPARE1
		RBBIT(0043, 0);			// D3: not used
		RBBIT(0042, 0);			// D2: not used
		RBBIT(0041, 0);			// D1: not used
		RBBIT(0040, 0);			// D0: not used

		// DD22 SN74HC595PW в управлении диапазонными фильтрами приёмника
		RBVAL(0031, glob_tx ? 0 : (1U << glob_bandf) >> 1, 7);		// D1: 1, D7..D1: band select бит выбора диапазонного фильтра приёмника
		RBBIT(0030, ! xvrtr && glob_bandf != 0 && txgated);		// D0: включение подачи смещения на выходной каскад усилителя мощности

		// DD21 SN74HC595PW в управлении диапазонными фильтрами приёмника
		RBVAL(0026, glob_att, 2);			/* D7:D6: 12 dB and 6 dB attenuator control */
		RBVAL(0024, ~ ((! xvrtr && txgated) ? powerxlat [glob_stage1level] : HARDWARE_OPA2674I_SHUTDOWN), 2);	// A1..A0 of OPA2674I-14D in stage 1
		RBBIT(0023, xvrtr && glob_fanflag);			/* D3: XVRTR PA FAN */
		RBBIT(0022, xvrtr || (glob_bandf == 0));		// D2: средневолновый ФНЧ - управление реле на выходе фильтров
		RBBIT(0021, ! xvrtr && glob_tx);				// D1: TX ANT relay
		RBBIT(0020, glob_bandf == 0);		// D0: средневолновый ФНЧ - управление реле на входе

		// DD28 SN74HC595PW рядом с DIN8
		RBBIT(0017, glob_poweron);			// POWER_HOLD_ON added in next version
		RBBIT(0016, ! xvrtr && glob_fanflag);			// FAN_CTL added in LVDS version
		RBBIT(0015, ! xvrtr && glob_tx);				// EXT_PTT2 added in LVDS version
		RBBIT(0014, ! xvrtr && glob_tx);				// EXT_PTT added in LVDS version
		RBVAL(0010, glob_bandf3, 4);		/* D3:D0: DIN8 EXT PA band select */

		// DD20 STP08CP05TTR рядом с DIN8
		RBBIT(0007, ! glob_reset_n);		// D7: NMEA reset
		RBBIT(0006, 0);						/* D6: not used */
		RBBIT(0005, 0);						/* D5: not used */
		RBBIT(0004, 0);						/* D4: not used */
		RBBIT(0003, lcdblcode & 0x02);		/* D3	- LCD backlight  - removed in LVDS version*/
		RBBIT(0002, lcdblcode & 0x02);		/* D2	- LCD backlight  - removed in LVDS version*/
		RBBIT(0001, lcdblcode & 0x01);		/* D2:D1 - LCD backlight  - removed in LVDS version*/
		RBBIT(0000, glob_kblight);			/* D0: keyboard backlight */

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}
}


#elif CTLREGMODE_STORCH_V9A

/* STM32MP157, дополнения для подключения трансвертора */

#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{

#if defined(DDS1_TYPE)
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
#endif
	//prog_rfadc_update();			// AD9246 vref divider update

	// registers chain control register
	{
		//Current Output at Full Power A1 = 1, A0 = 1, VO = 0 ±500 ±380 ±350 ±320 mA min A
		//Current Output at Power Cutback A1 = 1, A0 = 0, VO = 0 ±450 ±350 ±320 ±300 mA min A
		//Current Output at Idle Power A1 = 0, A0 = 1, VO = 0 ±100 ±60 ±55 ±50 mA min A

		enum
		{
			HARDWARE_OPA2674I_FULLPOWER = 0x03,
			HARDWARE_OPA2674I_POWERCUTBACK = 0x02,
			HARDWARE_OPA2674I_IDLEPOWER = 0x01,
			HARDWARE_OPA2674I_SHUTDOWN = 0x00
		};
		static const FLASHMEM uint_fast8_t powerxlat [] =
		{
			HARDWARE_OPA2674I_IDLEPOWER,
			HARDWARE_OPA2674I_POWERCUTBACK,
			HARDWARE_OPA2674I_FULLPOWER,
		};
		const spitarget_t target = targetctl1;

		rbtype_t rbbuff [10] = { 0 };
		const uint_fast8_t txgated = glob_tx && glob_txgate;
		const uint_fast8_t xvrtr = bandf_calc_getxvrtr(glob_bandf);
		//PRINTF("prog_ctrlreg: glob_bandf=%d, xvrtr=%d\n", glob_bandf, xvrtr);

#if WITHAUTOTUNER
	#if WITHAUTOTUNER_AVBELNN
		// Плата управления LPF и тюнером от avbelnn

		// Схему брал на краснодарском форуме Аист сообщение 545 от avbelnn.
		// http://www.cqham.ru/forum/showthread.php?36525-QRP-SDR-трансивер-Аист-(Storch)&p=1541543&viewfull=1#post1541543

		RBBIT(0107, 0);	// REZ4
		RBBIT(0106, 0);	// REZ3
		RBBIT(0105, 0);	// REZ2_OC
		RBBIT(0104, glob_antenna);	// REZ1_OC -> antenna switch
		RBBIT(0103, ! (glob_tx && ! glob_autotune));	// HP/LP: 0: high power, 1: low power
		RBBIT(0102, glob_tx);
		RBBIT(0101, glob_fanflag);	// FAN
		// 0100 is a bpf7
		RBVAL(0072, 1U << glob_bandf2, 7);	// BPF7..BPF1 (fences: 2.4 MHz, 3.9 MHz, 7.4 MHz, 14.8 MHz, 22 MHz, 30 MHz, 50 MHz)
		RBBIT(0071, glob_tuner_type);		// TY
		RBBIT(0070, ! glob_tuner_bypass);	// в обесточенном состоянии - режим BYPASS
		RBVAL8(0060, glob_tuner_C);
		RBVAL8(0050, glob_tuner_L);

	#elif SHORTSET8 || FULLSET8

	#elif SHORTSET7 || FULLSET7

		/* +++ Управление согласующим устройством */
		/* регистр управления массивом конденсаторов */
		RBBIT(0067, glob_tuner_bypass ? 0 : glob_tuner_type);		/* pin 7: TYPE OF TUNER 	*/
		RBVAL(0060, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_C) >> 1), 7);/* Capacitors tuner bank 	*/
		/* регистр управления наборной индуктивностью. */
		RBBIT(0057, ! glob_tuner_bypass);		// pin 7: обход СУ
		RBVAL(0050, glob_tuner_bypass ? 0 : (revbits8(glob_tuner_L) >> 1), 7);			/* pin 15, 1..6: Inductors tuner bank 	*/
		/* --- Управление согласующим устройством */

	#else
		#error WITHAUTOTUNER and unknown details
	#endif
#endif /* WITHAUTOTUNER */

		// DD23 SN74HC595PW + ULN2003APW на разъём управления LPF
		RBBIT(0047, ! xvrtr && txgated);		// D7 - XS18 PIN 16: PTT
		RBVAL(0040, 1U << glob_bandf2, 7);		// D0..D6: band select бит выбора диапазонного фильтра передатчика

		// DD42 SN74HC595PW
		RBBIT(0037, xvrtr && ! glob_tx);	// D7 - XVR_RXMODE
		RBBIT(0036, xvrtr && glob_tx);		// D6 - XVR_TXMODE
		RBBIT(0035, 0);			// D5: CTLSPARE2
		RBBIT(0034, 0);			// D4: CTLSPARE1
		RBBIT(0033, 0);			// D3: not used
		RBBIT(0032, ! glob_bglightoff);			// D2: LCD_BL_ENABLE
		RBBIT(0031, 0);			// D1: not used
		RBBIT(0030, 0);			// D0: not used

		// DD22 SN74HC595PW в управлении диапазонными фильтрами приёмника
		RBVAL(0021, glob_tx ? 0 : (1U << glob_bandf) >> 1, 7);		// D1: 1, D7..D1: band select бит выбора диапазонного фильтра приёмника
		RBBIT(0020, ! xvrtr && glob_bandf != 0 && txgated);		// D0: включение подачи смещения на выходной каскад усилителя мощности

		// DD21 SN74HC595PW в управлении диапазонными фильтрами приёмника
		RBVAL(0016, glob_att, 2);			/* D7:D6: 12 dB and 6 dB attenuator control */
		RBVAL(0014, ~ ((! xvrtr && txgated) ? powerxlat [glob_stage1level] : HARDWARE_OPA2674I_SHUTDOWN), 2);	// A1..A0 of OPA2674I-14D in stage 1
		RBBIT(0013, xvrtr && glob_fanflag);			/* D3: XVRTR PA FAN */
		RBBIT(0012, xvrtr || (glob_bandf == 0));		// D2: средневолновый ФНЧ - управление реле на выходе фильтров
		RBBIT(0011, ! xvrtr && glob_tx);				// D1: TX ANT relay
		RBBIT(0010, glob_bandf == 0);		// D0: средневолновый ФНЧ - управление реле на входе

		// DD28 SN74HC595PW рядом с DIN8
		RBBIT(0007, glob_poweron);			// POWER_HOLD_ON added in next version
		RBBIT(0006, ! xvrtr && glob_fanflag);			// FAN_CTL added in LVDS version
		RBBIT(0005, ! xvrtr && glob_tx);				// EXT_PTT2 added in LVDS version
		RBBIT(0004, ! xvrtr && glob_tx);				// EXT_PTT added in LVDS version
		RBVAL(0000, glob_bandf3, 4);		/* D3:D0: DIN8 EXT PA band select */

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}
}

#elif CTLREGMODE_STORCH_V4
// Modem v2
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
#if defined(DDS1_TYPE)
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
#endif

	{
		const spitarget_t target = targetctl1;
		rbtype_t rbbuff [1] = { 0 };

		RBBIT(0007, glob_tx);					// антенное реле
		RBBIT(0005, glob_tx && glob_txgate);	// питание предварительного усилителя и цепь смещения оконечного
		RBBIT(0004, ! glob_tx);					// питание УВЧ

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}
}

#elif CTLREGMODE_STORCH_V5	// USB FS, USB HS, DSP и FPGA, DUAL WATCH, SD-CARD mimi rx board

#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// "Storch" с USB, DSP и FPGA, SD-CARD
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
#if defined(DDS1_TYPE)
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
#endif
#if WITHAUTOTUNER_UA1CEI
	ua1ceituner_send();
#endif /* WITHAUTOTUNER_UA1CEI */

	// registers chain control register
	{
		const uint_fast8_t lcdblcode = (glob_bglight - WITHLCDBACKLIGHTMIN);
		const spitarget_t target = targetctl1;

		rbtype_t rbbuff [2] = { 0 };

		// DD16 STP08CP05TTR в управлении диапазонными фильтрами приёмника
		RBVAL(0010, glob_tx ? 0 : (1U << glob_bandf), 8);		// D1: 1, D7..D1: band select бит выбора диапазонного фильтра приёмника

		// DD14 STP08CP05TTR рядом с DIN8
		//RBBIT(0007, ! glob_reset_n);		// D7: NMEA reset
		RBBIT(0007, glob_tx);				// D7: PTT out for UA1CEI
		RBVAL(0005, glob_att, 2);			/* D5:D5: 12 dB and 6 dB attenuator control */
		RBBIT(0004,  glob_bandf == 0);		/* D4: средневолновый ФНЧ - управление реле на выходе фильтров */
		RBBIT(0003, lcdblcode & 0x02);		/* D3	- LCD backlight */
		RBBIT(0002, lcdblcode & 0x02);		/* D2	- LCD backlight */
		RBBIT(0001, lcdblcode & 0x01);		/* D2:D1 - LCD backlight */
		RBBIT(0000, glob_tx);			/* D0: PTT out for UA1CEI keyboard backlight */

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}
}

#elif CTLREGMODE_STORCH_V6	// USB FS, USB HS, DSP и FPGA, DUAL WATCH, SD-CARD mimi rx board
// Rmainunit_v5la.pcb STM32H743IIT6, TFT 4.3", 2xmini-USB, mini SD-CARD, NAU8822L и FPGA EP4CE22E22I7N

#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// "Storch" с USB, DSP и FPGA, SD-CARD
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
#if defined(DDS1_TYPE)
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
#endif

#if WITHAUTOTUNER_UA1CEI
	ua1ceituner_send();
#endif /* WITHAUTOTUNER_UA1CEI */

	// registers chain control register
	{
		const uint_fast8_t lcdblcode = (glob_bglight - WITHLCDBACKLIGHTMIN);
		const spitarget_t target = targetctl1;

		rbtype_t rbbuff [6] = { 0 };

#if WITHAUTOTUNER
		RBBIT(0057, 0);	// REZ4
		RBBIT(0056, 0);	// REZ3
		RBBIT(0055, 0);	// REZ2_OC
		RBBIT(0054, 0);	// REZ1_OC
		RBBIT(0053, ! (glob_tx && ! glob_autotune));	// HP/LP: 0: high power, 1: low power
		RBBIT(0052, glob_tx);
		RBBIT(0051, glob_fanflag);	// FAN
		RBVAL(0042, 1U << glob_bandf2, 7);	// BPF7..BPF1 (fences: 2.4 MHz, 3.9 MHz, 7.4 MHz, 14.8 MHz, 22 MHz, 30 MHz, 50 MHz)
		RBBIT(0041, glob_tuner_type);		// TY
		RBBIT(0040, ! glob_tuner_bypass);	// в обесточенном состоянии - режим BYPASS
		RBVAL8(0030, glob_tuner_C);
		RBVAL8(0020, glob_tuner_L);
#endif /* WITHAUTOTUNER */

		// DD16 STP08CP05TTR в управлении диапазонными фильтрами приёмника
		RBVAL(0010, glob_tx ? 0 : (1U << glob_bandf), 8);		// D1: 1, D7..D1: band select бит выбора диапазонного фильтра приёмника

		// DD14 STP08CP05TTR рядом с DIN8
		RBBIT(0007, ! glob_reset_n);		// D7: NMEA reset
		RBVAL(0005, glob_att, 2);			/* D5:D5: 12 dB and 6 dB attenuator control */
		RBBIT(0004,  glob_bandf == 0);		/* D4: средневолновый ФНЧ - управление реле на выходе фильтров */
		RBBIT(0003, lcdblcode & 0x02);		/* D3	- LCD backlight */
		RBBIT(0002, lcdblcode & 0x02);		/* D2	- LCD backlight */
		RBBIT(0001, lcdblcode & 0x01);		/* D2:D1 - LCD backlight */
		RBBIT(0000, glob_tx/*glob_kblight*/);			/* D0: keyboard backlight */

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}
}

#elif CTLREGMODE_OLEG4Z_V1	// USB FS, USB HS, DSP и FPGA, DUAL WATCH, SD-CARD & PA on board

#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// "Storch" с USB, DSP и FPGA, SD-CARD
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
#if defined(DDS1_TYPE)
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
#endif

	// registers chain control register
	{
		const uint_fast8_t xvrtr = glob_bandf >= 11;
		uint_fast16_t xvtr_bandmask = (1U << 4);	// See R820T_IFFREQ
		uint_fast16_t bandmask = xvrtr ? xvtr_bandmask : 1U << glob_bandf;
		//bandmask = 0;
		//xvrtr = 0;
		const spitarget_t target = targetctl1;

		rbtype_t rbbuff [2] = { 0 };

		// U12 SN74HC595PW в управлении диапазонными фильтрами приёмника
		RBVAL(0011, bandmask, 7);		// QB..QH: band select бит выбора диапазонного фильтра передатчика
		RBBIT(0010, 0);						/*  */

		// U9 SN74HC595PW в управлении диапазонными фильтрами приёмника
		RBVAL(0006, xvrtr ? 0x03 : glob_att, 2);			// DG..DH: ATT
		RBVAL(0002, bandmask >> 7, 4);		// DC..DF: band select бит выбора диапазонного фильтра передатчика
		RBBIT(0001, 0);						// QB
		RBBIT(0000, xvrtr);					// QA: > 50 MHz ON

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}
}

#elif CTLREGMODE_OLEG4Z_V2	// USB FS, USB HS, DSP и FPGA, DUAL WATCH, SD-CARD & PA on board

#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

static uint_fast8_t adj8bits(uint_fast8_t v)
{
	uint_fast8_t b0 = (v & 0x80) != 0;
	return ((v & 0x7F) << 1) | b0;
}

// "Storch" с USB, DSP и FPGA, SD-CARD
static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
#if defined(DDS1_TYPE)
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
#endif

	// registers chain control register
	{
		enum { STARTNUX = 14 };
		enum { RF1, RF2, RF3, RF4, RF5, RF6 };
		const uint_fast8_t rfi = glob_bandf >= STARTNUX ? (glob_bandf - STARTNUX) : 5;
		uint_fast8_t xvrtr = glob_bandf >= 11;
		//uint_fast32_t xvtr_bandmask = ((uint_fast32_t) 1U << 4);	// See R820T_IFFREQ
		uint_fast32_t bandmask = (uint_fast32_t) 1U << glob_bandf;
		static const uint_fast8_t xltIN [] =  { RF5, RF4, RF3, RF2, RF1, /* */ RF6, RF6, RF6, RF6, RF6, RF6, RF6};	// каналы входного клммутатора HMC252AQS24E
		static const uint_fast8_t xltOUT [] = { RF4, RF1, RF5, RF2, RF3, /* */ RF6, RF6, RF6, RF6, RF6, RF6, RF6};	// каналы выходного коммутатора HMC252AQS24E
		const uint_fast8_t uhfmuxIN = xltIN [rfi];		// U2 HMC252AQS24E - BPF_ON_17,BPF_ON_16,BPF_ON_15
		const uint_fast8_t uhfmuxOUT = xltOUT [rfi];		// U3 HMC252AQS24E - BPF_ON_20, BPF_ON_19, BPF_ON_18
		const uint_fast8_t bpfon15 = (uhfmuxIN & 0x01) != 0;
		const uint_fast8_t bpfon16 = (uhfmuxIN & 0x02) != 0;
		const uint_fast8_t bpfon17 = (uhfmuxIN & 0x04) != 0;
		const uint_fast8_t bpfon18 = (uhfmuxOUT & 0x01) != 0;
		const uint_fast8_t bpfon19 = (uhfmuxOUT & 0x02) != 0;
		const uint_fast8_t bpfon20 = (uhfmuxOUT & 0x04) != 0;

		//PRINTF(PSTR("prog_ctrlreg: glob_bandf=%u, uhfmuxIN=%02X, uhfmuxOUT=%02X\n"), glob_bandf, uhfmuxIN, uhfmuxOUT);

		//bandmask = 0;
		//xvrtr = 0;
		const spitarget_t target = targetctl1;

		rbtype_t rbbuff [4] = { 0 };

		RBBIT(031, bpfon20);	

		RBVAL(026, glob_att, 2);			// HF ATT
		RBVAL(024, ~ glob_att, 2);			// UHF ATT
		RBBIT(023, bpfon19);
		RBBIT(022, bpfon18);
		RBBIT(021, bpfon17);
		RBBIT(020, xvrtr);

		RBBIT(017, bpfon15);
		RBVAL(011, bandmask >> 8, 6);
		RBBIT(010, bpfon16);

		RBVAL(000, adj8bits(bandmask >> 0), 8);
		//RBBIT(0007, 1);	

		spi_select(target, CTLREG_SPIMODE);
		prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
		spi_unselect(target);
	}
}

#elif CTLREGMODE_V7_ARM //ARM_CTLSTYLE_V7_H_INCLUDED	//  новая плата с двумя аттенюаторами, без или с FM, с УВЧ - совмещённая с синтезатором, 6 ГУН

#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

/* ctl register interface */
static void 
//NOINLINEAT
prog_ctldacreg(void)	// CTLSTYLE_V7 @ ARM
{
	const spitarget_t target = targetctldac1;
#if defined (LO1MODE_HYBRID) || defined (LO1MODE_FIXSCALE)
	const uint_fast8_t vcomask = (1U << glob_vco);
#else
	const uint_fast8_t vcomask = 0;
#endif

	prog_select(target);	/* start sending data to target chip */

#if defined (DAC1_TYPE)
	prog_val(target, glob_dac1, 8);
#endif /* defined (DAC1_TYPE) */

	/* регистр управления (74HC595), расположенный на плате синтезатора */
	prog_bit(target, ! glob_reset_n);		/* d7 in control register - ad9951 RESET */
	prog_bit(target, 0x00 /* glob_bglight */);				/* d6 in control register - LD light ON */
	prog_val(target, vcomask, 6);	/* d0..d5 in control register */

	prog_unselect(target);	/* done sending data to target chip */
}

static void 
//NOINLINEAT
prog_rxctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetrxc1;
	const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

	prog_select(target);	/* start sending data to target chip */
	/* учет диапазона - на 0-м диапазоне обход УВЧ включается принудительно */
	prog_val(target, glob_bandf, 4);			// D4..D7: band select код выбора диапазонного фильтра
	prog_val(target, glob_att, 2);	/* D3:D2: ATTENUATOR RELAYS POWER */
	prog_bit(target, glob_preamp && (glob_bandf != 0));				/* D1: RF amplifier */
	prog_bit(target, glob_tx);					/* D0: TX mode: 1 - TX режим передачи */

	// programming RX control registers
	// filter codes:
	// 0x06 - fil3 3.1 kHz
	// 0x01 - fil2 2.7 kHz
	// 0x00 - fil0 9.0 kHz
	// 0x02 - fil1 6.0 kHz
	// 0x03 - fil5 0.5 kHz
	// 0x04 - fil2 15 kHz (bypass)
	// 0x05 - unused (9 kHz filter in FM strip)
	// 0x07 - unused (6 kHz filter in FM strip)
	prog_val(target, glob_filter, 3);	/* D5, D6, D7: select IF filter, wrong order of bits */
	prog_bit(target, 0x00);		/* D4 unused output */
	prog_bit(target, 0x00);		/* D3 unused output */
	prog_bit(target, glob_tx ? 0x00 : fm);		/* D2: FM DETECTOR POWER */
	prog_bit(target, 0x00);	/* D1 unused output  */
	prog_bit(target, 0x00);	/* D0 unused output  */

	prog_bit(target, (glob_agc == BOARD_AGCCODE_OFF));		/* D7: AGC OFF */
	prog_val(target, glob_agc, 3);	/* D4,D5,D6:  AGC code (delay) */
	prog_val(target, glob_tx ? BOARD_DETECTOR_MUTE : glob_af_input, 2);	/* D2..D3: AF input selection 0-ssb, 1-am, 2-mute, 3-fm */
	prog_bit(target, glob_tx);		/* D1: IF amp ad605 OFF in TX mode */
	prog_bit(target, fm || am);	// D0: switch lo4 off in AM and FM modes

	prog_unselect(target);	/* done sending data to target chip */


}

#elif CTLREGMODE_V7_ATMEGA //ATMEGA_CTLSTYLE_V7_H_INCLUDED	//  новая плата с двумя аттенюаторами, без или с FM, с УВЧ - совмещённая с синтезатором, 6 ГУН
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

// Ьез FM
/* ctl register interface */
static void 
//NOINLINEAT
prog_ctldacreg(void)	// CTLSTYLE_V7 @ ATMEGA
{
	const spitarget_t target = targetctldac1;
#if defined (LO1MODE_HYBRID) || defined (LO1MODE_FIXSCALE)
	const uint_fast8_t vcomask = (1U << glob_vco);
#else
	const uint_fast8_t vcomask = 0;
#endif

	prog_select(target);	/* start sending data to target chip */

#if defined (DAC1_TYPE)
	prog_val(target, glob_dac1, 8);
#endif /* defined (DAC1_TYPE) */

	/* регистр управления (74HC595), расположенный на плате синтезатора */
	prog_bit(target, ! glob_reset_n);		/* d7 in control register - ad9951 RESET */
	prog_bit(target, 0x00 /* glob_bglight */);				/* d6 in control register - LD light ON */
	prog_val(target, vcomask, 6);	/* d0..d5 in control register */

	prog_unselect(target);	/* done sending data to target chip */
}

static void 
//NOINLINEAT
prog_rxctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetrxc1;
	const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated


	prog_select(target);	/* start sending data to target chip */
	/* учет диапазона - на 0-м диапазоне обход УВЧ включается принудительно */
	prog_val(target, glob_bandf, 4);			// D4..D7: band select код выбора диапазонного фильтра
	prog_bit(target, 0 != (glob_att & 0x02));	/* D3: second stage (20 dB) atteuator on */
	prog_bit(target, 0 != (glob_att & 0x01));	/* D2: 10 dB ATTENUATOR RELAYS POWER */
	prog_bit(target, glob_preamp && (glob_bandf != 0));				/* D1: RF amplifier */
	prog_bit(target, glob_tx);					/* D0: TX mode: 1 - TX режим передачи */

	// programming RX control registers
	// filter codes:
	// 0x06 - fil3 3.1 kHz
	// 0x01 - fil2 2.7 kHz
	// 0x00 - fil0 9.0 kHz
	// 0x02 - fil1 6.0 kHz
	// 0x03 - fil5 0.5 kHz
	// 0x04 - fil2 15 kHz (bypass)
	// 0x05 - unused (9 kHz filter in FM strip)
	// 0x07 - unused (6 kHz filter in FM strip)
	prog_val(target, glob_filter, 3);	/* D5, D6, D7: select IF filter, wrong order of bits */
	prog_bit(target, 0x00);		/* D4 unused output */
	prog_bit(target, 0x00);		/* D3 unused output */
	prog_bit(target, glob_tx ? 0x00 : fm);		/* D2: FM DETECTOR POWER */
	prog_bit(target, 0x00);	/* D1 unused output  */
	prog_bit(target, 0x00);	/* D0 unused output  */

	prog_bit(target, (glob_agc == BOARD_AGCCODE_OFF));		/* D7: AGC OFF */
	prog_val(target, glob_agc, 3);	/* D4,D5,D6:  AGC code (delay) */
	prog_val(target, glob_tx ? BOARD_DETECTOR_MUTE : glob_af_input, 2);	/* D2..D3: AF input selection 0-ssb, 1-am, 2-mute, 3-fm */
	prog_bit(target, glob_tx);		/* D1: IF amp ad605 OFF in TX mode */
	prog_bit(target, fm || am);	// D0: switch lo4 off in AM and FM modes

	prog_unselect(target);	/* done sending data to target chip */


}

#elif ARM_CTLSTYLE_V7A_H_INCLUDED		//  новая плата с двумя аттенюаторами, без или с FM, с УВЧ - совмещённая с синтезатором
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */
/* ctl register interface */

static void 
//NOINLINEAT
prog_ctldacreg(void)	// ARM_CTLSTYLE_V7A_H_INCLUDED
{
	const spitarget_t target = targetctldac1;
#if defined (LO1MODE_HYBRID) || defined (LO1MODE_FIXSCALE)
	const uint_fast8_t vcomask = (1U << glob_vco);
#else
	const uint_fast8_t vcomask = 0;
#endif

	prog_select(target);	/* start sending data to target chip */

	/* регистр управления (74HC595), расположенный на плате синтезатора */
	prog_bit(target, ! glob_reset_n);		/* d7 in control register - ad9951 RESET */
	prog_bit(target, 0x00 /* glob_bglight */);				/* d6 in control register - LD light ON */
	prog_val(target, vcomask, 6);	/* d0..d5 in control register */

	prog_unselect(target);	/* done sending data to target chip */


}

static void 
//NOINLINEAT
prog_rxctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetrxc1;
	const uint_fast8_t fm = glob_af_input == BOARD_DETECTOR_FM;	// FM mode activated
	const uint_fast8_t am = glob_af_input == BOARD_DETECTOR_AM;	// AM mode activated

	prog_select(target);	/* start sending data to target chip */

	/* учет диапазона - на 0-м диапазоне обход УВЧ включается принудительно */
	prog_val(target, glob_bandf, 4);			// D4..D7: band select код выбора диапазонного фильтра
	prog_bit(target, 0 != (glob_att & 0x02));	/* D3: second stage (20 dB) atteuator on */
	prog_bit(target, 0 != (glob_att & 0x01));	/* D2: 10 dB ATTENUATOR RELAYS POWER */
	prog_bit(target, glob_preamp && (glob_bandf != 0));				/* D1: RF amplifier */
	prog_bit(target, glob_tx);					/* D0: TX mode: 1 - TX режим передачи */

	// programming RX control registers
	// filter codes:
	// 0x06 - fil3 3.1 kHz
	// 0x01 - fil2 2.7 kHz
	// 0x00 - fil0 9.0 kHz
	// 0x02 - fil1 6.0 kHz
	// 0x03 - fil5 0.5 kHz
	// 0x04 - fil2 15 kHz (bypass)
	// 0x05 - unused (9 kHz filter in FM strip)
	// 0x07 - unused (6 kHz filter in FM strip)
	prog_val(target, glob_filter, 3);	/* D5, D6, D7: select IF filter, wrong order of bits */
	prog_bit(target, glob_tx);			/* D4: TX mode: 1 - TX режим передачи */
	prog_bit(target, glob_tx ? 0x00 : am);		/* D3: AM DETECTOR POWER */
	prog_bit(target, glob_tx ? 0x00 : fm);		/* D2: FM DETECTOR POWER */
	prog_bit(target, (glob_agc == BOARD_AGCCODE_OFF));	/* D1: AGC OFF */
	prog_bit(target, 0x00);	/* D0 unused output  */

	prog_val(target, glob_agc, 3);	/* D5,D6,D7:  AGC code (delay) */
	prog_val(target, glob_tx ? BOARD_DETECTOR_MUTE : glob_af_input, 2);	/* D3..D4: AF input selection 0-ssb, 1-am, 2-mute, 3-fm */
	prog_bit(target, glob_tx && glob_txcw);	/* D2: DDB_MOD_UNBALANCE  */
	prog_bit(target, glob_mikemute);	/* D1: MIKE_AMP_MUTE  */
	prog_bit(target, fm || am);	// D0: switch lo4 off in AM and FM modes

	prog_unselect(target);	/* done sending data to target chip */


}

#elif CTLREGSTYLE_DISCO32
	// FPGA_V2
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

static void 
prog_ctrlreg(uint_fast8_t plane)
{
	//const spitarget_t target = targetctl1;
	prog_fpga_update(targetfpga1);
}

#elif CTLREGSTYLE_WDKP
#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	//const spitarget_t target = targetctl1;
}

#elif CTLREGMODE_4Z5KY_V1

	#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

/* Синтезатор 4Z5KY с двухстрочником http://ur5yfv.ucoz.ua/forum/28-19-2 */

static void 
//NOINLINEAT
prog_ctrlreg(uint_fast8_t plane)
{
	const spitarget_t target = targetctl1;
	rbtype_t rbbuff [2] = { 0 };

	/* регистр управления (74HC595), дальше от процессора */
	RBBIT(017, glob_att);			/* pin 07 ATTEN */
	RBBIT(016, glob_preamp);		/* pin 06 PRE */
	RBBIT(015, glob_af_input == BOARD_DETECTOR_FM);	/* pin 05 reserved */
	RBBIT(014, glob_affilter);		/* pin 04 NARROW */
	RBBIT(013, glob_notch);			/* pin 03 NOTCH */
	RBBIT(012, glob_vox);			/* pin 02 VOX */
	RBBIT(011, 0);					/* pin 01 COMP */
	RBBIT(010, glob_tx);			/* pin 15 TX (was: reserved) */

	/* регистр управления (74HC595), ближе к процессору */
	//RBVAL(04, bandcode, 4);		/* pin 07:04: BAND3:BAND0 */
	RBBIT(007, glob_bandf & 0x01);	/* pin 07 - BAND0 */
	RBBIT(006, glob_bandf & 0x02);	/* pin 06 - BAND1 */
	RBBIT(005, glob_bandf & 0x04);	/* pin 05 - BAND2 */
	RBBIT(004, glob_bandf & 0x08);	/* pin 04 - BAND3 */

	RBBIT(003, ! glob_filter);		/* pin 03 - включение широкого фильтра по ПЧ, was: NOR */
	RBBIT(002, glob_af_input == BOARD_DETECTOR_AM);	/* pin 02 - REVERS AM MODE */
	RBBIT(001, glob_filter);		/* pin 01 - включение узкого фильтра по ПЧ, was: +CW */
	RBBIT(000, glob_tx && glob_txcw);/* pin 15 - передача в режиме телеграфа, was: reserved */

	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}



#elif CTLREGMODE_NOCTLREG

	#define BOARD_NPLANES	1	/* в данной конфигурации не требуется обновлять множество регистров со "слоями" */

	static void 
	//NOINLINEAT
	prog_ctrlreg(uint_fast8_t plane)
	{
		//const spitarget_t target = targetctl1;
	}

#else

	#error No suitable CTLREGMODE_xxx or CTLSTYLE_xxx macro defined

#endif

#if CTLREGMODE_RAVENDSP_V3 || CTLREGMODE_RAVENDSP_V4 || CTLREGMODE_RAVENDSP_V5

static void 
board_update_rxctrlreg(void)
{
	uint_fast8_t plane;
	for (plane = 0; plane < BOARD_NPLANES; ++ plane)
		prog_rxctrlreg(plane); 	/* управление приемником */
}

#else

static void 
board_update_ctrlreg(void)
{
	uint_fast8_t plane;
	for (plane = 0; plane < BOARD_NPLANES; ++ plane)
		prog_ctrlreg(plane); 	/* управление приемником */
	}

#endif

// Обнуление теневых переменных, синхронизация регистров с теневыми переменными.
// Обнулние выполняется start-up, Выдаём в регистры текущее состояние
static void board_update_initial(void)
{
	prog_gpioreg();
#if CTLREGMODE_RAVENDSP_V3 || CTLREGMODE_RAVENDSP_V4 || CTLREGMODE_RAVENDSP_V5
	prog_ctldacreg();	/* регистр выбора ГУН, сброс DDS и ЦАП подстройки опорника */
	board_update_rxctrlreg();
#elif (ATMEGA_CTLSTYLE_V7_H_INCLUDED || ARM_CTLSTYLE_V7_H_INCLUDED || ARM_CTLSTYLE_V7A_H_INCLUDED)
	prog_ctldacreg();	/* регистр выбора ГУН, сброс DDS и ЦАП подстройки опорника */
	board_update_rxctrlreg();
#else
	board_update_ctrlreg();
#endif
	prog_dsplreg();				// обновление регистров DSP
	prog_fltlreg();				// обновление регистров DSP
}

// Обновление непосредственно подключенных к процессору сигналов
static void
board_update_direct(void)
{
}

// Обновление прямо подключенных к SPI регистру сигналов
static void
board_update_spi(void)
{
	
}

// Зависящие от предидущих (например, кодек, RESET которого формируется через SPI регистр)
static void 
board_update_spi2(void)
{
	
}

// Выдача в регистры текущего состояния теневых переменных.
void 
//NOINLINEAT
board_update(void)
{
	//PRINTF(PSTR("board_update start.\n"));

	board_update_direct();	// Обновление непосредственно подключенных к процессору сигналов
	board_update_spi();		// Обновление прямо подключенных к SPI регистру сигналов
	board_update_spi2();	// Зависящие от предидущих (например, кодек, RESET которого формируется через SPI регистр)


#if CTLREGMODE_RAVENDSP_V3 || CTLREGMODE_RAVENDSP_V4 || CTLREGMODE_RAVENDSP_V5
	if (flag_ctldacreg != 0)
	{
		flag_ctldacreg = 0;
		prog_ctldacreg();	/* регистр выбора ГУН, сброс DDS и ЦАП подстройки опорника */
	}
	if (flag_ctrlreg != 0)
	{
		flag_ctrlreg = 0;
		prog_gpioreg();
		uint_fast8_t plane;
		for (plane = 0; plane < BOARD_NPLANES; ++ plane)
			prog_rxctrlreg(plane); 	/* управление приемником */
	}

#elif ATMEGA_CTLSTYLE_V7_H_INCLUDED || ARM_CTLSTYLE_V7_H_INCLUDED || ARM_CTLSTYLE_V7A_H_INCLUDED
	if (flag_ctldacreg != 0)
	{
		flag_ctldacreg = 0;
		prog_ctldacreg();	/* регистр выбора ГУН, сброс DDS и ЦАП подстройки опорника */
	}
	if (flag_rxctrlreg != 0)
	{
		flag_rxctrlreg = 0;
		prog_gpioreg();
		board_update_rxctrlreg();
	}
#else
	if (flag_ctrlreg != 0)
	{
		flag_ctrlreg = 0;
		prog_gpioreg();
		board_update_ctrlreg();
	}
#endif

	prog_dsplreg_update();		// услолвное обновление регистров DSP
	prog_fltlreg_update();		// услолвное обновление регистров DSP
	prog_codecreg_update();		// услолвное обновление регистров аудио кодека
	//PRINTF(PSTR("board_update done.\n"));
}

/* Установка запроса на обновление сигналов управления */
static void
board_ctlreg1changed(void)
{
#if CTLREGMODE_RAVENDSP_V3 || CTLREGMODE_RAVENDSP_V4 || CTLREGMODE_RAVENDSP_V5
		flag_ctrlreg = 1;	/* управление синтезатором  */
#elif ATMEGA_CTLSTYLE_V7_H_INCLUDED || ARM_CTLSTYLE_V7_H_INCLUDED || ARM_CTLSTYLE_V7A_H_INCLUDED
		flag_rxctrlreg = 1;	/* управление приемником */
#else
		flag_ctrlreg = 1;
#endif
}

/* Установка запроса на обновление сигналов управления */
static void
board_ctlreg2changed(void)
{
#if CTLREGMODE_RAVENDSP_V3 || CTLREGMODE_RAVENDSP_V4 || CTLREGMODE_RAVENDSP_V5
	flag_ctldacreg = 1;	/* управление ЦАП */
#elif ATMEGA_CTLSTYLE_V7_H_INCLUDED || ARM_CTLSTYLE_V7_H_INCLUDED || ARM_CTLSTYLE_V7A_H_INCLUDED
	flag_ctldacreg = 1;	/* управление синтезатором и ЦАП */
#else
	flag_ctrlreg = 1; 
#endif
}

/////////////////////////////////////////////
// +++ Набор функций требования установки сигналов на управляющих выходах.
/////////////////////////////////////////////

/* установить код выбора диапазонного фильтра */
void 
board_set_bandf(uint_fast8_t n)
{
	if (glob_bandf != n)
	{
		glob_bandf = n;
		board_ctlreg1changed();
	}
}
/* установить код выбора диапазонного фильтра, с которого включается ФВЧ перед УВЧ в SW20xx */
void 
board_set_bandfonhpf(uint_fast8_t n)
{
	if (glob_bandfonhpf != n)
	{
		glob_bandfonhpf = n;
		board_ctlreg1changed();
	}
}

/* Установить код диапазонного фильтра, на котором вкдючать UHF */
void 
board_set_bandfonuhf(uint_fast8_t n)
{
	if (glob_bandfonhpf != n)
	{
		glob_bandfonuhf = n;
		board_ctlreg1changed();
	}
}


/* установить код выбора диапазонного фильтра (или ФНЧ) передатчика */
void 
board_set_bandf2(uint_fast8_t n)
{
	if (glob_bandf2 != n)
	{
		glob_bandf2 = n;
		board_ctlreg1changed();
	}
}

/* управление через разъем ACC */
void 
board_set_bandf3(uint_fast8_t n)
{
	if (glob_bandf3 != n)
	{
		glob_bandf3 = n;
		board_ctlreg1changed();
	}
}

void 
board_set_pabias(uint_fast8_t n)
{
	if (glob_pabias != n)
	{
		glob_pabias = n;
		board_ctlreg1changed();
	}
}

void 
board_set_filter(uint_fast16_t n)
{
	if (glob_filter != n)
	{
		glob_filter = n;
		board_ctlreg1changed();
	}
}

// Функция должна быть независима от архитектуры управляющенр регистра
void 
board_set_detector(uint_fast8_t n)
{
	if (glob_af_input != n)
	{
		glob_af_input = n;
		board_ctlreg1changed();
	}
}

void 
board_set_nfm(uint_fast8_t v)
{
	const uint_fast8_t n = v != 0;
	if (glob_nfm != n)
	{
		glob_nfm = n;
		board_ctlreg1changed();
	}
}

/* Включние noise blanker на SW2014FM */
void 
board_set_nfmnbon(uint_fast8_t v)
{
	const uint_fast8_t n = v != 0;
	if (glob_nfmnbon != n)
	{
		glob_nfmnbon = n;
		board_ctlreg1changed();
	}
}

void 
board_set_agc(uint_fast8_t n)
{
	if (glob_agc != n)
	{
		glob_agc = n;
		board_ctlreg1changed();
		board_dsp1regchanged();
	}
}


/* установить выходную мощность WITHPOWERTRIMMIN..WITHPOWERTRIMMAX */
void 
board_set_opowerlevel(uint_fast8_t n)
{
	if (glob_opowerlevel != n)
	{
		glob_opowerlevel = n;
		board_ctlreg1changed();
		board_dsp1regchanged();
	}
}


/* значение на выходе ЦАП для увода частоты опорного генератора PLL */
void 
board_set_maxlabdac(uint_fast16_t n)
{
	if (glob_maxlabdac != n)
	{
		glob_maxlabdac = n;
		board_ctlreg1changed();
	}
}

/* не нулевой аргумент - прием нижней боковой в приемнике прямого преобразования */
void
board_set_if4lsb(uint_fast8_t v)	/* требуется для приемников прямого преобразования */
{
	const uint_fast8_t n = v != 0;
	if (glob_if4lsb != n)
	{
		glob_if4lsb = n;
		board_ctlreg1changed();
	}
}

void 
board_set_lctl1(uint_fast8_t v)
{
	const uint_fast8_t n = v != 0;
	if (glob_lctl1 != n)
	{
		glob_lctl1 = n;
		board_ctlreg1changed();
	}
}

/* формирование сигнала "RESET" для codec2. 0 - снять reset. */
void board_codec2_nreset(uint_fast8_t v)
{
	const uint_fast8_t n = v != 0;
	if (glob_codec2_nreset != n)
	{
		glob_codec2_nreset = n;
		board_ctlreg1changed();
	}
}

/* выставить уровень на сигнале lcd reset, 1 - снять reset. */
void board_lcd_reset(uint_fast8_t v)	
{
	const uint_fast8_t n = v != 0;
	if (glob_lcdreset != n)
	{
		glob_lcdreset = n;
		board_ctlreg1changed();
	}
}


/* включение на передачу */
void
board_set_tx(uint_fast8_t v)
{
	const uint_fast8_t n = v != 0;
	if (glob_tx != n)
	{
		glob_tx = n;
		board_ctlreg1changed();
	}
}

/* перевести в режим минимального потребления */
void board_set_sleep(uint_fast8_t v)
{
	const uint_fast8_t n = v != 0;
	if (glob_sleep != n)
	{
		glob_sleep = n;
		board_ctlreg1changed();
	}
}

/* включить вентилятор */
void
board_setfanflag(uint_fast8_t v)
{
	const uint_fast8_t n = v != 0;
	if (glob_fanflag != n)
	{
		glob_fanflag = n;
		board_ctlreg1changed();
	}
}

/* отключить микрофонный усилитель */
void
board_set_mikemute(uint_fast8_t v)
{
	const uint_fast8_t n = v != 0;
	if (glob_mikemute != n)
	{
		glob_mikemute = n;
		board_ctlreg1changed();
		board_dsp1regchanged();
	}
}

/* включение подсветки дисплея */
void
board_set_bglight(uint_fast8_t dispoff, uint_fast8_t dispbright)
{
	if (glob_bglightoff != dispoff || glob_bglight != dispbright)
	{
		glob_bglightoff = dispoff;
		glob_bglight = dispbright;
		board_ctlreg1changed();
	}
}

#if WITHDCDCFREQCTL
/* установка делителя для формирования рабочей частоты преобразователя подсветки */
void 
board_set_blfreq(uint_fast32_t n)	
{
	// TIM16_CH1 - PF6
	if (glob_blfreq != n)
	{
		glob_blfreq = n;
		//board_ctlreg1changed();
		HARDWARE_DCDC_SETDIV(n);
	}
}

#endif /* WITHDCDCFREQCTL */

#if WITHKBDBACKLIGHT
/* включение подсветки клавиатуры */
void 
board_set_kblight(uint_fast8_t v)
{
	const uint_fast8_t n = v != 0;
	if (glob_kblight != n)
	{
		glob_kblight = n;
		board_ctlreg1changed();
	}
}
#endif /* WITHKBDBACKLIGHT */

#if WITHPWBUTTON	/* Наличие схемы электронного включения питания */
/* Включение удерхания схемы питания */
void
board_set_poweron(uint_fast8_t v)
{
	const uint_fast8_t n = v != 0;
	if (glob_poweron != n)
	{
		glob_poweron = n;
		board_ctlreg1changed();
	}
}
#endif /* WITHPWBUTTON */

/* включение УВЧ */
void
board_set_preamp(uint_fast8_t v)
{
	uint_fast8_t n = v != 0;
	if (glob_preamp != n)
	{
		glob_preamp = n;
		board_ctlreg1changed();
	}
}

void
board_set_att(
	uint_fast8_t n		// 0..2 - grade of attenuation
	)
{
	if (glob_att != n)
	{
		glob_att = n;
		board_ctlreg1changed();
	}
}

void
board_set_antenna(
	uint_fast8_t n		// номер антенны
	)
{
	if (glob_antenna != n)
	{
		glob_antenna = n;
		board_ctlreg1changed();
	}
}

void
board_set_dac1(uint_fast8_t n)	/* подстройка опорного генератора */
{
	if (glob_dac1 != n)
	{
		glob_dac1 = n;
		board_ctlreg2changed();
	}
}

void
board_set_adcfifo(uint_fast8_t v)
{
	uint_fast8_t n = v != 0;
	if (glob_adcfifo != n)
	{
		glob_adcfifo = n;
		board_ctlreg1changed();
	}
}

/* смещение для выходного сигнала с АЦП */
void
board_set_adcoffset(int_fast16_t n)
{
	if (glob_adcoffset != n)
	{
		glob_adcoffset = n;
		board_ctlreg1changed();
	}
}


void
board_set_dactest(uint_fast8_t v)
{
	uint_fast8_t n = v != 0;
	if (glob_dactest != n)
	{
		glob_dactest = n;
		board_ctlreg1changed();
	}
}

void
board_set_tx_inh_enable(uint_fast8_t v)
{
	uint_fast8_t n = v != 0;
	if (glob_tx_inh_enable != n)
	{
		glob_tx_inh_enable = n;
		board_ctlreg1changed();
	}
}

/* разрешение прямого формирования модуляции в FPGA */
void
board_set_tx_bpsk_enable(uint_fast8_t v)
{
	uint_fast8_t n = v != 0;
	if (glob_tx_bpsk_enable != n)
	{
		glob_tx_bpsk_enable = n;
		board_ctlreg1changed();
	}
}

void
board_set_mode_wfm(uint_fast8_t v)
{
	uint_fast8_t n = v != 0;
	if (glob_mode_wfm != n)
	{
		glob_mode_wfm = n;
		board_ctlreg1changed();
	}
}

void
board_set_xvrtr(uint_fast8_t v)
{
	uint_fast8_t n = v != 0;
	if (glob_xvrtr != n)
	{
		glob_xvrtr = n;
		board_ctlreg1changed();
	}
}

void
board_set_dacstraight(uint_fast8_t v)
{
	uint_fast8_t n = v != 0;
	if (glob_dacstraight != n)
	{
		glob_dacstraight = n;
		board_ctlreg1changed();
	}
}

/* управление зашумлением в LTC2208 */
void
board_set_dither(uint_fast8_t v)
{
	uint_fast8_t n = v != 0;
	if (glob_dither != n)
	{
		glob_dither = n;
		board_ctlreg1changed();
	}
}

/* управление интерфейсом в LTC2208 */
void
board_set_adcrand(uint_fast8_t v)
{
	uint_fast8_t n = v != 0;
	if (glob_adcrand != n)
	{
		glob_adcrand = n;
		board_ctlreg1changed();
	}
}

/* не нулевой аргумент - включение VOX */
void
board_set_vox(uint_fast8_t v)	
{
	const uint_fast8_t n = v != 0;
	if (glob_vox != n)
	{
		glob_vox = n;
		board_ctlreg1changed();
		board_dsp1regchanged();
	}
}

/* не нулевой аргумент - включение НЧ режекторного фильтра */
void
board_set_notch(uint_fast8_t v)	
{
	const uint_fast8_t n = v != 0;
	if (glob_notch != n)
	{
		glob_notch = n;
		board_ctlreg1changed();
		board_dsp1regchanged();
	}
}
/* не нулевой аргумент - включение НЧ режекторного фильтра CW */
void
board_set_notchnarrow(uint_fast8_t v)	
{
	const uint_fast8_t n = v != 0;
	if (glob_notchnarrow != n)
	{
		glob_notchnarrow = n;
		board_ctlreg1changed();
		board_dsp1regchanged();
	}
}

/* не нулевой аргумент - включение НЧ режекторного фильтра */
void
board_set_affilter(uint_fast8_t v)	
{
	const uint_fast8_t n = v != 0;
	if (glob_affilter != n)
	{
		glob_affilter = n;
		board_ctlreg1changed();
		board_dsp1regchanged();
	}
}

/*  */
void
board_set_loudspeaker(uint_fast8_t v)	
{
	const uint_fast8_t n = v != 0;
	if (glob_loudspeaker_off != n)
	{
		glob_loudspeaker_off = n;
		board_ctlreg1changed();
		board_codec1regchanged();
	}
}

void
board_set_txcw(uint_fast8_t v)	/* Включение передачи в обход балансного модулятора */
{
	const uint_fast8_t n = v != 0;
	if (glob_txcw != n)
	{
		glob_txcw = n;
		board_ctlreg1changed();
	}
}

void
board_set_txgate(uint_fast8_t v)	/* разрешение драйвера и оконечного усилителя */
{
	const uint_fast8_t n = v != 0;
	if (glob_txgate != n)
	{
		glob_txgate = n;
		board_ctlreg1changed();
	}
}

void
board_set_tuner_C(uint_fast8_t n)	/* установка значение конденсатора в согласующем устройстве */
{
	if (glob_tuner_C != n)
	{
		glob_tuner_C = n;
		board_ctlreg1changed();
	}
}

void
board_set_tuner_L(uint_fast8_t n)	/* установка значение индуктивности в согласующем устройстве */
{
	if (glob_tuner_L != n)
	{
		glob_tuner_L = n;
		board_ctlreg1changed();
	}
}

void
board_set_tuner_type(uint_fast8_t v)	/* вариант повышающего/понижающего согласования */
{
	const uint_fast8_t n = v != 0;
	if (glob_tuner_type != n)
	{
		glob_tuner_type = n;
		board_ctlreg1changed();
	}
}

void
board_set_tuner_bypass(uint_fast8_t v)	/* обход согласующего устройства */
{
	const uint_fast8_t n = v != 0;
	if (glob_tuner_bypass != n)
	{
		glob_tuner_bypass = n;
		board_ctlreg1changed();
	}
}

/* не нулевой аргумент - Находимся в режиме настройки согласующего устройства */
void
board_set_autotune(uint_fast8_t v)	/* Находимся в режиме настройки согласующего устройства */
{
	const uint_fast8_t n = v != 0;
	if (glob_autotune != n)
	{
		glob_autotune = n;
		board_ctlreg1changed();
	}
}

/* не нулевой аргумент - включение user output 1 */
void
board_set_user1(uint_fast8_t v)	
{
	if (glob_user1 != v)
	{
		glob_user1 = v;
		board_ctlreg1changed();
	}
}

/* не нулевой аргумент - включение user output 2 */
void
board_set_user2(uint_fast8_t v)	
{
	const uint_fast8_t n = v != 0;
	if (glob_user2 != n)
	{
		glob_user2 = n;
		board_ctlreg1changed();
	}
}

/* не нулевой аргумент - включение user output 3 */
void
board_set_user3(uint_fast8_t v)	
{
	const uint_fast8_t n = v != 0;
	if (glob_user3 != n)
	{
		glob_user3 = n;
		board_ctlreg1changed();
	}
}

/* не нулевой аргумент - включение user output 4 */
void
board_set_user4(uint_fast8_t v)	
{
	const uint_fast8_t n = v != 0;
	if (glob_user4 != n)
	{
		glob_user4 = n;
		board_ctlreg1changed();
	}
}

/* не нулевой аргумент - включение user output 5 */
void
board_set_user5(uint_fast8_t v)	
{
	const uint_fast8_t n = v != 0;
	if (glob_user5 != n)
	{
		glob_user5 = n;
		board_ctlreg1changed();
	}
}

void
board_set_reset_n(uint_fast8_t v)	/* установка сигнала RESET всем устройствам */
{
	const uint_fast8_t n = v != 0;
	if (glob_reset_n != n)
	{
		glob_reset_n = n;
		board_ctlreg1changed();
		board_ctlreg2changed();
	}
}

void
board_set_flt_reset_n(uint_fast8_t v)	/* установка сигнала RESET всем устройствам */
{
	const uint_fast8_t n = v != 0;
	if (glob_flt_reset_n != n)
	{
		glob_flt_reset_n = n;
		board_ctlreg1changed();
		board_ctlreg2changed();
	}
}

void
board_set_i2s_enable(uint_fast8_t v)	/* разрешение генерации тактовой частоты для I2S в FPGA */
{
	const uint_fast8_t n = v != 0;
	if (glob_i2s_enable != n)
	{
		glob_i2s_enable = n;
		board_ctlreg1changed();
	}
}

/* Для выбора диапазона - частота с дискретностью 100 кГц */
void
board_set_bcdfreq100k(uint_fast16_t bcdfreq)
{
	if (glob_bcdfreq != bcdfreq)
	{
		glob_bcdfreq = bcdfreq;
		board_ctlreg1changed();
	}
}

void 
board_ctl_set_vco(
	uint_fast8_t n)	// 0..3 - code of VCO
{
	if (glob_vco != n)
	{
		glob_vco = n;
		board_ctlreg2changed();
	}
}


void 
board_setlo2xtal(
	uint_fast8_t n)	// code of xtal
{
	if (glob_lo2xtal != n)
	{
		glob_lo2xtal = n;
		board_ctlreg1changed();
	}
}

/* SW-2011-RDX */
void
board_set_scalelo1(uint_fast8_t v)
{
	if (glob_lo1scale != v)
	{
		glob_lo1scale = v;
		board_ctlreg1changed();
	}
}

void
board_set_stage1level(uint_fast8_t v)
{
	if (glob_stage1level != v)
	{
		glob_stage1level = v;
		board_ctlreg1changed();
	}
}

void
board_set_stage2level(uint_fast8_t v)
{
	if (glob_stage2level != v)
	{
		glob_stage2level = v;
		board_ctlreg1changed();
	}
}

void
board_set_sdcardpoweron(uint_fast8_t v)
{
	const uint_fast8_t n = v != 0;
	if (glob_sdcardpoweron != n)
	{
		glob_sdcardpoweron = n;
		board_ctlreg1changed();
	}
}

void
board_set_usbflashpoweron(uint_fast8_t v)
{
	const uint_fast8_t n = v != 0;
	if (glob_usbflashpoweron != n)
	{
		glob_usbflashpoweron = n;
		board_ctlreg1changed();
	}
}

void
board_set_attvalue(uint_fast8_t v)
{
	if (glob_attvalue != v)
	{
		glob_attvalue = v;
		board_ctlreg1changed();
	}
}



/////////////////////////////////////////////
// --- Набор функций требования установки сигналов на управляющих выходах.
/////////////////////////////////////////////

#if (DDS1_TYPE == DDS_TYPE_ATTINY2313) || (DDS2_TYPE == DDS_TYPE_ATTINY2313) || (DDS3_TYPE == DDS_TYPE_ATTINY2313)
static void prog_softdds_freq(
	spitarget_t target,		/* addressing to chip */
	const ftw_t * value
	)
{
#if 1
	const uint_fast32_t v32 = * value;

	spi_select(target, SPIC_MODE3);	/* start sending data to target chip */
	spi_progval8_p1(target, v32 >> 24);
	spi_progval8_p2(target, v32 >> 16);
	spi_progval8_p2(target, v32 >> 8);
	spi_progval8_p2(target, v32 >> 0);
	spi_complete(target);
	spi_unselect(target);	/* done sending data to target chip */

#else
	i2c_start(targetdds1 == targetdds1 ? 0xf8 : 0xf0);	// addr+wr

	i2c_write(v32 >> 24);
	i2c_write(v32 >> 16);
	i2c_write(v32 >> 8);
	i2c_write(v32 >> 0);
	i2c_waitsend();
	i2c_stop();
#endif
}
#endif


#if defined(DDS1_TYPE)

#if WITHLO1LEVELADJ

void 
prog_dds1_setlevel(uint_fast8_t percent)
{
#if (DDS1_TYPE == DDS_TYPE_AD9951)
	phase_t dmult;

	phase_fromuint(& dmult, percent * 0x3fffUL / 100);
	prog_ad9951_asf(targetdds1, & dmult);
	prog_pulse_ioupdate();
#endif
}
#endif /* WITHLO1LEVELADJ */

// interface functiom
void 
prog_dds1_ftw_sub(const ftw_t * value)
{
#if (DDS1_TYPE == DDS_TYPE_FPGAV1)
	prog_fpga_freq2(targetfpga1, value);
	prog_pulse_ioupdate();
#endif
}

// interface functiom
void 
prog_dds1_ftw_sub3(const ftw_t * value)
{
#if (DDS1_TYPE == DDS_TYPE_FPGAV1)
	prog_fpga_freq3(targetfpga1, value);
	prog_pulse_ioupdate();
#endif
}

// interface functiom
void 
prog_dds1_ftw_sub4(const ftw_t * value)
{
#if (DDS1_TYPE == DDS_TYPE_FPGAV1)
	prog_fpga_freq4(targetfpga1, value);
	prog_pulse_ioupdate();
#endif
}

void 
prog_dds1_ftw(const ftw_t * value)
{
#if LO1PHASES
	#if (DDS1_TYPE == DDS_TYPE_AD9951)
		prog_ad9951_freq0(targetdds1, value);
		prog_ad9951_freq0(targetdds1a, value);
		prog_pulse_ioupdate();
	#endif

#else	/* LO1PHASES */

	#if (DDS1_TYPE == DDS_TYPE_AD9852)
		prog_ad9852_freq1(targetdds1, value);
		prog_pulse_ioupdate();
	#elif (DDS1_TYPE == DDS_TYPE_FPGAV1)
		prog_fpga_freq1(targetfpga1, value);
	#elif (DDS1_TYPE == DDS_TYPE_FPGAV2)
		prog_fpga_freq1(targetfpga1, value);
	#elif (DDS1_TYPE == DDS_TYPE_AD9857)
		prog_ad9857_freq(targetdds1, 0x00, value);
		prog_pulse_ioupdate();
	#elif (DDS1_TYPE == DDS_TYPE_AD9951)
		prog_ad9951_freq0(targetdds1, value);
		prog_pulse_ioupdate();
	#elif (DDS1_TYPE == DDS_TYPE_AD9834)
		#if DDS1_AD9834_COMPARATOR_ON
			prog_ad9834_freq(targetdds1, value, & dds1_profile, 1);
		#else
			prog_ad9834_freq(targetdds1, value, & dds1_profile, 0);
		#endif
	#elif (DDS1_TYPE == DDS_TYPE_AD9851)
		prog_ad9851_freq(targetdds1, value, DDS1_CLK_MUL);
	#elif (DDS1_TYPE == DDS_TYPE_AD9835)
		prog_ad9835_freq(targetdds1, value, & dds1_profile);
	#elif (DDS1_TYPE == DDS_TYPE_ATTINY2313)
		prog_softdds_freq(targetdds1, value);
	#else
		//(void) target;

#endif

#endif	/* LO1PHASES */
}

// Установка центральной частоты панорамного индикатора
void 
prog_rts1_ftw(const ftw_t * value)
{
#if (DDS1_TYPE == DDS_TYPE_FPGAV1)
	prog_fpga_freq1_rts(targetfpga1, value);
#elif (DDS1_TYPE == DDS_TYPE_FPGAV2)
	prog_fpga_freq1_rts(targetfpga1, value);
#endif
}


// Установка частоты конвертора
void prog_xvtr_freq(
	uint_fast32_t f,		/* частота */
	uint_fast8_t enable		/* разрешить работу */
	)
{
#if XVTR_R820T2
	r820t_enable(enable);
	if (enable)
		r820t_setfreq(f);
#endif /* XVTR_R820T2 */
}


// interface functiom
void 
prog_dds1_ftw_noioupdate(const ftw_t * value)
{
#if LO1PHASES
#if (DDS1_TYPE == DDS_TYPE_AD9951)
	prog_ad9951_freq0(targetdds1, value);
	prog_ad9951_freq0(targetdds1a, value);
#endif

#else	/* LO1PHASES */

#if (DDS1_TYPE == DDS_TYPE_AD9852)
	prog_ad9852_freq1(targetdds1, value);
#elif (DDS1_TYPE == DDS_TYPE_FPGAV1)
	prog_fpga_freq1(targetfpga1, value);
#elif (DDS1_TYPE == DDS_TYPE_FPGAV2)
	prog_fpga_freq1(targetfpga1, value);
#elif (DDS1_TYPE == DDS_TYPE_AD9857)
	prog_ad9857_freq(targetdds1, 0x00, value);
#elif (DDS1_TYPE == DDS_TYPE_AD9951)
	prog_ad9951_freq0(targetdds1, value);
#elif (DDS1_TYPE == DDS_TYPE_AD9834)
	#if DDS1_AD9834_COMPARATOR_ON
		prog_ad9834_freq(targetdds1, value, & dds1_profile, 1);
	#else
		prog_ad9834_freq(targetdds1, value, & dds1_profile, 0);
	#endif
#elif (DDS1_TYPE == DDS_TYPE_AD9851)
	prog_ad9851_freq(targetdds1, value, DDS1_CLK_MUL);
#elif (DDS1_TYPE == DDS_TYPE_AD9835)
	prog_ad9835_freq(targetdds1, value, & dds1_profile);
#elif (DDS1_TYPE == DDS_TYPE_ATTINY2313)
	prog_softdds_freq(targetdds1, value);
#else
	//(void) target;

#endif

#endif	/* LO1PHASES */
}

static 
void 
prog_dds1_initialize(void)
{
#if defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_AD9852)
	prog_ad9852_init(targetdds1, 0, DDS1_CLK_MUL);
#elif defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_FPGAV1)
	prog_fpga_initialize(targetfpga1);
#elif defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_FPGAV2)
	prog_fpga_initialize(targetfpga1);
#elif defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_AD9857)
	prog_ad9857_init(targetdds1, 0, DDS1_CLK_MUL);
#elif defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_AD9951)
	prog_ad9951_init(targetdds1, DDS1_CLK_MUL, (REFERENCE_FREQ * DDS1_CLK_MUL / DDS1_CLK_DIV) >= 200000000);
#elif defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_AD9851)
	prog_ad9851_init(targetdds1, DDS1_CLK_MUL);
#elif defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_AD9834)
	#if DDS1_AD9834_COMPARATOR_ON
		prog_ad9834_init(targetdds1, & dds1_profile, 1);
	#else
		prog_ad9834_init(targetdds1, & dds1_profile, 0);
	#endif
#elif defined(DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_AD9835)
	prog_ad9835_init(targetdds1, & dds1_profile);
#else

#endif
}


#endif // defined(DDS1_TYPE)

#if defined(DDS2_TYPE)

// interface functiom
void prog_dds2_ftw(const ftw_t * value)
{
#if (DDS2_TYPE == DDS_TYPE_AD9852)
	prog_ad9852_freq1(targetdds2, value);
	prog_pulse_ioupdate();
#elif (DDS2_TYPE == DDS_TYPE_FPGAV1)
	prog_fpga_freq1(targetfpga1, value);
	prog_pulse_ioupdate();
#elif (DDS2_TYPE == DDS_TYPE_AD9857)
	prog_ad9857_freq(targetdds2, 0x00, value);
	prog_pulse_ioupdate();
#elif (DDS2_TYPE == DDS_TYPE_AD9951)
	prog_ad9951_freq0(targetdds2, value);
	prog_pulse_ioupdate();
#elif (DDS2_TYPE == DDS_TYPE_AD9834)
	#if DDS2_AD9834_COMPARATOR_ON
		prog_ad9834_freq(targetdds2, value, & dds2_profile, 1);
	#else
		prog_ad9834_freq(targetdds2, value, & dds2_profile, 0);
	#endif
#elif (DDS2_TYPE == DDS_TYPE_AD9851)
	prog_ad9851_freq(targetdds2, value, DDS2_CLK_MUL);
#elif (DDS2_TYPE == DDS_TYPE_AD9835)
	prog_ad9835_freq(targetdds2, value, & dds2_profile);
#elif (DDS2_TYPE == DDS_TYPE_ATTINY2313)
	prog_softdds_freq(targetdds2, value);

#endif
}

static void 
prog_dds2_initialize(void)
{
#if defined(DDS2_TYPE) && (DDS2_TYPE == DDS_TYPE_AD9852)
	prog_ad9852_init(targetdds2, 0, DDS2_CLK_MUL);
#elif defined(DDS2_TYPE) && (DDS2_TYPE == DDS_TYPE_FPGAV1)
	prog_fpga_initialize(targetfpga1);
#elif defined(DDS2_TYPE) && (DDS2_TYPE == DDS_TYPE_AD9857)
	prog_ad9857_init(targetdds2, 0, DDS2_CLK_MUL);
#elif defined(DDS2_TYPE) && (DDS2_TYPE == DDS_TYPE_AD9951)
	prog_ad9951_init(targetdds2, DDS2_CLK_MUL, (REFERENCE_FREQ * DDS2_CLK_MUL / DDS2_CLK_DIV) >= 200000000);
#elif defined(DDS2_TYPE) && (DDS2_TYPE == DDS_TYPE_AD9851)
	prog_ad9851_init(targetdds2, DDS2_CLK_MUL);
#elif defined(DDS2_TYPE) && (DDS2_TYPE == DDS_TYPE_AD9834)
	#if DDS2_AD9834_COMPARATOR_ON
		prog_ad9834_init(targetdds2, & dds2_profile, 1);
	#else
		prog_ad9834_init(targetdds2, & dds2_profile, 0);
	#endif
#elif defined(DDS2_TYPE) && (DDS2_TYPE == DDS_TYPE_AD9835)
	prog_ad9835_init(targetdds2, & dds2_profile);
#endif
}

#endif // defined(DDS2_TYPE)

#if defined(DDS3_TYPE)

// interface functiom
void prog_dds3_ftw(const ftw_t * value)
{
#if (DDS3_TYPE == DDS_TYPE_AD9852)
	prog_ad9852_freq1(targetdds3, value);
	prog_pulse_ioupdate();
#elif (DDS3_TYPE == DDS_TYPE_FPGAV1)
	prog_fpga_freq1(targetfpga1, value);
	prog_pulse_ioupdate();
#elif (DDS3_TYPE == DDS_TYPE_AD9857)
	prog_ad9857_freq(targetdds3, 0x00, value);
	prog_pulse_ioupdate();
#elif (DDS3_TYPE == DDS_TYPE_AD9951)
	prog_ad9951_freq0(targetdds3, value);
	prog_pulse_ioupdate();
#elif (DDS3_TYPE == DDS_TYPE_AD9834)
	#if DDS3_AD9834_COMPARATOR_ON
		prog_ad9834_freq(targetdds3, value, & dds3_profile, 1);
	#else
		prog_ad9834_freq(targetdds3, value, & dds3_profile, 0);
	#endif
#elif (DDS3_TYPE == DDS_TYPE_AD9851)
	prog_ad9851_freq(targetdds3, value, DDS3_CLK_MUL);
#elif (DDS3_TYPE == DDS_TYPE_AD9835)
	prog_ad9835_freq(targetdds3, value, & dds3_profile);
#elif (DDS3_TYPE == DDS_TYPE_ATTINY2313)
	prog_softdds_freq(targetdsd3, value);

#endif
}

static void 
prog_dds3_initialize(void)
{
#if defined(DDS3_TYPE) && (DDS3_TYPE == DDS_TYPE_AD9852)
	prog_ad9852_init(targetdds3, 0, DDS3_CLK_MUL);
#elif defined(DDS3_TYPE) && (DDS3_TYPE == DDS_TYPE_FPGAV1)
	prog_fpga_initialize(targetfpga1);
#elif defined(DDS3_TYPE) && (DDS3_TYPE == DDS_TYPE_AD9857)
	prog_ad9857_init(targetdds3, 0, DDS3_CLK_MUL);
#elif defined(DDS3_TYPE) && (DDS3_TYPE == DDS_TYPE_AD9951)
	prog_ad9951_init(targetdds3, DDS3_CLK_MUL, (REFERENCE_FREQ * DDS3_CLK_MUL / DDS3_CLK_DIV) >= 200000000);
#elif defined(DDS3_TYPE) && (DDS3_TYPE == DDS_TYPE_AD9851)
	prog_ad9851_init(targetdds3, DDS3_CLK_MUL);
#elif defined(DDS3_TYPE) && (DDS3_TYPE == DDS_TYPE_AD9834)
	#if DDS3_AD9834_COMPARATOR_ON
		prog_ad9834_init(targetdds3, & dds3_profile, 1);
	#else
		prog_ad9834_init(targetdds3, & dds3_profile, 0);
	#endif
#elif defined(DDS3_TYPE) && (DDS3_TYPE == DDS_TYPE_AD9835)
	prog_ad9835_init(targetdds3, & dds3_profile);
#endif
}

#endif // defined(DDS3_TYPE)

#if defined(PLL1_TYPE)

#if HYBRID_PLL1_POSITIVE		/* в петле ФАПЧ нет инвертирующего усилителя */
	enum { pll1_polarity = 1 };	/* без инвертирующего усилителя в активном фильтре */
#elif HYBRID_PLL1_NEGATIVE		/* в петле ФАПЧ есть инвертирующий усилитель */
	enum { pll1_polarity = 0 };	/* С инвертирующим усилителем в активном фильтре */
#else
	//#error Undefined HYBRID_PLL1_POSITIVE or HYBRID_PLL1_NEGATIVE board option
#endif

static void 
board_pll1_initialize(void)
{
#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_LMX2306)
	prog_lmx2306_init(targetpll1);
	prog_lmx2306_func(targetpll1, pll1_polarity, LMX2306_FOLD_DGND, 0);	/* set phase det polarity */
	prog_lmx2306_r(targetpll1, & r1_ph);	/* Set divider F osc R - 3...16383 */
	prog_lmx2306_n(targetpll1, & phase_last_n1, LMX2306_GO_S7TATE);	/* Set divider F osc R - 3...16383 */
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_LMX1601)
	prog_lmx1601_init(targetpll1);
	prog_lmx1601_func(targetpll1, pll1_polarity);	/* set phase det polarity */
	prog_lmx1601_r(targetpll1, & r1_ph, LMX1601_FOLD_GROUND);	/* Set divider F osc R - 3...16383 */
	prog_lmx1601_n(targetpll1, & phase_last_n1);	/* Set divider F osc R - 3...16383 */
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_ADF4001)
	//prog_adf4001_init(targetpll1);
	prog_adf4001_func(targetpll1, pll1_polarity, ADF4001_FOLD_DGND, PLL1_CPI, 1);	/* set phase det polarity */
	prog_adf4001_func(targetpll1, pll1_polarity, ADF4001_FOLD_DGND, PLL1_CPI, 0);	/* set phase det polarity */
	prog_adf4001_r(targetpll1, & r1_ph);	/* Set divider F osc R - 3...16383 */
	prog_adf4001_n(targetpll1, & phase_last_n1, ADF4001_GO_STATE);	/* Set divider F osc R - 1...16383 */
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_ADF4360)
	prog_adf4360_r(targetpll1, & r1_ph);	/* Set divider F osc R - 3...16383 */
	prog_adf4360_func(targetpll1, ADF4360_FOLD_DGND);	/* 0x01 - mux out lock indicator */
	prog_adf4360_n(targetpll1, & phase_last_n1);	/* Set divider F osc R - 1...16383 */
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_MC145170)
	prog_mc145170_init(targetpll1);
	prog_mc145170_func(targetpll1, !pll1_polarity);	/* set phase det polarity */
	prog_mc145170_r(targetpll1, & r1_ph);	/* Set divider F osc R - 3...16383 */
	prog_mc145170_n(targetpll1, & phase_last_n1);	/* Set divider F osc R - 3...16383 */
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_TSA6057)
	prog_tsa6057_n(targetpll1, & phase_last_n1, TSA6057_PLL1_RCODE, 1);	/* r=160/400/4000 */
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_UMA1501)
	prog_uma1501_init(targetpll1);
	prog_uma1501_r(targetpll1, & r1_ph);	/* Set divider F osc R - 3...16383 */
	prog_uma1501_n(targetpll1, & phase_last_n1);	/* Set divider F osc R - 3...16383 */
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_LM7001)
	prog_lm7001_initialize(targetpll1);	/* */
	prog_lm7001_divider(targetpll1, & phase_last_n1, 0x00, LM7001_PLL1_RCODE);	/* Set divider N */
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_HMC830)
	prog_hmc830_initialize(targetpll1);
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_RFFC5071)
	prog_rffc5071_initialize(targetpll1);
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570)
	si570_initialize();	
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_NONE)
	// ФАПЧ внешняя, никак не управляется.
#else
	#error Unknown value of PLL1_TYPE
#endif
}

// interface functiom
pllhint_t board_pll1_get_hint(
	uint_fast32_t f		/* требуемая частота на выходе PLL1 */
	)
{
#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_HMC830)
	return prog_hmc830_get_hint(f);
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570)
	return si570_get_hint(f);
#elif MULTIVFO
	return pll1_getoutdivider(f) * HYBRID_NVFOS + pll1_getvco(f);	// В этом случае hint храние код ГУН
#else
	return pll1_getoutdivider(f);	// В этом случае hint храние код ГУН
#endif
}

// interface functiom
uint_fast16_t board_pll1_get_divider(pllhint_t hint)
{
#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_HMC830)
	return prog_hmc830_get_divider(hint);
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570)
	return si570_get_divider(hint);
#elif MULTIVFO
	return hint / HYBRID_NVFOS;	/* Делитель, встроенный в ГУН (1 - без делителя) */
#else
	return hint;	/* Делитель, встроенный в ГУН (1 - без делителя) */
#endif
}

// interface functiom
/* установка конфигурации ГУН на основании hint */
void board_pll1_set_vco(pllhint_t hint)
{
	//PRINTF(PSTR("pll1_getvco(%ld) = %d\n"), (long) f, pll1_getvco(f));
#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_HMC830)
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570)
#elif MULTIVFO
	// В этом случае hint храние код ГУН
	board_ctl_set_vco((uint_fast8_t) hint % HYBRID_NVFOS);	/* выбор ГУН, соответствующего требуемой частоте генерации */
	board_update();						/* вывести забуферированные изменения в регистры */
#else
	// В этом случае hint храние код ГУН
#endif
}

// interface functiom
/* установка выходного делителя ГУН на основании hint */
void board_pll1_set_vcodivider(pllhint_t hint)
{
#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_HMC830)
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570)
#elif MULTIVFO && LO1DIVIDEVCO
	pll1_setoutdivider(hint / HYBRID_NVFOS);
#elif LO1DIVIDEVCO
	pll1_setoutdivider(hint);
#endif
}

// interface functiom
/* установка требуемого делителя и конфигурации ГУН на основании hint (если требуется) */
uint_fast8_t board_pll1_set_n(
	const phase_t * value, 
	pllhint_t hint, 
	uint_fast8_t stop
	)
{
	if (stop != 0)
	{
		if (phase_last_n1 == * value && last_hint1 == hint)
			return 0;	// не требуется второй вызов фунции для запуска PLL1
		phase_last_n1 = * value;
		last_hint1 = hint;
#if 0
		{
			static unsigned  count;
			count ++;
			char buff [22];

			local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
				PSTR("%02x"), (unsigned) count
				 );
			display_at(0, 1, buff);
		}
#endif

	}

#if (PLL1_TYPE == PLL_TYPE_LMX2306)
	prog_lmx2306_func(targetpll1, pll1_polarity, LMX2306_FOLD_DGND, stop);	/* set phase det polarity */
	if (stop)
		prog_lmx2306_n(targetpll1, value, LMX2306_GO_STATE);		/* set divider f in  n - 56..65535 */
	return 1;	// требуется второй вызов фунции для запуска PLL1
#elif (PLL1_TYPE == PLL_TYPE_LMX1601)
	if (stop)
		prog_lmx1601_n(targetpll1, value);		/* set divider f in  n - 56..65535 */
	return 0;	// не требуется второй вызов фунции для запуска PLL1
#elif (PLL1_TYPE == PLL_TYPE_ADF4001)
	prog_adf4001_func(targetpll1, pll1_polarity, ADF4001_FOLD_DGND, PLL1_CPI, stop);	/* set phase det polarity */
	if (stop)
		prog_adf4001_n(targetpll1, value, ADF4001_GO_STATE);		/* set divider f in  n - 1..65535 */
	return 1;	// требуется второй вызов фунции для запуска PLL1
#elif (PLL1_TYPE == PLL_TYPE_ADF4360)
	if (stop)
		prog_adf4360_n(targetpll1, value);		/* set divider f in  n - 1..65535 */
	return 0;	// не требуется второй вызов фунции для запуска PLL1
#elif (PLL1_TYPE == PLL_TYPE_MC145170)
	if (stop)
		prog_mc145170_n(targetpll1, value);		/* set divider f in  n - 56..65535 */
	return 0;	// не требуется второй вызов фунции для запуска PLL1
#elif (PLL1_TYPE == PLL_TYPE_TSA6057)
	if (stop)
		prog_tsa6057_n(targetpll1, value, TSA6057_PLL1_RCODE, 1);	/* r=160/400/4000 */
	return 0;	// не требуется второй вызов фунции для запуска PLL1
#elif (PLL1_TYPE == PLL_TYPE_UMA1501)
	if (stop)
		prog_uma1501_n(targetpll1, value);		/* set divider f in  n - 56..65535 */
	return 0;	// не требуется второй вызов фунции для запуска PLL1
#elif (PLL1_TYPE == PLL_TYPE_LM7001)
	if (stop)
		prog_lm7001_divider(targetpll1, value, 0x00, LM7001_PLL1_RCODE);	/* Set divider F osc N - 3...16383 */
	return 0;	// не требуется второй вызов фунции для запуска PLL1
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_HMC830)
	if (stop)
		prog_hmc830_n(targetpll1, last_hint1, value);
	return 0;	// не требуется второй вызов фунции для запуска PLL1
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570)
	if (stop)
		si570_n(last_hint1, value);
	return 0;	// не требуется второй вызов фунции для запуска PLL1
#elif defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_NONE)
	// ФАПЧ внешняя, никак не управляется.
	return 0;	// не требуется второй вызов фунции для запуска PLL1
#else
	#error Unknown value of PLL1_TYPE
#endif
}

#endif // defined(PLL1_TYPE)

#if defined(PLL2_TYPE)

	static phase_t phase_lo2_n;
	#if LO2_PLL_R == 0
		static phase_t phase_lo2_r = 256;	// просто что-то.
	#else
		static phase_t phase_lo2_r = LO2_PLL_R;
	#endif

// Use variables phase_lo2_r and phase_lo2_n

static void prog_pll2_initialize(
	uint_fast8_t asdividern	/* как делитель с программируемым делением rf (N) - используется выход */
	)
{
	enum { pll2_polarity = 1 };	/* без инвертирующего усилителя в активном фильтре */

	/* программирование делителя производится в updateboard */

	//phase_lo2_n = LO2_PLL_N;

#if defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_LMX2306)
	prog_lmx2306_init(targetpll2);		
	prog_lmx2306_func(targetpll2, pll2_polarity, asdividern ? LMX2306_FOLD_N_OUT : LMX2306_FOLD_DGND, 0);	/* set phase det pll2_polarity */
	prog_lmx2306_r(targetpll2, & phase_lo2_r);	/* Set divider F osc R - 3...16383 */
	//prog_lmx2306_n(targetpll2, & phase_lo2_n, LMX2306_GO_STATE);	/* Set divider F in  N - 56..65535 */
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_LMX1601)
	prog_lmx1601_init(targetpll2);		
	prog_lmx1601_func(targetpll2, pll2_polarity);	/* set phase det polarity */
	prog_lmx1601_r(targetpll2, & phase_lo2_r, asdividern ? LMX1601_FOLD_MAIN_N_OUT : LMX1601_FOLD_GROUND);	/* Set divider F osc R - 3...16383 */
	//prog_lmx2306_n(targetpll2, & phase_lo2_n, LMX2306_GO_STATE);	/* Set divider F in  N - 56..65535 */
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_ADF4001)
	//prog_adf4001_init(targetpll2);		
	prog_adf4001_func(targetpll2, pll2_polarity, asdividern ? ADF4001_FOLD_N_OUT : ADF4001_FOLD_DGND, PLL2_CPI, 1);	/* set phase det pll2_polarity, 0x02 - N divider output */
	prog_adf4001_func(targetpll2, pll2_polarity, asdividern ? ADF4001_FOLD_N_OUT : ADF4001_FOLD_DGND, PLL2_CPI, 0);	/* set phase det pll2_polarity, 0x02 - N divider output */
	prog_adf4001_r(targetpll2, & phase_lo2_r);	/* Set divider F osc R - 3...16383 */
	//prog_adf4001_n(targetpll2, & phase_lo2_n, ADF4001_GO_STATE);	/* Set divider F in  N - 1..65535 */
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_ADF4360)
	prog_adf4360_r(targetpll2, & phase_lo2_r);	/* Set divider F osc R - 3...16383 */
	prog_adf4360_func(targetpll2, asdividern ? ADF4360_FOLD_N_OUT : ADF4360_FOLD_DGND);		/* set phase det polarity, 0x02 - N divider output */
	//prog_adf4360_n(targetpll2, & phase_lo2_n);	/* Set divider F in  N - 1..65535 */
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_CMX992)
	prog_cmx992_reset(targetpll2);		
	prog_cmx992_general_control(targetpll2);	/* Enable LO */

	prog_cmx992_ifpll_m(targetpll2, & phase_lo2_r);	/* Set divider F osc R -  */
	//prog_cmx992_ifpll_n(targetpll2, & phase_lo2_n);	/* Set divider F in  N -  */

	prog_cmx992_rx_control(targetpll2, 1, LO1_POWER2);
	prog_cmx992_rx_mode(targetpll2);

	//prog_cmx992_tx_control(targetpll2);
	//prog_cmx992_tx_mode(targetpll2);
	//prog_cmx992_tx_gain(targetpll2);

#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_MC145170)
	prog_mc145170_init(targetpll2);		
	prog_mc145170_func(targetpll2, !pll2_polarity);	/* set phase det polarity */
	prog_mc145170_r(targetpll2, & phase_lo2_r);	/* Set divider F osc R - 3...16383 */
	//prog_mc145170_n(targetpll2, & phase_lo2_n);	/* Set divider F in  N - 56..65535 */
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_LM7001)
	prog_lm7001_divider(targetpll2, & phase_lo2_n, 0x00, LM7001_PLL2_RCODE);	/* Set divider N */
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_HMC830)
	prog_hmc830_initialize(targetpll2);
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_RFFC5071)
	prog_rffc5071_initialize(targetpll2);
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_NONE)
#else
	#error Unknown value of PLL2_TYPE
#endif
}

// interface functiom
void prog_pll2_n(const phase_t * value)
{
	if (phase_lo2_n == * value)
		return;
	phase_lo2_n = * value;

#if defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_LMX2306)
	prog_lmx2306_n(targetpll2, & phase_lo2_n, LMX2306_GO_STATE);		/* set divider f in  n - 56..65535 */
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_LMX1601)
	prog_lmx1601_n(targetpll2, & phase_lo2_n);		/* set divider f in  n - 56..65535 */
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_ADF4001)
	prog_adf4001_n(targetpll2, & phase_lo2_n, ADF4001_GO_STATE);		/* set divider f in  n - 1..65535 */
#elif defined(PLL2_TYPE) && (PLL12TYPE == PLL_TYPE_ADF4360)
	prog_adf4360_n(targetpll2, & phase_lo2_n);		/* set divider f in  n - 1..65535 */
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_MC145170)
	prog_mc145170_n(targetpll2, & phase_lo2_n);		/* set divider f in  n - 1..65535 */
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_CMX992)
	prog_cmx992_ifpll_n(targetpll2, & phase_lo2_n);	/* Set divider F in  N -  */
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_NONE)
#else
	#error Unknown value of PLL2_TYPE
#endif
}

// interface functiom
void prog_pll2_r(const phase_t * value)
{
	enum { asdividern = 0 };
	enum { pll2_polarity = 1 };	/* без инвертирующего усилителя в активном фильтре */

	if (phase_lo2_r == * value)
		return;
	phase_lo2_r = * value;

#if defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_LMX2306)
	//prog_lmx2306_init(targetpll2);		
	prog_lmx2306_func(targetpll2, pll2_polarity, asdividern ? LMX2306_FOLD_N_OUT : LMX2306_FOLD_DGND, 0);	/* set phase det pll2_polarity */
	prog_lmx2306_r(targetpll2, & phase_lo2_r);	/* Set divider F osc R - 3...16383 */
	//prog_lmx2306_n(targetpll2, & phase_lo2_n, LMX2306_GO_STATE);	/* Set divider F in  N - 56..65535 */
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_LMX1601)
	//prog_lmx1601_init(targetpll2);		
	prog_lmx1601_func(targetpll2, pll2_polarity);	/* set phase det polarity */
	prog_lmx1601_r(targetpll2, & phase_lo2_r, asdividern ? LMX1601_FOLD_MAIN_N_OUT : LMX1601_FOLD_GROUND);	/* Set divider F osc R - 3...16383 */
	//prog_lmx2306_n(targetpll2, & phase_lo2_n, LMX2306_GO_STATE);	/* Set divider F in  N - 56..65535 */
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_ADF4001)
	//prog_adf4001_init(targetpll2);		
	prog_adf4001_func(targetpll2, pll2_polarity, asdividern ? ADF4001_FOLD_N_OUT : ADF4001_FOLD_DGND, PLL2_CPI, 1);	/* set phase det pll2_polarity, 0x02 - N divider output */
	prog_adf4001_func(targetpll2, pll2_polarity, asdividern ? ADF4001_FOLD_N_OUT : ADF4001_FOLD_DGND, PLL2_CPI, 0);	/* set phase det pll2_polarity, 0x02 - N divider output */
	prog_adf4001_r(targetpll2, & phase_lo2_r);	/* Set divider F osc R - 3...16383 */
	//prog_adf4001_n(targetpll2, & phase_lo2_n, ADF4001_GO_STATE);	/* Set divider F in  N - 1..65535 */
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_ADF4360)
	prog_adf4360_r(targetpll2, & phase_lo2_r);	/* Set divider F osc R - 3...16383 */
	prog_adf4360_func(targetpll2, asdividern ? ADF4360_FOLD_N_OUT : ADF4360_FOLD_DGND);		/* set phase det polarity, 0x02 - N divider output */
	//prog_adf4360_n(targetpll2, & phase_lo2_n);	/* Set divider F in  N - 1..65535 */
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_CMX992)
	//prog_cmx992_reset(targetpll2);		
	//prog_cmx992_general_control(targetpll2);	/* Enable LO */

	prog_cmx992_ifpll_m(targetpll2, & phase_lo2_r);	/* Set divider F osc R -  */
	//prog_cmx992_ifpll_n(targetpll2, & phase_lo2_n);	/* Set divider F in  N -  */

	prog_cmx992_rx_control(targetpll2, 1, LO1_POWER2);
	prog_cmx992_rx_mode(targetpll2);

	//prog_cmx992_tx_control(targetpll2);
	//prog_cmx992_tx_mode(targetpll2);
	//prog_cmx992_tx_gain(targetpll2);

#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_MC145170)
	//prog_mc145170_init(targetpll2);		
	prog_mc145170_func(targetpll2, !pll2_polarity);	/* set phase det polarity */
	prog_mc145170_r(targetpll2, & phase_lo2_r);	/* Set divider F osc R - 3...16383 */
	//prog_mc145170_n(targetpll2, & phase_lo2_n);	/* Set divider F in  N - 56..65535 */
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_LM7001)
	prog_lm7001_divider(targetpll2, & phase_lo2_n, 0x00, LM7001_PLL2_RCODE);	/* Set divider N */
#elif defined(PLL2_TYPE) && (PLL2_TYPE == PLL_TYPE_NONE)
#else
	#error Unknown value of PLL2_TYPE
#endif
}

#endif

// End of frequency control functions set.

#if WITHFQMETER
uint_fast32_t board_get_fqmeter(void)
{
	uint_fast32_t v = 0;
	#if (DDS1_TYPE == DDS_TYPE_FPGAV1)
		v = prog_fpga_getfqmeter(targetfpga1);
	#endif
	return v;
}	
#endif /* WITHFQMETER */

static
void
board_set_dac1value(uint_fast8_t plane, uint_fast8_t v)	/* подстройка опорного генератора */
{
	if (glob_dac1value [plane] != v)
	{
		glob_dac1value [plane] = v;
		board_ctlreg2changed();
	}
}

void
prog_dac1_a_value(uint_fast8_t v)
{
	board_set_dac1value(0, v);
}

void
prog_dac1_b_value(uint_fast8_t v)
{
	board_set_dac1value(1, v);
}

#if defined(CODEC2_TYPE) && (CODEC2_TYPE == CODEC_TYPE_FPGAV1)

/* dummy function */
static void fpga_initialize_fullduplex(void)
{
}



const codec2if_t * board_getfpgacodecif(void)
{
	static const char codecname [] = "FPGA_V1";

	/* Интерфейс цправления кодеком */
	static const codec2if_t ifc =
	{
		fpga_initialize_fullduplex,
		codecname
	};

	return & ifc;
}

#endif /* defined(CODEC2_TYPE) && (CODEC2_TYPE == CODEC_TYPE_FPGAV1) */

#if FPGA_CONF_DONE_BIT != 0
/* получение сигнала завершения конфигурации FPGA. Возврат: 0 - конфигурация не завершена */
static uint_fast8_t board_fpga_get_CONF_DONE(void)
{
	return (FPGA_CONF_DONE_INPUT & FPGA_CONF_DONE_BIT) != 0;

}
#endif

#if FPGA_NSTATUS_BIT != 0
static uint_fast8_t board_fpga_get_NSTATUS(void)
{
	return (FPGA_NSTATUS_INPUT & FPGA_NSTATUS_BIT) != 0;

}
#endif

#if WITHFPGAWAIT_AS || WITHFPGALOAD_PS


#if 0 && FPGA_INIT_DONE_BIT != 0

/* не на всех платах соединено с процессором */
static uint_fast8_t board_fpga_get_INIT_DONE(void)
{
	return (FPGA_INIT_DONE_INPUT & FPGA_INIT_DONE_BIT) != 0;

}

#endif

static void board_fpga_loader_initialize(void)
{
	hardware_spi_master_setfreq(SPIC_SPEEDUFAST, SPISPEEDUFAST);
	HARDWARE_FPGA_LOADER_INITIALIZE();
}


#if WITHFPGALOAD_PS

#if ! (CPUSTYLE_R7S721 || CPUSTYLE_STM32MP1)
/* на процессоре renesas образ располагается в памяти, испольщуемой для хранений буферов DSP части */
static const FLASHMEMINIT uint16_t rbfimage0 [] =
{
#include "rbfimages.h"
};

/* получить расположение в памяти и количество элементов в массиве для загрузки FPGA */
const uint16_t * getrbfimage(size_t * count)
{
	* count = sizeof rbfimage0 / sizeof rbfimage0 [0];
	return & rbfimage0 [0];
}

#endif /* ! (CPUSTYLE_R7S721 || CPUSTYLE_STM32MP1) */

/* FPGA загружается процессором с помощью SPI */
static void board_fpga_loader_PS(void)
{
#if (WITHSPIHW && WITHSPI16BIT)	// for skip in test configurations
	unsigned retries = 0;
restart:

	if (++ retries > 4)
	{
		PRINTF(PSTR("fpga: board_fpga_loader_PS: FPGA is not respond.\n"));
		return;
	}
	;

	unsigned long w = 1000;
	do {
		size_t rbflength;
		const uint16_t * p = getrbfimage(& rbflength);

		PRINTF("fpga: board_fpga_loader_PS start\n");
		/* After power up, the Cyclone IV device holds nSTATUS low during POR delay. */

		FPGA_NCONFIG_PORT_S(FPGA_NCONFIG_BIT);
		local_delay_ms(1);
		/* 1) Выставить "1" на nCONFIG */
		//PRINTF(PSTR("fpga: FPGA_NCONFIG_BIT=1\n"));
		FPGA_NCONFIG_PORT_C(FPGA_NCONFIG_BIT);
		local_delay_ms(1);
		/* x) Дождаться "0" на nSTATUS */
		//PRINTF("fpga: waiting for FPGA_NSTATUS_BIT==0\n");
		while (board_fpga_get_NSTATUS() != 0)
		{
			local_delay_ms(1);
			if (-- w == 0)
				goto restart;
		}
		FPGA_NCONFIG_PORT_S(FPGA_NCONFIG_BIT);
		local_delay_ms(1);
		/* 2) Дождаться "1" на nSTATUS */
		//PRINTF("fpga: waiting for FPGA_NSTATUS_BIT==1\n");
		while (board_fpga_get_NSTATUS() == 0)
		{
			local_delay_ms(1);
			if (-- w == 0)
				goto restart;
		}
		/* 3) Выдать байты (бладший бит .rbf файла первым) */
		//PRINTF("fpga: start sending RBF image (%lu of 16-bit words)\n", rbflength);
		if (rbflength != 0)
		{
			unsigned wcd = 0;
			size_t n = rbflength - 1;
			//

			hardware_spi_connect_b16(SPIC_SPEEDUFAST, SPIC_MODE3);

			hardware_spi_b16_p1(* p ++);
			while (n --)
			{
				if (board_fpga_get_CONF_DONE() != 0)
				{
					//PRINTF("fpga: Unexpected state of CONF_DONE==1\n");
					break;
				}
				hardware_spi_b16_p2(* p ++);
			}

			//PRINTF("fpga: done sending RBF image, waiting for CONF_DONE==1\n");
			/* 4) Дождаться "1" на CONF_DONE */
			while (board_fpga_get_CONF_DONE() == 0)
			{
				++ wcd;
				hardware_spi_b16_p2(0xffff);
			}

			hardware_spi_complete_b16();

			hardware_spi_disconnect();

			//PRINTF("fpga: CONF_DONE asserted, wcd=%u\n", wcd);
			/*
			After the configuration data is accepted and CONF_DONE goes
			high, Cyclone IV devices require 3,192 clock cycles to initialize properly and enter
			user mode.
			*/
		}
	} while (board_fpga_get_NSTATUS() == 0);	// если ошибка - повторяем
	//PRINTF("fpga: board_fpga_loader_PS done\n");
	/* проверяем, проинициализировалась ли FPGA (вошла в user mode). */
	while (HARDWARE_FPGA_IS_USER_MODE() == 0)
	{
		local_delay_ms(1);
		if (-- w == 0)
			goto restart;
	}
	PRINTF("board_fpga_loader_PS: usermode okay\n");
#endif /* (WITHSPIHW && WITHSPI16BIT) */	// for skip in test configurations
}

#elif WITHFPGAWAIT_AS

/* FPGA загружается из собственной микросхемы загрузчика - дождаться окончания загрузки перед инициализацией SPI в процессоре */
static void board_fpga_loader_wait_AS(void)
{
	PRINTF(PSTR("fpga: board_fpga_loader_wait_AS start\n"));
	/* After power up, the Cyclone IV device holds nSTATUS low during POR delay. */
	/* 1) Выставить "1" на nCONFIG */
	//PRINTF(PSTR("fpga: FPGA_NCONFIG_BIT=1\n"));
	FPGA_NCONFIG_PORT_C(FPGA_NCONFIG_BIT);
	local_delay_ms(5);
	/* x) Дождаться "0" на nSTATUS */
	//PRINTF(PSTR("fpga: waiting for FPGA_NSTATUS_BIT==0\n"));
	while (board_fpga_get_NSTATUS() != 0)
			;
	FPGA_NCONFIG_PORT_S(FPGA_NCONFIG_BIT);

	/* 1) Дождаться "1" на nSTATUS */
	//PRINTF(PSTR("fpga: waiting for FPGA_NSTATUS_BIT==1\n"));
	while (board_fpga_get_NSTATUS() == 0)
		;
	/* 2) Дождаться "1" на CONF_DONE */
	//PRINTF(PSTR("fpga: waiting for CONF_DONE==1\n"));
	while (board_fpga_get_CONF_DONE() == 0)
		;
	PRINTF(PSTR("fpga: CONF_DONE asserted\n"));
	/* проверяем, проинициализировалась ли FPGA (вошла в user mode). */
	while (HARDWARE_FPGA_IS_USER_MODE() == 0)
		;
	PRINTF(PSTR("fpga: board_fpga_loader_wait_AS done\n"));
}
#endif

#endif /* WITHFPGAWAIT_AS || WITHFPGALOAD_PS */

#if FPGA_NCONFIG_BIT != 0

void board_fpga_reset(void)
{
	unsigned w = 500;
	/* After power up, the Cyclone IV device holds nSTATUS low during POR delay. */

	FPGA_NCONFIG_PORT_S(FPGA_NCONFIG_BIT);
	local_delay_ms(1);
	/* 1) Выставить "1" на nCONFIG */
	//PRINTF(PSTR("fpga: FPGA_NCONFIG_BIT=1\n"));
	FPGA_NCONFIG_PORT_C(FPGA_NCONFIG_BIT);
	/* x) Дождаться "0" на nSTATUS */
	//PRINTF(PSTR("fpga: waiting for FPGA_NSTATUS_BIT==0\n"));
	while (board_fpga_get_NSTATUS() != 0)
	{
		local_delay_ms(1);
		if (-- w == 0)
			goto restart;
	}
	FPGA_NCONFIG_PORT_S(FPGA_NCONFIG_BIT);
	/* 2) Дождаться "1" на nSTATUS */
	//PRINTF(PSTR("fpga: waiting for FPGA_NSTATUS_BIT==1\n"));
	while (board_fpga_get_NSTATUS() == 0)
	{
		local_delay_ms(1);
		if (-- w == 0)
			goto restart;
	}
restart:
	;
}
#endif

#if WITHDSPEXTFIR

void board_fpga_fir_initialize(void)
{
	PRINTF(PSTR("board_fpga_fir_initialize start\n"));

	TARGET_FPGA_FIR_INITIALIZE();

	hardware_spi_master_setfreq(SPIC_SPEEDUFAST, SPISPEEDUFAST);

	/* Сброс fir filter в FPGA */
	board_set_flt_reset_n(1);	// снять сигнал сброса
	board_update();
	board_set_flt_reset_n(0);	// Выставить сигнал сброса (требуется при применении ALTERA FIR compiler).
	board_update();
	board_set_flt_reset_n(1);	// снять сигнал сброса
	board_update();

	PRINTF(PSTR("board_fpga_fir_initialize done\n"));
}

// Передача одного (первого) 32-битного значения и формирование строба.
static void board_fpga_fir_coef_p1(int_fast32_t v)
{
#if WITHSPI32BIT
	hardware_spi_b32_p1(v);		// на последнем бите формируется coef_in_clk

#elif WITHSPI16BIT
	hardware_spi_b16_p1(v >> 16);
	hardware_spi_b16_p2(v >> 0);		// на последнем бите формируется coef_in_clk

#else /* WITHSPI32BIT */
	hardware_spi_b8_p1(v >> 24);
	hardware_spi_b8_p2(v >> 16);
	hardware_spi_b8_p2(v >> 8);
	hardware_spi_b8_p2(v >> 0);	// на последнем бите формируется coef_in_clk

#endif /* WITHSPI32BIT */
}

// Передача одного (последующего) 32-битного значения и формирование строба.
static void board_fpga_fir_coef_p2(int_fast32_t v)
{
#if WITHSPI32BIT
	hardware_spi_b32_p2(v);		// на последнем бите формируется coef_in_clk

#elif WITHSPI16BIT
	hardware_spi_b16_p2(v >> 16);
	hardware_spi_b16_p2(v >> 0);		// на последнем бите формируется coef_in_clk

#else /* WITHSPI32BIT */
	hardware_spi_b8_p2(v >> 24);
	hardware_spi_b8_p2(v >> 16);
	hardware_spi_b8_p2(v >> 8);
	hardware_spi_b8_p2(v >> 0);	// на последнем бите формируется coef_in_clk

#endif /* WITHSPI32BIT */
}

static void
board_fpga_fir_complete(void)
{
#if WITHSPI32BIT
	hardware_spi_complete_b32();

#elif WITHSPI16BIT
	hardware_spi_complete_b16();

#else /* WITHSPI32BIT */
	hardware_spi_complete_b8();

#endif /* WITHSPI32BIT */
}

static void
board_fpga_fir_connect(void)
{
#if WITHSPI32BIT
	hardware_spi_connect_b32(SPIC_SPEEDUFAST, SPIC_MODE3);

	hardware_spi_b32_p1(0);	// provide clock for reset bit counter while CS=1
	hardware_spi_complete_b32();

#elif WITHSPI16BIT
	hardware_spi_connect_b16(SPIC_SPEEDUFAST, SPIC_MODE3);

	hardware_spi_b16_p1(0);	// provide clock for reset bit counter while CS=1
	hardware_spi_complete_b16();

#else /* WITHSPI32BIT */
	hardware_spi_connect(SPIC_SPEEDUFAST, SPIC_MODE3);

	hardware_spi_b8_p1(0);	// provide clock for reset bit counter while CS=1
	hardware_spi_complete_b8();

#endif /* WITHSPI32BIT */

#if defined (TARGET_FPGA_FIR_CS_BIT)
	TARGET_FPGA_FIR_CS_PORT_C(TARGET_FPGA_FIR_CS_BIT);	/* start sending data to target chip */

#else /* defined (TARGET_FPGA_FIR_CS_BIT) */
	prog_select(targetfir1);	/* start sending data to target chip */

#endif /* defined (TARGET_FPGA_FIR_CS_BIT) */
}

static void
board_fpga_fir_disconnect(void)
{
#if defined (TARGET_FPGA_FIR_CS_BIT)
	TARGET_FPGA_FIR_CS_PORT_S(TARGET_FPGA_FIR_CS_BIT); /* Disable SPI */

#else /* defined (TARGET_FPGA_FIR_CS_BIT) */
	prog_unselect(targetfir1);			/* Disable SPI */

#endif /* defined (TARGET_FPGA_FIR_CS_BIT) */

	hardware_spi_disconnect();
}

/*
static void sendbatch(uint_fast8_t ifir)
{

	static const int_fast32_t va [] =
	{
	#include "C:\user\SVN\tools\coef_seq\fir_normalized_coeff961_lpf_1550_reseq_b.txt"
	};

	unsigned i;
	for (i = 0; i < sizeof va / sizeof va [0]; ++ i)
		board_fpga_fir_coef_p2(va [i]);
}
*/	
// переупорядачивание коэффициентов при выдаче в FIR
// Оригинальный код взят из coef_seq.cpp, Copyright (C) 1991-2012 Altera Corporation
//
// two banks, symmetrical 961:
// coef_seq.exe fir_normalized_coeff961_lpf_1550.txt fir_normalized_coeff961_lpf_1550_reseq_b.txt MCV M4K MSYM 128 2 SGL 1 32
//
static void single_rate_out_write_mcv(const int_fast32_t * coef, int coef_length, int coef_bit_width)
{

	enum coef_store_type { LC, M512, M4K, DUMMY, AUTO };
	enum struct_type { MCV, SER, MBS, PAR };
	//enum sym_chk { NOSYM,MSYM};
	enum poly_type { SGL, INT, DEC };

	//const enum struct_type struct_type = MCV;
	const enum coef_store_type coef_store_type = M4K;
	const int sym = 1;
	const enum poly_type poly_type = SGL;
	const int num_cycles = 512;
	//const int coef_bit_width = 25;

	int mcv_coef_length;
	int mcv_reload_zero_insert;
/****************************MCV FIR resequence Single rate************************/
	int coef_one_mem;
	int num_mac;
	int mem_num;
	const int half_len = (int) ceilf(coef_length / 2.0f);
	int zeros_insert;
	int i,j;

	//PRINTF("single_rate_out_write_mcv: coef_length=%d, half_len=%d\n", coef_length, half_len);
	//if (struct_type == MCV )
	{
		if (sym != 0 && (poly_type == SGL || poly_type == DEC))
		{
			coef_length = half_len;
		}
		num_mac = (int) ceilf((float) coef_length / (float) num_cycles);
		mcv_coef_length = num_mac * num_cycles;		// number of coefficients for write
		if (coef_store_type == M512 || coef_store_type == AUTO)
		{
			coef_one_mem = (int) floorf(18.0f / coef_bit_width);
			if(coef_one_mem == 0)
			{
				coef_one_mem = 1;
			}

		}
		else if (coef_store_type == M4K)
		{
			coef_one_mem = (int) floorf(36.0f / coef_bit_width);
		}
		else
		{
			coef_one_mem = 1;
		}

		mem_num = (int) ceilf((float) num_mac / (float) coef_one_mem);
		mcv_reload_zero_insert = mem_num * coef_one_mem - num_mac;
		if (mem_num == 1)
		{
			mcv_reload_zero_insert = 0;
		}

		if (sym == 0)
		{
			zeros_insert = 0;
		}
		else
		{
			zeros_insert = (int) floorf((float) (mcv_coef_length - coef_length));
		}

		int_fast32_t tmp_coef [mcv_coef_length];
		int_fast32_t wrk_coef [mcv_coef_length];

		// сперва "0", потом значения
		for (i=0; i < zeros_insert; ++ i)
			tmp_coef [i] = 0;
		for (i=0; i < coef_length; ++ i)
			tmp_coef [i + zeros_insert] = coef [i];

		//assert(mcv_coef_length == (coef_length + zeros_insert));

		for (i = 0; i < mcv_coef_length; ++ i)
		{
			wrk_coef[i] = tmp_coef [i];
		}

		for (j = 0; j< num_mac; ++j)
		{
			for (i = 0; i<num_cycles; ++i)
			{
				const int k = i * num_mac + j;
				if (i == 0)
				{
					tmp_coef [k] = wrk_coef [(num_cycles - 1) * num_mac + j];
				}
				else
				{
					tmp_coef [k] = wrk_coef [(i - 1) * num_mac + j];
				}
			}
		}

		for (j = 0; j < num_mac; ++j)
		{
			for (i = 0; i<num_cycles; ++i)
			{
				const int k = i * num_mac + (num_mac - 1 - j);
				if (coef_store_type == LC)
				{
					wrk_coef [j*num_cycles + i] = tmp_coef [k];
				}
			}
		}

		if (mcv_reload_zero_insert > 0)
		{
			for ( i = 0; i < num_cycles; i++)
			{
				for (j = 0; j < num_mac + mcv_reload_zero_insert ; j++)
				{
					int ini_index = i * (num_mac) + j;
					int new_index = i * (num_mac + mcv_reload_zero_insert) + j;
					if(j < (mem_num-1) * coef_one_mem)
					{
						tmp_coef [new_index] = wrk_coef[ini_index];
					}
					else if (j < (mem_num-1) * coef_one_mem + mcv_reload_zero_insert)
					{
						tmp_coef [new_index] = 0;
					}
					else
					{
						tmp_coef [new_index] = wrk_coef [ini_index - mcv_reload_zero_insert];
					}
				}
			}

			//PRINTF(PSTR("send from tmp_coef[], n=%u, @0=%08lX\n"), (num_mac + mcv_reload_zero_insert) * num_cycles, tmp_coef [0]);
			for (i = 0; i < (num_mac + mcv_reload_zero_insert) * num_cycles; ++ i)
			{
				//coef[i] = tmp_coef[i] ;
				board_fpga_fir_coef_p2(tmp_coef [i]);
			}
		}
		else
		{
			//PRINTF(PSTR("send from wrk_coef[], n=%u, @0=%08lX\n"), (num_mac + mcv_reload_zero_insert) * num_cycles, wrk_coef [0]);
			for (i = 0; i< (num_mac + mcv_reload_zero_insert) * num_cycles; ++i)
			{
				//coef[i] = wrk_coef[i] ;
				board_fpga_fir_coef_p2(wrk_coef [i]);
			}
		}
	}

}

/* Выдача расчитанных параметров фильтра в FPGA (симметричные) */
static void 
board_fpga_fir_send(
	const uint_fast8_t ifir,	// номер FIR фильтра в FPGA
	const int_fast32_t * const k, unsigned Ntap, unsigned CWidth
	)
{
#if (WITHSPIHW && WITHSPI16BIT)	// for skip in test configurations

	ASSERT(CWidth <= 24);
	//PRINTF(PSTR("board_fpga_fir_send: ifir=%u, Ntap=%u\n"), ifir, Ntap);
	board_fpga_fir_connect();

	// strobe
	board_fpga_fir_coef_p1(0x00000000);	// one strobe before, without WE required
	board_fpga_fir_complete();

	switch (ifir)
	{
	case 0:
		TARGET_FPGA_FIR1_WE_PORT_C(TARGET_FPGA_FIR1_WE_BIT);
		break;
	case 1:
		TARGET_FPGA_FIR2_WE_PORT_C(TARGET_FPGA_FIR2_WE_BIT);
		break;
	default:
		ASSERT(0);
		break;
	}

	board_fpga_fir_coef_p1(0x00000000);	// 1-st dummy
	board_fpga_fir_coef_p2(0x00000000);	// 2-nd dummy

	//single_rate_out_write_ser(k, Ntap / 2 + 1); // NtapCoeffs(Ntap);
	single_rate_out_write_mcv(k, Ntap, CWidth); // NtapCoeffs(Ntap);
	//sendbatch();

	board_fpga_fir_complete();
	switch (ifir)
	{
	case 0:
		TARGET_FPGA_FIR1_WE_PORT_S(TARGET_FPGA_FIR1_WE_BIT);
		break;
	case 1:
		TARGET_FPGA_FIR2_WE_PORT_S(TARGET_FPGA_FIR2_WE_BIT);
		break;
	default:
		ASSERT(0);
		break;
	}

	// strobe
	board_fpga_fir_coef_p1(0x00000000);	// one strobe after, without WE required
	board_fpga_fir_complete();

	board_fpga_fir_disconnect();
#endif /* (WITHSPIHW && WITHSPI16BIT) */
}

/* поменять местами значение загружаемого профиля FIR фильтра в FPGA */
static void 
boart_tgl_firprofile(
	const uint_fast8_t ifir	// номер FIR фильтра в FPGA
	)
{
	ASSERT(ifir < (sizeof glob_firprofile / sizeof glob_firprofile [0]));

	glob_firprofile [ifir] = ! glob_firprofile [ifir];
	prog_fpga_ctrlreg(targetfpga1);	// FPGA control register
}


void board_reload_fir(uint_fast8_t ifir, const int_fast32_t * const k, unsigned Ntap, unsigned CWidth)
{
	//PRINTF(PSTR("board_reload_fir: ifir=%u, Ntap=%u\n"), ifir, Ntap);
	board_fpga_fir_send(ifir, k, Ntap, CWidth);		/* загрузить массив коэффициентов в FPGA */
	boart_tgl_firprofile(ifir);
}

#endif /* WITHDSPEXTFIR */


/* получения признака переполнения АЦП приёмного тракта */
uint_fast8_t boad_fpga_adcoverflow(void)
{
#if defined (TARGET_FPGA_OVF_GET)
	return TARGET_FPGA_OVF_GET;
#else /* defined (TARGET_FPGA_OVF_GET) */
	return 0;
#endif /* defined (TARGET_FPGA_OVF_GET) */
}

/* получения признака переполнения АЦП микрофонного тракта */
uint_fast8_t boad_mike_adcoverflow(void)
{
#if WITHIF4DSP
	return dsp_getmikeadcoverflow();
#else /* WITHIF4DSP */
	return 0;
#endif /* WITHIF4DSP */
}

/* инициализация при запрещённых прерываниях.
*/
void board_init_io(void)
{
	board_gpio_init();			/* инициализация на вывод битов PIO процессора, если некоторые биты управляются напрямую без SPI */

#if WITHFPGAWAIT_AS
	/* FPGA загружается из собственной микросхемы загрузчика - дождаться окончания загрузки перед инициализацией SPI в процессоре */
	board_fpga_loader_initialize();
	board_fpga_loader_wait_AS();
#endif /* WITHFPGAWAIT_AS */


#if (WITHTWIHW || WITHTWISW)
	i2c_initialize();
#endif /* WITHTWIHW || WITHTWISW */

#if WITHSPIHW || WITHSPISW
	spi_initialize();
#endif /* WITHSPIHW || WITHSPISW */

#if WITHFPGALOAD_PS
	/* FPGA загружается процессором с помощью SPI */
	board_fpga_loader_initialize();
	board_fpga_loader_PS();
#endif /* WITHFPGALOAD_PS */

	board_update_initial();		// Обнуление теневых переменных, синхронизация регистров с теневыми переменными.
	board_reset();			/* формирование импульса на reset_n */

#if WITHSPISLAVE
	hardware_spi_slave_initialize();
#endif /* WITHSPISLAVE */

#if WITHI2SHW
	hardware_audiocodec_initialize();	// Интерфейс к НЧ кодеку
#endif /* WITHI2SHW */

#if WITHSAI1HW
	hardware_fpgacodec_initialize();	// Интерфейс к ВЧ кодеку
#endif /* WITHSAI1HW */

#if WITHSAI2HW
	hardware_fpgaspectrum_initialize();	// Интерфейс к источнику данных о спектре
#endif /* WITHSAI2HW */

#if WITHSAI3HW
	hardware_fpgawidespectrum_initialize();	// Интерфейс к источнику данных о спектре
#endif /* WITHSAI3HW */

#if WITHCPUDACHW
	hardware_dac_initialize();	/* инициализация DAC на STM32F4xx */
#endif /* WITHCPUDACHW */

#if WITHDSPEXTFIR
	board_fpga_fir_initialize();	// порт формирования стробов перезагрузки коэффициентов FIR фильтра в FPGA
#endif /* WITHDSPEXTFIR */

#if WITHCPUADCHW
	board_adc_initialize();
#endif /* WITHCPUADCHW */
}

#if defined (RTC1_TYPE)

/* вызывается при разрешённых прерываниях. */
static void board_rtc_initialize(void)
{
	uint_fast8_t loadreq = board_rtc_chip_initialize();

	if (loadreq == 0)
	{
		/* проверка значений в RTC на допустимость */
		uint_fast16_t year;
		uint_fast8_t month, day;
		uint_fast8_t hour, minute, secounds;
		board_rtc_getdatetime(& year, & month, & day, & hour, & minute, & secounds);
		
		PRINTF(PSTR("board_rtc_initialize: %4d-%02d-%02d %02d:%02d:%02d\n"), year, month, day, hour, minute, secounds);

		if (month < 1 || month > 12 ||
			day < 1 || day > 31 ||
			hour > 23 || minute > 59 || secounds > 59)
		{
			loadreq = 1;
		}
	}

	if (loadreq != 0)
	{
		// Алгоритм найден тут: https://electronix.ru/forum/index.php?showtopic=141655&view=findpost&p=1495868
		static const char ds [] = __DATE__;
		static const char ts [] = __TIME__;

		#define COMPILE_HOUR   (((ts [0]-'0')*10) + (ts [1]-'0'))
		#define COMPILE_MINUTE (((ts [3]-'0')*10) + (ts [4]-'0'))
		#define COMPILE_SECOND (((ts [6]-'0')*10) + (ts [7]-'0'))

		#define COMPILE_YEAR  ((((ds [7]-'0')*10+(ds [8]-'0'))*10+(ds [9]-'0'))*10+(ds [10]-'0'))

		#define COMPILE_MONTH   ((ds [2] == 'n' ? (ds [1] == 'a'? 0 : 5) \
								: ds [2] == 'b' ? 1 \
								: ds [2] == 'r' ? (ds [0] == 'M'? 2 : 3) \
								: ds [2] == 'y' ? 4 \
								: ds [2] == 'l' ? 6 \
								: ds [2] == 'g' ? 7 \
								: ds [2] == 'p' ? 8 \
								: ds [2] == 't' ? 9 \
								: ds [2] == 'v' ? 10 : 11)+1)

		#define COMPILE_DAY  ((ds [4]==' ' ? 0 : ds [4]-'0')*10+(ds [5]-'0'))	
		
		board_rtc_setdatetime(COMPILE_YEAR, COMPILE_MONTH, COMPILE_DAY, COMPILE_HOUR, COMPILE_MINUTE, COMPILE_SECOND);

		//board_rtc_setdate(2016, 3, 1);
	}
}

#else /* defined (RTC1_TYPE) */

// Функция-заглушка для работы FAT FS на системах без RTC
void board_rtc_getdatetime(
	uint_fast16_t * year,
	uint_fast8_t * month,	// 01-12
	uint_fast8_t * day,
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * secounds
	)
{
	// Алгоритм найден тут: https://electronix.ru/forum/index.php?showtopic=141655&view=findpost&p=1495868
	static const char ds [] = __DATE__;
	static const char ts [] = __TIME__;

	#define COMPILE_HOUR   (((ts [0]-'0')*10) + (ts [1]-'0'))
	#define COMPILE_MINUTE (((ts [3]-'0')*10) + (ts [4]-'0'))
	#define COMPILE_SECOND (((ts [6]-'0')*10) + (ts [7]-'0'))

	#define COMPILE_YEAR  ((((ds [7]-'0')*10+(ds [8]-'0'))*10+(ds [9]-'0'))*10+(ds [10]-'0'))

	#define COMPILE_MONTH   ((ds [2] == 'n' ? (ds [1] == 'a'? 0 : 5) \
							: ds [2] == 'b' ? 1 \
							: ds [2] == 'r' ? (ds [0] == 'M'? 2 : 3) \
							: ds [2] == 'y' ? 4 \
							: ds [2] == 'l' ? 6 \
							: ds [2] == 'g' ? 7 \
							: ds [2] == 'p' ? 8 \
							: ds [2] == 't' ? 9 \
							: ds [2] == 'v' ? 10 : 11)+1)

	#define COMPILE_DAY  ((ds [4]==' ' ? 0 : ds [4]-'0')*10+(ds [5]-'0'))		

	* year = COMPILE_YEAR;
	* month = COMPILE_MONTH;
	* day = COMPILE_DAY;
	* hour = COMPILE_HOUR;
	* minute = COMPILE_MINUTE;
	* secounds = COMPILE_SECOND;
}

#endif /* defined (RTC1_TYPE) */


#if ADC1_TYPE == ADC_TYPE_AD9246

static void ad9246_write(uint_fast16_t addr, uint_fast8_t data)
{
	const spitarget_t target = targetadc1;	/* addressing to chip */
	enum { DTL_1, DTL_2, DTL_3, DTL_1_STREAMING };	// 0: 1 byte of data can be transferred

	spi_select(target, SPIC_MODE3);
	spi_progval8_p1(target, (DTL_1 << 5) | ((addr >> 8) & 0x1F));		// Chip Aaddress, D7=0: write
	spi_progval8_p2(target, addr);	// 2-nd byte of instruction header
	spi_progval8_p2(target, data);	// register data
	spi_complete(target);
	spi_unselect(target);
}

static void ad9246_initialize(void)
{
	ad9246_write(0x00, 0x18 | 0x24);	// soft reset
	local_delay_ms(50);
	//ad9246_write(0x00, 0x18);			// soft reset deassert

	ad9246_write(0x09, 0x00);	// Duty cycle stabilizer off
	//ad9246_write(0x0D, 0x01);	// test_io: midscale short

	// TODO: move to update procs
	if (glob_preamp)
		ad9246_write(0x18, 0x00);	// VREF: VREF = 1.25 V
	else
	{
		//ad9246_write(0x18, 0x40);	// VREF: VREF = 1.50 V
		//ad9246_write(0x18, 0x80);	// VREF: VREF = 1.75 V
		ad9246_write(0x18, 0xC0);	// VREF: VREF = 2.00 V
	}

	ad9246_write(0x14, 0x01);	// output_mode: twos complement

	ad9246_write(0xFF, 0x01);			// SW transfer
	//ad9246_write(0xFF, 0x00);			// SW transfer
}
#endif /* ADC1_TYPE == ADC_TYPE_AD9246 */

//#if defined(ADC1_TYPE)
static void prog_rfadc_initialize(void)
{
	#if ADC1_TYPE == ADC_TYPE_AD9246
		ad9246_initialize();
	#endif /* ADC1_TYPE == ADC_TYPE_AD9246 */


}

static void prog_rfadc_update(void)
{
	#if ADC1_TYPE == ADC_TYPE_AD9246
	if (glob_preamp)
		ad9246_write(0x18, 0x00);	// VREF: VREF = 1.25 V
	else
	{
		//ad9246_write(0x18, 0x40);	// VREF: VREF = 1.50 V
		//ad9246_write(0x18, 0x80);	// VREF: VREF = 1.75 V
		ad9246_write(0x18, 0xC0);	// VREF: VREF = 2.00 V
	}
	ad9246_write(0xFF, 0x01);			// SW transfer
	#endif /* ADC1_TYPE == ADC_TYPE_AD9246 */


}
//#endif /* defined(ADC1_TYPE) */


void board_reset(void)
{
	/* Один общий сигнал на все микросхемы */
	board_set_reset_n(1);	// снять сигнал сброса
	board_update();
	board_set_reset_n(0);	// Выставить сигнал сброса (реально уже в "0").
	board_update();
	board_set_reset_n(1);	// снять сигнал сброса
	board_update();
}

/* Initialize chips. All coeffecienters should be already calculated before. */
/* вызывается при разрешённых прерываниях. */
void board_init_chips(void)
{
#if XVTR_R820T2
	r820t_initialize();
	r820t_enable(0);
#endif /* XVTR_R820T2 */

#if defined(ADC1_TYPE)
	prog_rfadc_initialize();
#endif /* defined(ADC1_TYPE) */

#if WITHSI5351AREPLACE
	// do nothing
#else /* WITHSI5351AREPLACE */
	#if defined(DDS1_TYPE)
		prog_dds1_initialize();
	#endif /* defined(DDS1_TYPE) */
	#if defined(DDS2_TYPE)
		prog_dds2_initialize();
	#endif /* defined(DDS2_TYPE) */
	#if defined(DDS3_TYPE)
		prog_dds3_initialize();
	#endif /* defined(DDS3_TYPE) */
#endif /* WITHSI5351AREPLACE */

#if MULTIVFO
	prog_vcodata_init();	// Инициализация границ диапазонов ГУН или границ фильтров за DDS.
#endif /* MULTIVFO */

	// calculate global constants - 
	// setup scaler PLL - 1-st lo
#if defined(PLL1_TYPE)
	board_pll1_initialize();
#endif /* defined(PLL1_TYPE) */

	// setup fixed PLL - 2-nd lo
#if LO1DIVIDEVCO
	prog_pll2_initialize(1);
#else /* LO1DIVIDEVCO */
	#if defined (PLL2_TYPE)
		prog_pll2_initialize(0);
	#endif
#endif /* LO1DIVIDEVCO */

#if defined (RTC1_TYPE)
	board_rtc_initialize();
#endif /* defined (RTC1_TYPE) */

#if defined (TSC1_TYPE)
	board_tsc_initialize();
#endif /* defined (TSC1_TYPE) */

#if defined(CODEC1_TYPE)
	{
		const codec1if_t * const ifc1 = board_getaudiocodecif();

		ifc1->stop();
	}

#endif /* defined(CODEC1_TYPE) */
}

/* Initialize chips. All coeffecienters should be already calculated before. */
/* вызывается при разрешённых прерываниях. */
void board_init_chips2(void)
{
#if defined(CODEC1_TYPE)
	{
		const codec1if_t * const ifc1 = board_getaudiocodecif();

		PRINTF(PSTR("af codec type = '%s'\n"), ifc1->label);

		ifc1->initialize();
		prog_codec1reg();
	}

#endif /* defined(CODEC1_TYPE) */
	//prog_cmx992_initialize(target);

#if defined(CODEC2_TYPE)

	const codec2if_t * const ifc2 = board_getfpgacodecif();

	PRINTF(PSTR("if codec type = '%s'\n"), ifc2->label);
	// MCLK должен уже подаваться в момент инициализации
	ifc2->initialize();	
#endif /* defined(CODEC2_TYPE) */
}
/*
	функция вызывается из обработчиков прерывания или при запрещённых прерываниях.
	получить состояние пинов элкетронного ключа
*/
uint_fast8_t 
hardware_elkey_getpaddle(uint_fast8_t reverse)
{
	uint_fast8_t r = ELKEY_PADDLE_NONE;

#if (ELKEY_BIT_RIGHT != 0) || (ELKEY_BIT_LEFT != 0)

	#if defined (ELKEY_TARGET_LEFT_PIN) && defined (ELKEY_TARGET_RIGHT_PIN)
		// Сигналы от ключа находятся на разных портах ввода-вывода процессора
		const portholder_t stsleft = ELKEY_TARGET_LEFT_PIN;
		const portholder_t stsright = ELKEY_TARGET_RIGHT_PIN;
	#elif defined (ELKEY_TARGET_PIN)
		// Оба сигнала от ключа находятся на одном порту ввода-вывода процессора.
		const portholder_t stsleft = ELKEY_TARGET_PIN;
		const portholder_t stsright = stsleft;
	#else
		#error ELKEY_TARGET_PIN or both ELKEY_TARGET_LEFT_PIN and ELKEY_TARGET_RIGHT_PIN should be defined
	#endif

	if ((stsleft & ELKEY_BIT_LEFT) == 0)
		r |= (reverse ? ELKEY_PADDLE_DASH : ELKEY_PADDLE_DIT);
	if ((stsright & ELKEY_BIT_RIGHT) == 0)
		r |= (reverse ? ELKEY_PADDLE_DIT : ELKEY_PADDLE_DASH);

#endif /*  (ELKEY_BIT_RIGHT != 0) || (ELKEY_BIT_LEFT != 0) */
	return r;
}

#if WITHELKEY
// Инициализация входов электронного ключа, входа CAT_DTR
void 
hardware_elkey_ports_initialize(void)
{
#if defined (ELKEY_INITIALIZE)

	ELKEY_INITIALIZE();

#endif

#if WITHNMEA
	/* сигнал PPS от GPS/GLONASS/GALILEO модуля */
	NMEA_INITIALIZE();

#elif WITHTX && WITHCAT && defined (FROMCAT_DTR_INITIALIZE)

	/* манипуляция от порта RS-232 */
	FROMCAT_DTR_INITIALIZE();

#endif /* WITHTX && WITHCAT */
}

#endif /* WITHELKEY */

#if WITHTX

/* функция вызывается из пользовательской программы. */
uint_fast8_t 
hardware_get_ptt(void)
{
#if WITHBBOX && defined (WITHBBOXTX)
	return WITHBBOXTX;	// автоматический переход на передачу
#elif ELKEY328
	return 1;	// todo: 328
#elif defined (HARDWARE_GET_PTT)
	return HARDWARE_GET_PTT();
#else /*  */
	return 0;
#endif /*  */
}

/* функция вызывается из пользовательской программы. */
/* запрос от внешней аппаратуры на переход в режим TUNE */
uint_fast8_t 
hardware_get_tune(void)
{
#if WITHBBOX && defined (WITHBBOXTUNE)
	return WITHBBOXTUNE;	// автоматический переход в режим настройки
#elif defined (HARDWARE_GET_TUNE)
	return HARDWARE_GET_TUNE();
#else /*  */
	return 0;
#endif /*  */
}

/* функция вызывается из обработчиков прерывания или при запрещённых прерываниях. */
uint_fast8_t 
hardware_get_txdisable(void)
{
#if defined (HARDWARE_GET_TXDISABLE)
	return HARDWARE_GET_TXDISABLE();
#else /*  */
	return 0;
#endif /*  */
}

// Инициализация входа PTT, входа CAT_RTS и TXDISABLE
void 
hardware_ptt_port_initialize(void)
{
#if defined (PTT_INITIALIZE)
	PTT_INITIALIZE();
#endif /* defined (PTT_INITIALIZE) */
#if WITHCAT
	FROMCAT_RTS_INITIALIZE();
#endif /* WITHCAT */
}

/* функция вызывается из обработчиков прерывания или при запрещённых прерываниях. */
void hardware_txpath_set(
	portholder_t txpathstate
	)
{
#if defined (TXPATH_BIT_GATE)
	const portholder_t mask = TXPATH_BIT_GATE;
#elif defined (TXPATH_BITS_ENABLE)
	const portholder_t mask = TXPATH_BITS_ENABLE;
#else
	const portholder_t mask = 0;
#endif

#if CPUSTYLE_ATMEGA
	// ксли процессор может только целиком читать/писать весь регситр состояния плрта вывода.
	#if defined (TXPATH_BIT_GATE)

		TXPATH_TARGET_PORT = (TXPATH_TARGET_PORT & ~ mask) | (txpathstate & mask);

	#elif defined (TXPATH_BIT_ENABLE_SSB) || defined (TXPATH_BIT_ENABLE_CW) || defined (TXPATH_BIT_ENABLE_AM) || defined (TXPATH_BIT_ENABLE_NFM)
		// неактивное состояние - запрограммированное на ввод.
		// В регистре дланных "0".
		TXPATH_TARGET_DDR = (TXPATH_TARGET_DDR & ~ mask) | 
			(txpathstate & mask);	// на нужных битах открываются выходы, в регистре данных "0" в нужном месте.

	#else

		(void) mask;
		//#error Missing definition of TXPATH_BIT_GATE оr TXPATH_BIT_ENABLE_xxx

	#endif

#elif CPUSTYLE_ARM || CPUSTYLE_ATXMEGA
	// если у процессора есть возможность ставить/сбрасывать биты в регистрах состояния вывода по отдельности,
	// кроме этого - порт пограммируется на работу в режиме "открытый сток".
	#if defined (TXPATH_BIT_GATE)

		TXPATH_TARGET_PORT_C(~ txpathstate & mask);
		TXPATH_TARGET_PORT_S(txpathstate & mask);

	#elif defined (TXPATH_BIT_ENABLE_SSB) && defined (TXPATH_BIT_ENABLE_CW)

		TXPATH_TARGET_PORT_S(~ txpathstate & mask);
		TXPATH_TARGET_PORT_C(txpathstate & mask);

	#else

		(void) mask;
		//#error Missing definition of TXPATH_BIT_GATE оr TXPATH_BIT_ENABLE_xxx

	#endif

#else
	#error Undefined CPUSTYLE_XXX
#endif
}

// Инициализация управления трактом передатчика
void 
hardware_txpath_initialize(void)
{

#if CPUSTYLE_ATMEGA

	// Биты управления трактом на передачу
	#if defined (TXPATH_BITS_ENABLE)
		// неактивное состояние - запрограммированное на ввод.
		// В регистре дланных "0".
		TXPATH_TARGET_PORT &= ~ TXPATH_BITS_ENABLE;
		TXPATH_TARGET_DDR &= ~ TXPATH_BITS_ENABLE;	/* бит выхода манипуляции - открытый сток */

	#elif defined (TXPATH_BIT_GATE)

		TXPATH_TARGET_PORT &= ~ TXPATH_BIT_GATE;	/* неактивное состояние - "0" */
		TXPATH_TARGET_DDR |= TXPATH_BIT_GATE;	/* бит выхода манипуляции */
	
	#elif TXPATH_BIT_GATE_RX
		#error TODO: complete TXPATH_BIT_GATE_RX support
	#endif

#elif CPUSTYLE_ATXMEGA

	TXPATH_INITIALIZE();

#elif CPUSTYLE_ARM

	TXPATH_INITIALIZE();

#else

	#error Undefined CPUSTYLE_XXX

#endif
}

#else /* WITHTX */

void hardware_ptt_port_initialize(void)
{

}

/* функция вызывается из пользовательской программы. */
/* запрос от внешней аппаратуры на переход в режим TUNE */
uint_fast8_t 
hardware_get_tune(void)
{
	return 0;
}
/* функция вызывается из пользовательской программы. */
uint_fast8_t
hardware_get_ptt(void)
{
	return 0;
}

/* функция вызывается из обработчиков прерывания или при запрещённых прерываниях. */
void hardware_txpath_set(
	portholder_t txpathstate
	)
{
}

void
hardware_txpath_initialize(void)
{
}

#endif /* WITHTX */

/* получить значение от АЦП s-метра */
uint_fast8_t 
board_getsmeter(uint_fast8_t * tracemax, uint_fast8_t minval, uint_fast8_t maxval, uint_fast8_t clean)
{
#if WITHBARS || WITHINTEGRATEDDSP
	#if WITHINTEGRATEDDSP
		return dsp_getsmeter(tracemax, minval, maxval, clean);
	#elif WITHCPUADCHW
		const uint_fast8_t v = board_getadc_filtered_u8(SMETERIX, minval, maxval);
		* tracemax = v;
		return v;
	#else
		* tracemax = minval;
		return minval;
	#endif /* WITHINTEGRATEDDSP */
#else /* WITHBARS || WITHINTEGRATEDDSP */
	* tracemax = minval;
	return minval;
#endif /* WITHBARS || WITHINTEGRATEDDSP */
}



uint_fast8_t board_getvox(void)	/* получить значение от детектора VOX 0..UINT8_MAX */
{
#if WITHTX && WITHVOX
	#if WITHINTEGRATEDDSP
		return dsp_getvox(UINT8_MAX);
	#else /* WITHINTEGRATEDDSP */
		return board_getadc_unfiltered_u8(VOXIX, 0, UINT8_MAX);
	#endif /* WITHINTEGRATEDDSP */
#else /* WITHTX && WITHVOX */
	return 0;
#endif /* WITHTX && WITHVOX */
}

uint_fast8_t board_getavox(void)	/* получить значение от детектора Anti-VOX 0..UINT8_MAX */
{
#if WITHTX && WITHVOX
	#if WITHINTEGRATEDDSP
		return dsp_getavox(UINT8_MAX);
	#else /* WITHINTEGRATEDDSP */
		return board_getadc_unfiltered_u8(AVOXIX, 0, UINT8_MAX);
	#endif /* WITHINTEGRATEDDSP */
#else /* WITHTX && WITHVOX */
	return 0;
#endif /* WITHTX && WITHVOX */
}

#if WITHTX && WITHSWRMTR

// возврат считанных с АЦП значений forward и reflected
// коррекция неодинаковости детекторов
adcvalholder_t board_getswrmeter_unfiltered(
	adcvalholder_t * reflected, 	// в знаяениях АЦП
	uint_fast8_t swrcalibr	// 90..110 - коррекция
	)
{
	// SWR indicator test
	// 1000 & 333 = swr=2, 1000 & 250 = swr=1,66, 1000 & 500 = swr=3
	//* reflected = 333;
	//return 1000;
	* reflected = board_getadc_unfiltered_truevalue(REF) * (unsigned long) swrcalibr / 100;		// калибровка - умножение на 0.8...1.2 с точностью в 0.01;
	return board_getadc_unfiltered_truevalue(FWD);
}

// возврат считанных с АЦП значений forward и reflected
// коррекция неодинаковости детекторов
adcvalholder_t board_getswrmeter(
	adcvalholder_t * reflected, 	// в знаяениях АЦП
	uint_fast8_t swrcalibr	// 90..110 - коррекция
	)
{
	// SWR indicator test
	// 1000 & 333 = swr=2, 1000 & 250 = swr=1,66, 1000 & 500 = swr=3
	//* reflected = 333;	
	//return 1000;
	* reflected = board_getadc_filtered_truevalue(REFMRRIX) * (unsigned long) swrcalibr / 100;		// калибровка - умножение на 0.8...1.2 с точностью в 0.01;
	return board_getadc_filtered_truevalue(FWDMRRIX);
}

uint_fast8_t board_getpwrmeter(
	uint_fast8_t * toptrace	// peak hold
	)
{
	const uint_fast8_t f = board_getadc_unfiltered_u8(PWRMRRIX, 0, UINT8_MAX);
	* toptrace = f;
	return f;
}

#elif WITHTX && WITHPWRMTR

uint_fast8_t board_getpwrmeter(
	uint_fast8_t * toptrace		// peak hold
	)
{
	const uint_fast8_t f = board_getadc_unfiltered_u8(PWRMRRIX, 0, UINT8_MAX);
	* toptrace = f;
	return f;
}

#else

// нет такой функции
// возврат считанных с АЦП значений forward и reflected
adcvalholder_t board_getswrmeter_unfiltered(
	adcvalholder_t * reflected, 	// в знаяениях АЦП
	uint_fast8_t swrcalibr	// 90..110 - коррекция
	)
{
	const adcvalholder_t forward = 100;
	* reflected = 0;
	return forward;
}

// нет такой функции
// возврат считанных с АЦП значений forward и reflected
adcvalholder_t board_getswrmeter(
	adcvalholder_t * reflected, 	// в знаяениях АЦП
	uint_fast8_t swrcalibr	// 90..110 - коррекция
	)
{
	const adcvalholder_t forward = 100;
	* reflected = 0;
	return forward;
}

// нет такой функции
uint_fast8_t board_getpwrmeter(
	uint_fast8_t * toptrace	// peak hold
	)
{
#if WITHPWRLIN
#else /* WITHPWRLIN */
#endif /* WITHPWRLIN */
	* toptrace = 0;
	return 0;
}
#endif


#if (SIDETONE_TARGET_BIT != 0) || WITHINTEGRATEDDSP

//#define WITHSIDETONEDEBUG	1	// всегда пищать с частотой 1 кГц - отладка

enum 
{
#if WITHSUBTONES
	SNDI_SUBTONE,
#endif /* WITHSUBTONES */
	SNDI_KEYBEEP,		// меньшие номера - более приоритетные звуки
	SNDI_SIDETONE,
#if WITHSIDETONEDEBUG
	SNDI_DEBUG,
#endif /* WITHSIDETONEDEBUG */
	//
	SNDI_SIZE		// количество профилей звуков, которые могут звучать в устройстве
};




// параметры программирования таймеров
static uint_fast8_t gstate [SNDI_SIZE];		/* признак включённого */
static uint_fast8_t gprei [SNDI_SIZE];
static unsigned gvalue [SNDI_SIZE];	/* делитель или FTW для синтезатора озвучки */
static uint_least16_t gtone [SNDI_SIZE];
static RAMDTCM SPINLOCK_t gpreilock = SPINLOCK_INIT;

/* если параметры для данной частоты уже рассчитывали - просто возврат */
static uint_fast8_t 
board_calcs_setfreq(
	uint_fast8_t sndi,		/* sound profile code */
	uint_least16_t tonefreq01)	/* tonefreq - частота в десятых долях герца. Минимум - 400 герц (определено набором команд CAT). */
{
	if (gtone [sndi] == tonefreq01)
		return 0;	/* нет изменения параметров */
	gtone [sndi] = tonefreq01;

	unsigned value;
	const uint_fast8_t prei = hardware_calc_sound_params(tonefreq01, & value);


	system_disableIRQ();
	SPIN_LOCK(& gpreilock);
	gprei [sndi] = prei;
	gvalue [sndi] = value;
	SPIN_UNLOCK(& gpreilock);
	system_enableIRQ();

	return 1;
}

/* вызывается при запрещённых прерываниях */
static void
board_sounds_resched(void)
{
	uint_fast8_t i;

	for (i = 0; i < SNDI_SIZE; ++ i)
	{
		if (gstate [i] != 0)
		{
			hardware_sounds_setfreq(gprei [i], gvalue [i]);		// эта функция всегда включает звук
			return;
		}
	}
	/* ни одного звука для генерации не осталось */
	hardware_sounds_disable();	// выключаем звук
}

/* подзвучка клавиш - установка тона */
/* вызывается из update hardware (user mode).	*/
void 
board_keybeep_setfreq(
	uint_least16_t tonefreq)	/* tonefreq - частота в герцах. Минимум - 400 герц (определено набором команд CAT). */
{
	enum { sndi = SNDI_KEYBEEP };
	if (board_calcs_setfreq(sndi, tonefreq * 10) != 0)	/* если частота изменилась - перепрограммируем */
	{
		system_disableIRQ();
		SPIN_LOCK(& gpreilock);
		board_sounds_resched();
		SPIN_UNLOCK(& gpreilock);
		system_enableIRQ();
	}
}

/* самоконтроль - установка тона */
/* вызывается из update hardware (user mode).	*/
void 
board_sidetone_setfreq(
	uint_least16_t tonefreq)	/* tonefreq - частота в герцах. Минимум - 400 герц (определено набором команд CAT). */
{
	enum { sndi = SNDI_SIDETONE };
	if (board_calcs_setfreq(sndi, tonefreq * 10) != 0)	/* если частота изменилась - перепрограммируем */
	{
		system_disableIRQ();
		SPIN_LOCK(& gpreilock);
		board_sounds_resched();
		SPIN_UNLOCK(& gpreilock);
		system_enableIRQ();
	}
}


/* подзвучка клавиш (вызывается из обработчика перрываний) */
void board_keybeep_enable(uint_fast8_t state)
{
	const uint_fast8_t v = state != 0;
	enum { sndi = SNDI_KEYBEEP };

	if (gstate [sndi] != v)
	{
		gstate [sndi] = v;
		board_sounds_resched();
	}
}

/* самоконтроль (вызывается из обработчика перрываний) */
void board_sidetone_enable(uint_fast8_t state)
{
	const uint_fast8_t v = state != 0;
	enum { sndi = SNDI_SIDETONE };

	if (gstate [sndi] != v)
	{
		gstate [sndi] = v;
		board_sounds_resched();
	}
}

/* тестовый звук (вызывается из обработчика перрываний или инициализации при запрещённых прерываниях) */
void board_testsound_enable(uint_fast8_t state)
{
#if WITHSIDETONEDEBUG
	const uint_fast8_t v = state != 0;
	enum { sndi = SNDI_DEBUG };

	if (gstate [sndi] != v)
	{
		gstate [sndi] = v;
		board_sounds_resched();
	}
#endif /* WITHSIDETONEDEBUG */
}

/* NFM передача subtone */
/* вызывается из update hardware (user mode).	*/
void 
board_subtone_setfreq(
	uint_least16_t tonefreq01)	/* tonefreq - частота в десятых долях герца. */
{
#if WITHSUBTONES
	enum { sndi = SNDI_SUBTONE };
	if (board_calcs_setfreq(sndi, tonefreq01) != 0)	/* если частота изменилась - перепрограммируем */
	{
		system_disableIRQ();
		SPIN_LOCK(& gpreilock);
		board_sounds_resched();
		SPIN_UNLOCK(& gpreilock);
		system_enableIRQ();
	}
#endif /* WITHSUBTONES */
}

void board_subtone_enable(uint_fast8_t state)
{
#if WITHSUBTONES
	const uint_fast8_t v = state != 0;
	enum { sndi = SNDI_SUBTONE };

	if (gstate [sndi] != v)
	{
		gstate [sndi] = v;
		board_sounds_resched();
	}
#endif /* WITHSUBTONES */
}

/* вызывается при запрещённых прерываниях. */
void board_beep_initialize(void)
{
	hardware_beep_initialize();
	hardware_sounds_disable();

#if WITHSIDETONEDEBUG

	enum { sndi = SNDI_DEBUG };
	gstate [sndi] = 1;
	if (board_calcs_setfreq(sndi, 1000 * 10) != 0)	/* если частота изменилась - перепрограммируем */
	{
		board_sounds_resched();
	}
#endif /* WITHSIDETONEDEBUG */
}


#else	 /* SIDETONE_TARGET_BIT != 0 */
/* функция - заглушка */
/* подзвучка клавиш */
/* вызывается из update hardware (манипуляция может происходить из прерываний).	*/
void 
board_keybeep_setfreq(
	uint_least16_t tonefreq)	/* tonefreq - частота в герцах. Минимум - 400 герц (определено набором команд CAT). */
{
}
/* функция - заглушка */
/* самоконтроль */
/* вызывается из update hardware (манипуляция может происходить из прерываний).	*/
void 
board_sidetone_setfreq(
	uint_least16_t tonefreq)	/* tonefreq - частота в герцах. Минимум - 400 герц (определено набором команд CAT). */
{
}
/* функция - заглушка */
/* подзвучка клавиш  */
void board_keybeep_enable(uint_fast8_t state)
{

}
/* функция - заглушка */
/* тестовый звук (вызывается из обработчика перрываний или инициализации при запрещённых прерываниях) */
void board_testsound_enable(uint_fast8_t state)
{
}
/* функция - заглушка */
/* самоконтроль */
void board_sidetone_enable(uint_fast8_t state)
{
}
/* функция - заглушка */
/* subtone */
void
board_subtone_setfreq(
	uint_least16_t tonefreq01)	/* tonefreq - частота в десятых долях герца. */
{
}
/* функция - заглушка */
/* subtone */
void board_subtone_enable(uint_fast8_t state)
{
}

/* функция - заглушка */
/* вызывается при запрещённых прерываниях. */
void board_beep_initialize(void)
{
}

#endif /* SIDETONE_TARGET_BIT != 0 */


// ADC intgerface functions

// Для поддержки случаев, когда входы АЦП используются не подряд
// Готовые значения для выдачи в регистр ADCMUX
static const uint_fast8_t adcinputs [] =
{

#if WITHVOLTLEVEL 
	VOLTSOURCE, // Средняя точка делителя напряжения, для АКБ
#endif /* WITHVOLTLEVEL */

#if WITHBARS
	#if ! WITHINTEGRATEDDSP
		SMETERIX,		// вход S-метра
	#endif /* ! WITHINTEGRATEDDSP */
	#if WITHTX && WITHSWRMTR
		PWRI,		// Индикатор мощности передатчика
		FWD,
		REF,
	#elif WITHPWRMTR
		PWRI,		// Индикатор мощности передатчика
	#endif
#endif /* WITHBARS */

#if WITHPOTWPM
	POTWPM,		/* потенциометр ввода скорости передачи в телеграфе */
#endif /* WITHPOTWPM */
#if WITHPOTPOWER
	POTPOWER,			// регулировка мощности
#endif /* WITHPOTPOWER */
#if WITHPOTPBT
	POTPBT,		// потенциометр управления сужением полосы ПЧ
#endif /* WITHPOTPBT */
#if WITHIFSHIFT && WITHPOTIFSHIFT
	POTIFSHIFT,	// потенциометр управления сдвигом полосы ПЧ
#endif /* WITHIFSHIFT && WITHPOTIFSHIFT */
#if WITHPOTNOTCH && WITHNOTCHFREQ
	POTNOTCH,			// потенциометр управления частотой NOTCH фильтра
#endif /* WITHPOTNOTCH && WITHNOTCHFREQ */

#if WITHTX && WITHVOX && ! WITHINTEGRATEDDSP
	AVOXIX,			// Уровень Anti-VOX
	VOXIX,			// Уровень VOX
#endif /* WITHTX && WITHVOX && ! WITHINTEGRATEDDSP */

#if WITHTEMPSENSOR
	TEMPIX,
#endif /* WITHTEMPSENSOR */

#if WITHREFSENSOR
	VREFIX,			// Reference voltage
#endif /* WITHREFSENSOR */

#if WITHVOLTSENSOR
	VOLTSIX,
#endif /* WITHVOLTSENSOR */

#if WITHTHERMOLEVEL
	XTHERMOIX,		// Exernal thermo sensor ST LM235Z
#endif /* WITHTHERMOLEVEL */

#if WITHPOTIFGAIN
	POTIFGAIN,
#endif /* WITHPOTIFGAIN */
#if WITHPOTAFGAIN
	POTAFGAIN,
#endif /* WITHPOTAFGAIN */
#if WITHCURRLEVEL2
	PASENSEIX2,		// 100W PA current sense - ACS712-30 chip
	PAREFERIX2,
#elif WITHCURRLEVEL
	PASENSEIX,		// PA or driver current sense - ACS712-05 chip
#endif /* WITHCURRLEVEL */

#if KEYBOARD_USE_ADC

	// Входы опроса клавиатуры
#ifdef KI_LIST
	KI_LIST
#elif KI_COUNT == 6
	KI0, KI1, KI2, KI3, KI4, KI5,
#elif KI_COUNT == 5
	KI0, KI1, KI2, KI3, KI4,
#elif KI_COUNT == 4
	KI0, KI1, KI2, KI3,
#elif KI_COUNT == 3
	KI0, KI1, KI2,
#elif KI_COUNT == 2
	KI0, KI1,
#elif KI_COUNT == 1
	KI0,
#elif KI_COUNT == 0
#else
	#error KI_COUNT or KI_LIST is not defined
#endif

#endif	/* KEYBOARD_USE_ADC */
	
#if CTLSTYLE_RAVENDSP_V1	// Трансивер Вороненок с IF DSP трактом
	POTAUX1,		// PC3 AUX1
	POTAUX2,		// PC4 AUX2
	POTAUX3,		// PC5 AUX3
#endif /* CTLSTYLE_RAVENDSP_V1 */
#if CTLSTYLE_RAVENDSP_V3	// Трансивер Вороненок с IF DSP трактом
	POTAUX1,		// PC3 AUX1
	POTAUX2,		// PC4 AUX2
	ALCINIX,		// PC5 ALC IN
#endif /* CTLSTYLE_RAVENDSP_V3 */
#if CTLSTYLE_RAVENDSP_V4	// Трансивер Вороненок с IF DSP трактом
	POTAUX1,		// PC3 AUX1
	POTAUX2,		// PC4 AUX2
	ALCINIX,		// PC5 ALC IN
#endif /* CTLSTYLE_RAVENDSP_V4 */
#if CTLSTYLE_RAVENDSP_V5
	ALCINIX,		// PB1 ALC IN
#endif /* CTLSTYLE_RAVENDSP_V5 */
#if CTLSTYLE_RAVENDSP_V6
	ALCINIX,		// PB1 ALC IN
#endif /* CTLSTYLE_RAVENDSP_V6 */
#if CTLSTYLE_RAVENDSP_V7
	ALCINIX,		// PB1 ALC IN
#endif /* CTLSTYLE_RAVENDSP_V7 */
#if CTLSTYLE_RAVENDSP_V8
	ALCINIX,		// PB1 ALC IN
#endif /* CTLSTYLE_RAVENDSP_V8 */
#if CTLSTYLE_STORCH_V1
	ALCINIX,		// PB1 ALC IN
#endif /* CTLSTYLE_STORCH_V1 */
#if CTLSTYLE_STORCH_V2
	ALCINIX,		// PB1 ALC IN
#endif /* CTLSTYLE_STORCH_V2 */
};

/* получить количество каналов АЦП, задействованных в устройстве */
uint_fast8_t board_get_adcinputs(void)
{
	return sizeof adcinputs / sizeof adcinputs [0];
}

/* получить канал АЦП но индексу в таблице */
uint_fast8_t board_get_adcch(uint_fast8_t i)
{
	if (i >= board_get_adcinputs())
		return 0;
	ASSERT(i < board_get_adcinputs());
	return adcinputs [i];
}

enum
{
	BOARD_ADCFILTER_DIRECT,		/* фильтрация не применяется (значение для всех каналов по умолчанию) */
	BOARD_ADCFILTER_TRACETOP3S,	/* Отслеживание максимума с постоянной времени 3 секунды */
	BOARD_ADCFILTER_LPF,			/* ФНЧ, параметр задается в виде числа с фиксированной точкой */
	//
	BOARD_ADCFILTER_TYPECOUNT
};

enum { ADC_LPF_WND = NTICKS(320) };	// длительность окна

typedef struct lpfdata_tag
{
	sadcvalholder_t queue [ADC_LPF_WND];
	size_t qpos;	/* индекс в очереди куда будем сейчас писать */
	int32_t summ;	/* сумма всех элементов очереди */
} lpfdata_t;

typedef struct boardadc_tag
{

	volatile adcvalholder_t adc_data_raw;	// входные данные для фильтра
	adcvalholder_t adc_data_filtered;		// выход фильтра
	uint8_t adc_data_smoothed_u8;		// выход фильтра
	uint8_t adc_filter;			/* методы фильтрации данных */
	void * lpf;
} boardadc_t;

static boardadc_t badcst [HARDWARE_ADCINPUTS];

/* получить максимальное возможное значение от АЦП */
adcvalholder_t board_getadc_fsval(uint_fast8_t adci)	
{
	ASSERT(adci < HARDWARE_ADCINPUTS);
	boardadc_t * const padcs = & badcst [adci];
	if (adci >= BOARD_ADCX0BASE || adci >= BOARD_ADCX1BASE)
	{
		return 4095;	// MCP3208
	}
	return (1uL << HARDWARE_ADCBITS) - 1;
}

/* получить значение от АЦП */
adcvalholder_t board_getadc_filtered_truevalue(uint_fast8_t adci)	
{
	ASSERT(adci < HARDWARE_ADCINPUTS);
	boardadc_t * const padcs = & badcst [adci];
	return padcs->adc_data_filtered;
}

/* получить значение от АЦП */
adcvalholder_t board_getadc_unfiltered_truevalue(uint_fast8_t adci)	
{
	static const struct
	{
		uint8_t ch;
		uint8_t diff;
	} xad2xlt [8] =
	{
			{	0, 0, },	// DRAIN (negative from midpoint at CH1: ch0=in-, ch1=in+)
			{	1, 0, },
			{	2, 0, },
			{	3, 0, },
			{	4, 0, },
			{	5, 0, },
			{	6, 0, },
			{	7, 0, },
	};

	ASSERT(adci < HARDWARE_ADCINPUTS);
	boardadc_t * const padcs = & badcst [adci];

	if (adci >= BOARD_ADCMRRBASE)
	{
		// mirror - значения АЦП устанавливабтся выходами программных компонентов, без считывания с аппаратуры.
		return padcs->adc_data_raw;
	}
	if (adci >= BOARD_ADCX1BASE)
	{
		// external SPI device (PA BOARD ADC)
#if defined (targetxad2)
		uint_fast8_t valid;
		uint_fast8_t ch = adci - BOARD_ADCX1BASE;
		//PRINTF("targetxad2: ch = %u\n", ch);
		return mcp3208_read(targetxad2, xad2xlt [ch].diff, xad2xlt [ch].ch, & valid);
#else /* defined (targetxad2) */
		return 0;
#endif /* defined (targetxad2) */
	}
	if (adci >= BOARD_ADCX0BASE)
	{
		/* on-board ADC MCP3208-BI/SL chip select (potentiometers) */
#if defined (targetadc2)
		uint_fast8_t valid;
		uint_fast8_t ch = adci - BOARD_ADCX0BASE;
		//PRINTF("targetadc2: ch = %u\n", ch);
		return mcp3208_read(targetadc2, 0, ch, & valid);
#else /* defined (targetadc2) */
		return 0;
#endif /* defined (targetadc2) */
	}
	ASSERT(adci < HARDWARE_ADCINPUTS);
	return padcs->adc_data_raw;
}


/* получить отфильтрованное значение от АЦП в диапазоне lower..upper (включая границы) */
uint_fast8_t board_getadc_filtered_u8(uint_fast8_t adci, uint_fast8_t lower, uint_fast8_t upper)
{
	ASSERT(adci < HARDWARE_ADCINPUTS);
	boardadc_t * const padcs = & badcst [adci];
	const adcvalholder_t t = board_getadc_filtered_truevalue(adci);	// текущее отфильтрованное значение данного АЦП
	const uint_fast8_t v = lower + ((uint_fast32_t) t * (upper - lower) / board_getadc_fsval(adci));	// нормируем к требуемому диапазону
	ASSERT(v >= lower && v <= upper);
	return v;
}

/* получить отфильтрованное значение от АЦП в диапазоне lower..upper (включая границы) */
uint_fast16_t board_getadc_filtered_u16(uint_fast8_t adci, uint_fast16_t lower, uint_fast16_t upper)
{
	ASSERT(adci < HARDWARE_ADCINPUTS);
	boardadc_t * const padcs = & badcst [adci];
	const adcvalholder_t t = board_getadc_filtered_truevalue(adci);	// текущее отфильтрованное значение данного АЦП
	const uint_fast16_t v = lower + ((uint_fast32_t) t * (upper - lower) / board_getadc_fsval(adci));	// нормируем к требуемому диапазону
	ASSERT(v >= lower && v <= upper);
	return v;
}

/* получить отфильтрованное значение от АЦП в диапазоне lower..upper (включая границы) */
uint_fast32_t board_getadc_filtered_u32(uint_fast8_t adci, uint_fast32_t lower, uint_fast32_t upper)
{
	ASSERT(adci < HARDWARE_ADCINPUTS);
	boardadc_t * const padcs = & badcst [adci];
	const adcvalholder_t t = board_getadc_filtered_truevalue(adci);	// текущее отфильтрованное значение данного АЦП
	const uint_fast32_t v = lower + ((uint_fast64_t) t * (upper - lower) / board_getadc_fsval(adci));	// нормируем к требуемому диапазону
	ASSERT(v >= lower && v <= upper);
	return v;
}

/* получить отфильтрованное значение от АЦП в диапазоне lower..upper (включая границы) */
/* поскольку используется для получения позиции потенциометра, применяется фильтрация "гистерезис" */
uint_fast8_t board_getpot_filtered_u8(uint_fast8_t adci, uint_fast8_t lower, uint_fast8_t upper, adcvalholder_t * data)
{
	ASSERT(adci < HARDWARE_ADCINPUTS);
	boardadc_t * const padcs = & badcst [adci];
	const adcvalholder_t t = board_getadc_unfiltered_truevalue(adci);	// текущее отфильтрованное значение данного АЦП
	const uint_fast8_t v = lower + ((uint_fast32_t) filter_hyst(data, t) * (upper - lower) / board_getadc_fsval(adci));	// нормируем к требуемому диапазону
	ASSERT(v >= lower && v <= upper);
	return v;
}

/* получить отфильтрованное значение от АЦП в диапазоне lower..upper (включая границы) */
/* поскольку используется для получения позиции потенциометра, применяется фильтрация "гистерезис" */
uint_fast16_t board_getpot_filtered_u16(uint_fast8_t adci, uint_fast16_t lower, uint_fast16_t upper, adcvalholder_t * data)
{
	ASSERT(adci < HARDWARE_ADCINPUTS);
	boardadc_t * const padcs = & badcst [adci];
	const adcvalholder_t t = board_getadc_unfiltered_truevalue(adci);	// текущее отфильтрованное значение данного АЦП
	const uint_fast16_t v = lower + ((uint_fast32_t) filter_hyst(data, t) * (upper - lower) / board_getadc_fsval(adci));	// нормируем к требуемому диапазону
	ASSERT(v >= lower && v <= upper);
	return v;
}

/* при изменении отфильтрованного значения этого АЦП в диапазоне lower..upper (включая границы)
    возвращаемое значение на каждом вызове приближается к нему на единицу
*/
uint_fast8_t board_getadc_smoothed_u8(uint_fast8_t adci, uint_fast8_t lower, uint_fast8_t upper)
{
	ASSERT(adci < HARDWARE_ADCINPUTS);
	boardadc_t * const padcs = & badcst [adci];
	const uint_fast8_t r = padcs->adc_data_smoothed_u8; // ранее возвращённое значение для данного АЦП
	const adcvalholder_t t = board_getadc_filtered_truevalue(adci);	// текущее отфильтрованное значение данного АЦП
	const uint_fast8_t v = lower + ((uint_fast32_t) t * (upper - lower) / board_getadc_fsval(adci));	// нормируем к требуемому диапазону
	if (r > v)
		padcs->adc_data_smoothed_u8 -= 1;
	else if (r < v)
		padcs->adc_data_smoothed_u8 += 1;
	ASSERT(v >= lower && v <= upper);
	return r;
}

/* получить значение от АЦП в диапазоне lower..upper (включая границы) */
uint_fast8_t board_getadc_unfiltered_u8(uint_fast8_t adci, uint_fast8_t lower, uint_fast8_t upper)	/* получить значение от АЦП в диапазоне lower..upper (включая границы) */
{
	ASSERT(adci < HARDWARE_ADCINPUTS);
	const adcvalholder_t t = board_getadc_unfiltered_truevalue(adci);
	const uint_fast8_t v = lower + (uint_fast8_t) ((uint_fast32_t) t * (upper - lower) / board_getadc_fsval(adci));	// нормируем к требуемому диапазону
	ASSERT(v >= lower && v <= upper);
	return v;
}

/* получить значение от АЦП в диапазоне lower..upper (включая границы) */
uint_fast16_t board_getadc_unfiltered_u16(uint_fast8_t adci, uint_fast16_t lower, uint_fast16_t upper)	/* получить значение от АЦП в диапазоне lower..upper (включая границы) */
{
	ASSERT(adci < HARDWARE_ADCINPUTS);
	const adcvalholder_t t = board_getadc_unfiltered_truevalue(adci);
	const uint_fast16_t v = lower + (uint_fast16_t) ((uint_fast32_t) t * (upper - lower) / board_getadc_fsval(adci));	// нормируем к требуемому диапазону
	ASSERT(v >= lower && v <= upper);
	return v;
}

/* получить значение от АЦП в диапазоне lower..upper (включая границы) */
uint_fast32_t board_getadc_unfiltered_u32(uint_fast8_t adci, uint_fast32_t lower, uint_fast32_t upper)	/* получить значение от АЦП в диапазоне lower..upper (включая границы) */
{
	ASSERT(adci < HARDWARE_ADCINPUTS);
	const adcvalholder_t t = board_getadc_unfiltered_truevalue(adci);
	const uint_fast32_t v = lower + (uint_fast32_t) ((uint_fast64_t) t * (upper - lower) / board_getadc_fsval(adci));	// нормируем к требуемому диапазону
	ASSERT(v >= lower && v <= upper);
	return v;
}

/* установить способ фильтрации данных (в момент выборки их регистра АЦП */
static void hardware_set_adc_filter(uint_fast8_t adci, uint_fast8_t v)
{
	ASSERT(adci < HARDWARE_ADCINPUTS);
	boardadc_t * const padcs = & badcst [adci];
	padcs->adc_filter = v;
}

static void lpf_initialize(lpfdata_t * lpfdata)
{
	memset(& lpfdata->queue, 0, sizeof lpfdata->queue);
	lpfdata->qpos = 0;
	lpfdata->summ = 0;
}

static adcvalholder_t
lpf_filter(lpfdata_t * lpfdata, adcvalholder_t raw)
{
	lpfdata->summ += (sadcvalholder_t) raw;	// добавить входящее
	lpfdata->summ -= lpfdata->queue [lpfdata->qpos];	// вычесть выходящее
	lpfdata->queue [lpfdata->qpos] = raw;
	lpfdata->qpos = (lpfdata->qpos + 1) % ADC_LPF_WND;
	return lpfdata->summ / ADC_LPF_WND;
}

static adcvalholder_t
tracetop3s_filter(adcvalholder_t raw, adcvalholder_t v0)
{
	enum { DELAY3SNUM = 993, DELAY3SDENOM = 1000 };	// todo: сделать расчет в зависимости от частоты системного таймера
	return v0 < raw ? raw : v0 * (uint_fast32_t) DELAY3SNUM / DELAY3SDENOM;
}

/* Установить способ фильтрации данных LPF и частоту среза - параметр 1.0..0.0, умноженное на BOARD_ADCFILTER_LPF_DENOM */
static void
hardware_set_adc_filterLPF(uint_fast8_t adci, lpfdata_t * lpfdata)
{
	ASSERT(adci < HARDWARE_ADCINPUTS);
	boardadc_t * const padcs = & badcst [adci];
	padcs->adc_filter = BOARD_ADCFILTER_LPF;
	padcs->lpf = lpfdata;
	lpf_initialize(lpfdata);
}

// Функция вызывается из обработчика прерывания завершения преобразования
// канала АЦП для запиоминания преобразованного занчения.
void board_adc_store_data(uint_fast8_t adci, adcvalholder_t v)
{
	ASSERT(adci < HARDWARE_ADCINPUTS);
	boardadc_t * const padcs = & badcst [adci];
	ASSERT(v <= board_getadc_fsval(adci));
	padcs->adc_data_raw = v;
}

#if 0

// http://www.poprobot.ru/theory/low_pass_filter

// комплементарный фильтр ???

// pot = (1-K)*pot + K*pot_raw
// где pot - отфильтрованный сигнал, pot_raw - значение на аналоговом входе,
// и наконец K - коэффициент фильтра, который варьируется от 0.0 до 1.0.

//k - коэффицент фильтра 0.0 - 1.0
unsigned int val_tmp_0; //переменная для временного хранения результата измерения

unsigned int get_ADC_fwd(float k)
{
	unsigned int val;
	val = (1 - k) * val_tmp_0 + k * ADC1_Get_Sample(ADC_INDEX_SENSOR_FWD);
	val_tmp_0 = val;

	return val;
}

//k - коэффицент фильтра 0.0 - 1.0
unsigned int val_tmp_1; //переменная для временного хранения результата измерения

unsigned int get_ADC_ref(float k)
{
	unsigned int val;
	val = (1 - k) * val_tmp_1 + k * ADC1_Get_Sample(ADC_INDEX_SENSOR_REF);
	val_tmp_1 = val;

	return val;
}

#endif

// Значение обновляется, если новое значение отличается на HYDELTA или более дискретов
// todo: использоать эту запись алгоритма, только он сейчас, похоже, не работает.
//if ((adc_data_filtered [i] + HYDELTA) <= raw || (raw + HYDELTA) <= adc_data_filtered [i])
//	adc_data_filtered [i] = raw;
adcvalholder_t filter_hyst(
	adcvalholder_t * pv0, 
	adcvalholder_t raw
	)
{
	enum { HYDELTA = (1U << (HARDWARE_ADCBITS - 8)) };
	const adcvalholder_t v0 = * pv0;
	if (raw < v0 && (v0 - raw) >= HYDELTA)
		* pv0 = raw;
	else if (raw > v0 && (raw - v0) >= HYDELTA)
		* pv0 = raw;

	return * pv0;
}
						   
// Функция вызывается из обработчика прерывания после получения значения от последнего канала АЦП
void board_adc_filtering(void)
{
	uint_fast8_t i;

	for (i = 0; i < sizeof badcst / sizeof badcst [0]; ++ i)
	{
		boardadc_t * const padcs = & badcst [i];
		const adcvalholder_t raw = padcs->adc_data_raw;
		switch (padcs->adc_filter)
		{
		case BOARD_ADCFILTER_DIRECT:
			// Значение просто обновляется
			padcs->adc_data_filtered = raw;
			break;

		case BOARD_ADCFILTER_TRACETOP3S:
			// Отслеживание максимума с постоянной времени 3 секунды
			padcs->adc_data_filtered = tracetop3s_filter(raw, padcs->adc_data_filtered);
			break;

		case BOARD_ADCFILTER_LPF:
			padcs->adc_data_filtered = lpf_filter(padcs->lpf, raw);
			break;

		default:
			padcs->adc_data_filtered = raw;
			break;
		}
	}
}

/*
	Для некоторых каналов АЦП вклюяаем фильтрацию значений.
	Для потенциометров на регулировках устраняется дребезг значений.
 */
static void
adcfilters_initialize(void)
{
	#if WITHBARS && ! WITHINTEGRATEDDSP
		hardware_set_adc_filter(SMETERIX, BOARD_ADCFILTER_TRACETOP3S);
	#endif /* WITHBARS && ! WITHINTEGRATEDDSP */

	#if WITHTX && (WITHSWRMTR || WITHPWRMTR)
		{
			static lpfdata_t pwr;

			hardware_set_adc_filterLPF(PWRI, & pwr);	// Включить фильтр
			//hardware_set_adc_filter(PWRI, BOARD_ADCFILTER_DIRECT);		// Отключить фильтр
		}
	#endif /* WITHTX && (WITHSWRMTR || WITHPWRMTR) */

	#if WITHCURRLEVEL2
		{
			static lpfdata_t pasense2;
			static lpfdata_t parefer2;

			hardware_set_adc_filterLPF(PASENSEMRRIX2, & pasense2);	// Включить фильтр с параметром 0.03
			hardware_set_adc_filterLPF(PAREFERMRRIX2, & parefer2);	// Включить фильтр с параметром 0.03
		}
	#elif WITHCURRLEVEL
		{
			static lpfdata_t pasense;

			hardware_set_adc_filterLPF(PASENSEMRRIX, & pasense);	// Включить фильтр с параметром 0.03
		}
	#endif /* WITHCURRLEVEL */

	#if WITHTHERMOLEVEL
		{
			static lpfdata_t temperature;

			hardware_set_adc_filterLPF(XTHERMOMRRIX, & temperature);	// Включить фильтр с параметром 0.03
		}
	#endif /* WITHTHERMOLEVEL */

	#if WITHSWRMTR || WITHPWRMTR
		{
			static lpfdata_t fwd;
			static lpfdata_t ref;

			hardware_set_adc_filterLPF(REFMRRIX, & ref);	// Включить фильтр с параметром 0.03
			hardware_set_adc_filterLPF(FWDMRRIX, & fwd);	// Включить фильтр с параметром 0.03
		}
	#endif /* WITHSWRMTR || WITHPWRMTR */
}


void board_adc_initialize(void)
{
	if (board_get_adcinputs() == 0)
		return;
#if WITHDEBUG
	// Отладочная печать таблицы входов АЦП, используемых в работе
	{
		uint_fast8_t i;
		/* какие из каналов включать.. */
		PRINTF(PSTR("ADCINPUTS_COUNT=%d: "), (int) board_get_adcinputs());
		for (i = 0; i < board_get_adcinputs(); ++ i)
		{
			PRINTF(PSTR("%d%c"), board_get_adcch(i), (i + 1) == board_get_adcinputs() ? '\n' : ',');
		}
	}
#endif /* WITHDEBUG */

#if WITHCPUADCHW

	hardware_adc_initialize();

#endif /* WITHCPUADCHW */
	adcfilters_initialize();
}


#if WITHKEYBOARD

#if KEYBOARD_USE_ADC6

static uint_fast8_t
kbd_adc6_decode(
	uint_fast8_t v
	)
{
	enum 
	{ 
		L0 = 0, L6 = 255,	// Крайние значение шкалы АЦП
		// Расчётные значения для идеальных значений резисторов:
		// Сверху - 6.8К, кнопки подключены "звездочкой" через резисторы - 0, 2К, 3.9К, 6.8К, 12К, 27К
		L1 = 58, L2 = 93, L3 = 128, L4 = 163, L5 = 204
	};

	enum { 
		F1 = (L1 - L0) / 2 + L0,
		F2 = (L2 - L1) / 2 + L1,
		F3 = (L3 - L2) / 2 + L2,
		F4 = (L4 - L3) / 2 + L3,
		F5 = (L5 - L4) / 2 + L4,
		F6 = (L6 - L5) / 2 + L5
		};

	if (v < F4)
	{
		if (v < F1)	return 0;								
		if (v < F2)	return 1;
		if (v < F3)	return 2;
		return 3;
	}
	else
	{
		if (v < F5)	return 4;
		if (v < F6)	return 5;
	}

	return KEYBOARD_NOKEY;
}

#endif /* KEYBOARD_USE_ADC6 */

#if KEYBOARD_USE_ADC6_V1

static uint_fast8_t
kbd_adc6v1_decode(
	uint_fast8_t v
	)
{
	enum 
	{ 
		L0 = 0, L6 = 255,	// Крайние значение шкалы АЦП
		// Расчётные значения для идеальных значений резисторов:
		// Сверху - 6.8К, между кнопками "цепочкой" 1.3К, 2К, 3.3К, 6.8К, 20К
		L1 = 41, L2 = 87, L3 = 126, L4 = 170, L5 = 213
	};

	enum { 
		F1 = (L1 - L0) / 2 + L0,
		F2 = (L2 - L1) / 2 + L1,
		F3 = (L3 - L2) / 2 + L2,
		F4 = (L4 - L3) / 2 + L3,
		F5 = (L5 - L4) / 2 + L4,
		F6 = (L6 - L5) / 2 + L5
		};

	if (v < F4)
	{
		if (v < F1)	return 0;								
		if (v < F2)	return 1;
		if (v < F3)	return 2;
		return 3;
	}
	else
	{
		if (v < F5)	return 4;
		if (v < F6)	return 5;
	}

	return KEYBOARD_NOKEY;
}
#endif /* KEYBOARD_USE_ADC6_V1 */

/* проверка нажатия кнопки второго валкодера - возврат 1 */
static uint_fast8_t
bnchk_encoder2(void)
{
#if defined (TARGET_ENC2BTN_GET)
	return TARGET_ENC2BTN_GET;
#else /* defined (TARGET_ENC2BTN_GET) */
	return 0;
#endif /* defined (TARGET_ENC2BTN_GET) */
}

/* проверка нажатия кнопки питания - возврат 1 */
static uint_fast8_t
bnchk_poweroff(void)
{
#if defined (TARGET_POWERBTN_GET)
	return TARGET_POWERBTN_GET;
#else /* defined (TARGET_POWERBTN_GET) */
	return 0;
#endif /* defined (TARGET_POWERBTN_GET) */
}

//#define KI_LIST	KI5, KI4, KI3, KI2, KI1, KI0,	// инициализаторы для функции перекодировки
//#define KI_LIST	KI4, KI3, KI2, KI1, KI0,	// инициализаторы для функции перекодировки
//#define KI_LIST	KI2, KI1, KI0,	// инициализаторы для функции перекодировки
//#define KI_LIST	KI0,	// инициализаторы для функции перекодировки

// вызывается с частотой TICKS_FREQUENCY (например, 200 Гц) с запрещенными прерываниями.
// Если ничего - возвращаем KEYBOARD_NOKEY
uint_fast8_t 
board_get_pressed_key(void)
{
	//return KEYBOARD_NOKEY;
#if KEYBOARD_USE_ADC
	// KI_COUNT - количесто входов АЦП, используемых для опроса клавиатуры
	static const uint_fast8_t kitable [KI_COUNT] =
	{
	#ifdef KI_LIST
		KI_LIST
	#elif KI_COUNT == 6
		KI5, KI4, KI3, KI2, KI1, KI0,
	#elif KI_COUNT == 5
		KI4, KI3, KI2, KI1, KI0,
	#elif KI_COUNT == 4
		KI3, KI2, KI1, 	KI0,
	#elif KI_COUNT == 3
		KI2, KI1, KI0,
	#elif KI_COUNT == 2
		KI1, KI0,
	#elif KI_COUNT == 1
		KI0,
	#else
		#error KI_COUNT or KI_LIST is not defined
	#endif
	};

	// Для безклавиатурных конфигураций
#if KEYBOARD_USE_ADC && KI_COUNT == 0
	return KEYBOARD_NOKEY;
#endif /* KEYBOARD_USE_ADC && KI_COUNT == 0 */

	uint_fast8_t ki;
	#if KEYBOARD_USE_ADC6 || KEYBOARD_USE_ADC6_V1

		enum { NK = 6 };

	#else /* KEYBOARD_USE_ADC6 || KEYBOARD_USE_ADC6_V1 */

		enum { X = KEYBOARD_NOKEY, NK = 4 };
		static const uint_fast8_t kixlat4 [] = 
		{
			0, X, 1, 1, 1, X, 2, 2, 2, X, 3, 3, 3, 3, X, X,	/* с защитными интервалами */
		};

	#endif /* KEYBOARD_USE_ADC6 || KEYBOARD_USE_ADC6_V1 */

	for (ki = 0; ki < KI_COUNT; ++ ki)
	{
	#if KEYBOARD_USE_ADC6
		// шесть кнопок на одном входе АЦП
		const uint_fast8_t v = kbd_adc6_decode(board_getadc_unfiltered_u8(kitable [ki], 0, 255));
	#elif KEYBOARD_USE_ADC6_V1
		// шесть кнопок на одном входе АЦП
		const uint_fast8_t v = kbd_adc6v1_decode(board_getadc_unfiltered_u8(kitable [ki], 0, 255));
	#else /* KEYBOARD_USE_ADC6 || KEYBOARD_USE_ADC6_V1 */
		// исправление ошибочного срабатывания - вокруг значений при нажатых клавишах
		// (между ними) добавляются защитные интервалы, обрабаатываемые как ненажатая клавиша.
		// Последний инлекс не выдается, отпущеная кнопка - предпоследний.
		// четыре кнопки на одном входе АЦП
		const uint_fast8_t v = kixlat4 [board_getadc_unfiltered_u8(kitable [ki], 0, sizeof kixlat4 / sizeof kixlat4 [0] - 1)];
	#endif /* KEYBOARD_USE_ADC6 || KEYBOARD_USE_ADC6_V1 */
		if (v != KEYBOARD_NOKEY)
		{
			return ki * NK + v;
		}
	}
	enum { AKBDEND = KI_COUNT * NK };
#else	/* KEYBOARD_USE_ADC */
	enum { AKBDEND = 0 };
#endif	/* KEYBOARD_USE_ADC */

#if defined (TARGET_ENC2BTN_GET)
	if (TARGET_ENC2BTN_GET != 0)
		return AKBDEND + 0;
#endif /* defined (TARGET_ENC2BTN_GET) */
#if defined (TARGET_POWERBTN_GET)
	if (TARGET_POWERBTN_GET != 0)
		return AKBDEND + 1;
#endif /* defined (TARGET_POWERBTN_GET) */

#if KBD_MASK

	uint_fast8_t i;
	uint_fast8_t bitpos;
	const portholder_t v = ~ KBD_TARGET_PIN;
	portholder_t srcmask = KBD_MASK;
	
	for (bitpos = 0, i = AKBDEND; srcmask != 0; ++ bitpos, srcmask >>= 1)
	{
		const portholder_t mask = (portholder_t)1 << bitpos;
		if ((srcmask & 0x01) == 0)
			continue;	// нет интересующего бита в этой позиции
		if ((mask & v) != 0)
		{
			return i;	// есть нажатие
		}
		++ i;
	}

#endif	/* KBD_MASK */

	return KEYBOARD_NOKEY;
}


#endif /* WITHKEYBOARD */

#if WITHELKEY


// P 2 + 4 + 4 + 2 + 2
// A 2 + 4 + 2
// R 2 + 4 + 2 + 2
// I 2 + 2 + 2
// S 2 + 2 + 2 + 2
// space: 4 (между словами семь интервалов)
// total: 30 + 16 = 46 dits
// 60000 / 46 = 1304
//
#define PARIS_NUMDOTS 50	//46	// количество одноточечных интервалов при передаче слова PARIS и паузы между словами
//#define WPMSCALE (60000U / PARIS_NUMDOTS)	// длительность одной точки в мс при передаче одного слова PARIS за минуту.

// 46 * wpm / 60 - частота в герцах следования перепадов состояния линии выхода манипуляции с точками

// одна точка формируется десятью (ELKEY_DISCRETE) периодами прерываний
// 20 Hz = 1 точка и 1 пауза за 1 секунду

static uint_fast8_t glob_wpm;

/* обработка меню - установить скорость */
void board_set_wpm(
	uint_fast8_t wpm
	)
{
	if (glob_wpm != wpm)
	{
		glob_wpm = wpm;
		// Переход от WPM к частоте прерываний таймера
		// константа 60 в вычислениях - это 60 секунд.
		const uint_fast32_t ticksfreq = (uint_fast32_t) PARIS_NUMDOTS * ELKEY_DISCRETE * wpm / 60;

		hardware_elkey_set_speed(ticksfreq);
	}
}

#endif /* WITHELKEY */


#if 0
// 
void hardware_cw_diagnostics_noirq(
	uint_fast8_t c1,
	uint_fast8_t c2,
	uint_fast8_t c3)
{
	enum { DIT = 100, DASH = DIT * 3, PAUSE = DIT * 1, PAUSE3 = DIT * 3 };

	//system_disableIRQ();

	board_beep_enable(1);
	if (c1) local_delay_ms(DASH); else local_delay_ms(DIT);
	board_beep_enable(0);

	local_delay_ms(PAUSE);

	board_beep_enable(1);
	if (c2) local_delay_ms(DASH); else local_delay_ms(DIT);
	board_beep_enable(0);

	local_delay_ms(PAUSE);

	board_beep_enable(1);
	if (c3) local_delay_ms(DASH); else local_delay_ms(DIT);
	board_beep_enable(0);

	local_delay_ms(PAUSE3);

	//system_enableIRQ();
}

void hardware_cw_diagnostics(
	uint_fast8_t c1,
	uint_fast8_t c2,
	uint_fast8_t c3)
{
	system_disableIRQ();

	hardware_cw_diagnostics_noirq(c1, c2, c3);

	system_enableIRQ();
}


#endif

#if WITHDEBUG && WITHUSBCDC && WITHDEBUG_CDC


// Очереди символов для обмена с host
enum { qSZdevice = 8192 };

static uint8_t debugusb_queue [qSZdevice];
static unsigned debugusb_qp, debugusb_qg;

// Передать символ в host
static uint_fast8_t	debugusb_qput(uint_fast8_t c)
{
	unsigned qpt = debugusb_qp;
	const unsigned next = (qpt + 1) % qSZdevice;
	if (next != debugusb_qg)
	{
		debugusb_queue [qpt] = c;
		debugusb_qp = next;
		return 1;
	}
	return 0;
}

// Получить символ в host
static uint_fast8_t debugusb_qget(uint_fast8_t * pc)
{
	if (debugusb_qp != debugusb_qg)
	{
		* pc = debugusb_queue [debugusb_qg];
		debugusb_qg = (debugusb_qg + 1) % qSZdevice;
		return 1;
	}
	return 0;
}

// получить состояние очереди передачи
static uint_fast8_t debugusb_qempty(void)
{
	return debugusb_qp == debugusb_qg;
}

enum { qSZhost = 32 };

static uint8_t debugusb_ci_queue [qSZhost];
static unsigned debugusb_ci_qp, debugusb_ci_qg;

// Передать символ в device
static uint_fast8_t	debugusb_ci_qput(uint_fast8_t c)
{
	unsigned qpt = debugusb_ci_qp;
	const unsigned next = (qpt + 1) % qSZhost;
	if (next != debugusb_ci_qg)
	{
		debugusb_ci_queue [qpt] = c;
		debugusb_ci_qp = next;
		return 1;
	}
	return 0;
}

// Получить символ в host
static uint_fast8_t debugusb_ci_qget(uint_fast8_t * pc)
{
	if (debugusb_ci_qp != debugusb_ci_qg)
	{
		* pc = debugusb_ci_queue [debugusb_ci_qg];
		debugusb_ci_qg = (debugusb_ci_qg + 1) % qSZhost;
		return 1;
	}
	return 0;
}

// получить состояние очереди передачи
static uint_fast8_t debugusb_ci_qempty(void)
{
	return debugusb_ci_qp == debugusb_ci_qg;
}

uint_fast8_t debugusb_putchar(uint_fast8_t c)/* передача символа если готов порт */
{
	system_disableIRQ();
	const uint_fast8_t f = debugusb_qput(c);
	if (f)
		HARDWARE_DEBUG_ENABLETX(1);
	system_enableIRQ();
	return f;
}

uint_fast8_t debugusb_getchar(char * cp) /* приём символа, если готов порт */
{
	uint_fast8_t c;
	system_disableIRQ();
	const uint_fast8_t f = debugusb_ci_qget(& c);
	system_enableIRQ();
	if (f)
		* cp = c;
	return f;
}

void debugusb_parsechar(uint_fast8_t c)				/* вызывается из обработчика прерываний */
{
	debugusb_ci_qput(c);
}

void debugusb_sendchar(void * ctx)							/* вызывается из обработчика прерываний */
{
	uint_fast8_t c;
	if (debugusb_qget(& c))
	{
		HARDWARE_DEBUG_TX(ctx, c);
		if (debugusb_qempty())
			HARDWARE_DEBUG_ENABLETX(0);
	}
	else
	{
		HARDWARE_DEBUG_ENABLETX(0);
	}
}

// Вызывается из user-mode программы при запрещённых прерываниях.
void debugusb_initialize(void)
{
}

#endif /* WITHDEBUG && WITHUSBCDC && WITHDEBUG_CDC */

// Read ADC MCP3204/MCP3208
uint_fast16_t
mcp3208_read(
	spitarget_t target,
	uint_fast8_t diff,
	uint_fast8_t adci,
	uint_fast8_t * valid
	)
{
	uint_fast16_t v0, v1, v2, v3;
	// сдвинуто, чтобы позиция временной диаграммы,
	// где формируется время выборки, не попадала на паузу между байтами.
	const uint_fast8_t cmd1 = 0x10 | (diff ? 0x00 : 0x08) | (adci & 0x07);
	uint_fast32_t rv;

	enum { LSBPOS = 0 };
	const spi_speeds_t adcspeed = SPIC_SPEED400k;
	const spi_modes_t adcmode = SPIC_MODE3;

#if WITHSPI32BIT

	hardware_spi_connect_b32(adcspeed, adcmode);
	prog_select(target);

	hardware_spi_b32_p1((uint_fast32_t) cmd1 << (LSBPOS + 14));
	rv = hardware_spi_complete_b32();

	prog_unselect(target);
	hardware_spi_disconnect();


#elif WITHSPI16BIT

	hardware_spi_connect_b16(adcspeed, adcmode);
	prog_select(target);

	hardware_spi_b16_p1((uint_fast32_t) cmd1 << (LSBPOS + 14) >> 16);
	v0 = hardware_spi_complete_b16();
	hardware_spi_b16_p1(0);
	v1 = hardware_spi_complete_b16();

	prog_unselect(target);
	hardware_spi_disconnect();

	rv = ((uint_fast32_t) v0 << 16) | v1;

#else

	spi_select2(target, adcmode, adcspeed);	// for 50 kS/S and 24 bit words

	v0 = spi_read_byte(target, (uint_fast32_t) cmd1 << (LSBPOS + 14) >> 24);
	v1 = spi_read_byte(target, (uint_fast32_t) cmd1 << (LSBPOS + 14) >> 16);
	v2 = spi_read_byte(target, (uint_fast32_t) cmd1 << (LSBPOS + 14) >> 8);
	v3 = spi_read_byte(target, 0x00);

	spi_unselect(target);

	rv = ((uint_fast32_t) v0 << 24) | ((uint_fast32_t) v1 << 16) | ((uint_fast32_t) v2 << 8) | v3;

#endif

	* valid = ((rv >> (LSBPOS + 12)) & 0x01) == 0;
	return (rv >> LSBPOS) & 0xFFF;
}

