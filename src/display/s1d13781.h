/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Поддержка контроллера TFT панели Epson S1D13781
//

#ifndef S1D13781_H_INCLUDED
#define S1D13781_H_INCLUDED

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if LCDMODE_S1D13781 //|| LCDMODE_LQ043T3DX02K

#define DISPLAYMODES_FPS 5	/* количество обновлений отображения режимов работы */
#define DISPLAY_FPS	20	/* обновление показаний частоты десять раз в секунду */
#define DISPLAYSWR_FPS 10	/* количество обновлений SWR и панорамы за секунду */

#define LCDMODE_COLORED	1

#if LCDMODE_S1D13781

	// размеры могут отличаться - для эмуляции мелких дисплеев на этом.
	//#define DIM_X               480
	//#define DIM_Y              272
	#define S1D_DISPLAY_WIDTH               480
	#define S1D_DISPLAY_HEIGHT              272

	#define S1D_DISPLAY_FRAME_RATE          0
	#define S1D_DISPLAY_PCLK                9000000L
	#define S1D_PHYSICAL_REG_ADDR           0x00060800uL
	#define S1D_PHYSICAL_VMEM_ADDR          0x00000000uL
	#define S1D_PHYSICAL_REG_SIZE           593L
	#define S1D_PHYSICAL_VMEM_SIZE          393216L
	#define S1D_PALETTE_SIZE                256

	#if 0

		// 24 bpp
		#define S1D_DISPLAY_BPP                 24
		#define S1D_DISPLAY_SCANLINE_BYTES      (S1D_DISPLAY_WIDTH * 3)
		#define S1D_PHYSICAL_VMEM_REQUIRED      391680L		// память без учёта PIP
		#define LCDMODE_PIXELSIZE 3

	#elif 1

		// 16 bpp
		#define S1D_DISPLAY_BPP                 16
		#define S1D_DISPLAY_SCANLINE_BYTES      (S1D_DISPLAY_WIDTH * 2)
		#define S1D_PHYSICAL_VMEM_REQUIRED      261120L		// память без учёта PIP
		#define LCDMODE_PIXELSIZE 2

	#else

		// 8 bpp
		#define S1D_DISPLAY_BPP                 8
		#define S1D_DISPLAY_SCANLINE_BYTES      (S1D_DISPLAY_WIDTH * 1)
		#define S1D_PHYSICAL_VMEM_REQUIRED      130560L		// память без учёта PIP
		#define LCDMODE_PIXELSIZE 1

	#endif

	#define FREEMEMSTART S1D_PHYSICAL_VMEM_REQUIRED
	//#define FREEMEMSTART (S1D_PHYSICAL_VMEM_REQUIRED * 2) // эксперементы с PIP
	#define PIPMEMSTART (S1D_PHYSICAL_VMEM_REQUIRED * 1) // эксперементы с PIP
	// 480 x 272 pixel

#endif

#if (S1D_DISPLAY_BPP == 8) || LCDMODE_MAIN_L8


	typedef uint_fast8_t COLORMAIN_T;
	typedef uint8_t PACKEDCOLORMAIN_T;

	// RRRGGGBB

	#define TFTRGB(red, green, blue) \
		(  (unsigned char) \
			(	\
				(((red) >> 0) & 0xe0)  | \
				(((green) >> 3) & 0x1c) | \
				(((blue) >> 6) & 0x03) \
			) \
		)

	typedef PACKEDCOLORMAIN_T PACKEDCOLORPIP_T;
	typedef COLORMAIN_T COLORPIP_T;

#elif S1D_DISPLAY_BPP == 16 || ! LCDMODE_MAIN_L8

	//#define LCDMODE_RGB565 1
	typedef uint_fast16_t COLORMAIN_T;
	typedef uint16_t PACKEDCOLORMAIN_T;

	// RRRR.RGGG.GGGB.BBBB
	#define TFTRGB(red, green, blue) \
		(  (uint_fast16_t) \
			(	\
				(((uint_fast16_t) (red) << 8) &   0xf800)  | \
				(((uint_fast16_t) (green) << 3) & 0x07e0) | \
				(((uint_fast16_t) (blue) >> 3) &  0x001f) \
			) \
		)

	#define TFTRGB565 TFTRGB

	typedef PACKEDCOLORMAIN_T PACKEDCOLORPIP_T;
	typedef COLORMAIN_T COLORPIP_T;

#elif S1D_DISPLAY_BPP == 24

	typedef uint_fast32_t COLORMAIN_T;
	typedef uint32_t PACKEDCOLORMAIN_T;

	// RRRRRRR.GGGGGGGG.BBBBBBBB
	#define TFTRGB(red, green, blue) \
		(  (unsigned long) \
			(	\
				(((unsigned long) (red) << 16) &   0xFF0000ul)  | \
				(((unsigned long) (green) << 8) & 0xFF00ul) | \
				(((unsigned long) (blue) >> 0) &  0xFFul) \
			) \
		)

	typedef PACKEDCOLORMAIN_T PACKEDCOLORPIP_T;
	typedef COLORMAIN_T COLORPIP_T;

#endif

#endif /* LCDMODE_S1D13781 */
#endif /* S1D13781_H_INCLUDED */

