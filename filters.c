/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"


#include "display.h"
#include "formats.h"

// Названия фильтров для отображения на дисплее

static const char FLASHMEM 
	strFlashWide [] = "WID",
	strFlashNarrow [] = "NAR",

	strFlash0p3 [] = "0.3",
	strFlash0p5 [] = "0.5",
	strFlash1p0 [] = "1.0",
	strFlash1p5 [] = "1.5",
	strFlash1p8 [] = "1.8",
	strFlash2p0 [] = "2.0",
	strFlash2p1 [] = "2.1",
	strFlash2p4 [] = "2.4",
	strFlash2p7 [] = "2.7",
	strFlash3p0 [] = "3.0",
	strFlash3p1 [] = "3.1",
	strFlash6p0 [] = "6.0",
	strFlash7p8 [] = "7.8",
	strFlash8p0 [] = "8.0",
	strFlash9p0 [] = "9.0",
	strFlash10p0 [] = "10.",
	strFlash18p0 [] = "18.",
	strFlash17p0 [] = "17.",
	strFlash15p0 [] = "15.",
	strFlash120p0 [] = "120";


#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_128)
// 
#define IF3LOWER 126600ul
#define IF3CWCENTER 128000ul
#define IF3UPPER 129400ul
#define IF3MIDDLE ((IF3LOWER + IF3UPPER) / 2)

#define LO4FREQ	128000ul		/* частота BFO для случая работы без последней DDS */

#define IF3FREQBASE (IF3CWCENTER - IF3OFFS)

#if (LO2_SIDE != LOCODE_INVALID)

	// LO2 формируется PLL или прямо берётся с опорного генератора
	static const lo2param_t lo2param =
	{
		{	LO2_PLL_N, LO2_PLL_N, },	// lo2n: { rx, tx }
		{	LO2_PLL_R, LO2_PLL_R, },	// lo2r: { rx, tx }
		IF3FREQBASE,
	};

#else

	static const lo2param_t lo2param =
	{
		{	0, 0, },	// lo2n: { rx, tx }
		{	1, 1, },	// lo2r: { rx, tx }
		IF3FREQBASE,
	};

#endif


#if (IF3_FMASK & IF3_FMASK_3P1)
filter_t fi_3p1 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	(1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_CWZ),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3LOWER - IF3FREQBASE),
	(uint16_t) (IF3UPPER - IF3FREQBASE),
	{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_3P1),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,				/* lo2 parameters set index for this filter */
	strFlash3p1,	/* название фильтра для отображения */
};

	#if WITHTX && WITHSAMEBFO == 0
	filter_t fi_3p1_tx =
	{
		0,
		(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
		0,
		(uint16_t) (IF3LOWER - IF3FREQBASE),
		(uint16_t) (IF3UPPER - IF3FREQBASE),
		{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* код выбора данного фильтра на плате приемника */
		0x01,	/* этот фильтр установлен */
		0,	/* 0 - считается "узким", 1 - "широким" */
		IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
		& lo2param,				/* lo2 parameters set index for this filter */
		strFlash3p1,	/* название фильтра для отображения */
	};
	#endif /* WITHTX && WITHSAMEBFO == 0 */
#endif


#if (IF3_FMASK & IF3_FMASK_0P5)
filter_t fi_0p5 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,	/* TX modes */
	(1U << MODE_CW) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3CWCENTER - IF3FREQBASE),	/* средняя частота фильтра */
	(uint16_t) 0,
	{ BOARD_FILTER_0P5, BOARD_FILTER_0P5, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_0P5),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,				/* lo2 parameters set index for this filter */
	strFlash0p5,	/* название фильтра для отображения */
};
#endif

#if (IF3_FMASK & IF3_FMASK_1P8)
filter_t fi_1p8 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,	/* TX modes */
	0,	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3LOWER - IF3FREQBASE),
	(uint16_t) (IF3UPPER - IF3FREQBASE),
	{ BOARD_FILTER_1P8, BOARD_FILTER_1P8, },					/* код выбора данного фильтра на плате приемника (заменяет телеграфный) */
	0 != (IF3_FHAVE & IF3_FMASK_1P8),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,				/* lo2 parameters set index for this filter */
	strFlash1p8,	/* название фильтра для отображения */
};
#endif

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_128) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_200)

#define IF3FREQBASE (200000UL - IF3OFFS)
#define IF3CWCENTER 201000UL
#define LO4FREQ	200000UL		/* частота BFO для случая работы без последней DDS */

#if (LO2_SIDE != LOCODE_INVALID)

	// LO2 формируется PLL или прямо берётся с опорного генератора
	static const lo2param_t lo2param =
	{
		{	LO2_PLL_N, LO2_PLL_N, },	// lo2n
		{	LO2_PLL_R, LO2_PLL_R, },	// lo2r
		IF3FREQBASE,
	};

#else

	static const lo2param_t lo2param =
	{
		{	0, 0, },	// lo2n
		{	1, 1, },	// lo2r
		IF3FREQBASE,
	};

#endif

