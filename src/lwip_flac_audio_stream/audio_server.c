#include "hardware.h"
#include "formats.h"

#include "audio_server.h"
#include "ogg_flac_stream.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

/* Pointer to the currently active listener connection */
static struct tcp_pcb *active_client_pcb = NULL;
static uint32_t current_frame_num = 0;

/* Configured audio format parameters */
static flac_audio_config_t stream_cfg = { 48000, 1, 16 };

/* Buffer for building one complete OggFLAC audio page */
static uint8_t ogg_page_buf[OGG_FLAC_MAX_PAGE_SIZE];

/* Safely close a client connection */
static void close_audio_client(struct tcp_pcb *tpcb) {
    if (tpcb == NULL) return;
    tcp_arg(tpcb, NULL);
    tcp_recv(tpcb, NULL);
    tcp_err(tpcb, NULL);
    tcp_sent(tpcb, NULL);
    tcp_close(tpcb);
    if (active_client_pcb == tpcb) {
        active_client_pcb = NULL;
    }
}

/* Error callback: clear active_client_pcb only if the error belongs to the active socket */
static void audio_err_callback(void *arg, err_t err) {
    (void)err;
    struct tcp_pcb *tpcb = (struct tcp_pcb *)arg;
    if (active_client_pcb == tpcb) {
        active_client_pcb = NULL;
    }
}

/* Sent callback for static HTML page: close socket only after browser ACKs receipt */
static err_t html_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    (void)arg;
    (void)len;
    close_audio_client(tpcb);
    return ERR_OK;
}

/* Handle incoming HTTP requests */
static err_t audio_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    (void)arg;
    (void)err;

    if (p == NULL) {
        /* Remote client closed the connection */
        close_audio_client(tpcb);
        return ERR_OK;
    }

    /* Check if the browser requested the audio stream */
    if (strncmp((char *)p->payload, "GET /stream", 11) == 0) {
        /* Disconnect previous listener if a new one connects */
        if (active_client_pcb != NULL && active_client_pcb != tpcb) {
            close_audio_client(active_client_pcb);
        }

        active_client_pcb = tpcb;
        current_frame_num = 0;

        /* Acknowledge incoming HTTP request bytes */
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);

        /* Send HTTP 200 OK header for infinite Ogg audio stream */
        const char *http_header =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: audio/ogg\r\n"
            "Accept-Ranges: none\r\n"
            "Cache-Control: no-cache, no-store\r\n"
            "Connection: close\r\n\r\n";

        /* Generate initial OggFLAC headers (Page 0: BOS + STREAMINFO, Page 1: Vorbis Comment) */
        uint8_t ogg_headers[OGG_FLAC_HEADERS_MAX_SIZE];
        size_t ogg_hdr_len = ogg_flac_write_headers(&stream_cfg, ogg_headers);

        tcp_write(tpcb, http_header, (u16_t)strlen(http_header), TCP_WRITE_FLAG_COPY);
        if (ogg_hdr_len > 0) {
            tcp_write(tpcb, ogg_headers, (u16_t)ogg_hdr_len, TCP_WRITE_FLAG_COPY);
        }
        tcp_output(tpcb);

        return ERR_OK;
    } else {
        /* Serve a simple HTML5 player page with exact Content-Length on any other path */
        char html_body[384];
        int body_len = snprintf(html_body, sizeof(html_body),
            "<!DOCTYPE html><html><head><meta charset='utf-8'>"
            "<title>Live OggFLAC Stream</title></head><body>"
            "<h3>Live Uncompressed OggFLAC Stream (%lu Hz, %u ch, %u-bit)</h3>"
            "<audio controls autoplay src='/stream'></audio>"
            "</body></html>",
            (unsigned long)stream_cfg.sample_rate_hz,
            (unsigned int)stream_cfg.channels,
            (unsigned int)stream_cfg.bits_per_sample);

        char response[640];
        int resp_len = snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n\r\n"
            "%s",
            body_len, html_body);

        /* Acknowledge incoming bytes and free pbuf before closing */
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);

        /* Close connection only after TCP ACK confirms delivery */
        tcp_sent(tpcb, html_sent_callback);

        if (resp_len > 0 && resp_len < (int)sizeof(response)) {
            tcp_write(tpcb, response, (u16_t)resp_len, TCP_WRITE_FLAG_COPY);
            tcp_output(tpcb);
        } else {
            close_audio_client(tpcb);
        }

        return ERR_OK;
    }
}

/* Accept new incoming TCP connection */
static err_t audio_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    (void)arg;
    (void)err;

    /* Disable Nagle's algorithm for minimal audio latency */
    tcp_nagle_disable(newpcb);

    /* Pass newpcb as callback argument so error handler can identify the socket */
    tcp_arg(newpcb, newpcb);
    tcp_recv(newpcb, audio_recv_callback);
    tcp_err(newpcb, audio_err_callback);
    return ERR_OK;
}

int audio_server_init(uint32_t sample_rate_hz, uint8_t channels, uint8_t bits_per_sample) {
    if (sample_rate_hz == 0 || sample_rate_hz > 655350 ||
        channels < 1 || channels > FLAC_MAX_CHANNELS ||
        (bits_per_sample != 8 && bits_per_sample != 16 && bits_per_sample != 24)) {
        return -1;
    }

    stream_cfg.sample_rate_hz  = sample_rate_hz;
    stream_cfg.channels        = channels;
    stream_cfg.bits_per_sample = bits_per_sample;

    struct tcp_pcb *pcb = tcp_new();
    if (pcb != NULL) {
        if (tcp_bind(pcb, IP_ADDR_ANY, 80) == ERR_OK) {
            pcb = tcp_listen(pcb);
            tcp_accept(pcb, audio_accept_callback);
            return 0;
        }
        tcp_close(pcb);
    }
    return -1;
}

void audio_server_push_pcm(const void *pcm_interleaved_samples) {
    if (active_client_pcb == NULL || pcm_interleaved_samples == NULL) {
        return;
    }

    /* Pack 512 interleaved PCM samples into a valid OggFLAC page */
    size_t page_len = ogg_flac_encode_page(&stream_cfg, pcm_interleaved_samples,
                                           current_frame_num, ogg_page_buf);
//PRINTF("page_len=%u ", (unsigned) page_len);
    if (page_len == 0) {
        return;
    }

    /* Send only if TCP send buffer has enough space to avoid RAM exhaustion */
    if (tcp_sndbuf(active_client_pcb) >= page_len) {
        err_t err = tcp_write(active_client_pcb, ogg_page_buf, (u16_t)page_len, TCP_WRITE_FLAG_COPY);
        if (err == ERR_OK) {
            tcp_output(active_client_pcb);
            current_frame_num++;
        }
    }
}
