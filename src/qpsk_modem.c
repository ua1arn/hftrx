#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if WITHIF4DSP && 0
/*
 * QPSK Single-Channel Modem Transceiver - PART 1 (TX)
 * Fully decoupled independent structures ensuring reentrancy.
 * Implements block-based processing using CMSIS-DSP vector operations.
 * Uses non-integer sample rate to symbol rate conversion via dynamic fractional NCOs.
 * Uses ARM_MORPH macro for CMSIS-DSP data types and generic functions.
 * Uses dspdefines.h for optimized trigonometric and square root macros.
 */

#include "dspdefines.h"
#include "audio.h"
#include "formats.h"

#if WITHINTEGRATEDDSP

#include "dspdefines.h"
#include "arm_math.h"

#define RX_NUM_TAPS 31
#define TX_NUM_TAPS 31
#define QPSK_NORM   0.70710678

/* Independent Transmitter Context Configuration */
typedef struct {
    ARM_MORPH(arm_fir_instance) rrc_filter_i;
    ARM_MORPH(arm_fir_instance) rrc_filter_q;

    /* Internalized filter buffers */
    FLOAT_t filter_coeffs[TX_NUM_TAPS];
    FLOAT_t state_buffer_i[TX_NUM_TAPS + 4 - 1];
    FLOAT_t state_buffer_q[TX_NUM_TAPS + 4 - 1];

    FLOAT_t timing_nco;       /* Timing accumulator for DAC step */
    FLOAT_t timing_step;      /* Dynamic symbol_rate / sample_rate ratio */

    FLOAT_t current_symbol_i; /* Current QPSK symbol mapping (I axis) */
    FLOAT_t current_symbol_q; /* Current QPSK symbol mapping (Q axis) */
} qpsk_modem_tx_t;

/* Independent Receiver Context Configuration */
typedef struct {
    ARM_MORPH(arm_fir_instance) matched_filter_i;
    ARM_MORPH(arm_fir_instance) matched_filter_q;

    /* Internalized filter buffers */
    FLOAT_t filter_coeffs[RX_NUM_TAPS];
    FLOAT_t state_buffer_i[RX_NUM_TAPS + 4 - 1];
    FLOAT_t state_buffer_q[RX_NUM_TAPS + 4 - 1];

    /* Carrier recovery (Costas Loop) variables */
    FLOAT_t phase_nco;           /* Carrier phase accumulator */
    FLOAT_t phase_step_nco;      /* Adjusted carrier frequency */
    FLOAT_t costas_kp;           /* Proportional gain for Costas */
    FLOAT_t costas_ki;           /* Integral gain for Costas */
    FLOAT_t costas_integrator;   /* Integrator memory for Costas */

    /* Symbol timing recovery (Gardner Loop) variables */
    FLOAT_t timing_nco;          /* Fractional timing accumulator */
    FLOAT_t timing_step;         /* Base step (2 * Symbol Rate / Sample Rate) */
    FLOAT_t gardner_kp;          /* Proportional gain for Gardner */
    FLOAT_t gardner_ki;          /* Integral gain for Gardner */
    FLOAT_t gardner_integrator;  /* Integrator memory for Gardner */

    /* Buffers for Gardner TED (3 consecutive half-symbol samples required) */
    FLOAT_t history_i[3];        /* [0]=n-1, [1]=n-1/2, [2]=n */
    FLOAT_t history_q[3];
    uint32_t sample_idx;         /* Toggles between 0 (midpoint) and 1 (strobe) */

    FLOAT_t last_filt_i;         /* Interpolator history */
    FLOAT_t last_filt_q;

    /* Lock Detector metric */
    FLOAT_t phase_lock_metric;   /* Exponential moving average phase lock */
    FLOAT_t alpha_lock;          /* Smoothing factor for lock metrics */
    uint32_t is_phase_locked;    /* Boolean status flag */
} qpsk_modem_rx_t;