#if (IF3_FMASK & IF3_FMASK_3P1)
filter_t fi_3p1 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) ((200000 + 0) - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
	(uint16_t) ((200000 + 3700) - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_3P1),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,				/* lo2 parameters set index for this filter */
	strFlash3p1,		/* название фильтра для отображения */
};
	#if WITHTX && WITHSAMEBFO == 0
	filter_t fi_3p1_tx =
	{
		0,
		(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
		0,
		(uint16_t) (200000 - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
		(uint16_t) (203700 - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
		{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* код выбора данного фильтра на плате приемника */
		0x01,	/* этот фильтр установлен */
		0,	/* 0 - считается "узким", 1 - "широким" */
		IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
		& lo2param,				/* lo2 parameters set index for this filter */
		strFlash3p1,		/* название фильтра для отображения */
	};
	#endif
#endif

#if (IF3_FMASK & IF3_FMASK_1P8)
filter_t fi_1p8 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) ((200000 + 0) - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
	(uint16_t) ((200000 + 3700) - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_1P8, BOARD_FILTER_1P8, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_3P1),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,				/* lo2 parameters set index for this filter */
	strFlash1p8,		/* название фильтра для отображения */
};
#endif


#if (IF3_FMASK & IF3_FMASK_2P7)
filter_t fi_2p7 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) ((200000 + 0) - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
	(uint16_t) ((200000 + 2700) - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_2P7, BOARD_FILTER_2P7, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_3P1),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,				/* lo2 parameters set index for this filter */
	strFlash2p7,		/* название фильтра для отображения */
};
	#if WITHTX && WITHSAMEBFO == 0
	filter_t fi_2p7_tx =
	{
		0,
		(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
		0,
		(uint16_t) ((200000 + 0) - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
		(uint16_t) ((200000 + 2700) - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
		{ BOARD_FILTER_2P7, BOARD_FILTER_2P7, },					/* код выбора данного фильтра на плате приемника */
		0x01,		/* этот фильтр установлен */
		0,	/* 0 - считается "узким", 1 - "широким" */
		IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
		& lo2param,				/* lo2 parameters set index for this filter */
		strFlash2p7,		/* название фильтра для отображения */
	};
	#endif
#endif

#if (IF3_FMASK & IF3_FMASK_0P5)
filter_t fi_0p5 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,
	(1U << MODE_CW),						/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3CWCENTER - IF3FREQBASE),          /* частота центра полосы пропускания */
	(uint16_t) (0),						/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_0P5, BOARD_FILTER_0P5, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_0P5),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,				/* lo2 parameters set index for this filter */
	strFlash0p5,		/* название фильтра для отображения */
};
#endif

#if (IF3_FMASK & IF3_FMASK_6P0)

filter_t fi_6p0 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	0,
	(1U << MODE_AM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) ((200000 + 0) - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
	(uint16_t) ((200000 + 6000) - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
	{ BOARD_FILTER_6P0, BOARD_FILTER_6P0, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_6P0),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,				/* lo2 parameters set index for this filter */
	strFlash6p0,	/* название фильтра для отображения */
};

#endif

#endif	/* IF3_TYPE_200 */

#if CTLSTYLE_RA4YBO_V1

/*
1. в ССБ будет один ЭМФ _3,1 кГц от Аверс,который ты мне подогнал летом.(код 00)
2. в АМ,ФМ три коммутируемых пьезофильтра на 455 кГц с полосами 3,0 кГц(код 10); 6,0кГц(код 01) ;10,0кГц.(код 11)
на передачу ,не коммутируются, в формирователе ССБ отдельный ЭМФ на 500 кГц полосой 2,35 или 3,1 кГц, В или Н пока не знаю.
на передачу ,не коммутируются,  в АМ  хочу поставить пьезо 455 кГц полосой 6 или 10кГц.
*/

#define IF3FREQBASE (500000L - IF3OFFS)

#define IF3CWCENTER455 455000L
#define IF3FREQBASE455 (IF3CWCENTER455 - IF3OFFS)

static const lo2param_t lo2param500 =
{
	// 500 kHz parameters
	{	LO2_PLL_N500, LO2_PLL_N500, },	// lo2n
	{	LO2_PLL_R500, LO2_PLL_R500, },	// lo2r
	IF3FREQBASE,
};

static const lo2param_t lo2param455 =
{
	// 455 kHz parameters
	{	LO2_PLL_N455, LO2_PLL_N455, },	// lo2n
	{	LO2_PLL_R455, LO2_PLL_R455, },	// lo2r
	IF3FREQBASE455,
};

static const lo2param_t lo2paramWFM =
{
	// 10.7 MHz parameters
	{	0, 0, },	// lo2n
	{	LO2_PLL_R455, LO2_PLL_R455, },	// lo2r
	WFM_IF1,
};

static const filter_t fi_wfm =
{
	(1U << MODE_WFM),	/* rx: фильтр допустим для этих режимов */
	0,					/* tx: фильтр допустим для этих режимов */
	(1U << MODE_WFM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) 0,		/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	BOARD_FILTER_WFM,					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	0,    	    /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2paramWFM,				/* lo2 parameters set index for this filter */
	strFlash120p0,		/* название фильтра для отображения */
};

filter_t fi_3p0_455 =
{
	(1U << MODE_AM) | (1U << MODE_NFM),		/* RX mask */
	0,									/* tx: фильтр допустим для этих режимов */
	0, //(1U << MODE_AM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	0x03,					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash3p0,	/* название фильтра для отображения */
};

filter_t fi_6p0_455 =
{
	(1U << MODE_AM) | (1U << MODE_NFM),	/* RX mask */
	(1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_TUNE),	/* tx: фильтр допустим для этих режимов */
	(1U << MODE_AM) | (1U << MODE_NFM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	0x01,					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash6p0,	/* название фильтра для отображения */
};

filter_t fi_10p0_455 =
{
	(1U << MODE_AM) | (1U << MODE_NFM),	/* RX mask */
	0,									/* TX mask: фильтр допустим для этих режимов */
	0, //(1U << MODE_CW),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	0x02,					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash10p0,	/* название фильтра для отображения */
};

// 500 kHz
filter_t fi_3p1 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* RX mask */
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* tx: фильтр допустим для этих режимов */
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (500000L - IF3FREQBASE),
	(uint16_t) (503700L - IF3FREQBASE),
	0x00,					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash3p1,	/* название фильтра для отображения */
};

// 500 kHz
filter_t fi_3p1_tx =
{
	0, //(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* RX mask */
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* tx: фильтр допустим для этих режимов */
	0, //(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (500000L - IF3FREQBASE),
	(uint16_t) (503700L - IF3FREQBASE),
	0x00,					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash3p1,	/* название фильтра для отображения */
};

#endif /* CTLSTYLE_RA4YBO_V1 */

#if CTLSTYLE_RA4YBO_V2

/*
1. в ССБ будет один ЭМФ _3,1 кГц от Аверс,который ты мне подогнал летом.(код 00)
2. в АМ,ФМ три коммутируемых пьезофильтра на 455 кГц с полосами 3,0 кГц(код 10); 6,0кГц(код 01) ;10,0кГц.(код 11)
на передачу ,не коммутируются, в формирователе ССБ отдельный ЭМФ на 500 кГц полосой 2,35 или 3,1 кГц, В или Н пока не знаю.
на передачу ,не коммутируются,  в АМ  хочу поставить пьезо 455 кГц полосой 6 или 10кГц.
*/

#define IF3FREQBASE (500000L - IF3OFFS)

#define IF3CWCENTER455 455000L
#define IF3FREQBASE455 (IF3CWCENTER455 - IF3OFFS)

static const lo2param_t lo2param500 =
{
	// 500 kHz parameters
	{	LO2_PLL_N500, LO2_PLL_N500, },	// lo2n
	{	LO2_PLL_R500, LO2_PLL_R500, },	// lo2r
	IF3FREQBASE,
};

static const lo2param_t lo2param455 =
{
	// 455 kHz parameters
	{	LO2_PLL_N455, LO2_PLL_N455, },	// lo2n
	{	LO2_PLL_R455, LO2_PLL_R455, },	// lo2r
	IF3FREQBASE455,
};

static const lo2param_t lo2paramWFM =
{
	// 10.7 MHz parameters
	{	0, 0, },	// lo2n
	{	LO2_PLL_R455, LO2_PLL_R455, },	// lo2r
	WFM_IF1,
};


static const filter_t fi_wfm =
{
	(1U << MODE_WFM),	/* rx: фильтр допустим для этих режимов */
	0,					/* tx: фильтр допустим для этих режимов */
	(1U << MODE_WFM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) 0,		/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	0x07,					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	0,    	    /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2paramWFM,				/* lo2 parameters set index for this filter */
	strFlash120p0,		/* название фильтра для отображения */
};

filter_t fi_3p0_455 =
{
	(1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM),		/* RX mask */
	0,									/* tx: фильтр допустим для этих режимов */
	0, //(1U << MODE_AM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	0x01,					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash3p0,	/* название фильтра для отображения */
};

filter_t fi_6p0_455 =
{
	(1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM),	/* RX mask */
	(1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_TUNE),	/* tx: фильтр допустим для этих режимов */
	(1U << MODE_AM) | (1U << MODE_NFM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	0x02,					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash6p0,	/* название фильтра для отображения */
};

filter_t fi_10p0_455 =
{
	(1U << MODE_AM) | (1U << MODE_NFM),	/* RX mask */
	0,									/* TX mask: фильтр допустим для этих режимов */
	0, //(1U << MODE_CW),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	0x03,					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash10p0,	/* название фильтра для отображения */
};

// 500 kHz
filter_t fi_3p1 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* RX mask */
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* tx: фильтр допустим для этих режимов */
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (500000L - IF3FREQBASE),
	(uint16_t) (503700L - IF3FREQBASE),
	0x00,					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash3p1,	/* название фильтра для отображения */
};

#endif /* CTLSTYLE_RA4YBO_V2 */

#if CTLSTYLE_RA4YBO_V3

/*
1. в ССБ будет один ЭМФ _3,1 кГц от Аверс,который ты мне подогнал летом.(код 00)
2. в АМ,ФМ три коммутируемых пьезофильтра на 455 кГц с полосами 3,0 кГц(код 10); 6,0кГц(код 01) ;10,0кГц.(код 11)
на передачу ,не коммутируются, в формирователе ССБ отдельный ЭМФ на 500 кГц полосой 2,35 или 3,1 кГц, В или Н пока не знаю.
на передачу ,не коммутируются,  в АМ  хочу поставить пьезо 455 кГц полосой 6 или 10кГц.
*/

#define IF3FREQBASE (500000L - IF3OFFS)

#define IF3CWCENTER455 455000L
#define IF3FREQBASE455 (IF3CWCENTER455 - IF3OFFS)
#define IF3LOWER455 (IF3CWCENTER455-1500)	// (IF3CWCENTER - 1800L)
#define IF3UPPER455 (IF3CWCENTER455+1500)	// (IF3CWCENTER + 1800L)

static const lo2param_t lo2param500 =
{
	// 500 kHz parameters
	{	LO2_PLL_N500, LO2_PLL_N500, },	// lo2n
	{	LO2_PLL_R500, LO2_PLL_R500, },	// lo2r
	IF3FREQBASE,
};

static const lo2param_t lo2param455 =
{
	// 455 kHz parameters
	{	LO2_PLL_N455, LO2_PLL_N455, },	// lo2n
	{	LO2_PLL_R455, LO2_PLL_R455, },	// lo2r
	IF3FREQBASE455,
};

static const lo2param_t lo2paramWFM =
{
	// 10.7 MHz parameters
	{	LO2_PLL_N455, LO2_PLL_N455, },	// lo2n - не нули, чтобы не болтался гетеродин
	{	LO2_PLL_R455, LO2_PLL_R455, },	// lo2r
	WFM_IF1,
};

static const filter_t fi_wfm =
{
	(1U << MODE_WFM),	/* rx: фильтр допустим для этих режимов */
	0,					/* tx: фильтр допустим для этих режимов */
	(1U << MODE_WFM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) 0,		/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	{ BOARD_FILTER_RX_WFM, BOARD_FILTER_OFF, },					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	0,    	    /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2paramWFM,				/* lo2 parameters set index for this filter */
	strFlash120p0,		/* название фильтра для отображения */
};

// mechanical filter emf-500-0.6-S - RX only
filter_t fi_0p5 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,																			/* tx: фильтр допустим для этих режимов */
	(1U << MODE_CW),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (500000L - IF3FREQBASE),	/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	{ BOARD_FILTER500_RX_0P5, BOARD_FILTER_OFF, },					/* код выбора данного фильтра на плате приемника */
	1,			/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param500,			/* lo2 parameters set index for this filter */
	strFlash0p5,		/* название фильтра для отображения */
};

// mechanical 500 kHz emf-500-3.1-N	- RX/TX 
filter_t fi_3p1 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* RX mask */
	(1U << MODE_CW) | (1U << MODE_SSB) | (1U << MODE_TUNE),
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (496610L - IF3FREQBASE),
	(uint16_t) (500000L - IF3FREQBASE),
	{ BOARD_FILTER500_RX_3P1, BOARD_FILTER500_TX_3P1, },					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash3p1,	/* название фильтра для отображения */
};

// collins filter - RX only
// 2.4kHz filter
filter_t fi_3p0_455 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),		/* RX mask */
	0,	/* tx: фильтр допустим для этих режимов */
	0, //(1U << MODE_AM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3LOWER455 - IF3FREQBASE455),
	(uint16_t) (IF3UPPER455 - IF3FREQBASE455),
	{ BOARD_FILTER455_RX_3P0, BOARD_FILTER_OFF, },					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash2p4,	/* название фильтра для отображения */
};

