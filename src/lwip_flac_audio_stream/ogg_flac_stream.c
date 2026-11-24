#include "ogg_flac_stream.h"
#include <string.h>

#define OGG_STREAM_SERIAL 0x12345678

// Temporary buffer large enough for max config (480 samples * 8 channels * 3 bytes + headers = 11550 bytes)
// If you only use up to 2 channels / 16-bit, you can reduce this to 2048 to save RAM.
static uint8_t raw_flac_tmp[FLAC_BLOCK_SIZE * 8 * 3 + 32];

static uint32_t ogg_crc32(const uint8_t *data, size_t len) {
    uint32_t crc = 0;
    while (len--) {
        crc ^= (uint32_t)(*data++) << 24;
        for (int i = 0; i < 8; i++) {
            crc = (crc & 0x80000000) ? ((crc << 1) ^ 0x04C11DB7) : (crc << 1);
        }
    }
    return crc;
}

static size_t build_ogg_page(uint8_t header_type, uint64_t granule_pos, uint32_t page_seq,
                             const uint8_t *payload, size_t payload_len, uint8_t *out_buf) {
    uint8_t num_segments = (uint8_t)((payload_len / 255) + 1);
    size_t header_len = 27 + num_segments;

    out_buf[0] = 'O'; out_buf[1] = 'g'; out_buf[2] = 'g'; out_buf[3] = 'S';
    out_buf[4] = 0x00;
    out_buf[5] = header_type;

    for (int i = 0; i < 8; i++) {
        out_buf[6 + i] = (uint8_t)(granule_pos >> (i * 8));
    }

    out_buf[14] = (uint8_t)(OGG_STREAM_SERIAL);
    out_buf[15] = (uint8_t)(OGG_STREAM_SERIAL >> 8);
    out_buf[16] = (uint8_t)(OGG_STREAM_SERIAL >> 16);
    out_buf[17] = (uint8_t)(OGG_STREAM_SERIAL >> 24);

    out_buf[18] = (uint8_t)(page_seq);
    out_buf[19] = (uint8_t)(page_seq >> 8);
    out_buf[20] = (uint8_t)(page_seq >> 16);
    out_buf[21] = (uint8_t)(page_seq >> 24);

    out_buf[22] = 0; out_buf[23] = 0; out_buf[24] = 0; out_buf[25] = 0;
    out_buf[26] = num_segments;

    size_t rem = payload_len;
    for (uint8_t i = 0; i < num_segments; i++) {
        if (rem >= 255) {
            out_buf[27 + i] = 255;
            rem -= 255;
        } else {
            out_buf[27 + i] = (uint8_t)rem;
            rem = 0;
        }
    }

    memcpy(&out_buf[header_len], payload, payload_len);
    size_t total_len = header_len + payload_len;

    uint32_t crc = ogg_crc32(out_buf, total_len);
    out_buf[22] = (uint8_t)(crc);
    out_buf[23] = (uint8_t)(crc >> 8);
    out_buf[24] = (uint8_t)(crc >> 16);
    out_buf[25] = (uint8_t)(crc >> 24);

    return total_len;
}

size_t ogg_flac_write_headers(const flac_audio_config_t *cfg, uint8_t *out_buf) {
    size_t offset = 0;

    // Page 0: OggFLAC Identification Packet (51 bytes)
    uint8_t id_packet[51];
    id_packet[0] = 0x7F;
    id_packet[1] = 'F'; id_packet[2] = 'L'; id_packet[3] = 'A'; id_packet[4] = 'C';
    id_packet[5] = 0x01; // Major version
    id_packet[6] = 0x00; // Minor version
    id_packet[7] = 0x00; // Number of header packets (high)
    id_packet[8] = 0x01; // Number of header packets (low: 1 Vorbis comment follows)
    flac_write_stream_header(cfg, &id_packet[9]);

    offset += build_ogg_page(0x02, 0, 0, id_packet, sizeof(id_packet), &out_buf[offset]);

    // Page 1: Mandatory Empty Vorbis Comment Metadata Block (16 bytes)
    const uint8_t vorbis_comment[16] = {
        0x84,             // Last-metadata-block flag (1) + VORBIS_COMMENT (4)
        0x00, 0x00, 0x0C, // Length = 12 bytes
        0x04, 0x00, 0x00, 0x00, 'l', 'w', 'I', 'P',
        0x00, 0x00, 0x00, 0x00
    };

    offset += build_ogg_page(0x00, 0, 1, vorbis_comment, sizeof(vorbis_comment), &out_buf[offset]);

    return offset;
}

size_t ogg_flac_encode_page(const flac_audio_config_t *cfg, const void *pcm_in, uint32_t frame_num, uint8_t *out_buf) {
    size_t flac_len = flac_encode_verbatim_frame(cfg, pcm_in, frame_num, raw_flac_tmp);
    if (flac_len == 0) return 0;

    uint64_t granule_pos = (uint64_t)(frame_num + 1) * FLAC_BLOCK_SIZE;
    return build_ogg_page(0x00, granule_pos, frame_num + 2, raw_flac_tmp, flac_len, out_buf);
}
