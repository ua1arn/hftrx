/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#ifndef AD9852_C_INCLUDED
#define AD9852_C_INCLUDED


#define OUTSHAPE9852 0	/* Use OSK multiplier with 0x0fc0 */
#define OUTSHAPE9852VAL 0x0400 //0x0fc0

/* programming AD9852/AD9854 */

static void prog_ad9852(
	spitarget_t target,		/* addressing to chip */
	unsigned char address,	/* serial adress value */
	const phase_t * val,
	uint_fast8_t n				/* number of bits to send */
	)
{
	prog_select(target);	/* start sending data to target chip */

	prog_val(target, 0, 4);		/* write, x, x, x */
	prog_val(target, address, 4);		/* A3 ,A2, A1, A0 */
	prog_phbits(target, val, n, n);

	prog_unselect(target);	/* done sending data to target chip */

}

static void prog_ad9852_fill(
	spitarget_t target,		/* addressing to chip */
	unsigned char address,	/* serial adress value */
	const phase_t * val,
	uint_fast8_t n,			/* number of leading bits to send */
	uint_fast8_t z			/* number of trailing zeroes to send */
	)
{
	prog_select(target);	/* start sending data to target chip */

	prog_val(target, 0, 4);		/* write, x, x, x */
	prog_val(target, address, 4);		/* A3 ,A2, A1, A0 */
	prog_phbits(target, val, n, n);
	prog_phbits(target, & phase_0, z, z);

	prog_unselect(target);	/* done sending data to target chip */

}

/* set frequency tuning word1 to DDS */
static void prog_ad9852_freq1(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val
	)
{
	enum { r = FTW_RESOLUTION < 48 ? FTW_RESOLUTION : 48 };
	enum { z = 48 - r };

	prog_ad9852_fill(target, 0x02, val, r, z);

}


/* set update clock word to DDS */
static void prog_ad9852_update_clock(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val
	)
{
	prog_ad9852(target, 0x05, val, 32);
}


/* set control word to DDS */
static void prog_ad9852_control_word(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val
	)
{
	prog_ad9852(target, 0x07, val, 32);
}

#if 0

/* set frequency tuning word2 to DDS */
static void prog_ad9852_freq2(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val
	)
{
	prog_ad9852_fill(target, 0x03, val, FTW_RESOLUTION, 48 - FTW_RESOLUTION);
}


/* set delta frequency tuning word to DDS */
static void prog_ad9852_delta_freq(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val
	)
{
	prog_ad9852_fill(target, 0x04, val, FTW_RESOLUTION, 48 - FTW_RESOLUTION);
}

/* set ramp rate clock word to DDS */
static void prog_ad9852_ramp_rate_clock(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val
	)
{
	prog_ad9852(target, 0x06, val, 24);
}


/* set digital multiplier to DDS */
/* 12 bit of value has meaning	*/
static void prog_ad9852_output_shape_key_I(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val
	)
{
	prog_ad9852(target, 0x08, val, 16);
}

// ad9854 specific
/* 12 bit of value has meaning	*/
static void prog_ad9852_output_shape_key_Q(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val
	)
{
	prog_ad9852(target, 0x09, val, 16);
}

// ad9854 specific
/* Q DAC register	*/
static void prog_ad9852_Q_DAC_register(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val
	)
{
	prog_ad9852(target, 0x0b, val, 16);
}

#endif

/* complex init */


static void 
//NOINLINEAT
prog_ad9852_init(
	spitarget_t target,		/* addressing to chip */
	unsigned char powerdown,
	unsigned fqscale
			 )
{
	// установка режимов ad9854
	// 52 и 54 микросхемы немного отличаются
	if (powerdown != 0)
	{
		/* sleep mode - offline everything */
		phase_t ctl;
		phase_fromulong(& ctl, 
			(0x17UL << 24) |		// init - 0x10
			(0x20UL << 16) |		// init - 0x64
			(0x00UL << 8) |		// init - 0x01	10 - qdac 00 - control dac
			0x40UL //(0x60 << 0)			// 0x40 - bypass inv sinc, 0x20 - osk enable
			);

		prog_ad9852_control_word(target, & ctl);
		prog_pulse_ioupdate();
#if 0
		// так как мы не переключаемся в режим со внешним 
		// io update clock, второе программирование не нужно.
		// но все-таки делаем его
		prog_ad9852_control_word(target, & ctl);
		prog_pulse_ioupdate();
#endif
	}
	else
	{
		phase_t ctl;
		if (fqscale == 1)
		{
			/* no internal multiplier */
			phase_fromulong(& ctl, 
				(0x14L << 24) |		// init - 0x10
				(0x64L << 16) |		// 0x20 - bypass PLL init - 0x64
				(0x00L << 8) |		// init - 0x01	10 - qdac 00 - control dac
#if OUTSHAPE9852
				0x60L //(0x60 << 0)			// 0x40 - bypass inv sinc, 0x20 - osk enable
#else
				0x40L //(0x60 << 0)			// 0x40 - bypass inv sinc, 0x20 - osk enable
#endif
				);
		}
		else
		{
			/* With internal multiplier */
			phase_fromulong(& ctl, 
				(0x14L << 24) |		// init - 0x10
				((0x00L | fqscale) << 16) |		// low VCO range (below 200 MHz)
				(0x00L << 8) |		// init - 0x01	10 - qdac 00 - control dac
#if OUTSHAPE9852
				0x60L //(0x60 << 0)			// 0x40 - bypass inv sinc, 0x20 - osk enable
#else
				0x40L //(0x60 << 0)			// 0x40 - bypass inv sinc, 0x20 - osk enable
#endif
				);
		}
		prog_ad9852_control_word(target, & ctl);
		prog_pulse_ioupdate();
#if 1
		// так как мы не переключаемся в режим со внешним 
		// io update clock, второе программирование не нужно.
		// но все-таки делаем его
		prog_ad9852_control_word(target, & ctl);
		prog_pulse_ioupdate();
#endif
	}

	// Программируем update clock rate
//#if TESTPOINTS
	{
		phase_t upd;

		phase_fromuint(& upd, 0x40);
		prog_ad9852_update_clock(target, & upd);
		prog_pulse_ioupdate();
	}
//#endif	
	// установить выходной уровень
#if OUTSHAPE9852
	{
		phase_t dmult;
		//
		// такое значение рекомендовано вместо 0x0fff
		// как дающее меньший уровень побочный сигналов
		//
		phase_fromuint(& dmult, OUTSHAPE9852VAL);
		prog_ad9852_output_shape_key_I(target, & dmult);
		// ad9854 specific
		//prog_ad9852_output_shape_key_Q(target, & dmult);
		prog_pulse_ioupdate();
	}
#endif

}

#endif /* AD9852_C_INCLUDED */

