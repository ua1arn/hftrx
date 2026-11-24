#ifndef AUDIO_SERVER_H
#define AUDIO_SERVER_H

#include <stdint.h>

/**
 * Initialize the lwIP Raw API HTTP audio streaming server on port 80.
 * Call once after lwIP stack initialization.
 */
void audio_server_init(void);

/**
 * Push a buffer of 512 raw 16-bit PCM samples to the connected HTTP client.
 * At 16 kHz sample rate, call this function every 32 ms.
 *
 * NOTE: If using an RTOS (e.g. FreeRTOS), invoke this function from the lwIP
 * tcpip_thread context (via tcpip_callback) since lwIP Raw API is not thread-safe.
 *
 * @param pcm_512_samples Pointer to 512 16-bit signed PCM samples
 */
void audio_server_push_pcm(const int16_t *pcm_512_samples);

#endif /* AUDIO_SERVER_H */
