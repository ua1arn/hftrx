#include "hardware.h"

#if WITHINTEGRATEDDSP

#include "dspdefines.h"
#include "arm_math.h"

#define OFDM_NUM_CHANNELS   8    /* Количество рабочих BPSK каналов */
#define FFT_LEN             16   /* Размер FFT/IFFT для ортогональности */
#define CYCLIC_PREFIX_LEN   4    /* Длина циклического префикса (CP) */
#define OFDM_SYMBOL_LEN     (FFT_LEN + CYCLIC_PREFIX_LEN) /* Всего сэмплов в символе = 20 */

/* Карта распределения поднесущих: смещаем рабочие каналы от DC (бины 1..8) */
static const uint8_t subcarrier_map[OFDM_NUM_CHANNELS] = {1, 2, 3, 4, 5, 6, 7, 8};

/* ========================================================================== */
/*                           СТРУКТУРЫ ДАННЫХ                                 */
/* ========================================================================== */

/* Контекст одной поднесущей (канала) */
typedef struct {
    FLOAT_t phase_nco;           /* Петля Костаса: аккумулятор фазы */
    FLOAT_t phase_step_nco;      /* Шаг фазы */
    FLOAT_t costas_kp;           /* Пропорциональный коэффициент */
    FLOAT_t costas_ki;           /* Интегральный коэффициент */
    FLOAT_t costas_integrator;   /* Интегратор петли Костаса */
    
    FLOAT_t phase_lock_metric;   /* Метрика захвата фазы */
    uint32_t is_phase_locked;    /* Флаг захвата */
} ofdm_subcarrier_bpsk_t;

/* Главный контекст OFDM Модема */
typedef struct {
    ofdm_subcarrier_bpsk_t rx_subcarriers[OFDM_NUM_CHANNELS];
    
    /* Экземпляры CMSIS-DSP FFT */
    ARM_MORPH(arm_cfft_instance) cfft_inst;
    
    /* Внутренние комплексные буферы (размер: FFT_LEN * 2, так как [Re, Im, Re, Im...]) */
    FLOAT_t fft_buffer[FFT_LEN * 2];
    FLOAT_t tx_time_buffer[OFDM_SYMBOL_LEN * 2]; /* Временной буфер TX с учетом CP */
    
    /* Индексы/счетчики для потоковой обработки сэмплов */
    uint32_t tx_sample_idx;
    uint32_t rx_sample_idx;
    FLOAT_t rx_time_buffer[OFDM_SYMBOL_LEN * 2]; /* Окно приема кадра */
    
    FLOAT_t alpha_lock;          /* Усреднение метрики захвата */
    uint32_t state;              /* 0 = IDLE, 1 = RX, 2 = TX */
} ofdm_modem_t;

/* Глобальный контекст модема */
static ofdm_modem_t ofdm_modem;

/* ========================================================================== */
/*                         ИНИЦИАЛИЗАЦИЯ И СБРОС                              */
/* ========================================================================== */

void ofdm_modem_init(void)
{
    /* Инициализация структуры CFFT из CMSIS-DSP (размер 16, плавающая точка) */
    ARM_MORPH(arm_cfft_init)(&ofdm_modem.cfft_inst, FFT_LEN);
    
    ofdm_modem.alpha_lock = 0.02f;
    ofdm_modem.tx_sample_idx = 0;
    ofdm_modem.rx_sample_idx = 0;
    ofdm_modem.state = 0;

    /* Инициализация индивидуальных петель Костаса для каждого BPSK-канала */
    for (uint32_t ch = 0; ch < OFDM_NUM_CHANNELS; ch++)
    {
        ofdm_subcarrier_bpsk_t *sub = &ofdm_modem.rx_subcarriers[ch];
        sub->phase_nco = 0.0f;
        sub->phase_step_nco = 0.0f;
        sub->costas_kp = 0.04f;
        sub->costas_ki = 0.0008f;
        sub->costas_integrator = 0.0f;
        sub->phase_lock_metric = 0.0f;
        sub->is_phase_locked = 0;
    }
}

void ofdm_modem_reset(void)
{
    ofdm_modem.tx_sample_idx = 0;
    ofdm_modem.rx_sample_idx = 0;
    
    for (uint32_t i = 0; i < OFDM_SYMBOL_LEN * 2; i++) {
        ofdm_modem.tx_time_buffer[i] = 0.0f;
        ofdm_modem.rx_time_buffer[i] = 0.0f;
    }

    for (uint32_t ch = 0; ch < OFDM_NUM_CHANNELS; ch++)
    {
        ofdm_subcarrier_bpsk_t *sub = &ofdm_modem.rx_subcarriers[ch];
        sub->phase_nco = 0.0f;
        sub->phase_step_nco = 0.0f;
        sub->costas_integrator = 0.0f;
        sub->phase_lock_metric = 0.0f;
        sub->is_phase_locked = 0;
    }
}

