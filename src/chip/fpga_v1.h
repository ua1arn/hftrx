/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#ifndef FPGA_V1_C_INCLUDED
#define FPGA_V1_C_INCLUDED

extern const phase_t phase_0;

#define FPGAREG_V1_SPISPEED		SPIC_SPEEDUFAST
#define FPGAREG_V1_SPIMODE		SPIC_MODE3

#define FPGA_DECODE_CTLREG	(1u << 0)
#define FPGA_DECODE_NCO1	(1u << 1)
#define FPGA_DECODE_FQMETER	(0)
#define FPGA_DECODE_NBLVL	(1u << 6)


// Send a frame of bytes via SPI
static void
board_fpga1_spi_send_frame(
	spitarget_t target,
	const uint8_t * buff,
	unsigned int size
	)
{
	prog_spi_io(target, FPGAREG_V1_SPISPEED, FPGAREG_V1_SPIMODE, buff, size, NULL, 0, NULL, 0);
}

// Read a frame of bytes via SPI
static void
board_fpga1_spi_exchange_frame(
	spitarget_t target,
	const uint8_t * tbuff,
	uint8_t * rbuff,
	unsigned int size
	)
{
	prog_spi_exchange(target, SPIC_SPEEDFAST, CTLREG_SPIMODE, tbuff, rbuff, size);
}

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

	board_fpga1_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
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
#if (CTLREGMODE_OLEG4Z_V1 || CTLREGMODE_OLEG4Z_V2)
    prog_fpga_freqX(target, val, FPGA_DECODE_NCO1);
#else /* (CTLREGMODE_OLEG4Z_V1 || CTLREGMODE_OLEG4Z_V2) */
    mirror_nco1 = * val;
#endif /* (CTLREGMODE_OLEG4Z_V1 || CTLREGMODE_OLEG4Z_V2) */
}

static void prog_fpga_freq2(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val		/* FTW parameter for NCO */
	)
{
	mirror_nco2 = * val;
}

static void prog_fpga_freq3(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val		/* FTW parameter for NCO */
	)
{
	mirror_nco3 = * val;
}

static void prog_fpga_freq4(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val		/* FTW parameter for NCO */
	)
{
	mirror_nco4 = * val;
}

static void prog_fpga_freq1_rts(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val		/* FTW parameter for NCO */
	)
{
	mirror_ncorts = * val;
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

	RBBIT(28, glob_nb_en [1]);				/* b28: nb2_en - Noise Blanker RX2 enable */
	RBBIT(27, glob_nb_en [0]);				/* b27: nb1_en - Noise Blanker RX1 enable */
	RBBIT(26, ! glob_sleep && glob_tx);		/* b26: txdac_en - включение ЦАП */
	RBBIT(25, glob_tx_loopback);			/* b25: tx_loopback - включение спектроанализатора сигнала передачи */
//	RBVAL(16, glob_adcoffset, 9);			/* b24..b16: adcoffset - смещение для выходного сигнала с АЦП */
	RBBIT(15, glob_mode_wfm);				/* b15: mode_wfm - разрешение передачи в DSP квадратур 192 кГц */
	RBBIT(14, glob_tx_bpsk_enable);			/* b14: tx_bpsk_enable - разрешение прямого формирования модуляции в FPGA */
	RBBIT(13, glob_tx_inh_enable);			/* b13: tx_inh_enable - разрешение реакции на вход tx_inh */
	RBBIT(12, glob_dactest);				/* b12: dactest */
	RBBIT(11, ! glob_sleep && glob_xvrtr);	/* b11: xvrtr_enable */
	RBBIT(10, glob_dacstraight);			/* b10: dacstraight - Требуется формирование кода для ЦАП в режиме беззнакового кода*/
	RBBIT(9, glob_i2s_enable);				/* b9: i2s_enable */
	RBBIT(8, glob_firprofile [1]);			/* b8: fir2_profile - что используется для фильтрации FIR2 */
	RBBIT(7, glob_firprofile [0]);			/* b7: fir_profile - что используется для фильтрации FIR1 */
	RBBIT(6, glob_flt_reset_n && glob_reset_n);				/* b6: flt_reset_n net */
	RBBIT(5, glob_dither);					/* b5: adc_dith net */
	RBBIT(4, glob_adcrand);					/* b4: adc_rand net  */
	RBBIT(3, ! glob_sleep && glob_preamp);	/* b3: adc_pga net */
#if ! WITHWAVPLAYER
	// Для экономии потребления - не включаеи АЦП и ЦАП
	RBBIT(2, ! glob_sleep && glob_tx);		/* b2: mode_tx net  */
	RBBIT(1, ! glob_sleep && ! glob_tx);	/* b1: mode_rx net  */
#endif /* ! WITHWAVPLAYER */
	RBBIT(0, glob_reset_n);					/* b0: ! reset_n net - FIR filter требует перехода из "1" в "0" на reset_n для нормальной работы */

	board_fpga1_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
}


#if WITHFQMETER

static uint_fast32_t prog_fpga_getfqmeter(
	spitarget_t target		/* addressing to chip */
	)
{
	static const uint8_t t [5] = { 0, 0, 0, 0, FPGA_DECODE_FQMETER, };
	uint8_t r [ARRAY_SIZE(t)];
	uint_fast8_t pps;

	board_fpga1_spi_exchange_frame(target, t, r, ARRAY_SIZE(r));

	pps = r [4];	/* PPS input state */

	(void) pps;
	return USBD_peek_u32_BE(r);
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

#if WITHIQSHIFT

	iq_shift_cic_rx(CALIBRATION_IQ_CIC_RX_SHIFT);
	iq_shift_fir_rx(CALIBRATION_IQ_FIR_RX_SHIFT);
	iq_shift_tx(CALIBRATION_TX_SHIFT);

#endif /* WITHIQSHIFT */
}

#endif /* FPGA_V1_C_INCLUDED */

