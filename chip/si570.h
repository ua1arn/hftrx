/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef SI570_C_INCLUDED
#define SI570_C_INCLUDED

static uint_fast8_t si570_reg /* = 0x07 */; // Control of devices with 20 ppm and 50 ppm stability

#define SI570_DCO_BITS 28			 /* lower 28 bits is a fractional part */
//#define SI570_FOSC_MIN 		(4850000000ul)	/* 4850 MHz  */
//#define SI570_FOSC_MAX 		(5670000000ul)	 /* 5670 MHz  */
#define SI570_ADDRESS_W	0xAA	/* I2C target address for write */ 
#define SI570_ADDRESS_R	(SI570_ADDRESS_W | 0x01)	/* I2C target address for read */ 

#define SI570_REG_135 135
	#define SI570_STATUS_NEWDCO			0x40	/* бит захвата новой частоты */
	#define SI570_STATUS_RECALL			0x01	/* Recall NVM status */


#define SI570_XTAL_INIT	114285000UL	// 114.285 MHz
#define SI570_REG_137 137

#if PLL1_FRACTIONAL_LENGTH != SI570_DCO_BITS
	#error Wrong PLL1_FRACTIONAL_LENGTH value
#endif /* PLL1_FRACTIONAL_LENGTH != SI570_DCO_BITS */
#if SYNTH_R1 != 1
	#error Wrong SYNTH_R1 value
#endif /* SYNTH_R1 != 1 */
#if DDS1_CLK_MUL != 1
	#error Wrong DDS1_CLK_MUL value
#endif /* DDS1_CLK_MUL != 1 */
#if DDS1_CLK_DIV != 1
	#error Wrong DDS1_CLK_DIV value
#endif /* DDS1_CLK_DIV != 1 */

#if ! defined (WITHTWIHW) && ! defined (WITHTWISW)
	#error WITHTWIHW or WITHTWISW should be defined
#endif

/* проверяем, попали ли в диапазон частот */
static uint_fast8_t
si570_verify_xtall(
	uint_fast32_t freq		// расчитанная частота опорника - должно быть 114.285 MHz +/- 2 ppt
	)
{
	return 
		freq <= (uint_fast32_t) ((uint_fast64_t) SI570_XTAL_INIT * 1005 / 1000) &&
		freq >= (uint_fast32_t) ((uint_fast64_t) SI570_XTAL_INIT * 995 / 1000);
}


static uint_fast8_t
si570_get_status(void)
{
	uint_fast8_t v;

	//TP();
	i2c_start(SI570_ADDRESS_W);
	i2c_write_withrestart(SI570_REG_135);		// Register 135
	i2c_start(SI570_ADDRESS_R);
	i2c_read(& v, I2C_READ_ACK_NACK);	/* чтение первого и единственного байта ответа */
	//TP();

	return v;
}

static void 
si570_freeze_dco(void)
{
	//TP();
	i2c_start(SI570_ADDRESS_W);
	i2c_write(SI570_REG_137);
	i2c_write(0x10);	// freeze DCO bit
	i2c_waitsend();
	i2c_stop();
	//TP();
}

static void 
si570_unfreeze_dco(void)
{
	//TP();
	i2c_start(SI570_ADDRESS_W);
	i2c_write(SI570_REG_137);
	i2c_write(0x00);	// freeze DCO bit
	i2c_waitsend();
	i2c_stop();
	//TP();
}


static void 
si570_newfreq_dco(void)
{
	//TP();
	i2c_start(SI570_ADDRESS_W);
	i2c_write(SI570_REG_135);
	i2c_write(SI570_STATUS_NEWDCO);	// New Frequency Applied bit
	i2c_waitsend();
	i2c_stop();
	//TP();
}

static void 
si570_freeze_m(void)
{
	//TP();
	i2c_start(SI570_ADDRESS_W);
	i2c_write(SI570_REG_135);
	i2c_write(0x20);	// freeze M bit
	i2c_waitsend();
	i2c_stop();
	//TP();
}

static void 
si570_unfreeze_m(void)
{
	//TP();
	i2c_start(SI570_ADDRESS_W);
	i2c_write(SI570_REG_135);
	i2c_write(0x00);	// unfreeze M bit
	i2c_waitsend();
	i2c_stop();
	//TP();
}


