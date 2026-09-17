#include "hardware.h"

#if WITHINTEGRATEDDSP && 0

#include "dspdefines.h"
#include "audio.h"
#include "formats.h"

/*
 * BPSK Modem Transceiver
 * Implements non-integer sample rate to symbol rate conversion using dynamic fractional NCOs.
 * Uses CMSIS-DSP via ARM_MORPH macro and hftrx/dspdefines.h for trigonometric operations.
 * Highly encapsulated, static linkage, completely reentrant context model.
 */

#define RX_NUM_TAPS 31
#define TX_NUM_TAPS 31

/* ========================================================================== */
/*                           DATA STRUCTURES & ENUMS                          */
/* ========================================================================== */

/* BPSK Demodulator State Structure with Costas, Gardner Loops and Lock Detectors */
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

    /* Buffer for Gardner TED (3 consecutive half-symbol samples required) */
    FLOAT_t history_i[3];        /* [0]=n-1 (strobe), [1]=n-1/2 (midpoint), [2]=n (strobe) */
    FLOAT_t history_q[3];
    uint32_t sample_idx;         /* Toggles between 0 (midpoint) and 1 (strobe) */

    FLOAT_t last_filt_i;         /* Interpolator history */
    FLOAT_t last_filt_q;

    /* Lock Detectors metrics (EMA / Low Pass Filtered states) */
    FLOAT_t timing_lock_metric;  /* Value near 1.0 indicates stable symbol sync */
    FLOAT_t phase_lock_metric;   /* Value near 1.0 indicates stable phase lock */
    FLOAT_t alpha_lock;          /* Smoothing factor for lock metrics (e.g., 0.01) */

    uint32_t is_timing_locked;   /* Boolean status flag */
    uint32_t is_phase_locked;    /* Boolean status flag */
} bpsk_demod_t;

/* BPSK Modulator State Structure */
typedef struct {
    ARM_MORPH(arm_fir_instance) rrc_filter_i;
    ARM_MORPH(arm_fir_instance) rrc_filter_q;

    /* Internalized filter buffers */
    FLOAT_t filter_coeffs[TX_NUM_TAPS];
    FLOAT_t state_buffer_i[TX_NUM_TAPS + 4 - 1];
    FLOAT_t state_buffer_q[TX_NUM_TAPS + 4 - 1];

    FLOAT_t timing_nco;       /* Timing accumulator for DAC step */
    FLOAT_t timing_step;      /* Dynamic symbol_rate / sample_rate ratio */

    FLOAT_t carrier_phase;    /* IF Carrier phase accumulator */
    FLOAT_t carrier_step;     /* IF Carrier frequency step */

    FLOAT_t last_tx_i;        /* History for fractional interpolation to DAC rate */
    FLOAT_t last_tx_q;

    FLOAT_t current_symbol_i; /* Current symbol mapping (+1.0 or -1.0) */
    FLOAT_t current_symbol_q;
} bpsk_mod_t;

/* Transceiver states enum */
typedef enum {
    BPSK_STATE_IDLE = 0,
    BPSK_STATE_RX,
    BPSK_STATE_TX
} bpsk_state_t;

/* Top-level Transceiver Structure */
typedef struct {
    bpsk_demod_t demod;
    bpsk_mod_t mod;
    bpsk_state_t state;
    FLOAT_t sample_rate;
    FLOAT_t symbol_rate;
    FLOAT_t tx_carrier_freq;
} bpsk_transceiver_t;

/* ========================================================================== */
/*                             FILTER GENERATION                              */
/* ========================================================================== */

/**
 * @brief Computes Root-Raised Cosine (RRC) filter coefficients at runtime.
 * @param coeffs Pointer to the destination array of size num_taps.
 * @param num_taps Number of filter coefficients (must be odd for linear phase).
 * @param alpha Roll-off factor (typically between 0.2 and 0.5).
 * @param samples_per_symbol Number of samples per symbol clock grid.
 */
