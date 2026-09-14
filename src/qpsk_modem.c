#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if WITHIF4DSP
#include "dspdefines.h"
#include "audio.h"
#include "formats.h"

/* Standard QPSK Constellation point scaling: 1 / sqrt(2) ≈ 0.7071067811865475 */
#define QPSK_VAL   M_SQRT1_2 // 0.7071067811865475

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
// Макросы для подстройки петли (подберите под вашу скорость)
#define TIMING_LOOP_KP   0.005f
#define TIMING_LOOP_KI   0.00005f

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
    FLOAT_t nominal_step;         // Базовый шаг (Symbol Rate / Sample Rate)

} qpsk_rx_timing_recovery_t;


/* Loop Filter Proportional (Kp) and Integral (Ki) gains for carrier recovery tracking */
#define CARRIER_LOOP_KP   0.05
#define CARRIER_LOOP_KI   0.0005

/* Unified Structure containing the entire state of the QPSK Costas Loop */
typedef struct {
	FLOAT_t frequency;
    FLOAT_t phase;          // Текущая фаза опорного генератора (в радианах, от -PI до +PI)
    FLOAT_t freq_error;     // Накопленная ошибка частоты (интегральный накопитель петли)

    // Коэффициенты пропорционально-интегрального (PI) фильтра петли Костаса
    FLOAT_t kp;             // Пропорциональный коэффициент (боевое значение ~0.02f ... 0.05f)
    FLOAT_t ki;             // Интегральный коэффициент (боевое значение ~0.0002f ... 0.0005f)

    // Поля для отладки и мониторинга созвездия (опционально, но крайне полезно)
    FLOAT_t last_phase_error; // Последняя вычисленная ошибка фазы детектором Костаса
    uint32_t lock_counter;    // Счетчик захвата (для определения, синхронизирован ли приёмник)
} qpsk_rx_carrier_recovery_t;


static qpsk_rx_carrier_recovery_t carrier_loop;
static qpsk_rx_timing_recovery_t timing_loop;

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
 * @brief Петля Костаса (Carrier Recovery) для компенсации сдвига частоты и фазы
 * @param loop Указатель на структуру состояния петли
 * @param strobe_i Входной синфазный отсчет от интерполятора Фэрроу
 * @param strobe_q Входной квадратурный отсчет от интерполятора Фэрроу
 * @param out_i Указатель для записи скорректированного значения I
 * @param out_q Указатель для записи скорректированного значения Q
 */
void qpsk_carrier_recovery_process(
    qpsk_rx_carrier_recovery_t* loop,
    FLOAT_t strobe_i,
    FLOAT_t strobe_q,
    FLOAT_t* out_i,
    FLOAT_t* out_q)
{
    // 1. Вычисляем синус и косинус текущей фазы опорного генератора
    // (Для оптимизации на ARM Cortex-M4/M7 лучше использовать arm_sin_f32 / arm_cos_f32)
	FLOAT_t sin_p = arm_sin_f32(loop->phase);
	FLOAT_t cos_p = arm_cos_f32(loop->phase);

    // 2. Поворот созвездия (Демодуляция / Умножение на опорный сигнал)
    // Корректируем фазу входного сэмпла
    FLOAT_t i_rot =  strobe_i * cos_p + strobe_q * sin_p;
    FLOAT_t q_rot = -strobe_i * sin_p + strobe_q * cos_p;

    *out_i = i_rot;
    *out_q = q_rot;

    // 3. Жесткое решение (Hard Decision) для вычисления знака
    FLOAT_t sign_i = (i_rot >= 0.0f) ? 1.0f : -1.0f;
    FLOAT_t sign_q = (q_rot >= 0.0f) ? 1.0f : -1.0f;

    // 4. Детектор фазовой ошибки Костаса (Costas Phase Error Detector для QPSK)
    // Формула: Error = sign(I) * Q - sign(Q) * I
    FLOAT_t phase_error = sign_i * q_rot - sign_q * i_rot;

    // 5. Фильтр петли (PI-регулятор)
    // Интегральная часть (коррекция частоты)
    loop->freq_error += loop->ki * phase_error;

    // Ограничение ухода частоты (Anti-windup), чтобы петля не улетала при долгом отсутствии сигнала
    if (loop->freq_error > 0.1f)  loop->freq_error = 0.1f;
    if (loop->freq_error < -0.1f) loop->freq_error = -0.1f;

    // Пропорционально-интегральное обновление фазы опорного генератора
    loop->phase += (loop->kp * phase_error) + loop->freq_error;

    // Приведение фазы к диапазону [-PI, +PI] или [0, 2*PI]
    if (loop->phase > (FLOAT_t)M_PI) {
        loop->phase -= 2.0f * (FLOAT_t)M_PI;
    } else if (loop->phase < -(FLOAT_t)M_PI) {
        loop->phase += 2.0f * (FLOAT_t)M_PI;
    }
}

