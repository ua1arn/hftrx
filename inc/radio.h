/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Определения, специфические для устройств, относящихся к радиосвязи.
//


#ifndef RADIO_H_INCLUDED
#define RADIO_H_INCLUDED

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined (WITHUSBUACIN) && defined (WITHUSBUACOUT)
	#define WITHUSBUAC 1
#elif defined (WITHUSBUACIN)
	#define WITHUSBUAC 1
#elif defined (WITHUSBUACOUT)
	#define WITHUSBUAC 1
#elif WITHUSBUAC
	#define WITHUSBUACIN	1
	#define WITHUSBUACOUT	1
#endif /* WITHUSBUAC */


#if WITHBBOX && defined (WITHBBOXFREQ)
	#define DEFAULTDIALFREQ	WITHBBOXFREQ
#elif ! defined (DEFAULTDIALFREQ)
	#define DEFAULTDIALFREQ	7012000L
#endif

#define WITHMAXRXTXDELAY	100
#define WITHMAXTXRXDELAY	100

#define WITHNOTCHFREQMIN	300
#define WITHNOTCHFREQMAX	5000

#define WITHNOTCHWIDTHMIN	100
#define WITHNOTCHWIDTHMAX	1000

/* параметры отображения панорамы */
#define WITHSPPARTMIN		0		/*  % */
#define WITHSPPARTMAX		100		/*  % */

#if WITHPOTIFGAIN
	// потенциометром
	#define	BOARD_IFGAIN_MIN	0		/* код управления усилением ВЧ тракта */
	#define	BOARD_IFGAIN_MAX	255		/* код управления усилением ВЧ тракта */
#else /* WITHPOTIFGAIN */
	// валкодером
	#define	BOARD_IFGAIN_MIN	0		/* код управления усилением ВЧ тракта */
	#define	BOARD_IFGAIN_MAX	25		/* код управления усилением ВЧ тракта */
#endif /* WITHPOTIFGAIN */

#if WITHPOTAFGAIN
	// потенциометром
	#define	BOARD_AFGAIN_MIN	0		/* код управления усилением НЧ тракта */
	#define	BOARD_AFGAIN_MAX	255		/* код управления усилением НЧ тракта */
#else /* WITHPOTAFGAIN */
	// валкодером
	#define	BOARD_AFGAIN_MIN	0		/* код управления усилением НЧ тракта */
	#define	BOARD_AFGAIN_MAX	25		/* код управления усилением НЧ тракта */
#endif /* WITHPOTAFGAIN */

#define WITHLINEINGAINMIN	0		/* код управления усилением входа с линии */
#define WITHLINEINGAINMAX	255		/* код управления усилением входа с линии */

#define WITHMIKEINGAINMIN	0		/* код управления усилением входа с микрофона, %  */
#define WITHMIKEINGAINMAX	100		/* код управления усилением входа с микрофона, %  */

#define WITHMIKECLIPMIN		0
#define WITHMIKECLIPMAX		90

#define WITHMIKEAGCMIN		10
#define WITHMIKEAGCMAX		60

#define WITHVOXDELAYMIN		10	/* ms */
#define WITHVOXDELAYMAX		250	/* ms */

#define WITHVOXLEVELMIN		0	/*  % */
#define WITHVOXLEVELMAX		100	/*  % */

#define WITHAVOXLEVELMIN	0	/*  % */
#define WITHAVOXLEVELMAX	100	/*  % */

#define WITHFILTSOFTMIN		0		/* код управления сглаживанием скатов фильтра основной селекции на приёме */
#define WITHFILTSOFTMAX		10		/* код управления сглаживанием скатов фильтра основной селекции на приёме */

#define WITHREVERBDELAYMIN 1	/* минимальная задержка ревербератора, % */
#define WITHREVERBDELAYMAX 100	/* максимальная задержка ревербератора, % */

#define WITHREVERBLOSSMIN 0		/* минимальное ослабление на возврате ревербератора (dB) */
#define WITHREVERBLOSSMAX 30	/* максимальная ослабление на возврате ревербератора (dB) */

#define WITHCOMPATTACKMIN 1		/* минимальное время атаки компрессора (мс) */
#define WITHCOMPATTACKMAX 100	/* максимальная время атаки компрессора (мс) */
#define WITHCOMPRELEASEMIN 1	/* минимальное время освобождения компрессора (мс) */
#define WITHCOMPRELEASEMAX 100	/* максимальная время освобождения компрессора (мс) */
#define WITHCOMPHOLDMIN 1		/* минимальное время удержания компрессора (мс) */
#define WITHCOMPHOLDMAX 100		/* максимальная время удержания компрессора (мс) */
#define WITHCOMPGAINMIN 1		/* минимальное отношение компрессии (db) */
#define WITHCOMPGAINMAX 20		/* максимальная отношение компрессии (db) */
#define WITHCOMPTHRESHOLDMIN 1	/* минимальный порог компрессии (db) */
#define WITHCOMPTHRESHOLDMAX 20	/* максимальная порог компрессии (db) */

#define NRLEVELMAX 60

#define WITHMAXFSPOWER	(+ 60)
#define WITHMINFSPOWER	(- 160)


#if defined (DAC1_TYPE)
	#define WITHDAC1VALMIN	0
	#define WITHDAC1VALMAX	255
#endif /* defined (DAC1_TYPE) */

#if WITHIF4DSP

	#define BOARD_DETECTOR_SSB 	0		// Заглушка

	#define	BOARD_AGCCODE_ON	0x00
	#define	BOARD_AGCCODE_OFF	0x01

	#define BOARD_NOTCH_OFF		0
	#define BOARD_NOTCH_MANUAL	1
	#define BOARD_NOTCH_AUTO	2
	#define WITHLMSAUTONOTCH	1	/* Использование AUTONOTCH	*/
	//#define WITHLEAKYLMSANR		1	/* Использование LeakyLmsNr */

	#define WITHNOTCHFREQ		1	/* NOTCH фильтр с устанавливаемой через меню или потенциометром частотой */
	#define WITHSAM				1	/* synchronous AM demodulation */
	#define WITHMIC1LEVEL		1	/* установка усиления микрофона */

	#define	SQUELCHMAX	255	/* Kenwood's value */
	#define WITHAGCMODENONE		1	/* Режимами АРУ с кнопок не управляем */
#endif /* WITHIF4DSP */

#define CWPITCHSCALE 	10
#define CWPITCHMIN10	(100 / CWPITCHSCALE) // (400 / 10) kenwood min value 400 Hz
#define CWPITCHMAX10	(1900 / CWPITCHSCALE)

#if LO3_SIDE == LOCODE_INVALID
	//#error PBT can be used only with triple conversion schemes
#endif

#ifndef DDS2_CLK_DIV
	#define DDS2_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS2 */
#endif

enum
{
	DSPCTL_MODE_IDLE = 0,		/* режим, в котором не выполняется никакая обработка. */

	DSPCTL_MODE_RX_BASE,

	DSPCTL_MODE_RX_BPSK,
	DSPCTL_MODE_RX_FREEDV,
	DSPCTL_MODE_RX_WIDE, 	/* Приём с заданием полосы значениями верхнего и нижнего среза АЧХ */
	DSPCTL_MODE_RX_NARROW, 	/* Приём с заданием полосы значением центральной частоты и полосы пропускания */
	DSPCTL_MODE_RX_AM,
	DSPCTL_MODE_RX_SAM,
	DSPCTL_MODE_RX_NFM,
	DSPCTL_MODE_RX_DSB,		/* приём двух боковых полос (режим настройки по нулевым биениям */
	DSPCTL_MODE_RX_DRM,		/* принятиый сигнал транслируется на выход на частоте 12 кГц */
	DSPCTL_MODE_RX_ISB,		/* прием двух независимых боковых полос */
	DSPCTL_MODE_RX_WFM,		/* прием WFM (демодуляция в FPGA, только без WITHUSEDUALWATCH)	*/

