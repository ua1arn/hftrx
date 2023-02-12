#if 1
// WO240128A
	#define CHAR_W	8
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP
		// x=30, y=16

		enum
		{
			BDTH_ALLRX = 30,	// ширина зоны для отображение полосы на индикаторе
			BDTH_LEFTRX = 15,	// ширина индикатора баллов
			BDTH_RIGHTRX = BDTH_ALLRX - BDTH_LEFTRX,	// ширина индикатора плюсов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 20,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//					  "012345678901234567890123456789"
		#define SMETERMAP 	  "S 1  3  5  7  9  +20  +40  +60"	//
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			#define SWRPWRMAP "1 | 2 | 3 0%      50%     100%"
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		#else
			#error Not designed for work DSTYLE_UR3LMZMOD without WITHSHOWSWRPWR
			//#define POWERMAP  " 0 10 20 40 60 80 100"
			//#define SWRMAP    "1    |    2    |   3 "	//
			//#define SWRMAX	(SWRMIN * 31 / 10)	// 3.1 - значение на полной шкале
		#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x81,
			PATTERN_BAR_EMPTYHALF = 0x81
		};
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
		/* ---------------------------------- */
			{	0,	0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3,	0,	display2_ant5,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	9,	0,	display_hplp2,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	16, 0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// attenuator state
			{	21, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	25,	0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	27, 0,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	2,	display_freqXbig_a, REDRM_FREQ, REDRSUBSET(DPAGE0), },	// fullwidth = 8 constantly
		/* ---------------------------------- */
			//{	0,	8,	display2_mainsub3, REDRM_MODE, REDRSUBSET(DPAGE0), },	// main/sub RX
			{	4,	8,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPL
			{	12, 8,	display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },
			{	27, 8,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	11,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
			{	0,	12,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// Отображение оцифровки шкалы S-метра
			/* ---------------------------------- */
			{	0,	14,	display2_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
	#if WITHNOTCHONOFF || WITHNOTCHFREQ
			// see next {	4,	14,	display2_notch5,		REDRM_MODE, REDRSUBSET(DPAGE0), },
	#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ  */
	#if WITHAUTOTUNER
			{	4,	14,	display2_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	8,	14,	display2_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
	#endif /* WITHAUTOTUNER  */
		/* ---------------------------------- */
	#if WITHVOLTLEVEL
			{	12,	14,	display2_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter with "V"
	#endif /* WITHVOLTLEVEL  */
	#if defined (RTC1_TYPE)
			//{	18,	14,	display_time8,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME
			{	18,	14,	display2_datetime12,	REDRM_BARS, REDRSUBSET(DPAGE0), },	// DATE&TIME Jan-01 13:40
	#endif /* defined (RTC1_TYPE) */
		#if WITHNOTCHONOFF || WITHNOTCHFREQ
			{	27, 14,	display_notch3, REDRM_MODE, REDRSUBSET(DPAGE0), },	// 3.7
		#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
		/* ---------------------------------- */
	#if WITHMENU
			{	1,	0,	display2_multilinemenu_block_groups,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
			{	LABELW + 2,	0,	display2_multilinemenu_block_params,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
			{	LABELW*2 + 3,	0,	display2_multilinemenu_block_vals,	REDRM_MVAL, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
		#if WITHVOLTLEVEL && WITHCURRLEVEL
			{	0,	9,	display2_voltlevelV5, REDRM_VOLT, REDRSUBSET_MENU, },	// voltmeter with "V"
			{	6,	9,	display2_currlevelA6, REDRM_VOLT, REDRSUBSET_MENU, },	// amphermeter with "A"
		#endif /* WITHVOLTLEVEL && WITHCURRLEVEL */
	#endif /* WITHMENU */
		};
	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 8;
			p->ystep = 2;	// количество ячеек разметки на одну строку меню
			p->reverse = 1;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#elif DSTYLE_UR3LMZMOD
		// Version with specreum display
		// x=30, y=16
		enum
		{
			BDCV_ALLRX = 7,			// количество ячееек, отведенное под S-метр, панораму, иные отображения
			BDCV_SPMRX = BDCV_ALLRX,	// вертикальный размер спектра в ячейках		};
			BDCV_WFLRX = BDCV_ALLRX,	// вертикальный размер водопада в ячейках		};
			BDCO_SPMRX = 0,	// смещение спектра по вертикали в ячейках от начала общего поля
			BDCO_WFLRX = 0,	// смещение водопада по вертикали в ячейках от начала общего поля

			BDTH_ALLRX = 30,	// ширина зоны для отображение полосы на индикаторе
			BDTH_LEFTRX = 15,	// ширина индикатора баллов
			BDTH_RIGHTRX = BDTH_ALLRX - BDTH_LEFTRX,	// ширина индикатора плюсов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 20,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//					  "012345678901234567890123456789"
		#define SMETERMAP 	  "S 1  3  5  7  9  +20  +40  +60"	//
		//#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			#define SWRPWRMAP "1 | 2 | 3 0%      50%     100%"
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		//#else
		//	#error Not designed for work DSTYLE_UR3LMZMOD without WITHSHOWSWRPWR
			//#define POWERMAP  " 0 10 20 40 60 80 100"
			//#define SWRMAP    "1    |    2    |   3 "	//
			//#define SWRMAX	(SWRMIN * 31 / 10)	// 3.1 - значение на полной шкале
		//#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x81,
			PATTERN_BAR_EMPTYHALF = 0x81
		};
		//#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DPAGE1,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		enum
		{
			PG0 = REDRSUBSET(DPAGE0),
			PG1 = REDRSUBSET(DPAGE1),
			PGALL = PG0 | PG1 | REDRSUBSET_MENU,
			PGLATCH = PGALL | REDRSUBSET_SLEEP,
			PGINI = REDRSUBSET_INIT,
			PGunused
		};
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
		/* ---------------------------------- */
			{	0,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
			{	3,	0,	display2_ant5,		REDRM_MODE, PGALL, },
			{	9,	0,	display_ovf3,		REDRM_BARS, PGALL, },	// ovf
			{	14,	0,	display_pre3,		REDRM_MODE, PGALL, },	// pre
			{	18, 0,	display2_att4,		REDRM_MODE, PGALL, },	// attenuator state
			{	22,	0,	display_lockstate1, REDRM_MODE, PGALL, },
			{	23, 0,	display2_rxbw3,		REDRM_MODE, PGALL, },
			{	27, 0,	display2_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	2,	display_freqXbig_a, REDRM_FREQ, PGALL, },	// fullwidth = 8 constantly
		#if ! WITHDSPEXTDDC
			{	27, 2,	display2_agc3,		REDRM_MODE, PGALL, },
		#endif /* ! WITHDSPEXTDDC */
			{	27, 4,	display2_voxtune3,	REDRM_MODE, PGALL, },
		/* ---------------------------------- */
		#if WITHUSEAUDIOREC
			{	0,	7,	display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента
		#endif /* WITHUSEAUDIOREC */
			{	4,	7,	display2_mainsub3, REDRM_MODE, PGALL, },	// main/sub RX
			{	8,	7,	display2_vfomode3,	REDRM_MODE, PGALL, },	// SPL
			{	16, 7,	display2_freqX_b,	REDRM_FRQB, PGALL, },
			{	27, 7,	display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	9,	display2_bars,		REDRM_BARS, PG0, },	// S-METER, SWR-METER, POWER-METER
			{	0,	10,	display2_legend,	REDRM_MODE, PG0, },	// Отображение оцифровки шкалы S-метра
			/* ---------------------------------- */
			{	0,	9,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
			{	0,	9,	display2_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
			{	0,	9,	display2_spectrum,	REDRM_BARS, PG1, },// подготовка изображения спектра
			{	0,	9,	display2_colorbuff,	REDRM_BARS,	PG1, },// Отображение водопада и/или спектра
			/* ---------------------------------- */
			{	0,	14,	display_time5,		REDRM_BARS, PG0, },	// TIME
		#if WITHVOLTLEVEL
			{	6,	14,	display2_voltlevelV5, REDRM_VOLT, PG0, },	// voltmeter with "V"
		#endif /* WITHVOLTLEVEL  */
		#if WITHCURRLEVEL || WITHCURRLEVEL2
			{	11, 14,	display2_currlevelA6, REDRM_VOLT, PG0, },	// amphermeter with "A"
		#endif /*  WITHCURRLEVEL || WITHCURRLEVEL2 */
		#if WITHAMHIGHKBDADJ
			{	6, 14,	display_amfmhighcut4,REDRM_MODE, PG0, },	// 3.70
		#endif /* WITHAMHIGHKBDADJ */
			{	19, 14, display_siglevel7, 	REDRM_BARS, PG0, },		// signal level dBm
			//{	18, 14,	display_samfreqdelta8, REDRM_BARS, PG0, },	/* Получить информацию об ошибке настройки в режиме SAM */
		#if WITHNOTCHONOFF || WITHNOTCHFREQ
			{	27, 14,	display_notch3, REDRM_MODE, PG0, },	// 3.7
		#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
		/* ---------------------------------- */
	#if WITHMENU
			{	1,	9,	display2_multilinemenu_block_groups,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
			{	LABELW + 2,	9,	display2_multilinemenu_block_params,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
			{	LABELW*2 + 3,	9,	display2_multilinemenu_block_vals,	REDRM_MVAL, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
		#if WITHVOLTLEVEL && WITHCURRLEVEL
			//{	0,	9,	display2_voltlevelV5, REDRM_VOLT, REDRSUBSET_MENU, },	// voltmeter with "V"
			//{	6,	9,	display2_currlevelA6, REDRM_VOLT, REDRSUBSET_MENU, },	// amphermeter with "A"
		#endif /* WITHVOLTLEVEL && WITHCURRLEVEL */
	#endif /* WITHMENU */
		};
	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 3;
			p->ystep = 2;	// количество ячеек разметки на одну строку меню
			p->reverse = 1;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#else /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */
		// RA1AGO version
		// x=30, y=16

		enum
		{
			BDCV_ALLRX = 7,			// количество ячееек, отведенное под S-метр, панораму, иные отображения
			BDCV_SPMRX = BDCV_ALLRX,	// вертикальный размер спектра в ячейках		};
			BDCV_WFLRX = BDCV_ALLRX,	// вертикальный размер водопада в ячейках		};
			BDCO_SPMRX = 0,	// смещение спектра по вертикали в ячейках от начала общего поля
			BDCO_WFLRX = 0,	// смещение водопада по вертикали в ячейках от начала общего поля
			BDTH_ALLRX = 30,	// ширина зоны для отображение полосы на индикаторе
			BDTH_LEFTRX = 15,	// ширина индикатора баллов
			BDTH_RIGHTRX = BDTH_ALLRX - BDTH_LEFTRX,	// ширина индикатора плюсов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 20,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//					  "012345678901234567890123456789"
		#define SMETERMAP 	  "S 1  3  5  7  9  +20  +40  +60"	//
		//#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			#define SWRPWRMAP "1 | 2 | 3 0%      50%     100%"
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		//#else
		//	#error Not designed for work DSTYLE_UR3LMZMOD without WITHSHOWSWRPWR
			//#define POWERMAP  " 0 10 20 40 60 80 100"
			//#define SWRMAP    "1    |    2    |   3 "	//
			//#define SWRMAX	(SWRMIN * 31 / 10)	// 3.1 - значение на полной шкале
		//#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x81,
			PATTERN_BAR_EMPTYHALF = 0x81
		};
		//#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
		#if WITHDSPEXTDDC
			DPAGE1,					// Страница, в которой отображаются основные (или все)
		#endif /* WITHDSPEXTDDC */
			DISPLC_MODCOUNT
		};
		enum
		{
			PG0 = REDRSUBSET(DPAGE0),
		#if WITHDSPEXTDDC
			PG1 = REDRSUBSET(DPAGE1),
			PGALL = PG0 | PG1 | REDRSUBSET_MENU,
		#else /* WITHDSPEXTDDC */
			PGALL = PG0 | REDRSUBSET_MENU,
		#endif /* WITHDSPEXTDDC */
			PGLATCH = PGALL | REDRSUBSET_SLEEP,
			PGINI = REDRSUBSET_INIT,
			PGunused
		};
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
		/* ---------------------------------- */
			{	0,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
			{	3,	0,	display2_ant5,		REDRM_MODE, PGALL, },
		#if WITHDSPEXTDDC
			{	9,	0,	display_ovf3,		REDRM_BARS, PGALL, },	// ovf
			{	14,	0,	display_pre3,		REDRM_MODE, PGALL, },	// pre
			{	18, 0,	display2_att4,		REDRM_MODE, PGALL, },	// attenuator state
			{	22,	0,	display_lockstate1, REDRM_MODE, PGALL, },
		#else /* WITHDSPEXTDDC */
			{	9,	0,	display_pre3,		REDRM_MODE, PGALL, },	// pre
			{	13, 0,	display2_att4,		REDRM_MODE, PGALL, },	// attenuator state
			{	18,	0,	display2_lockstate4, REDRM_MODE, PGALL, },
		#endif /* WITHDSPEXTDDC */
			{	23, 0,	display2_rxbw3,		REDRM_MODE, PGALL, },
			{	27, 0,	display2_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	2,	display_freqXbig_a, REDRM_FREQ, PGALL, },	// fullwidth = 8 constantly
		#if ! WITHDSPEXTDDC
			{	27, 2,	display2_agc3,		REDRM_MODE, PGALL, },
		#endif /* ! WITHDSPEXTDDC */
			{	27, 4,	display2_voxtune3,	REDRM_MODE, PGALL, },
		/* ---------------------------------- */
		#if WITHUSEAUDIOREC
			{	0,	7,	display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента
		#endif /* WITHUSEAUDIOREC */
			{	4,	7,	display2_mainsub3, REDRM_MODE, PGALL, },	// main/sub RX
			{	8,	7,	display2_vfomode3,	REDRM_MODE, PGALL, },	// SPL
			{	16, 7,	display2_freqX_b,	REDRM_FRQB, PGALL, },
			{	27, 7,	display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		/* ---------------------------------- */
			{	0,	9,	display2_bars,		REDRM_BARS, PG0 | REDRSUBSET_MENU, },	// S-METER, SWR-METER, POWER-METER
			{	0,	10,	display2_legend,	REDRM_MODE, PG0 | REDRSUBSET_MENU, },	// Отображение оцифровки шкалы S-метра
			/* ---------------------------------- */
		#if WITHDSPEXTDDC

			{	0,	9,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
			{	0,	9,	display2_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
			{	0,	9,	display2_spectrum,	REDRM_BARS, PG1, },// подготовка изображения спектра
			{	0,	9,	display2_colorbuff,	REDRM_BARS,	PG1, },// Отображение водопада и/или спектра
		#else /* WITHDSPEXTDDC */
			{	27, 12,	display2_atu3,		REDRM_MODE, PGALL, },	// ATU
			{	27, 14,	display2_byp3,		REDRM_MODE, PGALL, },	// BYP
		#endif /* WITHDSPEXTDDC */
			/* ---------------------------------- */
			{	0,	14,	display_time5,		REDRM_BARS, PG0, },	// TIME
		#if WITHVOLTLEVEL
			{	6,	14,	display2_voltlevelV5, REDRM_VOLT, PG0, },	// voltmeter with "V"
		#endif /* WITHVOLTLEVEL  */
		#if WITHCURRLEVEL || WITHCURRLEVEL2
			{	11, 14,	display2_currlevelA6, REDRM_VOLT, PG0, },	// amphermeter with "A"
		#endif /*  WITHCURRLEVEL || WITHCURRLEVEL2 */
		#if WITHAMHIGHKBDADJ
			{	6, 14,	display_amfmhighcut4,REDRM_MODE, PG0, },	// 3.70
		#endif /* WITHAMHIGHKBDADJ */
			{	18, 14,	display_samfreqdelta8, REDRM_BARS, PG0 | REDRSUBSET_MENU, },	/* Получить информацию об ошибке настройки в режиме SAM */
		#if WITHNOTCHONOFF || WITHNOTCHFREQ
			{	27, 14,	display_notch3, REDRM_MODE, PG0, },	// 3.7
		#endif /* WITHNOTCHONOFF || WITHNOTCHFREQ */
		/* ---------------------------------- */
	#if WITHMENU
			{	0,	12,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
			{	0,	14,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
			{	4,	14,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
			//{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
		#if WITHVOLTLEVEL && (WITHCURRLEVEL || WITHCURRLEVEL2)
			//{	0,	14,	display2_voltlevelV5, REDRM_VOLT, REDRSUBSET_MENU, },	// voltmeter with "V"
			//{	6,	14,	display2_currlevelA6, REDRM_VOLT, REDRSUBSET_MENU, },	// amphermeter with "A"
		#endif /* WITHVOLTLEVEL && (WITHCURRLEVEL || WITHCURRLEVEL2) */
	#endif /* WITHMENU */
		};

	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 1;
			p->ystep = 2;	// количество ячеек разметки на одну строку меню
			p->reverse = 0;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#endif /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */

#elif DSTYLE_G_X320_Y240 && WITHSPECTRUMWF
	// DSP version - with spectrum scope
	// TFT панель 320 * 240 ADI_3.2_AM-240320D4TOQW-T00H(R)
	// 320*240 SF-TC240T-9370-T с контроллером ILI9341
	// 32*15 знакомест 10*16
	#define CHAR_W	10
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */

	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		//					"012345678901234567890123"
		#define SWRPWRMAP	"1   2   3   4  0% | 100%"
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
	#else
		//					"012345678901234567890123"
		#define POWERMAP	"0    25    50   75   100"
		#define SWRMAP		"1   |   2  |   3   |   4"	//
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
	#endif
	//						"012345678901234567890123"
	#define SMETERMAP		"1  3  5  7  9 +20 +40 60"

	enum
	{
		BDCV_ALLRX = 10,	// общая высота, отведенная под панораму, водопад и иные отображения

		BDCO_SPMRX = 0,	// смещение спектра по вертикали в ячейках от начала общего поля
		BDCV_SPMRX = 5,	// вертикальный размер спектра в ячейках

		BDCO_WFLRX = 4,	// смещение водопада по вертикали в ячейках от начала общего поля
		BDCV_WFLRX = 5,	// вертикальный размер водопада в ячейках

		BDTH_ALLRX = 24,	// ширина зоны для отображение полосы на индикаторе
		BDTH_RIGHTRX = 11,	// ширина индикатора плюсов
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
		BDTH_SPACERX = 0,
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 13,
		BDTH_SPACESWR = 2,
		BDTH_ALLPWR = 9,
		BDTH_SPACEPWR = 0
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	enum
	{
		PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
		PATTERN_BAR_FULL = 0x7e,
		PATTERN_BAR_HALF = 0x3c,
		PATTERN_BAR_EMPTYFULL = 0x00,
		PATTERN_BAR_EMPTYHALF = 0x00
	};
	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

	enum
	{
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DISPLC_MODCOUNT
	};
	enum {
		PG0 = REDRSUBSET(DPAGE0),
		PGALL = PG0 | REDRSUBSET_MENU,
		PGNOMEMU = PG0,
		PGLATCH = PGALL | REDRSUBSET_SLEEP,	// страницы, на которых возможно отображение водопада или панорамы.
		PGINI = REDRSUBSET_INIT,
		PGunused
	};
	#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
	#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0,	0,	display2_clearbg, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
		{	0,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
		{	3,	0,	display2_voxtune3,	REDRM_MODE, PGALL, },
		{	7,	0,	display2_att4,		REDRM_MODE, PGALL, },
		{	12, 0,	display_pre3,		REDRM_MODE, PGALL, },
	#if WITHDSPEXTDDC
		{	16, 0,	display_ovf3,		REDRM_BARS, PGALL, },	// ovf/pre
	#endif /* WITHDSPEXTDDC */
		{	20, 0,	display2_lockstate4, REDRM_MODE, PGALL, },
		{	25, 0,	display2_agc3,		REDRM_MODE, PGALL, },
		{	29, 0,	display2_rxbw3,		REDRM_MODE, PGALL, },

		{	0,	3,	display_freqXbig_a, REDRM_FREQ, PGALL, },
		{	25, 3,	display2_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		{	29, 3,	display2_nr3,		REDRM_MODE,	PGALL, },	// NR
		//---
		{	0,	9,	display_vfomode5,	REDRM_MODE, PGALL, },	// SPLIT
		{	6,	9,	display2_freqX_b,	REDRM_FRQB, PGALL, },
	#if WITHUSEDUALWATCH
		{	29, 9,	display2_mainsub3,	REDRM_MODE, PGNOMEMU, },	// main/sub RX
	#endif /* WITHUSEDUALWATCH */
		{	25, 9,	display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		//---
		{	0,	12,	display2_legend,	REDRM_MODE, PG0, },	// Отображение оцифровки шкалы S-метра, PWR & SWR-метра
		{	0,	15,	display2_bars,		REDRM_BARS, PG0, },	// S-METER, SWR-METER, POWER-METER
		{	27, 15,	display_smeter5,	REDRM_BARS, PG0, },	// signal level

		{	0,	18,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
		{	0,	18,	display2_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
		{	0,	18,	display2_spectrum,	REDRM_BARS, PG0, },// подготовка изображения спектра
		{	0,	18,	display2_waterfall,	REDRM_BARS, PG0, },// подготовка изображения водопада
		{	0,	18,	display2_colorbuff,	REDRM_BARS,	PG0, },// Отображение водопада и/или спектра

		//---
		//{	22, 25,	display_samfreqdelta8, REDRM_BARS, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */

	#if WITHVOLTLEVEL
		{	0, 28,	display2_voltlevelV5, REDRM_VOLT, PGALL, },	// voltmeter with "V"
	#endif /* WITHVOLTLEVEL */
	#if WITHCURRLEVEL || WITHCURRLEVEL2
		{	6, 28,	display2_currlevelA6, REDRM_VOLT, PGALL, },	// amphermeter with "A"
	#endif /* WITHCURRLEVEL || WITHCURRLEVEL2 */
	#if defined (RTC1_TYPE)
		{	13, 28,	display_time8,		REDRM_BARS, PGALL, },	// TIME
	#endif /* defined (RTC1_TYPE) */
	#if WITHUSEAUDIOREC
		{	25, 28,	display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента
	#endif /* WITHUSEAUDIOREC */
	#if WITHMENU
		{	1 + LABELW * 0 + 0,	18,	display2_multilinemenu_block_groups,	REDRM_MLBL, 	REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
		{	1 + LABELW * 1 + 1,	18,	display2_multilinemenu_block_params,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
		{	1 + LABELW * 2 + 2,	18,	display2_multilinemenu_block_vals,	REDRM_MVAL, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
	#endif /* WITHMENU */
		{	0,	0,	display2_nextfb, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
	};

	/* получить координаты окна с панорамой и/или водопадом. */
	void display2_getpipparams(pipparams_t * p)
	{
		p->x = 0; //GRID2X(0);	// позиция верхнего левого угла в пикселях
		p->y = GRID2Y(18);	// позиция верхнего левого угла в пикселях
		p->w = DIM_X; //GRID2X(CHARS2GRID(BDTH_ALLRX));	// размер по горизонтали в пикселях
		p->h = GRID2Y(BDCV_ALLRX);				// размер по вертикали в пикселях
	}

	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 5;
			p->ystep = 2;	// количество ячеек разметки на одну строку меню
			p->reverse = 1;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */
#endif

