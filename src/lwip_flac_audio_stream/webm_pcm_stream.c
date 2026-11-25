#include "webm_pcm_stream.h"
#include <string.h>

// Helper to write IEEE 754 64-bit float in Big-Endian (for WebM SamplingFrequency)
static void write_be_f64(uint8_t *dst, double val) {
    uint64_t bits;
    memcpy(&bits, &val, sizeof(bits));
    for (int i = 0; i < 8; i++) {
        dst[i] = (uint8_t)(bits >> ((7 - i) * 8));
    }
}

size_t webm_write_stream_header(const webm_audio_config_t *cfg, uint8_t *out_buf) {
    size_t idx = 0;

    // 1. EBML Header (DocType = "webm")
    const uint8_t ebml_hdr[] = {
        0x1A, 0x45, 0xDF, 0xA3, 0x9F,             // EBML ID + size (31)
        0x42, 0x86, 0x81, 0x01,                   // EBMLVersion = 1
        0x42, 0xF7, 0x81, 0x01,                   // EBMLReadVersion = 1
        0x42, 0xF2, 0x81, 0x04,                   // EBMLMaxIDLength = 4
        0x42, 0xF3, 0x81, 0x08,                   // EBMLMaxSizeLength = 8
        0x42, 0x82, 0x84, 'w', 'e', 'b', 'm',     // DocType = "webm"
        0x42, 0x87, 0x81, 0x02,                   // DocTypeVersion = 2
        0x42, 0x85, 0x81, 0x02                    // DocTypeReadVersion = 2
    };
    memcpy(&out_buf[idx], ebml_hdr, sizeof(ebml_hdr));
    idx += sizeof(ebml_hdr);

    // 2. Segment Header with Unknown/Infinite Size (0x01FFFFFFFFFFFFFF)
    const uint8_t seg_hdr[] = {
        0x18, 0x53, 0x80, 0x67,
        0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
    };
    memcpy(&out_buf[idx], seg_hdr, sizeof(seg_hdr));
    idx += sizeof(seg_hdr);

    // 3. Segment Info (TimecodeScale = 1,000,000 ns = 1 ms)
    const uint8_t info_hdr[] = {
        0x15, 0x49, 0xA9, 0x66, 0x87,             // Info ID + size (7)
        0x2A, 0xD7, 0xB1, 0x83, 0x0F, 0x42, 0x40  // TimecodeScale = 1000000 (1 ms)
    };
    memcpy(&out_buf[idx], info_hdr, sizeof(info_hdr));
    idx += sizeof(info_hdr);

    // 4. Tracks Element (Single Audio Track: CodecID = "A_PCM/INT/LIT")
    // TrackEntry payload = 37 bytes -> Tracks payload = 39 bytes
    const uint8_t tracks_prefix[] = {
        0x16, 0x54, 0xAE, 0x6B, 0xA7,             // Tracks ID + size (39)
        0xAE, 0xA5,                               // TrackEntry ID + size (37)
        0xD7, 0x81, 0x01,                         // TrackNumber = 1
        0x73, 0xC5, 0x81, 0x01,                   // TrackUID = 1
        0x83, 0x81, 0x02,                         // TrackType = 2 (Audio)
        0x86, 0x8D, 'A', '_', 'P', 'C', 'M', '/', 'I', 'N', 'T', '/', 'L', 'I', 'T', // CodecID
        0xE1, 0x90                                // Audio settings ID + size (16)
    };
    memcpy(&out_buf[idx], tracks_prefix, sizeof(tracks_prefix));
    idx += sizeof(tracks_prefix);

    // Audio sub-elements (16 bytes total):
    // SamplingFrequency (ID 0xB5, size 8 bytes float64 -> 10 bytes)
    out_buf[idx++] = 0xB5;
    out_buf[idx++] = 0x88;
    write_be_f64(&out_buf[idx], (double)cfg->sample_rate_hz);
    idx += 8;

    // Channels (ID 0x9F, size 1 -> 3 bytes)
    out_buf[idx++] = 0x9F;
    out_buf[idx++] = 0x81;
    out_buf[idx++] = cfg->channels;

    // BitDepth (ID 0x62 0x64, size 1 -> 4 bytes -> wait: 10+3+4 = 17 bytes! Adjust sizes below)
    out_buf[idx++] = 0x62;
    out_buf[idx++] = 0x64;
    out_buf[idx++] = 0x81;
    out_buf[idx++] = cfg->bits_per_sample;

    // Fix exact sizes for Audio (17), TrackEntry (38), Tracks (40)
    // Offsets from tracks_prefix start:
    size_t tp_start = idx - 17 - sizeof(tracks_prefix);
    out_buf[tp_start + 4]  = 0x80 | 40; // Tracks size = 40
    out_buf[tp_start + 6]  = 0x80 | 38; // TrackEntry size = 38
    out_buf[tp_start + 31] = 0x80 | 17; // Audio size = 17

    return idx;
}

