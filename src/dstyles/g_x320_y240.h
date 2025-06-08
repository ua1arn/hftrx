	// No DSP version - no spectrum scope
	// TFT панель 320 * 240 ADI_3.2_AM-240320D4TOQW-T00H(R)
	// 320*240 SF-TC240T-9370-T с контроллером ILI9341
	// 32*15 знакомест 10*16
	#define CHAR_W	10
	#define CHAR_H	8
	#define SMALLCHARH 16 /* Font height */

	#if DSTYLE_UR3LMZMOD && WITHONEATTONEAMP
		// TFT панель 320 * 240
		enum
		{
			BDTH_ALLRX = 20,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 11,	// ширина индикатора плюсов
			BDTH_LEFTRX = BDTH_ALLRX - BDTH_RIGHTRX,	// ширина индикатора баллов
			BDTH_SPACERX = 0,
		#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
			BDTH_ALLSWR = 5,
			BDTH_SPACESWR = 1,
			BDTH_ALLPWR = 10,
			BDTH_SPACEPWR = 0
		#else /* WITHSHOWSWRPWR */
			BDTH_ALLSWR = BDTH_ALLRX,
			BDTH_SPACESWR = BDTH_SPACERX,
			BDTH_ALLPWR = BDTH_ALLRX,
			BDTH_SPACEPWR = BDTH_SPACERX
		#endif /* WITHSHOWSWRPWR */
		};
		#define SMETERMAP "1 3 5 7 9 + 20 40 60"
		#define SWRPWRMAP "1 2 3  0%   |   100%"
		#define POWERMAP  "0 10 20 40 60 80 100"
		#define SWRMAX	(SWRMIN * 30 / 10)	// 3.0 - значение на полной шкале
		enum
		{
			PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
			PATTERN_BAR_FULL = 0xFF,
			PATTERN_BAR_HALF = 0xFF,
			PATTERN_BAR_EMPTYFULL = 0x00,
			PATTERN_BAR_EMPTYHALF = 0x00
		};

		enum {
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const dzone_t dzones [] =
		{
				{	0,	0,	0, 0, display2_preparebg,	& dzi_default, REDRSUBSET_SHOW, }, // Стирание фона
			{	0,	0,	0,	0,	display2_keyboard_screen0,	& dzi_default, PGALL | REDRSUBSET_SLEEP, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана
			{	0, 0,	2,	0,	display_txrxstate2, & dzi_txrx, REDRSUBSET(DPAGE0), },
			{	3, 0,	3,	0,	display2_voxtune3,	& dzi_default, REDRSUBSET(DPAGE0), },
			{	7, 0,	4,	0,	display2_att4,		& dzi_attenuator, REDRSUBSET(DPAGE0), },
			{	12, 0,	3,	0,	display2_preovf3,	& dzi_default, REDRSUBSET(DPAGE0), },
			{	16, 0,	1,	0,	display_lockstate1, & dzi_default, REDRSUBSET(DPAGE0), },
			{	19, 0,	3,	0,	display2_rxbw3,		& dzi_rxbw, REDRSUBSET(DPAGE0), },

			{	0, 8,	0,	0,	display_freqXbig_a, & dzi_default, REDRSUBSET(DPAGE0), },
			{	19, 8,	3,	0,	display2_mode3_a,	& dzi_modea,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	0, 18,	0,	0,	display_vfomode5,	& dzi_default, REDRSUBSET(DPAGE0), },	// SPLIT
			{	5, 18,	0,	0,	display2_freqX_b,	& dzi_freqb, REDRSUBSET(DPAGE0), },
			{	19, 18,	3,	0,	display2_mode3_b,	& dzi_modeb,	REDRSUBSET(DPAGE0), },	// SSB/CW/AM/FM/...

			{	1, 24,	0,	0,	display2_bars,		& dzi_default, REDRSUBSET(DPAGE0), },	// S-METER, SWR-METER, POWER-METER
		#if defined (RTC1_TYPE)
			{	0, 28,	0,	0,	display_time8,		& dzi_default, REDRSUBSET_MENU | REDRSUBSET(DPAGE0), },	// TIME
		#endif /* defined (RTC1_TYPE) */
			{	18, 28,	3,	0,	display2_agc3,		& dzi_default, REDRSUBSET(DPAGE0), },
		#if WITHMENU
			{	0,				DLES,	BDTH_ALLRX, 5, display2_multilinemenu_block,	& dzi_compat, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
		#endif /* WITHMENU */
			{	0,	0,	0, 0, display2_showmain,	& dzi_default, REDRSUBSET_SHOW, }, // запись подготовленного изображения на главный дисплей
		};

	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 5;
			p->ystep = 3;	// количество ячеек разметки на одну строку меню
			p->reverse = 1;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

	#else /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */
		// TFT панель 320 * 240
		// для Аиста
		enum
		{
			BDCV_ALLRX = 9,	// общая высота, отведенная под S-метр, панораму, водопад и иные отображения

//			BDCO_SPMRX = 0,	// смещение спектра по вертикали в ячейках от начала общего поля
//			BDCV_SPMRX = 4,	// вертикальный размер спектра в ячейках
//
//			BDCO_WFLRX = 4,	// смещение водопада по вертикали в ячейках от начала общего поля
//			BDCV_WFLRX = 5,	// вертикальный размер водопада в ячейках

			BDTH_ALLRX = 26,	// ширина зоны для отображение полосы на индикаторе
			BDTH_RIGHTRX = 16,	// ширина индикатора плюсов
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
			PATTERN_BAR_FULL = 0x7e,
			PATTERN_BAR_HALF = 0x3c,
			PATTERN_BAR_EMPTYFULL = 0x00,
			PATTERN_BAR_EMPTYHALF = 0x00
		};
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале

		enum
		{
			DPAGE0,					// Страница, в которой отображаются основные (или все)
		#if WITHIF4DSP
			DPAGE1,					// Страница, в которой отображается спектр иводопад
		#endif /* WITHIF4DSP */
			DISPLC_MODCOUNT
		};
		enum {
			PG0 = REDRSUBSET(DPAGE0),
		#if WITHIF4DSP
			PG1 = REDRSUBSET(DPAGE1),
			PGALL = PG0 | PG1 | REDRSUBSET_MENU,
			PGNOMEMU = PG0 | PG1,
		#else /* WITHIF4DSP */
			PGALL = PG0 | REDRSUBSET_MENU,
			PGNOMEMU = PG0,
		#endif /* WITHIF4DSP */
			PGLATCH = REDRSUBSET_LATCH,	// страницы, на которых возможно отображение водопада или панорамы.
			PGINI = REDRSUBSET_INIT,
			PGunused
		};
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
		#define DISPLC_RJ		1	// количество скрытых справа цифр в отображении частоты
		static const dzone_t dzones [] =
		{
				{	0,	0,	0, 0, display2_preparebg,	& dzi_default, REDRSUBSET_SHOW, }, // Стирание фона
			{	2,	0,	display_txrxstate2, & dzi_txrx, PGALL, },
			{	3,	0,	display2_voxtune3,	& dzi_default, PGALL, },
			{	7,	0,	display2_att4,		& dzi_attenuator, PGALL, },
			{	12, 0,	display_pre3,		& dzi_default, PGALL, },
		#if WITHDSPEXTDDC
			{	16, 0,	display2_ovf3,		& dzi_default, PGALL, },	// ovf/pre
		#endif /* WITHDSPEXTDDC */
			{	20, 0,	display2_lockstate4, & dzi_default, PGALL, },
			{	25, 0,	display2_agc3,		& dzi_default, PGALL, },
			{	29, 0,	display2_rxbw3,		& dzi_rxbw, PGALL, },

			{	0,	8,	display_freqXbig_a, & dzi_default, PGALL, },
			{	29, 8,	display2_mode3_a,	& dzi_modea,	PGALL, },	// SSB/CW/AM/FM/...
			//---
			{	0,	15,	display_vfomode5,	& dzi_default, PGALL, },	// SPLIT
			{	6,	15,	display2_freqX_b,	& dzi_default, PGALL, },
		#if WITHUSEDUALWATCH
			{	25, 15,	display2_mainsub3,	& dzi_default, PGNOMEMU, },	// main/sub RX
		#endif /* WITHUSEDUALWATCH */
			{	29, 15,	display2_mode3_b,	& dzi_modeb,	PGALL, },	// SSB/CW/AM/FM/...
			//---
			{	0,	18,	display2_bars,		& dzi_default, PG0, },	// S-METER, SWR-METER, POWER-METER
		#if WITHIF4DSP
			{	0,	18,	display2_wfl_init,	& dzi_default,	PGINI, },	// формирование палитры водопада
			{	0,	18,	display2_latchcombo,	& dzi_default,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
			{	0,	18,	display2_combo,	& dzi_default, PG1, },// подготовка изображения спектра

			{	27, 18,	display_smeter5,	& dzi_default, PGNOMEMU, },	// signal level
		#endif /* WITHIF4DSP */
			//---
			//{	22, 25,	display_samfreqdelta8, & dzi_default, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */

		#if WITHVOLTLEVEL
			{	0, 28,	display2_voltlevelV5, & dzi_voltlevel, PGALL, },	// voltmeter with "V"
		#endif /* WITHVOLTLEVEL */
		#if WITHCURRLEVEL || WITHCURRLEVEL2
			{	6, 28,	display2_currlevelA6, & dzi_currlevel, PGALL, },	// amphermeter with "A"
		#endif /* WITHCURRLEVEL || WITHCURRLEVEL2 */
		#if defined (RTC1_TYPE)
			{	13, 28,	display_time8,		& dzi_default, PGALL, },	// TIME
		#endif /* defined (RTC1_TYPE) */
		#if WITHUSEAUDIOREC
			{	25, 28,	display2_rec3,		& dzi_default, PGALL, },	// Отображение режима записи аудио фрагмента
		#endif /* WITHUSEAUDIOREC */
		#if WITHMENU
			{	0,				DLES,	BDTH_ALLRX, 5, display2_multilinemenu_block,	& dzi_compat, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
		#endif /* WITHMENU */
			{	0,	0,	0, 0, display2_showmain,	& dzi_default, REDRSUBSET_SHOW, }, // запись подготовленного изображения на главный дисплей
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

	#endif /* DSTYLE_UR3LMZMOD && WITHONEATTONEAMP */

