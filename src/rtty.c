#include "hardware.h"

#if WITHINTEGRATEDDSP && 1

#include "dspdefines.h"
#include "audio.h"
#include "serial.h"
#include "buffers.h"
#include "formats.h"
#include "display/display.h"

#define RTTY_STATE_IDLE         0
#define RTTY_STATE_START_BIT    1
#define RTTY_STATE_DATA_BITS    2
#define RTTY_STATE_STOP_BIT     3

/* Frequency Detector (Discriminator) sub-layer state memory structure */
typedef struct {
    FLOAT_t prev_in_i;           /* Historical real memory from previous sample */
    FLOAT_t prev_in_q;           /* Historical imaginary memory from previous sample */
    FLOAT_t lpf_state;           /* Leaky integrator filter memory envelope */
    FLOAT_t lpf_alpha;           /* Smoothing ratio optimized for baud carrier */
} rtty_freq_detector_t;

/* Baudot Asynchronous FSM sub-layer state machine driven by Integer NCO */
typedef struct {
    uint32_t fsm_state;          /* Active UART/Baudot framing state tracking mode */
    uint32_t nco_accumulator;    /* 32-bit fixed-point integer phase accumulator (Q32) */
    uint32_t nco_step;           /* 32-bit phase step matching the exact baud rate */
    uint32_t bit_shifter;        /* Shift register collecting raw payload streams */
    uint32_t bits_count;         /* Number of successfully accumulated data bits */
    uint32_t is_figures_case;    /* Case shifting matrix tracking configuration flag */
} rtty_baudot_fsm_t;

/* Main unified RTTY receiver containing isolated processing sub-layers as fields */
typedef struct {
    rtty_freq_detector_t detector; /* Embedded frequency discriminator core */
    rtty_baudot_fsm_t    fsm;      /* Embedded integer NCO asynchronous framing engine */
} rtty_receiver_t;

/**
 * @brief SUB-INIT 1: Initializes the differential frequency detector sub-layer.
 */
static void dsp_rtty_sub_init_detector(
    rtty_freq_detector_t * const self,
    const uint32_t sample_rate,
    const FLOAT_t baud_rate)
{
    self->prev_in_i = 0.0;
    self->prev_in_q = 0.0;
    self->lpf_state = 0.0;

    /* Calculate low-pass envelope cutoff matching expected symbol width */
    const FLOAT_t samples_per_bit = (FLOAT_t)sample_rate / baud_rate;
    self->lpf_alpha = 2.0 / samples_per_bit;
}

/**
 * @brief SUB-INIT 2: Initializes the Baudot bit receiver FSM sub-layer.
 */
static void dsp_rtty_sub_init_fsm(
    rtty_baudot_fsm_t * const self,
    const uint32_t sample_rate,
    const FLOAT_t baud_rate)
{
    self->fsm_state = RTTY_STATE_IDLE;
    self->nco_accumulator = 0;
    self->bit_shifter = 0;
    self->bits_count = 0;
    self->is_figures_case = 0;

    /* Compute precise 32-bit integer NCO step per single hardware sample tick (Q32) */
    const FLOAT_t ratio = baud_rate / (FLOAT_t)sample_rate;
    self->nco_step = (uint32_t)(ratio * 4294967296.0);
}

/**
 * @brief MAIN UNIFIED INITIALIZER: Sequentially calls independent sub-layer init functions.
 * @param self Pointer to the active unified hierarchical receiver context.
 * @param sample_rate Input hardware sample clock (typically 48000).
 * @param baud_rate Modulation speed (typically 45.45, 50, or 75 Baud).
 */
static void dsp_rtty_rx_init(
    rtty_receiver_t * const self,
    const uint32_t sample_rate,
    const FLOAT_t baud_rate)
{
    /* Initialize frequency discriminator sub-layer using nested detector object pointer */
    dsp_rtty_sub_init_detector(&self->detector, sample_rate, baud_rate);

    /* Initialize asynchronous integer NCO bit framing receiver sub-layer pointer */
    dsp_rtty_sub_init_fsm(&self->fsm, sample_rate, baud_rate);
}

/**
 * @brief Executes fast differential cross-product frequency discrimination.
 * @return uint32_t Returns 1 for MARK (positive frequency), 0 for SPACE (negative frequency).
 */
