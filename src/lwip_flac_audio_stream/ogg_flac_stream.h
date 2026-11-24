#ifndef OGG_FLAC_STREAM_H
#define OGG_FLAC_STREAM_H

#include <stdint.h>
#include <stddef.h>
#include "flac_stream.h"

/* Maximum size of initial OggFLAC headers (Page 0 + Page 1) */
#define OGG_FLAC_HEADERS_MAX_SIZE 160

/* Maximum size of one Ogg Page carrying an uncompressed FLAC frame */
#define OGG_FLAC_MAX_PAGE_SIZE (FLAC_MAX_FRAME_SIZE + 128)

/**
 * Generates the initial OggFLAC stream headers (Page 0: BOS + FLAC STREAMINFO, Page 1: Vorbis Comment)
 * for the specified audio format configuration.
 * Must be sent once right after the HTTP 200 OK header.
 *
 * @param cfg      Pointer to audio stream configuration
 * @param out_buf  Output buffer (at least OGG_FLAC_HEADERS_MAX_SIZE bytes)
 * @return         Total bytes written to out_buf, or 0 on invalid configuration
 */
size_t ogg_flac_write_headers(const flac_audio_config_t *cfg, uint8_t *out_buf);

/**
 * Encodes 512 interleaved PCM samples into an uncompressed FLAC frame wrapped inside a valid Ogg Page.
 *
 * @param cfg        Pointer to audio stream configuration
 * @param pcm_in     Pointer to interleaved PCM samples (FLAC_BLOCK_SIZE * channels samples)
 * @param frame_num  Sequential frame counter (0, 1, 2...)
 * @param out_buf    Output buffer (at least OGG_FLAC_MAX_PAGE_SIZE bytes)
 * @return           Total size of the Ogg Page in bytes, or 0 on error
 */
size_t ogg_flac_encode_page(const flac_audio_config_t *cfg, const void *pcm_in,
                            uint32_t frame_num, uint8_t *out_buf);

#endif /* OGG_FLAC_STREAM_H */