	DSPCTL_MODE_RX_TOP,

	DSPCTL_MODE_TX_BASE,

	DSPCTL_MODE_TX_SSB,
	DSPCTL_MODE_TX_DIGI,
	DSPCTL_MODE_TX_CW,
	DSPCTL_MODE_TX_AM,
	DSPCTL_MODE_TX_NFM,
	DSPCTL_MODE_TX_ISB,		/* передача двух независимых боковых полос */
	DSPCTL_MODE_TX_BPSK,
	DSPCTL_MODE_TX_FREEDV,

	DSPCTL_MODE_TX_TOP,
	//
	DSPCTL_MODE_COUNT
};

uint_fast8_t board_getvox(void);	/* получить значение от детектора VOX 0..UINT8_MAX */
uint_fast8_t board_getavox(void);	/* получить значение от детектора Anti-VOX 0..UINT8_MAX */

// filters.c interface

/* перечисление всех возможных режимов принимаемых приемником.
 * Режим - это комбинация фильтр + детектор + третий гетеродин + боковая полоса
 */

enum { 
#if WITHMODESETSMART
	SUBMODE_SSBSMART,
	SUBMODE_CWSMART, SUBMODE_CWZSMART, 
	SUBMODE_DIGISMART,
#else
	SUBMODE_USB, SUBMODE_LSB, 
	SUBMODE_CW, SUBMODE_CWR, SUBMODE_CWZ, 
	SUBMODE_AM,
	#if WITHSAM
		SUBMODE_SAM,	/* synchronous AM demodulation */
	#endif /* WITHSAM */
	SUBMODE_DRM,
	SUBMODE_DGL, SUBMODE_DGU,
	#if WITHMODESETFULLNFM || WITHMODESETMIXONLY3NFM || WITHMODESETFULLNFMWFM
		SUBMODE_NFM,
	#endif /* WITHMODESETFULLNFM || WITHMODESETMIXONLY3NFM || WITHMODESETFULLNFMWFM*/
	#if WITHWFM || WITHMODESETFULLNFMWFM
		SUBMODE_WFM,
	#endif /* WITHWFM || WITHMODESETFULLNFMWFM */
	#if WITHMODEM
		SUBMODE_BPSK,
	#endif /* WITHMODEM */
	SUBMODE_ISB,
	#if WITHFREEDV
		SUBMODE_FREEDV,
	#endif /* WITHFREEDV */
	#if WITHRTTY
		SUBMODE_RTTY,
	#endif /* WITHRTTY */
#endif
	//
	SUBMODE_COUNT
};

/* семейства режимов работы */
enum {
	MODE_CW = 0,/* cw & cwr */
	MODE_SSB,	/* usb or lsb */
	MODE_AM,	/* am */
#if WITHSAM
	MODE_SAM,	/* synchronous AM demodulation */
#endif /* WITHSAM */
	MODE_NFM,	/* fm */
	MODE_DRM,	/* приём DRM */
	MODE_CWZ,	/* cwz - калибровка по нулевым биениям. На передаче используется при TUNE */
	MODE_TUNE = MODE_CWZ,	/* cwz - калибровка по нулевым биениям. На передаче используется при TUNE */
#if WITHWFM || WITHMODESETFULLNFMWFM
	MODE_WFM,	/* WFM mode */
#endif /* WITHWFM || WITHMODESETFULLNFMWFM */
	MODE_DIGI,	/* dgu & dgl */
	MODE_RTTY,	/* rtty */
#if WITHMODEM
	MODE_MODEM,
#endif /* WITHMODEM */
	MODE_ISB,	/* usb & lsb */
#if WITHFREEDV
	MODE_FREEDV,
#endif /* WITHFREEDV */
	//
	MODE_COUNT	/* номера режимов представляют собой ещё позиции битов в масках допустимых режимов работы для фильтров */
};

//#if WITHMODEM || WITHWFM || WITHIF4DSP || CPUSTYLE_ARM
	typedef uint_fast16_t MODEMASK_T;	/* Тип, достаточный для хранения MODE_COUNT битов */
//#else /* WITHMODEM || WITHWFM */
//	typedef uint_fast8_t MODEMASK_T;	/* Тип, достаточный для хранения MODE_COUNT битов */
//#endif /* WITHMODEM || WITHWFM || WITHIF4DSP || CPUSTYLE_ARM */


typedef struct lo2param_tag
{
	uint_least64_t lo2n [2];			/* N параметр для ФАПЧ (RX и TX) */
	uint_least64_t lo2r [2];			/* R параметр для ФАПЧ (RX и TX) */
	int_fast32_t lo4base;				/* IF3FREQBASE, например */
} lo2param_t;

/* Описание фильтра */
/* Если фильтр присутствует только в тракте према, поле suitmodestx = 0 */
/* Если фильтр присутствует только в тракте передачи, поле suitmodesrx = 0 */
/* В массиве code заполняются поля соответственно коммктации фильтра в траакты приема и передачи */
typedef struct filter_tag
{
	MODEMASK_T suitmodesrx;	/* suitable for: modefamily bits */
	MODEMASK_T suitmodestx;	/* suitable for: modefamily bits */
	MODEMASK_T defmodes;	/* default for: modefamily bits */
	uint_fast16_t low_or_center;		/* frequency of low edge (for ssb detector) -  Hz - offset from ::if3base  */
	uint_fast16_t high;		/*  high edge frequency (for ssb detector) - Hz - offset from ::if3base, 0 for narrow filters */
	uint_fast16_t code [2];	/* code of filter for RX/TX board control */
	uint_fast8_t present;	/* this filter installed */
	uint_fast8_t widefilter;	/* 0 - считается "узким", 1 - "широким" */
	uint_fast16_t ceoffset;		/*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	const lo2param_t * lo2set;
	const char * labelf3;	/* name of filter - 3 chars width */
} filter_t;

#define IF3OFFS 15000	/* половина перестройки частоты ската через меню - удвоенное значение должно помещаться в uint_fast16_t */
#define IF3CEOFFS 5000	/* половина перестройки частоты центра через меню - удвоенное значение должно помещаться в uint_fast16_t */

// используемые в меню фильтры
extern filter_t fi_2p4;
extern filter_t fi_2p4_tx;
extern filter_t fi_2p7;
extern filter_t fi_2p7_tx;
extern filter_t fi_0p3;
extern filter_t fi_0p5;
extern filter_t fi_1p8;
extern filter_t fi_3p1;
extern filter_t fi_3p1_tx;
extern filter_t fi_6p0;
extern filter_t fi_7p8;
extern filter_t fi_8p0;
extern filter_t fi_9p0;
extern filter_t fi_15p0;
extern filter_t fi_15p0_tx_nfm;
extern filter_t fi_17p0;

extern filter_t fi_2p0_455;
extern filter_t fi_10p0_455;
extern filter_t fi_3p0_455;
extern filter_t fi_6p0_455;

extern uint_fast16_t lo4offset;			/* частота (без базы) третьего гетеродина */
extern uint_fast16_t lo4offsets [2]; 	/* частота (без базы) третьего гетеродина для разных боковых */

