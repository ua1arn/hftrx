#ifndef _RTTY_H_
#define _RTTY_H_

#include <stdint.h>

/* ========================================================================== */
/* RTTY TRANSCEIVER ENUMS AND CORES STRUCTURES DEFINITIONS                    */
/* ========================================================================== */

/**
 * @brief Strictly bounded enum definitions for the asynchronous Baudot RX FSM states.
 */
typedef enum {
    RTTY_STATE_IDLE = 0,
    RTTY_STATE_START_BIT,
    RTTY_STATE_DATA_BITS,
    RTTY_STATE_STOP_BIT
} rtty_fsm_state_t;

/**
 * @brief Strictly bounded enum definitions for the asynchronous Baudot TX FSM states.
 */
typedef enum {
    RTTY_TX_STATE_IDLE = 0,
    RTTY_TX_STATE_START_BIT,
    RTTY_TX_STATE_DATA_BITS,
    RTTY_TX_STATE_STOP_BIT
} rtty_tx_fsm_state_t;

/**
 * @brief Frequency Detector sub-layer state memory structure optimized for Zero-IF PLL.
 */
typedef struct {
    FLOAT_t phase_nco;           /* Phase accumulator for local tracking oscillator */
    FLOAT_t pll_integrator;      /* Integral loop filter memory accumulator */
    FLOAT_t lpf_state;           /* Leaky integrator filter memory envelope */

    /* Dynamic Runtime Loop Coefficients */
    FLOAT_t pll_kp;              /* Proportional loop gain tracking coefficient */
    FLOAT_t pll_ki;              /* Integral loop gain tracking coefficient */
    FLOAT_t pll_limit;           /* Strict physical boundary for targeted FSK shift (radians/sample) */
    FLOAT_t lpf_alpha;           /* Individual payload data slicing filter smoothing ratio */

    int invert_output;           /* Boolean flag to invert the discriminator bit output (0 or 1) */
    FLOAT_t prev_in_i;           /* Historical real memory from previous sample */
    FLOAT_t prev_in_q;           /* Historical imaginary memory from previous sample */
} rtty_freq_detector_t;

/**
 * @brief Baudot Asynchronous FSM sub-layer state machine driven by Integer NCO with embedded thread-safe FIFO.
 */
typedef struct {
    rtty_fsm_state_t fsm_state;  /* Enum tracking active UART/Baudot framing state */
    uint32_t nco_accumulator;    /* 32-bit fixed-point integer phase accumulator (Q32) */
    uint32_t nco_step;           /* 32-bit phase step matching the exact baud rate */
    uint32_t bit_shifter;        /* Shift register collecting raw payload streams */
    uint32_t bits_count;         /* Number of successfully accumulated data bits */
    int is_figures_case;         /* Boolean flag for Baudot ITA2 case shifting (0 or 1) */
    modem_fifo_t rx_fifo;        /* Embedded thread-safe receive queue field inside FSM */
} rtty_baudot_fsm_t;

/**
 * @brief Main unified RTTY receiver containing isolated processing sub-layers.
 */
typedef struct {
    rtty_freq_detector_t detector; /* Embedded PLL/NCO frequency tracking core */
    rtty_baudot_fsm_t    fsm;      /* Embedded integer NCO asynchronous framing engine with internal FIFO */
} rtty_receiver_t;

/**
 * @brief Isolated structure for Phase-Continuous FSK RTTY Transmitter with embedded thread-safe FIFO.
 */
