/* $Id$ */
// Ïðîåêò HF Dream Receiver (ÊÂ ïðè¸ìíèê ìå÷òû)
// àâòîð Ãåíà Çàâèäîâñêèé mgs2001@mail.ru
// UA1ARN
//
#ifndef ADF4360_C_INCLUDED
#define ADF4360_C_INCLUDED


#define ADF4360_PRESCALER_CODE 0	
#define ADF4360_PRESCALER_VALUE 8	

//#define ADF4360_PRESCALER_CODE 1	
//#define ADF4360_PRESCALER_VALUE 16	

//#define ADF4360_PRESCALER_CODE 2	
//#define ADF4360_PRESCALER_VALUE 32
	
enum
{
	ADF4360_FOLD_TRISTATE = 0x00,
	ADF4360_FOLD_LD = 0x01,
	ADF4360_FOLD_N_OUT = 0x02,
	ADF4360_FOLD_AVDD = 0x03,
	ADF4360_FOLD_R_OUT = 0x04,
	ADF4360_FOLD_LD_OPENDRAIN = 0x05,
	ADF4360_FOLD_SDO = 0x06,
	ADF4360_FOLD_DGND = 0x07
};

// R 1..16383
// This function should be called first.
// Ýòó ôóíêöèþ âûçâàòü ïðè èíèöèàëèçàöèè ïåðâîé. ×àñòîòà íà âõîäå REF äîëæíû ïðèñóòñòâîâàòü.
static void prog_adf4360_r(
	spitarget_t target,		/* addressing to chip */
	const phase_t * r		// ÄÅËÈÒÅËÜ ÎÏÎÐÍÎÉ ×ÀÑÒÎÒÓ
	)
{
	// R
	prog_select(target);;		// CS ON

	prog_val(target, 0x00, 2);		// reserved
	prog_val(target, 0x03, 2);		// BAND SELECT CLOCK divider
	prog_bit(target, 0);		// test mode bit
	prog_bit(target, 0);		// lock detect precision
	prog_val(target, 0x02, 2);		// ABP
	prog_phbits(target, r, 14, 14);	// r13..r0
	prog_val(target, 0x01, 2);		// ADDRESS
	
	prog_unselect(target);		// CS OFF
}

// N. minimum value = (prescaler * prescaler) - prescaler = 56 bf presccaler = 8
static void prog_adf4360_n(
	spitarget_t target,		/* addressing to chip */
	const phase_t * n		// ÄÅËÈÒÅËÜ ÎÏÎÐÍÎÉ ×ÀÑÒÎÒÓ
	)
{
	const div_t v = div(* n, ADF4360_PRESCALER_VALUE);
	// N
	prog_select(target);;		// CS ON

	prog_bit(target, 0);		// div sel
	prog_bit(target, 0);		// div/2 output
	prog_bit(target, 1);		// CP GAIN
	prog_val(target, v.quot >> 8, 5);	// B
	prog_val(target, v.quot >> 0, 8);	// B
	prog_bit(target, 0);		// reserv
	prog_val(target, v.rem, 5);	// B
	prog_val(target, 0x02, 2);		// ADDRESS

	prog_unselect(target);		// CS OFF
}

static void prog_adf4360_func(
	spitarget_t target,		/* addressing to chip */
	unsigned char muxout
	)
{
	enum { pd = 0 }; // power down
	// ctl
	// 0ff12c
	prog_select(target);;		// CS ON

	prog_val(target, ADF4360_PRESCALER_CODE, 2);		// prescalerL 0: 8/9, 1: 16/17, other: 32/33
	prog_bit(target, pd);		// pd2
	prog_bit(target, pd);		// pd1
	prog_val(target, 0x07, 3);		// cp current settings
	prog_val(target, 0x07, 3);		// cp current settings
	prog_val(target, 0x01, 2);		// power out level
	prog_bit(target, 0);		// mute-til-lock-detect
	prog_bit(target, 0);		// cp gain
	prog_bit(target, 0);		// cp3 (tree-state)
	prog_bit(target, 1);		// polarity
	prog_val(target, muxout, 3);		// mux out 0x01 - lock detect, 
	prog_bit(target, 0);		// cr
	prog_val(target, 0x03, 2);		// core power
	prog_val(target, 0x00, 2);		// address

	prog_unselect(target);		// CS OFF

	/*
	The duration of this interval is affected by the value of the capacitor on the CN pin (Pin 14). 
	This capacitor is used to reduce the close-in noise of the ADF4360-7 VCO. 
	The recommended value of this capacitor is 10 uF. 
	Using this value requires an interval of 10 ms between the latching in of the 
	control latch bits and latching in of the N counter latch bits. 
	If a shorter delay is required, the capacitor can be reduced. 
	A slight phase noise penalty is incurred by this change, 
	which is further explained in the Table 10.
	*/
	local_delay_ms(10);
}


#endif /* ADF4360_C_INCLUDED */
