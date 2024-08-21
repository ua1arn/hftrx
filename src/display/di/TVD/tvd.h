#ifndef _TVD_H_
#define _TVD_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "bsp_tvd.h"

#define TVD_INT_NUMBER 139


void TVD_Init(uint32_t mode);   //mode: NTSC, PAL
void TVD_CaptureOn(void);
void TVD_CaptureOff(void);
uint32_t TVD_Status(void);

#ifdef __cplusplus
}
#endif

#endif
