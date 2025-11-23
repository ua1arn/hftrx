
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

	static const dzone_t dzones [] =
	{
			{	0,	0,	0, 0, display2_preparebg,	& dzi_default, REDRSUBSET_SHOW, }, // Стирание фона
			{	0,	0,	2,	0,	display_txrxstate2, & dzi_txrx, PGALL, },
			{	3,	0,	5,	0,	display2_ant5,		& dzi_antenna, PGALL, },
			{	9,	0,	4,	0,	display2_att4,		& dzi_attenuator, PGALL, },
			{	14,	0,	3,	0,	display2_preovf3,	& dzi_default, PGALL, },
			{	18,	0,	1,	0,	display_lockstate1,	& dzi_default, PGALL, },	// LOCK (*)

		#if WITHENCODER2
			{	21, 0,	9,	0,	display2_fnlabel9,	& dzi_default, PGALL, },	// FUNC item label
			{	21,	4,	9,	0,	display2_fnvalue9,	& dzi_default, PGALL, },	// FUNC item value
			{	25, 12,	5,	0,	display2_notch5,		& dzi_default, PGALL, },	// NOTCH on/off
		#else /* WITHENCODER2 */
			{	25, 0,	5,	0,	display2_notch5,		& dzi_default, PGALL, },	// FUNC item label
			{	25,	4,	5,	0,	display2_notchfreq5,	& dzi_default, PGALL, },	// FUNC item value
		#endif /* WITHENCODER2 */

			{	26, 16,	3,	0,	display2_nr3,		& dzi_default, PGALL, },	// NR
	//		{	26,	16,	3,	0,	display2_agc3,		& dzi_default, PGALL, },	// AGC mode
			{	26,	20,	3,	0,	display2_voxtune3,	& dzi_default, PGALL, },	// VOX

			{	0,	4,	0,	0,	display2_freqX_a_init,	& dzi_default, PGINI, },	// MAIN FREQ Частота (большие цифры)
			{	0,	4,	0,	0,	display2_freqX_a,	& dzi_freqa, PGALL, },	// MAIN FREQ Частота Герцы маленьким шрифтом.
			{	21,	8,	3,	0,	display2_mode3_a,	& dzi_modea,	PGALL, },	// SSB/CW/AM/FM/...
			{	21,	12,	3,	0,	display2_rxbw3,		& dzi_rxbw, PGALL, },	// 3.1 / 0,5 / WID / NAR
			{	26,	8,	3,	0,	display2_datamode3,	& dzi_default, PGALL, },	// DATA mode indicator

			{	0,	16,	3,	0,	display2_rec3,		& dzi_default, PGALL, },	// Отображение режима записи аудио фрагмента
			{	0,	16,	3,	0,	display2_mainsub3,	& dzi_default, PGALL, },	// main/sub RX: A/A, A/B, B/A, etc

			{	5,	16,	3,	0,	display2_vfomode3,	& dzi_default, 	PGALL, },	// SPLIT - не очень нужно при наличии индикации на A/B (display2_mainsub3) яркостью.
			{	9,	16,	0,	0,	display2_freqX_b,	& dzi_freqb, 	PGALL, },	// SUB FREQ
			{	21,	16,	3,	0,	display2_mode3_b,	& dzi_modeb,	PGALL, },	// SSB/CW/AM/FM/...

	#if 1
	        {	0,  20,	0,	0,	display2_legend_rx, & dzi_default, PGSWR, },    // Отображение оцифровки шкалы S-метра
	        {	0,  24,	0,	0,	display2_bars_rx,   & dzi_default, PGSWR, },    // S-METER, SWR-METER, POWER-METER
			{	25, 24, 5,	0,	display2_smeors5, 	& dzi_default, PGSWR, },	 // уровень сигнала в баллах S или dBm
	        {	0,  28,	5,	0,	display2_legend_tx, & dzi_default, PGSWR, },    // Отображение оцифровки шкалы PWR & SWR-метра
	        {	0,  32,	0,	0,	display2_bars_tx,   & dzi_default, PGSWR, },    // S-METER, SWR-METER, POWER-METER

//			{	0,	28,	0,	BDCV_ALLRX,	display2_wfl_init,	& dzi_default,	PGINI, },	// формирование палитры водопада
//			{	0,	28,	0,	0,	display2_latchcombo,	& dzi_default,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
//			{	0,	28,	0,	BDCV_ALLRX,	display2_combo,	& dzi_default, PGSPE, },// подготовка изображения спектра
	#else
			{	0,	20,	0,	0,	display2_adctest,	& dzi_default, PGSWR, },	// ADC raw data print
	#endif

			//{	0,	51,	8,	0,	display_samfreqdelta8, & dzi_default, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */
			{	0,	51,	5,	0,	display_time5,		& dzi_default, PGALL,	},	// TIME
			{	6, 	51,	3,	0,	display2_atu3,		& dzi_tune, PGALL, },	// TUNER state (optional)
			{	10, 51,	3,	0,	display2_byp3,		& dzi_bypass, PGALL, },	// TUNER BYPASS state (optional)
			{	14, 51,	6,	0,	display2_thermoC,	& dzi_default, PGALL, },	// thermo sensor 20.7C
			{	19, 51,	5,	0,	display2_currlevel, & dzi_default, PGALL, },	// PA drain current d.dd without "A"
			{	25, 51,	5,	0,	display2_voltlevelV, & dzi_voltlevel, PGALL, },	// voltmeter with "V"
		#if WITHAMHIGHKBDADJ
			{	25, 51,	5,	0,	display_amfmhighcut5,& dzi_default, PGALL, },	// 13.70
		#endif /* WITHAMHIGHKBDADJ */

			// sleep mode display
			{	5,	24,	12,	0,	display2_datetime12,	& dzi_datetime12, PGSLP, },	// DATE & TIME // DATE&TIME Jan-01 13:40
			{	20, 24,	5,	0,	display2_voltlevelV, 	& dzi_voltlevel, PGSLP, },	// voltmeter with "V"

		#if WITHMENU
			{	0, MENU1ROW,	BDTH_ALLRX, (54 - MENU1ROW), display2_multilinemenu_block,	& dzi_compat, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
			#if WITHAUTOTUNER
			{	0,  		51,	20,	5,	display2_swrsts20,	& dzi_default, REDRSUBSET_MENU, },	// SWR METER display
			#endif /* WITHAUTOTUNER */
		#endif /* WITHMENU */
			{	0,	0,	0, 0, display2_showmain,	& dzi_default, REDRSUBSET_SHOW, }, // запись подготовленного изображения на главный дисплей
		};

	#if WITHMENU
		void display2_getmultimenu(multimenuwnd_t * p)
		{
			enum { YSTEP = 4 };		// количество ячеек разметки на одну строку меню
			p->multilinemenu_max_rows = (54 - MENU1ROW) / YSTEP;
			p->ystep = YSTEP;	// количество ячеек разметки на одну строку меню
			p->reverse = 1;
			p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
			p->xspan = BDTH_ALLRX;	/* количество знакомест по горизонтали, отдаваемое под меню */
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
	#define DISPLC_RADIUS 	0	// радиус закругления углов плиток в dzones
