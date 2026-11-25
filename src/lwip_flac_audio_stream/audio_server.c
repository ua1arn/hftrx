#include "hardware.h"
#if WITHLWIP
#include "clocks.h"

#include "audio_server.h"
#include "webm_flac_stream.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

/* Maximum number of simultaneous audio stream listeners */
#define MAX_AUDIO_CLIENTS 4

/* Per-client streaming state */
typedef struct {
    struct tcp_pcb *pcb;
    uint32_t frame_num;
    hwtimcount_t ts_connected;
} audio_client_t;

/* Pool of active streaming clients */
static audio_client_t clients[MAX_AUDIO_CLIENTS];

/* Active audio configuration (defaults to 48 kHz, 1 channel, 16-bit) */
static flac_audio_config_t server_audio_cfg = {48000, 1, 16};

/* Transmit buffer for one WebM Cluster + FLAC frame */
static uint8_t webm_cluster_buf[FLAC_BLOCK_SIZE * 8 * 3 + 64];

/* Finds client slot by tcp_pcb pointer, returns NULL if not found */
static audio_client_t *find_client_by_pcb(struct tcp_pcb *tpcb) {
    if (tpcb == NULL) return NULL;
    for (int i = 0; i < MAX_AUDIO_CLIENTS; i++) {
        if (clients[i].pcb == tpcb) {
            return &clients[i];
        }
    }
    return NULL;
}

/* Safely detaches callbacks, closes TCP connection, and frees client slot if applicable */
static void close_tcp_connection(struct tcp_pcb *tpcb) {
    if (tpcb == NULL) return;

    audio_client_t *client = find_client_by_pcb(tpcb);
    if (client != NULL) {
        client->pcb = NULL;
    }

    tcp_arg(tpcb, NULL);
    tcp_recv(tpcb, NULL);
    tcp_sent(tpcb, NULL);
    tcp_err(tpcb, NULL);
    tcp_close(tpcb);
}

/* Error callback: frees the client slot if lwIP aborts/resets the connection */
static void audio_err_callback(void *arg, err_t err) {
    (void)err;
    struct tcp_pcb *tpcb = (struct tcp_pcb *)arg;
    audio_client_t *client = find_client_by_pcb(tpcb);
    if (client != NULL) {
        client->pcb = NULL;
    }
}

/* Sent callback for static HTML page: closes connection after browser ACKs all HTML bytes */
static err_t html_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    (void)arg;
    (void)len;
    close_tcp_connection(tpcb);
    return ERR_OK;
}

/* HTTP request handler for GET /stream and GET / */
static err_t audio_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    (void)arg;
    (void)err;

    if (p == NULL) {
        /* Client closed connection (FIN received) */
        close_tcp_connection(tpcb);
        return ERR_OK;
    }

    if (strncmp((char *)p->payload, "GET /stream", 11) == 0) {
        /* Close any previous stream connection from the same remote IP (handles browser double-requests) */
        for (int i = 0; i < MAX_AUDIO_CLIENTS; i++) {
            if (clients[i].pcb != NULL && clients[i].pcb != tpcb &&
                ip_addr_cmp(&clients[i].pcb->remote_ip, &tpcb->remote_ip)) {
                close_tcp_connection(clients[i].pcb);
            }
        }

        /* Find a free slot for the new listener */
        audio_client_t *slot = NULL;
        for (int i = 0; i < MAX_AUDIO_CLIENTS; i++) {
            if (clients[i].pcb == NULL) {
                slot = &clients[i];
                break;
            }
        }

        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);

        if (slot == NULL) {
            /* All listener slots are busy -> return 503 Service Unavailable */
            static const char busy_resp[] =
                "HTTP/1.1 503 Service Unavailable\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 19\r\n"
                "Connection: close\r\n\r\n"
                "Server max clients.";

            tcp_sent(tpcb, html_sent_callback);
            tcp_write(tpcb, busy_resp, sizeof busy_resp - 1, TCP_WRITE_FLAG_COPY);
            tcp_output(tpcb);
            return ERR_OK;
        }

        /* Register new listener */
        slot->pcb = tpcb;
        slot->frame_num = 0;
        slot->ts_connected = timestamp_getticks();

        static const char http_header[] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: audio/webm\r\n"
            "Accept-Ranges: none\r\n"
            "Cache-Control: no-cache, no-store\r\n"
            "Connection: close\r\n\r\n";

        uint8_t webm_hdr[160];
        size_t webm_hdr_len = webm_flac_write_stream_header(&server_audio_cfg, webm_hdr);

        tcp_write(tpcb, http_header, sizeof http_header - 1, TCP_WRITE_FLAG_COPY);
        tcp_write(tpcb, webm_hdr, webm_hdr_len, TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
        return ERR_OK;
    } else {
        /* Count active listeners to display on the web page */
        unsigned int active_count = 0;
        for (int i = 0; i < MAX_AUDIO_CLIENTS; i++) {
            if (clients[i].pcb != NULL) active_count++;
        }

        char html_body[448];
        int body_len = snprintf(html_body, sizeof html_body,
            "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Live WebM FLAC</title></head><body>"
			"<h3>Live Uncompressed WebM FLAC (%lu Hz, %u ch, %u-bit)</h3>"
			"<h3>RX Frequency: (%u Hz)</h3>"
            "<p>Listeners: %u / %u</p>"
            "<audio controls autoplay src='/stream'></audio>"
            "</body></html>",
            (unsigned long)server_audio_cfg.sample_rate_hz,
            (unsigned int)server_audio_cfg.channels,
            (unsigned int)server_audio_cfg.bits_per_sample,
			(unsigned) hamradio_get_freq_rx(),
            active_count, (unsigned int)MAX_AUDIO_CLIENTS);

        char response[704];
        int resp_len = snprintf(response, sizeof response,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n\r\n"
            "%s",
            body_len, html_body);

        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);

        tcp_sent(tpcb, html_sent_callback);
        tcp_write(tpcb, response, resp_len, TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
        return ERR_OK;
    }
}

