/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if LCDMODE_HD44780


#include "display.h"
#include "hd44780.h"
#include "board.h"
#include "spi.h"

#include <stdlib.h>

#if defined (WRITEE_BIT) && defined (WRITEE_BIT_ZERO)
	#error Do not define WRITEE_BIT and WRITEE_BIT_ZERO together
#endif



#define HD44780_BUSYFLAG 0x80

#if ! LCDMODE_HARD_SPI
/* задержка на переходные процессы в проводах и буферах между индикатором и контроллером */
static void hd44780_delay(void)
{
	hardware_spi_io_delay();
	local_delay_us(25);
}

#endif /* LCDMODE_HARD_SPI */


#if LCDMODE_HARD_SPI

/* Можно только в случае использования SPI MODE3 */
#define OLED_SPIMODE SPIC_MODE3

#elif CPUSTYLE_ARM

// ARM subroutinue
// sent four bits from d3..d0 of argument v
static void 
hd44780_send4(uint_fast8_t v)
{
	DISPLAY_BUS_DATA_SET(v);
}

// ARM subroutinue
static 
//NOINLINEAT
uint_fast8_t 
hd44780_pulse_strobe(void)
{
	uint_fast8_t v;

	LCD_STROBE_PORT_S(LCD_STROBE_BIT); 	// activate EN signal
	hd44780_delay();
	v = DISPLAY_BUS_DATA_GET();	/* получить данные с шины LCD */
	LCD_STROBE_PORT_C(LCD_STROBE_BIT); 	// deactivate EN signal
	hd44780_delay();
	return v;
}

#if defined (WRITEE_BIT)
	// ARM subroutinue
	// rs=0: status register will be read
	// rs=1: data register will be read
	static void 
	hd44780_to_read(uint_fast8_t rs)
	{
		LCD_DATA_INITIALIZE_READ();	// переключить порт на чтение с выводов

		if (rs)
			LCD_RS_PORT_S(ADDRES_BIT);
		else
			LCD_RS_PORT_C(ADDRES_BIT);

		LCD_WE_PORT_S(WRITEE_BIT);	// H: READ DATA, L:WRITE DATA
		hd44780_delay();		// это может быть посделняя операция перед проверкой состояния
	}
#endif

// ARM subroutinue
// rs=0: status register will be written
// rs=1: data register will be written
static void 
hd44780_to_write(uint_fast8_t rs, uint_fast8_t nibble)
{
	if (rs)
		LCD_RS_PORT_S(ADDRES_BIT);
	else
		LCD_RS_PORT_C(ADDRES_BIT);

#if defined (WRITEE_BIT)
	LCD_WE_PORT_C(WRITEE_BIT);	// H: READ DATA, L:WRITE DATA
#elif defined (WRITEE_BIT_ZERO)
	LCD_WE_PORT_C(WRITEE_BIT_ZERO);	// H: READ DATA, L:WRITE DATA
#endif
	LCD_DATA_INITIALIZE_WRITE(nibble);	// открыть выходы порта
}

// ARM subroutinue
/* вызывается при запрещённых прерываниях. */
void 
hd44780_io_initialize(void)
{
	#if defined (WRITEE_BIT)
		LCD_CONTROL_INITIALIZE();
	#elif defined (WRITEE_BIT_ZERO)
		LCD_CONTROL_INITIALIZE_WEEZERO();
	#else
		LCD_CONTROL_INITIALIZE_WEENONE();
	#endif

	LCD_DATA_INITIALIZE_WRITE(0);
	//hd44780_to_read(0);	// rs=0: status register will be read - initial state
}

#elif CPUSTYLE_ATMEGA || CPUSTYLE_ATXMEGA

// ATMega subroutinue
// sent four bits from d3..d0 of argument v
static void 
//NOINLINEAT
hd44780_send4(uint_fast8_t v)
{
	DISPLAY_BUS_DATA_SET(v);
}

// ATMega subroutinue
static uint_fast8_t 
//NOINLINEAT
hd44780_pulse_strobe(void)
{
	uint_fast8_t v;

	LCD_STROBE_PORT |= LCD_STROBE_BIT; 	// activate EN signal
	hd44780_delay();
	v = DISPLAY_BUS_DATA_GET();	/* получить данные с шины LCD */
	LCD_STROBE_PORT &= ~ LCD_STROBE_BIT; // deactivate EN signal
	hd44780_delay();
	return v;
}

