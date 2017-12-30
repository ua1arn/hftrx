/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "board.h"
#include "audio.h"
#include "spifuncs.h"
#include "formats.h"	// for debug prints
#include "display.h"	// for debug prints

#include "tlv320aic23.h"	// константы управления усилением кодека
#include "nau8822.h"
#include "wm8994.h"

#include <limits.h>
#include <string.h>
#include <math.h>
#include <assert.h>

//#define WITHDYNAMICCOEFFS 1
//#define WITHLIMITEDAGCATTACK 1
#define DUALFILTERSPROCESSING 1	// Фильтры НЧ для левого и правого каналов - вынсено в конфигурационный файл
#define WITHDOUBLEFIRCOEFS 1

#if WITHUSEDUALWATCH && WITHDSPLOCALFIR
	#error Can not support WITHUSEDUALWATCH && WITHDSPLOCALFIR together
#endif /* WITHUSEDUALWATCH && WITHDSPLOCALFIR */

#if 0
	#ifdef __ARM_FP
		#warning Avaliable __ARM_FP
		#if __ARM_FP & 0x02
			#warning Avaliable __ARM_FP - half
		#endif
		#if __ARM_FP & 0x04
			#warning Avaliable __ARM_FP - single
		#endif
		#if __ARM_FP & 0x08
			#warning Avaliable __ARM_FP - double
		#endif
		#ifdef __ARM_FEATURE_FMA
			#warning Avaliable __ARM_FEATURE_FMA
		#endif
	#endif
	#ifdef __ARM_FP_FAST
		#warning __ARM_FP_FAST
	#endif
	#ifdef __ARM_FEATURE_UNALIGNED
		#warning __ARM_FEATURE_UNALIGNED
	#endif
	#ifdef __ARM_FEATURE_DSP
		#warning Avaliable__ARM_FEATURE_DSP
	#endif
	#ifdef __ARM_FP_FAST
		#warning Avaliable __ARM_FP_FAST
	#endif
	#ifdef __ARM_NEON
		#warning Avaliable __ARM_NEON
	#endif
	#ifdef __ARM_NEON_FP
		#warning Avaliable __ARM_NEON_FP
	#endif
	#ifdef FP_FAST_FMA
		#warning Avaliable FP_FAST_FMA
	#endif
	#ifdef FP_FAST_FMAF
		#warning Avaliable FP_FAST_FMAF
	#endif
	#ifdef __ARM_ACLE
		#warning Avaliable __ARM_ACLE
	#endif
#endif

#ifdef __ARM_ACLE
	#include <arm_acle.h>
#endif /* __ARM_ACLE */

#if __ARM_NEON
	#include <arm_neon.h>
#endif /* __ARM_NEON */

// ARMSAIRATE - sample rate IF кодека в герцах
#define NSAITICKS(t_ms) ((uint_fast16_t) (((uint_fast32_t) (t_ms) * ARMSAIRATE + 500) / 1000))

///////////////////////////////////////
//

static uint_fast8_t		glob_trxpath = 0;			/* Тракт, к которому относятся все последующие вызовы. При перередаяе используется индекс 0 */
static uint_fast16_t 	glob_rfgain = BOARD_RFGAIN_MIN;
static uint_fast8_t 	glob_dspmodes [2] = { DSPCTL_MODE_IDLE, DSPCTL_MODE_IDLE, };

static uint_fast8_t		glob_agcrate [2] = { 20, 20 }; //10	// 10 дБ изменение входного на 1 дБ выходного
static uint_fast8_t 	glob_agc_t1 [2] = { 95, 95 };
static uint_fast8_t 	glob_agc_t2 [2] = { 2, 2 };
static uint_fast8_t 	glob_agc_thung [2] = { 3, 3 };	// 0.3 S
static uint_fast8_t 	glob_agc_t4 [2] = { 120, 120 };

static int_fast16_t 	glob_aflowcutrx [2] = { 300, 300 } ;		// Частота низкочастотного среза полосы пропускания (в 10 Гц дискретах)
static int_fast16_t 	glob_afhighcutrx [2] = { 3400, 3400 };	// Частота высокочастотного среза полосы пропускания (в 100 Гц дискретах)

static int_fast16_t 	glob_aflowcuttx = 300 ;		// Частота низкочастотного среза полосы пропускания (в 10 Гц дискретах)
static int_fast16_t 	glob_afhighcuttx = 3400;	// Частота высокочастотного среза полосы пропускания (в 100 Гц дискретах)

static int_fast16_t		glob_fullbw6 [2] = { 1000, 1000 };		/* Частота среза фильтров ПЧ в алгоритме Уивера */
static int_fast32_t		glob_lo6 [2] = { 0, 0 };
//static uint_fast8_t		glob_fltsofter [2] = { WITHFILTSOFTMIN, WITHFILTSOFTMIN }; /* WITHFILTSOFTMIN..WITHFILTSOFTMAX Код управления сглаживанием скатов фильтра основной селекции на приёме */

static uint_fast8_t 	glob_nfm_sql_lelel = 127;
static uint_fast8_t 	glob_nfm_sql_off = 0;

static uint_fast8_t 	glob_swapiq = 0;	// поменять местами I и Q сэмплы в потоке RTS96

// codec-related parameters
static uint_fast16_t 	glob_afgain;
static uint_fast8_t 	glob_afmute;	/* отключить звук в наушниках и динамиках */
static uint_fast8_t 	glob_lineinput;	/* используется line input вместо микрофона */
static uint_fast8_t 	glob_mikebust20db;	/* Включение усилителя 20 дБ за микрофоном */
static uint_fast8_t		glob_mikeagc = 1;	/* Включение программной АРУ перед модулятором */
static uint_fast8_t		glob_mikeagcscale = 100;	/* На какую часть (в процентах) от полной амплитуды настроена АРУ микрофона */
static uint_fast8_t		glob_mikeagcgain = 40;	/* предел усиления в АРУ */
static uint_fast8_t		glob_mikehclip;			/* параметр ограничителя микрофона	*/
#if defined(CODEC1_TYPE)
static uint_fast8_t 	glob_mikeequal;	// Включение обработки сигнала с микрофона (эффекты, эквалайзер, ...)
static uint_fast8_t		glob_codec1_gains [HARDWARE_CODEC1_NPROCPARAMS]; // = { -2, -1, -3, +6, +9 };	// параметры эквалайзера
#endif /* defined(CODEC1_TYPE) */

static uint_fast16_t 	glob_lineamp = WITHLINEINGAINMAX;
static uint_fast16_t	glob_mik1level = WITHMIKEINGAINMAX;
static uint_fast8_t 	glob_txaudio = BOARD_TXAUDIO_MIKE;	// при SSB/AM/FM передача с тестовых источников
static uint_fast8_t		glob_mainsubrxmode = BOARD_RXMAINSUB_A_A;	// Левый/правый, A - main RX, B - sub RX

static uint_fast16_t 	glob_notch_freq = 1000;	/* частота NOTCH фильтра */
static uint_fast16_t	glob_notch_width = 500;	/* полоса NOTCH фильтра */
static uint_fast8_t 	glob_notch_on;		/* включение NOTCH фильтра */

static uint_fast8_t 	glob_cwedgetime = 4;		/* CW Rise Time (in 1 ms discrete) */
static uint_fast8_t 	glob_sidetonelevel = 10;	/* Уровень сигнала самоконтроля в процентах - 0%..100% */
static uint_fast8_t 	glob_subtonelevel = 0;	/* Уровень сигнала CTCSS в процентах - 0%..100% */
static uint_fast8_t 	glob_amdepth = 30;		/* Глубина модуляции в АМ - 0..100% */
static uint_fast8_t		glob_dacscale = 100;	/* На какую часть (в процентах) от полной амплитуды использцется ЦАП передатчика */

static uint_fast8_t 	glob_digigainmax = 96;
static uint_fast8_t		glob_gvad605 = UINT8_MAX;	/* напряжение на AD605 (управление усилением тракта ПЧ */

static int glob_fsadcpower10 = 0;	// мощность, соответствующая full scale от IF ADC с точностью 0.1 дБмВт

static uint_fast8_t		glob_modem_mode;		// применяемая модуляция
static uint_fast32_t	glob_modem_speed100 = 3125;	// скорость передачи с точностью 1/100 бод

static int_fast8_t		glob_afresponcerx;	// изменение тембра звука в канале приемника - на Samplerate/2 АЧХ становится на столько децибел 
static int_fast8_t		glob_afresponcetx;	// изменение тембра звука в канале передатчика - на Samplerate/2 АЧХ становится на столько децибел 

static int_fast8_t		glob_swaprts;		// управление боковой выхода спектроанализатора

#if WITHINTEGRATEDDSP

#define NPROF 2	/* количество профилей параметров DSP фильтров. */

// Определения для работ по оптимизации быстродействия
#if WITHDEBUG && 0


	static volatile uint32_t dtmax = 0, dtlast = 0, dtcount = 0;
	static volatile uint32_t dtmax2 = 0, dtlast2 = 0, dtcount2 = 0;
	static volatile uint32_t dtmax3 = 0, dtlast3 = 0, dtcount3 = 0;
	static uint32_t perft = 0;
	static uint32_t perft2 = 0;
	static uint32_t perft3 = 0;

	static void debug_cleardtmax(void)
	{
		dtmax = 0;
		dtmax2 = 0;
		dtmax3 = 0;
	}

	// получение из аппаратного счетчика монотонно увеличивающегося кода
	static uint32_t debug_getticks(void)
	{
	#if CPUSTYLE_STM32F
			//return SysTick->VAL & SysTick_VAL_CURRENT_Msk;
		return TIM3->CNT;
	#elif CPUSTYLE_R7S721
		return ~ OSTM1.OSTMnCNT;		// таймер считает на уменьшение
	#else
		#warning Wromg CPUSTYLE_xxx
		return 0;
	#endif
	}


	#define BEGIN_STAMP() do { \
			perft = debug_getticks(); \
		} while (0);

	#define END_STAMP() do { \
			const uint32_t t2 = debug_getticks(); \
			if (perft < t2) \
			{ \
				const uint32_t vdt = t2 - perft; \
				dtlast = vdt; /* текущее значение длительности */ \
				if (vdt > dtmax) \
					dtmax = vdt; /* максимальное значение длительности */ \
				++ dtcount; \
			} \
		} while (0);

	#define BEGIN_STAMP2() do { \
			perft2 = debug_getticks(); \
		} while (0);

	#define END_STAMP2() do { \
			const uint32_t t2 = debug_getticks(); \
			if (perft2 < t2) \
			{ \
				const uint32_t vdt = t2 - perft2; \
				dtlast2 = vdt; /* текущее значение длительности */ \
				if (vdt > dtmax2) \
					dtmax2 = vdt; /* максимальное значение длительности */ \
				++ dtcount2; \
			} \
		} while (0);

	#define BEGIN_STAMP3() do { \
			perft3 = debug_getticks(); \
		} while (0);

	#define END_STAMP3() do { \
			const uint32_t t2 = debug_getticks(); \
			if (perft3 < t2) \
			{ \
				const uint32_t vdt = t2 - perft3; \
				dtlast3 = vdt; /* текущее значение длительности */ \
				if (vdt > dtmax3) \
					dtmax3 = vdt; /* максимальное значение длительности */ \
				++ dtcount3; \
			} \
		} while (0);


	/* DSP speed test */
	void dsp_speed_diagnostics(void)
	{
		debug_printf_P(PSTR("dtcount=%u, dtmax=%u, dtlast=%u, "), dtcount, dtmax, dtlast);
		debug_printf_P(PSTR("dtcount2=%u, dtmax2=%u, dtlast2=%u, "), dtcount2, dtmax2, dtlast2);
		debug_printf_P(PSTR("dtcount3=%u, dtmax3=%u, dtlast3=%u\n"), dtcount3, dtmax3, dtlast3);
	}

#else /* WITHDEBUG */

	#define BEGIN_STAMP() do { \
		} while (0);

	#define END_STAMP() do { \
		} while (0);

	#define BEGIN_STAMP2() do { \
		} while (0);

	#define END_STAMP2() do { \
		} while (0);

	#define BEGIN_STAMP3() do { \
		} while (0);

	#define END_STAMP3() do { \
		} while (0);

	static void debug_cleardtmax(void)
	{
	}

	void dsp_speed_diagnostics(void)
	{
	}

#endif /* WITHDEBUG */



// Ограничение алгоритма генератции параметров фильтра - нечётное значение Ntap.
// Кроме того, для функций фильтрации с использованием симметрии коэффициентов, требуется кратность 2 половины Ntap

#define NtapValidate(n)	((unsigned) (n) / 8 * 8 + 1)
#define NtapCoeffs(n)	((unsigned) (n) / 2 + 1)


#if CPUSTYLE_R7S721
	#define FFTSizeFiltersM 10
	#define FFTSizeSpectrumM 10
#else
	#define FFTSizeFiltersM 10
	#define FFTSizeSpectrumM 10
#endif


#if WITHDSPEXTFIR || WITHDSPEXTDDC

	#define HARDWARE_FIRSHIFT	31	// fractional part: format is S0.31

	// Параметры фильтров в случае использования FPGA с фильтром на квадратурных каналах
	#define Ntap_trxi_IQ		1535	// Фильтр в FPGA

	// Фильтр для квадратурных каналов приёмника и передатчика в FPGA (целочисленный).
	// Параметры для передачи в FPGA
	static int_fast32_t FIRCoef_trxi_IQ [NtapCoeffs(Ntap_trxi_IQ)];
	#if WITHDOUBLEFIRCOEFS && (__ARM_FP & 0x08)
		static double FIRCwndL_trxi_IQ [NtapCoeffs(Ntap_trxi_IQ)];			// подготовленные значения функции окна
	#else
		static FLOAT_t FIRCwnd_trxi_IQ [NtapCoeffs(Ntap_trxi_IQ)];			// подготовленные значения функции окна
	#endif

#endif /* WITHDSPEXTFIR || WITHDSPEXTDDC */

#if WITHDSPEXTFIR


	#if CPUSTYLE_R7S721 && ! WITHUSEDUALWATCH
		// Без WITHUSEDUALWATCH
		#define Ntap_rx_AUDIO	NtapValidate(241)
		#define Ntap_tx_MIKE	NtapValidate(481)
	#elif CPUSTYLE_R7S721
		// есть режим WITHUSEDUALWATCH
		#define Ntap_rx_AUDIO	NtapValidate(241)
		#define Ntap_tx_MIKE	NtapValidate(481)
	#elif (defined (STM32F767xx) || defined (STM32F769xx)) && ! WITHUSEDUALWATCH
		// Без WITHUSEDUALWATCH
		#define Ntap_rx_AUDIO	NtapValidate(221)
		#define Ntap_tx_MIKE	NtapValidate(241)
	#elif (defined (STM32F767xx) || defined (STM32F769xx))
		// есть режим WITHUSEDUALWATCH
		#define Ntap_rx_AUDIO	NtapValidate(129)
		#define Ntap_tx_MIKE	NtapValidate(241)
	#elif CPUSTYLE_STM32H7XX && ! WITHUSEDUALWATCH
		// Без WITHUSEDUALWATCH
		#define Ntap_rx_AUDIO	NtapValidate(221)
		#define Ntap_tx_MIKE	NtapValidate(241)
	#elif CPUSTYLE_STM32H7XX
		// есть режим WITHUSEDUALWATCH
		#define Ntap_rx_AUDIO	NtapValidate(129)
		#define Ntap_tx_MIKE	NtapValidate(241)
	#elif CPUSTYLE_STM32F7XX && ! WITHUSEDUALWATCH
		// Без WITHUSEDUALWATCH
		#define Ntap_rx_AUDIO	NtapValidate(221)
		#define Ntap_tx_MIKE	NtapValidate(241)
	#elif CPUSTYLE_STM32F7XX
		// есть режим WITHUSEDUALWATCH
		#define Ntap_rx_AUDIO	NtapValidate(129)
		#define Ntap_tx_MIKE	NtapValidate(241)
	#elif CPUSTYLE_STM32F4XX && ! WITHUSEDUALWATCH
		// Без WITHUSEDUALWATCH (только)
		#define Ntap_rx_AUDIO	NtapValidate(129)
		#define Ntap_tx_MIKE	NtapValidate(129)
	#else
		#error Not suitable CPUSTYLE_xxx and WITHUSEDUALWATCH combination
	#endif

#endif /* WITHDSPEXTFIR */

#if WITHDSPLOCALFIR	
	/* Фильтрация квадратур осуществляется процессором */
	#if CPUSTYLE_R7S721
		#define Ntap_rx_SSB_IQ	NtapValidate(241)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(241)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_rx_AUDIO	NtapValidate(105)	// single samples, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation
	#elif CPUSTYLE_STM32F7XX
		#define Ntap_rx_SSB_IQ	NtapValidate(241)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(241)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_rx_AUDIO	NtapValidate(105)	// single samples, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation
	#else
		#define Ntap_rx_SSB_IQ	NtapValidate(181)	// SSB/CW filters: complex numbers, floating-point implementation
		#define Ntap_tx_SSB_IQ	NtapValidate(181)	// SSB/CW TX filter: complex numbers, floating-point implementation
		#define Ntap_rx_AUDIO	NtapValidate(105)	// single samples, floating-point implementation
		#define Ntap_tx_MIKE	NtapValidate(105)	// single samples, floating point implementation
	#endif

	// Фильтр для квадратурных каналов приёмника (floating point).
	static FLOAT_t FIRCoef_rx_SSB_IQ [NPROF] [NtapCoeffs(Ntap_rx_SSB_IQ)] = { { 0 }, { 0 } };
	static FLOAT_t FIRCwnd_rx_SSB_IQ [NtapCoeffs(Ntap_rx_SSB_IQ)];			// подготовленные значения функции окна

	// Фильтр для квадратурных каналов передатчика (floating point)
	static FLOAT_t FIRCoef_tx_SSB_IQ [NPROF] [NtapCoeffs(Ntap_tx_SSB_IQ)] = { { 0 }, { 0 } };
	static FLOAT_t FIRCwnd_tx_SSB_IQ [NtapCoeffs(Ntap_tx_SSB_IQ)];			// подготовленные значения функции окна

#endif /* WITHDSPLOCALFIR */

// Фильтр для аудиовыхода
// Обрабатывается как несимметричный
// массив паарметров для двух приемников
static FLOAT_t FIRCoef_rx_AUDIO [NPROF] [2 /* эта размерность номер тракта */] [NtapCoeffs(Ntap_rx_AUDIO)] = { { { 0 }, { 0 } }, { { 0 }, { 0 } } };
static FLOAT_t FIRCwnd_rx_AUDIO [NtapCoeffs(Ntap_rx_AUDIO)];			// подготовленные значения функции окна

// Фильтр для передатчика (floating point)
// Обрабатывается как несимметричный
static FLOAT_t FIRCoef_tx_MIKE [NPROF] [NtapCoeffs(Ntap_tx_MIKE)] = { { 0 }, { 0 } };
static FLOAT_t FIRCwnd_tx_MIKE [NtapCoeffs(Ntap_tx_MIKE)];			// подготовленные значения функции окна

/* Обработка производится всегда в наибольшей разрядности учавствующих кодеков. */
/* требуется согласовать разрядность данных IF и AF кодеков. */
#if (WITHIFDACWIDTH > WITHAFADCWIDTH)
	#define TXINSCALE		(1 << (WITHIFDACWIDTH - WITHAFADCWIDTH))		// характеризует разницу в разрядности АЦП источника сигнала и выходного ЦАП
	#define TXOUTDENOM		1
#elif (WITHIFDACWIDTH == WITHAFADCWIDTH)
	#define TXINSCALE		1				// характеризует разницу в разрядности АЦП источника сигнала и выходного ЦАП
	#define TXOUTDENOM		1
#else
	#error Strange WITHIFDACWIDTH & WITHAFADCWIDTH relations
#endif

/* требуется согласовать разрядность данных IF и AF кодеков. */
#if (WITHIFADCWIDTH > WITHAFDACWIDTH)
	#define RXINSCALE		1
	#define RXOUTDENOM		(1 << (WITHIFADCWIDTH - WITHAFDACWIDTH))				// характеризует разницу в разрядности АЦП источника сигнала и выходного ЦАП
#elif (WITHIFADCWIDTH == WITHAFDACWIDTH)
	#define RXINSCALE		1
	#define RXOUTDENOM		1				// характеризует разницу в разрядности АЦП источника сигнала и выходного ЦАП
#else
	#error Strange WITHIFADCWIDTH & WITHAFDACWIDTH relations
#endif

static FLOAT_t txlevelfence = INT_MAX;
static FLOAT_t txlevelfenceHALF = INT_MAX / 2;

static int_fast32_t txlevelfenceSSB_INTEGER = INT_MAX - 1;
static FLOAT_t txlevelfenceSSB = INT_MAX / 2;

static FLOAT_t txlevelfenceNFM = INT_MAX / 2;
static FLOAT_t txlevelfenceBPSK = INT_MAX / 2;
static FLOAT_t txlevelfenceCW = INT_MAX / 2;

static FLOAT_t rxlevelfence = INT_MAX;

static FLOAT_t mikefence = INT_MAX;
static FLOAT_t phonefence = INT_MAX;	// Разрядность поступающего на наушники сигнала
static FLOAT_t rxoutdenom = 1 / (FLOAT_t) RXOUTDENOM;

static volatile FLOAT_t nbfence;
static volatile FLOAT_t nfmoutscale;	// масштабирование (INT32_MAX + 1) к phonefence

static uint_fast8_t gwprof = 0;	// work profile - индекс конфигурационной информации, испольуемый для работы */

static uint_fast8_t globDSPMode [NPROF] [2] = { { DSPCTL_MODE_IDLE, DSPCTL_MODE_IDLE }, { DSPCTL_MODE_IDLE, DSPCTL_MODE_IDLE } };

/* Параметры АМ модулятора */
static volatile FLOAT_t amshapesignalHALF;
static volatile FLOAT_t amcarrierHALF;
static volatile FLOAT_t scaleDAC = 1;

static FLOAT_t shapeSidetoneStep(void);		// 0..1
static FLOAT_t shapeCWEnvelopStep(void);	// 0..1
static uint_fast8_t getTxShapeNotComplete(void);

static uint_fast8_t getRxGate(void);	/* разрешение работы тракта в режиме приёма */

//#include "sinetable.h"
//#include "sinetable_15.h"
#include "sinetable_14.h"
//#include "sinetable_13.h"
//#include "sinetable_12.h"
typedef uint32_t ncoftw_t;
typedef int32_t ncoftwi_t;
#define NCOFTWBITS 32	// количество битов в ncoftw_t
#define ASH (NCOFTWBITS - TABLELOG2)	// 22 = 32 - log2(number of items in sintable)
#define FTW2ANGLEI(ftw)	((uint32_t) (ftw) >> ASH)
#define FTW2ANGLEQ(ftw)	((uint32_t) ((ftw) + 0x40000000L) >> ASH)	// опережает на четверть оборота
#define FTWROUND(ftw) ((uint32_t) (ftw))
#define FTWAF001(freq) (((int_fast64_t) (freq) << NCOFTWBITS) / ARMI2SRATE100)
#define FTWAF(freq) (((int_fast64_t) (freq) << NCOFTWBITS) / ARMI2SRATE)
static FLOAT_t omega2ftw_k1; // = POWF(2, NCOFTWBITS);
#define OMEGA2FTWI(angle) ((ncoftwi_t) ((FLOAT_t) (angle) * omega2ftw_k1 / (FLOAT_t) M_TWOPI))	// angle in radians -pi..+pi to signed version of ftw_t

static RAMFUNC FLOAT_t peekvalf(uint32_t a)
{
	const ncoftw_t mask = (1UL << (TABLELOG2 - 2)) - 1;
	const ncoftw_t quoter = 1UL << (TABLELOG2 - 2);
	const ncoftw_t index = a & mask;
	switch (a >> (TABLELOG2 - 2))	 // получить номер квадранта по индексу в таблице
	{
	case 0: return sintable4f_fs [index];
	case 1: return sintable4f_fs [quoter - index];
	case 2: return - sintable4f_fs [index];
	case 3: return - sintable4f_fs [quoter - index];
	}
	return 0;
}

#if 1//WITHLOOPBACKTEST || WITHSUSBSPKONLY || WITHUSBHEADSET

static RAMFUNC int peekvali16(uint32_t a)
{
	const ncoftw_t mask = (1UL << (TABLELOG2 - 2)) - 1;
	const ncoftw_t quoter = 1UL << (TABLELOG2 - 2);
	const ncoftw_t index = a & mask;
	switch (a >> (TABLELOG2 - 2))	 // получить номер квадранта по индексу в таблице
	{
	case 0: return sintable4i32_fs [index] >> 16;
	case 1: return sintable4i32_fs [quoter - index] >> 16;
	case 2: return - sintable4i32_fs [index] >> 16;
	case 3: return - sintable4i32_fs [quoter - index] >> 16;
	}
	return 0;
}

static RAMFUNC int peekvali24(uint32_t a)
{
	const ncoftw_t mask = (1UL << (TABLELOG2 - 2)) - 1;
	const ncoftw_t quoter = 1UL << (TABLELOG2 - 2);
	const ncoftw_t index = a & mask;
	switch (a >> (TABLELOG2 - 2))	 // получить номер квадранта по индексу в таблице
	{
	case 0: return sintable4i32_fs [index] >> 8;
	case 1: return sintable4i32_fs [quoter - index] >> 8;
	case 2: return - sintable4i32_fs [index] >> 8;
	case 3: return - sintable4i32_fs [quoter - index] >> 8;
	}
	return 0;
}

static RAMFUNC int32_t peekvali32(uint32_t a)
{
	const ncoftw_t mask = (1UL << (TABLELOG2 - 2)) - 1;
	const ncoftw_t quoter = 1UL << (TABLELOG2 - 2);
	const ncoftw_t index = a & mask;
	switch (a >> (TABLELOG2 - 2))	 // получить номер квадранта по индексу в таблице
	{
	case 0: return sintable4i32_fs [index];
	case 1: return sintable4i32_fs [quoter - index];
	case 2: return - sintable4i32_fs [index];
	case 3: return - sintable4i32_fs [quoter - index];
	}
	return 0;
}

#endif /* WITHLOOPBACKTEST */

//////////////////////////////////////////
#if 1//WITHLOOPBACKTEST || WITHSUSBSPKONLY || WITHUSBHEADSET

static ncoftw_t anglestep_lout = FTWAF(700), anglestep_rout = FTWAF(500);
static ncoftw_t angle_lout, angle_rout;

static ncoftw_t anglestep_lout2 = FTWAF(5600), anglestep_rout2 = FTWAF(6300);
static ncoftw_t angle_lout2, angle_rout2;

int get_rout16(void)
{
	// Формирование значения для ROUT
	const int v = peekvali16(FTW2ANGLEI(angle_rout));
	angle_rout = FTWROUND(angle_rout + anglestep_rout);
	return v;
}

int get_lout16(void)
{
	// Формирование значения для LOUT
	const int v = peekvali16(FTW2ANGLEI(angle_lout));
	angle_lout = FTWROUND(angle_lout + anglestep_lout);
	return v;
}

static int get_rout24(void)
{
	// Формирование значения для ROUT
	const int v = peekvali24(FTW2ANGLEI(angle_rout2));
	angle_rout2 = FTWROUND(angle_rout2 + anglestep_rout2);
	return v;
}

static int get_lout24(void)
{
	// Формирование значения для LOUT
	const int v = peekvali24(FTW2ANGLEI(angle_lout2));
	angle_lout2 = FTWROUND(angle_lout2 + anglestep_lout2);
	return v;
}

#endif /* WITHLOOPBACKTEST */

//////////////////////////////////////////
static ncoftw_t anglestep_sidetone = 0; //FTWAF(700);
static ncoftw_t angle_sidetone = 0;

static RAMFUNC FLOAT_t get_float_sidetone(void)
{
	const FLOAT_t v = peekvalf(FTW2ANGLEI(angle_sidetone));
	angle_sidetone = FTWROUND(angle_sidetone + anglestep_sidetone);
	return v;
}

//////////////////////////////////////////

static ncoftw_t anglestep_subtone = 0; //FTWAF(700);
static ncoftw_t angle_subtone = 0;

static RAMFUNC FLOAT_t get_float_subtone(void)
{
	const FLOAT_t v = peekvalf(FTW2ANGLEI(angle_subtone));
	angle_subtone = FTWROUND(angle_subtone + anglestep_subtone);
	return v;
}

//////////////////////////////////////////

static ncoftw_t anglestep_toneout = FTWAF(700);
static ncoftw_t angle_toneout;

static RAMFUNC FLOAT_t get_singletonefloat(void)
{
	// Формирование значения для LOUT
	const FLOAT_t v = peekvalf(FTW2ANGLEI(angle_toneout));
	angle_toneout = FTWROUND(angle_toneout + anglestep_toneout);
	return v;
}

// двухтональный генератор для настройки
// параметры частот

// American (AT&T)
// dial tone 350 and 440 Hz
//static ncoftw_t anglestep_af1 = FTWAF(440);
//static ncoftw_t anglestep_af2 = FTWAF(350);

// Dual tone signal generator for SSB TX IMD3 tests - 1200 Hz spacing
// Document: Procedure Manual 2011 with page breaks.pdf, page 16
static ncoftw_t anglestep_af1 = FTWAF(700);
static ncoftw_t anglestep_af2 = FTWAF(1900);

static ncoftw_t angle_af1;
static ncoftw_t angle_af2;

// двухтональный генератор для настройки
static RAMFUNC FLOAT_t get_dualtonefloat(void)
{
	// Формирование значения выборки
	const FLOAT_t v1 = peekvalf(FTW2ANGLEI(angle_af1));
	const FLOAT_t v2 = peekvalf(FTW2ANGLEI(angle_af2));
	angle_af1 = FTWROUND(angle_af1 + anglestep_af1);
	angle_af2 = FTWROUND(angle_af2 + anglestep_af2);
	return (v1 + v2) / 2;
}


//////////////////////////////////////////
// Получение квадратурных значений для данной частоты
/*
static ncoftw_t anglestep_aflosim = FTWAF(700);
static ncoftw_t angle_aflosim = 0;
static INT32P_t get_int32_aflosim(void)
{
	INT32P_t v;
	v.IV = peekvali32(FTW2ANGLEI(angle_aflosim));
	v.QV = peekvali32(FTW2ANGLEQ(angle_aflosim));
	angle_aflosim = FTWROUND(angle_aflosim + anglestep_aflosim);
	return v;
}
*/

static ncoftw_t anglestep_aflo [NTRX] = { 0, };
static ncoftw_t angle_aflo [NTRX] = { 0, };
const ncoftw_t gnfmdeviationftw = FTWAF(2500);	// 2.5 kHz (-2.5..+2.5) deviation

// установить частоту
static void nco_setlo_ftw(ncoftw_t ftw, uint_fast8_t pathi)
{
	if (ftw == 0)
	{
		/* Для обеспечения максимальной амплитуды после суммирования квадратурных составляющих в FPA */
		//angle_aflo [pathi] = (ncoftw_t) 1 << (NCOFTWBITS - 2);	// 1/2 Pi
		/* Для обеспечения 0.7 от максимальной амплитуды после суммирования квадратурных составляющих в FPA */
		angle_aflo [pathi] = 0;	// 0 Pi
	}
	anglestep_aflo [pathi] = ftw;
}

// Получение квадратурных значений для данной частоты со смещением (в герцах)
// Returned is a full scale value
static RAMFUNC FLOAT32P_t get_float_aflo_delta(long int deltaftw, uint_fast8_t pathi)
{
	FLOAT32P_t v;
	v.IV = peekvalf(FTW2ANGLEI(angle_aflo [pathi]));
	v.QV = peekvalf(FTW2ANGLEQ(angle_aflo [pathi]));
	angle_aflo [pathi] = FTWROUND(angle_aflo [pathi] + anglestep_aflo [pathi] + deltaftw);
	return v;
}

#if 0
// Получение квадратурных значений для данной частоты со смещением (в герцах)
// Returned is a full scale value
static RAMFUNC INT32P_t get_int32_aflo_delta(long int deltaftw, uint_fast8_t pathi)
{
	INT32P_t v;
	v.IV = peekvali32(FTW2ANGLEI(angle_aflo [pathi]));
	v.QV = peekvali32(FTW2ANGLEQ(angle_aflo [pathi]));
	angle_aflo [pathi] = FTWROUND(angle_aflo [pathi] + anglestep_aflo [pathi] + deltaftw);
	return v;
}
#endif

//////////////////////////////////////////

#if ! WITHDSPEXTDDC

static const ncoftw_t anglestep_iflo = (1U << (NCOFTWBITS - 2));	// установить частоту в 1/4 sample rate
static ncoftw_t angle_iflo = 0;


// Получение квадратурных значений для данной частоты
static FLOAT32P_t get_float4_iflo(void)
{
	static const FLOAT_t sintable4f_fs [4] = { + 0, + 1, + 0, - 1 };
	FLOAT32P_t v;
	v.IV = (sintable4f_fs [FTW2ANGLEI(angle_iflo) >> (TABLELOG2 - 2)]);
	v.QV = (sintable4f_fs [FTW2ANGLEQ(angle_iflo) >> (TABLELOG2 - 2)]);
	angle_iflo = FTWROUND(angle_iflo + anglestep_iflo);
	return v;
}

#endif /* ! WITHDSPEXTDDC */

//////////////////////////////////////////

//////////////////////////////////////////

// Преобразовать отношение напряжений выраженное в "разах" к децибелам.
/*
static FLOAT_t ratio2db(FLOAT_t ratio)
{
	return LOG10F(ratio) * 20;
}
*/
// Преобразовать отношение выраженное в децибелах к "разам" отношения напряжений.

static FLOAT_t db2ratio(FLOAT_t valueDBb)
{
	return POWF(10, valueDBb / 20);
}

//////////////////////////////////////////

/* from "C Language Algorithms for Digital Signal Processing"
   by Paul M. Embree and Bruce Kimble, Prentice Hall, 1991 */

	
struct Complex
{
	FLOAT_t real;
	FLOAT_t imag;
};

#define FFTSizeFilters (1 << (FFTSizeFiltersM))
#define FFTSizeSpectrum (1 << (FFTSizeSpectrumM))

#if WITHDYNAMICCOEFFS

static struct Complex wm [FFTSizeFilters / 2];
static struct Complex wp [FFTSizeFilters / 2];


static void FFT_initw(struct Complex *w, int n, int sign)
{
   struct Complex *xj;
   int j, le;
   FLOAT_t arg, w_real, w_imag, wrecur_real, wrecur_imag, wtemp_real;

   //for (m = 0; (1 << m) < n; m++)
//	   ;
   le = n / 2;

   arg = (FLOAT_t) M_PI / le;
   //arg = 4 * ATANF(1) / le;
   wrecur_real = w_real = COSF(arg);
   wrecur_imag = w_imag = SINF(arg) * sign;
   xj = w;

   for (j = 1; j < le; j++)
   {
      xj->real = wrecur_real;
      xj->imag = wrecur_imag;
      xj++;
      wtemp_real  = wrecur_real * w_real - wrecur_imag * w_imag;
      wrecur_imag = wrecur_real * w_imag + wrecur_imag * w_real;
      wrecur_real = wtemp_real;
   }
   /* к этому элементу небыло обращения в оригинальном коде, выделялась так же без него память */
  xj->real = wrecur_real;
  xj->imag = wrecur_imag;
}

#else /* WITHDYNAMICCOEFFS */

#if FFTSizeFilters == 1024
/* FFT/IFFT w coefficients tables for 1024 elements */
static const FLASHMEM struct Complex wm [512] = 
{
	{ +0.99998117528260111000, -0.00613588464915447530 },
	{ +0.99992470183914450000, -0.01227153828571992500 },
	{ +0.99983058179582329000, -0.01840672990580482000 },
	{ +0.99969881869620414000, -0.02454122852291228800 },
	{ +0.99952941750109314000, -0.03067480317663662600 },
	{ +0.99932238458834943000, -0.03680722294135883200 },
	{ +0.99907772775264536000, -0.04293825693494082000 },
	{ +0.99879545620517241000, -0.04906767432741800800 },
	{ +0.99847558057329466000, -0.05519524434968993400 },
	{ +0.99811811290014907000, -0.06132073630220857100 },
	{ +0.99772306664419141000, -0.06744391956366405100 },
	{ +0.99729045667868998000, -0.07356456359966741200 },
	{ +0.99682029929116545000, -0.07968243797143009800 },
	{ +0.99631261218277778000, -0.08579731234443985200 },
	{ +0.99576741446765960000, -0.09190895649713269600 },
	{ +0.99518472667219660000, -0.09801714032956056200 },
	{ +0.99456457073425508000, -0.10412163387205453000 },
	{ +0.99390697000235573000, -0.11022220729388300000 },
	{ +0.99321194923479417000, -0.11631863091190471000 },
	{ +0.99247953459870952000, -0.12241067519921613000 },
	{ +0.99170975366909897000, -0.12849811079379309000 },
	{ +0.99090263542777945000, -0.13458070850712611000 },
	{ +0.99005821026229646000, -0.14065823933284916000 },
	{ +0.98917650996478035000, -0.14673047445536169000 },
	{ +0.98825756773074891000, -0.15279718525844335000 },
	{ +0.98730141815785777000, -0.15885814333386136000 },
	{ +0.98630809724459800000, -0.16491312048996984000 },
	{ +0.98527764238894056000, -0.17096188876030113000 },
	{ +0.98421009238692836000, -0.17700422041214867000 },
	{ +0.98310548743121562000, -0.18303988795514084000 },
	{ +0.98196386910955447000, -0.18906866414980608000 },
	{ +0.98078528040322954000, -0.19509032201612814000 },
	{ +0.97956976568543952000, -0.20110463484209176000 },
	{ +0.97831737071962654000, -0.20711137619221839000 },
	{ +0.97702814265775317000, -0.21311031991609122000 },
	{ +0.97570213003852735000, -0.21910124015686963000 },
	{ +0.97433938278557464000, -0.22508391135979267000 },
	{ +0.97293995220555884000, -0.23105810828067094000 },
	{ +0.97150389098625045000, -0.23702360599436700000 },
	{ +0.97003125319454264000, -0.24298017990326365000 },
	{ +0.96852209427441593000, -0.24892760574571990000 },
	{ +0.96697647104485074000, -0.25486565960451429000 },
	{ +0.96539444169768796000, -0.26079411791527524000 },
	{ +0.96377606579543840000, -0.26671275747489809000 },
	{ +0.96212140426904003000, -0.27262135544994864000 },
	{ +0.96043051941556423000, -0.27851968938505273000 },
	{ +0.95870347489586993000, -0.28440753721127143000 },
	{ +0.95694033573220716000, -0.29028467725446194000 },
	{ +0.95514116830576890000, -0.29615088824362334000 },
	{ +0.95330604035419197000, -0.30200594931922758000 },
	{ +0.95143502096900645000, -0.30784964004153437000 },
	{ +0.94952818059303479000, -0.31368174039889096000 },
	{ +0.94758559101773920000, -0.31950203081601514000 },
	{ +0.94560732538051928000, -0.32531029216226237000 },
	{ +0.94359345816195828000, -0.33110630575987582000 },
	{ +0.94154406518301870000, -0.33688985339221944000 },
	{ +0.93945922360218781000, -0.34266071731199377000 },
	{ +0.93733901191257274000, -0.34841868024943390000 },
	{ +0.93518350993894539000, -0.35416352542048968000 },
	{ +0.93299279883473674000, -0.35989503653498739000 },
	{ +0.93076696107898149000, -0.36561299780477313000 },
	{ +0.92850608047321326000, -0.37131719395183677000 },
	{ +0.92621024213830905000, -0.37700741021641748000 },
	{ +0.92387953251128452000, -0.38268343236508900000 },
	{ +0.92151403934203968000, -0.38834504669882552000 },
	{ +0.91911385169005544000, -0.39399204006104732000 },
	{ +0.91667905992104037000, -0.39962419984564601000 },
	{ +0.91420975570352825000, -0.40524131400498903000 },
	{ +0.91170603200542744000, -0.41084317105790308000 },
	{ +0.90916798309051994000, -0.41642956009763626000 },
	{ +0.90659570451491289000, -0.42200027079979874000 },
	{ +0.90398929312344090000, -0.42755509343028114000 },
	{ +0.90134884704601959000, -0.43309381885315101000 },
	{ +0.89867446569395137000, -0.43861623853852666000 },
	{ +0.89596624975618266000, -0.44412214457042820000 },
	{ +0.89322430119551277000, -0.44961132965460554000 },
	{ +0.89044872324475532000, -0.45508358712634273000 },
	{ +0.88763962040285138000, -0.46053871095823889000 },
	{ +0.88479709843093524000, -0.46597649576796502000 },
	{ +0.88192126434835250000, -0.47139673682599648000 },
	{ +0.87901222642863086000, -0.47679923006332092000 },
	{ +0.87607009419540394000, -0.48218377207912150000 },
	{ +0.87309497841828743000, -0.48755016014843466000 },
	{ +0.87008699110870868000, -0.49289819222978271000 },
	{ +0.86704624551568998000, -0.49822766697278048000 },
	{ +0.86397285612158403000, -0.50353838372571613000 },
	{ +0.86086693863776453000, -0.50883014254310555000 },
	{ +0.85772861000026934000, -0.51410274419322022000 },
	{ +0.85455798836539776000, -0.51935599016558809000 },
	{ +0.85135519310526231000, -0.52458968267846728000 },
	{ +0.84812034480329446000, -0.52980362468629294000 },
	{ +0.84485356524970423000, -0.53499761988709549000 },
	{ +0.84155497743689556000, -0.54017147272989119000 },
	{ +0.83822470555483519000, -0.54532498842204469000 },
	{ +0.83486287498637712000, -0.55045797293660303000 },
	{ +0.83146961230254224000, -0.55557023301960040000 },
	{ +0.82804504525775269000, -0.56066157619733414000 },
	{ +0.82458930278502218000, -0.56573181078361123000 },
	{ +0.82110251499110154000, -0.57078074588696526000 },
	{ +0.81758481315158049000, -0.57580819141784323000 },
	{ +0.81403632970594508000, -0.58081395809576242000 },
	{ +0.81045719825259144000, -0.58579785745643664000 },
	{ +0.80684755354379600000, -0.59075970185887194000 },
	{ +0.80320753148064161000, -0.59569930449243103000 },
	{ +0.79953726910790168000, -0.60061647938386664000 },
	{ +0.79583690460888024000, -0.60551104140432321000 },
	{ +0.79210657730020895000, -0.61038280627630703000 },
	{ +0.78834642762660290000, -0.61523159058062438000 },
	{ +0.78455659715557191000, -0.62005721176328665000 },
	{ +0.78073722857209116000, -0.62485948814238379000 },
	{ +0.77688846567322911000, -0.62963823891492443000 },
	{ +0.77301045336273366000, -0.63439328416364293000 },
	{ +0.76910333764557626000, -0.63912444486377318000 },
	{ +0.76516726562245552000, -0.64383154288978883000 },
	{ +0.76120238548425834000, -0.64851440102210978000 },
	{ +0.75720884650648113000, -0.65317284295377409000 },
	{ +0.75318679904360908000, -0.65780669329707586000 },
	{ +0.74913639452345593000, -0.66241577759016890000 },
	{ +0.74505778544146251000, -0.66699992230363458000 },
	{ +0.74095112535495555000, -0.67155895484701544000 },
	{ +0.73681656887736624000, -0.67609270357531293000 },
	{ +0.73265427167240915000, -0.68060099779544991000 },
	{ +0.72846439044822142000, -0.68508366777269725000 },
	{ +0.72424708295146312000, -0.68954054473706372000 },
	{ +0.72000250796137788000, -0.69397146088965078000 },
	{ +0.71573082528381482000, -0.69837624940896958000 },
	{ +0.71143219574521266000, -0.70275474445722197000 },
	{ +0.70710678118654369000, -0.70710678118654413000 },
	{ +0.70275474445722153000, -0.71143219574521299000 },
	{ +0.69837624940896914000, -0.71573082528381515000 },
	{ +0.69397146088965034000, -0.72000250796137810000 },
	{ +0.68954054473706328000, -0.72424708295146334000 },
	{ +0.68508366777269680000, -0.72846439044822153000 },
	{ +0.68060099779544947000, -0.73265427167240904000 },
	{ +0.67609270357531237000, -0.73681656887736602000 },
	{ +0.67155895484701478000, -0.74095112535495522000 },
	{ +0.66699992230363392000, -0.74505778544146195000 },
	{ +0.66241577759016812000, -0.74913639452345526000 },
	{ +0.65780669329707497000, -0.75318679904360841000 },
	{ +0.65317284295377309000, -0.75720884650648035000 },
	{ +0.64851440102210878000, -0.76120238548425756000 },
	{ +0.64383154288978783000, -0.76516726562245463000 },
	{ +0.63912444486377218000, -0.76910333764557526000 },
	{ +0.63439328416364194000, -0.77301045336273255000 },
	{ +0.62963823891492343000, -0.77688846567322800000 },
	{ +0.62485948814238279000, -0.78073722857209005000 },
	{ +0.62005721176328554000, -0.78455659715557080000 },
	{ +0.61523159058062327000, -0.78834642762660179000 },
	{ +0.61038280627630581000, -0.79210657730020784000 },
	{ +0.60551104140432188000, -0.79583690460887901000 },
	{ +0.60061647938386531000, -0.79953726910790046000 },
	{ +0.59569930449242969000, -0.80320753148064028000 },
	{ +0.59075970185887061000, -0.80684755354379456000 },
	{ +0.58579785745643531000, -0.81045719825259011000 },
	{ +0.58081395809576097000, -0.81403632970594364000 },
	{ +0.57580819141784179000, -0.81758481315157894000 },
	{ +0.57078074588696381000, -0.82110251499109987000 },
	{ +0.56573181078360968000, -0.82458930278502041000 },
	{ +0.56066157619733248000, -0.82804504525775080000 },
	{ +0.55557023301959862000, -0.83146961230254024000 },
	{ +0.55045797293660126000, -0.83486287498637501000 },
	{ +0.54532498842204280000, -0.83822470555483297000 },
	{ +0.54017147272988930000, -0.84155497743689323000 },
	{ +0.53499761988709360000, -0.84485356524970179000 },
	{ +0.52980362468629105000, -0.84812034480329190000 },
	{ +0.52458968267846529000, -0.85135519310525976000 },
	{ +0.51935599016558598000, -0.85455798836539509000 },
	{ +0.51410274419321811000, -0.85772861000026657000 },
	{ +0.50883014254310344000, -0.86086693863776176000 },
	{ +0.50353838372571402000, -0.86397285612158115000 },
	{ +0.49822766697277832000, -0.86704624551568699000 },
	{ +0.49289819222978054000, -0.87008699110870569000 },
	{ +0.48755016014843244000, -0.87309497841828432000 },
	{ +0.48218377207911922000, -0.87607009419540083000 },
	{ +0.47679923006331865000, -0.87901222642862764000 },
	{ +0.47139673682599414000, -0.88192126434834917000 },
	{ +0.46597649576796268000, -0.88479709843093179000 },
	{ +0.46053871095823656000, -0.88763962040284794000 },
	{ +0.45508358712634039000, -0.89044872324475188000 },
	{ +0.44961132965460321000, -0.89322430119550933000 },
	{ +0.44412214457042587000, -0.89596624975617911000 },
	{ +0.43861623853852433000, -0.89867446569394771000 },
	{ +0.43309381885314868000, -0.90134884704601592000 },
	{ +0.42755509343027881000, -0.90398929312343712000 },
	{ +0.42200027079979646000, -0.90659570451490912000 },
	{ +0.41642956009763399000, -0.90916798309051605000 },
	{ +0.41084317105790075000, -0.91170603200542344000 },
	{ +0.40524131400498670000, -0.91420975570352425000 },
	{ +0.39962419984564368000, -0.91667905992103627000 },
	{ +0.39399204006104499000, -0.91911385169005122000 },
	{ +0.38834504669882319000, -0.92151403934203535000 },
	{ +0.38268343236508673000, -0.92387953251128019000 },
	{ +0.37700741021641526000, -0.92621024213830472000 },
	{ +0.37131719395183460000, -0.92850608047320893000 },
	{ +0.36561299780477097000, -0.93076696107897705000 },
	{ +0.35989503653498528000, -0.93299279883473218000 },
	{ +0.35416352542048757000, -0.93518350993894084000 },
	{ +0.34841868024943173000, -0.93733901191256819000 },
	{ +0.34266071731199160000, -0.93945922360218315000 },
	{ +0.33688985339221728000, -0.94154406518301392000 },
	{ +0.33110630575987365000, -0.94359345816195339000 },
	{ +0.32531029216226021000, -0.94560732538051440000 },
	{ +0.31950203081601297000, -0.94758559101773421000 },
	{ +0.31368174039888880000, -0.94952818059302968000 },
	{ +0.30784964004153226000, -0.95143502096900134000 },
	{ +0.30200594931922548000, -0.95330604035418676000 },
	{ +0.29615088824362124000, -0.95514116830576357000 },
	{ +0.29028467725445983000, -0.95694033573220172000 },
	{ +0.28440753721126932000, -0.95870347489586427000 },
	{ +0.27851968938505062000, -0.96043051941555846000 },
	{ +0.27262135544994653000, -0.96212140426903414000 },
	{ +0.26671275747489598000, -0.96377606579543229000 },
	{ +0.26079411791527313000, -0.96539444169768174000 },
	{ +0.25486565960451218000, -0.96697647104484441000 },
	{ +0.24892760574571782000, -0.96852209427440950000 },
	{ +0.24298017990326157000, -0.97003125319453609000 },
	{ +0.23702360599436492000, -0.97150389098624379000 },
	{ +0.23105810828066886000, -0.97293995220555207000 },
	{ +0.22508391135979061000, -0.97433938278556775000 },
	{ +0.21910124015686763000, -0.97570213003852035000 },
	{ +0.21311031991608925000, -0.97702814265774618000 },
	{ +0.20711137619221648000, -0.97831737071961944000 },
	{ +0.20110463484208990000, -0.97956976568543230000 },
	{ +0.19509032201612631000, -0.98078528040322221000 },
	{ +0.18906866414980428000, -0.98196386910954692000 },
	{ +0.18303988795513906000, -0.98310548743120796000 },
	{ +0.17700422041214689000, -0.98421009238692070000 },
	{ +0.17096188876029939000, -0.98527764238893289000 },
	{ +0.16491312048996815000, -0.98630809724459023000 },
	{ +0.15885814333385972000, -0.98730141815785000000 },
	{ +0.15279718525844177000, -0.98825756773074114000 },
	{ +0.14673047445536014000, -0.98917650996477258000 },
	{ +0.14065823933284766000, -0.99005821026228868000 },
	{ +0.13458070850712467000, -0.99090263542777157000 },
	{ +0.12849811079379170000, -0.99170975366909098000 },
	{ +0.12241067519921478000, -0.99247953459870142000 },
	{ +0.11631863091190339000, -0.99321194923478595000 },
	{ +0.11022220729388173000, -0.99390697000234740000 },
	{ +0.10412163387205330000, -0.99456457073424676000 },
	{ +0.09801714032955936900, -0.99518472667218816000 },
	{ +0.09190895649713154400, -0.99576741446765105000 },
	{ +0.08579731234443875600, -0.99631261218276923000 },
	{ +0.07968243797142904300, -0.99682029929115690000 },
	{ +0.07356456359966638500, -0.99729045667868133000 },
	{ +0.06744391956366306600, -0.99772306664418275000 },
	{ +0.06132073630220763500, -0.99811811290014041000 },
	{ +0.05519524434968905300, -0.99847558057328589000 },
	{ +0.04906767432741718200, -0.99879545620516341000 },
	{ +0.04293825693494004300, -0.99907772775263637000 },
	{ +0.03680722294135811000, -0.99932238458834044000 },
	{ +0.03067480317663596000, -0.99952941750108404000 },
	{ +0.02454122852291167800, -0.99969881869619504000 },
	{ +0.01840672990580426900, -0.99983058179581419000 },
	{ +0.01227153828571943100, -0.99992470183913529000 },
	{ +0.00613588464915403730, -0.99998117528259189000 },
	{ -0.00000000000000038164, -0.99999999999999067000 },
	{ -0.00613588464915479970, -0.99998117528259178000 },
	{ -0.01227153828572019300, -0.99992470183913518000 },
	{ -0.01840672990580503200, -0.99983058179581397000 },
	{ -0.02454122852291244100, -0.99969881869619481000 },
	{ -0.03067480317663672000, -0.99952941750108371000 },
	{ -0.03680722294135886600, -0.99932238458834000000 },
	{ -0.04293825693494079900, -0.99907772775263592000 },
	{ -0.04906767432741793200, -0.99879545620516297000 },
	{ -0.05519524434968979500, -0.99847558057328523000 },
	{ -0.06132073630220837700, -0.99811811290013963000 },
	{ -0.06744391956366380100, -0.99772306664418198000 },
	{ -0.07356456359966710700, -0.99729045667868055000 },
	{ -0.07968243797142973700, -0.99682029929115601000 },
	{ -0.08579731234443943600, -0.99631261218276834000 },
	{ -0.09190895649713222400, -0.99576741446765016000 },
	{ -0.09801714032956003500, -0.99518472667218716000 },
	{ -0.10412163387205395000, -0.99456457073424565000 },
	{ -0.11022220729388237000, -0.99390697000234629000 },
	{ -0.11631863091190400000, -0.99321194923478484000 },
	{ -0.12241067519921536000, -0.99247953459870020000 },
	{ -0.12849811079379228000, -0.99170975366908964000 },
	{ -0.13458070850712525000, -0.99090263542777013000 },
	{ -0.14065823933284824000, -0.99005821026228713000 },
	{ -0.14673047445536072000, -0.98917650996477102000 },
	{ -0.15279718525844233000, -0.98825756773073958000 },
	{ -0.15885814333386028000, -0.98730141815784844000 },
	{ -0.16491312048996870000, -0.98630809724458868000 },
	{ -0.17096188876029994000, -0.98527764238893123000 },
	{ -0.17700422041214742000, -0.98421009238691903000 },
	{ -0.18303988795513954000, -0.98310548743120629000 },
	{ -0.18906866414980472000, -0.98196386910954514000 },
	{ -0.19509032201612669000, -0.98078528040322022000 },
	{ -0.20110463484209026000, -0.97956976568543030000 },
	{ -0.20711137619221684000, -0.97831737071961733000 },
	{ -0.21311031991608961000, -0.97702814265774396000 },
	{ -0.21910124015686797000, -0.97570213003851813000 },
	{ -0.22508391135979094000, -0.97433938278556542000 },
	{ -0.23105810828066917000, -0.97293995220554974000 },
	{ -0.23702360599436517000, -0.97150389098624135000 },
	{ -0.24298017990326176000, -0.97003125319453354000 },
	{ -0.24892760574571796000, -0.96852209427440683000 },
	{ -0.25486565960451230000, -0.96697647104484163000 },
	{ -0.26079411791527318000, -0.96539444169767885000 },
	{ -0.26671275747489598000, -0.96377606579542940000 },
	{ -0.27262135544994648000, -0.96212140426903103000 },
	{ -0.27851968938505051000, -0.96043051941555524000 },
	{ -0.28440753721126916000, -0.95870347489586094000 },
	{ -0.29028467725445961000, -0.95694033573219817000 },
	{ -0.29615088824362096000, -0.95514116830576001000 },
	{ -0.30200594931922514000, -0.95330604035418309000 },
	{ -0.30784964004153187000, -0.95143502096899757000 },
	{ -0.31368174039888841000, -0.94952818059302591000 },
	{ -0.31950203081601253000, -0.94758559101773032000 },
	{ -0.32531029216225971000, -0.94560732538051040000 },
	{ -0.33110630575987310000, -0.94359345816194939000 },
	{ -0.33688985339221666000, -0.94154406518300982000 },
	{ -0.34266071731199094000, -0.93945922360217893000 },
	{ -0.34841868024943101000, -0.93733901191256386000 },
	{ -0.35416352542048674000, -0.93518350993893651000 },
	{ -0.35989503653498439000, -0.93299279883472785000 },
	{ -0.36561299780477008000, -0.93076696107897261000 },
	{ -0.37131719395183366000, -0.92850608047320438000 },
	{ -0.37700741021641432000, -0.92621024213830017000 },
	{ -0.38268343236508578000, -0.92387953251127564000 },
	{ -0.38834504669882225000, -0.92151403934203080000 },
	{ -0.39399204006104399000, -0.91911385169004667000 },
	{ -0.39962419984564262000, -0.91667905992103160000 },
	{ -0.40524131400498559000, -0.91420975570351959000 },
	{ -0.41084317105789958000, -0.91170603200541878000 },
	{ -0.41642956009763271000, -0.90916798309051128000 },
	{ -0.42200027079979513000, -0.90659570451490423000 },
	{ -0.42755509343027748000, -0.90398929312343224000 },
	{ -0.43309381885314729000, -0.90134884704601093000 },
	{ -0.43861623853852288000, -0.89867446569394271000 },
	{ -0.44412214457042437000, -0.89596624975617412000 },
	{ -0.44961132965460171000, -0.89322430119550433000 },
	{ -0.45508358712633884000, -0.89044872324474700000 },
	{ -0.46053871095823495000, -0.88763962040284305000 },
	{ -0.46597649576796102000, -0.88479709843092691000 },
	{ -0.47139673682599242000, -0.88192126434834417000 },
	{ -0.47679923006331681000, -0.87901222642862265000 },
	{ -0.48218377207911733000, -0.87607009419539572000 },
	{ -0.48755016014843050000, -0.87309497841827921000 },
	{ -0.49289819222977849000, -0.87008699110870058000 },
	{ -0.49822766697277621000, -0.86704624551568188000 },
	{ -0.50353838372571180000, -0.86397285612157604000 },
	{ -0.50883014254310122000, -0.86086693863775665000 },
	{ -0.51410274419321589000, -0.85772861000026146000 },
	{ -0.51935599016558365000, -0.85455798836538988000 },
	{ -0.52458968267846284000, -0.85135519310525443000 },
	{ -0.52980362468628850000, -0.84812034480328657000 },
	{ -0.53499761988709094000, -0.84485356524969635000 },
	{ -0.54017147272988653000, -0.84155497743688767000 },
	{ -0.54532498842203991000, -0.83822470555482731000 },
	{ -0.55045797293659826000, -0.83486287498636935000 },
	{ -0.55557023301959552000, -0.83146961230253458000 },
	{ -0.56066157619732926000, -0.82804504525774503000 },
	{ -0.56573181078360635000, -0.82458930278501463000 },
	{ -0.57078074588696037000, -0.82110251499109399000 },
	{ -0.57580819141783823000, -0.81758481315157294000 },
	{ -0.58081395809575742000, -0.81403632970593764000 },
	{ -0.58579785745643165000, -0.81045719825258411000 },
	{ -0.59075970185886695000, -0.80684755354378868000 },
	{ -0.59569930449242603000, -0.80320753148063440000 },
	{ -0.60061647938386153000, -0.79953726910789458000 },
	{ -0.60551104140431800000, -0.79583690460887313000 },
	{ -0.61038280627630181000, -0.79210657730020195000 },
	{ -0.61523159058061916000, -0.78834642762659590000 },
	{ -0.62005721176328132000, -0.78455659715556492000 },
	{ -0.62485948814237846000, -0.78073722857208416000 },
	{ -0.62963823891491899000, -0.77688846567322223000 },
	{ -0.63439328416363738000, -0.77301045336272678000 },
	{ -0.63912444486376752000, -0.76910333764556948000 },
	{ -0.64383154288978317000, -0.76516726562244874000 },
	{ -0.64851440102210411000, -0.76120238548425156000 },
	{ -0.65317284295376843000, -0.75720884650647435000 },
	{ -0.65780669329707020000, -0.75318679904360231000 },
	{ -0.66241577759016324000, -0.74913639452344916000 },
	{ -0.66699992230362892000, -0.74505778544145584000 },
	{ -0.67155895484700967000, -0.74095112535494900000 },
	{ -0.67609270357530715000, -0.73681656887735980000 },
	{ -0.68060099779544414000, -0.73265427167240282000 },
	{ -0.68508366777269136000, -0.72846439044821520000 },
	{ -0.68954054473705784000, -0.72424708295145690000 },
	{ -0.69397146088964490000, -0.72000250796137166000 },
	{ -0.69837624940896370000, -0.71573082528380871000 },
	{ -0.70275474445721609000, -0.71143219574520655000 },
	{ -0.70710678118653825000, -0.70710678118653769000 },
	{ -0.71143219574520711000, -0.70275474445721553000 },
	{ -0.71573082528380916000, -0.69837624940896315000 },
	{ -0.72000250796137211000, -0.69397146088964434000 },
	{ -0.72424708295145723000, -0.68954054473705728000 },
	{ -0.72846439044821543000, -0.68508366777269081000 },
	{ -0.73265427167240293000, -0.68060099779544347000 },
	{ -0.73681656887735980000, -0.67609270357530649000 },
	{ -0.74095112535494889000, -0.67155895484700889000 },
	{ -0.74505778544145562000, -0.66699992230362803000 },
	{ -0.74913639452344882000, -0.66241577759016235000 },
	{ -0.75318679904360186000, -0.65780669329706931000 },
	{ -0.75720884650647380000, -0.65317284295376743000 },
	{ -0.76120238548425101000, -0.64851440102210312000 },
	{ -0.76516726562244808000, -0.64383154288978217000 },
	{ -0.76910333764556871000, -0.63912444486376652000 },
	{ -0.77301045336272589000, -0.63439328416363627000 },
	{ -0.77688846567322123000, -0.62963823891491788000 },
	{ -0.78073722857208327000, -0.62485948814237735000 },
	{ -0.78455659715556392000, -0.62005721176328021000 },
	{ -0.78834642762659490000, -0.61523159058061794000 },
	{ -0.79210657730020095000, -0.61038280627630059000 },
	{ -0.79583690460887213000, -0.60551104140431666000 },
	{ -0.79953726910789347000, -0.60061647938386009000 },
	{ -0.80320753148063329000, -0.59569930449242459000 },
	{ -0.80684755354378757000, -0.59075970185886562000 },
	{ -0.81045719825258300000, -0.58579785745643032000 },
	{ -0.81403632970593653000, -0.58081395809575598000 },
	{ -0.81758481315157172000, -0.57580819141783679000 },
	{ -0.82110251499109255000, -0.57078074588695882000 },
	{ -0.82458930278501297000, -0.56573181078360479000 },
	{ -0.82804504525774336000, -0.56066157619732770000 },
	{ -0.83146961230253280000, -0.55557023301959396000 },
	{ -0.83486287498636746000, -0.55045797293659660000 },
	{ -0.83822470555482531000, -0.54532498842203825000 },
	{ -0.84155497743688557000, -0.54017147272988475000 },
	{ -0.84485356524969413000, -0.53499761988708916000 },
	{ -0.84812034480328424000, -0.52980362468628672000 },
	{ -0.85135519310525209000, -0.52458968267846107000 },
	{ -0.85455798836538743000, -0.51935599016558176000 },
	{ -0.85772861000025891000, -0.51410274419321389000 },
	{ -0.86086693863775399000, -0.50883014254309933000 },
	{ -0.86397285612157337000, -0.50353838372570991000 },
	{ -0.86704624551567921000, -0.49822766697277426000 },
	{ -0.87008699110869792000, -0.49289819222977654000 },
	{ -0.87309497841827655000, -0.48755016014842850000 },
	{ -0.87607009419539295000, -0.48218377207911534000 },
	{ -0.87901222642861976000, -0.47679923006331482000 },
	{ -0.88192126434834117000, -0.47139673682599037000 },
	{ -0.88479709843092380000, -0.46597649576795896000 },
	{ -0.88763962040283995000, -0.46053871095823290000 },
	{ -0.89044872324474389000, -0.45508358712633679000 },
	{ -0.89322430119550134000, -0.44961132965459966000 },
	{ -0.89596624975617112000, -0.44412214457042232000 },
	{ -0.89867446569393972000, -0.43861623853852083000 },
	{ -0.90134884704600782000, -0.43309381885314524000 },
	{ -0.90398929312342902000, -0.42755509343027542000 },
	{ -0.90659570451490101000, -0.42200027079979313000 },
	{ -0.90916798309050795000, -0.41642956009763071000 },
	{ -0.91170603200541533000, -0.41084317105789753000 },
	{ -0.91420975570351604000, -0.40524131400498353000 },
	{ -0.91667905992102805000, -0.39962419984564057000 },
	{ -0.91911385169004300000, -0.39399204006104194000 },
	{ -0.92151403934202714000, -0.38834504669882020000 },
	{ -0.92387953251127186000, -0.38268343236508373000 },
	{ -0.92621024213829639000, -0.37700741021641232000 },
	{ -0.92850608047320060000, -0.37131719395183171000 },
	{ -0.93076696107896872000, -0.36561299780476814000 },
	{ -0.93299279883472375000, -0.35989503653498245000 },
	{ -0.93518350993893229000, -0.35416352542048479000 },
	{ -0.93733901191255964000, -0.34841868024942901000 },
	{ -0.93945922360217460000, -0.34266071731198894000 },
	{ -0.94154406518300537000, -0.33688985339221467000 },
	{ -0.94359345816194484000, -0.33110630575987110000 },
	{ -0.94560732538050585000, -0.32531029216225771000 },
	{ -0.94758559101772566000, -0.31950203081601053000 },
	{ -0.94952818059302102000, -0.31368174039888641000 },
	{ -0.95143502096899268000, -0.30784964004152993000 },
	{ -0.95330604035417810000, -0.30200594931922320000 },
	{ -0.95514116830575491000, -0.29615088824361901000 },
	{ -0.95694033573219306000, -0.29028467725445767000 },
	{ -0.95870347489585561000, -0.28440753721126721000 },
	{ -0.96043051941554980000, -0.27851968938504856000 },
	{ -0.96212140426902548000, -0.27262135544994454000 },
	{ -0.96377606579542363000, -0.26671275747489404000 },
	{ -0.96539444169767308000, -0.26079411791527124000 },
	{ -0.96697647104483575000, -0.25486565960451035000 },
	{ -0.96852209427440084000, -0.24892760574571604000 },
	{ -0.97003125319452743000, -0.24298017990325985000 },
	{ -0.97150389098623513000, -0.23702360599436326000 },
	{ -0.97293995220554341000, -0.23105810828066725000 },
	{ -0.97433938278555909000, -0.22508391135978906000 },
	{ -0.97570213003851169000, -0.21910124015686613000 },
	{ -0.97702814265773752000, -0.21311031991608781000 },
	{ -0.97831737071961078000, -0.20711137619221509000 },
	{ -0.97956976568542364000, -0.20110463484208857000 },
	{ -0.98078528040321344000, -0.19509032201612503000 },
	{ -0.98196386910953815000, -0.18906866414980306000 },
	{ -0.98310548743119908000, -0.18303988795513790000 },
	{ -0.98421009238691171000, -0.17700422041214578000 },
	{ -0.98527764238892390000, -0.17096188876029833000 },
	{ -0.98630809724458124000, -0.16491312048996715000 },
	{ -0.98730141815784100000, -0.15885814333385878000 },
	{ -0.98825756773073214000, -0.15279718525844088000 },
	{ -0.98917650996476358000, -0.14673047445535931000 },
	{ -0.99005821026227969000, -0.14065823933284688000 },
	{ -0.99090263542776258000, -0.13458070850712395000 },
	{ -0.99170975366908198000, -0.12849811079379103000 },
	{ -0.99247953459869243000, -0.12241067519921417000 },
	{ -0.99321194923477696000, -0.11631863091190284000 },
	{ -0.99390697000233841000, -0.11022220729388123000 },
	{ -0.99456457073423776000, -0.10412163387205285000 },
	{ -0.99518472667217917000, -0.09801714032955898000 },
	{ -0.99576741446764205000, -0.09190895649713121100 },
	{ -0.99631261218276024000, -0.08579731234443847800 },
	{ -0.99682029929114790000, -0.07968243797142882100 },
	{ -0.99729045667867233000, -0.07356456359966621900 },
	{ -0.99772306664417376000, -0.06744391956366295500 },
	{ -0.99811811290013142000, -0.06132073630220757900 },
	{ -0.99847558057327690000, -0.05519524434968905300 },
	{ -0.99879545620515442000, -0.04906767432741723800 },
	{ -0.99907772775262738000, -0.04293825693494015400 },
	{ -0.99932238458833145000, -0.03680722294135827000 },
	{ -0.99952941750107505000, -0.03067480317663617500 },
	{ -0.99969881869618604000, -0.02454122852291194800 },
	{ -0.99983058179580520000, -0.01840672990580459100 },
	{ -0.99992470183912630000, -0.01227153828571980700 },
	{ -0.99998117528258290000, -0.00613588464915446830 },
	{ -0.99999999999998168000, -0.00000000000000010495 },
};
#endif /* FFTSizeFilters == 1024 */
#if FFTSizeFilters == 1024
/* FFT/IFFT w coefficients tables for 1024 elements */
static const FLASHMEM struct Complex wp [512] = 
{
	{ +0.99998117528260111000, +0.00613588464915447530 },
	{ +0.99992470183914450000, +0.01227153828571992500 },
	{ +0.99983058179582329000, +0.01840672990580482000 },
	{ +0.99969881869620414000, +0.02454122852291228800 },
	{ +0.99952941750109314000, +0.03067480317663662600 },
	{ +0.99932238458834943000, +0.03680722294135883200 },
	{ +0.99907772775264536000, +0.04293825693494082000 },
	{ +0.99879545620517241000, +0.04906767432741800800 },
	{ +0.99847558057329466000, +0.05519524434968993400 },
	{ +0.99811811290014907000, +0.06132073630220857100 },
	{ +0.99772306664419141000, +0.06744391956366405100 },
	{ +0.99729045667868998000, +0.07356456359966741200 },
	{ +0.99682029929116545000, +0.07968243797143009800 },
	{ +0.99631261218277778000, +0.08579731234443985200 },
	{ +0.99576741446765960000, +0.09190895649713269600 },
	{ +0.99518472667219660000, +0.09801714032956056200 },
	{ +0.99456457073425508000, +0.10412163387205453000 },
	{ +0.99390697000235573000, +0.11022220729388300000 },
	{ +0.99321194923479417000, +0.11631863091190471000 },
	{ +0.99247953459870952000, +0.12241067519921613000 },
	{ +0.99170975366909897000, +0.12849811079379309000 },
	{ +0.99090263542777945000, +0.13458070850712611000 },
	{ +0.99005821026229646000, +0.14065823933284916000 },
	{ +0.98917650996478035000, +0.14673047445536169000 },
	{ +0.98825756773074891000, +0.15279718525844335000 },
	{ +0.98730141815785777000, +0.15885814333386136000 },
	{ +0.98630809724459800000, +0.16491312048996984000 },
	{ +0.98527764238894056000, +0.17096188876030113000 },
	{ +0.98421009238692836000, +0.17700422041214867000 },
	{ +0.98310548743121562000, +0.18303988795514084000 },
	{ +0.98196386910955447000, +0.18906866414980608000 },
	{ +0.98078528040322954000, +0.19509032201612814000 },
	{ +0.97956976568543952000, +0.20110463484209176000 },
	{ +0.97831737071962654000, +0.20711137619221839000 },
	{ +0.97702814265775317000, +0.21311031991609122000 },
	{ +0.97570213003852735000, +0.21910124015686963000 },
	{ +0.97433938278557464000, +0.22508391135979267000 },
	{ +0.97293995220555884000, +0.23105810828067094000 },
	{ +0.97150389098625045000, +0.23702360599436700000 },
	{ +0.97003125319454264000, +0.24298017990326365000 },
	{ +0.96852209427441593000, +0.24892760574571990000 },
	{ +0.96697647104485074000, +0.25486565960451429000 },
	{ +0.96539444169768796000, +0.26079411791527524000 },
	{ +0.96377606579543840000, +0.26671275747489809000 },
	{ +0.96212140426904003000, +0.27262135544994864000 },
	{ +0.96043051941556423000, +0.27851968938505273000 },
	{ +0.95870347489586993000, +0.28440753721127143000 },
	{ +0.95694033573220716000, +0.29028467725446194000 },
	{ +0.95514116830576890000, +0.29615088824362334000 },
	{ +0.95330604035419197000, +0.30200594931922758000 },
	{ +0.95143502096900645000, +0.30784964004153437000 },
	{ +0.94952818059303479000, +0.31368174039889096000 },
	{ +0.94758559101773920000, +0.31950203081601514000 },
	{ +0.94560732538051928000, +0.32531029216226237000 },
	{ +0.94359345816195828000, +0.33110630575987582000 },
	{ +0.94154406518301870000, +0.33688985339221944000 },
	{ +0.93945922360218781000, +0.34266071731199377000 },
	{ +0.93733901191257274000, +0.34841868024943390000 },
	{ +0.93518350993894539000, +0.35416352542048968000 },
	{ +0.93299279883473674000, +0.35989503653498739000 },
	{ +0.93076696107898149000, +0.36561299780477313000 },
	{ +0.92850608047321326000, +0.37131719395183677000 },
	{ +0.92621024213830905000, +0.37700741021641748000 },
	{ +0.92387953251128452000, +0.38268343236508900000 },
	{ +0.92151403934203968000, +0.38834504669882552000 },
	{ +0.91911385169005544000, +0.39399204006104732000 },
	{ +0.91667905992104037000, +0.39962419984564601000 },
	{ +0.91420975570352825000, +0.40524131400498903000 },
	{ +0.91170603200542744000, +0.41084317105790308000 },
	{ +0.90916798309051994000, +0.41642956009763626000 },
	{ +0.90659570451491289000, +0.42200027079979874000 },
	{ +0.90398929312344090000, +0.42755509343028114000 },
	{ +0.90134884704601959000, +0.43309381885315101000 },
	{ +0.89867446569395137000, +0.43861623853852666000 },
	{ +0.89596624975618266000, +0.44412214457042820000 },
	{ +0.89322430119551277000, +0.44961132965460554000 },
	{ +0.89044872324475532000, +0.45508358712634273000 },
	{ +0.88763962040285138000, +0.46053871095823889000 },
	{ +0.88479709843093524000, +0.46597649576796502000 },
	{ +0.88192126434835250000, +0.47139673682599648000 },
	{ +0.87901222642863086000, +0.47679923006332092000 },
	{ +0.87607009419540394000, +0.48218377207912150000 },
	{ +0.87309497841828743000, +0.48755016014843466000 },
	{ +0.87008699110870868000, +0.49289819222978271000 },
	{ +0.86704624551568998000, +0.49822766697278048000 },
	{ +0.86397285612158403000, +0.50353838372571613000 },
	{ +0.86086693863776453000, +0.50883014254310555000 },
	{ +0.85772861000026934000, +0.51410274419322022000 },
	{ +0.85455798836539776000, +0.51935599016558809000 },
	{ +0.85135519310526231000, +0.52458968267846728000 },
	{ +0.84812034480329446000, +0.52980362468629294000 },
	{ +0.84485356524970423000, +0.53499761988709549000 },
	{ +0.84155497743689556000, +0.54017147272989119000 },
	{ +0.83822470555483519000, +0.54532498842204469000 },
	{ +0.83486287498637712000, +0.55045797293660303000 },
	{ +0.83146961230254224000, +0.55557023301960040000 },
	{ +0.82804504525775269000, +0.56066157619733414000 },
	{ +0.82458930278502218000, +0.56573181078361123000 },
	{ +0.82110251499110154000, +0.57078074588696526000 },
	{ +0.81758481315158049000, +0.57580819141784323000 },
	{ +0.81403632970594508000, +0.58081395809576242000 },
	{ +0.81045719825259144000, +0.58579785745643664000 },
	{ +0.80684755354379600000, +0.59075970185887194000 },
	{ +0.80320753148064161000, +0.59569930449243103000 },
	{ +0.79953726910790168000, +0.60061647938386664000 },
	{ +0.79583690460888024000, +0.60551104140432321000 },
	{ +0.79210657730020895000, +0.61038280627630703000 },
	{ +0.78834642762660290000, +0.61523159058062438000 },
	{ +0.78455659715557191000, +0.62005721176328665000 },
	{ +0.78073722857209116000, +0.62485948814238379000 },
	{ +0.77688846567322911000, +0.62963823891492443000 },
	{ +0.77301045336273366000, +0.63439328416364293000 },
	{ +0.76910333764557626000, +0.63912444486377318000 },
	{ +0.76516726562245552000, +0.64383154288978883000 },
	{ +0.76120238548425834000, +0.64851440102210978000 },
	{ +0.75720884650648113000, +0.65317284295377409000 },
	{ +0.75318679904360908000, +0.65780669329707586000 },
	{ +0.74913639452345593000, +0.66241577759016890000 },
	{ +0.74505778544146251000, +0.66699992230363458000 },
	{ +0.74095112535495555000, +0.67155895484701544000 },
	{ +0.73681656887736624000, +0.67609270357531293000 },
	{ +0.73265427167240915000, +0.68060099779544991000 },
	{ +0.72846439044822142000, +0.68508366777269725000 },
	{ +0.72424708295146312000, +0.68954054473706372000 },
	{ +0.72000250796137788000, +0.69397146088965078000 },
	{ +0.71573082528381482000, +0.69837624940896958000 },
	{ +0.71143219574521266000, +0.70275474445722197000 },
	{ +0.70710678118654369000, +0.70710678118654413000 },
	{ +0.70275474445722153000, +0.71143219574521299000 },
	{ +0.69837624940896914000, +0.71573082528381515000 },
	{ +0.69397146088965034000, +0.72000250796137810000 },
	{ +0.68954054473706328000, +0.72424708295146334000 },
	{ +0.68508366777269680000, +0.72846439044822153000 },
	{ +0.68060099779544947000, +0.73265427167240904000 },
	{ +0.67609270357531237000, +0.73681656887736602000 },
	{ +0.67155895484701478000, +0.74095112535495522000 },
	{ +0.66699992230363392000, +0.74505778544146195000 },
	{ +0.66241577759016812000, +0.74913639452345526000 },
	{ +0.65780669329707497000, +0.75318679904360841000 },
	{ +0.65317284295377309000, +0.75720884650648035000 },
	{ +0.64851440102210878000, +0.76120238548425756000 },
	{ +0.64383154288978783000, +0.76516726562245463000 },
	{ +0.63912444486377218000, +0.76910333764557526000 },
	{ +0.63439328416364194000, +0.77301045336273255000 },
	{ +0.62963823891492343000, +0.77688846567322800000 },
	{ +0.62485948814238279000, +0.78073722857209005000 },
	{ +0.62005721176328554000, +0.78455659715557080000 },
	{ +0.61523159058062327000, +0.78834642762660179000 },
	{ +0.61038280627630581000, +0.79210657730020784000 },
	{ +0.60551104140432188000, +0.79583690460887901000 },
	{ +0.60061647938386531000, +0.79953726910790046000 },
	{ +0.59569930449242969000, +0.80320753148064028000 },
	{ +0.59075970185887061000, +0.80684755354379456000 },
	{ +0.58579785745643531000, +0.81045719825259011000 },
	{ +0.58081395809576097000, +0.81403632970594364000 },
	{ +0.57580819141784179000, +0.81758481315157894000 },
	{ +0.57078074588696381000, +0.82110251499109987000 },
	{ +0.56573181078360968000, +0.82458930278502041000 },
	{ +0.56066157619733248000, +0.82804504525775080000 },
	{ +0.55557023301959862000, +0.83146961230254024000 },
	{ +0.55045797293660126000, +0.83486287498637501000 },
	{ +0.54532498842204280000, +0.83822470555483297000 },
	{ +0.54017147272988930000, +0.84155497743689323000 },
	{ +0.53499761988709360000, +0.84485356524970179000 },
	{ +0.52980362468629105000, +0.84812034480329190000 },
	{ +0.52458968267846529000, +0.85135519310525976000 },
	{ +0.51935599016558598000, +0.85455798836539509000 },
	{ +0.51410274419321811000, +0.85772861000026657000 },
	{ +0.50883014254310344000, +0.86086693863776176000 },
	{ +0.50353838372571402000, +0.86397285612158115000 },
	{ +0.49822766697277832000, +0.86704624551568699000 },
	{ +0.49289819222978054000, +0.87008699110870569000 },
	{ +0.48755016014843244000, +0.87309497841828432000 },
	{ +0.48218377207911922000, +0.87607009419540083000 },
	{ +0.47679923006331865000, +0.87901222642862764000 },
	{ +0.47139673682599414000, +0.88192126434834917000 },
	{ +0.46597649576796268000, +0.88479709843093179000 },
	{ +0.46053871095823656000, +0.88763962040284794000 },
	{ +0.45508358712634039000, +0.89044872324475188000 },
	{ +0.44961132965460321000, +0.89322430119550933000 },
	{ +0.44412214457042587000, +0.89596624975617911000 },
	{ +0.43861623853852433000, +0.89867446569394771000 },
	{ +0.43309381885314868000, +0.90134884704601592000 },
	{ +0.42755509343027881000, +0.90398929312343712000 },
	{ +0.42200027079979646000, +0.90659570451490912000 },
	{ +0.41642956009763399000, +0.90916798309051605000 },
	{ +0.41084317105790075000, +0.91170603200542344000 },
	{ +0.40524131400498670000, +0.91420975570352425000 },
	{ +0.39962419984564368000, +0.91667905992103627000 },
	{ +0.39399204006104499000, +0.91911385169005122000 },
	{ +0.38834504669882319000, +0.92151403934203535000 },
	{ +0.38268343236508673000, +0.92387953251128019000 },
	{ +0.37700741021641526000, +0.92621024213830472000 },
	{ +0.37131719395183460000, +0.92850608047320893000 },
	{ +0.36561299780477097000, +0.93076696107897705000 },
	{ +0.35989503653498528000, +0.93299279883473218000 },
	{ +0.35416352542048757000, +0.93518350993894084000 },
	{ +0.34841868024943173000, +0.93733901191256819000 },
	{ +0.34266071731199160000, +0.93945922360218315000 },
	{ +0.33688985339221728000, +0.94154406518301392000 },
	{ +0.33110630575987365000, +0.94359345816195339000 },
	{ +0.32531029216226021000, +0.94560732538051440000 },
	{ +0.31950203081601297000, +0.94758559101773421000 },
	{ +0.31368174039888880000, +0.94952818059302968000 },
	{ +0.30784964004153226000, +0.95143502096900134000 },
	{ +0.30200594931922548000, +0.95330604035418676000 },
	{ +0.29615088824362124000, +0.95514116830576357000 },
	{ +0.29028467725445983000, +0.95694033573220172000 },
	{ +0.28440753721126932000, +0.95870347489586427000 },
	{ +0.27851968938505062000, +0.96043051941555846000 },
	{ +0.27262135544994653000, +0.96212140426903414000 },
	{ +0.26671275747489598000, +0.96377606579543229000 },
	{ +0.26079411791527313000, +0.96539444169768174000 },
	{ +0.25486565960451218000, +0.96697647104484441000 },
	{ +0.24892760574571782000, +0.96852209427440950000 },
	{ +0.24298017990326157000, +0.97003125319453609000 },
	{ +0.23702360599436492000, +0.97150389098624379000 },
	{ +0.23105810828066886000, +0.97293995220555207000 },
	{ +0.22508391135979061000, +0.97433938278556775000 },
	{ +0.21910124015686763000, +0.97570213003852035000 },
	{ +0.21311031991608925000, +0.97702814265774618000 },
	{ +0.20711137619221648000, +0.97831737071961944000 },
	{ +0.20110463484208990000, +0.97956976568543230000 },
	{ +0.19509032201612631000, +0.98078528040322221000 },
	{ +0.18906866414980428000, +0.98196386910954692000 },
	{ +0.18303988795513906000, +0.98310548743120796000 },
	{ +0.17700422041214689000, +0.98421009238692070000 },
	{ +0.17096188876029939000, +0.98527764238893289000 },
	{ +0.16491312048996815000, +0.98630809724459023000 },
	{ +0.15885814333385972000, +0.98730141815785000000 },
	{ +0.15279718525844177000, +0.98825756773074114000 },
	{ +0.14673047445536014000, +0.98917650996477258000 },
	{ +0.14065823933284766000, +0.99005821026228868000 },
	{ +0.13458070850712467000, +0.99090263542777157000 },
	{ +0.12849811079379170000, +0.99170975366909098000 },
	{ +0.12241067519921478000, +0.99247953459870142000 },
	{ +0.11631863091190339000, +0.99321194923478595000 },
	{ +0.11022220729388173000, +0.99390697000234740000 },
	{ +0.10412163387205330000, +0.99456457073424676000 },
	{ +0.09801714032955936900, +0.99518472667218816000 },
	{ +0.09190895649713154400, +0.99576741446765105000 },
	{ +0.08579731234443875600, +0.99631261218276923000 },
	{ +0.07968243797142904300, +0.99682029929115690000 },
	{ +0.07356456359966638500, +0.99729045667868133000 },
	{ +0.06744391956366306600, +0.99772306664418275000 },
	{ +0.06132073630220763500, +0.99811811290014041000 },
	{ +0.05519524434968905300, +0.99847558057328589000 },
	{ +0.04906767432741718200, +0.99879545620516341000 },
	{ +0.04293825693494004300, +0.99907772775263637000 },
	{ +0.03680722294135811000, +0.99932238458834044000 },
	{ +0.03067480317663596000, +0.99952941750108404000 },
	{ +0.02454122852291167800, +0.99969881869619504000 },
	{ +0.01840672990580426900, +0.99983058179581419000 },
	{ +0.01227153828571943100, +0.99992470183913529000 },
	{ +0.00613588464915403730, +0.99998117528259189000 },
	{ -0.00000000000000038164, +0.99999999999999067000 },
	{ -0.00613588464915479970, +0.99998117528259178000 },
	{ -0.01227153828572019300, +0.99992470183913518000 },
	{ -0.01840672990580503200, +0.99983058179581397000 },
	{ -0.02454122852291244100, +0.99969881869619481000 },
	{ -0.03067480317663672000, +0.99952941750108371000 },
	{ -0.03680722294135886600, +0.99932238458834000000 },
	{ -0.04293825693494079900, +0.99907772775263592000 },
	{ -0.04906767432741793200, +0.99879545620516297000 },
	{ -0.05519524434968979500, +0.99847558057328523000 },
	{ -0.06132073630220837700, +0.99811811290013963000 },
	{ -0.06744391956366380100, +0.99772306664418198000 },
	{ -0.07356456359966710700, +0.99729045667868055000 },
	{ -0.07968243797142973700, +0.99682029929115601000 },
	{ -0.08579731234443943600, +0.99631261218276834000 },
	{ -0.09190895649713222400, +0.99576741446765016000 },
	{ -0.09801714032956003500, +0.99518472667218716000 },
	{ -0.10412163387205395000, +0.99456457073424565000 },
	{ -0.11022220729388237000, +0.99390697000234629000 },
	{ -0.11631863091190400000, +0.99321194923478484000 },
	{ -0.12241067519921536000, +0.99247953459870020000 },
	{ -0.12849811079379228000, +0.99170975366908964000 },
	{ -0.13458070850712525000, +0.99090263542777013000 },
	{ -0.14065823933284824000, +0.99005821026228713000 },
	{ -0.14673047445536072000, +0.98917650996477102000 },
	{ -0.15279718525844233000, +0.98825756773073958000 },
	{ -0.15885814333386028000, +0.98730141815784844000 },
	{ -0.16491312048996870000, +0.98630809724458868000 },
	{ -0.17096188876029994000, +0.98527764238893123000 },
	{ -0.17700422041214742000, +0.98421009238691903000 },
	{ -0.18303988795513954000, +0.98310548743120629000 },
	{ -0.18906866414980472000, +0.98196386910954514000 },
	{ -0.19509032201612669000, +0.98078528040322022000 },
	{ -0.20110463484209026000, +0.97956976568543030000 },
	{ -0.20711137619221684000, +0.97831737071961733000 },
	{ -0.21311031991608961000, +0.97702814265774396000 },
	{ -0.21910124015686797000, +0.97570213003851813000 },
	{ -0.22508391135979094000, +0.97433938278556542000 },
	{ -0.23105810828066917000, +0.97293995220554974000 },
	{ -0.23702360599436517000, +0.97150389098624135000 },
	{ -0.24298017990326176000, +0.97003125319453354000 },
	{ -0.24892760574571796000, +0.96852209427440683000 },
	{ -0.25486565960451230000, +0.96697647104484163000 },
	{ -0.26079411791527318000, +0.96539444169767885000 },
	{ -0.26671275747489598000, +0.96377606579542940000 },
	{ -0.27262135544994648000, +0.96212140426903103000 },
	{ -0.27851968938505051000, +0.96043051941555524000 },
	{ -0.28440753721126916000, +0.95870347489586094000 },
	{ -0.29028467725445961000, +0.95694033573219817000 },
	{ -0.29615088824362096000, +0.95514116830576001000 },
	{ -0.30200594931922514000, +0.95330604035418309000 },
	{ -0.30784964004153187000, +0.95143502096899757000 },
	{ -0.31368174039888841000, +0.94952818059302591000 },
	{ -0.31950203081601253000, +0.94758559101773032000 },
	{ -0.32531029216225971000, +0.94560732538051040000 },
	{ -0.33110630575987310000, +0.94359345816194939000 },
	{ -0.33688985339221666000, +0.94154406518300982000 },
	{ -0.34266071731199094000, +0.93945922360217893000 },
	{ -0.34841868024943101000, +0.93733901191256386000 },
	{ -0.35416352542048674000, +0.93518350993893651000 },
	{ -0.35989503653498439000, +0.93299279883472785000 },
	{ -0.36561299780477008000, +0.93076696107897261000 },
	{ -0.37131719395183366000, +0.92850608047320438000 },
	{ -0.37700741021641432000, +0.92621024213830017000 },
	{ -0.38268343236508578000, +0.92387953251127564000 },
	{ -0.38834504669882225000, +0.92151403934203080000 },
	{ -0.39399204006104399000, +0.91911385169004667000 },
	{ -0.39962419984564262000, +0.91667905992103160000 },
	{ -0.40524131400498559000, +0.91420975570351959000 },
	{ -0.41084317105789958000, +0.91170603200541878000 },
	{ -0.41642956009763271000, +0.90916798309051128000 },
	{ -0.42200027079979513000, +0.90659570451490423000 },
	{ -0.42755509343027748000, +0.90398929312343224000 },
	{ -0.43309381885314729000, +0.90134884704601093000 },
	{ -0.43861623853852288000, +0.89867446569394271000 },
	{ -0.44412214457042437000, +0.89596624975617412000 },
	{ -0.44961132965460171000, +0.89322430119550433000 },
	{ -0.45508358712633884000, +0.89044872324474700000 },
	{ -0.46053871095823495000, +0.88763962040284305000 },
	{ -0.46597649576796102000, +0.88479709843092691000 },
	{ -0.47139673682599242000, +0.88192126434834417000 },
	{ -0.47679923006331681000, +0.87901222642862265000 },
	{ -0.48218377207911733000, +0.87607009419539572000 },
	{ -0.48755016014843050000, +0.87309497841827921000 },
	{ -0.49289819222977849000, +0.87008699110870058000 },
	{ -0.49822766697277621000, +0.86704624551568188000 },
	{ -0.50353838372571180000, +0.86397285612157604000 },
	{ -0.50883014254310122000, +0.86086693863775665000 },
	{ -0.51410274419321589000, +0.85772861000026146000 },
	{ -0.51935599016558365000, +0.85455798836538988000 },
	{ -0.52458968267846284000, +0.85135519310525443000 },
	{ -0.52980362468628850000, +0.84812034480328657000 },
	{ -0.53499761988709094000, +0.84485356524969635000 },
	{ -0.54017147272988653000, +0.84155497743688767000 },
	{ -0.54532498842203991000, +0.83822470555482731000 },
	{ -0.55045797293659826000, +0.83486287498636935000 },
	{ -0.55557023301959552000, +0.83146961230253458000 },
	{ -0.56066157619732926000, +0.82804504525774503000 },
	{ -0.56573181078360635000, +0.82458930278501463000 },
	{ -0.57078074588696037000, +0.82110251499109399000 },
	{ -0.57580819141783823000, +0.81758481315157294000 },
	{ -0.58081395809575742000, +0.81403632970593764000 },
	{ -0.58579785745643165000, +0.81045719825258411000 },
	{ -0.59075970185886695000, +0.80684755354378868000 },
	{ -0.59569930449242603000, +0.80320753148063440000 },
	{ -0.60061647938386153000, +0.79953726910789458000 },
	{ -0.60551104140431800000, +0.79583690460887313000 },
	{ -0.61038280627630181000, +0.79210657730020195000 },
	{ -0.61523159058061916000, +0.78834642762659590000 },
	{ -0.62005721176328132000, +0.78455659715556492000 },
	{ -0.62485948814237846000, +0.78073722857208416000 },
	{ -0.62963823891491899000, +0.77688846567322223000 },
	{ -0.63439328416363738000, +0.77301045336272678000 },
	{ -0.63912444486376752000, +0.76910333764556948000 },
	{ -0.64383154288978317000, +0.76516726562244874000 },
	{ -0.64851440102210411000, +0.76120238548425156000 },
	{ -0.65317284295376843000, +0.75720884650647435000 },
	{ -0.65780669329707020000, +0.75318679904360231000 },
	{ -0.66241577759016324000, +0.74913639452344916000 },
	{ -0.66699992230362892000, +0.74505778544145584000 },
	{ -0.67155895484700967000, +0.74095112535494900000 },
	{ -0.67609270357530715000, +0.73681656887735980000 },
	{ -0.68060099779544414000, +0.73265427167240282000 },
	{ -0.68508366777269136000, +0.72846439044821520000 },
	{ -0.68954054473705784000, +0.72424708295145690000 },
	{ -0.69397146088964490000, +0.72000250796137166000 },
	{ -0.69837624940896370000, +0.71573082528380871000 },
	{ -0.70275474445721609000, +0.71143219574520655000 },
	{ -0.70710678118653825000, +0.70710678118653769000 },
	{ -0.71143219574520711000, +0.70275474445721553000 },
	{ -0.71573082528380916000, +0.69837624940896315000 },
	{ -0.72000250796137211000, +0.69397146088964434000 },
	{ -0.72424708295145723000, +0.68954054473705728000 },
	{ -0.72846439044821543000, +0.68508366777269081000 },
	{ -0.73265427167240293000, +0.68060099779544347000 },
	{ -0.73681656887735980000, +0.67609270357530649000 },
	{ -0.74095112535494889000, +0.67155895484700889000 },
	{ -0.74505778544145562000, +0.66699992230362803000 },
	{ -0.74913639452344882000, +0.66241577759016235000 },
	{ -0.75318679904360186000, +0.65780669329706931000 },
	{ -0.75720884650647380000, +0.65317284295376743000 },
	{ -0.76120238548425101000, +0.64851440102210312000 },
	{ -0.76516726562244808000, +0.64383154288978217000 },
	{ -0.76910333764556871000, +0.63912444486376652000 },
	{ -0.77301045336272589000, +0.63439328416363627000 },
	{ -0.77688846567322123000, +0.62963823891491788000 },
	{ -0.78073722857208327000, +0.62485948814237735000 },
	{ -0.78455659715556392000, +0.62005721176328021000 },
	{ -0.78834642762659490000, +0.61523159058061794000 },
	{ -0.79210657730020095000, +0.61038280627630059000 },
	{ -0.79583690460887213000, +0.60551104140431666000 },
	{ -0.79953726910789347000, +0.60061647938386009000 },
	{ -0.80320753148063329000, +0.59569930449242459000 },
	{ -0.80684755354378757000, +0.59075970185886562000 },
	{ -0.81045719825258300000, +0.58579785745643032000 },
	{ -0.81403632970593653000, +0.58081395809575598000 },
	{ -0.81758481315157172000, +0.57580819141783679000 },
	{ -0.82110251499109255000, +0.57078074588695882000 },
	{ -0.82458930278501297000, +0.56573181078360479000 },
	{ -0.82804504525774336000, +0.56066157619732770000 },
	{ -0.83146961230253280000, +0.55557023301959396000 },
	{ -0.83486287498636746000, +0.55045797293659660000 },
	{ -0.83822470555482531000, +0.54532498842203825000 },
	{ -0.84155497743688557000, +0.54017147272988475000 },
	{ -0.84485356524969413000, +0.53499761988708916000 },
	{ -0.84812034480328424000, +0.52980362468628672000 },
	{ -0.85135519310525209000, +0.52458968267846107000 },
	{ -0.85455798836538743000, +0.51935599016558176000 },
	{ -0.85772861000025891000, +0.51410274419321389000 },
	{ -0.86086693863775399000, +0.50883014254309933000 },
	{ -0.86397285612157337000, +0.50353838372570991000 },
	{ -0.86704624551567921000, +0.49822766697277426000 },
	{ -0.87008699110869792000, +0.49289819222977654000 },
	{ -0.87309497841827655000, +0.48755016014842850000 },
	{ -0.87607009419539295000, +0.48218377207911534000 },
	{ -0.87901222642861976000, +0.47679923006331482000 },
	{ -0.88192126434834117000, +0.47139673682599037000 },
	{ -0.88479709843092380000, +0.46597649576795896000 },
	{ -0.88763962040283995000, +0.46053871095823290000 },
	{ -0.89044872324474389000, +0.45508358712633679000 },
	{ -0.89322430119550134000, +0.44961132965459966000 },
	{ -0.89596624975617112000, +0.44412214457042232000 },
	{ -0.89867446569393972000, +0.43861623853852083000 },
	{ -0.90134884704600782000, +0.43309381885314524000 },
	{ -0.90398929312342902000, +0.42755509343027542000 },
	{ -0.90659570451490101000, +0.42200027079979313000 },
	{ -0.90916798309050795000, +0.41642956009763071000 },
	{ -0.91170603200541533000, +0.41084317105789753000 },
	{ -0.91420975570351604000, +0.40524131400498353000 },
	{ -0.91667905992102805000, +0.39962419984564057000 },
	{ -0.91911385169004300000, +0.39399204006104194000 },
	{ -0.92151403934202714000, +0.38834504669882020000 },
	{ -0.92387953251127186000, +0.38268343236508373000 },
	{ -0.92621024213829639000, +0.37700741021641232000 },
	{ -0.92850608047320060000, +0.37131719395183171000 },
	{ -0.93076696107896872000, +0.36561299780476814000 },
	{ -0.93299279883472375000, +0.35989503653498245000 },
	{ -0.93518350993893229000, +0.35416352542048479000 },
	{ -0.93733901191255964000, +0.34841868024942901000 },
	{ -0.93945922360217460000, +0.34266071731198894000 },
	{ -0.94154406518300537000, +0.33688985339221467000 },
	{ -0.94359345816194484000, +0.33110630575987110000 },
	{ -0.94560732538050585000, +0.32531029216225771000 },
	{ -0.94758559101772566000, +0.31950203081601053000 },
	{ -0.94952818059302102000, +0.31368174039888641000 },
	{ -0.95143502096899268000, +0.30784964004152993000 },
	{ -0.95330604035417810000, +0.30200594931922320000 },
	{ -0.95514116830575491000, +0.29615088824361901000 },
	{ -0.95694033573219306000, +0.29028467725445767000 },
	{ -0.95870347489585561000, +0.28440753721126721000 },
	{ -0.96043051941554980000, +0.27851968938504856000 },
	{ -0.96212140426902548000, +0.27262135544994454000 },
	{ -0.96377606579542363000, +0.26671275747489404000 },
	{ -0.96539444169767308000, +0.26079411791527124000 },
	{ -0.96697647104483575000, +0.25486565960451035000 },
	{ -0.96852209427440084000, +0.24892760574571604000 },
	{ -0.97003125319452743000, +0.24298017990325985000 },
	{ -0.97150389098623513000, +0.23702360599436326000 },
	{ -0.97293995220554341000, +0.23105810828066725000 },
	{ -0.97433938278555909000, +0.22508391135978906000 },
	{ -0.97570213003851169000, +0.21910124015686613000 },
	{ -0.97702814265773752000, +0.21311031991608781000 },
	{ -0.97831737071961078000, +0.20711137619221509000 },
	{ -0.97956976568542364000, +0.20110463484208857000 },
	{ -0.98078528040321344000, +0.19509032201612503000 },
	{ -0.98196386910953815000, +0.18906866414980306000 },
	{ -0.98310548743119908000, +0.18303988795513790000 },
	{ -0.98421009238691171000, +0.17700422041214578000 },
	{ -0.98527764238892390000, +0.17096188876029833000 },
	{ -0.98630809724458124000, +0.16491312048996715000 },
	{ -0.98730141815784100000, +0.15885814333385878000 },
	{ -0.98825756773073214000, +0.15279718525844088000 },
	{ -0.98917650996476358000, +0.14673047445535931000 },
	{ -0.99005821026227969000, +0.14065823933284688000 },
	{ -0.99090263542776258000, +0.13458070850712395000 },
	{ -0.99170975366908198000, +0.12849811079379103000 },
	{ -0.99247953459869243000, +0.12241067519921417000 },
	{ -0.99321194923477696000, +0.11631863091190284000 },
	{ -0.99390697000233841000, +0.11022220729388123000 },
	{ -0.99456457073423776000, +0.10412163387205285000 },
	{ -0.99518472667217917000, +0.09801714032955898000 },
	{ -0.99576741446764205000, +0.09190895649713121100 },
	{ -0.99631261218276024000, +0.08579731234443847800 },
	{ -0.99682029929114790000, +0.07968243797142882100 },
	{ -0.99729045667867233000, +0.07356456359966621900 },
	{ -0.99772306664417376000, +0.06744391956366295500 },
	{ -0.99811811290013142000, +0.06132073630220757900 },
	{ -0.99847558057327690000, +0.05519524434968905300 },
	{ -0.99879545620515442000, +0.04906767432741723800 },
	{ -0.99907772775262738000, +0.04293825693494015400 },
	{ -0.99932238458833145000, +0.03680722294135827000 },
	{ -0.99952941750107505000, +0.03067480317663617500 },
	{ -0.99969881869618604000, +0.02454122852291194800 },
	{ -0.99983058179580520000, +0.01840672990580459100 },
	{ -0.99992470183912630000, +0.01227153828571980700 },
	{ -0.99998117528258290000, +0.00613588464915446830 },
	{ -0.99999999999998168000, +0.00000000000000010495 },
};
#endif /* FFTSizeFilters == 1024 */
#if FFTSizeFilters == 2048
/* FFT/IFFT w coefficients tables for 2048 elements */
static const FLASHMEM struct Complex wm [1024] = 
{
	{ +0.99999529380957619000, -0.00306795676296597610 },
	{ +0.99998117528260122000, -0.00613588464915447530 },
	{ +0.99995764455196390000, -0.00920375478205981940 },
	{ +0.99992470183914461000, -0.01227153828571992700 },
	{ +0.99988234745421267000, -0.01533920628498810200 },
	{ +0.99983058179582363000, -0.01840672990580482400 },
	{ +0.99976940535121550000, -0.02147408027546950800 },
	{ +0.99969881869620447000, -0.02454122852291228800 },
	{ +0.99961882249517886000, -0.02760814577896574000 },
	{ +0.99952941750109348000, -0.03067480317663662600 },
	{ +0.99943060455546207000, -0.03374117185137758700 },
	{ +0.99932238458834977000, -0.03680722294135883200 },
	{ +0.99920475861836422000, -0.03987292758773981100 },
	{ +0.99907772775264569000, -0.04293825693494082000 },
	{ +0.99894129318685720000, -0.04600318213091463000 },
	{ +0.99879545620517274000, -0.04906767432741801500 },
	{ +0.99864021818026549000, -0.05213170468028332400 },
	{ +0.99847558057329500000, -0.05519524434968994800 },
	{ +0.99830154493389311000, -0.05825826450043576600 },
	{ +0.99811811290014951000, -0.06132073630220858500 },
	{ +0.99792528619859633000, -0.06438263092985746500 },
	{ +0.99772306664419197000, -0.06744391956366406500 },
	{ +0.99751145614030390000, -0.07050457338961387000 },
	{ +0.99729045667869065000, -0.07356456359966744000 },
	{ +0.99706007033948352000, -0.07662386139203150600 },
	{ +0.99682029929116622000, -0.07968243797143014000 },
	{ +0.99657114579055539000, -0.08274026454937571900 },
	{ +0.99631261218277856000, -0.08579731234443992200 },
	{ +0.99604470090125263000, -0.08885355258252462800 },
	{ +0.99576741446766037000, -0.09190895649713275200 },
	{ +0.99548075549192749000, -0.09496349532963901900 },
	{ +0.99518472667219748000, -0.09801714032956061800 },
	{ +0.99487933079480628000, -0.10106986275482785000 },
	{ +0.99456457073425608000, -0.10412163387205461000 },
	{ +0.99424044945318868000, -0.10717242495680888000 },
	{ +0.99390697000235673000, -0.11022220729388310000 },
	{ +0.99356413552059597000, -0.11327095217756439000 },
	{ +0.99321194923479517000, -0.11631863091190481000 },
	{ +0.99285041445986577000, -0.11936521481099141000 },
	{ +0.99247953459871074000, -0.12241067519921625000 },
	{ +0.99209931314219257000, -0.12545498341154629000 },
	{ +0.99170975366910030000, -0.12849811079379322000 },
	{ +0.99131085984611622000, -0.13154002870288314000 },
	{ +0.99090263542778090000, -0.13458070850712622000 },
	{ +0.99048508425645798000, -0.13762012158648609000 },
	{ +0.99005821026229812000, -0.14065823933284929000 },
	{ +0.98962201746320189000, -0.14369503315029453000 },
	{ +0.98917650996478201000, -0.14673047445536183000 },
	{ +0.98872169196032489000, -0.14976453467732159000 },
	{ +0.98825756773075069000, -0.15279718525844352000 },
	{ +0.98778414164457340000, -0.15582839765426534000 },
	{ +0.98730141815785966000, -0.15885814333386156000 },
	{ +0.98680940181418675000, -0.16188639378011196000 },
	{ +0.98630809724459989000, -0.16491312048997006000 },
	{ +0.98579750916756870000, -0.16793829497473131000 },
	{ +0.98527764238894255000, -0.17096188876030136000 },
	{ +0.98474850180190554000, -0.17398387338746396000 },
	{ +0.98421009238693047000, -0.17700422041214889000 },
	{ +0.98366241921173170000, -0.18002290140569965000 },
	{ +0.98310548743121773000, -0.18303988795514112000 },
	{ +0.98253930228744257000, -0.18605515166344680000 },
	{ +0.98196386910955658000, -0.18906866414980636000 },
	{ +0.98137919331375589000, -0.19208039704989258000 },
	{ +0.98078528040323176000, -0.19509032201612841000 },
	{ +0.98018213596811865000, -0.19809841071795373000 },
	{ +0.97956976568544185000, -0.20110463484209207000 },
	{ +0.97894817531906353000, -0.20410896609281703000 },
	{ +0.97831737071962899000, -0.20711137619221870000 },
	{ +0.97767735782451137000, -0.21011183688046978000 },
	{ +0.97702814265775573000, -0.21311031991609153000 },
	{ +0.97636973133002247000, -0.21610679707621969000 },
	{ +0.97570213003852990000, -0.21910124015686996000 },
	{ +0.97502534506699556000, -0.22209362097320370000 },
	{ +0.97433938278557730000, -0.22508391135979303000 },
	{ +0.97364424965081342000, -0.22807208317088595000 },
	{ +0.97293995220556162000, -0.23105810828067136000 },
	{ +0.97222649707893771000, -0.23404195858354368000 },
	{ +0.97150389098625323000, -0.23702360599436748000 },
	{ +0.97077214072895179000, -0.24000302244874178000 },
	{ +0.97003125319454553000, -0.24298017990326420000 },
	{ +0.96928123535655009000, -0.24595505033579493000 },
	{ +0.96852209427441893000, -0.24892760574572048000 },
	{ +0.96775383709347707000, -0.25189781815421730000 },
	{ +0.96697647104485374000, -0.25486565960451496000 },
	{ +0.96619000344541417000, -0.25783110216215938000 },
	{ +0.96539444169769106000, -0.26079411791527590000 },
	{ +0.96458979328981442000, -0.26375467897483179000 },
	{ +0.96377606579544162000, -0.26671275747489881000 },
	{ +0.96295326687368565000, -0.26966832557291553000 },
	{ +0.96212140426904336000, -0.27262135544994942000 },
	{ +0.96128048581132242000, -0.27557181931095864000 },
	{ +0.96043051941556756000, -0.27851968938505356000 },
	{ +0.95957151308198640000, -0.28146493792575844000 },
	{ +0.95870347489587349000, -0.28440753721127232000 },
	{ +0.95782641302753480000, -0.28734745954473001000 },
	{ +0.95694033573221071000, -0.29028467725446283000 },
	{ +0.95604525134999829000, -0.29321916269425913000 },
	{ +0.95514116830577256000, -0.29615088824362434000 },
	{ +0.95422809510910744000, -0.29907982630804097000 },
	{ +0.95330604035419564000, -0.30200594931922858000 },
	{ +0.95237501271976777000, -0.30492922973540293000 },
	{ +0.95143502096901034000, -0.30784964004153542000 },
	{ +0.95048607394948370000, -0.31076715274961209000 },
	{ +0.94952818059303867000, -0.31368174039889207000 },
	{ +0.94856134991573227000, -0.31659337555616651000 },
	{ +0.94758559101774309000, -0.31950203081601630000 },
	{ +0.94660091308328564000, -0.32240767880107046000 },
	{ +0.94560732538052339000, -0.32531029216226354000 },
	{ +0.94460483726148226000, -0.32820984357909311000 },
	{ +0.94359345816196238000, -0.33110630575987698000 },
	{ +0.94257319760144898000, -0.33399965144201005000 },
	{ +0.94154406518302292000, -0.33688985339222072000 },
	{ +0.94050607059327052000, -0.33977688440682752000 },
	{ +0.93945922360219214000, -0.34266071731199504000 },
	{ +0.93840353406311039000, -0.34554132496398970000 },
	{ +0.93733901191257718000, -0.34841868024943518000 },
	{ +0.93626566717028059000, -0.35129275608556776000 },
	{ +0.93518350993894983000, -0.35416352542049101000 },
	{ +0.93409255040426120000, -0.35703096123343070000 },
	{ +0.93299279883474118000, -0.35989503653498883000 },
	{ +0.93188426558167037000, -0.36275572436739789000 },
	{ +0.93076696107898604000, -0.36561299780477458000 },
	{ +0.92964089584318355000, -0.36846682995337304000 },
	{ +0.92850608047321781000, -0.37131719395183826000 },
	{ +0.92736252565040334000, -0.37416406297145871000 },
	{ +0.92621024213831360000, -0.37700741021641898000 },
	{ +0.92504924078267992000, -0.37984720892405194000 },
	{ +0.92387953251128907000, -0.38268343236509056000 },
	{ +0.92270112833388096000, -0.38551605384391968000 },
	{ +0.92151403934204434000, -0.38834504669882713000 },
	{ +0.92031827670911304000, -0.39117038430225476000 },
	{ +0.91911385169006021000, -0.39399204006104899000 },
	{ +0.91790077562139294000, -0.39680998741671120000 },
	{ +0.91667905992104515000, -0.39962419984564773000 },
	{ +0.91544871608827039000, -0.40243465085941937000 },
	{ +0.91420975570353313000, -0.40524131400499080000 },
	{ +0.91296219042840066000, -0.40804416286497963000 },
	{ +0.91170603200543243000, -0.41084317105790491000 },
	{ +0.91044129225806969000, -0.41363831223843556000 },
	{ +0.90916798309052493000, -0.41642956009763821000 },
	{ +0.90788611648766882000, -0.41921688836322496000 },
	{ +0.90659570451491800000, -0.42200027079980074000 },
	{ +0.90529675931812137000, -0.42477968120910992000 },
	{ +0.90398929312344589000, -0.42755509343028320000 },
	{ +0.90267331823726138000, -0.43032648134008378000 },
	{ +0.90134884704602458000, -0.43309381885315312000 },
	{ +0.90001589201616283000, -0.43585707992225664000 },
	{ +0.89867446569395648000, -0.43861623853852882000 },
	{ +0.89732458070542098000, -0.44137126873171789000 },
	{ +0.89596624975618788000, -0.44412214457043042000 },
	{ +0.89459948563138536000, -0.44686884016237544000 },
	{ +0.89322430119551799000, -0.44961132965460787000 },
	{ +0.89184070939234539000, -0.45234958723377217000 },
	{ +0.89044872324476054000, -0.45508358712634511000 },
	{ +0.88904835585466724000, -0.45781330359887851000 },
	{ +0.88763962040285660000, -0.46053871095824128000 },
	{ +0.88622253014888319000, -0.46325978355186148000 },
	{ +0.88479709843094034000, -0.46597649576796746000 },
	{ +0.88336333866573413000, -0.46868882203582918000 },
	{ +0.88192126434835760000, -0.47139673682599892000 },
	{ +0.88047088905216331000, -0.47410021465055130000 },
	{ +0.87901222642863597000, -0.47679923006332342000 },
	{ +0.87754529020726380000, -0.47949375766015434000 },
	{ +0.87607009419540915000, -0.48218377207912411000 },
	{ +0.87458665227817867000, -0.48486924800079251000 },
	{ +0.87309497841829264000, -0.48755016014843738000 },
	{ +0.87159508665595364000, -0.49022648328829260000 },
	{ +0.87008699110871401000, -0.49289819222978554000 },
	{ +0.86857070597134345000, -0.49556526182577409000 },
	{ +0.86704624551569520000, -0.49822766697278342000 },
	{ +0.86551362409057164000, -0.50088538261124238000 },
	{ +0.86397285612158925000, -0.50353838372571924000 },
	{ +0.86242395611104306000, -0.50618664534515700000 },
	{ +0.86086693863776975000, -0.50883014254310877000 },
	{ +0.85930181835701080000, -0.51146885043797219000 },
	{ +0.85772861000027445000, -0.51410274419322355000 },
	{ +0.85614732837519691000, -0.51673179901765176000 },
	{ +0.85455798836540298000, -0.51935599016559153000 },
	{ +0.85296060493036607000, -0.52197529293715628000 },
	{ +0.85135519310526753000, -0.52458968267847084000 },
	{ +0.84974176800085488000, -0.52719913478190328000 },
	{ +0.84812034480329968000, -0.52980362468629671000 },
	{ +0.84649093877405446000, -0.53240312787720012000 },
	{ +0.84485356524970945000, -0.53499761988709948000 },
	{ +0.84320823964184788000, -0.53758707629564773000 },
	{ +0.84155497743690089000, -0.54017147272989507000 },
	{ +0.83989379419600196000, -0.54275078486451811000 },
	{ +0.83822470555484041000, -0.54532498842204868000 },
	{ +0.83654772722351434000, -0.54789405917310241000 },
	{ +0.83486287498638245000, -0.55045797293660714000 },
	{ +0.83317016470191563000, -0.55301670558002991000 },
	{ +0.83146961230254768000, -0.55557023301960462000 },
	{ +0.82976123379452549000, -0.55811853122055854000 },
	{ +0.82804504525775824000, -0.56066157619733847000 },
	{ +0.82632106284566598000, -0.56319934401383653000 },
	{ +0.82458930278502784000, -0.56573181078361567000 },
	{ +0.82284978137582887000, -0.56825895267013404000 },
	{ +0.82110251499110720000, -0.57078074588696981000 },
	{ +0.81934752007679945000, -0.57329716669804476000 },
	{ +0.81758481315158626000, -0.57580819141784789000 },
	{ +0.81581441080673633000, -0.57831379641165814000 },
	{ +0.81403632970595086000, -0.58081395809576719000 },
	{ +0.81225058658520644000, -0.58330865293770096000 },
	{ +0.81045719825259732000, -0.58579785745644153000 },
	{ +0.80865618158817754000, -0.58828154822264800000 },
	{ +0.80684755354380189000, -0.59075970185887694000 },
	{ +0.80503133114296621000, -0.59323229503980257000 },
	{ +0.80320753148064761000, -0.59569930449243613000 },
	{ +0.80137617172314302000, -0.59816070699634505000 },
	{ +0.79953726910790790000, -0.60061647938387164000 },
	{ +0.79769084094339404000, -0.60306659854035094000 },
	{ +0.79583690460888656000, -0.60551104140432821000 },
	{ +0.79397547755434028000, -0.60794978496777641000 },
	{ +0.79210657730021539000, -0.61038280627631225000 },
	{ +0.79023022143731314000, -0.61281008242941248000 },
	{ +0.78834642762660934000, -0.61523159058062971000 },
	{ +0.78645521359908876000, -0.61764730793780676000 },
	{ +0.78455659715557824000, -0.62005721176329198000 },
	{ +0.78265059616657884000, -0.62246127937415274000 },
	{ +0.78073722857209749000, -0.62485948814238912000 },
	{ +0.77881651238147898000, -0.62725181549514686000 },
	{ +0.77688846567323555000, -0.62963823891492987000 },
	{ +0.77495310659487704000, -0.63201873593981195000 },
	{ +0.77301045336274010000, -0.63439328416364837000 },
	{ +0.77106052426181682000, -0.63676186123628709000 },
	{ +0.76910333764558270000, -0.63912444486377862000 },
	{ +0.76713891193582351000, -0.64148101280858605000 },
	{ +0.76516726562246207000, -0.64383154288979438000 },
	{ +0.76318841726338438000, -0.64617601298331928000 },
	{ +0.76120238548426500000, -0.64851440102211544000 },
	{ +0.75920918897839118000, -0.65084668499638387000 },
	{ +0.75720884650648779000, -0.65317284295377975000 },
	{ +0.75520137689653988000, -0.65549285299961846000 },
	{ +0.75318679904361585000, -0.65780669329708175000 },
	{ +0.75116513190968981000, -0.66011434206742359000 },
	{ +0.74913639452346270000, -0.66241577759017489000 },
	{ +0.74710060598018357000, -0.66471097820334801000 },
	{ +0.74505778544146939000, -0.66699992230364069000 },
	{ +0.74300795213512505000, -0.66928258834663934000 },
	{ +0.74095112535496244000, -0.67155895484702166000 },
	{ +0.73888732446061856000, -0.67382900037875926000 },
	{ +0.73681656887737335000, -0.67609270357531914000 },
	{ +0.73473887809596694000, -0.67835004312986469000 },
	{ +0.73265427167241626000, -0.68060099779545624000 },
	{ +0.73056276922783092000, -0.68284554638525130000 },
	{ +0.72846439044822853000, -0.68508366777270358000 },
	{ +0.72635915508434934000, -0.68731534089176238000 },
	{ +0.72424708295147033000, -0.68954054473707016000 },
	{ +0.72212819392921868000, -0.69175925836416108000 },
	{ +0.72000250796138499000, -0.69397146088965733000 },
	{ +0.71787004505573504000, -0.69617713149146632000 },
	{ +0.71573082528382193000, -0.69837624940897625000 },
	{ +0.71358486878079697000, -0.70056879394325178000 },
	{ +0.71143219574521976000, -0.70275474445722874000 },
	{ +0.70927282643886891000, -0.70493408037590843000 },
	{ +0.70710678118655079000, -0.70710678118655101000 },
	{ +0.70493408037590821000, -0.70927282643886913000 },
	{ +0.70275474445722863000, -0.71143219574521988000 },
	{ +0.70056879394325167000, -0.71358486878079708000 },
	{ +0.69837624940897625000, -0.71573082528382215000 },
	{ +0.69617713149146632000, -0.71787004505573526000 },
	{ +0.69397146088965744000, -0.72000250796138521000 },
	{ +0.69175925836416119000, -0.72212819392921890000 },
	{ +0.68954054473707038000, -0.72424708295147044000 },
	{ +0.68731534089176261000, -0.72635915508434956000 },
	{ +0.68508366777270391000, -0.72846439044822875000 },
	{ +0.68284554638525163000, -0.73056276922783114000 },
	{ +0.68060099779545657000, -0.73265427167241637000 },
	{ +0.67835004312986502000, -0.73473887809596705000 },
	{ +0.67609270357531948000, -0.73681656887737346000 },
	{ +0.67382900037875948000, -0.73888732446061878000 },
	{ +0.67155895484702188000, -0.74095112535496277000 },
	{ +0.66928258834663956000, -0.74300795213512538000 },
	{ +0.66699992230364102000, -0.74505778544146972000 },
	{ +0.66471097820334835000, -0.74710060598018391000 },
	{ +0.66241577759017523000, -0.74913639452346303000 },
	{ +0.66011434206742392000, -0.75116513190969014000 },
	{ +0.65780669329708219000, -0.75318679904361630000 },
	{ +0.65549285299961890000, -0.75520137689654032000 },
	{ +0.65317284295378031000, -0.75720884650648834000 },
	{ +0.65084668499638443000, -0.75920918897839196000 },
	{ +0.64851440102211588000, -0.76120238548426578000 },
	{ +0.64617601298331984000, -0.76318841726338527000 },
	{ +0.64383154288979494000, -0.76516726562246296000 },
	{ +0.64148101280858660000, -0.76713891193582440000 },
	{ +0.63912444486377917000, -0.76910333764558370000 },
	{ +0.63676186123628775000, -0.77106052426181781000 },
	{ +0.63439328416364893000, -0.77301045336274099000 },
	{ +0.63201873593981250000, -0.77495310659487793000 },
	{ +0.62963823891493054000, -0.77688846567323655000 },
	{ +0.62725181549514764000, -0.77881651238148009000 },
	{ +0.62485948814238990000, -0.78073722857209871000 },
	{ +0.62246127937415352000, -0.78265059616658006000 },
	{ +0.62005721176329276000, -0.78455659715557957000 },
	{ +0.61764730793780753000, -0.78645521359909021000 },
	{ +0.61523159058063048000, -0.78834642762661078000 },
	{ +0.61281008242941337000, -0.79023022143731458000 },
	{ +0.61038280627631314000, -0.79210657730021694000 },
	{ +0.60794978496777730000, -0.79397547755434172000 },
	{ +0.60551104140432910000, -0.79583690460888812000 },
	{ +0.60306659854035183000, -0.79769084094339571000 },
	{ +0.60061647938387253000, -0.79953726910790968000 },
	{ +0.59816070699634594000, -0.80137617172314490000 },
	{ +0.59569930449243691000, -0.80320753148064961000 },
	{ +0.59323229503980335000, -0.80503133114296832000 },
	{ +0.59075970185887783000, -0.80684755354380400000 },
	{ +0.58828154822264889000, -0.80865618158817976000 },
	{ +0.58579785745644253000, -0.81045719825259954000 },
	{ +0.58330865293770195000, -0.81225058658520866000 },
	{ +0.58081395809576819000, -0.81403632970595308000 },
	{ +0.57831379641165925000, -0.81581441080673855000 },
	{ +0.57580819141784900000, -0.81758481315158849000 },
	{ +0.57329716669804587000, -0.81934752007680178000 },
	{ +0.57078074588697092000, -0.82110251499110953000 },
	{ +0.56825895267013526000, -0.82284978137583120000 },
	{ +0.56573181078361701000, -0.82458930278503018000 },
	{ +0.56319934401383787000, -0.82632106284566842000 },
	{ +0.56066157619733981000, -0.82804504525776079000 },
	{ +0.55811853122055988000, -0.82976123379452815000 },
	{ +0.55557023301960606000, -0.83146961230255034000 },
	{ +0.55301670558003135000, -0.83317016470191829000 },
	{ +0.55045797293660870000, -0.83486287498638523000 },
	{ +0.54789405917310408000, -0.83654772722351722000 },
	{ +0.54532498842205035000, -0.83822470555484330000 },
	{ +0.54275078486451989000, -0.83989379419600485000 },
	{ +0.54017147272989685000, -0.84155497743690377000 },
	{ +0.53758707629564939000, -0.84320823964185088000 },
	{ +0.53499761988710104000, -0.84485356524971256000 },
	{ +0.53240312787720179000, -0.84649093877405757000 },
	{ +0.52980362468629849000, -0.84812034480330267000 },
	{ +0.52719913478190517000, -0.84974176800085799000 },
	{ +0.52458968267847272000, -0.85135519310527075000 },
	{ +0.52197529293715816000, -0.85296060493036929000 },
	{ +0.51935599016559342000, -0.85455798836540620000 },
	{ +0.51673179901765365000, -0.85614732837520013000 },
	{ +0.51410274419322544000, -0.85772861000027767000 },
	{ +0.51146885043797408000, -0.85930181835701402000 },
	{ +0.50883014254311065000, -0.86086693863777297000 },
	{ +0.50618664534515889000, -0.86242395611104627000 },
	{ +0.50353838372572113000, -0.86397285612159247000 },
	{ +0.50088538261124427000, -0.86551362409057475000 },
	{ +0.49822766697278537000, -0.86704624551569831000 },
	{ +0.49556526182577604000, -0.86857070597134656000 },
	{ +0.49289819222978754000, -0.87008699110871712000 },
	{ +0.49022648328829466000, -0.87159508665595675000 },
	{ +0.48755016014843944000, -0.87309497841829586000 },
	{ +0.48486924800079456000, -0.87458665227818189000 },
	{ +0.48218377207912622000, -0.87607009419541237000 },
	{ +0.47949375766015651000, -0.87754529020726701000 },
	{ +0.47679923006332564000, -0.87901222642863919000 },
	{ +0.47410021465055352000, -0.88047088905216653000 },
	{ +0.47139673682600114000, -0.88192126434836082000 },
	{ +0.46868882203583140000, -0.88336333866573746000 },
	{ +0.46597649576796962000, -0.88479709843094367000 },
	{ +0.46325978355186359000, -0.88622253014888652000 },
	{ +0.46053871095824339000, -0.88763962040285982000 },
	{ +0.45781330359888062000, -0.88904835585467046000 },
	{ +0.45508358712634717000, -0.89044872324476376000 },
	{ +0.45234958723377422000, -0.89184070939234861000 },
	{ +0.44961132965460993000, -0.89322430119552121000 },
	{ +0.44686884016237755000, -0.89459948563138858000 },
	{ +0.44412214457043259000, -0.89596624975619110000 },
	{ +0.44137126873172006000, -0.89732458070542420000 },
	{ +0.43861623853853099000, -0.89867446569395981000 },
	{ +0.43585707992225886000, -0.90001589201616627000 },
	{ +0.43309381885315534000, -0.90134884704602813000 },
	{ +0.43032648134008600000, -0.90267331823726493000 },
	{ +0.42755509343028542000, -0.90398929312344956000 },
	{ +0.42477968120911214000, -0.90529675931812492000 },
	{ +0.42200027079980296000, -0.90659570451492155000 },
	{ +0.41921688836322724000, -0.90788611648767237000 },
	{ +0.41642956009764048000, -0.90916798309052860000 },
	{ +0.41363831223843783000, -0.91044129225807346000 },
	{ +0.41084317105790719000, -0.91170603200543610000 },
	{ +0.40804416286498191000, -0.91296219042840443000 },
	{ +0.40524131400499308000, -0.91420975570353702000 },
	{ +0.40243465085942165000, -0.91544871608827416000 },
	{ +0.39962419984565001000, -0.91667905992104903000 },
	{ +0.39680998741671347000, -0.91790077562139682000 },
	{ +0.39399204006105126000, -0.91911385169006410000 },
	{ +0.39117038430225703000, -0.92031827670911692000 },
	{ +0.38834504669882941000, -0.92151403934204823000 },
	{ +0.38551605384392196000, -0.92270112833388485000 },
	{ +0.38268343236509283000, -0.92387953251129296000 },
	{ +0.37984720892405421000, -0.92504924078268380000 },
	{ +0.37700741021642126000, -0.92621024213831760000 },
	{ +0.37416406297146099000, -0.92736252565040733000 },
	{ +0.37131719395184049000, -0.92850608047322192000 },
	{ +0.36846682995337526000, -0.92964089584318765000 },
	{ +0.36561299780477680000, -0.93076696107899015000 },
	{ +0.36275572436740011000, -0.93188426558167459000 },
	{ +0.35989503653499105000, -0.93299279883474540000 },
	{ +0.35703096123343292000, -0.93409255040426542000 },
	{ +0.35416352542049329000, -0.93518350993895416000 },
	{ +0.35129275608557003000, -0.93626566717028481000 },
	{ +0.34841868024943745000, -0.93733901191258151000 },
	{ +0.34554132496399198000, -0.93840353406311472000 },
	{ +0.34266071731199726000, -0.93945922360219658000 },
	{ +0.33977688440682968000, -0.94050607059327496000 },
	{ +0.33688985339222283000, -0.94154406518302736000 },
	{ +0.33399965144201216000, -0.94257319760145353000 },
	{ +0.33110630575987915000, -0.94359345816196705000 },
	{ +0.32820984357909527000, -0.94460483726148703000 },
	{ +0.32531029216226565000, -0.94560732538052816000 },
	{ +0.32240767880107252000, -0.94660091308329042000 },
	{ +0.31950203081601830000, -0.94758559101774797000 },
	{ +0.31659337555616845000, -0.94856134991573726000 },
	{ +0.31368174039889402000, -0.94952818059304367000 },
	{ +0.31076715274961403000, -0.95048607394948870000 },
	{ +0.30784964004153742000, -0.95143502096901533000 },
	{ +0.30492922973540493000, -0.95237501271977276000 },
	{ +0.30200594931923058000, -0.95330604035420075000 },
	{ +0.29907982630804297000, -0.95422809510911255000 },
	{ +0.29615088824362629000, -0.95514116830577767000 },
	{ +0.29321916269426107000, -0.95604525135000340000 },
	{ +0.29028467725446477000, -0.95694033573221582000 },
	{ +0.28734745954473195000, -0.95782641302753990000 },
	{ +0.28440753721127421000, -0.95870347489587859000 },
	{ +0.28146493792576033000, -0.95957151308199162000 },
	{ +0.27851968938505545000, -0.96043051941557289000 },
	{ +0.27557181931096048000, -0.96128048581132786000 },
	{ +0.27262135544995125000, -0.96212140426904880000 },
	{ +0.26966832557291737000, -0.96295326687369120000 },
	{ +0.26671275747490064000, -0.96377606579544728000 },
	{ +0.26375467897483362000, -0.96458979328982020000 },
	{ +0.26079411791527773000, -0.96539444169769684000 },
	{ +0.25783110216216121000, -0.96619000344542005000 },
	{ +0.25486565960451679000, -0.96697647104485962000 },
	{ +0.25189781815421913000, -0.96775383709348306000 },
	{ +0.24892760574572237000, -0.96852209427442493000 },
	{ +0.24595505033579679000, -0.96928123535655608000 },
	{ +0.24298017990326604000, -0.97003125319455152000 },
	{ +0.24000302244874361000, -0.97077214072895790000 },
	{ +0.23702360599436931000, -0.97150389098625944000 },
	{ +0.23404195858354551000, -0.97222649707894404000 },
	{ +0.23105810828067319000, -0.97293995220556784000 },
	{ +0.22807208317088778000, -0.97364424965081964000 },
	{ +0.22508391135979489000, -0.97433938278558363000 },
	{ +0.22209362097320556000, -0.97502534506700189000 },
	{ +0.21910124015687182000, -0.97570213003853634000 },
	{ +0.21610679707622152000, -0.97636973133002891000 },
	{ +0.21311031991609336000, -0.97702814265776217000 },
	{ +0.21011183688047161000, -0.97767735782451781000 },
	{ +0.20711137619222050000, -0.97831737071963543000 },
	{ +0.20410896609281881000, -0.97894817531906997000 },
	{ +0.20110463484209382000, -0.97956976568544840000 },
	{ +0.19809841071795548000, -0.98018213596812531000 },
	{ +0.19509032201613014000, -0.98078528040323842000 },
	{ +0.19208039704989427000, -0.98137919331376255000 },
	{ +0.18906866414980802000, -0.98196386910956335000 },
	{ +0.18605515166344844000, -0.98253930228744946000 },
	{ +0.18303988795514273000, -0.98310548743122450000 },
	{ +0.18002290140570126000, -0.98366241921173847000 },
	{ +0.17700422041215050000, -0.98421009238693735000 },
	{ +0.17398387338746554000, -0.98474850180191253000 },
	{ +0.17096188876030291000, -0.98527764238894955000 },
	{ +0.16793829497473284000, -0.98579750916757580000 },
	{ +0.16491312048997156000, -0.98630809724460711000 },
	{ +0.16188639378011344000, -0.98680940181419396000 },
	{ +0.15885814333386303000, -0.98730141815786687000 },
	{ +0.15582839765426679000, -0.98778414164458073000 },
	{ +0.15279718525844496000, -0.98825756773075812000 },
	{ +0.14976453467732304000, -0.98872169196033244000 },
	{ +0.14673047445536325000, -0.98917650996478967000 },
	{ +0.14369503315029591000, -0.98962201746320955000 },
	{ +0.14065823933285065000, -0.99005821026230578000 },
	{ +0.13762012158648745000, -0.99048508425646575000 },
	{ +0.13458070850712758000, -0.99090263542778878000 },
	{ +0.13154002870288448000, -0.99131085984612421000 },
	{ +0.12849811079379450000, -0.99170975366910830000 },
	{ +0.12545498341154754000, -0.99209931314220046000 },
	{ +0.12241067519921747000, -0.99247953459871874000 },
	{ +0.11936521481099262000, -0.99285041445987388000 },
	{ +0.11631863091190600000, -0.99321194923480338000 },
	{ +0.11327095217756555000, -0.99356413552060419000 },
	{ +0.11022220729388424000, -0.99390697000236494000 },
	{ +0.10717242495681001000, -0.99424044945319690000 },
	{ +0.10412163387205571000, -0.99456457073426441000 },
	{ +0.10106986275482893000, -0.99487933079481461000 },
	{ +0.09801714032956168600, -0.99518472667220592000 },
	{ +0.09496349532964006000, -0.99548075549193604000 },
	{ +0.09190895649713376500, -0.99576741446766892000 },
	{ +0.08885355258252560000, -0.99604470090126118000 },
	{ +0.08579731234444086500, -0.99631261218278722000 },
	{ +0.08274026454937664900, -0.99657114579056405000 },
	{ +0.07968243797143104200, -0.99682029929117499000 },
	{ +0.07662386139203238000, -0.99706007033949229000 },
	{ +0.07356456359966828700, -0.99729045667869953000 },
	{ +0.07050457338961470300, -0.99751145614031278000 },
	{ +0.06744391956366487000, -0.99772306664420096000 },
	{ +0.06438263092985825600, -0.99792528619860543000 },
	{ +0.06132073630220934900, -0.99811811290015873000 },
	{ +0.05825826450043650200, -0.99830154493390244000 },
	{ +0.05519524434969064900, -0.99847558057330432000 },
	{ +0.05213170468028400400, -0.99864021818027482000 },
	{ +0.04906767432741866700, -0.99879545620518195000 },
	{ +0.04600318213091524700, -0.99894129318686642000 },
	{ +0.04293825693494141700, -0.99907772775265502000 },
	{ +0.03987292758774037300, -0.99920475861837355000 },
	{ +0.03680722294135936600, -0.99932238458835920000 },
	{ +0.03374117185137809300, -0.99943060455547150000 },
	{ +0.03067480317663710500, -0.99952941750110291000 },
	{ +0.02760814577896619100, -0.99961882249518830000 },
	{ +0.02454122852291270800, -0.99969881869621391000 },
	{ +0.02147408027546989600, -0.99976940535122494000 },
	{ +0.01840672990580518100, -0.99983058179583306000 },
	{ +0.01533920628498843000, -0.99988234745422222000 },
	{ +0.01227153828572022500, -0.99992470183915427000 },
	{ +0.00920375478206008830, -0.99995764455197367000 },
	{ +0.00613588464915471550, -0.99998117528261088000 },
	{ +0.00306795676296618730, -0.99999529380958596000 },
	{ +0.00000000000000018128, -1.00000000000000980000 },
	{ -0.00306795676296582480, -0.99999529380958596000 },
	{ -0.00613588464915435380, -0.99998117528261099000 },
	{ -0.00920375478205972750, -0.99995764455197367000 },
	{ -0.01227153828571986500, -0.99992470183915438000 },
	{ -0.01533920628498806900, -0.99988234745422244000 },
	{ -0.01840672990580482000, -0.99983058179583340000 },
	{ -0.02147408027546953600, -0.99976940535122527000 },
	{ -0.02454122852291234700, -0.99969881869621424000 },
	{ -0.02760814577896583000, -0.99961882249518863000 },
	{ -0.03067480317663674400, -0.99952941750110325000 },
	{ -0.03374117185137773300, -0.99943060455547184000 },
	{ -0.03680722294135901200, -0.99932238458835954000 },
	{ -0.03987292758774002600, -0.99920475861837399000 },
	{ -0.04293825693494107000, -0.99907772775265546000 },
	{ -0.04600318213091490700, -0.99894129318686697000 },
	{ -0.04906767432741832700, -0.99879545620518251000 },
	{ -0.05213170468028366400, -0.99864021818027526000 },
	{ -0.05519524434969031600, -0.99847558057330477000 },
	{ -0.05825826450043616900, -0.99830154493390288000 },
	{ -0.06132073630220901500, -0.99811811290015928000 },
	{ -0.06438263092985792300, -0.99792528619860610000 },
	{ -0.06744391956366455100, -0.99772306664420174000 },
	{ -0.07050457338961439700, -0.99751145614031367000 },
	{ -0.07356456359966799500, -0.99729045667870042000 },
	{ -0.07662386139203208900, -0.99706007033949329000 },
	{ -0.07968243797143075000, -0.99682029929117599000 },
	{ -0.08274026454937635800, -0.99657114579056516000 },
	{ -0.08579731234444058800, -0.99631261218278833000 },
	{ -0.08885355258252533600, -0.99604470090126240000 },
	{ -0.09190895649713350100, -0.99576741446767014000 },
	{ -0.09496349532963979700, -0.99548075549193726000 },
	{ -0.09801714032956143600, -0.99518472667220725000 },
	{ -0.10106986275482870000, -0.99487933079481605000 },
	{ -0.10412163387205549000, -0.99456457073426585000 },
	{ -0.10717242495680979000, -0.99424044945319845000 },
	{ -0.11022220729388403000, -0.99390697000236650000 },
	{ -0.11327095217756536000, -0.99356413552060574000 },
	{ -0.11631863091190581000, -0.99321194923480494000 },
	{ -0.11936521481099244000, -0.99285041445987554000 },
	{ -0.12241067519921731000, -0.99247953459872051000 },
	{ -0.12545498341154740000, -0.99209931314220234000 },
	{ -0.12849811079379436000, -0.99170975366911007000 },
	{ -0.13154002870288431000, -0.99131085984612599000 },
	{ -0.13458070850712742000, -0.99090263542779067000 },
	{ -0.13762012158648732000, -0.99048508425646775000 },
	{ -0.14065823933285054000, -0.99005821026230789000 },
	{ -0.14369503315029580000, -0.98962201746321155000 },
	{ -0.14673047445536314000, -0.98917650996479167000 },
	{ -0.14976453467732292000, -0.98872169196033455000 },
	{ -0.15279718525844488000, -0.98825756773076034000 },
	{ -0.15582839765426673000, -0.98778414164458306000 },
	{ -0.15885814333386297000, -0.98730141815786931000 },
	{ -0.16188639378011341000, -0.98680940181419641000 },
	{ -0.16491312048997153000, -0.98630809724460955000 },
	{ -0.16793829497473281000, -0.98579750916757836000 },
	{ -0.17096188876030288000, -0.98527764238895221000 },
	{ -0.17398387338746552000, -0.98474850180191520000 },
	{ -0.17700422041215047000, -0.98421009238694013000 },
	{ -0.18002290140570126000, -0.98366241921174136000 },
	{ -0.18303988795514275000, -0.98310548743122739000 },
	{ -0.18605515166344846000, -0.98253930228745223000 },
	{ -0.18906866414980805000, -0.98196386910956623000 },
	{ -0.19208039704989432000, -0.98137919331376555000 },
	{ -0.19509032201613019000, -0.98078528040324142000 },
	{ -0.19809841071795553000, -0.98018213596812831000 },
	{ -0.20110463484209390000, -0.97956976568545151000 },
	{ -0.20410896609281889000, -0.97894817531907319000 },
	{ -0.20711137619222059000, -0.97831737071963865000 },
	{ -0.21011183688047169000, -0.97767735782452103000 },
	{ -0.21311031991609347000, -0.97702814265776539000 },
	{ -0.21610679707622166000, -0.97636973133003213000 },
	{ -0.21910124015687196000, -0.97570213003853956000 },
	{ -0.22209362097320573000, -0.97502534506700522000 },
	{ -0.22508391135979508000, -0.97433938278558696000 },
	{ -0.22807208317088803000, -0.97364424965082308000 },
	{ -0.23105810828067347000, -0.97293995220557128000 },
	{ -0.23404195858354582000, -0.97222649707894737000 },
	{ -0.23702360599436964000, -0.97150389098626289000 },
	{ -0.24000302244874397000, -0.97077214072896145000 },
	{ -0.24298017990326642000, -0.97003125319455519000 },
	{ -0.24595505033579718000, -0.96928123535655963000 },
	{ -0.24892760574572276000, -0.96852209427442848000 },
	{ -0.25189781815421958000, -0.96775383709348661000 },
	{ -0.25486565960451724000, -0.96697647104486328000 },
	{ -0.25783110216216171000, -0.96619000344542372000 },
	{ -0.26079411791527829000, -0.96539444169770061000 },
	{ -0.26375467897483418000, -0.96458979328982397000 },
	{ -0.26671275747490120000, -0.96377606579545116000 },
	{ -0.26966832557291798000, -0.96295326687369520000 },
	{ -0.27262135544995192000, -0.96212140426905290000 },
	{ -0.27557181931096114000, -0.96128048581133196000 },
	{ -0.27851968938505611000, -0.96043051941557711000 },
	{ -0.28146493792576105000, -0.95957151308199584000 },
	{ -0.28440753721127493000, -0.95870347489588292000 },
	{ -0.28734745954473268000, -0.95782641302754423000 },
	{ -0.29028467725446555000, -0.95694033573222015000 },
	{ -0.29321916269426190000, -0.95604525135000773000 },
	{ -0.29615088824362712000, -0.95514116830578200000 },
	{ -0.29907982630804381000, -0.95422809510911688000 },
	{ -0.30200594931923141000, -0.95330604035420508000 },
	{ -0.30492922973540576000, -0.95237501271977709000 },
	{ -0.30784964004153831000, -0.95143502096901966000 },
	{ -0.31076715274961497000, -0.95048607394949303000 },
	{ -0.31368174039889501000, -0.94952818059304800000 },
	{ -0.31659337555616945000, -0.94856134991574159000 },
	{ -0.31950203081601930000, -0.94758559101775242000 },
	{ -0.32240767880107352000, -0.94660091308329486000 },
	{ -0.32531029216226665000, -0.94560732538053260000 },
	{ -0.32820984357909627000, -0.94460483726149147000 },
	{ -0.33110630575988015000, -0.94359345816197160000 },
	{ -0.33399965144201321000, -0.94257319760145808000 },
	{ -0.33688985339222388000, -0.94154406518303202000 },
	{ -0.33977688440683068000, -0.94050607059327951000 },
	{ -0.34266071731199826000, -0.93945922360220102000 },
	{ -0.34554132496399298000, -0.93840353406311927000 },
	{ -0.34841868024943851000, -0.93733901191258606000 },
	{ -0.35129275608557115000, -0.93626566717028947000 },
	{ -0.35416352542049445000, -0.93518350993895871000 },
	{ -0.35703096123343414000, -0.93409255040427008000 },
	{ -0.35989503653499233000, -0.93299279883475006000 },
	{ -0.36275572436740144000, -0.93188426558167925000 },
	{ -0.36561299780477813000, -0.93076696107899493000 },
	{ -0.36846682995337665000, -0.92964089584319243000 },
	{ -0.37131719395184193000, -0.92850608047322669000 },
	{ -0.37416406297146243000, -0.92736252565041222000 },
	{ -0.37700741021642270000, -0.92621024213832248000 },
	{ -0.37984720892405566000, -0.92504924078268880000 },
	{ -0.38268343236509428000, -0.92387953251129795000 },
	{ -0.38551605384392340000, -0.92270112833388984000 },
	{ -0.38834504669883085000, -0.92151403934205312000 },
	{ -0.39117038430225848000, -0.92031827670912181000 },
	{ -0.39399204006105271000, -0.91911385169006898000 },
	{ -0.39680998741671497000, -0.91790077562140171000 },
	{ -0.39962419984565151000, -0.91667905992105392000 },
	{ -0.40243465085942315000, -0.91544871608827905000 },
	{ -0.40524131400499458000, -0.91420975570354179000 },
	{ -0.40804416286498341000, -0.91296219042840931000 },
	{ -0.41084317105790874000, -0.91170603200544098000 },
	{ -0.41363831223843939000, -0.91044129225807824000 },
	{ -0.41642956009764204000, -0.90916798309053337000 },
	{ -0.41921688836322885000, -0.90788611648767725000 },
	{ -0.42200027079980462000, -0.90659570451492644000 },
	{ -0.42477968120911380000, -0.90529675931812981000 },
	{ -0.42755509343028708000, -0.90398929312345433000 },
	{ -0.43032648134008766000, -0.90267331823726971000 },
	{ -0.43309381885315701000, -0.90134884704603291000 },
	{ -0.43585707992226058000, -0.90001589201617116000 },
	{ -0.43861623853853277000, -0.89867446569396481000 },
	{ -0.44137126873172189000, -0.89732458070542920000 },
	{ -0.44412214457043447000, -0.89596624975619610000 },
	{ -0.44686884016237949000, -0.89459948563139358000 },
	{ -0.44961132965461192000, -0.89322430119552620000 },
	{ -0.45234958723377627000, -0.89184070939235360000 },
	{ -0.45508358712634928000, -0.89044872324476876000 },
	{ -0.45781330359888273000, -0.88904835585467545000 },
	{ -0.46053871095824556000, -0.88763962040286482000 },
	{ -0.46325978355186581000, -0.88622253014889141000 },
	{ -0.46597649576797184000, -0.88479709843094856000 },
	{ -0.46868882203583362000, -0.88336333866574235000 },
	{ -0.47139673682600336000, -0.88192126434836582000 },
	{ -0.47410021465055574000, -0.88047088905217152000 },
	{ -0.47679923006332792000, -0.87901222642864418000 },
	{ -0.47949375766015884000, -0.87754529020727201000 },
	{ -0.48218377207912860000, -0.87607009419541737000 },
	{ -0.48486924800079700000, -0.87458665227818688000 },
	{ -0.48755016014844188000, -0.87309497841830086000 },
	{ -0.49022648328829710000, -0.87159508665596186000 },
	{ -0.49289819222979003000, -0.87008699110872223000 },
	{ -0.49556526182577859000, -0.86857070597135166000 },
	{ -0.49822766697278792000, -0.86704624551570342000 },
	{ -0.50088538261124693000, -0.86551362409057986000 },
	{ -0.50353838372572379000, -0.86397285612159747000 },
	{ -0.50618664534516156000, -0.86242395611105127000 },
	{ -0.50883014254311332000, -0.86086693863777797000 },
	{ -0.51146885043797674000, -0.85930181835701902000 },
	{ -0.51410274419322810000, -0.85772861000028267000 },
	{ -0.51673179901765631000, -0.85614732837520513000 },
	{ -0.51935599016559608000, -0.85455798836541108000 },
	{ -0.52197529293716083000, -0.85296060493037418000 },
	{ -0.52458968267847539000, -0.85135519310527563000 },
	{ -0.52719913478190783000, -0.84974176800086298000 },
	{ -0.52980362468630127000, -0.84812034480330778000 },
	{ -0.53240312787720467000, -0.84649093877406256000 },
	{ -0.53499761988710404000, -0.84485356524971755000 },
	{ -0.53758707629565228000, -0.84320823964185598000 },
	{ -0.54017147272989974000, -0.84155497743690899000 },
	{ -0.54275078486452277000, -0.83989379419601007000 },
	{ -0.54532498842205335000, -0.83822470555484851000 },
	{ -0.54789405917310718000, -0.83654772722352244000 },
	{ -0.55045797293661192000, -0.83486287498639056000 },
	{ -0.55301670558003468000, -0.83317016470192373000 },
	{ -0.55557023301960951000, -0.83146961230255578000 },
	{ -0.55811853122056343000, -0.82976123379453359000 },
	{ -0.56066157619734347000, -0.82804504525776634000 },
	{ -0.56319934401384153000, -0.82632106284567408000 },
	{ -0.56573181078362067000, -0.82458930278503584000 },
	{ -0.56825895267013904000, -0.82284978137583686000 },
	{ -0.57078074588697492000, -0.82110251499111520000 },
	{ -0.57329716669804998000, -0.81934752007680745000 },
	{ -0.57580819141785311000, -0.81758481315159426000 },
	{ -0.57831379641166336000, -0.81581441080674433000 },
	{ -0.58081395809577241000, -0.81403632970595885000 },
	{ -0.58330865293770617000, -0.81225058658521443000 },
	{ -0.58579785745644675000, -0.81045719825260532000 },
	{ -0.58828154822265322000, -0.80865618158818553000 },
	{ -0.59075970185888227000, -0.80684755354380977000 },
	{ -0.59323229503980790000, -0.80503133114297409000 },
	{ -0.59569930449244146000, -0.80320753148065538000 },
	{ -0.59816070699635038000, -0.80137617172315068000 },
	{ -0.60061647938387697000, -0.79953726910791556000 },
	{ -0.60306659854035627000, -0.79769084094340170000 },
	{ -0.60551104140433354000, -0.79583690460889422000 },
	{ -0.60794978496778174000, -0.79397547755434783000 },
	{ -0.61038280627631758000, -0.79210657730022294000 },
	{ -0.61281008242941781000, -0.79023022143732058000 },
	{ -0.61523159058063503000, -0.78834642762661677000 },
	{ -0.61764730793781220000, -0.78645521359909620000 },
	{ -0.62005721176329753000, -0.78455659715558568000 },
	{ -0.62246127937415840000, -0.78265059616658617000 },
	{ -0.62485948814239489000, -0.78073722857210481000 },
	{ -0.62725181549515263000, -0.77881651238148630000 },
	{ -0.62963823891493564000, -0.77688846567324277000 },
	{ -0.63201873593981772000, -0.77495310659488426000 },
	{ -0.63439328416365426000, -0.77301045336274732000 },
	{ -0.63676186123629308000, -0.77106052426182403000 },
	{ -0.63912444486378461000, -0.76910333764558991000 },
	{ -0.64148101280859215000, -0.76713891193583061000 },
	{ -0.64383154288980049000, -0.76516726562246917000 },
	{ -0.64617601298332539000, -0.76318841726339148000 },
	{ -0.64851440102212154000, -0.76120238548427210000 },
	{ -0.65084668499639009000, -0.75920918897839829000 },
	{ -0.65317284295378597000, -0.75720884650649478000 },
	{ -0.65549285299962468000, -0.75520137689654676000 },
	{ -0.65780669329708796000, -0.75318679904362273000 },
	{ -0.66011434206742980000, -0.75116513190969669000 },
	{ -0.66241577759018122000, -0.74913639452346958000 },
	{ -0.66471097820335434000, -0.74710060598019046000 },
	{ -0.66699992230364702000, -0.74505778544147627000 },
	{ -0.66928258834664567000, -0.74300795213513193000 },
	{ -0.67155895484702799000, -0.74095112535496932000 },
	{ -0.67382900037876570000, -0.73888732446062544000 },
	{ -0.67609270357532569000, -0.73681656887738023000 },
	{ -0.67835004312987124000, -0.73473887809597382000 },
	{ -0.68060099779546279000, -0.73265427167242314000 },
	{ -0.68284554638525785000, -0.73056276922783780000 },
	{ -0.68508366777271013000, -0.72846439044823541000 },
	{ -0.68731534089176893000, -0.72635915508435622000 },
	{ -0.68954054473707682000, -0.72424708295147711000 },
	{ -0.69175925836416774000, -0.72212819392922545000 },
	{ -0.69397146088966399000, -0.72000250796139165000 },
	{ -0.69617713149147298000, -0.71787004505574170000 },
	{ -0.69837624940898291000, -0.71573082528382859000 },
	{ -0.70056879394325844000, -0.71358486878080352000 },
	{ -0.70275474445723551000, -0.71143219574522631000 },
	{ -0.70493408037591521000, -0.70927282643887546000 },
	{ -0.70710678118655790000, -0.70710678118655723000 },
	{ -0.70927282643887601000, -0.70493408037591465000 },
	{ -0.71143219574522687000, -0.70275474445723496000 },
	{ -0.71358486878080407000, -0.70056879394325799000 },
	{ -0.71573082528382914000, -0.69837624940898246000 },
	{ -0.71787004505574237000, -0.69617713149147253000 },
	{ -0.72000250796139231000, -0.69397146088966366000 },
	{ -0.72212819392922600000, -0.69175925836416741000 },
	{ -0.72424708295147766000, -0.68954054473707660000 },
	{ -0.72635915508435678000, -0.68731534089176882000 },
	{ -0.72846439044823608000, -0.68508366777271001000 },
	{ -0.73056276922783847000, -0.68284554638525774000 },
	{ -0.73265427167242370000, -0.68060099779546268000 },
	{ -0.73473887809597438000, -0.67835004312987113000 },
	{ -0.73681656887738078000, -0.67609270357532558000 },
	{ -0.73888732446062610000, -0.67382900037876559000 },
	{ -0.74095112535497010000, -0.67155895484702788000 },
	{ -0.74300795213513271000, -0.66928258834664545000 },
	{ -0.74505778544147705000, -0.66699992230364691000 },
	{ -0.74710060598019123000, -0.66471097820335423000 },
	{ -0.74913639452347036000, -0.66241577759018111000 },
	{ -0.75116513190969747000, -0.66011434206742980000 },
	{ -0.75318679904362362000, -0.65780669329708796000 },
	{ -0.75520137689654765000, -0.65549285299962468000 },
	{ -0.75720884650649567000, -0.65317284295378608000 },
	{ -0.75920918897839929000, -0.65084668499639020000 },
	{ -0.76120238548427321000, -0.64851440102212166000 },
	{ -0.76318841726339270000, -0.64617601298332550000 },
	{ -0.76516726562247039000, -0.64383154288980060000 },
	{ -0.76713891193583184000, -0.64148101280859227000 },
	{ -0.76910333764559113000, -0.63912444486378484000 },
	{ -0.77106052426182536000, -0.63676186123629330000 },
	{ -0.77301045336274854000, -0.63439328416365448000 },
	{ -0.77495310659488548000, -0.63201873593981794000 },
	{ -0.77688846567324410000, -0.62963823891493587000 },
	{ -0.77881651238148764000, -0.62725181549515296000 },
	{ -0.78073722857210626000, -0.62485948814239523000 },
	{ -0.78265059616658761000, -0.62246127937415885000 },
	{ -0.78455659715558712000, -0.62005721176329798000 },
	{ -0.78645521359909776000, -0.61764730793781264000 },
	{ -0.78834642762661833000, -0.61523159058063548000 },
	{ -0.79023022143732213000, -0.61281008242941826000 },
	{ -0.79210657730022449000, -0.61038280627631802000 },
	{ -0.79397547755434938000, -0.60794978496778218000 },
	{ -0.79583690460889578000, -0.60551104140433398000 },
	{ -0.79769084094340348000, -0.60306659854035671000 },
	{ -0.79953726910791745000, -0.60061647938387741000 },
	{ -0.80137617172315267000, -0.59816070699635082000 },
	{ -0.80320753148065738000, -0.59569930449244179000 },
	{ -0.80503133114297620000, -0.59323229503980823000 },
	{ -0.80684755354381199000, -0.59075970185888260000 },
	{ -0.80865618158818775000, -0.58828154822265366000 },
	{ -0.81045719825260765000, -0.58579785745644719000 },
	{ -0.81225058658521676000, -0.58330865293770662000 },
	{ -0.81403632970596118000, -0.58081395809577285000 },
	{ -0.81581441080674666000, -0.57831379641166392000 },
	{ -0.81758481315159659000, -0.57580819141785367000 },
	{ -0.81934752007680989000, -0.57329716669805053000 },
	{ -0.82110251499111764000, -0.57078074588697558000 },
	{ -0.82284978137583931000, -0.56825895267013982000 },
	{ -0.82458930278503828000, -0.56573181078362145000 },
	{ -0.82632106284567652000, -0.56319934401384231000 },
	{ -0.82804504525776890000, -0.56066157619734425000 },
	{ -0.82976123379453626000, -0.55811853122056432000 },
	{ -0.83146961230255845000, -0.55557023301961039000 },
	{ -0.83317016470192640000, -0.55301670558003568000 },
	{ -0.83486287498639333000, -0.55045797293661292000 },
	{ -0.83654772722352533000, -0.54789405917310829000 },
	{ -0.83822470555485140000, -0.54532498842205457000 },
	{ -0.83989379419601295000, -0.54275078486452399000 },
	{ -0.84155497743691188000, -0.54017147272990096000 },
	{ -0.84320823964185898000, -0.53758707629565350000 },
	{ -0.84485356524972066000, -0.53499761988710515000 },
	{ -0.84649093877406567000, -0.53240312787720590000 },
	{ -0.84812034480331078000, -0.52980362468630260000 },
	{ -0.84974176800086609000, -0.52719913478190927000 },
	{ -0.85135519310527885000, -0.52458968267847683000 },
	{ -0.85296060493037740000, -0.52197529293716227000 },
	{ -0.85455798836541430000, -0.51935599016559753000 },
	{ -0.85614732837520824000, -0.51673179901765776000 },
	{ -0.85772861000028588000, -0.51410274419322954000 },
	{ -0.85930181835702224000, -0.51146885043797818000 },
	{ -0.86086693863778119000, -0.50883014254311476000 },
	{ -0.86242395611105449000, -0.50618664534516300000 },
	{ -0.86397285612160069000, -0.50353838372572524000 },
	{ -0.86551362409058308000, -0.50088538261124838000 },
	{ -0.86704624551570664000, -0.49822766697278942000 },
	{ -0.86857070597135488000, -0.49556526182578009000 },
	{ -0.87008699110872545000, -0.49289819222979153000 },
	{ -0.87159508665596508000, -0.49022648328829865000 },
	{ -0.87309497841830419000, -0.48755016014844343000 },
	{ -0.87458665227819021000, -0.48486924800079856000 },
	{ -0.87607009419542070000, -0.48218377207913016000 },
	{ -0.87754529020727534000, -0.47949375766016045000 },
	{ -0.87901222642864751000, -0.47679923006332953000 },
	{ -0.88047088905217485000, -0.47410021465055735000 },
	{ -0.88192126434836915000, -0.47139673682600497000 },
	{ -0.88336333866574579000, -0.46868882203583523000 },
	{ -0.88479709843095200000, -0.46597649576797345000 },
	{ -0.88622253014889485000, -0.46325978355186742000 },
	{ -0.88763962040286815000, -0.46053871095824722000 },
	{ -0.88904835585467878000, -0.45781330359888439000 },
	{ -0.89044872324477209000, -0.45508358712635094000 },
	{ -0.89184070939235693000, -0.45234958723377794000 },
	{ -0.89322430119552954000, -0.44961132965461365000 },
	{ -0.89459948563139691000, -0.44686884016238121000 },
	{ -0.89596624975619943000, -0.44412214457043619000 },
	{ -0.89732458070543253000, -0.44137126873172366000 },
	{ -0.89867446569396814000, -0.43861623853853454000 },
	{ -0.90001589201617460000, -0.43585707992226236000 },
	{ -0.90134884704603646000, -0.43309381885315879000 },
	{ -0.90267331823727326000, -0.43032648134008938000 },
	{ -0.90398929312345788000, -0.42755509343028880000 },
	{ -0.90529675931813336000, -0.42477968120911552000 },
	{ -0.90659570451492999000, -0.42200027079980634000 },
	{ -0.90788611648768081000, -0.41921688836323057000 },
	{ -0.90916798309053704000, -0.41642956009764376000 },
	{ -0.91044129225808190000, -0.41363831223844105000 },
	{ -0.91170603200544464000, -0.41084317105791041000 },
	{ -0.91296219042841298000, -0.40804416286498513000 },
	{ -0.91420975570354557000, -0.40524131400499630000 },
	{ -0.91544871608828271000, -0.40243465085942481000 },
	{ -0.91667905992105758000, -0.39962419984565312000 },
	{ -0.91790077562140548000, -0.39680998741671658000 },
	{ -0.91911385169007276000, -0.39399204006105432000 },
	{ -0.92031827670912558000, -0.39117038430226003000 },
	{ -0.92151403934205689000, -0.38834504669883235000 },
	{ -0.92270112833389362000, -0.38551605384392490000 },
	{ -0.92387953251130184000, -0.38268343236509578000 },
	{ -0.92504924078269268000, -0.37984720892405716000 },
	{ -0.92621024213832648000, -0.37700741021642420000 },
	{ -0.92736252565041633000, -0.37416406297146393000 },
	{ -0.92850608047323091000, -0.37131719395184343000 },
	{ -0.92964089584319665000, -0.36846682995337815000 },
	{ -0.93076696107899914000, -0.36561299780477968000 },
	{ -0.93188426558168358000, -0.36275572436740300000 },
	{ -0.93299279883475439000, -0.35989503653499388000 },
	{ -0.93409255040427441000, -0.35703096123343570000 },
	{ -0.93518350993896315000, -0.35416352542049601000 },
	{ -0.93626566717029380000, -0.35129275608557270000 },
	{ -0.93733901191259050000, -0.34841868024944012000 },
	{ -0.93840353406312371000, -0.34554132496399459000 },
	{ -0.93945922360220557000, -0.34266071731199987000 },
	{ -0.94050607059328406000, -0.33977688440683229000 },
	{ -0.94154406518303657000, -0.33688985339222544000 },
	{ -0.94257319760146274000, -0.33399965144201471000 },
	{ -0.94359345816197626000, -0.33110630575988165000 },
	{ -0.94460483726149624000, -0.32820984357909777000 },
	{ -0.94560732538053738000, -0.32531029216226809000 },
	{ -0.94660091308329963000, -0.32240767880107496000 },
	{ -0.94758559101775719000, -0.31950203081602074000 },
	{ -0.94856134991574648000, -0.31659337555617090000 },
	{ -0.94952818059305288000, -0.31368174039889646000 },
	{ -0.95048607394949791000, -0.31076715274961647000 },
	{ -0.95143502096902455000, -0.30784964004153981000 },
	{ -0.95237501271978198000, -0.30492922973540731000 },
	{ -0.95330604035420996000, -0.30200594931923291000 },
	{ -0.95422809510912188000, -0.29907982630804525000 },
	{ -0.95514116830578699000, -0.29615088824362856000 },
	{ -0.95604525135001273000, -0.29321916269426335000 },
	{ -0.95694033573222514000, -0.29028467725446705000 },
	{ -0.95782641302754923000, -0.28734745954473417000 },
	{ -0.95870347489588792000, -0.28440753721127643000 },
	{ -0.95957151308200095000, -0.28146493792576255000 },
	{ -0.96043051941558222000, -0.27851968938505761000 },
	{ -0.96128048581133718000, -0.27557181931096264000 },
	{ -0.96212140426905812000, -0.27262135544995342000 },
	{ -0.96295326687370053000, -0.26966832557291953000 },
	{ -0.96377606579545660000, -0.26671275747490275000 },
	{ -0.96458979328982952000, -0.26375467897483573000 },
	{ -0.96539444169770616000, -0.26079411791527979000 },
	{ -0.96619000344542938000, -0.25783110216216321000 },
	{ -0.96697647104486895000, -0.25486565960451874000 },
	{ -0.96775383709349239000, -0.25189781815422108000 },
	{ -0.96852209427443425000, -0.24892760574572426000 },
	{ -0.96928123535656541000, -0.24595505033579865000 },
	{ -0.97003125319456085000, -0.24298017990326787000 },
	{ -0.97077214072896723000, -0.24000302244874541000 },
	{ -0.97150389098626877000, -0.23702360599437108000 },
	{ -0.97222649707895337000, -0.23404195858354726000 },
	{ -0.97293995220557727000, -0.23105810828067491000 },
	{ -0.97364424965082907000, -0.22807208317088948000 },
	{ -0.97433938278559307000, -0.22508391135979655000 },
	{ -0.97502534506701133000, -0.22209362097320720000 },
	{ -0.97570213003854578000, -0.21910124015687343000 },
	{ -0.97636973133003846000, -0.21610679707622310000 },
	{ -0.97702814265777171000, -0.21311031991609491000 },
	{ -0.97767735782452736000, -0.21011183688047311000 },
	{ -0.97831737071964497000, -0.20711137619222197000 },
	{ -0.97894817531907952000, -0.20410896609282023000 },
	{ -0.97956976568545795000, -0.20110463484209520000 },
	{ -0.98018213596813486000, -0.19809841071795684000 },
	{ -0.98078528040324797000, -0.19509032201613147000 },
	{ -0.98137919331377210000, -0.19208039704989557000 },
	{ -0.98196386910957290000, -0.18906866414980930000 },
	{ -0.98253930228745900000, -0.18605515166344969000 },
	{ -0.98310548743123405000, -0.18303988795514395000 },
	{ -0.98366241921174802000, -0.18002290140570246000 },
	{ -0.98421009238694690000, -0.17700422041215166000 },
	{ -0.98474850180192208000, -0.17398387338746668000 },
	{ -0.98527764238895910000, -0.17096188876030402000 },
	{ -0.98579750916758535000, -0.16793829497473392000 },
	{ -0.98630809724461665000, -0.16491312048997261000 },
	{ -0.98680940181420351000, -0.16188639378011446000 },
	{ -0.98730141815787642000, -0.15885814333386403000 },
	{ -0.98778414164459027000, -0.15582839765426776000 },
	{ -0.98825756773076767000, -0.15279718525844591000 },
	{ -0.98872169196034199000, -0.14976453467732395000 },
	{ -0.98917650996479922000, -0.14673047445536413000 },
	{ -0.98962201746321909000, -0.14369503315029677000 },
	{ -0.99005821026231533000, -0.14065823933285149000 },
	{ -0.99048508425647530000, -0.13762012158648826000 },
	{ -0.99090263542779833000, -0.13458070850712836000 },
	{ -0.99131085984613376000, -0.13154002870288523000 },
	{ -0.99170975366911784000, -0.12849811079379522000 },
	{ -0.99209931314221000000, -0.12545498341154823000 },
	{ -0.99247953459872829000, -0.12241067519921814000 },
	{ -0.99285041445988342000, -0.11936521481099326000 },
	{ -0.99321194923481293000, -0.11631863091190660000 },
	{ -0.99356413552061373000, -0.11327095217756612000 },
	{ -0.99390697000237449000, -0.11022220729388478000 },
	{ -0.99424044945320644000, -0.10717242495681052000 },
	{ -0.99456457073427396000, -0.10412163387205620000 },
	{ -0.99487933079482416000, -0.10106986275482939000 },
	{ -0.99518472667221547000, -0.09801714032956211600 },
	{ -0.99548075549194559000, -0.09496349532964046300 },
	{ -0.99576741446767847000, -0.09190895649713413900 },
	{ -0.99604470090127073000, -0.08885355258252594600 },
	{ -0.99631261218279676000, -0.08579731234444118400 },
	{ -0.99657114579057360000, -0.08274026454937694100 },
	{ -0.99682029929118454000, -0.07968243797143130500 },
	{ -0.99706007033950184000, -0.07662386139203261600 },
	{ -0.99729045667870908000, -0.07356456359966849500 },
	{ -0.99751145614032233000, -0.07050457338961488300 },
	{ -0.99772306664421051000, -0.06744391956366502200 },
	{ -0.99792528619861498000, -0.06438263092985836700 },
	{ -0.99811811290016828000, -0.06132073630220943200 },
	{ -0.99830154493391199000, -0.05825826450043655000 },
	{ -0.99847558057331387000, -0.05519524434969067000 },
	{ -0.99864021818028437000, -0.05213170468028399000 },
	{ -0.99879545620519150000, -0.04906767432741862600 },
	{ -0.99894129318687597000, -0.04600318213091517800 },
	{ -0.99907772775266457000, -0.04293825693494131300 },
	{ -0.99920475861838309000, -0.03987292758774024100 },
	{ -0.99932238458836875000, -0.03680722294135919900 },
	{ -0.99943060455548105000, -0.03374117185137789200 },
	{ -0.99952941750111246000, -0.03067480317663687200 },
	{ -0.99961882249519785000, -0.02760814577896592700 },
	{ -0.99969881869622346000, -0.02454122852291241300 },
	{ -0.99976940535123449000, -0.02147408027546957400 },
	{ -0.99983058179584261000, -0.01840672990580483100 },
	{ -0.99988234745423177000, -0.01533920628498805200 },
	{ -0.99992470183916382000, -0.01227153828571981800 },
	{ -0.99995764455198322000, -0.00920375478205965120 },
	{ -0.99998117528262043000, -0.00613588464915424890 },
	{ -0.99999529380959551000, -0.00306795676296569120 },
	{ -1.00000000000001930000, +0.00000000000000034434 },
};
#endif /* FFTSizeFilters == 2048 */
#if FFTSizeFilters == 2048
/* FFT/IFFT w coefficients tables for 2048 elements */
static const FLASHMEM struct Complex wp [1024] = 
{
	{ +0.99999529380957619000, +0.00306795676296597610 },
	{ +0.99998117528260122000, +0.00613588464915447530 },
	{ +0.99995764455196390000, +0.00920375478205981940 },
	{ +0.99992470183914461000, +0.01227153828571992700 },
	{ +0.99988234745421267000, +0.01533920628498810200 },
	{ +0.99983058179582363000, +0.01840672990580482400 },
	{ +0.99976940535121550000, +0.02147408027546950800 },
	{ +0.99969881869620447000, +0.02454122852291228800 },
	{ +0.99961882249517886000, +0.02760814577896574000 },
	{ +0.99952941750109348000, +0.03067480317663662600 },
	{ +0.99943060455546207000, +0.03374117185137758700 },
	{ +0.99932238458834977000, +0.03680722294135883200 },
	{ +0.99920475861836422000, +0.03987292758773981100 },
	{ +0.99907772775264569000, +0.04293825693494082000 },
	{ +0.99894129318685720000, +0.04600318213091463000 },
	{ +0.99879545620517274000, +0.04906767432741801500 },
	{ +0.99864021818026549000, +0.05213170468028332400 },
	{ +0.99847558057329500000, +0.05519524434968994800 },
	{ +0.99830154493389311000, +0.05825826450043576600 },
	{ +0.99811811290014951000, +0.06132073630220858500 },
	{ +0.99792528619859633000, +0.06438263092985746500 },
	{ +0.99772306664419197000, +0.06744391956366406500 },
	{ +0.99751145614030390000, +0.07050457338961387000 },
	{ +0.99729045667869065000, +0.07356456359966744000 },
	{ +0.99706007033948352000, +0.07662386139203150600 },
	{ +0.99682029929116622000, +0.07968243797143014000 },
	{ +0.99657114579055539000, +0.08274026454937571900 },
	{ +0.99631261218277856000, +0.08579731234443992200 },
	{ +0.99604470090125263000, +0.08885355258252462800 },
	{ +0.99576741446766037000, +0.09190895649713275200 },
	{ +0.99548075549192749000, +0.09496349532963901900 },
	{ +0.99518472667219748000, +0.09801714032956061800 },
	{ +0.99487933079480628000, +0.10106986275482785000 },
	{ +0.99456457073425608000, +0.10412163387205461000 },
	{ +0.99424044945318868000, +0.10717242495680888000 },
	{ +0.99390697000235673000, +0.11022220729388310000 },
	{ +0.99356413552059597000, +0.11327095217756439000 },
	{ +0.99321194923479517000, +0.11631863091190481000 },
	{ +0.99285041445986577000, +0.11936521481099141000 },
	{ +0.99247953459871074000, +0.12241067519921625000 },
	{ +0.99209931314219257000, +0.12545498341154629000 },
	{ +0.99170975366910030000, +0.12849811079379322000 },
	{ +0.99131085984611622000, +0.13154002870288314000 },
	{ +0.99090263542778090000, +0.13458070850712622000 },
	{ +0.99048508425645798000, +0.13762012158648609000 },
	{ +0.99005821026229812000, +0.14065823933284929000 },
	{ +0.98962201746320189000, +0.14369503315029453000 },
	{ +0.98917650996478201000, +0.14673047445536183000 },
	{ +0.98872169196032489000, +0.14976453467732159000 },
	{ +0.98825756773075069000, +0.15279718525844352000 },
	{ +0.98778414164457340000, +0.15582839765426534000 },
	{ +0.98730141815785966000, +0.15885814333386156000 },
	{ +0.98680940181418675000, +0.16188639378011196000 },
	{ +0.98630809724459989000, +0.16491312048997006000 },
	{ +0.98579750916756870000, +0.16793829497473131000 },
	{ +0.98527764238894255000, +0.17096188876030136000 },
	{ +0.98474850180190554000, +0.17398387338746396000 },
	{ +0.98421009238693047000, +0.17700422041214889000 },
	{ +0.98366241921173170000, +0.18002290140569965000 },
	{ +0.98310548743121773000, +0.18303988795514112000 },
	{ +0.98253930228744257000, +0.18605515166344680000 },
	{ +0.98196386910955658000, +0.18906866414980636000 },
	{ +0.98137919331375589000, +0.19208039704989258000 },
	{ +0.98078528040323176000, +0.19509032201612841000 },
	{ +0.98018213596811865000, +0.19809841071795373000 },
	{ +0.97956976568544185000, +0.20110463484209207000 },
	{ +0.97894817531906353000, +0.20410896609281703000 },
	{ +0.97831737071962899000, +0.20711137619221870000 },
	{ +0.97767735782451137000, +0.21011183688046978000 },
	{ +0.97702814265775573000, +0.21311031991609153000 },
	{ +0.97636973133002247000, +0.21610679707621969000 },
	{ +0.97570213003852990000, +0.21910124015686996000 },
	{ +0.97502534506699556000, +0.22209362097320370000 },
	{ +0.97433938278557730000, +0.22508391135979303000 },
	{ +0.97364424965081342000, +0.22807208317088595000 },
	{ +0.97293995220556162000, +0.23105810828067136000 },
	{ +0.97222649707893771000, +0.23404195858354368000 },
	{ +0.97150389098625323000, +0.23702360599436748000 },
	{ +0.97077214072895179000, +0.24000302244874178000 },
	{ +0.97003125319454553000, +0.24298017990326420000 },
	{ +0.96928123535655009000, +0.24595505033579493000 },
	{ +0.96852209427441893000, +0.24892760574572048000 },
	{ +0.96775383709347707000, +0.25189781815421730000 },
	{ +0.96697647104485374000, +0.25486565960451496000 },
	{ +0.96619000344541417000, +0.25783110216215938000 },
	{ +0.96539444169769106000, +0.26079411791527590000 },
	{ +0.96458979328981442000, +0.26375467897483179000 },
	{ +0.96377606579544162000, +0.26671275747489881000 },
	{ +0.96295326687368565000, +0.26966832557291553000 },
	{ +0.96212140426904336000, +0.27262135544994942000 },
	{ +0.96128048581132242000, +0.27557181931095864000 },
	{ +0.96043051941556756000, +0.27851968938505356000 },
	{ +0.95957151308198640000, +0.28146493792575844000 },
	{ +0.95870347489587349000, +0.28440753721127232000 },
	{ +0.95782641302753480000, +0.28734745954473001000 },
	{ +0.95694033573221071000, +0.29028467725446283000 },
	{ +0.95604525134999829000, +0.29321916269425913000 },
	{ +0.95514116830577256000, +0.29615088824362434000 },
	{ +0.95422809510910744000, +0.29907982630804097000 },
	{ +0.95330604035419564000, +0.30200594931922858000 },
	{ +0.95237501271976777000, +0.30492922973540293000 },
	{ +0.95143502096901034000, +0.30784964004153542000 },
	{ +0.95048607394948370000, +0.31076715274961209000 },
	{ +0.94952818059303867000, +0.31368174039889207000 },
	{ +0.94856134991573227000, +0.31659337555616651000 },
	{ +0.94758559101774309000, +0.31950203081601630000 },
	{ +0.94660091308328564000, +0.32240767880107046000 },
	{ +0.94560732538052339000, +0.32531029216226354000 },
	{ +0.94460483726148226000, +0.32820984357909311000 },
	{ +0.94359345816196238000, +0.33110630575987698000 },
	{ +0.94257319760144898000, +0.33399965144201005000 },
	{ +0.94154406518302292000, +0.33688985339222072000 },
	{ +0.94050607059327052000, +0.33977688440682752000 },
	{ +0.93945922360219214000, +0.34266071731199504000 },
	{ +0.93840353406311039000, +0.34554132496398970000 },
	{ +0.93733901191257718000, +0.34841868024943518000 },
	{ +0.93626566717028059000, +0.35129275608556776000 },
	{ +0.93518350993894983000, +0.35416352542049101000 },
	{ +0.93409255040426120000, +0.35703096123343070000 },
	{ +0.93299279883474118000, +0.35989503653498883000 },
	{ +0.93188426558167037000, +0.36275572436739789000 },
	{ +0.93076696107898604000, +0.36561299780477458000 },
	{ +0.92964089584318355000, +0.36846682995337304000 },
	{ +0.92850608047321781000, +0.37131719395183826000 },
	{ +0.92736252565040334000, +0.37416406297145871000 },
	{ +0.92621024213831360000, +0.37700741021641898000 },
	{ +0.92504924078267992000, +0.37984720892405194000 },
	{ +0.92387953251128907000, +0.38268343236509056000 },
	{ +0.92270112833388096000, +0.38551605384391968000 },
	{ +0.92151403934204434000, +0.38834504669882713000 },
	{ +0.92031827670911304000, +0.39117038430225476000 },
	{ +0.91911385169006021000, +0.39399204006104899000 },
	{ +0.91790077562139294000, +0.39680998741671120000 },
	{ +0.91667905992104515000, +0.39962419984564773000 },
	{ +0.91544871608827039000, +0.40243465085941937000 },
	{ +0.91420975570353313000, +0.40524131400499080000 },
	{ +0.91296219042840066000, +0.40804416286497963000 },
	{ +0.91170603200543243000, +0.41084317105790491000 },
	{ +0.91044129225806969000, +0.41363831223843556000 },
	{ +0.90916798309052493000, +0.41642956009763821000 },
	{ +0.90788611648766882000, +0.41921688836322496000 },
	{ +0.90659570451491800000, +0.42200027079980074000 },
	{ +0.90529675931812137000, +0.42477968120910992000 },
	{ +0.90398929312344589000, +0.42755509343028320000 },
	{ +0.90267331823726138000, +0.43032648134008378000 },
	{ +0.90134884704602458000, +0.43309381885315312000 },
	{ +0.90001589201616283000, +0.43585707992225664000 },
	{ +0.89867446569395648000, +0.43861623853852882000 },
	{ +0.89732458070542098000, +0.44137126873171789000 },
	{ +0.89596624975618788000, +0.44412214457043042000 },
	{ +0.89459948563138536000, +0.44686884016237544000 },
	{ +0.89322430119551799000, +0.44961132965460787000 },
	{ +0.89184070939234539000, +0.45234958723377217000 },
	{ +0.89044872324476054000, +0.45508358712634511000 },
	{ +0.88904835585466724000, +0.45781330359887851000 },
	{ +0.88763962040285660000, +0.46053871095824128000 },
	{ +0.88622253014888319000, +0.46325978355186148000 },
	{ +0.88479709843094034000, +0.46597649576796746000 },
	{ +0.88336333866573413000, +0.46868882203582918000 },
	{ +0.88192126434835760000, +0.47139673682599892000 },
	{ +0.88047088905216331000, +0.47410021465055130000 },
	{ +0.87901222642863597000, +0.47679923006332342000 },
	{ +0.87754529020726380000, +0.47949375766015434000 },
	{ +0.87607009419540915000, +0.48218377207912411000 },
	{ +0.87458665227817867000, +0.48486924800079251000 },
	{ +0.87309497841829264000, +0.48755016014843738000 },
	{ +0.87159508665595364000, +0.49022648328829260000 },
	{ +0.87008699110871401000, +0.49289819222978554000 },
	{ +0.86857070597134345000, +0.49556526182577409000 },
	{ +0.86704624551569520000, +0.49822766697278342000 },
	{ +0.86551362409057164000, +0.50088538261124238000 },
	{ +0.86397285612158925000, +0.50353838372571924000 },
	{ +0.86242395611104306000, +0.50618664534515700000 },
	{ +0.86086693863776975000, +0.50883014254310877000 },
	{ +0.85930181835701080000, +0.51146885043797219000 },
	{ +0.85772861000027445000, +0.51410274419322355000 },
	{ +0.85614732837519691000, +0.51673179901765176000 },
	{ +0.85455798836540298000, +0.51935599016559153000 },
	{ +0.85296060493036607000, +0.52197529293715628000 },
	{ +0.85135519310526753000, +0.52458968267847084000 },
	{ +0.84974176800085488000, +0.52719913478190328000 },
	{ +0.84812034480329968000, +0.52980362468629671000 },
	{ +0.84649093877405446000, +0.53240312787720012000 },
	{ +0.84485356524970945000, +0.53499761988709948000 },
	{ +0.84320823964184788000, +0.53758707629564773000 },
	{ +0.84155497743690089000, +0.54017147272989507000 },
	{ +0.83989379419600196000, +0.54275078486451811000 },
	{ +0.83822470555484041000, +0.54532498842204868000 },
	{ +0.83654772722351434000, +0.54789405917310241000 },
	{ +0.83486287498638245000, +0.55045797293660714000 },
	{ +0.83317016470191563000, +0.55301670558002991000 },
	{ +0.83146961230254768000, +0.55557023301960462000 },
	{ +0.82976123379452549000, +0.55811853122055854000 },
	{ +0.82804504525775824000, +0.56066157619733847000 },
	{ +0.82632106284566598000, +0.56319934401383653000 },
	{ +0.82458930278502784000, +0.56573181078361567000 },
	{ +0.82284978137582887000, +0.56825895267013404000 },
	{ +0.82110251499110720000, +0.57078074588696981000 },
	{ +0.81934752007679945000, +0.57329716669804476000 },
	{ +0.81758481315158626000, +0.57580819141784789000 },
	{ +0.81581441080673633000, +0.57831379641165814000 },
	{ +0.81403632970595086000, +0.58081395809576719000 },
	{ +0.81225058658520644000, +0.58330865293770096000 },
	{ +0.81045719825259732000, +0.58579785745644153000 },
	{ +0.80865618158817754000, +0.58828154822264800000 },
	{ +0.80684755354380189000, +0.59075970185887694000 },
	{ +0.80503133114296621000, +0.59323229503980257000 },
	{ +0.80320753148064761000, +0.59569930449243613000 },
	{ +0.80137617172314302000, +0.59816070699634505000 },
	{ +0.79953726910790790000, +0.60061647938387164000 },
	{ +0.79769084094339404000, +0.60306659854035094000 },
	{ +0.79583690460888656000, +0.60551104140432821000 },
	{ +0.79397547755434028000, +0.60794978496777641000 },
	{ +0.79210657730021539000, +0.61038280627631225000 },
	{ +0.79023022143731314000, +0.61281008242941248000 },
	{ +0.78834642762660934000, +0.61523159058062971000 },
	{ +0.78645521359908876000, +0.61764730793780676000 },
	{ +0.78455659715557824000, +0.62005721176329198000 },
	{ +0.78265059616657884000, +0.62246127937415274000 },
	{ +0.78073722857209749000, +0.62485948814238912000 },
	{ +0.77881651238147898000, +0.62725181549514686000 },
	{ +0.77688846567323555000, +0.62963823891492987000 },
	{ +0.77495310659487704000, +0.63201873593981195000 },
	{ +0.77301045336274010000, +0.63439328416364837000 },
	{ +0.77106052426181682000, +0.63676186123628709000 },
	{ +0.76910333764558270000, +0.63912444486377862000 },
	{ +0.76713891193582351000, +0.64148101280858605000 },
	{ +0.76516726562246207000, +0.64383154288979438000 },
	{ +0.76318841726338438000, +0.64617601298331928000 },
	{ +0.76120238548426500000, +0.64851440102211544000 },
	{ +0.75920918897839118000, +0.65084668499638387000 },
	{ +0.75720884650648779000, +0.65317284295377975000 },
	{ +0.75520137689653988000, +0.65549285299961846000 },
	{ +0.75318679904361585000, +0.65780669329708175000 },
	{ +0.75116513190968981000, +0.66011434206742359000 },
	{ +0.74913639452346270000, +0.66241577759017489000 },
	{ +0.74710060598018357000, +0.66471097820334801000 },
	{ +0.74505778544146939000, +0.66699992230364069000 },
	{ +0.74300795213512505000, +0.66928258834663934000 },
	{ +0.74095112535496244000, +0.67155895484702166000 },
	{ +0.73888732446061856000, +0.67382900037875926000 },
	{ +0.73681656887737335000, +0.67609270357531914000 },
	{ +0.73473887809596694000, +0.67835004312986469000 },
	{ +0.73265427167241626000, +0.68060099779545624000 },
	{ +0.73056276922783092000, +0.68284554638525130000 },
	{ +0.72846439044822853000, +0.68508366777270358000 },
	{ +0.72635915508434934000, +0.68731534089176238000 },
	{ +0.72424708295147033000, +0.68954054473707016000 },
	{ +0.72212819392921868000, +0.69175925836416108000 },
	{ +0.72000250796138499000, +0.69397146088965733000 },
	{ +0.71787004505573504000, +0.69617713149146632000 },
	{ +0.71573082528382193000, +0.69837624940897625000 },
	{ +0.71358486878079697000, +0.70056879394325178000 },
	{ +0.71143219574521976000, +0.70275474445722874000 },
	{ +0.70927282643886891000, +0.70493408037590843000 },
	{ +0.70710678118655079000, +0.70710678118655101000 },
	{ +0.70493408037590821000, +0.70927282643886913000 },
	{ +0.70275474445722863000, +0.71143219574521988000 },
	{ +0.70056879394325167000, +0.71358486878079708000 },
	{ +0.69837624940897625000, +0.71573082528382215000 },
	{ +0.69617713149146632000, +0.71787004505573526000 },
	{ +0.69397146088965744000, +0.72000250796138521000 },
	{ +0.69175925836416119000, +0.72212819392921890000 },
	{ +0.68954054473707038000, +0.72424708295147044000 },
	{ +0.68731534089176261000, +0.72635915508434956000 },
	{ +0.68508366777270391000, +0.72846439044822875000 },
	{ +0.68284554638525163000, +0.73056276922783114000 },
	{ +0.68060099779545657000, +0.73265427167241637000 },
	{ +0.67835004312986502000, +0.73473887809596705000 },
	{ +0.67609270357531948000, +0.73681656887737346000 },
	{ +0.67382900037875948000, +0.73888732446061878000 },
	{ +0.67155895484702188000, +0.74095112535496277000 },
	{ +0.66928258834663956000, +0.74300795213512538000 },
	{ +0.66699992230364102000, +0.74505778544146972000 },
	{ +0.66471097820334835000, +0.74710060598018391000 },
	{ +0.66241577759017523000, +0.74913639452346303000 },
	{ +0.66011434206742392000, +0.75116513190969014000 },
	{ +0.65780669329708219000, +0.75318679904361630000 },
	{ +0.65549285299961890000, +0.75520137689654032000 },
	{ +0.65317284295378031000, +0.75720884650648834000 },
	{ +0.65084668499638443000, +0.75920918897839196000 },
	{ +0.64851440102211588000, +0.76120238548426578000 },
	{ +0.64617601298331984000, +0.76318841726338527000 },
	{ +0.64383154288979494000, +0.76516726562246296000 },
	{ +0.64148101280858660000, +0.76713891193582440000 },
	{ +0.63912444486377917000, +0.76910333764558370000 },
	{ +0.63676186123628775000, +0.77106052426181781000 },
	{ +0.63439328416364893000, +0.77301045336274099000 },
	{ +0.63201873593981250000, +0.77495310659487793000 },
	{ +0.62963823891493054000, +0.77688846567323655000 },
	{ +0.62725181549514764000, +0.77881651238148009000 },
	{ +0.62485948814238990000, +0.78073722857209871000 },
	{ +0.62246127937415352000, +0.78265059616658006000 },
	{ +0.62005721176329276000, +0.78455659715557957000 },
	{ +0.61764730793780753000, +0.78645521359909021000 },
	{ +0.61523159058063048000, +0.78834642762661078000 },
	{ +0.61281008242941337000, +0.79023022143731458000 },
	{ +0.61038280627631314000, +0.79210657730021694000 },
	{ +0.60794978496777730000, +0.79397547755434172000 },
	{ +0.60551104140432910000, +0.79583690460888812000 },
	{ +0.60306659854035183000, +0.79769084094339571000 },
	{ +0.60061647938387253000, +0.79953726910790968000 },
	{ +0.59816070699634594000, +0.80137617172314490000 },
	{ +0.59569930449243691000, +0.80320753148064961000 },
	{ +0.59323229503980335000, +0.80503133114296832000 },
	{ +0.59075970185887783000, +0.80684755354380400000 },
	{ +0.58828154822264889000, +0.80865618158817976000 },
	{ +0.58579785745644253000, +0.81045719825259954000 },
	{ +0.58330865293770195000, +0.81225058658520866000 },
	{ +0.58081395809576819000, +0.81403632970595308000 },
	{ +0.57831379641165925000, +0.81581441080673855000 },
	{ +0.57580819141784900000, +0.81758481315158849000 },
	{ +0.57329716669804587000, +0.81934752007680178000 },
	{ +0.57078074588697092000, +0.82110251499110953000 },
	{ +0.56825895267013526000, +0.82284978137583120000 },
	{ +0.56573181078361701000, +0.82458930278503018000 },
	{ +0.56319934401383787000, +0.82632106284566842000 },
	{ +0.56066157619733981000, +0.82804504525776079000 },
	{ +0.55811853122055988000, +0.82976123379452815000 },
	{ +0.55557023301960606000, +0.83146961230255034000 },
	{ +0.55301670558003135000, +0.83317016470191829000 },
	{ +0.55045797293660870000, +0.83486287498638523000 },
	{ +0.54789405917310408000, +0.83654772722351722000 },
	{ +0.54532498842205035000, +0.83822470555484330000 },
	{ +0.54275078486451989000, +0.83989379419600485000 },
	{ +0.54017147272989685000, +0.84155497743690377000 },
	{ +0.53758707629564939000, +0.84320823964185088000 },
	{ +0.53499761988710104000, +0.84485356524971256000 },
	{ +0.53240312787720179000, +0.84649093877405757000 },
	{ +0.52980362468629849000, +0.84812034480330267000 },
	{ +0.52719913478190517000, +0.84974176800085799000 },
	{ +0.52458968267847272000, +0.85135519310527075000 },
	{ +0.52197529293715816000, +0.85296060493036929000 },
	{ +0.51935599016559342000, +0.85455798836540620000 },
	{ +0.51673179901765365000, +0.85614732837520013000 },
	{ +0.51410274419322544000, +0.85772861000027767000 },
	{ +0.51146885043797408000, +0.85930181835701402000 },
	{ +0.50883014254311065000, +0.86086693863777297000 },
	{ +0.50618664534515889000, +0.86242395611104627000 },
	{ +0.50353838372572113000, +0.86397285612159247000 },
	{ +0.50088538261124427000, +0.86551362409057475000 },
	{ +0.49822766697278537000, +0.86704624551569831000 },
	{ +0.49556526182577604000, +0.86857070597134656000 },
	{ +0.49289819222978754000, +0.87008699110871712000 },
	{ +0.49022648328829466000, +0.87159508665595675000 },
	{ +0.48755016014843944000, +0.87309497841829586000 },
	{ +0.48486924800079456000, +0.87458665227818189000 },
	{ +0.48218377207912622000, +0.87607009419541237000 },
	{ +0.47949375766015651000, +0.87754529020726701000 },
	{ +0.47679923006332564000, +0.87901222642863919000 },
	{ +0.47410021465055352000, +0.88047088905216653000 },
	{ +0.47139673682600114000, +0.88192126434836082000 },
	{ +0.46868882203583140000, +0.88336333866573746000 },
	{ +0.46597649576796962000, +0.88479709843094367000 },
	{ +0.46325978355186359000, +0.88622253014888652000 },
	{ +0.46053871095824339000, +0.88763962040285982000 },
	{ +0.45781330359888062000, +0.88904835585467046000 },
	{ +0.45508358712634717000, +0.89044872324476376000 },
	{ +0.45234958723377422000, +0.89184070939234861000 },
	{ +0.44961132965460993000, +0.89322430119552121000 },
	{ +0.44686884016237755000, +0.89459948563138858000 },
	{ +0.44412214457043259000, +0.89596624975619110000 },
	{ +0.44137126873172006000, +0.89732458070542420000 },
	{ +0.43861623853853099000, +0.89867446569395981000 },
	{ +0.43585707992225886000, +0.90001589201616627000 },
	{ +0.43309381885315534000, +0.90134884704602813000 },
	{ +0.43032648134008600000, +0.90267331823726493000 },
	{ +0.42755509343028542000, +0.90398929312344956000 },
	{ +0.42477968120911214000, +0.90529675931812492000 },
	{ +0.42200027079980296000, +0.90659570451492155000 },
	{ +0.41921688836322724000, +0.90788611648767237000 },
	{ +0.41642956009764048000, +0.90916798309052860000 },
	{ +0.41363831223843783000, +0.91044129225807346000 },
	{ +0.41084317105790719000, +0.91170603200543610000 },
	{ +0.40804416286498191000, +0.91296219042840443000 },
	{ +0.40524131400499308000, +0.91420975570353702000 },
	{ +0.40243465085942165000, +0.91544871608827416000 },
	{ +0.39962419984565001000, +0.91667905992104903000 },
	{ +0.39680998741671347000, +0.91790077562139682000 },
	{ +0.39399204006105126000, +0.91911385169006410000 },
	{ +0.39117038430225703000, +0.92031827670911692000 },
	{ +0.38834504669882941000, +0.92151403934204823000 },
	{ +0.38551605384392196000, +0.92270112833388485000 },
	{ +0.38268343236509283000, +0.92387953251129296000 },
	{ +0.37984720892405421000, +0.92504924078268380000 },
	{ +0.37700741021642126000, +0.92621024213831760000 },
	{ +0.37416406297146099000, +0.92736252565040733000 },
	{ +0.37131719395184049000, +0.92850608047322192000 },
	{ +0.36846682995337526000, +0.92964089584318765000 },
	{ +0.36561299780477680000, +0.93076696107899015000 },
	{ +0.36275572436740011000, +0.93188426558167459000 },
	{ +0.35989503653499105000, +0.93299279883474540000 },
	{ +0.35703096123343292000, +0.93409255040426542000 },
	{ +0.35416352542049329000, +0.93518350993895416000 },
	{ +0.35129275608557003000, +0.93626566717028481000 },
	{ +0.34841868024943745000, +0.93733901191258151000 },
	{ +0.34554132496399198000, +0.93840353406311472000 },
	{ +0.34266071731199726000, +0.93945922360219658000 },
	{ +0.33977688440682968000, +0.94050607059327496000 },
	{ +0.33688985339222283000, +0.94154406518302736000 },
	{ +0.33399965144201216000, +0.94257319760145353000 },
	{ +0.33110630575987915000, +0.94359345816196705000 },
	{ +0.32820984357909527000, +0.94460483726148703000 },
	{ +0.32531029216226565000, +0.94560732538052816000 },
	{ +0.32240767880107252000, +0.94660091308329042000 },
	{ +0.31950203081601830000, +0.94758559101774797000 },
	{ +0.31659337555616845000, +0.94856134991573726000 },
	{ +0.31368174039889402000, +0.94952818059304367000 },
	{ +0.31076715274961403000, +0.95048607394948870000 },
	{ +0.30784964004153742000, +0.95143502096901533000 },
	{ +0.30492922973540493000, +0.95237501271977276000 },
	{ +0.30200594931923058000, +0.95330604035420075000 },
	{ +0.29907982630804297000, +0.95422809510911255000 },
	{ +0.29615088824362629000, +0.95514116830577767000 },
	{ +0.29321916269426107000, +0.95604525135000340000 },
	{ +0.29028467725446477000, +0.95694033573221582000 },
	{ +0.28734745954473195000, +0.95782641302753990000 },
	{ +0.28440753721127421000, +0.95870347489587859000 },
	{ +0.28146493792576033000, +0.95957151308199162000 },
	{ +0.27851968938505545000, +0.96043051941557289000 },
	{ +0.27557181931096048000, +0.96128048581132786000 },
	{ +0.27262135544995125000, +0.96212140426904880000 },
	{ +0.26966832557291737000, +0.96295326687369120000 },
	{ +0.26671275747490064000, +0.96377606579544728000 },
	{ +0.26375467897483362000, +0.96458979328982020000 },
	{ +0.26079411791527773000, +0.96539444169769684000 },
	{ +0.25783110216216121000, +0.96619000344542005000 },
	{ +0.25486565960451679000, +0.96697647104485962000 },
	{ +0.25189781815421913000, +0.96775383709348306000 },
	{ +0.24892760574572237000, +0.96852209427442493000 },
	{ +0.24595505033579679000, +0.96928123535655608000 },
	{ +0.24298017990326604000, +0.97003125319455152000 },
	{ +0.24000302244874361000, +0.97077214072895790000 },
	{ +0.23702360599436931000, +0.97150389098625944000 },
	{ +0.23404195858354551000, +0.97222649707894404000 },
	{ +0.23105810828067319000, +0.97293995220556784000 },
	{ +0.22807208317088778000, +0.97364424965081964000 },
	{ +0.22508391135979489000, +0.97433938278558363000 },
	{ +0.22209362097320556000, +0.97502534506700189000 },
	{ +0.21910124015687182000, +0.97570213003853634000 },
	{ +0.21610679707622152000, +0.97636973133002891000 },
	{ +0.21311031991609336000, +0.97702814265776217000 },
	{ +0.21011183688047161000, +0.97767735782451781000 },
	{ +0.20711137619222050000, +0.97831737071963543000 },
	{ +0.20410896609281881000, +0.97894817531906997000 },
	{ +0.20110463484209382000, +0.97956976568544840000 },
	{ +0.19809841071795548000, +0.98018213596812531000 },
	{ +0.19509032201613014000, +0.98078528040323842000 },
	{ +0.19208039704989427000, +0.98137919331376255000 },
	{ +0.18906866414980802000, +0.98196386910956335000 },
	{ +0.18605515166344844000, +0.98253930228744946000 },
	{ +0.18303988795514273000, +0.98310548743122450000 },
	{ +0.18002290140570126000, +0.98366241921173847000 },
	{ +0.17700422041215050000, +0.98421009238693735000 },
	{ +0.17398387338746554000, +0.98474850180191253000 },
	{ +0.17096188876030291000, +0.98527764238894955000 },
	{ +0.16793829497473284000, +0.98579750916757580000 },
	{ +0.16491312048997156000, +0.98630809724460711000 },
	{ +0.16188639378011344000, +0.98680940181419396000 },
	{ +0.15885814333386303000, +0.98730141815786687000 },
	{ +0.15582839765426679000, +0.98778414164458073000 },
	{ +0.15279718525844496000, +0.98825756773075812000 },
	{ +0.14976453467732304000, +0.98872169196033244000 },
	{ +0.14673047445536325000, +0.98917650996478967000 },
	{ +0.14369503315029591000, +0.98962201746320955000 },
	{ +0.14065823933285065000, +0.99005821026230578000 },
	{ +0.13762012158648745000, +0.99048508425646575000 },
	{ +0.13458070850712758000, +0.99090263542778878000 },
	{ +0.13154002870288448000, +0.99131085984612421000 },
	{ +0.12849811079379450000, +0.99170975366910830000 },
	{ +0.12545498341154754000, +0.99209931314220046000 },
	{ +0.12241067519921747000, +0.99247953459871874000 },
	{ +0.11936521481099262000, +0.99285041445987388000 },
	{ +0.11631863091190600000, +0.99321194923480338000 },
	{ +0.11327095217756555000, +0.99356413552060419000 },
	{ +0.11022220729388424000, +0.99390697000236494000 },
	{ +0.10717242495681001000, +0.99424044945319690000 },
	{ +0.10412163387205571000, +0.99456457073426441000 },
	{ +0.10106986275482893000, +0.99487933079481461000 },
	{ +0.09801714032956168600, +0.99518472667220592000 },
	{ +0.09496349532964006000, +0.99548075549193604000 },
	{ +0.09190895649713376500, +0.99576741446766892000 },
	{ +0.08885355258252560000, +0.99604470090126118000 },
	{ +0.08579731234444086500, +0.99631261218278722000 },
	{ +0.08274026454937664900, +0.99657114579056405000 },
	{ +0.07968243797143104200, +0.99682029929117499000 },
	{ +0.07662386139203238000, +0.99706007033949229000 },
	{ +0.07356456359966828700, +0.99729045667869953000 },
	{ +0.07050457338961470300, +0.99751145614031278000 },
	{ +0.06744391956366487000, +0.99772306664420096000 },
	{ +0.06438263092985825600, +0.99792528619860543000 },
	{ +0.06132073630220934900, +0.99811811290015873000 },
	{ +0.05825826450043650200, +0.99830154493390244000 },
	{ +0.05519524434969064900, +0.99847558057330432000 },
	{ +0.05213170468028400400, +0.99864021818027482000 },
	{ +0.04906767432741866700, +0.99879545620518195000 },
	{ +0.04600318213091524700, +0.99894129318686642000 },
	{ +0.04293825693494141700, +0.99907772775265502000 },
	{ +0.03987292758774037300, +0.99920475861837355000 },
	{ +0.03680722294135936600, +0.99932238458835920000 },
	{ +0.03374117185137809300, +0.99943060455547150000 },
	{ +0.03067480317663710500, +0.99952941750110291000 },
	{ +0.02760814577896619100, +0.99961882249518830000 },
	{ +0.02454122852291270800, +0.99969881869621391000 },
	{ +0.02147408027546989600, +0.99976940535122494000 },
	{ +0.01840672990580518100, +0.99983058179583306000 },
	{ +0.01533920628498843000, +0.99988234745422222000 },
	{ +0.01227153828572022500, +0.99992470183915427000 },
	{ +0.00920375478206008830, +0.99995764455197367000 },
	{ +0.00613588464915471550, +0.99998117528261088000 },
	{ +0.00306795676296618730, +0.99999529380958596000 },
	{ +0.00000000000000018128, +1.00000000000000980000 },
	{ -0.00306795676296582480, +0.99999529380958596000 },
	{ -0.00613588464915435380, +0.99998117528261099000 },
	{ -0.00920375478205972750, +0.99995764455197367000 },
	{ -0.01227153828571986500, +0.99992470183915438000 },
	{ -0.01533920628498806900, +0.99988234745422244000 },
	{ -0.01840672990580482000, +0.99983058179583340000 },
	{ -0.02147408027546953600, +0.99976940535122527000 },
	{ -0.02454122852291234700, +0.99969881869621424000 },
	{ -0.02760814577896583000, +0.99961882249518863000 },
	{ -0.03067480317663674400, +0.99952941750110325000 },
	{ -0.03374117185137773300, +0.99943060455547184000 },
	{ -0.03680722294135901200, +0.99932238458835954000 },
	{ -0.03987292758774002600, +0.99920475861837399000 },
	{ -0.04293825693494107000, +0.99907772775265546000 },
	{ -0.04600318213091490700, +0.99894129318686697000 },
	{ -0.04906767432741832700, +0.99879545620518251000 },
	{ -0.05213170468028366400, +0.99864021818027526000 },
	{ -0.05519524434969031600, +0.99847558057330477000 },
	{ -0.05825826450043616900, +0.99830154493390288000 },
	{ -0.06132073630220901500, +0.99811811290015928000 },
	{ -0.06438263092985792300, +0.99792528619860610000 },
	{ -0.06744391956366455100, +0.99772306664420174000 },
	{ -0.07050457338961439700, +0.99751145614031367000 },
	{ -0.07356456359966799500, +0.99729045667870042000 },
	{ -0.07662386139203208900, +0.99706007033949329000 },
	{ -0.07968243797143075000, +0.99682029929117599000 },
	{ -0.08274026454937635800, +0.99657114579056516000 },
	{ -0.08579731234444058800, +0.99631261218278833000 },
	{ -0.08885355258252533600, +0.99604470090126240000 },
	{ -0.09190895649713350100, +0.99576741446767014000 },
	{ -0.09496349532963979700, +0.99548075549193726000 },
	{ -0.09801714032956143600, +0.99518472667220725000 },
	{ -0.10106986275482870000, +0.99487933079481605000 },
	{ -0.10412163387205549000, +0.99456457073426585000 },
	{ -0.10717242495680979000, +0.99424044945319845000 },
	{ -0.11022220729388403000, +0.99390697000236650000 },
	{ -0.11327095217756536000, +0.99356413552060574000 },
	{ -0.11631863091190581000, +0.99321194923480494000 },
	{ -0.11936521481099244000, +0.99285041445987554000 },
	{ -0.12241067519921731000, +0.99247953459872051000 },
	{ -0.12545498341154740000, +0.99209931314220234000 },
	{ -0.12849811079379436000, +0.99170975366911007000 },
	{ -0.13154002870288431000, +0.99131085984612599000 },
	{ -0.13458070850712742000, +0.99090263542779067000 },
	{ -0.13762012158648732000, +0.99048508425646775000 },
	{ -0.14065823933285054000, +0.99005821026230789000 },
	{ -0.14369503315029580000, +0.98962201746321155000 },
	{ -0.14673047445536314000, +0.98917650996479167000 },
	{ -0.14976453467732292000, +0.98872169196033455000 },
	{ -0.15279718525844488000, +0.98825756773076034000 },
	{ -0.15582839765426673000, +0.98778414164458306000 },
	{ -0.15885814333386297000, +0.98730141815786931000 },
	{ -0.16188639378011341000, +0.98680940181419641000 },
	{ -0.16491312048997153000, +0.98630809724460955000 },
	{ -0.16793829497473281000, +0.98579750916757836000 },
	{ -0.17096188876030288000, +0.98527764238895221000 },
	{ -0.17398387338746552000, +0.98474850180191520000 },
	{ -0.17700422041215047000, +0.98421009238694013000 },
	{ -0.18002290140570126000, +0.98366241921174136000 },
	{ -0.18303988795514275000, +0.98310548743122739000 },
	{ -0.18605515166344846000, +0.98253930228745223000 },
	{ -0.18906866414980805000, +0.98196386910956623000 },
	{ -0.19208039704989432000, +0.98137919331376555000 },
	{ -0.19509032201613019000, +0.98078528040324142000 },
	{ -0.19809841071795553000, +0.98018213596812831000 },
	{ -0.20110463484209390000, +0.97956976568545151000 },
	{ -0.20410896609281889000, +0.97894817531907319000 },
	{ -0.20711137619222059000, +0.97831737071963865000 },
	{ -0.21011183688047169000, +0.97767735782452103000 },
	{ -0.21311031991609347000, +0.97702814265776539000 },
	{ -0.21610679707622166000, +0.97636973133003213000 },
	{ -0.21910124015687196000, +0.97570213003853956000 },
	{ -0.22209362097320573000, +0.97502534506700522000 },
	{ -0.22508391135979508000, +0.97433938278558696000 },
	{ -0.22807208317088803000, +0.97364424965082308000 },
	{ -0.23105810828067347000, +0.97293995220557128000 },
	{ -0.23404195858354582000, +0.97222649707894737000 },
	{ -0.23702360599436964000, +0.97150389098626289000 },
	{ -0.24000302244874397000, +0.97077214072896145000 },
	{ -0.24298017990326642000, +0.97003125319455519000 },
	{ -0.24595505033579718000, +0.96928123535655963000 },
	{ -0.24892760574572276000, +0.96852209427442848000 },
	{ -0.25189781815421958000, +0.96775383709348661000 },
	{ -0.25486565960451724000, +0.96697647104486328000 },
	{ -0.25783110216216171000, +0.96619000344542372000 },
	{ -0.26079411791527829000, +0.96539444169770061000 },
	{ -0.26375467897483418000, +0.96458979328982397000 },
	{ -0.26671275747490120000, +0.96377606579545116000 },
	{ -0.26966832557291798000, +0.96295326687369520000 },
	{ -0.27262135544995192000, +0.96212140426905290000 },
	{ -0.27557181931096114000, +0.96128048581133196000 },
	{ -0.27851968938505611000, +0.96043051941557711000 },
	{ -0.28146493792576105000, +0.95957151308199584000 },
	{ -0.28440753721127493000, +0.95870347489588292000 },
	{ -0.28734745954473268000, +0.95782641302754423000 },
	{ -0.29028467725446555000, +0.95694033573222015000 },
	{ -0.29321916269426190000, +0.95604525135000773000 },
	{ -0.29615088824362712000, +0.95514116830578200000 },
	{ -0.29907982630804381000, +0.95422809510911688000 },
	{ -0.30200594931923141000, +0.95330604035420508000 },
	{ -0.30492922973540576000, +0.95237501271977709000 },
	{ -0.30784964004153831000, +0.95143502096901966000 },
	{ -0.31076715274961497000, +0.95048607394949303000 },
	{ -0.31368174039889501000, +0.94952818059304800000 },
	{ -0.31659337555616945000, +0.94856134991574159000 },
	{ -0.31950203081601930000, +0.94758559101775242000 },
	{ -0.32240767880107352000, +0.94660091308329486000 },
	{ -0.32531029216226665000, +0.94560732538053260000 },
	{ -0.32820984357909627000, +0.94460483726149147000 },
	{ -0.33110630575988015000, +0.94359345816197160000 },
	{ -0.33399965144201321000, +0.94257319760145808000 },
	{ -0.33688985339222388000, +0.94154406518303202000 },
	{ -0.33977688440683068000, +0.94050607059327951000 },
	{ -0.34266071731199826000, +0.93945922360220102000 },
	{ -0.34554132496399298000, +0.93840353406311927000 },
	{ -0.34841868024943851000, +0.93733901191258606000 },
	{ -0.35129275608557115000, +0.93626566717028947000 },
	{ -0.35416352542049445000, +0.93518350993895871000 },
	{ -0.35703096123343414000, +0.93409255040427008000 },
	{ -0.35989503653499233000, +0.93299279883475006000 },
	{ -0.36275572436740144000, +0.93188426558167925000 },
	{ -0.36561299780477813000, +0.93076696107899493000 },
	{ -0.36846682995337665000, +0.92964089584319243000 },
	{ -0.37131719395184193000, +0.92850608047322669000 },
	{ -0.37416406297146243000, +0.92736252565041222000 },
	{ -0.37700741021642270000, +0.92621024213832248000 },
	{ -0.37984720892405566000, +0.92504924078268880000 },
	{ -0.38268343236509428000, +0.92387953251129795000 },
	{ -0.38551605384392340000, +0.92270112833388984000 },
	{ -0.38834504669883085000, +0.92151403934205312000 },
	{ -0.39117038430225848000, +0.92031827670912181000 },
	{ -0.39399204006105271000, +0.91911385169006898000 },
	{ -0.39680998741671497000, +0.91790077562140171000 },
	{ -0.39962419984565151000, +0.91667905992105392000 },
	{ -0.40243465085942315000, +0.91544871608827905000 },
	{ -0.40524131400499458000, +0.91420975570354179000 },
	{ -0.40804416286498341000, +0.91296219042840931000 },
	{ -0.41084317105790874000, +0.91170603200544098000 },
	{ -0.41363831223843939000, +0.91044129225807824000 },
	{ -0.41642956009764204000, +0.90916798309053337000 },
	{ -0.41921688836322885000, +0.90788611648767725000 },
	{ -0.42200027079980462000, +0.90659570451492644000 },
	{ -0.42477968120911380000, +0.90529675931812981000 },
	{ -0.42755509343028708000, +0.90398929312345433000 },
	{ -0.43032648134008766000, +0.90267331823726971000 },
	{ -0.43309381885315701000, +0.90134884704603291000 },
	{ -0.43585707992226058000, +0.90001589201617116000 },
	{ -0.43861623853853277000, +0.89867446569396481000 },
	{ -0.44137126873172189000, +0.89732458070542920000 },
	{ -0.44412214457043447000, +0.89596624975619610000 },
	{ -0.44686884016237949000, +0.89459948563139358000 },
	{ -0.44961132965461192000, +0.89322430119552620000 },
	{ -0.45234958723377627000, +0.89184070939235360000 },
	{ -0.45508358712634928000, +0.89044872324476876000 },
	{ -0.45781330359888273000, +0.88904835585467545000 },
	{ -0.46053871095824556000, +0.88763962040286482000 },
	{ -0.46325978355186581000, +0.88622253014889141000 },
	{ -0.46597649576797184000, +0.88479709843094856000 },
	{ -0.46868882203583362000, +0.88336333866574235000 },
	{ -0.47139673682600336000, +0.88192126434836582000 },
	{ -0.47410021465055574000, +0.88047088905217152000 },
	{ -0.47679923006332792000, +0.87901222642864418000 },
	{ -0.47949375766015884000, +0.87754529020727201000 },
	{ -0.48218377207912860000, +0.87607009419541737000 },
	{ -0.48486924800079700000, +0.87458665227818688000 },
	{ -0.48755016014844188000, +0.87309497841830086000 },
	{ -0.49022648328829710000, +0.87159508665596186000 },
	{ -0.49289819222979003000, +0.87008699110872223000 },
	{ -0.49556526182577859000, +0.86857070597135166000 },
	{ -0.49822766697278792000, +0.86704624551570342000 },
	{ -0.50088538261124693000, +0.86551362409057986000 },
	{ -0.50353838372572379000, +0.86397285612159747000 },
	{ -0.50618664534516156000, +0.86242395611105127000 },
	{ -0.50883014254311332000, +0.86086693863777797000 },
	{ -0.51146885043797674000, +0.85930181835701902000 },
	{ -0.51410274419322810000, +0.85772861000028267000 },
	{ -0.51673179901765631000, +0.85614732837520513000 },
	{ -0.51935599016559608000, +0.85455798836541108000 },
	{ -0.52197529293716083000, +0.85296060493037418000 },
	{ -0.52458968267847539000, +0.85135519310527563000 },
	{ -0.52719913478190783000, +0.84974176800086298000 },
	{ -0.52980362468630127000, +0.84812034480330778000 },
	{ -0.53240312787720467000, +0.84649093877406256000 },
	{ -0.53499761988710404000, +0.84485356524971755000 },
	{ -0.53758707629565228000, +0.84320823964185598000 },
	{ -0.54017147272989974000, +0.84155497743690899000 },
	{ -0.54275078486452277000, +0.83989379419601007000 },
	{ -0.54532498842205335000, +0.83822470555484851000 },
	{ -0.54789405917310718000, +0.83654772722352244000 },
	{ -0.55045797293661192000, +0.83486287498639056000 },
	{ -0.55301670558003468000, +0.83317016470192373000 },
	{ -0.55557023301960951000, +0.83146961230255578000 },
	{ -0.55811853122056343000, +0.82976123379453359000 },
	{ -0.56066157619734347000, +0.82804504525776634000 },
	{ -0.56319934401384153000, +0.82632106284567408000 },
	{ -0.56573181078362067000, +0.82458930278503584000 },
	{ -0.56825895267013904000, +0.82284978137583686000 },
	{ -0.57078074588697492000, +0.82110251499111520000 },
	{ -0.57329716669804998000, +0.81934752007680745000 },
	{ -0.57580819141785311000, +0.81758481315159426000 },
	{ -0.57831379641166336000, +0.81581441080674433000 },
	{ -0.58081395809577241000, +0.81403632970595885000 },
	{ -0.58330865293770617000, +0.81225058658521443000 },
	{ -0.58579785745644675000, +0.81045719825260532000 },
	{ -0.58828154822265322000, +0.80865618158818553000 },
	{ -0.59075970185888227000, +0.80684755354380977000 },
	{ -0.59323229503980790000, +0.80503133114297409000 },
	{ -0.59569930449244146000, +0.80320753148065538000 },
	{ -0.59816070699635038000, +0.80137617172315068000 },
	{ -0.60061647938387697000, +0.79953726910791556000 },
	{ -0.60306659854035627000, +0.79769084094340170000 },
	{ -0.60551104140433354000, +0.79583690460889422000 },
	{ -0.60794978496778174000, +0.79397547755434783000 },
	{ -0.61038280627631758000, +0.79210657730022294000 },
	{ -0.61281008242941781000, +0.79023022143732058000 },
	{ -0.61523159058063503000, +0.78834642762661677000 },
	{ -0.61764730793781220000, +0.78645521359909620000 },
	{ -0.62005721176329753000, +0.78455659715558568000 },
	{ -0.62246127937415840000, +0.78265059616658617000 },
	{ -0.62485948814239489000, +0.78073722857210481000 },
	{ -0.62725181549515263000, +0.77881651238148630000 },
	{ -0.62963823891493564000, +0.77688846567324277000 },
	{ -0.63201873593981772000, +0.77495310659488426000 },
	{ -0.63439328416365426000, +0.77301045336274732000 },
	{ -0.63676186123629308000, +0.77106052426182403000 },
	{ -0.63912444486378461000, +0.76910333764558991000 },
	{ -0.64148101280859215000, +0.76713891193583061000 },
	{ -0.64383154288980049000, +0.76516726562246917000 },
	{ -0.64617601298332539000, +0.76318841726339148000 },
	{ -0.64851440102212154000, +0.76120238548427210000 },
	{ -0.65084668499639009000, +0.75920918897839829000 },
	{ -0.65317284295378597000, +0.75720884650649478000 },
	{ -0.65549285299962468000, +0.75520137689654676000 },
	{ -0.65780669329708796000, +0.75318679904362273000 },
	{ -0.66011434206742980000, +0.75116513190969669000 },
	{ -0.66241577759018122000, +0.74913639452346958000 },
	{ -0.66471097820335434000, +0.74710060598019046000 },
	{ -0.66699992230364702000, +0.74505778544147627000 },
	{ -0.66928258834664567000, +0.74300795213513193000 },
	{ -0.67155895484702799000, +0.74095112535496932000 },
	{ -0.67382900037876570000, +0.73888732446062544000 },
	{ -0.67609270357532569000, +0.73681656887738023000 },
	{ -0.67835004312987124000, +0.73473887809597382000 },
	{ -0.68060099779546279000, +0.73265427167242314000 },
	{ -0.68284554638525785000, +0.73056276922783780000 },
	{ -0.68508366777271013000, +0.72846439044823541000 },
	{ -0.68731534089176893000, +0.72635915508435622000 },
	{ -0.68954054473707682000, +0.72424708295147711000 },
	{ -0.69175925836416774000, +0.72212819392922545000 },
	{ -0.69397146088966399000, +0.72000250796139165000 },
	{ -0.69617713149147298000, +0.71787004505574170000 },
	{ -0.69837624940898291000, +0.71573082528382859000 },
	{ -0.70056879394325844000, +0.71358486878080352000 },
	{ -0.70275474445723551000, +0.71143219574522631000 },
	{ -0.70493408037591521000, +0.70927282643887546000 },
	{ -0.70710678118655790000, +0.70710678118655723000 },
	{ -0.70927282643887601000, +0.70493408037591465000 },
	{ -0.71143219574522687000, +0.70275474445723496000 },
	{ -0.71358486878080407000, +0.70056879394325799000 },
	{ -0.71573082528382914000, +0.69837624940898246000 },
	{ -0.71787004505574237000, +0.69617713149147253000 },
	{ -0.72000250796139231000, +0.69397146088966366000 },
	{ -0.72212819392922600000, +0.69175925836416741000 },
	{ -0.72424708295147766000, +0.68954054473707660000 },
	{ -0.72635915508435678000, +0.68731534089176882000 },
	{ -0.72846439044823608000, +0.68508366777271001000 },
	{ -0.73056276922783847000, +0.68284554638525774000 },
	{ -0.73265427167242370000, +0.68060099779546268000 },
	{ -0.73473887809597438000, +0.67835004312987113000 },
	{ -0.73681656887738078000, +0.67609270357532558000 },
	{ -0.73888732446062610000, +0.67382900037876559000 },
	{ -0.74095112535497010000, +0.67155895484702788000 },
	{ -0.74300795213513271000, +0.66928258834664545000 },
	{ -0.74505778544147705000, +0.66699992230364691000 },
	{ -0.74710060598019123000, +0.66471097820335423000 },
	{ -0.74913639452347036000, +0.66241577759018111000 },
	{ -0.75116513190969747000, +0.66011434206742980000 },
	{ -0.75318679904362362000, +0.65780669329708796000 },
	{ -0.75520137689654765000, +0.65549285299962468000 },
	{ -0.75720884650649567000, +0.65317284295378608000 },
	{ -0.75920918897839929000, +0.65084668499639020000 },
	{ -0.76120238548427321000, +0.64851440102212166000 },
	{ -0.76318841726339270000, +0.64617601298332550000 },
	{ -0.76516726562247039000, +0.64383154288980060000 },
	{ -0.76713891193583184000, +0.64148101280859227000 },
	{ -0.76910333764559113000, +0.63912444486378484000 },
	{ -0.77106052426182536000, +0.63676186123629330000 },
	{ -0.77301045336274854000, +0.63439328416365448000 },
	{ -0.77495310659488548000, +0.63201873593981794000 },
	{ -0.77688846567324410000, +0.62963823891493587000 },
	{ -0.77881651238148764000, +0.62725181549515296000 },
	{ -0.78073722857210626000, +0.62485948814239523000 },
	{ -0.78265059616658761000, +0.62246127937415885000 },
	{ -0.78455659715558712000, +0.62005721176329798000 },
	{ -0.78645521359909776000, +0.61764730793781264000 },
	{ -0.78834642762661833000, +0.61523159058063548000 },
	{ -0.79023022143732213000, +0.61281008242941826000 },
	{ -0.79210657730022449000, +0.61038280627631802000 },
	{ -0.79397547755434938000, +0.60794978496778218000 },
	{ -0.79583690460889578000, +0.60551104140433398000 },
	{ -0.79769084094340348000, +0.60306659854035671000 },
	{ -0.79953726910791745000, +0.60061647938387741000 },
	{ -0.80137617172315267000, +0.59816070699635082000 },
	{ -0.80320753148065738000, +0.59569930449244179000 },
	{ -0.80503133114297620000, +0.59323229503980823000 },
	{ -0.80684755354381199000, +0.59075970185888260000 },
	{ -0.80865618158818775000, +0.58828154822265366000 },
	{ -0.81045719825260765000, +0.58579785745644719000 },
	{ -0.81225058658521676000, +0.58330865293770662000 },
	{ -0.81403632970596118000, +0.58081395809577285000 },
	{ -0.81581441080674666000, +0.57831379641166392000 },
	{ -0.81758481315159659000, +0.57580819141785367000 },
	{ -0.81934752007680989000, +0.57329716669805053000 },
	{ -0.82110251499111764000, +0.57078074588697558000 },
	{ -0.82284978137583931000, +0.56825895267013982000 },
	{ -0.82458930278503828000, +0.56573181078362145000 },
	{ -0.82632106284567652000, +0.56319934401384231000 },
	{ -0.82804504525776890000, +0.56066157619734425000 },
	{ -0.82976123379453626000, +0.55811853122056432000 },
	{ -0.83146961230255845000, +0.55557023301961039000 },
	{ -0.83317016470192640000, +0.55301670558003568000 },
	{ -0.83486287498639333000, +0.55045797293661292000 },
	{ -0.83654772722352533000, +0.54789405917310829000 },
	{ -0.83822470555485140000, +0.54532498842205457000 },
	{ -0.83989379419601295000, +0.54275078486452399000 },
	{ -0.84155497743691188000, +0.54017147272990096000 },
	{ -0.84320823964185898000, +0.53758707629565350000 },
	{ -0.84485356524972066000, +0.53499761988710515000 },
	{ -0.84649093877406567000, +0.53240312787720590000 },
	{ -0.84812034480331078000, +0.52980362468630260000 },
	{ -0.84974176800086609000, +0.52719913478190927000 },
	{ -0.85135519310527885000, +0.52458968267847683000 },
	{ -0.85296060493037740000, +0.52197529293716227000 },
	{ -0.85455798836541430000, +0.51935599016559753000 },
	{ -0.85614732837520824000, +0.51673179901765776000 },
	{ -0.85772861000028588000, +0.51410274419322954000 },
	{ -0.85930181835702224000, +0.51146885043797818000 },
	{ -0.86086693863778119000, +0.50883014254311476000 },
	{ -0.86242395611105449000, +0.50618664534516300000 },
	{ -0.86397285612160069000, +0.50353838372572524000 },
	{ -0.86551362409058308000, +0.50088538261124838000 },
	{ -0.86704624551570664000, +0.49822766697278942000 },
	{ -0.86857070597135488000, +0.49556526182578009000 },
	{ -0.87008699110872545000, +0.49289819222979153000 },
	{ -0.87159508665596508000, +0.49022648328829865000 },
	{ -0.87309497841830419000, +0.48755016014844343000 },
	{ -0.87458665227819021000, +0.48486924800079856000 },
	{ -0.87607009419542070000, +0.48218377207913016000 },
	{ -0.87754529020727534000, +0.47949375766016045000 },
	{ -0.87901222642864751000, +0.47679923006332953000 },
	{ -0.88047088905217485000, +0.47410021465055735000 },
	{ -0.88192126434836915000, +0.47139673682600497000 },
	{ -0.88336333866574579000, +0.46868882203583523000 },
	{ -0.88479709843095200000, +0.46597649576797345000 },
	{ -0.88622253014889485000, +0.46325978355186742000 },
	{ -0.88763962040286815000, +0.46053871095824722000 },
	{ -0.88904835585467878000, +0.45781330359888439000 },
	{ -0.89044872324477209000, +0.45508358712635094000 },
	{ -0.89184070939235693000, +0.45234958723377794000 },
	{ -0.89322430119552954000, +0.44961132965461365000 },
	{ -0.89459948563139691000, +0.44686884016238121000 },
	{ -0.89596624975619943000, +0.44412214457043619000 },
	{ -0.89732458070543253000, +0.44137126873172366000 },
	{ -0.89867446569396814000, +0.43861623853853454000 },
	{ -0.90001589201617460000, +0.43585707992226236000 },
	{ -0.90134884704603646000, +0.43309381885315879000 },
	{ -0.90267331823727326000, +0.43032648134008938000 },
	{ -0.90398929312345788000, +0.42755509343028880000 },
	{ -0.90529675931813336000, +0.42477968120911552000 },
	{ -0.90659570451492999000, +0.42200027079980634000 },
	{ -0.90788611648768081000, +0.41921688836323057000 },
	{ -0.90916798309053704000, +0.41642956009764376000 },
	{ -0.91044129225808190000, +0.41363831223844105000 },
	{ -0.91170603200544464000, +0.41084317105791041000 },
	{ -0.91296219042841298000, +0.40804416286498513000 },
	{ -0.91420975570354557000, +0.40524131400499630000 },
	{ -0.91544871608828271000, +0.40243465085942481000 },
	{ -0.91667905992105758000, +0.39962419984565312000 },
	{ -0.91790077562140548000, +0.39680998741671658000 },
	{ -0.91911385169007276000, +0.39399204006105432000 },
	{ -0.92031827670912558000, +0.39117038430226003000 },
	{ -0.92151403934205689000, +0.38834504669883235000 },
	{ -0.92270112833389362000, +0.38551605384392490000 },
	{ -0.92387953251130184000, +0.38268343236509578000 },
	{ -0.92504924078269268000, +0.37984720892405716000 },
	{ -0.92621024213832648000, +0.37700741021642420000 },
	{ -0.92736252565041633000, +0.37416406297146393000 },
	{ -0.92850608047323091000, +0.37131719395184343000 },
	{ -0.92964089584319665000, +0.36846682995337815000 },
	{ -0.93076696107899914000, +0.36561299780477968000 },
	{ -0.93188426558168358000, +0.36275572436740300000 },
	{ -0.93299279883475439000, +0.35989503653499388000 },
	{ -0.93409255040427441000, +0.35703096123343570000 },
	{ -0.93518350993896315000, +0.35416352542049601000 },
	{ -0.93626566717029380000, +0.35129275608557270000 },
	{ -0.93733901191259050000, +0.34841868024944012000 },
	{ -0.93840353406312371000, +0.34554132496399459000 },
	{ -0.93945922360220557000, +0.34266071731199987000 },
	{ -0.94050607059328406000, +0.33977688440683229000 },
	{ -0.94154406518303657000, +0.33688985339222544000 },
	{ -0.94257319760146274000, +0.33399965144201471000 },
	{ -0.94359345816197626000, +0.33110630575988165000 },
	{ -0.94460483726149624000, +0.32820984357909777000 },
	{ -0.94560732538053738000, +0.32531029216226809000 },
	{ -0.94660091308329963000, +0.32240767880107496000 },
	{ -0.94758559101775719000, +0.31950203081602074000 },
	{ -0.94856134991574648000, +0.31659337555617090000 },
	{ -0.94952818059305288000, +0.31368174039889646000 },
	{ -0.95048607394949791000, +0.31076715274961647000 },
	{ -0.95143502096902455000, +0.30784964004153981000 },
	{ -0.95237501271978198000, +0.30492922973540731000 },
	{ -0.95330604035420996000, +0.30200594931923291000 },
	{ -0.95422809510912188000, +0.29907982630804525000 },
	{ -0.95514116830578699000, +0.29615088824362856000 },
	{ -0.95604525135001273000, +0.29321916269426335000 },
	{ -0.95694033573222514000, +0.29028467725446705000 },
	{ -0.95782641302754923000, +0.28734745954473417000 },
	{ -0.95870347489588792000, +0.28440753721127643000 },
	{ -0.95957151308200095000, +0.28146493792576255000 },
	{ -0.96043051941558222000, +0.27851968938505761000 },
	{ -0.96128048581133718000, +0.27557181931096264000 },
	{ -0.96212140426905812000, +0.27262135544995342000 },
	{ -0.96295326687370053000, +0.26966832557291953000 },
	{ -0.96377606579545660000, +0.26671275747490275000 },
	{ -0.96458979328982952000, +0.26375467897483573000 },
	{ -0.96539444169770616000, +0.26079411791527979000 },
	{ -0.96619000344542938000, +0.25783110216216321000 },
	{ -0.96697647104486895000, +0.25486565960451874000 },
	{ -0.96775383709349239000, +0.25189781815422108000 },
	{ -0.96852209427443425000, +0.24892760574572426000 },
	{ -0.96928123535656541000, +0.24595505033579865000 },
	{ -0.97003125319456085000, +0.24298017990326787000 },
	{ -0.97077214072896723000, +0.24000302244874541000 },
	{ -0.97150389098626877000, +0.23702360599437108000 },
	{ -0.97222649707895337000, +0.23404195858354726000 },
	{ -0.97293995220557727000, +0.23105810828067491000 },
	{ -0.97364424965082907000, +0.22807208317088948000 },
	{ -0.97433938278559307000, +0.22508391135979655000 },
	{ -0.97502534506701133000, +0.22209362097320720000 },
	{ -0.97570213003854578000, +0.21910124015687343000 },
	{ -0.97636973133003846000, +0.21610679707622310000 },
	{ -0.97702814265777171000, +0.21311031991609491000 },
	{ -0.97767735782452736000, +0.21011183688047311000 },
	{ -0.97831737071964497000, +0.20711137619222197000 },
	{ -0.97894817531907952000, +0.20410896609282023000 },
	{ -0.97956976568545795000, +0.20110463484209520000 },
	{ -0.98018213596813486000, +0.19809841071795684000 },
	{ -0.98078528040324797000, +0.19509032201613147000 },
	{ -0.98137919331377210000, +0.19208039704989557000 },
	{ -0.98196386910957290000, +0.18906866414980930000 },
	{ -0.98253930228745900000, +0.18605515166344969000 },
	{ -0.98310548743123405000, +0.18303988795514395000 },
	{ -0.98366241921174802000, +0.18002290140570246000 },
	{ -0.98421009238694690000, +0.17700422041215166000 },
	{ -0.98474850180192208000, +0.17398387338746668000 },
	{ -0.98527764238895910000, +0.17096188876030402000 },
	{ -0.98579750916758535000, +0.16793829497473392000 },
	{ -0.98630809724461665000, +0.16491312048997261000 },
	{ -0.98680940181420351000, +0.16188639378011446000 },
	{ -0.98730141815787642000, +0.15885814333386403000 },
	{ -0.98778414164459027000, +0.15582839765426776000 },
	{ -0.98825756773076767000, +0.15279718525844591000 },
	{ -0.98872169196034199000, +0.14976453467732395000 },
	{ -0.98917650996479922000, +0.14673047445536413000 },
	{ -0.98962201746321909000, +0.14369503315029677000 },
	{ -0.99005821026231533000, +0.14065823933285149000 },
	{ -0.99048508425647530000, +0.13762012158648826000 },
	{ -0.99090263542779833000, +0.13458070850712836000 },
	{ -0.99131085984613376000, +0.13154002870288523000 },
	{ -0.99170975366911784000, +0.12849811079379522000 },
	{ -0.99209931314221000000, +0.12545498341154823000 },
	{ -0.99247953459872829000, +0.12241067519921814000 },
	{ -0.99285041445988342000, +0.11936521481099326000 },
	{ -0.99321194923481293000, +0.11631863091190660000 },
	{ -0.99356413552061373000, +0.11327095217756612000 },
	{ -0.99390697000237449000, +0.11022220729388478000 },
	{ -0.99424044945320644000, +0.10717242495681052000 },
	{ -0.99456457073427396000, +0.10412163387205620000 },
	{ -0.99487933079482416000, +0.10106986275482939000 },
	{ -0.99518472667221547000, +0.09801714032956211600 },
	{ -0.99548075549194559000, +0.09496349532964046300 },
	{ -0.99576741446767847000, +0.09190895649713413900 },
	{ -0.99604470090127073000, +0.08885355258252594600 },
	{ -0.99631261218279676000, +0.08579731234444118400 },
	{ -0.99657114579057360000, +0.08274026454937694100 },
	{ -0.99682029929118454000, +0.07968243797143130500 },
	{ -0.99706007033950184000, +0.07662386139203261600 },
	{ -0.99729045667870908000, +0.07356456359966849500 },
	{ -0.99751145614032233000, +0.07050457338961488300 },
	{ -0.99772306664421051000, +0.06744391956366502200 },
	{ -0.99792528619861498000, +0.06438263092985836700 },
	{ -0.99811811290016828000, +0.06132073630220943200 },
	{ -0.99830154493391199000, +0.05825826450043655000 },
	{ -0.99847558057331387000, +0.05519524434969067000 },
	{ -0.99864021818028437000, +0.05213170468028399000 },
	{ -0.99879545620519150000, +0.04906767432741862600 },
	{ -0.99894129318687597000, +0.04600318213091517800 },
	{ -0.99907772775266457000, +0.04293825693494131300 },
	{ -0.99920475861838309000, +0.03987292758774024100 },
	{ -0.99932238458836875000, +0.03680722294135919900 },
	{ -0.99943060455548105000, +0.03374117185137789200 },
	{ -0.99952941750111246000, +0.03067480317663687200 },
	{ -0.99961882249519785000, +0.02760814577896592700 },
	{ -0.99969881869622346000, +0.02454122852291241300 },
	{ -0.99976940535123449000, +0.02147408027546957400 },
	{ -0.99983058179584261000, +0.01840672990580483100 },
	{ -0.99988234745423177000, +0.01533920628498805200 },
	{ -0.99992470183916382000, +0.01227153828571981800 },
	{ -0.99995764455198322000, +0.00920375478205965120 },
	{ -0.99998117528262043000, +0.00613588464915424890 },
	{ -0.99999529380959551000, +0.00306795676296569120 },
	{ -1.00000000000001930000, -0.00000000000000034434 },
};
#endif /* FFTSizeFilters == 2048 */
#if FFTSizeFilters == 4096
/* FFT/IFFT w coefficients tables for 4096 elements */
static const FLASHMEM struct Complex wm [2048] = 
{
	{ +0.99999882345170188000, -0.00153398018628476550 },
	{ +0.99999529380957608000, -0.00306795676296597610 },
	{ +0.99998941108192818000, -0.00460192612044857050 },
	{ +0.99998117528260089000, -0.00613588464915447440 },
	{ +0.99997058643097381000, -0.00766982873953109610 },
	{ +0.99995764455196356000, -0.00920375478205981770 },
	{ +0.99994234967602358000, -0.01073765916726449100 },
	{ +0.99992470183914417000, -0.01227153828571992400 },
	{ +0.99990470108285245000, -0.01380538852806038700 },
	{ +0.99988234745421212000, -0.01533920628498809700 },
	{ +0.99985764100582342000, -0.01687298794728171000 },
	{ +0.99983058179582296000, -0.01840672990580481700 },
	{ +0.99980116988788370000, -0.01994042855151443400 },
	{ +0.99976940535121472000, -0.02147408027546950100 },
	{ +0.99973528826056113000, -0.02300768146883936500 },
	{ +0.99969881869620358000, -0.02454122852291228100 },
	{ +0.99965999674395856000, -0.02607471782910389000 },
	{ +0.99961882249517786000, -0.02760814577896572900 },
	{ +0.99957529604674844000, -0.02914150876419371200 },
	{ +0.99952941750109237000, -0.03067480317663661200 },
	{ +0.99948118696616606000, -0.03220802540830456500 },
	{ +0.99943060455546084000, -0.03374117185137756600 },
	{ +0.99937767038800196000, -0.03527423889821392600 },
	{ +0.99932238458834854000, -0.03680722294135880400 },
	{ +0.99926474728659354000, -0.03834012037355266600 },
	{ +0.99920475861836300000, -0.03987292758773978300 },
	{ +0.99914241872481602000, -0.04140564097707670500 },
	{ +0.99907772775264447000, -0.04293825693494078600 },
	{ +0.99901068585407238000, -0.04447077185493862600 },
	{ +0.99894129318685587000, -0.04600318213091458800 },
	{ +0.99886954991428256000, -0.04753548415695926800 },
	{ +0.99879545620517129000, -0.04906767432741797300 },
	{ +0.99871901223387172000, -0.05059974903689923300 },
	{ +0.99864021818026394000, -0.05213170468028326800 },
	{ +0.99855907422975809000, -0.05366353765273047100 },
	{ +0.99847558057329355000, -0.05519524434968988600 },
	{ +0.99838973740733894000, -0.05672682116690768600 },
	{ +0.99830154493389156000, -0.05825826450043569000 },
	{ +0.99821100336047686000, -0.05978957074663980600 },
	{ +0.99811811290014785000, -0.06132073630220850200 },
	{ +0.99802287377148480000, -0.06285175756416133700 },
	{ +0.99792528619859455000, -0.06438263092985738200 },
	{ +0.99782535041111009000, -0.06591335279700372100 },
	{ +0.99772306664418997000, -0.06744391956366395400 },
	{ +0.99761843513851789000, -0.06897432762826663500 },
	{ +0.99751145614030179000, -0.07050457338961375900 },
	{ +0.99740212990127353000, -0.07203465324688920800 },
	{ +0.99729045667868843000, -0.07356456359966730100 },
	{ +0.99717643673532430000, -0.07509430084792118000 },
	{ +0.99706007033948107000, -0.07662386139203133900 },
	{ +0.99694135776498016000, -0.07815324163279409300 },
	{ +0.99682029929116367000, -0.07968243797142997300 },
	{ +0.99669689520289395000, -0.08121144680959228900 },
	{ +0.99657114579055273000, -0.08274026454937553900 },
	{ +0.99644305135004052000, -0.08426888759332391800 },
	{ +0.99631261218277589000, -0.08579731234443972700 },
	{ +0.99617982859569476000, -0.08732553520619190700 },
	{ +0.99604470090124975000, -0.08885355258252443400 },
	{ +0.99590722941740939000, -0.09038136087786480300 },
	{ +0.99576741446765737000, -0.09190895649713254300 },
	{ +0.99562525638099186000, -0.09343633584574759200 },
	{ +0.99548075549192450000, -0.09496349532963879700 },
	{ +0.99533391214047984000, -0.09649043135525238500 },
	{ +0.99518472667219449000, -0.09801714032956038200 },
	{ +0.99503319943811608000, -0.09954361866006911100 },
	{ +0.99487933079480306000, -0.10106986275482760000 },
	{ +0.99472312110432315000, -0.10259586902243606000 },
	{ +0.99456457073425286000, -0.10412163387205434000 },
	{ +0.99440368005767643000, -0.10564715371341037000 },
	{ +0.99424044945318535000, -0.10717242495680859000 },
	{ +0.99407487930487670000, -0.10869744401313845000 },
	{ +0.99390697000235328000, -0.11022220729388280000 },
	{ +0.99373672194072182000, -0.11174671121112632000 },
	{ +0.99356413552059253000, -0.11327095217756407000 },
	{ +0.99338921114807777000, -0.11479492660650979000 },
	{ +0.99321194923479172000, -0.11631863091190447000 },
	{ +0.99303235019784852000, -0.11784206150832467000 },
	{ +0.99285041445986222000, -0.11936521481099105000 },
	{ +0.99266614244894513000, -0.12088808723577675000 },
	{ +0.99247953459870708000, -0.12241067519921586000 },
	{ +0.99229059134825437000, -0.12393297511851183000 },
	{ +0.99209931314218869000, -0.12545498341154587000 },
	{ +0.99190570043060622000, -0.12697669649688548000 },
	{ +0.99170975366909631000, -0.12849811079379278000 },
	{ +0.99151147331874079000, -0.13001922272223293000 },
	{ +0.99131085984611222000, -0.13154002870288267000 },
	{ +0.99110791372327367000, -0.13306052515713862000 },
	{ +0.99090263542777679000, -0.13458070850712572000 },
	{ +0.99069502544266130000, -0.13610057517570573000 },
	{ +0.99048508425645365000, -0.13762012158648557000 },
	{ +0.99027281236316567000, -0.13913934416382573000 },
	{ +0.99005821026229357000, -0.14065823933284874000 },
	{ +0.98984127845881698000, -0.14217680351944756000 },
	{ +0.98962201746319722000, -0.14369503315029394000 },
	{ +0.98940042779137671000, -0.14521292465284694000 },
	{ +0.98917650996477735000, -0.14673047445536122000 },
	{ +0.98895026451029933000, -0.14824767898689548000 },
	{ +0.98872169196032011000, -0.14976453467732093000 },
	{ +0.98849079285269292000, -0.15128103795732961000 },
	{ +0.98825756773074580000, -0.15279718525844282000 },
	{ +0.98802201714327986000, -0.15431297301301949000 },
	{ +0.98778414164456840000, -0.15582839765426459000 },
	{ +0.98754394179435556000, -0.15734345561623761000 },
	{ +0.98730141815785466000, -0.15885814333386078000 },
	{ +0.98705657130574720000, -0.16037245724292759000 },
	{ +0.98680940181418164000, -0.16188639378011116000 },
	{ +0.98655991026477163000, -0.16339994938297253000 },
	{ +0.98630809724459489000, -0.16491312048996923000 },
	{ +0.98605396334619166000, -0.16642590354046341000 },
	{ +0.98579750916756359000, -0.16793829497473045000 },
	{ +0.98553873531217229000, -0.16945029123396721000 },
	{ +0.98527764238893745000, -0.17096188876030047000 },
	{ +0.98501423101223595000, -0.17247308399679520000 },
	{ +0.98474850180190032000, -0.17398387338746304000 },
	{ +0.98448045538321705000, -0.17549425337727062000 },
	{ +0.98421009238692514000, -0.17700422041214792000 },
	{ +0.98393741344921493000, -0.17851377093899667000 },
	{ +0.98366241921172626000, -0.18002290140569865000 },
	{ +0.98338511032154718000, -0.18153160826112411000 },
	{ +0.98310548743121229000, -0.18303988795514006000 },
	{ +0.98282355119870124000, -0.18454773693861870000 },
	{ +0.98253930228743724000, -0.18605515166344572000 },
	{ +0.98225274136628526000, -0.18756212858252866000 },
	{ +0.98196386910955114000, -0.18906866414980528000 },
	{ +0.98167268619697901000, -0.19057475482025182000 },
	{ +0.98137919331375034000, -0.19208039704989149000 },
	{ +0.98108339115048238000, -0.19358558729580266000 },
	{ +0.98078528040322610000, -0.19509032201612730000 },
	{ +0.98048486177346506000, -0.19659459767007925000 },
	{ +0.98018213596811310000, -0.19809841071795259000 },
	{ +0.97987710369951331000, -0.19960175762112997000 },
	{ +0.97956976568543619000, -0.20110463484209090000 },
	{ +0.97926012264907769000, -0.20260703884442011000 },
	{ +0.97894817531905776000, -0.20410896609281584000 },
	{ +0.97863392442941866000, -0.20561041305309821000 },
	{ +0.97831737071962310000, -0.20711137619221748000 },
	{ +0.97799851493455259000, -0.20861185197826243000 },
	{ +0.97767735782450549000, -0.21011183688046856000 },
	{ +0.97735390014519552000, -0.21161132736922650000 },
	{ +0.97702814265774984000, -0.21311031991609028000 },
	{ +0.97670008612870729000, -0.21460881099378568000 },
	{ +0.97636973133001659000, -0.21610679707621841000 },
	{ +0.97603707903903447000, -0.21760427463848250000 },
	{ +0.97570213003852391000, -0.21910124015686866000 },
	{ +0.97536488511665231000, -0.22059769010887240000 },
	{ +0.97502534506698957000, -0.22209362097320240000 },
	{ +0.97468351068850612000, -0.22358902922978885000 },
	{ +0.97433938278557120000, -0.22508391135979167000 },
	{ +0.97399296216795117000, -0.22657826384560883000 },
	{ +0.97364424965080720000, -0.22807208317088457000 },
	{ +0.97329324605469347000, -0.22956536582051770000 },
	{ +0.97293995220555540000, -0.23105810828066992000 },
	{ +0.97258436893472755000, -0.23255030703877402000 },
	{ +0.97222649707893150000, -0.23404195858354221000 },
	{ +0.97186633748027451000, -0.23553305940497429000 },
	{ +0.97150389098624690000, -0.23702360599436600000 },
	{ +0.97113915844972021000, -0.23851359484431719000 },
	{ +0.97077214072894535000, -0.24000302244874025000 },
	{ +0.97040283868755051000, -0.24149188530286811000 },
	{ +0.97003125319453898000, -0.24298017990326262000 },
	{ +0.96965738512428745000, -0.24446790274782287000 },
	{ +0.96928123535654342000, -0.24595505033579329000 },
	{ +0.96890280477642388000, -0.24744161916777194000 },
	{ +0.96852209427441227000, -0.24892760574571879000 },
	{ +0.96813910474635734000, -0.25041300657296389000 },
	{ +0.96775383709347040000, -0.25189781815421558000 },
	{ +0.96736629222232351000, -0.25338203699556877000 },
	{ +0.96697647104484696000, -0.25486565960451318000 },
	{ +0.96658437447832790000, -0.25634868248994153000 },
	{ +0.96619000344540729000, -0.25783110216215760000 },
	{ +0.96579335887407836000, -0.25931291513288479000 },
	{ +0.96539444169768407000, -0.26079411791527407000 },
	{ +0.96499325285491500000, -0.26227470702391220000 },
	{ +0.96458979328980732000, -0.26375467897482996000 },
	{ +0.96418406395174039000, -0.26523403028551040000 },
	{ +0.96377606579543440000, -0.26671275747489698000 },
	{ +0.96336579978094861000, -0.26819085706340173000 },
	{ +0.96295326687367844000, -0.26966832557291359000 },
	{ +0.96253846804435372000, -0.27114515952680646000 },
	{ +0.96212140426903614000, -0.27262135544994737000 },
	{ +0.96170207652911699000, -0.27409690986870472000 },
	{ +0.96128048581131509000, -0.27557181931095648000 },
	{ +0.96085663310767400000, -0.27704608030609823000 },
	{ +0.96043051941556012000, -0.27851968938505139000 },
	{ +0.96000214573766018000, -0.27999264308027150000 },
	{ +0.95957151308197874000, -0.28146493792575622000 },
	{ +0.95913862246183612000, -0.28293657045705356000 },
	{ +0.95870347489586571000, -0.28440753721127005000 },
	{ +0.95826607140801179000, -0.28587783472707878000 },
	{ +0.95782641302752702000, -0.28734745954472768000 },
	{ +0.95738450078897008000, -0.28881640820604759000 },
	{ +0.95694033573220294000, -0.29028467725446050000 },
	{ +0.95649391890238911000, -0.29175226323498737000 },
	{ +0.95604525134999041000, -0.29321916269425674000 },
	{ +0.95559433413076500000, -0.29468537218051238000 },
	{ +0.95514116830576457000, -0.29615088824362185000 },
	{ +0.95468575494133212000, -0.29761570743508425000 },
	{ +0.95422809510909934000, -0.29907982630803848000 },
	{ +0.95376818988598400000, -0.30054324141727146000 },
	{ +0.95330604035418753000, -0.30200594931922603000 },
	{ +0.95284164760119228000, -0.30346794657200926000 },
	{ +0.95237501271975944000, -0.30492922973540032000 },
	{ +0.95190613680792591000, -0.30638979537085881000 },
	{ +0.95143502096900190000, -0.30784964004153270000 },
	{ +0.95096166631156853000, -0.30930876031226651000 },
	{ +0.95048607394947504000, -0.31076715274960925000 },
	{ +0.95000824500183634000, -0.31222481392182266000 },
	{ +0.94952818059302990000, -0.31368174039888919000 },
	{ +0.94904588185269378000, -0.31513792875252011000 },
	{ +0.94856134991572350000, -0.31659337555616351000 },
	{ +0.94807458592226934000, -0.31804807738501256000 },
	{ +0.94758559101773421000, -0.31950203081601325000 },
	{ +0.94709436635277022000, -0.32095523242787283000 },
	{ +0.94660091308327654000, -0.32240767880106741000 },
	{ +0.94610523237039634000, -0.32385936651785041000 },
	{ +0.94560732538051417000, -0.32531029216226043000 },
	{ +0.94510719328525339000, -0.32676045232012929000 },
	{ +0.94460483726147304000, -0.32820984357909000000 },
	{ +0.94410025849126544000, -0.32965846252858494000 },
	{ +0.94359345816195317000, -0.33110630575987382000 },
	{ +0.94308443746608628000, -0.33255336986604161000 },
	{ +0.94257319760143965000, -0.33399965144200672000 },
	{ +0.94205973977101010000, -0.33544514708452888000 },
	{ +0.94154406518301348000, -0.33688985339221728000 },
	{ +0.94102617505088193000, -0.33833376696553835000 },
	{ +0.94050607059326097000, -0.33977688440682402000 },
	{ +0.93998375303400661000, -0.34121920232027952000 },
	{ +0.93945922360218248000, -0.34266071731199149000 },
	{ +0.93893248353205716000, -0.34410142598993593000 },
	{ +0.93840353406310073000, -0.34554132496398610000 },
	{ +0.93787237643998245000, -0.34698041084592068000 },
	{ +0.93733901191256752000, -0.34841868024943157000 },
	{ +0.93680344173591423000, -0.34985612979013192000 },
	{ +0.93626566717027093000, -0.35129275608556410000 },
	{ +0.93572568948107293000, -0.35272855575520767000 },
	{ +0.93518350993894017000, -0.35416352542048735000 },
	{ +0.93463912981967334000, -0.35559766170478080000 },
	{ +0.93409255040425143000, -0.35703096123342687000 },
	{ +0.93354377297882873000, -0.35846342063373338000 },
	{ +0.93299279883473141000, -0.35989503653498495000 },
	{ +0.93243962926845492000, -0.36132580556845106000 },
	{ +0.93188426558166060000, -0.36275572436739395000 },
	{ +0.93132670908117288000, -0.36418478956707662000 },
	{ +0.93076696107897605000, -0.36561299780477058000 },
	{ +0.93020502289221141000, -0.36704034571976391000 },
	{ +0.92964089584317355000, -0.36846682995336905000 },
	{ +0.92907458125930809000, -0.36989244714893083000 },
	{ +0.92850608047320782000, -0.37131719395183421000 },
	{ +0.92793539482261012000, -0.37274106700951243000 },
	{ +0.92736252565039334000, -0.37416406297145460000 },
	{ +0.92678747430457409000, -0.37558617848921383000 },
	{ +0.92621024213830361000, -0.37700741021641487000 },
	{ +0.92563083050986505000, -0.37842775480876223000 },
	{ +0.92504924078266981000, -0.37984720892404777000 },
	{ +0.92446547432525483000, -0.38126576922215899000 },
	{ +0.92387953251127897000, -0.38268343236508640000 },
	{ +0.92329141671951975000, -0.38410019501693166000 },
	{ +0.92270112833387063000, -0.38551605384391546000 },
	{ +0.92210866874333719000, -0.38693100551438514000 },
	{ +0.92151403934203402000, -0.38834504669882286000 },
	{ +0.92091724152918153000, -0.38975817406985303000 },
	{ +0.92031827670910260000, -0.39117038430225048000 },
	{ +0.91971714629121937000, -0.39258167407294808000 },
	{ +0.91911385169004978000, -0.39399204006104471000 },
	{ +0.91850839432520426000, -0.39540147894781291000 },
	{ +0.91790077562138250000, -0.39680998741670687000 },
	{ +0.91729099700836991000, -0.39821756215337012000 },
	{ +0.91667905992103460000, -0.39962419984564329000 },
	{ +0.91606496579932362000, -0.40102989718357213000 },
	{ +0.91544871608825973000, -0.40243465085941493000 },
	{ +0.91483031223793798000, -0.40383845756765058000 },
	{ +0.91420975570352248000, -0.40524131400498631000 },
	{ +0.91358704794524259000, -0.40664321687036542000 },
	{ +0.91296219042838989000, -0.40804416286497508000 },
	{ +0.91233518462331442000, -0.40944414869225398000 },
	{ +0.91170603200542155000, -0.41084317105790030000 },
	{ +0.91107473405516803000, -0.41224122666987922000 },
	{ +0.91044129225805892000, -0.41363831223843084000 },
	{ +0.90980570810464390000, -0.41503442447607786000 },
	{ +0.90916798309051405000, -0.41642956009763338000 },
	{ +0.90852811871629779000, -0.41782371582020850000 },
	{ +0.90788611648765782000, -0.41921688836322007000 },
	{ +0.90724197791528738000, -0.42060907444839862000 },
	{ +0.90659570451490690000, -0.42200027079979574000 },
	{ +0.90594729780726002000, -0.42339047414379205000 },
	{ +0.90529675931811027000, -0.42477968120910481000 },
	{ +0.90464409057823769000, -0.42616788872679562000 },
	{ +0.90398929312343479000, -0.42755509343027809000 },
	{ +0.90333236849450327000, -0.42894129205532550000 },
	{ +0.90267331823725017000, -0.43032648134007856000 },
	{ +0.90201214390248441000, -0.43171065802505321000 },
	{ +0.90134884704601326000, -0.43309381885314790000 },
	{ +0.90068342922863809000, -0.43447596056965160000 },
	{ +0.90001589201615140000, -0.43585707992225137000 },
	{ +0.89934623697933269000, -0.43723717366103992000 },
	{ +0.89867446569394493000, -0.43861623853852344000 },
	{ +0.89800057974073089000, -0.43999427130962904000 },
	{ +0.89732458070540932000, -0.44137126873171245000 },
	{ +0.89664647017867116000, -0.44274722756456575000 },
	{ +0.89596624975617611000, -0.44412214457042487000 },
	{ +0.89528392103854848000, -0.44549601651397736000 },
	{ +0.89459948563137359000, -0.44686884016236977000 },
	{ +0.89391294514519415000, -0.44824061228521545000 },
	{ +0.89322430119550622000, -0.44961132965460210000 },
	{ +0.89253355540275547000, -0.45098098904509931000 },
	{ +0.89184070939233351000, -0.45234958723376628000 },
	{ +0.89114576479457397000, -0.45371712100015926000 },
	{ +0.89044872324474855000, -0.45508358712633917000 },
	{ +0.88974958638306345000, -0.45644898239687925000 },
	{ +0.88904835585465514000, -0.45781330359887251000 },
	{ +0.88834503330958681000, -0.45917654752193943000 },
	{ +0.88763962040284439000, -0.46053871095823529000 },
	{ +0.88693211879433254000, -0.46189979070245801000 },
	{ +0.88622253014887087000, -0.46325978355185549000 },
	{ +0.88551085613619018000, -0.46461868630623310000 },
	{ +0.88479709843092802000, -0.46597649576796141000 },
	{ +0.88408125871262522000, -0.46733320874198364000 },
	{ +0.88336333866572181000, -0.46868882203582313000 },
	{ +0.88264333997955302000, -0.47004333245959079000 },
	{ +0.88192126434834517000, -0.47139673682599276000 },
	{ +0.88119711347121210000, -0.47274903195033791000 },
	{ +0.88047088905215076000, -0.47410021465054514000 },
	{ +0.87974259280003742000, -0.47545028174715098000 },
	{ +0.87901222642862342000, -0.47679923006331720000 },
	{ +0.87827979165653147000, -0.47814705642483812000 },
	{ +0.87754529020725125000, -0.47949375766014807000 },
	{ +0.87680872380913566000, -0.48083933060032896000 },
	{ +0.87607009419539661000, -0.48218377207911772000 },
	{ +0.87532940310410090000, -0.48352707893291369000 },
	{ +0.87458665227816612000, -0.48486924800078607000 },
	{ +0.87384184346535676000, -0.48621027612448131000 },
	{ +0.87309497841827999000, -0.48755016014843083000 },
	{ +0.87234605889438133000, -0.48888889691975801000 },
	{ +0.87159508665594088000, -0.49022648328828600000 },
	{ +0.87084206347006876000, -0.49156291610654479000 },
	{ +0.87008699110870125000, -0.49289819222977882000 },
	{ +0.86932987134859663000, -0.49423230851595451000 },
	{ +0.86857070597133079000, -0.49556526182576732000 },
	{ +0.86780949676329322000, -0.49689704902264931000 },
	{ +0.86704624551568266000, -0.49822766697277659000 },
	{ +0.86628095402450289000, -0.49955711254507662000 },
	{ +0.86551362409055899000, -0.50088538261123550000 },
	{ +0.86474425751945228000, -0.50221247404570546000 },
	{ +0.86397285612157659000, -0.50353838372571225000 },
	{ +0.86319942171211395000, -0.50486310853126226000 },
	{ +0.86242395611103029000, -0.50618664534515001000 },
	{ +0.86164646114307109000, -0.50750899105296554000 },
	{ +0.86086693863775698000, -0.50883014254310166000 },
	{ +0.86008539042937993000, -0.51015009670676137000 },
	{ +0.85930181835699815000, -0.51146885043796497000 },
	{ +0.85851622426443253000, -0.51278640063355763000 },
	{ +0.85772861000026179000, -0.51410274419321633000 },
	{ +0.85693897741781844000, -0.51541787801945760000 },
	{ +0.85614732837518415000, -0.51673179901764443000 },
	{ +0.85535366473518570000, -0.51804450409599390000 },
	{ +0.85455798836539021000, -0.51935599016558409000 },
	{ +0.85376030113810109000, -0.52066625414036161000 },
	{ +0.85296060493035331000, -0.52197529293714873000 },
	{ +0.85215890162390950000, -0.52328310347565077000 },
	{ +0.85135519310525476000, -0.52458968267846318000 },
	{ +0.85054948126559304000, -0.52589502747107897000 },
	{ +0.84974176800084211000, -0.52719913478189562000 },
	{ +0.84893205521162918000, -0.52850200154222271000 },
	{ +0.84812034480328669000, -0.52980362468628894000 },
	{ +0.84730663868584777000, -0.53110400115124923000 },
	{ +0.84649093877404147000, -0.53240312787719213000 },
	{ +0.84567324698728841000, -0.53370100180714708000 },
	{ +0.84485356524969646000, -0.53499761988709127000 },
	{ +0.84403189549005575000, -0.53629297906595719000 },
	{ +0.84320823964183467000, -0.53758707629563951000 },
	{ +0.84238259964317508000, -0.53887990853100243000 },
	{ +0.84155497743688756000, -0.54017147272988686000 },
	{ +0.84072537497044719000, -0.54146176585311745000 },
	{ +0.83989379419598864000, -0.54275078486450978000 },
	{ +0.83906023707030186000, -0.54403852673087771000 },
	{ +0.83822470555482720000, -0.54532498842204025000 },
	{ +0.83738720161565106000, -0.54661016691082864000 },
	{ +0.83654772722350113000, -0.54789405917309386000 },
	{ +0.83570628435374172000, -0.54917666218771344000 },
	{ +0.83486287498636913000, -0.55045797293659848000 },
	{ +0.83401750110600725000, -0.55173798840470112000 },
	{ +0.83317016470190230000, -0.55301670558002114000 },
	{ +0.83232086776791869000, -0.55429412145361368000 },
	{ +0.83146961230253436000, -0.55557023301959574000 },
	{ +0.83061640030883532000, -0.55684503727515355000 },
	{ +0.82976123379451205000, -0.55811853122054955000 },
	{ +0.82890411477185388000, -0.55939071185912947000 },
	{ +0.82804504525774469000, -0.56066157619732937000 },
	{ +0.82718402727365803000, -0.56193112124468270000 },
	{ +0.82632106284565243000, -0.56319934401382732000 },
	{ +0.82545615400436645000, -0.56446624152051261000 },
	{ +0.82458930278501419000, -0.56573181078360635000 },
	{ +0.82372051122738033000, -0.56699604882510179000 },
	{ +0.82284978137581533000, -0.56825895267012461000 },
	{ +0.82197711527923045000, -0.56952051934694026000 },
	{ +0.82110251499109355000, -0.57078074588696037000 },
	{ +0.82022598256942347000, -0.57203962932475017000 },
	{ +0.81934752007678580000, -0.57329716669803521000 },
	{ +0.81846712958028756000, -0.57455335504770877000 },
	{ +0.81758481315157250000, -0.57580819141783823000 },
	{ +0.81670057286681663000, -0.57706167285567245000 },
	{ +0.81581441080672257000, -0.57831379641164837000 },
	{ +0.81492632905651530000, -0.57956455913939853000 },
	{ +0.81403632970593698000, -0.58081395809575731000 },
	{ +0.81314441484924216000, -0.58206199034076822000 },
	{ +0.81225058658519256000, -0.58330865293769096000 },
	{ +0.81135484701705241000, -0.58455394295300800000 },
	{ +0.81045719825258344000, -0.58579785745643154000 },
	{ +0.80955764240403993000, -0.58704039352091064000 },
	{ +0.80865618158816366000, -0.58828154822263801000 },
	{ +0.80775281792617892000, -0.58952131864105661000 },
	{ +0.80684755354378790000, -0.59075970185886684000 },
	{ +0.80594039057116496000, -0.59199669496203355000 },
	{ +0.80503133114295222000, -0.59323229503979236000 },
	{ +0.80412037739825437000, -0.59446649918465699000 },
	{ +0.80320753148063351000, -0.59569930449242592000 },
	{ +0.80229279553810429000, -0.59693070806218895000 },
	{ +0.80137617172312881000, -0.59816070699633472000 },
	{ +0.80045766219261139000, -0.59938929840055688000 },
	{ +0.79953726910789358000, -0.60061647938386120000 },
	{ +0.79861499463474928000, -0.60184224705857226000 },
	{ +0.79769084094337961000, -0.60306659854034028000 },
	{ +0.79676481020840717000, -0.60428953094814808000 },
	{ +0.79583690460887191000, -0.60551104140431755000 },
	{ +0.79490712632822536000, -0.60673112703451648000 },
	{ +0.79397547755432551000, -0.60794978496776564000 },
	{ +0.79304196047943198000, -0.60916701233644510000 },
	{ +0.79210657730020062000, -0.61038280627630137000 },
	{ +0.79116933021767843000, -0.61159716392645380000 },
	{ +0.79023022143729826000, -0.61281008242940149000 },
	{ +0.78928925316887388000, -0.61402155893103028000 },
	{ +0.78834642762659446000, -0.61523159058061871000 },
	{ +0.78740174702901955000, -0.61644017453084543000 },
	{ +0.78645521359907389000, -0.61764730793779576000 },
	{ +0.78550682956404205000, -0.61885298796096810000 },
	{ +0.78455659715556336000, -0.62005721176328099000 },
	{ +0.78360451860962643000, -0.62125997651107934000 },
	{ +0.78265059616656396000, -0.62246127937414164000 },
	{ +0.78169483207104762000, -0.62366111752568620000 },
	{ +0.78073722857208272000, -0.62485948814237802000 },
	{ +0.77977778792300267000, -0.62605638840433520000 },
	{ +0.77881651238146410000, -0.62725181549513576000 },
	{ +0.77785340420944116000, -0.62844576660182427000 },
	{ +0.77688846567322056000, -0.62963823891491855000 },
	{ +0.77592169904339570000, -0.63082922962841603000 },
	{ +0.77495310659486194000, -0.63201873593980051000 },
	{ +0.77398269060681080000, -0.63320675505004875000 },
	{ +0.77301045336272489000, -0.63439328416363694000 },
	{ +0.77203639715037242000, -0.63557832048854757000 },
	{ +0.77106052426180172000, -0.63676186123627565000 },
	{ +0.77008283699333591000, -0.63794390362183540000 },
	{ +0.76910333764556760000, -0.63912444486376696000 },
	{ +0.76812202852335332000, -0.64030348218414290000 },
	{ +0.76713891193580830000, -0.64148101280857439000 },
	{ +0.76615399019630082000, -0.64265703396621809000 },
	{ +0.76516726562244686000, -0.64383154288978273000 },
	{ +0.76417874053610468000, -0.64500453681553516000 },
	{ +0.76318841726336917000, -0.64617601298330751000 },
	{ +0.76219629813456680000, -0.64734596863650318000 },
	{ +0.76120238548424968000, -0.64851440102210345000 },
	{ +0.76020668165119021000, -0.64968130739067420000 },
	{ +0.75920918897837586000, -0.65084668499637188000 },
	{ +0.75820990981300318000, -0.65201053109695051000 },
	{ +0.75720884650647247000, -0.65317284295376776000 },
	{ +0.75620600141438243000, -0.65433361783179145000 },
	{ +0.75520137689652445000, -0.65549285299960625000 },
	{ +0.75419497531687707000, -0.65665054572941983000 },
	{ +0.75318679904360042000, -0.65780669329706942000 },
	{ +0.75217685044903071000, -0.65896129298202810000 },
	{ +0.75116513190967438000, -0.66011434206741115000 },
	{ +0.75015164580620308000, -0.66126583783998283000 },
	{ +0.74913639452344727000, -0.66241577759016235000 },
	{ +0.74811938045039150000, -0.66356415861203033000 },
	{ +0.74710060598016803000, -0.66471097820333536000 },
	{ +0.74608007351005168000, -0.66585623366550017000 },
	{ +0.74505778544145385000, -0.66699992230362803000 },
	{ +0.74403374417991719000, -0.66814204142650901000 },
	{ +0.74300795213510951000, -0.66928258834662646000 },
	{ +0.74198041172081886000, -0.67042156038016354000 },
	{ +0.74095112535494689000, -0.67155895484700878000 },
	{ +0.73992009545950388000, -0.67269476907076331000 },
	{ +0.73888732446060290000, -0.67382900037874649000 },
	{ +0.73785281478845366000, -0.67496164610200238000 },
	{ +0.73681656887735747000, -0.67609270357530626000 },
	{ +0.73577858916570116000, -0.67722217013717068000 },
	{ +0.73473887809595106000, -0.67835004312985170000 },
	{ +0.73369743811464783000, -0.67947631989935520000 },
	{ +0.73265427167240038000, -0.68060099779544314000 },
	{ +0.73160938122388008000, -0.68172407417163983000 },
	{ +0.73056276922781505000, -0.68284554638523809000 },
	{ +0.72951443814698447000, -0.68396541179730541000 },
	{ +0.72846439044821265000, -0.68508366777269036000 },
	{ +0.72741262860236322000, -0.68620031168002849000 },
	{ +0.72635915508433335000, -0.68731534089174895000 },
	{ +0.72530397237304811000, -0.68842875278408022000 },
	{ +0.72424708295145424000, -0.68954054473705673000 },
	{ +0.72318848930651469000, -0.69065071413452439000 },
	{ +0.72212819392920258000, -0.69175925836414753000 },
	{ +0.72106619931449534000, -0.69286617481741430000 },
	{ +0.72000250796136889000, -0.69397146088964368000 },
	{ +0.71893712237279161000, -0.69507511397999044000 },
	{ +0.71787004505571894000, -0.69617713149145255000 },
	{ +0.71680127852108666000, -0.69727751083087619000 },
	{ +0.71573082528380583000, -0.69837624940896237000 },
	{ +0.71465868786275621000, -0.69947334464027333000 },
	{ +0.71358486878078076000, -0.70056879394323779000 },
	{ +0.71250937056467945000, -0.70166259474015780000 },
	{ +0.71143219574520355000, -0.70275474445721453000 },
	{ +0.71035334685704943000, -0.70384524052447417000 },
	{ +0.70927282643885270000, -0.70493408037589400000 },
	{ +0.70819063703318230000, -0.70602126144932875000 },
	{ +0.70710678118653458000, -0.70710678118653636000 },
	{ +0.70602126144932686000, -0.70819063703318408000 },
	{ +0.70493408037589189000, -0.70927282643885436000 },
	{ +0.70384524052447195000, -0.71035334685705098000 },
	{ +0.70275474445721231000, -0.71143219574520511000 },
	{ +0.70166259474015558000, -0.71250937056468089000 },
	{ +0.70056879394323535000, -0.71358486878078220000 },
	{ +0.69947334464027078000, -0.71465868786275755000 },
	{ +0.69837624940895981000, -0.71573082528380716000 },
	{ +0.69727751083087353000, -0.71680127852108788000 },
	{ +0.69617713149144989000, -0.71787004505572005000 },
	{ +0.69507511397998778000, -0.71893712237279273000 },
	{ +0.69397146088964090000, -0.72000250796136989000 },
	{ +0.69286617481741153000, -0.72106619931449623000 },
	{ +0.69175925836414465000, -0.72212819392920335000 },
	{ +0.69065071413452150000, -0.72318848930651536000 },
	{ +0.68954054473705384000, -0.72424708295145490000 },
	{ +0.68842875278407734000, -0.72530397237304867000 },
	{ +0.68731534089174595000, -0.72635915508433391000 },
	{ +0.68620031168002538000, -0.72741262860236366000 },
	{ +0.68508366777268725000, -0.72846439044821298000 },
	{ +0.68396541179730230000, -0.72951443814698480000 },
	{ +0.68284554638523498000, -0.73056276922781527000 },
	{ +0.68172407417163672000, -0.73160938122388020000 },
	{ +0.68060099779543992000, -0.73265427167240049000 },
	{ +0.67947631989935187000, -0.73369743811464794000 },
	{ +0.67835004312984826000, -0.73473887809595118000 },
	{ +0.67722217013716712000, -0.73577858916570116000 },
	{ +0.67609270357530260000, -0.73681656887735747000 },
	{ +0.67496164610199871000, -0.73785281478845355000 },
	{ +0.67382900037874283000, -0.73888732446060268000 },
	{ +0.67269476907075965000, -0.73992009545950366000 },
	{ +0.67155895484700512000, -0.74095112535494656000 },
	{ +0.67042156038015976000, -0.74198041172081841000 },
	{ +0.66928258834662280000, -0.74300795213510906000 },
	{ +0.66814204142650524000, -0.74403374417991652000 },
	{ +0.66699992230362426000, -0.74505778544145307000 },
	{ +0.66585623366549640000, -0.74608007351005090000 },
	{ +0.66471097820333158000, -0.74710060598016725000 },
	{ +0.66356415861202656000, -0.74811938045039061000 },
	{ +0.66241577759015846000, -0.74913639452344627000 },
	{ +0.66126583783997894000, -0.75015164580620186000 },
	{ +0.66011434206740716000, -0.75116513190967316000 },
	{ +0.65896129298202399000, -0.75217685044902938000 },
	{ +0.65780669329706531000, -0.75318679904359909000 },
	{ +0.65665054572941572000, -0.75419497531687574000 },
	{ +0.65549285299960214000, -0.75520137689652300000 },
	{ +0.65433361783178723000, -0.75620600141438088000 },
	{ +0.65317284295376354000, -0.75720884650647080000 },
	{ +0.65201053109694629000, -0.75820990981300151000 },
	{ +0.65084668499636766000, -0.75920918897837431000 },
	{ +0.64968130739066998000, -0.76020668165118865000 },
	{ +0.64851440102209934000, -0.76120238548424801000 },
	{ +0.64734596863649896000, -0.76219629813456502000 },
	{ +0.64617601298330329000, -0.76318841726336728000 },
	{ +0.64500453681553094000, -0.76417874053610269000 },
	{ +0.64383154288977840000, -0.76516726562244486000 },
	{ +0.64265703396621388000, -0.76615399019629882000 },
	{ +0.64148101280857017000, -0.76713891193580630000 },
	{ +0.64030348218413868000, -0.76812202852335121000 },
	{ +0.63912444486376274000, -0.76910333764556549000 },
	{ +0.63794390362183107000, -0.77008283699333369000 },
	{ +0.63676186123627121000, -0.77106052426179938000 },
	{ +0.63557832048854312000, -0.77203639715036998000 },
	{ +0.63439328416363250000, -0.77301045336272234000 },
	{ +0.63320675505004431000, -0.77398269060680813000 },
	{ +0.63201873593979607000, -0.77495310659485916000 },
	{ +0.63082922962841159000, -0.77592169904339281000 },
	{ +0.62963823891491411000, -0.77688846567321768000 },
	{ +0.62844576660181983000, -0.77785340420943827000 },
	{ +0.62725181549513132000, -0.77881651238146110000 },
	{ +0.62605638840433075000, -0.77977778792299957000 },
	{ +0.62485948814237358000, -0.78073722857207950000 },
	{ +0.62366111752568176000, -0.78169483207104440000 },
	{ +0.62246127937413720000, -0.78265059616656063000 },
	{ +0.62125997651107479000, -0.78360451860962310000 },
	{ +0.62005721176327633000, -0.78455659715556014000 },
	{ +0.61885298796096344000, -0.78550682956403883000 },
	{ +0.61764730793779110000, -0.78645521359907056000 },
	{ +0.61644017453084077000, -0.78740174702901611000 },
	{ +0.61523159058061405000, -0.78834642762659091000 },
	{ +0.61402155893102561000, -0.78928925316887022000 },
	{ +0.61281008242939694000, -0.79023022143729449000 },
	{ +0.61159716392644925000, -0.79116933021767466000 },
	{ +0.61038280627629682000, -0.79210657730019673000 },
	{ +0.60916701233644055000, -0.79304196047942810000 },
	{ +0.60794978496776109000, -0.79397547755432152000 },
	{ +0.60673112703451193000, -0.79490712632822125000 },
	{ +0.60551104140431300000, -0.79583690460886769000 },
	{ +0.60428953094814353000, -0.79676481020840295000 },
	{ +0.60306659854033573000, -0.79769084094337528000 },
	{ +0.60184224705856759000, -0.79861499463474495000 },
	{ +0.60061647938385643000, -0.79953726910788914000 },
	{ +0.59938929840055211000, -0.80045766219260683000 },
	{ +0.59816070699632995000, -0.80137617172312425000 },
	{ +0.59693070806218407000, -0.80229279553809973000 },
	{ +0.59569930449242092000, -0.80320753148062884000 },
	{ +0.59446649918465200000, -0.80412037739824971000 },
	{ +0.59323229503978736000, -0.80503133114294756000 },
	{ +0.59199669496202856000, -0.80594039057116018000 },
	{ +0.59075970185886184000, -0.80684755354378301000 },
	{ +0.58952131864105151000, -0.80775281792617404000 },
	{ +0.58828154822263290000, -0.80865618158815866000 },
	{ +0.58704039352090565000, -0.80955764240403483000 },
	{ +0.58579785745642654000, -0.81045719825257834000 },
	{ +0.58455394295300300000, -0.81135484701704719000 },
	{ +0.58330865293768597000, -0.81225058658518734000 },
	{ +0.58206199034076322000, -0.81314441484923694000 },
	{ +0.58081395809575220000, -0.81403632970593176000 },
	{ +0.57956455913939342000, -0.81492632905650997000 },
	{ +0.57831379641164327000, -0.81581441080671713000 },
	{ +0.57706167285566723000, -0.81670057286681108000 },
	{ +0.57580819141783302000, -0.81758481315156695000 },
	{ +0.57455335504770355000, -0.81846712958028189000 },
	{ +0.57329716669802999000, -0.81934752007678013000 },
	{ +0.57203962932474495000, -0.82022598256941781000 },
	{ +0.57078074588695515000, -0.82110251499108777000 },
	{ +0.56952051934693504000, -0.82197711527922457000 },
	{ +0.56825895267011939000, -0.82284978137580922000 },
	{ +0.56699604882509647000, -0.82372051122737422000 },
	{ +0.56573181078360102000, -0.82458930278500808000 },
	{ +0.56446624152050728000, -0.82545615400436034000 },
	{ +0.56319934401382199000, -0.82632106284564621000 },
	{ +0.56193112124467737000, -0.82718402727365181000 },
	{ +0.56066157619732404000, -0.82804504525773848000 },
	{ +0.55939071185912415000, -0.82890411477184756000 },
	{ +0.55811853122054422000, -0.82976123379450573000 },
	{ +0.55684503727514811000, -0.83061640030882888000 },
	{ +0.55557023301959019000, -0.83146961230252769000 },
	{ +0.55429412145360801000, -0.83232086776791203000 },
	{ +0.55301670558001537000, -0.83317016470189553000 },
	{ +0.55173798840469535000, -0.83401750110600037000 },
	{ +0.55045797293659271000, -0.83486287498636225000 },
	{ +0.54917666218770766000, -0.83570628435373473000 },
	{ +0.54789405917308820000, -0.83654772722349413000 },
	{ +0.54661016691082298000, -0.83738720161564395000 },
	{ +0.54532498842203447000, -0.83822470555481998000 },
	{ +0.54403852673087194000, -0.83906023707029465000 },
	{ +0.54275078486450401000, -0.83989379419598142000 },
	{ +0.54146176585311168000, -0.84072537497043998000 },
	{ +0.54017147272988109000, -0.84155497743688024000 },
	{ +0.53887990853099665000, -0.84238259964316764000 },
	{ +0.53758707629563374000, -0.84320823964182712000 },
	{ +0.53629297906595141000, -0.84403189549004809000 },
	{ +0.53499761988708550000, -0.84485356524968869000 },
	{ +0.53370100180714131000, -0.84567324698728064000 },
	{ +0.53240312787718624000, -0.84649093877403359000 },
	{ +0.53110400115124323000, -0.84730663868583989000 },
	{ +0.52980362468628295000, -0.84812034480327880000 },
	{ +0.52850200154221672000, -0.84893205521162107000 },
	{ +0.52719913478188962000, -0.84974176800083379000 },
	{ +0.52589502747107297000, -0.85054948126558472000 },
	{ +0.52458968267845729000, -0.85135519310524643000 },
	{ +0.52328310347564488000, -0.85215890162390107000 },
	{ +0.52197529293714284000, -0.85296060493034487000 },
	{ +0.52066625414035572000, -0.85376030113809254000 },
	{ +0.51935599016557821000, -0.85455798836538166000 },
	{ +0.51804450409598801000, -0.85535366473517715000 },
	{ +0.51673179901763855000, -0.85614732837517560000 },
	{ +0.51541787801945160000, -0.85693897741780978000 },
	{ +0.51410274419321034000, -0.85772861000025302000 },
	{ +0.51278640063355163000, -0.85851622426442364000 },
	{ +0.51146885043795909000, -0.85930181835698927000 },
	{ +0.51015009670675548000, -0.86008539042937093000 },
	{ +0.50883014254309578000, -0.86086693863774810000 },
	{ +0.50750899105295966000, -0.86164646114306209000 },
	{ +0.50618664534514413000, -0.86242395611102129000 },
	{ +0.50486310853125638000, -0.86319942171210484000 },
	{ +0.50353838372570647000, -0.86397285612156738000 },
	{ +0.50221247404569980000, -0.86474425751944295000 },
	{ +0.50088538261122983000, -0.86551362409054955000 },
	{ +0.49955711254507096000, -0.86628095402449334000 },
	{ +0.49822766697277093000, -0.86704624551567289000 },
	{ +0.49689704902264364000, -0.86780949676328345000 },
	{ +0.49556526182576166000, -0.86857070597132102000 },
	{ +0.49423230851594890000, -0.86932987134858686000 },
	{ +0.49289819222977327000, -0.87008699110869148000 },
	{ +0.49156291610653918000, -0.87084206347005888000 },
	{ +0.49022648328828039000, -0.87159508665593088000 },
	{ +0.48888889691975246000, -0.87234605889437133000 },
	{ +0.48755016014842528000, -0.87309497841826988000 },
	{ +0.48621027612447576000, -0.87384184346534655000 },
	{ +0.48486924800078046000, -0.87458665227815580000 },
	{ +0.48352707893290808000, -0.87532940310409046000 },
	{ +0.48218377207911212000, -0.87607009419538617000 },
	{ +0.48083933060032336000, -0.87680872380912522000 },
	{ +0.47949375766014246000, -0.87754529020724081000 },
	{ +0.47814705642483246000, -0.87827979165652093000 },
	{ +0.47679923006331154000, -0.87901222642861288000 },
	{ +0.47545028174714532000, -0.87974259280002676000 },
	{ +0.47410021465053948000, -0.88047088905214010000 },
	{ +0.47274903195033224000, -0.88119711347120133000 },
	{ +0.47139673682598715000, -0.88192126434833429000 },
	{ +0.47004333245958518000, -0.88264333997954203000 },
	{ +0.46868882203581752000, -0.88336333866571082000 },
	{ +0.46733320874197803000, -0.88408125871261423000 },
	{ +0.46597649576795580000, -0.88479709843091692000 },
	{ +0.46461868630622749000, -0.88551085613617908000 },
	{ +0.46325978355184988000, -0.88622253014885966000 },
	{ +0.46189979070245246000, -0.88693211879432121000 },
	{ +0.46053871095822979000, -0.88763962040283284000 },
	{ +0.45917654752193393000, -0.88834503330957515000 },
	{ +0.45781330359886702000, -0.88904835585464337000 },
	{ +0.45644898239687376000, -0.88974958638305157000 },
	{ +0.45508358712633368000, -0.89044872324473656000 },
	{ +0.45371712100015377000, -0.89114576479456187000 },
	{ +0.45234958723376079000, -0.89184070939232130000 },
	{ +0.45098098904509382000, -0.89253355540274315000 },
	{ +0.44961132965459655000, -0.89322430119549379000 },
	{ +0.44824061228520984000, -0.89391294514518160000 },
	{ +0.44686884016236417000, -0.89459948563136105000 },
	{ +0.44549601651397175000, -0.89528392103853582000 },
	{ +0.44412214457041926000, -0.89596624975616346000 },
	{ +0.44274722756456009000, -0.89664647017865839000 },
	{ +0.44137126873170673000, -0.89732458070539645000 },
	{ +0.43999427130962326000, -0.89800057974071801000 },
	{ +0.43861623853851767000, -0.89867446569393206000 },
	{ +0.43723717366103415000, -0.89934623697931970000 },
	{ +0.43585707992224559000, -0.90001589201613841000 },
	{ +0.43447596056964582000, -0.90068342922862499000 },
	{ +0.43309381885314213000, -0.90134884704600005000 },
	{ +0.43171065802504743000, -0.90201214390247120000 },
	{ +0.43032648134007284000, -0.90267331823723684000 },
	{ +0.42894129205531978000, -0.90333236849448972000 },
	{ +0.42755509343027237000, -0.90398929312342113000 },
	{ +0.42616788872678996000, -0.90464409057822404000 },
	{ +0.42477968120909920000, -0.90529675931809661000 },
	{ +0.42339047414378644000, -0.90594729780724625000 },
	{ +0.42200027079979013000, -0.90659570451489313000 },
	{ +0.42060907444839296000, -0.90724197791527361000 },
	{ +0.41921688836321441000, -0.90788611648764395000 },
	{ +0.41782371582020278000, -0.90852811871628381000 },
	{ +0.41642956009762766000, -0.90916798309050006000 },
	{ +0.41503442447607214000, -0.90980570810462991000 },
	{ +0.41363831223842507000, -0.91044129225804482000 },
	{ +0.41224122666987340000, -0.91107473405515393000 },
	{ +0.41084317105789442000, -0.91170603200540745000 },
	{ +0.40944414869224810000, -0.91233518462330032000 },
	{ +0.40804416286496920000, -0.91296219042837568000 },
	{ +0.40664321687035954000, -0.91358704794522827000 },
	{ +0.40524131400498042000, -0.91420975570350804000 },
	{ +0.40383845756764469000, -0.91483031223792355000 },
	{ +0.40243465085940905000, -0.91544871608824518000 },
	{ +0.40102989718356630000, -0.91606496579930896000 },
	{ +0.39962419984563752000, -0.91667905992101983000 },
	{ +0.39821756215336435000, -0.91729099700835504000 },
	{ +0.39680998741670109000, -0.91790077562136752000 },
	{ +0.39540147894780714000, -0.91850839432518927000 },
	{ +0.39399204006103894000, -0.91911385169003479000 },
	{ +0.39258167407294237000, -0.91971714629120438000 },
	{ +0.39117038430224482000, -0.92031827670908761000 },
	{ +0.38975817406984742000, -0.92091724152916643000 },
	{ +0.38834504669881731000, -0.92151403934201881000 },
	{ +0.38693100551437964000, -0.92210866874332187000 },
	{ +0.38551605384390997000, -0.92270112833385531000 },
	{ +0.38410019501692616000, -0.92329141671950443000 },
	{ +0.38268343236508090000, -0.92387953251126342000 },
	{ +0.38126576922215349000, -0.92446547432523918000 },
	{ +0.37984720892404233000, -0.92504924078265416000 },
	{ +0.37842775480875679000, -0.92563083050984929000 },
	{ +0.37700741021640949000, -0.92621024213828784000 },
	{ +0.37558617848920850000, -0.92678747430455832000 },
	{ +0.37416406297144933000, -0.92736252565037747000 },
	{ +0.37274106700950715000, -0.92793539482259424000 },
	{ +0.37131719395182894000, -0.92850608047319194000 },
	{ +0.36989244714892555000, -0.92907458125929210000 },
	{ +0.36846682995336377000, -0.92964089584315757000 },
	{ +0.36704034571975869000, -0.93020502289219531000 },
	{ +0.36561299780476542000, -0.93076696107895995000 },
	{ +0.36418478956707151000, -0.93132670908115667000 },
	{ +0.36275572436738884000, -0.93188426558164428000 },
	{ +0.36132580556844596000, -0.93243962926843849000 },
	{ +0.35989503653497984000, -0.93299279883471486000 },
	{ +0.35846342063372827000, -0.93354377297881219000 },
	{ +0.35703096123342176000, -0.93409255040423478000 },
	{ +0.35559766170477564000, -0.93463912981965658000 },
	{ +0.35416352542048213000, -0.93518350993892330000 },
	{ +0.35272855575520246000, -0.93572568948105606000 },
	{ +0.35129275608555893000, -0.93626566717025395000 },
	{ +0.34985612979012676000, -0.93680344173589725000 },
	{ +0.34841868024942640000, -0.93733901191255053000 },
	{ +0.34698041084591552000, -0.93787237643996546000 },
	{ +0.34554132496398093000, -0.93840353406308374000 },
	{ +0.34410142598993076000, -0.93893248353204006000 },
	{ +0.34266071731198633000, -0.93945922360216549000 },
	{ +0.34121920232027436000, -0.93998375303398951000 },
	{ +0.33977688440681886000, -0.94050607059324387000 },
	{ +0.33833376696553319000, -0.94102617505086472000 },
	{ +0.33688985339221211000, -0.94154406518299627000 },
	{ +0.33544514708452372000, -0.94205973977099278000 },
	{ +0.33399965144200150000, -0.94257319760142233000 },
	{ +0.33255336986603634000, -0.94308443746606885000 },
	{ +0.33110630575986849000, -0.94359345816193563000 },
	{ +0.32965846252857961000, -0.94410025849124790000 },
	{ +0.32820984357908467000, -0.94460483726145550000 },
	{ +0.32676045232012396000, -0.94510719328523574000 },
	{ +0.32531029216225515000, -0.94560732538049652000 },
	{ +0.32385936651784514000, -0.94610523237037847000 },
	{ +0.32240767880106214000, -0.94660091308325855000 },
	{ +0.32095523242786755000, -0.94709436635275213000 },
	{ +0.31950203081600803000, -0.94758559101771600000 },
	{ +0.31804807738500734000, -0.94807458592225102000 },
	{ +0.31659337555615830000, -0.94856134991570495000 },
	{ +0.31513792875251490000, -0.94904588185267513000 },
	{ +0.31368174039888397000, -0.94952818059301114000 },
	{ +0.31222481392181745000, -0.95000824500181746000 },
	{ +0.31076715274960404000, -0.95048607394945617000 },
	{ +0.30930876031226129000, -0.95096166631154944000 },
	{ +0.30784964004152748000, -0.95143502096898258000 },
	{ +0.30638979537085353000, -0.95190613680790648000 },
	{ +0.30492922973539499000, -0.95237501271974001000 },
	{ +0.30346794657200388000, -0.95284164760117274000 },
	{ +0.30200594931922065000, -0.95330604035416777000 },
	{ +0.30054324141726607000, -0.95376818988596423000 },
	{ +0.29907982630803309000, -0.95422809510907958000 },
	{ +0.29761570743507887000, -0.95468575494131236000 },
	{ +0.29615088824361652000, -0.95514116830574469000 },
	{ +0.29468537218050711000, -0.95559433413074502000 },
	{ +0.29321916269425147000, -0.95604525134997032000 },
	{ +0.29175226323498210000, -0.95649391890236890000 },
	{ +0.29028467725445523000, -0.95694033573218262000 },
	{ +0.28881640820604237000, -0.95738450078894954000 },
	{ +0.28734745954472246000, -0.95782641302750648000 },
	{ +0.28587783472707357000, -0.95826607140799125000 },
	{ +0.28440753721126483000, -0.95870347489584518000 },
	{ +0.28293657045704834000, -0.95913862246181558000 },
	{ +0.28146493792575100000, -0.95957151308195821000 },
	{ +0.27999264308026628000, -0.96000214573763953000 },
	{ +0.27851968938504618000, -0.96043051941553936000 },
	{ +0.27704608030609301000, -0.96085663310765312000 },
	{ +0.27557181931095132000, -0.96128048581129411000 },
	{ +0.27409690986869961000, -0.96170207652909590000 },
	{ +0.27262135544994226000, -0.96212140426901493000 },
	{ +0.27114515952680129000, -0.96253846804433241000 },
	{ +0.26966832557290837000, -0.96295326687365701000 },
	{ +0.26819085706339646000, -0.96336579978092707000 },
	{ +0.26671275747489165000, -0.96377606579541286000 },
	{ +0.26523403028550507000, -0.96418406395171874000 },
	{ +0.26375467897482469000, -0.96458979328978567000 },
	{ +0.26227470702390698000, -0.96499325285489324000 },
	{ +0.26079411791526891000, -0.96539444169766220000 },
	{ +0.25931291513287968000, -0.96579335887405648000 },
	{ +0.25783110216215249000, -0.96619000344538530000 },
	{ +0.25634868248993642000, -0.96658437447830581000 },
	{ +0.25486565960450813000, -0.96697647104482476000 },
	{ +0.25338203699556378000, -0.96736629222230108000 },
	{ +0.25189781815421064000, -0.96775383709344798000 },
	{ +0.25041300657295901000, -0.96813910474633491000 },
	{ +0.24892760574571393000, -0.96852209427438973000 },
	{ +0.24744161916776711000, -0.96890280477640123000 },
	{ +0.24595505033578846000, -0.96928123535652078000 },
	{ +0.24446790274781804000, -0.96965738512426469000 },
	{ +0.24298017990325779000, -0.97003125319451622000 },
	{ +0.24149188530286328000, -0.97040283868752764000 },
	{ +0.24000302244873545000, -0.97077214072892248000 },
	{ +0.23851359484431245000, -0.97113915844969734000 },
	{ +0.23702360599436126000, -0.97150389098622392000 },
	{ +0.23553305940496957000, -0.97186633748025153000 },
	{ +0.23404195858353755000, -0.97222649707890851000 },
	{ +0.23255030703876942000, -0.97258436893470435000 },
	{ +0.23105810828066534000, -0.97293995220553220000 },
	{ +0.22956536582051315000, -0.97329324605467027000 },
	{ +0.22807208317088004000, -0.97364424965078400000 },
	{ +0.22657826384560434000, -0.97399296216792786000 },
	{ +0.22508391135978720000, -0.97433938278554788000 },
	{ +0.22358902922978441000, -0.97468351068848269000 },
	{ +0.22209362097319799000, -0.97502534506696614000 },
	{ +0.22059769010886801000, -0.97536488511662889000 },
	{ +0.21910124015686433000, -0.97570213003850048000 },
	{ +0.21760427463847823000, -0.97603707903901105000 },
	{ +0.21610679707621414000, -0.97636973132999305000 },
	{ +0.21460881099378143000, -0.97670008612868364000 },
	{ +0.21311031991608606000, -0.97702814265772620000 },
	{ +0.21161132736922231000, -0.97735390014517187000 },
	{ +0.21011183688046439000, -0.97767735782448184000 },
	{ +0.20861185197825829000, -0.97799851493452894000 },
	{ +0.20711137619221340000, -0.97831737071959946000 },
	{ +0.20561041305309416000, -0.97863392442939490000 },
	{ +0.20410896609281179000, -0.97894817531903389000 },
	{ +0.20260703884441608000, -0.97926012264905371000 },
	{ +0.20110463484208690000, -0.97956976568541221000 },
	{ +0.19960175762112600000, -0.97987710369948922000 },
	{ +0.19809841071794865000, -0.98018213596808890000 },
	{ +0.19659459767007534000, -0.98048486177344074000 },
	{ +0.19509032201612342000, -0.98078528040320190000 },
	{ +0.19358558729579881000, -0.98108339115045806000 },
	{ +0.19208039704988766000, -0.98137919331372592000 },
	{ +0.19057475482024802000, -0.98167268619695447000 },
	{ +0.18906866414980150000, -0.98196386910952649000 },
	{ +0.18756212858252494000, -0.98225274136626062000 },
	{ +0.18605515166344205000, -0.98253930228741249000 },
	{ +0.18454773693861506000, -0.98282355119867637000 },
	{ +0.18303988795513643000, -0.98310548743118742000 },
	{ +0.18153160826112050000, -0.98338511032152220000 },
	{ +0.18002290140569507000, -0.98366241921170117000 },
	{ +0.17851377093899309000, -0.98393741344918983000 },
	{ +0.17700422041214436000, -0.98421009238689994000 },
	{ +0.17549425337726707000, -0.98448045538319173000 },
	{ +0.17398387338745949000, -0.98474850180187501000 },
	{ +0.17247308399679168000, -0.98501423101221064000 },
	{ +0.17096188876029697000, -0.98527764238891202000 },
	{ +0.16945029123396374000, -0.98553873531214675000 },
	{ +0.16793829497472701000, -0.98579750916753806000 },
	{ +0.16642590354045997000, -0.98605396334616602000 },
	{ +0.16491312048996581000, -0.98630809724456925000 },
	{ +0.16339994938296917000, -0.98655991026474599000 },
	{ +0.16188639378010783000, -0.98680940181415600000 },
	{ +0.16037245724292429000, -0.98705657130572155000 },
	{ +0.15885814333385750000, -0.98730141815782879000 },
	{ +0.15734345561623436000, -0.98754394179432958000 },
	{ +0.15582839765426137000, -0.98778414164454242000 },
	{ +0.15431297301301627000, -0.98802201714325377000 },
	{ +0.15279718525843963000, -0.98825756773071971000 },
	{ +0.15128103795732648000, -0.98849079285266683000 },
	{ +0.14976453467731782000, -0.98872169196029391000 },
	{ +0.14824767898689239000, -0.98895026451027301000 },
	{ +0.14673047445535814000, -0.98917650996475104000 },
	{ +0.14521292465284388000, -0.98940042779135040000 },
	{ +0.14369503315029092000, -0.98962201746317080000 },
	{ +0.14217680351944456000, -0.98984127845879055000 },
	{ +0.14065823933284577000, -0.99005821026226715000 },
	{ +0.13913934416382279000, -0.99027281236313913000 },
	{ +0.13762012158648265000, -0.99048508425642701000 },
	{ +0.13610057517570287000, -0.99069502544263455000 },
	{ +0.13458070850712289000, -0.99090263542774981000 },
	{ +0.13306052515713582000, -0.99110791372324658000 },
	{ +0.13154002870287990000, -0.99131085984608514000 },
	{ +0.13001922272223018000, -0.99151147331871359000 },
	{ +0.12849811079379003000, -0.99170975366906922000 },
	{ +0.12697669649688276000, -0.99190570043057902000 },
	{ +0.12545498341154318000, -0.99209931314216138000 },
	{ +0.12393297511850915000, -0.99229059134822695000 },
	{ +0.12241067519921321000, -0.99247953459867955000 },
	{ +0.12088808723577414000, -0.99266614244891749000 },
	{ +0.11936521481098848000, -0.99285041445983446000 },
	{ +0.11784206150832215000, -0.99303235019782066000 },
	{ +0.11631863091190199000, -0.99321194923476386000 },
	{ +0.11479492660650735000, -0.99338921114805001000 },
	{ +0.11327095217756165000, -0.99356413552056466000 },
	{ +0.11174671121112395000, -0.99373672194069396000 },
	{ +0.11022220729388045000, -0.99390697000232531000 },
	{ +0.10869744401313615000, -0.99407487930484861000 },
	{ +0.10717242495680633000, -0.99424044945315704000 },
	{ +0.10564715371340815000, -0.99440368005764812000 },
	{ +0.10412163387205216000, -0.99456457073422444000 },
	{ +0.10259586902243391000, -0.99472312110429473000 },
	{ +0.10106986275482549000, -0.99487933079477464000 },
	{ +0.09954361866006704300, -0.99503319943808755000 },
	{ +0.09801714032955835500, -0.99518472667216584000 },
	{ +0.09649043135525038600, -0.99533391214045119000 },
	{ +0.09496349532963684100, -0.99548075549189585000 },
	{ +0.09343633584574566300, -0.99562525638096311000 },
	{ +0.09190895649713065600, -0.99576741446762862000 },
	{ +0.09038136087786295700, -0.99590722941738052000 },
	{ +0.08885355258252261600, -0.99604470090122077000 },
	{ +0.08732553520619013000, -0.99617982859566567000 },
	{ +0.08579731234443799300, -0.99631261218274669000 },
	{ +0.08426888759332222500, -0.99644305135001132000 },
	{ +0.08274026454937388800, -0.99657114579052342000 },
	{ +0.08121144680959067900, -0.99669689520286464000 },
	{ +0.07968243797142840500, -0.99682029929113425000 },
	{ +0.07815324163279256700, -0.99694135776495063000 },
	{ +0.07662386139202986800, -0.99706007033945143000 },
	{ +0.07509430084791975100, -0.99717643673529455000 },
	{ +0.07356456359966590000, -0.99729045667865857000 },
	{ +0.07203465324688784800, -0.99740212990124366000 },
	{ +0.07050457338961244000, -0.99751145614027170000 },
	{ +0.06897432762826535800, -0.99761843513848769000 },
	{ +0.06744391956366273300, -0.99772306664415966000 },
	{ +0.06591335279700254200, -0.99782535041107967000 },
	{ +0.06438263092985623000, -0.99792528619856402000 },
	{ +0.06285175756416022700, -0.99802287377145416000 },
	{ +0.06132073630220743300, -0.99811811290011709000 },
	{ +0.05978957074663877900, -0.99821100336044610000 },
	{ +0.05825826450043471200, -0.99830154493386081000 },
	{ +0.05672682116690674900, -0.99838973740730808000 },
	{ +0.05519524434968898400, -0.99847558057326269000 },
	{ +0.05366353765272961100, -0.99855907422972723000 },
	{ +0.05213170468028245600, -0.99864021818023307000 },
	{ +0.05059974903689846300, -0.99871901223384074000 },
	{ +0.04906767432741724500, -0.99879545620514021000 },
	{ +0.04753548415695858100, -0.99886954991425136000 },
	{ +0.04600318213091395000, -0.99894129318682456000 },
	{ +0.04447077185493803600, -0.99901068585404107000 },
	{ +0.04293825693494024400, -0.99907772775261317000 },
	{ +0.04140564097707621200, -0.99914241872478460000 },
	{ +0.03987292758773933200, -0.99920475861833147000 },
	{ +0.03834012037355226400, -0.99926474728656201000 },
	{ +0.03680722294135845000, -0.99932238458831701000 },
	{ +0.03527423889821361400, -0.99937767038797032000 },
	{ +0.03374117185137729500, -0.99943060455542920000 },
	{ +0.03220802540830434300, -0.99948118696613431000 },
	{ +0.03067480317663643500, -0.99952941750106050000 },
	{ +0.02914150876419358000, -0.99957529604671658000 },
	{ +0.02760814577896564600, -0.99961882249514589000 },
	{ +0.02607471782910385600, -0.99965999674392647000 },
	{ +0.02454122852291229200, -0.99969881869617139000 },
	{ +0.02300768146883942800, -0.99973528826052882000 },
	{ +0.02147408027546961200, -0.99976940535118231000 },
	{ +0.01994042855151459400, -0.99980116988785117000 },
	{ +0.01840672990580502500, -0.99983058179579032000 },
	{ +0.01687298794728196700, -0.99985764100579066000 },
	{ +0.01533920628498840400, -0.99988234745417937000 },
	{ +0.01380538852806074400, -0.99990470108281970000 },
	{ +0.01227153828572033000, -0.99992470183911131000 },
	{ +0.01073765916726494500, -0.99994234967599060000 },
	{ +0.00920375478206032420, -0.99995764455193059000 },
	{ +0.00766982873953165390, -0.99997058643094083000 },
	{ +0.00613588464915508330, -0.99998117528256791000 },
	{ +0.00460192612044922970, -0.99998941108189510000 },
	{ +0.00306795676296668650, -0.99999529380954288000 },
	{ +0.00153398018628552680, -0.99999882345166857000 },
	{ +0.00000000000000081250, -0.99999999999996658000 },
	{ -0.00153398018628390180, -0.99999882345166846000 },
	{ -0.00306795676296506110, -0.99999529380954266000 },
	{ -0.00460192612044760430, -0.99998941108189476000 },
	{ -0.00613588464915345790, -0.99998117528256747000 },
	{ -0.00766982873953002840, -0.99997058643094039000 },
	{ -0.00920375478205869880, -0.99995764455193015000 },
	{ -0.01073765916726332000, -0.99994234967599016000 },
	{ -0.01227153828571870200, -0.99992470183911075000 },
	{ -0.01380538852805911400, -0.99990470108281904000 },
	{ -0.01533920628498677100, -0.99988234745417870000 },
	{ -0.01687298794728033300, -0.99985764100579000000 },
	{ -0.01840672990580338700, -0.99983058179578954000 },
	{ -0.01994042855151295300, -0.99980116988785028000 },
	{ -0.02147408027546796700, -0.99976940535118131000 },
	{ -0.02300768146883778000, -0.99973528826052771000 },
	{ -0.02454122852291064400, -0.99969881869617017000 },
	{ -0.02607471782910220400, -0.99965999674392514000 },
	{ -0.02760814577896399100, -0.99961882249514444000 },
	{ -0.02914150876419192200, -0.99957529604671502000 },
	{ -0.03067480317663477000, -0.99952941750105895000 },
	{ -0.03220802540830267100, -0.99948118696613264000 },
	{ -0.03374117185137562300, -0.99943060455542743000 },
	{ -0.03527423889821193500, -0.99937767038796854000 },
	{ -0.03680722294135676400, -0.99932238458831513000 },
	{ -0.03834012037355057100, -0.99926474728656012000 },
	{ -0.03987292758773763200, -0.99920475861832958000 },
	{ -0.04140564097707450500, -0.99914241872478260000 },
	{ -0.04293825693493853700, -0.99907772775261106000 },
	{ -0.04447077185493632900, -0.99901068585403896000 },
	{ -0.04600318213091224300, -0.99894129318682245000 },
	{ -0.04753548415695686700, -0.99886954991424914000 },
	{ -0.04906767432741551700, -0.99879545620513788000 },
	{ -0.05059974903689672800, -0.99871901223383830000 },
	{ -0.05213170468028071500, -0.99864021818023052000 },
	{ -0.05366353765272786200, -0.99855907422972467000 },
	{ -0.05519524434968722100, -0.99847558057326014000 },
	{ -0.05672682116690497300, -0.99838973740730552000 },
	{ -0.05825826450043292100, -0.99830154493385814000 },
	{ -0.05978957074663698200, -0.99821100336044344000 },
	{ -0.06132073630220562900, -0.99811811290011443000 },
	{ -0.06285175756415840900, -0.99802287377145138000 },
	{ -0.06438263092985439800, -0.99792528619856113000 },
	{ -0.06591335279700069600, -0.99782535041107667000 },
	{ -0.06744391956366088700, -0.99772306664415655000 },
	{ -0.06897432762826352600, -0.99761843513848447000 },
	{ -0.07050457338961059500, -0.99751145614026837000 },
	{ -0.07203465324688598800, -0.99740212990124022000 },
	{ -0.07356456359966404000, -0.99729045667865512000 },
	{ -0.07509430084791787800, -0.99717643673529099000 },
	{ -0.07662386139202799500, -0.99706007033944777000 },
	{ -0.07815324163279069300, -0.99694135776494686000 },
	{ -0.07968243797142651700, -0.99682029929113036000 },
	{ -0.08121144680958877800, -0.99669689520286064000 },
	{ -0.08274026454937197200, -0.99657114579051942000 },
	{ -0.08426888759332029600, -0.99644305135000721000 },
	{ -0.08579731234443605000, -0.99631261218274259000 },
	{ -0.08732553520618817300, -0.99617982859566145000 },
	{ -0.08885355258252064500, -0.99604470090121644000 },
	{ -0.09038136087786097200, -0.99590722941737608000 },
	{ -0.09190895649712865800, -0.99576741446762407000 },
	{ -0.09343633584574365100, -0.99562525638095856000 },
	{ -0.09496349532963481400, -0.99548075549189119000 },
	{ -0.09649043135524834600, -0.99533391214044653000 },
	{ -0.09801714032955630200, -0.99518472667216118000 },
	{ -0.09954361866006497500, -0.99503319943808277000 },
	{ -0.10106986275482341000, -0.99487933079476976000 },
	{ -0.10259586902243181000, -0.99472312110428984000 },
	{ -0.10412163387205005000, -0.99456457073421956000 },
	{ -0.10564715371340602000, -0.99440368005764312000 },
	{ -0.10717242495680421000, -0.99424044945315204000 },
	{ -0.10869744401313403000, -0.99407487930484340000 },
	{ -0.11022220729387831000, -0.99390697000231998000 },
	{ -0.11174671121112179000, -0.99373672194068852000 },
	{ -0.11327095217755948000, -0.99356413552055922000 },
	{ -0.11479492660650516000, -0.99338921114804446000 },
	{ -0.11631863091189978000, -0.99321194923475842000 },
	{ -0.11784206150831993000, -0.99303235019781522000 },
	{ -0.11936521481098626000, -0.99285041445982891000 },
	{ -0.12088808723577192000, -0.99266614244891183000 },
	{ -0.12241067519921099000, -0.99247953459867377000 },
	{ -0.12393297511850691000, -0.99229059134822106000 },
	{ -0.12545498341154093000, -0.99209931314215538000 },
	{ -0.12697669649688051000, -0.99190570043057291000 },
	{ -0.12849811079378776000, -0.99170975366906300000 },
	{ -0.13001922272222788000, -0.99151147331870748000 },
	{ -0.13154002870287756000, -0.99131085984607892000 },
	{ -0.13306052515713346000, -0.99110791372324036000 },
	{ -0.13458070850712051000, -0.99090263542774348000 },
	{ -0.13610057517570048000, -0.99069502544262800000 },
	{ -0.13762012158648027000, -0.99048508425642035000 },
	{ -0.13913934416382037000, -0.99027281236313236000 },
	{ -0.14065823933284333000, -0.99005821026226026000 },
	{ -0.14217680351944209000, -0.98984127845878367000 },
	{ -0.14369503315028842000, -0.98962201746316392000 },
	{ -0.14521292465284136000, -0.98940042779134352000 },
	{ -0.14673047445535559000, -0.98917650996474416000 },
	{ -0.14824767898688981000, -0.98895026451026613000 },
	{ -0.14976453467731524000, -0.98872169196028692000 },
	{ -0.15128103795732387000, -0.98849079285265973000 },
	{ -0.15279718525843702000, -0.98825756773071260000 },
	{ -0.15431297301301364000, -0.98802201714324667000 },
	{ -0.15582839765425871000, -0.98778414164453521000 },
	{ -0.15734345561623167000, -0.98754394179432237000 },
	{ -0.15885814333385478000, -0.98730141815782146000 },
	{ -0.16037245724292154000, -0.98705657130571411000 },
	{ -0.16188639378010505000, -0.98680940181414856000 },
	{ -0.16339994938296637000, -0.98655991026473855000 },
	{ -0.16491312048996301000, -0.98630809724456181000 },
	{ -0.16642590354045717000, -0.98605396334615858000 },
	{ -0.16793829497472415000, -0.98579750916753051000 },
	{ -0.16945029123396085000, -0.98553873531213920000 },
	{ -0.17096188876029406000, -0.98527764238890436000 },
	{ -0.17247308399678873000, -0.98501423101220287000 },
	{ -0.17398387338745652000, -0.98474850180186735000 },
	{ -0.17549425337726407000, -0.98448045538318407000 },
	{ -0.17700422041214131000, -0.98421009238689217000 },
	{ -0.17851377093899001000, -0.98393741344918195000 },
	{ -0.18002290140569194000, -0.98366241921169328000 },
	{ -0.18153160826111733000, -0.98338511032151421000 },
	{ -0.18303988795513323000, -0.98310548743117931000 },
	{ -0.18454773693861182000, -0.98282355119866827000 },
	{ -0.18605515166343878000, -0.98253930228740427000 },
	{ -0.18756212858252166000, -0.98225274136625240000 },
	{ -0.18906866414979823000, -0.98196386910951827000 },
	{ -0.19057475482024475000, -0.98167268619694614000 },
	{ -0.19208039704988436000, -0.98137919331371759000 },
	{ -0.19358558729579547000, -0.98108339115044962000 },
	{ -0.19509032201612006000, -0.98078528040319335000 },
	{ -0.19659459767007195000, -0.98048486177343230000 },
	{ -0.19809841071794526000, -0.98018213596808035000 },
	{ -0.19960175762112259000, -0.97987710369948056000 },
	{ -0.20110463484208346000, -0.97956976568540344000 },
	{ -0.20260703884441261000, -0.97926012264904494000 },
	{ -0.20410896609280829000, -0.97894817531902512000 },
	{ -0.20561041305309061000, -0.97863392442938602000 },
	{ -0.20711137619220982000, -0.97831737071959046000 },
	{ -0.20861185197825471000, -0.97799851493451995000 },
	{ -0.21011183688046078000, -0.97767735782447285000 },
	{ -0.21161132736921867000, -0.97735390014516288000 },
	{ -0.21311031991608240000, -0.97702814265771720000 },
	{ -0.21460881099377774000, -0.97670008612867465000 },
	{ -0.21610679707621042000, -0.97636973132998395000 },
	{ -0.21760427463847448000, -0.97603707903900183000 },
	{ -0.21910124015686058000, -0.97570213003849127000 },
	{ -0.22059769010886426000, -0.97536488511661967000 },
	{ -0.22209362097319421000, -0.97502534506695693000 },
	{ -0.22358902922978061000, -0.97468351068847348000 },
	{ -0.22508391135978340000, -0.97433938278553867000 },
	{ -0.22657826384560051000, -0.97399296216791864000 },
	{ -0.22807208317087618000, -0.97364424965077467000 },
	{ -0.22956536582050927000, -0.97329324605466094000 },
	{ -0.23105810828066142000, -0.97293995220552287000 },
	{ -0.23255030703876550000, -0.97258436893469502000 },
	{ -0.23404195858353363000, -0.97222649707889897000 },
	{ -0.23553305940496566000, -0.97186633748024198000 },
	{ -0.23702360599435732000, -0.97150389098621437000 },
	{ -0.23851359484430845000, -0.97113915844968768000 },
	{ -0.24000302244873145000, -0.97077214072891282000 },
	{ -0.24149188530285925000, -0.97040283868751798000 },
	{ -0.24298017990325371000, -0.97003125319450645000 },
	{ -0.24446790274781391000, -0.96965738512425492000 },
	{ -0.24595505033578427000, -0.96928123535651090000 },
	{ -0.24744161916776289000, -0.96890280477639135000 },
	{ -0.24892760574570968000, -0.96852209427437985000 },
	{ -0.25041300657295473000, -0.96813910474632492000 },
	{ -0.25189781815420637000, -0.96775383709343799000 },
	{ -0.25338203699555950000, -0.96736629222229109000 },
	{ -0.25486565960450386000, -0.96697647104481466000 },
	{ -0.25634868248993214000, -0.96658437447829559000 },
	{ -0.25783110216214816000, -0.96619000344537498000 },
	{ -0.25931291513287530000, -0.96579335887404605000 },
	{ -0.26079411791526452000, -0.96539444169765176000 },
	{ -0.26227470702390260000, -0.96499325285488269000 },
	{ -0.26375467897482030000, -0.96458979328977512000 },
	{ -0.26523403028550069000, -0.96418406395170819000 },
	{ -0.26671275747488721000, -0.96377606579540220000 },
	{ -0.26819085706339191000, -0.96336579978091641000 },
	{ -0.26966832557290371000, -0.96295326687364624000 },
	{ -0.27114515952679652000, -0.96253846804432153000 },
	{ -0.27262135544993737000, -0.96212140426900394000 },
	{ -0.27409690986869467000, -0.96170207652908490000 },
	{ -0.27557181931094638000, -0.96128048581128300000 },
	{ -0.27704608030608807000, -0.96085663310764191000 },
	{ -0.27851968938504118000, -0.96043051941552804000 },
	{ -0.27999264308026128000, -0.96000214573762821000 },
	{ -0.28146493792574595000, -0.95957151308194688000 },
	{ -0.28293657045704323000, -0.95913862246180437000 },
	{ -0.28440753721125966000, -0.95870347489583396000 },
	{ -0.28587783472706835000, -0.95826607140798004000 },
	{ -0.28734745954471719000, -0.95782641302749527000 },
	{ -0.28881640820603705000, -0.95738450078893833000 },
	{ -0.29028467725444990000, -0.95694033573217119000 },
	{ -0.29175226323497672000, -0.95649391890235735000 },
	{ -0.29321916269424603000, -0.95604525134995866000 },
	{ -0.29468537218050161000, -0.95559433413073325000 },
	{ -0.29615088824361102000, -0.95514116830573281000 },
	{ -0.29761570743507337000, -0.95468575494130037000 },
	{ -0.29907982630802754000, -0.95422809510906759000 },
	{ -0.30054324141726046000, -0.95376818988595236000 },
	{ -0.30200594931921498000, -0.95330604035415589000 },
	{ -0.30346794657199816000, -0.95284164760116075000 },
	{ -0.30492922973538916000, -0.95237501271972791000 },
	{ -0.30638979537084760000, -0.95190613680789438000 },
	{ -0.30784964004152143000, -0.95143502096897037000 },
	{ -0.30930876031225518000, -0.95096166631153700000 },
	{ -0.31076715274959787000, -0.95048607394944362000 },
	{ -0.31222481392181123000, -0.95000824500180492000 },
	{ -0.31368174039887770000, -0.94952818059299859000 },
	{ -0.31513792875250857000, -0.94904588185266248000 },
	{ -0.31659337555615197000, -0.94856134991569219000 },
	{ -0.31804807738500096000, -0.94807458592223814000 },
	{ -0.31950203081600165000, -0.94758559101770301000 },
	{ -0.32095523242786117000, -0.94709436635273903000 },
	{ -0.32240767880105575000, -0.94660091308324534000 },
	{ -0.32385936651783875000, -0.94610523237036515000 },
	{ -0.32531029216224877000, -0.94560732538048298000 },
	{ -0.32676045232011758000, -0.94510719328522219000 },
	{ -0.32820984357907829000, -0.94460483726144195000 },
	{ -0.32965846252857317000, -0.94410025849123436000 },
	{ -0.33110630575986200000, -0.94359345816192208000 },
	{ -0.33255336986602979000, -0.94308443746605519000 },
	{ -0.33399965144199484000, -0.94257319760140856000 },
	{ -0.33544514708451700000, -0.94205973977097901000 },
	{ -0.33688985339220534000, -0.94154406518298250000 },
	{ -0.33833376696552636000, -0.94102617505085095000 },
	{ -0.33977688440681203000, -0.94050607059322999000 },
	{ -0.34121920232026748000, -0.93998375303397563000 },
	{ -0.34266071731197945000, -0.93945922360215162000 },
	{ -0.34410142598992383000, -0.93893248353202630000 },
	{ -0.34554132496397399000, -0.93840353406306987000 },
	{ -0.34698041084590853000, -0.93787237643995158000 },
	{ -0.34841868024941935000, -0.93733901191253666000 },
	{ -0.34985612979011965000, -0.93680344173588337000 },
	{ -0.35129275608555177000, -0.93626566717024007000 },
	{ -0.35272855575519529000, -0.93572568948104207000 },
	{ -0.35416352542047491000, -0.93518350993890931000 },
	{ -0.35559766170476831000, -0.93463912981964248000 },
	{ -0.35703096123341438000, -0.93409255040422057000 },
	{ -0.35846342063372083000, -0.93354377297879787000 },
	{ -0.35989503653497235000, -0.93299279883470054000 },
	{ -0.36132580556843841000, -0.93243962926842405000 },
	{ -0.36275572436738129000, -0.93188426558162973000 },
	{ -0.36418478956706390000, -0.93132670908114201000 },
	{ -0.36561299780475781000, -0.93076696107894519000 },
	{ -0.36704034571975108000, -0.93020502289218054000 },
	{ -0.36846682995335617000, -0.92964089584314269000 },
	{ -0.36989244714891789000, -0.92907458125927722000 },
	{ -0.37131719395182122000, -0.92850608047317695000 },
	{ -0.37274106700949944000, -0.92793539482257925000 },
	{ -0.37416406297144156000, -0.92736252565036248000 },
	{ -0.37558617848920073000, -0.92678747430454322000 },
	{ -0.37700741021640172000, -0.92621024213827274000 },
	{ -0.37842775480874902000, -0.92563083050983419000 },
	{ -0.37984720892403456000, -0.92504924078263895000 },
	{ -0.38126576922214572000, -0.92446547432522397000 },
	{ -0.38268343236507307000, -0.92387953251124810000 },
	{ -0.38410019501691828000, -0.92329141671948900000 },
	{ -0.38551605384390203000, -0.92270112833383988000 },
	{ -0.38693100551437165000, -0.92210866874330644000 },
	{ -0.38834504669880932000, -0.92151403934200327000 },
	{ -0.38975817406983942000, -0.92091724152915078000 },
	{ -0.39117038430223683000, -0.92031827670907185000 },
	{ -0.39258167407293437000, -0.91971714629118861000 },
	{ -0.39399204006103095000, -0.91911385169001902000 },
	{ -0.39540147894779915000, -0.91850839432517351000 },
	{ -0.39680998741669304000, -0.91790077562135175000 },
	{ -0.39821756215335624000, -0.91729099700833927000 },
	{ -0.39962419984562941000, -0.91667905992100396000 },
	{ -0.40102989718355819000, -0.91606496579929297000 },
	{ -0.40243465085940094000, -0.91544871608822909000 },
	{ -0.40383845756763659000, -0.91483031223790745000 },
	{ -0.40524131400497226000, -0.91420975570349194000 },
	{ -0.40664321687035132000, -0.91358704794521217000 },
	{ -0.40804416286496092000, -0.91296219042835947000 },
	{ -0.40944414869223977000, -0.91233518462328400000 },
	{ -0.41084317105788604000, -0.91170603200539113000 },
	{ -0.41224122666986490000, -0.91107473405513761000 },
	{ -0.41363831223841646000, -0.91044129225802850000 },
	{ -0.41503442447606342000, -0.90980570810461359000 },
	{ -0.41642956009761889000, -0.90916798309048374000 },
	{ -0.41782371582019395000, -0.90852811871626749000 },
	{ -0.41921688836320553000, -0.90788611648762763000 },
	{ -0.42060907444838402000, -0.90724197791525718000 },
	{ -0.42200027079978109000, -0.90659570451487670000 },
	{ -0.42339047414377734000, -0.90594729780722982000 },
	{ -0.42477968120909010000, -0.90529675931808007000 },
	{ -0.42616788872678085000, -0.90464409057820749000 },
	{ -0.42755509343026327000, -0.90398929312340459000 },
	{ -0.42894129205531062000, -0.90333236849447318000 },
	{ -0.43032648134006368000, -0.90267331823722019000 },
	{ -0.43171065802503827000, -0.90201214390245454000 },
	{ -0.43309381885313292000, -0.90134884704598339000 },
	{ -0.43447596056963655000, -0.90068342922860822000 },
	{ -0.43585707992223627000, -0.90001589201612153000 },
	{ -0.43723717366102482000, -0.89934623697930283000 },
	{ -0.43861623853850834000, -0.89867446569391518000 },
	{ -0.43999427130961388000, -0.89800057974070113000 },
	{ -0.44137126873169724000, -0.89732458070537957000 },
	{ -0.44274722756455048000, -0.89664647017864152000 },
	{ -0.44412214457040960000, -0.89596624975614647000 },
	{ -0.44549601651396203000, -0.89528392103851884000 },
	{ -0.44686884016235440000, -0.89459948563134395000 },
	{ -0.44824061228520007000, -0.89391294514516451000 },
	{ -0.44961132965458672000, -0.89322430119547658000 },
	{ -0.45098098904508388000, -0.89253355540272583000 },
	{ -0.45234958723375079000, -0.89184070939230387000 },
	{ -0.45371712100014372000, -0.89114576479454433000 },
	{ -0.45508358712632357000, -0.89044872324471891000 },
	{ -0.45644898239686360000, -0.88974958638303381000 },
	{ -0.45781330359885680000, -0.88904835585462549000 },
	{ -0.45917654752192366000, -0.88834503330955716000 },
	{ -0.46053871095821947000, -0.88763962040281474000 },
	{ -0.46189979070244214000, -0.88693211879430289000 },
	{ -0.46325978355183955000, -0.88622253014884134000 },
	{ -0.46461868630621711000, -0.88551085613616065000 },
	{ -0.46597649576794536000, -0.88479709843089849000 },
	{ -0.46733320874196760000, -0.88408125871259569000 },
	{ -0.46868882203580703000, -0.88336333866569228000 },
	{ -0.47004333245957464000, -0.88264333997952349000 },
	{ -0.47139673682597660000, -0.88192126434831575000 },
	{ -0.47274903195032170000, -0.88119711347118279000 },
	{ -0.47410021465052887000, -0.88047088905212145000 },
	{ -0.47545028174713466000, -0.87974259280000822000 },
	{ -0.47679923006330083000, -0.87901222642859433000 },
	{ -0.47814705642482169000, -0.87827979165650238000 },
	{ -0.47949375766013158000, -0.87754529020722216000 },
	{ -0.48083933060031242000, -0.87680872380910657000 },
	{ -0.48218377207910118000, -0.87607009419536752000 },
	{ -0.48352707893289709000, -0.87532940310407181000 },
	{ -0.48486924800076941000, -0.87458665227813703000 },
	{ -0.48621027612446466000, -0.87384184346532767000 },
	{ -0.48755016014841412000, -0.87309497841825090000 },
	{ -0.48888889691974130000, -0.87234605889435224000 },
	{ -0.49022648328826923000, -0.87159508665591179000 },
	{ -0.49156291610652797000, -0.87084206347003967000 },
	{ -0.49289819222976194000, -0.87008699110867216000 },
	{ -0.49423230851593758000, -0.86932987134856754000 },
	{ -0.49556526182575034000, -0.86857070597130170000 },
	{ -0.49689704902263232000, -0.86780949676326413000 },
	{ -0.49822766697275955000, -0.86704624551565357000 },
	{ -0.49955711254505952000, -0.86628095402447391000 },
	{ -0.50088538261121829000, -0.86551362409053001000 },
	{ -0.50221247404568825000, -0.86474425751942330000 },
	{ -0.50353838372569504000, -0.86397285612154762000 },
	{ -0.50486310853124494000, -0.86319942171208497000 },
	{ -0.50618664534513269000, -0.86242395611100131000 },
	{ -0.50750899105294822000, -0.86164646114304211000 },
	{ -0.50883014254308434000, -0.86086693863772812000 },
	{ -0.51015009670674405000, -0.86008539042935106000 },
	{ -0.51146885043794765000, -0.85930181835696939000 },
	{ -0.51278640063354020000, -0.85851622426440377000 },
	{ -0.51410274419319890000, -0.85772861000023304000 },
	{ -0.51541787801944006000, -0.85693897741778968000 },
	{ -0.51673179901762689000, -0.85614732837515539000 },
	{ -0.51804450409597624000, -0.85535366473515695000 },
	{ -0.51935599016556644000, -0.85455798836536145000 },
	{ -0.52066625414034395000, -0.85376030113807233000 },
	{ -0.52197529293713107000, -0.85296060493032466000 },
	{ -0.52328310347563312000, -0.85215890162388086000 },
	{ -0.52458968267844552000, -0.85135519310522612000 },
	{ -0.52589502747106120000, -0.85054948126556440000 },
	{ -0.52719913478187785000, -0.84974176800081347000 },
	{ -0.52850200154220484000, -0.84893205521160064000 },
	{ -0.52980362468627096000, -0.84812034480325826000 },
	{ -0.53110400115123113000, -0.84730663868581935000 },
	{ -0.53240312787717403000, -0.84649093877401316000 },
	{ -0.53370100180712898000, -0.84567324698726021000 },
	{ -0.53499761988707306000, -0.84485356524966826000 },
	{ -0.53629297906593898000, -0.84403189549002755000 },
	{ -0.53758707629562130000, -0.84320823964180658000 },
	{ -0.53887990853098411000, -0.84238259964314699000 },
	{ -0.54017147272986854000, -0.84155497743685947000 },
	{ -0.54146176585309913000, -0.84072537497041910000 },
	{ -0.54275078486449146000, -0.83989379419596055000 },
	{ -0.54403852673085940000, -0.83906023707027377000 },
	{ -0.54532498842202193000, -0.83822470555479911000 },
	{ -0.54661016691081032000, -0.83738720161562297000 },
	{ -0.54789405917307554000, -0.83654772722347304000 },
	{ -0.54917666218769501000, -0.83570628435371364000 },
	{ -0.55045797293658005000, -0.83486287498634115000 },
	{ -0.55173798840468258000, -0.83401750110597928000 },
	{ -0.55301670558000260000, -0.83317016470187433000 },
	{ -0.55429412145359513000, -0.83232086776789083000 },
	{ -0.55557023301957709000, -0.83146961230250649000 },
	{ -0.55684503727513479000, -0.83061640030880746000 },
	{ -0.55811853122053068000, -0.82976123379448419000 },
	{ -0.55939071185911049000, -0.82890411477182602000 },
	{ -0.56066157619731039000, -0.82804504525771694000 },
	{ -0.56193112124466371000, -0.82718402727363027000 },
	{ -0.56319934401380833000, -0.82632106284562468000 },
	{ -0.56446624152049363000, -0.82545615400433869000 },
	{ -0.56573181078358736000, -0.82458930278498643000 },
	{ -0.56699604882508281000, -0.82372051122735257000 },
	{ -0.56825895267010562000, -0.82284978137578757000 },
	{ -0.56952051934692116000, -0.82197711527920281000 },
	{ -0.57078074588694117000, -0.82110251499106590000 },
	{ -0.57203962932473085000, -0.82022598256939594000 },
	{ -0.57329716669801589000, -0.81934752007675826000 },
	{ -0.57455335504768945000, -0.81846712958026002000 },
	{ -0.57580819141781892000, -0.81758481315154508000 },
	{ -0.57706167285565313000, -0.81670057286678921000 },
	{ -0.57831379641162906000, -0.81581441080669514000 },
	{ -0.57956455913937921000, -0.81492632905648787000 },
	{ -0.58081395809573799000, -0.81403632970590967000 },
	{ -0.58206199034074890000, -0.81314441484921485000 },
	{ -0.58330865293767153000, -0.81225058658516525000 },
	{ -0.58455394295298846000, -0.81135484701702509000 },
	{ -0.58579785745641189000, -0.81045719825255613000 },
	{ -0.58704039352089088000, -0.80955764240401262000 },
	{ -0.58828154822261813000, -0.80865618158813635000 },
	{ -0.58952131864103674000, -0.80775281792615172000 },
	{ -0.59075970185884696000, -0.80684755354376070000 },
	{ -0.59199669496201357000, -0.80594039057113775000 },
	{ -0.59323229503977237000, -0.80503133114292502000 },
	{ -0.59446649918463701000, -0.80412037739822717000 },
	{ -0.59569930449240582000, -0.80320753148060642000 },
	{ -0.59693070806216886000, -0.80229279553807731000 },
	{ -0.59816070699631463000, -0.80137617172310183000 },
	{ -0.59938929840053679000, -0.80045766219258441000 },
	{ -0.60061647938384111000, -0.79953726910786671000 },
	{ -0.60184224705855216000, -0.79861499463472252000 },
	{ -0.60306659854032019000, -0.79769084094335285000 },
	{ -0.60428953094812798000, -0.79676481020838041000 },
	{ -0.60551104140429735000, -0.79583690460884515000 },
	{ -0.60673112703449628000, -0.79490712632819871000 },
	{ -0.60794978496774543000, -0.79397547755429887000 },
	{ -0.60916701233642490000, -0.79304196047940545000 },
	{ -0.61038280627628116000, -0.79210657730017409000 },
	{ -0.61159716392643360000, -0.79116933021765201000 },
	{ -0.61281008242938129000, -0.79023022143727184000 },
	{ -0.61402155893100996000, -0.78928925316884757000 },
	{ -0.61523159058059840000, -0.78834642762656815000 },
	{ -0.61644017453082511000, -0.78740174702899324000 },
	{ -0.61764730793777545000, -0.78645521359904758000 },
	{ -0.61885298796094779000, -0.78550682956401574000 },
	{ -0.62005721176326067000, -0.78455659715553705000 },
	{ -0.62125997651105902000, -0.78360451860960012000 },
	{ -0.62246127937412132000, -0.78265059616653765000 },
	{ -0.62366111752566578000, -0.78169483207102131000 },
	{ -0.62485948814235759000, -0.78073722857205641000 },
	{ -0.62605638840431466000, -0.77977778792297636000 },
	{ -0.62725181549511511000, -0.77881651238143779000 },
	{ -0.62844576660180362000, -0.77785340420941484000 },
	{ -0.62963823891489790000, -0.77688846567319425000 },
	{ -0.63082922962839538000, -0.77592169904336938000 },
	{ -0.63201873593977986000, -0.77495310659483574000 },
	{ -0.63320675505002799000, -0.77398269060678471000 },
	{ -0.63439328416361618000, -0.77301045336269880000 },
	{ -0.63557832048852680000, -0.77203639715034633000 },
	{ -0.63676186123625489000, -0.77106052426177563000 },
	{ -0.63794390362181463000, -0.77008283699330982000 },
	{ -0.63912444486374620000, -0.76910333764554151000 },
	{ -0.64030348218412203000, -0.76812202852332723000 },
	{ -0.64148101280855341000, -0.76713891193578232000 },
	{ -0.64265703396619711000, -0.76615399019627484000 },
	{ -0.64383154288976163000, -0.76516726562242088000 },
	{ -0.64500453681551406000, -0.76417874053607870000 },
	{ -0.64617601298328631000, -0.76318841726334330000 },
	{ -0.64734596863648197000, -0.76219629813454093000 },
	{ -0.64851440102208224000, -0.76120238548422381000 },
	{ -0.64968130739065288000, -0.76020668165116445000 },
	{ -0.65084668499635057000, -0.75920918897835010000 },
	{ -0.65201053109692908000, -0.75820990981297742000 },
	{ -0.65317284295374622000, -0.75720884650644682000 },
	{ -0.65433361783176991000, -0.75620600141435679000 },
	{ -0.65549285299958471000, -0.75520137689649880000 },
	{ -0.65665054572939829000, -0.75419497531685142000 },
	{ -0.65780669329704788000, -0.75318679904357477000 },
	{ -0.65896129298200656000, -0.75217685044900506000 },
	{ -0.66011434206738961000, -0.75116513190964884000 },
	{ -0.66126583783996129000, -0.75015164580617755000 },
	{ -0.66241577759014070000, -0.74913639452342184000 },
	{ -0.66356415861200868000, -0.74811938045036608000 },
	{ -0.66471097820331360000, -0.74710060598014261000 },
	{ -0.66585623366547841000, -0.74608007351002625000 },
	{ -0.66699992230360616000, -0.74505778544142842000 },
	{ -0.66814204142648703000, -0.74403374417989177000 },
	{ -0.66928258834660448000, -0.74300795213508419000 },
	{ -0.67042156038014145000, -0.74198041172079354000 },
	{ -0.67155895484698669000, -0.74095112535492158000 },
	{ -0.67269476907074111000, -0.73992009545947857000 },
	{ -0.67382900037872417000, -0.73888732446057759000 },
	{ -0.67496164610198006000, -0.73785281478842846000 },
	{ -0.67609270357528395000, -0.73681656887733238000 },
	{ -0.67722217013714836000, -0.73577858916567607000 },
	{ -0.67835004312982927000, -0.73473887809592597000 },
	{ -0.67947631989933266000, -0.73369743811462274000 },
	{ -0.68060099779542060000, -0.73265427167237540000 },
	{ -0.68172407417161729000, -0.73160938122385510000 },
	{ -0.68284554638521544000, -0.73056276922779018000 },
	{ -0.68396541179728276000, -0.72951443814695960000 },
	{ -0.68508366777266771000, -0.72846439044818778000 },
	{ -0.68620031168000584000, -0.72741262860233846000 },
	{ -0.68731534089172630000, -0.72635915508430871000 },
	{ -0.68842875278405757000, -0.72530397237302358000 },
	{ -0.68954054473703397000, -0.72424708295142981000 },
	{ -0.69065071413450163000, -0.72318848930649027000 },
	{ -0.69175925836412477000, -0.72212819392917826000 },
	{ -0.69286617481739154000, -0.72106619931447102000 },
	{ -0.69397146088962081000, -0.72000250796134457000 },
	{ -0.69507511397996757000, -0.71893712237276741000 },
	{ -0.69617713149142957000, -0.71787004505569474000 },
	{ -0.69727751083085310000, -0.71680127852106257000 },
	{ -0.69837624940893928000, -0.71573082528378174000 },
	{ -0.69947334464025013000, -0.71465868786273212000 },
	{ -0.70056879394321458000, -0.71358486878075678000 },
	{ -0.70166259474013459000, -0.71250937056465546000 },
	{ -0.70275474445719122000, -0.71143219574517969000 },
	{ -0.70384524052445085000, -0.71035334685702556000 },
	{ -0.70493408037587069000, -0.70927282643882894000 },
	{ -0.70602126144930544000, -0.70819063703315854000 },
	{ -0.70710678118651304000, -0.70710678118651082000 },
	{ -0.70819063703316076000, -0.70602126144930311000 },
	{ -0.70927282643883105000, -0.70493408037586824000 },
	{ -0.71035334685702767000, -0.70384524052444830000 },
	{ -0.71143219574518168000, -0.70275474445718866000 },
	{ -0.71250937056465746000, -0.70166259474013193000 },
	{ -0.71358486878075866000, -0.70056879394321181000 },
	{ -0.71465868786273401000, -0.69947334464024724000 },
	{ -0.71573082528378351000, -0.69837624940893628000 },
	{ -0.71680127852106423000, -0.69727751083085010000 },
	{ -0.71787004505569640000, -0.69617713149142646000 },
	{ -0.71893712237276897000, -0.69507511397996447000 },
	{ -0.72000250796134613000, -0.69397146088961770000 },
	{ -0.72106619931447247000, -0.69286617481738833000 },
	{ -0.72212819392917960000, -0.69175925836412155000 },
	{ -0.72318848930649160000, -0.69065071413449841000 },
	{ -0.72424708295143103000, -0.68954054473703075000 },
	{ -0.72530397237302480000, -0.68842875278405424000 },
	{ -0.72635915508430993000, -0.68731534089172297000 },
	{ -0.72741262860233968000, -0.68620031168000239000 },
	{ -0.72846439044818900000, -0.68508366777266427000 },
	{ -0.72951443814696071000, -0.68396541179727932000 },
	{ -0.73056276922779118000, -0.68284554638521200000 },
	{ -0.73160938122385610000, -0.68172407417161374000 },
	{ -0.73265427167237629000, -0.68060099779541705000 },
	{ -0.73369743811462362000, -0.67947631989932900000 },
	{ -0.73473887809592675000, -0.67835004312982539000 },
	{ -0.73577858916567673000, -0.67722217013714436000 },
	{ -0.73681656887733304000, -0.67609270357527995000 },
	{ -0.73785281478842912000, -0.67496164610197606000 },
	{ -0.73888732446057814000, -0.67382900037872018000 },
	{ -0.73992009545947901000, -0.67269476907073700000 },
	{ -0.74095112535492191000, -0.67155895484698258000 },
	{ -0.74198041172079365000, -0.67042156038013734000 },
	{ -0.74300795213508419000, -0.66928258834660037000 },
	{ -0.74403374417989165000, -0.66814204142648292000 },
	{ -0.74505778544142820000, -0.66699992230360194000 },
	{ -0.74608007351002603000, -0.66585623366547420000 },
	{ -0.74710060598014227000, -0.66471097820330949000 },
	{ -0.74811938045036563000, -0.66356415861200446000 },
	{ -0.74913639452342129000, -0.66241577759013648000 },
	{ -0.75015164580617688000, -0.66126583783995696000 },
	{ -0.75116513190964807000, -0.66011434206738528000 },
	{ -0.75217685044900429000, -0.65896129298200212000 },
	{ -0.75318679904357400000, -0.65780669329704344000 },
	{ -0.75419497531685065000, -0.65665054572939385000 },
	{ -0.75520137689649791000, -0.65549285299958027000 },
	{ -0.75620600141435579000, -0.65433361783176536000 },
	{ -0.75720884650644571000, -0.65317284295374167000 },
	{ -0.75820990981297642000, -0.65201053109692442000 },
	{ -0.75920918897834910000, -0.65084668499634590000 },
	{ -0.76020668165116345000, -0.64968130739064822000 },
	{ -0.76120238548422281000, -0.64851440102207758000 },
	{ -0.76219629813453982000, -0.64734596863647731000 },
	{ -0.76318841726334208000, -0.64617601298328164000 },
	{ -0.76417874053607748000, -0.64500453681550929000 },
	{ -0.76516726562241955000, -0.64383154288975686000 },
	{ -0.76615399019627350000, -0.64265703396619234000 },
	{ -0.76713891193578099000, -0.64148101280854875000 },
	{ -0.76812202852332589000, -0.64030348218411726000 },
	{ -0.76910333764554006000, -0.63912444486374143000 },
	{ -0.77008283699330826000, -0.63794390362180986000 },
	{ -0.77106052426177396000, -0.63676186123625000000 },
	{ -0.77203639715034456000, -0.63557832048852192000 },
	{ -0.77301045336269691000, -0.63439328416361129000 },
	{ -0.77398269060678271000, -0.63320675505002311000 },
	{ -0.77495310659483363000, -0.63201873593977487000 },
	{ -0.77592169904336727000, -0.63082922962839050000 },
	{ -0.77688846567319203000, -0.62963823891489301000 },
	{ -0.77785340420941262000, -0.62844576660179874000 },
	{ -0.77881651238143546000, -0.62725181549511022000 },
	{ -0.77977778792297381000, -0.62605638840430966000 },
	{ -0.78073722857205374000, -0.62485948814235259000 },
	{ -0.78169483207101853000, -0.62366111752566089000 },
	{ -0.78265059616653476000, -0.62246127937411633000 },
	{ -0.78360451860959723000, -0.62125997651105402000 },
	{ -0.78455659715553416000, -0.62005721176325557000 },
	{ -0.78550682956401285000, -0.61885298796094268000 },
	{ -0.78645521359904458000, -0.61764730793777034000 },
	{ -0.78740174702899013000, -0.61644017453082001000 },
	{ -0.78834642762656493000, -0.61523159058059329000 },
	{ -0.78928925316884424000, -0.61402155893100485000 },
	{ -0.79023022143726851000, -0.61281008242937618000 },
	{ -0.79116933021764857000, -0.61159716392642849000 },
	{ -0.79210657730017064000, -0.61038280627627606000 },
	{ -0.79304196047940190000, -0.60916701233641979000 },
	{ -0.79397547755429532000, -0.60794978496774033000 },
	{ -0.79490712632819505000, -0.60673112703449117000 },
	{ -0.79583690460884149000, -0.60551104140429224000 },
	{ -0.79676481020837664000, -0.60428953094812277000 },
	{ -0.79769084094334886000, -0.60306659854031497000 },
	{ -0.79861499463471841000, -0.60184224705854683000 },
	{ -0.79953726910786260000, -0.60061647938383567000 },
	{ -0.80045766219258019000, -0.59938929840053135000 },
	{ -0.80137617172309750000, -0.59816070699630919000 },
	{ -0.80229279553807298000, -0.59693070806216331000 },
	{ -0.80320753148060209000, -0.59569930449240016000 },
	{ -0.80412037739822284000, -0.59446649918463124000 },
	{ -0.80503133114292058000, -0.59323229503976671000 },
	{ -0.80594039057113320000, -0.59199669496200791000 },
	{ -0.80684755354375604000, -0.59075970185884119000 },
	{ -0.80775281792614706000, -0.58952131864103097000 },
	{ -0.80865618158813168000, -0.58828154822261236000 },
	{ -0.80955764240400785000, -0.58704039352088511000 },
	{ -0.81045719825255136000, -0.58579785745640611000 },
	{ -0.81135484701702021000, -0.58455394295298257000 },
	{ -0.81225058658516036000, -0.58330865293766565000 },
	{ -0.81314441484920996000, -0.58206199034074302000 },
	{ -0.81403632970590478000, -0.58081395809573211000 },
	{ -0.81492632905648299000, -0.57956455913937333000 },
	{ -0.81581441080669004000, -0.57831379641162317000 },
	{ -0.81670057286678388000, -0.57706167285564713000 },
	{ -0.81758481315153964000, -0.57580819141781292000 },
	{ -0.81846712958025458000, -0.57455335504768346000 },
	{ -0.81934752007675271000, -0.57329716669800990000 },
	{ -0.82022598256939039000, -0.57203962932472485000 },
	{ -0.82110251499106035000, -0.57078074588693506000 },
	{ -0.82197711527919715000, -0.56952051934691506000 },
	{ -0.82284978137578180000, -0.56825895267009952000 },
	{ -0.82372051122734669000, -0.56699604882507670000 },
	{ -0.82458930278498055000, -0.56573181078358126000 },
	{ -0.82545615400433281000, -0.56446624152048763000 },
	{ -0.82632106284561868000, -0.56319934401380234000 },
	{ -0.82718402727362428000, -0.56193112124465772000 },
	{ -0.82804504525771094000, -0.56066157619730439000 },
	{ -0.82890411477182002000, -0.55939071185910449000 },
	{ -0.82976123379447808000, -0.55811853122052457000 },
	{ -0.83061640030880124000, -0.55684503727512857000 },
	{ -0.83146961230250005000, -0.55557023301957076000 },
	{ -0.83232086776788439000, -0.55429412145358870000 },
	{ -0.83317016470186789000, -0.55301670557999616000 },
	{ -0.83401750110597273000, -0.55173798840467614000 },
	{ -0.83486287498633460000, -0.55045797293657350000 },
	{ -0.83570628435370709000, -0.54917666218768857000 },
	{ -0.83654772722346638000, -0.54789405917306910000 },
	{ -0.83738720161561619000, -0.54661016691080389000 },
	{ -0.83822470555479223000, -0.54532498842201549000 },
	{ -0.83906023707026678000, -0.54403852673085296000 },
	{ -0.83989379419595356000, -0.54275078486448503000 },
	{ -0.84072537497041211000, -0.54146176585309269000 },
	{ -0.84155497743685237000, -0.54017147272986210000 },
	{ -0.84238259964313977000, -0.53887990853097778000 },
	{ -0.84320823964179925000, -0.53758707629561486000 },
	{ -0.84403189549002022000, -0.53629297906593254000 },
	{ -0.84485356524966071000, -0.53499761988706673000 },
	{ -0.84567324698725255000, -0.53370100180712254000 },
	{ -0.84649093877400550000, -0.53240312787716759000 },
	{ -0.84730663868581169000, -0.53110400115122469000 },
	{ -0.84812034480325049000, -0.52980362468626441000 },
	{ -0.84893205521159276000, -0.52850200154219829000 },
	{ -0.84974176800080548000, -0.52719913478187130000 },
	{ -0.85054948126555630000, -0.52589502747105465000 },
	{ -0.85135519310521801000, -0.52458968267843897000 },
	{ -0.85215890162387264000, -0.52328310347562657000 },
	{ -0.85296060493031634000, -0.52197529293712452000 },
	{ -0.85376030113806400000, -0.52066625414033740000 },
	{ -0.85455798836535302000, -0.51935599016555989000 },
	{ -0.85535366473514851000, -0.51804450409596969000 },
	{ -0.85614732837514695000, -0.51673179901762023000 },
	{ -0.85693897741778113000, -0.51541787801943340000 },
	{ -0.85772861000022438000, -0.51410274419319213000 },
	{ -0.85851622426439500000, -0.51278640063353353000 },
	{ -0.85930181835696051000, -0.51146885043794099000 },
	{ -0.86008539042934218000, -0.51015009670673750000 },
	{ -0.86086693863771924000, -0.50883014254307779000 },
	{ -0.86164646114303323000, -0.50750899105294167000 },
	{ -0.86242395611099232000, -0.50618664534512614000 },
	{ -0.86319942171207587000, -0.50486310853123850000 },
	{ -0.86397285612153840000, -0.50353838372568860000 },
	{ -0.86474425751941397000, -0.50221247404568192000 },
	{ -0.86551362409052057000, -0.50088538261121207000 },
	{ -0.86628095402446437000, -0.49955711254505325000 },
	{ -0.86704624551564391000, -0.49822766697275328000 },
	{ -0.86780949676325436000, -0.49689704902262605000 },
	{ -0.86857070597129193000, -0.49556526182574406000 },
	{ -0.86932987134855777000, -0.49423230851593136000 },
	{ -0.87008699110866239000, -0.49289819222975573000 },
	{ -0.87084206347002979000, -0.49156291610652170000 },
	{ -0.87159508665590180000, -0.49022648328826296000 },
	{ -0.87234605889434225000, -0.48888889691973508000 },
	{ -0.87309497841824080000, -0.48755016014840796000 },
	{ -0.87384184346531746000, -0.48621027612445850000 },
	{ -0.87458665227812671000, -0.48486924800076325000 },
	{ -0.87532940310406138000, -0.48352707893289093000 },
	{ -0.87607009419535709000, -0.48218377207909502000 },
	{ -0.87680872380909614000, -0.48083933060030631000 },
	{ -0.87754529020721173000, -0.47949375766012547000 },
	{ -0.87827979165649184000, -0.47814705642481553000 },
	{ -0.87901222642858379000, -0.47679923006329467000 },
	{ -0.87974259279999767000, -0.47545028174712850000 },
	{ -0.88047088905211091000, -0.47410021465052271000 },
	{ -0.88119711347117213000, -0.47274903195031553000 },
	{ -0.88192126434830498000, -0.47139673682597050000 },
	{ -0.88264333997951261000, -0.47004333245956853000 },
	{ -0.88336333866568129000, -0.46868882203580092000 },
	{ -0.88408125871258469000, -0.46733320874196149000 },
	{ -0.88479709843088739000, -0.46597649576793931000 },
	{ -0.88551085613614944000, -0.46461868630621106000 },
	{ -0.88622253014883001000, -0.46325978355183350000 },
	{ -0.88693211879429157000, -0.46189979070243614000 },
	{ -0.88763962040280320000, -0.46053871095821353000 },
	{ -0.88834503330954551000, -0.45917654752191772000 },
	{ -0.88904835585461373000, -0.45781330359885086000 },
	{ -0.88974958638302182000, -0.45644898239685766000 },
	{ -0.89044872324470681000, -0.45508358712631758000 },
	{ -0.89114576479453211000, -0.45371712100013772000 },
	{ -0.89184070939229154000, -0.45234958723374474000 },
	{ -0.89253355540271340000, -0.45098098904507783000 },
	{ -0.89322430119546403000, -0.44961132965458062000 },
	{ -0.89391294514515185000, -0.44824061228519396000 },
	{ -0.89459948563133129000, -0.44686884016234835000 },
	{ -0.89528392103850607000, -0.44549601651395598000 },
	{ -0.89596624975613359000, -0.44412214457040350000 },
	{ -0.89664647017862853000, -0.44274722756454438000 },
	{ -0.89732458070536658000, -0.44137126873169108000 },
	{ -0.89800057974068803000, -0.43999427130960767000 },
	{ -0.89867446569390197000, -0.43861623853850212000 },
	{ -0.89934623697928961000, -0.43723717366101866000 },
	{ -0.90001589201610821000, -0.43585707992223016000 },
	{ -0.90068342922859479000, -0.43447596056963045000 },
	{ -0.90134884704596985000, -0.43309381885312676000 },
	{ -0.90201214390244089000, -0.43171065802503211000 },
	{ -0.90267331823720653000, -0.43032648134005758000 },
	{ -0.90333236849445941000, -0.42894129205530457000 },
	{ -0.90398929312339082000, -0.42755509343025722000 },
	{ -0.90464409057819362000, -0.42616788872677486000 },
	{ -0.90529675931806619000, -0.42477968120908416000 },
	{ -0.90594729780721583000, -0.42339047414377146000 },
	{ -0.90659570451486260000, -0.42200027079977520000 },
	{ -0.90724197791524308000, -0.42060907444837808000 },
	{ -0.90788611648761341000, -0.41921688836319954000 },
	{ -0.90852811871625327000, -0.41782371582018796000 },
	{ -0.90916798309046942000, -0.41642956009761289000 },
	{ -0.90980570810459926000, -0.41503442447605743000 },
	{ -0.91044129225801418000, -0.41363831223841041000 },
	{ -0.91107473405512318000, -0.41224122666985880000 },
	{ -0.91170603200537670000, -0.41084317105787987000 },
	{ -0.91233518462326957000, -0.40944414869223361000 },
	{ -0.91296219042834492000, -0.40804416286495476000 },
	{ -0.91358704794519752000, -0.40664321687034516000 },
	{ -0.91420975570347729000, -0.40524131400496610000 },
	{ -0.91483031223789268000, -0.40383845756763043000 },
	{ -0.91544871608821432000, -0.40243465085939478000 },
	{ -0.91606496579927810000, -0.40102989718355209000 },
	{ -0.91667905992098897000, -0.39962419984562336000 },
	{ -0.91729099700832417000, -0.39821756215335025000 },
	{ -0.91790077562133665000, -0.39680998741668705000 },
	{ -0.91850839432515829000, -0.39540147894779309000 },
	{ -0.91911385169000381000, -0.39399204006102490000 },
	{ -0.91971714629117340000, -0.39258167407292838000 },
	{ -0.92031827670905664000, -0.39117038430223089000 },
	{ -0.92091724152913546000, -0.38975817406983354000 },
	{ -0.92151403934198783000, -0.38834504669880349000 },
	{ -0.92210866874329089000, -0.38693100551436588000 },
	{ -0.92270112833382434000, -0.38551605384389626000 },
	{ -0.92329141671947346000, -0.38410019501691250000 },
	{ -0.92387953251123245000, -0.38268343236506730000 },
	{ -0.92446547432520820000, -0.38126576922213995000 },
	{ -0.92504924078262318000, -0.37984720892402885000 },
	{ -0.92563083050981831000, -0.37842775480874336000 },
	{ -0.92621024213825676000, -0.37700741021639611000 },
	{ -0.92678747430452724000, -0.37558617848919518000 },
	{ -0.92736252565034649000, -0.37416406297143606000 },
	{ -0.92793539482256326000, -0.37274106700949394000 },
	{ -0.92850608047316097000, -0.37131719395181578000 },
	{ -0.92907458125926112000, -0.36989244714891245000 },
	{ -0.92964089584312659000, -0.36846682995335073000 },
	{ -0.93020502289216433000, -0.36704034571974570000 },
	{ -0.93076696107892898000, -0.36561299780475248000 },
	{ -0.93132670908112569000, -0.36418478956705863000 },
	{ -0.93188426558161319000, -0.36275572436737602000 },
	{ -0.93243962926840740000, -0.36132580556843313000 },
	{ -0.93299279883468378000, -0.35989503653496707000 },
	{ -0.93354377297878111000, -0.35846342063371556000 },
	{ -0.93409255040420369000, -0.35703096123340911000 },
	{ -0.93463912981962549000, -0.35559766170476304000 },
	{ -0.93518350993889221000, -0.35416352542046958000 },
	{ -0.93572568948102497000, -0.35272855575518997000 },
	{ -0.93626566717022286000, -0.35129275608554644000 },
	{ -0.93680344173586605000, -0.34985612979011432000 },
	{ -0.93733901191251934000, -0.34841868024941403000 },
	{ -0.93787237643993426000, -0.34698041084590320000 },
	{ -0.93840353406305255000, -0.34554132496396867000 },
	{ -0.93893248353200887000, -0.34410142598991850000 },
	{ -0.93945922360213419000, -0.34266071731197412000 },
	{ -0.93998375303395820000, -0.34121920232026220000 },
	{ -0.94050607059321245000, -0.33977688440680676000 },
	{ -0.94102617505083330000, -0.33833376696552114000 },
	{ -0.94154406518296474000, -0.33688985339220012000 },
	{ -0.94205973977096125000, -0.33544514708451173000 },
	{ -0.94257319760139069000, -0.33399965144198956000 },
	{ -0.94308443746603721000, -0.33255336986602446000 },
	{ -0.94359345816190399000, -0.33110630575985667000 },
	{ -0.94410025849121626000, -0.32965846252856784000 },
	{ -0.94460483726142386000, -0.32820984357907296000 },
	{ -0.94510719328520409000, -0.32676045232011225000 },
	{ -0.94560732538046477000, -0.32531029216224350000 },
	{ -0.94610523237034672000, -0.32385936651783354000 },
	{ -0.94660091308322680000, -0.32240767880105059000 },
	{ -0.94709436635272037000, -0.32095523242785606000 },
	{ -0.94758559101768425000, -0.31950203081599660000 },
	{ -0.94807458592221927000, -0.31804807738499596000 },
	{ -0.94856134991567320000, -0.31659337555614697000 },
	{ -0.94904588185264338000, -0.31513792875250357000 },
	{ -0.94952818059297939000, -0.31368174039887270000 },
	{ -0.95000824500178571000, -0.31222481392180623000 },
	{ -0.95048607394942430000, -0.31076715274959288000 },
	{ -0.95096166631151757000, -0.30930876031225019000 },
	{ -0.95143502096895072000, -0.30784964004151644000 },
	{ -0.95190613680787461000, -0.30638979537084254000 },
	{ -0.95237501271970815000, -0.30492922973538406000 },
	{ -0.95284164760114087000, -0.30346794657199300000 },
	{ -0.95330604035413591000, -0.30200594931920982000 },
	{ -0.95376818988593226000, -0.30054324141725530000 },
	{ -0.95422809510904760000, -0.29907982630802238000 },
	{ -0.95468575494128027000, -0.29761570743506821000 },
	{ -0.95514116830571261000, -0.29615088824360586000 },
	{ -0.95559433413071282000, -0.29468537218049651000 },
	{ -0.95604525134993812000, -0.29321916269424086000 },
	{ -0.95649391890233670000, -0.29175226323497155000 },
	{ -0.95694033573215043000, -0.29028467725444473000 },
	{ -0.95738450078891735000, -0.28881640820603188000 },
	{ -0.95782641302747418000, -0.28734745954471203000 },
	{ -0.95826607140795894000, -0.28587783472706313000 },
	{ -0.95870347489581287000, -0.28440753721125445000 },
	{ -0.95913862246178327000, -0.28293657045703802000 },
	{ -0.95957151308192590000, -0.28146493792574073000 },
	{ -0.96000214573760723000, -0.27999264308025607000 },
	{ -0.96043051941550706000, -0.27851968938503602000 },
	{ -0.96085663310762082000, -0.27704608030608291000 },
	{ -0.96128048581126180000, -0.27557181931094121000 },
	{ -0.96170207652906359000, -0.27409690986868956000 },
	{ -0.96212140426898263000, -0.27262135544993227000 },
	{ -0.96253846804430010000, -0.27114515952679136000 },
	{ -0.96295326687362470000, -0.26966832557289849000 },
	{ -0.96336579978089476000, -0.26819085706338663000 },
	{ -0.96377606579538055000, -0.26671275747488188000 },
	{ -0.96418406395168643000, -0.26523403028549536000 },
	{ -0.96458979328975325000, -0.26375467897481503000 },
	{ -0.96499325285486082000, -0.26227470702389738000 },
	{ -0.96539444169762978000, -0.26079411791525936000 },
	{ -0.96579335887402395000, -0.25931291513287019000 },
	{ -0.96619000344535277000, -0.25783110216214306000 },
	{ -0.96658437447827328000, -0.25634868248992698000 },
	{ -0.96697647104479223000, -0.25486565960449870000 },
	{ -0.96736629222226855000, -0.25338203699555439000 },
	{ -0.96775383709341534000, -0.25189781815420131000 },
	{ -0.96813910474630227000, -0.25041300657294974000 },
	{ -0.96852209427435709000, -0.24892760574570472000 },
	{ -0.96890280477636859000, -0.24744161916775795000 },
	{ -0.96928123535648814000, -0.24595505033577936000 },
	{ -0.96965738512423205000, -0.24446790274780900000 },
	{ -0.97003125319448358000, -0.24298017990324880000 },
	{ -0.97040283868749500000, -0.24149188530285434000 },
	{ -0.97077214072888973000, -0.24000302244872657000 },
	{ -0.97113915844966459000, -0.23851359484430359000 },
	{ -0.97150389098619117000, -0.23702360599435246000 },
	{ -0.97186633748021878000, -0.23553305940496083000 },
	{ -0.97222649707887565000, -0.23404195858352886000 },
	{ -0.97258436893467148000, -0.23255030703876076000 },
	{ -0.97293995220549923000, -0.23105810828065673000 },
	{ -0.97329324605463730000, -0.22956536582050460000 },
	{ -0.97364424965075103000, -0.22807208317087155000 },
	{ -0.97399296216789488000, -0.22657826384559590000 },
	{ -0.97433938278551491000, -0.22508391135977882000 },
	{ -0.97468351068844972000, -0.22358902922977608000 },
	{ -0.97502534506693306000, -0.22209362097318971000 },
	{ -0.97536488511659580000, -0.22059769010885980000 },
	{ -0.97570213003846740000, -0.21910124015685614000 },
	{ -0.97603707903897796000, -0.21760427463847007000 },
	{ -0.97636973132995997000, -0.21610679707620603000 },
	{ -0.97670008612865056000, -0.21460881099377338000 },
	{ -0.97702814265769311000, -0.21311031991607807000 },
	{ -0.97735390014513879000, -0.21161132736921434000 },
	{ -0.97767735782444876000, -0.21011183688045648000 },
	{ -0.97799851493449586000, -0.20861185197825044000 },
	{ -0.97831737071956637000, -0.20711137619220560000 },
	{ -0.97863392442936181000, -0.20561041305308642000 },
	{ -0.97894817531900080000, -0.20410896609280410000 },
	{ -0.97926012264902063000, -0.20260703884440842000 },
	{ -0.97956976568537901000, -0.20110463484207927000 },
	{ -0.97987710369945602000, -0.19960175762111843000 },
	{ -0.98018213596805570000, -0.19809841071794113000 },
	{ -0.98048486177340755000, -0.19659459767006787000 },
	{ -0.98078528040316870000, -0.19509032201611601000 },
	{ -0.98108339115042487000, -0.19358558729579145000 },
	{ -0.98137919331369272000, -0.19208039704988036000 },
	{ -0.98167268619692127000, -0.19057475482024078000 },
	{ -0.98196386910949329000, -0.18906866414979431000 },
	{ -0.98225274136622742000, -0.18756212858251781000 },
	{ -0.98253930228737929000, -0.18605515166343495000 },
	{ -0.98282355119864317000, -0.18454773693860799000 },
	{ -0.98310548743115422000, -0.18303988795512940000 },
	{ -0.98338511032148901000, -0.18153160826111350000 },
	{ -0.98366241921166797000, -0.18002290140568811000 },
	{ -0.98393741344915664000, -0.17851377093898618000 },
	{ -0.98421009238686674000, -0.17700422041213751000 },
	{ -0.98448045538315854000, -0.17549425337726027000 },
	{ -0.98474850180184181000, -0.17398387338745275000 },
	{ -0.98501423101217744000, -0.17247308399678499000 },
	{ -0.98527764238887883000, -0.17096188876029034000 },
	{ -0.98553873531211356000, -0.16945029123395716000 },
	{ -0.98579750916750486000, -0.16793829497472046000 },
	{ -0.98605396334613282000, -0.16642590354045347000 },
	{ -0.98630809724453594000, -0.16491312048995935000 },
	{ -0.98655991026471268000, -0.16339994938296276000 },
	{ -0.98680940181412269000, -0.16188639378010147000 },
	{ -0.98705657130568825000, -0.16037245724291799000 },
	{ -0.98730141815779549000, -0.15885814333385126000 },
	{ -0.98754394179429628000, -0.15734345561622815000 },
	{ -0.98778414164450912000, -0.15582839765425521000 },
	{ -0.98802201714322047000, -0.15431297301301017000 },
	{ -0.98825756773068640000, -0.15279718525843358000 },
	{ -0.98849079285263353000, -0.15128103795732045000 },
	{ -0.98872169196026061000, -0.14976453467731185000 },
	{ -0.98895026451023971000, -0.14824767898688646000 },
	{ -0.98917650996471773000, -0.14673047445535226000 },
	{ -0.98940042779131709000, -0.14521292465283805000 },
	{ -0.98962201746313749000, -0.14369503315028515000 },
	{ -0.98984127845875713000, -0.14217680351943882000 },
	{ -0.99005821026223373000, -0.14065823933284008000 },
	{ -0.99027281236310571000, -0.13913934416381715000 },
	{ -0.99048508425639359000, -0.13762012158647707000 },
	{ -0.99069502544260113000, -0.13610057517569732000 },
	{ -0.99090263542771639000, -0.13458070850711740000 },
	{ -0.99110791372321316000, -0.13306052515713038000 },
	{ -0.99131085984605172000, -0.13154002870287451000 },
	{ -0.99151147331868017000, -0.13001922272222485000 },
	{ -0.99170975366903569000, -0.12849811079378476000 },
	{ -0.99190570043054549000, -0.12697669649687754000 },
	{ -0.99209931314212785000, -0.12545498341153802000 },
	{ -0.99229059134819342000, -0.12393297511850404000 },
	{ -0.99247953459864602000, -0.12241067519920816000 },
	{ -0.99266614244888396000, -0.12088808723576913000 },
	{ -0.99285041445980093000, -0.11936521481098351000 },
	{ -0.99303235019778713000, -0.11784206150831722000 },
	{ -0.99321194923473033000, -0.11631863091189711000 },
	{ -0.99338921114801637000, -0.11479492660650252000 },
	{ -0.99356413552053102000, -0.11327095217755688000 },
	{ -0.99373672194066032000, -0.11174671121111922000 },
	{ -0.99390697000229167000, -0.11022220729387577000 },
	{ -0.99407487930481497000, -0.10869744401313153000 },
	{ -0.99424044945312340000, -0.10717242495680177000 },
	{ -0.99440368005761448000, -0.10564715371340364000 },
	{ -0.99456457073419080000, -0.10412163387204770000 },
	{ -0.99472312110426109000, -0.10259586902242951000 },
	{ -0.99487933079474100000, -0.10106986275482115000 },
	{ -0.99503319943805391000, -0.09954361866006275500 },
	{ -0.99518472667213220000, -0.09801714032955412300 },
	{ -0.99533391214041755000, -0.09649043135524620900 },
	{ -0.99548075549186221000, -0.09496349532963270500 },
	{ -0.99562525638092947000, -0.09343633584574158300 },
	{ -0.99576741446759498000, -0.09190895649712663200 },
	{ -0.99590722941734688000, -0.09038136087785898800 },
	{ -0.99604470090118713000, -0.08885355258251870200 },
	{ -0.99617982859563203000, -0.08732553520618627200 },
	{ -0.99631261218271305000, -0.08579731234443419000 },
	{ -0.99644305134997768000, -0.08426888759331846400 },
	{ -0.99657114579048978000, -0.08274026454937018200 },
	{ -0.99669689520283100000, -0.08121144680958702900 },
	{ -0.99682029929110061000, -0.07968243797142481000 },
	{ -0.99694135776491699000, -0.07815324163278902800 },
	{ -0.99706007033941779000, -0.07662386139202638500 },
	{ -0.99717643673526091000, -0.07509430084791630900 },
	{ -0.99729045667862493000, -0.07356456359966251400 },
	{ -0.99740212990121002000, -0.07203465324688451700 },
	{ -0.99751145614023806000, -0.07050457338960916500 },
	{ -0.99761843513845405000, -0.06897432762826213900 },
	{ -0.99772306664412602000, -0.06744391956365955500 },
	{ -0.99782535041104603000, -0.06591335279699941900 },
	{ -0.99792528619853038000, -0.06438263092985316300 },
	{ -0.99802287377142052000, -0.06285175756415721500 },
	{ -0.99811811290008345000, -0.06132073630220447700 },
	{ -0.99821100336041246000, -0.05978957074663587100 },
	{ -0.99830154493382717000, -0.05825826450043185300 },
	{ -0.99838973740727444000, -0.05672682116690394600 },
	{ -0.99847558057322905000, -0.05519524434968622900 },
	{ -0.99855907422969359000, -0.05366353765272691100 },
	{ -0.99864021818019943000, -0.05213170468027980600 },
	{ -0.99871901223380710000, -0.05059974903689586800 },
	{ -0.99879545620510657000, -0.04906767432741469800 },
	{ -0.99886954991421772000, -0.04753548415695609000 },
	{ -0.99894129318679092000, -0.04600318213091151400 },
	{ -0.99901068585400743000, -0.04447077185493565600 },
	{ -0.99907772775257953000, -0.04293825693493791300 },
	{ -0.99914241872475096000, -0.04140564097707392900 },
	{ -0.99920475861829783000, -0.03987292758773709800 },
	{ -0.99926474728652837000, -0.03834012037355007800 },
	{ -0.99932238458828337000, -0.03680722294135632000 },
	{ -0.99937767038793668000, -0.03527423889821153900 },
	{ -0.99943060455539556000, -0.03374117185137526900 },
	{ -0.99948118696610067000, -0.03220802540830236500 },
	{ -0.99952941750102686000, -0.03067480317663451000 },
	{ -0.99957529604668294000, -0.02914150876419170600 },
	{ -0.99961882249511225000, -0.02760814577896382500 },
	{ -0.99965999674389283000, -0.02607471782910208300 },
	{ -0.99969881869613775000, -0.02454122852291057100 },
	{ -0.99973528826049518000, -0.02300768146883775900 },
	{ -0.99976940535114867000, -0.02147408027546799500 },
	{ -0.99980116988781753000, -0.01994042855151302900 },
	{ -0.99983058179575668000, -0.01840672990580351200 },
	{ -0.99985764100575703000, -0.01687298794728050700 },
	{ -0.99988234745414573000, -0.01533920628498699500 },
	{ -0.99990470108278606000, -0.01380538852805938800 },
	{ -0.99992470183907767000, -0.01227153828571902500 },
	{ -0.99994234967595697000, -0.01073765916726369300 },
	{ -0.99995764455189695000, -0.00920375478205912380 },
	{ -0.99997058643090719000, -0.00766982873953050550 },
	{ -0.99998117528253427000, -0.00613588464915398610 },
	{ -0.99998941108186146000, -0.00460192612044818450 },
	{ -0.99999529380950924000, -0.00306795676296569290 },
	{ -0.99999882345163493000, -0.00153398018628458490 },
	{ -0.99999999999993294000, +0.00000000000000007785 },
};
#endif /* FFTSizeFilters == 4096 */
#if FFTSizeFilters == 4096
/* FFT/IFFT w coefficients tables for 4096 elements */
static const FLASHMEM struct Complex wp [2048] = 
{
	{ +0.99999882345170188000, +0.00153398018628476550 },
	{ +0.99999529380957608000, +0.00306795676296597610 },
	{ +0.99998941108192818000, +0.00460192612044857050 },
	{ +0.99998117528260089000, +0.00613588464915447440 },
	{ +0.99997058643097381000, +0.00766982873953109610 },
	{ +0.99995764455196356000, +0.00920375478205981770 },
	{ +0.99994234967602358000, +0.01073765916726449100 },
	{ +0.99992470183914417000, +0.01227153828571992400 },
	{ +0.99990470108285245000, +0.01380538852806038700 },
	{ +0.99988234745421212000, +0.01533920628498809700 },
	{ +0.99985764100582342000, +0.01687298794728171000 },
	{ +0.99983058179582296000, +0.01840672990580481700 },
	{ +0.99980116988788370000, +0.01994042855151443400 },
	{ +0.99976940535121472000, +0.02147408027546950100 },
	{ +0.99973528826056113000, +0.02300768146883936500 },
	{ +0.99969881869620358000, +0.02454122852291228100 },
	{ +0.99965999674395856000, +0.02607471782910389000 },
	{ +0.99961882249517786000, +0.02760814577896572900 },
	{ +0.99957529604674844000, +0.02914150876419371200 },
	{ +0.99952941750109237000, +0.03067480317663661200 },
	{ +0.99948118696616606000, +0.03220802540830456500 },
	{ +0.99943060455546084000, +0.03374117185137756600 },
	{ +0.99937767038800196000, +0.03527423889821392600 },
	{ +0.99932238458834854000, +0.03680722294135880400 },
	{ +0.99926474728659354000, +0.03834012037355266600 },
	{ +0.99920475861836300000, +0.03987292758773978300 },
	{ +0.99914241872481602000, +0.04140564097707670500 },
	{ +0.99907772775264447000, +0.04293825693494078600 },
	{ +0.99901068585407238000, +0.04447077185493862600 },
	{ +0.99894129318685587000, +0.04600318213091458800 },
	{ +0.99886954991428256000, +0.04753548415695926800 },
	{ +0.99879545620517129000, +0.04906767432741797300 },
	{ +0.99871901223387172000, +0.05059974903689923300 },
	{ +0.99864021818026394000, +0.05213170468028326800 },
	{ +0.99855907422975809000, +0.05366353765273047100 },
	{ +0.99847558057329355000, +0.05519524434968988600 },
	{ +0.99838973740733894000, +0.05672682116690768600 },
	{ +0.99830154493389156000, +0.05825826450043569000 },
	{ +0.99821100336047686000, +0.05978957074663980600 },
	{ +0.99811811290014785000, +0.06132073630220850200 },
	{ +0.99802287377148480000, +0.06285175756416133700 },
	{ +0.99792528619859455000, +0.06438263092985738200 },
	{ +0.99782535041111009000, +0.06591335279700372100 },
	{ +0.99772306664418997000, +0.06744391956366395400 },
	{ +0.99761843513851789000, +0.06897432762826663500 },
	{ +0.99751145614030179000, +0.07050457338961375900 },
	{ +0.99740212990127353000, +0.07203465324688920800 },
	{ +0.99729045667868843000, +0.07356456359966730100 },
	{ +0.99717643673532430000, +0.07509430084792118000 },
	{ +0.99706007033948107000, +0.07662386139203133900 },
	{ +0.99694135776498016000, +0.07815324163279409300 },
	{ +0.99682029929116367000, +0.07968243797142997300 },
	{ +0.99669689520289395000, +0.08121144680959228900 },
	{ +0.99657114579055273000, +0.08274026454937553900 },
	{ +0.99644305135004052000, +0.08426888759332391800 },
	{ +0.99631261218277589000, +0.08579731234443972700 },
	{ +0.99617982859569476000, +0.08732553520619190700 },
	{ +0.99604470090124975000, +0.08885355258252443400 },
	{ +0.99590722941740939000, +0.09038136087786480300 },
	{ +0.99576741446765737000, +0.09190895649713254300 },
	{ +0.99562525638099186000, +0.09343633584574759200 },
	{ +0.99548075549192450000, +0.09496349532963879700 },
	{ +0.99533391214047984000, +0.09649043135525238500 },
	{ +0.99518472667219449000, +0.09801714032956038200 },
	{ +0.99503319943811608000, +0.09954361866006911100 },
	{ +0.99487933079480306000, +0.10106986275482760000 },
	{ +0.99472312110432315000, +0.10259586902243606000 },
	{ +0.99456457073425286000, +0.10412163387205434000 },
	{ +0.99440368005767643000, +0.10564715371341037000 },
	{ +0.99424044945318535000, +0.10717242495680859000 },
	{ +0.99407487930487670000, +0.10869744401313845000 },
	{ +0.99390697000235328000, +0.11022220729388280000 },
	{ +0.99373672194072182000, +0.11174671121112632000 },
	{ +0.99356413552059253000, +0.11327095217756407000 },
	{ +0.99338921114807777000, +0.11479492660650979000 },
	{ +0.99321194923479172000, +0.11631863091190447000 },
	{ +0.99303235019784852000, +0.11784206150832467000 },
	{ +0.99285041445986222000, +0.11936521481099105000 },
	{ +0.99266614244894513000, +0.12088808723577675000 },
	{ +0.99247953459870708000, +0.12241067519921586000 },
	{ +0.99229059134825437000, +0.12393297511851183000 },
	{ +0.99209931314218869000, +0.12545498341154587000 },
	{ +0.99190570043060622000, +0.12697669649688548000 },
	{ +0.99170975366909631000, +0.12849811079379278000 },
	{ +0.99151147331874079000, +0.13001922272223293000 },
	{ +0.99131085984611222000, +0.13154002870288267000 },
	{ +0.99110791372327367000, +0.13306052515713862000 },
	{ +0.99090263542777679000, +0.13458070850712572000 },
	{ +0.99069502544266130000, +0.13610057517570573000 },
	{ +0.99048508425645365000, +0.13762012158648557000 },
	{ +0.99027281236316567000, +0.13913934416382573000 },
	{ +0.99005821026229357000, +0.14065823933284874000 },
	{ +0.98984127845881698000, +0.14217680351944756000 },
	{ +0.98962201746319722000, +0.14369503315029394000 },
	{ +0.98940042779137671000, +0.14521292465284694000 },
	{ +0.98917650996477735000, +0.14673047445536122000 },
	{ +0.98895026451029933000, +0.14824767898689548000 },
	{ +0.98872169196032011000, +0.14976453467732093000 },
	{ +0.98849079285269292000, +0.15128103795732961000 },
	{ +0.98825756773074580000, +0.15279718525844282000 },
	{ +0.98802201714327986000, +0.15431297301301949000 },
	{ +0.98778414164456840000, +0.15582839765426459000 },
	{ +0.98754394179435556000, +0.15734345561623761000 },
	{ +0.98730141815785466000, +0.15885814333386078000 },
	{ +0.98705657130574720000, +0.16037245724292759000 },
	{ +0.98680940181418164000, +0.16188639378011116000 },
	{ +0.98655991026477163000, +0.16339994938297253000 },
	{ +0.98630809724459489000, +0.16491312048996923000 },
	{ +0.98605396334619166000, +0.16642590354046341000 },
	{ +0.98579750916756359000, +0.16793829497473045000 },
	{ +0.98553873531217229000, +0.16945029123396721000 },
	{ +0.98527764238893745000, +0.17096188876030047000 },
	{ +0.98501423101223595000, +0.17247308399679520000 },
	{ +0.98474850180190032000, +0.17398387338746304000 },
	{ +0.98448045538321705000, +0.17549425337727062000 },
	{ +0.98421009238692514000, +0.17700422041214792000 },
	{ +0.98393741344921493000, +0.17851377093899667000 },
	{ +0.98366241921172626000, +0.18002290140569865000 },
	{ +0.98338511032154718000, +0.18153160826112411000 },
	{ +0.98310548743121229000, +0.18303988795514006000 },
	{ +0.98282355119870124000, +0.18454773693861870000 },
	{ +0.98253930228743724000, +0.18605515166344572000 },
	{ +0.98225274136628526000, +0.18756212858252866000 },
	{ +0.98196386910955114000, +0.18906866414980528000 },
	{ +0.98167268619697901000, +0.19057475482025182000 },
	{ +0.98137919331375034000, +0.19208039704989149000 },
	{ +0.98108339115048238000, +0.19358558729580266000 },
	{ +0.98078528040322610000, +0.19509032201612730000 },
	{ +0.98048486177346506000, +0.19659459767007925000 },
	{ +0.98018213596811310000, +0.19809841071795259000 },
	{ +0.97987710369951331000, +0.19960175762112997000 },
	{ +0.97956976568543619000, +0.20110463484209090000 },
	{ +0.97926012264907769000, +0.20260703884442011000 },
	{ +0.97894817531905776000, +0.20410896609281584000 },
	{ +0.97863392442941866000, +0.20561041305309821000 },
	{ +0.97831737071962310000, +0.20711137619221748000 },
	{ +0.97799851493455259000, +0.20861185197826243000 },
	{ +0.97767735782450549000, +0.21011183688046856000 },
	{ +0.97735390014519552000, +0.21161132736922650000 },
	{ +0.97702814265774984000, +0.21311031991609028000 },
	{ +0.97670008612870729000, +0.21460881099378568000 },
	{ +0.97636973133001659000, +0.21610679707621841000 },
	{ +0.97603707903903447000, +0.21760427463848250000 },
	{ +0.97570213003852391000, +0.21910124015686866000 },
	{ +0.97536488511665231000, +0.22059769010887240000 },
	{ +0.97502534506698957000, +0.22209362097320240000 },
	{ +0.97468351068850612000, +0.22358902922978885000 },
	{ +0.97433938278557120000, +0.22508391135979167000 },
	{ +0.97399296216795117000, +0.22657826384560883000 },
	{ +0.97364424965080720000, +0.22807208317088457000 },
	{ +0.97329324605469347000, +0.22956536582051770000 },
	{ +0.97293995220555540000, +0.23105810828066992000 },
	{ +0.97258436893472755000, +0.23255030703877402000 },
	{ +0.97222649707893150000, +0.23404195858354221000 },
	{ +0.97186633748027451000, +0.23553305940497429000 },
	{ +0.97150389098624690000, +0.23702360599436600000 },
	{ +0.97113915844972021000, +0.23851359484431719000 },
	{ +0.97077214072894535000, +0.24000302244874025000 },
	{ +0.97040283868755051000, +0.24149188530286811000 },
	{ +0.97003125319453898000, +0.24298017990326262000 },
	{ +0.96965738512428745000, +0.24446790274782287000 },
	{ +0.96928123535654342000, +0.24595505033579329000 },
	{ +0.96890280477642388000, +0.24744161916777194000 },
	{ +0.96852209427441227000, +0.24892760574571879000 },
	{ +0.96813910474635734000, +0.25041300657296389000 },
	{ +0.96775383709347040000, +0.25189781815421558000 },
	{ +0.96736629222232351000, +0.25338203699556877000 },
	{ +0.96697647104484696000, +0.25486565960451318000 },
	{ +0.96658437447832790000, +0.25634868248994153000 },
	{ +0.96619000344540729000, +0.25783110216215760000 },
	{ +0.96579335887407836000, +0.25931291513288479000 },
	{ +0.96539444169768407000, +0.26079411791527407000 },
	{ +0.96499325285491500000, +0.26227470702391220000 },
	{ +0.96458979328980732000, +0.26375467897482996000 },
	{ +0.96418406395174039000, +0.26523403028551040000 },
	{ +0.96377606579543440000, +0.26671275747489698000 },
	{ +0.96336579978094861000, +0.26819085706340173000 },
	{ +0.96295326687367844000, +0.26966832557291359000 },
	{ +0.96253846804435372000, +0.27114515952680646000 },
	{ +0.96212140426903614000, +0.27262135544994737000 },
	{ +0.96170207652911699000, +0.27409690986870472000 },
	{ +0.96128048581131509000, +0.27557181931095648000 },
	{ +0.96085663310767400000, +0.27704608030609823000 },
	{ +0.96043051941556012000, +0.27851968938505139000 },
	{ +0.96000214573766018000, +0.27999264308027150000 },
	{ +0.95957151308197874000, +0.28146493792575622000 },
	{ +0.95913862246183612000, +0.28293657045705356000 },
	{ +0.95870347489586571000, +0.28440753721127005000 },
	{ +0.95826607140801179000, +0.28587783472707878000 },
	{ +0.95782641302752702000, +0.28734745954472768000 },
	{ +0.95738450078897008000, +0.28881640820604759000 },
	{ +0.95694033573220294000, +0.29028467725446050000 },
	{ +0.95649391890238911000, +0.29175226323498737000 },
	{ +0.95604525134999041000, +0.29321916269425674000 },
	{ +0.95559433413076500000, +0.29468537218051238000 },
	{ +0.95514116830576457000, +0.29615088824362185000 },
	{ +0.95468575494133212000, +0.29761570743508425000 },
	{ +0.95422809510909934000, +0.29907982630803848000 },
	{ +0.95376818988598400000, +0.30054324141727146000 },
	{ +0.95330604035418753000, +0.30200594931922603000 },
	{ +0.95284164760119228000, +0.30346794657200926000 },
	{ +0.95237501271975944000, +0.30492922973540032000 },
	{ +0.95190613680792591000, +0.30638979537085881000 },
	{ +0.95143502096900190000, +0.30784964004153270000 },
	{ +0.95096166631156853000, +0.30930876031226651000 },
	{ +0.95048607394947504000, +0.31076715274960925000 },
	{ +0.95000824500183634000, +0.31222481392182266000 },
	{ +0.94952818059302990000, +0.31368174039888919000 },
	{ +0.94904588185269378000, +0.31513792875252011000 },
	{ +0.94856134991572350000, +0.31659337555616351000 },
	{ +0.94807458592226934000, +0.31804807738501256000 },
	{ +0.94758559101773421000, +0.31950203081601325000 },
	{ +0.94709436635277022000, +0.32095523242787283000 },
	{ +0.94660091308327654000, +0.32240767880106741000 },
	{ +0.94610523237039634000, +0.32385936651785041000 },
	{ +0.94560732538051417000, +0.32531029216226043000 },
	{ +0.94510719328525339000, +0.32676045232012929000 },
	{ +0.94460483726147304000, +0.32820984357909000000 },
	{ +0.94410025849126544000, +0.32965846252858494000 },
	{ +0.94359345816195317000, +0.33110630575987382000 },
	{ +0.94308443746608628000, +0.33255336986604161000 },
	{ +0.94257319760143965000, +0.33399965144200672000 },
	{ +0.94205973977101010000, +0.33544514708452888000 },
	{ +0.94154406518301348000, +0.33688985339221728000 },
	{ +0.94102617505088193000, +0.33833376696553835000 },
	{ +0.94050607059326097000, +0.33977688440682402000 },
	{ +0.93998375303400661000, +0.34121920232027952000 },
	{ +0.93945922360218248000, +0.34266071731199149000 },
	{ +0.93893248353205716000, +0.34410142598993593000 },
	{ +0.93840353406310073000, +0.34554132496398610000 },
	{ +0.93787237643998245000, +0.34698041084592068000 },
	{ +0.93733901191256752000, +0.34841868024943157000 },
	{ +0.93680344173591423000, +0.34985612979013192000 },
	{ +0.93626566717027093000, +0.35129275608556410000 },
	{ +0.93572568948107293000, +0.35272855575520767000 },
	{ +0.93518350993894017000, +0.35416352542048735000 },
	{ +0.93463912981967334000, +0.35559766170478080000 },
	{ +0.93409255040425143000, +0.35703096123342687000 },
	{ +0.93354377297882873000, +0.35846342063373338000 },
	{ +0.93299279883473141000, +0.35989503653498495000 },
	{ +0.93243962926845492000, +0.36132580556845106000 },
	{ +0.93188426558166060000, +0.36275572436739395000 },
	{ +0.93132670908117288000, +0.36418478956707662000 },
	{ +0.93076696107897605000, +0.36561299780477058000 },
	{ +0.93020502289221141000, +0.36704034571976391000 },
	{ +0.92964089584317355000, +0.36846682995336905000 },
	{ +0.92907458125930809000, +0.36989244714893083000 },
	{ +0.92850608047320782000, +0.37131719395183421000 },
	{ +0.92793539482261012000, +0.37274106700951243000 },
	{ +0.92736252565039334000, +0.37416406297145460000 },
	{ +0.92678747430457409000, +0.37558617848921383000 },
	{ +0.92621024213830361000, +0.37700741021641487000 },
	{ +0.92563083050986505000, +0.37842775480876223000 },
	{ +0.92504924078266981000, +0.37984720892404777000 },
	{ +0.92446547432525483000, +0.38126576922215899000 },
	{ +0.92387953251127897000, +0.38268343236508640000 },
	{ +0.92329141671951975000, +0.38410019501693166000 },
	{ +0.92270112833387063000, +0.38551605384391546000 },
	{ +0.92210866874333719000, +0.38693100551438514000 },
	{ +0.92151403934203402000, +0.38834504669882286000 },
	{ +0.92091724152918153000, +0.38975817406985303000 },
	{ +0.92031827670910260000, +0.39117038430225048000 },
	{ +0.91971714629121937000, +0.39258167407294808000 },
	{ +0.91911385169004978000, +0.39399204006104471000 },
	{ +0.91850839432520426000, +0.39540147894781291000 },
	{ +0.91790077562138250000, +0.39680998741670687000 },
	{ +0.91729099700836991000, +0.39821756215337012000 },
	{ +0.91667905992103460000, +0.39962419984564329000 },
	{ +0.91606496579932362000, +0.40102989718357213000 },
	{ +0.91544871608825973000, +0.40243465085941493000 },
	{ +0.91483031223793798000, +0.40383845756765058000 },
	{ +0.91420975570352248000, +0.40524131400498631000 },
	{ +0.91358704794524259000, +0.40664321687036542000 },
	{ +0.91296219042838989000, +0.40804416286497508000 },
	{ +0.91233518462331442000, +0.40944414869225398000 },
	{ +0.91170603200542155000, +0.41084317105790030000 },
	{ +0.91107473405516803000, +0.41224122666987922000 },
	{ +0.91044129225805892000, +0.41363831223843084000 },
	{ +0.90980570810464390000, +0.41503442447607786000 },
	{ +0.90916798309051405000, +0.41642956009763338000 },
	{ +0.90852811871629779000, +0.41782371582020850000 },
	{ +0.90788611648765782000, +0.41921688836322007000 },
	{ +0.90724197791528738000, +0.42060907444839862000 },
	{ +0.90659570451490690000, +0.42200027079979574000 },
	{ +0.90594729780726002000, +0.42339047414379205000 },
	{ +0.90529675931811027000, +0.42477968120910481000 },
	{ +0.90464409057823769000, +0.42616788872679562000 },
	{ +0.90398929312343479000, +0.42755509343027809000 },
	{ +0.90333236849450327000, +0.42894129205532550000 },
	{ +0.90267331823725017000, +0.43032648134007856000 },
	{ +0.90201214390248441000, +0.43171065802505321000 },
	{ +0.90134884704601326000, +0.43309381885314790000 },
	{ +0.90068342922863809000, +0.43447596056965160000 },
	{ +0.90001589201615140000, +0.43585707992225137000 },
	{ +0.89934623697933269000, +0.43723717366103992000 },
	{ +0.89867446569394493000, +0.43861623853852344000 },
	{ +0.89800057974073089000, +0.43999427130962904000 },
	{ +0.89732458070540932000, +0.44137126873171245000 },
	{ +0.89664647017867116000, +0.44274722756456575000 },
	{ +0.89596624975617611000, +0.44412214457042487000 },
	{ +0.89528392103854848000, +0.44549601651397736000 },
	{ +0.89459948563137359000, +0.44686884016236977000 },
	{ +0.89391294514519415000, +0.44824061228521545000 },
	{ +0.89322430119550622000, +0.44961132965460210000 },
	{ +0.89253355540275547000, +0.45098098904509931000 },
	{ +0.89184070939233351000, +0.45234958723376628000 },
	{ +0.89114576479457397000, +0.45371712100015926000 },
	{ +0.89044872324474855000, +0.45508358712633917000 },
	{ +0.88974958638306345000, +0.45644898239687925000 },
	{ +0.88904835585465514000, +0.45781330359887251000 },
	{ +0.88834503330958681000, +0.45917654752193943000 },
	{ +0.88763962040284439000, +0.46053871095823529000 },
	{ +0.88693211879433254000, +0.46189979070245801000 },
	{ +0.88622253014887087000, +0.46325978355185549000 },
	{ +0.88551085613619018000, +0.46461868630623310000 },
	{ +0.88479709843092802000, +0.46597649576796141000 },
	{ +0.88408125871262522000, +0.46733320874198364000 },
	{ +0.88336333866572181000, +0.46868882203582313000 },
	{ +0.88264333997955302000, +0.47004333245959079000 },
	{ +0.88192126434834517000, +0.47139673682599276000 },
	{ +0.88119711347121210000, +0.47274903195033791000 },
	{ +0.88047088905215076000, +0.47410021465054514000 },
	{ +0.87974259280003742000, +0.47545028174715098000 },
	{ +0.87901222642862342000, +0.47679923006331720000 },
	{ +0.87827979165653147000, +0.47814705642483812000 },
	{ +0.87754529020725125000, +0.47949375766014807000 },
	{ +0.87680872380913566000, +0.48083933060032896000 },
	{ +0.87607009419539661000, +0.48218377207911772000 },
	{ +0.87532940310410090000, +0.48352707893291369000 },
	{ +0.87458665227816612000, +0.48486924800078607000 },
	{ +0.87384184346535676000, +0.48621027612448131000 },
	{ +0.87309497841827999000, +0.48755016014843083000 },
	{ +0.87234605889438133000, +0.48888889691975801000 },
	{ +0.87159508665594088000, +0.49022648328828600000 },
	{ +0.87084206347006876000, +0.49156291610654479000 },
	{ +0.87008699110870125000, +0.49289819222977882000 },
	{ +0.86932987134859663000, +0.49423230851595451000 },
	{ +0.86857070597133079000, +0.49556526182576732000 },
	{ +0.86780949676329322000, +0.49689704902264931000 },
	{ +0.86704624551568266000, +0.49822766697277659000 },
	{ +0.86628095402450289000, +0.49955711254507662000 },
	{ +0.86551362409055899000, +0.50088538261123550000 },
	{ +0.86474425751945228000, +0.50221247404570546000 },
	{ +0.86397285612157659000, +0.50353838372571225000 },
	{ +0.86319942171211395000, +0.50486310853126226000 },
	{ +0.86242395611103029000, +0.50618664534515001000 },
	{ +0.86164646114307109000, +0.50750899105296554000 },
	{ +0.86086693863775698000, +0.50883014254310166000 },
	{ +0.86008539042937993000, +0.51015009670676137000 },
	{ +0.85930181835699815000, +0.51146885043796497000 },
	{ +0.85851622426443253000, +0.51278640063355763000 },
	{ +0.85772861000026179000, +0.51410274419321633000 },
	{ +0.85693897741781844000, +0.51541787801945760000 },
	{ +0.85614732837518415000, +0.51673179901764443000 },
	{ +0.85535366473518570000, +0.51804450409599390000 },
	{ +0.85455798836539021000, +0.51935599016558409000 },
	{ +0.85376030113810109000, +0.52066625414036161000 },
	{ +0.85296060493035331000, +0.52197529293714873000 },
	{ +0.85215890162390950000, +0.52328310347565077000 },
	{ +0.85135519310525476000, +0.52458968267846318000 },
	{ +0.85054948126559304000, +0.52589502747107897000 },
	{ +0.84974176800084211000, +0.52719913478189562000 },
	{ +0.84893205521162918000, +0.52850200154222271000 },
	{ +0.84812034480328669000, +0.52980362468628894000 },
	{ +0.84730663868584777000, +0.53110400115124923000 },
	{ +0.84649093877404147000, +0.53240312787719213000 },
	{ +0.84567324698728841000, +0.53370100180714708000 },
	{ +0.84485356524969646000, +0.53499761988709127000 },
	{ +0.84403189549005575000, +0.53629297906595719000 },
	{ +0.84320823964183467000, +0.53758707629563951000 },
	{ +0.84238259964317508000, +0.53887990853100243000 },
	{ +0.84155497743688756000, +0.54017147272988686000 },
	{ +0.84072537497044719000, +0.54146176585311745000 },
	{ +0.83989379419598864000, +0.54275078486450978000 },
	{ +0.83906023707030186000, +0.54403852673087771000 },
	{ +0.83822470555482720000, +0.54532498842204025000 },
	{ +0.83738720161565106000, +0.54661016691082864000 },
	{ +0.83654772722350113000, +0.54789405917309386000 },
	{ +0.83570628435374172000, +0.54917666218771344000 },
	{ +0.83486287498636913000, +0.55045797293659848000 },
	{ +0.83401750110600725000, +0.55173798840470112000 },
	{ +0.83317016470190230000, +0.55301670558002114000 },
	{ +0.83232086776791869000, +0.55429412145361368000 },
	{ +0.83146961230253436000, +0.55557023301959574000 },
	{ +0.83061640030883532000, +0.55684503727515355000 },
	{ +0.82976123379451205000, +0.55811853122054955000 },
	{ +0.82890411477185388000, +0.55939071185912947000 },
	{ +0.82804504525774469000, +0.56066157619732937000 },
	{ +0.82718402727365803000, +0.56193112124468270000 },
	{ +0.82632106284565243000, +0.56319934401382732000 },
	{ +0.82545615400436645000, +0.56446624152051261000 },
	{ +0.82458930278501419000, +0.56573181078360635000 },
	{ +0.82372051122738033000, +0.56699604882510179000 },
	{ +0.82284978137581533000, +0.56825895267012461000 },
	{ +0.82197711527923045000, +0.56952051934694026000 },
	{ +0.82110251499109355000, +0.57078074588696037000 },
	{ +0.82022598256942347000, +0.57203962932475017000 },
	{ +0.81934752007678580000, +0.57329716669803521000 },
	{ +0.81846712958028756000, +0.57455335504770877000 },
	{ +0.81758481315157250000, +0.57580819141783823000 },
	{ +0.81670057286681663000, +0.57706167285567245000 },
	{ +0.81581441080672257000, +0.57831379641164837000 },
	{ +0.81492632905651530000, +0.57956455913939853000 },
	{ +0.81403632970593698000, +0.58081395809575731000 },
	{ +0.81314441484924216000, +0.58206199034076822000 },
	{ +0.81225058658519256000, +0.58330865293769096000 },
	{ +0.81135484701705241000, +0.58455394295300800000 },
	{ +0.81045719825258344000, +0.58579785745643154000 },
	{ +0.80955764240403993000, +0.58704039352091064000 },
	{ +0.80865618158816366000, +0.58828154822263801000 },
	{ +0.80775281792617892000, +0.58952131864105661000 },
	{ +0.80684755354378790000, +0.59075970185886684000 },
	{ +0.80594039057116496000, +0.59199669496203355000 },
	{ +0.80503133114295222000, +0.59323229503979236000 },
	{ +0.80412037739825437000, +0.59446649918465699000 },
	{ +0.80320753148063351000, +0.59569930449242592000 },
	{ +0.80229279553810429000, +0.59693070806218895000 },
	{ +0.80137617172312881000, +0.59816070699633472000 },
	{ +0.80045766219261139000, +0.59938929840055688000 },
	{ +0.79953726910789358000, +0.60061647938386120000 },
	{ +0.79861499463474928000, +0.60184224705857226000 },
	{ +0.79769084094337961000, +0.60306659854034028000 },
	{ +0.79676481020840717000, +0.60428953094814808000 },
	{ +0.79583690460887191000, +0.60551104140431755000 },
	{ +0.79490712632822536000, +0.60673112703451648000 },
	{ +0.79397547755432551000, +0.60794978496776564000 },
	{ +0.79304196047943198000, +0.60916701233644510000 },
	{ +0.79210657730020062000, +0.61038280627630137000 },
	{ +0.79116933021767843000, +0.61159716392645380000 },
	{ +0.79023022143729826000, +0.61281008242940149000 },
	{ +0.78928925316887388000, +0.61402155893103028000 },
	{ +0.78834642762659446000, +0.61523159058061871000 },
	{ +0.78740174702901955000, +0.61644017453084543000 },
	{ +0.78645521359907389000, +0.61764730793779576000 },
	{ +0.78550682956404205000, +0.61885298796096810000 },
	{ +0.78455659715556336000, +0.62005721176328099000 },
	{ +0.78360451860962643000, +0.62125997651107934000 },
	{ +0.78265059616656396000, +0.62246127937414164000 },
	{ +0.78169483207104762000, +0.62366111752568620000 },
	{ +0.78073722857208272000, +0.62485948814237802000 },
	{ +0.77977778792300267000, +0.62605638840433520000 },
	{ +0.77881651238146410000, +0.62725181549513576000 },
	{ +0.77785340420944116000, +0.62844576660182427000 },
	{ +0.77688846567322056000, +0.62963823891491855000 },
	{ +0.77592169904339570000, +0.63082922962841603000 },
	{ +0.77495310659486194000, +0.63201873593980051000 },
	{ +0.77398269060681080000, +0.63320675505004875000 },
	{ +0.77301045336272489000, +0.63439328416363694000 },
	{ +0.77203639715037242000, +0.63557832048854757000 },
	{ +0.77106052426180172000, +0.63676186123627565000 },
	{ +0.77008283699333591000, +0.63794390362183540000 },
	{ +0.76910333764556760000, +0.63912444486376696000 },
	{ +0.76812202852335332000, +0.64030348218414290000 },
	{ +0.76713891193580830000, +0.64148101280857439000 },
	{ +0.76615399019630082000, +0.64265703396621809000 },
	{ +0.76516726562244686000, +0.64383154288978273000 },
	{ +0.76417874053610468000, +0.64500453681553516000 },
	{ +0.76318841726336917000, +0.64617601298330751000 },
	{ +0.76219629813456680000, +0.64734596863650318000 },
	{ +0.76120238548424968000, +0.64851440102210345000 },
	{ +0.76020668165119021000, +0.64968130739067420000 },
	{ +0.75920918897837586000, +0.65084668499637188000 },
	{ +0.75820990981300318000, +0.65201053109695051000 },
	{ +0.75720884650647247000, +0.65317284295376776000 },
	{ +0.75620600141438243000, +0.65433361783179145000 },
	{ +0.75520137689652445000, +0.65549285299960625000 },
	{ +0.75419497531687707000, +0.65665054572941983000 },
	{ +0.75318679904360042000, +0.65780669329706942000 },
	{ +0.75217685044903071000, +0.65896129298202810000 },
	{ +0.75116513190967438000, +0.66011434206741115000 },
	{ +0.75015164580620308000, +0.66126583783998283000 },
	{ +0.74913639452344727000, +0.66241577759016235000 },
	{ +0.74811938045039150000, +0.66356415861203033000 },
	{ +0.74710060598016803000, +0.66471097820333536000 },
	{ +0.74608007351005168000, +0.66585623366550017000 },
	{ +0.74505778544145385000, +0.66699992230362803000 },
	{ +0.74403374417991719000, +0.66814204142650901000 },
	{ +0.74300795213510951000, +0.66928258834662646000 },
	{ +0.74198041172081886000, +0.67042156038016354000 },
	{ +0.74095112535494689000, +0.67155895484700878000 },
	{ +0.73992009545950388000, +0.67269476907076331000 },
	{ +0.73888732446060290000, +0.67382900037874649000 },
	{ +0.73785281478845366000, +0.67496164610200238000 },
	{ +0.73681656887735747000, +0.67609270357530626000 },
	{ +0.73577858916570116000, +0.67722217013717068000 },
	{ +0.73473887809595106000, +0.67835004312985170000 },
	{ +0.73369743811464783000, +0.67947631989935520000 },
	{ +0.73265427167240038000, +0.68060099779544314000 },
	{ +0.73160938122388008000, +0.68172407417163983000 },
	{ +0.73056276922781505000, +0.68284554638523809000 },
	{ +0.72951443814698447000, +0.68396541179730541000 },
	{ +0.72846439044821265000, +0.68508366777269036000 },
	{ +0.72741262860236322000, +0.68620031168002849000 },
	{ +0.72635915508433335000, +0.68731534089174895000 },
	{ +0.72530397237304811000, +0.68842875278408022000 },
	{ +0.72424708295145424000, +0.68954054473705673000 },
	{ +0.72318848930651469000, +0.69065071413452439000 },
	{ +0.72212819392920258000, +0.69175925836414753000 },
	{ +0.72106619931449534000, +0.69286617481741430000 },
	{ +0.72000250796136889000, +0.69397146088964368000 },
	{ +0.71893712237279161000, +0.69507511397999044000 },
	{ +0.71787004505571894000, +0.69617713149145255000 },
	{ +0.71680127852108666000, +0.69727751083087619000 },
	{ +0.71573082528380583000, +0.69837624940896237000 },
	{ +0.71465868786275621000, +0.69947334464027333000 },
	{ +0.71358486878078076000, +0.70056879394323779000 },
	{ +0.71250937056467945000, +0.70166259474015780000 },
	{ +0.71143219574520355000, +0.70275474445721453000 },
	{ +0.71035334685704943000, +0.70384524052447417000 },
	{ +0.70927282643885270000, +0.70493408037589400000 },
	{ +0.70819063703318230000, +0.70602126144932875000 },
	{ +0.70710678118653458000, +0.70710678118653636000 },
	{ +0.70602126144932686000, +0.70819063703318408000 },
	{ +0.70493408037589189000, +0.70927282643885436000 },
	{ +0.70384524052447195000, +0.71035334685705098000 },
	{ +0.70275474445721231000, +0.71143219574520511000 },
	{ +0.70166259474015558000, +0.71250937056468089000 },
	{ +0.70056879394323535000, +0.71358486878078220000 },
	{ +0.69947334464027078000, +0.71465868786275755000 },
	{ +0.69837624940895981000, +0.71573082528380716000 },
	{ +0.69727751083087353000, +0.71680127852108788000 },
	{ +0.69617713149144989000, +0.71787004505572005000 },
	{ +0.69507511397998778000, +0.71893712237279273000 },
	{ +0.69397146088964090000, +0.72000250796136989000 },
	{ +0.69286617481741153000, +0.72106619931449623000 },
	{ +0.69175925836414465000, +0.72212819392920335000 },
	{ +0.69065071413452150000, +0.72318848930651536000 },
	{ +0.68954054473705384000, +0.72424708295145490000 },
	{ +0.68842875278407734000, +0.72530397237304867000 },
	{ +0.68731534089174595000, +0.72635915508433391000 },
	{ +0.68620031168002538000, +0.72741262860236366000 },
	{ +0.68508366777268725000, +0.72846439044821298000 },
	{ +0.68396541179730230000, +0.72951443814698480000 },
	{ +0.68284554638523498000, +0.73056276922781527000 },
	{ +0.68172407417163672000, +0.73160938122388020000 },
	{ +0.68060099779543992000, +0.73265427167240049000 },
	{ +0.67947631989935187000, +0.73369743811464794000 },
	{ +0.67835004312984826000, +0.73473887809595118000 },
	{ +0.67722217013716712000, +0.73577858916570116000 },
	{ +0.67609270357530260000, +0.73681656887735747000 },
	{ +0.67496164610199871000, +0.73785281478845355000 },
	{ +0.67382900037874283000, +0.73888732446060268000 },
	{ +0.67269476907075965000, +0.73992009545950366000 },
	{ +0.67155895484700512000, +0.74095112535494656000 },
	{ +0.67042156038015976000, +0.74198041172081841000 },
	{ +0.66928258834662280000, +0.74300795213510906000 },
	{ +0.66814204142650524000, +0.74403374417991652000 },
	{ +0.66699992230362426000, +0.74505778544145307000 },
	{ +0.66585623366549640000, +0.74608007351005090000 },
	{ +0.66471097820333158000, +0.74710060598016725000 },
	{ +0.66356415861202656000, +0.74811938045039061000 },
	{ +0.66241577759015846000, +0.74913639452344627000 },
	{ +0.66126583783997894000, +0.75015164580620186000 },
	{ +0.66011434206740716000, +0.75116513190967316000 },
	{ +0.65896129298202399000, +0.75217685044902938000 },
	{ +0.65780669329706531000, +0.75318679904359909000 },
	{ +0.65665054572941572000, +0.75419497531687574000 },
	{ +0.65549285299960214000, +0.75520137689652300000 },
	{ +0.65433361783178723000, +0.75620600141438088000 },
	{ +0.65317284295376354000, +0.75720884650647080000 },
	{ +0.65201053109694629000, +0.75820990981300151000 },
	{ +0.65084668499636766000, +0.75920918897837431000 },
	{ +0.64968130739066998000, +0.76020668165118865000 },
	{ +0.64851440102209934000, +0.76120238548424801000 },
	{ +0.64734596863649896000, +0.76219629813456502000 },
	{ +0.64617601298330329000, +0.76318841726336728000 },
	{ +0.64500453681553094000, +0.76417874053610269000 },
	{ +0.64383154288977840000, +0.76516726562244486000 },
	{ +0.64265703396621388000, +0.76615399019629882000 },
	{ +0.64148101280857017000, +0.76713891193580630000 },
	{ +0.64030348218413868000, +0.76812202852335121000 },
	{ +0.63912444486376274000, +0.76910333764556549000 },
	{ +0.63794390362183107000, +0.77008283699333369000 },
	{ +0.63676186123627121000, +0.77106052426179938000 },
	{ +0.63557832048854312000, +0.77203639715036998000 },
	{ +0.63439328416363250000, +0.77301045336272234000 },
	{ +0.63320675505004431000, +0.77398269060680813000 },
	{ +0.63201873593979607000, +0.77495310659485916000 },
	{ +0.63082922962841159000, +0.77592169904339281000 },
	{ +0.62963823891491411000, +0.77688846567321768000 },
	{ +0.62844576660181983000, +0.77785340420943827000 },
	{ +0.62725181549513132000, +0.77881651238146110000 },
	{ +0.62605638840433075000, +0.77977778792299957000 },
	{ +0.62485948814237358000, +0.78073722857207950000 },
	{ +0.62366111752568176000, +0.78169483207104440000 },
	{ +0.62246127937413720000, +0.78265059616656063000 },
	{ +0.62125997651107479000, +0.78360451860962310000 },
	{ +0.62005721176327633000, +0.78455659715556014000 },
	{ +0.61885298796096344000, +0.78550682956403883000 },
	{ +0.61764730793779110000, +0.78645521359907056000 },
	{ +0.61644017453084077000, +0.78740174702901611000 },
	{ +0.61523159058061405000, +0.78834642762659091000 },
	{ +0.61402155893102561000, +0.78928925316887022000 },
	{ +0.61281008242939694000, +0.79023022143729449000 },
	{ +0.61159716392644925000, +0.79116933021767466000 },
	{ +0.61038280627629682000, +0.79210657730019673000 },
	{ +0.60916701233644055000, +0.79304196047942810000 },
	{ +0.60794978496776109000, +0.79397547755432152000 },
	{ +0.60673112703451193000, +0.79490712632822125000 },
	{ +0.60551104140431300000, +0.79583690460886769000 },
	{ +0.60428953094814353000, +0.79676481020840295000 },
	{ +0.60306659854033573000, +0.79769084094337528000 },
	{ +0.60184224705856759000, +0.79861499463474495000 },
	{ +0.60061647938385643000, +0.79953726910788914000 },
	{ +0.59938929840055211000, +0.80045766219260683000 },
	{ +0.59816070699632995000, +0.80137617172312425000 },
	{ +0.59693070806218407000, +0.80229279553809973000 },
	{ +0.59569930449242092000, +0.80320753148062884000 },
	{ +0.59446649918465200000, +0.80412037739824971000 },
	{ +0.59323229503978736000, +0.80503133114294756000 },
	{ +0.59199669496202856000, +0.80594039057116018000 },
	{ +0.59075970185886184000, +0.80684755354378301000 },
	{ +0.58952131864105151000, +0.80775281792617404000 },
	{ +0.58828154822263290000, +0.80865618158815866000 },
	{ +0.58704039352090565000, +0.80955764240403483000 },
	{ +0.58579785745642654000, +0.81045719825257834000 },
	{ +0.58455394295300300000, +0.81135484701704719000 },
	{ +0.58330865293768597000, +0.81225058658518734000 },
	{ +0.58206199034076322000, +0.81314441484923694000 },
	{ +0.58081395809575220000, +0.81403632970593176000 },
	{ +0.57956455913939342000, +0.81492632905650997000 },
	{ +0.57831379641164327000, +0.81581441080671713000 },
	{ +0.57706167285566723000, +0.81670057286681108000 },
	{ +0.57580819141783302000, +0.81758481315156695000 },
	{ +0.57455335504770355000, +0.81846712958028189000 },
	{ +0.57329716669802999000, +0.81934752007678013000 },
	{ +0.57203962932474495000, +0.82022598256941781000 },
	{ +0.57078074588695515000, +0.82110251499108777000 },
	{ +0.56952051934693504000, +0.82197711527922457000 },
	{ +0.56825895267011939000, +0.82284978137580922000 },
	{ +0.56699604882509647000, +0.82372051122737422000 },
	{ +0.56573181078360102000, +0.82458930278500808000 },
	{ +0.56446624152050728000, +0.82545615400436034000 },
	{ +0.56319934401382199000, +0.82632106284564621000 },
	{ +0.56193112124467737000, +0.82718402727365181000 },
	{ +0.56066157619732404000, +0.82804504525773848000 },
	{ +0.55939071185912415000, +0.82890411477184756000 },
	{ +0.55811853122054422000, +0.82976123379450573000 },
	{ +0.55684503727514811000, +0.83061640030882888000 },
	{ +0.55557023301959019000, +0.83146961230252769000 },
	{ +0.55429412145360801000, +0.83232086776791203000 },
	{ +0.55301670558001537000, +0.83317016470189553000 },
	{ +0.55173798840469535000, +0.83401750110600037000 },
	{ +0.55045797293659271000, +0.83486287498636225000 },
	{ +0.54917666218770766000, +0.83570628435373473000 },
	{ +0.54789405917308820000, +0.83654772722349413000 },
	{ +0.54661016691082298000, +0.83738720161564395000 },
	{ +0.54532498842203447000, +0.83822470555481998000 },
	{ +0.54403852673087194000, +0.83906023707029465000 },
	{ +0.54275078486450401000, +0.83989379419598142000 },
	{ +0.54146176585311168000, +0.84072537497043998000 },
	{ +0.54017147272988109000, +0.84155497743688024000 },
	{ +0.53887990853099665000, +0.84238259964316764000 },
	{ +0.53758707629563374000, +0.84320823964182712000 },
	{ +0.53629297906595141000, +0.84403189549004809000 },
	{ +0.53499761988708550000, +0.84485356524968869000 },
	{ +0.53370100180714131000, +0.84567324698728064000 },
	{ +0.53240312787718624000, +0.84649093877403359000 },
	{ +0.53110400115124323000, +0.84730663868583989000 },
	{ +0.52980362468628295000, +0.84812034480327880000 },
	{ +0.52850200154221672000, +0.84893205521162107000 },
	{ +0.52719913478188962000, +0.84974176800083379000 },
	{ +0.52589502747107297000, +0.85054948126558472000 },
	{ +0.52458968267845729000, +0.85135519310524643000 },
	{ +0.52328310347564488000, +0.85215890162390107000 },
	{ +0.52197529293714284000, +0.85296060493034487000 },
	{ +0.52066625414035572000, +0.85376030113809254000 },
	{ +0.51935599016557821000, +0.85455798836538166000 },
	{ +0.51804450409598801000, +0.85535366473517715000 },
	{ +0.51673179901763855000, +0.85614732837517560000 },
	{ +0.51541787801945160000, +0.85693897741780978000 },
	{ +0.51410274419321034000, +0.85772861000025302000 },
	{ +0.51278640063355163000, +0.85851622426442364000 },
	{ +0.51146885043795909000, +0.85930181835698927000 },
	{ +0.51015009670675548000, +0.86008539042937093000 },
	{ +0.50883014254309578000, +0.86086693863774810000 },
	{ +0.50750899105295966000, +0.86164646114306209000 },
	{ +0.50618664534514413000, +0.86242395611102129000 },
	{ +0.50486310853125638000, +0.86319942171210484000 },
	{ +0.50353838372570647000, +0.86397285612156738000 },
	{ +0.50221247404569980000, +0.86474425751944295000 },
	{ +0.50088538261122983000, +0.86551362409054955000 },
	{ +0.49955711254507096000, +0.86628095402449334000 },
	{ +0.49822766697277093000, +0.86704624551567289000 },
	{ +0.49689704902264364000, +0.86780949676328345000 },
	{ +0.49556526182576166000, +0.86857070597132102000 },
	{ +0.49423230851594890000, +0.86932987134858686000 },
	{ +0.49289819222977327000, +0.87008699110869148000 },
	{ +0.49156291610653918000, +0.87084206347005888000 },
	{ +0.49022648328828039000, +0.87159508665593088000 },
	{ +0.48888889691975246000, +0.87234605889437133000 },
	{ +0.48755016014842528000, +0.87309497841826988000 },
	{ +0.48621027612447576000, +0.87384184346534655000 },
	{ +0.48486924800078046000, +0.87458665227815580000 },
	{ +0.48352707893290808000, +0.87532940310409046000 },
	{ +0.48218377207911212000, +0.87607009419538617000 },
	{ +0.48083933060032336000, +0.87680872380912522000 },
	{ +0.47949375766014246000, +0.87754529020724081000 },
	{ +0.47814705642483246000, +0.87827979165652093000 },
	{ +0.47679923006331154000, +0.87901222642861288000 },
	{ +0.47545028174714532000, +0.87974259280002676000 },
	{ +0.47410021465053948000, +0.88047088905214010000 },
	{ +0.47274903195033224000, +0.88119711347120133000 },
	{ +0.47139673682598715000, +0.88192126434833429000 },
	{ +0.47004333245958518000, +0.88264333997954203000 },
	{ +0.46868882203581752000, +0.88336333866571082000 },
	{ +0.46733320874197803000, +0.88408125871261423000 },
	{ +0.46597649576795580000, +0.88479709843091692000 },
	{ +0.46461868630622749000, +0.88551085613617908000 },
	{ +0.46325978355184988000, +0.88622253014885966000 },
	{ +0.46189979070245246000, +0.88693211879432121000 },
	{ +0.46053871095822979000, +0.88763962040283284000 },
	{ +0.45917654752193393000, +0.88834503330957515000 },
	{ +0.45781330359886702000, +0.88904835585464337000 },
	{ +0.45644898239687376000, +0.88974958638305157000 },
	{ +0.45508358712633368000, +0.89044872324473656000 },
	{ +0.45371712100015377000, +0.89114576479456187000 },
	{ +0.45234958723376079000, +0.89184070939232130000 },
	{ +0.45098098904509382000, +0.89253355540274315000 },
	{ +0.44961132965459655000, +0.89322430119549379000 },
	{ +0.44824061228520984000, +0.89391294514518160000 },
	{ +0.44686884016236417000, +0.89459948563136105000 },
	{ +0.44549601651397175000, +0.89528392103853582000 },
	{ +0.44412214457041926000, +0.89596624975616346000 },
	{ +0.44274722756456009000, +0.89664647017865839000 },
	{ +0.44137126873170673000, +0.89732458070539645000 },
	{ +0.43999427130962326000, +0.89800057974071801000 },
	{ +0.43861623853851767000, +0.89867446569393206000 },
	{ +0.43723717366103415000, +0.89934623697931970000 },
	{ +0.43585707992224559000, +0.90001589201613841000 },
	{ +0.43447596056964582000, +0.90068342922862499000 },
	{ +0.43309381885314213000, +0.90134884704600005000 },
	{ +0.43171065802504743000, +0.90201214390247120000 },
	{ +0.43032648134007284000, +0.90267331823723684000 },
	{ +0.42894129205531978000, +0.90333236849448972000 },
	{ +0.42755509343027237000, +0.90398929312342113000 },
	{ +0.42616788872678996000, +0.90464409057822404000 },
	{ +0.42477968120909920000, +0.90529675931809661000 },
	{ +0.42339047414378644000, +0.90594729780724625000 },
	{ +0.42200027079979013000, +0.90659570451489313000 },
	{ +0.42060907444839296000, +0.90724197791527361000 },
	{ +0.41921688836321441000, +0.90788611648764395000 },
	{ +0.41782371582020278000, +0.90852811871628381000 },
	{ +0.41642956009762766000, +0.90916798309050006000 },
	{ +0.41503442447607214000, +0.90980570810462991000 },
	{ +0.41363831223842507000, +0.91044129225804482000 },
	{ +0.41224122666987340000, +0.91107473405515393000 },
	{ +0.41084317105789442000, +0.91170603200540745000 },
	{ +0.40944414869224810000, +0.91233518462330032000 },
	{ +0.40804416286496920000, +0.91296219042837568000 },
	{ +0.40664321687035954000, +0.91358704794522827000 },
	{ +0.40524131400498042000, +0.91420975570350804000 },
	{ +0.40383845756764469000, +0.91483031223792355000 },
	{ +0.40243465085940905000, +0.91544871608824518000 },
	{ +0.40102989718356630000, +0.91606496579930896000 },
	{ +0.39962419984563752000, +0.91667905992101983000 },
	{ +0.39821756215336435000, +0.91729099700835504000 },
	{ +0.39680998741670109000, +0.91790077562136752000 },
	{ +0.39540147894780714000, +0.91850839432518927000 },
	{ +0.39399204006103894000, +0.91911385169003479000 },
	{ +0.39258167407294237000, +0.91971714629120438000 },
	{ +0.39117038430224482000, +0.92031827670908761000 },
	{ +0.38975817406984742000, +0.92091724152916643000 },
	{ +0.38834504669881731000, +0.92151403934201881000 },
	{ +0.38693100551437964000, +0.92210866874332187000 },
	{ +0.38551605384390997000, +0.92270112833385531000 },
	{ +0.38410019501692616000, +0.92329141671950443000 },
	{ +0.38268343236508090000, +0.92387953251126342000 },
	{ +0.38126576922215349000, +0.92446547432523918000 },
	{ +0.37984720892404233000, +0.92504924078265416000 },
	{ +0.37842775480875679000, +0.92563083050984929000 },
	{ +0.37700741021640949000, +0.92621024213828784000 },
	{ +0.37558617848920850000, +0.92678747430455832000 },
	{ +0.37416406297144933000, +0.92736252565037747000 },
	{ +0.37274106700950715000, +0.92793539482259424000 },
	{ +0.37131719395182894000, +0.92850608047319194000 },
	{ +0.36989244714892555000, +0.92907458125929210000 },
	{ +0.36846682995336377000, +0.92964089584315757000 },
	{ +0.36704034571975869000, +0.93020502289219531000 },
	{ +0.36561299780476542000, +0.93076696107895995000 },
	{ +0.36418478956707151000, +0.93132670908115667000 },
	{ +0.36275572436738884000, +0.93188426558164428000 },
	{ +0.36132580556844596000, +0.93243962926843849000 },
	{ +0.35989503653497984000, +0.93299279883471486000 },
	{ +0.35846342063372827000, +0.93354377297881219000 },
	{ +0.35703096123342176000, +0.93409255040423478000 },
	{ +0.35559766170477564000, +0.93463912981965658000 },
	{ +0.35416352542048213000, +0.93518350993892330000 },
	{ +0.35272855575520246000, +0.93572568948105606000 },
	{ +0.35129275608555893000, +0.93626566717025395000 },
	{ +0.34985612979012676000, +0.93680344173589725000 },
	{ +0.34841868024942640000, +0.93733901191255053000 },
	{ +0.34698041084591552000, +0.93787237643996546000 },
	{ +0.34554132496398093000, +0.93840353406308374000 },
	{ +0.34410142598993076000, +0.93893248353204006000 },
	{ +0.34266071731198633000, +0.93945922360216549000 },
	{ +0.34121920232027436000, +0.93998375303398951000 },
	{ +0.33977688440681886000, +0.94050607059324387000 },
	{ +0.33833376696553319000, +0.94102617505086472000 },
	{ +0.33688985339221211000, +0.94154406518299627000 },
	{ +0.33544514708452372000, +0.94205973977099278000 },
	{ +0.33399965144200150000, +0.94257319760142233000 },
	{ +0.33255336986603634000, +0.94308443746606885000 },
	{ +0.33110630575986849000, +0.94359345816193563000 },
	{ +0.32965846252857961000, +0.94410025849124790000 },
	{ +0.32820984357908467000, +0.94460483726145550000 },
	{ +0.32676045232012396000, +0.94510719328523574000 },
	{ +0.32531029216225515000, +0.94560732538049652000 },
	{ +0.32385936651784514000, +0.94610523237037847000 },
	{ +0.32240767880106214000, +0.94660091308325855000 },
	{ +0.32095523242786755000, +0.94709436635275213000 },
	{ +0.31950203081600803000, +0.94758559101771600000 },
	{ +0.31804807738500734000, +0.94807458592225102000 },
	{ +0.31659337555615830000, +0.94856134991570495000 },
	{ +0.31513792875251490000, +0.94904588185267513000 },
	{ +0.31368174039888397000, +0.94952818059301114000 },
	{ +0.31222481392181745000, +0.95000824500181746000 },
	{ +0.31076715274960404000, +0.95048607394945617000 },
	{ +0.30930876031226129000, +0.95096166631154944000 },
	{ +0.30784964004152748000, +0.95143502096898258000 },
	{ +0.30638979537085353000, +0.95190613680790648000 },
	{ +0.30492922973539499000, +0.95237501271974001000 },
	{ +0.30346794657200388000, +0.95284164760117274000 },
	{ +0.30200594931922065000, +0.95330604035416777000 },
	{ +0.30054324141726607000, +0.95376818988596423000 },
	{ +0.29907982630803309000, +0.95422809510907958000 },
	{ +0.29761570743507887000, +0.95468575494131236000 },
	{ +0.29615088824361652000, +0.95514116830574469000 },
	{ +0.29468537218050711000, +0.95559433413074502000 },
	{ +0.29321916269425147000, +0.95604525134997032000 },
	{ +0.29175226323498210000, +0.95649391890236890000 },
	{ +0.29028467725445523000, +0.95694033573218262000 },
	{ +0.28881640820604237000, +0.95738450078894954000 },
	{ +0.28734745954472246000, +0.95782641302750648000 },
	{ +0.28587783472707357000, +0.95826607140799125000 },
	{ +0.28440753721126483000, +0.95870347489584518000 },
	{ +0.28293657045704834000, +0.95913862246181558000 },
	{ +0.28146493792575100000, +0.95957151308195821000 },
	{ +0.27999264308026628000, +0.96000214573763953000 },
	{ +0.27851968938504618000, +0.96043051941553936000 },
	{ +0.27704608030609301000, +0.96085663310765312000 },
	{ +0.27557181931095132000, +0.96128048581129411000 },
	{ +0.27409690986869961000, +0.96170207652909590000 },
	{ +0.27262135544994226000, +0.96212140426901493000 },
	{ +0.27114515952680129000, +0.96253846804433241000 },
	{ +0.26966832557290837000, +0.96295326687365701000 },
	{ +0.26819085706339646000, +0.96336579978092707000 },
	{ +0.26671275747489165000, +0.96377606579541286000 },
	{ +0.26523403028550507000, +0.96418406395171874000 },
	{ +0.26375467897482469000, +0.96458979328978567000 },
	{ +0.26227470702390698000, +0.96499325285489324000 },
	{ +0.26079411791526891000, +0.96539444169766220000 },
	{ +0.25931291513287968000, +0.96579335887405648000 },
	{ +0.25783110216215249000, +0.96619000344538530000 },
	{ +0.25634868248993642000, +0.96658437447830581000 },
	{ +0.25486565960450813000, +0.96697647104482476000 },
	{ +0.25338203699556378000, +0.96736629222230108000 },
	{ +0.25189781815421064000, +0.96775383709344798000 },
	{ +0.25041300657295901000, +0.96813910474633491000 },
	{ +0.24892760574571393000, +0.96852209427438973000 },
	{ +0.24744161916776711000, +0.96890280477640123000 },
	{ +0.24595505033578846000, +0.96928123535652078000 },
	{ +0.24446790274781804000, +0.96965738512426469000 },
	{ +0.24298017990325779000, +0.97003125319451622000 },
	{ +0.24149188530286328000, +0.97040283868752764000 },
	{ +0.24000302244873545000, +0.97077214072892248000 },
	{ +0.23851359484431245000, +0.97113915844969734000 },
	{ +0.23702360599436126000, +0.97150389098622392000 },
	{ +0.23553305940496957000, +0.97186633748025153000 },
	{ +0.23404195858353755000, +0.97222649707890851000 },
	{ +0.23255030703876942000, +0.97258436893470435000 },
	{ +0.23105810828066534000, +0.97293995220553220000 },
	{ +0.22956536582051315000, +0.97329324605467027000 },
	{ +0.22807208317088004000, +0.97364424965078400000 },
	{ +0.22657826384560434000, +0.97399296216792786000 },
	{ +0.22508391135978720000, +0.97433938278554788000 },
	{ +0.22358902922978441000, +0.97468351068848269000 },
	{ +0.22209362097319799000, +0.97502534506696614000 },
	{ +0.22059769010886801000, +0.97536488511662889000 },
	{ +0.21910124015686433000, +0.97570213003850048000 },
	{ +0.21760427463847823000, +0.97603707903901105000 },
	{ +0.21610679707621414000, +0.97636973132999305000 },
	{ +0.21460881099378143000, +0.97670008612868364000 },
	{ +0.21311031991608606000, +0.97702814265772620000 },
	{ +0.21161132736922231000, +0.97735390014517187000 },
	{ +0.21011183688046439000, +0.97767735782448184000 },
	{ +0.20861185197825829000, +0.97799851493452894000 },
	{ +0.20711137619221340000, +0.97831737071959946000 },
	{ +0.20561041305309416000, +0.97863392442939490000 },
	{ +0.20410896609281179000, +0.97894817531903389000 },
	{ +0.20260703884441608000, +0.97926012264905371000 },
	{ +0.20110463484208690000, +0.97956976568541221000 },
	{ +0.19960175762112600000, +0.97987710369948922000 },
	{ +0.19809841071794865000, +0.98018213596808890000 },
	{ +0.19659459767007534000, +0.98048486177344074000 },
	{ +0.19509032201612342000, +0.98078528040320190000 },
	{ +0.19358558729579881000, +0.98108339115045806000 },
	{ +0.19208039704988766000, +0.98137919331372592000 },
	{ +0.19057475482024802000, +0.98167268619695447000 },
	{ +0.18906866414980150000, +0.98196386910952649000 },
	{ +0.18756212858252494000, +0.98225274136626062000 },
	{ +0.18605515166344205000, +0.98253930228741249000 },
	{ +0.18454773693861506000, +0.98282355119867637000 },
	{ +0.18303988795513643000, +0.98310548743118742000 },
	{ +0.18153160826112050000, +0.98338511032152220000 },
	{ +0.18002290140569507000, +0.98366241921170117000 },
	{ +0.17851377093899309000, +0.98393741344918983000 },
	{ +0.17700422041214436000, +0.98421009238689994000 },
	{ +0.17549425337726707000, +0.98448045538319173000 },
	{ +0.17398387338745949000, +0.98474850180187501000 },
	{ +0.17247308399679168000, +0.98501423101221064000 },
	{ +0.17096188876029697000, +0.98527764238891202000 },
	{ +0.16945029123396374000, +0.98553873531214675000 },
	{ +0.16793829497472701000, +0.98579750916753806000 },
	{ +0.16642590354045997000, +0.98605396334616602000 },
	{ +0.16491312048996581000, +0.98630809724456925000 },
	{ +0.16339994938296917000, +0.98655991026474599000 },
	{ +0.16188639378010783000, +0.98680940181415600000 },
	{ +0.16037245724292429000, +0.98705657130572155000 },
	{ +0.15885814333385750000, +0.98730141815782879000 },
	{ +0.15734345561623436000, +0.98754394179432958000 },
	{ +0.15582839765426137000, +0.98778414164454242000 },
	{ +0.15431297301301627000, +0.98802201714325377000 },
	{ +0.15279718525843963000, +0.98825756773071971000 },
	{ +0.15128103795732648000, +0.98849079285266683000 },
	{ +0.14976453467731782000, +0.98872169196029391000 },
	{ +0.14824767898689239000, +0.98895026451027301000 },
	{ +0.14673047445535814000, +0.98917650996475104000 },
	{ +0.14521292465284388000, +0.98940042779135040000 },
	{ +0.14369503315029092000, +0.98962201746317080000 },
	{ +0.14217680351944456000, +0.98984127845879055000 },
	{ +0.14065823933284577000, +0.99005821026226715000 },
	{ +0.13913934416382279000, +0.99027281236313913000 },
	{ +0.13762012158648265000, +0.99048508425642701000 },
	{ +0.13610057517570287000, +0.99069502544263455000 },
	{ +0.13458070850712289000, +0.99090263542774981000 },
	{ +0.13306052515713582000, +0.99110791372324658000 },
	{ +0.13154002870287990000, +0.99131085984608514000 },
	{ +0.13001922272223018000, +0.99151147331871359000 },
	{ +0.12849811079379003000, +0.99170975366906922000 },
	{ +0.12697669649688276000, +0.99190570043057902000 },
	{ +0.12545498341154318000, +0.99209931314216138000 },
	{ +0.12393297511850915000, +0.99229059134822695000 },
	{ +0.12241067519921321000, +0.99247953459867955000 },
	{ +0.12088808723577414000, +0.99266614244891749000 },
	{ +0.11936521481098848000, +0.99285041445983446000 },
	{ +0.11784206150832215000, +0.99303235019782066000 },
	{ +0.11631863091190199000, +0.99321194923476386000 },
	{ +0.11479492660650735000, +0.99338921114805001000 },
	{ +0.11327095217756165000, +0.99356413552056466000 },
	{ +0.11174671121112395000, +0.99373672194069396000 },
	{ +0.11022220729388045000, +0.99390697000232531000 },
	{ +0.10869744401313615000, +0.99407487930484861000 },
	{ +0.10717242495680633000, +0.99424044945315704000 },
	{ +0.10564715371340815000, +0.99440368005764812000 },
	{ +0.10412163387205216000, +0.99456457073422444000 },
	{ +0.10259586902243391000, +0.99472312110429473000 },
	{ +0.10106986275482549000, +0.99487933079477464000 },
	{ +0.09954361866006704300, +0.99503319943808755000 },
	{ +0.09801714032955835500, +0.99518472667216584000 },
	{ +0.09649043135525038600, +0.99533391214045119000 },
	{ +0.09496349532963684100, +0.99548075549189585000 },
	{ +0.09343633584574566300, +0.99562525638096311000 },
	{ +0.09190895649713065600, +0.99576741446762862000 },
	{ +0.09038136087786295700, +0.99590722941738052000 },
	{ +0.08885355258252261600, +0.99604470090122077000 },
	{ +0.08732553520619013000, +0.99617982859566567000 },
	{ +0.08579731234443799300, +0.99631261218274669000 },
	{ +0.08426888759332222500, +0.99644305135001132000 },
	{ +0.08274026454937388800, +0.99657114579052342000 },
	{ +0.08121144680959067900, +0.99669689520286464000 },
	{ +0.07968243797142840500, +0.99682029929113425000 },
	{ +0.07815324163279256700, +0.99694135776495063000 },
	{ +0.07662386139202986800, +0.99706007033945143000 },
	{ +0.07509430084791975100, +0.99717643673529455000 },
	{ +0.07356456359966590000, +0.99729045667865857000 },
	{ +0.07203465324688784800, +0.99740212990124366000 },
	{ +0.07050457338961244000, +0.99751145614027170000 },
	{ +0.06897432762826535800, +0.99761843513848769000 },
	{ +0.06744391956366273300, +0.99772306664415966000 },
	{ +0.06591335279700254200, +0.99782535041107967000 },
	{ +0.06438263092985623000, +0.99792528619856402000 },
	{ +0.06285175756416022700, +0.99802287377145416000 },
	{ +0.06132073630220743300, +0.99811811290011709000 },
	{ +0.05978957074663877900, +0.99821100336044610000 },
	{ +0.05825826450043471200, +0.99830154493386081000 },
	{ +0.05672682116690674900, +0.99838973740730808000 },
	{ +0.05519524434968898400, +0.99847558057326269000 },
	{ +0.05366353765272961100, +0.99855907422972723000 },
	{ +0.05213170468028245600, +0.99864021818023307000 },
	{ +0.05059974903689846300, +0.99871901223384074000 },
	{ +0.04906767432741724500, +0.99879545620514021000 },
	{ +0.04753548415695858100, +0.99886954991425136000 },
	{ +0.04600318213091395000, +0.99894129318682456000 },
	{ +0.04447077185493803600, +0.99901068585404107000 },
	{ +0.04293825693494024400, +0.99907772775261317000 },
	{ +0.04140564097707621200, +0.99914241872478460000 },
	{ +0.03987292758773933200, +0.99920475861833147000 },
	{ +0.03834012037355226400, +0.99926474728656201000 },
	{ +0.03680722294135845000, +0.99932238458831701000 },
	{ +0.03527423889821361400, +0.99937767038797032000 },
	{ +0.03374117185137729500, +0.99943060455542920000 },
	{ +0.03220802540830434300, +0.99948118696613431000 },
	{ +0.03067480317663643500, +0.99952941750106050000 },
	{ +0.02914150876419358000, +0.99957529604671658000 },
	{ +0.02760814577896564600, +0.99961882249514589000 },
	{ +0.02607471782910385600, +0.99965999674392647000 },
	{ +0.02454122852291229200, +0.99969881869617139000 },
	{ +0.02300768146883942800, +0.99973528826052882000 },
	{ +0.02147408027546961200, +0.99976940535118231000 },
	{ +0.01994042855151459400, +0.99980116988785117000 },
	{ +0.01840672990580502500, +0.99983058179579032000 },
	{ +0.01687298794728196700, +0.99985764100579066000 },
	{ +0.01533920628498840400, +0.99988234745417937000 },
	{ +0.01380538852806074400, +0.99990470108281970000 },
	{ +0.01227153828572033000, +0.99992470183911131000 },
	{ +0.01073765916726494500, +0.99994234967599060000 },
	{ +0.00920375478206032420, +0.99995764455193059000 },
	{ +0.00766982873953165390, +0.99997058643094083000 },
	{ +0.00613588464915508330, +0.99998117528256791000 },
	{ +0.00460192612044922970, +0.99998941108189510000 },
	{ +0.00306795676296668650, +0.99999529380954288000 },
	{ +0.00153398018628552680, +0.99999882345166857000 },
	{ +0.00000000000000081250, +0.99999999999996658000 },
	{ -0.00153398018628390180, +0.99999882345166846000 },
	{ -0.00306795676296506110, +0.99999529380954266000 },
	{ -0.00460192612044760430, +0.99998941108189476000 },
	{ -0.00613588464915345790, +0.99998117528256747000 },
	{ -0.00766982873953002840, +0.99997058643094039000 },
	{ -0.00920375478205869880, +0.99995764455193015000 },
	{ -0.01073765916726332000, +0.99994234967599016000 },
	{ -0.01227153828571870200, +0.99992470183911075000 },
	{ -0.01380538852805911400, +0.99990470108281904000 },
	{ -0.01533920628498677100, +0.99988234745417870000 },
	{ -0.01687298794728033300, +0.99985764100579000000 },
	{ -0.01840672990580338700, +0.99983058179578954000 },
	{ -0.01994042855151295300, +0.99980116988785028000 },
	{ -0.02147408027546796700, +0.99976940535118131000 },
	{ -0.02300768146883778000, +0.99973528826052771000 },
	{ -0.02454122852291064400, +0.99969881869617017000 },
	{ -0.02607471782910220400, +0.99965999674392514000 },
	{ -0.02760814577896399100, +0.99961882249514444000 },
	{ -0.02914150876419192200, +0.99957529604671502000 },
	{ -0.03067480317663477000, +0.99952941750105895000 },
	{ -0.03220802540830267100, +0.99948118696613264000 },
	{ -0.03374117185137562300, +0.99943060455542743000 },
	{ -0.03527423889821193500, +0.99937767038796854000 },
	{ -0.03680722294135676400, +0.99932238458831513000 },
	{ -0.03834012037355057100, +0.99926474728656012000 },
	{ -0.03987292758773763200, +0.99920475861832958000 },
	{ -0.04140564097707450500, +0.99914241872478260000 },
	{ -0.04293825693493853700, +0.99907772775261106000 },
	{ -0.04447077185493632900, +0.99901068585403896000 },
	{ -0.04600318213091224300, +0.99894129318682245000 },
	{ -0.04753548415695686700, +0.99886954991424914000 },
	{ -0.04906767432741551700, +0.99879545620513788000 },
	{ -0.05059974903689672800, +0.99871901223383830000 },
	{ -0.05213170468028071500, +0.99864021818023052000 },
	{ -0.05366353765272786200, +0.99855907422972467000 },
	{ -0.05519524434968722100, +0.99847558057326014000 },
	{ -0.05672682116690497300, +0.99838973740730552000 },
	{ -0.05825826450043292100, +0.99830154493385814000 },
	{ -0.05978957074663698200, +0.99821100336044344000 },
	{ -0.06132073630220562900, +0.99811811290011443000 },
	{ -0.06285175756415840900, +0.99802287377145138000 },
	{ -0.06438263092985439800, +0.99792528619856113000 },
	{ -0.06591335279700069600, +0.99782535041107667000 },
	{ -0.06744391956366088700, +0.99772306664415655000 },
	{ -0.06897432762826352600, +0.99761843513848447000 },
	{ -0.07050457338961059500, +0.99751145614026837000 },
	{ -0.07203465324688598800, +0.99740212990124022000 },
	{ -0.07356456359966404000, +0.99729045667865512000 },
	{ -0.07509430084791787800, +0.99717643673529099000 },
	{ -0.07662386139202799500, +0.99706007033944777000 },
	{ -0.07815324163279069300, +0.99694135776494686000 },
	{ -0.07968243797142651700, +0.99682029929113036000 },
	{ -0.08121144680958877800, +0.99669689520286064000 },
	{ -0.08274026454937197200, +0.99657114579051942000 },
	{ -0.08426888759332029600, +0.99644305135000721000 },
	{ -0.08579731234443605000, +0.99631261218274259000 },
	{ -0.08732553520618817300, +0.99617982859566145000 },
	{ -0.08885355258252064500, +0.99604470090121644000 },
	{ -0.09038136087786097200, +0.99590722941737608000 },
	{ -0.09190895649712865800, +0.99576741446762407000 },
	{ -0.09343633584574365100, +0.99562525638095856000 },
	{ -0.09496349532963481400, +0.99548075549189119000 },
	{ -0.09649043135524834600, +0.99533391214044653000 },
	{ -0.09801714032955630200, +0.99518472667216118000 },
	{ -0.09954361866006497500, +0.99503319943808277000 },
	{ -0.10106986275482341000, +0.99487933079476976000 },
	{ -0.10259586902243181000, +0.99472312110428984000 },
	{ -0.10412163387205005000, +0.99456457073421956000 },
	{ -0.10564715371340602000, +0.99440368005764312000 },
	{ -0.10717242495680421000, +0.99424044945315204000 },
	{ -0.10869744401313403000, +0.99407487930484340000 },
	{ -0.11022220729387831000, +0.99390697000231998000 },
	{ -0.11174671121112179000, +0.99373672194068852000 },
	{ -0.11327095217755948000, +0.99356413552055922000 },
	{ -0.11479492660650516000, +0.99338921114804446000 },
	{ -0.11631863091189978000, +0.99321194923475842000 },
	{ -0.11784206150831993000, +0.99303235019781522000 },
	{ -0.11936521481098626000, +0.99285041445982891000 },
	{ -0.12088808723577192000, +0.99266614244891183000 },
	{ -0.12241067519921099000, +0.99247953459867377000 },
	{ -0.12393297511850691000, +0.99229059134822106000 },
	{ -0.12545498341154093000, +0.99209931314215538000 },
	{ -0.12697669649688051000, +0.99190570043057291000 },
	{ -0.12849811079378776000, +0.99170975366906300000 },
	{ -0.13001922272222788000, +0.99151147331870748000 },
	{ -0.13154002870287756000, +0.99131085984607892000 },
	{ -0.13306052515713346000, +0.99110791372324036000 },
	{ -0.13458070850712051000, +0.99090263542774348000 },
	{ -0.13610057517570048000, +0.99069502544262800000 },
	{ -0.13762012158648027000, +0.99048508425642035000 },
	{ -0.13913934416382037000, +0.99027281236313236000 },
	{ -0.14065823933284333000, +0.99005821026226026000 },
	{ -0.14217680351944209000, +0.98984127845878367000 },
	{ -0.14369503315028842000, +0.98962201746316392000 },
	{ -0.14521292465284136000, +0.98940042779134352000 },
	{ -0.14673047445535559000, +0.98917650996474416000 },
	{ -0.14824767898688981000, +0.98895026451026613000 },
	{ -0.14976453467731524000, +0.98872169196028692000 },
	{ -0.15128103795732387000, +0.98849079285265973000 },
	{ -0.15279718525843702000, +0.98825756773071260000 },
	{ -0.15431297301301364000, +0.98802201714324667000 },
	{ -0.15582839765425871000, +0.98778414164453521000 },
	{ -0.15734345561623167000, +0.98754394179432237000 },
	{ -0.15885814333385478000, +0.98730141815782146000 },
	{ -0.16037245724292154000, +0.98705657130571411000 },
	{ -0.16188639378010505000, +0.98680940181414856000 },
	{ -0.16339994938296637000, +0.98655991026473855000 },
	{ -0.16491312048996301000, +0.98630809724456181000 },
	{ -0.16642590354045717000, +0.98605396334615858000 },
	{ -0.16793829497472415000, +0.98579750916753051000 },
	{ -0.16945029123396085000, +0.98553873531213920000 },
	{ -0.17096188876029406000, +0.98527764238890436000 },
	{ -0.17247308399678873000, +0.98501423101220287000 },
	{ -0.17398387338745652000, +0.98474850180186735000 },
	{ -0.17549425337726407000, +0.98448045538318407000 },
	{ -0.17700422041214131000, +0.98421009238689217000 },
	{ -0.17851377093899001000, +0.98393741344918195000 },
	{ -0.18002290140569194000, +0.98366241921169328000 },
	{ -0.18153160826111733000, +0.98338511032151421000 },
	{ -0.18303988795513323000, +0.98310548743117931000 },
	{ -0.18454773693861182000, +0.98282355119866827000 },
	{ -0.18605515166343878000, +0.98253930228740427000 },
	{ -0.18756212858252166000, +0.98225274136625240000 },
	{ -0.18906866414979823000, +0.98196386910951827000 },
	{ -0.19057475482024475000, +0.98167268619694614000 },
	{ -0.19208039704988436000, +0.98137919331371759000 },
	{ -0.19358558729579547000, +0.98108339115044962000 },
	{ -0.19509032201612006000, +0.98078528040319335000 },
	{ -0.19659459767007195000, +0.98048486177343230000 },
	{ -0.19809841071794526000, +0.98018213596808035000 },
	{ -0.19960175762112259000, +0.97987710369948056000 },
	{ -0.20110463484208346000, +0.97956976568540344000 },
	{ -0.20260703884441261000, +0.97926012264904494000 },
	{ -0.20410896609280829000, +0.97894817531902512000 },
	{ -0.20561041305309061000, +0.97863392442938602000 },
	{ -0.20711137619220982000, +0.97831737071959046000 },
	{ -0.20861185197825471000, +0.97799851493451995000 },
	{ -0.21011183688046078000, +0.97767735782447285000 },
	{ -0.21161132736921867000, +0.97735390014516288000 },
	{ -0.21311031991608240000, +0.97702814265771720000 },
	{ -0.21460881099377774000, +0.97670008612867465000 },
	{ -0.21610679707621042000, +0.97636973132998395000 },
	{ -0.21760427463847448000, +0.97603707903900183000 },
	{ -0.21910124015686058000, +0.97570213003849127000 },
	{ -0.22059769010886426000, +0.97536488511661967000 },
	{ -0.22209362097319421000, +0.97502534506695693000 },
	{ -0.22358902922978061000, +0.97468351068847348000 },
	{ -0.22508391135978340000, +0.97433938278553867000 },
	{ -0.22657826384560051000, +0.97399296216791864000 },
	{ -0.22807208317087618000, +0.97364424965077467000 },
	{ -0.22956536582050927000, +0.97329324605466094000 },
	{ -0.23105810828066142000, +0.97293995220552287000 },
	{ -0.23255030703876550000, +0.97258436893469502000 },
	{ -0.23404195858353363000, +0.97222649707889897000 },
	{ -0.23553305940496566000, +0.97186633748024198000 },
	{ -0.23702360599435732000, +0.97150389098621437000 },
	{ -0.23851359484430845000, +0.97113915844968768000 },
	{ -0.24000302244873145000, +0.97077214072891282000 },
	{ -0.24149188530285925000, +0.97040283868751798000 },
	{ -0.24298017990325371000, +0.97003125319450645000 },
	{ -0.24446790274781391000, +0.96965738512425492000 },
	{ -0.24595505033578427000, +0.96928123535651090000 },
	{ -0.24744161916776289000, +0.96890280477639135000 },
	{ -0.24892760574570968000, +0.96852209427437985000 },
	{ -0.25041300657295473000, +0.96813910474632492000 },
	{ -0.25189781815420637000, +0.96775383709343799000 },
	{ -0.25338203699555950000, +0.96736629222229109000 },
	{ -0.25486565960450386000, +0.96697647104481466000 },
	{ -0.25634868248993214000, +0.96658437447829559000 },
	{ -0.25783110216214816000, +0.96619000344537498000 },
	{ -0.25931291513287530000, +0.96579335887404605000 },
	{ -0.26079411791526452000, +0.96539444169765176000 },
	{ -0.26227470702390260000, +0.96499325285488269000 },
	{ -0.26375467897482030000, +0.96458979328977512000 },
	{ -0.26523403028550069000, +0.96418406395170819000 },
	{ -0.26671275747488721000, +0.96377606579540220000 },
	{ -0.26819085706339191000, +0.96336579978091641000 },
	{ -0.26966832557290371000, +0.96295326687364624000 },
	{ -0.27114515952679652000, +0.96253846804432153000 },
	{ -0.27262135544993737000, +0.96212140426900394000 },
	{ -0.27409690986869467000, +0.96170207652908490000 },
	{ -0.27557181931094638000, +0.96128048581128300000 },
	{ -0.27704608030608807000, +0.96085663310764191000 },
	{ -0.27851968938504118000, +0.96043051941552804000 },
	{ -0.27999264308026128000, +0.96000214573762821000 },
	{ -0.28146493792574595000, +0.95957151308194688000 },
	{ -0.28293657045704323000, +0.95913862246180437000 },
	{ -0.28440753721125966000, +0.95870347489583396000 },
	{ -0.28587783472706835000, +0.95826607140798004000 },
	{ -0.28734745954471719000, +0.95782641302749527000 },
	{ -0.28881640820603705000, +0.95738450078893833000 },
	{ -0.29028467725444990000, +0.95694033573217119000 },
	{ -0.29175226323497672000, +0.95649391890235735000 },
	{ -0.29321916269424603000, +0.95604525134995866000 },
	{ -0.29468537218050161000, +0.95559433413073325000 },
	{ -0.29615088824361102000, +0.95514116830573281000 },
	{ -0.29761570743507337000, +0.95468575494130037000 },
	{ -0.29907982630802754000, +0.95422809510906759000 },
	{ -0.30054324141726046000, +0.95376818988595236000 },
	{ -0.30200594931921498000, +0.95330604035415589000 },
	{ -0.30346794657199816000, +0.95284164760116075000 },
	{ -0.30492922973538916000, +0.95237501271972791000 },
	{ -0.30638979537084760000, +0.95190613680789438000 },
	{ -0.30784964004152143000, +0.95143502096897037000 },
	{ -0.30930876031225518000, +0.95096166631153700000 },
	{ -0.31076715274959787000, +0.95048607394944362000 },
	{ -0.31222481392181123000, +0.95000824500180492000 },
	{ -0.31368174039887770000, +0.94952818059299859000 },
	{ -0.31513792875250857000, +0.94904588185266248000 },
	{ -0.31659337555615197000, +0.94856134991569219000 },
	{ -0.31804807738500096000, +0.94807458592223814000 },
	{ -0.31950203081600165000, +0.94758559101770301000 },
	{ -0.32095523242786117000, +0.94709436635273903000 },
	{ -0.32240767880105575000, +0.94660091308324534000 },
	{ -0.32385936651783875000, +0.94610523237036515000 },
	{ -0.32531029216224877000, +0.94560732538048298000 },
	{ -0.32676045232011758000, +0.94510719328522219000 },
	{ -0.32820984357907829000, +0.94460483726144195000 },
	{ -0.32965846252857317000, +0.94410025849123436000 },
	{ -0.33110630575986200000, +0.94359345816192208000 },
	{ -0.33255336986602979000, +0.94308443746605519000 },
	{ -0.33399965144199484000, +0.94257319760140856000 },
	{ -0.33544514708451700000, +0.94205973977097901000 },
	{ -0.33688985339220534000, +0.94154406518298250000 },
	{ -0.33833376696552636000, +0.94102617505085095000 },
	{ -0.33977688440681203000, +0.94050607059322999000 },
	{ -0.34121920232026748000, +0.93998375303397563000 },
	{ -0.34266071731197945000, +0.93945922360215162000 },
	{ -0.34410142598992383000, +0.93893248353202630000 },
	{ -0.34554132496397399000, +0.93840353406306987000 },
	{ -0.34698041084590853000, +0.93787237643995158000 },
	{ -0.34841868024941935000, +0.93733901191253666000 },
	{ -0.34985612979011965000, +0.93680344173588337000 },
	{ -0.35129275608555177000, +0.93626566717024007000 },
	{ -0.35272855575519529000, +0.93572568948104207000 },
	{ -0.35416352542047491000, +0.93518350993890931000 },
	{ -0.35559766170476831000, +0.93463912981964248000 },
	{ -0.35703096123341438000, +0.93409255040422057000 },
	{ -0.35846342063372083000, +0.93354377297879787000 },
	{ -0.35989503653497235000, +0.93299279883470054000 },
	{ -0.36132580556843841000, +0.93243962926842405000 },
	{ -0.36275572436738129000, +0.93188426558162973000 },
	{ -0.36418478956706390000, +0.93132670908114201000 },
	{ -0.36561299780475781000, +0.93076696107894519000 },
	{ -0.36704034571975108000, +0.93020502289218054000 },
	{ -0.36846682995335617000, +0.92964089584314269000 },
	{ -0.36989244714891789000, +0.92907458125927722000 },
	{ -0.37131719395182122000, +0.92850608047317695000 },
	{ -0.37274106700949944000, +0.92793539482257925000 },
	{ -0.37416406297144156000, +0.92736252565036248000 },
	{ -0.37558617848920073000, +0.92678747430454322000 },
	{ -0.37700741021640172000, +0.92621024213827274000 },
	{ -0.37842775480874902000, +0.92563083050983419000 },
	{ -0.37984720892403456000, +0.92504924078263895000 },
	{ -0.38126576922214572000, +0.92446547432522397000 },
	{ -0.38268343236507307000, +0.92387953251124810000 },
	{ -0.38410019501691828000, +0.92329141671948900000 },
	{ -0.38551605384390203000, +0.92270112833383988000 },
	{ -0.38693100551437165000, +0.92210866874330644000 },
	{ -0.38834504669880932000, +0.92151403934200327000 },
	{ -0.38975817406983942000, +0.92091724152915078000 },
	{ -0.39117038430223683000, +0.92031827670907185000 },
	{ -0.39258167407293437000, +0.91971714629118861000 },
	{ -0.39399204006103095000, +0.91911385169001902000 },
	{ -0.39540147894779915000, +0.91850839432517351000 },
	{ -0.39680998741669304000, +0.91790077562135175000 },
	{ -0.39821756215335624000, +0.91729099700833927000 },
	{ -0.39962419984562941000, +0.91667905992100396000 },
	{ -0.40102989718355819000, +0.91606496579929297000 },
	{ -0.40243465085940094000, +0.91544871608822909000 },
	{ -0.40383845756763659000, +0.91483031223790745000 },
	{ -0.40524131400497226000, +0.91420975570349194000 },
	{ -0.40664321687035132000, +0.91358704794521217000 },
	{ -0.40804416286496092000, +0.91296219042835947000 },
	{ -0.40944414869223977000, +0.91233518462328400000 },
	{ -0.41084317105788604000, +0.91170603200539113000 },
	{ -0.41224122666986490000, +0.91107473405513761000 },
	{ -0.41363831223841646000, +0.91044129225802850000 },
	{ -0.41503442447606342000, +0.90980570810461359000 },
	{ -0.41642956009761889000, +0.90916798309048374000 },
	{ -0.41782371582019395000, +0.90852811871626749000 },
	{ -0.41921688836320553000, +0.90788611648762763000 },
	{ -0.42060907444838402000, +0.90724197791525718000 },
	{ -0.42200027079978109000, +0.90659570451487670000 },
	{ -0.42339047414377734000, +0.90594729780722982000 },
	{ -0.42477968120909010000, +0.90529675931808007000 },
	{ -0.42616788872678085000, +0.90464409057820749000 },
	{ -0.42755509343026327000, +0.90398929312340459000 },
	{ -0.42894129205531062000, +0.90333236849447318000 },
	{ -0.43032648134006368000, +0.90267331823722019000 },
	{ -0.43171065802503827000, +0.90201214390245454000 },
	{ -0.43309381885313292000, +0.90134884704598339000 },
	{ -0.43447596056963655000, +0.90068342922860822000 },
	{ -0.43585707992223627000, +0.90001589201612153000 },
	{ -0.43723717366102482000, +0.89934623697930283000 },
	{ -0.43861623853850834000, +0.89867446569391518000 },
	{ -0.43999427130961388000, +0.89800057974070113000 },
	{ -0.44137126873169724000, +0.89732458070537957000 },
	{ -0.44274722756455048000, +0.89664647017864152000 },
	{ -0.44412214457040960000, +0.89596624975614647000 },
	{ -0.44549601651396203000, +0.89528392103851884000 },
	{ -0.44686884016235440000, +0.89459948563134395000 },
	{ -0.44824061228520007000, +0.89391294514516451000 },
	{ -0.44961132965458672000, +0.89322430119547658000 },
	{ -0.45098098904508388000, +0.89253355540272583000 },
	{ -0.45234958723375079000, +0.89184070939230387000 },
	{ -0.45371712100014372000, +0.89114576479454433000 },
	{ -0.45508358712632357000, +0.89044872324471891000 },
	{ -0.45644898239686360000, +0.88974958638303381000 },
	{ -0.45781330359885680000, +0.88904835585462549000 },
	{ -0.45917654752192366000, +0.88834503330955716000 },
	{ -0.46053871095821947000, +0.88763962040281474000 },
	{ -0.46189979070244214000, +0.88693211879430289000 },
	{ -0.46325978355183955000, +0.88622253014884134000 },
	{ -0.46461868630621711000, +0.88551085613616065000 },
	{ -0.46597649576794536000, +0.88479709843089849000 },
	{ -0.46733320874196760000, +0.88408125871259569000 },
	{ -0.46868882203580703000, +0.88336333866569228000 },
	{ -0.47004333245957464000, +0.88264333997952349000 },
	{ -0.47139673682597660000, +0.88192126434831575000 },
	{ -0.47274903195032170000, +0.88119711347118279000 },
	{ -0.47410021465052887000, +0.88047088905212145000 },
	{ -0.47545028174713466000, +0.87974259280000822000 },
	{ -0.47679923006330083000, +0.87901222642859433000 },
	{ -0.47814705642482169000, +0.87827979165650238000 },
	{ -0.47949375766013158000, +0.87754529020722216000 },
	{ -0.48083933060031242000, +0.87680872380910657000 },
	{ -0.48218377207910118000, +0.87607009419536752000 },
	{ -0.48352707893289709000, +0.87532940310407181000 },
	{ -0.48486924800076941000, +0.87458665227813703000 },
	{ -0.48621027612446466000, +0.87384184346532767000 },
	{ -0.48755016014841412000, +0.87309497841825090000 },
	{ -0.48888889691974130000, +0.87234605889435224000 },
	{ -0.49022648328826923000, +0.87159508665591179000 },
	{ -0.49156291610652797000, +0.87084206347003967000 },
	{ -0.49289819222976194000, +0.87008699110867216000 },
	{ -0.49423230851593758000, +0.86932987134856754000 },
	{ -0.49556526182575034000, +0.86857070597130170000 },
	{ -0.49689704902263232000, +0.86780949676326413000 },
	{ -0.49822766697275955000, +0.86704624551565357000 },
	{ -0.49955711254505952000, +0.86628095402447391000 },
	{ -0.50088538261121829000, +0.86551362409053001000 },
	{ -0.50221247404568825000, +0.86474425751942330000 },
	{ -0.50353838372569504000, +0.86397285612154762000 },
	{ -0.50486310853124494000, +0.86319942171208497000 },
	{ -0.50618664534513269000, +0.86242395611100131000 },
	{ -0.50750899105294822000, +0.86164646114304211000 },
	{ -0.50883014254308434000, +0.86086693863772812000 },
	{ -0.51015009670674405000, +0.86008539042935106000 },
	{ -0.51146885043794765000, +0.85930181835696939000 },
	{ -0.51278640063354020000, +0.85851622426440377000 },
	{ -0.51410274419319890000, +0.85772861000023304000 },
	{ -0.51541787801944006000, +0.85693897741778968000 },
	{ -0.51673179901762689000, +0.85614732837515539000 },
	{ -0.51804450409597624000, +0.85535366473515695000 },
	{ -0.51935599016556644000, +0.85455798836536145000 },
	{ -0.52066625414034395000, +0.85376030113807233000 },
	{ -0.52197529293713107000, +0.85296060493032466000 },
	{ -0.52328310347563312000, +0.85215890162388086000 },
	{ -0.52458968267844552000, +0.85135519310522612000 },
	{ -0.52589502747106120000, +0.85054948126556440000 },
	{ -0.52719913478187785000, +0.84974176800081347000 },
	{ -0.52850200154220484000, +0.84893205521160064000 },
	{ -0.52980362468627096000, +0.84812034480325826000 },
	{ -0.53110400115123113000, +0.84730663868581935000 },
	{ -0.53240312787717403000, +0.84649093877401316000 },
	{ -0.53370100180712898000, +0.84567324698726021000 },
	{ -0.53499761988707306000, +0.84485356524966826000 },
	{ -0.53629297906593898000, +0.84403189549002755000 },
	{ -0.53758707629562130000, +0.84320823964180658000 },
	{ -0.53887990853098411000, +0.84238259964314699000 },
	{ -0.54017147272986854000, +0.84155497743685947000 },
	{ -0.54146176585309913000, +0.84072537497041910000 },
	{ -0.54275078486449146000, +0.83989379419596055000 },
	{ -0.54403852673085940000, +0.83906023707027377000 },
	{ -0.54532498842202193000, +0.83822470555479911000 },
	{ -0.54661016691081032000, +0.83738720161562297000 },
	{ -0.54789405917307554000, +0.83654772722347304000 },
	{ -0.54917666218769501000, +0.83570628435371364000 },
	{ -0.55045797293658005000, +0.83486287498634115000 },
	{ -0.55173798840468258000, +0.83401750110597928000 },
	{ -0.55301670558000260000, +0.83317016470187433000 },
	{ -0.55429412145359513000, +0.83232086776789083000 },
	{ -0.55557023301957709000, +0.83146961230250649000 },
	{ -0.55684503727513479000, +0.83061640030880746000 },
	{ -0.55811853122053068000, +0.82976123379448419000 },
	{ -0.55939071185911049000, +0.82890411477182602000 },
	{ -0.56066157619731039000, +0.82804504525771694000 },
	{ -0.56193112124466371000, +0.82718402727363027000 },
	{ -0.56319934401380833000, +0.82632106284562468000 },
	{ -0.56446624152049363000, +0.82545615400433869000 },
	{ -0.56573181078358736000, +0.82458930278498643000 },
	{ -0.56699604882508281000, +0.82372051122735257000 },
	{ -0.56825895267010562000, +0.82284978137578757000 },
	{ -0.56952051934692116000, +0.82197711527920281000 },
	{ -0.57078074588694117000, +0.82110251499106590000 },
	{ -0.57203962932473085000, +0.82022598256939594000 },
	{ -0.57329716669801589000, +0.81934752007675826000 },
	{ -0.57455335504768945000, +0.81846712958026002000 },
	{ -0.57580819141781892000, +0.81758481315154508000 },
	{ -0.57706167285565313000, +0.81670057286678921000 },
	{ -0.57831379641162906000, +0.81581441080669514000 },
	{ -0.57956455913937921000, +0.81492632905648787000 },
	{ -0.58081395809573799000, +0.81403632970590967000 },
	{ -0.58206199034074890000, +0.81314441484921485000 },
	{ -0.58330865293767153000, +0.81225058658516525000 },
	{ -0.58455394295298846000, +0.81135484701702509000 },
	{ -0.58579785745641189000, +0.81045719825255613000 },
	{ -0.58704039352089088000, +0.80955764240401262000 },
	{ -0.58828154822261813000, +0.80865618158813635000 },
	{ -0.58952131864103674000, +0.80775281792615172000 },
	{ -0.59075970185884696000, +0.80684755354376070000 },
	{ -0.59199669496201357000, +0.80594039057113775000 },
	{ -0.59323229503977237000, +0.80503133114292502000 },
	{ -0.59446649918463701000, +0.80412037739822717000 },
	{ -0.59569930449240582000, +0.80320753148060642000 },
	{ -0.59693070806216886000, +0.80229279553807731000 },
	{ -0.59816070699631463000, +0.80137617172310183000 },
	{ -0.59938929840053679000, +0.80045766219258441000 },
	{ -0.60061647938384111000, +0.79953726910786671000 },
	{ -0.60184224705855216000, +0.79861499463472252000 },
	{ -0.60306659854032019000, +0.79769084094335285000 },
	{ -0.60428953094812798000, +0.79676481020838041000 },
	{ -0.60551104140429735000, +0.79583690460884515000 },
	{ -0.60673112703449628000, +0.79490712632819871000 },
	{ -0.60794978496774543000, +0.79397547755429887000 },
	{ -0.60916701233642490000, +0.79304196047940545000 },
	{ -0.61038280627628116000, +0.79210657730017409000 },
	{ -0.61159716392643360000, +0.79116933021765201000 },
	{ -0.61281008242938129000, +0.79023022143727184000 },
	{ -0.61402155893100996000, +0.78928925316884757000 },
	{ -0.61523159058059840000, +0.78834642762656815000 },
	{ -0.61644017453082511000, +0.78740174702899324000 },
	{ -0.61764730793777545000, +0.78645521359904758000 },
	{ -0.61885298796094779000, +0.78550682956401574000 },
	{ -0.62005721176326067000, +0.78455659715553705000 },
	{ -0.62125997651105902000, +0.78360451860960012000 },
	{ -0.62246127937412132000, +0.78265059616653765000 },
	{ -0.62366111752566578000, +0.78169483207102131000 },
	{ -0.62485948814235759000, +0.78073722857205641000 },
	{ -0.62605638840431466000, +0.77977778792297636000 },
	{ -0.62725181549511511000, +0.77881651238143779000 },
	{ -0.62844576660180362000, +0.77785340420941484000 },
	{ -0.62963823891489790000, +0.77688846567319425000 },
	{ -0.63082922962839538000, +0.77592169904336938000 },
	{ -0.63201873593977986000, +0.77495310659483574000 },
	{ -0.63320675505002799000, +0.77398269060678471000 },
	{ -0.63439328416361618000, +0.77301045336269880000 },
	{ -0.63557832048852680000, +0.77203639715034633000 },
	{ -0.63676186123625489000, +0.77106052426177563000 },
	{ -0.63794390362181463000, +0.77008283699330982000 },
	{ -0.63912444486374620000, +0.76910333764554151000 },
	{ -0.64030348218412203000, +0.76812202852332723000 },
	{ -0.64148101280855341000, +0.76713891193578232000 },
	{ -0.64265703396619711000, +0.76615399019627484000 },
	{ -0.64383154288976163000, +0.76516726562242088000 },
	{ -0.64500453681551406000, +0.76417874053607870000 },
	{ -0.64617601298328631000, +0.76318841726334330000 },
	{ -0.64734596863648197000, +0.76219629813454093000 },
	{ -0.64851440102208224000, +0.76120238548422381000 },
	{ -0.64968130739065288000, +0.76020668165116445000 },
	{ -0.65084668499635057000, +0.75920918897835010000 },
	{ -0.65201053109692908000, +0.75820990981297742000 },
	{ -0.65317284295374622000, +0.75720884650644682000 },
	{ -0.65433361783176991000, +0.75620600141435679000 },
	{ -0.65549285299958471000, +0.75520137689649880000 },
	{ -0.65665054572939829000, +0.75419497531685142000 },
	{ -0.65780669329704788000, +0.75318679904357477000 },
	{ -0.65896129298200656000, +0.75217685044900506000 },
	{ -0.66011434206738961000, +0.75116513190964884000 },
	{ -0.66126583783996129000, +0.75015164580617755000 },
	{ -0.66241577759014070000, +0.74913639452342184000 },
	{ -0.66356415861200868000, +0.74811938045036608000 },
	{ -0.66471097820331360000, +0.74710060598014261000 },
	{ -0.66585623366547841000, +0.74608007351002625000 },
	{ -0.66699992230360616000, +0.74505778544142842000 },
	{ -0.66814204142648703000, +0.74403374417989177000 },
	{ -0.66928258834660448000, +0.74300795213508419000 },
	{ -0.67042156038014145000, +0.74198041172079354000 },
	{ -0.67155895484698669000, +0.74095112535492158000 },
	{ -0.67269476907074111000, +0.73992009545947857000 },
	{ -0.67382900037872417000, +0.73888732446057759000 },
	{ -0.67496164610198006000, +0.73785281478842846000 },
	{ -0.67609270357528395000, +0.73681656887733238000 },
	{ -0.67722217013714836000, +0.73577858916567607000 },
	{ -0.67835004312982927000, +0.73473887809592597000 },
	{ -0.67947631989933266000, +0.73369743811462274000 },
	{ -0.68060099779542060000, +0.73265427167237540000 },
	{ -0.68172407417161729000, +0.73160938122385510000 },
	{ -0.68284554638521544000, +0.73056276922779018000 },
	{ -0.68396541179728276000, +0.72951443814695960000 },
	{ -0.68508366777266771000, +0.72846439044818778000 },
	{ -0.68620031168000584000, +0.72741262860233846000 },
	{ -0.68731534089172630000, +0.72635915508430871000 },
	{ -0.68842875278405757000, +0.72530397237302358000 },
	{ -0.68954054473703397000, +0.72424708295142981000 },
	{ -0.69065071413450163000, +0.72318848930649027000 },
	{ -0.69175925836412477000, +0.72212819392917826000 },
	{ -0.69286617481739154000, +0.72106619931447102000 },
	{ -0.69397146088962081000, +0.72000250796134457000 },
	{ -0.69507511397996757000, +0.71893712237276741000 },
	{ -0.69617713149142957000, +0.71787004505569474000 },
	{ -0.69727751083085310000, +0.71680127852106257000 },
	{ -0.69837624940893928000, +0.71573082528378174000 },
	{ -0.69947334464025013000, +0.71465868786273212000 },
	{ -0.70056879394321458000, +0.71358486878075678000 },
	{ -0.70166259474013459000, +0.71250937056465546000 },
	{ -0.70275474445719122000, +0.71143219574517969000 },
	{ -0.70384524052445085000, +0.71035334685702556000 },
	{ -0.70493408037587069000, +0.70927282643882894000 },
	{ -0.70602126144930544000, +0.70819063703315854000 },
	{ -0.70710678118651304000, +0.70710678118651082000 },
	{ -0.70819063703316076000, +0.70602126144930311000 },
	{ -0.70927282643883105000, +0.70493408037586824000 },
	{ -0.71035334685702767000, +0.70384524052444830000 },
	{ -0.71143219574518168000, +0.70275474445718866000 },
	{ -0.71250937056465746000, +0.70166259474013193000 },
	{ -0.71358486878075866000, +0.70056879394321181000 },
	{ -0.71465868786273401000, +0.69947334464024724000 },
	{ -0.71573082528378351000, +0.69837624940893628000 },
	{ -0.71680127852106423000, +0.69727751083085010000 },
	{ -0.71787004505569640000, +0.69617713149142646000 },
	{ -0.71893712237276897000, +0.69507511397996447000 },
	{ -0.72000250796134613000, +0.69397146088961770000 },
	{ -0.72106619931447247000, +0.69286617481738833000 },
	{ -0.72212819392917960000, +0.69175925836412155000 },
	{ -0.72318848930649160000, +0.69065071413449841000 },
	{ -0.72424708295143103000, +0.68954054473703075000 },
	{ -0.72530397237302480000, +0.68842875278405424000 },
	{ -0.72635915508430993000, +0.68731534089172297000 },
	{ -0.72741262860233968000, +0.68620031168000239000 },
	{ -0.72846439044818900000, +0.68508366777266427000 },
	{ -0.72951443814696071000, +0.68396541179727932000 },
	{ -0.73056276922779118000, +0.68284554638521200000 },
	{ -0.73160938122385610000, +0.68172407417161374000 },
	{ -0.73265427167237629000, +0.68060099779541705000 },
	{ -0.73369743811462362000, +0.67947631989932900000 },
	{ -0.73473887809592675000, +0.67835004312982539000 },
	{ -0.73577858916567673000, +0.67722217013714436000 },
	{ -0.73681656887733304000, +0.67609270357527995000 },
	{ -0.73785281478842912000, +0.67496164610197606000 },
	{ -0.73888732446057814000, +0.67382900037872018000 },
	{ -0.73992009545947901000, +0.67269476907073700000 },
	{ -0.74095112535492191000, +0.67155895484698258000 },
	{ -0.74198041172079365000, +0.67042156038013734000 },
	{ -0.74300795213508419000, +0.66928258834660037000 },
	{ -0.74403374417989165000, +0.66814204142648292000 },
	{ -0.74505778544142820000, +0.66699992230360194000 },
	{ -0.74608007351002603000, +0.66585623366547420000 },
	{ -0.74710060598014227000, +0.66471097820330949000 },
	{ -0.74811938045036563000, +0.66356415861200446000 },
	{ -0.74913639452342129000, +0.66241577759013648000 },
	{ -0.75015164580617688000, +0.66126583783995696000 },
	{ -0.75116513190964807000, +0.66011434206738528000 },
	{ -0.75217685044900429000, +0.65896129298200212000 },
	{ -0.75318679904357400000, +0.65780669329704344000 },
	{ -0.75419497531685065000, +0.65665054572939385000 },
	{ -0.75520137689649791000, +0.65549285299958027000 },
	{ -0.75620600141435579000, +0.65433361783176536000 },
	{ -0.75720884650644571000, +0.65317284295374167000 },
	{ -0.75820990981297642000, +0.65201053109692442000 },
	{ -0.75920918897834910000, +0.65084668499634590000 },
	{ -0.76020668165116345000, +0.64968130739064822000 },
	{ -0.76120238548422281000, +0.64851440102207758000 },
	{ -0.76219629813453982000, +0.64734596863647731000 },
	{ -0.76318841726334208000, +0.64617601298328164000 },
	{ -0.76417874053607748000, +0.64500453681550929000 },
	{ -0.76516726562241955000, +0.64383154288975686000 },
	{ -0.76615399019627350000, +0.64265703396619234000 },
	{ -0.76713891193578099000, +0.64148101280854875000 },
	{ -0.76812202852332589000, +0.64030348218411726000 },
	{ -0.76910333764554006000, +0.63912444486374143000 },
	{ -0.77008283699330826000, +0.63794390362180986000 },
	{ -0.77106052426177396000, +0.63676186123625000000 },
	{ -0.77203639715034456000, +0.63557832048852192000 },
	{ -0.77301045336269691000, +0.63439328416361129000 },
	{ -0.77398269060678271000, +0.63320675505002311000 },
	{ -0.77495310659483363000, +0.63201873593977487000 },
	{ -0.77592169904336727000, +0.63082922962839050000 },
	{ -0.77688846567319203000, +0.62963823891489301000 },
	{ -0.77785340420941262000, +0.62844576660179874000 },
	{ -0.77881651238143546000, +0.62725181549511022000 },
	{ -0.77977778792297381000, +0.62605638840430966000 },
	{ -0.78073722857205374000, +0.62485948814235259000 },
	{ -0.78169483207101853000, +0.62366111752566089000 },
	{ -0.78265059616653476000, +0.62246127937411633000 },
	{ -0.78360451860959723000, +0.62125997651105402000 },
	{ -0.78455659715553416000, +0.62005721176325557000 },
	{ -0.78550682956401285000, +0.61885298796094268000 },
	{ -0.78645521359904458000, +0.61764730793777034000 },
	{ -0.78740174702899013000, +0.61644017453082001000 },
	{ -0.78834642762656493000, +0.61523159058059329000 },
	{ -0.78928925316884424000, +0.61402155893100485000 },
	{ -0.79023022143726851000, +0.61281008242937618000 },
	{ -0.79116933021764857000, +0.61159716392642849000 },
	{ -0.79210657730017064000, +0.61038280627627606000 },
	{ -0.79304196047940190000, +0.60916701233641979000 },
	{ -0.79397547755429532000, +0.60794978496774033000 },
	{ -0.79490712632819505000, +0.60673112703449117000 },
	{ -0.79583690460884149000, +0.60551104140429224000 },
	{ -0.79676481020837664000, +0.60428953094812277000 },
	{ -0.79769084094334886000, +0.60306659854031497000 },
	{ -0.79861499463471841000, +0.60184224705854683000 },
	{ -0.79953726910786260000, +0.60061647938383567000 },
	{ -0.80045766219258019000, +0.59938929840053135000 },
	{ -0.80137617172309750000, +0.59816070699630919000 },
	{ -0.80229279553807298000, +0.59693070806216331000 },
	{ -0.80320753148060209000, +0.59569930449240016000 },
	{ -0.80412037739822284000, +0.59446649918463124000 },
	{ -0.80503133114292058000, +0.59323229503976671000 },
	{ -0.80594039057113320000, +0.59199669496200791000 },
	{ -0.80684755354375604000, +0.59075970185884119000 },
	{ -0.80775281792614706000, +0.58952131864103097000 },
	{ -0.80865618158813168000, +0.58828154822261236000 },
	{ -0.80955764240400785000, +0.58704039352088511000 },
	{ -0.81045719825255136000, +0.58579785745640611000 },
	{ -0.81135484701702021000, +0.58455394295298257000 },
	{ -0.81225058658516036000, +0.58330865293766565000 },
	{ -0.81314441484920996000, +0.58206199034074302000 },
	{ -0.81403632970590478000, +0.58081395809573211000 },
	{ -0.81492632905648299000, +0.57956455913937333000 },
	{ -0.81581441080669004000, +0.57831379641162317000 },
	{ -0.81670057286678388000, +0.57706167285564713000 },
	{ -0.81758481315153964000, +0.57580819141781292000 },
	{ -0.81846712958025458000, +0.57455335504768346000 },
	{ -0.81934752007675271000, +0.57329716669800990000 },
	{ -0.82022598256939039000, +0.57203962932472485000 },
	{ -0.82110251499106035000, +0.57078074588693506000 },
	{ -0.82197711527919715000, +0.56952051934691506000 },
	{ -0.82284978137578180000, +0.56825895267009952000 },
	{ -0.82372051122734669000, +0.56699604882507670000 },
	{ -0.82458930278498055000, +0.56573181078358126000 },
	{ -0.82545615400433281000, +0.56446624152048763000 },
	{ -0.82632106284561868000, +0.56319934401380234000 },
	{ -0.82718402727362428000, +0.56193112124465772000 },
	{ -0.82804504525771094000, +0.56066157619730439000 },
	{ -0.82890411477182002000, +0.55939071185910449000 },
	{ -0.82976123379447808000, +0.55811853122052457000 },
	{ -0.83061640030880124000, +0.55684503727512857000 },
	{ -0.83146961230250005000, +0.55557023301957076000 },
	{ -0.83232086776788439000, +0.55429412145358870000 },
	{ -0.83317016470186789000, +0.55301670557999616000 },
	{ -0.83401750110597273000, +0.55173798840467614000 },
	{ -0.83486287498633460000, +0.55045797293657350000 },
	{ -0.83570628435370709000, +0.54917666218768857000 },
	{ -0.83654772722346638000, +0.54789405917306910000 },
	{ -0.83738720161561619000, +0.54661016691080389000 },
	{ -0.83822470555479223000, +0.54532498842201549000 },
	{ -0.83906023707026678000, +0.54403852673085296000 },
	{ -0.83989379419595356000, +0.54275078486448503000 },
	{ -0.84072537497041211000, +0.54146176585309269000 },
	{ -0.84155497743685237000, +0.54017147272986210000 },
	{ -0.84238259964313977000, +0.53887990853097778000 },
	{ -0.84320823964179925000, +0.53758707629561486000 },
	{ -0.84403189549002022000, +0.53629297906593254000 },
	{ -0.84485356524966071000, +0.53499761988706673000 },
	{ -0.84567324698725255000, +0.53370100180712254000 },
	{ -0.84649093877400550000, +0.53240312787716759000 },
	{ -0.84730663868581169000, +0.53110400115122469000 },
	{ -0.84812034480325049000, +0.52980362468626441000 },
	{ -0.84893205521159276000, +0.52850200154219829000 },
	{ -0.84974176800080548000, +0.52719913478187130000 },
	{ -0.85054948126555630000, +0.52589502747105465000 },
	{ -0.85135519310521801000, +0.52458968267843897000 },
	{ -0.85215890162387264000, +0.52328310347562657000 },
	{ -0.85296060493031634000, +0.52197529293712452000 },
	{ -0.85376030113806400000, +0.52066625414033740000 },
	{ -0.85455798836535302000, +0.51935599016555989000 },
	{ -0.85535366473514851000, +0.51804450409596969000 },
	{ -0.85614732837514695000, +0.51673179901762023000 },
	{ -0.85693897741778113000, +0.51541787801943340000 },
	{ -0.85772861000022438000, +0.51410274419319213000 },
	{ -0.85851622426439500000, +0.51278640063353353000 },
	{ -0.85930181835696051000, +0.51146885043794099000 },
	{ -0.86008539042934218000, +0.51015009670673750000 },
	{ -0.86086693863771924000, +0.50883014254307779000 },
	{ -0.86164646114303323000, +0.50750899105294167000 },
	{ -0.86242395611099232000, +0.50618664534512614000 },
	{ -0.86319942171207587000, +0.50486310853123850000 },
	{ -0.86397285612153840000, +0.50353838372568860000 },
	{ -0.86474425751941397000, +0.50221247404568192000 },
	{ -0.86551362409052057000, +0.50088538261121207000 },
	{ -0.86628095402446437000, +0.49955711254505325000 },
	{ -0.86704624551564391000, +0.49822766697275328000 },
	{ -0.86780949676325436000, +0.49689704902262605000 },
	{ -0.86857070597129193000, +0.49556526182574406000 },
	{ -0.86932987134855777000, +0.49423230851593136000 },
	{ -0.87008699110866239000, +0.49289819222975573000 },
	{ -0.87084206347002979000, +0.49156291610652170000 },
	{ -0.87159508665590180000, +0.49022648328826296000 },
	{ -0.87234605889434225000, +0.48888889691973508000 },
	{ -0.87309497841824080000, +0.48755016014840796000 },
	{ -0.87384184346531746000, +0.48621027612445850000 },
	{ -0.87458665227812671000, +0.48486924800076325000 },
	{ -0.87532940310406138000, +0.48352707893289093000 },
	{ -0.87607009419535709000, +0.48218377207909502000 },
	{ -0.87680872380909614000, +0.48083933060030631000 },
	{ -0.87754529020721173000, +0.47949375766012547000 },
	{ -0.87827979165649184000, +0.47814705642481553000 },
	{ -0.87901222642858379000, +0.47679923006329467000 },
	{ -0.87974259279999767000, +0.47545028174712850000 },
	{ -0.88047088905211091000, +0.47410021465052271000 },
	{ -0.88119711347117213000, +0.47274903195031553000 },
	{ -0.88192126434830498000, +0.47139673682597050000 },
	{ -0.88264333997951261000, +0.47004333245956853000 },
	{ -0.88336333866568129000, +0.46868882203580092000 },
	{ -0.88408125871258469000, +0.46733320874196149000 },
	{ -0.88479709843088739000, +0.46597649576793931000 },
	{ -0.88551085613614944000, +0.46461868630621106000 },
	{ -0.88622253014883001000, +0.46325978355183350000 },
	{ -0.88693211879429157000, +0.46189979070243614000 },
	{ -0.88763962040280320000, +0.46053871095821353000 },
	{ -0.88834503330954551000, +0.45917654752191772000 },
	{ -0.88904835585461373000, +0.45781330359885086000 },
	{ -0.88974958638302182000, +0.45644898239685766000 },
	{ -0.89044872324470681000, +0.45508358712631758000 },
	{ -0.89114576479453211000, +0.45371712100013772000 },
	{ -0.89184070939229154000, +0.45234958723374474000 },
	{ -0.89253355540271340000, +0.45098098904507783000 },
	{ -0.89322430119546403000, +0.44961132965458062000 },
	{ -0.89391294514515185000, +0.44824061228519396000 },
	{ -0.89459948563133129000, +0.44686884016234835000 },
	{ -0.89528392103850607000, +0.44549601651395598000 },
	{ -0.89596624975613359000, +0.44412214457040350000 },
	{ -0.89664647017862853000, +0.44274722756454438000 },
	{ -0.89732458070536658000, +0.44137126873169108000 },
	{ -0.89800057974068803000, +0.43999427130960767000 },
	{ -0.89867446569390197000, +0.43861623853850212000 },
	{ -0.89934623697928961000, +0.43723717366101866000 },
	{ -0.90001589201610821000, +0.43585707992223016000 },
	{ -0.90068342922859479000, +0.43447596056963045000 },
	{ -0.90134884704596985000, +0.43309381885312676000 },
	{ -0.90201214390244089000, +0.43171065802503211000 },
	{ -0.90267331823720653000, +0.43032648134005758000 },
	{ -0.90333236849445941000, +0.42894129205530457000 },
	{ -0.90398929312339082000, +0.42755509343025722000 },
	{ -0.90464409057819362000, +0.42616788872677486000 },
	{ -0.90529675931806619000, +0.42477968120908416000 },
	{ -0.90594729780721583000, +0.42339047414377146000 },
	{ -0.90659570451486260000, +0.42200027079977520000 },
	{ -0.90724197791524308000, +0.42060907444837808000 },
	{ -0.90788611648761341000, +0.41921688836319954000 },
	{ -0.90852811871625327000, +0.41782371582018796000 },
	{ -0.90916798309046942000, +0.41642956009761289000 },
	{ -0.90980570810459926000, +0.41503442447605743000 },
	{ -0.91044129225801418000, +0.41363831223841041000 },
	{ -0.91107473405512318000, +0.41224122666985880000 },
	{ -0.91170603200537670000, +0.41084317105787987000 },
	{ -0.91233518462326957000, +0.40944414869223361000 },
	{ -0.91296219042834492000, +0.40804416286495476000 },
	{ -0.91358704794519752000, +0.40664321687034516000 },
	{ -0.91420975570347729000, +0.40524131400496610000 },
	{ -0.91483031223789268000, +0.40383845756763043000 },
	{ -0.91544871608821432000, +0.40243465085939478000 },
	{ -0.91606496579927810000, +0.40102989718355209000 },
	{ -0.91667905992098897000, +0.39962419984562336000 },
	{ -0.91729099700832417000, +0.39821756215335025000 },
	{ -0.91790077562133665000, +0.39680998741668705000 },
	{ -0.91850839432515829000, +0.39540147894779309000 },
	{ -0.91911385169000381000, +0.39399204006102490000 },
	{ -0.91971714629117340000, +0.39258167407292838000 },
	{ -0.92031827670905664000, +0.39117038430223089000 },
	{ -0.92091724152913546000, +0.38975817406983354000 },
	{ -0.92151403934198783000, +0.38834504669880349000 },
	{ -0.92210866874329089000, +0.38693100551436588000 },
	{ -0.92270112833382434000, +0.38551605384389626000 },
	{ -0.92329141671947346000, +0.38410019501691250000 },
	{ -0.92387953251123245000, +0.38268343236506730000 },
	{ -0.92446547432520820000, +0.38126576922213995000 },
	{ -0.92504924078262318000, +0.37984720892402885000 },
	{ -0.92563083050981831000, +0.37842775480874336000 },
	{ -0.92621024213825676000, +0.37700741021639611000 },
	{ -0.92678747430452724000, +0.37558617848919518000 },
	{ -0.92736252565034649000, +0.37416406297143606000 },
	{ -0.92793539482256326000, +0.37274106700949394000 },
	{ -0.92850608047316097000, +0.37131719395181578000 },
	{ -0.92907458125926112000, +0.36989244714891245000 },
	{ -0.92964089584312659000, +0.36846682995335073000 },
	{ -0.93020502289216433000, +0.36704034571974570000 },
	{ -0.93076696107892898000, +0.36561299780475248000 },
	{ -0.93132670908112569000, +0.36418478956705863000 },
	{ -0.93188426558161319000, +0.36275572436737602000 },
	{ -0.93243962926840740000, +0.36132580556843313000 },
	{ -0.93299279883468378000, +0.35989503653496707000 },
	{ -0.93354377297878111000, +0.35846342063371556000 },
	{ -0.93409255040420369000, +0.35703096123340911000 },
	{ -0.93463912981962549000, +0.35559766170476304000 },
	{ -0.93518350993889221000, +0.35416352542046958000 },
	{ -0.93572568948102497000, +0.35272855575518997000 },
	{ -0.93626566717022286000, +0.35129275608554644000 },
	{ -0.93680344173586605000, +0.34985612979011432000 },
	{ -0.93733901191251934000, +0.34841868024941403000 },
	{ -0.93787237643993426000, +0.34698041084590320000 },
	{ -0.93840353406305255000, +0.34554132496396867000 },
	{ -0.93893248353200887000, +0.34410142598991850000 },
	{ -0.93945922360213419000, +0.34266071731197412000 },
	{ -0.93998375303395820000, +0.34121920232026220000 },
	{ -0.94050607059321245000, +0.33977688440680676000 },
	{ -0.94102617505083330000, +0.33833376696552114000 },
	{ -0.94154406518296474000, +0.33688985339220012000 },
	{ -0.94205973977096125000, +0.33544514708451173000 },
	{ -0.94257319760139069000, +0.33399965144198956000 },
	{ -0.94308443746603721000, +0.33255336986602446000 },
	{ -0.94359345816190399000, +0.33110630575985667000 },
	{ -0.94410025849121626000, +0.32965846252856784000 },
	{ -0.94460483726142386000, +0.32820984357907296000 },
	{ -0.94510719328520409000, +0.32676045232011225000 },
	{ -0.94560732538046477000, +0.32531029216224350000 },
	{ -0.94610523237034672000, +0.32385936651783354000 },
	{ -0.94660091308322680000, +0.32240767880105059000 },
	{ -0.94709436635272037000, +0.32095523242785606000 },
	{ -0.94758559101768425000, +0.31950203081599660000 },
	{ -0.94807458592221927000, +0.31804807738499596000 },
	{ -0.94856134991567320000, +0.31659337555614697000 },
	{ -0.94904588185264338000, +0.31513792875250357000 },
	{ -0.94952818059297939000, +0.31368174039887270000 },
	{ -0.95000824500178571000, +0.31222481392180623000 },
	{ -0.95048607394942430000, +0.31076715274959288000 },
	{ -0.95096166631151757000, +0.30930876031225019000 },
	{ -0.95143502096895072000, +0.30784964004151644000 },
	{ -0.95190613680787461000, +0.30638979537084254000 },
	{ -0.95237501271970815000, +0.30492922973538406000 },
	{ -0.95284164760114087000, +0.30346794657199300000 },
	{ -0.95330604035413591000, +0.30200594931920982000 },
	{ -0.95376818988593226000, +0.30054324141725530000 },
	{ -0.95422809510904760000, +0.29907982630802238000 },
	{ -0.95468575494128027000, +0.29761570743506821000 },
	{ -0.95514116830571261000, +0.29615088824360586000 },
	{ -0.95559433413071282000, +0.29468537218049651000 },
	{ -0.95604525134993812000, +0.29321916269424086000 },
	{ -0.95649391890233670000, +0.29175226323497155000 },
	{ -0.95694033573215043000, +0.29028467725444473000 },
	{ -0.95738450078891735000, +0.28881640820603188000 },
	{ -0.95782641302747418000, +0.28734745954471203000 },
	{ -0.95826607140795894000, +0.28587783472706313000 },
	{ -0.95870347489581287000, +0.28440753721125445000 },
	{ -0.95913862246178327000, +0.28293657045703802000 },
	{ -0.95957151308192590000, +0.28146493792574073000 },
	{ -0.96000214573760723000, +0.27999264308025607000 },
	{ -0.96043051941550706000, +0.27851968938503602000 },
	{ -0.96085663310762082000, +0.27704608030608291000 },
	{ -0.96128048581126180000, +0.27557181931094121000 },
	{ -0.96170207652906359000, +0.27409690986868956000 },
	{ -0.96212140426898263000, +0.27262135544993227000 },
	{ -0.96253846804430010000, +0.27114515952679136000 },
	{ -0.96295326687362470000, +0.26966832557289849000 },
	{ -0.96336579978089476000, +0.26819085706338663000 },
	{ -0.96377606579538055000, +0.26671275747488188000 },
	{ -0.96418406395168643000, +0.26523403028549536000 },
	{ -0.96458979328975325000, +0.26375467897481503000 },
	{ -0.96499325285486082000, +0.26227470702389738000 },
	{ -0.96539444169762978000, +0.26079411791525936000 },
	{ -0.96579335887402395000, +0.25931291513287019000 },
	{ -0.96619000344535277000, +0.25783110216214306000 },
	{ -0.96658437447827328000, +0.25634868248992698000 },
	{ -0.96697647104479223000, +0.25486565960449870000 },
	{ -0.96736629222226855000, +0.25338203699555439000 },
	{ -0.96775383709341534000, +0.25189781815420131000 },
	{ -0.96813910474630227000, +0.25041300657294974000 },
	{ -0.96852209427435709000, +0.24892760574570472000 },
	{ -0.96890280477636859000, +0.24744161916775795000 },
	{ -0.96928123535648814000, +0.24595505033577936000 },
	{ -0.96965738512423205000, +0.24446790274780900000 },
	{ -0.97003125319448358000, +0.24298017990324880000 },
	{ -0.97040283868749500000, +0.24149188530285434000 },
	{ -0.97077214072888973000, +0.24000302244872657000 },
	{ -0.97113915844966459000, +0.23851359484430359000 },
	{ -0.97150389098619117000, +0.23702360599435246000 },
	{ -0.97186633748021878000, +0.23553305940496083000 },
	{ -0.97222649707887565000, +0.23404195858352886000 },
	{ -0.97258436893467148000, +0.23255030703876076000 },
	{ -0.97293995220549923000, +0.23105810828065673000 },
	{ -0.97329324605463730000, +0.22956536582050460000 },
	{ -0.97364424965075103000, +0.22807208317087155000 },
	{ -0.97399296216789488000, +0.22657826384559590000 },
	{ -0.97433938278551491000, +0.22508391135977882000 },
	{ -0.97468351068844972000, +0.22358902922977608000 },
	{ -0.97502534506693306000, +0.22209362097318971000 },
	{ -0.97536488511659580000, +0.22059769010885980000 },
	{ -0.97570213003846740000, +0.21910124015685614000 },
	{ -0.97603707903897796000, +0.21760427463847007000 },
	{ -0.97636973132995997000, +0.21610679707620603000 },
	{ -0.97670008612865056000, +0.21460881099377338000 },
	{ -0.97702814265769311000, +0.21311031991607807000 },
	{ -0.97735390014513879000, +0.21161132736921434000 },
	{ -0.97767735782444876000, +0.21011183688045648000 },
	{ -0.97799851493449586000, +0.20861185197825044000 },
	{ -0.97831737071956637000, +0.20711137619220560000 },
	{ -0.97863392442936181000, +0.20561041305308642000 },
	{ -0.97894817531900080000, +0.20410896609280410000 },
	{ -0.97926012264902063000, +0.20260703884440842000 },
	{ -0.97956976568537901000, +0.20110463484207927000 },
	{ -0.97987710369945602000, +0.19960175762111843000 },
	{ -0.98018213596805570000, +0.19809841071794113000 },
	{ -0.98048486177340755000, +0.19659459767006787000 },
	{ -0.98078528040316870000, +0.19509032201611601000 },
	{ -0.98108339115042487000, +0.19358558729579145000 },
	{ -0.98137919331369272000, +0.19208039704988036000 },
	{ -0.98167268619692127000, +0.19057475482024078000 },
	{ -0.98196386910949329000, +0.18906866414979431000 },
	{ -0.98225274136622742000, +0.18756212858251781000 },
	{ -0.98253930228737929000, +0.18605515166343495000 },
	{ -0.98282355119864317000, +0.18454773693860799000 },
	{ -0.98310548743115422000, +0.18303988795512940000 },
	{ -0.98338511032148901000, +0.18153160826111350000 },
	{ -0.98366241921166797000, +0.18002290140568811000 },
	{ -0.98393741344915664000, +0.17851377093898618000 },
	{ -0.98421009238686674000, +0.17700422041213751000 },
	{ -0.98448045538315854000, +0.17549425337726027000 },
	{ -0.98474850180184181000, +0.17398387338745275000 },
	{ -0.98501423101217744000, +0.17247308399678499000 },
	{ -0.98527764238887883000, +0.17096188876029034000 },
	{ -0.98553873531211356000, +0.16945029123395716000 },
	{ -0.98579750916750486000, +0.16793829497472046000 },
	{ -0.98605396334613282000, +0.16642590354045347000 },
	{ -0.98630809724453594000, +0.16491312048995935000 },
	{ -0.98655991026471268000, +0.16339994938296276000 },
	{ -0.98680940181412269000, +0.16188639378010147000 },
	{ -0.98705657130568825000, +0.16037245724291799000 },
	{ -0.98730141815779549000, +0.15885814333385126000 },
	{ -0.98754394179429628000, +0.15734345561622815000 },
	{ -0.98778414164450912000, +0.15582839765425521000 },
	{ -0.98802201714322047000, +0.15431297301301017000 },
	{ -0.98825756773068640000, +0.15279718525843358000 },
	{ -0.98849079285263353000, +0.15128103795732045000 },
	{ -0.98872169196026061000, +0.14976453467731185000 },
	{ -0.98895026451023971000, +0.14824767898688646000 },
	{ -0.98917650996471773000, +0.14673047445535226000 },
	{ -0.98940042779131709000, +0.14521292465283805000 },
	{ -0.98962201746313749000, +0.14369503315028515000 },
	{ -0.98984127845875713000, +0.14217680351943882000 },
	{ -0.99005821026223373000, +0.14065823933284008000 },
	{ -0.99027281236310571000, +0.13913934416381715000 },
	{ -0.99048508425639359000, +0.13762012158647707000 },
	{ -0.99069502544260113000, +0.13610057517569732000 },
	{ -0.99090263542771639000, +0.13458070850711740000 },
	{ -0.99110791372321316000, +0.13306052515713038000 },
	{ -0.99131085984605172000, +0.13154002870287451000 },
	{ -0.99151147331868017000, +0.13001922272222485000 },
	{ -0.99170975366903569000, +0.12849811079378476000 },
	{ -0.99190570043054549000, +0.12697669649687754000 },
	{ -0.99209931314212785000, +0.12545498341153802000 },
	{ -0.99229059134819342000, +0.12393297511850404000 },
	{ -0.99247953459864602000, +0.12241067519920816000 },
	{ -0.99266614244888396000, +0.12088808723576913000 },
	{ -0.99285041445980093000, +0.11936521481098351000 },
	{ -0.99303235019778713000, +0.11784206150831722000 },
	{ -0.99321194923473033000, +0.11631863091189711000 },
	{ -0.99338921114801637000, +0.11479492660650252000 },
	{ -0.99356413552053102000, +0.11327095217755688000 },
	{ -0.99373672194066032000, +0.11174671121111922000 },
	{ -0.99390697000229167000, +0.11022220729387577000 },
	{ -0.99407487930481497000, +0.10869744401313153000 },
	{ -0.99424044945312340000, +0.10717242495680177000 },
	{ -0.99440368005761448000, +0.10564715371340364000 },
	{ -0.99456457073419080000, +0.10412163387204770000 },
	{ -0.99472312110426109000, +0.10259586902242951000 },
	{ -0.99487933079474100000, +0.10106986275482115000 },
	{ -0.99503319943805391000, +0.09954361866006275500 },
	{ -0.99518472667213220000, +0.09801714032955412300 },
	{ -0.99533391214041755000, +0.09649043135524620900 },
	{ -0.99548075549186221000, +0.09496349532963270500 },
	{ -0.99562525638092947000, +0.09343633584574158300 },
	{ -0.99576741446759498000, +0.09190895649712663200 },
	{ -0.99590722941734688000, +0.09038136087785898800 },
	{ -0.99604470090118713000, +0.08885355258251870200 },
	{ -0.99617982859563203000, +0.08732553520618627200 },
	{ -0.99631261218271305000, +0.08579731234443419000 },
	{ -0.99644305134997768000, +0.08426888759331846400 },
	{ -0.99657114579048978000, +0.08274026454937018200 },
	{ -0.99669689520283100000, +0.08121144680958702900 },
	{ -0.99682029929110061000, +0.07968243797142481000 },
	{ -0.99694135776491699000, +0.07815324163278902800 },
	{ -0.99706007033941779000, +0.07662386139202638500 },
	{ -0.99717643673526091000, +0.07509430084791630900 },
	{ -0.99729045667862493000, +0.07356456359966251400 },
	{ -0.99740212990121002000, +0.07203465324688451700 },
	{ -0.99751145614023806000, +0.07050457338960916500 },
	{ -0.99761843513845405000, +0.06897432762826213900 },
	{ -0.99772306664412602000, +0.06744391956365955500 },
	{ -0.99782535041104603000, +0.06591335279699941900 },
	{ -0.99792528619853038000, +0.06438263092985316300 },
	{ -0.99802287377142052000, +0.06285175756415721500 },
	{ -0.99811811290008345000, +0.06132073630220447700 },
	{ -0.99821100336041246000, +0.05978957074663587100 },
	{ -0.99830154493382717000, +0.05825826450043185300 },
	{ -0.99838973740727444000, +0.05672682116690394600 },
	{ -0.99847558057322905000, +0.05519524434968622900 },
	{ -0.99855907422969359000, +0.05366353765272691100 },
	{ -0.99864021818019943000, +0.05213170468027980600 },
	{ -0.99871901223380710000, +0.05059974903689586800 },
	{ -0.99879545620510657000, +0.04906767432741469800 },
	{ -0.99886954991421772000, +0.04753548415695609000 },
	{ -0.99894129318679092000, +0.04600318213091151400 },
	{ -0.99901068585400743000, +0.04447077185493565600 },
	{ -0.99907772775257953000, +0.04293825693493791300 },
	{ -0.99914241872475096000, +0.04140564097707392900 },
	{ -0.99920475861829783000, +0.03987292758773709800 },
	{ -0.99926474728652837000, +0.03834012037355007800 },
	{ -0.99932238458828337000, +0.03680722294135632000 },
	{ -0.99937767038793668000, +0.03527423889821153900 },
	{ -0.99943060455539556000, +0.03374117185137526900 },
	{ -0.99948118696610067000, +0.03220802540830236500 },
	{ -0.99952941750102686000, +0.03067480317663451000 },
	{ -0.99957529604668294000, +0.02914150876419170600 },
	{ -0.99961882249511225000, +0.02760814577896382500 },
	{ -0.99965999674389283000, +0.02607471782910208300 },
	{ -0.99969881869613775000, +0.02454122852291057100 },
	{ -0.99973528826049518000, +0.02300768146883775900 },
	{ -0.99976940535114867000, +0.02147408027546799500 },
	{ -0.99980116988781753000, +0.01994042855151302900 },
	{ -0.99983058179575668000, +0.01840672990580351200 },
	{ -0.99985764100575703000, +0.01687298794728050700 },
	{ -0.99988234745414573000, +0.01533920628498699500 },
	{ -0.99990470108278606000, +0.01380538852805938800 },
	{ -0.99992470183907767000, +0.01227153828571902500 },
	{ -0.99994234967595697000, +0.01073765916726369300 },
	{ -0.99995764455189695000, +0.00920375478205912380 },
	{ -0.99997058643090719000, +0.00766982873953050550 },
	{ -0.99998117528253427000, +0.00613588464915398610 },
	{ -0.99998941108186146000, +0.00460192612044818450 },
	{ -0.99999529380950924000, +0.00306795676296569290 },
	{ -0.99999882345163493000, +0.00153398018628458490 },
	{ -0.99999999999993294000, -0.00000000000000007785 },
};
#endif /* FFTSizeFilters == 4096 */

#endif /* WITHDYNAMICCOEFFS */

static void FFT_process(struct Complex *x, int n, int m, const struct Complex *w)
{
   struct Complex u, tm;
   const struct Complex *wptr;
   int i, j, k, l, le, windex = 1;

   //for (m = 0; (1 << m) < n; m++)
//	   ;
   //const int m = FFTSizeFiltersM;
   le = n;
   for (l = 0; l < m; l++)
   {
      le /= 2;
      for (i = 0; i < n; i += 2 * le)
      {
		 struct Complex temp, *xi, *xip;
         xi = x + i;
         xip = xi + le;
         temp.real = xi->real + xip->real;
         temp.imag = xi->imag + xip->imag;
         xip->real = xi->real - xip->real;
         xip->imag = xi->imag - xip->imag;
         *xi = temp;
      }
      wptr = w + windex - 1;
      for (j = 1; j < le; j++)
      {
         u = *wptr;
         for (i = j; i < n; i += 2 * le)
         {
			struct Complex temp, *xi, *xip;
            xi = x + i;
            xip = xi + le;
            temp.real = xi->real + xip->real;
            temp.imag = xi->imag + xip->imag;
            tm.real = xi->real - xip->real;
            tm.imag = xi->imag - xip->imag;
            xip->real = tm.real * u.real - tm.imag * u.imag;
            xip->imag = tm.real * u.imag + tm.imag * u.real;
            *xi = temp;
         }
         wptr += windex;
      }
      windex *= 2;
   }
   j = 0;
   for (i = 1; i < n - 1; i++)
   {
      for (k = n / 2; k <= j; k /= 2)
         j -= k;
      j += k;
      if (i < j)
      {
		 struct Complex temp, *xi, *xj;
         xi = x + i;
         xj = x + j;
         temp = *xj;
         *xj = *xi;
         *xi = temp;
      }
   }
}

static void FFT(struct Complex *x, int n, int m)
{
	FFT_process(x, n, m, wm);
}

static void IFFT(struct Complex *x, int n, int m)
{
	int i;
	FFT_process(x, n, m, wp);
	for (i = 0; i < n; i++)
	{
		x[i].real /= n;
		x[i].imag /= n;
	}
}

static void FFT_initialize(void)
{
#if WITHDYNAMICCOEFFS

	FFT_initw(wm, FFTSizeFilters, -1);
	FFT_initw(wp, FFTSizeFilters, 1);

#endif /* WITHDYNAMICCOEFFS */
}

////////////////////////////////////////////////

static struct Complex Sig [FFTSizeFilters];

/* получение пикового значения АЧХ */
static FLOAT_t getmaxresponce(void)
{
	FLOAT_t r = (FLOAT_t) 1 / 16384;
	int i;
	for (i = 0; i < FFTSizeFilters / 2; ++ i)
	{
		r = FMAXF(r, SQRTF(Sig [i].real * Sig [i].real + Sig [i].imag * Sig [i].imag));
	}
	return r;
}
//====================================================
//  calculate impulse response of FIR filter
//====================================================

// Получение АЧХ из коэффициентов симмметричного FIR

static void imp_response(const FLOAT_t *dCoeff, int iCoefNum) 
{
	const int iHalfLen = (iCoefNum - 1) / 2;
	int i;

	//---------------------------
	// copy coefficients to Sig
	//---------------------------
	Sig [iHalfLen].real = dCoeff [iHalfLen];
	Sig [iHalfLen].imag = 0;
	for (i = 1; i <= iHalfLen; ++ i) 
	{
		const FLOAT_t k = dCoeff [iHalfLen - i];
		Sig [iHalfLen - i].real = k;
		Sig [iHalfLen + i].real = k;
		Sig [iHalfLen - i].imag = 0;
		Sig [iHalfLen + i].imag = 0;
	} 	

	//---------------------------
	// append zeros
	//---------------------------
	for (i = iCoefNum; i < FFTSizeFilters; ++ i) {
		Sig [i].real = 0;
		Sig [i].imag = 0;
	} 	

	//---------------------------
	// Do FFT
	//---------------------------

	FFT(Sig, FFTSizeFilters, FFTSizeFiltersM); 
}

//====================================================
// construct FIR coefficients from frequency response
//====================================================
static void reconstruct(void)
{

	// Центр симметрии Sig - ячейка с индексом FFTSizeFilters / 2
	// Преобразование из responce в набор коэффициентов для симметричного FIR

#if 1
#else
	int i;
	// Центр симметрии Sig - ячейка с индексом FFTSizeFilters / 2
	// +++ black magic
	for (i = 0; i < FFTSizeFilters / 2; i++)
	{
		Sig [FFTSizeFilters - 1 - i].imag = - Sig [i + 1].imag;
		Sig [FFTSizeFilters - 1 - i].real = Sig [i + 1].real;
	}
	Sig [FFTSizeFilters / 2].imag = Sig [FFTSizeFilters / 2 + 1].imag;
	Sig [FFTSizeFilters / 2].real = Sig [FFTSizeFilters / 2 + 1].real;
	// --- black magic
#endif
	IFFT(Sig, FFTSizeFilters, FFTSizeFiltersM); 

}

static void sigtocoeffs(FLOAT_t *dCoeff, int iCoefNum)
{
	const int j = NtapCoeffs(iCoefNum);
	int i;
	//---------------------------
	// Magnitude in dB
	//---------------------------
	for (i = 0; i < j; ++ i) {
		dCoeff [i] = Sig [i].real;
	}

}

static void scalecoeffs(FLOAT_t *dCoeff, int iCoefNum, FLOAT_t scale)
{
	const int j = NtapCoeffs(iCoefNum);
	int i;
	//---------------------------
	// Magnitude in dB
	//---------------------------
	for (i = 0; i < j; ++ i) {
		dCoeff [i] *= scale;
	}

}


static void fir_design_applaywindow(FLOAT_t *dCoeff, const FLOAT_t *dWindow, int iCoefNum);
static void fir_design_applaywindowL(double *dCoeff, const double *dWindow, int iCoefNum);

// slope: изменение тембра звука - на Samplerate/2 АЧХ становится на столько децибел 
// scale: общий масштаб изменения АЧХ
static void correctspectrumcomplex(int_fast8_t targetdb)
{
#if 1
	const FLOAT_t slope = db2ratio(targetdb);
	// Центр симметрии Sig - ячейка с индексом FFTSizeFilters / 2
	FLOAT_t scale = 1;
	const FLOAT_t step = POWF(slope, (FLOAT_t) 1 / (FFTSizeFilters / 2 - 1));
	const int n = FFTSizeFilters / 2;
	int i;
	for (i = 1; i < n; ++ i, scale *= step)
	{
		Sig [i].real *= scale;
		Sig [i].imag *= scale;
		Sig [FFTSizeFilters - i].real = Sig [i].real;
		Sig [FFTSizeFilters - i].imag = - Sig [i].imag;
	}
	/* корректируем центральный элемент массива */
	Sig [FFTSizeFilters / 2].real *= scale;
	Sig [FFTSizeFilters / 2].imag *= scale;
	/* лишний? */
	//Sig [0].real = 0;
	//Sig [0].imag = 0;
#else

	// https://ru.wikipedia.org/wiki/%D0%A6%D0%B2%D0%B5%D1%82%D0%B0_%D1%88%D1%83%D0%BC%D0%B0
	// Броуновский (красный, "коричневый") шум
	// Энергия шума падает на 6 децибел на октаву
	const FLOAT_t ratio = db2ratio(targetdb);
	const FLOAT_t slope = LOG10F(ratio) / (FLOAT_t) M_LOG2E;
	const int n = FFTSizeFilters / 2;
	//y = exp(log(100) - log(x)), x: 1..100
	const FLOAT_t delta = 1 / EXPF(slope * LOGF((FLOAT_t) 1 / n));
	int i;
	for (i = 1; i < n; ++ i)
	{
		const FLOAT_t scale = EXPF(slope * LOGF((FLOAT_t) i / n)) * delta;
		Sig [i].real *= scale;
		Sig [i].imag *= scale;
		Sig [FFTSizeFilters - i].real = Sig [i].real;
		Sig [FFTSizeFilters - i].imag = - Sig [i].imag;
	}
	/* корректируем центральный элемент массива */
	Sig [FFTSizeFilters / 2].real *= ratio;
	Sig [FFTSizeFilters / 2].imag *= ratio;
#endif
}

// Формирование наклона АЧХ звукового тракта приемника
static void fir_design_adjust_rx(FLOAT_t *dCoeff, const FLOAT_t *dWindow, int iCoefNum, uint_fast8_t usewindow)
{
	if (glob_afresponcerx != 0)
	{
		imp_response(dCoeff, iCoefNum);	//  calculate impulse response of FIR filter
		correctspectrumcomplex(glob_afresponcerx);
		reconstruct();					// construct FIR coefficients from frequency response
		sigtocoeffs(dCoeff, iCoefNum);
	}

	if (usewindow != 0)
		fir_design_applaywindow(dCoeff, dWindow, iCoefNum);

	imp_response(dCoeff, iCoefNum);	//  calculate impulse response of FIR filter
	const FLOAT_t resp = getmaxresponce();
	scalecoeffs(dCoeff, iCoefNum, 1 / resp);	// нормалтизация к. передаци к заданному значению (1)
}

// Формирование наклона АЧХ звукового тракта передатчика
static void fir_design_adjust_tx(FLOAT_t *dCoeff, const FLOAT_t *dWindow, int iCoefNum)
{
	if (glob_afresponcetx != 0)
	{
		imp_response(dCoeff, iCoefNum);	//  calculate impulse response of FIR filter
		correctspectrumcomplex(glob_afresponcetx);
		reconstruct();	// construct FIR coefficients from frequency response
		sigtocoeffs(dCoeff, iCoefNum);
	}
	fir_design_applaywindow(dCoeff, dWindow, iCoefNum);
	imp_response(dCoeff, iCoefNum);	//  calculate impulse response of FIR filter
	const FLOAT_t resp = getmaxresponce();
	scalecoeffs(dCoeff, iCoefNum, 1 / resp);	// нормалтизация к. передаци к заданному значению (1)
}

// Расчёт коэффициента для работы в дискретном времени системы АРУ.
// вызывается на каждый сэмпл с АЦП - частота ARMSAIRATE в герцах.
// Аргумент: постоянная времени цепи в секундах
// Результат: 1 - мгновенно, 0 - никогда

static FLOAT_t MAKETAUIF(FLOAT_t t)
{
	if (t == 0)
		return 1;

	const FLOAT_t samplerate = ARMSAIRATE;	// 48 kHz
	const FLOAT_t step = POWF((FLOAT_t) M_SQRT1_2, 1 / (t * samplerate));
	//const FLOAT_t step = EXPF(- 1 / (t * samplerate));

	return 1 - step;
}

static FLOAT_t MAKETAUAF(FLOAT_t t)
{
	if (t == 0)
		return 1;

	const FLOAT_t samplerate = ARMI2SRATE;	// 48 kHz or 12 kHz
	const FLOAT_t step = POWF((FLOAT_t) M_SQRT1_2, 1 / (t * samplerate));
	//const FLOAT_t step = EXPF(- 1 / (t * samplerate));

	return 1 - step;
}

// Результат: 1 - мгновенно
static FLOAT_t MAKETAUAF0(void)
{
	return 1;
}

// интегрирующее звено.
// v1 - current value, v2 - charger value
// R2 by series with capacitor
// chargespeed 0: никогда, 1: мгновенно
// Большим значениям сигнала соответствуют более положительные значения.

static void charge2(volatile FLOAT_t * vcap, FLOAT_t vinput, FLOAT_t chargespeed)
{
	* vcap += (vinput - * vcap) * chargespeed;
}

typedef struct agcstate
{
	FLOAT_t  agcfastcap;	// разница после выпрямления
	FLOAT_t  agcslowcap;	// разница после выпрямления
	unsigned agchangticks;				// сколько сэмплов надо сохранять agcslowcap неизменным.
} agcstate_t;

typedef struct agcparams
{
	uint_fast8_t agcoff;	// признак отключения АРУ

	// Временные паарметры АРУ

	// постоянные времени цепи АРУ для реакции на импульсные помехи (быстрая АРУ).
	FLOAT_t dischargespeedfast;	//0.02f;	// 1 - мгновенно, 0 - никогда
	FLOAT_t	chargespeedfast;

	// постоянные времени основного фильтра АРУ -  время заряды должно быть того же порядка, что и разряд цепи быстрой АРУ
	FLOAT_t chargespeedslow;		//0.05f;	// 1 - мгновенно, 0 - никогда
	FLOAT_t dischargespeedslow;	// 1 - мгновенно, 0 - никогда
	unsigned hungticks;				// сколько сэмплов надо сохранять agcslowcap неизменным.

	// Амплитудные параметры АРУ

	FLOAT_t gainlimit;				// Максимальное усиление в разах по напряжению, допустимое для АРУ
	FLOAT_t	mininput;
	FLOAT_t levelfence;				// Максимальнное значение на выхоле АРУ
	FLOAT_t agcfactor;				// Параметр при вычислении "спортивной" АРУ
} agcparams_t;


/////////////
// agc +++

static FLOAT_t agc_calcagcfactor(uint_fast8_t rate)
{
	return - (1 - 1 / (FLOAT_t) rate);
}

// Начальная установка  параметров АРУ приёмника

static void agc_parameters_initialize(volatile agcparams_t * agcp)
{
	agcp->agcoff = 0;

	agcp->dischargespeedfast = MAKETAUIF((FLOAT_t) 0.095);
#if WITHLIMITEDAGCATTACK
	agcp->chargespeedfast = MAKETAUIF((FLOAT_t) 0.001);	// 1 mS
#else /* WITHLIMITEDAGCATTACK */
	agcp->chargespeedfast = MAKETAUAF0();
#endif /* WITHLIMITEDAGCATTACK */

	agcp->chargespeedslow = MAKETAUIF((FLOAT_t) 0.095);
	agcp->dischargespeedslow = MAKETAUIF((FLOAT_t) 0.2);	

	agcp->hungticks = NSAITICKS(300);			// 0.3 secounds

	agcp->gainlimit = db2ratio(60);
	agcp->mininput = 16;
	agcp->levelfence = rxlevelfence;
	agcp->agcfactor = agc_calcagcfactor(10);

	//debug_printf_P(PSTR("agc_parameters_initialize: dischargespeedfast=%f, chargespeedfast=%f\n"), agcp->dischargespeedfast, agcp->chargespeedfast);
}

// Установка параметров АРУ приёмника

static void agc_parameters_update(volatile agcparams_t * const agcp, FLOAT_t gainlimit, uint_fast8_t pathi)
{
	const uint_fast8_t flatgain = glob_agcrate [pathi] == UINT8_MAX;

	agcp->agcoff = (glob_agc == BOARD_AGCCODE_OFF);

	agcp->dischargespeedfast = MAKETAUIF((int) glob_agc_t4 [pathi] * (FLOAT_t) 0.001);	// в милисекундах

	agcp->chargespeedslow = MAKETAUIF((int) glob_agc_t1 [pathi] * (FLOAT_t) 0.001);	// в милисекундах
	agcp->dischargespeedslow = MAKETAUIF((int) glob_agc_t2 [pathi] * (FLOAT_t) 0.1);	// в сотнях милисекунд (0.1 секунды)
	agcp->hungticks = NSAITICKS(glob_agc_thung [pathi] * 100);			// в сотнях милисекунд (0.1 секунды)

	agcp->gainlimit = gainlimit;
	agcp->agcfactor = flatgain ? (FLOAT_t) -1 : agc_calcagcfactor(glob_agcrate [pathi]);

	//debug_printf_P(PSTR("agc_parameters_update: dischargespeedfast=%f, chargespeedfast=%f\n"), agcp->dischargespeedfast, agcp->chargespeedfast);
}

// Начальная установка параметров АРУ микрофонного тракта передатчика

static void comp_parameters_initialize(volatile agcparams_t * agcp)
{
	agcp->agcoff = 0;

	agcp->dischargespeedfast = MAKETAUIF((FLOAT_t) 0.100);
	agcp->chargespeedfast = MAKETAUAF0();

	agcp->chargespeedslow = MAKETAUIF((FLOAT_t) 0.200);
	agcp->dischargespeedslow = MAKETAUIF((FLOAT_t) 0.200);

	agcp->hungticks = NSAITICKS(300);			// 0.3 secounds

	agcp->gainlimit = db2ratio(60);
	agcp->mininput = 1;
	agcp->levelfence = txlevelfence * (int) glob_mikeagcscale / 100;
	agcp->agcfactor = (FLOAT_t) - 1;
}

// Установка параметров АРУ передатчика

static void comp_parameters_update(volatile agcparams_t * const agcp, FLOAT_t gainlimit)
{
	agcp->agcoff = glob_mikeagc == 0;

	agcp->gainlimit = gainlimit;
	agcp->levelfence = txlevelfence * (int) glob_mikeagcscale / 100;
}

// детектор АРУ - поддерживает выходное значение пропорционально сигналу 
// со всеми положенными задержками на срабатывание/отпускание

static void
performagc(const volatile agcparams_t * agcp, volatile agcstate_t * st, FLOAT_t sample)
{
	// быстрая цепь АРУ
	if (st->agcfastcap < sample)
	{
		// заряжается в соответствии с параметром agcp->chargespeedfast
		charge2(& st->agcfastcap, sample, agcp->chargespeedfast);
	}
	else
	{
		// разряд со скоростью agcp->dischargespeedfast
		charge2(& st->agcfastcap, sample, agcp->dischargespeedfast);
	}

	// медленная цепь АРУ
	// hang time processing
	if (st->agcslowcap < st->agcfastcap)	// требуется заряд slow цепи
	{
		// заряжается
		charge2(& st->agcslowcap, st->agcfastcap, agcp->chargespeedslow);
		st->agchangticks = agcp->hungticks;
	}
	else if (st->agcslowcap > st->agcfastcap)	// требуется заряд slow цепи
	{
		if (st->agchangticks == 0)
		{
			// разряжается
			charge2(& st->agcslowcap, st->agcfastcap, agcp->dischargespeedslow);
		}
		else
		{
			// Ждем окончания hang time
			st->agchangticks -= 1;
		}
	}
	else
	{
		// не меняется значение
		st->agchangticks = agcp->hungticks;
	}
}

static FLOAT_t performagcresultslow(const volatile agcstate_t * st)
{
	return FMAXF(st->agcfastcap, st->agcslowcap);	// разница после ИЛИ
}

static FLOAT_t performagcresultfast(const volatile agcstate_t * st)
{
	return st->agcfastcap;
}


///////////////////////////

static FLOAT_t mikeinlevel;
static FLOAT_t VOXDISCHARGE;
static FLOAT_t VOXCHARGE = 0;

// Возвращает значения 0..255
uint_fast8_t dsp_getvox(void)
{
	return mikeinlevel / (1 << (WITHAFADCWIDTH - 8 - 1));	// масшабирование q15 к 0..255
}

// Возвращает значения 0..255
uint_fast8_t dsp_getavox(void)
{
	return 0;
}

static void voxmeter_initialize(void)
{
	VOXCHARGE = MAKETAUAF0();	// Пиковый детектор со временем зарада 0
	VOXDISCHARGE = MAKETAUAF((FLOAT_t) 0.02);	// Пиковый детектор со временем разряда 0.02 секунды
}

// Шумоподавитель NFM

static FLOAT_t NBTAU;
static FLOAT_t holdmaxv3;
static void testholdmax3(FLOAT_t v)
{
	FLOAT_t x = holdmaxv3;
	FLOAT_t t = FABSF(v);
	x = (x < t) ? t : x * NBTAU;
	holdmaxv3 = x;
}
long getholdmax3(void)
{
	return holdmaxv3;
}


/////////////////////////////
// Формирование аудиопотока к оператору
//

#if 0
/* Фильтр, использующий сгенерированные MATLAB таблицы параметров.
 * Tnx to RX9CIM
 */
static FLOAT_t iir_filter(
	FLOAT_t new_sample, 
	FLOAT_t (* aWHistory) [2], 
	const FLOAT_t (* aNUM) [3], const FLOAT_t (* aDEN) [3]
	)
{
	int i;
	for (i = 0; i < MWSPT_NSEC; i ++)
	{
		FLOAT_t buf = new_sample;
		int j;
		for (j = 1; j < DL_lpf_1550 [i]; j ++)
		{
			buf -= aDEN[i][j] * aWHistory [i][j - 1];
		}
		buf /= aDEN[i][0];
		new_sample = buf * aNUM [i][0];
		for (j = 1; j < NL_lpf_1550 [i]; j ++)
		{  
			new_sample += aNUM[i][j] * aWHistory [i] [j - 1];
		}
		aWHistory [i] [1] = aWHistory [i] [0];
		aWHistory [i] [0] = buf;
	}
	return new_sample;
}
#endif

// Получить усиление фильтра на частоте 0
// Массив коэффициентов для симметричного фильтра
static FLOAT_t testgain_float_DC(const FLOAT_t * dCoeff, int iCoefNum)
{
	const int iHalfLen = (iCoefNum - 1) / 2;
	int iCnt;
	FLOAT_t dSum = 0;
	for (iCnt = 0; iCnt < iHalfLen; iCnt ++)
	{
		dSum += dCoeff [iCnt] * 2;	// Умножение на 2 - так как расчитана половина коэффициентов (симметричный фильтр)
	}
	dSum += dCoeff [iHalfLen];		// прогнозируемое усиление для фильтра с float операциями
	return FABSF(dSum);		// Получаем модуль усиления на постоянной составляющей.
}

// Получить усиление фильтра на частоте 0
// Массив коэффициентов для симметричного фильтра
static double testgain_float_DCL(const double * dCoeff, int iCoefNum)
{
	const int iHalfLen = (iCoefNum - 1) / 2;
	int iCnt;
	double dSum = 0;
	for (iCnt = 0; iCnt < iHalfLen; iCnt ++)
	{
		dSum += dCoeff [iCnt] * 2;	// Умножение на 2 - так как расчитана половина коэффициентов (симметричный фильтр)
	}
	dSum += dCoeff [iHalfLen];		// прогнозируемое усиление для фильтра с float операциями
	return fabs(dSum);		// Получаем модуль усиления на постоянной составляющей.
}

#if 0
	double getWindow(int i, int n) {
	    if (n == 1)
		return 1;
	    double x = 2*pi*i/(n-1);
	    double n2 = n/2; // int
	    switch (windowChooser.getSelectedIndex()) {
	    case 0: return 1; // rect
	    case 1: return .54 - .46*Math.cos(x); // hamming
	    case 2: return .5  -  .5*Math.cos(x); // hann
	    case 3: return .42 -  .5*Math.cos(x) + .08*Math.cos(2*x); // blackman
	    case 4: // kaiser 
				{
				double kaiserAlphaPi = kaiserBar.getValue()*pi/120.;
				double q = (2*i/(double) n)-1;
				return bessi0(kaiserAlphaPi*Math.sqrt(1-q*q));
				}
	    case 5: return (i < n2) ? i/n2 : 2-i/n2; // bartlett
	    case 6: { double xt = (i-n2)/n2; return 1-xt*xt; } // welch
	    }
	    return 0;
	}
#endif

	/*
	double getWindow(int i, int n) {
	    if (n == 1)
		return 1;
	    double x = 2*pi*i/(n-1);
	    double n2 = n/2; // int
	    switch (windowChooser.getSelectedIndex()) {
	    case 0: return 1; // rect
	    case 1: return .54 - .46*Math.cos(x); // hamming
	    case 2: return .5  -  .5*Math.cos(x); // hann
	    case 3: return .42 -  .5*Math.cos(x) + .08*Math.cos(2*x); // blackman
	    case 4: // kaiser 
				{
		double kaiserAlphaPi = kaiserBar.getValue()*pi/120.;
		double q = (2*i/(double) n)-1;
		return bessi0(kaiserAlphaPi*Math.sqrt(1-q*q));
	    }
	    case 5: return (i < n2) ? i/n2 : 2-i/n2; // bartlett
	    case 6: { double xt = (i-n2)/n2; return 1-xt*xt; } // welch
	    }
	    return 0;
	}
	*/

// Calculate window function (blackman-harris, hamming, rectangular)
static FLOAT_t fir_design_window(int iCnt, int iCoefNum)
{
	const int n = iCoefNum - 1;
	const FLOAT_t a = (FLOAT_t) M_TWOPI * iCnt / n;
	const FLOAT_t a2 = 2 * (FLOAT_t) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	const FLOAT_t a3 = 3 * (FLOAT_t) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	const FLOAT_t a4 = 4 * (FLOAT_t) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	const FLOAT_t a5 = 5 * (FLOAT_t) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	const FLOAT_t a6 = 6 * (FLOAT_t) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	
	switch (BOARD_WTYPE_BLACKMAN_HARRIS_MOD)
	{
	default:
	case BOARD_WTYPE_BLACKMAN_HARRIS:
		// Blackman-Harris
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.35875 
				- (FLOAT_t) 0.48829 * COSF(a) 
				+ (FLOAT_t) 0.14128 * COSF(a2) 
				- (FLOAT_t) 0.01168 * COSF(a3)
				);
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_HARRIS_MOD:
		// Modified Blackman-Harris (classic)
		// http://nsg.sourceforge.net/doc/windows/winfuns_code.php
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.35872
				- (FLOAT_t) 0.48832 * COSF(a) 
				+ (FLOAT_t) 0.14128 * COSF(a2) 
				- (FLOAT_t) 0.01168 * COSF(a3)
				);	// blackman-harris
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_HARRIS_3TERM:
		// Blackman-Harris 3-term
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.42323
				- (FLOAT_t) 0.49755 * COSF(a) 
				+ (FLOAT_t) 0.07922 * COSF(a2) 
				);	// blackman-harris
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_HARRIS_3TERM_MOD:
		// Blackman-Harris 3-term
		// in https://www.dsprelated.com/freebooks/sasp/Three_Term_Blackman_Harris_Window.html
		// 0.4243801 0.4973406 0.0782793
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.4243801
				- (FLOAT_t) 0.4973406 * COSF(a) 
				+ (FLOAT_t) 0.0782793 * COSF(a2) 
				);	// blackman-harris
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_HARRIS_7TERM:
		// Blackman-Harris 7-term
		// На float с одинарной точностью не имеет смысла переходить с BOARD_WTYPE_BLACKMAN_HARRIS
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.27105140069342
				- (FLOAT_t) 0.43329793923448 * COSF(a) 
				+ (FLOAT_t) 0.21812299954311 * COSF(a2) 
				- (FLOAT_t) 0.06592544638803 * COSF(a3) 
				+ (FLOAT_t) 0.01081174209837 * COSF(a4) 
				- (FLOAT_t) 0.00077658482522 * COSF(a5) 
				+ (FLOAT_t) 0.00001388721735 * COSF(a6)
			);
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_NUTTALL:
		// Blackman-Nuttall
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.3635819
				- (FLOAT_t) 0.4891775 * COSF(a) 
				+ (FLOAT_t) 0.1365995 * COSF(a2) 
				- (FLOAT_t) 0.0106411 * COSF(a3)
				);	
			return w;
		}
	case BOARD_WTYPE_HAMMING:
		// Hamming
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.54
				- (FLOAT_t) 0.46 * COSF(a)
				);	
			return w;
		}
	case BOARD_WTYPE_HANN:
		// Hann
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.5
				- (FLOAT_t) 0.5 * COSF(a)
				);	
			return w;
		}
	case BOARD_WTYPE_RECTANGULAR:
		// rectangular
		{
			const FLOAT_t w = 1;
			return w;
		}
	}
}


// Calculate window function (blackman-harris, hamming, rectangular)
static double fir_design_windowL(int iCnt, int iCoefNum)
{
	const int n = iCoefNum - 1;
	const double a = (double) M_TWOPI * iCnt / n;
	const double a2 = 2 * (double) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	const double a3 = 3 * (double) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	const double a4 = 4 * (double) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	const double a5 = 5 * (double) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	const double a6 = 6 * (double) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	
	switch (BOARD_WTYPE_BLACKMAN_HARRIS_MOD)
	{
	default:
	case BOARD_WTYPE_BLACKMAN_HARRIS:
		// Blackman-Harris
		{
			const double w = (
				+ (double) 0.35875
				- (double) 0.48829 * cos(a) 
				+ (double) 0.14128 * cos(a2) 
				- (double) 0.01168 * cos(a3)
			);	
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_HARRIS_MOD:
		// Modified Blackman-Harris (classic)
		// http://nsg.sourceforge.net/doc/windows/winfuns_code.php
		{
			const double w = (
				+ (double) 0.35872
				- (double) 0.48832 * cos(a) 
				+ (double) 0.14128 * cos(a2) 
				- (double) 0.01168 * cos(a3)
				);	// blackman-harris
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_HARRIS_3TERM:
		// Blackman-Harris 3-term
		{
			const double w = (
				+ (double) 0.42323
				- (double) 0.49755 * cos(a) 
				+ (double) 0.07922 * cos(a2) 
				);	// blackman-harris
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_HARRIS_3TERM_MOD:
		// Blackman-Harris 3-term
		// in https://www.dsprelated.com/freebooks/sasp/Three_Term_Blackman_Harris_Window.html
		// 0.4243801 0.4973406 0.0782793
		{
			const double w = (
				+ (double) 0.4243801
				- (double) 0.4973406 * cos(a) 
				+ (double) 0.0782793 * cos(a2) 
				);	// blackman-harris
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_HARRIS_7TERM:
		// Blackman-Harris 7-term
		// На float с одинарной точностью не имеет смысла переходить с BOARD_WTYPE_BLACKMAN_HARRIS
		{
			const double w = (
				+ (double) 0.27105140069342
				- (double) 0.43329793923448 * cos(a) 
				+ (double) 0.21812299954311 * cos(a2) 
				- (double) 0.06592544638803 * cos(a3) 
				+ (double) 0.01081174209837 * cos(a4) 
				- (double) 0.00077658482522 * cos(a5) 
				+ (double) 0.00001388721735 * cos(a6)
			);
			return w;
		}
	case BOARD_WTYPE_BLACKMAN_NUTTALL:
		// Blackman-Nuttall
		{
			const double w = (
				+ (double) 0.3635819
				- (double) 0.4891775 * cos(a) 
				+ (double) 0.1365995 * cos(a2) 
				- (double) 0.0106411 * cos(a3)
				);	
			return w;
		}
	case BOARD_WTYPE_HAMMING:
		// Hamming
		{
			const double w = (
				+ (double) 0.54
				- (double) 0.46 * cos(a)
				);	
			return w;
		}
	case BOARD_WTYPE_HANN:
		// Hann
		{
			const double w = (
				+ (double) 0.5
				- (double) 0.5 * cos(a)
				);	
			return w;
		}
	case BOARD_WTYPE_RECTANGULAR:
		// rectangular
		{
			const double w = 1;
			return w;
		}
	}
}


////////////////////////////////////////////////

// Расчёт фильтра нижних частот
// Расчёт фильтра без наложения оконной функции
static void fir_design_lowpass(FLOAT_t *dCoeff, int iCoefNum, FLOAT_t m_fCutHigh)
{
	const int iHalfLen = (iCoefNum - 1) / 2;
	//float dCoeff [iHalfLen + 1];	/* Use GCC extension */
	int iCnt;

	/*------------------*/
	/*  Lowpass filter  */
	/*------------------*/

	dCoeff [iHalfLen] = m_fCutHigh;
	for (iCnt = 1; iCnt <= iHalfLen; iCnt ++)
	{
		const FLOAT_t a = (FLOAT_t) M_PI * iCnt;
		const FLOAT_t k = SINF(m_fCutHigh * a) / a;
		dCoeff [iHalfLen - iCnt] = k;
	}
}

// Расчёт фильтра нижних частот
// Расчёт фильтра без наложения оконной функции
static void fir_design_lowpassL(double *dCoeff, int iCoefNum, double m_fCutHigh)
{
	const int iHalfLen = (iCoefNum - 1) / 2;
	//float dCoeff [iHalfLen + 1];	/* Use GCC extension */
	int iCnt;

	/*------------------*/
	/*  Lowpass filter  */
	/*------------------*/

	dCoeff [iHalfLen] = m_fCutHigh;
	for (iCnt = 1; iCnt <= iHalfLen; iCnt ++)
	{
		const double a = (double) M_PI * iCnt;
		const double k = sin(m_fCutHigh * a) / a;
		dCoeff [iHalfLen - iCnt] = k;
	}
}

// Расчёт фильтра - пробки
// ((CutLow > 0.0) && (CutHigh < 1.0) && (CutLow < CutHigh))
// Расчёт фильтра без наложения оконной функции
static void fir_design_bandstop(FLOAT_t * dCoeff, int iCoefNum, FLOAT_t fCutLow, FLOAT_t fCutHigh)
{
	const int iHalfLen = (iCoefNum - 1) / 2;
	int iCnt;

	/*------------------*/
	/* Bandstop filter  */
	/*------------------*/
	//if ((CutLow > 0.0) && (CutHigh < 1.0) && (CutLow>CutHigh)) {

	dCoeff [iHalfLen] = fCutLow - fCutHigh;   
	for (iCnt = 1; iCnt <= iHalfLen; iCnt ++) 
	{ 
		const FLOAT_t a = (FLOAT_t) M_PI_2 * iCnt;	// M_PI_2 - constant pi/2
		const FLOAT_t k = SINF((fCutLow - fCutHigh) * a) * COSF((fCutHigh + fCutLow) * a) / a;  
		dCoeff [iHalfLen - iCnt] = k;
	}   

	dCoeff [iHalfLen] += 1;   
}

// Расчёт фильтра без наложения оконной функции
static void fir_design_passtrough(FLOAT_t * dCoeff, int iCoefNum, FLOAT_t dGain)
{
	const int iHalfLen = (iCoefNum - 1) / 2;
	int iCnt;

	for (iCnt = 1; iCnt <= iHalfLen; iCnt ++)
	{
		dCoeff [iHalfLen - iCnt] = 0;
	}
	dCoeff [iHalfLen] = dGain;
}

// Без наложения оконной функции
static void fir_design_bandpass(FLOAT_t * dCoeff, int iCoefNum, FLOAT_t fCutLow, FLOAT_t fCutHigh)
{
	const int iHalfLen = (iCoefNum - 1) / 2;
	//float dCoeff [iHalfLen + 1];	/* Use GCC extension */
	//const float dGain = 1;		// Требуемое усиление фильтра
	int iCnt;

	dCoeff [iHalfLen] = fCutHigh - fCutLow;
	for (iCnt = 1; iCnt <= iHalfLen; iCnt ++)
	{
		const FLOAT_t a = (FLOAT_t) M_PI_2 * iCnt;	// M_PI_2 - constant pi/2
		const FLOAT_t k = SINF((fCutHigh - fCutLow) * a) *
										 COSF((fCutHigh + fCutLow) * a) / a;
		dCoeff [iHalfLen - iCnt] = k;
	}
}

// Наложение оконной функции
static void fir_design_applaywindow(FLOAT_t *dCoeff, const FLOAT_t *dWindow, int iCoefNum)
{
	const int j = NtapCoeffs(iCoefNum);
	int iCnt;
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		dCoeff [iCnt] *= dWindow [iCnt];
	}
}

// Наложение оконной функции
static void fir_design_applaywindowL(double *dCoeff, const double *dWindow, int iCoefNum)
{
	const int j = NtapCoeffs(iCoefNum);
	int iCnt;
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		dCoeff [iCnt] *= dWindow [iCnt];
	}
}

// подготовка буфера с оконной функцией
static void fir_design_windowbuff(FLOAT_t *dWindow, int iCoefNum)
{
	const int j = NtapCoeffs(iCoefNum);
	int iCnt;
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		dWindow [iCnt] = fir_design_window(iCnt, iCoefNum);
	}
}

// подготовка буфера с оконной функцией
static void fir_design_windowbuffL(double *dWindow, int iCoefNum)
{
	const int j = NtapCoeffs(iCoefNum);
	int iCnt;
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		dWindow [iCnt] = fir_design_windowL(iCnt, iCoefNum);
	}
}

// Масштабирование для симметричного фильтра
static void fir_design_scale(FLOAT_t * dCoeff, int iCoefNum, FLOAT_t dScale)
{
	const int j = NtapCoeffs(iCoefNum);
	int iCnt;
	if (dScale == 1)
		return;
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		dCoeff [iCnt] *= dScale;
	}
}

// Масштабирование для симметричного фильтра
static void fir_design_scaleL(double * dCoeff, int iCoefNum, double dScale)
{
	const int j = NtapCoeffs(iCoefNum);
	int iCnt;
	if (dScale == 1)
		return;
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		dCoeff [iCnt] *= dScale;
	}
}

#if 0
/* получение ограничнного размера фильтра */
static int getCoefNumLtdValidated(int iCoefNum)
{
	enum { WITHFILTSOFTDENOM = 10, WITHFILTSOFTSCALE = 8 };	// Количество коэффициентов уменьшается до 0.2 от исходного значения
	const int iCoefNumLtd = iCoefNum * WITHFILTSOFTSCALE * (glob_fltsofter - WITHFILTSOFTMIN) / (WITHFILTSOFTDENOM * (WITHFILTSOFTMAX - WITHFILTSOFTMIN));
	return NtapValidate(iCoefNum - iCoefNumLtd);
}
#endif

/* расчёт паарметров - частота для функций постоения фильтров */
#define GETNORMFREQ(freq)	((freq) * 2 / (FLOAT_t) ARMSAIRATE)
#define GETNORMFREQAUDIO(freq)	((freq) * 2 / (FLOAT_t) ARMI2SRATE)
#define GETNORMFREQAUDIOL(freq)	((freq) * 2 / (double) ARMI2SRATE)

static FLOAT_t fir_design_normfreq(int freq)
{
	return GETNORMFREQAUDIO(freq);
}

static double fir_design_normfreqL(int freq)
{
	return GETNORMFREQAUDIOL(freq);
}

static void fir_design_lowpass_freq(FLOAT_t * dCoeff, int iCoefNum, int iCutHigh)
{
	fir_design_lowpass(dCoeff, iCoefNum, fir_design_normfreq(iCutHigh));
}

// с управлением крутизной скатов и нормированием усиления, с наложением окна
static void fir_design_lowpass_freq_scaled(FLOAT_t * dCoeff, const FLOAT_t * dWindow, int iCoefNum, int iCutHigh, FLOAT_t dGain)
{
	fir_design_lowpass(dCoeff, iCoefNum, fir_design_normfreq(iCutHigh));
	fir_design_applaywindow(dCoeff, dWindow, iCoefNum);
	fir_design_scale(dCoeff, iCoefNum, dGain / testgain_float_DC(dCoeff, iCoefNum));
}

// с управлением крутизной скатов и нормированием усиления, с наложением окна
static void fir_design_lowpass_freq_scaledL(double * dCoeff, const double * dWindow, int iCoefNum, int iCutHigh, double dGain)
{
	fir_design_lowpassL(dCoeff, iCoefNum, fir_design_normfreqL(iCutHigh));
	fir_design_applaywindowL(dCoeff, dWindow, iCoefNum);
	fir_design_scaleL(dCoeff, iCoefNum, dGain / testgain_float_DCL(dCoeff, iCoefNum));
}

// Массив коэффициентов для несимметричного фильтра
static void fir_design_bandpass_freq(FLOAT_t * dCoeff, int iCoefNum, int iCutLow, int iCutHigh)
{
	fir_design_bandpass(dCoeff, iCoefNum, fir_design_normfreq(iCutLow), fir_design_normfreq(iCutHigh));
}

#if WITHDSPEXTFIR

// преобразование к целым
static void fir_design_copy_integers(int_fast32_t * lCoeff, const FLOAT_t * dCoeff, int iCoefNum)
{
	const FLOAT_t scaleout = POWF(2, HARDWARE_FIRSHIFT);
	int iCnt;
	const int j = NtapCoeffs(iCoefNum);
	// копируем результат.
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		lCoeff [iCnt] = dCoeff [iCnt] * scaleout;
	}
}

static void fir_design_integer_lowpass_scaled(int_fast32_t *lCoeff, const FLOAT_t *dWindow, int iCoefNum, int iCutHigh, FLOAT_t dGain)
{
	FLOAT_t dCoeff [NtapCoeffs(iCoefNum)];	/* Use GCC extension */
	fir_design_lowpass_freq_scaled(dCoeff, dWindow, iCoefNum, iCutHigh, dGain);	// с управлением крутизной скатов и нормированием усиления, с наложением окна
	fir_design_copy_integers(lCoeff, dCoeff, iCoefNum);
}

// преобразование к целым
static void fir_design_copy_integersL(int_fast32_t * lCoeff, const double * dCoeff, int iCoefNum)
{
	const double scaleout = pow(2, HARDWARE_FIRSHIFT);
	int iCnt;
	const int j = NtapCoeffs(iCoefNum);
	// копируем результат.
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		lCoeff [iCnt] = dCoeff [iCnt] * scaleout;
	}
}

static void fir_design_integer_lowpass_scaledL(int_fast32_t *lCoeff, const double *dWindow, int iCoefNum, int iCutHigh, double dGain)
{
	double dCoeff [NtapCoeffs(iCoefNum)];	/* Use GCC extension */
	fir_design_lowpass_freq_scaledL(dCoeff, dWindow, iCoefNum, iCutHigh, dGain);	// с управлением крутизной скатов и нормированием усиления, с наложением окна
	fir_design_copy_integersL(lCoeff, dCoeff, iCoefNum);
}

static void fir_design_integers_passtrough(int_fast32_t *lCoeff, int iCoefNum, FLOAT_t dGain)
{
	FLOAT_t dCoeff [NtapCoeffs(iCoefNum)];	/* Use GCC extension */
	fir_design_passtrough(dCoeff, iCoefNum, dGain);
	fir_design_copy_integers(lCoeff, dCoeff, iCoefNum);
}

#endif /* WITHDSPEXTFIR */

#if 0
// debug function
static void writecoefs(const int_fast32_t * lCoeff, int iCoefNum)
{
	debug_printf_P(PSTR("# iCoefNum = %i\n"), iCoefNum);
	const int iHalfLen = (iCoefNum - 1) / 2;
	int i;
	for (i = 0; i <= iHalfLen; ++ i)
	{
		debug_printf_P(PSTR("%ld\n"), lCoeff [i]);
	}
	i -= 1;
	for (; -- i >= 0; )
	{
		debug_printf_P(PSTR("%ld\n"), lCoeff [i]);
	}
	debug_printf_P(PSTR("# end\n"));
}
#endif

/*-----------------------------------*/

// float32 * float32 -> float32
static RAMFUNC FLOAT32P_t scalepair(FLOAT32P_t a, FLOAT_t b)
{
	a.IV *= b;
	a.QV *= b;
	return a;
}

#if 0

// int32 * int32 -> int32
static RAMFUNC FLOAT32P_t scalepair_int32(INT32P_t a, int_fast32_t b)
{
	FLOAT32P_t v;
	const int_fast64_t bi = b;
	v.IV = (a.IV * bi) >> 31;
	v.QV = (a.QV * bi) >> 31;
	return v;
}
#endif

#if WITHDSPLOCALFIR


#if WITHDSPEXTDDC
// Фильтр квадратурных каналов приёмника
// Используется в случае внешнего DDCV
static RAMFUNC_NONILINE FLOAT32P_t filter_firp_rx_SSB_IQ(FLOAT32P_t NewSample)
{
	const FLOAT_t * const k = FIRCoef_rx_SSB_IQ [gwprof];
	enum { Ntap = Ntap_rx_SSB_IQ, NtapHalf = Ntap / 2 };
	// буфер с сохраненными значениями сэмплов
	static FLOAT32P_t x [Ntap * 2] = { { { 0, 0 }, }, };
	static uint_fast16_t fir_head = 0;

	// shift the old samples
	// fir_head -  Начало обрабатываемой части буфера
	// fir_head + Ntap -  Позиция за концом обрабатываемого буфер
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
    x [fir_head] = x [fir_head + Ntap] = NewSample;

	uint_fast16_t bh = fir_head + NtapHalf;			// Начало обрабатываемой части буфера
	uint_fast16_t bt = bh;	// Позиция за концом обрабатываемого буфера
    // Calculate the new output
	uint_fast16_t n = NtapHalf;
	// Выборка в середине буфера
	FLOAT32P_t v = scalepair(x [bh], k [n]);            // sample at middle of buffer
	do
	{	
		{
			const FLOAT_t kv = k [-- n];
			v.IV += kv * (x [-- bh].IV + x [++ bt].IV);
			v.QV += kv * (x [bh].QV + x [bt].QV);
		}
		{
			const FLOAT_t kv = k [-- n];
			v.IV += kv * (x [-- bh].IV + x [++ bt].IV);
			v.QV += kv * (x [bh].QV + x [bt].QV);
		}
	}
	while (n != 0);

    return v;
}
// Фильтр квадратурных каналов передатчика
// Используется в случае внешнего DUC
static RAMFUNC_NONILINE FLOAT32P_t filter_firp_tx_SSB_IQ(FLOAT32P_t NewSample)
{
	const FLOAT_t * const k = FIRCoef_tx_SSB_IQ [gwprof];
	enum { Ntap = Ntap_tx_SSB_IQ, NtapHalf = Ntap / 2 };
	// буфер с сохраненными значениями сэмплов
	static FLOAT32P_t x [Ntap * 2] = { { { 0, 0 }, }, };
	static uint_fast16_t fir_head = 0;

	// shift the old samples
	// fir_head -  Начало обрабатываемой части буфера
	// fir_head + Ntap -  Позиция за концом обрабатываемого буфер
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
    x [fir_head] = x [fir_head + Ntap] = NewSample;

	uint_fast16_t bh = fir_head + NtapHalf;			// Начало обрабатываемой части буфера
	uint_fast16_t bt = bh;	// Позиция за концом обрабатываемого буфера
    // Calculate the new output
	uint_fast16_t n = NtapHalf;
	// Выборка в середине буфера
	FLOAT32P_t v = scalepair(x [bh], k [n]);            // sample at middle of buffer
	do
	{	
		{
			const FLOAT_t kv = k [-- n];
			v.IV += kv * (x [-- bh].IV + x [++ bt].IV);
			v.QV += kv * (x [bh].QV + x [bt].QV);
		}
		{
			const FLOAT_t kv = k [-- n];
			v.IV += kv * (x [-- bh].IV + x [++ bt].IV);
			v.QV += kv * (x [bh].QV + x [bt].QV);
		}
	}
	while (n != 0);

    return v;
}

#else  /* WITHDSPEXTDDC */
// Приёмник
// Фильтр (floating point) для квадратурных сигналов. Оптимизация под multiplier 0 1 0 -1.
// Speed tests: DT=145 (129), ntaps=481

static RAMFUNC_NONILINE FLOAT32P_t filter_fir4_rx_SSB_IQ(FLOAT32P_t NewSample, uint_fast8_t useI) 
{
	const FLOAT_t * const k = FIRCoef_rx_SSB_IQ [gwprof];
	enum { Ntap = Ntap_rx_SSB_IQ, NtapHalf = Ntap / 2 };
	FLOAT32P_t rv;

	// буфер с сохраненными значениями сэмплов
	static FLOAT32P_t x [Ntap * 2] = { { { 0, 0, }, }, }; // input samples (force CCM allocation)
	static uint_fast16_t fir_head = 0;		// позиция записи в буфер в последний раз

	// shift the old samples
	// fir_head -  Начало обрабатываемой части буфера
	// fir_head + Ntap -  Позиция за концом обрабатываемого буфер
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
	const uint_fast16_t fir_tail = fir_head + Ntap;
	// Calculate the new output

	uint_fast16_t n;
	if (useI != 0)
	{
		// Если текущий == 0 - его и все через один игнорируем
		x [fir_head].IV = NewSample.IV;
		x [fir_tail].IV = NewSample.IV;
		//
		// Суммирование I
		//
		// Выборка в середине буфера
		rv.IV = k [NtapHalf] * x [fir_head + NtapHalf].IV;
		uint_fast16_t bh = fir_head;	// Начало обрабатываемой части буфера
		uint_fast16_t bt = fir_tail;	// Позиция за концом обрабатываемого буфера
		// NewSample.QV здесь равно 0 - можно не суммировать
		n = 0;
		do
		{	
			// Вычисляем I
			// 1_23 * 1_31 = 1_54. Это после умножения на FIRCoef.
			rv.IV += k [n ++] * (x [bh ++].IV + x [-- bt].IV);
			// Пропускаем Q
			++ n;
			++ bh;
			-- bt;
		}
		while (n < NtapHalf);
		//
		// Суммирование Q
		//
		rv.QV = 0;
		bh = fir_head;			// Начало обрабатываемой части буфера
		bt = fir_tail;			// Позиция за концом обрабатываемого буфера
		n = 0;
		do
		{	
			// Пропускаем I
			++ n;
			++ bh;
			-- bt;
			// Вычисляем Q
			// 1_23 * 1_31 = 1_54. Это после умножения на FIRCoef.
			rv.QV += k [n ++] * (x [bh ++].QV + x [-- bt].QV);
		}
		while (n < NtapHalf);
	}
	else
	{
		// Если текущий == 0 - его и все через один игнорируем
		x [fir_head].QV = NewSample.QV;
		x [fir_tail].QV = NewSample.QV;
		//
		// Суммирование Q
		//
		// Выборка в середине буфера
		rv.QV = k [NtapHalf] * x [fir_head + NtapHalf].QV;
		uint_fast16_t bh = fir_head;	// Начало обрабатываемой части буфера
		uint_fast16_t bt = fir_tail;	// Позиция за концом обрабатываемого буфера
		// NewSample.IV здесь равно 0 - можно не суммировать
		n = 0;
		do
		{	
			// Вычисляем Q
			// 1_23 * 1_31 = 1_54. Это после умножения на FIRCoef.
			rv.QV += k [n ++] * (x [bh ++].QV + x [-- bt].QV);
			// Пропускаем I
			++ n;
			++ bh;
			-- bt;
		}
		while (n < NtapHalf);
		//
		// Суммирование I
		//
		rv.IV = 0;
		bh = fir_head;			// Начало обрабатываемой части буфера
		bt = fir_tail;			// Позиция за концом обрабатываемого буфера
		n = 0;
		do
		{	
			// Пропускаем Q
			++ n;
			++ bh;
			-- bt;
			// Вычисляем I
			// 1_23 * 1_31 = 1_54. Это после умножения на FIRCoef.
			rv.IV += k [n ++] * (x [bh ++].IV + x [-- bt].IV);
		}
		while (n < NtapHalf);
	}
    return rv;
}

// Фильтр квадратурных каналов передатчика. Оптимизация под multiplier 0 1 0 -1.
// Результаты интересуют не все, а в зависимости от useI
static RAMFUNC_NONILINE FLOAT32P_t filter_fir4_tx_SSB_IQ(FLOAT32P_t NewSample, uint_fast8_t useI) 
{
	const FLOAT_t * const k = FIRCoef_tx_SSB_IQ [gwprof];
	enum { Ntap = Ntap_tx_SSB_IQ, NtapHalf = Ntap / 2 };
	// буфер с сохраненными значениями сэмплов
	static FLOAT32P_t x [Ntap * 2] = { { { 0, 0, }, }, }; // input samples (force CCM allocation)
	static uint_fast16_t fir_head = 0;

	// shift the old samples
	// fir_head -  Начало обрабатываемой части буфера
	// fir_head + Ntap -  Позиция за концом обрабатываемого буфер
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);

	// Если текущий == 0 - его и все через один игнорируем
	uint_fast16_t bh = fir_head;			// Начало обрабатываемой части буфера
	uint_fast16_t bt = fir_head + Ntap;	// Позиция за концом обрабатываемого буфера
    // Calculate the new output
    x [bh] = NewSample;
    x [bt] = NewSample;
	uint_fast16_t n = 0;
	FLOAT32P_t v = { { 0, 0 }, };
	if (useI != 0)
	{
		// На выходе интересует только I
		do
		{	
			v.IV += k [n ++] * (x [bh ++].IV + x [-- bt].IV);
			v.IV += k [n ++] * (x [bh ++].IV + x [-- bt].IV);
		}
		while (n < NtapHalf);
		// Выборка в середине буфера
		v.IV += k [NtapHalf] * x [bh].IV;
	}
	else
	{
		// На выходе интересует только Q
		do
		{	
			v.QV += k [n ++] * (x [bh ++].QV + x [-- bt].QV);
			v.QV += k [n ++] * (x [bh ++].QV + x [-- bt].QV);
		}
		while (n < NtapHalf);
		// Выборка в середине буфера
		v.QV += k [NtapHalf] * x [bh].QV;
	}

    return v;
}
#endif /* WITHDSPEXTDDC */

#endif /* WITHDSPLOCALFIR */

/* Выполнение обработки в симметричном FIR фильтре */
static RAMFUNC FLOAT_t filter_fir_compute(const FLOAT_t * const pk0, const FLOAT_t * xbh, uint_fast16_t n)
{
	const FLOAT_t * xbt = xbh;		// позиция справа от центра
    // Calculate the new output
	// Выборка в середине буфера
	FLOAT_t v = pk0 [-- n] * * -- xbh;             // sample at middle of buffer
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
	float32x4_t v4 = vdupq_n_f32(0);
#endif /* __ARM_NEON_FP */
	do
	{	
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
		// получиь значения из левой половины
		xbh -= 4;
		const float32x4_t vh = vld1q_f32(xbh);
		// получить значения из правой половины (требуется измененить порядок на противоположный)
		const float32x4_t vt = vld1q_f32(xbt);
		xbt += 4;
		/* суммируем левую с правой */
		const float32x4_t va = vaddq_f32(
			vrev64q_f32(vcombine_f32(vget_high_f32(vt), vget_low_f32(vt))), // меняем порядок на противоположный
			vh);
		/* коэффициенты */
		n -= 4;
		const float32x4_t vk = vld1q_f32(& pk0 [n]);
		/* умножение с накоплением */
		v4 = vmlaq_f32(v4, va, vk);
#elif defined (FMAF)
		v = FMAF(pk0 [-- n], * -- xbh + * xbt ++, v);	/* вычисление результата без потери точности из-за округления после умножения */
#else /* defined (FMAF) */
		v += pk0 [-- n] * (* -- xbh + * xbt ++);
#endif /* defined (FMAF) */
	}
	while (n != 0);
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
	v += vgetq_lane_f32(v4, 0);
	v += vgetq_lane_f32(v4, 1);
	v += vgetq_lane_f32(v4, 2);
	v += vgetq_lane_f32(v4, 3);
#endif /* __ARM_NEON_FP */

	return v;
}

#if WITHUSEDUALWATCH && DUALFILTERSPROCESSING

/* Выполнение обработки в симметричном FIR фильтре */
// фильтрация пар значений одинаковым фильтром
static RAMFUNC_NONILINE FLOAT32P_t filter_fir_compute_Pair(const FLOAT_t * const pk0, const FLOAT32P_t * xbh, uint_fast16_t n)
{
	const FLOAT32P_t * xbt = xbh;		// позиция справа от центра
    // Calculate the new output
	// Выборка в середине буфера
	const FLOAT32P_t t1 = * -- xbh;
	const FLOAT_t k = pk0 [-- n];
	FLOAT32P_t v = { { k * t1.IV, k * t1.QV } };             // sample at middle of buffer
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
	float32x4_t v4 = vdupq_n_f32(0);	// lane 0, 2, IV values, lane 1, 3: QV values
#endif /* __ARM_NEON_FP */
	do
	{	
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
		// получиь значения из левой половины
		xbh -= 2;
		const float32x4_t vh = vcombine_f32(vld1_f32(xbh [0].ivqv), vld1_f32(xbh [1].ivqv));	// lane 0, 2, IV values, lane 1, 3: QV values
		// получиь значения из правой половины (в обратном порядке - приводим к том же порядку, что и коэффициенты)
		const float32x4_t vt = vcombine_f32(vld1_f32(xbt [1].ivqv), vld1_f32(xbt [0].ivqv));	// lane 0, 2, IV values, lane 1, 3: QV values
		xbt += 2;
		/* суммируем левую с правой */
		const float32x4_t va = vaddq_f32(
			vt,
			vh);
		/* коэффициенты */
		n -= 2;
		const float32x4_t vk = vcombine_f32(vdup_n_f32(pk0 [n + 0]), vdup_n_f32(pk0 [n + 1]));	// lane 0, 2: IV values, lane 1, 3: QV values
		/* умножение с накоплением */
		v4 = vmlaq_f32(v4, va, vk);
#elif defined (FMAF)
		const FLOAT32P_t t1 = * -- xbh;
		const FLOAT32P_t t2 = * xbt ++;
		const FLOAT_t k = pk0 [-- n];
		v.IV = FMAF(k, t1.IV + t2.IV, v.IV);	/* вычисление результата без потери точности из-за округления после умножения */
		v.QV = FMAF(k, t1.QV + t2.QV, v.QV);	/* вычисление результата без потери точности из-за округления после умножения */
#else /* defined (FMAF) */
		const FLOAT32P_t t1 = * -- xbh;
		const FLOAT32P_t t2 = * xbt ++;
		const FLOAT_t k = pk0 [-- n];
		v.IV += k * (t1.IV + t2.IV);
		v.QV += k * (t1.QV + t2.QV);
#endif /* defined (FMAF) */
	}
	while (n != 0);
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
	v.IV += vgetq_lane_f32(v4, 0);	// IV values
	v.IV += vgetq_lane_f32(v4, 2);	// IV values
	v.QV += vgetq_lane_f32(v4, 1);	// QV values
	v.QV += vgetq_lane_f32(v4, 3);	// QV values
#endif /* __ARM_NEON_FP */

	return v;
}


/* Выполнение обработки в симметричном FIR фильтре */
// фильтрация пар значений разными фильтрами
static RAMFUNC_NONILINE FLOAT32P_t filter_fir_compute_Pair2(const FLOAT_t * const pk0, const FLOAT_t * const pk1, const FLOAT32P_t * xbh, uint_fast16_t n)
{
	const FLOAT32P_t * xbt = xbh;		// позиция справа от центра
    // Calculate the new output
	// Выборка в середине буфера
	const FLOAT32P_t t1 = * -- xbh;
	const FLOAT_t k0 = pk0 [-- n];
	const FLOAT_t k1 = pk1 [n];
	FLOAT32P_t v = { { k0 * t1.IV, k1 * t1.QV } };             // sample at middle of buffer
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
	float32x4_t v4 = vdupq_n_f32(0);	// lane 0, 2, IV values, lane 1, 3: QV values
#endif /* __ARM_NEON_FP */
	do
	{	
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
		// получиь значения из левой половины
		xbh -= 2;
		const float32x4_t vh = vcombine_f32(vld1_f32(xbh [0].ivqv), vld1_f32(xbh [1].ivqv));	// lane 0, 2, IV values, lane 1, 3: QV values
		// получиь значения из правой половины (в обратном порядке - приводим к том же порядку, что и коэффициенты)
		const float32x4_t vt = vcombine_f32(vld1_f32(xbt [1].ivqv), vld1_f32(xbt [0].ivqv));	// lane 0, 2, IV values, lane 1, 3: QV values
		xbt += 2;
		/* суммируем левую с правой */
		const float32x4_t va = vaddq_f32(
			vt,
			vh);
		/* коэффициенты */
		n -= 2;
		const float32_t vks [4] = { pk0 [n + 0], pk1 [n + 0], pk0 [n + 1], pk1 [n + 1] };
		//const float32x4_t vk = vld1q_f32(vls);
		/* умножение с накоплением */
		v4 = vmlaq_f32(v4, va, vld1q_f32(vks));
#elif defined (FMAF)
		const FLOAT32P_t t1 = * -- xbh;
		const FLOAT32P_t t2 = * xbt ++;
		const FLOAT_t k0 = pk0 [-- n];
		const FLOAT_t k1 = pk1 [n];
		v.IV = FMAF(k0, t1.IV + t2.IV, v.IV);	/* вычисление результата без потери точности из-за округления после умножения */
		v.QV = FMAF(k1, t1.QV + t2.QV, v.QV);	/* вычисление результата без потери точности из-за округления после умножения */
#else /* defined (FMAF) */
		const FLOAT32P_t t1 = * -- xbh;
		const FLOAT32P_t t2 = * xbt ++;
		const FLOAT_t k0 = pk0 [-- n];
		const FLOAT_t k1 = pk1 [n];
		v.IV += k0 * (t1.IV + t2.IV);
		v.QV += k1 * (t1.QV + t2.QV);
#endif /* defined (FMAF) */
	}
	while (n != 0);
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24
	v.IV += vgetq_lane_f32(v4, 0);	// IV values
	v.IV += vgetq_lane_f32(v4, 2);	// IV values
	v.QV += vgetq_lane_f32(v4, 1);	// QV values
	v.QV += vgetq_lane_f32(v4, 3);	// QV values
#endif /* __ARM_NEON_FP */

	return v;
}

#endif /* DUALFILTERSPROCESSING */

// Фильтр микрофона передатчика
static RAMFUNC_NONILINE FLOAT_t filter_fir_tx_MIKE(FLOAT_t NewSample, uint_fast8_t bypass) 
{
	enum { Ntap = Ntap_tx_MIKE, NtapHalf = Ntap / 2 };
	// буфер с сохраненными значениями сэмплов
	static FLOAT_t xshift [Ntap * 2] = { 0, };
	static uint_fast16_t fir_head = 0;

	// shift the old samples
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
    xshift [fir_head] = xshift [fir_head + Ntap] = NewSample;

	return bypass ? xshift [fir_head + NtapHalf] : filter_fir_compute(FIRCoef_tx_MIKE [gwprof], & xshift [fir_head + NtapHalf + 1], NtapHalf + 1);
}

// Звуковой фильтр приёмника.
static RAMFUNC_NONILINE FLOAT_t filter_fir_rx_AUDIO_A(FLOAT_t NewSample) 
{
	enum { Ntap = Ntap_rx_AUDIO, NtapHalf = Ntap / 2 };
	// буфер с сохраненными значениями сэмплов
	static FLOAT_t xshift [Ntap * 2] = { 0, };
	static uint_fast16_t fir_head = 0;

	// shift the old samples
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
    xshift [fir_head] = xshift [fir_head + Ntap] = NewSample;

	return filter_fir_compute(FIRCoef_rx_AUDIO [gwprof] [0], & xshift [fir_head + NtapHalf + 1], NtapHalf + 1);
}

#if WITHUSEDUALWATCH

// Звуковой фильтр приёмника.
static RAMFUNC_NONILINE FLOAT_t filter_fir_rx_AUDIO_B(FLOAT_t NewSample) 
{
#if ! DUALFILTERSPROCESSING
	return NewSample;
#endif /* ! DUALFILTERSPROCESSING */

	enum { Ntap = Ntap_rx_AUDIO, NtapHalf = Ntap / 2 };
	// буфер с сохраненными значениями сэмплов
	static FLOAT_t xshift [Ntap * 2] = { 0, };
	static uint_fast16_t fir_head = 0;

	// shift the old samples
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
    xshift [fir_head] = xshift [fir_head + Ntap] = NewSample;

	return filter_fir_compute(FIRCoef_rx_AUDIO [gwprof] [1], & xshift [fir_head + NtapHalf + 1], NtapHalf + 1);
}

#if DUALFILTERSPROCESSING
// Звуковой фильтр приёмника.
// фильтрация пар значений одинаковым фильтром
static RAMFUNC_NONILINE FLOAT32P_t filter_fir_rx_AUDIO_Pair(FLOAT32P_t NewSample) 
{
	enum { Ntap = Ntap_rx_AUDIO, NtapHalf = Ntap / 2 };
	// буфер с сохраненными значениями сэмплов
	static FLOAT32P_t xshift [Ntap * 2] = { 0, };
	static uint_fast16_t fir_head = 0;

	// shift the old samples
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
    xshift [fir_head] = xshift [fir_head + Ntap] = NewSample;

	return filter_fir_compute_Pair(FIRCoef_rx_AUDIO [gwprof] [0], & xshift [fir_head + NtapHalf + 1], NtapHalf + 1);
}

// Звуковой фильтр приёмника.
// фильтрация пар значений разными фильтрами
static RAMFUNC_NONILINE FLOAT32P_t filter_fir_rx_AUDIO_Pair2(FLOAT32P_t NewSample) 
{
	enum { Ntap = Ntap_rx_AUDIO, NtapHalf = Ntap / 2 };
	// буфер с сохраненными значениями сэмплов
	static FLOAT32P_t xshift [Ntap * 2] = { 0, };
	static uint_fast16_t fir_head = 0;

	// shift the old samples
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
    xshift [fir_head] = xshift [fir_head + Ntap] = NewSample;

	return filter_fir_compute_Pair2(FIRCoef_rx_AUDIO [gwprof] [0], FIRCoef_rx_AUDIO [gwprof] [1], & xshift [fir_head + NtapHalf + 1], NtapHalf + 1);
}
#endif /* DUALFILTERSPROCESSING */

#endif /* WITHUSEDUALWATCH */


static RAMFUNC uint_fast8_t isdspmoderx(uint_fast8_t dspmode)
{
	return dspmode > DSPCTL_MODE_RX_BASE && dspmode < DSPCTL_MODE_RX_TOP;
}

static RAMFUNC uint_fast8_t isdspmodetx(uint_fast8_t dspmode)
{
	return dspmode > DSPCTL_MODE_TX_BASE && dspmode < DSPCTL_MODE_TX_TOP;
}

static int inside(int low, int val, int high)
{
	if (low > val)
		return low;
	if (high < val)
		return high;
	return val;
}

// Установка параметров тракта приёмника
static void audio_setup_wiver(const uint_fast8_t spf, const uint_fast8_t pathi)
{
	const uint_fast8_t dspmode = glob_dspmodes [pathi];
	const uint_fast16_t fullbw6 = glob_fullbw6 [pathi];
#if WITHDSPEXTDDC
	#if WITHDSPLOCALFIR
		const FLOAT_t rxfiltergain = 1;
		const FLOAT_t txfiltergain = 1;
	#endif /* WITHDSPLOCALFIR */
#else /* WITHDSPEXTDDC */
	const FLOAT_t rxfiltergain = 2;	// Для IQ фильтра можно так - для компенсации 0.5 усиления из-за перемножителя перед ним.
	const FLOAT_t txfiltergain = 2;	// Для IQ фильтра можно так - для компенсации 0.5 усиления из-за перемножителя перед ним.
#endif /* WITHDSPEXTDDC */

	const int cutfreq = fullbw6 / 2;
	//debug_printf_P(PSTR("audio_setup_wiver: fullbw6=%u\n"), (unsigned) fullbw6);

	if (fullbw6 == INT16_MAX)
	{
		//debug_printf_P(PSTR("audio_setup_wiver: construct bypass glob_fullbw6=%u\n"), (unsigned) glob_fullbw6);
	#if WITHDSPLOCALFIR
		if (isdspmoderx(dspmode))
			fir_design_passtrough(FIRCoef_rx_SSB_IQ [spf], Ntap_rx_SSB_IQ, rxfiltergain);
		else if (isdspmodetx(dspmode))
			fir_design_passtrough(FIRCoef_tx_SSB_IQ [spf], Ntap_tx_SSB_IQ, txfiltergain);
	#else /* WITHDSPLOCALFIR */
		(void) dspmode;
		fir_design_integers_passtrough(FIRCoef_trxi_IQ, Ntap_trxi_IQ, 1);
	#endif /* WITHDSPLOCALFIR */
	}
	else
	{
		//debug_printf_P(PSTR("audio_setup_wiver: construct filter glob_fullbw6=%u\n"), (unsigned) glob_fullbw6);
	#if WITHDSPLOCALFIR
		if (isdspmoderx(dspmode))
			fir_design_lowpass_freq_scaled(FIRCoef_rx_SSB_IQ [spf], FIRCwnd_rx_SSB_IQ, Ntap_rx_SSB_IQ, cutfreq, rxfiltergain);	// с управлением крутизной скатов и нормированием усиления, с наложением окна
		else if (isdspmodetx(dspmode))
			fir_design_lowpass_freq_scaled(FIRCoef_tx_SSB_IQ [spf], FIRCwnd_tx_SSB_IQ, Ntap_tx_SSB_IQ, cutfreq, txfiltergain);	// с управлением крутизной скатов и нормированием усиления, с наложением окна
	#else /* WITHDSPLOCALFIR */
		(void) dspmode;
		#if WITHDOUBLEFIRCOEFS && (__ARM_FP & 0x08)
			fir_design_integer_lowpass_scaledL(FIRCoef_trxi_IQ, FIRCwndL_trxi_IQ, Ntap_trxi_IQ, cutfreq, 1);
		#else
			fir_design_integer_lowpass_scaled(FIRCoef_trxi_IQ, FIRCwnd_trxi_IQ, Ntap_trxi_IQ, cutfreq, 1);
		#endif
	#endif /* WITHDSPLOCALFIR */
	}

#if WITHDSPEXTFIR && WITHDSPLOCALFIR
	// если есть и внешний и внутренний фильтр - внешний перводится в режим passtrough - для тестирования
	fir_design_integers_passtrough(FIRCoef_trxi_IQ, Ntap_trxi_IQ, 1);
#endif /* WITHDSPEXTDDC && WITHDSPLOCALFIR */

#if WITHDSPEXTDDC && WITHDSPEXTFIR
	// загрузка коэффициентов фильтра в FPGA
	//writecoefs(FIRCoef_trxi_IQ, Ntap_trxi_IQ);	/* печать коэффициентов фильтра */
	board_fpga_fir_send(pathi, FIRCoef_trxi_IQ, Ntap_trxi_IQ);		/* загрузить массив коэффициентов в FPGA */
	boart_tgl_firprofile(pathi);
#endif /* WITHDSPEXTDDC && WITHDSPEXTFIR */
}

// Установка параметров тракта приёмника
static void audio_setup_rx(const uint_fast8_t spf, const uint_fast8_t pathi)
{
	const int cutfreqlow = glob_aflowcutrx [pathi];
	const int cutfreqhigh = glob_afhighcutrx [pathi];
	FLOAT_t * const dCoeff = FIRCoef_rx_AUDIO [spf] [pathi];
	const FLOAT_t * const dWindow = FIRCwnd_rx_AUDIO;
	enum { iCoefNum = Ntap_rx_AUDIO };

	switch (glob_dspmodes [0])
	{
	case DSPCTL_MODE_RX_DSB:
		// В этом режиме фильтр не используется
		//fir_design_passtrough(dCoeff, iCoefNum, 1);
		// ФНЧ
		fir_design_lowpass_freq(dCoeff, iCoefNum, cutfreqhigh);
		fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 1);	// Формирование наклона АЧХ
		break;

	case DSPCTL_MODE_RX_SAM:
		// ФНЧ
		//fir_design_lowpass_freq(dCoeff, iCoefNum, cutfreqhigh);
		fir_design_bandpass_freq(dCoeff, iCoefNum, cutfreqlow, cutfreqhigh);
		fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 1);	// Формирование наклона АЧХ
		break;

	case DSPCTL_MODE_RX_WFM:
	case DSPCTL_MODE_RX_AM:
	case DSPCTL_MODE_RX_WIDE:
		// audio
		if (glob_notch_on != 0)
		{
			// частоты SSB фильтра
			//const int fssbL = cutfreqlow;
			//const int fssbH = cutfreqhigh;

			// Частоты NOTCH фильтра
			const int fNotch = inside(cutfreqlow + glob_notch_width / 2, glob_notch_freq, cutfreqhigh - glob_notch_width / 2);
			const int fcutL = fNotch - glob_notch_width / 2;
			const int fcutH = fNotch + glob_notch_width / 2;
			if (0)
			{
				// Расчитывается Notch
				fir_design_bandstop(dCoeff, iCoefNum, fir_design_normfreq(fcutL), fir_design_normfreq(fcutH));
				fir_design_scale(dCoeff, iCoefNum, 1 / testgain_float_DC(dCoeff, iCoefNum));	// Масштабирование для несимметричного фильтра
				fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 0);	// Формирование наклона АЧХ, без применения оконной функции
			}
			else
			{
				FLOAT_t dC2 [NtapCoeffs(iCoefNum)];
				int i;
				// расчитывается два неперекрывающихся полосовых фильтра
				fir_design_bandpass_freq(dC2, iCoefNum, cutfreqlow, fcutL);	// низкочастотная полоса пропускания
				fir_design_bandpass_freq(dCoeff, iCoefNum, fcutH, cutfreqhigh);	// высокочастотная полоса пропускания
				// суммирование эоэффициентов
				for (i = 0; i < NtapCoeffs(iCoefNum); ++ i)
					dCoeff [i] += dC2 [i];
				fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 0);	// Формирование наклона АЧХ, без применения оконной функции
			}
		}
		else
		{
			fir_design_bandpass_freq(dCoeff, iCoefNum, cutfreqlow, cutfreqhigh);
			fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 1);	// Формирование наклона АЧХ
		}
		break;

	case DSPCTL_MODE_RX_NARROW:
	case DSPCTL_MODE_RX_FREEDV:
		// audio
		fir_design_bandpass_freq(dCoeff, iCoefNum, cutfreqlow, cutfreqhigh);
		fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 1);	// Формирование наклона АЧХ
		break;

	case DSPCTL_MODE_RX_DRM:
		// audio
		// В этом режиме фильтр не используется
		fir_design_passtrough(dCoeff, iCoefNum, 1);		// сигнал через НЧ фильтр не проходит
		break;


	case DSPCTL_MODE_RX_NFM:
		// audio
		fir_design_bandpass_freq(dCoeff, iCoefNum, cutfreqlow, cutfreqhigh);
		fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 1);	// Формирование наклона АЧХ
		NBTAU = 1 - MAKETAUIF((FLOAT_t) 0.1);
		break;

	// в режиме передачи
	default:
		break;
	}
}


// Установка параметров тракта передатчика
static void audio_setup_mike(const uint_fast8_t spf)
{
	FLOAT_t * const dCoeff = FIRCoef_tx_MIKE [spf];
	const FLOAT_t * const dWindow = FIRCwnd_tx_MIKE;
	enum { iCoefNum = Ntap_tx_MIKE };

	switch (glob_dspmodes [0])
	{
	case DSPCTL_MODE_TX_BPSK:
		// 15-ти герцовый ФНЧ перед модулятором - из "канонического" описания модуляции в статье в QEX July/Aug 1999 7 - x9907003.pdf
		fir_design_lowpass_freq(dCoeff, iCoefNum, 15);
		fir_design_scale(dCoeff, iCoefNum, 1 / testgain_float_DC(dCoeff, iCoefNum));	// Масштабирование для несимметричного фильтра
		fir_design_applaywindow(dCoeff, dWindow, iCoefNum);
		break;

	// Голосовые режимиы
	case DSPCTL_MODE_TX_NFM:
	case DSPCTL_MODE_TX_SSB:
	case DSPCTL_MODE_TX_AM:
	case DSPCTL_MODE_TX_FREEDV:
		fir_design_bandpass_freq(dCoeff, iCoefNum, glob_aflowcuttx, glob_afhighcuttx);
		fir_design_adjust_tx(dCoeff, dWindow, iCoefNum);	// Применение эквалайзера к микрофону
		break;

	// в режиме приема или в режимах передачи без микрофона - ничего не делаем
	default:
		fir_design_passtrough(dCoeff, iCoefNum, 1);
		break;
	}
}

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
		CTLSTYLE_OLEG4Z_V1 || \
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

// установить частоты среза тракта ПЧ
// Вызывается из пользовательской программы, но может быть вызвана и до инициализации DSP - вызывается из updateboard.
static void audio_update(const uint_fast8_t spf, uint_fast8_t pathi)
{
	globDSPMode  [spf] [pathi] = glob_dspmodes [pathi];
	//debug_printf_P(PSTR("nco_setlo_ftw: freq=%d\n"), glob_lo6A);
	const ncoftw_t lo6_ftw = FTWAF(- glob_lo6 [pathi]);
	nco_setlo_ftw(lo6_ftw, pathi);	// Приёмник #0

#if WITHLOOPBACKTEST
	fir_design_bandpass_freq(FIRCoef_tx_MIKE [spf], Ntap_tx_MIKE, glob_aflowcuttx, glob_afhighcuttx);
	fir_design_applaywindow(FIRCoef_tx_MIKE [spf], FIRCwnd_tx_MIKE, Ntap_tx_MIKE);
#endif /* WITHLOOPBACKTEST */


	// второй фильтр грузится только в режиме приёма (обеспечиватся внешним циклом).
	audio_setup_wiver(spf, pathi);	/* Установка параметров ФНЧ в тракте обработки сигнала алгоритм Уивера */

	audio_setup_rx(spf, pathi);

	debug_cleardtmax();		// сброс максимального значения в тесте производительности DSP

#if 0
	debug_printf_P(PSTR("audio_update[pathi=%d]: dsp_mode=%d, bw6=%d, lo6=%d, rx=%d..%d, tx=%d..%d\n"), 
		(int) pathi, 
		(int) glob_dspmodes [pathi], 
		(int) glob_fullbw6 [pathi], 
		(int) glob_lo6 [pathi], 
		glob_aflowcutrx [pathi], glob_afhighcutrx [pathi], 
		glob_aflowcuttx, glob_afhighcuttx
		);
#endif
}

static void modem_update(void)
{
#if WITHMODEM
	modem_set_speed(glob_modem_speed100);
	modem_set_mode(glob_modem_mode);

	debug_printf_P(PSTR("modem_update: modem_speed100=%d, modem_mode\n"), (int) (glob_modem_speed100 / 100), (int) (glob_modem_speed100 % 100), (int) glob_modem_mode);
#endif /* WITHMODEM */
}

///////////////////////

static FLOAT_t agclogof10 = 1;
	
static void agc_state_initialize(volatile agcstate_t * st, const volatile agcparams_t * agcp)
{
	const FLOAT_t f0 = agcp->levelfence;
	const FLOAT_t m0 = agcp->mininput;
	const FLOAT_t siglevel = 0;
	const FLOAT_t level = FMINF(FMAXF(m0, siglevel), f0);	// работаем в диапазоне от 1 до f
	const FLOAT_t ratio = (agcp->agcoff ? m0 : level) / f0;

	const FLOAT_t streingth_log = logf(ratio);
	const FLOAT_t caplevel = streingth_log;

	st->agcfastcap = caplevel;
	st->agcslowcap = caplevel;
}

// Для работы функции performagc требуется siglevel, больште значения которого 
// соответствуют большим уровням сигнала. может быть отрицательным
static RAMFUNC FLOAT_t agccalcstraingth(const volatile agcparams_t * const agcp, FLOAT_t siglevel)
{
	const FLOAT_t f0 = agcp->levelfence;
	const FLOAT_t m0 = agcp->mininput;
	const FLOAT_t level = FMINF(FMAXF(m0, siglevel), f0);	// работаем в диапазоне от 1 до levelfence
	const FLOAT_t ratio = (agcp->agcoff ? m0 : level) / f0;

	const FLOAT_t streingth_log = LOGF(ratio);
	return streingth_log;
}

// По отфильтрованому в соответствии с заданными временныме параметрами показатлю
// силы сигнала получаем требуемое усиление (в разах отношения напряжений).
static RAMFUNC FLOAT_t agccalcgain(const volatile agcparams_t * const agcp, FLOAT_t streingth)
{
	const FLOAT_t gain0 = POWF((FLOAT_t) M_E, streingth * agcp->agcfactor);
	// реализация "спортивной" АРУ
	// Увеличению сигнала на glob_agcrate децибел должно соответствовать 
	// увеличение выхода приёмника на 1 децтибел
	const FLOAT_t gain = FMINF(agcp->gainlimit, gain0);
	return gain;
}

// По отфильтрованому в соответствии с заданными временныме параметрами показатлю
// силы сигнала получаем относительный уровень (логарифмированный)
static RAMFUNC FLOAT_t agccalcstraingthlog10(const volatile agcparams_t * const agcp, FLOAT_t streingth)
{
	return streingth / agclogof10;	// уже логарифмировано
}

static RAMFUNC FLOAT_t agc_getsiglevel(
	FLOAT32P_t sampleiq
	)
{
	const FLOAT_t sample1 = SQRTF((FLOAT_t) sampleiq.IV * sampleiq.IV + (FLOAT_t) sampleiq.QV * sampleiq.QV);
	const FLOAT_t sample2 = FMAXF(FABSF(sampleiq.IV), FABSF(sampleiq.QV));	// используется эта формула, так как цель - исключить арифметическое переполнение.
	const FLOAT_t sample = FMAXF(sample1, sample2);

	//return sample2;
	return sample;
}
//
// постоянные времени системы АРУ
// Инициализация сделана для того, чтобы поместить эти переменные в обюласть CCM памяти
// Присвоение осмысленных значений производится в соответствующих функциях инициализации.

static volatile agcstate_t rxagcstate [NTRX] = { { 0, } };	// На каждый приёмник
static volatile agcstate_t txagcstate = { 0, };

static volatile agcparams_t rxagcparams [NPROF] [NTRX] = { { { 0, } } };
static volatile agcparams_t txagcparams [NPROF] = { { 0, } };
static volatile uint_fast8_t gwagcprofrx = 0;	// work profile - индекс конфигурационной информации, испольуемый для работы */
static volatile uint_fast8_t gwagcproftx = 0;	// work profile - индекс конфигурационной информации, испольуемый для работы */
//	
static void agc_initialize(void)
{
	// Установка параметров АРУ приёмника
	gwagcprofrx = 0;
	gwagcproftx = 0;

	agclogof10 = LOGF(10);

	uint_fast8_t profile;
	for (profile = 0; profile < NPROF; ++ profile)
	{
		uint_fast8_t pathi;
		for (pathi = 0; pathi < NTRX; ++ pathi)
		{

			agc_parameters_initialize(& rxagcparams [profile] [pathi]);
			agc_state_initialize(& rxagcstate [pathi], & rxagcparams [profile] [pathi]);
		}

		// Микрофон всегда с flatgain=1
		comp_parameters_initialize(& txagcparams [profile]);
		agc_state_initialize(& txagcstate, & txagcparams [profile]);
	}

#if WITHDSPEXTDDC

#elif WITHDACOUTDSPAGC
	setagcattenuation(0, 0);	// в кодах ЦАП уменьшение усиления
	setlevelindicator(0);
#endif /* WITHDSPEXTDDC */
}

static RAMFUNC FLOAT32P_t
agc_delaysignal(
	FLOAT32P_t v, 
	uint_fast8_t pathi
	)
{
#if WITHLIMITEDAGCATTACK

	enum { DLY = NSAITICKS(5) };	// данные задерживаются на указанное количество mS

	static FLOAT32P_t data [NTRX] [DLY] = { { { 0, 0 }, } };
	static unsigned pos [NTRX] = { 0, };

	unsigned * const pp = & pos [pathi];
	unsigned posv = * pp;
	FLOAT32P_t * const pdata = & data [pathi] [posv];

	const FLOAT32P_t r = * pdata;	// получаем задержанные данные
	* pdata = v;					// записываем вновь поступившие
	* pp = (posv == 0) ? (DLY - 1) : (posv - 1);	// обеспечение кольцевого пермещения индекса

	return r;

#else /* WITHLIMITEDAGCATTACK */

	return v;

#endif /* WITHLIMITEDAGCATTACK */
}

// АРУ вперёд для floaing-point тракта
// получение усиления
static RAMFUNC FLOAT_t agc_forvard_float(
	const uint_fast8_t dspmode, 
	FLOAT32P_t sampleiq, 
	uint_fast8_t pathi
	)
{
	BEGIN_STAMP3();
	const FLOAT_t siglevel0 = agc_getsiglevel(sampleiq);
	const volatile agcparams_t * const agcp = & rxagcparams [gwagcprofrx] [pathi];
	volatile agcstate_t * const st = & rxagcstate [pathi];

	performagc(agcp, st, agccalcstraingth(agcp, siglevel0));	// измеритель уровня сигнала
	const FLOAT_t gain = agccalcgain(agcp, performagcresultslow(st));
	END_STAMP3();
	return gain;
}
// Функция для S-метра - получение десятичного логарифма уровня сигнала от FS
/* Вызывается из user-mode программы */
static void agc_reset(
	uint_fast8_t pathi
	)
{
	return;
	volatile agcstate_t * const st = & rxagcstate [pathi];
	const volatile agcparams_t * const agcp = & rxagcparams [gwagcprofrx] [pathi];
	const FLOAT_t m0 = agcp->mininput;
	global_disableIRQ();
	st->agcfastcap = m0;
	st->agcslowcap = m0;
	global_enableIRQ();
#if ! CTLSTYLE_V1D
	for (;;)
	{
		local_delay_ms(1);
		global_disableIRQ();
		const FLOAT_t v = performagcresultslow(st);
		global_enableIRQ();
		if (v != m0)
			break;
	}
#endif
}
// Функция для S-метра - получение десятичного логарифма уровня сигнала от FS
/* Вызывается из user-mode программы */
static FLOAT_t agc_forvard_getstreigthlog10(
	FLOAT_t * tracemax,
	uint_fast8_t pathi
	)
{
	volatile agcparams_t * const agcp = & rxagcparams [gwagcprofrx] [pathi];
	volatile const agcstate_t * const st = & rxagcstate [pathi];

	const FLOAT_t fltstraingthfast = performagcresultfast(st);	// измеритель уровня сигнала
	const FLOAT_t fltstraingthslow = performagcresultslow(st);	// измеритель уровня сигнала
	* tracemax = agccalcstraingthlog10(agcp, fltstraingthslow);
	return agccalcstraingthlog10(agcp, fltstraingthfast);
}

/* получить значение уровня сигнала в децибелах, отступая от upper */
/* -73.01dBm == 50 uV rms == S9 */
/* Вызывается из user-mode программы */
uint_fast8_t 
dsp_getsmeter(uint_fast8_t * tracemax, uint_fast8_t lower, uint_fast8_t upper, uint_fast8_t clean)
{
	const uint_fast8_t pathi = 0;	// тракт, испольуемый для показа s-метра
	if (clean != 0)
		agc_reset(pathi);
	FLOAT_t tmaxf;
	int level = upper + (int) (agc_forvard_getstreigthlog10(& tmaxf, pathi) * 200 + (glob_fsadcpower10 + 5)) / 10;	// преобразование в децибелы и отступаем от правой границы шкалы
	int tmax = upper + (int) (tmaxf * 200 + (glob_fsadcpower10 + 5)) / 10;

	if (tmax > (int) upper)
		tmax = upper;
	else if (tmax < (int) lower)
		tmax = lower;

	if (level > (int) upper)
		level = upper;
	else if (level < (int) lower)
		level = lower;

	* tracemax = tmax;
	return level;
}

static FLOAT_t mickecliplevelp [NPROF] = { + INT_MAX, + INT_MAX };
static FLOAT_t mickeclipleveln [NPROF] = { - INT_MAX, - INT_MAX };
static FLOAT_t mickeclipscale [NPROF] = { 1, 1 };

// ару и компрессор микрофона
static RAMFUNC FLOAT_t txmikeagc(FLOAT_t vi)
{
	volatile agcparams_t * const agcp = & txagcparams [gwagcproftx];
	if (agcp->agcoff == 0)
	{
		const FLOAT_t siglevel0 = FABSF(vi);
		volatile agcstate_t * const st = & txagcstate;

		performagc(agcp, st, agccalcstraingth(agcp, siglevel0));	// измеритель уровня сигнала
		const FLOAT_t gain = agccalcgain(agcp, performagcresultslow(st));
		vi *= gain;
	}

	{
		// Ограничитель
		const FLOAT_t levelp = mickecliplevelp [gwagcproftx];
		const FLOAT_t leveln = mickeclipleveln [gwagcproftx];
		if (vi > levelp)	
			vi = levelp;
		else if (vi < leveln)
			vi = leveln;
		vi *= mickeclipscale [gwagcproftx];
	}

	return vi;
}


void debug_showcompstate(void)
{
	const volatile agcstate_t * const st = & txagcstate;
	char s [128];

	snprintf(s, sizeof s / sizeof s [0], 
		"agcfastcap=%f, agchangticks=%u, agcslowcap=%f\n", 
		st->agcfastcap, st->agchangticks, st->agcslowcap);
	dbg_puts_impl(s);
}

// agc ---

/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: High Pass
Filter model: Chebyshev
Filter order: 4
Sampling Frequency: 48 KHz
Cut Frequency: 7.500000 KHz
Pass band Ripple: 1.000000 dB
Coefficents Quantization: float

Z domain Zeros
z = 1.000000 + j 0.000000
z = 1.000000 + j 0.000000
z = 1.000000 + j 0.000000
z = 1.000000 + j 0.000000

Z domain Poles
z = 0.639035 + j -0.302403
z = 0.639035 + j 0.302403
z = 0.501849 + j -0.734622
z = 0.501849 + j 0.734622
***************************************************************/
static RAMFUNC float iir_nfmnbbpf(FLOAT_t NewSample) {
	enum { NCoef = 4 };
   static const FLOAT_t ACoef[NCoef+1] = {
        0.48404079901937469000,
        -1.93616319607749880000,
        2.90424479411624810000,
        -1.93616319607749880000,
        0.48404079901937469000
    };

    static const FLOAT_t BCoef[NCoef+1] = {
        1.00000000000000000000,
        -2.28176832599058790000,
        2.57413221290498310000,
        -1.51328228347904200000,
        0.39561310580923453000
    };

	static uint_fast8_t iir_stage = 0;
    static FLOAT_t y[2 * (NCoef+1)] = { 0 }; //output samples
    static FLOAT_t x[2 * (NCoef+1)] = { 0 }; //input samples
    int n;

	// shift the old samples
	iir_stage = (iir_stage == 0) ? (NCoef) : (iir_stage - 1);
    //shift the old samples
    //for(n=NCoef; n>0; n--) {
    //   x[n] = x[n-1];
    //   y[n] = y[n-1];
    //}

    //Calculate the new output
    x [iir_stage] = x [iir_stage + (NCoef+1)] = NewSample;
    y [iir_stage] = y [iir_stage + (NCoef+1)] = ACoef[0] * NewSample;

    for (n = 1; n <= NCoef; n ++)
        y [iir_stage + 0] += ACoef [n] * x [iir_stage + n] - BCoef [n] * y [iir_stage + n];
    
    return y [iir_stage + 0];
// original
    //shift the old samples
    for(n=NCoef; n>0; n--) {
       x[n] = x[n-1];
       y[n] = y[n-1];
    }

    //Calculate the new output
    x[0] = NewSample;
    y[0] = ACoef[0] * x[0];
    for(n=1; n<=NCoef; n++)
        y[0] += ACoef[n] * x[n] - BCoef[n] * y[n];
    
    return y[0];


}

/* Значения 0..1 */

static FLOAT_t sidetonevolume = 0; //(glob_sidetonelevel / (FLOAT_t) 100);
static FLOAT_t mainvolumerx = 1; //1 - sidetonevolume;
static FLOAT_t subtonevolume = 0; //(glob_subtonelevel / (FLOAT_t) 100);
static FLOAT_t mainvolumetx = 1; //1 - subtonevolume;

// Здесь значение выборки в диапазоне, допустимом для кодека
static RAMFUNC FLOAT_t injectsidetone(FLOAT_t v, FLOAT_t sdtn)
{

	return v * mainvolumerx + sdtn * sidetonevolume;
}

// Здесь значение выборки в диапазоне, допустимом для кодека
static RAMFUNC FLOAT_t injectsubtone(FLOAT_t v, FLOAT_t ctcss)
{

	return v * mainvolumetx + ctcss * subtonevolume;
}

// Поддержка генерации белого шума
static unsigned long local_random(unsigned long num)
{

	static unsigned long rand_val = 123456UL;

	if (rand_val & 0x80000000UL)
		rand_val = (rand_val << 1);
	else	
		rand_val = (rand_val << 1) ^ 0x201051UL;

	return (rand_val % num);
}

// return audio sample in range [- txlevelfence.. + txlevelfence]
static RAMFUNC FLOAT_t preparevi(
	int vi0,
	uint_fast8_t dspmode,
	FLOAT_t ctcss	// субтон, audio sample in range [- txlevelfence.. + txlevelfence]
	)
{
	FLOAT_t vi0f = vi0;

#if 0
	// Устранение выхода за пределы разрядной сетки
	if (vi0f > mikefence)
		vi0f = mikefence;
	else if (vi0f < - mikefence)
		vi0f = - mikefence;
#endif

	switch (dspmode)
	{
	case DSPCTL_MODE_TX_BPSK:
		return txlevelfenceBPSK;	// постоянная составляющая с максимальным уровнем

	case DSPCTL_MODE_TX_CW:
		return txlevelfenceHALF;	// постоянная составляющая с максимальным уровнем

	case DSPCTL_MODE_TX_SSB:
	case DSPCTL_MODE_TX_AM:
	case DSPCTL_MODE_TX_NFM:
	case DSPCTL_MODE_TX_FREEDV:
		// Источник нормируется к txlevelfenceSSB
		switch (glob_txaudio)
		{
		//case BOARD_TXAUDIO_MIKE:
		//case BOARD_TXAUDIO_LINE:
		//case BOARD_TXAUDIO_USB:
		default:
			// VOX detector и разрядная цепь
			// Поддержка работы VOX
			charge2(& mikeinlevel, FABSF(vi0f), 
				(mikeinlevel < vi0f) ? VOXCHARGE : VOXDISCHARGE);
			// источник - микрофон, LINE IN или USB
			// see glob_mik1level (0..100)
			return injectsubtone(txmikeagc(vi0f * txlevelfenceSSB / mikefence), ctcss); //* TXINSCALE; // источник сигнала - микрофон

		case BOARD_TXAUDIO_NOISE:
			// источник - шум
			//vf = filter_fir_tx_MIKE((local_random(2UL * IFDACMAXVAL) - IFDACMAXVAL), 0);	// шум
			// return audio sample in range [- txlevelfence.. + txlevelfence]
			return injectsubtone((int) (local_random(2 * txlevelfenceSSB_INTEGER - 1) - txlevelfenceSSB_INTEGER), ctcss);	// шум

		case BOARD_TXAUDIO_2TONE:
			// источник - двухтоновый сигнал
			// return audio sample in range [- txlevelfence.. + txlevelfence]
			return injectsubtone(get_dualtonefloat() * txlevelfenceSSB, ctcss);		// источник сигнала - двухтональный генератор для настройки

		case BOARD_TXAUDIO_1TONE:
			// источник - синусоидальный сигнал
			// return audio sample in range [- txlevelfence.. + txlevelfence]
			return injectsubtone(get_singletonefloat() * txlevelfenceSSB, ctcss);

		case BOARD_TXAUDIO_MUTE:
			return injectsubtone(0, ctcss);
		}
	}
	// В режиме приёма или bypass ничего не делаем.
	return 0;
}

/* получить I/Q пару для передачи в up-converter */
static RAMFUNC FLOAT32P_t baseband_modulator(
	FLOAT_t vi,
	uint_fast8_t dspmode,
	FLOAT_t shape
	)
{
	const uint_fast8_t pathi = 0;	// тракт, испольуемый при передаче
	switch (dspmode)
	{
	default:
		{
			const FLOAT32P_t vfb = { { 0, 0 } };
			// В режиме приёма ничего не делаем.
			return vfb;
		}

#if WITHMODEM
	case DSPCTL_MODE_TX_BPSK:
		{
			const FLOAT32P_t vfb = scalepair(modem_get_tx_iq(getTxShapeNotComplete()), txlevelfenceBPSK * shape);
	#if WITHMODEMIQLOOPBACK
			modem_demod_iq(vfb);	// debug loopback
	#endif /* WITHMODEMIQLOOPBACK */
			return vfb;
		}
#endif /* WITHMODEM */

	case DSPCTL_MODE_TX_CW:
		{
			// vi - audio sample in range [- txlevelfence.. + txlevelfence]
			//const FLOAT32P_t vfb = scalepair_int32(get_int32_aflo_delta(0, pathi), vi * shape);
			const FLOAT32P_t vfb = scalepair(get_float_aflo_delta(0, pathi), txlevelfenceCW * shape);
			return vfb;
		}

	case DSPCTL_MODE_TX_SSB:
	case DSPCTL_MODE_TX_FREEDV:
		{
			// vi - audio sample in range [- txlevelfence.. + txlevelfence]
			//const FLOAT32P_t vfb = scalepair_int32(get_int32_aflo_delta(0, pathi), vi * shape);
			const FLOAT32P_t vfb = scalepair(get_float_aflo_delta(0, pathi), vi * shape);
			return vfb;
		}
	
	case DSPCTL_MODE_TX_AM:
		{
			// vi - audio sample in range [- txlevelfenceSSB.. + txlevelfenceSSB]
			// input range: of vi: (- IFDACMAXVAL) .. (+ IFDACMAXVAL)
			const FLOAT_t peak = amcarrierHALF + vi * amshapesignalHALF;
			//const FLOAT32P_t vfb = scalepair_int32(get_int32_aflo_delta(0, pathi), peak * shape);
			const FLOAT32P_t vfb = scalepair(get_float_aflo_delta(0, pathi), peak * shape);
			return vfb;
		}

	case DSPCTL_MODE_TX_NFM:
		{
			// vi - audio sample in range [- txlevelfence.. + txlevelfence]
			const long int deltaftw = (long) gnfmdeviationftw * vi / txlevelfenceSSB;	// Учитывается нормирование источника звука
			//const FLOAT32P_t vfb = scalepair_int32(get_int32_aflo_delta(deltaftw, pathi), txlevelfenceHALF * shape);
			const FLOAT32P_t vfb = scalepair(get_float_aflo_delta(deltaftw, pathi), txlevelfenceNFM * shape);
			return vfb;
		}
	}
}

// В канале всегда LEFT JUSTIFIED

static RAMFUNC int_fast32_t iq2tx(int_fast32_t v)
{
	return v << (32 - WITHIFDACWIDTH);
}


#if WITHDSPEXTDDC


// ПЕРЕДАЧА
// обрабатывается 16 битное (WITHAFADCWIDTH) число
// используется в случае внешнего DUC
static RAMFUNC void processafadcsampleiq(
	INT32P_t vi0,	// выборка с микрофона (в vi)
	uint_fast8_t dspmode,
	FLOAT_t shape,	// 0..1 - огибающая
	FLOAT_t ctcss	// субтон, audio sample in range [- txlevelfence.. + txlevelfence]
	)
{
	// vi - audio sample in range [- txlevelfence.. + txlevelfence]
	FLOAT_t vi = preparevi(vi0.IV, dspmode, ctcss);	// vi нормирован к разрядности выходного ЦАП
	if (isdspmodetx(dspmode))
	{
		vi = filter_fir_tx_MIKE(vi, 0);
#if WITHDSPLOCALFIR
		const FLOAT32P_t vfb = filter_firp_tx_SSB_IQ(baseband_modulator(vi, dspmode, shape));
#else /* WITHDSPLOCALFIR */
		const FLOAT32P_t vfb = baseband_modulator(vi, dspmode, shape);
#endif /* WITHDSPLOCALFIR */
		savesampleout32stereo(iq2tx(vfb.IV), iq2tx(vfb.QV));	// Запись в поток к передатчику I/Q значений.
	}
	else
	{
		filter_fir_tx_MIKE(vi, 1);		// Фильтр не применяется, только выполняется сдвиг в линии задержки
		savesampleout32stereo(0, 0);
	}
}

#else /* WITHDSPEXTDDC */


// ПЕРЕДАЧА
// обрабатывается 16 битное (WITHAFADCWIDTH) число
static RAMFUNC void processafadcsample(
	INT32P_t vi0,	// выборка с микрофона (в vi)
	uint_fast8_t dspmode,
	FLOAT_t shape,	// 0..1 - огибающая
	FLOAT_t ctcss	// субтон, audio sample in range [- txlevelfence.. + txlevelfence]
	)
{
#if WITHDSPLOCALFIR == 0
	#error WITHDSPLOCALFIR should be defined
#endif /* WITHDSPLOCALFIR == 0 */

	FLOAT_t vi = preparevi(vi0.IV, dspmode, ctcss);	// vi нормирован к разрядности выходного ЦАП
	// vi - audio sample in range [- txlevelfence.. + txlevelfence]
	if (isdspmodetx(dspmode))
	{
		vi = filter_fir_tx_MIKE(vi, 0);
		const FLOAT32P_t vfb = baseband_modulator(vi, dspmode, shape);
		// Здесь, имея квадратурные сигналы vfb.IV и vfb.QV, 
		// производим Digital Up Conversion
		const FLOAT32P_t v_if = get_float4_iflo();	// частота 12 кГц - 1/4 частоты выборок АЦП - можно воспользоваться целыми значениями.
		const FLOAT32P_t e1 = filter_fir4_tx_SSB_IQ(vfb, v_if.IV != 0);		// 1.85 kHz - фильтр имеет усиление 2.0
		const FLOAT_t r = (e1.QV * v_if.QV + e1.IV * v_if.IV);	// переносим на выходную частоту ("+" - без инверсии).
		// Интерфейс с ВЧ - одноканальный ADC/DAC
		savesampleout32stereo(iq2tx(r * shape), 0);	// кодек получает 24 бита left justified в 32-х битном числе.
	}
	else
	{
		filter_fir_tx_MIKE(vi, 1);		// Фильтр не применяется, только выполняется сдвиг в линии задержки
		savesampleout32stereo(0, 0);
	}
}

#endif /* WITHDSPEXTDDC */

//
// Todo: Use FLT_EPSILON (DBL_EPSILON)
// See also: https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
#if 0

// http://www.vcl.salk.edu/~micah/FM/FM.html micah@salk.edu
// To make the code faster you can change the ATAN2F to the following:

// Fast approximate arctan2 code posted by Jim Shima
static RAMFUNC FLOAT_t arctan2(FLOAT_t y, FLOAT_t x)
{
	FLOAT_t angle;
	//const float abs_y = y >= 0 ? y : - y;
	const FLOAT_t abs_y = FABSF(y);
	if (x >= 0)
	{
		const FLOAT_t r = (x - abs_y) / (x + abs_y + 1e-40f); // add 1e-40 to prevent 0/0 condition
		angle = (FLOAT_t) 0.1963 * r * r * r - (FLOAT_t) 0.9817 * r + (FLOAT_t) M_PI_4;
	} else {
		const float r = (x + abs_y) / (abs_y - x);
		angle = (FLOAT_t) 0.1963 * r * r * r - (FLOAT_t) 0.9817 * r + 3 * (FLOAT_t) M_PI_4;
	}
	if (y < 0) 
		return - angle;     // negate if in quad III or IV
	else 
		return angle;
}
#endif



//////////////////////////

// Демодуляция FM
static RAMFUNC ncoftwi_t demodulator_FM(
	FLOAT32P_t vp1,
	const uint_fast8_t pathi				// 0/1: main_RX/sub_RX
	)
{
	// Здесь, имея квадратурные сигналы vp1.IV и vp1.QV, начинаем демодуляции
	//
	// tnx Vladimir Vassilevsky
	// http://www.dsprelated.com/showmessage/71491/2.php
	//
	static ncoftwi_t prev_fi [NTRX] = { 0, };

	if (vp1.IV == 0 && vp1.QV == 0)
		vp1.QV = 1;
	const ncoftwi_t fi = OMEGA2FTWI(ATAN2F(vp1.QV, vp1.IV));	//  returns a value in the range –pi to pi radians, using the signs of both parameters to determine the quadrant of the return value.

	const ncoftwi_t d_fi = (ncoftwi_t) (fi - prev_fi [pathi]);
	prev_fi [pathi] = fi;

	return d_fi;
}


enum { AMDSTAGES = 7, AMDOUT_IDX = (3 * AMDSTAGES) };


struct amd
{
	//int run;
	//int buff_size;					// buffer size
	//FLOAT_t *in_buff;					// pointer to input buffer
	//FLOAT_t *out_buff;				// pointer to output buffer
	//int mode;							// demodulation mode
	//FLOAT_t sample_rate;				// sample rate
	FLOAT_t dc;							// dc component in demodulated output
	ncoftwi_t omegai_min;					// pll - minimum lock check parameter
	ncoftwi_t omegai_max;					// pll - maximum lock check parameter
	ncoftwi_t phsi;						// pll - phase accumulator
	ncoftwi_t omegai;						// pll - locked pll frequency
	ncoftwi_t fil_outi;					// pll - filter output
	int64_t g1i, g2i;					// pll - filter gain parameters

	FLOAT_t mtauR;						// carrier removal multiplier
	FLOAT_t onem_mtauR;					// 1.0 - carrier_removal_multiplier
	FLOAT_t mtauI;						// carrier insertion multiplier
	FLOAT_t onem_mtauI;					// 1.0 - carrier_insertion_multiplier

	FLOAT_t a [3 * AMDSTAGES + 3];		// Filter a variables
	FLOAT_t b [3 * AMDSTAGES + 3];		// Filter b variables
	FLOAT_t c [3 * AMDSTAGES + 3];		// Filter c variables
	FLOAT_t d [3 * AMDSTAGES + 3];		// Filter d variables
	FLOAT_t c0 [AMDSTAGES];				// Filter coefficients - path 0
	FLOAT_t c1 [AMDSTAGES];				// Filter coefficients - path 1
	FLOAT_t dsI;						// delayed sample, I path
	FLOAT_t dsQ;						// delayed sample, Q path
	FLOAT_t dc_insert;					// dc component to insert in output
	int sbmode;						// sideband mode
	//int levelfade;					// Fade Leveler switch
};

static struct amd amds [NTRX] = { { 0 }, };	// force CCM allocation

/* Получить информацию об ошибке настройки в режиме SAM */
/* Получить значение отклонения частоты с точностью 0.1 герца */
uint_fast8_t hamradio_get_samdelta10(int_fast32_t * p, uint_fast8_t pathi)
{
	const uint_fast32_t sample_rate10 = ARMSAIRATE * 10;

	* p = ((int_fast64_t) amds [pathi].omegai * sample_rate10) >> 32;
	return glob_dspmodes [pathi] == DSPCTL_MODE_RX_SAM;
}

static volatile int32_t saved_delta_fi [NTRX] = { 0, };	// force CCM allocation

/* Получить значение отклонения частоты с точностью 0.1 герца */
uint_fast8_t dsp_getfreqdelta10(int_fast32_t * p, uint_fast8_t pathi)
{
	const uint_fast32_t sample_rate10 = ARMSAIRATE * 10;

	* p = ((int_fast64_t) saved_delta_fi [pathi] * sample_rate10) >> 32;
	return glob_dspmodes [pathi] == DSPCTL_MODE_RX_NFM;
}

static void init_amd(struct amd * a)
{
	a->phsi = 0;
	a->fil_outi = 0;
	a->omegai = 0;

	//fade leveler
	a->dc = 0;
	a->dc_insert = 0;

	//sideband separation
	a->c0 [0] = (FLOAT_t) -0.328201924180698;
	a->c0 [1] = (FLOAT_t) -0.744171491539427;
    a->c0 [2] = (FLOAT_t) -0.923022915444215;
    a->c0 [3] = (FLOAT_t) -0.978490468768238;
    a->c0 [4] = (FLOAT_t) -0.994128272402075;
    a->c0 [5] = (FLOAT_t) -0.998458978159551;
    a->c0 [6] = (FLOAT_t) -0.999790306259206;
   
    a->c1 [0] = (FLOAT_t) -0.0991227952747244;
    a->c1 [1] = (FLOAT_t) -0.565619728761389;
    a->c1 [2] = (FLOAT_t) -0.857467122550052;
    a->c1 [3] = (FLOAT_t) -0.959123933111275;
    a->c1 [4] = (FLOAT_t) -0.988739372718090;
    a->c1 [5] = (FLOAT_t) -0.996959189310611;
    a->c1 [6] = (FLOAT_t) -0.999282492800792;
}


static void 
create_amd(
	struct amd * a,
	//int run,
	//int mode,
	//int levelfade,
	int sbmode,
	//int sample_rate,
	int fmin,
	int fmax,
	FLOAT_t zeta,		// pll - damping factor; as coded, must be <=1.0
	FLOAT_t omegaN,		// pll - natural frequency
	FLOAT_t tauR,		// carrier removal time constant
	FLOAT_t tauI		// carrier insertion time constant
	)
{
	FLOAT_t g1, g2;						// pll - filter gain parameters
	FLOAT_t knorm = POWF(2, NCOFTWBITS);
	const FLOAT_t sample_rate = (FLOAT_t) ARMSAIRATE;
	//a->run = run;
	//a->mode = mode;
	//a->levelfade = levelfade;
	a->sbmode = sbmode;

	a->omegai_min = FTWAF(fmin);
	a->omegai_max = FTWAF(fmax);
	g1 = 1 - EXPF(- 2 * omegaN * zeta / sample_rate);
	g2 = - g1 + 2 * (1 - EXPF(- omegaN * zeta / sample_rate) * COSF(omegaN / sample_rate * SQRTF(1 - zeta * zeta)));

	a->g1i = g1 * knorm;	// 2^32
	a->g2i = g2 * knorm;	// 2^32

	// carrier removal
	a->mtauR = EXPF(- 1 / (sample_rate * tauR));
	a->onem_mtauR = 1 - a->mtauR;
	a->mtauI = EXPF(- 1 / (sample_rate * tauI));
	a->onem_mtauI = 1 - a->mtauI;

	init_amd(a);
}

#if 0
static void 
flush_amd(struct amd * a)
{
	a->dc = 0;
	a->dc_insert = 0;
}
#endif

// Демодуляция SAM
static RAMFUNC FLOAT_t 
demodulator_SAM(
	FLOAT32P_t vp1,
	const uint_fast8_t pathi				// 0/1: main_RX/sub_RX
	)
{
	// taken from Warren PrattВґs WDSP, 2016
	// http://svn.tapr.org/repos_sdr_hpsdr/trunk/W5WC/PowerSDR_HPSDR_mRX_PS/Source/wdsp/amd.c

	FLOAT_t audio;	// выходной сэмпл (ненормированнгое значение).
	FLOAT32P_t vco;
	FLOAT_t corr [2];
	ncoftwi_t deti;
	ncoftwi_t del_outi;
	FLOAT_t ai, bi, aq, bq;
	FLOAT_t ai_ps, bi_ps, aq_ps, bq_ps;

	struct amd * const a = & amds [pathi];

	vco.QV = peekvalf(FTW2ANGLEI(a->phsi));	// COSF(a->phs);
	vco.IV = peekvalf(FTW2ANGLEQ(a->phsi));	// SINF(a->phs);

	ai = vp1.IV * vco.IV;
	bi = vp1.IV * vco.QV;
	aq = vp1.QV * vco.IV;
	bq = vp1.QV * vco.QV;

	if (a->sbmode != 0)
	{
		int j;

		a->a [0] = a->dsI;
		a->b [0] = bi;
		a->c [0] = a->dsQ;
		a->d [0] = aq;
		a->dsI = ai;
		a->dsQ = bq;

		for (j = 0; j < AMDSTAGES; ++ j)
		{
			const int k = 3 * j;
			a->a [k + 3] = a->c0 [j] * (a->a [k] - a->a [k + 5]) + a->a [k + 2];
			a->b [k + 3] = a->c1 [j] * (a->b [k] - a->b [k + 5]) + a->b [k + 2];
			a->c [k + 3] = a->c0 [j] * (a->c [k] - a->c [k + 5]) + a->c [k + 2];
			a->d [k + 3] = a->c1 [j] * (a->d [k] - a->d [k + 5]) + a->d [k + 2];
		}
		ai_ps = a->a [AMDOUT_IDX];
		bi_ps = a->b [AMDOUT_IDX];
		bq_ps = a->c [AMDOUT_IDX];
		aq_ps = a->d [AMDOUT_IDX];

		for (j = AMDOUT_IDX + 2; j > 0; j--)
		{
			a->a [j] = a->a [j - 1];
			a->b [j] = a->b [j - 1];
			a->c [j] = a->c [j - 1];
			a->d [j] = a->d [j - 1];
		}
	}

	corr [0] = + ai + bq;
	corr [1] = - bi + aq;

	switch (a->sbmode)
	{
	default:
	case 0:	//both sidebands
		{
			audio = corr [0];
			break;
		}
	case 1:	//LSB
		{
			audio = (ai_ps - bi_ps) + (aq_ps + bq_ps);
			break;
		}
	case 2:	//USB
		{
			audio = (ai_ps + bi_ps) - (aq_ps - bq_ps);
			break;
		}
	}

	if (0/*a->levelfade*/)
	{
		a->dc = a->mtauR * a->dc + a->onem_mtauR * audio;
		a->dc_insert = a->mtauI * a->dc_insert + a->onem_mtauI * corr[0];
		audio += a->dc_insert - a->dc;
	}

	if ((corr [0] == 0) && (corr [1] == 0)) 
		corr [0] = 1;

	deti = OMEGA2FTWI(ATAN2F(corr [1], corr [0]));	// - M_PI .. + M_PI
	a->omegai += (a->g2i * deti) >> 32;
	if (a->omegai < a->omegai_min) 
		a->omegai = a->omegai_min;
	else if (a->omegai > a->omegai_max) 
		a->omegai = a->omegai_max;

	del_outi = a->fil_outi;
	a->fil_outi = (int32_t) ((a->g1i * deti) >> 32) + a->omegai;
	a->phsi += del_outi;	/* "заворот" по модулю M_TWOPI автоматически обеспечивается целочисленным переполнением */

	return audio;
}


// ПРИЁМ
// Обрабатывается floating point квадратура
// При необходимости применяется АРУ
// Возвращается сэмпл - выход детектора, нормалтзованный для попадания в диапазон WITHAFDACWIDTH
static RAMFUNC_NONILINE FLOAT_t baseband_demodulator(
	FLOAT32P_t vp0f,					// Квадратурные значения выборки
	const uint_fast8_t dspmode, 
	const uint_fast8_t pathi				// 0/1: main_RX/sub_RX
	)
{
#if DUALFILTERSPROCESSING
	enum { DUALRXFLT = 1 };
#else /* DUALFILTERSPROCESSING */
	enum { DUALRXFLT = 0 };
#endif /* DUALFILTERSPROCESSING */

	FLOAT_t r;
	switch (dspmode)
	{
	default:
		// режим передачи
		{
			r = 0;	
		}
		break;

	case DSPCTL_MODE_RX_DSB:
	case DSPCTL_MODE_RX_WIDE:
	case DSPCTL_MODE_RX_NARROW:
	case DSPCTL_MODE_RX_DRM:
	case DSPCTL_MODE_RX_FREEDV:
		{
			// use floating point
			const FLOAT_t gain = agc_forvard_float(dspmode, vp0f, pathi);
			const FLOAT32P_t vp1 = scalepair(agc_delaysignal(vp0f, pathi), gain);
			const FLOAT32P_t af = get_float_aflo_delta(0, pathi);	// средняя частота выходного спектра
			r = (vp1.QV * af.QV + vp1.IV * af.IV); // переносим на выходную частоту ("+" - без инверсии).
			r = r * rxoutdenom;
		}
		break;

#if WITHMODEM
	case DSPCTL_MODE_RX_BPSK:
		if (pathi == 0)
		{
			/*const FLOAT_t gain = */agc_forvard_float(dspmode, vp0f, pathi);	// для отображения S-метра
			//INT32P_t vp0i32;
			//saved_delta_fi [pathi] = demodulator_FM(vp0f, pathi);	// погрешность настройки - требуется фильтровать ФНЧ
			modem_demod_iq(vp0f);
		}
		r = 0;
		break;
#endif /* WITHMODEM */

	case DSPCTL_MODE_RX_NFM:
		if (/*DUALRXFLT || */pathi == 0)
		{
			// Демодуляция NBFM
			/*const FLOAT_t gain = */agc_forvard_float(dspmode, vp0f, pathi);	// для отображения S-метра
			saved_delta_fi [pathi] = demodulator_FM(vp0f, pathi);	// погрешность настройки - требуется фильтровать ФНЧ
			//const int fdelta10 = ((int64_t) saved_delta_fi [pathi] * ARMSAIRATE * 10) >> 32;	// Отклнение частоты в 0.1 герц единицах
			// значение для прослушивания
			// 0.707 == M_SQRT1_2
			const FLOAT_t sample = saved_delta_fi [pathi]; //(FLOAT_t) M_SQRT1_2;
			BEGIN_STAMP2();
			r = sample * nfmoutscale; //* rxoutdenom;	// масштабирование к разрядности аудио-кодека 1_31 -> 1_15
			END_STAMP2();
			if (glob_nfm_sql_off == 0)
			{
				// "шумодав"
				testholdmax3(iir_nfmnbbpf(sample));
				const int nbopen = getholdmax3() < nbfence;
				r = (nbopen != 0) ? r : (r / 16);
			}
		}
		else
			r = 0;
		break;

	case DSPCTL_MODE_RX_AM:
		if (DUALRXFLT || pathi == 0)
		{
			/* AM demodulation */
			// Здесь, имея квадратурные сигналы vp1.IV и vp1.QV, начинаем демодуляции
			const FLOAT_t gain = agc_forvard_float(dspmode, vp0f, pathi);
			const FLOAT32P_t vp1 = scalepair(agc_delaysignal(vp0f, pathi), gain);
			// Демодуляция АМ
			const FLOAT_t sample = SQRTF(vp1.IV * vp1.IV + vp1.QV * vp1.QV);// * (FLOAT_t) 0.5; //M_SQRT1_2;
			//saved_delta_fi [pathi] = demodulator_FM(vp2, pathi);	// погрешность настройки - требуется фильтровать ФНЧ
			r = sample * rxoutdenom;
		}
		else
			r = 0;
		break;

	case DSPCTL_MODE_RX_WFM:
		r = 0;
		break;

	case DSPCTL_MODE_RX_SAM:
		if (/*DUALRXFLT || */pathi == 0)
		{
			/* synchronous AM demodulation */
			// Здесь, имея квадратурные сигналы vp1.IV и vp1.QV, начинаем демодуляции
			const FLOAT_t gain = agc_forvard_float(dspmode, vp0f, pathi);
			const FLOAT32P_t vp1 = scalepair(agc_delaysignal(vp0f, pathi), gain);
			//const FLOAT_t sample = SQRTF(vp1.IV * vp1.IV + vp1.QV * vp1.QV) * (FLOAT_t) 0.5; //M_SQRT1_2;
			//saved_delta_fi [pathi] = demodulator_FM(vp2, pathi);	// погрешность настройки - требуется фильтровать ФНЧ
			// Демодуляция SАМ
			const FLOAT_t sample = demodulator_SAM(vp1, pathi);
			r = sample * rxoutdenom;
		}
		else
			r = 0;
		break;
	}
	return r;
}

// Расширение знакового 24/32 битного числа (left justified) до 32 бит
static RAMFUNC int_fast32_t q23lj_to_q31(uint32_t v)
{
	return ((int32_t) v >> (32 - WITHIFADCWIDTH));	// с кодека принято left justified 24 бита в 32-х битном числе
}

#if WITHDSPEXTDDC

// ПРИЁМ
// Обрабатывается 32-х битная квадратура
// Возвращается сэмпл - выход детектора
static FLOAT32P_t processifadcsampleIQ_ISB(
	const int32_t iv0, const int32_t qv0		// Квадратурные значения выборки
	)
{
	enum { pathi = 0 };				// 0/1: main_RX/sub_RX
	FLOAT32P_t rv = { 0 };

	return rv;
}

// ПРИЁМ
// Обрабатывается 32-х битная квадратура
// Возвращается сэмпл - выход детектора
static RAMFUNC FLOAT_t processifadcsampleIQ(
	const uint32_t iv0, const uint32_t qv0,		// Квадратурные значения выборки
	const uint_fast8_t dspmode, 
	const uint_fast8_t pathi				// 0/1: main_RX/sub_RX
	)
{
	if (isdspmoderx(dspmode))
	{
		FLOAT32P_t vp0 = { { q23lj_to_q31(iv0), q23lj_to_q31(qv0) } };
#if WITHDSPLOCALFIR
		BEGIN_STAMP();
		vp0 = filter_firp_rx_SSB_IQ(vp0);
		END_STAMP();
#endif /* WITHDSPLOCALFIR */
		return baseband_demodulator(vp0, dspmode, pathi);
	}
	else
	{
		return 0;
	}
}

#else /* WITHDSPEXTDDC */

// ПРИЁМ
// Обрабатывается 24-х битное число.
// Возвращается сэмпл - выход детектора
//
static RAMFUNC FLOAT_t processifadcsamplei(uint32_t v1, uint_fast8_t dspmode)
{
	const uint_fast8_t pathi = 0;

	if (isdspmoderx(dspmode))
	{
		// down-converter с 12 кГц на zero IF
		const FLOAT32P_t if_lo = get_float4_iflo();
		BEGIN_STAMP();
		const FLOAT32P_t vp0 = filter_fir4_rx_SSB_IQ(scalepair(if_lo, q23lj_to_q31(v1) * RXINSCALE), if_lo.IV != 0); // частота 12 кГц - 1/4 частоты выборок АЦП - можно воспользоваться целыми значениями.
		END_STAMP();
		return baseband_demodulator(vp0, dspmode, pathi);
	}
	else
	{
		return 0;
	}
}


#endif /* WITHDSPEXTDDC */

#if WITHLOOPBACKTEST

// Обеспечение работы тестового канала прохождения стерео звука
static INT32P_t loopbacktestaudio(INT32P_t vi0, uint_fast8_t dspmode, FLOAT_t shape)
{
	INT32P_t vi;
	vi = vi0;		// Прослушивание микрофонного сигнала
	//
	// Здесь выбираем, что прослушиваем при тесте
	BEGIN_STAMP2();
	//vi.IV = txmikeagc(vi.IV * txlevelfence / mikefence) / TXINSCALE;
	//vi.IV = filter_fir_rx_AUDIO_A(vi.IV);		// Прослушивание микрофонного сигнала через фильтр приёмника
	//vi.IV = filter_fir_tx_MIKE(vi.IV, 0);		// Прослушивание микрофонного сигнала через фильтр передатчика
	END_STAMP2();
	//vi.IV = 0;	// заглушить левый канал
	//vi.QV = 0;	// заглушить правый канал
	//vi.IV = get_dualtone16();	// двухтоновый сигнал

#if WITHSECTOGGLE

	// генерация переиодического сигнала
	{
		FLOAT_t v = (get_lout16() + get_rout16()) * shape / 2;
		vi.IV = v;
		vi.QV = v;
	}

#elif 0

	// Самопрослушивание микрофона
	vi.QV = vi.IV;	// в правый канал копируем левый канал (микрофон)

#else /* WITHSECTOGGLE */

	// Генерация двух тонов для разных каналов
	vi.IV = get_lout16();		// тон 700 Hz
	vi.QV = get_rout16();		// тон 500 Hz
	//vi.QV = vi.IV;	// в правый канал копируем левый канал (микрофон)

#endif /* WITHSECTOGGLE */

	//float f0 = ssb_iir(vi); // / 4; // * (0x7fff / (float) 0x16000);
	//const float f = filter_fir_tx_MIKE(vi0, 0); //ssb_iir2(vi) * (0x7ff0 / (float) 0x7fff);
	//const float f = vi;
	//const float denom = 0.99999f;

	//float f0 = ssb_iir(vi); // / 4; // * (0x7fff / (float) 0x16000);
	//const float f = filter_fir_tx_MIKE(vi, 0); //ssb_iir2(vi) * (0x7ff0 / (float) 0x7fff);
	//const float f = vi;
	//const float denom = 0.99999f;
	//float f1 = FABSF(vi);

	//float f2 = FABSF(f);
	//vmax = (f2 > vmax) ? f2 : vmax * denom;

	return vi;
}

#endif /* WITHLOOPBACKTEST */

static RAMFUNC INT32P_t getsampmlemike2(void)			/* получить очередной оцифрованый сэмпл с микрофона */
{
	INT32P_t v;
	if (getsampmlemike(& v) == 0)
	{
		v.IV = 0;
		v.IV = 0;
	}
	return v;
}

#if WITHDTMFPROCESSING

// === Определяем потребные блоки данных
// *** Буфера декодированных "блоков" TDM-каналов
// ***  (по 2 буфера на прием и передачу)


#define DTMF_STEPS              205 // Число шагов преобразования данных
                                    //  при DTMF-детекции
                                    //  (длина буфера данных одного канала приемника для
                                    //      анализа DTMF-информации)
#define DTMF_FREQ               8   // Число DTMF-частот
#define DTMF_EMPTY              (-1)  // Код отсутствия принятой цифры


static int16_t inp_samples [DTMF_STEPS];
static volatile int dtmfbi = 0;

static FLOAT_t powIQ(FLOAT_t i, FLOAT_t q)
{
	return SQRTF(i * i + q * q);
}


/******************************************************************************

        Function Name    : int16_t Dtmf_receive (DTMF_buffer_info *)

        Description      : Функция детекции DTMF-набора

        Parameters       : Указатель на буфер значений входного потока
                            фиксированной длины

        Returns          : Код определенной цифры

*******************************************************************************/
int16_t Dtmf_receive (const int16_t * samples)
{
	// *** Таблицы коэффициентов S и C для восьми частот DTMF (697-1633 Гц)
	static const FLOAT_t S_coeff [DTMF_FREQ] = 
	{
		0.520488f / 1, 0.568561f / 1, 0.620326f / 1, 0.673593f / 1,
		0.813151f / 1, 0.867070f / 1, 0.916816f / 1, 0.958745f / 1,
	};
	static const FLOAT_t C_coeff [DTMF_FREQ] = 
	{
		0.853868f / 1, 0.822640f / 1, 0.784343f / 1, 0.739102f / 1,
		0.582052f / 1, 0.498185f / 1, 0.399309f / 1, 0.284266f / 1,
	};

	// *** Таблица получения кода принятой DTMF-цифры
	static const int16_t Dtmf_code [16] = 
		{0x01,0x04,0x07,0x0B,0x02,0x05,0x08,0x0A,
		0x03,0x06,0x09,0x0C,0x0D,0x0E,0x0F,0x00};

	FLOAT_t         SinA [DTMF_FREQ]; // = { 0.0 };            // Вычисляемые значения синуса
	FLOAT_t         CosA [DTMF_FREQ]; // = { 0.0 };            //  и косинуса
	FLOAT_t         furie_image[DTMF_FREQ][2]; // = { { 0.0, 0.0 } };  // Вещественная и мнимая части частот
	FLOAT_t         powS [DTMF_FREQ];            // Вычисляемые значения синуса
	int     i,j;


    // *** Initialisation section ***
    // --- Инициализируем данные функции
    for (i = 0; i < DTMF_FREQ; ++i)
    {
        furie_image[i][0] = 0.0f;    // Вещественная часть
        furie_image[i][1] = 0.0f;    // Мнимая часть
        SinA[i] = 0.0f;               // Значение синуса
        CosA[i] = 0.7f;               // Значение косинуса
//        CosA[i] = 1.0f;               // Значение косинуса
    }
    // *** Обработка информации ***
    // Осуществляем вычисление вещественной и мнимой составляющих Фурье-образа
    //  по всем 8 частотам
    for ( i = 0; i < DTMF_STEPS; ++i )
    {   
		// Для каждого шага преобразования
		const float sample = samples [i];

        for ( j = 0; j < DTMF_FREQ; ++j )
        {   // Для каждой DTMF-частоты
            FLOAT_t Cosv = CosA [j];
            FLOAT_t Sinv = SinA [j];
			FLOAT_t sin_tmp = Sinv;

            furie_image[j][0] += sample * Cosv;   // Re
            furie_image[j][1] += sample * Sinv;   // Im
            SinA [j] = Sinv * C_coeff [j] + Cosv * S_coeff[j];
            CosA [j] = Cosv * C_coeff [j] - sin_tmp * S_coeff[j];
        }
    }

    // Вычисляем значения квадратов весов разложенных частот
    for ( j = 0; j < DTMF_FREQ; ++j )
    {   // Используем для хранения весов массив Sin
        powS [j] = powIQ(furie_image[j][0], furie_image[j][1]);
    }

    // Первые 4 значения powS [j] - веса младших 4-х частот,
    // вторые 4 значения powS [j] - веса старших 4-х частот,
    // Используем для определения частот с максимальным весом массив Cos []
	 int     il, ih;
	FLOAT_t tcos0;
	FLOAT_t tcos1;

	tcos0 = 0; //powS [0];
	il = 0;
	for ( j = 0; j < DTMF_FREQ/2; ++j )   // Определяем наибольший вес младших частот
        if ( powS[j] > tcos0 )
        {   tcos0 = powS[j];
            il = j;                 // Запоминаем индекс частоты максимального веса
        }

	tcos1 = 0; //powS [DTMF_FREQ/2];
	ih = 0; //DTMF_FREQ/2;
    for ( j = DTMF_FREQ/2; j < DTMF_FREQ; ++j )     // Определяем наибольший вес старших частот
        if ( powS[j] > tcos1 )
        {   tcos1 = powS[j];
            ih = j;                 // Запоминаем индекс частоты максимального веса
        }
	const FLOAT_t fence = 4;		// отношение мощностей
    // Проверяем превосходство найденных наибольших весов не менее, чем в 16 раз
    for ( i = 0; i < DTMF_FREQ / 2; ++ i )
    {   if ( i == il )  
            continue;
        if ( tcos0 / powS[i] < fence )  // Одна из младших частот не меньше максимальной
                                        //  как минимум в 16 раз
            return  DTMF_EMPTY;         // Условие надежного приема не выполнено
    }

    for ( i = DTMF_FREQ / 2; i < DTMF_FREQ; ++ i )
    {   if ( i == ih )
            continue;
        if ( tcos1 / powS [i] < fence )  // Одна из старших частот не меньше максимальной
                                        //  как минимум в 16 раз
            return  DTMF_EMPTY;         // Условие надежного приема не выполнено
    }

	if (tcos0 == 0 || tcos1 == 0)
		return DTMF_EMPTY;

    // Определяем и возвращаем принятую цифру
    return (Dtmf_code [il | (ih & 3) << 2]);
}

void dtmftest(void)
{
	//debug_printf_P(PSTR("."));
	if (dtmfbi >= DTMF_STEPS)
	{
		long t = 0;
		static const char digits [16] =
		{
			'D', '1', '2', '3', '4', '5', '6', '7', '8', '9',
			'0', '*', '#', 'A', 'B', 'C',
		};
		const int digit = Dtmf_receive(inp_samples);
		/*
		debug_printf_P("spectrum:[%8d %8d %8d %8d %8d %8d %8d %8d] ",
				(int) (powS [0] * 10),
				(int) (powS [1] * 10),
				(int) (powS [2] * 10),
				(int) (powS [3] * 10),
				(int) (powS [4] * 10),
				(int) (powS [5] * 10),
				(int) (powS [6] * 10),
				(int) (powS [7] * 10)
				);
		*/
		//t += nsaiirqsrx - tstart;
		if (digit != DTMF_EMPTY)
		{
			if (digit >= (sizeof digits / sizeof digits [0]))
				debug_printf_P(PSTR("dtmf: 0x%02x\n"), digit);
			else
				debug_printf_P(PSTR("dtmf: %c\n"), digit < 16 ? digits [digit] : 'Z');
		}
		else
		{
			//debug_printf_P(PSTR("dtmf: none\n"));
		}
		dtmfbi = 0;
	}
	else
	{
		//debug_printf_P(PSTR(":"));
	}
}

#endif /* WITHDTMFPROCESSING */

// Выдача в USB UAC
static RAMFUNC void recordsampleUAC(int left, int right)
{
#if WITHUSBUAC && ! WITHRTSNOAUDIO
	savesamplerecord16uacin(left, right);	// Запись демодулированного сигнала без озвучки клавиш в USB 
#endif /* WITHUSBUAC && ! WITHRTSNOAUDIO */
}

// Запись на SD CARD
static RAMFUNC void recordsampleSD(int left, int right)
{
#if WITHUSEAUDIOREC
	savesamplerecord16SD(left, right);	// Запись демодулированного сигнала без озвучки клавиш на SD CARD
#endif /* WITHUSEAUDIOREC */
}

static RAMFUNC uint_fast8_t isneedfiltering(uint_fast8_t dspmode)
{
	switch (dspmode)
	{
	case DSPCTL_MODE_RX_DSB:
	case DSPCTL_MODE_RX_WIDE:
	case DSPCTL_MODE_RX_NARROW:
	case DSPCTL_MODE_RX_FREEDV:
	case DSPCTL_MODE_RX_NFM:
	case DSPCTL_MODE_RX_AM:
	case DSPCTL_MODE_RX_SAM:
	case DSPCTL_MODE_RX_ISB:
	case DSPCTL_MODE_RX_WFM:
		return 1;

#if WITHUSBUAC
	case DSPCTL_MODE_RX_DRM:	// в этом режиме не проходит в наушники
		return 0;
#else /* WITHUSBUAC */
	case DSPCTL_MODE_RX_DRM:	// в этом режиме не проходит в наушники
		return 0;
#endif /* WITHUSBUAC */

	default:
		// in transmit modes
		return 0;
	}
}

static RAMFUNC uint_fast8_t isneedmute(uint_fast8_t dspmode)
{
	switch (dspmode)
	{
	case DSPCTL_MODE_RX_WIDE:
	case DSPCTL_MODE_RX_NARROW:
	case DSPCTL_MODE_RX_FREEDV:
	case DSPCTL_MODE_RX_NFM:
	case DSPCTL_MODE_RX_AM:
	case DSPCTL_MODE_RX_SAM:
	case DSPCTL_MODE_RX_ISB:
	case DSPCTL_MODE_RX_DSB:
	case DSPCTL_MODE_RX_WFM:
		return 0;

	case DSPCTL_MODE_RX_BPSK:
		return 1;

#if WITHUSBUAC
	case DSPCTL_MODE_RX_DRM:	// в этом режиме не проходит в наушники
		return 1;
#else /* WITHUSBUAC */
	case DSPCTL_MODE_RX_DRM:	// в этом режиме проходит в наушники
		return 0;
#endif /* WITHUSBUAC */

	default:
		// in transmit modes
		return 1;
	}
}

static RAMFUNC FLOAT_t filterRxAudio_rxA(FLOAT_t v, uint_fast8_t dspmode)
{
	return isneedmute(dspmode) ? 0 : (isneedfiltering(dspmode) ? filter_fir_rx_AUDIO_A(v) : v);
}

#if WITHUSEDUALWATCH

static RAMFUNC FLOAT_t filterRxAudio_rxB(FLOAT_t v, uint_fast8_t dspmode)
{
	return isneedmute(dspmode) ? 0 : (isneedfiltering(dspmode) ? filter_fir_rx_AUDIO_B(v) : v);
}

// фильтры разные - не можем позволить себе паралельную обработку
static RAMFUNC FLOAT32P_t filterRxAudio_Pair2(FLOAT32P_t v, uint_fast8_t dspmodeA, uint_fast8_t dspmodeB)
{
	const uint_fast8_t m0 = isneedmute(dspmodeA);
	const uint_fast8_t f0 = isneedfiltering(dspmodeA);
	const uint_fast8_t m1 = isneedmute(dspmodeB);
	const uint_fast8_t f1 = isneedfiltering(dspmodeB);
	if (f0 && f1 && ! m0 && ! m1)
	{
		return filter_fir_rx_AUDIO_Pair2(v);	// фильтрация пар значений разными фильтрами
	}
	else
	{
		FLOAT32P_t vFiltered;
		vFiltered.IV = m0 ? 0 : (f0 ? filter_fir_rx_AUDIO_A(v.IV) : v.IV);
		vFiltered.QV = m1 ? 0 : (f1 ? filter_fir_rx_AUDIO_B(v.QV) : v.QV);
		return vFiltered;
	}
}

// фильтры одинаковые - можем позволить себе паралельную обработку
static RAMFUNC FLOAT32P_t filterRxAudio_Pair(FLOAT32P_t v, uint_fast8_t dspmode)
{
	if (isneedmute(dspmode))
	{
		const FLOAT32P_t vFiltered = { { 0, 0 } };
		return vFiltered;
	}
	else if (isneedfiltering(dspmode))
		return filter_fir_rx_AUDIO_Pair(v);	// фильтрация пар значений одинаковым фильтром
	else
		return v;
}

#endif /* WITHUSEDUALWATCH */

#if (WITHRTS96 || WITHRTS192) && ! WITHTRANSPARENTIQ

// Поддержка панорпамы и водопада

enum { NTap256 = FFTSizeSpectrum };

static volatile uint_fast8_t rendering;

static struct Complex x256 [NTap256 * 2] = { { 0, 0 }, };
static uint_fast16_t fft_head = 0;

// формирование отображения спектра
void savesamplespectrum96stereo(FLOAT_t iv, FLOAT_t qv)
{
	if (rendering == 0)
	{
		const struct Complex NewSample = { iv, qv };

		// shift the old samples
		// fft_head -  Начало обрабатываемой части буфера
		// fft_head + NTap256 -  Позиция за концом обрабатываемого буфер
		fft_head = (fft_head == 0) ? (NTap256 - 1) : (fft_head - 1);
		x256 [fft_head] = x256 [fft_head + NTap256] = NewSample;

	}
}

//static uint_fast8_t	glob_waterfalrange = 64;
static FLOAT_t wnd256 [FFTSizeSpectrum];
static const FLOAT_t waterfalrange = 64;
static FLOAT_t toplogdb; // = LOG10F((FLOAT_t) INT32_MAX / waterfalrange); 

static void buildsigwnd(FLOAT_t *w, int n /* FFTSize */)
{
	int i;
	for (i = 0; i < n; i++)
	{
		w [i] = fir_design_window(i, n);
	}
}

static void adjustwmwp(struct Complex *w, int n /* FFTSize */)
{
	int i;
	for (i = 0; i < n; i++)
	{
		const FLOAT_t r = wnd256 [i]; //fir_design_window(i, n);
		w [i].real *= r;
		w [i].imag *= r;
	}
}

static FLOAT_t getmag2(const struct Complex * Sig)
{
	const FLOAT_t mag = SQRTF(Sig->real * Sig->real + Sig->imag * Sig->imag);
	return mag;
}

// преобразование индекса массива FFT
// в горизонтальную координату окна отображения спектра
static int mapfft2raster(
	int i,	/* FFT buffer index */
	int dx	/* Raster position */
	)
{
	const int xx = (i * (dx - 1)) / (FFTSizeSpectrum - 1);	// отображение большего диапазона индексов в меньший
	const int x = (xx + dx / 2) % dx;	// перемещение центральной частоты
	return x;
}

int dsp_mag2y(FLOAT_t mag, uint_fast16_t dy)
{
	if (mag < waterfalrange)
		return 0;
	const FLOAT_t v = mag / waterfalrange;
	int val = LOG10F(v) * ((int_fast16_t) dy - 1) / toplogdb;

	if (val >= dy) 
		val = dy - 1;
	else if (val < 0) 
		val = 0;
	return val;
}

// Копрование информации о спектре с текущую строку буфера 
// wfarray (преобразование к пикселям растра */
void dsp_getspectrumrow(
	FLOAT_t * const hbase,
	uint_fast16_t dx	// pixel X width of display window
	)
{
	uint_fast16_t i;
	struct Complex * const Sig = & x256 [fft_head];	// первый элемент массива комплексных чисел
	rendering = 1;
	adjustwmwp(Sig, FFTSizeSpectrum);
	IFFT(Sig, FFTSizeSpectrum, FFTSizeSpectrumM);

	// очистка строки буфера истории отображения
	for (i = 0; i < dx; ++ i)
	{
		FLOAT_t * const p1 = & hbase [i];
		* p1 = 0;
	}

	// копирование в буфер истории отображения
	for (i = 0; i < FFTSizeSpectrum; ++ i)
	{
		const int x = mapfft2raster(i, dx);
		const FLOAT_t v = getmag2(& Sig [i]);
		FLOAT_t * const p1 = & hbase [x];
		* p1 = FMAXF(* p1, v);
	}
	rendering = 0;

}

static void
dsp_rasterinitialize(void)
{
	buildsigwnd(wnd256, FFTSizeSpectrum);
	toplogdb = LOG10F((FLOAT_t) INT32_MAX / waterfalrange);
}

#endif /* (WITHRTS96 || WITHRTS192) && ! WITHTRANSPARENTIQ */

#if WITHDSPEXTDDC
// использование данных о спектре, передаваемых в общем фрейме
static void RAMFUNC 
saverts96(const uint32_t * buff)
{
#if WITHRTS96 && ! WITHTRANSPARENTIQ
#if WITHUSBHW && WITHUSBUAC
	// если используется конвертор на Rafael Micro R820T - требуется инверсия спектра
	if (glob_swapiq != glob_swaprts)
	{
		savesampleout96stereo(
			(int_fast32_t) buff [DMABUF32RTS0Q],	// previous
			(int_fast32_t) buff [DMABUF32RTS0I]
			);	
		savesampleout96stereo(
			(int_fast32_t) buff [DMABUF32RTS1Q],	// current
			(int_fast32_t) buff [DMABUF32RTS1I]
			);	
	}
	else
	{
		savesampleout96stereo(
			(int_fast32_t) buff [DMABUF32RTS0I],	// previous
			(int_fast32_t) buff [DMABUF32RTS0Q]
			);	
		savesampleout96stereo(
			(int_fast32_t) buff [DMABUF32RTS1I],	// current
			(int_fast32_t) buff [DMABUF32RTS1Q]
			);	
	}
#endif /* WITHUSBHW && WITHUSBUAC */

	// формирование отображения спектра
	// если используется конвертор на Rafael Micro R820T - требуется инверсия спектра
	if (glob_swaprts != 0)
	{
		savesamplespectrum96stereo(
			(int_fast32_t) buff [DMABUF32RTS0Q],	// previous
			(int_fast32_t) buff [DMABUF32RTS0I]
			);	
		savesamplespectrum96stereo(
			(int_fast32_t) buff [DMABUF32RTS1Q],	// current
			(int_fast32_t) buff [DMABUF32RTS1I]
			);	
	}
	else
	{
		savesamplespectrum96stereo(
			(int_fast32_t) buff [DMABUF32RTS0I],	// previous
			(int_fast32_t) buff [DMABUF32RTS0Q]
			);	
		savesamplespectrum96stereo(
			(int_fast32_t) buff [DMABUF32RTS1I],	// current
			(int_fast32_t) buff [DMABUF32RTS1Q]
			);	
	}

#endif /* WITHRTS96 && ! WITHTRANSPARENTIQ */
}

#endif /* WITHDSPEXTDDC */

static FLOAT_t
getmoni(
	uint_fast8_t dspmode, 
	FLOAT_t sdtn, 
	FLOAT_t mike
	)
{
	switch (dspmode)
	{
	default:
		return sdtn;

	case DSPCTL_MODE_TX_SSB:
	case DSPCTL_MODE_TX_FREEDV:
	case DSPCTL_MODE_TX_AM:
	case DSPCTL_MODE_TX_NFM:
		return mike;
	}
}

// Обработка полученного от DMA буфера с выборками или квадратурами (или двухканальный приём).
void RAMFUNC dsp_extbuffer32rx(const uint32_t * buff)
{
	ASSERT(buff != NULL);
	ASSERT(gwprof < NPROF);
	const uint_fast8_t dspmodeA = globDSPMode [gwprof] [0];
	const uint_fast8_t tx = isdspmodetx(dspmodeA);
#if WITHUSEDUALWATCH
	const uint_fast8_t dspmodeB = tx ? DSPCTL_MODE_IDLE : globDSPMode [gwprof] [1];
#endif /* WITHUSEDUALWATCH */
	unsigned i;
	const int rxgate = getRxGate();


	for (i = 0; i < DMABUFFSIZE32RX; i += DMABUFSTEP32RX)
	{
	#if ! WITHTRANSPARENTIQ
		const FLOAT_t ctcss = get_float_subtone() * txlevelfence;
		const FLOAT_t sdtn = get_float_sidetone() * phonefence * shapeSidetoneStep();	// Здесь значение выборки в диапазоне, допустимом для кодека
		const INT32P_t vi = getsampmlemike2();	// с микрофона (или 0, если ещё не запустился) */
		const FLOAT_t moni = getmoni(dspmodeA, sdtn, vi.IV);
		const FLOAT_t shape = shapeCWEnvelopStep() * scaleDAC;	// 0..1
	#endif /* ! WITHTRANSPARENTIQ */

#if WITHSUSBSPKONLY
		// тестирование в режиме USB SPEAKER

		if (isdspmodetx(dspmodeA))
		{
			//const INT32P_t dual = loopbacktestaudio(vi, dspmodeA, shape);
			INT32P_t dual;
			dual.IV = get_lout16();		// тон 700 Hz
			dual.QV = get_rout16();		// тон 500 Hz
			savesampleout32stereo(iq2tx(dual.IV), iq2tx(dual.QV));	// кодек получает 24 бита left justified в 32-х битном числе.
			//savesampleout16stereo(injectsidetone(dual.IV, sdtn), injectsidetone(dual.QV, sdtn));
			recordsampleUAC(dual.IV, dual.QV);	// Запись в UAC демодулированного сигнала без озвучки клавиш
		}
		else if (isdspmoderx(dspmodeA))
		{
			//const INT32P_t dual = loopbacktestaudio(vi, dspmodeA, shape);
			const INT32P_t dual = vi;
			savesampleout32stereo(iq2tx(dual.IV), iq2tx(dual.QV));	// кодек получает 24 бита left justified в 32-х битном числе.
			//savesampleout16stereo(injectsidetone(dual.IV, sdtn), injectsidetone(dual.QV, sdtn));
			recordsampleUAC(get_lout16(), get_rout16());	// Запись в UAC демодулированного сигнала без озвучки клавиш
		}
		else
		{
			savesampleout32stereo(iq2tx(0), iq2tx(0));	// кодек получает 24 бита left justified в 32-х битном числе.
		}

#elif WITHTRANSPARENTIQ
		/* процессор просто поддерживает двунаправленный обмен между USB и FPGA */
	
		savesampleout96stereo(
			(int_fast32_t) buff [i + DMABUF32RX0I],
			(int_fast32_t) buff [i + DMABUF32RX0Q]
			);

		static INT32P_t vi;
		static uint_fast8_t outupsamplecnt;
		if ((outupsamplecnt ++ & 0x01) == 0)	// в сторону передатчика идут 96 кГц фреймы
		{
			vi = getsampmlemike2();	// с микрофона (или 0, если ещё не запустился) */
		}
		savesampleout32stereo(vi.IV << 16, vi.QV << 16);

#elif WITHDTMFPROCESSING
		// тестирование распозначания DTMF

		INT32P_t dual;
		//dual.IV = vi.IV; //get_lout16();
		dual.IV = get_lout16();
		dual.QV = 0;
		processafadcsampleiq(dual, dspmodeA, shape, ctcss);	// обработка одного сэмпла с микрофона
		// Тестирование распознавания DTMF
		if (dtmfbi < DTMF_STEPS)
		{
			inp_samples [dtmfbi] = vi.IV;
			++ dtmfbi;
		}
		savesampleout16stereo(injectsidetone(dual.IV, sdtn), injectsidetone(dual.QV, sdtn));

#elif WITHDSPEXTDDC
	// Режимы трансиверов с внешним DDC

		saverts96(buff + i);	// использование данных о спектре, передаваемых в общем фрейме

	#if WITHLOOPBACKTEST

		const INT32P_t dual = loopbacktestaudio(vi, dspmodeA, shape);
		processafadcsampleiq(dual, dspmodeA, shape, ctcss);	// обработка одного сэмпла с микрофона
		//
		// Тестирование источников и потребителей звука
		recordsampleSD(dual.IV, dual.QV);	// Запись демодулированного сигнала без озвучки клавиш
		recordsampleUAC(dual.IV, dual.QV);	// Запись в UAC демодулированного сигнала без озвучки клавиш
		savesampleout16stereo(injectsidetone(dual.IV, sdtn), injectsidetone(dual.QV, sdtn));
		//savesampleout16stereo(dual.IV, dual.QV);

	#elif WITHUSBHEADSET
		/* трансивер работает USB гарнитурой для компютера - режим тестирования */

		recordsampleUAC(get_lout16(), get_rout16());	// Запись в UAC демодулированного сигнала без озвучки клавиш
		savesampleout16stereo(vi.IV, vi.QV);	/* к line output подключен модем - озвучку запрещаем */
		//savesampleout16stereo(injectsidetone(vi.IV, sdtn), injectsidetone(vi.QV, sdtn));
		savesampleout32stereo(iq2tx(0), iq2tx(0));

	#elif WITHUSEDUALWATCH

		processafadcsampleiq(vi, dspmodeA, shape, ctcss);	// Передатчик - обработка одного сэмпла с микрофона
		//
		// Двухканальный приёмник

		if (dspmodeA == DSPCTL_MODE_RX_ISB)
		{
			/* прием независимых боковых полос */
			// Обработка буфера с парами значений
			const FLOAT32P_t pair = processifadcsampleIQ_ISB(
				(int_fast32_t) buff [i + DMABUF32RX0I] * rxgate,	// Расширяем 24-х битные числа до 32 бит
				(int_fast32_t) buff [i + DMABUF32RX0Q] * rxgate	// Расширяем 24-х битные числа до 32 бит
				);	
			const FLOAT32P_t filtered = filterRxAudio_Pair(pair, dspmodeA);
			recordsampleSD(filtered.IV, filtered.QV);	// Запись демодулированного сигнала без озвучки клавиш
			recordsampleUAC(filtered.IV, filtered.QV);	// Запись в UAC демодулированного сигнала без озвучки клавиш
			savesampleout16stereo(filtered.IV, filtered.QV);	/* к line output подключен модем - озвучку запрещаем */
		}
		else
		{
			// buff data layout: I main/I sub/Q main/Q sub
			const FLOAT_t rxA = processifadcsampleIQ(
				(int_fast32_t) buff [i + DMABUF32RX0I] * rxgate,	// Расширяем 24-х битные числа до 32 бит
				(int_fast32_t) buff [i + DMABUF32RX0Q] * rxgate,	// Расширяем 24-х битные числа до 32 бит
				dspmodeA,
				0	// MAIN RX
				);

			const FLOAT_t rxB = processifadcsampleIQ(
				(int_fast32_t) buff [i + DMABUF32RX1I] * rxgate,	// Расширяем 24-х битные числа до 32 бит
				(int_fast32_t) buff [i + DMABUF32RX1Q] * rxgate,	// Расширяем 24-х битные числа до 32 бит
				dspmodeB,
				1	// SUB RX
				);	


			switch (glob_mainsubrxmode)
			{
			default:
			case BOARD_RXMAINSUB_A_A:
				// left:A/right:A
				{
					const FLOAT_t filtered = filterRxAudio_rxA(rxA, dspmodeA);

					recordsampleSD(filtered, filtered);	// Запись демодулированного сигнала без озвучки клавиш
					recordsampleUAC(filtered, filtered);	// Запись в UAC демодулированного сигнала без озвучки клавиш
					savesampleout16stereo(injectsidetone(filtered, sdtn), injectsidetone(filtered, sdtn));
				}
				break;

			case BOARD_RXMAINSUB_A_B:
				// left:A/right:B
				if /*(0)*/(dspmodeA == dspmodeB)
				{
					// фильтры одинаковые - можем позволить себе паралельную обработку
					const FLOAT32P_t pair = { { rxA, rxB } };
					const FLOAT32P_t filtered = filterRxAudio_Pair(pair, dspmodeA);

					recordsampleSD(filtered.IV, filtered.QV);	// Запись демодулированного сигнала без озвучки клавиш
					recordsampleUAC(filtered.IV, filtered.QV);	// Запись в UAC демодулированного сигнала без озвучки клавиш
					savesampleout16stereo(injectsidetone(filtered.IV, sdtn), injectsidetone(filtered.QV, sdtn));
				}
				else
				{
					const FLOAT32P_t pair = { { rxA, rxB } };
					const FLOAT32P_t filtered = filterRxAudio_Pair2(pair, dspmodeA, dspmodeB);

					recordsampleSD(filtered.IV, filtered.QV);	// Запись демодулированного сигнала без озвучки клавиш
					recordsampleUAC(filtered.IV, filtered.QV);	// Запись в UAC демодулированного сигнала без озвучки клавиш
					savesampleout16stereo(injectsidetone(filtered.IV, sdtn), injectsidetone(filtered.QV, sdtn));
				}
				break;

			case BOARD_RXMAINSUB_B_A:
				// left:B/right:A
				if /*(0)*/(dspmodeA == dspmodeB)
				{
					// фильтры одинаковые - можем позволить себе паралельную обработку
					const FLOAT32P_t pair = { { rxA, rxB } };
					const FLOAT32P_t filtered = filterRxAudio_Pair(pair, dspmodeA);

					recordsampleSD(filtered.QV, filtered.IV);	// Запись демодулированного сигнала без озвучки клавиш
					recordsampleUAC(filtered.QV, filtered.IV);	// Запись в UAC демодулированного сигнала без озвучки клавиш
					savesampleout16stereo(injectsidetone(filtered.QV, sdtn), injectsidetone(filtered.IV, sdtn));
				}
				else
				{
					const FLOAT32P_t pair = { { rxA, rxB } };
					const FLOAT32P_t filtered = filterRxAudio_Pair2(pair, dspmodeA, dspmodeB);

					recordsampleSD(filtered.QV, filtered.IV);	// Запись демодулированного сигнала без озвучки клавиш
					recordsampleUAC(filtered.QV, filtered.IV);	// Запись в UAC демодулированного сигнала без озвучки клавиш
					savesampleout16stereo(injectsidetone(filtered.QV, sdtn), injectsidetone(filtered.IV, sdtn));
				}
				break;

			case BOARD_RXMAINSUB_B_B:
				// left:B/right:B
				{
					const FLOAT_t filtered = filterRxAudio_rxB(rxB, dspmodeB);	// todo: пока только с фильтром

					recordsampleSD(filtered, filtered);	// Запись демодулированного сигнала без озвучки клавиш
					recordsampleUAC(filtered, filtered);	// Запись в UAC демодулированного сигнала без озвучки клавиш
					savesampleout16stereo(injectsidetone(filtered, sdtn), injectsidetone(filtered, sdtn));
				}
				break;
			}
		}

	#else /* WITHUSEDUALWATCH */

		processafadcsampleiq(vi, dspmodeA, shape, ctcss);	// Передатчик - обработка одного сэмпла с микрофона
		// Одноканальный приёмник

		if (dspmodeA == DSPCTL_MODE_RX_WFM)
		{
			/* прием WFM (демодуляция в FPGA, только без WITHUSEDUALWATCH)	*/
			const FLOAT_t left = (int_fast32_t) buff [i + DMABUF32RX1I] * rxgate;		// Расширяем 24-х битные числа до 32 бит
			//const FLOAT_t right = (int_fast32_t) buff [i + DMABUF32RX1Q] * rxgate;		// Расширяем 24-х битные числа до 32 бит
			//const FLOAT_t leftFiltered = filterRxAudio_rxA(left * nfmoutscale, dspmodeA);
			const FLOAT_t leftFiltered = filterRxAudio_rxA(get_lout16(), dspmodeA);	// TODO: debug
			recordsampleSD(leftFiltered, leftFiltered);	// Запись демодулированного сигнала без озвучки клавиш
			recordsampleUAC(leftFiltered, leftFiltered);	// Запись в UAC демодулированного сигнала без озвучки клавиш
			savesampleout16stereo(injectsidetone(leftFiltered, sdtn), injectsidetone(leftFiltered, sdtn));
		}
		else if (dspmodeA == DSPCTL_MODE_RX_ISB)
		{
			/* прием независимых боковых полос */
			// Обработка буфера с парами значений
			const FLOAT32P_t rv = processifadcsampleIQ_ISB(
				(int_fast32_t) buff [i + DMABUF32RX0I] * rxgate,	// Расширяем 24-х битные числа до 32 бит
				(int_fast32_t) buff [i + DMABUF32RX0Q] * rxgate	// Расширяем 24-х битные числа до 32 бит
				);	
			recordsampleSD(rv.IV, rv.QV);	// Запись демодулированного сигнала без озвучки клавиш
			recordsampleUAC(rv.IV, rv.QV);	// Запись в UAC демодулированного сигнала без озвучки клавиш
			savesampleout16stereo(rv.IV, rv.QV);	/* к line output подключен модем - озвучку запрещаем */
		}
		else
		{
			// Обработка буфера с парами значений
			const FLOAT_t left = processifadcsampleIQ(
				(int_fast32_t) buff [i + DMABUF32RX0I] * rxgate,	// Расширяем 24-х битные числа до 32 бит
				(int_fast32_t) buff [i + DMABUF32RX0Q] * rxgate,	// Расширяем 24-х битные числа до 32 бит
				dspmodeA,
				0		// MAIN RX
				);	

			const FLOAT_t leftFiltered = filterRxAudio_rxA(left, dspmodeA);
			recordsampleSD(leftFiltered, leftFiltered);	// Запись демодулированного сигнала без озвучки клавиш
			recordsampleUAC(leftFiltered, leftFiltered);	// Запись в UAC демодулированного сигнала без озвучки клавиш
			savesampleout16stereo(injectsidetone(leftFiltered, sdtn), injectsidetone(leftFiltered, sdtn));
		}

	#endif /*  DMABUFSTEP32 == 4 */

#else /* WITHDSPEXTDDC */
	// Режимы трансиверов без внешнкго DDC

	#if WITHLOOPBACKTEST

		const INT32P_t dual = loopbacktestaudio(vi, dspmodeA, shape);
		processafadcsample(dual, dspmodeA, shape, ctcss);	// обработка одного сэмпла с микрофона
		//
		// Тестирование источников и потребителей звука
		recordsampleSD(dual.IV, dual.QV);	// Запись демодулированного сигнала без озвучки клавиш
		recordsampleUAC(dual.IV, dual.QV);	// Запись в UAC демодулированного сигнала без озвучки клавиш
		savesampleout16stereo(injectsidetone(dual.IV, sdtn), injectsidetone(dual.QV, sdtn));

	#else /* WITHLOOPBACKTEST */

		processafadcsample(vi, dspmodeA, shape, ctcss);	// Передатчик - использование принятого с AF ADC буфера

		const FLOAT_t left = processifadcsamplei(buff [i + DMABUF32RX] * rxgate, dspmodeA);	// Расширяем 24-х битные числа до 32 бит
		const FLOAT_t leftFiltered = filterRxAudio_rxA(left, dspmodeA);
		recordsampleSD(leftFiltered, leftFiltered);	// Запись демодулированного сигнала без озвучки клавиш
		recordsampleUAC(leftFiltered, leftFiltered);	// Запись в UAC демодулированного сигнала без озвучки клавиш
		savesampleout16stereo(injectsidetone(leftFiltered, sdtn), injectsidetone(leftFiltered, sdtn));

	#endif /* WITHLOOPBACKTEST */
#endif /* WITHDSPEXTDDC */
	}
}

//////////////////////////////////////////
// glob_cwedgetime - длительность нарастания/спада огибающей CW (и сигнала самоконтроля) в единицах милисекунд

static unsigned enveloplen = NSAITICKS(5) + 1;	/* Изменяется через меню. */
static unsigned shapeSidetonePos = 0;
static volatile uint_fast8_t shapeSidetoneInpit = 0;

static unsigned shapeCWEnvelopPos = 0;
static volatile uint_fast8_t cwgateflag = 0;
static volatile uint_fast8_t rxgateflag = 0;

// 0..1
static RAMFUNC FLOAT_t peakshapef(unsigned shapePos)	/* 0 <= shapePos <= enveloplen */
{
	const FLOAT_t halflevel = (FLOAT_t) 0.5;
	const unsigned halflen = sizeof sintable4f_fs / sizeof sintable4f_fs [0] - 1;	// таблица для одного квадранта
	const unsigned i = shapePos * (halflen * 2 - 1) / enveloplen;
	const FLOAT_t v = (i <= halflen) ? (halflevel - sintable4f_fs [halflen - i] * halflevel) : (halflevel + sintable4f_fs [i - halflen] * halflevel);
	// Возведение в степень 4 - идея от Oleg Skidan
	return v * v * v * v;
}

// Формирование огибающей для самоконтрола
// 0..1
static RAMFUNC FLOAT_t shapeSidetoneStep(void)
{
	/* при регулировке длительности нарастания/спада из меню текущая позиция не корректируется */
	if (shapeSidetoneInpit >= enveloplen)
		shapeSidetoneInpit = enveloplen;

	const FLOAT_t v = peakshapef(shapeSidetonePos);
	
	if (shapeSidetoneInpit != 0)
		shapeSidetonePos = shapeSidetonePos >= enveloplen ? enveloplen : (shapeSidetonePos + 1);
	else
		shapeSidetonePos = shapeSidetonePos == 0 ? 0 : (shapeSidetonePos - 1);
	return v;
}

// Формирование огибающей для передачи
// 0..1
static RAMFUNC FLOAT_t shapeCWEnvelopStep(void)
{
	/* при регулировке длительности нарастания/спада из меню текущая позиция не корректируется */
	if (shapeCWEnvelopPos >= enveloplen)
		shapeCWEnvelopPos = enveloplen;
	const FLOAT_t v = peakshapef(shapeCWEnvelopPos);

	if (cwgateflag != 0)
		shapeCWEnvelopPos = shapeCWEnvelopPos >= enveloplen ? enveloplen : (shapeCWEnvelopPos + 1);
	else
		shapeCWEnvelopPos = shapeCWEnvelopPos == 0 ? 0 : (shapeCWEnvelopPos - 1);
	return v;
}

// Возврат признака того, что передавать модему ещё рано - не полностью завершено формирование огибающей
static RAMFUNC uint_fast8_t getTxShapeNotComplete(void)
{
	/* при регулировке длительности нарастания/спада из меню текущая позиция не корректируется */
	if (shapeCWEnvelopPos >= enveloplen)
		shapeCWEnvelopPos = enveloplen;
	return shapeCWEnvelopPos != enveloplen;
}

/* разрешение работы тракта в режиме приёма */
static uint_fast8_t getRxGate(void)
{
#if TXGFV_RX != 0
	return rxgateflag;
#else
	return ! cwgateflag;
#endif
}


// манипуляция и переключение на передачу
void dsp_txpath_set(portholder_t txpathstate)
{
#if WITHINTEGRATEDDSP
	#if WITHTX
		cwgateflag = (txpathstate & (TXGFV_TX_CW | TXGFV_TX_SSB | TXGFV_TX_AM | TXGFV_TX_NFM)) != 0;
		#if TXGFV_RX != 0
			rxgateflag = (txpathstate & (TXGFV_RX)) != 0;
		#endif /* TXGFV_RX != 0 */
	#else /* WITHTX */
		rxgateflag = 1;
	#endif /* WITHTX */
#endif /* WITHINTEGRATEDDSP */
}

//////////////////////////////////////////

/* после изменения набора формируемых звуков - обновление программирования таймера. */
void hardware_sounds_disable(void)
{
	//anglestep_sidetone = 0;
	shapeSidetoneInpit = 0;
}

#if SIDETONE_TARGET_BIT != 0
	#error SIDETONE_TARGET_BIT != 0
#endif
// called from interrupt or with disabled interrupts
// всегда включаем генерацию выходного сигнала
void hardware_sounds_setfreq(
	uint_fast8_t prei,		// returned value from hardware_calc_sound_params
	unsigned value
	)
{
	anglestep_sidetone = value;
	shapeSidetoneInpit = 1;
}

// return code: prescaler
uint_fast8_t
hardware_calc_sound_params(
	uint_least16_t tonefreq,	/* tonefreq - частота в десятых долях герца. Минимум - 400 герц (определено набором команд CAT). */
	unsigned * pvalue)
{
	* pvalue = FTWAF001(tonefreq * 10u);	// В сотых долях герца
	return 0;
}

/* Получить значение входной ПЧ для обработки DSP */
int_fast32_t
dsp_get_ifreq(void)
{
#if WITHDSPEXTDDC
	return 0;
#else /* WITHDSPEXTDDC */
	return ARMSAIRATE / 4;
#endif /* WITHDSPEXTDDC */
}

/* Получить значение частоты выборок выходного потока DSP */
int_fast32_t
dsp_get_sampleraterx(void)
{
	return ARMI2SRATE;
}

/* Получить значение частоты выборок выходного потока DSP, увеличенное в scale раз */
int_fast32_t
dsp_get_sampleraterxscaled(uint_fast8_t scale)
{
	return ARMI2SRATEX(scale);
}

/* Получить значение частоты выборок входного потока DSP */
int_fast32_t
dsp_get_sampleratetx(void)
{
	return ARMI2SRATE;
}

/* Получить значение частоты выборок выходного потока DSP */
int_fast32_t
dsp_get_samplerate100(void)
{
	return ARMI2SRATE100;
}

// Передача параметров в DSP модуль
// Обновление параметров приемника (кроме фильтров).
static void 
rxparam_update(uint_fast8_t profile, uint_fast8_t pathi)
{
	// Параметры АРУ приёмника
	{
		const int gainmax = glob_digigainmax;	// Верхний предел регулировки усиления
		const int gainmin = 0;	// Нижний предел регулировки усиления
		const int gaindb = ((gainmax - gainmin) * (int) (glob_rfgain - BOARD_RFGAIN_MIN) / (int) (BOARD_RFGAIN_MAX - BOARD_RFGAIN_MIN)) + gainmin;	// -20..+100 dB
		const FLOAT_t manualrfgain = db2ratio(gaindb);
		
		agc_parameters_update(& rxagcparams [profile] [pathi], manualrfgain, pathi);	// приёмник #0,#1
	}

	// Параметры SAM приёмника
	{
		const int pll = 4000;
		const int zeta_help = 65;
		const FLOAT_t zeta = (FLOAT_t) zeta_help / (FLOAT_t) 100; // PLL step response: smaller, slower response 1.0 - 0.1
		const FLOAT_t omegaN = 200; // PLL bandwidth 50.0 - 1000.0
		const FLOAT_t tauR = (FLOAT_t) 0.02; // original 0.02;
		const FLOAT_t tauI = (FLOAT_t) 1.4; // original 1.4;  
		create_amd(& amds [pathi], 0, - pll, + pll, zeta, omegaN, tauR, tauI);
	}
	// шумодав NFM
	nbfence = POWF(2, WITHIFADCWIDTH - 8) * (int) glob_nfm_sql_lelel;	// glob_nfm_sql_lelel: 0..255

	// Уровень сигнала самоконтроля
	sidetonevolume = (glob_sidetonelevel / (FLOAT_t) 100);
	mainvolumerx = 1 - sidetonevolume;

}
// Передача параметров в DSP модуль
// Обновление параметров передатчика (кроме фильтров).
static void 
txparam_update(uint_fast8_t profile)
{

	// Параметры АРУ микрофона
	comp_parameters_update(& txagcparams [profile], (int) glob_mikeagcgain);

	{
		// Настройка ограничителя
		const FLOAT_t FS = txagcparams [profile].levelfence;
		const FLOAT_t grade = 1 - (glob_mikehclip / (FLOAT_t) 100);
		mickeclipscale [profile] = 1 / grade;
		mickecliplevelp [profile] = FS * grade;
		mickeclipleveln [profile] = - FS * grade;
	}

	{
		// AM parameters
		const FLOAT_t amshapesignal = (FLOAT_t) (int) glob_amdepth / (100 + (int) glob_amdepth);
		amshapesignalHALF = amshapesignal / 2;
		amcarrierHALF = txlevelfenceHALF - txlevelfenceHALF * amshapesignal;
	}

	scaleDAC = (FLOAT_t) (int) glob_dacscale / 100;

	subtonevolume = (glob_subtonelevel / (FLOAT_t) 100);
	mainvolumetx = 1 - subtonevolume;

#if WITHCPUDACHW && WITHPOWERTRIM
	// ALC
	// регулировка напряжения на REFERENCE INPUT TXDAC AD9744 
	HARDWARE_DAC_ALC((glob_opowerlevel - WITHPOWERTRIMMIN) * dac_dacfs_coderange / (WITHPOWERTRIMMAX - WITHPOWERTRIMMIN) + dac_dacfs_lowcode);
#endif /* WITHCPUDACHW && WITHPOWERTRIM */
}

// Передача параметров в DSP модуль
static void 
trxparam_update(void)
{
	// CW & sidetone edge
	enveloplen = NSAITICKS(glob_cwedgetime) + 1;		/* количество сэмплов, за которое меняется огибающая */

}

/* вызывается при разрешённых прерываниях. */
void dsp_initialize(void)
{
	FFT_initialize();
#if (WITHRTS96 || WITHRTS192) && ! WITHTRANSPARENTIQ
	dsp_rasterinitialize();
#endif /* WITHRTS96 && ! WITHTRANSPARENTIQ */

	fir_design_windowbuff(FIRCwnd_tx_MIKE, Ntap_tx_MIKE);
	fir_design_windowbuff(FIRCwnd_rx_AUDIO, Ntap_rx_AUDIO);

#if WITHDSPEXTFIR
	#if WITHDOUBLEFIRCOEFS && (__ARM_FP & 0x08)
		fir_design_windowbuffL(FIRCwndL_trxi_IQ, Ntap_trxi_IQ);
	#else
		fir_design_windowbuff(FIRCwnd_trxi_IQ, Ntap_trxi_IQ);
	#endif
#endif /* WITHDSPEXTFIR */

#if WITHDSPLOCALFIR	
	fir_design_windowbuff(FIRCwnd_rx_SSB_IQ, Ntap_rx_SSB_IQ);
	fir_design_windowbuff(FIRCwnd_tx_SSB_IQ, Ntap_tx_SSB_IQ);
#endif /* WITHDSPLOCALFIR */

	omega2ftw_k1 = POWF(2, NCOFTWBITS);

	// 0.707 == M_SQRT1_2
	/* http://gregstoll.dyndns.org/~gregstoll/floattohex/ use for tests */

	// Разрядность передающего тракта
	#if WITHIFDACWIDTH > DSP_FLOAT_BITSMANTISSA
		const int_fast32_t dacFS = 0x7ffff000L >> (32 - WITHIFDACWIDTH);	/* 0x7ffff800L так как float имеет максимум 24 бита в мантиссе (23 явных и один - старший - подразумевается всегда единица) */
	#else /* WITHIFDACWIDTH > DSP_FLOAT_BITSMANTISSA */
		const int_fast32_t dacFS = (((uint_fast64_t) 1 << (WITHIFDACWIDTH - 1)) - 1);
	#endif /* WITHIFDACWIDTH > DSP_FLOAT_BITSMANTISSA */

	txlevelfence = dacFS * db2ratio(- (FLOAT_t) 1.75) * (FLOAT_t) M_SQRT1_2;	// контролировать по отсутствию индикации переполнения DUC при передаче
	txlevelfenceSSB_INTEGER = txlevelfence;	// Для источника шума
	txlevelfenceHALF = txlevelfence / 2;	// Для режимов с lo6=0 - у которых нет подавления нерабочей боковой

	txlevelfenceSSB = txlevelfence;
#if WITHTXDACFULL
	txlevelfenceBPSK = txlevelfence;
	txlevelfenceNFM = txlevelfence;
	txlevelfenceCW = txlevelfence;
#else /* WITHTXDACFULL */
	txlevelfenceBPSK = txlevelfence / 2;
	txlevelfenceNFM = txlevelfence / 2;
	txlevelfenceCW = txlevelfence / 2;
#endif /* WITHTXDACFULL */

	// Разрядность приёмного тракта
	#if WITHIFADCWIDTH > DSP_FLOAT_BITSMANTISSA
		const int_fast32_t adcFS = (0x7ffff000L >> (32 - WITHIFADCWIDTH));	/* 0x7ffff800L так как float имеет максимум 24 бита в мантиссе (23 явных и один - старший - подразумевается всегда единица) */
	#else /* WITHIFADCWIDTH > DSP_FLOAT_BITSMANTISSA */
		const int_fast32_t adcFS = (((uint_fast64_t) 1 << (WITHIFADCWIDTH - 1)) - 1);
	#endif /* WITHIFADCWIDTH > DSP_FLOAT_BITSMANTISSA */

	rxlevelfence = adcFS * db2ratio(- 1);
	// Разрядность поступающего с микрофона сигнала
	mikefence = POWF(2, WITHAFADCWIDTH - 1) - 1;

	// Разрядность поступающего на наушники сигнала
	phonefence = (POWF(2, WITHAFDACWIDTH - 1) - 1)  * db2ratio(- 1);
	// масштабирование (INT32_MAX + 1) к phonefence
	nfmoutscale = POWF(2, - (int) (NCOFTWBITS - 1)) * phonefence;

	agc_initialize();
	voxmeter_initialize();
	trxparam_update();
	const uint_fast8_t rprofile = ! gwagcprofrx;	// индекс профиля, который станет рабочим

	uint_fast8_t pathi;
	for (pathi = 0; pathi < NTRX; ++ pathi)
		rxparam_update(rprofile, pathi);
	gwagcprofrx = rprofile;

	const uint_fast8_t tprofile = ! gwagcproftx;	// индекс профиля, который станет рабочим
	txparam_update(tprofile);
	gwagcproftx = tprofile;

	const uint_fast8_t spf = ! gwprof;	// индекс профиля для подготовки параметров DSP

	audio_setup_mike(spf);
	for (pathi = 0; pathi < NTRX; ++ pathi)
		audio_update(spf, pathi);
	gwprof = spf;

	modem_update();

	debug_printf_P(PSTR("dsp_initialize: ARMI2SRATE=%lu\n"), (unsigned long) ARMI2SRATE);
	debug_printf_P(PSTR("dsp_initialize: ARMI2SRATE100=%lu.%02lu\n"), (unsigned long) (ARMI2SRATE100 / 100), (unsigned long) (ARMI2SRATE100 % 100));

#if 0
	{
		float32_t vs [4] = { 1, 2, 3, 4 };
		float32x4_t v1 = vld1q_f32(vs);
		float32x4_t v2 = vrev64q_f32(v1);
		// output: neontest: 2 1 4 3
		debug_printf_P(PSTR("neontest: %d %d %d %d\n"), (int) vgetq_lane_f32(v2, 0), (int) vgetq_lane_f32(v2, 1), (int) vgetq_lane_f32(v2, 2), (int) vgetq_lane_f32(v2, 3));
	}
#endif
}

void 
prog_dsplreg(void)
{
	const uint_fast8_t pathn = isdspmodetx(glob_dspmodes [0]) ? 1 : NTRX;	// при передаче только тракт с идексом 0
	trxparam_update();
	const uint_fast8_t rprofile = ! gwagcprofrx;	// индекс профиля, который станет рабочим
	uint_fast8_t pathi;
	for (pathi = 0; pathi < pathn; ++ pathi)
		rxparam_update(rprofile, pathi);
	gwagcprofrx = rprofile;

	const uint_fast8_t tprofile = ! gwagcproftx;	// индекс профиля, который станет рабочим
	txparam_update(tprofile);
	gwagcproftx = tprofile;

#if (CTLSTYLE_RAVENDSP_V1 || CTLSTYLE_DSPV1A)
	if (isdspmoderx(glob_dspmodes [0]))
	{
		// усиление тракта ПЧ (AD605)
		HARDWARE_DAC_AGC((glob_gvad605 * dac_agc_coderange) / UINT8_MAX + dac_agc_lowcode);
	}
	else
	{
		// при передаче закрыть
		HARDWARE_DAC_AGC(0);
	}
#endif /* (CTLSTYLE_RAVENDSP_V1 || CTLSTYLE_DSPV1A) */

}

void 
prog_fltlreg(void)
{
	const uint_fast8_t pathn = isdspmodetx(glob_dspmodes [0]) ? 1 : NTRX;	// при передаче только тракт с идексом 0
	// Обновление расчитываемых параметров
	const uint_fast8_t spf = ! gwprof;	// индекс профиля для подготовки параметров DSP

	audio_setup_mike(spf);
	uint_fast8_t pathi;
	for (pathi = 0; pathi < pathn; ++ pathi)
		audio_update(spf, pathi);

	gwprof = spf;

	modem_update();
}

#elif WITHEXTERNALDDSP /* имеется управление внешней DSP платой. */


#define DSPREG_SPIMODE	SPIC_MODE3	// DSP module для управления платой "Дятел"

void 
prog_dsplreg(void)
{
	const spitarget_t target = targetdsp1;
	uint_fast8_t buff [DSPCTL_BUFSIZE]; // = { 0 };
	uint_fast8_t i;

	buff [DSPCTL_OFFSET_MODEA] = glob_dspmodes [0];
	buff [DSPCTL_OFFSET_MODEB] = glob_dspmodes [1];
	buff [DSPCTL_OFFSET_AFGAIN_HI] = glob_afgain >> 8;
	buff [DSPCTL_OFFSET_AFGAIN_LO] = glob_afgain;
	buff [DSPCTL_OFFSET_AFMUTE] = glob_afmute;	/* отключить звук в наушниках и динамиках */
	buff [DSPCTL_OFFSET_RFGAIN_LO] = glob_rfgain;
	buff [DSPCTL_OFFSET_AGCOFF] = (glob_agc == BOARD_AGCCODE_OFF);
	buff [DSPCTL_OFFSET_MICLEVEL_HI] = glob_mik1level >> 8;
	buff [DSPCTL_OFFSET_MICLEVEL_LO] = glob_mik1level;
	buff [DSPCTL_OFFSET_AGC_T1] = glob_agc_t1;
	buff [DSPCTL_OFFSET_AGC_T2] = glob_agc_t2;
	buff [DSPCTL_OFFSET_AGC_T4] = glob_agc_t4;
	buff [DSPCTL_OFFSET_AGC_THUNG] = glob_agc_thung;
	buff [DSPCTL_OFFSET_AGCRATE] = glob_agcrate; // may be UINT8_MAX

	buff [DSPCTL_OFFSET_NFMSQLLEVEL] = glob_nfm_sql_lelel;
	buff [DSPCTL_OFFSET_NFMSQLOFF] = glob_nfm_sql_off;
	buff [DSPCTL_OFFSET_CWEDGETIME] = glob_cwedgetime;
	buff [DSPCTL_OFFSET_SIDETONELVL] = glob_sidetonelevel;

	buff [DSPCTL_OFFSET_NOTCH_ON] = glob_notch_on;
	buff [DSPCTL_OFFSET_NOTCH_WIDTH_HI] = glob_notch_width >> 8;
	buff [DSPCTL_OFFSET_NOTCH_WIDTH_LO] = glob_notch_width >> 0;
	buff [DSPCTL_OFFSET_NOTCH_FREQ_HI] = glob_notch_freq >> 8;
	buff [DSPCTL_OFFSET_NOTCH_FREQ_LO] = glob_notch_freq >> 0;
	buff [DSPCTL_OFFSET_LO6_FREQ_HI] = glob_lo6A >> 8;
	buff [DSPCTL_OFFSET_LO6_FREQ_LO] = glob_lo6A >> 0;
	buff [DSPCTL_OFFSET_FULLBW6_HI] = glob_fullbw6 >> 8;
	buff [DSPCTL_OFFSET_FULLBW6_LO] = glob_fullbw6 >> 0;

	buff [DSPCTL_OFFSET_LOWCUTRX_HI] = glob_aflowcutrx >> 8;
	buff [DSPCTL_OFFSET_LOWCUTRX_LO] = glob_aflowcutrx >> 0;
	buff [DSPCTL_OFFSET_HIGHCUTRX_HI] = glob_afhighcutrx >> 8;
	buff [DSPCTL_OFFSET_HIGHCUTRX_LO] = glob_afhighcutrx >> 0;

	buff [DSPCTL_OFFSET_LOWCUTTX_HI] = glob_aflowcuttx >> 8;
	buff [DSPCTL_OFFSET_LOWCUTTX_LO] = glob_aflowcuttx >> 0;
	buff [DSPCTL_OFFSET_HIGHCUTTX_HI] = glob_afhighcuttx >> 8;
	buff [DSPCTL_OFFSET_HIGHCUTTX_LO] = glob_afhighcuttx >> 0;

	buff [DSPCTL_OFFSET_DIGIGAINMAX] = glob_digigainmax;

	//buff [DSPCTL_OFFSET_FLTSOFTER] = glob_fltsofter;
	buff [DSPCTL_OFFSET_AMDEPTH] = glob_amdepth;
	buff [DSPCTL_OFFSET_MIKEAGC] = glob_mikeagc;
	buff [DSPCTL_OFFSET_MIKEAGCSCALE] = glob_mikeagcscale;

	spi_select(target, DSPREG_SPIMODE);
	spi_progval8_p1(target, buff [0]);
	for (i = 1; i < (sizeof buff / sizeof buff [0]); ++ i)
		spi_progval8_p2(target, buff [i]);
	spi_complete(target);
	spi_unselect(target);
}

/* 1/4 FS (12 kHz) или 0 для DSP */
int_fast32_t
dsp_get_ifreq(void)
{
	return DEFAULT_DSP_IF;		/* про частоту ПЧ внешнего DSP знаем только по этому определению. */
}

void 
prog_fltlreg(void)
{
}

#else

void 
prog_dsplreg(void)
{
}

void 
prog_fltlreg(void)
{
}

#endif /* WITHINTEGRATEDDSP */


void 
prog_codec1reg(void)
{
#if defined(CODEC1_TYPE)
	const codec1if_t * const ifc1 = board_getaudiocodecif();

	// also use glob_mik1level
	ifc1->setvolume(glob_afgain, glob_afmute, glob_loudspeaker_off);
	ifc1->setlineinput(glob_lineinput, glob_mikebust20db, glob_mik1level, glob_lineamp);
	ifc1->setprocparams(glob_mikeequal, glob_codec1_gains);	/* параметры обработки звука с микрофона (эхо, эквалайзер, ...) */

#endif /* defined(CODEC1_TYPE) */
}


static uint_fast8_t flag_dsp1reg;	/* признак модификации теневых значений. Требуется вывод в регистры */
static uint_fast8_t flag_flt1reg;	/* признак модификации теневых значений. Требуется вывод в регистры */
static uint_fast8_t flag_codec1reg;	/* признак модификации теневых значений. Требуется вывод в регистры */

/* запрос может устанавливаться из обработчика прерывания в случае WITHSPISLAVE */
void prog_dsplreg_update(void)
{	
	uint_fast8_t f;
#if WITHSPISLAVE
	disableIRQ();
	f = flag_dsp1reg;
	flag_dsp1reg = 0;
	enableIRQ();
#else /* WITHSPISLAVE */
	f = flag_dsp1reg;
	flag_dsp1reg = 0;
#endif /* WITHSPISLAVE */
	if (f != 0)
	{
		prog_dsplreg();
	}
}
/* запрос может устанавливаться из обработчика прерывания в случае WITHSPISLAVE */
void prog_fltlreg_update(void)
{	
	uint_fast8_t f;
#if WITHSPISLAVE
	disableIRQ();
	f = flag_flt1reg;
	flag_flt1reg = 0;
	enableIRQ();
#else /* WITHSPISLAVE */
	f = flag_flt1reg;
	flag_flt1reg = 0;
#endif /* WITHSPISLAVE */
	if (f != 0)
	{
		prog_fltlreg();
	}
}

/* запрос может устанавливаться из обработчика прерывания в случае WITHSPISLAVE */
void prog_codecreg_update(void)		// услолвное обновление регистров аудио кодека
{
	uint_fast8_t f;
#if WITHSPISLAVE
	disableIRQ();
	f = flag_codec1reg;
	flag_codec1reg = 0;
	enableIRQ();
#else /* WITHSPISLAVE */
	f = flag_codec1reg;
	flag_codec1reg = 0;
#endif /* WITHSPISLAVE */
	if (f != 0)
	{
		prog_codec1reg();
	}
}

/* Функция может вызываться из обработчика прерывания в случае WITHSPISLAVE */
/* Установка запроса на обновление сигналов управления */
void
board_dsp1regchanged(void)
{
	flag_dsp1reg = 1; 
}
/* Функция может вызываться из обработчика прерывания в случае WITHSPISLAVE */
/* Установка запроса на обновление сигналов управления */
void
board_flt1regchanged(void)
{
#if WITHEXTERNALDDSP /* имеется управление внешней DSP платой. */
	// в этом случае обращаемся к другой функции
	flag_dsp1reg = 1; 
#else /* WITHEXTERNALDDSP */
	flag_flt1reg = 1; 
#endif /* WITHEXTERNALDDSP */
}
/* Функция может вызываться из обработчика прерывания в случае WITHSPISLAVE */
/* Установка запроса на обновление сигналов управления */
void
board_codec1regchanged(void)
{
	flag_codec1reg = 1; 
}

////////////////////////////////


/* Тракт, к которому относятся все последующие вызовы. При перередаяе используется индекс 0 */
void board_set_trxpath(uint_fast8_t v)
{
	glob_trxpath = v;
}


/*	Мощность, соответствующая full scale от IF ADC */
void
board_set_fsadcpower10(int_fast16_t v)
{
	if (glob_fsadcpower10 != v)
	{
		glob_fsadcpower10 = v;
		board_dsp1regchanged();
	}
}

void
board_set_rfgain(uint_fast16_t v)
{
	if (glob_rfgain != v)
	{
		glob_rfgain = v;
		board_dsp1regchanged();
	}
}

void
board_set_agcrate(uint_fast8_t n)	/* на n децибел изменения входного сигнала 1 дБ выходного. UINT8_MAX - "плоская" АРУ */
{
	if (glob_agcrate [glob_trxpath] != n)
	{
		glob_agcrate [glob_trxpath] = n;
		board_dsp1regchanged();
	}
}

void
board_set_agc_t1(uint_fast8_t n)	/* подстройка параметра АРУ */
{
	if (glob_agc_t1 [glob_trxpath] != n)
	{
		glob_agc_t1 [glob_trxpath] = n;
		board_dsp1regchanged();
	}
}

void
board_set_agc_t2(uint_fast8_t n)	/* подстройка параметра АРУ */
{
	if (glob_agc_t2 [glob_trxpath] != n)
	{
		glob_agc_t2 [glob_trxpath] = n;
		board_dsp1regchanged();
	}
}

void
board_set_agc_t4(uint_fast8_t n)	/* подстройка параметра АРУ */
{
	if (glob_agc_t4 [glob_trxpath] != n)
	{
		glob_agc_t4 [glob_trxpath] = n;
		board_dsp1regchanged();
	}
}

void
board_set_agc_thung(uint_fast8_t n)	/* подстройка параметра АРУ HUNG TIME */
{
	if (glob_agc_thung [glob_trxpath] != n)
	{
		glob_agc_thung [glob_trxpath] = n;
		board_dsp1regchanged();
	}
}

void 
board_set_nfm_sql_lelel(uint_fast8_t n)	/* уровень открывания шумоподавителя NFM */
{
	if (glob_nfm_sql_lelel != n)
	{
		glob_nfm_sql_lelel = n;
		board_dsp1regchanged();
	}
}

void 
board_set_nfm_sql_off(uint_fast8_t v)	/* отключение шумоподавителя NFM */
{
	const uint_fast8_t n = v != 0;
	if (glob_nfm_sql_off != n)
	{
		glob_nfm_sql_off = n;
		board_dsp1regchanged();
	}
}

void 
board_set_swapiq(uint_fast8_t v)	/* поменять местами I и Q сэмплы в потоке RTS96 */
{
	const uint_fast8_t n = v != 0;
	if (glob_swapiq != n)
	{
		glob_swapiq = n;
		board_dsp1regchanged();
	}
}

void 
board_set_swaprts(uint_fast8_t v)	/* если используется конвертор на Rafael Micro R820T - требуется инверсия спектра */
{
	const uint_fast8_t n = v != 0;
	if (glob_swaprts != n)
	{
		glob_swaprts = n;
		board_dsp1regchanged();
	}
}

void 
board_set_notch_freq(uint_fast16_t n)	/* частота NOTCH фильтра */
{
	if (glob_notch_freq != n)
	{
		glob_notch_freq = n;
		board_flt1regchanged();		// параметры этой функции используются в audio_update();
	}
}

void 
board_set_notch_width(uint_fast16_t n)	/* полоса NOTCH фильтра */
{
	if (glob_notch_width != n)
	{
		glob_notch_width = n;
		board_flt1regchanged();		// параметры этой функции используются в audio_update();
	}
}

void 
board_set_notch_on(uint_fast8_t v)	/* включение NOTCH фильтра */
{
	const uint_fast8_t n = v != 0;
	if (glob_notch_on != n)
	{
		glob_notch_on = n;
		board_flt1regchanged();		// параметры этой функции используются в audio_update();
	}
}

void 
board_set_sidetonelevel(uint_fast8_t n)	/* Уровень сигнала самоконтроля в процентах - 0%..100% */
{
	if (glob_sidetonelevel != n)
	{
		glob_sidetonelevel = n;
		board_dsp1regchanged();
	}
}

void 
board_set_subtonelevel(uint_fast8_t n)	/* Уровень сигнала CTCSS в процентах - 0%..100% */
{
	if (glob_subtonelevel != n)
	{
		glob_subtonelevel = n;
		board_dsp1regchanged();
	}
}

void 
board_set_cwedgetime(uint_fast8_t n)	/* Время нарастания/спада огибающей телеграфа при передаче - в 1 мс */
{
	if (glob_cwedgetime != n)
	{
		glob_cwedgetime = n;
		board_dsp1regchanged();
	}
}

void 
board_set_amdepth(uint_fast8_t n)	/* Глубина модуляции в АМ - 0..100% */
{
	if (glob_amdepth != n)
	{
		glob_amdepth = n;
		board_dsp1regchanged();
	}
}

void 
board_set_dacscale(uint_fast8_t n)	/* Использование амплитуды сигнала с ЦАП передатчика - 0..100% */
{
	if (glob_dacscale != n)
	{
		glob_dacscale = n;
		board_dsp1regchanged();
	}
}

void 
board_set_mik1level(uint_fast16_t n)	/* усиление микрофонного усилителя */
{
	if (glob_mik1level != n)
	{
		glob_mik1level = n;
		board_codec1regchanged();
	}
}

// Параметр для регулировки уровня на выходе аудио-ЦАП
void
board_set_afgain(uint_fast16_t v)
{
	if (glob_afgain != v)
	{
		glob_afgain = v;
		board_codec1regchanged();
	}
}

// Альтернативные источники сигнала при передаче
void
board_set_txaudio(uint_fast8_t v)
{
	if (glob_txaudio != v)
	{
		glob_txaudio = v;
		board_dsp1regchanged();
	}
}

// Левый/правый, A - main RX, B - sub RX
void board_set_mainsubrxmode(uint_fast8_t v)
{
	if (glob_mainsubrxmode != v)
	{
		glob_mainsubrxmode = v;
		board_dsp1regchanged();
	}
}

// Параметр для регулировки уровня на входе аудио-ЦАП при работе с LINE IN
void
board_set_lineamp(uint_fast16_t v)
{
	if (glob_lineamp != v)
	{
		glob_lineamp = v;
		board_codec1regchanged();
	}
}


// Включение line input вместо микрофона
void
board_set_lineinput(uint_fast8_t n)
{
	const uint_fast8_t v = n != 0;
	if (glob_lineinput != v)
	{
		glob_lineinput = v;
		board_codec1regchanged();
	}
}

// Включение предусилителя за микрофоном
void
board_set_mikebust20db(uint_fast8_t n)
{
	const uint_fast8_t v = n != 0;
	if (glob_mikebust20db != v)
	{
		glob_mikebust20db = v;
		board_codec1regchanged();
	}
}

/* Включение программной АРУ перед модулятором */
void
board_set_mikeagc(uint_fast8_t n)
{
	const uint_fast8_t v = n != 0;
	if (glob_mikeagc != v)
	{
		glob_mikeagc = v;
		board_dsp1regchanged();
	}
}

/* На какую часть (в процентах) от полной амплитуды настроена АРУ микрофона */
void
board_set_mikeagcscale(uint_fast8_t v)
{
	if (glob_mikeagcscale != v)
	{
		glob_mikeagcscale = v;
		board_dsp1regchanged();
	}
}

/* Максимальное усидение АРУ микрофона */
void
board_set_mikeagcgain(uint_fast8_t v)
{
	if (glob_mikeagcgain != v)
	{
		glob_mikeagcgain = v;
		board_dsp1regchanged();
	}
}

/* Ограничитель в тракте микрофона */
void
board_set_mikehclip(uint_fast8_t v)
{
	if (glob_mikehclip != v)
	{
		glob_mikehclip = v;
		board_dsp1regchanged();
	}
}

/* изменение тембра звука - на Samplerate/2 АЧХ падает на столько децибел  */
void
board_set_afresponcerx(int_fast8_t v)
{
	if (glob_afresponcerx != v)
	{
		glob_afresponcerx = v;
		board_flt1regchanged();
	}
}
/* изменение тембра звука - на Samplerate/2 АЧХ падает на столько децибел  */
void
board_set_afresponcetx(int_fast8_t v)
{
	if (glob_afresponcetx != v)
	{
		glob_afresponcetx = v;
		board_flt1regchanged();
	}
}

#if defined(CODEC1_TYPE) && WITHAFCODEC1HAVEPROC
// включение обработки сигнала с микрофона (эффекты, эквалайзер, ...)
void
board_set_mikeequal(uint_fast8_t n)
{
	const uint_fast8_t v = n != 0;
	if (glob_mikeequal != v)
	{
		glob_mikeequal = v;
		board_codec1regchanged();
	}
}

// Эквалайзер 80Hz 230Hz 650Hz 	1.8kHz 5.3kHz
void board_set_mikeequalparams(const uint_fast8_t * p)
{
	// 
	if (memcmp(glob_codec1_gains, p, sizeof glob_codec1_gains) != 0)
	{
		memcpy(glob_codec1_gains, p, sizeof glob_codec1_gains);
		board_codec1regchanged();
	}

}

#endif /* defined(CODEC1_TYPE) && WITHAFCODEC1HAVEPROC */

/* отключить звук в наушниках и динамиках */
void
board_set_afmute(uint_fast8_t n)
{
	const uint_fast8_t v = n != 0;
	if (glob_afmute != v)
	{
		glob_afmute = v;
		board_codec1regchanged();
	}
}

// Параметр для установки режима работы приёмника A/передатчика A
void
board_set_dspmode(uint_fast8_t v)
{
	if (glob_dspmodes [glob_trxpath] != v)
	{
		glob_dspmodes [glob_trxpath] = v;
		board_flt1regchanged();		// параметры этой функции используются в audio_update();
	}
}

void board_set_lo6(int_fast32_t f)
{
	if (glob_lo6 [glob_trxpath] != f)
	{
		glob_lo6 [glob_trxpath] = f;
		board_flt1regchanged();
	}
}

/* Установка частоты среза фильтров ПЧ в алгоритме Уивера - параметр полная полоса пропускания */
void board_set_fullbw6(int_fast16_t n)
{
	if (glob_fullbw6 [glob_trxpath] != n)
	{
		glob_fullbw6 [glob_trxpath] = n;
		board_flt1regchanged();	// параметры этой функции используются в audio_update();
	}
}

#if 0
/* Код управления сглаживанием скатов фильтра основной селекции на приёме */
/* WITHFILTSOFTMIN..WITHFILTSOFTMAX */
void board_set_fltsofter(uint_fast8_t n)
{
	if (glob_fltsofter [glob_trxpath] != n)
	{
		glob_fltsofter [glob_trxpath] = n;
		board_flt1regchanged();	// параметры этой функции используются в audio_update();
	}
}
#endif

void 
board_set_aflowcutrx(int_fast16_t n)	/* Нижняя частота среза фильтра НЧ по приему */
{
	if (glob_aflowcutrx [glob_trxpath] != n)
	{
		glob_aflowcutrx [glob_trxpath] = n;
		board_flt1regchanged();	// параметры этой функции используются в audio_update();
	}
}

void 
board_set_afhighcutrx(int_fast16_t n)	/* Верхняя частота среза фильтра НЧ по приему */
{
	if (glob_afhighcutrx [glob_trxpath] != n)
	{
		glob_afhighcutrx [glob_trxpath] = n;
		board_flt1regchanged();	// параметры этой функции используются в audio_update();
	}
}

void 
board_set_aflowcuttx(int_fast16_t n)	/* Нижняя частота среза фильтра НЧ по передаче */
{
	if (glob_aflowcuttx != n)
	{
		glob_aflowcuttx = n;
		board_flt1regchanged();	// параметры этой функции используются в audio_update();
	}
}

void 
board_set_afhighcuttx(int_fast16_t n)	/* Верхняя частота среза фильтра НЧ по передаче */
{
	if (glob_afhighcuttx != n)
	{
		glob_afhighcuttx = n;
		board_flt1regchanged();	// параметры этой функции используются в audio_update();
	}
}

/* диапазон ручной регулировки цифрового усиления - максимальное значение */
void
board_set_digigainmax(uint_fast8_t v)
{
	if (glob_digigainmax != v)
	{
		glob_digigainmax = v;
		board_dsp1regchanged();
	}
}

/* напряжение на AD605 (управление усилением тракта ПЧ */
void
board_set_gvad605(uint_fast8_t v)
{
	if (glob_gvad605 != v)
	{
		glob_gvad605 = v;
		board_dsp1regchanged();
	}
}

// скорость передачи с точностью 1/100 бод
void
board_set_modem_speed100(uint_fast32_t v)
{
	if (glob_modem_speed100 != v)
	{
		glob_modem_speed100 = v;
		board_flt1regchanged();
	}
}

// применяемая модуляция
void
board_set_modem_mode(uint_fast8_t v)
{
	if (glob_modem_mode != v)
	{
		glob_modem_mode = v;
		board_flt1regchanged();
	}
}

#if WITHSPISLAVE

// вызывается из прерывания для обработки принятого блока данных
void hardware_spi_slave_callback(uint8_t * buff, uint_fast8_t len)
{
	if (len == DSPCTL_BUFSIZE)
	{
		board_set_dspmodeA(buff [DSPCTL_OFFSET_MODEA]);
		board_set_dspmodeB(buff [DSPCTL_OFFSET_MODEB]);
		board_set_agc(buff [DSPCTL_OFFSET_AGCOFF] ? BOARD_AGCCODE_OFF : BOARD_AGCCODE_ON);
#if ! WITHPOTGAIN
		board_set_rfgain(buff [DSPCTL_OFFSET_RFGAIN_HI * 256] + buff [DSPCTL_OFFSET_RFGAIN_LO]);
		board_set_afgain(buff [DSPCTL_OFFSET_AFGAIN_HI * 256] + buff [DSPCTL_OFFSET_AFGAIN_LO]);
#endif /* ! WITHPOTGAIN */
		board_set_afmute(buff [DSPCTL_OFFSET_AFMUTE]);
		board_set_agc_t1(buff [DSPCTL_OFFSET_AGC_T1]);
		board_set_agc_t2(buff [DSPCTL_OFFSET_AGC_T2]);
		board_set_agc_t4(buff [DSPCTL_OFFSET_AGC_T4]);
		board_set_agc_thung(buff [DSPCTL_OFFSET_AGC_THUNG]);
		board_set_agcrate(buff [DSPCTL_OFFSET_AGCRATE]);	// на n децибел изменения входного сигнала 1 дБ выходного. UINT8_MAX - "плоская" АРУ
	
		board_set_mik1level(buff [DSPCTL_OFFSET_MICLEVEL_HI] * 256 + buff [DSPCTL_OFFSET_MICLEVEL_LO]);
		board_set_nfm_sql_lelel(buff [DSPCTL_OFFSET_NFMSQLLEVEL]);
		board_set_nfm_sql_off(buff [DSPCTL_OFFSET_NFMSQLOFF]);

		board_set_afhighcutrx(buff [DSPCTL_OFFSET_HIGHCUTRX_HI] * 256 + buff [DSPCTL_OFFSET_HIGHCUTRX_LO]);
		board_set_aflowcutrx(buff [DSPCTL_OFFSET_LOWCUTRX_HI] * 256 + buff [DSPCTL_OFFSET_LOWCUTRX_LO]);

		board_set_afhighcuttx(buff [DSPCTL_OFFSET_HIGHCUTTX_HI] * 256 + buff [DSPCTL_OFFSET_HIGHCUTTX_LO]);
		board_set_aflowcuttx(buff [DSPCTL_OFFSET_LOWCUTTX_HI] * 256 + buff [DSPCTL_OFFSET_LOWCUTTX_LO]);

		board_set_cwedgetime(buff [DSPCTL_OFFSET_CWEDGETIME]);
		board_set_sidetonelevel(buff [DSPCTL_OFFSET_SIDETONELVL]);

		board_set_notch_on(buff [DSPCTL_OFFSET_NOTCH_ON]);
		board_set_notch_width(buff [DSPCTL_OFFSET_NOTCH_WIDTH_HI] * 256 + buff [DSPCTL_OFFSET_NOTCH_WIDTH_LO]);
		board_set_notch_freq(buff [DSPCTL_OFFSET_NOTCH_FREQ_HI] * 256 + buff [DSPCTL_OFFSET_NOTCH_FREQ_LO]);
		board_set_lo6(buff [DSPCTL_OFFSET_LO6_FREQ_HI] * 256 + buff [DSPCTL_OFFSET_LO6_FREQ_LO]);
		board_set_fullbw6(buff [DSPCTL_OFFSET_FULLBW6_HI] * 256 + buff [DSPCTL_OFFSET_FULLBW6_LO]);
		board_set_digigainmax(buff [DSPCTL_OFFSET_DIGIGAINMAX]);
		//board_set_fltsofter(buff [DSPCTL_OFFSET_FLTSOFTER]);
		board_set_amdepth(buff [DSPCTL_OFFSET_AMDEPTH]);
		board_set_mikeagc(buff [DSPCTL_OFFSET_MIKEAGC]);
		board_set_mikeagcscale(buff [DSPCTL_OFFSET_MIKEAGCSCALE]);
	}
}

#endif /* WITHSPISLAVE */
