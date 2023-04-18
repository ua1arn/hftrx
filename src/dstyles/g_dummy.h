
#if WITHSHOWSWRPWR	/* на дисплее одновременно отображаются SWR-meter и PWR-meter */
	//					"012345678901234567890123"
	#define SWRPWRMAP	"1   2   3   4  0% | 100%"
	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
#else
	//					"012345678901234567890123"
	#define POWERMAP	"0    25    50   75   100"
	#define SWRMAP		"1   |   2  |   3   |   4"	//
	#define SWRMAX	(SWRMIN * 40 / 10)	// 4.0 - значение на полной шкале
#endif
//						"012345678901234567890123"
#define SMETERMAP		"1  3  5  7  9 +20 +40 60"
enum
{
	BDTH_ALLRXBARS = 24,	// ширина зоны для отображение барграфов на индикаторе
	BDTH_ALLRX = 40,	// ширина зоны для отображение графического окна на индикаторе

	BDTH_LEFTRX = 12,	// ширина индикатора баллов (без плюслв)
	BDTH_RIGHTRX = BDTH_ALLRXBARS - BDTH_LEFTRX,	// ширина индикатора плюсов
	BDTH_SPACERX = 0,
	BDTH_ALLSWR = 13,
	BDTH_SPACESWR = 2,
	BDTH_ALLPWR = 9,
	BDTH_SPACEPWR = 0,

	BDCV_ALLRX = ROWS2GRID(55),	// количество строк, отведенное под S-метр, панораму, иные отображения
	/* совмещение на одном экрание водопада и панорамы */
	BDCO_SPMRX = ROWS2GRID(0),	// смещение спектра по вертикали в ячейках от начала общего поля
	BDCV_SPMRX = ROWS2GRID(27),	// вертикальный размер спектра в ячейках
	BDCO_WFLRX = BDCV_SPMRX,	// смещение водопада по вертикали в ячейках от начала общего поля
	BDCV_WFLRX = BDCV_ALLRX - BDCV_SPMRX	// вертикальный размер водопада в ячейках
};
enum {
	DLES = 35	// spectrum window upper line
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
	{	0,	0,	display2_dummy,	REDRM_MODE,	REDRSUBSET_SLEEP | REDRSUBSET_MENU },
	{	0,	0,	display2_keyboard_screen0,	REDRM_KEYB, PGALL | REDRSUBSET_SLEEP, }, // Обработка клавиатуры и валкодеров при нахождении в режиме основного экрана
	{	0,	0,	display2_keyboard_menu,		REDRM_KEYB, REDRSUBSET_MENU, }, // Обработка клавиатуры и валкодеров при нахождении в режиме меню
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
		p->y = 0;	// позиция верхнего левого угла в пикселях
		p->w = DIM_X; //GRID2X(CHARS2GRID(BDTH_ALLRX));	// размер по горизонтали в пикселях
		p->h = DIM_Y;				// размер по вертикали в пикселях
		//p->frame = (uintptr_t) 0;
	}
