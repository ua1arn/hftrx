/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Поддержка ST M41T81 real time clock
//
#ifndef M41T81_H_INCLUDED
#define M41T81_H_INCLUDED

#define M41T81_ADDRESS_W	0xD0	
#define M41T81_ADDRESS_R	(M41T81_ADDRESS_W | 0x01)

static void m41t81_readbuff(
	uint_fast8_t * b,
	uint_fast8_t n,
	uint_fast8_t r
	)
{
	i2c_start(M41T81_ADDRESS_W);
	i2c_write_withrestart(r);	// register address
	i2c_start(M41T81_ADDRESS_R);
	if (n == 1)
	{
		i2c_read(b, I2C_READ_ACK_NACK);
	}
	else
	{
		i2c_read(b ++, I2C_READ_ACK_1);
		n -= 2;
		while (n --)
			i2c_read(b ++, I2C_READ_ACK);
		i2c_read(b ++, I2C_READ_NACK);
	}
}

static void m41t81_writebuff(
	const uint_fast8_t * b,
	uint_fast8_t n,
	uint_fast8_t r		// Addr
	)
{
	i2c_start(M41T81_ADDRESS_W);
	i2c_write(r);	// register address
	while (n --)
		i2c_write(* b ++);
	i2c_waitsend();
	i2c_stop();
}


static void m41t81_setclearbit(
	uint_fast8_t r,		// Addr
	uint_fast8_t mask,	// биты, которые требуется модифицировать
	uint_fast8_t value)	// состояние битов, которое требуется установить.
{
	uint_fast8_t b [1];

	m41t81_readbuff(b, sizeof b / sizeof b[0], r);

	b [0] = (b [0] & ~ mask) | (mask & value);

	m41t81_writebuff(b, sizeof b / sizeof b[0], r);
}

/*
static uint_fast8_t m41t81_testbit(
	uint_fast8_t r,			// регистр RTC
	uint_fast8_t mask	// состояние битов, которое требуется проверить.
	)	
{
	uint_fast8_t b [1];

	m41t81_readbuff(b, sizeof b / sizeof b[0], r);

	return (b [0] & mask) != 0;
}
*/

// input value 0x00..0x99, return value 0..99
static uint_fast8_t 
m41t81_bcd2bin(uint_fast8_t v)
{
	const div_t d = div(v, 16);
	return d.quot * 10 + d.rem;
}

// input value: 0..99, return value 0x00..0x99
static uint_fast8_t 
m41t81_bin2bcd(uint_fast8_t v)
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
	const uint_fast8_t rt = 0x01;	// Addr
	uint_fast8_t bt [3];

	m41t81_readbuff(bt, sizeof bt / sizeof bt [0], rt);

	bt [0] = (bt [0] & ~ 0x7f) | (0x7f & m41t81_bin2bcd(secounds));	// r=1
	bt [1] = (bt [1] & ~ 0x7f) | (0x7f & m41t81_bin2bcd(minutes));	// r=2
	bt [2] = (bt [2] & ~ 0x3f) | (0x3f & m41t81_bin2bcd(hours));	// r=3

	m41t81_writebuff(bt, sizeof bt / sizeof bt [0], rt);
}

