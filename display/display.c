/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//
// Доработки для LS020 Василий Линывый, livas60@mail.ru
//

#include "hardware.h"
#include "board.h"
#include "display.h"
#include "formats.h"
#include <string.h>
#include "../inc/spi.h"	// hardware_spi_master_send_frame

#if LCDMODE_LTDC && ! defined (SDRAM_BANK_ADDR)
	// буфер экрана
	RAMFRAMEBUFF ALIGNX_BEGIN volatile FRAMEBUFF_T framebuff0 ALIGNX_END;	//L8 (8-bit Luminance or CLUT)
#endif /* LCDMODE_LTDC */

/*
	Dead time value in the AXI clock cycle inserted between two consecutive accesses on
	the AXI master port. These bits represent the minimum guaranteed number of cycles
	between two consecutive AXI accesses
 */
#define DMA2D_AMTCR_DT_VALUE 255	/* 0..255 */
#define DMA2D_AMTCR_DT_ENABLE 1	/* 0..1 */

#define MDMA_CH	MDMA_Channel0
#define MDMA_CTCR_xSIZE_RGB565	0x01	// 2 byte
#define MDMA_CCR_PL_VALUE 0	// PL: prioruty 0..3: min..max

#if LCDMODE_LTDC_L24
	#define DMA2D_FGPFCCR_CM_VALUE	(1 * DMA2D_FGPFCCR_CM_0)	/* 0001: RGB888 */
	#define DMA2D_OPFCCR_CM_VALUE	(1 * DMA2D_OPFCCR_CM_0)	/* 001: RGB888 */
#elif LCDMODE_LTDC_L8
	#define DMA2D_FGPFCCR_CM_VALUE	(5 * DMA2D_FGPFCCR_CM_0)	/* 0101: L8 */
	#define MDMA_CTCR_xSIZE			0x00	// 1 byte
	//#define DMA2D_OPFCCR_CM_VALUE	(x * DMA2D_OPFCCR_CM_0)	/* not supported */
#else /* LCDMODE_LTDC_L8 */
	#define DMA2D_FGPFCCR_CM_VALUE	(2 * DMA2D_FGPFCCR_CM_0)	/* 0010: RGB565 */
	#define DMA2D_OPFCCR_CM_VALUE	(2 * DMA2D_OPFCCR_CM_0)	/* 010: RGB565 */
	#define MDMA_CTCR_xSIZE			0x01	// 2 byte
#endif /* LCDMODE_LTDC_L8 */

static void
ltdc_horizontal_pixels(
	volatile PACKEDCOLOR_T * tgr,		// target raster
	const FLASHMEM uint8_t * raster,
	uint_fast16_t width	// number of bits (start from LSB first byte in raster)
	);

static uint_fast8_t
mdma_getbus(uintptr_t addr)
{
#if CPUSTYLE_STM32H7XX
	addr &= 0xFF000000uL;
	return (addr == 0x00000000uL || addr == 0x20000000uL);
#elif CPUSTYLE_STM32MP1
	return 1;
	addr &= 0xFF000000uL;
	return (addr == 0x00000000uL || addr == 0x20000000uL);
#else
	return 0;
#endif
}


static uint_fast8_t
mdma_tlen(uint_fast32_t nb, uint_fast8_t ds)
{
	return 32; //128;
	return (nb < 128 ? nb : 128) / ds * ds;
}

// DBURST value must be programmed as to ensure that the burst size is lower than the
// Transfer Length. If this is not ensured, the result is unpredictable.

// И еще несколько условий.. лучше в 0 оставить.

static uint_fast8_t
mdma_getburst(uint_fast16_t w, uint_fast8_t force0)
{
	if (force0)
		return 0;
	return 4;	// if RAMFRAMEBUFF used for tgcolor. Then RAMDTCM - 6 is valid
	if (w >= 128)
		return 7;
	if (w >= 64)
		return 6;
	if (w >= 32)
		return 5;
	if (w >= 16)
		return 4;
	if (w >= 8)
		return 3;
	if (w >= 4)
		return 2;
	if (w >= 2)
		return 1;
	return 0;
}

