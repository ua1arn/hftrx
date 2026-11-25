#ifndef AUDIO_SERVER_H
#define AUDIO_SERVER_H

#include <stdint.h>

/**
 * Initializes the lwIP HTTP WebM-FLAC audio streaming server on port 80.
 *
 * @param sample_rate_hz  Sample rate in Hz (e.g. 12000, 16000, 44100, 48000)
 * @param channels        Number of channels: 1 (mono) to 8
 * @param bits_per_sample Bit depth: 8, 16, or 24
 */
void audio_server_init(uint32_t sample_rate_hz, uint8_t channels, uint8_t bits_per_sample);

/**
 * Encodes 480 PCM samples per channel into a WebM-FLAC Cluster and sends it to the client.
 * Call periodically when 480 samples per channel are ready:
 * - 48000 Hz -> every 10.0 ms (100 calls/sec)
 * - 24000 Hz -> every 20.0 ms (50 calls/sec)
 * - 16000 Hz -> every 30.0 ms (33.33 calls/sec)
 * - 12000 Hz -> every 40.0 ms (25 calls/sec)
 * -  8000 Hz -> every 60.0 ms (16.67 calls/sec)
 *
 * IMPORTANT: If using FreeRTOS, execute this inside tcpip_thread via tcpip_callback()
 * because lwIP Raw API functions are not thread-safe.
 */
void audio_server_push_pcm(const void *pcm_480_samples_per_ch);

#endif /* AUDIO_SERVER_H */
