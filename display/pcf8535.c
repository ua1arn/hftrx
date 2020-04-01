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

#if LCDMODE_PCF8535 || LCDMODE_PCF8531

/* растровые шрифты */
#include "./fonts/uc1601s_font_small.h"
#if FONTSTYLE_ITALIC
	#include "./fonts/uc1601s_ifont_half.h"
	#include "./fonts/uc1601s_ifont_big.h"
#else /* FONTSTYLE_ITALIC */
	#include "./fonts/uc1601s_font_half.h"
	#include "./fonts/uc1601s_font_big.h"
#endif /* FONTSTYLE_ITALIC */

	// PCF8535, PCF8531 Address:
	// write-only devices

	#define LCD_ADDR_W 0x78 //address, R/W = 0
	//#define LCD_ADDR_R 0x79 //address, R/W = 1

	//Control Byte:

	#define LCD_CTRL 0x00 //Co = 0, D/C = 0
	#define LCD_CO_CTRL 0x80 //Co = 1, D/C = 0
	#define LCD_DATA 0x40 //Co = 0, D/C = 1

	//H[2:0] = XXX - RAM Read/Write Command Page:

	#define LCD_NOP 0x00 //no operation
	#define LCD_PAGE_FN 0x01 //H[2:0] = 111: select Function and RAM Command Page

	//H[2:0] = 111 - Function and RAM Command Page:

	#define LCD_PAGE_DS 0x0E //H[2:0] = 110: select Display Setting Command Page
	#define LCD_PAGE_HV 0x0D //H[2:0] = 101: select HV-gen Command Page
	#define LCD_PAGE_SP 0x0B //H[2:0] = 011: select Special Feature Command Page

	#define LCD_POWERDN 0x14 //PD = 1, V = 0: power down
	#define LCD_VERT_ADDR 0x12 //PD = 0, V = 1: vertical addressing
	#define LCD_HOR_ADDR 0x10 //PD = 0, V = 0: horizontal addressing

	#define LCD_XM_0 0x20 //XMo = 0, X addressing 0..127
	#define LCD_XM_1 0x24 //XMo = 0, X addressing 128..132

	#define LCD_ADDR_Y 0x40 //+ Y address 0..8
	#define LCD_ADDR_X 0x80 //+ X address 0..127

	//H[2:0] = 110 - Display Setting Command Page:

	#define LCD_BLANK 0x04 //D = 0, E = 0: display blank
	#define LCD_NORMAL 0x06 //D = 1, E = 0: normal mode
	#define LCD_ALL_ON 0x05 //D = 0, E = 1: all display segments on
	#define LCD_INV_VID 0x07 //D = 1, E = 1: inverse video

	#define LCD_NO_MIRROR 0x08 //MX = 0, MY = 0: no mirror
	#define LCD_X_MIRROR 0x0C //MX = 1, MY = 0: X mirror
	#define LCD_Y_MIRROR 0x0A //MX = 0, MY = 1: Y mirror
	#define LCD_XY_MIRROR 0x0E //MX = 1, MY = 1: Y mirror

	#define LCD_BIAS_1_11 0x10 //[BS2:BS0] = 0: bias 1/11
	#define LCD_BIAS_1_10 0x11 //[BS2:BS0] = 1: bias 1/10
	#define LCD_BIAS_1_9 0x12 //[BS2:BS0] = 2: bias 1/9
	#define LCD_BIAS_1_8 0x13 //[BS2:BS0] = 3: bias 1/8
	#define LCD_BIAS_1_7 0x14 //[BS2:BS0] = 4: bias 1/7
	#define LCD_BIAS_1_6 0x15 //[BS2:BS0] = 5: bias 1/6
	#define LCD_BIAS_1_5 0x16 //[BS2:BS0] = 6: bias 1/5
	#define LCD_BIAS_1_4 0x17 //[BS2:BS0] = 7: bias 1/4

	#define LCD_BIAS_L 0x20 //IB = 0: low bias current mode
	#define LCD_BIAS_H 0x24 //IB = 1: high bias current mode

	#define LCD_MUX_1_17 0x80 //[M2:M0] = 0: multiplex rate 1/17
	#define LCD_MUX_1_26 0x81 //[M2:M0] = 1: multiplex rate 1/26
	#define LCD_MUX_1_34 0x82 //[M2:M0] = 2: multiplex rate 1/34
	#define LCD_MUX_1_49 0x83 //[M2:M0] = 3: multiplex rate 1/49
	#define LCD_MUX_1_65 0x84 //[M2:M0] = 4: multiplex rate 1/65

	//H[2:0] = 101 - HV-gen Command Page:

	#define LCD_HV_OFF 0x04 //PRS = 0, HVE = 0: HV-gen disabled
	#define LCD_HV_LOW 0x05 //PRS = 0, HVE = 1: HV-gen on, Vlcd low range
	#define LCD_HV_HIGH 0x07 //PRS = 1, HVE = 1: HV-gen on, Vlcd high range

	#define LCD_VMUL_X2 0x08 //[S1:S0] = 0: multiplication factor 2
	#define LCD_VMUL_X3 0x09 //[S1:S0] = 1: multiplication factor 3
	#define LCD_VMUL_X4 0x0A //[S1:S0] = 2: multiplication factor 4
	#define LCD_VMUL_X5 0x0B //[S1:S0] = 3: multiplication factor 5

	#define LCD_TC_0_00 0x10 //[TC2:TC0] = 0: temp. coeff. 0.00E-3 1/°C
	#define LCD_TC_0_44 0x11 //[TC2:TC0] = 1: temp. coeff. -0.44E-3 1/°C
	#define LCD_TC_1_10 0x12 //[TC2:TC0] = 2: temp. coeff. -1.10E-3 1/°C
	#define LCD_TC_1_45 0x13 //[TC2:TC0] = 3: temp. coeff. -1.45E-3 1/°C
	#define LCD_TC_1_91 0x14 //[TC2:TC0] = 4: temp. coeff. -1.91E-3 1/°C
	#define LCD_TC_2_15 0x15 //[TC2:TC0] = 5: temp. coeff. -2.15E-3 1/°C
	#define LCD_TC_2_32 0x16 //[TC2:TC0] = 6: temp. coeff. -2.32E-3 1/°C
	#define LCD_TC_2_74 0x17 //[TC2:TC0] = 7: temp. coeff. -2.74E-3 1/°C

	#define LCD_START_TM 0x21 //TM = 1: start temperature measurement

	#define LCD_V_LCD 0x80 //+ Vlcd 0..127

	//H[2:0] = 011 - Special Feature Command Page:

	#define LCD_DISP_OFF 0x06 //DOF = 1, DM = 0: display off
	#define LCD_EXT_VLCD 0x04 //DOF = 0, DM = 0: external Vlcd (if HVE = 0)
	#define LCD_INT_VLCD 0x04 //DOF = 0, DM = 0: internal Vlcd (if HVE = 1)
	#define LCD_VDD_VLCD 0x05 //DOF = 0, DM = 1: using Vdd as Vlcd (if HVE = 0)

	#define LCD_OSC_EN 0x08 //EC = 0: internal oscillator enabled
	#define LCD_OSC_DIS 0x0A //EC = 1: internal oscillator disabled

	#define LCD_NO_SWAP 0x40 //TRS = 0, BRS = 0: no row swap
	#define LCD_TOP_SWAP 0x50 //TRS = 1, BRS = 0: top row swap
	#define LCD_BOT_SWAP 0x48 //TRS = 0, BRS = 1: bottom row swap
	#define LCD_ALL_SWAP 0x58 //TRS = 1, BRS = 1: all row swap


