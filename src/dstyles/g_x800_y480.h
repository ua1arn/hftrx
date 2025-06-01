	// вариант без сенсорного экрана
	// стрелочный S-метр
	// TFT панель AT070TN90
	// 480/5 = 96, 800/16=50

	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале (на этом дизайне нет, просто для того чтобы компилировлось)

	enum {
		DLES = 30,		// spectrum window upper line
        DLE1 = 96 - GRID2Y(1),		// 96-5
		DLE_unused
	};

	enum
	{
		BDTH_ALLRXBARS = 30,	// ширина зоны для отображение барграфов на индикаторе

		BDTH_ALLRX = 50, 		// ширина зоны для отображение графического окна на индикаторе
		BDCV_ALLRX = ROWS2GRID(60 /* DLE1 - DLES */),	// количество строк, отведенное под панораму и волопад.

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

		{	0,	0,	0, 0, display2_preparebg,	& dzi_default, REDRSUBSET_SHOW, }, // Стирание фона
		{	0,	0,	0, 0, display2_keyboard_screen0,	& dzi_default, PGALL | REDRSUBSET_SLEEP, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана

		{	0,	0, 	7, 4, display_siglevel7, 	& dzi_default, PGALL, },	// signal level dBm
		//{	0,	0, 	5, 4, display2_smeors5, 	& dzi_default, PGALL, },	// уровень сигнала в баллах S или dBm
		{	10,	0,	4, 4, display2_rxbwval4,	& dzi_default, PGALL, },	// RX BW value
		{	15,	0,	2, 4, display_txrxstate2, 	& dzi_default, PGALL, },
		{	18, 0,	3, 4, display2_atu3,		& dzi_default, PGALL, },	// TUNER state (optional)
		{	22, 0,	3, 4, display2_byp3,		& dzi_default, PGALL, },	// TUNER BYPASS state (optional)
		{	26,	0,	5, 4, display2_ant5,		& dzi_default, PGALL, },
		{	32,	0,	4, 4, display2_att4,		& dzi_default, PGALL, },
		{	37,	0,	3, 4, display2_preovf3,		& dzi_default, PGALL, },
	#if WITHBARS
		{   0, 	4,  0, 0, display2_smeter15_init, & dzi_default, PGINI, },	//  Инициализация стрелочного прибора
		{   0, 	4,	15, 20, display2_smeter15, 	& dzi_default, PGALL, },	// Изображение стрелочного прибора
	#endif /* WITHBARS */
	#if WITHAFSPECTRE
		{	0,	4,	0, 0, display2_af_spectre15_init,	& dzi_default, PGINI, },
		{	0,	4,	0, 0, display2_af_spectre15_latch,	& dzi_default,	PGLATCH, },
		{	0,	4,	15, 20, display2_af_spectre15,		& dzi_default, PG0, },
	#endif /* WITHAFSPECTRE */

		{	15,	6,	0, 0, display2_freqX_a_init,	& dzi_default, PGINI, },	// MAIN FREQ Частота (большие цифры)
		{	15,	6,	21, 13, display2_freqX_a,	& dzi_default, PGALL, },	// MAIN FREQ Частота Герцы маленьким шрифтом.

		{	41, 0,	9, 4, display2_fnlabel9,	& dzi_default, PGALL, },	// FUNC item label
		{	41,	4,	9, 4, display2_fnvalue9,	& dzi_default, PGALL, },	// FUNC item value

		{	37, 10,	3, 4, display2_mode3_a,		& dzi_default,	PGALL, },	// SSB/CW/AM/FM/...
		{	41, 10,	3, 4, display2_rxbw3,		& dzi_default, PGALL, },	// 3.1 / 0,5 / WID / NAR
		{	46, 10,	3, 4, display2_agc3,		& dzi_default, PGALL, },	// AGC mode

		{	37, 15,	3, 4, display2_nr3,			& dzi_default, PGALL, },	// NR : was: AGC
		{	41, 15,	3, 4, display2_datamode3,	& dzi_default, PGALL, },	// DATA mode indicator
		{	45, 15,	5, 4, display2_notch5,		& dzi_default, PGALL, },	// NOTCH on/off

		{	15, 20,	3, 4, display2_mainsub3,	& dzi_default, PGALL, },	// main/sub RX: A/A, A/B, B/A, etc
		{	20,	20,	3, 4, display2_vfomode3,	& dzi_default, PGALL, },	// SPL
		{	24,	20,	12, 4, display2_freqX_b,	& dzi_default, PGALL, },	// SUB FREQ
		{	37, 20,	3, 4, display2_mode3_b,		& dzi_default,	PGALL, },	// SSB/CW/AM/FM/...
		{	41, 20,	3, 4, display2_voxtune3,	& dzi_default, PGALL, },	// VOX
		{	45,	20,	4, 4, display2_lockstate4, 	& dzi_default, PGALL, },	// LOCK

		// размещены под S-метром (15 ячеек)
		{	1, 	25,	5, 4, display2_voltlevelV5, & dzi_default, PGALL, },	// voltmeter with "V"
		{	7, 	25,	6, 4, display2_currlevelA6, & dzi_default, PGALL, },	// PA drain current d.dd with "A"

		//{	14, 25,
		//{	19, 25,
		//{	23, 25,
		//{	27, 25,
		{	33, 25,	3, 4, display2_rec3,		& dzi_default, PGALL, },	// Отображение режима записи аудио фрагмента
		{	37, 25,	3, 4, display2_spk3,		& dzi_default, PGALL, },	// отображение признака включения динамика
		{	41, 25, 3, 4, display2_bkin3,		& dzi_default, PGALL, },	// BREAK-IN
		{	45,	25,	5, 4, display2_wpm5, 		& dzi_default, PGALL, },	// 22WPM

		//{	24, 30,	10, 4, display_freqmeter10, & dzi_default, PGALL, },	// измеренная частота опоры
		//{	37, 30,	8, 4, display2_freqdelta8, & dzi_default, PGALL, },	// выход ЧМ демодулятора

	#if WITHSPECTRUMWF
		{	0,	DLES,	CHARS2GRID(BDTH_ALLRX), BDCV_ALLRX, display2_wfl_init,	& dzi_default,	PGINI, },	// формирование палитры водопада
		{	0,	DLES,	CHARS2GRID(BDTH_ALLRX), BDCV_ALLRX,	display2_latchcombo,	& dzi_default,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
		{	0,	DLES,	CHARS2GRID(BDTH_ALLRX), BDCV_ALLRX, display2_gcombo,	& dzi_default, PG0, },// подготовка изображения спектра
	#endif /* WITHSPECTRUMWF */

		{	0,	DLE1,	12, 4, display2_datetime12,	& dzi_default, PGALL,	},	// DATE&TIME Jan-01 13:40
		{	13,	DLE1,	9, 4, display2_span9,		& dzi_default, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */
		{	23, DLE1,	4, 4, display2_thermo4,	& dzi_default, PGALL, },	// thermo sensor
		{	28, DLE1,	3, 4, display2_usbsts3,		& dzi_default, PG0, },	// USB host status
		{	32, DLE1,	7, 4, display2_classa7,		& dzi_default, PGALL, },	// Class-A power amplifier
		//{	28, DLE1,	10, 5, display_freqmeter10, & dzi_default, PGALL, },	// измеренная частота опоры

	#if WITHMENU
		{	0,				0,		0, 0, display2_keyboard_menu,				& dzi_default, REDRSUBSET_MENU, }, // Обработка клавиатуры и валкодеров при нахождении в режиме меню
		{	3,				DLES,	0, 0, display2_multilinemenu_block_groups,	& dzi_default, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
		{	LABELW + 5,		DLES,	0, 0, display2_multilinemenu_block_params,	& dzi_default, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
		{	LABELW*2 + 6,	DLES,	0, 0, display2_multilinemenu_block_vals,	& dzi_default, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
	#if WITHAUTOTUNER
		{	28, 			DLE1,	22, 5, display2_swrsts22,	& dzi_default, REDRSUBSET_MENU, },	// SWR METER display
	#endif /* WITHAUTOTUNER */
	#endif /* WITHMENU */

		// sleep mode display
		{	5,	25,	12, 4, display2_datetime12,	& dzi_default, PGSLP, },	// DATE & TIME // DATE&TIME Jan-01 13:40
		{	20, 25,	5, 4, display2_voltlevelV5, & dzi_default, PGSLP, },	// voltmeter with "V"
	#if WITHMENU
		{	15, 25,	0, 0, display2_popup,		& dzi_default, PG0, },	// Всплывающие меню. В конце массива для перекрытия всего что под ним
	#endif /* WITHMENU */

	#if 0
		{	0,	0,	0, 0, display2_vtty_init,	& dzi_default,	PGINI, },	// Подготовка видеобуфера окна протокола
		{	0,	0, 0, 0, display2_vtty,	& dzi_default, PG0, },		// Вывод текущего состояния протокола
		{	0,	(DIM_Y - GRID2Y(5)) / 5, 9, 5, display2_freqsof9,	& dzi_default, PG0, },		// Вывод текущего состояния протокола
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
 #id2 { position:absolute; left:0px; top:0px; width:112px; height:20px; }
 #id3 { position:absolute; left:160px; top:0px; width:64px; height:20px; }
 #id4 { position:absolute; left:240px; top:0px; width:32px; height:20px; }
 #id5 { position:absolute; left:288px; top:0px; width:48px; height:20px; }
 #id6 { position:absolute; left:352px; top:0px; width:48px; height:20px; }
 #id7 { position:absolute; left:416px; top:0px; width:80px; height:20px; }
 #id8 { position:absolute; left:512px; top:0px; width:64px; height:20px; }
 #id9 { position:absolute; left:592px; top:0px; width:48px; height:20px; }
 #id11 { position:absolute; left:0px; top:20px; width:240px; height:100px; }
 #id13 { position:absolute; left:240px; top:30px; width:336px; height:65px; }
 #id14 { position:absolute; left:656px; top:0px; width:144px; height:20px; }
 #id15 { position:absolute; left:656px; top:20px; width:144px; height:20px; }
 #id16 { position:absolute; left:592px; top:50px; width:48px; height:20px; }
 #id17 { position:absolute; left:656px; top:50px; width:48px; height:20px; }
 #id18 { position:absolute; left:736px; top:50px; width:48px; height:20px; }
 #id19 { position:absolute; left:592px; top:75px; width:48px; height:20px; }
 #id20 { position:absolute; left:656px; top:75px; width:48px; height:20px; }
 #id21 { position:absolute; left:720px; top:75px; width:80px; height:20px; }
 #id23 { position:absolute; left:320px; top:100px; width:48px; height:20px; }
 #id24 { position:absolute; left:384px; top:100px; width:192px; height:20px; }
 #id25 { position:absolute; left:592px; top:100px; width:48px; height:20px; }
 #id26 { position:absolute; left:656px; top:100px; width:48px; height:20px; }
 #id27 { position:absolute; left:720px; top:100px; width:64px; height:20px; }
 #id28 { position:absolute; left:16px; top:125px; width:80px; height:20px; }
 #id29 { position:absolute; left:112px; top:125px; width:96px; height:20px; }
 #id30 { position:absolute; left:528px; top:125px; width:48px; height:20px; }
 #id31 { position:absolute; left:592px; top:125px; width:48px; height:20px; }
 #id32 { position:absolute; left:656px; top:125px; width:48px; height:20px; }
 #id33 { position:absolute; left:720px; top:125px; width:80px; height:20px; }
 #id34 { position:absolute; left:592px; top:150px; width:128px; height:20px; }
 #id37 { position:absolute; left:0px; top:175px; width:800px; height:275px; }
 #id38 { position:absolute; left:0px; top:455px; width:192px; height:20px; }
 #id39 { position:absolute; left:208px; top:455px; width:144px; height:20px; }
 #id40 { position:absolute; left:368px; top:455px; width:64px; height:20px; }
 #id41 { position:absolute; left:448px; top:455px; width:48px; height:20px; }
 #id42 { position:absolute; left:512px; top:455px; width:112px; height:20px; }
 .on { background-color:red; color:white }
 .off { background-color:green; color:black }
</style>
</head>
<body style="background-color:orange;">
 <div id="id2" class="on">WWWWWWW</div>
 <div id="id3" class="on">WWWW</div>
 <div id="id4" class="on off">WW</div>
 <div id="id5" style="background-color:blue; color:black;">WWW</div>
 <div id="id6" style="background-color:blue; color:black;">WWW</div>
 <div id="id7" style="background-color:blue; color:black;">WWWWW</div>
 <div id="id8" style="background-color:blue; color:black;">WWWW</div>
 <div id="id9" style="background-color:blue; color:black;">WWW</div>
 <div id="id11" style="background-color:blue; color:black;"><img src='smeter'/></div>
 <div id="id13" style="background-color:blue; color:black;"><img src='bigfreq'/></div>
 <div id="id14" style="background-color:blue; color:black;">WWWWWWWWW</div>
 <div id="id15" style="background-color:blue; color:black;">WWWWWWWWW</div>
 <div id="id16" style="background-color:blue; color:black;">WWW</div>
 <div id="id17" style="background-color:blue; color:black;">WWW</div>
 <div id="id18" style="background-color:blue; color:black;">WWW</div>
 <div id="id19" style="background-color:blue; color:black;">WWW</div>
 <div id="id20" style="background-color:blue; color:black;">WWW</div>
 <div id="id21" style="background-color:blue; color:black;">WWWWW</div>
 <div id="id23" style="background-color:blue; color:black;">WWW</div>
 <div id="id24" style="background-color:blue; color:black;">WWWWWWWWWWWW</div>
 <div id="id25" style="background-color:blue; color:black;">WWW</div>
 <div id="id26" style="background-color:blue; color:black;">WWW</div>
 <div id="id27" style="background-color:blue; color:black;">WWWW</div>
 <div id="id28" style="background-color:blue; color:black;">WWWWW</div>
 <div id="id29" style="background-color:blue; color:black;">WWWWWW</div>
 <div id="id30" style="background-color:blue; color:black;">WWW</div>
 <div id="id31" style="background-color:blue; color:black;">WWW</div>
 <div id="id32" style="background-color:blue; color:black;">WWW</div>
 <div id="id33" style="background-color:blue; color:black;">WWWWW</div>
 <div id="id34" style="background-color:blue; color:black;">WWWWWWWW</div>
 <div id="id37" style="background-color:blue; color:black;"><img src='waterfal'/></div>
 <div id="id38" style="background-color:blue; color:black;">Q</div>
 <div id="id39" style="background-color:blue; color:black;">WWWWWWWWW</div>
 <div id="id40" style="background-color:blue; color:black;">WWWW</div>
 <div id="id41" style="background-color:blue; color:black;">WWW</div>
 <div id="id42" style="background-color:blue; color:black;">WWWWWWW</div>
</body>
</html>
)##";

	const char * display2_gethtml(uint_fast8_t page)
	{
		return hftrx_layout_html;
	}

#endif /* WITHRENDERHTML */

