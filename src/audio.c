/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints
#include "board.h"
#include "audio.h"
#include "buffers.h"
#include "audio_reverb.h"
#include "audio_compressor.h"

#include "codecs.h"

#include <limits.h>
#include <string.h>
#include <math.h>

#include "dspdefines.h"

#if WITHFT8
	#include "ft8.h"
#endif /* WITHFT8 */

//#define WITHDOUBLEFIRCOEFS 1

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

	#if __ARM_ARCH_7A__
		#warning Avaliable __ARM_ARCH_7A__
	#endif /* __ARM_ARCH_7A__ */

	#if __ARM_ARCH_8A__
		#warning Avaliable __ARM_ARCH_8A__
	#endif /* __ARM_ARCH_8A__ */
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
static uint_fast16_t 	glob_ifgain = BOARD_IFGAIN_MIN;
static uint_fast8_t 	glob_dspmodes [2] = { DSPCTL_MODE_IDLE, DSPCTL_MODE_IDLE, };

static uint_fast8_t		glob_agcrate [2] = { 20, 20 }; //10	// 10 дБ изменение входного на 1 дБ выходного
static uint_fast8_t 	glob_agc_scale [2] = { 100, 100 }; // scale в процентах - Для эксперементов по улучшению приема АМ
static uint_fast8_t 	glob_agc_t0 [2] = { 0, 0 }; // chargespeedfast в милисекундах
static uint_fast8_t 	glob_agc_t1 [2] = { 95, 95 }; // chargespeedslow в милисекундах
static uint_fast8_t 	glob_agc_t2 [2] = { 2, 2 };	// dischargespeedslow в сотнях милисекунд (0.1 секунды)
static uint_fast8_t 	glob_agc_thung [2] = { 3, 3 };	// 0.3 S
static uint_fast8_t 	glob_agc_t4 [2] = { 120, 120 }; // dischargespeedfast в милисекундах

static int_fast16_t 	glob_aflowcutrx [2] = { 300, 300 } ;		// Частота низкочастотного среза полосы пропускания (в 10 Гц дискретах)
static int_fast16_t 	glob_afhighcutrx [2] = { 3400, 3400 };	// Частота высокочастотного среза полосы пропускания (в 100 Гц дискретах)

static int_fast16_t 	glob_aflowcuttx = 300 ;		// Частота низкочастотного среза полосы пропускания (в 10 Гц дискретах)
static int_fast16_t 	glob_afhighcuttx = 3400;	// Частота высокочастотного среза полосы пропускания (в 100 Гц дискретах)

static int_fast16_t		glob_fullbw6 [2] = { 1000, 1000 };		/* Частота среза фильтров ПЧ в алгоритме Уивера */
static int_fast32_t		glob_lo6 [2] = { 0, 0 };
//static uint_fast8_t		glob_fltsofter [2] = { WITHFILTSOFTMIN, WITHFILTSOFTMIN }; /* WITHFILTSOFTMIN..WITHFILTSOFTMAX Код управления сглаживанием скатов фильтра основной селекции на приёме */
static int_fast16_t 	glob_gainnfmrx [2] = { 100, 100 };
static uint_fast8_t 	glob_squelch;

// codec-related parameters
static uint_fast16_t 	glob_afgain;
static uint_fast8_t 	glob_afmute;	/* отключить звук в наушниках и динамиках */
static uint_fast8_t		glob_lineinput;	/* используется line input вместо микрофона */
static uint_fast8_t 	glob_mikeboost20db;	/* Включение усилителя 20 дБ за микрофоном */
static uint_fast8_t		glob_mikeagc = 1;	/* Включение программной АРУ перед модулятором */
static uint_fast8_t		glob_mikeagcgain = 40;	/* предел усиления в АРУ */
static uint_fast8_t		glob_mikehclip;			/* параметр ограничителя микрофона	*/
#if defined(CODEC1_TYPE) && defined (HARDWARE_CODEC1_NPROCPARAMS)
static uint_fast8_t 	glob_mikeequal;	// Включение обработки сигнала с микрофона (эффекты, эквалайзер, ...)
static uint_fast8_t		glob_codec1_gains [HARDWARE_CODEC1_NPROCPARAMS]; // = { -2, -1, -3, +6, +9 };	// параметры эквалайзера
#endif /* defined(CODEC1_TYPE) && defined (HARDWARE_CODEC1_NPROCPARAMS) */

#if WITHAFEQUALIZER
static uint_fast8_t 	glob_equalizer_rx;
static uint_fast8_t 	glob_equalizer_tx;
static uint_fast8_t		glob_equalizer_rx_gains [AF_EQUALIZER_BANDS];
static uint_fast8_t		glob_equalizer_tx_gains [AF_EQUALIZER_BANDS];
#endif /* WITHAFEQUALIZER */

static uint_fast8_t		glob_compattack;
static uint_fast8_t		glob_comprelease;
static uint_fast8_t		glob_comphold;
static uint_fast8_t		glob_compgain;
static uint_fast8_t		glob_compthreshold;

#if WITHREVERB
	static uint_fast8_t glob_reverb;		/* ревербератор */
	static uint_fast8_t glob_reverbdelay = 50;		/* ревербератор - задержка (%) */
	static uint_fast8_t glob_reverbloss = 30;		/* ревербератор - ослабление db на возврате */
#endif /* WITHREVERB */

static uint_fast16_t 	glob_lineamp = WITHLINEINGAINMAX;
static uint_fast16_t	glob_mik1level = WITHMIKEINGAINMAX;
static uint_fast8_t 	glob_txaudio = BOARD_TXAUDIO_MIKE;	// при SSB/AM/FM передача с тестовых источников

#if WITHNOTCHONOFF || WITHNOTCHFREQ
static uint_fast16_t 	glob_notch_freq = 1000;	/* частота NOTCH фильтра */
static uint_fast16_t	glob_notch_width = 500;	/* полоса NOTCH фильтра */
static uint_fast8_t 	glob_notch_mode = BOARD_NOTCH_OFF;		/* включение NOTCH фильтра */
#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */

static uint_fast8_t 	glob_cwedgetime = 4;		/* CW Rise Time (in 1 ms discrete) */
static uint_fast8_t 	glob_sidetonelevel = 10;	/* Уровень сигнала самоконтроля в процентах - 0%..100% */
static uint_fast8_t 	glob_moniflag = 1;		/* Уровень сигнала самопрослушивания в процентах - 0%..100% */
static uint_fast8_t		glob_cwssbtx = 1;			/* разрешение передачи телеграфа как тона в режиме SSB */
static uint_fast8_t 	glob_subtonelevel = 0;	/* Уровень сигнала CTCSS в процентах - 0%..100% */
static uint_fast8_t 	glob_amdepth = 30;		/* Глубина модуляции в АМ - 0..100% */
#if WITHIF4DSP
static uint_fast16_t	glob_dacscale = BOARDDACSCALEMAX;	/* На какую часть (в процентах в квадрате) от полной амплитуды использцется ЦАП передатчика */
#endif /* WITHIF4DSP */
static uint_fast16_t	glob_digiscale = 100;	/* Увеличение усиления при передаче в цифровых режимах 100..300% */
static uint_fast16_t	glob_cwscale = 100;	/* Увеличение усиления при передаче в цифровых режимах 100..300% */
static uint_fast16_t	glob_designscale = 100;	/* используется при калибровке параметров интерполятора */
static uint_fast8_t 	glob_digigainmax = 96;
static uint_fast8_t		glob_gvad605 = UINT8_MAX;	/* напряжение на AD605 (управление усилением тракта ПЧ */

static int_fast16_t		glob_fsadcpower10 = 0;	// мощность, соответствующая full scale от IF ADC с точностью 0.1 дБмВт

static uint_fast8_t		glob_modem_mode;		// применяемая модуляция
static uint_fast32_t	glob_modem_speed100 = 3125;	// скорость передачи с точностью 1/100 бод

static int_fast8_t		glob_afresponcerx;	// изменение тембра звука в канале приёмника - на Samplerate/2 АЧХ становится на столько децибел
static int_fast8_t		glob_afresponcetx;	// изменение тембра звука в канале передатчика - на Samplerate/2 АЧХ становится на столько децибел

static uint_fast8_t		glob_mainsubrxmode = BOARD_RXMAINSUB_A_A;	// Левый/правый, A - main RX, B - sub RX

static uint_fast8_t		glob_nfmdeviation100 = 75;	// 7.5 kHz максимальная девиация в NFM

static uint_fast8_t 	glob_dspagc;
static uint_fast8_t		glob_dsploudspeaker_off;

static volatile uint_fast8_t uacoutplayer;	/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
static volatile uint_fast8_t datavox;	/* автоматическое изменение источника при появлении звука со стороны компьютера */


#if WITHINTEGRATEDDSP

static uint_fast8_t istxreplaced(void)
{
#if WITHUSBHW && WITHUSBUACOUT
	return (datavox != 0 && buffers_get_uacoutalt() != 0);
#else /* WITHUSBHW && WITHUSBUACOUT */
	return 0;
#endif /* WITHUSBHW && WITHUSBUACOUT */
}

#define NPROF 2	/* количество профилей параметров DSP фильтров. */

// Определения для работ по оптимизации быстродействия
#if WITHDEBUG && 0

	// stm32f746, no dualwatch:
	//	dtcount=0, dtmax=0, dtlast=0, dtcount2=41807716, dtmax2=1244, dtlast2=739, dtcount3=41806755, dtmax3=1446, dtlast3=916
	// R7S721xxx, Neon, dualwatch:
	//	dtcount=0, dtmax=0, dtlast=0, dtcount2=15890107, dtmax2=1119, dtlast2=590, dtcount3=31778668, dtmax3=1169, dtlast3=723
	// R7S721xxx, no Neon, dualwatch:
	// dtcount=0, dtmax=0, dtlast=0, dtcount2=184728, dtmax2=1461, dtlast2=911, dtcount3=367872, dtmax3=1169, dtlast3=713

	static volatile uint_fast32_t dtmax = 0, dtlast = 0, dtcount = 0;
	static volatile uint_fast32_t dtmax2 = 0, dtlast2 = 0, dtcount2 = 0;
	static volatile uint_fast32_t dtmax3 = 0, dtlast3 = 0, dtcount3 = 0;
	static uint_fast32_t perft = 0;
	static uint_fast32_t perft2 = 0;
	static uint_fast32_t perft3 = 0;

	static void debug_cleardtmax(void)
	{
		dtmax = 0;
		dtmax2 = 0;
		dtmax3 = 0;
	}

	#define BEGIN_STAMP() do { \
			perft = cpu_getdebugticks(); \
		} while (0)

	#define END_STAMP() do { \
			const uint_fast32_t t2 = cpu_getdebugticks(); \
			if (perft < t2) \
			{ \
				const uint_fast32_t vdt = t2 - perft; \
				dtlast = vdt; /* текущее значение длительности */ \
				if (vdt > dtmax) \
					dtmax = vdt; /* максимальное значение длительности */ \
				++ dtcount; \
			} \
		} while (0)

	#define BEGIN_STAMP2() do { \
			perft2 = cpu_getdebugticks(); \
		} while (0)

	#define END_STAMP2() do { \
			const uint_fast32_t t2 = cpu_getdebugticks(); \
			if (perft2 < t2) \
			{ \
				const uint_fast32_t vdt = t2 - perft2; \
				dtlast2 = vdt; /* текущее значение длительности */ \
				if (vdt > dtmax2) \
					dtmax2 = vdt; /* максимальное значение длительности */ \
				++ dtcount2; \
			} \
		} while (0)

	#define BEGIN_STAMP3() do { \
			perft3 = cpu_getdebugticks(); \
		} while (0)

	#define END_STAMP3() do { \
			const uint_fast32_t t2 = cpu_getdebugticks(); \
			if (perft3 < t2) \
			{ \
				const uint_fast32_t vdt = t2 - perft3; \
				dtlast3 = vdt; /* текущее значение длительности */ \
				if (vdt > dtmax3) \
					dtmax3 = vdt; /* максимальное значение длительности */ \
				++ dtcount3; \
			} \
		} while (0)


	//static uint32_t dd [4];
	/* DSP speed test */
	void dsp_speed_diagnostics(void)
	{
		//PRINTF(PSTR("data=%08lX,%08lX,%08lX,%08lX\n"), dd [0], dd [1], dd [2], dd [3]);
		PRINTF(PSTR("dtcount=%" PRIuFAST32 ", dtmax=%" PRIuFAST32 ", dtlast=%" PRIuFAST32 ", "), dtcount, dtmax, dtlast);
		PRINTF(PSTR("dtcount2=%" PRIuFAST32 ", dtmax2=%" PRIuFAST32 ", dtlast2=%" PRIuFAST32 ", "), dtcount2, dtmax2, dtlast2);
		PRINTF(PSTR("dtcount3=%" PRIuFAST32 ", dtmax3=%" PRIuFAST32 ", dtlast3=%" PRIuFAST32 "\n"), dtcount3, dtmax3, dtlast3);
	}

#else /* WITHDEBUG */

	#define BEGIN_STAMP() do { \
		} while (0)

	#define END_STAMP() do { \
		} while (0)

	#define BEGIN_STAMP2() do { \
		} while (0)

	#define END_STAMP2() do { \
		} while (0)

	#define BEGIN_STAMP3() do { \
		} while (0)

	#define END_STAMP3() do { \
		} while (0)

	static void debug_cleardtmax(void)
	{
	}

	void dsp_speed_diagnostics(void)
	{
	}

#endif /* WITHDEBUG */

void beginstamp(void)
{
	BEGIN_STAMP();
}

void endstamp(void)
{
	END_STAMP();
}

void beginstamp2(void)
{
	BEGIN_STAMP2();
}

void endstamp2(void)
{
	END_STAMP2();
}

void beginstamp3(void)
{
	BEGIN_STAMP3();
}

void endstamp3(void)
{
	END_STAMP3();
}

#if WITHDSPEXTFIR || WITHDSPEXTDDC
	// Фильтр для квадратурных каналов приёмника и передатчика в FPGA (целочисленный).
	// Параметры для передачи в FPGA
	#if WITHDOUBLEFIRCOEFS && (__ARM_FP & 0x08)
		static double FIRCwndL_trxi_IQ [NtapCoeffs(Ntap_trxi_IQ)];			// подготовленные значения функции окна
	#else
		static FLOAT_t FIRCwnd_trxi_IQ [NtapCoeffs(Ntap_trxi_IQ)];			// подготовленные значения функции окна
	#endif

#endif /* WITHDSPEXTFIR || WITHDSPEXTDDC */

#if WITHDSPLOCALFIR	

	// Фильтр для квадратурных каналов приёмника (floating point).
	static RAMDTCM FLOAT_t FIRCoef_rx_SSB_IQ [NPROF] [NtapCoeffs(Ntap_rx_SSB_IQ)];
	static RAMDTCM FLOAT_t FIRCwnd_rx_SSB_IQ [NtapCoeffs(Ntap_rx_SSB_IQ)];			// подготовленные значения функции окна

	// Фильтр для квадратурных каналов передатчика (floating point)
	static RAMDTCM FLOAT_t FIRCoef_tx_SSB_IQ [NPROF] [NtapCoeffs(Ntap_tx_SSB_IQ)];
	static RAMDTCM FLOAT_t FIRCwnd_tx_SSB_IQ [NtapCoeffs(Ntap_tx_SSB_IQ)];			// подготовленные значения функции окна

#endif /* WITHDSPLOCALFIR */

#if WITHDSPLOCALTXFIR
	static RAMDTCM FLOAT_t FIRCoef_tx_SSB_IQ [NPROF] [NtapCoeffs(Ntap_tx_SSB_IQ)];
	static RAMDTCM FLOAT_t FIRCwnd_tx_SSB_IQ [NtapCoeffs(Ntap_tx_SSB_IQ)];			// подготовленные значения функции окна
#endif /* WITHDSPLOCALTXFIR */

// Фильтр для передатчика (floating point)
// Обрабатывается как несимметричный
static RAMBIGDTCM FLOAT_t FIRCoef_tx_MIKE [NPROF] [NtapCoeffs(Ntap_tx_MIKE)];
static FLOAT_t FIRCwnd_tx_MIKE [NtapCoeffs(Ntap_tx_MIKE)];			// подготовленные значения функции окна

#if WITHSKIPUSERMODE
static RAMBIGDTCM FLOAT_t FIRCoef_rx_AUDIO [NPROF] [2 /* эта размерность номер тракта */] [Ntap_rx_AUDIO];	/* полный размет без учета симметрии */
#endif /* WITHSKIPUSERMODE */
static FLOAT_t FIRCwnd_rx_AUDIO [NtapCoeffs(Ntap_rx_AUDIO)];			/* подготовленные значения функции окна - с учетом симметрии (половина) */

//static void * fft_lookup;


// Используется при формировании корректированной АЧХ звука. Должно быть размером достаточным, чтобы влезли используемые фильтры
#define FFTSizeFilters 1024

struct ComplexHFTRX
{
	FLOAT_t real;
	FLOAT_t imag;
};

/* этот массив используется при перерасчете АЧЪ фильтров НЧ - не real time задача */
static RAM_D2 struct ComplexHFTRX Sig [FFTSizeFilters];

#define fftixreal(i) ((i * 2) + 0)
#define fftiximag(i) ((i * 2) + 1)

static FLOAT_t txlevelfenceAM = (FLOAT_t) 1 / 2;

static FLOAT_t txlevelfenceSSB = (FLOAT_t) 1 / 2;
static FLOAT_t txlevelfenceDIGI = (FLOAT_t) 1 / 2;

static FLOAT_t txlevelfenceNFM = (FLOAT_t) 1 / 2;
static FLOAT_t txlevelfenceBPSK = (FLOAT_t) 1 / 2;
static FLOAT_t txlevelfenceCW = (FLOAT_t) 1 / 2;

static uint_fast8_t gwprof = 0;	// work profile - индекс конфигурационной информации, испольуемый для работы */

static uint_fast8_t globDSPMode [NPROF] [2] = { { DSPCTL_MODE_IDLE, DSPCTL_MODE_IDLE }, { DSPCTL_MODE_IDLE, DSPCTL_MODE_IDLE } };

/* Параметры АМ модулятора */
static volatile FLOAT_t amshapesignalHALF;
static volatile FLOAT_t amcarrierHALF;
static volatile FLOAT_t scaleDAC = 1;

static FLOAT_t shapeSidetoneStep(void);		// 0..1
static FLOAT_t shapeTXEnvelopStep(void);	// 0..1
static FLOAT_t shapeCWSSBEnvelopStep(void);	// 0..1
static uint_fast8_t getTxShapeNotComplete(void);

static FLOAT32P_t getsampmlemike2(void);
static FLOAT32P_t getsampmleusb2(void);

static uint_fast8_t getRxGate(void);	/* разрешение работы тракта в режиме приёма */

typedef uint32_t ncoftw_t;
typedef int32_t ncoftwi_t;
#define NCOFTWBITS 32	// количество битов в ncoftw_t
#define FTWROUND(ftw) ((uint32_t) (ftw))
#define FTWAF001(freq) (((int_fast64_t) (freq) << NCOFTWBITS) / ARMI2SRATE100)
#define FTWAF(freq) (((int_fast64_t) (freq) << NCOFTWBITS) / (int_fast64_t) ARMI2SRATE)
static FLOAT_t omega2ftw_k1; // = POWF(2, NCOFTWBITS);
#define OMEGA2FTWI(angle) ((ncoftwi_t) ((FLOAT_t) (angle) * omega2ftw_k1 / (FLOAT_t) M_TWOPI))	// angle in radians -pi..+pi to signed version of ftw_t

// Convert ncoftw_t to q31 argument for arm_sin_cos_q31
// The Q31 input value is in the range [-1 0.999999] and is mapped to a degree value in the range [-180 179].
#define FTW2_SINCOS_Q31(angle) ((ncoftwi_t) (angle))
// Convert ncoftw_t to q31 argument for arm_sin_q31
// The Q31 input value is in the range [0 +0.9999] and is mapped to a radian value in the range [0 2*PI).
#define FTW2_COS_Q31(angle) ((q31_t) ((((ncoftw_t) (angle)) + 0x80000000) / 2))
#define FAST_Q31_2_FLOAT(val) ((q31_t) (val) / (FLOAT_t) 2147483648)

#if 0
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
#endif

static FLOAT_t getsinf(ncoftw_t angle)
{
	FLOAT_t v;
	const q31_t sinv = arm_sin_q31(FTW2_COS_Q31(angle));
	//v = FAST_Q31_2_FLOAT(sinv);	// todo: use arm_q31_to_float
	//arm_q31_to_float(& sinv, & v, 1);
	v = FAST_Q31_2_FLOAT(sinv);
	return v;
}

static FLOAT_t getcosf(ncoftw_t angle)
{
	FLOAT_t v;
	const q31_t cosv = arm_cos_q31(FTW2_COS_Q31(angle));
	//v = FAST_Q31_2_FLOAT(cosv);	// todo: use arm_q31_to_float
	//arm_q31_to_float(& cosv, & v, 1);
	v = FAST_Q31_2_FLOAT(cosv);
	return v;
}

static FLOAT32P_t getsincosf(ncoftw_t angle)
{
	FLOAT32P_t v;
	//q31_t sincosv [2];
#if 1
	FLOAT_t sinv = arm_sin_q31(FTW2_COS_Q31(angle));
	FLOAT_t cosv = arm_cos_q31(FTW2_COS_Q31(angle));
#else
	arm_sin_cos_q31(FTW2_SINCOS_Q31(angle), & sincosv [0], & sincosv [1]);
	// at index 0 all fine
	// at index 1 with sidetones
#endif
	//arm_q31_to_float(sincosv, v.ivqv, 2);
	v.IV = FAST_Q31_2_FLOAT(sinv);
	v.QV = FAST_Q31_2_FLOAT(cosv);
	return v;
}

//////////////////////////////////////////


static ncoftw_t anglestep_modulation = FTWAF001(10);	/* 0.1 s period */
static ncoftw_t angle_modulation;

static ncoftw_t anglestep_lout = FTWAF(700), anglestep_rout = FTWAF(500);
static ncoftw_t angle_lout, angle_rout;

static ncoftw_t anglestep_lout2 = FTWAF(5600), anglestep_rout2 = FTWAF(6300);
static ncoftw_t angle_lout2, angle_rout2;

// test IQ frequency
static ncoftw_t anglestep_monofreq = FTWAF(0);
static ncoftw_t angle_monofreq;

// test IQ frequency
static ncoftw_t anglestep_monofreq2 = FTWAF(5600);
static ncoftw_t angle_monofreq2;

// test IQ frequency
static RAMFUNC FLOAT32P_t get_float_monofreq(void)
{
	const FLOAT32P_t v = getsincosf(angle_monofreq);
	angle_monofreq = FTWROUND(angle_monofreq + anglestep_monofreq);
	return v;
}

// test IQ frequency
static FLOAT32P_t get_float_monofreq2(void)
{
	const FLOAT32P_t v = getsincosf(angle_monofreq2);
	angle_monofreq2 = FTWROUND(angle_monofreq2 + anglestep_monofreq2);
	return v;
}

FLOAT_t get_rout(void)
{
    // Формирование значения для ROUT
	const FLOAT_t v = getcosf(angle_rout);
	angle_rout = FTWROUND(angle_rout + anglestep_rout);
	return v;
}

FLOAT_t get_lout(void)
{
	// Формирование значения для LOUT
	const FLOAT_t v = getcosf(angle_lout);
	angle_lout = FTWROUND(angle_lout + anglestep_lout);
	return v;
}

FLOAT_t get_modulation(void)
{
	// Формирование значения для LOUT
	const FLOAT_t v = getcosf(angle_modulation);
	angle_modulation = FTWROUND(angle_modulation + anglestep_modulation);
	return v;
}

//////////////////////////////////////////
/// получение тона для самоконтроля телеграфа (или озвучки клавиш)
static ncoftw_t anglestep_sidetone;
static ncoftw_t angle_sidetone;

static FLOAT_t get_float_sidetone(void)
{
	const FLOAT_t v = getcosf(angle_sidetone);
	angle_sidetone = FTWROUND(angle_sidetone + anglestep_sidetone);
	return v;
}


//////////////////////////////////////////
/// получение тона для замещения передачи SSB телеграфом
///
static ncoftw_t anglestep_sidetonetxssb;
static ncoftw_t angle_sidetonetxssb;

static FLOAT_t get_float_sidetonetxssb(void)
{
	const FLOAT_t v = getcosf(angle_sidetonetxssb);
	angle_sidetonetxssb = FTWROUND(angle_sidetonetxssb + anglestep_sidetonetxssb);
	return v;
}

//////////////////////////////////////////
/// получение тона для формирования ctcss
static ncoftw_t anglestep_subtone;
static ncoftw_t angle_subtone;

static FLOAT_t get_float_subtone(void)
{
	const FLOAT_t v = getcosf(angle_subtone);
	angle_subtone = FTWROUND(angle_subtone + anglestep_subtone);
	return v;
}

