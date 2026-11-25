#include "audio_server.h"
#include "webm_flac_stream.h"
#include "lwip/tcp.h"
#include "lwip/tcpip.h"
#include <string.h>
#include <stdio.h>
#if 0
/* Maximum supported configuration for internal static buffers (2 ch, 24-bit/32-bit container)
 * Increase channel multiplier from 2 to 8 if streaming multi-channel (>2 ch) audio. */
#define MAX_SUPPORTED_CHANNELS 2
#define MAX_BYTES_PER_SAMPLE   4

static struct tcp_pcb *active_client_pcb = NULL;
static uint32_t current_frame_num = 0;
static uint64_t total_samples_sent = 0;
static flac_audio_config_t server_audio_cfg = {48000, 1, 16};

/* Output buffer for one encoded WebM Cluster + FLAC frame (accessed only in tcpip_thread) */
static uint8_t webm_cluster_buf[FLAC_BLOCK_SIZE * MAX_SUPPORTED_CHANNELS * 3 + 64];

/* Lock-free Ping-Pong double buffer for thread-safe handoff from Audio Task / DMA ISR to tcpip_thread */
static uint8_t pcm_ping_pong_buf[2][FLAC_BLOCK_SIZE * MAX_SUPPORTED_CHANNELS * MAX_BYTES_PER_SAMPLE];
static volatile uint8_t active_ping_pong_idx = 0;
static volatile uint8_t tcpip_push_pending = 0;

/* Closes a client TCP connection cleanly (must be called from tcpip_thread context) */
static void close_audio_client(struct tcp_pcb *tpcb) {
    if (tpcb == NULL) return;
    tcp_arg(tpcb, NULL);
    tcp_recv(tpcb, NULL);
    tcp_sent(tpcb, NULL);
    tcp_err(tpcb, NULL);
    tcp_close(tpcb);
    if (active_client_pcb == tpcb) {
        active_client_pcb = NULL;
    }
}

/* Error callback invoked by lwIP inside tcpip_thread when a socket aborts or resets */
static void audio_err_callback(void *arg, err_t err) {
    (void)err;
    struct tcp_pcb *tpcb = (struct tcp_pcb *)arg;
    /* Only clear active_client_pcb if the error belongs to the currently active stream socket */
    if (active_client_pcb == tpcb) {
        active_client_pcb = NULL;
    }
}

/* Sent callback invoked by lwIP inside tcpip_thread once the browser ACKs the HTML index page */
static err_t html_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    (void)arg;
    (void)len;
    close_audio_client(tpcb);
    return ERR_OK;
}

/* Receive callback invoked by lwIP inside tcpip_thread when an HTTP request arrives */
static err_t audio_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    (void)arg;
    (void)err;

    if (p == NULL) {
        /* Remote peer closed the connection (FIN) */
        close_audio_client(tpcb);
        return ERR_OK;
    }

    if (strncmp((char *)p->payload, "GET /stream", 11) == 0) {
        /* Disconnect any previous listener so only one active stream runs at a time */
        if (active_client_pcb != NULL && active_client_pcb != tpcb) {
            close_audio_client(active_client_pcb);
        }

        active_client_pcb = tpcb;
        current_frame_num = 0;
        total_samples_sent = 0;

        static const char http_header[] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: audio/webm\r\n"
            "Accept-Ranges: none\r\n"
            "Cache-Control: no-cache, no-store\r\n"
            "Connection: close\r\n\r\n";

        uint8_t webm_hdr[WEBM_STREAM_HEADER_MAX_SIZE];
        size_t webm_hdr_len = webm_flac_write_stream_header(&server_audio_cfg, webm_hdr);

        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);

        /* Transmit HTTP response header followed immediately by WebM EBML/Tracks header */
        tcp_write(tpcb, http_header, sizeof http_header - 1, TCP_WRITE_FLAG_COPY);
        tcp_write(tpcb, webm_hdr, webm_hdr_len, TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
        return ERR_OK;
    } else {
        /* Serve minimal HTML5 player page on root '/' or any other path */
        char html_body[384];
        int body_len = snprintf(html_body, sizeof html_body,
            "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Live WebM FLAC</title></head><body>"
            "<h3>Live Uncompressed WebM-FLAC (%lu Hz, %u ch, %u-bit)</h3>"
            "<audio controls autoplay src='/stream'></audio>"
            "</body></html>",
            (unsigned long)server_audio_cfg.sample_rate_hz,
            (unsigned int)server_audio_cfg.channels,
            (unsigned int)server_audio_cfg.bits_per_sample);

        char response[576];
        int resp_len = snprintf(response, sizeof response,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n\r\n"
            "%s",
            body_len, html_body);

        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);

        /* Close connection only after TCP ACK is received in html_sent_callback */
        tcp_sent(tpcb, html_sent_callback);
        tcp_write(tpcb, response, resp_len, TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
        return ERR_OK;
    }
}

