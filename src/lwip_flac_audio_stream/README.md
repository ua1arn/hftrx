# Thread-Safe Uncompressed WebM-FLAC Live Audio Server for lwIP

Streams infinite, bit-exact uncompressed PCM audio wrapped in **WebM (`CodecID: A_FLAC`, `SUBFRAME_VERBATIM`)** over HTTP using the lwIP Raw API. Fully compatible with Edge, Chrome, Firefox, and Safari.

## Features
- **Thread-Safe & ISR-Safe:** `audio_server_push_pcm()` uses a lock-free Ping-Pong double buffer and `tcpip_try_callback()` so all lwIP Raw API calls (`tcp_write`, `tcp_output`, `tcp_sent`, `tcp_close`) run strictly inside `tcpip_thread`.
- **Zero Compression Overhead:** Uses FLAC `VERBATIM` frames (no LPC/FIR math) inside a WebM infinite segment (`0x01FFFFFFFFFFFFFF`).
- **Embedded-Friendly Style:** All lookup/header tables use `static const` (stored in Flash `.rodata`, zero stack allocation) and parenthesis-free `sizeof` on variables/arrays.

## Call Periods (`FLAC_BLOCK_SIZE = 480` samples/channel)
Call `audio_server_push_pcm()` once every 480 samples per channel:
- **48,000 Hz:** every **10.0 ms** (100 calls/sec)
- **24,000 Hz:** every **20.0 ms** (50 calls/sec)
- **16,000 Hz:** every **30.0 ms** (33.33 calls/sec)
- **12,000 Hz:** every **40.0 ms** (25 calls/sec)

## Usage Example
```c
#include "audio_server.h"

/* Initialize server (e.g., 48 kHz, 1 channel mono, 16-bit PCM) */
audio_server_init(48000, 1, 16);

/* Inside your 10 ms Audio Task or DMA Half/Full Transfer ISR: */
int16_t pcm_block[480];
audio_server_push_pcm(pcm_block);
```
