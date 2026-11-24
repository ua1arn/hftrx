#ifndef LWIPOPTS_EXAMPLE_H
#define LWIPOPTS_EXAMPLE_H

/* Recommended lwIP TCP buffer settings for uncompressed OggFLAC streaming
 * Examples of stream bitrates:
 * - 16 kHz, 1 ch, 16-bit:  ~33 KB/s  (~265 kbps)
 * - 48 kHz, 1 ch, 16-bit:  ~99 KB/s  (~790 kbps)
 * - 48 kHz, 2 ch, 16-bit: ~198 KB/s (~1.58 Mbps)
 * - 48 kHz, 2 ch, 24-bit: ~297 KB/s (~2.38 Mbps)
 */

#define TCP_MSS                 1460
#define TCP_WND                 (8 * TCP_MSS)
#define TCP_SND_BUF             (16 * TCP_MSS)
#define TCP_SND_QUEUELEN        (2 * TCP_SND_BUF / TCP_MSS)

/* Ensure enough PBUF pool entries for continuous streaming */
#define PBUF_POOL_SIZE          32

#endif /* LWIPOPTS_EXAMPLE_H */
