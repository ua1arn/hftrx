/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Доработки для LS020 Василий Линывый, livas60@mail.ru
//

#ifndef DISPLAY_H_INCLUDED
#define DISPLAY_H_INCLUDED

#include "hardware.h"	/* зависящие от процессора функции работы с портами */


typedef uint_fast16_t COLOR565_T;
typedef uint16_t PACKEDCOLOR565_T;

#if LCDMODE_UC1601

	#include "uc1601s.h"

#endif /* LCDMODE_UC1601 */

#if LCDMODE_RDX0120 || LCDMODE_G1203H
	// Маленькие дисплеи с 32 точками по вертикали
	#include "pcf8535.h"

	#define HALFCOUNT_BIG 1		// big and half sizes
	#define HALFCOUNT_SMALL 1	// small size
	#define HALFCOUNT_SMALL2 1	// small2 size
	#define HALFCOUNT_BAR 1		// small size

	#define HALFCOUNT_FREQA HALFCOUNT_BIG
	#define DEFAULT_LCD_CONTRAST	100

#elif LCDMODE_RDX0077

	#define HALFCOUNT_BIG 2		// big and half sizes
	#define HALFCOUNT_SMALL 1	// small size
	#define HALFCOUNT_BAR 1		// small size

	#define HALFCOUNT_FREQA HALFCOUNT_BIG
	#define DEFAULT_LCD_CONTRAST	100 	//  на SW2011RDX питание 8 вольт

#elif LCDMODE_RDX0154 || LCDMODE_RDT065

	#define HALFCOUNT_BIG 2		// big and half sizes
	#define HALFCOUNT_SMALL 1	// small size
	#define HALFCOUNT_SMALL2 1	// small2 size
	#define HALFCOUNT_BAR 1		// small size

	#define HALFCOUNT_FREQA HALFCOUNT_BIG

	#define DEFAULT_LCD_CONTRAST	100 	//  на SW2011RDX питание 8 вольт

#elif LCDMODE_UC1608
	// 240*128

	#include "uc1608.h"

	#define HALFCOUNT_BIG 5		// big and half sizes
	#define HALFCOUNT_SMALL 2	// small size
	#define HALFCOUNT_SMALL2 1	// small2 size
	#define HALFCOUNT_BAR 1		// small size

	#define DEFAULT_LCD_CONTRAST	20

	#define HALFCOUNT_FREQA HALFCOUNT_BIG

#elif LCDMODE_LS020 || LCDMODE_LPH88 || LCDMODE_L2F50

	#if LCDMODE_L2F50
		#include "l2f50.h"
	#elif LCDMODE_LS020
		#include "ls020.h"
	#elif LCDMODE_LPH88
		#include "lph88.h"
	#else
		#error Wrong LCDMODE_xxx
	#endif

	#define HALFCOUNT_BIG 1		// big and half sizes
	#define HALFCOUNT_SMALL 1	// small size
	#define HALFCOUNT_SMALL2 1	// small2 size
	#define HALFCOUNT_BAR 1		// small size

	#define HALFCOUNT_FREQA HALFCOUNT_BIG

	#define DEFAULT_LCD_CONTRAST	255

#elif LCDMODE_S1D13781 || LCDMODE_LQ043T3DX02K

	#include "s1d13781.h"

	#define HALFCOUNT_BIG 1		// big and half sizes
	#define HALFCOUNT_SMALL 1	// small size
	#define HALFCOUNT_SMALL2 1	// small2 size
	#define HALFCOUNT_BAR 1		// small size

	#define HALFCOUNT_FREQA HALFCOUNT_BIG

#elif LCDMODE_WH2002 || LCDMODE_WH2004 || LCDMODE_WH2002_IGOR

	#include "hd44780.h"

	#define HALFCOUNT_BIG 1		// big and half sizes
	#define HALFCOUNT_SMALL 1	// small size
	#define HALFCOUNT_SMALL2 1	// small2 size
	#define HALFCOUNT_BAR 1		// small size

	#define HALFCOUNT_FREQA HALFCOUNT_BIG

