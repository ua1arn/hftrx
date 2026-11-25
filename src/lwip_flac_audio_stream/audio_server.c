#include "hardware.h"
#include "clocks.h"

#include "audio_server.h"
#include "webm_flac_stream.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

/* Currently connected streaming client PCB (single-listener implementation) */
static struct tcp_pcb *active_client_pcb = NULL;

/* Sequential frame counter and total samples transmitted since client connection */
static uint32_t current_frame_num = 0;
static uint64_t total_samples_sent = 0;
static hwtimcount_t ts_connetat;

/* Active audio configuration (defaults to 48 kHz, 1 channel, 16-bit) */
static flac_audio_config_t server_audio_cfg = {48000, 1, 16};

/* Transmit buffer for one WebM Cluster + FLAC frame:
 * Max size for 480 samples * 8 ch * 3 bytes = 11520 bytes + ~64 bytes headers.
 * Can be reduced to 2100 bytes if using at most 2 channels / 16-bit audio. */
static uint8_t webm_cluster_buf[FLAC_BLOCK_SIZE * 8 * 3 + 64];

/* Safely detaches callbacks and closes a TCP connection */
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

/* Error callback: clears active_client_pcb only if the error belongs to the active stream socket */
static void audio_err_callback(void *arg, err_t err) {
    (void)err;
    struct tcp_pcb *tpcb = (struct tcp_pcb *)arg;
    if (active_client_pcb == tpcb) {
        active_client_pcb = NULL;
    }
}

/* Sent callback for static HTML page: closes connection only after browser ACKs all HTML bytes */
static err_t html_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    (void)arg;
    (void)len;
    close_audio_client(tpcb);
    return ERR_OK;
}

/* HTTP request handler for GET /stream and GET / */
static err_t audio_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    (void)arg;
    (void)err;

    if (p == NULL) {
        /* Client closed connection (FIN received) */
        close_audio_client(tpcb);
        return ERR_OK;
    }

    if (strncmp((char *)p->payload, "GET /stream", 11) == 0) {
        /* If another listener was active, close its old connection cleanly */
        if (active_client_pcb != NULL && active_client_pcb != tpcb) {
            close_audio_client(active_client_pcb);
        }

        active_client_pcb = tpcb;
        current_frame_num = 0;
        total_samples_sent = 0;

        ts_connetat = timestamp_getticks();

        /* HTTP/1.1 streaming response without Content-Length and with Accept-Ranges disabled */
        static const char http_header[] =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: audio/webm\r\n"
            "Accept-Ranges: none\r\n"
            "Cache-Control: no-cache, no-store\r\n"
            "Connection: close\r\n\r\n";

        uint8_t webm_hdr[160];
        size_t webm_hdr_len = webm_flac_write_stream_header(&server_audio_cfg, webm_hdr);

        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);

        /* Send HTTP header immediately followed by EBML + Segment + Tracks + FLAC STREAMINFO */
        tcp_write(tpcb, http_header, sizeof http_header - 1, TCP_WRITE_FLAG_COPY);
        tcp_write(tpcb, webm_hdr, webm_hdr_len, TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
        return ERR_OK;
    } else {
        /* Serve simple HTML5 player page with exact Content-Length */
        char html_body[384];
        int body_len = snprintf(html_body, sizeof html_body,
            "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Live WebM FLAC</title></head><body>"
            "<h3>Live Uncompressed WebM FLAC (%lu Hz, %u ch, %u-bit)</h3>"
            "<audio controls autoplay src='/stream'></audio>"
            "</body></html>",
            (unsigned long)server_audio_cfg.sample_rate_hz,
            (unsigned int)server_audio_cfg.channels,
            (unsigned int)server_audio_cfg.bits_per_sample);

        char response[640];
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

    /* Disable Nagle's algorithm to transmit audio clusters without artificial 200ms delay */
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

    /* Compute exact cluster timestamp in milliseconds from total transmitted samples */
    //uint32_t timestamp_ms = (uint32_t)((total_samples_sent * 1000ULL) / server_audio_cfg.sample_rate_hz);
    uint32_t timestamp_ms = (timestamp_getmask() & (timestamp_getticks() - ts_connetat)) / (timestamp_getfreq() / 1000);

    size_t cluster_len = webm_flac_encode_cluster(&server_audio_cfg, pcm_480_samples_per_ch,
                                                  current_frame_num, timestamp_ms, webm_cluster_buf);
    if (cluster_len == 0) {
        return;
    }

    /* Drop packet if TCP send buffer is congested to prevent RAM exhaustion on Wi-Fi hiccups */
    if (tcp_sndbuf(active_client_pcb) >= cluster_len) {
        err_t err = tcp_write(active_client_pcb, webm_cluster_buf, cluster_len, TCP_WRITE_FLAG_COPY);
        if (err == ERR_OK) {
            tcp_output(active_client_pcb);
            current_frame_num++;
            total_samples_sent += FLAC_BLOCK_SIZE;
        }
    }
}