typedef struct {
    /* Baudot Serializer Asynchronous FSM Layer */
    rtty_tx_fsm_state_t tx_fsm_state; /* Active serialization framing mode */
    uint32_t nco_baud_accumulator;    /* 32-bit fixed-point baud clock accumulator (Q32) */
    uint32_t nco_baud_step;           /* 32-bit baud phase increment per sample tick */
    uint32_t bit_shifter;            /* Shift register holding currently transmitted frame */
    uint32_t bits_count;             /* Counter for transmitted data bits */
    int is_figures_case;             /* Boolean flag tracking the active TX case matrix (0 or 1) */
    int tx_active;                   /* Flag indicating active transmission session (0 or 1) */

    /* Phase-Continuous FSK Modulator Layer */
    FLOAT_t phase_carrier;           /* Phase continuous carrier accumulator in radians */
    FLOAT_t freq_shift_half_nco;     /* Target half-shift speed step in radians per sample */
    FLOAT_t magnitude;               /* Output IQ vector amplitude scale */

    /* Case Switching Request Latches */
    int request_letters;             /* Pending latch to inject LTRS escape code (0 or 1) */
    int request_figures;             /* Pending latch to inject FIGS escape code (0 or 1) */
    int invert_output;               /* Boolean flag to invert the frequency shift direction (0 or 1) */

    /* Embedded Transmit Queue Field */
    modem_fifo_t tx_fifo;            /* Embedded thread-safe transmit queue field */
} rtty_transmitter_t;

/* ========================================================================== */
/* RTTY TRANSCEIVER INTERFACE EXPORTED API PROTOTYPES                         */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief GLOBAL RX INITIALIZER: Prepares the RTTY processing core context and flushes its integrated fsm.rx_fifo.
 */
void dsp_rtty_rx_init(
    rtty_receiver_t * const self,
    const uint32_t sample_rate,
    const FLOAT_t shift_hz,
    const FLOAT_t baud_rate);

/**
 * @brief GLOBAL TX INITIALIZER: Prepares the transmitter context and flushes its integrated tx_fifo.
 */
void dsp_rtty_tx_init(
    rtty_transmitter_t * const self,
    const uint32_t sample_rate,
    const FLOAT_t shift_hz,
    const FLOAT_t baud_rate,
    const FLOAT_t output_magnitude);

/**
 * @brief EXPORT EXTERNAL LAYER: Dynamically toggles RTTY RX spectrum inversion mode at runtime.
 */
void dsp_rtty_rx_set_reverse(rtty_receiver_t * const self, const int invert);

/**
 * @brief EXPORT EXTERNAL LAYER: Dynamically toggles RTTY TX spectrum shift inversion mode at runtime.
 */
void dsp_rtty_tx_set_reverse(rtty_transmitter_t * const self, const int invert);

/**
 * @brief EXPORT EXTERNAL LAYER: Interface function to safely push a character directly into the transmitter's embedded queue.
 * @return uint32_t Returns 1 on successful placement, 0 if internal storage queue is full.
 */
uint32_t dsp_rtty_tx_push_char(rtty_transmitter_t * const self, const uint8_t character);

/**
 * @brief EXPORT EXTERNAL LAYER: Interface function to safely pop a decoded character directly from the receiver's embedded FSM queue.
 * @return uint32_t Returns 1 if valid text byte was extracted, 0 if queue is currently empty.
 */
uint32_t dsp_rtty_rx_pop_char(rtty_receiver_t * const self, uint8_t * const output_byte);

/**
 * @brief EXPORT LAYER: Main RTTY processing function for streaming input IQ samples.
 */
void dsp_rtty_rx_process_sample(
	    rtty_receiver_t * const self,
	    const FLOAT_t in_i,
	    const FLOAT_t in_q,
	    void (* const put_char_cb)(rtty_baudot_fsm_t * self, const uint8_t character));

/**
 * @brief EXPORT LAYER: Main RTTY processing function for generating output IQ samples from the embedded tx_fifo.
 */
void dsp_rtty_tx_process_sample(
    rtty_transmitter_t * const self,
    FLOAT_t * const out_i,
    FLOAT_t * const out_q);


void dsp_rttyrxcharacter(rtty_baudot_fsm_t * self, const uint8_t c);
uint32_t rtty_rx_byte(rtty_baudot_fsm_t * const self, uint8_t *output_byte);

#ifdef __cplusplus
}
#endif

#endif /* _RTTY_H_ */
