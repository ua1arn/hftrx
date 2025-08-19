/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Доработки для LS020 Василий Линывый, livas60@mail.ru
//
// Функции построения изображений в буфере - вне зависимости от того, есть ли в процессоре LTDC.'
// Например при offscreen composition растровых изображений для SPI дисплеев
//

#include "hardware.h"

#include "board.h"
#include "display.h"
#include "clocks.h"
#include "formats.h"
#include "spi.h"	// hardware_spi_master_send_frame
#include "utils.h"
#include "display2.h"
#include "fontmaps.h"
#include <string.h>


#if WITHLVGL

#include "draw/lv_draw_private.h"
#include "draw/lv_draw_image_private.h"

//#include "src/lvgl_gui/styles.h"

#endif /* WITHLVGL */

static void softfill(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	COLORPIP_T color	// цвет
	)
{
	// программная реализация
	const unsigned t = GXADJ(dx) - w;
	while (h --)
	{
		unsigned n = w;
		while (n)
		{
			switch (n)
			{
			default:
				* buffer ++ = color; -- n;
			case 3:
				* buffer ++ = color; -- n;
			case 2:
				* buffer ++ = color; -- n;
			case 1:
				* buffer ++ = color; -- n;
			}
		}
		buffer += t;
	}
}

#if CPUSTYLE_ALLWINNER && WITHMDMAHW
	/* Использование G2D для формирования изображений */

	//#include "g2d_driver.h"

	/* BLD LAYER ALPHA MODE*/
	typedef enum {
		xG2D_PIXEL_ALPHA,
		xG2D_GLOBAL_ALPHA,
		xG2D_MIXER_ALPHA,
	} xg2d_alpha_mode_enh;

#if LCDMODE_ARGB8888
	#define VI_ImageFormat 0x00	//G2D_FMT_ARGB_AYUV8888
	#define ROT_ImageFormat 0x00	//G2D_FMT_ARGB_AYUV8888
	#define UI_ImageFormat 0x00	//G2D_FMT_ARGB_AYUV8888
	#define WB_ImageFormat 0x00	//G2D_FMT_ARGB_AYUV8888

#elif LCDMODE_RGB565
	#define VI_ImageFormat 0x0A
	#define ROT_ImageFormat 0x0A
	#define UI_ImageFormat 0x0A
	#define WB_ImageFormat 0x0A

#else
	#error Unsupported framebuffer format. Looks like you need remove WITHLTDCHW
#endif


// 0x01: ABGR_8888
static unsigned awxx_get_srcformat(unsigned keyflag)
{
	return (keyflag & BITBLT_FLAG_SRC_ABGR8888) ? 0x01 : VI_ImageFormat;
}

static unsigned awxx_g2d_get_ui_attr(unsigned srcFormat)
{
	unsigned ui_attr = 0;
	ui_attr = UINT32_C(255) << 24;
	//	if (img->bpremul)
	//		vi_attr |= 0x2 << 16;	/* LAY_PREMUL_CTL */
	ui_attr |= srcFormat << 8;
	//ui_attr |= G2D_GLOBAL_ALPHA << 1; // linux sample use G2D_PIXEL_ALPHA -> 0xFF000401
	ui_attr |= xG2D_PIXEL_ALPHA << 1; // нужно для работы color key linux sample use G2D_PIXEL_ALPHA -> 0xFF000401
	//ui_attr |= (UINT32_C(1) << 4);	/* Use FILLC register */
	ui_attr |= 1;
	return ui_attr;
}

static unsigned awxx_g2d_get_vi_attr(unsigned srcFormat)
{
	unsigned vi_attr = 0;
	vi_attr = UINT32_C(255) << 24;
	vi_attr |= srcFormat << 8;
	vi_attr |= UINT32_C(1) << 15;
	//vi_attr |= G2D_GLOBAL_ALPHA << 1; // linux sample use G2D_PIXEL_ALPHA -> 0xFF000401
	vi_attr |= xG2D_PIXEL_ALPHA << 1; // нужно для работы color key linux sample use G2D_PIXEL_ALPHA -> 0xFF000401
	//vi_attr |= (UINT32_C(1) << 4);	/* Use FILLC register */
	vi_attr |= 1;
	return vi_attr;
}

/* Получение RGB888, который нужен для работы функций сравынния ключевого цвета.
 * Должно совпадать с алгоритмом выборки из памяти UIx_ и VI0_
 * */
static COLOR24_T awxx_key_color_conversion(COLORPIP_T color)
{
//	PRINTF("awxx_key_color_conversion: color=%08" PRIXFAST32 "\n", (uint_fast32_t) color);
//	PRINTF("awxx_key_color_conversion: r=%08" PRIXFAST32 "\n", (uint_fast32_t) COLORPIP_R(color));
//	PRINTF("awxx_key_color_conversion: g=%08" PRIXFAST32 "\n", (uint_fast32_t) COLORPIP_G(color));
//	PRINTF("awxx_key_color_conversion: b=%08" PRIXFAST32 "\n", (uint_fast32_t) COLORPIP_B(color));
	return COLOR24(COLORPIP_R(color), COLORPIP_G(color), COLORPIP_B(color));
}

/* Создание режима блендера BLD_CTL */
// 5.10.9.9 BLD control register

static uint_fast32_t awxx_bld_ctl(
	uint_fast32_t afd,	// Specifies the coefficient that used in destination alpha data Qd.
	uint_fast32_t afs,	// Specifies the coefficient that used in source alpha data Qs.
	uint_fast32_t pfd,	// Specifies the coefficient that used in destination pixel data Qs.
	uint_fast32_t pfs	// Specifies the coefficient that used in source pixel data Qs.
	)
{
	return
		(afd << 24) |	// BLEND_AFD Specifies the coefficient that used in destination alpha data Qd.
		(afs << 16) |	// BLEND_AFS Specifies the coefficient that used in source alpha data Qs.
		(pfd << 8) |	// BLEND_PFD Specifies the coefficient that used in destination pixel data Qs.
		(pfs << 0) |	// BLEND_PFS Specifies the coefficient that used in source pixel data Qs.
		0;
}
static uint_fast32_t awxx_bld_ctl2(
	uint_fast32_t fd,	// Specifies the coefficient that used in destination data Qd.
	uint_fast32_t fs	// Specifies the coefficient that used in source data Qs.
	)
{
	return awxx_bld_ctl(fd, fs, fd, fs);
}

#if defined (G2D_MIXER)

static void aw_g2d_initialize(void)
{
}

/* Отключаем все источники */
static void awxx_g2d_mixer_reset(void)
{
	ASSERT((G2D_MIXER->G2D_MIXER_CTRL & (UINT32_C(1) << 31)) == 0);

	//	memset(G2D_V0, 0, sizeof * G2D_V0);
	//	memset(G2D_UI0, 0, sizeof * G2D_UI0);
	//	memset(G2D_UI1, 0, sizeof * G2D_UI1);
	//	memset(G2D_UI2, 0, sizeof * G2D_UI2);
	//	memset(G2D_BLD, 0, sizeof * G2D_BLD);
	//	memset(G2D_WB, 0, sizeof * G2D_WB);

	//	G2D_TOP->G2D_AHB_RST &= ~ ((UINT32_C(1) << 1) | (UINT32_C(1) << 0));	// Assert reset: 0x02: rot, 0x01: mixer
	//	G2D_TOP->G2D_AHB_RST |= (UINT32_C(1) << 1) | (UINT32_C(1) << 0);	// De-assert reset: 0x02: rot, 0x01: mixer

	G2D_VSU->VS_CTRL = 0;
	G2D_ROT->ROT_CTL = 0;
	G2D_V0->V0_ATTCTL = 0;
	G2D_UI0->UI_ATTR = 0;
	G2D_UI1->UI_ATTR = 0;
	G2D_UI2->UI_ATTR = 0;
	G2D_BLD->BLD_FILL_COLOR_CTL = 0;
	G2D_BLD->BLD_KEY_CTL = 0;
}

// Register Configuration Queue setup
static void awxx_rcq(uintptr_t buff, unsigned len)
{
	G2D_TOP->RCQ_CTRL = 0;	// При  0 тут возможна установка параметров
	G2D_TOP->RCQ_HEADER_LOW_ADDR = ptr_lo32(buff);
	G2D_TOP->RCQ_HEADER_HIGH_ADDR = ptr_hi32(buff);
	G2D_TOP->RCQ_HEADER_LEN = len;

	ASSERT(G2D_TOP->RCQ_HEADER_LOW_ADDR == ptr_lo32(buff));
	ASSERT(G2D_TOP->RCQ_HEADER_HIGH_ADDR == ptr_hi32(buff));
	ASSERT(G2D_TOP->RCQ_HEADER_LEN == len);
}


static LCLSPINLOCK_t rtmxlock;

static void g2d_rtmx_accure(void)
{
	LCLSPIN_LOCK(& rtmxlock);
}
static void g2d_rtmx_release(void)
{
	LCLSPIN_UNLOCK(& rtmxlock);
}

/* Запуск и ожидание завершения работы G2D */
/* 0 - timeout. 1 - OK */
static int hwacc_rtmx_waitdone(void)
{
	unsigned n = 0x2000000;
	for (;;)
	{
		const uint_fast32_t MASK = (UINT32_C(1) << 0);	/* FINISH_IRQ */
		const uint_fast32_t mixer_int = G2D_MIXER->G2D_MIXER_INTERRUPT;
		//const uint_fast32_t rot_int = G2D_ROT->ROT_INT;
		if (((mixer_int & MASK) != 0))
		{
			G2D_MIXER->G2D_MIXER_INTERRUPT = MASK;
			break;
		}
//		if (((rot_int & MASK) != 0))
//		{
//			G2D_ROT->ROT_INT = MASK;
//			break;
//		}
		if (-- n == 0)
		{
			PRINTF("G2D_MIXER->G2D_MIXER_CTRL=%08X, G2D_MIXER->G2D_MIXER_INTERRUPT=%08X\n", (unsigned) G2D_MIXER->G2D_MIXER_CTRL, (unsigned) G2D_MIXER->G2D_MIXER_INTERRUPT);
			//PRINTF("G2D_ROT->ROT_CTL=%08X, G2D_ROT->ROT_INT=%08X\n", (unsigned) G2D_ROT->ROT_CTL, (unsigned) G2D_ROT->ROT_INT);
			return 0;
		}
	}
	return 1;
}

/* Запускаем и ждём завершения обработки */
static void awxx_g2d_rtmix_startandwait(void)
{
	G2D_MIXER->G2D_MIXER_CTRL |= (UINT32_C(1) << 31);	/* start the module */
	if (hwacc_rtmx_waitdone() == 0)
	{
		PRINTF("awxx_g2d_rtmix_startandwait: timeout G2D_MIXER->G2D_MIXER_CTRL=%08X\n", (unsigned) G2D_MIXER->G2D_MIXER_CTRL);
		ASSERT(0);
	}
	ASSERT((G2D_MIXER->G2D_MIXER_CTRL & (UINT32_C(1) << 31)) == 0);
}


static void
awg2d_bitblt(unsigned keyflag, COLORPIP_T keycolor,
		unsigned srcFormat, unsigned sstride,
		uint_fast32_t ssizehw, uintptr_t saddr,
		unsigned tstride, uint_fast32_t tsizehw,
		uintptr_t taddr
		)
{
	//	memset(G2D_V0, 0, sizeof * G2D_V0);
	//	memset(G2D_UI0, 0, sizeof * G2D_UI0);
	//	memset(G2D_UI1, 0, sizeof * G2D_UI1);
	//	memset(G2D_UI2, 0, sizeof * G2D_UI2);
	//	memset(G2D_BLD, 0, sizeof * G2D_BLD);
	//	memset(G2D_WB, 0, sizeof * G2D_WB);
	//	G2D_TOP->G2D_AHB_RST &= ~ ((UINT32_C(1) << 1) | (UINT32_C(1) << 0));	// Assert reset: 0x02: rot, 0x01: mixer
	//	G2D_TOP->G2D_AHB_RST |= (UINT32_C(1) << 1) | (UINT32_C(1) << 0);	// De-assert reset: 0x02: rot, 0x01: mixer
	g2d_rtmx_accure();
	ASSERT((G2D_MIXER->G2D_MIXER_CTRL & (UINT32_C(1) << 31)) == 0);
	awxx_g2d_mixer_reset(); /* Отключаем все источники */
	if ((keyflag & BITBLT_FLAG_CKEY) != 0)
	{
		const COLOR24_T keycolor24 = awxx_key_color_conversion(keycolor);
		/* 5.10.9.10 BLD color key control register */
		//G2D_BLD->BLD_KEY_CTL = 0x03;	/* G2D_CK_SRC = 0x03, G2D_CK_DST = 0x01 */
		G2D_BLD->BLD_KEY_CTL = (0x01u << 1) |// KEY0_MATCH_DIR 1: when the pixel value matches source image, it displays the pixel form destination image.
				(UINT32_C(1) << 0) |// KEY0_EN 1: enable color key in Alpha Blender0.
				0;
		/* 5.10.9.11 BLD color key configuration register */
		G2D_BLD->BLD_KEY_CON = 0 * (UINT32_C(1) << 2) |// KEY0R_MATCH 0: match color if value inside keys range
				0 * (UINT32_C(1) << 1) |// KEY0G_MATCH 0: match color if value inside keys range
				0 * (UINT32_C(1) << 0) |// KEY0B_MATCH 0: match color if value inside keys range
				0;
		G2D_BLD->BLD_KEY_MAX = keycolor24;
		G2D_BLD->BLD_KEY_MIN = keycolor24;
		/* установка поверхности - источника (анализируется) */
		G2D_UI2->UI_ATTR = awxx_g2d_get_ui_attr(srcFormat);
		G2D_UI2->UI_PITCH = sstride;
		G2D_UI2->UI_FILLC = 0;
		G2D_UI2->UI_COOR = 0; // координаты куда класть. Фон заполняенся цветом BLD_BK_COLOR
		G2D_UI2->UI_MBSIZE = ssizehw; // сколько брать от исходного буфера
		G2D_UI2->UI_SIZE = ssizehw; // параметры окна исходного буфера
		G2D_UI2->UI_LADD = ptr_lo32(saddr);
		G2D_UI2->UI_HADD = ptr_hi32(saddr);
		/* эта поверхность источник данных когда есть совпадение с ключевым цветом */
		G2D_V0->V0_ATTCTL = awxx_g2d_get_vi_attr(VI_ImageFormat);
		G2D_V0->V0_PITCH0 = tstride;
		G2D_V0->V0_FILLC = 0;
		G2D_V0->V0_COOR = 0; // координаты куда класть. Фон заполняенся цветом BLD_BK_COLOR
		G2D_V0->V0_MBSIZE = tsizehw; // сколько брать от исходного буфера
		G2D_V0->V0_SIZE = tsizehw; // параметры окна исходного буфера
		G2D_V0->V0_LADD0 = ptr_lo32(taddr);
		G2D_V0->V0_HADD = (ptr_hi32(taddr) & 0xFF) << 0;
		G2D_BLD->BLD_SIZE = tsizehw; // размер выходного буфера после scaler
		/* источник когда есть совпадние */
		G2D_BLD->BLD_CH_ISIZE[0] = tsizehw;
		G2D_BLD->BLD_CH_OFFSET[0] = 0; // ((row) << 16) | ((col) << 0);
		/* источник для анализа */
		G2D_BLD->BLD_CH_ISIZE[1] = tsizehw;
		G2D_BLD->BLD_CH_OFFSET[1] = 0; // ((row) << 16) | ((col) << 0);
		G2D_BLD->BLD_FILL_COLOR_CTL = (UINT32_C(1) << 8) |	// 8: P0_EN Pipe0 enable
				(UINT32_C(1) << 9) |	// 9: P1_EN Pipe1 enable
				0;
		G2D_BLD->ROP_CTL = 0x00F0; // 0x00F0 G2D_V0, 0x55F0 UI1, 0xAAF0 UI2
		G2D_BLD->ROP_INDEX[0] = 0; // ? зависят от ROP_CTL
		G2D_BLD->ROP_INDEX[1] = 0;
		G2D_BLD->BLD_CTL = awxx_bld_ctl2(3, 1); //awxx_bld_ctl(3, 1, 3, 1); //0x03010301;	// G2D_BLD_SRCOVER - default value
	}
	else
	{
		/* без keycolor */
		/* установка поверхности - источника (безусловно) */
		//		G2D_UI2->UI_ATTR = awxx_g2d_get_ui_attr();
		//		G2D_UI2->UI_PITCH = sstride;
		//		G2D_UI2->UI_FILLC = 0;
		//		G2D_UI2->UI_COOR = 0;			// координаты куда класть. Фон заполняенся цветом BLD_BK_COLOR
		//		G2D_UI2->UI_MBSIZE = ssizehw; // сколько брать от исходного буфера
		//		G2D_UI2->UI_SIZE = ssizehw;		// параметры окна исходного буфера
		//		G2D_UI2->UI_LADD = ptr_lo32(saddr);
		//		G2D_UI2->UI_HADD = ptr_hi32(saddr);
		G2D_V0->V0_ATTCTL = awxx_g2d_get_vi_attr(srcFormat);
		G2D_V0->V0_PITCH0 = sstride;
		G2D_V0->V0_FILLC = 0; //TFTRGB(255, 0, 0);    // unused
		G2D_V0->V0_COOR = 0; // координаты куда класть. Фон заполняенся цветом BLD_BK_COLOR
		G2D_V0->V0_MBSIZE = ssizehw; // сколько брать от исходного буфера
		G2D_V0->V0_SIZE = ssizehw; // параметры окна исходного буфера
		G2D_V0->V0_LADD0 = ptr_lo32(saddr);
		G2D_V0->V0_HADD = ptr_hi32(saddr);
		G2D_BLD->BLD_SIZE = tsizehw; // размер выходного буфера после scaler
		G2D_BLD->BLD_CH_ISIZE[0] = ssizehw;
		G2D_BLD->BLD_CH_OFFSET[0] = 0; // ((row) << 16) | ((col) << 0);
		G2D_BLD->BLD_CH_ISIZE[1] = ssizehw;
		G2D_BLD->BLD_CH_OFFSET[1] = 0; // ((row) << 16) | ((col) << 0);
		G2D_BLD->BLD_KEY_CTL = 0;
		G2D_BLD->BLD_KEY_CON = 0;
		G2D_BLD->BLD_FILL_COLOR_CTL = (UINT32_C(1) << 8) |	// 8: P0_EN Pipe0 enable - VI0
				//(UINT32_C(1) << 9) |	// 9: P1_EN Pipe1 enable - UI2
				0;
		G2D_BLD->ROP_CTL = 0x00F0; // 0x00F0 G2D_V0, 0x55F0 UI1, 0xAAF0 UI2
		G2D_BLD->ROP_INDEX[0] = 0; // ? зависят от ROP_CTL
		G2D_BLD->ROP_INDEX[1] = 0;
		//G2D_BLD->BLD_CTL = 0x00010001;	// G2D_BLD_COPY
		//G2D_BLD->BLD_CTL = 0x00000000;	// G2D_BLD_CLEAR
		G2D_BLD->BLD_CTL = awxx_bld_ctl2(3, 1); //awxx_bld_ctl(3, 1, 3, 1); //0x03010301;	// G2D_BLD_SRCOVER - default value
	}
	//G2D_BLD->BLD_FILLC0 = ~ 0;
	//G2D_BLD->BLD_PREMUL_CTL |= (UINT32_C(1) << 0);	// 0 or 1 - sel 1 or sel 0
	G2D_BLD->BLD_OUT_COLOR = 0x00000000; /* 0x00000000 */
	G2D_BLD->BLD_CSC_CTL = 0x00000000; /* 0x00000000 */
	G2D_BLD->BLD_BK_COLOR = 0;
	G2D_BLD->BLD_PREMUL_CTL = 0x00000000; /* 0x00000000 */
	/* Write-back settings */
	G2D_WB->WB_ATT = WB_ImageFormat;
	G2D_WB->WB_SIZE = tsizehw;
	G2D_WB->WB_PITCH0 = tstride; /* taddr buffer stride */
	G2D_WB->WB_LADD0 = ptr_lo32(taddr);
	G2D_WB->WB_HADD0 = ptr_hi32(taddr);
	awxx_g2d_rtmix_startandwait(); /* Запускаем и ждём завершения обработки */
	g2d_rtmx_release();
}

#endif /* defined (G2D_MIXER) */

#if defined (G2D_ROT)

/* Запуск и ожидание завершения работы G2D  (ROT) */
/* 0 - timeout. 1 - OK */
static int hwacc_rot_waitdone(void)
{
	unsigned n = 0x2000000;
	for (;;)
	{
		const uint_fast32_t MASK = (UINT32_C(1) << 0);	/* FINISH_IRQ */
		//const uint_fast32_t mixer_int = G2D_MIXER->G2D_MIXER_INTERRUPT;
		const uint_fast32_t rot_int = G2D_ROT->ROT_INT;
//		if (((mixer_int & MASK) != 0))
//		{
//			G2D_MIXER->G2D_MIXER_INTERRUPT = MASK;
//			break;
//		}
		if (((rot_int & MASK) != 0))
		{
			G2D_ROT->ROT_INT = MASK;
			break;
		}
		if (-- n == 0)
		{
			//PRINTF("G2D_MIXER->G2D_MIXER_CTRL=%08X, G2D_MIXER->G2D_MIXER_INTERRUPT=%08X\n", (unsigned) G2D_MIXER->G2D_MIXER_CTRL, (unsigned) G2D_MIXER->G2D_MIXER_INTERRUPT);
			PRINTF("G2D_ROT->ROT_CTL=%08X, G2D_ROT->ROT_INT=%08X\n", (unsigned) G2D_ROT->ROT_CTL, (unsigned) G2D_ROT->ROT_INT);
			return 0;
		}
	}
	return 1;
}

static void awxx_g2d_rot_startandwait(void)
{
	G2D_ROT->ROT_CTL |= (UINT32_C(1) << 31);	// start
	if (hwacc_rot_waitdone() == 0)
	{
		PRINTF("awxx_g2d_rot_startandwait: timeout G2D_ROT->ROT_CTL=%08X\n", (unsigned) G2D_ROT->ROT_CTL);
		ASSERT(0);
	}
	ASSERT((G2D_ROT->ROT_CTL & (UINT32_C(1) << 31)) == 0);
}

#endif /* defined (G2D_ROT) */

static LCLSPINLOCK_t rotlock;

static void g2d_rot_accure(void)
{
	LCLSPIN_LOCK(& rotlock);
}
static void g2d_rot_release(void)
{
	LCLSPIN_UNLOCK(& rotlock);
}

/* Коприрование с применением блока G2D_ROT */
static void
hwaccel_rotcopy(
	uintptr_t saddr,
	unsigned sstride,
	uint_fast32_t ssizehw,
	uintptr_t taddr,
	unsigned tstride,
	uint_fast32_t tsizehw,
	uint_fast32_t rot_ctl
	)
{
	g2d_rot_accure();
	ASSERT((G2D_ROT->ROT_CTL & (UINT32_C(1) << 31)) == 0);

	G2D_ROT->ROT_CTL = 0;
	G2D_ROT->ROT_IFMT = ROT_ImageFormat;

	G2D_ROT->ROT_ISIZE = ssizehw;
	G2D_ROT->ROT_IPITCH0 = sstride;	// Y/RGB/ARGB data memory. Should be 128bit aligned.
//	G2D_ROT->ROT_IPITCH1 = sstride;	// U/UV data memory
//	G2D_ROT->ROT_IPITCH2 = sstride; // V data memory
	G2D_ROT->ROT_ILADD0 = ptr_lo32(saddr); // Should be 128bit aligned.
	G2D_ROT->ROT_IHADD0 = ptr_hi32(saddr) & 0xff;
//	G2D_ROT->ROT_ILADD1 = ptr_lo32(saddr);
//	G2D_ROT->ROT_IHADD1 = ptr_hi32(saddr) & 0xff;
//	G2D_ROT->ROT_ILADD2 = ptr_lo32(saddr);
//	G2D_ROT->ROT_IHADD2 = ptr_hi32(saddr) & 0xff;

	G2D_ROT->ROT_OPITCH0 = tstride;	// Should be 128bit aligned.
//	G2D_ROT->ROT_OPITCH1 = tstride;
//	G2D_ROT->ROT_OPITCH2 = tstride;
	G2D_ROT->ROT_OSIZE = tsizehw;
	G2D_ROT->ROT_OLADD0 = ptr_lo32(taddr);	// Alignment not required
	G2D_ROT->ROT_OHADD0 = ptr_hi32(taddr) & 0xff;
//	G2D_ROT->ROT_OLADD1 = ptr_lo32(taddr);
//	G2D_ROT->ROT_OHADD1 = ptr_hi32(taddr) & 0xff;
//	G2D_ROT->ROT_OLADD2 = ptr_lo32(taddr);
//	G2D_ROT->ROT_OHADD2 = ptr_hi32(taddr) & 0xff;

	//G2D_ROT->ROT_CTL |= (UINT32_C(1) << 7);	// flip horizontal
	//G2D_ROT->ROT_CTL |= (UINT32_C(1) << 6);	// flip vertical
	//G2D_ROT->ROT_CTL |= (UINT32_C(1) << 4);	// rotate (0: 0deg, 1: 90deg, 2: 180deg, 3: 270deg) CW
	G2D_ROT->ROT_CTL = rot_ctl;
	G2D_ROT->ROT_CTL |= (UINT32_C(1) << 0);		// ENABLE
	awxx_g2d_rot_startandwait();		/* Запускаем и ждём завершения обработки */
	g2d_rot_release();

}

