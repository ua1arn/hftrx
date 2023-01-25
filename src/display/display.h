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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef uint_fast16_t COLOR565_T;
typedef uint16_t PACKEDCOLOR565_T;
// RRRRRRR.GGGGGGGG.BBBBBBBB
typedef uint_fast32_t COLOR24_T;
#define COLOR24(red, green, blue) \
	(  (uint_fast32_t) \
		(	\
			(((uint_fast32_t) (red) << 16) & 0xFF0000u)  | \
			(((uint_fast32_t) (green) << 8) & 0xFF00u) | \
			(((uint_fast32_t) (blue) << 0) & 0xFFu) \
		) \
	)

// Get color componens from COLOR24_T value
#define COLOR24_R(v) (((v) >> 16) & 0xFF)
#define COLOR24_G(v) (((v) >> 8) & 0xFF)
#define COLOR24_B(v) (((v) >> 0) & 0xFF)

#define COLOR24_KEY	COLOR24(0xA0, 0, 0xA0)	// Цвет для прозрачных пикселей


enum gradient_style
{
	GRADIENT_BLUE_YELLOW_RED,		// blue -> yellow -> red
	GRADIENT_BLACK_YELLOW_RED,		// black -> yellow -> red
	GRADIENT_BLACK_YELLOW_GREEN,	// black -> yellow -> green
	GRADIENT_BLACK_RED,				// black -> red
	GRADIENT_BLACK_GREEN,			// black -> green
	GRADIENT_BLACK_BLUE,			// black -> blue
	GRADIENT_BLACK_WHITE			// black -> white
};

COLOR24_T colorgradient(unsigned pos, unsigned maxpos);


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

#elif LCDMODE_S1D13781 || LCDMODE_LQ043T3DX02K || LCDMODE_AT070TN90 || LCDMODE_AT070TNA2 || LCDMODE_TCG104XGLPAPNN || LCDMODE_H497TLB01P4

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

#elif LCDMODE_DUMMY
	/* При использовании frame buffer цвета восьмибитные */
	typedef uint_fast8_t COLORMAIN_T;
	typedef uint8_t PACKEDCOLORMAIN_T;
	typedef uint_fast8_t COLORPIP_T;
	typedef uint8_t PACKEDCOLORPIP_T;

	// для формирования растра с изображением водопада и спектра
	// RRRR.RGGG.GGGB.BBBB
	#define TFTRGB565(red, green, blue) 0

	#define TFTRGB(red, green, blue) 0

#else
	#error Undefined display type
#endif


