#include "hardware.h"

#if WITHINTEGRATEDDSP && 1

//////////////////
/// interleaver

/*
 * OFDM Bit Packer / Unpacker with Matrix Interleaver and FEC (7, 4) Hamming Code
 * PART 1 OF 3: Headers, Isolated Context Structures and Ring Buffer FIFO Queues.
 * Fully decoupled structures ensuring reentrancy compliant with hftrx architecture.
 */

#include "hardware.h"

#if WITHINTEGRATEDDSP

#include "dspdefines.h"

#define MODEM_FIFO_SIZE     256
#define INTERLEAVE_ROWS     8   /* Matches OFDM_NUM_CHANNELS */
#define INTERLEAVE_COLS     8   /* Depth of time interleaving */
#define INTERLEAVE_SIZE     (INTERLEAVE_ROWS * INTERLEAVE_COLS) /* 64 bits = 8 bytes */

/* Simple FIFO/Ring Buffer structure for USB stream interfacing */
typedef struct {
    uint8_t storage[MODEM_FIFO_SIZE];
    volatile uint32_t head;
    volatile uint32_t tail;
    volatile uint32_t count;
} modem_fifo_t;

/* Independent Transmitter Packer/Interleaver/FEC Context */
typedef struct {
    modem_fifo_t tx_fifo;
    uint8_t tx_matrix[INTERLEAVE_ROWS][INTERLEAVE_COLS];
    uint32_t tx_col_idx;
} ofdm_packer_tx_t;

/* Independent Receiver Unpacker/Deinterleaver/FEC Context */
typedef struct {
    modem_fifo_t rx_fifo;
    uint8_t rx_matrix[INTERLEAVE_ROWS][INTERLEAVE_COLS];
    uint32_t rx_col_idx;
} ofdm_packer_rx_t;

/* ========================================================================== */
/*                             INTERNAL FIFO HELPERS                          */
/* ========================================================================== */

static void fifo_init(modem_fifo_t *fifo)
{
    fifo->head = 0;
    fifo->tail = 0;
    fifo->count = 0;
}

static uint32_t fifo_push(modem_fifo_t *fifo, uint8_t data)
{
    if (fifo->count >= MODEM_FIFO_SIZE) {
        return 0; /* FIFO Full allocation error */
    }
    fifo->storage[fifo->head] = data;
    fifo->head = (fifo->head + 1) % MODEM_FIFO_SIZE;
    fifo->count++;
    return 1;
}

static uint32_t fifo_pop(modem_fifo_t *fifo, uint8_t *data)
{
    if (fifo->count == 0) {
        return 0; /* FIFO Empty condition */
    }
    *data = fifo->storage[fifo->tail];
    fifo->tail = (fifo->tail + 1) % MODEM_FIFO_SIZE;
    fifo->count--;
    return 1;
}
/*
 * OFDM Bit Packer / Unpacker with Matrix Interleaver and FEC (7, 4) Hamming Code
 * PART 2 OF 3: Hamming (7, 4) FEC Engine and Transmitter (TX) API.
 * Uses strict bitwise operations and loop mappings into the interleaver grid.
 */

/* ========================================================================== */
/*                         HAMMING (7, 4) FEC CORE ENGINE                     */
/* ========================================================================== */

/**
 * @brief Encodes 4 bits of data into a 7-bit Hamming codeword.
 *        Data bits mapped to positions: 3, 5, 6, 7. Parity bits: 1, 2, 4.
 * @param nibble Input 4-bit data (lower nibble).
 * @return uint8_t Encoded 7-bit codeword.
 */
static uint8_t hamming_74_encode(uint8_t nibble)
{
    uint8_t d1 = (nibble >> 0) & 1;
    uint8_t d2 = (nibble >> 1) & 1;
    uint8_t d3 = (nibble >> 2) & 1;
    uint8_t d4 = (nibble >> 3) & 1;

    /* Calculate parity bits using XOR */
    uint8_t p1 = d1 ^ d2       ^ d4;
    uint8_t p2 = d1      ^ d3  ^ d4;
    uint8_t p3 =      d2 ^ d3  ^ d4;

    /* Construct 7-bit codeword: [p1 p2 d1 p3 d2 d3 d4] */
    return (p1 << 6) | (p2 << 5) | (d1 << 4) | (p3 << 3) | (d2 << 2) | (d3 << 1) | d4;
}

/**
 * @brief Decodes a 7-bit Hamming codeword and fixes single-bit errors.
 * @param codeword Input received 7-bit codeword.
 * @return uint8_t Decoded and corrected 4-bit data payload.
 */
static uint8_t hamming_74_decode(uint8_t codeword)
{
    uint8_t p1 = (codeword >> 6) & 1;
    uint8_t p2 = (codeword >> 5) & 1;
    uint8_t d1 = (codeword >> 4) & 1;
    uint8_t p3 = (codeword >> 3) & 1;
    uint8_t d2 = (codeword >> 2) & 1;
    uint8_t d3 = (codeword >> 1) & 1;
    uint8_t d4 = (codeword >> 0) & 1;

    /* Compute syndrome vector bits */
    uint8_t s1 = p1 ^ d1 ^ d2      ^ d4;
    uint8_t s2 = p2 ^ d1      ^ d3 ^ d4;
    uint8_t s3 = p3      ^ d2 ^ d3 ^ d4;

    uint8_t syndrome = (s1 << 2) | (s2 << 1) | s3;

    /* Error correction lookup based on syndrome value */
    if (syndrome != 0)
    {
        /* Invert the corrupted bit matching the specific error syndrome position */
        switch (syndrome) {
            case 7: d4 ^= 1; break; /* Error in d4 */
            case 6: d1 ^= 1; break; /* Error in d1 */
            case 5: d2 ^= 1; break; /* Error in d2 */
            case 3: d3 ^= 1; break; /* Error in d3 */
            default: break;         /* Parity bit errors can be ignored for data extraction */
        }
    }

    /* Return reconstructed corrected 4-bit data */
    return (d4 << 3) | (d3 << 2) | (d2 << 1) | d1;
}

/* ========================================================================== */
/*                          TRANSMITTER (TX) API                              */
/* ========================================================================== */

/**
 * @brief Runtime initialization of the standalone OFDM transmitter packer context.
 */
static void ofdm_packer_tx_init(ofdm_packer_tx_t *self)
{
    fifo_init(&self->tx_fifo);
    self->tx_col_idx = 0;
    for (uint32_t r = 0; r < INTERLEAVE_ROWS; r++) {
        for (uint32_t c = 0; c < INTERLEAVE_COLS; c++) {
            self->tx_matrix[r][c] = 0;
        }
    }
}

