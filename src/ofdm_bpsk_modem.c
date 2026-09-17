#include "hardware.h"

#if WITHINTEGRATEDDSP && 1

#include "dspdefines.h"
#include "audio.h"
#include "buffers.h"
#include "formats.h"

#define OFDM_NUM_CHANNELS   8
#define FFT_LEN             128   /* Increased from 16 to fit 3 kHz bandwidth */
#define CYCLIC_PREFIX_LEN   16    /* Proportional guard interval (1/8 of FFT) */
#define OFDM_SYMBOL_LEN     (FFT_LEN + CYCLIC_PREFIX_LEN) /* 144 samples */

/* New subcarrier map: places 8 active channels in the middle of the audio passband */
/* Bins 1 to 8 correspond to frequencies: 375, 750, 1125, 1500, 1875, 2250, 2625, 3000 Hz */
static const uint8_t subcarrier_map[OFDM_NUM_CHANNELS] = {1, 2, 3, 4, 5, 6, 7, 8};

/* ========================================================================== */
/*                             STRUCTURES & CONTEXTS                          */
/* ========================================================================== */

/* Context structure for an individual subcarrier channel tracking */
typedef struct {
    FLOAT_t phase_nco;           /* Costas loop NCO phase accumulator */
    FLOAT_t phase_step_nco;      /* Dynamic phase step adjusted by loop filter */
    FLOAT_t costas_kp;           /* Proportional loop gain */
    FLOAT_t costas_ki;           /* Integral loop gain */
    FLOAT_t costas_integrator;   /* Integral loop accumulator memory */

    FLOAT_t phase_lock_metric;   /* Exponential moving average lock indicator */
    uint32_t is_phase_locked;    /* Boolean lock status flag */
} ofdm_subcarrier_bpsk_t;

/* Independent Transmitter Context Configuration */
typedef struct {
    ARM_MORPH(arm_cfft_instance) cfft_inst;

    FLOAT_t fft_buffer[FFT_LEN * 2];
    FLOAT_t tx_time_buffer[OFDM_SYMBOL_LEN * 2];
    uint32_t tx_sample_idx;
} ofdm_modem_tx_t;

/* Independent Receiver Context Configuration */
typedef struct {
    ofdm_subcarrier_bpsk_t rx_subcarriers[OFDM_NUM_CHANNELS];
    ARM_MORPH(arm_cfft_instance) cfft_inst;

    FLOAT_t fft_buffer[FFT_LEN * 2];
    FLOAT_t rx_time_buffer[OFDM_SYMBOL_LEN * 2];
    uint32_t rx_sample_idx;

    FLOAT_t alpha_lock;          /* Low-pass smoothing factor for metrics */
} ofdm_modem_rx_t;

/* ========================================================================== */
/*                          TRANSMITTER (TX) IMPLEMENTATION                   */
/* ========================================================================== */

/**
 * @brief Runtime initialization of the standalone OFDM transmitter context.
 * @param self Pointer to the uninitialized transmitter structure footprint.
 */
static void ofdm_modem_tx_init(ofdm_modem_tx_t *self)
{
    ARM_MORPH(arm_cfft_init)(&self->cfft_inst, FFT_LEN);
    self->tx_sample_idx = 0;
    ARM_MORPH(arm_fill)(0, self->fft_buffer, FFT_LEN * 2);
    ARM_MORPH(arm_fill)(0, self->tx_time_buffer, OFDM_SYMBOL_LEN * 2);
}

/**
 * @brief Resets transient caches and trackers inside the transmitter instance.
 * @param self Pointer to the active transmitter context.
 */
static void ofdm_modem_tx_reset(ofdm_modem_tx_t *self)
{
    self->tx_sample_idx = 0;
    ARM_MORPH(arm_fill)(0, self->tx_time_buffer, OFDM_SYMBOL_LEN * 2);
}

/**
 * @brief Block-based transmitter modulation processing with peak normalization.
 * @param self Pointer to the unique transmitter configuration block.
 * @param get_bits_cb External callback supplying 8 parallel bits (one per channel).
 * @param out_buffer_i Output array destination for modulated Real components.
 * @param out_buffer_q Output array destination for modulated Imaginary components.
 * @param block_size Size of the transceiver processing hardware block frame.
 */