/* Accepts new incoming TCP connections on port 80 */
static err_t audio_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    (void)arg;
    (void)err;

    tcp_nagle_disable(newpcb);
    tcp_arg(newpcb, newpcb);
    tcp_recv(newpcb, audio_recv_callback);
    tcp_err(newpcb, audio_err_callback);
    return ERR_OK;
}

void audio_server_init(uint32_t sample_rate_hz, uint8_t channels, uint8_t bits_per_sample) {
    memset(clients, 0, sizeof clients);

    server_audio_cfg.sample_rate_hz = sample_rate_hz;
    server_audio_cfg.channels = channels;
    server_audio_cfg.bits_per_sample = bits_per_sample;

    struct tcp_pcb *pcb = tcp_new();
    if (pcb != NULL) {
        if (tcp_bind(pcb, IP_ADDR_ANY, 80) == ERR_OK) {
            pcb = tcp_listen(pcb);
            tcp_accept(pcb, audio_accept_callback);
        }
    }
}

void audio_server_push_pcm(const void *pcm_480_samples_per_ch) {
    if (server_audio_cfg.sample_rate_hz == 0) {
        return;
    }

    /* Check if at least one listener is connected before spending CPU on FLAC encoding */
    int has_clients = 0;
    for (int i = 0; i < MAX_AUDIO_CLIENTS; i++) {
        if (clients[i].pcb != NULL) {
            has_clients = 1;
            break;
        }
    }
    if (!has_clients) {
        return;
    }

    /* Encode FLAC frame once (using frame_num = 0; WebM SimpleBlock uses Cluster Timecode for sync) */
    static uint32_t global_frame_num = 0; // Добавьте в начало audio_server.c

    const hwtimcount_t now = timestamp_getticks();
    const uint32_t ticks_per_ms = timestamp_getfreq() / 1000;
    const uint32_t encoder_ts_ms = now / ticks_per_ms;

    size_t cluster_len = webm_flac_encode_cluster(&server_audio_cfg, pcm_480_samples_per_ch,
                                                  global_frame_num++, encoder_ts_ms, webm_cluster_buf);
    if (cluster_len == 0) {
        return;
    }

    /* Broadcast the encoded cluster to all connected listeners */
    for (int i = 0; i < MAX_AUDIO_CLIENTS; i++) {
        if (clients[i].pcb == NULL) {
            continue;
        }

        /* Patch per-client 32-bit Big-Endian Cluster Timecode at bytes 10..13 of webm_cluster_buf */
        uint32_t client_ts_ms = (timestamp_getmask() & (now - clients[i].ts_connected)) / ticks_per_ms;
        webm_cluster_buf[10] = (uint8_t)(client_ts_ms >> 24);
        webm_cluster_buf[11] = (uint8_t)(client_ts_ms >> 16);
        webm_cluster_buf[12] = (uint8_t)(client_ts_ms >> 8);
        webm_cluster_buf[13] = (uint8_t)(client_ts_ms & 0xFF);

        /* Send only if this client's TCP send buffer has enough space */
        if (tcp_sndbuf(clients[i].pcb) >= cluster_len) {
            if (tcp_write(clients[i].pcb, webm_cluster_buf, cluster_len, TCP_WRITE_FLAG_COPY) == ERR_OK) {
                tcp_output(clients[i].pcb);
                clients[i].frame_num++;
            }
        }
    }
}
#endif
