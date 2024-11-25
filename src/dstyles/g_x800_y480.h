	// вариант без сенсорного экрана
	// стрелочный S-метр
	// TFT панель AT070TN90
	// 480/5 = 96, 800/16=50

	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале (на этом дизайне нет, просто для того чтобы компилировлось)

	enum {
		DLES = 35,		// spectrum window upper line
        DLE1 = 96 - GRID2Y(1),		// 96-5
		DLE_unused
	};

	enum
	{
		BDTH_ALLRXBARS = 30,	// ширина зоны для отображение барграфов на индикаторе

		BDTH_ALLRX = 50, 		// ширина зоны для отображение графического окна на индикаторе
		BDCV_ALLRX = ROWS2GRID(55 /* DLE1 - DLES */),	// количество строк, отведенное под панораму и волопад.

		BDTH_LEFTRX = 17,	// ширина индикатора баллов (без плюсов)
		BDTH_RIGHTRX = BDTH_ALLRXBARS - BDTH_LEFTRX,	// ширина индикатора плюсов
		BDTH_SPACERX = 0,
		BDTH_ALLSWR = 16,
		BDTH_SPACESWR = 2,
		BDTH_ALLPWR = 12,
		BDTH_SPACEPWR = 0,

		/* совмещение на одном экрание водопада и панорамы */
		BDCO_SPMRX = ROWS2GRID(0),	// смещение спектра по вертикали в ячейках от начала общего поля
		BDCV_SPMRX = ROWS2GRID(35)	// вертикальный размер спектра в ячейках
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
	static const FLASHMEM struct dzone dzones [] =
	{
		/* общий для всех режимов элемент */
		{	0,	0,	0, 0, display2_clearbg, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
		{	0,	0,	0, 0, display2_keyboard_screen0,	REDRM_KEYB, PGALL | REDRSUBSET_SLEEP, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана

		{	0,	0, 	7, 5, display_siglevel7, 	REDRM_BARS, PGALL, },	// signal level dBm
		//{	0,	0, 	5, 5, display2_smeors5, 	REDRM_BARS, PGALL, },	// уровень сигнала в баллах S или dBm
		{	10,	0,	4, 5, display2_rxbwval4,	REDRM_MODE, PGALL, },	// RX BW value
		{	15,	0,	2, 5, display_txrxstate2, REDRM_MODE, PGALL, },
		{	18, 0,	3, 5, display2_atu3,		REDRM_MODE, PGALL, },	// TUNER state (optional)
		{	22, 0,	3, 5, display2_byp3,		REDRM_MODE, PGALL, },	// TUNER BYPASS state (optional)
		{	26,	0,	5, 5, display2_ant5,		REDRM_MODE, PGALL, },
		{	32,	0,	4, 5, display2_att4,		REDRM_MODE, PGALL, },
		{	37,	0,	3, 5, display2_preovf3,	REDRM_BARS, PGALL, },
	#if WITHBARS
		{   0, 	4,  0, 0, display2_smeter15_init,REDRM_INIS, PGINI, },	//  Инициализация стрелочного прибора
		{   0, 	4,	15, 20, display2_smeter15, 	REDRM_BARS, PGALL, },	// Изображение стрелочного прибора
	#endif /* WITHBARS */
	#if WITHAFSPECTRE
		{	0,	4,	0, 0, display2_af_spectre15_init,	REDRM_INIS, PGINI, },
		{	0,	4,	0, 0, display2_af_spectre15_latch,	REDRM_BARS,	PGLATCH, },
		{	0,	4,	15, 20, display2_af_spectre15,		REDRM_BARS, PG0, },
	#endif /* WITHAFSPECTRE */

		{	15,	6,	0, 0, display2_freqX_a_init,	REDRM_INIS, PGINI, },	// MAIN FREQ Частота (большие цифры)
		{	15,	6,	21, 13, display2_freqX_a,	REDRM_FREQ, PGALL, },	// MAIN FREQ Частота Герцы маленьким шрифтом.

		{	41, 0,	9, 5, display2_fnlabel9,	REDRM_MODE, PGALL, },	// FUNC item label
		{	41,	4,	9, 5, display2_fnvalue9,	REDRM_MODE, PGALL, },	// FUNC item value

		{	37, 10,	3, 5, display2_mode3_a,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		{	41, 10,	3, 5, display2_rxbw3,		REDRM_MODE, PGALL, },	// 3.1 / 0,5 / WID / NAR
		{	46, 10,	3, 5, display2_agc3,		REDRM_MODE, PGALL, },	// AGC mode

		{	37, 15,	3, 5, display2_nr3,		REDRM_MODE, PGALL, },	// NR : was: AGC
		{	41, 15,	3, 5, display2_datamode3,	REDRM_MODE, PGALL, },	// DATA mode indicator
		{	45, 15,	5, 5, display2_notch5,	REDRM_MODE, PGALL, },	// NOTCH on/off

		{	15, 20,	3, 0, display2_mainsub3,	REDRM_MODE, PGALL, },	// main/sub RX: A/A, A/B, B/A, etc
		{	20,	20,	3, 5, display2_vfomode3,	REDRM_MODE, PGALL, },	// SPL
		{	24,	20,	12, 5, display2_freqX_b,	REDRM_FRQB, PGALL, },	// SUB FREQ
		{	37, 20,	3, 5, display2_mode3_b,	REDRM_MODE,	PGALL, },	// SSB/CW/AM/FM/...
		{	41, 20,	3, 5, display2_voxtune3,	REDRM_MODE, PGALL, },	// VOX
		{	45,	20,	4, 5, display2_lockstate4, REDRM_MODE, PGALL, },	// LOCK

		// размещены под S-метром (15 ячеек)
		{	1, 	25,	5, 5, display2_voltlevelV5, REDRM_VOLT, PGALL, },	// voltmeter with "V"
		{	7, 	25,	6, 5, display2_currlevelA6, REDRM_VOLT, PGALL, },	// PA drain current d.dd with "A"

		//{	14, 25,
		//{	19, 25,
		//{	23, 25,
		//{	27, 25,
		{	33, 25,	3, 5, display2_rec3,		REDRM_BARS, PGALL, },	// Отображение режима записи аудио фрагмента
		{	37, 25,	3, 5, display2_spk3,		REDRM_MODE, PGALL, },	// отображение признака включения динамика
		{	41, 25, 3, 5, display2_bkin3,		REDRM_MODE, PGALL, },	// BREAK-IN
		{	45,	25,	5, 5, display2_wpm5, 		REDRM_BARS, PGALL, },	// 22WPM

		//{	24, 30,	10, 5, display_freqmeter10, REDRM_BARS, PGALL, },	// измеренная частота опоры
		{	37, 30,	8, 5, display2_freqdelta8, REDRM_BARS, PGALL, },	// выход ЧМ демодулятора

	#if WITHSPECTRUMWF
		{	0,	DLES,	0, 0, display2_wfl_init,	REDRM_INIS,	PGINI, },	// формирование палитры водопада
		{	0,	DLES,	0, 0, display2_latchwaterfall,	REDRM_BARS,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
		{	0,	DLES,	BDTH_ALLRX, BDCV_ALLRX, display2_gcombo,	REDRM_BARS, PG0, },// подготовка изображения спектра
	#endif /* WITHSPECTRUMWF */

		{	0,	DLE1,	12, 5, display2_datetime12,	REDRM_BARS, PGALL,	},	// DATE&TIME Jan-01 13:40
		{	13,	DLE1,	9, 5, display2_span9,		REDRM_MODE, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */
		{	23, DLE1,	4, 5, display2_thermo4,	REDRM_VOLT, PGALL, },	// thermo sensor
		{	28, DLE1,	3, 5, display2_usbsts3,		REDRM_BARS, PG0, },	// USB host status
		{	32, DLE1,	7, 5, display2_classa7,		REDRM_BARS, PGALL, },	// Class-A power amplifier
		//{	28, DLE1,	10, 5, display_freqmeter10, REDRM_BARS, PGALL, },	// измеренная частота опоры

	#if WITHMENU
		{	0,				0,		0, 0, display2_keyboard_menu,					REDRM_KEYB, REDRSUBSET_MENU, }, // Обработка клавиатуры и валкодеров при нахождении в режиме меню
		{	3,				DLES,	0, 0, display2_multilinemenu_block_groups,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
		{	LABELW + 5,		DLES,	0, 0, display2_multilinemenu_block_params,	REDRM_MLBL, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
		{	LABELW*2 + 6,	DLES,	0, 0, display2_multilinemenu_block_vals,	REDRM_MVAL, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
	#if WITHAUTOTUNER
		{	28, 			DLE1,	22, 5, display2_swrsts22,	REDRM_BARS, REDRSUBSET_MENU, },	// SWR METER display
	#endif /* WITHAUTOTUNER */
	#endif /* WITHMENU */

		// sleep mode display
		{	5,	25,	12, 5, display2_datetime12,	REDRM_BARS, PGSLP, },	// DATE & TIME // DATE&TIME Jan-01 13:40
		{	20, 25,	5, 5, display2_voltlevelV5, REDRM_VOLT, PGSLP, },	// voltmeter with "V"
	#if WITHMENU
		{	15, 25,	0, 0, display2_popup,		REDRM_MODE, PG0, },	// Всплывающие меню. В конце массива для перекрытия всего что под ним
	#endif /* WITHMENU */

	#if 0
		{	0,	0,	0, 0, display2_vtty_init,	REDRM_INIS,	PGINI, },	// Подготовка видеобуфера окна протокола
		{	0,	0, 0, 0, display2_vtty,	REDRM_BARS, PG0, },		// Вывод текущего состояния протокола
		{	0,	(DIM_Y - GRID2Y(5)) / 5, 9, 5, display2_freqsof9,	REDRM_BARS, PG0, },		// Вывод текущего состояния протокола
	#endif

		/* общий для всех режимов элемент */
		{	0,	0,	0, 0, display2_nextfb, 	REDRM_MODE, PGALL | REDRSUBSET_SLEEP, },
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

#if WITHRENDERHTML

	static const char hftrx_layout_html [] =
	R"##(
	<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
	<html>
	  <head>
	    <meta charset="utf-8">
	    <title>HF TRX 800x480</title>
	    <meta name="viewport" content="width=device-width,initial-scale=1.0">
	    <!--link rel="stylesheet" type="text/css" href="hftrx.css"-->  
	<style>
	</style>
	</head>
	<body style="background-color:orange;">
	 <div style="position:absolute; left:0px; top:0px; width:112px; height:25px; background-color:blue; color:black; ">X</div>
	 <div style="position:absolute; left:160px; top:0px; width:64px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:240px; top:0px; width:32px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:288px; top:0px; width:48px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:352px; top:0px; width:48px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:416px; top:0px; width:80px; height:25px; background-color:blue; color:black; ">X</div>
	 <div style="position:absolute; left:512px; top:0px; width:64px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:592px; top:0px; width:48px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:0px; top:20px; width:224px; height:100px; background-color:blue; color:black; ">X</div>
	 <div style="position:absolute; left:240px; top:30px; width:336px; height:65px; background-color:blue; color:black; ">X</div>
	 <div style="position:absolute; left:656px; top:0px; width:144px; height:25px; background-color:blue; color:black; ">X</div>
	 <div style="position:absolute; left:656px; top:20px; width:144px; height:25px; background-color:blue; color:black; ">X</div>
	 <div style="position:absolute; left:592px; top:50px; width:48px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:656px; top:50px; width:48px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:736px; top:50px; width:48px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:592px; top:75px; width:48px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:656px; top:75px; width:48px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:720px; top:75px; width:80px; height:25px; background-color:blue; color:black; ">X</div>
	 <div style="position:absolute; left:320px; top:100px; width:48px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:384px; top:100px; width:192px; height:25px; background-color:blue; color:black; ">X</div>
	 <div style="position:absolute; left:592px; top:100px; width:48px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:656px; top:100px; width:48px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:720px; top:100px; width:64px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:16px; top:125px; width:80px; height:25px; background-color:blue; color:black; ">X</div>
	 <div style="position:absolute; left:112px; top:125px; width:96px; height:25px; background-color:blue; color:black; ">X</div>
	 <div style="position:absolute; left:528px; top:125px; width:48px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:592px; top:125px; width:48px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:656px; top:125px; width:48px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:720px; top:125px; width:80px; height:25px; background-color:blue; color:black; ">X</div>
	 <div style="position:absolute; left:592px; top:150px; width:128px; height:25px; background-color:blue; color:black; ">X</div>
	 <div style="position:absolute; left:0px; top:175px; width:800px; height:275px; background-color:blue; color:black; ">X</div>
	 <div style="position:absolute; left:0px; top:455px; width:192px; height:25px; background-color:blue; color:black; ">X</div>
	 <div style="position:absolute; left:208px; top:455px; width:144px; height:25px; background-color:blue; color:black; ">X</div>
	 <div style="position:absolute; left:368px; top:455px; width:64px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:448px; top:455px; width:48px; height:25px; background-color:green; color:black; ">X</div>
	 <div style="position:absolute; left:512px; top:455px; width:112px; height:25px; background-color:blue; color:black; ">X</div>
	</body>
	</html>

	)##";

	const char * display2_gethtml(void)
	{
		return hftrx_layout_html;
	}

#endif /* WITHRENDERHTML */

