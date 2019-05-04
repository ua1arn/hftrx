/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// (c) Ingo Gerlach 11/2000
//
#ifndef TSA6057_C_INCLUDED
#define TSA6057_C_INCLUDED

#if SCL_CLOCK > 100000L
	#error TSA6057 IS A LOW-SPEED I2C CHIP
#endif
/*


Ansteuereung TSA6057
		128	64	32	16	8	4	2	1	
--------------------------------------------------------------------------
Adresse:	1	1	0	0	0	1	A0	0  	: 196 / 198
SubAdress:	0	0	0	0	0	0	0	0  	: 0 
DB0:		S6	S5	S4	S3	S2	S1	S0	CP
DB1:		S14	S13	S12	S11	S10	S9	S8	S7
DB2:		Ref1	Ref2	FM-_AM	_FM-AM	x	BS	S16	S15	: 
DB3:		T1	T2	T3	X	X	X	X	X	: 0

REF1/2	:	Raster 
0	0	1  KHz 	: 0
0	1	10 KHz	: 64
1	0	25 KHz	: 128

Mode:	16	AM
	32	FM

BS	1
*/

/*
 Schematics: http://ic.pics.livejournal.com/vitsserg/16042243/405869/405869_original.jpg
 */

#define TSA6057_ADR	196		// as=0: Adresse default 196 , (198 )
//#define TSA6057_ADR	198		// as=1

#define TSA6057_AM	16		// AM 
#define TSA6057_FM	32		// FM
#define TSA6057_R01_4000	0		// Raster 1 KHz		- R divider = 4000
#define TSA6057_R10_400	64		// Raster 10 KHz	- R divider = 400
#define TSA6057_R25_160	128		// Raster 25 KHz	- R divider = 160
//#define TSA6057_BS	4		// Bandswitch
#define TSA6057_T 0	// T bits
#define TSA6057_CP 1	// CP bit

enum
{
	TSA6057_MASK_CP = 0x01,	// db0 data
	TSA6057_MASK_REF = 0xc0,	// db2 data
	TSA6057_MASK_FMAM = 0x30,	// db2 data
	TSA6057_MASK_BS = 0x04,	// db2 data
	TSA6057_MASK_T = 0xe0	// db3 data
};

static void 
prog_tsa6057_n(
	spitarget_t target,		/* addressing to chip */
	const phase_t * n,		/* делитель N  */
	uint_fast8_t rcode,		/* TSA6057_R25_160/TSA6057_R10_400/TSA6057_R01_4000 */
	uint_fast8_t bs		/* TSA6057_R25_160/TSA6057_R10_400/TSA6057_R01_4000 */
	)
{
	(void) target;
	const uint_fast16_t v = (uint_fast16_t) * n;

	i2c_start(TSA6057_ADR);
	i2c_write(0x00);
	i2c_write(
		((v & 0x7f) << 1) | 
		(TSA6057_MASK_CP & TSA6057_CP) |	/* S6:S0 bits */
		0);
	i2c_write(v >> 7);	// S14:S7 bits
	i2c_write(
		((v >> 15) & 0x03) |	// S16:S15 bits
		(rcode & TSA6057_MASK_REF) |
		(TSA6057_FM & TSA6057_MASK_FMAM)  |
		(bs ? TSA6057_MASK_BS : 0) |	
		0);
	i2c_write(TSA6057_T & TSA6057_MASK_T);
	i2c_waitsend();
	i2c_stop();
}

#endif /* TSA6057_C_INCLUDED */