#if LCDMODE_LTDC

	#define DISPLAYMODES_FPS 5	/* количество обновлений отображения режимов работы */
	#define DISPLAY_FPS	10	/* обновление показаний частоты десять раз в секунду */
	#define DISPLAYSWR_FPS 10	/* количество обновлений SWR и панорамы за секунду */

	#define LCDMODE_COLORED	1

	#if LCDMODE_LTDC_L24

		typedef uint_fast32_t COLORMAIN_T;
		struct rgb_24b
		{
			uint8_t r, g, b;
		} ATTRPACKED;// аттрибут GCC, исключает "дыры" в структуре.

		typedef struct rgb_24b PACKEDCOLORMAIN_T;	// 3 байта для последовательной выдачи как RGB для LCDMODE_ILI8961

		// RRRRRRR.GGGGGGGG.BBBBBBBB
		#define TFTRGB(red, green, blue) \
			(  (uint32_t) \
				(	\
					(((uint_fast32_t) (red) << 16) & 0xFF0000)  | \
					(((uint_fast32_t) (green) << 8) & 0xFF00) | \
					(((uint_fast32_t) (blue) << 0) & 0xFF) \
				) \
			)

		// Get color componens from framebuffer value
		#define COLORMAIN_R(v) (((v) & 0xFF0000) >> 16)
		#define COLORMAIN_G(v) (((v) & 0xFF00) >> 8)
		#define COLORMAIN_B(v) (((v) & 0xFF) >> 0)

	#elif LCDMODE_MAIN_L8

		/* При использовании frame buffer цвета восьмибитные */
		typedef uint_fast8_t COLORMAIN_T;
		typedef uint8_t PACKEDCOLORMAIN_T;

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
					(((uint_fast16_t) (red) << 8) &   0xF800)  | \
					(((uint_fast16_t) (green) << 3) & 0x07E0) | \
					(((uint_fast16_t) (blue) >> 3) &  0x001F) \
				) \
			)

	#elif LCDMODE_MAIN_ARGB888 && (CPUSTYLE_XC7Z || CPUSTYLE_XCZU) && ! WITHTFT_OVER_LVDS

		// RBG named order
		typedef uint_fast32_t COLORMAIN_T;
		typedef uint32_t PACKEDCOLORMAIN_T;

		#define TFTRGB(red, green, blue) \
			(  (uint_fast32_t) \
				(	\
					(((uint_fast32_t) ((red) & 0xFF)) << 16)  | \
					(((uint_fast32_t) ((green) & 0xFF)) << 8)  | \
					(((uint_fast32_t) ((blue) & 0xFF)) << 0)  | \
					0 \
				) \
			)

		// для формирования растра с изображением водопада и спектра
		#define TFTRGB565 TFTRGB

		// Get color componens from framebuffer value
		#define COLORMAIN_R(v) (((v) & 0xFF0000) >> 16)
		#define COLORMAIN_G(v) (((v) & 0xFF00) >> 8)
		#define COLORMAIN_B(v) (((v) & 0xFF) >> 0)


	#elif LCDMODE_MAIN_ARGB888

		//#define LCDMODE_RGB565 1
		typedef uint_fast32_t COLORMAIN_T;
		typedef uint32_t PACKEDCOLORMAIN_T;

		// RRRR.RGGG.GGGB.BBBB
	#define TFTRGB(red, green, blue) \
		(  (uint_fast32_t) \
			(	\
				((uint_fast32_t) (255) << 24)  | /* Alpha value - full color */ \
				(((uint_fast32_t) (red) << 16) &   0xFF0000)  | \
				(((uint_fast32_t) (green) << 8) & 0xFF00) | \
				(((uint_fast32_t) (blue) >> 0) &  0x00FF) \
			) \
		)
	#define TFTALPHA(alpha, color24) \
		(  (uint_fast32_t) \
			(	\
				((uint_fast32_t) (alpha) << 24)  | /* Alpha value */ \
					(((uint_fast32_t) (color24)) &  0x00FFFFFF) \
			) \
		)

		// для формирования растра с изображением водопада и спектра
		#define TFTRGB565 TFTRGB

		// Get color componens from framebuffer value
		#define COLORMAIN_R(v) (((v) & 0xFF0000) >> 16)
		#define COLORMAIN_G(v) (((v) & 0xFF00) >> 8)
		#define COLORMAIN_B(v) (((v) & 0xFF) >> 0)

	#else /* LCDMODE_MAIN_L8 */

		//#define LCDMODE_RGB565 1
		typedef uint_fast16_t COLORMAIN_T;
		typedef uint16_t PACKEDCOLORMAIN_T;

		// RRRR.RGGG.GGGB.BBBB
		#define TFTRGB(red, green, blue) ( \
				(uint_fast16_t) (	\
					(((uint_fast16_t) (red) << 8) &   0xF800)  | \
					(((uint_fast16_t) (green) << 3) & 0x07E0) | \
					(((uint_fast16_t) (blue) >> 3) &  0x001F) \
				) \
			)

		// для формирования растра с изображением водопада и спектра
		#define TFTRGB565 TFTRGB

		// Get color componens from framebuffer value
		#define COLORMAIN_R(v) ((((v) & 0xF800) >> 8) | (((v) & 0xE000) >> 13))
		#define COLORMAIN_G(v) ((((v) & 0x07E0) >> 3) | (((v) & 0x0600) >> 9))
		#define COLORMAIN_B(v) ((((v) & 0x001F) << 3) | (((v) & 0x001C) >> 2))

	#endif /* LCDMODE_MAIN_L8 */

	#if LCDMODE_PIP_L8
		typedef uint8_t PACKEDCOLORPIP_T;
		typedef uint_fast8_t COLORPIP_T;

	#elif LCDMODE_PIP_RGB565
		typedef uint16_t PACKEDCOLORPIP_T;
		typedef uint_fast16_t COLORPIP_T;

	#else /* LCDMODE_PIP_RGB565 */
		/* если только MAIN - тип PIP соответствует */
		typedef PACKEDCOLORMAIN_T PACKEDCOLORPIP_T;
		typedef COLORMAIN_T COLORPIP_T;

	#endif /* LCDMODE_PIP_L8 */

#endif /* LCDMODE_LTDC */

#ifndef DCACHEROWSIZE
	#define GXALIGN 1	/* количество пикселей в строке видеобуфера кратно этому заначению */
#elif (__CORTEX_A != 0) ||  (__CORTEX_M != 0)
	#define GXALIGN (DCACHEROWSIZE / LCDMODE_PIXELSIZE)	/* количество пикселей в строке видеобуфера кратно этому заначению */
#else
	#define GXALIGN 1	/* количество пикселей в строке видеобуфера кратно этому заначению */
#endif

