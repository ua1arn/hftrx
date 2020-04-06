/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Karen Tadevosyan, RA3APW, Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Hittite HMC830, HMC832
//
#ifndef HMC830_C_INCLUDED
#define HMC830_C_INCLUDED

/*
	Need Open mode need: CS active low.

	pll_operating_guide_rf_vcos.pdf:

	4.17.2 HMCSPI Protocol Decision after Power-On Reset
	On power up both types of modes are active and listening.
	A decision to select the desired SPI protocol is made on the first occurrence of SEN or SCLK following a
	hard reset, after which the protocol is fixed and only changeable by cycling the power OFF and ON.
	a. If a rising edge on SEN is detected first HMC Mode is selected.
	b. If a rising edge on SCLK is detected first Open mode is selected.

*/
#define HMC830_SPIMODE SPIC_MODE3	// MODE3 only supported

#define HMC830_FRACTIONAL_LENGTH 24	/* lower 24 bits is a fractional part */
//#define HMC830_FTW_FRACTMASK ((1UL << HMC830_FRACTIONAL_LENGTH) - 1)

#if PLL1_FRACTIONAL_LENGTH != HMC830_FRACTIONAL_LENGTH
	#error Wrong PLL1_FRACTIONAL_LENGTH value
#endif /* PLL1_FRACTIONAL_LENGTH != HMC830_FRACTIONAL_LENGTH */
#if DDS1_CLK_MUL != 1
	#error Wrong DDS1_CLK_MUL value
#endif /* DDS1_CLK_MUL != 1 */

static void 
prog_hmc830_write(
	spitarget_t target,		/* addressing to chip */
	uint_fast8_t r,
	uint_fast32_t v
	)
{
	rbtype_t rbbuff [4] = { 0 };

	RBVAL8(030, v >> 16);
	RBVAL8(020, v >> 8);
	RBVAL8(010, v >> 0);

	RBVAL(003, r, 5);		// r4..r0 bits
	RBVAL(000, 0x00, 3);	// a2..a0 bits - LHTTPLL Address bits are "000"


	spi_select(target, HMC830_SPIMODE);

	spi_progval8_p1(target, rbbuff [0]);
	spi_progval8_p2(target, rbbuff [1]);
	spi_progval8_p2(target, rbbuff [2]);
	spi_progval8_p2(target, rbbuff [3]);
	spi_complete(target);

	spi_unselect(target);
}

/* VCO Serial Port Interface (SPI) */
/* For field values see 6.0 VCO Subsystem Register Map of pll_operating_guide_rf_vcos.pdf */

static void 
prog_hmc830_VSPI(
	spitarget_t target,		/* addressing to chip */
	uint_fast16_t vco_data,	/* 9 bits */
	uint_fast8_t VCO_Reg	/* 4 bits */
	)
{
	enum { VCO_ID = 0 }; /* 3 bits (000 or 101 depending upon the subsystem) */
	const uint_fast16_t v =
		((uint_fast16_t) vco_data << 7) |
		((uint_fast16_t) VCO_Reg << 3) |
		((uint_fast16_t) VCO_ID);

	prog_hmc830_write(target, 0x05, v);
}


struct FREQR {
  uint16_t divider;	// общий делитель
  uint16_t VCO_Reg02h;	// параметры для программирования
  uint32_t fmin;
  uint32_t fmax;			// эта частота не входит в диапазон
};

#if 1

static const struct FREQR FLASHMEM hmc830_freqs [] = {
	{ 1,    0x1C0 | 1,	1500000000UL, 3000000000UL, },
	{ 2,    0x1C0 | 2,	 750000000UL, 1500000000UL, },
	{ 4,    0x0C0 | 4,	 500000000UL,  750000000UL, },
	{ 6,    0x0C0 | 6,	 375000000UL,  500000000UL, }, /* 375.000000..500.000000 */
	{ 8,    0x0C0 | 8,	 250000000UL,  375000000UL, }, /* 250.000000..375.000000 */
	{ 12,	0x0C0 | 12,  125000000UL,  250000000UL, }, /* 125.000000..250.000000 */
	{ 16,   0x0C0 | 16,	  93750000UL,  125000000UL, },
	{ 32,   0x0C0 | 32,	  75000000UL,   93750000UL, },
    { 40, 	0x0C0 | 40,   37500000UL,   75000000UL, }, /* 37.500000..75.000000 */
	{ 62,	0x0C0 | 62,   24193548UL,   37500000UL, }, /* 24.193548..37.500000 */
};

#else