/**
 * @brief Forces a hard reset and flushes internal transmitter queues and matrices.
 */
static void ofdm_packer_tx_reset(ofdm_packer_tx_t *self)
{
    fifo_init(&self->tx_fifo);
    self->tx_col_idx = 0;
}

/**
 * @brief Injects a raw text byte received from USB CDC into the isolated transmitter queue.
 */
static void ofdm_packer_put_tx_byte(ofdm_packer_tx_t *self, uint8_t byte)
{
    fifo_push(&self->tx_fifo, byte);
}

static void ofdm_packer_get_bits_callback(ofdm_packer_tx_t * self, uint8_t *bits)
{
    uint8_t tx_byte = 0;

    /* Извлекаем 1 байт из FIFO USB. Если пусто — гоним нули (Idle) */
    if (fifo_pop(&self->tx_fifo, &tx_byte))
    {
        /* Нарезаем байт на 8 поднесущих. Порядок бит должен строго соответствовать приемнику! */
        bits[0] = (tx_byte >> 0) & 1;
        bits[1] = (tx_byte >> 1) & 1;
        bits[2] = (tx_byte >> 2) & 1;
        bits[3] = (tx_byte >> 3) & 1;
        bits[4] = (tx_byte >> 4) & 1;
        bits[5] = (tx_byte >> 5) & 1;
        bits[6] = (tx_byte >> 6) & 1;
        bits[7] = (tx_byte >> 7) & 1;
    }
    else
    {
        for (uint32_t ch = 0; ch < 8; ch++) {
            bits[ch] = 0;
        }
    }
}

/*
 * OFDM Bit Packer / Unpacker with Matrix Interleaver and FEC (7, 4) Hamming Code
 * PART 3 OF 3: Receiver (RX) API and Deinterleaver Engine.
 * Fills Deinterleaver Matrix vertically and decodes Hamming FEC horizontally.
 */

/* ========================================================================== */
/*                            RECEIVER (RX) API                               */
/* ========================================================================== */

/**
 * @brief Runtime initialization of the standalone OFDM receiver unpacker context.
 */
static void ofdm_packer_rx_init(ofdm_packer_rx_t *self)
{
    fifo_init(&self->rx_fifo);
    self->rx_col_idx = 0;
    for (uint32_t r = 0; r < INTERLEAVE_ROWS; r++) {
        for (uint32_t c = 0; c < INTERLEAVE_COLS; c++) {
            self->rx_matrix[r][c] = 0;
        }
    }
}

/**
 * @brief Forces a hard reset and flushes internal receiver tracking queues and deinterleavers.
 */
static void ofdm_packer_rx_reset(ofdm_packer_rx_t *self)
{
    fifo_init(&self->rx_fifo);
    self->rx_col_idx = 0;
}

/**
 * @brief Extracts a successfully decoded text byte from the receiver queue to send to USB.
 * @return uint32_t Returns 1 if a byte is available, 0 if queue is empty.
 */
static uint32_t ofdm_packer_get_rx_byte(ofdm_packer_rx_t *self, uint8_t *output_byte)
{
    return fifo_pop(&self->rx_fifo, output_byte);
}

static void ofdm_packer_process_bits_callback(ofdm_packer_rx_t * self, const uint8_t *bits)
{
    uint8_t rx_byte = 0;

    /* Собираем байт обратно из 8 ортогональных каналов */
    rx_byte |= (bits[0] & 1) << 0;
    rx_byte |= (bits[1] & 1) << 1;
    rx_byte |= (bits[2] & 1) << 2;
    rx_byte |= (bits[3] & 1) << 3;
    rx_byte |= (bits[4] & 1) << 4;
    rx_byte |= (bits[5] & 1) << 5;
    rx_byte |= (bits[6] & 1) << 6;
    rx_byte |= (bits[7] & 1) << 7;

    /* Отправляем чистый байт в буфер USB */
    fifo_push(&self->rx_fifo, rx_byte);
}

#endif /* WITHINTEGRATEDDSP */

#include "dspdefines.h"
#include "audio.h"
#include "buffers.h"
#include "formats.h"

#define OFDM_NUM_CHANNELS   8
#define FFT_LEN             256
#define CYCLIC_PREFIX_LEN   32    /* Increased from 16 to 32 for phase alignment */
#define OFDM_SYMBOL_LEN     (FFT_LEN + CYCLIC_PREFIX_LEN) /* 160 samples */

#define TX_W_LEN   	8
#define RX_W_LEN 	8

#define SYNC_HALF_LEN   128  /* FFT_LEN / 2 */

typedef enum {
    STATE_SEARCHING_PREAMBLE = 0,
    STATE_PROCESSING_DATA
} sync_state_t;

/*
 * Symmetric Subcarrier Map for Quadrature Up-Converter:
 * Bins 1..4   -> Positive frequencies (USB): +375, +750, +1125, +1500 Hz
 * Bins 124..127 -> Negative frequencies (LSB): -1500, -1125, -750, -375 Hz
 */
static const uint16_t subcarrier_map[OFDM_NUM_CHANNELS] = {
    1, 3, 5, 7,        /* Positive bins (Channels 0, 1, 2, 3) */
	FFT_LEN - 7, FFT_LEN - 5, FFT_LEN - 3, FFT_LEN - 1  /* Negative bins (Channels 4, 5, 6, 7) */
};

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

typedef struct {
    ARM_MORPH(arm_cfft_instance) cfft_inst;

    FLOAT_t fft_buffer[FFT_LEN * 2];
    FLOAT_t tx_time_buffer[OFDM_SYMBOL_LEN * 2];
    uint32_t tx_sample_idx;

    /* Expanded window LUTs to hold duplicated weights for [Re, Im] pairs */
    FLOAT_t window_rise_complex[TX_W_LEN * 2];
    FLOAT_t window_fall_complex[TX_W_LEN * 2];

    ofdm_packer_tx_t ofdm_srv_tx;

    uint8_t tx_preamble_required;

} ofdm_modem_tx_t;

