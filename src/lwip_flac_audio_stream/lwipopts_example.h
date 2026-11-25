/* Recommended lwipopts.h settings for thread-safe real-time WebM-FLAC streaming */

#ifndef LWIPOPTS_EXAMPLE_H
#define LWIPOPTS_EXAMPLE_H

/* TCP Segment and Send Buffer sizes (at least 16 * MSS for 48 kHz stereo stability) */
#define TCP_MSS                 1460
#define TCP_SND_BUF             (16 * TCP_MSS)
#define TCP_SND_QUEUELEN        (2 * TCP_SND_BUF / TCP_MSS)

/* Ensure enough PBUF pool buffers for outgoing audio frames + incoming ACKs */
#define PBUF_POOL_SIZE          32

/* Essential for thread-safe tcpip_try_callback() calls from Audio Task / DMA ISR */
#define TCPIP_MBOX_SIZE         32

#endif /* LWIPOPTS_EXAMPLE_H */
