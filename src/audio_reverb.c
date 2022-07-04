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
    unsigned    index;
    unsigned    delay;
    FLOAT_t     gain;
} reverbch_t;

static FLOAT_t    wet0 = 0;
static FLOAT_t    wet1 = 1;
static FLOAT_t    result = 0;

static reverbch_t     cf0 = { .buf = cfbuf0, .gain = 0.805 };
static reverbch_t     cf1 = { .buf = cfbuf1, .gain = 0.827 };
static reverbch_t     cf2 = { .buf = cfbuf2, .gain = 0.783 };
static reverbch_t     cf3 = { .buf = cfbuf3, .gain = 0.764 };
static reverbch_t     ap0 = { .buf = apbuf0, .gain = 0.7 };
static reverbch_t     ap1 = { .buf = apbuf1, .gain = 0.7 };
static reverbch_t     ap2 = { .buf = apbuf2, .gain = 0.7 };

static FLOAT_t calc_comb(FLOAT_t in, reverbch_t *comb) {
    FLOAT_t   reedback = comb->buf[comb->index];

    if (reedback > 1) {
        reedback = 1;
    } else if (reedback < -1) {
        reedback = -1;
    }

    FLOAT_t newv = reedback * comb->gain + in;

    if (newv > 1) {
        newv = 1;
    } else if (newv < -1) {
        newv = -1;
    }

    comb->buf[comb->index] = newv;
    comb->index++;

    if (comb->index >= comb->delay)
        comb->index = 0;

    return reedback;
}

static FLOAT_t calc_all_pass(FLOAT_t in, reverbch_t *allpass) {
    FLOAT_t   reedback = allpass->buf[allpass->index] - allpass->gain*in;

    if (reedback > 1) {
        reedback = 1;
    } else if (reedback < -1) {
        reedback = -1;
    }

    FLOAT_t   new = reedback*allpass->gain + in;

    if (new > 1) {
        new = 1;
    } else if (new < -1) {
        new = -1;
    }

    allpass->buf[allpass->index] = new;
    allpass->index++;

    if (allpass->index > allpass->delay)
        allpass->index = 0;

    return reedback;
}

void audio_reverb_set_delay(unsigned delay) {

    cf0.delay = CF0 * delay / 100;   if (cf0.index >= cf0.delay) cf0.index = 0;
    cf1.delay = CF1 * delay / 100;   if (cf1.index >= cf1.delay) cf1.index = 0;
    cf2.delay = CF2 * delay / 100;   if (cf2.index >= cf2.delay) cf2.index = 0;
    cf3.delay = CF3 * delay / 100;   if (cf3.index >= cf3.delay) cf3.index = 0;

    ap0.delay = AP0 * delay / 100;   if (ap0.index >= ap0.delay) ap0.index = 0;
    ap1.delay = AP1 * delay / 100;   if (ap1.index >= ap1.delay) ap1.index = 0;
    ap2.delay = AP2 * delay / 100;   if (ap2.index >= ap2.delay) ap2.index = 0;
}

void audio_reverb_set_loss(FLOAT_t ratio) {
    wet0 = ratio;
    wet1 = 1 - ratio;
}

FLOAT_t audio_reverb_calc(FLOAT_t in) {
    result = (calc_comb(in, &cf0) + calc_comb(in, &cf1) + calc_comb(in, &cf2) + calc_comb(in, &cf3)) / 4;

    result = calc_all_pass(result, &ap0);
    result = calc_all_pass(result, &ap1);
    result = calc_all_pass(result, &ap2);

    if (result > 1) {
        result = 1;
    } else if (result < -1) {
        result = -1;
    }

    return wet0 * result + wet1 * in;
}
