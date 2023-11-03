/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED

#include "hardware.h"
#include "dspdefines.h"

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if WITHUSEDUALWATCH
	#define NTRX 2	/* количество трактов приемника. */
#else /* WITHUSEDUALWATCH */
	#define NTRX 1	/* количество трактов приемника. */
#endif /* WITHUSEDUALWATCH */


enum
{
	BOARD_WTYPE_BLACKMAN_HARRIS,
	BOARD_WTYPE_BLACKMAN_HARRIS_MOD,
	BOARD_WTYPE_BLACKMAN_HARRIS_3TERM,
	BOARD_WTYPE_BLACKMAN_HARRIS_3TERM_MOD,
	BOARD_WTYPE_BLACKMAN_HARRIS_4TERM,
	BOARD_WTYPE_BLACKMAN_HARRIS_7TERM,
	BOARD_WTYPE_BLACKMAN_NUTTALL,
	BOARD_WTYPE_NUTTALL,		// Nuttall window, continuous first derivative
	BOARD_WTYPE_HAMMING,
	BOARD_WTYPE_HANN,
	BOARD_WTYPE_RECTANGULAR,
	//
	BOARD_WTYPE_count
};

#define BOARD_WTYPE_FILTERS BOARD_WTYPE_BLACKMAN_HARRIS_4TERM
#define BOARD_WTYPE_SPECTRUM BOARD_WTYPE_NUTTALL	// такой же тип окна испольуется по умолчанию в HDSDR

FLOAT_t fir_design_window(int iCnt, int iCoefNum, int wtype); // Calculate window function (blackman-harris, hamming, rectangular)

// Ограничение алгоритма генерации параметров фильтра - нечётное значение Ntap.
// Кроме того, для функций фильтрации с использованием симметрии коэффициентов, требуется кратность 2 половины Ntap

#define NtapValidate(n)	((unsigned) (n) / 8 * 8 + 1)	/* Гарантируется пригодность для симметричного фильтра */
#define NtapCoeffs(n)	((unsigned) (n) / 2 + 1)

#if WITHDSPLOCALFIR || WITHDSPLOCALTXFIR

	/* Фильтрация квадратур осуществляется процессором */

	#if CPUSTYLE_R7S721
		#define Ntap_rx_SSB_IQ	NtapValidate(241)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(241)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation
		#define	Ntap_rx_AUDIO	NtapValidate(241)

	#elif CPUSTYLE_STM32MP1 || CPUSTYLE_XC7Z || (CPUSTYLE_T113 && WITHDSPLOCALFIR)
		#define Ntap_rx_SSB_IQ	NtapValidate(241)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(241)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(241)	// single samples, floating point implementation
		#define	Ntap_rx_AUDIO	NtapValidate(241)

	#elif CPUSTYLE_STM32F7XX
		#define Ntap_rx_SSB_IQ	NtapValidate(241)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(241)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation
		#define	Ntap_rx_AUDIO	NtapValidate(241)

	#else
		#define Ntap_rx_SSB_IQ	NtapValidate(181)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(181)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation
		#define	Ntap_rx_AUDIO	NtapValidate(241)

	#endif

#else /* WITHDSPLOCALFIR */

	/* Фильтрация квадратур осуществляется FPGA */

	#if CPUSTYLE_STM32MP1 || CPUSTYLE_T113 || CPUSTYLE_F133 || CPUSTYLE_XC7Z
		#define	Ntap_rx_AUDIO	NtapValidate(1023)
		#define Ntap_tx_MIKE	NtapValidate(511)

	#else /* CPUSTYLE_STM32MP1 */
		#define	Ntap_rx_AUDIO	NtapValidate(511)
		#define Ntap_tx_MIKE	NtapValidate(511)

	#endif /* CPUSTYLE_STM32MP1 */

#endif /* WITHDSPLOCALFIR */

#if WITHUSBMIKET113 && WITHCODEC1_WHBLOCK_DUPLEX_MASTER

	/* работа со встроенным кодеком Allwinner t113-s3 */
	/* параметры входного/выходного адаптеров */
	#define WITHADAPTERCODEC1WIDTH	20		// 1 бит знак и 19 бит значащих
	#define WITHADAPTERCODEC1SHIFT	12		// количество незанятых битов справа.
	typedef int32_t aubufv_t;
	typedef int_fast32_t aufastbufv_t;
	typedef int_fast64_t aufastbufv2x_t;	/* тип для работы ресэмплера при получении среднего арифметического */