static void calc_rrc_coeffs(FLOAT_t * coeffs, uint32_t num_taps, FLOAT_t alpha, FLOAT_t samples_per_symbol)
{
    int32_t center = (int32_t)(num_taps - 1) / 2;
    FLOAT_t energy_sum = 0;

    for (int32_t i = 0; i < (int32_t)num_taps; i++)
    {
        /* t is the relative time normalized to the symbol period Ts */
        FLOAT_t t = (FLOAT_t)(i - center) / samples_per_symbol;

        if (t == 0)
        {
            /* Special case at the center of the impulse response (t = 0) */
            coeffs[i] = 1 - alpha + 4 * alpha / M_PI;
        }
        else if (t == 1 / (4 * alpha) || t == -1 / (4 * alpha))
        {
            /* Special case for the indeterminate form 0/0 at t = +- Ts / 4alpha */
            FLOAT_t pi_term = M_PI / 4;
            coeffs[i] = (alpha / (2 * M_PI)) * ((1 + 2 / M_PI) * SINF(pi_term) + (1 - 2 / M_PI) * COSF(pi_term));
        }
        else
        {
            /* Standard Root-Raised Cosine equation */
            FLOAT_t num1 = SINF(M_PI * t * (1 - alpha));
            FLOAT_t num2 = 4 * alpha * t * COSF(M_PI * t * (1 + alpha));
            FLOAT_t den = M_PI * t * (1 - 4 * alpha * alpha * t * t);

            coeffs[i] = (num1 + num2) / den;
        }

        energy_sum += coeffs[i] * coeffs[i];
    }

    /* Compute normalization factor using SQRTF macro from dspdefines.h */
    FLOAT_t norm_factor = SQRTF(energy_sum);

    /* Normalize the filter coefficients to maintain unity energy gain */
    if (norm_factor > 0)
    {
        for (uint32_t i = 0; i < num_taps; i++)
        {
            coeffs[i] /= norm_factor;
        }
    }
}

/* ========================================================================== */
/*                             DEMODULATOR FUNCTIONS                          */
/* ========================================================================== */

/**
 * @brief Initializes modem including Costas, Gardner loop, and Lock Detectors.
 */
static void bpsk_demod_init(bpsk_demod_t * ctx, FLOAT_t sample_rate, FLOAT_t symbol_rate)
{
    /* Gardner requires 2 samples per symbol */
    ctx->timing_step = (2 * symbol_rate) / sample_rate;
    ctx->timing_nco = 0;
    ctx->sample_idx = 0;

    ctx->phase_nco = 0;
    ctx->phase_step_nco = 0;

    ctx->costas_kp = 0.05;
    ctx->costas_ki = 0.001;
    ctx->costas_integrator = 0;

    ctx->gardner_kp = 0.02;
    ctx->gardner_ki = 0.0005;
    ctx->gardner_integrator = 0;

    ctx->last_filt_i = 0;
    ctx->last_filt_q = 0;

    /* Initialize Lock Detectors parameters */
    ctx->timing_lock_metric = 0;
    ctx->phase_lock_metric = 0;
    ctx->alpha_lock = 0.01;      /* Slow time-constant for averaging over ~100 symbols */
    ctx->is_timing_locked = 0;
    ctx->is_phase_locked = 0;

    for (uint32_t i = 0; i < 3; i++) {
        ctx->history_i[i] = 0;
        ctx->history_q[i] = 0;
    }

    /* Calculate RRC coefficients on the fly (Gardner grid relies on exactly 2 samples per symbol) */
    calc_rrc_coeffs(ctx->filter_coeffs, RX_NUM_TAPS, 0.35, 2);

    ARM_MORPH(arm_fir_init)(&ctx->matched_filter_i, RX_NUM_TAPS, ctx->filter_coeffs, ctx->state_buffer_i, 1);
    ARM_MORPH(arm_fir_init)(&ctx->matched_filter_q, RX_NUM_TAPS, ctx->filter_coeffs, ctx->state_buffer_q, 1);
}

/**
 * @brief Resets the demodulator loops, integrators, and internal filter states.
 */
