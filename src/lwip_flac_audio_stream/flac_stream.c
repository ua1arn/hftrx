#include "hardware.h"
#include "flac_stream.h"

// Заголовок: 'fLaC' + STREAMINFO (16000 Гц, 1 канал, 16 бит, 0 семплов = бесконечность)
const uint8_t FLAC_STREAM_HEADER[42] = {
    'f', 'L', 'a', 'C',
    0x80, 0x00, 0x00, 0x22, // Last metadata block + STREAMINFO (34 байта)
    0x02, 0x00,             // Min block size = 512
    0x02, 0x00,             // Max block size = 512
    0x00, 0x00, 0x00,       // Min frame size = 0 (неизвестно)
    0x00, 0x00, 0x00,       // Max frame size = 0 (неизвестно)
    0x03, 0xE8, 0x00, 0xF0, // 16000 Hz (0x03E80), 1 ch (0), 16 bps (15 = 0xF)
    0x00, 0x00, 0x00, 0x00, // Total samples = 0 (бесконечный поток)
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0 // MD5 = 0
};

// Расчет CRC-8 (полином 0x07) для заголовка фрейма FLAC
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

// Расчет CRC-16 (полином 0x8005) для всего фрейма FLAC
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

// Кодирование номера фрейма в формат UTF-8 (по спецификации FLAC)
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

    // 1. Заголовок фрейма FLAC
    out_buf[idx++] = 0xFF; // Sync code (14 бит: 11111111 111110) + reserved (0) + blocking strategy (0 = fixed)
    out_buf[idx++] = 0xF8;
    out_buf[idx++] = 0x68; // Block size: 0110 (читается из конца заголовка как 16-бит), Sample rate: 1000 (16 кГц)
    out_buf[idx++] = 0x08; // Channel: 0000 (1 канал моно), Sample size: 100 (16 бит), Reserved: 0

    // Номер фрейма в UTF-8
    idx += encode_utf8_uint32(frame_num, &out_buf[idx]);

    // Размер блока - 1 (512 - 1 = 511 = 0x01FF) в формате Big-Endian
    out_buf[idx++] = (uint8_t)((FLAC_BLOCK_SIZE - 1) >> 8);
    out_buf[idx++] = (uint8_t)((FLAC_BLOCK_SIZE - 1) & 0xFF);

    // Контрольная сумма заголовка фрейма (CRC-8)
    out_buf[idx] = calc_crc8(out_buf, idx);
    idx++;

    // 2. Заголовок субфрейма (Subframe header)
    // 0 (padding) + 000001 (тип SUBFRAME_VERBATIM = несжатые данные) + 0 (нет wasted bits)
    out_buf[idx++] = 0x02;

    // 3. Сырые аудиоданные (перевод из Little-Endian микроконтроллера в Big-Endian для FLAC)
    for (size_t i = 0; i < FLAC_BLOCK_SIZE; i++) {
        uint16_t sample = (uint16_t)pcm_in[i];
        out_buf[idx++] = (uint8_t)(sample >> 8);   // Старший байт
        out_buf[idx++] = (uint8_t)(sample & 0xFF); // Младший байт
    }

    // 4. Футер фрейма (CRC-16 от всего фрейма)
    uint16_t crc16 = calc_crc16(out_buf, idx);
    out_buf[idx++] = (uint8_t)(crc16 >> 8);
    out_buf[idx++] = (uint8_t)(crc16 & 0xFF);

    return idx;
}
