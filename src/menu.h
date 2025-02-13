/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Описание меню, используется только в main.c
//

static const FLASHMEM struct menudef menutable [] =
{
#if WITHAUTOTUNER && 1 // Tuner parameters debug
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL2("TUNER   ", "Tuner"), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrptuner),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
#if WITHPOWERTRIM
	#if WITHLOWPOWEREXTTUNE
		(const struct paramdefdef [1]) {
			QLABEL("ATU PWR "), 7, 0, 0,	ISTEP5,		/* мощность при работе автоматического согласующего устройства */
			ITEM_VALUE,
			WITHPOWERTRIMMIN, WITHPOWERTRIMMAX,
			OFFSETOF(struct nvmap, gtunepower),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& gtunepower,
			getzerobase,
		},
	#endif /* WITHLOWPOWEREXTTUNE */
#elif WITHPOWERLPHP
	#if WITHLOWPOWEREXTTUNE
	(const struct paramdefdef [1]) {
		QLABEL("ATU PWR "), 7, 0, RJ_POWER,	ISTEP1,		/* мощность при работе автоматического согласующего устройства */
		ITEM_VALUE,
		0, PWRMODE_COUNT - 1,
		OFFSETOF(struct nvmap, gtunepower),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gtunepower,
		getzerobase,
	},
	#endif /* WITHLOWPOWEREXTTUNE */
