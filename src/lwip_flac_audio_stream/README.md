# lwIP Infinite Uncompressed WebM-FLAC Audio Streamer

Streams live, lossless, uncompressed PCM audio wrapped in `SUBFRAME_VERBATIM` FLAC frames inside a WebM (`audio/webm`, `CodecID: A_FLAC`) container over lwIP Raw API.

## Block Size & Call Period
Each call to `audio_server_push_pcm()` expects **480 samples per channel**:
* **48 000 Hz**: every **10.0 ms** (100 calls/sec)
* **24 000 Hz**: every **20.0 ms** (50 calls/sec)
* **16 000 Hz**: every **30.0 ms** (33.33 calls/sec)
* **12 000 Hz**: every **40.0 ms** (25 calls/sec)
* **8 000 Hz**: every **60.0 ms** (16.67 calls/sec)

## Usage Example
```c
#include "audio_server.h"

/* Initialize server: 48 kHz, 1 channel (mono), 16-bit PCM */
audio_server_init(48000, 1, 16);

/* Inside your 10 ms timer / DMA callback (via tcpip_callback if using FreeRTOS): */
int16_t pcm_buffer[480];
audio_server_push_pcm(pcm_buffer);
```
