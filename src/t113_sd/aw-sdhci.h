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

#include "aw-sdmmc.h"

#define read32(r)    (*(volatile uint32_t*)(r))
#define write32(r,v) do {(*(volatile uint32_t*)(r))=(v); } while (0)

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