/**
 * @brief Внутренний кубический интерполятор Фэрроу для interleaved IQ потока
 * @param base_ptr Указатель на начало массива FLOAT_t (указывает на I-компоненту)
 * @param base_idx Индекс текущего комплексного сэмпла (k)
 * @param mu Дробный интервал [0.0 ... 1.0) между сэмплами k и k+1
 * @param offset 0 для канала I, 1 для канала Q (так как данные interleaved: I0, Q0, I1, Q1...)
 */
static inline FLOAT_t farrow_interpolate_iq(const FLOAT_t* base_ptr, int base_idx, FLOAT_t mu, int offset)
{
    // Шаг (stride) равен 2, так как I и Q чередуются
    // Извлекаем 4 комплексные точки вокруг расчетного места: (base_idx - 1), base_idx, (base_idx + 1), (base_idx + 2)
    FLOAT_t v0 = base_ptr[((base_idx - 1) * 2) + offset];
    FLOAT_t v1 = base_ptr[((base_idx)     * 2) + offset];
    FLOAT_t v2 = base_ptr[((base_idx + 1) * 2) + offset];
    FLOAT_t v3 = base_ptr[((base_idx + 2) * 2) + offset];

    // Вычисление полиномиальных коэффициентов Фэрроу (схема Лагранжа)
    FLOAT_t c0 = v1;
    FLOAT_t c1 = -0.5f * v0 + 0.5f * v2;
    FLOAT_t c2 = v0 - 2.5f * v1 + 2.0f * v2 - 0.5f * v3;
    FLOAT_t c3 = -0.5f * v0 + 1.5f * v1 - 1.5f * v2 + 0.5f * v3;

    // Вычисление значения по схеме Горнера: ((c3 * mu + c2) * mu + c1) * mu + c0
    return ((c3 * mu + c2) * mu + c1) * mu + c0;
}

// Состояние упаковщика бит (сбрасывать в ноль перед началом приёма блока)
static uint8_t  tx_rx_bit_accumulator = 0; // Накопитель текущего байта
static int      tx_rx_bit_count = 0;       // Сколько бит уже упаковано в текущий байт (0..7)
static int      rx_payload_byte_idx = 0;   // Индекс текущего байта в выходном массиве


/* ========================================================================= */
/* CONFIGURATION METRICS AND MEMORY POOL CALCULATIONS                        */
/* ========================================================================= */

#define MODEM_RRC_TAPS          511//49   /* RRC filter length (typically 6 symbols * 8 sps + 1) */
#define MODEM_TX_BLOCK_SAMPLES  40960//64   /* Number of complex I/Q samples processed per TX FIR iteration */
#define MODEM_RX_BLOCK_SAMPLES  40960//128  /* Number of complex I/Q samples processed per RX DMA hardware interrupt */

/* Maximum application data capacity for a single transmission burst transaction */
#define APP_MAX_DATA_BYTES      40960//32

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

/**
 * @brief Упаковка принятых бит I и Q каналов в байты (MSB-first, Gray coded)
 * @param bit_i Бит из синфазного канала (0 или 1)
 * @param bit_q Бит из квадратурного канала (0 или 1)
 */
