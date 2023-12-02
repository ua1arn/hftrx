/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#include "hardware.h"
#include "formats.h"
#include "audio.h"
#include "buffers.h"

#if WITHINTEGRATEDDSP

// Utilites code taken from UA3REO project

typedef struct {
    double re;
    double im;
} complex_d;

static void complex_mul(complex_d *a, complex_d *b);
static void complex_div(complex_d *p, complex_d *q);

void biquad_create(iir_filter_t *filter, unsigned sect_num)
{
	memset(filter, 0, sizeof * filter);

	filter->sections = sect_num;
	filter->sect_ord = IIR_BIQUAD_SECTION_ORDER;

	memset(filter->a, 0, sizeof filter->a);
	memset(filter->b, 0, sizeof filter->b);
	memset(filter->d, 0, sizeof filter->d);
}

void fill_biquad_coeffs(iir_filter_t *filter, FLOAT_t *coeffs, unsigned sect_num)
{
	//transpose and save coefficients
	unsigned ind = 0;
	for(uint8_t sect = 0; sect < sect_num; sect++)
	{
		coeffs[ind + 0] = filter->b[sect * 3 + 0];
		coeffs[ind + 1] = filter->b[sect * 3 + 1];
		coeffs[ind + 2] = filter->b[sect * 3 + 2];
		coeffs[ind + 3] = -filter->a[sect * 3 + 1];
		coeffs[ind + 4] = -filter->a[sect * 3 + 2];
		ind += 5;
	}
}

void biquad_zero(struct iir_filter *filter)
{
    double *a = filter->a;
    double *b = filter->b;
    int n;
    int i;

    n = filter->sections;
    for (i = 0; i < n; i += 1) {
        b[0] = 0;
        b[1] = 0;
        b[2] = 0;
        a[0] = 0;
        a[1] = 0;
        a[2] = 0;
    }
}

void biquad_init_lowpass(struct iir_filter *filter, FLOAT_t fs, FLOAT_t fc)
{
	double *a = filter->a;
	double *b = filter->b;
	double w = 2 * M_PI * fc / fs;
	double phi, alpha;
	int i, k;
	int n;

	n = filter->sections;
	for (i = 0; i < n; i += 1) {
		k = n - i - 1.0;
		phi = M_PI / (4.0 * n) * (k * 2.0 + 1.0);
		alpha = sin(w) * cos(phi);

		b[0] = (1.0 - cos(w)) / (2.0 * (1.0 + alpha));
		b[1] = (1.0 - cos(w)) / (1.0 + alpha);
		b[2] = (1.0 - cos(w)) / (2.0 * (1.0 + alpha));
		a[0] = 1.0;
		a[1] = -2.0 * cos(w) / (1.0 + alpha);
		a[2] = (1.0 - alpha) / (1.0 + alpha);
		a += 3;
		b += 3;
	}

	for (i = 0; i < (n + 1) * 2; i += 1) {
		filter->d[i] = 0;
	}
}

void biquad_init_highpass(struct iir_filter *filter, FLOAT_t fs, FLOAT_t f) {
    double *a = filter->a;
    double *b = filter->b;
    double w = 2.0 * M_PI * f / fs;
    double phi, alpha;
    int n, i, k;

    n = filter->sections;

    for (i = 0; i < n; i += 1) {
        k = n - i - 1;
        phi = M_PI / (4.0 * n) * (k * 2.0 + 1.0);
        alpha = sin(w) * cos(phi);

        b[0] = (1.0 + cos(w)) / (2.0 * (1.0 + alpha));
        b[1] = -(1.0 + cos(w)) / (1.0 + alpha);
        b[2] = (1.0 + cos(w)) / (2.0 * (1.0 + alpha));
        a[1] = -2.0 * cos(w) / (1.0 + alpha);
        a[2] = (1.0 - alpha) / (1.0 + alpha);
        a += 3;
        b += 3;
    }

    for (i = 0; i < (n + 1) * 2; i += 1)
        filter->d[i] = 0;
}

static void complex_square(complex_d *s)
{
    double x, y;

    x = s->re;
    y = s->im;

    s->re = x * x - y * y;
    s->im = 2.0 * x * y;
}

static void complex_sqrt(complex_d *s)
{
    double x, y;
    double r, phi;

    x = s->re;
    y = s->im;
    if (x == 0 && y == 0) {
        return;
    }

    /* Converting to polar */
    phi = atan2(y, x);
    r = sqrt(x * x + y * y);

    /* Square root */
    phi /= 2.0;
    r = sqrt(r);

    /* Back to cartesian */
    s->re = r * cos(phi);
    s->im = r * sin(phi);
}

static void complex_mul(complex_d *a, complex_d *b)
{
    double x, y;

    x = a->re * b->re - a->im * b->im;
    y = a->re * b->im + a->im * b->re;

    a->re = x;
    a->im = y;
}