// piezo filter - RX/TX
filter_t fi_10p0_455 =
{
	(1U << MODE_AM) | (1U << MODE_NFM),	/* RX mask */
	(1U << MODE_AM) | (1U << MODE_NFM),									/* TX mask: фильтр допустим для этих режимов */
	0, //(1U << MODE_CW),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	{ BOARD_FILTER455_RX_10P0, BOARD_FILTER455_TX_10P0, },					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash10p0,	/* название фильтра для отображения */
};

// piezo filter - RX only
filter_t fi_6p0_455 =
{
	(1U << MODE_AM) | (1U << MODE_NFM),	/* RX mask */
	0,	/* tx: фильтр допустим для этих режимов */
	(1U << MODE_AM) | (1U << MODE_NFM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	{ BOARD_FILTER455_RX_6P0, BOARD_FILTER_OFF, },					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash6p0,	/* название фильтра для отображения */
};

// piezo filter - RX only
filter_t fi_2p0_455 =
{
	(1U << MODE_AM) | (1U << MODE_NFM),	/* RX mask */
	0,											/* tx: фильтр допустим для этих режимов */
	0,	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3CWCENTER455 - IF3FREQBASE455),	/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	{ BOARD_FILTER455_RX_2P1, BOARD_FILTER_OFF, },					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param455,	/* lo2 parameters set index for this filter */
	strFlash2p0,	/* название фильтра для отображения */
};


#endif /* CTLSTYLE_RA4YBO_V3 */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_500)

#define IF3MIDDLE 500000L
#if BOARD_FILTER_0P5_LSB
	#define IF3CWCENTER 499650L
#else /* BOARD_FILTER_0P5_LSB */
	#define IF3CWCENTER 500000L
#endif /* BOARD_FILTER_0P5_LSB */
#define IF3LOWER 500000L
#define IF3UPPER 503700L
#define IF3FREQBASE (IF3MIDDLE - IF3OFFS)
#define LO4FREQ	IF3LOWER		/* частота BFO для случая работы без последней DDS */

#if CTLSTYLE_RA4YBO

static const lo2param_t lo2param500 =
{
	{	LO2_PLL_N_RX, LO2_PLL_N_TX, },	// lo2n
	{	LO2_PLL_R_RX, LO2_PLL_R_TX, },	// lo2r
	IF3FREQBASE,
};

static const lo2param_t lo2paramWFM =
{
	{	0, 0, },						// lo2n
	{	LO2_PLL_R_RX, LO2_PLL_R_TX, },	// lo2r
	WFM_IF1,
};

static const filter_t fi_wfm =
{
	(1U << MODE_WFM),	/* rx: фильтр допустим для этих режимов */
	0,					/* tx: фильтр допустим для этих режимов */
	(1U << MODE_WFM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) 0,		/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	BOARD_FILTER_WFM,					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	0,    	    /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2paramWFM,	/* lo2 parameters set index for this filter */
	strFlash120p0,		/* название фильтра для отображения */
};

//Вобщем у меня сейчас стоят на плате:0,3С_0,6С _1,5С _2,35В_3,1Н_7,8С_17,0С

// 500-0.3-s
filter_t fi_0p3 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,		// Set of TX modes
	0, //(1U << MODE_CW),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3CWCENTER - IF3FREQBASE),	/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	{ BOARD_FILTER_0P3, BOARD_FILTER_0P3, },					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash0p3,	/* название фильтра для отображения */
};
// 500-0.6-s
filter_t fi_0p5 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,		// Set of TX modes
	(1U << MODE_CW) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3CWCENTER - IF3FREQBASE),	/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	{ BOARD_FILTER_0P5, BOARD_FILTER_0P5, },					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash0p5,	/* название фильтра для отображения */
};
// 500-1.0-s
filter_t fi_1p5 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_SSB),
	0,		// Set of TX modes
	0, //(1U << MODE_CW),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3CWCENTER - IF3FREQBASE),	/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	{ BOARD_FILTER_1P5, BOARD_FILTER_1P5, },					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash1p5,	/* название фильтра для отображения */
};


// 500-2.35-V
filter_t fi_2p4 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,		// Set of TX modes
	0, //(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (500100UL - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
	(uint16_t) (503050UL - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_2P4, BOARD_FILTER_2P4, },					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash2p4,	/* название фильтра для отображения */
};

// 500-3.1-N
filter_t fi_3p1 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	// Set of TX modes
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (496400UL - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
	(uint16_t) (500200UL - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_3P1),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash3p1,		/* название фильтра для отображения */
};


// 500-7.8-s
filter_t fi_7p8 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	0,		// Set of TX modes
	(1U << MODE_AM) | (1U << MODE_NFM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) ((IF3MIDDLE - IF3FREQBASE) - (7800 / 2)),	/* частота нижнего среза полосы пропускания */
	(uint16_t) ((IF3MIDDLE - IF3FREQBASE) + (7800 / 2)),	/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_7P8, BOARD_FILTER_7P8, },					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash7p8,	/* название фильтра для отображения */
};
// 500-17.0-s
filter_t fi_17p0 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	0,		// Set of TX modes
	0, //(1U << MODE_CW),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) ((IF3MIDDLE - IF3FREQBASE) - (17000 / 2)),	/* частота нижнего среза полосы пропускания */
	(uint16_t) ((IF3MIDDLE - IF3FREQBASE) + (17000 / 2)),	/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_17P0, BOARD_FILTER_17P0, },					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash17p0,	/* название фильтра для отображения */
};

// 500-3.1-v
	#if WITHTX && WITHSAMEBFO == 0
	filter_t fi_3p1_tx =
	{
		0, //(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
		(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
		0, //(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
		(uint16_t) (496400UL - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
		(uint16_t) (500200UL - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
		{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* код выбора данного фильтра на плате приемника */
		0x01,	/* этот фильтр установлен */
		0,	/* 0 - считается "узким", 1 - "широким" */
		IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
		& lo2param500,	/* lo2 parameters set index for this filter */
		strFlash3p1,		/* название фильтра для отображения */
	};
	#endif
// 500-17.0-s
filter_t fi_17p0_tx =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),		// Set of TX modes
	0, //(1U << MODE_CW),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) ((IF3MIDDLE - IF3FREQBASE) - (17000 / 2)),	/* частота нижнего среза полосы пропускания */
	(uint16_t) ((IF3MIDDLE - IF3FREQBASE) + (17000 / 2)),	/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_17P0, BOARD_FILTER_17P0, },					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param500,	/* lo2 parameters set index for this filter */
	strFlash17p0,	/* название фильтра для отображения */
};


#else /* CTLSTYLE_RA4YBO */


static const lo2param_t lo2param =
{
	{	LO2_PLL_N, LO2_PLL_N, },	// lo2n
	{	LO2_PLL_R, LO2_PLL_R, },	// lo2r
	IF3FREQBASE,
};

	// filter codes:
	// 0x06 - fil3 3.1 kHz
	// 0x01 - fil2 2.7 kHz
	// 0x00 - fil0 9.0 kHz
	// 0x02 - fil1 6.0 kHz
	// 0x03 - fil5 0.5 kHz
	// 0x04 - fil2 15 kHz (bypass)
	// 0x05 - unused
	// 0x07 - unused

#if (IF3_FMASK & IF3_FMASK_2P7)
// 500-2.7-n
filter_t fi_2p7 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM), // Set of TX modes
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (496800UL - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
	(uint16_t) (500300UL - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_2P7, BOARD_FILTER_2P7, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_2P7),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash2p7,		/* название фильтра для отображения */
};

