#include "ogg_flac_stream.h"
#include <string.h>

#define OGG_STREAM_SERIAL 0x12345678

/* Ogg CRC-32 calculation (polynomial 0x04C11DB7, direct, non-reflected) */
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

/* Build a single Ogg Page containing one packet (up to 65025 bytes) */
static size_t build_ogg_page(uint8_t header_type, uint64_t granule_pos, uint32_t page_seq,
                             const uint8_t *payload, size_t payload_len, uint8_t *out_buf) {
    uint8_t num_segments = (uint8_t)((payload_len / 255) + 1);
    size_t header_len = 27 + num_segments;

    /* 1. OggS capture pattern & version */
    out_buf[0] = 'O'; out_buf[1] = 'g'; out_buf[2] = 'g'; out_buf[3] = 'S';
    out_buf[4] = 0x00;        /* Stream structure version */
    out_buf[5] = header_type; /* 0x02 = BOS, 0x00 = normal */

    /* 2. Granule position (64-bit Little-Endian) */
    for (int i = 0; i < 8; i++) {
        out_buf[6 + i] = (uint8_t)(granule_pos >> (i * 8));
    }

    /* 3. Bitstream serial number (32-bit Little-Endian) */
    out_buf[14] = (uint8_t)(OGG_STREAM_SERIAL);
    out_buf[15] = (uint8_t)(OGG_STREAM_SERIAL >> 8);
    out_buf[16] = (uint8_t)(OGG_STREAM_SERIAL >> 16);
    out_buf[17] = (uint8_t)(OGG_STREAM_SERIAL >> 24);

    /* 4. Page sequence number (32-bit Little-Endian) */
    out_buf[18] = (uint8_t)(page_seq);
    out_buf[19] = (uint8_t)(page_seq >> 8);
    out_buf[20] = (uint8_t)(page_seq >> 16);
    out_buf[21] = (uint8_t)(page_seq >> 24);

    /* 5. CRC checksum placeholder (must be 0 during calculation) */
    out_buf[22] = 0; out_buf[23] = 0; out_buf[24] = 0; out_buf[25] = 0;

    /* 6. Segment table */
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

    /* 7. Copy payload */
    memcpy(&out_buf[header_len], payload, payload_len);
    size_t total_len = header_len + payload_len;

    /* 8. Calculate and write Ogg CRC-32 */
    uint32_t crc = ogg_crc32(out_buf, total_len);
    out_buf[22] = (uint8_t)(crc);
    out_buf[23] = (uint8_t)(crc >> 8);
    out_buf[24] = (uint8_t)(crc >> 16);
    out_buf[25] = (uint8_t)(crc >> 24);

    return total_len;
}

size_t ogg_flac_write_headers(const flac_audio_config_t *cfg, uint8_t *out_buf) {
    if (cfg == NULL || out_buf == NULL) {
        return 0;
    }

    uint8_t flac_hdr[FLAC_STREAM_HEADER_SIZE];
    if (flac_write_stream_header(cfg, flac_hdr) == 0) {
        return 0;
    }

    size_t offset = 0;

    /* --- Page 0: OggFLAC Identification Packet (51 bytes) --- */
    uint8_t id_packet[51];
    id_packet[0] = 0x7F;
    id_packet[1] = 'F'; id_packet[2] = 'L'; id_packet[3] = 'A'; id_packet[4] = 'C';
    id_packet[5] = 0x01; /* Major version 1 */
    id_packet[6] = 0x00; /* Minor version 0 */
    id_packet[7] = 0x00; /* Number of header packets (high byte) */
    id_packet[8] = 0x01; /* Number of header packets (low byte: 1 Vorbis comment packet follows) */
    memcpy(&id_packet[9], flac_hdr, FLAC_STREAM_HEADER_SIZE); /* 'fLaC' + STREAMINFO */

    offset += build_ogg_page(0x02, 0, 0, id_packet, sizeof(id_packet), &out_buf[offset]);

    /* --- Page 1: Mandatory Empty Vorbis Comment Metadata Block (16 bytes) --- */
    const uint8_t vorbis_comment[16] = {
        0x84,             /* Last-metadata-block flag (1) + VORBIS_COMMENT type (4) */
        0x00, 0x00, 0x0C, /* Length = 12 bytes */
        0x04, 0x00, 0x00, 0x00, 'l', 'w', 'I', 'P', /* Vendor string ("lwIP") */
        0x00, 0x00, 0x00, 0x00  /* User comment list length = 0 */
    };

    offset += build_ogg_page(0x00, 0, 1, vorbis_comment, sizeof(vorbis_comment), &out_buf[offset]);

    return offset;
}

size_t ogg_flac_encode_page(const flac_audio_config_t *cfg, const void *pcm_in,
                            uint32_t frame_num, uint8_t *out_buf) {
    static uint8_t raw_flac[FLAC_MAX_FRAME_SIZE];

    size_t flac_len = flac_encode_verbatim_frame(cfg, pcm_in, frame_num, raw_flac);
    if (flac_len == 0) {
        return 0;
    }

    /* Granule position = total samples per channel encoded up to the end of this page */
    uint64_t granule_pos = (uint64_t)(frame_num + 1) * FLAC_BLOCK_SIZE;

    /* Audio pages start at page_seq = 2 (Pages 0 and 1 were headers) */
    return build_ogg_page(0x00, granule_pos, frame_num + 2, raw_flac, flac_len, out_buf);
}