#if defined (WRITEE_BIT)
	// ATMega subroutinue
	// rs=0: status register will be read
	// rs=1: data register will be read
	// set we and rs lines state
	static void 
	//NOINLINEAT
	hd44780_to_read(uint_fast8_t rs)
	{
		LCD_DATA_DIRECTION &= ~ LCD_DATAS_BITS;
		if (rs)
			LCD_RS_WE_PORT |= ADDRES_BIT;
		else
			LCD_RS_WE_PORT &= ~ ADDRES_BIT;
		LCD_RS_WE_PORT |= WRITEE_BIT;	// H: READ DATA, L:WRITE DATA
		hd44780_delay();
	}
#endif

// ATMega subroutinue
// rs=0: status register will be written
// rs=1: data register will be written
// set we and rs lines state
static void 
//NOINLINEAT
hd44780_to_write(uint_fast8_t rs, uint_fast8_t nibble)
{
	if (rs)
		LCD_RS_WE_PORT |= ADDRES_BIT;
	else
		LCD_RS_WE_PORT &= ~ ADDRES_BIT;

#if defined (WRITEE_BIT)
	LCD_RS_WE_PORT &= ~ WRITEE_BIT;
#elif defined (WRITEE_BIT_ZERO)
	LCD_RS_WE_PORT &= ~ WRITEE_BIT_ZERO;
#endif
	LCD_DATA_DIRECTION |= LCD_DATAS_BITS;
}
// ATMega subroutinue
/* вызывается при запрещённых прерываниях. */
void
//NOINLINEAT
hd44780_io_initialize(void)
{
	LCD_STROBE_PORT &= ~ LCD_STROBE_BIT;	// deactivate enable pin
	LCD_STROBE_DIRECTION |= LCD_STROBE_BIT;

#if defined (WRITEE_BIT)
	LCD_RS_WE_DIRECTION |= WRITEE_BIT;
#elif defined(WRITEE_BIT_ZERO)
	LCD_RS_WE_DIRECTION |= WRITEE_BIT_ZERO;
	LCD_RS_WE_PORT &= ~ WRITEE_BIT_ZERO;
#endif

	LCD_RS_WE_DIRECTION |= ADDRES_BIT;
	//hd44780_to_read(0);	// rs=0: status register will be read - initial state
}

#elif CPUSTYLE_STM32F1XX
	#error: to be implemented for CPUSTYLE_STM32F1XX


#else // CPUSTYLE_ATMEGA
 	#error Undefined CPUSTYLE_XXX
#endif // CPUSTYLE_ARM


// architecture-independet function
static uint_fast8_t
//NOINLINEAT
hd44780_read_status(void)
{
	enum { rs = 0 };	// rs=0: status register will be read - initial state

#if LCDMODE_HARD_SPI
	const spitarget_t target = targetlcd;	/* адрес контроллера дисплея на SPI шине */
	uint_fast8_t v;

	prog_select(target);	/* start sending data to target chip */

	prog_bit(target, rs);	/* register select */
	prog_bit(target, 0x01);	/* 1 - read, 0 - write. */

	#if WITHSPIHW
		/* Можно только в случае использования SPI MODE3 */
		hardware_spi_connect(SPIC_SPEEDFAST, OLED_SPIMODE);
		(void) hardware_spi_b16(0xff);
		v = hardware_spi_b16(0xff);
		hardware_spi_disconnect();

	#else /* WITHSPIHW */

		prog_spi_to_read(target);
		(void) spi_read_byte(target, 0xff);	/* dummy byte - пропускаем */
		v = spi_read_byte(target, 0xff);		/* получаем данные */
		prog_spi_to_write(target);
	#endif /* WITHSPIHW */
	prog_unselect(target);	/* done sending data to target chip */

	return v;

#else /* LCDMODE_HARD_SPI */

	#if defined (WRITEE_BIT)
		uint_fast8_t hi, lo;

		hd44780_to_read(rs);	// rs=0: status register will be read
		// read 1-st nibble from high port lines
		hi = hd44780_pulse_strobe();
		// read 2-nd nibble from high port lines
		lo = hd44780_pulse_strobe();

		return (hi * 16) + lo;
	#else /* defined (WRITEE_BIT) */
		local_delay_ms(2);	// задержка - вызов этой функции происходит 1 раз.
		return 0;
	#endif /* defined (WRITEE_BIT) */

#endif /* LCDMODE_HARD_SPI */
}

