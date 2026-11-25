#include "hardware.h"
#if WITHLWIP
#include "ws_audio_server.h"
#include "lwip/tcp.h"
#include <string.h>
#include <stdio.h>

#define MAX_WS_CLIENTS 4

/* Transmit buffer: 4-byte WebSocket header + 480 samples * 2 ch * 2 bytes = 1924 bytes */
static uint8_t ws_tx_buf[4 + WS_AUDIO_BLOCK_SAMPLES * 2 * 2];

static struct tcp_pcb *ws_clients[MAX_WS_CLIENTS];
static uint32_t ws_sample_rate = 8000;
static uint8_t ws_channels = 1;

/* =========================================================================
 * Minimal Standalone SHA-1 & Base64 for RFC 6455 WebSocket Handshake
 * ========================================================================= */

#define ROL32(val, bits) (((val) << (bits)) | ((val) >> (32 - (bits))))

static void sha1_compute(const uint8_t *data, size_t len, uint8_t hash_out[20]) {
    uint32_t state[5] = { 0x67452301UL, 0xEFCDAB89UL, 0x98BADCFEUL, 0x10325476UL, 0xC3D2E1F0UL };
    uint8_t block[64];
    uint32_t w[80];
    uint64_t total_bits = (uint64_t)len * 8ULL;

    size_t offset = 0;
    int padded = 0;
    int done = 0;

    while (!done) {
        size_t chunk = 0;
        if (offset < len) {
            chunk = len - offset;
            if (chunk > 64) chunk = 64;
            memcpy(block, &data[offset], chunk);
            offset += chunk;
        }

        if (chunk < 64) {
            if (!padded) {
                block[chunk++] = 0x80;
                padded = 1;
            }
            if (chunk <= 56) {
                memset(&block[chunk], 0, 56 - chunk);
                for (int i = 0; i < 8; i++) {
                    block[56 + i] = (uint8_t)(total_bits >> ((7 - i) * 8));
                }
                done = 1;
            } else {
                memset(&block[chunk], 0, 64 - chunk);
            }
        }

        for (int i = 0; i < 16; i++) {
            w[i] = ((uint32_t)block[i * 4 + 0] << 24) |
                     ((uint32_t)block[i * 4 + 1] << 16) |
                     ((uint32_t)block[i * 4 + 2] << 8)  |
                     ((uint32_t)block[i * 4 + 3]);
        }
        for (int i = 16; i < 80; i++) {
            w[i] = ROL32(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        }

        uint32_t a = state[0], b = state[1], c = state[2], d = state[3], e = state[4];
        for (int i = 0; i < 80; i++) {
            uint32_t f, k;
            if (i < 20) {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999UL;
            } else if (i < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1UL;
            } else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDCUL;
            } else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6UL;
            }
            uint32_t temp = ROL32(a, 5) + f + e + k + w[i];
            e = d; d = c; c = ROL32(b, 30); b = a; a = temp;
        }

        state[0] += a; state[1] += b; state[2] += c; state[3] += d; state[4] += e;
    }

    for (int i = 0; i < 5; i++) {
        hash_out[i * 4 + 0] = (uint8_t)(state[i] >> 24);
        hash_out[i * 4 + 1] = (uint8_t)(state[i] >> 16);
        hash_out[i * 4 + 2] = (uint8_t)(state[i] >> 8);
        hash_out[i * 4 + 3] = (uint8_t)(state[i]);
    }
}