#endif /* WITHPOWERTRIM */
	(const struct paramdefdef [1]) {
		QLABEL("TUNER L "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		LMIN, LMAX,
		OFFSETOF(struct nvmap, bandgroups [0].otxants [0].tunerind),
		getselector_bandgroupant, nvramoffs_bandgroupant, valueoffs0,
		& tunerind,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("TUNER C "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		CMIN, CMAX,
		OFFSETOF(struct nvmap, bandgroups [0].otxants [0].tunercap),
		getselector_bandgroupant, nvramoffs_bandgroupant, valueoffs0,
		& tunercap,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("TUNER TY"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, KSCH_COUNT - 1,
		OFFSETOF(struct nvmap, bandgroups [0].otxants [0].tunertype),
		getselector_bandgroupant, nvramoffs_bandgroupant, valueoffs0,
		NULL,
		& tunertype,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("TUNER WT"), 7, 0, 0,	ISTEP5,	// задержка перед измерением после переключения реле
		ITEM_VALUE, 
		10, 250,
		OFFSETOF(struct nvmap, gtunerdelay),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gtunerdelay,
		getzerobase, /* складывается со смещением и отображается */
	},
#if WITHAUTOTUNER_N7DDCALGO
	(const struct paramdefdef [1]) {
		QLABEL("C LINEAR"), 7, 0, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, KSCH_COUNT - 1,
		OFFSETOF(struct nvmap, gn7ddclinearC),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gn7ddclinearC,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("L LINEAR"), 7, 0, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, KSCH_COUNT - 1,
		OFFSETOF(struct nvmap, gn7ddclinearL),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gn7ddclinearL,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHAUTOTUNER_N7DDCALGOT */
#endif /* WITHAUTOTUNER */
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL2("DISPLAY ", "Display"), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrpdisplay),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
#if WITHLCDBACKLIGHT
	(const struct paramdefdef [1]) {
		QLABEL2("LCD LIGH", "TFT Backlight"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		WITHLCDBACKLIGHTMIN, WITHLCDBACKLIGHTMAX, 
		OFFSETOF(struct nvmap, gbglight),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbglight,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHLCDBACKLIGHT */
#if WITHKBDBACKLIGHT
	(const struct paramdefdef [1]) {
		QLABEL2("KBD LIGH", "KBD Backlight"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, gkblight),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gkblight,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHKBDBACKLIGHT */
#if WITHLCDBACKLIGHT || WITHKBDBACKLIGHT
	(const struct paramdefdef [1]) {
		QLABEL2("DIMM TIM", "Dimmer Time"), 7, 0, 0,	ISTEP5,
		ITEM_VALUE,
		0, 240, 
		OFFSETOF(struct nvmap, gdimmtime),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gdimmtime,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHKBDBACKLIGHT */
#if WITHSLEEPTIMER
	(const struct paramdefdef [1]) {
		QLABEL2("SLEEPTIM", "Sleep Time"), 7, 0, 0,	ISTEP5,
		ITEM_VALUE,
		0, 240, 
		OFFSETOF(struct nvmap, gsleeptime),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gsleeptime,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHSLEEPTIMER */
#if LCDMODE_COLORED
	// Для цветных дисплеев можно менять цвет фона
//	(const struct paramdefdef [1]) {
//		QLABEL("BLUE BG "), 8, 3, RJ_ON,	ISTEP1,
//		ITEM_VALUE,
//		0, 1,
//		OFFSETOF(struct nvmap, gbluebgnd),
//		getselector0, nvramoffs0, valueoffs0,
//		NULL,
//		& gbluebgnd,
//		getzerobase, /* складывается со смещением и отображается */
//	},
#endif
	(const struct paramdefdef [1]) {
		QLABEL2("SHOW dBm", "Show dBm"), 8, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, gshowdbm),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gshowdbm,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("FREQ FPS"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		4, 35,							/* частота обновления показаний частоты от 5 до 35 раз в секунду */
		OFFSETOF(struct nvmap, gdisplayfreqsfps),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gdisplayfreqsfps,
		getzerobase, /* складывается со смещением и отображается */
	},
#if WITHBARS
	(const struct paramdefdef [1]) {
		QLABEL("BARS FPS"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		4, 40,							/* частота обновления барграфов от 5 до 40 раз в секунду */
		OFFSETOF(struct nvmap, gdisplaybarsfps),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gdisplaybarsfps,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHBARS */
#if WITHSPECTRUMWF
	& xgviewstyle,	/* стиль отображения спектра и панорамы */
#if WITHVIEW_3DSS
	(const struct paramdefdef [1]) {
		QLABEL2("FREQ MRK", "Freq marker"), 7, 5, RJ_YES, ISTEP1,
		ITEM_VALUE,
		0, 1,				/* Для VIEW_3DSS - индикация полосы пропускания на спектре */
		OFFSETOF(struct nvmap, gview3dss_mark),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gview3dss_mark,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHVIEW_3DSS */
	& xgtopdbspe,	/* нижний предел FFT */
	& xgbottomdbspe,	/* верхний предел FFT */
	& xgwflevelsep,	// чувствительность водопада регулируется отдельной парой параметровs
	& xgtopdbwfl,	/* нижний предел FFT waterflow */
	& xgbottomdbwfl,	/* верхний предел FFT waterflow */
	(const struct paramdefdef [1]) {
		QLABEL("STEP DB "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		3, 40,							/* диапазон отображаемых значений (0-отключаем отображение сетки уровней) */
		OFFSETOF(struct nvmap, glvlgridstep),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& glvlgridstep,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL2("RXBW IND", "RX BW Indicator"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, 100,			/* 0..100 - насыщнность цвета заполнения "шторки" - индикатор полосы пропускания примника на спкктре. */
		OFFSETOF(struct nvmap, grxbwsatu),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& grxbwsatu,
		getzerobase, /* складывается со смещением и отображается */
	},
#if BOARD_FFTZOOM_POW2MAX > 0
	& xgzoomxpow2,	/* уменьшение отображаемого участка спектра */
#endif /* BOARD_FFTZOOM_POW2MAX > 0 */
	(const struct paramdefdef [1]) {
		QLABEL2("SPEC TX ", "TX Spectrum"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1,							/* разрешение или запрет раскраски спектра */
		OFFSETOF(struct nvmap, gtxloopback),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gtxloopback,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("BETA PAN"), 7, 2, 0,	ISTEP1,
		ITEM_VALUE,
		10, 100,							/* beta - парамеры видеофильтра спектра */
		OFFSETOF(struct nvmap, gspecbeta100),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gspecbeta100,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("BETA WFL"), 7, 2, 0,	ISTEP1,
		ITEM_VALUE,
		10, 100,							/* beta - парамеры видеофильтра водопада */
		OFFSETOF(struct nvmap, gwflbeta100),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gwflbeta100,
		getzerobase, /* складывается со смещением и отображается */
	},
#if (WITHSWRMTR || WITHSHOWSWRPWR)
	(const struct paramdefdef [1]) {
		QLABEL2("SMETER  ", "S-meter Type"), 7, 3, RJ_SMETER,	ISTEP1,
		ITEM_VALUE,
		0, 1,							/* выбор внешнего вида прибора - стрелочный или градусник */
		OFFSETOF(struct nvmap, gsmetertype),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gsmetertype,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* (WITHSWRMTR || WITHSHOWSWRPWR) */
#endif /* WITHSPECTRUMWF */
#if WITHDSPEXTDDC
	(const struct paramdefdef [1]) {
		QLABEL2("SHOW OVF", "ADC OVF Show"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1,							/* разрешение или запрет раскраски спектра */
		OFFSETOF(struct nvmap, gshowovf),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gshowovf,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHDSPEXTDDC */
#if defined (RTC1_TYPE)
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("CLOCK   "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrpclock),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
	(const struct paramdefdef [1]) {
		QLABEL("TM YEAR "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE, 
		2015, 2099, 
		MENUNONVRAM, //OFFSETOF(struct nvmap, tunerind),
		getselector0, nvramoffs0, valueoffs0,
		& grtcyear,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("TM MONTH"), 7, 3, RJ_MONTH,	ISTEP1,
		ITEM_VALUE, 
		1, 12, 
		MENUNONVRAM, //OFFSETOF(struct nvmap, tunerind),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& grtcmonth,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("TM DAY  "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE, 
		1, 31, 
		MENUNONVRAM, //OFFSETOF(struct nvmap, tunerind),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& grtcday,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("TM HOUR "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE, 
		0, 23, 
		MENUNONVRAM, //OFFSETOF(struct nvmap, tunerind),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& grtchour,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("TM MIN  "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE, 
		0, 59, 
		MENUNONVRAM, //OFFSETOF(struct nvmap, tunerind),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& grtcminute,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("TM SET  "), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE, 
		0, 1, 
		MENUNONVRAM, //OFFSETOF(struct nvmap, tunerind),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& grtcstrobe,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* defined (RTC1_TYPE) */
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL2("FILTERS ", "Filters"), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrpfilters),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
#if WITHIF4DSP
	(const struct paramdefdef [1]) {
		QLABEL("NR LEVEL"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, NRLEVELMAX, 
		OFFSETOF(struct nvmap, gnoisereductvl),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gnoisereductvl,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHIF4DSP */
#if WITHIF4DSP
	(const struct paramdefdef [1]) {
		QLABEL("CW W WDT"), 7, 2, 0, 	ISTEP10,	// CW bandwidth for WIDE
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		10, 180,			/* 100 Hz..1800, Hz in 100 Hz steps */
		RMT_BWPROPSLEFT_BASE(BWPROPI_CWWIDE),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_cwwide.left10_width10,
		getzerobase, 
	},
	(const struct paramdefdef [1]) {
		QLABEL("CW W SFT"), 7, 0, 0, 	ISTEP1,	// CW filter edges for WIDE
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		WITHFILTSOFTMIN, WITHFILTSOFTMAX,			/* 0..100 */
		RMT_BWPROPSFLTSOFTER_BASE(BWPROPI_CWWIDE),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_cwwide.fltsofter,
		getzerobase,
	},
	(const struct paramdefdef [1]) {
		QLABEL("CW N WDT"), 7, 2, 0, 	ISTEP10,	// CW bandwidth for NARROW
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		10, 180,			/* 100 Hz..1800, Hz in 100 Hz steps */
		RMT_BWPROPSLEFT_BASE(BWPROPI_CWNARROW),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_cwnarrow.left10_width10,
		getzerobase, 
	},
	(const struct paramdefdef [1]) {
		QLABEL("CW N SFT"), 7, 0, 0, 	ISTEP1,	// CW filter edges for NARROW
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		WITHFILTSOFTMIN, WITHFILTSOFTMAX,			/* 0..100 */
		RMT_BWPROPSFLTSOFTER_BASE(BWPROPI_CWNARROW),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_cwnarrow.fltsofter,
		getzerobase,
	},
	(const struct paramdefdef [1]) {
		QLABEL("SSB W HI"), 6, 1, 0,	ISTEP1,		/* Подстройка полосы пропускания - SSB WIDE */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWRIGHTMIN, BWRIGHTMAX, 		// 0.8 kHz-18 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_SSBWIDE),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_ssbwide.right100,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("SSB W LO"), 7, 2, 0,	ISTEP5,		/* Подстройка полосы пропускания - SSB WIDE */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWLEFTMIN, BWLEFTMAX, 		// 50 Hz-700 Hz
		RMT_BWPROPSLEFT_BASE(BWPROPI_SSBWIDE),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_ssbwide.left10_width10,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("SSBW AFR"), 3 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		AFRESPONCEMIN, AFRESPONCEMAX,			/* изменение тембра звука - на Samplerate/2 АЧХ изменяется на столько децибел  */
		RMT_BWPROPSAFRESPONCE_BASE(BWPROPI_SSBWIDE),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_ssbwide.afresponce,
		getafresponcebase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("SSB M HI"), 6, 1, 0,	ISTEP1,		/* Подстройка полосы пропускания - SSB MEDIUM */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWRIGHTMIN, BWRIGHTMAX, 		// 0.8 kHz-18 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_SSBMEDIUM),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_ssbmedium.right100,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("SSB M LO"), 7, 2, 0,	ISTEP5,		/* Подстройка полосы пропускания - SSB MEDIUM */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWLEFTMIN, BWLEFTMAX, 		// 50 Hz-700 Hz
		RMT_BWPROPSLEFT_BASE(BWPROPI_SSBMEDIUM),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_ssbmedium.left10_width10,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("SSBM AFR"), 3 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		AFRESPONCEMIN, AFRESPONCEMAX,			/* изменение тембра звука - на Samplerate/2 АЧХ изменяется на столько децибел  */
		RMT_BWPROPSAFRESPONCE_BASE(BWPROPI_SSBMEDIUM),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_ssbmedium.afresponce,
		getafresponcebase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("SSB N HI"), 6, 1, 0,	ISTEP1,		/* Подстройка полосы пропускания - SSB NARROW */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWRIGHTMIN, BWRIGHTMAX, 		// 0.8 kHz-18 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_SSBNARROW),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_ssbnarrow.right100,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("SSB N LO"), 7, 2, 0,	ISTEP5,		/* Подстройка полосы пропускания - SSB NARROW */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWLEFTMIN, BWLEFTMAX, 		// 50 Hz-700 Hz
		RMT_BWPROPSLEFT_BASE(BWPROPI_SSBNARROW),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_ssbnarrow.left10_width10,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("SSBN AFR"), 3 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		AFRESPONCEMIN, AFRESPONCEMAX,			/* изменение тембра звука - на Samplerate/2 АЧХ изменяется на столько децибел  */
		RMT_BWPROPSAFRESPONCE_BASE(BWPROPI_SSBNARROW),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_ssbnarrow.afresponce,
		getafresponcebase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AM W HI "), 6, 1, 0,	ISTEP2,		/* Подстройка полосы пропускания - AM WIDE */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWRIGHTMIN, BWRIGHTMAX, 		// 0.8 kHz-18 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_AMWIDE),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_amwide.right100,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AM W LO "), 7, 2, 0,	ISTEP5,		/* подстройка полосы пропускания - AM WIDE */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWLEFTMIN, BWLEFTMAX,		// 50 Hz..700 Hz
		RMT_BWPROPSLEFT_BASE(BWPROPI_AMWIDE),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_amwide.left10_width10,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AM W AFR"), 3 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		AFRESPONCEMIN, AFRESPONCEMAX,			/* изменение тембра звука - на Samplerate/2 АЧХ изменяется на столько децибел  */
		RMT_BWPROPSAFRESPONCE_BASE(BWPROPI_AMWIDE),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_amwide.afresponce,
		getafresponcebase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AM N HI "), 6, 1, 0,	ISTEP2,		/* Подстройка полосы пропускания - AM NARROW */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWRIGHTMIN, BWRIGHTMAX, 		// 0.8 kHz-18 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_AMNARROW),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_amnarrow.right100,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AM N LO "), 7, 2, 0,	ISTEP5,		/* подстройка полосы пропускания - AM NARROW */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWLEFTMIN, BWLEFTMAX,		// 50 Hz..700 Hz
		RMT_BWPROPSLEFT_BASE(BWPROPI_AMNARROW),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_amnarrow.left10_width10,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AM N AFR"), 3 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		AFRESPONCEMIN, AFRESPONCEMAX,			/* изменение тембра звука - на Samplerate/2 АЧХ изменяется на столько децибел  */
		RMT_BWPROPSAFRESPONCE_BASE(BWPROPI_AMNARROW),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_amnarrow.afresponce,
		getafresponcebase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("SSBTX HI"), 6, 1, 0,	ISTEP1,		/* Подстройка полосы пропускания - TX SSB */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWRIGHTMIN, BWRIGHTMAX, 		// 0.8 kHz-18 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_SSBTX),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_ssbtx.right100,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("SSBTX LO"), 7, 2, 0,	ISTEP1,		/* подстройка полосы пропускания - TX SSB */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWLEFTMIN, BWLEFTMAX,		// 50 Hz..700 Hz
		RMT_BWPROPSLEFT_BASE(BWPROPI_SSBTX),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_ssbtx.left10_width10,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("SSBTXAFR"), 3 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		AFRESPONCEMIN, AFRESPONCEMAX,			/* изменение тембра звука - на Samplerate/2 АЧХ изменяется на столько децибел  */
		RMT_BWPROPSAFRESPONCE_BASE(BWPROPI_SSBTX),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_ssbtx.afresponce,
		getafresponcebase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("DIGI HI "), 6, 1, 0,	ISTEP1,		/* Подстройка полосы пропускания - TX SSB */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWRIGHTMIN, BWRIGHTMAX, 		// 0.8 kHz-18 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_DIGIWIDE),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_digiwide.right100,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("DIGI LO "), 7, 2, 0,	ISTEP1,		/* подстройка полосы пропускания - TX SSB */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWLEFTMIN, BWLEFTMAX,		// 50 Hz..700 Hz
		RMT_BWPROPSLEFT_BASE(BWPROPI_DIGIWIDE),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bwprop_digiwide.left10_width10,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHIF4DSP */

#if WITHIFSHIFT && ! WITHPOTIFSHIFT
	// Увеличение значения параметра смещает слышимую часть спектра в более высокие частоты
	(const struct paramdefdef [1]) {
		QLABEL("IF SHIFT"), 4 + WSIGNFLAG, 2, 1, 	ISTEP50,
		ITEM_VALUE,
		IFSHIFTTMIN, IFSHIFTMAX,			/* -3 kHz..+3 kHz in 50 Hz steps */
		OFFSETOF(struct nvmap, ifshifoffset),
		getselector0, nvramoffs0, valueoffs0,
		& ifshifoffset.value,
		NULL,
		getifshiftbase, 
	},
#endif /* WITHIFSHIFT && ! WITHPOTIFSHIFT */

#if WITHDUALFLTR	/* Переворот боковых за счёт переключения фильтра верхней или нижней боковой полосы */
	(const struct paramdefdef [1]) {
		QLABEL("BFO FREQ"), 7, 2, 1,	ISTEP10,
		ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		OFFSETOF(struct nvmap, lo4offset),
		getselector0, nvramoffs0, valueoffs0,
		& lo4offset,
		NULL,
		getlo4base, /* складывается со смещением и отображается */
	},
#elif WITHDUALBFO	/* Переворот боковых за счёт переключения частоты опорного генератора */
	(const struct paramdefdef [1]) {
		QLABEL("BFO USB "), 7, 2, 1,	ISTEP10,
		ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		OFFSETOF(struct nvmap, lo4offsets [0]),
		getselector0, nvramoffs0, valueoffs0,
		& lo4offsets [0],
		NULL,
		getlo4base, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("BFO LSB "), 7, 2, 1,	ISTEP10,
		ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		OFFSETOF(struct nvmap, lo4offsets [1]),
		getselector0, nvramoffs0, valueoffs0,
		& lo4offsets [1],
		NULL,
		getlo4base, /* складывается со смещением и отображается */
	},
#elif WITHFIXEDBFO	/* Переворот боковых за счёт 1-го гетродина (особенно, если нет подстраиваемого BFO) */
	(const struct paramdefdef [1]) {
		QLABEL("BFO FREQ"), 7, 2, 1,	ISTEP10,
		ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		OFFSETOF(struct nvmap, lo4offset),
		getselector0, nvramoffs0, valueoffs0,
		& lo4offset,
		NULL,
		getlo4base, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("LAST LSB"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, glo4lsb),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& glo4lsb,
		getzerobase, /* складывается со смещением и отображается */
	},

#elif (defined (IF3_MODEL) && (IF3_MODEL != IF3_TYPE_DCRX) && (IF3_MODEL != IF3_TYPE_BYPASS))
	/* Обычная схема - выбор ПЧ делается перестановкой последнего гетеродина */

#if ! CTLSTYLE_SW2011ALL
#if WITHTX
	(const struct paramdefdef [1]) {
		QLABEL("DC TX CW"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, /* 0 - off, 1 - on */
		OFFSETOF(struct nvmap, dctxmodecw),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& dctxmodecw,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHTX */
#endif /* ! CTLSTYLE_SW2011ALL */

	#if (IF3_FMASK & IF3_FMASK_2P4)
	(const struct paramdefdef [1]) {
		QLABEL("2.4 USB "), 7, 2, 1,	ISTEP10,
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		OFFSETOF(struct nvmap, usbe2p4),
		getselector0, nvramoffs0, valueoffs0,
		& fi_2p4.high,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	(const struct paramdefdef [1]) {
		QLABEL("2.4 LSB "), 7, 2, 1,	ISTEP10,
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		OFFSETOF(struct nvmap, lsbe2p4),
		getselector0, nvramoffs0, valueoffs0,
		& fi_2p4.low_or_center,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	#endif /* (IF3_FMASK & IF3_FMASK_2P4) */
	#if (IF3_FMASK & IF3_FMASK_2P7)
	(const struct paramdefdef [1]) {
		QLABEL("BFO7 USB"), 7, 2, 1,	ISTEP10,
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		OFFSETOF(struct nvmap, usbe2p7),
		getselector0, nvramoffs0, valueoffs0,
		& fi_2p7.high,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	(const struct paramdefdef [1]) {
		QLABEL("BFO7 LSB"), 7, 2, 1,	ISTEP10,
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		OFFSETOF(struct nvmap, lsbe2p7),
		getselector0, nvramoffs0, valueoffs0,
		& fi_2p7.low_or_center,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7) != 0
	(const struct paramdefdef [1]) {
		QLABEL("CAR7 USB"), 7, 2, 1,	ISTEP10,
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		OFFSETOF(struct nvmap, usbe2p7tx),
		getselector0, nvramoffs0, valueoffs0,
		& fi_2p7_tx.high,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	(const struct paramdefdef [1]) {
		QLABEL("CAR7 LSB"), 7, 2, 1,	ISTEP10,
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		OFFSETOF(struct nvmap, lsbe2p7tx),
		getselector0, nvramoffs0, valueoffs0,
		& fi_2p7_tx.low_or_center,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	#endif /* WITHTX && WITHSAMEBFO == 0 */
	#endif /* (IF3_FMASK & IF3_FMASK_2P7) */
	
	#if (IF3_FMASK & IF3_FMASK_3P1)
	(const struct paramdefdef [1]) {
		QLABEL("BFO USB "), 7, 2, 1,	ISTEP10,
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		OFFSETOF(struct nvmap, usbe3p1),
		getselector0, nvramoffs0, valueoffs0,
		& fi_3p1.high,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	(const struct paramdefdef [1]) {
		QLABEL("BFO LSB "), 7, 2, 1,	ISTEP10,
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		OFFSETOF(struct nvmap, lsbe3p1),
		getselector0, nvramoffs0, valueoffs0,
		& fi_3p1.low_or_center,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_3P1) != 0
	(const struct paramdefdef [1]) {
		QLABEL("CAR USB "), 7, 2, 1,	ISTEP10,
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		OFFSETOF(struct nvmap, usbe3p1tx),
		getselector0, nvramoffs0, valueoffs0,
		& fi_3p1_tx.high,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	(const struct paramdefdef [1]) {
		QLABEL("CAR LSB "), 7, 2, 1,	ISTEP10,
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		OFFSETOF(struct nvmap, lsbe3p1tx),
		getselector0, nvramoffs0, valueoffs0,
		& fi_3p1_tx.low_or_center,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	#endif /* WITHTX && WITHSAMEBFO == 0 */
	#endif /* (IF3_FMASK & IF3_FMASK_3P1) */
	
	#if (IF3_FMASK & IF3_FMASK_0P3)
	(const struct paramdefdef [1]) {
		QLABEL("HAVE 0.3"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, hascw0p3),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& fi_0p3.present,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("CNTR 0.3"), 7, 2, 1,	ISTEP10,	/* центральная частота телеграфного фильтра */
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		OFFSETOF(struct nvmap, carr0p3),
		getselector0, nvramoffs0, valueoffs0,
		& fi_0p3.low_or_center,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	#endif /* (IF3_FMASK & IF3_FMASK_0P3) */

	#if (IF3_FMASK & IF3_FMASK_0P5)
	(const struct paramdefdef [1]) {
		QLABEL("HAVE 0.5"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, hascw0p5),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& fi_0p5.present,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("CNTR 0.5"), 7, 2, 1,	ISTEP10,	/* центральная частота телеграфного фильтра */
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		OFFSETOF(struct nvmap, carr0p5),
		getselector0, nvramoffs0, valueoffs0,
		& fi_0p5.low_or_center,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	#endif /* (IF3_FMASK & IF3_FMASK_0P5) */

	#if (IF3_FMASK & IF3_FMASK_1P8)
	(const struct paramdefdef [1]) {
		QLABEL("HAVE 1.8"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, hascw1p8),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& fi_1p8.present,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("1.8 USB "), 7, 2, 1,	ISTEP10,
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		OFFSETOF(struct nvmap, usbe1p8),
		getselector0, nvramoffs0, valueoffs0,
		& fi_1p8.high,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	(const struct paramdefdef [1]) {
		QLABEL("1.8 LSB "), 7, 2, 1,	ISTEP10,
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		OFFSETOF(struct nvmap, lsbe1p8),
		getselector0, nvramoffs0, valueoffs0,
		& fi_1p8.low_or_center,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	#endif /* (IF3_FMASK & IF3_FMASK_1P8) */

	#if (IF3_FMASK & IF3_FMASK_2P4)
	(const struct paramdefdef [1]) {
		QLABEL("HAVE 2.4"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, hascw2p4),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& fi_2p4.present,
		getzerobase, /* складывается со смещением и отображается */
	},
	#endif /* (IF3_FMASK & IF3_FMASK_2P4) */

	#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P4)
	(const struct paramdefdef [1]) {
		QLABEL("HAVE T24"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, hascw2p4_tx),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& fi_2p4_tx.present,
		getzerobase, /* складывается со смещением и отображается */
	},
	#endif /* WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P4) */

	#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7)
	(const struct paramdefdef [1]) {
		QLABEL("HAVE T27"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, hascw2p7_tx),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& fi_2p7_tx.present,
		getzerobase, /* складывается со смещением и отображается */
	},
	#endif /* WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7) */

	#if 0 && WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_3P1)
	(const struct paramdefdef [1]) {
		QLABEL("HAVE T31"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, hascw3p1_tx),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& fi_3p1_tx.present,
		getzerobase, /* складывается со смещением и отображается */
	},
	#endif /* WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7) */

	#if (IF3_FMASK & IF3_FMASK_6P0)
	(const struct paramdefdef [1]) {
		QLABEL("HAVE 6.0"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, hascw6p0),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& fi_6p0.present,
		getzerobase, /* складывается со смещением и отображается */
	},
	/* нстройка центральной частоты для тех фильтров, у которых нет индивидуальной настройки скатов */
	(const struct paramdefdef [1]) {
		QLABEL("6K OFFS "), 4 + WSIGNFLAG, 2, 1, 	ISTEP10,
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		OFFSETOF(struct nvmap, cfreq6k),
		getselector0, nvramoffs0, valueoffs0,
		& fi_6p0.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif /* (IF3_FMASK & IF3_FMASK_6P0) */

	#if (IF3_FMASK & IF3_FMASK_7P8)
	/* нстройка центральной частоты для тех фильтров, у которых нет индивидуальной настройки скатов */
	(const struct paramdefdef [1]) {
		QLABEL("7K8 OFFS"), 4 + WSIGNFLAG, 2, 1, 	ISTEP10,
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		OFFSETOF(struct nvmap, cfreq7p8k),
		getselector0, nvramoffs0, valueoffs0,
		& fi_7p8.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif /* (IF3_FMASK & IF3_FMASK_7P8) */

	#if (IF3_FMASK & IF3_FMASK_8P0)
	/* нстройка центральной частоты для тех фильтров, у которых нет индивидуальной настройки скатов */
	(const struct paramdefdef [1]) {
		QLABEL("8K OFFS "), 4 + WSIGNFLAG, 2, 1, 	ISTEP10,
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		OFFSETOF(struct nvmap, cfreq8k),
		getselector0, nvramoffs0, valueoffs0,
		& fi_8p0.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif
	#if (IF3_FMASK & IF3_FMASK_9P0)
	/* нстройка центральной частоты для тех фильтров, у которых нет индивидуальной настройки скатов */
	(const struct paramdefdef [1]) {
		QLABEL("9K OFFS "), 4 + WSIGNFLAG, 2, 1, 	ISTEP10,
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		OFFSETOF(struct nvmap, cfreq9k),
		getselector0, nvramoffs0, valueoffs0,
		& fi_9p0.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif /* (IF3_FMASK & IF3_FMASK_9P0) */
	#if (IF3_FMASK & IF3_FMASK_15P0)
	#if WITHDEDICATEDNFM /* Для NFM отдельный тракт со своим фильтром */
	/* нстройка центральной частоты для тех фильтров, у которых нет индивидуальной настройки скатов */
	(const struct paramdefdef [1]) {
		QLABEL("NFM OFFS"), 4 + WSIGNFLAG, 2, 1, 	ISTEP10,
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		OFFSETOF(struct nvmap, cfreq15k_nfm),
		getselector0, nvramoffs0, valueoffs0,
		& fi_15p0_tx_nfm.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#else /* WITHDEDICATEDNFM */
	/* нстройка центральной частоты для тех фильтров, у которых нет индивидуальной настройки скатов */
	(const struct paramdefdef [1]) {
		QLABEL("15K OFFS"), 4 + WSIGNFLAG, 2, 1, 	ISTEP10,
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		OFFSETOF(struct nvmap, cfreq15k),
		getselector0, nvramoffs0, valueoffs0,
		& fi_15p0.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif /* WITHDEDICATEDNFM */
	#endif /* (IF3_FMASK & IF3_FMASK_15P0) */
	#if (IF3_FMASK & IF3_FMASK_17P0)
	/* нстройка центральной частоты для тех фильтров, у которых нет индивидуальной настройки скатов */
	(const struct paramdefdef [1]) {
		QLABEL("17K OFFS"), 4 + WSIGNFLAG, 2, 1, 	ISTEP10,
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		OFFSETOF(struct nvmap, cfreq17k),
		getselector0, nvramoffs0, valueoffs0,
		& fi_17p0.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif

#endif /* defined (IF3_MODEL) && (IF3_MODEL != IF3_TYPE_DCRX) && (IF3_MODEL != IF3_TYPE_BYPASS) */

#if WITHNOTCHFREQ
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("NOTCH   "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrpnotch),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
	(const struct paramdefdef [1]) {
		QLABEL("NOTCH   "), 8, 3, RJ_NOTCH,	ISTEP1,		/* управление режимом NOTCH */
		ITEM_VALUE,
		0, NOTCHMODE_COUNT - 1,
		RMT_NOTCHTYPE_BASE,							/* управление режимом NOTCH */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gnotchtype,
		getzerobase, /* складывается со смещением и отображается */
	},
	#if ! WITHPOTNOTCH
	(const struct paramdefdef [1]) {
		QLABEL("NTCH FRQ"), 7, 2, 1,	ISTEP50,		/* управление частотой NOTCH. */
		ITEM_VALUE,
		WITHNOTCHFREQMIN, WITHNOTCHFREQMAX,
		OFFSETOF(struct nvmap, gnotchfreq),	/* центральная частота NOTCH */
		getselector0, nvramoffs0, valueoffs0,
		& gnotchfreq.value,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("NTCH WDT"), 7, 0, 0,	ISTEP50,		/* полоса режекции NOTCH. */
		ITEM_VALUE,
		WITHNOTCHWIDTHMIN, WITHNOTCHWIDTHMAX,
		OFFSETOF(struct nvmap, gnotchwidth),	/* полоса режекции NOTCH */
		getselector0, nvramoffs0, valueoffs0,
		& gnotchwidth.value,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
	#endif /* ! WITHPOTNOTCH */
#elif WITHNOTCHONOFF
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("NOTCH   "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrpnotch),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
	(const struct paramdefdef [1]) {
		QLABEL("NOTCH   "), 8, 3, RJ_ON,	ISTEP1,		/* управление режимом NOTCH */
		ITEM_VALUE,
		0, NOTCHMODE_COUNT - 1,
		RMT_NOTCH_BASE,							/* управление режимом NOTCH */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gnotch,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHNOTCHFREQ */

#if defined (IF3_MODEL) && (IF3_MODEL != IF3_TYPE_DCRX)
#if ! WITHPOTPBT && WITHPBT // && (LO3_SIDE != LOCODE_INVALID)
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("PBTS   "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrppbts),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
	(const struct paramdefdef [1]) {
		QLABEL("PBT     "), 4 + WSIGNFLAG, 2, 1, 	ISTEP50,
		ITEM_VALUE,
		PBTMIN, PBTMAX,			/* -15 kHz..+15 kHz in 5 Hz steps */
		OFFSETOF(struct nvmap, pbtoffset),
		getselector0, nvramoffs0, valueoffs0,
		& gpbtoffset,
		NULL,
		getpbtbase, 
	},
#endif /* ! WITHPOTPBT && WITHPBT && (LO3_SIDE != LOCODE_INVALID) */
#endif /* defined (IF3_MODEL) && (IF3_MODEL != IF3_TYPE_DCRX) */

#if WITHELKEY
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("ELKEY   "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrpelkey),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
	// разрешено не только в случае наличия электронного ключа - требуется при переключении режимов CW/SSB
	& xgcwpitch10,
  #if ! WITHPOTWPM
	& xgelkeywpm,
  #endif /* ! WITHPOTWPM */
  #if WITHVIBROPLEX
	& xgelkeyslope,
  #endif /* WITHVIBROPLEX */
	& xgelkeymode,	/* режим электронного ключа */
	& xgelkeyreverse,	/* режим электронного ключа - поменять местами точки с тире или нет. */
	& xgdashratio,
	& xgspaceratio,
#if WITHTX
	& xgbkinenable,	/* автоматическое управление передатчиком (от телеграфного манипулятора) */
	& xgbkindelay,	/* задержка в десятках ms */
#endif /* WITHTX */
#if WITHIF4DSP
	& xgcwedgetime,	/* Set the rise time of the transmitted CW envelope. */
#endif /* WITHIF4DSP */
#if WITHTX && WITHIF4DSP
	& xgcwssbtx,
#endif /* WITHTX && WITHIF4DSP */
#endif /* WITHELKEY */
#if WITHDSPEXTDDC	/* QLABEL("ВоронёнокQLABEL(" с DSP и FPGA */
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("RF ADC  "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrprfadc),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
#if 0 && ! CTLREGMODE_STORCH_V9A
	(const struct paramdefdef [1]) {
		QLABEL("ADC RAND"), 8, 3, RJ_ON,	ISTEP1,	/* управление интерфейсом в LTC2208 */
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, gadcrand),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gadcrand,
		getzerobase, 
	},
#endif /* ! CTLREGMODE_STORCH_V9A */
	(const struct paramdefdef [1]) {
		QLABEL("ADC DITH"), 8, 3, RJ_ON,	ISTEP1,	/* управление зашумлением в LTC2208 */
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, gdither),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gdither,
		getzerobase, 
	},
//	(const struct paramdefdef [1]) {
//		QLABEL("ADC FIFO"), 8, 3, RJ_ON,	ISTEP1,	/*  */
//		ITEM_VALUE,
//		0, 1,
//		OFFSETOF(struct nvmap, gadcfifo),
//		getselector0, nvramoffs0, valueoffs0,
//		NULL,
//		& gadcfifo,
//		getzerobase,
//	},
//	(const struct paramdefdef [1]) {
//		QLABEL("ADC OFFS"), 5 + WSIGNFLAG, 0, 0,	ISTEP1,	/* смещение для выходного сигнала с АЦП */
//		ITEM_VALUE,
//		ADCOFFSETMID - 200, ADCOFFSETMID + 200,
//		OFFSETOF(struct nvmap, gadcoffset),
//		getselector0, nvramoffs0, valueoffs0,
//		& gadcoffset,
//		NULL,
//		getadcoffsbase,	/* складывается со смещением и отображается */
//	},
#endif /* WITHDSPEXTDDC */
#if WITHTX
#if WITHVOX
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("VOX     "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrpvox),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
	& xgvoxenable,
	& xgvoxdelay,
	& xgvoxlevel,
	& xgavoxlevel,
#endif /* WITHVOX */
#endif /* WITHTX */
#if WITHCAT
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("CAT     "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrpcat),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
	& xcatenable,
#if WITHCAT_MUX
	& xgcatmux,
#endif /* WITHCAT_MUX */
#if WITHUSBCDCACM == 0 || WITHCAT_MUX
	& xcatbaudrate,
#endif /* WITHUSBCDCACM == 0 || WITHCAT_MUX */
#if WITHTX
	& xcatsigptt,
#endif /* WITHTX */
	& xcatsigkey,
#endif /* WITHCAT */

#if WITHSUBTONES && WITHTX
/* group name +++ */
	& xgctssgroup,
/* group name --- */
	& xgctssenable,	//  Continuous Tone-Coded Squelch System or CTCSS control
	& xgsubtonei,	//  Continuous Tone-Coded Squelch System or CTCSS freq
#if WITHIF4DSP
	& xgctsslevel,	/* Select the CTCSS transmit level. */
#endif /* WITHIF4DSP */
#endif /* WITHSUBTONES && WITHTX */
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("AUDIO   "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrpaudio),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
#if WITHIF4DSP
	#if ! WITHPOTAFGAIN
	& xafgain1,	// Громкость в процентах
	#endif /* ! WITHPOTAFGAIN */
	#if ! WITHPOTIFGAIN
	& xrfgain1,	// Усиление ПЧ/ВЧ в процентах
	#endif /* ! WITHPOTIFGAIN */
#endif /* WITHIF4DSP */
#if (SIDETONE_TARGET_BIT != 0) || WITHINTEGRATEDDSP
	(const struct paramdefdef [1]) {
		QLABEL("KEY BEEP"), 6, 2, 0, 	ISTEP5,		/* регулировка тона озвучки клавиш */
		ITEM_VALUE,
		80, 250,			/* 800 Hz..2500, Hz in 50 Hz steps */
		OFFSETOF(struct nvmap, gkeybeep10),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gkeybeep10,
		getzerobase, 
	},
#endif /* (SIDETONE_TARGET_BIT != 0) || WITHINTEGRATEDDSP */
#if WITHMUTEALL && WITHTX
	(const struct paramdefdef [1]) {
		QLABEL("MUTE ALL"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, gmuteall),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gmuteall,
		getzerobase, 
	},
#endif /* WITHMUTEALL && WITHTX */
#if WITHIF4DSP
	(const struct paramdefdef [1]) {
		QLABEL("SQUELCH "), 7, 0, 0,	ISTEP1,		/* squelch level */
		ITEM_VALUE,
		0, SQUELCHMAX, 
		OFFSETOF(struct nvmap, gsquelch),	/* уровень сигнала болше которого открывается шумодав */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gsquelch.value,
		getzerobase, /* складывается со смещением и отображается */
	},
#if ! WITHPOTNFMSQL
	(const struct paramdefdef [1]) {
		QLABEL("SQUELNFM"), 7, 0, 0,	ISTEP1,		/* squelch level */
		ITEM_VALUE,
		0, SQUELCHMAX,
		OFFSETOF(struct nvmap, gsquelchNFM),	/* уровень сигнала болше которого открывается шумодав */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gsquelchNFM,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* ! WITHPOTNFMSQL */
	(const struct paramdefdef [1]) {
		QLABEL("SDTN LVL"), 7, 0, 0,	ISTEP1,		/* Select the CW sidetone or keypad sound output level.. */
		ITEM_VALUE,
		0, 100, 
		OFFSETOF(struct nvmap, gsidetonelevel),	/* Уровень сигнала самоконтроля в процентах - 0%..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gsidetonelevel,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("MONI EN "), 8, 3, RJ_ON,	ISTEP1,		/* Select the monitoring sound output enable */
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, gmoniflag),	/* разрешение самопрослушивания */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gmoniflag,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHIF4DSP */
#if WITHTX && WITHIF4DSP
#if WITHWAVPLAYER || WITHSENDWAV
	(const struct paramdefdef [1]) {
		QLABEL("LOOP MSG"), 7, 0, 0,	ISTEP1,		/* Select the monitoring sound output level.. */
		ITEM_VALUE,
		0, ARRAY_SIZE(loopnames) - 1,
		OFFSETOF(struct nvmap, gloopmsg),	/* Уровень сигнала самопрослушивания в процентах - 0%..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gloopmsg,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("LOOP SEC"), 7, 0, 0,	ISTEP1,		/* Select the monitoring sound output level.. */
		ITEM_VALUE,
		15, 240,
		OFFSETOF(struct nvmap, gloopsec),	/* Уровень сигнала самопрослушивания в процентах - 0%..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gloopsec,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHWAVPLAYER || WITHSENDWAV */
#if WITHMIC1LEVEL
	& xgmik1level,
#endif /* ITHMIC1LEVEL */
	#if WITHAFCODEC1HAVELINEINLEVEL	/* кодек имеет управление усилением с линейного входа */
	& xglineamp,	/* подстройка усиления с линейного входа через меню. */
	#endif /* WITHAFCODEC1HAVELINEINLEVEL */
	(const struct paramdefdef [1]) {
		QLABEL("MIC SSB "), 8, 5, RJ_TXAUDIO,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, BOARD_TXAUDIO_count - 1, 					// при SSB/AM/FM передача с тестовых источников
		RMT_TXAUDIO_BASE(MODE_SSB),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gtxaudio [MODE_SSB],
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("MIC DIG "), 8, 5, RJ_TXAUDIO,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, BOARD_TXAUDIO_count - 1, 					// при SSB/AM/FM передача с тестовых источников
		RMT_TXAUDIO_BASE(MODE_DIGI),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gtxaudio [MODE_DIGI],
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("MIC AM  "), 8, 5, RJ_TXAUDIO,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, BOARD_TXAUDIO_count - 1, 					// при SSB/AM/FM передача с тестовых источников
		RMT_TXAUDIO_BASE(MODE_AM),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gtxaudio [MODE_AM],
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("MIC FM  "), 8, 5, RJ_TXAUDIO,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, BOARD_TXAUDIO_count - 1, 					// при SSB/AM/FM передача с тестовых источников
		RMT_TXAUDIO_BASE(MODE_NFM),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gtxaudio [MODE_NFM],
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("MIC AGC "), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,	
		0, 1, 					/* Включение программной АРУ перед модулятором */
		OFFSETOF(struct nvmap, gmikeagc),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gmikeagc,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("MICAGCGN"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,	
		WITHMIKEAGCMIN, WITHMIKEAGCMAX, 	/* максимальное усиление АРУ микрофона в дБ */
		OFFSETOF(struct nvmap, gmikeagcgain),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gmikeagcgain,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("MIC CLIP"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,	
		0, 90, 					/* Ограничение */
		OFFSETOF(struct nvmap, gmikehclip),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gmikehclip,
		getzerobase, /* складывается со смещением и отображается */
	},
#if WITHCOMPRESSOR
	(const struct paramdefdef [1]) {
		QLABEL("COMP ATK"), 7, 0, 0,	ISTEP5,
		ITEM_VALUE,	
		WITHCOMPATTACKMIN, WITHCOMPATTACKMAX,
		OFFSETOF(struct nvmap, gcompressor_attack),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gcompressor_attack,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("COMP RLS"), 7, 0, 0,	ISTEP5,
		ITEM_VALUE,	
		WITHCOMPRELEASEMIN, WITHCOMPRELEASEMAX,
		OFFSETOF(struct nvmap, gcompressor_release),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gcompressor_release,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("COMP HLD"), 7, 0, 0,	ISTEP5,
		ITEM_VALUE,	
		WITHCOMPHOLDMIN, WITHCOMPHOLDMAX,
		OFFSETOF(struct nvmap, gcompressor_hold),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gcompressor_hold,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("COMP GN "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,	
		WITHCOMPGAINMIN, WITHCOMPGAINMAX,
		OFFSETOF(struct nvmap, gcompressor_gain),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gcompressor_gain,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("COMP TH "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,	
		WITHCOMPTHRESHOLDMIN, WITHCOMPTHRESHOLDMAX,
		OFFSETOF(struct nvmap, gcompressor_threshold),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gcompressor_threshold,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHCOMPRESSOR */
#if WITHREVERB
	(const struct paramdefdef [1]) {
		QLABEL2("REVERB  ", "Reverberator"), 7, 0, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 					/* ревербератор */
		OFFSETOF(struct nvmap, greverb),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& greverb,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL2("RVB TIME", "Reverb Delay"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		WITHREVERBDELAYMIN, WITHREVERBDELAYMAX, 					/* ревербератор - задержка */
		OFFSETOF(struct nvmap, greverbdelay),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& greverbdelay,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL2("RVB LOSS", "Reverb Loss"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		WITHREVERBLOSSMIN, WITHREVERBLOSSMAX, 					/* ревербератор - ослабление на возврате */
		OFFSETOF(struct nvmap, greverbloss),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& greverbloss,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHREVERB */
	(const struct paramdefdef [1]) {
		QLABEL2("MICBOOST", "Mike boost"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,	
		0, 1, 					// предусилитель сигнала с микрофона
		OFFSETOF(struct nvmap, gmikeboost20db),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gmikeboost20db,
		getzerobase, /* складывается со смещением и отображается */
	},
	#if WITHAFCODEC1HAVEPROC	/* кодек имеет управление обработкой микрофонного сигнала (эффекты, эквалайзер, ...) */
	& xgmikeequalizer,	// включение обработки сигнала с микрофона (эффекты, эквалайзер, ...)
	// Эквалайзер 80Hz 230Hz 650Hz 	1.8kHz 5.3kHz
	(const struct paramdefdef [1]) {
		QLABEL("EQUA .08"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, EQUALIZERBASE * 2,
		OFFSETOF(struct nvmap, gmikeequalizerparams [0]),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gmikeequalizerparams [0],
		getequalizerbase, /* складывается с -12 и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("EQUA .23"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, EQUALIZERBASE * 2,
		OFFSETOF(struct nvmap, gmikeequalizerparams [1]),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gmikeequalizerparams [1],
		getequalizerbase, /* складывается с -12 и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("EQUA .65"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, EQUALIZERBASE * 2,
		OFFSETOF(struct nvmap, gmikeequalizerparams [2]),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gmikeequalizerparams [2],
		getequalizerbase, /* складывается с -12 и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("EQUA 1.8"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, EQUALIZERBASE * 2,
		OFFSETOF(struct nvmap, gmikeequalizerparams [3]),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gmikeequalizerparams [3],
		getequalizerbase, /* складывается с -12 и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("EQUA 5.3"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, EQUALIZERBASE * 2,
		OFFSETOF(struct nvmap, gmikeequalizerparams [4]),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gmikeequalizerparams [4],
		getequalizerbase, /* складывается с -12 и отображается */
	},
	#endif /* WITHAFCODEC1HAVEPROC */
#if WITHAFEQUALIZER
	(const struct paramdefdef [1]) {
		QLABEL2("RX EQ   ", "RX Equalizer"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, geqrx),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& geqrx,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL2("RX 0.4k ", "RX EQ 400 Hz"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, AF_EQUALIZER_BASE * 2,
		OFFSETOF(struct nvmap, geqrxparams [0]),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& geqrxparams [0],
		hamradio_get_af_equalizer_base,
	},
	(const struct paramdefdef [1]) {
		QLABEL2("RX 1.5k ", "RX EQ 1500 Hz"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, AF_EQUALIZER_BASE * 2,
		OFFSETOF(struct nvmap, geqrxparams [1]),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& geqrxparams [1],
		hamradio_get_af_equalizer_base,
	},
	(const struct paramdefdef [1]) {
		QLABEL2("RX 2.7k ", "RX EQ 2700 Hz"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, AF_EQUALIZER_BASE * 2,
		OFFSETOF(struct nvmap, geqrxparams [2]),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& geqrxparams [2],
		hamradio_get_af_equalizer_base,
	},
	(const struct paramdefdef [1]) {
		QLABEL2("TX EQ   ", "TX Equalizer"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, geqtx),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& geqtx,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL2("TX 0.4k ", "TX EQ 400 Hz"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, AF_EQUALIZER_BASE * 2,
		OFFSETOF(struct nvmap, geqtxparams [0]),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& geqtxparams [0],
		hamradio_get_af_equalizer_base,
	},
	(const struct paramdefdef [1]) {
		QLABEL2("TX 1.5k ", "TX EQ 1500 Hz"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, AF_EQUALIZER_BASE * 2,
		OFFSETOF(struct nvmap, geqtxparams [1]),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& geqtxparams [1],
		hamradio_get_af_equalizer_base,
	},
	(const struct paramdefdef [1]) {
		QLABEL2("TX 2.7k ", "TX EQ 2700 Hz"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, AF_EQUALIZER_BASE * 2,
		OFFSETOF(struct nvmap, geqtxparams [2]),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& geqtxparams [2],
		hamradio_get_af_equalizer_base,
	},
#endif /* WITHAFEQUALIZER */
#endif /* WITHTX && WITHIF4DSP */
#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_NAU8822L)
//	unsigned ALCNEN = 0;	// ALC noise gate function control bit
//	unsigned ALCNTH = 0;	// ALC noise gate threshold level
//	unsigned ALCEN = 1;	// only left channel ALC enabled
//	unsigned ALCMXGAIN = 7;	// Set maximum gain limit for PGA volume setting changes under ALC control
//	unsigned ALCMNGAIN = 0;	// Set minimum gain value limit for PGA volume setting changes under ALC control
	(const struct paramdefdef [1]) {
		QLABEL("ALC NCEN"), 7, 0, RJ_ON,	ISTEP1,		/* ALC noise gate function control bit. */
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, ALCNEN),	/* ALC noise gate function control bit */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& ALCNEN,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("ALCNTH  "), 7, 0, 0,	ISTEP1,		/* ALC noise gate threshold level. */
		ITEM_VALUE,
		0, 7,
		OFFSETOF(struct nvmap, ALCNTH),	/* ALC noise gate threshold level */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& ALCNTH,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("ALC EN  "), 7, 0, RJ_ON,	ISTEP1,		/* ALC enabled. */
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, ALCEN),	/* ALC enabled */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& ALCEN,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("ALCMXGAN"), 7, 0, 0,	ISTEP1,		/* Set maximum gain limit for PGA volume setting changes under ALC control. */
		ITEM_VALUE,
		0, 7,
		OFFSETOF(struct nvmap, ALCMXGAIN),	/* Set maximum gain limit for PGA volume setting changes under ALC control */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& ALCMXGAIN,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("ALCMNGAN"), 7, 0, 0,	ISTEP1,		/* Set minimum gain value limit for PGA volume setting changes under ALC contro. */
		ITEM_VALUE,
		0, 7,
		OFFSETOF(struct nvmap, ALCMNGAIN),	/* Set minimum gain value limit for PGA volume setting changes under ALC contro */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& ALCMNGAIN,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_NAU8822L) */
#if WITHUSEAUDIOREC
	(const struct paramdefdef [1]) {
		QLABEL("SD RECRD"), 8, 3, RJ_ON,	ISTEP1,		/* автоматически начинаем запись на SD CARD при включении */
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, recmode),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& recmode,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHUSEAUDIOREC */
#if WITHUSBHW && (WITHUSBUACOUT || WITHUSBUACIN || WITHUSEUSBBT)
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("USB     "), 0, 0, 0, 0,
		ITEM_GROUP,
		0, 0,
		OFFSETOF(struct nvmap, ggrpusb),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
#if WITHIF4DSP
#if WITHUSEUSBBT
	(const struct paramdefdef [1]) {
		QLABEL("BT CONN "), 7, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 					/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
		OFFSETOF(struct nvmap, gusbbt),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gusbbt,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHUSEUSBBT */
#if WITHUSBUAC
#if WITHTX
	& xgdatatx,	/* автоматическое изменение источника при появлении звука со стороны компьютера */
#endif /* WITHTX */
	& xguacplayer,	/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
	#if WITHRTS96 || WITHRTS192
	& xgswapiq,	/* Поменять местами I и Q сэмплы в потоке RTS96 */
	& xgusb_ft8cn,	/* совместимость VID/PID для работы с программой FT8CN */
	#endif /* WITHRTS96 || WITHRTS192 */
#if WITHTX
	& xggaindigitx,	/* Увеличение усиления при передаче в цифровых режимах 90..300% */
#endif /* WITHTX */
#endif /* WITHUSBUAC */
#endif /* WITHIF4DSP */
#endif /* WITHUSBHW && (WITHUSBUACOUT || WITHUSBUACIN || WITHUSEUSBBT) */
#if WITHIF4DSP
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("AGC     "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrpagc),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
	(const struct paramdefdef [1]) {
		QLABEL("ADC FS  "), 3 + WSIGNFLAG, 1, 0,	ISTEP1,		/* Калиьровка S-метра - момент перегрузки */
		ITEM_VALUE,
		0, FSADCPOWEROFFSET10 * 2, 		// -50..+50 dBm
		OFFSETOF(struct nvmap, gfsadcpower10 [0]),
		getselector0, nvramoffs0, valueoffs0,
		& gfsadcpower10 [0],	// 16 bit
		NULL,
		getfsasdcbase10, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("ADC FSXV"), 3 + WSIGNFLAG, 1, 0,	ISTEP1,		/* с колнвертором Калиьровка S-метра - момент перегрузки */
		ITEM_VALUE,
		0, FSADCPOWEROFFSET10 * 2, 		// -50..+50 dBm
		OFFSETOF(struct nvmap, gfsadcpower10 [1]),
		getselector0, nvramoffs0, valueoffs0,
		& gfsadcpower10 [1],	// 16 bit
		NULL,
		getfsasdcbase10, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AGC OFF "), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,	
		0, 1, 					// предусилитель сигнала с микрофона
		OFFSETOF(struct nvmap, gagcoff),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gagcoff,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("IFGN MAX"), 7, 0, 0,	ISTEP1,		/* диапазон ручной (или автоматической) регулировки цифрового усиления - максимальное значение */
		ITEM_VALUE,
		40, 120, 		// 40..120 dB
		OFFSETOF(struct nvmap, gdigigainmax),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gdigigainmax,	// 8 bit
		getzerobase, /* складывается со смещением и отображается */
	},
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("AGC SSB "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrpagcssb),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
	(const struct paramdefdef [1]) {
		QLABEL("AGC RATE"), 7, 0, 0,	ISTEP1,		/* подстройка параметра АРУ через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		1, AGC_RATE_FLAT,
		OFFSETOF(struct nvmap, afsets [AGCSETI_SSB].rate),	/* На N децибел изменения входного сигнала происходит 1 дБ выходного */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gagc [AGCSETI_SSB].rate,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AGC HUNG"), 6, 1, 0,	ISTEP1,		/* подстройка параметра АРУ (время удержания медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, 250, 
		OFFSETOF(struct nvmap, afsets [AGCSETI_SSB].thung10),	/* время удержания медленной цепи АРУ */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gagc [AGCSETI_SSB].thung10,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AGC T1  "), 7, 0, 0,	ISTEP10,		/* подстройка параметра АРУ (время срабатывания медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		10, 250, 
		OFFSETOF(struct nvmap, afsets [AGCSETI_SSB].t1),	/* время срабатывания медленной цепи АРУ */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gagc [AGCSETI_SSB].t1,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AGC T2  "), 6, 1, 0,	ISTEP1,		/* подстройка параметра АРУ (время разряда медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		1, 100, 
		OFFSETOF(struct nvmap, afsets [AGCSETI_SSB].release10),	/* время разряда медленной цепи АРУ */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gagc [AGCSETI_SSB].release10,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AGC T4  "), 7, 0, 0,	ISTEP10,		/* подстройка параметра АРУ (время разряда быстрой цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		10, 250, 
		OFFSETOF(struct nvmap, afsets [AGCSETI_SSB].t4),	/* время разряда быстрой цепи АРУ */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gagc [AGCSETI_SSB].t4,
		getzerobase, /* складывается со смещением и отображается */
	},
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("AGC CW  "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrpagccw),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
	(const struct paramdefdef [1]) {
		QLABEL("AGC RATE"), 7, 0, 0,	ISTEP1,		/* подстройка параметра АРУ через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		1, AGC_RATE_FLAT,
		OFFSETOF(struct nvmap, afsets [AGCSETI_CW].rate),	/* На N децибел изменения входного сигнала происходит 1 дБ выходного */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gagc [AGCSETI_CW].rate,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AGC HUNG"), 6, 1, 0,	ISTEP1,		/* подстройка параметра АРУ (время удержания медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, 250, 
		OFFSETOF(struct nvmap, afsets [AGCSETI_CW].thung10),	/* время удержания медленной цепи АРУ */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gagc [AGCSETI_CW].thung10,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AGC T1  "), 7, 0, 0,	ISTEP10,		/* подстройка параметра АРУ (время срабатывания медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		10, 250, 
		OFFSETOF(struct nvmap, afsets [AGCSETI_CW].t1),	/* время срабатывания медленной цепи АРУ */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gagc [AGCSETI_CW].t1,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AGC T2  "), 6, 1, 0,	ISTEP1,		/* подстройка параметра АРУ (время разряда медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		1, 100, 
		OFFSETOF(struct nvmap, afsets [AGCSETI_CW].release10),	/* время разряда медленной цепи АРУ */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gagc [AGCSETI_CW].release10,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AGC T4  "), 7, 0, 0,	ISTEP10,		/* подстройка параметра АРУ (время разряда быстрой цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		10, 250, 
		OFFSETOF(struct nvmap, afsets [AGCSETI_CW].t4),	/* время разряда быстрой цепи АРУ */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gagc [AGCSETI_CW].t4,
		getzerobase, /* складывается со смещением и отображается */
	},
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("AGC DIGI"), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrpagcdigi),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
	(const struct paramdefdef [1]) {
		QLABEL("AGC RATE"), 7, 0, 0,	ISTEP1,		/* подстройка параметра АРУ через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		1, AGC_RATE_FLAT,
		OFFSETOF(struct nvmap, afsets [AGCSETI_DIGI].rate),	/* На N децибел изменения входного сигнала происходит 1 дБ выходного */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gagc [AGCSETI_DIGI].rate,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AGC HUNG"), 6, 1, 0,	ISTEP1,		/* подстройка параметра АРУ (время удержания медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, 250, 
		OFFSETOF(struct nvmap, afsets [AGCSETI_DIGI].thung10),	/* время удержания медленной цепи АРУ */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gagc [AGCSETI_DIGI].thung10,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AGC T1  "), 7, 0, 0,	ISTEP10,		/* подстройка параметра АРУ (время срабатывания медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		10, 250, 
		OFFSETOF(struct nvmap, afsets [AGCSETI_DIGI].t1),	/* время срабатывания медленной цепи АРУ */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gagc [AGCSETI_DIGI].t1,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AGC T2  "), 6, 1, 0,	ISTEP1,		/* подстройка параметра АРУ (время разряда медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		1, 100, 
		OFFSETOF(struct nvmap, afsets [AGCSETI_DIGI].release10),	/* время разряда медленной цепи АРУ */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gagc [AGCSETI_DIGI].release10,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("AGC T4  "), 7, 0, 0,	ISTEP10,		/* подстройка параметра АРУ (время разряда быстрой цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		10, 250, 
		OFFSETOF(struct nvmap, afsets [AGCSETI_DIGI].t4),	/* время разряда быстрой цепи АРУ */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gagc [AGCSETI_DIGI].t4,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHIF4DSP */
#if WITHMODEM
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("MODEM   "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrpmodem),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
	(const struct paramdefdef [1]) {
		QLABEL("DATA MOD"), 5, 0, RJ_MDMMODE, 	ISTEP1,
		ITEM_VALUE,
		0, 1,			/* 0: BPSK, 1: QPSK */
		OFFSETOF(struct nvmap, gmodemmode),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gmodemmode,
		getzerobase, 
	},
	(const struct paramdefdef [1]) {
		QLABEL("DATA SPD"), 7, 2, RJ_MDMSPEED, 	ISTEP1,
		ITEM_VALUE,
		0, ARRAY_SIZE(modembr2int100) - 1,
		OFFSETOF(struct nvmap, gmodemspeed),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gmodemspeed,
		getzerobase, 
	},
#endif /* WITHMODEM */
#if WITHLFM
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("LFM     "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		OFFSETOF(struct nvmap, ggrplfm),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
	(const struct paramdefdef [1]) {
		QLABEL("LFM MODE"), 8, 3, RJ_ON, 	ISTEP1,
		ITEM_VALUE,
		0, 1,			/* LFM mode enable */
		OFFSETOF(struct nvmap, lfmmode),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& lfmmode,
		getzerobase, 
	},
	(const struct paramdefdef [1]) {
		QLABEL("LFM STRT"), 5, 1, 0, 	ISTEP1,
		ITEM_VALUE,
		(TUNE_BOTTOM / 100000) + 1, (TUNE_TOP / 100000) - 1,			/* 1.0 MHz.. 55.0 MHz in 100 kHz steps */
		OFFSETOF(struct nvmap, lfmstart100k),
		getselector0, nvramoffs0, valueoffs0,
		& lfmstart100k,
		NULL,
		getzerobase, 
	},
	(const struct paramdefdef [1]) {
		QLABEL("LFM STOP"), 5, 1, 0, 	ISTEP1,
		ITEM_VALUE,
		(TUNE_BOTTOM / 100000) + 1, (TUNE_TOP / 100000) - 1,			/* 1.0 MHz.. 55.0 MHz in 100 kHz steps */
		OFFSETOF(struct nvmap, lfmstop100k),
		getselector0, nvramoffs0, valueoffs0,
		& lfmstop100k,
		NULL,
		getzerobase, 
	},
	(const struct paramdefdef [1]) {
		QLABEL("LFM SPD "), 5, 0, 0, 	ISTEP1,
		ITEM_VALUE,
		50, 550,			/* 50 kHz/sec..550 kHz/sec, 1 kHz/sec steps */
		OFFSETOF(struct nvmap, lfmspeed1k),
		getselector0, nvramoffs0, valueoffs0,
		& lfmspeed1k,
		NULL,
		getzerobase, 
	},
	// Секунды от начала часа до запуска
	(const struct paramdefdef [1]) {
		QLABEL("LFM OFST"), 5, 0, 0, 	ISTEP1,
		ITEM_VALUE,
		0, 60 * 60 - 1,			/* 0..59:59 */
		OFFSETOF(struct nvmap, lfmtoffset),
		getselector0, nvramoffs0, valueoffs0,
		& lfmtoffset,
		NULL,
		getzerobase, 
	},
	// Интервал в секундах между запусками в пределах часа
	(const struct paramdefdef [1]) {
		QLABEL("LFM PERI"), 5, 0, 0, 	ISTEP1,
		ITEM_VALUE,
		1, 60 * 60 - 1,			/* 00:01..59:59 */
		OFFSETOF(struct nvmap, lfmtinterval),
		getselector0, nvramoffs0, valueoffs0,
		& lfmtinterval,
		NULL,
		getzerobase,
	},
    (const struct paramdefdef [1]) {
        QLABEL("LFM SHFT"), 5 + WSIGNFLAG, 0, 0,     ISTEP1,
        ITEM_VALUE,
        0, 2 * LFMFREQBIAS,            /*  */
        OFFSETOF(struct nvmap, lfmfreqbias),
        getselector0, nvramoffs0, valueoffs0,
        & lfmfreqbias,
        NULL,
        getlfmbias,
    },
#endif /* WITHLFM */

#if WITHTX
/* settings page header */
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("TX ADJ  "), 0, 0, 0, 0,
		ITEM_GROUP,
		0, 0,
		OFFSETOF(struct nvmap, ggrptxadj),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */

/* settings page list */
#if WITHIF4DSP
	(const struct paramdefdef [1]) {
		QLABEL("DACSCALE"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gdacscale),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gdacscale,
		getzerobase, /* складывается со смещением и отображается */
	},

	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("HP MW/LW"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_b [0]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [0].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("HP 160m "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_b [1]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [1].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("HP 80m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_b [2]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [2].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("HP 40m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_b [3]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [3].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("HP 30m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_b [4]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [4].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("HP 20m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_b [5]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [5].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("HP 17m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_b [6]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [6].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("HP 15m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_b [7]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [7].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("HP 12m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_b [8]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [8].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("HP 10m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_b [9]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [9].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("HP 6m   "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_b [10]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [10].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("HP 2m   "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_b [11]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [11].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("HP 0.7m "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_b [12]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [12].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("HP ACC13"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_b [13]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [13].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("HP ACC14"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_b [14]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [14].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("HP ACC15"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_b [15]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [15].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},

	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("LP MW/LW"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_a [0]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [0].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("LP 160m "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_a [1]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [1].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("LP 80m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_a [2]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [2].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("LP 40m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_a [3]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [3].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("LP 30m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_a [4]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [4].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("LP 20m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_a [5]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [5].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("LP 17m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_a [6]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [6].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("LP 15m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_a [7]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [7].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("LP 12m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_a [8]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [8].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("LP 10m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_a [9]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [9].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("LP 6m   "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_a [10]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [10].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("LP 2m   "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_a [11]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [11].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("LP 0.7m "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_a [12]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [12].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("LP ACC13"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_a [13]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [13].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("LP ACC14"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_a [14]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [14].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("LP ACC15"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_a [15]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [15].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
#if WITHPACLASSA
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("CA MW/LW"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_classa [0]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [0].adj_classa,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("CA 160m "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_classa [1]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [1].adj_classa,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("CA 80m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_classa [2]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [2].adj_classa,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("CA 40m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_classa [3]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [3].adj_classa,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("CA 30m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_classa [4]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [4].adj_classa,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("CA 20m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_classa [5]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [5].adj_classa,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("CA 17m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_classa [6]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [6].adj_classa,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("CA 15m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_classa [7]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [7].adj_classa,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("CA 12m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_classa [8]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [8].adj_classa,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("CA 10m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_classa [9]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [9].adj_classa,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("CA 6m   "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_classa [10]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [10].adj_classa,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("CA 2m   "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_classa [11]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [11].adj_classa,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("CA 0.7m "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_classa [12]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [12].adj_classa,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("CA ACC13"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_classa [13]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [13].adj_classa,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("CA ACC14"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_classa [14]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [14].adj_classa,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	(const struct paramdefdef [1]) {
		QLABEL("CA ACC15"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		OFFSETOF(struct nvmap, gbandf2adj_classa [15]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandf2adj [15].adj_classa,
		getzerobase, /* складывается со смещением и отображается */
	},

#endif /* WITHPACLASSA */

	& xgamdepth,		/* Глубина модуляции в АМ - 0..100% */
	& xgnfmdeviation,	/* Девиация при передаче в NFM - в сотнях герц */
	& xggaincwtx,		/* Увеличение усиления при передаче в цифровых режимах 100..300% */

#endif /* WITHIF4DSP */
#if WITHFANTIMER
	(const struct paramdefdef [1]) {
		QLABEL("FAN TIME"), 7, 0, 0,	ISTEP5,
		ITEM_VALUE,
		0, FANPATIMEMAX,
		OFFSETOF(struct nvmap, gfanpatime),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gfanpatime,
		getzerobase, /* складывается со смещением и отображается */
	},
	#if WITHFANPWM
	(const struct paramdefdef [1]) {
		QLABEL("FAN FLOW"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		WITHFANPWMMIN, WITHFANPWMMAX,
		OFFSETOF(struct nvmap, gfanpapwm),
		getselector0, nvramoffs0, valueoffs0,
		& gfanpapwm,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
	#endif /* WITHFANPWM */
#endif /* WITHFANTIMER */

#if WITHPOWERTRIM
  #if ! WITHPOTPOWER
	(const struct paramdefdef [1]) {
		QLABEL("TX POWER"), 7, 0, 0,	ISTEP5,		/* мощность при обычной работе на передачу */
		ITEM_VALUE,
		WITHPOWERTRIMMIN, WITHPOWERTRIMMAX,
		OFFSETOF(struct nvmap, gnormalpower),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gnormalpower.value,
		getzerobase,
	},
	#if WITHPACLASSA
		/* усилитель мощности поддерживает переключение в класс А */
		(const struct paramdefdef [1]) {
			QLABEL2("CLASSA  ", "Class A"), 7, 0, RJ_ON,	ISTEP1,		/* использование режима клвсс А при передаче */
			ITEM_VALUE,
			0, 1,
			OFFSETOF(struct nvmap, gclassamode),
			getselector0, nvramoffs0, valueoffs0,
			NULL,
			& gclassamode,
			getzerobase,
		},
	#endif /* WITHPACLASSA */
  #endif /* ! WITHPOTPOWER */
#elif WITHPOWERLPHP
	#if ! CTLSTYLE_SW2011ALL
	(const struct paramdefdef [1]) {
		QLABEL("TX POWER"), 7, 0, RJ_POWER,	ISTEP1,		/* мощность при обычной работе на передачу */
		ITEM_VALUE,
		0, PWRMODE_COUNT - 1,
		OFFSETOF(struct nvmap, gpwri),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gpwri,
		getzerobase,
	},
	#endif /* ! CTLSTYLE_SW2011ALL */
#endif /* WITHPOWERTRIM */

#if ! CTLSTYLE_SW2011ALL
	(const struct paramdefdef [1]) {
		QLABEL("TX GATE "), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, gtxgate),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gtxgate,
		getzerobase, 
	},
#endif /* ! CTLSTYLE_SW2011ALL */

#if WITHPABIASTRIM
	(const struct paramdefdef [1]) {
		QLABEL("PA BIAS "), 7, 0, 0,	ISTEP1,		/* регулировка тока покоя оконечного каскада передатчика */
		ITEM_VALUE,
		WITHPABIASMIN, WITHPABIASMAX,
		OFFSETOF(struct nvmap, gpabias),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gpabias,
		getzerobase, 
	},
#endif /* WITHPABIASTRIM && WITHTX */
#if WITHDSPEXTDDC	/* QLABEL("ВоронёнокQLABEL(" с DSP и FPGA */
	(const struct paramdefdef [1]) {
		QLABEL("DAC TEST"), 8, 3, RJ_ON,	ISTEP1,	/*  */
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, gdactest),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gdactest,
		getzerobase,
	},
#endif /* WITHDSPEXTDDC */

#endif /* WITHTX */

/* settings page header */
/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL("SPECIAL "), 0, 0, 0, 0,
		ITEM_GROUP,
		0, 0,
		OFFSETOF(struct nvmap, ggrpsecial),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */

/* settings page list */
#if WITHRFSG
	(const struct paramdefdef [1]) {
		QLABEL("RFSG MOD"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, userfsg),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& userfsg,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHRFSG */
#if WITHENCODER
	(const struct paramdefdef [1]) {
		QLABEL("ENC1 RES"), 7, 0, RJ_ENCRES,	ISTEP1,
		ITEM_VALUE,
		0, ARRAY_SIZE(encresols) - 1,
		OFFSETOF(struct nvmap, genc1pulses),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& genc1pulses,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("ENC1 DYN"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, genc1dynamic),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& genc1dynamic,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("ENC1 DIV"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		1, 128, 	/* /1 ... /128 */
		OFFSETOF(struct nvmap, genc1div),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& genc1div,
		getzerobase,
	},
	(const struct paramdefdef [1]) {
		QLABEL("BIG STEP"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, gbigstep),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbigstep,
		getzerobase,
	},
#if WITHENCODER2
	(const struct paramdefdef [1]) {
		QLABEL("ENC2 DIV"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		1, 8, 	/* /1 ... /8 */
		OFFSETOF(struct nvmap, genc2div),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& genc2div,
		getzerobase,
	},
#endif /* WITHENCODER2 */
#endif /* WITHENCODER */

#if WITHTX
#if WITHIF4DSP
	(const struct paramdefdef [1]) {
		QLABEL("NFM GAIN"), 7, 1, 0,	ISTEP1,		/* дополнительное усиление по НЧ в режиме приёма NFM 100..1000% */
		ITEM_VALUE,
		10, 100,
		OFFSETOF(struct nvmap, ggainnfmrx10),	/* дополнительное усиление по НЧ в режиме приёма NFM 100..1000% */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& ggainnfmrx10,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHIF4DSP */
#if WITHIF4DSP
	(const struct paramdefdef [1]) {
		QLABEL2("RGR BERP", "Roger Beep"), 7, 0, RJ_ON,	ISTEP5,	/* разрешение (не-0) или запрещение (0) формирования roger beep */
		ITEM_VALUE,
		0, 1,						/* разрешение (не-0) или запрещение (0) формирования roger beep */
		OFFSETOF(struct nvmap, grgbeep),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& grgbeep,
		getzerobase,
	},
#endif /* WITHIF4DSP */
//	(const struct paramdefdef [1]) {
//		QLABEL("REPT HF "), 5 + WSIGNFLAG, 0, 0, 	ISTEP1,
//		ITEM_VALUE,
//		RPTOFFSMIN, RPTOFFSMAX,		/* repeater offset */
//		OFFSETOF(struct nvmap, rptroffshf1k),
//		getselector0, nvramoffs0, valueoffs0,
//		& rptroffshf1k,
//		NULL,
//		getrptoffsbase,
//	},
//	(const struct paramdefdef [1]) {
//		QLABEL("REPT UHF"), 5 + WSIGNFLAG, 0, 0, 	ISTEP1,
//		ITEM_VALUE,
//		RPTOFFSMIN, RPTOFFSMAX,		/* repeater offset */
//		OFFSETOF(struct nvmap, rptroffsuhf1k),
//		getselector0, nvramoffs0, valueoffs0,
//		& rptroffsuhf1k,
//		NULL,
//		getrptoffsbase,
//	},
#endif /* WITHTX */

#if defined(REFERENCE_FREQ)
#if defined (DAC1_TYPE)
	(const struct paramdefdef [1]) {
		QLABEL("REF ADJ "), 7, 0, 0,	ISTEP1,		/* подстройка частоты опорного генератора (напряжением) через меню. */
		ITEM_VALUE,
		WITHDAC1VALMIN, WITHDAC1VALMAX, 
		OFFSETOF(struct nvmap, dac1level),
		getselector0, nvramoffs0, valueoffs0,
		NULL,	/* подстройка опорника */
		& dac1level,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* defined (DAC1_TYPE) */
	(const struct paramdefdef [1]) {
		QLABEL("REF FREQ"), 9, 3, 0,	ISTEP1,		/* ввод реальной частоты опорного генератора через меню. */
		ITEM_VALUE,
		0, OSCSHIFT * 2 - 1, 
		OFFSETOF(struct nvmap, refbias),
		getselector0, nvramoffs0, valueoffs0,
		& refbias,	/* подстройка частоты опорника */
		NULL,
		getrefbase, 	/* складывается со смещением и отображается */
	},
#endif	/* defined(REFERENCE_FREQ) */
#if (LO3_SIDE != LOCODE_INVALID) && LO3_FREQADJ	/* подстройка частоты гетеродина через меню. */
	(const struct paramdefdef [1]) {
		QLABEL("LO3 FRQ "), 9, 3, 0,	ISTEP1,
		ITEM_VALUE,
		LO2AMIN, LO2AMAX, 
		OFFSETOF(struct nvmap, lo3offset),
		getselector0, nvramoffs0, valueoffs0,
		& lo3offset,	/* подстройка частоты гетеродина */
		NULL,
		getlo3base, 	/* складывается со смещением и отображается */
	},
#endif	/* (LO3_SIDE != LOCODE_INVALID) && LO3_FREQADJ */
#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570)
	(const struct paramdefdef [1]) {
		QLABEL("REFSI570"), 9, 3, 0,	ISTEP1,
		ITEM_VALUE,
		0, OSCSHIFT * 2 - 1, 
		OFFSETOF(struct nvmap, si570_xtall_offset),
		getselector0, nvramoffs0, valueoffs0,
		& si570_xtall_offset,	/* подстройка опорника */
		NULL,
		si570_get_xtall_base, 	/* складывается со смещением и отображается */
	},
#endif /* defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570) */
#if WITHONLYBANDS
	(const struct paramdefdef [1]) {
		QLABEL("BANDONLY"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, withonlybands),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& withonlybands,
		getzerobase, 
	},
#endif /* WITHONLYBANDS */
	(const struct paramdefdef [1]) {
		QLABEL("STAYFREQ"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, stayfreq),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& stayfreq,
		getzerobase, 
	},
#if WITHVOLTLEVEL && ! WITHREFSENSOR
	(const struct paramdefdef [1]) {
		QLABEL("BAT CALI"), 7, 1, 0,	ISTEP1,			/* калибровочный параметр делителя напряжения АКБ */
		ITEM_VALUE,
		ADCVREF_CPU, 255,	// 3.3/5.0 .. 25.5 вольта
		OFFSETOF(struct nvmap, voltcalibr100mV),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& voltcalibr100mV,
		getzerobase,
	},
#endif /* WITHVOLTLEVEL && ! WITHREFSENSOR */
#if (WITHCURRLEVEL || WITHCURRLEVEL2)
	(const struct paramdefdef [1]) {
		QLABEL("IPA CALI"), 5 + WSIGNFLAG, 2, 0,	ISTEP1,			/* калибровочный параметр делителя напряжения АКБ */
		ITEM_VALUE,
		0, IPACALI_RANGE,
		OFFSETOF(struct nvmap, gipacali),
		getselector0, nvramoffs0, valueoffs0,
		& gipacali,
		NULL,
		getipacalibase,
	},
#endif /* (WITHCURRLEVEL || WITHCURRLEVEL2) */
#if WITHTX
#if WITHSWRMTR && ! WITHSHOWSWRPWR
	(const struct paramdefdef [1]) {
		QLABEL("SWR SHOW"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, swrmode),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& swrmode,
		getzerobase, 
	},
#endif /* WITHSWRMTR && ! WITHSHOWSWRPWR */
	(const struct paramdefdef [1]) {
		QLABEL("SWR CALI"), 7, 2, 0,	ISTEP1,		/* калибровка SWR-метра */
		ITEM_VALUE,
		50, 200, //80, 120, 
		OFFSETOF(struct nvmap, swrcalibr),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& swrcalibr,
		getzerobase, 
	},
#if (WITHSWRMTR || WITHSHOWSWRPWR)
	(const struct paramdefdef [1]) {
		QLABEL("FWD LOWR"), 7, 0, 0,	ISTEP1,		/* нечувствительность SWR-метра */
		ITEM_VALUE,
		1, (1U << HARDWARE_ADCBITS) - 1, 
		OFFSETOF(struct nvmap, minforward),
		getselector0, nvramoffs0, valueoffs0,
		& minforward,
		NULL,
		getzerobase, 
	},
	(const struct paramdefdef [1]) {
		QLABEL("PWR CALI"), 7, 0, 0,	ISTEP1,		/* калибровка PWR-метра */
		ITEM_VALUE,
		1, 255,
		OFFSETOF(struct nvmap, maxpwrcali),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& maxpwrcali,
		getzerobase,
	},
	(const struct paramdefdef [1]) {
		QLABEL("SWR PROT"), 7, 0, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,						/* защита от превышения КСВ */
		OFFSETOF(struct nvmap, gswrprot),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gswrprot,
		getzerobase,
	},
	
#elif WITHPWRMTR
	(const struct paramdefdef [1]) {
		QLABEL("PWR CALI"), 7, 0, 0,	ISTEP1,		/* калибровка PWR-метра */
		ITEM_VALUE,
		10, 255,
		OFFSETOF(struct nvmap, maxpwrcali),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& maxpwrcali,
		getzerobase,
	},
#endif
#if (WITHTHERMOLEVEL || WITHTHERMOLEVEL2)
	(const struct paramdefdef [1]) {
		QLABEL("HEAT LIM"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		20, 85,						/* порог срабатывания защиты по температуре */
		OFFSETOF(struct nvmap, gtempvmax),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gtempvmax,
		getzerobase,
	},
	(const struct paramdefdef [1]) {
		QLABEL("HEATPROT"), 7, 0, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,						/* защита от перегрева */
		OFFSETOF(struct nvmap, gheatprot),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gheatprot,
		getzerobase,
	},
#endif /* (WITHTHERMOLEVEL || WITHTHERMOLEVEL2) */
#if WITHIF4DSP || defined (TXPATH_BIT_ENABLE_SSB) || defined (TXPATH_BIT_ENABLE_CW) || defined (TXPATH_BIT_GATE)
	(const struct paramdefdef [1]) {
		QLABEL("RXTX DLY"), 7, 0, 0,	ISTEP5,	/* 5 mS step of changing value */
		ITEM_VALUE,
		5, WITHMAXRXTXDELAY,						/* 5..100 ms delay */
		OFFSETOF(struct nvmap, rxtxdelay),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& rxtxdelay,
		getzerobase, 
	},
	(const struct paramdefdef [1]) {
		QLABEL("TXRX DLY"), 7, 0, 0,	ISTEP5,	/* 5 mS step of changing value */
		ITEM_VALUE,
		5, WITHMAXTXRXDELAY,						/* 5..100 ms delay */
		OFFSETOF(struct nvmap, txrxdelay),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& txrxdelay,
		getzerobase, 
	},
#endif /* GATEs */
#endif /* WITHTX */
#if WITHLO1LEVELADJ
	(const struct paramdefdef [1]) {
		QLABEL("LO1 LEVL"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, 100, 		/* уровень (амплитуда) LO1 в процентах */
		OFFSETOF(struct nvmap, lo1level),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& lo1level,
		getzerobase, 
	},
#endif /* WITHLO1LEVELADJ */
#if LO1PHASES
	(const struct paramdefdef [1]) {
		QLABEL("PHASE RX"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, 65535, /* добавление к коду смещения фазы */
		OFFSETOF(struct nvmap, phaserx),
		getselector0, nvramoffs0, valueoffs0,
		& phasesmap [0],
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
#if WITHTX
	(const struct paramdefdef [1]) {
		QLABEL("PHASE TX"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, 65535, /* добавление к коду смещения фазы */
		OFFSETOF(struct nvmap, phasetx),
		getselector0, nvramoffs0, valueoffs0,
		& phasesmap [1],
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHTX */
#endif /* LO1PHASES */

#if LO1MODE_HYBRID
	(const struct paramdefdef [1]) {
		QLABEL("ALIGN MD"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, alignmode),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& alignmode,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif	/* LO1MODE_HYBRID */

#if 1//WITHDEBUG
	(const struct paramdefdef [1]) {
		QLABEL("FORCEXVR"), 7, 3, RJ_YES,	ISTEP1,	/* принудительно включить коммутацию трансвертора */
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, gforcexvrtr),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gforcexvrtr,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif	/* WITHDEBUG */

#if LO1FDIV_ADJ
	(const struct paramdefdef [1]) {
		QLABEL("LO1DV RX"), 7, 0, RJ_POW2,	ISTEP1,
		ITEM_VALUE,
		0, 3, 	/* Output of synthesizer multiplied to 1, 2, 4 or 8 */
		OFFSETOF(struct nvmap, lo1powrx),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& lo1powmap [0],
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("LO1DV TX"), 7, 0, RJ_POW2,	ISTEP1,
		ITEM_VALUE,
		0, 3, 	/* Output of synthesizer multiplied to 1, 2, 4 or 8 */
		OFFSETOF(struct nvmap, lo1powtx),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& lo1powmap [1],
		getzerobase, /* складывается со смещением и отображается */
	},
#endif
#if LO4FDIV_ADJ
	(const struct paramdefdef [1]) {
		QLABEL("LO4DV RX"), 7, 0, RJ_POW2,	ISTEP1,
		ITEM_VALUE,
		0, 3, 	/* Output of synthesizer multiplied to 1, 2, 4 or 8 */
		OFFSETOF(struct nvmap, lo4powrx),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& lo4powmap [0],
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("LO4DV TX"), 7, 0, RJ_POW2,	ISTEP1,
		ITEM_VALUE,
		0, 3, 	/* Output of synthesizer multiplied to 1, 2, 4 or 8 */
		OFFSETOF(struct nvmap, lo4powtx),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& lo4powmap [1],
		getzerobase, /* складывается со смещением и отображается */
	},
#endif

#if WITHBARS
	(const struct paramdefdef [1]) {
		QLABEL("S9 LEVEL"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		1, UINT8_MAX - 1, 
		OFFSETOF(struct nvmap, s9level),
		getselector0, nvramoffs0, valueoffs0,
		NULL,			/* калибровка уровней S-метра */
		& s9level,
		getzerobase, 
	},
	(const struct paramdefdef [1]) {
		QLABEL("S9 DELTA"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		1, UINT8_MAX - 1, 
		OFFSETOF(struct nvmap, s9delta),
		getselector0, nvramoffs0, valueoffs0,
		NULL,			/* калибровка уровней S-метра */
		& s9delta,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("+60DELTA"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		1, UINT8_MAX - 1, 
		OFFSETOF(struct nvmap, s9_60_delta),
		getselector0, nvramoffs0, valueoffs0,
		NULL,			/* калибровка уровней S-метра */
		& s9_60_delta,
		getzerobase, /* складывается со смещением и отображается */
	},
	(const struct paramdefdef [1]) {
		QLABEL("ATTPRESH"), 7, 0, RJ_ON,	ISTEP1,	/* attenuator-preamplifier shift */
		ITEM_VALUE,
		0, 1,
		OFFSETOF(struct nvmap, gattpresh),	/* корректировка показаний с-метра по включенному аттенюатору и предусилителю */
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gattpresh,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHBARS */

#if WITHANTSELECT2
	(const struct paramdefdef [1]) {
		QLABEL2("HF ANT F", "HF Ant freq"), 3, 0, 0, ISTEP1,
		ITEM_VALUE,
		TUNE_BOTTOM / 1000000, (TUNE_TOP - 1) / 1000000,
		OFFSETOF(struct nvmap, hffreqswitch),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& hffreqswitch,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHANTSELECT2 */

	& xgbandset11m,
#if WITHBCBANDS
	(const struct paramdefdef [1]) {
		QLABEL("BAND BC "), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, gbandsetbcast),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& gbandsetbcast,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHBCBANDS */

#if CTLSTYLE_SW2011ALL
#if TUNE_6MBAND
	(const struct paramdefdef [1]) {
		QLABEL("BAND 50 "), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, bandset6m),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bandset6m,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* TUNE_6MBAND */
#if TUNE_4MBAND
	(const struct paramdefdef [1]) {
		QLABEL("BAND 70 "), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, bandset4m),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bandset4m,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* TUNE_6MBAND */
#if TUNE_2MBAND
	(const struct paramdefdef [1]) {
		QLABEL("BAND 144"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		OFFSETOF(struct nvmap, bandset2m),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		& bandset2m,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* TUNE_2MBAND */
#endif /* CTLSTYLE_SW2011ALL */

/* group name +++ */
	(const struct paramdefdef [1]) {
		QLABEL2("ABOUT   ", "About"), 0, 0, 0, 0,
		ITEM_GROUP,
		0, 0,
		OFFSETOF(struct nvmap, ggrpabout),
		getselector0, nvramoffs0, valueoffs0,
		NULL,
		NULL,
		NULL,
	},
/* group name --- */
	(const struct paramdefdef [1]) {
		QLABEL("VERSION "), 7, 0, RJ_COMPILED, 	ISTEP_RO,	// тип процессора
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, 0,
		MENUNONVRAM,
		getselector0, nvramoffs0, valueoffs0,
		& gzero,
		NULL,
		getzerobase,
	},
	(const struct paramdefdef [1]) {
		QLABEL("S/N     "), 7, 0, RJ_SERIALNR, 	ISTEP_RO,	// Индивидуальный номер изделия
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, 0,
		MENUNONVRAM,
		getselector0, nvramoffs0, valueoffs0,
		& gzero,
		NULL,
		getzerobase,
	},
	& xgcputype,
	& xgcpufreq,
#if CPUSTYLE_STM32MP1
	(const struct paramdefdef [1]) {
		QLABEL("DDR FREQ"), 7, 0, 0, 	ISTEP_RO,	// частота памяти
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, 0,
		MENUNONVRAM,
		getselector0, nvramoffs0, valueoffs0,
		& gzero,
		NULL,
		getddrfreqbase,
	},
	(const struct paramdefdef [1]) {
		QLABEL("BUS FREQ"), 7, 0, 0, 	ISTEP_RO,	// частота шины
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, 0,
		MENUNONVRAM,
		getselector0, nvramoffs0, valueoffs0,
		& gzero,
		NULL,
		getaxissfreqbase,
	},
#endif
};
