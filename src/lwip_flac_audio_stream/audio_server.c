#include "hardware.h"
#include "formats.h"

#include "audio_server.h"
#include "flac_stream.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

/* Pointer to the currently active audio stream client */
static struct tcp_pcb *active_client_pcb = NULL;
static uint32_t current_frame_num = 0;

/* Scratch buffer for assembling a single FLAC frame (512 * 2 + ~15 bytes overhead) */
static uint8_t flac_frame_buf[FLAC_BLOCK_SIZE * 2 + 20];

/* Close TCP connection and clean up callbacks */
static void close_client_pcb(struct tcp_pcb *tpcb) {
    if (tpcb == NULL) {
        return;
    }
    tcp_arg(tpcb, NULL);
    tcp_recv(tpcb, NULL);
    tcp_sent(tpcb, NULL);
    tcp_err(tpcb, NULL);
    tcp_close(tpcb);

    if (active_client_pcb == tpcb) {
        active_client_pcb = NULL;
    }
}

/* Error callback: triggered when connection is aborted or reset by peer */
static void audio_err_callback(void *arg, err_t err) {
    struct tcp_pcb *tpcb = (struct tcp_pcb *)arg;
    /* Only clear active_client_pcb if the error belongs to the active stream socket */
    if (active_client_pcb == tpcb) {
        active_client_pcb = NULL;
    }
}

/* Sent callback for static HTML page: close socket only after browser ACKs the data */
static err_t html_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    close_client_pcb(tpcb);
    return ERR_OK;
}

/* Receive callback: handles incoming HTTP requests */
static err_t audio_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        /* Remote host closed the connection */
        close_client_pcb(tpcb);
        return ERR_OK;
    }

    if (strncmp((char *)p->payload, "GET /stream", 11) == 0) {
        /* If another client was previously streaming, disconnect it */
        if (active_client_pcb != NULL && active_client_pcb != tpcb) {
            close_client_pcb(active_client_pcb);
        }

        active_client_pcb = tpcb;
        current_frame_num = 0;

        /* Acknowledge received HTTP request bytes */
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);

        /* Send HTTP response headers without Content-Length for infinite streaming */
        const char *http_header =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: audio/flac\r\n"
            "Cache-Control: no-cache, no-store\r\n"
            "Connection: close\r\n\r\n";

        tcp_write(tpcb, http_header, strlen(http_header), TCP_WRITE_FLAG_COPY);

        /* Immediately send the 42-byte global FLAC stream header */
        tcp_write(tpcb, FLAC_STREAM_HEADER, sizeof(FLAC_STREAM_HEADER), TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
    } else {
        /* Serve default HTML5 player page with exact Content-Length */
        const char *html_body =
            "<!DOCTYPE html><html><head><meta charset='utf-8'>"
            "<title>lwIP Live Audio Stream</title></head><body>"
            "<h3>Live Uncompressed FLAC Stream</h3>"
            "<audio controls autoplay src='/stream'></audio>"
            "</body></html>";

        char response[512];
        int resp_len = snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n\r\n"
            "%s",
            (int)strlen(html_body), html_body);

        /* Acknowledge received bytes before scheduling socket closure */
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);

        /* Close socket once TCP ACK is received from the browser */
        tcp_sent(tpcb, html_sent_callback);

        tcp_write(tpcb, response, resp_len, TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
    }

    return ERR_OK;
}

/* Accept callback: configure new incoming TCP connection */
static err_t audio_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    /* Disable Nagle's algorithm to minimize audio latency */
    tcp_nagle_disable(newpcb);

    /* Pass PCB pointer as callback argument so error handler can identify the socket */
    tcp_arg(newpcb, newpcb);
    tcp_recv(newpcb, audio_recv_callback);
    tcp_err(newpcb, audio_err_callback);
    return ERR_OK;
}

void audio_server_init(void) {
    struct tcp_pcb *pcb = tcp_new();
    if (pcb != NULL) {
        if (tcp_bind(pcb, IP_ADDR_ANY, 80) == ERR_OK) {
            pcb = tcp_listen(pcb);
            tcp_accept(pcb, audio_accept_callback);
        }
    }
}

void audio_server_push_pcm(const int16_t *pcm_512_samples) {
    if (active_client_pcb == NULL) {
        return;
    }

    /* Encode 512 PCM samples into an uncompressed FLAC frame (~1035 bytes) */
    size_t frame_len = flac_encode_verbatim_frame(pcm_512_samples, current_frame_num, flac_frame_buf);
//    printhex_titled(0, flac_frame_buf, frame_len, "flac_frame_buf");
//    ASSERT(0);
    /* Verify available TCP send buffer space to prevent RAM exhaustion on slow links */
    if (tcp_sndbuf(active_client_pcb) >= frame_len) {
        err_t err = tcp_write(active_client_pcb, flac_frame_buf, frame_len, TCP_WRITE_FLAG_COPY);
        if (err == ERR_OK) {
            tcp_output(active_client_pcb);
            current_frame_num++;
        }
    }
}
