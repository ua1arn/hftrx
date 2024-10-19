#ifndef MAIN_H_
#define MAIN_H_

#include "hardware.h"
#include "formats.h"

#include "display.h"

extern RAMNC uint8_t xxfb1 [512 * 512 * 4];
extern RAMNC uint8_t xxfb2 [512 * 512 * 4];
//	extern RAMNC uint8_t xxfb3 [512 * 512 * 4];
#define  framebuffer1 ((uintptr_t) xxfb1) // 0x43000000
#define  framebuffer2 ((uintptr_t) xxfb2) // (0x45000000+(512*512))
//	#define  framebuffer3 ((uintptr_t) xxfb3) // (0x47000000+2*(512*512))

#define LCDX 512
#define LCDY 270
#define LCD_FRAME_OFFSET (LCDX*LCDY)

#define SOCID_A64	0x1689
#define SOCID_H3	0x1680
#define SOCID_V3S	0x1681
#define SOCID_H5	0x1718
#define SOCID_R40	0x1701


#endif
