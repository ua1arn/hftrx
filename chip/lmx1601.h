/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#ifndef LMX1601_H_INCLUDED
#define LMX1601_H_INCLUDED
/*
	LMX1600 2.0 GHz/500 MHz
	LMX1601 1.1 GHz/500 MHz
	LMX1602 1.1 GHz/1.1 GHz

	Dual modulus prescaler:
	— 2 GHz/500 MHz option: (Main) 32/33 (Aux) 8/9
	— 1.1 GHz/500 MHz option: (Main) 16/17 (Aux) 8/9
	— 1.1 GHz/1.1 GHz option: (Main) 16/17 (Aux) 16/17

	В этом проекте используется AUX канал LMX1601

*/

enum
{
	LMX1601_FOLD_GROUND = 0x00,			// OSCout = crystal mode
	LMX1601_FOLD_ONE = 0x01,			// OSCout = crystal mode
	LMX1601_FOLD_MAIN_LD = 0x02,		// +1: OSCout = crystal mode, +0: OSCout = logic mode
	LMX1601_FOLD_AUX_LD = 0x04,			// +1: OSCout = crystal mode, +0: OSCout = logic mode
	LMX1601_FOLD_MAIN_AND_AUX_LD = 0x06,// +1: OSCout = crystal mode, +0: OSCout = logic mode	
	LMX1601_FOLD_MAIN_R_OUT = 0x08,		// +1: OSCout = crystal mode, +0: OSCout = logic mode
	LMX1601_FOLD_AUX_R_OUT = 0x0a,		// +1: OSCout = crystal mode, +0: OSCout = logic mode
	LMX1601_FOLD_MAIN_N_OUT = 0x0c,		// +1: OSCout = crystal mode, +0: OSCout = logic mode
	LMX1601_FOLD_AUX_N_OUT = 0x0e,		// +1: OSCout = crystal mode, +0: OSCout = logic mode
};

enum lmx1601_ctlbits
{
	LMX1601_AUX_R = 0x00,
	LMX1601_AUX_N = 0x01,
	LMX1601_MAIN_R = 0x02,
	LMX1601_MAIN_N = 0x03,
};

/* programming LMX1601 */
static void prog_lmx1601_init(
	spitarget_t target		/* addressing to chip */
	)
{

}

// В этом проекте используется AUX канал LMX1601

static void prog_lmx1601_func(
	spitarget_t target,		/* addressing to chip */
	uint_fast8_t polarity
	)
{
	const uint_fast16_t mainR = 100;	/* значение по умолчанию для канала  MAIN */

	prog_select(target);	/* start sending data to target chip */

	prog_bit(target, 0x01);		/* AUX_CP_GAIN: 0 = 160 uA typ, 1 = 1600 uA typ*/
	prog_bit(target, 0x00);		/* MAIN_CP_GAIN: 0 = 160 uA typ, 1 = 1600 uA typ */
	prog_bit(target, polarity);	/* AUX_PD_POL */
	prog_bit(target, 0x00);		/* MAIN_PD_POL */
	prog_val(target, mainR >> 8, 4);	/* high 4 bits of main_r */
	prog_val(target, mainR, 8);			/* low 8 bits of main_r  */
	prog_val(target, LMX1601_MAIN_R, 2);	/* c2,c1 bit */

	prog_unselect(target);
}

// В этом проекте используется AUX канал LMX1601

/* Set divider F osc R - 3...16383 */
void prog_lmx1601_r(
	spitarget_t target,		/* addressing to chip */
	const phase_t * r,		/* divider F osc R - 3...16383 */
	uint_fast8_t FoLD
	)
{
	prog_select(target);	/* start sending data to target chip */

	prog_val(target, FoLD, 4);	/* 17..14 bits: FoLD state */
	prog_phbits(target, r, 12, 12);
	prog_val(target, LMX1601_AUX_R, 2);	/* c2,c1 bit */

	prog_unselect(target);
}

// В этом проекте используется AUX канал LMX1601

/* Set divider F in  N - 56..65535 */
static void prog_lmx1601_n(
	spitarget_t target,		/* addressing to chip */
	const phase_t * n		/* divider F in  N - 56..65535 */
	)
{
	prog_select(target);	/* start sending data to target chip */

	prog_phbits(target, n, 15, 12);		/* B bits */
	prog_phbits(target, n, 4, 4);		/* A bits - only 3 lower used */
	prog_val(target, LMX1601_AUX_N, 2);	/* c2,c1 bit */

	prog_unselect(target);
}

#endif /* LMX1601_H_INCLUDED */
