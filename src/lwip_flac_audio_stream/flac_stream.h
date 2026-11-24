#ifndef FLAC_STREAM_H
#define FLAC_STREAM_H

#include <stdint.h>
#include <stddef.h>

// Размер одного блока (рекомендуется 512, чтобы фрейм целиком влезал в один TCP-пакет)
#define FLAC_BLOCK_SIZE 512

// Глобальный 42-байтный заголовок потока (16 кГц, 16 бит, моно, бесконечная длина)
extern const uint8_t FLAC_STREAM_HEADER[42];

/**
 * Упаковывает 512 сырых 16-битных семплов (Little-Endian, как обычно отдают АЦП/I2S)
 * в готовый FLAC-фрейм (режим VERBATIM - без сжатия).
 * 
 * @param pcm_in      Указатель на массив из FLAC_BLOCK_SIZE (512) семплов
 * @param frame_num   Порядковый номер фрейма (инкрементируется с каждым пакетом: 0, 1, 2...)
 * @param out_buf     Выходной буфер (размер должен быть не менее FLAC_BLOCK_SIZE * 2 + 16 байт)
 * @return            Итоговый размер сформированного FLAC-фрейма в байтах
 */
size_t flac_encode_verbatim_frame(const int16_t *pcm_in, uint32_t frame_num, uint8_t *out_buf);

#endif // FLAC_STREAM_H
