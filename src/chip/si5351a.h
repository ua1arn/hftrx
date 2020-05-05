/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef SI5351a_C_INCLUDED
#define SI5351a_C_INCLUDED

// 
// Author: Hans Summers, 2015
// Website: http://www.hanssummers.com
//
// A very very simple Si5351a demonstration
// using the Si5351a module kit http://www.hanssummers.com/synth
// Please also refer to SiLabs AN619 which describes all the registers to use
//
// See also https://github.com/etherkit/Si5351Arduino/tree/master/src
//

#define WITHSMARTDIVIDERS 1

#define SI5351a_DCO_BITS 10			 /* lower 10 bits is a fractional part */

#if 0

#if defined (PLL1_FRACTIONAL_LENGTH) && PLL1_FRACTIONAL_LENGTH != SI5351a_DCO_BITS
	#error Wrong PLL1_FRACTIONAL_LENGTH value
#endif /* defined (PLL1_FRACTIONAL_LENGTH) && PLL1_FRACTIONAL_LENGTH != SI5351a_DCO_BITS */
#if defined (PLL2_FRACTIONAL_LENGTH) && PLL2_FRACTIONAL_LENGTH != SI5351a_DCO_BITS
	#error Wrong PLL1_FRACTIONAL_LENGTH value
#endif /* defined (PLL2_FRACTIONAL_LENGTH) && PLL2_FRACTIONAL_LENGTH != SI5351a_DCO_BITS */
#if SYNTH_R1 != 1
	#error Wrong SYNTH_R1 value
#endif /* SYNTH_R1 != 1 */
#if DDS1_CLK_MUL != 1
	#error Wrong DDS1_CLK_MUL value
#endif /* DDS1_CLK_MUL != 1 */
#if DDS1_CLK_DIV != 1
	#error Wrong DDS1_CLK_DIV value
#endif /* DDS1_CLK_DIV != 1 */


#endif

#define SI5351a_PLL_INPUT_SOURCE	15			// Register definitions
#define SI5351a_CLK0_CONTROL	16
#define SI5351a_CLK1_CONTROL	17
#define SI5351a_CLK2_CONTROL	18
#define SI5351a_CLK3_CONTROL	19
#define SI5351a_CLK4_CONTROL	20
#define SI5351a_CLK5_CONTROL	21
#define SI5351a_CLK6_CONTROL	22		// D6=FBA_INT
#define SI5351a_CLK7_CONTROL	23		// D6=FBB_INT
#define SI5351a_SYNTH_PLL_A	26
#define SI5351a_SYNTH_PLL_B	34
#define SI5351a_SYNTH_MS_0		42
#define SI5351a_SYNTH_MS_1		50
#define SI5351a_SYNTH_MS_2		58
#define SI5351a_PLL_RESET		177
#define SI5351a_PLL_LOADCAP		183
#define SI5351a_FANOUT		187		//  Fanout Enable
#define SI5351a_SSC_R1			149		// Spread Spectrum Parameters 149..161

#define SI5351a_CLK0_PHOFF	165	// [6..0] : CLK0 Initial Phase Offset
#define SI5351a_CLK1_PHOFF	166	// [6..0] : CLK1 Initial Phase Offset
#define SI5351a_CLK2_PHOFF	167	// [6..0] : CLK2 Initial Phase Offset


#define SI5351a_R_DIV_1		0x00			// R-division ratio definitions
#define SI5351a_R_DIV_2		0x10
#define SI5351a_R_DIV_4		0x20
#define SI5351a_R_DIV_8		0x30
#define SI5351a_R_DIV_16	0x40
#define SI5351a_R_DIV_32	0x50
#define SI5351a_R_DIV_64	0x60
#define SI5351a_R_DIV_128	0x70

#define SI5351a_CLK_SRC_PLL_A	0x00
#define SI5351a_CLK_SRC_PLL_B	0x20

#define XTAL_FREQ	dds2ref			// Crystal frequency
//#define XTAL_FREQ	25000000uL			// Crystal frequency
//#define XTAL_FREQ	27000000uL			// Crystal frequency




//#define SI53xx_I2C_WRITE (0x62 * 2)		// Si5351A-B04486-GT I2C address for writing to the Si5351A - see https://www.silabs.com/internal-apps-management/ClockBuilderDocuments/Si5351A-B04486-GT_datasheet_addendum.pdf
#define SI53xx_I2C_WRITE 0xC0		// I2C address for writing to the Si5351A

