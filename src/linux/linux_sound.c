/*
 * By RA4ASN
 */

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"
#include "buffers.h"
#include "radio.h"
#include "audio.h"

#if LINUX_SUBSYSTEM

#include "linux_subsystem.h"
#include "common.h"

#if defined(CODEC1_TYPE) && (CODEC1_TYPE == CODEC_TYPE_ALSA)

#include <alsa/asoundlib.h>

pthread_t alsa_t;
snd_pcm_t * pcm_ph = NULL;
snd_pcm_uframes_t frames = DMABUFFSIZE16TX / DMABUFFSTEP16TX;
snd_pcm_uframes_t alsa_buffer_size = ARMI2SRATE * 50 / 1000; // 50 мс
unsigned int actual_sample_rate = ARMI2SRATE;
snd_pcm_sframes_t error;
uint8_t vol_shift = 0;

void * alsa_thread(void * args)
{
	const snd_pcm_uframes_t half_buf = alsa_buffer_size / 2;

	while(1)
	{
		snd_pcm_sframes_t avail = snd_pcm_avail_update(pcm_ph);

		if (avail == -EPIPE)
		{
			printf("Buffer overrun, recovering...");
			snd_pcm_recover(pcm_ph, avail, 0);
		}
		else if (avail == -ESTRPIPE)
		{
			printf("Threat was freesing, resuming...");
			snd_pcm_resume(pcm_ph);
		}
		else if (avail > 0 && avail < half_buf)
			usleep(1000);
		else
		{
			const uintptr_t addr_ph = getfilled_dmabuffer16tx();
			int32_t * b = (int32_t *) addr_ph;
#if WITHAUDIOSAMPLESREC && WITHTOUCHGUI
			as_rx(b);
#endif /* WITHAUDIOSAMPLESREC && WITHTOUCHGUI*/

			arm_shift_q31(b, - vol_shift, b, DMABUFFSIZE16TX);

		    if ((error = snd_pcm_writei(pcm_ph, b, frames)) != frames) {
		    	printf("Write to PCM device failed: %s\n", snd_strerror(error));
		        if (error == -EPIPE)
		            snd_pcm_prepare(pcm_ph);
		        else if (error == -ESTRPIPE)
		        	snd_pcm_resume(pcm_ph);
		    }

			release_dmabuffer16tx(addr_ph);
		}
	}
}

int alsa_init(const char * card_name)
{
	if ((error = snd_pcm_open(& pcm_ph, card_name, SND_PCM_STREAM_PLAYBACK, SND_PCM_ASYNC)) < 0) {
		printf("Cannot open PCM device: %s\n", snd_strerror(error));
		return 1;
	}

	snd_pcm_hw_params_t *params;
	snd_pcm_hw_params_alloca(&params);

	if ((error = snd_pcm_hw_params_any(pcm_ph, params)) < 0) {
		printf("Cannot initialize hardware parameter structure: %s\n", snd_strerror(error));
		return 1;
	}

	if ((error = snd_pcm_hw_params_set_access(pcm_ph, params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		printf("Cannot set access type: %s\n", snd_strerror(error));
		return 1;
	}

	if ((error = snd_pcm_hw_params_set_format(pcm_ph, params, SND_PCM_FORMAT_S24_LE)) < 0) {
		printf("Cannot set sample format: %s\n", snd_strerror(error));
		return 1;
	}

	if ((error = snd_pcm_hw_params_set_rate_near(pcm_ph, params, & actual_sample_rate, 0)) < 0) {
		printf("Cannot set sample rate: %s\n", snd_strerror(error));
		return 1;
	}

	if ((error = snd_pcm_hw_params_set_channels(pcm_ph, params, DMABUFFSTEP16TX)) < 0) {
		printf("Cannot set channel count: %s\n", snd_strerror(error));
		return 1;
	}

	if ((error = snd_pcm_hw_params_set_buffer_size_near(pcm_ph, params, & alsa_buffer_size)) < 0) {
		printf("Cannot set channel count: %s\n", snd_strerror(error));
		return 1;
	}

	if ((error = snd_pcm_hw_params(pcm_ph, params)) < 0) {
		printf("Cannot set parameters: %s\n", snd_strerror(error));
		return 1;
	}

	int32_t * s = calloc(alsa_buffer_size, sizeof(int32_t));
	snd_pcm_writei(pcm_ph, s, alsa_buffer_size);
	free(s);

	linux_create_thread(& alsa_t, alsa_thread, 50, alsa_thread_core);

	return 0;
}

void alsa_close(void)
{
	linux_cancel_thread(alsa_t);
	snd_pcm_drain(pcm_ph);
	snd_pcm_close(pcm_ph);
}

#if BLUETOOTH_ALSA

char alsa_cards[2][10] = { "default", "bluealsa" };
char names[2][10] = { "speaker", "bluetooth" };
uint8_t card_id = 0;

char * get_alsa_out(void)
{
	ASSERT(card_id < 2);
	return names[card_id];
}

void alsa_switch_out(void)
{
	card_id = (card_id + 1) % 2;

	alsa_close();
	if(alsa_init(alsa_cards[card_id]))
	{
		// если bluetooth audio не доступно, повторить инициализацию по умолчанию
		card_id = 0;
		alsa_init(alsa_cards[card_id]);
	}
}

#endif /* BLUETOOTH_ALSA */

static void dummy_stop (void) {}
uint_fast8_t dummy_clocksneed(void) {}
static void dummy_setprocparams (uint_fast8_t procenable, const uint_fast8_t * gains) {}
static void dummy_lineinput(uint_fast8_t v, uint_fast8_t mikeboost20db, uint_fast16_t mikegain, uint_fast16_t linegain) {}
static void dummy_setlineinput (uint_fast8_t linein, uint_fast8_t mikeboost20db, uint_fast16_t mikegain, uint_fast16_t linegain) {}

static void alsa_setvolume (uint_fast16_t gainL, uint_fast16_t gainR, uint_fast8_t mute, uint_fast8_t mutespk)
{
	vol_shift = mutespk ? BOARD_AFGAIN_MAX : BOARD_AFGAIN_MAX - gainL;
}

static void alsa_initialize(void (* io_control)(uint_fast8_t on), uint_fast8_t master)
{
	ASSERT(! alsa_init("default"));
}

const codec1if_t *
board_getaudiocodecif(void)
{

	static const char codecname [] = "ALSA";

	/* Интерфейс управления кодеком */
	static const codec1if_t ifc =
	{
		dummy_clocksneed,
		dummy_stop,
		alsa_initialize,
		alsa_setvolume,
		dummy_lineinput,
		dummy_setprocparams,
		codecname
	};

	return & ifc;
}

#endif /* (CODEC1_TYPE == CODEC_TYPE_ALSA) */

#endif /* LINUX_SUBSYSTEM */