#elif CODEC1_FRAMEBITS == 64

	/* параметры входного/выходного адаптеров */
	#define WITHADAPTERCODEC1WIDTH	24		// 1 бит знак и 23 бит значащих
	#define WITHADAPTERCODEC1SHIFT	8		// количество незанятых битов справа.
	typedef int32_t aubufv_t;
	typedef int_fast32_t aufastbufv_t;
	typedef int_fast64_t aufastbufv2x_t;	/* тип для работы ресэмплера при получении среднего арифметического */

#elif CODEC1_FRAMEBITS == 32

	/* параметры входного/выходного адаптеров */
	#define WITHADAPTERCODEC1WIDTH	16		// 1 бит знак и 15 бит значащих
	#define WITHADAPTERCODEC1SHIFT	0		// количество незанятых битов справа.
	typedef int16_t aubufv_t;
	typedef int_fast16_t aufastbufv_t;
	typedef int_fast32_t aufastbufv2x_t;	/* тип для работы ресэмплера при получении среднего арифметического */

#else /* CODEC1_FRAMEBITS == 64 */

	//#error Unsupported CODEC1_FRAMEBITS value

#endif /* CODEC1_FRAMEBITS == 64 */


#if CPUSTYLE_XC7Z

	/* параметры входного/выходного адаптеров */
	// IF RX
	#define WITHADAPTERIFADCWIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHADAPTERIFADCSHIFT	0		// количество незанятых битов справа.
	// RTS96
	#define WITHADAPTERRTS96_WIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHADAPTERRTS96_SHIFT	0		// количество незанятых битов справа.
	// RTS192
	#define WITHADAPTERRTS192_WIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHADAPTERRTS192_SHIFT	0		// количество незанятых битов справа.
	// IF TX
	#define WITHADAPTERIFDACWIDTH	16		// 1 бит знак и 15 бит значащих
	#define WITHADAPTERIFDACSHIFT	0		// количество незанятых битов справа.
	typedef int32_t IFADCvalue_t;
	typedef int16_t IFDACvalue_t;

#elif CPUSTYLE_T113 && WITHDSPLOCALFIR

	/* параметры входного/выходного адаптеров */
	// IF RX
	#define WITHADAPTERIFADCWIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHADAPTERIFADCSHIFT	0		// количество незанятых битов справа.
	// RTS96
	#define WITHADAPTERRTS96_WIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHADAPTERRTS96_SHIFT	0		// количество незанятых битов справа.
	// RTS192
	#define WITHADAPTERRTS192_WIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHADAPTERRTS192_SHIFT	0		// количество незанятых битов справа.
	// IF TX
	#define WITHADAPTERIFDACWIDTH	32		// 1 бит знак и 15 бит значащих
	#define WITHADAPTERIFDACSHIFT	0		// количество незанятых битов справа.
	typedef int32_t IFADCvalue_t;
	typedef int32_t IFDACvalue_t;

#else /* CPUSTYLE_XC7Z */

	/* параметры входного/выходного адаптеров */
	// IF RX
	#define WITHADAPTERIFADCWIDTH	28		// 1 бит знак и 27 бит значащих
	#define WITHADAPTERIFADCSHIFT	0		// количество незанятых битов справа.
	// RTS96
	#define WITHADAPTERRTS96_WIDTH	28		// 1 бит знак и 27 бит значащих
	#define WITHADAPTERRTS96_SHIFT	0		// количество незанятых битов справа.
	// RTS192
	#define WITHADAPTERRTS192_WIDTH	32		// 1 бит знак и 31 бит значащих
	#define WITHADAPTERRTS192_SHIFT	0		// количество незанятых битов справа.
	// IF TX
	#define WITHADAPTERIFDACWIDTH	28		// 1 бит знак и 27 бит значащих
	#define WITHADAPTERIFDACSHIFT	0		// количество незанятых битов справа.
	typedef int32_t IFADCvalue_t;
	typedef int32_t IFDACvalue_t;

#endif /* CPUSTYLE_XC7Z */

typedef struct adapter_tag
{
	FLOAT_t inputK;
	FLOAT_t outputK;
	FLOAT_t outputKexact;
	int leftbit;
	int rightspace;
	int lshift32;	// input convrtsion
	int rshift32;
	const char * name;
} adapter_t;

typedef struct transform_tag
{
	int lshift32;
	int rshift32;
	int lshift64;
	int rshift64;
} transform_t;

