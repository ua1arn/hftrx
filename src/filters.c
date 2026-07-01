/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "formats.h"	// for debug prints
#include "board.h"

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
#if (IF3_FMASK & IF3_FMASK_0P5)
		& fi_0p5,
#endif /*  */
#if (IF3_FMASK & IF3_FMASK_3P1)
		& fi_3p1,
	#if WITHTX && WITHSAMEBFO == 0 && (IF3_MODEL != IF3_TYPE_DCRX)
		& fi_3p1_tx,
	#endif /*  */
#endif /*  */
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

uint_fast8_t
getgfasize(void)
{
	return GFA_SIZE;
}

/* функция работает с кэшем параметров режима работы */
/* получение фильтра для выбранного режима работы
 */
const filter_t * 
getrxfilter(
	uint_fast8_t submode,
	uint_fast8_t ix		/* текущий номер фильтра  */
	)
{
	if (GFA_SIZE == 0)
	{
		void wrong_GFA_SIZE(void);
		//PRINTF(PSTR("getfilter: wrong configuration GFA_SIZE=%d\n"), GFA_SIZE);
		wrong_GFA_SIZE();
		return gfa [0];
	}
	return gfa [ix];
}

/* функция работает с кэшем параметров режима работы */
/* получение фильтра для выбранного режима работы
 */
const filter_t * 
gettxfilter(
	uint_fast8_t submode,
	uint_fast8_t ix		/* текущий номер фильтра  */
	)
{
#if WITHTX && ! (WITHSAMEBFO || FQMODEL_DCTRX)
	if (GFA_SIZE == 0)
	{
		void wrong_GFA_SIZE(void);
		//PRINTF(PSTR("getfilter: wrong configuration GFA_SIZE=%d\n"), GFA_SIZE);
		wrong_GFA_SIZE();
		return gfa [0];
	}
#endif
	return gfa [ix];
}



/* получение номера фильтра "по умолчанию" для выбранного режима */
/* может быть вызвана как для приёма так и для передачи */
uint_fast8_t 
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
