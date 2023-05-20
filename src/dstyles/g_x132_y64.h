	// RDX0154
	// по горизонтали 22 знакоместа (x=0..21)
	#define CHAR_W	6
	#define CHAR_H	8
	#define SMALLCHARH 8 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP
		// SW20XXX
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
		//					  "0123456789012345678901"
		#define SMETERMAP 	  " 1 3 5 7 9 + 20 40 60 "	//
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			#define SWRPWRMAP "1 | 2 | 3 0%   | 100% "
			#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		#else
			#define POWERMAP  " 0 10 20 40 60 80 100 "
			#define SWRMAP    "1    |    2    |   3  "	//
			#define SWRMAX	(SWRMIN * 32 / 10)	// 3.2 - значение на полной шкале
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
		// по горизонтали 22 знакоместа (x=0..21)
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0,	0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },	// TX/RX
			{	3,	0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// VOX/TUN
			{	7,	0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// PRE/ATT/___
			{	11, 0,	display2_lockstate4, REDRM_MODE, REDRSUBSET(DPAGE0), },	// LOCK
			{	19, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// 3.1

			{	0,	1,	display2_freqX_a_init,	REDRM_INIS, REDRSUBSET_INIT, },
			{	0,	1,	display2_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },
			{	19, 2,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/..
			//
			{	2,	4,	display_vfomode5,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	8,	4,	display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },
			{	19, 4,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0,	5,	display_hplp2,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// HP/LP
			{	3,	5,	display2_voltlevelV5,REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter with "V"
		#if WITHANTSELECTRX || WITHANTSELECT1RX
			{	9,	5,	display2_ant5,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// ANTENNA
		#elif WITHANTSELECT
			{	9,	5,	display2_ant5,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// ANTENNA
		#endif /* WITHANTSELECT */
			{	9,	5,	display_time5,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME
		#if WITHTX && WITHAUTOTUNER
			{	15, 5,	display2_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// ATU
			{	19, 5,	display2_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// BYP
		#endif /* WITHTX && WITHAUTOTUNER */

			{	0,	6,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
			{	0,	7,	display2_legend,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// Отображение оцифровки шкалы S-метра
		#if WITHMENU
			{	0,	0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
			{	0,	1,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
			{	4,	1,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
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
			BDTH_ALLSWR = 8,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 8,
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
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
			PATTERN_BAR_EMPTYHALF = 0x00	//0x00
		};
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const FLASHMEM struct dzone dzones [] =
		{
			{	0, 0,	display_txrxstate2, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	3, 0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	7, 0,	display2_att4,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	12, 0,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	16, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	19, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET(DPAGE0), },

			{	0, 2,	display2_freqX_a_init,	REDRM_INIS, REDRSUBSET_INIT, },	// x=1 then !WIDEFREQ
			{	0, 2,	display2_freqX_a,	REDRM_FREQ, REDRSUBSET(DPAGE0), },	// x=1 then !WIDEFREQ
			{	19, 3,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 5,	display_voltlevel4, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// voltmeter without "V"
			{	0, 6,	display2_currlevelA6, REDRM_VOLT, REDRSUBSET(DPAGE0), },	// amphermeter with "A"
			{	13, 5,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
			{	16, 5,	display2_freqX_b,	REDRM_FRQB, REDRSUBSET(DPAGE0), },	// x=9 then !WIDEFREQ
			{	19, 5,	display2_mode3_b,	REDRM_MODE,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 6,	display2_atu3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
			{	4, 6,	display2_byp3,		REDRM_MODE, REDRSUBSET(DPAGE0), },

	#if WITHIF4DSP
		#if WITHUSEAUDIOREC
			{	19, 7,	display2_rec3,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// Отображение режима записи аудио фрагмента
		#endif /* WITHUSEAUDIOREC */
	#else /* WITHIF4DSP */
			{	19, 7,	display2_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
	#endif /* WITHIF4DSP */
	#if WITHUSEDUALWATCH
			{	19, 6,	display2_mainsub3, REDRM_BARS, REDRSUBSET(DPAGE0), },	// main/sub RX
	#endif /* WITHUSEDUALWATCH */

			{	0, 7,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER

		#if WITHMENU
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

	#endif /* DSTYLE_UR3LMZMOD */
