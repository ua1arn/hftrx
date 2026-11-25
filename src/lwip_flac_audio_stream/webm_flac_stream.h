#ifndef WEBM_FLAC_STREAM_H
#define WEBM_FLAC_STREAM_H

#include <stdint.h>
#include <stddef.h>
#include "flac_stream.h"

#define WEBM_STREAM_HEADER_MAX_SIZE 160
#define WEBM_CLUSTER_OVERHEAD 23

/**
 * Generates the WebM/Matroska initialization segment containing:
 * - EBML Header (DocType = "webm")
 * - Segment Header with Unknown/Infinite size (0x01FFFFFFFFFFFFFF)
 * - Segment Info (TimecodeScale = 1,000,000 ns = 1 ms)
 * - Tracks Element (TrackType = Audio, CodecID = "A_FLAC", Audio settings, CodecPrivate = 42B FLAC header)
 *
 * @param cfg      Audio stream configuration
 * @param out_buf  Output buffer (at least WEBM_STREAM_HEADER_MAX_SIZE bytes)
 * @return         Number of bytes written (154 bytes)
 */
size_t webm_flac_write_stream_header(const flac_audio_config_t *cfg, uint8_t *out_buf);

/**
 * Encodes 480 PCM samples per channel into an uncompressed FLAC frame and wraps it
 * inside a WebM Cluster + SimpleBlock element with the specified timestamp.
 *
 * @param cfg           Audio stream configuration
 * @param pcm_in        Pointer to interleaved Little-Endian PCM samples
 * @param frame_num     Sequential frame index (0, 1, 2...)
 * @param timestamp_ms  Cluster timestamp in milliseconds
 * @param out_buf       Output buffer (at least 480 * channels * (bits_per_sample/8) + 64 bytes)
 * @return              Total WebM Cluster size in bytes, or 0 on error
 */
size_t webm_flac_encode_cluster(const flac_audio_config_t *cfg, const void *pcm_in,
                                uint32_t frame_num, uint32_t timestamp_ms, uint8_t *out_buf);

#endif /* WEBM_FLAC_STREAM_H */