/* ========================================================================== */
/*                          ОБРАБОТКА ПЕРЕДАЧИ (TX)                           */
/* ========================================================================== */

/**
 * @brief Модуляция одного сэмпла OFDM (вызывается на частоте дискретизации ЦАП)
 * @param get_bits_cb Колбэк, запрашивающий массив из 8 бит (по одному на канал)
 * @param out_dac_i Выходной сэмпл I
 * @param out_dac_q Выходной сэмпл Q
 */
void ofdm_modem_tx_sample(void (*get_bits_cb)(uint8_t *bits), FLOAT_t *out_dac_i, FLOAT_t *out_dac_q)
{
    /* Если текущий сформированный OFDM символ полностью передан в ЦАП */
    if (ofdm_modem.tx_sample_idx >= OFDM_SYMBOL_LEN)
    {
        ofdm_modem.tx_sample_idx = 0;
        
        /* 1. Очищаем частотный буфер (Re/Im пары) */
        for (uint32_t i = 0; i < FFT_LEN * 2; i++) {
            ofdm_modem.fft_buffer[i] = 0.0f;
        }
        
        /* 2. Опрашиваем источник данных для получения 8 параллельных бит */
        uint8_t tx_bits[OFDM_NUM_CHANNELS] = {0};
        get_bits_cb(tx_bits);
        
        /* 3. Маппинг BPSK на рабочие поднесущие согласно карте subcarrier_map */
        for (uint32_t ch = 0; ch < OFDM_NUM_CHANNELS; ch++)
        {
            uint32_t bin_idx = subcarrier_map[ch];
            /* 1 -> +1.0, 0 -> -1.0 */
            ofdm_modem.fft_buffer[bin_idx * 2]     = tx_bits[ch] ? 1.0f : -1.0f; /* Re */
            ofdm_modem.fft_buffer[bin_idx * 2 + 1] = 0.0f;                       /* Im */
        }
        
        /* 4. Выполняем ОБПФ (IFFT) через CMSIS-DSP CFFT */
        /* Флаг isInverseFFT = 1, флаг bitReverseFlag = 1 */
        ARM_MORPH(arm_cfft)(&ofdm_modem.cfft_inst, ofdm_modem.fft_buffer, 1, 1);
        
        /* 5. Формируем временной кадр с Циклическим Префиксом (CP) */
        /* Копируем последние CYCLIC_PREFIX_LEN сэмплов из fft_buffer в начало tx_time_buffer */
        uint32_t cp_start_bin = FFT_LEN - CYCLIC_PREFIX_LEN;
        for (uint32_t i = 0; i < CYCLIC_PREFIX_LEN; i++)
        {
            ofdm_modem.tx_time_buffer[i * 2]     = ofdm_modem.fft_buffer[(cp_start_bin + i) * 2];
            ofdm_modem.tx_time_buffer[i * 2 + 1] = ofdm_modem.fft_buffer[(cp_start_bin + i) * 2 + 1];
        }
        
        /* Копируем основное тело FFT следом за префиксом */
        for (uint32_t i = 0; i < FFT_LEN; i++)
        {
            ofdm_modem.tx_time_buffer[(CYCLIC_PREFIX_LEN + i) * 2]     = ofdm_modem.fft_buffer[i * 2];
            ofdm_modem.tx_time_buffer[(CYCLIC_PREFIX_LEN + i) * 2 + 1] = ofdm_modem.fft_buffer[i * 2 + 1];
        }
    }
    
    /* Выводим текущий сэмпл в тракт DUC/ЦАП трансивера */
    *out_dac_i = ofdm_modem.tx_time_buffer[ofdm_modem.tx_sample_idx * 2];
    *out_dac_q = ofdm_modem.tx_time_buffer[ofdm_modem.tx_sample_idx * 2 + 1];
    
    ofdm_modem.tx_sample_idx++;
}

/* ========================================================================== */
/*                            ОБРАБОТКА ПРИЕМА (RX)                           */
/* ========================================================================== */

/**
 * @brief Демодуляция одного входного сэмпла (вызывается в прерывании АЦП/DDC)
 * @param in_i Входной IQ сэмпл I
 * @param in_q Входной IQ сэмпл Q
 * @param out_bits Массив куда запишутся 8 декодированных бит (если символ завершен)
 * @return uint32_t Возвращает 1, если OFDM символ принят и биты готовы, иначе 0
 */
