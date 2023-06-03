/*
 * By RA4ASN
 */

#include "hardware.h"	/* зависящие от процессора функции работы с портами */
#include "formats.h"	// for debug prints
#include "audio.h"
#include "board.h"
#include "ft8.h"

#if LINUX_SUBSYSTEM && WITHFT8

#include <sys\time.h>

void ft8_decode_buf(float * signal, timestamp_t ts);

static uint8_t ft8_enable = 0;
static struct cond_thread ct_ft8;
static pthread_t ft8_sync_t, ft8_proc_t;
static subscribefloat_t ft8_outregister;
ft8_t ft8;
static timestamp_t ts;
static pthread_mutex_t mutex_rxfill;

float rx_buf[2][ft8_sample_rate * ft8_length];
uint8_t rx_index = 0;	// current filling buffer
uint32_t idx = 0;

void ft8_processing_thread(void)
{
	while(1)
	{
		safe_cond_wait(& ct_ft8);
		board_rtc_cached_gettime(& ts.hour, & ts.minute, & ts.second);
		printf("%02d:%02d:%02d\n", ts.hour, ts.minute, ts.second);
		uint8_t prev_idx = (rx_index + 1) % 2;
		ft8_decode_buf(rx_buf[prev_idx], ts);
		hamradio_gui_parse_ft8buf();
	}
}

void ft8_sync_thread(void)
{
	struct timeval lTime;
	gettimeofday(&lTime, NULL);
	uint32_t sec   = lTime.tv_sec % 15;
	uint32_t usec  = sec * 1000000 + lTime.tv_usec;
	uint32_t uwait = 15000000 - usec;
	printf("FT8: wait for time sync (start in %d sec)\n", uwait / 1000000);

	while(1)
	{
		gettimeofday(&lTime, NULL);
		sec   = lTime.tv_sec % 15;
		usec  = sec * 1000000 + lTime.tv_usec;
		uwait = 15000000 - usec;
		usleep(uwait);
		pthread_mutex_lock(& mutex_rxfill);
		rx_index = (rx_index + 1) % 2;
		idx = 0;
		pthread_mutex_unlock(& mutex_rxfill);
		safe_cond_signal(& ct_ft8);
	}
}

void ft8_set_state(uint8_t v)
{
	ft8_enable = v != 0;

	if (v)
	{
		linux_init_cond(& ct_ft8);
		linux_create_thread(& ft8_sync_t, ft8_sync_thread, 50, 1);
		linux_create_thread(& ft8_proc_t, ft8_processing_thread, 5, 0);
	}
	else
	{
		linux_cancel_thread(ft8_proc_t);
		linux_cancel_thread(ft8_sync_t);
		linux_destroy_cond(& ct_ft8);
	}
}

uint8_t get_ft8_state(void)
{
	return ft8_enable;
}

void ft8_txfill(float * sample)
{

}

static void ft8_fill(void * ctx, FLOAT_t ch0, FLOAT_t ch1)
{
	if (ft8_enable)
	{
		pthread_mutex_lock(& mutex_rxfill);

		if (idx < ft8_sample_rate * ft8_length)
		{
			rx_buf[rx_index][idx] = ch0;
			idx ++;
		}

		pthread_mutex_unlock(& mutex_rxfill);
	}
}

void ft8_initialize(void)
{
	subscribefloat(& speexoutfloat, & ft8_outregister, NULL, ft8_fill);
	pthread_mutex_init(& mutex_rxfill, NULL);
}

#endif /* LINUX_SUBSYSTEM && WITHFT8 */