#elif LCDMODE_WH1602 || LCDMODE_WH1604

	#include "hd44780.h"

	#define HALFCOUNT_BIG 1		// big and half sizes
	#define HALFCOUNT_SMALL 1	// small size
	#define HALFCOUNT_SMALL2 1	// small2 size
	#define HALFCOUNT_BAR 1		// small size

	#define HALFCOUNT_FREQA HALFCOUNT_BIG

#elif LCDMODE_ILI9320
	/* Индикатор 248*320 с контроллером ILI9320 */

	#include "ILI9320.h"

	#define HALFCOUNT_BIG 1		// big and half sizes
	#define HALFCOUNT_SMALL 1 // 2	// small size
	#define HALFCOUNT_SMALL2 1	// small2 size
	#define HALFCOUNT_BAR 1		// small size

	#define HALFCOUNT_FREQA HALFCOUNT_BIG

	#define DEFAULT_LCD_CONTRAST	255

#elif LCDMODE_ILI9225 || LCDMODE_ST7781
	/* Индикатор 178*220 с контроллером ILI9320 */
	// 320*240 ST7781

	#include "ILI9225.h"


	#define HALFCOUNT_BIG 1		// big and half sizes
	#define HALFCOUNT_SMALL 1	// small size
	#define HALFCOUNT_SMALL2 1	// small2 size
	#define HALFCOUNT_BAR 1		// small size

	#define HALFCOUNT_FREQA HALFCOUNT_BIG

#elif LCDMODE_ST7735 || LCDMODE_ILI9163 || LCDMODE_ILI9341
	/* Индикатор 160*128 с контроллером Sitronix ST7735 */
	/* Индикатор 176*132 с контроллером ILITEK ILI9163 */
	/* Индикатор 320*240 с контроллером ILITEK ILI9341 */

	#include "st7735.h"

	#define HALFCOUNT_BIG 1		// big and half sizes
	#define HALFCOUNT_SMALL 1	// small size
	#define HALFCOUNT_SMALL2 1	// small2 size
	#define HALFCOUNT_BAR 1		// small size

	#define HALFCOUNT_FREQA HALFCOUNT_BIG

#elif LCDMODE_ILI8961
	/* Индикатор 320*240 с контроллером ILITEK ILI8961 */

	#include "ili8961.h"

	#define HALFCOUNT_BIG 1		// big and half sizes
	#define HALFCOUNT_SMALL 1	// small size
	#define HALFCOUNT_SMALL2 1	// small2 size
	#define HALFCOUNT_BAR 1		// small size

	#define HALFCOUNT_FREQA HALFCOUNT_BIG

#elif LCDMODE_ST7565S || LCDMODE_PTE1206
	/* Индикатор WO12864C2-TFH# 128*64 с контроллером Sitronix ST7565S */

	#include "st7565s.h"


	#define HALFCOUNT_BIG 2		// big and half sizes
	#define HALFCOUNT_SMALL 1	// small size
	#define HALFCOUNT_SMALL2 1	// small2 size
	#define HALFCOUNT_BAR 1		// small size

	#define HALFCOUNT_FREQA HALFCOUNT_BIG

#elif LCDMODE_PCF8535 || LCDMODE_PCF8531

	#include "pcf8535.h"

	#define DEFAULT_LCD_CONTRAST	55 // 100

	#define HALFCOUNT_BIG 2		// big and half sizes
	#define HALFCOUNT_SMALL 1	// small size
	#define HALFCOUNT_SMALL2 1	// small2 size

	#define HALFCOUNT_FREQA HALFCOUNT_BIG
#else
	#error Undefined display type
#endif


