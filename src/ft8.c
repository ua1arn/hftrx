/*
 * FT8 implementation given from https://github.com/kgoba/ft8_lib,
 * adapted to Storch by RA4ASN
 */
#include "hardware.h"
#include "formats.h"

#if WITHFT8

#include "ft8.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#include "ft8/unpack.h"
#include "ft8/ldpc.h"
#include "ft8/decode.h"
#include "ft8/constants.h"
#include "ft8/encode.h"
#include "ft8/crc.h"

#include "speex/kiss_fftr.h"

void ft8_irqhandler_core1(void);

ft8_t ft8;
uint32_t bufind1 = 0, bufind2 = 0;
uint8_t fill_ft8_buf1 = 0, fill_ft8_buf2 = 0, ft8_enable = 0;
const uint32_t bufsize = ft8_sample_rate * ft8_length;
uint8_t ft8_tx = 0;
uint32_t bufind = 0;
uint8_t ft8_mox_request = 0;

const int kMin_score = 10; // Minimum sync score threshold for candidates
const int kMax_candidates = 120;
const int kLDPC_iterations = 20;

const int kMax_decoded_messages = 50;

const int kFreq_osr = 2;
const int kTime_osr = 2;

const float kFSK_dev = 6.25f; // tone deviation in Hz and symbol rate

float hann_i(int i, int N)
{
    float x = sinf((float)M_PI * i / N);
    return x * x;
}

float hamming_i(int i, int N)
{
    const float a0 = (float)25 / 46;
    const float a1 = 1 - a0;

    float x1 = cosf(2 * (float)M_PI * i / N);
    return a0 - a1 * x1;
}

float blackman_i(int i, int N)
{
    const float alpha = 0.16f; // or 2860/18608
    const float a0 = (1 - alpha) / 2;
    const float a1 = 1.0f / 2;
    const float a2 = alpha / 2;

    float x1 = cosf(2 * (float)M_PI * i / N);
    float x2 = 2 * x1 * x1 - 1; // Use double angle formula

    return a0 - a1 * x1 + a2 * x2;
}

static float max2(float a, float b)
{
    return (a >= b) ? a : b;
}

// Compute FFT magnitudes (log power) for each timeslot in the signal
void extract_power(const float signal[], waterfall_t *power, int block_size)
{
    const int subblock_size = block_size / power->time_osr;
    const int nfft = block_size * power->freq_osr;
    const float fft_norm = 2.0f / nfft;
    const int len_window = 1.8f * block_size; // hand-picked and optimized

    float window[nfft];
    for (int i = 0; i < nfft; ++i)
    {
        // window[i] = 1;
        // window[i] = hann_i(i, nfft);
        // window[i] = blackman_i(i, nfft);
        // window[i] = hamming_i(i, nfft);
        window[i] = (i < len_window) ? hann_i(i, len_window) : 0;
    }

    size_t fft_work_size;
    kiss_fftr_alloc(nfft, 0, 0, &fft_work_size);

//    PRINTF("Block size = %d\n", block_size);
//    PRINTF("Subblock size = %d\n", subblock_size);
//    PRINTF("N_FFT = %d\n", nfft);
//    PRINTF("FFT work area = %lu\n", fft_work_size);

    void *fft_work = malloc(fft_work_size);
    kiss_fftr_cfg fft_cfg = kiss_fftr_alloc(nfft, 0, fft_work, &fft_work_size);

    int offset = 0;
    float max_mag = -120.0f;
    for (int idx_block = 0; idx_block < power->num_blocks; ++idx_block)
    {
        // Loop over two possible time offsets (0 and block_size/2)
        for (int time_sub = 0; time_sub < power->time_osr; ++time_sub)
        {
            kiss_fft_scalar timedata[nfft];
            kiss_fft_cpx freqdata[nfft / 2 + 1];
            float mag_db[nfft / 2 + 1];

            // Extract windowed signal block
            for (int pos = 0; pos < nfft; ++pos)
            {
                timedata[pos] = window[pos] * signal[(idx_block * block_size) + (time_sub * subblock_size) + pos];
            }

            kiss_fftr(fft_cfg, timedata, freqdata);

            // Compute log magnitude in decibels
            for (int idx_bin = 0; idx_bin < nfft / 2 + 1; ++idx_bin)
            {
                float mag2 = (freqdata[idx_bin].i * freqdata[idx_bin].i) + (freqdata[idx_bin].r * freqdata[idx_bin].r);
                mag_db[idx_bin] = 10.0f * log10f(1E-12f + mag2 * fft_norm * fft_norm);
            }

            // Loop over two possible frequency bin offsets (for averaging)
            for (int freq_sub = 0; freq_sub < power->freq_osr; ++freq_sub)
            {
                for (int pos = 0; pos < power->num_bins; ++pos)
                {
                    float db = mag_db[pos * power->freq_osr + freq_sub];
                    // Scale decibels to unsigned 8-bit range and clamp the value
                    // Range 0-240 covers -120..0 dB in 0.5 dB steps
                    int scaled = (int)(2 * db + 240);

                    power->mag[offset] = (scaled < 0) ? 0 : ((scaled > 255) ? 255 : scaled);
                    ++offset;

                    if (db > max_mag)
                        max_mag = db;
                }
            }
        }
    }

    printf("Max magnitude: %.1f dB\n", max_mag);
    free(fft_work);
}

