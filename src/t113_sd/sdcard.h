#ifndef __SDCARD_H__
#define __SDCARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "sdhci-t113.h"

struct sdcard_t
{
	uint32_t version;
	uint32_t ocr;
	uint32_t rca;
	uint32_t cid[4];
	uint32_t csd[4];
	uint8_t extcsd[512];

	uint32_t high_capacity;
	uint32_t tran_speed;
	uint32_t dsr_imp;
	uint32_t read_bl_len;
	uint32_t write_bl_len;
	uint64_t capacity;
};

int sdcard_init(void);

uint64_t mmc_read_blocks(uint8_t * buf, uint64_t start, uint64_t blkcnt);
uint64_t mmc_write_blocks(uint8_t * buf, uint64_t start, uint64_t blkcnt);

#ifdef __cplusplus
}
#endif

#endif
