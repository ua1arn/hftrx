/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#ifndef MC145170_C_INCLUDED
#define MC145170_C_INCLUDED

//#ifndef WITHSPISW
//	#error WITHSPISW should be defined for this chip
//#endif

#define MC145170_SPIMODE SPIC_MODE3
/*
	Programming MC145170D2
	*/

/* Number of clocks is: 9 to 13 */
static void prog_mc145170_init(
	spitarget_t target		/* addressing to chip */
	)
{
#if 0
	prog_select(target);
	prog_unselect(target);

	prog_select(target);

	//prog_val(target, 0x00, 8);	/* Initialization sequence */
	prog_val(target, 0x02, 5);	/* Initialization sequence */

	prog_unselect(target);
#endif
}

/* Number of clocks is: 8 */
static void prog_mc145170_func(
	spitarget_t target,		/* addressing to chip */
	unsigned char pol		/* ph det polarity: 0- inv, 1 - direct */
	)
{
	rbtype_t rbbuff [1] = { 0 };

	RBBIT(007, pol);		/* C7: POL (1-with invertor, 0 - direct)	*/
	RBBIT(006, 0x01);		/* C6: PDA/B - 0x00 - A, 0x01 - B (tri-state)	*/
	RBBIT(005, 0x00);		/* C5: LDE		*/
#if TESTPOINTS
	RBVAL(002, 0x01, 3);	/* C4..C2: OSC0 0 - low, 2 - osc/2	*/
	RBBIT(001, 0x01);		/* C1: Enables Fv output */
	RBBIT(000, 0x01);		/* C0: Enables Fr output */
#else
	RBVAL(002, 0x00, 3);	/* C4..C2: OSC0 0 - low, 2 - osc/2	*/
	RBBIT(001, 0x00);		/* C1: Enables Fv output */
	RBBIT(000, 0x00);		/* C0: Enables Fr output */
#endif

	spi_select(target, MC145170_SPIMODE);

	spi_progval8_p1(target, rbbuff [0]);	/* C bits */
	spi_complete(target);

	spi_unselect(target);
}

/* Set divider F osc R - 1, 5..32767 */
/* Number of clocks is: 15 or 24 */
void prog_mc145170_r(
	spitarget_t target,		/* addressing to chip */
	const phase_t * r		/* divider F osc R - 1, 5..32767 */
	)
{
	const uint_fast16_t v = (uint_fast16_t) * r;

	spi_select(target, MC145170_SPIMODE);

	spi_progval8_p1(target, 0x00);	/* R bits */
	spi_progval8_p2(target, v >> 8);
	spi_progval8_p2(target, v >> 0);
	spi_complete(target);

	spi_unselect(target);
}

/* Set divider F in  N - 40..65535 */
/* Number of clocks is: 16 */
static void prog_mc145170_n(
	spitarget_t target,		/* addressing to chip */
	const phase_t * n		/* divider F in  N - 56..65535 */
	)
{
	const uint_fast16_t v = (uint_fast16_t) * n;

	spi_select(target, MC145170_SPIMODE);

	spi_progval8_p1(target, v >> 8);	/* B bits */
	spi_progval8_p2(target, v >> 0);
	spi_complete(target);

	spi_unselect(target);
}

#endif /* MC145170_C_INCLUDED */