//////////////////////////////////////////
static ncoftw_t anglestep_toneout = FTWAF(700);
static ncoftw_t angle_toneout;

static FLOAT_t get_singletonefloat(void)
{
	// Формирование значения для LOUT
	const FLOAT_t v = getcosf(angle_toneout);
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
static RAMDTCM ncoftw_t anglestep_af1 = FTWAF(700);
static RAMDTCM ncoftw_t anglestep_af2 = FTWAF(1900);

static RAMDTCM ncoftw_t angle_af1;
static RAMDTCM ncoftw_t angle_af2;

// двухтональный генератор для настройки
static RAMFUNC FLOAT_t get_dualtonefloat(void)
{
	// Формирование значения выборки
	const FLOAT_t v1 = getcosf(angle_af1);
	const FLOAT_t v2 = getcosf(angle_af2);
	angle_af1 = FTWROUND(angle_af1 + anglestep_af1);
	angle_af2 = FTWROUND(angle_af2 + anglestep_af2);
	return (v1 + v2) / 2;
}

static RAMDTCM unsigned delaysetlo6 [NTRX];	// задержка переключения частоты lo6 на время прохода сигнала через FPGA FIR
static RAMDTCM ncoftw_t anglestep_aflo [NTRX];
static RAMDTCM ncoftw_t anglestep_aflo_shadow [NTRX];
static RAMDTCM ncoftw_t angle_aflo [NTRX];
static RAMDTCM ncoftw_t gnfmdeviationftw = FTWAF(7500);	// 2.5 kHz (-2.5..+2.5) deviation

// установить частоту
static void nco_setlo_ftw(ncoftw_t ftw, uint_fast8_t pathi)
{
#if WITHDSPEXTFIR || WITHDSPEXTDDC
	delaysetlo6 [pathi] = Ntap_trxi_IQ / 2;
	anglestep_aflo_shadow [pathi] = ftw;
#elif WITHDSPLOCALFIR
	delaysetlo6 [pathi] = Ntap_rx_SSB_IQ / 2;
	anglestep_aflo_shadow [pathi] = ftw;
#else
	anglestep_aflo [pathi] = ftw;
	if (ftw == 0)
	{
		/* Для обеспечения 0.7 от максимальной амплитуды после суммирования квадратурных составляющих в FPA */
		angle_aflo [pathi] = 0;	// 0 Pi
	}
#endif
}

/* задержка установки нового значение частоты генератора - возврат 1 если закончилась отработка времени */
static RAMFUNC uint_fast8_t nco_setlo6_delay(uint_fast8_t pathi, uint_fast8_t tx)
{
#if WITHDSPEXTFIR || WITHDSPEXTDDC || WITHDSPLOCALFIR
	if (tx != 0)
	{
		if (delaysetlo6 [pathi] != 0)
		{
			delaysetlo6 [pathi] = 0;	// предотвращаем повторное срабатывание
			if ((anglestep_aflo [pathi] = anglestep_aflo_shadow [pathi]) == 0)
			{
				/* Для обеспечения 0.7 от максимальной амплитуды после суммирования квадратурных составляющих в FPA */
				angle_aflo [pathi] = 0;	// 0 Pi
			}
		}
	}
	else
	{
		if (delaysetlo6 [pathi] != 0 && -- delaysetlo6 [pathi] == 0)
		{
			if ((anglestep_aflo [pathi] = anglestep_aflo_shadow [pathi]) == 0)
			{
				/* Для обеспечения 0.7 от максимальной амплитуды после суммирования квадратурных составляющих в FPA */
				angle_aflo [pathi] = 0;	// 0 Pi
			}
		}
	}
	return delaysetlo6 [pathi] == 0;
#else
	return 1;
#endif
}

// Получение квадратурных значений для данной частоты со смещением (в герцах)
// Returned is a full scale value
static RAMFUNC FLOAT32P_t get_float_aflo_delta(long int deltaftw, uint_fast8_t pathi)
{
	const ncoftw_t angle = angle_aflo [pathi];
	const FLOAT32P_t v = getsincosf(angle);
	angle_aflo [pathi] = FTWROUND(angle + anglestep_aflo [pathi] + deltaftw);
	return v;
}

//////////////////////////////////////////

#if ! WITHDSPEXTDDC

static RAMDTCM const ncoftw_t anglestep_iflo = (1U << (NCOFTWBITS - 2));	// установить частоту в 1/4 sample rate
static RAMDTCM ncoftw_t angle_iflo = 0;


// Получение квадратурных значений для данной частоты
static FLOAT32P_t get_float4_iflo(void)
{
	static const FLOAT_t sintable4f [4] = { + 0, + 1, + 0, - 1 };
	FLOAT32P_t v;
	v.IV = (sintable4f [(angle_iflo) >> 30]);
	v.QV = (sintable4f [(angle_iflo + 0x40000000uL) >> 30]);
	angle_iflo = FTWROUND(angle_iflo + anglestep_iflo);
	return v;
}

#endif /* ! WITHDSPEXTDDC */


//////////////////////////////////////////

// Преобразовать отношение напряжений выраженное в "разах" к децибелам.

static FLOAT_t ratio2db(FLOAT_t ratio)
{
	return LOG10F(ratio) * 20;
}

// Преобразовать отношение выраженное в децибелах к "разам" отношения напряжений.

static FLOAT_t db2ratio(FLOAT_t valueDBb)
{
	return POWF(10, valueDBb / 20);
}

//////////////////////////////////////////

// Адаптер - преобразователь формата из внешнего по отношению к DSP блоку формата.
// Внешние форматы - целочисленные - в основном определяются типами зранящимися в DMA буфере данными.
// Внутренний формат - FLOAT_t в диапазоне -1 .. +1

void adpt_initialize(
	adapter_t * adp,
	int leftbit,	// Номер бита слева от знакового во внешнем формате в значащих разрядах
	int rightspace,	// количество незанятых битов справа.
	const char * name
	)
{
	int signpos = leftbit - 1;
	/* Форматы с павающей точкой обеспечивают точное представление степеней двойки */
	adp->inputK = LDEXPF(1, - signpos);
	adp->outputK = LDEXPF(1, signpos) * db2ratio(- (FLOAT_t) 0.03);
	adp->outputKexact = LDEXPF(1, signpos);
	adp->rightspace = rightspace;
	adp->leftbit = leftbit;
	adp->lshift32 = 32 - leftbit - rightspace;
	adp->rshift32 = 32 - leftbit;
	//PRINTF("adpt_initialize: leftbit=%d, rightspace=%d, lshift32=%d, rshift32=%d\n", leftbit, rightspace, adp->lshift32, adp->rshift32);
	adp->name = name;
}

// Преобразование значения целочисленного типа во внутреннее представление.
FLOAT_t adpt_input(const adapter_t * adp, int32_t v)
{
	return (FLOAT_t) ((v << adp->lshift32) >> adp->rshift32) * adp->inputK;
}

// Преобразование во внешнее (целочисленное) представление.
int32_t adpt_output(const adapter_t * adp, FLOAT_t v)
{
//	if (v < -1 || v > 1)
//	{
//		PRINTF("adpt_output: '%s' v=%f\n", adp->name, v);
//	}
//	ASSERT(v <= 1);
//	ASSERT(v >= - 1);
	return (int32_t) (adp->outputK * v) << adp->rightspace;
}

// Преобразование во внешнее представление.
int32_t adpt_outputL(const adapter_t * adp, double v)
{
//	if (v < -1 || v > 1)
//	{
//		PRINTF("adpt_outputL: '%s' v=%f\n", adp->name, v);
//	}
//	ASSERT(v <= 1);
//	ASSERT(v >= - 1);
	return (int32_t) (adp->outputK * v) << adp->rightspace;
}

// точное преобразование во внешнее представление.
int32_t adpt_outputexact(const adapter_t * adp, FLOAT_t v)
{
//	if (v < -1 || v > 1)
//	{
//		PRINTF("adpt_outputexact: '%s' v=%f\n", adp->name, v);
//	}
//	ASSERT(v <= 1);
//	ASSERT(v >= - 1);
	return (int32_t) (adp->outputKexact * v) << adp->rightspace;
}

// точное преобразование во внешнее представление.
int32_t adpt_outputexactL(const adapter_t * adp, double v)
{
//	if (v < -1 || v > 1)
//	{
//		PRINTF("adpt_outputexactL: '%s' v=%f\n", adp->name, v);
//	}
//	ASSERT(v <= 1);
//	ASSERT(v >= - 1);
	return (int32_t) (adp->outputKexact * v) << adp->rightspace;
}

// точное преобразование между внешними целочисленными представлениями.
void transform_initialize(
	transform_t * tfm,
	const adapter_t * informat,
	const adapter_t * outformat
	)
{
	const int inwidth = informat->leftbit + informat->rightspace;
	const int outwidth = outformat->leftbit + outformat->rightspace;
	tfm->lshift32 = 32 - inwidth;
	tfm->rshift32 = 32 - outwidth;
	tfm->lshift64 = 64 - inwidth;
	tfm->rshift64 = 64 - outwidth;
}

// точное преобразование между внешними целочисленными представлениями.
// Знаковое число 32 бит
int32_t transform_do32(
	const transform_t * tfm,
	int32_t v
	)
{
	return (v << tfm->lshift32) >> tfm->rshift32;
}

// точное преобразование между внешними целочисленными представлениями.
// Знаковое число 64 бит
int64_t transform_do64(
	const transform_t * tfm,
	int64_t v
	)
{
	return (v << tfm->lshift64) >> tfm->rshift64;
}

static adapter_t fpgafircoefsout;
adapter_t afcodecrx;		/* от микрофона */
adapter_t afcodectx;		/* к наушникам */
adapter_t ifcodecrx;		/* канал от FPGA к процессору */
adapter_t ifcodectx;		/* канал от процессора к FPGA */

#if WITHRTS96
adapter_t ifspectrumin96;	/* канал от FPGA к процессору */
adapter_t rts96in;	/* Аудиоданные (спектр) в компютер из трансивера */
#endif /* WITHRTS96 */

#if WITHRTS192
adapter_t ifspectrumin192;	/* канал от FPGA к процессору */
adapter_t rts192in;	/* Аудиоданные (спектр) в компютер из трансивера */
#endif /* WITHRTS96 */
adapter_t nfmdemod;		/* Преобразование выхода demodulator_FM() */

#if WITHUSEAUDIOREC
adapter_t sdcardio;
#endif /* WITHUSEAUDIOREC */

#if WITHRTS96
transform_t if2rts96out;	// преобразование из выхода панорамы FPGA в формат UAB AUDIO RTS
#endif /* WITHRTS96 */

#if WITHRTS192
transform_t if2rts192out;	// преобразование из выхода панорамы FPGA в формат UAB AUDIO RTS
#endif /* WITHRTS192 */

static void adapterst_initialize(void)
{
	/* FPGA FIR коэффициенты */
	adpt_initialize(& fpgafircoefsout, HARDWARE_COEFWIDTH, 0, "fpgafircoefsout");
	/* Аудиокодек */
	adpt_initialize(& afcodecrx, WITHADAPTERCODEC1WIDTH, WITHADAPTERCODEC1SHIFT, "afcodecrx");
	adpt_initialize(& afcodectx, WITHADAPTERCODEC1WIDTH, WITHADAPTERCODEC1SHIFT, "afcodectx");
	/* IF codec / FPGA */
	adpt_initialize(& ifcodecrx, WITHADAPTERIFADCWIDTH, WITHADAPTERIFADCSHIFT, "ifcodecrx");
	adpt_initialize(& ifcodectx, WITHADAPTERIFDACWIDTH, WITHADAPTERIFDACSHIFT, "ifcodectx");
#if WITHUSEAUDIOREC
	/* SD CARD */
	adpt_initialize(& sdcardio, audiorec_getwidth(), 0, "sdcardio");
#endif /* WITHUSEAUDIOREC */

#if WITHRTS96
	/* канал квадратур USB AUDIO */
	adpt_initialize(& ifspectrumin96, WITHADAPTERRTS96_WIDTH, WITHADAPTERRTS96_SHIFT, "ifspectrumin96");
	adpt_initialize(& rts96in, UACIN_RTS96_SAMPLEBYTES * 8, 0, "rts96in");
	transform_initialize(& if2rts96out, & ifspectrumin96, & rts96in);
#endif /* WITHRTS96 */
#if WITHRTS192
	/* канал квадратур USB AUDIO */
	adpt_initialize(& ifspectrumin192, WITHADAPTERRTS192_WIDTH, WITHADAPTERRTS192_SHIFT, "ifspectrumin192");
	adpt_initialize(& rts192in, UACIN_RTS192_SAMPLEBYTES * 8, 0, "rts192in");
	transform_initialize(& if2rts192out, & ifspectrumin192, & rts192in);
#endif /* WITHRTS192 */
	/* Преобразование выхода demodulator_FM() */
	adpt_initialize(& nfmdemod, 32, 0, "nfmdemod");
}

//////////////////////////////////////////

#if 1

// функции для замены стандартной библиотеки

#define FP_OPER 1		/* FPU op code error			*/
#define FP_ZDIV 2		/* FPU divide by zero			*/
#define FP_FTOI 3		/* FPU float to integer conv error	*/
#define FP_OFLO 4		/* FPU overflow				*/
#define FP_UFLO 5		/* FPU underflow			*/
#define FP_UDEF 6		/* FPU undefined variable (-0)		*/
#define FP_BIGI 7		/* Atof input too large			*/
#define FP_BADC 8		/* Bad character in atof input string	*/
#define FP_NESQ 9		/* Square root of negative number	*/
#define FP_LEXP	10		/* Exp argument too large		*/
#define FP_SEXP 11		/* Exp argument too small		*/
#define FP_NLOG 12		/* Log argument zero or negative	*/
#define FP_TANE 13		/* Argument of tan too large		*/
#define FP_TRIG 14		/* Argument of sin/cos too large	*/
#define FP_ATAN 15		/* Atan2 arguments both zero		*/
#define FP_COTE 16		/* Argument of cotan too small		*/
#define FP_ARSC 17		/* Bad argument for asin/acos		*/
#define FP_SINH 18		/* Argument of sinh too large		*/
#define FP_COSH 19		/* Argument of cosh too large		*/
#define FP_POWN 20		/* Negative argument in pow		*/
#define FP_POWO 21		/* Result of pow overflows		*/
#define FP_POWU 22		/* Result of pow underflows		*/

#define LOG_HUGE ((FLOAT_t) 0.880296919311130543E+02)	/* log of HUGE		*/
#define LOG_TINY ((FLOAT_t) -0.887228391116729997E+02)	/* log of TINY		*/

#define LOGBE2 ((FLOAT_t) 0.69314718055994530942)	/* log of 2 to base e		*/
#define LOGB2E ((FLOAT_t) 1.44269504088896341)	/* log of e to base 2		*/

#define HUGE ((FLOAT_t) 0.170141183460469230E+39)   /* largest no = 2**+127     */


static void CMEMSGF(int erno, FLOAT_t * datap)
{
	* datap = 0;
}


// tnx UA3REO
FLOAT_t local_log10(FLOAT_t x)
{
	FLOAT_t Y, F;
	int e;

	F = FREXPF(FABSF(x), & e);
	Y = (FLOAT_t) 1.23149591368684;
	Y *= F;
	Y += (FLOAT_t) -4.11852516267426;
	Y *= F;
	Y += (FLOAT_t) 6.02197014179219;
	Y *= F;
	Y += (FLOAT_t) -3.13396450166353;
	Y += e;
	return (Y * (FLOAT_t) 0.3010299956639812);
}

FLOAT_t local_log(FLOAT_t x)
{
	static const FLOAT_t C1 = ((FLOAT_t) 0.693359375);		/* C1 + C2 should represent log 2 to	*/
	static const FLOAT_t C2 = ((FLOAT_t) -2.12194440054690583e-4);	/* more than machine precision	*/

	static const FLOAT_t a0 = -0.641249434237455811e+2;
	static const FLOAT_t a1 =  0.163839435630215342e+2;
	static const FLOAT_t a2 = -0.789561128874912573e+0;
	static const FLOAT_t b0 = -0.769499321084948798e+3;
	static const FLOAT_t b1 =  0.312032220919245328e+3;
	static const FLOAT_t b2 = -0.356679777390346462e+2;

    FLOAT_t a, b, f, r, w, z;
    int n;

    if (x <= (FLOAT_t)0.0) 
	{
		CMEMSGF(FP_NLOG, &x);
		if (x == (FLOAT_t)0.0)
			return(-HUGE);
		else
			x = -x;
	}

    f = FREXPF(x, &n);
    if (f > M_SQRT1_2) 
	{
		a = (f - (FLOAT_t)0.5) - (FLOAT_t)0.5;
		b = f * (FLOAT_t)0.5 + (FLOAT_t)0.5;
    }
    else 
	{
		n--;
		a = f - (FLOAT_t)0.5;
		b = a * (FLOAT_t)0.5 + (FLOAT_t)0.5;
    }
    z = a / b;
    w = z * z;
    a = (a2 * w + a1) * w + a0;
    b = ((w + b2) * w + b1) * w + b0;
    r = z + z * w * a / b;
    z = n;
    return ((z * C2 + r) + z * C1);
}

// from ftp://ftp.update.uu.se/pub/pdp11/rt/cmath/pow.c

#define MAXEXP 2031		/* (MAX_EXP * 16) - 1			*/
#define MINEXP (-2047)		/* (MIN_EXP * 16) - 1			*/

FLOAT_t local_pow(FLOAT_t x, FLOAT_t y)
{

	static const FLOAT_t a1[] = {
		1.0,
		0.95760328069857365,
		0.91700404320467123,
		0.87812608018664974,
		0.84089641525371454,
		0.80524516597462716,
		0.77110541270397041,
		0.73841307296974966,
		0.70710678118654752,
		0.67712777346844637,
		0.64841977732550483,
		0.62092890603674203,
		0.59460355750136054,
		0.56939431737834583,
		0.54525386633262883,
		0.52213689121370692,
		0.50000000000000000
	};
	static const FLOAT_t a2[] = {
		 0.24114209503420288E-17,
		 0.92291566937243079E-18,
		-0.15241915231122319E-17,
		-0.35421849765286817E-17,
		-0.31286215245415074E-17,
		-0.44654376565694490E-17,
		 0.29306999570789681E-17,
		 0.11260851040933474E-17
	};

	static const FLOAT_t p1 = 0.833333333333332114e-1;
	static const FLOAT_t p2 = 0.125000000005037992e-1;
	static const FLOAT_t p3 = 0.223214212859242590e-2;
	static const FLOAT_t p4 = 0.434457756721631196e-3;
	static const FLOAT_t q1 = 0.693147180559945296e0;
	static const FLOAT_t q2 = 0.240226506959095371e0;
	static const FLOAT_t q3 = 0.555041086640855953e-1;
	static const FLOAT_t q4 = 0.961812905951724170e-2;
	static const FLOAT_t q5 = 0.133335413135857847e-2;
	static const FLOAT_t q6 = 0.154002904409897646e-3;
	static const FLOAT_t q7 = 0.149288526805956082e-4;
	static const FLOAT_t k = 0.442695040888963407;

    FLOAT_t g, r, u1, u2, v, w, w1, w2, y1, y2, z;
    int iw1, m, p;

    if (y == 0)
		return 1;
    if (x <= 0) 
	{
		if (x == 0) 
		{
			if (y > 0)
				return(x);
			CMEMSGF(FP_POWO, &y);
			return(HUGE);
		}
		else 
		{
			CMEMSGF(FP_POWN, &x);
			x = -x;
		}
    }
    g = FREXPF(x, &m);
    p = 0;
    if (g <= a1[8])
	p = 8;
    if (g <= a1[p + 4])
	p += 4;
    if (g <= a1[p + 2])
	p += 2;
    p++;
    z = ((g - a1[p]) - a2[p / 2]) / (g + a1[p]);
    z += z;
    v = z * z;
    r = (((p4 * v + p3) * v + p2) * v + p1) * v * z;
    r += k * r;
    u2 = (r + z * k) + z;
    u1 = (FLOAT_t)0.0625 * (FLOAT_t)(16 * m - p);
    y1 = (FLOAT_t)0.0625 * (FLOAT_t)((int)(16 * y));
    y2 = y - y1;
    w = u2 * y + u1 * y2;
    w1 = (FLOAT_t)0.0625 * (FLOAT_t)((int)(16 * w));
    w2 = w - w1;
    w = w1 + u1 * y1;
    w1 = (FLOAT_t)0.0625 * (FLOAT_t)((int)(16 * w));
    w2 += (w - w1);
    w = (FLOAT_t)0.0625 * (FLOAT_t)((int)(16 * w2));
    iw1 = 16 * (w1 + w);
    w2 -= w;
    while (w2 > 0) 
	{
		iw1++;
		w2 -= (FLOAT_t) 0.0625;
    }
    if (iw1 > MAXEXP) 
	{
		CMEMSGF(FP_POWO, &y);
		return(HUGE);
    }
    if (iw1 < MINEXP) 
	{
		CMEMSGF(FP_POWU, &y);
		return(0);
    }
    m = iw1 / 16;
    if (iw1 >= 0)
	m++;	
    p = 16 * m - iw1;
    z = ((((((q7*w2 + q6)*w2 + q5)*w2 + q4)*w2 + q3)*w2 + q2)*w2 + q1)*w2;
    z = a1[p] + a1[p] * z;
    return LDEXPF(z, m);

}

// from: ftp://ftp.update.uu.se/pub/pdp11/rt/cmath/exp.c


FLOAT_t local_exp(FLOAT_t x)
{
	static const FLOAT_t EPS2 = ((FLOAT_t) 6.93889401e-18);	/* exp(eps) = 1.0 to m/c precision	*/
	static const FLOAT_t C1 = ((FLOAT_t) 0.693359375);		/* C1 + C2 should represent log 2 to	*/

	static const FLOAT_t C2 = ((FLOAT_t) -2.12194440054690583e-4);	/* more than machine precision	*/
	static const FLOAT_t p0 = 0.249999999999999993e0;
	static const FLOAT_t p1 = 0.694360001511792852e-2;
	static const FLOAT_t p2 = 0.165203300268279130e-4;
	static const FLOAT_t q0 = 0.5;
	static const FLOAT_t q1 = 0.555538666969001188e-1;
	static const FLOAT_t q2 = 0.495862884905441294e-3;

    FLOAT_t g, gp, q, xn, z;
    int n;

/* check for exponent too large, if so print argument and return large no */
    if (x >= LOG_HUGE){
		CMEMSGF(FP_LEXP, & x);
		return(HUGE);
    }
/* check for exponent underflow, if so call error system (set at warning
   level) and return zero */
    if (x < LOG_TINY){
		CMEMSGF(FP_SEXP, & x);
		return (0);
    }
    if (x < EPS2 && x > - EPS2)
		return 1;
    z = x * LOGB2E;
    if (z < 0)
		n = z - (FLOAT_t)0.5;
    else
		n = z + (FLOAT_t)0.5;
    xn = n;
    g = (x - xn * C1) - xn * C2;
    z = g * g;
    gp = ((p2 * z + p1) * z + p0) * g;
    q = (q2 * z + q1) * z + q0;
    return(LDEXPF((FLOAT_t) 0.5 + gp / (q - gp), ++n));
}

#endif


//////////////////////////////////////////


// Нормирование уровня сигнала к шкале
// возвращает значения от 0 до ymax включительно
// 0 - минимальный сигнал, ymax - максимальный
int dsp_mag2y(
	FLOAT_t mag,
	int ymax,
	int_fast16_t topdb,		/* верхний предел спектроанализатора (positive number of decibels) */
	int_fast16_t bottomdb		/* нижний предел спектроанализатора (positive number of decibels) */
	)
{
	const FLOAT_t r = ratio2db(mag);
	const int y = ymax - (int) ((r + topdb) * ymax / - (bottomdb - topdb));

	if (y > ymax)
		return ymax;
	if (y < 0)
		return 0;
	return y;
}


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
	ARM_MORPH(arm_cfft_instance) fftinstance;
	VERIFY(ARM_MATH_SUCCESS == ARM_MORPH(arm_cfft_init)(& fftinstance, FFTSizeFilters));
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
//	for (i = iCoefNum; i < FFTSizeFilters; ++ i) {
//		Sig [i].real = 0;
//		Sig [i].imag = 0;
//	}
	ARM_MORPH(arm_fill)(0, (FLOAT_t *) & Sig [iCoefNum], (FFTSizeFilters - iCoefNum) * 2);
	//---------------------------
	// Do FFT
	//---------------------------


	/* Process the data through the CFFT/CIFFT module */
	ARM_MORPH(arm_cfft)(& fftinstance, (FLOAT_t *) Sig, 0, 1);

	//ARM_MORPH(arm_cmplx_mag_squared)(sg, MagArr, MagLen);

}

