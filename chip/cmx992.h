/* $Id$ */
//
// cmx991/cmx992 chip interface
//
#ifndef CMX992_C_INCLUDED
#define CMX992_C_INCLUDED


// Write Only register;
#define CMX992_W_GEN_RESET	0x10 // General Reset Register (Address only, no data) Address $10
#define CMX992_W_GEN_CTRL	0x11 // General Control Register, 8-bit write only. Address $11
#define CMX992_W_RX_CTRL	0x12 // Rx Control Register, 8-bit write only. Address $12
#define CMX992_W_RX_MODE	0x13 // Rx Mode Register, 8-bit write only. Address $13
#define CMX992_W_TX_CTRL	0x14 // Tx Control Register, 8-bit write only. Address $14
#define CMX992_W_TX_MODE	0x15 // Tx Mode Register, 8-bit write only. Address $15
#define CMX992_W_TX_GAIN	0x16 // Tx Gain Register, 8-bit write only Address $16
#define CMX992_W_IFPLL_M	0x20 // IF PLL M Divider Register, 8-bit write only Address $20-$21
#define CMX992_W_IFPLL_N	0x22 // IF PLL N Divider Register, 8-bit write only Address $22-$23

// Read Only register;
#define CMX992_R_GEN_CTL	0xE1 // General Control Register, 8-bit read only. Address $E1
#define CMX992_R_RX_CTRL	0xE2 // Rx Control Register, 8-bit read only. Address $E2
#define CMX992_R_RX_MODE	0xE3 // Rx Mode Register, 8-bit read only. Address $E3
#define CMX992_R_TX_CTRL	0xE4 // Tx Control Register, 8-bit read only. Address $E4
#define CMX992_R_TX_MODE	0xE5 // Tx Mode Register, 8-bit read only. Address $E5
#define CMX992_R_TX_GAIN	0xE6 // Tx Gain Register, 8-bit read only Address $E6
#define CMX992_R_IFPLL_M	0xD0 // IF PLL M Divider Register, 8-bit read only Address $D0-$D1
#define CMX992_R_IFPLL_N	0xD2 // IF PLL N Divider Register, 8-bit read only Address $D2-$D3

#include "../display.h"
#include "../formats.h"


// 6.1 General Reset Command (CMX991/CMX992)

static 
void prog_cmx992_reset(
	spitarget_t target		/* addressing to chip */
	)
{
	prog_select(target);			// CS активен
	
	prog_val(target, CMX992_W_GEN_RESET, 8);	// address
	
	prog_unselect(target);		// CS неактивен


}

// 6.2 General Control Register (CMX991/CMX992)

static 
void prog_cmx992_general_control(	
	spitarget_t target		/* addressing to chip */
	)
{

	prog_select(target);			// CS активен
	
	prog_val(target, CMX992_W_GEN_CTRL, 8);	// address
	
	prog_bit(target, 0x01);		// D7: enable bias
	prog_bit(target, 0x00);		// D6: for IF > 75MHz then set IFH = С1Т, for IF < 75MHz use IFH = С0Т.
	prog_val(target, 0x00, 2);		// D5..D4: Output Mode Control: 0 - IQ mode, 1 - IF:I, 3: IF:Q output
	prog_val(target, 0x03, 2);		// D3..D2: VCO negative resistance, 0x00 - NR maximum
	prog_bit(target, 0x01);		// D1: VCO_Buff Enable
	prog_bit(target, 0x01);		// D0: VCO_NR Enable


	prog_unselect(target);		// CS неактивен


}

// 6.3 Rx Control Register (CMX991/CMX992)

static 
void prog_cmx992_rx_control(
	spitarget_t target,		/* addressing to chip */
	uint_fast8_t lna,		// lna output pin state
	uint_fast8_t lo1power2		// 0: lo1 /1, 1: /2, 2: /4
	)
{
	enum { RXLO1DIV1 = 0x01, RXLO1DIV2 = 0x00, RXLO1DIV4 = 0x02 };
	static const lo1divs [] =
	{
		RXLO1DIV1,
		RXLO1DIV2,
		RXLO1DIV4,
	};

	prog_select(target);			// CS активен
	
	prog_val(target, CMX992_W_RX_CTRL, 8);	// address
	
	prog_bit(target, 0x01);		// D7: mix1 powering
	prog_bit(target, 0x01);		// D6: I/Q power
	prog_bit(target, 0x00);		// D5: Enable baseband differential amplifiers was: "1"
	prog_bit(target, 0x01);		// D4: Enable SLI amplifier TODO: set to "0"
	prog_bit(target, lna != 0);		// D3: LNA power
	prog_val(target, lo1divs [lo1power2], 2);		// D2..D1: div, 
									// 0x00: /2, 
									// 0x01: /1, 
									// 0x02:/4, 
									// 0x03: do not use
	prog_bit(target, 0x01);		// D0: vbias output

	prog_unselect(target);		// CS неактивен


}

