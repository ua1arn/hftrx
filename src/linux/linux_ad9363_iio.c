/*
 * By RA4ASN
 */

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "buffers.h"
#include "audio.h"
#include <arm_math.h>

#if LINUX_SUBSYSTEM && WITHAD936XIIO

// source: https://github.com/analogdevicesinc/libiio/blob/main/examples/ad9361-iiostream.c

#include <iio/iio.h>
#include <iio/iio-debug.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include "fir_coeffs.h"

/* helper macros */
#define MHZ(x) ((long long)(x*1000000.0 + .5))
#define GHZ(x) ((long long)(x*1000000000.0 + .5))

#define IIO_ENSURE(expr) { \
	if (!(expr)) { \
		(void) fprintf(stderr, "assertion failed (%s:%d)\n", __FILE__, __LINE__); \
		(void) abort(); \
	} \
}

#define DECIM_COEFF	24
#define BLOCK_SIZE	(DMABUFFSIZE32RX * DECIM_COEFF / 2)

/* RX is input, TX is output */
enum iodev { RX, TX };

/* common RX and TX streaming params */
struct stream_cfg {
	long long bw_hz; // Analog banwidth in Hz
	long long fs_hz; // Baseband sample rate in Hz
	long long lo_hz; // Local oscillator frequency in Hz
	const char* rfport; // Port name
};

/* static scratch mem for strings */
static char tmpstr[64];

/* IIO structs required for streaming */
static struct iio_context *ctx   = NULL;
static struct iio_channel *rx0_i = NULL;
static struct iio_channel *rx0_q = NULL;
static struct iio_channel *tx0_i = NULL;
static struct iio_channel *tx0_q = NULL;
static struct iio_buffer  *rxbuf = NULL;
static struct iio_buffer  *txbuf = NULL;
static struct iio_stream  *rxstream = NULL;
static struct iio_stream  *txstream = NULL;
static struct iio_channels_mask *rxmask = NULL;
static struct iio_channels_mask *txmask = NULL;

static bool stop = false;
extern uint32_t * ph_fifo;

static int32_t buf96k_i[DMABUFFSIZE32RX / 2], buf96k_q[DMABUFFSIZE32RX / 2];
static int32_t buf2304k_i[BLOCK_SIZE], buf2304k_q[BLOCK_SIZE];

static uint32_t cnt96 = 0, cnt2304 = 0;

arm_fir_decimate_instance_q31 firdec_x24_i, firdec_x24_q;
q31_t fir_state_i[NUM_FIR_TAPS + BLOCK_SIZE - 1], fir_state_q[NUM_FIR_TAPS + BLOCK_SIZE - 1];

void iq_mutex_unlock(void);

void iio_stop_stream(void)
{
	stop = true;
}

void iq_proc(int32_t * buf_i, int32_t * buf_q, uint16_t * count)
{
	uintptr_t addr32rx = allocate_dmabuffer32rx();
	uint32_t * b = (uint32_t *) addr32rx;

	for (int i = 0; i < DMABUFFSIZE32RX; i += 8)
	{
		b[i + 0] = buf_i[* count];
		b[i + 1] = buf_q[* count];

		b[i + 4] = buf_i[* count];
		b[i + 5] = buf_q[* count];
		(* count) ++;

		b[i + 6] = buf_i[* count];
		b[i + 7] = buf_q[* count];
		(* count) ++;
	}

	save_dmabuffer32rx(addr32rx);

	uintptr_t addr_ph = getfilled_dmabuffer16tx();
	b = (uint32_t *) addr_ph;

	for (int i = 0; i < DMABUFFSIZE16TX; i ++)
		* ph_fifo = b[i];

	release_dmabuffer16tx(addr_ph);
}