// architecture-independet function
// возвращаем 0 если не дождались готовности
static 
uint_fast8_t
//NOINLINEAT
hd44780_wait_ready(void)
{
	uint_fast8_t n;

    // not need switch to read - is a default state
	// но эти выводы могут быть использованы совместно с чем-либо (SPI).
	for (n = 255; -- n;)
	{
		const uint_fast8_t stsh = hd44780_read_status();	// read status byte: 4 calls of hd44780_delay(). = 80 uS. 200*80uS = 1.6 mS

		if ((stsh & HD44780_BUSYFLAG) == 0)
			return 1;	// Дождались готовности
	}
	return 0;
}


/* дождаться готовности индикатора, подготовится к записи данных. */
void 
hd44780_put_char_begin(void)	/* Выдать CSB при использовании SPI интерфейса */
{
	enum { rs = 1 };		/* Register select: 0 - data register, 1 - command register */

	hd44780_wait_ready();

#if LCDMODE_HARD_SPI
	const spitarget_t target = targetlcd;	/* адрес контроллера дисплея на SPI шине */


	prog_select(target);	/* start sending data to target chip */

	prog_bit(target, rs);	/* register select */
	prog_bit(target, 0x00);	/* 1 - read, 0 - write. */

	#if WITHSPIHW
		/* Можно только в случае использования SPI MODE3 */
		hardware_spi_connect(SPIC_SPEEDFAST, OLED_SPIMODE);
	#endif /* WITHSPIHW */

#else /* LCDMODE_HARD_SPI */
	/* паралельный интерфейс */
	hd44780_to_write(rs, 0);	// data register select
#endif /* LCDMODE_HARD_SPI */
}

void hd44780_put_char_end(void)	/* Снять CSB при использовании SPI интерфейса */
{
#if LCDMODE_HARD_SPI
	const spitarget_t target = targetlcd;	/* адрес контроллера дисплея на SPI шине */

	#if WITHSPIHW
		/* Можно только в случае использования SPI MODE3 */
		hardware_spi_disconnect();
	#endif /* WITHSPIHW */

	prog_unselect(target);	/* done sending data to target chip */

#else
#endif /* LCDMODE_HARD_SPI */
}



// architecture-independet function
// rs = 1
// Выдача байта данных без проверки готовности.
// Для индикатора на последовательном интерфейсе - выдача в послеовательности без
// перевода CSB в активное состояние и выдачи двух битов RS и #WE в начале.
// Эти операйии делаются в функции hd44780_put_char_begin(). По окончании
// передачи блока вызывается вункция hd44780_put_char_end().
void 
//NOINLINEAT
hd44780_wrdata_fast(
	uint_fast8_t v)
{
#if LCDMODE_HARD_SPI
	const spitarget_t target = targetlcd;	/* адрес контроллера дисплея на SPI шине */

#if WITHSPIHW
	(void) target;
	hardware_spi_b16(v);
#else
	prog_val(target, v, 8);		/* передаём данные */
#endif /* WITHSPIHW */

#else /* LCDMODE_HARD_SPI */
	// паралельный режим обмиена с индикатором
	hd44780_send4(v >> 4);
	hd44780_pulse_strobe();

	hd44780_send4(v);
	hd44780_pulse_strobe();
#endif /* LCDMODE_HARD_SPI */
}


// architecture-independet function
// rs = 1
static void 
//NOINLINEAT
hd44780_wrdata_slow(
	uint_fast8_t v)
{
	hd44780_put_char_begin();	/* Выдать CSB при использовании SPI интерфейса и два бита о записи данных */
	hd44780_wrdata_fast(v);
	hd44780_put_char_end();
}

// architecture-independet function
// rs = 0
static void 
//NOINLINEAT
hd44780_wrcmd(
	uint_fast8_t v)
{
	enum { rs = 0 };

	if (hd44780_wait_ready() != 0)
	{
#if LCDMODE_HARD_SPI
		const spitarget_t target = targetlcd;	/* адрес контроллера дисплея на SPI шине */

		prog_select(target);	/* start sending data to target chip */
		prog_bit(target, rs);	/* register select */
		prog_bit(target, 0x00);	/* 1 - read, 0 - write. */
		prog_val(target, v, 8);		/* передаём данные */
		prog_unselect(target);	/* done sending data to target chip */

#else
		hd44780_to_write(rs, v >> 4);

		hd44780_send4(v >> 4);
		hd44780_pulse_strobe();

		hd44780_send4(v);
		hd44780_pulse_strobe();
#endif /* LCDMODE_HARD_SPI */
	}
}

