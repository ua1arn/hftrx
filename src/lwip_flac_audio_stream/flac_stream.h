#ifndef FLAC_STREAM_H
#define FLAC_STREAM_H

#include <stdint.h>
#include <stddef.h>

/* 480 samples per channel = exact integer millisecond periods for common rates:
 * 48 kHz -> 10.0 ms, 24 kHz -> 20.0 ms, 16 kHz -> 30.0 ms, 12 kHz -> 40.0 ms, 8 kHz -> 60.0 ms */
#define FLAC_BLOCK_SIZE 480
#define FLAC_STREAM_HEADER_SIZE 42

typedef struct {
    uint32_t sample_rate_hz; /* Audio sample rate in Hz (e.g. 12000, 16000, 44100, 48000) */
    uint8_t channels;        /* Number of audio channels: 1 (mono) to 8 */
    uint8_t bits_per_sample; /* Bit depth per sample: 8, 16, or 24 */
} flac_audio_config_t;

/**
 * Generates the mandatory 42-byte 'fLaC' marker + STREAMINFO metadata block.
 * Total sample count is set to 0, which officially signals an infinite live stream.
 *
 * @param cfg      Pointer to audio stream configuration
 * @param out_buf  Destination buffer (must be at least FLAC_STREAM_HEADER_SIZE bytes)
 * @return         Number of bytes written (42)
 */
size_t flac_write_stream_header(const flac_audio_config_t *cfg, uint8_t *out_buf);

/**
 * Packs FLAC_BLOCK_SIZE (480) uncompressed PCM samples per channel into a valid FLAC frame
 * using SUBFRAME_VERBATIM (zero compression, minimal CPU overhead).
 *
 * Expected input layout in pcm_in (interleaved across channels):
 * - bits_per_sample == 8:  const int8_t*  [480 * channels]
 * - bits_per_sample == 16: const int16_t* [480 * channels]
 * - bits_per_sample == 24: const int32_t* [480 * channels] (lower 24 bits used)
 *
 * @param cfg        Pointer to audio stream configuration
 * @param pcm_in     Pointer to raw Little-Endian PCM samples
 * @param frame_num  Sequential frame index (0, 1, 2...)
 * @param out_buf    Destination buffer
 * @return           Encoded FLAC frame size in bytes, or 0 on invalid parameters
 */
size_t flac_encode_verbatim_frame(const flac_audio_config_t *cfg, const void *pcm_in, uint32_t frame_num, uint8_t *out_buf);

#endif /* FLAC_STREAM_H */