#if LCDMODE_LTDC

	#if LCDMODE_LTDC_L24

		typedef uint_fast32_t COLOR_T;
		struct rgb_24b
		{
			uint8_t r, g, b;
		} __attribute__ ((packed));// аттрибут GCC, исключает "дыры" в структуре.

		typedef struct rgb_24b PACKEDCOLOR_T;	// 3 байта для последовательной выдачи как RGB для LCDMODE_ILI8961

		// RRRRRRR.GGGGGGGG.BBBBBBBB
		#define TFTRGB(red, green, blue) \
			(  (unsigned long) \
				(	\
					(((unsigned long) (red) << 16) & 0xFF0000ul)  | \
					(((unsigned long) (green) << 8) & 0xFF00ul) | \
					(((unsigned long) (blue) << 0) & 0xFFul) \
				) \
			)

  #elif LCDMODE_LTDC_L8

		/* При использовании frame buffer цвета восьмибитные */
		typedef uint_fast8_t COLOR_T;
		typedef uint8_t PACKEDCOLOR_T;


		// RRRGGGBB
		#define TFTRGB(red, green, blue) \
			(  (unsigned char) \
				(	\
					(((red) >> 0) & 0xe0)  | \
					(((green) >> 3) & 0x1c) | \
					(((blue) >> 6) & 0x03) \
				) \
			)

		// для формирования растра с изображением водопада и спектра
		// RRRR.RGGG.GGGB.BBBB
		#define TFTRGB565(red, green, blue) \
			(  (uint_fast16_t) \
				(	\
					(((uint_fast16_t) (red) << 8) &   0xf800)  | \
					(((uint_fast16_t) (green) << 3) & 0x07e0) | \
					(((uint_fast16_t) (blue) >> 3) &  0x001f) \
				) \
			)

	#else /* LCDMODE_LTDC_L8 */

		//#define LCDMODE_RGB565 1
		typedef uint_fast16_t COLOR_T;
		typedef uint16_t PACKEDCOLOR_T;

		// RRRR.RGGG.GGGB.BBBB
		#define TFTRGB(red, green, blue) \
			(  (uint_fast16_t) \
				(	\
					(((uint_fast16_t) (red) << 8) &   0xf800)  | \
					(((uint_fast16_t) (green) << 3) & 0x07e0) | \
					(((uint_fast16_t) (blue) >> 3) &  0x001f) \
				) \
			)

		// для формирования растра с изображением водопада и спектра
		#define TFTRGB565 TFTRGB

	#endif /* LCDMODE_LTDC_L8 */

#endif /* LCDMODE_LTDC */

#if LCDMODE_S1D13781
	// биты слова буфера располагаются на экране горизонтально
	// старший битт левее
	typedef uint16_t GX_t;	/* тип элмента буфера для выдачи монохромного растра */
	#define MGSIZE(dx, dy)	((dy) * (((unsigned long) (dx) + 15) / 16))	// размер буфера для монохромного растра
	#define GXSIZE(dx, dy)	((unsigned long) (dx) * (dy))	// размер буфера для цветного растра
#elif LCDMODE_COLORED
	// биты слова буфера располагаются на экране вертикально
	typedef uint8_t GX_t;	/* тип элмента буфера для выдачи монохромного растра */
	#define MGSIZE(dx, dy)	((dx) * (((unsigned long) (dy) + 7) / 8))	// размер буфера для монохромного растра
	#define GXSIZE(dx, dy)	((unsigned long) (dx) * (dy))	// размер буфера для цветного растра
#else	/* LCDMODE_S1D13781 */
	// биты слова буфера располагаются на экране вертикально
	typedef uint8_t GX_t;	/* тип элмента буфера для выдачи монохромного растра */
	#define MGSIZE(dx, dy)	((dx) * (((unsigned long) (dy) + 7) / 8))	// размер буфера для монохромного растра
	#define GXSIZE(dx, dy)	((unsigned long) (dx) * (dy))	// размер буфера для цветного растра
#endif	/* */

// определение основных цветов
///