static uint32_t dsp_rtty_detector_process(
    rtty_freq_detector_t * const self,
    const FLOAT_t in_i,
    const FLOAT_t in_q)
{
    /* Complex conjugate vector product to calculate instantaneous frequency deviation */
    const FLOAT_t phase_error = in_q * self->prev_in_i - in_i * self->prev_in_q;

    /* Update historical sample memory blocks */
    self->prev_in_i = in_i;
    self->prev_in_q = in_q;

    /* Smooth the fast error discriminator output to strip off-band noise spikes */
    self->lpf_state += self->lpf_alpha * (phase_error - self->lpf_state);

    /* Hard slicing boundary decision */
    return (self->lpf_state >= 0.0) ? 1 : 0;
}

/**
 * @brief SUB-FUNCTION 1: Differential cross-product frequency discriminator.
 * @return uint32_t Returns 1 for MARK (positive frequency), 0 for SPACE (negative frequency).
 */
static uint32_t dsp_rtty_sub_execute_discriminator(
	rtty_freq_detector_t * const self,
    const FLOAT_t in_i,
    const FLOAT_t in_q)
{
    /* Instantaneous frequency tracking via complex conjugate vector multiplication */
    const FLOAT_t phase_error = in_q * self->prev_in_i - in_i * self->prev_in_q;

    /* Save active samples to history buffers */
    self->prev_in_i = in_i;
    self->prev_in_q = in_q;

    /* Smooth error discriminator output to clear off-band noise spikes */
    self->lpf_state += self->lpf_alpha * (phase_error - self->lpf_state);

    /* Hard slicing decision boundary (MARK frequency vs SPACE frequency) */
    return (self->lpf_state >= 0.0) ? 1 : 0;
}

/* Strictly bounded element-by-element configuration of ITA2 Baudot matrices */
static const uint8_t rtty_ita2_letters [32] = {
    ' ', ' ', 'E', '\n', 'A', ' ', 'S', 'I',
    'U', '\r', 'D', 'R',  'J', 'N', 'F', 'C',
    'K', 'T', 'Z', 'L',  'W', 'H', 'Y', 'P',
    'Q', 'O', 'B', 'G',  ' ', 'M', 'X', 'V'
};

static const uint8_t rtty_ita2_figures [32] = {
    ' ', ' ', '3', '\n', '-', ' ', '8', '7',
    '\'', '7', '4', ' ', '4', '5', ',', ':',
    '!', '?', ' ', '"',  '9', '0', '+', '1',
    '5', '6', '1', ' ',  '1', '2', ' ', ' '
};

static const char RTTY_Letters[32] = {
	'\0', 'E', '\n', 'A', ' ', 'S', 'I', 'U',
	'\r', 'D', 'R', 'J', 'N', 'F', 'C', 'K',
	'T', 'Z', 'L', 'W', 'H', 'Y', 'P', 'Q',
	'O', 'B', 'G', ' ', 'M', 'X', 'V', ' ',
};

static const char RTTY_Symbols[32] = {
	'\0', '3', '\n', '-', ' ', '\a', '8', '7',
	'\r', '$', '4', '\'', ',', '!', ':', '(',
	'5', '"', ')', '2', '#', '6', '0', '1',
	'9', '?', '&', ' ', '.', '/', ';', ' ',
};

/**
 * @brief Initializes the Baudot FSM deserializer and computes integer NCO phase step.
 */
static void dsp_rtty_fsm_init(
    rtty_baudot_fsm_t * const self,
    const uint32_t sample_rate,
    const FLOAT_t baud_rate)
{
    self->fsm_state = RTTY_STATE_IDLE;
    self->nco_accumulator = 0;
    self->bit_shifter = 0;
    self->bits_count = 0;
    self->is_figures_case = 0;

    /* Compute precise integer NCO step per single hardware sample tick (Q32) */
    /* step = (baud_rate / sample_rate) * 4294967296.0 */
    const FLOAT_t ratio = baud_rate / (FLOAT_t)sample_rate;
    self->nco_step = (uint32_t)(ratio * 4294967296.0);
}

/**
 * @brief Asynchronous Baudot FSM driven by an integer NCO phase accumulator.
 * @param raw_bit Sliced polar bit input (1 or 0) arriving from the frequency detector.
 */