#define GXADJ(dx) (((dx) + (GXALIGN - 1)) / GXALIGN * GXALIGN)
#define MGADJ(dx) (((dx) + (MGALIGN - 1)) / MGALIGN * MGALIGN)

#if LCDMODE_S1D13781
	// биты слова буфера располагаются на экране горизонтально
	// старший бит левее
	#define MGALIGN 16
	typedef uint16_t GX_t;	/* тип элемента буфера для выдачи монохромного растра */
#elif LCDMODE_COLORED
	// биты слова буфера располагаются на экране вертикально
	#define MGALIGN 8
	typedef uint8_t GX_t;	/* тип элемента буфера для выдачи монохромного растра */
#else	/* LCDMODE_S1D13781 */
	// биты слова буфера располагаются на экране вертикально
	#define MGALIGN 8
	typedef uint8_t GX_t;	/* тип элемента буфера для выдачи монохромного растра */
#endif	/* */

#define MGSIZE(dx, dy)	((unsigned long) MGADJ(dx) * (dy))	// размер буфера для монохромного растра
#define GXSIZE(dx, dy)	((unsigned long) GXADJ(dx) * (dy))	// размер буфера для цветного растра

// Хранение описания буфера для функций построения изображений
typedef struct gtg_tag
{
	PACKEDCOLORMAIN_T * buffer;	// Буфер в памяти
	uint16_t dx;	// ширина буфера
	uint16_t dy;	// высота буфера
} GTG_t;

COLORMAIN_T display_getbgcolor(void);
void display_setbgcolor(COLORMAIN_T c);

// Интерфейсные функции, специфические для драйвера дисплея - зависящие от типа микросхемы контроллера.
void display_hardware_initialize(void);	/* вызывается при запрещённых прерываниях. */
void display_hdmi_initialize(void);
void display_reset(void);				/* вызывается при разрешённых прерываниях. */
void display_initialize(void);			/* вызывается при разрешённых прерываниях. */
void display_uninitialize(void);			/* вызывается при разрешённых прерываниях. */
void display_nextfb(void);				/* переключаем на следующий фреймбуфер */
void display_set_contrast(uint_fast8_t v);
void display_palette(void);				// Palette reload

void tc358768_initialize(const videomode_t * vdmode);
void tc358768_wakeup(const videomode_t * vdmode);
void tc358768_deinitialize(void);

// siiI9022A Lattice Semiconductor Corp HDMI Transmitter
void sii9022x_initialize(const videomode_t * vdmode);
void sii9022x_wakeup(const videomode_t * vdmode);
void sii9022x_deinitialize(void);

void panel_initialize(const videomode_t * vdmode);
void panel_wakeup(void);
void panel_deinitialize(void);

/* индивидуальные функции драйвера дисплея - реализованы в соответствующем из файлов */
void display_clear(void);
void display_flush(void);	// для framebufer дисплеев - вытолкнуть кэш память
void colmain_setcolors(COLORMAIN_T fg, COLORMAIN_T bg);
void colmain_setcolors3(COLORMAIN_T fg, COLORMAIN_T bg, COLORMAIN_T bgfg);	// bgfg - цвет для отрисовки антиалиасинга

/* работа с цветным буфером */
void display_plotfrom(uint_fast16_t x, uint_fast16_t y);	// Координаты в пикселях
void display_plotstart(uint_fast16_t dy);	// Высота окна источника в пикселях
void display_plot(const PACKEDCOLORMAIN_T * buffer, uint_fast16_t dx, uint_fast16_t dy, uint_fast16_t xpix, uint_fast16_t ypix);	// Размеры окна в пикселях и начальная точка рисования
void display_plotstop(void);

// самый маленький шрифт
uint_fast16_t display_wrdata2_begin(uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp);
void display_wrdata2_end(void);
uint_fast16_t display_put_char_small2(uint_fast16_t xpix, uint_fast16_t ypix, uint_fast8_t c, uint_fast8_t lowhalf);
// полоса индикатора
uint_fast16_t display_wrdatabar_begin(uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp);
uint_fast16_t display_barcolumn(uint_fast16_t xpix, uint_fast16_t ypix, uint_fast8_t pattern);	// Выдать восемь цветных пикселей, младший бит - самый верхний в растре
void display_wrdatabar_end(void);
// большие и средние цифры (частота)
uint_fast16_t display_wrdatabig_begin(uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp);
uint_fast16_t display_put_char_big(uint_fast16_t xpix, uint_fast16_t ypix, uint_fast8_t c, uint_fast8_t lowhalf);
uint_fast16_t display_put_char_half(uint_fast16_t xpix, uint_fast16_t ypix, uint_fast8_t c, uint_fast8_t lowhalf);
void display_wrdatabig_end(void);
// обычный шрифт
uint_fast16_t display_wrdata_begin(uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp);
uint_fast16_t display_put_char_small(uint_fast16_t xpix, uint_fast16_t ypix, uint_fast8_t c, uint_fast8_t lowhalf);
uint_fast16_t display_put_char_small_xy(uint_fast16_t x, uint_fast16_t y, uint_fast8_t c, COLOR565_T fg);
void display_wrdata_end(void);