void app_modem_push_bits_to_payload(uint8_t bit_i, uint8_t bit_q)
{
    // Защита от выхода за границы буфера полезной нагрузки
    // Максимальный размер буфера в байтах равен: MODEM_RX_BLOCK_SAMPLES / 32
    // (Поскольку на 1 символ приходится 2 бита, а при SPS=8 на байт уходит 32 сэмпла)
    if (rx_payload_byte_idx >= (MODEM_RX_BLOCK_SAMPLES / 32)) {
        return;
    }

    // --- Обработка бита канала I (Старший бит дибита) ---
    tx_rx_bit_accumulator <<= 1;
    if (bit_i) {
        tx_rx_bit_accumulator |= 0x01;
    }
    tx_rx_bit_count++;

    // Если накопили полный байт (8 бит), сбрасываем его в буфер payload
    if (tx_rx_bit_count >= 8) {
        rx_decoded_data_payload[rx_payload_byte_idx++] = tx_rx_bit_accumulator;
        tx_rx_bit_accumulator = 0;
        tx_rx_bit_count = 0;
    }

    // Защитная проверка перед обработкой второго бита
    if (rx_payload_byte_idx >= (MODEM_RX_BLOCK_SAMPLES / 32)) {
        return;
    }

    // --- Обработка бита канала Q (Младший бит дибита) ---
    tx_rx_bit_accumulator <<= 1;
    if (bit_q) {
        tx_rx_bit_accumulator |= 0x01;
    }
    tx_rx_bit_count++;

    // Если накопили полный байт, сбрасываем его в буфер payload
    if (tx_rx_bit_count >= 8) {
        rx_decoded_data_payload[rx_payload_byte_idx++] = tx_rx_bit_accumulator;
        tx_rx_bit_accumulator = 0;
        tx_rx_bit_count = 0;
    }
}

/**
 * @brief Петля синхронизации символов для некратных скоростей (Gardner TED + Farrow Interpolator)
 * @param loop Указатель на структуру состояния тайминга
 * @param rx_data Входной interleaved массив комплексных сэмплов [I0, Q0, I1, Q1...] после RRC-фильтра
 * @param num_samples Количество комплексных сэмплов в блоке (MODEM_RX_BLOCK_SAMPLES)
 */
void qpsk_rx_timing_process(qpsk_rx_timing_recovery_t* loop, const FLOAT_t* rx_data, int num_samples)
{
    // Цикл идет строго с шагом 1 по физическим отсчетам ЦАП/АЦП.
    // Оставляем запас с краев (от 1 до num_samples - 2) для корректной работы 4-точечного интерполятора.
    for (int k = 1; k < (num_samples - 2); k++)
    {
        // 1. Накапливаем плавающую фазу.
        // К номинальному шагу (например, 7200/48000 = 0.15) прибавляется интегральная поправка частоты кварца.
        FLOAT_t current_step = loop->nominal_step + loop->loop_integrator;
        loop->fractional_symbol_idx += current_step;

        // 2. Условие пересечения границы символа (Strobe Point)
        if (loop->fractional_symbol_idx >= 1.0f)
        {
            loop->fractional_symbol_idx -= 1.0f;

            // Вычисляем mu: точное положение пика символа в подпространстве между отсчетами k и k+1
            FLOAT_t mu = loop->fractional_symbol_idx / current_step;

            // 3. Интерполируем СТРОБ-ОТСЧЕТ (Центр «глазка» диаграммы) для I и Q каналов
            FLOAT_t strobe_i = farrow_interpolate_iq(rx_data, k, mu, 0); // offset = 0 для I
            FLOAT_t strobe_q = farrow_interpolate_iq(rx_data, k, mu, 1); // offset = 1 для Q

            // 4. Интерполируем СРЕДНЮЮ ТОЧКУ (Midpoint) для детектора Гарднера.
            // Она должна отставать ровно на половину длительности символа (сдвиг по фазе на -0.5).
            FLOAT_t mu_mid = mu - 0.5f;
            int k_mid = k;

            // Если mu_mid ушел в отрицательную зону, сдвигаем базовый индекс k на 1 отсчет назад
            if (mu_mid < 0.0f) {
                mu_mid += 1.0f;
                k_mid -= 1;
            }
            FLOAT_t midpoint_i = farrow_interpolate_iq(rx_data, k_mid, mu_mid, 0);
            FLOAT_t midpoint_q = farrow_interpolate_iq(rx_data, k_mid, mu_mid, 1);

            // 5. Детектор временной ошибки Гарднера (Gardner Timing Error Detector)
            // Измеряет разность амплитуд между текущим и прошлым стробом, взвешенную по средней точке
            const FLOAT_t timing_error = midpoint_i * (strobe_i - loop->prev_strobe_i) +
                                         midpoint_q * (strobe_q - loop->prev_strobe_q);

            // 6. Пропорционально-Интегральный (PI) фильтр петли синхронизации
            // Интегральная часть корректирует частоту (убирает статическую ошибку)
            loop->loop_integrator += (TIMING_LOOP_KI * timing_error);

            // Защита от разноса частоты (Anti-windup): ограничиваем уход в пределах +/- 2% от номинала
            FLOAT_t max_freq_deviation = loop->nominal_step * 0.02f;
            if (loop->loop_integrator > max_freq_deviation)  loop->loop_integrator = max_freq_deviation;
            if (loop->loop_integrator < -max_freq_deviation) loop->loop_integrator = -max_freq_deviation;

            // Пропорциональный сдвиг фазы применяется напрямую к накопителю для мгновенной подстройки
            loop->fractional_symbol_idx += (TIMING_LOOP_KP * timing_error);

            // Сохраняем текущие значения стробов для следующего шага детектора
            loop->prev_strobe_i = strobe_i;
            loop->prev_strobe_q = strobe_q;

            // 7. Передаем интерполированные стробы на компенсацию несущей (Петлю Костаса)
            FLOAT_t dynamic_i = 0.0f;
            FLOAT_t dynamic_q = 0.0f;

            // Вызываем вашу обновленную qpsk_carrier_recovery_process
            qpsk_carrier_recovery_process(&carrier_loop, strobe_i, strobe_q, &dynamic_i, &dynamic_q);

            // 8. Жесткое решение и упаковка бит (демодуляция данных)
            uint8_t bit_i = (dynamic_i >= 0.0f) ? 1 : 0;
            uint8_t bit_q = (dynamic_q >= 0.0f) ? 1 : 0;

            // Функция, которая складывает биты в байты внутри rx_decoded_data_payload
            app_modem_push_bits_to_payload(bit_i, bit_q);
        }
    }
}

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

