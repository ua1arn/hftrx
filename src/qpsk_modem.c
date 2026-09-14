#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if WITHIF4DSP
#include "dspdefines.h"
#include "formats.h"

/* Standard QPSK Constellation point scaling: 1 / sqrt(2) ≈ 0.7071067811865475 */
#define QPSK_VAL   0.7071067811865475

/**
 * @brief  Modulates a byte stream into an interleaved I/Q sample stream using QPSK with Gray coding.
 *         Outputs exactly 8 interleaved I/Q samples per symbol (16 FLOAT_t values per 2 bits).
 *
 * @param  iq_out       Pointer to the target interleaved destination buffer [I0, Q0, I1, Q1, ...].
 *                      Must be allocated to hold at least (num_bytes * 4 * 8 * 2) FLOAT_t elements.
 * @param  data_in      Pointer to the source binary data bytes.
 * @param  num_bytes    Total number of raw data bytes to modulate.
 * @return int          Total number of interleaved complex samples generated (samples_count = num_bytes * 4 * 8).
 */
static int qpsk_modulate_bytes(FLOAT_t *const iq_out, const uint8_t *const data_in, const int num_bytes) {
    int sample_idx = 0;

    for (int b = 0; b < num_bytes; b++) {
        const uint8_t current_byte = data_in[b];

        /* Extract 4 dibits (pairs of 2 bits) from each byte, from MSB to LSB */
        for (int pair = 0; pair < 4; pair++) {
            const int shift = 6 - pair * 2;
            const uint8_t dibit = (current_byte >> shift) & 3;

            FLOAT_t i_val = 0;
            FLOAT_t q_val = 0;

            /* Map to QPSK Constellation using Gray Coding:
               00 -> (+0.707, +0.707)
               01 -> (-0.707, +0.707)
               11 -> (-0.707, -0.707)
               10 -> (+0.707, -0.707) */
            switch (dibit) {
                case 0: /* 00 */
                    i_val = QPSK_VAL;
                    q_val = QPSK_VAL;
                    break;
                case 1: /* 01 */
                    i_val = -QPSK_VAL;
                    q_val = QPSK_VAL;
                    break;
                case 3: /* 11 */
                    i_val = -QPSK_VAL;
                    q_val = -QPSK_VAL;
                    break;
                case 2: /* 10 */
                    i_val = QPSK_VAL;
                    q_val = -QPSK_VAL;
                    break;
            }

            /* First sample holds the actual symbol value data */
            iq_out[sample_idx] = i_val;
            iq_out[sample_idx + 1] = q_val;
            sample_idx += 2;

            /* Insert 7 zero-stuffed samples to achieve SAMPLES_PER_SYMBOL = 8 (Upsampling) */
            for (int zero_fill = 0; zero_fill < 7; zero_fill++) {
                iq_out[sample_idx] = 0;
                iq_out[sample_idx + 1] = 0;
                sample_idx += 2;
            }
        }
    }

    /* Return total complex samples generated */
    return sample_idx / 2;
}

/**
 * @brief  Generates a Root Raised Cosine (RRC) pulse shaping filter coefficients
 *         using analytical formulas, applies a preformed window, and normalizes total energy.
 *
 * @param  h                 Pointer to target array for calculated coefficients (allocated size must be >= num_taps).
 * @param  preformed_window  Pointer to the pre-calculated window coefficients (size must be >= num_taps).
 * @param  num_taps          Fixed length of the FIR filter (Must be an ODD number, typically 49 for 6-symbol delay).
 * @param  sps               Samples per symbol factor (strictly fixed to 8 in this modem architecture).
 * @param  beta              Roll-off factor (passed as 0.35 according to modem specifications).
 * @return None
 */