typedef struct pipparams_tag
{
	uint16_t x, y, w, h;	// в пикселях
	//uintptr_t frame;	// default framebufer

} pipparams_t;

void display2_getpipparams(pipparams_t * p);	/* получить координаты окна с панорамой и/или водопадом. */

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
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y	// вертикальная координата пикселя (0..dy-1) сверху вниз
	);

/* поставить точку в буфере кадра */
void display_pixelbuffer_xor(
	GX_t * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y	// вертикальная координата пикселя (0..dy-1) сверху вниз
	);
void display_pixelbuffer_line(
	GX_t * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x0,	
	uint_fast16_t y0,
	uint_fast16_t x1,	
	uint_fast16_t y1
	);
void display_pixelbuffer_clear(
	GX_t * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy
	);

// Заполнение буфера сполшным цветом
// Формат RGB565
// Эта функция используется только в тесте
void colpip_fill(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	COLORPIP_T color
	);

// Заполнение буфера сполшным цветом
void gtg_fill(
		const GTG_t * gtg,
		COLORPIP_T color
		);

// поставить цветную точку.
void gtg_point(
	const GTG_t * gtg,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	);

// Выдать цветной буфер на дисплей
// В случае фреймбуфеных дисплеев - формат цвета и там и там одинаковый
void colpip_to_main(
	uintptr_t srcinvalidateaddr,	// параметры clean источника
	int_fast32_t srcinvalidatesize,
	const PACKEDCOLORPIP_T * buffer,	// источник
	uint_fast16_t dx,	// ширина буфера источника
	uint_fast16_t dy,	// высота буфера источника
	uint_fast16_t col,	// целевая горизонтальная координата левого верхнего угла на экране (0..dx-1) слева направо
	uint_fast16_t row	// целевая вертикальная координата левого верхнего угла на экране (0..dy-1) сверху вниз
	);

// Нарисовать линию указанным цветом
void colpip_line(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x0,	
	uint_fast16_t y0,
	uint_fast16_t x1,	
	uint_fast16_t y1,
	COLORPIP_T color
	);

// Нарисовать закрашенный или пустой прямоугольник
void colpip_rect(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// размер буфера
	uint_fast16_t dy,	// размер буфера
	uint_fast16_t x1,	// начальная координата
	uint_fast16_t y1,	// начальная координата
	uint_fast16_t x2,	// конечная координата (включена в заполняемую облсть)
	uint_fast16_t y2,	// конечная координата (включена в заполняемую облсть)
	COLORPIP_T color,
	uint_fast8_t fill
	);

// Поставить цветную точку.
void colpip_point(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	);

// поставить цветную точку (модификация с сохранением старого изоьражения).
void colpip_point_xor(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	);

/* возвращает новую позицию по x */
uint_fast16_t
colpip_string(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// размеры буфера
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s
	);

/* возвращает новую позицию по x */
uint_fast16_t
colmain_string(
	PACKEDCOLORMAIN_T * buffer,
	uint_fast16_t dx,	// размеры буфера
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s
	);
// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string_tbg(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	);
// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string_x2_tbg(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	);
// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string_x2ra90_tbg(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg,		// цвет вывода текста
	COLORPIP_T bg		// цвет фона
	);
// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string_count(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T fg,		// цвет вывода текста
	const char * s,		// строка для вывода
	size_t len			// количество символов
	);
// Используется при выводе на графический индикатор,
void
colpip_string_x2_count(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T fg,		// цвет вывода текста
	const char * s,		// строка для вывода
	size_t len			// количество символов
	);
// Используется при выводе на графический индикатор,
void
colpip_string_x2ra90_count(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T fg,		// цвет вывода текста
	COLORPIP_T bg,		// цвет вывода текста
	const char * s,		// строка для вывода
	size_t len			// количество символов
	);
// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string2_tbg(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	);
// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void colpip_string3_tbg(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	);
// Используется при выводе на графический индикатор,
// Возвращает ширину строки в пикселях
uint_fast16_t strwidth3(
	const char * s
	);
// Возвращает ширину строки в пикселях
uint_fast16_t strwidth2(
	const char * s
	);
// Возвращает ширину строки в пикселях
uint_fast16_t strwidth(
	const char * s
	);