/* Helper: Root-Raised Cosine (RRC) filter generation */
static void calc_rrc_coeffs(FLOAT_t *coeffs, uint32_t num_taps, FLOAT_t alpha, FLOAT_t samples_per_symbol)
{
    int32_t center = (int32_t)(num_taps - 1) / 2;
    FLOAT_t energy_sum = 0;

    for (int32_t i = 0; i < (int32_t)num_taps; i++)
    {
        FLOAT_t t = (FLOAT_t)(i - center) / samples_per_symbol;

        if (t == 0)
        {
            coeffs[i] = 1 - alpha + 4 * alpha / M_PI;
        }
        else if (t == 1 / (4 * alpha) || t == -1 / (4 * alpha))
        {
            FLOAT_t pi_term = M_PI / 4;
            coeffs[i] = (alpha / (2 * M_PI)) * ((1 + 2 / M_PI) * SINF(pi_term) + (1 - 2 / M_PI) * COSF(pi_term));
        }
        else
        {
            FLOAT_t num1 = SINF(M_PI * t * (1 - alpha));
            FLOAT_t num2 = 4 * alpha * t * COSF(M_PI * t * (1 + alpha));
            FLOAT_t den = M_PI * t * (1 - 4 * alpha * alpha * t * t);

            coeffs[i] = (num1 + num2) / den;
        }

        energy_sum += coeffs[i] * coeffs[i];
    }

    FLOAT_t norm_factor = SQRTF(energy_sum);

    if (norm_factor > 0)
    {
        for (uint32_t i = 0; i < num_taps; i++)
        {
            coeffs[i] /= norm_factor;
        }
    }
}

/* TX Functions */
static void qpsk_modem_tx_init(qpsk_modem_tx_t *self, FLOAT_t sample_rate, FLOAT_t symbol_rate)
{
    self->timing_step = symbol_rate / sample_rate;
    self->timing_nco = 0;
    self->current_symbol_i = QPSK_NORM;
    self->current_symbol_q = QPSK_NORM;

    /* Generate shaping RRC filter coefficients on the fly */
    calc_rrc_coeffs(self->filter_coeffs, TX_NUM_TAPS, 0.35, sample_rate / symbol_rate);

    ARM_MORPH(arm_fir_init)(&self->rrc_filter_i, TX_NUM_TAPS, self->filter_coeffs, self->state_buffer_i, 1);
    ARM_MORPH(arm_fir_init)(&self->rrc_filter_q, TX_NUM_TAPS, self->filter_coeffs, self->state_buffer_q, 1);
}

static void qpsk_modem_tx_reset(qpsk_modem_tx_t *self)
{
    self->timing_nco = 0;
    self->current_symbol_i = QPSK_NORM;
    self->current_symbol_q = QPSK_NORM;
    ARM_MORPH(arm_fill)(0, self->state_buffer_i, TX_NUM_TAPS + 4 - 1);
    ARM_MORPH(arm_fill)(0, self->state_buffer_q, TX_NUM_TAPS + 4 - 1);
}

static void qpsk_modem_tx_block(qpsk_modem_tx_t *self, void (*get_next_2_bits_cb)(uint8_t *bits), FLOAT_t *out_buffer_i, FLOAT_t *out_buffer_q, uint32_t block_size)
{
    for (uint32_t sample_idx = 0; sample_idx < block_size; sample_idx++)
    {
        self->timing_nco += self->timing_step;

        if (self->timing_nco >= 1)
        {
            self->timing_nco -= 1;

            uint8_t tx_bits[2] = {0};
            get_next_2_bits_cb(tx_bits);

            /* Map 2 bits to QPSK constellation axes */
            self->current_symbol_i = tx_bits[0] ? QPSK_NORM : -QPSK_NORM;
            self->current_symbol_q = tx_bits[1] ? QPSK_NORM : -QPSK_NORM;
        }

        /* Pulse shape the continuous mapped symbols through CMSIS-DSP FIR */
        ARM_MORPH(arm_fir)(&self->rrc_filter_i, &self->current_symbol_i, &out_buffer_i[sample_idx], 1);
        ARM_MORPH(arm_fir)(&self->rrc_filter_q, &self->current_symbol_q, &out_buffer_q[sample_idx], 1);
    }
}
/* RX Functions */
static void qpsk_modem_rx_init(qpsk_modem_rx_t *self, FLOAT_t sample_rate, FLOAT_t symbol_rate)
{
    /* Gardner TED loop tracks at exactly 2 samples per symbol grid */
    self->timing_step = (2 * symbol_rate) / sample_rate;
    self->timing_nco = 0;
    self->sample_idx = 0;

    self->phase_nco = 0;
    self->phase_step_nco = 0;

    self->costas_kp = 0.04;
    self->costas_ki = 0.0008;
    self->costas_integrator = 0;

    self->gardner_kp = 0.02;
    self->gardner_ki = 0.0005;
    self->gardner_integrator = 0;

    self->last_filt_i = 0;
    self->last_filt_q = 0;

    self->phase_lock_metric = 0;
    self->alpha_lock = 0.01;
    self->is_phase_locked = 0;

    /* Initialize history arrays using CMSIS block fill */
    ARM_MORPH(arm_fill)(0, self->history_i, 3);
    ARM_MORPH(arm_fill)(0, self->history_q, 3);

    calc_rrc_coeffs(self->filter_coeffs, RX_NUM_TAPS, 0.35, 2);

    ARM_MORPH(arm_fir_init)(&self->matched_filter_i, RX_NUM_TAPS, self->filter_coeffs, self->state_buffer_i, 1);
    ARM_MORPH(arm_fir_init)(&self->matched_filter_q, RX_NUM_TAPS, self->filter_coeffs, self->state_buffer_q, 1);
}

