#ifndef _G2D_H_
#define _G2D_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "g2d_bsp.h"

void G2D_Init(void);

extern unsigned int PorterDuff;

extern enum g2d_scan_order scan_order;

int g2d_fill(g2d_fillrect *para);
int g2d_blit(g2d_blt *para);
int g2d_stretchblit(g2d_stretchblt *para);

#ifdef __cplusplus
}
#endif

#endif