static void 
si570_recal(void)
{
	//TP();
	i2c_start(SI570_ADDRESS_W);
	i2c_write(SI570_REG_135);
	i2c_write(SI570_STATUS_RECALL);	// RECALL bit
	i2c_waitsend();
	i2c_stop();
	//TP();
}

// Эти две переменные не ststic для доступа к ним из меню.
static int_fast32_t si570_xtall_base; 
uint_fast16_t si570_xtall_offset = OSCSHIFT; 

int_fast32_t si570_get_xtall_base(void)
{
	return si570_xtall_base;
}

static uint_fast64_t anchorftw;
static uint_fast64_t anchorftw_bottom;
static uint_fast64_t anchorftw_top;
static pllhint_t anchor_hint;	// итндекс в таблице конфигураций freqs

/* сохранение текущего значения anchor и границ перестройки от него */
static void si570_setanchorftw(
	uint_fast64_t ftw
	)
{
	//uint_fast64_t ppt3p5 = ftw * 35 / 10000;
	const uint_fast64_t ppt3p5 = ftw * 7 / 2000;
	anchorftw = ftw;
	anchorftw_bottom = ftw - ppt3p5;
	anchorftw_top = ftw + ppt3p5;
}


// Инициализация микросхемы и программы расчёта управляющих слов.
// 
static uint_fast8_t
si570_hs_decode(uint_fast8_t code)
{
	switch (code)
	{
	case 0x00:	return 4;
	case 0x01:	return 5;
	case 0x02:	return 6;
	case 0x03:	return 7;
	// 0x04 - unused
	case 0x05:	return 9;
	// 0x06 - unused
	case 0x07:	return 11;
	}
	return 1;
}