int_fast32_t getlo4base(void); /* = IF3FREQBASE */
int_fast32_t getcefreqshiftbase(void); /* = (int_fast32_t) 0 - IF3CEOFFS */

int_fast32_t
getif3filtercenter(
	const filter_t * workfilter
	);

/* = IF3FREQBASE (для фильтра) */
int_fast32_t getlo4baseflt(
	const filter_t * workfilter 
	);

uint_fast16_t calc_next(uint_fast16_t v, uint_fast16_t low, uint_fast16_t high);

uint_fast8_t getgfasize(void);

const filter_t * 
NOINLINEAT
getrxfilter(
	uint_fast8_t submode,
	uint_fast8_t ix		/* текущий номер фильтра  */
	);

const filter_t * 
NOINLINEAT
gettxfilter(
	uint_fast8_t submode,
	uint_fast8_t ix		/* текущий номер фильтра  */
	);

uint_fast8_t 
//NOINLINEAT
getdefflt(
	uint_fast8_t mode,
	uint_fast8_t ix		/* текущий номер фильтра - возвращается если не нашли допустимых */
	);
uint_fast8_t 
//NOINLINEAT
getsuitablerx(
	uint_fast8_t mode,
	uint_fast8_t ix		/* текущий номер фильтра - возвращается если не нашли допустимых */
	);
uint_fast8_t 
//NOINLINEAT
getsuitabletx(
	uint_fast8_t mode,
	uint_fast8_t ix		/* текущий номер фильтра - возвращается если не нашли допустимых */
	);

uint_fast8_t 
findfilter(
	uint_fast8_t mode,
	uint_fast8_t ix,		// текущий быбранный фильтр
	unsigned width);			// полоса фильтра в герцах

const phase_t * getplo2n(
	const filter_t * workfilter,
	uint_fast8_t tx			/* признак работы в режиме передачи */
	);

const phase_t * getplo2r(
	const filter_t * workfilter,
	uint_fast8_t tx			/* признак работы в режиме передачи */
	);


enum
{
	BOARD_TXAUDIO_MIKE,	// "MIKE ",
	BOARD_TXAUDIO_LINE,	// "LINE ",
	BOARD_TXAUDIO_USB,	// "USB AUDIO",
	BOARD_TXAUDIO_BT,	// "BT AUDIO",
	BOARD_TXAUDIO_2TONE,	// "2TONE",
	BOARD_TXAUDIO_NOISE,	// "NOISE",
	BOARD_TXAUDIO_1TONE,	// "1TONE",
	BOARD_TXAUDIO_MUTE,		// no signal to tx
	//
	BOARD_TXAUDIO_unused
};

enum
{
	BOARD_CATSIG_NONE,
	BOARD_CATSIG_SER1_DTR,
	BOARD_CATSIG_SER1_RTS,
#if WITHUSBHW && WITHUSBCDCACM && WITHUSBCDCACM_N > 1
	BOARD_CATSIG_SER2_DTR,
	BOARD_CATSIG_SER2_RTS,
#endif /* WITHUSBHW && WITHUSBCDCACM && WITHUSBCDCACM_N > 1 */
	BOARD_CATSIG_count
};

enum
{
	BOARD_RXMAINSUB_A_A,	// в оба уха приемник A
	BOARD_RXMAINSUB_A_B,	// Левый - A, правый - B
	BOARD_RXMAINSUB_B_A,
	BOARD_RXMAINSUB_B_B,
	BOARD_RXMAINSUB_TWO,	// в оба аудиоканала поступает сумма выходов приемников.
	//
	BOARD_RXMAINSUB_count
};

void lfm_run(void);
void lfm_disable(void);
int iflfmactive(void);
uint_fast32_t getlfmfreq(void);

/* подготовка работы задержек переключения приём-передача */
void vox_initialize(void);
void vox_set_levels(uint_fast8_t level, uint_fast8_t alevel);	/* установить уровень срабатывания vox и anti-vox */
uint_fast8_t vox_getptt(void);

void seq_set_bkin_time(uint_fast8_t bkin_delay_tens);
void seq_set_vox_time(uint_fast8_t vox_delay_tens);
void seq_set_rgbeep(uint_fast8_t state);	/* разрешение (не-0) или запрещение (0) формирования roger beep */

// Для управления трактом надо задать управляющие слова dsp/фппаратуры для следующих состояний:
//
// состояние приёма
// состояние перехода с приёма на передачу и с передачи на приём
// состояние передачи SSB
// состояние передачи CW (а так же AM, NFM)

enum
{
	TXGFI_RX = 0,	// индекс состояния для режима приёма
	TXGFI_TRANSIENT,	// индекс состояния для режима перехода с приёма на передачу и с передачи на приём
	TXGFI_TX_KEYUP,		// режим передачи без нажатого ключа
	TXGFI_TX_KEYDOWN,	// режим передачи с нажатым ключём
	//
	TXGFI_SIZE	// количество элементов
};

// Подготовленные управляющие слова

#define TXGFV_RX		(UINT8_C(1) << 0)			// приём
#define TXGFV_TRANS		(UINT8_C(1) << 1)			// переход между режимами приёма и передачи
#define TXGFV_TX_SSB	(UINT8_C(1) << 2)
#define TXGFV_TX_CW		(UINT8_C(1) << 3)
#define TXGFV_TX_AM		(UINT8_C(1) << 4)
#define TXGFV_TX_NFM	(UINT8_C(1) << 5)

void seq_set_txgate(const portholder_t * txgfp, const uint8_t * sdtnp);	/* как включать передатчик в данном режиме работы из прерываний */

void hardware_cw_diagnostics_noirq(
	uint_fast8_t c1,
	uint_fast8_t c2,
	uint_fast8_t c3);
void hardware_cw_diagnostics(
	uint_fast8_t c1,
	uint_fast8_t c2,
	uint_fast8_t c3);


/* на разъёме ключа разводка такая:
 Вертикальный ключ:
	- центральный контакт - KEY
	- кольцевой контакт - NC
	- земляной контакт - GND

 Манипулятр (не перевёрнутое в меню состояние):
	- центральный контакт - DOT (точки)
	- кольцевой контакт - DASH (тире)
	- земляной контакт - GND
*/
enum
{
	ELKEY_PADDLE_NONE = 0x00,
	ELKEY_PADDLE_DIT = 0x01,
	ELKEY_PADDLE_DASH = 0x02,
};

#define ELKEY_DISCRETE 20		/* длительности точки и тире с точностью до 1/20 от длительности точки */

uint_fast8_t cat_get_keydown(void);	// Получить нажате ключа от порта управления, вызывается из обработчика перерываний

enum
{
	ELKEY_MODE_ACS,	// режим электронного ключа - ACS,
	ELKEY_MODE_ELE,	// electronic key
	ELKEY_MODE_OFF,	// straight key
	ELKEY_MODE_BUG	// BUG key emulation
};

/* электронный ключ */
void seq_initialize(void);
void elkey_initialize(void);
void board_set_wpm(uint_fast8_t wpm); /* обработка меню - установить скорость  */
void elkey_set_format(uint_fast8_t dashratio, uint_fast8_t spaceratio); /* обработка меню - установить отношениея тире к точке (в десятках процентов). */
void elkey_set_mode(uint_fast8_t mode, uint_fast8_t reverse);	/* режим электронного ключа - 0 - asf, 1 - paddle, 2 - keyer */
void elkey_set_slope(uint_fast8_t slope);	// скорость уменьшения длительности точки и паузы - имитация виброплекса
uint_fast8_t elkey_get_output(void);
uint_fast8_t elkey_get_ptt(void);
uint_fast8_t elkey_getnextcw(void);	// Получение символа для передачи (только верхний регистр)
uint_fast8_t vox_getbkin(void);