static void
pcf8535_reset(void)
{
	local_delay_ms(5);		// delay 5 mS
	board_lcd_reset(0); 	// Pull RST pin down
	board_update();
	local_delay_ms(10);		// delay 10 mS (for LPH88, 5 for LS020).
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(5);		// delay 5 mS
}

/*
 Функция установки курсора в позицию x,y
 X - координата по горизонтали в пределах 0-132, 
 Y - координата по вертикали (строка, Page) в пределах 0-7
*/ 
static void pcf8535_set_addr_column(uint_fast8_t x, uint_fast8_t y)		// 
{

	i2c_start(LCD_ADDR_W);           // адрес
	i2c_write(LCD_CTRL);     // control byte
	i2c_write(LCD_PAGE_FN);           // на основн стр
	i2c_write(x >= 128 ? LCD_XM_1 : LCD_XM_0);
	i2c_write(LCD_ADDR_Y | (y & 0x0f));     // Y = 0;
	i2c_write(LCD_ADDR_X | (x & 0x7f));     // X = 0;
	i2c_waitsend();
	i2c_stop();
}

static void pcf8535_clear(void)
{
    uint_fast8_t cnt_y;

	for (cnt_y = 0; cnt_y < ((DIM_Y + 7) / 8); ++ cnt_y) // RDX0154
	{
	    uint_fast8_t cnt_x;

		pcf8535_set_addr_column(0, cnt_y);

		i2c_start(LCD_ADDR_W);	// адрес
		i2c_write(LCD_DATA); // control byte

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
smallfont_decode(uint_fast8_t c)
{
	return c - ' ';
}


// начало выдаче байтов (записи в видеопамять)
// Вызывается в начале выдачи строки
static void pcf8535_put_char_begin(void)
{
	i2c_start(LCD_ADDR_W);	// адрес
	i2c_write(LCD_DATA); // control byte
}

// конец выдаче байтов (записи в видеопамять)
// Вызывается в конце выдачи строки
static void pcf8535_put_char_end(void)
{
	i2c_waitsend();
    i2c_stop();
}


static void putvbuff(uint_fast8_t c)
{
#if LCDMODE_PCF8531
	i2c_write(revbits8(c));
#else /* LCDMODE_PCF8531 */
	i2c_write(c);
#endif /* LCDMODE_PCF8531 */
}

// Вызовы этой функции (или группу вызовов) требуется "обрамить" парой вызовов
// uc1601s_put_char_begin() и uc1601s_put_char_end().
//
static void pcf8535_put_char_small(char cc)
{
    const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	enum { NCOLS = (sizeof uc1601s_font [c] / sizeof uc1601s_font[c][0]) };
	const FLASHMEM uint8_t * p = & uc1601s_font[c][0];
	uint_fast8_t i;

	for (i = 0; i < NCOLS; ++ i)
    	putvbuff(p [i]);
}


// многополосный вывод символов - за несколько горизонтальных проходов.
// Нумерация полос - сверху вниз, начиная с 0

// Вызовы этой функции (или группу вызовов) требуется "обрамить" парой вызовов
// uc1601s_put_char_begin() и uc1601s_put_char_end().
//
static void pcf8535_put_char_big(char cc, uint_fast8_t lowhalf)
{
	enum { NBV = (BIGCHARH / 8) }; // сколько байтов в одной вертикали
	uint_fast8_t i = 1 * ((cc == '.' || cc == '#') ? 6 : 0);	// начальная колонка знакогенератора, откуда начинать.
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NCOLS = (sizeof uc1601s_bigfont [c][lowhalf] / sizeof uc1601s_bigfont [c][lowhalf] [0]) };
	const FLASHMEM uint8_t * p = & uc1601s_bigfont [c][lowhalf][0];

	for (; i < NCOLS; ++ i)
    	putvbuff(p [i]);
}

// многополосный вывод символов - за несколько горизонтальных проходов.
// Нумерация полос - сверху вниз, начиная с 0

// Вызовы этой функции (или группу вызовов) требуется "обрамить" парой вызовов
// uc1601s_put_char_begin() и uc1601s_put_char_end().
//
static void pcf8535_put_char_half(char cc, uint_fast8_t lowhalf)
{
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NCOLS = (sizeof uc1601s_halffont [c][lowhalf] / sizeof uc1601s_halffont [c][lowhalf] [0]) };
	const FLASHMEM uint8_t * p = & uc1601s_halffont [c][lowhalf][0];
	uint_fast8_t i;

	for (i = 0; i < NCOLS; ++ i)
    	putvbuff(p [i]);
}


