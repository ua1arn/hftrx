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


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined (WITHUSBUACIN) && defined (WITHUSBUACOUT)
	#edfine WITHUSBUAC 1
#elif defined (WITHUSBUACIN)
	#edfine WITHUSBUAC 1
#elif defined (WITHUSBUACOUT)
	#edfine WITHUSBUAC 1
#elif WITHUSBUAC
	#define WITHUSBUACIN	1
	#define WITHUSBUACOUT	1
#endif /* WITHUSBUAC */

typedef uint_least64_t ftw_t;	/* тип, подходящий по размерам для хранения промежуточных результатов вычислений */
typedef uint_least64_t phase_t;

#define CWPITCHSCALE 10

#if WITHBBOX && defined (WITHBBOXFREQ)
	#define DEFAULTDIALFREQ	WITHBBOXFREQ
#elif ! defined (DEFAULTDIALFREQ)
	#define DEFAULTDIALFREQ	7012000L
#endif

#define WITHMAXRXTXDELAY	100
#define WITHMAXTXRXDELAY	100

#define WITHNOTCHFREQMIN	300
#define WITHNOTCHFREQMAX	5000

/* параметры отображения панорамы */
#define WITHTOPDBMIN 0
#define WITHTOPDBMAX 60
#define WITHTOPDBDEFAULT 30

#define WITHBOTTOMDBMIN 80
#define WITHBOTTOMDBMAX 160

#if WITHBOTTOMDBVAL
#define WITHBOTTOMDBDEFAULT WITHBOTTOMDBVAL
#else
#define WITHBOTTOMDBDEFAULT 130
#endif /* WITHBOTTOMDBVAL */

enum
{
	BOARD_WTYPE_BLACKMAN_HARRIS,
	BOARD_WTYPE_BLACKMAN_HARRIS_MOD,
	BOARD_WTYPE_BLACKMAN_HARRIS_3TERM,
	BOARD_WTYPE_BLACKMAN_HARRIS_3TERM_MOD,
	BOARD_WTYPE_BLACKMAN_HARRIS_7TERM,
	BOARD_WTYPE_BLACKMAN_NUTTALL,
	BOARD_WTYPE_HAMMING,
	BOARD_WTYPE_HANN,
	BOARD_WTYPE_RECTANGULAR,
	//
	BOARD_WTYPE_count
};

#define BOARD_WTYPE_FILTERS BOARD_WTYPE_BLACKMAN_HARRIS_MOD
#define BOARD_WTYPE_SPECTRUM BOARD_WTYPE_HAMMING



#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_DCRX) 
	#if WITHIFSHIFT
		#error Can not be defined WITHIFSHIFT together with FQMODEL_DCTRX
	#endif
	#if WITHPBT
		#error Can not be defined WITHPBT together with FQMODEL_DCTRX
	#endif
	#if WITHDUALBFO
		#error Can not be defined WITHDUALBFO together with FQMODEL_DCTRX
	#endif
	#if WITHFIXEDBFO
		#error Can not be defined WITHFIXEDBFO together with FQMODEL_DCTRX
	#endif
	#if WITHDUALFLTR
		#error Can not be defined WITHDUALFLTR together with FQMODEL_DCTRX
	#endif
#endif

#if WITHPOTGAIN	// Для совместимости с теми конфигурациями, где разрешются регулировки только парой
	#define WITHPOTIFGAIN		1	/* регуляторы усиления ПЧ на потенциометрах */
	#define WITHPOTAFGAIN		1	/* регуляторы усиления НЧ на потенциометрах */
#endif /* WITHPOTGAIN */

#if ELKEY328
	#define CWWPMMIN	12 //328 10
	#define CWWPMMAX	30 //328 60
#else
	#define CWWPMMIN	4	// В ts-590s от 4-х, а не от 10 как в остальных kenwood
	#define CWWPMMAX	60
#endif

#define	BOARD_IFGAIN_MIN	0		/* код управления усилением ВЧ тракта */
#define	BOARD_IFGAIN_MAX	255		/* код управления усилением ВЧ тракта */

#define	BOARD_AFGAIN_MIN	0		/* код управления усилением НЧ тракта */
#define	BOARD_AFGAIN_MAX	255		/* код управления усилением НЧ тракта */

#define WITHLINEINGAINMIN	0		/* код управления усилением входа с линии */
#define WITHLINEINGAINMAX	255		/* код управления усилением входа с линии */

#define WITHMIKEINGAINMIN	0		/* код управления усилением входа с микрофона */
#define WITHMIKEINGAINMAX	255		/* код управления усилением входа с микрофона */

#define WITHMIKECLIPMIN		0
#define WITHMIKECLIPMAX		90

#define WITHMIKEAGCMIN		10
#define WITHMIKEAGCMAX		60

#define WITHVOXDELAYMIN		10
#define WITHVOXDELAYMAX		250

#define WITHVOXLEVELMIN		0
#define WITHVOXLEVELMAX		100

#define WITHAVOXLEVELMIN	0
#define WITHAVOXLEVELMAX	100

//#define WITHFILTSOFTMIN		0		/* код управления сглаживанием скатов фильтра основной селекции на приёме */
//#define WITHFILTSOFTMAX		100		/* код управления сглаживанием скатов фильтра основной селекции на приёме */

#define WITHREVERBDELAYMIN 5	/* минимальная задержка ревербератора (ms) */
#define WITHREVERBDELAYMAX 200	/* максимальная задержка ревербератора (ms) */

#define WITHREVERBLOSSMIN 6		/* минимальное ослабление на возврате ревербератора (dB) - кратно ISTEP3 */
#define WITHREVERBLOSSMAX 39	/* максимальная ослабление на возврате ревербератора (dB) - кратно ISTEP3 */

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
	#define WITHSUBTONES		1	/* выполняется формирование субтона при передаче NFM */
	#define WITHSAM				1	/* synchronous AM demodulation */
	#define WITHIFSHIFT			1	/* используется IF SHIFT */
	#define WITHMIC1LEVEL		1	/* установка усиления микрофона */

	#define	SQUELCHMAX	255	/* Kenwood's value */
#endif /* WITHIF4DSP */

#define CWPITCHMIN10	(100 / 10) // (400 / 10) kenwood min value 400 Hz
#define CWPITCHMAX10	(1900 / 10)

#if LO3_SIDE == LOCODE_INVALID
	//#error PBT can be used only with triple conversion schemes
#endif

#ifndef DDS2_CLK_DIV
	#define DDS2_CLK_DIV	1		/* Делитель опорной частоты перед подачей в DDS2 */
#endif

extern uint_fast8_t glob_agc;
extern uint_fast8_t	glob_loudspeaker_off;

enum
{
	DSPCTL_OFFSET_MODEA,
	DSPCTL_OFFSET_MODEB,
	DSPCTL_OFFSET_AFGAIN_HI,
	DSPCTL_OFFSET_AFGAIN_LO,
	DSPCTL_OFFSET_AFMUTE,
	DSPCTL_OFFSET_IFGAIN_HI,
	DSPCTL_OFFSET_IFGAIN_LO,
	DSPCTL_OFFSET_AGCOFF,
	DSPCTL_OFFSET_AGC_T1,
	DSPCTL_OFFSET_AGC_T2,
	DSPCTL_OFFSET_AGC_T4,
	DSPCTL_OFFSET_AGC_THUNG,
	DSPCTL_OFFSET_AGCRATE,

	DSPCTL_OFFSET_MICLEVEL_HI,
	DSPCTL_OFFSET_MICLEVEL_LO,

	DSPCTL_OFFSET_CWEDGETIME,
	DSPCTL_OFFSET_SIDETONELVL,			// уровень самоконтроля/озвучки клавиш

	DSPCTL_OFFSET_LOWCUTRX_HI,		// иижняя частота среза аудио фильтра
	DSPCTL_OFFSET_LOWCUTRX_LO,		// иижняя частота среза аудио фильтра
	DSPCTL_OFFSET_HIGHCUTRX_HI,		// верхняя частота среза аудио фильтра
	DSPCTL_OFFSET_HIGHCUTRX_LO,		// верхняя частота среза аудио фильтра

	DSPCTL_OFFSET_LOWCUTTX_HI,		// иижняя частота среза аудио фильтра
	DSPCTL_OFFSET_LOWCUTTX_LO,		// иижняя частота среза аудио фильтра
	DSPCTL_OFFSET_HIGHCUTTX_HI,		// верхняя частота среза аудио фильтра
	DSPCTL_OFFSET_HIGHCUTTX_LO,		// верхняя частота среза аудио фильтра

	DSPCTL_OFFSET_NOTCH_MODE,
	DSPCTL_OFFSET_NOTCH_WIDTH_HI,
	DSPCTL_OFFSET_NOTCH_WIDTH_LO,
	DSPCTL_OFFSET_NOTCH_FREQ_HI,
	DSPCTL_OFFSET_NOTCH_FREQ_LO,
	DSPCTL_OFFSET_LO6_FREQ_HI,
	DSPCTL_OFFSET_LO6_FREQ_LO,
	DSPCTL_OFFSET_FULLBW6_HI,
	DSPCTL_OFFSET_FULLBW6_LO,
	DSPCTL_OFFSET_CWPITCH_HI,
	DSPCTL_OFFSET_CWPITCH_LO,
	DSPCTL_OFFSET_DIGIGAINMAX,
	//DSPCTL_OFFSET_FLTSOFTER,
	DSPCTL_OFFSET_AMDEPTH,
	DSPCTL_OFFSET_MIKEAGC,
	DSPCTL_OFFSET_MIKEHCLIP,

	// Это имя всегда последнее - размер буфера
	DSPCTL_BUFSIZE
};

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


uint_fast8_t board_getsmeter(uint_fast8_t * tracemax, uint_fast8_t minval, uint_fast8_t maxval, uint_fast8_t clean);	/* получить значение от АЦП s-метра */

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
	const FLASHMEM char * labelf3;	/* name of filter - 3 chars width */
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
getif3byedge(
	const filter_t * workfilter,
	uint_fast8_t mode,			/* код семейства режима работы */
	uint_fast8_t mix4lsb,		/* формируем гетеродин для указанной боковой полосы */
	uint_fast8_t tx,			/* для режима передачи - врежиме CW - смещения частоты не требуется. */
	uint_fast8_t gcwpitch10
	);

int_fast32_t
getif3filtercenter(
	const filter_t * workfilter
	);

/* = IF3FREQBASE (для фильтра) */
int_fast32_t getlo4baseflt(
	const filter_t * workfilter 
	);

uint_fast8_t calc_next(uint_fast8_t v, uint_fast8_t low, uint_fast8_t high);

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
#if WITHUSBUACOUT
	BOARD_TXAUDIO_USB,	// "USB AUDIO",
#endif /* WITHUSBUACOUT */
	BOARD_TXAUDIO_2TONE,	// "2TONE",
	BOARD_TXAUDIO_NOISE,	// "NOISE",
	BOARD_TXAUDIO_1TONE,	// "1TONE",
	BOARD_TXAUDIO_MUTE,		// no signal to tx
	//
	BOARD_TXAUDIO_count
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
void spool_lfm(void);	// возврат не-0, если включён режим LFM
void lfm_run(void);
void hardware_lfm_setupdatefreq(unsigned ticksfreq);
void hardware_lfm_timer_initialize(void);

/* подготовка работы задержек переключения приём-передача */
void vox_initialize(void);
void vox_enable(uint_fast8_t voxstate, uint_fast8_t vox_delay_tens);	/* разрешение (не-0) или запрещение (0) работы vox. */
void vox_set_levels(uint_fast8_t level, uint_fast8_t alevel);	/* установить уровень срабатывания vox и anti-vox */
void vox_probe(uint_fast8_t vlevel, uint_fast8_t alevel);		/* предъявить для проверки отдетектированный уровень сигнала и anti-vox */

void seq_set_bkin_enable(uint_fast8_t bkinstate, uint_fast8_t bkin_delay_tens);	/* разрешение (не-0) или запрещение (0) работы параметры BREAK-IN. */
void seq_set_cw_enable(uint_fast8_t state);	/* разрешение (не-0) или запрещение (0) работы qsk. означает работу CW */
void seq_set_rgbeep(uint_fast8_t state);	/* разрешение (не-0) или запрещение (0) формирования roger beep */

// Для управления трактом надо задать управляющие слова для следующих состояний:
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
//#define TXGFV_RX		0
//#define TXGFV_TRANS		0			// переход между режимами приёма и передачи
//#define TXGFV_TX_SSB	TXPATH_BIT_ENABLE_SSB
//#define TXGFV_TX_CW		TXPATH_BIT_ENABLE_CW

//	portholder_t txgfva [TXGFI_SIZE];	// усостояния выходов для разных режимов
//	uint_fast8_t sdtnva [TXGFI_SIZE];	// признаки включения самоконтроля для разных режимов

//	{ TXGFV_RX, TXGFV_TRANS, TXGFV_TRANS, TXGFV_TX_CW }, // для CW
//	{ 0, 0, 0, 1 },	// для CW

//	{ TXGFV_RX, TXGFV_TRANS, TXGFV_TX_SSB, TXGFV_TX_SSB } // для SSB
//	{ 0, 0, 0, 0 },	// для SSB

void seq_set_txgate_P(const portholder_t FLASHMEM * txgfp, const uint_fast8_t FLASHMEM * sdtnp);	/* как включать передатчик в данном режиме работы из прерываний */

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


/* электронный ключ */
void seq_initialize(void);
void elkey_initialize(void);
void board_set_wpm(uint_fast8_t wpm); /* обработка меню - установить скорость  */
void elkey_set_format(uint_fast8_t dashratio, uint_fast8_t spaceratio); /* обработка меню - установить отношениея тире к точке (в десятках процентов). */
void elkey_set_mode(uint_fast8_t mode, uint_fast8_t reverse);	/* режим электронного ключа - 0 - asf, 1 - paddle, 2 - keyer */
void elkey_set_slope(uint_fast8_t slope);	// скорость уменьшения длительности точки и паузы - имитация виброплекса
uint_fast8_t elkey_get_output(void);
uint_fast8_t elkey_getnextcw(void);	// Получение символа для передачи (только верхний регистр)

/* обработка меню - установить задержку пре переходе на передачу и обратно. */
void seq_set_rxtxdelay(
	uint_fast8_t rxtxdelay, 
	uint_fast8_t txrxdelay,
	uint_fast8_t pretxdelay
	);
// запрос из user-mode части программы на переход на передачу для tune.
void seq_txrequest(uint_fast8_t tune, uint_fast8_t ptt);

/* процедура возвращает из сиквенсора запрос на переключение на передачу в основную программу */
uint_fast8_t seq_get_txstate(void);
/* подтверждение от user-mode программы о том, что смена режима приём-передача осуществлена */
void seq_ask_txstate(
	uint_fast8_t tx);	/* 0 - периферия находимся в состоянии приёма, иначе - в состоянии передачи */

