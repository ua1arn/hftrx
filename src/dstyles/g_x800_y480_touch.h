	// Вариант с сенсорным экраном
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
		BDTH_ALLRX = 50, //DIM_X / GRID2X(1),	// ширина зоны для отображение графического окна на индикаторе
		BDCV_ALLRX = ROWS2GRID(61),	// количество строк, отведенное под S-метр, панораму, иные отображения

		BDTH_LEFTRX = 17,	// ширина индикатора баллов (без плюсов)
		BDTH_RIGHTRX = BDTH_ALLRXBARS - BDTH_LEFTRX,	// ширина индикатора плюсов
		BDTH_SPACERX = 0,
		BDTH_ALLSWR = 16,
		BDTH_SPACESWR = 2,
		BDTH_ALLPWR = 12,
		BDTH_SPACEPWR = 0,

		/* совмещение на одном экрание водопада и панорамы */
		BDCO_SPMRX = ROWS2GRID(0),	// смещение спектра по вертикали в ячейках от начала общего поля
		BDCV_SPMRX = ROWS2GRID(27)	// вертикальный размер спектра в ячейках
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
		PGLATCH = PGALL | REDRSUBSET_SLEEP,	// страницы, на которых возможно отображение водопада или панорамы.
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

	// 480/5 = 96, 800/16=50
	// 272/5 = 54, 480/16=30 (old)
	//#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	//#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */
	//#define SMALLCHARH 15 /* Font height */
	//#define SMALLCHARW 16 /* Font width */
	static const FLASHMEM struct dzone dzones [] =
	{
		{	0,	0,	0,	0,	display2_clearbg, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
		{	0,	0,	0,	0,	display2_keyboard_screen0,	REDRM_KEYB, PGALL | REDRSUBSET_SLEEP, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана
		//{	10,	0,	6,	0,	display2_rxbwval6alt,	REDRM_MODE, PGALL, },	// RX BW value
		{	17,	0,	2,	0,	display_txrxstate2, REDRM_MODE, PGALL, },
		{	20,	0,	5,	0,	display2_ant5,		REDRM_MODE, PGALL, },
		{	26,	0,	4,	0,	display2_att4,		REDRM_MODE, PGALL, },
		{	31,	0,	3,	0,	display2_preovf3,	REDRM_BARS, PGALL, },
		{	35,	0,	1,	0,	display2_genham1,	REDRM_BARS, PGALL, },	// Отображение режима General Coverage / HAM bands
		{	38,	0,	4,	0,	display2_lockstate4, REDRM_MODE, PGALL, },	// LOCK
		{	43, 0,	5,	0,	display2_notch5,	REDRM_MODE, PGALL, },	// NOTCH on/off
		{	43, 5,	3,	0,	display2_voxtune3,	REDRM_MODE, PGALL, },	// VOX
		{	47, 5,	3,	0,	display2_datamode3,	REDRM_MODE, PGALL, },	// DATA mode indicator
		{	47, 15,	3,	0,	display2_usbsts3,	REDRM_BARS, PGALL, },	// USB host status
		{	47, 20,	3,	0,	display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента
#if WITHBARS
		{    0, 4,  0,	0,	display2_smeter15_init,REDRM_INIS, PGINI, },	//  Инициализация стрелочного прибора
		{    0, 4,  15,	0,	display2_smeter15, 	REDRM_BARS, PGALL, },	// Изображение стрелочного прибора
#endif /* WITHBARS */
#if WITHAFSPECTRE
		{	0,	4,	0,	0,	display2_af_spectre15_init,	REDRM_INIS, PGINI, },
		{	0,	4,	0,	0,	display2_af_spectre15_latch,	REDRM_BARS,	PGLATCH, },
		{	0,	4,	15,	0,	display2_af_spectre15,		REDRM_BARS, PGSPE, },
#endif /* WITHAFSPECTRE */

//		{   47, 20, 3,	0,	display2_bkin3,		REDRM_MODE, PGALL, },
	#if WITHENCODER2
//		{	41, 0,	9,	0,	display2_fnlabel9,	REDRM_MODE, PGALL, },	// FUNC item label
//		{	41,	4,	9,	0,	display2_fnvalue9,	REDRM_MODE, PGALL, },	// FUNC item value
//		{	45, 15,	5,	0,	display2_notch5,		REDRM_MODE, PGALL, },	// NOTCH on/off
	#else /* WITHENCODER2 */
		{	45, 0,	5,	0,	display2_notch5,		REDRM_MODE, PGALL, },	// FUNC item label
		{	45,	4,	5,	0,	display2_notchfreq5,	REDRM_BARS, PGALL, },	// FUNC item value
	#endif /* WITHENCODER2 */

//		{	46, 20,	3,	0,	display2_agc3,		REDRM_MODE, PGALL, },	// AGC mode
//		{	46, 25,	3,	0,	display2_voxtune3,	REDRM_MODE, PGALL, },	// VOX
//		{	46, 30,	3,	0,	display2_datamode3,	REDRM_MODE, PGALL, },	// DATA mode indicator
//		{	46, 35,	3,	0,	display2_nr3,		REDRM_MODE, PGALL, },	// NR
		{	43, 15,	3,	0,	display2_atu3,		REDRM_MODE, PGALL, },	// TUNER state (optional)
		{	43, 20,	3,	0,	display2_byp3,		REDRM_MODE, PGALL, },	// TUNER BYPASS state (optional)

		{	17,	7,	0,	0,	display2_freqX_a_init,	REDRM_INIS, PGINI, },	// MAIN FREQ Частота (большие цифры)
		{	17,	7,	0,	0,	display2_freqX_a,	REDRM_FREQ, PGALL, },	// MAIN FREQ Частота Герцы маленьким шрифтом.
		{	38, 10,	3,	0,	display2_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		{	43, 10,	3,	0,	display2_rxbw3,		REDRM_MODE, PGALL, },	// 3.1 / 0,5 / WID / NAR
		{	47, 10,	3,	0,	display2_nr3,		REDRM_MODE, PGALL, },	// NR : was: AGC
		{	38, 15,	3,	0,	display2_mainsub3,	REDRM_MODE, PGALL, },	// main/sub RX: A/A, A/B, B/A, etc

		{	43,	15,	3,	0,	display2_vfomode3,	REDRM_MODE, PGALL, },	// SPLIT
		{	26,	20,	0,	0,	display2_freqX_b,	REDRM_FRQB, PGALL, },	// SUB FREQ
		{	38, 20,	3,	0,	display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...

#if 1
		// на освободившеемя место добавить статусную строку с различной информацией
//		{	0,	25,	0,	0,	display2_legend,	REDRM_MODE, PGSWR, },	// Отображение оцифровки шкалы S-метра
//		{	0,	30,	0,	0,	display2_bars,		REDRM_BARS, PGSWR, },	// S-METER, SWR-METER, POWER-METER
		{	0,	0, 4,	0,	display2_siglevel4, 	REDRM_BARS, PGSWR, },	// signal level dBm
//		{	36, 30,	8,	0,	display2_freqdelta8, REDRM_BARS, PGSWR, },	// выход ЧМ демодулятора
	#if WITHSPECTRUMWF || WITHAFSPECTRE
		{	0,	DLES,	0,	0,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
		{	0,	DLES,	0,	0,	display2_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
		{	0,	DLES,	0,	0,	display2_spectrum,	REDRM_BARS, PGSPE, },// подготовка изображения спектра
		{	0,	DLES,	0,	0,	display2_waterfall,	REDRM_BARS, PGWFL, },// подготовка изображения водопада
	#endif /* WITHSPECTRUMWF */
#else
		{	0,	25,	0,	0,	display2_adctest,	REDRM_BARS, PGSWR, },	// ADC raw data print
#endif
	#if WITHAMHIGHKBDADJ
		//{	XX, DLE1,	0,	0,	display_amfmhighcut4,REDRM_MODE, PGALL, },	// 3.70
	#endif /* WITHAMHIGHKBDADJ */
		//{	XX,	DLE1,	8,	0,	display_samfreqdelta8, REDRM_BARS, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */
		// sleep mode display
		{	5,	25,	12,	0,	display2_datetime12, REDRM_BARS, PGSLP, },	// DATE & TIME // DATE&TIME Jan-01 13:40
		{	20, 25,	5,	0,	display2_voltlevelV5, REDRM_VOLT, PGSLP, },	// voltmeter with "V"
		{	0,	DLES, 0,	0,	gui_WM_walkthrough, REDRM_BARS, PGWFL | PGSPE, },

		{	0,	0, 0,	0,	display2_keyboard_menu, REDRM_KEYB, REDRSUBSET_MENU, }, // Обработка клавиатуры и валкодеров при нахождении в режиме меню

		{	0,	0,	0,	0,	display2_nextfb, REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
	};

#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		p->multilinemenu_max_rows = 15;
		p->ystep = 4;	// количество ячеек разметки на одну строку меню
		p->reverse = 1;
		p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

	/* получить координаты окна с панорамой и/или водопадом. */
	void display2_getpipparams(pipparams_t * p)
	{
		p->x = 0; //GRID2X(0);	// позиция верхнего левого угла в пикселях
		p->y = GRID2Y(DLES);	// позиция верхнего левого угла в пикселях
		p->w = DIM_X; //GRID2X(CHARS2GRID(BDTH_ALLRX));	// размер по горизонтали в пикселях
		p->h = GRID2Y(BDCV_ALLRX);				// размер по вертикали в пикселях
	}
