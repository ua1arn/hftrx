#include "hardware.h"

#if WITHINTEGRATEDDSP

#include "dspdefines.h"

/* AI-generated */

#define RX_NUM_TAPS 31

/* BPSK Demodulator State Structure with Costas and Gardner Loops */
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
} bpsk_demod_t;


/**
 * @brief Computes Root-Raised Cosine (RRC) filter coefficients at runtime based on dynamic oversampling.
 * @param coeffs Pointer to the destination array of size num_taps.
 * @param num_taps Number of filter coefficients (must be odd for linear phase).
 * @param alpha Roll-off factor (typically between 0.2 and 0.5).
 * @param samples_per_symbol Dynamic calculated ratio of sample_rate / symbol_rate.
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

/**
 * @brief Initializes modem including Costas and Gardner loop parameters.
 * @param ctx Pointer to the demodulator context structure.
 * @param sample_rate Input ADC rate in Hz.
 * @param symbol_rate Target Baud rate.
 */
static void bpsk_demod_init(bpsk_demod_t * ctx, FLOAT_t sample_rate, FLOAT_t symbol_rate)
{
    /* Gardner requires 2 samples per symbol */
    ctx->timing_step = (2 * symbol_rate) / sample_rate;
    ctx->timing_nco = 0;
    ctx->sample_idx = 0;

    ctx->phase_nco = 0;
    ctx->phase_step_nco = 0;

    /* Loop coefficients - adjusted via Type Promotion rules */
    ctx->costas_kp = 0.05;
    ctx->costas_ki = 0.001;
    ctx->costas_integrator = 0;

    ctx->gardner_kp = 0.02;
    ctx->gardner_ki = 0.0005;
    ctx->gardner_integrator = 0;

    ctx->last_filt_i = 0;
    ctx->last_filt_q = 0;

    for (uint32_t i = 0; i < RX_NUM_TAPS; i++) {
        ctx->filter_coeffs[i] = 0; /* Coefficients should be initialized dynamically later */
    }

    for (uint32_t i = 0; i < 3; i++) {
        ctx->history_i[i] = 0;
        ctx->history_q[i] = 0;
    }

    FLOAT_t samples_per_symbol = sample_rate / symbol_rate;
    calc_rrc_coeffs(ctx->filter_coeffs, RX_NUM_TAPS, 0.35, samples_per_symbol);

    ARM_MORPH(arm_fir_init)(&ctx->matched_filter_i, RX_NUM_TAPS, ctx->filter_coeffs, ctx->state_buffer_i, 1);
    ARM_MORPH(arm_fir_init)(&ctx->matched_filter_q, RX_NUM_TAPS, ctx->filter_coeffs, ctx->state_buffer_q, 1);
}

/**
 * @brief Process single incoming ADC baseband IQ sample.
 * @param ctx Pointer to the demodulator context structure.
 * @param in_i Input I
 * @param in_q Input Q
 * @param out_sym_i Buffer for extracted synchronized symbol I
 * @param out_sym_q Buffer for extracted synchronized symbol Q
 * @return uint32_t Number of valid symbols written to output (0 or 1).
 */
