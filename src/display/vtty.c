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

#include "display.h"
#include "display2.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include "formats.h"
#include "fontmaps.h"

enum { VTTY_COLS = 64 };
enum { VTTY_ROWS = 24 };
enum { VTTY_CHARPIX = SMALLCHARW };	// количество пикселей по горизонтали на один символ текста
enum { VTTY_ROWSPIX = SMALLCHARH + 5 };	// количество пикселей по вертикали на одну строку текста
enum { VTTY_DX = VTTY_COLS * VTTY_CHARPIX, VTTY_DY = VTTY_ROWS * VTTY_ROWSPIX };


// цвета отрисовки
#define VTTY_FG COLORMAIN_GREEN
#define VTTY_BG COLORMAIN_BLACK

typedef struct vtty_tag
{
#if LCDMODE_LTDCSDRAMBUFF
	PACKEDCOLORMAIN_T * fb;
#else
	PACKEDCOLORMAIN_T fb [GXSIZE(VTTY_DX, VTTY_DY)];
#endif
	unsigned scroll;	// эта строка отображается верхней в целевом прямоугольнике. 0..VTTY_ROWS-1
	unsigned row;		// 0..VTTY_ROWS-1
	unsigned col;		// 0..VTTY_COLS-1
} vtty_t;

static vtty_t vtty0;

void display_vtty_initialize(void);
int display_vtty_putchar(char ch);
// копирование растра в видеобуфер отображения
void display_vtty_show(
	uint_fast16_t x,
	uint_fast16_t y
	);
void display_vtty_printf(const char * format, ...);

void display_vtty_initialize(void)
{
	vtty_t * const vt = & vtty0;
#if LCDMODE_LTDCSDRAMBUFF
	vt->fb = (PACKEDCOLORMAIN_T *) (SDRAM_BANK_ADDR + 24uL * 1024 * 1024);
#endif
	vt->scroll = 0;
	vt->row = 0;
	vt->col = 0;

	colmain_fillrect(vt->fb, VTTY_DX, VTTY_DY, 0, 0, VTTY_DX, VTTY_DY, VTTY_BG);	// очищаем видеобуфер
}

// копирование растра в видеобуфер отображения
void display_vtty_show(
	uint_fast16_t x,
	uint_fast16_t y
	)
{
	PACKEDCOLORMAIN_T * const tfb = colmain_fb_draw();
	vtty_t * const vt = & vtty0;
	enum { H = VTTY_ROWSPIX };
	// координаты верхней части в целевом виждеобуяере
	const uint_fast16_t tgy1 = 0;
	const uint_fast16_t tgh1 = (VTTY_ROWS - vt->scroll) * H;
	// координаты нижней части в целевом виждеобуяере
	const uint_fast16_t tgh2 = vt->scroll * H;
	const uint_fast16_t tgy2 = tgh1;
	// отрисовываем буфер двумя кусками
	if (1)
	{
		// верхняя часть целевого растра (начиная со scroll в видеобуфере терминала)
		colpip_plot(
				(uintptr_t) tfb, GXSIZE(DIM_X, DIM_Y),
				tfb, DIM_X, DIM_Y, x, y + tgy1,
				(uintptr_t) vt->fb, GXSIZE(VTTY_DX, VTTY_DY),	// папаметры для clean
				colmain_mem_at(vt->fb, VTTY_DX, VTTY_DY, 0, tgh2),	// начальный адрес источника
				VTTY_DX, tgh1);	// размеры источника
	}
	if (tgh2 != 0)
	{
		// нижняя часть
		colpip_plot(
				(uintptr_t) tfb, 0 * GXSIZE(DIM_X, DIM_Y),
				tfb, DIM_X, DIM_Y, x, y + tgy2,
				(uintptr_t) vt->fb, 0 * GXSIZE(VTTY_DX, VTTY_DY),	// папаметры для clean
				colmain_mem_at(vt->fb, VTTY_DX, VTTY_DY, 0, tgh1),	// начальный адрес источника
				VTTY_DX, tgh2);	// размеры источника
	}
}

static void display_vtty_scrollup(
	vtty_t * vt,
	int nlines
	)
{
	// очищаем видеобуфер
	colmain_fillrect(
			vt->fb, VTTY_DX, VTTY_DY,
			0, (vt->row + vt->scroll) % VTTY_ROWS * VTTY_ROWSPIX,
			VTTY_DX, nlines * VTTY_ROWSPIX,
			VTTY_BG);
	// перемещаем начало.
	vt->scroll = (vt->scroll + nlines) % VTTY_ROWS;
}

static void display_vtty_cout(
	vtty_t * const vt,
	char ch)
{
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
			char str [2] = { ch, '\0' };
			colpip_string_tbg(
					vt->fb, VTTY_DX, VTTY_DY,
					vt->col * VTTY_CHARPIX, (vt->row + vt->scroll) % VTTY_ROWS * VTTY_ROWSPIX,
					str, VTTY_FG);

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
	for (int i = 0; i < n; ++ i)
		display_vtty_putchar(b [i]);
	va_end(ap);
}