/* Полная таблица */
static const struct FREQR FLASHMEM hmc830_freqs [] =
{
        { 1, 0x1C0 | 1,         1500000000UL, 3000000000UL, }, /* 1500.000000..3000.000000 */
        { 2, 0x1C0 | 2,          750000000UL, 1500000000UL, }, /* 750.000000..1500.000000 */
        { 4, 0x0C0 | 4,          375000000UL,  750000000UL, }, /* 375.000000..750.000000 */
        { 6, 0x0C0 | 6,          250000000UL,  500000000UL, }, /* 250.000000..500.000000 */
        { 8, 0x0C0 | 8,          187500000UL,  375000000UL, }, /* 187.500000..375.000000 */
        { 10, 0x0C0 | 10,        150000000UL,  300000000UL, }, /* 150.000000..300.000000 */
        { 12, 0x0C0 | 12,        125000000UL,  250000000UL, }, /* 125.000000..250.000000 */
        { 14, 0x0C0 | 14,        107142857UL,  214285714UL, }, /* 107.142857..214.285714 */
        { 16, 0x0C0 | 16,         93750000UL,  187500000UL, }, /* 93.750000..187.500000 */
        { 18, 0x0C0 | 18,         83333333UL,  166666666UL, }, /* 83.333333..166.666666 */
        { 20, 0x0C0 | 20,         75000000UL,  150000000UL, }, /* 75.000000..150.000000 */
        { 22, 0x0C0 | 22,         68181818UL,  136363636UL, }, /* 68.181818..136.363636 */
        { 24, 0x0C0 | 24,         62500000UL,  125000000UL, }, /* 62.500000..125.000000 */
        { 26, 0x0C0 | 26,         57692307UL,  115384615UL, }, /* 57.692307..115.384615 */
        { 28, 0x0C0 | 28,         53571428UL,  107142857UL, }, /* 53.571428..107.142857 */
        { 30, 0x0C0 | 30,         50000000UL,  100000000UL, }, /* 50.000000..100.000000 */
        { 32, 0x0C0 | 32,         46875000UL,   93750000UL, }, /* 46.875000..93.750000 */
        { 34, 0x0C0 | 34,         44117647UL,   88235294UL, }, /* 44.117647..88.235294 */
        { 36, 0x0C0 | 36,         41666666UL,   83333333UL, }, /* 41.666666..83.333333 */
        { 38, 0x0C0 | 38,         39473684UL,   78947368UL, }, /* 39.473684..78.947368 */
        { 40, 0x0C0 | 40,         37500000UL,   75000000UL, }, /* 37.500000..75.000000 */
        { 42, 0x0C0 | 42,         35714285UL,   71428571UL, }, /* 35.714285..71.428571 */
        { 44, 0x0C0 | 44,         34090909UL,   68181818UL, }, /* 34.090909..68.181818 */
        { 46, 0x0C0 | 46,         32608695UL,   65217391UL, }, /* 32.608695..65.217391 */
        { 48, 0x0C0 | 48,         31250000UL,   62500000UL, }, /* 31.250000..62.500000 */
        { 50, 0x0C0 | 50,         30000000UL,   60000000UL, }, /* 30.000000..60.000000 */
        { 52, 0x0C0 | 52,         28846153UL,   57692307UL, }, /* 28.846153..57.692307 */
        { 54, 0x0C0 | 54,         27777777UL,   55555555UL, }, /* 27.777777..55.555555 */
        { 56, 0x0C0 | 56,         26785714UL,   53571428UL, }, /* 26.785714..53.571428 */
        { 58, 0x0C0 | 58,         25862068UL,   51724137UL, }, /* 25.862068..51.724137 */
        { 60, 0x0C0 | 60,         25000000UL,   50000000UL, }, /* 25.000000..50.000000 */
        { 62, 0x0C0 | 62,         24193548UL,   48387096UL, }, /* 24.193548..48.387096 */
};

#endif

// Для данной микросхемы hint равен индексу в hmc830_freqs
static pllhint_t 
prog_hmc830_get_hint(
	uint_fast32_t freq		/* требуемая частота на выходе PLL1 */
	)
{
	uint_fast8_t high = (sizeof hmc830_freqs / sizeof hmc830_freqs [0]);
	uint_fast8_t low = 0;
	uint_fast8_t middle;	// результат поиска

	// Двоичный поиск
	while (low < high)
	{
		middle = (high - low) / 2 + low;
		if (freq < hmc830_freqs [middle].fmin)	// нижняя граница не включается - для обеспечения формального попадания частоты DCO в рабочий диапазон
			low = middle + 1;
		else if (freq >= hmc830_freqs [middle].fmax)
			high = middle;		// переходим к поиску в меньших индексах
		else
			goto found;
	}
#if 0
	display_at_P(0, 0, PSTR("[HMC830 Err]"));
	return 0;		/* требуемую частоту невозожно получить */
#endif

found: 
	// нужная комбинация делителей найдена.
	;
	return (pllhint_t) middle;
}