/* состояние секвенсора (промежуточные состояния для подготовки передачи и переключения реле при передаче) */
// Параметр функции board_set_seqphase()
enum
{
	SEQPHASE_INIT,	// RX
	SEQPHASE_FULLTX,	// TX
	//
	SEQPHASE_COUNT
};

/* обработка меню - установить задержку пре переходе на передачу и обратно. */
void seq_set_rxtxdelay(
	uint_fast8_t rxtxdelay, 
	uint_fast8_t txrxdelay,
	uint_fast8_t pretxdelay
	);
// запрос из user-mode части программы на переход на передачу и для для tune.
void seq_txrequest(uint_fast8_t ptt);

/* процедура возвращает из сиквенсора запрос на переключение на передачу в основную программу */
uint_fast8_t seq_get_txstate(void);
/* подтверждение от user-mode программы о том, что смена режима приём-передача осуществлена */
void seq_ask_txstate(
	uint_fast8_t tx);	/* 0 - периферия находимся в состоянии приёма, иначе - в состоянии передачи */

void seq_purge(void);	/* очистка запомненных нажатий до этого момента. Вызывается из user-mode программы */

uint_fast8_t seq_get_phase(void);	// состояние секвенсора (промежуточные состояния для подготовки передачи и переключения реле при передаче)

uint_fast8_t hardware_getshutdown(void);	/* возвращаем запрос на выключение - от компаратора питания */

void hardware_elkey_timer_initialize(void);
void hardware_elkey_set_speed(uint_fast32_t ticksfreq);
void hardware_elkey_ports_initialize(void); // Инициализация входов электронного ключа, входа CAT_DTR
void hardware_ptt_port_initialize(void);	// Инициализация входа PTT, входа CAT_RTS и TXDISABLE

uint_fast8_t hardware_elkey_getpaddle(uint_fast8_t reverse);
uint_fast8_t hardware_get_ptt(void);
uint_fast8_t hardware_get_tune(void);	/* запрос от внешней аппаратуры на переход в режим TUNE */
uint_fast8_t hardware_get_txdisable(void);

void hardware_txpath_initialize(void);
void hardware_txpath_set(portholder_t txpathstate);
void dsp_txpath_set(portholder_t txpathstate, uint_fast8_t keydown);	// вызывается из SYSTEM обработчика прерываний
void dsp_sidetone_setfreq(uint_least16_t tonefreq01);	/* tonefreq01 - частота в десятых долях герца. . */

/* сиквенсор приём-передача - и по таймерным и по 1/ELKEY_DISCRETE точки */
void elkey_spool_dots(void);	/* электронный ключ - вызывается с периодом 1/ELKEY_DISCRETE от длительности точки. */
void spool_0p128(void);	// OPERA support
/* Коды описывающие преобразования спектра в тракте */
#define LOCODE_UPPER	0	/* При преобразовании на этом гетеродине происходит инверсия спектра */
#define LOCODE_LOWER	1	/* При преобразовании на этом гетеродине нет инверсии спектра */
#define LOCODE_TARGETED	2	/* Этот гетеродин управляется для получения требуемой боковой. Только один может быть таким. */
#define LOCODE_INVALID	3	/* Этого гетеродина (и сместеля) нет. */

/* DUC/DDC. обработка квадратур в DSP процессоре */
#if FQMODEL_FPGA
	#define DUCDDC_FREQ (REFERENCE_FREQ * DDS1_CLK_MUL)		/* частота, которой тактируется NCO в DUC/DDC */
	/* Описание структуры преобразований частоты в тракте */

	#define R820T_IFFREQ 4063000uL
	#define R820T_REFERENCE (REFERENCE_FREQ / 4)	// 122.88 / 4 = 30.720 MHz
	//#define R820T_LOSTEP (R820T_REFERENCE / 16)	// 1.92 MHz
	#define R820T_LOSTEP (R820T_REFERENCE / 32)	// 920 kHz

	#if XVTR_R820T2
		#define LO0_SIDE_F(freq) ((freq) >= 56000000uL ? LOCODE_UPPER : LOCODE_INVALID)	/* при отсутствующем конверторе - на нём нет инверсии спектра */
		#define LO1_SIDE	LOCODE_LOWER	/* rf->baseband: При преобразовании на этом гетеродине нет инверсии спектра */
	#else /* XVTR_R820T2 */
		#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
		#define LO1_SIDE_F(freq) ((((freq) % DUCDDC_FREQ) >= (DUCDDC_FREQ / 2)) ? LOCODE_UPPER : LOCODE_LOWER)	/* rf->baseband: сторона зависит от рабочей частоты */
	#endif /* XVTR_R820T2 */
	//#define LO1_SIDE	LOCODE_LOWER	/* rf->baseband: При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO2_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#define LO4_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: при отсутствующем гетеродине - на нём нет инверсии спектра */

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */
	#define LO4_POWER2 0
	#define IF3_MODEL IF3_TYPE_BYPASS

	#if defined (TUNE_BOTTOM) && (TUNE_TOP)
	#elif BANDSELSTYLERE_UPCONV56M && XVTR_NYQ1
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define TUNE_TOP (DUCDDC_FREQ * 1 + 56000000L)		/* верхняя частота настройки */
		//#define TUNE_TOP 56000000L		/* верхняя частота настройки */
		#define NOXVRTUNE_TOP 56000000L		/* верхняя частота настройки без трансвертора */
	#elif BANDSELSTYLERE_UPCONV56M && XVTR_R820T2
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define NOXVRTUNE_TOP 56000000L		/* верхняя частота настройки без AD936x */
		#define XVRTUNE_BOTTOM 70000000L		/* нижняя частота настройки AD936x */
		#define TUNE_TOP 3000000000L		/* верхняя частота настройки */
	#elif BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define TUNE_TOP 56000000L		/* верхняя частота настройки */
	#elif BANDSELSTYLERE_UPCONV56M_45M	/* версия до 45 МГц */
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define TUNE_TOP 45000000L		/* верхняя частота настройки */
	#elif BANDSELSTYLERE_UPCONV56M_36M	/* версия до 36 МГц */
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define TUNE_TOP 36000000L		/* верхняя частота настройки */
	#elif BANDSELSTYLERE_UPCONV32M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz нижняя частота настройки */
		#define TUNE_TOP 32000000L		/* верхняя частота настройки */
	#elif BANDSELSTYLERE_UPCONV36M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz нижняя частота настройки */
		#define TUNE_TOP 36000000L		/* верхняя частота настройки */
	#elif BANDSELSTYLE_OLEG4Z
		#define TUNE_BOTTOM 20000L		/* 20 kHz нижняя частота настройки */
		#define TUNE_TOP 1700000000L	/* 1.7 GHz верхняя частота настройки */
	#elif BANDSELSTYLE_OLEG4Z_V2
		#define TUNE_BOTTOM 20000L		/* 20 kHz нижняя частота настройки */
		#define TUNE_TOP 1700000000L	/* 1.7 GHz верхняя частота настройки */
	#elif BANDSELSTYLERE_UHF_137M_174M
		#define TUNE_BOTTOM 137000000L		/* нижняя частота настройки */
		#define TUNE_TOP 174000000L			/* верхняя частота настройки */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

#endif /* FQMODEL_FPGA */