/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
static void 
pcf8535_bar_column(uint_fast8_t pattern)
{
	putvbuff(pattern);
}


static uint_fast8_t vbias = DEFAULT_LCD_CONTRAST;
//static uint_fast8_t bisaratio = 0xeb;	// 0xe8, 0xe9, 0xea, 0xeb - 6,7,8,9 bias ratios

static void pcf8535_set_contrast(uint_fast8_t v)
{
	if (vbias != v)
	{
		vbias = v;
		i2c_start(LCD_ADDR_W);
		i2c_write(LCD_CTRL);     //control byte
		i2c_write(LCD_PAGE_FN);  //select Function and RAM Command Page
		i2c_write(LCD_PAGE_HV);  //select HV-gen Command Page
		i2c_write(LCD_V_LCD + (vbias & 0x7f)); //set Vlcd
		i2c_waitsend();
		i2c_stop();
	}
}

#if 0

// Выключение дисплея (и включение его).
// TODO: перенести сюда управление питанием LCD
static void pcf8535_disable(uint_fast8_t state)
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

// pcf8531 function
static void write_data(uint_fast8_t dat)  
{
	i2c_start(LCD_ADDR_W);
	i2c_write(0x40);
	i2c_write(dat);
	i2c_waitsend();
	i2c_stop();
}