#if ! defined (G2D_MIXER)

static COLORPIP_T bgcolor;
static PACKEDCOLORPIP_T bgscreen [GXSIZE(DIM_X, DIM_Y)];

/* Заполнение цветом фона - будет использоваться для аппаратной оптимизации заполнения копированием */
static void aw_g2d_initialize(void)
{
	bgcolor = display2_getbgcolor();
	// программная реализация
	unsigned i;
	for (i = 0; i < ARRAY_SIZE(bgscreen); ++ i)
	{
		bgscreen [i] = bgcolor;
	}
	dcache_clean_invalidate((uintptr_t) bgscreen, sizeof bgscreen);
	//PRINTF("aw_g2d_initialize: bgcolor=%08X\n", (unsigned) bgcolor);
}


static void hwaccel_fillrect(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,	// ширина буфера
	uintptr_t taddr,
	uint_fast32_t tstride,
	uint_fast32_t tsizehw,
	unsigned alpha_unused,
	COLOR24_T color24_unused,
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	COLORPIP_T color,	// цвет
	unsigned fillmask
	)
{
	enum { wf = 64 };
	enum { hf = 64 };
	if (0)
	{
	}
//	else if (w >= wf && h >= hf)
//	{
//		static RAMNC PACKEDCOLORPIP_T colorscreen [GXSIZE(wf, hf)];
//		unsigned i;
//		for (i = 0; i < ARRAY_SIZE(colorscreen); ++ i)
//			colorscreen [i] = color;
//
//		const uint_fast32_t ssizehw = ((hf - 1) << 16) | ((wf - 1) << 0);
//		// valid rot_ctl bits 400001F2
//		const uint_fast32_t flag = (UINT32_C(1) << 8) | 0*(UINT32_C(1) << 1);
//		hwaccel_rotcopy((uintptr_t) colorscreen, GXADJ(hf) * sizeof (PACKEDCOLORPIP_T), ssizehw, taddr, tstride, tsizehw, flag);
//	}
	else if (w > 1 && h > 1 && color == bgcolor)
	{
		//const uint_fast32_t ssizehw = ((DIM_Y - 1) << 16) | ((DIM_X - 1) << 0);	// вызывает странные записи в память если ширниа одинаковая а высота получателя меньше.
		const uint_fast32_t ssizehw = tsizehw;
		hwaccel_rotcopy((uintptr_t) bgscreen, GXADJ(DIM_X) * sizeof (PACKEDCOLORPIP_T), ssizehw, taddr, tstride, tsizehw, 0);
	}
	else
	{
		softfill(buffer, dx, w, h, color);	// программная реализация
	}
}

#else


#define VSU_ZOOM0_SIZE	1
#define VSU_ZOOM1_SIZE	8
#define VSU_ZOOM2_SIZE	4
#define VSU_ZOOM3_SIZE	1
#define VSU_ZOOM4_SIZE	1
#define VSU_ZOOM5_SIZE	1

// Note: VPHASE1 is a register with 1 bit signed, 3 bit integer and 18 bit fraction. Valid value ranges from -0x3FFFF .. 0x3FFFF

#define VSU_PHASE_NUM            32
#define VSU_PHASE_FRAC_BITWIDTH  19
#define VSU_PHASE_FRAC_REG_SHIFT 1
#define VSU_FB_FRAC_BITWIDTH     32

static void vsu_fir_linear(volatile uint32_t * p)
{
	unsigned i;
	const unsigned N = 64;
	// taken from linearcoefftab32
	static const uint32_t values [] =
	{
		0x00004000, 0x00023E00, 0x00043C00, 0x00063A00,
		0x00083800, 0x000A3600, 0x000C3400, 0x000E3200,
		0x00103000, 0x00122E00, 0x00142C00, 0x00162A00,
		0x00182800, 0x001A2600, 0x001C2400, 0x001E2200,
		0x00202000, 0x00221E00, 0x00241C00, 0x00261A00,
		0x00281800, 0x002A1600, 0x002C1400, 0x002E1200,
		0x00301000, 0x00320E00, 0x00340C00, 0x00360A00,
		0x00380800, 0x003A0600, 0x003C0400, 0x003E0200,
	};
	for (i = 0; i < ARRAY_SIZE(values) && i < N; ++ i)
	{
		p [i] = values [i];
	}
	for (; i < N; ++ i)
	{
		p [i] = 0;
	}
}
/**
 * function       : g2d_vsu_calc_fir_coef(unsigned int step)
 * description    : set fir coefficients
 * parameters     :
 *                  step		<horizontal scale ratio of vsu>
 * return         :
 *                  offset (in word) of coefficient table
 */
static uint32_t g2d_vsu_calc_fir_coef(uint32_t step)
{
	uint32_t pt_coef;
	uint32_t scale_ratio, int_part, float_part, fir_coef_ofst;

	scale_ratio = step >> (VSU_PHASE_FRAC_BITWIDTH - 3);
	int_part = scale_ratio >> 3;
	float_part = scale_ratio & 0x7;
	fir_coef_ofst = (int_part == 0) ? VSU_ZOOM0_SIZE :
	    (int_part == 1) ? VSU_ZOOM0_SIZE + float_part :
	    (int_part ==
	     2) ? VSU_ZOOM0_SIZE + VSU_ZOOM1_SIZE +
	    (float_part >> 1) : (int_part ==
				  3) ? VSU_ZOOM0_SIZE + VSU_ZOOM1_SIZE +
	    VSU_ZOOM2_SIZE : (int_part ==
			       4) ? VSU_ZOOM0_SIZE + VSU_ZOOM1_SIZE +
	    VSU_ZOOM2_SIZE + VSU_ZOOM3_SIZE : VSU_ZOOM0_SIZE +
	    VSU_ZOOM1_SIZE + VSU_ZOOM2_SIZE + VSU_ZOOM3_SIZE + VSU_ZOOM4_SIZE;

	return fir_coef_ofst;
}
static const int32_t lan2coefftab32_full [512] = {
	0x00004000, 0x000140ff, 0x00033ffe, 0x00043ffd, 0x00063efc, 0xff083dfc,
	0x000a3bfb, 0xff0d39fb, 0xff0f37fb, 0xff1136fa, 0xfe1433fb,
	0xfe1631fb, 0xfd192ffb, 0xfd1c2cfb, 0xfd1f29fb, 0xfc2127fc,
	0xfc2424fc, 0xfc2721fc, 0xfb291ffd, 0xfb2c1cfd, 0xfb2f19fd,
	0xfb3116fe, 0xfb3314fe, 0xfa3611ff, 0xfb370fff, 0xfb390dff,
	0xfb3b0a00, 0xfc3d08ff, 0xfc3e0600, 0xfd3f0400, 0xfe3f0300,
	0xff400100,
	/* counter = 1 */
	0x00004000, 0x000140ff, 0x00033ffe, 0x00043ffd, 0x00063efc,
	0xff083dfc, 0x000a3bfb, 0xff0d39fb, 0xff0f37fb, 0xff1136fa,
	0xfe1433fb, 0xfe1631fb, 0xfd192ffb, 0xfd1c2cfb, 0xfd1f29fb,
	0xfc2127fc, 0xfc2424fc, 0xfc2721fc, 0xfb291ffd, 0xfb2c1cfd,
	0xfb2f19fd, 0xfb3116fe, 0xfb3314fe, 0xfa3611ff, 0xfb370fff,
	0xfb390dff, 0xfb3b0a00, 0xfc3d08ff, 0xfc3e0600, 0xfd3f0400,
	0xfe3f0300, 0xff400100,
	/* counter = 2 */
	0xff053804, 0xff063803, 0xff083801, 0xff093701, 0xff0a3700,
	0xff0c3500, 0xff0e34ff, 0xff1033fe, 0xff1232fd, 0xfe1431fd,
	0xfe162ffd, 0xfe182dfd, 0xfd1b2cfc, 0xfd1d2afc, 0xfd1f28fc,
	0xfd2126fc, 0xfd2323fd, 0xfc2621fd, 0xfc281ffd, 0xfc2a1dfd,
	0xfc2c1bfd, 0xfd2d18fe, 0xfd2f16fe, 0xfd3114fe, 0xfd3212ff,
	0xfe3310ff, 0xff340eff, 0x00350cff, 0x00360a00, 0x01360900,
	0x02370700, 0x03370600,
	/* counter = 3 */
	0xff083207, 0xff093206, 0xff0a3205, 0xff0c3203, 0xff0d3103,
	0xff0e3102, 0xfe113001, 0xfe132f00, 0xfe142e00, 0xfe162dff,
	0xfe182bff, 0xfe192aff, 0xfe1b29fe, 0xfe1d27fe, 0xfe1f25fe,
	0xfd2124fe, 0xfe2222fe, 0xfe2421fd, 0xfe251ffe, 0xfe271dfe,
	0xfe291bfe, 0xff2a19fe, 0xff2b18fe, 0xff2d16fe, 0x002e14fe,
	0x002f12ff, 0x013010ff, 0x02300fff, 0x03310dff, 0x04310cff,
	0x05310a00, 0x06310900,
	/* counter = 4 */
	0xff0a2e09, 0xff0b2e08, 0xff0c2e07, 0xff0e2d06, 0xff0f2d05,
	0xff102d04, 0xff122c03, 0xfe142c02, 0xfe152b02, 0xfe172a01,
	0xfe182901, 0xfe1a2800, 0xfe1b2700, 0xfe1d2500, 0xff1e24ff,
	0xfe2023ff, 0xff2121ff, 0xff2320fe, 0xff241eff, 0x00251dfe,
	0x00261bff, 0x00281afe, 0x012818ff, 0x012a16ff, 0x022a15ff,
	0x032b13ff, 0x032c12ff, 0x052c10ff, 0x052d0fff, 0x062d0d00,
	0x072d0c00, 0x082d0b00,
	/* counter = 5 */
	0xff0c2a0b, 0xff0d2a0a, 0xff0e2a09, 0xff0f2a08, 0xff102a07,
	0xff112a06, 0xff132905, 0xff142904, 0xff162803, 0xff172703,
	0xff182702, 0xff1a2601, 0xff1b2501, 0xff1c2401, 0xff1e2300,
	0xff1f2200, 0x00202000, 0x00211f00, 0x01221d00, 0x01231c00,
	0x01251bff, 0x02251aff, 0x032618ff, 0x032717ff, 0x042815ff,
	0x052814ff, 0x052913ff, 0x06291100, 0x072a10ff, 0x082a0e00,
	0x092a0d00, 0x0a2a0c00,
	/* counter = 6 */
	0xff0d280c, 0xff0e280b, 0xff0f280a, 0xff102809, 0xff112808,
	0xff122708, 0xff142706, 0xff152705, 0xff162605, 0xff172604,
	0xff192503, 0xff1a2403, 0x001b2302, 0x001c2202, 0x001d2201,
	0x001e2101, 0x011f1f01, 0x01211e00, 0x01221d00, 0x02221c00,
	0x02231b00, 0x03241900, 0x04241800, 0x04251700, 0x052616ff,
	0x06261400, 0x072713ff, 0x08271100, 0x08271100, 0x09271000,
	0x0a280e00, 0x0b280d00,
	/* counter = 7 */
	0xff0e260d, 0xff0f260c, 0xff10260b, 0xff11260a, 0xff122609,
	0xff132608, 0xff142508, 0xff152507, 0x00152506, 0x00172405,
	0x00182305, 0x00192304, 0x001b2203, 0x001c2103, 0x011d2002,
	0x011d2002, 0x011f1f01, 0x021f1e01, 0x02201d01, 0x03211c00,
	0x03221b00, 0x04221a00, 0x04231801, 0x05241700, 0x06241600,
	0x07241500, 0x08251300, 0x09251200, 0x09261100, 0x0a261000,
	0x0b260f00, 0x0c260e00,
	/* counter = 8 */
	0xff0e250e, 0xff0f250d, 0xff10250c, 0xff11250b, 0x0011250a,
	0x00132409, 0x00142408, 0x00152407, 0x00162307, 0x00172306,
	0x00182206, 0x00192205, 0x011a2104, 0x011b2004, 0x011c2003,
	0x021c1f03, 0x021e1e02, 0x031e1d02, 0x03201c01, 0x04201b01,
	0x04211a01, 0x05221900, 0x05221801, 0x06231700, 0x07231600,
	0x07241500, 0x08241400, 0x09241300, 0x0a241200, 0x0b241100,
	0x0c241000, 0x0d240f00,
	/* counter = 9 */
	0x000e240e, 0x000f240d, 0x0010240c, 0x0011240b, 0x0013230a,
	0x0013230a, 0x00142309, 0x00152308, 0x00162208, 0x00172207,
	0x01182106, 0x01192105, 0x011a2005, 0x021b1f04, 0x021b1f04,
	0x021d1e03, 0x031d1d03, 0x031e1d02, 0x041e1c02, 0x041f1b02,
	0x05201a01, 0x05211901, 0x06211801, 0x07221700, 0x07221601,
	0x08231500, 0x09231400, 0x0a231300, 0x0a231300, 0x0b231200,
	0x0c231100, 0x0d231000,
	/* counter = 10 */
	0x000f220f, 0x0010220e, 0x0011220d, 0x0012220c, 0x0013220b,
	0x0013220b, 0x0015210a, 0x0015210a, 0x01162108, 0x01172008,
	0x01182007, 0x02191f06, 0x02191f06, 0x021a1e06, 0x031a1e05,
	0x031c1d04, 0x041c1c04, 0x041d1c03, 0x051d1b03, 0x051e1a03,
	0x061f1902, 0x061f1902, 0x07201801, 0x08201701, 0x08211601,
	0x09211501, 0x0a211500, 0x0b211400, 0x0b221300, 0x0c221200,
	0x0d221100, 0x0e221000,
	/* counter = 11 */
	0x0010210f, 0x0011210e, 0x0011210e, 0x0012210d, 0x0013210c,
	0x0014200c, 0x0114200b, 0x0115200a, 0x01161f0a, 0x01171f09,
	0x02171f08, 0x02181e08, 0x03181e07, 0x031a1d06, 0x031a1d06,
	0x041b1c05, 0x041c1c04, 0x051c1b04, 0x051d1a04, 0x061d1a03,
	0x071d1903, 0x071e1803, 0x081e1802, 0x081f1702, 0x091f1602,
	0x0a201501, 0x0b1f1501, 0x0b201401, 0x0c211300, 0x0d211200,
	0x0e201200, 0x0e211100,
	/* counter = 12 */
	0x00102010, 0x0011200f, 0x0012200e, 0x0013200d, 0x0013200d,
	0x01141f0c, 0x01151f0b, 0x01151f0b, 0x01161f0a, 0x02171e09,
	0x02171e09, 0x03181d08, 0x03191d07, 0x03191d07, 0x041a1c06,
	0x041b1c05, 0x051b1b05, 0x051c1b04, 0x061c1a04, 0x071d1903,
	0x071d1903, 0x081d1803, 0x081e1703, 0x091e1702, 0x0a1f1601,
	0x0a1f1502, 0x0b1f1501, 0x0c1f1401, 0x0d201300, 0x0d201300,
	0x0e201200, 0x0f201100,
	/* counter = 13 */
	0x00102010, 0x0011200f, 0x00121f0f, 0x00131f0e, 0x00141f0d,
	0x01141f0c, 0x01141f0c, 0x01151e0c, 0x02161e0a, 0x02171e09,
	0x03171d09, 0x03181d08, 0x03181d08, 0x04191c07, 0x041a1c06,
	0x051a1b06, 0x051b1b05, 0x061b1a05, 0x061c1a04, 0x071c1904,
	0x081c1903, 0x081d1803, 0x091d1703, 0x091e1702, 0x0a1e1602,
	0x0b1e1502, 0x0c1e1501, 0x0c1f1401, 0x0d1f1400, 0x0e1f1300,
	0x0e1f1201, 0x0f1f1200,
	/* counter = 14 */
	0x00111e11, 0x00121e10, 0x00131e0f, 0x00131e0f, 0x01131e0e,
	0x01141d0e, 0x02151d0c, 0x02151d0c, 0x02161d0b, 0x03161c0b,
	0x03171c0a, 0x04171c09, 0x04181b09, 0x05181b08, 0x05191b07,
	0x06191a07, 0x061a1a06, 0x071a1906, 0x071b1905, 0x081b1805,
	0x091b1804, 0x091c1704, 0x0a1c1703, 0x0a1c1604, 0x0b1d1602,
	0x0c1d1502, 0x0c1d1502, 0x0d1d1402, 0x0e1d1401, 0x0e1e1301,
	0x0f1e1300, 0x101e1200,
	/* counter = 15 */
	0x00111e11, 0x00121e10, 0x00131d10, 0x01131d0f, 0x01141d0e,
	0x01141d0e, 0x02151c0d, 0x02151c0d, 0x03161c0b, 0x03161c0b,
	0x04171b0a, 0x04171b0a, 0x05171b09, 0x05181a09, 0x06181a08,
	0x06191a07, 0x07191907, 0x071a1906, 0x081a1806, 0x081a1806,
	0x091a1805, 0x0a1b1704, 0x0a1b1704, 0x0b1c1603, 0x0b1c1603,
	0x0c1c1503, 0x0d1c1502, 0x0d1d1402, 0x0e1d1401, 0x0f1d1301,
	0x0f1d1301, 0x101e1200,
	/* counter = 16 */
};

static void vsu_fir_bytable(volatile uint32_t * p, unsigned offset)
{
	unsigned i;
	const unsigned N = 64;
	for (i = 0; i < 32 && i < N; ++ i)
	{
		p [i] = lan2coefftab32_full [32 * offset + i];
	}
	for (; i < N; ++ i)
	{
		p [i] = 0;
	}
}

// https://github.com/DongshanPI/D1s-Melis/blob/b289fdae3e6245c3f185259903c91e7db204cfb5/ekernel/drivers/hal/source/g2d_rcq/g2d_mixer_type.h#L375
// ROP_INDEX structure:
//	union g2d_mixer_rop_ch3_index0 {
//		unsigned int dwval;
//		struct {
//			unsigned int index0node0:3;
//			unsigned int index0node1:1;
//			unsigned int index0node2:1;
//			unsigned int index0node3:1;
//			unsigned int index0node4:4;
//			unsigned int index0node5:1;
//			unsigned int index0node6:4;
//			unsigned int index0node7:1;
//			unsigned int ch0ign_en:1;
//			unsigned int ch1ign_en:1;
//			unsigned int ch2ign_en:1;
//			unsigned int res0:13;
//		} bits;
//	};

static void hwaccel_fillrect(
	PACKEDCOLORPIP_T * __restrict buffer_UNUSED,
	uint_fast16_t dx_UNUSED,	// ширина буфера
	uintptr_t taddr,
	uint_fast32_t tstride,
	uint_fast32_t tsizehw,
	unsigned alpha,
	COLOR24_T color24,
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	COLORPIP_T pipecolor,	// цвет
	unsigned fillmask
	)
{

	g2d_rtmx_accure();
	awxx_g2d_mixer_reset();	/* Отключаем все источники */
	ASSERT((G2D_MIXER->G2D_MIXER_CTRL & (UINT32_C(1) << 31)) == 0);

	if (fillmask & FILL_FLAG_MIXBG)
	{
		/* установка поверхности - источника (анализируется) */
		G2D_UI2->UI_ATTR = awxx_g2d_get_ui_attr(VI_ImageFormat);
		G2D_UI2->UI_PITCH = tstride;
		G2D_UI2->UI_FILLC = 0;
		G2D_UI2->UI_COOR = 0;			// координаты куда класть. Фон заполняенся цветом BLD_BK_COLOR
		G2D_UI2->UI_MBSIZE = tsizehw; // сколько брать от исходного буфера
		G2D_UI2->UI_SIZE = tsizehw;		// параметры окна исходного буфера
		G2D_UI2->UI_LADD = ptr_lo32(taddr);
		G2D_UI2->UI_HADD = ptr_hi32(taddr);

		G2D_BLD->BLD_FILL_COLOR [0] = (alpha * (UINT32_C(1) << 24)) | (color24 & 0xFFFFFF); // цвет и alpha канал
		G2D_BLD->BLD_SIZE = tsizehw;	// размер выходного буфера
		G2D_BLD->ROP_CTL = 0*0xAAF0;	// 0x00F0 G2D_V0, 0x55F0 UI1, 0xAAF0 UI2
		G2D_BLD->ROP_INDEX [0] = 0;		// ? зависят от ROP_CTL
		G2D_BLD->ROP_INDEX [1] = 0;
		G2D_BLD->BLD_PREMUL_CTL=0*0x00000001; /* 0x00000001 */
		G2D_BLD->BLD_OUT_COLOR=0*0x002; //0*0x00000001; /* 0x00000001 */

		G2D_BLD->BLD_CH_ISIZE [0] = tsizehw;
		G2D_BLD->BLD_CH_OFFSET [0] = 0;// ((row) << 16) | ((col) << 0);
		G2D_BLD->BLD_CH_ISIZE [1] = tsizehw;
		G2D_BLD->BLD_CH_OFFSET [1] = 0;// ((row) << 16) | ((col) << 0);

		G2D_BLD->BLD_FILL_COLOR_CTL =
			(UINT32_C(1) << 9) |    	// P1_EN: Pipe1 enable - old frame buffer
			(UINT32_C(1) << 8) |    	// P0_EN: Pipe0 enable
			(UINT32_C(1) << 0) |		// P0_FCEN: Pipe0 fill color enable
			0;
		G2D_BLD->BLD_CTL = awxx_bld_ctl2(1, 3); // отрисовка полупрозначного прямоугольника уменьшает интенсивность цветов под ним (alpha канал не меняется)

	}
	else
	{
		G2D_BLD->BLD_FILL_COLOR [0] = (alpha * (UINT32_C(1) << 24)) | (color24 & 0xFFFFFF); // цвет и alpha канал
		G2D_BLD->BLD_SIZE = tsizehw;	// размер выходного буфера
		G2D_BLD->ROP_CTL = 0*0x00F0;	// 0x00F0 G2D_V0, 0x55F0 UI1, 0xAAF0 UI2
		G2D_BLD->ROP_INDEX [0] = 0;		// ? зависят от ROP_CTL
		G2D_BLD->ROP_INDEX [1] = 0;
		G2D_BLD->BLD_PREMUL_CTL=0*0x00000001; /* 0x00000001 */
		G2D_BLD->BLD_OUT_COLOR=0*0x002; //0*0x00000001; /* 0x00000001 */

		G2D_BLD->BLD_CH_ISIZE [0] = tsizehw;
		G2D_BLD->BLD_CH_OFFSET [0] = 0;// ((row) << 16) | ((col) << 0);

		// BLD_FILL_COLOR_CTL: BLD_FILLC [0] или BLD_BK_COLOR
		G2D_BLD->BLD_FILL_COLOR_CTL =
			(UINT32_C(1) << 8) |    	// P0_EN: Pipe0 enable
			(UINT32_C(1) << 0) |		// P0_FCEN: Pipe0 fill color enable
			0;
		G2D_BLD->BLD_CTL = awxx_bld_ctl2(0, 0); // G2D_BLD_CLEAR

	}

	/* Write-back settings */
	G2D_WB->WB_ATT = WB_ImageFormat;
	G2D_WB->WB_SIZE = tsizehw;
	G2D_WB->WB_PITCH0 = tstride;
	G2D_WB->WB_LADD0 = ptr_lo32(taddr);
	G2D_WB->WB_HADD0 = ptr_hi32(taddr);

	awxx_g2d_rtmix_startandwait();		/* Запускаем и ждём завершения обработки */
	g2d_rtmx_release();
}
#endif

#elif (WITHMDMAHW || WITHDMA2DHW) && (CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)


/*
	Dead time value in the AXI clock cycle inserted between two consecutive accesses on
	the AXI master port. These bits represent the minimum guaranteed number of cycles
	between two consecutive AXI accesses
 */
#define DMA2D_AMTCR_DT_VALUE 255u	/* 0..255 */
#define DMA2D_AMTCR_DT_ENABLE 1u	/* 0..1 */

#define MDMA_CH		MDMA_Channel0
#define MDMA_DATA	(MDMA_Channel1->CSAR)	// регистр выделенного канала MDMA используется для хранения значения цвета. Переиферия не кэшируется.

#ifndef DMA2D_CR_LOM
	#define DMA2D_CR_LOM	(UINT32_C(1) << 6)	/* documented but missing in headers. */
#endif

#define MDMA_CCR_PL_VALUE 0u	// PL: priority 0..3: min..max

#if LCDMODE_LTDC_L24
	#define DMA2D_FGPFCCR_CM_VALUE_MAIN		(1 * DMA2D_FGPFCCR_CM_0)	/* 0001: RGB888 */
	#define MDMA_CTCR_xSIZE_MAIN			0x00	// 1 byte
	//#define DMA2D_OPFCCR_CM_VALUE_MAIN	(1 * DMA2D_OPFCCR_CM_0)	/* 001: RGB888 */