#if ! LCDMODE_HARD_SPI

// architecture-independet function
// вывод четырёхбитного управляющего слова в процессе инициализации дисплея.
// rs = 0
static void
//NOINLINEAT
hd44780_wrcmd4_nowait(
	uint_fast8_t v)
{
	enum { rs = 0 };
	local_delay_ms(10);		// 4.1 ms in examples from ATMEL
	local_delay_ms(10);
	hd44780_to_write(rs, v);

	hd44780_send4(v);
	hd44780_pulse_strobe();

	//hd44780_to_read(0);	// rs=0: status register will be read - initial state
}

#if defined (WRITEE_BIT)

static uint_fast8_t
testdisplay(void)
{
	const uint_fast8_t signature = 0x56; //0x36;

	hd44780_wrcmd(0x80 | signature);	/* переключение на работу с DDRAM */

	return (hd44780_read_status() & ~ HD44780_BUSYFLAG) == signature;
}
#endif

#endif


/* x, y - zero-based positions */
static void 
hd44780_gotoaddr(uint_fast8_t addr)
{
	hd44780_wrcmd(0x80 | addr);
}

static void hd44780_clear(void)
{
	hd44780_wrcmd(0x01);		// очистка дисплея
//	hd44780_wrcmd(0x02);		// home
	#if ! defined (WRITEE_BIT)
		local_delay_ms(20);
	#endif
}

/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
static void hd44780_bar_column(uint_fast8_t pattern)
{
	hd44780_wrdata_slow(pattern);
}

#if WITHBARS
// Вокруг этой функции вызывать hd44780_put_char_begin() и hd44780_put_char_end().

void
colmain_bar(
	uint_fast8_t width,	/* количество знакомест, занимаемых индикатором */
	/* was: unsigned int */ uint_fast8_t value,		/* значение, которое надо отобразить */
	uint_fast8_t tracevalue,		/* значение маркера, которое надо отобразить */
	/* was: unsigned int */ uint_fast8_t topvalue,	/* значение, соответствующее полностью заполненному индикатору */
	uint_fast8_t charbase, /* PATTERN_BAR_HALF или PATTERN_BAR_FULL */
	uint_fast8_t fullbasetop, /* PATTERN_BAR_HALF или PATTERN_BAR_FULL */
	uint_fast8_t emptybase)	
{
	const div_t d = div(value * GRID2X(CHARS2GRID(width)) / topvalue, GRID2X(CHARS2GRID(1)));
	uint_fast8_t i;

	// hd44780_put_char_begin и hd44780_wrdata_fast на OLED здесь не работают.
	for (i = 0; i < d.quot; ++ i)
	{
		hd44780_bar_column(GRID2X(CHARS2GRID(1)) + charbase);	/* полностью заполненный прямоугольник */
	}
	if (d.rem != 0)
	{
		hd44780_bar_column(d.rem + charbase);	/* частично заполненныё прямоугольник - заполненность определяется типом */
		++ i;
	}
	for (; i < width; ++ i)
	{
		hd44780_bar_column(0x00 + charbase);	/* пустой прямоугольник */
	}
}

// architecture-independet function
// 
static void
//NOINLINEAT
hd44780_progbar(
		uint_fast8_t code,	/* код символа (0..7) */
		uint_fast8_t mask5bit,
		uint_fast8_t allbits,
		uint_fast8_t blank)
{
	uint_fast8_t i;

	hd44780_wrcmd(0x40 | (code & 0x07) * 8);	/* переключение на работу с SGRAM */

	// hd44780_put_char_begin и hd44780_wrdata_fast на OLED здесь не работают.
	// придётся медленным способом...
	for (i = 0; i < blank; ++ i)
	{
		hd44780_wrdata_slow(0x00);	/* верхняя часть без точек */
	}
	hd44780_wrdata_slow(allbits);	/* первая строка всегда с разметкой */
	++ i;
	for (; i < 6; ++ i)
	{
		hd44780_wrdata_slow(mask5bit & 0x1f);
	}
	
	hd44780_wrdata_slow(allbits);	/* пследняя строка всегда с разметкой */
	hd44780_wrdata_slow(0x00);	/* нижняя часть без точек */

	hd44780_wrcmd(0x80);	/* переключение на работу с DDRAM */
}