static void calculate_rrc_coeffs(FLOAT_t *const h, const FLOAT_t *const preformed_window, const int num_taps, const int sps, const FLOAT_t beta) {
    const FLOAT_t alpha = (num_taps - 1) / 2;
    const int half_taps = (num_taps + 1) / 2;

    /* Step 1: Synthesize raw analytical RRC impulse response using time-domain index mapping */
    for (int n = 0; n < half_taps; n++) {
        /* Normalized time variable relative to symbol period T (where T = sps samples) */
        const FLOAT_t t = (n - alpha) / sps;

        FLOAT_t h_target = 0;

        /* Check for the core singularity point at the absolute center of the pulse (t == 0) */
        if (t == 0) {
            h_target = 1 - beta + 4 * beta / M_PI;
        }
        /* Check for special critical zero-denominator points at t == ±1 / (4 * beta) */
        else if (FABSF(FABSF(t) - 1 / (4 * beta)) < 1e-7) {
            const FLOAT_t term1 = 1 + 2 / M_PI;
            const FLOAT_t term2 = 1 - 2 / M_PI;
            h_target = (beta / SQRTF(2)) * (term1 * COSF(M_PI / (4 * beta)) + term2 * SINF(M_PI / (4 * beta)));
        }
        /* General continuous evaluation formula for all other time coordinates */
        else {
            const FLOAT_t sin_term = SINF(M_PI * t * (1 - beta));
            const FLOAT_t cos_term = COSF(M_PI * t * (1 + beta));
            const FLOAT_t numerator = sin_term + 4 * beta * t * cos_term;
            const FLOAT_t denominator = M_PI * t * (1 - 16 * beta * beta * t * t);
            h_target = numerator / denominator;
        }

        /* Store raw coefficients symmetrically directly into the output buffer */
        h[n] = h_target;
        h[num_taps - 1 - n] = h[n];
    }

    /* Step 2: Apply the preformed window via optimized vector multiplication from CMSIS-DSP */
    ARM_MORPH(arm_mult)(h, preformed_window, h, num_taps);

    /* Step 3: Precise energy normalization (Sum of squares must equal 1 for unity gain) */
    FLOAT_t energy_sum = 0;

    /* Calculate the total energy profile of the filter vector using arm_power */
    ARM_MORPH(arm_power)(h, num_taps, &energy_sum);
    energy_sum = FABSF(energy_sum);

    if (energy_sum > 0) {
        const FLOAT_t scale_factor = 1 / SQRTF(energy_sum);

        /* Re-scale the entire FIR array to establish strict unity energy distribution */
        ARM_MORPH(arm_scale)(h, scale_factor, h, num_taps);
    }
}
/* Unified Structure containing the entire state and contexts of the QPSK Modulator */
typedef struct {
    int num_taps;
    int block_size_samples;         /* Block size measured in complex samples (pairs of I/Q) */
    FLOAT_t *rrc_coeffs;            /* Allocated memory size must be >= num_taps */
    FLOAT_t *state_buffer;          /* Allocated memory size must be >= (2 * num_taps + 2 * block_size_samples - 2) */
    ARM_MORPH(arm_fir_instance) fir_instance;
} qpsk_modulator_t;

/**
 * @brief  Initializes the QPSK Modulator workspace, builds the RRC filter, and binds CMSIS-DSP contexts.
 *
 * @param  mod              Pointer to the target modulator instance structure.
 * @param  num_taps         Length of the RRC filter (Must be an ODD number, typically 49).
 * @param  block_samples    The streaming audio/IQ block length measured in complex sample points (pairs).
 * @param  preformed_window Pointer to the pre-calculated window coefficients (size must be >= num_taps).
 * @param  coeffs_mem       Pointer to the static array reserved for filter weights (size >= num_taps).
 * @param  state_mem        Pointer to the history state array (size >= 2 * num_taps + 2 * block_samples - 2).
 * @return None
 */
static void qpsk_modulator_init(qpsk_modulator_t *const mod, const int num_taps, const int block_samples, const FLOAT_t *const preformed_window, FLOAT_t *const coeffs_mem, FLOAT_t *const state_mem) {
    mod->num_taps = num_taps;
    mod->block_size_samples = block_samples;
    mod->rrc_coeffs = coeffs_mem;
    mod->state_buffer = state_mem;

    /* 1. Generate the Root Raised Cosine filter coefficients (SAMPLES_PER_SYMBOL is strictly fixed to 8, beta is 0.35) */
    calculate_rrc_coeffs(mod->rrc_coeffs, preformed_window, mod->num_taps, 8, 0.35);

    /* 2. Bind targets into standard CMSIS-DSP FIR instance configuration structure */
    /* Note: For interleaved complex processing, arm_fir_init expects the state buffer layout to adapt accordingly */
    ARM_MORPH(arm_fir_init)(&(mod->fir_instance), mod->num_taps, mod->rrc_coeffs, mod->state_buffer, mod->block_size_samples * 2);
}

/**
 * @brief  Executes single-pass spectral pulse shaping filtration on an interleaved I/Q stream.
 *         Processes raw upsampled symbols to narrow down the transmission bandwidth via CMSIS-DSP.
 *
 * @param  mod   Pointer to the active modulator instance context structure.
 * @param  pSrc  Pointer to the source interleaved upsampled I/Q buffer [I0, Q0, I1, Q1, ...].
 * @param  pDst  Pointer to the destination filtered and spectrum-shaped interleaved I/Q write buffer.
 * @return None
 */
static void qpsk_modulator_shape_pulse(qpsk_modulator_t *const mod, const FLOAT_t *pSrc, FLOAT_t *pDst) {
    /* CMSIS-DSP real FIR can be perfectly applied to interleaved complex data blocks if we treat */
    /* the incoming payload as two independent interleaved streams (Real/Imaginary paths) processed sequentially */
    ARM_MORPH(arm_fir)(&(mod->fir_instance), (FLOAT_t *)pSrc, pDst, mod->block_size_samples * 2);
}
/* Unified Structure containing the entire state and contexts of the Receiver Matched RRC Filter */
typedef struct {
    int num_taps;
    int block_size_samples;         /* Block size measured in complex samples (pairs of I/Q) */
    FLOAT_t *rrc_coeffs;            /* Allocated memory size must be >= num_taps */
    FLOAT_t *state_buffer;          /* Allocated memory size must be >= (2 * num_taps + 2 * block_size_samples - 2) */
    ARM_MORPH(arm_fir_instance) fir_instance;
} qpsk_rx_matched_filter_t;