void
si570_initialize(void)
{
	local_delay_ms(15);		// Ожидане после power-on
	si570_recal();		// востановить значения настроек "по-умолчанию".
	local_delay_ms(15);

	// Дождаться, когда установится выходная частота (вдруг, нельзя раньше времени программировать?)
	// В документации сказано, что этот бит исчесает после установления новой частоты.
	uint_fast8_t w = 255;
	while (w -- && (si570_get_status() & SI570_STATUS_NEWDCO) != 0)
		local_delay_us(40);

		//uint_fast8_t v0 = 0, v1 = 0;
		//uint_fast8_t v2, v3, v4, v5;
	uint_fast8_t addrindex;

	// Список стартовых адресов внутреннего адресного пространства Si570
	static const FLASHMEM uint_fast8_t addrs [] =
	{
		0x0d,
		0x07,
	};
	for (addrindex = 0; addrindex < sizeof addrs / sizeof addrs [0]; ++ addrindex)
	{
		uint_fast8_t freqindex;

		// Список стартовых частот Si570
		static const FLASHMEM uint_fast32_t inifreqs [] =
		{
			56320000UL,
			10000000UL,
			15000000UL,
		};
		uint_fast8_t v0 = 0, v1 = 0;
		uint_fast8_t v2, v3, v4, v5;

		si570_reg = addrs [addrindex];

		//TP();
		// чтение текущей частоты настройки DCO
		//uint_fast8_t err = 
		i2c_start(SI570_ADDRESS_W);		// ||
		i2c_write_withrestart(si570_reg);	// ||			// Register #7 or #13
		i2c_waitsend();
		i2c_start(SI570_ADDRESS_R); 	//  ||
		i2c_read(& v0, I2C_READ_ACK_1);	// ||
		i2c_read(& v1, I2C_READ_ACK);	// ||
		i2c_read(& v2, I2C_READ_ACK);	// ||
		i2c_read(& v3, I2C_READ_ACK);	// ||
		i2c_read(& v4, I2C_READ_ACK);	// ||
		i2c_read(& v5, I2C_READ_NACK);	// ||

		//TP();

		//char buff [21];
		//local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("%02X%02X%02X%02X%02X%02X"), v0, v1, v2, v3, v4, v5);
		//local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("%02X%02X%02X%02X%02X%02X"), v[0], v[1], v[2], v[3], v[4], v[5]);
		//display_at(0, 1, buff);
		//for (;;)
		//	;
		
		const uint_fast8_t hs_code = (v0 >> 5) & 0x07;
		const uint_fast8_t n = ((v0 & 0x1f) * 4 + (v1 & 0xc0) / 64) + 1;
		const uint_fast16_t divider = si570_hs_decode(hs_code) * n;

		si570_setanchorftw( 
			((uint_fast64_t) (v1 & 0x3f) << 32) |
			(
				(((uint_fast32_t) v2 << 24)) |
				(((uint_fast32_t) v3 << 16)) |
				(((uint_fast32_t) v4 << 8)) |
				(((uint_fast32_t) v5 << 0))
			)
			);


		if (divider == 0)
			continue;		// нет микросхемы

		for (freqindex = 0; freqindex < (sizeof inifreqs / sizeof inifreqs [0]); ++ freqindex)
		{
			// Расчёт по известному FTW, выходной частоте и делителю опорной частоты.
			//
			// ftw = (freq << bits) / xtall
			// ftw * xtall = freq << bits;
			// xtall = (freq << bits) / ftw
			const uint_fast32_t si570_xtall = (((uint_fast64_t) inifreqs [freqindex] * divider) << SI570_DCO_BITS) / anchorftw;

			if (si570_verify_xtall(si570_xtall))
			{
				si570_xtall_base = si570_xtall - OSCSHIFT;
				anchor_hint = si570_get_hint(inifreqs [freqindex]);
				goto allDone;
			}
		}
	}
	// Для случая, когда ни одно из предположений не оправдалос.
	si570_xtall_base = SI570_XTAL_INIT - OSCSHIFT;
	anchor_hint = (pllhint_t) 0;
	si570_reg = 0x07;
allDone:
	;

#if 0
	// Печать параметров на экране
	{
		//static unsigned  count;
		//count ++;
		char buff [22];
		//const unsigned hs_code = (v0 >> 5) & 0x07;
		//const unsigned n = ((v0 & 0x1f) * 4 + (v1 & 0xc0) / 64) + 1;
		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			//PSTR("%02X:%08lX,%u*%u"),
			PSTR("%02X%08lX"),
			 (unsigned) (anchorftw >> 32),
			 (unsigned long) anchorftw//,
				//si570_hs_decode(hs_code),
				//n
			 );
		display_setcolors(COLOR_WHITE, COLOR_BLACK);
		display_at(0, 0, buff);

		local_snprintf_P(buff, sizeof buff / sizeof buff [0],
			PSTR("[%lu]"),
			 (unsigned long) (si570_xtall_base + si570_xtall_offset)
			 );
		display_setcolors(COLOR_WHITE, COLOR_BLACK);
		display_at(0, 2, buff);
	}
	for (;;)
		;
#endif

}

struct FREQ {
  //uint_fast8_t hsdiv;		// не используется в алгоритме
  uint8_t n1;
  uint8_t hsdiv_code;
  uint16_t divider;	// общий делитель
  uint32_t fmin;
  uint32_t fmax;
};

#if (MODEL_73050 || MODEL_73050_IF0P5)

/* частичная таблица - нет необходимости формировать все частоты из допустимых для Si570/Si571 */
// todo: подправить границы перекрывающихся диапазонов - по возможности убрать их из диапазонов с ручной перестройкой

static const struct FREQ FLASHMEM freqs [] = {
  { /*  7,   */ 6, 0x03,    42,  115476190,  135000000 },	// 42.426190 - 61.950000
  { /*  4,   */ 12, 0x00,    48,  101041666,  118125000 },	// 27.991666 - 45.075000
  { /*  4,   */ 14, 0x00,    56,   86607142,  101041666 },	// 13.557142 - 27.991666	// верхняя граница отредактирована.
  { /*  4,   */ 16, 0x00,    64,   75781250,   88593750 },	// 2.731250 - 15.543750
  { /*  4,   */ 18, 0x00,    72,   67361111,   78750000 },	// ..5.700000
};

#elif (MODEL_64455 || MODEL_64455_IF0P5)

/* частичная таблица - нет необходимости формировать все частоты из допустимых для Si570/Si571 */
// todo: подправить границы перекрывающихся диапазонов - по возможности убрать их из диапазонов с ручной перестройкой

