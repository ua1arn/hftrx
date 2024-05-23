	// Вариант с сенсорным экраном
	// TFT панель AT070TN90
	// 480/5 = 96, 800/16=50

void wait_iq(
	uint_fast8_t x,
	uint_fast8_t y,
	dctx_t * pctx
	)
{
	linux_wait_iq();
}

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
		DPAGE1,					// служебная страница для полноэкранных окон touch GUI
		DISPLC_MODCOUNT
	};

	enum
	{
		PG0 = REDRSUBSET(DPAGE0),
		PG1 = REDRSUBSET(DPAGE1),
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
#if ! WITHLVGL
		{	0,	0,	display2_clearbg, 	REDRM_ALL, PG1, },
		{	0,	0,	display2_keyboard_screen0,	REDRM_ALL, PG0, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана
		//{	10,	0,	display2_rxbwval6alt,	REDRM_ALL, PG0, },	// RX BW value
		{	17,	0,	display_txrxstate5alt, REDRM_ALL, PG0, },
		{	21,	0,	display2_ant7alt,		REDRM_ALL, PG0, },
		{	26,	0,	display2_att5alt,		REDRM_ALL, PG0, },
		{	31,	0,	display2_preovf5alt,	REDRM_ALL, PG0, },
		{	35,	0,	display2_genham1,	REDRM_ALL, PG0, },	// Отображение режима General Coverage / HAM bands
		{	38,	0,	display2_lockstate5alt, REDRM_ALL, PG0, },	// LOCK
		{	42, 0,	display2_notch7alt,	REDRM_ALL, PG0, },	// NOTCH on/off
		{	42, 5,	display2_voxtune3,	REDRM_ALL, PG0, },	// VOX
		{	46, 5,	display2_datamode3,	REDRM_ALL, PG0, },	// DATA mode indicator
		{	46, 15,	display2_usbsts3,	REDRM_ALL, PG0, },	// USB host status
		{	46, 20,	display2_rec3,		REDRM_ALL, PG0, },	// Отображение режима записи аудио фрагмента

		{   0,  0,  layout_init,		REDRM_INIS, PGINI, },
//		{   47, 20, display2_bkin3,		REDRM_ALL, PG0, },
//		{	46, 20,	display2_agc3,		REDRM_ALL, PG0, },	// AGC mode
//		{	46, 25,	display2_voxtune3,	REDRM_ALL, PG0, },	// VOX
//		{	46, 30,	display2_datamode3,	REDRM_ALL, PG0, },	// DATA mode indicator
//		{	46, 35,	display2_nr3,		REDRM_ALL, PG0, },	// NR
		{	42, 15,	display2_atu4alt,		REDRM_ALL, PG0, },	// TUNER state (optional)
		{	42, 20,	display2_byp4alt,		REDRM_ALL, PG0, },	// TUNER BYPASS state (optional)

		{	17,	7,	display2_freqX_a_init,	REDRM_INIS, PGINI, },	// MAIN FREQ Частота (большие цифры)
		{	17,	7,	display2_freqX_a,	REDRM_ALL, PG0, },	// MAIN FREQ Частота (большие цифры)
		{	38, 10,	display2_mode3_a,	REDRM_ALL,	PG0, },	// SSB/CW/AM/FM/...
		{	42, 10,	display2_rxbw3,		REDRM_ALL, PG0, },	// 3.1 / 0,5 / WID / NAR
		{	46, 10,	display2_nr3,		REDRM_ALL, PG0, },	// NR : was: AGC
		{	38, 15,	display2_mainsub3,	REDRM_ALL, PG0, },	// main/sub RX: A/A, A/B, B/A, etc

		{	42,	15,	display2_vfomode5alt,	REDRM_ALL, PG0, },	// SPLIT
		{	26,	20,	display2_freqX_b,	REDRM_ALL, PG0, },	// SUB FREQ
		{	38, 20,	display2_mode3_b,	REDRM_ALL,	PG0, },	// SSB/CW/AM/FM/...

		{	0,	0,	wait_iq, 			REDRM_ALL,	PG0, },
#if WITHBARS
		{    0, 4,  display2_smeter15_init,	REDRM_INIS, PGINI, },	//  Инициализация стрелочного прибора
		{    0, 4,  display2_smeter15, 		REDRM_ALL, PG0, },	// Изображение стрелочного прибора
#endif /* WITHBARS */
#if WITHAFSPECTRE
		{	0,	4,	display2_af_spectre15_init,	REDRM_INIS, PGINI, },
		{	0,	4,	display2_af_spectre15_latch,	REDRM_ALL,	PG0, },
		{	0,	4,	display2_af_spectre15,		REDRM_ALL, PG0, },
#endif /* WITHAFSPECTRE */

		{	0,	0, display2_siglevel4, 	REDRM_ALL, PG0, },	// signal level dBm
		{	0,	DLES,	display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
		{	0,	DLES,	display2_latchwaterfall,	REDRM_ALL,	PG0, },	// формирование данных спектра для последующего отображения спектра или водопада
		{	0,	DLES,	display2_spectrum,	REDRM_ALL, PG0, },// подготовка изображения спектра
		{	0,	DLES,	display2_waterfall,	REDRM_ALL, PG0, },// подготовка изображения водопада
		{	0,	DLES,	display2_colorbuff,	REDRM_ALL, PG0, },// Отображение водопада и/или спектра
		{	0,	DLES,	gui_WM_walkthrough,	REDRM_ALL, PG1, },

		{	0,	0,	display2_nextfb, 	REDRM_ALL, PG1, },
#endif /* ! WITHLVGL */
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