/* заполнение прямоугольной области буфера цветом в представлении по умолчанию. DMA2D не умеет 8-bit пиксели */
static void RAMFUNC_NONILINE display_fillrect_main(
	volatile PACKEDCOLOR_T * buffer,
	uint_fast16_t dx,	// размеры буфера
	uint_fast16_t dy,
	uint_fast16_t col,	// позиция окна в буфере,
	uint_fast16_t row,
	uint_fast16_t w,	// размер окна
	uint_fast16_t h,
	COLOR_T fgcolor,
	COLOR_T bgcolor,
	uint_fast8_t hpattern	// horizontal pattern (LSB - left)
	)
{
	if (w == 0 || h == 0)
		return;

#if WITHMDMAHW
	static RAMFRAMEBUFF ALIGNX_BEGIN volatile PACKEDCOLOR_T tgcolor ALIGNX_END;	/* значение цвета для заполнения области памяти */
	tgcolor = fgcolor;

	arm_hardware_flush((uintptr_t) & tgcolor, sizeof tgcolor);

	arm_hardware_flush((uintptr_t) buffer, sizeof (* buffer) * dx * dy);

	MDMA_CH->CCR &= ~ MDMA_CCR_EN_Msk;
	MDMA_CH->CDAR = (uintptr_t) & buffer [row * dx + col];
	MDMA_CH->CSAR = (uintptr_t) & tgcolor;
	const uint_fast8_t tlen = mdma_tlen(w * sizeof (* buffer), sizeof (* buffer));
	const uint_fast8_t sburst = mdma_getburst(w, mdma_getbus(MDMA_CH->CSAR));
	const uint_fast8_t dburst = mdma_getburst(w, mdma_getbus(MDMA_CH->CDAR));
	MDMA_CH->CTCR =
		(0x00 << MDMA_CTCR_SINC_Pos) | 	// Source increment mode: 00: Source address pointer is fixed
		(MDMA_CTCR_xSIZE << MDMA_CTCR_SSIZE_Pos) |
		(0x00 << MDMA_CTCR_SINCOS_Pos) |
		(sburst << MDMA_CTCR_SBURST_Pos) |
		(0x02 << MDMA_CTCR_DINC_Pos) |	// Destination increment mode: 10: Destination address pointer is incremented
		(MDMA_CTCR_xSIZE << MDMA_CTCR_DSIZE_Pos) |
		(0x00 << MDMA_CTCR_DINCOS_Pos) |
		(dburst << MDMA_CTCR_DBURST_Pos) |	// Destination burst transfer configuration
		((tlen - 1) << MDMA_CTCR_TLEN_Pos) |		// buffer Transfer Length (number of bytes - 1)
		(0x00 << MDMA_CTCR_PKE_Pos) |
		(0x00 << MDMA_CTCR_PAM_Pos) |
		(0x02 << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01 << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01 << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((sizeof (* buffer) * sizeof (* buffer) * (w)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00 << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00 << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((h - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((sizeof (* buffer) * (0)) << MDMA_CBRUR_SUV_Pos) |				// Source address Update Value
		((sizeof (* buffer) * (dx - w)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	//PRINTF("w=%d, MDMA_CH->CDAR=%08lX, CSAR=%08lX, MDMA_CH->CBNDTR=%08lX\n", w, MDMA_CH->CDAR, MDMA_CH->CSAR, MDMA_CH->CBNDTR);
	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(mdma_getbus(MDMA_CH->CSAR) << MDMA_CTBR_SBUS_Pos) |
		(mdma_getbus(MDMA_CH->CDAR) << MDMA_CTBR_DBUS_Pos) |
		0;

	//TP();
	MDMA_CH->CIFCR = MDMA_CIFCR_CLTCIF_Msk | MDMA_CIFCR_CBTIF_Msk |
					MDMA_CIFCR_CBRTIF_Msk | MDMA_CIFCR_CCTCIF_Msk | MDMA_CIFCR_CTEIF_Msk;
	// Set priority
	MDMA_CH->CCR = (MDMA_CH->CCR & ~ (MDMA_CCR_PL_Msk)) |
			(MDMA_CCR_PL_VALUE < MDMA_CCR_PL_Pos) |
			0;
	MDMA_CH->CCR |= MDMA_CCR_EN_Msk;
	/* start transfer */
	MDMA_CH->CCR |= MDMA_CCR_SWRQ_Msk;
	/* wait for complete */
	while ((MDMA_CH->CISR & MDMA_CISR_CTCIF_Msk) == 0)	// Channel x Channel Transfer Complete interrupt flag
		;
	//TP();

#elif WITHDMA2DHW && ! LCDMODE_LTDC_L8

	// just writes the color defined in the DMA2D_OCOLR register 
	// to the area located at the address pointed by the DMA2D_OMAR 
	// and defined in the DMA2D_NLR and DMA2D_OOR.

	arm_hardware_flush((uintptr_t) buffer, sizeof (* buffer) * dx * dy);

	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) & buffer [row * dx + col];
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((dx - w) << DMA2D_OOR_LO_Pos) |
		0;

	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		(h << DMA2D_NLR_NL_Pos) |
		(w << DMA2D_NLR_PL_Pos) |
		0;

	DMA2D->OCOLR = 
		fgcolor |
		0;

	DMA2D->OPFCCR = (DMA2D->OPFCCR & ~ (DMA2D_OPFCCR_CM)) |
		(DMA2D_OPFCCR_CM_VALUE) |	/* framebuffer pixel format */
		0;

	/* set AXI master timer */
	DMA2D->AMTCR = (DMA2D->AMTCR & ~ (DMA2D_AMTCR_DT | DMA2D_AMTCR_EN)) |
		(DMA2D_AMTCR_DT_VALUE << DMA2D_AMTCR_DT_Pos) |
		DMA2D_AMTCR_DT_ENABLE * DMA2D_AMTCR_EN |
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		3 * DMA2D_CR_MODE_0 |	// 11: Register-to-memory (no FG nor BG, only output stage active)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;


#else /* WITHDMA2DHW*/

	const uint_fast16_t tail = dx - w;	// сколько надо прибавить к указателю буфера после заполнения, чтобы оказатся в начале области в следующей строке
	buffer += (dx * row) + col;	// начальная позиция в буфере
	if (hpattern == 0xFF)
	{
		// foreground color fill
		while (h --)
		{
			volatile PACKEDCOLOR_T * const startmem = buffer;
#if LCDMODE_LTDC_L8
			memset((void *) buffer, fgcolor, w);
			buffer += dx;
#else /* LCDMODE_LTDC_L8 */
			uint_fast16_t n = w;
			while (n --)
				* buffer ++ = fgcolor;
			buffer += tail;
#endif /* LCDMODE_LTDC_L8 */
			arm_hardware_flush((uintptr_t) startmem, sizeof (* startmem) * w);
		}
	}
	else if (hpattern == 0x00)
	{
		// background color fill
		while (h --)
		{
			volatile PACKEDCOLOR_T * const startmem = buffer;
#if LCDMODE_LTDC_L8
			memset((void *) buffer, bgcolor, w);
			buffer += dx;
#else /* LCDMODE_LTDC_L8 */
			uint_fast16_t n = w;
			while (n --)
				* buffer ++ = bgcolor;
			buffer += tail;
#endif /* LCDMODE_LTDC_L8 */
			arm_hardware_flush((uintptr_t) startmem, sizeof (* startmem) * w);
		}
	}
	else
	{
		const uint_fast8_t pat = ((hpattern << 8) | hpattern) >> (col % 8);
		// Dotted horizontal line
		enum { BUFLEN = (DIM_X + 7) / 8 + 1 };	// размер буфера с черно-белым растром - единица добавляется для случая когда x отрисовки не кратно 8
		static uint_fast8_t lasthpattern;	// паттерн для которого выполняли заполнение буфера растра
		static uint8_t raster [BUFLEN];
		if (lasthpattern != pat)
		{
			lasthpattern = pat;
			memset(raster, pat, BUFLEN);
		}
		// заполнение области экрана
		while (h --)
		{
	#if LCDMODE_HORFILL
		// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
			ltdc_horizontal_pixels(buffer, raster, w);
	#else /* LCDMODE_HORFILL */
	#endif /* LCDMODE_HORFILL */
			buffer += dx;
		}
	}

#endif /* WITHDMA2DHW */
}


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
}

#endif /* WITHDMA2DHW */

#if WITHMDMAHW

void arm_hardware_mdma_initialize(void)
{
#if CPUSTYLE_STM32MP1
	/* Enable the DMA2D Clock */
	RCC->MP_AHB6ENSETR |= RCC_MC_AHB6ENSETR_MDMAEN;	/* MDMA clock enable */
	(void) RCC->MP_AHB6ENSETR;
	RCC->MP_AHB6LPENSETR |= RCC_MC_AHB6LPENSETR_MDMALPEN;	/* MDMA clock enable */
	(void) RCC->MP_AHB6LPENSETR;
	//RCC->MP_TZAHB6ENSETR |= RCC_MP_TZAHB6ENSETR_MDMAEN;
	//(void) RCC->MP_TZAHB6ENSETR;

#elif CPUSTYLE_STM32H7XX
	/* Enable the DMA2D Clock */
	RCC->AHB3ENR |= RCC_AHB3ENR_MDMAEN_Msk;	/* MDMA clock enable */
	(void) RCC->AHB3ENR;
	RCC->AHB3LPENR |= RCC_AHB3LPENR_MDMALPEN_Msk;
	(void) RCC->AHB3LPENR;

#else /* CPUSTYLE_STM32H7XX */
	/* Enable the DMA2D Clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_MDMAEN;	/* MDMA clock enable */
	(void) RCC->AHB1ENR;
	RCC->AHB3LPENR |= RCC_AHB3LPENR_MDMALPEN_Msk;
	(void) RCC->AHB3LPENR;

#endif /* CPUSTYLE_STM32H7XX */
}

#endif /* WITHMDMAHW */


/* заполнение прямоугольной области буфера цветом */
static void
hwaccel_fillrect_RGB565(
	volatile PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	// размеры буфера
	uint_fast16_t dy,
	uint_fast16_t col,	// позиция окна в буфере,
	uint_fast16_t row,
	uint_fast16_t w,	// размер окна
	uint_fast16_t h,
	COLOR565_T color
	)
{
	enum { PIXSIZE = sizeof (* buffer) };
	if (w == 0 || h == 0)
		return;
#if WITHMDMAHW

	// используется software triggered repeated block transfer
	//dx=480,dy=272,col=0,row=0,w=480,h=272;

	static RAMFRAMEBUFF ALIGNX_BEGIN volatile PACKEDCOLOR565_T tgcolor ALIGNX_END;	/* значение цвета для заполнения области памяти */
	tgcolor = color;

	arm_hardware_flush((uintptr_t) & tgcolor, sizeof tgcolor);

	arm_hardware_flush((uintptr_t) buffer, sizeof (* buffer) * GXSIZE(dx, dy));

	MDMA_CH->CCR &= ~ MDMA_CCR_EN_Msk;
	//while ((MDMA_CH->CCR & MDMA_CCR_EN_Msk) != 0)
	//	;
	MDMA_CH->CDAR = (uintptr_t) & buffer [row * dx + col];
	MDMA_CH->CSAR = (uintptr_t) & tgcolor;
	const uint_fast8_t tlen = mdma_tlen(w * sizeof (* buffer), sizeof (* buffer));
	const uint_fast8_t sburst = mdma_getburst(w, mdma_getbus(MDMA_CH->CSAR));
	const uint_fast8_t dburst = mdma_getburst(w, mdma_getbus(MDMA_CH->CDAR));
	MDMA_CH->CTCR =
		(0x00 << MDMA_CTCR_SINC_Pos) | 	// Source increment mode: 00: Source address pointer is fixed
		(MDMA_CTCR_xSIZE_RGB565 << MDMA_CTCR_SSIZE_Pos) |
		(0x00 << MDMA_CTCR_SINCOS_Pos) |
		(sburst << MDMA_CTCR_SBURST_Pos) |
		(0x02 << MDMA_CTCR_DINC_Pos) |	// Destination increment mode: 10: Destination address pointer is incremented
		(MDMA_CTCR_xSIZE_RGB565 << MDMA_CTCR_DSIZE_Pos) |
		(0x00 << MDMA_CTCR_DINCOS_Pos) |
		(dburst << MDMA_CTCR_DBURST_Pos) |	// Destination burst transfer configuration
		((tlen - 1) << MDMA_CTCR_TLEN_Pos) |		// buffer Transfer Length (number of bytes - 1)
		(0x00 << MDMA_CTCR_PKE_Pos) |
		(0x00 << MDMA_CTCR_PAM_Pos) |
		(0x02 << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01 << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01 << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((sizeof (* buffer) * sizeof (* buffer) * (w)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00 << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00 << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((h - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((sizeof (* buffer) * (0)) << MDMA_CBRUR_SUV_Pos) |				// Source address Update Value
		((sizeof (* buffer) * (dx - w)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(mdma_getbus(MDMA_CH->CSAR) << MDMA_CTBR_SBUS_Pos) |
		(mdma_getbus(MDMA_CH->CDAR) << MDMA_CTBR_DBUS_Pos) |
		0;

	//TP();
	//PRINTF("dx=%d,dy=%d,x=%d,y=%d,w=%d,h=%d, CSAR=%08lX, CDAR=%08lX\n", dx, dy, col, row, w, h, MDMA_CH->CSAR, MDMA_CH->CDAR);
	MDMA_CH->CIFCR = MDMA_CIFCR_CLTCIF_Msk | MDMA_CIFCR_CBTIF_Msk |
					MDMA_CIFCR_CBRTIF_Msk | MDMA_CIFCR_CCTCIF_Msk | MDMA_CIFCR_CTEIF_Msk;
	// Set priority
	MDMA_CH->CCR = (MDMA_CH->CCR & ~ (MDMA_CCR_PL_Msk)) |
			(MDMA_CCR_PL_VALUE < MDMA_CCR_PL_Pos) |
			0;
	MDMA_CH->CCR |= MDMA_CCR_EN_Msk;
	/* start transfer */
	MDMA_CH->CCR |= MDMA_CCR_SWRQ_Msk;
	/* wait for complete */
	while ((MDMA_CH->CISR & MDMA_CISR_CTCIF_Msk) == 0)	// Channel x Channel Transfer Complete interrupt flag
		; // PRINTF("MDMA_CH->CISR=%08lX ", MDMA_CH->CISR)
	//local_delay_ms(1250);
	//TP();

#elif WITHDMA2DHW

	// just writes the color defined in the DMA2D_OCOLR register 
	// to the area located at the address pointed by the DMA2D_OMAR 
	// and defined in the DMA2D_NLR and DMA2D_OOR.

	arm_hardware_flush((uintptr_t) buffer, sizeof (* buffer) * GXSIZE(dx, dy));

	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) & buffer [row * dx + col];
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((dx - w) << DMA2D_OOR_LO_Pos) |
		0;

	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		(h << DMA2D_NLR_NL_Pos) |
		(w << DMA2D_NLR_PL_Pos) |
		0;

	DMA2D->OCOLR = 
		color |
		0;

	DMA2D->OPFCCR = (DMA2D->OPFCCR & ~ (DMA2D_OPFCCR_CM)) |
		(2 * DMA2D_FGPFCCR_CM_0) |	/* 010: RGB565 Color mode - framebuffer pixel format */
		0;

	/* set AXI master timer */
	DMA2D->AMTCR = (DMA2D->AMTCR & ~ (DMA2D_AMTCR_DT | DMA2D_AMTCR_EN)) |
		(DMA2D_AMTCR_DT_VALUE << DMA2D_AMTCR_DT_Pos) |
		DMA2D_AMTCR_DT_ENABLE * DMA2D_AMTCR_EN |
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		3 * DMA2D_CR_MODE_0 |	// 11: Register-to-memory (no FG nor BG, only output stage active)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;

#else /* WITHDMA2DHW */

	const unsigned t = dx - w;
	buffer += (dx * row) + col;
	while (h --)
	{
		volatile PACKEDCOLOR565_T * startmem = buffer;

		unsigned n = w;
		while (n --)
			* buffer ++ = color;
		buffer += t;
		arm_hardware_flush((uintptr_t) startmem, sizeof (* startmem) * w);
	}



#endif /* WITHDMA2DHW */
}

#if LCDMODE_COLORED
static COLOR_T bgcolor;
#endif /* LCDMODE_COLORED */

void 
display_setbgcolor(COLOR_T c)
{
#if LCDMODE_COLORED
	bgcolor = c;
#endif /* LCDMODE_COLORED */
}

COLOR_T 
display_getbgcolor(void)
{
#if LCDMODE_COLORED
	return bgcolor;
#else /* LCDMODE_COLORED */
	return COLOR_BLACK;
#endif /* LCDMODE_COLORED */
}



// Используется при выводе на графический индикатор,
void
display_string(const char * s, uint_fast8_t lowhalf)
{
	char c;

	display_wrdata_begin();
	while((c = * s ++) != '\0') 
		display_put_char_small(c, lowhalf);
	display_wrdata_end();
}

// Используется при выводе на графический индикатор,
void
display_string_P(const FLASHMEM  char * s, uint_fast8_t lowhalf)
{
	char c;

	display_wrdata_begin();
	while((c = * s ++) != '\0') 
		display_put_char_small(c, lowhalf);
	display_wrdata_end();
}

// Используется при выводе на графический индикатор,
// самый маленький шрифт
void
display_string2(const char * s, uint_fast8_t lowhalf)
{
	char c;

	display_wrdata2_begin();
	while((c = * s ++) != '\0') 
		display_put_char_small2(c, lowhalf);
	display_wrdata2_end();
}



// Используется при выводе на графический индикатор,
// самый маленький шрифт
void
display_string2_P(const FLASHMEM  char * s, uint_fast8_t lowhalf)
{
	char c;

	display_wrdata2_begin();
	while((c = * s ++) != '\0') 
		display_put_char_small2(c, lowhalf);
	display_wrdata2_end();
}


// Выдача строки из ОЗУ в указанное место экрана.
void 
//NOINLINEAT
display_at(uint_fast8_t x, uint_fast8_t y, const char * s)
{
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{

		display_gotoxy(x, y + lowhalf);
		display_string(s, lowhalf);

	} while (lowhalf --);
}

// Выдача строки из ПЗУ в указанное место экрана.
void 
//NOINLINEAT
display_at_P(uint_fast8_t x, uint_fast8_t y, const FLASHMEM char * s)
{
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{

		display_gotoxy(x, y + lowhalf);
		display_string_P(s, lowhalf);

	} while (lowhalf --);
}

/* выдать на дисплей монохромный буфер с размерами dx * dy битов */
void display_showbuffer(
	const GX_t * buffer,
	unsigned dx,	// пиксели
	unsigned dy,	// пиксели
	uint_fast8_t col,	// сетка
	uint_fast8_t row	// сетка
	)
{
#if LCDMODE_S1D13781

	s1d13781_showbuffer(buffer, dx, dy, col, row);

#else /* LCDMODE_S1D13781 */

	#if WITHSPIHWDMA && (LCDMODE_UC1608 | 0)
		// на LCDMODE_S1D13781 почему-то DMA сбивает контроллер
		arm_hardware_flush((uintptr_t) buffer, sizeof (* buffer) * MGSIZE(dx, dy));	// количество байтов
	#endif

	uint_fast8_t lowhalf = (dy) / 8 - 1;
	if (lowhalf == 0)
		return;
	do
	{
		uint_fast8_t pos;
		const GX_t * const p = buffer + lowhalf * dx;	// начало данных горизонтальной полосы в памяти
		//debug_printf_P(PSTR("display_showbuffer: col=%d, row=%d, lowhalf=%d\n"), col, row, lowhalf);
		display_plotfrom(GRID2X(col), GRID2Y(row) + lowhalf * 8);		// курсор в начало первой строки
		// выдача горизонтальной полосы
		display_wrdatabar_begin();
	#if WITHSPIHWDMA && (0)
		// на LCDMODE_S1D13781 почему-то DMA сбивает контроллер
		// на LCDMODE_UC1608 портит мохранене теузей частоты и режима работы (STM32F746xx)
		hardware_spi_master_send_frame(p, dx);
	#else
		for (pos = 0; pos < dx; ++ pos)
			display_barcolumn(p [pos]);	// Выдать восемь цветных пикселей, младший бит - самый верхний в растре
	#endif
		display_wrdatabar_end();
	} while (lowhalf --);

#endif /* LCDMODE_S1D13781 */
}

#if LCDMODE_S1D13781

	// младший бит левее
	static const uint_fast16_t mapcolumn [16] =
	{
		0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080, // биты для манипуляций с видеобуфером
		0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000,
	};

#elif LCDMODE_UC1608 || LCDMODE_UC1601

	/* старшие биты соответствуют верхним пикселям изображения */
	// млдший бит ниже в растре
	static const uint_fast8_t mapcolumn [8] =
	{
		0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01, // биты для манипуляций с видеобуфером
	};
#else /* LCDMODE_UC1608 || LCDMODE_UC1601 */

	/* младшие биты соответствуют верхним пикселям изображения */
	// млдший бит выше в растре
	static const uint_fast8_t mapcolumn [8] =
	{
		0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, // биты для манипуляций с видеобуфером
	};

#endif /* LCDMODE_UC1608 || LCDMODE_UC1601 */

// начальная инициализация буфера
void display_colorbuffer_fill(
	PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	COLOR565_T color
	)
{
#if WITHDMA2DHW && LCDMODE_LTDC

	hwaccel_fillrect_RGB565(buffer, dx, dy, 0, 0, dx, dy, color);

#else /* WITHDMA2DHW && LCDMODE_LTDC */

	uint_fast32_t len = (uint_fast32_t) dx * dy;
	if (sizeof (* buffer) == 1)
	{
		memset((void *) buffer, color, len);
	}
	else if ((len & 0x07) == 0)
	{
		len /= 8;
		while (len --)
		{
			* buffer ++ = color;
			* buffer ++ = color;
			* buffer ++ = color;
			* buffer ++ = color;
			* buffer ++ = color;
			* buffer ++ = color;
			* buffer ++ = color;
			* buffer ++ = color;
		}
	}
	else if ((len & 0x03) == 0)
	{
		len /= 4;
		while (len --)
		{
			* buffer ++ = color;
			* buffer ++ = color;
			* buffer ++ = color;
			* buffer ++ = color;
		}
	}
	else
	{
		while (len --)
			* buffer ++ = color;
	}

#endif /* WITHDMA2DHW && LCDMODE_LTDC && ! LCDMODE_LTDC_L8 */
}

// поставить цветную точку.
void display_colorbuffer_set(
	PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLOR565_T color
	)
{
	//ASSERT(row < dy);
	//ASSERT(col < dx);
	//ASSERT(((uintptr_t) buffer & 0x01) == 0);
#if LCDMODE_HORFILL
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	buffer [dx * row + col] = color;
#else /* LCDMODE_HORFILL */
	// индекс младшей размерности перебирает вертикальную координату дисплея
	buffer [dy * col + row] = color;
#endif /* LCDMODE_HORFILL */
}

// поставить цветную точку.
void display_colorbuffer_xor(
	PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLOR565_T color
	)
{
	ASSERT(((uintptr_t) buffer & 0x01) == 0);
	//ASSERT(row < dy);
	//ASSERT(col < dx);
#if LCDMODE_HORFILL
	// индекс младшей размерности перебирает горизонтальную координату дисплея
	buffer [dx * row + col] ^= color;
#else /* LCDMODE_HORFILL */
	// индекс младшей размерности перебирает вертикальную координату дисплея
	buffer [dy * col + row] ^= color;
#endif /* LCDMODE_HORFILL */
}


// копирование в большее или равное окно
// размер пикселя - определяется конфигурацией.
static void hwaccel_copy_main(
	const PACKEDCOLOR_T * src,
	volatile PACKEDCOLOR_T * dst,
	unsigned w,
	unsigned t,	// разница в размере строки получателя от источника
	unsigned h
	)
{
	if (w == 0 || h == 0)
		return;
#if WITHMDMAHW
	MDMA_CH->CCR &= ~ MDMA_CCR_EN_Msk;
	MDMA_CH->CDAR = (uintptr_t) dst;
	MDMA_CH->CSAR = (uintptr_t) src;
	const uint_fast8_t tlen = mdma_tlen(w * sizeof (PACKEDCOLOR_T), sizeof (PACKEDCOLOR_T));
	const uint_fast8_t sburst = mdma_getburst(w, mdma_getbus(MDMA_CH->CSAR));
	const uint_fast8_t dburst = mdma_getburst(w, mdma_getbus(MDMA_CH->CDAR));
	MDMA_CH->CTCR =
		(0x02 << MDMA_CTCR_SINC_Pos) | 	// Source increment mode: 10: address pointer is incremented
		(MDMA_CTCR_xSIZE << MDMA_CTCR_SSIZE_Pos) |
		(0x00 << MDMA_CTCR_SINCOS_Pos) |
		(sburst << MDMA_CTCR_SBURST_Pos) |
		(0x02 << MDMA_CTCR_DINC_Pos) |	// Destination increment mode: 10: Destination address pointer is incremented
		(MDMA_CTCR_xSIZE << MDMA_CTCR_DSIZE_Pos) |
		(0x00 << MDMA_CTCR_DINCOS_Pos) |
		(dburst << MDMA_CTCR_DBURST_Pos) |	// Destination burst transfer configuration
		((tlen - 1) << MDMA_CTCR_TLEN_Pos) |		// buffer Transfer Length (number of bytes - 1)
		(0x00 << MDMA_CTCR_PKE_Pos) |
		(0x00 << MDMA_CTCR_PAM_Pos) |
		(0x02 << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01 << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01 << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((sizeof (PACKEDCOLOR_T) * sizeof (PACKEDCOLOR_T) * (w)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00 << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00 << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((h - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((sizeof (PACKEDCOLOR_T) * (0)) << MDMA_CBRUR_SUV_Pos) |		// Source address Update Value
		((sizeof (PACKEDCOLOR_T) * (t)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(mdma_getbus(MDMA_CH->CSAR) << MDMA_CTBR_SBUS_Pos) |
		(mdma_getbus(MDMA_CH->CDAR) << MDMA_CTBR_DBUS_Pos) |
		0;

	//TP();
	MDMA_CH->CIFCR = MDMA_CIFCR_CLTCIF_Msk | MDMA_CIFCR_CBTIF_Msk |
					MDMA_CIFCR_CBRTIF_Msk | MDMA_CIFCR_CCTCIF_Msk | MDMA_CIFCR_CTEIF_Msk;
	// Set priority
	MDMA_CH->CCR = (MDMA_CH->CCR & ~ (MDMA_CCR_PL_Msk)) |
			(MDMA_CCR_PL_VALUE < MDMA_CCR_PL_Pos) |
			0;
	MDMA_CH->CCR |= MDMA_CCR_EN_Msk;
	/* start transfer */
	MDMA_CH->CCR |= MDMA_CCR_SWRQ_Msk;
	/* wait for complete */
	while ((MDMA_CH->CISR & MDMA_CISR_CTCIF_Msk) == 0)	// Channel x Channel Transfer Complete interrupt flag
		;
	//TP();

#elif WITHDMA2DHW
	/* исходный растр */
	DMA2D->FGMAR = (uintptr_t) src;
	DMA2D->FGOR = (DMA2D->FGOR & ~ (DMA2D_FGOR_LO)) |
		(0 << DMA2D_FGOR_LO_Pos) |
		0;
	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) dst;
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((t) << DMA2D_OOR_LO_Pos) |
		0;
	/* размер пересылаемого растра */
	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		((h) << DMA2D_NLR_NL_Pos) |
		((w) << DMA2D_NLR_PL_Pos) |
		0;
	/* формат пикселя */
	DMA2D->FGPFCCR = (DMA2D->FGPFCCR & ~ (DMA2D_FGPFCCR_CM)) |
		DMA2D_FGPFCCR_CM_VALUE |	/* Color mode - framebuffer pixel format */
		0;

	/* set AXI master timer */
	DMA2D->AMTCR = (DMA2D->AMTCR & ~ (DMA2D_AMTCR_DT | DMA2D_AMTCR_EN)) |
		(DMA2D_AMTCR_DT_VALUE << DMA2D_AMTCR_DT_Pos) |
		DMA2D_AMTCR_DT_ENABLE * DMA2D_AMTCR_EN |
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		0 * DMA2D_CR_MODE_0 |	// 00: Memory-to-memory (FG fetch only)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;

#endif
}

// копирование в большее или равное окно
static void hwaccel_copy_RGB565(
	const PACKEDCOLOR565_T * src,
	volatile PACKEDCOLOR565_T * dst,
	unsigned w,
	unsigned t,	// разница в размере строки получателя от источника
	unsigned h
	)
{
	if (w == 0 || h == 0)
		return;
#if WITHMDMAHW
	MDMA_CH->CCR &= ~ MDMA_CCR_EN_Msk;
	MDMA_CH->CDAR = (uintptr_t) dst;
	MDMA_CH->CSAR = (uintptr_t) src;
	const uint_fast8_t tlen = mdma_tlen(w * sizeof (PACKEDCOLOR565_T), sizeof (PACKEDCOLOR565_T));
	const uint_fast8_t sburst = mdma_getburst(w, mdma_getbus(MDMA_CH->CSAR));
	const uint_fast8_t dburst = mdma_getburst(w, mdma_getbus(MDMA_CH->CDAR));
	MDMA_CH->CTCR =
		(0x02 << MDMA_CTCR_SINC_Pos) | 	// Source increment mode: 10: address pointer is incremented
		(MDMA_CTCR_xSIZE << MDMA_CTCR_SSIZE_Pos) |
		(0x00 << MDMA_CTCR_SINCOS_Pos) |
		(sburst << MDMA_CTCR_SBURST_Pos) |
		(0x02 << MDMA_CTCR_DINC_Pos) |	// Destination increment mode: 10: Destination address pointer is incremented
		(MDMA_CTCR_xSIZE << MDMA_CTCR_DSIZE_Pos) |
		(0x00 << MDMA_CTCR_DINCOS_Pos) |
		(dburst << MDMA_CTCR_DBURST_Pos) |	// Destination burst transfer configuration
		((tlen - 1) << MDMA_CTCR_TLEN_Pos) |		// buffer Transfer Length (number of bytes - 1)
		(0x00 << MDMA_CTCR_PKE_Pos) |
		(0x00 << MDMA_CTCR_PAM_Pos) |
		(0x02 << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01 << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01 << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((sizeof (PACKEDCOLOR565_T) * sizeof (PACKEDCOLOR565_T) * (w)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00 << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00 << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((h - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((sizeof (PACKEDCOLOR565_T) * (0)) << MDMA_CBRUR_SUV_Pos) |		// Source address Update Value
		((sizeof (PACKEDCOLOR565_T) * (t)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(mdma_getbus(MDMA_CH->CSAR) << MDMA_CTBR_SBUS_Pos) |
		(mdma_getbus(MDMA_CH->CDAR) << MDMA_CTBR_DBUS_Pos) |
		0;

	//TP();
	MDMA_CH->CIFCR = MDMA_CIFCR_CLTCIF_Msk | MDMA_CIFCR_CBTIF_Msk |
					MDMA_CIFCR_CBRTIF_Msk | MDMA_CIFCR_CCTCIF_Msk | MDMA_CIFCR_CTEIF_Msk;
	// Set priority
	MDMA_CH->CCR = (MDMA_CH->CCR & ~ (MDMA_CCR_PL_Msk)) |
			(MDMA_CCR_PL_VALUE < MDMA_CCR_PL_Pos) |
			0;
	MDMA_CH->CCR |= MDMA_CCR_EN_Msk;
	/* start transfer */
	MDMA_CH->CCR |= MDMA_CCR_SWRQ_Msk;
	/* wait for complete */
	while ((MDMA_CH->CISR & MDMA_CISR_CTCIF_Msk) == 0)	// Channel x Channel Transfer Complete interrupt flag
		;
	//TP();

#elif WITHDMA2DHW
	/* исходный растр */
	DMA2D->FGMAR = (uintptr_t) src;
	DMA2D->FGOR = (DMA2D->FGOR & ~ (DMA2D_FGOR_LO)) |
		(0 << DMA2D_FGOR_LO_Pos) |
		0;
	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) dst;
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((t) << DMA2D_OOR_LO_Pos) |
		0;
	/* размер пересылаемого растра */
	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		((h) << DMA2D_NLR_NL_Pos) |
		((w) << DMA2D_NLR_PL_Pos) |
		0;
	/* формат пикселя */
	DMA2D->FGPFCCR = (DMA2D->FGPFCCR & ~ (DMA2D_FGPFCCR_CM)) |
		DMA2D_FGPFCCR_CM_VALUE |	/* Color mode - framebuffer pixel format */
		0;

	/* set AXI master timer */
	DMA2D->AMTCR = (DMA2D->AMTCR & ~ (DMA2D_AMTCR_DT | DMA2D_AMTCR_EN)) |
		(DMA2D_AMTCR_DT_VALUE << DMA2D_AMTCR_DT_Pos) |
		DMA2D_AMTCR_DT_ENABLE * DMA2D_AMTCR_EN |
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		0 * DMA2D_CR_MODE_0 |	// 00: Memory-to-memory (FG fetch only)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;

#endif
}

#if LCDMODE_LTDC_PIP16

// Выдать буфер на дисплей
void display_colorbuffer_show(
	const PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата левого верхнего угла на экране (0..dx-1) слева направо
	uint_fast16_t row	// вертикальная координата левого верхнего угла на экране (0..dy-1) сверху вниз
	)
{
}

// установить данный буфер как область для PIP
void display_colorbuffer_pip(
	const PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy
	)
{
	arm_hardware_flush((uintptr_t) buffer, (uint_fast32_t) dx * dy * sizeof * buffer);
	arm_hardware_ltdc_pip_set((uintptr_t) buffer);
}

#elif LCDMODE_LTDC_L8

// Для L8 основного дисплея копирование в него RGB565 не очень простая задача...
// Выдать буфер на дисплей. Функции бывают только для не L8 режимов
void display_colorbuffer_show(
	const PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата левого верхнего угла на экране (0..dx-1) слева направо
	uint_fast16_t row	// вертикальная координата левого верхнего угла на экране (0..dy-1) сверху вниз
	)
{
}

#elif LCDMODE_LTDC_L24

// Для L24 RGB888 основного дисплея копирование в него RGB565 еще надо сделать... С этим справится DMA2D

#else

// Выдать буфер на дисплей. Функции бывают только для не L8 режимов
void display_colorbuffer_show(
	const PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата левого верхнего угла на экране (0..dx-1) слева направо
	uint_fast16_t row	// вертикальная координата левого верхнего угла на экране (0..dy-1) сверху вниз
	)
{
#if WITHMDMAHW && LCDMODE_LTDC

	arm_hardware_flush((uintptr_t) buffer, sizeof (* buffer) * GXSIZE(dx, dy));

	#if LCDMODE_HORFILL
		hwaccel_copy_RGB565(buffer, & framebuff [row] [col], dx, DIM_SECOND - dx, dy);
	#else /* LCDMODE_HORFILL */
		hwaccel_copy_RGB565(buffer, & framebuff [col] [row], dy, DIM_FIRST - dy, dx);
	#endif /* LCDMODE_HORFILL */

#elif WITHDMA2DHW && LCDMODE_LTDC

	arm_hardware_flush((uintptr_t) buffer, sizeof (* buffer) * GXSIZE(dx, dy));

	#if LCDMODE_HORFILL

		/* исходный растр */
		DMA2D->FGMAR = (uintptr_t) buffer;
		DMA2D->FGOR = (DMA2D->FGOR & ~ (DMA2D_FGOR_LO)) |
			(0 << DMA2D_FGOR_LO_Pos) |
			0;
		/* целевой растр */
		DMA2D->OMAR = (uintptr_t) & framebuff [row] [col];
		DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
			((DIM_X - dx) << DMA2D_OOR_LO_Pos) |
			0;
		/* размер пересылаемого растра */
		DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
			(dy << DMA2D_NLR_NL_Pos) |
			(dx << DMA2D_NLR_PL_Pos) |
			0;
		/* формат пикселя */
		DMA2D->FGPFCCR = (DMA2D->FGPFCCR & ~ (DMA2D_FGPFCCR_CM)) |
			DMA2D_FGPFCCR_CM_VALUE |	/* Color mode - framebuffer pixel format */
			0;

		/* set AXI master timer */
		DMA2D->AMTCR = (DMA2D->AMTCR & ~ (DMA2D_AMTCR_DT | DMA2D_AMTCR_EN)) |
			(DMA2D_AMTCR_DT_VALUE << DMA2D_AMTCR_DT_Pos) |
			DMA2D_AMTCR_DT_ENABLE * DMA2D_AMTCR_EN |
			0;

		/* запустить операцию */
		DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
			0 * DMA2D_CR_MODE_0 |	// 00: Memory-to-memory (FG fetch only)
			1 * DMA2D_CR_START |
			0;

		/* ожидаем выполнения операции */
		while ((DMA2D->CR & DMA2D_CR_START) != 0)
			;

	#else /* LCDMODE_HORFILL */
		#warning To be implemented
	#endif /* LCDMODE_HORFILL */

#else /* WITHDMA2DHW && LCDMODE_LTDC */

	#if LCDMODE_COLORED
		display_plotfrom(col, row);
		display_plotstart(dy);
		display_plot(buffer, dx, dy);
		display_plotstop();
	#endif
#endif /* WITHDMA2DHW && LCDMODE_LTDC */
}
#endif /* LCDMODE_LTDC_PIP16 */

// Routine to draw a line in the RGB565 color to the LCD.
// The line is drawn from (xmin,ymin) to (xmax,ymax).
// The algorithm used to draw the line is "Bresenham's line
// algorithm". 
#define SWAP(a, b)  do { (a) ^= (b); (b) ^= (a); (a) ^= (b); } while (0)
// Нарисовать линию указанным цветом
void display_colorbuffer_line_set(
	PACKEDCOLOR565_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t x0,	
	uint_fast16_t y0,
	uint_fast16_t x1,	
	uint_fast16_t y1,
	COLOR565_T color
	)
{
	int xmin = x0;
	int xmax = x1;
	int ymin = y0;
	int ymax = y1;
   int Dx = xmax - xmin; 
   int Dy = ymax - ymin;
   int steep = (abs(Dy) >= abs(Dx));
   if (steep) {
       SWAP(xmin, ymin);
       SWAP(xmax, ymax);
       // recompute Dx, Dy after swap
       Dx = xmax - xmin;
       Dy = ymax - ymin;
   }
   int xstep = 1;
   if (Dx < 0) {
       xstep = -1;
       Dx = -Dx;
   }
   int ystep = 1;
   if (Dy < 0) {
       ystep = -1;		
       Dy = -Dy; 
   }
   int TwoDy = 2*Dy; 
   int TwoDyTwoDx = TwoDy - 2*Dx; // 2*Dy - 2*Dx
   int E = TwoDy - Dx; //2*Dy - Dx
   int y = ymin;
   int xDraw, yDraw;
   int x;
   for (x = xmin; x != xmax; x += xstep) {		
       if (steep) {			
           xDraw = y;
           yDraw = x;
       } else {			
           xDraw = x;
           yDraw = y;
       }
       // plot
       //LCD_PlotPoint(xDraw, yDraw, color);
	   display_colorbuffer_set(buffer, dx, dy, xDraw, yDraw, color);
       // next
       if (E > 0) {
           E += TwoDyTwoDx; //E += 2*Dy - 2*Dx;
           y = y + ystep;
       } else {
           E += TwoDy; //E += 2*Dy;
       }
   }
}

#undef SWAP

// погасить точку
void display_pixelbuffer(
	GX_t * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row	// вертикальная координата пикселя (0..dy-1) сверху вниз
	)
{
#if LCDMODE_S1D13781

	//row = (dy - 1) - row;		// смена направления
	GX_t * const rowstart = buffer + row * ((dx + 15) / 16);	// начало данных строки растра в памяти
	GX_t * const p = rowstart + col / 16;	
	//* p |= mapcolumn [col % 16];	// установить точку
	* p &= ~ mapcolumn [col % 16];	// погасить точку
	//* p ^= mapcolumn [col % 16];	// инвертировать точку

#else /* LCDMODE_S1D13781 */

	//row = (dy - 1) - row;		// смена направления
	GX_t * const p = buffer + (row / 8) * dx + col;	// начало данных горизонтальной полосы в памяти
	//* p |= mapcolumn [row % 8];	// установить точку
	* p &= ~ mapcolumn [row % 8];	// погасить точку
	//* p ^= mapcolumn [row % 8];	// инвертировать точку

#endif /* LCDMODE_S1D13781 */
}

/* Исключающее ИЛИ с точкой в растре */
void display_pixelbuffer_xor(
	GX_t * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row	// вертикальная координата пикселя (0..dy-1) сверху вниз
	)
{
#if LCDMODE_S1D13781
	//row = (dy - 1) - row;		// смена направления
	GX_t * const rowstart = buffer + row * ((dx + 15) / 16);	// начало данных строки растра в памяти
	GX_t * const p = rowstart + col / 16;	
	//* p |= mapcolumn [col % 16];	// установить точку
	//* p &= ~ mapcolumn [col % 16];	// погасить точку
	* p ^= mapcolumn [col % 16];	// инвертировать точку

#else /* LCDMODE_S1D13781 */

	//row = (dy - 1) - row;		// смена направления
	GX_t * const p = buffer + (row / 8) * dx + col;	// начало данных горизонтальной полосы в памяти
	//* p |= mapcolumn [row % 8];	// установить точку
	//* p &= ~ mapcolumn [row % 8];	// погасить точку
	* p ^= mapcolumn [row % 8];	// инвертировать точку

#endif /* LCDMODE_S1D13781 */
}

void display_pixelbuffer_clear(
	GX_t * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy
	)
{
	memset(buffer, 0xFF, (size_t) MGSIZE(dx, dy) * (sizeof * buffer));			// рисование способом погасить точку
}

// Routine to draw a line in the RGB565 color to the LCD.
// The line is drawn from (xmin,ymin) to (xmax,ymax).
// The algorithm used to draw the line is "Bresenham's line
// algorithm". 
#define SWAP(a, b)  do { (a) ^= (b); (b) ^= (a); (a) ^= (b); } while (0)
// Нарисовать линию указанным цветом
void display_pixelbuffer_line(
	GX_t * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t x0,	
	uint_fast16_t y0,
	uint_fast16_t x1,	
	uint_fast16_t y1
	)
{
	int xmin = x0;
	int xmax = x1;
	int ymin = y0;
	int ymax = y1;
   int Dx = xmax - xmin; 
   int Dy = ymax - ymin;
   int steep = (abs(Dy) >= abs(Dx));
   if (steep) {
       SWAP(xmin, ymin);
       SWAP(xmax, ymax);
       // recompute Dx, Dy after swap
       Dx = xmax - xmin;
       Dy = ymax - ymin;
   }
   int xstep = 1;
   if (Dx < 0) {
       xstep = -1;
       Dx = -Dx;
   }
   int ystep = 1;
   if (Dy < 0) {
       ystep = -1;		
       Dy = -Dy; 
   }
   int TwoDy = 2*Dy; 
   int TwoDyTwoDx = TwoDy - 2*Dx; // 2*Dy - 2*Dx
   int E = TwoDy - Dx; //2*Dy - Dx
   int y = ymin;
   int xDraw, yDraw;
   int x;
   for (x = xmin; x != xmax; x += xstep) {		
       if (steep) {			
           xDraw = y;
           yDraw = x;
       } else {			
           xDraw = x;
           yDraw = y;
       }
       // plot
       //LCD_PlotPoint(xDraw, yDraw, color);
	   display_pixelbuffer(buffer, dx, dy, xDraw, yDraw);
       // next
       if (E > 0) {
           E += TwoDyTwoDx; //E += 2*Dy - 2*Dx;
           y = y + ystep;
       } else {
           E += TwoDy; //E += 2*Dy;
       }
   }
}
#undef SWAP

#if LCDMODE_LTDC

void display_putpixel(
	uint_fast16_t x,
	uint_fast16_t y,
	COLOR_T color
	)
{
#if LCDMODE_HORFILL
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	// индекс младшей размерности перебирает горизонтальную координату дисплея
	framebuff [y] [x] = color;
#else /* LCDMODE_HORFILL */
	framebuff [x] [y] = color;
#endif /* LCDMODE_HORFILL */
}

static void 
bitblt_fill(
	uint_fast16_t x, uint_fast16_t y, 	// координаты в пикселях
	uint_fast16_t w, uint_fast16_t h, 	// размеры в пикселях
	COLOR_T fgcolor,
	COLOR_T bgcolor,
	uint_fast8_t hpattern	// horizontal pattern (LSB - left)
	)
{

#if WITHMDMAHW && LCDMODE_LTDC

	display_fillrect_main(& framebuff [0] [0], DIM_X, DIM_Y, x, y, w, h, fgcolor, bgcolor, hpattern);

#elif WITHDMA2DHW && LCDMODE_LTDC && ! LCDMODE_LTDC_L8

	hwaccel_fillrect_RGB565(& framebuff [0] [0], DIM_X, DIM_Y, x, y, w, h, fgcolor);

#else /* WITHDMA2DHW && LCDMODE_LTDC && ! LCDMODE_LTDC_L8 */

	display_fillrect_main(& framebuff [0] [0], DIM_X, DIM_Y, x, y, w, h, fgcolor, bgcolor, hpattern);

#endif /* WITHDMA2DHW && LCDMODE_LTDC && ! LCDMODE_LTDC_L8 */
}

void display_solidbar(uint_fast16_t x, uint_fast16_t y, uint_fast16_t x2, uint_fast16_t y2, COLOR_T color)
{
	if (x2 < x)
	{
		uint_fast16_t t = x;
		x = x2, x2 = t;
	}
	if (y2 < y)
	{
		uint_fast16_t t = y;
		y = y2, y2 = t;
	}
	bitblt_fill(x, y, x2 - x, y2 - y, color, color, 0xFF);
}

#endif /* LCDMODE_LTDC */

/*
 * настройка портов для последующей работы с дополнительными (кроме последовательного канала)
 * сигналами дисплея.
 */
/* вызывается при запрещённых прерываниях. */
void display_hardware_initialize(void)
{
	debug_printf_P(PSTR("display_hardware_initialize start\n"));


#if WITHDMA2DHW
	// Image construction hardware
	arm_hardware_dma2d_initialize();
#endif /* WITHDMA2DHW */
#if WITHMDMAHW
	// Image construction hardware
	arm_hardware_mdma_initialize();
#endif
#if WITHLTDCHW
	// STM32xxx LCD-TFT Controller (LTDC)
	// RENESAS Video Display Controller 5
	arm_hardware_ltdc_initialize();
#endif /* WITHLTDCHW */

#if LCDMODE_HARD_SPI
#elif LCDMODE_HARD_I2C
#elif LCDMODE_LTDC
#else
	#if LCDMODE_HD44780 && (LCDMODE_SPI == 0)
		hd44780_io_initialize();
	#else /* LCDMODE_HD44780 && (LCDMODE_SPI == 0) */
		DISPLAY_BUS_INITIALIZE();	// see LCD_CONTROL_INITIALIZE, LCD_DATA_INITIALIZE_WRITE
	#endif /* LCDMODE_HD44780 && (LCDMODE_SPI == 0) */
#endif
	debug_printf_P(PSTR("display_hardware_initialize done\n"));
}

#if LCDMODE_LTDC

#if DSTYLE_G_X320_Y240
	// в знакогенераторе изображения символов "по вертикалти"
	// Для дисплеев 320 * 240
	#include "./fonts/ILI9341_font_small.h"
	#include "./fonts/ILI9341_font_half.h"
	#include "./fonts/ILI9341_font_big.h"

	#define	ls020_smallfont	ILI9341_smallfont
	#define	ls020_halffont	ILI9341_halffont
	#define	ls020_bigfont	ILI9341_bigfont

#elif DSTYLE_G_X480_Y272
	// в знакогенераторе изображения символов "по горизонтали"
	#include "./fonts/S1D13781_font_small_LTDC.h"
	#include "./fonts/S1D13781_font_half_LTDC.h"
	#include "./fonts/S1D13781_font_big_LTDC.h"

#elif DSTYLE_G_X800_Y480
	// в знакогенераторе изображения символов "по горизонтали"
	#include "./fonts/S1D13781_font_small_LTDC.h"
	#include "./fonts/S1D13781_font_half_LTDC.h"
	#include "./fonts/S1D13781_font_big_LTDC.h"

#else /*  */
	// в знакогенераторе изображения символов "по вертикалти"
	//#error Undefined display layout

	#include "./fonts/ls020_font_small.h"
	#include "./fonts/ls020_font_half.h"
	#include "./fonts/ls020_font_big.h"
#endif /* DSTYLE_G_X320_Y240 */


#if ! LCDMODE_LTDC_L24
#include "./byte2crun.h"
#endif /* ! LCDMODE_LTDC_L24 */


/* позиция в растре, куда будет выдаваться следующий пиксель */
static PACKEDCOLOR_T ltdc_fg, ltdc_bg;

#if ! LCDMODE_LTDC_L24
static const FLASHMEM PACKEDCOLOR_T (* byte2run) [256][8] = & byte2run_COLOR_WHITE_COLOR_BLACK;
#endif /* ! LCDMODE_LTDC_L24 */

static unsigned ltdc_first, ltdc_second;	// в пикселях
static unsigned ltdc_h;						// высота символа (полосы) в пикселях
static unsigned ltdc_secondoffs;			// в пикселях

void display_setcolors(COLOR_T fg, COLOR_T bg)
{

#if ! LCDMODE_LTDC_L24
	ltdc_fg = fg;
	ltdc_bg = bg;
#else /* ! LCDMODE_LTDC_L24 */

	ltdc_fg.r = fg >> 16;
	ltdc_fg.g = fg >> 8;
	ltdc_fg.b = fg >> 0;
	ltdc_bg.r = bg >> 16;
	ltdc_bg.g = bg >> 8;
	ltdc_bg.b = bg >> 0;

#endif /* ! LCDMODE_LTDC_L24 */

#if ! LCDMODE_LTDC_L24

	if (fg == COLOR_WHITE && bg == COLOR_DARKGREEN)
		byte2run = & byte2run_COLOR_WHITE_COLOR_DARKGREEN;
	else if (fg == COLOR_YELLOW && bg == COLOR_BLACK)
		byte2run = & byte2run_COLOR_YELLOW_COLOR_BLACK;
	else if (fg == COLOR_WHITE && bg == COLOR_BLACK)
		byte2run = & byte2run_COLOR_WHITE_COLOR_BLACK;
	else if (fg == COLOR_BLACK && bg == COLOR_GREEN)
		byte2run = & byte2run_COLOR_BLACK_COLOR_GREEN;
	else if (fg == COLOR_BLACK && bg == COLOR_RED)
		byte2run = & byte2run_COLOR_BLACK_COLOR_RED;
	else if (fg == COLOR_GREEN && bg == COLOR_BLACK)
		byte2run = & byte2run_COLOR_GREEN_COLOR_BLACK;
	else if (fg == COLOR_RED && bg == COLOR_BLACK)
		byte2run = & byte2run_COLOR_RED_COLOR_BLACK;
	else
		byte2run = & byte2run_COLOR_WHITE_COLOR_BLACK;

#endif /* ! LCDMODE_LTDC_L24 */

}

void display_setcolors3(COLOR_T fg, COLOR_T bg, COLOR_T fgbg)
{
	display_setcolors(fg, bg);
}

// Выдать один цветной пиксель
static void 
ltdc_pix1color(
	uint_fast8_t cgcol,		// смещение в пикселях относительно координат, поставленных display_gotoxy
	uint_fast8_t cgrow,
	PACKEDCOLOR_T color
	)
{
	volatile PACKEDCOLOR_T * const p = & framebuff [ltdc_first + cgrow] [ltdc_second + ltdc_secondoffs + cgcol];
	// размещаем пиксели по горизонтали
	//debug_printf_P(PSTR("framebuff=%p, ltdc_first=%d, cgrow=%d, ltdc_second=%d, ltdc_secondoffs=%d, cgcol=%d\n"), framebuff, ltdc_first, cgrow, ltdc_second, ltdc_secondoffs, cgcol);
	* p = color;
	arm_hardware_flush((uintptr_t) p, sizeof * p);
}


// Выдать один цветной пиксель (фон/символ)
static void 
ltdc_pixel(
	uint_fast8_t cgcol,		// смещение в пикселях относительно координат, поставленных display_gotoxy
	uint_fast8_t cgrow,
	uint_fast8_t v			// 0 - цвет background, иначе - foreground
	)
{
	ltdc_pix1color(cgcol, cgrow, v ? ltdc_fg : ltdc_bg);
}


// Выдать восемь цветных пикселей, младший бит - самый верхний в растре
static void 
ltdc_vertical_pixN(
	uint_fast8_t v,		// pattern
	uint_fast8_t w		// number of lower bits used in pattern
	)
{

#if LCDMODE_LTDC_L24 || LCDMODE_HORFILL

	// TODO: для паттернов шире чем восемь бит, повторить нужное число раз.
	ltdc_pixel(0, 0, v & 0x01);
	ltdc_pixel(0, 1, v & 0x02);
	ltdc_pixel(0, 2, v & 0x04);
	ltdc_pixel(0, 3, v & 0x08);
	ltdc_pixel(0, 4, v & 0x10);
	ltdc_pixel(0, 5, v & 0x20);
	ltdc_pixel(0, 6, v & 0x40);
	ltdc_pixel(0, 7, v & 0x80);

	++ ltdc_secondoffs;

#else /* LCDMODE_LTDC_L24 */
	// размещаем пиксели по горизонтали
	// TODO: для паттернов шире чем восемь бит, повторить нужное число раз.
	const FLASHMEM PACKEDCOLOR_T * const pcl = (* byte2run) [v];
	memcpy((void *) & framebuff [ltdc_first] [ltdc_second + ltdc_secondoffs], pcl, sizeof (* pcl) * w);
	arm_hardware_flush((uintptr_t) & framebuff [ltdc_first] [ltdc_second + ltdc_secondoffs], sizeof (PACKEDCOLOR_T) * w);
	if ((ltdc_secondoffs += 8) >= ltdc_h)
	{
		ltdc_secondoffs -= ltdc_h;
		++ ltdc_first;
	}
#endif /* LCDMODE_LTDC_L24 */
}


static uint_fast8_t
//NOINLINEAT
bigfont_decode(uint_fast8_t c)
{
	// '#' - узкий пробел
	if (c == ' ' || c == '#')
		return 11;
	if (c == '_')
		return 10;		// курсор - позиция редактирвания частоты
	if (c == '.')
		return 12;		// точка
	return c - '0';		// остальные - цифры 0..9
}


static uint_fast8_t
smallfont_decode(uint_fast8_t c)
{
	return c - ' ';
}

#if LCDMODE_HORFILL
// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд

static void RAMFUNC ltdc_horizontal_pixels(
	volatile PACKEDCOLOR_T * tgr,		// target raster
	const FLASHMEM uint8_t * raster,
	uint_fast16_t width	// number of bits (start from LSB first byte in raster)
	)
{
	uint_fast16_t col;
	uint_fast16_t w = width;

	for (col = 0; w >= 8; col += 8, w -= 8)
	{
		const FLASHMEM PACKEDCOLOR_T * const pcl = (* byte2run) [* raster ++];
		memcpy((void *) (tgr + col), pcl, sizeof (* tgr) * 8);
	}
	if (w != 0)
	{
		const FLASHMEM PACKEDCOLOR_T * const pcl = (* byte2run) [* raster ++];
		memcpy((void *) (tgr + col), pcl, sizeof (* tgr) * w);
	}
	arm_hardware_flush((uintptr_t) tgr, sizeof (* tgr) * width);
}

// Вызов этой функции только внутри display_wrdata_begin() и 	display_wrdata_end();
static void RAMFUNC_NONILINE ltdc_horizontal_put_char_small(char cc)
{
	const uint_fast8_t width = SMALLCHARW;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH; ++ cgrow)
	{
		volatile PACKEDCOLOR_T * const tgr = & framebuff [ltdc_first + cgrow] [ltdc_second];
		ltdc_horizontal_pixels(tgr, S1D13781_smallfont_LTDC [c] [cgrow], width);
	}
	ltdc_second += width;
}

// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
static void RAMFUNC_NONILINE ltdc_horizontal_put_char_big(char cc)
{
	const uint_fast8_t width = ((cc == '.' || cc == '#') ? BIGCHARW_NARROW  : BIGCHARW);	// полнаяширина символа в пикселях
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < BIGCHARH; ++ cgrow)
	{
		volatile PACKEDCOLOR_T * const tgr = & framebuff [ltdc_first + cgrow] [ltdc_second];
		ltdc_horizontal_pixels(tgr, S1D13781_bigfont_LTDC [c] [cgrow], width);
	}
	ltdc_second += width;
}

// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
static void RAMFUNC_NONILINE ltdc_horizontal_put_char_half(char cc)
{
	const uint_fast8_t width = HALFCHARW;
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < HALFCHARH; ++ cgrow)
	{
		volatile PACKEDCOLOR_T * const tgr = & framebuff [ltdc_first + cgrow] [ltdc_second];
		ltdc_horizontal_pixels(tgr, S1D13781_halffont_LTDC [c] [cgrow], width);
	}
	ltdc_second += width;
}

#else /* LCDMODE_HORFILL */

// Вызов этой функции только внутри display_wrdata_begin() и 	display_wrdata_end();
static void RAMFUNC_NONILINE ltdc_vertical_put_char_small(char cc)
{
	uint_fast8_t i = 0;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_smallfont [0] / sizeof ls020_smallfont [0] [0]) };
	const FLASHMEM uint8_t * const p = & ls020_smallfont [c] [0];
	
	for (; i < NBYTES; ++ i)
		ltdc_vertical_pixN(p [i], 8);	// Выдать восемь цветных пикселей, младший бит - самый верхний в растре
}

// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
static void RAMFUNC_NONILINE ltdc_vertical_put_char_big(char cc)
{
	// '#' - узкий пробел
	enum { NBV = (BIGCHARH / 8) }; // сколько байтов в одной вертикали
	uint_fast8_t i = NBV * ((cc == '.' || cc == '#') ? 12 : 0);	// начальная колонка знакогенератора, откуда начинать.
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_bigfont [0] / sizeof ls020_bigfont [0] [0]) };
	const FLASHMEM uint8_t * const p = & ls020_bigfont [c] [0];
	
	for (; i < NBYTES; ++ i)
		ltdc_vertical_pixN(p [i], 8);	// Выдать восемь цветных пикселей, младший бит - самый верхний в растре
}

// Вызов этой функции только внутри display_wrdatabig_begin() и display_wrdatabig_end();
static void RAMFUNC_NONILINE ltdc_vertical_put_char_half(char cc)
{
	uint_fast8_t i = 0;
    const uint_fast8_t c = bigfont_decode((unsigned char) cc);
	enum { NBYTES = (sizeof ls020_halffont [0] / sizeof ls020_halffont [0] [0]) };
	const FLASHMEM uint8_t * const p = & ls020_halffont [c] [0];
	
	for (; i < NBYTES; ++ i)
		ltdc_vertical_pixN(p [i], 8);	// Выдать восемь цветных пикселей, младший бит - самый верхний в растре
}

#endif /* LCDMODE_HORFILL */



/* копирование содержимого окна с перекрытием для водопада */
void
display_scroll_down(
	uint_fast16_t x0,	// левый верхний угол окна
	uint_fast16_t y0,	// левый верхний угол окна
	uint_fast16_t w, 	// до 65535 пикселей - ширина окна
	uint_fast16_t h, 	// до 65535 пикселей - высота окна
	uint_fast16_t n,	// количество строк прокрутки
	int_fast16_t hshift	// количество пиксеелей для сдвига влево (отрицательное число) или вправо (положительное).
	)
{
#if WITHDMA2DHW && LCDMODE_LTDC

#if LCDMODE_HORFILL
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	/* TODO: В DMA2D нет средств управления направлением пересылки, потому данный код копирует сам на себя данные (размножает) */
	/* исходный растр */
	DMA2D->FGMAR = (uintptr_t) & framebuff [y0 + 0] [x0];
	DMA2D->FGOR = (DMA2D->FGOR & ~ (DMA2D_FGOR_LO)) |
		((DIM_X - w) << DMA2D_FGOR_LO_Pos) |
		0;
	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) & framebuff [y0 + n] [x0];
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((DIM_X - w) << DMA2D_OOR_LO_Pos) |
		0;
	/* размер пересылаемого растра */
	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		((h - n) << DMA2D_NLR_NL_Pos) |
		(w << DMA2D_NLR_PL_Pos) |
		0;
	/* формат пикселя */
	DMA2D->FGPFCCR = (DMA2D->FGPFCCR & ~ (DMA2D_FGPFCCR_CM)) |
		DMA2D_FGPFCCR_CM_VALUE |	/* Color mode - framebuffer pixel format */
		0;

	/* set AXI master timer */
	DMA2D->AMTCR = (DMA2D->AMTCR & ~ (DMA2D_AMTCR_DT | DMA2D_AMTCR_EN)) |
		(DMA2D_AMTCR_DT_VALUE << DMA2D_AMTCR_DT_Pos) |
		DMA2D_AMTCR_DT_ENABLE * DMA2D_AMTCR_EN |
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		0 * DMA2D_CR_MODE_0 |	// 00: Memory-to-memory (FG fetch only)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;

#else /* LCDMODE_HORFILL */
#endif /* LCDMODE_HORFILL */

#endif /* WITHDMA2DHW && LCDMODE_LTDC */
}

/* копирование содержимого окна с перекрытием для водопада */
void
display_scroll_up(
	uint_fast16_t x0,	// левый верхний угол окна
	uint_fast16_t y0,	// левый верхний угол окна
	uint_fast16_t w, 	// до 65535 пикселей - ширина окна
	uint_fast16_t h, 	// до 65535 пикселей - высота окна
	uint_fast16_t n,	// количество строк прокрутки
	int_fast16_t hshift	// количество пиксеелей для сдвига влево (отрицательное число) или вправо (положительное).
	)
{
#if WITHDMA2DHW && LCDMODE_LTDC
#if LCDMODE_HORFILL
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд

	/* исходный растр */
	DMA2D->FGMAR = (uintptr_t) & framebuff [y0 + n] [x0];
	DMA2D->FGOR = (DMA2D->FGOR & ~ (DMA2D_FGOR_LO)) |
		((DIM_X - w) << DMA2D_FGOR_LO_Pos) |
		0;
	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) & framebuff [y0 + 0] [x0];
	DMA2D->OOR = (DMA2D->OOR & ~ (DMA2D_OOR_LO)) |
		((DIM_X - w) << DMA2D_OOR_LO_Pos) |
		0;
	/* размер пересылаемого растра */
	DMA2D->NLR = (DMA2D->NLR & ~ (DMA2D_NLR_NL | DMA2D_NLR_PL)) |
		((h - n) << DMA2D_NLR_NL_Pos) |
		(w << DMA2D_NLR_PL_Pos) |
		0;
	/* формат пикселя */
	DMA2D->FGPFCCR = (DMA2D->FGPFCCR & ~ (DMA2D_FGPFCCR_CM)) |
		DMA2D_FGPFCCR_CM_VALUE |	/* Color mode - framebuffer pixel format */
		0;

	/* set AXI master timer */
	DMA2D->AMTCR = (DMA2D->AMTCR & ~ (DMA2D_AMTCR_DT | DMA2D_AMTCR_EN)) |
		(DMA2D_AMTCR_DT_VALUE << DMA2D_AMTCR_DT_Pos) |
		DMA2D_AMTCR_DT_ENABLE * DMA2D_AMTCR_EN |
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		0 * DMA2D_CR_MODE_0 |	// 00: Memory-to-memory (FG fetch only)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;

#else /* LCDMODE_HORFILL */
#endif /* LCDMODE_HORFILL */
#endif /* WITHDMA2DHW && LCDMODE_LTDC */
}


/* индивидуальные функции драйвера дисплея - реализованы в соответствующем из файлов */
void display_clear(void)
{
	const COLOR_T bg = display_getbgcolor();

#if WITHMDMAHW && LCDMODE_LTDC

	display_fillrect_main(& framebuff [0] [0], DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, bg, bg, 0xFF);

#elif WITHDMA2DHW && LCDMODE_LTDC && ! LCDMODE_LTDC_L8

	hwaccel_fillrect_RGB565(& framebuff [0] [0], DIM_X, DIM_Y, 0, 0, DIM_X, DIM_Y, bg);

#elif LCDMODE_LTDC_L8 && LCDMODE_LTDC

	memset((void *) framebuff, bg, DIM_FIRST * DIM_SECOND);
	//memset((void *) framebuff, COLOR_WHITE, DIM_FIRST * DIM_SECOND);	// debug version
	arm_hardware_flush((uintptr_t) framebuff, sizeof framebuff);

#elif LCDMODE_LTDC_L24 && LCDMODE_LTDC

	uint_fast16_t i, j;
	// fill
	PACKEDCOLOR_T c;
	c.r = bg >> 16;
	c.g = bg >> 8;
	c.b = bg >> 0;
	for (i = 0; i < DIM_FIRST; ++ i)
	{
		for (j = 0; j < DIM_SECOND; ++ j)
			framebuff [i][j] = c;
	}

	arm_hardware_flush((uintptr_t) framebuff, sizeof framebuff);

#elif LCDMODE_LTDC

	uint_fast16_t i, j;
	// fill
	for (i = 0; i < DIM_FIRST; ++ i)
	{
		for (j = 0; j < DIM_SECOND; ++ j)
			framebuff [i][j] = bg;
	}

	arm_hardware_flush((uintptr_t) framebuff, sizeof framebuff);

#endif /* LCDMODE_LTDC_L8 */
}

static uint_fast8_t stored_xgrid, stored_ygrid;	// используется в display_dispbar

void display_gotoxy(uint_fast8_t x, uint_fast8_t y)
{
	stored_xgrid = x;	// используется в display_dispbar
	stored_ygrid = y;	// используется в display_dispbar

#if LCDMODE_HORFILL
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	ltdc_second = GRID2X(x);
	ltdc_first = GRID2Y(y);
#else /* LCDMODE_HORFILL */
	ltdc_first = GRID2X(x);
	ltdc_second = GRID2Y(y);
#endif /* LCDMODE_HORFILL */

	//debug_printf_P(PSTR("display_gotoxy: CHAR_H=%d, CHAR_W=%d, x=%d, y=%d, ltdc_first=%d, ltdc_second=%d\n"), CHAR_H, CHAR_W, x, y, ltdc_first, ltdc_second);
	ASSERT(ltdc_first < DIM_FIRST);
	ASSERT(ltdc_second < DIM_SECOND);
}

// Координаты в пикселях
void display_plotfrom(uint_fast16_t x, uint_fast16_t y)
{
#if LCDMODE_ILI8961 || LCDMODE_LQ043T3DX02K
	ltdc_second = x;
	ltdc_first = y;
#else
	ltdc_first = x;
	ltdc_second = y;
#endif
	//debug_printf_P(PSTR("display_gotoxy: CHAR_H=%d, CHAR_W=%d, x=%d, y=%d, ltdc_first=%d, ltdc_second=%d\n"), CHAR_H, CHAR_W, x, y, ltdc_first, ltdc_second);
	ASSERT(ltdc_first < DIM_FIRST);
	ASSERT(ltdc_second < DIM_SECOND);
}

void display_plotstart(
	uint_fast16_t height	// Высота окна в пикселях
	)
{

}

void display_plot(
	const PACKEDCOLOR_T * buffer, 
	uint_fast16_t dx,	// Размеры окна в пикселях
	uint_fast16_t dy
	)
{
#if LCDMODE_HORFILL

	#if WITHMDMAHW || (WITHDMA2DHW && ! LCDMODE_LTDC_L8)
		arm_hardware_flush((uintptr_t) buffer, sizeof (* buffer) * dx * dy);
		hwaccel_copy_main(buffer, & framebuff [ltdc_first] [ltdc_second], dx, DIM_SECOND - dx, dy);
		ltdc_first += dy;

	#else /* WITHMDMAHW || (WITHLTDCHW && ! LCDMODE_LTDC_L8) */
		// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
		const size_t len = dx * sizeof * buffer;
		while (dy --)
		{
			volatile PACKEDCOLOR_T * const p = & framebuff [ltdc_first] [ltdc_second];
			memcpy((void *) p, buffer, len);
			arm_hardware_flush((uintptr_t) p, len);
			buffer += dx;
			++ ltdc_first;
		}

	#endif /* WITHMDMAHW || (WITHLTDCHW && ! LCDMODE_LTDC_L8) */
#else /* LCDMODE_HORFILL */
	#if WITHMDMAHW || (WITHDMA2DHW && ! LCDMODE_LTDC_L8)

		arm_hardware_flush((uintptr_t) buffer, sizeof (* buffer) * dx * dy);
		hwaccel_copy_main(buffer, & framebuff [ltdc_first] [ltdc_second], dy, DIM_FIRST - dy, dx);
		ltdc_first += dx;

	#else /* WITHMDMAHW || (WITHLTDCHW && ! LCDMODE_LTDC_L8) */
		const size_t len = dy * sizeof * buffer;
		while (dx --)
		{
			volatile PACKEDCOLOR_T * const p = & framebuff [ltdc_first] [ltdc_second];
			memcpy((void *) p, buffer, len);
			arm_hardware_flush((uintptr_t) p, len);
			buffer += dy;
			++ ltdc_first;
		}

	#endif /* WITHMDMAHW || (WITHLTDCHW && ! LCDMODE_LTDC_L8) */
#endif /* LCDMODE_HORFILL */
}

void display_plotstop(void)
{

}

#if LCDMODE_HORFILL

// Вызовы этой функции (или группу вызовов) требуется "обрамить" парой вызовов
// display_wrdatabar_begin() и display_wrdatabar_end().
void display_dispbar(
	uint_fast8_t width,	/* количество знакомест, занимаемых индикатором */
	uint_fast8_t value,		/* значение, которое надо отобразить */
	uint_fast8_t tracevalue,		/* значение маркера, которое надо отобразить */
	uint_fast8_t topvalue,	/* значение, соответствующее полностью заполненному индикатору */
	uint_fast8_t vpattern,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL */
	uint_fast8_t patternmax,	/* DISPLAY_BAR_HALF или DISPLAY_BAR_FULL - для отображения запомненного значения */
	uint_fast8_t emptyp			/* паттерн для заполнения между штрихами */
	)
{
	ASSERT(value <= topvalue);
	ASSERT(tracevalue <= topvalue);
	const uint_fast16_t wfull = GRID2X(width);
	const uint_fast16_t h = SMALLCHARH; //GRID2Y(1);
	const uint_fast16_t x = GRID2X(stored_xgrid);
	const uint_fast16_t y = GRID2Y(stored_ygrid);
	const uint_fast16_t wpart = (uint_fast32_t) wfull * value / topvalue;
	const uint_fast16_t wmark = (uint_fast32_t) wfull * tracevalue / topvalue;
	const uint_fast8_t hpattern = 0x33;

	bitblt_fill(x, y, wpart, h, ltdc_fg, ltdc_bg, hpattern);
	bitblt_fill(x + wpart, y, wfull - wpart, h, ltdc_bg, ltdc_bg, 0x00);
	if (wmark < wfull && wmark >= wpart)
		bitblt_fill(x + wmark, y, 1, h, ltdc_fg, ltdc_bg, 0xFF);
}

#endif /* LCDMODE_HORFILL */

// самый маленький шрифт
void display_wrdata2_begin(void)
{
	ltdc_secondoffs = 0;
	ltdc_h = SMALLCHARH;
}

void display_wrdata2_end(void)
{
}

void display_put_char_small2(uint_fast8_t c, uint_fast8_t lowhalf)
{
#if LCDMODE_HORFILL
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	ltdc_horizontal_put_char_small(c);
#else /* LCDMODE_HORFILL */
	ltdc_vertical_put_char_small(c);
#endif /* LCDMODE_HORFILL */
}

// полоса индикатора
void display_wrdatabar_begin(void)
{
	ltdc_secondoffs = 0;
	ltdc_h = 8;
}

// Выдать восемь цветных пикселей, младший бит - самый верхний в растре
void display_barcolumn(uint_fast8_t pattern)
{
	ltdc_vertical_pixN(pattern, 8);	// Выдать восемь цветных пикселей, младший бит - самый верхний в растре
}

void display_wrdatabar_end(void)
{
}

// большие и средние цифры (частота)
void display_wrdatabig_begin(void)
{
	ltdc_secondoffs = 0;
	ltdc_h = BIGCHARH;
}

void display_put_char_big(uint_fast8_t c, uint_fast8_t lowhalf)
{
#if LCDMODE_HORFILL
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	ltdc_horizontal_put_char_big(c);
#else /* LCDMODE_HORFILL */
	ltdc_vertical_put_char_big(c);
#endif /* LCDMODE_HORFILL */
}

void display_put_char_half(uint_fast8_t c, uint_fast8_t lowhalf)
{
#if LCDMODE_HORFILL
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	ltdc_horizontal_put_char_half(c);
#else /* LCDMODE_HORFILL */
	ltdc_vertical_put_char_half(c);
#endif /* LCDMODE_HORFILL */
}

void display_wrdatabig_end(void)
{
}

// обычный шрифт
void display_wrdata_begin(void)
{
	ltdc_secondoffs = 0;
	ltdc_h = SMALLCHARH;
}

void display_put_char_small(uint_fast8_t c, uint_fast8_t lowhalf)
{
#if LCDMODE_HORFILL
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	ltdc_horizontal_put_char_small(c);
#else /* LCDMODE_HORFILL */
	ltdc_vertical_put_char_small(c);
#endif /* LCDMODE_HORFILL */
}

void display_wrdata_end(void)
{
}

#if LCDMODE_LQ043T3DX02K || LCDMODE_AT070TN90 || LCDMODE_AT070TNA2

// заглушки

/* аппаратный сброс дисплея - перед инициализаций */
/* вызывается при разрешённых прерываниях. */
void
display_reset(void)
{
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(1); // Delay 1ms
	board_lcd_reset(0); 	// Pull RST pin down
	board_update();
	local_delay_ms(10); // Delay 10ms
	board_lcd_reset(1); 	// Pull RST pin up
	board_update();
	local_delay_ms(50); // Delay 50 ms
}
/* вызывается при разрешённых прерываниях. */
void display_initialize(void)
{
}

void display_set_contrast(uint_fast8_t v)
{
}

/* Разряжаем конденсаторы питания */
void display_discharge(void)
{
}

#endif /* LCDMODE_LQ043T3DX02K */
#endif /* LCDMODE_LTDC */
