#ifndef FLAC_STREAM_H
#define FLAC_STREAM_H

#include <stdint.h>
#include <stddef.h>

/* 480 samples per channel = exact 10.0 ms at 48 kHz, 20.0 ms at 24 kHz, 30.0 ms at 16 kHz */
#define FLAC_BLOCK_SIZE 480
#define FLAC_STREAM_HEADER_SIZE 42

typedef struct {
    uint32_t sample_rate_hz; /* e.g., 12000, 16000, 44100, 48000 */
    uint8_t channels;        /* 1 to 8 (1 = mono, 2 = stereo interleaved) */
    uint8_t bits_per_sample; /* 8, 16, or 24 */
} flac_audio_config_t;

/**
 * Generates the mandatory 42-byte FLAC stream header ('fLaC' marker + STREAMINFO metadata block)
 * configured for infinite live streaming (total samples = 0).
 *
 * @param cfg      Audio stream parameters
 * @param out_buf  Output buffer (must be at least FLAC_STREAM_HEADER_SIZE bytes)
 * @return         Number of bytes written (42), or 0 on invalid parameters
 */
size_t flac_write_stream_header(const flac_audio_config_t *cfg, uint8_t *out_buf);

/**
 * Encodes FLAC_BLOCK_SIZE (480) uncompressed PCM samples per channel into a valid FLAC frame
 * using SUBFRAME_VERBATIM (zero mathematical compression, bit-exact lossless).
 *
 * Input buffer layout (interleaved across channels):
 * - bits_per_sample == 8:  const int8_t*  [480 * channels]
 * - bits_per_sample == 16: const int16_t* [480 * channels]
 * - bits_per_sample == 24: const int32_t* [480 * channels] (lower 24 bits used)
 *
 * @param cfg        Audio stream parameters
 * @param pcm_in     Pointer to interleaved Little-Endian PCM samples
 * @param frame_num  Sequential frame index (0, 1, 2...)
 * @param out_buf    Output buffer (at least 480 * channels * (bits_per_sample/8) + 32 bytes)
 * @return           Encoded FLAC frame length in bytes, or 0 on error
 */
size_t flac_encode_verbatim_frame(const flac_audio_config_t *cfg, const void *pcm_in, uint32_t frame_num, uint8_t *out_buf);

#endif /* FLAC_STREAM_H */
