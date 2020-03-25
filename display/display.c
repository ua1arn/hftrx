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
#include "gui.h"
#include "formats.h"
#include <string.h>
#include "spi.h"	// hardware_spi_master_send_frame

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
#define MDMA_CCR_PL_VALUE 0	// PL: prioruty 0..3: min..max

#if LCDMODE_LTDC_L24
	#define DMA2D_FGPFCCR_CM_VALUE_MAIN	(1 * DMA2D_FGPFCCR_CM_0)	/* 0001: RGB888 */
	//#define DMA2D_OPFCCR_CM_VALUE	(1 * DMA2D_OPFCCR_CM_0)	/* 001: RGB888 */

#elif LCDMODE_LTDC_L8
	#define DMA2D_FGPFCCR_CM_VALUE_MAIN	(5 * DMA2D_FGPFCCR_CM_0)	/* 0101: L8 */
	#define MDMA_CTCR_xSIZE_MAIN			0x00	// 1 byte
	////#define DMA2D_OPFCCR_CM_VALUE	(x * DMA2D_OPFCCR_CM_0)	/* not supported */

#else /* LCDMODE_LTDC_L8 */
	#define DMA2D_FGPFCCR_CM_VALUE_MAIN	(2 * DMA2D_FGPFCCR_CM_0)	/* 0010: RGB565 */
	//#define DMA2D_OPFCCR_CM_VALUE	(2 * DMA2D_OPFCCR_CM_0)	/* 010: RGB565 */
	#define MDMA_CTCR_xSIZE_MAIN			0x01	// 2 byte

#endif /* LCDMODE_LTDC_L8 */

#define DMA2D_FGPFCCR_CM_VALUE_L24	(1 * DMA2D_FGPFCCR_CM_0)	/* 0001: RGB888 */
#define DMA2D_FGPFCCR_CM_VALUE_L16	(2 * DMA2D_FGPFCCR_CM_0)	/* 0010: RGB565 */
#define DMA2D_FGPFCCR_CM_VALUE_L8	(5 * DMA2D_FGPFCCR_CM_0)	/* 0101: L8 */

#define MDMA_CTCR_xSIZE_U16			0x01	// 2 byte
#define MDMA_CTCR_xSIZE_U8			0x00	// 1 byte
#define MDMA_CTCR_xSIZE_RGB565		0x01	// 2 byte

static void
ltdc_horizontal_pixels(
	volatile PACKEDCOLOR_T * tgr,		// target raster
	const FLASHMEM uint8_t * raster,
	uint_fast16_t width	// number of bits (start from LSB first byte in raster)
	);

#if WITHMDMAHW

