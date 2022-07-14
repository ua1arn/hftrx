#ifndef AUDIO_COMPRESSOR_INCLUDED
#define AUDIO_COMPRESSOR_INCLUDED

#include "audio.h"

void audio_compressor_set_attack(unsigned samples);
void audio_compressor_set_release(unsigned samples);
void audio_compressor_set_hold(unsigned samples);
void audio_compressor_set_threshold(FLOAT_t ratio);
void audio_compressor_set_gainreduce(FLOAT_t ratio);
void audio_compressor_recalc();

FLOAT_t audio_compressor_calc(FLOAT_t in);

#endif