FLOAT_t adpt_input(const adapter_t * adp, int32_t v);
int32_t adpt_output(const adapter_t * adp, FLOAT_t v);
int32_t adpt_outputL(const adapter_t * adp, double v);
int32_t adpt_outputexact(const adapter_t * adp, FLOAT_t v);	// точное преобразование между внешними целочисленными представлениями.
int32_t adpt_outputexactL(const adapter_t * adp, double v);	// точное преобразование между внешними целочисленными представлениями.
void adpt_initialize(adapter_t * adp, int leftbit, int rightspace, const char * name);
int32_t transform_do32(const transform_t * tfm, int32_t v); // точное преобразование между внешними целочисленными представлениями.
int64_t transform_do64(const transform_t * tfm, int64_t v); // точное преобразование между внешними целочисленными представлениями.
void transform_initialize(transform_t * tfm, const adapter_t * informat, const adapter_t * outformat);

extern adapter_t afcodecrx;	/* от микрофона */
extern adapter_t afcodectx;	/* к наушникам */

//extern adapter_t ifcodecrx;	/* канал от FPGA к процессору */
//extern adapter_t ifcodectx;	/* канал от процессора к FPGA */
extern adapter_t ifspectrumin96;	/* канал от FPGA к процессору */
extern adapter_t ifspectrumin192;	/* канал от FPGA к процессору */

extern adapter_t rts96in;	/* Аудиоданные (спектр) в компютер из трансивера */
extern adapter_t rts192in;	/* Аудиоданные (спектр) в компютер из трансивера */
extern adapter_t sdcardio;
extern transform_t if2rts96out;	// преобразование из выхода панорамы FPGA в формат UAB AUDIO RTS
extern transform_t if2rts192out;	// преобразование из выхода панорамы FPGA в формат UAB AUDIO RTS

unsigned audiorec_getwidth(void);

// DUCDDC_FREQ = REFERENCE_FREQ * DDS1_CLK_MUL
#if WITHDSPEXTFIR || WITHDSPEXTDDC
	#if CPUSTYLE_XC7Z && DIRECT_122M88_X1
		// Параметры фильтров в случае использования FPGA с фильтром на квадратурных каналах
		//#define Ntap_trxi_IQ		1535	// Фильтр в FPGA (1024+512-1)
		#define Ntap_trxi_IQ		1023	// Фильтр в FPGA
		#define HARDWARE_COEFWIDTH	24		// Разрядность коэффициентов. format is S0.23
		// калибровка делается при использовании параметра WITHTXCPATHCALIBRATE
		//#define HARDWARE_DACSCALE	(0.88)	// stages=8, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора
		#define HARDWARE_DACSCALE	(0.71)	// stages=9, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора

		#define FPGADECIMATION 2560uL	// должно быть кратно 256

		#define ARMI2SMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))	// 48 kHz
		#define ARMSAIMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))	// 48 kHz

	#elif CPUSTYLE_XC7Z && DIRECT_61M440_X1
		// Параметры фильтров в случае использования FPGA с фильтром на квадратурных каналах
		//#define Ntap_trxi_IQ		1535	// Фильтр в FPGA (1024+512-1)
		#define Ntap_trxi_IQ		1023	// Фильтр в FPGA
		#define HARDWARE_COEFWIDTH	24		// Разрядность коэффициентов. format is S0.23
		// калибровка делается при использовании параметра WITHTXCPATHCALIBRATE
		//#define HARDWARE_DACSCALE	(0.88)	// stages=8, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора
		#define HARDWARE_DACSCALE	(0.71)	// stages=9, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора

		#define FPGADECIMATION 1280uL	// должно быть кратно 256

		#define ARMI2SMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))	// 48 kHz
		#define ARMSAIMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))	// 48 kHz

	#elif CPUSTYLE_XC7Z && DIRECT_96M_X1
		// Параметры фильтров в случае использования FPGA с фильтром на квадратурных каналах
		//#define Ntap_trxi_IQ		1535	// Фильтр в FPGA (1024+512-1)
		#define Ntap_trxi_IQ		1023	// Фильтр в FPGA
		#define HARDWARE_COEFWIDTH	24		// Разрядность коэффициентов. format is S0.23
		// калибровка делается при использовании параметра WITHTXCPATHCALIBRATE
		//#define HARDWARE_DACSCALE	(0.88)	// stages=8, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора
		#define HARDWARE_DACSCALE	(0.71)	// stages=9, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора

		#define FPGADECIMATION 2000uL	// должно быть кратно 256

		#define ARMI2SMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))	// 48 kHz
		#define ARMSAIMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))	// 48 kHz

	#elif CPUSTYLE_XC7Z
		// Параметры фильтров в случае использования FPGA с фильтром на квадратурных каналах
		//#define Ntap_trxi_IQ		1535	// Фильтр в FPGA (1024+512-1)
		#define Ntap_trxi_IQ		1023	// Фильтр в FPGA
		#define HARDWARE_COEFWIDTH	24		// Разрядность коэффициентов. format is S0.23
		// калибровка делается при использовании параметра WITHTXCPATHCALIBRATE
		//#define HARDWARE_DACSCALE	(0.88)	// stages=8, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора
		#define HARDWARE_DACSCALE	(0.71)	// stages=9, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора

		#define FPGADECIMATION 1024uL	// должно быть кратно 256

		#define ARMI2SMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))	// 48 kHz
		#define ARMSAIMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))	// 48 kHz

	#else /* CPUSTYLE_XC7Z */
		// Параметры фильтров в случае использования FPGA с фильтром на квадратурных каналах
		//#define Ntap_trxi_IQ		1535	// Фильтр в FPGA (1024+512-1)
		#define Ntap_trxi_IQ		1023	// Фильтр в FPGA
		#define HARDWARE_COEFWIDTH	24		// Разрядность коэффициентов. format is S0.23
		// калибровка делается при использовании параметра WITHTXCPATHCALIBRATE
		//#define HARDWARE_DACSCALE	(0.88)	// stages=8, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора
		#define HARDWARE_DACSCALE	(0.71)	// stages=9, на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора

		#define FPGADECIMATION 2560uL
		#define FPGADIVIDERATIO 5uL
		#define EXTI2S_FREQ (DUCDDC_FREQ / FPGADIVIDERATIO)
		#define EXTSAI_FREQ (DUCDDC_FREQ / FPGADIVIDERATIO)

		#define ARMI2SMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))
		#define ARMSAIMCLK	(DUCDDC_FREQ / (FPGADECIMATION / 256))

	#endif /* CPUSTYLE_XC7Z */