// 500-2.7-n

	#if WITHTX && WITHSAMEBFO == 0
	filter_t fi_2p7_tx =
	{
		0,
		(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
		0,
		(uint16_t) (496800UL - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
		(uint16_t) (500300UL - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
		{ BOARD_FILTER_2P7, BOARD_FILTER_2P7, },					/* код выбора данного фильтра на плате приемника */
		0x01,	/* этот фильтр установлен */
		0,	/* 0 - считается "узким", 1 - "широким" */
		IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
		& lo2param,			/* lo2 parameters set index for this filter */
		strFlash2p7,		/* название фильтра для отображения */
	};
	#endif
#endif


#if (IF3_FMASK & IF3_FMASK_3P1)

// 500-3-v
filter_t fi_3p1 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	#if BOARD_FILTER_3P1_LSB
	(uint16_t) (496300UL - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
	(uint16_t) (500000UL - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
	#else /* BOARD_FILTER_3P1_LSB */
	(uint16_t) (500000UL - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
	(uint16_t) (503700UL - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
	#endif /* BOARD_FILTER_3P1_LSB */
	{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_3P1),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash3p1,		/* название фильтра для отображения */
};
	#if WITHTX && WITHSAMEBFO == 0
	filter_t fi_3p1_tx =
	{
		0,
		(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
		0,
		#if BOARD_FILTER_3P1_LSB
		(uint16_t) (496300UL - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
		(uint16_t) (500000UL - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
		#else /* BOARD_FILTER_3P1_LSB */
		(uint16_t) (500000UL - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
		(uint16_t) (503700UL - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
		#endif /* BOARD_FILTER_3P1_LSB */
		{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* код выбора данного фильтра на плате приемника */
		0x01,	/* этот фильтр установлен */
		0,	/* 0 - считается "узким", 1 - "широким" */
		IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
		& lo2param,			/* lo2 parameters set index for this filter */
		strFlash3p1,		/* название фильтра для отображения */
	};
	#endif
#endif


#if (IF3_FMASK & IF3_FMASK_0P3)
// 500-0.5-v
filter_t fi_0p3 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ),
	0,
	(1U << MODE_CW),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3CWCENTER - IF3FREQBASE),	/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	{ BOARD_FILTER_0P3, BOARD_FILTER_0P3, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_0P3),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash0p3,		/* название фильтра для отображения */
};
#endif

#if (IF3_FMASK & IF3_FMASK_0P5)
// 500-0.5-v
filter_t fi_0p5 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ),
	0,
	(1U << MODE_CW),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3CWCENTER - IF3FREQBASE),	/* центральная частота полосы пропускания */
	(uint16_t) 0,				
	{ BOARD_FILTER_0P5, BOARD_FILTER_0P5, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_0P5),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash0p5,		/* название фильтра для отображения */
};
#endif

#if (IF3_FMASK & IF3_FMASK_2P4)
// Используется 2.7 кГц фильтр всесто 6 кГц
// 500-2.4-n
filter_t fi_2p4 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (497100UL - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
	(uint16_t) (500000UL - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_2P4, BOARD_FILTER_2P4, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_2P4),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash2p4,		/* название фильтра для отображения */
};
#endif

#if (IF3_FMASK & IF3_FMASK_1P8)
filter_t fi_1p8 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI) | (1U << MODE_NFM),
	0,					/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (497100UL - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
	(uint16_t) (500000UL - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_1P8, BOARD_FILTER_1P8, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_1P8),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash1p8,		/* название фильтра для отображения */
};
#endif

#if (IF3_FMASK & IF3_FMASK_6P0)
filter_t fi_6p0 =
{
	(1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_DRM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,
	(1U << MODE_DRM) | (1U << MODE_NFM) | (1U << MODE_AM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3MIDDLE - 3500L - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
	(uint16_t) (IF3MIDDLE + 3500L - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_6P0, BOARD_FILTER_6P0, },					/* код выбора данного фильтра на плате приемника */
	0x00,	/* при 500 кГц ПЧ этот фильтр обычно не установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash6p0,		/* название фильтра для отображения */
};
#endif

#if (IF3_FMASK & IF3_FMASK_8P0)
filter_t fi_8p0 =
{
	(1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_DRM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,
	(1U << MODE_DRM) | (1U << MODE_NFM) | (1U << MODE_AM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3MIDDLE - 4500L - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
	(uint16_t) (IF3MIDDLE + 4500L - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_8P0, BOARD_FILTER_8P0, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_8P0),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash8p0,		/* название фильтра для отображения */
};
#endif

#if (IF3_FMASK & IF3_FMASK_9P0)
filter_t fi_9p0 =
{
	(1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_DRM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,
	(1U << MODE_DRM) | (1U << MODE_NFM) | (1U << MODE_AM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3MIDDLE - 5000L - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
	(uint16_t) (IF3MIDDLE + 5000L - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_9P0, BOARD_FILTER_9P0, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_9P0),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash9p0,		/* название фильтра для отображения */
};
#endif

#if (IF3_FMASK & IF3_FMASK_15P0)
// на старой плате с 45 МГц фильтром есть отдельное подключение для FM тракта (фильтр с другим кодом).
// Для 500 кГц версии тракта ПЧ
filter_t fi_15p0_nfm =
{
	(1U << MODE_NFM),
	0,
	(1U << MODE_NFM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3MIDDLE - 7500L - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
	(uint16_t) (IF3MIDDLE + 7500L - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_15P0_NFM, BOARD_FILTER_15P0_NFM, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_15P0),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash15p0,		/* название фильтра для отображения */
};
#endif

#if (IF3_FMASK & IF3_FMASK_15P0)
filter_t fi_15p0 =
{
	(1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_DRM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,
	(1U << MODE_DRM) | (1U << MODE_NFM) | (1U << MODE_AM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3MIDDLE - 7500L - IF3FREQBASE),          /* частота нижнего среза полосы пропускания */
	(uint16_t) (IF3MIDDLE + 7500L - IF3FREQBASE),		/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_15P0, BOARD_FILTER_15P0, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_15P0),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash15p0,		/* название фильтра для отображения */
};
#endif

#endif /* CTLSTYLE_RA4YBO */

#endif/* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_500s) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_455)

	#define IF3_CUSTOM_CW_CENTER	(455000L)
	#define IF3_CUSTOM_SSB_LOWER	(455000L - (3700 / 2))
	#define IF3_CUSTOM_SSB_UPPER	(455000L + (3700 / 2))
	#define IF3_CUSTOM_WIDE_CENTER	(455000L)		/* центральная частота для широких фильтров и passtrough */
	

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_455) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_215)

	#define IF3_CUSTOM_CW_CENTER	215000L
	#define IF3_CUSTOM_SSB_LOWER	213150L
	#define IF3_CUSTOM_SSB_UPPER	216850L
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_215) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_9045)

	#define IF3_CUSTOM_CW_CENTER	9045000L
	#define IF3_CUSTOM_SSB_LOWER	(IF3_CUSTOM_CW_CENTER - 1500)
	#define IF3_CUSTOM_SSB_UPPER	(IF3_CUSTOM_CW_CENTER + 1500)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_9045) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_5000)

	#define IF3_CUSTOM_CW_CENTER	4998404L	//4999404L
	#define IF3_CUSTOM_SSB_LOWER	4997000L	//4999758L
	#define IF3_CUSTOM_SSB_UPPER	5000000L	//5002638L
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_5000) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_5645)

	#define IF3_CUSTOM_CW_CENTER	5645000L
	#define IF3_CUSTOM_SSB_LOWER	(IF3_CUSTOM_CW_CENTER - 1800)
	#define IF3_CUSTOM_SSB_UPPER	(IF3_CUSTOM_CW_CENTER + 1800)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_5645) */


#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_6000)

	#define IF3_CUSTOM_CW_CENTER	5999400L
	#define IF3_CUSTOM_SSB_LOWER	5999300L	// 5999550L
	#define IF3_CUSTOM_SSB_UPPER	6003200L	// 6002650L
	#define IF3_CUSTOM_WIDE_CENTER	6000000L	// 6 kHz filter center freq

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_6000) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_6000_SW2015)

	#define IF3_CUSTOM_CW_CENTER	5999400L
	#define IF3_CUSTOM_SSB_LOWER	5999300L	// 5999550L
	#define IF3_CUSTOM_SSB_UPPER	6002400L	// 6002650L
	#define IF3_CUSTOM_WIDE_CENTER	6000000L	// 6 kHz filter center freq

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_6000_SW2015) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_6000_SW2016MINI)

	#define IF3_CUSTOM_CW_CENTER	5999400L
	#define IF3_CUSTOM_SSB_LOWER	5999500L
	#define IF3_CUSTOM_SSB_UPPER	6002500L
	#define IF3_CUSTOM_WIDE_CENTER	6000000L	// 6 kHz filter center freq

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_6000_SW2015) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_6000_SW2014NFM)

	#define IF3_CUSTOM_CW_CENTER	5999400L
	#define IF3_CUSTOM_SSB_LOWER	5999300L
	#define IF3_CUSTOM_SSB_UPPER	6002300L
	#define IF3_CUSTOM_WIDE_CENTER	6000000L	// 6 kHz filter center freq

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_6000_SW2014NFM) */


