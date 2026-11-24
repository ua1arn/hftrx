#ifndef LWIPOPTS_EXAMPLE_H
#define LWIPOPTS_EXAMPLE_H

/* Recommended lwIP TCP buffer settings for real-time audio streaming */
#define TCP_MSS                 1460
#define TCP_SND_BUF             (6 * TCP_MSS)
#define TCP_SND_QUEUELEN        (2 * TCP_SND_BUF / TCP_MSS)
#define MEMP_NUM_TCP_SEG        TCP_SND_QUEUELEN

#endif /* LWIPOPTS_EXAMPLE_H */