/**
 * @brief  Initializes the Receiver Matched RRC Filter workspace and binds CMSIS-DSP contexts.
 *         Uses the exact same RRC synthesis parameters (sps=8, beta=0.35) as the transmitter.
 *
 * @param  rx_filter        Pointer to the target receiver matched filter instance structure.
 * @param  num_taps         Length of the RRC filter (Must be an ODD number, typically 49).
 * @param  block_samples    The streaming IQ block length measured in complex sample points (pairs).
 * @param  preformed_window Pointer to the pre-calculated window coefficients (size must be >= num_taps).
 * @param  coeffs_mem       Pointer to the static array reserved for filter weights (size >= num_taps).
 * @param  state_mem        Pointer to the history state array (size >= 2 * num_taps + 2 * block_samples - 2).
 * @return None
 */
static void qpsk_rx_matched_filter_init(qpsk_rx_matched_filter_t *const rx_filter, const int num_taps, const int block_samples, const FLOAT_t *const preformed_window, FLOAT_t *const coeffs_mem, FLOAT_t *const state_mem) {
    rx_filter->num_taps = num_taps;
    rx_filter->block_size_samples = block_samples;
    rx_filter->rrc_coeffs = coeffs_mem;
    rx_filter->state_buffer = state_mem;

    /* 1. Generate the same Root Raised Cosine filter coefficients as used in TX (SAMPLES_PER_SYMBOL = 8, beta = 0.35) */
    calculate_rrc_coeffs(rx_filter->rrc_coeffs, preformed_window, rx_filter->num_taps, 8, 0.35);

    /* 2. Bind targets into standard CMSIS-DSP FIR instance configuration structure for interleaved complex data */
    ARM_MORPH(arm_fir_init)(&(rx_filter->fir_instance), rx_filter->num_taps, rx_filter->rrc_coeffs, rx_filter->state_buffer, rx_filter->block_size_samples * 2);
}

/**
 * @brief  Executes single-pass matched filtration on an incoming interleaved complex I/Q stream.
 *         Maximizes SNR and shapes the signal before timing and carrier recovery blocks.
 *
 * @param  rx_filter Pointer to the active receiver matched filter instance context structure.
 * @param  pSrc      Pointer to the source incoming interleaved raw I/Q buffer [I0, Q0, I1, Q1, ...].
 * @param  pDst      Pointer to the destination filtered interleaved I/Q write buffer.
 * @return None
 */
static void qpsk_rx_matched_filter_process(qpsk_rx_matched_filter_t *const rx_filter, const FLOAT_t *pSrc, FLOAT_t *pDst) {
    /* Process the interleaved payload as two independent interleaved streams (Real/Imaginary paths) */
    ARM_MORPH(arm_fir)(&(rx_filter->fir_instance), (FLOAT_t *)pSrc, pDst, rx_filter->block_size_samples * 2);
}
/* Loop Filter Proportional (Kp) and Integral (Ki) gains for timing recovery loop tracking */
#define TIMING_LOOP_KP    0.01
#define TIMING_LOOP_KI    0.0001

/* Interleaved IQ complex access macros */
#define I_SAMPLE(ptr, idx) ((ptr)[(idx) * 2])
#define Q_SAMPLE(ptr, idx) ((ptr)[((idx) * 2) + 1])

/* Unified Structure containing the entire state of the Gardner Timing Recovery Loop */
typedef struct {
    FLOAT_t loop_integrator;        /* Integral accumulator of the PI loop filter */
    FLOAT_t fractional_symbol_idx;  /* Dynamic internal timing pointer inside symbol boundary */
    int last_strobe_idx;            /* Index tracker of the previously selected strobe sample */

    /* Memory history registers to hold preceding complex strobe and midpoint samples */
    FLOAT_t prev_strobe_i;
    FLOAT_t prev_strobe_q;
    FLOAT_t prev_midpoint_i;
    FLOAT_t prev_midpoint_q;
} qpsk_rx_timing_recovery_t;

/**
 * @brief  Resets and prepares the Gardner tracking loop state instance.
 *
 * @param  loop  Pointer to the target timing recovery loop instance structure.
 * @return None
 */
static void qpsk_rx_timing_init(qpsk_rx_timing_recovery_t *const loop) {
    loop->loop_integrator = 0;
    loop->fractional_symbol_idx = 0;
    loop->last_strobe_idx = 0;
    loop->prev_strobe_i = 0;
    loop->prev_strobe_q = 0;
    loop->prev_midpoint_i = 0;
    loop->prev_midpoint_q = 0;
}