void stream(size_t rx_sample, size_t tx_sample, size_t block_size,
	    struct iio_stream *rxstream, struct iio_stream *txstream,
	    const struct iio_channel *rxchn, const struct iio_channel *txchn)
{
	const struct iio_device *dev;
	const struct iio_context *ctx;
	const struct iio_block *txblock, *rxblock;
	ssize_t nrx = 0;
	ssize_t ntx = 0;
	int err;

	dev = iio_channel_get_device(rxchn);
	ctx = iio_device_get_context(dev);

	arm_fir_decimate_init_q31(& firdec_x24_i, NUM_FIR_TAPS, DECIM_COEFF, fir_coeffs, fir_state_i, BLOCK_SIZE);
	arm_fir_decimate_init_q31(& firdec_x24_q, NUM_FIR_TAPS, DECIM_COEFF, fir_coeffs, fir_state_q, BLOCK_SIZE);

	while (!stop) {
		int16_t *p_dat, *p_end;
		ptrdiff_t p_inc;

		rxblock = iio_stream_get_next_block(rxstream);
		err = iio_err(rxblock);
		if (err) {
			printf("Unable to receive block");
			return;
		}

		txblock = iio_stream_get_next_block(txstream);
		err = iio_err(txblock);
		if (err) {
			printf("Unable to send block");
			return;
		}

		/* READ: Get pointers to RX buf and read IQ from RX buf port 0 */
		p_inc = rx_sample;
		p_end = (int16_t *) iio_block_end(rxblock);
		for (p_dat = (int16_t *) iio_block_first(rxblock, rxchn); p_dat < p_end;
		     p_dat += p_inc / sizeof(*p_dat))
		{
			/* Example: swap I and Q */
			int16_t i = p_dat[0];
			int16_t q = p_dat[1];

			buf2304k_i[cnt2304] = i << 16;
			buf2304k_q[cnt2304] = q << 16;

			cnt2304 ++;
			if (cnt2304 >= BLOCK_SIZE)
			{
				cnt2304 = 0;
				arm_fir_decimate_q31(& firdec_x24_i, buf2304k_i, buf96k_i, BLOCK_SIZE);
				arm_fir_decimate_q31(& firdec_x24_q, buf2304k_q, buf96k_q, BLOCK_SIZE);

				uint16_t j = 0;

				iq_proc(buf96k_i, buf96k_q, & j);
				iq_proc(buf96k_i, buf96k_q, & j);

				iq_mutex_unlock();
			}
		}

		/* WRITE: Get pointers to TX buf and write IQ to TX buf port 0 */
		p_inc = tx_sample;
		p_end = (int16_t *) iio_block_end(txblock);
		for (p_dat = (int16_t *) iio_block_first(txblock, txchn); p_dat < p_end;
		     p_dat += p_inc / sizeof(*p_dat)) {
			* (int16_t *) (p_dat + 0) = 0; /* Real (I) */
			* (int16_t *) (p_dat + 1) = 0; /* Imag (Q) */
		}
	}
}

/* cleanup and exit */
void ad936x_shutdown(void)
{
	printf("* Destroying streams\n");
	if (rxstream) {iio_stream_destroy(rxstream); }
	if (txstream) { iio_stream_destroy(txstream); }

	printf("* Destroying buffers\n");
	if (rxbuf) { iio_buffer_destroy(rxbuf); }
	if (txbuf) { iio_buffer_destroy(txbuf); }

	printf("* Destroying channel masks\n");
	if (rxmask) { iio_channels_mask_destroy(rxmask); }
	if (txmask) { iio_channels_mask_destroy(txmask); }

	printf("* Destroying context\n");
	if (ctx) { iio_context_destroy(ctx); }
	// exit(0);
}

/* check return value of attr_write function */
static void errchk(int v, const char* what) {
	 if (v < 0) { fprintf(stderr, "Error %d writing to channel \"%s\"\nvalue may not be supported.\n", v, what); ad936x_shutdown(); }
}

/* write attribute: long long int */
static void wr_ch_lli(struct iio_channel *chn, const char* what, long long val)
{
	const struct iio_attr *attr = iio_channel_find_attr(chn, what);

	errchk(attr ? iio_attr_write_longlong(attr, val) : -ENOENT, what);
}

/* write attribute: string */
static void wr_ch_str(struct iio_channel *chn, const char* what, const char* str)
{
	const struct iio_attr *attr = iio_channel_find_attr(chn, what);

	errchk(attr ? iio_attr_write_string(attr, str) : -ENOENT, what);
}

/* helper function generating channel names */
static char* get_ch_name(const char* type, int id)
{
	snprintf(tmpstr, sizeof(tmpstr), "%s%d", type, id);
	return tmpstr;
}