static void qpsk_modem_rx_reset(qpsk_modem_rx_t *self)
{
    self->phase_nco = 0;
    self->phase_step_nco = 0;
    self->costas_integrator = 0;

    self->timing_nco = 0;
    self->gardner_integrator = 0;
    self->sample_idx = 0;

    self->last_filt_i = 0;
    self->last_filt_q = 0;

    ARM_MORPH(arm_fill)(0, self->state_buffer_i, RX_NUM_TAPS + 4 - 1);
    ARM_MORPH(arm_fill)(0, self->state_buffer_q, RX_NUM_TAPS + 4 - 1);

    ARM_MORPH(arm_fill)(0, self->history_i, 3);
    ARM_MORPH(arm_fill)(0, self->history_q, 3);
}

static void qpsk_modem_rx_block(qpsk_modem_rx_t *self, const FLOAT_t *in_buffer_i, const FLOAT_t *in_buffer_q, uint32_t block_size, void (*process_2_bits_cb)(const uint8_t *bits))
{
    for (uint32_t sample_idx = 0; sample_idx < block_size; sample_idx++) {
        FLOAT_t filt_i, filt_q;

        /* 1. Carrier Phase De-rotation (Costas Loop Correction) */
        FLOAT_t cos_p = COSF(self->phase_nco);
        FLOAT_t sin_p = SINF(self->phase_nco);

        FLOAT_t rot_i = in_buffer_i[sample_idx] * cos_p + in_buffer_q[sample_idx] * sin_p;
        FLOAT_t rot_q = in_buffer_q[sample_idx] * cos_p - in_buffer_i[sample_idx] * sin_p;

        /* 2. CMSIS-DSP Matched RRC Filtering Execution */
        ARM_MORPH(arm_fir)(&self->matched_filter_i, &rot_i, &filt_i, 1);
        ARM_MORPH(arm_fir)(&self->matched_filter_q, &rot_q, &filt_q, 1);

        /* 3. Fractional Timing Interpolation Loop (Gardner Grid) */
        self->timing_nco += self->timing_step;

        if (self->timing_nco >= 1) {
            self->timing_nco -= 1;

            /* Linear interpolation based on fractional NCO index */
            FLOAT_t mu = self->timing_nco / self->timing_step;
            FLOAT_t inter_i = self->last_filt_i + mu * (filt_i - self->last_filt_i);
            FLOAT_t inter_q = self->last_filt_q + mu * (filt_q - self->last_filt_q);

            /* Shift tracking histories inside arrays */
            self->history_i[0] = self->history_i[1];
            self->history_i[1] = self->history_i[2];
            self->history_i[2] = inter_i;

            self->history_q[0] = self->history_q[1];
            self->history_q[1] = self->history_q[2];
            self->history_q[2] = inter_q;

            self->sample_idx++;

            /* Evaluate TED and Loop updates every 2 half-symbol periods */
            if (self->sample_idx >= 2) {
                self->sample_idx = 0;

                /* Gardner Timing Error Detector formulated across both I and Q branches */
                FLOAT_t error_gardner_i = self->history_i[1] * (self->history_i[2] - self->history_i[0]);
                FLOAT_t error_gardner_q = self->history_q[1] * (self->history_q[2] - self->history_q[0]);
                FLOAT_t error_gardner = error_gardner_i + error_gardner_q;

                self->gardner_integrator += self->gardner_ki * error_gardner;
                self->timing_step = self->timing_step + self->gardner_integrator + self->gardner_kp * error_gardner;

                /* 4-Quadrant Costas Loop Phase Error Detector execution for QPSK */
                FLOAT_t sign_i = (self->history_i[2] >= 0) ? 1 : -1;
                FLOAT_t sign_q = (self->history_q[2] >= 0) ? 1 : -1;
                FLOAT_t error_costas = sign_i * self->history_q[2] - sign_q * self->history_i[2];

                self->costas_integrator += self->costas_ki * error_costas;
                self->phase_step_nco = self->costas_integrator + self->costas_kp * error_costas;

                /* Phase Lock Detector metric optimized for QPSK */
                FLOAT_t abs_i = (self->history_i[2] >= 0) ? self->history_i[2] : -self->history_i[2];
                FLOAT_t abs_q = (self->history_q[2] >= 0) ? self->history_q[2] : -self->history_q[2];
                FLOAT_t instant_lock = (abs_i - abs_q) * (abs_i - abs_q) / (abs_i * abs_i + abs_q * abs_q + 1e-6);

                self->phase_lock_metric += self->alpha_lock * (instant_lock - self->phase_lock_metric);
                self->is_phase_locked = (self->phase_lock_metric < 0.3) ? 1 : 0; /* Lower values indicate lock near axes intersections */

                /* QPSK Hard Decision Slicer Boundary Extraction (Extracts 2 bits) */
                uint8_t rx_bits[2];
                rx_bits[0] = (self->history_i[2] >= 0) ? 1 : 0;
                rx_bits[1] = (self->history_q[2] >= 0) ? 1 : 0;
                process_2_bits_cb(rx_bits);
            }
        }

        self->last_filt_i = filt_i;
        self->last_filt_q = filt_q;

        /* Propagate NCO phase updates forward tracking input frequency */
        self->phase_nco += self->phase_step_nco;
        if (self->phase_nco >= 2 * M_PI) self->phase_nco -= 2 * M_PI;
        if (self->phase_nco < 0) self->phase_nco += 2 * M_PI;
    }
}

