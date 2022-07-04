#ifndef AUDIO_REVERB_INCLUDED
#define AUDIO_REVERB_INCLUDED

#include "audio.h"

void audio_reverb_set_delay(unsigned delay);
void audio_reverb_set_loss(FLOAT_t ratio);
FLOAT_t audio_reverb_calc(FLOAT_t in);

#endif