// Внешние буферы и структуры состояния из вашего проекта
static FLOAT_t rx_upsample_workspace[MODEM_RX_BLOCK_SAMPLES * 2];
static qpsk_rx_timing_recovery_t timing_loop;

/**
 * @brief Главная функция приёма и демодуляции QPSK блока для некратных скоростей
 * @param rx_dma_buffer Входной interleaved буфер от АЦП/DMA [I0, Q0, I1, Q1 ...] (размер: MODEM_RX_BLOCK_SAMPLES * 2)
 * @param rx_decoded_bytes Выходной буфер для демодулированных байт (rx_decoded_data_payload)
 * @return int Количество успешно декодированных БАЙТ в текущем блоке DMA
 */
int qpsk_modem_receive(const FLOAT_t* rx_dma_buffer, uint8_t* rx_decoded_bytes)
{
    // 1. Очистка состояния потокового упаковщика бит перед каждым новым кадром DMA
    tx_rx_bit_accumulator = 0;
    tx_rx_bit_count = 0;
    rx_payload_byte_idx = 0;

    // Безопасно зануляем выходной буфер, чтобы в логах не оставалось старых данных
    memset(rx_decoded_bytes, 0, MODEM_RX_BLOCK_SAMPLES / 32);

    // 2. RRC Согласованная фильтрация (Matched Filtering) Найквиста
    // Входной interleaved-поток пропускается через фильтр для подавления межсимвольной интерференции (ISI).
    // Длина массива составляет MODEM_RX_BLOCK_SAMPLES комплексных сэмплов (умножаем на 2 для FLOAT_t)

#if defined(ARM_MATH_CM4) || defined(ARM_MATH_CM7) || defined(ARM_MATH_H7)
    // Аппаратное ускорение на микроконтроллерах STM32/ARM через CMSIS-DSP
    arm_fir_f32(&rx_filter_instance, (float32_t*)rx_dma_buffer, (float32_t*)rx_upsample_workspace, MODEM_RX_BLOCK_SAMPLES * 2);
#else
    // Вариант для автономных тестов на ПК (где CMSIS-DSP может быть недоступен).
    // Выполняем прямую программную свертку или копирование:
    memcpy(rx_upsample_workspace, rx_dma_buffer, MODEM_RX_BLOCK_SAMPLES * 2 * sizeof(FLOAT_t));
#endif

    // 3. Запуск конвейера синхронизации символов Фэрроу-Гарднера
    // Эта функция посэмпльно шагает по rx_upsample_workspace и внутри себя:
    //   а) Вычисляет mu (дробный сдвиг фазы) для интерполятора Фэрроу.
    //   б) Восстанавливает истинные отсчеты I/Q (Strobe) между сэмплами АЦП.
    //   в) Корректирует уход частоты кварца через детектор Гарднера (Timing Recovery).
    //   г) Вызывает qpsk_carrier_recovery_process (Костас) для компенсации фазы несущей.
    //   д) Вызывает app_modem_push_bits_to_payload для упаковки бит в байты.
    qpsk_rx_timing_process(&timing_loop, rx_upsample_workspace, MODEM_RX_BLOCK_SAMPLES);

    // 4. Возвращаем итоговое количество записанных байт
    // Переменная rx_payload_byte_idx инкрементируется внутри app_modem_push_bits_to_payload на каждый 8-й бит
    return rx_payload_byte_idx;
}

