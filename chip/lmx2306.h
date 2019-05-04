/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#ifndef LMX2306_H_INCLUDED
#define LMX2306_H_INCLUDED

#define LMX2306_SPIMODE SPIC_MODE3	// MODE3 only supported

#define LMX2306_GO_STATE 0	/* prog_lmx2306_n parameter: N19=LOW ICPo = 250 µA, N19=HIGH ICPo = 1 mA. */

enum
{
	LMX2306_FOLD_TRISTATE = 0x00,
	LMX2306_FOLD_LD = 0x01,
	LMX2306_FOLD_N_OUT = 0x02,
	LMX2306_FOLD_AVDD = 0x03,
	LMX2306_FOLD_R_OUT = 0x04,
	LMX2306_FOLD_LD_OPENDRAIN = 0x05,
	LMX2306_FOLD_SDO = 0x06,
	LMX2306_FOLD_DGND = 0x07
};

/* programming LMX2306 */
static void prog_lmx2306_init(
	spitarget_t target		/* addressing to chip */
	)
{
	spi_select(target, LMX2306_SPIMODE);
	spi_progval8_p1(target, 0x03);
	spi_complete(target);
	spi_unselect(target);

	//prog_select(target);

	//prog_bit(target, 1);	/* c2 bit */
	//prog_bit(target, 1);	/* c1 bit */

	//prog_unselect(target);
}

static void prog_lmx2306_func(
	spitarget_t target,		/* addressing to chip */
	uint_fast8_t polarity,
	uint_fast8_t FoLD,		/* F5..F3: Fo/LD: 4 - output of R divider, 2 - output of N divider  */
	uint_fast8_t stop
	)
{
	rbtype_t rbbuff [3] = { 0 };

	RBBIT(20, 0);	/* F19: test mode should be 0 */
	RBBIT(19, 0);	/* F18: power down mode = 0 */
	RBVAL(16, 0, 3);	/* F17..F15: test mode should be 0 */
	RBVAL(12, 0, 4);	/* F14..F11: timeout counter */
	RBVAL(9, 0x00, 3);	/* F10..F8: fastlock modes */
	RBBIT(8, 0);	/* F7: CP tri-state */

	RBBIT(7, polarity);	/* F6: phase detector polarity */
	RBVAL(4, FoLD, 3);	/* F5..F3: Fo/LD: 4 - output of R divider, 2 - output of N divider  */
	RBBIT(3, 0);	/* F2: power down */
	RBBIT(2, 0x00);	/* F1: counter reset */

	RBBIT(1, 1);	/* C2: bit */
	RBBIT(0, 0);	/* C1: bit */

	spi_select(target, LMX2306_SPIMODE);

	spi_progval8_p1(target, rbbuff [0]);
	spi_progval8_p2(target, rbbuff [1]);
	spi_progval8_p2(target, rbbuff [2]);
	spi_complete(target);

	spi_unselect(target);
}

/* Set divider F osc R - 3...16383 */
void prog_lmx2306_r(
	spitarget_t target,		/* addressing to chip */
	const phase_t * r		/* divider F osc R - 3...16383 */
	)
{
	const uint_fast16_t v = (uint_fast16_t) * r;

	spi_select(target, LMX2306_SPIMODE);

	spi_progval8_p1(target, 0x00);
	spi_progval8_p2(target, (v >> 6) & 0x3f);
	spi_progval8_p2(target, v << 2);		// six lower bits and c2=0 and c1=0
	spi_complete(target);

	spi_unselect(target);

	//prog_select(target);	/* start sending data to target chip */

	//prog_bit(target, 0);	/* LD precision */

	//prog_val(target, 0, 4);	/* R18..F15: test mode should be 0 */
	//prog_phbits(target, r, 14, 14);

	//prog_bit(target, 0);	/* c2 bit */
	//prog_bit(target, 0);	/* c1 bit */

	//prog_unselect(target);
}

/*
	N19 determines the charge pump current magnitude 
	
*/

/* Set divider F in  N - 56..65535 */
static void prog_lmx2306_n(
	spitarget_t target,		/* addressing to chip */
	const phase_t * n,		/* divider F in  N - 56..65535 */
	uint_fast8_t go
	)
{
	const ldiv_t d = ldiv(* n, 8);		/* 8 - прескалер работает в режиме 8/9 */
	const uint_fast16_t a = d.rem;					// 5 bit used
	const uint_fast16_t b = d.quot & 0x1fff;		// 13 bit used

	spi_select(target, LMX2306_SPIMODE);

	spi_progval8_p1(target, (b >> 9) | (go ? 0x10 : 0x00));
	spi_progval8_p2(target, (b >> 1));
	spi_progval8_p2(target, ((b & 0x01) << 7) | (a << 2) | 0x01);	// c2=0 and c1=1
	spi_complete(target);

	spi_unselect(target);

	//prog_select(target);	/* start sending data to target chip */
							/* N19=LOW ICPo = 250 µA, N19=HIGH ICPo = 1 mA. */
	//prog_bit(target, go);	/* GO bit - for fast changing frequency w/o lost synch */
	//prog_phbits(target, n, 16, 13);	/* B bits */
	//prog_phbits(target, n, 5, 5);		/* A bits -  only 3 lower used */
	//prog_bit(target, 0);	/* c2 bit */
	//prog_bit(target, 1);	/* c1 bit */

	//prog_unselect(target);
}

#endif /* LMX2306_H_INCLUDED */
