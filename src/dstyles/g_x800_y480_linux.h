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
		BDCV_ALLRX = ROWS2GRID(51),	// количество строк, отведенное под S-метр, панораму, иные отображения

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
		DPAGE1,					// служебная страница для полноэкранных окон touch GUI
		DISPLC_MODCOUNT
	};

	enum
	{
		PG0 = REDRSUBSET(DPAGE0),
		PG0_1 = PG0 | REDRSUBSET(DPAGE1),
		PGINI = REDRSUBSET_INIT,
		PGLATCH = REDRSUBSET_LATCH,

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
	static const dzone_t dzones [] =
	{
#if ! WITHLVGL
			{	0,	0,	0, 0, display2_preparebg,	& dzi_default, REDRSUBSET_SHOW, }, // Стирание фона

		//{	10,	0,	6,	0,	display2_rxbwval6alt,	& dzi_default, PG0, },	// RX BW value
		{	17,	0,	5,	0,	display_txrxstate5alt, & dzi_default, PG0, },
		{	21,	0,	7,	0,	display2_ant7alt,		& dzi_default, PG0, },
		{	26,	0,	5,	0,	display2_att5alt,		& dzi_default, PG0, },
		{	31,	0,	5,	0,	display2_preovf5alt,	& dzi_default, PG0, },
		{	35,	0,	1,	0,	display2_genham1,	& dzi_default, PG0, },	// Отображение режима General Coverage / HAM bands
		{	38,	0,	5,	0,	display2_lockstate5alt, & dzi_default, PG0, },	// LOCK
		{	42, 0,	7,	0,	display2_notch7alt,	& dzi_default, PG0, },	// NOTCH on/off
		{	42, 5,	3,	0,	display2_voxtune3,	& dzi_default, PG0, },	// VOX
		{	46, 5,	3,	0,	display2_datamode3,	& dzi_default, PG0, },	// DATA mode indicator
		{	46, 15,	3,	0,	display2_usbsts3,	& dzi_default, PG0, },	// USB host status
		{	46, 20,	3,	0,	display2_rec3,		& dzi_default, PG0, },	// Отображение режима записи аудио фрагмента

		{   0,  0,  0,	0,	layout_init,		& dzi_default, PGINI, },
//		{   47, 20, 3,	0,	display2_bkin3,		& dzi_default, PG0, },
//		{	46, 20,	3,	0,	display2_agc3,		& dzi_default, PG0, },	// AGC mode
//		{	46, 25,	3,	0,	display2_voxtune3,	& dzi_default, PG0, },	// VOX
//		{	46, 30,	3,	0,	display2_datamode3,	& dzi_default, PG0, },	// DATA mode indicator
//		{	46, 35,	3,	0,	display2_nr3,		& dzi_default, PG0, },	// NR
		{	42, 15,	4,	0,	display2_atu4alt,		& dzi_default, PG0, },	// TUNER state (optional)
		{	42, 20,	4,	0,	display2_byp4alt,		& dzi_default, PG0, },	// TUNER BYPASS state (optional)

		{	17,	7,	0,	0,	display2_freqX_a_init,	& dzi_default, PGINI, },	// MAIN FREQ Частота (большие цифры)
		{	17,	7,	0,	0,	display2_freqX_a,	& dzi_freqa, PG0, },	// MAIN FREQ Частота Герцы маленьким шрифтом.
		{	38, 10,	3,	0,	display2_mode3_a,	& dzi_modea,	PG0, },	// SSB/CW/AM/FM/...
		{	42, 10,	3,	0,	display2_rxbw3,		& dzi_rxbw, PG0, },	// 3.1 / 0,5 / WID / NAR
		{	46, 10,	3,	0,	display2_nr3,		& dzi_default, PG0, },	// NR : was: AGC
		{	38, 15,	3,	0,	display2_mainsub3,	& dzi_default, PG0, },	// main/sub RX: A/A, A/B, B/A, etc

		{	42,	15,	5,	0,	display2_vfomode5alt,	& dzi_default, PG0, },	// SPLIT
		{	26,	20,	0,	0,	display2_freqX_b,	& dzi_default, PG0, },	// SUB FREQ
		{	38, 20,	3,	0,	display2_mode3_b,	& dzi_modeb,	PG0, },	// SSB/CW/AM/FM/...

//		{	0,	0,	0,	0,	wait_iq, 			& dzi_default,	PG0, },
#if WITHBARS
		{    0, 4,  0,	0,	display2_smeter15_init,	& dzi_default, PGINI, },	//  Инициализация стрелочного прибора
		{    0, 4,  15,	20,	display2_smeter15, 		& dzi_compat, PG0, },	// Изображение стрелочного прибора
#endif /* WITHBARS */
#if WITHAFSPECTRE
		{	0,	4,	0,	0,	display2_af_spectre15_init,	& dzi_default, PGINI, },
		{	0,	4,	0,	0,	display2_af_spectre15_latch,	& dzi_default,	PGLATCH, },
		{	0,	4,	15,	0,	display2_af_spectre15,		& dzi_default, PG0, },
#endif /* WITHAFSPECTRE */

		{	0,	0, 4,	0,	display2_siglevel4, 	& dzi_default, PG0, },	// signal level dBm
		{	0,	DLES,	50,	BDCV_ALLRX,	display2_wfl_init,	& dzi_default,	PGINI, },	// формирование палитры водопада
		{	0,	DLES,	50,	BDCV_ALLRX,	display2_latchcombo,	& dzi_default,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
		{	0,	DLES,	50,	BDCV_ALLRX,	display2_gcombo,	& dzi_default, PG0_1, },// подготовка изображения спектра
		{	0,	DLES,	0,	0,	gui_WM_walkthrough,	& dzi_default, PG0_1, },

		{	0,	0,	0, 0, display2_showmain,	& dzi_default, REDRSUBSET_SHOW, }, // запись подготовленного изображения на главный дисплей
#endif /* ! WITHLVGL */
	};

#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		p->multilinemenu_max_rows = 15;
		p->ystep = 4;	// количество ячеек разметки на одну строку меню
		p->reverse = 1;
		p->valuew = 8;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		p->xspan = BDTH_ALLRX;	/* количество знакомест по горизонтали, отдаваемое под меню */
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