static void dsp_rtty_fsm_process_sample(
    rtty_baudot_fsm_t * const self,
    const uint32_t raw_bit,
    void (* const put_char_cb)(const uint8_t character))
{
    switch (self->fsm_state)
    {
        case RTTY_STATE_IDLE:
            /* RTTY idle state is MARK (1). Transition to SPACE (0) triggers START bit execution */
            if (raw_bit == 0)
            {
                self->fsm_state = RTTY_STATE_START_BIT;
                /* Align integer NCO to sample exactly at the mid-point of the start bit pulse */
                self->nco_accumulator = 0x80000000; /* Pre-bias phase to 50% window */
            }
            break;

        case RTTY_STATE_START_BIT:
            /* Accumulate phase step continuously */
            self->nco_accumulator += self->nco_step;

            /* Check if integer overflow occurred (Phase accumulator wrapped around = bit boundary) */
            if (self->nco_accumulator < self->nco_step)
            {
                /* Verify that start bit remains valid SPACE (0) at its midpoint boundary */
                if (raw_bit == 0)
                {
                    self->fsm_state = RTTY_STATE_DATA_BITS;
                    self->bits_count = 0;
                    self->bit_shifter = 0;
                    self->nco_accumulator = 0; /* Reset phase grid for next data bits */
                }
                else
                {
                    self->fsm_state = RTTY_STATE_IDLE; /* Spurious start trigger reset */
                }
            }
            break;

        case RTTY_STATE_DATA_BITS:
            self->nco_accumulator += self->nco_step;

            if (self->nco_accumulator < self->nco_step)
            {
                /* Sample and shift incoming payload bit into the shift register (LSB first) */
                self->bit_shifter |= (raw_bit << self->bits_count);
                self->bits_count++;

                /* Process frame compilation if all 5 standalone ITA2 data bits are aggregated */
                if (self->bits_count >= 5)
                {
                    self->fsm_state = RTTY_STATE_STOP_BIT;
                    self->nco_accumulator = 0;

                    /* --- CONDITIONAL BAUDOT ITA2 ALPHABET TRANSULATION LAYER --- */
                    const uint32_t raw_code = self->bit_shifter & 0x1F;

                    if (raw_code == 0x1F)
                    {
                        self->is_figures_case = 0; /* LETTERS escape code vector reached */
                    }
                    else if (raw_code == 0x1B)
                    {
                        self->is_figures_case = 1; /* FIGURES escape code vector reached */
                    }
                    else if (raw_code > 0x00)
                    {
                        /* Extract ASCII value using active case matrix selection pointer */
                        const uint8_t ascii_char = self->is_figures_case ?
                                                    rtty_ita2_figures[raw_code] :
                                                    rtty_ita2_letters[raw_code];

                        if (ascii_char != ' ')
                        {
                            put_char_cb(ascii_char);
                        }
                    }
                }
            }
            break;

        case RTTY_STATE_STOP_BIT:
            self->nco_accumulator += self->nco_step;

            if (self->nco_accumulator < self->nco_step)
            {
                /* Stop bit period successfully executed, return to scanning mode */
                self->fsm_state = RTTY_STATE_IDLE;
            }
            break;

        default:
            self->fsm_state = RTTY_STATE_IDLE;
            break;
    }
}

/**
 * @brief SUB-FUNCTION 2: Asynchronous Baudot bit receiver driven by integer NCO phase accumulator.
 * @param raw_bit Sliced polar bit input (1 or 0) arriving from the discriminator stage.
 */
static void dsp_rtty_sub_execute_fsm(
	rtty_baudot_fsm_t * const self,
    const uint32_t raw_bit,
    void (* const put_char_cb)(const uint8_t character))
{
    switch (self->fsm_state)
    {
        case RTTY_STATE_IDLE:
            /* RTTY idle state is MARK (1). Transition to SPACE (0) triggers START bit execution */
            if (raw_bit == 0)
            {
                self->fsm_state = RTTY_STATE_START_BIT;
                /* Align 32-bit integer NCO to sample exactly at the mid-point of the start pulse width */
                self->nco_accumulator = 0x80000000; /* Pre-bias phase register to 50% grid window */
            }
            break;

        case RTTY_STATE_START_BIT:
            /* Accumulate fixed integer step continuously */
            self->nco_accumulator += self->nco_step;

            /* Verify if integer overflow occurred (Phase register wrapped around = bit boundary reached) */
            if (self->nco_accumulator < self->nco_step)
            {
                /* Verify that start bit remains valid SPACE (0) at its midpoint boundary marker */
                if (raw_bit == 0)
                {
                    self->fsm_state = RTTY_STATE_DATA_BITS;
                    self->bits_count = 0;
                    self->bit_shifter = 0;
                    self->nco_accumulator = 0; /* Reset phase grid loop for next data bits stream */
                }
                else
                {
                    self->fsm_state = RTTY_STATE_IDLE; /* Spurious start noise trigger correction */
                }
            }
            break;

        case RTTY_STATE_DATA_BITS:
            self->nco_accumulator += self->nco_step;

            if (self->nco_accumulator < self->nco_step)
            {
                /* Sample and shift incoming payload bit into the shift register (LSB first) */
                self->bit_shifter |= (raw_bit << self->bits_count);
                self->bits_count++;

                /* Process frame compilation if all 5 standalone ITA2 data bits are aggregated */
                if (self->bits_count >= 5)
                {
                    self->fsm_state = RTTY_STATE_STOP_BIT;
                    self->nco_accumulator = 0;

                    /* --- CONDITIONAL BAUDOT ITA2 ALPHABET TRANSLATION LAYER --- */
                    const uint32_t raw_code = self->bit_shifter & 0x1F;

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
                        /* Extract valid ASCII code via selection of the active tracking case matrix */
                        const uint8_t ascii_char = self->is_figures_case ?
                                                    rtty_ita2_figures[raw_code] :
                                                    rtty_ita2_letters[raw_code];

                        if (ascii_char != ' ')
                        {
                            put_char_cb(ascii_char);
                        }
                    }
                }
            }
            break;

        case RTTY_STATE_STOP_BIT:
            self->nco_accumulator += self->nco_step;

            if (self->nco_accumulator < self->nco_step)
            {
                /* Stop bit period successfully executed, safely return to scanning mode */
                self->fsm_state = RTTY_STATE_IDLE;
            }
            break;

        default:
            self->fsm_state = RTTY_STATE_IDLE;
            break;
    }
}