typedef struct {
    ofdm_subcarrier_bpsk_t rx_subcarriers[OFDM_NUM_CHANNELS];
    ARM_MORPH(arm_cfft_instance) cfft_inst;

    FLOAT_t fft_buffer[FFT_LEN * 2];
    FLOAT_t rx_time_buffer[OFDM_SYMBOL_LEN * 2];
    uint32_t rx_sample_idx;

    FLOAT_t alpha_lock;

    /* Pre-calculated window LUTs to hold duplicated weights for [Re, Im] pairs */
    FLOAT_t window_rise_complex[RX_W_LEN * 2];
    FLOAT_t window_fall_complex[RX_W_LEN * 2];

    ofdm_packer_rx_t ofdm_srv_rx;

    /* --- DIGITAL AGC STATE VARIABLES --- */
    FLOAT_t agc_gain;            /* Current multiplier for incoming samples */
    FLOAT_t agc_env;             /* Smoothed signal energy envelope */
    FLOAT_t agc_target;          /* Desired RMS target for the FFT block */
    FLOAT_t agc_attack;          /* Fast tracking coefficient */
    FLOAT_t agc_decay;           /* Slow release coefficient */

    sync_state_t sync_state;

    /* Скользящие линии задержки для комплексного сигнала */
    FLOAT_t delay_buffer_i[SYNC_HALF_LEN];
    FLOAT_t delay_buffer_q[SYNC_HALF_LEN];
    uint32_t delay_ptr;

    /* Метрики автокоррелятора Шмидля-Кокса */
    FLOAT_t R_i;  /* Вещественная часть окна корреляции */
    FLOAT_t R_q;  /* Мнимая часть окна корреляции */
    FLOAT_t E;    /* Мгновенная энергия половины символа */
} ofdm_modem_rx_t;


/**
 * @brief Runtime initialization of the standalone OFDM transmitter context.
 *        Generates complex window LUT weights using arm_sin_cos_f32.
 */
static void ofdm_modem_tx_init(ofdm_modem_tx_t *self)
{
    ARM_MORPH(arm_cfft_init)(&self->cfft_inst, FFT_LEN);
    self->tx_sample_idx = 0;
    ARM_MORPH(arm_fill)(0, self->fft_buffer, FFT_LEN * 2);
    ARM_MORPH(arm_fill)(0, self->tx_time_buffer, OFDM_SYMBOL_LEN * 2);

    /* Generate complex window LUT weights using arm_sin_cos_f32 */
    for (int i = 0; i < TX_W_LEN; i++)
    {
        float32_t sin_val, cos_val;
        /* Convert radians to degrees for CMSIS-DSP */
        float32_t phase_degrees = (float32_t)(M_PI * i / TX_W_LEN) * (180.0f / (float32_t)M_PI);

        /* Calculate sine and cosine simultaneously */
        arm_sin_cos_f32(phase_degrees, &sin_val, &cos_val);

        FLOAT_t w_rise = (1 - (FLOAT_t)cos_val) / 2;
        FLOAT_t w_fall = (1 + (FLOAT_t)cos_val) / 2;

        /* Duplicate weight for both Real and Imaginary components of the sample */
        self->window_rise_complex[i * 2]     = w_rise;
        self->window_rise_complex[i * 2 + 1] = w_rise;

        self->window_fall_complex[i * 2]     = w_fall;
        self->window_fall_complex[i * 2 + 1] = w_fall;
    }
    /* Initialize upper service data buffers and interleavers */
    ofdm_packer_tx_init(&self->ofdm_srv_tx);
    self->tx_preamble_required = 0;
}

/**
 * @brief Resets transient caches and trackers inside the transmitter instance.
 * @param self Pointer to the active transmitter context.
 */
static void ofdm_modem_tx_reset(ofdm_modem_tx_t *self)
{
    self->tx_sample_idx = 0;
    ARM_MORPH(arm_fill)(0, self->tx_time_buffer, OFDM_SYMBOL_LEN * 2);
    ofdm_packer_tx_reset(&self->ofdm_srv_tx);
}

static void ofdm_modem_rx_init(ofdm_modem_rx_t *self)
{
    ARM_MORPH(arm_cfft_init)(&self->cfft_inst, FFT_LEN);
    self->rx_sample_idx = 0;
    self->alpha_lock = 0.02;
    ARM_MORPH(arm_fill)(0, self->fft_buffer, FFT_LEN * 2);
    ARM_MORPH(arm_fill)(0, self->rx_time_buffer, OFDM_SYMBOL_LEN * 2);

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

    /* PRE-CALCULATE RX COMPLEX WINDOW LUT USING arm_sin_cos_f32 */
    for (int i = 0; i < RX_W_LEN; i++)
    {
        float32_t sin_val, cos_val;
        float32_t phase_degrees = (float32_t)(M_PI * i / RX_W_LEN) * (180.0f / (float32_t)M_PI);

        arm_sin_cos_f32(phase_degrees, &sin_val, &cos_val);

        FLOAT_t w_rise = (1 - (FLOAT_t)cos_val) / 2;
        FLOAT_t w_fall = (1 + (FLOAT_t)cos_val) / 2;

        self->window_rise_complex[i * 2]     = w_rise;
        self->window_rise_complex[i * 2 + 1] = w_rise;

        self->window_fall_complex[i * 2]     = w_fall;
        self->window_fall_complex[i * 2 + 1] = w_fall;
    }
    ofdm_packer_rx_init(&self->ofdm_srv_rx);

    /* Initialize AGC parameters with implicit type promotion */
    self->agc_gain = 1;
    self->agc_env = 0.01;
    self->agc_target = 1;

    /* Time constants tailored for 256-point symbol rate tracking on HF */
    self->agc_attack = 0.1;      /* Fast attack to handle sudden ionospheric bursts */
    self->agc_decay = 0.01;      /* Slow decay to prevent breathing on data changes */

    /* Сброс автокоррелятора */
    self->sync_state = STATE_SEARCHING_PREAMBLE;
    self->delay_ptr = 0;
    self->R_i = 0;
    self->R_q = 0;
    self->E = 0.001; /* Защита от деления на ноль */

    ARM_MORPH(arm_fill)(0, self->delay_buffer_i, SYNC_HALF_LEN);
    ARM_MORPH(arm_fill)(0, self->delay_buffer_q, SYNC_HALF_LEN);
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
    ofdm_packer_rx_reset(& self->ofdm_srv_rx);

    /* Initialize AGC parameters with implicit type promotion */
    self->agc_gain = 1;
    self->agc_env = 0.01;
    self->agc_target = 1;

    /* Time constants tailored for 256-point symbol rate tracking on HF */
    self->agc_attack = 0.1;      /* Fast attack to handle sudden ionospheric bursts */
    self->agc_decay = 0.01;      /* Slow decay to prevent breathing on data changes */

    /* Сброс автокоррелятора */
    self->sync_state = STATE_SEARCHING_PREAMBLE;
    self->delay_ptr = 0;
    self->R_i = 0;
    self->R_q = 0;
    self->E = 0.001; /* Защита от деления на ноль */

    ARM_MORPH(arm_fill)(0, self->delay_buffer_i, SYNC_HALF_LEN);
    ARM_MORPH(arm_fill)(0, self->delay_buffer_q, SYNC_HALF_LEN);
}