#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_5500)

	#define IF3_CUSTOM_CW_CENTER	(5500000L-1500)
	#define IF3_CUSTOM_SSB_LOWER	(IF3_CUSTOM_CW_CENTER - 1500)
	#define IF3_CUSTOM_SSB_UPPER	(IF3_CUSTOM_CW_CENTER + 1500)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_5500) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_5250)

	#define IF3_CUSTOM_CW_CENTER	(5250000L-1500)
	#define IF3_CUSTOM_SSB_LOWER	(IF3_CUSTOM_CW_CENTER - 1500)
	#define IF3_CUSTOM_SSB_UPPER	(IF3_CUSTOM_CW_CENTER + 1500)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_5250) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_9000)

	#define IF3_CUSTOM_CW_CENTER	9000000L
	#define IF3_CUSTOM_SSB_LOWER	8998500L // (IF3_CUSTOM_CW_CENTER - 1500)
	#define IF3_CUSTOM_SSB_UPPER	9001500L // (IF3_CUSTOM_CW_CENTER + 1500)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_9000) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_8192)

	#define IF3_CUSTOM_CW_CENTER	8191000L
	#define IF3_CUSTOM_SSB_LOWER	8190350L //  8190350 - info from 4Z5KY
	#define IF3_CUSTOM_SSB_UPPER	8193850L
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_8192) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_8215)
// кварцевые фильтры от  трансивера FT-747
	#define IF3_CUSTOM_CW_CENTER	8215000L
	#define IF3_CUSTOM_SSB_LOWER	(IF3_CUSTOM_CW_CENTER - 1500L)
	#define IF3_CUSTOM_SSB_UPPER	(IF3_CUSTOM_CW_CENTER + 1500L)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_8215) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_8868)
	// Опора 36.131.00   LSB-8865850 USB-8868700

	#define IF3DF (0)
	//#define IF3DF (8862000L - 8865850L)
	//#define IF3DF (8191000L - 8865850L)
	//#define IF3DF (8975000L - 8865850L)
	//#define IF3DF (8822000L - 8865850L)	// Version for RU3VG

	#define IF3_CUSTOM_CW_CENTER	(8866500L + IF3DF)
	#define IF3_CUSTOM_SSB_LOWER	(8865850L + IF3DF)
	#define IF3_CUSTOM_SSB_UPPER	(8868700L + IF3DF)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_8868) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_8000)
	// hansgo_1969@mail.ru: скаты фильтра SSB 7997,184 - 7999,930. CW - 7997,557 - 7998,126.

	#define IF3_CUSTOM_CW_CENTER	(7997840L)
	#define IF3_CUSTOM_SSB_LOWER	(7997184L)
	#define IF3_CUSTOM_SSB_UPPER	(7999930L)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_8000) */


#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_10000)

	#define IF3_CUSTOM_CW_CENTER	(9998500L)
	#define IF3_CUSTOM_SSB_LOWER	(9996700L)
	#define IF3_CUSTOM_SSB_UPPER	(10000200L)
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_10000) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_10700)

	#define IF3_CUSTOM_CW_CENTER	(10700000L)
	#define IF3_CUSTOM_SSB_LOWER	(10700000L - (3100/2))
	#define IF3_CUSTOM_SSB_UPPER	(10700000L + (3100/2))
	#define IF3_CUSTOM_WIDE_CENTER	((IF3_CUSTOM_SSB_UPPER + IF3_CUSTOM_SSB_LOWER) / 2)

	#undef IF3_MODEL
	#define IF3_MODEL	IF3_TYPE_CUSTOM

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_10700) */


#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_CUSTOM)

#if ! defined (IF3_CUSTOM_WIDE_CENTER) || ! defined (IF3_CUSTOM_CW_CENTER) || ! defined (IF3_CUSTOM_SSB_LOWER) || ! defined (IF3_CUSTOM_SSB_UPPER)
	#error Please define IF3_CUSTOM_WIDE_CENTER, IF3_CUSTOM_CW_CENTER, IF3_CUSTOM_SSB_LOWER and IF3_CUSTOM_SSB_UPPER
#endif

#define IF3WIDECENTER IF3_CUSTOM_WIDE_CENTER
#define IF3CWCENTER IF3_CUSTOM_CW_CENTER
#define IF3LOWER IF3_CUSTOM_SSB_LOWER	// (IF3CWCENTER - 1800L)
#define IF3UPPER IF3_CUSTOM_SSB_UPPER	// (IF3CWCENTER + 1800L)

#define LO4FREQ	IF3LOWER		/* частота BFO для случая работы без последней DDS */

#define IF3FREQBASE (IF3CWCENTER - IF3OFFS)
#define IF3CWCENTER455 (45000000 - 455000)
#define IF3FREQBASE455 (IF3CWCENTER455 - IF3OFFS)



#if (LO2_SIDE != LOCODE_INVALID)

	// LO2 формируется PLL или прямо берётся с опорного генератора
	static const lo2param_t lo2param =
	{
		{	LO2_PLL_N, LO2_PLL_N, },	// lo2n: { rx, tx }
		{	LO2_PLL_R, LO2_PLL_R, },	// lo2r: { rx, tx }
		IF3FREQBASE,
	};

	#if WITHDEDICATEDNFM

	static const lo2param_t lo2param455 =
	{
		// 455 kHz parameters
		{	455000, 455000, },	// lo2n: { rx, tx }
		{	REFERENCE_FREQ, REFERENCE_FREQ, },	// lo2r: { rx, tx }
		IF3FREQBASE455,
	};
	#endif

#else

	static const lo2param_t lo2param =
	{
		{	0, 0, },	// lo2n
		{	1, 1, },	// lo2r
		IF3FREQBASE,
	};

	static const lo2param_t lo2param455 =
	{
		{	0, 0, },	// lo2n
		{	1, 1, },	// lo2r
		IF3FREQBASE455,
	};


#endif

#if (IF3_FMASK & IF3_FMASK_0P5)
filter_t fi_0p5 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,																			/* tx: фильтр допустим для этих режимов */
	(1U << MODE_CW) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3CWCENTER - IF3FREQBASE),	/* средняя частота фильтра */
	(uint16_t) 0,
	{ BOARD_FILTER_0P5, BOARD_FILTER_0P5, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_0P5),	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,		/* lo2 parameters set index for this filter */
	strFlash0p5,	/* название фильтра для отображения */
};
#endif /* (IF3_FMASK & IF3_FMASK_0P5) */

#if (IF3_FMASK & IF3_FMASK_1P8)
filter_t fi_1p8 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,																			/* tx: фильтр допустим для этих режимов */
	0,	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3LOWER - IF3FREQBASE),
	(uint16_t) (IF3UPPER - IF3FREQBASE),
	{ BOARD_FILTER_1P8, BOARD_FILTER_1P8, },					/* код выбора данного фильтра на плате приемника (заменяет телеграфный) */
	0 != (IF3_FHAVE & IF3_FMASK_1P8),	/* этот фильтр установлен */
	1,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,		/* lo2 parameters set index for this filter */
	strFlash1p8,	/* название фильтра для отображения */
};
#endif /* (IF3_FMASK & IF3_FMASK_1P8) */

#if (IF3_FMASK & IF3_FMASK_2P7)
filter_t fi_2p7 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* tx: фильтр допустим для этих режимов */
	(1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_CWZ),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3LOWER - IF3FREQBASE),
	(uint16_t) (IF3UPPER - IF3FREQBASE),
	{ BOARD_FILTER_2P7, BOARD_FILTER_2P7, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_2P7),	/* этот фильтр установлен */
	1,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,		/* lo2 parameters set index for this filter */
	strFlash2p7,	/* название фильтра для отображения */
};
#endif /* (IF3_FMASK & IF3_FMASK_2P7) */

#if (IF3_FMASK & IF3_FMASK_3P1)
filter_t fi_3p1 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* rx: фильтр допустим для этих режимов */
	(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* tx: фильтр допустим для этих режимов */
	(1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_CWZ),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) (IF3LOWER - IF3FREQBASE),
	(uint16_t) (IF3UPPER - IF3FREQBASE),
	{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_3P1),	/* этот фильтр установлен */
	1,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,		/* lo2 parameters set index for this filter */
	strFlash3p1,	/* название фильтра для отображения */
};
#endif /* (IF3_FMASK & IF3_FMASK_2P7) */

#if (IF3_FMASK & IF3_FMASK_6P0)