// Возвращает высоту строки в пикселях
uint_fast16_t strheight(
	const char * s
	);

/* копирование содержимого окна с перекрытием для водопада */
void
display_scroll_down(
	uint_fast16_t x0,	// левый верхний угол окна
	uint_fast16_t y0,	// левый верхний угол окна
	uint_fast16_t w, 	// до 65535 пикселей - ширина окна
	uint_fast16_t h, 	// до 65535 пикселей - высота окна
	uint_fast16_t n,	// количество строк прокрутки
	int_fast16_t hshift	// количество пиксеелей для сдвига влево (отрицательное число) или вправо (положительное).
	);

/* копирование содержимого окна с перекрытием для водопада */
void
display_scroll_up(
	uint_fast16_t x0,	// левый верхний угол окна
	uint_fast16_t y0,	// левый верхний угол окна
	uint_fast16_t w, 	// до 65535 пикселей - ширина окна
	uint_fast16_t h, 	// до 65535 пикселей - высота окна
	uint_fast16_t n,	// количество строк прокрутки
	int_fast16_t hshift	// количество пиксеелей для сдвига влево (отрицательное число) или вправо (положительное).
	);

void
display_panel(
	uint_fast8_t x, // левый верхний угод
	uint_fast8_t y,
	uint_fast8_t w, // ширина и высота в знакоместах
	uint_fast8_t h
	);

void
display2_menu_value(
	uint_fast8_t x,
	uint_fast8_t y,
	int_fast32_t value,
	uint_fast8_t width,	// full width (if >= 128 - display with sign)
	uint_fast8_t comma,		// comma position (from right, inside width)
	uint_fast8_t rj,		// right truncated
	uint_fast8_t lowhalf
	);

// Вызовы этой функции (или группу вызовов) требуется "обрамить" парой вызовов
// display_wrdatabar_begin() и display_wrdatabar_end().
void display_bar(
	uint_fast16_t xpix,
	uint_fast16_t ypix,
	uint_fast8_t width,	/* количество знакомест, занимаемых индикатором */
	uint_fast8_t value,		/* значение, которое надо отобразить */
	uint_fast8_t tracevalue,		/* значение маркера, которое надо отобразить */
	uint_fast8_t topvalue,	/* значение, соответствующее полностью заполненному индикатору */
	uint_fast8_t pattern,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL */
	uint_fast8_t patternmax,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL - для отображения запомненного значения */
	uint_fast8_t emptyp			/* паттерн для заполнения между штрихами */
	);

void display_at(uint_fast8_t x, uint_fast8_t y, const char * s);		// Выдача строки из ОЗУ в указанное место экрана.
void display_x2_at(uint_fast8_t x, uint_fast8_t y, const char * s);		// Выдача строки из ОЗУ в указанное место экрана.
void display_at_P(uint_fast8_t x, uint_fast8_t y, const FLASHMEM char * s); // Выдача строки из ПЗУ в указанное место экрана.
/* заполнение прямоугольника на основном экране произвольным цветом
*/
void
display_fillrect(
	uint_fast16_t x, uint_fast16_t y, 	// координаты в пикселях
	uint_fast16_t w, uint_fast16_t h, 	// размеры в пикселях
	COLORMAIN_T color
	);
/* рисование линии на основном экране произвольным цветом
*/
void
display_line(
	int x1, int y1,
	int x2, int y2,
	COLORMAIN_T color
	);

/* заполнение прямоугольника в буфере произвольным цветом
*/
void
colmain_fillrect(
	PACKEDCOLORMAIN_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x, uint_fast16_t y, 	// координаты в пикселях
	uint_fast16_t w, uint_fast16_t h, 	// размеры в пикселях
	COLORMAIN_T color
	);

// скоприовать прямоугольник с типом пикселей соответствующим pip
void colpip_plot(
	uintptr_t dstinvalidateaddr,	// параметры clean invalidate получателя
	int_fast32_t dstinvalidatesize,
	PACKEDCOLORPIP_T * tbuffer,	// получатель
	uint_fast16_t tdx,	// получатель
	uint_fast16_t tdy,	// получатель
	uint_fast16_t x,	// получатель
	uint_fast16_t y,	// получатель
	uintptr_t srcinvalidateaddr,	// параметры clean источника
	int_fast32_t srcinvalidatesize,
	const PACKEDCOLORPIP_T * buffer, 	// источник
	uint_fast16_t dx,	// источник Размеры окна в пикселях
	uint_fast16_t dy	// источник
	);