/**
 * @brief Block-based transmitter modulation processing with mathematically pure IFFT layout.
 *        Ensures strict subcarrier orthogonality and ideal single-sideband IQ generation.
 */
static void OLDofdm_modem_tx_block(ofdm_modem_tx_t *self, void (*get_bits_cb)(ofdm_modem_tx_t *self, uint8_t *bits), FLOAT_t *out_buffer_i, FLOAT_t *out_buffer_q, uint32_t block_size)
{
	const FLOAT_t magnitude = 32;
    for (uint32_t sample_idx = 0; sample_idx < block_size; sample_idx++)
    {
        /* Regenerate symbol payload if the active time domain vector cache is exhausted */
        if (self->tx_sample_idx >= OFDM_SYMBOL_LEN)
        {
            self->tx_sample_idx = 0;

            /* Clear the entire FFT complex plane using CMSIS-DSP vector fill */
            ARM_MORPH(arm_fill)(0, self->fft_buffer, FFT_LEN * 2);

            uint8_t tx_bits[OFDM_NUM_CHANNELS] = {0};
            get_bits_cb(self, tx_bits);

            /* MATHEMATICALLY CORRECT BPSK-OFDM MAPPING: */
            /* Imaginary part MUST be 0.0 to preserve native CFFT subcarrier orthogonality. */
            /* Single-sideband IQ signal is achieved by filling ONLY bins 1..8 and keeping bins 120..127 at 0. */
            for (uint32_t ch = 0; ch < OFDM_NUM_CHANNELS; ch++)
            {
                const uint32_t bin_idx = subcarrier_map[ch];

                self->fft_buffer[bin_idx * 2]     = tx_bits[ch] ? magnitude : -magnitude; /* Real (I) component */
                //self->fft_buffer[bin_idx * 2 + 1] = 0.0;                        /* Imaginary (Q) component strictly ZERO */
            }

            /* Inverse Complex FFT execution: isInverseFFT = 1, bitReverseFlag = 1 */
            //ARM_MORPH(arm_cfft)(&self->cfft_inst, self->fft_buffer, 1, 1);
            dsp_cfft(&self->cfft_inst, self->fft_buffer, 1);

            /* Construct the Cyclic Prefix window using fast block memory transport */
            uint32_t cp_start = (FFT_LEN - CYCLIC_PREFIX_LEN) * 2; // (128 - 32) * 2 = 192

            /* Copy the tail part of the IFFT output to the beginning of the transmission frame */
            ARM_MORPH(arm_copy)(&self->fft_buffer[cp_start],
                                self->tx_time_buffer,
								CYCLIC_PREFIX_LEN * 2);

            /* Copy the entire useful IFFT payload directly following the guard prefix interval */
            ARM_MORPH(arm_copy)(self->fft_buffer,
                                &self->tx_time_buffer[CYCLIC_PREFIX_LEN * 2],
                                FFT_LEN * 2);

            /* --- VECTOR OPTIMIZATION: TRANSITION WINDOWING VIA CMSIS-DSP MULT --- */
            /* Smooth the absolute beginning of the symbol (Rising edge) */
            ARM_MORPH(arm_mult)(self->tx_time_buffer,
                                self->window_rise_complex,
                                self->tx_time_buffer,
								TX_W_LEN * 2);

            /* Smooth the absolute end of the symbol (Falling edge) */
            uint32_t sym_end_offset = (OFDM_SYMBOL_LEN - TX_W_LEN) * 2;
            ARM_MORPH(arm_mult)(&self->tx_time_buffer[sym_end_offset],
                                self->window_fall_complex,
                                &self->tx_time_buffer[sym_end_offset],
								TX_W_LEN * 2);
       }

        /* Stream serialized data samples into active processing streams for hftrx path */
        out_buffer_i[sample_idx] = self->tx_time_buffer[self->tx_sample_idx * 2];
        out_buffer_q[sample_idx] = self->tx_time_buffer[self->tx_sample_idx * 2 + 1];

        self->tx_sample_idx++;
    }
}

/**
 * @brief Block-based receiver demodulation processing with RX Time-Domain Windowing.
 *        Trigonometry optimized via direct arm_sin_cos_f32 execution.
 */