static void ofdm_modem_tx_block(ofdm_modem_tx_t *self, void (*get_bits_cb)(uint8_t *bits), FLOAT_t *out_buffer_i, FLOAT_t *out_buffer_q, uint32_t block_size)
{
    /* Calculate exact scaling factor for 128-point IFFT normalization */
    FLOAT_t scale_factor = 1.0 / FFT_LEN;

    for (uint32_t sample_idx = 0; sample_idx < block_size; sample_idx++)
    {
        /* Regenerate symbol payload if the active time domain vector cache is exhausted */
        if (self->tx_sample_idx >= OFDM_SYMBOL_LEN)
        {
            self->tx_sample_idx = 0;

            /* Vectorized memory initialization utilizing abstract CMSIS SIMD features */
            ARM_MORPH(arm_fill)(0, self->fft_buffer, FFT_LEN * 2);

            uint8_t tx_bits[OFDM_NUM_CHANNELS] = {0};
            get_bits_cb(tx_bits);

            /* High-speed manual loop unrolling mapping inputs directly into spectral bins */
            self->fft_buffer[subcarrier_map[0] * 2] = tx_bits[0] ? 1 : -1;
            self->fft_buffer[subcarrier_map[1] * 2] = tx_bits[1] ? 1 : -1;
            self->fft_buffer[subcarrier_map[2] * 2] = tx_bits[2] ? 1 : -1;
            self->fft_buffer[subcarrier_map[3] * 2] = tx_bits[3] ? 1 : -1;
            self->fft_buffer[subcarrier_map[4] * 2] = tx_bits[4] ? 1 : -1;
            self->fft_buffer[subcarrier_map[5] * 2] = tx_bits[5] ? 1 : -1;
            self->fft_buffer[subcarrier_map[6] * 2] = tx_bits[6] ? 1 : -1;
            self->fft_buffer[subcarrier_map[7] * 2] = tx_bits[7] ? 1 : -1;

            /* Inverse Complex FFT operation: isInverseFFT = 1, bitReverseFlag = 1 */
            ARM_MORPH(arm_cfft)(&self->cfft_inst, self->fft_buffer, 1, 1);

            /* VECTOR OPTIMIZATION: Scale the time-domain buffer to maintain the [-1.0 .. +1.0] range */
            /* Correct CMSIS-DSP argument order: pSrc, scale, pDst, blockSize */
            ARM_MORPH(arm_scale)(self->fft_buffer, scale_factor, self->fft_buffer, FFT_LEN * 2);

            /* Structural mapping of the Cyclic Prefix guard interval using arm_copy */
            uint32_t cp_src_offset = (FFT_LEN - CYCLIC_PREFIX_LEN) * 2;
            ARM_MORPH(arm_copy)(&self->fft_buffer[cp_src_offset],
                                self->tx_time_buffer,
                                CYCLIC_PREFIX_LEN * 2);

            /* Structural mapping of the core information payload directly after the prefix */
            ARM_MORPH(arm_copy)(self->fft_buffer,
                                &self->tx_time_buffer[CYCLIC_PREFIX_LEN * 2],
                                FFT_LEN * 2);
        }

        /* Serialize data elements into separate processing branches for hftrx DAC path */
        out_buffer_i[sample_idx] = self->tx_time_buffer[self->tx_sample_idx * 2];
        out_buffer_q[sample_idx] = self->tx_time_buffer[self->tx_sample_idx * 2 + 1];

        self->tx_sample_idx++;
    }
}

/* ========================================================================== */
/*                           RECEIVER (RX) IMPLEMENTATION                     */
/* ========================================================================== */

/**
 * @brief Runtime initialization of the standalone OFDM receiver context.
 * @param self Pointer to the uninitialized receiver structure footprint.
 */
static void ofdm_modem_rx_init(ofdm_modem_rx_t *self)
{
    ARM_MORPH(arm_cfft_init)(&self->cfft_inst, FFT_LEN);
    self->rx_sample_idx = 0;
    self->alpha_lock = 0.02;
    ARM_MORPH(arm_fill)(0, self->fft_buffer, FFT_LEN * 2);
    ARM_MORPH(arm_fill)(0, self->rx_time_buffer, OFDM_SYMBOL_LEN * 2);

    /* Setup independent Costas tracking parameters for each discrete channel */
    for (uint32_t ch = 0; ch < OFDM_NUM_CHANNELS; ch++)
    {
        ofdm_subcarrier_bpsk_t *sub = &self->rx_subcarriers[ch];
        sub->phase_nco = 0;
        sub->phase_step_nco = 0;
        sub->costas_kp = 0.04;
        sub->costas_ki = 0.0008;
        sub->costas_integrator = 0;
        sub->phase_lock_metric = 0;
        sub->is_phase_locked = 0;
    }
}