// ceramic filter
filter_t fi_6p0 =
{
	(1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM)  | (1U << MODE_DRM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,						/* tx: фильтр допустим для этих режимов */
	(1U << MODE_AM) | (1U << MODE_NFM)  | (1U << MODE_DRM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) ((IF3WIDECENTER - IF3FREQBASE) - (6000/2)),	/* частота нижнего среза полосы пропускания */
	(uint16_t) ((IF3WIDECENTER - IF3FREQBASE) + (6000/2)),	/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_6P0, BOARD_FILTER_6P0, },				/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_6P0),	/* этот фильтр установлен */
	1,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,		/* lo2 parameters set index for this filter */
	strFlash6p0,	/* название фильтра для отображения */
};
#endif


#if (IF3_FMASK & IF3_FMASK_8P0)

// passtrough filter
filter_t fi_8p0 =
{
	(1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_DRM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,						/* tx: фильтр допустим для этих режимов */
	(1U << MODE_DRM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) ((IF3WIDECENTER - IF3FREQBASE) - (8000/2)),	/* частота нижнего среза полосы пропускания */
	(uint16_t) ((IF3WIDECENTER - IF3FREQBASE) + (8000/2)),	/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_8P0, BOARD_FILTER_8P0, },				/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_8P0),	/* этот фильтр установлен */
	1,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,		/* lo2 parameters set index for this filter */
	strFlash8p0,	/* название фильтра для отображения */
};
#endif

#if (IF3_FMASK & IF3_FMASK_9P0)
// ceramic filter
filter_t fi_9p0 =
{
	(1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM)  | (1U << MODE_DRM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,						/* tx: фильтр допустим для этих режимов */
	(1U << MODE_AM) | (1U << MODE_NFM)  | (1U << MODE_DRM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) ((IF3WIDECENTER - IF3FREQBASE) - (9000/2)),	/* частота нижнего среза полосы пропускания */
	(uint16_t) ((IF3WIDECENTER - IF3FREQBASE) + (9000/2)),	/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_9P0, BOARD_FILTER_9P0, },				/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_6P0),	/* этот фильтр установлен */
	1,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,		/* lo2 parameters set index for this filter */
	strFlash9p0,	/* название фильтра для отображения */
};
#endif


#if (IF3_FMASK & IF3_FMASK_15P0)
// passtrough filter
filter_t fi_15p0 =
{
	0, //(1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_DRM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,						/* tx: фильтр допустим для этих режимов */
	0, //(1U << MODE_DRM),	/* фильтр "по умолчанию" для этих режимов */
	(uint16_t) ((IF3CWCENTER455 - IF3FREQBASE455) - (15000/2)),	/* частота нижнего среза полосы пропускания */
	(uint16_t) ((IF3CWCENTER455 - IF3FREQBASE455) + (15000/2)),	/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_15P0, BOARD_FILTER_15P0, },				/* код выбора данного фильтра на плате приемника */
	0 != (IF3_FHAVE & IF3_FMASK_15P0),	/* этот фильтр установлен */
	1,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param455,		/* lo2 parameters set index for this filter */
	strFlash15p0,	/* название фильтра для отображения */
};

	#if WITHDEDICATEDNFM /* Для NFM отдельный тракт со своим фильтром */ 

	// passtrough filter
	filter_t fi_15p0_nfm =
	{
		(1U << MODE_NFM),		/* rx: фильтр допустим для этих режимов */
		0,						/* tx: фильтр допустим для этих режимов */
		(1U << MODE_NFM),	/* фильтр "по умолчанию" для этих режимов */
		(uint16_t) ((IF3CWCENTER455 - IF3FREQBASE455) - (15000/2)),	/* частота нижнего среза полосы пропускания */
		(uint16_t) ((IF3CWCENTER455 - IF3FREQBASE455) + (15000/2)),	/* частота верхнего среза полосы пропускания */
		{ BOARD_FILTER_15P0_NFM, BOARD_FILTER_15P0_NFM, },				/* код выбора данного фильтра на плате приемника */
		0 != (IF3_FHAVE & IF3_FMASK_15P0),	/* этот фильтр установлен */
		1,	/* 0 - считается "узким", 1 - "широким" */
		IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
		& lo2param455,		/* lo2 parameters set index for this filter */
		strFlash15p0,	/* название фильтра для отображения */
	};
	// passtrough filter
		#if (IF3_FMASKTX & IF3_FMASK_15P0)
		filter_t fi_15p0_tx_nfm =
		{
			0,											/* rx: фильтр допустим для этих режимов */
			(1U << MODE_NFM),							/* tx: фильтр допустим для этих режимов */
			(1U << MODE_NFM),	/* фильтр "по умолчанию" для этих режимов */
			(uint16_t) ((IF3CWCENTER455 - IF3FREQBASE455) - (15000/2)),	/* частота нижнего среза полосы пропускания */
			(uint16_t) ((IF3CWCENTER455 - IF3FREQBASE455) + (15000/2)),	/* частота верхнего среза полосы пропускания */
			{ BOARD_FILTER_15P0_NFM, BOARD_FILTER_15P0_NFM, },				/* код выбора данного фильтра на плате приемника */
			0 != (IF3_FHAVE & IF3_FMASK_15P0),	/* этот фильтр установлен */
			1,	/* 0 - считается "узким", 1 - "широким" */
			IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
			& lo2param455,		/* lo2 parameters set index for this filter */
			strFlash15p0,	/* название фильтра для отображения */
		};
		#endif /* (IF3_FMASKTX & IF3_FMASK_15P0) */
	#endif /* WITHDEDICATEDNFM */
#endif


#if WITHTX && WITHSAMEBFO == 0
	#if (IF3_FMASKTX & IF3_FMASK_2P7)
	filter_t fi_2p7_tx =
	{
		0,											/* rx: фильтр допустим для этих режимов */
		(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
		0,
		(uint16_t) (IF3LOWER - IF3FREQBASE),
		(uint16_t) (IF3UPPER - IF3FREQBASE),
		{ BOARD_FILTER_2P7, BOARD_FILTER_2P7, },					/* код выбора данного фильтра на плате приемника */
		0x01,	/* этот фильтр установлен */
		1,	/* 0 - считается "узким", 1 - "широким" */
		IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
		& lo2param,		/* lo2 parameters set index for this filter */
		strFlash2p7,	/* название фильтра для отображения */
	};
	#endif /* (IF3_FMASK & IF3_FMASK_2P7) */
	#if (IF3_FMASKTX & IF3_FMASK_3P1)
	filter_t fi_3p1_tx =
	{
		0,											/* rx: фильтр допустим для этих режимов */
		(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
		0,
		(uint16_t) (IF3LOWER - IF3FREQBASE),
		(uint16_t) (IF3UPPER - IF3FREQBASE),
		{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* код выбора данного фильтра на плате приемника */
		0x01,	/* этот фильтр установлен */
		1,	/* 0 - считается "узким", 1 - "широким" */
		IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
		& lo2param,		/* lo2 parameters set index for this filter */
		strFlash3p1,	/* название фильтра для отображения */
	};
	#endif /* (IF3_FMASK & IF3_FMASK_3P1) */
#endif /* WITHTX && WITHSAMEBFO == 0 */

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_CUSTOM) */


#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_DCRX)

#define LO4FREQ 0
#define IF3FREQBASE (LO4FREQ - IF3OFFS)

#if (LO2_SIDE != LOCODE_INVALID)

	// LO2 формируется PLL или прямо берётся с опорного генератора
	static const lo2param_t lo2param =
	{
		{	LO2_PLL_N, LO2_PLL_N, },	// lo2n
		{	LO2_PLL_R, LO2_PLL_R, },	// lo2r
		IF3FREQBASE,
	};

#else

	static const lo2param_t lo2param =
	{
		{	0, 0, },	// lo2n
		{	1, 1, },	// lo2r
		IF3FREQBASE,
	};

#endif

#if (IF3_FMASK & IF3_FMASK_3P1)
filter_t fi_3p1 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,															/* tx: фильтр допустим для этих режимов */
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	0,					/* частота нижнего среза полосы пропускания */
	0,					/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* код выбора данного фильтра на плате приемника */
	1,	/* этот фильтр установлен */
	1,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash3p1,		/* название фильтра для отображения */
};
#endif

#if (IF3_FMASK & IF3_FMASK_2P1)
filter_t fi_2p1 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,																			/* tx: фильтр допустим для этих режимов */
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	0,					/* частота нижнего среза полосы пропускания */
	0,					/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_2P1, BOARD_FILTER_2P1, },					/* код выбора данного фильтра на плате приемника */
	1,	/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash2p1,		/* название фильтра для отображения */
};
#endif

#if (IF3_FMASK & IF3_FMASK_0P5)
filter_t fi_0p5 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ)  | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,																			/* tx: фильтр допустим для этих режимов */
	(1U << MODE_CW),	/* фильтр "по умолчанию" для этих режимов */
	0,/* частота нижнего среза полосы пропускания */
	0,	/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_0P5, BOARD_FILTER_0P5, },					/* код выбора данного фильтра на плате приемника */
	1,			/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash0p5,		/* название фильтра для отображения */
};
#endif

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_DCRX) */

