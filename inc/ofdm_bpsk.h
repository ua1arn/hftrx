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
 * Enhanced with pre-calculated pre-compiled look-up window weights arrays.
 */
typedef struct {
    /* OFDM Framing Serialization State Machine Layer */
    ofdm_tx_fsm_state_t tx_fsm_state;
    uint32_t nco_baud_accumulator;
    uint32_t nco_baud_step;
    uint32_t bit_shifter;
    uint32_t bits_count;
    int tx_active;

    /* Multicarrier Phase Accumulators Arrays */
    FLOAT_t subcarrier_phases[OFDM_MAX_SUBCARRIERS];
    FLOAT_t subcarrier_steps[OFDM_MAX_SUBCARRIERS];
    FLOAT_t subcarrier_sign_q[OFDM_MAX_SUBCARRIERS];
    FLOAT_t magnitude;
    uint32_t active_tones_count;

    /* Differential Encoder Memory Layer */
    uint32_t prev_subcarrier_bits[OFDM_MAX_SUBCARRIERS];

    /* Embedded Sample-by-Sample Raised-Cosine Windowing Layer */
    uint32_t symbol_sample_idx;
    uint32_t total_symbol_len;
    uint32_t window_len;
    uint32_t cp_len;

    /* PRE-CALCULATED LOOK-UP ARRAYS FOR MAXIMUM INTERRUPT PERFORMANCE */
    FLOAT_t window_fade_in[OFDM_MAX_WIN_LEN];  /* Lookup table for smooth fade-in curve */
    FLOAT_t window_fade_out[OFDM_MAX_WIN_LEN]; /* Lookup table for smooth fade-out curve */

    /* Embedded Transmit Thread-Safe Queue Field */
    modem_fifo_t tx_fifo;
} ofdm_modem_tx_t;


#define OFDM_MAX_WIN_LEN        128
#define OFDM_FFT_LUT_SIZE       256  /* Exact static size matching our high-speed 125 Baud FFT window */


/**
 * @brief Complete structure for sample-by-sample multi-carrier OFDM-DBPSK Receiver.
 * Fully optimized with pre-calculated CMSIS-DSP Look-Up Tables (LUT).
 */
typedef struct {
    /* OFDM Framing Deserialization State Machine Layer */
    ofdm_rx_fsm_state_t rx_fsm_state;
    uint32_t symbol_sample_idx;
    uint32_t total_symbol_len;
    uint32_t cp_len;
    uint32_t fft_len;
    int rx_active;

    /* Multicarrier Demodulation Integrators (DFT Core) */
    FLOAT_t integrator_i[OFDM_MAX_SUBCARRIERS];
    FLOAT_t integrator_q[OFDM_MAX_SUBCARRIERS];
    uint32_t active_tones_count;

    /* HIGH-SPEED HARDWARE OPTIMIZED LUT MATRICES */
    /* Dimensions: [subcarrier_index][sample_index_inside_fft_window] */
    FLOAT_t rx_lut_cos[OFDM_MAX_SUBCARRIERS][OFDM_FFT_LUT_SIZE];
    FLOAT_t rx_lut_sin[OFDM_MAX_SUBCARRIERS][OFDM_FFT_LUT_SIZE];

    /* Differential Decoder History Memory Layer */
    FLOAT_t prev_integrator_i[OFDM_MAX_SUBCARRIERS];
    FLOAT_t prev_integrator_q[OFDM_MAX_SUBCARRIERS];

    /* Bit Deserializer Register Grid */
    uint32_t bit_shifter;
    uint32_t bits_count;

    /* Embedded Receive Thread-Safe Queue Field */
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
