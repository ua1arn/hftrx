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

#define SPINLOCK_t		pthread_mutex_t
#define SPINLOCK_INIT	PTHREAD_MUTEX_INITIALIZER
#define SPINLOCK_INITIALIZE(p)	do { memcpy((void *) p, & md, sizeof(md)); } while(0)

void HARDWARE_DEBUG_GETCHAR(char * c)	{ * c = 0; }
void HARDWARE_DEBUG_PUTCHAR(char c)		{  }

typedef pthread_mutex_t spinlock_t;

void linux_subsystem_init(void);
void linux_user_init(void);
int linux_framebuffer_init(void);
uint32_t * linux_get_fb(uint32_t * size);

uint8_t linux_xgpi_read_pin(uint8_t pin);
void linux_xgpo_write_pin(uint8_t pin, uint8_t val);
uint_fast8_t gpio_readpin(uint8_t pin);
void gpio_writepin(uint8_t pin, uint8_t val);

uint16_t linux_i2c_read(uint16_t slave_address, uint16_t reg, uint8_t * buf, const uint8_t size);

void linux_spi_init(void);
void spi_cs_init(uint8_t cs);
void spi_cs_set(uint8_t cs, uint8_t val);
void spi_sclk_set(void);
void spi_sclk_clear(void);
void spi_mosi_set(void);
void spi_mosi_clear(void);
uint8_t spi_miso_get(void);

void board_rtc_getdate(
	uint_fast16_t * year,
	uint_fast8_t * month,
	uint_fast8_t * dayofmonth
	);

void board_rtc_gettime(
	uint_fast8_t * hour,
	uint_fast8_t * minute,
	uint_fast8_t * secounds
	);

#endif /* LINUX_SUBSYSTEM */
#endif /* LINUX_SUBSYSTEM_H */