// В зависимости от частотной схемы - параметры математической части синтезатора

#if DIRECT_100M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	100000000
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_100M0_X1 */

#if DIRECT_125M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	125000000
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_125M0_X1 */

#if DIRECT_122M88_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	122880000
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_122M88_X1 */

#if DIRECT_96M_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	96000000
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_96M_X1 */

#if DIRECT_12M288_X10
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	12288000
	#define DDS1_CLK_MUL	10 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	10		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	10		/* Умножитель в DDS3 */
#endif	/* DIRECT_12M288_X10 */

#if DIRECT_160M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	160000000
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_160M0_X1 */

#if DIRECT_48M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	48000000
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_48M0_X1 */

#if DIRECT_50M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	50000000
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_50M0_X1 */

#if DIRECT_75M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	75000000
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_75M0_X1 */

#if DIRECT_80M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	80000000ul
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS2 */
#endif	/* DIRECT_80M0_X1 */

#if DIRECT_60M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	60000000ul
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS2 */
#endif	/* DIRECT_60M0_X1 */

#if DIRECT_48M0_X8
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	48000000ul
	#define DDS1_CLK_MUL	8 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_48M0_X8 */

#define IF3_TYPE_BYPASS	1

/* все возможные фильтры. Не ноль соответствующем бите IF3_FMASK разрешает включение/выключение данного фильтра. */
#define IF3_FMASK_0P3	(1U << 0)	/* наличие фильтра 0.3 кГц	*/
#define IF3_FMASK_0P5	(1U << 1)	/* наличие фильтра 0.5 кГц	*/
#define IF3_FMASK_1P0	(1U << 2)	/* наличие фильтра 1.0 кГц	*/
#define IF3_FMASK_1P5	(1U << 3)	/* наличие фильтра 1.5 кГц	*/
#define IF3_FMASK_1P8	(1U << 4)	/* наличие фильтра 1.8 кГц	*/
#define IF3_FMASK_2P1	(1U << 5)	/* наличие фильтра 2.1 кГц	*/
#define IF3_FMASK_2P4	(1U << 6)	/* наличие фильтра 2.4 кГц	*/
#define IF3_FMASK_2P7	(1U << 7)	/* наличие фильтра 2.7 кГц	*/
#define IF3_FMASK_3P1	(1U << 8)	/* наличие фильтра 3.1 кГц	*/
#define IF3_FMASK_6P0	(1U << 9)	/* наличие фильтра 6.0 кГц	*/
#define IF3_FMASK_7P8	(1U << 10)	/* наличие фильтра 7.8 кГц	*/
#define IF3_FMASK_8P0	(1U << 11)	/* наличие фильтра 8.0 кГц	*/
#define IF3_FMASK_9P0	(1U << 12)	/* наличие фильтра 9.0 кГц	*/
#define IF3_FMASK_15P0	(1U << 13)	/* наличие фильтра 15.0 кГц	*/
#define IF3_FMASK_17P0	(1U << 14)	/* наличие фильтра 17.0 кГц	*/
#define IF3_FMASK_120P0	(1U << 15)	/* наличие фильтра 120 кГц	*/

//extern uint_fast8_t s9level;		/* уровни калибровки S-метра */
//extern uint_fast8_t s9delta;		// 9 баллов - 8 интервалов - по 6 децибел каждый
//extern uint_fast8_t s9_60_delta;		// 60 dB
extern uint_fast16_t minforward;
extern uint_fast8_t swrcalibr;
extern uint_fast8_t maxpwrcali;
extern uint_fast8_t swrmode;

void updateboard(void);	/* полная перенастройка */


#define DBVALOFFSET_BASE WITHMINFSPOWER	// - 160

/* Беззнаковые значения, хранимые в NVRAM */
#define WITHTOPDBMIN 		(- 50 - DBVALOFFSET_BASE)
#define WITHTOPDBMAX 		(+ 20 - DBVALOFFSET_BASE)
#define WITHTOPDBDEFAULT 	(- 30 - DBVALOFFSET_BASE)
#define WITHTOPDBDEFAULTTX 	(0 - DBVALOFFSET_BASE)

/* Беззнаковые значения, хранимые в NVRAM */
#define WITHBOTTOMDBMIN 	(WITHMINFSPOWER - DBVALOFFSET_BASE)
#define WITHBOTTOMDBMAX 	(+ 10 - DBVALOFFSET_BASE)
#define WITHBOTTOMDBDEFAULT (- 130 - DBVALOFFSET_BASE)
#define WITHBOTTOMDBDEFAULTTX (- 110 - DBVALOFFSET_BASE)

/* выбор внешнего вида прибора - стрелочный или градусник */
enum
{
	SMETER_TYPE_BARS,
	SMETER_TYPE_DIAL,
	//
	SMETER_TYPE_COUNT
};

/* стиль отображения спектра и панорамы */
enum
{
	VIEW_LINE,		// ломаная линия
	VIEW_FILL,		// залитый зеленым спектр
	VIEW_COLOR,		// раскрашенный цветовым градиентом спектр
	VIEW_DOTS,		// точки без соединения между ними
#if WITHVIEW_3DSS
	VIEW_3DSS,		// дизайн панорамы под 3DSS Yaesu
#endif /* WITHVIEW_3DSS */
	//
	VIEW_count
};

/* Управление частичной полосоц отображением спектра/волопада */
enum
{
	SPECTRUMWIDTH_MULT = 11,	// 44 кГц
	SPECTRUMWIDTH_DENOM = 12,
};

const char * get_band_label3(unsigned b); /* получение человекопонятного названия диапазона */

#define NMICPROFCELLS	3


#define CWWPMMIN	4	// В ts-590s от 4-х, а не от 10 как в остальных kenwood
#define CWWPMMAX	60

uint_fast8_t edgepins_getptt(void);		/* Есть ли запрос на передачу с одного из источников */

uint_fast16_t get_swr(uint_fast16_t swr_fullscale);

void gnss_initialize(void);	/* инициализация парсера GNSS потока NMEA */
int board_islfmmode(void);
uint_fast8_t islfmstart(unsigned now);

void bt_initialize(void);

#define N7DDCTUNE_OK 0
#define N7DDCTUNE_ERROR 1	// Not enough power
#define N7DDCTUNE_ABORT 2	// Aborted by keyboard
int n7ddc_tune(int linearC, int linearL, int (* cb)(void * ctx), void * ctx);	// 1 if no power, 2 if aborted

void n7ddc_settuner(unsigned inductors, unsigned capcitors, unsigned type);	// Set tuner parameters, wait tuner ADC delay
unsigned n7ddc_get_swr(void);

#define SWRMIN 10	// минимум - соответствует SWR = 1.0, точность = 0.1
unsigned get_swr_cached(unsigned rangemax);

void gui_update(void);

typedef struct {
	char label [10][10];
} bws_t;

typedef enum {
	BAND_TYPE_HAM,
	BAND_TYPE_BROADCAST
} gui_band_type_t;

typedef struct {
	uint_fast32_t init_freq;
	uint_fast8_t index;
	gui_band_type_t type;
	char name[10];
} band_array_t;

typedef struct {
	char name[20];
	uint8_t index;
} menu_names_t;

// шаг изменения значения параметра
enum
{
	ISTEP_RO = 0,
	ISTEP1 = 1,
	ISTEP2 = 2,
	ISTEP3 = 3,
	ISTEP5 = 5,
	ISTEP10 = 10,
	ISTEP50 = 50,
	ISTEP100 = 100,
	ISTEPLARGE_1 = 255
	//ISTEPG,
	//
};