typedef struct {
    FLOAT_t tx_step;                // Шаг фазы (Symbol Rate / Sample Rate)
    FLOAT_t tx_phase;               // Текущая накопленная фаза символа (0.0 ... 1.0)
    int current_symbol_output_idx;  // Индекс текущего символа в буфере передачи
} qpsk_tx_nco_t;

// Инициализация в app_modem_system_setup()
void app_modem_tx_system_setup(qpsk_tx_nco_t* tx, FLOAT_t sample_rate, FLOAT_t symbol_rate)
{
    tx->tx_step = symbol_rate / sample_rate; // Например: 7200.0 / 48000.0 = 0.15
    tx->tx_phase = 0.0f;
    tx->current_symbol_output_idx = 0;
}

// Внешняя структура состояния NCO передатчика (объявлена в qpsk_modem.c)
static qpsk_tx_nco_t tx_nco;

// Внешние буферы из вашего проекта
extern FLOAT_t tx_hardware_output_io[TX_UPSAMPLE_BUF_SIZE];
extern FLOAT_t tx_upsample_workspace[TX_UPSAMPLE_BUF_SIZE];

/* ========================================================================= */
/* APPLICATION LAYER CONTROL IMPLEMENTATION                                  */
/* ========================================================================= */

/* Global static driver context instance */
static qpsk_modem_t hf_digital_modem;

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
    // ... Инициализация окон и буферов
    ARM_MORPH(arm_blackman_harris_92db)(preformed_window_mem, MODEM_RRC_TAPS);

	// 1. Задаем базовые физические частоты тракта
	const FLOAT_t sample_rate = ARMI2SRATE; // Частота дискретизации кодека/ЦАП/АЦП
	const FLOAT_t symbol_rate = 600;//7200.0f;  // Некратная скорость символов (7200 Бод)

	// 2. Инициализация ПЕРЕДАТЧИКА (TX) через NCO
	app_modem_tx_system_setup(&tx_nco, sample_rate, symbol_rate);

    // Номинальный шаг фазы на один входной сэмпл Fs
    timing_loop.nominal_step = symbol_rate / sample_rate;
    timing_loop.fractional_symbol_idx = 0.0f;
    timing_loop.loop_integrator = 0.0f;

	carrier_loop.phase = 0.0f;       // Стартуем с нулевой фазы
	carrier_loop.freq_error = 0.0f;  // Стартуем с нулевого ухода частоты

	// Для некратных скоростей (например, 7200 Бод при 48кГц)
	// оптимально подходят следующие базовые значения:
	carrier_loop.kp = 0.025f;        // Чувствительность к мгновенному сдвигу фазы
	carrier_loop.ki = 0.00025f;      // Скорость подстройки под постоянный уход частоты (кварца)

	carrier_loop.last_phase_error = 0.0f;
	carrier_loop.lock_counter = 0;

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
 * @brief Функция интерполяции тапов RRC-фильтра (для передатчика)
 * Находит точное значение импульсной характеристики Найквиста в промежутке между тапами
 * @param rrc_taps Указатель на массив коэффициентов фильтра (preformed_window_mem)
 * @param num_taps Количество коэффициентов (MODEM_RRC_TAPS, например, 49 или 65)
 * @param mu Дробное смещение фазы от 0.0 до 1.0
 * @param tap_idx Базовый индекс целого тапа
 */