static void bpsk_demod_reset(bpsk_demod_t * ctx)
{
    ctx->phase_nco = 0;
    ctx->phase_step_nco = 0;
    ctx->costas_integrator = 0;

    ctx->timing_nco = 0;
    ctx->gardner_integrator = 0;
    ctx->sample_idx = 0;

    ctx->last_filt_i = 0;
    ctx->last_filt_q = 0;

    for (uint32_t i = 0; i < 3; i++) {
        ctx->history_i[i] = 0;
        ctx->history_q[i] = 0;
    }

    ctx->timing_lock_metric = 0;
    ctx->phase_lock_metric = 0;
    ctx->is_timing_locked = 0;
    ctx->is_phase_locked = 0;

    for (uint32_t i = 0; i < (RX_NUM_TAPS + 4 - 1); i++) {
        ctx->state_buffer_i[i] = 0;
        ctx->state_buffer_q[i] = 0;
    }
}

/**
 * @brief Process single incoming ADC baseband IQ sample with lock estimation.
 */
static uint32_t bpsk_demod_process_sample(bpsk_demod_t * ctx, FLOAT_t in_i, FLOAT_t in_q, FLOAT_t * out_sym_i, FLOAT_t * out_sym_q)
{
    FLOAT_t filtered_i;
    FLOAT_t filtered_q;
    uint32_t symbols_produced = 0;

    ARM_MORPH(arm_fir)(&ctx->matched_filter_i, &in_i, &filtered_i, 1);
    ARM_MORPH(arm_fir)(&ctx->matched_filter_q, &in_q, &filtered_q, 1);

    ctx->timing_nco += (ctx->timing_step + ctx->gardner_integrator);

    if (ctx->timing_nco >= 1)
    {
        ctx->timing_nco -= 1;

        FLOAT_t mu = ctx->timing_nco / (ctx->timing_step + ctx->gardner_integrator);

        FLOAT_t interp_i = ctx->last_filt_i + mu * (filtered_i - ctx->last_filt_i);
        FLOAT_t interp_q = ctx->last_filt_q + mu * (filtered_q - ctx->last_filt_q);

        FLOAT_t cos_p = COSF(ctx->phase_nco);
        FLOAT_t sin_p = SINF(ctx->phase_nco);

        FLOAT_t derot_i = interp_i * cos_p + interp_q * sin_p;
        FLOAT_t derot_q = interp_q * cos_p - interp_i * sin_p;

        ctx->history_i[0] = ctx->history_i[1];
        ctx->history_i[1] = ctx->history_i[2];
        ctx->history_i[2] = derot_i;

        ctx->history_q[0] = ctx->history_q[1];
        ctx->history_q[1] = ctx->history_q[2];
        ctx->history_q[2] = derot_q;

        if (ctx->sample_idx == 1)
        {
            /* 1. Gardner Timing Error Detector & Lock Estimation */
            FLOAT_t error_g = ctx->history_i[1] * (ctx->history_i[2] - ctx->history_i[0]) +
                              ctx->history_q[1] * (ctx->history_q[2] - ctx->history_q[0]);

            ctx->gardner_integrator += error_g * ctx->gardner_ki;

            /* Timing Lock Metric: Evaluate power ratio between Strobe and Midpoint samples */
            FLOAT_t power_strobe = ctx->history_i[2] * ctx->history_i[2] + ctx->history_q[2] * ctx->history_q[2];
            FLOAT_t power_midpoint = ctx->history_i[1] * ctx->history_i[1] + ctx->history_q[1] * ctx->history_q[1];

            FLOAT_t instant_t_metric = 0;
            if (power_strobe + power_midpoint > 0)
            {
                instant_t_metric = (power_strobe - power_midpoint) / (power_strobe + power_midpoint);
            }

            ctx->timing_lock_metric += ctx->alpha_lock * (instant_t_metric - ctx->timing_lock_metric);
            ctx->is_timing_locked = (ctx->timing_lock_metric > 0.5) ? 1 : 0;

            /* 2. Costas Loop Phase Error Detector & Lock Estimation */
            FLOAT_t error_c = derot_i * derot_q;

            ctx->costas_integrator += error_c * ctx->costas_ki;
            ctx->phase_step_nco = error_c * ctx->costas_kp + ctx->costas_integrator;

            /* Phase Lock Metric for BPSK: cos(2*theta) = (I^2 - Q^2) / (I^2 + Q^2) */
            FLOAT_t i2 = derot_i * derot_i;
            FLOAT_t q2 = derot_q * derot_q;

            FLOAT_t instant_p_metric = 0;
            if (i2 + q2 > 0)
            {
                instant_p_metric = (i2 - q2) / (i2 + q2);
            }

            ctx->phase_lock_metric += ctx->alpha_lock * (instant_p_metric - ctx->phase_lock_metric);
            ctx->is_phase_locked = (ctx->phase_lock_metric > 0.6 && ctx->is_timing_locked) ? 1 : 0;

            *out_sym_i = derot_i;
            *out_sym_q = derot_q;
            symbols_produced = 1;

            ctx->sample_idx = 0;
        }
        else
        {
            ctx->sample_idx = 1;
        }
    }

    ctx->phase_nco += ctx->phase_step_nco;

    if (ctx->phase_nco >= 2 * M_PI) {
        ctx->phase_nco -= 2 * M_PI;
    } else if (ctx->phase_nco < 0) {
        ctx->phase_nco += 2 * M_PI;
    }

    ctx->last_filt_i = filtered_i;
    ctx->last_filt_q = filtered_q;

    return symbols_produced;
}