// pcf8531 function
static void write_com(uint_fast8_t com)  
{
	i2c_start(LCD_ADDR_W);
	i2c_write(0x00);
	i2c_write(com);
	i2c_waitsend();
	i2c_stop();
}

static void pcf8531_initialize(void)
{
	write_com(0x01); //Instruction set   RAM command page  (H[1:0] = 00)
	write_com(0x20);   //function and RAM command page PD = 0 and V = 0
	write_com(0x09);  //function and RAM command page select display setting  command page H[1:0] = 01

	write_com(0x0c);  //display setting command page; set normal mode (D = 1; IM = 0 and E = 0)
	write_com(0x05);  //select multiplex rate 1:34
	write_com(0x15); //set Bias System (BSx) 1/6
	write_com(0x01);  //H[2:0] independent command; select function and RAM command page H[1:0] = 00

	write_com(0x0a);  //function and RAM command page; select HV-gen command page H[1:0] = 10
	write_com(0x20);//Temperature control
	write_com(0x0b);  //HV-gen command page; select voltage multiplication factor 5 S[1:0] = 11
	write_com(0x22);  //HV-gen command page; select temperature coefficient 2 TC[2:0] = 010
	write_com(0x05);//HV-gen command page; select high VLCD programming range (PRS = 0); voltage multiplier off (HVE = 1)
	write_com(0x80+(0x59&0x7f));//HV-gen command page; setVLCD = 6.0v 
}

static void pcf8535_initialize(void)
{
	// The recommended wait period is 20 us per 100 nF of the
	enum { RECHARGEDELAY_MS = 20 };

	i2c_start(LCD_ADDR_W);
	i2c_write(LCD_CTRL);     //control byte

	i2c_write(LCD_PAGE_FN);  //select Function and RAM Command Page
	i2c_write(LCD_HOR_ADDR); //PD = 0

	i2c_write(LCD_PAGE_DS);  //select Display Setting Command Page
#if LCDMODE_TIC154
	#if LCDMODE_PCF8535_TOPDOWN
		i2c_write(LCD_NO_MIRROR); //set mirror mode for use with TIC154 - с зеркалированием по горизонтали
	#else
		i2c_write(LCD_XY_MIRROR); //set mirror mode for use with TIC154 - с зеркалированием по горизонтали и вертикали
	#endif
#else
	#if LCDMODE_PCF8535_TOPDOWN
		i2c_write(LCD_X_MIRROR); //set mirror mode for use with TIC218 - без зеркалирования по вертикали
	#else
		i2c_write(LCD_Y_MIRROR); //set mirror mode for use with TIC218 - без зеркалирования по горизонтали
	#endif
#endif

	i2c_write(LCD_BIAS_1_9); //set bias
	i2c_write(LCD_BIAS_H);   //select bias current
	i2c_write(LCD_MUX_1_65); //set mux rate
	i2c_write(LCD_NORMAL);   //D = 1, E = 0

	i2c_waitsend();
	i2c_stop();

	// Подключаем дисплей в обход преобразователя.

	i2c_start(LCD_ADDR_W);
	i2c_write(LCD_CTRL);     //control byte

	i2c_write(LCD_PAGE_FN);  //select Function and RAM Command Page
	i2c_write(LCD_PAGE_SP);  //select Special Feature Command Page
	i2c_write(LCD_ALL_SWAP); //TRS = 1, BRS = 1
	i2c_write(LCD_VDD_VLCD); //DM = 1

	i2c_waitsend();
	i2c_stop();

	local_delay_ms(RECHARGEDELAY_MS);

	// Программирование преобразователя питания.

	i2c_start(LCD_ADDR_W);
	i2c_write(LCD_CTRL);     //control byte


	i2c_write(LCD_PAGE_FN);  //select Function and RAM Command Page
	i2c_write(LCD_PAGE_HV);  //select HV-gen Command Page

	i2c_write(LCD_TC_1_10);  //temp. coeff. -1.10E-3 1/°C
	i2c_write(LCD_V_LCD + (vbias & 0x7f)); //set Vlcd
	i2c_write(LCD_HV_LOW);   //PRS = 0, HVE = 1
	i2c_waitsend();
	i2c_stop();

	local_delay_ms(RECHARGEDELAY_MS);

	// Программирование преобразователя питания.
	// Последовательное увеличение напряжения на выходе преобразователя.
	unsigned char i;
	for (i = 0; i < 2; ++ i)
	{
		static const FLASHMEM unsigned char vmuls [4] = { LCD_VMUL_X2, LCD_VMUL_X3, LCD_VMUL_X4, LCD_VMUL_X5, };

		i2c_start(LCD_ADDR_W);
		i2c_write(LCD_CTRL);     //control byte


		i2c_write(LCD_PAGE_FN);  //select Function and RAM Command Page
		i2c_write(LCD_PAGE_HV);  //select HV-gen Command Page

		i2c_write(vmuls [i]);  //set voltage mul factor
		i2c_waitsend();
		i2c_stop();

		local_delay_ms(RECHARGEDELAY_MS);
	}

	/* переключаем дисплей на преобразователь напряжения. */
	i2c_start(LCD_ADDR_W);
	i2c_write(LCD_CTRL);     //control byte

	i2c_write(LCD_PAGE_FN);  //select Function and RAM Command Page
	i2c_write(LCD_PAGE_SP);  //select Special Feature Command Page
	i2c_write(LCD_INT_VLCD); //DM = 0

	i2c_waitsend();
	i2c_stop();

}


