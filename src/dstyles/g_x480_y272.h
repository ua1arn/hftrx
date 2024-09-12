
	// TFT панель SONY PSP-1000
	// 272/5 = 54, 480/16=30
	// без панорамы и водопада

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
		BDTH_ALLRX = 24,	// ширина зоны для отображение полосы на индикаторе
		BDTH_LEFTRX = 12,	// ширина индикатора баллов
		BDTH_RIGHTRX = BDTH_ALLRX - BDTH_LEFTRX,	// ширина индикатора плюсов
		BDTH_SPACERX = 0,
	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		BDTH_ALLSWR = 13,
		BDTH_SPACESWR = 2,
		BDTH_ALLPWR = 9,
		BDTH_SPACEPWR = 0,
	#else /* WITHSHOWSWRPWR */
		BDTH_ALLSWR = BDTH_ALLRX,
		BDTH_SPACESWR = BDTH_SPACERX,
		BDTH_ALLPWR = BDTH_ALLRX,
		BDTH_SPACEPWR = BDTH_SPACERX,
	#endif /* WITHSHOWSWRPWR */

		BDCV_ALLRX = ROWS2GRID(20),	// количество ячееек, отведенное под S-метр, панораму, иные отображения
	};

	enum
	{
		PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
		PATTERN_BAR_FULL = 0xFF,
		PATTERN_BAR_HALF = 0x3c,
		PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
		PATTERN_BAR_EMPTYHALF = 0x00	//0x00
	};

		/* совмещение на одном экрание водопада и панорамы */
		enum
		{
			DPAGE0,					// Страница, в которой отображаются основные (или все)
			DISPLC_MODCOUNT
		};

		enum
		{
			PG0 = REDRSUBSET(DPAGE0),
			PGNOMEMU = PG0,
			PGSWR = PG0,	// страница отоюражения S-meter и SWR-meter
			PGALL = PG0 | REDRSUBSET_MENU,
			PGSLP = REDRSUBSET_SLEEP,
			PGINI = REDRSUBSET_INIT,
			PGunused
		};

	#if 1//TUNE_TOP > 100000000uL
		#define DISPLC_WIDTH	9	// количество цифр в отображении частоты
	#else
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
	#endif
	#define DISPLC_RJ		0	// количество скрытых справа цифр в отображении частоты

	static const FLASHMEM struct dzone dzones [] =
	{
			{	0,	0,	display2_clearbg, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
			{	0,	0,	display2_keyboard_screen0,	REDRM_KEYB, PGALL | REDRSUBSET_SLEEP, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана
			{	0,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
			{	3,	0,	display2_ant5,		REDRM_MODE, PGALL, },
			{	9,	0,	display2_att4,		REDRM_MODE, PGALL, },
			{	14,	0,	display2_preovf3,	REDRM_BARS, PGALL, },
			{	18,	0,	display_lockstate1,	REDRM_BARS, PGALL, },	// LOCK (*)

		#if WITHENCODER2
			{	21, 0,	display2_fnlabel9,	REDRM_MODE, PGALL, },	// FUNC item label
			{	21,	4,	display2_fnvalue9,	REDRM_MODE, PGALL, },	// FUNC item value
			{	25, 12,	display2_notch5,		REDRM_MODE, PGALL, },	// NOTCH on/off
		#else /* WITHENCODER2 */
			{	25, 0,	display2_notch5,		REDRM_MODE, PGALL, },	// FUNC item label
			{	25,	4,	display2_notchfreq5,	REDRM_BARS, PGALL, },	// FUNC item value
		#endif /* WITHENCODER2 */

			{	26, 16,	display2_nr3,		REDRM_MODE, PGALL, },	// NR
	//		{	26,	16,	display2_agc3,		REDRM_MODE, PGALL, },	// AGC mode
			{	26,	20,	display2_voxtune3,	REDRM_MODE, PGALL, },	// VOX

			{	0,	4,	display2_freqX_a_init,	REDRM_INIS, PGINI, },	// MAIN FREQ Частота (большие цифры)
			{	0,	4,	display2_freqX_a,	REDRM_FREQ, PGALL, },	// MAIN FREQ Частота (большие цифры)
			{	21,	8,	display2_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
			{	21,	12,	display2_rxbw3,		REDRM_MODE, PGALL, },	// 3.1 / 0,5 / WID / NAR
			{	26,	8,	display2_datamode3,	REDRM_MODE, PGALL, },	// DATA mode indicator

			{	0,	16,	display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента
			{	0,	16,	display2_mainsub3,	REDRM_MODE, PGALL, },	// main/sub RX: A/A, A/B, B/A, etc

			{	5,	16,	display2_vfomode3,	REDRM_MODE, PGALL, },	// SPLIT - не очень нужно при наличии индикации на A/B (display2_mainsub3) яркостью.
			{	9,	16,	display2_freqX_b,	REDRM_FRQB, PGALL, },	// SUB FREQ
			{	21,	16,	display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...

	#if 1
	        {	0,  20,	display2_legend_rx, REDRM_MODE, PGSWR, },    // Отображение оцифровки шкалы S-метра
	        {	0,  24,	display2_bars_rx,   REDRM_BARS, PGSWR, },    // S-METER, SWR-METER, POWER-METER
			{	25, 24, display2_smeors5, 	REDRM_BARS, PGSWR, },	 // уровень сигнала в баллах S или dBm
	        {	0,  28,	display2_legend_tx, REDRM_MODE, PGSWR, },    // Отображение оцифровки шкалы PWR & SWR-метра
	        {	0,  32,	display2_bars_tx,   REDRM_BARS, PGSWR, },    // S-METER, SWR-METER, POWER-METER

//			{	0,	28,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
//			{	0,	28,	display2_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
//			{	0,	28,	display2_spectrum,	REDRM_BARS, PGSPE, },// подготовка изображения спектра
//			{	0,	28,	display2_waterfall,	REDRM_BARS, PGWFL, },// подготовка изображения водопада
	#else
			{	0,	20,	display2_adctest,	REDRM_BARS, PGSWR, },	// ADC raw data print
	#endif

			//{	0,	51,	display_samfreqdelta8, REDRM_BARS, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */
			{	0,	51,	display_time5,		REDRM_BARS, PGALL,	},	// TIME
			{	6, 	51,	display2_atu3,		REDRM_MODE, PGALL, },	// TUNER state (optional)
			{	10, 51,	display2_byp3,		REDRM_MODE, PGALL, },	// TUNER BYPASS state (optional)
			{	14, 51,	display2_thermo5,	REDRM_VOLT, PGALL, },	// thermo sensor 20.7C
			{	19, 51,	display2_currlevel5, REDRM_VOLT, PGALL, },	// PA drain current d.dd without "A"
			{	25, 51,	display2_voltlevelV5, REDRM_VOLT, PGALL, },	// voltmeter with "V"
		#if WITHAMHIGHKBDADJ
			{	25, 51,	display_amfmhighcut5,REDRM_MODE, PGALL, },	// 13.70
		#endif /* WITHAMHIGHKBDADJ */

			// sleep mode display
			{	5,	24,	display2_datetime12,	REDRM_BARS, PGSLP, },	// DATE & TIME // DATE&TIME Jan-01 13:40
			{	20, 24,	display2_voltlevelV5, REDRM_VOLT, PGSLP, },	// voltmeter with "V"

		#if WITHMENU
			{	0,				0,		display2_keyboard_menu,					REDRM_KEYB, REDRSUBSET_MENU, }, // Обработка клавиатуры и валкодеров при нахождении в режиме меню
			{	1,	25,	display2_multilinemenu_block_groups,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
			{	LABELW + 1,	25,	display2_multilinemenu_block_params,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
			{	LABELW * 2 + 2,	25,	display2_multilinemenu_block_vals,	REDRM_MVAL, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
		#endif /* WITHMENU */
			{	0,	0,	display2_nextfb, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
		};

	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			p->multilinemenu_max_rows = 6;
			p->ystep = 4;	// количество ячеек разметки на одну строку меню
			p->reverse = 1;
			p->valuew = 10;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		}
	#endif /* WITHMENU */

		/* получить координаты окна с панорамой и/или водопадом. */
		void display2_getpipparams(pipparams_t * p)
		{
			p->x = 0; //GRID2X(0);	// позиция верхнего левого угла в пикселях
			p->y = GRID2Y(28);	// позиция верхнего левого угла в пикселях
			p->w = DIM_X; //GRID2X(CHARS2GRID(BDTH_ALLRX));	// размер по горизонтали в пикселях
			p->h = GRID2Y(BDCV_ALLRX);				// размер по вертикали в пикселях
		}