/**
 * @brief  Executes Gardner Timing Error Detection and decimates incoming 8-sps samples
 *         down to exactly 1-sps optimal strobes for subsequent phase demodulation.
 *
 * @param  loop             Pointer to the active timing tracking instance structure.
 * @param  iq_in            Pointer to the source filtered incoming interleaved I/Q stream (8 samples per symbol).
 * @param  in_samples_count Total size of the input buffer measured in complex samples (pairs).
 * @param  iq_out           Pointer to the destination array where optimal 1-sps strobes will be stored.
 *                          The allocation size must be sufficient to hold up to (in_samples_count / 8 + 2) pairs.
 * @return int              Total number of synchronized 1-sps symbols extracted into the destination buffer.
 */
static int qpsk_rx_timing_process(qpsk_rx_timing_recovery_t *const loop, const FLOAT_t *const iq_in, const int in_samples_count, FLOAT_t *const iq_out) {
    int symbols_extracted = 0;

    for (int k = 0; k < in_samples_count; k++) {
        /* Increment internal pointer by base step modified by the loop filter accumulator output */
        /* Base step is 1.0, meaning we naturally advance 1 sample per hardware loop iteration */
        loop->fractional_symbol_idx += 1 + loop->loop_integrator;

        /* A symbol boundary occurs roughly every 8 samples */
        if (loop->fractional_symbol_idx >= 8) {
            /* Wrap the accumulator pointer back within the 8-sample modulo boundary */
            loop->fractional_symbol_idx -= 8;

            /* Identify indices for current strobe and midpoint samples (located 4 samples backwards) */
            const int current_strobe_k = k;
            const int current_midpoint_k = k - 4;

            FLOAT_t strobe_i = 0;
            FLOAT_t strobe_q = 0;
            FLOAT_t midpoint_i = 0;
            FLOAT_t midpoint_q = 0;

            /* Extract current strobe values from input stream or fall back to previous state history */
            if (current_strobe_k >= 0) {
                strobe_i = I_SAMPLE(iq_in, current_strobe_k);
                strobe_q = Q_SAMPLE(iq_in, current_strobe_k);
            } else {
                strobe_i = loop->prev_strobe_i;
                strobe_q = loop->prev_strobe_q;
            }

            /* Extract current midpoint values from input stream or fall back to previous state history */
            if (current_midpoint_k >= 0) {
                midpoint_i = I_SAMPLE(iq_in, current_midpoint_k);
                midpoint_q = Q_SAMPLE(iq_in, current_midpoint_k);
            } else {
                midpoint_i = loop->prev_midpoint_i;
                midpoint_q = loop->prev_midpoint_q;
            }

            /* Calculate Gardner Timing Error metric using complex signal transitions */
            const FLOAT_t timing_error = midpoint_i * (strobe_i - loop->prev_strobe_i) +
                                         midpoint_q * (strobe_q - loop->prev_strobe_q);

            /* Pass the error value through Proportional-Integral (PI) loop filter architecture */
            loop->loop_integrator += timing_error * TIMING_LOOP_KI;
            const FLOAT_t step_adjustment = timing_error * TIMING_LOOP_KP + loop->loop_integrator;

            /* Enforce proportional loop tuning constraints to maintain loop stability boundaries */
            if (loop->loop_integrator > 0.1)  loop->loop_integrator = 0.1;
            if (loop->loop_integrator < -0.1) loop->loop_integrator = -0.1;

            /* Output the synchronized 1-sps optimal complex sample pair */
            I_SAMPLE(iq_out, symbols_extracted) = strobe_i;
            Q_SAMPLE(iq_out, symbols_extracted) = strobe_q;
            symbols_extracted += 1;

            /* Update state histories for the subsequent iteration evaluation */
            loop->prev_strobe_i = strobe_i;
            loop->prev_strobe_q = strobe_q;
            loop->prev_midpoint_i = midpoint_i;
            loop->prev_midpoint_q = midpoint_q;
        }
    }

    return symbols_extracted;
}
/* Loop Filter Proportional (Kp) and Integral (Ki) gains for carrier recovery tracking */
#define CARRIER_LOOP_KP   0.05
#define CARRIER_LOOP_KI   0.0005

/* Unified Structure containing the entire state of the QPSK Costas Loop */
typedef struct {
    FLOAT_t phase;              /* Current estimated carrier phase error in radians */
    FLOAT_t frequency;          /* Current estimated frequency offset in radians/sample */
} qpsk_rx_carrier_recovery_t;

/**
 * @brief  Resets and prepares the Costas Loop tracking state instance.
 *
 * @param  loop  Pointer to the target carrier recovery loop instance structure.
 * @return None
 */
static void qpsk_rx_carrier_init(qpsk_rx_carrier_recovery_t *const loop) {
    loop->phase = 0;
    loop->frequency = 0;
}

/**
 * @brief  Executes 4th-order Costas Loop to track carrier phase/frequency offsets
 *         and locks the QPSK constellation points perfectly to the decision axes.
 *
 * @param  loop          Pointer to the active carrier tracking instance structure.
 * @param  iq_in         Pointer to the source 1-sps synchronized complex samples array.
 * @param  symbols_count Total number of complex symbols inside the incoming buffer.
 * @param  iq_out        Pointer to the destination array where phase-corrected symbols are stored.
 *                       Can be identical to iq_in for in-place processing (size >= symbols_count * 2).
 * @return None
 */