/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{
	if (0)
	{
	int i;
	i2c_start(0x70);	// address
	i2c_write(0xC8);	// 0xc8 mode register
	i2c_write(0xF0);	// 0xf0 blink register
	i2c_write(0xE0);	// 0xe0 device select register
	i2c_write(0x00);	// 0x00 pointer register
	
	// light up all the segments, initialize the local display buffer as well
	for(i = 0; i < 20; i++) {
	i2c_write(0xFF);
	}
	
	i2c_waitsend();
	i2c_stop();
	for (;;)
	;
	}

#if LCDMODE_PCF8531
	pcf8531_initialize();
#elif LCDMODE_PCF8535
	pcf8535_initialize();
#else
	#error Wrong LCDMODE_XXX
#endif
}

void display_set_contrast(uint_fast8_t v)
{
	pcf8535_set_contrast(v);
}

void 
display_clear(void)
{
	pcf8535_clear();
}

void
//NOINLINEAT
display_setcolors(COLORMAIN_T fg, COLORMAIN_T bg)
{
	(void) fg;
	(void) bg;
}

void display_setcolors3(COLORMAIN_T fg, COLORMAIN_T bg, COLORMAIN_T fgbg)
{
	display_setcolors(fg, bg);
}

void
display_wrdata_begin(void)
{
	pcf8535_put_char_begin();
}

void
display_wrdata_end(void)
{
	pcf8535_put_char_end();
}

void
display_wrdata2_begin(void)
{
	pcf8535_put_char_begin();
}

void
display_wrdata2_end(void)
{
	pcf8535_put_char_end();
}


void
display_wrdatabig_begin(void)
{
	pcf8535_put_char_begin();
}


void
display_wrdatabig_end(void)
{
	pcf8535_put_char_end();
}

/* отображение одной вертикальной полосы на графическом индикаторе */
/* старшие биты соответствуют верхним пикселям изображения */
/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
void 
display_barcolumn(uint_fast8_t pattern)
{
	pcf8535_bar_column(pattern);
}

void
display_wrdatabar_begin(void)
{
	pcf8535_put_char_begin();
}

void
display_wrdatabar_end(void)
{
	pcf8535_put_char_end();
}

void
display_put_char_big(uint_fast8_t c, uint_fast8_t lowhalf)
{
	pcf8535_put_char_big(c, lowhalf);
}

void
display_put_char_half(uint_fast8_t c, uint_fast8_t lowhalf)
{
	pcf8535_put_char_half(c, lowhalf);
}


// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
void
display_put_char_small(uint_fast8_t c, uint_fast8_t lowhalf)
{
	(void) lowhalf;
	pcf8535_put_char_small(c);
}
// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
void
display_put_char_small2(uint_fast8_t c, uint_fast8_t lowhalf)
{
	(void) lowhalf;
	pcf8535_put_char_small(c);
}

	//uint8_t x = h * CHAR_W;
	//uint8_t y = Y_DIM - 5 - (v * CHAR_H);

void
display_gotoxy(uint_fast8_t x, uint_fast8_t y)
{
	pcf8535_set_addr_column(x * CHAR_W, y);
}
// Координаты в пикселях
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
	pcf8535_set_addr_column(x, y / CHAR_H);
}

void display_plotstart(
	uint_fast16_t height	// Высота окна в пикселях
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
	pcf8535_reset();
}


/* Разряжаем конденсаторы питания */
void display_discharge(void)
{
}

#endif /* LCDMODE_PCF8535 || LCDMODE_PCF8531 */
