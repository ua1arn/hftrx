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

// Get color components from COLOR24_T value
#define COLOR24_R(v) (((v) >> 16) & 0xFF)
#define COLOR24_G(v) (((v) >> 8) & 0xFF)
#define COLOR24_B(v) (((v) >> 0) & 0xFF)

/* цвет должен корректно преобразовываться в RGB565 и обратно */
#define COLORPIP_KEY	(TFTRGB(0xA0, 0, 0xA0))	// Цвет для прозрачных пикселей

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

#if LCDMODE_DUMMY
	/* При использовании frame buffer цвета восьмибитные */
	typedef uint_fast8_t COLORPIP_T;
	typedef uint8_t PACKEDCOLORPIP_T;

	// для формирования растра с изображением водопада и спектра
	// RRRR.RGGG.GGGB.BBBB
	#define TFTRGB(red, green, blue) 0
	#define TFTALPHA(alpha, color) 0	/* No alpha channel supported in this mode */

#endif


#if LCDMODE_LTDC

	#define DISPLAYMODES_FPS 5	/* количество обновлений отображения режимов работы */
	#define DISPLAY_FPS	10	/* обновление показаний частоты десять раз в секунду */
	#define DISPLAYSWR_FPS 10	/* количество обновлений SWR и панорамы за секунду */

	#define LCDMODE_COLORED	1

	#if LCDMODE_LTDC_L24

		typedef uint_fast32_t COLORPIP_T;
		struct rgb_24b
		{
			uint8_t r, g, b;
		} ATTRPACKED;// аттрибут GCC, исключает "дыры" в структуре.

		typedef struct rgb_24b PACKEDCOLORPIP_T;	// 3 байта для последовательной выдачи как RGB для LCDMODE_ILI8961

		// RRRRRRR.GGGGGGGG.BBBBBBBB
		#define TFTRGB(red, green, blue) \
			(  (uint32_t) \
				(	\
					(((uint_fast32_t) (red) << 16) & 0xFF0000)  | \
					(((uint_fast32_t) (green) << 8) & 0xFF00) | \
					(((uint_fast32_t) (blue) << 0) & 0xFF) \
				) \
			)
		#define TFTALPHA(alpha, color) (color)	/* No alpha channel supported in this mode */

		// Get color components from framebuffer value
		#define COLORPIP_A(v) (255)
		#define COLORPIP_R(v) (((v) & 0xFF0000) >> 16)
		#define COLORPIP_G(v) (((v) & 0xFF00) >> 8)
		#define COLORPIP_B(v) (((v) & 0xFF) >> 0)

	#elif LCDMODE_MAIN_L8

		/* При использовании frame buffer цвета восьмибитные */
		typedef uint_fast8_t COLORPIP_T;
		typedef uint8_t PACKEDCOLORPIP_T;

		// RRRGGGBB
		#define TFTRGB(red, green, blue) \
			(  (unsigned char) \
				(	\
					(((red) >> 0) & 0xe0)  | \
					(((green) >> 3) & 0x1c) | \
					(((blue) >> 6) & 0x03) \
				) \
			)
		#define TFTALPHA(alpha, color) (color)	/* No alpha channel supported in this mode */
		// Get color components from framebuffer value
		#define COLORPIP_A(v) (255)
		#define COLORPIP_R(v) (((v) & 0xE0) >> 0)
		#define COLORPIP_G(v) (((v) & 0x1C) << 3)
		#define COLORPIP_B(v) (((v) & 0x03) << 6)

	#elif LCDMODE_MAIN_ARGB8888

		typedef uint_fast32_t COLORPIP_T;
		typedef uint32_t PACKEDCOLORPIP_T;

		// AAAAAAAA.RRRRRRR.GGGGGGGG.BBBBBBBB
		#define TFTRGB(red, green, blue) \
			(  (uint_fast32_t) ( \
					((uint_fast32_t) (255) << 24)  | /* Alpha channel value - opaque */ \
					(((uint_fast32_t) (red) << 16) & 0xFF0000)  | \
					(((uint_fast32_t) (green) << 8) & 0xFF00) | \
					(((uint_fast32_t) (blue) << 0) &  0x00FF) \
				) \
			)
		#define TFTALPHA(alpha, color24) \
			(  (uint_fast32_t) ( \
					((uint_fast32_t) (alpha) << 24)  | /* Alpha value, 0: transparent, 255: opaque */ \
					(((uint_fast32_t) (color24)) & 0x00FFFFFF) \
				) \
			)

		// Get color components from framebuffer value
		#define COLORPIP_A(v) (((v) & 0xFF000000) >> 24)
		#define COLORPIP_R(v) (((v) & 0xFF0000) >> 16)
		#define COLORPIP_G(v) (((v) & 0xFF00) >> 8)
		#define COLORPIP_B(v) (((v) & 0xFF) >> 0)

	#elif LCDMODE_MAIN_RGB565

		typedef uint_fast16_t COLORPIP_T;
		typedef uint16_t PACKEDCOLORPIP_T;

		// RRRR.RGGG.GGGB.BBBB
		#define TFTRGB(red, green, blue) ( \
				(uint_fast16_t) (	\
					(((uint_fast16_t) (red) << 8) &   0xF800)  | \
					(((uint_fast16_t) (green) << 3) & 0x07E0) | \
					(((uint_fast16_t) (blue) >> 3) &  0x001F) \
				) \
			)
		#define TFTALPHA(alpha, color) (color)	/* No alpha channel supported in this mode */

		// Get color components from framebuffer value
		#define COLORPIP_A(v) (255)
		#define COLORPIP_R(v) ((((v) & 0xF800) >> 8) | (((v) & 0xE000) >> 13))
		#define COLORPIP_G(v) ((((v) & 0x07E0) >> 3) | (((v) & 0x0600) >> 9))
		#define COLORPIP_B(v) ((((v) & 0x001F) << 3) | (((v) & 0x001C) >> 2))

	#endif /* LCDMODE_MAIN_L8 */