static size_t base64_encode_20(const uint8_t in[20], char out[32]) {
    static const char tbl[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t j = 0;
    for (size_t i = 0; i < 20; i += 3) {
        uint32_t v = (uint32_t)in[i] << 16;
        if (i + 1 < 20) v |= (uint32_t)in[i + 1] << 8;
        if (i + 2 < 20) v |= (uint32_t)in[i + 2];

        out[j++] = tbl[(v >> 18) & 0x3F];
        out[j++] = tbl[(v >> 12) & 0x3F];
        out[j++] = (i + 1 < 20) ? tbl[(v >> 6) & 0x3F] : '=';
        out[j++] = (i + 2 < 20) ? tbl[v & 0x3F] : '=';
    }
    out[j] = '\0';
    return j;
}

/* =========================================================================
 * TCP Connection & WebSocket Protocol Handlers
 * ========================================================================= */

static int find_ws_slot(struct tcp_pcb *tpcb) {
    if (tpcb == NULL) return -1;
    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
        if (ws_clients[i] == tpcb) return i;
    }
    return -1;
}

static void close_ws_connection(struct tcp_pcb *tpcb) {
    if (tpcb == NULL) return;
    int slot = find_ws_slot(tpcb);
    if (slot >= 0) {
        ws_clients[slot] = NULL;
    }
    tcp_arg(tpcb, NULL);
    tcp_recv(tpcb, NULL);
    tcp_sent(tpcb, NULL);
    tcp_err(tpcb, NULL);
    tcp_close(tpcb);
}

static void ws_err_callback(void *arg, err_t err) {
    (void)err;
    struct tcp_pcb *tpcb = (struct tcp_pcb *)arg;
    int slot = find_ws_slot(tpcb);
    if (slot >= 0) {
        ws_clients[slot] = NULL;
    }
}