static void sigtocoeffs(FLOAT_t *dCoeff, int iCoefNum)
{
	const int j = NtapCoeffs(iCoefNum);
	int i;
	//---------------------------
	// Magnitude in dB
	//---------------------------
	for (i = 0; i < j && i < FFTSizeFilters; ++ i) {
		dCoeff [i] = Sig [i].real;
	}

}

static void scalecoeffs(FLOAT_t *dCoeff, int iCoefNum, FLOAT_t scale)
{
	const int j = NtapCoeffs(iCoefNum);
	ARM_MORPH(arm_scale)(dCoeff, scale, dCoeff, j);
}


static void fir_design_applaywindow(FLOAT_t *dCoeff, const FLOAT_t *dWindow, int iCoefNum);
static void fir_design_applaywindowL(double *dCoeff, const double *dWindow, int iCoefNum);

// slope: изменение тембра звука - на Samplerate/2 АЧХ становится на столько децибел
// scale: общий масштаб изменения АЧХ
static void correctspectrumcomplex(int_fast8_t targetdb)
{
	ARM_MORPH(arm_cfft_instance) fftinstance;

	VERIFY(ARM_MATH_SUCCESS == ARM_MORPH(arm_cfft_init)(& fftinstance, FFTSizeFilters));
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

	// Construct FIR coefficients from frequency response
	/* Process the data through the CFFT/CIFFT module */
	ARM_MORPH(arm_cfft)(& fftinstance, (FLOAT_t *) Sig, !0, 1);	// inverse FFT

	//arm_cmplx_mag_squared_f32(sg, MagArr, MagLen);
}

#define GAIN_1 1
// Формирование наклона АЧХ звукового тракта приёмника
static void fir_design_adjust_rx(FLOAT_t * dCoeff, const FLOAT_t * dWindow, int iCoefNum, uint_fast8_t usewindow, FLOAT_t gain)
{
	if (glob_afresponcerx != 0)
	{
		imp_response(dCoeff, iCoefNum);	// Получение АЧХ из коэффициентов симмметричного FIR
		correctspectrumcomplex(glob_afresponcerx);
		sigtocoeffs(dCoeff, iCoefNum);
	}

	if (usewindow != 0)
		fir_design_applaywindow(dCoeff, dWindow, iCoefNum);

	imp_response(dCoeff, iCoefNum);	// Получение АЧХ из коэффициентов симмметричного FIR для последующего масштабирования коэффициентов
	const FLOAT_t resp = getmaxresponce();
	scalecoeffs(dCoeff, iCoefNum, gain / resp);	// нормализация коэффициентоа передачи к заданному значению (1)
}

// Формирование наклона АЧХ звукового тракта передатчика
static void fir_design_adjust_tx(FLOAT_t *dCoeff, const FLOAT_t *dWindow, int iCoefNum)
{
	if (glob_afresponcetx != 0)
	{
		imp_response(dCoeff, iCoefNum);	// Получение АЧХ из коэффициентов симмметричного FIR
		correctspectrumcomplex(glob_afresponcetx);
		sigtocoeffs(dCoeff, iCoefNum);
	}
	fir_design_applaywindow(dCoeff, dWindow, iCoefNum);
	imp_response(dCoeff, iCoefNum);	// Получение АЧХ из коэффициентов симмметричного FIR для последующего масштабирования коэффициентов
	const FLOAT_t resp = getmaxresponce();
	scalecoeffs(dCoeff, iCoefNum, 1 / resp);	// нормализация к. передаци к заданному значению (1)
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

// Аргумент: постоянная времени цепи в секундах
// Результат: 1 - мгновенно, 0 - никогда
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
	LCLSPINLOCK_t lock /* = LCLSPINLOCK_INIT */;
} agcstate_t;

typedef struct agcparams
{
	uint_fast8_t agcoff;	// признак отключения АРУ

	// Временные парметры АРУ

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
	agcp->chargespeedfast = MAKETAUAF0();

	agcp->chargespeedslow = MAKETAUIF((FLOAT_t) 0.095);
	agcp->dischargespeedslow = MAKETAUIF((FLOAT_t) 0.2);	

	agcp->hungticks = NSAITICKS(300);			// 0.3 seconds

	agcp->gainlimit = db2ratio(60);
	agcp->mininput = db2ratio(- 160);
	agcp->levelfence = 1;
	agcp->agcfactor = agc_calcagcfactor(10);

	//PRINTF(PSTR("agc_parameters_initialize: dischargespeedfast=%f, chargespeedfast=%f\n"), agcp->dischargespeedfast, agcp->chargespeedfast);
}

// Установка параметров АРУ приёмника

static void agc_parameters_update(volatile agcparams_t * const agcp, FLOAT_t gainlimit, uint_fast8_t pathi)
{
	const uint_fast8_t flatgain = glob_agcrate [pathi] == UINT8_MAX;

	agcp->agcoff = (glob_dspagc == BOARD_AGCCODE_OFF);

	agcp->dischargespeedfast = MAKETAUIF((int) glob_agc_t4 [pathi] * (FLOAT_t) 0.001);	// в милисекундах

	agcp->chargespeedfast = MAKETAUIF((int) glob_agc_t0 [pathi] * (FLOAT_t) 0.001);	// в милисекундах
	agcp->chargespeedslow = MAKETAUIF((int) glob_agc_t1 [pathi] * (FLOAT_t) 0.001);	// в милисекундах
	agcp->dischargespeedslow = MAKETAUIF((int) glob_agc_t2 [pathi] * (FLOAT_t) 0.1);	// в сотнях милисекунд (0.1 секунды)
	agcp->hungticks = NSAITICKS(glob_agc_thung [pathi] * 100);			// в сотнях милисекунд (0.1 секунды)

	agcp->gainlimit = gainlimit;
	agcp->levelfence = (int) glob_agc_scale [pathi] * (FLOAT_t) 0.01;	/* Для эксперементов по улучшению приема АМ */
	agcp->agcfactor = flatgain ? (FLOAT_t) -1 : agc_calcagcfactor(glob_agcrate [pathi]);

	//PRINTF(PSTR("agc_parameters_update: dischargespeedfast=%f, chargespeedfast=%f\n"), agcp->dischargespeedfast, agcp->chargespeedfast);
}

// Установка параметров S-метра приёмника

static void agc_smeter_parameters_update(volatile agcparams_t * const agcp)
{
	agcp->agcoff = 0;

	agcp->chargespeedfast = MAKETAUIF((FLOAT_t) 0.1);	// 100 mS
	agcp->dischargespeedfast = MAKETAUIF((FLOAT_t) 0.1);	// 100 mS
	agcp->chargespeedslow = MAKETAUIF((FLOAT_t) 0.1);	// 100 mS
	agcp->dischargespeedslow = MAKETAUIF((FLOAT_t) 0.4);	// 400 mS
	agcp->hungticks = NSAITICKS(1000);			// в сотнях милисекунд (1 секунда)

	agcp->gainlimit = db2ratio(60);
	agcp->agcfactor = (FLOAT_t) -1;

#if CTLSTYLE_OLEG4Z_V1
	agcp->chargespeedfast = MAKETAUAF0();
	agcp->chargespeedfast = MAKETAUIF((FLOAT_t) 0.005);	// 5 mS
	agcp->dischargespeedfast = MAKETAUIF((FLOAT_t) 0.005);	// 5 mS
#endif /* CTLSTYLE_OLEG4Z_V1 */

	//PRINTF(PSTR("agc_parameters_update: dischargespeedfast=%f, chargespeedfast=%f\n"), agcp->dischargespeedfast, agcp->chargespeedfast);
}

// Начальная установка параметров АРУ микрофонного тракта передатчика

static void comp_parameters_initialize(volatile agcparams_t * agcp)
{
	agcp->agcoff = 0;

	agcp->dischargespeedfast = MAKETAUIF((FLOAT_t) 0.100);
	agcp->chargespeedfast = MAKETAUAF0();

	agcp->chargespeedslow = MAKETAUIF((FLOAT_t) 0.200);
	agcp->dischargespeedslow = MAKETAUIF((FLOAT_t) 0.200);

	agcp->hungticks = NSAITICKS(300);			// 0.3 seconds

	agcp->gainlimit = db2ratio(60);
	agcp->mininput = db2ratio(- 160);
	agcp->levelfence = txlevelfenceSSB;
	agcp->agcfactor = (FLOAT_t) - 1;
}

// Установка параметров АРУ передатчика

static void comp_parameters_update(volatile agcparams_t * const agcp, FLOAT_t gainlimit)
{
	agcp->agcoff = glob_mikeagc == 0;

	agcp->gainlimit = gainlimit;
	agcp->levelfence = txlevelfenceSSB;
}

// детектор АРУ - поддерживает выходное значение пропорционально сигналу
// со всеми положенными задержками на срабатывание/отпускание