// особые случаи отображения значения параметра
enum
{
	RJ_BASE0,
	RJ_YES = 128,	/* значение в поле rj, при котором отображаем как Yes/No */
	RJ_ON,			/* значение в поле rj, при котором отображаем как On/Off */
	RJ_POW2,		/* параметр - степень двойки. Отображается результат */
	RJ_MDMSPEED,	/* параметр - скорость модема */
	RJ_MDMMODE,		/* параметр - тип модуляции модема */
	RJ_SIGNED,		/* отображние знакового числа (меню на втором валкодере) */
	RJ_UNSIGNED,		/* отображние знакового числа (меню на втором валкодере) */
	RJ_CB,			/* для получения текста есть callback функция */
	RJ_PERCENTS,	/* отображение в процентах от диапазона допустимых значений */
	//
	RJ_notused
};

// WSIGNFLAG
// qspecial bits
#define ITEM_VALUE	(UINT8_C(1) << 0)	/* пункт меню для редактирования параметра */
#define ITEM_GROUP	(UINT8_C(1) << 1)	/* пункт меню без изменяемого значения - связан с подменю */

#define ITEM_FILTERU	(UINT8_C(1) << 2)	/* пункт меню для подстройки частот фильтра ПЧ (высокочастотный скат) */
#define ITEM_FILTERL	(UINT8_C(1) << 3)	/* пункт меню для подстройки частот фильтра ПЧ (низкочастотный скат) */

#define ITEM_NOINITNVRAM	(UINT8_C(1) << 4)	/* значение этого пункта не используется при начальной инициализации NVRAM */
#define ITEM_LISTSELECT	(UINT8_C(1) << 5)	/* отображение этого элемента в lvgl списком значений на выбор */

#define QLABEL(s1) (s1), (s1), (s1)
#define QLABEL2(s1, s2) (s1), (s2), (s2)
#define QLABEL3(s1, s2, s3) (s1), (s2), (s3)
#define QLABELENC2(s1) (s1), (s1), (s1)

typedef struct paramdefdef
{
	const char * qlabel;		/* название пункта меню для компактного отображения */
	const char * label;			/* название пункта меню для широкого меню */
	const char * enc2label;		/* название пункта меню для FUNC меню */

	uint8_t qwidth_unused, qcomma, qrj;
	uint8_t qistep;
	uint8_t qspecial;	/* признак к какому меню относится */

	uint16_t qbottom, qupper;	/* ограничения на редактируемое значение (upper - включая) */

	nvramaddress_t qnvram;				/* Если MENUNONVRAM - только меняем в памяти */
	unsigned (* qselector)(unsigned * count);
	nvramaddress_t (* qnvramoffs)(nvramaddress_t base, unsigned sel);	/* Смещение при доступе к NVRAM. Нужно при работе с настройками специфическрми для диапазона например */
	ptrdiff_t (* valoffs)(unsigned sel);		/* индекс для работы с массивом переменных */

	uint_fast16_t * apval16;			/* переменная, которую подстраиваем - если она 16 бит */
	uint_fast8_t * apval8;			/* переменная, которую подстраиваем  - если она 8 бит*/
	int_fast32_t (* funcoffs)(void);	/* при отображении и использовании добавляется число отсюда */
	size_t (* getvaltext)(char * buff, size_t count, int_fast32_t value);	/* получить текст значения параметра - see RJ_CB */
} paramdefdef_t;

struct menudef
{
	const struct paramdefdef * pd;
};