/**
 * @brief Forces a hard reset of operational internal receiver tracking loops.
 * @param self Pointer to the active receiver context.
 */
static void ofdm_modem_rx_reset(ofdm_modem_rx_t *self)
{
    self->rx_sample_idx = 0;
    ARM_MORPH(arm_fill)(0, self->rx_time_buffer, OFDM_SYMBOL_LEN * 2);

    for (uint32_t ch = 0; ch < OFDM_NUM_CHANNELS; ch++)
    {
        ofdm_subcarrier_bpsk_t *sub = &self->rx_subcarriers[ch];
        sub->phase_nco = 0;
        sub->phase_step_nco = 0;
        sub->costas_integrator = 0;
        sub->phase_lock_metric = 0;
        sub->is_phase_locked = 0;
    }
}

/**
 * @brief Block-based receiver demodulation processing driven by isolated context.
 * @param self Pointer to the unique receiver configuration block.
 * @param in_buffer_i Input array containing hardware ADC DDC Real data stream.
 * @param in_buffer_q Input array containing hardware ADC DDC Imaginary data stream.
 * @param block_size Size of the transceiver processing hardware block frame.
 * @param process_bits_cb Callback function executed immediately when a symbol frame decode completes.
 */
static void ofdm_modem_rx_block(ofdm_modem_rx_t *self, const FLOAT_t *in_buffer_i, const FLOAT_t *in_buffer_q, uint32_t block_size, void (*process_bits_cb)(const uint8_t *bits))
{
    for (uint32_t sample_idx = 0; sample_idx < block_size; sample_idx++)
    {
        /* Gather raw input pairs sequentially inside the time frame sliding window */
        self->rx_time_buffer[self->rx_sample_idx * 2]     = in_buffer_i[sample_idx];
        self->rx_time_buffer[self->rx_sample_idx * 2 + 1] = in_buffer_q[sample_idx];
        self->rx_sample_idx++;
        
        /* Process when a full symbol payload boundaries are successfully accumulated */
        if (self->rx_sample_idx >= OFDM_SYMBOL_LEN)
        {
            self->rx_sample_idx = 0;

            /* Slice out the cyclic prefix guard band via high speed memory transport */
            ARM_MORPH(arm_copy)(&self->rx_time_buffer[CYCLIC_PREFIX_LEN * 2],
                                self->fft_buffer,
                                FFT_LEN * 2);

            /* Forward Complex FFT conversion: isInverseFFT = 0, bitReverseFlag = 1 */
            ARM_MORPH(arm_cfft)(&self->cfft_inst, self->fft_buffer, 0, 1);
            
            uint8_t rx_bits[OFDM_NUM_CHANNELS];// = {0};
            
            /* De-rotate phase offsets and track multi-frequency channel state variations */
            for (uint32_t ch = 0; ch < OFDM_NUM_CHANNELS; ch++)
            {
                uint32_t bin_idx = subcarrier_map[ch];
                ofdm_subcarrier_bpsk_t *sub = &self->rx_subcarriers[ch];

                FLOAT_t raw_i = self->fft_buffer[bin_idx * 2];
                FLOAT_t raw_q = self->fft_buffer[bin_idx * 2 + 1];

                /* Vector phase calculations driven by hftrx dspdefines.h macro calls */
                FLOAT_t cos_p = COSF(sub->phase_nco);
                FLOAT_t sin_p = SINF(sub->phase_nco);

                /* Complex phase de-rotation multiplication */
                FLOAT_t derot_i = raw_i * cos_p + raw_q * sin_p;
                FLOAT_t derot_q = raw_q * cos_p - raw_i * sin_p;

                /* Costas BPSK Phase Error Detector metric: e = I * Q */
                FLOAT_t error_c = derot_i * derot_q;

                /* Closed-loop frequency and tracking updates */
                sub->costas_integrator += error_c * sub->costas_ki;
                sub->phase_step_nco = error_c * sub->costas_kp + sub->costas_integrator;

                /* Quality monitoring assessment metric calculations using implicit type promotion */
                FLOAT_t i2 = derot_i * derot_i;
                FLOAT_t q2 = derot_q * derot_q;
                FLOAT_t instant_metric = 0;

                if (i2 + q2 > 0) {
                    instant_metric = (i2 - q2) / (i2 + q2);
                }
                sub->phase_lock_metric += self->alpha_lock * (instant_metric - sub->phase_lock_metric);
                sub->is_phase_locked = (sub->phase_lock_metric > 0.55) ? 1 : 0;

                /* Slicer decision boundary output evaluation */
                rx_bits[ch] = (derot_i >= 0) ? 1 : 0;

                /* Update step bounded modulo 2*pi execution */
                sub->phase_nco += sub->phase_step_nco;
                if (sub->phase_nco >= 2 * M_PI) sub->phase_nco -= 2 * M_PI;
                if (sub->phase_nco < 0) sub->phase_nco += 2 * M_PI;
            }
            
            /* Direct processing of extracted frame data stream */
            process_bits_cb(rx_bits);
        }
    }
}

