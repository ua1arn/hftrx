/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "display.h"
#include <stdint.h>

#include "./display.h"
#include "./formats.h"

#if LCDMODE_UC1601

#if ! defined (WITHTWIHW) && ! defined (WITHTWISW)
	#error WITHTWIHW or WITHTWISW should be defined
#endif

#include "uc1601s.h"

/* растровые шрифты */
#include "fontmaps.h"


#define LCD_ADDR_W 0x70	// destination address
#define LCD_ADDR_R (LCD_ADDR_W | 0x01)	// destination address


static void 
//NOINLINEAT
display_lcd_reset(uint_fast8_t v)
{
	board_lcd_reset(v); 	// Pull RST pin up/down
	board_update();
	local_delay_ms(10);		// delay 10 mS
}

static void
uc1601s_reset(void)
{
	local_delay_ms(5);		// delay 5 mS
	display_lcd_reset(0); 	// Pull RST pin down
	display_lcd_reset(1); 	// Pull RST pin up
}


// начало выдаче байтов (записи в видеопамять)
// Вызывается в начале выдачи строки
static void 
//NOINLINEAT
uc1601s_put_char_begin(void)
{
    i2c_start(LCD_ADDR_W | 0x02);		// 
}

static void 
NOINLINEAT
uc1601s_put_char_end(void)
{
	i2c_waitsend();
	i2c_stop();
}





/*
 Функция установки курсора в позицию x,y
 X - координата по горизонтали в пределах 9..127 .0-132,
 Y - координата по вертикали (строка, Page) в пределах 0-7
*/ 
static void 
//NOINLINEAT
uc1601s_set_addr_column(uint_fast8_t x, uint_fast8_t y)		// 
{

	i2c_start(LCD_ADDR_W);
    i2c_write(0xb0 | y);	// Set Y (Page Address)
    i2c_write(x & 0x0f);	// Set X (Column Address            0b0000 x4 x3 x2 x1) 0..131
    i2c_write((x >> 4) | 0x10);	// Set X (Column Address    0b0001 x7 x6 x5 x4)
	i2c_waitsend();
    i2c_stop();

}




static void 
//NOINLINEAT
uc1601s_clear(void)
{
    uint_fast8_t cnt_y;

	for (cnt_y = 0; cnt_y < ((DIM_Y + 7) / 8); ++ cnt_y)
	{
	    uint_fast8_t cnt_x;

		display_gotoxy(0, cnt_y);

		i2c_start(LCD_ADDR_W | 0x02);

	    for (cnt_x = 0; cnt_x < DIM_X; ++ cnt_x)
		{
			i2c_write(0x00);
		}
		i2c_waitsend();
	    i2c_stop();
	}
}


// uc1601s_bigfont & uc1601s_halffont decode
static uint_fast8_t 
NOINLINEAT
bigfont_decode(uint_fast8_t c)
{
	// '#' - узкий пробел
	if (c == ' ' || c == '#')
		return 11;
	if (c == '_')
		return 10;		// курсор - позиция редактирвания частоты
	if (c == '.')
		return 12;		// точка
	return c - '0';		// остальные - цифры 0..9
}

static uint_fast8_t 
//NOINLINEAT
smallfont_decode(uint_fast8_t c)
{
	return c - ' ';
}


// Вызовы этой функции (или группу вызовов) требуется "обрамить" парой вызовов
// uc1601s_put_char_begin() и uc1601s_put_char_end().
//
static void 
//NOINLINEAT
uc1601s_put_char_small(char cc)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	enum { NCOLS = (sizeof uc1601s_font[0] / sizeof uc1601s_font[0][0]) };
	const FLASHMEM uint8_t * const p = & uc1601s_font[c][0];

	for (; i < NCOLS; ++ i)
    	i2c_write(p [i]);
}

// многополосный вывод символов - за несколько горизонтальных проходов.
// Нумерация полос - сверху вниз, начиная с 0

