/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Поддержка MAXIM DS1305EN RTC clock chip with SPI interface
// SERMODE pin tied high.
//
#ifndef DS1305_H_INCLUDED
#define DS1305_H_INCLUDED

#define DS1305_SPIMODE SPIC_MODE3
#define DS1305_SPISPEED	SPIC_SPEED400k

enum
{
	DS1305REG_TIME = 0,
	DS1305REG_DAYOFMONTH = 4,
	DS1305REG_CONTROL = 15,	// Control Register
	DS1305REG_TCS = 17		// Trickle Charger Register
};

static void ds1305_readbuff(
	uint8_t * data,
	uint_fast8_t len,
	uint_fast8_t addr		// Addr
	)
{
	const spitarget_t target = targetrtc1;		/* addressing to chip */

	local_delay_us(10);		// 4 uS required
	spi_select2(target, DS1305_SPIMODE, DS1305_SPISPEED);
	local_delay_us(10);		// 4 uS required

	spi_progval8_p1(target, addr & 0x7F);	// D7=0: read mode
	spi_complete(target);

	spi_to_read(target);

	prog_spi_read_frame(target, data, len);

	spi_to_write(target);
	spi_unselect(target);	/* done sending data to target chip */
}

static void ds1305_writebuff(
	const uint8_t * data,
	uint_fast8_t len,
	uint_fast8_t addr		// Addr
	)
{
	const spitarget_t target = targetrtc1;		/* addressing to chip */

	local_delay_us(10);		// 4 uS required
	spi_select2(target, DS1305_SPIMODE, DS1305_SPISPEED);
	local_delay_us(10);		// 4 uS required

	spi_progval8_p1(target, addr | 0x80);	// D7=1: write mode
	spi_complete(target);
	prog_spi_send_frame(target, data, len);

	spi_unselect(target);	/* done sending data to target chip */
}

#if 0

static void ds1305_setclearbit(
	uint_fast8_t r,		// Addr
	uint_fast8_t mask,	// биты, которые требуется модифицировать
	uint_fast8_t value)	// состояние битов, которое требуется установить.
{
	uint8_t b [1];

	ds1305_readbuff(b, sizeof b / sizeof b[0], r);

	b [0] = (b [0] & ~ mask) | (mask & value);

	ds1305_writebuff(b, sizeof b / sizeof b[0], r);
}

static uint_fast8_t ds1305_testbit(
	uint_fast8_t r,			// регистр RTC
	uint_fast8_t mask	// состояние битов, которое требуется проверить.
	)	
{
	uint_fast8_t b [1];

	ds1305_readbuff(b, sizeof b / sizeof b[0], r);

	return (b [0] & mask) != 0;
}

#endif

// input value 0x00..0x99, return value 0..99
static uint_fast8_t 
ds1305_bcd2bin(uint_fast8_t v)
{
	const div_t d = div(v, 16);
	return d.quot * 10 + d.rem;
}

// input value: 0..99, return value 0x00..0x99
static uint_fast8_t 
ds1305_bin2bcd(uint_fast8_t v)
{
	const div_t d = div(v, 10);
	return d.quot * 16 + d.rem;
}

void board_rtc_settime(
	uint_fast8_t hours,
	uint_fast8_t minutes,
	uint_fast8_t secounds
	)
{
	const uint_fast8_t rt = DS1305REG_TIME;	// Addr
	uint8_t bt [3];
	static const uint8_t b_WP [1] = { 0x40 };
	static const uint8_t b_WE [1] = { 0x00 };

	bt [0] = 0x7f & ds1305_bin2bcd(secounds);	// r=0
	bt [1] = 0x7f & ds1305_bin2bcd(minutes);	// r=1
	bt [2] = 0x3f & ds1305_bin2bcd(hours);		// r=2

	// write enable
	//ds1305_writebuff(b_WE, sizeof b_WE / sizeof b_WE [0], DS1305REG_CONTROL);
	// write data
	ds1305_writebuff(bt, sizeof bt / sizeof bt [0], rt);
	// write protect
	//ds1305_writebuff(b_WP, sizeof b_WP / sizeof b_WP [0], DS1305REG_CONTROL);
}

void board_rtc_setdatetime(
	uint_fast16_t year,
	uint_fast8_t month,
	uint_fast8_t dayofmonth,
	uint_fast8_t hours,
	uint_fast8_t minutes,
	uint_fast8_t secounds
	)
{
	static const uint8_t b_WP [1] = { 0x40 };
	static const uint8_t b_WE [1] = { 0x00 };
	const uint_fast8_t rt = DS1305REG_TIME;	// Addr
	uint8_t bt [3];

	bt [0] = 0x7f & ds1305_bin2bcd(secounds);	// r=0
	bt [1] = 0x7f & ds1305_bin2bcd(minutes);	// r=1
	bt [2] = 0x3f & ds1305_bin2bcd(hours);		// r=2

	// write enable
	//ds1305_writebuff(b_WE, sizeof b_WE / sizeof b_WE [0], DS1305REG_CONTROL);

	// write data
	ds1305_writebuff(bt, sizeof bt / sizeof bt [0], rt);

	// Writing the time and date registers within one second 
	// after writing the seconds register ensures consistent data.

	const uint_fast8_t rd = DS1305REG_DAYOFMONTH;	// Addr
	uint8_t bd [3];

	bd [0] = 0x3f & ds1305_bin2bcd(dayofmonth);	// r=4
	bd [1] = 0x01f & ds1305_bin2bcd(month);		// r=5
	bd [2] = ds1305_bin2bcd(year % 100);		// r=6

	ds1305_writebuff(bd, sizeof bd / sizeof bd [0], rd);
	// write protect
	//ds1305_writebuff(b_WP, sizeof b_WP / sizeof b_WP [0], DS1305REG_CONTROL);
}


