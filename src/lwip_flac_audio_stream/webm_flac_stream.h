#ifndef WEBM_FLAC_STREAM_H
#define WEBM_FLAC_STREAM_H

#include <stdint.h>
#include <stddef.h>
#include "flac_stream.h"

/**
 * Builds the initial WebM / Matroska stream header:
 * EBML Header + Segment (Unknown/Infinite Size) + Info + Tracks (CodecID = "A_FLAC", CodecPrivate = 42B FLAC header).
 * Must be sent once right after the HTTP 200 OK header.
 *
 * @param cfg      Pointer to audio stream configuration
 * @param out_buf  Destination buffer (at least 160 bytes)
 * @return         Total header length in bytes
 */
size_t webm_flac_write_stream_header(const flac_audio_config_t *cfg, uint8_t *out_buf);

/**
 * Encodes 480 PCM samples per channel into an uncompressed FLAC frame and wraps it
 * inside a WebM Cluster + SimpleBlock packet for live streaming.
 *
 * @param cfg           Pointer to audio stream configuration
 * @param pcm_in        Pointer to raw Little-Endian PCM samples
 * @param frame_num     Sequential frame index (0, 1, 2...)
 * @param timestamp_ms  Cluster timestamp in milliseconds from stream start
 * @param out_buf       Destination buffer
 * @return              Total WebM Cluster packet size in bytes, or 0 on error
 */
size_t webm_flac_encode_cluster(const flac_audio_config_t *cfg, const void *pcm_in,
                                uint32_t frame_num, uint32_t timestamp_ms, uint8_t *out_buf);

#endif /* WEBM_FLAC_STREAM_H */
