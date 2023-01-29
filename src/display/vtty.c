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
	enum { VTTY_ROWS = (DIM_Y - GRID2Y(5)) / VTTY_ROWSPIX};
#endif
enum { VTTY_DX = VTTY_COLS * VTTY_CHARPIX, VTTY_DY = VTTY_ROWS * VTTY_ROWSPIX };


// цвета отрисовки
#define VTTY_FG COLORMAIN_GREEN
#define VTTY_BG COLORMAIN_BLACK

typedef struct vtty_tag
{
	ALIGNX_BEGIN PACKEDCOLORPIP_T fb [GXSIZE(VTTY_DX, VTTY_DY)] ALIGNX_END;
	unsigned scroll;	// эта строка отображается верхней в целевом прямоугольнике. 0..VTTY_ROWS-1
	unsigned row;		// 0..VTTY_ROWS-1
	unsigned col;		// 0..VTTY_COLS-1
} vtty_t;

static uint_fast8_t debugvtty_qget(uint_fast8_t * pc);
static void display_vtty_cout(
	vtty_t * const vt,
	char ch
	);

static volatile int vtty_inited;

static RAMFRAMEBUFF ALIGNX_BEGIN vtty_t vtty0 ALIGNX_END;

int display_vtty_putchar(char ch);
void display_vtty_printf(const char * format, ...);
void display_vtty_printf_irq(const char * format, ...);
void display2_vtty(
	uint_fast8_t x0,
	uint_fast8_t y0,
	dctx_t * pctx
	);
void display_vtty_clrscr(void);
void display_vtty_gotoxy(unsigned x, unsigned y);

int display_vtty_maxx(void);
int display_vtty_maxy(void);

static void display_vtty_initialize(void)
{
	vtty_t * const vt = & vtty0;
	vt->scroll = 0;
	vt->row = 0;
	vt->col = 0;
	colpip_fillrect(vt->fb, VTTY_DX, VTTY_DY, 0, 0, VTTY_DX, VTTY_DY, VTTY_BG);	// очищаем видеобуфер
	PRINTF("display_vtty_initialize: rows=%u, cols=%u\n", VTTY_ROWS, VTTY_COLS);
	vtty_inited = 1;
	display_vtty_printf("display_vtty_initialize: rows=%u, cols=%u\n", VTTY_ROWS, VTTY_COLS);
}

void display_vtty_clrscr(void)
{
	vtty_t * const vt = & vtty0;

	vt->scroll = 0;
	vt->row = 0;
	vt->col = 0;

	colpip_fillrect(vt->fb, VTTY_DX, VTTY_DY, 0, 0, VTTY_DX, VTTY_DY, VTTY_BG);	// очищаем видеобуфер
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
static void display_vtty_show(
	uint_fast16_t x,
	uint_fast16_t y
	)
{
	PACKEDCOLORPIP_T * const tfb = colmain_fb_draw();
//	colpip_fillrect(tfb, DIM_X, DIM_Y, x, y, VTTY_DX, VTTY_DY, VTTY_BG);	// обозначам место под вывод информации
//	return;
	vtty_t * const vt = & vtty0;
	enum { H = VTTY_ROWSPIX };
	// координаты верхней части в целевом видеобуфере
	const uint_fast16_t tgh1 = (VTTY_ROWS - vt->scroll) * H;	// высота
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
				(uintptr_t) vt->fb, GXSIZE(VTTY_DX, VTTY_DY) * sizeof (PACKEDCOLORPIP_T),	// параметры для clean
				colpip_mem_at(vt->fb, VTTY_DX, VTTY_DY, 0, tgh2),	// начальный адрес источника
				VTTY_DX, tgh1,	// размеры источника
				BITBLT_FLAG_NONE, 0);
	}
	if (1 && tgh2 != 0)
	{
		// нижняя часть
		colpip_bitblt(
				(uintptr_t) tfb, 1 * GXSIZE(DIM_X, DIM_Y) * sizeof (PACKEDCOLORPIP_T),
				tfb, DIM_X, DIM_Y, x, y + tgy2,
				(uintptr_t) vt->fb, 1 * GXSIZE(VTTY_DX, VTTY_DY) * sizeof (PACKEDCOLORPIP_T),	// параметры для clean
				colpip_mem_at(vt->fb, VTTY_DX, VTTY_DY, 0, 0),	// начальный адрес источника
				VTTY_DX, tgh2,	// размеры источника
				BITBLT_FLAG_NONE, 0);
	}
}

void display2_vtty(
	uint_fast8_t x0,
	uint_fast8_t y0,
	dctx_t * pctx
	)
{
	const uint_fast16_t x = GRID2X(x0);
	const uint_fast16_t y = GRID2Y(y0);

	for (;;)
	{
		vtty_t * const vt = & vtty0;
		uint_fast8_t f;
		uint_fast8_t c;
		system_disableIRQ();
		f = debugvtty_qget(& c);
		system_enableIRQ();
		if (f == 0)
			break;
		display_vtty_cout(vt, c);

	}
	display_vtty_show(x, y);
}