/* RGB 24-bits color table definition (RGB888). */
#define COLOR_BLACK          TFTRGB(0x00, 0x00, 0x00)
#define COLOR_WHITE          TFTRGB(0xFF, 0xFF, 0xFF)
#define COLOR_BLUE           TFTRGB(0x00, 0x00, 0xFF)
#define COLOR_GREEN          TFTRGB(0x00, 0xFF, 0x00)
#define COLOR_RED            TFTRGB(0xFF, 0x00, 0x00)
#define COLOR_NAVY           TFTRGB(0x00, 0x00, 0x80)
#define COLOR_DARKBLUE       TFTRGB(0x00, 0x00, 0x8B)
#define COLOR_DARKGREEN      TFTRGB(0x00, 0x64, 0x00)
#define COLOR_DARKGREEN2     TFTRGB(0x00, 0x40, 0x00)
#define COLOR_DARKCYAN       TFTRGB(0x00, 0x8B, 0x8B)
#define COLOR_CYAN           TFTRGB(0x00, 0xFF, 0xFF)
#define COLOR_TURQUOISE      TFTRGB(0x40, 0xE0, 0xD0)
#define COLOR_INDIGO         TFTRGB(0x4B, 0x00, 0x82)
#define COLOR_DARKRED        TFTRGB(0x80, 0x00, 0x00)
#define COLOR_DARKRED2       TFTRGB(0x40, 0x00, 0x00)
#define COLOR_OLIVE          TFTRGB(0x80, 0x80, 0x00)
#define COLOR_GRAY           TFTRGB(0x80, 0x80, 0x80)
#define COLOR_SKYBLUE        TFTRGB(0x87, 0xCE, 0xEB)
#define COLOR_BLUEVIOLET     TFTRGB(0x8A, 0x2B, 0xE2)
#define COLOR_LIGHTGREEN     TFTRGB(0x90, 0xEE, 0x90)
#define COLOR_DARKVIOLET     TFTRGB(0x94, 0x00, 0xD3)
#define COLOR_YELLOWGREEN    TFTRGB(0x9A, 0xCD, 0x32)
#define COLOR_BROWN          TFTRGB(0xA5, 0x2A, 0x2A)
#define COLOR_DARKGRAY       TFTRGB(0xA9, 0xA9, 0xA9)
#define COLOR_SIENNA         TFTRGB(0xA0, 0x52, 0x2D)
#define COLOR_LIGHTBLUE      TFTRGB(0xAD, 0xD8, 0xE6)
#define COLOR_GREENYELLOW    TFTRGB(0xAD, 0xFF, 0x2F)
#define COLOR_SILVER         TFTRGB(0xC0, 0xC0, 0xC0)
#define COLOR_LIGHTGREY      TFTRGB(0xD3, 0xD3, 0xD3)
#define COLOR_LIGHTCYAN      TFTRGB(0xE0, 0xFF, 0xFF)
#define COLOR_VIOLET         TFTRGB(0xEE, 0x82, 0xEE)
#define COLOR_AZUR           TFTRGB(0xF0, 0xFF, 0xFF)
#define COLOR_BEIGE          TFTRGB(0xF5, 0xF5, 0xDC)
#define COLOR_MAGENTA        TFTRGB(0xFF, 0x00, 0xFF)
#define COLOR_TOMATO         TFTRGB(0xFF, 0x63, 0x47)
#define COLOR_GOLD           TFTRGB(0xFF, 0xD7, 0x00)
#define COLOR_ORANGE         TFTRGB(0xFF, 0xA5, 0x00)
#define COLOR_SNOW           TFTRGB(0xFF, 0xFA, 0xFA)
#define COLOR_YELLOW         TFTRGB(0xFF, 0xFF, 0x00)

#define COLOR_BROWN   TFTRGB(0xA5, 0x2A, 0x2A)	// коричневый
#define COLOR_PEAR    TFTRGB(0xD1, 0xE2, 0x31)	// грушевый

#define COLOR_KEY	TFTRGB(0xA0, 0, 0xA0)	// Цвет для прозрачных пикселей

// Цвета используемые для отображения
// различных элементов на экране.

#define LCOLOR	COLOR_GREEN		// цвет левой половины S-метра
#define RCOLOR	COLOR_RED			// цвет правой половины S-метра
#define PWRCOLOR	COLOR_RED		// цвет измерителя мощности
#define SWRCOLOR	COLOR_YELLOW		// цвет SWR-метра

#define OVFCOLOR COLOR_RED
#define LOCKCOLOR COLOR_RED
#define MODECOLOR COLOR_WHITE
#define TXRXMODECOLOR COLOR_BLACK
#define MODECOLORBG_TX COLOR_RED
#define MODECOLORBG_RX	COLOR_GREEN
#define TIMECOLOR	COLOR_GREEN
#define VOLTCOLOR	COLOR_GREEN

