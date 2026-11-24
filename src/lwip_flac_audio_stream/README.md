# lwIP Configurable Uncompressed OggFLAC Audio Stream Server

Infinite, lossless, uncompressed PCM audio streaming over HTTP using **lwIP Raw API** and **OggFLAC (VERBATIM mode)** with runtime configuration for **sample rate**, **channel count**, and **bit depth**.

## Fix in v8
- Fixed C string literal formatting (`\r\n` CRLF line endings instead of unescaped newlines) in `http_header` and `response` inside `audio_server.c`.

## Quick Start

1. Initialize the server with your desired format parameters:
   ```c
   /* Example: 48 kHz, 2 channels (stereo), 16-bit PCM */
   audio_server_init(48000, 2, 16);
   ```

2. Every time your I2S/ADC DMA collects `FLAC_BLOCK_SIZE` (512) frames (samples per channel), pass the interleaved buffer to `audio_server_push_pcm()`:
   - **8-bit:** `int8_t buf[512 * channels]`
   - **16-bit:** `int16_t buf[512 * channels]` (e.g. `[L0, R0, L1, R1, ...]`)
   - **24-bit:** `int32_t buf[512 * channels]` (24-bit signed values in 32-bit words)