uint32_t ofdm_modem_rx_sample(FLOAT_t in_i, FLOAT_t in_q, uint8_t *out_bits)
{
    /* Записываем входящий сэмпл во временное окно кадра */
    ofdm_modem.rx_time_buffer[ofdm_modem.rx_sample_idx * 2]     = in_i;
    ofdm_modem.rx_time_buffer[ofdm_modem.rx_sample_idx * 2 + 1] = in_q;
    ofdm_modem.rx_sample_idx++;
    
    /* Ждем, пока наберется полный символ (Окно + Циклический префикс) */
    if (ofdm_modem.rx_sample_idx >= OFDM_SYMBOL_LEN)
    {
        ofdm_modem.rx_sample_idx = 0; /* Сброс для следующего символа */
        
        /* 1. Удаляем циклический префикс: переносим данные из полезного окна (после CP) в FFT буфер */
        for (uint32_t i = 0; i < FFT_LEN; i++)
        {
            ofdm_modem.fft_buffer[i * 2]     = ofdm_modem.rx_time_buffer[(CYCLIC_PREFIX_LEN + i) * 2];
            ofdm_modem.fft_buffer[i * 2 + 1] = ofdm_modem.rx_time_buffer[(CYCLIC_PREFIX_LEN + i) * 2 + 1];
        }
        
        /* 2. Выполняем Прямое БПФ (FFT) */
        /* Флаг isInverseFFT = 0, флаг bitReverseFlag = 1 */
        ARM_MORPH(arm_cfft)(&ofdm_modem.cfft_inst, ofdm_modem.fft_buffer, 0, 1);
        
        /* 3. Обработка каждой из 8 поднесущих индивидуальными петлями Костаса */
        for (uint32_t ch = 0; ch < OFDM_NUM_CHANNELS; ch++)
        {
            uint32_t bin_idx = subcarrier_map[ch];
            ofdm_subcarrier_bpsk_t *sub = &ofdm_modem.rx_subcarriers[ch];
            
            /* Вытаскиваем комплексное значение сопряженного бина */
            FLOAT_t raw_i = ofdm_modem.fft_buffer[bin_idx * 2];
            FLOAT_t raw_q = ofdm_modem.fft_buffer[bin_idx * 2 + 1];
            
            /* Деротация фазы индивидуальным NCO канала */
            FLOAT_t cos_p = COSF(sub->phase_nco);
            FLOAT_t sin_p = SINF(sub->phase_nco);
            
            /* Комплексное перемножение для компенсации фазового сдвига */
            FLOAT_t derot_i = raw_i * cos_p + raw_q * sin_p;
            FLOAT_t derot_q = raw_q * cos_p - raw_i * sin_p;
            
            /* Вычисление фазовой ошибки детектора Костаса для BPSK: e = I * Q */
            FLOAT_t error_c = derot_i * derot_q;
            
            /* Интегрирование ошибки частоты/фазы */
            sub->costas_integrator += error_c * sub->costas_ki;
            sub->phase_step_nco = error_c * sub->costas_kp + sub->costas_integrator;
            
            /* Расчет метрики захвата фазы поднесущей: (I^2 - Q^2) / (I^2 + Q^2) */
            FLOAT_t i2 = derot_i * derot_i;
            FLOAT_t q2 = derot_q * derot_q;
            FLOAT_t instant_metric = 0.0f;
            if (i2 + q2 > 0.0f) {
                instant_metric = (i2 - q2) / (i2 + q2);
            }
            sub->phase_lock_metric += ofdm_modem.alpha_lock * (instant_metric - sub->phase_lock_metric);
            sub->is_phase_locked = (sub->phase_lock_metric > 0.55f) ? 1 : 0;
            
            /* Жёсткое решение по знаку (BPSK Слайсер): I >= 0 -> 1, I < 0 -> 0 */
            out_bits[ch] = (derot_i >= 0.0f) ? 1 : 0;
            
            /* Обновление фазы NCO для данного канала */
            sub->phase_nco += sub->phase_step_nco;
            if (sub->phase_nco >= 2.0f * M_PI) sub->phase_nco -= 2.0f * M_PI;
            if (sub->phase_nco < 0.0f) sub->phase_nco += 2.0f * M_PI;
        }
        
        return 1; /* OFDM символ успешно обработан, 8 бит готовы к выдаче */
    }
    
    return 0; /* Символ еще собирается */
}

#endif /* WITHINTEGRATEDDSP */
