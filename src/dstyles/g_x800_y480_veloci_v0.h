// переносной трансивер с сенсорным экраном
// стрелочный S-метр
// TFT панель 5" 800x480
// 480/5 = 96, 800/16=50

#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале (на этом дизайне нет, просто для того чтобы компилировлось)
enum {
	DLEM = 30,		// Menu window upper line
	DLES = 30 + 9,		// spectrum window upper line
	DLEB = 96 - GRID2Y(1) - 1,		// 96-5	- bottom line
	MIDMENU = 30,
	//
	DLE_unused
};

/* параметры спектра/водопада */
enum
{
	BDTH_ALLRX = (50), 		// ширина зоны для отображение графического окна на индикаторе
	BDCV_ALLRX = (DLEB - DLES) - 1,	// количество cells, отведенное под панораму и волопад.
	//
	BDTH_ALL = 50, 		// ширина индикатора
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
#define DISPLC_RADIUS 	5	// радиус закругления углов плиток в dzones

// 480/5 = 96, 800/16=50
// 272/5 = 54, 480/16=30 (old)




static const dzone_t dzones [] =
{
	{	0,	0,	0, 0, display2_preparebg,	& dzi_default, REDRSUBSET_SHOW, }, // Стирание фона

	{	0,	0,	9,	5,	display2_ENC1F,	& dzi_compat, PGALL, },
	{	9,	0,	9,	5,	display2_ENC2F,	& dzi_compat, PGALL, },

//	{	0,	0, 	7,	5,	display_siglevel7, 	& dzi_siglevel, PGALL, },	// signal level dBm
	//{	0,	0, 	5,	5,	display2_smeors5, 	& dzi_compat, PGALL, },	// уровень сигнала в баллах S или dBm
	//{	10,	0,	4,	5,	display2_rxbwval4,	& dzi_rxbwval, PGALL, },	// RX BW value
	{	19,	0,	3,	5,	display_txrxstate2, 	& dzi_txrx, PGALL, },
	{	22,	0,	5,	5,	display2_att4,		& dzi_attenuator, PGALL, },
	{	27,	0,	4,	5,	display2_preovf3,		& dzi_preamp_ovf, PGALL, },

	{	32,	0,	9,	5,	display2_ENC3F,	& dzi_compat, PGALL, },
	{	41,	0,	9,	5,	display2_ENC4F,	& dzi_compat, PGALL, },


#if WITHBARS
	{   0, 	5,  0,	0,	display2_smeter15_init,	NULL, PGINI, },	//  Инициализация стрелочного прибора
	{   0, 	5,  SM_BG_W_CELLS, SM_BG_H_CELLS, display2_smeter15,		& dzi_compat, PGALL, },	//  Инициализация стрелочного прибора
//	{   0, 	5,	15,	19,	display2_dummy, 		& dzi_smtr2, PGALL, },	// Placeholder
#endif /* WITHBARS */
#if WITHAFSPECTRE
	{	0,	5,	0,	0,	display2_af_spectre15_init,		NULL, PGINI, },
	{	0,	5,	0,	0,	display2_af_spectre15_latch,	NULL,	PGLATCH, },
	{	0,	5,	SM_BG_W_CELLS,	SM_BG_H_CELLS,	display2_af_spectre15,			& dzi_compat, PGALL, },
#endif /* WITHAFSPECTRE */

	{	15,	8,	0, 0, display2_freqX_a_init,	& dzi_default, PGINI, },	// MAIN FREQ Частота (большие цифры)
	{	15,	8,	21, 11,	display2_freqX_a,	& dzi_freqa, PGALL, },	// MAIN FREQ Частота Герцы маленьким шрифтом.


	{	36, 10,	4,	5,	display2_mode3_a,	& dzi_modea,	PGALL, },	// SSB/CW/AM/FM/...
	{	40, 10,	4,	5,	display2_rxbw3,	& dzi_rxbw, PGALL, },	// 3.1 / 0,5 / WID / NAR
	//{	44, 10,	6,	5,	display2_agc3,	& dzi_agc, PGALL, },	// AGC mode
	{	44,	10,	6,	5,	display2_rxbwval4,	& dzi_rxbwval, PGALL, },	// RX BW value

	{	36, 15,	4,	5,	display2_nr3,	& dzi_nr, PGALL, },	// NR : was: AGC
	{	40, 15,	4,	5,	display2_datamode3,	& dzi_datamode, PGALL, },	// DATA mode indicator
	{	44, 15,	6,	5,	display2_notch5,	& dzi_notch, PGALL, },	// NOTCH on/off

	{	16, 20,	4,	5,	display2_mainsub3,	& dzi_compat, PGALL, },	// main/sub RX: A/A, A/B, B/A, etc
	{	20,	20,	4,	5,	display2_vfomode3,	& dzi_vfomode, PGALL, },	// SPL
	{	24,	20,	12,	5,	display2_freqX_b,	& dzi_freqb, PGALL, },	// SUB FREQ 144.150.000
	{	36, 20,	4,	5,	display2_mode3_b,	& dzi_modeb,	PGALL, },	// SSB/CW/AM/FM/...
	{	40, 20,	4,	5,	display2_voxtune3,	& dzi_voxtune, PGALL, },	// VOX
	{	44,	20,	6,	5,	display2_dummy, & dzi_default, PGALL, },	// Placeholder

	// размещены под S-метром (15 ячеек)
	{	0, 	25,	6,	5,	display2_voltlevelV, & dzi_voltlevel, PGALL, },	// voltmeter with "V"
	{	6, 	25,	6,	5,	display2_currlevelA, & dzi_currlevel, PGALL, },	// PA drain current d.dd with "A"

	{	18,	25,	6,	5,	display2_ant5,	& dzi_antenna, PGALL, },
	{	24, 25,	4,	5,	display2_byp3,	& dzi_bypass, PGALL, },	// TUNER BYPASS state (optional)
	{ 	28, 25, 4,	5,	display2_dummy, & dzi_default, PGALL, },	// Placeholder
	{	32, 25,	4,	5,	display2_rec3,	& dzi_rec, PGALL, },	// Отображение режима записи аудио фрагмента
	{	36, 25,	4,	5,	display2_spk3,	& dzi_spk, PGALL, },	// отображение признака включения динамика
	{	40, 25, 4,	5,	display2_bkin3,	& dzi_bkin, PGALL, },	// BREAK-IN
	//{	44,	25,	6,	5,	display2_wpm5, & dzi_wpm, PGALL, },	// 22WPM
	{	44,	25,	6,	5,	display2_lockstate4, & dzi_lock, PGALL, },	// LOCK

	//{	24, 30,	10,	5,	display2_freqmeter10, & dzi_default, PGALL, },	// измеренная частота опоры
	//{	36, 30,	8,	5,	display2_freqdelta8, & dzi_default, PGALL, },	// выход ЧМ демодулятора

	{	0,	DLES,	BDTH_ALLRX,	DLEB - DLES - 1,	display2_wfl_init,		NULL,	PGINI, },	// формирование палитры водопада
	{	0,	DLES,	BDTH_ALLRX,	DLEB - DLES - 1,	display2_latchcombo,	NULL,	PGLATCH, },	// формирование данных спектра для последующего отображения спектра или водопада
	{	0,	DLES,	BDTH_ALLRX,	DLEB - DLES - 1,	display2_gcombo,		& dzi_gcombo, PGWFL | PGSPE, },// подготовка изображения спектра и волрада

	{	0,	DLEB,	13,	5,	display2_datetime12,	& dzi_datetime12, PGALL,	},	// DATE&TIME Jan-01 13:40
	//{	13,	DLEB,	10,	5,	display2_span9,			& dzi_default, PGALL, },	/* Получить информацию об ошибке настройки в режиме SAM */
#if WITHMGLOOP
	{	13,	DLEB, 	10, 5, display2_mla9,		& dzi_compat, PGALL, },
#endif /* WITHMGLOOP */
	{	23, DLEB,	6,	5,	display2_thermo,		& dzi_thermo, PG0, },	// thermo sensor
	{	29, DLEB,	4,	5,	display2_usbsts3,		& dzi_usbact, PG0, },	// USB host status
	{	33, DLEB,	3,	5,	display2_btsts2,		& dzi_btact, PG0, },	// USB host status
	{	36, DLEB,	8,	5,	display2_classa7,		& dzi_classa, PG0, },	// Class-A power amplifier
	{	44, DLEB, 	6,  5, 	display2_rxctcss5, 		& dzi_compat, PG0, },
	//{	28, DLEB,	10,	5,	display2_freqmeter10, 	& dzi_default, PGALL, },	// измеренная частота опоры

#if WITHMENU
	{	0, DLEM,	BDTH_ALLRX, (DLEB - DLEM) - 1, display2_multilinemenu_block,	& dzi_compat, REDRSUBSET_MENU, }, //Блок с пунктами меню (группы)
#if WITHAUTOTUNER
	{	29, DLEB,	21,	5,			display2_swrsts20,	& dzi_compat, REDRSUBSET_MENU, },	// SWR METER display
#endif /* WITHAUTOTUNER */
	// Middle bar
	{	0, MIDMENU,	BDTH_ALLRX,	8,	display2_midbar,  	& dzi_compat, PG0, },
#endif /* WITHMENU */

	// sleep mode display
	{	5,	25,	13,	4,	display2_datetime12,		& dzi_datetime12, PGSLP, },	// DATE & TIME // DATE&TIME Jan-01 13:40
	{	20, 25,	6,	4,	display2_voltlevelV, 	& dzi_voltlevel, PGSLP, },	// voltmeter with "V"

	{	0,	0,	0, 0, display2_showmain,	& dzi_default, REDRSUBSET_SHOW, }, // запись подготовленного изображения на главный дисплей
};

#if WITHMENU
void display2_getmultimenu(multimenuwnd_t * p)
{
	enum { YSTEP = 5 };		// количество ячеек разметки на одну строку меню
	p->multilinemenu_max_rows = (DLEB - DLEM) / YSTEP;
	p->ystep = YSTEP;	// количество ячеек разметки на одну строку меню
	p->reverse = 1;
	p->valuew = 9;	/* количество текстовых символов занимаемых полем вывола значения в меню. */
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
 #id1 { position:absolute; left:0px; top:0px; width:144px; height:25px; }
 #id2 { position:absolute; left:144px; top:0px; width:144px; height:25px; }
 #txrx { position:absolute; left:304px; top:0px; width:48px; height:25px; }
 #ant { position:absolute; left:352px; top:0px; width:80px; height:25px; }
 #ant { position:absolute; left:432px; top:0px; width:64px; height:25px; }
 #id6 { position:absolute; left:512px; top:0px; width:144px; height:25px; }
 #id7 { position:absolute; left:656px; top:0px; width:144px; height:25px; }
 #id9 { position:absolute; left:0px; top:25px; width:240px; height:100px; }
 #freq-a { position:absolute; left:240px; top:40px; width:336px; height:55px; }
 #mode-a { position:absolute; left:576px; top:50px; width:64px; height:25px; }
 #rxbw { position:absolute; left:640px; top:50px; width:64px; height:25px; }
 #rxbwval { position:absolute; left:704px; top:50px; width:96px; height:25px; }
 #nr { position:absolute; left:576px; top:75px; width:64px; height:25px; }
 #datamode { position:absolute; left:640px; top:75px; width:64px; height:25px; }
 #notch { position:absolute; left:704px; top:75px; width:96px; height:25px; }
 #id18 { position:absolute; left:256px; top:100px; width:64px; height:25px; }
 #vfomode { position:absolute; left:320px; top:100px; width:64px; height:25px; }
 #freq-b { position:absolute; left:384px; top:100px; width:192px; height:25px; }
 #mode-b { position:absolute; left:576px; top:100px; width:64px; height:25px; }
 #voxtune { position:absolute; left:640px; top:100px; width:64px; height:25px; }
 #default { position:absolute; left:704px; top:100px; width:96px; height:25px; }
 #voltlevel { position:absolute; left:0px; top:125px; width:96px; height:25px; }
 #currlevel { position:absolute; left:96px; top:125px; width:96px; height:25px; }
 #ant { position:absolute; left:288px; top:125px; width:96px; height:25px; }
 #byp { position:absolute; left:384px; top:125px; width:64px; height:25px; }
 #default { position:absolute; left:448px; top:125px; width:64px; height:25px; }
 #rec { position:absolute; left:512px; top:125px; width:64px; height:25px; }
 #spk { position:absolute; left:576px; top:125px; width:64px; height:25px; }
 #bkin { position:absolute; left:640px; top:125px; width:64px; height:25px; }
 #lock { position:absolute; left:704px; top:125px; width:96px; height:25px; }
 #gcombo { position:absolute; left:0px; top:195px; width:800px; height:250px; }
 #datetime12 { position:absolute; left:0px; top:450px; width:208px; height:25px; }
 #thermo { position:absolute; left:368px; top:450px; width:96px; height:25px; }
 #usbact { position:absolute; left:464px; top:450px; width:64px; height:25px; }
 #bt { position:absolute; left:528px; top:450px; width:48px; height:25px; }
 #classa { position:absolute; left:576px; top:450px; width:128px; height:25px; }
 #id41 { position:absolute; left:704px; top:450px; width:96px; height:25px; }
 #id44 { position:absolute; left:0px; top:150px; width:800px; height:40px; }
</style>
</head>
<body style="background-color:orange;">
 <div id="id1" style="background-color:blue; color:black;">WWWWWWWWW</div>
 <div id="id2" style="background-color:blue; color:black;">WWWWWWWWW</div>
 <div id="txrx" style="background-color:blue; color:black;"><img src='txrx'/></div>
 <div id="ant" style="background-color:blue; color:black;"><img src='ant'/></div>
 <div id="ant" style="background-color:blue; color:black;"><img src='ant'/></div>
 <div id="id6" style="background-color:blue; color:black;">WWWWWWWWW</div>
 <div id="id7" style="background-color:blue; color:black;">WWWWWWWWW</div>
 <div id="id9" style="background-color:blue; color:black;">WWWWWWWWWWWWWWW</div>
 <div id="freq-a" style="background-color:blue; color:black;"><img src='freq-a'/></div>
 <div id="mode-a" style="background-color:blue; color:black;"><img src='mode-a'/></div>
 <div id="rxbw" style="background-color:blue; color:black;"><img src='rxbw'/></div>
 <div id="rxbwval" style="background-color:blue; color:black;"><img src='rxbwval'/></div>
 <div id="nr" style="background-color:blue; color:black;"><img src='nr'/></div>
 <div id="datamode" style="background-color:blue; color:black;"><img src='datamode'/></div>
 <div id="notch" style="background-color:blue; color:black;"><img src='notch'/></div>
 <div id="id18" style="background-color:blue; color:black;">WWWW</div>
 <div id="vfomode" style="background-color:blue; color:black;"><img src='vfomode'/></div>
 <div id="freq-b" style="background-color:blue; color:black;"><img src='freq-b'/></div>
 <div id="mode-b" style="background-color:blue; color:black;"><img src='mode-b'/></div>
 <div id="voxtune" style="background-color:blue; color:black;"><img src='voxtune'/></div>
 <div id="default" style="background-color:blue; color:black;"><img src='default'/></div>
 <div id="voltlevel" style="background-color:blue; color:black;"><img src='voltlevel'/></div>
 <div id="currlevel" style="background-color:blue; color:black;"><img src='currlevel'/></div>
 <div id="ant" style="background-color:blue; color:black;"><img src='ant'/></div>
 <div id="byp" style="background-color:blue; color:black;"><img src='byp'/></div>
 <div id="default" style="background-color:blue; color:black;"><img src='default'/></div>
 <div id="rec" style="background-color:blue; color:black;"><img src='rec'/></div>
 <div id="spk" style="background-color:blue; color:black;"><img src='spk'/></div>
 <div id="bkin" style="background-color:blue; color:black;"><img src='bkin'/></div>
 <div id="lock" style="background-color:blue; color:black;"><img src='lock'/></div>
 <div id="gcombo" style="background-color:blue; color:black;"><img src='gcombo'/></div>
 <div id="datetime12" style="background-color:blue; color:black;"><img src='datetime12'/></div>
 <div id="thermo" style="background-color:blue; color:black;"><img src='thermo'/></div>
 <div id="usbact" style="background-color:blue; color:black;"><img src='usbact'/></div>
 <div id="bt" style="background-color:blue; color:black;"><img src='bt'/></div>
 <div id="classa" style="background-color:blue; color:black;"><img src='classa'/></div>
 <div id="id41" style="background-color:blue; color:black;">WWWWWW</div>
 <div id="id44" style="background-color:blue; color:black;">             WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW</div>
</body>
</html>
)##";

	const char * display2_gethtml(uint_fast8_t page)
	{
		return hftrx_layout_html;
	}

#endif /* WITHRENDERHTML */