static void OLDofdm_modem_rx_block(ofdm_modem_rx_t *self, const FLOAT_t *in_buffer_i, const FLOAT_t *in_buffer_q, uint32_t block_size, void (*process_bits_cb)(ofdm_modem_rx_t *self, const uint8_t *bits))
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

            /* --- HIGH SPEED DIGITAL AGC ENGINE --- */
            /* Compute the total energy of the 256-point complex time-domain block */
            FLOAT_t power_sum = 0;
            ARM_MORPH(arm_power)(self->fft_buffer, FFT_LEN * 2, &power_sum);

            /* Calculate exact RMS using SQRTF macro from dspdefines.h */
            FLOAT_t current_rms = SQRTF(power_sum / (FLOAT_t)FFT_LEN);

            /* Dynamic envelope tracking (Attack / Decay leaky integrator) */
            if (current_rms > self->agc_env) {
                self->agc_env += self->agc_attack * (current_rms - self->agc_env);
            } else {
                self->agc_env += self->agc_decay * (current_rms - self->agc_env);
            }

            /* Guard against division by zero */
            if (self->agc_env > 1e-5) {
                self->agc_gain = self->agc_target / self->agc_env;
            }

            /* Vectorized Scaling: Perfectly normalize the block amplitude before FFT stage */
            ARM_MORPH(arm_scale)(self->fft_buffer, self->agc_gain, self->fft_buffer, FFT_LEN * 2);
            /* ------------------------------------- */

            /* --- VECTOR OPTIMIZATION: WINDOWING VIA CMSIS-DSP MULT --- */
            ARM_MORPH(arm_mult)(self->fft_buffer, self->window_rise_complex, self->fft_buffer, RX_W_LEN * 2);
            uint32_t fft_end_offset = (FFT_LEN - RX_W_LEN) * 2;
            ARM_MORPH(arm_mult)(&self->fft_buffer[fft_end_offset], self->window_fall_complex, &self->fft_buffer[fft_end_offset], RX_W_LEN * 2);

            /* ... (дальнейший ваш цикл деротации поднесущих через arm_sin_cos_f32) ... */
            /* Forward Complex FFT conversion: isInverseFFT = 0, bitReverseFlag = 1 */
            //ARM_MORPH(arm_cfft)(&self->cfft_inst, self->fft_buffer, 0, 1);
            dsp_cfft(&self->cfft_inst, self->fft_buffer, 0);

            uint8_t rx_bits[OFDM_NUM_CHANNELS] = {0};
            uint32_t all_channel_locked = 1;

            /* De-rotate phase offsets and track multi-frequency channel state variations */
            for (uint32_t ch = 0; ch < OFDM_NUM_CHANNELS; ch++)
            {
                const uint32_t bin_idx = subcarrier_map[ch];
                ofdm_subcarrier_bpsk_t *sub = &self->rx_subcarriers[ch];

                FLOAT_t raw_i = self->fft_buffer[bin_idx * 2];
                FLOAT_t raw_q = self->fft_buffer[bin_idx * 2 + 1];

                /* Declare strict float32_t targets required by direct CMSIS-DSP API */
                float32_t sin_val, cos_val;

                /* Convert phase from radians [0..2*PI] to degrees [-180..180] for arm_sin_cos_f32 */
                float32_t phase_degrees = (float32_t)sub->phase_nco * (180.0f / (float32_t)M_PI);
                if (phase_degrees > 180.0f) {
                    phase_degrees -= 360.0f;
                }

                /* Call native float32 CMSIS function directly to compute sin/cos simultaneously */
                arm_sin_cos_f32(phase_degrees, &sin_val, &cos_val);

                /* Cast output back to polymorphic FLOAT_t wrapper for processing loop */
                FLOAT_t sin_p = (FLOAT_t)sin_val;
                FLOAT_t cos_p = (FLOAT_t)cos_val;

                /* Complex phase de-rotation multiplication */
                FLOAT_t derot_i = raw_i * cos_p + raw_q * sin_p;
                FLOAT_t derot_q = raw_q * cos_p - raw_i * sin_p;

                /* Bounded Amplitude Normalization for Costas Loop stability */
                FLOAT_t mag2 = derot_i * derot_i + derot_q * derot_q;
                if (mag2 > 1e-6)
                {
                    FLOAT_t mag = SQRTF(mag2);
                    derot_i /= mag;
                    derot_q /= mag;
                }

                /* Costas BPSK Phase Error Detector metric: e = I * Q */
                FLOAT_t error_c = derot_i * derot_q;

                /* Closed-loop frequency and tracking updates */
                sub->costas_integrator += error_c * sub->costas_ki;
                sub->phase_step_nco = error_c * sub->costas_kp + sub->costas_integrator;

                /* Quality monitoring assessment metric calculations */
                FLOAT_t instant_metric = (derot_i * derot_i) - (derot_q * derot_q);
                sub->phase_lock_metric += self->alpha_lock * (instant_metric - sub->phase_lock_metric);
                sub->is_phase_locked = (sub->phase_lock_metric > 0.55) ? 1 : 0;

                if (! sub->is_phase_locked) {
                    all_channel_locked = 0;
                }

                /* Slicer decision boundary output evaluation */
                rx_bits[ch] = (derot_i >= 0) ? 1 : 0;

                /* Update step bounded modulo 2*pi execution */
                sub->phase_nco += sub->phase_step_nco;
                if (sub->phase_nco >= 2 * M_PI) sub->phase_nco -= 2 * M_PI;
                if (sub->phase_nco < 0) sub->phase_nco += 2 * M_PI;
            }

            /* Автомат сброса: если все каналы потеряли захват фазы — уходим в поиск */
            if (all_channel_locked == 0) {
                self->sync_state = STATE_SEARCHING_PREAMBLE;
                self->rx_sample_idx = 0;
            } else {
                /* Direct processing of extracted frame data stream */
                process_bits_cb(self, rx_bits);
            }
        }
    }
}