//////////////////
/// test


static const uint8_t testarray [] =
{
	'D', 'E', 'A', 'D', 'B', 'E', 'E', 'F',
	'A', 'B', 'B', 'A', '1', '9', '8', '0',
#if 0
	 0x48, 0x21, 0x18, 0x0C, 0x82, 0x03, 0x82, 0x00,
	 0x00, 0x34, 0x84, 0x20, 0x94, 0x20, 0x84, 0x00,
	 0x00, 0x18, 0x01, 0x10, 0x00, 0x18, 0x00, 0x0C,
	 0x88, 0x02, 0x80, 0x58, 0x42, 0x41, 0x82, 0x4C,
	 0x82, 0x0B, 0x48, 0x20, 0x28, 0x41, 0x44, 0x61,
	 0x44, 0x21, 0x19, 0x4A, 0x01, 0xA0, 0x04, 0x10,
	 0x11, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x84, 0x00,
	 0x00, 0x05, 0x28, 0x24, 0xC8, 0x20, 0x38, 0xA0,
	 0x68, 0x80, 0x06, 0x14, 0x42, 0x11, 0x94, 0x20,
	 0x94, 0x20, 0x04, 0x10, 0x15, 0x01, 0x10, 0x00,
	 0x00, 0x88, 0x08, 0x80, 0x28, 0x00, 0x04, 0x23,
	 0x48, 0xC8, 0x20, 0xB4, 0x86, 0x02, 0x80, 0x06,
	 0x14, 0x56, 0x80, 0x94, 0xA0, 0x94, 0x20, 0x48,
	 0x41, 0x01, 0x10, 0x11, 0x00, 0x00, 0x00, 0x00,
	 0x00, 0x00, 0x0E, 0xD3, 0x60, 0x00, 0x02, 0x80,
	 0x12, 0x06, 0x28, 0x00, 0x00, 0x00, 0x09, 0x42,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48,
	 0x03, 0x84, 0x08, 0x80, 0x00, 0x00, 0x94, 0x20,
	 0x62, 0x80, 0x00, 0x01, 0x11, 0x80, 0x94, 0x20,
	 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x00, 0x14,
	 0x03, 0x84, 0x08, 0x80, 0x00, 0x28, 0x09, 0x42,
	 0x24, 0x28, 0x04, 0x80, 0x84, 0x11, 0x04, 0x19,
	 0x42, 0x44, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00,
	 0x00, 0x04, 0x86, 0x48, 0x00, 0x00, 0x00, 0x09,
	 0x46, 0x24, 0x28, 0x00, 0x00, 0x11, 0x04, 0x19,
	 0x4A, 0x00, 0x11, 0x00, 0x01, 0x10, 0x00, 0x01,
	 0x40, 0x00, 0x44, 0x00, 0x04, 0x40, 0x00, 0x44,
	 0x84, 0x34, 0x4E, 0xDA, 0x40, 0x00, 0x01, 0x20,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x80, 0x18,
	 0x00, 0x24, 0x88, 0x0D, 0x82, 0x80, 0x04, 0x10,
	 0x06, 0x28, 0x00, 0x01, 0x20, 0x18, 0x41, 0x94,
	 0x24, 0x10, 0x11, 0x00, 0x05, 0x81, 0x00, 0x00,
	 0x08, 0x80, 0x00, 0x00, 0x88, 0x24, 0x42, 0x02,
	 0x80, 0x02, 0x44, 0x24, 0x14, 0x20, 0x84, 0x08,
	 0x44, 0x11, 0x20, 0x01, 0x14, 0x40, 0x00, 0x08,
	 0x80, 0x28, 0x08, 0x80, 0x02, 0x44, 0x20, 0x9C,
	 0x22, 0x4C, 0x82, 0x02, 0x80, 0x14, 0x68, 0x44,
	 0x24, 0x10, 0x09, 0x42, 0x04, 0x44, 0x10, 0x48,
	 0x01, 0x10, 0x11, 0x00, 0x00, 0x02, 0x80, 0x88,
	 0x00, 0x00, 0x14, 0x12, 0x24, 0x42, 0x00, 0x51,
	 0x40, 0x34, 0x10, 0x65, 0x40, 0x18, 0x84, 0x54,
	 0x88, 0x44, 0x10, 0x51, 0x40, 0x38, 0x1F, 0x1F,
	 0x62, 0x00, 0x00, 0x00, 0x00, 0x28, 0x91, 0x88,
	 0x80, 0x61, 0x84, 0x80, 0xC9, 0x81, 0x21, 0x84,
	 0x28, 0x81, 0x22, 0x80, 0x42, 0x00, 0x04, 0x20,
	 0x12, 0x41, 0x48, 0x41, 0x04, 0x10, 0x11, 0x00,
	 0x00, 0x00, 0x98, 0x41, 0x80, 0x06, 0x88, 0x00,
	 0x24, 0x42, 0x24, 0x42, 0x88, 0x94, 0x22, 0x80,
	 0x41, 0x04, 0x12, 0x45, 0x21, 0x41, 0x9C, 0x20,
	 0x28, 0x00, 0x41, 0x01, 0x10, 0x08, 0x40, 0x14,
#endif
};

