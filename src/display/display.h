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

	#elif LCDMODE_PALETTE256

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

	#elif LCDMODE_ARGB8888

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

	#elif LCDMODE_RGB565

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

	#endif /* LCDMODE_PALETTE256 */

#endif /* LCDMODE_LTDC */

// Get color components from RGB565 value
#define RGB565_A(v) (255)
#define RGB565_R(v) ((((v) & 0xF800) >> 8) | (((v) & 0xE000) >> 13))
#define RGB565_G(v) ((((v) & 0x07E0) >> 3) | (((v) & 0x0600) >> 9))
#define RGB565_B(v) ((((v) & 0x001F) << 3) | (((v) & 0x001C) >> 2))

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


typedef struct unifont_tag
{
	uint_fast16_t (* decode)(const struct unifont_tag * font, char cc);	// получение ci
	const void * (* getcharraster)(const struct unifont_tag * font, char c);	// получение начального адреса растра для символа
	uint_fast8_t (* font_drawwidth)(const struct unifont_tag * font, char cc);	// ширина в пиксеях данного символа (может быть меньше чем поле width)
	uint_fast8_t (* font_drawheight)(const struct unifont_tag * font);	// высота в пикселях (се символы шрифта одной высоты)
	uint_fast16_t (* font_draw)(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, const struct unifont_tag * font, char cc, COLORPIP_T fg);
	uint_fast16_t (* font_prerender)(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, const struct unifont_tag * font, char cc, COLORPIP_T fg);
	uint8_t bytesw;		// байтов в одной строке знакогенератора символа
	const void * fontraster;		// начало знакогенератора в памяти
	const char * label;		// название для диагностики
} unifont_t;

enum gxstyle_texthalign
{
	GXSTYLE_HALIGN_LEFT,
	GXSTYLE_HALIGN_CENTER,
	GXSTYLE_HALIGN_RIGHT
};

enum gxstyle_textvalign
{
	GXSTYLE_VALIGN_TOP,
	GXSTYLE_VALIGN_CENTER,
	GXSTYLE_VALIGN_BOTTOM
};

typedef struct gxstyle_tag
{
	PACKEDCOLORPIP_T textcolor, bgcolor;
	uint8_t bgradius;	// радиус углов прямоугольника контура
	uint8_t bgfilled;	// необходимость заполнения прямоугольника контура
	uint_fast16_t bgbackoffw;	// уменьшение размера плашки по горизонтали
	uint_fast16_t bgbackoffh;	// уменьшение размера плашки по вертикали
	enum gxstyle_texthalign	texthalign;
	enum gxstyle_textvalign textvalign;
	const unifont_t * font;		// Шрифт, который будет использоваться
} gxstyle_t;

void gxstyle_initialize(gxstyle_t * dbstyle);
void gxstyle_textcolor(gxstyle_t * dbstyle, COLORPIP_T fg, COLORPIP_T bg);
void gxstyle_texthalign(gxstyle_t * dbstyle, enum gxstyle_texthalign a);
void gxstyle_textvalign(gxstyle_t * dbstyle, enum gxstyle_textvalign a);
void gxstyle_setsmallfont(gxstyle_t * dbstyle, const unifont_t * font);
void gxstyle_setsbigandhalffont(gxstyle_t * dbstyle);
void gxstyle_setbgbackoff(gxstyle_t * dbstyle, unsigned x, unsigned y);
void gxstyle_setbgradius(gxstyle_t * dbstyle, unsigned r);
void gxstyle_setbgrfilled(gxstyle_t * dbstyle, unsigned f);

// Интерфейсные функции, специфические для драйвера дисплея - зависящие от типа микросхемы контроллера.
void display_hardware_initialize(void);
void display_reset(void);
void display_initialize(void);
void display_uninitialize(void);
void display_lvgl_initialize(void);
uint32_t display_get_lvformat(void);	/* получить LVGL код формата цвета, используемый в построении  изображений */