static uint32_t bpsk_demod_process_sample(bpsk_demod_t * ctx, FLOAT_t in_i, FLOAT_t in_q, FLOAT_t * out_sym_i, FLOAT_t * out_sym_q)
{
    FLOAT_t filtered_i;
    FLOAT_t filtered_q;
    uint32_t symbols_produced = 0;

    ARM_MORPH(arm_fir)(&ctx->matched_filter_i, &in_i, &filtered_i, 1);
    ARM_MORPH(arm_fir)(&ctx->matched_filter_q, &in_q, &filtered_q, 1);

    /* Update timing NCO with current loop adjusted step */
    ctx->timing_nco += (ctx->timing_step + ctx->gardner_integrator);

    if (ctx->timing_nco >= 1)
    {
        ctx->timing_nco -= 1;

        /* Linear interpolation weight calculation */
        FLOAT_t mu = ctx->timing_nco / (ctx->timing_step + ctx->gardner_integrator);

        FLOAT_t interp_i = ctx->last_filt_i + mu * (filtered_i - ctx->last_filt_i);
        FLOAT_t interp_q = ctx->last_filt_q + mu * (filtered_q - ctx->last_filt_q);

        /* Apply Carrier Phase Correction via COSF/SINF macros from dspdefines.h */
        FLOAT_t cos_p = COSF(ctx->phase_nco);
        FLOAT_t sin_p = SINF(ctx->phase_nco);

        FLOAT_t derot_i = interp_i * cos_p + interp_q * sin_p;
        FLOAT_t derot_q = interp_q * cos_p - interp_i * sin_p;

        /* Update history shift registers for the Gardner TED */
        ctx->history_i[0] = ctx->history_i[1];
        ctx->history_i[1] = ctx->history_i[2];
        ctx->history_i[2] = derot_i;

        ctx->history_q[0] = ctx->history_q[1];
        ctx->history_q[1] = ctx->history_q[2];
        ctx->history_q[2] = derot_q;

        /* Alternating between midpoint sample and strobe sample */
        if (ctx->sample_idx == 1)
        {
            /* 1. Gardner Timing Error Detector computation */
            /* error = I(n-1/2) * (I(n) - I(n-1)) + Q(n-1/2) * (Q(n) - Q(n-1)) */
            FLOAT_t error_g = ctx->history_i[1] * (ctx->history_i[2] - ctx->history_i[0]) +
                              ctx->history_q[1] * (ctx->history_q[2] - ctx->history_q[0]);

            /* PI Filter for Gardner Loop */
            ctx->gardner_integrator += error_g * ctx->gardner_ki;

            /* 2. Costas Loop Phase Error Detector (for BPSK: error = I * Q) */
            FLOAT_t error_c = derot_i * derot_q;

            /* PI Filter for Costas Loop */
            ctx->costas_integrator += error_c * ctx->costas_ki;
            ctx->phase_step_nco = error_c * ctx->costas_kp + ctx->costas_integrator;

            /* Return verified symbol sample */
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

    /* Advance Carrier NCO phase tracking */
    ctx->phase_nco += ctx->phase_step_nco;

    /* Wrap-around phase normalization inside [0, 2*PI] */
    if (ctx->phase_nco >= 2 * M_PI) {
        ctx->phase_nco -= 2 * M_PI;
    } else if (ctx->phase_nco < 0) {
        ctx->phase_nco += 2 * M_PI;
    }

    ctx->last_filt_i = filtered_i;
    ctx->last_filt_q = filtered_q;

    return symbols_produced;
}

#define TX_NUM_TAPS 31

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

/**
 * @brief Initializes the BPSK modulator context.
 * @param ctx Pointer to the modulator context structure.
 * @param sample_rate Output DAC sample rate in Hz.
 * @param symbol_rate Target transmission symbol rate in Baud.
 * @param carrier_freq Optional IF carrier frequency in Hz (0 for baseband IQ).
 */
static void bpsk_mod_init(bpsk_mod_t * ctx, FLOAT_t sample_rate, FLOAT_t symbol_rate, FLOAT_t carrier_freq)
{
    /* Calculate precise fractional step for the DAC grid */
    ctx->timing_step = symbol_rate / sample_rate;
    ctx->timing_nco = 1; /* Force immediate symbol fetch on first sample */

    ctx->carrier_phase = 0;
    ctx->carrier_step = (2 * M_PI * carrier_freq) / sample_rate;

    ctx->last_tx_i = 0;
    ctx->last_tx_q = 0;
    ctx->current_symbol_i = 0;
    ctx->current_symbol_q = 0;

    /* Dynamic calculation of oversampling ratio (Samples Per Symbol) */
    FLOAT_t samples_per_symbol = sample_rate / symbol_rate;

    /* Calculate RRC coefficients on the fly based on the calculated oversampling */
    calc_rrc_coeffs(ctx->filter_coeffs, TX_NUM_TAPS, 0.35, samples_per_symbol);

    ARM_MORPH(arm_fir_init)(&ctx->rrc_filter_i, TX_NUM_TAPS, ctx->filter_coeffs, ctx->state_buffer_i, 1);
    ARM_MORPH(arm_fir_init)(&ctx->rrc_filter_q, TX_NUM_TAPS, ctx->filter_coeffs, ctx->state_buffer_q, 1);
}

/**
 * @brief Generates a single IQ pair for the DAC destination grid.
 * @param ctx Pointer to the modulator context structure.
 * @param get_next_bit_callback Pointer to external function providing the next data bit (returns 0 or 1).
 * @param out_dac_i Pointer to output DAC In-phase destination sample.
 * @param out_dac_q Pointer to output DAC Quadrature destination sample.
 */
static void bpsk_mod_process_sample(bpsk_mod_t * ctx, uint32_t (*get_next_bit_callback)(void), FLOAT_t * out_dac_i, FLOAT_t * out_dac_q)
{
    FLOAT_t rrc_out_i;
    FLOAT_t rrc_out_q;
    FLOAT_t raw_upsample_i = 0;
    FLOAT_t raw_upsample_q = 0;

    /* Advance symbol timing NCO based on DAC sample clock */
    ctx->timing_nco += ctx->timing_step;

    /* Check if it is time to map a new symbol */
    if (ctx->timing_nco >= 1)
    {
        ctx->timing_nco -= 1;

        /* Fetch new data bit via callback */
        uint32_t bit = get_next_bit_callback();

        /* BPSK Mapping: 1 -> +1.0, 0 -> -1.0 using Type Promotion rules */
        ctx->current_symbol_i = bit ? 1 : -1;
        ctx->current_symbol_q = 0;

        /* Impulse excitation for the shaping filter */
        raw_upsample_i = ctx->current_symbol_i;
        raw_upsample_q = ctx->current_symbol_q;
    }
    else
    {
        /* Stuffing zeros between symbols (classic upsampling processing) */
        raw_upsample_i = 0;
        raw_upsample_q = 0;
    }

    /* Pulse shaping via CMSIS-DSP FIR instance */
    ARM_MORPH(arm_fir)(&ctx->rrc_filter_i, &raw_upsample_i, &rrc_out_i, 1);
    ARM_MORPH(arm_fir)(&ctx->rrc_filter_q, &raw_upsample_q, &rrc_out_q, 1);

    /* Fractional interpolation for precise positioning between symbol intervals */
    FLOAT_t mu = ctx->timing_nco;
    FLOAT_t interp_i = ctx->last_tx_i + mu * (rrc_out_i - ctx->last_tx_i);
    FLOAT_t interp_q = ctx->last_tx_q + mu * (rrc_out_q - ctx->last_tx_q);

    /* Store current shaping outputs for the next interpolation step */
    ctx->last_tx_i = rrc_out_i;
    ctx->last_tx_q = rrc_out_q;

    /* Digital Upconversion (DUC) if carrier frequency is non-zero */
    if (ctx->carrier_step > 0)
    {
        FLOAT_t cos_c = COSF(ctx->carrier_phase);
        FLOAT_t sin_c = SINF(ctx->carrier_phase);

        /* Complex mixing to IF target */
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
        /* Direct Baseband IQ Output */
        *out_dac_i = interp_i;
        *out_dac_q = interp_q;
    }
}

#endif /* WITHINTEGRATEDDSP */
