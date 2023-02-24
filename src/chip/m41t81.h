/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Поддержка ST M41T81 real time clock
// M41T81M6,M41T81M6F
// M41T81SM6,M41T81SM6F
//
#ifndef M41T81_H_INCLUDED
#define M41T81_H_INCLUDED

#define M41T81_ADDRESS_W	0xD0	
#define M41T81_ADDRESS_R	(M41T81_ADDRESS_W | 0x01)

static void m41t81_readbuff(
	uint8_t * b,
	uint_fast8_t n,
	uint_fast8_t r
	)
{
#if WITHTWIHW
	uint8_t bufw = r;
	i2chw_write(M41T81_ADDRESS_W, & bufw, 1);
	i2chw_read(M41T81_ADDRESS_R, b, n);
#elif WITHTWISW
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
#endif
}

static void m41t81_writebuff(
	const uint8_t * b,
	uint_fast8_t n,
	uint_fast8_t r		// Addr
	)
{
#if WITHTWIHW
	uint8_t buff [n + 1];
	buff [0] = r;
	memcpy(buff + 1, b, n);
	i2chw_write(M41T81_ADDRESS_W, buff, n + 1);
#elif WITHTWISW
	i2c_start(M41T81_ADDRESS_W);
	i2c_write(r);	// register address
	while (n --)
		i2c_write(* b ++);
	i2c_waitsend();
	i2c_stop();
#endif
}