void NEWofdm_modem_rx_block(ofdm_modem_rx_t *self, const FLOAT_t *in_buffer_i, const FLOAT_t *in_buffer_q, uint32_t block_size, void (*process_bits_cb)(ofdm_modem_rx_t *self, const uint8_t *bits))
{
    const FLOAT_t sync_threshold = 0.55;
    const FLOAT_t alpha_sync = 0.05;

    for (uint32_t sample_idx = 0; sample_idx < block_size; sample_idx++)
    {
        const FLOAT_t curr_i = in_buffer_i[sample_idx];
        const FLOAT_t curr_q = in_buffer_q[sample_idx];

        /* Извлекаем задержанный на половину БПФ (128 сэмплов) сигнал */
        const FLOAT_t del_i = self->delay_buffer_i[self->delay_ptr];
        const FLOAT_t del_q = self->delay_buffer_q[self->delay_ptr];

        /* Обновляем скользящую линию задержки */
        self->delay_buffer_i[self->delay_ptr] = curr_i;
        self->delay_buffer_q[self->delay_ptr] = curr_q;
        self->delay_ptr = (self->delay_ptr + 1) % SYNC_HALF_LEN;

        /* Мгновенная взаимная корреляция текущего и задержанного отсчетов */
        const FLOAT_t cross_i = curr_i * del_i + curr_q * del_q;
        const FLOAT_t cross_q = curr_q * del_i - curr_i * del_q;

        /* Мгновенная энергия половины символа */
        FLOAT_t curr_energy = curr_i * curr_i + curr_q * curr_q;

        /* Скользящее интегрирование (экспоненциальный фильтр) */
        self->R_i += alpha_sync * (cross_i - self->R_i);
        self->R_q += alpha_sync * (cross_q - self->R_q);
        self->E   += alpha_sync * (curr_energy - self->E);

        /* Расчет квадрата модуля метрики Шмидля-Кокса */
        const FLOAT_t mag_R2 = self->R_i * self->R_i + self->R_q * self->R_q;
        const FLOAT_t E2 = self->E * self->E;
        const FLOAT_t metric = mag_R2 / (E2 + 1e-6f);

        /* Конечно-автоматный триггер захвата кадра (FSM) */
        if (self->sync_state == STATE_SEARCHING_PREAMBLE)
        {
            if (metric > sync_threshold)
            {
                /* Преамбула найдена: сбрасываем индекс под полезное тело БПФ */
                self->sync_state = STATE_PROCESSING_DATA;
                self->rx_sample_idx = 0;
            }
            else
            {
                continue; /* Продолжаем скользящий поиск в эфире */
            }
        }

        /* Накопление комплексного кадра во временной буфер */
        self->rx_time_buffer[self->rx_sample_idx * 2] = curr_i;
        self->rx_time_buffer[self->rx_sample_idx * 2 + 1] = curr_q;
        self->rx_sample_idx++;

        /* Если накопили полный OFDM символ (включая полезное тело сигнала) */
        if (self->rx_sample_idx >= OFDM_SYMBOL_LEN)
        {
            self->rx_sample_idx = 0;

            /* 1. Извлекаем тело сигнала, отрезая циклический префикс */
            ARM_MORPH(arm_copy)(&self->rx_time_buffer[CYCLIC_PREFIX_LEN * 2], self->fft_buffer, FFT_LEN * 2);

            /* 2. Блочная цифровая АРУ по RMS мощности */
            FLOAT_t power_sum;
            ARM_MORPH(arm_power)(self->fft_buffer, FFT_LEN * 2, &power_sum);
            FLOAT_t current_rms = SQRTF(power_sum / (FLOAT_t)FFT_LEN);

            if (current_rms > self->agc_env) {
                self->agc_env += self->agc_attack * (current_rms - self->agc_env);
            } else {
                self->agc_env += self->agc_decay * (current_rms - self->agc_env);
            }

            if (self->agc_env > 1e-5f) {
                self->agc_gain = self->agc_target / self->agc_env;
            }
            ARM_MORPH(arm_scale)(self->fft_buffer, self->agc_gain, self->fft_buffer, FFT_LEN * 2);

            /* 3. Векторное оконное сглаживание краев перед БПФ */
            ARM_MORPH(arm_mult)(self->fft_buffer, self->window_rise_complex, self->fft_buffer, RX_W_LEN * 2);
            uint32_t fft_end_offset = (FFT_LEN - RX_W_LEN) * 2;
            ARM_MORPH(arm_mult)(&self->fft_buffer[fft_end_offset], self->window_fall_complex, &self->fft_buffer[fft_end_offset], RX_W_LEN * 2);

            /* 4. Прямое комплексное преобразование Фурье */
            dsp_cfft(&self->cfft_inst, self->fft_buffer, 0);

            uint8_t rx_bits[OFDM_NUM_CHANNELS] = {0};
            uint32_t all_channel_locked = 1;

            /* 5. Цикл демодуляции и петель Костаса по всем 8 поднесущим */
            for (uint32_t ch = 0; ch < OFDM_NUM_CHANNELS; ch++)
            {
                const uint32_t bin_idx = subcarrier_map[ch];
                ofdm_subcarrier_bpsk_t *sub = &self->rx_subcarriers[ch];

                FLOAT_t raw_i = self->fft_buffer[bin_idx * 2];
                FLOAT_t raw_q = self->fft_buffer[bin_idx * 2 + 1];

                /* Одновременный быстрый расчет синуса и косинуса */
                float32_t sin_val, cos_val;
                float32_t phase_degrees = (float32_t)sub->phase_nco * (180.0f / (float32_t)M_PI);
                if (phase_degrees > 180.0f) {
                    phase_degrees -= 360.0f;
                }
                arm_sin_cos_f32(phase_degrees, &sin_val, &cos_val);

                const FLOAT_t sin_p = (FLOAT_t)sin_val;
                const FLOAT_t cos_p = (FLOAT_t)cos_val;

                /* Деротация (поворот) фазы */
                FLOAT_t derot_i = raw_i * cos_p + raw_q * sin_p;
                FLOAT_t derot_q = raw_q * cos_p - raw_i * sin_p;

                /* Амплитудная нормировка созвездия */
                const FLOAT_t mag2 = derot_i * derot_i + derot_q * derot_q;
                if (mag2 > 1e-6) {
                	const FLOAT_t mag = 1 / SQRTF(mag2);
                    derot_i *= mag;
                    derot_q *= mag;
                }

                /* Расчет фазовой ошибки дискриминатора Костаса для BPSK */
                FLOAT_t error_c = derot_i * derot_q;
                sub->costas_integrator += error_c * sub->costas_ki;
                sub->phase_step_nco = error_c * sub->costas_kp + sub->costas_integrator;

                /* Оценка качества удержания захвата фазы (Lock Detector) */
                FLOAT_t instant_metric = (derot_i * derot_i) - (derot_q * derot_q);
                sub->phase_lock_metric += self->alpha_lock * (instant_metric - sub->phase_lock_metric);
                sub->is_phase_locked = (sub->phase_lock_metric > 0.55) ? 1 : 0;

                if (! sub->is_phase_locked) {
                    all_channel_locked = 0;
                }

                /* Жесткое решение демодулятора (Slicer) */
                rx_bits[ch] = ! (derot_i >= 0) ? 1 : 0;		// ТУТ ФИКС БАГА

                /* Шаг аккумулятора фазы NCO */
                sub->phase_nco += sub->phase_step_nco;
                if (sub->phase_nco >= 2 * M_PI) sub->phase_nco -= 2 * M_PI;
                if (sub->phase_nco < 0) sub->phase_nco += 2 * M_PI;
            }

            /* Автомат сброса: если все каналы потеряли захват фазы — уходим в поиск */
            if (all_channel_locked == 0) {
                self->sync_state = STATE_SEARCHING_PREAMBLE;
                self->rx_sample_idx = 0;
            } else {
                /* Direct processing of extracted frame data stream */
               process_bits_cb(self, rx_bits);
            }
        }
    }
}

