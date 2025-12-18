	// вариант без сенсорного экрана
	// стрелочный S-метр
	/* AT070TNA2 panel (1024*600) - 7" display */
	// 600/5 = 120, 1024/16=64

	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале (на этом дизайне нет, просто для того чтобы компилировлось)

	enum {
		DLES = 35,		// spectrum window upper line
        DLE1 = 120 - GRID2Y(1),		// 96-5
		DLE_unused
	};

	enum
	{
		BDTH_ALLRXBARS = 30,	// ширина зоны для отображение барграфов на индикаторе

		BDTH_ALLRX = 64, 		// ширина зоны для отображение графического окна на индикаторе
		BDCV_ALLRX = (79 /* DLE1 - DLES */),	// количество строк, отведенное под панораму и волопад.

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
		PGINI = REDRSUBSET_INIT,
		PGunused
	};

	#if 1//TUNE_TOP > 100000000uL
		#define DISPLC_WIDTH	9	// количество цифр в отображении частоты
	#else
		#define DISPLC_WIDTH	8	// количество цифр в отображении частоты
	#endif
	#define DISPLC_RJ		0	// количество скрытых справа цифр в отображении частоты

	// 600/5 = 120, 1024/16=64
	// 480/5 = 96, 800/16=50
	// 272/5 = 54, 480/16=30 (old)
	//#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
	//#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */
	
	
	static const dzone_t dzones [] =
	{
			{	0,	0,	0, 0, display2_preparebg,	& dzi_default, REDRSUBSET_SHOW, }, // Стирание фона

		{	0,	0, 	7, 4, display_siglevel7, 	& dzi_siglevel, PGALL, },	// signal level dBm
		//{	0,	0, 	5, 4, display2_smeors5, 	& dzi_default, PGALL, },	// уровень сигнала в баллах S или dBm
		{	10,	0,	4, 4, display2_rxbwval4,	& dzi_rxbwval, PGALL, },	// RX BW value
		{	15,	0,	2, 4, display_txrxstate2, 	& dzi_txrx, PGALL, },
		{	18, 0,	3, 4, display2_atu3,		& dzi_tune, PGALL, },	// TUNER state (optional)
		{	22, 0,	3, 4, display2_byp3,		& dzi_bypass, PGALL, },	// TUNER BYPASS state (optional)
		{	26,	0,	5, 4, display2_ant5,		& dzi_antenna, PGALL, },
		{	32,	0,	4, 4, display2_att4,		& dzi_attenuator, PGALL, },
		{	37,	0,	3, 4, display2_preovf3,		& dzi_preamp_ovf, PGALL, },
	#if WITHBARS
		{   0, 	4,  0, 0, display2_smeter15_init,& dzi_default, PGINI, },	//  Инициализация стрелочного прибора
		{   0, 	4,	SM_BG_W_CELLS, SM_BG_H_CELLS, display2_smeter15, 	& dzi_smtr2, PGALL, },	// Изображение стрелочного прибора
	#endif /* WITHBARS */
	#if WITHAFSPECTRE
		{	0,	4,	0, 0, display2_af_spectre15_init,	& dzi_default, PGINI, },
		{	0,	4,	0, 0, display2_af_spectre15_latch,	& dzi_default,	PGLATCH, },
		{	0,	4,	SM_BG_W_CELLS,	SM_BG_H_CELLS, display2_af_spectre15,		& dzi_default, PGSPE, },
	#endif /* WITHAFSPECTRE */

		{	15,	6,	0, 0, display2_freqX_a_init,	& dzi_default, PGINI, },	// MAIN FREQ Частота (большие цифры)
		{	15,	6,	21, 11, display2_freqX_a,		& dzi_freqa, PGALL, },	// MAIN FREQ Герцы маленьким шрифтом.

		{	41, 0,	9, 9, display2_fnblock9,	& dzi_default, PGALL, },	// FUNC menu item label & value

		{	37, 10,	3, 4, display2_mode3_a,		& dzi_modea,	PGALL, },	// SSB/CW/AM/FM/...
		{	41, 10,	3, 4, display2_rxbw3,		& dzi_rxbw, PGALL, },	// 3.1 / 0,5 / WID / NAR
		{	46, 10,	3, 4, display2_agc3,		& dzi_default, PGALL, },	// AGC mode

		{	37, 15,	3, 4, display2_nr3,			& dzi_default, PGALL, },	// NR : was: AGC
		{	41, 15,	3, 4, display2_datamode3,	& dzi_default, PGALL, },	// DATA mode indicator
		{	45, 15,	5, 4, display2_notch5,		& dzi_default, PGALL, },	// NOTCH on/off

		{	15, 20,	3, 4, display2_mainsub3,	& dzi_default, PGALL, },	// main/sub RX: A/A, A/B, B/A, etc
		{	20,	20,	3, 4, display2_vfomode3,	& dzi_default, PGALL, },	// SPL
		{	24,	20,	0, 4, display2_freqX_b,		& dzi_freqb, PGALL, },	// SUB FREQ
		{	37, 20,	0, 4, display2_mode3_b,		& dzi_modeb,	PGALL, },	// SSB/CW/AM/FM/...
		{	41, 20,	3, 4, display2_voxtune3,	& dzi_default, PGALL, },	// VOX
		{	45,	20,	4, 4, display2_lockstate4, & dzi_default, PGALL, },	// LOCK

		// размещены под S-метром (15 ячеек)
		{	1, 	25,	5, 4, display2_voltlevelV, & dzi_voltlevel, PGALL, },	// voltmeter with "V"
		{	7, 	25,	6, 4, display2_currlevelA, & dzi_currlevel, PGALL, },	// PA drain current d.dd with "A"

		//{	14, 25, 0, 0,
		//{	19, 25, 0, 0,
		//{	23, 25, 0, 0,
		//{	27, 25, 0, 0,
		{	33, 25,	3, 4, display2_rec3,		& dzi_default, PGALL, },	// Отображение режима записи аудио фрагмента
		{	37, 25,	3, 4, display2_spk3,		& dzi_default, PGALL, },	// отображение признака включения динамика
		{	41, 25, 0, 4, display2_bkin3,		& dzi_default, PGALL, },	// BREAK-IN
		{	45,	25,	5, 4, display2_wpm5, 		& dzi_default, PGALL, },	// 22WPM

		//{	24, 30,	10, 0, display_freqmeter10, & dzi_default, PGALL, },	// измеренная частота опоры
		{	37, 30,	8, 0, display2_freqdelta8, 	& dzi_default, PGALL, },	// выход ЧМ демодулятора

	#if WITHSPECTRUMWF
		{	0,	DLES,	(BDTH_ALLRX),	BDCV_ALLRX, display2_wfl_init,	& dzi_default,	PGINI, },	// формирование палитры водопада
		{	0,	DLES,	(BDTH_ALLRX),	BDCV_ALLRX, display2_latchcombo,	& dzi_default,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
		{	0,	DLES,	(BDTH_ALLRX),	BDCV_ALLRX, display2_gcombo,	& dzi_default, PGSPE, },// подготовка изображения спектра
	#endif /* WITHSPECTRUMWF */

		{	0,	DLE1,	12, 4, display2_datetime12,	& dzi_datetime12, PGALL,	},	// DATE&TIME Jan-01 13:40
		{	13,	DLE1,	9, 4, display2_span9,		& dzi_default, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */
		{	23, DLE1,	4, 4, display2_thermo,	& dzi_default, PGALL, },	// thermo sensor
		{	28, DLE1,	3, 4, display2_usbsts3,		& dzi_default, PG0, },	// USB host status
		{	32, DLE1,	7, 4, display2_classa7,		& dzi_default, PG0, },	// Class-A power amplifier
		//{	28, DLE1,	10, 4, display_freqmeter10, & dzi_default, PGALL, },	// измеренная частота опоры

	#if WITHMENU
		{	0,				DLES,	BDTH_ALLRX, (DLE1 - DLES) - 1, display2_multilinemenu_block,	& dzi_compat, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
	#if WITHAUTOTUNER
		{	28, 			DLE1,	20, 0, display2_swrsts20,	& dzi_default, REDRSUBSET_MENU, },	// SWR METER display
	#endif /* WITHAUTOTUNER */
	#endif /* WITHMENU */

		// sleep mode display
		{	5,	25,	12, 4, display2_datetime12,	& dzi_datetime12, PGSLP, },	// DATE & TIME // DATE&TIME Jan-01 13:40
		{	20, 25,	5, 4, display2_voltlevelV, & dzi_voltlevel, PGSLP, },	// voltmeter with "V"

	#if 0
		{	0,	0,	0, 0, display2_vtty_init,	& dzi_default,	PGINI, },	// Подготовка видеобуфера окна протокола
		{	0,	0, 0, 0, display2_vtty,	& dzi_default, PG0, },		// Вывод текущего состояния протокола
		{	0,	(DIM_Y - GRID2Y(5)) / 5, 0, 0, display2_freqsof9,	& dzi_default, PG0, },		// Вывод текущего состояния протокола
	#endif

		{	0,	0,	0, 0, display2_showmain,	& dzi_default, REDRSUBSET_SHOW, }, // запись подготовленного изображения на главный дисплей
	};

#if WITHMENU
	void display2_getmultimenu(multimenuwnd_t * p)
	{
		enum { YSTEP = 5 };		// количество ячеек разметки на одну строку меню
		p->multilinemenu_max_rows = (DLE1 - DLES) / YSTEP;
		p->ystep = YSTEP;	// количество ячеек разметки на одну строку меню
		p->reverse = 1;
		p->valuew = 10;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
		p->xspan = BDTH_ALLRX;	/* количество знакомест по горизонтали, отдаваемое под меню */
	}
#endif /* WITHMENU */

	/* получить координаты окна с панорамой и/или водопадом. */
	void display2_getpipparams(pipparams_t * p)
	{
		p->x = GRID2X(0);	// позиция верхнего левого угла в пикселях
		p->y = GRID2Y(DLES);	// позиция верхнего левого угла в пикселях
		p->w = GRID2X((BDTH_ALLRX));	// размер по горизонтали в пикселях
		p->h = GRID2Y(BDCV_ALLRX);				// размер по вертикали в пикселях
	}
	#define DISPLC_RADIUS 	0	// радиус закругления углов плиток в dzones