// скоприовать прямоугольник с типом пикселей соответствующим pip
// с поворотом вправо на 90 градусов
void colpip_plot_ra90(
	uintptr_t dstinvalidateaddr,	// параметры clean invalidate получателя
	int_fast32_t dstinvalidatesize,
	PACKEDCOLORPIP_T * dst,	// получатель
	uint_fast16_t tdx,	// получатель Размеры окна в пикселях
	uint_fast16_t tdy,	// получатель
	uint_fast16_t x,	// получатель Позиция
	uint_fast16_t y,	// получатель
	uintptr_t srcinvalidateaddr,	// параметры clean источника
	int_fast32_t srcinvalidatesize,
	const PACKEDCOLORPIP_T * src, 	// источник
	uint_fast16_t sdx,	// источник Размеры окна в пикселях
	uint_fast16_t sdy	// источник
	);

void
colmain_string3_at_xy(
	PACKEDCOLORMAIN_T * buffer,
	const uint_fast16_t dx,
	const uint_fast16_t dy,
	uint_fast16_t x,
	uint_fast16_t y,
	const char * s
	);


/* Нарисовать прямоугольник со скругленными углами */
void
colmain_rounded_rect(
		PACKEDCOLORMAIN_T * buffer,
		uint_fast16_t bx,	// ширина буфера
		uint_fast16_t by,	// высота буфера
		uint_fast16_t x1,
		uint_fast16_t y1,
		uint_fast16_t x2,
		uint_fast16_t y2,
		uint_fast8_t r,		// радиус закругления углов
		COLORMAIN_T color,
		uint_fast8_t fill
		);

void
polar_to_dek(
		uint_fast16_t xc,
		uint_fast16_t yc,
		uint_fast16_t gs,
		uint_fast16_t r,
		uint_fast16_t * x,
		uint_fast16_t * y,
		uint_fast8_t style);

void
display_radius_buf(
		PACKEDCOLORMAIN_T * buffer,
		uint_fast16_t bx,	// ширина буфера
		uint_fast16_t by,	// высота буфера
		int xc, int yc,
		unsigned gs,
		unsigned r1, unsigned r2,
		COLORMAIN_T color,
		int antialiasing,
		int style);			// 1 - растягивание по горизонтали

void
display_segm_buf(
		PACKEDCOLORMAIN_T * buffer,
		uint_fast16_t bx,	// ширина буфера
		uint_fast16_t by,	// высота буфера
		int xc, int yc,
		unsigned gs, unsigned ge,
		unsigned r, int step,
		COLORMAIN_T color,
		int antialiasing,
		int style);			// 1 - растягивание по горизонтали;

// Нарисовать вертикальную цветную полосу
void
display_colorbuf_xor_vline(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row0,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	COLORPIP_T color
	);

// Нарисовать вертикальную цветную полосу
// Формат RGB565
void
display_colorbuf_set_vline(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t col,	// горизонтальная координата начального пикселя (0..dx-1) слева направо
	uint_fast16_t row0,	// вертикальная координата начального пикселя (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	COLORPIP_T color
	);

// Нарисовать горизонтальную цветную полосу
// Формат RGB565
void
display_colorbuf_set_hline(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t col,	// горизонтальная координата начального пикселя (0..dx-1) слева направо
	uint_fast16_t row0,	// вертикальная координата начального пикселя (0..dy-1) сверху вниз
	uint_fast16_t w,	// ширина
	COLORPIP_T color
	);

// получить адрес требуемой позиции в буфере
PACKEDCOLORMAIN_T *
colmain_mem_at_debug(
	PACKEDCOLORMAIN_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * file,
	int line
	);

#define colmain_mem_at(a,b,c,d,e) (colmain_mem_at_debug((a), (b), (c), (d), (e), __FILE__, __LINE__))

void display_putpixel(
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORMAIN_T color
	);

void colmain_putpixel(
	PACKEDCOLORMAIN_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORMAIN_T color
	);

void display_at_xy(uint_fast16_t x, uint_fast16_t y, const char * s);	/* вывод строки */
void display_snapshot(PACKEDCOLORMAIN_T * buffer, uint_fast16_t dx, uint_fast16_t dy);	/* запись видимого изображения */
void display_snapshot_write(PACKEDCOLORMAIN_T * buffer, uint_fast16_t dx, uint_fast16_t dy);	/* запись видимого изображения в файл */
void display_snapshot_req(void);