#define BIGCOLOR COLOR_YELLOW // GOLD
#define BIGCOLORHALF TFTRGB(192, 192, 0) //COLOR_OLIVE // DARK GOLD
//#define FRQCOLOR GRAY
#define FRQCOLOR COLOR_YELLOW // GOLD

#define MENUCOLOR COLOR_WHITE
#define MNUVALCOLOR COLOR_WHITE

uint_fast8_t display_getpagesmax(void);	// количество разных вариантов отображения (menuset)
uint_fast8_t display_getfreqformat(uint_fast8_t * prjv);	// получить параметры отображения частоты (для функции прямого ввода)

typedef struct pipparams_tag
{
	uint_fast16_t x, y, w, h;	// в пикселях

} pipparams_t;

void display2_getpipparams(pipparams_t * p);	/* получить координаты окна с панорамой и/или водопадом. */


void display2_bgprocess(void);	// выполнение шагов state machine отображения дисплея
void display2_bgreset(void);	// сброс state machine отображения дисплея

void display_dispfreq_a2(
	uint_fast32_t freq,
	uint_fast8_t blinkpos,		// позиция (степень 10) редактируесого символа
	uint_fast8_t blinkstate,	// в месте редактируемого символа отображается подчёркивание (0 - пробел)
	uint_fast8_t menuset	/* индекс режима отображения (0..3) */
	);

void display_dispfreq_ab(
	uint_fast8_t menuset	/* индекс режима отображения (0..3) */
	);
void display_volts(
	uint_fast8_t menuset,	/* индекс режима отображения (0..3) */
	uint_fast8_t extra		/* находимся в режиме отображения настроек */
	);

void display_mode_lock(
	uint_fast8_t menuset	/* индекс режима отображения (0..3) */
	);

// Статическая часть отображения режима работы
void display_mode_subset(
	uint_fast8_t menuset	/* индекс режима отображения (0..3) */
	);

// S-meter, SWR-meter, voltmeter
void display_barmeters_subset(
	uint_fast8_t menuset,	/* индекс режима отображения (0..3) */
	uint_fast8_t extra		/* находимся в режиме отображения настроек */
	);

// отображения названия параметра или группы - не в режиме редактирования
void display_menuitemlabel(
	void * pv
	);

// отображение значения параметра
void display_menuitemvalue(
	void * pv
	);


// Вызывается из display2.c
void 
display2_bars(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	);

// Вызывается из display2.c (версия для CTLSTYLE_RA4YBO_AM0)
void 
display2_bars_amv0(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	);

// Вызывается из display2.c
// значение параметра
void display_menu_valxx(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	);

// Вызывается из display2.c
// название редактируемого параметра
void display_menu_lblst(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	);

// Вызывается из display2.c
// код редактируемого параметра
void display_menu_lblc3(
	uint_fast8_t x, 
	uint_fast8_t y, 
	void * pv
	);

void display_smeter(
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t value,
	uint_fast8_t tracemax,
	uint_fast8_t level9,	// s9 level
	uint_fast8_t delta1,	// s9 - s0 delta
	uint_fast8_t delta2		// s9+50 - s9 delta
	);

void display_smeter_amv0(
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t value,
	uint_fast8_t tracemax,
	uint_fast8_t level9,	// s9 level
	uint_fast8_t delta1,	// s9 - s0 delta
	uint_fast8_t delta2		// s9+50 - s9 delta
	);

void display_swrmeter(  
	uint_fast8_t x, 
	uint_fast8_t y, 
	adcvalholder_t forward, 
	adcvalholder_t reflected, // скорректированное
	uint_fast16_t minforward
	);

// Вызывается из display2_bars_amv0 (версия для CTLSTYLE_RA4YBO_AM0)
void display_modulationmeter_amv0(  
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t value,
	uint_fast8_t maxvalue
	);

void display_pwrmeter(
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t pwr, 
	uint_fast8_t tracemax, 
	uint_fast8_t maxpwrcali		// значение для отклонения на всю шкалу
	);

void display_pwrmeter_amvo(
	uint_fast8_t x, 
	uint_fast8_t y, 
	uint_fast8_t pwr, 
	uint_fast8_t tracemax, 
	uint_fast8_t maxpwrcali		// значение для отклонения на всю шкалу
	);

