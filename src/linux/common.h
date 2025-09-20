#ifndef LINUX_COMMON_H
#define LINUX_COMMON_H

enum {
	rx_cic_shift_min = 32,
	rx_cic_shift_max = 64,
	rx_fir_shift_min = 32,
	rx_fir_shift_max = 56,
	tx_shift_min = 16,
	tx_shift_max = 32,
};

enum {
	rx_fir_shift_pos 	= 0,
	tx_shift_pos 		= 8,
	rx_cic_shift_pos 	= 16,
	tx_state_pos 		= 24,
	resetn_modem_pos 	= 25,
	hw_vfo_sel_pos		= 26,
	adc_rand_pos 		= 27,
	iq_test_pos			= 28,
	wnb_pos				= 29,
	stream_reset_pos 	= 30,
	fir_load_reset_pos 	= 31,
};

enum {
	CNT16TX = DMABUFFSIZE16TX / DMABUFFSTEP16TX,
	CNT32RX = DMABUFFSIZE32RX / DMABUFFSTEP32RX,
	SIZERX8 = DMABUFFSIZE32RX * 4,
};

enum {
#if CPUSTYLE_XC7Z
	stream_core = 1,
	alsa_thread_core = 1,
	iq_thread_core = 1,
	iio_thread_core = 1,
	spool_thread_core = 0,
	nmea_thread_core = 1,
#elif CPUSTYLE_RK356X || CPUSTYLE_BROADCOM
	stream_thread_core = 3,
	alsa_thread_core = 3,
	iq_thread_core = 2,
	iio_thread_core = 2,
	spool_thread_core = 0,
	nmea_thread_core = 1,
#endif
};

#endif /* #ifndef LINUX_COMMON_H */
