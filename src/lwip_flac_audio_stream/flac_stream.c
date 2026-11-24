#include "flac_stream.h"
#include <string.h>

/* CRC-8 calculation (polynomial 0x07) for FLAC frame header */
static uint8_t calc_crc8(const uint8_t *data, size_t len) {
    uint8_t crc = 0;
    while (len--) {
        crc ^= *data++;
        for (int i = 0; i < 8; i++) {
            crc = (crc & 0x80) ? (uint8_t)((crc << 1) ^ 0x07) : (uint8_t)(crc << 1);
        }
    }
    return crc;
}

/* CRC-16 calculation (polynomial 0x8005) for entire FLAC frame */
static uint16_t calc_crc16(const uint8_t *data, size_t len) {
    uint16_t crc = 0;
    while (len--) {
        crc ^= (uint16_t)(*data++) << 8;
        for (int i = 0; i < 8; i++) {
            crc = (crc & 0x8000) ? (uint16_t)((crc << 1) ^ 0x8005) : (uint16_t)(crc << 1);
        }
    }
    return crc;
}

/* Encode frame number in UTF-8 format per FLAC specification */
static size_t encode_utf8_uint32(uint32_t val, uint8_t *buf) {
    if (val < 0x80) {
        buf[0] = (uint8_t)val;
        return 1;
    } else if (val < 0x800) {
        buf[0] = (uint8_t)(0xC0 | (val >> 6));
        buf[1] = (uint8_t)(0x80 | (val & 0x3F));
        return 2;
    } else if (val < 0x10000) {
        buf[0] = (uint8_t)(0xE0 | (val >> 12));
        buf[1] = (uint8_t)(0x80 | ((val >> 6) & 0x3F));
        buf[2] = (uint8_t)(0x80 | (val & 0x3F));
        return 3;
    } else if (val < 0x200000) {
        buf[0] = (uint8_t)(0xF0 | (val >> 18));
        buf[1] = (uint8_t)(0x80 | ((val >> 12) & 0x3F));
        buf[2] = (uint8_t)(0x80 | ((val >> 6) & 0x3F));
        buf[3] = (uint8_t)(0x80 | (val & 0x3F));
        return 4;
    } else {
        buf[0] = (uint8_t)(0xF8 | (val >> 24));
        buf[1] = (uint8_t)(0x80 | ((val >> 18) & 0x3F));
        buf[2] = (uint8_t)(0x80 | ((val >> 12) & 0x3F));
        buf[3] = (uint8_t)(0x80 | ((val >> 6) & 0x3F));
        buf[4] = (uint8_t)(0x80 | (val & 0x3F));
        return 5;
    }
}

/* Validate audio configuration */
static int is_valid_config(const flac_audio_config_t *cfg) {
    if (cfg == NULL) return 0;
    if (cfg->sample_rate_hz == 0 || cfg->sample_rate_hz > 655350) return 0;
    if (cfg->channels < 1 || cfg->channels > FLAC_MAX_CHANNELS) return 0;
    if (cfg->bits_per_sample != 8 && cfg->bits_per_sample != 16 && cfg->bits_per_sample != 24) return 0;
    return 1;
}

/* Map standard sample rates to 4-bit FLAC frame header codes */
static uint8_t get_sample_rate_code(uint32_t sample_rate_hz) {
    switch (sample_rate_hz) {
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
            /* Non-standard sample rate: read from end of frame header */
            if (sample_rate_hz <= 255000 && (sample_rate_hz % 1000) == 0) {
                return 0x0C; /* 8-bit sample rate in kHz */
            } else if (sample_rate_hz <= 65535) {
                return 0x0D; /* 16-bit sample rate in Hz */
            } else if (sample_rate_hz <= 655350 && (sample_rate_hz % 10) == 0) {
                return 0x0E; /* 16-bit sample rate in tens of Hz */
            }
            return 0x00; /* Get from STREAMINFO */
    }
}

/* Map bit depth to 3-bit FLAC frame header code */
static uint8_t get_sample_size_code(uint8_t bits_per_sample) {
    switch (bits_per_sample) {
        case 8:  return 0x01; /* 8 bits per sample */
        case 16: return 0x04; /* 16 bits per sample */
        case 24: return 0x06; /* 24 bits per sample */
        default: return 0x00; /* Get from STREAMINFO */
    }
}

size_t flac_write_stream_header(const flac_audio_config_t *cfg, uint8_t *out_buf) {
    if (!is_valid_config(cfg) || out_buf == NULL) {
        return 0;
    }

    memset(out_buf, 0, FLAC_STREAM_HEADER_SIZE);

    /* 1. Stream marker 'fLaC' */
    out_buf[0] = 'f'; out_buf[1] = 'L'; out_buf[2] = 'a'; out_buf[3] = 'C';

    /* 2. Metadata block header: Last-metadata-block (1) + STREAMINFO (0), length = 34 (0x22) */
    out_buf[4] = 0x80;
    out_buf[5] = 0x00;
    out_buf[6] = 0x00;
    out_buf[7] = 0x22;

    /* 3. Minimum and maximum block size (512 samples) */
    out_buf[8]  = (uint8_t)(FLAC_BLOCK_SIZE >> 8);
    out_buf[9]  = (uint8_t)(FLAC_BLOCK_SIZE & 0xFF);
    out_buf[10] = (uint8_t)(FLAC_BLOCK_SIZE >> 8);
    out_buf[11] = (uint8_t)(FLAC_BLOCK_SIZE & 0xFF);

    /* Bytes 12..17: Min/Max frame size = 0 (unknown) */

    /* 4. Pack Sample Rate (20 bits), Channels-1 (3 bits), BitsPerSample-1 (5 bits), TotalSamples (36 bits = 0) */
    uint32_t sr  = cfg->sample_rate_hz & 0xFFFFF;
    uint8_t  ch  = (uint8_t)((cfg->channels - 1) & 0x07);
    uint8_t  bps = (uint8_t)((cfg->bits_per_sample - 1) & 0x1F);

    out_buf[18] = (uint8_t)(sr >> 12);
    out_buf[19] = (uint8_t)(sr >> 4);
    out_buf[20] = (uint8_t)(((sr & 0x0F) << 4) | (ch << 1) | (bps >> 4));
    out_buf[21] = (uint8_t)((bps & 0x0F) << 4); /* Lower 4 bits are upper bits of 36-bit total samples (0) */

    /* Bytes 22..25: Lower 32 bits of total samples = 0 (infinite stream) */
    /* Bytes 26..41: Unset MD5 signature (all zeros) */

    return FLAC_STREAM_HEADER_SIZE;
}