#define SI53xx_I2C_READ  (SI53xx_I2C_WRITE | 1)		// I2C address for reading to the Si5351A

static void
si535x_SendRegister(uint_fast8_t reg, uint_fast8_t data)
{
	i2c_start(SI53xx_I2C_WRITE);
	i2c_write(reg);
	i2c_write(data);
	i2c_waitsend();
	i2c_stop();	
}

static void si535x_ReadRegister(uint_fast8_t reg, uint_fast8_t * data)
{
	i2c_start(SI53xx_I2C_WRITE);
	i2c_write_withrestart(reg);
	i2c_start(SI53xx_I2C_READ);
	i2c_read(data, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */
}


//
// Set up specified PLL with mult, num and denom
// mult is 15..90
// num is 0..1,048,575 (0xFFFFF)
// denom is 0..1,048,575 (0xFFFFF)
//
static void
si535x_setupPLL(
	uint_fast8_t pllbasereg, 
	uint_fast8_t mult,	// a
	uint_fast32_t num,	// b
	uint_fast32_t denom	// c in paragraph 3.2. (Feedback Multisynth Divider Equations) of AN619.pdf
	)
{
	const uint_fast32_t Pz = (uint32_t) ((uint64_t) 128 * num / denom);

	const uint_fast32_t P1 = (uint32_t) (128 * (uint32_t) mult + Pz - 512);
	const uint_fast32_t P2 = (uint32_t) (128 * num - denom * Pz);
	const uint_fast32_t P3 = denom;

	// Write Operation - Burst (Auto Address Increment)
	i2c_start(SI53xx_I2C_WRITE);
	i2c_write(pllbasereg);

	i2c_write((P3 & 0x0000FF00) >> 8);
	i2c_write((P3 & 0x000000FF));
	i2c_write((P1 & 0x00030000) >> 16);
	i2c_write((P1 & 0x0000FF00) >> 8);
	i2c_write((P1 & 0x000000FF));
	i2c_write(((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));
	i2c_write((P2 & 0x0000FF00) >> 8);
	i2c_write((P2 & 0x000000FF));

	i2c_waitsend();
	i2c_stop();	
}

//
// Set up MultiSynth with integer divider and R divider
// R divider is the bit value which is OR'ed onto the appropriate register, it is a #define in si5351a.h
//
static void
si535x_setupMultisynth(uint_fast8_t synth, uint_fast32_t divider, uint_fast8_t outRdiv)
{
	const uint_fast32_t P1 = 128 * divider - 512;	// 18-bit number is an encoded representation of the integer part of the Multi-SynthX divider
	const uint_fast32_t P2 = 0;						// P2 = 0, P3 = 1 forces an integer value for the divider
	const uint_fast32_t P3 = 1;

	// Write Operation - Burst (Auto Address Increment)
	i2c_start(SI53xx_I2C_WRITE);
	i2c_write(synth);

	i2c_write((P3 & 0x0000FF00) >> 8);				// MSx_P3[15:8]
	i2c_write((P3 & 0x000000FF));					// MSx_P3[7:0]
	i2c_write(((P1 & 0x00030000) >> 16) | outRdiv | (divider == 4 ? 0x0c : 0x00));	// Rx_DIV[2:0], MSx_DIVBY4[1:0], MSx_P1[17:16]
	i2c_write((P1 & 0x0000FF00) >> 8);				// MSx_P1[15:8]
	i2c_write((P1 & 0x000000FF));					// MSx_P1[7:0]
	i2c_write(((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));	// MSx_P3[19:16], MSx_P2[19:16]
	i2c_write((P2 & 0x0000FF00) >> 8);				// MSx_P2[15:8]
	i2c_write((P2 & 0x000000FF));					// MSx_P2[7:0]

	i2c_waitsend();
	i2c_stop();	
}

//
// Switches off Si5351a output
// Example: si5351aOutputOff(SI5351a_CLK0_CONTROL);
// will switch off output CLK0
//
/*
static void
si5351aOutputOff(uint_fast8_t clk)
{
	//si535x__init();
	
	si535x_SendRegister(clk, 0x80);		// Refer to SiLabs AN619 to see bit values - 0x80 turns off the output stage

	//si535x__exit();
}
*/
struct FREQ {
  uint8_t plldiv;	// должно быть чётное число Valid Multisynth divider ratios are 4, 6, 8,
  uint8_t outdiv;	// Rx Output Divider code (SI5351a_R_DIV_1..SI5351a_R_DIV_128)
  uint16_t divider;	// общий делитель
  uint32_t fmin;
  uint32_t fmax;
};

static const FLASHMEM struct FREQ freqs [] = {
  { 4, SI5351a_R_DIV_1,		1,		100000000,  200000000 },
  { 4, SI5351a_R_DIV_2,		2,		50000000,  100000000 },	
  { 4, SI5351a_R_DIV_4,		4,		25000000,  50000000 },
  { 4, SI5351a_R_DIV_8,		8,		12500000,  25000000 },
  { 4, SI5351a_R_DIV_16,	16,		6250000,  12500000 },
  { 4, SI5351a_R_DIV_32,	32,		3125000,  6250000 },
  { 4, SI5351a_R_DIV_64,	64,		1562500,  3125000 },
  { 4, SI5351a_R_DIV_128,	128,	2500,  1562500 },	// multysync out: 320 kHz and above
};

static pllhint_t si5351a_get_hint(
	const uint_fast32_t freq	// требуемая частота
	)
{
	uint_fast8_t high = (sizeof freqs / sizeof freqs [0]);
	uint_fast8_t low = 0;
	uint_fast8_t middle;	// результат поиска

	// Двоичный поиск
	while (low < high)
	{
		middle = (high - low) / 2 + low;
		if (freq < freqs [middle].fmin)	// нижняя граница не включается - для обеспечения формального попадания частоты DCO в рабочий диапазон
			low = middle + 1;
		else if (freq >= freqs [middle].fmax)
			high = middle;		// переходим к поиску в меньших индексах
		else
			goto found;
	}
#if 0
	display_at_P(0, 0, PSTR("[si5351a Err]"));
#endif

found: 
	// нужная комбинация делителей найдена. Программирование Si570/Si571
	;
	return (pllhint_t) middle;
}

/* получить делитель по коду hint */
static uint_fast16_t 
si5351a_get_divider(pllhint_t hint)
{
	return freqs [(uint_fast8_t) hint].divider;
}


static const uint_fast8_t pllbase [2] =
{
	SI5351a_SYNTH_PLL_A,
	SI5351a_SYNTH_PLL_B,
};

static const uint_fast8_t multisynchbase [2] =
{
	SI5351a_SYNTH_MS_0,
	SI5351a_SYNTH_MS_1,
};

static uint_fast8_t si5351aSetFrequencyX(
	uint_fast8_t clkout, 
	uint_fast32_t frequency, 
	pllhint_t hint
	)
{
	uint_fast32_t pllFreq;
	const uint_fast32_t xtalFreq = XTAL_FREQ;
	uint_fast32_t l;
	uint_fast64_t f;
	uint_fast8_t mult;
	uint_fast32_t num;
	uint_fast32_t denom;
	uint_fast32_t divider;

	frequency *= si5351a_get_divider(hint);

	// intermediate VCO frequency in the range of 600 to 900 MHz
	// Valid Multisynth divider ratios are 4, 6, 8,
	divider = 900000000uL / frequency;// Calculate the division ratio. 900,000,000 is the maximum internal 
									// PLL frequency: 900MHz
	if (divider % 2) 
		divider -= 1;		// Ensure an even integer division ratio

	pllFreq = divider * frequency;	// Calculate the pllFrequency: the divider * desired output frequency

	denom = 0x000FFFFF;				// For simplicity we set the denominator to the maximum 0x000FFFFF
	mult = pllFreq / xtalFreq;		// Determine the multiplier to get to the required pllFrequency
	l = pllFreq % xtalFreq;			// It has three parts:
	f = l;							// mult is an integer that must be in the range 15..90
	f *= (uint_fast64_t) denom;					// num and denom are the fractional parts, the numerator and denominator
	f /= xtalFreq;					// each is 20 bits (range 0..0x000FFFFF)
	num = f;						// the actual multiplier is  mult + num / denom

									// Set up PLL B with the calculated multiplication ratio
	si535x_setupPLL(pllbase [clkout], mult, num, denom);

	// Set up MultiSynth divider 1, with the calculated divider. 
	// The final R division stage can divide by a power of two, from 1..128. 
	// represented by constants SI5351a_R_DIV1 to SI5351a_R_DIV128 (see si5351a.h header file)
	// If you want to output frequencies below 1MHz, you have to use the 
	// final R division stage
	si535x_setupMultisynth(multisynchbase [clkout], divider, freqs [hint].outdiv);
	return mult;
}

// 
// Set CLK0 output ON and to the specified frequency
// Frequency is in the range 1MHz to 150MHz
// Example: si5351aSetFrequency(10000000);
// will set output CLK0 to 10MHz
//
// This example sets up PLL A
// and MultiSynth 0
// and produces the output on CLK0
//
static void si5351aSetFrequencyA(uint_fast32_t frequency)
{
	static uint_fast8_t skipreset;
	static uint_fast8_t oldmult;
	static pllhint_t oldhint = (pllhint_t) -1;

	const pllhint_t hint = si5351a_get_hint(frequency);
	const uint_fast8_t mult = si5351aSetFrequencyX(0, frequency, hint);	// called from synth_lo1_setfrequ

	if (skipreset == 0 || mult != oldmult || hint != oldhint)
	{
		si535x_SendRegister(SI5351a_PLL_RESET, 0x20);	// PLL A reset	
		// Finally switch on the CLK1 output (0x4F)
		// and set the MultiSynth0 input to be PLL A
		si535x_SendRegister(SI5351a_CLK0_CONTROL, 0x4F | SI5351a_CLK_SRC_PLL_A);

		skipreset = 1;
		oldmult = mult;
		oldhint = hint;
	}
}

static void si5351aSetFrequencyB(uint_fast32_t frequency)
{
	static uint_fast8_t skipreset;
	static uint_fast8_t oldmult;
	static pllhint_t oldhint = (pllhint_t) -1;

	if (0 == frequency)
	{
		si535x_SendRegister(SI5351a_CLK1_CONTROL, 0x80 | 0x4F | SI5351a_CLK_SRC_PLL_B);
		skipreset = 0;	// запрос на переинициализацию выхода
		return;
	}

	const pllhint_t hint = si5351a_get_hint(frequency);
	const uint_fast8_t mult = si5351aSetFrequencyX(1, frequency, hint);	// called from synth_lo1_setfrequ

	// Reset the PLL. This causes a glitch in the output. For small changes to 
	// the parameters, you don't need to reset the PLL, and there is no glitch
	if (skipreset == 0 || mult != oldmult || hint != oldhint)
	{
		si535x_SendRegister(SI5351a_PLL_RESET, 0x80);	// PLL B reset	
		// Finally switch on the CLK1 output (0x4F)
		// and set the MultiSynth1 input to be PLL B
		si535x_SendRegister(SI5351a_CLK1_CONTROL, 0x4F | SI5351a_CLK_SRC_PLL_B);

		skipreset = 1;
		oldmult = mult;
		oldhint = hint;
	}

}

// Function template
static void si5351aQuadrature(void)
{
	si535x_SendRegister(SI5351a_CLK0_PHOFF, 0x00);
	si535x_SendRegister(SI5351a_CLK1_PHOFF, 0x00);
}

static void si5351aInitialize(void)
{
	// с этими строками частокол пораженок
	//si535x_SendRegister(SI5351a_CLK6_CONTROL, 0x40);	// D6=FBA_INT
	//si535x_SendRegister(SI5351a_CLK7_CONTROL, 0x40);	// D6=FBB_INT

	si535x_SendRegister(SI5351a_PLL_INPUT_SOURCE, 0x00);	// todo: select proper CLKIN_DIV[1..0] field value (Si5351C only)
	si535x_SendRegister(SI5351a_PLL_LOADCAP, 0xC0 | 0x12);
	//si535x_SendRegister(SI5351a_SSC_R1, 0x00);	// SSC_EN=0
	//si535x_SendRegister(SI5351a_FANOUT, 0x10);	// MS_FANOUT_EN=1
	// Finally switch on the CLK0 output (0x4F)
	// and set the MultiSynth0 input to be PLL A
	//si535x_SendRegister(SI5351a_CLK0_CONTROL, 0x4F | SI5351a_CLK_SRC_PLL_A);

	//si535x_SendRegister(SI5351a_CLK2_CONTROL, 0x40);	// D7=CLKx_PDN=1
	//si535x_SendRegister(SI5351a_CLK3_CONTROL, 0x40);	// D7=CLKx_PDN=1
	//si535x_SendRegister(SI5351a_CLK4_CONTROL, 0x40);	// D7=CLKx_PDN=1
	//si535x_SendRegister(SI5351a_CLK5_CONTROL, 0x40);	// D7=CLKx_PDN=1
	//si535x_SendRegister(SI5351a_CLK6_CONTROL, 0x40);	// D7=CLKx_PDN=1
	//si535x_SendRegister(SI5351a_CLK7_CONTROL, 0x40);	// D7=CLKx_PDN=1

}


#endif /* SI5351a_C_INCLUDED */