#endif /* LCDMODE_LTDC */

#ifndef DCACHEROWSIZE
	#define GXALIGN 1	/* количество пикселей в строке видеобуфера кратно этому заначению */
#elif defined (LCDMODE_PIXELSIZE)
	#define GXALIGN (DCACHEROWSIZE / LCDMODE_PIXELSIZE)	/* количество пикселей в строке видеобуфера кратно этому заначению */
#else
#define GXALIGN 1	/* количество пикселей в строке видеобуфера кратно этому заначению */
#endif

#define GXADJ(dx) (((dx) + (GXALIGN - 1)) / GXALIGN * GXALIGN)
#define GXSIZE(dx, dy)	((uint_fast32_t) GXADJ((dx)) * (dy))	// размер буфера для цветного растра

// Draw Buffer
typedef struct gxdrawb_tag
{
	PACKEDCOLORPIP_T * buffer;
	uint16_t dx;	// горизонтальный размер в пикселях
	uint16_t dy;	// вертикальный размер в пикселях
	uintptr_t cachebase;
	int32_t cachesize;
	uint16_t stride;
	void * layerv;	// LVGL lv_layer_t *
} gxdrawb_t;

void gxdrawb_initialize(gxdrawb_t * db, PACKEDCOLORPIP_T * buffer, uint_fast16_t dx, uint_fast16_t dy);
void gxdrawb_initlvgl(gxdrawb_t * db, void * layer);

// Интерфейсные функции, специфические для драйвера дисплея - зависящие от типа микросхемы контроллера.
void display_hardware_initialize(void);	/* вызывается при запрещённых прерываниях. */
void display_reset(void);				/* вызывается при разрешённых прерываниях. */
void display_initialize(void);			/* вызывается при разрешённых прерываниях. */
void display_uninitialize(void);			/* вызывается при разрешённых прерываниях. */
void display_lvgl_initialize(void);			/* вызывается при разрешённых прерываниях. */
uint32_t display_get_lvformat(void);	/* получить LVGL код формата цвета, используемый в построении  изображений */

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
void display_clear(const gxdrawb_t * db);	// Заполниить цветом фона
void colmain_setcolors(COLORPIP_T fg, COLORPIP_T bg);
void colmain_setcolors3(COLORPIP_T fg, COLORPIP_T bg, COLORPIP_T bgfg);	// bgfg - цвет для отрисовки антиалиасинга

