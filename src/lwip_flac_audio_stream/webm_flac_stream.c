#include "webm_flac_stream.h"
#include <string.h>

/* Serializes a 64-bit IEEE 754 floating-point number in Big-Endian (Matroska float format) */
static void write_be_f64(uint8_t *dst, double val) {
    uint64_t bits;
    memcpy(&bits, &val, sizeof bits);
    for (int i = 0; i < 8; i++) {
        dst[i] = (uint8_t)(bits >> ((7 - i) * 8));
    }
}

size_t webm_flac_write_stream_header(const flac_audio_config_t *cfg, uint8_t *out_buf) {
    /* 1. EBML Header (36 bytes total): identifies stream as WebM v2 */
    static const uint8_t ebml_hdr[] = {
        0x1A, 0x45, 0xDF, 0xA3, 0x9F,             /* EBML ID (0x1A45DFA3) + VINT size (31 = 0x80 | 31) */
        0x42, 0x86, 0x81, 0x01,                   /* EBMLVersion = 1 */
        0x42, 0xF7, 0x81, 0x01,                   /* EBMLReadVersion = 1 */
        0x42, 0xF2, 0x81, 0x04,                   /* EBMLMaxIDLength = 4 */
        0x42, 0xF3, 0x81, 0x08,                   /* EBMLMaxSizeLength = 8 */
        0x42, 0x82, 0x84, 'w', 'e', 'b', 'm',     /* DocType = "webm" (4 bytes) */
        0x42, 0x87, 0x81, 0x02,                   /* DocTypeVersion = 2 */
        0x42, 0x85, 0x81, 0x02                    /* DocTypeReadVersion = 2 */
    };

    /* 2. Segment Header (12 bytes):
     *    0x01FFFFFFFFFFFFFF is the official Matroska/WebM "Unknown/Infinite Size" marker for live streams */
    static const uint8_t seg_hdr[] = {
        0x18, 0x53, 0x80, 0x67,
        0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };

    /* 3. Segment Info (12 bytes): sets TimecodeScale = 1,000,000 ns (1 tick = 1 ms) */
    static const uint8_t info_hdr[] = {
        0x15, 0x49, 0xA9, 0x66, 0x87,             /* Info ID (0x1549A966) + VINT size (7) */
        0x2A, 0xD7, 0xB1, 0x83, 0x0F, 0x42, 0x40  /* TimecodeScale = 1000000 (0x0F4240) */
    };

    /* 4. Tracks Element Prefix (27 bytes):
     *    Tracks payload = 84 bytes (2B TrackEntry header + 82B TrackEntry payload)
     *    TrackEntry payload = 82 bytes:
     *      3B TrackNumber + 4B TrackUID + 3B TrackType + 8B CodecID + 19B Audio + 45B CodecPrivate */
    static const uint8_t tracks_prefix[] = {
        0x16, 0x54, 0xAE, 0x6B, (uint8_t)(0x80 | 84), /* Tracks ID + VINT size (84) */
        0xAE, (uint8_t)(0x80 | 82),                   /* TrackEntry ID + VINT size (82) */
        0xD7, 0x81, 0x01,                             /* TrackNumber = 1 */
        0x73, 0xC5, 0x81, 0x01,                       /* TrackUID = 1 */
        0x83, 0x81, 0x02,                             /* TrackType = 2 (Audio) */
        0x86, 0x86, 'A', '_', 'F', 'L', 'A', 'C',     /* CodecID = "A_FLAC" (6 bytes) */
        0xE1, (uint8_t)(0x80 | 17)                    /* Audio sub-element ID + VINT size (17) */
    };

    size_t idx = 0;

    memcpy(&out_buf[idx], ebml_hdr, sizeof ebml_hdr);
    idx += sizeof ebml_hdr;

    memcpy(&out_buf[idx], seg_hdr, sizeof seg_hdr);
    idx += sizeof seg_hdr;

    memcpy(&out_buf[idx], info_hdr, sizeof info_hdr);
    idx += sizeof info_hdr;

    memcpy(&out_buf[idx], tracks_prefix, sizeof tracks_prefix);
    idx += sizeof tracks_prefix;

    /* Audio sub-elements (17 bytes total):
     * - SamplingFrequency (ID 0xB5, size 8 bytes float64 -> 10 bytes) */
    out_buf[idx++] = 0xB5;
    out_buf[idx++] = 0x88;
    write_be_f64(&out_buf[idx], (double)cfg->sample_rate_hz);
    idx += 8;

    /* - Channels (ID 0x9F, size 1 byte -> 3 bytes) */
    out_buf[idx++] = 0x9F;
    out_buf[idx++] = 0x81;
    out_buf[idx++] = cfg->channels;

    /* - BitDepth (ID 0x6264, size 1 byte -> 4 bytes) */
    out_buf[idx++] = 0x62;
    out_buf[idx++] = 0x64;
    out_buf[idx++] = 0x81;
    out_buf[idx++] = cfg->bits_per_sample;

    /* CodecPrivate (ID 0x63A2, size 42 bytes -> 45 bytes total):
     * Contains the mandatory 42-byte 'fLaC' + STREAMINFO metadata block */
    out_buf[idx++] = 0x63;
    out_buf[idx++] = 0xA2;
    out_buf[idx++] = (uint8_t)(0x80 | FLAC_STREAM_HEADER_SIZE);
    flac_write_stream_header(cfg, &out_buf[idx]);
    idx += FLAC_STREAM_HEADER_SIZE;

    return idx;
}

