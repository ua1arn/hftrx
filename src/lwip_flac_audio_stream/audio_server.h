#ifndef AUDIO_SERVER_H
#define AUDIO_SERVER_H

#include <stdint.h>

/**
 * Initializes the lwIP HTTP WebM-FLAC audio streaming server on port 80.
 *
 * @param sample_rate_hz  Audio sample rate in Hz (e.g. 16000, 44100, 48000)
 * @param channels        Number of channels: 1 (mono) to 8 (stereo interleaved)
 * @param bits_per_sample Bit depth: 8, 16, or 24
 */
void audio_server_init(uint32_t sample_rate_hz, uint8_t channels, uint8_t bits_per_sample);

/**
 * Encodes and transmits 480 PCM samples per channel to the connected HTTP client.
 * Call every 10.0 ms for 48 kHz (or every 30.0 ms for 16 kHz).
 *
 * NOTE: If using FreeRTOS, invoke this from the tcpip_thread context via tcpip_callback().
 */
void audio_server_push_pcm(const void *pcm_480_samples_per_ch);

#endif // AUDIO_SERVER_H
