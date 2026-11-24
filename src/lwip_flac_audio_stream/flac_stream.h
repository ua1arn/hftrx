#ifndef FLAC_STREAM_H
#define FLAC_STREAM_H

#include <stdint.h>
#include <stddef.h>

/* Block size in samples (512 samples * 2 bytes + header fits within standard 1460-byte TCP MSS) */
#define FLAC_BLOCK_SIZE 512

/* Global 42-byte FLAC stream header: 16 kHz, 16-bit, Mono, infinite stream length (0 samples) */
extern const uint8_t FLAC_STREAM_HEADER[42];

/**
 * Encodes 512 raw 16-bit signed PCM samples (Little-Endian) into an uncompressed
 * FLAC frame (SUBFRAME_VERBATIM) with valid CRC-8 and CRC-16 checksums.
 *
 * @param pcm_in      Pointer to an array of FLAC_BLOCK_SIZE (512) 16-bit samples
 * @param frame_num   Sequential frame counter (0, 1, 2, ...)
 * @param out_buf     Output buffer (must be at least FLAC_BLOCK_SIZE * 2 + 20 bytes)
 * @return            Total size of the encoded FLAC frame in bytes
 */
size_t flac_encode_verbatim_frame(const int16_t *pcm_in, uint32_t frame_num, uint8_t *out_buf);

#endif /* FLAC_STREAM_H */