/* returns ad9361 phy device */
static struct iio_device* get_ad9361_phy(void)
{
	struct iio_device *dev =  iio_context_find_device(ctx, "ad9361-phy");
	IIO_ENSURE(dev && "No ad9361-phy found");
	return dev;
}

/* finds AD9361 streaming IIO devices */
static bool get_ad9361_stream_dev(enum iodev d, struct iio_device **dev)
{
	switch (d) {
	case TX: *dev = iio_context_find_device(ctx, "cf-ad9361-dds-core-lpc"); return *dev != NULL;
	case RX: *dev = iio_context_find_device(ctx, "cf-ad9361-lpc");  return *dev != NULL;
	default: IIO_ENSURE(0); return false;
	}
}

/* finds AD9361 streaming IIO channels */
static bool get_ad9361_stream_ch(enum iodev d, struct iio_device *dev, int chid, struct iio_channel **chn)
{
	*chn = iio_device_find_channel(dev, get_ch_name("voltage", chid), d == TX);
	if (!*chn)
		*chn = iio_device_find_channel(dev, get_ch_name("altvoltage", chid), d == TX);
	return *chn != NULL;
}

/* finds AD9361 phy IIO configuration channel with id chid */
static bool get_phy_chan(enum iodev d, int chid, struct iio_channel **chn)
{
	switch (d) {
	case RX: *chn = iio_device_find_channel(get_ad9361_phy(), get_ch_name("voltage", chid), false); return *chn != NULL;
	case TX: *chn = iio_device_find_channel(get_ad9361_phy(), get_ch_name("voltage", chid), true);  return *chn != NULL;
	default: IIO_ENSURE(0); return false;
	}
}

/* finds AD9361 local oscillator IIO configuration channels */
static bool get_lo_chan(enum iodev d, struct iio_channel **chn)
{
	switch (d) {
	 // LO chan is always output, i.e. true
	case RX: *chn = iio_device_find_channel(get_ad9361_phy(), get_ch_name("altvoltage", 0), true); return *chn != NULL;
	case TX: *chn = iio_device_find_channel(get_ad9361_phy(), get_ch_name("altvoltage", 1), true); return *chn != NULL;
	default: IIO_ENSURE(0); return false;
	}
}

/* applies streaming configuration through IIO */
bool cfg_ad9361_streaming_ch(struct stream_cfg *cfg, enum iodev type, int chid)
{
	const struct iio_attr *attr;
	struct iio_channel *chn = NULL;

	// Configure phy and lo channels
	printf("* Acquiring AD9361 phy channel %d\n", chid);
	if (!get_phy_chan(type, chid, &chn)) {	return false; }

	attr = iio_channel_find_attr(chn, "rf_port_select");
	if (attr)
		errchk(iio_attr_write_string(attr, cfg->rfport), cfg->rfport);
	wr_ch_lli(chn, "rf_bandwidth",       cfg->bw_hz);
	wr_ch_lli(chn, "sampling_frequency", cfg->fs_hz);

	// Configure LO channel
	printf("* Acquiring AD9361 %s lo channel\n", type == TX ? "TX" : "RX");
	if (!get_lo_chan(type, &chn)) { return false; }
	wr_ch_lli(chn, "frequency", cfg->lo_hz);
	return true;
}

uint8_t gui_ad936x_find(const char * uri)
{
	if (rxstream) return 2;

	if (ctx) { iio_context_destroy(ctx); }
	ctx = iio_create_context(NULL, uri);

	if (iio_err(ctx)) return 1;

	if (get_ad9361_phy()) return 0;

	return 1;
}

