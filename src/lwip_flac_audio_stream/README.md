# lwIP Infinite Uncompressed Audio Stream (FLAC Verbatim)

This project implements a zero-compression, infinite-duration HTTP audio streamer using **lwIP Raw API** and **HTML5 `<audio>`**.

## Key Fixes in v3
1. **FLAC Frame Header (`0x78`)**: Fixed block size code from `0x68` (8-bit size) to `0x78` (16-bit size) so browsers properly parse 512-sample frames (`0x01FF`).
2. **Default Page (`GET /`)**: Added explicit `Content-Length` and deferred `tcp_close()` until `html_sent_callback` triggers on TCP ACK.
3. **Socket Cleanup Safety**: Added `tcp_arg(newpcb, newpcb)` so closing a stale/secondary browser connection does not clear `active_client_pcb`.

## Usage
1. Call `audio_server_init()` after lwIP network interface initialization.
2. Every 32 ms (when 512 samples are ready at 16 kHz), call `audio_server_push_pcm(pcm_buffer)`.
   * *Note for FreeRTOS users:* Wrap `audio_server_push_pcm` inside `tcpip_callback()` to keep lwIP Raw API calls thread-safe.