static void qpsk_rx_carrier_process(qpsk_rx_carrier_recovery_t *const loop, const FLOAT_t *const iq_in, const int symbols_count, FLOAT_t *const iq_out) {
    for (int s = 0; s < symbols_count; s++) {
        const int idx = s * 2;
        const FLOAT_t i_in = iq_in[idx];
        const FLOAT_t q_in = iq_in[idx + 1];

        /* Calculate current oscillator mixing coordinates */
        const FLOAT_t cos_theta = COSF(loop->phase);
        const FLOAT_t sin_theta = SINF(loop->phase);

        /* Step 1: Rotate input complex symbol by the tracked loop phase offset */
        const FLOAT_t i_out = i_in * cos_theta + q_in * sin_theta;
        const FLOAT_t q_out = q_in * cos_theta - i_in * sin_theta;

        /* Write phase-corrected coordinates into output matrix arrays */
        iq_out[idx] = i_out;
        iq_out[idx + 1] = q_out;

        /* Step 2: Extract QPSK Phase Error using sign-slicing decision cross-multiplication */
        const FLOAT_t sign_i = (i_out >= 0) ? 1 : -1;
        const FLOAT_t sign_q = (q_out >= 0) ? 1 : -1;
        const FLOAT_t phase_error = sign_i * q_out - sign_q * i_out;

        /* Step 3: Pass error metric through the Proportional-Integral (PI) Loop Filter layout */
        loop->frequency += phase_error * CARRIER_LOOP_KI;
        loop->phase += phase_error * CARRIER_LOOP_KP + loop->frequency;

        /* Standard phase modulo wrapping to maintain precision within the (-M_PI, +M_PI) boundaries */
        if (loop->phase > M_PI) {
            loop->phase -= 2 * M_PI;
        }
        else if (loop->phase < -M_PI) {
            loop->phase += 2 * M_PI;
        }
    }
}
/**
 * @brief  Slices phase-corrected I/Q symbols and decodes them into a raw byte stream
 *         using inverse Gray mapping. Accumulates 4 symbols (8 bits) per output byte.
 *
 * @param  data_out         Pointer to the target destination buffer for decoded bytes.
 *                          Must be allocated to hold at least (symbols_count / 4) bytes.
 * @param  iq_in            Pointer to the source synchronized and phase-locked interleaved I/Q stream.
 * @param  symbols_count    Total number of complex symbols inside the incoming buffer.
 *                          Should ideally be a multiple of 4 to extract full bytes.
 * @return int              Total number of fully decoded data bytes written to data_out.
 */
static int qpsk_demodulate_bytes(uint8_t *const data_out, const FLOAT_t *const iq_in, const int symbols_count) {
    int byte_idx = 0;
    uint8_t current_byte = 0;

    for (int s = 0; s < symbols_count; s++) {
        const int idx = s * 2;
        const FLOAT_t i_val = iq_in[idx];
        const FLOAT_t q_val = iq_in[idx + 1];

        uint8_t dibit = 0;

        /* Hard decision slicing based on quadrant mapping:
           I >= 0, Q >= 0 -> 00 (0)
           I <  0, Q >= 0 -> 01 (1)
           I <  0, Q <  0 -> 11 (3)
           I >= 0, Q <  0 -> 10 (2) */
        if (i_val >= 0) {
            if (q_val >= 0) {
                dibit = 0; /* 00 */
            } else {
                dibit = 2; /* 10 */
            }
        } else {
            if (q_val >= 0) {
                dibit = 1; /* 01 */
            } else {
                dibit = 3; /* 11 */
            }
        }

        /* Identify symbol position within the current forming byte (0 to 3) */
        const int pair_pos = s & 3;
        const int shift = 6 - pair_pos * 2;

        /* Pack the decoded 2-bit symbol into the appropriate byte position */
        current_byte |= (dibit << shift);

        /* Once 4 symbols (8 bits) are aggregated, flush the byte to the destination array */
        if (pair_pos == 3) {
            data_out[byte_idx] = current_byte;
            byte_idx += 1;
            current_byte = 0; /* Clear accumulator for the next byte sequence */
        }
    }

    /* Return total number of completely reconstructed bytes */
    return byte_idx;
}

/* --- Top-Level Integrated QPSK Modem Structure --- */
typedef struct {
    /* Operational block metrics */
    int rrc_num_taps;
    int tx_block_samples;           /* TX processing capacity measured in complex sample pairs */
    int rx_block_samples;           /* RX processing capacity measured in complex sample pairs */

    /* Sub-component functional context instances */
    qpsk_modulator_t tx_mod;
    qpsk_rx_matched_filter_t rx_filter;
    qpsk_rx_timing_recovery_t rx_timing;
    qpsk_rx_carrier_recovery_t rx_carrier;
} qpsk_modem_t;