size_t flac_encode_verbatim_frame(const flac_audio_config_t *cfg, const void *pcm_in,
                                  uint32_t frame_num, uint8_t *out_buf) {
    if (!is_valid_config(cfg) || pcm_in == NULL || out_buf == NULL) {
        return 0;
    }

    size_t idx = 0;
    uint8_t sr_code   = get_sample_rate_code(cfg->sample_rate_hz);
    uint8_t ch_code   = (uint8_t)(cfg->channels - 1); /* 0000 = 1 ch, 0001 = 2 ch (independent), etc. */
    uint8_t size_code = get_sample_size_code(cfg->bits_per_sample);

    /* 1. FLAC Frame Header */
    out_buf[idx++] = 0xFF; /* Sync code (14 bits: 11111111 111110) + reserved (0) + fixed block size (0) */
    out_buf[idx++] = 0xF8;
    out_buf[idx++] = (uint8_t)(0x70 | (sr_code & 0x0F)); /* 0111 = 16-bit block size at end + 4-bit sample rate */
    out_buf[idx++] = (uint8_t)((ch_code << 4) | (size_code << 1)); /* 4-bit channel + 3-bit sample size + reserved (0) */

    /* Frame number in UTF-8 */
    idx += encode_utf8_uint32(frame_num, &out_buf[idx]);

    /* 16-bit (block size - 1) in Big-Endian (512 - 1 = 511 = 0x01FF) */
    out_buf[idx++] = (uint8_t)((FLAC_BLOCK_SIZE - 1) >> 8);
    out_buf[idx++] = (uint8_t)((FLAC_BLOCK_SIZE - 1) & 0xFF);

    /* Optional sample rate bytes if non-standard sample rate code (0x0C, 0x0D, 0x0E) was selected */
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

    /* Frame header CRC-8 */
    out_buf[idx] = calc_crc8(out_buf, idx);
    idx++;

    /* 2. Subframes (one VERBATIM subframe per channel, non-interleaved inside FLAC frame) */
    uint8_t num_channels = cfg->channels;

    if (cfg->bits_per_sample == 8) {
        const int8_t *samples = (const int8_t *)pcm_in;
        for (uint8_t ch = 0; ch < num_channels; ch++) {
            out_buf[idx++] = 0x02; /* Subframe header: SUBFRAME_VERBATIM (000001 << 1), no wasted bits */
            for (size_t i = 0; i < FLAC_BLOCK_SIZE; i++) {
                out_buf[idx++] = (uint8_t)samples[i * num_channels + ch];
            }
        }
    } else if (cfg->bits_per_sample == 16) {
        const int16_t *samples = (const int16_t *)pcm_in;
        for (uint8_t ch = 0; ch < num_channels; ch++) {
            out_buf[idx++] = 0x02; /* Subframe header: SUBFRAME_VERBATIM */
            for (size_t i = 0; i < FLAC_BLOCK_SIZE; i++) {
                uint16_t s = (uint16_t)samples[i * num_channels + ch];
                out_buf[idx++] = (uint8_t)(s >> 8);   /* Big-Endian MSB */
                out_buf[idx++] = (uint8_t)(s & 0xFF); /* Big-Endian LSB */
            }
        }
    } else if (cfg->bits_per_sample == 24) {
        const int32_t *samples = (const int32_t *)pcm_in;
        for (uint8_t ch = 0; ch < num_channels; ch++) {
            out_buf[idx++] = 0x02; /* Subframe header: SUBFRAME_VERBATIM */
            for (size_t i = 0; i < FLAC_BLOCK_SIZE; i++) {
                uint32_t s = (uint32_t)samples[i * num_channels + ch];
                out_buf[idx++] = (uint8_t)((s >> 16) & 0xFF); /* Big-Endian Byte 2 (MSB) */
                out_buf[idx++] = (uint8_t)((s >> 8)  & 0xFF); /* Big-Endian Byte 1 */
                out_buf[idx++] = (uint8_t)(s & 0xFF);         /* Big-Endian Byte 0 (LSB) */
            }
        }
    }

    /* 3. Frame Footer (CRC-16 over entire frame) */
    uint16_t crc16 = calc_crc16(out_buf, idx);
    out_buf[idx++] = (uint8_t)(crc16 >> 8);
    out_buf[idx++] = (uint8_t)(crc16 & 0xFF);

    return idx;
}