static int testindex;

static void test_ofdm_get_preamble_bits(uint8_t *bits)
{
	memset(bits, 0xFF, 8);
}

static void test_ofdm_get_bits(uint8_t *bits)
{
	const uint8_t data = testarray [testindex];

	bits [0] = !! (data & (UINT8_C(1) << 7));
	bits [1] = !! (data & (UINT8_C(1) << 6));
	bits [2] = !! (data & (UINT8_C(1) << 5));
	bits [3] = !! (data & (UINT8_C(1) << 4));
	bits [4] = !! (data & (UINT8_C(1) << 3));
	bits [5] = !! (data & (UINT8_C(1) << 2));
	bits [6] = !! (data & (UINT8_C(1) << 1));
	bits [7] = !! (data & (UINT8_C(1) << 0));

	testindex = (testindex + 1) % (sizeof testarray / sizeof testarray [0]);
}

static void test_ofdm_process_null_bits(const uint8_t *bits)
{
}
static void test_ofdm_process_bits(const uint8_t *bits)
{
	unsigned v = 0;

	v |= (UINT8_C(1) << 7) * !! bits [0];
	v |= (UINT8_C(1) << 6) * !! bits [1];
	v |= (UINT8_C(1) << 5) * !! bits [2];
	v |= (UINT8_C(1) << 4) * !! bits [3];
	v |= (UINT8_C(1) << 3) * !! bits [4];
	v |= (UINT8_C(1) << 2) * !! bits [5];
	v |= (UINT8_C(1) << 1) * !! bits [6];
	v |= (UINT8_C(1) << 0) * !! bits [7];
	//PRINTF("0x%02X, ", v);
	PRINTF("%c", v);
}

static ofdm_modem_tx_t tx;
static ofdm_modem_rx_t rx;

void modem_fill(IFADCvalue_t * buff, adapter_t * ap)
{
	FLOAT_t i, q;
	const FLOAT_t denom = 1;//1. / 16;
	ofdm_modem_tx_block(& tx, test_ofdm_get_bits, & i, & q, 1);

	buff [DMABUF32RX0I] = adpt_output(ap, i * denom);
	buff [DMABUF32RX0I] = adpt_output(ap, q * denom);
}


void modem_test(void)
{
	TP();

	ofdm_modem_tx_init(& tx);
	ofdm_modem_rx_init(& rx);

	enum { BUFFLEN = 256 };
	FLOAT_t buffer_i [BUFFLEN];
	FLOAT_t buffer_q [BUFFLEN];

	ofdm_modem_tx_block(& tx, test_ofdm_get_preamble_bits, buffer_i, buffer_q, BUFFLEN);
	ofdm_modem_rx_block(& rx, buffer_i, buffer_q, BUFFLEN, test_ofdm_process_null_bits);
	unsigned i;
	for (i = 0; i < 1000; ++ i)
	{

		ofdm_modem_tx_block(& tx, test_ofdm_get_bits, buffer_i, buffer_q, BUFFLEN);
		ofdm_modem_rx_block(& rx, buffer_i, buffer_q, BUFFLEN, test_ofdm_process_bits);
	}
	PRINTF("\n");
	TP();
}

#endif /* WITHINTEGRATEDDSP */