static err_t html_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    (void)arg;
    (void)len;
    if (tpcb->snd_queuelen == 0) {
        close_ws_connection(tpcb);
    }
    return ERR_OK;
}
/* Parses Sec-WebSocket-Key from HTTP Upgrade request and sends 101 Switching Protocols */
static int perform_ws_handshake(struct tcp_pcb *tpcb, const char *req, u16_t req_len) {
    static const char key_hdr[] = "Sec-WebSocket-Key:";
    static const char ws_guid[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

    int free_slot = -1;
    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
        if (ws_clients[i] == NULL) {
            free_slot = i;
            break;
        }
    }
    if (free_slot < 0) return -1;

    const char *p = strstr(req, key_hdr);
    if (p == NULL) return -1;
    p += sizeof key_hdr - 1;
    while (*p == ' ' || *p == '\t') p++;

    char concat_buf[96];
    size_t klen = 0;
    while (p[klen] != '\r' && p[klen] != '\n' && p[klen] != '\0' &&
           (size_t)(&p[klen] - req) < req_len && klen < 48) {
        concat_buf[klen] = p[klen];
        klen++;
    }
    if (klen == 0) return -1;

    memcpy(&concat_buf[klen], ws_guid, sizeof ws_guid - 1);
    size_t total_concat = klen + (sizeof ws_guid - 1);

    uint8_t sha1_digest[20];
    char accept_key[32];
    sha1_compute((const uint8_t *)concat_buf, total_concat, sha1_digest);
    base64_encode_20(sha1_digest, accept_key);

    char resp[256];
    int resp_len = snprintf(resp, sizeof resp,
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n\r\n",
        accept_key);

    ws_clients[free_slot] = tpcb;
    tcp_write(tpcb, resp, resp_len, TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    return 0;
}

static err_t ws_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    (void)arg;
    (void)err;

    if (p == NULL) {
        close_ws_connection(tpcb);
        return ERR_OK;
    }

    int slot = find_ws_slot(tpcb);
    if (slot >= 0) {
        /* Handle incoming WebSocket control frames (Close / Ping) */
        const uint8_t *frame = (const uint8_t *)p->payload;
        if (p->len >= 2) {
            uint8_t opcode = frame[0] & 0x0F;
            if (opcode == 0x08) {
                tcp_recved(tpcb, p->tot_len);
                pbuf_free(p);
                close_ws_connection(tpcb);
                return ERR_OK;
            } else if (opcode == 0x09) {
                static const uint8_t pong_frame[2] = { 0x8A, 0x00 };
                tcp_write(tpcb, pong_frame, sizeof pong_frame, 0);
                tcp_output(tpcb);
            }
        }
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);
        return ERR_OK;
    }

    char req_copy[512];
    u16_t copy_len = (p->tot_len < sizeof req_copy - 1) ? p->tot_len : (sizeof req_copy - 1);
    pbuf_copy_partial(p, req_copy, copy_len, 0);
    req_copy[copy_len] = '\0';

    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);

    if (strncmp(req_copy, "GET /ws", 7) == 0) {
        if (perform_ws_handshake(tpcb, req_copy, copy_len) != 0) {
            close_ws_connection(tpcb);
        }
        return ERR_OK;
    } else {
        unsigned int active_count = 0;
        for (int i = 0; i < MAX_WS_CLIENTS; i++) {
            if (ws_clients[i] != NULL) active_count++;
        }

        static const int WS_JITTER_BUFFER_MS = 300;

        static const char html_template[] =
            "<!DOCTYPE html><html><head><meta charset='utf-8'>"
            "<meta name='viewport' content='width=device-width,initial-scale=1'>"
            "<title>WebSocket PCM Audio</title></head><body>"
            "<h3>Live WebSocket 16-bit PCM (%lu Hz, %u ch)</h3>"
            "<p>Listeners: %u / %u | <span id='st'>Stopped</span></p>"
            "<button id='btn'>&#9654; Start Audio</button> "
            "Vol: <input id='vol' type='range' min='0' max='2' step='0.05' value='1'><br><br>"
            "<script>"
            "const SR=%lu,CH=%u,BUF_MS=%d;"
            "let ws=null,ctx=null,gain=1.0;"
            "const st=document.getElementById('st');"
            "document.getElementById('vol').oninput=(e)=>gain=parseFloat(e.target.value);"
            "document.getElementById('btn').onclick=()=>{"
            "if(ws){ws.close();ws=null;st.textContent='Stopped';return;}"
            "ctx=new(window.AudioContext||window.webkitAudioContext)({sampleRate:SR});"
            "const targetSec=BUF_MS/1000.0;"
            "const cap=Math.max(8192,Math.ceil(SR*targetSec*3.0))*CH;"
            "const targetCnt=Math.ceil(SR*targetSec)*CH;"
            "const maxBuf=Math.ceil(SR*targetSec*1.5)*CH;"
            "const rb=new Float32Array(cap);"
            "let wr=0,rd=0,cnt=0,playing=false,last=new Float32Array(CH);"
            "const sp=ctx.createScriptProcessor(1024,0,CH);"
            "sp.onaudioprocess=(ev)=>{"
            "const n=ev.outputBuffer.length;"
            "if(!playing){if(cnt>=targetCnt)playing=true;else{"
            "for(let c=0;c<CH;c++)ev.outputBuffer.getChannelData(c).fill(0);return;}}"
            "for(let i=0;i<n;i++){"
            "if(cnt>=CH){"
            "for(let c=0;c<CH;c++){"
            "const s=rb[rd]*gain;rd=(rd+1)%%cap;cnt--;"
            "last[c]=s;ev.outputBuffer.getChannelData(c)[i]=s;}"
            "}else{"
            "playing=false;"
            "for(let c=0;c<CH;c++){"
            "last[c]*=0.92;ev.outputBuffer.getChannelData(c)[i]=last[c];}"
            "}}};"
            "sp.connect(ctx.destination);"
            "ws=new WebSocket('ws://'+location.host+'/ws');"
            "ws.binaryType='arraybuffer';"
            "ws.onopen=()=>st.textContent='Buffering '+BUF_MS+' ms...';"
            "ws.onclose=()=>{sp.disconnect();ctx.close();st.textContent='Disconnected';ws=null;};"
            "ws.onmessage=(e)=>{"
            "const i16=new Int16Array(e.data);"
            "if(cnt+i16.length>maxBuf){"
            "const drop=Math.floor((cnt+i16.length-targetCnt)/CH)*CH;"
            "if(drop>0&&drop<cnt){rd=(rd+drop)%%cap;cnt-=drop;}}"
            "for(let i=0;i<i16.length;i++){"
            "if(cnt<cap){rb[wr]=i16[i]/32768.0;wr=(wr+1)%%cap;cnt++;}}"
            "st.textContent=(playing?'Live':'Buffering')+' | Buffer: '+Math.round((cnt/CH)/SR*1000)+' ms';"
            "};};"
            "</script></body></html>";

        static char html_body[2600];
        char http_hdr[160];

        int body_len = snprintf(html_body, sizeof html_body, html_template,
            (unsigned long)ws_sample_rate, (unsigned int)ws_channels,
            active_count, (unsigned int)MAX_WS_CLIENTS,
            (unsigned long)ws_sample_rate, (unsigned int)ws_channels,
            WS_JITTER_BUFFER_MS);

        int hdr_len = snprintf(http_hdr, sizeof http_hdr,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html; charset=utf-8\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n\r\n",
            body_len);

        tcp_sent(tpcb, html_sent_callback);
        tcp_write(tpcb, http_hdr, hdr_len, TCP_WRITE_FLAG_COPY);
        tcp_write(tpcb, html_body, body_len, TCP_WRITE_FLAG_COPY);
        tcp_output(tpcb);
        return ERR_OK;
    }
}

