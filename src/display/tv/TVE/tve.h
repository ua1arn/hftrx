#ifndef _TVE_H_
#define _TVE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "de_tvec.h"

//#define DISPLAY_TOP_BASE 0x05460000
//#define TCON_TV0_BASE    0x05470000
//#define TVE_TOP_BASE     0x05600000
#define TVE_BASE         0x05604000

#define TV_INT_NUMBER  123
#define TVE_INT_NUMBER 126

void TCONTVandTVE_Init(unsigned int mode);
void TV_VSync(void);

#ifdef __cplusplus
}
#endif

#endif
