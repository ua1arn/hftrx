#include "hardware.h"

#if WITHINTEGRATEDDSP && 1

#include "dspdefines.h"
#include "audio.h"
#include "serial.h"
#include "buffers.h"
#include "formats.h"
#include "display/display.h"

#define MODEM_FIFO_SIZE         256

/* Strictly bounded enum definitions for the asynchronous Baudot FSM states */
typedef enum {
    RTTY_STATE_IDLE = 0,
    RTTY_STATE_START_BIT,
    RTTY_STATE_DATA_BITS,
    RTTY_STATE_STOP_BIT
} rtty_fsm_state_t;

typedef enum {
    RTTY_TX_STATE_IDLE = 0,
    RTTY_TX_STATE_START_BIT,
    RTTY_TX_STATE_DATA_BITS,
    RTTY_TX_STATE_STOP_BIT
} rtty_tx_fsm_state_t;

/* Unified and fully lock-free single-producer single-consumer circular queue */
typedef struct {
    uint8_t storage[MODEM_FIFO_SIZE];
    volatile uint32_t head;
    volatile uint32_t tail;
    /* Field volatile uint32_t count is completely removed to secure atomicity */
} modem_fifo_t;

/* Frequency Detector sub-layer state memory structure optimized for Zero-IF PLL/NCO */
typedef struct {
    FLOAT_t prev_in_i;           /* Historical real memory from previous sample */
    FLOAT_t prev_in_q;           /* Historical imaginary memory from previous sample */

    FLOAT_t phase_nco;           /* Phase accumulator for local tracking oscillator */
    FLOAT_t pll_kp;              /* Proportional loop gain tracking coefficient */
    FLOAT_t pll_ki;              /* Integral loop gain tracking coefficient */
    FLOAT_t pll_integrator;      /* Integral loop filter memory accumulator */

    FLOAT_t lpf_state;           /* Leaky integrator filter memory envelope */
    FLOAT_t lpf_alphaOLD;           /* Smoothing ratio optimized for baud carrier */
    FLOAT_t lpf_alphaNEW;           /* Smoothing ratio optimized for baud carrier */
    int invert_output;           /* Boolean flag to invert the discriminator bit output (0 or 1) */
} rtty_freq_detector_t;

/* Isolated structure for Phase-Continuous FSK RTTY Transmitter with embedded FIFO */
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

    /* EMBEDDED TRANSMIT QUEUE FIELD */
    modem_fifo_t tx_fifo;            /* Dedicated standalone volatile atomic FIFO ring */
} rtty_transmitter_t;

/* Baudot Asynchronous FSM sub-layer state machine driven by Integer NCO */
typedef struct {
    rtty_fsm_state_t fsm_state;  /* Enum tracking active UART/Baudot framing state */
    uint32_t nco_accumulator;    /* 32-bit fixed-point integer phase accumulator (Q32) */
    uint32_t nco_step;           /* 32-bit phase step matching the exact baud rate */
    uint32_t bit_shifter;        /* Shift register collecting raw payload streams */
    uint32_t bits_count;         /* Number of successfully accumulated data bits */
    int is_figures_case;         /* Boolean flag for Baudot ITA2 case shifting (0 or 1) */
    modem_fifo_t rx_fifo;
} rtty_baudot_fsm_t;

/* Main unified RTTY receiver containing isolated processing sub-layers as fields */
typedef struct {
    rtty_freq_detector_t detector; /* Embedded frequency discriminator core */
    rtty_baudot_fsm_t    fsm;      /* Embedded integer NCO asynchronous framing engine */
} rtty_receiver_t;


/* ========================================================================== */
/*                             INTERNAL FIFO HELPERS                          */
/* ========================================================================== */

/**
 * @brief Thread-safe lock-free buffer initialization.
 */
static void fifo_init(modem_fifo_t * const fifo)
{
    fifo->head = 0;
    fifo->tail = 0;
}

/**
 * @brief Thread-safe lock-free byte injection (Called strictly by ONE producer thread/interrupt).
 * @return uint32_t Returns 1 on success, 0 if the buffer is mathematically full.
 */
static uint32_t fifo_push(modem_fifo_t * const fifo, const uint8_t data)
{
    const uint32_t next_head = (fifo->head + 1) % MODEM_FIFO_SIZE;

    /* Check if the next step hits the tail pointer boundary (Buffer Full) */
    if (next_head == fifo->tail) {
        return 0;
    }

    fifo->storage[fifo->head] = data;

    /* Atomic write of the head index closes the transaction. Interrupt safe. */
    fifo->head = next_head;
    return 1;
}

/**
 * @brief Thread-safe lock-free byte extraction (Called strictly by ONE consumer thread/interrupt).
 * @return uint32_t Returns 1 on success, 0 if the buffer is empty.
 */
static uint32_t fifo_pop(modem_fifo_t * const fifo, uint8_t * const data)
{
    /* If head and tail pointers are equal, the ring is mathematically empty */
    if (fifo->tail == fifo->head) {
        return 0;
    }

    *data = fifo->storage[fifo->tail];

    /* Atomic write of the tail index closes the transaction. Interrupt safe. */
    fifo->tail = (fifo->tail + 1) % MODEM_FIFO_SIZE;
    return 1;
}

/**
 * @brief Supplementary helper to safely extract current elements count at runtime.
 */