static err_t ws_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    (void)arg;
    (void)err;
    tcp_nagle_disable(newpcb);
    tcp_arg(newpcb, newpcb);
    tcp_recv(newpcb, ws_recv_callback);
    tcp_err(newpcb, ws_err_callback);
    return ERR_OK;
}

void ws_audio_server_init(uint32_t sample_rate_hz, uint8_t channels) {
    memset(ws_clients, 0, sizeof ws_clients);
    ws_sample_rate = sample_rate_hz;
    ws_channels = (channels == 2) ? 2 : 1;

    struct tcp_pcb *pcb = tcp_new();
    if (pcb != NULL) {
        if (tcp_bind(pcb, IP_ADDR_ANY, 80) == ERR_OK) {
            pcb = tcp_listen(pcb);
            tcp_accept(pcb, ws_accept_callback);
        }
    }
}

void ws_audio_server_push_pcm(const int16_t *pcm_480_samples_per_ch) {
    if (pcm_480_samples_per_ch == NULL) return;

    int has_clients = 0;
    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
        if (ws_clients[i] != NULL) {
            has_clients = 1;
            break;
        }
    }
    if (!has_clients) return;

    size_t payload_bytes = WS_AUDIO_BLOCK_SAMPLES * ws_channels * sizeof(int16_t);
    size_t hdr_len = 0;

    /* Build unmasked RFC 6455 Binary Frame header (0x82 = FIN + Binary opcode) */
    ws_tx_buf[0] = 0x82;
    if (payload_bytes < 126) {
        ws_tx_buf[1] = (uint8_t)payload_bytes;
        hdr_len = 2;
    } else {
        ws_tx_buf[1] = 126;
        ws_tx_buf[2] = (uint8_t)(payload_bytes >> 8);
        ws_tx_buf[3] = (uint8_t)(payload_bytes & 0xFF);
        hdr_len = 4;
    }

    memcpy(&ws_tx_buf[hdr_len], pcm_480_samples_per_ch, payload_bytes);
    size_t total_frame_len = hdr_len + payload_bytes;

    for (int i = 0; i < MAX_WS_CLIENTS; i++) {
        struct tcp_pcb *pcb = ws_clients[i];
        if (pcb == NULL) continue;

        if (tcp_sndbuf(pcb) >= total_frame_len) {
            if (tcp_write(pcb, ws_tx_buf, total_frame_len, TCP_WRITE_FLAG_COPY) == ERR_OK) {
                tcp_output(pcb);
            }
        }
    }
}
#endif /* WITHLWIP */