/**
 * @brief  Initializes the complete top-level QPSK modem entity, allocating internal
 *         sub-component contexts and executing structural filters compilation.
 *
 * @param  mod              Pointer to the target top-level modem context structure.
 * @param  rrc_num_taps     Length of the RRC filters (Must be an ODD number, typically 49).
 * @param  tx_block_samples The execution block length for TX path measured in complex samples.
 * @param  rx_block_samples The execution block length for RX path measured in complex samples.
 * @param  preformed_window Pointer to the pre-calculated window coefficients (size >= rrc_num_taps).
 * @param  tx_coeffs_mem    Static array memory for TX filter weights (size >= rrc_num_taps).
 * @param  tx_state_mem     Static array memory for TX state history (size >= 2 * rrc_num_taps + 2 * tx_block_samples - 2).
 * @param  rx_coeffs_mem    Static array memory for RX filter weights (size >= rrc_num_taps).
 * @param  rx_state_mem     Static array memory for RX state history (size >= 2 * rrc_num_taps + 2 * rx_block_samples - 2).
 * @return None
 */
static void qpsk_modem_init(qpsk_modem_t *const mod, const int rrc_num_taps, const int tx_block_samples, const int rx_block_samples, const FLOAT_t *const preformed_window, FLOAT_t *const tx_coeffs_mem, FLOAT_t *const tx_state_mem, FLOAT_t *const rx_coeffs_mem, FLOAT_t *const rx_state_mem) {
    mod->rrc_num_taps = rrc_num_taps;
    mod->tx_block_samples = tx_block_samples;
    mod->rx_block_samples = rx_block_samples;

    /* 1. Initialize Transmitter Path Sub-systems */
    qpsk_modulator_init(&(mod->tx_mod), rrc_num_taps, tx_block_samples, preformed_window, tx_coeffs_mem, tx_state_mem);

    /* 2. Initialize Receiver Path Sub-systems */
    qpsk_rx_matched_filter_init(&(mod->rx_filter), rrc_num_taps, rx_block_samples, preformed_window, rx_coeffs_mem, rx_state_mem);
    qpsk_rx_timing_init(&(mod->rx_timing));
    qpsk_rx_carrier_init(&(mod->rx_carrier));
}

/**
 * @brief  Top-level Transmitter API function. Converts raw data bytes directly into
 *         a spectrum-shaped, upsampled interleaved complex I/Q hardware transmission stream.
 *
 * @param  mod           Pointer to the active top-level modem context structure.
 * @param  data_in       Pointer to the source binary payload bytes to transmit.
 * @param  num_bytes     Total size of the binary payload data in bytes.
 * @param  iq_out        Pointer to the target interleaved I/Q destination buffer [I0, Q0, I1, ...].
 *                       Must be allocated to hold at least (num_bytes * 64) FLOAT_t variables.
 * @param  tmp_upsample_buf Temporary workspace buffer required to hold upsampled raw symbols.
 *                          Must be allocated to hold at least (num_bytes * 64) FLOAT_t variables.
 * @return int           Total number of complex samples generated inside iq_out.
 */
static int qpsk_modem_transmit(qpsk_modem_t *const mod, const uint8_t *const data_in, const int num_bytes, FLOAT_t *const iq_out, FLOAT_t *const tmp_upsample_buf) {
    /* 1. Map bits to Gray-coded constellation points and upsample by stuffing zeros (8 sps) */
    const int generated_samples = qpsk_modulate_bytes(tmp_upsample_buf, data_in, num_bytes);

    /* 2. Process chunks of upsampled samples through the RRC pulse shaping FIR filter */
    int processed_samples = 0;
    while (processed_samples < generated_samples) {
        /* Determine chunk boundaries matching the predefined processing block configuration */
        const int samples_to_process = (generated_samples - processed_samples < mod->tx_block_samples) ?
                                       (generated_samples - processed_samples) : mod->tx_block_samples;

        /* Execute RRC shaping directly onto the streaming chunk partition */
        qpsk_modulator_shape_pulse(&(mod->tx_mod), &tmp_upsample_buf[processed_samples * 2], &iq_out[processed_samples * 2]);
        processed_samples += samples_to_process;
    }

    return processed_samples;
}

/**
 * @brief  Top-level Receiver API function. Processes an incoming chunk of raw interleaved
 *         complex hardware I/Q samples through the entire demodulation stack to recover data bytes.
 *
 * @param  mod               Pointer to the active top-level modem context structure.
 * @param  iq_in             Pointer to the source incoming interleaved raw 8-sps I/Q sample stream.
 * @param  iq_matched_out    Pointer to an internal temporary storage buffer for matched filtering output.
 *                           Size must be >= (mod->rx_block_samples * 2) FLOAT_t elements.
 * @param  iq_strobe_buf     Pointer to an internal temporary storage buffer for timing loop outputs.
 *                           Size must be >= (mod->rx_block_samples / 4) FLOAT_t elements.
 * @param  data_out          Pointer to the target destination buffer for extracted data bytes.
 *                           Size must be >= (mod->rx_block_samples / 32) bytes.
 * @return int               Total number of fully reconstructed data bytes successfully decoded.
 */