void NEWofdm_modem_tx_block(ofdm_modem_tx_t *self, void (*get_bits_cb)(ofdm_modem_tx_t *self, uint8_t *bits), FLOAT_t *out_buffer_i, FLOAT_t *out_buffer_q, uint32_t block_size)
{
	const FLOAT_t magnitude = 32;
    for (uint32_t sample_idx = 0; sample_idx < block_size; sample_idx++)
    {
        /* Если кеш временных сэмплов текущего OFDM-символа исчерпан */
        if (self->tx_sample_idx >= OFDM_SYMBOL_LEN)
        {
            self->tx_sample_idx = 0;

            /* Полностью очищаем комплексную плоскость БПФ */
            ARM_MORPH(arm_fill)(0.0f, self->fft_buffer, FFT_LEN * 2);

            /* АВТОМАТ КАНАЛЬНОЙ СИНХРОНИЗАЦИИ: ГЕНЕРАЦИЯ ПРЕАМБУЛЫ ШМИДЛЯ-КОКСА */
            if (self->tx_preamble_required)
            {
                /* Сбрасываем триггер: преамбула передается ровно один раз перед пакетом данных */
                self->tx_preamble_required = 0;

                /* Маппинг преамбулы Шмидля-Кокса: пишем ТОЛЬКО в четные частотные бины */
                /* Это математически создает две идентичные половины во временной области */
                /* Используем знаки фиксированной PN-последовательности для надежного захвата */
                self->fft_buffer[2 * 2]  =  magnitude;  /* Bin 2 */
                self->fft_buffer[4 * 2]  = -magnitude;  /* Bin 4 */
                self->fft_buffer[6 * 2]  =  magnitude;  /* Bin 6 */
                self->fft_buffer[8 * 2]  =  magnitude;  /* Bin 8 */
                self->fft_buffer[10 * 2] = -magnitude;  /* Bin 10 */
                self->fft_buffer[12 * 2] =  magnitude;  /* Bin 12 */
                self->fft_buffer[14 * 2] = -magnitude;  /* Bin 14 */
                self->fft_buffer[16 * 2] = -magnitude;  /* Bin 16 */
            }
            else
            {
                /* РЕЖИМ ПЕРЕДАЧИ ПОЛЕЗНЫХ ДАННЫХ */
                uint8_t tx_bits[OFDM_NUM_CHANNELS] = {0};
                get_bits_cb(self, tx_bits);

                /* MATHEMATICALLY CORRECT BPSK-OFDM MAPPING: */
                /* Imaginary part MUST be 0.0 to preserve native CFFT subcarrier orthogonality. */
                /* Single-sideband IQ signal is achieved by filling ONLY bins 1..8 and keeping bins 120..127 at 0. */
                for (uint32_t ch = 0; ch < OFDM_NUM_CHANNELS; ch++)
                {
                    const uint32_t bin_idx = subcarrier_map[ch];

                    self->fft_buffer[bin_idx * 2]     = tx_bits[ch] ? magnitude : -magnitude; /* Real (I) component */
                    //self->fft_buffer[bin_idx * 2 + 1] = 0.0;                        /* Imaginary (Q) component strictly ZERO */
                }
            }

            /* Обратное БПФ: переводим частотную сетку во временной комплексный сигнал */
            /* isInverseFFT = 1, bitReverseFlag = 1 */
            ARM_MORPH(arm_cfft)(&self->cfft_inst, self->fft_buffer, 1, 1);

            /* СБОРКА OFDM-КАДРА: ГЕНЕРАЦИЯ ЦИКЛИЧЕСКОГО ПРЕФИКСА (CP) */
            /* Копируем последние 16 комплексных отсчетов (хвост) ОБПФ в начало префикса */
            uint32_t cp_source_start = FFT_LEN - CYCLIC_PREFIX_LEN;
            for (uint32_t i = 0; i < CYCLIC_PREFIX_LEN; i++)
            {
                uint32_t src_idx = (cp_source_start + i) * 2;
                uint32_t dst_idx = i * 2;
                self->tx_time_buffer[dst_idx]     = self->fft_buffer[src_idx];
                self->tx_time_buffer[dst_idx + 1] = self->fft_buffer[src_idx + 1];
            }

            /* Копируем основное полезное тело ОБПФ (256 точек) сразу за префиксом */
            for (uint32_t i = 0; i < FFT_LEN; i++)
            {
                uint32_t src_idx = i * 2;
                uint32_t dst_idx = (CYCLIC_PREFIX_LEN + i) * 2;
                self->tx_time_buffer[dst_idx]     = self->fft_buffer[src_idx];
                self->tx_time_buffer[dst_idx + 1] = self->fft_buffer[src_idx + 1];
            }

            /* ВЕКТОРНОЕ ОКОННОЕ СГЛАЖИВАНИЕ СТЫКОВ СИМВОЛА (RAISED COSINE MULT) */
            ARM_MORPH(arm_mult)(self->tx_time_buffer, self->window_rise_complex, self->tx_time_buffer, TX_W_LEN * 2);
            uint32_t sym_end_offset = (OFDM_SYMBOL_LEN - TX_W_LEN) * 2;
            ARM_MORPH(arm_mult)(&self->tx_time_buffer[sym_end_offset], self->window_fall_complex, &self->tx_time_buffer[sym_end_offset], TX_W_LEN * 2);
        }

        /* Потоковый вывод комплексных квадратур наружу для up-converter трансивера */
        out_buffer_i[sample_idx] = self->tx_time_buffer[self->tx_sample_idx * 2];
        out_buffer_q[sample_idx] = self->tx_time_buffer[self->tx_sample_idx * 2 + 1];
        self->tx_sample_idx++;
    }
}


/* ========================================================================== */
/*                          STATIC CALLBACK BRIDGES                           */
/* ========================================================================== */

/**
 * @brief Bridge function connecting the physical modulator with the internal bit packer.
 */
static void dsp_ofdm_tx_bits_bridge(ofdm_modem_tx_t *self, uint8_t *bits)
{
    /* FIXED: Extract the integrated service context directly from the active PHY block */
    ofdm_packer_get_bits_callback(&self->ofdm_srv_tx, bits);
}

/**
 * @brief Bridge function connecting the physical demodulator with the internal deinterleaver.
 */
static void dsp_ofdm_rx_bits_bridge(ofdm_modem_rx_t *self, const uint8_t *bits)
{
    /* FIXED: Extract the integrated service context directly from the active PHY block */
    ofdm_packer_process_bits_callback(&self->ofdm_srv_rx, bits);
}

/* ========================================================================== */
/*                         PUBLIC CORE TRANSCEIVER API                        */
/* ========================================================================== */

/**
 * @brief External interface for the hftrx USB CDC UART layer to inject text characters for transmission.
 *        Must be called from main.c / usb_cdc.c passing the active transmitter object instance.
 */
void dsp_ofdm_push_char_to_tx(ofdm_modem_tx_t *self, uint8_t c)
{
    ofdm_packer_put_tx_byte(&self->ofdm_srv_tx, c);
}

/**
 * @brief External interface for the hftrx USB CDC UART layer to poll for decoded text characters.
 */
