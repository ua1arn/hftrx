#include "hardware.h"
#include "audio_server.h"
#include "flac_stream.h"
#include "lwip/tcp.h"
#include <string.h>

// Указатель на активного слушателя (для простоты — один клиент)
static struct tcp_pcb *active_client_pcb = NULL;
static uint32_t current_frame_num = 0;

// Буфер для сборки одного FLAC-фрейма (512 * 2 + ~15 байт заголовков)
static uint8_t flac_frame_buf[FLAC_BLOCK_SIZE * 2 + 20];

// Обработка ошибок или разрыва соединения
static void audio_err_callback(void *arg, err_t err) {
    (void)arg;
    (void)err;
    active_client_pcb = NULL;
}

// Закрытие соединения
static void close_audio_client(struct tcp_pcb *tpcb) {
    tcp_arg(tpcb, NULL);
    tcp_recv(tpcb, NULL);
    tcp_err(tpcb, NULL);
    tcp_close(tpcb);
    if (active_client_pcb == tpcb) {
        active_client_pcb = NULL;
    }
}

// Обработка входящего HTTP-запроса от браузера
static err_t audio_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    (void)arg;
    (void)err;
    if (p == NULL) {
        TP();
        // Клиент закрыл вкладку или нажал стоп
        close_audio_client(tpcb);
        return ERR_OK;
    }

    // Проверяем, что пришел запрос GET /stream
    if (strncmp((char *)p->payload, "GET /stream", 11) == 0) {
        TP();
        // Если уже был подключен другой клиент — отключаем его
        if (active_client_pcb != NULL && active_client_pcb != tpcb) {
            close_audio_client(active_client_pcb);
        }

        active_client_pcb = tpcb;
        current_frame_num = 0; // Сбрасываем счетчик фреймов для нового клиента

        // Отправляем HTTP-заголовок без Content-Length
        const char *http_header = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: audio/flac\r\n"
            "Cache-Control: no-cache, no-store\r\n"
            "Connection: close\r\n\r\n";

        tcp_write(tpcb, http_header, strlen(http_header), TCP_WRITE_FLAG_COPY);
        
        // Сразу следом отправляем 42-байтный заголовок потока FLAC
        tcp_write(tpcb, FLAC_STREAM_HEADER, sizeof(FLAC_STREAM_HEADER), TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
    } else {
        TP();
       // На любой другой запрос (например, GET /) отдаем простую HTML-страницу с плеером
        const char *html_page = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n\r\n"
            "<!DOCTYPE html><html><body>"
            "<h3>ESP32 / STM32 Live Uncompressed FLAC Stream</h3>"
            "<audio controls autoplay src='/stream'></audio>"
            "</body></html>";
            
        tcp_write(tpcb, html_page, strlen(html_page), TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
        close_audio_client(tpcb);
    }

    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
    return ERR_OK;
}

// Принятие нового TCP-подключения
static err_t audio_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    (void)arg;
    (void)err;

    // Отключаем алгоритм Нейгла (TCP_NODELAY) для минимальной задержки звука
    tcp_nagle_disable(newpcb);
    
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
        return; // Нет подключенных слушателей
    }

    // Упаковываем 512 семплов в FLAC-фрейм (~1034 байта)
    size_t frame_len = flac_encode_verbatim_frame(pcm_512_samples, current_frame_num, flac_frame_buf);

    // Проверяем, хватает ли места в буфере отправки TCP
    if (tcp_sndbuf(active_client_pcb) >= frame_len) {
        err_t err = tcp_write(active_client_pcb, flac_frame_buf, frame_len, TCP_WRITE_FLAG_COPY);
        if (err == ERR_OK) {
            tcp_output(active_client_pcb);
            current_frame_num++; // Увеличиваем номер фрейма только при успешной постановке в очередь
        }
    }
}
