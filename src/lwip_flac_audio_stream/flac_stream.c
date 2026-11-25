#include "flac_stream.h"
#include <string.h>

/* Calculates CRC-8 (polynomial 0x07) over the FLAC frame header */
static uint8_t calc_crc8(const uint8_t *data, size_t len) {
    uint8_t crc = 0;
    while (len--) {
        crc ^= *data++;
        for (int i = 0; i < 8; i++) {
            crc = (crc & 0x80) ? ((crc << 1) ^ 0x07) : (crc << 1);
        }
    }
    return crc;
}

/* Calculates CRC-16 (polynomial 0x8005) over the entire FLAC frame */
static uint16_t calc_crc16(const uint8_t *data, size_t len) {
    uint16_t crc = 0;
    while (len--) {
        crc ^= (uint16_t)(*data++) << 8;
        for (int i = 0; i < 8; i++) {
            crc = (crc & 0x8000) ? ((crc << 1) ^ 0x8005) : (crc << 1);
        }
    }
    return crc;
}

/* Encodes 32-bit frame number using FLAC's extended UTF-8 variable-length coding */
static size_t encode_utf8_uint32(uint32_t val, uint8_t *buf) {
    if (val < 0x80) {
        buf[0] = (uint8_t)val;
        return 1;
    } else if (val < 0x800) {
        buf[0] = 0xC0 | (val >> 6);
        buf[1] = 0x80 | (val & 0x3F);
        return 2;
    } else if (val < 0x10000) {
        buf[0] = 0xE0 | (val >> 12);
        buf[1] = 0x80 | ((val >> 6) & 0x3F);
        buf[2] = 0x80 | (val & 0x3F);
        return 3;
    } else if (val < 0x200000) {
        buf[0] = 0xF0 | (val >> 18);
        buf[1] = 0x80 | ((val >> 12) & 0x3F);
        buf[2] = 0x80 | ((val >> 6) & 0x3F);
        buf[3] = 0x80 | (val & 0x3F);
        return 4;
    } else {
        buf[0] = 0xF8 | (val >> 24);
        buf[1] = 0x80 | ((val >> 18) & 0x3F);
        buf[2] = 0x80 | ((val >> 12) & 0x3F);
        buf[3] = 0x80 | ((val >> 6) & 0x3F);
        buf[4] = 0x80 | (val & 0x3F);
        return 5;
    }
}

/* Maps sample rate in Hz to the 4-bit FLAC frame header code */
static uint8_t get_sample_rate_code(uint32_t sr) {
    switch (sr) {
        case 88200:  return 0x01;
        case 176400: return 0x02;
        case 192000: return 0x03;
        case 8000:   return 0x04;
        case 16000:  return 0x05;
        case 22050:  return 0x06;
        case 24000:  return 0x07;
        case 32000:  return 0x08;
        case 44100:  return 0x09;
        case 48000:  return 0x0A;
        case 96000:  return 0x0B;
        default:
            if (sr <= 255000 && (sr % 1000 == 0)) return 0x0C; /* 8-bit kHz at header end */
            if (sr <= 65535) return 0x0D;                      /* 16-bit Hz at header end */
            if (sr <= 655350 && (sr % 10 == 0)) return 0x0E;   /* 16-bit tens of Hz at header end */
            return 0x00;
    }
}

/* Maps bit depth to the 3-bit FLAC frame header code */
static uint8_t get_sample_size_code(uint8_t bps) {
    switch (bps) {
        case 8:  return 0x01;
        case 12: return 0x02;
        case 16: return 0x04;
        case 20: return 0x05;
        case 24: return 0x06;
        case 32: return 0x07;
        default: return 0x00;
    }
}

size_t flac_write_stream_header(const flac_audio_config_t *cfg, uint8_t *out_buf) {
    if (!cfg || !out_buf) return 0;

    memset(out_buf, 0, FLAC_STREAM_HEADER_SIZE);

    /* 4-byte 'fLaC' stream marker */
    out_buf[0] = 'f'; out_buf[1] = 'L'; out_buf[2] = 'a'; out_buf[3] = 'C';

    /* Metadata block header: Last-metadata-block flag (0x80) | STREAMINFO type (0x00), length = 34 (0x000022) */
    out_buf[4] = 0x80;
    out_buf[5] = 0x00; out_buf[6] = 0x00; out_buf[7] = 0x22;

    /* Minimum and maximum block size in samples (480 = 0x01E0, Big-Endian) */
    out_buf[8]  = (uint8_t)(FLAC_BLOCK_SIZE >> 8);
    out_buf[9]  = (uint8_t)(FLAC_BLOCK_SIZE & 0xFF);
    out_buf[10] = (uint8_t)(FLAC_BLOCK_SIZE >> 8);
    out_buf[11] = (uint8_t)(FLAC_BLOCK_SIZE & 0xFF);

    /* Bytes 12..17: Min/Max frame size = 0 (unknown for live streams) */

    /* Bytes 18..21: SampleRate (20 bits) | Channels-1 (3 bits) | BitsPerSample-1 (5 bits) | TotalSamples[35:32] (0) */
    uint32_t sr = cfg->sample_rate_hz;
    uint8_t ch_minus_1 = (cfg->channels - 1) & 0x07;
    uint8_t bps_minus_1 = (cfg->bits_per_sample - 1) & 0x1F;

    out_buf[18] = (uint8_t)(sr >> 12);
    out_buf[19] = (uint8_t)(sr >> 4);
    out_buf[20] = (uint8_t)(((sr & 0x0F) << 4) | (ch_minus_1 << 1) | (bps_minus_1 >> 4));
    out_buf[21] = (uint8_t)((bps_minus_1 & 0x0F) << 4);

    /* Bytes 22..25: Total samples lower 32 bits = 0 (signals infinite stream) */
    /* Bytes 26..41: Uncompressed audio MD5 = 0 (allowed for live streaming) */
    return FLAC_STREAM_HEADER_SIZE;
}