// Вызовы этой функции (или группу вызовов) требуется "обрамить" парой вызовов
// uc1601s_put_char_begin() и uc1601s_put_char_end().
//
static void 
//NOINLINEAT
uc1601s_put_char_big(char cc, uint_fast8_t lowhalf)
{
	// '#' - узкий пробел
	enum { NBV = (BIGCHARH / 8) }; // сколько байтов в одной вертикали
	uint_fast8_t i = 1 * ((cc == '.' || cc == '#') ? 6 : 0);	// начальная колонка знакогенератора, откуда начинать.
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NCOLS = (sizeof uc1601s_bigfont [0][0] / sizeof uc1601s_bigfont [0][0][0]) };
	const FLASHMEM uint8_t * const p = & uc1601s_bigfont [c][lowhalf][0];

	for (; i < NCOLS; ++ i)
    	i2c_write(p [i]);
}

// многополосный вывод символов - за несколько горизонтальных проходов.
// Нумерация полос - сверху вниз, начиная с 0

// Вызовы этой функции (или группу вызовов) требуется "обрамить" парой вызовов
// uc1601s_put_char_begin() и uc1601s_put_char_end().
//
static void 
//NOINLINEAT
uc1601s_put_char_half(char cc, uint_fast8_t lowhalf)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NCOLS = (sizeof uc1601s_halffont [c][lowhalf] / sizeof uc1601s_halffont [c][lowhalf] [0]) };
	const FLASHMEM uint8_t * const p = & uc1601s_halffont [c][lowhalf][0];

	for (; i < NCOLS; ++ i)
    	i2c_write(p [i]);

}

/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
static void 
//NOINLINEAT
uc1601s_bar_column(uint_fast8_t pattern)
{
	i2c_write(pattern);
}


static uint_fast8_t vbias = DEFAULT_LCD_CONTRAST;
//static uint_fast8_t bisaratio = 0xeb;	// 0xe8, 0xe9, 0xea, 0xeb - 6,7,8,9 bias ratios

static void 
//NOINLINEAT
uc1601s_set_contrast(uint_fast8_t v)
{
	if (vbias != v)
	{
		vbias = v;
		i2c_start(LCD_ADDR_W);
		//i2c_write(bisaratio);	// 0xe8, 0xe9, 0xea, 0xeb - 6,7,8,9 bias ratios
		i2c_write(0x81);  // Set Vbias Potentiometer UC1601
		i2c_write(vbias);
		i2c_waitsend();
		i2c_stop();
	}
}

#if 0

// Выключение дисплея (и включение его).
// TODO: перенести сюда управление питанием LCD
void uc1601s_disable(uint_fast8_t state)
{
	if (state == 0)
	{
		// Enable LCD
	}
	else
	{
		// Disable LCD
	}
}
#endif

