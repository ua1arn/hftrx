#ifndef FLAC_STREAM_H
#define FLAC_STREAM_H

#include <stdint.h>
#include <stddef.h>

// 480 samples = exactly 10.0 ms at 48 kHz (or 30.0 ms at 16 kHz)
#define FLAC_BLOCK_SIZE 480
#define FLAC_STREAM_HEADER_SIZE 42

typedef struct {
    uint32_t sample_rate_hz; // e.g. 16000, 44100, 48000
    uint8_t channels;        // 1 to 8 (1 = mono, 2 = stereo interleaved)
    uint8_t bits_per_sample; // 8, 16, or 24
} flac_audio_config_t;

/**
 * Generates the 42-byte 'fLaC' + STREAMINFO metadata block for infinite streaming.
 */
size_t flac_write_stream_header(const flac_audio_config_t *cfg, uint8_t *out_buf);

/**
 * Encodes FLAC_BLOCK_SIZE (480) uncompressed PCM samples per channel into a valid FLAC frame.
 */
size_t flac_encode_verbatim_frame(const flac_audio_config_t *cfg, const void *pcm_in, uint32_t frame_num, uint8_t *out_buf);

#endif // FLAC_STREAM_H
