#include "hardware.h"

#if WITHINTEGRATEDDSP && 1

#include "dspdefines.h"
#include "audio.h"
#include "formats.h"

#define OFDM_NUM_CHANNELS   8
#define FFT_LEN             16
#define CYCLIC_PREFIX_LEN   4
#define OFDM_SYMBOL_LEN     (FFT_LEN + CYCLIC_PREFIX_LEN)

static const uint8_t subcarrier_map[OFDM_NUM_CHANNELS] = {1, 2, 3, 4, 5, 6, 7, 8};

typedef struct {
    FLOAT_t phase_nco;
    FLOAT_t phase_step_nco;
    FLOAT_t costas_kp;
    FLOAT_t costas_ki;
    FLOAT_t costas_integrator;
    FLOAT_t phase_lock_metric;
    uint32_t is_phase_locked;
} ofdm_subcarrier_bpsk_t;

typedef struct {
    ofdm_subcarrier_bpsk_t rx_subcarriers[OFDM_NUM_CHANNELS];
    ARM_MORPH(arm_cfft_instance) cfft_inst;
    FLOAT_t fft_buffer[FFT_LEN * 2];
    FLOAT_t tx_time_buffer[OFDM_SYMBOL_LEN * 2];
    FLOAT_t rx_time_buffer[OFDM_SYMBOL_LEN * 2];
    uint32_t tx_sample_idx;
    uint32_t rx_sample_idx;
    FLOAT_t alpha_lock;
} ofdm_modem_t;

static ofdm_modem_t ofdm_modem;

static void ofdm_modem_init(void)
{
    ARM_MORPH(arm_cfft_init)(&ofdm_modem.cfft_inst, FFT_LEN);
    ofdm_modem.alpha_lock = 0.02;
    ofdm_modem.tx_sample_idx = 0;
    ofdm_modem.rx_sample_idx = 0;

    for (uint32_t ch = 0; ch < OFDM_NUM_CHANNELS; ch++)
    {
        ofdm_subcarrier_bpsk_t *sub = &ofdm_modem.rx_subcarriers[ch];
        sub->phase_nco = 0;
        sub->phase_step_nco = 0;
        sub->costas_kp = 0.04;
        sub->costas_ki = 0.0008;
        sub->costas_integrator = 0;
        sub->phase_lock_metric = 0;
        sub->is_phase_locked = 0;
    }
}

static void ofdm_modem_reset(void)
{
    ofdm_modem.tx_sample_idx = 0;
    ofdm_modem.rx_sample_idx = 0;
    ARM_MORPH(arm_fill)(0, ofdm_modem.tx_time_buffer, OFDM_SYMBOL_LEN * 2);
    ARM_MORPH(arm_fill)(0, ofdm_modem.rx_time_buffer, OFDM_SYMBOL_LEN * 2);

    for (uint32_t ch = 0; ch < OFDM_NUM_CHANNELS; ch++)
    {
        ofdm_subcarrier_bpsk_t *sub = &ofdm_modem.rx_subcarriers[ch];
        sub->phase_nco = 0;
        sub->phase_step_nco = 0;
        sub->costas_integrator = 0;
        sub->phase_lock_metric = 0;
        sub->is_phase_locked = 0;
    }
}