static int qpsk_modem_receive(qpsk_modem_t *const mod, const FLOAT_t *const iq_in, FLOAT_t *const iq_matched_out, FLOAT_t *const iq_strobe_buf, uint8_t *const data_out) {
    /* 1. Execute input matched RRC filtration to maximize SNR and reject adjacent channel noise */
    qpsk_rx_matched_filter_process(&(mod->rx_filter), iq_in, iq_matched_out);

    /* 2. Execute Gardner Timing Tracking to extract 1-sps synchronous strobes from 8-sps data stream */
    const int extracted_symbols = qpsk_rx_timing_process(&(mod->rx_timing), iq_matched_out, mod->rx_block_samples, iq_strobe_buf);

    /* If no full symbol boundaries were tracked during this block period, exit early */
    if (extracted_symbols <= 0) {
        return 0;
    }

    /* 3. Execute 4th-order Costas Loop to track carrier offset and lock constellation orientation axes */
    /* Processing can be safely executed in-place inside the strobe workspace buffer to save RAM footprint */
    qpsk_rx_carrier_process(&(mod->rx_carrier), iq_strobe_buf, extracted_symbols, iq_strobe_buf);

    /* 4. Perform hard decision quadrant slicing and unpack Gray symbols back into binary data bytes */
    const int decoded_bytes = qpsk_demodulate_bytes(data_out, iq_strobe_buf, extracted_symbols);

    return decoded_bytes;
}
#include <stdint.h>
#include "dspdefines.h"    /* Hardware floating point macros, FLOAT_t, and arm_math.h inclusions */

/* ========================================================================= */
/* CONFIGURATION METRICS AND MEMORY POOL CALCULATIONS                        */
/* ========================================================================= */

#define MODEM_RRC_TAPS          49   /* RRC filter length (typically 6 symbols * 8 sps + 1) */
#define MODEM_TX_BLOCK_SAMPLES  4096//64   /* Number of complex I/Q samples processed per TX FIR iteration */
#define MODEM_RX_BLOCK_SAMPLES  4096//128  /* Number of complex I/Q samples processed per RX DMA hardware interrupt */

/* Maximum application data capacity for a single transmission burst transaction */
#define APP_MAX_DATA_BYTES      4096//32

/* Derived memory requirements for internal state buffers (CMSIS-DSP layout criteria) */
#define TX_STATE_SIZE  (2 * MODEM_RRC_TAPS + 2 * MODEM_TX_BLOCK_SAMPLES - 2)
#define RX_STATE_SIZE  (2 * MODEM_RRC_TAPS + 2 * MODEM_RX_BLOCK_SAMPLES - 2)

/* Derived size for upsampled transmission workspace (32 bytes * 4 symbols/byte * 8 sps * 2 elements[I,Q]) */
#define TX_UPSAMPLE_BUF_SIZE    (APP_MAX_DATA_BYTES * 4 * 8 * 2)

/* --- Static Memory Allocation Pools (Internal Linkage via static) --- */
static FLOAT_t preformed_window_mem[MODEM_RRC_TAPS];

/* Modulator (TX) dedicated memory assets */
static FLOAT_t tx_coeffs_pool[MODEM_RRC_TAPS];
static FLOAT_t tx_state_pool[TX_STATE_SIZE];
static FLOAT_t tx_upsample_workspace[TX_UPSAMPLE_BUF_SIZE];
static FLOAT_t tx_hardware_output_io[TX_UPSAMPLE_BUF_SIZE]; /* Shaped I/Q payload ready for DAC/DMA */

/* Demodulator (RX) dedicated memory assets */
static FLOAT_t rx_coeffs_pool[MODEM_RRC_TAPS];
static FLOAT_t rx_state_pool[RX_STATE_SIZE];
static FLOAT_t rx_matched_workspace[MODEM_RX_BLOCK_SAMPLES * 2];
static FLOAT_t rx_strobe_workspace[(MODEM_RX_BLOCK_SAMPLES / 4) * 2]; /* Holds 1-sps strobes, 2 floats per pair */
static uint8_t rx_decoded_data_payload[MODEM_RX_BLOCK_SAMPLES / 32];  /* Extracted data payload output */

/* Global static driver context instance */
static qpsk_modem_t hf_digital_modem;

/* ========================================================================= */
/* APPLICATION LAYER CONTROL IMPLEMENTATION                                  */
/* ========================================================================= */

/**
 * @brief  System hardware boot-strapping layer. Prepares allocation links
 *         and starts initial modulators and tracking loop configurations.
 * @return None
 */
static void app_modem_system_setup(void) {
    /*
     * APPLICATION NOTE:
     * Before invoking this setup routine, 'preformed_window_mem' must be populated
     * with valid window coefficients (e.g., Blackman-Harris or Hamming) matching
     * the chosen length 'MODEM_RRC_TAPS'. This can be done via a startup helper
     * or standard memory mapping initialization hooks.
     */

    /* Initialize the integrated top-level modem object, linking all static memory blocks */
    qpsk_modem_init(
        &hf_digital_modem,
        MODEM_RRC_TAPS,
        MODEM_TX_BLOCK_SAMPLES,
        MODEM_RX_BLOCK_SAMPLES,
        preformed_window_mem,
        tx_coeffs_pool,
        tx_state_pool,
        rx_coeffs_pool,
        rx_state_pool
    );
}