static const struct FREQ FLASHMEM freqs [] = {
  { /*  7,   */ 6, 0x03,    42,  115476190,  135000000 },
  { /*  4,   */ 12, 0x00,    48,  101041666,  118125000 },
  { /*  4,   */ 14, 0x00,    56,   86607142,  101250000 },
  { /*  4,   */ 16, 0x00,    64,   75781250,   88593750 },
  { /*  4,   */ 18, 0x00,    72,   67361111,   78750000 },
  { /*  6,   */ 14, 0x02,    84,   57738095,   67500000 },
};

#elif (MODEL_45_IF455 || MODEL_45_IF0P5)

/* частичная таблица - нет необходимости формировать все частоты из допустимых для Si570/Si571 */
// todo: подправить границы перекрывающихся диапазонов - по возможности убрать их из диапазонов с ручной перестройкой

static const struct FREQ FLASHMEM freqs [] = {
  { /*  4,   */ 16, 0x00,    64,   75781250,   88593750 },
  { /*  4,   */ 18, 0x00,    72,   67361111,   78750000 },
  { /*  6,   */ 14, 0x02,    84,   57738095,   67500000 },
  { /*  7,   */ 14, 0x03,    98,   49489795,   57857142 },
  { /*  4,   */ 28, 0x00,   112,   43303571,   50625000 },
};

#elif \
	(FQMODEL_45_IF6000 || FQMODEL_45_IF6000_UHF144 || FQMODEL_45_IF6000_UHF430 || \
	FQMODEL_45_IF8868 || FQMODEL_45_IF8868_UHF144 || FQMODEL_45_IF8868_UHF430) || \
	0

/* частичная таблица - нет необходимости формировать все частоты из допустимых для Si570/Si571 */
// todo: подправить границы перекрывающихся диапазонов - по возможности убрать их из диапазонов с ручной перестройкой

static const struct FREQ FLASHMEM freqs [] = {
  { /*  4,   */ 14, 0x00,    56,   86607142,  101250000 },
  { /*  4,   */ 16, 0x00,    64,   75781250,   88593750 },
  { /*  4,   */ 18, 0x00,    72,   67361111,   78750000 },
  { /*  6,   */ 14, 0x02,    84,   57738095,   67500000 },
  { /*  7,   */ 14, 0x03,    98,   49489795,   57857142 },
  { /*  4,   */ 28, 0x00,   112,   43303571,   50625000 },
};

#elif (FQMODEL_TRX8M || FQMODEL_TRX500K || FQMODEL_TRX455K)

/* частичная таблица - нет необходимости формировать все частоты из допустимых для Si570/Si571 */
/* Усечена как для случая применения микросхем с CMOS выходом (до 180 МГц) */

static const struct FREQ FLASHMEM freqs [] = {
  { /*  7,   */ 4, 0x03,    28,  173214285,  202500000 },		/* с этой стрроки начинаются диапазоны для CMOS версии Si570 */
  { /*  4,   */ 8, 0x00,    32,  151562500,  177187500 },
  { /*  6,   */ 6, 0x02,    36,  134722222,  157500000 },
  { /*  7,   */ 6, 0x03,    42,  115476190,  135000000 },
  { /*  4,   */ 12, 0x00,    48,  101041666,  118125000 },
  { /*  4,   */ 14, 0x00,    56,   86607142,  101250000 },
  { /*  4,   */ 16, 0x00,    64,   75781250,   88593750 },
  { /*  4,   */ 18, 0x00,    72,   67361111,   78750000 },
  { /*  6,   */ 14, 0x02,    84,   57738095,   67500000 },
  { /*  7,   */ 14, 0x03,    98,   49489795,   57857142 },
  { /*  4,   */ 28, 0x00,   112,   43303571,   50625000 },
  { /*  5,   */ 26, 0x01,   130,   37307692,   43615384 },
  { /*  5,   */ 30, 0x01,   150,   32333333,   37800000 },
  { /*  5,   */ 34, 0x01,   170,   28529411,   33352941 },
  { /*  9,   */ 22, 0x05,   198,   24494949,   28636363 },
  { /*  5,   */ 46, 0x01,   230,   21086956,   24652173 },
  { /*  7,   */ 38, 0x03,   266,   18233082,   21315789 },
  { /*  5,   */ 62, 0x01,   310,   15645161,   18290322 },
  { /*  4,   */ 90, 0x00,   360,   13472222,   15750000 },
  { /*  5,   */ 84, 0x01,   420,   11547619,   13500000 },
  { /*  5,   */ 98, 0x01,   490,    9897959,   11571428 },
  { /* 11,   */ 52, 0x07,   572,    8479020,    9912587 },
  { /*  9,   */ 74, 0x05,   666,    7282282,    8513513 },
  { /*  9,   */ 86, 0x05,   774,    6266149,    7325581 },
  { /* 11,   */ 82, 0x07,   902,    5376940,    6286031 },
  { /*  9,   */116, 0x05,  1044,    4645593,    5431034 },
  //{ /* 11,   */110, 0x07,  1210,    4008264,    4685950 },
  { /* 11,   */110, 0x07,  1210,    100,    4685950 },
};