#elif LCDMODE_PALETTE256
	#define DMA2D_FGPFCCR_CM_VALUE_MAIN		(5 * DMA2D_FGPFCCR_CM_0)	/* 0101: L8 */
	#define MDMA_CTCR_xSIZE_MAIN			0x00	// 1 byte
	////#define DMA2D_OPFCCR_CM_VALUE	(x * DMA2D_OPFCCR_CM_0)	/* not supported */

#elif LCDMODE_ARGB8888
	#define DMA2D_FGPFCCR_CM_VALUE_MAIN		(0 * DMA2D_FGPFCCR_CM_0)	/* 0000: ARGB8888 */
	#define MDMA_CTCR_xSIZE_MAIN			0x02	// 10: Word (32-bit)
	#define DMA2D_OPFCCR_CM_VALUE_MAIN		(0 * DMA2D_OPFCCR_CM_0)	/* 0: 000: ARGB8888 */

#elif LCDMODE_RGB565
	#define DMA2D_FGPFCCR_CM_VALUE_MAIN		(2 * DMA2D_FGPFCCR_CM_0)	/* 0010: RGB565 */
	#define MDMA_CTCR_xSIZE_MAIN			0x01	// 2 byte
	#define DMA2D_OPFCCR_CM_VALUE_MAIN		(2 * DMA2D_OPFCCR_CM_0)	/* 010: RGB565 */
#else
	#define DMA2D_FGPFCCR_CM_VALUE_MAIN		(1 * DMA2D_FGPFCCR_CM_0)	/* 0001: RGB888 */
	#define MDMA_CTCR_xSIZE_MAIN			0x00	// 1 byte

#endif /* LCDMODE_PALETTE256 */

#if 1

	#define DMA2D_FGPFCCR_CM_VALUE_PIP	DMA2D_FGPFCCR_CM_VALUE_MAIN
	#define DMA2D_OPFCCR_CM_VALUE_PIP	DMA2D_OPFCCR_CM_VALUE_MAIN
	#define MDMA_CTCR_xSIZE_PIP			MDMA_CTCR_xSIZE_MAIN

#endif /* LCDMODE_PALETTE256 */

//#define DMA2D_FGPFCCR_CM_VALUE_L24	(1 * DMA2D_FGPFCCR_CM_0)	/* 0001: RGB888 */
//#define DMA2D_FGPFCCR_CM_VALUE_L16	(2 * DMA2D_FGPFCCR_CM_0)	/* 0010: RGB565 */
//#define DMA2D_FGPFCCR_CM_VALUE_L8	(5 * DMA2D_FGPFCCR_CM_0)	/* 0101: L8 */

//#define MDMA_CTCR_xSIZE_U32			0x02	// 4 byte
//#define MDMA_CTCR_xSIZE_U16			0x01	// 2 byte
//#define MDMA_CTCR_xSIZE_U8			0x00	// 1 byte
//#define MDMA_CTCR_xSIZE_RGB565		0x01	// 2 byte

#endif /* CPUSTYLE_ALLWINNER && WITHMDMAHW */

#if WITHMDMAHW && (CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)

static uint_fast8_t
mdma_getbus(uintptr_t addr)
{
#if CPUSTYLE_STM32H7XX
	addr &= 0xFF000000uL;
	return (addr == 0x00000000uL || addr == 0x20000000uL);

#elif CPUSTYLE_STM32MP1
	// SYSMEM
	// DDRCTRL
	/*
	 * 0: The system/AXI bus is used on channel x.
	 * 1: The AHB bus/TCM is used on channel x.
	 */
	addr &= 0xFF000000uL;
	return (addr == 0x00000000uL || addr == 0x20000000uL);

#elif CPUSTYLE_STM32MP1 && CORE_CM4
	#error M4 core not supported
	/*
	 * 0: The system/AXI bus is used on channel x.
	 * 1: The AHB bus/TCM is used on channel x.
	 */
	addr &= 0xFF000000uL;
	return (addr == 0x00000000uL || addr == 0x20000000uL);

#else
	return 0;

#endif
}


static uint_fast8_t
mdma_tlen(uint_fast32_t nb, uint_fast8_t ds)
{
	return (nb < 128 ? nb : 128) / ds * ds;
}

//	For the TCM memory accesses, the burst access is only allowed when the increment
//	and data size are identical and lower than or equal to 32-bit.

// DBURST value must be programmed as to ensure that the burst size is lower than the
// Transfer Length. If this is not ensured, the result is unpredictable.

// И еще несколько условий..

static uint_fast8_t
mdma_getburst(uint_fast16_t tlen, uint_fast8_t bus, uint_fast8_t xinc)
{
	if (bus == 0 && xinc == 0)
		return 4;
	if (xinc == 0)
		return 0;

	if (bus != 0)
		return 0;

	if (tlen >= 128)
		return 7;
	if (tlen >= 64)
		return 6;
	if (tlen >= 32)
		return 5;
	if (tlen >= 16)
		return 4;
	if (tlen >= 8)
		return 3;
	if (tlen >= 4)
		return 2;
	if (tlen >= 2)
		return 1;
	return 0;
}

static void
mdma_stop_unused(void)
{
	MDMA_CH->CCR &= ~ MDMA_CCR_EN_Msk;
	while ((MDMA_CH->CCR & MDMA_CCR_EN_Msk) != 0)
		;
}

/* запустить пересылку и дождаться завершения. */
static void
mdma_startandwait(void)
{
	// MDMA implementation
	MDMA_CH->CIFCR =
		MDMA_CIFCR_CLTCIF_Msk |
		MDMA_CIFCR_CBTIF_Msk |
		MDMA_CIFCR_CBRTIF_Msk |
		MDMA_CIFCR_CCTCIF_Msk |
		MDMA_CIFCR_CTEIF_Msk |
		0;
	(void) MDMA_CH->CIFCR;

	// Set priority
	MDMA_CH->CCR = (MDMA_CH->CCR & ~ (MDMA_CCR_PL_Msk)) |
			(MDMA_CCR_PL_VALUE << MDMA_CCR_PL_Pos) |
			0;
	(void) MDMA_CH->CCR;

	MDMA_CH->CCR |= MDMA_CCR_EN_Msk;
	(void) MDMA_CH->CCR;
	/* start transfer */
	MDMA_CH->CCR |= MDMA_CCR_SWRQ_Msk;
	(void) MDMA_CH->CCR;

	/* wait for complete */
	while ((MDMA_CH->CISR & MDMA_CISR_CTCIF_Msk) == 0)	// Channel x Channel Transfer Complete interrupt flag
	{
		ASSERT((MDMA_CH->CISR & MDMA_CISR_TEIF_Msk) == 0);	/* Channel x transfer error interrupt flag */
	}
	//__DMB();	//ensure the ordering of data cache maintenance operations and their effects
	ASSERT((MDMA_CH->CISR & MDMA_CISR_TEIF_Msk) == 0);	/* Channel x transfer error interrupt flag */

}

void arm_hardware_mdma_initialize(void)
{
#if CPUSTYLE_STM32MP1
	/* Enable the MDMA Clock */
	RCC->MP_AHB6ENSETR = RCC_MP_AHB6ENSETR_MDMAEN;	/* MDMA clock enable */
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6LPENSETR = RCC_MP_AHB6LPENSETR_MDMALPEN;	/* MDMA clock enable */
	(void) RCC->MP_AHB6LPENSETR;
	//RCC->MP_TZAHB6ENSETR = RCC_MP_TZAHB6ENSETR_MDMAEN;
	//(void) RCC->MP_TZAHB6ENSETR;

	/* SYSCFG clock enable */
	RCC->MP_APB3ENSETR = RCC_MP_APB3ENSETR_SYSCFGEN;
	(void) RCC->MP_APB3ENSETR;
	RCC->MP_APB3LPENSETR = RCC_MP_APB3LPENSETR_SYSCFGLPEN;
	(void) RCC->MP_APB3LPENSETR;
	/*
	 * Interconnect update : select master using the port 1.
	 * LTDC = AXI_M9.
	 * MDMA = AXI_M7.
	 */
	SYSCFG->ICNR |= SYSCFG_ICNR_AXI_M7;
	(void) SYSCFG->ICNR;

#elif CPUSTYLE_STM32H7XX
	/* Enable the MDMA Clock */
	RCC->AHB3ENR |= RCC_AHB3ENR_MDMAEN_Msk;	/* MDMA clock enable */
	(void) RCC->AHB3ENR;
	RCC->AHB3LPENR |= RCC_AHB3LPENR_MDMALPEN_Msk;
	(void) RCC->AHB3LPENR;

#else /* CPUSTYLE_STM32H7XX */
	/* Enable the MDMA Clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_MDMAEN;	/* MDMA clock enable */
	(void) RCC->AHB1ENR;
	RCC->AHB3LPENR |= RCC_AHB3LPENR_MDMALPEN_Msk;
	(void) RCC->AHB3LPENR;

#endif /* CPUSTYLE_STM32H7XX */
}

// Add custom draw unit
#if WITHLVGL

// Add custom draw unit
void lvglhw_initialize(void)
{

}

#endif /* WITHLVGL */

#elif WITHMDMAHW & CPUSTYLE_ALLWINNER

/* Использование G2D для формирования изображений */
// https://github.com/tinalinux/linux-3.10/blob/46f73ef4efcb4014b25e5ad1eca750ad62a1d0ff/drivers/char/sunxi_g2d/g2d_driver.c
// https://github.com/tinalinux/linux-3.10/blob/46f73ef4efcb4014b25e5ad1eca750ad62a1d0ff/drivers/char/sunxi_g2d/g2d_regs.h
// https://github.com/tinalinux/linux-3.10/blob/46f73ef4efcb4014b25e5ad1eca750ad62a1d0ff/drivers/char/sunxi_g2d/g2d_bsp_sun8iw11.c


// https://github.com/lianghuixin/licee4.4/blob/bfee1d63fa355a54630244307296a00a973b70b0/linux-4.4/drivers/char/sunxi_g2d/g2d_bsp_v2.c

void arm_hardware_mdma_initialize(void)
{
#if CPUSTYLE_T507 || CPUSTYLE_H616
	{
		// AW_G2D_开发指南.pdf 图2‑1: design_spec0
		// G2D101r1p0
		// 300 MHz clock
		//PRINTF("arm_hardware_mdma_initialize (G2D)\n");
		unsigned M = 4;	/* M = 1..32 */
		unsigned divider = 0;

		CCU->MBUS_CFG_REG |= (UINT32_C(1) << 30);				// MBUS Reset 1: De-assert reset
		CCU->MBUS_MAT_CLK_GATING_REG |= (UINT32_C(1) << 10);	// Gating MBUS Clock For G2D
		//local_delay_us(10);

		// PLL_VIDEO1 may be used for LVDS synchronization
		// User manual say about 250 MHz default.
		//	CLK_SRC_SEL.
		//	Clock Source Select
		//	0: PLL_DE
		//	1: PLL_PERI0(2X)
		CCU->G2D_CLK_REG = (CCU->G2D_CLK_REG & ~ (UINT32_C(0x01) << 24) & ~ (UINT32_C(0x0F) << 0)) |
			0x01 * (UINT32_C(1) << 24) |	// CLK_SRC_SEL. Clock Source Select 0: PLL_DE 1: PLL_PERI0(2X)
			(M - 1) * (UINT32_C(1) << 0) | // FACTOR_M
			0;
		CCU->G2D_CLK_REG |= (UINT32_C(1) << 31);	// G2D_CLK_GATING
		local_delay_us(10);
		PRINTF("allwnr_t507_get_g2d_freq()=%u MHz\n", (unsigned) (allwnr_t507_get_g2d_freq() / 1000 / 1000));

		//CCU->G2D_BGR_REG = 0;
		CCU->G2D_BGR_REG |= (UINT32_C(1) << 0);		/* Enable gating clock for G2D 1: Pass */
		CCU->G2D_BGR_REG &= ~ (UINT32_C(1) << 16);	/* G2D reset 0: Assert */
		CCU->G2D_BGR_REG |= (UINT32_C(1) << 16);	/* G2D reset 1: De-assert */
		local_delay_us(10);

		/* на Allwinner T507 модифицируемы только младшие 8 бит */
		G2D_TOP->G2D_SCLK_DIV = (G2D_TOP->G2D_SCLK_DIV & ~ 0xFF) |
			divider * (UINT32_C(1) << 4) |	// ROT divider (looks like power of 2) CORE1_SCLK_DIV
			divider * (UINT32_C(1) << 0) |	// MIXER divider (looks like power of 2) CORE0_SCLK_DIV
			0;
		(void) G2D_TOP->G2D_SCLK_DIV;
		//local_delay_us(10);

		if (1)
		{
#if defined (G2D_MIXER)
			// MIXER
			G2D_TOP->G2D_SCLK_GATE |= (UINT32_C(1) << 0);	// Gate open: 0x02: rot, 0x01: mixer
			G2D_TOP->G2D_HCLK_GATE |= (UINT32_C(1) << 0);	// Gate open: 0x02: rot, 0x01: mixer
			G2D_TOP->G2D_AHB_RST &= ~ ~ (UINT32_C(1) << 0);	// Assert reset: 0x02: rot, 0x01: mixer
			G2D_TOP->G2D_AHB_RST |= (UINT32_C(1) << 0);	// De-assert reset: 0x02: rot, 0x01: mixer
#endif /* defined (G2D_MIXER) */
		}

		if (1)
		{
			// ROT
			G2D_TOP->G2D_SCLK_GATE |= (UINT32_C(1) << 1);	// Gate open: 0x02: rot, 0x01: mixer
			G2D_TOP->G2D_HCLK_GATE |= (UINT32_C(1) << 1);	// Gate open: 0x02: rot, 0x01: mixer
			G2D_TOP->G2D_AHB_RST &= ~ (UINT32_C(1) << 1);	// Assert reset: 0x02: rot, 0x01: mixer
			G2D_TOP->G2D_AHB_RST |= (UINT32_C(1) << 1);	// De-assert reset: 0x02: rot, 0x01: mixer
		}

		local_delay_ms(10);

		// G2D version=01010100
		// https://github.com/MYIR-ALLWINNER/myir-t5-kernel/blob/a7089355dd727f5aaedade642f5fbc5b354b215a/drivers/char/sunxi_g2d/g2d_rcq/g2d_top_type.h#L57
		//PRINTF("G2D version=%08" PRIX32 "\n", G2D_TOP->G2D_VERSION);

	}

#elif (CPUSTYLE_T113 || CPUSTYLE_F133)
	// https://github.com/lianghuixin/licee4.4/blob/bfee1d63fa355a54630244307296a00a973b70b0/linux-4.4/drivers/char/sunxi_g2d/g2d_bsp_v2.c
	// В Linux используется 300 МГц
	// https://github.com/duvitech-llc/tina-t113-linux-5.4/blob/150c69d4f2b0886db269cc7883f007e2cdcd839c/drivers/char/sunxi_g2d/g2d_rcq/g2d.c#L432
	//PRINTF("arm_hardware_mdma_initialize (G2D)\n");
	//unsigned M = 3;	/* 400 MHz M = 1..32 */
	unsigned M = 4;	/* 300 MHz M = 1..32 */
	unsigned divider = 0;

	CCU->MBUS_CLK_REG |= (UINT32_C(1) << 30);				// MBUS Reset 1: De-assert reset
	CCU->MBUS_MAT_CLK_GATING_REG |= (UINT32_C(1) << 10);	// Gating MBUS Clock For G2D
	//local_delay_us(10);

	// User manual say about 250 MHz default.
	CCU->G2D_CLK_REG = (CCU->G2D_CLK_REG & ~ (UINT32_C(0x07) << 24) & ~ (UINT32_C(0x1F) << 0)) |
		0x00 * (UINT32_C(1) << 24) |	// 000: PLL_PERI(2X), 001: PLL_VIDEO0(4X), 010: PLL_VIDEO1(4X), 011: PLL_AUDIO1(DIV2)
		(M - 1) * (UINT32_C(1) << 0) | // FACTOR_M
		0;
	CCU->G2D_CLK_REG |= (UINT32_C(1) << 31);	// G2D_CLK_GATING
	local_delay_us(10);
	//PRINTF("allwnr_t113_get_g2d_freq()=%u MHz\n", (unsigned) (allwnr_t113_get_g2d_freq() / 1000 / 1000));

	//CCU->G2D_BGR_REG = 0;
	CCU->G2D_BGR_REG |= (UINT32_C(1) << 0);		/* Enable gating clock for G2D 1: Pass */
	CCU->G2D_BGR_REG &= ~ (UINT32_C(1) << 16);	/* G2D reset 0: Assert */
	CCU->G2D_BGR_REG |= (UINT32_C(1) << 16);	/* G2D reset 1: De-assert */
	(void) CCU->G2D_BGR_REG;
	local_delay_us(10);

	/* на Allwinner T113-S3 и F133 модифицируемы только младшие 8 бит */
	G2D_TOP->G2D_SCLK_DIV = (G2D_TOP->G2D_SCLK_DIV & ~ 0xFF) |
		divider * (UINT32_C(1) << 4) |	// ROT divider (looks like power of 2) CORE1_SCLK_DIV
		divider * (UINT32_C(1) << 0) |	// MIXER divider (looks like power of 2) CORE0_SCLK_DIV
		0;
	(void) G2D_TOP->G2D_SCLK_DIV;
	//local_delay_us(10);

	G2D_TOP->G2D_SCLK_GATE |= (UINT32_C(1) << 1) | (UINT32_C(1) << 0);	// Gate open: 0x02: rot, 0x01: mixer
	(void) G2D_TOP->G2D_SCLK_GATE;
	G2D_TOP->G2D_HCLK_GATE |= (UINT32_C(1) << 1) | (UINT32_C(1) << 0);	// Gate open: 0x02: rot, 0x01: mixer
	(void) G2D_TOP->G2D_HCLK_GATE;
	G2D_TOP->G2D_AHB_RST &= ~ (UINT32_C(1) << 1) & ~ (UINT32_C(1) << 0);	// Assert reset: 0x02: rot, 0x01: mixer
	(void) G2D_TOP->G2D_AHB_RST;
	G2D_TOP->G2D_AHB_RST |= (UINT32_C(1) << 1) | (UINT32_C(1) << 0);	// De-assert reset: 0x02: rot, 0x01: mixer
	(void) G2D_TOP->G2D_AHB_RST;

	local_delay_ms(10);

	// G2D version=01100114
	//PRINTF("G2D version=%08" PRIX32 "\n", G2D_TOP->G2D_VERSION);

	//memset(G2D_TOP, 0xFF, sizeof * G2D_TOP);
	awxx_rcq(0xDEADBEEF, 64);
//	PRINTF("G2D_TOP:\n");
//	printhex32(G2D_TOP_BASE, G2D_TOP, 256);
#else
	#error Unhandled CPUSTYLE_xxx
#endif
	// peri:   allwnr_t113_get_g2d_freq()=600000000
	// video0: allwnr_t113_get_g2d_freq()=297000000
	// video1: allwnr_t113_get_g2d_freq()=297000000
	// audio1: allwnr_t113_get_g2d_freq()=768000000
	//PRINTF("allwnr_t113_get_g2d_freq()=%" PRIuFAST32 "\n", allwnr_t113_get_g2d_freq());

	aw_g2d_initialize();
	 //mixer_set_reg_base(G2D_BASE);
	//PRINTF("arm_hardware_mdma_initialize (G2D) done.\n");
}

#if WITHLVGL

#include "misc/lv_area_private.h"

#define DRAW_UNIT_ID_AWG2D 77
#define DRAW_UNIT_ID_AWROT 78

#define LV_DRAW_AWG2D_DRAW_UNIT_CNT 1

typedef struct _draw_awg2d_unit_t {
    lv_draw_unit_t base_unit;
} draw_awg2d_unit_t;

typedef struct _draw_awrot_unit_t {
    lv_draw_unit_t base_unit;
} draw_awrot_unit_t;


#if defined (G2D_MIXER)

static void
awg2d_bitblt(unsigned keyflag, COLORPIP_T keycolor,
		unsigned srcFormat, unsigned sstride,
		uint_fast32_t ssizehw, uintptr_t saddr,
		unsigned tstride, uint_fast32_t tsizehw,
		uintptr_t taddr);

// на t113 пока не работает правильно (чёрный квадрат под Emma Smith)
static void
draw_awg2d_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc, const lv_area_t * area, lv_layer_t * layer)
{
	//PRINTF("draw_awg2d_image\n");
	// Copy rectangle
	ASSERT(lv_area_get_width(& dsc->image_area) == lv_area_get_width(area));
	ASSERT(lv_area_get_height(& dsc->image_area) == lv_area_get_height(area));
//	PRINTF("draw_awg2d_image: tw/th=%d/%d, x/y=%d/%d, w/h=%d/%d\n",
//			(int) lv_area_get_width(& dsc->image_area), (int) lv_area_get_height(& dsc->image_area),
//			(int) area->x1, (int) area->y1, (int) lv_area_get_width(area), (int) lv_area_get_height(area));


    const lv_area_t * coords = &t->area;
    lv_area_t clipped_coords;
    if(!lv_area_intersect(&clipped_coords, coords, &t->clip_area)) {
        return ;//LV_DRAW_UNIT_IDLE;
    }

    void * const dest = lv_draw_layer_go_to_xy(layer,
                                         clipped_coords.x1 - layer->buf_area.x1,
                                         clipped_coords.y1 - layer->buf_area.y1);
//    ASSERT(dest);
//    PRINTF("dest=%p (base=%p)\n", dest, layer->draw_buf->data);	// правильный адрес получателя

//    // Где источник?
//    PRINTF("src=%p\n", dsc->src);
    const lv_draw_buf_t * dbf = dsc->src;
    ASSERT(LV_IMAGE_SRC_VARIABLE == lv_image_src_get_type(dbf));

    const uint_fast16_t sw = lv_area_get_width(area);
    const uint_fast16_t sh = lv_area_get_height(area);
//    PRINTF("sw/sh=%d/%d\n", (int) sw, (int) sh);

    const unsigned keyflag = 0;
    const COLORPIP_T keycolor = 0;
	const unsigned srcFormat = awxx_get_srcformat(keyflag);
	const unsigned tstride = lv_draw_buf_width_to_stride(lv_area_get_width(&layer->buf_area), dsc->base.layer->color_format);
	const unsigned sstride = dbf->header.stride;
	const uintptr_t taddr = (uintptr_t) dest;
	//const uintptr_t saddr = (uintptr_t) lv_draw_buf_goto_xy(dbf, dsc->image_area.x1, dsc->image_area.y1);
	const uintptr_t saddr = (uintptr_t) lv_draw_buf_goto_xy(dbf, 0, 0);
	const uint_fast32_t ssizehw = ((sh - 1) << 16) | ((sw - 1) << 0);
	const uint_fast32_t tsizehw = ((sh - 1) << 16) | ((sw - 1) << 0);		/* размер совпадающий с источником - просто для удобства */

	const uintptr_t dstinvalidateaddr = (uintptr_t) layer->draw_buf->data;
	const int_fast32_t dstinvalidatesize = layer->draw_buf->data_size;
	const uintptr_t srcinvalidateaddr = (uintptr_t) dbf->data;
	const int_fast32_t srcinvalidatesize = dbf->data_size;

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	dcache_clean(srcinvalidateaddr, srcinvalidatesize);

	awg2d_bitblt(keyflag, keycolor, srcFormat, sstride, ssizehw, saddr, tstride,
			tsizehw, taddr);
}

static void
draw_awg2d_layer(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc, const lv_area_t * area, lv_layer_t * layer)
{
	//PRINTF("draw_awg2d_layer\n");
	// Copy rectangle
	ASSERT(lv_area_get_width(& dsc->image_area) == lv_area_get_width(area));
	ASSERT(lv_area_get_height(& dsc->image_area) == lv_area_get_height(area));
//	PRINTF("draw_awg2d_image: tw/th=%d/%d, x/y=%d/%d, w/h=%d/%d\n",
//			(int) lv_area_get_width(& dsc->image_area), (int) lv_area_get_height(& dsc->image_area),
//			(int) area->x1, (int) area->y1, (int) lv_area_get_width(area), (int) lv_area_get_height(area));


    const lv_area_t * coords = &t->area;
    lv_area_t clipped_coords;
    if(!lv_area_intersect(&clipped_coords, coords, &t->clip_area)) {
        return ;//LV_DRAW_UNIT_IDLE;
    }

    void * const dest = lv_draw_layer_go_to_xy(layer,
                                         clipped_coords.x1 - layer->buf_area.x1,
                                         clipped_coords.y1 - layer->buf_area.y1);
//    ASSERT(dest);
//    PRINTF("dest=%p (base=%p)\n", dest, layer->draw_buf->data);	// правильный адрес получателя

//    // Где источник?
//    PRINTF("src=%p\n", dsc->src);
    const lv_draw_buf_t * dbf = dsc->src;
    ASSERT(LV_IMAGE_SRC_VARIABLE == lv_image_src_get_type(dbf));

    const uint_fast16_t sw = lv_area_get_width(area);
    const uint_fast16_t sh = lv_area_get_height(area);
//    PRINTF("sw/sh=%d/%d\n", (int) sw, (int) sh);

    const unsigned keyflag = 0;
    const COLORPIP_T keycolor = 0;
	const unsigned srcFormat = awxx_get_srcformat(keyflag);
	const unsigned tstride = lv_draw_buf_width_to_stride(lv_area_get_width(&layer->buf_area), dsc->base.layer->color_format);
	const unsigned sstride = dbf->header.stride;
	const uintptr_t taddr = (uintptr_t) dest;
	//const uintptr_t saddr = (uintptr_t) lv_draw_buf_goto_xy(dbf, dsc->image_area.x1, dsc->image_area.y1);
	const uintptr_t saddr = (uintptr_t) lv_draw_buf_goto_xy(dbf, 0, 0);
	const uint_fast32_t ssizehw = ((sh - 1) << 16) | ((sw - 1) << 0);
	const uint_fast32_t tsizehw = ((sh - 1) << 16) | ((sw - 1) << 0);		/* размер совпадающий с источником - просто для удобства */

	const uintptr_t dstinvalidateaddr = (uintptr_t) layer->draw_buf->data;
	const int_fast32_t dstinvalidatesize = layer->draw_buf->data_size;
	const uintptr_t srcinvalidateaddr = (uintptr_t) dbf->data;
	const int_fast32_t srcinvalidatesize = dbf->data_size;

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	dcache_clean(srcinvalidateaddr, srcinvalidatesize);

	awg2d_bitblt(keyflag, keycolor, srcFormat, sstride, ssizehw, saddr, tstride,
			tsizehw, taddr);
}