#else
	// калибровка делается при использовании параметра WITHTXCPATHCALIBRATE
	#define HARDWARE_DACSCALE	(1)	// на сколько уменьшаем от возможного выходной код для предотвращения переполнения выходлного сумматора

#endif /* WITHDSPEXTFIR || WITHDSPEXTDDC */

#if WITHDSPEXTFIR && WITHI2SCLOCKFROMPIN
	#define ARMI2SMCLKX(scale)	(DUCDDC_FREQ * (uint_fast64_t) (scale) / FPGADECIMATION)
#else /* WITHDSPEXTFIR && WITHI2SCLOCKFROMPIN */
	#define ARMI2SMCLKX(scale)	(ARMSAIMCLK * (uint_fast64_t) (scale) / 256)
#endif /* WITHDSPEXTFIR && WITHI2SCLOCKFROMPIN */

#define ARMSAIRATE		(ARMSAIMCLK / 256)	// SAI sample rate (FPGA/IF CODEC side)

#if WITHDTMFPROCESSING

	#define ARMI2SRATE			((int32_t) 8000)	// I2S sample rate audio codec (human side)
	#define ARMI2SRATEX(scale)	((int32_t) (ARMI2SRATE * (scale)))	// I2S sample rate audio codec (human side)
	#define ARMI2SRATE100		((int32_t) ARMI2SRATEX(100))

#else /* WITHDTMFPROCESSING */

	#define ARMI2SRATE			((int32_t) (ARMI2SMCLK / 256))	// I2S sample rate audio codec (human side)
	#define ARMI2SRATEX(scale)	((int32_t) (ARMI2SMCLKX(scale)))	// I2S sample rate audio codec (human side)
	#define ARMI2SRATE100		((int32_t) (ARMI2SRATEX(100)))

#endif /* WITHDTMFPROCESSING */

uint_fast8_t modem_getnextbit(
	uint_fast8_t suspend	// передавать модему ещё рано - не полностью завершено формирование огибающей
	 );
// обработка бита на приёме
void
modem_frames_decode(
	uint_fast8_t v
	);

uint_fast8_t getsampmlemike(FLOAT32P_t * v);			/* получить очередной оцифрованый сэмпл с микрофона */
uint_fast8_t getsampmleusb(FLOAT32P_t * v);				/* получить очередной оцифрованый сэмпл с USB UAC OUT после ресэмплигнга */

//#endif /* WITHINTEGRATEDDSP */


FLOAT_t get_lout(void); // тестовые функции
FLOAT_t get_rout(void);

void prog_dsplreg(void);
void prog_fltlreg(void);
void prog_codec1reg(void);
void prog_codecreg_update(void);		// услолвное обновление регистров аудио кодека
void prog_dsplreg_update(void);
void prog_fltlreg_update(void);
void board_dsp1regchanged(void);
void prog_codec1reg(void);