static uint32_t fifo_get_count(const modem_fifo_t * const fifo)
{
    const uint32_t snapshot_head = fifo->head;
    const uint32_t snapshot_tail = fifo->tail;

    if (snapshot_head >= snapshot_tail) {
        return snapshot_head - snapshot_tail;
    }

    return (MODEM_FIFO_SIZE - snapshot_tail) + snapshot_head;
}


/////////////////////////////////
/// TX

/**
 * @brief SUB-FUNCTION: Asynchronous Baudot serialization machine execution loop step.
 * @param self Pointer to the active isolated transmitter context.
 * @return uint32_t Returns the current targeted FSK bit polarity (1 for MARK, 0 for SPACE).
 */
static uint32_t dsp_rtty_sub_execute_tx_fsm(rtty_transmitter_t * const self)
{

	/* Index corresponds directly to ASCII value minus 32 (space offset control boundary) */
	static const uint8_t rtty_encode_letters [] = {
	    0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x03, 0x19, 0x0E, 0x09, 0x01, 0x0D, 0x1A, 0x14, 0x06, 0x0B, 0x0F, 0x12, 0x1C, 0x0C, 0x18,
	    0x16, 0x17, 0x0A, 0x05, 0x10, 0x07, 0x1E, 0x13, 0x1D, 0x15, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	static const uint8_t rtty_encode_figures [] = {
	    0x04, 0x00, 0x13, 0x00, 0x12, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x16, 0x0E, 0x01, 0x0C, 0x1A,
	    0x16, 0x17, 0x13, 0x01, 0x0A, 0x10, 0x15, 0x07, 0x06, 0x18, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x11,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

    uint32_t active_fsk_bit = 1;

    if (self->tx_active)
    {
        const uint32_t prev_acc = self->nco_baud_accumulator;
        self->nco_baud_accumulator += self->nco_baud_step;

        /* Check for integer NCO clock overflow condition (bit boundary window reached) */
        const int bit_tick_edge = (self->nco_baud_accumulator < prev_acc) ? 1 : 0;

        switch (self->tx_fsm_state)
        {
            case RTTY_TX_STATE_IDLE:
                uint8_t tx_char;
                /* Extract data character directly from the integrated tx_fifo field */
                if (fifo_pop(&self->tx_fifo, &tx_char))
                {
                    if (tx_char >= 32 && tx_char < 127)
                    {
                        const uint32_t lut_idx = tx_char - 32;
                        const uint32_t code_ltrs = rtty_encode_letters[lut_idx];
                        const uint32_t code_figs = rtty_encode_figures[lut_idx];

                        /* Case shifting management checks */
                        if (code_ltrs == 0x00 && code_figs > 0x00 && !self->is_figures_case)
                        {
                            self->bit_shifter = 0x1B; /* Force FIGS escape symbol */
                            self->request_figures = 1;
                            self->is_figures_case = 1;
                        }
                        else if (code_ltrs > 0x00 && code_figs == 0x00 && self->is_figures_case)
                        {
                            self->bit_shifter = 0x1F; /* Force LTRS escape symbol */
                            self->request_letters = 1;
                            self->is_figures_case = 0;
                        }
                        else
                        {
                            self->bit_shifter = self->is_figures_case ? code_figs : code_ltrs;
                        }

                        self->tx_fsm_state = RTTY_TX_STATE_START_BIT;
                        self->nco_baud_accumulator = 0;
                    }
                }
                else
                {
                    self->tx_active = 0; /* Queue empty, drop active transmission flag */
                }
                break;

            case RTTY_TX_STATE_START_BIT:
                active_fsk_bit = 0; /* START bit is strictly SPACE (0) */
                if (bit_tick_edge)
                {
                    self->tx_fsm_state = RTTY_TX_STATE_DATA_BITS;
                    self->bits_count = 0;
                }
                break;

            case RTTY_TX_STATE_DATA_BITS:
                /* Extract active serial bit stream starting from LSB position */
                active_fsk_bit = (self->bit_shifter >> self->bits_count) & 0x01;
                if (bit_tick_edge)
                {
                    self->bits_count++;
                    if (self->bits_count >= 5)
                    {
                        self->tx_fsm_state = RTTY_TX_STATE_STOP_BIT;
                    }
                }
                break;

            case RTTY_TX_STATE_STOP_BIT:
                active_fsk_bit = 1; /* STOP bit is strictly MARK (1) */
                if (bit_tick_edge)
                {
                    /* Frame serialization loop fulfilled, check if escape codes pending latch reset */
                    if (self->request_letters || self->request_figures)
                    {
                        /* Immediately reload the actual alphanumeric char that triggered escape */
                        self->request_letters = 0;
                        self->request_figures = 0;
                        self->tx_fsm_state = RTTY_TX_STATE_IDLE;
                        self->nco_baud_accumulator = 0xFFFFFFFF; /* Force evaluation on next sample tick */
                    }
                    else
                    {
                        self->tx_fsm_state = RTTY_TX_STATE_IDLE;
                    }
                }
                break;

            default:
                self->tx_fsm_state = RTTY_TX_STATE_IDLE;
                break;
        }
    }
    else
    {
        /* Idle mode tracking: scan dedicated FIFO container speed-throttled to sample steps */
        if (self->nco_baud_accumulator == 0)
        {
            if (fifo_get_count(&self->tx_fifo))
            {
                self->tx_active = 1;
                self->tx_fsm_state = RTTY_TX_STATE_IDLE;
            }
        }
        self->nco_baud_accumulator++;
        if (self->nco_baud_accumulator >= self->nco_baud_step)
        {
            self->nco_baud_accumulator = 0;
        }
    }

    return active_fsk_bit;
}

/**
 * @brief MAIN UNIFIED TRANSMITTER FUNCTION: Processes FSM bit tracking and modulates analytical IQ signals.
 */
static void dsp_rtty_tx_process_sample(
    rtty_transmitter_t * const self,
    FLOAT_t * const out_i,
    FLOAT_t * const out_q)
{
    /* Step 1: Run asynchronous framing serializer loop layer using the dedicated FIFO context */
    const uint32_t active_fsk_bit = dsp_rtty_sub_execute_tx_fsm(self);

    /* Step 2: Phase-continuous frequency shifting based on serial bit state polarity */
    const FLOAT_t instant_frequency_step = active_fsk_bit ? self->freq_shift_half_nco : -self->freq_shift_half_nco;

    self->phase_carrier += instant_frequency_step;
    if (self->phase_carrier >= (2.0 * M_PI)) self->phase_carrier -= (2.0 * M_PI);
    if (self->phase_carrier < 0.0)           self->phase_carrier += (2.0 * M_PI);

    /* Step 3: Fast complex IQ projections generation via hardware NEON SIMD registers */
    float32_t sin_val, cos_val;
    const float32_t phase_degrees = (float32_t)self->phase_carrier * (180.0f / (float32_t)M_PI);

    arm_sin_cos_f32(phase_degrees, &sin_val, &cos_val);

    *out_i = (FLOAT_t)cos_val * self->magnitude;
    *out_q = (FLOAT_t)sin_val * self->magnitude;
}


/**
 * @brief GLOBAL TX INITIALIZER: Prepares the transmitter context and flushes its integrated tx_fifo.
 */
static void dsp_rtty_tx_init(
    rtty_transmitter_t * const self,
    const uint32_t sample_rate,
    const FLOAT_t shift_hz,
    const FLOAT_t baud_rate,
    const FLOAT_t output_magnitude)
{
    self->tx_fsm_state = RTTY_TX_STATE_IDLE;
    self->nco_baud_accumulator = 0;
    self->bit_shifter = 0;
    self->bits_count = 0;
    self->is_figures_case = 0;
    self->tx_active = 0;
    self->phase_carrier = 0.0;
    self->magnitude = output_magnitude;
    self->request_letters = 0;
    self->request_figures = 0;
    self->invert_output = 0;
    self->freq_shift_half_nco = (2.0 * M_PI * (shift_hz / 2.0)) / (FLOAT_t)sample_rate;
    const FLOAT_t ratio = baud_rate / (FLOAT_t)sample_rate;
    self->nco_baud_step = (uint32_t)(ratio * 4294967296.0);

    /* Call your native firmware ring buffer initializer helper on the embedded field */
    fifo_init(&self->tx_fifo);
}

static void dsp_rtty_tx_set_reverse(
	rtty_transmitter_t * const self,
	const int invert)
{
    self->invert_output = invert ? 1 : 0;
}

//////////////
/// RX

/* Strictly bounded element-by-element configuration of ITA2 Baudot character matrices */

static const char rtty_ita2_letters [32] = {
    '\0', 'E', '\n', 'A', ' ', 'S', 'I', 'U',
    '\r', 'D', 'R', 'J', 'N', 'F', 'C', 'K',
    'T', 'Z', 'L', 'W', 'H', 'Y', 'P', 'Q',
    'O', 'B', 'G', ' ', 'M', 'X', 'V', ' ',
};

static const char rtty_ita2_figures [32] = {
    '\0', '3', '\n', '-', ' ', '\a', '8', '7',
    '\r', '$', '4', '\'', ',', '!', ':', '(',
    '5', '"', ')', '2', '#', '6', '0', '1',
    '9', '?', '&', ' ', '.', '/', ';', ' ',
};

/**
 * @brief SUB-INIT 1: Initializes the differential frequency detector sub-layer with optimized speed.
 * @brief SUB-INIT 1: Initializes the PLL/NCO frequency tracking detector sub-layer.
 */
static void dsp_rtty_sub_init_detector(
    rtty_freq_detector_t * const self,
    const uint32_t sample_rate,
    const FLOAT_t baud_rate)
{
    self->prev_in_i = 0.0;
    self->prev_in_q = 0.0;

    self->phase_nco = 0.0;
    self->pll_integrator = 0.0;
    self->lpf_state = 0.0;
    self->invert_output = 0; /* Default configuration is non-inverted (USB style) */

    /* Loop coefficients optimized for tight tracking of standard FSK shifts around Zero-IF */
    self->pll_kp = 0.06;
    self->pll_ki = 0.0015;

    const FLOAT_t samples_per_bit = (FLOAT_t)sample_rate / baud_rate;
    self->lpf_alphaNEW = 4.0 / samples_per_bit;
    // old version (differential frequency detector)
    self->lpf_alphaOLD = 8.0 / samples_per_bit;

}

/**
 * @brief SUB-INIT 2: Initializes the Baudot bit receiver FSM sub-layer.
 */
static void dsp_rtty_sub_init_fsm(
    rtty_baudot_fsm_t * const self,
    const uint32_t sample_rate,
    const FLOAT_t baud_rate)
{
    fifo_init(&self->rx_fifo);
    self->fsm_state = RTTY_STATE_IDLE;
    self->nco_accumulator = 0;
    self->bit_shifter = 0;
    self->bits_count = 0;
    self->is_figures_case = 0;

    const FLOAT_t ratio = baud_rate / (FLOAT_t)sample_rate;
    self->nco_step = (uint32_t)(ratio * 4294967296.0);
}

/**
 * @brief EXPORT EXTERNAL LAYER: Dynamically toggles RTTY spectrum inversion mode at runtime.
 */
static void dsp_rtty_rx_set_reverse(rtty_receiver_t * const self, const int invert)
{
    self->detector.invert_output = invert ? 1 : 0;
}

/**
 * @brief GLOBAL RX INITIALIZER: Prepares the RTTY processing core context.
 */
static void dsp_rtty_rx_init(rtty_receiver_t * const self, const uint32_t sample_rate, const FLOAT_t baud_rate)
{
    dsp_rtty_sub_init_detector(&self->detector, sample_rate, baud_rate);
    dsp_rtty_sub_init_fsm(&self->fsm, sample_rate, baud_rate);
}

/**
 * @brief SUB-FUNCTION 1: Differential cross-product frequency discriminator.
 * @return uint32_t Returns 1 for MARK (positive frequency), 0 for SPACE (negative frequency).
 */
static uint32_t dsp_rtty_sub_execute_discriminatorOLD(
	rtty_freq_detector_t * const self,
    const FLOAT_t in_i,
    const FLOAT_t in_q)
{
    /* Calculate instantaneous phase error cross-product */
    const FLOAT_t phase_error = in_q * self->prev_in_i - in_i * self->prev_in_q;

    self->prev_in_i = in_i;
    self->prev_in_q = in_q;

    /* Low-pass envelope integration */
    self->lpf_state += self->lpf_alphaOLD * (phase_error - self->lpf_state);

    /* Slicing boundary execution */
    const uint32_t raw_bit = (self->lpf_state >= 0.0) ? 1 : 0;

    /* Apply fast hardware-friendly inversion layer using native XOR operation with typecast */
    return raw_bit ^ (uint32_t)self->invert_output;
}
/**
 * @brief SUB-FUNCTION 1: Phase Locked Loop (PLL) frequency tracker with NCO de-rotation.
 * @return uint32_t Returns the final sliced bit (inverted or non-inverted based on internal flag).
 */
static uint32_t dsp_rtty_sub_execute_discriminatorNEW(
    rtty_freq_detector_t * const self,
    const FLOAT_t in_i,
    const FLOAT_t in_q)
{
    float32_t sin_val, cos_val;

    /* Convert current local tracking phase accumulator from radians to degrees for CMSIS core */
    const float32_t phase_degrees = (float32_t)self->phase_nco * (180.0f / (float32_t)M_PI);

    /* High-speed hardware core trigonometry execution via NEON SIMD registers */
    arm_sin_cos_f32(phase_degrees, &sin_val, &cos_val);

    const FLOAT_t local_i = (FLOAT_t)cos_val;
    const FLOAT_t local_q = (FLOAT_t)sin_val;

    /* Complex multiplier phase error discriminator: phase_error = Im(V_in * V_local^*) */
    const FLOAT_t phase_error = in_q * local_i - in_i * local_q;

    /* Update loop integrator via integral gain factor */
    self->pll_integrator += phase_error * self->pll_ki;

    /* Compute next instantaneous NCO phase step based on proportional and integral error components */
    const FLOAT_t current_step = (phase_error * self->pll_kp) + self->pll_integrator;

    /* Step local NCO phase accumulator with strict wrap-around */
    self->phase_nco += current_step;
    if (self->phase_nco >= (2.0 * M_PI)) self->phase_nco -= (2.0 * M_PI);
    if (self->phase_nco < 0.0)           self->phase_nco += (2.0 * M_PI);

    /* Smooth the stable loop integrator output (frequency deviation) to get clean bit envelope */
    self->lpf_state += self->lpf_alphaNEW * (self->pll_integrator - self->lpf_state);

    /* Slicing boundary: Positive tracked frequency vs Negative tracked frequency */
    const uint32_t raw_bit = (self->lpf_state >= 0.0) ? 1 : 0;

    /* Execute rapid hardware-friendly inversion layer using native XOR operation */
    return raw_bit ^ (uint32_t)self->invert_output;
}

/**
 * @brief SUB-FUNCTION 2: Asynchronous Baudot bit receiver driven by enum-typed FSM.
 */
static void dsp_rtty_sub_execute_fsm(
    rtty_baudot_fsm_t * const self,
    const uint32_t raw_bit,
    void (* const put_char_cb)(rtty_baudot_fsm_t * self, const uint8_t character))
{
    switch (self->fsm_state)
    {
        case RTTY_STATE_IDLE:
            if (raw_bit == 0)
            {
                self->fsm_state = RTTY_STATE_START_BIT;
                self->nco_accumulator = 0x80000000;
            }
            break;

        case RTTY_STATE_START_BIT:
            /* Accumulate fixed integer step continuously */
            self->nco_accumulator += self->nco_step;

            /* Verify if integer overflow occurred (Phase register wrapped around) */
            if (self->nco_accumulator < self->nco_step)
            {
                /* Verify that start bit remains valid SPACE (0) at its midpoint boundary marker */
                if (raw_bit == 0)
                {
                    self->fsm_state = RTTY_STATE_DATA_BITS;
                    self->bits_count = 0;
                    self->bit_shifter = 0;

                    /* --- СКОРРЕКТИРОВАНО: НЕ ОБНУЛЯЕМ АККУМУЛЯТОР! --- */
                    /* Сохраняем накопленную фазу переполнения, чтобы сетка не прыгала */
                }
                else
                {
                    self->fsm_state = RTTY_STATE_IDLE;
                }
            }
            break;

        case RTTY_STATE_DATA_BITS:
            self->nco_accumulator += self->nco_step;

            if (self->nco_accumulator < self->nco_step)
            {
                self->bit_shifter |= (raw_bit << self->bits_count);
                self->bits_count++;

                if (self->bits_count >= 5)
                {
                    self->fsm_state = RTTY_STATE_STOP_BIT;
                    self->nco_accumulator = 0;

                    const uint32_t raw_code = self->bit_shifter & 0x1F;
#if 0
                    static const char hex [] = "0123456789ABCDEF";
                    put_char_cb(self, hex [(raw_code >> 4) & 0x0F]);
                    put_char_cb(self, hex [(raw_code >> 0) & 0x0F]);
                    put_char_cb(self, ' ');
#else
                    if (raw_code == 0x1F)
                    {
                        self->is_figures_case = 0; /* LETTERS shift escape received */
                    }
                    else if (raw_code == 0x1B)
                    {
                        self->is_figures_case = 1; /* FIGURES shift escape received */
                    }
                    else if (raw_code > 0x00)
                    {
                        const uint8_t ascii_char = self->is_figures_case ?
                                                    rtty_ita2_figures[raw_code ] :
                                                    rtty_ita2_letters[raw_code];

                       if (ascii_char)
                    	   put_char_cb(self, ascii_char);
                    }
#endif
                }
            }
            break;

        case RTTY_STATE_STOP_BIT:
            self->nco_accumulator += self->nco_step;

            if (self->nco_accumulator < self->nco_step)
            {
                self->fsm_state = RTTY_STATE_IDLE;
            }
            break;

        default:
            self->fsm_state = RTTY_STATE_IDLE;
            break;
    }
}

/**
 * @brief MAIN UNIFIED WRAPPER FUNCTION: Pipe execution loop core.
 */
static void dsp_rtty_rx_process_sample(
    rtty_receiver_t * const self,
    const FLOAT_t in_i,
    const FLOAT_t in_q,
    void (* const put_char_cb)(rtty_baudot_fsm_t * self, const uint8_t character))
{
    const uint32_t raw_bit = dsp_rtty_sub_execute_discriminatorOLD(&self->detector, in_i, in_q);
    dsp_rtty_sub_execute_fsm(&self->fsm, raw_bit, put_char_cb);
}

//////////////
/**
 * @brief Extracts a successfully decoded text byte from the receiver queue to send to USB.
 * @return uint32_t Returns 1 if a byte is available, 0 if queue is empty.
 */
static uint32_t rtty_rx_byte(rtty_receiver_t * const self, uint8_t *output_byte)
{
    return fifo_pop(&self->fsm.rx_fifo, output_byte);
}

static void rxcharacter(rtty_baudot_fsm_t * self, const uint8_t c)
{
	fifo_push(&self->rx_fifo, c);
}

static rtty_receiver_t rx_stream;
static rtty_transmitter_t tx_stream;

static void rtty_spool(void * ctx)
{
	(void) ctx;

	uint8_t c;
	if (rtty_rx_byte(& rx_stream, & c))
	{
		display_vtty_putchar(c);
		//dbg_putchar(c);
	}
}

void RTTYModem_SetParam(int_fast32_t RTTY_Speed10, int_fast32_t RTTY_Shift, int invert_output)
{
	dsp_rtty_rx_init(& rx_stream, ARMSAIRATE, RTTY_Speed10 / (FLOAT_t) 10);
	dsp_rtty_rx_set_reverse(& rx_stream, invert_output);

	dsp_rtty_tx_init(& tx_stream, ARMSAIRATE, RTTY_Shift, RTTY_Speed10 / (FLOAT_t) 10, 1);
	dsp_rtty_tx_set_reverse(& tx_stream, invert_output);
}

void RTTY_SampleRX(uint_fast8_t pathi, FLOAT_t i, FLOAT_t q)
{
	if (pathi == 0)
		dsp_rtty_rx_process_sample(& rx_stream, i, q, rxcharacter);
}

void RTTY_SampleTX(FLOAT_t * i, FLOAT_t * q)
{
	dsp_rtty_tx_process_sample(& tx_stream, i, q);
}

// Передача символа
void RTTY_TX(uint8_t c)
{
	fifo_push(& tx_stream.tx_fifo, c);
}

void RTTYModem_Init(void)
{
	static dpcobj_t dpcobj;

	dsp_rtty_rx_init(& rx_stream, ARMSAIRATE, 50);
	dsp_rtty_rx_set_reverse(& rx_stream, 1);

	dpcobj_initialize(& dpcobj, rtty_spool, NULL);
	board_dpc_addentry(& dpcobj, board_dpc_coreid());
}

#endif /* WITHINTEGRATEDDSP */


#if WITHRTTY && WITHIF4DSP && 0


#define BIQUAD_COEFF_IN_STAGE 5													  // coefficients space, mark and LPF filters

#define RTTY_LPF_STAGES 2
#define RTTY_BPF_STAGES 2
#define RTTY_BPF_WIDTH (RTTY_Shift / 4)	// мне кажется это должна быть функция от скорости передачи

#define RTTY_SYMBOL_CODE 0x1B	//(0b11011)
#define RTTY_LETTER_CODE 0x1F	//(0b11111)

typedef enum {
	RTTY_STATE_WAIT_START,
	RTTY_STATE_BIT,
} rtty_state_t;

typedef enum {
	RTTY_MODE_LETTERS,
	RTTY_MODE_SYMBOLS
} rtty_charSetMode_t;

typedef enum {
    RTTY_STOP_1,
    RTTY_STOP_1_5,
    RTTY_STOP_2
} rtty_stopbits_t;

typedef struct
{
	uint16_t oneBitSampleCount;
	int32_t DPLLBitPhase;
	int32_t DPLLOldVal;
	rtty_state_t state;// = RTTY_STATE_WAIT_START;
	rtty_charSetMode_t charSetMode;// = RTTY_MODE_LETTERS;
	uint8_t byteResult;// = 0;
	uint16_t byteResult_bnum;// = 0;
	int stopBits;// = RTTY_STOP_1;
	//lpf
	FLOAT_t LPF_Filter_Coeffs[BIQUAD_COEFF_IN_STAGE * RTTY_LPF_STAGES];
	FLOAT_t LPF_Filter_State[2 * RTTY_LPF_STAGES];
	ARM_MORPH(arm_biquad_cascade_df2T_instance) RTTY_LPF_Filter;

	//mark
	FLOAT_t mark_Filter_Coeffs[BIQUAD_COEFF_IN_STAGE * RTTY_BPF_STAGES];
	FLOAT_t mark_Filter_State[2 * RTTY_BPF_STAGES];
	ARM_MORPH(arm_biquad_cascade_df2T_instance) mark_Filter;

	//space
	FLOAT_t space_Filter_Coeffs[BIQUAD_COEFF_IN_STAGE * RTTY_BPF_STAGES];
	FLOAT_t space_Filter_State[2 * RTTY_BPF_STAGES];
	ARM_MORPH(arm_biquad_cascade_df2T_instance) space_Filter;
    int invert_output;           /* Boolean flag to invert the discriminator bit output (0 or 1) */

    rtty_baudot_fsm_t    fsm;      /* Embedded integer NCO asynchronous framing engine */

} rtty_rx_t;
// Public variables
//extern char RTTY_Decoder_Text[RTTY_DECODER_STRLEN + 1];

//Ported from https://github.com/df8oe/UHSDR/blob/active-devel/mchf-eclipse/drivers/audio/rtty.c


// FSK shift: 170 200 425 850
// FSK tone freq 1275 2125

// The standard mark and space tones are 2125 hz and 2295 hz respectively
//#define	DEFAULT_RTTY_PITCH	1275	/* mark тон DIGI modes - 2.125 кГц (1275 2125) */

// TTY: 10100.550
// TTY: 10100.600
// DDK2 DDK7 DDK9 10100.8 KHZ - Центральная частота (между пиками), 450 Hz shift, 50 baud
// peaks: mark: 10101.025, space: 10100.575
//
// Kenwood:
// The standard mark and space tones are 2125 hz and 2295 hz respectively
//#define RTTY_FreqMark DEFAULT_RTTY_PITCH		// /* mark тон DIGI modes - 2.125 кГц (1275 2125) */

static rtty_rx_t rtty0;

void RTTYModem_SetParam(int_fast32_t centerFreq, int_fast32_t RTTY_Speed10, int_fast32_t RTTY_Shift, int invert_output)
{
	rtty_rx_t * const self = & rtty0;
	const int_fast32_t sample_rate = ARMI2SRATE;
	// The standard mark and space tones are 2125 hz and 2295 hz respectively
	const int_fast32_t RTTY_FreqMark = (centerFreq - RTTY_Shift / 2);		// /* mark тон DIGI modes - 2.125 кГц (1275 2125) */
	const int_fast32_t RTTY_FreqSpace = (centerFreq + RTTY_Shift / 2);

	self->DPLLBitPhase = 0;
	self->DPLLOldVal = 0;

	iir_filter_t f0;
	//speed
	self->oneBitSampleCount = ROUNDF((FLOAT_t) sample_rate * 10 / RTTY_Speed10);

	//RTTY LPF Filter
	biquad_create(& f0, RTTY_LPF_STAGES);
	biquad_init_lowpass(& f0, sample_rate, RTTY_Speed10 * 2 / 10);
	fill_biquad_coeffs(& f0, self->LPF_Filter_Coeffs);
    ARM_MORPH(arm_fill)(0, self->LPF_Filter_State, ARRAY_SIZE(self->LPF_Filter_State));
    ARM_MORPH(arm_biquad_cascade_df2T_init)(&self->RTTY_LPF_Filter, RTTY_LPF_STAGES, self->LPF_Filter_Coeffs, self->LPF_Filter_State);

	//RTTY mark filter
	biquad_create(& f0, RTTY_BPF_STAGES);
	biquad_init_bandpass(& f0, sample_rate, RTTY_FreqMark - RTTY_BPF_WIDTH / 2, RTTY_FreqMark + RTTY_BPF_WIDTH / 2);
	fill_biquad_coeffs(& f0, self->mark_Filter_Coeffs);
    ARM_MORPH(arm_fill)(0, self->mark_Filter_State, ARRAY_SIZE(self->mark_Filter_State));
	ARM_MORPH(arm_biquad_cascade_df2T_init)(&self->mark_Filter, RTTY_BPF_STAGES, self->mark_Filter_Coeffs, self->mark_Filter_State);

	//RTTY space filter
	biquad_create(& f0, RTTY_BPF_STAGES);
	biquad_init_bandpass(& f0, sample_rate, RTTY_FreqSpace - RTTY_BPF_WIDTH / 2, RTTY_FreqSpace + RTTY_BPF_WIDTH / 2);
	fill_biquad_coeffs(& f0, self->space_Filter_Coeffs);
    ARM_MORPH(arm_fill)(0, self->space_Filter_State, ARRAY_SIZE(self->space_Filter_State));
	ARM_MORPH(arm_biquad_cascade_df2T_init)(&self->space_Filter, RTTY_BPF_STAGES, self->space_Filter_Coeffs, self->space_Filter_State);

	self->state = RTTY_STATE_WAIT_START;
	self->charSetMode = RTTY_MODE_LETTERS;
	self->byteResult = 0;
	self->byteResult_bnum = 0;
	self->stopBits = RTTY_STOP_1;

	self->invert_output = invert_output;

    /* Initialize asynchronous integer NCO bit framing receiver sub-layer pointer */
	int_fast32_t baud_rate = RTTY_Speed10 / 10;
    dsp_rtty_sub_init_fsm(&self->fsm, sample_rate, baud_rate);
}

// adapted from https://github.com/ukhas/dl-fldigi/blob/master/src/include/misc.h
static FLOAT_t RTTYDecoder_decayavg(rtty_rx_t * self, FLOAT_t average, FLOAT_t input, int weight)
{
	FLOAT_t retval;
	if (weight <= 1)
	{
		retval = input;
	}
	else
	{
		retval = ((input - average) / (FLOAT_t)weight) + average;
	}
	return retval;
}

// this function returns the bit value of the current sample
static int RTTYDecoder_demodulator(rtty_rx_t * self, FLOAT_t sample)
{
	FLOAT_t space_mag = 0;
	FLOAT_t mark_mag = 0;
	ARM_MORPH(arm_biquad_cascade_df2T)(&self->space_Filter, &sample, &space_mag, 1);
	ARM_MORPH(arm_biquad_cascade_df2T)(&self->mark_Filter, &sample, &mark_mag, 1);

	FLOAT_t v1 = 0;
	// calculating the RMS of the two lines (squaring them)
	space_mag *= space_mag;
	mark_mag *= mark_mag;

	// RTTY decoding with ATC = automatic threshold correction
	FLOAT_t helper = space_mag;
	space_mag = mark_mag;
	mark_mag = helper;
	static FLOAT_t mark_env = 0.0;
	static FLOAT_t space_env = 0.0;
	static FLOAT_t mark_noise = 0.0;
	static FLOAT_t space_noise = 0.0;
	// experiment to implement an ATC (Automatic threshold correction), DD4WH, 2017_08_24
	// everything taken from FlDigi, licensed by GNU GPLv2 or later
	// https://github.com/ukhas/dl-fldigi/blob/master/src/cw_rtty/rtty.cxx
	// calculate envelope of the mark and space signals
	// uses fast attack and slow decay
	mark_env = RTTYDecoder_decayavg(self, mark_env, mark_mag, (mark_mag > mark_env) ? self->oneBitSampleCount / 4 : self->oneBitSampleCount * 16);
	space_env = RTTYDecoder_decayavg(self, space_env, space_mag, (space_mag > space_env) ? self->oneBitSampleCount / 4 : self->oneBitSampleCount * 16);
	// calculate the noise on the mark and space signals
	mark_noise = RTTYDecoder_decayavg(self, mark_noise, mark_mag, (mark_mag < mark_noise) ? self->oneBitSampleCount / 4 : self->oneBitSampleCount * 48);
	space_noise = RTTYDecoder_decayavg(self, space_noise, space_mag, (space_mag < space_noise) ? self->oneBitSampleCount / 4 : self->oneBitSampleCount * 48);
	// the noise floor is the lower signal of space and mark noise
	FLOAT_t noise_floor = (space_noise < mark_noise) ? space_noise : mark_noise;

	// Linear ATC, section 3 of www.w7ay.net/site/Technical/ATC
	// v1 = space_mag - mark_mag - 0.5 * (space_env - mark_env);

	// Compensating for the noise floor by using clipping
	FLOAT_t mclipped = 0, sclipped = 0;
	mclipped = mark_mag > mark_env ? mark_env : mark_mag;
	sclipped = space_mag > space_env ? space_env : space_mag;
	if (mclipped < noise_floor)
	{
		mclipped = noise_floor;
	}
	if (sclipped < noise_floor)
	{
		sclipped = noise_floor;
	}

	// Optimal ATC (Section 6 of of www.w7ay.net/site/Technical/ATC)
	v1 = (mclipped - noise_floor) * (mark_env - noise_floor) - (sclipped - noise_floor) * (space_env - noise_floor) - 0.25 * ((mark_env - noise_floor) * (mark_env - noise_floor) - (space_env - noise_floor) * (space_env - noise_floor));
	ARM_MORPH(arm_biquad_cascade_df2T)(&self->RTTY_LPF_Filter, &v1, &v1, 1);

	// RTTY without ATC, which works very well too!
	// inverting line 1
	/*
	 mark_mag *= -1;

	// summing the two lines
	v1 = mark_mag + space_mag;

	// lowpass filtering the summed line
	arm_biquad_cascade_df2T_f32(&RTTY_LPF_Filter, &v1, &v1, 1);
	*/

	return (v1 > 0) ? ! self->invert_output : self->invert_output;
}

// this function returns only 1 when the start bit is successfully received
static int RTTYDecoder_waitForStartBit(rtty_rx_t * self, FLOAT_t sample)
{
	int retval = 0;
	int bitResult;
	static int16_t wait_for_start_state = 0;
	static int16_t wait_for_half = 0;

	bitResult = RTTYDecoder_demodulator(self, sample);

	switch (wait_for_start_state)
	{
	case 0:
		// waiting for a falling edge
		if (bitResult != 0)
		{
			wait_for_start_state++;
		}
		break;
	case 1:
		if (bitResult != 1)
		{
			wait_for_start_state++;
		}
		break;
	case 2:
		wait_for_half = self->oneBitSampleCount / 2;
		wait_for_start_state++;
		/* no break */
	case 3:
		wait_for_half--;
		if (wait_for_half == 0)
		{
			retval = (bitResult == 0);
			wait_for_start_state = 0;
		}
		break;
	}
	return retval;
}

// this function returns 1 once at the half of a bit with the bit's value
static int RTTYDecoder_getBitDPLL(rtty_rx_t * self, FLOAT_t sample, int *val_p)
{
	static int phaseChanged = 0;
	int retval = 0;

	if (self->DPLLBitPhase < self->oneBitSampleCount)
	{
		*val_p = RTTYDecoder_demodulator(self, sample);

		if (!phaseChanged && *val_p != self->DPLLOldVal)
		{
			if (self->DPLLBitPhase < self->oneBitSampleCount / 2)
			{
				self->DPLLBitPhase += self->oneBitSampleCount / 32; // early
			}
			else
			{
				self->DPLLBitPhase -= self->oneBitSampleCount / 32; // late
			}
			phaseChanged = 1;
		}
		self->DPLLOldVal = *val_p;
		self->DPLLBitPhase++;
	}

	if (self->DPLLBitPhase >= self->oneBitSampleCount)
	{
		self->DPLLBitPhase -= self->oneBitSampleCount;
		retval = 1;
	}

	return retval;
}

static void RTTYDecoder_Process2(
		rtty_rx_t * self,
		const FLOAT_t *bufferIn,
		unsigned len,
	    void (* const put_char_cb)(const uint8_t character)
		)
{
	for (uint32_t buf_pos = 0; buf_pos < len; buf_pos++)
	{
		switch (self->state)
		{
		case RTTY_STATE_WAIT_START: // not synchronized, need to wait for start bit
			if (RTTYDecoder_waitForStartBit(self, bufferIn[buf_pos]))
			{
				self->state = RTTY_STATE_BIT;
				self->byteResult_bnum = 1;
				self->byteResult = 0;
			}
			break;
		case RTTY_STATE_BIT:
			// reading 7 more bits
			if (self->byteResult_bnum < 8)
			{
				int bitResult = 0;
				if (RTTYDecoder_getBitDPLL(self, bufferIn[buf_pos], &bitResult))
				{
					switch (self->byteResult_bnum)
					{
					case 6: // stop bit 1
					case 7: // stop bit 2
						if (bitResult == 0)
						{
							// not in sync
							self->state = RTTY_STATE_WAIT_START;
						}
						if (self->stopBits != RTTY_STOP_2 && self->byteResult_bnum == 6)
						{
							// we pretend to be at the 7th bit after receiving the first stop bit if we have less than 2 stop bits
							// this omits check for 1.5 bit condition but we should be more or less safe here, may cause
							// a little more unaligned receive but without that shortcut we simply cannot receive these configurations
							// so it is worth it
							self->byteResult_bnum = 7;
						}
						break;
					default:
						self->byteResult |= !! bitResult << (self->byteResult_bnum - 1);
					}
					self->byteResult_bnum++;
				}
			}
			if (self->byteResult_bnum == 8 && self->state == RTTY_STATE_BIT)
			{

				switch (self->byteResult)
				{
				case RTTY_LETTER_CODE:
					self->charSetMode = RTTY_MODE_LETTERS;
					// println(" ^L^");
					break;
				case RTTY_SYMBOL_CODE:
					self->charSetMode = RTTY_MODE_SYMBOLS;
					// println(" ^F^");
					break;
				case 0x00:
					break;
				default:
					if (self->byteResult < 0x1F)
					{
						char charResult;
						switch (self->charSetMode)
						{
						case RTTY_MODE_SYMBOLS:
							charResult = rtty_ita2_figures [self->byteResult];
							break;
						case RTTY_MODE_LETTERS:
						default:
							charResult = rtty_ita2_letters [self->byteResult];
							break;
						}
						//RESULT !!!!
						put_char_cb(charResult);
					}
					break;
				}
				self->state = RTTY_STATE_WAIT_START;
			}
		}
	}
}

////////////////////////
///

static void put_char_null(const uint8_t character)
{
}

static void put_char_vtty(const uint8_t character)
{
	//print(character);
	//PRINTF("%c", charResult);
	//display_vtty_printf("%c", charResult);
	display_vtty_putchar(character);
//					char str[2] = {0};
//					str[0] = character;
//					if (strlen(RTTY_Decoder_Text) >= RTTY_DECODER_STRLEN)
//						shiftTextLeft(RTTY_Decoder_Text, 1);
//					strcat(RTTY_Decoder_Text, str);
//					LCD_UpdateQuery.TextBar = 1;
}

void RTTYDecoder_Process(const FLOAT_t *bufferIn, unsigned len) // start RTTY decoder for the data block
{
	rtty_rx_t * const self = & rtty0;
	RTTYDecoder_Process2(self, bufferIn, len, put_char_null);
}

#endif /* WITHRTTY */

