/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "board.h"
#include "audio_reverb.h"

#define CF0 3460
#define CF1 2988
#define CF2 3882
#define CF3 4312

#define AP0 480
#define AP1 161
#define AP2 46

static FLOAT_t    cfbuf0[CF0];
static FLOAT_t    cfbuf1[CF1];
static FLOAT_t    cfbuf2[CF2];
static FLOAT_t    cfbuf3[CF3];

static FLOAT_t    apbuf0[AP0];
static FLOAT_t    apbuf1[AP1];
static FLOAT_t    apbuf2[AP2];

typedef struct {
    FLOAT_t     *buf;
    uint16_t    index;
    uint16_t    delay;
    FLOAT_t     gain;
} item_t;

static FLOAT_t    wet0 = 0.0f;
static FLOAT_t    wet1 = 1.0f;
static FLOAT_t    result = 0.0f;

static item_t     cf0 = { .buf = cfbuf0, .gain = 0.805f };
static item_t     cf1 = { .buf = cfbuf1, .gain = 0.827f };
static item_t     cf2 = { .buf = cfbuf2, .gain = 0.783f }; 
static item_t     cf3 = { .buf = cfbuf3, .gain = 0.764f };
static item_t     ap0 = { .buf = apbuf0, .gain = 0.7f };
static item_t     ap1 = { .buf = apbuf1, .gain = 0.7f };
static item_t     ap2 = { .buf = apbuf2, .gain = 0.7f };

static FLOAT_t calc_comb(FLOAT_t in, item_t *comb) {
    FLOAT_t   readback = comb->buf[comb->index];
    FLOAT_t   new = readback * comb->gain + in;

    comb->buf[comb->index] = new;
    comb->index++;

    if (comb->index >= comb->delay)
        comb->index = 0;

    return readback;
}

static FLOAT_t calc_all_pass(FLOAT_t in, item_t *allpass) {
    FLOAT_t   reedback = allpass->buf[allpass->index] - allpass->gain*in;
    FLOAT_t   new = reedback*allpass->gain + in;

    allpass->buf[allpass->index] = new;
    allpass->index++;

    if (allpass->index > allpass->delay)
        allpass->index = 0;

    return reedback;
}

void audio_reverb_set_delay(FLOAT_t delay) {
    FLOAT_t x = delay / 100.0f;

    cf0.delay = (uint16_t) (x * CF0);   if (cf0.index >= cf0.delay) cf0.index = 0;
    cf1.delay = (uint16_t) (x * CF1);   if (cf1.index >= cf1.delay) cf1.index = 0;
    cf2.delay = (uint16_t) (x * CF2);   if (cf2.index >= cf2.delay) cf2.index = 0;
    cf3.delay = (uint16_t) (x * CF3);   if (cf3.index >= cf3.delay) cf3.index = 0;

    ap0.delay = (uint16_t) (x * AP0);   if (ap0.index >= ap0.delay) ap0.index = 0;
    ap1.delay = (uint16_t) (x * AP1);   if (ap1.index >= ap1.delay) ap1.index = 0;
    ap2.delay = (uint16_t) (x * AP2);   if (ap2.index >= ap2.delay) ap2.index = 0;
}

void audio_reverb_set_loss(FLOAT_t loss) {
    FLOAT_t x = loss / 100.f;

    wet0 = 1.0f - x;
    wet1 = x;
}

FLOAT_t audio_reverb_calc(FLOAT_t in) {
    result = (calc_comb(in, &cf0) + calc_comb(in, &cf1) + calc_comb(in, &cf2) + calc_comb(in, &cf3)) / 4.0f;

    result = calc_all_pass(result, &ap0);
    result = calc_all_pass(result, &ap1);
    result = calc_all_pass(result, &ap2);

    return wet0 * result + wet1 * in;
}