// самый маленький шрифт
uint_fast16_t display_wrdata2_begin(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp);
void display_wrdata2_end(const gxdrawb_t * db);
uint_fast16_t display_put_char_small2(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc, uint_fast8_t lowhalf);
// полоса индикатора
uint_fast16_t display_wrdatabar_begin(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp);
uint_fast16_t display_barcolumn(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, uint_fast8_t pattern);	// Выдать восемь цветных пикселей, младший бит - самый верхний в растре
void display_wrdatabar_end(const gxdrawb_t * db);
// большие и средние цифры (частота)
uint_fast16_t display_wrdatabig_begin(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp);
uint_fast16_t display_put_char_big(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc, uint_fast8_t lowhalf);
uint_fast16_t display_put_char_half(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc, uint_fast8_t lowhalf);
void display_wrdatabig_end(const gxdrawb_t * db);
// обычный шрифт
uint_fast16_t display_wrdata_begin(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp);
uint_fast16_t display_put_char_small(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc, uint_fast8_t lowhalf);
uint_fast16_t display_put_char_small_xy(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc, COLOR565_T fg);
void display_wrdata_end(const gxdrawb_t * db);

// большие и средние цифры (частота)
uint_fast16_t render_wrdatabig_begin(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp);
uint_fast16_t render_char_big(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc, uint_fast8_t lowhalf);
uint_fast16_t render_char_half(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, char cc, uint_fast8_t lowhalf);
void render_wrdatabig_end(const gxdrawb_t * db);

typedef struct pipparams_tag
{
	uint16_t x, y, w, h;	// в пикселях
} pipparams_t;

void display2_getpipparams(pipparams_t * p);	/* получить координаты окна с панорамой и/или водопадом. */
const char * display2_gethtml(uint_fast8_t page);

// Заполнение буфера сполшным цветом
// Формат RGB565
// Эта функция используется только в тесте
void colpip_fill(
	const gxdrawb_t * db,
	COLORPIP_T color
	);

// Нарисовать закрашенный или пустой прямоугольник
void colpip_rect(
	const gxdrawb_t * db,
	uint_fast16_t x1,	// начальная координата
	uint_fast16_t y1,	// начальная координата
	uint_fast16_t x2,	// конечная координата (включена в заполняемую облсть)
	uint_fast16_t y2,	// конечная координата (включена в заполняемую облсть)
	COLORPIP_T color,
	uint_fast8_t fill
	);

// Поставить цветную точку.
void colpip_point(
	const gxdrawb_t * db,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	);

// поставить цветную точку (модификация с сохранением старого изоьражения).
void colpip_point_xor(
	const gxdrawb_t * db,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	);

/* возвращает новую позицию по x */
uint_fast16_t
colpip_string(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s
	);

// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string_tbg(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	);
// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string_x2_tbg(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	);
// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string_x2ra90_tbg(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg,		// цвет вывода текста
	COLORPIP_T bg		// цвет фона
	);
// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_text(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T fg,		// цвет вывода текста
	const char * s,		// строка для вывода
	size_t len			// количество символов
	);
// Используется при выводе на графический индикатор,
void
colpip_text_x2(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T fg,		// цвет вывода текста
	const char * s,		// строка для вывода
	size_t len			// количество символов
	);
// Используется при выводе на графический индикатор,
void
colpip_string_x2ra90_count(
	const gxdrawb_t * db,
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
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	);
// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void colpip_string3_tbg(
	const gxdrawb_t * db,
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
display_scroll_down(const gxdrawb_t * db,
	uint_fast16_t x0,	// левый верхний угол окна
	uint_fast16_t y0,	// левый верхний угол окна
	uint_fast16_t w, 	// до 65535 пикселей - ширина окна
	uint_fast16_t h, 	// до 65535 пикселей - высота окна
	uint_fast16_t n,	// количество строк прокрутки
	int_fast16_t hshift	// количество пиксеелей для сдвига влево (отрицательное число) или вправо (положительное).
	);

/* копирование содержимого окна с перекрытием для водопада */
void
display_scroll_up(const gxdrawb_t * db,
	uint_fast16_t x0,	// левый верхний угол окна
	uint_fast16_t y0,	// левый верхний угол окна
	uint_fast16_t w, 	// до 65535 пикселей - ширина окна
	uint_fast16_t h, 	// до 65535 пикселей - высота окна
	uint_fast16_t n,	// количество строк прокрутки
	int_fast16_t hshift	// количество пиксеелей для сдвига влево (отрицательное число) или вправо (положительное).
	);

void
display_panel(const gxdrawb_t * db,
	uint_fast8_t x, // левый верхний угод
	uint_fast8_t y,
	uint_fast8_t w, // ширина и высота в знакоместах
	uint_fast8_t h
	);

