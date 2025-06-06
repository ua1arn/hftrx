	// стрелочный S-метр
	// TFT панель AT070TN90
	// 480/5 = 96, 800/16=50

void linux_lvgl_gui(const gxdrawb_t * db,
		uint_fast8_t x,
		uint_fast8_t y,
		uint_fast8_t xspan,
		uint_fast8_t yspan,
		dctx_t * pctx
		);

	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале (на этом дизайне нет, просто для того чтобы компилировлось)

	enum {
		DLES = 27,		// spectrum window upper line
        DLE1 = 92 - GRID2Y(1),		// 96-5
		DLE_unused
	};

	enum
	{
		BDTH_ALLRXBARS = 30,	// ширина зоны для отображение барграфов на индикаторе

		BDTH_ALLRX = 50, 		// ширина зоны для отображение графического окна на индикаторе
		BDCV_ALLRX = ROWS2GRID(DLE1 - DLES - 1),	// количество строк, отведенное под панораму и волопад.

		BDTH_LEFTRX = 17,	// ширина индикатора баллов (без плюсов)
		BDTH_RIGHTRX = BDTH_ALLRXBARS - BDTH_LEFTRX,	// ширина индикатора плюсов
		BDTH_SPACERX = 0,
		BDTH_ALLSWR = 16,
		BDTH_SPACESWR = 2,
		BDTH_ALLPWR = 12,
		BDTH_SPACEPWR = 0,

		BDTH_ALL = 50, 		// ширина индикатора
		//
		B_unused
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
		PGSWR = PG0,	// страница отоюражения S-meter и SWR-meter
		PGLATCH = REDRSUBSET_LATCH,	// страницы, на которых возможно отображение водопада или панорамы.
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
	static const dzone_t dzones [] =
	{

//		{	0,	0,	0, 0, display2_preparebg,	& dzi_default, REDRSUBSET_SHOW, }, // Стирание фона
		{	0,	0,	0, 0, display2_keyboard_screen0,	& dzi_default, PGALL | REDRSUBSET_SLEEP, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана

		{	0,	0, 	7, 5, display_siglevel7, 	& dzi_siglevel, PGALL, },	// signal level dBm
		{	10,	0,	4, 5, display2_rxbwval4,	& dzi_rxbwval, PGALL, },	// RX BW value
		{	15,	0,	2, 5, display_txrxstate2, 	& dzi_txrx, PGALL, },
		{	26,	0,	5, 5, display2_ant5,		& dzi_antenna, PGALL, },
		{	31,	0,	5, 5, display2_att4,		& dzi_attenuator, PGALL, },
		{	36,	0,	5, 5, display2_preovf3,		& dzi_preamp_ovf, PGALL, },
	#if WITHBARS
		{   0, 	5,  0, 0, display2_smeter15_init, & dzi_default, PGINI, },	//  Инициализация стрелочного прибора
		{   0, 	5,	15, 20, display2_smeter15, 	& dzi_smeter, PGALL, },	// Изображение стрелочного прибора
	#endif /* WITHBARS */
	#if WITHAFSPECTRE
		{	0,	5,	0, 0, display2_af_spectre15_init,	& dzi_default, PGINI, },
		{	0,	5,	0, 0, display2_af_spectre15_latch,	& dzi_default,	PGLATCH, },
		{	0,	5,	15, 20, display2_af_spectre15,		& dzi_default, PG0, },
	#endif /* WITHAFSPECTRE */

		{	15,	5,	0, 0, display2_freqX_a_init,	& dzi_default, PGINI, },	// MAIN FREQ Частота (большие цифры)
		{	15,	5,	21, 15, display2_freqX_a,	& dzi_freqa, PGALL, },	// MAIN FREQ Частота Герцы маленьким шрифтом.


		//{	36,	5,	5, 5, display2_dummy,		& dzi_default, PGALL, },	// placeholder
		{	36, 10,	5, 5, display2_mode3_a,		& dzi_modea,	PGALL, },	// SSB/CW/AM/FM/...
		{	41, 10,	4, 5, display2_rxbw3,		& dzi_rxbw, PGALL, },	// 3.1 / 0,5 / WID / NAR
		{	45, 10,	5, 5, display2_agc3,		& dzi_default, PGALL, },	// AGC mode

		{	36, 15,	5, 5, display2_nr3,			& dzi_default, PGALL, },	// NR : was: AGC
		{	41, 15,	4, 5, display2_datamode3,	& dzi_default, PGALL, },	// DATA mode indicator
		{	45, 15,	5, 5, display2_notch5,		& dzi_default, PGALL, },	// NOTCH on/off

		{	15, 20,	3, 5, display2_mainsub3,	& dzi_default, PGALL, },	// main/sub RX: A/A, A/B, B/A, etc
		{	20,	20,	3, 5, display2_vfomode3,	& dzi_default, PGALL, },	// SPL
		{	24,	20,	12, 5, display2_freqX_b,	& dzi_freqb, PGALL, },	// SUB FREQ
		{	36, 20,	5, 5, display2_mode3_b,		& dzi_modeb,	PGALL, },	// SSB/CW/AM/FM/...
		{	41, 20,	4, 5, display2_voxtune3,	& dzi_default, PGALL, },	// VOX
		{	45,	20,	5, 5, display2_lockstate4, 	& dzi_default, PGALL, },	// LOCK

//		{	0,	0,	5, 96 - 5, linux_lvgl_gui, 		& dzi_GUI, PGINI, },

		//{	14, 25,
		//{	19, 25,
		//{	23, 25,
		//{	27, 25,


	#if WITHSPECTRUMWF
		{	0,	DLES,	CHARS2GRID(BDTH_ALLRX), BDCV_ALLRX, display2_wfl_init,	& dzi_default,	PGINI, },	// формирование палитры водопада
		{	0,	DLES,	CHARS2GRID(BDTH_ALLRX), BDCV_ALLRX,	display2_latchcombo,	& dzi_default,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
		{	0,	DLES,	CHARS2GRID(BDTH_ALLRX), BDCV_ALLRX, display2_gcombo,	& dzi_gcombo, PG0, },// подготовка изображения спектра
	#endif /* WITHSPECTRUMWF */


	#if WITHMENU
		{	0,				0,		0, 0, display2_keyboard_menu,				& dzi_default, REDRSUBSET_MENU, }, // Обработка клавиатуры и валкодеров при нахождении в режиме меню
		{	0,				DLES,	LABELW, (DLE1 - DLES) - 1, display2_multilinemenu_block_groups,	& dzi_default, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
		{	LABELW + 1,		DLES,	LABELW, (DLE1 - DLES) - 1, display2_multilinemenu_block_params,	& dzi_default, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
		{	LABELW*2 + 1,	DLES,	BDTH_ALL - (LABELW*2 + 1), (DLE1 - DLES) - 1, display2_multilinemenu_block_vals,	& dzi_default, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
	#if WITHAUTOTUNER
		{	28, 			DLE1,	22, 5, display2_swrsts22,	& dzi_default, REDRSUBSET_MENU, },	// SWR METER display
	#endif /* WITHAUTOTUNER */
	#endif /* WITHMENU */

//		{	0,	0,	0, 0, display2_showmain,	& dzi_default, REDRSUBSET_SHOW, }, // запись подготовленного изображения на главный дисплей
	};

#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		enum { YSTEP = 5 };		// количество ячеек разметки на одну строку меню
		p->multilinemenu_max_rows = (DLE1 - DLES) / YSTEP;
		p->ystep = YSTEP;	// количество ячеек разметки на одну строку меню
		p->reverse = 1;
		p->valuew = 10;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
	}
#endif /* WITHMENU */

	/* получить координаты окна с панорамой и/или водопадом. */
	void display2_getpipparams(pipparams_t * p)
	{
		p->x = GRID2X(0);	// позиция верхнего левого угла в пикселях
		p->y = GRID2Y(DLES);	// позиция верхнего левого угла в пикселях
		p->w = GRID2X(CHARS2GRID(BDTH_ALLRX));	// размер по горизонтали в пикселях
		p->h = GRID2Y(BDCV_ALLRX);				// размер по вертикали в пикселях
	}
