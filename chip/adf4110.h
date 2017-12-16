/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#ifndef ADF4110_H_INCLUDED
#define ADF4110_H_INCLUDED


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

#define PLL1_PRESCALER	0x00	/* 0: 8/9, ... 3: 64/65 */
#define PLL2_PRESCALER	0x00	/* 0: 8/9, ... 3: 64/65 */

#define PLL1_CPI 0x07
#define PLL2_CPI 0x07

enum
{
	ADF4110_FOLD_TRISTATE = 0x00,
	ADF4110_FOLD_LD = 0x01,
	ADF4110_FOLD_N_OUT = 0x02,
	ADF4110_FOLD_AVDD = 0x03,
	ADF4110_FOLD_R_OUT = 0x04,
	ADF4110_FOLD_LD_OPENDRAIN = 0x05,
	ADF4110_FOLD_SDO = 0x06,
	ADF4110_FOLD_DGND = 0x07
};
/* programming ADF4110 */
static void prog_adf4110_init(
	spitarget_t target		/* addressing to chip */
	)
{

	prog_select(target);

	prog_bit(target, 1);	/* c2 bit */
	prog_bit(target, 1);	/* c1 bit */

	prog_unselect(target);
}

static void prog_adf4110_func(
	spitarget_t target,		/* addressing to chip */
	unsigned char polarity,
	unsigned char FoLD,		/* F5..F3: Fo/LD: 4 - output of R divider, 2 - output of N divider  */
	unsigned char cpipll,
	unsigned char prescaler
	)
{
	prog_select(target);	/* start sending data to target chip */

	prog_val(target, 0x00, 2);	/* (see W below). DB23..DB22: perescaler 0:8/9, 1:16/17, 2:32/33, 3:64/65 */
	prog_bit(target, 0);	/* DB21: power down : PD2 */
	prog_val(target, cpipll, 3);	/* DB20..DB18: current setting 2 */
	prog_val(target, cpipll, 3);	/* DB17..DB15: current setting 1 */
	prog_val(target, 0x00, 4);	/* DB14..F11: timeout counter */
	prog_val(target, 0x00, 2);	/* DB10..DB9: fastlock modes */
	prog_bit(target, 0);	/* DB8: CP tri-state */
	prog_bit(target, polarity);	/* DB7: phase detector polarity */
	prog_val(target, FoLD, 3);	/* DB6..DB4: Fo/LD: 4 - output of R divider, 2 - output of N divider  */
	prog_bit(target, 0);	/* DB3: power down */
	prog_bit(target, 0x00);	/* DB2: counter reset */

	prog_bit(target, 1);	/* DB1: bit */
	prog_bit(target, 0);	/* DB0: bit */

	prog_unselect(target);
}


/* Set divider F osc R - 3...16383 */
void prog_adf4110_r(
	spitarget_t target,		/* addressing to chip */
	const phase_t * r		/* divider F osc R - 3...16383 */
	)
{
	prog_select(target);	/* start sending data to target chip */

	prog_bit(target, 0);	/* DB23: reserved */
	prog_bit(target, 0);	/* DB22: DLY */
	prog_bit(target, 0);	/* DB21: SYNC */
	prog_bit(target, 0);	/* DB20: LD precision */

	prog_val(target, 0x00, 2);	/* DB19..DB18: ABP2, ABP1 */
	prog_val(target, 0, 2);	/* DB17..DB16: test mode should be 0 */
	prog_phbits(target, r, 14, 14);	/* DB15..DB2: R14..R1 */

	prog_bit(target, 0);	/* c2 bit */
	prog_bit(target, 0);	/* c1 bit */

	prog_unselect(target);
}


/*
	N19 determines the charge pump current magnitude 
	
*/

/* Set divider F in  N - 56..65535 */
static void prog_adf4110_n(
	spitarget_t target,		/* addressing to chip */
	const phase_t * n,		/* divider F in  N - 56..65535 */
	unsigned char go,
	unsigned char prescaler	/* preacaler code - same as used in init */
	)
{
	//enum { W = 3 };
	enum { W = (prescaler + 3) };
	prog_select(target);	/* start sending data to target chip */

	prog_val(target, 0x00, 2);	/* DB23:DB22: reserved - zeroes */
							/* N19=LOW ICPo = 250 µA, N19=HIGH ICPo = 1 mA. */
	prog_bit(target, go);	/* GO bit - for fast changing frequency w/o lost synch */
	prog_phbits(target, n, 13 + W, 13);	/* B bits */
	
	prog_val(target, 0x00, 6 - W);	/* : high A bits - zeroes */
	prog_phbits(target, n, W, W);	/* low A bits - value  */

	prog_bit(target, 0);	/* c2 bit */
	prog_bit(target, 1);	/* c1 bit */

	prog_unselect(target);
}

#endif /* ADF4110_H_INCLUDED */
