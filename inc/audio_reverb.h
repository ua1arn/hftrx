#ifndef AUDIO_REVERB_INCLUDED
#define AUDIO_REVERB_INCLUDED

#include "audio.h"

extern void audio_reverb_set_delay(FLOAT_t delay);
extern void audio_reverb_set_loss(FLOAT_t loss);
extern FLOAT_t audio_reverb_calc(FLOAT_t in);

#endif