void uc1601s_initialize(void)
{


	local_delay_ms(10);			// delay required after [power on] reset

#if 1
	i2c_start(LCD_ADDR_W);
	i2c_write(0xe2);		// system reset
	i2c_waitsend();
	i2c_stop();
#endif
	local_delay_ms(10);			// delay required after [power on] reset

#if LCDMODE_RDX0120
	// LC[1]: Mirror X
	// LC[2]: Mirror Y
	// В маленьклм индикаторе 64*32
	#if LCDMODE_UC1601S_TOPDOWN
		i2c_start(LCD_ADDR_W);
		i2c_write(0xc6);  // 
		i2c_waitsend();
		i2c_stop();
	#else /* LCDMODE_UC1601S_TOPDOWN */
		i2c_start(LCD_ADDR_W);
		i2c_write(0xc0);  // ok
		i2c_waitsend();
		i2c_stop();
	#endif /* LCDMODE_UC1601S_TOPDOWN */

#elif LCDMODE_RDX0077 || LCDMODE_RDT065 || LCDMODE_UC1601S_XMIRROR
	// В большом индикаторе 128 * 64
	#if LCDMODE_RDT065 || LCDMODE_UC1601S_TOPDOWN
		// перевёрнутое положение индикатора - чёрной зоной на дисплее с контактами вверх.
		i2c_start(LCD_ADDR_W);
		i2c_write(0xC6);  // Set LCD Mapping Control - выключаем отзеркаливание RDX0154
		i2c_waitsend();
		i2c_stop();
	#else /* LCDMODE_UC1601S_TOPDOWN */
		// нормальное положение индикатора - чёрной зоной на дисплее с контактами вниз.
		i2c_start(LCD_ADDR_W);
		i2c_write(0xC0);  //  Set LCD Mapping Control - включаем отзеркаливание RDX0077
		i2c_waitsend();
		i2c_stop();

		i2c_start(LCD_ADDR_W);
		i2c_write(0xEB);  // Set LCD bias ratio 1/6
		i2c_waitsend();
		i2c_stop();

	#endif /* LCDMODE_UC1601S_TOPDOWN */

#else /* LCDMODE_RDX0120 */

	// В большом индикаторе 132 * 65
	#if LCDMODE_UC1601S_TOPDOWN
		// перевёрнутое положение индикатора - чёрной зоной на дисплее с контактами вверх.
		i2c_start(LCD_ADDR_W);
		i2c_write(0xC4);  // Set LCD Mapping Control - выключаем отзеркаливание RDX0154
		i2c_waitsend();
		i2c_stop();
	#else /* LCDMODE_UC1601S_TOPDOWN */
		// нормальное положение индикатора - чёрной зоной на дисплее с контактами вниз.
		i2c_start(LCD_ADDR_W);
		i2c_write(0xC2);  // Set LCD Mapping Control - включаем отзеркаливание RDX0154
		i2c_waitsend();
		i2c_stop();
	#endif /* LCDMODE_UC1601S_TOPDOWN */

#endif /* LCDMODE_RDX0120 */

	i2c_start(LCD_ADDR_W);
	i2c_write(0x88);  // Set RAM Address Control
	i2c_waitsend();
	i2c_stop();

#if LCDMODE_RDX0120
	// cen = 0x3f, dst = 0, den = 0x3f
	// Set duty = 1/33, bias = 1/6
	i2c_start(LCD_ADDR_W);
	i2c_write(0xE8);  // Set LCD bias ratio 1/6
	i2c_waitsend();
	i2c_stop();




#elif LCDMODE_RDX0077

#else /* LCDMODE_RDX0120 */
#endif /* LCDMODE_RDX0120 */

	// 32*64
	// 128 * 64
	// 132 * 64
	i2c_start(LCD_ADDR_W);
	i2c_write(0xF1);  // CEN: Set COM end, default = 63
	i2c_write(DIM_Y - 1);

	i2c_write(0xF2);  // DST: Set display start
	i2c_write(0);
	i2c_write(0xF3);  // DEN: Set display end
	i2c_write(DIM_Y - 1);

	i2c_write(0x85);  // Enable Partial Display

	//i2c_write(0x84);  // Disable Partial Display
	i2c_waitsend();
	i2c_stop();

#if LCDMODE_UC1601S_EXTPOWER
	i2c_start(LCD_ADDR_W);
	i2c_write(0x28);		// Set Power Control - use external VLcd power supplay
	i2c_waitsend();
	i2c_stop();
#else
	i2c_start(LCD_ADDR_W);
	i2c_write(0x2e);		// Set Power Control - use internal charge pump
	i2c_waitsend();
	i2c_stop();
#endif

	i2c_start(LCD_ADDR_W);
	//i2c_write(bisaratio);	// 0xe8, 0xe9, 0xea, 0xeb - 6,7,8,9 bias ratios
	i2c_write(0x81);  // Set Vbias Potentiometer TIC154
	i2c_write(vbias);

	i2c_write(0xAF);  // Set Display Enable
	i2c_waitsend();
	i2c_stop();
	local_delay_us(10);		// delay required after Set Display Enable command

	i2c_start(LCD_ADDR_W);
	i2c_write(0xA0);  // frame rate = 80 fps
	i2c_waitsend();
	i2c_stop();

#if 0
	// тестирование наличия индикатора
	uint_fast8_t v1 = 0xaa, v2 = 0x55;
	i2c_start(LCD_ADDR_R);
	i2c_read(& v1, I2C_READ_ACK_1);
	i2c_read(& v2, I2C_READ_NACK);

	//display_at(0, 0, "status = ");
	// expected: 0x88, 0x60	- RDX0077
	// expected: 0xa8, 0x60
	// expected: 0xc8, 0x60
	debug_printf_P(PSTR("UC1601s status=%02" PRIXFAST8 ":%02" PRIXFAST8 "\n"), v1, v2);
	//
	char buff [8];
	local_snprintf_P(buff, 8, PSTR("%02X:%02X"), v1, v2);
	display_at(0, 0, buff);
	local_delay_ms(300);
	//for (;;)
	//	;
#endif

}

