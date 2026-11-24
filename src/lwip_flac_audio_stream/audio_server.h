#ifndef AUDIO_SERVER_H
#define AUDIO_SERVER_H

#include <stdint.h>

/**
 * Инициализация HTTP-сервера аудиопотока (вызвать один раз при старте после инициализации lwIP)
 */
void audio_server_init(void);

/**
 * Отправка очередной порции из 512 семплов (16 кГц, 16 бит, моно) активному клиенту.
 * Вызывать каждые 32 мс при заполнении DMA/I2S буфера.
 * ВАЖНО: В FreeRTOS вызывать из потока tcpip_thread (через tcpip_callback).
 */
void audio_server_push_pcm(const int16_t *pcm_512_samples);

#endif // AUDIO_SERVER_H
