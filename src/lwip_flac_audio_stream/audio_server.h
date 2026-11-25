#ifndef AUDIO_SERVER_H
#define AUDIO_SERVER_H

#include <stdint.h>

/**
 * Thread-safe initialization of the lwIP HTTP WebM-FLAC audio streaming server on port 80.
 * Can be called either before starting the RTOS scheduler or from any RTOS task
 * (internally delegates socket setup to lwIP tcpip_thread via tcpip_callback).
 *
 * @param sample_rate_hz  Audio sample rate in Hz (e.g., 12000, 16000, 44100, 48000)
 * @param channels        Number of channels: 1 (mono) to 8
 * @param bits_per_sample Bit depth: 8, 16, or 24
 */
void audio_server_init(uint32_t sample_rate_hz, uint8_t channels, uint8_t bits_per_sample);

/**
 * Thread-safe function to push 480 interleaved Little-Endian PCM samples per channel.
 * Safe to call from ANY FreeRTOS task or DMA interrupt service routine (ISR).
 * Uses an internal Ping-Pong buffer and non-blocking tcpip_try_callback() so that
 * all lwIP Raw API calls (tcp_write, tcp_output) execute strictly inside tcpip_thread.
 *
 * Call period = 480 / sample_rate_hz:
 * - 48000 Hz -> every 10.0 ms
 * - 24000 Hz -> every 20.0 ms
 * - 16000 Hz -> every 30.0 ms
 * - 12000 Hz -> every 40.0 ms
 *
 * @param pcm_480_samples_per_ch Pointer to 480 * channels PCM samples
 */
void audio_server_push_pcm(const void *pcm_480_samples_per_ch);

#endif /* AUDIO_SERVER_H */
