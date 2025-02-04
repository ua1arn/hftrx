#ifndef AUDIO_REVERB_INCLUDED
#define AUDIO_REVERB_INCLUDED

#include "audio.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void audio_reverb_set_delay(unsigned delay);
void audio_reverb_set_loss(FLOAT_t ratio);
FLOAT_t audio_reverb_calc(FLOAT_t in);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
