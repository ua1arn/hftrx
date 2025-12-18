	// вариант без сенсорного экрана и без offscreen composition
	// Для тестирования на минимальном объеме памяти.
	// TFT панель AT070TN90
	// 480/5 = 96, 800/16=50

	#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
		//					"012345678901234567890123456789"
		#define SWRPWRMAP	"1    2    3    4  0%   |  100%"
		#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
	#else
		#warning Should be defined WITHSHOWSWRPWR
	#endif
	//						"012345678901234567890123456789"
	#define SMETERMAP		"1   3   5   7   9  +20 +40 +60"
	enum
	{
		BDTH_ALLRXBARS = 30,	// ширина зоны для отображение барграфов на индикаторе
#if 1
		BDTH_ALLRX = 50, //DIM_X / GRID2X(1),	// ширина зоны для отображение графического окна на индикаторе
		BDCV_ALLRX = (49),	// количество строк, отведенное под S-метр, панораму, иные отображения
#else
		BDTH_ALLRX = 40,	// ширина зоны для отображение графического окна на индикаторе
		BDCV_ALLRX = (55),	// количество строк, отведенное под S-метр, панораму, иные отображения
#endif

		BDTH_LEFTRX = 17,	// ширина индикатора баллов (без плюсов)
		BDTH_RIGHTRX = BDTH_ALLRXBARS - BDTH_LEFTRX,	// ширина индикатора плюсов
		BDTH_SPACERX = 0,
		BDTH_ALLSWR = 16,
		BDTH_SPACESWR = 2,
		BDTH_ALLPWR = 12,
		BDTH_SPACEPWR = 0,
		//
		B_unused
	};
	enum {
		DLES = 35,		// spectrum window upper line
        DLE1 = 93,		//
		DLE_unused
	};


	enum
	{
		PATTERN_SPACE = 0x00,	/* очищаем место за SWR и PWR метром этим символом */
		PATTERN_BAR_FULL = 0xFF,
		PATTERN_BAR_HALF = 0x3c,
		PATTERN_BAR_EMPTYFULL = 0x00,	//0x00
		PATTERN_BAR_EMPTYHALF = 0x00	//0x00
	};

	enum
	{
		DPAGE0,					// Страница, в которой отображаются основные (или все)
		DISPLC_MODCOUNT
	};

	enum
	{
		PG0 = REDRSUBSET(DPAGE0),
		PGALL = PG0 | REDRSUBSET_MENU,
		PGWFL = PG0,	// страница отображения водопада
		PGSPE = PG0,	// страница отображения панорамы
		PGSWR = PG0,	// страница отоюражения S-meter и SWR-meter
		PGLATCH = REDRSUBSET_LATCH,	// страницы, на которых возможно отображение водопада или панорамы.
		PGSLP = REDRSUBSET_SLEEP,
		PGunused
	};

	#if 1//TUNE_TOP > 100000000uL
		#define DISPLC_WIDTH	9	// количество цифр в отображении частоты
	#else
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
	#endif
	#define DISPLC_RJ		0	// количество скрытых справа цифр в отображении частоты

	// 480/5 = 96, 800/16=50
	// 272/5 = 54, 480/16=30 (old)
	//#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	//#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */
	
	
	static const dzone_t dzones [] =
	{
		{	0,	0,	0, 0, display2_preparebg,	& dzi_default, REDRSUBSET_SHOW, }, // Стирание фона

		{	0,	0,	2,	0,	display_txrxstate2, * dzi_txrx, PGALL, },
		{	3,	0,	5,	0,	display2_ant5,		REDRM_MODE, PGALL, },
		{	9,	0,	4,	0,	display2_att4,		REDRM_MODE, PGALL, },
		{	14,	0,	3,	0,	display2_preovf3,	REDRM_BARS, PGALL, },
		{	18,	0,	1,	0,	display2_genham1,	REDRM_BARS, PGALL, },	// Отображение режима General Coverage / HAM bands
		{	21,	0,	4,	0,	display2_lockstate4, REDRM_MODE, PGALL, },	// LOCK

	#if WITHENCODER2
		{	41, 0,	9,	8,	display2_fnblock9,	REDRM_MODE, PGALL, },	// FUNC menu item label & value
		{	45, 15,	5,	0,	display2_notch5,		REDRM_MODE, PGALL, },	// NOTCH on/off
	#else /* WITHENCODER2 */
		{	45, 0,	5,	0,	display2_notch5,		REDRM_MODE, PGALL, },	// FUNC item label
		{	45,	4,	5,	0,	display2_notchfreq5,	REDRM_BARS, PGALL, },	// FUNC item value
	#endif /* WITHENCODER2 */

		{	34, 20,	3,	0,	display2_atu3,		REDRM_MODE, PGALL, },	// TUNER state (optional)
		{	38, 20,	3,	0,	display2_byp3,		REDRM_MODE, PGALL, },	// TUNER BYPASS state (optional)
		{   42, 20, 3,	0,	display2_bkin3,		REDRM_MODE, PGALL, },
		{	46, 20,	3,	0,	display2_voxtune3,	REDRM_MODE, PGALL, },	// VOX

		{	38, 25,	3,	0,	display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента

		{	42, 25,	3,	0,	display2_datamode3,	REDRM_MODE, PGALL, },	// DATA mode indicator
		{	46, 25,	3,	0,	display2_nr3,		REDRM_MODE, PGALL, },	// NR : was: AGC

		{	0,	7,	0,	0,	display2_freqX_a_init,	REDRM_INIS, PGINI, },	// MAIN FREQ Частота (большие цифры)
		{	0,	7,	21,	11,	display2_freqX_a,	& dzi_freqa, PGALL, },	// MAIN FREQ Частота Герцы маленьким шрифтом.
		{	21, 10,	3,	0,	display2_mode3_a,	& dzi_modea,	PGALL, },	// SSB/CW/AM/FM/...
		{	26, 10,	3,	0,	display2_rxbw3,		REDRM_MODE, PGALL, },	// 3.1 / 0,5 / WID / NAR
		{	21, 15,	3,	0,	display2_mainsub3,	REDRM_MODE, PGALL, },	// main/sub RX: A/A, A/B, B/A, etc

		{	26,	15,	3,	0,	display2_vfomode3,	REDRM_MODE, PGALL, },	// SPLIT
		{	9,	20,	0,	0,	display2_freqX_b,	REDRM_FRQB, PGALL, },	// SUB FREQ
		{	21, 20,	3,	0,	display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...

#if 1
		{	0,	25,	0,	0,	display2_legend,	REDRM_MODE, PGSWR, },	// Отображение оцифровки шкалы S-метра
		{	0,	30,	0,	0,	display2_bars,		REDRM_BARS, PGSWR, },	// S-METER, SWR-METER, POWER-METER
		//{	0,	35,	0,	0,	display2_legend_tx,	REDRM_MODE, PGSWR, },	// Отображение оцифровки шкалы PWR & SWR-метра
		//{	0,	40,	0,	0,	display2_bars_tx,	REDRM_BARS, PGSWR, },	// S-METER, SWR-METER, POWER-METER
		{	31,	30, 4,	0,	display2_siglevel4, 	REDRM_BARS, PGSWR, },	// signal level dBm
		{	36, 30,	8,	0,	display2_freqdelta8, REDRM_BARS, PGSWR, },	// выход ЧМ демодулятора
		{	46, 30,	3,	0,	display2_agc3,		REDRM_MODE, PGALL, },	// AGC mode
	#if WITHSPECTRUMWF
		{	0,	DLES,	50,	BDCV_ALLRX,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
		{	0,	DLES,	50,	BDCV_ALLRX,	display2_latchcombo,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
		{	0,	DLES,	50,	BDCV_ALLRX,	display2_combo,	REDRM_BARS, PGSPE, },// подготовка изображения спектра
	#endif /* WITHSPECTRUMWF */
#else
		{	0,	25,	0,	0,	display2_adctest,	REDRM_BARS, PGSWR, },	// ADC raw data print
#endif



		{	0,	DLE1,	12,	0,	display2_datetime12,	REDRM_BARS, PGALL,	},	// DATE&TIME Jan-01 13:40
		{	13,	DLE1,	9,	0,	display2_span9,		REDRM_MODE, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */
		{	23, DLE1,	0,	0,	display2_thermo,	REDRM_VOLT, PGALL, },	// thermo sensor
		{	28, DLE1,	3,	0,	display2_usbsts3,		REDRM_BARS, PGALL, },	// USB host status
		//{	28, DLE1,	0,	0,	display_freqmeter10, REDRM_BARS, PGALL, },	// измеренная частота опоры

		{	39, DLE1,	5,	0,	display2_currlevel, REDRM_VOLT, PGALL, },	// PA drain current d.dd without "A"
		{	45, DLE1,	5,	0,	display2_voltlevelV, REDRM_VOLT, PGALL, },	// voltmeter with "V"
	#if WITHAMHIGHKBDADJ
		//{	XX, DLE1,	display_amfmhighcut4,REDRM_MODE, PGALL, },	// 3.70
	#endif /* WITHAMHIGHKBDADJ */
		//{	XX,	DLE1,	display_samfreqdelta8, REDRM_BARS, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */

	#if WITHMENU
		{	0,				30,	BDTH_ALLRX, (DLE1 - DLES) - 1, display2_multilinemenu_block,	& dzi_compat, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
	#endif /* WITHMENU */

		// sleep mode display
		{	5,	25,	12,	0,	display2_datetime12,	REDRM_BARS, PGSLP, },	// DATE & TIME // DATE&TIME Jan-01 13:40
		{	20, 25,	5,	0,	display2_voltlevelV, REDRM_VOLT, PGSLP, },	// voltmeter with "V"

		{	0,	0,	0, 0, display2_showmain,	& dzi_default, REDRSUBSET_SHOW, }, // запись подготовленного изображения на главный дисплей
	};

#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		p->multilinemenu_max_rows = 15;
		p->ystep = 4;	// количество ячеек разметки на одну строку меню
		p->reverse = 1;
		p->valuew = 10;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		p->xspan = BDTH_ALLRX;	/* количество знакомест по горизонтали, отдаваемое под меню */
	}
#endif /* WITHMENU */

	/* получить координаты окна с панорамой и/или водопадом. */
	void display2_getpipparams(pipparams_t * p)
	{
		p->x = 0; //GRID2X(0);	// позиция верхнего левого угла в пикселях
		p->y = GRID2Y(DLES);	// позиция верхнего левого угла в пикселях
		p->w = DIM_X; //GRID2X((BDTH_ALLRX));	// размер по горизонтали в пикселях
		p->h = GRID2Y(BDCV_ALLRX);				// размер по вертикали в пикселях
	}
	#define DISPLC_RADIUS 	0	// радиус закругления углов плиток в dzones