uint32_t dsp_ofdm_pop_char_from_rx(ofdm_modem_rx_t *self, uint8_t *c)
{
    return ofdm_packer_get_rx_byte(&self->ofdm_srv_rx, c);
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

static void test_ofdm_get_preamble_bits(ofdm_modem_tx_t *self, uint8_t *bits)
{
	static int phase;
	memset(bits, phase ? 0x55 : 0xAA, 8);
	phase = ! phase;
}

static void test_ofdm_get_bits(ofdm_modem_tx_t *self, uint8_t *bits)
{
	static int testindex;
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

static void test_ofdm_get_bits_fill(ofdm_modem_tx_t *self, uint8_t *bits)
{
	static int testindex;
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

static void test_ofdm_get_bits_flip(ofdm_modem_tx_t *self, uint8_t *bits)
{
	static int testindex;
	bits [0] = testindex ? 0xAA : 0x55;
	bits [1] = testindex ? 0xAA : 0x55;
	bits [2] = testindex ? 0xAA : 0x55;
	bits [3] = testindex ? 0xAA : 0x55;
	bits [4] = testindex ? 0xAA : 0x55;
	bits [5] = testindex ? 0xAA : 0x55;
	bits [6] = testindex ? 0xAA : 0x55;
	bits [7] = testindex ? 0xAA : 0x55;
	testindex = ! testindex;
}

static void test_ofdm_process_null_bits(ofdm_modem_rx_t *self, const uint8_t *bits)
{
}

static void test_ofdm_process_bits(ofdm_modem_rx_t *self, const uint8_t *bits)
{
	static char conbuff [128];
	static int conbufidx;
	unsigned v = 0;

	v |= (UINT8_C(1) << 7) * !! bits [0];
	v |= (UINT8_C(1) << 6) * !! bits [1];
	v |= (UINT8_C(1) << 5) * !! bits [2];
	v |= (UINT8_C(1) << 4) * !! bits [3];
	v |= (UINT8_C(1) << 3) * !! bits [4];
	v |= (UINT8_C(1) << 2) * !! bits [5];
	v |= (UINT8_C(1) << 1) * !! bits [6];
	v |= (UINT8_C(1) << 0) * !! bits [7];

#if 1
	//PRINTF("0x%02X, ", v);
	PRINTF("%c", v);
#else
	conbuff [conbufidx] = v;
	if (++ conbufidx >= ARRAY_SIZE(conbuff))
	{
		PRINTF("%*.*s\n", conbufidx, conbufidx, conbuff);
		for (;;)
			;
	}
#endif
}

static ofdm_modem_tx_t tx_stream;
static ofdm_modem_rx_t rx_stream;

void modem_fill(IFADCvalue_t * buff)
{
	const adapter_t * const ap = & ifcodecrx;
	FLOAT_t i, q;
	NEWofdm_modem_tx_block(& tx_stream, dsp_ofdm_tx_bits_bridge, & i, & q, 1);
	FLOAT_t scale = 0.1;

	buff [DMABUF32RX0I] = adpt_output(ap, i * scale);
	buff [DMABUF32RX0Q] = adpt_output(ap, q * scale);
}

void modem_parse(const IFADCvalue_t * buff)
{
	const adapter_t * const ap = & ifcodecrx;
	const FLOAT_t i = adpt_input(ap, buff [DMABUF32RX0I]);
	const FLOAT_t q = adpt_input(ap, buff [DMABUF32RX0Q]);
	NEWofdm_modem_rx_block(& rx_stream, & i, & q, 1, dsp_ofdm_rx_bits_bridge);
}


void modem_spool(void * ctx)
{
	uint8_t c;
	if (dsp_ofdm_pop_char_from_rx(& rx_stream, & c) && c != 0 && c != 0xFF)
	{
		PRINTF("ofdm rx: %02X\n", c);
	}

}

void modem_send(uint_fast8_t c)
{
	if (c == 0x1B)
	{
		PRINTF("OFDM modem reset\n");
		ofdm_modem_rx_reset(& rx_stream);
		local_delay_ms(200);
		tx_stream.tx_preamble_required = 1;
		PRINTF("OFDM modem reset done\n");
	}

	dsp_ofdm_push_char_to_tx(& tx_stream, c);
}

///////////////////////////////////////////
static FLOAT_t vming, vmaxg;

static void pathclipping(FLOAT_t * buff, unsigned len)
{
	FLOAT_t vmin, vmax;

	ARM_MORPH(arm_min_no_idx)(buff, len, & vmin);
	ARM_MORPH(arm_max_no_idx)(buff, len, & vmax);
	vming = FMINF(vming, vmin);
	vmaxg = FMAXF(vmaxg, vmax);
	//return;

	adapter_t * const ap = & ifcodecrx;
	while (len --)
	{
		int_fast32_t v = adpt_output(ap, * buff);
		* buff ++ = adpt_input(ap, v);
	}
}

static void nullmodem(FLOAT_t * buff_i, FLOAT_t * buff_q, unsigned len)
{
	while (len --)
	{
		const FLOAT_t i = * buff_i;
		const FLOAT_t q = * buff_q;

		const FLOAT32P_t pair = xget_float_monofreq();

		const FLOAT_t absv = i * pair.IV + q * pair.QV;

		const FLOAT_t i2 = absv * pair.IV;
		const FLOAT_t q2 = absv * pair.QV;

		* buff_i = i2;
		* buff_q = q2;
		//
		++ buff_i;
		++ buff_q;
	}
}


void modem_init(void)
{
	ofdm_modem_tx_init(& tx_stream);
	ofdm_modem_rx_init(& rx_stream);

	return;
}

void modem_test(void)
{
	TP();

	ofdm_modem_tx_t tx;
	ofdm_modem_rx_t rx;

	enum { BUFFLEN = 256 * 4 };
	FLOAT_t buffer_i [BUFFLEN];
	FLOAT_t buffer_q [BUFFLEN];


	ofdm_modem_tx_init(& tx);
	ofdm_modem_rx_init(& rx);

	rx.sync_state = STATE_PROCESSING_DATA;

	NEWofdm_modem_tx_block(& tx, test_ofdm_get_preamble_bits, buffer_i, buffer_q, BUFFLEN);

	pathclipping(buffer_i, BUFFLEN);
	pathclipping(buffer_q, BUFFLEN);
	nullmodem(buffer_i, buffer_q, BUFFLEN);

	OLDofdm_modem_rx_block(& rx, buffer_i, buffer_q, BUFFLEN, test_ofdm_process_null_bits);

	unsigned i;
	for (i = 0; i < 100; ++ i)
	{

		NEWofdm_modem_tx_block(& tx, test_ofdm_get_bits, buffer_i, buffer_q, BUFFLEN);

		pathclipping(buffer_i, BUFFLEN);
		pathclipping(buffer_q, BUFFLEN);
		nullmodem(buffer_i, buffer_q, BUFFLEN);

		OLDofdm_modem_rx_block(& rx, buffer_i, buffer_q, BUFFLEN, test_ofdm_process_bits);
	}
	PRINTF("\n");
	PRINTF("OFDM_SYMBOL_LEN=%d\n", (int) OFDM_SYMBOL_LEN);
	PRINTF("Ranges: vming=%d, vmaxg=%d\n", (int) vming, (int) vmaxg);
	printf("Ranges: vming=%f, vmaxg=%f\n", vming, vmaxg);
	TP();
}

#endif /* WITHINTEGRATEDDSP */