void seq_purge(void);	/* очистка запомненных нажатий до этого момента. Вызывается из user-mode программы */

uint_fast8_t hardware_getshutdown(void);	/* возвращаем запрос на выключение - от компаратора питания */

void hardware_elkey_timer_initialize(void);
void hardware_elkey_set_speed(uint_fast32_t ticksfreq);
void hardware_elkey_set_speed128(uint_fast32_t ticksfreq, int scale);
void hardware_elkey_ports_initialize(void); // Инициализация входов электронного ключа, входа CAT_DTR
void hardware_ptt_port_initialize(void);	// Инициализация входа PTT, входа CAT_RTS и TXDISABLE

uint_fast8_t hardware_elkey_getpaddle(uint_fast8_t reverse);
uint_fast8_t hardware_get_ptt(void);
uint_fast8_t hardware_get_tune(void);	/* запрос от внешней аппаратуры на переход в режим TUNE */
uint_fast8_t hardware_get_txdisable(void);

void hardware_txpath_initialize(void);
void hardware_txpath_set(portholder_t txpathstate);
void dsp_txpath_set(portholder_t txpathstate);

/* сиквенсор приём-передача - и по таймерным и по 1/ELKEY_DISCRETE точки */
void elkey_spool_dots(void);	/* электронный ключ - вызывается с периодом 1/ELKEY_DISCRETE от длительности точки. */
void spool_0p128(void);	// OPERA support
/* Коды описывающие преобразования спектра в тракте */
#define LOCODE_UPPER	0	/* При преобразовании на этом гетеродине происходит инверсия спектра */
#define LOCODE_LOWER	1	/* При преобразовании на этом гетеродине нет инверсии спектра */
#define LOCODE_TARGETED	2	/* Этот гетеродин управляется для получения требуемой боковой. Только один может быть таким. */
#define LOCODE_INVALID	3	/* Этого гетеродина (и сместеля) нет. */

/* Первая ПЧ - 10.7 MHz */
#if FQMODEL_FMRADIO

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO2_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#define LO4_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: при отсутствующем гетеродине - на нём нет инверсии спектра */

	#define WITHWFM 1
	#define LO1MODE_DIRECT	1
	#define	PLL1_FRACTIONAL_LENGTH	0	/* Целочисленный синтезатор */

	#define REFERENCE_FREQ		4000000UL	/* Опорная частота */
	#define SYNTH_R1			160				/* Шаг перестройки (частота сравнения) - 25 кГц */
	#define TSA6057_PLL1_RCODE	TSA6057_R25_160	/* параметр программирования TSA6057 */
	#define	DDS1_CLK_MUL	1

	#define	TUNE_BOTTOM	87000000L
	#define	TUNE_TOP	108500000L

	#define	WFM_IF1	10700000ul

	#define IF3_MODEL IF3_TYPE_455

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */

	//#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#define LO4_POWER2	0		/* если 0 - делителей в тракте третьего гетеродина перед смесителем нет. */

	#define SYNTHLOWFREQ 	(WFM_IF1 + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
	#define SYNTHTOPFREQ 	(WFM_IF1 + TUNE_TOP)		/* Highest frequency of 1-st LO */

	#if (HYBRID_NVFOS == 1)
		#define FREQ_DENOM 1000000
		#define FREQ_SCALE 1500000	
	#else
		#error HYBRID_NVFOS not defined
	#endif

#endif /* FQMODEL_FMRADIO */

/* Первая ПЧ - 73050 кГц, вторая - 455 кГц или 500 кГц */
#if FQMODEL_73050 || FQMODEL_73050_IF0P5

	#if FQMODEL_73050_IF0P5

		#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
		#define LO1_SIDE_F(freq) (((freq) < 73050000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* сторона зависит от рабочей частоты */
		#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#if WITHIF4DSP
			// архитектура для DSP на последней ПЧ
			#define LO4_SIDE	LOCODE_LOWER	/* параметр проверен по отсутствию помехи при приёме для FQMODEL_80455. При преобразовании на этом гетеродине нет инверсии спектра */
			#define LO5_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
		#else /* WITHIF4DSP */
			// обычная архитектура для слухового приёмв
			#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
			//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: при отсутствующем гетеродине - на нём нет инверсии спектра */
		#endif /* WITHIF4DSP */

		#if MODEL_DIRECT
			#define DIRECT_72M550_X5	1	/* DDS LO1, LO2 = 72595 kHz - for 73050 kHz filter from Kenwood */	
		#else
			#define HYBRID_72M550_10M7	1	/* Hybrid LO1, LO2 = 72595 kHz - for 73050 kHz filter from Kenwood */
		#endif

		#define IF3_MODEL IF3_TYPE_500

		//#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

		#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */
		#define LO2_PLL_R	1		/* опорный сигнал прямо используется как LO2 */
		#define LO2_PLL_N	1		/* опорный сигнал прямо используется как LO2  */

	#elif FQMODEL_73050

		#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
		#define LO1_SIDE_F(freq) (((freq) < 73050000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* сторона зависит от рабочей частоты */
		#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#if WITHIF4DSP
			// архитектура для DSP на последней ПЧ
			#define LO4_SIDE	LOCODE_LOWER	/* параметр проверен по отсутствию помехи при приёме для FQMODEL_80455. При преобразовании на этом гетеродине нет инверсии спектра */
			#define LO5_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
		#else /* WITHIF4DSP */
			// обычная архитектура для слухового приёмв
			#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
			//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: при отсутствующем гетеродине - на нём нет инверсии спектра */
		#endif /* WITHIF4DSP */

		#if MODEL_DIRECT
			#define DIRECT_72M595_X5	1	/* DDS LO1, LO2 = 72595 kHz - for 73050 kHz filter from Kenwood */	
		#else
			#define HYBRID_72M595_10M7	1	/* Hybrid LO1, LO2 = 72595 kHz - for 73050 kHz filter from Kenwood */
		#endif

		#define IF3_MODEL IF3_TYPE_455

		#define LO2_PLL_R	1		/* опорный сигнал прямо используется как LO2 */
		#define LO2_PLL_N	1		/* опорный сигнал прямо используется как LO2  */

		//#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

		#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */
	#else
		#error Config error
	#endif

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */
	#define LO4_POWER2 0

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define TUNE_TOP 56000000L		/* верхняя частота настройки */
		#define NOXVRTUNE_TOP 56000000L		/* верхняя частота настройки бещ трансвертора */
	#elif BANDSELSTYLERE_UPCONV56M_45M	/* версия до 45 МГц */
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define TUNE_TOP 45000000L		/* верхняя частота настройки */
		#define NOXVRTUNE_TOP 45000000L		/* верхняя частота настройки бещ трансвертора */
	#elif BANDSELSTYLERE_UPCONV56M_36M	/* версия до 36 МГц */
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define TUNE_TOP 36000000L		/* верхняя частота настройки */
		#define NOXVRTUNE_TOP 36000000L		/* верхняя частота настройки бещ трансвертора */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if (MODEL_DIRECT == 0)

		#define SYNTHLOWFREQ 	(73050000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(73050000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=73.050000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1099410      /* Full VCO range 73.080000..129.050000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#elif (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=73.050000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1152762      /* Full VCO range 73.080000..129.050000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#else
			#error HYBRID_NVFOS not defined
		#endif
	#endif


#endif /* FQMODEL_73050 */

/* Первая ПЧ - 70455 кГц, вторая - 455 кГц */
#if FQMODEL_70455

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	//#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO1_SIDE_F(freq) (((freq) < 70455000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* сторона зависит от рабочей частоты */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#if WITHIF4DSP
		// архитектура для DSP на последней ПЧ
		#define LO4_SIDE	LOCODE_LOWER	/* параметр проверен по отсутствию помехи при приёме для FQMODEL_80455. При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO5_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#else /* WITHIF4DSP */
		// обычная архитектура для слухового приёмв
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: при отсутствующем гетеродине - на нём нет инверсии спектра */
	#endif /* WITHIF4DSP */

	#if MODEL_DIRECT
		#define DIRECT_70M000_X5	1	/* DDS LO1, LO2 = 70000 kHz - for 70455 kHz filter from FT-1000 */	
	#else
		#define HYBRID_70M000_10M7	1	/* Hybrid LO1, LO2 = 70000 kHz - for 70455 kHz filter from FT-1000 */
	#endif

	#define IF3_MODEL IF3_TYPE_455

	#define LO2_PLL_R	1		/* опорный сигнал прямо используется как LO2 */
	#define LO2_PLL_N	1		/* опорный сигнал прямо используется как LO2  */

	//#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */
	#define LO4_POWER2 0

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define TUNE_TOP 56000000L		/* верхняя частота настройки */
		#define NOXVRTUNE_TOP 56000000L		/* верхняя частота настройки бещ трансвертора */
	#elif BANDSELSTYLERE_UPCONV56M_45M	/* версия до 45 МГц */
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define TUNE_TOP 45000000L		/* верхняя частота настройки */
	#elif BANDSELSTYLERE_UPCONV56M_36M	/* версия до 36 МГц */
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define TUNE_TOP 36000000L		/* верхняя частота настройки */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if (MODEL_DIRECT == 0)

		#define SYNTHLOWFREQ 	(70455000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(70455000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=70.455000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1102317      /* Full VCO range 70.485000..126.455000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#elif (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=70.455000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1157336      /* Full VCO range 70.485000..126.455000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#else
			#error HYBRID_NVFOS not defined

		#endif
	#endif


#endif /* FQMODEL_70455 */

/* Первая ПЧ - 70451.5 кГц, вторая - 9011 кГц */
#if FQMODEL_70451_9011k

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	//#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO1_SIDE_F(freq) (((freq) < 70455000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* сторона зависит от рабочей частоты */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#if WITHIF4DSP
		// архитектура для DSP на последней ПЧ
		#define LO4_SIDE	LOCODE_LOWER	/* параметр проверен по отсутствию помехи при приёме для FQMODEL_80455. При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO5_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#else /* WITHIF4DSP */
		// обычная архитектура для слухового приёмв
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: при отсутствующем гетеродине - на нём нет инверсии спектра */
	#endif /* WITHIF4DSP */

	#if MODEL_DIRECT
		#define DIRECT_61M440_X5	1	/* DDS LO1, LO2 = 61440 kHz */
	#else
		#define HYBRID_61440_10M7	1	/* Hybrid LO1, LO2 = 70000 kHz - for 70455 kHz filter from FT-1000 */
	#endif

	// Выбор ПЧ
	#define IF3_CUSTOM_CW_CENTER	(9011000L)
	#define IF3_CUSTOM_SSB_LOWER	(9011000L - 1500)
	#define IF3_CUSTOM_SSB_UPPER	(9011000L + 1500)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#define IF3_MODEL	IF3_TYPE_CUSTOM

	#define LO2_PLL_R	1		/* опорный сигнал прямо используется как LO2 */
	#define LO2_PLL_N	1		/* опорный сигнал прямо используется как LO2  */

	//#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */
	#define LO4_POWER2 0

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define TUNE_TOP 56000000L		/* верхняя частота настройки */
		#define NOXVRTUNE_TOP 56000000L		/* верхняя частота настройки бещ трансвертора */
	#elif BANDSELSTYLERE_UPCONV56M_45M	/* версия до 45 МГц */
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define TUNE_TOP 45000000L		/* верхняя частота настройки */
	#elif BANDSELSTYLERE_UPCONV56M_36M	/* версия до 36 МГц */
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define TUNE_TOP 36000000L		/* верхняя частота настройки */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if (MODEL_DIRECT == 0)

		#define SYNTHLOWFREQ 	(70455000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(70455000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=70.455000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1102317      /* Full VCO range 70.485000..126.455000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#elif (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=70.455000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1157336      /* Full VCO range 70.485000..126.455000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#else
			#error HYBRID_NVFOS not defined

		#endif
	#endif


#endif /* FQMODEL_70451_9011k */

/* RU6BK: 1-st if=10.7 MHz, 2-nd if=500k, fixed BFO */
#if FQMODEL_10M7_500K
	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	//#define WITHCLASSICSIDE	1	/* сторона первого гетеродина зависит от рабочей частоты */
	//#define LO1_SIDE_F(freq) (((freq) < 13950000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* сторона зависит от рабочей частоты */
	#define LO2_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
	//#define LO2_SIDE	LOCODE_LOWER	/* параметр проверен по отсутствию помехи при приёме для FQMODEL_80455. При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	//#define LO4_SIDE	LOCODE_TARGETED	/* при WITHFIXEDBFO используется значение из меню */
	#if WITHIF4DSP
		// архитектура для DSP на последней ПЧ
		#define LO4_SIDE	LOCODE_LOWER	/* параметр проверен по отсутствию помехи при приёме для FQMODEL_80455. При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO5_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#else /* WITHIF4DSP */
		// обычная архитектура для слухового приёмв
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: при отсутствующем гетеродине - на нём нет инверсии спектра */
	#endif /* WITHIF4DSP */
	#define WITHFIXEDBFO	1	/* Переворот боковых за счёт 2-го гетродина 10.2 MHz / 10.9 MHz */

	#define IF3_MODEL IF3_TYPE_500

	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define LO1FDIV_ADJ	1		/* если определено - подстройка делителя в тракте первого гетеродина разрешена через меню */
	#define LO1_POWER2	0		/* 0 - Начальное значение - без делителя */
	#define LO4_POWER2 0
	#define LO2_DDSGENERATED	1			// dds3 used as LO2 output

	#if (LO2_SIDE == LOCODE_LOWER)
		#define LO2_PLL_R	REFERENCE_FREQ		/* 1 Hz step */
		#define LO2_PLL_N	10200000uL		/* for 10.2 MHz LO2 */
	#elif (LO2_SIDE == LOCODE_UPPER)
		#define LO2_PLL_R	REFERENCE_FREQ		/* 1 Hz step */
		#define LO2_PLL_N	10700000uL		/* for 10.2 MHz LO2 */
	#endif

	#if BANDSELSTYLERE_LOCONV32M_NLB
		#define TUNE_BOTTOM 1500000L
		#define TUNE_TOP 32000000L
	#elif BANDSELSTYLERE_LOCONV32M
		#define TUNE_BOTTOM 100000L
		#define TUNE_TOP 32000000L
	#elif BANDSELSTYLE_LADVABEST
		#define TUNE_BOTTOM 1600000L			/* нижняя частота настройки */
		#define TUNE_TOP 56000000L
	#else
		#error Strange BANDSELSTYLExxx
	#endif

#endif /* FQMODEL_10M7_500K */

