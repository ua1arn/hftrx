/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#ifndef AD9857_C_INCLUDED
#define AD9857_C_INCLUDED

/* programming AD9857 */
static void prog_ad9857(
	spitarget_t target,		/* addressing to chip */
	unsigned char address,	/* serial adress value */
	const phase_t * val,
	uint_fast8_t n				/* number of bits to send */
	)
{
	uint_fast8_t length;	/* length code */

	switch (n)
	{
	case 32:	length = 3; break;
	case 24:	length = 2; break;
	case 16:	length = 1; break;

	case 8:		
	default:
				length = 0; break;
	}
	prog_select(target);	/* start sending data to target chip */

   	prog_val(target, length, 3);		/* write, x, x, x */
	prog_val(target, address, 5);		/* a4, A3 ,A2, A1, A0 */

	prog_phbits(target, val, n, n);

	prog_unselect(target);	/* done sending data to target chip */

}

/* set frequency tuning word1 to DDS */
static void prog_ad9857_freq(
	spitarget_t target,		/* addressing to chip */
	unsigned char bank,
	const phase_t * val
	)
{
	/* NOTE: addres is auto-decrements (MSB ACTIVE) */
	prog_ad9857(target, (bank * 6 + 0x02) + 0x03, val, 32);
}

/* set frequency tuning word1 to DDS */
static void prog_ad9857_ouputscale(
	spitarget_t target,		/* addressing to chip */
	unsigned char bank,
	unsigned char value		// defaut value in bank 0 - 0xb5
	)
{
	phase_t val;
	phase_fromuint(& val, value);
	prog_ad9857(target, (bank * 6 + 0x02) + 0x05, & val, 8);
}

static void prog_ad9857_init_nomult(
	spitarget_t target		/* addressing to chip */
	)
{
	phase_t val;

	phase_fromuint(& val, 
		0x00 | //0x00 |	/* 0x80 - sdio input only, 0x00 - bidirectional */
		0x20 | /* 0x20 - pll lock control */
		0x01	/* PLL multiplier, 0x01 - pll bypass */
	);
	// register 0 set
	prog_ad9857(target, 0x00, & val, 8);

	phase_fromuint(& val, 
		0x00 | //0x40 |		// 0x40 - inverse SINC bypass (for narrow-band generators)
		0x01		// 0x01 - single tone mode
		);
	// register 1 set
	prog_ad9857(target, 0x01, & val, 8);
}

static void prog_ad9857_init_mult(
	spitarget_t target,		/* addressing to chip */
	unsigned char multosc	/* on-chip clock multiplier used */
	)
{
	phase_t val;

	phase_fromuint(& val, 
		0x00 | //0x00 |	/* 0x80 - sdio input only, 0x00 - bidirectional */
		0x20 | /* 0x20 - pll lock control */
		multosc	/* PLL multiplier, 4..20, 0x01 - pll bypass */
	);
	// register 0 set
	prog_ad9857(target, 0x00, & val, 8);

	phase_fromuint(& val, 
		0x00 | //0x40 |		// 0x40 - inverse SINC bypass (for narrow-band generators)
		0x01		// 0x01 - single tone mode
		);
	// register 1 set
	prog_ad9857(target, 0x01, & val, 8);
}

#endif /* AD9857_C_INCLUDED */