// 6.4 Rx Mode Register (CMX991/CMX992)

static 
void prog_cmx992_rx_mode(
	spitarget_t target		/* addressing to chip */
	)
{
	enum { if1path = 0 };

	prog_select(target);			// CS активен
	
	prog_val(target, CMX992_W_RX_MODE, 8);	// address
	
	prog_bit(target, if1path);		// D7: IF in: b7 selects the IF input, b7 = С0Т selects IFIP1 and b7 = С1Т selects IFIP2
	prog_bit(target, if1path);		// D6: mix out: b6 selects the IF output of the Rx 1st Mixer, b6 = С0Т selects MIXOUT1 and b6 = С1Т selects MIXOUT2.
	prog_bit(target, 0x01);		// D5: I/Q filter : Writing b5 = Т1Т I/Q Filter BW = 1MHz; Writing b5 = Т0Т I/Q Filter BW = 100kHz.
	prog_bit(target, 0x00);		// D4: Cal enable
	prog_val(target, 0x00, 4);		// D3..D0: VGA attenuation level = N * 6 dB, N = 0..8


	prog_unselect(target);		// CS неактивен


}

// 6.5 Tx Control Register (CMX991 only)

static 
void prog_cmx992_tx_control(
	spitarget_t target		/* addressing to chip */
	)
{
	prog_select(target);			// CS активен
	
	prog_val(target, CMX992_W_TX_CTRL, 8);	// address
	
	prog_bit(target, 0x00);		// D7: Reserved set to С0Т
	prog_bit(target, 0x01);		// D6: TxMix power
	prog_bit(target, 0x00);		// D5: Reserved set to С0Т
	prog_bit(target, 0x01);		// D4: I/Q Mod power
	prog_bit(target, 0x00);		// D3: Reserved set to С0Т
	prog_bit(target, 0x00);		// D2: 0Reserved set to С0Т
	prog_bit(target, 0x00);		// D1: TX freq LO range: Т0Т for frequency below 600MHz;
	prog_bit(target, 0x00);		// D0: TX I/Q out selection


	prog_unselect(target);		// CS неактивен


}

// 6.6 Tx Mode Register (CMX991 only)

static 
void prog_cmx992_tx_mode(
	spitarget_t target		/* addressing to chip */
	)
{
	prog_select(target);			// CS активен
	
	prog_val(target, CMX992_W_TX_MODE, 8);	// address
	
	prog_bit(target, 0x00);		// D7: reserved: 0
	prog_bit(target, 0x00);		// D6: reserved: 0
	prog_val(target, 0x00, 2);	// D5..D4: TX filter bandwidth
	prog_bit(target, 0x00);		// D3: reserved: 0
	prog_bit(target, 0x00);		// D2: tx mode b2: tx image rejecting sideband
	prog_bit(target, 0x00);		// D1: TX LO LO divider selection: 0: /2, 1: /4
	prog_bit(target, 0x00);		// D0: TX IF LO divider selection: 0: /4, 1: /2


	prog_unselect(target);		// CS неактивен


}

// 6.7 Tx Gain Register (CMX991 only)

static 
void prog_cmx992_tx_gain(
	spitarget_t target		/* addressing to chip */
	)
{
	prog_select(target);			// CS активен
	
	prog_val(target, CMX992_W_TX_GAIN, 8);	// address
	
	prog_val(target, 0x00, 2);		// D7..D6: TX gain mode
	prog_bit(target, 0x00);		// D5: reserved, 0
	prog_bit(target, 0x00);		// D4: reserved, 0
	prog_bit(target, 0x00);		// D3: reserved, 0
	prog_bit(target, 0x00);		// D2: reserved, 0
	prog_bit(target, 0x00);		// D1: reserved, 0
	prog_bit(target, 0x00);		// D0: reserved, 0


	prog_unselect(target);		// CS неактивен


}

// 6.8 IF PLL M Divider (CMX991/CMX992)
// Reference divider
// m = 2..8191

static 
void prog_cmx992_ifpll_m(
	spitarget_t target,		/* addressing to chip */
	const phase_t * m)
{
	prog_select(target);			// CS активен
	
	prog_val(target, CMX992_W_IFPLL_M + 0, 8);	// address
	prog_phbits(target, m, 8, 8);				// D7..D0: low byte of M divider
	//prog_val(target, (m >> 0) & 0xff, 8);		// D7..D0: low byte of M divider

	prog_unselect(target);		// CS неактивен

	prog_select(target);			// CS активен

	prog_val(target, CMX992_W_IFPLL_M + 1, 8);	// address
	
	prog_bit(target, 1);		// D7: PLL enable
	prog_bit(target, 0x00);		// D5: Should be 0 at wrtite, PLL lock state at read
	prog_bit(target, 1);		// D6: CP enable
	prog_phbits(target, m, 13, 5);				// D4..D0: High bits of M divider
	//prog_val(target, (m >> 8), 5);		// D4..D0: High bits of M divider

	prog_unselect(target);		// CS неактивен
}

