#ifndef LINUX_SUBSYSTEM_H
#define LINUX_SUBSYSTEM_H

#include "hardware.h"

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

extern pthread_mutex_t md;

#define LCLSPINLOCK_t		pthread_mutex_t
#define LCLSPINLOCK_INIT	PTHREAD_MUTEX_INITIALIZER
#define LCLSPINLOCK_INITIALIZE(p)	do { memcpy((void *) p, & md, sizeof(md)); } while(0)


uint_fast8_t dummy_putchar(uint_fast8_t c);
uint_fast8_t dummy_getchar(char * cp);

#define HARDWARE_DEBUG_INITIALIZE() 		do { } while (0)
#define HARDWARE_DEBUG_SET_SPEED(baudrate) 	do { } while (0)
#define HARDWARE_DEBUG_PUTCHAR(c)			(dummy_putchar(c))
#define HARDWARE_DEBUG_GETCHAR(pc) 			(dummy_getchar(pc))

typedef pthread_mutex_t lclspinlock_t;

void linux_exit(void);
void linux_subsystem_init(void);
void linux_user_init(void);
int linux_framebuffer_init(void);
uint32_t * linux_get_fb(uint32_t * size);
void linux_create_thread(pthread_t * tid, void * process, int priority, int cpuid);
void linux_run_shell_cmd(const char * argv []);

uint8_t linux_xgpi_read_pin(uint8_t pin);
void linux_xgpo_write_pin(uint8_t pin, uint8_t val);
uint_fast8_t gpio_readpin(uint8_t pin);
void gpio_writepin(uint8_t pin, uint8_t val);

uint16_t linux_i2c_read(uint16_t slave_address, uint16_t reg, uint8_t * buf, const uint8_t size);

void cs_i2c_assert(spitarget_t target);
void cs_i2c_deassert(spitarget_t target);
void cs_i2c_disable(void);

float xczu_get_cpu_temperature(void);
void hamradio_set_hw_vfo(uint_fast8_t v);

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

#endif /* LINUX_SUBSYSTEM */
#endif /* LINUX_SUBSYSTEM_H */
