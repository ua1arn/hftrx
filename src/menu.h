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
#if ! WITHFLATMENU
	{
		QLABEL2("TUNER   ", "Tuner"), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrptuner),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
#if WITHPOWERTRIM
	#if WITHLOWPOWEREXTTUNE
		{
			QLABEL("ATU PWR "), 7, 0, 0,	ISTEP5,		/* мощность при работе автоматического согласующего устройства */
			ITEM_VALUE,
			WITHPOWERTRIMMIN, WITHPOWERTRIMMAX,
			offsetof(struct nvmap, gtunepower),
			nvramoffs0,
			NULL,
			& gtunepower,
			getzerobase,
		},
	#endif /* WITHLOWPOWEREXTTUNE */
#elif WITHPOWERLPHP
	#if WITHLOWPOWEREXTTUNE
	{
		QLABEL("ATU PWR "), 7, 0, RJ_POWER,	ISTEP1,		/* мощность при работе автоматического согласующего устройства */
		ITEM_VALUE,
		0, PWRMODE_COUNT - 1,
		offsetof(struct nvmap, gtunepower),
		nvramoffs0,
		NULL,
		& gtunepower,
		getzerobase,
	},
	#endif /* WITHLOWPOWEREXTTUNE */
#endif /* WITHPOWERTRIM */
	{
		QLABEL("TUNER L "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		LMIN, LMAX,
		offsetof(struct nvmap, bandgroups [0].oants [0].tunerind),
		nvramoffs_bandgroupant,
		& tunerind,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("TUNER C "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		CMIN, CMAX,
		offsetof(struct nvmap, bandgroups [0].oants [0].tunercap),
		nvramoffs_bandgroupant,
		& tunercap,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("TUNER TY"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, KSCH_COUNT - 1,
		offsetof(struct nvmap, bandgroups [0].oants [0].tunertype),
		nvramoffs_bandgroupant,
		NULL,
		& tunertype,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("TUNER WT"), 7, 0, 0,	ISTEP5,	// задержка перед измерением после переключения реле
		ITEM_VALUE, 
		10, 250,
		offsetof(struct nvmap, tunerdelay),
		nvramoffs0,
		NULL,
		& tunerdelay,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHAUTOTUNER */
#if ! WITHFLATMENU
	{
		QLABEL2("DISPLAY ", "Display"), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpdisplay),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
#if defined (DEFAULT_LCD_CONTRAST)
	{
		QLABEL("LCD CONT"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
#if LCDMODE_PCF8535
		42, 70, 				// для TIC218 @ 3.3 volt - 45..60
#elif LCDMODE_RDX0120
		70, 120, 				// для amator - 110..220
#elif LCDMODE_UC1601
		75, 220, 				// для amator - 110..220
#elif LCDMODE_LS020 || LCDMODE_ILI9320
		0, 255, 				// LS020 - 0..255
#elif LCDMODE_UC1608
		0, 63, 					// UC1608 - 0..63
#endif /* LCDMODE_UC1601 */
		offsetof(struct nvmap, gcontrast),
		nvramoffs0,
		NULL,
		& gcontrast,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* defined (DEFAULT_LCD_CONTRAST) */
#if 0//WITHDCDCFREQCTL
	{
		QLABEL("DCDC DIV"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		4, UINT16_MAX, 
		offsetof(struct nvmap, dcdcrefdiv),
		nvramoffs0,
		& dcdcrefdiv,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHDCDCFREQCTL */
#if WITHLCDBACKLIGHT
	{
		QLABEL2("LCD LIGH", "TFT Backlight"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		WITHLCDBACKLIGHTMIN, WITHLCDBACKLIGHTMAX, 
		offsetof(struct nvmap, gbglight),
		nvramoffs0,
		NULL,
		& gbglight,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHLCDBACKLIGHT */
#if WITHKBDBACKLIGHT
	{
		QLABEL("KBD LIGH"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, gkblight),
		nvramoffs0,
		NULL,
		& gkblight,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHKBDBACKLIGHT */
#if WITHLCDBACKLIGHT || WITHKBDBACKLIGHT
	{
		QLABEL2("DIMM TIM", "Dimmer Time"), 7, 0, 0,	ISTEP5,
		ITEM_VALUE,
		0, 240, 
		offsetof(struct nvmap, gdimmtime),
		nvramoffs0,
		NULL,
		& gdimmtime,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHKBDBACKLIGHT */
#if WITHSLEEPTIMER
	{
		QLABEL2("SLEEPTIM", "Sleep Time"), 7, 0, 0,	ISTEP5,
		ITEM_VALUE,
		0, 240, 
		offsetof(struct nvmap, gsleeptime),
		nvramoffs0,
		NULL,
		& gsleeptime,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHSLEEPTIMER */
#if LCDMODE_COLORED
	// Для цветных дисплеев можно менять цвет фона
//	{
//		QLABEL("BLUE BG "), 8, 3, RJ_ON,	ISTEP1,
//		ITEM_VALUE,
//		0, 1,
//		offsetof(struct nvmap, gbluebgnd),
//		nvramoffs0,
//		NULL,
//		& gbluebgnd,
//		getzerobase, /* складывается со смещением и отображается */
//	},
#endif
	{
		QLABEL2("SHOW dBm", "Show dBm"), 8, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, gshowdbm),
		nvramoffs0,
		NULL,
		& gshowdbm,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("FREQ FPS"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		4, 25,							/* частота обновления показаний частоты от 5 до 25 раз в секунду */
		offsetof(struct nvmap, gdisplayfreqsfps),
		nvramoffs0,
		NULL,
		& gdisplayfreqsfps,
		getzerobase, /* складывается со смещением и отображается */
	},
#if WITHBARS
	{
		QLABEL("BARS FPS"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		4, 40,							/* частота обновления барграфов от 5 до 40 раз в секунду */
		offsetof(struct nvmap, gdisplaybarsfps),
		nvramoffs0,
		NULL,
		& gdisplaybarsfps,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHBARS */
#if WITHSPECTRUMWF
	{
		QLABEL2("VIEW STL", "View style"), 7, 5, RJ_VIEW, ISTEP1,
		ITEM_VALUE,
		0, VIEW_COUNT - 1,				/* стиль отображения спектра и панорамы */
		offsetof(struct nvmap, gviewstyle),
		nvramoffs0,
		NULL,
		& gviewstyle,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL2("FREQ MRK", "Freq marker"), 7, 5, RJ_YES, ISTEP1,
		ITEM_VALUE,
		0, 1,				/* Для VIEW_3DSS - индикация полосы пропускания на спектре */
		offsetof(struct nvmap, gview3dss_mark),
		nvramoffs0,
		NULL,
		& gview3dss_mark,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("TOP DB  "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		WITHTOPDBMIN, WITHTOPDBMAX,							/* сколько не показывать сверху */
		offsetof(struct nvmap, bandgroups [0].gtopdbspe),
		nvramoffs_bandgroup,
		NULL,
		& gtopdbspe,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("BOTTM DB"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		WITHBOTTOMDBMIN, WITHBOTTOMDBMAX,							/* диапазон отображаемых значений */
		offsetof(struct nvmap, bandgroups [0].gbottomdbspe),
		nvramoffs_bandgroup,
		NULL,
		& gbottomdbspe,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("WFPARAMS"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1,							/* водопад отдельными папаметрами */
		offsetof(struct nvmap, gwflevelsep),
		nvramoffs0,
		NULL,
		& gwflevelsep,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("TOP WF  "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		WITHTOPDBMIN, WITHTOPDBMAX,							/* сколько не показывать сверху */
		offsetof(struct nvmap, bandgroups [0].gtopdbwfl),
		nvramoffs_bandgroup,
		NULL,
		& gtopdbwfl,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("BOTTM WF"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		WITHBOTTOMDBMIN, WITHBOTTOMDBMAX,							/* диапазон отображаемых значений */
		offsetof(struct nvmap, bandgroups [0].gbottomdbwfl),
		nvramoffs_bandgroup,
		NULL,
		& gbottomdbwfl,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("STEP DB "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		3, 40,							/* диапазон отображаемых значений (0-отключаем отображение сетки уровней) */
		offsetof(struct nvmap, glvlgridstep),
		nvramoffs0,
		NULL,
		& glvlgridstep,
		getzerobase, /* складывается со смещением и отображается */
	},
#if BOARD_FFTZOOM_POW2MAX > 0
	{
		QLABEL("ZOOM PAN"), 7, 0, RJ_POW2,	ISTEP1,
		ITEM_VALUE,
		0, BOARD_FFTZOOM_POW2MAX,							/* уменьшение отображаемого участка спектра */
		offsetof(struct nvmap, bandgroups [0].gzoomxpow2),
		nvramoffs_bandgroup,
		NULL,
		& gzoomxpow2,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* BOARD_FFTZOOM_POW2MAX > 0 */
	{
		QLABEL2("SPEC TX ", "TX Spectrum"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1,							/* разрешение или запрет раскраски спектра */
		offsetof(struct nvmap, gtxloopback),
		nvramoffs0,
		NULL,
		& gtxloopback,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("BETA PAN"), 7, 2, 0,	ISTEP1,
		ITEM_VALUE,
		10, 100,							/* beta - парамеры видеофильтра спектра */
		offsetof(struct nvmap, gspecbeta100),
		nvramoffs0,
		NULL,
		& gspecbeta100,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("BETA WFL"), 7, 2, 0,	ISTEP1,
		ITEM_VALUE,
		10, 100,							/* beta - парамеры видеофильтра водопада */
		offsetof(struct nvmap, gwflbeta100),
		nvramoffs0,
		NULL,
		& gwflbeta100,
		getzerobase, /* складывается со смещением и отображается */
	},
#if (WITHSWRMTR || WITHSHOWSWRPWR)
	{
		QLABEL2("SMETER  ", "S-meter Type"), 7, 3, RJ_SMETER,	ISTEP1,
		ITEM_VALUE,
		0, 1,							/* выбор внешнего вида прибора - стрелочный или градусник */
		offsetof(struct nvmap, gsmetertype),
		nvramoffs0,
		NULL,
		& gsmetertype,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* (WITHSWRMTR || WITHSHOWSWRPWR) */
#endif /* WITHSPECTRUMWF */
#if WITHDSPEXTDDC
	{
		QLABEL2("SHOW OVF", "ADC OVF Show"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1,							/* разрешение или запрет раскраски спектра */
		offsetof(struct nvmap, gshowovf),
		nvramoffs0,
		NULL,
		& gshowovf,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHDSPEXTDDC */
#if defined (RTC1_TYPE)
#if ! WITHFLATMENU
	{
		QLABEL("CLOCK   "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpclock),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		QLABEL("TM YEAR "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE, 
		2015, 2099, 
		MENUNONVRAM, //offsetof(struct nvmap, tunerind),
		nvramoffs0,
		& grtcyear,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("TM MONTH"), 7, 3, RJ_MONTH,	ISTEP1,
		ITEM_VALUE, 
		1, 12, 
		MENUNONVRAM, //offsetof(struct nvmap, tunerind),
		nvramoffs0,
		NULL,
		& grtcmonth,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("TM DAY  "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE, 
		1, 31, 
		MENUNONVRAM, //offsetof(struct nvmap, tunerind),
		nvramoffs0,
		NULL,
		& grtcday,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("TM HOUR "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE, 
		0, 23, 
		MENUNONVRAM, //offsetof(struct nvmap, tunerind),
		nvramoffs0,
		NULL,
		& grtchour,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("TM MIN  "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE, 
		0, 59, 
		MENUNONVRAM, //offsetof(struct nvmap, tunerind),
		nvramoffs0,
		NULL,
		& grtcminute,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("TM SET  "), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE, 
		0, 1, 
		MENUNONVRAM, //offsetof(struct nvmap, tunerind),
		nvramoffs0,
		NULL,
		& grtcstrobe,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* defined (RTC1_TYPE) */
#if ! WITHFLATMENU
	{
		QLABEL2("FILTERS ", "Filters"), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpfilters),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
#if WITHIF4DSP
	{
		QLABEL("NR LEVEL"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, NRLEVELMAX, 
		offsetof(struct nvmap, gnoisereductvl),
		nvramoffs0,
		NULL,
		& gnoisereductvl,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHIF4DSP */
#if WITHIF4DSP
	{
		QLABEL("CW W WDT"), 7, 2, 0, 	ISTEP10,	// CW bandwidth for WIDE
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		10, 180,			/* 100 Hz..1800, Hz in 100 Hz steps */
		RMT_BWPROPSLEFT_BASE(BWPROPI_CWWIDE),
		nvramoffs0,
		NULL,
		& bwprop_cwwide.left10_width10,
		getzerobase, 
	},
	{
		QLABEL("CW N WDT"), 7, 2, 0, 	ISTEP10,	// CW bandwidth for NARROW
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		10, 180,			/* 100 Hz..1800, Hz in 100 Hz steps */
		RMT_BWPROPSLEFT_BASE(BWPROPI_CWNARROW),
		nvramoffs0,
		NULL,
		& bwprop_cwnarrow.left10_width10,
		getzerobase, 
	},
	{
		QLABEL("SSB W HI"), 6, 1, 0,	ISTEP1,		/* Подстройка полосы пропускания - SSB WIDE */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWRIGHTMIN, BWRIGHTMAX, 		// 0.8 kHz-18 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_SSBWIDE),
		nvramoffs0,
		NULL,
		& bwprop_ssbwide.right100,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("SSB W LO"), 7, 2, 0,	ISTEP5,		/* Подстройка полосы пропускания - SSB WIDE */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWLEFTMIN, BWLEFTMAX, 		// 50 Hz-700 Hz
		RMT_BWPROPSLEFT_BASE(BWPROPI_SSBWIDE),
		nvramoffs0,
		NULL,
		& bwprop_ssbwide.left10_width10,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("SSBW AFR"), 3 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		AFRESPONCEMIN, AFRESPONCEMAX,			/* изменение тембра звука - на Samplerate/2 АЧХ изменяется на столько децибел  */
		RMT_BWPROPSAFRESPONCE_BASE(BWPROPI_SSBWIDE),
		nvramoffs0,
		NULL,
		& bwprop_ssbwide.afresponce,
		getafresponcebase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("SSB M HI"), 6, 1, 0,	ISTEP1,		/* Подстройка полосы пропускания - SSB MEDIUM */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWRIGHTMIN, BWRIGHTMAX, 		// 0.8 kHz-18 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_SSBMEDIUM),
		nvramoffs0,
		NULL,
		& bwprop_ssbmedium.right100,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("SSB M LO"), 7, 2, 0,	ISTEP5,		/* Подстройка полосы пропускания - SSB MEDIUM */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWLEFTMIN, BWLEFTMAX, 		// 50 Hz-700 Hz
		RMT_BWPROPSLEFT_BASE(BWPROPI_SSBMEDIUM),
		nvramoffs0,
		NULL,
		& bwprop_ssbmedium.left10_width10,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("SSBM AFR"), 3 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		AFRESPONCEMIN, AFRESPONCEMAX,			/* изменение тембра звука - на Samplerate/2 АЧХ изменяется на столько децибел  */
		RMT_BWPROPSAFRESPONCE_BASE(BWPROPI_SSBMEDIUM),
		nvramoffs0,
		NULL,
		& bwprop_ssbmedium.afresponce,
		getafresponcebase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("SSB N HI"), 6, 1, 0,	ISTEP1,		/* Подстройка полосы пропускания - SSB NARROW */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWRIGHTMIN, BWRIGHTMAX, 		// 0.8 kHz-18 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_SSBNARROW),
		nvramoffs0,
		NULL,
		& bwprop_ssbnarrow.right100,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("SSB N LO"), 7, 2, 0,	ISTEP5,		/* Подстройка полосы пропускания - SSB NARROW */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWLEFTMIN, BWLEFTMAX, 		// 50 Hz-700 Hz
		RMT_BWPROPSLEFT_BASE(BWPROPI_SSBNARROW),
		nvramoffs0,
		NULL,
		& bwprop_ssbnarrow.left10_width10,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("SSBN AFR"), 3 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		AFRESPONCEMIN, AFRESPONCEMAX,			/* изменение тембра звука - на Samplerate/2 АЧХ изменяется на столько децибел  */
		RMT_BWPROPSAFRESPONCE_BASE(BWPROPI_SSBNARROW),
		nvramoffs0,
		NULL,
		& bwprop_ssbnarrow.afresponce,
		getafresponcebase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AM W HI "), 6, 1, 0,	ISTEP2,		/* Подстройка полосы пропускания - AM WIDE */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWRIGHTMIN, BWRIGHTMAX, 		// 0.8 kHz-18 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_AMWIDE),
		nvramoffs0,
		NULL,
		& bwprop_amwide.right100,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AM W LO "), 7, 2, 0,	ISTEP5,		/* подстройка полосы пропускания - AM WIDE */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWLEFTMIN, BWLEFTMAX,		// 50 Hz..700 Hz
		RMT_BWPROPSLEFT_BASE(BWPROPI_AMWIDE),
		nvramoffs0,
		NULL,
		& bwprop_amwide.left10_width10,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AM W AFR"), 3 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		AFRESPONCEMIN, AFRESPONCEMAX,			/* изменение тембра звука - на Samplerate/2 АЧХ изменяется на столько децибел  */
		RMT_BWPROPSAFRESPONCE_BASE(BWPROPI_AMWIDE),
		nvramoffs0,
		NULL,
		& bwprop_amwide.afresponce,
		getafresponcebase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AM N HI "), 6, 1, 0,	ISTEP2,		/* Подстройка полосы пропускания - AM NARROW */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWRIGHTMIN, BWRIGHTMAX, 		// 0.8 kHz-18 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_AMNARROW),
		nvramoffs0,
		NULL,
		& bwprop_amnarrow.right100,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AM N LO "), 7, 2, 0,	ISTEP5,		/* подстройка полосы пропускания - AM NARROW */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWLEFTMIN, BWLEFTMAX,		// 50 Hz..700 Hz
		RMT_BWPROPSLEFT_BASE(BWPROPI_AMNARROW),
		nvramoffs0,
		NULL,
		& bwprop_amnarrow.left10_width10,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AM N AFR"), 3 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		AFRESPONCEMIN, AFRESPONCEMAX,			/* изменение тембра звука - на Samplerate/2 АЧХ изменяется на столько децибел  */
		RMT_BWPROPSAFRESPONCE_BASE(BWPROPI_AMNARROW),
		nvramoffs0,
		NULL,
		& bwprop_amnarrow.afresponce,
		getafresponcebase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("SSBTX HI"), 6, 1, 0,	ISTEP1,		/* Подстройка полосы пропускания - TX SSB */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWRIGHTMIN, BWRIGHTMAX, 		// 0.8 kHz-18 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_SSBTX),
		nvramoffs0,
		NULL,
		& bwprop_ssbtx.right100,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("SSBTX LO"), 7, 2, 0,	ISTEP1,		/* подстройка полосы пропускания - TX SSB */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWLEFTMIN, BWLEFTMAX,		// 50 Hz..700 Hz
		RMT_BWPROPSLEFT_BASE(BWPROPI_SSBTX),
		nvramoffs0,
		NULL,
		& bwprop_ssbtx.left10_width10,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("SSBTXAFR"), 3 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		AFRESPONCEMIN, AFRESPONCEMAX,			/* изменение тембра звука - на Samplerate/2 АЧХ изменяется на столько децибел  */
		RMT_BWPROPSAFRESPONCE_BASE(BWPROPI_SSBTX),
		nvramoffs0,
		NULL,
		& bwprop_ssbtx.afresponce,
		getafresponcebase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("DIGI HI "), 6, 1, 0,	ISTEP1,		/* Подстройка полосы пропускания - TX SSB */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWRIGHTMIN, BWRIGHTMAX, 		// 0.8 kHz-18 kHz
		RMT_BWPROPSRIGHT_BASE(BWPROPI_DIGIWIDE),
		nvramoffs0,
		NULL,
		& bwprop_digiwide.right100,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("DIGI LO "), 7, 2, 0,	ISTEP1,		/* подстройка полосы пропускания - TX SSB */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		BWLEFTMIN, BWLEFTMAX,		// 50 Hz..700 Hz
		RMT_BWPROPSLEFT_BASE(BWPROPI_DIGIWIDE),
		nvramoffs0,
		NULL,
		& bwprop_digiwide.left10_width10,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHIF4DSP */

#if WITHIFSHIFT && ! WITHPOTIFSHIFT
	// Увеличение значения параметра смещает слышимую часть спектра в более высокие частоты
	{
		QLABEL("IF SHIFT"), 4 + WSIGNFLAG, 2, 1, 	ISTEP50,
		ITEM_VALUE,
		IFSHIFTTMIN, IFSHIFTMAX,			/* -3 kHz..+3 kHz in 50 Hz steps */
		offsetof(struct nvmap, ifshifoffset),
		nvramoffs0,
		& ifshifoffset.value,
		NULL,
		getifshiftbase, 
	},
#endif /* WITHIFSHIFT && ! WITHPOTIFSHIFT */

#if WITHDUALFLTR	/* Переворот боковых за счёт переключения фильтра верхней или нижней боковой полосы */
	{
		QLABEL("BFO FREQ"), 7, 2, 1,	ISTEP10,
		ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lo4offset),
		nvramoffs0,
		& lo4offset,
		NULL,
		getlo4base, /* складывается со смещением и отображается */
	},
#elif WITHDUALBFO	/* Переворот боковых за счёт переключения частоты опорного генератора */
	{
		QLABEL("BFO USB "), 7, 2, 1,	ISTEP10,
		ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lo4offsets [0]),
		nvramoffs0,
		& lo4offsets [0],
		NULL,
		getlo4base, /* складывается со смещением и отображается */
	},
	{
		QLABEL("BFO LSB "), 7, 2, 1,	ISTEP10,
		ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lo4offsets [1]),
		nvramoffs0,
		& lo4offsets [1],
		NULL,
		getlo4base, /* складывается со смещением и отображается */
	},
#elif WITHFIXEDBFO	/* Переворот боковых за счёт 1-го гетродина (особенно, если нет подстраиваемого BFO) */
	{
		QLABEL("BFO FREQ"), 7, 2, 1,	ISTEP10,
		ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lo4offset),
		nvramoffs0,
		& lo4offset,
		NULL,
		getlo4base, /* складывается со смещением и отображается */
	},
	{
		QLABEL("LAST LSB"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, glo4lsb),
		nvramoffs0,
		NULL,
		& glo4lsb,
		getzerobase, /* складывается со смещением и отображается */
	},

#elif (defined (IF3_MODEL) && (IF3_MODEL != IF3_TYPE_DCRX) && (IF3_MODEL != IF3_TYPE_BYPASS))
	/* Обычная схема - выбор ПЧ делается перестановкой последнего гетеродина */

#if ! CTLSTYLE_SW2011ALL
#if WITHTX
	{
		QLABEL("DC TX CW"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, /* 0 - off, 1 - on */
		offsetof(struct nvmap, dctxmodecw),
		nvramoffs0,
		NULL,
		& dctxmodecw,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHTX */
#endif /* ! CTLSTYLE_SW2011ALL */

	#if (IF3_FMASK & IF3_FMASK_2P4)
	{
		QLABEL("2.4 USB "), 7, 2, 1,	ISTEP10,
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, usbe2p4),
		nvramoffs0,
		& fi_2p4.high,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	{
		QLABEL("2.4 LSB "), 7, 2, 1,	ISTEP10,
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lsbe2p4),
		nvramoffs0,
		& fi_2p4.low_or_center,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	#endif /* (IF3_FMASK & IF3_FMASK_2P4) */
	#if (IF3_FMASK & IF3_FMASK_2P7)
	{
		QLABEL("BFO7 USB"), 7, 2, 1,	ISTEP10,
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, usbe2p7),
		nvramoffs0,
		& fi_2p7.high,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	{
		QLABEL("BFO7 LSB"), 7, 2, 1,	ISTEP10,
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lsbe2p7),
		nvramoffs0,
		& fi_2p7.low_or_center,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7) != 0
	{
		QLABEL("CAR7 USB"), 7, 2, 1,	ISTEP10,
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, usbe2p7tx),
		nvramoffs0,
		& fi_2p7_tx.high,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	{
		QLABEL("CAR7 LSB"), 7, 2, 1,	ISTEP10,
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lsbe2p7tx),
		nvramoffs0,
		& fi_2p7_tx.low_or_center,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	#endif /* WITHTX && WITHSAMEBFO == 0 */
	#endif /* (IF3_FMASK & IF3_FMASK_2P7) */
	
	#if (IF3_FMASK & IF3_FMASK_3P1)
	{
		QLABEL("BFO USB "), 7, 2, 1,	ISTEP10,
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, usbe3p1),
		nvramoffs0,
		& fi_3p1.high,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	{
		QLABEL("BFO LSB "), 7, 2, 1,	ISTEP10,
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lsbe3p1),
		nvramoffs0,
		& fi_3p1.low_or_center,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_3P1) != 0
	{
		QLABEL("CAR USB "), 7, 2, 1,	ISTEP10,
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, usbe3p1tx),
		nvramoffs0,
		& fi_3p1_tx.high,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	{
		QLABEL("CAR LSB "), 7, 2, 1,	ISTEP10,
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lsbe3p1tx),
		nvramoffs0,
		& fi_3p1_tx.low_or_center,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	#endif /* WITHTX && WITHSAMEBFO == 0 */
	#endif /* (IF3_FMASK & IF3_FMASK_3P1) */
	
	#if (IF3_FMASK & IF3_FMASK_0P3)
	{
		QLABEL("HAVE 0.3"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, hascw0p3),
		nvramoffs0,
		NULL,
		& fi_0p3.present,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("CNTR 0.3"), 7, 2, 1,	ISTEP10,	/* центральная частота телеграфного фильтра */
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, carr0p3),
		nvramoffs0,
		& fi_0p3.low_or_center,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	#endif /* (IF3_FMASK & IF3_FMASK_0P3) */

	#if (IF3_FMASK & IF3_FMASK_0P5)
	{
		QLABEL("HAVE 0.5"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, hascw0p5),
		nvramoffs0,
		NULL,
		& fi_0p5.present,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("CNTR 0.5"), 7, 2, 1,	ISTEP10,	/* центральная частота телеграфного фильтра */
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, carr0p5),
		nvramoffs0,
		& fi_0p5.low_or_center,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	#endif /* (IF3_FMASK & IF3_FMASK_0P5) */

	#if (IF3_FMASK & IF3_FMASK_1P8)
	{
		QLABEL("HAVE 1.8"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, hascw1p8),
		nvramoffs0,
		NULL,
		& fi_1p8.present,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("1.8 USB "), 7, 2, 1,	ISTEP10,
		ITEM_FILTERU | ITEM_VALUE,
		10, IF3OFFS * 2 - 10, 
		offsetof(struct nvmap, usbe1p8),
		nvramoffs0,
		& fi_1p8.high,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	{
		QLABEL("1.8 LSB "), 7, 2, 1,	ISTEP10,
		ITEM_FILTERL | ITEM_VALUE,
		10, IF3OFFS * 2 - 10,
		offsetof(struct nvmap, lsbe1p8),
		nvramoffs0,
		& fi_1p8.low_or_center,
		NULL,
		NULL,	/* базоое значение для отображения берётся из структуры filter_t */
	},
	#endif /* (IF3_FMASK & IF3_FMASK_1P8) */

	#if (IF3_FMASK & IF3_FMASK_2P4)
	{
		QLABEL("HAVE 2.4"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, hascw2p4),
		nvramoffs0,
		NULL,
		& fi_2p4.present,
		getzerobase, /* складывается со смещением и отображается */
	},
	#endif /* (IF3_FMASK & IF3_FMASK_2P4) */

	#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P4)
	{
		QLABEL("HAVE T24"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, hascw2p4_tx),
		nvramoffs0,
		NULL,
		& fi_2p4_tx.present,
		getzerobase, /* складывается со смещением и отображается */
	},
	#endif /* WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P4) */

	#if WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7)
	{
		QLABEL("HAVE T27"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, hascw2p7_tx),
		nvramoffs0,
		NULL,
		& fi_2p7_tx.present,
		getzerobase, /* складывается со смещением и отображается */
	},
	#endif /* WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7) */

	#if 0 && WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_3P1)
	{
		QLABEL("HAVE T31"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, hascw3p1_tx),
		nvramoffs0,
		NULL,
		& fi_3p1_tx.present,
		getzerobase, /* складывается со смещением и отображается */
	},
	#endif /* WITHTX && WITHSAMEBFO == 0 && (IF3_FMASKTX & IF3_FMASK_2P7) */

	#if (IF3_FMASK & IF3_FMASK_6P0)
	{
		QLABEL("HAVE 6.0"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, hascw6p0),
		nvramoffs0,
		NULL,
		& fi_6p0.present,
		getzerobase, /* складывается со смещением и отображается */
	},
	/* нстройка центральной частоты для тех фильтров, у которых нет индивидуальной настройки скатов */
	{
		QLABEL("6K OFFS "), 4 + WSIGNFLAG, 2, 1, 	ISTEP10,
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		offsetof(struct nvmap, cfreq6k),
		nvramoffs0,
		& fi_6p0.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif /* (IF3_FMASK & IF3_FMASK_6P0) */

	#if (IF3_FMASK & IF3_FMASK_7P8)
	/* нстройка центральной частоты для тех фильтров, у которых нет индивидуальной настройки скатов */
	{
		QLABEL("7K8 OFFS"), 4 + WSIGNFLAG, 2, 1, 	ISTEP10,
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		offsetof(struct nvmap, cfreq7p8k),
		nvramoffs0,
		& fi_7p8.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif /* (IF3_FMASK & IF3_FMASK_7P8) */

	#if (IF3_FMASK & IF3_FMASK_8P0)
	/* нстройка центральной частоты для тех фильтров, у которых нет индивидуальной настройки скатов */
	{
		QLABEL("8K OFFS "), 4 + WSIGNFLAG, 2, 1, 	ISTEP10,
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		offsetof(struct nvmap, cfreq8k),
		nvramoffs0,
		& fi_8p0.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif
	#if (IF3_FMASK & IF3_FMASK_9P0)
	/* нстройка центральной частоты для тех фильтров, у которых нет индивидуальной настройки скатов */
	{
		QLABEL("9K OFFS "), 4 + WSIGNFLAG, 2, 1, 	ISTEP10,
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		offsetof(struct nvmap, cfreq9k),
		nvramoffs0,
		& fi_9p0.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif /* (IF3_FMASK & IF3_FMASK_9P0) */
	#if (IF3_FMASK & IF3_FMASK_15P0)
	#if WITHDEDICATEDNFM /* Для NFM отдельный тракт со своим фильтром */
	/* нстройка центральной частоты для тех фильтров, у которых нет индивидуальной настройки скатов */
	{
		QLABEL("NFM OFFS"), 4 + WSIGNFLAG, 2, 1, 	ISTEP10,
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		offsetof(struct nvmap, cfreq15k_nfm),
		nvramoffs0,
		& fi_15p0_tx_nfm.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#else /* WITHDEDICATEDNFM */
	/* нстройка центральной частоты для тех фильтров, у которых нет индивидуальной настройки скатов */
	{
		QLABEL("15K OFFS"), 4 + WSIGNFLAG, 2, 1, 	ISTEP10,
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		offsetof(struct nvmap, cfreq15k),
		nvramoffs0,
		& fi_15p0.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif /* WITHDEDICATEDNFM */
	#endif /* (IF3_FMASK & IF3_FMASK_15P0) */
	#if (IF3_FMASK & IF3_FMASK_17P0)
	/* нстройка центральной частоты для тех фильтров, у которых нет индивидуальной настройки скатов */
	{
		QLABEL("17K OFFS"), 4 + WSIGNFLAG, 2, 1, 	ISTEP10,
		ITEM_VALUE,
		0, IF3CEOFFS * 2,
		offsetof(struct nvmap, cfreq17k),
		nvramoffs0,
		& fi_17p0.ceoffset,
		NULL,
		getcefreqshiftbase, 
	},
	#endif

#endif /* defined (IF3_MODEL) && (IF3_MODEL != IF3_TYPE_DCRX) && (IF3_MODEL != IF3_TYPE_BYPASS) */

#if WITHNOTCHFREQ
#if ! WITHFLATMENU
	{
		QLABEL("NOTCH   "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpnotch),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		QLABEL("NOTCH   "), 8, 3, RJ_NOTCH,	ISTEP1,		/* управление режимом NOTCH */
		ITEM_VALUE,
		0, NOTCHMODE_COUNT - 1,
		RMT_NOTCHTYPE_BASE,							/* управление режимом NOTCH */
		nvramoffs0,
		NULL,
		& gnotchtype,
		getzerobase, /* складывается со смещением и отображается */
	},
	#if ! WITHPOTNOTCH
	{
		QLABEL("NTCH FRQ"), 7, 2, 1,	ISTEP50,		/* управление частотой NOTCH. */
		ITEM_VALUE,
		WITHNOTCHFREQMIN, WITHNOTCHFREQMAX,
		offsetof(struct nvmap, gnotchfreq),	/* центральная частота NOTCH */
		nvramoffs0,
		& gnotchfreq.value,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("NTCH WDT"), 7, 0, 0,	ISTEP50,		/* полоса режекции NOTCH. */
		ITEM_VALUE,
		WITHNOTCHWIDTHMIN, WITHNOTCHWIDTHMAX,
		offsetof(struct nvmap, gnotchwidth),	/* полоса режекции NOTCH */
		nvramoffs0,
		& gnotchwidth.value,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
	#endif /* ! WITHPOTNOTCH */
#elif WITHNOTCHONOFF
#if ! WITHFLATMENU
	{
		QLABEL("NOTCH   "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpnotch),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		QLABEL("NOTCH   "), 8, 3, RJ_ON,	ISTEP1,		/* управление режимом NOTCH */
		ITEM_VALUE,
		0, NOTCHMODE_COUNT - 1,
		RMT_NOTCH_BASE,							/* управление режимом NOTCH */
		nvramoffs0,
		NULL,
		& gnotch,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHNOTCHFREQ */

#if defined (IF3_MODEL) && (IF3_MODEL != IF3_TYPE_DCRX)
#if ! WITHPOTPBT && WITHPBT // && (LO3_SIDE != LOCODE_INVALID)
#if ! WITHFLATMENU
	{
		QLABEL("PBTS   "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrppbts),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		QLABEL("PBT     "), 4 + WSIGNFLAG, 2, 1, 	ISTEP50,
		ITEM_VALUE,
		PBTMIN, PBTMAX,			/* -15 kHz..+15 kHz in 5 Hz steps */
		offsetof(struct nvmap, pbtoffset),
		nvramoffs0,
		& gpbtoffset,
		NULL,
		getpbtbase, 
	},
#endif /* ! WITHPOTPBT && WITHPBT && (LO3_SIDE != LOCODE_INVALID) */
#endif /* defined (IF3_MODEL) && (IF3_MODEL != IF3_TYPE_DCRX) */

#if WITHELKEY
#if ! WITHFLATMENU
	{
		QLABEL("ELKEY   "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpelkey),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	// разрешено не только в случае наличия электронного ключа - требуется при переключении режимов CW/SSB
	{
		QLABEL("CW PITCH"), 7, 2, 0, 	ISTEP1,
		ITEM_VALUE,
		CWPITCHMIN10, CWPITCHMAX10,	// 40, 190,			/* 400 Hz..1900, Hz in 10 Hz steps */
		offsetof(struct nvmap, gcwpitch10),
		nvramoffs0,
		NULL,
		& gcwpitch10,
		getzerobase,
	},
  #if ! WITHPOTWPM
	{
		QLABEL("CW SPEED"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		CWWPMMIN, CWWPMMAX,		// minimal WPM = 10, maximal = 60 (also changed by command KS).
		offsetof(struct nvmap, elkeywpm),
		nvramoffs0,
		NULL,
		& elkeywpm.value,
		getzerobase, 
	},
  #endif /* ! WITHPOTWPM */
  #if WITHVIBROPLEX
	{
		QLABEL("VIBROPLX"), 7, 0, 0,	ISTEP1,		/* скорость уменьшения длительности точки и паузы - имитация виброплекса */
		ITEM_VALUE,
		0, 5,		// minimal 0 - без эффекта Виброплекса
		offsetof(struct nvmap, elkeyslope),
		nvramoffs0,
		NULL,
		& elkeyslope,
		getzerobase, 
	},
  #endif /* WITHVIBROPLEX */
	{
		QLABEL("KEYER   "), 6, 0, RJ_ELKEYMODE,	ISTEP1,
		ITEM_VALUE,
		0, 3,	/* режим электронного ключа - 0 - ACS, 1 - electronic key, 2 - straight key, 3 - BUG key */
		offsetof(struct nvmap, elkeymode),
		nvramoffs0,
		NULL,
		& elkeymode,
		getzerobase, 
	},
	{
		QLABEL("KEY REV "), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1,	/* режим электронного ключа - поменять местами точки с тире или нет. */
		offsetof(struct nvmap, elkeyreverse),
		nvramoffs0,
		NULL,
		& elkeyreverse,
		getzerobase, 
	},
	{
		QLABEL("DASH LEN"), 7, 1, 0,	ISTEP1,
		ITEM_VALUE,
		23, 45, 
		offsetof(struct nvmap, dashratio),
		nvramoffs0,
		NULL,
		& dashratio,
		getzerobase, 
	},
	{
		QLABEL("DOT LEN "), 7, 1, 0,	ISTEP1,
		ITEM_VALUE,
		7, 13, 
		offsetof(struct nvmap, spaceratio),
		nvramoffs0,
		NULL,
		& spaceratio,
		getzerobase, 
	},
#if WITHTX
	{
		QLABEL("BREAK-IN"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, bkinenable),
		nvramoffs0,
		NULL,
		& bkinenable,
		getzerobase, 
	},
	{
		QLABEL("CW DELAY"), 7, 2, 0,	ISTEP1,	/* задержка в десятках ms */
		ITEM_VALUE,
		5, 160,						/* 0.05..1.6 секунды */
		offsetof(struct nvmap, bkindelay),
		nvramoffs0,
		NULL,
		& bkindelay,
		getzerobase, 
	},
#endif /* WITHTX */
#if WITHIF4DSP
	{
		QLABEL("EDGE TIM"), 7, 0, 0,	ISTEP1,		/* Set the rise time of the transmitted CW envelope. */
		ITEM_VALUE,
		2, 16, 
		offsetof(struct nvmap, gcwedgetime),	/* Время нарастания/спада огибающей телеграфа при передаче - в 1 мс */
		nvramoffs0,
		NULL,
		& gcwedgetime,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHIF4DSP */
#if WITHTX && WITHIF4DSP
	{
		QLABEL("SSB TXCW"), 8, 3, RJ_ON,	ISTEP1,		/*  */
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, gcwssbtx),	/* разрешение передачи телеграфа как тона в режиме SSB */
		nvramoffs0,
		NULL,
		& gcwssbtx,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHTX && WITHIF4DSP */
#endif /* WITHELKEY */
#if WITHDSPEXTDDC	/* QLABEL("ВоронёнокQLABEL(" с DSP и FPGA */
#if ! WITHFLATMENU
	{
		QLABEL("RF ADC  "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrprfadc),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
#if 0 && ! CTLREGMODE_STORCH_V9A
	{
		QLABEL("ADC RAND"), 8, 3, RJ_ON,	ISTEP1,	/* управление интерфейсом в LTC2208 */
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, gadcrand),
		nvramoffs0,
		NULL,
		& gadcrand,
		getzerobase, 
	},
#endif /* ! CTLREGMODE_STORCH_V9A */
	{
		QLABEL("ADC DITH"), 8, 3, RJ_ON,	ISTEP1,	/* управление зашумлением в LTC2208 */
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, gdither),
		nvramoffs0,
		NULL,
		& gdither,
		getzerobase, 
	},
	{
		QLABEL("ADC FIFO"), 8, 3, RJ_ON,	ISTEP1,	/*  */
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, gadcfifo),
		nvramoffs0,
		NULL,
		& gadcfifo,
		getzerobase, 
	},
	{
		QLABEL("ADC OFFS"), 5 + WSIGNFLAG, 0, 0,	ISTEP1,	/* смещение для выходного сигнала с АЦП */
		ITEM_VALUE,
		ADCOFFSETMID - 200, ADCOFFSETMID + 200,
		offsetof(struct nvmap, gadcoffset),
		nvramoffs0,
		& gadcoffset,
		NULL,
		getadcoffsbase,	/* складывается со смещением и отображается */
	},
#endif /* WITHDSPEXTDDC */
#if WITHTX
#if WITHVOX
#if ! WITHFLATMENU
	{
		QLABEL("VOX     "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpvox),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		QLABEL("VOX EN  "), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, gvoxenable),
		nvramoffs0,
		NULL,
		& gvoxenable,
		getzerobase, 
	},
	{
		QLABEL("VOXDELAY"), 7, 2, 0,	ISTEP5,	/* 50 mS step of changing value */
		ITEM_VALUE,
		WITHVOXDELAYMIN, WITHVOXDELAYMAX,						/* 0.1..2.5 seconds delay */
		offsetof(struct nvmap, voxdelay),
		nvramoffs0,
		NULL,
		& voxdelay,
		getzerobase, 
	},
	{
		QLABEL("VOX LEVL"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		WITHVOXLEVELMIN, WITHVOXLEVELMAX,
		offsetof(struct nvmap, gvoxlevel),
		nvramoffs0,
		NULL,
		& gvoxlevel,
		getzerobase, 
	},
	{
		QLABEL("AVOX LEV"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		WITHAVOXLEVELMIN, WITHAVOXLEVELMAX,
		offsetof(struct nvmap, gavoxlevel),
		nvramoffs0,
		NULL,
		& gavoxlevel,
		getzerobase, 
	},
#endif /* WITHVOX */
#endif /* WITHTX */
#if WITHCAT
#if ! WITHFLATMENU
	{
		QLABEL("CAT     "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpcat),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		QLABEL("CAT ENAB"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, catenable),
		nvramoffs0,
		NULL,
		& catenable,
		getzerobase,
	},
	#if WITHUSBCDCACM == 0
	{
		QLABEL("CAT SPD "), 7, 0, RJ_CATSPEED,	ISTEP1,
		ITEM_VALUE,
		0, (sizeof catbr2int / sizeof catbr2int [0]) - 1,
		offsetof(struct nvmap, catbaudrate),
		nvramoffs0,
		NULL,
		& catbaudrate,
		getzerobase,
	},
	#endif /* WITHUSBCDCACM == 0 */
#if WITHTX
	{
		QLABEL("CAT PTT "), 8, 8, RJ_CATSIG,	ISTEP1,
		ITEM_VALUE,
		0, BOARD_CATSIG_count - 1,
		offsetof(struct nvmap, catsigptt),
		nvramoffs0,
		NULL,
		& catsigptt,
		getzerobase,
	},
#endif /* WITHTX */
	{
		QLABEL("CAT KEY "), 8, 8, RJ_CATSIG,	ISTEP1,
		ITEM_VALUE,
		0, BOARD_CATSIG_count - 1,
		offsetof(struct nvmap, catsigkey),
		nvramoffs0,
		NULL,
		& catsigkey,
		getzerobase, 
	},
#endif /* WITHCAT */

#if WITHSUBTONES && WITHTX
#if ! WITHFLATMENU
	{
		QLABEL("CTCSS   "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpctcss),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		QLABEL("CTCSS   "), 8, 3, RJ_ON,	ISTEP1,	//  Continuous Tone-Coded Squelch System or CTCSS control
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, gctssenable),
		nvramoffs0,
		NULL,
		& gctssenable,
		getzerobase, 
	},
	{
		QLABEL("CTCSS FQ"), 7, 1, RJ_SUBTONE,	ISTEP1,	//  Continuous Tone-Coded Squelch System or CTCSS freq
		ITEM_VALUE,
		0, sizeof gsubtones / sizeof gsubtones [0] - 1, 
		offsetof(struct nvmap, gsubtonei),
		nvramoffs0,
		NULL,
		& gsubtonei,
		getzerobase, 
	},
#if WITHIF4DSP
	{
		QLABEL("CTCSSLVL"), 7, 0, 0,	ISTEP1,		/* Select the CTCSS transmit level. */
		ITEM_VALUE,
		0, 100, 
		offsetof(struct nvmap, gsubtonelevel),	/* Уровень сигнала самоконтроля в процентах - 0%..100% */
		nvramoffs0,
		NULL,
		& gsubtonelevel,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHIF4DSP */
#endif /* WITHSUBTONES && WITHTX */
#if ! WITHFLATMENU
	{
		QLABEL("AUDIO   "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpaudio),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
#if WITHIF4DSP
	#if ! WITHPOTAFGAIN
	{
		QLABEL("AF GAIN "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		BOARD_AFGAIN_MIN, BOARD_AFGAIN_MAX, 					// Громкость в процентах
		offsetof(struct nvmap, afgain1),
		nvramoffs0,
		& afgain1.value,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
	#endif /* ! WITHPOTAFGAIN */
	#if ! WITHPOTIFGAIN
	{
		QLABEL("RF GAIN "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		BOARD_IFGAIN_MIN, BOARD_IFGAIN_MAX, 					// Усиление ПЧ/ВЧ в процентах
		offsetof(struct nvmap, rfgain1),
		nvramoffs0,
		& rfgain1.value,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
	#endif /* ! WITHPOTIFGAIN */
#endif /* WITHIF4DSP */
#if (SIDETONE_TARGET_BIT != 0) || WITHINTEGRATEDDSP
	{
		QLABEL("KEY BEEP"), 6, 2, 0, 	ISTEP5,		/* регулировка тона озвучки клавиш */
		ITEM_VALUE,
		80, 250,			/* 800 Hz..2500, Hz in 50 Hz steps */
		offsetof(struct nvmap, gkeybeep10),
		nvramoffs0,
		NULL,
		& gkeybeep10,
		getzerobase, 
	},
#endif /* (SIDETONE_TARGET_BIT != 0) || WITHINTEGRATEDDSP */
#if WITHMUTEALL && WITHTX
	{
		QLABEL("MUTE ALL"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, gmuteall),
		nvramoffs0,
		NULL,
		& gmuteall,
		getzerobase, 
	},
#endif /* WITHMUTEALL && WITHTX */
#if WITHIF4DSP
	{
		QLABEL("SQUELCH "), 7, 0, 0,	ISTEP1,		/* squelch level */
		ITEM_VALUE,
		0, SQUELCHMAX, 
		offsetof(struct nvmap, gsquelch),	/* уровень сигнала болше которого открывается шумодав */
		nvramoffs0,
		NULL,
		& gsquelch.value,
		getzerobase, /* складывается со смещением и отображается */
	},
#if ! WITHPOTNFMSQL
	{
		QLABEL("SQUELNFM"), 7, 0, 0,	ISTEP1,		/* squelch level */
		ITEM_VALUE,
		0, SQUELCHMAX,
		offsetof(struct nvmap, gsquelchNFM),	/* уровень сигнала болше которого открывается шумодав */
		nvramoffs0,
		NULL,
		& gsquelchNFM,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* ! WITHPOTNFMSQL */
	{
		QLABEL("SDTN LVL"), 7, 0, 0,	ISTEP1,		/* Select the CW sidetone or keypad sound output level.. */
		ITEM_VALUE,
		0, 100, 
		offsetof(struct nvmap, gsidetonelevel),	/* Уровень сигнала самоконтроля в процентах - 0%..100% */
		nvramoffs0,
		NULL,
		& gsidetonelevel,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("MONI EN "), 8, 3, RJ_ON,	ISTEP1,		/* Select the monitoring sound output enable */
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, gmoniflag),	/* разрешение самопрослушивания */
		nvramoffs0,
		NULL,
		& gmoniflag,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHIF4DSP */
#if WITHTX && WITHIF4DSP
#if WITHWAVPLAYER || WITHSENDWAV
	{
		QLABEL("LOOP MSG"), 7, 0, 0,	ISTEP1,		/* Select the monitoring sound output level.. */
		ITEM_VALUE,
		0, (sizeof loopnames / sizeof loopnames [0]) - 1,
		offsetof(struct nvmap, gloopmsg),	/* Уровень сигнала самопрослушивания в процентах - 0%..100% */
		nvramoffs0,
		NULL,
		& gloopmsg,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("LOOP SEC"), 7, 0, 0,	ISTEP1,		/* Select the monitoring sound output level.. */
		ITEM_VALUE,
		15, 240,
		offsetof(struct nvmap, gloopsec),	/* Уровень сигнала самопрослушивания в процентах - 0%..100% */
		nvramoffs0,
		NULL,
		& gloopsec,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHWAVPLAYER || WITHSENDWAV */
#if WITHMIC1LEVEL
	{
		QLABEL("MIC LEVL"), 7, 0, 0,	ISTEP1,		/* подстройка усиления микрофонного усилителя через меню. */
		ITEM_VALUE,
		WITHMIKEINGAINMIN, WITHMIKEINGAINMAX,
		offsetof(struct nvmap, gmik1level),	/* усиление микрофонного усилителя */
		nvramoffs0,
		& gmik1level,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* ITHMIC1LEVEL */
	#if WITHAFCODEC1HAVELINEINLEVEL	/* кодек имеет управление усилением с линейного входа */
	{
		QLABEL("LINE LVL"), 7, 0, 0,	ISTEP1,		/* подстройка усиления с линейного входа через меню. */
		ITEM_VALUE,
		WITHLINEINGAINMIN, WITHLINEINGAINMAX, 
		offsetof(struct nvmap, glineamp),	/* усиление с линейного входа */
		nvramoffs0,
		& glineamp,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
	#endif /* WITHAFCODEC1HAVELINEINLEVEL */
	{
		QLABEL("MIC SSB "), 8, 5, RJ_TXAUDIO,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, BOARD_TXAUDIO_count - 1, 					// при SSB/AM/FM передача с тестовых источников
		RMT_TXAUDIO_BASE(MODE_SSB),
		nvramoffs0,
		NULL,
		& gtxaudio [MODE_SSB],
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("MIC DIG "), 8, 5, RJ_TXAUDIO,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, BOARD_TXAUDIO_count - 1, 					// при SSB/AM/FM передача с тестовых источников
		RMT_TXAUDIO_BASE(MODE_DIGI),
		nvramoffs0,
		NULL,
		& gtxaudio [MODE_DIGI],
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("MIC AM  "), 8, 5, RJ_TXAUDIO,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, BOARD_TXAUDIO_count - 1, 					// при SSB/AM/FM передача с тестовых источников
		RMT_TXAUDIO_BASE(MODE_AM),
		nvramoffs0,
		NULL,
		& gtxaudio [MODE_AM],
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("MIC FM  "), 8, 5, RJ_TXAUDIO,	ISTEP1,
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, BOARD_TXAUDIO_count - 1, 					// при SSB/AM/FM передача с тестовых источников
		RMT_TXAUDIO_BASE(MODE_NFM),
		nvramoffs0,
		NULL,
		& gtxaudio [MODE_NFM],
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("MIC AGC "), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,	
		0, 1, 					/* Включение программной АРУ перед модулятором */
		offsetof(struct nvmap, gmikeagc),
		nvramoffs0,
		NULL,
		& gmikeagc,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("MICAGCGN"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,	
		WITHMIKEAGCMIN, WITHMIKEAGCMAX, 	/* максимальное усиление АРУ микрофона в дБ */
		offsetof(struct nvmap, gmikeagcgain),
		nvramoffs0,
		NULL,
		& gmikeagcgain,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("MIK CLIP"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,	
		0, 90, 					/* Ограничение */
		offsetof(struct nvmap, gmikehclip),
		nvramoffs0,
		NULL,
		& gmikehclip,
		getzerobase, /* складывается со смещением и отображается */
	},
#if WITHCOMPRESSOR
	{
		QLABEL("COMP ATK"), 7, 0, 0,	ISTEP5,
		ITEM_VALUE,	
		WITHCOMPATTACKMIN, WITHCOMPATTACKMAX,
		offsetof(struct nvmap, gcompressor_attack),
		nvramoffs0,
		NULL,
		& gcompressor_attack,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("COMP RLS"), 7, 0, 0,	ISTEP5,
		ITEM_VALUE,	
		WITHCOMPRELEASEMIN, WITHCOMPRELEASEMAX,
		offsetof(struct nvmap, gcompressor_release),
		nvramoffs0,
		NULL,
		& gcompressor_release,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("COMP HLD"), 7, 0, 0,	ISTEP5,
		ITEM_VALUE,	
		WITHCOMPHOLDMIN, WITHCOMPHOLDMAX,
		offsetof(struct nvmap, gcompressor_hold),
		nvramoffs0,
		NULL,
		& gcompressor_hold,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("COMP GN "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,	
		WITHCOMPGAINMIN, WITHCOMPGAINMAX,
		offsetof(struct nvmap, gcompressor_gain),
		nvramoffs0,
		NULL,
		& gcompressor_gain,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("COMP TH "), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,	
		WITHCOMPTHRESHOLDMIN, WITHCOMPTHRESHOLDMAX,
		offsetof(struct nvmap, gcompressor_threshold),
		nvramoffs0,
		NULL,
		& gcompressor_threshold,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHCOMPRESSOR */
#if WITHREVERB
	{
		QLABEL2("REVERB  ", "Reverberator"), 7, 0, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 					/* ревербератор */
		offsetof(struct nvmap, greverb),
		nvramoffs0,
		NULL,
		& greverb,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL2("RVB TIME", "Reverb Delay"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		WITHREVERBDELAYMIN, WITHREVERBDELAYMAX, 					/* ревербератор - задержка */
		offsetof(struct nvmap, greverbdelay),
		nvramoffs0,
		NULL,
		& greverbdelay,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL2("RVB LOSS", "Reverb Loss"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		WITHREVERBLOSSMIN, WITHREVERBLOSSMAX, 					/* ревербератор - ослабление на возврате */
		offsetof(struct nvmap, greverbloss),
		nvramoffs0,
		NULL,
		& greverbloss,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHREVERB */
	{
		QLABEL2("MIKBOOST", "Mike boost"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,	
		0, 1, 					// предусилитель сигнала с микрофона
		offsetof(struct nvmap, gmikeboost20db),
		nvramoffs0,
		NULL,
		& gmikeboost20db,
		getzerobase, /* складывается со смещением и отображается */
	},
	#if WITHAFCODEC1HAVEPROC	/* кодек имеет управление обработкой микрофонного сигнала (эффекты, эквалайзер, ...) */
	{
		QLABEL("MIK EQUA"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, gmikeequalizer),
		nvramoffs0,
		NULL,
		& gmikeequalizer,
		getzerobase, /* складывается со смещением и отображается */
	},
	// Эквалайзер 80Hz 230Hz 650Hz 	1.8kHz 5.3kHz
	{
		QLABEL("EQUA .08"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, EQUALIZERBASE * 2,
		offsetof(struct nvmap, gmikeequalizerparams [0]),
		nvramoffs0,
		NULL,
		& gmikeequalizerparams [0],
		getequalizerbase, /* складывается с -12 и отображается */
	},
	{
		QLABEL("EQUA .23"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, EQUALIZERBASE * 2,
		offsetof(struct nvmap, gmikeequalizerparams [1]),
		nvramoffs0,
		NULL,
		& gmikeequalizerparams [1],
		getequalizerbase, /* складывается с -12 и отображается */
	},
	{
		QLABEL("EQUA .65"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, EQUALIZERBASE * 2,
		offsetof(struct nvmap, gmikeequalizerparams [2]),
		nvramoffs0,
		NULL,
		& gmikeequalizerparams [2],
		getequalizerbase, /* складывается с -12 и отображается */
	},
	{
		QLABEL("EQUA 1.8"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, EQUALIZERBASE * 2,
		offsetof(struct nvmap, gmikeequalizerparams [3]),
		nvramoffs0,
		NULL,
		& gmikeequalizerparams [3],
		getequalizerbase, /* складывается с -12 и отображается */
	},
	{
		QLABEL("EQUA 5.3"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, EQUALIZERBASE * 2,
		offsetof(struct nvmap, gmikeequalizerparams [4]),
		nvramoffs0,
		NULL,
		& gmikeequalizerparams [4],
		getequalizerbase, /* складывается с -12 и отображается */
	},
	#endif /* WITHAFCODEC1HAVEPROC */
#if WITHAFEQUALIZER
	{
		QLABEL2("RX EQ   ", "RX Equalizer"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, geqrx),
		nvramoffs0,
		NULL,
		& geqrx,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL2("RX 0.4k ", "RX EQ 400 Hz"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, AF_EQUALIZER_BASE * 2,
		offsetof(struct nvmap, geqrxparams [0]),
		nvramoffs0,
		NULL,
		& geqrxparams [0],
		hamradio_get_af_equalizer_base,
	},
	{
		QLABEL2("RX 1.5k ", "RX EQ 1500 Hz"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, AF_EQUALIZER_BASE * 2,
		offsetof(struct nvmap, geqrxparams [1]),
		nvramoffs0,
		NULL,
		& geqrxparams [1],
		hamradio_get_af_equalizer_base,
	},
	{
		QLABEL2("RX 2.7k ", "RX EQ 2700 Hz"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, AF_EQUALIZER_BASE * 2,
		offsetof(struct nvmap, geqrxparams [2]),
		nvramoffs0,
		NULL,
		& geqrxparams [2],
		hamradio_get_af_equalizer_base,
	},
	{
		QLABEL2("TX EQ   ", "TX Equalizer"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, geqtx),
		nvramoffs0,
		NULL,
		& geqtx,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL2("TX 0.4k ", "TX EQ 400 Hz"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, AF_EQUALIZER_BASE * 2,
		offsetof(struct nvmap, geqtxparams [0]),
		nvramoffs0,
		NULL,
		& geqtxparams [0],
		hamradio_get_af_equalizer_base,
	},
	{
		QLABEL2("TX 1.5k ", "TX EQ 1500 Hz"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, AF_EQUALIZER_BASE * 2,
		offsetof(struct nvmap, geqtxparams [1]),
		nvramoffs0,
		NULL,
		& geqtxparams [1],
		hamradio_get_af_equalizer_base,
	},
	{
		QLABEL2("TX 2.7k ", "TX EQ 2700 Hz"), 2 + WSIGNFLAG, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, AF_EQUALIZER_BASE * 2,
		offsetof(struct nvmap, geqtxparams [2]),
		nvramoffs0,
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
	{
		QLABEL("ALC NCEN"), 7, 0, RJ_ON,	ISTEP1,		/* ALC noise gate function control bit. */
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, ALCNEN),	/* ALC noise gate function control bit */
		nvramoffs0,
		NULL,
		& ALCNEN,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("ALCNTH  "), 7, 0, 0,	ISTEP1,		/* ALC noise gate threshold level. */
		ITEM_VALUE,
		0, 7,
		offsetof(struct nvmap, ALCNTH),	/* ALC noise gate threshold level */
		nvramoffs0,
		NULL,
		& ALCNTH,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("ALC EN  "), 7, 0, RJ_ON,	ISTEP1,		/* ALC enabled. */
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, ALCEN),	/* ALC enabled */
		nvramoffs0,
		NULL,
		& ALCEN,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("ALCMXGAN"), 7, 0, 0,	ISTEP1,		/* Set maximum gain limit for PGA volume setting changes under ALC control. */
		ITEM_VALUE,
		0, 7,
		offsetof(struct nvmap, ALCMXGAIN),	/* Set maximum gain limit for PGA volume setting changes under ALC control */
		nvramoffs0,
		NULL,
		& ALCMXGAIN,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("ALCMNGAN"), 7, 0, 0,	ISTEP1,		/* Set minimum gain value limit for PGA volume setting changes under ALC contro. */
		ITEM_VALUE,
		0, 7,
		offsetof(struct nvmap, ALCMNGAIN),	/* Set minimum gain value limit for PGA volume setting changes under ALC contro */
		nvramoffs0,
		NULL,
		& ALCMNGAIN,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_NAU8822L) */
#if WITHUSEAUDIOREC
	{
		QLABEL("SD RECRD"), 8, 3, RJ_ON,	ISTEP1,		/* автоматически начинаем запись на SD CARD при включении */
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, recmode),
		nvramoffs0,
		NULL,
		& recmode,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHUSEAUDIOREC */
#if WITHUSBHW && (WITHUSBUACOUT || WITHUSBUACIN)
#if ! WITHFLATMENU
	{
		QLABEL("USB     "), 0, 0, 0, 0,
		ITEM_GROUP,
		0, 0,
		offsetof(struct nvmap, ggrpusb),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
#if WITHIF4DSP
#if WITHUSBUAC
#if WITHTX
	{
		QLABEL("USB DATA"), 8, 3, RJ_ON,	ISTEP1,		/* автоматическое изменение источника при появлении звука со стороны компьютера */
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, gdatatx),
		nvramoffs0,
		NULL,
		& gdatatx,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHTX */
	{
		QLABEL("PLAY USB"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 					/* режим прослушивания выхода компьютера в наушниках трансивера - отладочный режим */
		offsetof(struct nvmap, guacplayer),
		nvramoffs0,
		NULL,
		& guacplayer,
		getzerobase, /* складывается со смещением и отображается */
	},
	#if WITHRTS96 || WITHRTS192 || WITHTRANSPARENTIQ
	{
		QLABEL("I/Q SWAP"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 					/* Поменять местами I и Q сэмплы в потоке RTS96 */
		offsetof(struct nvmap, gswapiq),
		nvramoffs0,
		NULL,
		& gswapiq,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("FT8CN   "), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 					/* совместимость VID/PID для работы с программой FT8CN */
		offsetof(struct nvmap, gusb_ft8cn),
		nvramoffs0,
		NULL,
		& gusb_ft8cn,
		getzerobase, /* складывается со смещением и отображается */
	},
	#endif /* WITHRTS96 || WITHRTS192 || WITHTRANSPARENTIQ */
	{
		QLABEL2("FT8BOOST", "FT8 Boost"),	7, 2, 0,	ISTEP1,		/* Увеличение усиления при передаче в цифровых режимах 90..300% */
		ITEM_VALUE,
		90, 300,
		offsetof(struct nvmap, ggaindigitx),
		nvramoffs0,
		& ggaindigitx,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHUSBUAC */
#endif /* WITHIF4DSP */
#endif /* WITHUSBHW && (WITHUSBUACOUT || WITHUSBUACIN) */
#if WITHIF4DSP
#if ! WITHFLATMENU
	{
		QLABEL("AGC     "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpagc),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		QLABEL("ADC FS  "), 3 + WSIGNFLAG, 1, 0,	ISTEP1,		/* Калиьровка S-метра - момент перегрузки */
		ITEM_VALUE,
		0, FSADCPOWEROFFSET10 * 2, 		// -50..+50 dBm
		offsetof(struct nvmap, gfsadcpower10 [0]),
		nvramoffs0,
		& gfsadcpower10 [0],	// 16 bit
		NULL,
		getfsasdcbase10, /* складывается со смещением и отображается */
	},
	{
		QLABEL("ADC FSXV"), 3 + WSIGNFLAG, 1, 0,	ISTEP1,		/* с колнвертором Калиьровка S-метра - момент перегрузки */
		ITEM_VALUE,
		0, FSADCPOWEROFFSET10 * 2, 		// -50..+50 dBm
		offsetof(struct nvmap, gfsadcpower10 [1]),
		nvramoffs0,
		& gfsadcpower10 [1],	// 16 bit
		NULL,
		getfsasdcbase10, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AGC OFF "), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,	
		0, 1, 					// предусилитель сигнала с микрофона
		offsetof(struct nvmap, gagcoff),
		nvramoffs0,
		NULL,
		& gagcoff,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("IFGN MAX"), 7, 0, 0,	ISTEP1,		/* диапазон ручной (или автоматической) регулировки цифрового усиления - максимальное значение */
		ITEM_VALUE,
		40, 120, 		// 40..120 dB
		offsetof(struct nvmap, gdigigainmax),
		nvramoffs0,
		NULL,
		& gdigigainmax,	// 8 bit
		getzerobase, /* складывается со смещением и отображается */
	},
#if ! WITHFLATMENU
	{
		QLABEL("AGC SSB "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpagcssb),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		QLABEL("AGC RATE"), 7, 0, 0,	ISTEP1,		/* подстройка параметра АРУ через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		1, AGC_RATE_FLAT,
		offsetof(struct nvmap, afsets [AGCSETI_SSB].rate),	/* На N децибел изменения входного сигнала происходит 1 дБ выходного */
		nvramoffs0,
		NULL,
		& gagc [AGCSETI_SSB].rate,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AGC HUNG"), 6, 1, 0,	ISTEP1,		/* подстройка параметра АРУ (время удержания медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_SSB].thung10),	/* время удержания медленной цепи АРУ */
		nvramoffs0,
		NULL,
		& gagc [AGCSETI_SSB].thung10,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AGC T1  "), 7, 0, 0,	ISTEP10,		/* подстройка параметра АРУ (время срабатывания медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		10, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_SSB].t1),	/* время срабатывания медленной цепи АРУ */
		nvramoffs0,
		NULL,
		& gagc [AGCSETI_SSB].t1,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AGC T2  "), 6, 1, 0,	ISTEP1,		/* подстройка параметра АРУ (время разряда медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		1, 100, 
		offsetof(struct nvmap, afsets [AGCSETI_SSB].release10),	/* время разряда медленной цепи АРУ */
		nvramoffs0,
		NULL,
		& gagc [AGCSETI_SSB].release10,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AGC T4  "), 7, 0, 0,	ISTEP10,		/* подстройка параметра АРУ (время разряда быстрой цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		10, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_SSB].t4),	/* время разряда быстрой цепи АРУ */
		nvramoffs0,
		NULL,
		& gagc [AGCSETI_SSB].t4,
		getzerobase, /* складывается со смещением и отображается */
	},
#if ! WITHFLATMENU
	{
		QLABEL("AGC CW  "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpagccw),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		QLABEL("AGC RATE"), 7, 0, 0,	ISTEP1,		/* подстройка параметра АРУ через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		1, AGC_RATE_FLAT,
		offsetof(struct nvmap, afsets [AGCSETI_CW].rate),	/* На N децибел изменения входного сигнала происходит 1 дБ выходного */
		nvramoffs0,
		NULL,
		& gagc [AGCSETI_CW].rate,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AGC HUNG"), 6, 1, 0,	ISTEP1,		/* подстройка параметра АРУ (время удержания медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_CW].thung10),	/* время удержания медленной цепи АРУ */
		nvramoffs0,
		NULL,
		& gagc [AGCSETI_CW].thung10,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AGC T1  "), 7, 0, 0,	ISTEP10,		/* подстройка параметра АРУ (время срабатывания медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		10, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_CW].t1),	/* время срабатывания медленной цепи АРУ */
		nvramoffs0,
		NULL,
		& gagc [AGCSETI_CW].t1,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AGC T2  "), 6, 1, 0,	ISTEP1,		/* подстройка параметра АРУ (время разряда медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		1, 100, 
		offsetof(struct nvmap, afsets [AGCSETI_CW].release10),	/* время разряда медленной цепи АРУ */
		nvramoffs0,
		NULL,
		& gagc [AGCSETI_CW].release10,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AGC T4  "), 7, 0, 0,	ISTEP10,		/* подстройка параметра АРУ (время разряда быстрой цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		10, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_CW].t4),	/* время разряда быстрой цепи АРУ */
		nvramoffs0,
		NULL,
		& gagc [AGCSETI_CW].t4,
		getzerobase, /* складывается со смещением и отображается */
	},
#if ! WITHFLATMENU
	{
		QLABEL("AGC DIGI"), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpagcdigi),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		QLABEL("AGC RATE"), 7, 0, 0,	ISTEP1,		/* подстройка параметра АРУ через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		1, AGC_RATE_FLAT,
		offsetof(struct nvmap, afsets [AGCSETI_DIGI].rate),	/* На N децибел изменения входного сигнала происходит 1 дБ выходного */
		nvramoffs0,
		NULL,
		& gagc [AGCSETI_DIGI].rate,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AGC HUNG"), 6, 1, 0,	ISTEP1,		/* подстройка параметра АРУ (время удержания медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_DIGI].thung10),	/* время удержания медленной цепи АРУ */
		nvramoffs0,
		NULL,
		& gagc [AGCSETI_DIGI].thung10,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AGC T1  "), 7, 0, 0,	ISTEP10,		/* подстройка параметра АРУ (время срабатывания медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		10, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_DIGI].t1),	/* время срабатывания медленной цепи АРУ */
		nvramoffs0,
		NULL,
		& gagc [AGCSETI_DIGI].t1,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AGC T2  "), 6, 1, 0,	ISTEP1,		/* подстройка параметра АРУ (время разряда медленной цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		1, 100, 
		offsetof(struct nvmap, afsets [AGCSETI_DIGI].release10),	/* время разряда медленной цепи АРУ */
		nvramoffs0,
		NULL,
		& gagc [AGCSETI_DIGI].release10,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("AGC T4  "), 7, 0, 0,	ISTEP10,		/* подстройка параметра АРУ (время разряда быстрой цепи) через меню. */
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		10, 250, 
		offsetof(struct nvmap, afsets [AGCSETI_DIGI].t4),	/* время разряда быстрой цепи АРУ */
		nvramoffs0,
		NULL,
		& gagc [AGCSETI_DIGI].t4,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHIF4DSP */
#if WITHMODEM
#if ! WITHFLATMENU
	{
		QLABEL("MODEM   "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrpmodem),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		QLABEL("DATA MOD"), 5, 0, RJ_MDMMODE, 	ISTEP1,
		ITEM_VALUE,
		0, 1,			/* 0: BPSK, 1: QPSK */
		offsetof(struct nvmap, gmodemmode),
		nvramoffs0,
		NULL,
		& gmodemmode,
		getzerobase, 
	},
	{
		QLABEL("DATA SPD"), 7, 2, RJ_MDMSPEED, 	ISTEP1,
		ITEM_VALUE,
		0, (sizeof modembr2int100 / sizeof modembr2int100 [0]) - 1, 
		offsetof(struct nvmap, gmodemspeed),
		nvramoffs0,
		NULL,
		& gmodemspeed,
		getzerobase, 
	},
#endif /* WITHMODEM */
#if WITHLFM
#if ! WITHFLATMENU
	{
		QLABEL("LFM     "), 0, 0, 0, 0,
		ITEM_GROUP, 
		0, 0, 
		offsetof(struct nvmap, ggrplfm),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		QLABEL("LFM MODE"), 8, 3, RJ_ON, 	ISTEP1,
		ITEM_VALUE,
		0, 1,			/* LFM mode enable */
		offsetof(struct nvmap, lfmmode),
		nvramoffs0,
		NULL,
		& lfmmode,
		getzerobase, 
	},
	{
		QLABEL("LFM STRT"), 5, 1, 0, 	ISTEP1,
		ITEM_VALUE,
		10, 300,			/* 10.0 MHz.. 30.0 MHz in 100 kHz steps */
		offsetof(struct nvmap, lfmstart100k),
		nvramoffs0,
		& lfmstart100k,
		NULL,
		getzerobase, 
	},
	{
		QLABEL("LFM STOP"), 5, 1, 0, 	ISTEP1,
		ITEM_VALUE,
		10, 300,			/* 0.0 MHz.. 30.0 MHz in 100 kHz steps */
		offsetof(struct nvmap, lfmstop100k),
		nvramoffs0,
		& lfmstop100k,
		NULL,
		getzerobase, 
	},
	{
		QLABEL("LFM SPD "), 5, 0, 0, 	ISTEP1,
		ITEM_VALUE,
		50, 550,			/* 50 kHz/sec..550 kHz/sec, 1 kHz/sec steps */
		offsetof(struct nvmap, lfmspeed1k),
		nvramoffs0,
		& lfmspeed1k,
		NULL,
		getzerobase, 
	},
	// Секунды от начала часа до запуска
	{
		QLABEL("LFM OFST"), 5, 0, 0, 	ISTEP1,
		ITEM_VALUE,
		0, 60 * 60 - 1,			/* 0..59:59 */
		offsetof(struct nvmap, lfmtoffset),
		nvramoffs0,
		& lfmtoffset,
		NULL,
		getzerobase, 
	},
	// Интервал в секундах между запусками в пределах часа
	{
		QLABEL("LFM PERI"), 5, 0, 0, 	ISTEP1,
		ITEM_VALUE,
		1, 60 * 60 - 1,			/* 00:01..59:59 */
		offsetof(struct nvmap, lfmtinterval),
		nvramoffs0,
		& lfmtinterval,
		NULL,
		getzerobase,
	},
    {
        QLABEL("LFM OFFS"), 5 + WSIGNFLAG, 0, 0,     ISTEP1,
        ITEM_VALUE,
        0, 2 * LFMFREQBIAS,            /*  */
        offsetof(struct nvmap, lfmfreqbias),
        nvramoffs0,
        & lfmfreqbias,
        NULL,
        getlfmbias,
    },
#endif /* WITHLFM */

#if WITHTX
/* settings page header */
#if ! WITHFLATMENU
	{
		QLABEL("TX ADJ  "), 0, 0, 0, 0,
		ITEM_GROUP,
		0, 0,
		offsetof(struct nvmap, ggrptxadj),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */

/* settings page list */
#if WITHIF4DSP
	{
		QLABEL("DACSCALE"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gdacscale),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gdacscale,
		getzerobase, /* складывается со смещением и отображается */
	},

	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("HP MW/LW"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_b [0]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [0].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("HP 160m "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_b [1]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [1].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("HP 80m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_b [2]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [2].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("HP 40m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_b [3]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [3].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("HP 30m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_b [4]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [4].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("HP 20m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_b [5]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [5].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("HP 17m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_b [6]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [6].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("HP 15m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_b [7]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [7].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("HP 12m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_b [8]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [8].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("HP 10m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_b [9]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [9].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("HP 6m   "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_b [10]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [10].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("HP 2m   "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_b [11]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [11].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("HP 0.7m "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_b [12]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [12].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("HP ACC13"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_b [13]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [13].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("HP ACC14"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_b [14]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [14].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("HP ACC15"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_b [15]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [15].adj_b,
		getzerobase, /* складывается со смещением и отображается */
	},

	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("LP MW/LW"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_a [0]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [0].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("LP 160m "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_a [1]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [1].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("LP 80m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_a [2]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [2].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("LP 40m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_a [3]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [3].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("LP 30m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_a [4]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [4].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("LP 20m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_a [5]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [5].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("LP 17m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_a [6]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [6].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("LP 15m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_a [7]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [7].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("LP 12m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_a [8]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [8].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("LP 10m  "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_a [9]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [9].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("LP 6m   "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_a [10]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [10].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("LP 2m   "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_a [11]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [11].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("LP 0.7m "), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_a [12]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [12].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("LP ACC13"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_a [13]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [13].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("LP ACC14"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_a [14]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [14].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},
	// gbandf2adj [NUMLPFADJ]
	{
		QLABEL("LP ACC15"), 7, 0, 0,	ISTEP1,		/* Подстройка амплитуды сигнала с ЦАП передатчика */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gbandf2adj_a [15]),	/* Амплитуда сигнала с ЦАП передатчика - 0..100% */
		nvramoffs0,
		NULL,
		& gbandf2adj [15].adj_a,
		getzerobase, /* складывается со смещением и отображается */
	},

	{
		QLABEL("AM DEPTH"), 7, 0, 0,	ISTEP1,		/* Подстройка глубины модуляции в АМ */
		ITEM_VALUE,
		0, 100,
		offsetof(struct nvmap, gamdepth),	/* Глубина модуляции в АМ - 0..100% */
		nvramoffs0,
		NULL,
		& gamdepth,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("NFM DEVI"), 7, 1, 0,	ISTEP1,		/* Подстройка девиации на передачу */
		ITEM_VALUE,
		0, 120,
		offsetof(struct nvmap, gnfmdeviation),	/* девиация в сотнях герц */
		nvramoffs0,
		NULL,
		& gnfmdeviation,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("CW BOOST"),	7, 2, 0,	ISTEP1,		/* Увеличение усиления при передаче в цифровых режимах 90..300% */
		ITEM_VALUE,
		30, 100,
		offsetof(struct nvmap, ggaincwtx),
		nvramoffs0,
		& ggaincwtx,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},

#endif /* WITHIF4DSP */
#if WITHFANTIMER
	{
		QLABEL("FAN TIME"), 7, 0, 0,	ISTEP5,
		ITEM_VALUE,
		0, FANPATIMEMAX,
		offsetof(struct nvmap, gfanpatime),
		nvramoffs0,
		NULL,
		& gfanpatime,
		getzerobase, /* складывается со смещением и отображается */
	},
	#if WITHFANPWM
	{
		QLABEL("FAN PWM "), 7, 0, 0,	ISTEP5,
		ITEM_VALUE,
		0, 255,
		offsetof(struct nvmap, gfanpapwm),
		nvramoffs0,
		& gfanpapwm,
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
	#endif /* WITHFANPWM */
#endif /* WITHFANTIMER */

#if WITHPOWERTRIM
  #if ! WITHPOTPOWER
	{
		QLABEL("TX POWER"), 7, 0, 0,	ISTEP5,		/* мощность при обычной работе на передачу */
		ITEM_VALUE,
		WITHPOWERTRIMMIN, WITHPOWERTRIMMAX,
		offsetof(struct nvmap, gnormalpower),
		nvramoffs0,
		NULL,
		& gnormalpower.value,
		getzerobase,
	},
	#if WITHPACLASSA
		/* усилитель мощности поддерживает переключение в класс А */
		{
			QLABEL2("CLASSA  ", "Class A"), 7, 0, RJ_ON,	ISTEP1,		/* использование режима клвсс А при передаче */
			ITEM_VALUE,
			0, 1,
			offsetof(struct nvmap, gclassamode),
			nvramoffs0,
			NULL,
			& gclassamode,
			getzerobase,
		},
		{
			QLABEL2("CLASSA P", "Class A Pwr"), 7, 0, 0,	ISTEP1,		/* мощность при обычной работе на передачу */
			ITEM_VALUE,
			WITHPOWERTRIMMIN, WITHPOWERTRIMMAX,
			offsetof(struct nvmap, gclassapower),
			nvramoffs0,
			NULL,
			& gclassapower,
			getzerobase,
		},
	#endif /* WITHPACLASSA */
  #endif /* ! WITHPOTPOWER */
#elif WITHPOWERLPHP
	#if ! CTLSTYLE_SW2011ALL
	{
		QLABEL("TX POWER"), 7, 0, RJ_POWER,	ISTEP1,		/* мощность при обычной работе на передачу */
		ITEM_VALUE,
		0, PWRMODE_COUNT - 1,
		offsetof(struct nvmap, gpwri),
		nvramoffs0,
		NULL,
		& gpwri,
		getzerobase,
	},
	#endif /* ! CTLSTYLE_SW2011ALL */
#endif /* WITHPOWERTRIM */

#if ! CTLSTYLE_SW2011ALL
	{
		QLABEL("TX GATE "), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, gtxgate),
		nvramoffs0,
		NULL,
		& gtxgate,
		getzerobase, 
	},
#endif /* ! CTLSTYLE_SW2011ALL */

#if WITHPABIASTRIM
	{
		QLABEL("PA BIAS "), 7, 0, 0,	ISTEP1,		/* регулировка тока покоя оконечного каскада передатчика */
		ITEM_VALUE,
		WITHPABIASMIN, WITHPABIASMAX,
		offsetof(struct nvmap, gpabias),
		nvramoffs0,
		NULL,
		& gpabias,
		getzerobase, 
	},
#endif /* WITHPABIASTRIM && WITHTX */
#if WITHDSPEXTDDC	/* QLABEL("ВоронёнокQLABEL(" с DSP и FPGA */
	{
		QLABEL("DAC TEST"), 8, 3, RJ_ON,	ISTEP1,	/*  */
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, gdactest),
		nvramoffs0,
		NULL,
		& gdactest,
		getzerobase,
	},
#endif /* WITHDSPEXTDDC */

#endif /* WITHTX */

/* settings page header */
#if ! WITHFLATMENU
	{
		QLABEL("SPECIAL "), 0, 0, 0, 0,
		ITEM_GROUP,
		0, 0,
		offsetof(struct nvmap, ggrpsecial),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */

/* settings page list */
#if WITHRFSG
	{
		QLABEL("RFSG MOD"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, userfsg),
		nvramoffs0,
		NULL,
		& userfsg,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHRFSG */
#if WITHENCODER
	{
		QLABEL("ENC1 RES"), 7, 0, RJ_ENCRES,	ISTEP1,
		ITEM_VALUE,
		0, (sizeof encresols / sizeof encresols [0]) - 1,
		offsetof(struct nvmap, genc1pulses),
		nvramoffs0,
		NULL,
		& genc1pulses,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("ENC1 DYN"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, genc1dynamic),
		nvramoffs0,
		NULL,
		& genc1dynamic,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("ENC1 DIV"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		1, 128, 	/* /1 ... /128 */
		offsetof(struct nvmap, genc1div),
		nvramoffs0,
		NULL,
		& genc1div,
		getzerobase,
	},
	{
		QLABEL("BIG STEP"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, gbigstep),
		nvramoffs0,
		NULL,
		& gbigstep,
		getzerobase,
	},
#if WITHENCODER2
	{
		QLABEL("ENC2 DIV"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		1, 8, 	/* /1 ... /8 */
		offsetof(struct nvmap, genc2div),
		nvramoffs0,
		NULL,
		& genc2div,
		getzerobase,
	},
#endif /* WITHENCODER2 */
#endif /* WITHENCODER */

#if WITHTX
#if WITHIF4DSP
	{
		QLABEL("NFM GAIN"), 7, 1, 0,	ISTEP1,		/* дополнительное усиление по НЧ в режиме приёма NFM 100..1000% */
		ITEM_VALUE,
		10, 100,
		offsetof(struct nvmap, ggainnfmrx10),	/* дополнительное усиление по НЧ в режиме приёма NFM 100..1000% */
		nvramoffs0,
		NULL,
		& ggainnfmrx10,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHIF4DSP */
#if WITHIF4DSP
	{
		QLABEL2("RGR BERP", "Roger Beep"), 7, 0, RJ_ON,	ISTEP5,	/* разрешение (не-0) или запрещение (0) формирования roger beep */
		ITEM_VALUE,
		0, 1,						/* разрешение (не-0) или запрещение (0) формирования roger beep */
		offsetof(struct nvmap, grgbeep),
		nvramoffs0,
		NULL,
		& grgbeep,
		getzerobase,
	},
#endif /* WITHIF4DSP */
//	{
//		QLABEL("REPT HF "), 5 + WSIGNFLAG, 0, 0, 	ISTEP1,
//		ITEM_VALUE,
//		RPTOFFSMIN, RPTOFFSMAX,		/* repeater offset */
//		offsetof(struct nvmap, rptroffshf1k),
//		nvramoffs0,
//		& rptroffshf1k,
//		NULL,
//		getrptoffsbase,
//	},
//	{
//		QLABEL("REPT UHF"), 5 + WSIGNFLAG, 0, 0, 	ISTEP1,
//		ITEM_VALUE,
//		RPTOFFSMIN, RPTOFFSMAX,		/* repeater offset */
//		offsetof(struct nvmap, rptroffsuhf1k),
//		nvramoffs0,
//		& rptroffsuhf1k,
//		NULL,
//		getrptoffsbase,
//	},
#endif /* WITHTX */

#if defined(REFERENCE_FREQ)
#if defined (DAC1_TYPE)
	{
		QLABEL("REF ADJ "), 7, 0, 0,	ISTEP1,		/* подстройка частоты опорного генератора (напряжением) через меню. */
		ITEM_VALUE,
		WITHDAC1VALMIN, WITHDAC1VALMAX, 
		offsetof(struct nvmap, dac1level),
		nvramoffs0,
		NULL,	/* подстройка опорника */
		& dac1level,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* defined (DAC1_TYPE) */
	{
		QLABEL("REF FREQ"), 9, 3, 0,	ISTEP1,		/* ввод реальной частоты опорного генератора через меню. */
		ITEM_VALUE,
		0, OSCSHIFT * 2 - 1, 
		offsetof(struct nvmap, refbias),
		nvramoffs0,
		& refbias,	/* подстройка частоты опорника */
		NULL,
		getrefbase, 	/* складывается со смещением и отображается */
	},
#endif	/* defined(REFERENCE_FREQ) */
#if (LO3_SIDE != LOCODE_INVALID) && LO3_FREQADJ	/* подстройка частоты гетеродина через меню. */
	{
		QLABEL("LO3 FRQ "), 9, 3, 0,	ISTEP1,
		ITEM_VALUE,
		LO2AMIN, LO2AMAX, 
		offsetof(struct nvmap, lo3offset),
		nvramoffs0,
		& lo3offset,	/* подстройка частоты гетеродина */
		NULL,
		getlo3base, 	/* складывается со смещением и отображается */
	},
#endif	/* (LO3_SIDE != LOCODE_INVALID) && LO3_FREQADJ */
#if defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570)
	{
		QLABEL("REFSI570"), 9, 3, 0,	ISTEP1,
		ITEM_VALUE,
		0, OSCSHIFT * 2 - 1, 
		offsetof(struct nvmap, si570_xtall_offset),
		nvramoffs0,
		& si570_xtall_offset,	/* подстройка опорника */
		NULL,
		si570_get_xtall_base, 	/* складывается со смещением и отображается */
	},
#endif /* defined(PLL1_TYPE) && (PLL1_TYPE == PLL_TYPE_SI570) */
#if WITHONLYBANDS
	{
		QLABEL("BANDONLY"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, withonlybands),
		nvramoffs0,
		NULL,
		& withonlybands,
		getzerobase, 
	},
#endif /* WITHONLYBANDS */
	{
		QLABEL("STAYFREQ"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, stayfreq),
		nvramoffs0,
		NULL,
		& stayfreq,
		getzerobase, 
	},
#if WITHVOLTLEVEL && ! WITHREFSENSOR
	{
		QLABEL("BAT CALI"), 7, 1, 0,	ISTEP1,			/* калибровочный параметр делителя напряжения АКБ */
		ITEM_VALUE,
		ADCVREF_CPU, 255,	// 3.3/5.0 .. 25.5 вольта
		offsetof(struct nvmap, voltcalibr100mV),
		nvramoffs0,
		NULL,
		& voltcalibr100mV,
		getzerobase,
	},
#endif /* WITHVOLTLEVEL && ! WITHREFSENSOR */
#if (WITHCURRLEVEL || WITHCURRLEVEL2)
	{
		QLABEL("IPA CALI"), 5 + WSIGNFLAG, 2, 0,	ISTEP1,			/* калибровочный параметр делителя напряжения АКБ */
		ITEM_VALUE,
		0, IPACALI_RANGE,
		offsetof(struct nvmap, gipacali),
		nvramoffs0,
		& gipacali,
		NULL,
		getipacalibase,
	},
#endif /* (WITHCURRLEVEL || WITHCURRLEVEL2) */
#if WITHTX
#if WITHSWRMTR && ! WITHSHOWSWRPWR
	{
		QLABEL("SWR SHOW"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, swrmode),
		nvramoffs0,
		NULL,
		& swrmode,
		getzerobase, 
	},
#endif /* WITHSWRMTR && ! WITHSHOWSWRPWR */
#if (WITHSWRMTR || WITHSHOWSWRPWR)
	{
		QLABEL("SWR CALI"), 7, 2, 0,	ISTEP1,		/* калибровка SWR-метра */
		ITEM_VALUE,
		50, 200, //80, 120, 
		offsetof(struct nvmap, swrcalibr),
		nvramoffs0,
		NULL,
		& swrcalibr,
		getzerobase, 
	},
	{
		QLABEL("FWD LOWR"), 7, 0, 0,	ISTEP1,		/* нечувствительность SWR-метра */
		ITEM_VALUE,
		1, (1U << HARDWARE_ADCBITS) - 1, 
		offsetof(struct nvmap, minforward),
		nvramoffs0,
		& minforward,
		NULL,
		getzerobase, 
	},
	{
		QLABEL("PWR CALI"), 7, 0, 0,	ISTEP1,		/* калибровка PWR-метра */
		ITEM_VALUE,
		1, 255,
		offsetof(struct nvmap, maxpwrcali),
		nvramoffs0,
		NULL,
		& maxpwrcali,
		getzerobase,
	},
	{
		QLABEL("SWR PROT"), 7, 0, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,						/* защита от превышения КСВ */
		offsetof(struct nvmap, gswrprot),
		nvramoffs0,
		NULL,
		& gswrprot,
		getzerobase,
	},
	
#elif WITHPWRMTR
	{
		QLABEL("PWR CALI"), 7, 0, 0,	ISTEP1,		/* калибровка PWR-метра */
		ITEM_VALUE,
		10, 255,
		offsetof(struct nvmap, maxpwrcali),
		nvramoffs0,
		NULL,
		& maxpwrcali,
		getzerobase,
	},
#endif
#if WITHTHERMOLEVEL
	{
		QLABEL("HEAT LIM"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		20, 85,						/* порог срабатывания защиты по температуре */
		offsetof(struct nvmap, gtempvmax),
		nvramoffs0,
		NULL,
		& gtempvmax,
		getzerobase,
	},
	{
		QLABEL("HEATPROT"), 7, 0, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1,						/* защита от перегрева */
		offsetof(struct nvmap, gheatprot),
		nvramoffs0,
		NULL,
		& gheatprot,
		getzerobase,
	},
#endif /* WITHTHERMOLEVEL */
#if WITHIF4DSP || defined (TXPATH_BIT_ENABLE_SSB) || defined (TXPATH_BIT_ENABLE_CW) || defined (TXPATH_BIT_GATE)
	{
		QLABEL("RXTX DLY"), 7, 0, 0,	ISTEP5,	/* 5 mS step of changing value */
		ITEM_VALUE,
		5, WITHMAXRXTXDELAY,						/* 5..100 ms delay */
		offsetof(struct nvmap, rxtxdelay),
		nvramoffs0,
		NULL,
		& rxtxdelay,
		getzerobase, 
	},
	{
		QLABEL("TXRX DLY"), 7, 0, 0,	ISTEP5,	/* 5 mS step of changing value */
		ITEM_VALUE,
		5, WITHMAXTXRXDELAY,						/* 5..100 ms delay */
		offsetof(struct nvmap, txrxdelay),
		nvramoffs0,
		NULL,
		& txrxdelay,
		getzerobase, 
	},
#endif /* GATEs */
#endif /* WITHTX */
#if WITHLO1LEVELADJ
	{
		QLABEL("LO1 LEVL"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, 100, 		/* уровень (амплитуда) LO1 в процентах */
		offsetof(struct nvmap, lo1level),
		nvramoffs0,
		NULL,
		& lo1level,
		getzerobase, 
	},
#endif /* WITHLO1LEVELADJ */
#if LO1PHASES
	{
		QLABEL("PHASE RX"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, 65535, /* добавление к коду смещения фазы */
		offsetof(struct nvmap, phaserx),
		nvramoffs0,
		& phasesmap [0],
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
#if WITHTX
	{
		QLABEL("PHASE TX"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		0, 65535, /* добавление к коду смещения фазы */
		offsetof(struct nvmap, phasetx),
		nvramoffs0,
		& phasesmap [1],
		NULL,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHTX */
#endif /* LO1PHASES */

#if LO1MODE_HYBRID
	{
		QLABEL("ALIGN MD"), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, alignmode),
		nvramoffs0,
		NULL,
		& alignmode,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif	/* LO1MODE_HYBRID */

#if LO1FDIV_ADJ
	{
		QLABEL("LO1DV RX"), 7, 0, RJ_POW2,	ISTEP1,
		ITEM_VALUE,
		0, 3, 	/* Output of synthesizer multiplied to 1, 2, 4 or 8 */
		offsetof(struct nvmap, lo1powrx),
		nvramoffs0,
		NULL,
		& lo1powmap [0],
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("LO1DV TX"), 7, 0, RJ_POW2,	ISTEP1,
		ITEM_VALUE,
		0, 3, 	/* Output of synthesizer multiplied to 1, 2, 4 or 8 */
		offsetof(struct nvmap, lo1powtx),
		nvramoffs0,
		NULL,
		& lo1powmap [1],
		getzerobase, /* складывается со смещением и отображается */
	},
#endif
#if LO4FDIV_ADJ
	{
		QLABEL("LO4DV RX"), 7, 0, RJ_POW2,	ISTEP1,
		ITEM_VALUE,
		0, 3, 	/* Output of synthesizer multiplied to 1, 2, 4 or 8 */
		offsetof(struct nvmap, lo4powrx),
		nvramoffs0,
		NULL,
		& lo4powmap [0],
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("LO4DV TX"), 7, 0, RJ_POW2,	ISTEP1,
		ITEM_VALUE,
		0, 3, 	/* Output of synthesizer multiplied to 1, 2, 4 or 8 */
		offsetof(struct nvmap, lo4powtx),
		nvramoffs0,
		NULL,
		& lo4powmap [1],
		getzerobase, /* складывается со смещением и отображается */
	},
#endif

#if WITHBARS
	{
		QLABEL("S9 LEVEL"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		1, UINT8_MAX - 1, 
		offsetof(struct nvmap, s9level),
		nvramoffs0,
		NULL,			/* калибровка уровней S-метра */
		& s9level,
		getzerobase, 
	},
	{
		QLABEL("S9 DELTA"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		1, UINT8_MAX - 1, 
		offsetof(struct nvmap, s9delta),
		nvramoffs0,
		NULL,			/* калибровка уровней S-метра */
		& s9delta,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("+60DELTA"), 7, 0, 0,	ISTEP1,
		ITEM_VALUE,
		1, UINT8_MAX - 1, 
		offsetof(struct nvmap, s9_60_delta),
		nvramoffs0,
		NULL,			/* калибровка уровней S-метра */
		& s9_60_delta,
		getzerobase, /* складывается со смещением и отображается */
	},
	{
		QLABEL("ATTPRESH"), 7, 0, RJ_ON,	ISTEP1,	/* attenuator-preamplifier shift */
		ITEM_VALUE,
		0, 1,
		offsetof(struct nvmap, gattpresh),	/* корректировка показаний с-метра по включенному аттенюатору и предусилителю */
		nvramoffs0,
		NULL,
		& gattpresh,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHBARS */

	{
		QLABEL("BAND 27 "), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, bandset11m),
		nvramoffs0,
		NULL,
		& bandset11m,
		getzerobase, /* складывается со смещением и отображается */
	},
#if WITHANTSELECT2
	{
		QLABEL2("HF ANT F", "HF Ant freq"), 3, 0, 0, ISTEP1,
		ITEM_VALUE,
		TUNE_BOTTOM / 1000000, (TUNE_TOP - 1) / 1000000,
		offsetof(struct nvmap, hffreqswitch),
		nvramoffs0,
		NULL,
		& hffreqswitch,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHANTSELECT2 */
#if WITHBCBANDS
	{
		QLABEL("BAND BC "), 7, 3, RJ_YES,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, gbandsetbcast),
		nvramoffs0,
		NULL,
		& gbandsetbcast,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* WITHBCBANDS */

#if CTLSTYLE_SW2011ALL
#if TUNE_6MBAND
	{
		QLABEL("BAND 50 "), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, bandset6m),
		nvramoffs0,
		NULL,
		& bandset6m,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* TUNE_6MBAND */
#if TUNE_4MBAND
	{
		QLABEL("BAND 70 "), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, bandset4m),
		nvramoffs0,
		NULL,
		& bandset4m,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* TUNE_6MBAND */
#if TUNE_2MBAND
	{
		QLABEL("BAND 144"), 8, 3, RJ_ON,	ISTEP1,
		ITEM_VALUE,
		0, 1, 
		offsetof(struct nvmap, bandset2m),
		nvramoffs0,
		NULL,
		& bandset2m,
		getzerobase, /* складывается со смещением и отображается */
	},
#endif /* TUNE_2MBAND */
#endif /* CTLSTYLE_SW2011ALL */

#if ! WITHFLATMENU
	{
		QLABEL2("ABOUT   ", "About"), 0, 0, 0, 0,
		ITEM_GROUP,
		0, 0,
		offsetof(struct nvmap, ggrpabout),
		nvramoffs0,
		NULL,
		NULL,
		NULL,
	},
#endif /* ! WITHFLATMENU */
	{
		QLABEL("VERSION "), 7, 0, RJ_COMPILED, 	ISTEP_RO,	// тип процессора
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, 0,
		MENUNONVRAM,
		nvramoffs0,
		& gzero,
		NULL,
		getzerobase,
	},
	{
		QLABEL("S/N     "), 7, 0, RJ_SERIALNR, 	ISTEP_RO,	// Индивидуальный номер изделия
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, 0,
		MENUNONVRAM,
		nvramoffs0,
		& gzero,
		NULL,
		getzerobase,
	},
	{
		QLABEL("CPU TYPE"), 7, 0, RJ_CPUTYPE, 	ISTEP_RO,	// тип процессора
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, 0,
		MENUNONVRAM,
		nvramoffs0,
		& gzero,
		NULL,
		getzerobase,
	},
	{
		QLABEL("CPU FREQ"), 7, 0, 0, 	ISTEP_RO,	// частота процессора
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, 0,
		MENUNONVRAM,
		nvramoffs0,
		& gzero,
		NULL,
		getcpufreqbase,
	},
#if CPUSTYLE_STM32MP1
	{
		QLABEL("DDR FREQ"), 7, 0, 0, 	ISTEP_RO,	// частота памяти
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, 0,
		MENUNONVRAM,
		nvramoffs0,
		& gzero,
		NULL,
		getddrfreqbase,
	},
	{
		QLABEL("BUS FREQ"), 7, 0, 0, 	ISTEP_RO,	// частота шины
		ITEM_VALUE | ITEM_NOINITNVRAM,	/* значение этого пункта не используется при начальной инициализации NVRAM */
		0, 0,
		MENUNONVRAM,
		nvramoffs0,
		& gzero,
		NULL,
		getaxissfreqbase,
	},
#endif
};
