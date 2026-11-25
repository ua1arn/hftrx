/* Recommended TCP buffer settings in lwipopts.h for real-time audio streaming */
#define TCP_MSS                 1460
#define TCP_WND                 (8 * TCP_MSS)
#define TCP_SND_BUF             (16 * TCP_MSS)
#define TCP_SND_QUEUELEN        (2 * TCP_SND_BUF / TCP_MSS)
#define MEMP_NUM_TCP_SEG        TCP_SND_QUEUELEN
#define LWIP_TCP_NODELAY        1
