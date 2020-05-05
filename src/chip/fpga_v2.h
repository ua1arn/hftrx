/* $Id$ */
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
#ifndef FPGA_V2_C_INCLUDED
#define FPGA_V2_C_INCLUDED

extern const phase_t phase_0;

static uint_fast32_t fpgav2_ftw1;

#define FPGA_SPIMODE SPIC_MODE3


static void prog_fpga_update(
	spitarget_t target		/* addressing to chip */
	)
{
	rbtype_t rbbuff [11] = { 0 };	

	RBVAL8(10 * 8, fpgav2_ftw1 >> 24);	// rx
	RBVAL8(9 * 8, fpgav2_ftw1 >> 16);
	RBVAL8(8 * 8, fpgav2_ftw1 >> 8);
	RBVAL8(7 * 8, fpgav2_ftw1 >> 0);

	RBVAL8(6 * 8, fpgav2_ftw1 >> 24);	// tx
	RBVAL8(5 * 8, fpgav2_ftw1 >> 16);
	RBVAL8(4 * 8, fpgav2_ftw1 >> 8);
	RBVAL8(3 * 8, fpgav2_ftw1 >> 0);

	RBVAL8(2 * 8, 0);			// att

	RBVAL8(1 * 8, glob_tx ? 255 : 0);	// tx power

	RBVAL8(0 * 8, 0);	// format

	spi_select2(target, FPGA_SPIMODE, SPIC_SPEEDFAST);
	prog_spi_send_frame(target, rbbuff, sizeof rbbuff / sizeof rbbuff [0]);
	spi_unselect(target);
}

static uint_fast32_t ftw2freq(uint_fast32_t ftw)
{
	return ((uint_fast64_t) ftw * REFERENCE_FREQ * DDS1_CLK_MUL) >> 32;
}

/* programming FPGA SPI registers */
// http://www.sdr-deluxe.com/publ/ddc_module_1_cifrovoj_radiotrakt_transivera/1-1-0-51
static void prog_fpga_freq1(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val		/* FTW parameter for NCO */
	)
{
#if FTW_RESOLUTION >= 32
	fpgav2_ftw1 = ftw2freq((* val) >> (FTW_RESOLUTION - 32));
#else
	fpgav2_ftw1 = ftw2freq((* val) << (32 - FTW_RESOLUTION));
#endif
	prog_fpga_update(target);
}

static void prog_fpga_freq1_rts(
	spitarget_t target,		/* addressing to chip */
	const phase_t * val		/* FTW parameter for NCO */
	)
{
}
#if 0
// FPGA control register
static void
prog_fpga_ctrlreg(
	spitarget_t target		/* addressing to chip */
	)
{
}
#endif

static void prog_fpga_initialize(
	spitarget_t target		/* addressing to chip */
	)
{
	prog_fpga_update(target);
}

#endif /* FPGA_V2_C_INCLUDED */

