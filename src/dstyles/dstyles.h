#if DSTYLE_T_X20_Y4

	enum
	{
		BDTH_ALLRX = 14,	// ширина зоны для отображение полосы на индикаторе
		BDTH_RIGHTRX = 5,	// ширина индикатора плюсов
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
		BDTH_SPACERX = 0,
		//
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 9,
		BDTH_SPACESWR = 1,
		BDTH_ALLPWR = 4,
		BDTH_SPACEPWR = 0
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

	enum {
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DPAGE1,					// Страница, в которой отображаются основные (или все)
		DISPLC_MODCOUNT,
	};
	enum {
		DPAGEEXT = REDRSUBSET(DPAGE0) | REDRSUBSET(DPAGE1)
	};
	#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
	#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0,	0,	display2_keyboard_screen0,	REDRM_KEYB, REDRSUBSET_ALL | REDRSUBSET_SLEEP, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана
		{	0, 0,	display_freqchr_a,	REDRM_FREQ, DPAGEEXT, },	// частота для символьных дисплеев
		{	0,	0,	display2_keyboard_screen0,	REDRM_KEYB, PGALL | REDRSUBSET_SLEEP, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана
		//{	0, 0,	display_txrxstate2,	REDRM_MODE, DPAGEEXT, },
		{	9, 0,	display2_mode3_a,	REDRM_MODE,	DPAGEEXT, },	// SSB/CW/AM/FM/...
		{	12, 0,	display_lockstate1,	REDRM_MODE, DPAGEEXT, },
		{	13, 0,	display2_rxbw3,		REDRM_MODE, DPAGEEXT, },
		{	17, 0,	display2_vfomode3,	REDRM_MODE, DPAGEEXT, },	// SPLIT

		{	0, 1,	display_att_tx3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
		{	4, 1,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },
	#if defined (RTC1_TYPE)
		{	8, 1,	display_time8,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// TIME
	#endif /* defined (RTC1_TYPE) */
	#if WITHUSEDUALWATCH
		{	0, 1,	display_freqchr_b,	REDRM_FREQ, REDRSUBSET(DPAGE1), },	// частота для символьных дисплеев
		{	17, 1,	display2_mainsub3, REDRM_MODE, DPAGEEXT, },	// main/sub RX
	#endif /* WITHUSEDUALWATCH */

		{	0, 2,	display2_bars,		REDRM_BARS, DPAGEEXT, },	// S-METER, SWR-METER, POWER-METER
		{	17, 2,	display2_voxtune3,	REDRM_MODE, DPAGEEXT, },

	#if WITHVOLTLEVEL && WITHCURRLEVEL
		{	0, 3,	display_voltlevel4, REDRM_VOLT, DPAGEEXT, },	// voltmeter with "V"
		{	5, 3,	display2_currlevel5, REDRM_VOLT, DPAGEEXT, },	// without "A"
	#endif /* WITHVOLTLEVEL && WITHCURRLEVEL */
#if WITHIF4DSP
	#if WITHUSEAUDIOREC
		{	13, 3,	display2_rec3,		REDRM_BARS, DPAGEEXT, },	// Отображение режима записи аудио фрагмента
	#endif /* WITHUSEAUDIOREC */
#endif /* WITHIF4DSP */
		{	17, 3,	display2_agc3,		REDRM_MODE, DPAGEEXT, },

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

#elif DSTYLE_T_X20_Y2
	/*
		Вот то, как сейчас описан дисплей 2*20, предлагаю внести в это
		изменения (вместо рисования картинок) - это будет отдельный,
		твой персональный формат.
		Можно указывать, какой элемент в каком из наборов отображаемых символов находится.
		Если есть желание для приёма и передачи сделать разные элементы - укажи это в комментарии,
		я дополню. Просто эти замещающие друг друга элементы должны занимать одинаковое
		место на экране, затирая старое изображение при переключении режима отображения.
		То, что переключается по MENU (REDRSUBSET(0)/REDRSUBSET(1)) - перерисовывается со стиранием экрана.

	*/
	enum
	{
		BDTH_ALLRX = 12,	// ширина зоны для отображение полосы на индикаторе
		BDTH_RIGHTRX = 4,	// ширина индикатора плюсов
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
		BDTH_SPACERX = 1,		/* количество позиций, затираемых справа от полосы S-метра */
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 6,
		BDTH_SPACESWR = 1,	/* количество позиций, затираемых справа от полосы SWR-метра */
		BDTH_ALLPWR = 5,
		BDTH_SPACEPWR = 0	/* количество позиций, затираемых справа от полосы PWR-метра */
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

	enum {
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DPAGE_SMETER,
		DPAGE_TIME,
		DISPLC_MODCOUNT
	};
	#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
	#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0,	0,	display2_keyboard_screen0,	REDRM_KEYB, REDRSUBSET_ALL | REDRSUBSET_SLEEP, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана
		// строка 0
		{	0, 0,	display2_vfomode3,	REDRM_MODE, REDRSUBSET_ALL, },	// SPLIT
		{	4, 0,	display_freqchr_a,	REDRM_FREQ, REDRSUBSET_ALL, },	// частота для символьных дисплеев
		{	12, 0,	display_lockstate1,	REDRM_MODE, REDRSUBSET_ALL, },
		{	13, 0,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET_ALL, },	// SSB/CW/AM/FM/...
		{	17, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET_ALL, },
		// строка 1 - постоянные элементы
		{	0, 1,	display2_voxtune3,	REDRM_MODE, REDRSUBSET_ALL, },
		// строка 1
		{	4, 1,	display_att_tx3,	REDRM_MODE, REDRSUBSET(DPAGE0), },
	#if WITHDSPEXTDDC
		{	9, 1,	display2_preovf3,	REDRM_BARS, REDRSUBSET(DPAGE0), },	// ovf/pre
	#else /* WITHDSPEXTDDC */
		{	9, 1,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// pre
	#endif /* WITHDSPEXTDDC */
		// строка 1
		{	4, 1,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE_SMETER), },	// S-METER, SWR-METER, POWER-METER
		{	4, 1,	display_time8,		REDRM_BARS, REDRSUBSET(DPAGE_TIME), },	// TIME
		// строка 1 - постоянные элементы
		{	17, 1,	display2_agc3,		REDRM_MODE, REDRSUBSET_ALL, },

		//{	0, 0,	display_txrxstate2,	REDRM_MODE, REDRSUBSET_ALL, },
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

#elif DSTYLE_T_X20_Y2_IGOR

	enum
	{
		BDTH_ALLRX = 12,	// ширина зоны для отображение полосы на индикаторе
		BDTH_RIGHTRX = 4,	// ширина индикатора плюсов
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
		BDTH_SPACERX = 1,		/* количество позиций, затираемых справа от полосы S-метра */
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 6,
		BDTH_SPACESWR = 1,	/* количество позиций, затираемых справа от полосы SWR-метра */
		BDTH_ALLPWR = 5,
		BDTH_SPACEPWR = 0	/* количество позиций, затираемых справа от полосы PWR-метра */
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

	enum {
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DPAGE1 = 0,					// Страница, в которой отображаются основные (или все)
		DISPLC_MODCOUNT
	};
	#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
	#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0,	0,	display2_keyboard_screen0,	REDRM_KEYB, PGALL | REDRSUBSET_SLEEP, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана
		{	0, 0,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0) | REDRSUBSET(DPAGE1), },	// SPLIT
		{	4, 0,	display_freqchr_a,	REDRM_FREQ, REDRSUBSET(DPAGE0) | REDRSUBSET(DPAGE1), },	// частота для символьных дисплеев
		{	12, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET(DPAGE0) | REDRSUBSET(DPAGE1), },
		{	4, 1,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
		//{	0, 1, display_pbt,		REDRM_BARS, REDRSUBSET(DPAGE1), },	// PBT +00.00
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

#elif DSTYLE_T_X16_Y2 && DSTYLE_SIMPLEFREQ

	enum
	{
		BDTH_ALLRX = 12,	// ширина зоны для отображение полосы на индикаторе
		BDTH_RIGHTRX = 4,	// ширина индикатора плюсов
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
		BDTH_SPACERX = 0,
		//
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 7,
		BDTH_SPACESWR = 1,
		BDTH_ALLPWR = 4,
		BDTH_SPACEPWR = 0
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	40	// 4.0 - значение на полной шкале
	enum {
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		//
		DISPLC_MODCOUNT
	};
	#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
	#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0, 0,	display_freqchr_a,	REDRM_FREQ, REDRSUBSET_ALL, },	// частота для символьных дисплеев
		{	0,	0,	display2_keyboard_screen0,	REDRM_KEYB, REDRSUBSET_ALL, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана
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

#elif DSTYLE_T_X16_Y2

	enum
	{
		BDTH_ALLRX = 16,	// ширина зоны для отображение полосы на индикаторе
		BDTH_RIGHTRX = 5,	// ширина индикатора плюсов
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
		BDTH_SPACERX = 0,
		//
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 7,
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
	#define SWRMAX	40	// 4.0 - значение на полной шкале
	enum {
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DPAGE_SMETER,			// Страница с отображением S-метра, SWR-метра
	#if WITHUSEAUDIOREC
		//DPAGE_SDCARD,
	#endif /* WITHUSEAUDIOREC */
	#if WITHUSEDUALWATCH
		DPAGE_SUBRX,
	#endif /* WITHUSEDUALWATCH */
	#if defined (RTC1_TYPE)
		DPAGE_TIME,
	#endif /* defined (RTC1_TYPE) */
	#if WITHMODEM
		DPAGE_BPSK,
	#endif /* WITHMODEM */
	#if WITHVOLTLEVEL && WITHCURRLEVEL
		DPAGE_VOLTS,
	#endif /* WITHVOLTLEVEL && WITHCURRLEVEL */
		//
		DISPLC_MODCOUNT
	};
	#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
	#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0,	0,	display2_keyboard_screen0,	REDRM_KEYB, REDRSUBSET_ALL | REDRSUBSET_SLEEP, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана
		{	0, 0,	display_freqchr_a,	REDRM_FREQ, REDRSUBSET_ALL, },	// частота для символьных дисплеев
		{	8, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET_ALL, },
		{	9, 0,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET_ALL, },	// SSB/CW/AM/FM/...
		{	13, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET_ALL, },

		{	0, 1,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// SPLIT
		{	4, 1,	display_att_tx3,	REDRM_MODE, REDRSUBSET(DPAGE0), },	// при скрытом s-metre, при передаче показывает TX
	#if WITHDSPEXTDDC
		{	9, 1,	display2_preovf3,	REDRM_BARS, REDRSUBSET(DPAGE0), },	// ovf/pre
	#else /* WITHDSPEXTDDC */
		{	9, 1,	display_pre3,		REDRM_MODE, REDRSUBSET(DPAGE0), },	// pre
	#endif /* WITHDSPEXTDDC */
#if WITHIF4DSP
	#if WITHUSEAUDIOREC
		{	13, 1,	display2_rec3,		REDRM_BARS, REDRSUBSET(DPAGE0) /*| REDRSUBSET(DPAGE_SMETER)*/, },	// Отображение режима записи аудио фрагмента
	#endif /* WITHUSEAUDIOREC */
#else /* WITHIF4DSP */
		{	13, 1,	display2_agc3,		REDRM_MODE, REDRSUBSET(DPAGE0) /*| REDRSUBSET(DPAGE_SMETER)*/, },
#endif /* WITHIF4DSP */

		{	0, 1,	display2_bars,		REDRM_BARS, REDRSUBSET(DPAGE_SMETER), },	// S-METER, SWR-METER, POWER-METER

	#if WITHVOLTLEVEL && WITHCURRLEVEL
		{	0, 1,	display2_voltlevelV5, REDRM_VOLT, REDRSUBSET(DPAGE_VOLTS), },	// voltmeter with "V"
		{	6, 1,	display2_currlevelA6, REDRM_VOLT, REDRSUBSET(DPAGE_VOLTS), },	// amphermeter with "A"
	#endif /* WITHVOLTLEVEL && WITHCURRLEVEL */
	#if WITHMODEM
		{	0, 1,	display2_freqdelta8, REDRM_BARS, REDRSUBSET(DPAGE_BPSK), },	// выход ЧМ демодулятора
	#endif /* WITHMODEM */
	#if WITHUSEDUALWATCH
		{	0, 1,	display_freqchr_b,	REDRM_FREQ, REDRSUBSET(DPAGE_SUBRX), },	// FREQ B
		{	9, 1,	display2_vfomode3,	REDRM_MODE, REDRSUBSET(DPAGE_SUBRX), },	// SPLIT
		{	13, 1,	display2_mainsub3, REDRM_MODE, REDRSUBSET(DPAGE_SUBRX), },	// main/sub RX
	#endif /* WITHUSEDUALWATCH */

	//#if WITHUSEAUDIOREC
	//	{	0, 1,	display_recstatus,	REDRM_BARS, REDRSUBSET(DPAGE_SDCARD), },	// recording debug information
	//	{	13, 1,	display2_rec3,		REDRM_BARS, REDRSUBSET(DPAGE_SDCARD), },	// Отображение режима записи аудио фрагмента
	//#endif /* WITHUSEAUDIOREC */

	#if defined (RTC1_TYPE)
		{	0, 1,	display_time8,		REDRM_BARS, REDRSUBSET(DPAGE_TIME), },	// TIME
	#if WITHUSEDUALWATCH
		{	9, 1,	display2_mainsub3,	REDRM_BARS, REDRSUBSET(DPAGE_TIME), },	// main/sub RX
	#endif /* WITHUSEDUALWATCH */
	#if WITHUSEAUDIOREC
		{	13, 1,	display2_rec3,		REDRM_BARS, REDRSUBSET(DPAGE_TIME), },	// Отображение режима записи аудио фрагмента
	#endif /* WITHUSEAUDIOREC */
	#endif /* defined (RTC1_TYPE) */


		//{	0, 0,	display_txrxstate2,	REDRM_MODE, REDRSUBSET_ALL, },
		//{	0, 0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET_ALL, },
	#if WITHMENU
		{	0,				0,		display2_keyboard_menu,					REDRM_KEYB, REDRSUBSET_MENU, }, // Обработка клавиатуры и валкодеров при нахождении в режиме меню
		{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра
		{	0, 1,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
		{	4, 1,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
		{	15, 0,	display_lockstate1,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
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

#elif DSTYLE_T_X16_Y4

	enum
	{
		BDTH_ALLRX = 14,	// ширина зоны для отображение полосы на индикаторе
		BDTH_RIGHTRX = 5,	// ширина индикатора плюсов
		BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
		BDTH_SPACERX = 0,
		//
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 9,
		BDTH_SPACESWR = 1,
		BDTH_ALLPWR = 4,
		BDTH_SPACEPWR = 0
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX
	#endif /* WITHSHOWSWRPWR */
	};
	#define SWRMAX	40	// 4.0 - значение на полной шкале

	enum {
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DISPLC_MODCOUNT
	};
	#define DISPLC_WIDTH	7	// количество цифр в отображении частоты
	#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0,	0,	display2_keyboard_screen0,	REDRM_KEYB, REDRSUBSET_ALL | REDRSUBSET_SLEEP, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана
		{	0, 0,	display_freqchr_a,	REDRM_FREQ, REDRSUBSET_ALL, },	// частота для символьных дисплеев
		{	8, 0,	display_lockstate1, REDRM_MODE, REDRSUBSET_ALL, },
		{	9, 0,	display2_mode3_a,	REDRM_MODE,	REDRSUBSET_ALL, },	// SSB/CW/AM/FM/...
		{	13, 0,	display2_rxbw3,		REDRM_MODE, REDRSUBSET_ALL, },

		{	0, 1,	display2_vfomode3,	REDRM_MODE, REDRSUBSET_ALL, },	// SPLIT
		{	4, 1,	display_att_tx3,	REDRM_MODE, REDRSUBSET_ALL, },		// при передаче показывает TX
	#if WITHDSPEXTDDC
		{	9, 1,	display2_preovf3,	REDRM_BARS, REDRSUBSET_ALL, },	// ovf/pre
	#else /* WITHDSPEXTDDC */
		{	9, 1,	display_pre3,		REDRM_MODE, REDRSUBSET_ALL, },	// pre
	#endif /* WITHDSPEXTDDC */

		{	0, 2,	display2_bars,		REDRM_BARS, REDRSUBSET_ALL, },	// S-METER, SWR-METER, POWER-METER

		{	0, 3,	display2_voxtune3,	REDRM_MODE, REDRSUBSET_ALL, },
	#if defined (RTC1_TYPE)
		{	4, 3,	display_time8,		REDRM_BARS, REDRSUBSET_ALL, },	// TIME
	#endif /* defined (RTC1_TYPE) */
		{	13, 3,	display2_agc3,		REDRM_MODE, REDRSUBSET_ALL, },

		//{	0, 0,	display_txrxstate2,	REDRM_MODE, REDRSUBSET_ALL, },
		//{	0, 0,	display2_voxtune3,	REDRM_MODE, REDRSUBSET_ALL, },

	#if WITHMENU
		{	0,				0,		display2_keyboard_menu,					REDRM_KEYB, REDRSUBSET_MENU, }, // Обработка клавиатуры и валкодеров при нахождении в режиме меню
		{	0, 0,	display2_menu_valxx,	REDRM_MVAL, REDRSUBSET_MENU, },	// значение параметра параметра
		{	0, 1,	display2_menu_lblc3,	REDRM_MFXX, REDRSUBSET_MENU, },	// код редактируемого параметра
		{	4, 1,	display2_menu_lblst,	REDRM_MLBL, REDRSUBSET_MENU, },	// название редактируемого параметра
		{	15, 0,	display_lockstate1,	REDRM_MODE, REDRSUBSET_MENU, },	// состояние блокировки валкодера
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

#elif DSTYLE_G_X64_Y32
    #include "g_x64_y32.h"
#elif DSTYLE_G_X128_Y64
    #include "g_x128_y64.h"
#elif DSTYLE_G_X132_Y64
    #include "g_x132_y64.h"
#elif DSTYLE_G_X160_Y128
    #include "g_x160_y128.h"
#elif DSTYLE_G_X176_Y132
    #include "g_x176_y132.h"
#elif DSTYLE_G_X220_Y176
    #include "g_x220_y176.h"
#elif DSTYLE_G_X240_Y128
    #include "g_x240_y128.h"
#elif DSTYLE_G_X320_Y240
    #include "g_x320_y240.h"
#elif DSTYLE_G_X480_Y272 && WITHSPECTRUMWF && ! WITHTOUCHGUI
    #include "g_x480_y272_spectrum_notouch.h"
#elif DSTYLE_G_X480_Y272
    #include "g_x480_y272.h"
#elif DSTYLE_G_X800_Y480 && WITHTOUCHGUI && WITHALTERNATIVELAYOUT
    #include "g_x800_y480_touch_alternative.h"
#elif DSTYLE_G_X800_Y480 && WITHTOUCHGUI //&& WITHSPECTRUMWF
    #include "g_x800_y480_touch.h"
#elif DSTYLE_G_X1024_Y600
	#include "g_x1024_y600.h"
#elif DSTYLE_G_X800_Y480
	#include "g_x800_y480.h"
#elif DSTYLE_G_X800_Y480 && 1	//&& WITHSPECTRUMWF
    #include "g_x800_y480_mini.h"
#elif DSTYLE_G_DUMMY
	#include "g_dummy.h"
#else
	#error TODO: to be implemented
#endif /* LCDMODE_LS020 */