// 6.9 PLL N Divider (CMX991/CMX992)
// Feedback divider
// n = 80..32767

static 
void prog_cmx992_ifpll_n(
	spitarget_t target,		/* addressing to chip */
	const phase_t * n
	)
{
	prog_select(target);			// CS активен
	
	prog_val(target, CMX992_W_IFPLL_N + 0, 8);	// address
	prog_phbits(target, n, 8, 8);				// D7..D0: low byte of N divider
	//prog_val(target, (n >> 0) & 0xff, 8);		// D7..D0: low byte of N divider

	prog_unselect(target);		// CS неактивен

	prog_select(target);			// CS активен
	
	prog_val(target, CMX992_W_IFPLL_N + 1, 8);	// address
	prog_bit(target, 0x00);		// D7: reserved
	prog_phbits(target, n, 15, 7);				// D6..D0: High bits of N divider
	//prog_val(target, (n >> 8), 7);		// D6..D0: High bits of N divider

	prog_unselect(target);		// CS неактивен

}

#if 1
static unsigned char prog_cmx992_read_reg(
	spitarget_t target,		/* addressing to chip */
	unsigned char addr)
{
	unsigned char v;

	prog_select(target);			// CS активен

	prog_val(target, addr, 8);	// address
	prog_spi_to_read(target);
	v = prog_read_byte(target);
	prog_spi_to_write(target);

	prog_unselect(target);		// CS неактивен
	return v;
}

void prog_cmx992_print(spitarget_t target)
{
	//spitarget_t target = targetpll2;		/* addressing to chip */
	struct state
	{
		unsigned char x, y;
		unsigned char addr;
		const char * name;
		unsigned char bytes;
	};

	static const struct state states [] =
	{
		{	0, 0, CMX992_R_GEN_CTL, "GEN_CTL", 1,	},
		{	0, 1, CMX992_R_RX_CTRL, "RX_CTRL", 1,	},
		{	0, 2, CMX992_R_RX_MODE, "RX_MODE", 1,	},
		{	0, 3, CMX992_R_TX_CTRL, "TX_CTRL", 1,	},
		{	0, 4, CMX992_R_TX_MODE, "TX_MODE", 1,	},
		{	0, 5, CMX992_R_TX_GAIN, "TX_GAIN", 1,	},
		{	0, 6, CMX992_R_IFPLL_M, "IFPLL_M", 2,	},
		{	0, 7, CMX992_R_IFPLL_N, "IFPLL_N", 2,	},

	};

	unsigned char i;
	
	for (i = 0; i < (sizeof states / sizeof states [0]); ++ i)
	{
		const struct state * const s = & states [i];
		char buff [5];
		//unsigned char v1, v2;
		display_gotoxy(s->x, s->y);
		display_string(s->name, 0);
		display_string(" ", 0);
		switch (s->bytes)
		{
		case 1:
			local_snprintf_P(buff, 5, PSTR("%02X"), prog_cmx992_read_reg(target, s->addr));
			display_string(buff, 0);
			break;
		case 2:
			local_snprintf_P(buff, 5, PSTR("%02X%02X"), prog_cmx992_read_reg(target, s->addr + 1), prog_cmx992_read_reg(target, s->addr + 0));
			display_string(buff, 0);
			break;
		}
	}
}

#endif

#if 1
void prog_cmx992_initialize(spitarget_t target)
{

	//spitarget_t target = targetpll2;		/* addressing to chip */

	prog_cmx992_reset(target);
	prog_cmx992_general_control(target);

	//const phase_t n = (107 * 4);	// Output freq = 42.8 MHz
	const phase_t n = (450 * 4);	// Output freq = 42.8 MHz
	const phase_t m = 200;	// Reference FREQ = 20.0 MHz
	prog_cmx992_ifpll_n(target, & n);	// Output freq = 171.2 MHz
	prog_cmx992_ifpll_m(target, & m);	// Reference FREQ = 20.0 MHz

	prog_cmx992_rx_control(target, 1, LO1_POWER2);
	prog_cmx992_rx_mode(target);

	for (;0;)
	{
		prog_cmx992_rx_control(target, 1, LO1_POWER2);
		local_delay_ms(100);
		prog_cmx992_rx_control(target, 0, LO1_POWER2);
		local_delay_ms(100);
	}

	//prog_cmx992_tx_control(target);
	//prog_cmx992_tx_mode(target);
	//prog_cmx992_tx_gain(target);

}
#endif

#endif /* CMX992_C_INCLUDED */
