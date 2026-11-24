#ifndef OGG_FLAC_STREAM_H
#define OGG_FLAC_STREAM_H

#include <stdint.h>
#include <stddef.h>
#include "flac_stream.h"

/**
 * Generates the initial OggFLAC stream headers (Page 0: BOS + FLAC STREAMINFO, Page 1: Vorbis Comment).
 */
size_t ogg_flac_write_headers(const flac_audio_config_t *cfg, uint8_t *out_buf);

/**
 * Encodes FLAC_BLOCK_SIZE (480) PCM samples per channel into an uncompressed FLAC frame
 * wrapped inside a valid Ogg Page.
 */
size_t ogg_flac_encode_page(const flac_audio_config_t *cfg, const void *pcm_in, uint32_t frame_num, uint8_t *out_buf);

#endif // OGG_FLAC_STREAM_H
