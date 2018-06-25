/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Поддержка DS1307/DS3231 real time clock - tnx for UR3QTD
//
#ifndef M41T81_H_INCLUDED
#define M41T81_H_INCLUDED

#define DS1307_ADDRESS_W	0xD0
#define DS1307_ADDRESS_R	0xD1

static void ds1307_readbuff(
	uint_fast8_t * b,
	uint_fast8_t n,
	uint_fast8_t r
	)
{
	i2c_start(DS1307_ADDRESS_W);
	i2c_write_withrestart(r);	// register address
	//i2c_write(r);
	i2c_start(DS1307_ADDRESS_R);
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

static void ds1307_writebuff(
	const uint_fast8_t * b,
	uint_fast8_t n,
	uint_fast8_t r		// Addr
	)
{
	i2c_start(DS1307_ADDRESS_W);
	i2c_write(r);	// register address
	while (n --)
		i2c_write(* b ++);
	i2c_waitsend();
	i2c_stop();
}

// input value 0x00..0x99, return value 0..99
static uint_fast8_t ds1307_bcd2bin(uint_fast8_t v)
{
	const div_t d = div(v, 16);
	return d.quot * 10 + d.rem;
}

// input value: 0..99, return value 0x00..0x99
static uint_fast8_t ds1307_bin2bcd(uint_fast8_t v)
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
	i2c_start(DS1307_ADDRESS_W);
	i2c_write(0x00);	// register address
	i2c_write(ds1307_bin2bcd(secounds));
	i2c_write(ds1307_bin2bcd(minutes));
	i2c_write(ds1307_bin2bcd(hours));
	i2c_waitsend();
	i2c_stop();
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
	i2c_start(DS1307_ADDRESS_W);
	i2c_write(0x00);	// register address
	i2c_write(ds1307_bin2bcd(secounds));	// 0
	i2c_write(ds1307_bin2bcd(minutes));		// 1
	i2c_write(ds1307_bin2bcd(hours));		// 2
	i2c_write(0x02);						// 3
	i2c_write(ds1307_bin2bcd(dayofmonth));	// 4
	i2c_write(ds1307_bin2bcd(month));		// 5
	i2c_write(ds1307_bin2bcd(year % 100));	// 6 year
	i2c_waitsend();
	i2c_stop();
}


void board_rtc_setdate(
	uint_fast16_t year,
	uint_fast8_t month,	// 1..12
	uint_fast8_t dayofmonth
	)
{
	i2c_start(DS1307_ADDRESS_W);
	i2c_write(0x04);	// register address
	i2c_write(ds1307_bin2bcd(dayofmonth));
	i2c_write(ds1307_bin2bcd(month));
	i2c_write(ds1307_bin2bcd(year % 100));
	i2c_waitsend();
	i2c_stop();
}

void board_rtc_getdate(
	uint_fast16_t * year,
	uint_fast8_t * month,
	uint_fast8_t * dayofmonth
	)
{
	const uint_fast8_t r = 0x04;	// Addr
	uint_fast8_t b [3];

	ds1307_readbuff(b, sizeof b / sizeof b [0], r);

	* year = 2000 + ds1307_bcd2bin(b [2]);		// r=6
	* month = ds1307_bcd2bin(b [1]);	// r=5 01-12
	* dayofmonth = ds1307_bcd2bin(b [0]);		// r=4
}

void board_rtc_gettime(
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * secounds
	)
{
	const uint_fast8_t r = 0x00;	// Addr
	uint_fast8_t b [3];

	ds1307_readbuff(b, sizeof b / sizeof b [0], r);

	* hour = ds1307_bcd2bin(b [2]);		// r=2
	* minute = ds1307_bcd2bin(b [1]);	// r=1
	* secounds = ds1307_bcd2bin(b [0]);	// r=0
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
	const uint_fast8_t r = 0x00;
	uint_fast8_t b [7];

	ds1307_readbuff(b, sizeof b / sizeof b [0], r);

	* year = 2000 + ds1307_bcd2bin(b [6]);// r=6
	* month = ds1307_bcd2bin(b [5]);		// r=5
	* dayofmonth = ds1307_bcd2bin(b [4]);// r=4
	* hour = ds1307_bcd2bin(b [2]);		// r=2
	* minute = ds1307_bcd2bin(b [1]);	// r=1
	* secounds = ds1307_bcd2bin(b [0]);	// r=0
}

/* возврат не-0 если требуется начальная загрузка значений */
uint_fast8_t board_rtc_chip_initialize(void)
{
	//uint_fast8_t isec;
	i2c_start(DS1307_ADDRESS_W);
	i2c_write(0x0E); // r=14 DS3231
	i2c_write(0x00); //EOSC=0,BBSQW=0,CONV=0,RS2=0,RS1=0,INTCN=0,A2IE=0,A1IE=0
	i2c_stop();
		
	return 0;
}

#endif /* M41T81_H_INCLUDED */
