#include "hardware.h"

#if WITHINTEGRATEDDSP && 1

#include "dspdefines.h"
#include "audio.h"
#include "serial.h"
#include "buffers.h"
#include "formats.h"

/* Baudot ITA2 Code Character Mapping Matrices (Letters vs Figures) */
/* Strictly bounded element-by-element configuration of ITA2 Baudot matrices */
static const uint8_t rtty_ita2_letters[32] = {
    ' ', ' ', 'E', '\n', 'A', ' ', 'S', 'I',
    'U', '\r', 'D', 'R',  'J', 'N', 'F', 'C',
    'K', 'T', 'Z', 'L',  'W', 'H', 'Y', 'P',
    'Q', 'O', 'B', 'G',  ' ', 'M', 'X', 'V'
};

static const uint8_t rtty_ita2_figures[32] = {
    ' ', ' ', '3', '\n', '-', ' ', '8', '7',
    '\'', '7', '4', ' ', '4', '5', ',', ':',
    '!', '?', ' ', '"',  '9', '0', '+', '1',
    '5', '6', '1', ' ',  '1', '2', ' ', ' '
};

#define RTTY_STATE_IDLE         0
#define RTTY_STATE_START_BIT    1
#define RTTY_STATE_DATA_BITS    2
#define RTTY_STATE_STOP_BIT     3

/* Isolated context structure optimized for Zero-IF RTTY PLL receiver */
typedef struct {
    FLOAT_t prev_in_i;
    FLOAT_t prev_in_q;

    /* PLL Phase Locked Loop tracking variables inside Zero-IF */
    FLOAT_t phase_nco;           /* Phase accumulator for local tracking oscillator */
    FLOAT_t freq_shift_half_nco; /* Target shift offset amplitude in radians per sample */
    FLOAT_t pll_kp;              /* Proportional loop gain tracking coefficient */
    FLOAT_t pll_ki;              /* Integral loop gain tracking coefficient */
    FLOAT_t pll_integrator;      /* Integral loop filter memory accumulator */

    /* Low-pass data slicing bit filter */
    FLOAT_t lpf_state;           /* Leaky integrator memory for bit envelope smoothing */
    FLOAT_t lpf_alpha;           /* Smoothing ratio adapted dynamically to baud rate */

    /* Baudot asynchronous bit sampler and state machine (FSM) */
    uint32_t fsm_state;          /* Current UART/Baudot framing tracking state */
    uint32_t sample_timer;       /* Downcounter for bit duration synchronized to baud */
    uint32_t samples_per_bit;    /* Fractional internal sample step per single bit clock */
    uint32_t bit_shifter;        /* Shift register collecting incoming payload bits */
    uint32_t bits_count;         /* Number of collected data bits inside active frame */

    /* Shift state tracker (Letters vs Figures mapping flag) */
    uint32_t is_figures_case;    /* Boolean flag for Baudot ITA2 case shifting */
} rtty_zero_if_rx_t;

/**
 * @brief Dynamic configuration of the RTTY PLL engine for Zero-IF architecture.
 * @param self Pointer to the active isolated receiver context.
 * @param sample_rate Input hardware hardware sample clock (typically 48000).
 * @param shift_hz Total frequency shift delta (typically 170 Hz, 425 Hz or 850 Hz).
 * @param baud_rate Modulation speed (typically 45.45, 50, or 75 Baud).
 */