#if defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_BYPASS)

#define LO4FREQ 0
#define IF3FREQBASE (LO4FREQ - IF3OFFS)

#if (LO2_SIDE != LOCODE_INVALID)

	// LO2 формируется PLL или прямо берётся с опорного генератора
	static const lo2param_t lo2param =
	{
		{	LO2_PLL_N, LO2_PLL_N, },	// lo2n
		{	LO2_PLL_R, LO2_PLL_R, },	// lo2r
		IF3FREQBASE,
	};

#else

	static const lo2param_t lo2param =
	{
		{	0, 0, },	// lo2n
		{	1, 1, },	// lo2r
		IF3FREQBASE,
	};

#endif

#if (IF3_FMASK & IF3_FMASK_3P1)
filter_t fi_3p1 =
{
	(1U << MODE_DRM) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* RX mask */
	(1U << MODE_DRM) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_NFM),		/* TX mask */
	(1U << MODE_SSB) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* фильтр "по умолчанию" для этих режимов */
	IF3OFFS,					/* частота нижнего среза полосы пропускания */
	IF3OFFS,					/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* код выбора данного фильтра на плате приемника */
	1,	/* этот фильтр установлен */
	1,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlashWide,		/* название фильтра для отображения */
};

filter_t fi_3p1_tx =
{
	0,											/* rx: фильтр допустим для этих режимов */
	(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_SSB) | (1U << MODE_AM) | (1U << MODE_RTTY) | (1U << MODE_DIGI),
	0,
	IF3OFFS,					/* частота нижнего среза полосы пропускания */
	IF3OFFS,					/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_3P1, BOARD_FILTER_3P1, },					/* код выбора данного фильтра на плате приемника */
	0x01,	/* этот фильтр установлен */
	1,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,		/* lo2 parameters set index for this filter */
	strFlash3p1,	/* название фильтра для отображения */
};

#endif

#if (IF3_FMASK & IF3_FMASK_0P5)
filter_t fi_0p5 =
{
	(1U << MODE_SSB) | (1U << MODE_CW) | (1U << MODE_CWZ) | (1U << MODE_RTTY) | (1U << MODE_DIGI),	/* RX mask */
	(1U << MODE_CW) | (1U << MODE_TUNE) | (1U << MODE_RTTY) | (1U << MODE_DIGI),						/* TX mask */
	(1U << MODE_CW),	/* фильтр "по умолчанию" для этих режимов */
	IF3OFFS,					/* частота нижнего среза полосы пропускания */
	IF3OFFS,					/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_0P5, BOARD_FILTER_0P5, },					/* код выбора данного фильтра на плате приемника */
	1,			/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlashNarrow,		/* название фильтра для отображения */
};
#endif

#if WITHWFM
static const filter_t fi_wfm =
{
	(1U << MODE_WFM),	/* rx: фильтр допустим для этих режимов */
	0,					/* tx: фильтр допустим для этих режимов */
	(1U << MODE_WFM),	/* фильтр "по умолчанию" для этих режимов */
	IF3OFFS,					/* частота нижнего среза полосы пропускания */
	IF3OFFS,					/* частота верхнего среза полосы пропускания */
	{ BOARD_FILTER_0P5, BOARD_FILTER_0P5, },					/* код выбора данного фильтра на плате приемника */
	1,			/* этот фильтр установлен */
	0,	/* 0 - считается "узким", 1 - "широким" */
	IF3CEOFFS,        /*  смещённый на IF3CEOFFS сдвиг центральной частоты: IF3CEOFFS - 0 герц */
	& lo2param,			/* lo2 parameters set index for this filter */
	strFlash120p0,		/* название фильтра для отображения */
};
#endif /* WITHWFM */

#endif /* defined (IF3_MODEL) && (IF3_MODEL == IF3_TYPE_BYPASS) */

/* список фильтров, имеющихся в приемнике */
#if (IF3_FMASK == 0)
	#error IF3_FMASK error
#endif
static const filter_t * const FLASHMEM gfa [] =
{
#if CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2
		& fi_3p0_455,
		& fi_3p1,
		& fi_6p0_455,
		& fi_10p0_455,
#elif CTLSTYLE_RA4YBO_V3
		& fi_0p5,
		& fi_3p1,
		& fi_3p0_455,
		& fi_2p0_455,
		& fi_6p0_455,
		& fi_10p0_455, // tx
#else /* CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2*/
#if (IF3_FMASK & IF3_FMASK_0P3)
		& fi_0p3,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_0P5)
		& fi_0p5,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_1P0)
		& fi_1p0,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_1P5)
		& fi_1p5,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_1P8)
		& fi_1p8,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_2P1)
		& fi_2p1,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_2P4)
		& fi_2p4,
	#if WITHTX && WITHSAMEBFO == 0
		& fi_2p4_tx,
	#endif /*  */
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_2P7)
		& fi_2p7,
	#if WITHTX && WITHSAMEBFO == 0
		& fi_2p7_tx,
	#endif /*  */
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_3P1)
		& fi_3p1,
	#if WITHTX && WITHSAMEBFO == 0
		& fi_3p1_tx,
	#endif /*  */
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_6P0)
		& fi_6p0,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_7P8)
		& fi_7p8,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_8P0)
		& fi_8p0,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_9P0)
		& fi_9p0,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_17P0)
		& fi_17p0,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_15P0)
		& fi_15p0,
		& fi_15p0_nfm,
	#if (IF3_FMASKTX & IF3_FMASK_15P0)
		& fi_15p0_tx_nfm,
	#endif /* (IF3_FMASKTX & IF3_FMASK_15P0) */
#endif /*  */
#endif /* CTLSTYLE_RA4YBO_V1 || CTLSTYLE_RA4YBO_V2 */
#if WITHWFM
		& fi_wfm,
#endif /* WITHWFM */
};

#if WITHTX
#if (IF3_FMASK & IF3_FMASKTX) == 0
	#error IF3_FMASKTX error
#endif

#endif /* WITHTX */


#define GFA_SIZE (sizeof gfa / sizeof gfa [0])	/* количество фильтров в системе */


#if WITHFIXEDBFO 	/* переворачивание боковой полосы за счёт изменения частоты первого гетеродина */
	uint_fast16_t lo4offset = (uint_fast16_t) (LO4FREQ - IF3FREQBASE);			/* частота (без базы) третьего гетеродина */
#elif WITHDUALFLTR	/* переворачивание боковой полосы за счёт переключения фильтров */
	uint_fast16_t lo4offset = (uint_fast16_t) (LO4FREQ - IF3FREQBASE);			/* частота (без базы) третьего гетеродина */
#elif WITHDUALBFO	/* переворачивание боковой полосы за счёт переключения BFO */
	uint_fast16_t lo4offsets [2] = 
	{
		(uint_fast16_t) (IF3LOWER - IF3FREQBASE),			/* частота (без базы) третьего гетеродина USB */
		(uint_fast16_t) (IF3UPPER - IF3FREQBASE),			/* частота (без базы) третьего гетеродина LSB */
	};
#endif

#if WITHDUALFLTR || WITHDUALBFO || WITHFIXEDBFO

/* нстройка центральной частоты для тех фильтров, у которых нет индивидуальной настройки скатов */

int_fast32_t 
getlo4base(void) /* = IF3FREQBASE */
{
	return IF3FREQBASE;
}

#endif /* WITHDUALFLTR || WITHDUALBFO || WITHFIXEDBFO */

int_fast32_t 
getcefreqshiftbase(void) /* = (int_fast32_t) 0 - IF3CEOFFS */
{
	return (int_fast32_t) 0 - IF3CEOFFS;
}


/* = IF3FREQBASE (для фильтра) */
int_fast32_t getlo4baseflt(
	const filter_t * workfilter 
	)
{
	return workfilter->lo2set->lo4base;	// IF3FREQBASE, например
}


static
int_fast32_t
getfilteredge(
	const filter_t * workfilter,
	const uint_fast8_t mix4lsb	/* интересует высокочастотный срез фильтра */
	)
{
	const int_fast32_t base = getlo4baseflt(workfilter) + workfilter->ceoffset - IF3CEOFFS;
	if (mix4lsb == 0 || workfilter->high == 0)	/* workfilter->narrow здесь "защита от дурака" */
		return base + workfilter->low_or_center;
	else
		return base + workfilter->high;
}

int_fast32_t
getif3filtercenter(
	const filter_t * workfilter
	)
{
//#if (IF3_MODEL == IF3_TYPE_DCRX) || (IF3_MODEL == IF3_TYPE_BYPASS)
//	return 0;
//#else
	const int_fast32_t base = getlo4baseflt(workfilter) + workfilter->ceoffset - IF3CEOFFS;
	if (workfilter->high == 0)
		return base + workfilter->low_or_center;
	else
		return base + (((unsigned long) workfilter->low_or_center + workfilter->high) / 2);	/* center freq of filter */
//#endif
}

