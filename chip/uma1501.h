/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#ifndef UMA1501_C_INCLUDED
#define UMA1501_C_INCLUDED

#define UMA1501_SPIMODE SPIC_MODE3	// MODE3 only supported

#define UMA1501_GO_STATE 0	/* prog_uma1501_n parameter: . */

enum
{
	UMA1501_FOLD_TRISTATE = 0x00,
	UMA1501_FOLD_LD = 0x01,
	UMA1501_FOLD_N_OUT = 0x02,
	UMA1501_FOLD_AVDD = 0x03,
	UMA1501_FOLD_R_OUT = 0x04,
	UMA1501_FOLD_LD_OPENDRAIN = 0x05,
	UMA1501_FOLD_SDO = 0x06,
	UMA1501_FOLD_DGND = 0x07
};


/*
CPI6 CPI5 CP14		I CP (mA)
CPI3 CPI2 CPI1	2.7k.	4.7k. 10k.
	0 0 0		1.088	0.625	0.294
	0 0 1		2.176	1.25	0.588
	0 1 0		3.264	1.875	0.882
	0 1 1		4.352	2.5		1.176
	1 0 0		5.44	3.125	1.47
	1 0 1		6.528	3.75	1.764
	1 1 0		7.616	4.375	2.058
	1 1 1		8.704	5.0		2.352
*/
//#define PLL1_CPI 0x03
//#define PLL2_CPI 0x03

#define PLL1_CPI 0x07
#define PLL2_CPI 0x07

static void 
prog_uma1501_init(
	spitarget_t target		/* addressing to chip */
	)
{
}

static void 
prog_uma1501_func(
	spitarget_t target,		/* addressing to chip */
	uint_fast8_t polarity,
	uint_fast8_t FoLD,		/* UMA1501_FOLD_xxx constants  */
	uint_fast8_t cpipll,
	uint_fast8_t stop
	)
{
	rbtype_t rbbuff [3] = { 0 };

	RBVAL(22, 0x00, 2);	/* DB23..DB22: reserved */
	RBBIT(21, 0);	/* DB21: power down */
	RBVAL(18, cpipll, 3);	/* DB20..DB18: current setting 2 */
	RBVAL(15, cpipll, 3);	/* DB17..DB15: current setting 1 */
	RBVAL(11, 0, 4);	/* DB14..F11: timeout counter */
	RBVAL(9, 0x00, 2);	/* DB10..DB9: fastlock modesL 0 - disabled */
	RBBIT(8, 0);	/* DB8: CP tri-state */
	RBBIT(7, polarity);	/* DB7: phase detector polarity */
	RBVAL(4, FoLD, 3);	/* DB6..DB4: Fo/LD: 4 - output of R divider, 2 - output of N divider  */
	RBBIT(3, 0);	/* DB3: power down */
	RBBIT(2, stop);	/* DB2: counter reset */

	RBBIT(1, 1);	/* DB1: bit */
	RBBIT(0, 0);	/* DB0: bit */

	spi_select(target, UMA1501_SPIMODE);

	spi_progval8_p1(target, rbbuff [0]);
	spi_progval8_p2(target, rbbuff [1]);
	spi_progval8_p2(target, rbbuff [2]);
	spi_complete(target);

	spi_unselect(target);
}


/* Set divider F osc R - 3...16383 */
static void 
prog_uma1501_r(
	spitarget_t target,		/* addressing to chip */
	const phase_t * r		/* divider F osc R - 3...16383 */
	)
{
	rbtype_t rbbuff [3] = { 0 };

	RBVAL(21, 0x00, 3);	/* don't care */
	RBBIT(20, 0x00);	/* LD precision */

	RBVAL(16, 0x00, 4);	/* R18..F15: test mode should be 0 */

	const uint_fast16_t rv = * r;

	RBVAL_W16(2, rv, 14);
	RBBIT(1, 0x00);	/* c2 bit */
	RBBIT(0, 0x00);	/* c1 bit */


	spi_select(target, UMA1501_SPIMODE);

	spi_progval8_p1(target, rbbuff [0]);
	spi_progval8_p2(target, rbbuff [1]);
	spi_progval8_p2(target, rbbuff [2]);
	spi_complete(target);

	spi_unselect(target);
}


/* Set divider F in  N - 1..65535 */
static void 
prog_uma1501_n(
	spitarget_t target,		/* addressing to chip */
	const phase_t * n		/* divider F in  N - 56..65535 */
	)
{
	uint_fast8_t go = 0;
	const uint_fast16_t v16 = * n;

	spi_select(target, UMA1501_SPIMODE);	/* start sending data to target chip */

	spi_progval8_p1(target, v16 >> 8);
	spi_progval8_p2(target, v16 >> 0);
	spi_progval8_p2(target, 0x01);	/* six reserved bits, then two bits - register select */
	spi_complete(target);

	spi_unselect(target);	/* done sending data to target chip */
}

#endif /* UMA1501_C_INCLUDED */
