/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

/* Проверенно с AD9834, работает с AD9833 и AD9838 */

#ifndef AD9834_C_INCLUDED
#define AD9834_C_INCLUDED


#if defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_AD9834)
	#if (DDS1_CLK_MUL != 1)
		#error AD9834 as DDS1 use x1 clock multiplier only
	#endif
#endif
#if defined (DDS2_TYPE) && (DDS2_TYPE == DDS_TYPE_AD9834)
	#if (DDS2_CLK_MUL != 1)
		#error AD9834 as DDS2 use x1 clock multiplier only
	#endif
#endif
#if defined (DDS3_TYPE) && (DDS3_TYPE == DDS_TYPE_AD9834)
	#if (DDS3_CLK_MUL != 1)
		#error AD9834 as DDS3 use x1 clock multiplier only
	#endif
#endif

#if FTW_RESOLUTION < 28
	#error FTW_RESOLUTION should be >= 28
#endif

#define AD9834_SPIMODE SPIC_MODE2	// MODE2 - work. Do not use MODE3 with this chip

/* complex init */

/* internal function */
/* this function is'not control chip select/fsync line */
static void 
//NOINLINEAT
prog_ad9834_control(
	spitarget_t target,		/* addressing to chip */
	uint_fast8_t reset,	/* 0/1 */
	uint_fast8_t comparator, 
	uint_fast8_t fsel		/* freq profile for output 0/1 */
	)
{
	// Set control word
	enum { hlb = 0 };
	const uint_fast8_t fselmask = fsel ? 0x08 : 0x00;

	// биты 15..8
	if (reset)
		spi_progval8_p1(target, 0x01);	// RESET = 1
	else
		spi_progval8_p1(target, 0x20 | fselmask);	// B28=1, FSEL=0 or FSEL=1, PSEL=0
	// Биты 7..0
#if 0
	// Выход наружу старшего бита SIN DAC
	// 0x40 - SLEEP12 = 1 powers down the on-chip DAC
	spi_progval8_p2(target, 0x68);	/* 0x68 - частота, на которую запрограммирован DDS, 0x60 - делённая пополам */
#else
	// нормальная работа, синусоидальный 
	spi_progval8_p2(target, comparator ? 0x38 : 0x00);
#endif
	spi_complete(target);
}

/* set frequency tuning word1 to DDS */
static void 
//NOINLINEAT
prog_ad9834_freq(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val,
	uint_fast8_t * profile,	/* место для хранения информации о последнем использованном профиле */
	uint_fast8_t comparator
	)
{
	const uint_fast8_t fselnext = (* profile) == 0;

	/* 0x01: freq0, 0x02: freq1 */ 
	const uint_fast8_t freqx8 = fselnext ? 0x80 : 0x40;



	spi_select(target, AD9834_SPIMODE);	/* start sending data to target chip */

	// assume - last initialization match to active_profile data.

	const uint_fast32_t v32 = * val;
	// LSBs (13..0 bits of FTW)
	spi_progval8_p1(target, freqx8 | ((v32 >> (8 + (FTW_RESOLUTION - 28))) & 0x3f));
	spi_progval8_p2(target, (v32 >> (0 + (FTW_RESOLUTION - 28))) & 0xff );
	// MSBs (27..24 bits of FTW)
	spi_progval8_p2(target, freqx8 | ((v32 >> (22 + (FTW_RESOLUTION - 28))) & 0x3f));
	spi_progval8_p2(target, (v32 >> (14 + (FTW_RESOLUTION - 28))) & 0xff);
	spi_complete(target);


	/* switch output for next profile */
	prog_ad9834_control(target, 0, comparator, fselnext);

	spi_unselect(target);	/* done sending data to target chip */

	* profile = fselnext;

}

static void 
//NOINLINEAT
prog_ad9834_init(
	spitarget_t target,		/* addressing to chip */
	uint_fast8_t * profile,	/* место для хранения информации о последнем использованном профиле */
	uint_fast8_t comparator	/* разрешение работы компаратора */
			 )
{
	enum { fsel = 0 };

	spi_select(target, AD9834_SPIMODE);	/* start sending data to target chip */

	prog_ad9834_control(target, 1, comparator, fsel);	// reset pulse
	prog_ad9834_control(target, 0, comparator, fsel);

	spi_unselect(target);	/* done sending data to target chip */

	* profile = fsel;

}

#endif /* AD9834_C_INCLUDED */