/* Возврат "частоты ПЧ №4". Для телеграфа и цифры - середина спектра, для SSB - частота подавленной несущей */
/* в режиме dc не вызывается */
int_fast32_t 
getif3byedge(
	const filter_t * workfilter,
	uint_fast8_t mode,			/* код семейства режима работы */
	uint_fast8_t mix4lsb,		/* формируем гетеродин для указанной боковой полосы */
	uint_fast8_t tx,			/* для режима передачи - врежиме CW - смещения частоты не требуется. */
	uint_fast8_t cwpitch10
	)
{
	const int_fast32_t cwpitchsigned = (int) cwpitch10 * (mix4lsb ? - CWPITCHSCALE : CWPITCHSCALE);	/* тон телеграфа */
	const int_fast32_t centerfreq = getif3filtercenter(workfilter);
	/* для фильтров, у которых narrow=1 или для
		режимов с настройкой "по спектру" - отсчёт от центральной частоты
	*/
	const uint_fast8_t narrow = workfilter->high == 0;
	switch (mode)
	{
	/* Возврат "частоты ПЧ №3". Для телеграфа - середина спектра */
	case MODE_CW: 
		if (narrow || tx)
		{
			/* для режима передачи - врежиме CW - смещения частоты не требуется. */
			return centerfreq;	
		}
		else
		{
			/* обычный ssb фильтр */
			/* узкие фильтры не должны попадать в этот режим работы. */
			return getfilteredge(workfilter, mix4lsb) + cwpitchsigned;	/* возвращаем частоту, которую должен занимать CW сигнал в полосе пропускания */
		}

	/* Возврат "частоты ПЧ №3". для SSB - частота подавленной несущей */
	case MODE_DIGI:
	case MODE_SSB: 
		if (narrow)
		{
			/* защита "от дурака" - в USB/LSB узкие фильтры не должны включаться... но все же... */
			/* в случае прямого преобразовния сюда не попадаем. */
			return centerfreq - cwpitchsigned;
		}
		else
		{
			/* обычный ssb фильтр */
			/* узкие фильтры не должны попадать в этот режим работы. */
			return getfilteredge(workfilter, mix4lsb);
		}
	}
	return centerfreq;
}

uint_fast8_t
getgfasize(void)
{
	return GFA_SIZE;
}

/* функция работает с кэшем параметров режима работы */
/* получение фильтра для выбранного режима работы
 */
const filter_t * 
NOINLINEAT
getrxfilter(
	uint_fast8_t submode,
	uint_fast8_t ix		/* текущий номер фильтра  */
	)
{
	if (GFA_SIZE == 0)
	{
		void wrong_GFA_SIZE(void);
		debug_printf_P(PSTR("getfilter: wrong configuration GFA_SIZE=%d\n"), GFA_SIZE);
		wrong_GFA_SIZE();
		return gfa [0];
	}
	return gfa [ix];
}

/* функция работает с кэшем параметров режима работы */
/* получение фильтра для выбранного режима работы
 */
const filter_t * 
NOINLINEAT
gettxfilter(
	uint_fast8_t submode,
	uint_fast8_t ix		/* текущий номер фильтра  */
	)
{
#if CTLREGSTYLE_SW2014NFM || CTLREGSTYLE_SW2016VHF
	if (submode == SUBMODE_NFM)
	{
		return & fi_15p0_tx_nfm;
	}
#endif
#if WITHTX && ! (WITHSAMEBFO || FQMODEL_DCTRX)
	if (GFA_SIZE == 0)
	{
		void wrong_GFA_SIZE(void);
		debug_printf_P(PSTR("getfilter: wrong configuration GFA_SIZE=%d\n"), GFA_SIZE);
		wrong_GFA_SIZE();
		return gfa [0];
	}
#endif
	return gfa [ix];
}



/* получение номера фильтра "по умолчанию" для выбранного режима */
/* может быть вызвана как для приёма так и для передачи */
uint_fast8_t 
//NOINLINEAT
getdefflt(
	uint_fast8_t mode,
	uint_fast8_t ix		/* текущий номер фильтра - возвращается если не нашли допустимых */
	)
{
	const uint_fast8_t ix0 = ix;	// начальный индекс - пербираем пока не вернемся к нему
	const MODEMASK_T mask = (MODEMASK_T) 1 << mode;
	for (;;)
	{
		const filter_t * const pf = gfa [ix];
		if ((pf->defmodes & mask) != 0 && pf->present != 0)
			break;
		ix = calc_next(ix, 0, GFA_SIZE - 1);
		if (ix == ix0)
			break;
	}
	return ix;
}

/* получение номера фильтра, допустимого при приёме для выбранного режима - с переходом на следующий, пока не найдём подходящий. */
uint_fast8_t 
getsuitablerx(
	uint_fast8_t mode,
	uint_fast8_t ix		/* текущий номер фильтра - возвращается если не нашли допустимых */
	)
{
	const uint_fast8_t ix0 = ix;	// начальный индекс - пербираем пока не вернемся к нему
	const MODEMASK_T mask = (MODEMASK_T) 1 << mode;

	// Ищем среди фильттров c разрешением работы в режиме приема
	for (;;)
	{
		const filter_t * const pf = gfa [ix];
		if ((pf->suitmodesrx & mask) != 0 && pf->present != 0)
			return ix;	// нашли подходящий
		ix = calc_next(ix, 0, GFA_SIZE - 1);
		if (ix == ix0)
			break;
	}
	return ix;
}

/* получение номера фильтра, допустимого при передаче для выбранного режима - с переходом на следующий, пока не найдём подходящий. */
uint_fast8_t 
getsuitabletx(
	uint_fast8_t mode,
	uint_fast8_t ix		/* текущий номер фильтра - возвращается если не нашли допустимых */
	)
{
	const uint_fast8_t ix0 = ix;	// начальный индекс - пербираем пока не вернемся к нему
	const MODEMASK_T mask = (MODEMASK_T) 1 << mode;

	// Ищем среди фильттров без разрешения работы в режиме приема
	for (;;)
	{
		const filter_t * const pf = gfa [ix];
		if ((pf->suitmodestx & mask) != 0 && (pf->suitmodesrx & mask) == 0 && pf->present != 0)
			return ix;	// нашли подходящий
		ix = calc_next(ix, 0, GFA_SIZE - 1);
		if (ix == ix0)
			break;
	}

	// Если не нашли среди фильтров "только для передачи", ищем как обычно
	for (;;)
	{
		const filter_t * const pf = gfa [ix];
		if ((pf->suitmodestx & mask) != 0 && pf->present != 0)
			return ix;	// нашли подходящий
		ix = calc_next(ix, 0, GFA_SIZE - 1);
		if (ix == ix0)
			break;
	}
	return ix;
}

const phase_t * getplo2n(
	const filter_t * workfilter,
	uint_fast8_t tx			/* признак работы в режиме передачи */
	)
{
	return & workfilter->lo2set->lo2n [tx];
}

const phase_t * getplo2r(
	const filter_t * workfilter,
	uint_fast8_t tx			/* признак работы в режиме передачи */
	)
{
	return & workfilter->lo2set->lo2r [tx];
}

#if WITHCAT

/* получение номера фильтра, соответствующего данной полосе */
uint_fast8_t 
findfilter(
	uint_fast8_t mode,
	uint_fast8_t ix,		// текущий быбранный фильтр
	unsigned width)			// полоса фильтра в герцах
{
#if (IF3_FMASK & IF3_FMASK_3P1) == IF3_FMASK
	const filter_t * const pf = & fi_3p1;
#elif (IF3_FMASK & IF3_FMASK_2P7) == IF3_FMASK
	const filter_t * const pf = & fi_2p7;
#elif (IF3_FMASK & IF3_FMASK_3P1)
	const filter_t * const pf = (width > 500) ? & fi_3p1 : & fi_0p5;
#elif (IF3_FMASK & IF3_FMASK_2P7)
	const filter_t * const pf = (width > 500) ? & fi_2p7 : & fi_0p5;
#elif (IF3_FMASK & IF3_FMASK_6P0)
	const filter_t * const pf = & fi_6p0;	// заглушка, чтобы собирались конфигурации без ЭМФ
#elif (IF3_FMASK & IF3_FMASK_8P0)
	const filter_t * const pf = & fi_8p0;	// заглушка, чтобы собирались конфигурации без ЭМФ
#elif (IF3_FMASK & IF3_FMASK_15P0)
	const filter_t * const pf = & fi_15p0;	// заглушка, чтобы собирались конфигурации без ЭМФ
#else
	#error Bad IF3_FMASK defintion
#endif /*  */
	uint_fast8_t i;
	if (pf->present == 0)
		return ix;
	for (i = 0; i < GFA_SIZE; ++ i)
	{
		if (gfa [i] == pf)
			return i;
	}
	return ix;
}

#endif /* WITHCAT */