/* Первая ПЧ - 80455 кГц, вторая - 455 кГц */
#if FQMODEL_80455

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE_F(freq) (((freq) < 80455000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* сторона зависит от рабочей частоты */
	//#define LO1_SIDE_F(freq) (((freq) < 80455000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* сторона зависит от рабочей частоты */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#if WITHIF4DSP
		// архитектура для DSP на последней ПЧ
		#define LO4_SIDE	LOCODE_LOWER	/* параметр проверен по отсутствию помехи при приёме для FQMODEL_80455. При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO5_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#else /* WITHIF4DSP */
		// обычная архитектура для слухового приёмв
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: при отсутствующем гетеродине - на нём нет инверсии спектра */
	#endif /* WITHIF4DSP */

	#if MODEL_DIRECT
		#define DIRECT_80M0_X5	1	/* DDS LO1, LO2 = 80000 kHz - for 80455 kHz filter */	
	#else
		#define HYBRID_80M0_10M7	1	/* Hybrid LO1, LO2 = 80000 kHz - for 80455 kHz filter */
	#endif

	#define IF3_MODEL IF3_TYPE_455

	#define LO2_PLL_R	1		/* опорный сигнал прямо используется как LO2 */
	#define LO2_PLL_N	1		/* опорный сигнал прямо используется как LO2  */

	//#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */
	#define LO4_POWER2 0

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define TUNE_TOP 56000000L		/* верхняя частота настройки */
		#define NOXVRTUNE_TOP 56000000L		/* верхняя частота настройки бещ трансвертора */
	#elif BANDSELSTYLERE_UPCONV56M_45M	/* версия до 45 МГц */
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define TUNE_TOP 45000000L		/* верхняя частота настройки */
	#elif BANDSELSTYLERE_UPCONV56M_36M	/* версия до 36 МГц */
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define TUNE_TOP 36000000L		/* верхняя частота настройки */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if (MODEL_DIRECT == 0)

		#define SYNTHLOWFREQ 	(80455000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(80455000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=80.455000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1091974      /* Full VCO range 80.485000..136.455000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#elif (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=80.455000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1141086      /* Full VCO range 80.485000..136.455000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#else
			#error HYBRID_NVFOS not defined
		#endif
	#endif


#endif /* FQMODEL_80455 */

/* DUC/DDC. обработка квадратур в DSP процессоре */
#if FQMODEL_FPGA
	#define DUCDDC_FREQ (REFERENCE_FREQ * DDS1_CLK_MUL)		/* частота, которой тактируется NCO в DUC/DDC */
	/* Описание структуры преобразований частоты в тракте */

	#define R820T_IFFREQ 4063000uL
	#define R820T_REFERENCE (REFERENCE_FREQ / 4)	// 122.88 / 4 = 30.720 MHz
	//#define R820T_LOSTEP (R820T_REFERENCE / 16)	// 1.92 MHz
	#define R820T_LOSTEP (R820T_REFERENCE / 32)	// 920 kHz

	#if XVTR_R820T2
		#define LO0_SIDE_F(freq) ((freq) >= 50000000uL ? LOCODE_UPPER : LOCODE_INVALID)	/* при отсутствующем конверторе - на нём нет инверсии спектра */
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

	#if BANDSELSTYLERE_UPCONV56M && XVTR_NYQ1
		#define TUNE_BOTTOM 30000L		/* 30 kHz нижняя частота настройки */
		#define TUNE_TOP (DUCDDC_FREQ * 1 + 56000000L)		/* верхняя частота настройки */
		//#define TUNE_TOP 56000000L		/* верхняя частота настройки */
		#define NOXVRTUNE_TOP 56000000L		/* верхняя частота настройки без трансвертора */
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

#if FQMODEL_64455 || FQMODEL_64455_IF0P5

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE_F(freq) (((freq) < 64455000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* сторона зависит от рабочей частоты */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#if WITHIF4DSP
		// архитектура для DSP на последней ПЧ
		#define LO4_SIDE	LOCODE_LOWER	/* параметр проверен по отсутствию помехи при приёме для FQMODEL_80455. При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO5_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#else /* WITHIF4DSP */
		// обычная архитектура для слухового приёмв
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: при отсутствующем гетеродине - на нём нет инверсии спектра */
	#endif /* WITHIF4DSP */

	//#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define LO2_PLL_R	1		/* опорный сигнал прямо используется как LO2 */
	#define LO2_PLL_N	1		/* опорный сигнал прямо используется как LO2  */

	#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */

	/* схема частотообразования */
	#if FQMODEL_64455_IF0P5

		#if MODEL_DIRECT
			#define DIRECT_63M9_X6	1	// DDS1 clock = 383.730 MHz
		#elif HYBRID_13M
			#define HYBRID_63M9_13M0	1
		#else
			#define HYBRID_63M9_10M7	1
		#endif

		#define IF3_MODEL IF3_TYPE_500

	#elif FQMODEL_64455
		#if MODEL_DIRECT
			#define DIRECT_64M0_X6	1	// DDS1 clock = 384 MHz
		#elif HYBRID_13M
			#define HYBRID_64M0_13M0	1
		#else
			#define HYBRID_64M0_10M7	1
			//#define HYBRID_12M8_10M7	1
		#endif

		#define IF3_MODEL IF3_TYPE_455

	#else
		#error TODO: missing data for configuration
	#endif


	//#define LO1FDIV_ADJ	1		/* если определено - подстройка делителя в тракте первого гетеродина разрешена через меню */
	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 30000L		/* нижняя частота настройки */
		#define TUNE_TOP 56000000L		/* верхняя частота настройки */
		#define NOXVRTUNE_TOP 56000000L		/* верхняя частота настройки бещ трансвертора */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if (MODEL_DIRECT == 0)

		#define SYNTHLOWFREQ 	(64455000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(64455000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=64.455000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1109756      /* Full VCO range 64.485000..120.455000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#elif (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=64.455000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1169072      /* Full VCO range 64.485000..120.455000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#else
			#error HYBRID_NVFOS not defined
		#endif

	#endif

#endif // MODEL_64455

/* Первая ПЧ - 70200 кГц, вторая - 200 кГц (фильтр от приёмника EKD) */
#if FQMODEL_70200

	/* Описание структуры преобразований частоты в тракте */
	//#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE_F(freq) (((freq) < 70200000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* сторона зависит от рабочей частоты */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#if WITHIF4DSP
		// архитектура для DSP на последней ПЧ
		#define LO4_SIDE	LOCODE_LOWER	/* параметр проверен по отсутствию помехи при приёме для FQMODEL_80455. При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO5_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#else /* WITHIF4DSP */
		// обычная архитектура для слухового приёмв
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: при отсутствующем гетеродине - на нём нет инверсии спектра */
	#endif /* WITHIF4DSP */


	#define IF3_MODEL IF3_TYPE_200

	#define LO2_PLL_R	1		/* опорный сигнал прямо используется как LO2 */
	#define LO2_PLL_N	1		/* опорный сигнал прямо используется как LO2  */

	//#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */
	#define LO4_POWER2 0

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 100000L		/* нижняя частота настройки */
		#define TUNE_TOP 30000000L		/* верхняя частота настройки */
		#define NOXVRTUNE_TOP 30000000L		/* верхняя частота настройки бещ трансвертора */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if MODEL_DIRECT

		#define DIRECT_70M000_X5	1	/* DDS LO1, LO2 = 70000 kHz - for 70200 kHz filter from EKD */	

	#else /* MODEL_DIRECT */

		#define HYBRID_70M000_10M7	1	/* Hybrid LO1, LO2 = 70000 kHz - for 70200 kHz filter from EKD */

		#define SYNTHLOWFREQ 	(70200000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(70200000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=70.200000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1102612      /* Full VCO range 70.230000..126.200000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#elif (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=70.200000, tune: 0.030000..56.000000 */
			#define FREQ_SCALE 1157801      /* Full VCO range 70.230000..126.200000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#else
			#error HYBRID_NVFOS not defined
		#endif

	#endif /* MODEL_DIRECT */


#endif /* FQMODEL_70200 */

/* Первая ПЧ - 60.7 МГц, вторая - 10.7 МГц, третья - 500 кГц */
/* Первая ПЧ - 60.7 МГц, вторая - 10.7 МГц, третья - 200 кГц */
#if FQMODEL_60700_IF05 || FQMODEL_60700_IF02 || FQMODEL_60700_IF0455

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	//#define LO3_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_UPPER 	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
	#if WITHIF4DSP
		// архитектура для DSP на последней ПЧ
		#define LO4_SIDE	LOCODE_LOWER	/* параметр проверен по отсутствию помехи при приёме для FQMODEL_80455. При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO5_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#else /* WITHIF4DSP */
		// обычная архитектура для слухового приёмв
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: при отсутствующем гетеродине - на нём нет инверсии спектра */
	#endif /* WITHIF4DSP */
	
	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define LO2_PLL_R	1		/* опорный сигнал прямо используется как LO2 */
	#define LO2_PLL_N	1		/* опорный сигнал прямо используется как LO2  */

	#define	LO3_POWER2	0		/* 0 - нет делителя после генератора LO3 перед подачей на смеситель */
	//#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */


	#if FQMODEL_60700_IF05
		// Третья ПЧ - 500 кГц
		#if LO3_SIDE == LOCODE_UPPER
			#define LO3_PLL_R	500		/* Парметр для корректной работы математики */
			#define LO3_PLL_N	112		/* LO3 = 11.2 MHz */
		#else /* LO3_UPPER */
			#define LO3_PLL_R	500		/* Парметр для корректной работы математики */
			#define LO3_PLL_N	102		/* LO3 = 10.2 MHz */
		#endif /* LO3_UPPER */

		#define IF3_MODEL IF3_TYPE_500
	#elif FQMODEL_60700_IF02
		// Третья ПЧ - 200 кГц
		#if LO3_SIDE == LOCODE_UPPER
			#define LO3_PLL_R	500		/* Парметр для корректной работы математики */
			#define LO3_PLL_N	109		/* LO3 = 10.9 MHz */
		#else /* LO3_UPPER */
			#define LO3_PLL_R	500		/* Парметр для корректной работы математики */
			#define LO3_PLL_N	105		/* LO3 = 10.5 MHz */
		#endif /* LO3_UPPER */

		#define IF3_MODEL IF3_TYPE_200
	#elif FQMODEL_60700_IF0455
		// Третья ПЧ - 455 кГц
		#if LO3_SIDE == LOCODE_UPPER
			#define LO3_PLL_R	REFERENCE_FREQ		/* Парметр для корректной работы математики */
			#define LO3_PLL_N	11155000			/* LO3 = 11.155 MHz */
		#else /* LO3_UPPER */
			#define LO3_PLL_R	REFERENCE_FREQ		/* Парметр для корректной работы математики */
			#define LO3_PLL_N	10245000			/* LO3 = 10.245 MHz */
		#endif /* LO3_UPPER */

		#define IF3_MODEL IF3_TYPE_455
	#endif

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 30000L		/* нижняя частота настройки */
		#define TUNE_TOP 56000000L		/* верхняя частота настройки */
	#elif BANDSELSTYLERE_UPCONV32M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz нижняя частота настройки */
		#define TUNE_TOP 32000000L		/* верхняя частота настройки */
	#elif BANDSELSTYLERE_UPCONV36M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz нижняя частота настройки */
		#define TUNE_TOP 36000000L		/* верхняя частота настройки */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if (MODEL_DIRECT == 0)

		#define HYBRID_50M0_10M7	1	/*  */	

		#define SYNTHLOWFREQ 	(60700000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(60700000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (TUNE_BOTTOM == 30000L) && (TUNE_TOP == 56000000L)	// BANDSELSTYLERE_UPCONV56M
			#if (HYBRID_NVFOS == 6)
				/* 6 VCOs, IF=60.700000, tune: 0.030000..56.000000 */
				#define FREQ_SCALE 1115007      /* Full VCO range 60.730000..116.700000 */
				#define FREQ_DENOM 1000000      /* 6 VCOs */
			#elif (HYBRID_NVFOS == 4)
				/* 4 VCOs, IF=60.700000, tune: 0.030000..56.000000 */
				#define FREQ_SCALE 1177380      /* Full VCO range 60.730000..116.700000 */
				#define FREQ_DENOM 1000000      /* 4 VCOs */			
			#else
				#error HYBRID_NVFOS not defined
			#endif
		#elif (TUNE_BOTTOM == 100000L) && (TUNE_TOP == 32000000L)	// BANDSELSTYLERE_UPCONV32M
			#if (HYBRID_NVFOS == 6)
				/* 6 VCOs, IF=60.700000, tune: 0.100000..32.000000 */
				#define FREQ_SCALE 1072826      /* Full VCO range 60.800000..92.700000 */
				#define FREQ_DENOM 1000000      /* 6 VCOs */
			#elif (HYBRID_NVFOS == 4)
				/* 4 VCOs, IF=60.700000, tune: 0.100000..32.000000 */
				#define FREQ_SCALE 1111204      /* Full VCO range 60.800000..92.700000 */
				#define FREQ_DENOM 1000000      /* 4 VCOs */
			#else
				#error HYBRID_NVFOS not defined
			#endif
		#else
			#error Wrong BANDSELSTYLERE_xxx used
		#endif
	#else
	
		#define DIRECT_50M0_X8	1	/* DDS LO1, LO2 = 50000 kHz - for 60700 kHz filter */	

	#endif

#endif /* FQMODEL_60700_IF05 || FQMODEL_60700_IF02 */

/* Первая ПЧ - 45 МГц, вторая - 13.0 МГц, третья - 500 кГц */
#if FQMODEL_45000_13M_IF0P5

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_LOWER	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
	#if WITHIF4DSP
		// архитектура для DSP на последней ПЧ
		#define LO4_SIDE	LOCODE_LOWER	/* параметр проверен по отсутствию помехи при приёме для FQMODEL_80455. При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO5_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#else /* WITHIF4DSP */
		// обычная архитектура для слухового приёмв
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
		//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: при отсутствующем гетеродине - на нём нет инверсии спектра */
	#endif /* WITHIF4DSP */
	
	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define LO2_PLL_R	1		/* опорный сигнал прямо используется как LO2 */
	#define LO2_PLL_N	1		/* опорный сигнал прямо используется как LO2  */

	#define	LO3_POWER2	0		/* 0 - нет делителя после генератора LO3 перед подачей на смеситель */
	//#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */

	#define DIRECT_32M0_X12	1	/* DDS LO1, LO2 = 32000 kHz */	

	#if LO3_SIDE == LOCODE_UPPER
		#define LO3_PLL_R	320		/* Парметр для корректной работы математики */
		#define LO3_PLL_N	135		/* LO3 = 13.5 MHz */
	#else /* LO3_UPPER */
		#define LO3_PLL_R	320		/* Парметр для корректной работы математики */
		#define LO3_PLL_N	125		/* LO3 = 12.5 MHz */
	#endif /* LO3_UPPER */

	#define IF3_MODEL IF3_TYPE_500

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */

	#if BANDSELSTYLERE_UPCONV32M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz нижняя частота настройки */
		#define TUNE_TOP 32000000L		/* верхняя частота настройки */
	#elif BANDSELSTYLERE_UPCONV36M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz нижняя частота настройки */
		#define TUNE_TOP 36000000L		/* верхняя частота настройки */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

#endif /* FQMODEL_45000_13M_IF0P5 */

/* Первая ПЧ - 36000 кГц, вторая - 500 кГц */
#if FQMODEL_36000_IF500

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	//#define LO1_SIDE_F(freq) (((freq) < 70200000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* сторона зависит от рабочей частоты */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
#if WITHIF4DSP
	// архитектура для DSP на последней ПЧ
	#define LO4_SIDE	LOCODE_LOWER	/* параметр проверен по отсутствию помехи при приёме для FQMODEL_80455. При преобразовании на этом гетеродине нет инверсии спектра */
#else /* WITHIF4DSP */
	// обычная архитектура для слухового приёмв
	#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
#endif /* WITHIF4DSP */

	#define IF3_MODEL IF3_TYPE_500

	#define LO2_PLL_R	1		/* опорный сигнал прямо используется как LO2 */
	#define LO2_PLL_N	1		/* опорный сигнал прямо используется как LO2  */

	//#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */
	#define LO4_POWER2 0

	#if BANDSELSTYLERE_UPCONV32M
		#define TUNE_BOTTOM 100000L		/* нижняя частота настройки */
		#define TUNE_TOP 32000000L		/* верхняя частота настройки */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if MODEL_DIRECT
		#define DIRECT_35M5_X11	1	/* DDS LO1, LO2 = 36000 kHz - for 70200 kHz filter from EKD */	
	#else
		#define HYBRID_35M5_10M7	1	/* Hybrid LO1, LO2 = 36000 kHz - for 70200 kHz filter from EKD */

		#define SYNTHLOWFREQ 	(36000000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(36000000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 6)
			//#define FREQ_DENOM 1000000
			//#define FREQ_SCALE 1102612	// 32 Mhz over 6 VFO
		#elif (HYBRID_NVFOS == 4)
			//#define FREQ_DENOM 1000000
			//#define FREQ_SCALE 1157802	// 32 MHz over 4 VFO
		#else
			#error HYBRID_NVFOS not defined
		#endif
	#endif

#endif /* FQMODEL_36000_IF500 */

/* Сигнал-генератор на Si570 "B" grade */
#if FQMODEL_RFSG_SI570

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO2_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */

	#define LO1MODE_DIRECT		1
	#define IF3_MODEL IF3_TYPE_DCRX

	#define	DDS1_CLK_MUL	1	// заглушка
	#define	DDS2_CLK_MUL	1	// заглушка
	#define	DDS3_CLK_MUL	1	// заглушка
	#define	REFERENCE_FREQ 100000000

	#define LO1FDIV_ADJ	1		/* если определено - подстройка делителя в тракте первого гетеродина разрешена через меню */
	//#define LO1_POWER2	0		/* 8 - Это общиее деление (4 - в смесителе, 2 - формирователь меандра на выходе управляемого делителя) */
	#define LO1_POWER2	0		/* 8 - Это общиее деление (4 - в смесителе, 2 - формирователь меандра на выходе управляемого делителя) */

	//#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#define TUNE_BOTTOM 4008264L		/* нижняя частота настройки Si570 */
	#define TUNE_TOP 810000000L		/* верхняя частота настройки Si570  */

	#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */

#endif /* FQMODEL_RFSG_SI570 */

/* Первая ПЧ - 60.725 МГц, вторая - 10.725 МГц, третья - 500 кГц */
#if FQMODEL_60725_IF05

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	//#define LO3_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_UPPER 	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
	
	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define LO2_PLL_R	1		/* опорный сигнал прямо используется как LO2 */
	#define LO2_PLL_N	1		/* опорный сигнал прямо используется как LO2  */

	#define	LO3_POWER2	0		/* 0 - нет делителя после генератора LO3 перед подачей на смеситель */
	//#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */

	#define DIRECT_50M0_X8	1	/* DDS LO1, LO2 = 72595 kHz - for 73050 kHz filter from Kenwood */	

	#if LO3_SIDE == LOCODE_UPPER
		#define LO3_PLL_R	(50000/25)		/* Парметр для корректной работы математики */
		#define LO3_PLL_N	(11225/25)		/* LO3 = 11.225 MHz */
	#else /* LO3_UPPER */
		#define LO3_PLL_R	(50000/25)		/* Парметр для корректной работы математики */
		#define LO3_PLL_N	(10225/25)		/* LO3 = 10.225 MHz */
	#endif /* LO3_UPPER */

	#define IF3_MODEL IF3_TYPE_500

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 30000L		/* нижняя частота настройки */
		#define TUNE_TOP 56000000L		/* верхняя частота настройки */
	#else
		#error Wrong BANDSELSTYLExxx
	#endif /* BANDSELSTYLERE_UPCONV56M */

#endif /* FQMODEL_60725_IF05 */

/* Первая ПЧ - 45000 кГц, вторая - 455 кГц или 500 кГц или 128 кГц*/
#if FQMODEL_45_IF455 || FQMODEL_45_IF0P5 || FQMODEL_45_IF128

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */

	//#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define	LO4_POWER2	2		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */

	#if 0
		#define DIRECT_25M0_X6	1
		#define IF3_MODEL IF3_TYPE_DCRX
		#define LO2_PLL_R	REFERENCE_FREQ		
		#define LO2_PLL_N	45000000ul		

	#elif FQMODEL_45_IF128

		#if MODEL_DIRECT
			#define DIRECT_50M0_X8_LO4AD9951	1			/* DDS LO1, LO2 = 44500 kHz - for 45000 kHz filter from Kenwood */	
		#else
			#define HYBRID_44M5_10M7	1	/* Hybrid LO1, LO2 = 44500 kHz - for 45000 kHz filter */
		#endif
		//#define FIXED100_44M5		1
		//#define FIXED100_50M0		1
		#define IF3_MODEL IF3_TYPE_128

		#define LO2_DDSGENERATED	1			// dds3 used as LO2 output
		#define LO2_PLL_R	REFERENCE_FREQ		/* опорный сигнал прямо используется как LO2 */
		#define LO2_PLL_N	44872000ul		/* опорный сигнал прямо используется как LO2  */

	#elif FQMODEL_45_IF0P5
		#if MODEL_DIRECT
			#define DIRECT_44M5_X8	1			/* DDS LO1, LO2 = 44500 kHz - for 45000 kHz filter */	
		#else
			#define HYBRID_44M5_10M7	1	/* Hybrid LO1, LO2 = 44500 kHz - for 45000 kHz filter */
		#endif
		//#define FIXED100_44M5		1
		//#define FIXED100_50M0		1
		#define IF3_MODEL IF3_TYPE_500

		#define LO2_PLL_R	REFERENCE_FREQ		/* опорный сигнал прямо используется как LO2 */
		#define LO2_PLL_N	44500000ul		/* опорный сигнал прямо используется как LO2  */
		//#define LO2_PLL_R	1		/* опорный сигнал прямо используется как LO2 */
		//#define LO2_PLL_N	1		/* опорный сигнал прямо используется как LO2  */
	#elif FQMODEL_45_IF455
		#if MODEL_DIRECT
			#define DIRECT_44M545_X8		1
		#else
			#define HYBRID_44M545_10M7	1	/* Hybrid LO1, LO2 = 72595 kHz - for 73050 kHz filter from Kenwood */
		#endif
		//#define FIXED100_44M545		1
		//#define FIXED100_50M0		1

		#define IF3_MODEL IF3_TYPE_455

		//#define LO2_PLL_R	REFERENCE_FREQ		/* опорный сигнал прямо используется как LO2 */
		//#define LO2_PLL_N	44545000ul		/* опорный сигнал прямо используется как LO2  */
		#define LO2_PLL_R	1		/* опорный сигнал прямо используется как LO2 */
		#define LO2_PLL_N	1		/* опорный сигнал прямо используется как LO2  */
	#else
		#error Strange configuration...
	#endif

	#if BANDSELSTYLERE_UPCONV32M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz нижняя частота настройки */
		#define TUNE_TOP 32000000L		/* верхняя частота настройки */
	#elif BANDSELSTYLERE_UPCONV36M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz нижняя частота настройки */
		#define TUNE_TOP 36000000L		/* верхняя частота настройки */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if MODEL_DIRECT
	#else

		#define SYNTHLOWFREQ 	(45000000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(45000000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=45.000000, tune: 0.100000..36.000000 */
			#define FREQ_SCALE 1157649      /* Full VCO range 45.100000..81.000000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#elif (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=45.000000, tune: 0.100000..36.000000 */
			#define FREQ_SCALE 1102515      /* Full VCO range 45.100000..81.000000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#else
			#error HYBRID_NVFOS not defined
		#endif
	#endif


#endif /* MODEL_45_IF455 || MODEL_45_IF0P5 */

/* Nikolai: Первая ПЧ - 45000 кГц, вторая - 10700 кГц */
#if FQMODEL_45_IF10700_IF200

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_UPPER	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */

	#define DIRECT_34M3_X11	1	/*  */

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */

	#define LO2_PLL_R	343		/* For 0.1 MHz phase detector freq */
	#define LO2_PLL_N	343		/* for 34.3 MHz LO2 */
	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define LO3_PLL_R	343		/* For 0.1 MHz phase detector freq */
	#define LO3_PLL_N	109		/* for 10.9 MHz LO3 */
	#define	LO3_POWER2	0		/* 0 - нет делителя после генератора LO3 перед подачей на смеситель */
	//#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */

	#define IF3_MODEL IF3_TYPE_200

	#define WFM_IF1 (10700000UL)	/* 10.7 MHz */

	#if BANDSELSTYLERE_UPCONV32M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz нижняя частота настройки */
		#define TUNE_TOP 32000000L		/* верхняя частота настройки */
	#elif BANDSELSTYLERE_UPCONV36M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz нижняя частота настройки */
		#define TUNE_TOP 36000000L		/* верхняя частота настройки */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if MODEL_DIRECT
	#else

		#define SYNTHLOWFREQ 	(45000000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(45000000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=45.000000, tune: 0.100000..36.000000 */
			#define FREQ_SCALE 1157649      /* Full VCO range 45.100000..81.000000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#elif (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=45.000000, tune: 0.100000..36.000000 */
			#define FREQ_SCALE 1102515      /* Full VCO range 45.100000..81.000000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#else
			#error HYBRID_NVFOS not defined
		#endif
	#endif

#endif /* FQMODEL_45_IF10700_IF200 */

/* RA6LPO TRX: Первая ПЧ - 45000 кГц, вторая - 8868 кГц, третья - 200 кГц */
#if FQMODEL_45_IF8868_IF200

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_LOWER	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */

	//#define DIRECT_36M13_X11	1	/*  */
	#define DIRECT_50M0_X8	1	/*  */
	#define LO2_PLL_R	REFERENCE_FREQ		/* 1 Hz step */
	#define LO2_PLL_N	36130000		/* for 34.3 MHz LO2 */
	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */
	//#define	LO2_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#define LO3_PLL_R	REFERENCE_FREQ		/* 1 Hz step */
	#define LO3_PLL_N	8668000		/* for 10.9 MHz LO3 */
	#define	LO3_POWER2	0		/* 0 - нет делителя после генератора LO3 перед подачей на смеситель */
	#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */

	#define IF3_MODEL IF3_TYPE_200

	#if BANDSELSTYLERE_UPCONV32M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz нижняя частота настройки */
		#define TUNE_TOP 32000000L		/* верхняя частота настройки */
	#elif BANDSELSTYLERE_UPCONV36M
		#define TUNE_BOTTOM 100000L		/* 0.1 MHz нижняя частота настройки */
		#define TUNE_TOP 36000000L		/* верхняя частота настройки */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /* BANDSELSTYLERE_UPCONV56M */

	#if MODEL_DIRECT
	#else

		#define SYNTHLOWFREQ 	(45000000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(45000000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=45.000000, tune: 0.100000..36.000000 */
			#define FREQ_SCALE 1157649      /* Full VCO range 45.100000..81.000000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#elif (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=45.000000, tune: 0.100000..36.000000 */
			#define FREQ_SCALE 1102515      /* Full VCO range 45.100000..81.000000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#else
			#error HYBRID_NVFOS not defined
		#endif
	#endif

#endif /* FQMODEL_45_IF8868_IF200 */

#if FQMODEL_TRX5M_UA1FA		// Первая низкая ПЧ, вторая ПЧ 500 кГц

	#if ! defined (IF3_MODEL)
		#error Please define IF3_MODEL as IF3_TYPE_XXXX
		//#define IF3_MODEL IF3_TYPE_500
	#endif
	// Первый DDS используется как генератор
	#define LO1FDIV_ADJ	1		/* если определено - подстройка делителя в тракте первого гетеродина разрешена через меню */
	//#define LO1_POWER2	1		/* 8 - Это общиее деление (4 - в смесителе ТПП, 2 - формирователь меандра на выходе управляемого делителя) */
	#define LO1_POWER2	0		/* 0 - нет делителя после генератора LO1 перед подачей на смеситель */
	//#define LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define LO3_PLL_R	REFERENCE_FREQ		/* 1 Hz step */
	#define LO3_PLL_N	5000000UL		/* for 5.000 MHz LO3 */
	#define	LO3_POWER2	0		/* 0 - нет делителя после генератора LO3 перед подачей на смеситель */
	#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#if WITHLO4FREQPOWER2
		#define LO4FDIV_ADJ	1		/* если определено - подстройка делителя в тракте последнего гетеродина разрешена через меню */
		#define	LO4_POWER2	1		/* 1 - делитель на 2 после генератора LO4 перед подачей на смеситель */
	#else
		#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */
	#endif

	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#if WITHFIXEDBFO
		/* Описание структуры преобразований частоты в тракте */
		#define LO1_SIDE	LOCODE_TARGETED	/* При преобразовании на этом гетеродине происходит инверсия спектра */
		#define LO2_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#define LO3_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
		/* LO4_SIDE расчитывается в getsidelo4() и может быть LOCODE_UPPER или LOCODE_LOWER */
	#elif WITHCLASSICSIDE
		//#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
		#define LO1_SIDE_F(freq) (((freq) < 13950000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* сторона зависит от рабочей частоты */
		#define LO2_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#define LO3_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
	#else
		// WITHDUALBFO или классический вариант с AD9834 в BFO
		/* Описание структуры преобразований частоты в тракте */
		#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
		#define LO2_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#define LO3_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
	#endif

	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define TUNE_BOTTOM 1600000L		/* нижняя частота настройки */
	#define TUNE_TOP 32000000L		/* верхняя частота настройки */

#endif	/* FQMODEL_TRX5M_UA1FA */

#if FQMODEL_TRX8M2CONV	// Первая низкая ПЧ, далее ЭМФ на 200/455/500 кГц

	// Здесь  надо выбрать значение последней ПЧ обслуживаемого устройства

	#if ! defined (IF3_MODEL)
		#error Please define IF3_MODEL as IF3_TYPE_XXXX
	#endif /* ! defined (IF3_MODEL) */

	// Первый DDS используется как генератор
	#define LO1FDIV_ADJ	1		/* если определено - подстройка делителя в тракте первого гетеродина разрешена через меню */
	//#define LO1_POWER2	1		/* 8 - Это общиее деление (4 - в смесителе ТПП, 2 - формирователь меандра на выходе управляемого делителя) */
	#define LO1_POWER2	0		/* 0 - нет делителя после генератора LO1 перед подачей на смеситель */
	//#define LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define LO3_PLL_R	REFERENCE_FREQ		/* 1 Hz step */
	#define LO3_PLL_N	8900000		/* for 8.9 MHz LO3 */
	#define	LO3_POWER2	0		/* 0 - нет делителя после генератора LO3 перед подачей на смеситель */
	#define	LO3_FREQADJ	1	/* подстройка частоты гетеродина через меню. */

	#if WITHLO4FREQPOWER2
		#define LO4FDIV_ADJ	1		/* если определено - подстройка делителя в тракте последнего гетеродина разрешена через меню */
		#define	LO4_POWER2	1		/* 1 - делитель на 2 после генератора LO4 перед подачей на смеситель */
	#else
		#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */
	#endif

	// WITHDUALBFO или классический вариант с AD9834 в BFO
	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO2_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#define LO3_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */

	#if MODEL_DIRECT
		
	#elif MODEL_HYBRID
		// следующие два определения только вместе
		#define HYBRID_OCTAVE_80_160	1	/* плата с 4 ГУН */
	#else
		#error VFO model not specified
	#endif
	
	#define TUNE_BOTTOM 1500000L
	#define TUNE_TOP 32000000L

#endif // MODEL_TRX8M

#if FQMODEL_TRX8M	// Первая низкая ПЧ 8 МГц, 6 МГц, 5.5 МГц и другие

	// Здесь  надо выбрать значение последней ПЧ обслуживаемого устройства

	#if ! defined (IF3_MODEL)
		#error Please define IF3_MODEL as IF3_TYPE_XXXX
		//#define IF3_MODEL IF3_TYPE_10000
		//#define IF3_MODEL IF3_TYPE_8000
		//#define IF3_MODEL IF3_TYPE_5000
		//#define IF3_MODEL IF3_TYPE_6000
		//#define IF3_MODEL IF3_TYPE_9000
		//#define IF3_MODEL IF3_TYPE_8868
		//#define IF3_MODEL IF3_TYPE_5645	// Drake R-4C and Drake T-4XC (Drake Twins) - 5645 kHz
		//#define IF3_MODEL IF3_TYPE_5500
		//#define IF3_MODEL IF3_TYPE_9045
		////#define IF3_MODEL IF3_TYPE_500 использовать FQMODEL_TRX500K
		//#define IF3_MODEL IF3_TYPE_455
	#endif /* ! defined (IF3_MODEL) */

	// Первый DDS используется как генератор
	#ifndef LO1FDIV_ADJ
		#define LO1FDIV_ADJ	1		/* если определено - подстройка делителя в тракте первого гетеродина разрешена через меню */
		//#define LO1_POWER2	1		/* 8 - Это общиее деление (4 - в смесителе ТПП, 2 - формирователь меандра на выходе управляемого делителя) */
		#define LO1_POWER2	0		/* 0 - нет делителя после генератора LO1 перед подачей на смеситель */
	
	#endif	/* LO1FDIV_ADJ */

	//#define LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#if WITHLO4FREQPOWER2
		#define LO4FDIV_ADJ	1		/* если определено - подстройка делителя в тракте последнего гетеродина разрешена через меню */
		#define	LO4_POWER2	1		/* 1 - делитель на 2 после генератора LO4 перед подачей на смеситель */
	#else
		#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */
	#endif

	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#if WITHFIXEDBFO
		/* Описание структуры преобразований частоты в тракте */
		#define LO1_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
		#define LO2_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		/* LO4_SIDE расчитывается в getsidelo4() и может быть LOCODE_UPPER или LOCODE_LOWER */
	#elif WITHCLASSICSIDE
		//#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
		#define LO1_SIDE_F(freq) (((freq) < 13950000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* сторона зависит от рабочей частоты */
		#define LO2_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
	#else
		// WITHDUALBFO или классический вариант с AD9834 в BFO
		/* Описание структуры преобразований частоты в тракте */
		#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
		#define LO2_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
	#endif

	#if MODEL_DIRECT
		
	#elif MODEL_HYBRID
		// следующие два определения только вместе
		#define HYBRID_OCTAVE_80_160	1	/* плата с 4 ГУН */
	#else
		#error VFO model not specified
	#endif
	
	#if BANDSELSTYLERE_LOCONV32M_NLB
		#define TUNE_BOTTOM 1500000L
		#define TUNE_TOP 32000000L
	#elif BANDSELSTYLERE_LOCONV32M
		#define TUNE_BOTTOM 100000L
		#define TUNE_TOP 32000000L
	#elif BANDSELSTYLE_LADVABEST
		#define TUNE_BOTTOM 1600000L			/* нижняя частота настройки */
		#define TUNE_TOP 56000000L
	#elif BANDSELSTYLERE_RA4YBO_AM0
		#define TUNE_BOTTOM 150000L			/* 150 kHz нижняя частота настройки */
		#define TUNE_TOP 4000000L			/* 4 MHz - верх */
		#define BANDMIDDLE	2200000L
		#define UPPER_DEF	2200000L
	#else
		#error Strange BANDSELSTYLExxx
	#endif


#endif // MODEL_TRX8M


// Первая ПЧ 500 кГц
#if FQMODEL_TRX500K || FQMODEL_TRX455K

	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#if WITHFIXEDBFO
		/* Описание структуры преобразований частоты в тракте */
		#define LO1_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
		#define LO2_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		/* LO4_SIDE расчитывается в getsidelo4() и может быть LOCODE_UPPER или LOCODE_LOWER */
	#else
		/* Описание структуры преобразований частоты в тракте */
		#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
		#define LO2_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
	#endif
	// Здесь надо выбрать значение последней ПЧ обслуживаемого устройства

	#if FQMODEL_TRX500K
		#define IF3_MODEL IF3_TYPE_500
	#elif FQMODEL_TRX455K
		#define IF3_MODEL IF3_TYPE_455
	#else
		#erros Strange FQMODEL_XXX value
	#endif

	// Первый DDS используется как генератор
	#define LO1FDIV_ADJ	1		/* если определено - подстройка делителя в тракте первого гетеродина разрешена через меню */
	//#define LO1_POWER2	0		/* 8 - Это общиее деление (4 - в смесителе, 2 - формирователь меандра на выходе управляемого делителя) */
	#define LO1_POWER2	0		/* 8 - Это общиее деление (4 - в смесителе, 2 - формирователь меандра на выходе управляемого делителя) */


	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */
	#define LO2_PLL_R	1		/* LO2 = 0 MHz */
	#define LO2_PLL_N	0		/* LO2 = 0 MHz */

	/* Board hardware configuration */
	#if MODEL_DIRECT

	#else
		// следующие два определения только вместе
		#define HYBRID_OCTAVE_80_160	1	/* плата с 4 ГУН */
	#endif

	#if BANDSELSTYLERE_LOCONV32M_NLB
		#define TUNE_BOTTOM 1500000L
		#define TUNE_TOP 32000000L
	#elif BANDSELSTYLERE_LOCONV32M
		#define TUNE_BOTTOM 100000L
		#define TUNE_TOP 32000000L
	#elif BANDSELSTYLERE_LOCONV15M_NLB
		#define TUNE_BOTTOM 1600000L	// 1.6 МГц
		#define TUNE_TOP 15000000L		// в 500 кГц версии настраиваемся до 10.999999
	#elif BANDSELSTYLERE_LOCONV15M
		#define TUNE_BOTTOM 100000L		// 100 кГц
		#define TUNE_TOP 15000000L		// в 500 кГц версии настраиваемся до 10.999999
	#else
		#error Strange BANDSELSTYLExxx
	#endif

#endif // FQMODEL_TRX500K


// Интеролятор
#if FQMODEL_UW3DI

	// Обозначение LO1 и LO2 для данного синтезатора меняются местами.
	// В данной программе LO1 это всегда генератор с плавной настройкой

	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#if WITHFIXEDBFO
		/* Описание структуры преобразований частоты в тракте */
		#define LO1_SIDE_F(freq) (((freq) < 60000000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* сторона зависит от рабочей частоты */
		//#define LO1_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO2_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
		#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		/* LO4_SIDE расчитывается в getsidelo4() и может быть LOCODE_UPPER или LOCODE_LOWER */
	#else
		#error Use WITHFIXEDBFO with FQMODEL_UW3DI
		/* Описание структуры преобразований частоты в тракте */
		//#define LO1_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
		//#define LO2_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		//#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		//#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
	#endif
	// Здесь надо выбрать значение последней ПЧ обслуживаемого устройства

	#define IF3_MODEL IF3_TYPE_500

	//#define LO1FDIV_ADJ	1		/* если определено - подстройка делителя в тракте первого гетеродина разрешена через меню */
	//#define LO1_POWER2	0		/* 8 - Это общиее деление (4 - в смесителе, 2 - формирователь меандра на выходе управляемого делителя) */
	#define LO1_POWER2	0		/* 8 - Это общиее деление (4 - в смесителе, 2 - формирователь меандра на выходе управляемого делителя) */


	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */

	#define HINTLO2_GRANULARITY (5L * 100 * 1000)	/* Переключение кварцеав идёт с шагом 500 кГц */
	#define IF2FREQBOTOM	6000000L		/* минимальная частота настройки интерполяционного приемника */
	#define IF2FREQTOP		6500000L		/* Максимальная частота настройки интерполяционного приемника */

	/* Board hardware configuration */
	#if MODEL_DIRECT

	#elif MODEL_MAXLAB

		#define SYNTHLOWFREQ 5480000L	/* минимальная частота ГПД */
		#define DDS1_CLK_MUL 1

	#elif MODEL_HYBRID
		#define SYNTHLOWFREQ 5480000L	/* минимальная частота ГПД */
		#define SYNTHTOPFREQ 6020000L	/* минимальная частота ГПД */
		#define HYBRID_NVFOS 1
	#else
		// следующие два определения только вместе
		#define HYBRID_OCTAVE_80_160	1	/* плата с 4 ГУН */
	#endif

	#define TUNE_BOTTOM 1500000L
	#define TUNE_TOP 30000000L

#endif // FQMODEL_UW3DI


// прямое преобразование
#if FQMODEL_DCTRX

	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */

	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#if WITHIF4DSP
		/* Описание структуры преобразований частоты в тракте */
		#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO2_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#define LO4_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#else
		/* Описание структуры преобразований частоты в тракте */
		#define LO1_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
		#define LO2_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
	#endif

	#if 1

		#define IF3_MODEL IF3_TYPE_DCRX

		// Первый DDS используется как генератор
		#if 1
			#define LO1FDIV_ADJ	1		/* если определено - подстройка делителя в тракте первого гетеродина разрешена через меню */
			#define LO1_POWER2	2		/* 8 - Это общиее деление (4 - в смесителе, 2 - формирователь меандра на выходе управляемого делителя) */
		#else
			//#define LO1FDIV_ADJ	1		/* если определено - подстройка делителя в тракте первого гетеродина разрешена через меню */
			#define LO1_POWER2	0		/* 8 - Это общиее деление (4 - в смесителе, 2 - формирователь меандра на выходе управляемого делителя) */
		#endif


	// -- parameters for 128 MHz..256 MHz
	#elif 0

		#define IF3_MODEL IF3_TYPE_DCRX
		//#define IF3_MODEL IF3_TYPE_8000
		//#define IF3_MODEL IF3_TYPE_8868

		/* плата с гибридным синтезатором для трактов прямого преобразования */
		#define LO1FDIV_ADJ	1		/* если определено - подстройка делителя в тракте первого гетеродина разрешена через меню */
		#define LO1_POWER2	3		/* 8 - Это общиее деление (4 - в смесителе, 2 - формирователь меандра на выходе управляемого делителя) */

		#define LO1MODE_HYBRID	1
		#define HYBRID_OCTAVE_128_256	1
	#endif // structure versions

	#if BANDSELSTYLERE_UPCONV56M
		#define TUNE_BOTTOM 30000L	// 30 kHz
		#define TUNE_TOP 56000000L	// 56 MHz
	#elif BANDSELSTYLERE_LOCONV32M_NLB
		#define TUNE_BOTTOM 1600000L	// 1.6 МГц
		#define TUNE_TOP 32000000L
	#elif BANDSELSTYLERE_LOCONV32M
		#define TUNE_BOTTOM 100000L		// 100 кГц
		#define TUNE_TOP 32000000L
	#elif BANDSELSTYLERE_LOCONV15M_NLB
		#define TUNE_BOTTOM 1600000L	// 1.6 МГц
		#define TUNE_TOP 11000000L		// в 500 кГц версии настраиваемся до 10.999999
	#elif BANDSELSTYLERE_LOCONV15M
		#define TUNE_BOTTOM 100000L		// 100 кГц
		#define TUNE_TOP 11000000L		// в 500 кГц версии настраиваемся до 10.999999
	#else
		#error Strange BANDSELSTYLExxx
	#endif

	/* +++ параметры синтезатора с фиксированным коэф. умножения частоты DDS */
	//#define LO1MODE_FIXSCALE	1	
	//#define SYNTH_N1 256
	//#define SYNTH_R1 1
	/* --- параметры синтезатора с фиксированным коэф. умножения частоты DDS */

#endif // MODEL_DCTRX

#if FQMODEL_23M_IF500

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */

	#define DIRECT_22M5_X17	1	/* R-143 */
	#define IF3_MODEL IF3_TYPE_500

	#if BANDSELSTYLERE_UPCONV32M
		#define TUNE_BOTTOM 30000L		/* нижняя частота настройки */
		#define TUNE_TOP 32000000L		/* верхняя частота настройки */
	#else
		#error Wrong BANDSELSTYLERE_xxx used
	#endif /*  */

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */

	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */
	#define LO2_PLL_R	1		/* опорный сигнал прямо используется как LO2 */
	#define LO2_PLL_N	1		/* опорный сигнал прямо используется как LO2  */
	#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */

	//#define WFM_IF1 (10700000UL)	/* 10.7 MHz */
#endif /* FQMODEL_23M_IF500 */


/* For UT1MB */
#if FQMODEL_45M_46615120_IF8868

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */

	#define DIRECT_46615120_X8	1	/* SW2011 TRX scheme V.1 */
	#define IF3_MODEL IF3_TYPE_8868

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */
	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define LO2_PLL_R	46615120UL		/* Сетка 1 кГц */
	#define LO2_PLL_N	36132000UL		/* Что используется как LO2  */

	#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */

	#define WFM_IF1 (10700000UL)	/* 10.7 MHz */

	#define TUNE_BOTTOM 100000L		/* нижняя частота настройки */
	#define TUNE_TOP 36000000L		/* верхняя частота настройки */

	#if MODEL_DIRECT
	#else

		#define SYNTHLOWFREQ 	(45000000L + TUNE_BOTTOM)	/* Lowest frequency of 1-st LO */
		#define SYNTHTOPFREQ 	(45000000L + TUNE_TOP)		/* Highest frequency of 1-st LO */

		#if (HYBRID_NVFOS == 4)
			/* 4 VCOs, IF=45.000000, tune: 0.100000..36.000000 */
			#define FREQ_SCALE 1157649      /* Full VCO range 45.100000..81.000000 */
			#define FREQ_DENOM 1000000      /* 4 VCOs */
		#elif (HYBRID_NVFOS == 6)
			/* 6 VCOs, IF=45.000000, tune: 0.100000..36.000000 */
			#define FREQ_SCALE 1102515      /* Full VCO range 45.100000..81.000000 */
			#define FREQ_DENOM 1000000      /* 6 VCOs */
		#else
			#error HYBRID_NVFOS not defined
		#endif
	#endif

#endif /* FQMODEL_45M_46615120_IF8868 */



/* SW2011 TRX: Первая ПЧ - 45000 кГц, вторая - 8868 кГц */
#if \
	FQMODEL_45_IF8868 || \
	FQMODEL_45_IF8868_UHF144 || FQMODEL_45_IF8868_UHF430 || \
	FQMODEL_45_IF6000 || \
	FQMODEL_45_IF6000_UHF144 || FQMODEL_45_IF6000_UHF430 || \
	FQMODEL_40_IF8000 || FQMODEL_40_IF8000_UHF144 || \
	FQMODEL_45_IF5000 || FQMODEL_45_IF5000_UHF144 || \
	FQMODEL_45_IF8000_UHF433_REF53 || \
	0

	#if FQMODEL_45_IF8000_UHF433_REF53
		#define DIRECT_53M00_X7	1	/* ladvabest@gmail.com */
		#define IF3_MODEL IF3_TYPE_8000
	#elif FQMODEL_45_IF6000 || FQMODEL_45_IF6000_UHF144 || FQMODEL_45_IF6000_UHF430
		#if CTLSTYLE_SW2014FM
			#define DIRECT_39M00_X10_EXACT	1	/* SW2011 TRX scheme V.2 */
			#define IF3_MODEL IF3_TYPE_6000_SW2014NFM
		#elif CTLSTYLE_SW2015
			#define DIRECT_39M00_X10_EXACT	1	/* SW2011 TRX scheme V.2 */
			#define IF3_MODEL IF3_TYPE_6000_SW2015	/* Слегка выше по частоте верхний скат */
		#elif CTLSTYLE_SW2016
			#define DIRECT_39M00_X10_EXACT	1	/* SW2011 TRX scheme V.2 */
			#define IF3_MODEL IF3_TYPE_6000_SW2015	/* Слегка выше по частоте верхний скат */
		#elif CTLSTYLE_SW2016VHF
			#define DIRECT_39M00_X10_EXACT	1	/* SW2011 TRX scheme V.2 */
			#define IF3_MODEL IF3_TYPE_6000_SW2015	/* Слегка выше по частоте верхний скат */
		#elif CTLSTYLE_SW2018XVR
			#define DIRECT_39M00_X10_EXACT	1	/* SW2011 TRX scheme V.2 */
			#define IF3_MODEL IF3_TYPE_6000_SW2015	/* Слегка выше по частоте верхний скат */
		#elif CTLSTYLE_SW2016MINI
			#define DIRECT_39M00_X10_EXACT	1	/* SW2016 MINI TRX */
			#define IF3_MODEL IF3_TYPE_6000_SW2016MINI
		#else /* CTLSTYLE_SW2014FM */
			#define DIRECT_39M00_X10	1	/* SW2011 TRX scheme V.2 */
			#define IF3_MODEL IF3_TYPE_6000
		#endif /* CTLSTYLE_SW2014FM */
	#elif FQMODEL_45_IF8868 || FQMODEL_45_IF8868_UHF144 || FQMODEL_45_IF8868_UHF430
		#define DIRECT_36M13_X11	1	/* SW2011 TRX scheme V.1 */
		#define IF3_MODEL IF3_TYPE_8868
	#elif FQMODEL_40_IF8000 || FQMODEL_40_IF8000_UHF144
		#define DIRECT_40M0_X10	1
		#define IF3_MODEL IF3_TYPE_8000
	#elif FQMODEL_45_IF5000 || FQMODEL_45_IF5000_UHF144
		#define DIRECT_40M0_X10	1
		#define IF3_MODEL IF3_TYPE_5000
	#endif

	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */

	#if FQMODEL_45_IF8000_UHF433_REF53

		/* Описание структуры преобразований частоты в тракте */
		#define LO2_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсии спектра */
		#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#if WITHIF4DSP
			// архитектура для DSP на последней ПЧ
			#define LO4_SIDE	LOCODE_LOWER	/* параметр проверен по отсутствию помехи при приёме для FQMODEL_80455. При преобразовании на этом гетеродине нет инверсии спектра */
			#define LO5_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
		#else /* WITHIF4DSP */
			// обычная архитектура для слухового приёмв
			#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
			//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: при отсутствующем гетеродине - на нём нет инверсии спектра */
		#endif /* WITHIF4DSP */

		//#define TUNE_BOTTOM 30000L			/* нижняя частота настройки */
		#define TUNE_BOTTOM 1600000L			/* нижняя частота настройки */
		#define TUNE_TOP 490000000L		/* верхняя частота настройки */

	#elif FQMODEL_45_IF8868_UHF144 || FQMODEL_45_IF6000_UHF144 || FQMODEL_40_IF8000_UHF144 || FQMODEL_45_IF5000_UHF144

		/* Описание структуры преобразований частоты в тракте */
		#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#if WITHIF4DSP
			// архитектура для DSP на последней ПЧ
			#define LO4_SIDE	LOCODE_LOWER	/* параметр проверен по отсутствию помехи при приёме для FQMODEL_80455. При преобразовании на этом гетеродине нет инверсии спектра */
			#define LO5_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
		#else /* WITHIF4DSP */
			// обычная архитектура для слухового приёмв
			#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */
			//#define LO5_SIDE	LOCODE_INVALID	/* 12->baseband: при отсутствующем гетеродине - на нём нет инверсии спектра */
		#endif /* WITHIF4DSP */

		#define TUNE_BOTTOM 30000L			/* нижняя частота настройки */
		//#define TUNE_BOTTOM 1600000L			/* нижняя частота настройки */
		#define TUNE_TOP 170000000L		/* верхняя частота настройки */

	#elif FQMODEL_45_IF8868_UHF430 || FQMODEL_45_IF6000_UHF430	// UR7GP request

		/* Описание структуры преобразований частоты в тракте */
		#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */

		#define TUNE_BOTTOM 30000L			/* нижняя частота настройки */
		#define TUNE_TOP 470000000L		/* верхняя частота настройки */

	#else				// только HF

		/* Описание структуры преобразований частоты в тракте */
		#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
		#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
		#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */

		#define TUNE_BOTTOM 30000L			/* нижняя частота настройки */
		#define TUNE_TOP 32000000L		/* верхняя частота настройки */

	#endif

	#if CTLSTYLE_SW2016VHF || CTLSTYLE_SW2018XVR
		#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#else /* CTLSTYLE_SW2016VHF */
		#define LO1_SIDE_F(freq) (((freq) < 80000000L) ? LOCODE_UPPER : LOCODE_LOWER)	/* сторона зависит от рабочей частоты */
	#endif /* CTLSTYLE_SW2016VHF */


	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */
	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */
	#define LO2_PLL_R	1		/* опорный сигнал прямо используется как LO2 */
	#define LO2_PLL_N	1		/* опорный сигнал прямо используется как LO2  */
	#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */

	#define WFM_IF1 (10700000UL)	/* 10.7 MHz */

#endif /* FQMODEL_45_IF8868 */

/* Первая ПЧ - 58.1125 кГц, вторая - 8868 кГц - для gena, DL6RF */
#if FQMODEL_58M11_IF8868

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE	LOCODE_UPPER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#define LO4_SIDE	LOCODE_TARGETED	/* Этот гетеродин управляется для получения требуемой боковой */

	#define DIRECT_49M247_X8	1		/* SW2011 TRX from DL6RF */
	#define IF3_MODEL IF3_TYPE_8868

	#define TUNE_BOTTOM 30000L			/* нижняя частота настройки */
	#define TUNE_TOP 32000000L		/* верхняя частота настройки */

	#define LO1_POWER2	0		/* если 0 - делителей в тракте первого гетеродина перед смесителем нет. */

	#define	LO2_POWER2	0		/* 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define LO2_PLL_R	1		/* опорный сигнал прямо используется как LO2 */
	#define LO2_PLL_N	1		/* опорный сигнал прямо используется как LO2  */

	#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */

	#define WFM_IF1 (10700000UL)	/* 10.7 MHz */

#endif /* FQMODEL_45_IF8868 */

// CML evaluation board with CMX992 chip.
// Первая ПЧ 45 МГц
// Первая ПЧ 10.7 МГц
#if FQMODEL_GEN500

	/* Описание структуры преобразований частоты в тракте */
	#define LO0_SIDE	LOCODE_INVALID	/* при отсутствующем конверторе - на нём нет инверсии спектра */
	#define LO1_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине происходит инверсия спектра */
	#define LO2_SIDE	LOCODE_LOWER	/* При преобразовании на этом гетеродине нет инверсии спектра */
	#define LO3_SIDE	LOCODE_INVALID	/* при отсутствующем гетеродине - на нём нет инверсии спектра */
	#define LO4_SIDE	LOCODE_LOWER	/* Этот гетеродин управляется для получения требуемой боковой */

	#define IF3_MODEL IF3_TYPE_DCRX

	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	12800000L	/* LO2 = 63955 kHz - for 64455 kHz filter from ICOM with 500 kHz IF2 */
	//#define REFERENCE_FREQ	19200000L	/* LO2 = 63955 kHz - for 64455 kHz filter from ICOM with 500 kHz IF2 */
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */

	//#define	LO2_PLL_N	(450 * 4)	// 45 * 4 MHz
	#define	LO2_PLL_N	(107 * 4)	// 10.7 * 5 MHz
	#define LO2_PLL_R	128		// 100 kHz ref freq
	//#define LO2_PLL_R	192		// 100 kHz ref freq

	// Первый DDS используется как генератор
	//#define LO1FDIV_ADJ	1		/* если определено - подстройка делителя в тракте первого гетеродина разрешена через меню */
	//#define LO1_POWER2	0		/* 8 - Это общиее деление (4 - в смесителе, 2 - формирователь меандра на выходе управляемого делителя) */
	#define LO1_POWER2	0		/* 8 - Это общиее деление (4 - в смесителе, 2 - формирователь меандра на выходе управляемого делителя) */

	#define	LO2_POWER2	2		/* 2 - /4, 0 - нет делителя после генератора LO2 перед подачей на смеситель */

	#define	LO4_POWER2	0		/* 0 - нет делителя после генератора LO4 перед подачей на смеситель */

	/* Board hardware configuration */
	//#define HYBRID_500_600	1
	//#define DIRECT_100M0_X4		1	/* Board hardware configuration */
	//#define DIRECT_50M0_X8		1	/* Board hardware configuration */
	//#define DIRECT_40M0_X10		1	/* Board hardware configuration */
	//#define DIRECT_36M13_X11	1	/* SW2011 TRX scheme */
	//#define DIRECT_72M595_X5	1	/* Для проверки с проводами к плате приёмника от макета */

	// следующие два определения только вместе
	//#define HYBRID_OCTAVE_80_160	1	/* плата с 4 ГУН */


	#define TUNE_BOTTOM 470000000L
	#define TUNE_TOP 500000000L

	#define SYNTHLOWFREQ 470000000L
	#define FREQ_SCALE 120
	#define FREQ_DENOM 100
	#define HYBRID_NVFOS 6

#endif /* FQMODEL_GEN500 */

// В зависимости от частотной схемы - параметры математической части синтезатора

#if DIRECT_63M9_X6
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	63955000L	/* LO2 = 63955 kHz - for 64455 kHz filter from ICOM with 500 kHz IF2 */
	#define DDS1_CLK_MUL	6 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_63M9_X6 */

#if DIRECT_63M9_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	63955000L	/* LO2 = 63955 kHz - for 64455 kHz filter from ICOM with 500 kHz IF2 */
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_63M9_X1 */

#if DIRECT_64M0_X6
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	64000000L	/* LO2 = 64000 kHz - for 64455 kHz filter from ICOM with 455 kHz IF2 */
	#define DDS1_CLK_MUL	6 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_64M0_X6 */

#if DIRECT_64M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	64000000L	/* LO2 = 64000 kHz - for 64455 kHz filter from ICOM with 455 kHz IF2 */
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_64M0_X1 */

#if DIRECT_44M545_X8
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	44545000L	/* LO2 = 45 MHz 1-st IF with 455 kHz IF2 */
	#define DDS1_CLK_MUL	8 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_44M545_X8 */

#if DIRECT_44M5_X8
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	44500000L	/* LO2 = for 45000 kHz filter with 500 kHz IF2 */
	#define DDS1_CLK_MUL	8 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_44M5_X8 */

#if DIRECT_25M0_X6
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	25000000ul	
	#define DDS1_CLK_MUL	6 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_25M0_X6 */

#if DIRECT_72M550_X5
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	72550000L	/* LO2 = 72550 kHz - for 73050 kHz filter from Kenwood with 455 kHz IF2 */
	#define DDS1_CLK_MUL	5 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_72M550_X5 */

#if DIRECT_40M0_X10
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	40000000ul
	#define DDS1_CLK_MUL	10 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1 		/* Умножитель в DDS3 */
#endif	/* DIRECT_40M0_X10 */

#if DIRECT_100M0_X4
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	100000000ul
	#define DDS1_CLK_MUL	4 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1 		/* Умножитель в DDS3 */
#endif	/* DIRECT_100M0_X4 */

#if DIRECT_40M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	40000000ul
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1 		/* Умножитель в DDS3 */
#endif	/* DIRECT_40M0_X1 */

#if DIRECT_20M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	20000000ul
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1 		/* Умножитель в DDS3 */
#endif	/* DIRECT_20M0_X1 */

#if DIRECT_25M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	25000000ul
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1 		/* Умножитель в DDS3 */
#endif	/* DIRECT_25M0_X1 */

#if DIRECT_26M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	26000000ul
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS2 */
#endif	/* DIRECT_26M0_X1 */

#if DIRECT_27M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	27000000ul
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS2 */
#endif	/* DIRECT_27M0_X1 */

#if DIRECT_100M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	100000000ul
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_100M0_X1 */

#if DIRECT_125M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	125000000uL
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_125M0_X1 */

#if DIRECT_122M88_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	122880000uL
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_122M88_X1 */

#if DIRECT_12M288_X10
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	12288000uL
	#define DDS1_CLK_MUL	10 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	10		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	10		/* Умножитель в DDS3 */
#endif	/* DIRECT_12M288_X10 */

#if DIRECT_XXXX_X1
	#define LO1MODE_DIRECT	1
	//#define REFERENCE_FREQ	122880000ul
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_XXXX_X1 */

#if DIRECT_160M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	160000000ul
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_160M0_X1 */

#if DIRECT_16M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	16000000ul
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_16M0_X1 */

#if DIRECT_48M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	48000000ul
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_48M0_X1 */

#if FIXSCALE_48M0_X1_DIV256
	#define LO1MODE_FIXSCALE	1
	#define SYNTH_N1	256
	#define SYNTH_R1	1
	#define REFERENCE_FREQ	48000000ul
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
	#define HYBRID_NVFOS	0
#endif	/* FIXSCALE_48M0_X1_DIV256 */

#if DIRECT_44M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	44000000ul
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_44M0_X1 */

#if DIRECT_50M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	50000000ul
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_50M0_X1 */

#if DIRECT_75M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	75000000ul
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

#if DIRECT_80M0_X5
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	80000000ul
	#define DDS1_CLK_MUL	5 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_80M0_X5 */

#if DIRECT_32M0_X12
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	32000000ul
	#define DDS1_CLK_MUL	12 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_32M0_X12 */

#if DIRECT_35M5_X11
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	35500000ul
	#define DDS1_CLK_MUL	11 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_35M5_X11 */

#if DIRECT_50M0_X8
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	50000000ul
	#define DDS1_CLK_MUL	8 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_50M0_X8 */

#if DIRECT_60M0_X6
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	60000000ul
	#define DDS1_CLK_MUL	6 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_50M0_X8 */

#if DIRECT_50M0_X8_LO4AD9951
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	50000000ul
	#define DDS1_CLK_MUL	8 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	8		/* Умножитель в DDS3 */
#endif	/* DIRECT_50M0_X8_LO4AD9951 */

#if DIRECT_63M8976_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	63897600ul
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1 		/* Умножитель в DDS3 */
#endif	/* DIRECT_50M0_X8 */

#if DIRECT_63M8976_X5
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	63897600ul
	#define DDS1_CLK_MUL	5 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1 		/* Умножитель в DDS3 */
#endif	/* DIRECT_63M8976_X5 */

#if DIRECT_30M0_X6
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	30000000ul
	#define DDS1_CLK_MUL	6 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1 		/* Умножитель в DDS3 */
#endif	/* DIRECT_30M0_X6 */

#if DIRECT_32M0_X6
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	32000000ul
	#define DDS1_CLK_MUL	6 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1 		/* Умножитель в DDS3 */
#endif	/* DIRECT_32M0_X6 */

#if DIRECT_66M0_X6
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	66000000ul
	#define DDS1_CLK_MUL	6 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_66M0_X6 */

#if DIRECT_66M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	66000000ul
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_66M0_X1 */

/* SW2011 TRX from DL6RF, 2-nd if = 8865 kHz */
#if DIRECT_49M247_X8
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	49247000UL	/* LO2 = 49.247 MHz */
	#define DDS1_CLK_MUL	8 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_49M247_X8 */

#if DIRECT_34M3_X11
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	34300000ul
	#define DDS1_CLK_MUL	11 //5 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_34M3_X11 */
/* SW2011 TRX scheme V1 */

#if DIRECT_46615120_X8
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	46615120ul
	#define DDS1_CLK_MUL	8 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_46615120_X8 */

#if DIRECT_36M13_X11
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	36131000ul
	#define DDS1_CLK_MUL	11 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_36M13_X11 */

/* Board hardware configuration for Si5351a */
#if DIRECT_39M0_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	39000000uL
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_39M0_X1 */

/* R-143  */
#if DIRECT_22M5_X17
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	22500000ul
	#define DDS1_CLK_MUL	17 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif /* DIRECT_22M5_X17 */

/* SW2011 TRX scheme V2 */
#if DIRECT_39M00_X10
	#define LO1MODE_DIRECT	1
	#ifdef ATMEGA_CTLSTYLE_V9SF_US2IT_H_INCLUDED
		#define REFERENCE_FREQ	39000000L
	#else
		#define REFERENCE_FREQ	39002000L	//38994000L	//38997000ul
	#endif
	#define DDS1_CLK_MUL	10 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_39M00_X10 */

/* ladvabest@gmail.com */
#if DIRECT_53M00_X7
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	53000000L
	#define DDS1_CLK_MUL	7 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_53M00_X7 */

/* CTLREGSTYLE_SW2014NFM TRX - использован опорный генератор вместо кварца */
#if DIRECT_39M00_X10_EXACT
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	39000000L
	#define DDS1_CLK_MUL	10 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_39M00_X10_EXACT */

/* SW2011 TRX scheme V2 */
#if DIRECT_40M00_X10
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	40000000ul
	#define DDS1_CLK_MUL	10 //5 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* HYBRID_36M2_10M7 */

#if DIRECT_DDS_ATTINY2313
	//#define REFERENCE_FREQ	18432000UL	// 18.432 kHz - то, что оказалось в наличии
	#define REFERENCE_FREQ	20000000L	// 18.432 kHz - то, что оказалось в наличии
	#define DDS1_CLK_MUL	1		/* Умножитель в DDS1 */
	#define DDS1_CLK_DIV	9		/* Делитель опорной частоты перед подачей в DDS1 (7=NCO, 9=DDS) */

#endif /* DIRECT_DDS_ATTINY2313 */

#if HYBRID_64M0_13M0
	/* в качестве подчисточного фильтра за DDS используется пъезофильтр 13.0 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	64000000ul
	#define SYNTH_CFCENTER	13000000L	/* Центр полосы пропускания фильтра после DDS */
	#define SYNTH_R1 25	/* делитель после DDS - коэфециент деления опоры для PLL */
	#define DDS1_CLK_MUL	1		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* HYBRID_64M0_13M0 */

#if HYBRID_64M0_10M7
	/* в качестве подчисточного фильтра за DDS используется пъезофильтр 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	64000000L	/* At 10.6666 MHz is a center of mirroring */
	#define SYNTH_CFCENTER	10700000L	/* Центр полосы пропускания фильтра после DDS */
	#define SYNTH_R1 21	/* делитель после DDS - коэфециент деления опоры для PLL */
	#define DDS1_CLK_MUL	1		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* HYBRID_64M0_10M7 */

#if HYBRID_63M9_10M7
	/* в качестве подчисточного фильтра за DDS используется пъезофильтр 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	63955000L	/* 10659166,66 is a center of mrroring */
	#define SYNTH_CFCENTER	10700000L	/* Центр полосы пропускания фильтра после DDS */
	#define SYNTH_R1 23	/* делитель после DDS - коэфециент деления опоры для PLL */
	#define DDS1_CLK_MUL	1		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* HYBRID_63M9_10M7 */

#if HYBRID_80M0_10M7
	/* в качестве подчисточного фильтра за DDS используется пъезофильтр 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	80000000L	/*  */
	#define SYNTH_CFCENTER	10700000L	/* Центр полосы пропускания фильтра после DDS */
	#define SYNTH_R1 15	/* делитель после DDS - коэфециент деления опоры для PLL */
	#define DDS1_CLK_MUL	1		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* HYBRID_80M0_10M7 */

#if HYBRID_50M0_10M7
	/* в качестве подчисточного фильтра за DDS используется пъезофильтр 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	50000000L	/*  */
	#define SYNTH_CFCENTER	10700000L	/* Центр полосы пропускания фильтра после DDS */
	#define SYNTH_R1 21	/* делитель после DDS - коэфециент деления опоры для PLL */
	#define DDS1_CLK_MUL	1		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* HYBRID_50M0_10M7 */

#if HYBRID_63M9_13M0
	/* в качестве подчисточного фильтра за DDS используется пъезофильтр 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	63955000L
	#define SYNTH_CFCENTER	13000000L	/* Центр полосы пропускания фильтра после DDS */
	#define SYNTH_R1 25	/* делитель после DDS - коэфециент деления опоры для PLL */
	#define LO2_PLL_R	10		/* For 6.4 MHz phase detector freq */
	#define LO2_PLL_N	10		/* for 64 MHz LO2 */
	#define DDS1_CLK_MUL	1		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* HYBRID_64M0_13M0 */

#if HYBRID_500_600
	/* в качестве подчисточного фильтра за DDS используется пъезофильтр 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	100000000L	/*  */
	#define SYNTH_CFCENTER	10700000L	/* Центр полосы пропускания фильтра после DDS */
	#define SYNTH_R1 11	/* делитель после DDS - коэфециент деления опоры для PLL */
	#define LO2_PLL_R	1000		/* For 0.1 MHz phase detector freq */
	#define LO2_PLL_N	1800		/* for 180 MHz LO2 */
	#define DDS1_CLK_MUL	1		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* HYBRID_500_600 */

#if HYBRID_12M8_10M7
	/* в качестве подчисточного фильтра за DDS используется пъезофильтр 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	12800000ul
	#define SYNTH_CFCENTER	10700000L	/* Центр полосы пропускания фильтра после DDS */
	#define SYNTH_R1 21	/* делитель после DDS - коэфециент деления опоры для PLL */
	#define LO2_PLL_R	4		/* For 3.2 MHz phase detector freq */
	#define LO2_PLL_N	20		/* for 64 MHz LO2 */
	#define DDS1_CLK_MUL	4		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* HYBRID_12M8_10M7 */

#if DIRECT_72M595_X5
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	72595000L	/* LO2 = 72595 kHz - for 73050 kHz filter from Kenwood with 455 kHz IF2 */
	#define DDS1_CLK_MUL	5 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_72M595_X5 */

#if DIRECT_72M595_X1
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	72595000L	/* LO2 = 72595 kHz - for 73050 kHz filter from Kenwood with 455 kHz IF2 */
	#define DDS1_CLK_MUL	1 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_72M595_X5 */

// FQMODEL_73050_XXX should be defined
#if HYBRID_72M595_10M7
	/* в качестве подчисточного фильтра за DDS используется пъезофильтр 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	72595000L	/* LO2 = 72595 kHz - for 73050 kHz filter from Kenwood with 455 kHz IF2 */
	#define SYNTH_CFCENTER	10700000L	/* Центр полосы пропускания фильтра после DDS */
	#define SYNTH_R1	15	/* делитель после DDS - коэфециент деления опоры для PLL */
	#define DDS1_CLK_MUL	1		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* HYBRID_36M2_10M7 */

// FQMODEL_70455 jr FQMODEL_70200 use this definitions
#if DIRECT_70M000_X5
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	70000000L	/* LO2 = 70000 kHz - for 70455 kHz filter from FT1000D with 455 kHz IF2 */
	#define DDS1_CLK_MUL	5 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_70M000_X5 */

#if DIRECT_61M440_X5
	#define LO1MODE_DIRECT	1
	#define REFERENCE_FREQ	61440000L	/* LO2 = 61440 kHz - for 70455.5 kHz filter */
	#define DDS1_CLK_MUL	5 		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* DIRECT_61M440_X5 */

// FQMODEL_70455 should be defined
#if HYBRID_70M000_10M7
	/* в качестве подчисточного фильтра за DDS используется пъезофильтр 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	70000000L	/* LO2 = 70000 kHz - for 70455 kHz filter from Kenwood with 455 kHz IF2 */
	#define SYNTH_CFCENTER	10700000L	/* Центр полосы пропускания фильтра после DDS */
	#define SYNTH_R1	15	/* делитель после DDS - коэфециент деления опоры для PLL */
	#define DDS1_CLK_MUL	1		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* HYBRID_70M000_10M7 */

// MODEL_73050 should be defined
#if HYBRID_72M550_10M7
	/* в качестве подчисточного фильтра за DDS используется пъезофильтр 10.7 MHz */
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	72550000L	/* LO2 = 72595 kHz - for 73050 kHz filter from Kenwood with 455 kHz IF2 */
	#define SYNTH_CFCENTER	10700000L	/* Центр полосы пропускания фильтра после DDS */
	#define SYNTH_R1		15	/* делитель после DDS - коэфециент деления опоры для PLL */
	#define DDS1_CLK_MUL	1		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif	/* HYBRID_72M550_10M7 */

#if MAXLAB_7M2
	#define LO1MODE_MAXLAB	1	
	#define REFERENCE_FREQ	7200000L	// 7.2 МГц начальное значение - минимальная частота на выходе опорного генератора
	//#define REFERENCE_FREQ	8000000L	// 7.2 МГц начальное значение - минимальная частота на выходе опорного генератора
	// Параметры программирования PLL и расчёта делителей
	#define SYNTH_R1 7200
	#define LM7001_PLL1_RCODE	LM7001_RCODE_7200

	#if 0 // CPUSTYLE_ARM
		#define MAXLAB_DACARRAY 1	/* Управляющие напряжения для варикапа расчитываются заранее - хранятся в массиве */
		#define MAXLAB_LINEARDACRANGE 128	// до аппроксимации значения на ЦАП от 0 до MAXLAB_LINEARDACRANGE - 1
	#else
		//#define MAXLAB_DACARRAY 1	/* Управляющие напряжения для варикапа расчитываются заранее - хранятся в массиве */
		#define MAXLAB_LINEARDACRANGE 4096	// до аппроксимации значения на ЦАП от 0 до MAXLAB_LINEARDACRANGE - 1
	#endif
#endif	/* MAXLAB_7M2 */

#if FIXED100_44M5
	#define LO1MODE_FIXSCALE 1
	#define REFERENCE_FREQ	44500000L
	#define SYNTH_N1		100		/* делитель после VFO - коэфециент деления ГУН для PLL */
	#define SYNTH_R1		1
	#define DDS1_CLK_MUL	1
	#define DDS2_CLK_MUL	1
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif

#if FIXED100_44M545
	#define LO1MODE_FIXSCALE 1
	#define REFERENCE_FREQ	45545000L
	#define SYNTH_N1		100		/* делитель после VFO - коэфециент деления ГУН для PLL */
	#define SYNTH_R1		1
	#define DDS1_CLK_MUL	1
	#define DDS2_CLK_MUL	1
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif

#if FIXED100_50M0
	#define LO1MODE_FIXSCALE 1
	#define REFERENCE_FREQ	50000000L
	#define SYNTH_N1		100		/* делитель после VFO - коэфециент деления ГУН для PLL */
	#define SYNTH_R1		1
	#define DDS1_CLK_MUL	1
	#define DDS2_CLK_MUL	1
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif

#if HYBRID_44M5_10M7
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	12800000L
	#define SYNTH_CFCENTER	10700000L	/* Центр полосы пропускания фильтра после DDS */
	#define SYNTH_R1		25	/* делитель после DDS - коэфециент деления опоры для PLL */
	#define DDS1_CLK_MUL	1
	#define DDS2_CLK_MUL	1
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif // HYBRID_44M5_10M7 */

#if HYBRID_44M545_10M7
	#define LO1MODE_HYBRID	1
	#define REFERENCE_FREQ	44545000UL
	#define SYNTH_CFCENTER	10700000L	/* Центр полосы пропускания фильтра после DDS */
	#define SYNTH_R1		25	/* делитель после DDS - коэфециент деления опоры для PLL */
	#define DDS1_CLK_MUL	1
	#define DDS2_CLK_MUL	1
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */
#endif // HYBRID_44M545_10M7 */

#if HYBRID_OCTAVE_128_256
	/* в качестве подчисточного фильтра за DDS используется пъезофильтр 10.7 MHz */
	// ++ hybrid synthesizer parameters for 128 MHz..256 MHz
	#define SYNTHLOWFREQ 	128000000L	/* Lowest frequency of 1-st LO */
	#define SYNTHTOPFREQ 	256000000L	/* Highest frequency of 1-st LO */
	#define FREQ_DENOM 1000000L
	#define FREQ_SCALE 1189207L
	#define HYBRID_NVFOS 4
	#define SYNTH_R1 13	/* divider afrer DSS1 unit */
	// -- parameters for 128 MHz..256 MHz

	// ++ hybrid synthesizer parameters for 0.1..75 MHz with IF=80.455
	//#define LO1MODE_HYBRID	1
	//#define SYNTHLOWFREQ 	80555000	/* Lowest frequency of 1-st LO */
	//#define SYNTHTOPFREQ 	155455000	/* Highest frequency of 1-st LO */
	//#define FREQ_DENOM 1000000
	//#define FREQ_SCALE 1178632L
	//#define SYNTH_R1 15	/* divider afrer DSS1 unit */
	// -- parameters for 0.1..75 MHz with IF=80.455

	#define REFERENCE_FREQ	12800000ul
	#define SYNTH_CFCENTER	10700000L	/* Центр полосы пропускания фильтра после DDS */
	//#define REFERENCE_FREQ	32000000ul
	//#define SYNTH_CFCENTER	10760000L	/* Центр полосы пропускания фильтра после DDS */
	#define LO2_PLL_N	0		/* LO2 = 0 MHz */

	#define DDS1_CLK_MUL	1		/* Умножитель в DDS1 */
	//#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	//#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */

#endif

#if HYBRID_OCTAVE_80_160
	/* в качестве подчисточного фильтра за DDS используется пъезофильтр 10.7 MHz */
	// ++ hybrid synthesizer parameters for 80 MHz..160 MHz
	#define LO1MODE_HYBRID	1
	#define SYNTHLOWFREQ 	80000000L	/* Lowest frequency of 1-st LO */
	#define SYNTHTOPFREQ 	160000000L	/* Highest frequency of 1-st LO */
	#define FREQ_DENOM 1000000L
	#define FREQ_SCALE 1189207L
	#define HYBRID_NVFOS 4		
	#define SYNTH_R1 15	/* divider afrer DSS1 unit */

	#define REFERENCE_FREQ	64000000ul
	#define SYNTH_CFCENTER	10760000L	/* Центр полосы пропускания фильтра после DDS */
	//#define REFERENCE_FREQ	32000000ul
	//#define SYNTH_CFCENTER	10760000L	/* Центр полосы пропускания фильтра после DDS */
	#define LO2_PLL_N	0		/* LO2 = 0 MHz */

	#define DDS1_CLK_MUL	1		/* Умножитель в DDS1 */
	#define DDS2_CLK_MUL	1		/* Умножитель в DDS2 */
	#define DDS3_CLK_MUL	1		/* Умножитель в DDS3 */

#endif

#if HYBRID_DDS_ATTINY2313
	#define LO1MODE_HYBRID	1
	/* в качестве подчисточного фильтра за DDS используется пъезофильтр 455 kHz */
	#define SYNTH_CFCENTER	455000L	/* Центр полосы пропускания фильтра после DDS */
	//#define SYNTH_R1		25	/* делитель после DDS - коэфециент деления опоры для PLL */
	#define SYNTH_R1 72
	#define LM7001_PLL1_RCODE	LM7001_RCODE_72
	#define FTW_RESOLUTION 32
	//#define REFERENCE_FREQ	18432000L	// 18.432 kHz - то, что оказалось в наличии
	#define REFERENCE_FREQ	20000000L	// 18.432 kHz - то, что оказалось в наличии
	#define DDS1_CLK_MUL	1		/* Умножитель в DDS1 */
	#define DDS1_CLK_DIV	9		/* Делитель опорной частоты перед подачей в DDS1 (количество тактов на одну выборку выходного значения синусоилы) */

#endif /* HYBRID_DDS_ATTINY2313 */


extern uint_fast8_t s9level;		/* уровни калибровки S-метра */
extern uint_fast8_t s9delta;		// 9 баллов - 8 интервалов - по 6 децибел каждый
extern uint_fast8_t s9_60_delta;		// 60 dB
extern uint_fast16_t minforward;
extern uint_fast8_t swrcalibr;
extern uint_fast8_t maxpwrcali;
extern uint_fast8_t swrmode;

void display_swrmeter(
	uint_fast8_t x,
	uint_fast8_t y,
	adcvalholder_t forward,
	adcvalholder_t reflected, // скорректированное
	uint_fast16_t minforward
	);

uint_fast8_t hamradio_get_tx(void);
int_fast32_t hamradio_get_pbtvalue(void);	// Для отображения на дисплее
uint_fast8_t hamradio_get_atuvalue(void);
uint_fast8_t hamradio_get_genham_value(void);
uint_fast8_t hamradio_get_bypvalue(void);
uint_fast8_t hamradio_get_lockvalue(void);	// текущее состояние LOCK
uint_fast8_t hamradio_get_usefastvalue(void);	// текущее состояние FAST
uint_fast8_t hamradio_get_voxvalue(void);	// текущее состояние VOX
uint_fast8_t hamradio_get_tunemodevalue(void);	// текущее состояние TUNE
uint_fast32_t hamradio_get_freq_pathi(uint_fast8_t pathi);		// Частота VFO A/B для отображения на дисплее
uint_fast32_t hamradio_get_freq_a(void);		// Частота VFO A для отображения на дисплее
uint_fast32_t hamradio_get_freq_b(void);		// Частота VFO B для отображения на дисплее
uint_fast32_t hamradio_get_freq_rx(void);		// Частота VFO A для маркировки файлов
uint_fast32_t hamradio_get_modem_baudrate100(void);	// скорость передачи BPSK * 100
uint_fast8_t hamradio_get_notchvalue(int_fast32_t * p);		// Notch filter ON/OFF
const FLASHMEM char * hamradio_get_notchtype5_P(void);	// FREQ/ANOTCH
uint_fast8_t hamradio_get_nrvalue(int_fast32_t * p);		// NR ON/OFF
const FLASHMEM char * hamradio_get_mode_a_value_P(void);	// SSB/CW/AM/FM/..
const FLASHMEM char * hamradio_get_mode_b_value_P(void);	// SSB/CW/AM/FM/..
const FLASHMEM char * hamradio_get_rxbw_value_P(void);	// RX bandwidth
const FLASHMEM char * hamradio_get_pre_value_P(void);	// RX preamplifier
const FLASHMEM char * hamradio_get_att_value_P(void);	// RX attenuator
const FLASHMEM char * hamradio_get_agc3_value_P(void);	// RX agc time - 3-х буквенные абревиатуры
const FLASHMEM char * hamradio_get_agc4_value_P(void);	// RX agc time - 4-х буквенные абревиатуры
const FLASHMEM char * hamradio_get_ant5_value_P(void);	// antenna
const FLASHMEM char * hamradio_get_mainsubrxmode3_value_P(void);	// текущее состояние DUAL WATCH
const char * hamradio_get_vfomode3_value(uint_fast8_t * flag);	// VFO mode
const char * hamradio_get_vfomode5_value(uint_fast8_t * flag);	// VFO mode
uint_fast8_t hamradio_get_volt_value(void);	// Вольты в десятых долях
int_fast16_t hamradio_get_temperature_value(void);	// Градусы в десятых долях
int_fast16_t hamradio_get_pacurrent_value(void);	// Ток в десятках милиампер, может быть отрицательным
const FLASHMEM char * hamradio_get_hplp_value_P(void);	// HP/LP
uint_fast8_t hamradio_get_rec_value(void);	// AUDIO recording state
uint_fast8_t hamradio_get_amfm_highcut10_value(uint_fast8_t * flag);	// текущее значение верхней частоты среза НЧ фильтра АМ/ЧМ (в десятках герц)
uint_fast8_t hamradio_get_samdelta10(int_fast32_t * p, uint_fast8_t pathi);		/* Получить значение отклонения частоты с точностью 0.1 герца */
uint_fast8_t hamradio_get_usbh_active(void);
uint_fast8_t hamradio_get_datamode(void);	// источник звука для передачи - USB AUDIO
int_fast16_t hamradio_getleft_bp(uint_fast8_t pathi);	/* получить левый (низкочастотный) скат полосы пропускания для отображения "шторки" на спектранализаторе */
int_fast16_t hamradio_getright_bp(uint_fast8_t pathi);	/* получить правый (высокочастотный) скат полосы пропускания для отображения "шторки" на спектранализаторе */
uint_fast8_t hamradio_get_bkin_value(void);
uint_fast8_t hamradio_get_spkon_value(void);	// не-0: динамик включен

void hamradio_change_submode(uint_fast8_t newsubmode, uint_fast8_t need_correct_freq);
uint_fast8_t hamradio_get_low_bp(int_least16_t rotate);
uint_fast8_t hamradio_get_high_bp(int_least16_t rotate);
uint_fast8_t hamradio_get_bp_type(void);
void hamradio_set_agc_off(void);
void hamradio_set_agc_slow(void);
void hamradio_set_agc_fast(void);
void hamradio_disable_keyboard_redirect(void);
void hamradio_enable_keyboard_redirect(void);
uint_fast8_t hamradio_set_freq (uint_fast32_t freq);
void hamradio_set_lockmode (uint_fast8_t lock);
int_fast16_t hamradio_if_shift(int_fast8_t step);
uint_fast8_t hamradio_get_cw_wpm(void);
uint_fast8_t hamradio_get_gmikeequalizer(void);
void hamradio_set_gmikeequalizer(uint_fast8_t v);
uint_fast8_t hamradio_get_gmikeequalizerparams(uint_fast8_t i);
void hamradio_set_gmikeequalizerparams(uint_fast8_t i, uint_fast8_t v);
int_fast32_t hamradio_getequalizerbase(void);

#if WITHREVERB
void hamradio_set_greverb(uint_fast8_t v);
uint_fast8_t hamradio_get_greverb(void);
void hamradio_get_reverb_delay_limits(uint_fast8_t * min, uint_fast8_t * max);
void hamradio_get_reverb_loss_limits(uint_fast8_t * min, uint_fast8_t * max);
uint_fast8_t hamradio_get_reverb_delay(void);
uint_fast8_t hamradio_get_reverb_loss(void);
void hamradio_set_reverb_delay(uint_fast8_t v);
void hamradio_set_reverb_loss(uint_fast8_t v);
#endif /* WITHREVERB */

void hamradio_set_autonotch(uint_fast8_t v);
uint_fast8_t hamradio_get_autonotch(void);
void hamradio_set_gmoniflag(uint_fast8_t v);
uint_fast8_t hamradio_get_gmoniflag(void);
uint_fast8_t hamradio_get_gmikebust20db(void);
void hamradio_set_gmikebust20db(uint_fast8_t v);
uint_fast8_t hamradio_get_gmikeagc(void);
void hamradio_set_gmikeagc(uint_fast8_t v);
void hamradio_get_mic_level_limits(uint_fast8_t * min, uint_fast8_t * max);
void hamradio_set_mik1level(uint_fast8_t v);
uint_fast8_t hamradio_get_mik1level(void);
void hamradio_get_mic_clip_limits(uint_fast8_t * min, uint_fast8_t * max);
uint_fast8_t hamradio_get_gmikehclip(void);
void hamradio_set_gmikehclip(uint_fast8_t v);
void hamradio_get_mic_agc_limits(uint_fast8_t * min, uint_fast8_t * max);
uint_fast8_t hamradio_get_gmikeagcgain(void);
void hamradio_set_gmikeagcgain(uint_fast8_t v);

#if WITHVOX
void hamradio_set_gvoxenable(uint_fast8_t v);
uint_fast8_t hamradio_get_gvoxenable(void);
void hamradio_get_vox_delay_limits(uint_fast8_t * min, uint_fast8_t * max);
uint_fast8_t hamradio_get_vox_delay(void);
void hamradio_set_vox_delay(uint_fast8_t v);
void hamradio_get_vox_level_limits(uint_fast8_t * min, uint_fast8_t * max);
uint_fast8_t hamradio_get_vox_level(void);
void hamradio_set_vox_level(uint_fast8_t v);
void hamradio_get_antivox_delay_limits(uint_fast8_t * min, uint_fast8_t * max);
uint_fast8_t hamradio_get_antivox_level(void);
void hamradio_set_antivox_level(uint_fast8_t v);
#endif /* WITHVOX */

#if WITHTX
void hamradio_set_tune(uint_fast8_t v);
void hamradio_set_tx_tune_power(uint_fast8_t v);
uint_fast8_t hamradio_get_tx_tune_power(void);
void hamradio_set_tx_power(uint_fast8_t v);
uint_fast8_t hamradio_get_tx_power(void);
void hamradio_get_tx_power_limits(uint_fast8_t * min, uint_fast8_t * max);
#endif /* WITHTX */

#if WITHSPKMUTE
uint_fast8_t hamradio_get_gmutespkr(void);
void hamradio_set_gmutespkr(uint_fast8_t v);
#endif /* WITHSPKMUTE */

uint_fast8_t hamradio_verify_freq_bands(uint_fast32_t freq, uint_fast32_t * bottom, uint_fast32_t * top);

/* выбор внешнего вида прибора - стрелочный или градусник */
enum
{
	SMETER_TYPE_BARS,
	SMETER_TYPE_DIAL,
	SMETER_TYPE_COUNT
};

/* Управление частичной полосоц отображением спектра/волопада */
enum
{
	SPECTRUMWIDTH_MULT = 11,	// 44 кГц
	SPECTRUMWIDTH_DENOM = 12,
};

uint_fast8_t hamradio_get_gsmetertype(void);
void display2_set_smetertype(uint_fast8_t v);


const char * get_band_label3(unsigned b); /* получение человекопонятного названия диапазона */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RADIO_H_INCLUDED */