/**
 * @brief  Example application trigger function. Invoked when data needs to be pushed
 *         to the physical transmission medium (RF power amplifier path via DAC).
 *
 * @param  payload_data  Pointer to the source binary array containing data bytes to transfer.
 * @param  length_bytes  Total number of active payload bytes to modulate (must be <= APP_MAX_DATA_BYTES).
 * @return int           Total number of generated FLOAT_t elements inside 'tx_hardware_output_io'.
 */
static int app_modem_execute_transmission(const uint8_t *const payload_data, const int length_bytes) {
    int generated_floats = 0;

    if (length_bytes > 0 && length_bytes <= APP_MAX_DATA_BYTES) {
        /* Run high-level modulation pipeline to convert bytes into ready-to-transmit shaped I/Q samples */
        const int generated_complex_samples = qpsk_modem_transmit(
            &hf_digital_modem,
            payload_data,
            length_bytes,
            tx_hardware_output_io,
            tx_upsample_workspace
        );

        /* Calculate total float values count (each complex sample = I + Q = 2 floats) */
        generated_floats = generated_complex_samples * 2;
    }

    /* Returns count of valid data elements inside 'tx_hardware_output_io' to be routed to DAC DMA */
    return generated_floats;
}

/**
 * @brief  Example interrupt service vector routine (ISR). Triggered when the input
 *         ADC/SDR hardware pipeline finishes gathering a frame of complex I/Q stream.
 *
 * @param  raw_dma_iq_input  Pointer to the active raw hardware input buffer [I0, Q0, I1, Q1, ...].
 *                           Size must match (MODEM_RX_BLOCK_SAMPLES * 2) elements.
 * @return None
 */
static void app_modem_rx_dma_callback_isr(const FLOAT_t *const raw_dma_iq_input) {
    /*
     * Process incoming raw hardware block through the complete receiver demodulation pipeline.
     * Uses preallocated static workspaces to avoid frame processing latency.
     */
    const int recovered_bytes_count = qpsk_modem_receive(
        &hf_digital_modem,
        raw_dma_iq_input,
        rx_matched_workspace,
        rx_strobe_workspace,
        rx_decoded_data_payload
    );

    /* If bytes were successfully decoded from current frame, route them to application dispatcher */
    if (recovered_bytes_count > 0) {
    	printhex_titled(0, rx_decoded_data_payload, recovered_bytes_count, "rx_decoded_data_payload");
        for (int i = 0; i < recovered_bytes_count; i++) {
            const uint8_t data_byte = rx_decoded_data_payload[i];
            /* Execute packet assembly, CRC check, or pass to the terminal console handler */
             (void)data_byte;
        }
    }
}
/**
 * @brief  Pre-calculates window coefficients into the application memory pool
 *         using optimized CMSIS-DSP window generation functions.
 *         Must be executed once during hardware boot-strapping before initializing the modem.
 *
 * @param  window_mem   Pointer to the target destination window array in memory.
 *                      Allocation size must be exactly equal to num_taps.
 * @param  num_taps     Length of the window to generate (typically MODEM_RRC_TAPS = 49).
 * @return None
 */
static void app_modem_init_window(FLOAT_t *const window_mem, const int num_taps) {
    /*
     * Synthesize Blackman-Harris (92 dB) window weights into the provided buffer.
     * The ARM_MORPH macro resolves to either 'arm_blackman_harris_92db_f32'
     * or 'arm_blackman_harris_92db_f64' based on current project compiler settings.
     */
    ARM_MORPH(arm_blackman_harris_92db)(window_mem, num_taps);
}


void modem_test(void)
{
	TP();
	app_modem_init_window(preformed_window_mem, MODEM_RRC_TAPS);
	app_modem_system_setup();
	static const uint8_t txarray [] =
	{
			0x55, 0x55, 0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,
			0x55, 0x55, 0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,
			0x55, 0x55, 0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,
			0x55, 0x55, 0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,
			0x55, 0x55, 0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,  0x55, 0x55,
			0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
			0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
			0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
			0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
			0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
			0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
			0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
			0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF,
	};
	int n = app_modem_execute_transmission(txarray, ARRAY_SIZE(txarray));
	PRINTF("n=%d samples\n", n);
	TP();
	app_modem_rx_dma_callback_isr(tx_hardware_output_io);
	app_modem_rx_dma_callback_isr(tx_hardware_output_io);
	app_modem_rx_dma_callback_isr(tx_hardware_output_io);
	app_modem_rx_dma_callback_isr(tx_hardware_output_io);
	app_modem_rx_dma_callback_isr(tx_hardware_output_io);
	app_modem_rx_dma_callback_isr(tx_hardware_output_io);
}

#endif /* WITHIF4DSP */
