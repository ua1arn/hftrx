#ifndef WEBM_PCM_STREAM_H
#define WEBM_PCM_STREAM_H

#include <stdint.h>
#include <stddef.h>

#define WEBM_BLOCK_SAMPLES 480 // 10.0 ms at 48 kHz

typedef struct {
    uint32_t sample_rate_hz; // e.g. 16000, 44100, 48000
    uint8_t channels;        // 1 (mono) or 2 (stereo)
    uint8_t bits_per_sample; // 8, 16, or 24
} webm_audio_config_t;

/**
 * Writes the WebM EBML + Segment (Unknown Size) + Info + Tracks header (~98 bytes).
 */
size_t webm_write_stream_header(const webm_audio_config_t *cfg, uint8_t *out_buf);

/**
 * Wraps raw Little-Endian PCM samples into a WebM Cluster + SimpleBlock.
 * No byte-swapping and no CRC calculation required!
 *
 * @param cfg          Audio configuration
 * @param pcm_in       Pointer to raw PCM samples (480 * channels)
 * @param timestamp_ms Stream timestamp in milliseconds (increments by 10 ms per block at 48 kHz)
 * @param out_buf      Output buffer (at least 480 * channels * (bits/8) + 24 bytes)
 * @return             Total bytes written to out_buf
 */
size_t webm_encode_pcm_cluster(const webm_audio_config_t *cfg, const void *pcm_in, uint32_t timestamp_ms, uint8_t *out_buf);

#endif // WEBM_PCM_STREAM_H