// sorted version
const char * hamradio_change_view_style(uint_fast8_t v);
const char * hamradio_get_agc3_value(void);	// RX agc time - 3-х буквенные абревиатуры
const char * hamradio_get_agc4_value(void);	// RX agc time - 4-х буквенные абревиатуры
const char * hamradio_get_ant5_value(void);	// antenna
const char * hamradio_get_att_value_P(void);	// RX attenuator
const char * hamradio_get_hplp_value_P(void);	// HP/LP
const char * hamradio_get_mainsubrxmode3_value_P(void);	// текущее состояние DUAL WATCH
const char * hamradio_get_mode_a_value_P(void);	// SSB/CW/AM/FM/..
const char * hamradio_get_mode_b_value_P(uint_fast8_t * flag);	// SSB/CW/AM/FM/..
const char * hamradio_get_notchtype5_P(void);	// FREQ/ANOTCH
const char * hamradio_get_pre_value(void);	// RX preamplifier
const char * hamradio_get_preamp_value(void);
const char * hamradio_get_rxbw_label3(void);	// RX bandwidth - name
const char * hamradio_get_rxbw_value4(void);	// RX bandwidth - value
const char * hamradio_get_submode_label(uint8_t v);
const char * hamradio_get_vfomode3_value(uint_fast8_t * flag);	// VFO mode
const char * hamradio_get_vfomode5_value(uint_fast8_t * flag);	// VFO mode
const char * hamradio_gui_edit_menu_item(uint_fast16_t index, int_least16_t rotate);
const char * hamradio_midlabel5(uint_fast8_t section, uint_fast8_t * active);
const char * hamradio_midvalue5(uint_fast8_t section, uint_fast8_t * active);
int hamradio_walkmenu_getgroupanme(const void * groupitem, char * buff, size_t count);
int hamradio_walkmenu_getparamanme(const void * paramitem, char * buff, size_t count);
int hamradio_walkmenu_getparamvalue(const void * paramitem, char * buff, size_t count);
int_fast16_t hamradio_get_PAtemp_value(void);	// Градусы в десятых долях (может быть отрицательным). INT16_MAX - термодатчик не подключен
int_fast16_t hamradio_get_bottomdb(void);	/* значения со знаком */
int_fast16_t hamradio_get_pacurrent_value(void);	// Ток в десятках милиампер, может быть отрицательным
int_fast16_t hamradio_get_topdb(void);	/* значения со знаком */
int_fast16_t hamradio_getleft_bp(uint_fast8_t pathi);	/* получить левый (низкочастотный) скат полосы пропускания для отображения "шторки" на спектранализаторе */
int_fast16_t hamradio_getright_bp(uint_fast8_t pathi);	/* получить правый (высокочастотный) скат полосы пропускания для отображения "шторки" на спектранализаторе */
int_fast16_t hamradio_if_shift(int_fast8_t step);
int_fast32_t hamradio_get_pbtvalue(void);	// Для отображения на дисплее
int_fast32_t hamradio_getequalizerbase(void);
int_fast8_t hamradio_afresponce(int_fast8_t v);
uint32_t hamradio_get_gadcrand(void);
uint8_t hamradio_get_gbottomdb(void);
uint8_t hamradio_get_gtopdb(void);
uint8_t hamradio_get_spectrumpart(void);
uint_fast16_t hamradio_get_afgain(void);
uint_fast16_t hamradio_get_lfmstop100k(void);
uint_fast16_t hamradio_get_lfmtinterval(void);
uint_fast16_t hamradio_get_lfmtoffset(void);
uint_fast16_t hamradio_get_multilinemenu_block_groups(menu_names_t * vals);
uint_fast16_t hamradio_get_multilinemenu_block_params(menu_names_t * vals, uint_fast8_t index, uint_fast8_t max_count);
uint_fast16_t hamradio_notch_freq(int_fast8_t step);
uint_fast16_t hamradio_notch_width(int_fast8_t step);
uint_fast32_t hamradio_get_freq_a(void);		// Частота VFO A для отображения на дисплее
uint_fast32_t hamradio_get_freq_b(void);		// Частота VFO B для отображения на дисплее
uint_fast32_t hamradio_get_freq_pathi(uint_fast8_t pathi);		// Частота VFO A/B для отображения на дисплее
uint_fast32_t hamradio_get_freq_rx(void);		// Частота VFO A для маркировки файлов
uint_fast32_t hamradio_get_modem_baudrate100(void);	// скорость передачи BPSK * 100
uint_fast32_t hamradio_load_memory_cells(uint_fast8_t cell, uint_fast8_t set);
uint_fast8_t hamradio_change_nr(uint_fast8_t v);
uint_fast8_t hamradio_change_preamp(uint_fast8_t v);
uint_fast8_t hamradio_check_current_freq_by_band(uint_fast8_t band);
uint_fast8_t hamradio_get_agc_type(void);
uint_fast8_t hamradio_get_amfm_highcut10_value(uint_fast8_t * flag);	// текущее значение верхней частоты среза НЧ фильтра АМ/ЧМ (в десятках герц)
uint_fast8_t hamradio_get_antivox_level(void);
uint_fast8_t hamradio_get_att_db(void);
uint_fast8_t hamradio_get_att_dbs(uint_fast8_t * values, uint_fast8_t limit);
uint_fast8_t hamradio_get_atuvalue(void);
uint_fast8_t hamradio_get_bands(band_array_t * bands, uint_fast8_t count_only, uint_fast8_t is_bcast_need);
uint_fast8_t hamradio_get_bkin_value(void);
uint_fast8_t hamradio_get_blinkphase(void);	/* состояние для мерцающих индикаторов на диспле */
uint_fast8_t hamradio_get_bp_type_wide(void);	// не-0: параметры полосы пропускания - пара нижний срез/верхний срез
uint_fast8_t hamradio_get_bringENC1F(void);	// Вращали 3-й валкодер (ENC1F)
uint_fast8_t hamradio_get_bringENC2F(void);	// Вращали 4-й валкодер (ENC2F)
uint_fast8_t hamradio_get_bringENC3F(void);	// Вращали 5-й валкодер (ENC3F)
uint_fast8_t hamradio_get_bringENC4F(void);	// Вращали 6-й валкодер (ENC4F)
uint_fast8_t hamradio_get_bringSWR(const char * * label);	// Была ошибка SWR или что-то другое
uint_fast8_t hamradio_get_bringtuneA(void);	// Разрешить отображение текущей частоты на водопаде
uint_fast8_t hamradio_get_bringtuneB(void);	// Разрешить отображение текущей частоты на водопаде
uint_fast8_t hamradio_get_bws(bws_t * bws, uint_fast8_t limit);
uint_fast8_t hamradio_get_bypvalue(void);
uint_fast8_t hamradio_get_classa(void);
uint_fast8_t hamradio_get_ctcss_active(uint_fast32_t * freq);
uint_fast8_t hamradio_get_cw_wpm(void);
uint_fast8_t hamradio_get_datamode(void);	// источник звука для передачи - USB AUDIO
uint_fast8_t hamradio_get_ft8cn(void);
uint_fast8_t hamradio_get_genham_value(void);
uint_fast8_t hamradio_get_gmikeagc(void);
uint_fast8_t hamradio_get_gmikeagcgain(void);
uint_fast8_t hamradio_get_gmikeboost20db(void);
uint_fast8_t hamradio_get_gmikeequalizer(void);
uint_fast8_t hamradio_get_gmikeequalizerparams(uint_fast8_t i);
uint_fast8_t hamradio_get_gmikehclip(void);
uint_fast8_t hamradio_get_gmoniflag(void);
uint_fast8_t hamradio_get_gmutespkr(void);
uint_fast8_t hamradio_get_gnotch(void);
uint_fast8_t hamradio_get_gnotchtype(void);
uint_fast8_t hamradio_get_greverb(void);
uint_fast8_t hamradio_get_gsmetertype(void);
uint_fast8_t hamradio_get_gvfoab(void);
uint_fast8_t hamradio_get_gvoxenable(void);
uint_fast8_t hamradio_get_gzoomxpow2(void);
uint_fast8_t hamradio_get_high_bp(int_least16_t rotate);
uint_fast8_t hamradio_get_lfmmode(void);
uint_fast8_t hamradio_get_lockvalue(void);	// текущее состояние LOCK
uint_fast8_t hamradio_get_low_bp(int_least16_t rotate);
uint_fast8_t hamradio_get_mik1level(void);
uint_fast8_t hamradio_get_notchvalue(int_fast32_t * p);		// Notch filter ON/OFF
uint_fast8_t hamradio_get_nrvalue(int_fast32_t * p);		// NR ON/OFF
uint_fast8_t hamradio_get_rec_value(void);	// AUDIO recording state
uint_fast8_t hamradio_get_reverb_delay(void);
uint_fast8_t hamradio_get_reverb_loss(void);
uint_fast8_t hamradio_get_samdelta10(int_fast32_t * p, uint_fast8_t pathi);		/* Получить значение отклонения частоты с точностью 0.1 герца */
uint_fast8_t hamradio_get_spkon_value(void);	// не-0: динамик включен
uint_fast8_t hamradio_get_submode(void);
uint_fast8_t hamradio_get_tunemodevalue(void);	// текущее состояние TUNE
uint_fast8_t hamradio_get_tx(void);
uint_fast8_t hamradio_get_tx_power(void);
uint_fast8_t hamradio_get_tx_tune_power(void);
uint_fast8_t hamradio_get_usbbth_active(void);	// usb bt
uint_fast8_t hamradio_get_usbmsc_active(void);	// usb storage
uint_fast8_t hamradio_get_usefastvalue(void);	// текущее состояние FAST
uint_fast8_t hamradio_get_viewstyle(void);
uint_fast8_t hamradio_get_volt_value(void);	// Вольты в десятых долях
uint_fast8_t hamradio_get_vox_delay(void);
uint_fast8_t hamradio_get_vox_level(void);
uint_fast8_t hamradio_get_voxvalue(void);	// текущее состояние VOX
uint_fast8_t hamradio_load_mic_profile(uint_fast8_t cell, uint_fast8_t set);
uint_fast8_t hamradio_moxmode(uint_fast8_t v);
uint_fast8_t hamradio_set_freq(uint_fast32_t freq);
uint_fast8_t hamradio_split_toggle(void);
uint_fast8_t hamradio_tunemode(uint_fast8_t v);
uint_fast8_t hamradio_verify_freq_bands(uint_fast32_t freq, uint_fast32_t * bottom, uint_fast32_t * top);
unsigned hamradio_get_getsoffreq(void);
unsigned hamradio_get_pwr(void);
void hamradio_change_att(void);
void hamradio_change_submode(uint_fast8_t newsubmode, uint_fast8_t need_correct_freq);
void hamradio_clean_memory_cells(uint_fast8_t i);
void hamradio_clean_mic_profile(uint_fast8_t cell);
void hamradio_disable_encoder2_redirect (void);
void hamradio_disable_keyboard_redirect(void);
void hamradio_enable_encoder2_redirect (void);
void hamradio_enable_keyboard_redirect(void);
void hamradio_get_antivox_delay_limits(uint_fast8_t * min, uint_fast8_t * max);
int hamradio_get_label_ENC1F(uint_fast8_t active, char * buff, size_t count);	/* получить надпись для отображения состояния ENC1F */
int hamradio_get_label_ENC2F(uint_fast8_t active, char * buff, size_t count);	/* получить надпись для отображения состояняя ENC2F */
int hamradio_get_label_ENC3F(uint_fast8_t active, char * buff, size_t count);	/* получить надпись для отображения состояния ENC3F */
int hamradio_get_label_ENC4F(uint_fast8_t active, char * buff, size_t count);	/* получить надпись для отображения состояния ENC4F */
void hamradio_get_mic_agc_limits(uint_fast8_t * min, uint_fast8_t * max);
void hamradio_get_mic_clip_limits(uint_fast8_t * min, uint_fast8_t * max);
void hamradio_get_mic_level_limits(uint_fast8_t * min, uint_fast8_t * max);
void hamradio_get_multilinemenu_block_vals(menu_names_t * vals, uint_fast8_t index, uint_fast8_t cnt);
void hamradio_get_nmea_time(char * p, size_t sz);
void hamradio_get_reverb_delay_limits(uint_fast8_t * min, uint_fast8_t * max);
void hamradio_get_reverb_loss_limits(uint_fast8_t * min, uint_fast8_t * max);
void hamradio_get_tx_power_limits(uint_fast8_t * min, uint_fast8_t * max);
void hamradio_get_vox_delay_limits(uint_fast8_t * min, uint_fast8_t * max);
void hamradio_get_vox_level_limits(uint_fast8_t * min, uint_fast8_t * max);
void hamradio_goto_band_by_freq(uint_fast32_t f);
void hamradio_gui_enc2_update(void);
void hamradio_lfm_disable(void);
void hamradio_load_gui_settings(void * ptr);
void hamradio_save_gui_settings(const void * ptr);
void hamradio_save_memory_cells(uint_fast8_t i);
void hamradio_save_mic_profile(uint_fast8_t cell);
void hamradio_set_afgain(uint_fast16_t v);
void hamradio_set_agc_fast(void);
void hamradio_set_agc_slow(void);
void hamradio_set_antivox_level(uint_fast8_t v);
void hamradio_set_att_db(uint_fast8_t db);
void hamradio_set_bottomdb(int_fast16_t v);	/* значения со знаком */
void hamradio_set_bw(uint_fast8_t v);
void hamradio_set_gbottomdb(uint8_t v);
void hamradio_set_gdactest(uint8_t v);
void hamradio_set_gmikeagc(uint_fast8_t v);
void hamradio_set_gmikeagcgain(uint_fast8_t v);
void hamradio_set_gmikeboost20db(uint_fast8_t v);
void hamradio_set_gmikeequalizer(uint_fast8_t v);
void hamradio_set_gmikeequalizerparams(uint_fast8_t i, uint_fast8_t v);
void hamradio_set_gmikehclip(uint_fast8_t v);
void hamradio_set_gmoniflag(uint_fast8_t v);
void hamradio_set_gmutespkr(uint_fast8_t v);
void hamradio_set_gnotch(uint_fast8_t v);
void hamradio_set_gnotchtype(uint_fast8_t v);
void hamradio_set_greverb(uint_fast8_t v);
void hamradio_set_gtopdb(uint8_t v);
void hamradio_set_gvoxenable(uint_fast8_t v);
void hamradio_set_gzoomxpow2(uint_fast8_t v);
void hamradio_set_lfmmode(uint_fast8_t v);
void hamradio_set_lfmstop100k(uint_fast16_t v);
void hamradio_set_lfmtinterval(uint_fast16_t v);
void hamradio_set_lfmtoffset(uint_fast16_t v);
void hamradio_set_lock (uint_fast8_t lock);
void hamradio_set_mik1level(uint_fast8_t v);
void hamradio_set_moxmode(uint_fast8_t mode);
void hamradio_set_reverb_delay(uint_fast8_t v);
void hamradio_set_reverb_loss(uint_fast8_t v);
void hamradio_set_spectrumpart(uint8_t v);
void hamradio_set_topdb(int_fast16_t v);	/* значения со знаком */
void hamradio_set_tune(uint_fast8_t v);
void hamradio_set_tx_power(uint_fast8_t v);
void hamradio_set_tx_tune_power(uint_fast8_t v);
void hamradio_set_vox_delay(uint_fast8_t v);
void hamradio_set_vox_level(uint_fast8_t v);
void hamradio_setautotune(void);
void hamradio_setrx(void);
void hamradio_settemp_viewstyle(uint_fast8_t v);
void hamradio_split_mode_toggle(void);
void hamradio_split_vfo_swap(void);
void hamradio_tick_sof(void);
void hamradio_walkmenu(void * walkctx, void * (* groupcb)(void * walkctx, const void * groupitem), void (* itemcb)(void * walkctx, void * groupctx, const void * paramitem));