static uint_fast16_t 
prog_hmc830_get_divider(pllhint_t hint)
{
	return hmc830_freqs [(uint_fast8_t) hint].divider;
}


static void df2(
	int d,
	unsigned long n,
	unsigned long fract
	)
{
	char s [21];
	local_snprintf_P(s, 21, PSTR("/%-2d %02lX.%06lX"), d, n, fract);
	display_at(0, 1, s);
}

/* 
	Для случая PLL с дробным делителем передается полное значение,
	которое перед выдачей разделяется на целую и дробную часть.
*/
static void 
prog_hmc830_n(
	spitarget_t target,		/* addressing to chip */
	pllhint_t hint,
	const ftw_t * n
	)
{
	//const unsigned long PLL1_FRACTIONAL_MASK = (uint_fast32_t) (((uint_fast64_t) 1 << PLL1_FRACTIONAL_LENGTH) - 1);
	const ftw_t nv64 = * n;

	prog_hmc830_write(target, 0x03, nv64 >> HMC830_FRACTIONAL_LENGTH); // Integer Frequency Register
	prog_hmc830_write(target, 0x04, nv64 /* & PLL1_FRACTIONAL_MASK */); // Fractional Frequency Register

#if HMC830_FLAVOR_HMC832
	// +++HMC832LP6GE 
	prog_hmc830_VSPI(target, hmc830_freqs [hint].VCO_Reg02h & 0x1F, 0x02);	 /* was: 0000e090 VCO Output Divider, 1, 2, 4, 6, 8, 10, .. 62 */
	// ---HMC832LP6GE 
#else /* HMC830_FLAVOR_HMC832 */
	// +++HMC830LP6GE 
	prog_hmc830_VSPI(target, hmc830_freqs [hint].VCO_Reg02h, 0x02);	 /* was: 0000e090 VCO Output Divider, 1, 2, 4, 6, 8, 10, .. 62 */
	// ---HMC830LP6GE 
#endif /* HMC830_FLAVOR_HMC832 */

	// debug print
	//df2(prog_hmc830_get_divider(hint), nv64 >> HMC830_FRACTIONAL_LENGTH, nv64 & PLL1_FRACTIONAL_MASK);
}