const int sample_rate = ft8_sample_rate;
const int num_samples = ft8_length * sample_rate;
const int num_bins = (int)(sample_rate / (2 * kFSK_dev)); // number bins of FSK tone width that the spectrum can be divided into
const int block_size = (int)(sample_rate / kFSK_dev);     // samples corresponding to one FSK symbol
const int subblock_size = block_size / kTime_osr;
const int nfft = block_size * kFreq_osr;
const int num_blocks = (num_samples - nfft + subblock_size) / block_size;

void ft8_decode(float * signal)
{
	float time_total = 0;
	memset(ft8.rx_text, '\0', ft8_text_records * ft8_text_length);

	//PRINTF("Sample rate %d Hz, %d blocks, %d bins\n", sample_rate, num_blocks, num_bins);

	// Compute FFT over the whole signal and store it
	uint8_t mag_power[num_blocks * kFreq_osr * kTime_osr * num_bins];
	waterfall_t power = {
		.num_blocks = num_blocks,
		.num_bins = num_bins,
		.time_osr = kTime_osr,
		.freq_osr = kFreq_osr,
		.mag = mag_power};
	extract_power(signal, &power, block_size);

	// Find top candidates by Costas sync score and localize them in time and frequency
	candidate_t candidate_list[kMax_candidates];
	int num_candidates = find_sync(&power, kMax_candidates, candidate_list, kMin_score);

	// Hash table for decoded messages (to check for duplicates)
	int num_decoded = 0;
	message_t decoded[kMax_decoded_messages];
	message_t *decoded_hashtable[kMax_decoded_messages];

	// Initialize hash table pointers
	for (int i = 0; i < kMax_decoded_messages; ++i)
	{
		decoded_hashtable[i] = NULL;
	}

	// Go over candidates and attempt to decode messages
	for (int idx = 0; idx < num_candidates; ++idx)
	{
		const candidate_t *cand = &candidate_list[idx];
		if (cand->score < kMin_score)
			continue;

		float freq_hz = (cand->freq_offset + (float)cand->freq_sub / kFreq_osr) * kFSK_dev;
		float time_sec = (cand->time_offset + (float)cand->time_sub / kTime_osr) / kFSK_dev;

		message_t message;
		decode_status_t status;
		if (!decode(&power, cand, &message, kLDPC_iterations, &status))
		{
			if (status.ldpc_errors > 0)
			{
				//PRINTF("LDPC decode: %d errors\n", status.ldpc_errors);
			}
			else if (status.crc_calculated != status.crc_extracted)
			{
				//PRINTF("CRC mismatch!\n");
			}
			else if (status.unpack_status != 0)
			{
				//PRINTF("Error while unpacking!\n");
			}
			continue;
		}

		//printf("Checking hash table for %4.1fs / %4.1fHz [%d]...\n", time_sec, freq_hz, cand->score);
		int idx_hash = message.hash % kMax_decoded_messages;
		bool found_empty_slot = false;
		bool found_duplicate = false;
		do
		{
			if (decoded_hashtable[idx_hash] == NULL)
			{
				//PRINTF("Found an empty slot\n");
				found_empty_slot = true;
			}
			else if ((decoded_hashtable[idx_hash]->hash == message.hash) && (0 == strcmp(decoded_hashtable[idx_hash]->text, message.text)))
			{
				//PRINTF("Found a duplicate [%s]\n", message.text);
				found_duplicate = true;
			}
			else
			{
				//PRINTF("Hash table clash!\n");
				// Move on to check the next entry in hash table
				idx_hash = (idx_hash + 1) % kMax_decoded_messages;
			}
		} while (!found_empty_slot && !found_duplicate);

		if (found_empty_slot)
		{
			// Fill the empty hashtable slot
			memcpy(&decoded[idx_hash], &message, sizeof(message));
			decoded_hashtable[idx_hash] = &decoded[idx_hash];

			// Fake WSJT-X-like output for now
			int snr = 0; // TODO: compute SNR
//			PRINTF("000000 %+4.2f %4.0f ~ %s\n", time_sec, freq_hz, message.text);

			local_snprintf_P(ft8.rx_text [num_decoded], ft8_text_length, "%02d%02d%02d %4.0f %02d ~ %s",
								0, 0, 0, freq_hz, snr, message.text);

			num_decoded ++;
			time_total += time_sec;
		}
	}
	ft8.decoded_messages = num_decoded;
	PRINTF("%u ft8: decoded %d messages\n", (unsigned) (__get_MPIDR() & 0x03), num_decoded);
//	xcz_ipi_sendmsg_c0(FT8_MSG_DECODE_DONE);
}

