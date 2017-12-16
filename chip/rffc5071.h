/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// RFMD (Qorvo) RFFC5071
//
/*
 * Copyright 2012 Michael Ossmann
 *
 * This file is part of HackRF.
*/
#ifndef RFFC5071_C_INCLUDED
#define RFFC5071_C_INCLUDED

//#define PLL1_TYPE PLL_TYPE_RFFC5071	/* RFMD (Qorvo) RFFC5071 */
//#define PLL1_FRACTIONAL_LENGTH 24	/* lower 24 bits is a fractional part */

#define RFFC5071_FRACTIONAL_LENGTH 24	/* lower 24 bits is a fractional part */
//#define RFFC5071_FTW_FRACTMASK ((1UL << RFFC5071_FRACTIONAL_LENGTH) - 1)

#if PLL1_FRACTIONAL_LENGTH != RFFC5071_FRACTIONAL_LENGTH
	#error Wrong PLL1_FRACTIONAL_LENGTH value
#endif /* PLL1_FRACTIONAL_LENGTH != RFFC5071_FRACTIONAL_LENGTH */
#if DDS1_CLK_MUL != 1
	#error Wrong DDS1_CLK_MUL value
#endif /* DDS1_CLK_MUL != 1 */

/* 31 registers, each containing 16 bits of data. */
#define RFFC5071_NUM_REGS 31

extern uint16_t rffc5071_regs[RFFC5071_NUM_REGS];
extern uint32_t rffc5071_regs_dirty;




#define RFFC5071_REG_SET_CLEAN(r) rffc5071_regs_dirty &= ~(1UL<<r)
#define RFFC5071_REG_SET_DIRTY(r) rffc5071_regs_dirty |= (1UL<<r)

#include "rffc5071_regs.h"

/* Initialize chip. Call _setup() externally, as it calls _init(). */
extern void rffc5071_init(void);
extern void rffc5071_setup(void);


/* Write all dirty registers via SPI from memory. Mark all clean. Some
 * operations require registers to be written in a certain order. Use
 * provided routines for those operations. */
extern void rffc5071_regs_commit(void);

/* Set frequency (MHz). */
//extern void rffc5071_set_frequency(uint16_t mhz);

/* Set up rx only, tx only, or full duplex. Chip should be disabled
 * before _tx, _rx, or _rxtx are called. */
//extern void rffc5071_tx(void);
//extern void rffc5071_rx(void);
//extern void rffc5071_rxtx(void);
//extern void rffc5071_enable(void);
//extern void rffc5071_disable(void);

#define RFFC5071_SPIMODE SPIC_MODE3

/* SPI register write
 *
 * Send 25 bits:
 *   first bit is ignored,
 *   second bit is zero for write operation,
 *   next 7 bits are register address,
 *   next 16 bits are register value.
 */
// запись регистра
static void rffc5071_spi_write(
	spitarget_t target,
	uint_fast8_t addr,		// адрес регитра
	uint_fast16_t v			// данные
	)
{
	SCLK_NPULSE();
	SCLK_NPULSE();

	spi_select(target, RFFC5071_SPIMODE);

	SCLK_NPULSE();

	spi_progval8_p1(target, addr & 0x7f);		// bit7 = 0: write
	spi_progval8_p2(target, v >> 8);				// high byte
	spi_progval8_p2(target, v >> 0);				// low byte
	spi_complete(target);

	spi_unselect(target);
	SCLK_NPULSE();
}


/* SPI register read.
 *
 * Send 9 bits:
 *   first bit is ignored,
 *   second bit is one for read operation,
 *   next 7 bits are register address.
 * Then receive 16 bits (register value).
 */