void board_set_trxpath(uint_fast8_t v);	/* Тракт, к которому относятся все последующие вызовы. При перередаяе используется индекс 0 */
void board_set_mikemute(uint_fast8_t v);	/* отключить микрофонный усилитель */
void board_set_mik1level(uint_fast16_t v);	/* усиление микрофонного усилителя */
void board_set_agcrate(uint_fast8_t v);	/* на n децибел изменения входного сигнала 1 дБ выходного. UINT8_MAX - "плоская" АРУ */
void board_set_agc_t0(uint_fast8_t v);	/* подстройка параметра АРУ */
void board_set_agc_t1(uint_fast8_t v);	/* подстройка параметра АРУ */
void board_set_agc_t2(uint_fast8_t v);	/* подстройка параметра АРУ */
void board_set_agc_t4(uint_fast8_t v);	/* подстройка параметра АРУ */
void board_set_agc_thung(uint_fast8_t v);	/* подстройка параметра АРУ */
void board_set_agc_scale(uint_fast8_t v);	/* подстройка параметра АРУ */
void board_set_squelch(uint_fast8_t v);	/* уровень открывания шумоподавителя */
void board_set_notch_freq(uint_fast16_t n);	/* частота NOTCH фильтра */
void board_set_notch_width(uint_fast16_t n);	/* полоса NOTCH фильтра */
void board_set_notch_mode(uint_fast8_t n);	/* включение NOTCH фильтра */
void board_set_cwedgetime(uint_fast8_t n);	/* Время нарастания/спада огибающей телеграфа при передаче - в 1 мс */
void board_set_sidetonelevel(uint_fast8_t n);	/* Уровень сигнала самоконтроля в процентах - 0%..100% */
void board_set_moniflag(uint_fast8_t n);	/* разрешение самопрослушивания */
void board_set_cwssbtx(uint_fast8_t v);	/* разрешение передачи телеграфа как тона в режиме SSB */
void board_set_subtonelevel(uint_fast8_t n);	/* Уровень сигнала CTCSS в процентах - 0%..100% */
void board_set_amdepth(uint_fast8_t n);		/* Глубина модуляции в АМ - 0..100% */
void board_set_swaprts(uint_fast8_t v);	/* если используется конвертор на Rafael Micro R820T - требуется инверсия спектра */
void board_set_lo6(int_fast32_t f);
void board_set_fullbw6(int_fast16_t f);	/* Установка частоты среза фильтров ПЧ в алгоритме Уивера - параметр полная полоса пропускания */

void board_set_aflowcutrx(int_fast16_t v);		/* Нижняя частота среза фильтра НЧ */
void board_set_afhighcutrx(int_fast16_t v);	/* Верхняя частота среза фильтра НЧ */
void board_set_aflowcuttx(int_fast16_t v);		/* Нижняя частота среза фильтра НЧ */
void board_set_afhighcuttx(int_fast16_t v);	/* Верхняя частота среза фильтра НЧ */
void board_set_nfmdeviation100(uint_fast8_t v);	/* Девиация в NFM (сотни герц) */

void board_set_afgain(uint_fast16_t v);	// Параметр для регулировки уровня на выходе аудио-ЦАП
void board_set_ifgain(uint_fast16_t v);	// Параметр для регулировки усиления ПЧ/ВЧ
void board_set_dspmode(uint_fast8_t v);	// Параметр для установки режима работы приёмника A/передатчика A
void board_set_lineinput(uint_fast8_t n);	// Включение line input вместо микрофона
void board_set_lineamp(uint_fast16_t v);	// Параметр для регулировки уровня на входе аудио-ЦАП при работе с LINE IN
void board_set_txaudio(uint_fast8_t v);	// Альтернативные источники сигнала при передаче
void board_set_mikeboost20db(uint_fast8_t n);	// Включение предусилителя за микрофоном
void board_set_afmute(uint_fast8_t n);	// Отключение звука
void board_set_mikeequal(uint_fast8_t n);	// включение обработки сигнала с микрофона (эффекты, эквалайзер, ...)
void board_set_mikeequalparams(const uint_fast8_t * p);	// Эквалайзер 80Hz 230Hz 650Hz 	1.8kHz 5.3kHz
void board_set_mikeagc(uint_fast8_t n);		/* Включение программной АРУ перед модулятором */
void board_set_mikeagcgain(uint_fast8_t v);	/* Максимальное усидение АРУ микрофона */
void board_set_afresponcerx(int_fast8_t v);	/* изменение тембра звука в канале приемника */
void board_set_afresponcetx(int_fast8_t v);	/* изменение тембра звука в канале передатчика */
void board_set_mikehclip(uint_fast8_t gmikehclip);	/* Ограничитель */
void board_set_reverb(uint_fast8_t greverb, uint_fast8_t greverbdelay, uint_fast8_t greverbloss); /* ревербератор */
void board_set_compressor(uint_fast8_t attack, uint_fast8_t release, uint_fast8_t hold, uint_fast8_t gain, uint_fast8_t threshold);