/**
 * @brief MAIN UNIFIED WRAPPER FUNCTION: Sequentially executes discriminator and pipes the result into the bit FSM.
 * @param self Pointer to the active unified receiver context.
 * @param in_i Incoming real analytical I quadrature from Zero-IF mixer.
 * @param in_q Incoming imaginary analytical Q quadrature from Zero-IF mixer.
 * @param put_char_cb User hot-path callback to output completed text characters.
 */
static void dsp_rtty_rx_process_sample(
    rtty_receiver_t * const self,
    const FLOAT_t in_i,
    const FLOAT_t in_q,
    void (* const put_char_cb)(const uint8_t character))
{
    /* Step 1: Call the frequency discriminator sub-layer to extract the current sliced bit value */
    const uint32_t raw_bit = ! dsp_rtty_sub_execute_discriminator(&self->detector, in_i, in_q);

    /* Step 2: Immediately pipe the extracted bit into the integer NCO asynchronous framing receiver */
    dsp_rtty_sub_execute_fsm(&self->fsm, raw_bit, put_char_cb);
}

//////////////

static void rxcharacter(const uint8_t c)
{
	HARDWARE_DEBUG_PUTCHAR(c);
}

static rtty_receiver_t rx_stream;


void modem_parse(const IFADCvalue_t * buff)
{
	const adapter_t * const ap = & ifcodecrx;
	const FLOAT_t i = adpt_input(ap, buff [DMABUF32RX0I]);
	const FLOAT_t q = adpt_input(ap, buff [DMABUF32RX0Q]);
	dsp_rtty_rx_process_sample(& rx_stream, i, q, rxcharacter);
}

void modem_init(void)
{
	dsp_rtty_rx_init(& rx_stream, ARMSAIRATE, 50);
}


#endif /* WITHINTEGRATEDDSP */


#if WITHRTTY && WITHIF4DSP


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

void RTTYDecoder_Init2(int_fast32_t centerFreq, int_fast32_t RTTY_Speed10, int_fast32_t RTTY_Shift)
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
	/*mark_mag *= -1;

	// summing the two lines
	v1 = mark_mag + space_mag;

	// lowpass filtering the summed line
	arm_biquad_cascade_df2T_f32(&RTTY_LPF_Filter, &v1, &v1, 1);*/

	return (v1 > 0) ? 0 : 1;
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
						self->byteResult |= (bitResult ? 1 : 0) << (self->byteResult_bnum - 1);
					}
					self->byteResult_bnum++;
				}
			}
			if (self->byteResult_bnum == 8 && self->state == RTTY_STATE_BIT)
			{
				char charResult;

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
				default:
					switch (self->charSetMode)
					{
					case RTTY_MODE_SYMBOLS:
						charResult = rtty_ita2_figures [self->byteResult + 1];
						break;
					case RTTY_MODE_LETTERS:
					default:
						charResult = rtty_ita2_letters [self->byteResult + 1];
						break;
					}
					//RESULT !!!!
					put_char_cb(charResult);
					break;
				}
				self->state = RTTY_STATE_WAIT_START;
			}
		}
	}
}

////////////////////////
///

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
	RTTYDecoder_Process2(self, bufferIn, len, put_char_vtty);
}

#endif /* WITHRTTY */

