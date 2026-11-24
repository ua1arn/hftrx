#include "flac_stream.h"

/* Stream header: 'fLaC' marker + STREAMINFO metadata block (16000 Hz, 1 ch, 16 bps, 0 total samples) */
const uint8_t FLAC_STREAM_HEADER[42] = {
    'f', 'L', 'a', 'C',
    0x80, 0x00, 0x00, 0x22, /* Last metadata block flag (0x80) + STREAMINFO type (0) + 34-byte length */
    0x02, 0x00,             /* Minimum block size = 512 samples */
    0x02, 0x00,             /* Maximum block size = 512 samples */
    0x00, 0x00, 0x00,       /* Minimum frame size = 0 (unknown) */
    0x00, 0x00, 0x00,       /* Maximum frame size = 0 (unknown) */
    0x03, 0xE8, 0x00, 0xF0, /* Sample rate: 16000 Hz (0x03E80), Channels: 1 (0), Bits per sample: 16 (15 = 0xF) */
    0x00, 0x00, 0x00, 0x00, /* Total samples in stream = 0 (indicates infinite live stream) */
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 /* 16-byte MD5 signature (zeros allowed for live streaming) */
};

/* Calculate CRC-8 (polynomial 0x07) for FLAC frame header */
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

/* Calculate CRC-16 (polynomial 0x8005) for the entire FLAC frame */
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

/* Encode frame number using FLAC's UTF-8-like variable-length integer format */
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

size_t flac_encode_verbatim_frame(const int16_t *pcm_in, uint32_t frame_num, uint8_t *out_buf) {
    size_t idx = 0;

    /* 1. FLAC Frame Header */
    out_buf[idx++] = 0xFF; /* Sync code (14 bits: 11111111 111110) + reserved (0) + blocking strategy (0 = fixed) */
    out_buf[idx++] = 0xF8;
    out_buf[idx++] = 0x78; /* Block size code: 0111 (16-bit size at end of header), Sample rate: 1000 (16 kHz) */
    out_buf[idx++] = 0x08; /* Channel assignment: 0000 (1 ch mono), Sample size: 100 (16-bit), Reserved: 0 */

    /* UTF-8 encoded sequential frame number */
    idx += encode_utf8_uint32(frame_num, &out_buf[idx]);

    /* 16-bit (block_size - 1) in Big-Endian (512 - 1 = 511 = 0x01FF) */
    out_buf[idx++] = (uint8_t)((FLAC_BLOCK_SIZE - 1) >> 8);
    out_buf[idx++] = (uint8_t)((FLAC_BLOCK_SIZE - 1) & 0xFF);

    /* Frame header CRC-8 checksum */
    out_buf[idx] = calc_crc8(out_buf, idx);
    idx++;

    /* 2. Subframe Header: 0 (pad) + 000001 (SUBFRAME_VERBATIM = uncompressed) + 0 (no wasted bits) */
    out_buf[idx++] = 0x02;

    /* 3. Uncompressed PCM Audio Samples (convert from Little-Endian MCU format to Big-Endian FLAC format) */
    for (size_t i = 0; i < FLAC_BLOCK_SIZE; i++) {
        uint16_t sample = (uint16_t)pcm_in[i];
        out_buf[idx++] = (uint8_t)(sample >> 8);   /* MSB */
        out_buf[idx++] = (uint8_t)(sample & 0xFF); /* LSB */
    }

    /* 4. Frame Footer: CRC-16 checksum over the entire frame */
    uint16_t crc16 = calc_crc16(out_buf, idx);
    out_buf[idx++] = (uint8_t)(crc16 >> 8);
    out_buf[idx++] = (uint8_t)(crc16 & 0xFF);

    return idx;
}
