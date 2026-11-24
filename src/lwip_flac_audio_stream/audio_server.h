#ifndef AUDIO_SERVER_H
#define AUDIO_SERVER_H

#include <stdint.h>
#include "flac_stream.h"

/**
 * Initializes the lwIP HTTP audio streaming server on port 80 with configurable
 * audio stream parameters. Call once after lwIP stack initialization.
 *
 * @param sample_rate_hz   Sample rate in Hz (e.g., 16000, 44100, 48000)
 * @param channels         Number of audio channels: 1 (mono) to 8
 * @param bits_per_sample  Sample bit depth: 8, 16, or 24 bits
 * @return                 0 on success, -1 if parameters are invalid or bind failed
 */
int audio_server_init(uint32_t sample_rate_hz, uint8_t channels, uint8_t bits_per_sample);

/**
 * Pushes one block of FLAC_BLOCK_SIZE (512) interleaved PCM samples to the connected client.
 *
 * Expected buffer type & size (FLAC_BLOCK_SIZE * channels samples):
 * - 8-bit:  const int8_t*  (512 * channels bytes)
 * - 16-bit: const int16_t* (512 * channels * 2 bytes)
 * - 24-bit: const int32_t* (512 * channels * 4 bytes, 24-bit signed values in 32-bit words)
 *
 * IMPORTANT: If using FreeRTOS, do NOT call directly from an ISR or external task.
 * Schedule execution inside the tcpip_thread context via tcpip_callback().
 */
void audio_server_push_pcm(const void *pcm_interleaved_samples);

#endif /* AUDIO_SERVER_H */
