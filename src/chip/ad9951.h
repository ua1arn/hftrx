/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Провренно с AD9951, AD9954, AD9856
//
#ifndef AD9951_C_INCLUDED
#define AD9951_C_INCLUDED



#define AD9951_SPIMODE	SPIC_MODE3	// MODE2 & MODE3 tested
extern const phase_t phase_0;

//#define OUTSHAPE9951	1	/* Use OSK multiplier with OUTSHAPE9951VAL */
//#define OUTSHAPE9951VAL (0x3c00 / 1) // 14-bit value is a meaning

/* programming AD9951/AD9954 */
/* set frequency tuning word1 to DDS */
static void 
//NOINLINEAT
prog_ad9951_freq0(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val
	)
{
	enum { FTW0 = 0x04 };	/* Target register - FTW0 (0x04) */

#if FTW_RESOLUTION >= 32
	const uint_fast32_t v32 = * val >> (FTW_RESOLUTION - 32);
#else
	const uint_fast32_t v32 = * val << (32 - FTW_RESOLUTION);
#endif

	spi_select(target, AD9951_SPIMODE);	/* start sending data to target chip */

	spi_progval8_p1(target, FTW0);	/* write, address 4 */
	spi_progval8_p2(target, v32 >> 24);
	spi_progval8_p2(target, v32 >> 16);
	spi_progval8_p2(target, v32 >> 8);
	spi_progval8_p2(target, v32 >> 0);
	spi_complete(target);

	spi_unselect(target);	/* done sending data to target chip */
}

/* set control word CFR1 */
static void 
//NOINLINEAT
prog_ad9951_cfr1(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val
	)
{
	enum { CFR1 = 0x00 };	/* Target register - CFR1 (0x00) */
	const uint_fast32_t v32 = * val;

	spi_select(target, AD9951_SPIMODE);	/* start sending data to target chip */

	spi_progval8_p1(target, CFR1);	/* write, address 0 */
	spi_progval8_p2(target, v32 >> 24);
	spi_progval8_p2(target, v32 >> 16);
	spi_progval8_p2(target, v32 >> 8);
	spi_progval8_p2(target, v32 >> 0);
	spi_complete(target);

	spi_unselect(target);	/* done sending data to target chip */
}

/* set control word CFR2 */
static void 
//NOINLINEAT
prog_ad9951_cfr2(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val
	)
{
	enum { CFR2 = 0x01 };	/* Target register - CFR2 (0x01) */
	const uint_fast32_t v32 = * val;

	spi_select(target, AD9951_SPIMODE);	/* start sending data to target chip */

	spi_progval8_p1(target, CFR2);	/* write, address 1 */
	spi_progval8_p2(target, v32 >> 16);
	spi_progval8_p2(target, v32 >> 8);
	spi_progval8_p2(target, v32 >> 0);
	spi_complete(target);

	spi_unselect(target);	/* done sending data to target chip */
}

#if OUTSHAPE9951 || WITHLO1LEVELADJ

/* set control word ASF - Amplitude Scale Factor */
/* 12 bit of value has meaning	*/
static void 
//NOINLINEAT
prog_ad9951_asf(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val
	)
{
	enum { ASF = 0x02 };	/* Target register - CFR2 (0x01) */
	const uint_fast16_t v16 = * val;

	spi_select(target, AD9951_SPIMODE);	/* start sending data to target chip */

	spi_progval8_p1(target, ASF);	/* write, address 2 */
	spi_progval8_p2(target, v16 >> 8);
	spi_progval8_p2(target, v16 >> 0);

	spi_complete(target);
	spi_unselect(target);	/* start sending data to target chip */
}
#endif
/* complex init */


static void 
//NOINLINEAT
prog_ad9951_init(
	spitarget_t target,	/* addressing to chip */
	uint_fast8_t fqscale,
	uint_fast8_t vcogt250		/* cfr2<2> if freqscale != 0 and internal freq > 250 MHz */
			 )
{

	phase_t ctl1 = 0x00;
	//#define ctl1 phase_0
	phase_t ctl2;

	if (fqscale == 1)
	{
		/* no internal multiplier, PLL bypassed */
		phase_fromulong(& ctl2, 
			(0x00L << 16) |
			(((1 << 3)) << 0)	// CFR2<7:3>: Reference Clock Multiplier Control Bits
			);
	}
	else
	{
		const unsigned icp = 0;	/* cfr2<1:0> value */
		/*
			icp=0: optimum value with Rpll = 300 OHm, Cpll = 0.01 uF
		*/
		/* With internal multiplier */
		phase_fromulong(& ctl2, 
			(0x00L << 16) |
			(((fqscale << 3) | (vcogt250 ? 0x04L : 0x00L) | icp) << 0)
			);
	}


#if OUTSHAPE9951 || WITHLO1LEVELADJ
	ctl1 |= (1UL << 25);	/* OSK enble */
#endif

//#if CTLSTYLE_SW2011ALL
//	ctl1 |= (1UL << 6);	/* AD9952 comparator disable */
//#endif

	prog_ad9951_cfr2(target, & ctl2);	// Register with address 0x01
	prog_ad9951_cfr1(target, & ctl1);	// Register with address 0x00
	prog_pulse_ioupdate();

#if 0
	// так как мы не переключаемся в режим со внешним 
	// io update clock, второе программирование не нужно.
	// но все-таки делаем его
	prog_ad9951_cfr2(target, & ctl2);	// Register with address 0x01
	prog_ad9951_cfr1(target, & ctl1);	// Register with address 0x00
	prog_pulse_ioupdate();
#endif

	// установить выходной уровень
#if OUTSHAPE9951
	{
		phase_t dmult;
		//
		// такое значение рекомендовано вместо 0x03fff
		// как дающее меньший уровень побочный сигналов
		//
		phase_fromuint(& dmult, OUTSHAPE9951VAL);
		prog_ad9951_asf(target, & dmult);
		// ad9854 specific
		//prog_ad9951_output_shape_key_Q(target, & dmult);
		prog_pulse_ioupdate();
	}
#endif

}

#endif /* AD9951_C_INCLUDED */