static inline FLOAT_t farrow_interpolate_rrc(const FLOAT_t* rrc_taps, int num_taps, int tap_idx, FLOAT_t mu)
{
    // Защита от выхода за границы массива коэффициентов RRC
    if (tap_idx < 1 || tap_idx >= (num_taps - 2)) {
        return rrc_taps[tap_idx >= 0 && tap_idx < num_taps ? tap_idx : 0];
    }

    // 4 точки вокруг расчетного места в фильтре Найквиста
    FLOAT_t v0 = rrc_taps[tap_idx - 1];
    FLOAT_t v1 = rrc_taps[tap_idx];
    FLOAT_t v2 = rrc_taps[tap_idx + 1];
    FLOAT_t v3 = rrc_taps[tap_idx + 2];

    // Полином Фэрроу (кубический Лагранж)
    FLOAT_t c0 = v1;
    FLOAT_t c1 = -0.5f * v0 + 0.5f * v2;
    FLOAT_t c2 = v0 - 2.5f * v1 + 2.0f * v2 - 0.5f * v3;
    FLOAT_t c3 = -0.5f * v0 + 1.5f * v1 - 1.5f * v2 + 0.5f * v3;

    return ((c3 * mu + c2) * mu + c1) * mu + c0;
}

/**
 * @brief Формирование (модуляция) QPSK сигнала для некратных скоростей методом NCO+Farrow
 * @param txarray Массив исходных байт для передачи
 * @param size Количество байт в массиве txarray
 * @return int Количество сгенерированных элементов FLOAT_t в tx_hardware_output_io (в 2 раза больше комплексных сэмплов)
 */