/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{
	uc1601s_initialize();
}

void display_set_contrast(uint_fast8_t v)
{
	uc1601s_set_contrast(v);
}

void 
display_clear(void)
{
	uc1601s_clear();
}

void
colmain_setcolors(COLORMAIN_T fg, COLORMAIN_T bg)
{
	(void) fg;
	(void) bg;
}

void colmain_setcolors3(COLORMAIN_T fg, COLORMAIN_T bg, COLORMAIN_T fgbg)
{
	colmain_setcolors(fg, bg);
}

void
display_wrdata_begin(void)
{
	uc1601s_put_char_begin();
}

void
display_wrdata_end(void)
{
	uc1601s_put_char_end();
}

void
display_wrdatabar_begin(void)
{
	uc1601s_put_char_begin();
}

void
display_wrdatabar_end(void)
{
	uc1601s_put_char_end();
}


void
display_wrdatabig_begin(void)
{
	uc1601s_put_char_begin();
}


void
display_wrdatabig_end(void)
{
	uc1601s_put_char_end();
}

/* отображение одной вертикальной полосы на графическом индикаторе */
/* старшие биты соответствуют верхним пикселям изображения */
/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
void 
display_barcolumn(uint_fast8_t pattern)
{
	uc1601s_bar_column(pattern);
}

void
display_put_char_big(uint_fast8_t c, uint_fast8_t lowhalf)
{
	uc1601s_put_char_big(c, lowhalf);
}

void
display_put_char_half(uint_fast8_t c, uint_fast8_t lowhalf)
{
	uc1601s_put_char_half(c, lowhalf);
}


// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
void
display_put_char_small(uint_fast8_t c, uint_fast8_t lowhalf)
{
	(void) lowhalf;
	uc1601s_put_char_small(c);
}

// самый маленький шрифт
// stub function
void display_wrdata2_begin(void)
{
	display_wrdata_begin();
}
// stub function
void display_wrdata2_end(void)
{
	display_wrdata_end();
}
// stub function
void display_put_char_small2(uint_fast8_t c, uint_fast8_t lowhalf)
{
	display_put_char_small(c, lowhalf);
}

void
display_gotoxy(uint_fast8_t x, uint_fast8_t y)
{
#if LCDMODE_UC1601S_TOPDOWN || LCDMODE_RDT065
	uc1601s_set_addr_column(x * CHAR_W + (132 - DIM_X), DIM_Y / CHAR_H - 1 - y);
#else /* LCDMODE_UC1601S_TOPDOWN || LCDMODE_RDT065 */
	uc1601s_set_addr_column(x * CHAR_W, DIM_Y / CHAR_H - 1 - y);
#endif /* LCDMODE_UC1601S_TOPDOWN || LCDMODE_RDT065 */
}

// Координаты в пикселях
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
#if LCDMODE_UC1601S_TOPDOWN || LCDMODE_RDT065
	uc1601s_set_addr_column(x + (132 - DIM_X), DIM_Y / CHAR_H - 1 - y / CHAR_H);
#else /* LCDMODE_UC1601S_TOPDOWN || LCDMODE_RDT065 */
	uc1601s_set_addr_column(x, DIM_Y / CHAR_H - 1 - y / CHAR_H);
#endif /* LCDMODE_UC1601S_TOPDOWN || LCDMODE_RDT065 */
}


void display_plotstart(
	uint_fast16_t dy	// Высота окна в пикселях
	)
{

}

void display_plot(
	const PACKEDCOLORMAIN_T * buffer, 
	uint_fast16_t dx,	// Размеры окна в пикселях
	uint_fast16_t dy
	)
{

}

void display_plotstop(void)
{

}


/* аппаратный сброс дисплея - перед инициализаций */
/* вызывается при разрешённых прерываниях. */
void
display_reset(void)
{
	uc1601s_reset();
}

/* Разряжаем конденсаторы питания */
void display_discharge(void)
{
	i2c_start(LCD_ADDR_W);
	i2c_write(0xe2);		// system reset
	i2c_waitsend();
	i2c_stop();
}

#endif /* LCDMODE_UC1601 */
