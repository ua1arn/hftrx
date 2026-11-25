# lwIP Infinite Uncompressed WebM-FLAC Audio Streamer

Streams live uncompressed PCM audio wrapped in lossless `SUBFRAME_VERBATIM` FLAC frames inside an infinite WebM (`audio/webm`, `CodecID: A_FLAC`) container over HTTP using lwIP Raw API.

## Timing
* `FLAC_BLOCK_SIZE = 480` samples per channel.
* At **48,000 Hz**, call `audio_server_push_pcm()` every **10.0 ms** (100 times/sec).
* At **16,000 Hz**, call `audio_server_push_pcm()` every **30.0 ms**.

## Files
* `flac_stream.h` / `flac_stream.c`: Encodes raw Little-Endian PCM samples into uncompressed FLAC frames (`VERBATIM`).
* `webm_flac_stream.h` / `webm_flac_stream.c`: Wraps the FLAC header (`CodecPrivate`) and FLAC frames (`SimpleBlock`) into infinite-length WebM `Cluster` elements supported natively by Edge and Chrome.
* `audio_server.h` / `audio_server.c`: lwIP Raw API HTTP server.
