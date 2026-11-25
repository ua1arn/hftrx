#ifndef WS_AUDIO_SERVER_H
#define WS_AUDIO_SERVER_H

#include <stdint.h>

/* 480 samples per block = 60.0 ms at 8 kHz, 30.0 ms at 16 kHz, 10.0 ms at 48 kHz */
#define WS_AUDIO_BLOCK_SAMPLES 480

/**
 * Initializes the lwIP HTTP + WebSocket (RFC 6455) PCM audio server on port 80.
 *
 * @param sample_rate_hz  Sample rate in Hz (e.g., 8000, 12000, 16000, 24000, 48000)
 * @param channels        Number of channels: 1 (mono) or 2 (stereo interleaved)
 */
void ws_audio_server_init(uint32_t sample_rate_hz, uint8_t channels);

/**
 * Wraps and broadcasts 480 raw 16-bit Little-Endian PCM samples per channel
 * to all connected WebSocket clients as a binary frame (opcode 0x82).
 */
void ws_audio_server_push_pcm(const int16_t *pcm_480_samples_per_ch);

#endif /* WS_AUDIO_SERVER_H */
