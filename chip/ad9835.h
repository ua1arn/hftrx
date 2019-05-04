/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// AD9832, AD9835 support
//
// tested with AD9835

#ifndef AD9835_C_INCLUDED
#define AD9835_C_INCLUDED

#if defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_AD9835)
	#if (DDS1_CLK_MUL != 1)
		#error AD9835 as DDS1 use x1 clock multiplier only
	#endif
#endif
#if defined (DDS2_TYPE) && (DDS2_TYPE == DDS_TYPE_AD9835)
	#if (DDS2_CLK_MUL != 1)
		#error AD9835 as DDS2 use x1 clock multiplier only
	#endif
#endif
#if defined (DDS3_TYPE) && (DDS3_TYPE == DDS_TYPE_AD9835)
	#if (DDS3_CLK_MUL != 1)
		#error AD9835 as DDS3 use x1 clock multiplier only
	#endif
#endif

#define AD9835_SPIMODE SPIC_MODE2	/* MODE3 with chip is not work at all */

extern const phase_t phase_0;


#if defined (DDS1_TYPE) && (DDS1_TYPE == DDS_TYPE_AD9835)
	#if (DDS1_CLK_MUL != 1)
		#error AD9835 as DDS1 use x1 clock multiplier only
	#endif
#endif
#if defined (DDS2_TYPE) && (DDS2_TYPE == DDS_TYPE_AD9835)
	#if (DDS2_CLK_MUL != 1)
		#error AD9835 as DDS2 use x1 clock multiplier only
	#endif
#endif
#if defined (DDS3_TYPE) && (DDS3_TYPE == DDS_TYPE_AD9835)
	#if (DDS3_CLK_MUL != 1)
		#error AD9835 as DDS3 use x1 clock multiplier only
	#endif
#endif

#if FTW_RESOLUTION != 32
 #error TODO: modify AD9835.C file
#endif
/*
инициализация
0xF8, 0x00		; SLEEP = RESET = CLR = 1
0xB0, 0x00	; SYNC = SELSRC = 1
0xC0, 0x00	; SLEEP = RESET = 0

программирование нового значения частоты

если использовался регистр 1 - программируем в 0-й
0x30, ftw_07_00
0x21, ftw_15_00
0x32, ftw_23_16
0x23, ftw_31_24

0x50, 0x00 - переключаемся на регистр 0

иначе, программируем 1-й
0x34, ftw_07_00
0x25, ftw_15_00
0x36, ftw_23_16
0x27, ftw_31_24

0x58, 0x00 - переключаемся на регистр 1



*/

static void 
//NOINLINEAT
prog_ad9835_pair(
	spitarget_t target,		/* addressing to chip */
	uint_fast8_t v1,
	uint_fast8_t v2
	)
{

#if WITHSPIHW && WITHSPI16BIT

	hardware_spi_connect_b16(SPIC_SPEEDFAST, AD9835_SPIMODE);
	prog_select(target);	/* start sending data to target chip */
	hardware_spi_b16(v1 * 256 + v2);
	prog_unselect(target);	/* done sending data to target chip */
	hardware_spi_disconnect();

#else /* WITHSPIHW */

	spi_select(target, AD9835_SPIMODE);	/* start sending data to target chip */
	spi_progval8_p1(target, v1); 
	spi_progval8_p2(target, v2); 
	spi_complete(target);
	spi_unselect(target);	/* done sending data to target chip */

#endif /* WITHSPIHW */

}

/* internal data */
static void 
//NOINLINEAT
prog_ad9835_freq(
	spitarget_t target,		/* addressing to chip */
	const phase_t * ftw,		/* ftw value */
	uint_fast8_t * profile	/* место для хранения информации о последнем использованном профиле */
			 )
{
	const uint_fast32_t v = * ftw;
	const unsigned char newused = ! (* profile);

	/* если использовался регистр 1 - программируем в 0-й */
	prog_ad9835_pair(target, newused ? 0x34 : 0x30, v >> 0);
	prog_ad9835_pair(target, newused ? 0x25 : 0x21, v >> 8);
	prog_ad9835_pair(target, newused ? 0x36 : 0x32, v >> 16);
	prog_ad9835_pair(target, newused ? 0x27 : 0x23, v >> 24); 

	/* переключаемся на регистр 0 */ 
	prog_ad9835_pair(target, newused ? 0x58 : 0x50, 0x00); 


	(* profile) = newused;
}




/* programming AD9835 */
static void 
//NOINLINEAT
prog_ad9835_init(
	spitarget_t target,		/* addressing to chip */
	uint_fast8_t * profile	/* место для хранения информации о последнем использованном профиле */
	)
{
	//0xF8, 0x00		; SLEEP = RESET = CLR = 1
	prog_ad9835_pair(target, 0xF8, 0x00); 
	//0xB0, 0x00	; SYNC = SELSRC = 1
	prog_ad9835_pair(target, 0xB0, 0x00); 
	//0xC0, 0x00	; SLEEP = RESET = 0
	prog_ad9835_pair(target, 0xC0, 0x00); 
	(* profile) = 0;
}

#endif /* AD9835_C_INCLUDED */
