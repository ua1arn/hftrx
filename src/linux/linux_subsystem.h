#ifndef LINUX_SUBSYSTEM_H
#define LINUX_SUBSYSTEM_H

#include "hardware.h"
#include "src/display/display.h"
#include "spi.h"

#if LINUX_SUBSYSTEM

#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <err.h>
#include <errno.h>
#include <pthread.h>
#include "math.h"

#include <stdio.h>

#include <err.h>
#include <errno.h>

#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* Audio register map definitions */
#define AUDIO_REG_I2S_RESET 		 0x00   //Write only
#define AUDIO_REG_I2S_CTRL			 0x04
#define AUDIO_REG_I2S_CLK_CTRL 		 0x08
#define AUDIO_REG_I2S_PERIOD 		 0x18
#define AUDIO_REG_I2S_RX_FIFO 		 0x28   //Read only
#define AUDIO_REG_I2S_TX_FIFO 		 0x2C	//Write only


/* I2S reset mask definitions  */
#define TX_FIFO_RESET_MASK 		 	0x00000002
#define RX_FIFO_RESET_MASK 		 	0x00000004

/* I2S Control mask definitions  */
#define TX_ENABLE_MASK 		 		0x00000001
#define RX_ENABLE_MASK 		 		0x00000002

/* I2S Clock Control mask definitions  */
#define BCLK_DIV_MASK 		 		0x000000FF
#define LRCLK_DIV_MASK 		 		0x00FF0000

#if WITHEXTIO_LAN

enum {
	STREAM_IDLE,
	STREAM_LISTEN,
	STREAM_CONNECTED,
	STREAM_DISCONNECTED,
};

void server_restart(void);
void server_stop(void);
void server_start(void);
uint8_t stream_get_state(void);

#endif /* WITHEXTIO_LAN */

#if WITHAUDIOSAMPLESREC

enum {
	AS_IDLE,
	AS_RECORDING,
	AS_PLAYING,
	AS_TX,
	AS_READY,

	AS_COUNT,
};

uint8_t as_get_state(void);
uint8_t as_get_progress(void);
void as_toggle_record(void);
void as_toggle_play(void);
void as_toggle_trx(void);
void as_draw_spectrogram(COLORPIP_T * d, uint16_t len, uint16_t lim);
void as_tx(uint32_t * buf);
void as_rx(uint32_t * buf);

#endif /* WITHAUDIOSAMPLESREC */

extern pthread_mutex_t linux_md;

#define LCLSPINLOCK_t		pthread_mutex_t
#define LCLSPINLOCK_INIT	PTHREAD_MUTEX_INITIALIZER
#define LCLSPINLOCK_INITIALIZE(p)	do { memcpy((void *) p, & linux_md, sizeof(linux_md)); } while(0)

struct cond_thread {
    pthread_cond_t   ready_cond;
    pthread_mutex_t  ready_mutex;
    void * tag;
};

uint_fast8_t dummy_putchar(uint_fast8_t c);
uint_fast8_t dummy_getchar(char * cp);

#define HARDWARE_DEBUG_INITIALIZE() 		do { } while (0)
#define HARDWARE_DEBUG_SET_SPEED(baudrate) 	do { } while (0)
#define HARDWARE_DEBUG_PUTCHAR(c)			(dummy_putchar(c))
#define HARDWARE_DEBUG_GETCHAR(pc) 			(dummy_getchar(pc))

#define __DMB()	do { } while (0)

typedef pthread_mutex_t lclspinlock_t;

void linux_exit(void);
void linux_subsystem_init(void);
void linux_user_init(void);
int linux_framebuffer_init(void);
uint32_t * linux_get_fb(uint32_t * size);
void linux_create_thread(pthread_t * tid, void * (* process)(void * args), int priority, int cpuid);
void linux_cancel_thread(pthread_t tid);
void linux_run_shell_cmd(const char * argv []);
void safe_cond_signal(struct cond_thread * ct);
void safe_cond_wait(struct cond_thread * ct);
void linux_init_cond(struct cond_thread * ct);
int linux_verify_cond(struct cond_thread * ct);
void linux_destroy_cond(struct cond_thread * ct);
void linux_wait_iq(void);
int rpmsg_init(void);
void evdev_initialize(void);

uint8_t linux_xgpi_read_pin(uint8_t pin);
void linux_xgpo_write_pin(uint8_t pin, uint8_t val);
uint_fast8_t gpio_readpin(uint8_t pin);
void gpio_writepin(uint8_t pin, uint8_t val);

void cs_i2c_assert(spitarget_t target);
void cs_i2c_deassert(spitarget_t target);
void cs_i2c_disable(void);

float xczu_get_cpu_temperature(void);
void hamradio_set_hw_vfo(uint_fast8_t v);

void ad936x_set_freq(long long freq);
uint8_t get_ad936x_stream_status(void);
int ad9363_iio_start (const char * uri);
void iio_stop_stream(void);
uint8_t get_status_iio(void);

uint8_t iio_ad936x_find(const char * uri);
uint8_t iio_ad936x_start(const char * uri);
uint8_t iio_ad936x_stop(void);
void iio_ad936x_set_gain(uint8_t type, int gain);

void board_rtc_getdate(
	uint_fast16_t * year,
	uint_fast8_t * month,
	uint_fast8_t * dayofmonth
	);

void board_rtc_gettime(
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * seconds
	);

void load_memory_cells(uint32_t * mem, uint8_t cnt);
void write_memory_cells(uint32_t * mem, uint8_t cnt);

int sdl2_render_init(void);
void sdl2_render_update(uintptr_t frame);
void sdl2_render_close(void);

#endif /* LINUX_SUBSYSTEM */
#endif /* LINUX_SUBSYSTEM_H */