// чтение регистра
static uint_fast16_t rffc5071_spi_read(
	spitarget_t target,
	uint_fast8_t addr		// адрес регитра
	)
{
	uint_fast16_t v;			// данные

	//gpio_set(PORT_MIXER_ENX, PIN_MIXER_ENX);
	//gpio_set(PORT_MIXER_SCLK, PIN_MIXER_SCLK);
	//gpio_set(PORT_MIXER_SDATA, PIN_MIXER_SDATA);

	SCLK_NPULSE();
	SCLK_NPULSE();


	spi_select(target, RFFC5071_SPIMODE);

	SCLK_NPULSE();

	spi_progval8_p1(target, 0x80 | (addr & 0x7f));		// bit7 = 1: read
	spi_complete(target);


	SCLK_NPULSE();
	SCLK_NPULSE();

	spi_to_read(target);
	v = spi_read_byte(target, 0xff);
	v = v * 256 + spi_read_byte(target, 0xff);
	spi_to_write(target);

	spi_unselect(target);
	SCLK_NPULSE();

	return v;
}


/* Default register values. */
static const FLASHMEM uint16_t rffc5071_regs_default[RFFC5071_NUM_REGS] = { 
	0xbefa,   /* 00 */
	0x4064,   /* 01 */
	0x9055,   /* 02 */
	0x2d02,   /* 03 */
	0xacbf,   /* 04 */
	0xacbf,   /* 05 */
	0x0028,   /* 06 */
	0x0028,   /* 07 */
	0xff00,   /* 08 */
	0x8220,   /* 09 */
	0x0202,   /* 0A */
	0x4800,   /* 0B */
	0x1a94,   /* 0C */
	0xd89d,   /* 0D */
	0x8900,   /* 0E */
	0x1e84,   /* 0F */
	0x89d8,   /* 10 */
	0x9d00,   /* 11 */
	0x2a20,   /* 12 */
	0x0000,   /* 13 */
	0x0000,   /* 14 */
	0x0000,   /* 15 */
	0x0000,   /* 16 */
	0x4900,   /* 17 */
	0x0281,   /* 18 */
	0xf00f,   /* 19 */
	0x0000,   /* 1A */
	0x0000,   /* 1B */
	0xc840,   /* 1C */
	0x1000,   /* 1D */
	0x0005,   /* 1E */ };

uint16_t rffc5071_regs[RFFC5071_NUM_REGS];


static void rffc5071_reg_commit(
	spitarget_t target,		/* addressing to chip */
	uint8_t r
	)
{
	rffc5071_reg_write(target, r, rffc5071_regs[r]);
}


/* Mark all regsisters dirty so all will be written at init. */
static uint32_t rffc5071_regs_dirty = 0x7fffffff;

/* Write all dirty registers via SPI from memory. Mark all clean. Some
 * operations require registers to be written in a certain order. Use
 * provided routines for those operations. */
static void rffc5071_regs_commit(
	spitarget_t target		/* addressing to chip */
	)
{
	int r;
	for(r = 0; r < RFFC5071_NUM_REGS; r++) {
		if ((rffc5071_regs_dirty >> r) & 0x1) {
			rffc5071_reg_commit(target, r);
		}
	}
}


/* Set up all registers according to defaults specified in docs. */
static void prog_rffc5071_regsetup(
	spitarget_t target		/* addressing to chip */
	)
{
	//debug_printf_P(PSTR("# rffc5071_init\n"));
	memcpy_P(rffc5071_regs, rffc5071_regs_default, sizeof(rffc5071_regs));
	rffc5071_regs_dirty = 0x7fffffff;

	/* Write default register values to chip. */
	rffc5071_regs_commit(target);
}

/*
 * Set up pins for GPIO and SPI control, configure SSP peripheral for SPI, and
 * set our own default register configuration.
 */
