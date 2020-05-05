/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
/* programming LM7001 */

#ifndef LM7001_C_INCLUDED
#define LM7001_C_INCLUDED

//#include "../formats.h"	// debug printf


#define LM7001_SPIMODE SPIC_MODE2
//#define LM7001_PLL1_RCODE	LM7001_RCODE_7200
//#define LM7001_PLL2_RCODE	LM7001_RCODE_7200

/*
	R2 R1 R0 value - ref divider

	0 - 72 (100 kHz step)	// FMIN 45..130 MHz
	1 - 720 (10 kHz step)	// FMIN 5..30 MHz
	2 - 288 (25 kHz step)	// FMIN 45..130 MHz
	3 - 7200 (1 kHz step)	// FMIN 5..30 MHz
	4 - 144 (50 kHz step)	// FMIN 45..130 MHz
	5 - 800 (9 kHz step)	// FMIN 5..30 MHz
	6 - 1440 (5 kHz step)	// FMIN 5..30 MHz
	7 - 1440 (5 kHz step)	// FMIN 5..30 MHz
 */

#define LM7001_RCODE_72		0
#define LM7001_RCODE_144	4
#define LM7001_RCODE_288	2
#define LM7001_RCODE_720	1
#define LM7001_RCODE_800	5
#define LM7001_RCODE_1440	6
#define LM7001_RCODE_7200	3

/* set frequency divider to PLL */
static void 
prog_lm7001_divider(
	spitarget_t target,	/* addressing to chip */
	const phase_t * val,	/* N divider value */
	uint_fast8_t arg_B2B1B0,	// B2, B1 and B0 bits
	uint_fast8_t arg_R2R1R0 /* = LM7001_RCODE_7200 */ /* ref divider - see docs */
	)
{
	const uint_fast8_t arg_S = 1; 		/* 1=FM IN, 0 - AM IN */
	const uint_fast8_t arg_TB = 0;		/* 1: BO1 provide time base */
	const uint_fast8_t arg_T1T0 = 0;	/* LSI test bits should be = "0" */
	const uint_fast16_t v = * val;		/* делитель */
	rbtype_t rbbuff [3] = { 0 };
	
	RBBIT(007, arg_S);			/* S	*/
	RBVAL(004, arg_R2R1R0, 3);	/* R2,,R0 */
	RBBIT(003, arg_TB);			/* TB	*/
	RBVAL(000, arg_B2B1B0, 3);	/* B2..B0	 */
	RBVAL(016, arg_T1T0, 2);	/* T1.T0	*/
	if (arg_S != 0)
	{
		// в режиме FM до 16383
		RBVAL(010, v >> 8, 6);		/* D8..D13 - divider */
		RBVAL(020, v >> 0, 8);		/* D0..D7 - divider */
	}
	else
	{
		// в режиме АМ до 1023
		RBVAL(010, v >> 4, 6);		/* D8..D13 - divider */
		RBVAL(024, v >> 0, 4);		/* D4..D7 - divider */
	}


	/* Выдача байта МЛАДШИМ битом вперёд */
	spi_select2(target, LM7001_SPIMODE, SPIC_SPEED400k);			// передаём начиная с младшего бита

	spi_progval8_p1(target, revbits8(rbbuff [0]));		// D0..D7
	spi_progval8_p2(target, revbits8(rbbuff [1]));		// D8..D13, T0..T1 = 0
	spi_progval8_p2(target, revbits8(rbbuff [2]));		// B0..B2=0, TB=0, R0..R2=3, S=1

	spi_complete(target);
	spi_unselect(target);	/* done sending data to target chip */
}

static void 
prog_lm7001_initialize(
	spitarget_t target	/* addressing to chip */
{
	//hardware_spi_master_setfreq(SPIC_SPEED400k, SPISPEED400k);	// Slow clock speed (for chips like LM7001 or DS1305EN)
}
#endif /* LM7001_C_INCLUDED */