static void
draw_awg2d_fill(lv_draw_task_t * t, const lv_draw_fill_dsc_t * dsc, const lv_area_t * area, lv_layer_t * layer)
{
	//PRINTF("draw_awg2d_fill\n");
	// Fill rectangle
//	PRINTF("draw_awg2d_fill: x/y=%d/%d, w/h=%d/%d, color=%08X\n",
//			(int) area->x1, (int) area->y1,
//			(int) lv_area_get_width(area), (int) lv_area_get_height(area),
//			(unsigned) COLOR24(dsc->color.red, dsc->color.green, dsc->color.blue)
//			);

    const lv_area_t * coords = &t->area;
    lv_area_t clipped_coords;
    if(!lv_area_intersect(&clipped_coords, coords, &t->clip_area)) {
        return;// LV_DRAW_UNIT_IDLE;
    }

    void * dest = lv_draw_layer_go_to_xy(layer,
                                         clipped_coords.x1 - layer->buf_area.x1,
                                         clipped_coords.y1 - layer->buf_area.y1);

	const uint_fast16_t x = area->x1;
	const uint_fast16_t y = area->y1;
	const uint_fast16_t w = lv_area_get_width(area);
	const uint_fast16_t h = lv_area_get_height(area);

	COLORPIP_T color = TFTALPHA(dsc->opa * 255 / LV_OPA_COVER, TFTRGB(dsc->color.red, dsc->color.green, dsc->color.blue));
	//const COLOR24_T color = COLOR24(dsc->color.red, dsc->color.green, dsc->color.blue);
	const uint_fast16_t dx = lv_area_get_width(& layer->buf_area);
	PACKEDCOLORPIP_T * buffer = (PACKEDCOLORPIP_T *) dest;
	const uintptr_t dstinvalidateaddr = (uintptr_t) layer->draw_buf->data;	// параметры invalidate получателя
	const int_fast32_t dstinvalidatesize = layer->draw_buf->data_size;
	const unsigned fillmask = dsc->opa != LV_OPA_COVER ? FILL_FLAG_MIXBG : FILL_FLAG_NONE;
	const uintptr_t taddr = (uintptr_t) buffer;
	const unsigned tstride = layer->draw_buf->header.stride;
	const uint_fast32_t tsizehw = ((h - 1) << 16) | ((w - 1) << 0);

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	hwaccel_fillrect(buffer, dx, taddr, tstride, tsizehw, COLORPIP_A(color), (color & 0xFFFFFF), w, h, color, fillmask);

}

/**
 * Called to try to assign a draw task to itself.
 * `lv_draw_get_next_available_task` can be used to get an independent draw task.
 * A draw task should be assign only if the draw unit can draw it too
 * @param draw_unit     pointer to the draw unit
 * @param layer         pointer to a layer on which the draw task should be drawn
 * @return              >=0:    The number of taken draw task:
 *                                  0 means the task has not yet been completed.
 *                                  1 means a new task has been accepted.
 *                      -1:     The draw unit wanted to work on a task but couldn't do that
 *                              due to some errors (e.g. out of memory).
 *                              It signals that LVGL should call the dispatcher later again
 *                              to let draw unit try to start the rendering again.
 */
static int32_t awg2d_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    LV_PROFILER_DRAW_BEGIN;
    draw_awg2d_unit_t * draw_awg2d_unit = (draw_awg2d_unit_t *) draw_unit;

    lv_draw_task_t * const t = lv_draw_get_available_task(layer, NULL, DRAW_UNIT_ID_AWG2D);
    if(t == NULL) {
		//TP();
		LV_PROFILER_DRAW_END;
		return LV_DRAW_UNIT_IDLE;  /*Couldn't start rendering*/
    }

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;

    {
        /*Render the draw task*/
        switch(t->type)
        {
        default:
        	PRINTF("awrot_execute_drawing: t->type=%d\n", (int) t->type);
        	break;
        case LV_DRAW_TASK_TYPE_LAYER:
    		draw_awg2d_layer(t, (lv_draw_image_dsc_t *) t->draw_dsc, &t->area, layer);
    		break;
    	case LV_DRAW_TASK_TYPE_IMAGE:
    		draw_awg2d_image(t, (lv_draw_image_dsc_t *) t->draw_dsc, &t->area, layer);
    		break;
    	case LV_DRAW_TASK_TYPE_FILL:
    		draw_awg2d_fill(t, (lv_draw_fill_dsc_t *) t->draw_dsc, &t->area, layer);
    		break;
    //	case LV_DRAW_TASK_TYPE_BORDER:
    //		lv_draw_awg2d_border(t, t->draw_dsc, &t->area);
    //		break;
    //	case LV_DRAW_TASK_TYPE_BOX_SHADOW:
    //		lv_draw_awg2d_box_shadow(t, t->draw_dsc, &t->area);
    //		break;
    //	case LV_DRAW_TASK_TYPE_LETTER:
    //		lv_draw_awg2d_letter(t, t->draw_dsc, &t->area);
    //		break;
    //	case LV_DRAW_TASK_TYPE_LABEL:
    //		lv_draw_awg2d_label(t, t->draw_dsc, &t->area);
    //		break;
    //	case LV_DRAW_TASK_TYPE_ARC:
    //		lv_draw_awg2d_arc(t, t->draw_dsc, &t->area);
    //		break;
    //	case LV_DRAW_TASK_TYPE_LINE:
    //		lv_draw_awg2d_line(t, t->draw_dsc);
    //		break;
    //	case LV_DRAW_TASK_TYPE_TRIANGLE:
    //		lv_draw_awg2d_triangle(t, t->draw_dsc);
    //		break;
    //	case LV_DRAW_TASK_TYPE_MASK_RECTANGLE:
    //		lv_draw_awg2d_mask_rect(t, t->draw_dsc);
    //		break;
    //#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
    //	case LV_DRAW_TASK_TYPE_VECTOR:
    //		lv_draw_awg2d_vector(t, t->draw_dsc);
    //		break;
    //#endif
		}
    }
    t->state = LV_DRAW_TASK_STATE_READY;

    /*The draw unit is free now. Request a new dispatching as it can get a new task*/
    lv_draw_dispatch_request();

    LV_PROFILER_DRAW_END;
    return 1;
}


/**
 * Устанавливает preferred_draw_unit_id в DRAW_UNIT_ID_AWG2D,
 * если отрисовываем этим блоком
 * @param draw_unit
 * @param task
 * @return - LVGL не использует
 */
static int32_t awg2d_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task)
{
    LV_UNUSED(draw_unit);
    switch(task->type)
    {
    	case LV_DRAW_TASK_TYPE_IMAGE:	// на t113 пока не работает правильно (чёрный квадрат под Emma Smith)
    		return 0;
    	case LV_DRAW_TASK_TYPE_LAYER:
       	{
                lv_draw_image_dsc_t * draw_dsc = (lv_draw_image_dsc_t *) task->draw_dsc;
                //return 0;

                /* not support opaque */
                // also see draw_dsc->blend_mode
                if (draw_dsc->opa != LV_OPA_COVER)
                {
                    return 0;
                }
                if (draw_dsc->clip_radius != 0)
                {
                    return 0;
                }
                if (lv_color_to_int(draw_dsc->recolor) != lv_color_to_int(lv_color_black()))
                {
                    return 0;
                }
                /* not support skew */
                if (draw_dsc->skew_x != 0 || draw_dsc->skew_y != 0)
                {
                    return 0;
                }
                /* not support scale */
                if (draw_dsc->scale_x != LV_SCALE_NONE || draw_dsc->scale_y != LV_SCALE_NONE)
                {
                    return 0;
                }
                /* not support rotation */
                if (draw_dsc->rotation != 0)
                {
                    return 0;
                }
                if (lv_area_get_width(& draw_dsc->image_area) == 0 || lv_area_get_height(& draw_dsc->image_area) == 0)
                {
                    return 0;
                }
                if (lv_area_get_width(& draw_dsc->image_area) < 2 || lv_area_get_height(& draw_dsc->image_area) < 2)
                {
                    return 0;
                }
                bool masked = draw_dsc->bitmap_mask_src != NULL;
                if (masked)
                {
                    return 0;
                }
                lv_color_format_t cf = (lv_color_format_t) draw_dsc->header.cf;
                if (cf != display_get_lvformat())
                {
                    return 0;
                }
                if (masked && (cf == LV_COLOR_FORMAT_A8 || cf == LV_COLOR_FORMAT_RGB565A8))
                {
                    return 0;
                }
                if (cf >= LV_COLOR_FORMAT_PROPRIETARY_START)
                {
                    return 0;
                }
            }
            break;
        case LV_DRAW_TASK_TYPE_FILL:
        	{
                lv_draw_fill_dsc_t * dsc = task->draw_dsc;
                if (lv_area_get_width(& task->area) < 2 || lv_area_get_height(& task->area) < 2)
                	return 0;
                if(!(dsc->radius == 0
                     && dsc->grad.dir == LV_GRAD_DIR_NONE
                     && (dsc->base.layer->color_format == LV_COLOR_FORMAT_ARGB8888
                         || dsc->base.layer->color_format == LV_COLOR_FORMAT_XRGB8888
                         || dsc->base.layer->color_format == LV_COLOR_FORMAT_RGB888
                         || dsc->base.layer->color_format == LV_COLOR_FORMAT_RGB565))) {
                    return 0;
                }
            }
            break;

#if LV_USE_3DTEXTURE
        case LV_DRAW_TASK_TYPE_3D:
            return 0;
#endif
        default:
            return 0;
    }

    task->preferred_draw_unit_id = DRAW_UNIT_ID_AWG2D;
    task->preference_score = 0;	// похоже, для разбирательств между разными evaluate

    return 0;
}

/**
 * Called to signal the unit to complete all tasks in order to return their ready status.
 * This callback can be implemented in case of asynchronous task processing.
 * Below is an example to show the difference between synchronous and asynchronous:
 *
 * Synchronous:
 * LVGL thread              DRAW thread                 HW
 *
 * task1             -->    submit               -->    Receive task1
 *                          wait_for_finish()
 *                   <--    task1->state = READY <--    Complete task1
 * task2             -->    submit               -->    Receive task2
 *                          wait_for_finish()
 *                          task2->state = READY <--    Complete task2
 * task3             -->    submit               -->    Receive task3
 *                          wait_for_finish()
 *                   <--    task3->state = READY <--    Complete task3
 * task4             -->    submit               -->    Receive task4
 *                          wait_for_finish()
 *                   <--    task4->state = READY <--    Complete task4
 * NO MORE TASKS
 *
 *
 * Asynchronous:
 * LVGL thread              DRAW thread                 HW
 *                                                      is IDLE
 * task1             -->    queue task1
 *                          submit               -->    Receive task1
 * task2             -->    queue task2                 is BUSY (with task1)
 * task3             -->    queue task3                 still BUSY (with task1)
 * task4             -->    queue task4                 becomes IDLE
 *                   <--    task1->state = READY <--    Complete task1
 *                          submit               -->    Receive task2, task3, task4
 * NO MORE TASKS
 * wait_for_finish_cb()     wait_for_finish()
 *                                               <--    Complete task2, task3, task4
 *                   <--    task2->state = READY <--
 *                   <--    task3->state = READY <--
 *                   <--    task4->state = READY <--
 *
 * @param draw_unit
 * @return
 */
#if LV_USE_OS
int32_t awg2d_wait_for_finish(lv_draw_unit_t * draw_unit)
{
    draw_awg2d_unit_t * draw_awg2d_unit = (draw_awg2d_unit_t *) draw_unit;

	return 0;
}
#endif

static int32_t draw_awg2d_delete(lv_draw_unit_t * draw_unit)
{
#if LV_USE_OS
    draw_awg2d_unit_t * draw_awg2d_unit = (draw_awg2d_unit_t *) draw_unit;

    uint32_t i;
    for(i = 0; i < LV_DRAW_AWG2D_DRAW_UNIT_CNT; i++) {
        lv_draw_sw_thread_dsc_t * thread_dsc = &draw_awg2d_unit->thread_dscs[i];

        LV_LOG_INFO("cancel software rendering thread");
        thread_dsc->exit_status = true;

        if(thread_dsc->inited) {
            lv_thread_sync_signal(&thread_dsc->sync);
        }
        lv_thread_delete(&thread_dsc->thread);
    }

    return 0;
#else
    LV_UNUSED(draw_unit);
    return 0;
#endif
}

#endif /* defined (G2D_MIXER) */

#if defined (G2D_ROT)

static void
draw_awrot_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * dsc, const lv_area_t * area, lv_layer_t * layer)
{
	//PRINTF("draw_awrot_image\n");
#if 0
	PRINTF("draw src x=%d,y=%d,w=%d,h=%d\n", dsc->image_area.x1, dsc->image_area.y1,
			lv_area_get_width(& dsc->image_area),
			lv_area_get_height(& dsc->image_area));
#endif
	// Copy rectangle
	ASSERT(lv_area_get_width(& dsc->image_area) == lv_area_get_width(area));
	ASSERT(lv_area_get_height(& dsc->image_area) == lv_area_get_height(area));
//	PRINTF("draw_awg2d_image: tw/th=%d/%d, x/y=%d/%d, w/h=%d/%d\n",
//			(int) lv_area_get_width(& dsc->image_area), (int) lv_area_get_height(& dsc->image_area),
//			(int) area->x1, (int) area->y1, (int) lv_area_get_width(area), (int) lv_area_get_height(area));

    const lv_area_t * coords = &t->area;
    lv_area_t clipped_coords;
    if(!lv_area_intersect(&clipped_coords, coords, &t->clip_area)) {
        return ;//LV_DRAW_UNIT_IDLE;
    }

    void * const dest = lv_draw_layer_go_to_xy(layer,
                                         clipped_coords.x1 - layer->buf_area.x1,
                                         clipped_coords.y1 - layer->buf_area.y1);
//    ASSERT(dest);
//    PRINTF("dest=%p (base=%p)\n", dest, layer->draw_buf->data);	// правильный адрес получателя

//    // Где источник?
//    PRINTF("src=%p\n", dsc->src);
    const lv_draw_buf_t * dbf = dsc->src;
    ASSERT(LV_IMAGE_SRC_VARIABLE == lv_image_src_get_type(dbf));

    const uint_fast16_t w = lv_area_get_width(area);
    const uint_fast16_t h = lv_area_get_height(area);//    PRINTF("sw/sh=%d/%d\n", (int) sw, (int) sh);

    if (h == 0 || w == 0)
    	return;
    const unsigned keyflag = 0;
    const COLORPIP_T keycolor = 0;
	const unsigned srcFormat = awxx_get_srcformat(keyflag);
	const unsigned tstride = lv_draw_buf_width_to_stride(lv_area_get_width(&layer->buf_area), dsc->base.layer->color_format);
	const unsigned sstride = dbf->header.stride;
	const uintptr_t taddr = (uintptr_t) dest;
	//const uintptr_t saddr = (uintptr_t) lv_draw_buf_goto_xy(dbf, dsc->image_area.x1, dsc->image_area.y1);
	const uintptr_t saddr = (uintptr_t) lv_draw_buf_goto_xy(dbf, 0, 0);

	const uintptr_t dstinvalidateaddr = (uintptr_t) layer->draw_buf->data;
	const int_fast32_t dstinvalidatesize = layer->draw_buf->data_size;
	const uintptr_t srcinvalidateaddr = (uintptr_t) dbf->data;
	const int_fast32_t srcinvalidatesize = dbf->data_size;

	// target size для 4-х квадрантов
	// похоже, поворот учитывать не требуется. Но просто для "красоты" оставлю четыре варианта.
	const uint_fast32_t tsizehw4 [4] =
	{
		((h - 1) << 16) | ((w - 1) << 0),	// target size if 0 CCW
		((w - 1) << 16) | ((h - 1) << 0),	// target size if 90 CCW
		((h - 1) << 16) | ((w - 1) << 0),	// target size if 180 CCW
		((w - 1) << 16) | ((h - 1) << 0),	// target size if 270 CCW
	};

	const int mx = 0;
	const int my = 0;
	unsigned quadrant = 0;
	const uint_fast32_t ssizehw = ((h - 1) << 16) | ((w - 1) << 0); // source size
	const uint_fast32_t tsizehw = tsizehw4 [quadrant];
	uint_fast32_t rot_ctl = 0;
	rot_ctl |= !! mx * (UINT32_C(1) << 7);	// flip horizontal
	rot_ctl |= !! my * (UINT32_C(1) << 6);	// flip vertical
	rot_ctl |= ((0 - quadrant) & 0x03) * (UINT32_C(1) << 4);	// rotate (0: 0deg, 1: 90deg CW, 2: 180deg CW, 3: 270deg CW)

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	dcache_clean(srcinvalidateaddr, srcinvalidatesize);
	hwaccel_rotcopy(saddr, sstride, ssizehw, taddr, tstride, tsizehw, rot_ctl);
}

/**
 * Устанавливает preferred_draw_unit_id в DRAW_UNIT_ID_AWROT
 * если отрисовываем этим блоком
 * Блок позволяет копировать прямоугольники без изменения формата и размеров,
 * возможен поворот на углы кратные 90 градусам
 * @param draw_unit
 * @param task
 * @return - LVGL не использует
 */
static int32_t awrot_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task)
{
    LV_UNUSED(draw_unit);
    switch(task->type) {
        case LV_DRAW_TASK_TYPE_IMAGE:
        case LV_DRAW_TASK_TYPE_LAYER:
        	{
                lv_draw_image_dsc_t * draw_dsc = (lv_draw_image_dsc_t *) task->draw_dsc;
//        		PRINTF("ev src x=%d,y=%d,w=%d,h=%d\n", draw_dsc->image_area.x1, draw_dsc->image_area.y1,
//        				lv_area_get_width(& draw_dsc->image_area),
//						lv_area_get_height(& draw_dsc->image_area));

                /* not support skew */
                if (draw_dsc->skew_x != 0 || draw_dsc->skew_y != 0) {
                    return 0;
                }
                /* not support rotation */
                if (draw_dsc->rotation != 0)
                {
                    return 0;
                }
                /* not support scale */
                if (draw_dsc->scale_x != LV_SCALE_NONE || draw_dsc->scale_y != LV_SCALE_NONE) {
                    return 0;
                }
                if (lv_area_get_width(& draw_dsc->image_area) == 0 || lv_area_get_height(& draw_dsc->image_area) == 0)
                	return 0;

                /* not support tile */
                if (draw_dsc->tile) {
                	return 0;
                }

                if (draw_dsc->bitmap_mask_src) {
                	return 0;
                }

                bool masked = draw_dsc->bitmap_mask_src != NULL;
                if (masked)
                	return 0;
                lv_color_format_t cf = (lv_color_format_t) draw_dsc->header.cf;
                if (cf != display_get_lvformat())
                	return 0;
                if (masked && (cf == LV_COLOR_FORMAT_A8 || cf == LV_COLOR_FORMAT_RGB565A8)) {
                    return 0;
                }

                if( cf >= LV_COLOR_FORMAT_PROPRIETARY_START) {
                    return 0;
                }
            }
            break;
#if LV_USE_3DTEXTURE
        case LV_DRAW_TASK_TYPE_3D:
            return 0;
#endif
        default:
        	return 0;
    }

    task->preferred_draw_unit_id = DRAW_UNIT_ID_AWROT;
    task->preference_score = 0;	// похоже, для разбирательств между разными evaluate

    return 0;
}

/**
 * Called to try to assign a draw task to itself.
 * `lv_draw_get_next_available_task` can be used to get an independent draw task.
 * A draw task should be assign only if the draw unit can draw it too
 * @param draw_unit     pointer to the draw unit
 * @param layer         pointer to a layer on which the draw task should be drawn
 * @return              >=0:    The number of taken draw task:
 *                                  0 means the task has not yet been completed.
 *                                  1 means a new task has been accepted.
 *                      -1:     The draw unit wanted to work on a task but couldn't do that
 *                              due to some errors (e.g. out of memory).
 *                              It signals that LVGL should call the dispatcher later again
 *                              to let draw unit try to start the rendering again.
 */
static int32_t awrot_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    LV_PROFILER_DRAW_BEGIN;
    draw_awrot_unit_t * draw_awrot_unit = (draw_awrot_unit_t *) draw_unit;

    lv_draw_task_t * const t = lv_draw_get_available_task(layer, NULL, DRAW_UNIT_ID_AWROT);
    if(t == NULL) {
		//TP();
		LV_PROFILER_DRAW_END;
		return LV_DRAW_UNIT_IDLE;  /*Couldn't start rendering*/
    }

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;

    {
        /*Render the draw task*/
        switch(t->type)
        {
        default:
        	PRINTF("awrot_execute_drawing: t->type=%d\n", (int) t->type);
        	break;
        case LV_DRAW_TASK_TYPE_LAYER:
    	case LV_DRAW_TASK_TYPE_IMAGE:
    		draw_awrot_image(t, (lv_draw_image_dsc_t *) t->draw_dsc, &t->area, layer);
    		break;
//    	case LV_DRAW_TASK_TYPE_FILL:
//    		draw_awrot_fill(t, (lv_draw_fill_dsc_t *) t->draw_dsc, &t->area, layer);
//    		break;
    //	case LV_DRAW_TASK_TYPE_BORDER:
    //		lv_draw_awrot_border(t, t->draw_dsc, &t->area);
    //		break;
    //	case LV_DRAW_TASK_TYPE_BOX_SHADOW:
    //		lv_draw_awrot_box_shadow(t, t->draw_dsc, &t->area);
    //		break;
    //	case LV_DRAW_TASK_TYPE_LETTER:
    //		lv_draw_awrot_letter(t, t->draw_dsc, &t->area);
    //		break;
    //	case LV_DRAW_TASK_TYPE_LABEL:
    //		lv_draw_awrot_label(t, t->draw_dsc, &t->area);
    //		break;
    //	case LV_DRAW_TASK_TYPE_ARC:
    //		lv_draw_awrot_arc(t, t->draw_dsc, &t->area);
    //		break;
    //	case LV_DRAW_TASK_TYPE_LINE:
    //		lv_draw_awrot_line(t, t->draw_dsc);
    //		break;
    //	case LV_DRAW_TASK_TYPE_TRIANGLE:
    //		lv_draw_awrot_triangle(t, t->draw_dsc);
    //		break;
    //	case LV_DRAW_TASK_TYPE_MASK_RECTANGLE:
    //		lv_draw_awrot_mask_rect(t, t->draw_dsc);
    //		break;
    //#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
    //	case LV_DRAW_TASK_TYPE_VECTOR:
    //		lv_draw_awrot_vector(t, t->draw_dsc);
    //		break;
    //#endif
		}
    }
    t->state = LV_DRAW_TASK_STATE_READY;

    /*The draw unit is free now. Request a new dispatching as it can get a new task*/
    lv_draw_dispatch_request();

    LV_PROFILER_DRAW_END;
    return 1;
}
/**
 * Called to signal the unit to complete all tasks in order to return their ready status.
 * This callback can be implemented in case of asynchronous task processing.
 * Below is an example to show the difference between synchronous and asynchronous:
 *
 * Synchronous:
 * LVGL thread              DRAW thread                 HW
 *
 * task1             -->    submit               -->    Receive task1
 *                          wait_for_finish()
 *                   <--    task1->state = READY <--    Complete task1
 * task2             -->    submit               -->    Receive task2
 *                          wait_for_finish()
 *                          task2->state = READY <--    Complete task2
 * task3             -->    submit               -->    Receive task3
 *                          wait_for_finish()
 *                   <--    task3->state = READY <--    Complete task3
 * task4             -->    submit               -->    Receive task4
 *                          wait_for_finish()
 *                   <--    task4->state = READY <--    Complete task4
 * NO MORE TASKS
 *
 *
 * Asynchronous:
 * LVGL thread              DRAW thread                 HW
 *                                                      is IDLE
 * task1             -->    queue task1
 *                          submit               -->    Receive task1
 * task2             -->    queue task2                 is BUSY (with task1)
 * task3             -->    queue task3                 still BUSY (with task1)
 * task4             -->    queue task4                 becomes IDLE
 *                   <--    task1->state = READY <--    Complete task1
 *                          submit               -->    Receive task2, task3, task4
 * NO MORE TASKS
 * wait_for_finish_cb()     wait_for_finish()
 *                                               <--    Complete task2, task3, task4
 *                   <--    task2->state = READY <--
 *                   <--    task3->state = READY <--
 *                   <--    task4->state = READY <--
 *
 * @param draw_unit
 * @return
 */
