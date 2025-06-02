// переносной трансивер с сенсорным экраном
// стрелочный S-метр
// TFT панель 5" 800x480
// 480/5 = 96, 800/16=50

#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале (на этом дизайне нет, просто для того чтобы компилировлось)

enum {
	DLEM = 30,		// Menu window upper line
	DLES = 30 + GRID2Y(2),		// spectrum window upper line
	DLEB = 96 - GRID2Y(1),		// 96-5	- bottom line
	MIDLABEL = 30,
	MIDVALUE = MIDLABEL + GRID2Y(1),
	//
	DLE_unused
};

/* параметры спектра/водопада */
enum
{
	BDTH_ALLRX = CHARS2GRID(50), 		// ширина зоны для отображение графического окна на индикаторе
	BDCV_ALLRX = ROWS2GRID(DLEB - DLES) - 1,	// количество cells, отведенное под панораму и волопад.
	//
	B_unused
};

/* параметры S-метра */
enum
{
	BDTH_ALLRXBARS = 30,	// ширина зоны для отображение барграфов на индикаторе


	BDTH_LEFTRX = 17,	// ширина индикатора баллов (без плюсов)
	BDTH_RIGHTRX = BDTH_ALLRXBARS - BDTH_LEFTRX,	// ширина индикатора плюсов
	BDTH_SPACERX = 0,
	BDTH_ALLSWR = 16,
	BDTH_SPACESWR = 2,
	BDTH_ALLPWR = 12,
	BDTH_SPACEPWR = 0,
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

#define DISPLC_WIDTH	9	// количество цифр в отображении частоты
#define DISPLC_RJ		0	// количество скрытых справа цифр в отображении частоты

// 480/5 = 96, 800/16=50
// 272/5 = 54, 480/16=30 (old)
//#define GRID2X(cellsx) ((cellsx) * 16)	/* перевод ячеек сетки разметки в номер пикселя по горизонталм */
//#define GRID2Y(cellsy) ((cellsy) * 5)	/* перевод ячеек сетки разметки в номер пикселя по вертикали */
//#define SMALLCHARH 15 /* Font height */
//#define SMALLCHARW 16 /* Font width */
static const dzone_t dzones [] =
{
	{	0,	0,	0, 0, display2_preparebg,	& dzi_default, REDRSUBSET_SHOW, }, // Стирание фона

	{	0,	0,	0,	0,	display2_keyboard_screen0,	& dzi_default, PGALL | REDRSUBSET_SLEEP, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана

	{	0,	0,	9,	4,	display2_ENC1F_9,	& dzi_default, PGALL, },
	{	10,	0,	9,	4,	display2_ENC2F_9,	& dzi_default, PGALL, },

	//	{	0,	0, 	7,	0,	display_siglevel7, 	& dzi_default, PGALL, },	// signal level dBm
	//{	0,	0, 	5,	0,	display2_smeors5, 	& dzi_default, PGALL, },	// уровень сигнала в баллах S или dBm
	//{	10,	0,	4,	0,	display2_rxbwval4,	& dzi_default, PGALL, },	// RX BW value
	//{	15,	0,	2,	0,	display_txrxstate2, & dzi_default, PGALL, },
	{	20,	0,	4,	4,	display2_att4,		& dzi_default, PGALL, },
	{	25,	0,	3,	4,	display2_preovf3,	& dzi_default, PGALL, },

//	{	41, 0,	9,	4,	display2_fnlabel9,	& dzi_default, PGALL, },	// FUNC item label
//	{	41,	4,	9,	4,	display2_fnvalue9,	& dzi_default, PGALL, },	// FUNC item value

	{	31,	0,	9,	4,	display2_ENC3F_9,	& dzi_default, PGALL, },
	{	41,	0,	9,	4,	display2_ENC4F_9,	& dzi_default, PGALL, },


#if WITHBARS
	{   0, 	4,  0,	0,	display2_smeter15_init,& dzi_default, PGINI, },	//  Инициализация стрелочного прибора
	{   0, 	4,	15,	20,	display2_smeter15, 	& dzi_default, PGALL, },	// Изображение стрелочного прибора
#endif /* WITHBARS */
#if WITHAFSPECTRE
	{	0,	4,	0,	0,	display2_af_spectre15_init,	& dzi_default, PGINI, },
	{	0,	4,	0,	0,	display2_af_spectre15_latch,	& dzi_default,	PGLATCH, },
	{	0,	4,	15,	20,	display2_af_spectre15,		& dzi_default, PGSPE, },
#endif /* WITHAFSPECTRE */

	{	15,	6,	0,	0,	display2_freqX_a_init,	& dzi_default, PGINI, },	// MAIN FREQ Частота (большие цифры)
	{	15,	6,	21, 13,	display2_freqX_a,	& dzi_freqa, PGALL, },	// MAIN FREQ Частота Герцы маленьким шрифтом.


	{	37, 10,	3,	4,	display2_mode3_a,	& dzi_modea,	PGALL, },	// SSB/CW/AM/FM/...
	{	41, 10,	3,	4,	display2_rxbw3,		& dzi_default, PGALL, },	// 3.1 / 0,5 / WID / NAR
	{	46, 10,	3,	4,	display2_agc3,		& dzi_default, PGALL, },	// AGC mode

	{	37, 15,	3,	4,	display2_nr3,		& dzi_default, PGALL, },	// NR : was: AGC
	{	41, 15,	3,	4,	display2_datamode3,	& dzi_default, PGALL, },	// DATA mode indicator
	{	45, 15,	5,	4,	display2_notch5,	& dzi_default, PGALL, },	// NOTCH on/off

	{	15, 20,	3,	4,	display2_mainsub3,	& dzi_default, PGALL, },	// main/sub RX: A/A, A/B, B/A, etc
	{	20,	20,	3,	4,	display2_vfomode3,	& dzi_default, PGALL, },	// SPL
	{	24,	20,	12,	4,	display2_freqX_b,	& dzi_freqb, PGALL, },	// SUB FREQ
	{	37, 20,	3,	4,	display2_mode3_b,	& dzi_modeb,	PGALL, },	// SSB/CW/AM/FM/...
	{	41, 20,	3,	4,	display2_voxtune3,	& dzi_default, PGALL, },	// VOX
	//{	45,	20,	4,	4,	display2_lockstate4, & dzi_default, PGALL, },	// LOCK

	// размещены под S-метром (15 ячеек)
	{	1, 	25,	5,	4,	display2_voltlevelV5, & dzi_default, PGALL, },	// voltmeter with "V"
	{	7, 	25,	0,	4,	display2_currlevelA6, & dzi_default, PGALL, },	// PA drain current d.dd with "A"

	{	14,	25,	4,	4,	display2_rxbwval4,	& dzi_default, PGALL, },	// RX BW value
	{	19,	25,	5,	4,	display2_ant5,		& dzi_default, PGALL, },
	{	25, 25,	3,	4,	display2_byp3,		& dzi_default, PGALL, },	// TUNER BYPASS state (optional)

	{	33, 25,	3,	4,	display2_rec3,		& dzi_default, PGALL, },	// Отображение режима записи аудио фрагмента
	{	37, 25,	3,	4,	display2_spk3,		& dzi_default, PGALL, },	// отображение признака включения динамика
	{	41, 25, 3,	4,	display2_bkin3,		& dzi_default, PGALL, },	// BREAK-IN
	//{	45,	25,	5,	4,	display2_wpm5, 		& dzi_default, PGALL, },	// 22WPM
	{	45,	25,	4,	4,	display2_lockstate4, & dzi_default, PGALL, },	// LOCK

	//{	24, 30,	10,	4,	display_freqmeter10, & dzi_default, PGALL, },	// измеренная частота опоры
	//{	37, 30,	8,	4,	display2_freqdelta8, & dzi_default, PGALL, },	// выход ЧМ демодулятора

#if 1
	// Middle bar
	{	0, MIDLABEL,	BDTH_ALLRX,	4,	display2_midlabel,  & dzi_default, PG0, },
	{	0, MIDVALUE,	BDTH_ALLRX,	4,	display2_midvalue,  & dzi_default, PG0, },
#endif

#if WITHSPECTRUMWF
	{	0,	DLES,	BDTH_ALLRX,	BDCV_ALLRX,	display2_wfl_init,	& dzi_default,	PGINI, },	// формирование палитры водопада
	{	0,	DLES,	BDTH_ALLRX,	BDCV_ALLRX,	display2_latchcombo,	& dzi_default,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
	{	0,	DLES,	BDTH_ALLRX,	BDCV_ALLRX,	display2_gcombo,	& dzi_default, PGWFL | PGSPE, },// подготовка изображения спектра и волрада
#endif /* WITHSPECTRUMWF */

	{	0,	DLEB,	12,	4,	display2_datetime12,	& dzi_default, PGALL,	},	// DATE&TIME Jan-01 13:40
	//{	13,	DLEB,	9,	4,	display2_span9,		& dzi_default, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */
	{	23, DLEB,	0,	4,	display2_thermo4,	& dzi_default, PGALL, },	// thermo sensor
	{	28, DLEB,	3,	4,	display2_usbsts3,		& dzi_default, PG0, },	// USB host status
	{	32, DLEB,	7,	4,	display2_classa7,		& dzi_default, PGALL, },	// Class-A power amplifier
	//{	28, DLEB,	10,	4,	display_freqmeter10, & dzi_default, PGALL, },	// измеренная частота опоры

#if WITHMENU
	{	0,				0,		0,	0,	display2_keyboard_menu,					& dzi_default, REDRSUBSET_MENU, }, // Обработка клавиатуры и валкодеров при нахождении в режиме меню
	{	3,				DLEM,	0,	0,	display2_multilinemenu_block_groups,	& dzi_default, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
	{	LABELW + 5,		DLEM,	0,	0,	display2_multilinemenu_block_params,	& dzi_default, REDRSUBSET_MENU, }, //Блок с пунктами меню (параметры)
	{	LABELW*2 + 6,	DLEM,	0,	0,	display2_multilinemenu_block_vals,	& dzi_default, REDRSUBSET_MENU, }, //Блок с пунктами меню (значения)
#if WITHAUTOTUNER
	{	28, 			DLEB,	22,	4,	display2_swrsts22,	& dzi_default, REDRSUBSET_MENU, },	// SWR METER display
#endif /* WITHAUTOTUNER */
#endif /* WITHMENU */

	// sleep mode display
	{	5,	25,	12,	4,	display2_datetime12,	& dzi_default, PGSLP, },	// DATE & TIME // DATE&TIME Jan-01 13:40
	{	20, 25,	5,	4,	display2_voltlevelV5, & dzi_default, PGSLP, },	// voltmeter with "V"
#if WITHMENU
	{	15, 25,	0,	0,	display2_popup,		& dzi_default, PG0, },	// Всплывающие меню. В конце массива для перекрытия всего что под ним
#endif /* WITHMENU */

#if 0
	{	0,	0,	0,	0,	display2_vtty_init,	& dzi_default,	PGINI, },	// Подготовка видеобуфера окна протокола
	{	0,	0, 0,	0,	display2_vtty,	& dzi_default, PG0, },		// Вывод текущего состояния протокола
	{	0,	(DIM_Y - GRID2Y(5)) / 5, 9,	5,	 display2_freqsof9,	& dzi_default, PG0, },		// Вывод текущего состояния протокола
#endif

	{	0,	0,	0, 0, display2_showmain,	& dzi_default, REDRSUBSET_SHOW, }, // запись подготовленного изображения на главный дисплей

#if WITHHDMITVHW && 1
	// Второй дисплей с отличающимся дизайном
	{	0,	0,	0, 0, display2_preparebg,	& dzi_default, REDRSUBSET_SHOW, }, // Стирание фона

	{	15,	6,	21, 13,	display2_freqX_a,	& dzi_default, PGALL, },	// MAIN FREQ Частота Герцы маленьким шрифтом.
	{	37, 10,	3,	4,	display2_mode3_a,	& dzi_default,	PGALL, },	// SSB/CW/AM/FM/...
	{	41, 10,	3,	4,	display2_rxbw3,		& dzi_default, PGALL, },	// 3.1 / 0,5 / WID / NAR
	{	0,	20,	BDTH_ALLRX,	DLEB - 20,	display2_gcombo,	& dzi_default, PGALL, },// подготовка изображения спектра

	{	0,	0,	0, 0, display2_showhdmi,	& dzi_default, REDRSUBSET_SHOW, }, // запись подготовленного изображения на главный дисплей
#endif
};

#if WITHMENU
void display2_getmultimenu(multimenuwnd_t * p)
{
	enum { YSTEP = 5 };		// количество ячеек разметки на одну строку меню
	p->multilinemenu_max_rows = (DLEB - DLEM) / YSTEP;
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
 #id1 { position:absolute; left:0px; top:0px; width:144px; height:20px; }
 #id2 { position:absolute; left:160px; top:0px; width:144px; height:20px; }
 #id3x { position:absolute; left:320px; top:0px; width:64px; height:20px; }
 #id3 { z-index:999; position:absolute; left:400px; top:120px; width:120px; height:90px; }
 #id4 { position:absolute; left:400px; top:0px; width:48px; height:20px; }
 #id5 { position:absolute; left:496px; top:0px; width:144px; height:20px; }
 #id6 { position:absolute; left:656px; top:0px; width:144px; height:20px; }
 #id8 { position:absolute; left:0px; top:20px; width:240px; height:100px; }
 #id10 { position:absolute; left:240px; top:30px; width:336px; height:65px; }
 #id11 { position:absolute; left:592px; top:50px; width:48px; height:20px; }
 #id12 { position:absolute; left:656px; top:50px; width:48px; height:20px; }
 #id13 { position:absolute; left:736px; top:50px; width:48px; height:20px; }
 #id14 { position:absolute; left:592px; top:75px; width:48px; height:20px; }
 #id15 { position:absolute; left:656px; top:75px; width:48px; height:20px; }
 #id16 { position:absolute; left:720px; top:75px; width:80px; height:20px; }
 #id17 { position:absolute; left:240px; top:100px; width:48px; height:20px; }
 #id18 { position:absolute; left:320px; top:100px; width:48px; height:20px; }
 #id19 { position:absolute; left:384px; top:100px; width:192px; height:20px; }
 #id20 { position:absolute; left:592px; top:100px; width:48px; height:20px; }
 #id21 { position:absolute; left:656px; top:100px; width:48px; height:20px; }
 #id22 { position:absolute; left:16px; top:125px; width:80px; height:20px; }
 #id24 { position:absolute; left:224px; top:125px; width:64px; height:20px; }
 #id25 { position:absolute; left:304px; top:125px; width:80px; height:20px; }
 #id26 { position:absolute; left:400px; top:125px; width:48px; height:20px; }
 #id27 { position:absolute; left:528px; top:125px; width:48px; height:20px; }
 #id28 { position:absolute; left:592px; top:125px; width:48px; height:20px; }
 #id29 { position:absolute; left:656px; top:125px; width:48px; height:20px; }
 #id30 { position:absolute; left:720px; top:125px; width:64px; height:20px; }
 #id31 { position:absolute; left:592px; top:150px; width:128px; height:20px; }
 #id36 { position:absolute; left:0px; top:455px; width:192px; height:20px; }
 #id38 { position:absolute; left:448px; top:455px; width:48px; height:20px; }
 #id39 { position:absolute; left:512px; top:455px; width:112px; height:20px; }
 .on { background-color:red; color:white; visibility:visible; }
 .off { background-color:green; color:black; visibility:hidden; }
</style>
</head>
<body style="background-color:orange;">
 <div id="id1" class="on">WWWWWWWWW</div>
 <div id="id2" class="on">WWWWWWWWW</div>
 <div id="id3x" class="on off">WWWW</div>
 <div id="id3sssss" class="on off">Menu</div>
<div id="id3" class="on off">
    <div class="menu-head">Select band</div>
    <table style="width: 100%">
        <tr>
            <td><div class="menu-click">160m</div></td>
            <td><div class="menu-click">80m</div></td>
            <td><div class="menu-click">40m</div></td>
            <td><div class="menu-click">30m</div></td>
        </tr>
        <tr>
            <td><div class="menu-click">20m</div></td>
            <td><div class="menu-click on">17m</div></td>
            <td><div class="menu-click">15m</div></td>
            <td><div class="menu-click">12m</div></td>
        </tr>
        <tr>
            <td><div class="menu-click">10m</div></td>
            <td><div class="menu-click">6m</div></td>
            <td><div class="menu-click">2m</div></td>
            <td><div class="menu-click">70cm</div></td>
        </tr>
        <tr>
            <td><div class="menu-click">23cm</div></td>
            <td></td>
            <td></td>
            <td></td>
        </tr>
    </table>
</div>

 <div id="id4" style="background-color:blue; color:black;">WWW</div>
 <div id="id5" style="background-color:blue; color:black;">WWWWWWWWW</div>
 <div id="id6" style="background-color:blue; color:black;">WWWWWWWWW</div>
 <div id="id8" style="background-color:blue; color:black;">WWWWWWWWWWWWWWW</div>
 <div id="id10" style="background-color:blue; color:black;">WWWWWWWWWWWWWWWWWWWWW</div>
 <div id="id11" style="background-color:blue; color:black;">WWW</div>
 <div id="id12" style="background-color:blue; color:black;">WWW</div>
 <div id="id13" style="background-color:blue; color:black;">WWW</div>
 <div id="id14" style="background-color:blue; color:black;">WWW</div>
 <div id="id15" style="background-color:blue; color:black;">WWW</div>
 <div id="id16" style="background-color:blue; color:black;">WWWWW</div>
 <div id="id17" style="background-color:blue; color:black;">WWW</div>
 <div id="id18" style="background-color:blue; color:black;">WWW</div>
 <div id="id19" style="background-color:blue; color:black;">WWWWWWWWWWWW</div>
 <div id="id20" style="background-color:blue; color:black;">WWW</div>
 <div id="id21" style="background-color:blue; color:black;">WWW</div>
 <div id="id22" style="background-color:blue; color:black;">WWWWW</div>
 <div id="id24" style="background-color:blue; color:black;">WWWW</div>
 <div id="id25" style="background-color:blue; color:black;">WWWWW</div>
 <div id="id26" style="background-color:blue; color:black;">WWW</div>
 <div id="id27" style="background-color:blue; color:black;">WWW</div>
 <div id="id28" style="background-color:blue; color:black;">WWW</div>
 <div id="id29" style="background-color:blue; color:black;">WWW</div>
 <div id="id30" style="background-color:blue; color:black;">WWWW</div>
 <div id="id31" style="background-color:blue; color:black;">WWWWWWWW</div>
 <div id="id36" style="background-color:blue; color:black;">WWWWWWWWWWWW</div>
 <div id="id38" style="background-color:blue; color:black;">WWW</div>
 <div id="id39" style="background-color:blue; color:black;">WWWWWWW</div>
</body>
</html>
)##";

	const char * display2_gethtml(uint_fast8_t page)
	{
		return hftrx_layout_html;
	}

#endif /* WITHRENDERHTML */


