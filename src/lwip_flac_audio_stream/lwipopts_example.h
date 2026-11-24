/* Рекомендуемые настройки для lwipopts.h */
#define TCP_MSS                 1460
#define TCP_SND_BUF             (6 * TCP_MSS)
#define TCP_SND_QUEUELEN        (2 * TCP_SND_BUF / TCP_MSS)