/* ========================================================================== */
/*                              MODULATOR FUNCTIONS                           */
/* ========================================================================== */

/**
 * @brief Initializes the BPSK modulator context.
 */
static void bpsk_mod_init(bpsk_mod_t * ctx, FLOAT_t sample_rate, FLOAT_t symbol_rate, FLOAT_t carrier_freq)
{
    ctx->timing_step = symbol_rate / sample_rate;
    ctx->timing_nco = 1;

    ctx->carrier_phase = 0;
    ctx->carrier_step = (2 * M_PI * carrier_freq) / sample_rate;

    ctx->last_tx_i = 0;
    ctx->last_tx_q = 0;
    ctx->current_symbol_i = 0;
    ctx->current_symbol_q = 0;

    /* Dynamic calculation of oversampling ratio (Samples Per Symbol) */
    FLOAT_t samples_per_symbol = sample_rate / symbol_rate;

    calc_rrc_coeffs(ctx->filter_coeffs, TX_NUM_TAPS, 0.35, samples_per_symbol);

    ARM_MORPH(arm_fir_init)(&ctx->rrc_filter_i, TX_NUM_TAPS, ctx->filter_coeffs, ctx->state_buffer_i, 1);
    ARM_MORPH(arm_fir_init)(&ctx->rrc_filter_q, TX_NUM_TAPS, ctx->filter_coeffs, ctx->state_buffer_q, 1);
}

/**
 * @brief Resets the modulator timing, carrier phase, and filter state buffers.
 */
static void bpsk_mod_reset(bpsk_mod_t * ctx)
{
    ctx->timing_nco = 1;
    ctx->carrier_phase = 0;
    ctx->last_tx_i = 0;
    ctx->last_tx_q = 0;
    ctx->current_symbol_i = 0;
    ctx->current_symbol_q = 0;

    for (uint32_t i = 0; i < (TX_NUM_TAPS + 4 - 1); i++) {
        ctx->state_buffer_i[i] = 0;
        ctx->state_buffer_q[i] = 0;
    }
}

/**
 * @brief Generates a single IQ pair for the DAC destination grid.
 */
