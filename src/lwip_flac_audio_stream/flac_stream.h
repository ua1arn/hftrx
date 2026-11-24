#ifndef FLAC_STREAM_H
#define FLAC_STREAM_H

#include <stdint.h>
#include <stddef.h>

/* Number of audio frames (samples per channel) in one FLAC block */
#define FLAC_BLOCK_SIZE 512

/* Maximum supported channels and bytes per sample for static buffer sizing */
#define FLAC_MAX_CHANNELS 8
#define FLAC_MAX_BYTES_PER_SAMPLE 3

/* Global 42-byte FLAC stream header ('fLaC' + STREAMINFO block) */
#define FLAC_STREAM_HEADER_SIZE 42

/* Maximum size of a single encoded uncompressed FLAC frame */
#define FLAC_MAX_FRAME_SIZE (32 + (FLAC_BLOCK_SIZE * FLAC_MAX_CHANNELS * FLAC_MAX_BYTES_PER_SAMPLE))

/* Audio stream format configuration */
typedef struct {
    uint32_t sample_rate_hz;  /* Sample rate in Hz (e.g., 8000, 16000, 44100, 48000, 96000) */
    uint8_t  channels;        /* Number of channels: 1 (mono) to 8 */
    uint8_t  bits_per_sample; /* Bit depth: 8, 16, or 24 bits */
} flac_audio_config_t;

/**
 * Generates the 42-byte global FLAC stream header ('fLaC' + STREAMINFO metadata block)
 * configured with the specified audio parameters and infinite stream length (0 samples).
 *
 * @param cfg      Pointer to audio stream configuration
 * @param out_buf  Output buffer (must be at least FLAC_STREAM_HEADER_SIZE bytes)
 * @return         Number of bytes written (42), or 0 if configuration is invalid
 */
size_t flac_write_stream_header(const flac_audio_config_t *cfg, uint8_t *out_buf);

/**
 * Encodes one block of FLAC_BLOCK_SIZE (512) raw interleaved PCM samples into an
 * uncompressed FLAC frame (SUBFRAME_VERBATIM).
 *
 * Input buffer layout (interleaved across channels):
 * - 8-bit:  pointer to int8_t  array [FLAC_BLOCK_SIZE * channels] (signed 8-bit PCM)
 * - 16-bit: pointer to int16_t array [FLAC_BLOCK_SIZE * channels] (signed 16-bit LE PCM)
 * - 24-bit: pointer to int32_t array [FLAC_BLOCK_SIZE * channels] (signed 24-bit in 32-bit LE)
 *
 * @param cfg        Pointer to audio stream configuration
 * @param pcm_in     Pointer to interleaved PCM samples
 * @param frame_num  Sequential frame index (0, 1, 2...)
 * @param out_buf    Output buffer (must be at least FLAC_MAX_FRAME_SIZE bytes)
 * @return           Total encoded FLAC frame size in bytes, or 0 on error
 */
size_t flac_encode_verbatim_frame(const flac_audio_config_t *cfg, const void *pcm_in,
                                  uint32_t frame_num, uint8_t *out_buf);

#endif /* FLAC_STREAM_H */