static void dsp_rtty_zero_if_configure(
    rtty_zero_if_rx_t * const self,
    const uint32_t sample_rate,
    const FLOAT_t shift_hz,
    const FLOAT_t baud_rate)
{
    /* Calculate precise NCO step for half-shift deviation in radians per sample */
    /* For 170Hz shift at 48kHz, this is 2 * PI * 85 / 48000 = 0.01112649 radians */
    self->freq_shift_half_nco = (2.0 * M_PI * (shift_hz / 2.0)) / (FLOAT_t)sample_rate;

    /* Calculate dynamic clock ticks matching exact internal baud duration */
    self->samples_per_bit = sample_rate / baud_rate;

    /* Configure loop filter coefficients optimized for tight tracking around Zero-IF */
    self->pll_kp = 0.05;
    self->pll_ki = 0.001;
    self->pll_integrator = 0.0;
    self->phase_nco = 0.0;

    /* Data bit data low-pass filter alpha cutoff matching symbol period */
    self->lpf_alpha = 2.0 / (FLOAT_t)self->samples_per_bit;
    self->lpf_state = 0.0;

    /* Asynchronous serial UART framing machine initialization */
    self->fsm_state    = RTTY_STATE_IDLE;
    self->sample_timer = 0;
    self->bit_shifter  = 0;
    self->bits_count   = 0;
    self->is_figures_case = 0;

    self->prev_in_i = 0;
    self->prev_in_q = 0;
}

/**
 * @brief Ultra-fast sample-by-sample FSK demodulator and Baudot decoder for Zero-IF.
 * @param self Pointer to the active isolated receiver context.
 * @param in_i Raw incoming analytical I quadrature component from direct conversion mixer.
 * @param in_q Raw incoming analytical Q quadrature component from direct conversion mixer.
 * @param put_char_cb User callback to emit successfully decoded text chars into hftrx UI layer.
 */
static void dsp_rtty_zero_if_process_sample(
    rtty_zero_if_rx_t * const self,
    const FLOAT_t in_i,
    const FLOAT_t in_q,
    void (* const put_char_cb)(const uint8_t character))
{
    /* In a Zero-IF complex plane, the instantaneous phase error discriminator */
    /* between input vector and local NCO tracker simplifies to a cross-product */
    const FLOAT_t local_i = in_i;
    const FLOAT_t local_q = in_q;

    /* Phase discriminator output reflecting current vector rotation polarity */
    const FLOAT_t phase_error = local_q * in_i - local_i * in_q;

    /* Leaky loop integrator update stage */
    self->pll_integrator += phase_error * self->pll_ki;

    /* Low-pass filter the loop integrator output to extract the stable FSK bit envelope */
    self->lpf_state += self->lpf_alpha * (self->pll_integrator - self->lpf_state);

    /* Hard slicing decision boundary (Slicer): Positive frequency vs Negative frequency */
    /* If lpf_state >= 0, frequency is positive (+85Hz = MARK), else negative (-85Hz = SPACE) */
    const uint32_t raw_bit_polar = (self->lpf_state >= 0.0) ? 1 : 0;

    /* ----------------====================================================== */
    /* ASYNCHRONOUS BAUDOT FRAME CHANNELS HANDLING STATE MACHINE (FSM)         */
    /* --------------------------------====================================== */
    switch (self->fsm_state)
    {
        case RTTY_STATE_IDLE:
            /* RTTY idle tone is MARK (High). A transition to SPACE (Low) signals a START bit */
            if (raw_bit_polar == 0)
            {
                self->fsm_state = RTTY_STATE_START_BIT;
                /* Sample precisely at the mid-point of the expected start bit pulse duration */
                self->sample_timer = self->samples_per_bit / 2;
            }
            break;

        case RTTY_STATE_START_BIT:
            if (self->sample_timer > 0)
            {
                self->sample_timer--;
            }
            else
            {
            	HARDWARE_DEBUG_PUTCHAR('~');
                /* Confirm start bit validity at its center before opening payload stream */
                if (raw_bit_polar == 0)
                {
                    self->fsm_state = RTTY_STATE_DATA_BITS;
                    self->sample_timer = self->samples_per_bit;
                    self->bit_shifter = 0;
                    self->bits_count = 0;
                }
                else
                {
                    self->fsm_state = RTTY_STATE_IDLE; /* False start bit trigger reset */
                }
            }
            break;

        case RTTY_STATE_DATA_BITS:
            if (self->sample_timer > 0)
            {
                self->sample_timer--;
            }
            else
            {
                /* RTTY shifts payload starting from the least significant bit (LSB) */
                self->bit_shifter |= (raw_bit_polar << self->bits_count);
                self->bits_count++;
                self->sample_timer = self->samples_per_bit;

                /* Standard ITA2 RTTY uses exactly 5 data payload bits per symbol */
                if (self->bits_count >= 5)
                {
                    self->fsm_state = RTTY_STATE_STOP_BIT;
                    /* Standard RTTY stop bit length is longer (typically 1.5 bits duration) */
                    self->sample_timer = (self->samples_per_bit * 3) / 2;

                    /* --- CONDITIONAL BAUDOT ITA2 ALPHABET TRANSLATION LAYER --- */
                    const uint32_t raw_code = self->bit_shifter & 0x1F;

                    if (raw_code == 0x1F)
                    {
                        /* LETTERS (LTRS) escape code received: switch to alpha matrix */
                        self->is_figures_case = 0;
                    }
                    else if (raw_code == 0x1B)
                    {
                        /* FIGURES (FIGS) escape code received: switch to numeric matrix */
                        self->is_figures_case = 1;
                    }
                    else if (raw_code > 0x00)
                    {
                        /* Resolve the current output char using active case matrix tracking */
                        const uint8_t ascii_char = self->is_figures_case ?
                                                    rtty_ita2_figures[raw_code] :
                                                    rtty_ita2_letters[raw_code];

                        /* Forward character out via decoupled hot-path callback */
                        if (ascii_char != '_')
                        {
                            put_char_cb(ascii_char);
                        }
                    }
                }
            }
            break;

        case RTTY_STATE_STOP_BIT:
            if (self->sample_timer > 0)
            {
                self->sample_timer--;
            }
            else
            {
                /* Stop bit duration safely fulfilled, switch back to scan for next frame start */
                self->fsm_state = RTTY_STATE_IDLE;
            }
            break;

        default:
            self->fsm_state = RTTY_STATE_IDLE;
            break;
    }
}

