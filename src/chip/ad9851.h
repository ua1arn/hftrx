/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// AD9850, AD9851 support
//

#ifndef AD9851_C_INCLUDED
#define AD9851_C_INCLUDED

#if defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_AD9851)
	#if (DDS1_CLK_MUL != 1) && (DDS1_CLK_MUL != 6)
		#error AD9851 as DDS1 use x6 or x1 clock multipliers only
	#endif
#endif
#if defined (DDS2_TYPE) && (DDS2_TYPE == DDS_TYPE_AD9851)
	#if (DDS2_CLK_MUL != 1) && (DDS2_CLK_MUL != 6)
		#error AD9851 as DDS2 use x6 or x1 clock multipliers only
	#endif
#endif
#if defined (DDS3_TYPE) && (DDS3_TYPE == DDS_TYPE_AD9851)
	#if (DDS3_CLK_MUL != 1) && (DDS3_CLK_MUL != 6)
		#error AD9851 as DDS3 use x6 or x1 clock multipliers only
	#endif
#endif

extern const phase_t phase_0;


/* programming AD9850/AD9851 */

#define AD9851_SPIMODE SPIC_MODE3




/* Выдача байта МЛАДШИМ битом вперёд */
static void 
prog_ad9851_byte8_p1(
	spitarget_t target,	/* addressing to chip */
	uint_fast8_t v
	)
{
	spi_progval8_p1(target, revbits8(v));
}
/* Выдача байта МЛАДШИМ битом вперёд */
static void 
prog_ad9851_byte8_p2(
	spitarget_t target,	/* addressing to chip */
	uint_fast8_t v
	)
{
	spi_progval8_p2(target, revbits8(v));
}

/* set frequency tuning word1 to DDS */
static void 
prog_ad9851_freq(
	spitarget_t target,	/* addressing to chip */
	const phase_t * val,
	unsigned char ddsmult
	)
{
#if FTW_RESOLUTION != 32
	#error FTW_RESOLUTION  must be 32 for AD9851
#endif
	const uint_fast32_t ftw = * val;

	spi_select(target, AD9851_SPIMODE);	/* FSEL = 0 */

	prog_ad9851_byte8_p1(target, ftw >> 0);
	prog_ad9851_byte8_p2(target, ftw >> 8);
	prog_ad9851_byte8_p2(target, ftw >> 16);
	prog_ad9851_byte8_p2(target, ftw >> 24);
	prog_ad9851_byte8_p2(target, ddsmult == 6);	/* multiplier: 0 or 1 */

	spi_complete(target);

	spi_unselect(target);	/* done sending data to target chip, FSEL = 1 */
}

/* complex init */


static void 
//NOINLINEAT
prog_ad9851_init(
	spitarget_t target,		/* addressing to chip */
	unsigned ddsmult
			 )
{
	// Передёрнуть FSEL - перевод AD9850 в последовательный режим работы
	// Эти две функции должны гарантированно выдать импульс на соответствующий выход CS
	prog_select(target);	/* start sending data to target chip, FSEL = 0 */
	prog_unselect(target);	/* done sending data to target chip, FSEL = 1 */

	prog_ad9851_freq(target, & phase_0, ddsmult);
}

#endif /* AD9851_C_INCLUDED */
