#ifndef __SDHCI_H__
#define __SDHCI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//
//#include "Type.h"
//#include "types.h"
//
//#include "Gate.h"
//#include "timer.h"

#include "sdmmc.h"

//#define SMHC0_BASE 0x04020000

//#define SLOW_CLOCK             400000UL //400 kHz
//#define HOSC_CLOCK           24000000UL // 24 MHz
//#define PLLPERI1X_CLOCK_DIV 100000000UL //100 MHz

//#define FALSE 0
//#define TRUE  1

#define read32(r)    (*(volatile uint32_t*)(r))
#define write32(r,v) do {(*(volatile uint32_t*)(r))=(v); } while (0)

/*
static inline uint32_t read32(uint32_t r)
{
 return *(IO uint32_t*)r;
}

static inline void write32(uint32_t r,uint32_t v)
{
 *(IO uint32_t*)r=v;
}
*/

//#include "delay.h"

#define udelay local_delay_us

#ifndef min
#define min(x,y)  (((x)<(y))?(x):(y))
#endif

//#define LOG(...)

//----------------------------------------------------
#if 0
#define TICK_PER_MILLISECOND 6000 /* ������ 6 MHz */

#define ktime_t unsigned int

static inline uint32_t ktime_get(void)
{
 return AVS_CNT0_REG;
}

static inline uint32_t ktime_add_ms(uint32_t a,uint32_t b)
{
 return (a+b)*TICK_PER_MILLISECOND;
}

static inline uint8_t ktime_after(uint32_t a,uint32_t b)
{
 if(a>b)return 1;
 return 0;
}
#endif
//----------------------------------------------------

struct sdhci_cmd_t {
	uint32_t cmdidx;
	uint32_t cmdarg;
	uint32_t resptype;
	uint32_t response[4];
};

struct sdhci_data_t {
	uint8_t * buf;
	uint32_t flag;
	uint32_t blksz;
	uint32_t blkcnt;
};

struct sdhci_t
{
	uint32_t voltage;
	uint32_t width;
	uint32_t clock;
	int isspi;
	uintptr_t base;
	SMHC_TypeDef * instance;
};

#ifdef __cplusplus
}
#endif

#endif
