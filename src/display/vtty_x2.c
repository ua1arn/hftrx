/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"

#if ! LCDMODE_DUMMY && LCDMODE_LTDC

#include "display.h"
#include "display2.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "formats.h"
#include "fontmaps.h"

#if DIM_X == 720
	enum { VTTYx2_CHARPIX = SMALLCHARW * 2 };	// количество пикселей по горизонтали на один символ текста
	enum { VTTYx2_ROWSPIX = (SMALLCHARH + 1) * 2 };	// количество пикселей по вертикали на одну строку текста
	enum { VTTYx2_COLS = (GRID2Y(200 - 3) - 4) / VTTYx2_CHARPIX };
	enum { VTTYx2_ROWS = (GRID2X(8 * 4 - 1) - 4) / VTTYx2_ROWSPIX };

#else
	enum { VTTYx2_CHARPIX = SMALLCHARW * 2/2 };	// количество пикселей по горизонтали на один символ текста
	enum { VTTYx2_ROWSPIX = (SMALLCHARH + 1) * 2/2 };	// количество пикселей по вертикали на одну строку текста
	enum { VTTYx2_COLS = DIM_X / VTTYx2_CHARPIX };
	enum { VTTYx2_ROWS = 240 /* DIM_Y */ / VTTYx2_ROWSPIX };

#endif
enum { VTTYx2_DX = VTTYx2_COLS * VTTYx2_CHARPIX, VTTYx2_DY = VTTYx2_ROWS * VTTYx2_ROWSPIX };

// цвета отрисовки

typedef struct vtty_x2_tag
{
	ALIGNX_BEGIN PACKEDCOLORPIP_T fb [GXSIZE(VTTYx2_DX, VTTYx2_DY)] ALIGNX_END;
	ALIGNX_BEGIN PACKEDCOLORPIP_T fgshadow [GXSIZE(VTTYx2_ROWS, VTTYx2_COLS)] ALIGNX_END;	// цвета символов
	ALIGNX_BEGIN PACKEDCOLORPIP_T bgshadow [GXSIZE(VTTYx2_ROWS, VTTYx2_COLS)] ALIGNX_END;	// цвета фона
	uint8_t shadow [VTTYx2_COLS] [VTTYx2_ROWS];
	unsigned scroll;	// эта строка отображается верхней в целевом прямоугольнике. 0..VTTYx2_ROWS-1
	unsigned row;		// 0..VTTYx2_ROWS-1
	unsigned col;		// 0..VTTYx2_COLS-1
} vtty_x2_t;

static RAMFRAMEBUFF ALIGNX_BEGIN vtty_x2_t vtty_x2_0 ALIGNX_END;

static int hasscroll = 0;

// позиции полей цветов в значении атрибутов отображения
#define AFGPOS 0
#define ABGPOS 4

static int gwintop = 1;
static int gwinleft = 1;
static int gwinright = VTTYx2_COLS;
static int gwinbottom = VTTYx2_ROWS;
static unsigned gattr = (0x0F << AFGPOS) | (0x00 << ABGPOS);	// FFFFBBBB
static COLORPIP_T gfg = COLORMAIN_WHITE;
static COLORPIP_T gbg = COLORMAIN_BLACK;

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
void display_vtty_x2_gotoxy(unsigned x, unsigned y);

void display_vtty_x2_initialize(void)
{
	vtty_x2_t * const vt = & vtty_x2_0;
	vt->scroll = 0;
	vt->row = 0;
	vt->col = 0;
	colmain_fillrect(vt->fb, VTTYx2_DX, VTTYx2_DY, 0, 0, VTTYx2_DX, VTTYx2_DY, gbg);	// очищаем видеобуфер
	colmain_fillrect(vt->fgshadow, VTTYx2_COLS, VTTYx2_ROWS, 0, 0, VTTYx2_COLS, VTTYx2_ROWS, gfg);	// очищаем видеобуфер
	colmain_fillrect(vt->bgshadow, VTTYx2_COLS, VTTYx2_ROWS, 0, 0, VTTYx2_COLS, VTTYx2_ROWS, gbg);	// очищаем видеобуфер
	memset(vt->shadow, ' ', sizeof vt->shadow);
	PRINTF("display_vtty_x2_initialize: rows=%u, cols=%u\n", VTTYx2_ROWS, VTTYx2_COLS);
}

void display_vtty_x2_gotoxy(unsigned x, unsigned y)
{
	vtty_x2_t * const vt = & vtty_x2_0;
	if (x >= VTTYx2_COLS)
		return;
	if (y >= VTTYx2_ROWS)
		return;
	vt->row = y;
	vt->col = x;
	ASSERT(vt->scroll < VTTYx2_ROWS);
	ASSERT(vt->row < VTTYx2_ROWS);
	ASSERT(vt->col < VTTYx2_COLS);
}
// копирование растра в видеобуфер отображения
void display_vtty_x2_show(
	uint_fast16_t x,
	uint_fast16_t y
	)
{
	PACKEDCOLORPIP_T * const tfb = colmain_fb_draw();
//	colmain_fillrect(tfb, DIM_X, DIM_Y, x, y, VTTYx2_DX, VTTYx2_DY, VTTYx2_BG);	// обозначам место под вывод информации
//	return;
	vtty_x2_t * const vt = & vtty_x2_0;
	enum { H = VTTYx2_ROWSPIX };
	// координаты верхней части в целевом видеобуфере
	const uint_fast16_t tgh1 = (VTTYx2_ROWS - vt->scroll) * H;	// высота
	const uint_fast16_t tgy1 = 0;
	// координаты нижней части в целевом видеобуфере
	const uint_fast16_t tgh2 = vt->scroll * H;	// высота
	const uint_fast16_t tgy2 = tgh1;

	ASSERT(vt->fb != NULL);
	// отрисовываем буфер двумя кусками
	if (1)
	{
		// верхняя часть целевого растра (начиная со scroll в видеобуфере терминала)
		colpip_bitblt(
				(uintptr_t) tfb, GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORPIP_T),
				tfb, DIM_X, DIM_Y, x, y + tgy1,
				(uintptr_t) vt->fb, GXSIZE(VTTYx2_DX, VTTYx2_DY) * sizeof (PACKEDCOLORPIP_T),	// параметры для clean
				colpip_mem_at(vt->fb, VTTYx2_DX, VTTYx2_DY, 0, tgh2),	// начальный адрес источника
				VTTYx2_DX, tgh1,	// размеры источника
				BITBLT_FLAG_NONE, 0);
	}
	if (1 && tgh2 != 0)
	{
		// нижняя часть
		colpip_bitblt(
				(uintptr_t) tfb, 1 * GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORPIP_T),
				tfb, DIM_X, DIM_Y, x, y + tgy2,
				(uintptr_t) vt->fb, 1 * GXSIZE(VTTYx2_DX, VTTYx2_DY) * sizeof (PACKEDCOLORPIP_T),	// параметры для clean
				colpip_mem_at(vt->fb, VTTYx2_DX, VTTYx2_DY, 0, 0),	// начальный адрес источника
				VTTYx2_DX, tgh2,// размеры источника
				BITBLT_FLAG_NONE, 0);
	}
}

// копирование растра в видеобуфер отображения
// с поворотом вправо на 90 градусов
void display_vtty_x2_show_ra90(
	uint_fast16_t x,
	uint_fast16_t y
	)
{
	PACKEDCOLORPIP_T * const tfb = colmain_fb_draw();
//	colmain_fillrect(tfb, DIM_X, DIM_Y, x, y, VTTYx2_DX, VTTYx2_DY, VTTYx2_BG);	// обозначам место под вывод информации
//	return;
	vtty_x2_t * const vt = & vtty_x2_0;
	enum { H = VTTYx2_ROWSPIX };
	// координаты верхней части в целевом видеобуфере
	const uint_fast16_t tgh1 = (VTTYx2_ROWS - vt->scroll) * H;	// высота
	const uint_fast16_t tgy1 = 0;
	// координаты нижней части в целевом видеобуфере
	const uint_fast16_t tgh2 = vt->scroll * H;	// высота
	const uint_fast16_t tgy2 = tgh1;

	// отрисовываем буфер двумя кусками
	if (1)
	{
		// верхняя часть - правее в выхоном растре
		// верхняя часть целевого растра (начиная со scroll в видеобуфере терминала)
		colpip_bitblt_ra90(
				(uintptr_t) tfb, GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORPIP_T),
				tfb, DIM_X, DIM_Y, x + tgh2, y,
				(uintptr_t) vt->fb, GXSIZE(VTTYx2_DX, VTTYx2_DY) * sizeof (PACKEDCOLORPIP_T),	// параметры для clean
				colpip_mem_at(vt->fb, VTTYx2_DX, VTTYx2_DY, 0, tgh2),	// начальный адрес источника
				VTTYx2_DX, tgh1);	// размеры источника
	}
	if (1 && tgh2 != 0)
	{
		// нижняя часть - левее в выхоном растре
		colpip_bitblt_ra90(
				(uintptr_t) tfb, 1 * GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORPIP_T),
				tfb, DIM_X, DIM_Y, x, y,
				(uintptr_t) vt->fb, 1 * GXSIZE(VTTYx2_DX, VTTYx2_DY) * sizeof (PACKEDCOLORPIP_T),	// параметры для clean
				colpip_mem_at(vt->fb, VTTYx2_DX, VTTYx2_DY, 0, 0),	// начальный адрес источника
				VTTYx2_DX, tgh2);	// размеры источника
	}
}

static void display_vtty_x2_scrollup(
	vtty_x2_t * vt,
	int nlines
	)
{
	enum { H = VTTYx2_ROWSPIX };
	//TP();
	// перемещаем начало.
	vt->scroll = (vt->scroll + nlines) % VTTYx2_ROWS;
	const unsigned row = (VTTYx2_ROWS - 1 + vt->scroll) % VTTYx2_ROWS;
	// очищаем видеобуфер
	colmain_fillrect(
			vt->fb, VTTYx2_DX, VTTYx2_DY,
			0, row * H,
			VTTYx2_DX, nlines * H,
			gbg);
	colmain_fillrect(vt->fgshadow, VTTYx2_COLS, VTTYx2_ROWS, 0, row, VTTYx2_COLS, nlines, gfg);	// очищаем видеобуфер
	colmain_fillrect(vt->bgshadow, VTTYx2_COLS, VTTYx2_ROWS, 0, row, VTTYx2_COLS, nlines, gbg);	// очищаем видеобуфер
	memset(vt->shadow [row], ' ', sizeof vt->shadow [0] * nlines);
	ASSERT(vt->scroll < VTTYx2_ROWS);
	ASSERT(vt->row < VTTYx2_ROWS);
	ASSERT(vt->col < VTTYx2_COLS);
}

// https://dencode.com/string/hex

// синхронное сохранение символа в буфер и в виде растра
static void vtput(vtty_x2_t * const vt, unsigned col, unsigned row, char ch, COLORPIP_T fg, COLORPIP_T bg)
{
	const unsigned vpos = (row + vt->scroll) % VTTYx2_ROWS;

	colmain_fillrect(
			vt->fb, VTTYx2_DX, VTTYx2_DY,
			col * VTTYx2_CHARPIX, vpos * VTTYx2_ROWSPIX,
			VTTYx2_CHARPIX, VTTYx2_ROWSPIX, bg);	// очищаем видеобуфер под выыодимыи символом
#if DIM_X == 720
	colpip_text_x2(
			vt->fb, VTTYx2_DX, VTTYx2_DY,
			col * VTTYx2_CHARPIX, vpos * VTTYx2_ROWSPIX,
			fg, & ch, 1);
#else
	colpip_text(
			vt->fb, VTTYx2_DX, VTTYx2_DY,
			col * VTTYx2_CHARPIX, vpos * VTTYx2_ROWSPIX,
			fg, & ch, 1);
#endif
	colmain_putpixel(vt->fgshadow, VTTYx2_COLS, VTTYx2_ROWS, col, vpos, fg);	// сохраняем цвет символа
	colmain_putpixel(vt->bgshadow, VTTYx2_COLS, VTTYx2_ROWS, col, vpos, bg);	// сохраняем цвет фона
	vt->shadow [vpos] [col] = ch;
}

static void display_vtty_x2_cout(
	vtty_x2_t * const vt,
	char ch
	)
{
	ASSERT(vt->scroll < VTTYx2_ROWS);
	ASSERT(vt->row < VTTYx2_ROWS);
	ASSERT(vt->col < VTTYx2_COLS);
	switch (ch)
	{
	case '\r':
		vt->col = 0;
		break;
	case '\n':
		if (++ vt->row >= VTTYx2_ROWS)
		{
			vt->row = VTTYx2_ROWS - 1;
			display_vtty_x2_scrollup(vt, 1);
		}
		break;

	default:
		{
			vtput(vt, vt->col, vt->row, ch, gfg, gbg);
			if (++ vt->col >= VTTYx2_COLS)
			{
				vt->col = 0;
				if (++ vt->row >= VTTYx2_ROWS)
				{
					if (hasscroll)
					{
						vt->row = VTTYx2_ROWS - 1;
						display_vtty_x2_scrollup(vt, 1);
					}
					else
					{
						vt->row = 0;
					}
				}
			}
		}
		break;
	}
	ASSERT(vt->scroll < VTTYx2_ROWS);
	ASSERT(vt->row < VTTYx2_ROWS);
	ASSERT(vt->col < VTTYx2_COLS);
}

int display_vtty_x2_putchar(char ch)
{
	vtty_x2_t * const vt = & vtty_x2_0;
	if (ch == '\n')
		display_vtty_x2_putchar('\r');

	display_vtty_x2_cout(vt, ch);
	return (unsigned char) ch;
}

void display_vtty_x2_printf(const char * format, ...)
{
	char b [128];	// see stack sizes for interrupt handlers
	va_list	ap;
	va_start(ap, format);
	const int n = local_vsnprintf_P(b, sizeof b / sizeof b [0], format, ap);
	va_end(ap);
	for (int i = 0; i < n; ++ i)
		display_vtty_x2_putchar(b [i]);
}

int display_vtty_x2_maxx(void);
int display_vtty_x2_maxy(void);

int display_vtty_x2_maxx(void)
{
	return VTTYx2_COLS;
}

int display_vtty_x2_maxy(void)
{
	return VTTYx2_ROWS;
}

struct text_info {                      /* текущие условия отображения */
    char    winleft;                    /* левая координата окна */
    char    wintop;                     /* верхняя координата окна */
    char    winright;                   /* правая координата окна */
    char    winbottom;                  /* нижняя координата окна */
    char    attribute;                  /* цвет фона и цвет символа */
    char    normattr;                   /* = 0х0f фон BLACK и цвет WHITE */
    char    currmode;                   /* текущий видеорежим */
    char    screenheight;               /* вертикальный размер экрана */
    char    screenwidth;                /* горизонтальный размер экрана */
    char    curx;                       /* горизонтальная позиция курсора */
    char    cury;                       /* вертикальная позиция курсора */
};

void    gettextinfo (struct text_info *r)
{
	vtty_x2_t * const vt = & vtty_x2_0;

	r->attribute = gattr;
	r->currmode = 0;
	r->curx = vt->col - gwinleft + 2;
	r->cury = vt->row - gwintop + 2;
	//r->normattr = 0x0F;
	r->screenheight = display_vtty_x2_maxy();
	r->screenwidth = display_vtty_x2_maxx();

	r->wintop = gwintop;
	r->winleft = gwinleft;
	r->winright = gwinright;
	r->winbottom = gwinbottom;
}

static uint_fast8_t color2attr(COLORPIP_T color)
{
	switch (color)
	{
	//case COLORMAIN_GRAY: return 7;
	default:
	case COLORMAIN_WHITE: return 15;
	case COLORMAIN_BLACK: return 0;
	case COLORMAIN_RED: return 4;
	case COLORMAIN_GREEN: return 2;
	case COLORMAIN_BLUE: return 1;
	}
}

static COLORPIP_T attr2color(uint_fast8_t v)
{
	switch (v)
	{
	//case 7: return COLORMAIN_GRAY;
	default:
	case 15: return COLORMAIN_WHITE;
	case 0: return COLORMAIN_BLACK;
	case 4: return COLORMAIN_RED;
	case 2: return COLORMAIN_GREEN;
	case 1: return COLORMAIN_BLUE;
	}
}

// координаиы глобальные
void ex_gettext(int left, int top, int right, int bottom, void *destin)
{
	vtty_x2_t * const vt = & vtty_x2_0;
	uint8_t * p = destin;
	unsigned row = top - 1;
	unsigned h = bottom - top + 1;
	while (h --)
	{
		const unsigned v = (row + vt->scroll) % VTTYx2_ROWS;
		unsigned col = left - 1;
		unsigned w = right - left + 1;
		while (w --)
		{
			* p ++ = vt->shadow [v] [col];
			* p ++ =
					(color2attr(* colpip_mem_at(vt->fgshadow, VTTYx2_COLS, VTTYx2_ROWS, col, v)) << AFGPOS) +
					(color2attr(* colpip_mem_at(vt->bgshadow, VTTYx2_COLS, VTTYx2_ROWS, col, v)) << ABGPOS);
			++ col;
		}
		++ row;
	}
}

// координаиы глобальные
void puttext(int left, int top, int right, int bottom, void *source)
{
	vtty_x2_t * const vt = & vtty_x2_0;
	uint8_t * p = source;
	unsigned row = top - 1;
	unsigned h = bottom - top + 1;
	while (h --)
	{
		unsigned col = left - 1;
		unsigned w = right - left + 1;
		while (w --)
		{
			const uint_fast8_t ch = * p ++;
			const uint_fast8_t att = * p ++;
			vtput(vt, col, row, ch, attr2color((att >> AFGPOS) & 0x0F), attr2color((att >> ABGPOS) & 0x0F));
			++ col;
		}
		++ row;
	}
}

void    gotoxy(int x, int y)
{
	display_vtty_x2_gotoxy(x + gwinleft - 2, y + gwintop - 2);
}


void    clrscr(void)
{
	vtty_x2_t * const vt = & vtty_x2_0;
	unsigned row = gwintop - 1;
	unsigned h = gwinbottom - gwintop + 1;
	while (h --)
	{
		unsigned col = gwinleft - 1;
		unsigned w = gwinright - gwinleft + 1;
		while (w --)
		{
			vtput(vt, col ++, row, ' ', gfg, gbg);
		}
		++ row;
	}
	gotoxy(1, 1);
}

void    textattr (int newattr)
{
	gattr = newattr;
	gfg = attr2color((newattr >> AFGPOS) & 0x0F);
	gbg = attr2color((newattr >> ABGPOS) & 0x0F);
}

/*
 * Функция window() используется для создания прямоугольного текстового окна,
 * имеющего ле­вый верхний угол и правый нижний угол в точках с координатами left, top и right, bottom соответ­ственно.
 * Если какая-либо из координат недействительна, то функция window() не выполняет ни­каких действий.
 * После успешного вызова функции window() все ссылки на координаты местоположения интерпретируются относительно окна, а не экрана.
 *
 */
void    window(int left, int top, int right, int bottom)
{
	gwinleft = left;
	gwintop = top;
	gwinright = right;
	gwinbottom = bottom;
	gotoxy(1, 1);
}

void    _setcursortype (int cur_t)
{

}

int     putch (int c)
{
	vtty_x2_t * const vt = & vtty_x2_0;
	display_vtty_x2_cout(vt, c);
	return c;
}

int     cputs (const char *str)
{
	vtty_x2_t * const vt = & vtty_x2_0;
	int c;
	while ((c = * str ++) != '\0')
		display_vtty_x2_cout(vt, c);
	return 0;
}

#endif /* ! LCDMODE_DUMMY && LCDMODE_LTDC */