void board_set_uacplayer(uint_fast8_t v);	/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
void board_set_datatx(uint_fast8_t v);	/* автоматическое изменение источника при появлении звука со стороны компьютера */
void board_set_usb_ft8cn(uint_fast8_t v);	/* совместимость VID/PID для работы с программой FT8CN */

void dsp_initialize(void);

#if WITHINTEGRATEDDSP

	// Нормирование уровня сигнала к шкале
	// возвращает значения от 0 до ymax включительно
	// 0 - минимальный сигнал, ymax - максимальный
	int dsp_mag2y(FLOAT_t mag, int ymax, int_fast16_t topdb, int_fast16_t bottomdb);

#endif /* WITHINTEGRATEDDSP */

int_fast32_t dsp_get_ifreq(void);		/* Получить значение входной ПЧ для обработки DSP */
int_fast32_t dsp_get_sampleraterx(void);	/* Получить значение частоты выборок выходного потока DSP */
int_fast32_t dsp_get_sampleraterxscaled(uint_fast8_t scale);	/* Получить значение частоты выборок выходного потока DSP */
int_fast32_t dsp_get_sampleratetx(void);	/* Получить значение частоты выборок входного потока DSP */
int_fast32_t dsp_get_samplerate100(void);	/* Получить значение частоты выборок выходного потока DSP */

int_fast32_t dsp_get_samplerateuacin_audio48(void);		// UAC IN samplerate
int_fast32_t dsp_get_samplerateuacin_RTS96(void);		// UAC IN samplerate
int_fast32_t dsp_get_samplerateuacin_RTS192(void);		// UAC IN samplerate
int_fast32_t dsp_get_samplerateuacin_rts(void);			// RTS samplerate
int_fast32_t dsp_get_samplerateuacout(void);			// UAC OUT samplerate

uint_fast8_t dsp_getsmeter(uint_fast8_t * tracemax, uint_fast8_t lower, uint_fast8_t upper, uint_fast8_t clean);	/* получить значение от АЦП s-метра */
uint_fast16_t dsp_getsmeter10(uint_fast16_t * tracemax, uint_fast16_t lower, uint_fast16_t upper, uint_fast8_t clean);	/* получить значение от АЦП s-метра */
uint_fast8_t dsp_getvox(uint_fast8_t fullscale);	/* получить значение от детектора VOX */
uint_fast8_t dsp_getavox(uint_fast8_t fullscale);	/* получить значение от детектора Anti-VOX */
uint_fast8_t dsp_getfreqdelta10(int_fast32_t * p, uint_fast8_t pathi);	/* Получить значение отклонения частоты с точностью 0.1 герца */
uint_fast8_t dsp_getmikeadcoverflow(void); /* получения признака переполнения АЦП микрофонного тракта */

void dsp_speed_diagnostics(void);	/* DSP speed test */
void beginstamp(void);
void endstamp(void);
void beginstamp2(void);
void endstamp2(void);
void beginstamp3(void);
void endstamp3(void);

void buffers_diagnostics(void);
void dtmftest(void);
void dsp_recalceq_coeffs_rx_AUDIO(uint_fast8_t pathi, FLOAT_t * dCoeff);	// calculate full array of coefficients

void elfill_dmabufferuacin48(FLOAT_t ch0, FLOAT_t ch1);
void elfill_dmabuffer32tx(int_fast32_t ch0, int_fast32_t ch1);
void elfill_dmabuffer16txphones(FLOAT_t ch0, FLOAT_t ch1);
void elfill_recordswav48(FLOAT_t ch0, FLOAT_t ch1);

uint_fast8_t elfetch_dmabufferuacout48(FLOAT_t * dest);
uint_fast8_t elfetch_dmabuffer16rx(FLOAT_t * dest);
//uint_fast8_t elfetch_dmabuffer16moni(FLOAT_t * dest);


/* Сервис очереди сэмплов */
typedef struct { int dummy; } VOICE_t;

void voice_put(VOICE_t * p, FLOAT_t ch0, FLOAT_t ch1);
uint_fast8_t voice_get(VOICE_t * p, FLOAT32P_t * v);