void
display2_menu_value(const gxdrawb_t * db,
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
	const gxdrawb_t * db,
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

void display_at(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, const char * s);		// Выдача строки из ОЗУ в указанное место экрана.
void display_x2_at(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, const char * s);		// Выдача строки из ОЗУ в указанное место экрана.
void display_at_P(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, const char * s); // Выдача строки из ПЗУ в указанное место экрана.


void display_swrmeter(const gxdrawb_t * db,
	uint_fast8_t x,
	uint_fast8_t y,
	adcvalholder_t forward,
	adcvalholder_t reflected, // скорректированное
	uint_fast16_t minforward
	);

/* заполнение прямоугольника на основном экране произвольным цветом
*/
void
display_fillrect(
	const gxdrawb_t * db,
	uint_fast16_t x, uint_fast16_t y, 	// координаты в пикселях
	uint_fast16_t w, uint_fast16_t h, 	// размеры в пикселях
	COLORPIP_T color
	);
/* рисование линии на основном экране произвольным цветом
*/
void
display_line(const gxdrawb_t * db,
	int x1, int y1,
	int x2, int y2,
	COLORPIP_T color
	);

/* заполнение прямоугольника в буфере произвольным цветом
*/
void
colpip_fillrect(
	const gxdrawb_t * db,
	uint_fast16_t x, uint_fast16_t y, 	// координаты в пикселях
	uint_fast16_t w, uint_fast16_t h, 	// размеры в пикселях
	COLORPIP_T color
	);

#define FILL_FLAG_NONE		0x00
#define FILL_FLAG_MIXBG		0x01	// alpha со старым содержимым буферв

/* заполнение прямоугольника в буфере произвольным цветом
*/
void
colpip_fillrect2(
	const gxdrawb_t * db,
	uint_fast16_t x, uint_fast16_t y, 	// координаты в пикселях
	uint_fast16_t w, uint_fast16_t h, 	// размеры в пикселях
	COLORPIP_T color,
	unsigned fillmask
	);

#define BITBLT_FLAG_NONE			0x00
#define BITBLT_FLAG_CKEY			0x01
#define BITBLT_FLAG_SRC_ABGR8888		0x02	/* исходный имедж - ABGR8888 (от LuPng) */

// скоприовать прямоугольник с типом пикселей соответствующим pip
void colpip_bitblt(
	uintptr_t dstinvalidateaddr,	int_fast32_t dstinvalidatesize,	// параметры clean invalidate получателя
	const gxdrawb_t * tdb,	// получатель
	uint_fast16_t x,	uint_fast16_t y,	// позиция получателя
	uintptr_t srcinvalidateaddr,	int_fast32_t srcinvalidatesize,	// параметры clean источника
	const gxdrawb_t * sdb, 	// источник
	uint_fast16_t sx,	uint_fast16_t sy,	// источник Позиция окна
	uint_fast16_t sw,	uint_fast16_t sh,	// Размеры окна источника
	unsigned bitbltmask, COLORPIP_T keycolor
	);

// скоприовать прямоугольник с типом пикселей соответствующим pip
void colpip_stretchblt(
	uintptr_t dstinvalidateaddr,	int_fast32_t dstinvalidatesize,	// параметры clean invalidate получателя
	const gxdrawb_t * tdb,	// получатель
	uint_fast16_t x,	uint_fast16_t y,	// позиция получателя
	uint_fast16_t w,	uint_fast16_t h,	// Размеры окна получателя
	uintptr_t srcinvalidateaddr,	int_fast32_t srcinvalidatesize,	// параметры clean источника
	const gxdrawb_t * sdb,	// source buffer
	uint_fast16_t sx,	uint_fast16_t sy,	// источник Позиция (размеры совпадают с получателем)
	uint_fast16_t sw,	uint_fast16_t sh,	// Размеры окна источника
	unsigned keyflag, COLORPIP_T keycolor
	);

// копирование с поворотом
void colpip_copyrotate(
	uintptr_t dstinvalidateaddr,	int_fast32_t dstinvalidatesize,	// параметры clean invalidate получателя
	const gxdrawb_t * tdb,	// получатель
	uint_fast16_t x,	// начальная координата
	uint_fast16_t y,	// начальная координата
	uintptr_t srcinvalidateaddr,	int_fast32_t srcinvalidatesize,	// параметры clean источника
	const gxdrawb_t * sdb,	// source buffer
	uint_fast16_t sx,	// начальная координата
	uint_fast16_t sy,	// начальная координата
	uint_fast16_t w, uint_fast16_t h,	// source rectangle size
	uint_fast8_t mx,	// X mirror flag
	uint_fast8_t my,	// X mirror flag
	unsigned angle	// positive CCW angle
	);

// скоприовать прямоугольник с типом пикселей соответствующим pip
// с поворотом вправо на 90 градусов
void colpip_bitblt_ra90(
	uintptr_t dstinvalidateaddr,	// параметры clean invalidate получателя
	int_fast32_t dstinvalidatesize,
	const gxdrawb_t * tdb,	// получатель
	uint_fast16_t x,	// получатель Позиция
	uint_fast16_t y,	// получатель
	uintptr_t srcinvalidateaddr,	// параметры clean источника
	int_fast32_t srcinvalidatesize,
	const gxdrawb_t * sdb 	// источник
	);

void
colpip_string3_at_xy(
	const gxdrawb_t * db,
	uint_fast16_t x,
	uint_fast16_t y,
	const char * __restrict s
	);

uint_fast8_t colpip_hasalpha(void);

/* Нарисовать прямоугольник со скругленными углами */
void
colmain_rounded_rect(
	const gxdrawb_t * db,
	uint_fast16_t x1,
	uint_fast16_t y1,
	uint_fast16_t x2,
	uint_fast16_t y2,
	uint_fast8_t r,		// радиус закругления углов
	COLORPIP_T color,
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
colpip_radius(
	const gxdrawb_t * db,
	int xc, int yc,
	unsigned gs,
	unsigned r1, unsigned r2,
	COLORPIP_T color,
	int antialiasing,
	int style);			// 1 - растягивание по горизонтали

void
colpip_segm(
		const gxdrawb_t * db,
	int xc, int yc,
	unsigned gs, unsigned ge,
	unsigned r, int step,
	COLORPIP_T color,
	int antialiasing,
	int style);			// 1 - растягивание по горизонтали;

// Нарисовать вертикальную цветную полосу
void
colpip_xor_vline(
	const gxdrawb_t * db,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row0,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	COLORPIP_T color
	);

// Нарисовать вертикальную цветную полосу
// Формат RGB565
void
colpip_set_vline(
		const gxdrawb_t * db,
	uint_fast16_t col,	// горизонтальная координата начального пикселя (0..dx-1) слева направо
	uint_fast16_t row0,	// вертикальная координата начального пикселя (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	COLORPIP_T color
	);

// Нарисовать горизонтальную цветную полосу
// Формат RGB565
void
colpip_set_hline(
	const gxdrawb_t * db,
	uint_fast16_t col,	// горизонтальная координата начального пикселя (0..dx-1) слева направо
	uint_fast16_t row0,	// вертикальная координата начального пикселя (0..dy-1) сверху вниз
	uint_fast16_t w,	// ширина
	COLORPIP_T color
	);

// получить адрес требуемой позиции в буфере
PACKEDCOLORPIP_T *
colpip_mem_at_debug(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * file,
	int line
	);

// получить адрес требуемой позиции в буфере
const PACKEDCOLORPIP_T *
colpip_const_mem_at_debug(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * file,
	int line
	);

#define colpip_mem_at(a,b,c) (colpip_mem_at_debug((a), (b), (c), __FILE__, __LINE__))
#define colpip_const_mem_at(a,b,c) (colpip_const_mem_at_debug((a), (b), (c), __FILE__, __LINE__))

void display_putpixel(const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	);

void colpip_putpixel(const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	);

void display_snapshot(const gxdrawb_t * db);	/* запись видимого изображения */
void display_snapshot_write(const gxdrawb_t * db);	/* запись видимого изображения в файл */
void display_snapshot_req(void);

#if WITHLTDCHWVBLANKIRQ
	#define LCDMODE_MAIN_PAGES	3	// Используются буферы: один для полготовки изоображения, один готовый к отображению, один текцщий отображаемый
#else /* WITHLTDCHWVBLANKIRQ */
	#define LCDMODE_MAIN_PAGES	2	// Используются буферы: один для полготовки изоображения, один текцщий отображаемый
#endif /* WITHLTDCHWVBLANKIRQ */

PACKEDCOLORPIP_T * colmain_fb_draw(void);		// буфер для построения изображения
void colmain_nextfb(void);	// переключиться на использование для DRAW следующего фреймбуфера, текущий отобразить на основном дисплее
void colmain_nextfb_sub(void);	// переключиться на использование для DRAW следующего фреймбуфера, текущий отобразить на дополнительном дисплее
uint_fast8_t colmain_getindexbyaddr(uintptr_t addr);	// получить индекс видеобуфера по его адресу
void colmain_fb_list(uintptr_t * frames);	// получение массива планирующихся для работы framebuffers

//-----------------------
#define LCDMODE_TVOUT_PAGES	5	// Используются буферы: один для полготовки изоображения, один готовый к отображению, один текцщий отображаемый

#if WITHHDMITVHW
	// Второй дисплей масщтабиует на лету, на вход VI иждут кадры, совпадающие с основным дисплеем
	#define TVD_WIDTH  DIM_X
	#define TVD_HEIGHT DIM_Y
	typedef PACKEDCOLORPIP_T PACKEDTVBUFF_T;
	typedef COLORPIP_T TVBUFF_T;
	#define TVMODE_PIXELSIZE LCDMODE_PIXELSIZE
#else /* WITHHDMITVHW */
	#define TVD_WIDTH  720
	#define TVD_HEIGHT 576
	typedef uint8_t PACKEDTVBUFF_T;
	typedef uint_fast8_t TVBUFF_T;
	#define TVMODE_PIXELSIZE 1
#endif /* WITHHDMITVHW */

#define TVD_SIZE (TVD_WIDTH * TVD_HEIGHT)


PACKEDTVBUFF_T * tvout_fb_draw(void);
void tvout_nextfb(void);

#if WITHALPHA
#define DEFAULT_ALPHA WITHALPHA
#else
#define DEFAULT_ALPHA 128	// 0..255
#endif

void display2_xltrgb24(COLOR24_T * xtable);

void hwaccel_bitblt(
	uintptr_t dstinvalidateaddr,	// параметры clean invalidate получателя
	int_fast32_t dstinvalidatesize,
	const gxdrawb_t * tdb,
	uintptr_t srcinvalidateaddr,	// параметры clean источника
	int_fast32_t srcinvalidatesize,
	const gxdrawb_t * sdb,
	uint_fast16_t sw,	uint_fast16_t sh,	// Размеры окна источника
	unsigned keyflag, COLORPIP_T keycolor
	);

// копирование буфера с поворотом вправо на 90 градусов (четверть оборота).
void hwaccel_ra90(
	const gxdrawb_t * tdb,	// получатель
	uint_fast16_t tx,	// горизонтальная координата пикселя (0..dx-1) слева направо - в исходном нижний
	uint_fast16_t ty,	// вертикальная координата пикселя (0..dy-1) сверху вниз - в исходном левый
	const gxdrawb_t * sdb	// источник
	);

// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
void ltdc_horizontal_pixels(
	PACKEDCOLORPIP_T * tgr,		// target raster
	const uint8_t * raster,
	uint_fast16_t width	// number of bits (start from LSB first byte in raster)
	);

// Для произвольного шрифта
void ltdc_put_char_unified(
	const uint8_t * fontraster,
	uint_fast8_t width,		// пикселей в символе по горизонтали знакогнератора
	uint_fast8_t height,	// строк в символе по вертикали
	uint_fast8_t bytesw,	// байтов в одной строке символа
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	uint_fast8_t ci,	// индекс символа в знакогенераторе
	uint_fast8_t width2	// пикселей в символе по горизонтали отображается (для уменьшеных в ширину символов большиз шрифтов)
	);

// Установить прозрачность для прямоугольника
void display_transparency(const gxdrawb_t * db,
	uint_fast16_t x1, uint_fast16_t y1,
	uint_fast16_t x2, uint_fast16_t y2,
	uint_fast8_t alpha	// на сколько затемнять цвета (0 - чёрный, 255 - без изменений)
	);

void gpu_fillrect(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,	// ширина буфера
	uintptr_t taddr,
	uint_fast32_t tstride,
	uint_fast32_t tsizehw,
	unsigned alpha,
	COLOR24_T color24,
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	COLORPIP_T color	// цвет
	);

uint_fast8_t smallfont_decode(char cc);
uint_fast8_t bigfont_decode(char cc);
uint_fast8_t halffont_decode(char cc);
uint_fast8_t bigfont_width(char cc);
uint_fast8_t halffont_width(char cc);
uint_fast8_t smallfont_width(char cc);
uint_fast8_t smallfont2_width(char cc);
uint_fast8_t smallfont3_width(char cc);

int_fast32_t display_zoomedbw(void);

void display_string3_at_xy(const gxdrawb_t * db,uint_fast16_t x, uint_fast16_t y, const char * s, COLORPIP_T fg, COLORPIP_T bg);

void colpip_line(
	const gxdrawb_t * db,
	int xn, int yn,
	int xk, int yk,
	COLORPIP_T color,
	int antialiasing
	);

// Отображение цифр в поле "больших цифр" - индикатор основной частоты настройки аппарата.
void
display_value_big(
	const gxdrawb_t * db,
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
pix_display_value_big(
	const gxdrawb_t * db,
	uint_fast16_t xpix,	// x координата начала вывода значения
	uint_fast16_t ypix,	// y координата начала вывода значения
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

// Отображение цифр в поле "больших цифр" - индикатор основной частоты настройки аппарата.
/* из предварительно подготовленных буферов */
void
render_value_big(
	const gxdrawb_t * db,
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
pix_render_value_big(
	const gxdrawb_t * db,
	uint_fast16_t xpix,	// x координата начала вывода значения
	uint_fast16_t ypix,	// y координата начала вывода значения
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

void render_value_big_initialize(void);	// Подготовка отображения больщих символов valid chars: "0123456789 #._"

void
display_value_lower(
	const gxdrawb_t * db,
	uint_fast8_t xcell,	// x координата начала вывода значения
	uint_fast8_t ycell,	// y координата начала вывода значения
	uint_fast32_t freq,
	uint_fast8_t width, // = 8;	// full width
	uint_fast8_t comma, // = 2;	// comma position (from right, inside width)
	uint_fast8_t rj
	);

void
display_value_small(
	const gxdrawb_t * db,
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
	const gxdrawb_t * db,
	uint_fast16_t xpix,	// x координата начала вывода значения
	uint_fast16_t ypix,	// y координата начала вывода значения
	int_fast32_t freq,
	COLOR565_T fg
	);

void display_floodfill(
	const gxdrawb_t * db,
	uint_fast16_t x,	// начальная координата
	uint_fast16_t y,	// начальная координата
	COLORPIP_T newColor,
	COLORPIP_T oldColor
	);

COLORPIP_T getshadedcolor(
	COLORPIP_T dot, // исходный цвет
	uint_fast8_t alpha	// на сколько затемнять цвета (0 - чёрный, 255 - без изменений)
	);

void display_do_AA(
	const gxdrawb_t * db,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз)
	uint_fast16_t width,
	uint_fast16_t height
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
	const gxdrawb_t * db,
	uint_fast16_t x,
	uint_fast16_t y
	);
// копирование растра в видеобуфер отображения
// с поворотом вправо на 90 градусов
void display_vtty_x2_show_ra90(
	const gxdrawb_t * db,
	uint_fast16_t x,
	uint_fast16_t y
	);
void display_vtty_x2_printf(const char * format, ...);

int display_vtty_x2_maxx(void);
int display_vtty_x2_maxy(void);
void display_vtty_x2_gotoxy(unsigned x, unsigned y);

void openvg_init(const uintptr_t * frames);
void openvg_deinit(void);
void openvg_next(unsigned page);		// текущий буфер отрисовки становится отображаемым, OpenVG переключается на следующий буфер

void lvglhw_initialize(void);

extern const char * savestring;
extern const char * savewhere;

#if WITHRLEDECOMPRESS

typedef struct
{
	const uint16_t width;
	const uint16_t height;
	const uint16_t * data;
} picRLE_t;

COLORPIP_T convert_565_to_a888(uint16_t color);
void graw_picture_RLE(const gxdrawb_t * db, uint16_t x, uint16_t y, const picRLE_t * picture, PACKEDCOLORPIP_T bg_color);
void graw_picture_RLE_buf(const gxdrawb_t * db, uint16_t x, uint16_t y, const picRLE_t * picture, COLORPIP_T bg_color);

#endif /* WITHRLEDECOMPRESS */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DISPLAY_H_INCLUDED */
