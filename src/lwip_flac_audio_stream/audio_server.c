#include "audio_server.h"
#include "webm_flac_stream.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

static struct tcp_pcb *active_client_pcb = NULL;
static uint32_t current_frame_num = 0;
static uint64_t total_samples_sent = 0;
static flac_audio_config_t server_audio_cfg = {48000, 1, 16};

// Buffer for WebM Cluster + Uncompressed FLAC frame (480 samples * 2 ch * 3 bytes + 64 bytes header = 2944 bytes)
// Increase multiplier from 2 to 8 if using more than 2 channels.
static uint8_t webm_cluster_buf[FLAC_BLOCK_SIZE * 2 * 3 + 64];

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

static void audio_err_callback(void *arg, err_t err) {
    struct tcp_pcb *tpcb = (struct tcp_pcb *)arg;
    if (active_client_pcb == tpcb) {
        active_client_pcb = NULL;
    }
}

static err_t html_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    close_audio_client(tpcb);
    return ERR_OK;
}

static err_t audio_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        close_audio_client(tpcb);
        return ERR_OK;
    }

    if (strncmp((char *)p->payload, "GET /stream", 11) == 0) {
        if (active_client_pcb != NULL && active_client_pcb != tpcb) {
            close_audio_client(active_client_pcb);
        }

        active_client_pcb = tpcb;
        current_frame_num = 0;
        total_samples_sent = 0;

        const char *http_header =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: audio/webm\r\n"
            "Accept-Ranges: none\r\n"
            "Cache-Control: no-cache, no-store\r\n"
            "Connection: close\r\n\r\n";

        uint8_t webm_hdr[160];
        size_t webm_hdr_len = webm_flac_write_stream_header(&server_audio_cfg, webm_hdr);

        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);

        tcp_write(tpcb, http_header, strlen(http_header), TCP_WRITE_FLAG_COPY);
        tcp_write(tpcb, webm_hdr, webm_hdr_len, TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
        return ERR_OK;
    } else {
        char html_body[320];
        int body_len = snprintf(html_body, sizeof(html_body),
            "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Live WebM FLAC</title></head><body>"
            "<h3>Live Uncompressed WebM FLAC (%lu Hz, %u ch, %u-bit)</h3>"
            "<audio controls autoplay src='/stream'></audio>"
            "</body></html>",
            (unsigned long)server_audio_cfg.sample_rate_hz,
            (unsigned int)server_audio_cfg.channels,
            (unsigned int)server_audio_cfg.bits_per_sample);

        char response[512];
        int resp_len = snprintf(response, sizeof(response),
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

static err_t audio_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_nagle_disable(newpcb);
    tcp_arg(newpcb, newpcb);
    tcp_recv(newpcb, audio_recv_callback);
    tcp_err(newpcb, audio_err_callback);
    return ERR_OK;
}

void audio_server_init(uint32_t sample_rate_hz, uint8_t channels, uint8_t bits_per_sample) {
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
    if (active_client_pcb == NULL || server_audio_cfg.sample_rate_hz == 0) {
        return;
    }

    uint32_t timestamp_ms = (uint32_t)((total_samples_sent * 1000ULL) / server_audio_cfg.sample_rate_hz);

    size_t cluster_len = webm_flac_encode_cluster(&server_audio_cfg, pcm_480_samples_per_ch,
                                                  current_frame_num, timestamp_ms, webm_cluster_buf);
    if (cluster_len == 0) {
        return;
    }

    if (tcp_sndbuf(active_client_pcb) >= cluster_len) {
        err_t err = tcp_write(active_client_pcb, webm_cluster_buf, cluster_len, TCP_WRITE_FLAG_COPY);
        if (err == ERR_OK) {
            tcp_output(active_client_pcb);
            current_frame_num++;
            total_samples_sent += FLAC_BLOCK_SIZE;
        }
    }
}