VOICE_t * voice_moni16(void);
VOICE_t * voice_rec16(void);
//VOICE_t * voice_uacin48(void);
//VOICE_t * voice_uacout48(void);
//VOICE_t * voice_16txphones(void);
//VOICE_t * voice_swav48(void);

#define VOICE_MONI16 (voice_moni16())	// формирование звуового потока с сигналом самоконтроля
#define VOICE_REC16 (voice_rec16())		// аудиоданные - выход приемника
//#define VOICE_UACIN48 (voice_uacin48())	// поток с USB фудио устройства
//#define VOICE_UACOUT48 (voice_uacin48())	// поток на USB фудио устройство
//#define VOICE_TXPHONES (voice_16txphones())	// формирование звуового потока с демодулированным сигналом
//#define VOICE_WAV48 (voice_swav48())		// поток для записи на накопитель

void modem_initialze(void);
uint_fast8_t modem_get_ptt(void);

/* Интерфейс к AF кодеку */
typedef struct codec1if_tag
{
	uint_fast8_t (* clocksneed)(void);	/* требуется ли подача тактирования для инициадизации кодека */
	void (* stop) (void);
	void (* initialize) (void (* io_control)(uint_fast8_t on), uint_fast8_t master);
	void (* setvolume) (uint_fast16_t gain, uint_fast8_t mute, uint_fast8_t mutespk);	/* Установка громкости на наушники */
	void (* setlineinput) (uint_fast8_t linein, uint_fast8_t mikeboost20db, uint_fast16_t mikegain, uint_fast16_t linegain);	/* Выбор LINE IN как источника для АЦП вместо микрофона */
	void (* setprocparams) (uint_fast8_t procenable, const uint_fast8_t * gains);	/* параметры обработки звука с микрофона (эхо, эквалайзер, ...) */
	const char * label;									/* Название кодека (всегда последний элемент в структуре) */
} codec1if_t;

const codec1if_t * board_getaudiocodecif(void);		// получить интерфейс управления кодеком в сторону оператора

/* Интерфейс к IF кодеку */
typedef struct codec2if_tag
{
	uint_fast8_t (* clocksneed)(void);	/* требуется ли подача тактирования для инициадизации кодека */
	void (* initialize) (void (* io_control)(uint_fast8_t on), uint_fast8_t master);
	const char * label;
} codec2if_t;

const codec2if_t * board_getfpgacodecif(void);		// получить интерфейс управления кодеком для получения данных о радиосигнале

/* загрузка коэффициентов FIR фильтра в FPGA */
void board_fpga_fir_initialize(void);
void board_reload_fir(uint_fast8_t ifir, const int32_t * const k, const FLOAT_t * const kf, unsigned Ntap, unsigned CWidth); /* Выдача рассчитанных параметров фильтра в FPGA (симметричные) */

/* Получение пары (левый и правый) сжмплов для воспроизведения через аудиовыход трансивера.
 * Возврат 0, если нет ничего для воспроизведения.
 */
uint_fast8_t takewavsample(FLOAT32P_t * rv, uint_fast8_t suspend);
void recordsampleSD(FLOAT_t left, FLOAT_t right);
void recordsampleUAC(FLOAT_t left, FLOAT_t right);
void savemonistereo(FLOAT_t ch0, FLOAT_t ch1);

#if WITHINTEGRATEDDSP
void dsp_fillphones(unsigned nsamples);			// перед передачей по DMA в аудиокодек
#endif /* WITHINTEGRATEDDSP */

typedef FLOAT_t speexel_t;
uint_fast8_t takespeexready(speexel_t * * dest);
void releasespeexbuffer(speexel_t * t);
speexel_t * allocatespeexbuffer(void);
void savespeexbuffer(speexel_t * t);

typedef struct subscribefloat_tag
{
	LIST_ENTRY item;
	void * ctx;
	void (* cb)(void * ctx, FLOAT_t ch0, FLOAT_t ch1);
} subscribefloat_t;

typedef struct subscribefint_tag
{
	LIST_ENTRY item;
	void * ctx;
	void (* cb)(void * ctx, int_fast32_t ch0, int_fast32_t ch1);
} subscribeint32_t;

typedef struct deliverylist_tag
{
	LIST_ENTRY head;
	IRQLSPINLOCK_t listlock;
} deliverylist_t;

/* irqlv - на каком уровне IRQL_t будут вызываться фуекции подписавшихся */
void deliverylist_initialize(deliverylist_t * list, IRQL_t irqlv);

void deliveryfloat(deliverylist_t * head, FLOAT_t ch0, FLOAT_t ch1);
void deliveryfloat_buffer(deliverylist_t * head, const FLOAT_t * ch0, const FLOAT_t * ch1, unsigned n);
void deliveryint(deliverylist_t * head, int_fast32_t ch0, int_fast32_t ch1);