static void
agc_perform(const agcparams_t * agcp, agcstate_t * st, FLOAT_t sample)
{
	LCLSPIN_LOCK(& st->lock);

	if (st->agcfastcap < sample)
	{
		// быстрая цепь АРУ
		// заряжается в соответствии с параметром agcp->chargespeedfast
		charge2(& st->agcfastcap, sample, agcp->chargespeedfast);
	}
	else
	{
		// быстрая цепь АРУ
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
	LCLSPIN_UNLOCK(& st->lock);
}

static FLOAT_t agc_result_slow(agcstate_t * st)
{
	LCLSPIN_LOCK(& st->lock);
	const FLOAT_t v = FMAXF(st->agcfastcap, st->agcslowcap);	// разница после ИЛИ
	LCLSPIN_UNLOCK(& st->lock);

	return v;
}

static FLOAT_t agc_result_fast(agcstate_t * st)
{
	LCLSPIN_LOCK(& st->lock);
	const FLOAT_t v = st->agcfastcap;
	LCLSPIN_UNLOCK(& st->lock);

	return v;
}


///////////////////////////

static RAMDTCM FLOAT_t mikeinlevel;
static RAMDTCM FLOAT_t VOXDISCHARGE;
static RAMDTCM FLOAT_t VOXCHARGE = 0;

static RAMDTCM FLOAT_t dvoxlevel;
static RAMDTCM FLOAT_t DVOXDISCHARGE;
static RAMDTCM FLOAT_t DVOXCHARGE = 0;

// Возвращает значения 0..255
uint_fast8_t dsp_getvox(uint_fast8_t fullscale)
{
	unsigned v = FMAXF(mikeinlevel, 0 /* datavox == 0 ? 0 : dvoxlevel */ ) * fullscale;	// масшабирование к 0..255
	return v > fullscale ? fullscale : v;
}

// Возвращает значения 0..255
uint_fast8_t dsp_getavox(uint_fast8_t fullscale)
{
	return 0;
}

static void voxmeter_initialize(void)
{
	VOXCHARGE = MAKETAUAF0();	// Пиковый детектор со временем заряда 0
	VOXDISCHARGE = MAKETAUAF((FLOAT_t) 0.02);	// Пиковый детектор со временем разряда 0.02 секунды

	DVOXCHARGE = MAKETAUAF0();	// Пиковый детектор со временем заряда 0
	DVOXDISCHARGE = MAKETAUAF((FLOAT_t) 0.02);	// Пиковый детектор со временем разряда 0.02 секунды
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
	//PRINTF("testgain_float_DC: dSum=%f\n", dSum);
	if (dSum == 0)
		return 1;
	return dSum; //FABSF(dSum);		// Получаем модуль усиления на постоянной составляющей.
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
// https://www.edn.com/windowing-functions-improve-fft-results-part-i/
// https://en.wikipedia.org/wiki/Window_function
// https://www.weisang.com/en/documentation/fourierspectrumtaperingwindows_en/

FLOAT_t fir_design_window(int iCnt, int iCoefNum, int wtype)
{
	const FLOAT_t n = iCoefNum - 1;
	const FLOAT_t a = (FLOAT_t) M_TWOPI * iCnt / n;
	const FLOAT_t a2 = 2 * (FLOAT_t) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	const FLOAT_t a3 = 3 * (FLOAT_t) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	const FLOAT_t a4 = 4 * (FLOAT_t) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	const FLOAT_t a5 = 5 * (FLOAT_t) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	const FLOAT_t a6 = 6 * (FLOAT_t) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	
	switch (wtype)
	{
	default:
	case BOARD_WTYPE_BLACKMAN_HARRIS:
		// Blackman-Harris (same as MATLAB)
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
	case BOARD_WTYPE_BLACKMAN_HARRIS_4TERM:
		// Blackman-Harris (4-term)
		// Cos4 Blackman-Harris -74dB W=4
		// 0.40217-0.49703*cos(2*π*i/(n-1))+0.09892*cos(4*π*i/(n-1))-0.00188*cos(6*π*i/(n-1)), i=0..n-1
		// https://www.edn.com/windowing-functions-improve-fft-results-part-ii/
		// 0.40217, 0.49703, 0.09892, and 0.00188.
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.40217
				- (FLOAT_t) 0.49703 * COSF(a)
				+ (FLOAT_t) 0.09892 * COSF(a2)
				- (FLOAT_t) 0.00188 * COSF(a3)
				);
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
	case BOARD_WTYPE_NUTTALL:
		// Nuttall window, continuous first derivative
		{
			const FLOAT_t w = (
				+ (FLOAT_t) 0.355768
				- (FLOAT_t) 0.487396 * COSF(a)
				+ (FLOAT_t) 0.144232 * COSF(a2)
				- (FLOAT_t) 0.012604 * COSF(a3)
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

#if 0
// takem from Wolf project (UA3REO)
//Windowing
//Dolphâ€“Chebyshev
{
	const float64_t atten = 100.0;
	float64_t max = 0.0;
	float64_t tg = pow(10.0, atten / 20.0);
	float64_t x0 = cosh((1.0 / ((float64_t)FFT_SIZE - 1.0)) * acosh(tg));
	float64_t M = (FFT_SIZE - 1) / 2;
	if ((FFT_SIZE % 2) == 0)
		M = M + 0.5; /* handle even length windows */
	for (uint32_t nn = 0; nn < ((FFT_SIZE / 2) + 1); nn++)
	{
		float64_t n = nn - M;
		float64_t sum = 0.0;
		for (uint32_t i = 1; i <= M; i++)
		{
			float64_t cheby_poly = 0.0;
			float64_t cp_x = x0 * cos(M_PI * i / (float64_t)FFT_SIZE);
			float64_t cp_n = FFT_SIZE - 1;
			if (fabs(cp_x) <= 1)
				cheby_poly = cos(cp_n * acos(cp_x));
			else
				cheby_poly = cosh(cp_n * acosh(cp_x));

			sum += cheby_poly * cos(2.0 * n * M_PI * (float64_t)i / (float64_t)FFT_SIZE);
		}
		window_multipliers[nn] = tg + 2 * sum;
		window_multipliers[FFT_SIZE - nn - 1] = window_multipliers[nn];
		if (window_multipliers[nn] > max)
			max = window_multipliers[nn];
	}
	for (uint32_t nn = 0; nn < FFT_SIZE; nn++)
		window_multipliers[nn] /= max; /* normalise everything */
}

#endif

//#if (__ARM_FP & 0x08) && 1

// Calculate window function (blackman-harris, hamming, rectangular)
static double fir_design_windowL(int iCnt, int iCoefNum, int wtype)
{
	const double n = iCoefNum - 1;
	const double a = (double) M_TWOPI * iCnt / n;
	const double a2 = 2 * (double) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	const double a3 = 3 * (double) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	const double a4 = 4 * (double) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	const double a5 = 5 * (double) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	const double a6 = 6 * (double) M_TWOPI * iCnt / n;	// для повышения точности умножение перенесено до деления
	
	switch (wtype)
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
//#endif

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
	ARM_MORPH(arm_mult)(dCoeff, dWindow, dCoeff, NtapCoeffs(iCoefNum));	// arm_mult_f32/arm_mult_f64
}

// Наложение оконной функции
static void fir_design_applaywindowL(double *dCoeff, const double *dWindow, int iCoefNum)
{
	arm_mult_f64(dCoeff, dWindow, dCoeff, NtapCoeffs(iCoefNum));
}

// подготовка буфера с оконной функцией
// Учитываем симметрию.
static void fir_design_windowbuff_half(FLOAT_t *dWindow, int iCoefNum)
{
	const int j = NtapCoeffs(iCoefNum);
	int iCnt;

    ASSERT((iCoefNum % 2) == 1);
    for (iCnt = 0; iCnt < j; iCnt ++)
	{
		dWindow [iCnt] = fir_design_window(iCnt, iCoefNum, BOARD_WTYPE_FILTERS);
	}
}
//#if (__ARM_FP & 0x08) && 1

// подготовка буфера с оконной функцией
// Учитываем симметрию.
static void fir_design_windowbuffL_half(double *dWindow, int iCoefNum)
{
	const int j = NtapCoeffs(iCoefNum);
	int iCnt;
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		dWindow [iCnt] = fir_design_windowL(iCnt, iCoefNum, BOARD_WTYPE_FILTERS);
	}
}
//#endif

// Масштабирование для симметричного фильтра
static void fir_design_scale(FLOAT_t * dCoeff, int iCoefNum, FLOAT_t dScale)
{
	if (dScale == 1)
		return;
	ARM_MORPH(arm_scale)(dCoeff, dScale, dCoeff, NtapCoeffs(iCoefNum));	// arm_scale_f32/arm_scale_f64
}

// Масштабирование для симметричного фильтра
static void fir_design_scaleL(double * dCoeff, int iCoefNum, double dScale)
{
	if (dScale == 1)
		return;
	arm_scale_f64(dCoeff, dScale, dCoeff, NtapCoeffs(iCoefNum));
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

static void printdcoefs(const FLOAT_t * dCoeff, int iCoefNum, int line, const char * file)
{
	const int j = NtapCoeffs(iCoefNum);
	int iCnt;
	PRINTF("printdcoefs at %s/%d:\n", file, line);
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		PRINTF("%g,", dCoeff [iCnt]);
	}
	PRINTF("(iCnt=%d)\n", iCnt);
}

// с управлением крутизной скатов и нормированием усиления, с наложением окна
static void fir_design_lowpass_freq_scaled(FLOAT_t * dCoeff, const FLOAT_t * dWindow, int iCoefNum, int iCutHigh, FLOAT_t dGain)
{
	fir_design_lowpass(dCoeff, iCoefNum, fir_design_normfreq(iCutHigh));
	//printdcoefs(dCoeff, iCoefNum, __LINE__, __FILE__);
	fir_design_applaywindow(dCoeff, dWindow, iCoefNum);
	//printdcoefs(dCoeff, iCoefNum, __LINE__, __FILE__);
	fir_design_scale(dCoeff, iCoefNum, dGain / testgain_float_DC(dCoeff, iCoefNum));
	//testgain_float_DC(dCoeff, iCoefNum);
	//printdcoefs(dCoeff, iCoefNum, __LINE__, __FILE__);
}

//#if (__ARM_FP & 0x08) && 1

// с управлением крутизной скатов и нормированием усиления, с наложением окна
static void fir_design_lowpass_freq_scaledL(double * dCoeff, const double * dWindow, int iCoefNum, int iCutHigh, double dGain)
{
	fir_design_lowpassL(dCoeff, iCoefNum, fir_design_normfreqL(iCutHigh));
	fir_design_applaywindowL(dCoeff, dWindow, iCoefNum);
	fir_design_scaleL(dCoeff, iCoefNum, dGain / testgain_float_DCL(dCoeff, iCoefNum));
}
//#endif

// Массив коэффициентов для несимметричного фильтра
static void fir_design_bandpass_freq(FLOAT_t * dCoeff, int iCoefNum, int iCutLow, int iCutHigh)
{
	fir_design_bandpass(dCoeff, iCoefNum, fir_design_normfreq(iCutLow), fir_design_normfreq(iCutHigh));
}


#if WITHDSPEXTFIR || 1

// преобразование к целым
static void fir_design_copy_integers(int32_t * lCoeff, const FLOAT_t * dCoeff, int iCoefNum, const adapter_t * ap)
{
	//const FLOAT_t scaleout = POWF(2, HARDWARE_COEFWIDTH - 1);
	int iCnt;
	const int j = NtapCoeffs(iCoefNum);
	// копируем результат.
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		//lCoeff [iCnt] = dCoeff [iCnt] * scaleout;
		lCoeff [iCnt] = adpt_output(ap, dCoeff [iCnt]);
	}
}

static void fir_design_integers_passtrough(FLOAT_t * dCoeff, int32_t *lCoeff, int iCoefNum, FLOAT_t dGain, const adapter_t * ap)
{
	fir_design_passtrough(dCoeff, iCoefNum, dGain);
	fir_design_copy_integers(lCoeff, dCoeff, iCoefNum, ap);
}

static void fir_design_integer_lowpass_scaled(FLOAT_t * dCoeff, int32_t *lCoeff, const FLOAT_t *dWindow, int iCoefNum, int iCutHigh, FLOAT_t dGain, const adapter_t * ap)
{
	fir_design_lowpass_freq_scaled(dCoeff, dWindow, iCoefNum, iCutHigh, dGain);	// с управлением крутизной скатов и нормированием усиления, с наложением окна
	fir_design_copy_integers(lCoeff, dCoeff, iCoefNum, ap);
}
//#if (__ARM_FP & 0x08) && 1

// преобразование к целым
static void fir_design_copy_integersL(int_fast32_t * lCoeff, const double * dCoeff, int iCoefNum, const adapter_t * ap)
{
	//const double scaleout = pow(2, HARDWARE_COEFWIDTH - 1);
	int iCnt;
	const int j = NtapCoeffs(iCoefNum);
	// копируем результат.
	for (iCnt = 0; iCnt < j; iCnt ++)
	{
		//lCoeff [iCnt] = dCoeff [iCnt] * scaleout;
		lCoeff [iCnt] = adpt_outputL(ap, dCoeff [iCnt]);
	}
}

static void fir_design_integer_lowpass_scaledL(double * dCoeff, int_fast32_t *lCoeff, const double *dWindow, int iCoefNum, int iCutHigh, double dGain, const adapter_t * ap)
{
	fir_design_lowpass_freq_scaledL(dCoeff, dWindow, iCoefNum, iCutHigh, dGain);	// с управлением крутизной скатов и нормированием усиления, с наложением окна
	fir_design_copy_integersL(lCoeff, dCoeff, iCoefNum, ap);
}
//#endif

#endif /* WITHDSPEXTFIR */

#if 0
// debug function
static void writecoefs(const int_fast32_t * lCoeff, int iCoefNum)
{
	PRINTF(PSTR("# iCoefNum = %i\n"), iCoefNum);
	const int iHalfLen = (iCoefNum - 1) / 2;
	int i;
	for (i = 0; i <= iHalfLen; ++ i)
	{
		PRINTF(PSTR("%ld\n"), lCoeff [i]);
	}
	i -= 1;
	for (; -- i >= 0; )
	{
		PRINTF(PSTR("%ld\n"), lCoeff [i]);
	}
	PRINTF(PSTR("# end\n"));
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
// Фильтр квадратурных каналов приёмника A
// Используется в случае внешнего DDCV
static RAMFUNC_NONILINE FLOAT32P_t filter_firp_rx_SSB_IQ_A(FLOAT32P_t NewSample)
{
	const FLOAT_t * const k = FIRCoef_rx_SSB_IQ [gwprof];
	enum { Ntap = Ntap_rx_SSB_IQ, NtapHalf = Ntap / 2 };
	// буфер с сохранёнными значениями сэмплов
	static RAMDTCM FLOAT32P_t x [Ntap * 2];
	static RAMDTCM uint_fast16_t fir_head = 0;

	// shift the old samples
	// fir_head -  Начало обрабатываемой части буфера
	// fir_head + Ntap -  Позиция за концом обрабатываемого буфера
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

// Фильтр квадратурных каналов приёмника B
// Используется в случае внешнего DDCV
static RAMFUNC_NONILINE FLOAT32P_t filter_firp_rx_SSB_IQ_B(FLOAT32P_t NewSample)
{
	const FLOAT_t * const k = FIRCoef_rx_SSB_IQ [gwprof];
	enum { Ntap = Ntap_rx_SSB_IQ, NtapHalf = Ntap / 2 };
	// буфер с сохранёнными значениями сэмплов
	static RAMDTCM FLOAT32P_t x [Ntap * 2];
	static RAMDTCM uint_fast16_t fir_head = 0;

	// shift the old samples
	// fir_head -  Начало обрабатываемой части буфера
	// fir_head + Ntap -  Позиция за концом обрабатываемого буфера
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

	// буфер с сохранёнными значениями сэмплов
	static RAMDTCM FLOAT32P_t x [Ntap * 2]; // input samples (force CCM allocation)
	static RAMDTCM uint_fast16_t fir_head = 0;		// позиция записи в буфер в последний раз

	// shift the old samples
	// fir_head -  Начало обрабатываемой части буфера
	// fir_head + Ntap -  Позиция за концом обрабатываемого буфера
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
	// буфер с сохранёнными значениями сэмплов
	static RAMDTCM FLOAT32P_t x [Ntap * 2]; // input samples (force CCM allocation)
	static RAMDTCM uint_fast16_t fir_head = 0;

	// shift the old samples
	// fir_head -  Начало обрабатываемой части буфера
	// fir_head + Ntap -  Позиция за концом обрабатываемого буфера
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

#if WITHDSPLOCALTXFIR || WITHDSPLOCALFIR
// Фильтр квадратурных каналов передатчика
// Используется в случае внешнего DUC
static RAMFUNC_NONILINE FLOAT32P_t filter_firp_tx_SSB_IQ(FLOAT32P_t NewSample)
{
	const FLOAT_t * const k = FIRCoef_tx_SSB_IQ [gwprof];
	enum { Ntap = Ntap_tx_SSB_IQ, NtapHalf = Ntap / 2 };
	// буфер с сохранёнными значениями сэмплов
	static RAMDTCM FLOAT32P_t x [Ntap * 2];
	static RAMDTCM uint_fast16_t fir_head = 0;

	// shift the old samples
	// fir_head -  Начало обрабатываемой части буфера
	// fir_head + Ntap -  Позиция за концом обрабатываемого буфера
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

#endif /* WITHDSPLOCALTXFIR */

#if WITHSKIPUSERMODE

/* Выполнение обработки в симметричном FIR фильтре */
static RAMFUNC FLOAT_t filter_fir_compute(const FLOAT_t * const pk0, const FLOAT_t * xbh, uint_fast16_t n)
{
	const FLOAT_t * xbt = xbh;		// позиция справа от центра
    // Calculate the new output
	// Выборка в середине буфера
	FLOAT_t v = pk0 [-- n] * * -- xbh;             // sample at middle of buffer
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24 && 0
	float32x4_t v4 = vdupq_n_f32(0);
#endif /* __ARM_NEON_FP */
	do
	{	
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24 && 0
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
#if __ARM_NEON_FP && DSP_FLOAT_BITSMANTISSA == 24 && 0
	v += vgetq_lane_f32(v4, 0);
	v += vgetq_lane_f32(v4, 1);
	v += vgetq_lane_f32(v4, 2);
	v += vgetq_lane_f32(v4, 3);
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

#endif /* WITHSKIPUSERMODE */

enum { tx_MIKE_blockSize = DMABUFFSIZE32RX / DMABUFFSTEP32RX };	/* В заваисимости от того, из обработчика какого прерывания вызывается dsp_processtx */
//enum { tx_MIKE_blockSize = DMABUFFSIZE32TX / DMABUFFSTEP32TX };	/* В заваисимости от того, из обработчика какого прерывания вызывается dsp_processtx */
//enum { tx_MIKE_blockSize = DMABUFFSIZE16RX / DMABUFFSTEP16RX };	/* В заваисимости от того, из обработчика какого прерывания вызывается dsp_processtx */

static FLOAT_t tx_firEQcoeff [Ntap_tx_MIKE];
static ARM_MORPH(arm_fir_instance) tx_fir_instance;
static FLOAT_t tx_fir_state [tx_MIKE_blockSize - 1 + Ntap_tx_MIKE];

#if WITHSKIPUSERMODE

// Звуковой фильтр приёмника.
// фильтрация пар значений разными фильтрами
static RAMFUNC_NONILINE FLOAT32P_t filter_fir_rx_AUDIO_Pair2(FLOAT32P_t NewSample)
{
	enum { Ntap = Ntap_rx_AUDIO, NtapHalf = Ntap / 2 };
	// буфер с сохранёнными значениями сэмплов
	static RAMDTCM FLOAT32P_t xshift [Ntap * 2];
	static RAMDTCM uint_fast16_t fir_head;

	// shift the old samples
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
    xshift [fir_head] = xshift [fir_head + Ntap] = NewSample;

	return filter_fir_compute_Pair2(FIRCoef_rx_AUDIO [gwprof] [0], FIRCoef_rx_AUDIO [gwprof] [1], & xshift [fir_head + NtapHalf + 1], NtapHalf + 1);
}

// Звуковой фильтр приёмника.
static RAMFUNC_NONILINE FLOAT_t filter_fir_rx_AUDIO_A(FLOAT_t NewSample)
{
	enum { Ntap = Ntap_rx_AUDIO, NtapHalf = Ntap / 2 };
	// буфер с сохранёнными значениями сэмплов
	static RAMDTCM FLOAT_t xshift [Ntap * 2];
	static RAMDTCM uint_fast16_t fir_head;

	// shift the old samples
	fir_head = (fir_head == 0) ? (Ntap - 1) : (fir_head - 1);
    xshift [fir_head] = xshift [fir_head + Ntap] = NewSample;

	return filter_fir_compute(FIRCoef_rx_AUDIO [gwprof] [0], & xshift [fir_head + NtapHalf + 1], NtapHalf + 1);
}

#endif /* WITHSKIPUSERMODE */

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

static int_fast16_t audio_validatebw6(int_fast16_t n)
{
	const int_fast16_t bw6limit = ARMSAIRATE - 2;
	return (n == INT16_MAX || n < bw6limit) ? n : INT16_MAX;
}

// Установка параметров тракта приёмника
static void audio_setup_wiver(const uint_fast8_t spf, const uint_fast8_t pathi)
{
#if 1//WITHDSPEXTDDC && WITHDSPEXTFIR
	static int32_t FIRCoef_trxi_IQ [NtapCoeffs(Ntap_trxi_IQ)];
	static FLOAT_t dCoeff_trx_IQ [NtapCoeffs(Ntap_trxi_IQ)];	/* Use GCC extension */
#endif /* WITHDSPEXTDDC && WITHDSPEXTFIR */

	const uint_fast8_t dspmode = glob_dspmodes [pathi];
	const uint_fast16_t fullbw6 = audio_validatebw6(glob_fullbw6 [pathi]);
	const adapter_t * const ap = & fpgafircoefsout;			/* к какому типу надо прербразовывать */

#if WITHDSPEXTDDC
	#if WITHDSPLOCALFIR
		const FLOAT_t rxfiltergain = 1;
		const FLOAT_t txfiltergain = 1;
	#endif /* WITHDSPLOCALFIR */
#else /* WITHDSPEXTDDC */
	const FLOAT_t rxfiltergain = 2;	// Для IQ фильтра можно так - для компенсации 0.5 усиления из-за перемножителя перед ним.
	const FLOAT_t txfiltergain = 2;	// Для IQ фильтра можно так - для компенсации 0.5 усиления из-за перемножителя перед ним.
#endif /* WITHDSPEXTDDC */

//	PRINTF(PSTR("audio_setup_wiver: fullbw6[%u]=%u\n"), (unsigned) pathi, (unsigned) fullbw6);

	if (fullbw6 == INT16_MAX)
	{
		//PRINTF(PSTR("audio_setup_wiver: construct bypass glob_fullbw6=%u\n"), (unsigned) glob_fullbw6);
	#if WITHDSPLOCALFIR
		if (isdspmoderx(dspmode))
			fir_design_passtrough(FIRCoef_rx_SSB_IQ [spf], Ntap_rx_SSB_IQ, rxfiltergain);
		else if (isdspmodetx(dspmode))
			fir_design_passtrough(FIRCoef_tx_SSB_IQ [spf], Ntap_tx_SSB_IQ, txfiltergain);
	#else /* WITHDSPLOCALFIR */
		(void) dspmode;
		fir_design_integers_passtrough(dCoeff_trx_IQ, FIRCoef_trxi_IQ, Ntap_trxi_IQ, 1, ap);
	#endif /* WITHDSPLOCALFIR */
#if WITHDSPLOCALTXFIR
		if (isdspmodetx(dspmode))
			fir_design_passtrough(FIRCoef_tx_SSB_IQ [spf], Ntap_tx_SSB_IQ, 1);
#endif /* WITHDSPLOCALTXFIR */
	}
	else
	{
		const int cutfreq = fullbw6 / 2;
		//PRINTF(PSTR("audio_setup_wiver: construct filter glob_fullbw6=%u\n"), (unsigned) glob_fullbw6);
	#if WITHDSPLOCALFIR
		if (isdspmoderx(dspmode))
		{
			if (1)
				fir_design_lowpass_freq_scaled(FIRCoef_rx_SSB_IQ [spf], FIRCwnd_rx_SSB_IQ, Ntap_rx_SSB_IQ, cutfreq, rxfiltergain);	// с управлением крутизной скатов и нормированием усиления, с наложением окна
			else
				{
					FLOAT_t dGain = 1;
					FLOAT_t * dCoeff = FIRCoef_rx_SSB_IQ [spf];
					int iCoefNum = Ntap_rx_SSB_IQ;
					fir_design_lowpass(dCoeff, iCoefNum, fir_design_normfreq(cutfreq));
					if (dspmode == DSPCTL_MODE_RX_AM)
						fir_design_adjust_rx(FIRCoef_rx_SSB_IQ [spf], FIRCwnd_rx_SSB_IQ, Ntap_rx_SSB_IQ, 0, GAIN_1);	// Формирование наклона АЧХ
					fir_design_applaywindow(dCoeff, FIRCwnd_rx_SSB_IQ, iCoefNum);
					fir_design_scale(dCoeff, iCoefNum, dGain / testgain_float_DC(dCoeff, iCoefNum));
				}
		}
		else if (isdspmodetx(dspmode))
			fir_design_lowpass_freq_scaled(FIRCoef_tx_SSB_IQ [spf], FIRCwnd_tx_SSB_IQ, Ntap_tx_SSB_IQ, cutfreq, txfiltergain);	// с управлением крутизной скатов и нормированием усиления, с наложением окна

	#else /* WITHDSPLOCALFIR */
	#if WITHDSPLOCALTXFIR
			if (isdspmodetx(dspmode))
				fir_design_lowpass_freq_scaled(FIRCoef_tx_SSB_IQ [spf], FIRCwnd_tx_SSB_IQ, Ntap_tx_SSB_IQ, cutfreq, 1);	// с управлением крутизной скатов и нормированием усиления, с наложением окна
	#endif /* WITHDSPLOCALTXFIR */

		(void) dspmode;
		#if WITHDOUBLEFIRCOEFS && (__ARM_FP & 0x08)

		if (1)
			fir_design_integer_lowpass_scaledL(dCoeff_trx_IQ, FIRCoef_trxi_IQ, FIRCwndL_trxi_IQ, Ntap_trxi_IQ, cutfreq, 1, & fpgafircoefsout, ap);
		else
		{
			const int iCoefNum = Ntap_trxi_IQ;
			const double * const dWindow = FIRCwndL_trxi_IQ;
			double * const dCoeff = dCoeff_trx_IQ;
			//fir_design_lowpass_freq_scaledL(dCoeff, dWindow, iCoefNum, iCutHigh, dGain);	// с управлением крутизной скатов и нормированием усиления, с наложением окна
			{
				fir_design_lowpassL(dCoeff, iCoefNum, fir_design_normfreqL(cutfreq));
				if (dspmode == DSPCTL_MODE_RX_AM)
					fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 0, GAIN_1);	// Формирование наклона АЧХ
				fir_design_applaywindowL(dCoeff, dWindow, iCoefNum);
				fir_design_scaleL(dCoeff, iCoefNum, 1 / testgain_float_DCL(dCoeff, iCoefNum));
			}
			fir_design_copy_integersL(FIRCoef_trxi_IQ, dCoeff, iCoefNum, ap);
		}

		#else /* WITHDOUBLEFIRCOEFS && (__ARM_FP & 0x08) */

		if (1)
			fir_design_integer_lowpass_scaled(dCoeff_trx_IQ, FIRCoef_trxi_IQ, FIRCwnd_trxi_IQ, Ntap_trxi_IQ, cutfreq, 1, & fpgafircoefsout);
		else
		{
			const int iCoefNum = Ntap_trxi_IQ;
			const FLOAT_t * const dWindow = FIRCwnd_trxi_IQ;
			FLOAT_t * const dCoeff = dCoeff_trx_IQ;
			//fir_design_lowpass_freq_scaled(dCoeff, dWindow, iCoefNum, iCutHigh, dGain);	// с управлением крутизной скатов и нормированием усиления, с наложением окна
			{
				fir_design_lowpass(dCoeff, iCoefNum, fir_design_normfreq(cutfreq));
				if (dspmode == DSPCTL_MODE_RX_AM)
					fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 0, GAIN_1);	// Формирование наклона АЧХ
				fir_design_applaywindow(dCoeff, dWindow, iCoefNum);
				fir_design_scale(dCoeff, iCoefNum, 1 / testgain_float_DC(dCoeff, iCoefNum));
			}
			fir_design_copy_integers(FIRCoef_trxi_IQ, dCoeff, iCoefNum, ap);
		}

		#endif /* WITHDOUBLEFIRCOEFS && (__ARM_FP & 0x08) */

	#endif /* WITHDSPLOCALFIR */
	}

#if WITHDSPEXTFIR && WITHDSPLOCALFIR
	// если есть и внешний и внутренний фильтр - внешний перводится в режим passtrough - для тестирования
	fir_design_integers_passtrough(dCoeff_trx_IQ, FIRCoef_trxi_IQ, Ntap_trxi_IQ, 1, ap);
#endif /* WITHDSPEXTDDC && WITHDSPLOCALFIR */

	// Диагностика
#if 0
	{
		unsigned i;
		unsigned n = Ntap_trxi_IQ / 2 + 1;
		for (i = 0; i < n; ++ i)
		{
			PRINTF("%-4u: k=%d, kf=%f, conv=%d\n", i, FIRCoef_trxi_IQ [i], dCoeff_trx_IQ [i], adpt_output(& fpgafircoefsout, dCoeff_trx_IQ [i]));
			ASSERT(FIRCoef_trxi_IQ [i] == adpt_output(& fpgafircoefsout, dCoeff_trx_IQ [i]));
		}
		PRINTF("FIR Filter Ntap=%u gain=%f\n", Ntap_trxi_IQ, testgain_float_DC(dCoeff_trx_IQ, Ntap_trxi_IQ));
	}
#endif

#if WITHDSPEXTDDC && WITHDSPEXTFIR
	// загрузка коэффициентов фильтра в FPGA
	//writecoefs(FIRCoef_trxi_IQ, Ntap_trxi_IQ);	/* печать коэффициентов фильтра */
	board_reload_fir(pathi, FIRCoef_trxi_IQ, dCoeff_trx_IQ, Ntap_trxi_IQ, HARDWARE_COEFWIDTH);
#endif /* WITHDSPEXTDDC && WITHDSPEXTFIR */
}

// Duplicate symmetrical part of coeffs.
static void fir_expand_symmetric(FLOAT_t * dCoeff, int Ntap)
{
	ASSERT((Ntap % 2) == 1);
	const int half = Ntap / 2;
	int i;
	for (i = 1; i <= half; ++ i)
	{
		dCoeff [half + i] = dCoeff [half - i];
	}
}

// Duplicate symmetrical part of dCoeffSrc.
static void fir_expand_symmetric2(FLOAT_t * dCoeff, const FLOAT_t * dCoeffSrc, int Ntap)
{
	ASSERT((Ntap % 2) == 1);
	const int half = Ntap / 2;
	int i;
	for (i = 1; i <= half; ++ i)
	{
		dCoeff [half + i] = dCoeffSrc [half - i];
		dCoeff [half - i] = dCoeffSrc [half - i];
	}
	dCoeff [half] = dCoeffSrc [half];
}

// Установка параметров тракта передатчика
static void audio_setup_mike(const uint_fast8_t spf)
{
	FLOAT_t * const dCoeff = FIRCoef_tx_MIKE [spf];
	const FLOAT_t * const dWindow = FIRCwnd_tx_MIKE;
	enum { iCoefNum = Ntap_tx_MIKE };

	switch (glob_dspmodes [0])	// 0 - для передатчика
	{
	case DSPCTL_MODE_TX_BPSK:
		// 15-ти герцовый ФНЧ перед модулятором - из "канонического" описания модуляции в статье в QEX July/Aug 1999 7 - x9907003.pdf
		fir_design_lowpass_freq(dCoeff, iCoefNum, 15);
		fir_design_scale(dCoeff, iCoefNum, 1 / testgain_float_DC(dCoeff, iCoefNum));	// Масштабирование для несимметричного фильтра
		fir_design_applaywindow(dCoeff, dWindow, iCoefNum);
		break;

	// Голосовые режимиы
	case DSPCTL_MODE_TX_ISB:
	case DSPCTL_MODE_RX_ISB:
	case DSPCTL_MODE_RX_WIDE:
	case DSPCTL_MODE_TX_NFM:
	case DSPCTL_MODE_TX_DIGI:
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

	/* подготовка для CMSIS FIR фильтра передатчика */
	ASSERT(Ntap_tx_MIKE == iCoefNum);
	fir_expand_symmetric2(tx_firEQcoeff, dCoeff, iCoefNum);	// Duplicate symmetrical part of coeffs.
}

// установить частоты среза тракта ПЧ
// Вызывается из пользовательской программы, но может быть вызвана и до инициализации DSP - вызывается из updateboard.
static void audio_update(const uint_fast8_t spf, uint_fast8_t pathi)
{
	globDSPMode  [spf] [pathi] = glob_dspmodes [pathi];

	// второй фильтр грузится только в режиме приёма (обеспечиватся внешним циклом).
	audio_setup_wiver(spf, pathi);	/* Установка параметров ФНЧ в тракте обработки сигнала алгоритм Уивера */

	const ncoftw_t lo6_ftw = FTWAF(- glob_lo6 [pathi]);
	nco_setlo_ftw(lo6_ftw, pathi);
#if WITHSKIPUSERMODE
	dsp_recalceq_coeffs_rx_AUDIO(pathi, FIRCoef_rx_AUDIO [spf] [pathi]);	// calculate coefficients
#endif /* WITHSKIPUSERMODE */
	debug_cleardtmax();		// сброс максимального значения в тесте производительности DSP

#if 0
	PRINTF("audio_update tx=%d [pathi=%d]: dsp_mode=%d, bw6=%d, lo6=%d, rx=%d..%d, tx=%d..%d\n",
		(int) isdspmodetx(glob_dspmodes [pathi]),
		(int) pathi, 
		(int) glob_dspmodes [pathi], 
		(int) glob_fullbw6 [pathi], 
		(int) glob_lo6 [pathi], 
		glob_aflowcutrx [pathi], glob_afhighcutrx [pathi], 
		glob_aflowcuttx, glob_afhighcuttx
		);
#endif
}

// calculate 1/2 of coefficients
static void dsp_recalceq_coeffs_half(uint_fast8_t pathi, FLOAT_t * dCoeff, const FLOAT_t * dWindow, int iCoefNum)
{
	const int cutfreqlow = glob_aflowcutrx [pathi];
	const int cutfreqhigh = glob_afhighcutrx [pathi];

	ASSERT((iCoefNum % 2) == 1);
	switch (glob_dspmodes [pathi])
	{
	case DSPCTL_MODE_RX_DSB:
		// ФНЧ
		fir_design_lowpass_freq(dCoeff, iCoefNum, cutfreqhigh);
		fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 1, GAIN_1);	// Формирование наклона АЧХ
		break;

	case DSPCTL_MODE_RX_SAM:
		// ФНЧ
		//fir_design_lowpass_freq(dCoeff, iCoefNum, cutfreqhigh);
		fir_design_bandpass_freq(dCoeff, iCoefNum, cutfreqlow, cutfreqhigh);
		fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 1, GAIN_1);	// Формирование наклона АЧХ
		break;

	case DSPCTL_MODE_RX_WFM:
	case DSPCTL_MODE_RX_AM:
	case DSPCTL_MODE_RX_WIDE:
	case DSPCTL_MODE_TX_SSB:
	case DSPCTL_MODE_RX_ISB:
		// audio
		if (glob_notch_mode == BOARD_NOTCH_MANUAL)
		{
			// частоты SSB фильтра
			//const int fssbL = cutfreqlow;
			//const int fssbH = cutfreqhigh;

			// Частоты NOTCH фильтра
			const int fNotch = inside(cutfreqlow + glob_notch_width / 2, glob_notch_freq, cutfreqhigh - glob_notch_width / 2);
			const int fcutL = fNotch - glob_notch_width / 2;
			const int fcutH = fNotch + glob_notch_width / 2;
			if (1)
			{
				// Расчитывается Notch
				fir_design_bandstop(dCoeff, iCoefNum, fir_design_normfreq(fcutL), fir_design_normfreq(fcutH));
				fir_design_scale(dCoeff, iCoefNum, 1 / testgain_float_DC(dCoeff, iCoefNum));	// Масштабирование для несимметричного фильтра
				fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 0, GAIN_1);	// Формирование наклона АЧХ, без применения оконной функции
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
				fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 0, GAIN_1);	// Формирование наклона АЧХ, без применения оконной функции
			}
		}
		else
		{
			fir_design_bandpass_freq(dCoeff, iCoefNum, cutfreqlow, cutfreqhigh);
			fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 1, GAIN_1);	// Формирование наклона АЧХ
		}
		break;

	case DSPCTL_MODE_TX_CW:
	case DSPCTL_MODE_RX_NARROW:
	case DSPCTL_MODE_RX_FREEDV:
		// audio
		fir_design_bandpass_freq(dCoeff, iCoefNum, cutfreqlow, cutfreqhigh);
		fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 1, GAIN_1);	// Формирование наклона АЧХ
		break;

	case DSPCTL_MODE_RX_DRM:
		// audio
		// В этом режиме фильтр не используется
		fir_design_passtrough(dCoeff, iCoefNum, 1);		// сигнал через НЧ фильтр не проходит
		break;


	case DSPCTL_MODE_RX_NFM:
		// audio
		fir_design_bandpass_freq(dCoeff, iCoefNum, cutfreqlow, cutfreqhigh);
		fir_design_adjust_rx(dCoeff, dWindow, iCoefNum, 1, (int) glob_gainnfmrx [pathi] / (FLOAT_t) 100);	// Формирование наклона АЧХ
		break;

	// в режиме передачи
	default:
		fir_design_passtrough(dCoeff, iCoefNum, 1);		// сигнал через НЧ фильтр не проходит
		break;
	}
}


// calculate full array of coefficients
void dsp_recalceq_coeffs_rx_AUDIO(uint_fast8_t pathi, FLOAT_t * dCoeff)
{
	int iCoefNum = Ntap_rx_AUDIO;
	const FLOAT_t * const dWindow = FIRCwnd_rx_AUDIO;	// Array of NtapCoeffs(Ntap_rx_AUDIO) values
	dsp_recalceq_coeffs_half(pathi, dCoeff, dWindow, iCoefNum);	// calculate 1/2 of coefficients
	fir_expand_symmetric(dCoeff, iCoefNum);	// Duplicate symmetrical part of coeffs.
}

#if WITHMODEM

// RX
// ---------

static uint_fast32_t g_TxBitFreqFTW = 0;


// Возвращает не-0 каждые 32 мс (31.25 Гц) - вызывается с частотой ARMI2SRATE
static int bpsk31_phase_tick(void)
{
	static uint_fast32_t bps31_tx_bitrateNCO;
	auto uint_fast32_t old = bps31_tx_bitrateNCO;
	return ((bps31_tx_bitrateNCO += g_TxBitFreqFTW) < old);
}

// return 0/1 for 0/PI
static 
uint_fast8_t 
pbsk_get_phase(
	uint_fast8_t suspend	// передавать модему ещё рано - не полностью завершено формирование огибающей
	)
{
	static uint_fast8_t phase;

	if (bpsk31_phase_tick() && modem_getnextbit(suspend))
	{
		phase = ! phase;
	}
	return phase;
}

static volatile uint_fast8_t	glob_modemmode;		// применяемая модуляция (bpsk/qpsk)
static volatile uint_fast32_t	glob_modemspeed100;	// скорость передачи с точностью 1/100 бод

static uint16_t m_RxBitPhase;
static uint16_t g_RxBitFreqFTW;

// демодулятор BPSK
static void demod_bpsk2_symbol(int_fast32_t i, int_fast32_t q, int level)
{
	static int_fast32_t oldi, oldq;
	const int_fast64_t dot = (int_fast64_t) oldi * i + (int_fast64_t) oldq * q;
	const int_fast8_t bitv = ! (dot > 0);
	modem_frames_decode(bitv);

	oldi = i;
	oldq = q;
}




static void demod_bpsk(int_fast32_t RxSin, int_fast32_t RxCos)
{

	enum 
	{
		BITFILTERLENGTH = 16,	// не трогать
	};

	typedef long SAMPLEHOLDER_T;
		//saved values for bit synchronization filter taps
	static RAMDTCM SAMPLEHOLDER_T m_RxAmpFil [BITFILTERLENGTH];

	{	
		const SAMPLEHOLDER_T ISum = RxSin;
		const SAMPLEHOLDER_T QSum = RxCos;

		int	level = 30;
		long ampl = (ISum >> 16) * (ISum >> 16) + (QSum >> 16) * (QSum >> 16);

		// логарифм по основнияю 2
		while (ampl > 0)
		{
			level += 1;
			ampl >>= 1;
		}

		//Select 1 of 16
		const int BitPhaseInt = m_RxBitPhase >> 12;
		ASSERT(BitPhaseInt < BITFILTERLENGTH);
		m_RxAmpFil [BitPhaseInt] = (SAMPLEHOLDER_T)level;

		int i;
		ampl = 0;
		for (i = 0; i < BITFILTERLENGTH/2; i += 1)
		{
			ampl += m_RxAmpFil[i] - m_RxAmpFil [i + (BITFILTERLENGTH/2)];
		}

		//The correction is the amplitude times a synchronization gain, which is empirical.
		const long BitPhaseCorrection = (long) (ampl * 4);

		long NextSymPhase;	//long enough to contain the 17th bit when 16 bit adds overflow
		NextSymPhase = (long)m_RxBitPhase + (long) g_RxBitFreqFTW - BitPhaseCorrection;
		m_RxBitPhase = (unsigned short)NextSymPhase;

		if (NextSymPhase > 0xFFFF)
		{
			//We're at the centre of the bit:  31.25 Hz
			demod_bpsk2_symbol(ISum, QSum, level);
		}
	}
}

/////////////////////////////
// Интерфейсная функция демодулятора
// Вызыается с частотой ARMI2SRATE герц
// iq - квадратура, полученная от радиотракта

static void modem_demod_iq(FLOAT32P_t iq)
{
	switch (glob_modemmode)
	{
	case 0:
		// BPSK
		demod_bpsk(iq.IV, iq.QV);
		break;
	case 1:
		// QPSK
		//demod_qpsk(iq.IV, iq.QV);
		break;
	}
}
// Интерфейсная функция модулятора
// Вызыается с частотой ARMI2SRATE герц
// версия для высокоскоростных модемов
static int modem_get_tx_b(
	uint_fast8_t suspend	// передавать модему ещё рано - не полностью завершено формирование огибающей
	)
{

	return pbsk_get_phase(suspend);	
}
// Интерфейсная функция модулятора
// Вызыается с частотой ARMI2SRATE герц
// версия для низкоскоростных модемов
static FLOAT32P_t modem_get_tx_iq(
	uint_fast8_t suspend	// передавать модему ещё рано - не полностью завершено формирование огибающей
	)
{
	switch (glob_modemmode)
	{
	case 0:
		// BPSK
		{
			const int_fast32_t ph = (1 - pbsk_get_phase(suspend) * 2);
			const FLOAT32P_t iq = { { ph, ph } };
			return iq;
		}
	case 1:
		// QPSK
		{
			const FLOAT32P_t iq = { { 1, 0 } };
			return iq;
		}
	default:
		{
			const FLOAT32P_t iq = { { 1, 0 } };
			return iq;
		}

	}
}


static void modem_set_tx_speed(uint_fast32_t speed100)
{
	// TX
	g_TxBitFreqFTW = ((uint_fast64_t) speed100 << 32) / (ARMI2SRATE100);
}

static void modem_set_rx_speed(uint_fast32_t speed100)
{
	// RX
	g_RxBitFreqFTW = (((uint_fast64_t) speed100 << 16) / (ARMI2SRATE100));
}


/* Установить скорость, параметр с точностью 1/100 бод */
static void 
modem_set_speed(uint_fast32_t speed100)
{
	if (glob_modemspeed100 != speed100)
	{
		glob_modemspeed100 = speed100;

		modem_set_tx_speed(speed100);
		modem_set_rx_speed(speed100);
	}
}

/* Установить модуляцию для модема */
void modem_set_mode(uint_fast8_t modemmode)
{
	if (glob_modemmode != modemmode)
	{
		glob_modemmode = modemmode;	
	}
}
#endif /* WITHMODEM */

static void modem_update(void)
{
#if WITHMODEM
	modem_set_speed(glob_modem_speed100);
	modem_set_mode(glob_modem_mode);

	PRINTF(PSTR("modem_update: modem_speed100=%d.%02d, modem_mode=%d\n"), (int) (glob_modem_speed100 / 100), (int) (glob_modem_speed100 % 100), (int) glob_modem_mode);
#endif /* WITHMODEM */
}
///////////////////////

static RAMDTCM FLOAT_t agclogof10 = 1;
	
static void agc_state_initialize(volatile agcstate_t * st, const volatile agcparams_t * agcp)
{
	LCLSPINLOCK_INITIALIZE(& st->lock);
	const FLOAT_t f0 = agcp->levelfence;
	const FLOAT_t m0 = agcp->mininput;
	const FLOAT_t siglevel = 0;
	const FLOAT_t level = FMINF(FMAXF(m0, siglevel), f0);	// работаем в диапазоне от 1 до f
	const FLOAT_t ratio = (agcp->agcoff ? m0 : level) / f0;

	const FLOAT_t streingth_log = LOGF(ratio);
	const FLOAT_t caplevel = streingth_log;

	st->agcfastcap = caplevel;
	st->agcslowcap = caplevel;
}

// TODO: eliminate LOGF
// are equal:
//gain = valelout / adjsig * powf(10.0f, log10f(adjsig / valelin) * agcfactor);
//gain = valelout / adjsig * powf(adjsig / valelin, agcfactor);

// Для работы функции agc_perform требуется siglevel, больште значения которого
// соответствуют большим уровням сигнала. может быть отрицательным
static RAMFUNC FLOAT_t agccalcstrength_log(const volatile agcparams_t * const agcp, FLOAT_t siglevel)
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
static RAMFUNC FLOAT_t agccalcgain_log(const volatile agcparams_t * const agcp, FLOAT_t streingth)
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
static RAMFUNC FLOAT_t agc_calcstrengthlog10(const volatile agcparams_t * const agcp, FLOAT_t streingth)
{
	return streingth / agclogof10;	// уже логарифмировано
}

static RAMFUNC FLOAT_t agc_getsigpower(
	FLOAT32P_t sampleiq
	)
{
	const FLOAT_t sample = (FLOAT_t) sampleiq.IV * sampleiq.IV + (FLOAT_t) sampleiq.QV * sampleiq.QV;
	return sample;
}

//
// постоянные времени системы АРУ
// Инициализация сделана для того, чтобы поместить эти переменные в обюласть CCM памяти
// Присвоение осмысленных значений производится в соответствующих функциях инициализации.

static RAMDTCM agcstate_t rxsmeterstate [NTRX];	// На каждый приёмник
static RAMDTCM agcstate_t rxagcstate [NTRX];	// На каждый приёмник
static RAMDTCM agcstate_t txagcstate;

static RAMDTCM agcparams_t rxsmeterparams;
static RAMDTCM agcparams_t rxagcparams [NPROF] [NTRX];
static RAMDTCM agcparams_t txagcparams [NPROF];

static RAMDTCM volatile uint_fast8_t gwagcprofrx = 0;	// work profile - индекс конфигурационной информации, испольуемый для работы */
static RAMDTCM volatile uint_fast8_t gwagcproftx = 0;	// work profile - индекс конфигурационной информации, испольуемый для работы */

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
			// s-meter
			agc_parameters_initialize(& rxsmeterparams);
			agc_state_initialize(& rxsmeterstate [pathi], & rxsmeterparams);
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

// АРУ вперёд для floaing-point тракта
// получение измеренного уровня сигнала
static RAMFUNC FLOAT_t agc_measure_float(
	const uint_fast8_t dspmode, 
	FLOAT_t siglevel0,
	uint_fast8_t pathi
	)
{
	//BEGIN_STAMP3();

	const agcparams_t * const agcp = & rxagcparams [gwagcprofrx] [pathi];
	agcstate_t * const st = & rxagcstate [pathi];
	BEGIN_STAMP();
	const FLOAT_t strength = agccalcstrength_log(agcp, siglevel0);	// получение логарифмического хначения уровня сигнала
	END_STAMP();

	// показ S-метра
	agc_perform(& rxsmeterparams, & rxsmeterstate [pathi], strength);	// измеритель уровня сигнала

	//BEGIN_STAMP();
	agc_perform(agcp, st, strength);	// измеритель уровня сигнала
	//END_STAMP();

	//END_STAMP3();
	return agc_result_slow(st);
}

// АРУ вперёд для floaing-point тракта
// получение усиления
static RAMFUNC FLOAT_t agc_getgain_float(
	FLOAT_t fltstrengthslow,
	uint_fast8_t pathi
	)
{
	const volatile agcparams_t * const agcp = & rxagcparams [gwagcprofrx] [pathi];

	//BEGIN_STAMP();
	const FLOAT_t gain = agccalcgain_log(agcp, fltstrengthslow);
	//END_STAMP();

	return gain;
}

static RAMDTCM FLOAT_t manualsquelch [NTRX];

static RAMFUNC int agc_squelchopen(
	FLOAT_t fltstrengthslow,
	uint_fast8_t pathi
	)
{
	return fltstrengthslow > manualsquelch [pathi];
}

// Функция для S-метра - получение десятичного логарифма уровня сигнала от FS
/* Вызывается из user-mode программы */
static void agc_reset(
	uint_fast8_t pathi
	)
{
	agcparams_t * const agcp = & rxsmeterparams;
	agcstate_t * const st = & rxsmeterstate [pathi];
	FLOAT_t m0 = agcp->mininput;
	FLOAT_t m1;

	IRQL_t oldIrql;
	RiseIrql(IRQL_REALTIME, & oldIrql);
	st->agcfastcap = m0;
	st->agcslowcap = m0;
	LowerIrql(oldIrql);

#if ! CTLSTYLE_V1D		// не Плата STM32F429I-DISCO с процессором STM32F429ZIT6 - на ней приема нет
	for (;;)
	{
		local_delay_ms(1);

		IRQL_t oldIrql;
		RiseIrql(IRQL_REALTIME, & oldIrql);
		const FLOAT_t v = agc_result_slow(st);
		LowerIrql(oldIrql);

		if (v != m0)
		{
			m1 = v;
			break;
		}
	}
	for (;;)
	{
		local_delay_ms(1);

		IRQL_t oldIrql;
		RiseIrql(IRQL_REALTIME, & oldIrql);
		const FLOAT_t v = agc_result_slow(st);
		LowerIrql(oldIrql);

		if (v != m1)
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
	agcparams_t * const agcp = & rxsmeterparams;
	agcstate_t * const st = & rxsmeterstate [pathi];

	const FLOAT_t fltstrengthfast = agc_result_fast(st);	// измеритель уровня сигнала
	const FLOAT_t fltstrengthslow = agc_result_slow(st);	// измеритель уровня сигнала
	* tracemax = agc_calcstrengthlog10(agcp, fltstrengthslow);
	FLOAT_t r = agc_calcstrengthlog10(agcp, fltstrengthfast);

	return r;
}

static int computeslevel_1(
	FLOAT_t dbFS	// десятичный логарифм уровня сигнала от FS
	)
{
	return (dbFS * 200 + (glob_fsadcpower10 + 5)) / 10;
}

static int computeslevel_10(
	FLOAT_t dbFS	// десятичный логарифм уровня сигнала от FS
	)
{
	return (dbFS * 200 + (glob_fsadcpower10 + 5));
}

/* получить значение уровня сигнала в децибелах, отступая от upper */
/* -73.01dBm == 50 uV rms == S9 */
/* Вызывается из user-mode программы */
uint_fast8_t 
dsp_getsmeter(uint_fast8_t * tracemax, uint_fast8_t lower, uint_fast8_t upper, uint_fast8_t clean)
{
	const uint_fast8_t pathi = 0;	// тракт, испольуемый для показа s-метра
	//if (clean != 0)
	//	agc_reset(pathi);
	FLOAT_t tmaxf;
	int level = upper + computeslevel_1(agc_forvard_getstreigthlog10(& tmaxf, pathi));
	int tmax = upper + computeslevel_1(tmaxf);

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
/* получить значение уровня сигнала в децибелах, отступая от upper */
/* -73.01dBm == 50 uV rms == S9 */
/* Вызывается из user-mode программы */
uint_fast16_t
dsp_getsmeter10(uint_fast16_t * tracemax, uint_fast16_t lower, uint_fast16_t upper, uint_fast8_t clean)
{
	const uint_fast8_t pathi = 0;	// тракт, испольуемый для показа s-метра
	//if (clean != 0)
	//	agc_reset(pathi);
	FLOAT_t tmaxf;
	int level = upper + computeslevel_10(agc_forvard_getstreigthlog10(& tmaxf, pathi));
	int tmax = upper + computeslevel_10(tmaxf);

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

static FLOAT_t mickecliplevelp [NPROF] = { + INT_MAX, + INT_MAX };	/* positive limit */
static FLOAT_t mickeclipleveln [NPROF] = { - INT_MAX, - INT_MAX };	/* negative limit */
static FLOAT_t mickeclipscale [NPROF] = { 1, 1 };

// ару и компрессор микрофона
// На входе уже нормированный к txlevelfenceSSB сигнал
static RAMFUNC FLOAT_t txmikeagc(FLOAT_t vi)
{
	agcparams_t * const agcp = & txagcparams [gwagcproftx];
	if (agcp->agcoff == 0)
	{
		const FLOAT_t siglevel0 = FABSF(vi);
		agcstate_t * const st = & txagcstate;

		agc_perform(agcp, st, agccalcstrength_log(agcp, siglevel0));	// измеритель уровня сигнала
		const FLOAT_t gain = agccalcgain_log(agcp, agc_result_slow(st));
		vi *= gain;
	}
	return vi;
}

static RAMFUNC FLOAT_t txmikeclip(FLOAT_t vi)
{
	// Ограничитель
	const FLOAT_t levelp = mickecliplevelp [gwagcproftx];
	const FLOAT_t leveln = mickeclipleveln [gwagcproftx];
	if (vi > levelp)
		vi = levelp;
	else if (vi < leveln)
		vi = leveln;
	vi *= mickeclipscale [gwagcproftx];

	return vi;
}

/* получения признака переполнения АЦП микрофонного тракта - вызывается из user mode */
uint_fast8_t dsp_getmikeadcoverflow(void)
{
	volatile agcstate_t * const st = & txagcstate;
	const FLOAT_t FS = txagcparams [gwagcproftx].levelfence;	// txlevelfenceSSB
	return st->agcslowcap >= FS * db2ratio((FLOAT_t) - 1);
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
    static RAMDTCM FLOAT_t y[2 * (NCoef+1)]; //output samples
    static RAMDTCM FLOAT_t x[2 * (NCoef+1)]; //input samples
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
	if (uacoutplayer)
		return sdtn;
	return v * mainvolumerx + sdtn * sidetonevolume;
}

// sdtn, moni: значение выборки в диапазоне, допустимом для кодека
// shape: 0..1: 0 - monitor, 1 - sidetone
static FLOAT_t mixmonitor(FLOAT_t shape, FLOAT_t sdtn, FLOAT_t moni)
{
	if (uacoutplayer)
		return moni;
	return sdtn * shape + moni * glob_moniflag * (1 - shape);
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

// генератор шума для настройки
static RAMFUNC FLOAT_t get_noisefloat(void)
{
	const unsigned long middle = LONG_MAX;
	// Формирование значения выборки
	return (int) (local_random(2 * middle - 1) - middle) / (FLOAT_t) middle;
}

/* При необходимости добавить в самопрослушивание пердаваемый SSB сигнал */
static void monimux(
	uint_fast8_t dspmode,
	FLOAT32P_t * moni,		/* здесь может быть пара сэмплов от USB */
	const FLOAT_t * ssbtx
	)
{
	if (isdspmoderx(dspmode))
		return;
	switch (dspmode)
	{
	case DSPCTL_MODE_TX_DIGI:
	case DSPCTL_MODE_TX_SSB:
	case DSPCTL_MODE_TX_AM:
	case DSPCTL_MODE_TX_NFM:
	case DSPCTL_MODE_TX_FREEDV:
#if WITHUSBHW && WITHUSBUACOUT
		if (glob_txaudio != BOARD_TXAUDIO_USB && ! istxreplaced())
		{
			moni->IV = * ssbtx;
			moni->QV = * ssbtx;
		}
#else /* WITHUSBHW && WITHUSBUACOUT */
		moni->IV = * ssbtx;
		moni->QV = * ssbtx;
#endif /* WITHUSBHW && WITHUSBUACOUT */
		break;

	default:
		break;
	}
}
// return audio sample in range [- 1.. + 1]
static RAMFUNC FLOAT_t mikeinmux(
	uint_fast8_t dspmode,
	FLOAT32P_t * moni
	)
{
	const uint_fast8_t digitx = dspmode == DSPCTL_MODE_TX_DIGI;
	const FLOAT_t txlevelXXX = digitx || istxreplaced() ? txlevelfenceDIGI : txlevelfenceSSB;
	const FLOAT32P_t vi0p = getsampmlemike2();	// с микрофона (или 0, если ещё не запустился) */
	const FLOAT32P_t viusb0f = getsampmleusb2();	// с usb (или 0, если ещё не запустился) */
	FLOAT_t vi0f = vi0p.IV;

#if WITHFT8
	ft8_txfill(& vi0f);	// todo: add new DSPCTL_FT8 mode
#endif /* WITHFT8 */

#if WITHTXCPATHCALIBRATE
	return (FLOAT_t) glob_designscale / 100;
#endif /* WITHTXCPATHCALIBRATE */

	switch (dspmode)
	{
	case DSPCTL_MODE_TX_BPSK:
		return 0;	//txlevelfenceBPSK;	// постоянная составляющая с максимальным уровнем

	case DSPCTL_MODE_TX_CW:
		return txlevelfenceCW;	// постоянная составляющая с максимальным уровнем

	case DSPCTL_MODE_TX_DIGI:
	case DSPCTL_MODE_TX_SSB:
	case DSPCTL_MODE_TX_AM:
	case DSPCTL_MODE_TX_NFM:
	case DSPCTL_MODE_TX_FREEDV:
		switch (glob_txaudio)
		{
		default:
#if WITHAFCODEC1HAVELINEINLEVEL	/* кодек имеет управление усилением с линейного входа */
			// источник - LINE IN
		case BOARD_TXAUDIO_LINE:
#endif /* WITHAFCODEC1HAVELINEINLEVEL */
		case BOARD_TXAUDIO_MIKE:
#if WITHUSBHW && WITHUSBUACOUT
			if (istxreplaced())
				goto txfromusb;
#endif /* WITHUSBHW && WITHUSBUACOUT */
			//vi0f = get_rout();		// Тест - синусоида 700 герц амплитуы (-1..+1)
			// источник - микрофон
			vi0f = txmikeagc(vi0f * txlevelXXX);	// АРУ
			vi0f = txmikeclip(vi0f);				// Ограничитель
#if WITHREVERB
			vi0f = audio_reverb_calc(vi0f);				// Ревербератор
#endif /* WITHREVERB */
#if WITHCOMPRESSOR
			vi0f = audio_compressor_calc(vi0f);		// Компрессор
#endif /* WITHCOMPRESSOR */
			moni->IV = vi0f;
			moni->QV = vi0f;
			return vi0f;

#if WITHUSBHW && WITHUSBUACOUT
		case BOARD_TXAUDIO_USB:
			txfromusb:
			// источник - USB
			moni->IV = viusb0f.IV;
			moni->QV = viusb0f.QV;
			return viusb0f.IV * txlevelXXX;

#endif /* WITHUSBHW && WITHUSBUACOUT */

		case BOARD_TXAUDIO_NOISE:
			// источник - шум
			return get_noisefloat() * txlevelXXX;	// шум

		case BOARD_TXAUDIO_2TONE:
			// источник - двухтоновый сигнал
			return get_dualtonefloat() * txlevelXXX;		// источник сигнала - двухтональный генератор для настройки

		case BOARD_TXAUDIO_1TONE:
			// источник - синусоидальный сигнал
			return get_singletonefloat() * txlevelXXX;

		case BOARD_TXAUDIO_MUTE:
			return 0;
		}

	default:
		// В режиме приёма или bypass ничего не делаем.
		if (uacoutplayer)
		{
			* moni = viusb0f;
		}
		else
		{
			moni->IV = 0;
			moni->QV = 0;
		}
		return 0;
	}
}

/* получить I/Q пару для передачи в up-converter */
static RAMFUNC FLOAT32P_t baseband_modulator(
	FLOAT_t vi,
	uint_fast8_t dspmode
	)
{
	const FLOAT_t shape = shapeTXEnvelopStep() * scaleDAC;	// 0..1 - огибающая
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
				// высокоскоростной модем. Фильтр baseband на выходе не нужен
				const int txb = modem_get_tx_b(getTxShapeNotComplete());
		#if WITHMODEMIQLOOPBACK
				const int_fast32_t ph = (1 - txb * 2);
				const FLOAT32P_t iq = { { ph, ph } };
				modem_demod_iq(iq);	// debug loopback
		#endif /* WITHMODEMIQLOOPBACK */
				const int vv = txb ? 0 : - 1;	// txiq[63] управляет инверсией сигнала переж АЦП
				const FLOAT32P_t iqr = { { vv, vv } };
				return iqr;

		}
		{
			// add done inside processafadcsampleiq
			const FLOAT32P_t vfb = scalepair(modem_get_tx_iq(getTxShapeNotComplete()), txlevelfenceBPSK * shape);
		#if WITHMODEMIQLOOPBACK
				modem_demod_iq(vfb);	// debug loopback
		#endif /* WITHMODEMIQLOOPBACK */
			return vfb;
		}
	#endif /* WITHMODEM */

	case DSPCTL_MODE_TX_CW:
	case DSPCTL_MODE_TX_DIGI:
	case DSPCTL_MODE_TX_SSB:
	case DSPCTL_MODE_TX_FREEDV:
		{
			// vi - audio sample in range [- txlevelfence.. + txlevelfence]
			const FLOAT32P_t vfb = scalepair(get_float_aflo_delta(0, pathi), vi * shape);
			return vfb;
		}
	
	case DSPCTL_MODE_TX_AM:
		{
			// vi - audio sample in range [- txlevelfenceSSB.. + txlevelfenceSSB]
			// input range: of vi: (- IFDACMAXVAL) .. (+ IFDACMAXVAL)
			const FLOAT_t peak = amcarrierHALF + vi * amshapesignalHALF;
			const FLOAT32P_t vfb = scalepair(get_float_aflo_delta(0, pathi), peak * shape);
			return vfb;
		}

	case DSPCTL_MODE_TX_NFM:
		{
			// vi - audio sample in range [- txlevelfence.. + txlevelfence]
			const long int deltaftw = (int64_t) (long) gnfmdeviationftw * vi / txlevelfenceSSB;	// Учитывается нормирование источника звука
			const FLOAT32P_t vfb = scalepair(get_float_aflo_delta(deltaftw, pathi), txlevelfenceNFM * shape);
			return vfb;
		}
	}
}

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

// Демодуляция FM (без арктангенса).
static RAMFUNC ncoftwi_t demodulator_FMnew(
	FLOAT32P_t vp1,
	const uint_fast8_t pathi,				// 0/1: main_RX/sub_RX
	FLOAT_t sigpower
	)
{
	if (vp1.IV == 0 && vp1.QV == 0)
		vp1.QV = 1;
	// Здесь, имея квадратурные сигналы vp1.IV и vp1.QV, начинаем демодуляцию
	//
	// tnx Richard Lyons
	// https://www.embedded.com/dsp-tricks-frequency-demodulation-algorithms
	//
	enum { Ntap = 3 };

	// буфер с сохранёнными значениями сэмплов
	static RAMDTCM FLOAT32P_t xs [NTRX] [Ntap * 2]; // input samples (force CCM allocation)
	static RAMDTCM uint_fast8_t fir_heads [NTRX];		// позиция записи в буфер в последний раз
	uint_fast8_t * const phead = & fir_heads [pathi];
	// * phead -  Начало обрабатываемой части буфера
	// * phead + Ntap -  Позиция за концом обрабатываемого буфера
	// shift the old samples
	* phead = (* phead == 0) ? (Ntap - 1) : (* phead - 1);
	FLOAT32P_t * const xp = & xs [pathi] [* phead];
	xp [0] = xp [Ntap] = vp1;
	const FLOAT_t qt = (xp [0].QV - xp [2].QV) * xp [1].IV;
	const FLOAT_t it = (xp [0].IV - xp [2].IV) * xp [1].QV;
	const FLOAT_t r = (qt - it) / (sigpower * 2);
	return OMEGA2FTWI(r);
}

// Демодуляция FM
static RAMFUNC ncoftwi_t demodulator_FM(
	FLOAT32P_t vp1,
	const uint_fast8_t pathi,				// 0/1: main_RX/sub_RX
	FLOAT_t sigpower
	)
{
	// Здесь, имея квадратурные сигналы vp1.IV и vp1.QV, начинаем демодуляцию
	//
	// tnx Vladimir Vassilevsky
	// http://www.dsprelated.com/showmessage/71491/2.php
	//
	static RAMDTCM ncoftwi_t prev_fi [NTRX];

	if (vp1.IV == 0 && vp1.QV == 0)
		vp1.QV = 1;

#if 1
	float32_t result;
	VERIFY(arm_atan2_f32(vp1.QV, vp1.IV, & result) == ARM_MATH_SUCCESS);
	const ncoftwi_t fi = OMEGA2FTWI(result);	//  returns a value in the range –pi to pi radians, using the signs of both parameters to determine the quadrant of the return value.
#else
	const ncoftwi_t fi = OMEGA2FTWI(ATAN2F(vp1.QV, vp1.IV));	//  returns a value in the range –pi to pi radians, using the signs of both parameters to determine the quadrant of the return value.
#endif
	const ncoftwi_t d_fi = (ncoftwi_t) (fi - prev_fi [pathi]);
	prev_fi [pathi] = fi;

	return d_fi;
}


enum { AMDSTAGES = 7, AMDOUT_IDX = (3 * AMDSTAGES) };


struct amdemod
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

static RAMDTCM struct amdemod amds [NTRX];

/* Получить информацию об ошибке настройки в режиме SAM */
/* Получить значение отклонения частоты с точностью 0.1 герца */
uint_fast8_t hamradio_get_samdelta10(int_fast32_t * p, uint_fast8_t pathi)
{
	const uint_fast32_t sample_rate10 = ARMSAIRATE * 10;

	* p = ((int_fast64_t) amds [pathi].omegai * sample_rate10) >> 32;
	return glob_dspmodes [pathi] == DSPCTL_MODE_RX_SAM;
}

static RAMDTCM volatile int32_t saved_delta_fi [NTRX];	// force CCM allocation

/* Получить значение отклонения частоты с точностью 0.1 герца для отображения на дисплее */
uint_fast8_t dsp_getfreqdelta10(int_fast32_t * p, uint_fast8_t pathi)
{
	const int_fast32_t sample_rate10 = ARMSAIRATE * 10;

	* p = ((int_fast64_t) saved_delta_fi [pathi] * sample_rate10) >> 32;
	return glob_dspmodes [pathi] == DSPCTL_MODE_RX_NFM;
}

static void init_amd(struct amdemod * a)
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
	struct amdemod * a,
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
flush_amd(struct amdemod * a)
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

	FLOAT_t audio;	// выходной сэмпл (ненормированное значение).
	FLOAT_t corr [2];
	ncoftwi_t deti;
	ncoftwi_t del_outi;
	FLOAT_t ai, bi, aq, bq;
	FLOAT_t ai_ps, bi_ps, aq_ps, bq_ps;

	struct amdemod * const a = & amds [pathi];

	const FLOAT32P_t vco0 = getsincosf(a->phsi);
	ai = vp1.IV * vco0.QV;
	bi = vp1.IV * vco0.IV;
	aq = vp1.QV * vco0.QV;
	bq = vp1.QV * vco0.IV;

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
	else
	{
		ai_ps = 0;
		bi_ps = 0;
		bq_ps = 0;
		aq_ps = 0;

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
// Возвращается сэмпл - выход детектора
// return audio sample in range [- 1 .. + 1]
static RAMFUNC_NONILINE FLOAT_t baseband_demodulator(
	FLOAT32P_t vp0f,					// Квадратурные значения выборки
	const uint_fast8_t dspmode, 
	const uint_fast8_t pathi				// 0/1: main_RX/sub_RX
	)
{
	enum { DUALRXFLT = 1 };

	FLOAT_t r;
	switch (dspmode)
	{
	default:
		// режим передачи
		{
			r = 0;	
			//r = (pathi != 0 ? get_rout() : get_lout()) * (FLOAT_t) 0.9;
		}
		break;

	case DSPCTL_MODE_RX_DSB:
	case DSPCTL_MODE_RX_WIDE:
	case DSPCTL_MODE_RX_NARROW:
	case DSPCTL_MODE_RX_DRM:
	case DSPCTL_MODE_RX_FREEDV:
		{
			// use floating point
			const FLOAT_t sigpower = agc_getsigpower(vp0f);
			const FLOAT_t fltstrengthslow = agc_measure_float(dspmode, SQRTF(sigpower), pathi);
			const FLOAT_t gain = agc_getgain_float(fltstrengthslow, pathi);
			const FLOAT32P_t vp1 = scalepair(vp0f, gain);
			const FLOAT32P_t af = get_float_aflo_delta(0, pathi);	// средняя частота выходного спектра
			r = (vp1.QV * af.QV + vp1.IV * af.IV); // переносим на выходную частоту ("+" - без инверсии).
			//r = (pathi != 0 ? get_rout() : get_lout()) * (FLOAT_t) 0.9;
			//r = af.IV * 0.9f;
			r *= agc_squelchopen(fltstrengthslow, pathi);
		}
		break;

#if WITHMODEM
	case DSPCTL_MODE_RX_BPSK:
		if (pathi == 0)
		{
			const FLOAT_t sigpower = agc_getsigpower(vp0f);
			/*const FLOAT_t fltstrengthslow = */ agc_measure_float(dspmode, SQRTF(sigpower), pathi);
			//const FLOAT_t gain = agc_getgain_float(fltstrengthslow, pathi);
			//INT32P_t vp0i32;
			//saved_delta_fi [pathi] = demodulator_FM(vp0f, pathi, sigpower);	// погрешность настройки - требуется фильтровать ФНЧ
			modem_demod_iq(vp0f);
		}
		r = 0;
		break;
#endif /* WITHMODEM */

	case DSPCTL_MODE_RX_NFM:
		if (/*DUALRXFLT || */ pathi == 0)
		{
			// Демодуляция NBFM
			const FLOAT_t sigpower = agc_getsigpower(vp0f);
			const FLOAT_t fltstrengthslow = agc_measure_float(dspmode, SQRTF(sigpower), pathi);
			//const FLOAT_t gain = agc_getgain_float(fltstrengthslow, pathi);
			saved_delta_fi [pathi] = demodulator_FM(vp0f, pathi, sigpower);	// погрешность настройки - требуется фильтровать ФНЧ
			//const int fdelta10 = ((int64_t) saved_delta_fi [pathi] * ARMSAIRATE * 10) >> 32;	// Отклнение частоты в 0.1 герц единицах
			// значение для прослушивания
			// 0.707 == M_SQRT1_2
			const FLOAT_t sample = adpt_input(& nfmdemod, saved_delta_fi [pathi]);
			r = sample * agc_squelchopen(fltstrengthslow, pathi);
		}
		else
			r = 0;
		break;

	case DSPCTL_MODE_RX_AM:
		if (DUALRXFLT || pathi == 0)
		{
			/* AM demodulation */
			// Здесь, имея квадратурные сигналы vp1.IV и vp1.QV, начинаем демодуляции
			const FLOAT_t sigpower = agc_getsigpower(vp0f);
			const FLOAT_t fltstrengthslow = agc_measure_float(dspmode, SQRTF(sigpower), pathi);
			const FLOAT_t gain = agc_getgain_float(fltstrengthslow, pathi);
			const FLOAT32P_t vp1 = scalepair(vp0f, gain);
			// Демодуляция АМ
			const FLOAT_t sample = SQRTF(vp1.IV * vp1.IV + vp1.QV * vp1.QV);// * (FLOAT_t) 0.5; //M_SQRT1_2;
			//saved_delta_fi [pathi] = demodulator_FM(vp0f, pathi, sigpower);	// погрешность настройки - требуется фильтровать ФНЧ
			r = sample * agc_squelchopen(fltstrengthslow, pathi);
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
			const FLOAT_t sigpower = agc_getsigpower(vp0f);
			const FLOAT_t fltstrengthslow = agc_measure_float(dspmode, SQRTF(sigpower), pathi);
			const FLOAT_t gain = agc_getgain_float(fltstrengthslow, pathi);
			const FLOAT32P_t vp1 = scalepair(vp0f, gain);
			//const FLOAT_t sample = SQRTF(vp1.IV * vp1.IV + vp1.QV * vp1.QV) * (FLOAT_t) 0.5; //M_SQRT1_2;
			//saved_delta_fi [pathi] = demodulator_FM(vp0f, pathi, sigpower);	// погрешность настройки - требуется фильтровать ФНЧ
			// Демодуляция SАМ
			const FLOAT_t sample = demodulator_SAM(vp1, pathi);
			r = sample * agc_squelchopen(fltstrengthslow, pathi);
		}
		else
			r = 0;
		break;
	}
	return r;
}

#if WITHDSPEXTDDC

// ПРИЁМ ISB
// Обрабатывается 32-х битная квадратура
// Возвращается сэмпл - выход детектора
// return pair of audio samples in range [- 1 .. + 1]
static FLOAT32P_t processifadcsampleIQ_ISB(
	IFADCvalue_t iv0,	// Квадратурные значения выборки
	IFADCvalue_t qv0,	// Квадратурные значения выборки
	uint_fast8_t pathi				// 0/1: main_RX/sub_RX
	)
{
	FLOAT32P_t rv = { 0 };

	return rv;
}

// ПРИЁМ остальных режимов
// Обрабатывается 32-х битная квадратура
// Возвращается сэмпл - выход детектора
// return audio sample in range [- 1 .. + 1]
static RAMFUNC FLOAT_t processifadcsampleIQ(
	IFADCvalue_t iv0,	// Квадратурные значения выборки
	IFADCvalue_t qv0,	// Квадратурные значения выборки
	uint_fast8_t dspmode,
	uint_fast8_t pathi				// 0/1: main_RX/sub_RX
	)
{
#if WITHDSPLOCALFIR
	if (isdspmoderx(dspmode))
	{
		FLOAT32P_t vp0 = { { adpt_input(& ifcodecrx, iv0), adpt_input(& ifcodecrx, qv0) } };
		// BEGIN_STAMP();

#if WITHUSEDUALWATCH
		if (pathi)
			vp0 = filter_firp_rx_SSB_IQ_B(vp0);
		else
			vp0 = filter_firp_rx_SSB_IQ_A(vp0);
#else /* WITHUSEDUALWATCH */
		vp0 = filter_firp_rx_SSB_IQ_A(vp0);
#endif /* WITHUSEDUALWATCH */

		//END_STAMP();
		return baseband_demodulator(vp0, dspmode, pathi);
	}
	else
	{
		return 0;
	}
#else /* WITHUSEDUALWATCH */
	FLOAT32P_t vp0 = { { adpt_input(& ifcodecrx, iv0), adpt_input(& ifcodecrx, qv0) } };
	return baseband_demodulator(vp0, dspmode, pathi);
#endif /* WITHDSPLOCALFIR */
}

#else /* WITHDSPEXTDDC */

// ПРИЁМ
// Обрабатывается 24-х битное число.
// Возвращается сэмпл - выход детектора
// return audio sample in range [- 1 .. + 1]
static RAMFUNC FLOAT_t processifadcsamplei(IFADCvalue_t v1, uint_fast8_t dspmode)
{
	const uint_fast8_t pathi = 0;

	if (isdspmoderx(dspmode))
	{
		// down-converter с 12 кГц на zero IF
		const FLOAT32P_t if_lo = get_float4_iflo();
		BEGIN_STAMP();
		const FLOAT32P_t vp0 = filter_fir4_rx_SSB_IQ(scalepair(if_lo, adpt_input(& ifcodecrx, v1)), if_lo.IV != 0); // частота 12 кГц - 1/4 частоты выборок АЦП - можно воспользоваться целыми значениями.
		END_STAMP();
		return baseband_demodulator(vp0, dspmode, pathi);
	}
	else
	{
		return 0;
	}
}


#endif /* WITHDSPEXTDDC */

/* получить очередной оцифрованый сэмпл с микрофона. */
static RAMFUNC FLOAT32P_t getsampmlemike2(void)
{
	FLOAT32P_t v;
#if WITHSENDWAV
	if (takewavsample(& v, getTxShapeNotComplete()) != 0)
	{
		FLOAT32P_t dummy;
		getsampmlemike(& dummy);
	}
	else
#endif /* WITHSENDWAV */
	if (getsampmlemike(& v) == 0)
	{
		v.IV = 0;
		v.QV = 0;
	}
	// VOX detector и разрядная цепь
	// Поддержка работы VOX
	const FLOAT_t vi0f = FMAXF(FABSF(v.IV), FABSF(v.QV));
	charge2(& mikeinlevel, vi0f, (mikeinlevel < vi0f) ? VOXCHARGE : VOXDISCHARGE);

	return v;
}

/* получить очередной оцифрованый сэмпл с USB AUDIO канала. */
static RAMFUNC FLOAT32P_t getsampmleusb2(void)
{
	FLOAT32P_t v;
	if (getsampmleusb(& v) == 0)
	{
		v.IV = 0;
		v.QV = 0;
	}
	// VOX detector и разрядная цепь
	// Поддержка работы DATA VOX
	const FLOAT_t vi0f = FMAXF(FABSF(v.IV), FABSF(v.QV));
	charge2(& dvoxlevel, vi0f, (dvoxlevel < vi0f) ? DVOXCHARGE : DVOXDISCHARGE);

	return v;
}

#if WITHDTMFPROCESSING

// === Определяем потребные блоки данных
// *** Буфера декодированных "блоков" TDM-каналов
// ***  (по 2 буфера на прием и передачу)


#define DTMF_STEPS              205 // Число шагов преобразования данных
                                    //  при DTMF-детекции
                                    //  (длина буфера данных одного канала приёмника для
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
	//PRINTF(PSTR("."));
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
		PRINTF("spectrum:[%8d %8d %8d %8d %8d %8d %8d %8d] ",
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
				PRINTF(PSTR("dtmf: 0x%02x\n"), digit);
			else
				PRINTF(PSTR("dtmf: %c\n"), digit < 16 ? digits [digit] : 'Z');
		}
		else
		{
			//PRINTF(PSTR("dtmf: none\n"));
		}
		dtmfbi = 0;
	}
	else
	{
		//PRINTF(PSTR(":"));
	}
}

#endif /* WITHDTMFPROCESSING */

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

// Taken from https://stackoverflow.com/questions/11930594/calculate-atan2-without-std-functions-or-c99

// Approximates atan(x) normalized to the [-1,1] range
// with a maximum error of 0.1620 degrees.
#if 0
static float normalized_atan( float x )
{
    static const uint32_t sign_mask = 0x80000000;
    static const float b = 0.596227f;

    // Extract the sign bit
    uint32_t ux_s  = sign_mask & * (uint32_t *) & x;

    // Calculate the arctangent in the first quadrant
    float bx_a = fabsf( b * x );
    float num = bx_a + x * x;
    float atan_1q = num / ( 1.f + bx_a + num );

    // Restore the sign bit
    uint32_t atan_2q = ux_s | * (uint32_t *) & atan_1q;
    return * (float *) & atan_2q;
}
#endif
// Approximates atan2(y, x) normalized to the [0,4] range
// with a maximum error of 0.1620 degrees

static float normalized_atan2( float ay, float ax )
{
	union u
	{
		uint32_t i;
		float f;
	};

	union u ux, uy;
	ux.f = ax;
	uy.f = ay;

    static const uint32_t sign_mask = 0x80000000;
    static const float b = (float) 0.596227;

    // Extract the sign bits
    uint32_t ux_s  = sign_mask & ux.i;
    uint32_t uy_s  = sign_mask & uy.i;

    // Determine the quadrant offset
    float q = (float)( ( ~ux_s & uy_s ) >> 29 | ux_s >> 30 ); 

    // Calculate the arctangent in the first quadrant
    float bxy_a = fabsf( b * ux.f * uy.f );
    float num = bxy_a + uy.f * uy.f;
    float atan_1q =  num / ( ux.f * ux.f + bxy_a + num );

    union u atan_1qs;
    atan_1qs.f = atan_1q;

    // Translate it to the proper quadrant
    union u uatan_2qs;
    uatan_2qs.i = (ux_s ^ uy_s) | atan_1qs.i;
    return q + uatan_2qs.f;
}

static FLOAT_t
demod_WFM(
	int32_t i,
	int32_t q
	)
{
	i = (i << (32 - WITHADAPTERRTS96_WIDTH - WITHADAPTERRTS96_SHIFT)) >> (32 - WITHADAPTERRTS96_WIDTH);
	q = (q << (32 - WITHADAPTERRTS96_WIDTH - WITHADAPTERRTS96_SHIFT)) >> (32 - WITHADAPTERRTS96_WIDTH);

	if (i == 0 && q == 0)
		i = 1;

	static int prev_fi = 0;

	const int fi = ((int16_t) (0x4000 * normalized_atan2(q, i)));	//  returns a value in the range –pi to pi radians, using the signs of both parameters to determine the quadrant of the return value.

	const int d_fi = (int16_t) (fi - prev_fi);
	prev_fi = fi;

	return d_fi;
}

// Сохранение сэмплов с выхода демодулятора
void savedemod_to_AF_proc(FLOAT_t left, FLOAT_t right)
{
#if 0
	// для тестирования шумоподавителя.
	//const FLOAT_t tone = get_lout() * 0.9f;
	left = get_lout();
	right = get_rout();
#endif
#if WITHSKIPUSERMODE
	#if WITHUSEDUALWATCH
		const FLOAT32P_t i = { { left, right, }, };
		const FLOAT32P_t o = filter_fir_rx_AUDIO_Pair2(i);
		deliveryfloat(& afdemodoutfloat, o.IV, o.QV);
	#else /* WITHUSEDUALWATCH */
		const FLOAT_t o = filter_fir_rx_AUDIO_A(ch0);
		deliveryfloat(& afdemodoutfloat, o, o);
	#endif /* WITHUSEDUALWATCH */
#else /* WITHSKIPUSERMODE */
		deliveryfloat(& afdemodoutfloat, left, right);
#endif /* WITHSKIPUSERMODE */
}

#if WITHDSPEXTDDC && defined (DMABUF32RXWFM0Q)
// Обработка полученного от DMA буфера с выборками или квадратурами (или двухканальный приём).
// Вызывается на ARM_REALTIME_PRIORITY уровне.
void RAMFUNC dsp_extbuffer32wfm(const int32_t * buff)
{
	const uint_fast8_t pathi = 0;
	ASSERT(buff != NULL);
	ASSERT(gwprof < NPROF);
	const uint_fast8_t dspmodeA = globDSPMode [gwprof] [pathi];
	unsigned i;

	//memcpy(dd, buff, sizeof dd);

	if (dspmodeA == DSPCTL_MODE_RX_WFM)
	{
		for (i = 0; i < DMABUFFSIZE32RX; i += DMABUFFSTEP32RX)
		{
			// Детектор
			const FLOAT_t a0 = demod_WFM(buff [i + DMABUF32RXWFM0I], buff [i + DMABUF32RXWFM0Q]);
			const FLOAT_t a1 = demod_WFM(buff [i + DMABUF32RXWFM1I], buff [i + DMABUF32RXWFM1Q]);
			const FLOAT_t a2 = demod_WFM(buff [i + DMABUF32RXWFM2I], buff [i + DMABUF32RXWFM2Q]);
			const FLOAT_t a3 = demod_WFM(buff [i + DMABUF32RXWFM3I], buff [i + DMABUF32RXWFM3Q]);

			//volatile const FLOAT_t left = get_lout();
			const FLOAT_t left = (a0 + a1 + a2 + a3) / 4;
			savedemod_to_AF_proc(left, left);

			// Измеритель уровня
			const FLOAT32P_t p0 = { { buff [i + DMABUF32RXWFM0I], buff [i + DMABUF32RXWFM0Q] } };
			const FLOAT_t l0 = SQRTF(agc_getsigpower(p0));
			const FLOAT32P_t p1 = { { buff [i + DMABUF32RXWFM1I], buff [i + DMABUF32RXWFM1Q] } };
			const FLOAT_t l1 = SQRTF(agc_getsigpower(p1));
			const FLOAT32P_t p2 = { { buff [i + DMABUF32RXWFM2I], buff [i + DMABUF32RXWFM2Q] } };
			const FLOAT_t l2 = SQRTF(agc_getsigpower(p2));
			const FLOAT32P_t p3 = { { buff [i + DMABUF32RXWFM3I], buff [i + DMABUF32RXWFM3Q] } };
			const FLOAT_t l3 = SQRTF(agc_getsigpower(p3));

			agc_measure_float(DSPCTL_MODE_RX_WFM, FMAXF(FMAXF(l0, l1), FMAXF(l2, l3)) / 2, pathi);
		}
	}
}

#endif /* WITHDSPEXTDDC */

// перед передачей по DMA в аудиокодек
//  Здесь ответвляются потоки в USB и для записи на SD CARD
// realtime level
void dsp_fillphones(unsigned nsamples)
{
	enum { L = 0, R = 1 };
	ASSERT(gwprof < NPROF);
	const uint_fast8_t dspmodeA = globDSPMode [gwprof] [0];
	const uint_fast8_t tx = isdspmodetx(dspmodeA);
	unsigned i;
	while (nsamples --)
	{
		FLOAT32P_t b;
		FLOAT32P_t voice;
		FLOAT32P_t moni;
		const FLOAT_t sdtnenvelop = shapeSidetoneStep();	// 0..1: 0 - monitor, 1 - sidetone
		const FLOAT_t sdtnv = get_float_sidetone();
		ASSERT(sdtnenvelop >= 0 && sdtnenvelop <= 1);
		ASSERT(sdtnv >= - 1 && sdtnv <= + 1);
		if (voice_get(VOICE_REC16, & voice) == 0)
		{
			voice.IV =  0;	// левый канал
			voice.QV = 0; 	// правый канал
		}
		// Использование данных.
		if (voice_get(VOICE_MONI16, & moni) == 0)
		{
			moni.IV =  0;	// левый канал
			moni.QV = 0; 	// правый канал
		}

		/* Замещаем звук из мониторинга на sidetone пропорционально огибающей */
		const FLOAT_t moniL = mixmonitor(sdtnenvelop, sdtnv, moni.IV);
		const FLOAT_t moniR = mixmonitor(sdtnenvelop, sdtnv, moni.QV);

		FLOAT_t left = voice.IV;
		FLOAT_t right = voice.QV;
		//
#if WITHWAVPLAYER
		{
			FLOAT32P_t dual;

			if (takewavsample(& dual, 0) != 0)
			{
				left = dual.IV;
				right = dual.QV;
			}
		}
#elif WITHLFM
		// Обеспечиваем прослушивание стерео
#else /*  */
		switch (glob_mainsubrxmode)
		{
		case BOARD_RXMAINSUB_A_A:
			right = left;		// Для предотвращения посылки по USB данных от неинициализированного тракта приёмника B
			break;
		case BOARD_RXMAINSUB_B_B:
			left = right;
			break;
		}
#endif /*  */
		if (tx)
		{
			left = 0;
			right = 0;
		}
		if (1 && tx)
		{
			recordsampleSD(moniL, moniR);	// Запись самоконтроля и самопрослушки
			recordsampleUAC(moniL, moniR);	// Запись самоконтроля и самопрослушки
		}
		else
		{
			const FLOAT_t recleft = injectsidetone(left, moniL);
			const FLOAT_t recright = injectsidetone(right, moniR);
			recordsampleSD(recleft, recright);	// Запись демодулированного сигнала без озвучки клавиш
			recordsampleUAC(recleft, recright);	// Запись в UAC демодулированного сигнала без озвучки клавиш
		}

#if WITHLFM
		b.ivqv [L] = left;
		b.ivqv [R] = right;
#else /* */
		switch (glob_mainsubrxmode)
		{
		default:
		case BOARD_RXMAINSUB_A_A:
			// left:A/right:A
			b.ivqv [L] = (injectsidetone(left, moniL));
			b.ivqv [R] = (injectsidetone(right, moniR));
			break;
		case BOARD_RXMAINSUB_A_B:
			// left:A/right:B
			b.ivqv [L] = (injectsidetone(left, moniL));
			b.ivqv [R] = (injectsidetone(right, moniR));
			break;
		case BOARD_RXMAINSUB_B_A:
			// left:B/right:A
			b.ivqv [L] = (injectsidetone(right, moniL));
			b.ivqv [R] = (injectsidetone(left, moniR));
			break;
		case BOARD_RXMAINSUB_B_B:
			// left:B/right:B
			b.ivqv [L] = (injectsidetone(left, moniL));
			b.ivqv [R] = (injectsidetone(right, moniR));
			break;
		case BOARD_RXMAINSUB_TWO:
			// left, right:A+B
			{
				const FLOAT_t sumv = ((FLOAT_t) left + right) / 2;
				b.ivqv [L] = (injectsidetone(sumv, moniL));
				b.ivqv [R] = (injectsidetone(sumv, moniR));
			}
			break;
		}
#endif /*  */
		elfill_dmabuffer16tx(b.IV, b.QV);
	}
}

// Тестирование - заменить приянтые квадратуры синтезированными
void inject_testsignals(IFADCvalue_t * const dbuff)
{
#ifdef DMABUF32RX0I
	static FLOAT_t simlevelRX = (FLOAT_t) 0.0000001;	// -140 dBFS
	static FLOAT_t simlevelspec = (FLOAT_t) 0.05;	// -6 dBFS
	const FLOAT_t modulation = get_modulation() > 0;
	// приёмник
	const FLOAT32P_t simval = scalepair(get_float_monofreq(), simlevelRX * modulation);	// frequency
	dbuff [DMABUF32RX0I] = adpt_output(& ifcodecrx, simval.IV);
	dbuff [DMABUF32RX0Q] = adpt_output(& ifcodecrx, simval.QV);

#if FPGAMODE_GW2A
	// панорама
	const FLOAT32P_t simval0 = scalepair(get_float_monofreq2(), simlevelspec * modulation);	// frequency2
	dbuff [DMABUF32RTS0I] = adpt_output(& ifspectrumin96, simval0.IV);
	dbuff [DMABUF32RTS0Q] = adpt_output(& ifspectrumin96, simval0.QV);

#elif WITHRTS96
	// панорама
	// previous - oldest
	const FLOAT32P_t simval0 = scalepair(get_float_monofreq2(), simlevelspec * modulation);	// frequency2
	dbuff [DMABUF32RTS0I] = adpt_output(& ifspectrumin96, simval0.IV);
	dbuff [DMABUF32RTS0Q] = adpt_output(& ifspectrumin96, simval0.QV);

	// current	- nevest
	const FLOAT32P_t simval1 = scalepair(get_float_monofreq2(), simlevelspec * modulation);	// frequency2
	dbuff [DMABUF32RTS1I] = adpt_output(& ifspectrumin96, simval1.IV);
	dbuff [DMABUF32RTS1Q] = adpt_output(& ifspectrumin96, simval1.QV);
#endif /* WITHRTS96 */

#endif
}

/* выборка tx_MIKE_blockSize семплов из источников звука и формирование потока на передатчик */
/* В заваисимости от того, из обработчика какого прерывания вызывается dsp_processtx - меняем tx_MIKE_blockSize */
RAMFUNC void dsp_processtx(unsigned nsamples)
{
	ASSERT(tx_MIKE_blockSize == nsamples);
#if ! WITHTRANSPARENTIQ
	unsigned i;
	const uint_fast8_t dspmodeA = globDSPMode [gwprof] [0];
	/* обработка передачи */
	static FLOAT_t txfirbuff [tx_MIKE_blockSize];
	static FLOAT32P_t monitorbuff [tx_MIKE_blockSize];
	/* заполнение буфера сэмплами от микрофона или USB */
	for (i = 0; i < tx_MIKE_blockSize; ++ i)
	{
		monitorbuff [i].IV = 0;
		monitorbuff [i].QV = 0;
		txfirbuff [i] = mikeinmux(dspmodeA, & monitorbuff [i]);
	}
	/* формирование АЧХ перед модулятором */
	ARM_MORPH(arm_fir)(& tx_fir_instance, txfirbuff, txfirbuff, tx_MIKE_blockSize);

	/* Передача */
	for (i = 0; i < tx_MIKE_blockSize; ++ i)
	{
		const FLOAT_t shapecwssb = shapeCWSSBEnvelopStep();
		const FLOAT_t cwssbtone = get_float_sidetonetxssb() * txlevelfenceSSB;
		const FLOAT_t ctcss = get_float_subtone() * txlevelfenceSSB;
		monimux(dspmodeA, & monitorbuff [i], & txfirbuff [i]);	/* При необходимости добавить в самопрослушивание самоконтроль ключа и пердаваемый SSB сигнал */
		savemonistereo(monitorbuff [i].IV, monitorbuff [i].QV);	/* Самопрослушивание (сигнал SSB берется после фильтра) */
		FLOAT_t v = txfirbuff [i];
		if (dspmodeA == DSPCTL_MODE_TX_SSB && glob_cwssbtx != 0)
		{
			v = v * (1 - shapecwssb) + (cwssbtone * shapecwssb);	/* Заменяем передаваемый сигнал на тон пропорционально огибающей. */
		}
		FLOAT32P_t vfb = baseband_modulator(injectsubtone(v, ctcss), dspmodeA);	// Передатчик - формирование одного сэмпла (пары I/Q).

#if WITHDSPLOCALFIR || WITHDSPLOCALTXFIR
		if (dspmodeA != DSPCTL_MODE_TX_BPSK /*&& isdspmodetx(dspmodeA)*/)
		{
			/* работа без FIR фильтра в FPGA */
			vfb = filter_firp_tx_SSB_IQ(vfb);
		}
#endif /* WITHDSPLOCALFIR */

//		vfb.IV = 0;
//		vfb.QV = 0;

#if WITHDSPEXTDDC

		elfill_dmabuffer32tx(vfb.IV, vfb.QV);	// Запись в поток к передатчику I/Q значений.

#else /* WITHDSPEXTDDC */
		const FLOAT32P_t v_if = get_float4_iflo();	// частота 12 кГц - 1/4 частоты выборок АЦП - можно воспользоваться целыми значениями.
		const FLOAT32P_t e1 = filter_fir4_tx_SSB_IQ(vfb, v_if.IV != 0);		// 1.85 kHz - фильтр имеет усиление 2.0
		const FLOAT_t r = (e1.QV * v_if.QV + e1.IV * v_if.IV);	// переносим на выходную частоту ("+" - без инверсии).
		// Интерфейс с ВЧ - одноканальный ADC/DAC
		elfill_dmabuffer32tx(r, 0);	// Запись в поток к передатчику I/Q значений.
#endif /* WITHDSPEXTDDC */

	}
#endif /* ! WITHTRANSPARENTIQ */
}


FLOAT_t rxdmaproc(uint_fast8_t pathi, IFADCvalue_t iv, IFADCvalue_t qv)
{
	ASSERT(gwprof < NPROF);
	const uint_fast8_t tx = isdspmodetx(globDSPMode [gwprof] [0]);
	const uint_fast8_t dspmode = pathi ? (tx ? DSPCTL_MODE_IDLE : globDSPMode [gwprof] [1]) : globDSPMode [gwprof] [0];
	int rxgate = getRxGate();

	/* отсрочка установки частоты lo6 на время прохождения сигнала через FPGA FIR - аосле смены частоты LO1 */
	rxgate *= !! nco_setlo6_delay(pathi, tx);

#if WITHDSPEXTDDC

		if (dspmode == DSPCTL_MODE_RX_ISB)
		{
			/* прием независимых боковых полос */
			// Обработка буфера с парами значений
			const FLOAT32P_t rv = processifadcsampleIQ_ISB(iv * rxgate, qv * rxgate, pathi);
			return 0;
		}
		else
		{
			return processifadcsampleIQ(iv * rxgate, qv * rxgate, dspmode, pathi);
		}

#else /* WITHDSPEXTDDC */

	return processifadcsamplei(iv, dspmode);

#endif /* WITHDSPEXTDDC */
}

#if 0
// Обработка полученного от DMA буфера с выборками или квадратурами (или двухканальный приём).
// Вызывается на ARM_REALTIME_PRIORITY уровне.
void RAMFUNC dsp_step32rx(const IFADCvalue_t * buff)
{
	ASSERT(buff != NULL);
	ASSERT(gwprof < NPROF);
	const uint_fast8_t dspmodeA = globDSPMode [gwprof] [0];
	const uint_fast8_t tx = isdspmodetx(dspmodeA);
#if WITHUSEDUALWATCH
	const uint_fast8_t dspmodeB = tx ? DSPCTL_MODE_IDLE : globDSPMode [gwprof] [1];
#endif /* WITHUSEDUALWATCH */
	const unsigned i = 0;
	const int rxgate = getRxGate();

	/* отсрочка установки частоты lo6 на время прохождения сигнала через FPGA FIR - аосле смены частоты LO1 */
	#if WITHUSEDUALWATCH
		nco_setlo6_delay(0, tx);
		nco_setlo6_delay(1, tx);
	#else /* WITHUSEDUALWATCH */
		nco_setlo6_delay(0, tx);
	#endif /* WITHUSEDUALWATCH */

#if WITHSUSBSPKONLY
		// тестирование в режиме USB SPEAKER

		if (isdspmodetx(dspmodeA))
		{
			//const INT32P_t dual = loopbacktestaudio(vi, dspmodeA, shapecw);
			INT32P_t dual;
			dual.IV = get_lout();		// тон 700 Hz
			dual.QV = get_rout();		// тон 500 Hz
			elfill_dmabuffer32tx(dual.IV, dual.QV);	// Запись в поток к передатчику I/Q значений.
			//savesampleout16stereo(injectsidetone(dual.IV, sdtn), injectsidetone(dual.QV, sdtn));
			recordsampleUAC(dual.IV, dual.QV);	// Запись в UAC демодулированного сигнала без озвучки клавиш
		}
		else if (isdspmoderx(dspmodeA))
		{
			//const INT32P_t dual = loopbacktestaudio(vi, dspmodeA, shapecw);
			const FLOAT32P_t dual = vi;
			elfill_dmabuffer32tx(dual.IV, dual.QV);	// Запись в поток к передатчику I/Q значений.
			//savesampleout16stereo(injectsidetone(dual.IV, sdtn), injectsidetone(dual.QV, sdtn));
			recordsampleUAC(get_lout(), get_rout());	// Запись в UAC демодулированного сигнала без озвучки клавиш
		}
		else
		{
			elfill_dmabuffer32tx(0, 0);	// кодек получает 24 бита left justified в 32-х битном числе.
			recordsampleUAC(0, 0);	// Запись в UAC демодулированного сигнала без озвучки клавиш
		}

#elif WITHTRANSPARENTIQ
		/* процессор просто поддерживает двунаправленный обмен между USB и FPGA */
	
		savesampleout96stereo(
			buff [i + DMABUF32RX0I],
			buff [i + DMABUF32RX0Q]
			);

		static FLOAT32P_t vi;
		static uint_fast8_t outupsamplecnt;
		if ((outupsamplecnt ++ & 0x01) == 0)	// в сторону передатчика идут 96 кГц фреймы
		{
			vi = getsampmlemike2();	// с микрофона (или 0, если ещё не запустился) */
		}
		elfill_dmabuffer32tx(vi.IV, vi.QV);	// Запись в поток к передатчику I/Q значений.

#elif WITHDTMFPROCESSING
		// тестирование распозначания DTMF

		INT32P_t dual;
		//dual.IV = vi.IV; //get_lout();
		dual.IV = get_lout();
		dual.QV = 0;
		processafadcsampleiq(dual, viusb, dspmodeA, shapecw, shapecwssb, ctcss, & moni);	// Передатчик - формирование одного сэмпла (пары I/Q).
		savemonistereo(moni.IV, moni.QV);
		// Тестирование распознавания DTMF
		if (dtmfbi < DTMF_STEPS)
		{
			inp_samples [dtmfbi] = vi.IV;
			++ dtmfbi;
		}
		savedemod_to_AF_proc(dual.IV, dual.QV);

#elif WITHDSPEXTDDC

	#if WITHUSBHEADSET
	// Режимы трансиверов с внешним DDC

	#elif WITHUSEDUALWATCH

		//
		// Двухканальный приёмник

		if (dspmodeA == DSPCTL_MODE_RX_ISB)
		{
			/* прием независимых боковых полос с приемника A */
			// Обработка буфера с парами значений
			const FLOAT32P_t pair = processifadcsampleIQ_ISB(
				buff [i + DMABUF32RX0I] * rxgate,
				buff [i + DMABUF32RX0Q] * rxgate,
				0	// MAIN RX
				);	
			savedemod_to_AF_proc(pair.IV, pair.QV);	/* к line output подключен модем - озвучку запрещаем */
		}
		else if (0)
		{
			// тест - обход приемной части.
			savedemod_to_AF_proc(get_lout(), get_rout());
		}
		else
		{
			// buff data layout: I main/I sub/Q main/Q sub
			const FLOAT_t rxA = processifadcsampleIQ(
				buff [i + DMABUF32RX0I] * rxgate,
				buff [i + DMABUF32RX0Q] * rxgate,
				dspmodeA,
				0	// MAIN RX
				);

			const FLOAT_t rxB = processifadcsampleIQ(
				buff [i + DMABUF32RX1I] * rxgate,
				buff [i + DMABUF32RX1Q] * rxgate,
				dspmodeB,
				1	// SUB RX
				);	
			savedemod_to_AF_proc(rxA, rxB);
		}

	#else /* WITHUSEDUALWATCH */

		// Одноканальный приёмник

		if (dspmodeA == DSPCTL_MODE_RX_WFM)
		{
#if ! WITHSAI2HW
			BEGIN_STAMP2();
			const FLOAT_t leftFiltered = 0;
			END_STAMP2();
			savedemod_to_AF_proc(leftFiltered, leftFiltered);
#endif /* ! WITHSAI2HW */
		}
		else if (dspmodeA == DSPCTL_MODE_RX_ISB)
		{
			/* прием независимых боковых полос */
			// Обработка буфера с парами значений
			const FLOAT32P_t rv = processifadcsampleIQ_ISB(
				buff [i + DMABUF32RX0I] * rxgate,
				buff [i + DMABUF32RX0Q] * rxgate,
				0	// MAIN RX
				);	
			savedemod_to_AF_proc(rv.IV, rv.QV);	/* к line output подключен модем - озвучку запрещаем */
		}
		else
		{
			// Обработка буфера с парами значений
			const FLOAT_t left = processifadcsampleIQ(
				buff [i + DMABUF32RX0I] * rxgate,
				buff [i + DMABUF32RX0Q] * rxgate,
				dspmodeA,
				0		// MAIN RX
				);	
			savedemod_to_AF_proc(left, left);
		}

	#endif /*  WITHUSEDUALWATCH */

#else /* WITHDSPEXTDDC */
	const FLOAT_t left = processifadcsamplei(buff [i + DMABUF32RX] * rxgate, dspmodeA);	// Расширяем 24-х битные числа до 32 бит
	savedemod_to_AF_proc(left, left);

#endif /* WITHDSPEXTDDC */
}

#endif

//////////////////////////////////////////
// glob_cwedgetime - длительность нарастания/спада огибающей CW (и сигнала самоконтроля) в единицах милисекунд

static volatile unsigned enveloplen0 = NSAITICKS(5) + 1;	/* Изменяется через меню. */
static unsigned shapeSidetonePos = 0;
static volatile uint_fast8_t shapeSidetoneInpit = 0;
static volatile uint_fast8_t shapeCWSSBSidetoneInpit = 0;

static unsigned shapeTXEnvelopPos = 0;
static unsigned shapeCWSSBEnvelopPos = 0;
static volatile uint_fast8_t txgateInput = 0;
static volatile uint_fast8_t rxgateflag = 0;

// 0..1
static RAMFUNC FLOAT_t peakshapef(unsigned shapePos)	/* shapePos: от 0 до enveloplen0 включительно. */
{
	const q31_t halfcircle = INT32_MAX / 2;
	// The Q31 input value is in the range [0 +0.9999] and is mapped to a radian value in the range [0 2*PI).
	const q31_t cosv = arm_cos_q31((int_fast64_t) shapePos * halfcircle / enveloplen0);
	const FLOAT_t v = ((FLOAT_t) 1 - FAST_Q31_2_FLOAT(cosv)) * (FLOAT_t) 0.5;	// todo: use arm_q31_to_float
	return v;
}

// Формирование огибающей для самоконтрола
// 0..1
static RAMFUNC FLOAT_t shapeSidetoneStep(void)
{
	const unsigned enveloplen = enveloplen0;
	/* при регулировке длительности нарастания/спада из меню текущая позиция не корректируется */
	if (shapeSidetonePos >= enveloplen)
		shapeSidetonePos = enveloplen;

	const FLOAT_t v = peakshapef(shapeSidetonePos);
	
	if (shapeSidetoneInpit != 0)
		shapeSidetonePos = shapeSidetonePos >= enveloplen ? enveloplen : (shapeSidetonePos + 1);
	else
		shapeSidetonePos = shapeSidetonePos == 0 ? 0 : (shapeSidetonePos - 1);
	return v;
}

// Формирование огибающей для передачи
// 0..1
static RAMFUNC FLOAT_t shapeTXEnvelopStep(void)
{
	const unsigned enveloplen = enveloplen0;
	/* при регулировке длительности нарастания/спада из меню текущая позиция не корректируется */
	if (shapeTXEnvelopPos >= enveloplen)
		shapeTXEnvelopPos = enveloplen;
	const FLOAT_t v = peakshapef(shapeTXEnvelopPos);

	if (txgateInput != 0)
		shapeTXEnvelopPos = shapeTXEnvelopPos >= enveloplen ? enveloplen : (shapeTXEnvelopPos + 1);
	else
		shapeTXEnvelopPos = shapeTXEnvelopPos == 0 ? 0 : (shapeTXEnvelopPos - 1);
	return v;
}

// Формирование огибающей для передачи
// 0..1
static RAMFUNC FLOAT_t shapeCWSSBEnvelopStep(void)
{
	const unsigned enveloplen = enveloplen0;
	/* при регулировке длительности нарастания/спада из меню текущая позиция не корректируется */
	if (shapeCWSSBEnvelopPos >= enveloplen)
		shapeCWSSBEnvelopPos = enveloplen;
	const FLOAT_t v = peakshapef(shapeCWSSBEnvelopPos);

	if (shapeCWSSBSidetoneInpit != 0)
		shapeCWSSBEnvelopPos = shapeCWSSBEnvelopPos >= enveloplen ? enveloplen : (shapeCWSSBEnvelopPos + 1);
	else
		shapeCWSSBEnvelopPos = shapeCWSSBEnvelopPos == 0 ? 0 : (shapeCWSSBEnvelopPos - 1);
	return v;
}

// Возврат признака того, что передавать модему ещё рано - не полностью завершено формирование огибающей
static RAMFUNC uint_fast8_t getTxShapeNotComplete(void)
{
	const unsigned enveloplen = enveloplen0;
	/* при регулировке длительности нарастания/спада из меню текущая позиция не корректируется */
	if (shapeTXEnvelopPos >= enveloplen)
		shapeTXEnvelopPos = enveloplen;
	return shapeTXEnvelopPos != enveloplen;
}

/* разрешение работы тракта в режиме приёма */
static uint_fast8_t getRxGate(void)
{
#if TXGFV_RX != 0
	return rxgateflag;
#else
	return ! txgateInput;
#endif
}


// манипуляция и переключение на передачу
// вызывается из SYSTEM обработчика прерываний
void dsp_txpath_set(portholder_t txpathstate, uint_fast8_t keydown)
{
#if WITHINTEGRATEDDSP
	#if WITHTX
		txgateInput = (txpathstate & (TXGFV_TX_CW | TXGFV_TX_SSB | TXGFV_TX_AM | TXGFV_TX_NFM)) != 0;
		shapeCWSSBSidetoneInpit = keydown;
	#if TXGFV_RX != 0
		rxgateflag = (txpathstate & (TXGFV_RX)) != 0;
	#endif /* TXGFV_RX != 0 */
	#else /* WITHTX */
		rxgateflag = 1;
	#endif /* WITHTX */
#endif /* WITHINTEGRATEDDSP */
//	PRINTF("%u", shapeCWSSBSidetoneInpit);
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

void dsp_sidetone_setfreq(uint_least16_t tonefreq01)	/* tonefreq01 - частота в десятых долях герца. . */
{
	anglestep_sidetonetxssb = FTWAF001(tonefreq01 * 10u);	// В сотых долях герца


}
// return code: prescaler
uint_fast8_t
hardware_calc_sound_params(
	uint_least16_t tonefreq01,	/* tonefreq01 - частота в десятых долях герца. Минимум - 400 герц (определено набором команд CAT). */
	unsigned * pvalue)
{
	* pvalue = FTWAF001(tonefreq01 * 10u);	// В сотых долях герца
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
// UAC IN samplerate
// todo: сделать нормальный расчёт для некруглых значений ARMI2SRATE
int_fast32_t dsp_get_samplerateuacin_audio48(void)
{
	return dsp_get_sampleraterx();
}
// UAC IN samplerate
// todo: сделать нормальный расчёт для некруглых значений ARMI2SRATE
int_fast32_t dsp_get_samplerateuacin_RTS96(void)
{
#if FPGAMODE_GW2A
	return dsp_get_sampleraterxscaled(1);
#else
	return dsp_get_sampleraterxscaled(2);
#endif
}

// UAC IN samplerate
// todo: сделать нормальный расчёт для некруглых значений ARMI2SRATE
int_fast32_t dsp_get_samplerateuacin_RTS192(void)
{
	return dsp_get_sampleraterxscaled(4);
}

int_fast32_t dsp_get_samplerateuacin_rts(void)		// RTS samplerate
{
#if WITHRTS192
	return dsp_get_samplerateuacin_RTS192();
#elif WITHRTS96
	return dsp_get_samplerateuacin_RTS96();
#else
	return dsp_get_samplerateuacin_audio48();
#endif
}


// Передача параметров в DSP модуль
// Обновление параметров приёмника (кроме фильтров).
static void 
rxparam_update(uint_fast8_t profile, uint_fast8_t pathi)
{
	// Параметры АРУ приёмника
	{
		const int gainmax = glob_digigainmax;	// Верхний предел регулировки усиления
		const int gainmin = 0;	// Нижний предел регулировки усиления
		const int gaindb = ((gainmax - gainmin) * (int) (glob_ifgain - BOARD_IFGAIN_MIN) / (int) (BOARD_IFGAIN_MAX - BOARD_IFGAIN_MIN)) + gainmin;	// -20..+100 dB
		const FLOAT_t manualrfgain = db2ratio(gaindb);
		
		agc_parameters_update(& rxagcparams [profile] [pathi], manualrfgain, pathi);	// приёмник #0,#1
	}

	// Параметры S-метра приёмника
	{
		agc_smeter_parameters_update(& rxsmeterparams);
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

	// шумодав
	{
		const volatile agcparams_t * const agcp = & rxsmeterparams;

		const FLOAT_t upper = agccalcstrength_log(agcp, agcp->levelfence);
		const FLOAT_t lower = agccalcstrength_log(agcp, agcp->mininput);
		manualsquelch [pathi] = (int) glob_squelch * (upper - lower) / SQUELCHMAX + lower;
	}

	// Уровень сигнала самоконтроля
#if WITHWAVPLAYER
	// В этих вариантвх самоконтроля нет.
	sidetonevolume = 0;
#else /* */
	sidetonevolume = (glob_sidetonelevel / (FLOAT_t) 100);
#endif /*  */
	mainvolumerx = 1 - sidetonevolume;
}

// Передача параметров в DSP модуль
// Обновление параметров передатчика (кроме фильтров).
static void 
txparam_update(uint_fast8_t profile)
{
	const FLOAT_t txlevelfence = 1;	// контролировать по отсутствию индикации переполнения DUC при передаче

	#if WITHTXCPATHCALIBRATE
		const FLOAT_t c1MODES = (FLOAT_t) glob_designscale / 100;	// предотвращение переполнения
	#else /* WITHTXCPATHCALIBRATE */
		const FLOAT_t c1MODES = (FLOAT_t) HARDWARE_DACSCALE;	// предотвращение переполнения
	#endif
	const FLOAT_t c1DIGI = c1MODES * (FLOAT_t) glob_digiscale / 100;
	const FLOAT_t c1CW = c1MODES * (FLOAT_t) glob_cwscale / 100;

	txlevelfenceAM = 	txlevelfence * c1CW;	// Для режимов с lo6=0 - у которых нет подавления нерабочей боковой
	txlevelfenceSSB = 	txlevelfence * c1MODES;
	txlevelfenceBPSK = 	txlevelfence * c1MODES;
	txlevelfenceNFM = 	txlevelfence * c1CW;
	txlevelfenceCW = 	txlevelfence * c1CW;
	txlevelfenceBPSK = 	txlevelfence * c1CW;
	txlevelfenceDIGI = 	txlevelfence * c1DIGI;

	// Параметры АРУ микрофона
	comp_parameters_update(& txagcparams [profile], (int) glob_mikeagcgain);

	{
		// Настройка ограничителя
		const FLOAT_t FS = txagcparams [profile].levelfence;	// txlevelfenceSSB
		const FLOAT_t grade = 1 - (glob_mikehclip / (FLOAT_t) 100);
		mickeclipscale [profile] = 1 / grade;
		mickecliplevelp [profile] = FS * grade;
		mickeclipleveln [profile] = - FS * grade;
	}
	{
		// компрессор
	#if WITHCOMPRESSOR
		audio_compressor_set_attack(NSAITICKS(glob_compattack));
		audio_compressor_set_release(NSAITICKS(glob_comprelease));
		audio_compressor_set_hold(NSAITICKS(glob_comphold));
		audio_compressor_set_gainreduce(db2ratio(- (int) glob_compgain));
		audio_compressor_set_threshold(db2ratio(- (int) glob_compthreshold));
		audio_compressor_recalc();
	#endif /* WITHCOMPRESSOR */
	}
	{
		// ревербератор
	#if WITHREVERB
		audio_reverb_set_loss(glob_reverb ? db2ratio(- (int) glob_reverbloss) : 0);
		audio_reverb_set_delay(glob_reverbdelay);
	#endif /* WITHREVERB */
	}

	{
		// AM parameters
		const FLOAT_t amshapesignal = (FLOAT_t) (int) glob_amdepth / (100 + (int) glob_amdepth);
		amshapesignalHALF = amshapesignal / 2;
		amcarrierHALF = txlevelfenceAM - txlevelfenceAM * amshapesignal;
	}

	scaleDAC = (FLOAT_t) (int) glob_dacscale / BOARDDACSCALEMAX;

	subtonevolume = (glob_subtonelevel / (FLOAT_t) 100);
	mainvolumetx = 1 - subtonevolume;

	// Девиация в NFM
	gnfmdeviationftw = FTWAF((int) glob_nfmdeviation100 * 100L);
}

// Передача параметров в DSP модуль
static void 
trxparam_update(void)
{
	// CW & sidetone edge
	enveloplen0 = NSAITICKS(glob_cwedgetime) + 1;		/* количество сэмплов, за которое меняется огибающая */
	// 0.707 == M_SQRT1_2
	/* http://gregstoll.dyndns.org/~gregstoll/floattohex/ use for tests */
}

/* вызывается при разрешённых прерываниях. */
void dsp_initialize(void)
{
	PRINTF("dsp_initialize: ARMI2SRATE=%lu, ARMI2SRATE100=%lu.%02lu\n", (unsigned long) ARMI2SRATE, (unsigned long) (ARMI2SRATE100 / 100), (unsigned long) (ARMI2SRATE100 % 100));
	//PRINTF("DMABUFFSIZE32RX=%d, DMABUFFSTEP32RX=%d\n", (int) DMABUFFSIZE32RX, (int) DMABUFFSTEP32RX);

	fir_design_windowbuff_half(FIRCwnd_tx_MIKE, Ntap_tx_MIKE);
	fir_design_windowbuff_half(FIRCwnd_rx_AUDIO, Ntap_rx_AUDIO);
	//fft_lookup = spx_fft_init(2*SPEEXNN);

#if WITHDSPEXTFIR
	#if WITHDOUBLEFIRCOEFS && (__ARM_FP & 0x08)
		fir_design_windowbuffL_half(FIRCwndL_trxi_IQ, Ntap_trxi_IQ);
	#else
		fir_design_windowbuff_half(FIRCwnd_trxi_IQ, Ntap_trxi_IQ);
	#endif
#endif /* WITHDSPEXTFIR */

#if WITHDSPLOCALFIR	
	fir_design_windowbuff_half(FIRCwnd_rx_SSB_IQ, Ntap_rx_SSB_IQ);
	fir_design_windowbuff_half(FIRCwnd_tx_SSB_IQ, Ntap_tx_SSB_IQ);
#endif /* WITHDSPLOCALFIR */
#if WITHDSPLOCALTXFIR
	fir_design_windowbuff_half(FIRCwnd_tx_SSB_IQ, Ntap_tx_SSB_IQ);
#endif /* WITHDSPLOCALTXFIR */

	omega2ftw_k1 = POWF(2, NCOFTWBITS);

	adapterst_initialize();

	// Разрядность поступающего с микрофона сигнала

	agc_initialize();
	voxmeter_initialize();
	trxparam_update();
	const uint_fast8_t rprofile = ! gwagcprofrx;	// индекс профиля, который станет рабочим

	uint_fast8_t pathi;
	for (pathi = 0; pathi < NTRX; ++ pathi)
		rxparam_update(rprofile, pathi);
	gwagcprofrx = rprofile;

	ARM_MORPH(arm_fir_init)(& tx_fir_instance, Ntap_tx_MIKE, tx_firEQcoeff, tx_fir_state, tx_MIKE_blockSize);

	const uint_fast8_t tprofile = ! gwagcproftx;	// индекс профиля, который станет рабочим
	txparam_update(tprofile);
	gwagcproftx = tprofile;

	const uint_fast8_t spf = ! gwprof;	// индекс профиля для подготовки параметров DSP

	audio_setup_mike(spf);
	for (pathi = 0; pathi < NTRX; ++ pathi)
		audio_update(spf, pathi);
	gwprof = spf;

	modem_update();

#if 0
	{
		float32_t vs [4] = { 1, 2, 3, 4 };
		float32x4_t v1 = vld1q_f32(vs);
		float32x4_t v2 = vrev64q_f32(v1);
		// output: neontest: 2 1 4 3
		PRINTF(PSTR("neontest: %d %d %d %d\n"), (int) vgetq_lane_f32(v2, 0), (int) vgetq_lane_f32(v2, 1), (int) vgetq_lane_f32(v2, 2), (int) vgetq_lane_f32(v2, 3));
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
	ifc1->setvolume(glob_afgain, glob_afmute, glob_dsploudspeaker_off);
	ifc1->setlineinput(glob_lineinput, glob_mikeboost20db, glob_mik1level, glob_lineamp);
#if defined (HARDWARE_CODEC1_NPROCPARAMS)
	ifc1->setprocparams(glob_mikeequal, glob_codec1_gains);	/* параметры обработки звука с микрофона (эхо, эквалайзер, ...) */
#endif /* defined (HARDWARE_CODEC1_NPROCPARAMS) */
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
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	f = flag_dsp1reg;
	flag_dsp1reg = 0;
	LowerIrql(oldIrql);
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
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	f = flag_flt1reg;
	flag_flt1reg = 0;
	LowerIrql(oldIrql);
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
	IRQL_t oldIrql;
	RiseIrql(IRQL_SYSTEM, & oldIrql);
	f = flag_codec1reg;
	flag_codec1reg = 0;
	LowerIrql(oldIrql);
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
	flag_flt1reg = 1; 
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
board_set_ifgain(uint_fast16_t v)
{
	if (glob_ifgain != v)
	{
		glob_ifgain = v;
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

// scale в процентах
/* Для эксперементов по улучшению приема АМ */
void
board_set_agc_scale(uint_fast8_t n)	/* подстройка параметра АРУ */
{
	if (glob_agc_scale [glob_trxpath] != n)
	{
		glob_agc_scale [glob_trxpath] = n;
		board_dsp1regchanged();
	}
}

// chargespeedfast в милисекундах
void
board_set_agc_t0(uint_fast8_t n)	/* подстройка параметра АРУ */
{
	if (glob_agc_t0 [glob_trxpath] != n)
	{
		glob_agc_t0 [glob_trxpath] = n;
		board_dsp1regchanged();
	}
}

// chargespeedslow в милисекундах
void
board_set_agc_t1(uint_fast8_t n)	/* подстройка параметра АРУ */
{
	if (glob_agc_t1 [glob_trxpath] != n)
	{
		glob_agc_t1 [glob_trxpath] = n;
		board_dsp1regchanged();
	}
}

// dischargespeedslow в сотнях милисекунд (0.1 секунды)
void
board_set_agc_t2(uint_fast8_t n)	/* подстройка параметра АРУ */
{
	if (glob_agc_t2 [glob_trxpath] != n)
	{
		glob_agc_t2 [glob_trxpath] = n;
		board_dsp1regchanged();
	}
}

// dischargespeedfast в милисекундах
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
board_set_squelch(uint_fast8_t n)	/* уровень открывания шумоподавителя */
{
	if (glob_squelch != n)
	{
		glob_squelch = n;		board_dsp1regchanged();
	}
}

#if WITHNOTCHONOFF || WITHNOTCHFREQ

// Работает при BOARD_NOTCH_MANUAL
void 
board_set_notch_freq(uint_fast16_t n)	/* частота NOTCH фильтра */
{
	if (glob_notch_freq != n)
	{
		glob_notch_freq = n;
		board_flt1regchanged();		// параметры этой функции используются в audio_update();
	}
}

// Работает при BOARD_NOTCH_MANUAL
void 
board_set_notch_width(uint_fast16_t n)	/* полоса NOTCH фильтра */
{
	if (glob_notch_width != n)
	{
		glob_notch_width = n;
		board_flt1regchanged();		// параметры этой функции используются в audio_update();
	}
}

//	#define BOARD_NOTCH_OFF		0
//	#define BOARD_NOTCH_MANUAL	1
//	#define BOARD_NOTCH_AUTO	2
void 
board_set_notch_mode(uint_fast8_t n)	/* включение NOTCH фильтра */
{
	if (glob_notch_mode != n)
	{
		glob_notch_mode = n;
		board_flt1regchanged();		// параметры этой функции используются в audio_update();
	}
}

#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */

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
board_set_moniflag(uint_fast8_t v)	/* разрешение самопрослушивания */
{
	const uint_fast8_t n = v != 0;
	if (glob_moniflag != n)
	{
		glob_moniflag = n;
		board_dsp1regchanged();
	}
}

void
board_set_cwssbtx(uint_fast8_t v)	/* разрешение передачи телеграфа как тона в режиме SSB */
{
	const uint_fast8_t n = v != 0;
	if (glob_cwssbtx != n)
	{
		glob_cwssbtx = n;
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

//	#define BOARDDACSCALEMIN	0	// Нижний предел мощности (аргумент board_set_dacscale() */
//	#define BOARDDACSCALEMAX	10000	// Верхний предел мощности (аргумент board_set_dacscale() */
#if WITHIF4DSP

void 
board_set_dacscale(uint_fast16_t n)	/* Использование амплитуды сигнала с ЦАП передатчика - 0..100.00% */
{
	if (glob_dacscale != n)
	{
		glob_dacscale = n;	// BOARDDACSCALEMIN..BOARDDACSCALEMAX
		board_dsp1regchanged();
	}
	//PRINTF("board_set_dacscale = %u\n", (unsigned) glob_dacscale);
}

#endif /* WITHIF4DSP */

void 
board_set_digiscale(uint_fast16_t n)	/* Увеличение усиления при передаче в цифровых режимах 100..300% */
{
	if (glob_digiscale != n)
	{
		glob_digiscale = n;
		board_dsp1regchanged();
	}
}

void
board_set_cwscale(uint_fast16_t n)	/* Увеличение усиления при передаче в цифровых режимах 100..300% */
{
	if (glob_cwscale != n)
	{
		glob_cwscale = n;
		board_dsp1regchanged();
	}
}

void
board_set_designscale(uint_fast16_t n)	/* используется при калибровке параметров интерполятора */
{
	if (glob_designscale != n)
	{
		glob_designscale = n;
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
board_set_mikeboost20db(uint_fast8_t n)
{
	const uint_fast8_t v = n != 0;
	if (glob_mikeboost20db != v)
	{
		glob_mikeboost20db = v;
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

/* компрессор */
void
board_set_compressor(uint_fast8_t attack, uint_fast8_t release, uint_fast8_t hold, uint_fast8_t gain, uint_fast8_t threshold)
{
	if (glob_compattack != attack || glob_comprelease != release || glob_comphold != hold || glob_compgain != gain || glob_compthreshold != threshold)
	{
		glob_compattack = attack;
		glob_comprelease = release;
		glob_comphold = hold;
		glob_compgain = gain;
		glob_compthreshold = threshold;
		board_dsp1regchanged();
	}
}

/* ревербератор */
void
board_set_reverb(uint_fast8_t reverb, uint_fast8_t reverbdelay, uint_fast8_t reverbloss)
{
#if WITHREVERB
	if (glob_reverb != reverb || glob_reverbdelay != reverbdelay || glob_reverbloss != reverbloss)
	{
		glob_reverb = reverb;
		glob_reverbdelay = reverbdelay;
		glob_reverbloss = reverbloss;
		board_dsp1regchanged();
	}
#endif /* WITHREVERB */
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

/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
void board_set_uacplayer(uint_fast8_t v)
{
#if WITHUSBUAC
	uacoutplayer = v;
#endif /* WITHUSBUAC */
}

/* автоматическое изменение источника при появлении звука со стороны компьютера */
void board_set_datatx(uint_fast8_t v)
{
#if WITHUSBUAC && WITHTX
	datavox = v;
#endif /* WITHUSBUAC && WITHTX */
}

#if WITHAFEQUALIZER

#define EQ_STAGES				1
#define BIQUAD_COEFF_IN_STAGE 	5

static FLOAT_t EQ_RX_LOW_FILTER_State [2 * EQ_STAGES] = { 0 };
static FLOAT_t EQ_RX_MID_FILTER_State [2 * EQ_STAGES] = { 0 };
static FLOAT_t EQ_RX_HIGH_FILTER_State [2 * EQ_STAGES] = { 0 };
static FLOAT_t EQ_RX_LOW_FILTER_Coeffs [BIQUAD_COEFF_IN_STAGE * EQ_STAGES] = { 0 };
static FLOAT_t EQ_RX_MID_FILTER_Coeffs [BIQUAD_COEFF_IN_STAGE * EQ_STAGES] = { 0 };
static FLOAT_t EQ_RX_HIGH_FILTER_Coeffs [BIQUAD_COEFF_IN_STAGE * EQ_STAGES] = { 0 };

static ARM_MORPH(arm_biquad_cascade_df2T_instance) EQ_RX_LOW_FILTER = { EQ_STAGES, EQ_RX_LOW_FILTER_State, EQ_RX_LOW_FILTER_Coeffs };
static ARM_MORPH(arm_biquad_cascade_df2T_instance) EQ_RX_MID_FILTER = { EQ_STAGES, EQ_RX_MID_FILTER_State, EQ_RX_MID_FILTER_Coeffs };
static ARM_MORPH(arm_biquad_cascade_df2T_instance) EQ_RX_HIGH_FILTER = { EQ_STAGES, EQ_RX_HIGH_FILTER_State, EQ_RX_HIGH_FILTER_Coeffs };

static FLOAT_t EQ_TX_LOW_FILTER_State [2 * EQ_STAGES] = { 0 };
static FLOAT_t EQ_TX_MID_FILTER_State [2 * EQ_STAGES] = { 0 };
static FLOAT_t EQ_TX_HIGH_FILTER_State [2 * EQ_STAGES] = { 0 };
static FLOAT_t EQ_TX_LOW_FILTER_Coeffs [BIQUAD_COEFF_IN_STAGE * EQ_STAGES] = { 0 };
static FLOAT_t EQ_TX_MID_FILTER_Coeffs [BIQUAD_COEFF_IN_STAGE * EQ_STAGES] = { 0 };
static FLOAT_t EQ_TX_HIGH_FILTER_Coeffs [BIQUAD_COEFF_IN_STAGE * EQ_STAGES] = { 0 };

static ARM_MORPH(arm_biquad_cascade_df2T_instance) EQ_TX_LOW_FILTER = { EQ_STAGES, EQ_TX_LOW_FILTER_State, EQ_TX_LOW_FILTER_Coeffs };
static ARM_MORPH(arm_biquad_cascade_df2T_instance) EQ_TX_MID_FILTER = { EQ_STAGES, EQ_TX_MID_FILTER_State, EQ_TX_MID_FILTER_Coeffs };
static ARM_MORPH(arm_biquad_cascade_df2T_instance) EQ_TX_HIGH_FILTER = { EQ_STAGES, EQ_TX_HIGH_FILTER_State, EQ_TX_HIGH_FILTER_Coeffs };

void calcBiquad(uint32_t Fc, uint32_t Fs, FLOAT_t Q, FLOAT_t peakGain, FLOAT_t * outCoeffs)
{
	FLOAT_t a0, a1, a2, b1, b2, norm;

	FLOAT_t V = POWF(10.0f, FABSF(peakGain) / 20);
	FLOAT_t K = TANF(PI * Fc / Fs);
    if (peakGain >= 0)
    {
        norm = 1.0f / (1.0f + 1.0f / Q * K + K * K);
        a0 = (1.0f + V / Q * K + K * K) * norm;
        a1 = 2.0f * (K * K - 1.0f) * norm;
        a2 = (1.0f - V / Q * K + K * K) * norm;
        b1 = a1;
        b2 = (1.0f - 1.0f / Q * K + K * K) * norm;
    }
    else
    {
        norm = 1.0f / (1.0f + V / Q * K + K * K);
        a0 = (1.0f + 1.0f / Q * K + K * K) * norm;
        a1 = 2.0f * (K * K - 1.0f) * norm;
        a2 = (1.0f - 1.0f / Q * K + K * K) * norm;
        b1 = a1;
        b2 = (1.0f - V / Q * K + K * K) * norm;
    }

    //save coefficients
    outCoeffs[0] = a0;
    outCoeffs[1] = a1;
    outCoeffs[2] = a2;
    outCoeffs[3] = - b1;
    outCoeffs[4] = - b2;
}

void audio_rx_equalizer_init(void)
{
	FLOAT_t base = hamradio_get_af_equalizer_base();
	FLOAT_t max_coeff = 0;

	for (uint_fast8_t i = 0; i < 3; i ++)
		max_coeff = max_coeff < glob_equalizer_rx_gains [i] ? glob_equalizer_rx_gains [i] : max_coeff;

	max_coeff += base;

    calcBiquad(AF_EQUALIZER_LOW,  ARMI2SRATE, 1, glob_equalizer_rx_gains [0] + base - max_coeff, EQ_RX_LOW_FILTER_Coeffs);
    calcBiquad(AF_EQUALIZER_MID,  ARMI2SRATE, 1, glob_equalizer_rx_gains [1] + base - max_coeff, EQ_RX_MID_FILTER_Coeffs);
    calcBiquad(AF_EQUALIZER_HIGH, ARMI2SRATE, 1, glob_equalizer_rx_gains [2] + base - max_coeff, EQ_RX_HIGH_FILTER_Coeffs);
}

void audio_tx_equalizer_init(void)
{
	FLOAT_t base = hamradio_get_af_equalizer_base();
	FLOAT_t max_coeff = 0;

	for (uint_fast8_t i = 0; i < 3; i ++)
		max_coeff = max_coeff < glob_equalizer_tx_gains [i] ? glob_equalizer_tx_gains [i] : max_coeff;

	max_coeff += base;

    calcBiquad(AF_EQUALIZER_LOW,  ARMI2SRATE, 1.0f, glob_equalizer_tx_gains [0] + base - max_coeff, EQ_TX_LOW_FILTER_Coeffs);
    calcBiquad(AF_EQUALIZER_MID,  ARMI2SRATE, 1.0f, glob_equalizer_tx_gains [1] + base - max_coeff, EQ_TX_MID_FILTER_Coeffs);
    calcBiquad(AF_EQUALIZER_HIGH, ARMI2SRATE, 1.0f, glob_equalizer_tx_gains [2] + base - max_coeff, EQ_TX_HIGH_FILTER_Coeffs);
}

void audio_rx_equalizer(FLOAT_t * buffer, uint_fast16_t size)
{
	if (glob_equalizer_rx)
	{
		ARM_MORPH(arm_biquad_cascade_df2T)(& EQ_RX_LOW_FILTER, buffer, buffer, size);
		ARM_MORPH(arm_biquad_cascade_df2T)(& EQ_RX_MID_FILTER, buffer, buffer, size);
		ARM_MORPH(arm_biquad_cascade_df2T)(& EQ_RX_HIGH_FILTER, buffer, buffer, size);
	}
}

void
board_set_equalizer_rx(uint_fast8_t n)
{
	const uint_fast8_t v = n != 0;
	if (glob_equalizer_rx != v)
	{
		glob_equalizer_rx = v;
	}
}

void
board_set_equalizer_tx(uint_fast8_t n)
{
	const uint_fast8_t v = n != 0;
	if (glob_equalizer_tx != v)
	{
		glob_equalizer_tx = v;
	}
}

void board_set_equalizer_rx_gains(const uint_fast8_t * p)
{
	if (memcmp(glob_equalizer_rx_gains, p, sizeof glob_equalizer_rx_gains) != 0)
	{
		memcpy(glob_equalizer_rx_gains, p, sizeof glob_equalizer_rx_gains);
		audio_rx_equalizer_init();
	}
}

void board_set_equalizer_tx_gains(const uint_fast8_t * p)
{
	if (memcmp(glob_equalizer_tx_gains, p, sizeof glob_equalizer_tx_gains) != 0)
	{
		memcpy(glob_equalizer_tx_gains, p, sizeof glob_equalizer_tx_gains);
		audio_tx_equalizer_init();
	}
}

#endif /* WITHAFEQUALIZER */

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

void
board_set_gainnfmrx(int_fast16_t n)	/* дополнительное усиление по НЧ в режиме приёма NFM */
{
	if (glob_gainnfmrx [glob_trxpath] != n)
	{
		glob_gainnfmrx [glob_trxpath] = n;
		board_flt1regchanged();	// параметры этой функции используются в audio_update();
	}
}

/* Девиация в NFM (сотни герц) */
void
board_set_nfmdeviation100(uint_fast8_t v)
{
	if (glob_nfmdeviation100 != v)
	{
		glob_nfmdeviation100 = v;
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

// Левый/правый, A - main RX, B - sub RX
void board_set_mainsubrxmode(uint_fast8_t v)
{
	glob_mainsubrxmode = v;
}


/*  */
void
board_set_dsploudspeaker(uint_fast8_t v)
{
	const uint_fast8_t n = v != 0;
	if (glob_dsploudspeaker_off != n)
	{
		glob_dsploudspeaker_off = n;
		board_codec1regchanged();
	}
}

void
board_set_dspagc(uint_fast8_t n)
{
	if (glob_dspagc != n)
	{
		glob_dspagc = n;
		board_dsp1regchanged();
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
		board_set_boardagc(buff [DSPCTL_OFFSET_AGCOFF] ? BOARD_AGCCODE_OFF : BOARD_AGCCODE_ON);
		board_set_dspagc(buff [DSPCTL_OFFSET_AGCOFF] ? BOARD_AGCCODE_OFF : BOARD_AGCCODE_ON);
#if ! WITHPOTIFGAIN
		board_set_ifgain(buff [DSPCTL_OFFSET_IFGAIN_HI] * 256 + buff [DSPCTL_OFFSET_IFGAIN_LO]);
#endif /* ! WITHPOTIFGAIN */
#if ! WITHPOTAFGAIN
		board_set_afgain(buff [DSPCTL_OFFSET_AFGAIN_HI] * 256 + buff [DSPCTL_OFFSET_AFGAIN_LO]);
#endif /* ! WITHPOTAFGAIN */
		board_set_afmute(buff [DSPCTL_OFFSET_AFMUTE]);
		board_set_agc_t1(buff [DSPCTL_OFFSET_AGC_T1]);
		board_set_agc_t2(buff [DSPCTL_OFFSET_AGC_T2]);
		board_set_agc_t4(buff [DSPCTL_OFFSET_AGC_T4]);
		board_set_agc_thung(buff [DSPCTL_OFFSET_AGC_THUNG]);
		board_set_agcrate(buff [DSPCTL_OFFSET_AGCRATE]);	// на n децибел изменения входного сигнала 1 дБ выходного. UINT8_MAX - "плоская" АРУ
	
		board_set_mik1level(buff [DSPCTL_OFFSET_MICLEVEL_HI] * 256 + buff [DSPCTL_OFFSET_MICLEVEL_LO]);

		board_set_afhighcutrx(buff [DSPCTL_OFFSET_HIGHCUTRX_HI] * 256 + buff [DSPCTL_OFFSET_HIGHCUTRX_LO]);
		board_set_aflowcutrx(buff [DSPCTL_OFFSET_LOWCUTRX_HI] * 256 + buff [DSPCTL_OFFSET_LOWCUTRX_LO]);

		board_set_afhighcuttx(buff [DSPCTL_OFFSET_HIGHCUTTX_HI] * 256 + buff [DSPCTL_OFFSET_HIGHCUTTX_LO]);
		board_set_aflowcuttx(buff [DSPCTL_OFFSET_LOWCUTTX_HI] * 256 + buff [DSPCTL_OFFSET_LOWCUTTX_LO]);

		board_set_cwedgetime(buff [DSPCTL_OFFSET_CWEDGETIME]);
		board_set_sidetonelevel(buff [DSPCTL_OFFSET_SIDETONELVL]);

		board_set_notch_mode(buff [DSPCTL_OFFSET_NOTCH_MODE]);
		board_set_notch_width(buff [DSPCTL_OFFSET_NOTCH_WIDTH_HI] * 256 + buff [DSPCTL_OFFSET_NOTCH_WIDTH_LO]);
		board_set_notch_freq(buff [DSPCTL_OFFSET_NOTCH_FREQ_HI] * 256 + buff [DSPCTL_OFFSET_NOTCH_FREQ_LO]);
		board_set_lo6(buff [DSPCTL_OFFSET_LO6_FREQ_HI] * 256 + buff [DSPCTL_OFFSET_LO6_FREQ_LO]);
		board_set_fullbw6(buff [DSPCTL_OFFSET_FULLBW6_HI] * 256 + buff [DSPCTL_OFFSET_FULLBW6_LO]);
		board_set_digigainmax(buff [DSPCTL_OFFSET_DIGIGAINMAX]);
		//board_set_fltsofter(buff [DSPCTL_OFFSET_FLTSOFTER]);
		board_set_amdepth(buff [DSPCTL_OFFSET_AMDEPTH]);
		board_set_mikeagc(buff [DSPCTL_OFFSET_MIKEAGC]);
		board_set_mikehclip(buff [DSPCTL_OFFSET_MIKEHCLIP]);
	}
}

#endif /* WITHSPISLAVE */