void board_set_topdb(int_fast16_t v);			/* верхний предел FFT */
void board_set_bottomdb(int_fast16_t v);		/* нижний предел FFT */
void board_set_topdbwf(int_fast16_t v);			/* верхний предел FFT */
void board_set_bottomdbwf(int_fast16_t v);		/* нижний предел FFT */
void board_set_zoomxpow2(uint_fast8_t v);		/* уменьшение отображаемого участка спектра */
void board_set_wflevelsep(uint_fast8_t v); 		/* чувствительность водопада регулируется отдельной парой параметров */
void board_set_view_style(uint_fast8_t v);		/* стиль отображения спектра и панорамы */
void board_set_view3dss_mark(uint_fast8_t v);	/* Для VIEW_3DSS - индикация полосы пропускания на спектре */
void board_set_showdbm(uint_fast8_t v);			// Отображение уровня сигнала в dBm или S-memter (в зависимости от настроек)
void board_set_afspeclow(int_fast16_t v);		// нижняя частота отображения спектроанализатора
void board_set_afspechigh(int_fast16_t v);		// верхняя частота отображения спектроанализатора
void board_set_lvlgridstep(uint_fast8_t v);		/* Шаг сетки уровней в децибелах */

PACKEDCOLORMAIN_T * colmain_fb_draw(void);		// буфер для построения изображения
uint_fast8_t colmain_fb_next(void);				// переключиться на использование для DRAW следующего фреймбуфера (его номер возвращается)
void colmain_fb_initialize(void);
uint_fast8_t colmain_getindexbyaddr(uintptr_t addr);

#if WITHALPHA
#define DEFAULT_ALPHA WITHALPHA
#else
#define DEFAULT_ALPHA 128	// 0..255
#endif

void display2_xltrgb24(COLOR24_T * xtable);

void hwaccel_copy(
	uintptr_t dstinvalidateaddr,	// параметры clean invalidate получателя
	int_fast32_t dstinvalidatesize,
	PACKEDCOLORMAIN_T * dst,
	uint_fast16_t ddx,	// ширина буфера
	uint_fast16_t ddy,	// высота буфера
	uintptr_t srcinvalidateaddr,	// параметры clean источника
	int_fast32_t srcinvalidatesize,
	const PACKEDCOLORMAIN_T * src,
	uint_fast16_t sdx,	// ширина буфера
	uint_fast16_t sdy	// высота буфера
	);

// копирование буфера с поворотом вправо на 90 градусов (четверть оборота).
void hwaccel_ra90(
	PACKEDCOLORPIP_T * __restrict tbuffer,
	uint_fast16_t tdx,	// размер получателя
	uint_fast16_t tdy,
	uint_fast16_t tx,	// горизонтальная координата пикселя (0..dx-1) слева направо - в исходном нижний
	uint_fast16_t ty,	// вертикальная координата пикселя (0..dy-1) сверху вниз - в исходном левый
	const PACKEDCOLORPIP_T * __restrict sbuffer,
	uint_fast16_t sdx,	// размер источника
	uint_fast16_t sdy
	);

// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
void ltdc_horizontal_pixels(
	PACKEDCOLORMAIN_T * tgr,		// target raster
	const FLASHMEM uint8_t * raster,
	uint_fast16_t width	// number of bits (start from LSB first byte in raster)
	);

// Для произвольного шрифта
uint_fast16_t
RAMFUNC_NONILINE ltdc_horizontal_put_char_unified(
	const FLASHMEM uint8_t * fontraster,
	uint_fast8_t width,		// пикселей в символе по горизонтали знакогнератора
	uint_fast8_t width2,	// пикселей в символе по горизонтали отображается (для уменьшеных в ширину символов большиз шрифтов)
	uint_fast8_t height,	// строк в символе по вертикали
	uint_fast8_t bytesw,	// байтов в одной строке символа
	PACKEDCOLORMAIN_T * const __restrict buffer,
	const uint_fast16_t dx,
	const uint_fast16_t dy,
	uint_fast16_t x, uint_fast16_t y,
	uint_fast8_t cc
	);

// Установить прозрачность для прямоугольника
void display_transparency(
	uint_fast16_t x1, uint_fast16_t y1,
	uint_fast16_t x2, uint_fast16_t y2,
	uint_fast8_t alpha	// на сколько затемнять цвета (0 - чёрный, 255 - без изменений)
	);

// заполнение прямоугольной области в видеобуфере
void colpip_fillrect(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x,	// начальная координата
	uint_fast16_t y,	// начальная координата
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	COLORPIP_T color	// цвет
	);

uint_fast8_t smallfont_decode(uint_fast8_t c);
uint_fast8_t bigfont_decode(uint_fast8_t c);

int_fast32_t display_zoomedbw(void);

void display_string3_at_xy(uint_fast16_t x, uint_fast16_t y, const char * s, COLORMAIN_T fg, COLORMAIN_T bg);

void colmain_line(
		PACKEDCOLORMAIN_T * buffer,
		uint_fast16_t bx,	// ширина буфера
		uint_fast16_t by,	// высота буфера
		int xn, int yn,
		int xk, int yk,
		COLORMAIN_T color,
		int antialiasing);