#else

/* Полная таблица. Формировал Сергей Косинский. 24 марта 2011 года */
/* И её можно "усечь" в случае применения микросхем с CMOS выходом (до 180 МГц) */

static const struct FREQ FLASHMEM freqs [] = {
  { /*  5,   */ 1, 0x01,     5,  970000000, 1134000000 },
  { /*  6,   */ 1, 0x02,     6,  808333333,  945000000 },
  { /*  7,   */ 1, 0x03,     7,  692857142,  810000000 },
  { /*  4,   */ 2, 0x00,     8,  606250000,  708750000 },
  { /*  9,   */ 1, 0x05,     9,  538888888,  630000000 },
  { /*  5,   */ 2, 0x01,    10,  485000000,  567000000 },
  { /* 11,   */ 1, 0x07,    11,  440909090,  515454545 },
  { /*  6,   */ 2, 0x02,    12,  404166666,  472500000 },
  { /*  7,   */ 2, 0x03,    14,  346428571,  405000000 },
  { /*  4,   */ 4, 0x00,    16,  303125000,  354375000 },
  { /*  9,   */ 2, 0x05,    18,  269444444,  315000000 },
  { /*  5,   */ 4, 0x01,    20,  242500000,  283500000 },
  { /* 11,   */ 2, 0x07,    22,  220454545,  257727272 },
  { /*  4,   */ 6, 0x00,    24,  202083333,  236250000 },	
  { /*  7,   */ 4, 0x03,    28,  173214285,  202500000 },		/* с этой стрроки начинаются диапазоны для CMOS версии Si570 */
  { /*  4,   */ 8, 0x00,    32,  151562500,  177187500 },
  { /*  6,   */ 6, 0x02,    36,  134722222,  157500000 },
  { /*  7,   */ 6, 0x03,    42,  115476190,  135000000 },
  { /*  4,   */ 12, 0x00,    48,  101041666,  118125000 },
  { /*  4,   */ 14, 0x00,    56,   86607142,  101250000 },
  { /*  4,   */ 16, 0x00,    64,   75781250,   88593750 },
  { /*  4,   */ 18, 0x00,    72,   67361111,   78750000 },
  { /*  6,   */ 14, 0x02,    84,   57738095,   67500000 },
  { /*  7,   */ 14, 0x03,    98,   49489795,   57857142 },
  { /*  4,   */ 28, 0x00,   112,   43303571,   50625000 },
  { /*  5,   */ 26, 0x01,   130,   37307692,   43615384 },
  { /*  5,   */ 30, 0x01,   150,   32333333,   37800000 },
  { /*  5,   */ 34, 0x01,   170,   28529411,   33352941 },
  { /*  9,   */ 22, 0x05,   198,   24494949,   28636363 },
  { /*  5,   */ 46, 0x01,   230,   21086956,   24652173 },
  { /*  7,   */ 38, 0x03,   266,   18233082,   21315789 },
  { /*  5,   */ 62, 0x01,   310,   15645161,   18290322 },
  { /*  4,   */ 90, 0x00,   360,   13472222,   15750000 },
  { /*  5,   */ 84, 0x01,   420,   11547619,   13500000 },
  { /*  5,   */ 98, 0x01,   490,    9897959,   11571428 },
  { /* 11,   */ 52, 0x07,   572,    8479020,    9912587 },
  { /*  9,   */ 74, 0x05,   666,    7282282,    8513513 },
  { /*  9,   */ 86, 0x05,   774,    6266149,    7325581 },
  { /* 11,   */ 82, 0x07,   902,    5376940,    6286031 },
  { /*  9,   */116, 0x05,  1044,    4645593,    5431034 },
  //{ /* 11,   */110, 0x07,  1210,    4008264,    4685950 },
  { /* 11,   */110, 0x07,  1210,    100,    4685950 },
};
        