size_t flac_encode_verbatim_frame(const flac_audio_config_t *cfg, const void *pcm_in, uint32_t frame_num, uint8_t *out_buf) {
    if (!cfg || !pcm_in || !out_buf) return 0;
    if (cfg->channels < 1 || cfg->channels > 8) return 0;
    if (cfg->bits_per_sample != 8 && cfg->bits_per_sample != 16 && cfg->bits_per_sample != 24) return 0;

    uint8_t sr_code = get_sample_rate_code(cfg->sample_rate_hz);
    uint8_t ss_code = get_sample_size_code(cfg->bits_per_sample);
    if (sr_code == 0 || ss_code == 0) return 0;

    size_t idx = 0;

    /* 1. Frame Sync Code (14 bits: 0x3FFE) + Reserved (0) + Fixed-blocksize strategy (0) */
    out_buf[idx++] = 0xFF;
    out_buf[idx++] = 0xF8;

    /* Block size code 0x07 (16-bit block_size-1 follows header) | 4-bit sample rate code */
    out_buf[idx++] = (uint8_t)(0x70 | (sr_code & 0x0F));

    /* Channel assignment (0..7 for 1..8 independent channels) | 3-bit sample size code | Reserved (0) */
    uint8_t ch_assign = (cfg->channels - 1) & 0x0F;
    out_buf[idx++] = (uint8_t)((ch_assign << 4) | ((ss_code & 0x07) << 1));

    /* UTF-8 encoded sequential frame number */
    idx += encode_utf8_uint32(frame_num, &out_buf[idx]);

    /* 16-bit (block_size - 1) in Big-Endian (480 - 1 = 479 = 0x01DF) */
    out_buf[idx++] = (uint8_t)((FLAC_BLOCK_SIZE - 1) >> 8);
    out_buf[idx++] = (uint8_t)((FLAC_BLOCK_SIZE - 1) & 0xFF);

    /* Non-standard sample rate value if sr_code is 0x0C, 0x0D, or 0x0E */
    if (sr_code == 0x0C) {
        out_buf[idx++] = (uint8_t)(cfg->sample_rate_hz / 1000);
    } else if (sr_code == 0x0D) {
        out_buf[idx++] = (uint8_t)(cfg->sample_rate_hz >> 8);
        out_buf[idx++] = (uint8_t)(cfg->sample_rate_hz & 0xFF);
    } else if (sr_code == 0x0E) {
        uint16_t sr_tens = (uint16_t)(cfg->sample_rate_hz / 10);
        out_buf[idx++] = (uint8_t)(sr_tens >> 8);
        out_buf[idx++] = (uint8_t)(sr_tens & 0xFF);
    }

    /* Frame header CRC-8 checksum */
    out_buf[idx] = calc_crc8(out_buf, idx);
    idx++;

    /* 2. Subframes: FLAC stores channels sequentially (non-interleaved) inside each frame */
    uint8_t ch_count = cfg->channels;
    for (uint8_t ch = 0; ch < ch_count; ch++) {
        /* Subframe header: 0 (pad) | 000001 (SUBFRAME_VERBATIM) | 0 (no wasted bits) = 0x02 */
        out_buf[idx++] = 0x02;

        /* Convert Little-Endian MCU samples to Big-Endian FLAC PCM samples */
        if (cfg->bits_per_sample == 8) {
            const int8_t *samples = (const int8_t *)pcm_in;
            for (size_t i = 0; i < FLAC_BLOCK_SIZE; i++) {
                out_buf[idx++] = (uint8_t)samples[i * ch_count + ch];
            }
        } else if (cfg->bits_per_sample == 16) {
            const int16_t *samples = (const int16_t *)pcm_in;
            for (size_t i = 0; i < FLAC_BLOCK_SIZE; i++) {
                uint16_t s = (uint16_t)samples[i * ch_count + ch];
                out_buf[idx++] = (uint8_t)(s >> 8);
                out_buf[idx++] = (uint8_t)(s & 0xFF);
            }
        } else if (cfg->bits_per_sample == 24) {
            const int32_t *samples = (const int32_t *)pcm_in;
            for (size_t i = 0; i < FLAC_BLOCK_SIZE; i++) {
                uint32_t s = (uint32_t)samples[i * ch_count + ch];
                out_buf[idx++] = (uint8_t)((s >> 16) & 0xFF);
                out_buf[idx++] = (uint8_t)((s >> 8) & 0xFF);
                out_buf[idx++] = (uint8_t)(s & 0xFF);
            }
        }
    }

    /* 3. Frame Footer: 16-bit Big-Endian CRC-16 over the entire frame */
    uint16_t crc16 = calc_crc16(out_buf, idx);
    out_buf[idx++] = (uint8_t)(crc16 >> 8);
    out_buf[idx++] = (uint8_t)(crc16 & 0xFF);

    return idx;
}