static void ofdm_modem_tx_block(void (*get_bits_cb)(uint8_t *bits), FLOAT_t *out_buffer_i, FLOAT_t *out_buffer_q, uint32_t block_size)
{
    for (uint32_t sample_idx = 0; sample_idx < block_size; sample_idx++)
    {
        if (ofdm_modem.tx_sample_idx >= OFDM_SYMBOL_LEN)
        {
            ofdm_modem.tx_sample_idx = 0;
            ARM_MORPH(arm_fill)(0, ofdm_modem.fft_buffer, FFT_LEN * 2);

            uint8_t tx_bits[OFDM_NUM_CHANNELS] = {0};
            get_bits_cb(tx_bits);

            ofdm_modem.fft_buffer[subcarrier_map[0] * 2] = tx_bits[0] ? 1 : -1;
            ofdm_modem.fft_buffer[subcarrier_map[1] * 2] = tx_bits[1] ? 1 : -1;
            ofdm_modem.fft_buffer[subcarrier_map[2] * 2] = tx_bits[2] ? 1 : -1;
            ofdm_modem.fft_buffer[subcarrier_map[3] * 2] = tx_bits[3] ? 1 : -1;
            ofdm_modem.fft_buffer[subcarrier_map[4] * 2] = tx_bits[4] ? 1 : -1;
            ofdm_modem.fft_buffer[subcarrier_map[5] * 2] = tx_bits[5] ? 1 : -1;
            ofdm_modem.fft_buffer[subcarrier_map[6] * 2] = tx_bits[6] ? 1 : -1;
            ofdm_modem.fft_buffer[subcarrier_map[7] * 2] = tx_bits[7] ? 1 : -1;

            ARM_MORPH(arm_cfft)(&ofdm_modem.cfft_inst, ofdm_modem.fft_buffer, 1, 1);

            uint32_t cp_src_offset = (FFT_LEN - CYCLIC_PREFIX_LEN) * 2;
            ARM_MORPH(arm_copy)(&ofdm_modem.fft_buffer[cp_src_offset], ofdm_modem.tx_time_buffer, CYCLIC_PREFIX_LEN * 2);
            ARM_MORPH(arm_copy)(ofdm_modem.fft_buffer, &ofdm_modem.tx_time_buffer[CYCLIC_PREFIX_LEN * 2], FFT_LEN * 2);
        }
        
        out_buffer_i[sample_idx] = ofdm_modem.tx_time_buffer[ofdm_modem.tx_sample_idx * 2];
        out_buffer_q[sample_idx] = ofdm_modem.tx_time_buffer[ofdm_modem.tx_sample_idx * 2 + 1];
        ofdm_modem.tx_sample_idx++;
    }
}

static void ofdm_modem_rx_block(const FLOAT_t *in_buffer_i, const FLOAT_t *in_buffer_q, uint32_t block_size, void (*process_bits_cb)(const uint8_t *bits))
{
    for (uint32_t sample_idx = 0; sample_idx < block_size; sample_idx++)
    {
        ofdm_modem.rx_time_buffer[ofdm_modem.rx_sample_idx * 2]     = in_buffer_i[sample_idx];
        ofdm_modem.rx_time_buffer[ofdm_modem.rx_sample_idx * 2 + 1] = in_buffer_q[sample_idx];
        ofdm_modem.rx_sample_idx++;
        
        if (ofdm_modem.rx_sample_idx >= OFDM_SYMBOL_LEN)
        {
            ofdm_modem.rx_sample_idx = 0;
            ARM_MORPH(arm_copy)(&ofdm_modem.rx_time_buffer[CYCLIC_PREFIX_LEN * 2], ofdm_modem.fft_buffer, FFT_LEN * 2);
            ARM_MORPH(arm_cfft)(&ofdm_modem.cfft_inst, ofdm_modem.fft_buffer, 0, 1);
            
            uint8_t rx_bits[OFDM_NUM_CHANNELS] = {0};
            
            for (uint32_t ch = 0; ch < OFDM_NUM_CHANNELS; ch++)
            {
                uint32_t bin_idx = subcarrier_map[ch];
                ofdm_subcarrier_bpsk_t *sub = &ofdm_modem.rx_subcarriers[ch];

                FLOAT_t raw_i = ofdm_modem.fft_buffer[bin_idx * 2];
                FLOAT_t raw_q = ofdm_modem.fft_buffer[bin_idx * 2 + 1];

                FLOAT_t cos_p = COSF(sub->phase_nco);
                FLOAT_t sin_p = SINF(sub->phase_nco);

                FLOAT_t derot_i = raw_i * cos_p + raw_q * sin_p;
                FLOAT_t derot_q = raw_q * cos_p - raw_i * sin_p;

                FLOAT_t error_c = derot_i * derot_q;

                sub->costas_integrator += error_c * sub->costas_ki;
                sub->phase_step_nco = error_c * sub->costas_kp + sub->costas_integrator;

                FLOAT_t i2 = derot_i * derot_i;
                FLOAT_t q2 = derot_q * derot_q;
                FLOAT_t instant_metric = 0;

                if (i2 + q2 > 0) {
                    instant_metric = (i2 - q2) / (i2 + q2);
                }
                sub->phase_lock_metric += ofdm_modem.alpha_lock * (instant_metric - sub->phase_lock_metric);
                sub->is_phase_locked = (sub->phase_lock_metric > 0.55) ? 1 : 0;

                rx_bits[ch] = (derot_i >= 0) ? 1 : 0;

                sub->phase_nco += sub->phase_step_nco;
                if (sub->phase_nco >= 2 * M_PI) sub->phase_nco -= 2 * M_PI;
                if (sub->phase_nco < 0) sub->phase_nco += 2 * M_PI;
            }
            
            process_bits_cb(rx_bits);
        }
    }
}

void modem_test(void)
{
	TP();
}

#endif /* WITHINTEGRATEDDSP */