#if LV_USE_OS
int32_t awrot_wait_for_finish(lv_draw_unit_t * draw_unit)
{
    draw_awg2d_unit_t * draw_awg2d_unit = (draw_awg2d_unit_t *) draw_unit;

	return 0;
}
#endif

static int32_t draw_awrot_delete(lv_draw_unit_t * draw_unit)
{
#if LV_USE_OS
    draw_awrot_unit_t * draw_awrot_unit = (draw_awrot_unit_t *) draw_unit;

    uint32_t i;
    for(i = 0; i < LV_DRAW_AWG2D_DRAW_UNIT_CNT; i++) {
        lv_draw_sw_thread_dsc_t * thread_dsc = &draw_awrot_unit->thread_dscs[i];

        LV_LOG_INFO("cancel software rendering thread");
        thread_dsc->exit_status = true;

        if(thread_dsc->inited) {
            lv_thread_sync_signal(&thread_dsc->sync);
        }
        lv_thread_delete(&thread_dsc->thread);
    }

    return 0;
#else
    LV_UNUSED(draw_unit);
    return 0;
#endif
}

#endif /* defined (G2D_ROT) */

// Add custom draw unit
void lvglhw_initialize(void)
{
#if defined (G2D_MIXER)
	if (0)
	{
		// t113: flicker on LV_BUILD_DEMOS
		PRINTF("lvglhw_initialize: Enable G2D_MIXER hw accelerate for LVGL\n");
		//#if LV_DRAW_SW_COMPLEX == 1
		//    lv_draw_sw_mask_init();
		//#endif

			draw_awg2d_unit_t * draw_awg2d_unit = lv_draw_create_unit(sizeof(draw_awg2d_unit_t));
		    draw_awg2d_unit->base_unit.dispatch_cb = awg2d_dispatch;
		    draw_awg2d_unit->base_unit.evaluate_cb = awg2d_evaluate;
		    //draw_awg2d_unit->base_unit.wait_for_finish_cb = awg2d_wait_for_finish;
		    draw_awg2d_unit->base_unit.delete_cb = LV_USE_OS ? draw_awg2d_delete : NULL;
		    draw_awg2d_unit->base_unit.name = "AWG2D";

		#if LV_USE_OS
		    uint32_t i;
		    for(i = 0; i < LV_DRAW_AWG2D_DRAW_UNIT_CNT; i++) {
		        lv_draw_sw_thread_dsc_t * thread_dsc = &draw_awg2d_unit->thread_dscs[i];
		        thread_dsc->idx = i;
		        thread_dsc->draw_unit = (void *) draw_awg2d_unit;
		        lv_thread_init(&thread_dsc->thread, "swdraw", LV_DRAW_THREAD_PRIO, render_thread_cb,
		                       LV_DRAW_THREAD_STACK_SIZE, thread_dsc);
		    }
		#endif

		#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
		//    if(LV_DRAW_AWG2D_DRAW_UNIT_CNT > 1) {
		//        tvg_engine_init(TVG_ENGINE_SW, LV_DRAW_AWG2D_DRAW_UNIT_CNT);
		//    }
		//    else {
		//        tvg_engine_init(TVG_ENGINE_SW, 0);
		//    }
		#endif

		////    lv_ll_init(&LV_GLOBAL_DEFAULT()->draw_sw_blend_handler_ll, sizeof(lv_draw_sw_custom_blend_handler_t));

	}
#endif /* defined (G2D_MIXER) */
#if defined (G2D_ROT)
	if (1)
	{
		PRINTF("lvglhw_initialize: Enable G2D_ROT hw accelerate for LVGL\n");
		// Блок позволяет копировать прямоугольники без изменения формата и размеров,
		// возможен поворот на углы кратные 90 градусам

		//#if LV_DRAW_SW_COMPLEX == 1
		//    lv_draw_sw_mask_init();
		//#endif

			draw_awrot_unit_t * draw_awrot_unit = lv_draw_create_unit(sizeof(draw_awrot_unit_t));
		    draw_awrot_unit->base_unit.dispatch_cb = awrot_dispatch;
		    draw_awrot_unit->base_unit.evaluate_cb = awrot_evaluate;
		    //draw_awrot_unit->base_unit.wait_for_finish_cb = awrot_wait_for_finish;
		    draw_awrot_unit->base_unit.delete_cb = LV_USE_OS ? draw_awrot_delete : NULL;
		    draw_awrot_unit->base_unit.name = "AWROT";

		#if LV_USE_OS
		    uint32_t i;
		    for(i = 0; i < LV_DRAW_AWG2D_DRAW_UNIT_CNT; i++) {
		        lv_draw_sw_thread_dsc_t * thread_dsc = &draw_awrot_unit->thread_dscs[i];
		        thread_dsc->idx = i;
		        thread_dsc->draw_unit = (void *) draw_awrot_unit;
		        lv_thread_init(&thread_dsc->thread, "swdraw", LV_DRAW_THREAD_PRIO, render_thread_cb,
		                       LV_DRAW_THREAD_STACK_SIZE, thread_dsc);
		    }
		#endif

		#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
		//    if(LV_DRAW_AWG2D_DRAW_UNIT_CNT > 1) {
		//        tvg_engine_init(TVG_ENGINE_SW, LV_DRAW_AWG2D_DRAW_UNIT_CNT);
		//    }
		//    else {
		//        tvg_engine_init(TVG_ENGINE_SW, 0);
		//    }
		#endif

		////    lv_ll_init(&LV_GLOBAL_DEFAULT()->draw_sw_blend_handler_ll, sizeof(lv_draw_sw_custom_blend_handler_t));


	}
#endif /* defined (G2D_ROT) */
}

void draw_awg2d_deinit(void)
{
#if LV_USE_VECTOR_GRAPHIC && LV_USE_THORVG
    tvg_engine_term(TVG_ENGINE_SW);
#endif

#if LV_DRAW_SW_COMPLEX == 1
    ////lv_draw_sw_mask_deinit();
#endif
}

#endif /* WITHLVGL */

#endif /* WITHMDMAHW */


#if ! (LCDMODE_DUMMY)


#if WITHLVGL

uint32_t display_get_lvformat(void)
{
#if LCDMODE_LTDC
	#if LCDMODE_LTDC_L24
    	return LV_COLOR_FORMAT_NATIVE;
	#elif LCDMODE_PALETTE256
    	return LV_COLOR_FORMAT_L8;
	#elif LCDMODE_ARGB8888
    	return LV_COLOR_FORMAT_ARGB8888;
	#elif LCDMODE_RGB565
    	return LV_COLOR_FORMAT_RGB565;
	#endif
#else /* LCDMODE_LTDC */
    	return LV_COLOR_FORMAT_ARGB8888;
#endif /* LCDMODE_LTDC */
}
#endif /* WITHLVGL */