#define WSIGNFLAG 0x80	// отображается плюс или минус в зависимости от знака значения
#define WMINUSFLAG 0x40	// отображается пробел или минус в зависимости от знака значения
#define WWIDTHFLAG 0x3F	// оставшиеся биты под ширину поля

COLOR_T display_getbgcolor(void);
void display_setbgcolor(COLOR_T c);

void display_string(const char * s, uint_fast8_t lowhalf);
void display_string_P(const FLASHMEM char * s, uint_fast8_t lowhalf);
void display_string2(const char * s, uint_fast8_t lowhalf);		// самый маленький шрифт
void display_string2_P(const FLASHMEM char * s, uint_fast8_t lowhalf);	// самый маленький шрифт

// Интерфейсные функции, специфические для драйвера дисплея - зависящие от типа микросхемы контроллера.
void display_hardware_initialize(void);	/* вызывается при запрещённых прерываниях. */
void display_reset(void);				/* вызывается при разрешённых прерываниях. */
void display_initialize(void);			/* вызывается при разрешённых прерываниях. */
void display_discharge(void);			/* вызывается при разрешённых прерываниях. */
void display_set_contrast(uint_fast8_t v);
uint_fast8_t display_getreadystate(void);

/* индивидуальные функции драйвера дисплея - реализованы в соответствующем из файлов */
void display_clear(void);
void display_setcolors(COLOR_T fg, COLOR_T bg);
void display_setcolors3(COLOR_T fg, COLOR_T bg, COLOR_T bgfg);	// bgfg - цвет для отрисовки антиалиасинга
void display_gotoxy(uint_fast8_t x, uint_fast8_t y);

/* работа с цветным буфером */
void display_plotfrom(uint_fast16_t x, uint_fast16_t y);	// Координаты в пикселях
void display_plotstart(uint_fast16_t height);	// Высота окна в пикселях
void display_plot(const PACKEDCOLOR_T * buffer, uint_fast16_t dx, uint_fast16_t dy);	// Размеры окна в пикселях
void display_plotstop(void);

// самый маленький шрифт
void display_wrdata2_begin(void);
void display_wrdata2_end(void);
void display_put_char_small2(uint_fast8_t c, uint_fast8_t lowhalf);
// полоса индикатора
void display_wrdatabar_begin(void);
void display_barcolumn(uint_fast8_t pattern);	// Выдать восемь цветных пикселей, младший бит - самый верхний в растре
void display_wrdatabar_end(void);
// большие и средние цифры (частота)
void display_wrdatabig_begin(void);
void display_put_char_big(uint_fast8_t c, uint_fast8_t lowhalf);
void display_put_char_half(uint_fast8_t c, uint_fast8_t lowhalf);
void display_wrdatabig_end(void);
// обычный шрифт
void display_wrdata_begin(void);
void display_put_char_small(uint_fast8_t c, uint_fast8_t lowhalf);
void display_wrdata_end(void);

/* выдать на дисплей монохромный буфер с размерами dx * dy битов */
void display_showbuffer(
	const GX_t * buffer,
	unsigned dx,	// пиксели
	unsigned dy,	// пиксели
	uint_fast8_t col,	// сетка
	uint_fast8_t row	// сетка
	);
/* выдать на дисплей монохромный буфер с размерами dx * dy битов */
void s1d13781_showbuffer(
	const GX_t * buffer,
	unsigned dx,	// пиксели
	unsigned dy,	// пиксели
	uint_fast8_t x,	// сетка
	uint_fast8_t y	// сетка
	);
/* поставить точку в буфере кадра */
void display_pixelbuffer(
	GX_t * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y	// вертикальная координата пикселя (0..dy-1) сверху вниз
	);

/* поставить точку в буфере кадра */
void display_pixelbuffer_xor(
	GX_t * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y	// вертикальная координата пикселя (0..dy-1) сверху вниз
	);

// начальная инициализация буфера
// Формат RGB565
void display_colorbuffer_fill(
	PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	COLOR565_T color
	);