// Отображение цифр в поле "больших цифр" - индикатор основной частоты настройки аппарата.
void
display_value_big(
	uint_fast8_t xcell,	// x координата начала вывода значения
	uint_fast8_t ycell,	// y координата начала вывода значения
	uint_fast32_t freq,
	uint_fast8_t width, // = 8;	// full width
	uint_fast8_t comma, // = 2;	// comma position (from right, inside width)
	uint_fast8_t comma2,	// = comma + 3;		// comma position (from right, inside width)
	uint_fast8_t rj,	// = 1;		// right truncated
	uint_fast8_t blinkpos,		// позиция, где символ заменён пробелом
	uint_fast8_t blinkstate,	// 0 - пробел, 1 - курсор
	uint_fast8_t withhalf,		// 0 - только большие цифры
	uint_fast8_t lowhalf		// lower half
	);

void
display_value_lower(
	uint_fast8_t xcell,	// x координата начала вывода значения
	uint_fast8_t ycell,	// y координата начала вывода значения
	uint_fast32_t freq,
	uint_fast8_t width, // = 8;	// full width
	uint_fast8_t comma, // = 2;	// comma position (from right, inside width)
	uint_fast8_t rj
	);

void
display_value_small(
	uint_fast8_t xcell,	// x координата начала вывода значения
	uint_fast8_t ycell,	// y координата начала вывода значения
	int_fast32_t freq,
	uint_fast8_t width,	// full width (if >= 128 - display with sign)
	uint_fast8_t comma,		// comma position (from right, inside width)
	uint_fast8_t comma2,
	uint_fast8_t rj,		// right truncated
	uint_fast8_t lowhalf
	);

void display_value_small_xy(
	uint_fast16_t xpix,	// x координата начала вывода значения
	uint_fast16_t ypix,	// y координата начала вывода значения
	int_fast32_t freq,
	COLOR565_T fg
	);

void display_floodfill(
	PACKEDCOLORMAIN_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x,	// начальная координата
	uint_fast16_t y,	// начальная координата
	COLORMAIN_T newColor,
	COLORMAIN_T oldColor,
	uint_fast8_t type	// 0 - быстрая закраска (только выпуклый контур), 1 - более медленная закраска любого контура
	);

COLORPIP_T getshadedcolor(
	COLORPIP_T dot, // исходный цвет
	uint_fast8_t alpha	// на сколько затемнять цвета (0 - чёрный, 255 - без изменений)
	);


#define WSIGNFLAG 0x80	// отображается плюс или минус в зависимости от знака значения
#define WMINUSFLAG 0x40	// отображается пробел или минус в зависимости от знака значения
#define WWIDTHFLAG 0x3F	// оставшиеся биты под ширину поля

int display_vtty_putchar(char ch);
void display_vtty_printf(const char * format, ...);
void vtty_printhex(unsigned long voffs, const unsigned char * buff, unsigned length);

int display_vtty_maxx(void);
int display_vtty_maxy(void);
void display_vtty_gotoxy(unsigned x, unsigned y);

void display_vtty_x2_initialize(void);
int display_vtty_x2_putchar(char ch);
// копирование растра в видеобуфер отображения
void display_vtty_x2_show(
	uint_fast16_t x,
	uint_fast16_t y
	);
// копирование растра в видеобуфер отображения
// с поворотом вправо на 90 градусов
void display_vtty_x2_show_ra90(
	uint_fast16_t x,
	uint_fast16_t y
	);
void display_vtty_x2_printf(const char * format, ...);

int display_vtty_x2_maxx(void);
int display_vtty_x2_maxy(void);
void display_vtty_x2_gotoxy(unsigned x, unsigned y);

void openvg_init(PACKEDCOLORMAIN_T * const * frames);
void openvg_deinit(void);
void openvg_next(unsigned page);		// текущий буфер отрисовки становится отображаемым, OpenVG переключается на следующий буфер


extern const char * savestring;
extern const char * savewhere;

#if WITHRLEDECOMPRESS

typedef struct
{
	const uint16_t width;
	const uint16_t height;
	const uint16_t * data;
} picRLE_t;

PACKEDCOLORMAIN_T convert_565_to_a888(uint16_t color);
void graw_picture_RLE(uint16_t x, uint16_t y, const picRLE_t * picture, PACKEDCOLORMAIN_T bg_color);
void graw_picture_RLE_buf(PACKEDCOLORMAIN_T * const buf, uint_fast16_t dx, uint_fast16_t dy, uint16_t x, uint16_t y, const picRLE_t * picture, PACKEDCOLORMAIN_T bg_color);

#endif /* WITHRLEDECOMPRESS */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DISPLAY_H_INCLUDED */