static uint_fast8_t
mdma_getbus(uintptr_t addr)
{
#if CPUSTYLE_STM32H7XX
	addr &= 0xFF000000uL;
	return (addr == 0x00000000uL || addr == 0x20000000uL);
#elif CPUSTYLE_STM32MP1 && CORE_CA7
	// SYSMEM
	// DDRCTRL
	return 0;
#elif CPUSTYLE_STM32MP1 && ! CORE_CA7
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

// DBURST value must be programmed as to ensure that the burst size is lower than the
// Transfer Length. If this is not ensured, the result is unpredictable.

// И еще несколько условий..

static uint_fast8_t
mdma_getburst(uint_fast16_t tlen, uint_fast8_t bus, uint_fast8_t xinc)
{
	if (bus == 0 && xinc == 0)
		return 0;
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
#endif /* WITHMDMAHW */

// получение адреса в видобуфере
volatile uint8_t * hwacc_getbufaddr_u8(
	volatile uint8_t * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x,	// начальная координата
	uint_fast16_t y		// начальная координата
	)
{
	return & buffer [y * dx + x];
}

// получение адреса в видобуфере
volatile uint16_t * hwacc_getbufaddr_u16(
	volatile uint16_t * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x,	// начальная координата
	uint_fast16_t y		// начальная координата
	)
{
	return & buffer [y * dx + x];
}

void
hwacc_fillrect_u8(
	volatile uint8_t * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t col,	// начальная координата
	uint_fast16_t row,	// начальная координата
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	uint_fast8_t color	// цвет
	)
{
	if (w == 0 || h == 0)
		return;
	enum { PIXEL_SIZE = sizeof * buffer };
	enum { PIXEL_SIZE_CODE = 0 };
	ALIGNX_BEGIN volatile uint8_t tgcolor ALIGNX_END;	/* значение цвета для заполнения области памяти */

#if WITHMDMAHW
	// MDMA implementation

	tgcolor = color;

	arm_hardware_flush((uintptr_t) & tgcolor, sizeof tgcolor);
	arm_hardware_flush((uintptr_t) buffer, PIXEL_SIZE * GXSIZE(dx, dy));

	MDMA_CH->CCR &= ~ MDMA_CCR_EN_Msk;
	//while ((MDMA_CH->CCR & MDMA_CCR_EN_Msk) != 0)
	//	;
	MDMA_CH->CDAR = (uintptr_t) & buffer [row * dx + col];
	MDMA_CH->CSAR = (uintptr_t) & tgcolor;
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
		(0x00 << MDMA_CTCR_PKE_Pos) |
		(0x00 << MDMA_CTCR_PAM_Pos) |
		(0x02 << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01 << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01 << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((PIXEL_SIZE * (w)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00 << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00 << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((h - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((PIXEL_SIZE * (0)) << MDMA_CBRUR_SUV_Pos) |				// Source address Update Value
		((PIXEL_SIZE * (dx - w)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(sbus << MDMA_CTBR_SBUS_Pos) |
		(dbus << MDMA_CTBR_DBUS_Pos) |
		0;

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

#else
	// DMA2D не умеет оптимизировать заполнение восьмибитным цыетом
	// программная реализация

	const unsigned t = dx - w;
	buffer += (dx * row) + col;
	while (h --)
	{
		volatile uint8_t * startmem = buffer;

		unsigned n = w;
		while (n --)
			* buffer ++ = color;
		buffer += t;
		arm_hardware_flush((uintptr_t) startmem, PIXEL_SIZE * w);
	}

#endif
}

void
hwacc_fillrect_u16(
	volatile uint16_t * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t col,	// начальная координата
	uint_fast16_t row,	// начальная координата
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	uint_fast16_t color	// цвет
	)
{
	if (w == 0 || h == 0)
		return;
	enum { PIXEL_SIZE = sizeof * buffer };
	enum { PIXEL_SIZE_CODE = 1 };
	ALIGNX_BEGIN volatile uint16_t tgcolor ALIGNX_END;	/* значение цвета для заполнения области памяти */


#if WITHMDMAHW
	// MDMA implementation

	tgcolor = color;

	arm_hardware_flush((uintptr_t) & tgcolor, sizeof tgcolor);
	arm_hardware_flush((uintptr_t) buffer, PIXEL_SIZE * GXSIZE(dx, dy));

	MDMA_CH->CCR &= ~ MDMA_CCR_EN_Msk;
	//while ((MDMA_CH->CCR & MDMA_CCR_EN_Msk) != 0)
	//	;
	MDMA_CH->CDAR = (uintptr_t) & buffer [row * dx + col];
	MDMA_CH->CSAR = (uintptr_t) & tgcolor;
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
		(0x00 << MDMA_CTCR_PKE_Pos) |
		(0x00 << MDMA_CTCR_PAM_Pos) |
		(0x02 << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01 << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01 << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((PIXEL_SIZE * (w)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00 << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00 << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((h - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((PIXEL_SIZE * (0)) << MDMA_CBRUR_SUV_Pos) |				// Source address Update Value
		((PIXEL_SIZE * (dx - w)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(sbus << MDMA_CTBR_SBUS_Pos) |
		(dbus << MDMA_CTBR_DBUS_Pos) |
		0;

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
	// DMA2D implementation

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

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		3 * DMA2D_CR_MODE_0 |	// 11: Register-to-memory (no FG nor BG, only output stage active)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;


#else
	// программная реализация

	const unsigned t = dx - w;
	buffer += (dx * row) + col;
	while (h --)
	{
		volatile uint16_t * const startmem = buffer;

		unsigned n = w;
		while (n --)
			* buffer ++ = color;
		buffer += t;
		arm_hardware_flush((uintptr_t) startmem, PIXEL_SIZE * w);
	}

#endif
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

	/* set AXI master timer */
	DMA2D->AMTCR = (DMA2D->AMTCR & ~ (DMA2D_AMTCR_DT_Msk | DMA2D_AMTCR_EN_Msk)) |
		(DMA2D_AMTCR_DT_VALUE << DMA2D_AMTCR_DT_Pos) |
		(DMA2D_AMTCR_DT_ENABLE << DMA2D_AMTCR_EN_Pos) |
		0;
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

	/* SYSCFG clock enable */
	RCC->MP_APB3ENSETR = RCC_MC_APB3ENSETR_SYSCFGEN;
	(void) RCC->MP_APB3ENSETR;
	/*
	 * Interconnect update : select master using the port 1.
	 * LTDC = AXI_M9.
	 * MDMA = AXI_M7.
	 */
	//SYSCFG->ICNR |= SYSCFG_ICNR_AXI_M7;
	//(void) SYSCFG->ICNR;

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


static uint_fast8_t scalecolor(
	uint_fast8_t cv,	// color component value
	uint_fast8_t maxv,	// maximal color component value
	uint_fast8_t rmaxv	// resulting color component value
	)
{
	return (cv * rmaxv) / maxv;
}

// FIXME: доелать модификацию цвета для LCDMODE_LTDC_L8
static COLORPIP_T getshadedcolor(
	COLORPIP_T dot, // исходный цвет
	uint_fast8_t alpha	// на сколько затемнять цвета (0 - чёрный, 255 - без изменений)
	)
{
#if LCDMODE_LTDC_PIPL8
	return dot ^ 0x80;	// FIXME: use indexed color

#elif LCDMODE_LTDC_PIP16
	if (dot == COLORPIP_BLACK)
	{
		return TFTRGB565(alpha, alpha, alpha); // back gray
	}
	else
	{
		// RRRR.RGGG.GGGB.BBBB
		const uint_fast8_t r = scalecolor(((dot >> 11) & 0x001f) * 1, 31, alpha);
		const uint_fast8_t g = scalecolor(((dot >> 5) & 0x003f) * 1, 63, alpha);
		const uint_fast8_t b = scalecolor((dot & 0x001f) * 1, 31, alpha);

		return (r << 11) | (g << 5) | b;
		//return TFTRGB565(r * 8, g * 4, b * 8);	// TODO: test this code
	}

#else /*  */
	//#warning LCDMODE_LTDC_PIPL8 or LCDMODE_LTDC_PIP16 not defined
	return dot;

#endif /* LCDMODE_LTDC_PIPL8 */
}

// Установить прозрачность для прямоугольника
void pip_transparency_rect(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x1, uint_fast16_t y1,
	uint_fast16_t x2, uint_fast16_t y2,
	uint_fast8_t alpha	// на сколько затемнять цвета (0 - чёрный, 255 - без изменений)
	)
{
#if 1
	uint_fast16_t y;

	for (y = y1; y <= y2; y ++)
	{
		const uint_fast16_t yt = dx * y;
		for (uint_fast16_t x = x1; x <= x2; x ++)
		{
			buffer [yt + x] = getshadedcolor(buffer [yt + x], alpha);
		}
	}
#else
	uint_fast16_t y;

	for (y = y1; y <= y2; y ++)
	{
		for (uint_fast16_t x = x1; x <= x2; x ++)
		{
			PACKEDCOLORPIP_T * const p = display_colorbuffer_at(buffer, dx, dy, x, y);
			* p = getshadedcolor(* p, alpha);
		}
	}
#endif
}


// получить адрес требуемой позиции в буфере
PACKEDCOLORPIP_T *
display_colorbuffer_at(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y	// вертикальная координата пикселя (0..dy-1) сверху вниз
	)
{
	ASSERT(x < dx);
	ASSERT(y < dy);
#if LCDMODE_HORFILL
	return & buffer [y * dx + x];
#else /* LCDMODE_HORFILL */
	return & buffer [y * dx + x];
#endif /* LCDMODE_HORFILL */
}


/// Нарисовать вертикальную цветную полосу
// Формат RGB565
void
display_colorbuffer_xor_vline(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row0,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	COLORPIP_T color
	)
{
	while (h --)
		display_colorbuffer_xor(buffer, dx, dy, col, row0 ++, color);
}

// Нарисовать вертикальную цветную полосу
// Формат RGB565
void
display_colorbuffer_set_vline(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t col,	// горизонтальная координата начального пикселя (0..dx-1) слева направо
	uint_fast16_t row0,	// вертикальная координата начального пикселя (0..dy-1) сверху вниз
	uint_fast16_t h,	// высота
	COLORPIP_T color
	)
{
	while (h --)
		display_colorbuffer_set(buffer, dx, dy, col, row0 ++, color);
}

// заполнение прямоугольной области в видеобуфере
void display_colorbuffer_fillrect(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x,	// начальная координата
	uint_fast16_t y,	// начальная координата
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	COLORPIP_T color	// цвет
	)
{
#if LCDMODE_LTDC_PIP16
	hwacc_fillrect_u16(buffer, dx, dy, x, y, w, h, color);
#elif LCDMODE_LTDC_PIPL8
	hwacc_fillrect_u8(buffer, dx, dy, x, y, w, h, color);
#endif
}


// начальная инициализация буфера
// Эта функция используется только в тесте
void display_colorbuffer_fill(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	COLORPIP_T color
	)
{
#if LCDMODE_LTDC_PIP16
	hwacc_fillrect_u16(buffer, dx, dy, 0, 0, dx, dy, color);
#elif LCDMODE_LTDC_PIPL8
	hwacc_fillrect_u8(buffer, dx, dy, 0, 0, dx, dy, color);
#endif
}

// поставить цветную точку.
void display_colorbuffer_set(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	)
{
	* display_colorbuffer_at(buffer, dx, dy, col, row) = color;
}

// поставить цветную точку (модификация с сохранением старого изоьражения).
void display_colorbuffer_xor(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	)
{
	* display_colorbuffer_at(buffer, dx, dy, col, row) ^= color;
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
	const uint_fast32_t tlen = mdma_tlen(w * sizeof (PACKEDCOLOR_T), sizeof (PACKEDCOLOR_T));
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
		(0x00 << MDMA_CTCR_PKE_Pos) |
		(0x00 << MDMA_CTCR_PAM_Pos) |
		(0x02 << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01 << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01 << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((sizeof (PACKEDCOLOR_T) * (w)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00 << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00 << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((h - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((sizeof (PACKEDCOLOR_T) * (0)) << MDMA_CBRUR_SUV_Pos) |		// Source address Update Value
		((sizeof (PACKEDCOLOR_T) * (t)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(sbus << MDMA_CTBR_SBUS_Pos) |
		(dbus << MDMA_CTBR_DBUS_Pos) |
		0;

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
		DMA2D_FGPFCCR_CM_VALUE_MAIN |	/* Color mode - framebuffer pixel format */
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		0 * DMA2D_CR_MODE_0 |	// 00: Memory-to-memory (FG fetch only)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;

#else
	// программная реализация
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
/*
	const size_t len = dx * sizeof * buffer;
	while (dy --)
	{
		volatile PACKEDCOLOR_T * const p = & framebuff [ltdc_first] [ltdc_second];
		memcpy((void *) p, src, len);
		arm_hardware_flush((uintptr_t) p, len);
		src += dx;
		++ ltdc_first;
	}
*/


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
	const uint_fast32_t tlen = mdma_tlen(w * sizeof (PACKEDCOLOR565_T), sizeof (PACKEDCOLOR565_T));
	const uint_fast32_t sbus = mdma_getbus(MDMA_CH->CSAR);
	const uint_fast32_t dbus = mdma_getbus(MDMA_CH->CDAR);
	const uint_fast32_t sinc = 0x02; // Source increment mode: 10: address pointer is incremented
	const uint_fast32_t dinc = 0x02; // Destination increment mode: 10: Destination address pointer is incremented
	const uint_fast32_t sburst = mdma_getburst(tlen, sbus, sinc);
	const uint_fast32_t dburst = mdma_getburst(tlen, dbus, dinc);
	MDMA_CH->CTCR =
		(sinc << MDMA_CTCR_SINC_Pos) | 	// Source increment mode: 10: address pointer is incremented
		(MDMA_CTCR_xSIZE_RGB565 << MDMA_CTCR_SSIZE_Pos) |
		(MDMA_CTCR_xSIZE_RGB565 << MDMA_CTCR_SINCOS_Pos) |
		(sburst << MDMA_CTCR_SBURST_Pos) |
		(dinc << MDMA_CTCR_DINC_Pos) |	// Destination increment mode: 10: Destination address pointer is incremented
		(MDMA_CTCR_xSIZE_RGB565 << MDMA_CTCR_DSIZE_Pos) |
		(MDMA_CTCR_xSIZE_RGB565 << MDMA_CTCR_DINCOS_Pos) |
		(dburst << MDMA_CTCR_DBURST_Pos) |	// Destination burst transfer configuration
		((tlen - 1) << MDMA_CTCR_TLEN_Pos) |		// buffer Transfer Length (number of bytes - 1)
		(0x00 << MDMA_CTCR_PKE_Pos) |
		(0x00 << MDMA_CTCR_PAM_Pos) |
		(0x02 << MDMA_CTCR_TRGM_Pos) |		// Trigger Mode: 10: Each MDMA request (software or hardware) triggers a repeated block transfer (if the block repeat is 0, a single block is transferred)
		(0x01 << MDMA_CTCR_SWRM_Pos) |		// 1: hardware request are ignored. Transfer is triggered by software writing 1 to the SWRQ bit
		(0x01 << MDMA_CTCR_BWM_Pos) |
		0;
	MDMA_CH->CBNDTR =
		((sizeof (PACKEDCOLOR565_T) * (w)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00 << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00 << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((h - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((sizeof (PACKEDCOLOR565_T) * (0)) << MDMA_CBRUR_SUV_Pos) |		// Source address Update Value
		((sizeof (PACKEDCOLOR565_T) * (t)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
		0;

	MDMA_CH->CTBR = (MDMA_CH->CTBR & ~ (MDMA_CTBR_SBUS_Msk | MDMA_CTBR_DBUS_Msk)) |
		(sbus << MDMA_CTBR_SBUS_Pos) |
		(dbus << MDMA_CTBR_DBUS_Pos) |
		0;

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
		DMA2D_FGPFCCR_CM_VALUE_MAIN |	/* Color mode - framebuffer pixel format */
		0;

	/* запустить операцию */
	DMA2D->CR = (DMA2D->CR & ~ (DMA2D_CR_MODE)) |
		0 * DMA2D_CR_MODE_0 |	// 00: Memory-to-memory (FG fetch only)
		1 * DMA2D_CR_START |
		0;

	/* ожидаем выполнения операции */
	while ((DMA2D->CR & DMA2D_CR_START) != 0)
		;
#else
	// программная реализация
	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
/*
	const size_t len = dx * sizeof * buffer;
	while (dy --)
	{
		volatile PACKEDCOLOR_T * const p = & framebuff [ltdc_first] [ltdc_second];
		memcpy((void *) p, src, len);
		arm_hardware_flush((uintptr_t) p, len);
		src += dx;
		++ ltdc_first;
	}
*/

#endif
}

#if LCDMODE_LTDC_PIPL8

// И PIP и основной экран в формате L8

// установить данный буфер как область для PIP
void display_colorbuffer_pip(
	const PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy
	)
{
	arm_hardware_flush((uintptr_t) buffer, (uint_fast32_t) dx * dy * sizeof * buffer);
	arm_hardware_ltdc_pip_set((uintptr_t) buffer);
}

#elif LCDMODE_LTDC_PIP16

// Выдать буфер на дисплей
void display_colorbuffer_show(
	const PACKEDCOLORPIP_T * buffer,
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
	const PACKEDCOLORPIP_T * buffer,
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
	const PACKEDCOLORPIP_T * buffer,
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
			DMA2D_FGPFCCR_CM_VALUE_MAIN |	/* Color mode - framebuffer pixel format */
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
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	
	uint_fast16_t dy,
	uint_fast16_t x0,	
	uint_fast16_t y0,
	uint_fast16_t x1,	
	uint_fast16_t y1,
	COLORPIP_T color
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

   int TwoDy = 2 * Dy;
   int TwoDyTwoDx = TwoDy - 2 * Dx; // 2*Dy - 2*Dx
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

// Нарисовать закрашенный или пустой прямоугольник
void display_colorbuffer_rect(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// размер буфера
	uint_fast16_t dy,	// размер буфера
	uint_fast16_t x1,	// начальная координата
	uint_fast16_t y1,	// начальная координата
	uint_fast16_t x2,	// конечная координата (включена в заполняемую облсть)
	uint_fast16_t y2,	// конечная координата (включена в заполняемую облсть)
	PACKEDCOLOR565_T color,
	uint_fast8_t fill
	)
{
	ASSERT(x2 > x1);
	ASSERT(y2 > y1);

	const uint_fast16_t w = x2 - x1 + 1;	// размер по горизонтали
	const uint_fast16_t h = y2 - y1 + 1;	// размер по вертикали

	if (w < 3 || h < 3)
		return;

	if (fill != 0)
	{
		display_colorbuffer_fillrect(buffer, dx, dy, x1, y1, w, h, color);
	}
	else
	{
		display_colorbuffer_fillrect(buffer, dx, dy, x1, y1, w, 1, color);	// верхняя горизонталь
		display_colorbuffer_fillrect(buffer, dx, dy, x1, y2, w, 1, color);	// нижняя горизонталь
		display_colorbuffer_fillrect(buffer, dx, dy, x1, y1 + 1, 1, h - 2, color);	// левая вертикаль
		display_colorbuffer_fillrect(buffer, dx, dy, x2, y1 + 1, 1, h - 2, color);	// правая вертикаль
	}
}

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
display_fillrect_pattern(
	uint_fast16_t x, uint_fast16_t y, 	// координаты в пикселях
	uint_fast16_t w, uint_fast16_t h, 	// размеры в пикселях
	COLOR_T fgcolor,
	COLOR_T bgcolor,
	uint_fast8_t hpattern	// horizontal pattern (LSB - left)
	)
{
	// TODO: bgcolor и hpattern пока игнорируются
#if LCDMODE_LTDC_L8 && LCDMODE_LTDC
	hwacc_fillrect_u8(& framebuff [0] [0], DIM_X, DIM_Y, x, y, w, h, fgcolor);
#elif LCDMODE_LTDC
	hwacc_fillrect_u16(& framebuff [0] [0], DIM_X, DIM_Y, x, y, w, h, fgcolor);
#endif
}

/* заполнение прямоугольника на основном экране произвольным цветом
*/
void
display_fillrect(
	uint_fast16_t x, uint_fast16_t y, 	// координаты в пикселях
	uint_fast16_t w, uint_fast16_t h, 	// размеры в пикселях
	COLOR_T color
	)
{
	display_fillrect_pattern(x, y, w, h, color, color, 0xFF);
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

#include "fontmaps.h"

#if ! LCDMODE_LTDC_L24
#include "./byte2crun.h"
#endif /* ! LCDMODE_LTDC_L24 */


/* позиция в растре, куда будет выдаваться следующий пиксель */
static PACKEDCOLOR_T ltdc_fg, ltdc_bg;

#if ! LCDMODE_LTDC_L24
static const FLASHMEM PACKEDCOLOR_T (* byte2runmain) [256][8] = & byte2runmain_COLORMAIN_WHITE_COLORMAIN_BLACK;
static const FLASHMEM PACKEDCOLORPIP_T (* byte2runpip) [256][8] = & byte2runpip_COLORPIP_WHITE_COLORPIP_BLACK;
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

	COLORMAIN_SELECTOR(byte2runmain);

#endif /* ! LCDMODE_LTDC_L24 */

	COLORPIP_SELECTOR(byte2runpip);

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
	const FLASHMEM PACKEDCOLOR_T * const pcl = (* byte2runmain) [v];
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

// функции работы с colorbuffer не занимаются выталкиванеим кэш-памяти
static void RAMFUNC ltdcpip_horizontal_pixels(
	volatile PACKEDCOLORPIP_T * tgr,		// target raster
	const FLASHMEM uint8_t * raster,
	uint_fast16_t width	// number of bits (start from LSB first byte in raster)
	)
{
	uint_fast16_t col;
	uint_fast16_t w = width;

	for (col = 0; w >= 8; col += 8, w -= 8)
	{
		const FLASHMEM PACKEDCOLORPIP_T * const pcl = (* byte2runpip) [* raster ++];
		memcpy((void *) (tgr + col), pcl, sizeof (* tgr) * 8);
	}
	if (w != 0)
	{
		const FLASHMEM PACKEDCOLORPIP_T * const pcl = (* byte2runpip) [* raster];
		memcpy((void *) (tgr + col), pcl, sizeof (* tgr) * w);
	}
	// функции работы с colorbuffer не занимаются выталкиванеим кэш-памяти
	//arm_hardware_flush((uintptr_t) tgr, sizeof (* tgr) * width);
}

// функции работы с colorbuffer не занимаются выталкиванеим кэш-памяти
// Фон не трогаем
static void RAMFUNC ltdcpip_horizontal_pixels_tbg(
	volatile PACKEDCOLORPIP_T * tgr,		// target raster
	const FLASHMEM uint8_t * raster,
	uint_fast16_t width,	// number of bits (start from LSB first byte in raster)
	COLORPIP_T fg
	)
{
	uint_fast16_t w = width;

	for (; w >= 8; w -= 8, tgr += 8)
	{
		const uint_fast8_t v = * raster ++;
		if (v & 0x01)	tgr [0] = fg;
		if (v & 0x02)	tgr [1] = fg;
		if (v & 0x04)	tgr [2] = fg;
		if (v & 0x08)	tgr [3] = fg;
		if (v & 0x10)	tgr [4] = fg;
		if (v & 0x20)	tgr [5] = fg;
		if (v & 0x40)	tgr [6] = fg;
		if (v & 0x80)	tgr [7] = fg;
	}
	if (w != 0)
	{
		uint_fast8_t vlast = * raster;
		do
		{
			if (vlast & 0x01)
				* tgr = fg;
			++ tgr;
			vlast >>= 1;
		} while (-- w);
	}
}



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
		const FLASHMEM PACKEDCOLOR_T * const pcl = (* byte2runmain) [* raster ++];
		memcpy((void *) (tgr + col), pcl, sizeof (* tgr) * 8);
	}
	if (w != 0)
	{
		const FLASHMEM PACKEDCOLOR_T * const pcl = (* byte2runmain) [* raster ++];
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

// возвращаем на сколько пикселей вправо занимет отрисованный символ
static uint_fast16_t RAMFUNC_NONILINE ltdcpip_horizontal_put_char_small(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,
	uint_fast16_t y,
	char cc
	)
{
	const uint_fast8_t width = SMALLCHARW;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH; ++ cgrow)
	{
		volatile PACKEDCOLORPIP_T * const tgr = & buffer [(y + cgrow) * dx + x];
		ltdcpip_horizontal_pixels(tgr, S1D13781_smallfont_LTDC [c] [cgrow], width);
	}
	return width;
}

// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
static uint_fast16_t RAMFUNC_NONILINE ltdcpip_horizontal_put_char_small_tbg(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,
	uint_fast16_t y,
	char cc,
	COLOR565_T fg
	)
{
	const uint_fast8_t width = SMALLCHARW;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH; ++ cgrow)
	{
		volatile PACKEDCOLORPIP_T * const tgr = & buffer [(y + cgrow) * dx + x];
		ltdcpip_horizontal_pixels_tbg(tgr, S1D13781_smallfont_LTDC [c] [cgrow], width, fg);
	}
	return width;
}

// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
static uint_fast16_t RAMFUNC_NONILINE ltdcpip_horizontal_put_char_small2_tbg(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,
	uint_fast16_t y,
	char cc,
	COLORPIP_T fg
	)
{
	const uint_fast8_t width = SMALLCHARW2;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH2; ++ cgrow)
	{
		volatile PACKEDCOLORPIP_T * const tgr = & buffer [(y + cgrow) * dx + x];
		ltdcpip_horizontal_pixels_tbg(tgr, S1D13781_smallfont2_LTDC [c] [cgrow], width, fg);
	}
	return width;
}

// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
static uint_fast16_t RAMFUNC_NONILINE ltdcpip_horizontal_put_char_small3_tbg(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,
	uint_fast16_t y,
	char cc,
	COLORPIP_T fg
	)
{
	const uint_fast8_t width = SMALLCHARW3;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH3; ++ cgrow)
	{
		volatile PACKEDCOLORPIP_T * const tgr = & buffer [(y + cgrow) * dx + x];
		ltdcpip_horizontal_pixels_tbg(tgr, & S1D13781_smallfont3_LTDC [c] [cgrow], width, fg);
	}
	return width;
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

// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
display_colorbuff_string_tbg(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	)
{
	char c;

	while((c = * s ++) != '\0')
	{
		x += ltdcpip_horizontal_put_char_small_tbg(buffer, dx, dy, x, y, c, fg);
	}
}

// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
display_colorbuff_string2_tbg(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	)
{
	char c;

	while((c = * s ++) != '\0')
	{
		x += ltdcpip_horizontal_put_char_small2_tbg(buffer, dx, dy, x, y, c, fg);
	}
}

// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
display_colorbuff_string3_tbg(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * s,
	COLORPIP_T fg		// цвет вывода текста
	)
{
	char c;

	while((c = * s ++) != '\0')
	{
		x += ltdcpip_horizontal_put_char_small3_tbg(buffer, dx, dy, x, y, c, fg);
	}
}

// Возвращает ширину строки в пикселях
uint_fast16_t display_colorbuff_string3_width(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	const char * s
	)
{
	(void) buffer;
	(void) dx;
	(void) dy;
	return SMALLCHARW3 * strlen(s);
}

// Возвращает ширину строки в пикселях
uint_fast16_t display_colorbuff_string2_width(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	const char * s
	)
{
	(void) buffer;
	(void) dx;
	(void) dy;
	return SMALLCHARW2 * strlen(s);
}

// Возвращает ширину строки в пикселях
uint_fast16_t display_colorbuff_string_width(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	const char * s
	)
{
	(void) buffer;
	(void) dx;
	(void) dy;
	return SMALLCHARW * strlen(s);
}

// Возвращает высоту строки в пикселях
uint_fast16_t display_colorbuff_string_height(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	const char * s
	)
{
	(void) buffer;
	(void) dx;
	(void) dy;
	(void) s;
	return SMALLCHARH;
}


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
		DMA2D_FGPFCCR_CM_VALUE_MAIN |	/* Color mode - framebuffer pixel format */
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
		DMA2D_FGPFCCR_CM_VALUE_MAIN |	/* Color mode - framebuffer pixel format */
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
	display_fillrect(0, 0, DIM_X, DIM_Y, bg);
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

	display_fillrect_pattern(x, y, wpart, h, ltdc_fg, ltdc_bg, hpattern);
	display_fillrect_pattern(x + wpart, y, wfull - wpart, h, ltdc_bg, ltdc_bg, 0x00);
	if (wmark < wfull && wmark >= wpart)
		display_fillrect_pattern(x + wmark, y, 1, h, ltdc_fg, ltdc_bg, 0xFF);
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

#if WITHTOUCHGUI
void display_at_xy(uint_fast16_t x, uint_fast16_t y, const char * s)
{
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	do
	{
		ltdc_second = x;
		ltdc_first = y + lowhalf;
		display_string(s, lowhalf);

	} while (lowhalf --);
}
#endif /* WITHTOUCHGUI */

#endif /* LCDMODE_LQ043T3DX02K */
#endif /* LCDMODE_LTDC */
