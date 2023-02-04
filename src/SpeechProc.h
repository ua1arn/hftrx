#ifndef speech_proc_h
#define speech_proc_h
//#include "global.h"

#define PH_STAGES_MAX 31
#define SYMDETSMOOTH 0.99f
#define COMP_SMOOTH_FACTOR 0.999f
/*до 110, 110-300, 300-800, 800-2000, 2000 и выше.
По крайней мере оно среднеарифметическое */
#define BAND1_MIN_FREQ 0
#define BAND1_MAX_FREQ 110
#define BAND2_MIN_FREQ 110
#define BAND2_MAX_FREQ 300
#define BAND3_MIN_FREQ 300
#define BAND3_MAX_FREQ 800
#define BAND4_MIN_FREQ 800
#define BAND4_MAX_FREQ 2000
#define BAND5_MIN_FREQ 2000
#define BAND5_MAX_FREQ 3500
#define MAX_16bit_FLOAT 32768.0f
#define N_POLOS 5

typedef struct
{
float max_level_dB;
float  slope;//компрессия, в дБ
unsigned     sr;
float threshold;
int threshold_dB;//дБ, минимум -90, число относительно max_level_dB, отрицательное
float gain;
float gain_dB;
float env;
float env_dB;
float ratio;
unsigned trel;
float rel;
float tatt;//время атаки, в мс,
float att;
float ave_out;
float ave_gain;
int ave_gain_dB;

float knee;
float knee_width_dB;
float knee_low_bound_dB;
float knee_up_bound_dB;


float knee_width;
float knee_low_bound;
float knee_up_bound;

}
compparam_t;

typedef struct
{
float a[3];
float b[3];
float w[3];
//float fs;// sample rate
float Df;//bandwidth
float G;
//float GdB;//требуемое усиление , в дБ
float GB;
float be;
float c0;
float f0;//central frequency
}
eqparam_t;

typedef struct 
{
float fc;
float g;
//float sr;
unsigned nstages;
float x[2][PH_STAGES_MAX];
float y[2][PH_STAGES_MAX];
float a[2];
float b[2];
}
phase_rotator_t;

typedef struct
{
float alpha;
float neg_ave;
float pos_ave;
float symmetry;
unsigned cnt;
}
symdet_t;
/*extern void SpeechCompressor
    (
        float*  wav_in,     // signal
        int     n,          // N samples
            compparam_t* param
    );*/
//extern void Equalizer (float * in, int n, eqparam_t* param);
extern void SpeechProc (float* input, unsigned n);
extern void multiband_compressor (float* in, unsigned n, int threshold_dB, float slope_dB );
//extern void SpeechCompressorInit(compparam_t* param);
#endif