int ad9363_iio_start (const char * uri)
{
	// Streaming devices
	struct iio_device *tx;
	struct iio_device *rx;

	// RX and TX sample size
	size_t rx_sample_sz, tx_sample_sz;

	// Stream configurations
	struct stream_cfg rxcfg;
	struct stream_cfg txcfg;

	int err;

	// RX stream config
	rxcfg.bw_hz = MHZ(2.0);   	// 2 MHz rf bandwidth
	rxcfg.fs_hz = MHZ(2.304);   // for x24 decimation
	rxcfg.lo_hz = MHZ(433.0); 	// todo: add freq change
	rxcfg.rfport = "A_BALANCED"; // port A (select for rf freq.)

	// TX stream config
	txcfg.bw_hz = MHZ(2.0); 	// 2.0 MHz rf bandwidth
	txcfg.fs_hz = MHZ(2.304);   // for x24 interpolation
	txcfg.lo_hz = MHZ(433.0); 	// todo: add freq change
	txcfg.rfport = "A"; // port A (select for rf freq.)

	if (ctx) { iio_context_destroy(ctx); }

	printf("* Acquiring IIO context\n");
	IIO_ENSURE((ctx = iio_create_context(NULL, uri)) && "No context");
	IIO_ENSURE(iio_context_get_devices_count(ctx) > 0 && "No devices");

	printf("* Acquiring AD9361 streaming devices\n");
	IIO_ENSURE(get_ad9361_stream_dev(TX, &tx) && "No tx dev found");
	IIO_ENSURE(get_ad9361_stream_dev(RX, &rx) && "No rx dev found");

	printf("* Configuring AD9361 for streaming\n");
	IIO_ENSURE(cfg_ad9361_streaming_ch(&rxcfg, RX, 0) && "RX port 0 not found");
	IIO_ENSURE(cfg_ad9361_streaming_ch(&txcfg, TX, 0) && "TX port 0 not found");

	printf("* Initializing AD9361 IIO streaming channels\n");
	IIO_ENSURE(get_ad9361_stream_ch(RX, rx, 0, &rx0_i) && "RX chan i not found");
	IIO_ENSURE(get_ad9361_stream_ch(RX, rx, 1, &rx0_q) && "RX chan q not found");
	IIO_ENSURE(get_ad9361_stream_ch(TX, tx, 0, &tx0_i) && "TX chan i not found");
	IIO_ENSURE(get_ad9361_stream_ch(TX, tx, 1, &tx0_q) && "TX chan q not found");

	rxmask = iio_create_channels_mask(iio_device_get_channels_count(rx));
	if (!rxmask) {
		fprintf(stderr, "Unable to alloc channels mask\n");
		ad936x_shutdown();
	}

	txmask = iio_create_channels_mask(iio_device_get_channels_count(tx));
	if (!txmask) {
		fprintf(stderr, "Unable to alloc channels mask\n");
		ad936x_shutdown();
	}

	printf("* Enabling IIO streaming channels\n");
	iio_channel_enable(rx0_i, rxmask);
	iio_channel_enable(rx0_q, rxmask);
	iio_channel_enable(tx0_i, txmask);
	iio_channel_enable(tx0_q, txmask);

	printf("* Creating non-cyclic IIO buffers with 2.304 MiS\n");
	rxbuf = iio_device_create_buffer(rx, 0, rxmask);
	err = iio_err(rxbuf);
	if (err) {
		rxbuf = NULL;
		printf("Could not create RX buffer: %d\n", err);
		ad936x_shutdown();
	}
	txbuf = iio_device_create_buffer(tx, 0, txmask);
	err = iio_err(txbuf);
	if (err) {
		txbuf = NULL;
		printf("Could not create TX buffer: %d\n", err);
		ad936x_shutdown();
	}

	rxstream = iio_buffer_create_stream(rxbuf, 4, BLOCK_SIZE);
	err = iio_err(rxstream);
	if (err) {
		rxstream = NULL;
		printf("Could not create RX stream: %d\n", iio_err(rxstream));
		ad936x_shutdown();
	}

	txstream = iio_buffer_create_stream(txbuf, 4, BLOCK_SIZE);
	err = iio_err(txstream);
	if (err) {
		txstream = NULL;
		printf("Could not create TX stream: %d\n", iio_err(txstream));
		ad936x_shutdown();
	}

	rx_sample_sz = iio_device_get_sample_size(rx, rxmask);
	tx_sample_sz = iio_device_get_sample_size(tx, txmask);

	printf("* Starting IO streaming\n");
	stream(rx_sample_sz, tx_sample_sz, BLOCK_SIZE,
	       rxstream, txstream, rx0_i, tx0_i);

	ad936x_shutdown();

	return 0;
}

#endif /* LINUX_SUBSYSTEM && WITHAD936XIIO */