#endif /* WITHINTEGRATEDDSP */



//////////////////
/// test


static void qpsk_test_get2_preamble_bits(uint8_t *bits)
{
	bits [0] = 1;
	bits [1] = 1;
}

static const uint8_t testarray [] =
{
	'D', 'E', 'A', 'D', 'B', 'E', 'E', 'F',
	'A', 'B', 'B', 'A', '1', '9', '8', '0',
};

static int testindex;
static int testbitindex;

static void qpsk_test_get2_bits(uint8_t *bits)
{
	bits [0] = 0;
	bits [1] = 0;
	return;

	const uint8_t data = testarray [testindex];

	bits [0] = !! (data & (UINT8_C(1) << (testbitindex + 0)));
	bits [1] = !! (data & (UINT8_C(1) << (testbitindex + 1)));

	testbitindex = (testbitindex + 2) % 8;
	if (! testbitindex)
		testindex = (testindex + 1) % (sizeof testarray / sizeof testarray [0]);
}

static void test_process2_bits(const uint8_t * bits)
{
	static unsigned v;
	static unsigned bitpos;

	v |= !! bits [0] << (bitpos + 0);
	v |= !! bits [1] << (bitpos + 1);
	bitpos = (bitpos + 2) % 8;
	if (! bitpos)
	{
		PRINTF("%02X ", v);
		v = 0;
	}
}

void modem_test(void)
{
	TP();
	qpsk_modem_tx_t tx;
	qpsk_modem_rx_t rx;
	FLOAT_t sample_rate = ARMSAIRATE;
	FLOAT_t symbol_rate = 1200;
	qpsk_modem_tx_init(& tx, sample_rate, symbol_rate);
	qpsk_modem_rx_init(& rx, sample_rate, symbol_rate);

	enum { BUFFLEN = 256 };
	FLOAT_t buffer_i [BUFFLEN];
	FLOAT_t buffer_q [BUFFLEN];
	qpsk_modem_tx_block(& tx, qpsk_test_get2_preamble_bits, buffer_i, buffer_q, BUFFLEN);
	qpsk_modem_rx_block(& rx, buffer_i, buffer_q, BUFFLEN, test_process2_bits);

	unsigned i;
	for (i = 0; i < 10; ++ i)
	{

		qpsk_modem_tx_block(& tx, qpsk_test_get2_bits, buffer_i, buffer_q, BUFFLEN);
		qpsk_modem_rx_block(& rx, buffer_i, buffer_q, BUFFLEN, test_process2_bits);
	}
	PRINTF("\n");
	TP();
}

#endif /* WITHIF4DSP */