// ********************************************************************************

void ft8_irqhandler_core0(void)
{
	uint8_t msg = ft8.int_core0;

	if (msg == FT8_MSG_START_FILL_1)
	{
		ft8_start_fill();
	}
	else if (msg == FT8_MSG_DECODE_DONE)
	{
		hamradio_gui_parse_ft8buf();
	}
	else if (msg == FT8_MSG_ENCODE_DONE)
	{
		PRINTF("transmit message...\n\r");
		ft8_stop_fill();
		ft8_tx_enable();
	}
}

void ft8_irqhandler_core1(void)
{
	uint8_t msg = ft8.int_core1;
	PRINTF("ft8_irqhandler_core1 CPU%u\n", (unsigned) (__get_MPIDR() & 0x03));

	if (msg == FT8_MSG_DECODE_1) // start decode
	{
		//PRINTF("core 1: start decoding 1\r\n");
		ft8_decode(ft8.rx_buf1);
	}
	else if (msg == FT8_MSG_DECODE_2) // start decode
	{
		//PRINTF("core 1: start decoding 2\r\n");
		ft8_decode(ft8.rx_buf1);
	}
	else if (msg == FT8_MSG_ENCODE)  // transmit message
	{
//		ft8_encode(ft8.tx_buf, ft8.tx_text, ft8.tx_freq);
//    	ft8_decode(share_mem.ft8txbuf, & st1);
//		xcz_ipi_sendmsg_c0(FT8_MSG_ENCODE_DONE);
	}
}

void ft8_tx_enable(void)
{
	ft8_tx = 1;
	ft8_mox_request = 1;
}

void ft8_txfill(float * sample)
{
	const uint32_t bufsize = ft8_sample_rate * ft8_length;

	if (ft8_tx && hamradio_get_tx())
	{
		* sample = ft8.tx_buf [bufind];
		bufind ++;
		if (bufind >= bufsize)
		{
			ft8_tx = 0;
			bufind = 0;
			ft8_mox_request = 1;
		}
	}
}

void ft8_fill(float sample)
{
	system_disableIRQ();
	if (fill_ft8_buf1 == 1)
	{
		ASSERT(bufind1 < bufsize);
		ft8.rx_buf1 [bufind1] = sample;
		bufind1 ++;
		if (bufind1 >= bufsize)
		{
			fill_ft8_buf1 = 0;
			bufind1 = 0;
			xcz_ipi_sendmsg_c1(FT8_MSG_DECODE_1); // decode 1
		}
	}
	else if (fill_ft8_buf2 == 1)
	{
		ASSERT(bufind2 < bufsize);
		ft8.rx_buf2 [bufind2] = sample;
		bufind2 ++;
		if (bufind2 >= bufsize)
		{
			fill_ft8_buf2 = 0;
			bufind2 = 0;
			xcz_ipi_sendmsg_c1(FT8_MSG_DECODE_2); // decode 2
		}
	}
	system_enableIRQ();
}

void ft8_start_fill(void)
{
	if (fill_ft8_buf1)
	{
		system_disableIRQ();
		fill_ft8_buf2 = 1;
		system_enableIRQ();
		PRINTF("core 0: start fill 2\n");
	}
	else
	{
		system_disableIRQ();
		fill_ft8_buf1 = 1;
		system_enableIRQ();
		PRINTF("core 0: start fill 1\n");
	}
}

void ft8_stop_fill(void)
{
	fill_ft8_buf1 = 0;
	fill_ft8_buf1 = 0;
	bufind1 = 0;
	bufind2 = 0;
}

void ft8_set_state(uint8_t v)
{
	ft8_enable = v != 0;
	xcz_ipi_sendmsg_c1(FT8_MSG_DISABLE + ft8_enable);
}

uint8_t get_ft8_state(void)
{
	return ft8_enable;
}

void ft8_initialize(void)
{
	arm_hardware_set_handler_system(ft8_interrupt_core0, ft8_irqhandler_core0);
	arm_hardware_set_handler_realtime(ft8_interrupt_core1, ft8_irqhandler_core1);
}

#endif /* WITHFT8 */