void display_gpu_initialize(void);		/* g2d/mdma/gpu/dma2d initialize */

// Используется при выводе на графический индикатор с кординатами и размерами по сетке
void display_text(const gxdrawb_t * db, uint_fast8_t xcell, uint_fast8_t ycell, const char * s, uint_fast8_t xspan, uint_fast8_t yspan, const gxstyle_t * dbstyle);		// Выдача строки из ОЗУ в указанное место экрана.
// Используется при выводе на графический индикатор с кординатами и размерами в пикселях
void pix_display_text(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, uint_fast16_t w, uint_fast16_t h, const gxstyle_t * dbstyle, const char * s);
// Используется при выводе на графический индикатор с кординатами и размерами в пикселях
// Многострочное отображение
void pix_display_texts(const gxdrawb_t * db, uint_fast16_t xpix, uint_fast16_t ypix, uint_fast16_t w, uint_fast16_t h, const gxstyle_t * dbstyle, const char * const * slines, unsigned nlines);

uint_fast16_t
colpip_string(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const unifont_t * font,
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	);
// получить оба размера текстовой строки
uint_fast16_t
colpip_string_widthheight(
	const unifont_t * font,
	const char * s,
	uint_fast16_t * height
	);

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

// Заполнение буфера сполшным цветом
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

// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
uint_fast16_t
colpip_string_small(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
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

void display_bar(
	const gxdrawb_t * db,
	uint_fast16_t xpix,
	uint_fast16_t ypix,
	uint_fast8_t width,	/* количество знакомест, занимаемых индикатором */
	uint_fast8_t rowspan,	/* количество ячеек по вертикали, занимаемых индикатором */
	int_fast16_t value,		/* значение, которое надо отобразить */
	int_fast16_t tracevalue,		/* значение маркера, которое надо отобразить */
	int_fast16_t topvalue,	/* значение, соответствующее полностью заполненному индикатору */
	uint_fast8_t pattern,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL */
	uint_fast8_t patternmax,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL - для отображения запомненного значения */
	uint_fast8_t emptyp,			/* паттерн для заполнения между штрихами */
	const gxstyle_t * dbstyle	/* foreground and background colors, text alignment */
	);


// большие и средние цифры (частота)
uint_fast16_t display_wrdata_begin(uint_fast8_t xcell, uint_fast8_t ycell, uint_fast16_t * yp);

void display_swrmeter(const gxdrawb_t * db,
	uint_fast8_t x,
	uint_fast8_t y,
	uint_fast8_t colspan,
	uint_fast8_t rowspan,
	adcvalholder_t forward,
	adcvalholder_t reflected, // скорректированное
	uint_fast16_t minforward
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
#define FILL_FLAG_MIXBG		(1u << 0)	// alpha со старым содержимым буферв

/* заполнение прямоугольника в буфере произвольным цветом
*/
void
colpip_rectangle(
	const gxdrawb_t * db,
	uint_fast16_t x, uint_fast16_t y, 	// координаты в пикселях
	uint_fast16_t w, uint_fast16_t h, 	// размеры в пикселях
	COLORPIP_T color,
	unsigned fillmask,
	unsigned alpha
	);

#define BITBLT_FLAG_NONE			0u
#define BITBLT_FLAG_CKEY			(1u << 0)	// colpip_bitblt use keycolor parameter
#define BITBLT_FLAG_XMIRROR			(1u << 1)
#define BITBLT_FLAG_YMIRROR			(1u << 2)
#define BITBLT_FLAG_SRC_ABGR8888	(1u << 3)	/* исходный имедж - ABGR8888 (от LuPng) */

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
	int style);			// не-0: x2 растягивание по горизонтали

// Нарисовать вертикальную цветную полосу
void
colpip_xor_vline(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	COLORPIP_T color
	);

// Нарисовать вертикальную цветную полосу
void
colpip_set_vline(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата начального пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата начального пикселя (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	COLORPIP_T color
	);

// Нарисовать горизонтальную цветную полосу
void
colpip_set_hline(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата начального пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата начального пикселя (0..dy-1) сверху вниз
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

void colpip_line(
	const gxdrawb_t * db,
	int xn, int yn,
	int xk, int yk,
	COLORPIP_T color,
	int antialiasing
	);

// Отображение цифр в поле "больших цифр" - индикатор основной частоты настройки аппарата.
void
display_freq(
	const gxdrawb_t * db,
	uint_fast8_t xcell,	// x координата начала вывода значения
	uint_fast8_t ycell,	// y координата начала вывода значения
	uint_fast8_t xspan,
	uint_fast8_t yspan,
	int_fast32_t freq,
	uint_fast8_t width, // = 8;	// full width
	uint_fast8_t comma, // = 2;	// comma position (from right, inside width)
	uint_fast8_t comma2,	// = comma + 3;		// comma position (from right, inside width)
	uint_fast8_t rj,	// = 1;		// right truncated
	uint_fast8_t blinkpos,		// позиция, где символ заменён пробелом
	uint_fast8_t blinkstate,	// 0 - пробел, 1 - курсор
	uint_fast8_t withhalf,		// 0 - только большие цифры
	const gxstyle_t * dbstyle	/* foreground and background colors, text alignment */
	);

void
pix_display_value_big(
	const gxdrawb_t * db,
	uint_fast16_t xpix,	// x координата начала вывода значения
	uint_fast16_t ypix,	// y координата начала вывода значения
	uint_fast16_t w,
	uint_fast16_t h,
	int_fast32_t freq,
	uint_fast8_t width, // = 8;	// full width
	uint_fast8_t comma, // = 2;	// comma position (from right, inside width)
	uint_fast8_t comma2,	// = comma + 3;		// comma position (from right, inside width)
	uint_fast8_t rj,	// = 1;		// right truncated
	uint_fast8_t blinkpos,		// позиция, где символ заменён пробелом
	uint_fast8_t blinkstate,	// 0 - пробел, 1 - курсор
	uint_fast8_t withhalf,		// 0 - только большие цифры
	const gxstyle_t * dbstyle	/* foreground and background colors, text alignment */
	);

void rendered_value_big_initialize(const gxstyle_t * gxstylep);	// Подготовка отображения больщих символов valid chars: "0123456789 #._"

void
pix_display_value_small(
	const gxdrawb_t * db,
	uint_fast16_t xpix,	// x координата начала вывода значения
	uint_fast16_t ypix,	// y координата начала вывода значения
	uint_fast16_t w,
	uint_fast16_t h,
	int_fast32_t freq,
	uint_fast8_t width,	// full width (if >= 128 - display with sign)
	uint_fast8_t comma,		// comma position (from right, inside width)
	uint_fast8_t comma2,
	uint_fast8_t rj,		// right truncated
	const gxstyle_t * dbstyle	/* foreground and background colors, text alignment */
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

#if 1//WITHRLEDECOMPRESS

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

typedef struct pipparams_tag
{
	uint16_t x, y, w, h;	// в пикселях
} pipparams_t;

void display2_getpipparams(pipparams_t * p);	/* получить координаты окна с панорамой и/или водопадом. */
void board_set_tvoutformat(uint_fast8_t v);	/* установить видеорежим */

//--------------------------------------------------------------
// Структура шрифта одного размера (не более 16 пикселей шириной)
//--------------------------------------------------------------
typedef struct UB_Font_t {
  const uint16_t *table; // Таблица с данными
  uint16_t width;        // Ширина символа (в пикселях)
  uint16_t height;       // Высота символа (в пикселях)
}UB_Font;


//--------------------------------------------------------------
// Структура шрифта одного размера (неболее 32 пикселей шириной)
//--------------------------------------------------------------
typedef struct UB_Font32_t {
  const uint32_t *table; // Таблица с данными
  uint16_t width;        // Ширина символа (в пикселях)
  uint16_t height;       // Высота символа (в пикселях)
}UB_Font32;

//--------------------------------------------------------------
// Структура пропорционального шрифта (не более 16 пикселей шириной)
//--------------------------------------------------------------
typedef struct UB_pFont_t {
  const uint16_t *table; // Таблица с данными
  uint16_t height;       // Высота символа (в пикселях)
  uint16_t first_char;   // Первый символ  (Ascii код)
  uint16_t last_char;    // Последний символ (Ascii код)
}UB_pFont;

//--------------------------------------------------------------
// Рисует строку шрифтом одного размера на позиции х, у.
// Цвет шрифта и фон (шрифт = макс 16 пикселей в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void UB_Font_DrawString(const gxdrawb_t * db, uint_fast16_t x, uint_fast16_t y, const char *ptr, const UB_Font *font, COLORPIP_T vg);

//--------------------------------------------------------------
// Рисует строку шрифтом одного размера на позиции х, у.
// Цвет шрифта и фон (шрифт = макс 32 пикселя в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void UB_Font_DrawString32(const gxdrawb_t * db, uint_fast16_t x, uint_fast16_t y, const char *ptr, const UB_Font32 *font, COLORPIP_T vg);


//--------------------------------------------------------------
// Структура пропорционального шрифта (не более 32 пикселей шириной)
//--------------------------------------------------------------
typedef struct UB_pFont32_t {
  const uint32_t *table; // Таблица с данными
  uint16_t height;       // Высота символа (в пикселях)
  uint16_t first_char;   // Первый символ  (Ascii код)
  uint16_t last_char;    // Последний символ (Ascii код)
}UB_pFont32;


//--------------------------------------------------------------
// Рисование строки пропорционального шрифта с позицией X, Y
// Цвет шрифта плана и фона (шрифт = макс 16 пикселей в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void UB_Font_DrawPStringDbg(
		const char * file, int line,
		const gxdrawb_t * db,
		uint_fast16_t x, uint_fast16_t y,
		const char * ptr, const UB_pFont * font,
		COLORPIP_T vg);

#define UB_Font_DrawPString(...) do { \
	UB_Font_DrawPStringDbg(__FILE__, __LINE__, __VA_ARGS__); \
	} while (0)

//--------------------------------------------------------------
// Рисование строку пропорционального шрифта с позицией X, Y
// Цвет шрифта плана и фона (шрифт = макс 32 пикселя в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void UB_Font_DrawPString32(const gxdrawb_t * db,
		uint_fast16_t x, uint_fast16_t y,
		const char * ptr, const UB_pFont32 * font,
		COLORPIP_T vg);

// Возврат ширины строки в пикселях, пропорциональный шрифт 32 бит
uint16_t getwidth_Pstring32(const char * str, const UB_pFont32 * font);

// Возврат ширины строки в пикселях, пропорциональный шрифт меньше 32 бит
uint16_t getwidth_Pstring(const char * str, const UB_pFont * font);

// Возвращает ширину строки в пикселях, моноширинный шрифт
uint16_t getwidth_Mstring(const char * str, const UB_Font * font);

// Функуии (поля unifont_t) для работы с UB_pFont и UB_Font

// Для моноширинных знакогенераторов
uint_fast8_t ubmfont_width(const unifont_t * font, char cc);
uint_fast8_t ubmfont_height(const unifont_t * font);
uint_fast16_t ubmfont_decode(const unifont_t * font, char cc);
const void * ubmfont_getcharraster(const unifont_t * font, char cc);
uint_fast16_t ubmfont_render_char16(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	char cc,		// код символа для отображения
	COLORPIP_T fg
	);
uint_fast16_t ubmfont_render_char32(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	char cc,		// код символа для отображения
	COLORPIP_T fg
	);

// Для пропорциональных знакогенераторов
uint_fast8_t ubpfont_width(const unifont_t * font, char cc);
uint_fast8_t ubpfont_height(const unifont_t * font);
uint_fast16_t ubpfont_decode(const unifont_t * font, char cc);
const void * ubpfont_getcharraster(const unifont_t * font, char cc);
uint_fast16_t ubpfont_render_char16(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	char cc,		// код символа для отображения
	COLORPIP_T fg
	);
uint_fast16_t ubpfont_render_char32(
	const gxdrawb_t * db,
	uint_fast16_t xpix, uint_fast16_t ypix,	// позиция символа в целевом буфере
	const unifont_t * font,
	char cc,		// код символа для отображения
	COLORPIP_T fg
	);


// Готовые шрифты
extern UB_pFont gothic_12x16_p;
extern UB_Font gothic_11x13;

extern const unifont_t unifont_big;
extern const unifont_t unifont_half;
extern const unifont_t unifont_small;
extern const unifont_t unifont_small_x2;
extern const unifont_t unifont_small2;
extern const unifont_t unifont_small3;
extern const unifont_t unifont_gothic_11x13;
extern const unifont_t unifont_gothic_12x16p;	// proportional
extern const unifont_t unifont_Tahoma_Regular_88x77;
extern const unifont_t unifont_roboto32;
extern const unifont_t unifont_helvNeueTh70;

#ifdef __cplusplus
}
#endif /* __cplusplus */

/* RGB 24-bits color table definition (RGB888). */
#define COLOR_BLACK          TFTRGB(0x00, 0x00, 0x00)
#define COLOR_WHITEALL          TFTRGB(0xFF, 0xFF, 0xFF)
#define COLOR_BLUE           TFTRGB(0x00, 0x00, 0xFF)
#define COLOR_GREEN          TFTRGB(0x00, 0xFF, 0x00)
#define COLOR_RED            TFTRGB(0xFF, 0x00, 0x00)
#define COLOR_NAVY           TFTRGB(0x00, 0x00, 0x80)
#define COLOR_DARKBLUE       TFTRGB(0x00, 0x00, 0x8B)
#define COLOR_DARKGREEN      TFTRGB(0x00, 0x64, 0x00)
#define COLOR_DARKGREEN2     TFTRGB(0x00, 0x20, 0x00)
#define COLOR_DARKCYAN       TFTRGB(0x00, 0x8B, 0x8B)
#define COLOR_CYAN           TFTRGB(0x00, 0xFF, 0xFF)
#define COLOR_TURQUOISE      TFTRGB(0x40, 0xE0, 0xD0)
#define COLOR_INDIGO         TFTRGB(0x4B, 0x00, 0x82)
#define COLOR_DARKRED        TFTRGB(0x80, 0x00, 0x00)
#define COLOR_DARKRED2       TFTRGB(0x40, 0x00, 0x00)
#define COLOR_OLIVE          TFTRGB(0x80, 0x80, 0x00)
#define COLOR_DARKGRAY       TFTRGB(0x80, 0x80, 0x80)
#define COLOR_SKYBLUE        TFTRGB(0x87, 0xCE, 0xEB)
#define COLOR_BLUEVIOLET     TFTRGB(0x8A, 0x2B, 0xE2)
#define COLOR_LIGHTGREEN     TFTRGB(0x90, 0xEE, 0x90)
#define COLOR_DARKVIOLET     TFTRGB(0x94, 0x00, 0xD3)
#define COLOR_YELLOWGREEN    TFTRGB(0x9A, 0xCD, 0x32)
#define COLOR_BROWN          TFTRGB(0xA5, 0x2A, 0x2A)
#define COLOR_GRAY       	 TFTRGB(0xA9, 0xA9, 0xA9)
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
#define COLOR_BROWN   		 TFTRGB(0xA5, 0x2A, 0x2A)	// коричневый
#define COLOR_PEAR    		 TFTRGB(0xD1, 0xE2, 0x31)	// грушевый

#endif /* DISPLAY_H_INCLUDED */