int app_modem_execute_transmission(const uint8_t * txarray, unsigned int size)
{
    // 1. Сброс состояния NCO перед началом новой передачи
    // Значение tx_nco.tx_step должно быть рассчитано в app_modem_system_setup как (Symbol_Rate / Sample_Rate)
    tx_nco.tx_phase = 0.0f;
    tx_nco.current_symbol_output_idx = 0;

    int total_symbols_to_send = size * 4; // 4 символа QPSK в одном байте (по 2 бита на символ)
    int out_float_idx = 0;

    // Очищаем выходной буфер перед формированием сигнала
    memset(tx_hardware_output_io, 0, sizeof(tx_hardware_output_io));

    // 2. Генерация потока отсчетов на частоте дискретизации ЦАП (Fs)
    // Ограничиваем цикл размером аппаратного TX-буфера
    while (out_float_idx < (TX_UPSAMPLE_BUF_SIZE - 2))
    {
        // Накапливаем фазу символа в NCO
        tx_nco.tx_phase += tx_nco.tx_step;

        // Если фаза пересекает единицу — переключаемся на следующий QPSK символ
        if (tx_nco.tx_phase >= 1.0f)
        {
            tx_nco.tx_phase -= 1.0f;
            tx_nco.current_symbol_output_idx++;
        }

        // Если все символы кончились, даем хвосту фильтра затухнуть (Flush delay line)
        // Групповая задержка RRC составляет MODEM_RRC_TAPS символов.
        if (tx_nco.current_symbol_output_idx >= (total_symbols_to_send + (MODEM_RRC_TAPS / 8)))
        {
            break;
        }

        // 3. Вычисление значащей амплитуды текущего символа с учетом истории (Свертка)
        FLOAT_t accumulated_i = 0.0f;
        FLOAT_t accumulated_q = 0.0f;

        // Символьный шаг оверсемплинга в вещественном выражении (дробный SPS)
        FLOAT_t f_sps = 1.0f / tx_nco.tx_step;

        // Выполняем свертку: смотрим, какие символы из прошлого и будущего влияют на текущий сэмпл ЦАП
        // Окно свертки ограничено длиной RRC фильтра (MODEM_RRC_TAPS)
        int max_lookback_symbols = (int)(MODEM_RRC_TAPS / f_sps) + 1;

        for (int m = -max_lookback_symbols; m <= 0; m++)
        {
            int target_sym_idx = tx_nco.current_symbol_output_idx + m;

            // Если символ находится внутри границ нашего пакета данных
            if (target_sym_idx >= 0 && target_sym_idx < total_symbols_to_send)
            {
                // Извлекаем дибит из байтового массива txarray
                int byte_pos = target_sym_idx / 4;
                int sym_pos = target_sym_idx % 4;
                uint8_t current_byte = txarray[byte_pos];
                uint8_t dibit = (current_byte >> (6 - (sym_pos * 2))) & 0x03;

                // Отображение на QPSK созвездие (Грей-код)
                FLOAT_t symbol_i = (dibit & 0x02) ? 1.0f : -1.0f;
                FLOAT_t symbol_q = (dibit & 0x01) ? 1.0f : -1.0f;

                // Вычисляем, в какую точку RRC фильтра попадает этот символ
                // Дробная часть mu обеспечивает идеальную плавность переходов без джиттера
                FLOAT_t exact_tap_pos = ((FLOAT_t)(-m) + tx_nco.tx_phase) * f_sps;
                int base_tap_idx = (int)exact_tap_pos;
                FLOAT_t mu = exact_tap_pos - (FLOAT_t)base_tap_idx;

                // Дробно интерполируем значение RRC импульса Найквиста
                FLOAT_t rrc_weight = farrow_interpolate_rrc(preformed_window_mem, MODEM_RRC_TAPS, base_tap_idx, mu);

                // Накапливаем вклад символа в текущую точку времени
                accumulated_i += symbol_i * rrc_weight;
                accumulated_q += symbol_q * rrc_weight;
            }
        }

        // 4. Запись interleaved отсчетов в массив для выдачи в DMA/ЦАП
        tx_hardware_output_io[out_float_idx++] = accumulated_i; // Канал I
        tx_hardware_output_io[out_float_idx++] = accumulated_q; // Канал Q
    }

    // Возвращаем общее число сгенерированных float-элементов
    return out_float_idx;
}


/**
 * @brief Обработчик прерывания / Callback DMA для приёма данных модема
 * @param data_out Указатель на interleaved комплексный буфер [I0, Q0, I1, Q1 ...] от АЦП/DMA
 */
static void app_modem_rx_dma_callback_isr(const FLOAT_t * data_out)
{
    // 1. Точка контроля времени (если используется в вашем проекте для замера джиттера/профилирования)
    //TP();

    // 2. Вызов сквозного конвейера приема:
    // Внутри qpsk_modem_receive последовательно выполняются:
    //   - RRC фильтрация Найквиста (арпаратный/программный FIR)
    //   - Интерполяция Фэрроу и петля Гарднера (Timing Recovery)
    //   - Петля Костаса (Carrier Recovery)
    //   - Сборка и упаковка бит в байты (Gray Code, MSB-first)
    const int recovered_bytes_count = qpsk_modem_receive(data_out, rx_decoded_data_payload);

    // 3. Анализ и обработка результатов демодуляции
    if (recovered_bytes_count > 0)
    {
        // Для отладки на ПК или вывода в консоль
        //PRINTF("[DMA ISR] recovered_bytes_count: %d\n", recovered_bytes_count);
        printhex_titled(0, rx_decoded_data_payload, recovered_bytes_count, "rx_decoded_data_payload");

        // Здесь в боевом коде обычно вызывается верхнеуровневый разборщик пакетов,
        // например, поиск синхрослова, проверка CRC или передача в стек AX.25 / HDLC:
        // app_modem_parse_protocol_packet(rx_decoded_data_payload, recovered_bytes_count);
    }
    else
    {
        // Сигнал не обнаружен или петли синхронизации еще не захватили поток (идет шум)
        // В боевом коде здесь можно инкрементировать счетчик ошибок или пропуска кадров
    }

    // 4. Финальная точка контроля времени
    TP();
}

void modem_test(void)
{
	TP();
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