/**
 * @brief Ultra-fast sample-by-sample FSK demodulator and Baudot decoder for Zero-IF.
 * @param self Pointer to the active isolated receiver context.
 * @param in_i Raw incoming analytical I quadrature component from direct conversion mixer.
 * @param in_q Raw incoming analytical Q quadrature component from direct conversion mixer.
 * @param put_char_cb User callback to emit successfully decoded text chars into hftrx UI layer.
 */
static void dsp_rtty_zero_if_process_sample2(
    rtty_zero_if_rx_t * const self,
    const FLOAT_t in_i,
    const FLOAT_t in_q,
    void (* const put_char_cb)(const uint8_t character))
{
    /* Frequency discriminator via complex conjugate multiplication with previous sample: */
    /* V_now * V_prev^* = (in_i + j*in_q) * (prev_i - j*prev_q) */
    /* The imaginary part of this product is strictly proportional to instant frequency */
    const FLOAT_t phase_error = in_q * self->prev_in_i - in_i * self->prev_in_q;

    /* Save current incoming sample memory for the next loop execution step */
    self->prev_in_i = in_i;
    self->prev_in_q = in_q;

    /* Leaky loop filter integrator update stage */
    self->pll_integrator += phase_error * self->pll_ki;

    /* Low-pass filter the raw error to extract stable FSK bit envelope without noise spikes */
    self->lpf_state += self->lpf_alpha * (phase_error - self->lpf_state);

    /* Hard slicing decision boundary (Slicer): Positive frequency vs Negative frequency */
    /* If lpf_state >= 0.0 -> MARK tone (+85Hz), else SPACE tone (-85Hz) */
    const uint32_t raw_bit_polar = (self->lpf_state >= 0.0) ? 1 : 0;

    /* ----------------====================================================== */
    /* ASYNCHRONOUS BAUDOT FRAME CHANNELS HANDLING STATE MACHINE (FSM)         */
    /* --------------------------------====================================== */
    switch (self->fsm_state)
    {
        case RTTY_STATE_IDLE:
            /* RTTY idle tone is MARK (High). A transition to SPACE (Low) signals a START bit */
            if (raw_bit_polar == 0)
            {
                self->fsm_state = RTTY_STATE_START_BIT;
                /* Sample precisely at the mid-point of the expected start bit pulse duration */
                self->sample_timer = self->samples_per_bit / 2;
            }
            break;

        case RTTY_STATE_START_BIT:
            if (self->sample_timer > 0)
            {
                self->sample_timer--;
            }
            else
            {
                /* Confirm start bit validity at its center before opening payload stream */
                if (raw_bit_polar == 0)
                {
                    self->fsm_state = RTTY_STATE_DATA_BITS;
                    self->sample_timer = self->samples_per_bit;
                    self->bit_shifter = 0;
                    self->bits_count = 0;
                }
                else
                {
                    self->fsm_state = RTTY_STATE_IDLE; /* False start bit trigger reset */
                }
            }
            break;

        case RTTY_STATE_DATA_BITS:
            if (self->sample_timer > 0)
            {
                self->sample_timer--;
            }
            else
            {
                /* RTTY shifts payload starting from the least significant bit (LSB) */
                self->bit_shifter |= (raw_bit_polar << self->bits_count);
                self->bits_count++;
                self->sample_timer = self->samples_per_bit;

                /* Standard ITA2 RTTY uses exactly 5 data payload bits per symbol */
                if (self->bits_count >= 5)
                {
                    self->fsm_state = RTTY_STATE_STOP_BIT;
                    /* Standard RTTY stop bit length is longer (typically 1.5 bits duration) */
                    self->sample_timer = (self->samples_per_bit * 3) / 2;

                    /* --- CONDITIONAL BAUDOT ITA2 ALPHABET TRANSLATION LAYER --- */
                    const uint32_t raw_code = self->bit_shifter & 0x1F;

                    if (raw_code == 0x1F)
                    {
                        /* LETTERS (LTRS) escape code received: switch to alpha matrix */
                        self->is_figures_case = 0;
                    }
                    else if (raw_code == 0x1B)
                    {
                        /* FIGURES (FIGS) escape code received: switch to numeric matrix */
                        self->is_figures_case = 1;
                    }
                    else if (raw_code > 0x00)
                    {
                        /* Resolve the current output char using active case matrix tracking */
                        const uint8_t ascii_char = self->is_figures_case ?
                                                    rtty_ita2_figures[raw_code] :
                                                    rtty_ita2_letters[raw_code];

                        /* Forward character out via decoupled hot-path callback */
                        if (ascii_char != '_')
                        {
                            put_char_cb(ascii_char);
                        }
                    }
                }
            }
            break;

        case RTTY_STATE_STOP_BIT:
            if (self->sample_timer > 0)
            {
                self->sample_timer--;
            }
            else
            {
                /* Stop bit duration safely fulfilled, switch back to scan for next frame start */
                self->fsm_state = RTTY_STATE_IDLE;
            }
            break;

        default:
            self->fsm_state = RTTY_STATE_IDLE;
            break;
    }
}


static void rxcharacter(const uint8_t c)
{
	HARDWARE_DEBUG_PUTCHAR(c);
}

static rtty_zero_if_rx_t rx_stream;


void modem_parse(const IFADCvalue_t * buff)
{
	const adapter_t * const ap = & ifcodecrx;
	const FLOAT_t i = adpt_input(ap, buff [DMABUF32RX0I]);
	const FLOAT_t q = adpt_input(ap, buff [DMABUF32RX0Q]);
	dsp_rtty_zero_if_process_sample2(& rx_stream, i, q, rxcharacter);
}

void modem_init(void)
{
	dsp_rtty_zero_if_configure(& rx_stream, ARMSAIRATE, - 450, 50);
	//dsp_rtty_zero_if_configure(& rx_stream, ARMSAIRATE, 850, 200);
}


#endif /* WITHINTEGRATEDDSP */


