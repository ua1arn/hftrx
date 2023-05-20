	// RDX0077
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP

		enum
		{
			BDTH_ALLRX = 21,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 11,	// ширина индикатора плюсов
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 9,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 11,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//					  "012345678901234567890"
		#define SMETERMAP 	  " 1 3 5 7 9 + 20 40 60"	//
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			#define SWRPWRMAP "1 | 2 | 3 0%   | 100%"
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		#else
			#define POWERMAP  " 0 10 20 40 60 80 100"
			#define SWRMAP    "1    |    2    |   3 "	//
			#define SWRMAX	(SWRMIN * 31 / 10)	// 3.1 - значение на полной шкале
		#endif
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
		#if 1
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x7e,
			PATTERN_BAR_EMPTYFULL = 0x42,
			PATTERN_BAR_EMPTYHALF = 0x42
		#else
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x42,
			PATTERN_BAR_EMPTYHALF = 0x24
		#endif
		};

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0,	0,	display2_keyboard_screen0,	REDRM_KEYB, REDRSUBSET_ALL | REDRSUBSET_SLEEP, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },	// TX/RX
			{	3, 0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },		// VOX/TUN
			{	7, 0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	10, 0,	display_hplp2,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	13, 0,	display_voltlevel4, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter without "V"
			{	18, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },


			{	0,	1,	display2_freqX_a_init,	REDRM_INIS, REDRSUBSET_INIT, },
			{	0,	1,	display2_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	18, 2,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	2, 4,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	7, 4,	display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },
			{	18, 4,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...


			{	2, 5,	display2_lockstate4, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7, 5,	display2_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	11, 5,	display2_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0, 6,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
			{	0, 7,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },// Отображение оцифровки шкалы S-метра
		#if WITHMENU
			{	0,				0,		display2_keyboard_menu,					REDRM_KEYB, REDRSUBSET_MENU, }, // Обработка клавиатуры и валкодеров при нахождении в режиме меню
			{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
			{	0, 1,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
			{	4, 1,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
			{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
		#endif /* WITHMENU */
		};
	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 1;
			p->ystep = 1;	// количество ячеек разметки на одну строку меню
			p->reverse = 0;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#else /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */

		enum
		{
			BDTH_ALLRX = 17,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 6,	// ширина индикатора плюсов
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 10,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 6,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		//#define SMETERMAP "1 3 5 7 9 + 20 40 60"
		//#define POWERMAP  "0 10 20 40 60 80 100"
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x42,
			PATTERN_BAR_EMPTYHALF = 0x24
		};

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0,	0,	display2_keyboard_screen0,	REDRM_KEYB, REDRSUBSET_ALL | REDRSUBSET_SLEEP, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана
			{	0,	0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3,	0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7,	0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	16, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	18, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	12, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	0,	2,	display2_freqX_a_init,	REDRM_INIS, REDRSUBSET_INIT, },		// col = 1 for !WIDEFREQ
			{	0,	2,	display2_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },		// col = 1 for !WIDEFREQ
			{	18, 3,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			{	18, 5,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...
			{	0,	5,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	7,	5,	display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },		// x=8 then !WIDEFREQ
			{	18, 7,	display2_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	0,	7,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
		#if WITHMENU
			{	0,				0,		display2_keyboard_menu,					REDRM_KEYB, REDRSUBSET_MENU, }, // Обработка клавиатуры и валкодеров при нахождении в режиме меню
			{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
			{	0, 1,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
			{	4, 1,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
			{	16, 0,	display2_lockstate4,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
		#endif /* WITHMENU */
		};
	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 1;
			p->ystep = 1;	// количество ячеек разметки на одну строку меню
			p->reverse = 0;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#endif /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */
