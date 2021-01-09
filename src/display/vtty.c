/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// STM32xxx LCD-TFT Controller (LTDC)
// RENESAS Video Display Controller 5
//	Video Display Controller 5 (5): Image Synthesizer
//	Video Display Controller 5 (7): Output Controller

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

enum { VTTY_CHARPIX = SMALLCHARW };	// количество пикселей по горизонтали на один символ текста
enum { VTTY_ROWSPIX = SMALLCHARH + 2 };	// количество пикселей по вертикали на одну строку текста
#if DIM_X == 720
	enum { VTTY_COLS = (DIM_X - 16) / VTTY_CHARPIX };
	enum { VTTY_ROWS = 41 /*(DIM_Y - 20) / VTTY_ROWSPIX */};
#else
	enum { VTTY_COLS = (DIM_X - 0) / VTTY_CHARPIX };
	enum { VTTY_ROWS = (DIM_Y - 20) / VTTY_ROWSPIX};
#endif
enum { VTTY_DX = VTTY_COLS * VTTY_CHARPIX, VTTY_DY = VTTY_ROWS * VTTY_ROWSPIX };


// цвета отрисовки
#define VTTY_FG COLORMAIN_GREEN
#define VTTY_BG COLORMAIN_BLACK

typedef struct vtty_tag
{
	ALIGNX_BEGIN PACKEDCOLORMAIN_T fb [GXSIZE(VTTY_DX, VTTY_DY)] ALIGNX_END;
	unsigned scroll;	// эта строка отображается верхней в целевом прямоугольнике. 0..VTTY_ROWS-1
	unsigned row;		// 0..VTTY_ROWS-1
	unsigned col;		// 0..VTTY_COLS-1
} vtty_t;

static RAMFRAMEBUFF ALIGNX_BEGIN vtty_t vtty0 ALIGNX_END;

void display_vtty_initialize(void);
int display_vtty_putchar(char ch);
// копирование растра в видеобуфер отображения
void display_vtty_show(
	uint_fast16_t x,
	uint_fast16_t y
	);
void display_vtty_printf(const char * format, ...);
void display2_vtty(
	uint_fast8_t x0,
	uint_fast8_t y0,
	dctx_t * pctx
	);
void display_vtty_clrscr(void);
void display_vtty_gotoxy(unsigned x, unsigned y);

int display_vtty_maxx(void);
int display_vtty_maxy(void);

void display_vtty_initialize(void)
{
	vtty_t * const vt = & vtty0;
	vt->scroll = 0;
	vt->row = 0;
	vt->col = 0;
	colmain_fillrect(vt->fb, VTTY_DX, VTTY_DY, 0, 0, VTTY_DX, VTTY_DY, VTTY_BG);	// очищаем видеобуфер
	PRINTF("display_vtty_initialize: rows=%u, cols=%u\n", VTTY_ROWS, VTTY_COLS);
}

void display_vtty_clrscr(void)
{
	vtty_t * const vt = & vtty0;

	vt->scroll = 0;
	vt->row = 0;
	vt->col = 0;

	colmain_fillrect(vt->fb, VTTY_DX, VTTY_DY, 0, 0, VTTY_DX, VTTY_DY, VTTY_BG);	// очищаем видеобуфер
}

void display_vtty_gotoxy(unsigned x, unsigned y)
{
	vtty_t * const vt = & vtty0;
	if (x >= VTTY_COLS)
		return;
	if (y >= VTTY_ROWS)
		return;
	vt->row = y;
	vt->col = x;
	ASSERT(vt->scroll < VTTY_ROWS);
	ASSERT(vt->row < VTTY_ROWS);
	ASSERT(vt->col < VTTY_COLS);
}
// копирование растра в видеобуфер отображения
void display_vtty_show(
	uint_fast16_t x,
	uint_fast16_t y
	)
{
	PACKEDCOLORMAIN_T * const tfb = colmain_fb_draw();
//	colmain_fillrect(tfb, DIM_X, DIM_Y, x, y, VTTY_DX, VTTY_DY, VTTY_BG);	// обозначам место под вывод информации
//	return;
	vtty_t * const vt = & vtty0;
	enum { H = VTTY_ROWSPIX };
	// координаты верхней части в целевом видеобуфере
	const uint_fast16_t tgh1 = (VTTY_ROWS - vt->scroll) * H;	// высота
	const uint_fast16_t tgy1 = 0;
	// координаты нижней части в целевом видеобуфере
	const uint_fast16_t tgh2 = vt->scroll * H;	// высота
	const uint_fast16_t tgy2 = tgh1;

	// отрисовываем буфер двумя кусками
	if (1)
	{
		// верхняя часть целевого растра (начиная со scroll в видеобуфере терминала)
		colpip_plot(
				(uintptr_t) tfb, GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORMAIN_T),
				tfb, DIM_X, DIM_Y, x, y + tgy1,
				(uintptr_t) vt->fb, GXSIZE(VTTY_DX, VTTY_DY) * sizeof (PACKEDCOLORMAIN_T),	// параметры для clean
				colmain_mem_at(vt->fb, VTTY_DX, VTTY_DY, 0, tgh2),	// начальный адрес источника
				VTTY_DX, tgh1);	// размеры источника
	}
	if (1 && tgh2 != 0)
	{
		// нижняя часть
		colpip_plot(
				(uintptr_t) tfb, 1 * GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORMAIN_T),
				tfb, DIM_X, DIM_Y, x, y + tgy2,
				(uintptr_t) vt->fb, 1 * GXSIZE(VTTY_DX, VTTY_DY) * sizeof (PACKEDCOLORMAIN_T),	// параметры для clean
				colmain_mem_at(vt->fb, VTTY_DX, VTTY_DY, 0, 0),	// начальный адрес источника
				VTTY_DX, tgh2);	// размеры источника
	}
}

void display2_vtty(
	uint_fast8_t x0,
	uint_fast8_t y0,
	dctx_t * pctx
	)
{
	uint_fast16_t x = GRID2X(x0);
	uint_fast16_t y = GRID2Y(y0);

	display_vtty_show(x, y);
}

static void display_vtty_scrollup(
	vtty_t * vt,
	int nlines
	)
{
	enum { H = VTTY_ROWSPIX };
	//TP();
	// перемещаем начало.
	vt->scroll = (vt->scroll + nlines) % VTTY_ROWS;
	// очищаем видеобуфер
	colmain_fillrect(
			vt->fb, VTTY_DX, VTTY_DY,
			0, (VTTY_ROWS - 1 + vt->scroll) % VTTY_ROWS * H,
			VTTY_DX, nlines * H,
			VTTY_BG);
	ASSERT(vt->scroll < VTTY_ROWS);
	ASSERT(vt->row < VTTY_ROWS);
	ASSERT(vt->col < VTTY_COLS);
}

static void display_vtty_cout(
	vtty_t * const vt,
	char ch
	)
{
	ASSERT(vt->scroll < VTTY_ROWS);
	ASSERT(vt->row < VTTY_ROWS);
	ASSERT(vt->col < VTTY_COLS);
	switch (ch)
	{
	case '\r':
		vt->col = 0;
		break;
	case '\n':
		if (++ vt->row >= VTTY_ROWS)
		{
			vt->row = VTTY_ROWS - 1;
			display_vtty_scrollup(vt, 1);
		}
		break;

	default:
		{
			colpip_string_count(
					vt->fb, VTTY_DX, VTTY_DY,
					vt->col * VTTY_CHARPIX, (vt->row + vt->scroll) % VTTY_ROWS * VTTY_ROWSPIX,
					VTTY_FG, & ch, 1);

			if (++ vt->col >= VTTY_COLS)
			{
				vt->col = 0;
				if (++ vt->row >= VTTY_ROWS)
				{
					vt->row = VTTY_ROWS - 1;
					display_vtty_scrollup(vt, 1);
				}
			}
		}
		break;
	}
	ASSERT(vt->scroll < VTTY_ROWS);
	ASSERT(vt->row < VTTY_ROWS);
	ASSERT(vt->col < VTTY_COLS);
}

int display_vtty_putchar(char ch)
{
	vtty_t * const vt = & vtty0;
	if (ch == '\n')
		display_vtty_putchar('\r');

	display_vtty_cout(vt, ch);
	return (unsigned char) ch;
}

void display_vtty_printf(const char * format, ...)
{
	char b [128];	// see stack sizes for interrupt handlers
	va_list	ap;
	va_start(ap, format);
	const int n = local_vsnprintf_P(b, sizeof b / sizeof b [0], format, ap);
	va_end(ap);
	for (int i = 0; i < n; ++ i)
		display_vtty_putchar(b [i]);
}

int display_vtty_maxx(void)
{
	return VTTY_COLS;
}

int display_vtty_maxy(void)
{
	return VTTY_ROWS;
}

#endif /* ! LCDMODE_DUMMY */
