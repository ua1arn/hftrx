/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#ifndef FPGA_V1_C_INCLUDED
#define FPGA_V1_C_INCLUDED

extern const phase_t phase_0;

#define FPGA_SPIMODE SPIC_MODE3

#define FPGA_DECODE_CTLREG	(1u << 0)
#define FPGA_DECODE_NCO1	(1u << 1)
#define FPGA_DECODE_NCO2	(1u << 2)
#define FPGA_DECODE_NCORTS	(1u << 3)
#define FPGA_DECODE_FQMETER	(0)
#define FPGA_DECODE_NCO3	(1u << 4)
#define FPGA_DECODE_NCO4	(1u << 5)
#define FPGA_DECODE_NBLVL	(1u << 6)


/* programming FPGA SPI registers */

static void prog_fpga_valX(
	spitarget_t target,		/* addressing to chip */
	uint_fast32_t v32,		/* 32bit control value */
	uint_fast8_t addr
	)
{
	rbtype_t rbbuff [5] = { 0 };

	RBVAL8(040, addr);
	RBVAL8(030, v32 >> 24);
	RBVAL8(020, v32 >> 16);
	RBVAL8(010, v32 >> 8);
	RBVAL8(000, v32 >> 0);

	spi_select2(target, CTLREG_SPIMODE, SPIC_SPEEDUFAST);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

static void prog_fpga_freqX(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val,		/* FTW parameter for NCO */
	uint_fast8_t addr
	)
{
#if FTW_RESOLUTION >= 32
	const uint_fast32_t v32 = (* val) >> (FTW_RESOLUTION - 32);
#else
	const uint_fast32_t v32 = (* val) << (32 - FTW_RESOLUTION);
#endif
	prog_fpga_valX(target, v32, addr);
}

static void prog_fpga_freq1(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val		/* FTW parameter for NCO */
	)
{
	prog_fpga_freqX(target, val, FPGA_DECODE_NCO1);
}

static void prog_fpga_freq2(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val		/* FTW parameter for NCO */
	)
{
	prog_fpga_freqX(target, val, FPGA_DECODE_NCO2);
}

static void prog_fpga_freq3(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val		/* FTW parameter for NCO */
	)
{
	prog_fpga_freqX(target, val, FPGA_DECODE_NCO3);
}

static void prog_fpga_freq4(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val		/* FTW parameter for NCO */
	)
{
	prog_fpga_freqX(target, val, FPGA_DECODE_NCO4);
}

static void prog_fpga_freq1_rts(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val		/* FTW parameter for NCO */
	)
{
	prog_fpga_freqX(target, val, FPGA_DECODE_NCORTS);
}

static void prog_fpga_nblevel(
	spitarget_t target,		/* addressing to chip */
	uint_fast32_t val		/* FTW parameter for NCO */
	)
{
	prog_fpga_valX(target, val, FPGA_DECODE_NBLVL);
}

// FPGA control register
static void
prog_fpga_ctrlreg(
	spitarget_t target		/* addressing to chip */
	)
{
	rbtype_t rbbuff [5] = { 0 };	

	RBVAL8(040, FPGA_DECODE_CTLREG);

	RBVAL(16, glob_adcoffset, 9);			/* b24..b16: adcoffset - смещение для выходного сигнала с АЦП */
	RBBIT(15, glob_mode_wfm);				/* b15: mode_wfm - разрешение передачи в DSP квадратур 192 кГц */
	RBBIT(14, glob_tx_bpsk_enable);			/* b14: tx_bpsk_enable - разрешение прямого формирования модуляции в FPGA */
	RBBIT(13, glob_tx_inh_enable);			/* b13: tx_inh_enable - разрешение реакции на вход tx_inh */
	RBBIT(12, glob_dactest);				/* b12: dactest */
	RBBIT(11, ! glob_sleep && glob_xvrtr);	/* b11: xvrtr_enable */
	RBBIT(10, glob_dacstraight);			/* b10: dacstraight - Требуется формирование кода для ЦАП в режиме беззнакового кода*/
	RBBIT(9, glob_i2s_enable);				/* b9: i2s_enable */
	RBBIT(8, glob_firprofile [1]);			/* b8: fir2_profile - что используется для фильтрации FIR2 */
	RBBIT(7, glob_firprofile [0]);			/* b7: fir_profile - что используется для фильтрации FIR1 */
	RBBIT(6, glob_flt_reset_n);				/* b6: flt_reset_n net */
	RBBIT(5, glob_dither);					/* b5: adc_dith net */
	RBBIT(4, glob_adcrand);					/* b4: adc_rand net  */
	RBBIT(3, ! glob_sleep && glob_preamp);	/* b3: adc_pga net */
#if ! WITHWAVPLAYER
	// Для экономии потребления - не включаеи АЦП и ЦАП
	RBBIT(2, ! glob_sleep && glob_tx);		/* b2: mode_tx net  */
	RBBIT(1, ! glob_sleep && ! glob_tx);	/* b1: mode_rx net  */
#endif /* ! WITHWAVPLAYER */
	RBBIT(0, glob_reset_n);					/* b0: ! reset_n net - FIR filter требует перехода из "1" в "0" на reset_n для нормальной работы */

	spi_select2(target, CTLREG_SPIMODE, SPIC_SPEEDUFAST);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}


#if WITHFQMETER

static uint_fast32_t prog_fpga_getfqmeter(
	spitarget_t target		/* addressing to chip */
	)
{
	uint_fast8_t v0, v1, v2, v3;
	uint_fast8_t pps;

	spi_select2(target, CTLREG_SPIMODE, SPIC_SPEEDUFAST);	// В FPGA регистр тактируется не в прямую

	v0 = spi_read_byte(target, 0x00);
	v1 = spi_read_byte(target, 0x00);
	v2 = spi_read_byte(target, 0x00);
	v3 = spi_read_byte(target, 0x00);
	pps = spi_read_byte(target, FPGA_DECODE_FQMETER);		// состояние входа PPS

	spi_unselect(target);

	(void) pps;
	return 
		(uint_fast32_t) v0 << 24 | 
		(uint_fast32_t) v1 << 16 | 
		(uint_fast32_t) v2 << 8 | 
		(uint_fast32_t) v3 << 0 | 
		0;
}

#endif /* WITHFQMETER */


static void prog_fpga_initialize(
	spitarget_t target		/* addressing to chip */
	)
{
	prog_fpga_freq1(target, & phase_0);
	prog_fpga_freq2(target, & phase_0);
	prog_fpga_freq1_rts(target, & phase_0);
	prog_fpga_nblevel(target, INT32_MAX);	/* при таком значении NB не срабатывает */
}

#endif /* FPGA_V1_C_INCLUDED */

