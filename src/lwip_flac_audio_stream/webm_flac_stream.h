#ifndef WEBM_FLAC_STREAM_H
#define WEBM_FLAC_STREAM_H

#include <stdint.h>
#include <stddef.h>
#include "flac_stream.h"

/**
 * Writes the WebM EBML + Segment (Infinite Size) + Info + Tracks (A_FLAC + CodecPrivate) header.
 * Must be sent once right after the HTTP 200 OK header.
 *
 * @param cfg      Audio configuration
 * @param out_buf  Output buffer (at least 160 bytes)
 * @return         Total bytes written
 */
size_t webm_flac_write_stream_header(const flac_audio_config_t *cfg, uint8_t *out_buf);

/**
 * Encodes 480 PCM samples per channel into an uncompressed FLAC frame wrapped in a WebM Cluster.
 *
 * @param cfg          Audio configuration
 * @param pcm_in       Pointer to raw Little-Endian PCM samples (480 * channels)
 * @param frame_num    Sequential frame number (0, 1, 2...)
 * @param timestamp_ms Stream timestamp in milliseconds
 * @param out_buf      Output buffer (at least FLAC_BLOCK_SIZE * channels * (bits/8) + 64 bytes)
 * @return             Total bytes written
 */
size_t webm_flac_encode_cluster(const flac_audio_config_t *cfg, const void *pcm_in,
                                uint32_t frame_num, uint32_t timestamp_ms, uint8_t *out_buf);

#endif // WEBM_FLAC_STREAM_H