size_t webm_flac_encode_cluster(const flac_audio_config_t *cfg, const void *pcm_in,
                                uint32_t frame_num, uint32_t timestamp_ms, uint8_t *out_buf) {
    /* Encode FLAC frame directly at offset 23 (leaving bytes 0..22 for Cluster + SimpleBlock headers) */
    size_t flac_len = flac_encode_verbatim_frame(cfg, pcm_in, frame_num, &out_buf[23]);
    if (flac_len == 0) return 0;

    /* SimpleBlock payload = 4-byte block header + encoded FLAC frame */
    uint32_t sb_payload_len = 4 + (uint32_t)flac_len;

    /* Cluster payload = 6-byte Timecode element + 5-byte SimpleBlock ID/size + sb_payload_len */
    uint32_t cluster_payload_len = 6 + 5 + sb_payload_len;

    /* 1. Cluster Element ID (0x1F43B675) + 4-byte VINT size (0x10000000 | len) */
    out_buf[0] = 0x1F; out_buf[1] = 0x43; out_buf[2] = 0xB6; out_buf[3] = 0x75;
    out_buf[4] = (uint8_t)(0x10 | ((cluster_payload_len >> 24) & 0x0F));
    out_buf[5] = (uint8_t)((cluster_payload_len >> 16) & 0xFF);
    out_buf[6] = (uint8_t)((cluster_payload_len >> 8) & 0xFF);
    out_buf[7] = (uint8_t)(cluster_payload_len & 0xFF);

    /* 2. Cluster Timecode Element (ID 0xE7, size 4 bytes Big-Endian milliseconds) */
    out_buf[8]  = 0xE7;
    out_buf[9]  = 0x84;
    out_buf[10] = (uint8_t)(timestamp_ms >> 24);
    out_buf[11] = (uint8_t)(timestamp_ms >> 16);
    out_buf[12] = (uint8_t)(timestamp_ms >> 8);
    out_buf[13] = (uint8_t)(timestamp_ms & 0xFF);

    /* 3. SimpleBlock Element ID (0xA3) + 4-byte VINT size */
    out_buf[14] = 0xA3;
    out_buf[15] = (uint8_t)(0x10 | ((sb_payload_len >> 24) & 0x0F));
    out_buf[16] = (uint8_t)((sb_payload_len >> 16) & 0xFF);
    out_buf[17] = (uint8_t)((sb_payload_len >> 8) & 0xFF);
    out_buf[18] = (uint8_t)(sb_payload_len & 0xFF);

    /* 4. SimpleBlock Header (4 bytes):
     *    TrackNumber = 1 (VINT 0x81), Relative Timecode = 0 (int16 BE), Flags = 0x80 (Keyframe) */
    out_buf[19] = 0x81;
    out_buf[20] = 0x00;
    out_buf[21] = 0x00;
    out_buf[22] = 0x80;

    return 23 + flac_len;
}