static void bpsk_mod_process_sample(bpsk_mod_t * ctx, uint32_t (*get_next_bit_callback)(void), FLOAT_t * out_dac_i, FLOAT_t * out_dac_q)
{
    FLOAT_t rrc_out_i;
    FLOAT_t rrc_out_q;
    FLOAT_t raw_upsample_i = 0;
    FLOAT_t raw_upsample_q = 0;

    ctx->timing_nco += ctx->timing_step;

    if (ctx->timing_nco >= 1)
    {
        ctx->timing_nco -= 1;

        uint32_t bit = get_next_bit_callback();

        ctx->current_symbol_i = bit ? 1 : -1;
        ctx->current_symbol_q = 0;

        raw_upsample_i = ctx->current_symbol_i;
        raw_upsample_q = ctx->current_symbol_q;
    }
    else
    {
        raw_upsample_i = 0;
        raw_upsample_q = 0;
    }

    ARM_MORPH(arm_fir)(&ctx->rrc_filter_i, &raw_upsample_i, &rrc_out_i, 1);
    ARM_MORPH(arm_fir)(&ctx->rrc_filter_q, &raw_upsample_q, &rrc_out_q, 1);

    FLOAT_t mu = ctx->timing_nco;
    FLOAT_t interp_i = ctx->last_tx_i + mu * (rrc_out_i - ctx->last_tx_i);
    FLOAT_t interp_q = ctx->last_tx_q + mu * (rrc_out_q - ctx->last_tx_q);

    ctx->last_tx_i = rrc_out_i;
    ctx->last_tx_q = rrc_out_q;

    if (ctx->carrier_step > 0)
    {
        FLOAT_t cos_c = COSF(ctx->carrier_phase);
        FLOAT_t sin_c = SINF(ctx->carrier_phase);

        *out_dac_i = interp_i * cos_c - interp_q * sin_c;
        *out_dac_q = interp_i * sin_c + interp_q * cos_c;

        ctx->carrier_phase += ctx->carrier_step;
        if (ctx->carrier_phase >= 2 * M_PI)
        {
            ctx->carrier_phase -= 2 * M_PI;
        }
    }
    else
    {
        *out_dac_i = interp_i;
        *out_dac_q = interp_q;
    }
}

/* ========================================================================== */
/*                             TRANSCEIVER CONTROL                            */
/* ========================================================================== */

/**
 * @brief Top-level initialization of the BPSK transceiver.
 */
static void bpsk_trx_init(bpsk_transceiver_t * trx, FLOAT_t sample_rate, FLOAT_t symbol_rate, FLOAT_t tx_carrier_freq)
{
    trx->sample_rate = sample_rate;
    trx->symbol_rate = symbol_rate;
    trx->tx_carrier_freq = tx_carrier_freq;
    trx->state = BPSK_STATE_IDLE;

    bpsk_demod_init(&trx->demod, sample_rate, symbol_rate);
    bpsk_mod_init(&trx->mod, sample_rate, symbol_rate, tx_carrier_freq);
}

/**
 * @brief Sets the functional state of the transceiver machine.
 */
void bpsk_trx_set_state(bpsk_transceiver_t * trx, bpsk_state_t target_state)
{
    if (trx->state == target_state)
    {
        return;
    }

    switch (target_state)
    {
        case BPSK_STATE_IDLE:
            bpsk_demod_reset(&trx->demod);
            bpsk_mod_reset(&trx->mod);
            break;

        case BPSK_STATE_RX:
            bpsk_mod_reset(&trx->mod);
            bpsk_demod_reset(&trx->demod);
            break;

        case BPSK_STATE_TX:
            bpsk_demod_reset(&trx->demod);
            bpsk_mod_reset(&trx->mod);
            break;

        default:
            break;
    }

    trx->state = target_state;
}

/**
 * @brief Unified interface handler to execute processing tasks based on the active state.
 */
uint32_t bpsk_trx_process(bpsk_transceiver_t * trx,
                                 FLOAT_t in_i, FLOAT_t in_q,
                                 FLOAT_t * out_o_i, FLOAT_t * out_o_q,
                                 uint32_t (*get_bit_cb)(void))
{
    uint32_t result_status = 0;

    switch (trx->state)
    {
        case BPSK_STATE_RX:
            result_status = bpsk_demod_process_sample(&trx->demod, in_i, in_q, out_o_i, out_o_q);
            break;

        case BPSK_STATE_TX:
            bpsk_mod_process_sample(&trx->mod, get_bit_cb, out_o_i, out_o_q);
            result_status = 1;
            break;

        case BPSK_STATE_IDLE:
        default:
            *out_o_i = 0;
            *out_o_q = 0;
            break;
    }

    return result_status;
}


void modem_test(void)
{
	TP();
}

#endif /* WITHINTEGRATEDDSP */