/* Accept callback invoked by lwIP inside tcpip_thread for incoming TCP connections */
static err_t audio_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    (void)arg;
    (void)err;

    /* Disable Nagle's algorithm to avoid artificial buffering delays */
    tcp_nagle_disable(newpcb);
    tcp_arg(newpcb, newpcb);
    tcp_recv(newpcb, audio_recv_callback);
    tcp_err(newpcb, audio_err_callback);
    return ERR_OK;
}

/* Internal server initialization executed strictly inside lwIP tcpip_thread */
static void audio_server_init_in_tcpip(void *arg) {
    (void)arg;
    struct tcp_pcb *pcb = tcp_new();
    if (pcb != NULL) {
        if (tcp_bind(pcb, IP_ADDR_ANY, 80) == ERR_OK) {
            pcb = tcp_listen(pcb);
            tcp_accept(pcb, audio_accept_callback);
        }
    }
}

void audio_server_init(uint32_t sample_rate_hz, uint8_t channels, uint8_t bits_per_sample) {
    server_audio_cfg.sample_rate_hz = sample_rate_hz;
    server_audio_cfg.channels = channels;
    server_audio_cfg.bits_per_sample = bits_per_sample;

    /* Schedule PCB creation inside tcpip_thread for full thread safety */
    if (tcpip_callback(audio_server_init_in_tcpip, NULL) != ERR_OK) {
        /* Fallback if called before tcpip_init() creates the mailbox */
        audio_server_init_in_tcpip(NULL);
    }
}

/* Worker executed strictly inside lwIP tcpip_thread context */
static void audio_push_in_tcpip_context(void *arg) {
    const void *pcm_slot = (const void *)arg;

    /* Release pending flag so the producer task/ISR can enqueue the next frame */
    tcpip_push_pending = 0;

    if (active_client_pcb == NULL || server_audio_cfg.sample_rate_hz == 0) {
        return;
    }

    /* Calculate exact stream timestamp in milliseconds from sample counter */
    uint32_t timestamp_ms = (uint32_t)((total_samples_sent * 1000ULL) / server_audio_cfg.sample_rate_hz);

    size_t cluster_len = webm_flac_encode_cluster(&server_audio_cfg, pcm_slot,
                                                  current_frame_num, timestamp_ms, webm_cluster_buf);
    if (cluster_len == 0) {
        return;
    }

    /* Transmit only if TCP send buffer has enough space; drop frame on network stall to prevent OOM */
    if (tcp_sndbuf(active_client_pcb) >= cluster_len) {
        err_t err = tcp_write(active_client_pcb, webm_cluster_buf, cluster_len, TCP_WRITE_FLAG_COPY);
        if (err == ERR_OK) {
            tcp_output(active_client_pcb);
            current_frame_num++;
            total_samples_sent += FLAC_BLOCK_SIZE;
        }
    }
}

void audio_server_push_pcm(const void *pcm_480_samples_per_ch) {
    if (active_client_pcb == NULL || pcm_480_samples_per_ch == NULL) {
        return;
    }

    /* Prevent queue buildup if tcpip_thread has not finished sending the previous block */
    if (tcpip_push_pending) {
        return;
    }

    if (server_audio_cfg.channels < 1 || server_audio_cfg.channels > MAX_SUPPORTED_CHANNELS) {
        return;
    }

    /* 24-bit PCM samples are passed in 32-bit int32_t containers */
    size_t container_bytes = (server_audio_cfg.bits_per_sample == 24) ? 4 : (server_audio_cfg.bits_per_sample / 8);
    size_t copy_bytes = FLAC_BLOCK_SIZE * server_audio_cfg.channels * container_bytes;

    if (copy_bytes > sizeof pcm_ping_pong_buf[0]) {
        return;
    }

    uint8_t idx = active_ping_pong_idx;
    memcpy(pcm_ping_pong_buf[idx], pcm_480_samples_per_ch, copy_bytes);

    tcpip_push_pending = 1;

    /* Non-blocking ISR-safe post to lwIP tcpip_thread mailbox */
    if (tcpip_try_callback(audio_push_in_tcpip_context, pcm_ping_pong_buf[idx]) == ERR_OK) {
        active_ping_pong_idx ^= 1;
    } else {
        tcpip_push_pending = 0;
    }
}
#endif