#if LCDMODE_PIXELSIZE == 1
// Функция получает координаты и работает над буфером в горизонтальной ориентации.
static void
hwaccel_rect_u8(
	uintptr_t dstinvalidateaddr,	// параметры invalidate получателя
	int_fast32_t dstinvalidatesize,
	uint8_t * __restrict buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	uint_fast8_t color	// цвет
	)
{
	if (w == 0 || h == 0)
		return;
	enum { PIXEL_SIZE = sizeof * buffer };
	enum { PIXEL_SIZE_CODE = 0 };

#if WITHMDMAHW && (CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)
	// MDMA implementation

	//static ALIGNX_BEGIN volatile uint8_t tgcolor [(DCACHEROWSIZE + sizeof (uint8_t) - 1) / sizeof (uint8_t)] ALIGNX_END;	/* значение цвета для заполнения области памяти */
	//tgcolor [0] = color;
	MDMA_DATA = color;	// регистр выделенного канала MDMA используется для хранения значение цвета. Переиферия не кэшируется.
	(void) MDMA_DATA;

	//dcache_clean((uintptr_t) & tgcolor, sizeof tgcolor);
	dcache_clean_invalidate((uintptr_t) buffer, PIXEL_SIZE * GXSIZE(dx, dy));

	MDMA_CH->CDAR = (uintptr_t) buffer; // dest address
	MDMA_CH->CSAR = (uintptr_t) & MDMA_DATA;
	const uint_fast32_t tlen = mdma_tlen(w * PIXEL_SIZE, PIXEL_SIZE);
	const uint_fast32_t sbus = mdma_getbus(MDMA_CH->CSAR);
	const uint_fast32_t dbus = mdma_getbus(MDMA_CH->CDAR);
	const uint_fast32_t sinc = 0x00; // Source increment mode: 00: Source address pointer is fixed
	const uint_fast32_t dinc = 0x02; // Destination increment mode: 10: Destination address pointer is incremented
	const uint_fast32_t sburst = mdma_getburst(tlen, sbus, sinc);
	const uint_fast32_t dburst = mdma_getburst(tlen, dbus, dinc);
	MDMA_CH->CTCR =
		(sinc << MDMA_CTCR_SINC_Pos) | 	// Source increment mode: 00: Source address pointer is fixed
		(PIXEL_SIZE_CODE << MDMA_CTCR_SSIZE_Pos) |
		(PIXEL_SIZE_CODE << MDMA_CTCR_SINCOS_Pos) |
		(sburst << MDMA_CTCR_SBURST_Pos) |
		(dinc << MDMA_CTCR_DINC_Pos) |	// Destination increment mode: 10: Destination address pointer is incremented
		(PIXEL_SIZE_CODE << MDMA_CTCR_DSIZE_Pos) |
		(PIXEL_SIZE_CODE << MDMA_CTCR_DINCOS_Pos) |
		(dburst << MDMA_CTCR_DBURST_Pos) |	// Destination burst transfer configuration
		((tlen - 1) << MDMA_CTCR_TLEN_Pos) |		// buffer Transfer Length (number of bytes - 1)
		(0x00uL << MDMA_CTCR_PKE_Pos) |
		(0x00uL << MDMA_CTCR_PAM_Pos) |
		(0x02uL << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01uL << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01uL << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((PIXEL_SIZE * (w)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00uL << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00uL << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((h - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((PIXEL_SIZE * (0)) << MDMA_CBRUR_SUV_Pos) |				// Source address Update Value
		((PIXEL_SIZE * (GXADJ(dx) - w)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(sbus << MDMA_CTBR_SBUS_Pos) |
		(dbus << MDMA_CTBR_DBUS_Pos) |
		0;

	mdma_startandwait();

#else /* WITHMDMAHW */
	// программная реализация

	const size_t t = GXADJ(dx);
	while (h --)
	{
		memset(buffer, color, w);
		buffer += t;
	}

#endif /* WITHMDMAHW */
}

#endif /* LCDMODE_PIXELSIZE == 1 */
//
//uint32_t mixer_get_irq(void){
//	uint32_t reg_val = 0;
//
//	reg_val = read_wvalue(G2D_STATUS_REG);
//
//	return reg_val;
//}
//
//uint32_t mixer_get_irq0(void){
//	uint32_t reg_val = 0;
//
//	reg_val = read_wvalue(G2D_CMDQ_STS_REG);
//
//	return reg_val;
//}
//
//uint32_t mixer_clear_init(void){
//
//	G2D_V0->G2D_STATUS_REG, 0x300);
//	G2D_V0->G2D_CONTROL_REG, 0x0);
//
//	return 0;
//}
//
//uint32_t mixer_clear_init0(void){
//
//	G2D_V0->G2D_CMDQ_STS_REG, 0x100);
//	G2D_V0->G2D_CMDQ_CTL_REG, 0x0);
//
//	return 0;
//}

#if LCDMODE_PIXELSIZE == 2

// Функция получает координаты и работает над буфером в горизонтальной ориентации.
static void
hwaccel_rect_u16(
	uintptr_t dstinvalidateaddr,	// параметры invalidate получателя
	int_fast32_t dstinvalidatesize,
	uint16_t * __restrict buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	COLORPIP_T color	// цвет
	)
{
	if (w == 0 || h == 0)
		return;
	enum { PIXEL_SIZE = sizeof * buffer };
	enum { PIXEL_SIZE_CODE = 1 };

#if WITHMDMAHW && (CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)
	// MDMA implementation

	//static ALIGNX_BEGIN volatile uint16_t tgcolor [DCACHEROWSIZE / sizeof (uint16_t)] ALIGNX_END;	/* значение цвета для заполнения области памяти */
	//tgcolor [0] = color;
	MDMA_DATA = color;	// регистр выделенного канала MDMA используется для хранения значение цвета. Переиферия не кэшируется.
	(void) MDMA_DATA;

	//dcache_clean((uintptr_t) & tgcolor, sizeof tgcolor);
	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);

	MDMA_CH->CDAR = (uintptr_t) buffer; // dest address
	MDMA_CH->CSAR = (uintptr_t) & MDMA_DATA;
	const uint_fast32_t tlen = mdma_tlen(w * PIXEL_SIZE, PIXEL_SIZE);
	const uint_fast32_t sbus = mdma_getbus(MDMA_CH->CSAR);
	const uint_fast32_t dbus = mdma_getbus(MDMA_CH->CDAR);
	const uint_fast32_t sinc = 0x00; // Source increment mode: 00: Source address pointer is fixed
	const uint_fast32_t dinc = 0x02; // Destination increment mode: 10: Destination address pointer is incremented
	const uint_fast32_t sburst = mdma_getburst(tlen, sbus, sinc);
	const uint_fast32_t dburst = mdma_getburst(tlen, dbus, dinc);
	MDMA_CH->CTCR =
		(sinc << MDMA_CTCR_SINC_Pos) | 	// Source increment mode: 00: Source address pointer is fixed
		(PIXEL_SIZE_CODE << MDMA_CTCR_SSIZE_Pos) |
		(PIXEL_SIZE_CODE << MDMA_CTCR_SINCOS_Pos) |
		(sburst << MDMA_CTCR_SBURST_Pos) |
		(dinc << MDMA_CTCR_DINC_Pos) |	// Destination increment mode: 10: Destination address pointer is incremented
		(PIXEL_SIZE_CODE << MDMA_CTCR_DSIZE_Pos) |
		(PIXEL_SIZE_CODE << MDMA_CTCR_DINCOS_Pos) |
		(dburst << MDMA_CTCR_DBURST_Pos) |	// Destination burst transfer configuration
		((tlen - 1) << MDMA_CTCR_TLEN_Pos) |		// buffer Transfer Length (number of bytes - 1)
		(0x00uL << MDMA_CTCR_PKE_Pos) |
		(0x00uL << MDMA_CTCR_PAM_Pos) |
		(0x02uL << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01uL << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01uL << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((PIXEL_SIZE * (w)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00uL << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00uL << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((h - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((PIXEL_SIZE * (0)) << MDMA_CBRUR_SUV_Pos) |				// Source address Update Value
		((PIXEL_SIZE * (GXADJ(dx) - w)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(sbus << MDMA_CTBR_SBUS_Pos) |
		(dbus << MDMA_CTBR_DBUS_Pos) |
		0;

	//PRINTF("MDMA_CH->CDAR=%08X,dbus=%d, MDMA_CH->CSAR=%08X,sbus=%d, tlen=%u, BNDT-%08lX\n", MDMA_CH->CDAR, dbus, MDMA_CH->CSAR, sbus, tlen, (MDMA_CH->CBNDTR & MDMA_CBNDTR_BNDT_Msk) >> MDMA_CBNDTR_BNDT_Pos);
	mdma_startandwait();

#elif WITHDMA2DHW
	// DMA2D implementation

	// just writes the color defined in the DMA2D_OCOLR register
	// to the area located at the address pointed by the DMA2D_OMAR
	// and defined in the DMA2D_NLR and DMA2D_OOR.

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);

	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) buffer;
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((GXADJ(dx) - w) << DMA2D_OOR_LO_Pos) |
		0;

	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		(h << DMA2D_NLR_NL_Pos) |
		(w << DMA2D_NLR_PL_Pos) |
		0;

	DMA2D->OCOLR =
		color |
		0;

	DMA2D->OPFCCR = (DMA2D->OPFCCR & ~ (DMA2D_OPFCCR_CM)) |
		(2 * DMA2D_OPFCCR_CM_0) |	/* 010: RGB565 Color mode - framebuffer pixel format */
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		3 * DMA2D_CR_MODE_0 |	// 11: Register-to-memory (no FG nor BG, only output stage active)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;
	__DMB();

	ASSERT((DMA2D->ISR & DMA2D_ISR_CEIF) == 0);	// Configuration Error
	ASSERT((DMA2D->ISR & DMA2D_ISR_TEIF) == 0);	// Transfer Error

#elif WITHMDMAHW && CPUSTYLE_ALLWINNER
	/* Использование G2D для формирования изображений */

	if (w == 1)
	{
		/* Горизонтальные линии в один пиксель рисовать умеет аппаратура. */
		/* программная реализация отрисовки вертикальной линии в один пиксель */
		softfill(buffer, dx, w, h, color);	// программная реализация
		return;
	}

	const unsigned tstride = GXADJ(dx) * PIXEL_SIZE;
	const uintptr_t taddr = (uintptr_t) buffer;
	const uint_fast32_t tsizehw = ((h - 1) << 16) | ((w - 1) << 0);

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	hwaccel_fillrect(buffer, dx, taddr, tstride, tsizehw, COLORPIP_A(color),  COLOR24(COLORPIP_R(color), COLORPIP_G(color), COLORPIP_B(color)), w, h, color, FILL_FLAG_NONE);

#else /* WITHMDMAHW, WITHDMA2DHW */

	softfill(buffer, dx, w, h, color);	// программная реализация

#endif /* WITHMDMAHW, WITHDMA2DHW */
}

#endif /* LCDMODE_PIXELSIZE == 2 */

#if LCDMODE_PIXELSIZE == 3
// Функция получает координаты и работает над буфером в горизонтальной ориентации.
static void
hwaccel_rect_u24(
	uintptr_t dstinvalidateaddr,	// параметры invalidate получателя
	int_fast32_t dstinvalidatesize,
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	uint_fast32_t color	// цвет
	)
{
	if (w == 0 || h == 0)
		return;
	enum { PIXEL_SIZE = sizeof * buffer };
	//enum { PIXEL_SIZE_CODE = 1 };

	ASSERT(sizeof (* buffer) == 3);

#if 0 && WITHMDMAHW && (CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)
	// MDMA implementation

	//static ALIGNX_BEGIN volatile PACKEDCOLORPIP_T tgcolor [(DCACHEROWSIZE + sizeof (PACKEDCOLORPIP_T) - 1) / sizeof (PACKEDCOLORPIP_T)] ALIGNX_END;	/* значение цвета для заполнения области памяти */
	//tgcolor [0] = color;
	MDMA_DATA = color;	// регистр выделенного канала MDMA используется для хранения значение цвета. Переиферия не кэшируется.
	(void) MDMA_DATA;
	#error MDMA implementation need

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);

	MDMA_CH->CDAR = (uintptr_t) buffer; // dest address
	MDMA_CH->CSAR = (uintptr_t) & MDMA_DATA;
	const uint_fast32_t tlen = mdma_tlen(w * PIXEL_SIZE, PIXEL_SIZE);
	const uint_fast32_t sbus = mdma_getbus(MDMA_CH->CSAR);
	const uint_fast32_t dbus = mdma_getbus(MDMA_CH->CDAR);
	const uint_fast32_t sinc = 0x00; // Source increment mode: 00: Source address pointer is fixed
	const uint_fast32_t dinc = 0x02; // Destination increment mode: 10: Destination address pointer is incremented
	const uint_fast32_t sburst = mdma_getburst(tlen, sbus, sinc);
	const uint_fast32_t dburst = mdma_getburst(tlen, dbus, dinc);
	MDMA_CH->CTCR =
		(sinc << MDMA_CTCR_SINC_Pos) | 	// Source increment mode: 00: Source address pointer is fixed
		(PIXEL_SIZE_CODE << MDMA_CTCR_SSIZE_Pos) |
		(PIXEL_SIZE_CODE << MDMA_CTCR_SINCOS_Pos) |
		(sburst << MDMA_CTCR_SBURST_Pos) |
		(dinc << MDMA_CTCR_DINC_Pos) |	// Destination increment mode: 10: Destination address pointer is incremented
		(PIXEL_SIZE_CODE << MDMA_CTCR_DSIZE_Pos) |
		(PIXEL_SIZE_CODE << MDMA_CTCR_DINCOS_Pos) |
		(dburst << MDMA_CTCR_DBURST_Pos) |	// Destination burst transfer configuration
		((tlen - 1) << MDMA_CTCR_TLEN_Pos) |		// buffer Transfer Length (number of bytes - 1)
		(0x00uL << MDMA_CTCR_PKE_Pos) |
		(0x00uL << MDMA_CTCR_PAM_Pos) |
		(0x02uL << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01uL << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01uL << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((PIXEL_SIZE * (w)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00uL << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00uL << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((h - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((PIXEL_SIZE * (0)) << MDMA_CBRUR_SUV_Pos) |				// Source address Update Value
		((PIXEL_SIZE * (GXADJ(dx) - w)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(sbus << MDMA_CTBR_SBUS_Pos) |
		(dbus << MDMA_CTBR_DBUS_Pos) |
		0;

	mdma_startandwait();

#elif WITHDMA2DHW
	// DMA2D implementation

	// just writes the color defined in the DMA2D_OCOLR register
	// to the area located at the address pointed by the DMA2D_OMAR
	// and defined in the DMA2D_NLR and DMA2D_OOR.

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);

	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) buffer;
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((GXADJ(dx) - w) << DMA2D_OOR_LO_Pos) |
		0;

	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		(h << DMA2D_NLR_NL_Pos) |
		(w << DMA2D_NLR_PL_Pos) |
		0;

	DMA2D->OCOLR =
		color |
		0;

	DMA2D->OPFCCR = (DMA2D->OPFCCR & ~ (DMA2D_OPFCCR_CM)) |
		(1 * DMA2D_OPFCCR_CM_0) |	/* 001: RGB888 Color mode - framebuffer pixel format */
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		3 * DMA2D_CR_MODE_0 |	// 11: Register-to-memory (no FG nor BG, only output stage active)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;
	__DMB();

	ASSERT((DMA2D->ISR & DMA2D_ISR_CEIF) == 0);	// Configuration Error
	ASSERT((DMA2D->ISR & DMA2D_ISR_TEIF) == 0);	// Transfer Error

#else

	softfill(buffer, dx, w, h, color);	// программная реализация

#endif
}

#endif /* LCDMODE_PIXELSIZE == 3 */

#if LCDMODE_PIXELSIZE == 4
//#define FILL_FLAG_NONE		0x00
//#define FILL_FLAG_MIXBG		0x01	// alpha со старым содержимым буферв
// Функция получает координаты и работает над буфером в горизонтальной ориентации.
static void
hwaccel_rect_u32(
	uintptr_t dstinvalidateaddr,	// параметры invalidate получателя
	int_fast32_t dstinvalidatesize,
	uint32_t * __restrict buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	COLORPIP_T color,	// цвет
	unsigned fillmask
	)
{
	if (w == 0 || h == 0)
		return;
	enum { PIXEL_SIZE = sizeof * buffer };
	enum { PIXEL_SIZE_CODE = 2 };	// word (32-bit)

#if WITHMDMAHW && (CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)
	// MDMA implementation

	//static ALIGNX_BEGIN volatile uint32_t tgcolor [(DCACHEROWSIZE + sizeof (uint32_t) - 1) / sizeof (uint32_t)] ALIGNX_END;	/* значение цвета для заполнения области памяти */
	//tgcolor [0] = color;
	MDMA_DATA = color;	// регистр выделенного канала MDMA используется для хранения значение цвета. Переиферия не кэшируется.
	(void) MDMA_DATA;

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);

	MDMA_CH->CDAR = (uintptr_t) buffer; // dest address
	MDMA_CH->CSAR = (uintptr_t) & MDMA_DATA;
	const uint_fast32_t tlen = mdma_tlen(w * PIXEL_SIZE, PIXEL_SIZE);
	const uint_fast32_t sbus = mdma_getbus(MDMA_CH->CSAR);
	const uint_fast32_t dbus = mdma_getbus(MDMA_CH->CDAR);
	const uint_fast32_t sinc = 0x00; // Source increment mode: 00: Source address pointer is fixed
	const uint_fast32_t dinc = 0x02; // Destination increment mode: 10: Destination address pointer is incremented
	const uint_fast32_t sburst = mdma_getburst(tlen, sbus, sinc);
	const uint_fast32_t dburst = mdma_getburst(tlen, dbus, dinc);
	MDMA_CH->CTCR =
		(sinc << MDMA_CTCR_SINC_Pos) | 	// Source increment mode: 00: Source address pointer is fixed
		(PIXEL_SIZE_CODE << MDMA_CTCR_SSIZE_Pos) |
		(PIXEL_SIZE_CODE << MDMA_CTCR_SINCOS_Pos) |
		(sburst << MDMA_CTCR_SBURST_Pos) |
		(dinc << MDMA_CTCR_DINC_Pos) |	// Destination increment mode: 10: Destination address pointer is incremented
		(PIXEL_SIZE_CODE << MDMA_CTCR_DSIZE_Pos) |
		(PIXEL_SIZE_CODE << MDMA_CTCR_DINCOS_Pos) |
		(dburst << MDMA_CTCR_DBURST_Pos) |	// Destination burst transfer configuration
		((tlen - 1) << MDMA_CTCR_TLEN_Pos) |		// buffer Transfer Length (number of bytes - 1)
		(0x00uL << MDMA_CTCR_PKE_Pos) |
		(0x00uL << MDMA_CTCR_PAM_Pos) |
		(0x02uL << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01uL << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01uL << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((PIXEL_SIZE * (w)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00uL << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00uL << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((h - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((PIXEL_SIZE * (0)) << MDMA_CBRUR_SUV_Pos) |				// Source address Update Value
		((PIXEL_SIZE * (GXADJ(dx) - w)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(sbus << MDMA_CTBR_SBUS_Pos) |
		(dbus << MDMA_CTBR_DBUS_Pos) |
		0;

	mdma_startandwait();

#elif WITHDMA2DHW
	// DMA2D implementation

	// just writes the color defined in the DMA2D_OCOLR register
	// to the area located at the address pointed by the DMA2D_OMAR
	// and defined in the DMA2D_NLR and DMA2D_OOR.

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);

	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) buffer;
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((GXADJ(dx) - w) << DMA2D_OOR_LO_Pos) |
		0;

	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		(h << DMA2D_NLR_NL_Pos) |
		(w << DMA2D_NLR_PL_Pos) |
		0;

	DMA2D->OCOLR =
		color |
		0;

	DMA2D->OPFCCR = (DMA2D->OPFCCR & ~ (DMA2D_OPFCCR_CM)) |
		(2 * DMA2D_OPFCCR_CM_0) |	/* 010: RGB565 Color mode - framebuffer pixel format */
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		3 * DMA2D_CR_MODE_0 |	// 11: Register-to-memory (no FG nor BG, only output stage active)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;
	__DMB();

	ASSERT((DMA2D->ISR & DMA2D_ISR_CEIF) == 0);	// Configuration Error
	ASSERT((DMA2D->ISR & DMA2D_ISR_TEIF) == 0);	// Transfer Error

#elif WITHGPUHW && 0

	const uintptr_t taddr = (uintptr_t) buffer;
	const unsigned tstride = GXADJ(dx) * PIXEL_SIZE;
	const uint_fast32_t tsizehw = ((h - 1) << 16) | ((w - 1) << 0);

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	gpu_fillrect(buffer, dx, taddr, tstride, tsizehw, COLORPIP_A(color), (color & 0xFFFFFF), w, h, color);

#elif WITHMDMAHW && CPUSTYLE_ALLWINNER
	/* Использование G2D для формирования изображений */

	if (w == 1)
	{
		/* программная реализация отрисовки вертикальной линии в один пиксель */
		const unsigned t = GXADJ(dx);
		while (h --)
		{
			* buffer = color;
			buffer += t;
		}
		return;
	}

	const uintptr_t taddr = (uintptr_t) buffer;
	const unsigned tstride = GXADJ(dx) * PIXEL_SIZE;
	const uint_fast32_t tsizehw = ((h - 1) << 16) | ((w - 1) << 0);

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	hwaccel_fillrect(buffer, dx, taddr, tstride, tsizehw, COLORPIP_A(color), (color & 0xFFFFFF), w, h, color, fillmask);

#else /* WITHMDMAHW, WITHDMA2DHW */

	softfill(buffer, dx, w, h, color);	// программная реализация

#endif /* WITHMDMAHW, WITHDMA2DHW */
}

#endif /* LCDMODE_PIXELSIZE == 4 */

// получить адрес требуемой позиции в буфере
PACKEDCOLORPIP_T *
colpip_mem_at_debug(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * file,
	int line
	)
{
	if (x >= db->dx || y >= db->dy || db->buffer == NULL)
	{
		PRINTF("colpip_mem_at(%s/%d): dx=%u, dy=%u, x=%d, y=%d, savestring='%s', savewhere='%s'\n", file, line, db->dx, db->dy, x, y, savestring, savewhere);
	}
	ASSERT(x < db->dx);
	ASSERT(y < db->dy);
	ASSERT(db->buffer != NULL);

	return & db->buffer [y * GXADJ(db->dx) + x];
}

// получить адрес требуемой позиции в буфере
const PACKEDCOLORPIP_T *
colpip_const_mem_at_debug(
	const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * file,
	int line
	)
{
	if (x >= db->dx || y >= db->dy || db->buffer == NULL)
	{
		PRINTF("colpip_const_mem_at_debug(%s/%d): dx=%u, dy=%u, x=%d, y=%d, savestring='%s', savewhere='%s'\n", file, line, db->dx, db->dy, x, y, savestring, savewhere);
	}
	ASSERT(x < db->dx);
	ASSERT(y < db->dy);
	ASSERT(db->buffer != NULL);

	return & db->buffer [y * GXADJ(db->dx) + x];
}


/// Нарисовать вертикальную цветную полосу
// Формат RGB565
void
colpip_xor_vline(
	const gxdrawb_t * db,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row0,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	COLORPIP_T color
	)
{
	while (h --)
		colpip_point_xor(db, col, row0 ++, color);
}

// Нарисовать вертикальную цветную полосу
// Формат RGB565
void
colpip_set_vline(
	const gxdrawb_t * db,
	uint_fast16_t col,	// горизонтальная координата начального пикселя (0..dx-1) слева направо
	uint_fast16_t row0,	// вертикальная координата начального пикселя (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	COLORPIP_T color
	)
{
	/* рисуем прямоугольник шириной в 1 пиксель */
	//colpip_fillrect(db, col, row0, 1, h, color);
	while (h --)
		colpip_point(db, col, row0 ++, color);
}

// Нарисовать горизонтальную цветную полосу
// Формат RGB565
void
colpip_set_hline(
	const gxdrawb_t * db,
	uint_fast16_t col0,	// горизонтальная координата начального пикселя (0..dx-1) слева направо
	uint_fast16_t row0,	// вертикальная координата начального пикселя (0..dy-1) сверху вниз
	uint_fast16_t w,	// ширина
	COLORPIP_T color
	)
{
	/* рисуем прямоугольник высотой в 1 пиксель */
	//colpip_fillrect(db, col0, row0, w, 1, color);
	while (w --)
		colpip_point(db, col0 ++, row0, color);
}

uint_fast8_t colpip_hasalpha(void)
{
#if (CPUSTYLE_T113 || CPUSTYLE_F133) && WITHMDMAHW && LCDMODE_ARGB8888
	return 1;
#else
	return 0;
#endif
}

// заполнение прямоугольной области в видеобуфере
void colpip_fillrect(
	const gxdrawb_t * db,
	uint_fast16_t x,	// начальная координата
	uint_fast16_t y,	// начальная координата
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	COLORPIP_T color	// цвет
	)
{
	colpip_rectangle(db, x, y, w, h, color, FILL_FLAG_NONE);
}

//#define FILL_FLAG_NONE		0x00
//#define FILL_FLAG_MIXBG		0x01	// alpha со старым содержимым буферв

// заполнение прямоугольной области в видеобуфере
void colpip_rectangle(
	const gxdrawb_t * db,
	uint_fast16_t x,	// начальная координата
	uint_fast16_t y,	// начальная координата
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	COLORPIP_T color,
	unsigned fillmask
	)
{
	//PACKEDCOLORPIP_T * const buffer = db->buffer;
	const uint_fast16_t dx = db->dx;
	const uint_fast16_t dy = db->dy;
	ASSERT(x < dx);
	ASSERT((x + w) <= dx);
	ASSERT(y < dy);
	ASSERT((y + h) <= dy);
	PACKEDCOLORPIP_T * const tgr = colpip_mem_at(db, x, y);
	const uintptr_t dstinvalidateaddr = (uintptr_t) db->buffer;	// параметры invalidate получателя
	const int_fast32_t dstinvalidatesize = GXSIZE(dx, dy) * sizeof (PACKEDCOLORPIP_T);

#if LCDMODE_PALETTE256
	hwaccel_rect_u8(dstinvalidateaddr, dstinvalidatesize, tgr, dx, dy, w, h, color);

#elif LCDMODE_RGB565
	hwaccel_rect_u16(dstinvalidateaddr, dstinvalidatesize, tgr, dx, dy, w, h, color);

#elif LCDMODE_MAIN_L24
	hwaccel_rect_u24(dstinvalidateaddr, dstinvalidatesize, tgr, dx, dy, w, h, color);

#elif LCDMODE_ARGB8888
	hwaccel_rect_u32(dstinvalidateaddr, dstinvalidatesize, tgr, dx, dy, w, h, color, fillmask);

#endif
}

// копирование с поворотом
void colpip_copyrotate(
	uintptr_t dstinvalidateaddr,	int_fast32_t dstinvalidatesize,	// параметры clean invalidate получателя
	const gxdrawb_t * tdb, // target buffer
	uint_fast16_t x,	// начальная координата
	uint_fast16_t y,	// начальная координата
	uintptr_t srcinvalidateaddr,	int_fast32_t srcinvalidatesize,	// параметры clean источника
	const gxdrawb_t * sdb,	// source buffer
	uint_fast16_t sx,	// начальная координата
	uint_fast16_t sy,	// начальная координата
	uint_fast16_t w, uint_fast16_t h,	// source rectangle size
	uint_fast8_t mx,	// X mirror flag
	uint_fast8_t my,	// X mirror flag
	unsigned angle	// positive CCW angle
	)
{
	if (w == 0 || h == 0)
		return;
	enum { PIXEL_SIZE = sizeof (PACKEDCOLORPIP_T) };
//	enum { PIXEL_SIZE_CODE = 1 };

#if WITHMDMAHW && CPUSTYLE_ALLWINNER
	const uintptr_t saddr = (uintptr_t) colpip_const_mem_at(tdb, sx, sy);
	const unsigned tstride = GXADJ(tdb->dx) * PIXEL_SIZE;
	const unsigned sstride = GXADJ(sdb->dx) * PIXEL_SIZE;
	const uintptr_t taddr = (uintptr_t) colpip_mem_at(tdb, x, y);
	// target size для 4-х квадрантов
	// похоже, поворот учитывать не требуется. Но просто для "красоты" оставлю четыре варианта.
	const uint_fast32_t tsizehw4 [4] =
	{
		((h - 1) << 16) | ((w - 1) << 0),	// target size if 0 CCW
		((w - 1) << 16) | ((h - 1) << 0),	// target size if 90 CCW
		((h - 1) << 16) | ((w - 1) << 0),	// target size if 180 CCW
		((w - 1) << 16) | ((h - 1) << 0),	// target size if 270 CCW
	};

	const unsigned quadrant = (angle % 360) / 90;
	const uint_fast32_t ssizehw = ((h - 1) << 16) | ((w - 1) << 0); // source size
	const uint_fast32_t tsizehw = tsizehw4 [quadrant];
	uint_fast32_t rot_ctl = 0;
	rot_ctl |= !! mx * (UINT32_C(1) << 7);	// flip horizontal
	rot_ctl |= !! my * (UINT32_C(1) << 6);	// flip vertical
	rot_ctl |= ((0 - quadrant) & 0x03) * (UINT32_C(1) << 4);	// rotate (0: 0deg, 1: 90deg CW, 2: 180deg CW, 3: 270deg CW)

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	dcache_clean(srcinvalidateaddr, srcinvalidatesize);
	hwaccel_rotcopy(saddr, sstride, ssizehw, taddr, tstride, tsizehw, rot_ctl);

#endif /* WITHMDMAHW && CPUSTYLE_ALLWINNER */
}

void colpip_putpixel(const gxdrawb_t * db,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	)
{
	PACKEDCOLORPIP_T * const tgr = colpip_mem_at(db, x, y);
	#if LCDMODE_LTDC_L24
		tgr->r = color >> 16;
		tgr->g = color >> 8;
		tgr->b = color >> 0;
	#else /* LCDMODE_LTDC_L24 */
		* tgr = color;
	#endif /* LCDMODE_LTDC_L24 */
}

/*-----------------------------------------------------  V_Bre
 * void V_Bre (int xn, int yn, int xk, int yk)
 *
 * Подпрограмма иллюстрирующая построение вектора из точки
 * (xn,yn) в точку (xk, yk) методом Брезенхема.
 *
 * Построение ведется от точки с меньшими  координатами
 * к точке с большими координатами с единичным шагом по
 * координате с большим приращением.
 *
 * В общем случае исходный вектор проходит не через вершины
 * растровой сетки, а пересекает ее стороны.
 * Пусть приращение по X больше приращения по Y и оба они > 0.
 * Для очередного значения X нужно выбрать одну двух ближайших
 * координат сетки по Y.
 * Для этого проверяется как проходит  исходный  вектор - выше
 * или ниже середины расстояния между ближайшими значениями Y.
 * Если выше середины,  то Y-координату  надо  увеличить на 1,
 * иначе оставить прежней.
 * Для этой проверки анализируется знак переменной s,
 * соответствующей разности между истинным положением и
 * серединой расстояния между ближайшими Y-узлами сетки.
 */

void colpip_line(
	const gxdrawb_t * db,
	int xn, int yn,
	int xk, int yk,
	COLORPIP_T color,
	int antialiasing
	)
{
	int  dx, dy, s, sx, sy, kl, incr1, incr2;
	char swap;
	const COLORPIP_T sc = getshadedcolor(color, DEFAULT_ALPHA);

	/* Вычисление приращений и шагов */
	if ((dx = xk - xn) < 0)
	{
		dx = - dx;
		sx = - 1;
	}
	else if (dx > 0)
		sx = + 1;
	else
		sx = 0;

	if ((dy = yk - yn) < 0)
	{
		dy = - dy;
		sy = - 1;
	}
	else if (dy > 0)
		sy = + 1;
	else
		sy = 0;

	/* Учет наклона */
	if ((kl = dx) < (s = dy))
	{
		dx = s;  dy = kl;  kl = s; swap = 1;
	}
	else
	{
		swap = 0;
	}

	s = (incr1 = 2 * dy) - dx; /* incr1 - констан. перевычисления */
	/* разности если текущее s < 0  и  */
	/* s - начальное значение разности */
	incr2 = 2 * dx;         /* Константа для перевычисления    */
	/* разности если текущее s >= 0    */
	colpip_putpixel(db, xn, yn, color); /* Первый  пиксел вектора       */

	/*static */ uint_fast16_t xold, yold;
	xold = xn;
	yold = yn;
	while (-- kl >= 0)
	{
		if (s >= 0)
		{
			if (swap)
				xn += sx;
			else
				yn += sy;
			s-= incr2;
		}
		if (swap)
			yn += sy;
		else
			xn += sx;
		s += incr1;

		colpip_putpixel(db, xn, yn, color); /* Текущая  точка  вектора   */

		if (antialiasing)
		{
			if (((xold == xn - 1) || (xold == xn + 1)) && ((yold == yn - 1) || (yold == yn + 1)))
			{
				if (color != * colpip_mem_at(db, xn, yold))
					colpip_putpixel(db, xn, yold, sc);

				if (color != * colpip_mem_at(db, xold, yn))
					colpip_putpixel(db, xold, yn, sc);
//				colpip_putpixel(buffer, bx, by, xn, yn, sc);		// нужны дополнительные цвета для этих 2х точек
//				colpip_putpixel(buffer, bx, by, xold, yold, sc);
			}
			xold = xn;
			yold = yn;
		}
	}
}  /* V_Bre */


/* заливка замкнутого контура */
void display_floodfill(
	const gxdrawb_t * db,
	uint_fast16_t x,	// начальная координата
	uint_fast16_t y,	// начальная координата
	COLORPIP_T newColor,
	COLORPIP_T oldColor
	)
{
	PACKEDCOLORPIP_T * const buffer = db->buffer;
	const uint_fast16_t dx = db->dx;
	const uint_fast16_t dy = db->dy;
	ASSERT(x < dx);
	ASSERT(y < dy);
	PACKEDCOLORPIP_T * tgr = colpip_mem_at(db, x, y);

	if (* tgr == oldColor && * tgr != newColor)
	{
		* tgr = newColor;
		display_floodfill(db, x + 1, y, newColor, oldColor);
		display_floodfill(db, x - 1, y, newColor, oldColor);
		display_floodfill(db, x, y + 1, newColor, oldColor);
		display_floodfill(db, x, y - 1, newColor, oldColor);
	}
}

// Заполнение буфера сполшным цветом
// Эта функция используется только в тесте
void colpip_fill(
	const gxdrawb_t * db,
	COLORPIP_T color
	)
{
	PACKEDCOLORPIP_T * const buffer = db->buffer;
	const uint_fast16_t dx = db->dx;
	const uint_fast16_t dy = db->dy;

	const uintptr_t dstinvalidateaddr = (uintptr_t) buffer;	// параметры invalidate получателя
	const int_fast32_t dstinvalidatesize = GXSIZE(dx, dy) * sizeof * buffer;
#if LCDMODE_PALETTE256
	hwaccel_rect_u8(dstinvalidateaddr, dstinvalidatesize, buffer, dx, dy, dx, dy, color);

#elif LCDMODE_RGB565
	hwaccel_rect_u16(dstinvalidateaddr, dstinvalidatesize, buffer, dx, dy, dx, dy, color);

#elif LCDMODE_MAIN_L24
	hwaccel_rect_u24(dstinvalidateaddr, dstinvalidatesize, buffer, dx, dy, dx, dy, color);

#elif LCDMODE_ARGB8888
	hwaccel_rect_u32(dstinvalidateaddr, dstinvalidatesize, buffer, dx, dy, dx, dy, color, FILL_FLAG_NONE);

#endif
}


// поставить цветную точку.
void colpip_point(
	const gxdrawb_t * db,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	)
{
	* colpip_mem_at(db, col, row) = color;
}

// поставить цветную точку (модификация с сохранением старого изоьражения).
void colpip_point_xor(
	const gxdrawb_t * db,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	)
{
	* colpip_mem_at(db, col, row) ^= color;
}

// копирование в большее или равное окно
// размер пикселя - определяется конфигурацией.
// MDMA, DMA2D или программа
// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
void hwaccel_bitblt(
	uintptr_t dstinvalidateaddr,	// параметры clean invalidate получателя
	int_fast32_t dstinvalidatesize,
	const gxdrawb_t * tdb,
	uintptr_t srcinvalidateaddr,	// параметры clean источника
	int_fast32_t srcinvalidatesize,
	const gxdrawb_t * sdb,
	uint_fast16_t sw,	uint_fast16_t sh,	// Размеры окна источника
	unsigned keyflag, COLORPIP_T keycolor
	)
{
	if (sw == 0 || sh == 0)
		return;

#if WITHMDMAHW && (CPUSTYLE_STM32H7XX || CPUSTYLE_STM32MP1)
	// MDMA реализация

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	dcache_clean(srcinvalidateaddr, srcinvalidatesize);

	MDMA_CH->CDAR = (uintptr_t) tdb->buffer;
	MDMA_CH->CSAR = (uintptr_t) sdb->buffer;
	const uint_fast32_t tlen = mdma_tlen(sw * sizeof (PACKEDCOLORPIP_T), sizeof (PACKEDCOLORPIP_T));
	const uint_fast32_t sbus = mdma_getbus(MDMA_CH->CSAR);
	const uint_fast32_t dbus = mdma_getbus(MDMA_CH->CDAR);
	const uint_fast32_t sinc = 0x02; // Source increment mode: 10: address pointer is incremented
	const uint_fast32_t dinc = 0x02; // Destination increment mode: 10: Destination address pointer is incremented
	const uint_fast32_t sburst = mdma_getburst(tlen, sbus, sinc);
	const uint_fast32_t dburst = mdma_getburst(tlen, dbus, dinc);
	MDMA_CH->CTCR =
		(sinc << MDMA_CTCR_SINC_Pos) | 	// Source increment mode: 10: address pointer is incremented
		(MDMA_CTCR_xSIZE_MAIN << MDMA_CTCR_SSIZE_Pos) |
		(MDMA_CTCR_xSIZE_MAIN << MDMA_CTCR_SINCOS_Pos) |
		(sburst << MDMA_CTCR_SBURST_Pos) |
		(dinc << MDMA_CTCR_DINC_Pos) |	// Destination increment mode: 10: Destination address pointer is incremented
		(MDMA_CTCR_xSIZE_MAIN << MDMA_CTCR_DSIZE_Pos) |
		(MDMA_CTCR_xSIZE_MAIN << MDMA_CTCR_DINCOS_Pos) |
		(dburst << MDMA_CTCR_DBURST_Pos) |	// Destination burst transfer configuration
		((tlen - 1) << MDMA_CTCR_TLEN_Pos) |		// buffer Transfer Length (number of bytes - 1)
		(0x00u << MDMA_CTCR_PKE_Pos) |
		(0x00u << MDMA_CTCR_PAM_Pos) |
		(0x02u << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01u << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01u << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((sizeof (PACKEDCOLORPIP_T) * (sw)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00u << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00u << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((sh - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((sizeof (PACKEDCOLORPIP_T) * (GXADJ(sdb->dx) - (sw))) << MDMA_CBRUR_SUV_Pos) |		// Source address Update Value
		((sizeof (PACKEDCOLORPIP_T) * (GXADJ(tdb->dx) - (sw))) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(sbus << MDMA_CTBR_SBUS_Pos) |
		(dbus << MDMA_CTBR_DBUS_Pos) |
		0;

	mdma_startandwait();

#elif WITHDMA2DHW
	// DMA2D реализация
	// See DMA2D_FGCMAR for L8
	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	dcache_clean(srcinvalidateaddr, srcinvalidatesize);

	/* исходный растр */
	DMA2D->FGMAR = (uintptr_t) src;
	//	The line offset used for the foreground image, expressed in pixel when the LOM bit is
	//	reset and in byte when the LOM bit is set.
	DMA2D->FGOR = (DMA2D->FGOR & ~ (DMA2D_FGOR_LO)) |
		((GXADJ(sdb->dx) - sdx) << DMA2D_FGOR_LO_Pos) |
		0;
	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) dst;
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((GXADJ(tdb->dx) - sdx) << DMA2D_OOR_LO_Pos) |
		0;
	/* размер пересылаемого растра */
	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		((sdy) << DMA2D_NLR_NL_Pos) |
		((sdx) << DMA2D_NLR_PL_Pos) |
		0;
	/* формат пикселя */
	DMA2D->FGPFCCR = (DMA2D->FGPFCCR & ~ (DMA2D_FGPFCCR_CM | DMA2D_FGPFCCR_AM)) |
		0 * DMA2D_FGPFCCR_AM |
		DMA2D_FGPFCCR_CM_VALUE_MAIN |	/* Color mode - framebuffer pixel format */
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE | DMA2D_CR_LOM)) |
		0 * DMA2D_CR_LOM | // 0: Line offsets are expressed in pixels
		0 * DMA2D_CR_MODE_0 |	// 000: Memory-to-memory (FG fetch only)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;

	ASSERT((DMA2D->ISR & DMA2D_ISR_CEIF) == 0);	// Configuration Error
	ASSERT((DMA2D->ISR & DMA2D_ISR_TEIF) == 0);	// Transfer Error

	__DMB();

#elif WITHMDMAHW && CPUSTYLE_ALLWINNER && ! defined (G2D_MIXER)

	enum { PIXEL_SIZE = sizeof (PACKEDCOLORPIP_T) };
	const unsigned tstride = tdb->stride;
	const unsigned sstride = sdb->stride;
	const uintptr_t taddr = (uintptr_t) tdb->buffer;
	const uintptr_t saddr = (uintptr_t) sdb->buffer;
	const uint_fast32_t ssizehw = ((sh - 1) << 16) | ((sw - 1) << 0);
	const uint_fast32_t tsizehw = ((sh - 1) << 16) | ((sw - 1) << 0);		/* размер совпадающий с источником - просто для удобства */

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	dcache_clean(srcinvalidateaddr, srcinvalidatesize);

	hwaccel_rotcopy(saddr, sstride, ssizehw, taddr, tstride, tsizehw, 0);

#elif WITHMDMAHW && CPUSTYLE_ALLWINNER && defined (G2D_MIXER)
	/* Копирование - использование G2D для формирования изображений */

//	PRINTF("hwaccel_bitblt: tdx/tdy, sdx/sdy: %u/%u, %u/%u\n", (unsigned) tdx, (unsigned) tdy, (unsigned) sdx, (unsigned) sdy);
//	ASSERT(sdx > 1 && sdy > 1);
//	ASSERT(sdx > 2 && sdy > 2);
	const unsigned srcFormat = awxx_get_srcformat(keyflag);
	enum { PIXEL_SIZE = sizeof (PACKEDCOLORPIP_T) };
	const unsigned tstride = GXADJ(tdb->dx) * PIXEL_SIZE;
	const unsigned sstride = GXADJ(sdb->dx) * PIXEL_SIZE;
	const uintptr_t taddr = (uintptr_t) tdb->buffer;
	const uintptr_t saddr = (uintptr_t) sdb->buffer;
	const uint_fast32_t ssizehw = ((sh - 1) << 16) | ((sw - 1) << 0);
	const uint_fast32_t tsizehw = ((sh - 1) << 16) | ((sw - 1) << 0);		/* размер совпадающий с источником - просто для удобства */

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	dcache_clean(srcinvalidateaddr, srcinvalidatesize);

//	memset(G2D_V0, 0, sizeof * G2D_V0);
//	memset(G2D_UI0, 0, sizeof * G2D_UI0);
//	memset(G2D_UI1, 0, sizeof * G2D_UI1);
//	memset(G2D_UI2, 0, sizeof * G2D_UI2);
//	memset(G2D_BLD, 0, sizeof * G2D_BLD);
//	memset(G2D_WB, 0, sizeof * G2D_WB);

//	G2D_TOP->G2D_AHB_RST &= ~ ((UINT32_C(1) << 1) | (UINT32_C(1) << 0));	// Assert reset: 0x02: rot, 0x01: mixer
//	G2D_TOP->G2D_AHB_RST |= (UINT32_C(1) << 1) | (UINT32_C(1) << 0);	// De-assert reset: 0x02: rot, 0x01: mixer

	awg2d_bitblt(keyflag, keycolor, srcFormat, sstride, ssizehw, saddr, tstride,
			tsizehw, taddr);

#else
	// программная реализация

	if ((keyflag & BITBLT_FLAG_CKEY) != 0)
	{
		// для случая когда горизонтальные пиксели в видеопямяти источника располагаются подряд
		// работа с color key
		PACKEDCOLORPIP_T * src = sdb->buffer;
		PACKEDCOLORPIP_T * dst = tdb->buffer;
		const unsigned stail = GXADJ(sdb->dx) - sw;
		const unsigned dtail = GXADJ(tdb->dx) - sw;
		while (sh --)
		{
			unsigned w = sw;
			while (w --)
			{
				const COLORPIP_T c = * src ++;
				if (c != keycolor)
					* dst = c;
				dst ++;
			}
			src += stail;
			dst += dtail;
		}
	}
	else
	{
		PACKEDCOLORPIP_T * src = sdb->buffer;
		PACKEDCOLORPIP_T * dst = tdb->buffer;
		// для случая когда горизонтальные пиксели в видеопямяти источника располагаются подряд
		// и копируется полностью окно
		if (tdb->dx == sdb->dx && sw == GXADJ(sdb->dx) && tdb->dy == sh)
		{
			const size_t len = (size_t) GXSIZE(sdb->dx, sdb->dy) * sizeof * src;
			// ширина строки одинаковая в получателе и источнике
			memcpy(dst, src, len);
		}
		else
		{
			// Копируем построчно
			const size_t len = sw * sizeof * sdb->buffer;
			while (sh --)
			{
				memcpy(dst, src, len);
				src += GXADJ(sdb->dx);
				dst += GXADJ(tdb->dx);
			}
		}
	}

#endif
}

// скоприовать прямоугольник с изменением размера
void hwaccel_stretchblt(
	uintptr_t dstinvalidateaddr,	// параметры clean invalidate получателя
	int_fast32_t dstinvalidatesize,
	const gxdrawb_t * tdb,	// получатель
	uint_fast16_t w,	uint_fast16_t h,	// Размеры окна получателя
	uintptr_t srcinvalidateaddr,	// параметры clean источника
	int_fast32_t srcinvalidatesize,
	const gxdrawb_t * sdb,	// источник
	uint_fast16_t sw,	uint_fast16_t sh,	// Размеры окна источника в пикселях
	unsigned keyflag, COLORPIP_T keycolor
	)
{
#if WITHMDMAHW && CPUSTYLE_ALLWINNER && ! defined (G2D_MIXER)

	//#warning T507/H616 STRETCH BLT should be implemented

	const unsigned srcFormat = awxx_get_srcformat(keyflag);
	enum { PIXEL_SIZE = sizeof (PACKEDCOLORPIP_T) };
	const uint_fast32_t tsizehw = ((h - 1) << 16) | ((w - 1) << 0);
	const uint_fast32_t ssizehw = ((sh - 1) << 16) | ((sw - 1) << 0);
	const unsigned sstride = sdb->stride;
	const unsigned tstride = tdb->stride;
	const uintptr_t srclinear = (uintptr_t) sdb->buffer;
	const uintptr_t dstlinear = (uintptr_t) tdb->buffer;


	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	dcache_clean(srcinvalidateaddr, srcinvalidatesize);

	hwaccel_rotcopy(srclinear, sstride, ssizehw, dstlinear, tstride, tsizehw, 0);

#elif WITHMDMAHW && CPUSTYLE_ALLWINNER && defined (G2D_MIXER)
	/* Использование G2D для формирования изображений */

//	memset(G2D_V0, 0, sizeof * G2D_V0);
//	memset(G2D_UI0, 0, sizeof * G2D_UI0);
//	memset(G2D_UI1, 0, sizeof * G2D_UI1);
//	memset(G2D_UI2, 0, sizeof * G2D_UI2);
//	memset(G2D_BLD, 0, sizeof * G2D_BLD);
//	memset(G2D_WB, 0, sizeof * G2D_WB);

//	G2D_TOP->G2D_AHB_RST &= ~ ((UINT32_C(1) << 1) | (UINT32_C(1) << 0));	// Assert reset: 0x02: rot, 0x01: mixer
//	G2D_TOP->G2D_AHB_RST |= (UINT32_C(1) << 1) | (UINT32_C(1) << 0);	// De-assert reset: 0x02: rot, 0x01: mixer

//	PRINTF("colpip_stretchblt (resize): w/h=%d/%d, sdx/sdy=%d/%d\n", w, h, sdx, sdy);

	const unsigned srcFormat = awxx_get_srcformat(keyflag);
	enum { PIXEL_SIZE = sizeof (PACKEDCOLORPIP_T) };
	const uint_fast32_t tsizehw = ((h - 1) << 16) | ((w - 1) << 0);
	const uint_fast32_t ssizehw = ((sh - 1) << 16) | ((sw - 1) << 0);
	const unsigned sstride = GXADJ(sdb->dx) * PIXEL_SIZE;
	const unsigned tstride = GXADJ(tdb->dx) * PIXEL_SIZE;
	const uintptr_t srclinear = (uintptr_t) sdb->buffer;
	const uintptr_t dstlinear = (uintptr_t) tdb->buffer;


	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	dcache_clean(srcinvalidateaddr, srcinvalidatesize);

	g2d_rtmx_accure();
	awxx_g2d_mixer_reset();	/* Отключаем все источники */
	ASSERT((G2D_MIXER->G2D_MIXER_CTRL & (UINT32_C(1) << 31)) == 0);

//	G2D_TOP->G2D_AHB_RST &= ~ ((UINT32_C(1) << 1) | (UINT32_C(1) << 0));	// Assert reset: 0x02: rot, 0x01: mixer
//	G2D_TOP->G2D_AHB_RST |= (UINT32_C(1) << 1) | (UINT32_C(1) << 0);	// De-assert reset: 0x02: rot, 0x01: mixer


	if (w != sw || h != sh)
	{
		/* расчет масштабов */
		const uint_fast32_t yhstep = (((uint_fast32_t) sw << VSU_PHASE_FRAC_BITWIDTH) / w);
		const uint_fast32_t yvstep = (((uint_fast32_t) sh << VSU_PHASE_FRAC_BITWIDTH) / h);
		/* Включаем Scaler */
		G2D_VSU->VS_CTRL = 0x00000001;
		G2D_VSU->VS_OUT_SIZE = tsizehw;
		G2D_VSU->VS_GLB_ALPHA = 0x000000FF;

		//
		G2D_VSU->VS_Y_SIZE = ssizehw;
		G2D_VSU->VS_Y_HSTEP = yhstep << VSU_PHASE_FRAC_REG_SHIFT;
		G2D_VSU->VS_Y_VSTEP = yvstep << VSU_PHASE_FRAC_REG_SHIFT;
		G2D_VSU->VS_Y_HPHASE = 0;
		G2D_VSU->VS_Y_VPHASE0 = 0;

		// chroma
		G2D_VSU->VS_C_SIZE = ssizehw;
		G2D_VSU->VS_C_HSTEP = yhstep << VSU_PHASE_FRAC_REG_SHIFT;
		G2D_VSU->VS_C_VSTEP = yvstep << VSU_PHASE_FRAC_REG_SHIFT;
		G2D_VSU->VS_C_HPHASE = 0;
		G2D_VSU->VS_C_VPHASE0 = 0;

		//	if (fmt > G2D_FORMAT_IYUV422_Y1U0Y0V0)
		//		write_wvalue(VS_CTRL, 0x10101);
		//	else
		//		write_wvalue(VS_CTRL, 0x00000101);

		//G2D_VSU->VS_CTRL = 0x00010101;
		G2D_VSU->VS_CTRL |= (UINT32_C(1) << 8);

		vsu_fir_linear(G2D_VSU->VS_Y_HCOEF);	// 0x200
		vsu_fir_linear(G2D_VSU->VS_C_HCOEF);	// 0x400
		//vsu_fir_bytable(G2D_VSU->VS_Y_HCOEF, g2d_vsu_calc_fir_coef(yhstep));	// 0x200 при применении vsu_fir_linear менее размазано при увеличении
		//vsu_fir_bytable(G2D_VSU->VS_C_HCOEF, g2d_vsu_calc_fir_coef(yhstep));	// 0x400 при применении vsu_fir_linear менее размазано при увеличении

		vsu_fir_linear(G2D_VSU->VS_Y_VCOEF);	// 0x300
		//vsu_fir_bytable(G2D_VSU->VS_Y_VCOEF, g2d_vsu_calc_fir_coef(yvstep));	// 0x400 при применении vsu_fir_linear менее размазано при увеличении

		//	if (fmt >= G2D_FORMAT_IYUV422_Y1U0Y0V0)
		//		write_wvalue(VS_CTRL, 0x10001);
		//	else
		//		write_wvalue(VS_CTRL, 0x00001);

		G2D_VSU->VS_CTRL &= ~ (UINT32_C(1) << 8);
	}
	else
	{
		G2D_VSU->VS_CTRL = 0;
	}

	if ((keyflag & BITBLT_FLAG_CKEY) != 0)
	{
		const COLOR24_T keycolor24 = awxx_key_color_conversion(keycolor);
		/* 5.10.9.10 BLD color key control register */
		//G2D_BLD->BLD_KEY_CTL = 0x03;	/* G2D_CK_SRC = 0x03, G2D_CK_DST = 0x01 */
		G2D_BLD->BLD_KEY_CTL =
			0 * (0x01u << 1) |		// KEY0_MATCH_DIR 1: when the pixel value matches source image, it displays the pixel form destination image.
			(UINT32_C(1) << 0) |			// KEY0_EN 1: enable color key in Alpha Blender0.
			0;

		/* 5.10.9.11 BLD color key configuration register */
		G2D_BLD->BLD_KEY_CON =
			0 * (UINT32_C(1) << 2) |		// KEY0R_MATCH 0: match color if value inside keys range
			0 * (UINT32_C(1) << 1) |		// KEY0G_MATCH 0: match color if value inside keys range
			0 * (UINT32_C(1) << 0) |		// KEY0B_MATCH 0: match color if value inside keys range
			0;

		G2D_BLD->BLD_KEY_MAX = keycolor24;
		G2D_BLD->BLD_KEY_MIN = keycolor24;

		/* Данные для замены совпавших с keycolor */
		G2D_UI2->UI_ATTR = awxx_g2d_get_ui_attr(VI_ImageFormat);
		G2D_UI2->UI_PITCH = tstride;
		G2D_UI2->UI_FILLC = 0;
		G2D_UI2->UI_COOR = 0;			// координаты куда класть. Фон заполняенся цветом BLD_BK_COLOR
		G2D_UI2->UI_MBSIZE = tsizehw; // сколько брать от исходного буфера
		G2D_UI2->UI_SIZE = tsizehw;		// параметры окна исходного буфера
		G2D_UI2->UI_LADD = ptr_lo32(dstlinear);
		G2D_UI2->UI_HADD = ptr_hi32(dstlinear);

		/* Подача данных на вход VSU */
		G2D_V0->V0_ATTCTL = awxx_g2d_get_vi_attr(srcFormat);
		G2D_V0->V0_PITCH0 = sstride;
		G2D_V0->V0_FILLC = 0;
		G2D_V0->V0_COOR = 0;			// координаты куда класть. Фон заполняенся цветом BLD_BK_COLOR
		G2D_V0->V0_MBSIZE = ssizehw; 	// сколько брать от исходного буфера
		G2D_V0->V0_SIZE = ssizehw;		// параметры окна исходного буфера
		G2D_V0->V0_LADD0 = ptr_lo32(srclinear);
		G2D_V0->V0_HADD = (ptr_hi32(srclinear) & 0xFF) << 0;

		G2D_BLD->BLD_SIZE = tsizehw;	// размер выходного буфера
		/* источник когда есть совпадние ??? */
		G2D_BLD->BLD_CH_ISIZE [0] = tsizehw;
		G2D_BLD->BLD_CH_OFFSET [0] = 0;// ((row) << 16) | ((col) << 0);
		/* источник для анализа ??? */
		G2D_BLD->BLD_CH_ISIZE [1] = tsizehw;
		G2D_BLD->BLD_CH_OFFSET [1] = 0;// ((row) << 16) | ((col) << 0);

		G2D_BLD->BLD_FILL_COLOR_CTL =
			(UINT32_C(1) << 8) |	// 8: P0_EN Pipe0 enable
			(UINT32_C(1) << 9) |	// 9: P1_EN Pipe1 enable
			0;

		G2D_BLD->ROP_CTL = 0x00F0;	// 0x00F0 G2D_V0, 0x55F0 UI1, 0xAAF0 UI2
		G2D_BLD->ROP_INDEX [0] = 0;		// ? зависят от ROP_CTL
		G2D_BLD->ROP_INDEX [1] = 0;

//		G2D_BLD->BLD_CTL = awxx_bld_ctl(3, 1, 3, 1); //0x03010301;	// G2D_BLD_SRCOVER - default value
//		G2D_BLD->BLD_CTL = 0x00010001;	// G2D_BLD_COPY
//		G2D_BLD->BLD_CTL = 0x00000000;	// G2D_BLD_CLEAR
		G2D_BLD->BLD_CTL = awxx_bld_ctl2(1, 3); //awxx_bld_ctl(1, 3, 1, 3); //0x01030103;	// G2D_BLD_DSTOVER - проверить это ли
//		G2D_BLD->BLD_CTL = 0x01030103;	// G2D_BLD_DSTOVER
//		G2D_BLD->BLD_CTL = 0x00000000; 	// G2D_BLD_CLEAR
//		G2D_BLD->BLD_CTL = 0x00010001; 	// G2D_BLD_COPY
		G2D_BLD->BLD_CTL = awxx_bld_ctl2(1, 0); //awxx_bld_ctl(1, 0, 1, 0); //0x01000100; 	// G2D_BLD_DST - проверить это ли
//		G2D_BLD->BLD_CTL = 0x03010301; 	// G2D_BLD_SRCOVER
//		G2D_BLD->BLD_CTL = 0x01030103; 	// G2D_BLD_DSTOVER
//		G2D_BLD->BLD_CTL = 0x00020002; 	// G2D_BLD_SRCIN
//		G2D_BLD->BLD_CTL = 0x02000200; 	// G2D_BLD_DSTIN
//		G2D_BLD->BLD_CTL = 0x00030003; 	// G2D_BLD_SRCOUT
//		G2D_BLD->BLD_CTL = 0x03000300; 	// G2D_BLD_DSTOUT
//		G2D_BLD->BLD_CTL = 0x03020302; 	// G2D_BLD_SRCATOP
//		G2D_BLD->BLD_CTL = 0x02030203; 	// G2D_BLD_DSTATOP
//		G2D_BLD->BLD_CTL = 0x03030303; 	// G2D_BLD_XOR
	}
	else
	{
		G2D_V0->V0_ATTCTL = awxx_g2d_get_vi_attr(srcFormat);
		G2D_V0->V0_PITCH0 = sstride;
		G2D_V0->V0_FILLC = 0;
		G2D_V0->V0_COOR = 0;			// координаты куда класть. Фон заполняенся цветом BLD_BK_COLOR
		G2D_V0->V0_MBSIZE = ssizehw; 	// сколько брать от исходного буфера
		G2D_V0->V0_SIZE = ssizehw;		// параметры окна исходного буфера
		G2D_V0->V0_LADD0 = ptr_lo32(srclinear);
		G2D_V0->V0_HADD = (ptr_hi32(srclinear) & 0xFF) << 0;

		G2D_BLD->BLD_SIZE = tsizehw;	// размер выходного буфера
		G2D_BLD->BLD_CH_ISIZE [0] = tsizehw;

		G2D_BLD->BLD_FILL_COLOR_CTL =
			(UINT32_C(1) << 8) |	// 8: P0_EN Pipe0 enable
			0;

		G2D_BLD->ROP_CTL = 0x000000F0; /* 0x000000F0 */
		G2D_BLD->ROP_INDEX [0] = 0;		// ? зависят от ROP_CTL
		G2D_BLD->ROP_INDEX [1] = 0;
		G2D_BLD->BLD_CTL = awxx_bld_ctl2(3, 1); //awxx_bld_ctl(3, 1, 3, 1); //0x03010301;	// G2D_BLD_SRCOVER - default value
	}

	/* Write-back settings */
	G2D_WB->WB_ATT = WB_ImageFormat;
	G2D_WB->WB_LADD0 = ptr_lo32(dstlinear);
	G2D_WB->WB_HADD0 = ptr_hi32(dstlinear);
	G2D_WB->WB_PITCH0 = tstride;
	G2D_WB->WB_SIZE = tsizehw;

	awxx_g2d_rtmix_startandwait();		/* Запускаем и ждём завершения обработки */
	g2d_rtmx_release();

#else

	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);
	colpip_fillrect(tdb, 0, 0, w, h, COLORPIP_GREEN);
	dcache_clean_invalidate(dstinvalidateaddr, dstinvalidatesize);

#endif
}

// копирование буфера с поворотом вправо на 90 градусов (четверть оборота).
void hwaccel_ra90(
	const gxdrawb_t * tdb,	// получатель
	uint_fast16_t tx,	// горизонтальная координата пикселя (0..dx-1) слева направо - в исходном нижний
	uint_fast16_t ty,	// вертикальная координата пикселя (0..dy-1) сверху вниз - в исходном левый
	const gxdrawb_t * sdb	// источник
	)
{
	if (sdb->dx == 0 || sdb->dy == 0)
		return;

	uint_fast16_t x;	// x получателя
	for (x = 0; x < sdb->dy; ++ x)
	{
		uint_fast16_t y;	// y получателя
		for (y = 0; y < sdb->dx; ++ y)
		{
			const COLORPIP_T pixel = * colpip_const_mem_at(sdb, y, sdb->dy - 1 - x);	// выборка из исхолного битмапа
			* colpip_mem_at(tdb, tx + x, ty + y) = pixel;
		}
	}
}


//// Routine to draw a line in the RGB565 color to the LCD.
//// The line is drawn from (xmin,ymin) to (xmax,ymax).
//// The algorithm used to draw the line is "Bresenham's line
//// algorithm".
//#define SWAP(a, b)  do { (a) ^= (b); (b) ^= (a); (a) ^= (b); } while (0)
//// Нарисовать линию указанным цветом
//void colpip_line_no_aa(
//	PACKEDCOLORPIP_T * buffer,
//	uint_fast16_t dx,
//	uint_fast16_t dy,
//	uint_fast16_t x0,
//	uint_fast16_t y0,
//	uint_fast16_t x1,
//	uint_fast16_t y1,
//	COLORPIP_T color
//	)
//{
//	int xmin = x0;
//	int xmax = x1;
//	int ymin = y0;
//	int ymax = y1;
//	int Dx = xmax - xmin;
//	int Dy = ymax - ymin;
//	int steep = (abs(Dy) >= abs(Dx));
//	if (steep) {
//	   SWAP(xmin, ymin);
//	   SWAP(xmax, ymax);
//	   // recompute Dx, Dy after swap
//	   Dx = xmax - xmin;
//	   Dy = ymax - ymin;
//	}
//	int xstep = 1;
//	if (Dx < 0) {
//	   xstep = -1;
//	   Dx = -Dx;
//	}
//	int ystep = 1;
//	if (Dy < 0) {
//	   ystep = -1;
//	   Dy = -Dy;
//	}
//
//   int TwoDy = 2 * Dy;
//   int TwoDyTwoDx = TwoDy - 2 * Dx; // 2*Dy - 2*Dx
//   int E = TwoDy - Dx; //2*Dy - Dx
//   int y = ymin;
//   int xDraw, yDraw;
//   int x;
//   for (x = xmin; x != xmax; x += xstep) {
//       if (steep) {
//           xDraw = y;
//           yDraw = x;
//       } else {
//           xDraw = x;
//           yDraw = y;
//       }
//       // plot
//	   colpip_point(db, xDraw, yDraw, color);
//       // next
//       if (E > 0) {
//           E += TwoDyTwoDx; //E += 2*Dy - 2*Dx;
//           y = y + ystep;
//       } else {
//           E += TwoDy; //E += 2*Dy;
//       }
//   }
//}
//
//#undef SWAP

// Нарисовать закрашенный или пустой прямоугольник
void colpip_rect(
	const gxdrawb_t * db,
	uint_fast16_t x1,	// начальная координата
	uint_fast16_t y1,	// начальная координата
	uint_fast16_t x2,	// конечная координата (включена в заполняемую облсть)
	uint_fast16_t y2,	// конечная координата (включена в заполняемую облсть)
	COLORPIP_T color,
	uint_fast8_t fill
	)
{
	if (fill != 0)
	{
		const uint_fast16_t w = x2 - x1 + 1;	// размер по горизонтали
		const uint_fast16_t h = y2 - y1 + 1;	// размер по вертикали

		if (w < 3 || h < 3)
			return;

		colpip_fillrect(db, x1, y1, w, h, color);
	}
	else
	{
		int antialiasing = 0;
		colpip_line(db, x1, y1, x2, y1, color, antialiasing);		// верхняя горизонталь
		colpip_line(db, x1, y2, x2, y2, color, antialiasing);		// нижняя горизонталь
		colpip_line(db, x1, y1, x1, y2, color, antialiasing);		// левая вертикаль
		colpip_line(db, x2, y1, x2, y2, color, antialiasing);		// правая вертикаль
	}
}

#if WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC

static uint_fast8_t snapshot_req;
/* запись видимого изображения */
void
display_snapshot(const gxdrawb_t * db)
{
	if (snapshot_req != 0)
	{
		snapshot_req = 0;
		/* запись файла */
		display_snapshot_write(db);
	}
}

void display_snapshot_req(void)
{
	snapshot_req = 1;
}

#else /* WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC */
/* stub */
/* запись видимого изображения */
void
display_snapshot(const gxdrawb_t * db)
{
}

/* stub */
void display_snapshot_req(void)
{
}
#endif /* WITHDISPLAYSNAPSHOT && WITHUSEAUDIOREC */

// скоприовать прямоугольник без изменения размера
void colpip_bitblt(
	uintptr_t dstinvalidateaddr,	// параметры clean invalidate получателя
	int_fast32_t dstinvalidatesize,
	const gxdrawb_t * tdb,	// получатель
	uint_fast16_t x,	uint_fast16_t y,	// получатель Позиция
	uintptr_t srcinvalidateaddr,	// параметры clean источника
	int_fast32_t srcinvalidatesize,
	const gxdrawb_t * sdb, 	// источник
	uint_fast16_t sx,	uint_fast16_t sy,	// источник Позиция окна
	uint_fast16_t sw,	uint_fast16_t sh,	// Размеры окна источника
	unsigned keyflag, COLORPIP_T keycolor
	)
{
	//PRINTF("colpip_bitblt: tdx/tdy=%d/%d, sdx/sdy=%d/%d, keyflag=%08X\n", tdx, tdy, sdx, sdy, keyflag);
	gxdrawb_t sdba;
	gxdrawb_t tdba;
	gxdrawb_initialize(& sdba, colpip_mem_at(sdb, sx, sy), sdb->dx, sdb->dy - sy);
	gxdrawb_initialize(& tdba, colpip_mem_at(tdb, x, y), tdb->dx, tdb->dy - y);


	//ASSERT(((uintptr_t) src % DCACHEROWSIZE) == 0);	// TODO: добавиль парамтр для flush исходного растра
	hwaccel_bitblt(
		dstinvalidateaddr, dstinvalidatesize,	// target area clean invalidate parameters
		& tdba,
		srcinvalidateaddr, srcinvalidatesize,	// параметры clean источника
		& sdba,
		sw, sh,	// размеры окна источника
		keyflag, keycolor
		);
}

// скоприовать прямоугольник с изменением размера
void colpip_stretchblt(
	uintptr_t dstinvalidateaddr,	int_fast32_t dstinvalidatesize,	// параметры clean invalidate получателя
	const gxdrawb_t * tdb,	// получатель
	uint_fast16_t x,	uint_fast16_t y,	// позиция получателя
	uint_fast16_t w,	uint_fast16_t h,	// Размеры окна получателя
	uintptr_t srcinvalidateaddr,	int_fast32_t srcinvalidatesize,	// параметры clean источника
	const gxdrawb_t * sdb,	// source buffer
	uint_fast16_t sx,	uint_fast16_t sy,	// источник Позиция (размеры совпадают с получателем)
	uint_fast16_t sw,	uint_fast16_t sh,	// Размеры окна источника
	unsigned keyflag, COLORPIP_T keycolor
	)
{
	gxdrawb_t sdba;
	gxdrawb_t tdba;
	gxdrawb_initialize(& sdba, colpip_mem_at(sdb, sx, sy), sdb->dx, sdb->dy - sy);
	gxdrawb_initialize(& tdba, colpip_mem_at(tdb, x, y), tdb->dx, tdb->dy - y);

	hwaccel_stretchblt(
			dstinvalidateaddr, dstinvalidatesize,
			& tdba, w, h,
			srcinvalidateaddr, srcinvalidatesize,
			& sdba, sw, sh,
			keyflag, keycolor);

}

// скоприовать прямоугольник с типом пикселей соответствующим pip
// с поворотом вправо на 90 градусов
void colpip_bitblt_ra90(
	uintptr_t dstinvalidateaddr,	// параметры clean invalidate получателя
	int_fast32_t dstinvalidatesize,
	const gxdrawb_t * tdb,	// получатель
	uint_fast16_t x,	// получатель Позиция
	uint_fast16_t y,	// получатель
	uintptr_t srcinvalidateaddr,	// параметры clean источника
	int_fast32_t srcinvalidatesize,
	const gxdrawb_t * sdb 	// источник
	)
{
	//ASSERT(((uintptr_t) src % DCACHEROWSIZE) == 0);	// TODO: добавиль парамтр для flush исходного растра
	hwaccel_ra90(
		//dstinvalidateaddr, dstinvalidatesize,	// target area clean invalidate parameters
		tdb,
		x, y,
		//srcinvalidateaddr, srcinvalidatesize,	// параметры clean источника
		sdb
		);
}



#if LCDMODE_COLORED

// Установить прозрачность для прямоугольника
void display_transparency(const gxdrawb_t * db,
	uint_fast16_t x1, uint_fast16_t y1,
	uint_fast16_t x2, uint_fast16_t y2,
	uint_fast8_t alpha	// на сколько затемнять цвета (0 - чёрный, 255 - без изменений)
	)
{
	uint_fast16_t y;

	for (y = y1; y <= y2; y ++)
	{
		uint_fast16_t x;
		for (x = x1; x <= x2; x ++)
		{
			PACKEDCOLORPIP_T * const p = colpip_mem_at(db, x, y);
			* p = getshadedcolor(* p, alpha);
		}
	}
}

static uint_fast8_t scalecolor(
	uint_fast8_t cv,	// color component value
	uint_fast8_t maxv,	// maximal color component value
	uint_fast8_t rmaxv	// resulting maximal color component value
	)
{
	return (uint_fast16_t) cv * rmaxv / maxv;
}

/* модифицировать цвет в RGB24 */
static COLOR24_T
color24_shaded(
	COLOR24_T dot,
	uint_fast8_t alpha	// на сколько затемнять цвета (0 - чёрный, 255 - без изменений)
	)
{
	//return dot;	// test
	if (dot == 0)
		return COLOR24(alpha >> 2, alpha >> 2, alpha >> 2);
	const uint_fast8_t r = scalecolor((dot >> 16) & 0xFF, 255, alpha);
	const uint_fast8_t g = scalecolor((dot >> 8) & 0xFF, 255, alpha);
	const uint_fast8_t b = scalecolor((dot >> 0) & 0xFF, 255, alpha);
	return COLOR24(r, g, b);
}

/* модифицировать цвет в RGB24 */
static COLOR24_T
color24_aliased(
	COLOR24_T dot
	)
{
	return color24_shaded(dot, DEFAULT_ALPHA);	// test
}


/* модифицировать цвет */
COLORPIP_T getshadedcolor(
	COLORPIP_T dot, // исходный цвет
	uint_fast8_t alpha	// на сколько затемнять цвета (0 - чёрный, 255 - без изменений)
	)
{
#if defined (COLORPIP_SHADED)

	return dot |= COLORPIP_SHADED;

#elif LCDMODE_RGB565

	if (dot == COLORPIP_BLACK)
	{
		return TFTRGB(alpha, alpha, alpha); // back gray
	}
	else
	{
		// распаковка дисплейного представления
		const uint_fast8_t r = ((dot >> 11) & 0x001f) * 8;	// result in 0..255
		const uint_fast8_t g = ((dot >> 5) & 0x003f) * 4;	// result in 0..255
		const uint_fast8_t b = ((dot >> 0) & 0x001f) * 8;	// result in 0..255

		const COLOR24_T c = color24_shaded(COLOR24(r, g, b), alpha);
		return TFTRGB((c >> 16) & 0xFF, (c >> 8) & 0xFF, (c >> 0) & 0xFF);
	}

#elif LCDMODE_ARGB8888 && CPUSTYLE_XC7Z && ! WITHTFT_OVER_LVDS

	if (dot == COLORPIP_BLACK)
	{
		return TFTRGB(alpha, alpha, alpha); // back gray
	}
	else
	{
		// распаковка дисплейного представления
		const uint_fast8_t r = ((dot >> 16) & 0x00FF) * 1;	// result in 0..255
		const uint_fast8_t g = ((dot >> 0) & 0x00FF) * 1;	// result in 0..255
		const uint_fast8_t b = ((dot >> 8) & 0x00FF) * 1;	// result in 0..255

		const COLOR24_T c = color24_shaded(COLOR24(r, g, b), alpha);
		return TFTRGB((c >> 16) & 0xFF, (c >> 8) & 0xFF, (c >> 0) & 0xFF);
	}


#elif LCDMODE_ARGB8888

	if (dot == COLORPIP_BLACK)
	{
		return TFTRGB(alpha, alpha, alpha); // back gray
	}
	else
	{
		// распаковка дисплейного представления
		const uint_fast8_t r = ((dot >> 16) & 0x00FF) * 1;	// result in 0..255
		const uint_fast8_t g = ((dot >> 8) & 0x00FF) * 1;	// result in 0..255
		const uint_fast8_t b = ((dot >> 0) & 0x00FF) * 1;	// result in 0..255

		const COLOR24_T c = color24_shaded(COLOR24(r, g, b), alpha);
		return TFTRGB((c >> 16) & 0xFF, (c >> 8) & 0xFF, (c >> 0) & 0xFF);
	}


#elif LCDMODE_PIP_RGB24

	if (dot == COLORPIP_BLACK)
	{
		return COLOR24(alpha, alpha, alpha); // back gray
	}
	else
	{
		return color24_shaded(dot, alpha);
	}



#else /*  */
	return dot;

#endif /* */
}

#if defined (COLORPIP_SHADED)

static void fillpair_xltrgb24(COLOR24_T * xltable, unsigned i, COLOR24_T c)
{
	ASSERT(i < 128);
	xltable [i] = c;
	xltable [i | COLORPIP_SHADED] = color24_shaded(c, DEFAULT_ALPHA);
}

static void fillfour_xltrgb24(COLOR24_T * xltable, unsigned i, COLOR24_T c)
{
	ASSERT(i < 128 - 16);
	xltable [i] = c;
	xltable [i | COLORPIP_SHADED] = color24_shaded(c, DEFAULT_ALPHA);
	xltable [i | COLORPIP_ALIASED] =  color24_aliased(c);
	xltable [i | COLORPIP_SHADED | COLORPIP_ALIASED] = color24_aliased(color24_shaded(c, DEFAULT_ALPHA));
}

#endif /* defined (COLORPIP_SHADED) */

#endif /* LCDMODE_COLORED */

void display2_xltrgb24(COLOR24_T * xltable)
{
	unsigned i;

	//PRINTF("display2_xltrgb24: init indexed colors\n");

	for (i = 0; i < 256; ++ i)
	{
		xltable [i] = COLOR24(i, i, i);
	}

#if defined (COLORPIP_SHADED)

	// часть цветов с 0-го индекса используется в отображении водопада
	// остальные в дизайне
	for (i = 0; i < COLORPIP_BASE; ++ i)
	{
		fillpair_xltrgb24(xltable, i, colorgradient(i, COLORPIP_BASE - 1));
	}

#if 0
	{
		/* тестовое заполнение палитры для проверки целостности сигналов к TFT
		 * Используется совместно с test: вывод палитры на экран
		 */
		enum { TESTSIZE = 64 };
		for (i = 0; i < 256; ++ i)
		{
			xltable [i] = COLOR24(0, 0, 0);
		}

	#if 0
		/* RED */
		for (i = 0; i < TESTSIZE; ++ i)
		{
			const uint_fast8_t c = scalecolor(i, TESTSIZE - 1, 255);
			fillpair_xltrgb24(xltable, i, COLOR24(1 * c, 0 * c, 0 * c));	// проверить результат перед попыткой применить целочисленные вычисления!
		}
	#elif 0
		/* GREEN */
		for (i = 0; i < TESTSIZE; ++ i)
		{
			const uint_fast8_t c = scalecolor(i, TESTSIZE - 1, 255);
			fillpair_xltrgb24(xltable, i, COLOR24(0 * c, 1 * c, 0 * c));	// проверить результат перед попыткой применить целочисленные вычисления!
		}
	#else
		/* BLUE */
		for (i = 0; i < TESTSIZE; ++ i)
		{
			const uint_fast8_t c = scalecolor(i, TESTSIZE - 1, 255);
			fillpair_xltrgb24(xltable, i, COLOR24(0 * c, 0 * c, 1 * c));	// проверить результат перед попыткой применить целочисленные вычисления!
		}
	#endif
	}
#endif
	// Цвета используемые в дизайне

	fillfour_xltrgb24(xltable, COLORPIP_YELLOW    	, COLOR24(0xFF, 0xFF, 0x00));
	fillfour_xltrgb24(xltable, COLORPIP_ORANGE    	, COLOR24(0xFF, 0xA5, 0x00));
	fillfour_xltrgb24(xltable, COLORPIP_BLACK     	, COLOR24(0x00, 0x00, 0x00));
	fillfour_xltrgb24(xltable, COLORPIP_WHITE     	, COLOR24(0xFF, 0xFF, 0xFF));
	fillfour_xltrgb24(xltable, COLORPIP_GRAY      	, COLOR24(0x60, 0x60, 0x60));
	fillfour_xltrgb24(xltable, COLORPIP_DARKGREEN 	, COLOR24(0x00, 0x80, 0x00));
	fillfour_xltrgb24(xltable, COLORPIP_BLUE      	, COLOR24(0x00, 0x00, 0xFF));
	fillfour_xltrgb24(xltable, COLORPIP_GREEN     	, COLOR24(0x00, 0xFF, 0x00));
	fillfour_xltrgb24(xltable, COLORPIP_RED       	, COLOR24(0xFF, 0x00, 0x00));
	fillfour_xltrgb24(xltable, COLORPIP_DARKGRAY    , COLOR24(0x10, 0x10, 0x10));

	fillfour_xltrgb24(xltable, DSGN_LOCKED	  	, COLOR24(0x3C, 0x3C, 0x00));
	// код (COLORPIP_BASE + 15) освободися. GUI_MENUSELECTCOLOR?

	fillfour_xltrgb24(xltable, DSGN_GRIDCOLOR0      , COLOR24(0x80, 0x80, 0x00));        //COLOR_GRAY - center marker
	fillfour_xltrgb24(xltable, DSGN_GRIDCOLOR2     	, COLOR24(0x80, 0x00, 0x00));        //COLOR_DARKRED - other markers
	fillfour_xltrgb24(xltable, DSGN_SPECTRUMBG     	, COLOR24(0x00, 0x00, 0x00));            // фон спектра вне полосы пропускания
	fillfour_xltrgb24(xltable, DSGN_SPECTRUMBG2   	, COLOR24(0x00, 0x80, 0x80));        // фон спектра - полоса пропускания приемника
	fillfour_xltrgb24(xltable, DSGN_SPECTRUMBG2RX2  , COLOR24(0x00, 0x80, 0x80));        // фон спектра - полоса пропускания приемника
	fillfour_xltrgb24(xltable, DSGN_SPECTRUMFG		, COLOR24(0x00, 0xFF, 0x00));		// цвет спектра при сполошном заполнении

#elif LCDMODE_COLORED && ! LCDMODE_DUMMY
	//PRINTF("display2_xltrgb24: init RRRRRGGG GGGBBBBB colos\n");
	// Обычная таблица - все цвета могут быть использованы как индекс
	// Водопад отображается без использования инлдексов цветов

	for (i = 0; i < 256; ++ i)
	{
		const uint_fast8_t r = ((i & 0xe0) << 0) | ((i & 0xe0) >> 3) | ((i & 0xe0) >> 6);		// 3 bit red
		const uint_fast8_t g = ((i & 0x1c) << 3) | ((i & 0x1c) << 0) | ((i & 0x1c) >> 3) ;	// 3 bit green
		const uint_fast8_t b = ((i & 0x03) << 6) | ((i & 0x03) << 4) | ((i & 0x03) << 2) | ((i & 0x03) << 0);	// 2 bit blue
		xltable [i] = COLOR24(r, g, b);
	}

#else
	#warning Monochrome display without indexing colors
#endif /* */
}

#endif /* ! (LCDMODE_DUMMY) */

#if WITHDMA2DHW

void arm_hardware_dma2d_initialize(void)
{
#if CPUSTYLE_STM32H7XX
	/* Enable the DMA2D Clock */
	RCC->AHB3ENR |= RCC_AHB3ENR_DMA2DEN;	/* DMA2D clock enable */
	(void) RCC->AHB3ENR;

#else /* CPUSTYLE_STM32H7XX */
	/* Enable the DMA2D Clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2DEN;	/* DMA2D clock enable */
	(void) RCC->AHB1ENR;

#endif /* CPUSTYLE_STM32H7XX */

	/* set AXI master timer */
	DMA2D->AMTCR = (DMA2D->AMTCR & ~ (DMA2D_AMTCR_DT_Msk | DMA2D_AMTCR_EN_Msk)) |
		(DMA2D_AMTCR_DT_VALUE << DMA2D_AMTCR_DT_Pos) |
		(DMA2D_AMTCR_DT_ENABLE << DMA2D_AMTCR_EN_Pos) |
		0;
#if 0
	static ALIGNX_BEGIN uint32_t clut [256] ALIGNX_END;
	memset(clut, 0xFF, sizeof clut);
	dcache_clean((uintptr_t) clut, sizeof clut);
	DMA2D->FGCMAR = (uintptr_t) clut;
	DMA2D->BGCMAR = (uintptr_t) clut;
#endif
}

#endif /* WITHDMA2DHW */


#if WITHGPUHW && (CPUSTYLE_T507 || CPUSTYLE_H616)
//#define GPU_CTRLBASE (GPU_BASE + 0x10000)

// https://elixir.bootlin.com/linux/latest/source/drivers/gpu/drm/panfrost/panfrost_regs.h

/* GPU_COMMAND values */
#define GPU_COMMAND_NOP                0x00 /* No operation, nothing happens */
#define GPU_COMMAND_SOFT_RESET         0x01 /* Stop all external bus interfaces, and then reset the entire GPU. */
#define GPU_COMMAND_HARD_RESET         0x02 /* Immediately reset the entire GPU. */
#define GPU_COMMAND_PRFCNT_CLEAR       0x03 /* Clear all performance counters, setting them all to zero. */
#define GPU_COMMAND_PRFCNT_SAMPLE      0x04 /* Sample all performance counters, writing them out to memory */
#define GPU_COMMAND_CYCLE_COUNT_START  0x05 /* Starts the cycle counter, and system timestamp propagation */
#define GPU_COMMAND_CYCLE_COUNT_STOP   0x06 /* Stops the cycle counter, and system timestamp propagation */
#define GPU_COMMAND_CLEAN_CACHES       0x07 /* Clean all caches */
#define GPU_COMMAND_CLEAN_INV_CACHES   0x08 /* Clean and invalidate all caches */
#define GPU_COMMAND_SET_PROTECTED_MODE 0x09 /* Places the GPU in protected mode */


/* GPU_STATUS values */
#define GPU_STATUS_PRFCNT_ACTIVE            (1 << 2)    /* Set if the performance counters are active. */
#define GPU_STATUS_PROTECTED_MODE_ACTIVE    (1 << 7)    /* Set if protected mode is active */


/* IRQ flags */
#define GPU_FAULT               (1 << 0)    /* A GPU Fault has occurred */
#define MULTIPLE_GPU_FAULTS     (1 << 7)    /* More than one GPU Fault occurred. */
#define RESET_COMPLETED         (1 << 8)    /* Set when a reset has completed. */
#define POWER_CHANGED_SINGLE    (1 << 9)    /* Set when a single core has finished powering up or down. */
#define POWER_CHANGED_ALL       (1 << 10)   /* Set when all cores have finished powering up or down. */

#define PRFCNT_SAMPLE_COMPLETED (1 << 16)   /* Set when a performance count sample has completed. */
#define CLEAN_CACHES_COMPLETED  (1 << 17)   /* Set when a cache clean operation has completed. */

#define GPU_IRQ_REG_ALL (GPU_FAULT | MULTIPLE_GPU_FAULTS | 0*RESET_COMPLETED \
		| POWER_CHANGED_ALL | PRFCNT_SAMPLE_COMPLETED)

static void gpu_command(unsigned cmd)
{
	while ((GPU_CONTROL->GPU_STATUS & (UINT32_C(1) << 0)) != 0)
		;
	GPU_CONTROL->GPU_COMMAND = cmd;
//	unsigned v1 = GPU->GPU_STATUS;
//	unsigned v2 = GPU->GPU_STATUS;
//	unsigned v3 = GPU->GPU_STATUS;
//	PRINTF("cmd: %08X, Status: %08X, %08X, %08X\n", cmd, v1, v2, v3);
}

static void gpu_wait(unsigned mask)
{
	while ((GPU_CONTROL->GPU_IRQ_RAWSTAT & mask) != mask)
		;
	GPU_CONTROL->GPU_IRQ_CLEAR = mask;
}

/* AS_COMMAND register commands */
#define AS_COMMAND_NOP			0x00	/* NOP Operation */
#define AS_COMMAND_UPDATE		0x01	/* Broadcasts the values in AS_TRANSTAB and ASn_MEMATTR to all MMUs */
#define AS_COMMAND_LOCK			0x02	/* Issue a lock region command to all MMUs */
#define AS_COMMAND_UNLOCK		0x03	/* Issue a flush region command to all MMUs */
#define AS_COMMAND_FLUSH		0x04	/* Flush all L2 caches then issue a flush region command to all MMUs
						   (deprecated - only for use with T60x) */
#define AS_COMMAND_FLUSH_PT		0x04	/* Flush all L2 caches then issue a flush region command to all MMUs */
#define AS_COMMAND_FLUSH_MEM		0x05	/* Wait for memory accesses to complete, flush all the L1s cache then
						   flush all L2 caches then issue a flush region command to all MMUs */


static void gpu_as_command(unsigned as, unsigned cmd)
{

}

void GPU_IRQHandler(void)
{
	PRINTF("GPU_IRQHandler\n");
	PRINTF("GPU_CONTROL->GPU_IRQ_STATUS=%08X\n", (unsigned) GPU_CONTROL->GPU_IRQ_STATUS);
	ASSERT(0);
}

void GPU_JOB_IRQHandler(void)
{
	PRINTF("GPU_JOB_IRQHandler\n");
	ASSERT(0);
}

void GPU_MMU_IRQHandler(void)
{
	PRINTF("GPU_MMU_IRQHandler\n");
	ASSERT(0);
}

void GPU_EVENT_IRQHandler(void)
{
	PRINTF("GPU_EVENT_IRQHandler\n");
	ASSERT(0);
}

void gpu_fillrect(
	PACKEDCOLORPIP_T * __restrict buffer,
	uint_fast16_t dx,	// ширина буфера
	uintptr_t taddr,
	uint_fast32_t tstride,
	uint_fast32_t tsizehw,
	unsigned alpha,
	COLOR24_T color24,
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	COLORPIP_T color	// цвет
	)
{
	if (w == 0 || h == 0)
		return;
	if (w == 1 && h == 1)
	{
		* buffer = TFTALPHA(alpha, color24);
		return;
	}
	//hwaccel_fillrect(buffer, dx, taddr, tstride, tsizehw, COLORPIP_A(color), (color & 0xFFFFFF), w, h, color, FILL_FLAG_NONE);
	int32_t triangle0 [3] [2] = { { 0, 0 }, { 0, h - 1}, { w - 1, 0 } };
	int32_t triangle1 [3] [2] = { { w - 1, h - 1 }, { 0, h - 1}, { w - 1, 0 } };
}

// Graphic processor unit
void board_gpu_initialize(void)
{
	PRINTF("board_gpu_initialize start.\n");
	{
		//PRINTF("1 CCU->PLL_GPU0_CTRL_REG = %08X\n", (unsigned) CCU->PLL_GPU0_CTRL_REG);

		const unsigned N = 432 * 2 / 24;
		const unsigned M1 = 1;
		const unsigned M0 = 2;
		// PLL_GPU0 = 24 MHz*N/M0/M1
		CCU->PLL_GPU0_CTRL_REG = 0;
		allwnr_t507_module_pll_spr(& CCU->PLL_GPU0_CTRL_REG, & CCU->PLL_GPU0_PAT0_CTRL_REG);	// Set Spread Frequency Mode
		CCU->PLL_GPU0_CTRL_REG &= ~ (UINT32_C(1) << 31) & ~ (UINT32_C(1) << 27);
		CCU->PLL_GPU0_CTRL_REG |=
			(N - 1) * (UINT32_C(1) << 8) |
			(M1 - 1) * (UINT32_C(1) << 1) |
			(M0 - 1) * (UINT32_C(1) << 0) |
			0;
		CCU->PLL_GPU0_CTRL_REG |= (UINT32_C(1) << 31); // PLL_ENABLE
		CCU->PLL_GPU0_CTRL_REG |= (UINT32_C(1) << 29); // LOCK_ENABLE
		while ((CCU->PLL_GPU0_CTRL_REG  & (UINT32_C(1) << 28)) == 0)	// LOCK
			;
		CCU->PLL_GPU0_CTRL_REG |= (UINT32_C(1) << 27); // PLL_OUTPUT_ENABLE
		//PRINTF("2 CCU->PLL_GPU0_CTRL_REG = %08X\n", (unsigned) CCU->PLL_GPU0_CTRL_REG);

	}

	CCU->GPU_CLK1_REG |= (UINT32_C(1) << 31);	// PLL_PERI_BAK_CLK_GATING
	CCU->GPU_CLK0_REG |= (UINT32_C(1) << 31);	// SCLK_GATING

	PRCM->GPU_PWROFF_GATING = 0;

	CCU->GPU_BGR_REG |= (UINT32_C(1) << 0);	// Clock Gating
	CCU->GPU_BGR_REG &= ~ (UINT32_C(1) << 16);	// Assert Reset
	CCU->GPU_BGR_REG |= (UINT32_C(1) << 16);	// De-assert Reset

	PRINTF("allwnr_t507_get_gpu_freq()=%" PRIuFAST32 " MHz\n", allwnr_t507_get_gpu_freq() / 1000 / 1000);

	// https://github.com/bakhi/GPUReplay/blob/accce5d2bcbe5794b895156997f50a6fda86a87c/replayer/include/midgard/mali_kbase_gpu_id.h#L26

	// Mali G31 MP2 (Panfrost)
	PRINTF("board_gpu_initialize: GPU_ID=0x%08X (expected 0x%08X)\n", (unsigned) GPU_CONTROL->GPU_ID, 0x70930000);

	arm_hardware_set_handler_system(GPU_IRQn, GPU_IRQHandler);
	arm_hardware_set_handler_system(GPU_EVENT_IRQn, GPU_EVENT_IRQHandler);
	arm_hardware_set_handler_system(GPU_JOB_IRQn, GPU_JOB_IRQHandler);
	arm_hardware_set_handler_system(GPU_MMU_IRQn, GPU_MMU_IRQHandler);

	GPU_CONTROL->GPU_IRQ_CLEAR = GPU_IRQ_REG_ALL;
	GPU_CONTROL->GPU_IRQ_MASK = GPU_IRQ_REG_ALL;

	GPU_JOB_CONTROL->JOB_IRQ_CLEAR = 0xFFFFFFFF;
	GPU_JOB_CONTROL->JOB_IRQ_MASK = 0xFFFFFFFF;

	GPU_MMU->MMU_IRQ_CLEAR = 0xFFFFFFFF;
	GPU_MMU->MMU_IRQ_MASK = 0xFFFFFFFF;

	gpu_command(GPU_COMMAND_HARD_RESET);
	gpu_wait(RESET_COMPLETED);
	gpu_command(GPU_COMMAND_SOFT_RESET);
	gpu_wait(RESET_COMPLETED);
	gpu_command(GPU_COMMAND_NOP);

	// https://elixir.bootlin.com/linux/latest/source/drivers/gpu/drm/panfrost/panfrost_mmu.c

	PRINTF("board_gpu_initialize done.\n");
}

#elif CPUSTYLE_STM32MP1

void GPU_IRQHandler(void)
{
	PRINTF("GPU_IRQHandler\n");
}

// Graphic processor unit
void board_gpu_initialize(void)
{
	PRINTF("board_gpu_initialize start.\n");

	RCC->MP_AHB6ENSETR = RCC_MP_AHB6ENSETR_GPUEN;
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6LPENSETR = RCC_MP_AHB6LPENSETR_GPULPEN;
	(void) RCC->MP_AHB6LPENSETR;

	PRINTF("board_gpu_initialize: PRODUCTID=%08lX\n", (unsigned long) GPU->PRODUCTID);

//
	arm_hardware_set_handler_system(GPU_IRQn, GPU_IRQHandler);

	PRINTF("board_gpu_initialize done.\n");
}

#endif /* WITHGPUHW */
