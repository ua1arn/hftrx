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
void ft8_encode_buf(float * signal, char * message, float frequency);

static uint8_t ft8_enable = 0, ft8_rxfill = 0, ft8_tx = 0;
static struct cond_thread ct_ft8;
static pthread_t ft8_sync_t, ft8_proc_t;
static subscribefloat_t ft8_outregister;
ft8_t ft8;
static timestamp_t ts;
static pthread_mutex_t mutex_ft8;

static float rx_buf[2][ft8_sample_rate * ft8_length];
static uint8_t buf_num = 0;	// current filling buffer
static uint32_t idx = 0;

void ft8_processing_thread(void)
{
	while(1)
	{
		safe_cond_wait(& ct_ft8);
		board_rtc_cached_gettime(& ts.hour, & ts.minute, & ts.second);
		printf("%02d:%02d:%02d\n", ts.hour, ts.minute, ts.second);
		uint8_t prev_idx = (buf_num + 1) % 2;
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
		pthread_mutex_lock(& mutex_ft8);
		buf_num = (buf_num + 1) % 2;
		idx = 0;
		pthread_mutex_unlock(& mutex_ft8);
		safe_cond_signal(& ct_ft8);
	}
}

void ft8_txfill(float * sample)
{
	const uint32_t bufsize = ft8_sample_rate * ft8_length;
	static uint32_t tx_ind = 0;

	if (ft8_tx && hamradio_get_tx())
	{
		* sample = ft8.tx_buf [tx_ind];
		tx_ind ++;
		if (tx_ind >= bufsize || ft8.tx_buf [tx_ind] == 0)
		{
			pthread_mutex_lock(& mutex_ft8);
			ft8_tx = 0;
			tx_ind = 0;
			pthread_mutex_unlock(& mutex_ft8);
			hamradio_moxmode(1);
		}
	}
}

static void ft8_fill(void * ctx, FLOAT_t ch0, FLOAT_t ch1)
{
	pthread_mutex_lock(& mutex_ft8);

	if (ft8_enable && ft8_rxfill && idx < ft8_sample_rate * ft8_length)
	{
		ASSERT(buf_num < 2);

		rx_buf[buf_num][idx] = ch0;
		idx ++;
	}

	pthread_mutex_unlock(& mutex_ft8);
}

void ft8_set_state(uint8_t v)
{
	ft8_enable = v != 0;

	if (v)
	{
		subscribefloat(& speexoutfloat, & ft8_outregister, NULL, ft8_fill);
		linux_init_cond(& ct_ft8);
		linux_create_thread(& ft8_sync_t, ft8_sync_thread, 50, 1);
		linux_create_thread(& ft8_proc_t, ft8_processing_thread, 5, 0);

		pthread_mutex_lock(& mutex_ft8);
		ft8_rxfill = 1;
		pthread_mutex_unlock(& mutex_ft8);
	}
	else
	{
		pthread_mutex_lock(& mutex_ft8);
		ft8_rxfill = 0;
		pthread_mutex_unlock(& mutex_ft8);

		linux_cancel_thread(ft8_proc_t);
		linux_cancel_thread(ft8_sync_t);
		linux_destroy_cond(& ct_ft8);
		unsubscribefloat(& speexoutfloat, & ft8_outregister);
	}
}

uint8_t ft8_get_state(void)
{
	return ft8_enable;
}

void ft8_do_encode(void)
{
	pthread_mutex_lock(& mutex_ft8);
	ft8_rxfill = 0;
	pthread_mutex_unlock(& mutex_ft8);

	memset(ft8.tx_buf, 0, sizeof(float) * ft8_sample_rate * ft8_length);
	ft8_encode_buf(ft8.tx_buf, ft8.tx_text, ft8.tx_freq);
	hamradio_moxmode(1);

	pthread_mutex_lock(& mutex_ft8);
	ft8_tx = 1;
	pthread_mutex_unlock(& mutex_ft8);
}

void ft8_initialize(void)
{
	pthread_mutex_init(& mutex_ft8, NULL);
}

#endif /* LINUX_SUBSYSTEM && WITHFT8 */