void display2_vtty_init(
	uint_fast8_t x0,
	uint_fast8_t y0,
	dctx_t * pctx
	)
{
	display_vtty_initialize();
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
	colpip_fillrect(
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
			colpip_text(
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
	if (! vtty_inited)
		return (unsigned char) ch;
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


// Очереди символов для печати из прерываний
enum { qSZdevice = 8192 };

static uint8_t debugvtty_queue [qSZdevice];
static unsigned debugvtty_qp, debugvtty_qg;

// Передать символ в host
static uint_fast8_t	debugvtty_qput(uint_fast8_t c)
{
	unsigned qpt = debugvtty_qp;
	const unsigned next = (qpt + 1) % qSZdevice;
	if (next != debugvtty_qg)
	{
		debugvtty_queue [qpt] = c;
		debugvtty_qp = next;
		return 1;
	}
	return 0;
}

// Получить символ в host
static uint_fast8_t debugvtty_qget(uint_fast8_t * pc)
{
	if (debugvtty_qp != debugvtty_qg)
	{
		* pc = debugvtty_queue [debugvtty_qg];
		debugvtty_qg = (debugvtty_qg + 1) % qSZdevice;
		return 1;
	}
	return 0;
}

// получить состояние очереди передачи
static uint_fast8_t debugvtty_qempty(void)
{
	return debugvtty_qp == debugvtty_qg;
}

int display_vtty_putchar_irq(char ch)
{
	if (! vtty_inited)
		return (unsigned char) ch;
	vtty_t * const vt = & vtty0;
	if (ch == '\n')
		display_vtty_putchar_irq('\r');

	debugvtty_qput(ch);
	return (unsigned char) ch;
}

void display_vtty_printf_irq(const char * format, ...)
{
	char b [128];	// see stack sizes for interrupt handlers
	va_list	ap;
	va_start(ap, format);
	const int n = local_vsnprintf_P(b, sizeof b / sizeof b [0], format, ap);
	va_end(ap);
	for (int i = 0; i < n; ++ i)
		display_vtty_putchar_irq(b [i]);
}

int display_vtty_maxx(void)
{
	return VTTY_COLS;
}

int display_vtty_maxy(void)
{
	return VTTY_ROWS;
}


static int
vtty_toprintc(int c)
{
	if (c < 0x20 || c >= 0x7f)
		return '.';
	return c;
}

void
vtty_printhex(unsigned long voffs, const unsigned char * buff, unsigned length)
{
	enum { ROWSIZE = 12 };
	unsigned row;
	const unsigned rows = (length + (ROWSIZE - 1)) / ROWSIZE;

	for (row = 0; row < rows; ++ row)
	{
		unsigned j;
		const int remaining = length - row * ROWSIZE;
		const int trl = (ROWSIZE < remaining) ? ROWSIZE : remaining;
		display_vtty_printf(PSTR("%04lX "), voffs + row * ROWSIZE);
		for (j = 0; j < trl; ++ j)
			display_vtty_printf(PSTR(" %02X"), buff [row * ROWSIZE + j]);

		display_vtty_printf(PSTR("%*s"), (ROWSIZE - trl) * 3, "");

		display_vtty_printf(PSTR(" "));
		for (j = 0; j < trl; ++ j)
			display_vtty_printf(PSTR("%c"), vtty_toprintc(buff [row * ROWSIZE + j]));

		display_vtty_printf(PSTR("\n"));
	}
}

void
vtty_printhex_irqsystem(unsigned long voffs, const unsigned char * buff, unsigned length)
{
	enum { ROWSIZE = 12 };
	unsigned row;
	const unsigned rows = (length + (ROWSIZE - 1)) / ROWSIZE;

	for (row = 0; row < rows; ++ row)
	{
		unsigned j;
		const int remaining = length - row * ROWSIZE;
		const int trl = (ROWSIZE < remaining) ? ROWSIZE : remaining;
		display_vtty_printf_irq(PSTR("%04lX "), voffs + row * ROWSIZE);
		for (j = 0; j < trl; ++ j)
			display_vtty_printf_irq(PSTR(" %02X"), buff [row * ROWSIZE + j]);

		display_vtty_printf_irq(PSTR("%*s"), (ROWSIZE - trl) * 3, "");

		display_vtty_printf_irq(PSTR(" "));
		for (j = 0; j < trl; ++ j)
			display_vtty_printf_irq(PSTR("%c"), vtty_toprintc(buff [row * ROWSIZE + j]));

		display_vtty_printf_irq(PSTR("\n"));
	}
}


#endif /* ! LCDMODE_DUMMY */