static void
hd44780_initbar(void)
{
	const uint_fast8_t blank = 3;
	/* Программирование знакогенератора символами для использлвания в S-метре */
#if GRID2X(CHARS2GRID(1)) == 3
	/* на каждом знакоместе три градации уровня */
	hd44780_progbar(PATTERN_BAR_HALF + 0, (uint_fast8_t) (0x15 << 6), 0x15, blank);	/* пустой прямоугольник */
	hd44780_progbar(PATTERN_BAR_HALF + 1, (uint_fast8_t) (0x15 << 4), 0x15, blank);
	hd44780_progbar(PATTERN_BAR_HALF + 2, (uint_fast8_t) (0x15 << 2), 0x15, blank);	/* полносьтю заполненный прямоугольник */
	hd44780_progbar(PATTERN_BAR_HALF + 3, (uint_fast8_t) (0x15 << 0), 0x15, blank);	/* полносьтю заполненный прямоугольник */

	hd44780_progbar(PATTERN_BAR_FULL + 0, (uint_fast8_t) (0x15 << 6), 0x15, 1);	/* пустой прямоугольник */
	hd44780_progbar(PATTERN_BAR_FULL + 1, (uint_fast8_t) (0x15 << 4), 0x15, 1);
	hd44780_progbar(PATTERN_BAR_FULL + 2, (uint_fast8_t) (0x15 << 2), 0x15, 1);	/* полносьтю заполненный прямоугольник */
	hd44780_progbar(PATTERN_BAR_FULL + 3, (uint_fast8_t) (0x15 << 0), 0x15, 1);	/* полносьтю заполненный прямоугольник */
#elif GRID2X(CHARS2GRID(1)) == 2
	/* на каждом знакоместе две градации уровня */
	hd44780_progbar(PATTERN_BAR_HALF + 0, (uint_fast8_t) (0x1b << 6), 0x1b, blank);	/* пустой прямоугольник */
	hd44780_progbar(PATTERN_BAR_HALF + 1, (uint_fast8_t) (0x1b << 3), 0x1b, blank);
	hd44780_progbar(PATTERN_BAR_HALF + 2, (uint_fast8_t) (0x1b << 0), 0x1b, blank);	/* полносьтю заполненный прямоугольник */

	hd44780_progbar(PATTERN_BAR_FULL + 0, (uint_fast8_t) (0x1b << 6), 0x1b, 1);	/* пустой прямоугольник */
	hd44780_progbar(PATTERN_BAR_FULL + 1, (uint_fast8_t) (0x1b << 3), 0x1b, 1);
	hd44780_progbar(PATTERN_BAR_FULL + 2, (uint_fast8_t) (0x1b << 0), 0x1b, 1);	/* полносьтю заполненный прямоугольник */
#else
	#error CHARS2GRID(n) undefined
#endif
}
#endif /* WITHBARS */