static void m41t81_setclearbit(
	uint_fast8_t r,		// Addr
	uint_fast8_t mask,	// биты, которые требуется модифицировать
	uint_fast8_t value)	// состояние битов, которое требуется установить.
{
	uint8_t b [1];

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

static uint_fast8_t
m41t81_min(uint_fast8_t a, uint_fast8_t b)
{
	return a < b ? a : b;
}

static uint_fast8_t
m41t81_max(uint_fast8_t a, uint_fast8_t b)
{
	return a > b ? a : b;
}

// input value 0x00..0x99, return value 0..99
static uint_fast8_t 
m41t81_bcd2bin(uint_fast8_t v, uint_fast8_t low, uint_fast8_t high)
{
	const div_t d = div(v, 16);
	return m41t81_max(m41t81_min(d.quot * 10 + d.rem, high), low);
}

// input value: 0..99, return value 0x00..0x99
static uint_fast8_t 
m41t81_bin2bcd(uint_fast8_t v, uint_fast8_t low, uint_fast8_t high)
{
	const div_t d = div(m41t81_max(m41t81_min(v, high), low), 10);
	return d.quot * 16 + d.rem;
}


void board_rtc_settime(
	uint_fast8_t hours,
	uint_fast8_t minutes,
	uint_fast8_t seconds
	)
{
	const uint_fast8_t rt = 0x01;	// Addr
	uint8_t bt [3];

	m41t81_readbuff(bt, sizeof bt / sizeof bt [0], rt);

	bt [0] = (bt [0] & ~ 0x7f) | (0x7f & m41t81_bin2bcd(seconds, 0, 59));	// r=1
	bt [1] = (bt [1] & ~ 0x7f) | (0x7f & m41t81_bin2bcd(minutes, 0, 59));	// r=2
	bt [2] = (bt [2] & ~ 0x3f) | (0x3f & m41t81_bin2bcd(hours, 0, 23));	// r=3

	m41t81_writebuff(bt, sizeof bt / sizeof bt [0], rt);
}

void board_rtc_setdatetime(
	uint_fast16_t year,
	uint_fast8_t month,	// 1..12
	uint_fast8_t dayofmonth,
	uint_fast8_t hours,
	uint_fast8_t minutes,
	uint_fast8_t seconds
	)
{
	const uint_fast8_t rd = 0x05;	// Addr
	uint8_t bd [3];

	m41t81_readbuff(bd, sizeof bd / sizeof bd [0], rd);

	bd [2] = m41t81_bin2bcd(year % 100, 0, 99);							// r=7
	bd [1] = (bd [1] & ~ 0x01f) | (0x01f & m41t81_bin2bcd(month, 1, 12));	// r=6 01-12
	bd [0] = (bd [0] & ~ 0x3f) | (0x3f & m41t81_bin2bcd(dayofmonth, 1, 31));		// r=5

	m41t81_writebuff(bd, sizeof bd / sizeof bd [0], rd);

	const uint_fast8_t rt = 0x01;	// Addr
	uint8_t bt [3];

	m41t81_readbuff(bt, sizeof bt / sizeof bt [0], rt);

	bt [0] = (bt [0] & ~ 0x7f) | (0x7f & m41t81_bin2bcd(seconds, 0, 59));	// r=1
	bt [1] = (bt [1] & ~ 0x7f) | (0x7f & m41t81_bin2bcd(minutes, 0, 59));	// r=2
	bt [2] = (bt [2] & ~ 0x3f) | (0x3f & m41t81_bin2bcd(hours, 0, 23));		// r=3

	m41t81_writebuff(bt, sizeof bt / sizeof bt [0], rt);

}


void board_rtc_setdate(
	uint_fast16_t year,
	uint_fast8_t month,	// 1..12
	uint_fast8_t dayofmonth
	)
{
	const uint_fast8_t r = 0x05;	// Addr
	uint8_t b [3];

	m41t81_readbuff(b, sizeof b / sizeof b[0], r);

	b [2] = m41t81_bin2bcd(year % 100, 0, 99);							// r=7
	b [1] = (b [1] & ~ 0x01f) | (0x01f & m41t81_bin2bcd(month, 1, 12));	// r=6 01-12
	b [0] = (b [0] & ~ 0x3f) | (0x3f & m41t81_bin2bcd(dayofmonth, 1, 31));		// r=5

	m41t81_writebuff(b, sizeof b / sizeof b[0], r);
}

void board_rtc_getdate(
	uint_fast16_t * year,
	uint_fast8_t * month,
	uint_fast8_t * dayofmonth
	)
{
	const uint_fast8_t r = 0x05;	// Addr
	uint8_t b [3];

	m41t81_readbuff(b, sizeof b / sizeof b[0], r);

	* year = 2000 + m41t81_bcd2bin(b [2], 0, 99);		// r=7
	* month = m41t81_bcd2bin(b [1] & 0x1f, 1, 12);	// r=6 01-12
	* dayofmonth = m41t81_bcd2bin(b [0] & 0x3f, 1, 31);		// r=5
}

void board_rtc_gettime(
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * seconds
	)
{
	const uint_fast8_t r = 0x01;	// Addr
	uint8_t b [3];

	m41t81_readbuff(b, sizeof b / sizeof b[0], r);

	* hour = m41t81_bcd2bin(b [2] & 0x3f, 0, 23);		// r=3
	* minute = m41t81_bcd2bin(b [1] & 0x7f, 0, 59);	// r=2
	* seconds = m41t81_bcd2bin(b [0] & 0x7f, 0, 59);	// r=1
}

void board_rtc_getdatetime(
	uint_fast16_t * year,
	uint_fast8_t * month,	// 01-12
	uint_fast8_t * dayofmonth,
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * seconds
	)
{
	const uint_fast8_t r = 0x01;
	uint8_t b [7];

	m41t81_readbuff(b, sizeof b / sizeof b[0], r);

	* year = 2000 + m41t81_bcd2bin(b [6], 0, 99);		// r=7
	* month = m41t81_bcd2bin(b [5] & 0x1f, 1, 12);		// r=6
	* dayofmonth = m41t81_bcd2bin(b [4] & 0x3f, 1, 31);		// r=5
	* hour = m41t81_bcd2bin(b [2] & 0x3f, 0, 23);		// r=3
	* minute = m41t81_bcd2bin(b [1] & 0x7f, 0, 59);	// r=2
	* seconds = m41t81_bcd2bin(b [0] & 0x7f, 0, 59);	// r=1
}

/* возврат не-0 если требуется начальная загрузка значений */
uint_fast8_t board_rtc_chip_initialize(void)
{
	// Write RTC calibration value (0x00..0x1f)
	uint8_t rtc_calibratioin = 0x10;

	m41t81_setclearbit(0x08, 0x1f, rtc_calibratioin);
	m41t81_setclearbit(0x0c, 0x40, 0x00);	// ht=0
	m41t81_setclearbit(0x01, 0x80, 0x00);	// st=0
	m41t81_setclearbit(0x03, 0xc0, 0xc0);	// CB=1, CEB=1

	return 0;
}

#endif /* M41T81_H_INCLUDED */