void rffc5071_initialize(
	spitarget_t target		/* addressing to chip */
{
	prog_rffc5071_regsetup(target);
	//debug_printf_P(PSTR("# rffc5071_setup\n"));

#if 0
	/* initial setup */
	/* put zeros in freq contol registers */
	set_RFFC5071_P2N(0);
	set_RFFC5071_P2LODIV(0);
	set_RFFC5071_P2PRESC(0);
	set_RFFC5071_P2VCOSEL(0);

	set_RFFC5071_P2N(0);
	set_RFFC5071_P2LODIV(0);
	set_RFFC5071_P2PRESC(0);
	set_RFFC5071_P2VCOSEL(0);

	set_RFFC5071_P2N(0);
	set_RFFC5071_P2LODIV(0);
	set_RFFC5071_P2PRESC(0);
	set_RFFC5071_P2VCOSEL(0);
#endif
	/* set ENBL and MODE to be configured via 3-wire interface,
	 * not control pins. */
	set_RFFC5071_SIPIN(1);

	/* GPOs are active at all times */
	//set_RFFC5071_GATE(1);
	/* GPOs are off */
	set_RFFC5071_GATE(0);


	set_RFFC5071_LOCK(1);	// lock detect output

	rffc5071_regs_commit(target);
}

/* Read a register via SPI. Save a copy to memory and return
 * value. Discard any uncommited changes and mark CLEAN. */
uint16_t rffc5071_reg_read(uint8_t r)
{
	/* Readback register is not cached. */
	if (r == RFFC5071_READBACK_REG)
		return rffc5071_spi_read(r);

	/* Discard uncommited write when reading. This shouldn't
	 * happen, and probably has not been tested. */
	if ((rffc5071_regs_dirty >> r) & 0x1) {
		rffc5071_regs[r] = rffc5071_spi_read(r);
	};
	return rffc5071_regs[r];
}

/* Write value to register via SPI and save a copy to memory. Mark
 * CLEAN. */
void rffc5071_reg_write(
	spitarget_t target,		/* addressing to chip */
	uint8_t r, 
	uint16_t v
	)
{
	rffc5071_regs[r] = v;
	rffc5071_spi_write(target, r, v);
	RFFC5071_REG_SET_CLEAN(r);
}


void rffc5071_disable(void)  {
	//debug_printf_P(PSTR("# rfc5071_disable\n"));
	set_RFFC5071_ENBL(0);
	rffc5071_regs_commit(target);
}

void rffc5071_enable(void)  {
	//debug_printf_P(PSTR("# rfc5071_enable\n"));
	set_RFFC5071_ENBL(1);
	rffc5071_regs_commit(target);
}


void rffc5071_tx(int x) {
	//debug_printf_P(PSTR("# rffc5071_tx\n"));
	set_RFFC5071_ENBL(0);
	set_RFFC5071_FULLD(0);
	set_RFFC5071_MODE(1); /* mixer 2 used for both RX and TX */
	rffc5071_regs_commit(target);
}

void rffc5071_rx(int x) {
	//debug_printf_P(PSTR("# rfc5071_rx\n"));
	set_RFFC5071_ENBL(0);
	set_RFFC5071_FULLD(0);
	set_RFFC5071_MODE(1); /* mixer 2 used for both RX and TX */
	rffc5071_regs_commit(target);
}

/*
 * This function turns on both mixer (full-duplex) on the RFFC5071, but our
 * current hardware designs do not support full-duplex operation.
 */
void rffc5071_rxtx(void) {
	//debug_printf_P(PSTR("# rfc5071_rxtx\n"));
	set_RFFC5071_ENBL(0);
	set_RFFC5071_FULLD(1); /* mixer 1 and mixer 2 (RXTX) */
	rffc5071_regs_commit(target);

	rffc5071_enable();
}


#define FREQ_ONE_MHZ (1000UL*100UL)	// параметр отладочной печати

/* Calculate n_lo (lo in 100 kHz step) */
static uint_fast8_t rffc5071_calc_n_lo(uint_fast16_t lo)
{
	const uint_fast32_t LO_MAX = 54000UL;	// 5.4 GHz
	uint16_t x = LO_MAX / lo;
	uint8_t n_lo = 0;

	while ((x > 1) && (n_lo < 5)) {
		++ n_lo;
		x >>= 1;
	}
	return n_lo;
}


/* configure frequency synthesizer in integer mode (lo in 100 kHz step) */
void rffc5071_config_synth_int(uint_fast16_t lo) {

	const uint_fast32_t REF_FREQ = 144;		// 14.4 MHz
	
	//debug_printf_P(PSTR("# config_synth_int\n"));

	/* Calculate n_lo */
	const uint_fast8_t n_lo = rffc5071_calc_n_lo(lo);

	const uint_fast8_t lodiv = 1U << n_lo;
	const uint_fast32_t fvco = lodiv * lo;

	/* higher divider and charge pump current required above
	 * 3.2GHz. Programming guide says these values (fbkdiv, n,
	 * maybe pump?) can be changed back after enable in order to
	 * improve phase noise, since the VCO will already be stable
	 * and will be unaffected. */
	uint8_t fbkdiv;
	if (fvco > 32000) {	// 3.2 GHz
		fbkdiv = 4;
		set_RFFC5071_PLLCPL(3);
	} else {
		fbkdiv = 2;
		set_RFFC5071_PLLCPL(2);
	}

	const uint_fast64_t tmp_n = ((uint_fast64_t) fvco << RFFC5071_FRACTIONAL_LENGTH) / (fbkdiv * REF_FREQ);

	const uint_fast16_t n = tmp_n >> RFFC5071_FRACTIONAL_LENGTH;
	const uint_fast16_t p1nmsb = (tmp_n >> 8) & 0xffff;
	const uint_fast8_t p1nlsb = (tmp_n >> 0) & 0xff;

#if 0 //WITHDEBUG	
	const uint_fast64_t tune_freq_hz = (tmp_n * REF_FREQ * fbkdiv * FREQ_ONE_MHZ) / ((uint_fast64_t) lodiv << RFFC5071_FRACTIONAL_LENGTH);

	debug_printf_P(PSTR("# lo=%ld n_lo=%d lodiv=%d fvco=%ld fbkdiv=%d n=%d tune_freq_khz=%ld\n"),
			(long) lo, n_lo, lodiv, (long) fvco, fbkdiv, n, (long)(tune_freq_hz / 1000));

#endif /* WITHDEBUG */
	/* Path 1 */
	set_RFFC5071_P1VCOSEL(0);
	set_RFFC5071_P1LODIV(n_lo);
	set_RFFC5071_P1N(n);
	set_RFFC5071_P1PRESC(fbkdiv >> 1);
	set_RFFC5071_P1NMSB(p1nmsb);
	set_RFFC5071_P1NLSB(p1nlsb);

	/* Path 2 */
	set_RFFC5071_P2VCOSEL(0);
	set_RFFC5071_P2LODIV(n_lo);
	set_RFFC5071_P2N(n);
	set_RFFC5071_P2PRESC(fbkdiv >> 1);
	set_RFFC5071_P2NMSB(p1nmsb);
	set_RFFC5071_P2NLSB(p1nlsb);


	rffc5071_regs_commit(target);

	//return tune_freq_hz;
}

/* !!!!!!!!!!! hz is currently ignored !!!!!!!!!!! */
void rffc5071_set_frequency(uint16_t mhz, int hz) {
	//uint32_t tune_freq;

	rffc5071_disable();
	/* tune_freq = */ rffc5071_config_synth_int(mhz);
	rffc5071_enable();

	//return tune_freq;
}

void rffc5071_set_gpo(uint8_t gpo)
{
	/* We set GPO for both paths just in case. */
	set_RFFC5071_P1GPO(gpo);
	set_RFFC5071_P2GPO(gpo);

	rffc5071_regs_commit(target);
}

#if 0
// test
int xmain(int ac, char **av)
{
	rffc5071_setup();
	rffc5071_tx(0);
	rffc5071_set_frequency(500, 0);
	rffc5071_set_frequency(525, 0);
	rffc5071_set_frequency(550, 0);
	rffc5071_set_frequency(1500, 0);
	rffc5071_set_frequency(1525, 0);
	rffc5071_set_frequency(1550, 0);
	rffc5071_disable();
	rffc5071_rx(0);
	rffc5071_disable();
	rffc5071_rxtx();
	rffc5071_disable();

	return 0;
}
#endif

/*
static void rffc5071_test(
	spitarget_t target
	)
{
	//debug_printf_P(PSTR("rffc5071_test()\n"));
	uint_fast8_t r;
	for (r = 0; r < RFFC5071_READBACK_REG; ++ r)
	//for (;;)
	{
		uint_fast16_t v = rffc5071_spi_read(r);
		debug_printf_P(PSTR("R%02X=%04X\n"), r, v);
	}
}
*/
#endif /* RFFC5071_C_INCLUDED */