static void 
prog_hmc830_initialize(
	spitarget_t target		/* addressing to chip */
	)
{
	// Setup the HMC830
	prog_hmc830_write(target, 0x00, 0x000020);	// Reset. Not required but good programming practice
	prog_hmc830_write(target, 0x01, 0x000002);	// ! Enables Register
	prog_hmc830_write(target, 0x02, r1_ph);		// ! Reference Divider Register R=1

	// 6.5 VCO_Reg 04h, VCO_Reg 05h, VCO_Reg 06h Wideband Devices Only
	// Part # 		VCO Reg04h 	VCO Reg05h 	VCO Reg06h
	// HMC829LP6GE 	B6A0h 		7E28h 		Default (7FB0h)
	// HMC830LP6GE 	60A0h 		1628h 		Default (7FB0h)
	// HMC1032LP6GE 60A0h 		1628h 		Default (7FB0h)
	// HMC1034LP6GE 60A0h 		1628h 		Default (7FB0h)
#if HMC830_FLAVOR_HMC832
	// +++HMC832LP6GE 
	prog_hmc830_VSPI(target, 170, 0x05);
	prog_hmc830_VSPI(target, 201, 0x04);
	prog_hmc830_VSPI(target, 255, 0x06);
	// ---HMC832LP6GE 
#else /* HMC830_FLAVOR_HMC832 */
	// +++HMC830LP6GE 
	prog_hmc830_VSPI(target, 0x02C, 0x05);	 /* was: 00001628 VCO Subsystem */
	prog_hmc830_VSPI(target, 0x0C1, 0x04);	 /* was: 000060a0 VCO Subsystem */
	prog_hmc830_VSPI(target, 0x0FF, 0x06);	 /* was: 00007fb0 VCO Subsystem */
	// ---HMC830LP6GE 
#endif /* HMC830_FLAVOR_HMC832 */
	//prog_hmc830_VSPI(target, 0x1C0 | 1, 0x02);	 /* was: 0000e090 VCO Output Divider, 1, 2, 4, 6, 8, 10, .. 62 */
#if HMC830_FLAVOR_HMC832
	prog_hmc830_VSPI(target, 0x05e, 0x03);		/* was: 00002818 Differential Output */
#else /* HMC830_FLAVOR_HMC832 */
	prog_hmc830_VSPI(target, 0x050, 0x03);		/* was: 00002818 Differential Output */
	//prog_hmc830_VSPI(target, 0x051, 0x03);	/* was: 00002898 Single-ended output */
#endif /* HMC830_FLAVOR_HMC832 */
	prog_hmc830_VSPI(target, 0x000, 0x00);		/* was: 00000000 VCO Subsystem */

	prog_hmc830_write(target, 0x06, 0x200B4A);	// ! (Default 200B4Ah) Delta Sigma Modulator Configuration Register - Fractional Mode      
	//prog_hmc830_write(target, 0x06, 0x030F4A); // Delta Sigma Modulator Configuration Register - Fractional Mode      
	prog_hmc830_write(target, 0x07, 0x00014D);	// ! (Default 00014Dh) Lock Detect Register
	prog_hmc830_write(target, 0x08, 0xC1BEFF);	// ! (Default C1BEFFh) Analog Control Enable Register
	prog_hmc830_write(target, 0x09, 0x547FFF);	// ! (Default 403264h) Charge Pump Register 2.54mA with 405uA Down CP Offset current
	//prog_hmc830_write(target, 0x09, 0x5CBFFF); // Charge Pump Register 2.54mA with 570uA Down CP Offset current
	prog_hmc830_write(target, 0x0A, 0x002046);	// ! (Default 002205h) VCO AutoCAL Configuration Register

	prog_hmc830_write(target, 0x0B, 0x07C061);	// ! (Default 0F8061h) Phase Frequency Detector Register
	//prog_hmc830_write(target, 0x0B, 0x07C021);	// ! (Default 0F8061h) Phase Frequency Detector Register
	//prog_hmc830_write(target, 0x0B, 0x0FC061); // Phase Frequency Detector Register

	prog_hmc830_write(target, 0x0C, 0x000000);	// ! Fine Frequency Correction Register (0, 1, or 2..16383)
	//prog_hmc830_write(target, 0x0F, 0x000081); // Configure LD/SDO to output LD always (auto-mux to SDO during SPI reads
	prog_hmc830_write(target, 0x0F, 0x000001);	// ! Configure LD/SDO to output LD always

	//prog_hmc830_write(target, 0x03, 0x000028); // Integer Frequency Register
	//prog_hmc830_write(target, 0x04, 0x800000); // Fractional Frequency Register

	//prog_hmc830_write(target, 0x10, 0x000023);	// ! RO (Default 000020h)
	//prog_hmc830_write(target, 0x11, 0x08000D);	// ! RO (Default 7FFFFh) 
	//prog_hmc830_write(target, 0x12, 0x000003);	// ! RO (Default 000000h)
	//prog_hmc830_write(target, 0x13, 0x001259);	// ! RO (Default 1259h)

	/*
	// Setup the HMC830 Circuit on EVB
	// Program output for 2000MHz Integer Mode Operation with 50MHz Reference
	prog_hmc830_write(target, 0x0, 0x000020); // Reset. Not required but good programming practice
	prog_hmc830_write(target, 0x1, 0x000002); // Enables Register
	//prog_hmc830_write(target, 0x2, 0x000001); // Reference Divider Register R=1
	prog_hmc830_write(target, 0x5, 0x001628); // VCO Subsystem
	prog_hmc830_write(target, 0x5, 0x0060A0); // VCO Subsystem
	prog_hmc830_write(target, 0x5, 0x00E090); // VCO Subsystem VCO Output Divider = 1
	prog_hmc830_write(target, 0x5, 0x002818); // VCO Subsystem Differential Output.  For Single-ended output, write 2898h
	prog_hmc830_write(target, 0x5, 0x000000); // VCO Subsystem
	prog_hmc830_write(target, 0x6, 0x0303CA); // Delta Sigma Modulator Configuration Register - Integer Mode     
	prog_hmc830_write(target, 0x7, 0x00014D); // Lock Detect Register
	prog_hmc830_write(target, 0x8, 0xC1BEFF); // Analog Control Enable Register
	prog_hmc830_write(target, 0x9, 0x003FFF); // Charge Pump Register 2.54mA CP current with 0 CP Offset current
	prog_hmc830_write(target, 0xA, 0x002046); // VCO AutoCAL Configuration Register
	prog_hmc830_write(target, 0xB, 0x07C061); // Phase Frequency Detector Register
	prog_hmc830_write(target, 0xC, 0x000000); // Fine Frequency Correction Register
	prog_hmc830_write(target, 0xF, 0x000081); // Configure LD/SDO to output LD always (auto-mux to SDO during SPI reads
	prog_hmc830_write(target, 0x3, 0x000028); // Integer Frequency Register.  When this register is written VCO Auto-cal is initiated 
	*/
}
#endif /* HMC830_C_INCLUDED */