#endif

pllhint_t si570_get_hint(
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
	display_at_P(0, 0, PSTR("[Si570 Err]"));
#endif

found: 
	// нужная комбинация делителей найдена. Программирование Si570/Si571
	;
	return (pllhint_t) middle;
}

/* получить делитель по коду hint */
uint_fast16_t 
si570_get_divider(pllhint_t hint)
{
	return freqs [(uint_fast8_t) hint].divider;
}


void si570_n(
	pllhint_t hint, 
	const phase_t * value
	)
{
	const uint_fast64_t rftw = * value;
	// отслеживание изменения частоты по предварительно рассчитанным границам.
	const uint_fast8_t bigjump = (anchor_hint != hint) || (rftw >= anchorftw_top) || (rftw <= anchorftw_bottom);

	// Передача параметров в Si570
	if (bigjump != 0)
	{
		anchor_hint = hint;
		si570_setanchorftw(rftw);
		si570_freeze_dco();
	}
	else
	{
		si570_freeze_m();
	}

	const uint_fast8_t hs_div_code = freqs [hint].hsdiv_code;	// DCO High Speed Divider. 0 = /4, 1 = /5, 2 = /6. 3 = /7, 5 = /9, 7 = /11
	const uint_fast8_t n1 = freqs [hint].n1 - 1;				// CLKOUT Output Divider. 0 = /1, 127 = /128 
	const uint_fast32_t rfreqlow32 = rftw;						// оптимизация по скорости и размеру кода

	//TP();
	i2c_start(SI570_ADDRESS_W);
	i2c_write(si570_reg);		// Register #7 or #13
	i2c_write((hs_div_code << 5) | ((n1 & 0x7f) >> 2));
	i2c_write((n1 << 6) | ((rftw >> 32) & 0x3f));
	i2c_write(rfreqlow32 >> 24);
	i2c_write(rfreqlow32 >> 16);
	i2c_write(rfreqlow32 >> 8);
	i2c_write(rfreqlow32 >> 0);
	i2c_waitsend();
	i2c_stop();
	//TP();

	if (bigjump != 0)
	{
		si570_unfreeze_dco();
		si570_newfreq_dco();
		// Дождаться, когда установится выходная частота (вдруг, нельзя раньше времени программировать?)
		// В документации сказано, что этот бит исчезает после установления новой частоты.
		// и на это требуется не более 10 mS
		uint_fast8_t w = 255;
		while (w -- && (si570_get_status() & SI570_STATUS_NEWDCO) != 0)
			local_delay_us(40);
	}
	else
	{
		si570_unfreeze_m();
	}

#if 0
	if (bigjump != 0)
	{
		static unsigned  count;

		count ++;
		char buff [22];
		//unsigned char v1, v2;
		local_snprintf_P(buff, sizeof buff / sizeof buff [0], PSTR("swi: [%u]"), count );
		display_at(0, 1, buff);
	}
#endif

#if 0
	{
		static unsigned  count;
		const uint_fast32_t voofreqK = (unsigned long) ((uint_fast64_t) freq  * freqs [hint].divider / 1000);
		
		count ++;
		char buff [22];
		//unsigned char v1, v2;
		display_gotoxy(0, 6);
		local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
			PSTR("%2u,%3u,[%lu]"),
			 si570_hs_decode(freqs [hint].hsdiv_code), 
			 freqs [hint].n1, 
			 voofreqK
			 );
		display_string(buff, 0);

		local_snprintf_P(buff, sizeof buff / sizeof buff [0], 
			PSTR("[%lu],%u "),
			voofreqK / si570_hs_decode(freqs [hint].hsdiv_code) / freqs [hint].n1,
			count
			);

		display_gotoxy(0, 1);
		display_string(buff, 0);
	}
#endif
}

#endif /* SI570_C_INCLUDED */