// LVGL interface functions
int infocb_modea(char * b, size_t len, int * pstate);
int infocb_modeb(char * b, size_t len, int * pstate);
int infocb_freqa(char * b, size_t len, int * pstate);
int infocb_freqb(char * b, size_t len, int * pstate);
int infocb_ant5(char * b, size_t len, int * pstate);
int infocb_preamp_ovf(char * b, size_t len, int * pstate);
int infocb_tune(char * b, size_t len, int * pstate);
int infocb_bypass(char * b, size_t len, int * pstate);
int infocb_rxbw(char * b, size_t len, int * pstate);
int infocb_rxbwval(char * b, size_t len, int * pstate);
int infocb_voltlevel(char * b, size_t len, int * pstate);
int infocb_datetime12(char * b, size_t len, int * pstate);
int infocb_currlevel(char * b, size_t len, int * pstate);
int infocb_thermo(char * b, size_t len, int * pstate);
int infocb_siglevel(char * b, size_t len, int * pstate);
int infocb_attenuator(char * b, size_t len, int * pstate);
int infocb_bkin(char * b, size_t len, int * pstate);
int infocb_wpm(char * b, size_t len, int * pstate);
int infocb_spk(char * b, size_t len, int * pstate);
int infocb_rxbw(char * b, size_t len, int * pstate);
int infocb_rec(char * b, size_t len, int * pstate);
int infocb_usbact(char * b, size_t len, int * pstate);
int infocb_btact(char * b, size_t len, int * pstate);
int infocb_nr(char * b, size_t len, int * pstate);
int infocb_classa(char * b, size_t len, int * pstate);
int infocb_datamode(char * b, size_t len, int * pstate);
int infocb_voxtune(char * b, size_t len, int * pstate);
int infocb_vfomode(char * b, size_t len, int * pstate);
int infocb_lock(char * b, size_t len, int * pstate);
int infocb_agc(char * b, size_t len, int * pstate);
int infocb_notch(char * b, size_t len, int * pstate);
int infocb_txrx(char * b, size_t len, int * pstate);

size_t
param_format(
	const struct paramdefdef * pd,
	char * buff,
	size_t count,	// размер буфера
	int_fast32_t value
	);
size_t
param_formatpercents(
	const struct paramdefdef * pd,
	char * buff,
	size_t count,	// размер буфера
	int_fast32_t value
	);
size_t
param_formatabel(
	const struct paramdefdef * pd,
	char * buff,
	size_t count,	// размер буфера
	const char * (* getlabel)(const struct paramdefdef * pd)	// функция получения указателя на строку с названием параметра
	);
void
param_setvalue(
	const struct paramdefdef * pd,
	int_fast32_t v
	);
int_fast32_t
param_getvalue(
	const struct paramdefdef * pd
	);
void
param_load(
	const struct paramdefdef * pd
	);


extern unsigned volatile nmeamgloop_status, nmeamgloop_position;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RADIO_H_INCLUDED */