void subscribefloat(deliverylist_t * head, subscribefloat_t * target, void * ctx, void (* pfn)(void * ctx, FLOAT_t ch0, FLOAT_t ch1));
void unsubscribefloat(deliverylist_t * head, subscribefloat_t * target);
void subscribeint32(deliverylist_t * head, subscribeint32_t * target, void * ctx, void (* pfn)(void * ctx, int_fast32_t ch0, int_fast32_t ch1));
void unsubscribeint32(deliverylist_t * head, subscribeint32_t * target);

extern deliverylist_t rtstargetsint;	// выход обработчика DMA приема от FPGA
extern deliverylist_t speexoutfloat;	// выход speex и фильтра
extern deliverylist_t afdemodoutfloat;	// выход приемника


void dsp_extbuffer32rx(const IFADCvalue_t * buff);	// RX
//void dsp_extbuffer32rts(const IFADCvalue_t * buff);	// RX
void dsp_extbuffer32wfm(const IFADCvalue_t * buff);	// RX
void inject_testsignals(IFADCvalue_t * dbuff);
void savedemod_to_AF_proc(FLOAT_t left, FLOAT_t right);	// Сохранение сэмплов с выхода демодулятора

#if WITHAFEQUALIZER

enum {
	AF_EQUALIZER_BANDS = 3,		// число полос
	AF_EQUALIZER_BASE = 8,		// предел регулировки
	AF_EQUALIZER_LOW = 400,		// частота нижней полосы
	AF_EQUALIZER_MID = 1500,	// частота средней полосы
	AF_EQUALIZER_HIGH = 2700	// частота верхней полосы
};

int_fast32_t hamradio_get_af_equalizer_base(void);
int_fast32_t hamradio_get_af_equalizer_gain_rx(uint_fast8_t v);
void hamradio_set_af_equalizer_gain_rx(uint_fast8_t index, uint_fast8_t gain);
void board_set_equalizer_rx(uint_fast8_t n);
void board_set_equalizer_tx(uint_fast8_t n);
void board_set_equalizer_rx_gains(const uint_fast8_t * p);
void board_set_equalizer_tx_gains(const uint_fast8_t * p);
uint_fast8_t hamradio_get_geqrx(void);
void hamradio_set_geqrx(uint_fast8_t v);

void audio_rx_equalizer(float32_t *buffer, uint_fast16_t size);

#endif /* WITHAFEQUALIZER */

#if __STDC__ && ! CPUSTYLE_ATMEGA

#define MAXFLOAT	3.40282347e+38F

#if ! LINUX_SUBSYSTEM
	#define M_LN2		_M_LN2
#endif

#define M_E		2.7182818284590452354
#define M_LOG2E		1.4426950408889634074
#define M_LOG10E	0.43429448190325182765
#define M_LN10		2.30258509299404568402
#define M_PI		3.14159265358979323846
#define M_PI_2		1.57079632679489661923
#define M_PI_4		0.78539816339744830962
#define M_1_PI		0.31830988618379067154
#define M_2_PI		0.63661977236758134308
#define M_2_SQRTPI	1.12837916709551257390
#define M_SQRT2		1.41421356237309504880
#define M_SQRT1_2	0.70710678118654752440

#define M_TWOPI         (M_PI * 2.0)
#define M_3PI_4		2.3561944901923448370E0
#define M_SQRTPI        1.77245385090551602792981
#define M_LN2LO         1.9082149292705877000E-10
#define M_LN2HI         6.9314718036912381649E-1
#define M_SQRT3	1.73205080756887719000
#define M_IVLN10        0.43429448190325182765 /* 1 / log(10) */
#define M_LOG2_E        _M_LN2
#define M_INVLN2        1.4426950408889633870E0  /* 1 / log(2) */

#endif /* __STDC__ && ! CPUSTYLE_ATMEGA */

/* установка параметров приемника, передаваемых чрез I2S канал в FPGA */
uint_fast32_t dspfpga_get_nco1(void);
uint_fast32_t dspfpga_get_nco2(void);
uint_fast32_t dspfpga_get_nco3(void);
uint_fast32_t dspfpga_get_nco4(void);
uint_fast32_t dspfpga_get_ncorts(void);

extern volatile phase_t mirror_nco1;
extern volatile phase_t mirror_nco2;
extern volatile phase_t mirror_nco3;
extern volatile phase_t mirror_nco4;
extern volatile phase_t mirror_ncorts;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* AUDIO_H_INCLUDED */