void board_rtc_setdate(
	uint_fast16_t year,
	uint_fast8_t month,
	uint_fast8_t dayofmonth
	)
{
	const uint_fast8_t rd = DS1305REG_DAYOFMONTH;	// Addr
	uint8_t bd [3];
	static const uint8_t b_WP [1] = { 0x40 };
	static const uint8_t b_WE [1] = { 0x00 };

	bd [0] = 0x3f & ds1305_bin2bcd(dayofmonth);	// r=4
	bd [1] = 0x01f & ds1305_bin2bcd(month);		// r=5
	bd [2] = ds1305_bin2bcd(year % 100);		// r=6

	// write enable
	//ds1305_writebuff(b_WE, sizeof b_WE / sizeof b_WE [0], DS1305REG_CONTROL);
	// write data
	ds1305_writebuff(bd, sizeof bd / sizeof bd [0], rd);
	// write protect
	//ds1305_writebuff(b_WP, sizeof b_WP / sizeof b_WP [0], DS1305REG_CONTROL);
}

void board_rtc_getdate(
	uint_fast16_t * year,
	uint_fast8_t * month,
	uint_fast8_t * dayofmonth
	)
{
	const uint_fast8_t r = DS1305REG_DAYOFMONTH;	// Addr
	uint8_t b [3];

	ds1305_readbuff(b, sizeof b / sizeof b[0], r);

	* dayofmonth = ds1305_bcd2bin(b [0] & 0x3f);		// r=5
	* month = ds1305_bcd2bin(b [1] & 0x1f);	// r=6
	* year = 2000 + ds1305_bcd2bin(b [2]);		// r=7
}

void board_rtc_gettime(
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * secounds
	)
{
	const uint_fast8_t r = DS1305REG_TIME;	// Addr
	uint8_t b [3];

	ds1305_readbuff(b, sizeof b / sizeof b[0], r);

	//debug_printf_P(PSTR("board_rtc_gettime: %02X:%02X:%02X\n"), b [0], b [1], b [2]);

	* secounds = ds1305_bcd2bin(b [0] & 0x7f);	// r=1
	* minute = ds1305_bcd2bin(b [1] & 0x7f);	// r=2
	* hour = ds1305_bcd2bin(b [2] & 0x3f);		// r=3
}

void board_rtc_getdatetime(
	uint_fast16_t * year,
	uint_fast8_t * month,	// 01-12
	uint_fast8_t * dayofmonth,
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * secounds
	)
{
	const uint_fast8_t r = DS1305REG_TIME;
	uint8_t b [7];

	ds1305_readbuff(b, sizeof b / sizeof b[0], r);

	* secounds = ds1305_bcd2bin(b [0] & 0x7f);	// r=0
	* minute = ds1305_bcd2bin(b [1] & 0x7f);	// r=1
	* hour = ds1305_bcd2bin(b [2] & 0x3f);		// r=2
	* dayofmonth = ds1305_bcd2bin(b [4] & 0x3f);		// r=4
	* month = ds1305_bcd2bin(b [5] & 0x1f);		// r=5
	* year = 2000 + ds1305_bcd2bin(b [6]);		// r=6
}

/* возврат не-0 если требуется начальная загрузка значений */
uint_fast8_t board_rtc_chip_initialize(void)
{
	uint_fast8_t eosc;

	//hardware_spi_master_setfreq(SPIC_SPEED400k, SPISPEED400k);	// Slow clock speed (for chips like LM7001 or DS1305EN)
	//hardware_spi_master_setfreq(SPIC_SPEED100k, SPISPEED100k);	// Slow clock speed (for chips like LM7001 or DS1305EN)

	uint8_t b [1];
	static const uint8_t b0 [1] = { 0 };
	static const uint8_t b_WP [1] = { 0x40 };
	static const uint8_t b_WE [1] = { 0x00 };

	ds1305_readbuff(b, sizeof b / sizeof b[0], DS1305REG_CONTROL);
	eosc = (b [0] & 0x80) != 0;
	if (eosc != 0)
	{
		// Initial application of power,

		// write enable
		//ds1305_writebuff(b_WE, sizeof b_WE / sizeof b_WE [0], DS1305REG_CONTROL);

		//local_delay_ms(50);

		// enable oscillator
		ds1305_writebuff(b0, sizeof b0 / sizeof b0 [0], DS1305REG_CONTROL);

		local_delay_ms(50);

		// disable Trickle Charger
		//ds1305_writebuff(b0, sizeof b0 / sizeof b0 [0], DS1305REG_TCS);

	}

	// write protect
	//ds1305_writebuff(b_WP, sizeof b_WP / sizeof b_WP [0], DS1305REG_CONTROL);

	debug_printf_P(PSTR("board_rtc_chip_initialize: eosc=%d\n"), (int) eosc);
	return eosc;
}

#endif /* DS1305_H_INCLUDED */