void board_rtc_setdatetime(
	uint_fast16_t year,
	uint_fast8_t month,	// 1..12
	uint_fast8_t dayofmonth,
	uint_fast8_t hours,
	uint_fast8_t minutes,
	uint_fast8_t secounds
	)
{
	const uint_fast8_t rd = 0x05;	// Addr
	uint_fast8_t bd [3];

	m41t81_readbuff(bd, sizeof bd / sizeof bd [0], rd);

	bd [2] = m41t81_bin2bcd(year % 100);							// r=7
	bd [1] = (bd [1] & ~ 0x01f) | (0x01f & m41t81_bin2bcd(month));	// r=6 01-12
	bd [0] = (bd [0] & ~ 0x3f) | (0x3f & m41t81_bin2bcd(dayofmonth));		// r=5

	m41t81_writebuff(bd, sizeof bd / sizeof bd [0], rd);

	const uint_fast8_t rt = 0x01;	// Addr
	uint_fast8_t bt [3];

	m41t81_readbuff(bt, sizeof bt / sizeof bt [0], rt);

	bt [0] = (bt [0] & ~ 0x7f) | (0x7f & m41t81_bin2bcd(secounds));	// r=1
	bt [1] = (bt [1] & ~ 0x7f) | (0x7f & m41t81_bin2bcd(minutes));	// r=2
	bt [2] = (bt [2] & ~ 0x3f) | (0x3f & m41t81_bin2bcd(hours));		// r=3

	m41t81_writebuff(bt, sizeof bt / sizeof bt [0], rt);

}


void board_rtc_setdate(
	uint_fast16_t year,
	uint_fast8_t month,	// 1..12
	uint_fast8_t dayofmonth
	)
{
	const uint_fast8_t r = 0x05;	// Addr
	uint_fast8_t b [3];

	m41t81_readbuff(b, sizeof b / sizeof b[0], r);

	b [2] = m41t81_bin2bcd(year % 100);							// r=7
	b [1] = (b [1] & ~ 0x01f) | (0x01f & m41t81_bin2bcd(month));	// r=6 01-12
	b [0] = (b [0] & ~ 0x3f) | (0x3f & m41t81_bin2bcd(dayofmonth));		// r=5

	m41t81_writebuff(b, sizeof b / sizeof b[0], r);
}

void board_rtc_getdate(
	uint_fast16_t * year,
	uint_fast8_t * month,
	uint_fast8_t * dayofmonth
	)
{
	const uint_fast8_t r = 0x05;	// Addr
	uint_fast8_t b [3];

	m41t81_readbuff(b, sizeof b / sizeof b[0], r);

	* year = 2000 + m41t81_bcd2bin(b [2]);		// r=7
	* month = m41t81_bcd2bin(b [1] & 0x1f);	// r=6 01-12
	* dayofmonth = m41t81_bcd2bin(b [0] & 0x3f);		// r=5
}

void board_rtc_gettime(
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * secounds
	)
{
	const uint_fast8_t r = 0x01;	// Addr
	uint_fast8_t b [3];

	m41t81_readbuff(b, sizeof b / sizeof b[0], r);

	* hour = m41t81_bcd2bin(b [2] & 0x3f);		// r=3
	* minute = m41t81_bcd2bin(b [1] & 0x7f);	// r=2
	* secounds = m41t81_bcd2bin(b [0] & 0x7f);	// r=1
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
	const uint_fast8_t r = 0x01;
	uint_fast8_t b [7];

	m41t81_readbuff(b, sizeof b / sizeof b[0], r);

	* year = 2000 + m41t81_bcd2bin(b [6]);		// r=7
	* month = m41t81_bcd2bin(b [5] & 0x1f);		// r=6
	* dayofmonth = m41t81_bcd2bin(b [4] & 0x3f);		// r=5
	* hour = m41t81_bcd2bin(b [2] & 0x3f);		// r=3
	* minute = m41t81_bcd2bin(b [1] & 0x7f);	// r=2
	* secounds = m41t81_bcd2bin(b [0] & 0x7f);	// r=1
}

/* возврат не-0 если требуется начальная загрузка значений */
uint_fast8_t board_rtc_chip_initialize(void)
{
	// Write RTC calibration value (0x00..0x1f)
	uint_fast8_t rtc_calibratioin = 0x10;

	m41t81_setclearbit(0x08, 0x1f, rtc_calibratioin);
	m41t81_setclearbit(0x0c, 0x40, 0x00);	// ht=0
	m41t81_setclearbit(0x01, 0x80, 0x00);	// st=0
	m41t81_setclearbit(0x03, 0xc0, 0xc0);	// CB=1, CEB=1

	return 0;
}

#endif /* M41T81_H_INCLUDED */