// Формат RGB565
void 
dma2d_fillrect2_RGB565(
	PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	// размеры буфера
	uint_fast16_t dy,
	uint_fast16_t x,	// позиция окна в буфере
	uint_fast16_t y,
	uint_fast16_t w,	// размер окна
	uint_fast16_t h,
	COLOR565_T color
	);

// Выдать цветной буфер на дисплей
// Формат RGB565
void display_colorbuffer_show(
	const PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата левого верхнего угла на экране (0..dx-1) слева направо
	uint_fast16_t row	// вертикальная координата левого верхнего угла на экране (0..dy-1) сверху вниз
	);

// установить данный буфер как область для PIP
// Формат RGB565
void display_colorbuffer_pip(
	const PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy
	);

// Поставить цветную точку.
// Формат RGB565
void display_colorbuffer_set(
	PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLOR565_T color
	);

// Поставить цветную точку.
// Формат RGB565
void display_colorbuffer_xor(
	PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLOR565_T color
	);


/* копирование содержимого окна с перекрытием для водопада */
void
display_scroll_down(
	uint_fast16_t x0,	// левый верхний угол окна
	uint_fast16_t y0,	// левый верхний угол окна
	uint_fast16_t w, 	// до 65535 пикселей - ширина окна
	uint_fast16_t h, 	// до 65535 пикселей - высота окна
	uint_fast16_t n		// количество строк прокрутки
	);

/* копирование содержимого окна с перекрытием для водопада */
void
display_scroll_up(
	uint_fast16_t x0,	// левый верхний угол окна
	uint_fast16_t y0,	// левый верхний угол окна
	uint_fast16_t w, 	// до 65535 пикселей - ширина окна
	uint_fast16_t h, 	// до 65535 пикселей - высота окна
	uint_fast16_t n		// количество строк прокрутки
	);

void
display_panel(
	uint_fast8_t x, // левый верхний угод
	uint_fast8_t y,
	uint_fast8_t w, // ширина и высота в знакоместах
	uint_fast8_t h
	);

void
display_menu_value(
	int_fast32_t value,
	uint_fast8_t width,	// full width (if >= 128 - display with sign)
	uint_fast8_t comma,		// comma position (from right, inside width)
	uint_fast8_t rj,		// right truncated
	uint_fast8_t lowhalf
	);

// Вызовы этой функции (или группу вызовов) требуется "обрамить" парой вызовов
// display_wrdatabar_begin() и display_wrdatabar_end().
void display_dispbar(
	uint_fast8_t width,	/* количество знакомест, занимаемых индикатором */
	uint_fast8_t value,		/* значение, которое надо отобразить */
	uint_fast8_t tracevalue,		/* значение маркера, которое надо отобразить */
	uint_fast8_t topvalue,	/* значение, соответствующее полностью заполненному индикатору */
	uint_fast8_t pattern,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL */
	uint_fast8_t patternmax,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL - для отображения запомненного значения */
	uint_fast8_t emptyp			/* паттерн для заполнения между штрихами */
	);

void display_at(uint_fast8_t x, uint_fast8_t y, const char * s);		// Выдача строки из ОЗУ в указанное место экрана.
void display_at_P(uint_fast8_t x, uint_fast8_t y, const FLASHMEM char * s); // Выдача строки из ПЗУ в указанное место экрана.


#define BGCOLOR (display_getbgcolor())

