#include "audio_server.h"
#include "ogg_flac_stream.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

static struct tcp_pcb *active_client_pcb = NULL;
static uint32_t current_frame_num = 0;
static flac_audio_config_t server_audio_cfg = {48000, 1, 16};

// Buffer for Ogg Page (480 samples * 8 ch * 3 bytes + headers <= 11650 bytes)
// Can be reduced to 2100 bytes if you only use up to 2 channels / 16-bit
static uint8_t ogg_page_buf[FLAC_BLOCK_SIZE * 8 * 3 + 128];

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

        const char *http_header =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: audio/ogg; codecs=\"flac\"\r\n"
            "Accept-Ranges: none\r\n"
            "Cache-Control: no-cache, no-store\r\n"
            "Connection: close\r\n\r\n";

        uint8_t ogg_hdr[160];
        size_t ogg_hdr_len = ogg_flac_write_headers(&server_audio_cfg, ogg_hdr);

        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);

        tcp_write(tpcb, http_header, strlen(http_header), TCP_WRITE_FLAG_COPY);
        tcp_write(tpcb, ogg_hdr, ogg_hdr_len, TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
        return ERR_OK;
    } else {
        char html_body[320];
        int body_len = snprintf(html_body, sizeof(html_body),
            "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Live OggFLAC</title></head><body>"
            "<h3>Live Uncompressed OggFLAC (%lu Hz, %u ch, %u-bit)</h3>"
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
    if (active_client_pcb == NULL) {
        return;
    }

    size_t page_len = ogg_flac_encode_page(&server_audio_cfg, pcm_480_samples_per_ch, current_frame_num, ogg_page_buf);
    if (page_len == 0) {
        return;
    }

    if (tcp_sndbuf(active_client_pcb) >= page_len) {
        err_t err = tcp_write(active_client_pcb, ogg_page_buf, page_len, TCP_WRITE_FLAG_COPY);
        if (err == ERR_OK) {
            tcp_output(active_client_pcb);
            current_frame_num++;
        }
    }
}