static void
hd44780_powerondelay(void)
{
	local_delay_ms(10);
	local_delay_ms(10);
	local_delay_ms(10);
	local_delay_ms(10);
	local_delay_ms(10);
	local_delay_ms(10);
	local_delay_ms(10);

}
// architecture-independet function
// 
static void 
hd44780_initialize(void)
{
	hd44780_powerondelay();

#if ! LCDMODE_HARD_SPI
	/* при работе по паралельной шине */

	// Переключение дисплея из любого состояния в 4-х битную шину.
#if defined (WRITEE_BIT)
	if (!testdisplay())
	{
		hd44780_wrcmd4_nowait(0x03);
		hd44780_wrcmd4_nowait(0x03);
		hd44780_wrcmd4_nowait(0x02);
	}
	if (!testdisplay())
	{
		hd44780_wrcmd4_nowait(0x03);
		hd44780_wrcmd4_nowait(0x03);
		hd44780_wrcmd4_nowait(0x02);
	}
#else
	hd44780_wrcmd4_nowait(0x03);
	hd44780_wrcmd4_nowait(0x03);
	hd44780_wrcmd4_nowait(0x02);
#endif

	// Остальная инициализация
	hd44780_wrcmd(0x28);		// 4 bit, two lines

#else
	// Инициализация для режима работы SPI (8 бит, 2 строки).
	hd44780_wrcmd(0x38);		// 8 bit, two lines
#endif

	// Display enable command
	hd44780_wrcmd(0x17);		// WINSTAR OLED displays: character mode, power on
	hd44780_wrcmd(0x0c);     // Function Set: ВКЛ.LCD , КУРСОР ОТКЛ., МИГАНИЕ КУРС. ОТКЛ.

	hd44780_wrcmd(0x06);		// Entry Mode Set: автомат перемещение курсора вправо


#if WITHBARS
	hd44780_initbar();		// заполнение изображений передопредлённых символов.
#endif /* WITHBARS */
	hd44780_wrcmd(0x02);		// home


	display_gotoxy(0, 0);	/* переключение на работу с DDRAM - хоть и делается при инициализации BARS, без BARS нужно. */
	display_clear();	// обязательно требуется для нормальной работы, например, адресации.

	// тестовая выдача
	hd44780_wrdata_slow('O');
	hd44780_wrdata_slow('K');
	hd44780_wrdata_slow('>');

	// тест для контроля правильности подбора задержек
#if 0
	for (;;)
	{
		LCD_STROBE_PORT_S(LCD_STROBE_BIT); 	// activate EN signal
		hardware_spi_io_delay();
		local_delay_ms(1);
		LCD_STROBE_PORT_C(LCD_STROBE_BIT); 	// disactivate EN signal
		hardware_spi_io_delay();
		local_delay_ms(1);
	}
#endif
	//for (;;) ;
}


void display_set_contrast(uint_fast8_t v)
{
}

void 
display_clear(void)
{
	hd44780_clear();
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
	hd44780_put_char_begin();	/* Выдать CSB при использовании SPI интерфейса и два бита о записи данных */
}

void
display_wrdata_end(void)
{
	hd44780_put_char_end();
}

void
display_wrdatabar_begin(void)
{
}

void
display_wrdatabar_end(void)
{
}


void
display_wrdatabig_begin(void)
{
	hd44780_put_char_begin();	/* Выдать CSB при использовании SPI интерфейса и два бита о записи данных */
}


void
display_wrdatabig_end(void)
{
	hd44780_put_char_end();
}

/* отображение одной вертикальной полосы на графическом индикаторе */
/* старшие биты соответствуют верхним пикселям изображения */
/* вызывается между вызовами display_wrdatabar_begin() и display_wrdatabar_end() */
void 
display_barcolumn(uint_fast8_t pattern)
{
	hd44780_bar_column(pattern);
}

void
display_put_char_big(uint_fast8_t c, uint_fast8_t lowhalf)
{
	(void) lowhalf;
	hd44780_wrdata_fast(c);
}

void
display_put_char_half(uint_fast8_t c, uint_fast8_t lowhalf)
{
	(void) lowhalf;
	hd44780_wrdata_fast(c);
}


// Вызов этой функции только внутри display_wrdata_begin() и display_wrdata_end();
// Используется при выводе на графический ндикатор, если ТРЕБУЕТСЯ переключать полосы отображения
void
display_put_char_small(uint_fast8_t c, uint_fast8_t lowhalf)
{
	(void) lowhalf;
	hd44780_wrdata_fast(c);
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
	uint_fast8_t addr;	
	switch (y)
	{
	default:
	case 0:	addr = 0x00; break;
	case 1:	addr = 0x40; break;
#if LCDMODE_WH2004 /* || LCDMODE_WH1604 */		// для отладки - показать дизайн 1604 на индикаторе 2004
	case 2:	addr = 0x14; break;	// WINSTAR WH2004
	case 3:	addr = 0x54; break;	// WINSTAR WH2004
#elif LCDMODE_WH1604
	case 2:	addr = 0x10; break;	// WINSTAR WH1604
	case 3:	addr = 0x50; break;	// WINSTAR WH1604
#else
	case 2:	addr = 0x20; break;	// 20*4 specific
	case 3:	addr = 0x60; break;	// 20*4 specific
#endif
	}
	hd44780_gotoaddr(addr + x);
}

// Координаты в пикселях
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
}

void display_plotstart(
	uint_fast16_t dy	// Высота окна источника в пикселях
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

}


/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{
	hd44780_initialize();
}

/* Разряжаем конденсаторы питания */
void display_discharge(void)
{
}

#endif /* LCDMODE_HD44780 */