size_t webm_encode_pcm_cluster(const webm_audio_config_t *cfg, const void *pcm_in, uint32_t timestamp_ms, uint8_t *out_buf) {
    if (!cfg || !pcm_in || !out_buf) return 0;

    size_t bytes_per_sample = cfg->bits_per_sample / 8;
    size_t pcm_bytes = WEBM_BLOCK_SAMPLES * cfg->channels * bytes_per_sample;

    // SimpleBlock payload = 4 bytes header (TrackNum=1, Timecode=0, Flags=0x80) + pcm_bytes
    uint32_t sb_payload_len = 4 + (uint32_t)pcm_bytes;

    // Cluster payload = 6 bytes Timecode element (0xE7 0x84 + 4-byte BE ms) + 4 bytes SimpleBlock ID + 4 bytes VINT size + sb_payload_len
    uint32_t cluster_payload_len = 6 + 4 + 4 + sb_payload_len;

    size_t idx = 0;

    // 1. Cluster Element ID (0x1F43B675) + 4-byte VINT size (0x10000000 | len)
    out_buf[idx++] = 0x1F; out_buf[idx++] = 0x43; out_buf[idx++] = 0xB6; out_buf[idx++] = 0x75;
    out_buf[idx++] = (uint8_t)(0x10 | ((cluster_payload_len >> 24) & 0x0F));
    out_buf[idx++] = (uint8_t)((cluster_payload_len >> 16) & 0xFF);
    out_buf[idx++] = (uint8_t)((cluster_payload_len >> 8) & 0xFF);
    out_buf[idx++] = (uint8_t)(cluster_payload_len & 0xFF);

    // 2. Cluster Timecode (ID 0xE7, size 4 bytes Big-Endian ms)
    out_buf[idx++] = 0xE7;
    out_buf[idx++] = 0x84;
    out_buf[idx++] = (uint8_t)(timestamp_ms >> 24);
    out_buf[idx++] = (uint8_t)(timestamp_ms >> 16);
    out_buf[idx++] = (uint8_t)(timestamp_ms >> 8);
    out_buf[idx++] = (uint8_t)(timestamp_ms & 0xFF);

    // 3. SimpleBlock ID (0xA3) + 4-byte VINT size
    out_buf[idx++] = 0xA3;
    out_buf[idx++] = (uint8_t)(0x10 | ((sb_payload_len >> 24) & 0x0F));
    out_buf[idx++] = (uint8_t)((sb_payload_len >> 16) & 0xFF);
    out_buf[idx++] = (uint8_t)((sb_payload_len >> 8) & 0xFF);
    out_buf[idx++] = (uint8_t)(sb_payload_len & 0xFF);

    // 4. SimpleBlock Header: TrackNum (0x81 = Track 1), Relative Timecode (0x0000), Flags (0x80 = Keyframe)
    out_buf[idx++] = 0x81;
    out_buf[idx++] = 0x00;
    out_buf[idx++] = 0x00;
    out_buf[idx++] = 0x80;

    // 5. Raw PCM data directly (Little-Endian, zero conversion!)
    memcpy(&out_buf[idx], pcm_in, pcm_bytes);
    idx += pcm_bytes;

    return idx;
}