uint_fast8_t hamradio_get_tx(void);
int_fast32_t hamradio_get_pbtvalue(void);	// Для отображения на дисплее
uint_fast8_t hamradio_get_atuvalue(void);
uint_fast8_t hamradio_get_genham_value(void);
uint_fast8_t hamradio_get_bypvalue(void);
uint_fast8_t hamradio_get_lockvalue(void);	// текущее состояние LOCK
uint_fast8_t hamradio_get_usefastvalue(void);	// текущее состояние FAST
uint_fast8_t hamradio_get_voxvalue(void);	// текущее состояние VOX
uint_fast8_t hamradio_get_tunemodevalue(void);	// текущее состояние TUNE
uint_fast32_t hamradio_get_freq_a(void);		// Частота VFO A для отображения на дисплее
uint_fast32_t hamradio_get_freq_b(void);		// Частота VFO B для отображения на дисплее
uint_fast32_t hamradio_get_freq_rx(void);		// Частота VFO A для маркировки файлов
uint_fast32_t hamradio_get_modem_baudrate100(void);	// скорость передачи BPSK * 100
uint_fast8_t hamradio_get_notchvalue(int_fast32_t * p);		// Notch filter ON/OFF
const FLASHMEM char * hamradio_get_mode_a_value_P(void);	// SSB/CW/AM/FM/..
const FLASHMEM char * hamradio_get_mode_b_value_P(void);	// SSB/CW/AM/FM/..
const FLASHMEM char * hamradio_get_rxbw_value_P(void);	// RX bandwidth
const FLASHMEM char * hamradio_get_pre_value_P(void);	// RX preamplifier
const FLASHMEM char * hamradio_get_att_value_P(void);	// RX attenuator
const FLASHMEM char * hamradio_get_agc3_value_P(void);	// RX agc time - 3-х буквенные абревиатуры
const FLASHMEM char * hamradio_get_agc4_value_P(void);	// RX agc time - 4-х буквенные абревиатуры
const FLASHMEM char * hamradio_get_ant5_value_P(void);	// antenna
const FLASHMEM char * hamradio_get_mainsubrxmode3_value_P(void);	// текущее состояние DUAL WATCH
const char * hamradio_get_vfomode3_value(void);	// VFO mode
const char * hamradio_get_vfomode5_value(void);	// VFO mode
uint_fast8_t hamradio_get_volt_value(void);	// Вольты в десятых долях
int_fast16_t hamradio_get_pacurrent_value(void);	// Ток в десятках милиампер, может быть отрицательным
const FLASHMEM char * hamradio_get_hplp_value_P(void);	// HP/LP
uint_fast8_t hamradio_get_rec_value(void);	// AUDIO recording state
uint_fast8_t hamradio_get_amfm_highcut100_value(void);	// текущее значение верхней частоты среза НЧ фильтра АМ/ЧМ (в сотнях герц)
uint_fast8_t hamradio_get_samdelta10(int_fast32_t * p, uint_fast8_t pathi);		/* Получить значение отклонения частоты с точностью 0.1 герца */
uint_fast8_t hamradio_get_usbh_active(void);
uint_fast8_t hamradio_get_datamode(void);	// источник звука для передачи - USB AUDIO

#define SWRMIN 10	// минимум - соответствует SWR = 1.0, точность = 0.1

#if LCDMODE_LTDC

	#if LCDMODE_ILI8961 || LCDMODE_LQ043T3DX02K
		#define DIM_FIRST DIM_Y
		#define DIM_SECOND DIM_X
	#else
		#define DIM_FIRST DIM_X
		#define DIM_SECOND DIM_Y
	#endif

	typedef PACKEDCOLOR_T FRAMEBUFF_T [DIM_FIRST][DIM_SECOND];

	#if CPUSTYLE_R7S721
		#define SDRAM_BANK_ADDR     ((uintptr_t) 0x20200000)
	#elif STM32H743xx
		//#define SDRAM_BANK_ADDR     ((uintptr_t) D1_AXISRAM_BASE)
	#elif CPUSTYLE_STM32F4XX
		//#define SDRAM_BANK_ADDR     ((uintptr_t) 0xD0000000)
	#endif

	#if defined (SDRAM_BANK_ADDR) && LCDMODE_LTDCSDRAMBUFF && LCDMODE_LTDC
		#define framebuff (* (FRAMEBUFF_T *) SDRAM_BANK_ADDR)
	#else /* defined (SDRAM_BANK_ADDR) && LCDMODE_LTDCSDRAMBUFF && LCDMODE_LTDC */
		#define framebuff (framebuff0)
		extern FRAMEBUFF_T framebuff0;	//L8 (8-bit Luminance or CLUT)
	#endif /* defined (SDRAM_BANK_ADDR) && LCDMODE_LTDCSDRAMBUFF && LCDMODE_LTDC */


#endif /* LCDMODE_LTDC */


#endif /* DISPLAY_H_INCLUDED */
