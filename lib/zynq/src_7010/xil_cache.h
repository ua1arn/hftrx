#ifndef XIL_CACHE_H
#define XIL_CACHE_H

#include "hardware.h"

#ifdef __cplusplus
extern "C" {
#endif

void Xil_DCacheEnable(void);
void Xil_DCacheDisable(void);
void Xil_DCacheInvalidate(void);
void Xil_DCacheInvalidateRange(uintptr_t adr, uint32_t len);
void Xil_DCacheFlush(void);
void Xil_DCacheFlushRange(uintptr_t adr, uint32_t len);

void Xil_ICacheEnable(void);
void Xil_ICacheDisable(void);
void Xil_ICacheInvalidate(void);
void Xil_ICacheInvalidateRange(uintptr_t adr, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif
