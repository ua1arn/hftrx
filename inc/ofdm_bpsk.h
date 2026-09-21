#ifndef _OFDM_BPSK_H_
#define _OFDM_BPSK_H_

#include <stdint.h>
#include "rtty.h"  /* Shared inclusion for the unified lock-free modem_fifo_t core */

#define OFDM_MAX_SUBCARRIERS    16
#define OFDM_MAX_WIN_LEN        128  /* Maximum physical boundary for window weight arrays */

/* ========================================================================== */
/* OFDM TRANSCEIVER ENUMS AND CORES STRUCTURES DEFINITIONS                    */
/* ========================================================================== */

typedef enum {
    OFDM_TX_STATE_IDLE = 0,
    OFDM_TX_STATE_PREAMBLE,
    OFDM_TX_STATE_DATA_BITS
} ofdm_tx_fsm_state_t;

typedef enum {
    OFDM_RX_STATE_IDLE = 0,
    OFDM_RX_STATE_SYNC,
    OFDM_RX_STATE_DATA_BITS
} ofdm_rx_fsm_state_t;

/**
 * @brief Isolated structure for sample-by-sample multi-carrier OFDM-DBPSK Transmitter.
 * Enhanced with pre-calculated windows, LFSR scrambler, and symmetric quad-converter tracking.
 */
typedef struct {
    /* OFDM Framing Serialization State Machine Layer */
    ofdm_tx_fsm_state_t tx_fsm_state; /* Active serialization framing state machine mode */
    uint32_t nco_baud_accumulator;    /* 32-bit fixed-point symbol clock phase accumulator (Q32) */
    uint32_t nco_baud_step;           /* 32-bit symbol phase increment step per sample tick */
    uint32_t bit_shifter;            /* Shift register holding currently serialized payload byte */
    uint32_t bits_count;             /* Counter tracking successfully serialized payload data bits */
    int tx_active;                   /* Boolean flag tracking active transmission session (0 or 1) */

    /* Multicarrier Phase Accumulators Arrays */
    FLOAT_t subcarrier_phases[OFDM_MAX_SUBCARRIERS]; /* Continuous phase tracker for each parallel tone */
    FLOAT_t subcarrier_steps[OFDM_MAX_SUBCARRIERS];  /* Radians frequency movement step per sample tick */
    FLOAT_t subcarrier_sign_q[OFDM_MAX_SUBCARRIERS]; /* Complex quadrature sign inversion table for symmetric Zero-IF */
    FLOAT_t magnitude;                               /* Combined multi-carrier output IQ vector scale */
    uint32_t active_tones_count;                     /* Number of active parallel subcarriers used (<= 16) */

    /* Differential Encoder & Scrambler Memory Layer */
    uint32_t prev_subcarrier_bits[OFDM_MAX_SUBCARRIERS]; /* Phase polarity memory register grid */
    uint32_t scrambler_state;         /* LFSR register memory state (Galois polynomial tracking) */
    uint32_t current_bpsk_vector;     /* Encapsulated active parallel BPSK modulation symbols vector */

    /* Embedded Sample-by-Sample Raised-Cosine Windowing Layer */
    uint32_t symbol_sample_idx;      /* Linear index counter inside current active OFDM block */
    uint32_t total_symbol_len;       /* Complete hardware block frame duration width: FFT_LEN + CP_LEN */
    uint32_t window_len;             /* Configured smoothing edge window width size (TX_W_LEN) */
    uint32_t cp_len;                 /* Cyclic prefix guard interval time-domain width (CP_LEN) */

    /* Pre-calculated Look-Up Arrays For Maximum Interrupt Performance */
    FLOAT_t window_fade_in[OFDM_MAX_WIN_LEN];  /* Lookup table for smooth fade-in curve */
    FLOAT_t window_fade_out[OFDM_MAX_WIN_LEN]; /* Lookup table for smooth fade-out curve */

    /* Embedded Transmit Thread-Safe Queue Field */
    modem_fifo_t tx_fifo;            /* Embedded thread-safe lock-free SPSC transmit queue field */
} ofdm_modem_tx_t;

#define OFDM_MAX_WIN_LEN        128
#define OFDM_FFT_LUT_SIZE       256  /* Exact static size matching our high-speed 125 Baud FFT window */

typedef struct {
    ofdm_rx_fsm_state_t rx_fsm_state;
    uint32_t symbol_sample_idx;
    uint32_t total_symbol_len;
    uint32_t cp_len;
    uint32_t fft_len;
    int rx_active;

    FLOAT_t integrator_i[OFDM_MAX_SUBCARRIERS];
    FLOAT_t integrator_q[OFDM_MAX_SUBCARRIERS];
    uint32_t active_tones_count;

    FLOAT_t rx_lut_cos[OFDM_MAX_SUBCARRIERS][OFDM_FFT_LUT_SIZE];
    FLOAT_t rx_lut_sin[OFDM_MAX_SUBCARRIERS][OFDM_FFT_LUT_SIZE];

    FLOAT_t prev_integrator_i[OFDM_MAX_SUBCARRIERS];
    FLOAT_t prev_integrator_q[OFDM_MAX_SUBCARRIERS];

    /* BAREMETAL CORES MEMORY STATE EXTENSIONS FOR DE-SCRAMBLER */
    uint32_t scrambler_state;         /* LFSR receiver memory state tracking */

    uint32_t bit_shifter;
    uint32_t bits_count;

    modem_fifo_t rx_fifo;
} ofdm_modem_rx_t;

/* ========================================================================== */
/* OFDM TRANSCEIVER INTERFACE EXPORTED API PROTOTYPES                         */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

void dsp_ofdm_tx_init(
	    ofdm_modem_tx_t * const self,
	    const uint32_t sample_rate,
	    const uint32_t fft_len,
	    const uint32_t cp_len,
	    const uint32_t tx_w_len,
	    const FLOAT_t output_magnitude);

uint32_t dsp_ofdm_tx_push_char(ofdm_modem_tx_t * const self, const uint8_t character);

void dsp_ofdm_tx_process_sample(
    ofdm_modem_tx_t * const self,
    FLOAT_t * const out_i,
    FLOAT_t * const out_q);

void dsp_ofdm_rx_init(
	    ofdm_modem_rx_t * const self,
	    const uint32_t sample_rate,
	    const uint32_t fft_len,
	    const uint32_t cp_len);

uint32_t dsp_ofdm_rx_pop_char(ofdm_modem_rx_t * const self, uint8_t * const output_byte);

void dsp_ofdm_rx_process_sample(
    ofdm_modem_rx_t * const self,
    const FLOAT_t in_i,
    const FLOAT_t in_q);

#ifdef __cplusplus
}
#endif

#endif /* _OFDM_BPSK_H_ */