static void complex_div(complex_d *p, complex_d *q)
{
    double x, y;
    double r, phi;
    complex_d b;

    x = q->re;
    y = q->im;
    if (x == 0 && y == 0) {
        return;
    }

    /* Converting to polar */
    phi = atan2(y, x);
    r = sqrt(x * x + y * y);

    /* Reciprocal */
    phi = -phi;
    r = 1.0 / r;

    /* Back to cartesian */
    b.re = r * cos(phi);
    b.im = r * sin(phi);

    complex_mul(p, &b);
}

/*
 *  Convert from continuous (s) to discrete (z)
 *  using bilinear transform
 *  ts: sample period (T)
 */

static void bilinear_transform(complex_d *z, complex_d *s, double ts)
{
    complex_d p, q;
    double x = s->re;
    double y = s->im;

    x *= ts / 2.0;
    y *= ts / 2.0;
    p.re = 1.0 + x;
    p.im = y;
    q.re = 1.0 - x;
    q.im = -y;
    complex_div(&p, &q);
    *z = p;
}

/*
 *  Compute bandpass or bandstop filter parameters
 */

static void biquad_init_band(struct iir_filter *filter, FLOAT_t fs, FLOAT_t f1, FLOAT_t f2, int stop)
{
    double ts = 1.0 / fs;
    double bw, f;
    double w;
    complex_d p, q;
    complex_d z, s;
    double phi;
    complex_d _z, p_lp, p_bp;
    double k, x, y;
    double wa1, wa2, wa;
    double *a = filter->a;
    double *b = filter->b;
    int n, i;

    f = sqrt(f1 * f2);
    w = 2.0 * M_PI * f / fs;

    /* Map to continuous-time frequencies (pre-warp) */

    wa1 = 2.0 * fs * tan(M_PI * f1 * ts);
    wa2 = 2.0 * fs * tan(M_PI * f2 * ts);

    bw = wa2 - wa1;
    wa = sqrt(wa1 * wa2);

    n = filter->sections;

    for (i = 0; i < n; i += 1) {
        phi = M_PI / 2.0 + M_PI * (2.0 * i + 1.0) / (n * 2.0);
        x = cos(phi);
        y = sin(phi);

        p_lp.re = x * bw / (wa * 2.0);
        p_lp.im = y * bw / (wa * 2.0);

        /*
         *  Map every low-pass pole to a complex conjugate
         *  pair of band-bass poles
         */

        s = p_lp;
        complex_square(&s);
        s.re = 1.0 - s.re;
        s.im = 0.0 - s.im;
        complex_sqrt(&s);
        x = p_lp.re - s.im;
        y = p_lp.im + s.re;
        p_bp.re = x * wa;
        p_bp.im = y * wa;

        /*
         *  Convert every pair from continuous (s)
         *  to discrete (z) using bilinear transform
         */

        bilinear_transform(&z, &p_bp, ts);

        x = z.re;
        y = z.im;

        /*
         *  Find denominator coefficients from
         *  the complex conjugate pair of poles
         */

        a[0] = 1.0;
        a[1] = -2 * x;
        a[2] = x * x + y * y;

        if (stop) {
            /* Band-stop: zeros at Ï‰ and ~Ï‰ */
            s.re = 0;
            s.im = wa;
            bilinear_transform(&z, &s, ts);
            x = z.re;
            y = z.im;

            b[0] = 1.0;
            b[1] = -2.0 * x;
            b[2] = x * x + y * y;
        } else {
            /* Band-pass: zeros at Â±1 */
            b[0] = 1.0;
            b[1] = 0.0;
            b[2] = -1.0;
        }

        /* Scale the parameters to get unity gain in the bassband */

        if (stop) {
            /* Band-stop: unity gain at zero frequency */
            _z.re = 1.0;
            _z.im = 0.0;
        } else {
            /* Band-pass: unity gain at Ï‰ */
            _z.re = cos(w);
            _z.im = -sin(w);
        }

        p.re = b[2];
        p.im = 0;
        complex_mul(&p, &_z);
        p.re += b[1];
        complex_mul(&p, &_z);
        p.re += b[0];

        q.re = a[2];
        q.im = 0;
        complex_mul(&q, &_z);
        q.re += a[1];
        complex_mul(&q, &_z);
        q.re += 1.0;

        complex_div(&p, &q);

        x = p.re;
        y = p.im;
        k = 1.0 / sqrt(x * x + y * y);

        b[0] *= k;
        b[1] *= k;
        b[2] *= k;

        a += filter->sect_ord + 1;
        b += filter->sect_ord + 1;
    }

    for (i = 0; i < (n + 1) * 2; i += 1)
        filter->d[i] = 0;
}
void biquad_init_bandpass(struct iir_filter *filter, FLOAT_t fs, FLOAT_t f1, FLOAT_t f2)
{
	return biquad_init_band(filter, fs, f1, f2, 0);
}

void biquad_init_bandstop(struct iir_filter *filter, FLOAT_t fs, FLOAT_t f1, FLOAT_t f2)
{
	return biquad_init_band(filter, fs, f1, f2, 1);
}

#endif
