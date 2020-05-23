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
#include "gui.h"
#include "formats.h"
#include "spi.h"	// hardware_spi_master_send_frame
#include <string.h>


#include "fontmaps.h"

/*
	Dead time value in the AXI clock cycle inserted between two consecutive accesses on
	the AXI master port. These bits represent the minimum guaranteed number of cycles
	between two consecutive AXI accesses
 */
#define DMA2D_AMTCR_DT_VALUE 255	/* 0..255 */
#define DMA2D_AMTCR_DT_ENABLE 1	/* 0..1 */

#define MDMA_CH	MDMA_Channel0
#define MDMA_CCR_PL_VALUE 0	// PL: priority 0..3: min..max

#if LCDMODE_LTDC_L24
	#define DMA2D_FGPFCCR_CM_VALUE_MAIN	(1 * DMA2D_FGPFCCR_CM_0)	/* 0001: RGB888 */
	//#define DMA2D_OPFCCR_CM_VALUE_MAIN	(1 * DMA2D_OPFCCR_CM_0)	/* 001: RGB888 */

#elif LCDMODE_MAIN_L8
	#define DMA2D_FGPFCCR_CM_VALUE_MAIN	(5 * DMA2D_FGPFCCR_CM_0)	/* 0101: L8 */
	#define MDMA_CTCR_xSIZE_MAIN			0x00	// 1 byte
	////#define DMA2D_OPFCCR_CM_VALUE	(x * DMA2D_OPFCCR_CM_0)	/* not supported */

#else /* LCDMODE_MAIN_L8 */
	#define DMA2D_FGPFCCR_CM_VALUE_MAIN	(2 * DMA2D_FGPFCCR_CM_0)	/* 0010: RGB565 */
	//#define DMA2D_OPFCCR_CM_VALUE_MAIN	(2 * DMA2D_OPFCCR_CM_0)	/* 010: RGB565 */
	#define MDMA_CTCR_xSIZE_MAIN			0x01	// 2 byte

#endif /* LCDMODE_MAIN_L8 */

#if LCDMODE_PIP_L8
	#define DMA2D_FGPFCCR_CM_VALUE_PIP	(5 * DMA2D_FGPFCCR_CM_0)	/* 0101: L8 */
	#define MDMA_CTCR_xSIZE_PIP			0x00	// 1 byte
	////#define DMA2D_OPFCCR_CM_VALUE_MAIN	(x * DMA2D_OPFCCR_CM_0)	/* not supported */

#elif LCDMODE_PIP_RGB565
	#define DMA2D_FGPFCCR_CM_VALUE_PIP	(2 * DMA2D_FGPFCCR_CM_0)	/* 0010: RGB565 */
	//#define DMA2D_OPFCCR_CM_VALUE_PIP	(2 * DMA2D_OPFCCR_CM_0)	/* 010: RGB565 */
	#define MDMA_CTCR_xSIZE_PIP			0x01	// 2 byte

#else /* LCDMODE_MAIN_L8 */
	#define DMA2D_FGPFCCR_CM_VALUE_PIP	DMA2D_FGPFCCR_CM_VALUE_MAIN
	//#define DMA2D_OPFCCR_CM_VALUE_PIP	DMA2D_OPFCCR_CM_VALUE_MAIN
	#define MDMA_CTCR_xSIZE_PIP			MDMA_CTCR_xSIZE_MAIN

#endif /* LCDMODE_MAIN_L8 */

#define DMA2D_FGPFCCR_CM_VALUE_L24	(1 * DMA2D_FGPFCCR_CM_0)	/* 0001: RGB888 */
#define DMA2D_FGPFCCR_CM_VALUE_L16	(2 * DMA2D_FGPFCCR_CM_0)	/* 0010: RGB565 */
#define DMA2D_FGPFCCR_CM_VALUE_L8	(5 * DMA2D_FGPFCCR_CM_0)	/* 0101: L8 */

#define MDMA_CTCR_xSIZE_U16			0x01	// 2 byte
#define MDMA_CTCR_xSIZE_U8			0x00	// 1 byte
#define MDMA_CTCR_xSIZE_RGB565		0x01	// 2 byte

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
	return & buffer [y * GXADJ(dx) + x];
}

// Функция получает координаты и работает над буфером в горищонталтной ориентации.
void
hwacc_fillrect_u8(
	uint8_t * buffer,
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
	arm_hardware_flush_invalidate((uintptr_t) buffer, PIXEL_SIZE * GXSIZE(dx, dy));

	MDMA_CH->CCR &= ~ MDMA_CCR_EN_Msk;
	//while ((MDMA_CH->CCR & MDMA_CCR_EN_Msk) != 0)
	//	;
	MDMA_CH->CDAR = (uintptr_t) & buffer [row * GXADJ(dx) + col];
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
		((PIXEL_SIZE * (GXADJ(dx) - w)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
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
		hardware_nonguiyield();

#else
	// программная реализация

	const unsigned t = GXADJ(dx) - w;
	buffer += (GXADJ(dx) * row) + col;
	while (h --)
	{
		uint8_t * const startmem = buffer;

		unsigned n = w;
		while (n --)
			* buffer ++ = color;
		buffer += t;
	}

#endif
}

// Функция получает координаты и работает над буфером в горищонталтной ориентации.
void
hwacc_fillrect_u16(
	uint16_t * buffer,
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
	arm_hardware_flush_invalidate((uintptr_t) buffer, PIXEL_SIZE * GXSIZE(dx, dy));

	MDMA_CH->CCR &= ~ MDMA_CCR_EN_Msk;
	//while ((MDMA_CH->CCR & MDMA_CCR_EN_Msk) != 0)
	//	;
	MDMA_CH->CDAR = (uintptr_t) & buffer [row * GXADJ(dx) + col];
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
		((PIXEL_SIZE * (GXADJ(dx) - w)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
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
		hardware_nonguiyield();

#elif WITHDMA2DHW
	// DMA2D implementation

	// just writes the color defined in the DMA2D_OCOLR register
	// to the area located at the address pointed by the DMA2D_OMAR
	// and defined in the DMA2D_NLR and DMA2D_OOR.

	arm_hardware_flush_invalidate((uintptr_t) buffer, sizeof (* buffer) * GXSIZE(dx, dy));

	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) & buffer [row * GXADJ(dx) + col];
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
		hardware_nonguiyield();


#else
	// программная реализация

	const unsigned t = GXADJ(dx) - w;
	buffer += (GXADJ(dx) * row) + col;
	while (h --)
	{
		uint16_t * const startmem = buffer;

		unsigned n = w;
		while (n --)
			* buffer ++ = color;
		buffer += t;
	}

#endif
}

// Функция получает координаты и работает над буфером в горищонталтной ориентации.
void
hwacc_fillrect_u24(
	PACKEDCOLORMAIN_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t col,	// начальная координата
	uint_fast16_t row,	// начальная координата
	uint_fast16_t w,	// ширниа
	uint_fast16_t h,	// высота
	uint_fast32_t color	// цвет
	)
{
	if (w == 0 || h == 0)
		return;
	enum { PIXEL_SIZE = sizeof * buffer };
	//enum { PIXEL_SIZE_CODE = 1 };
	//ALIGNX_BEGIN volatile uint16_t tgcolor ALIGNX_END;	/* значение цвета для заполнения области памяти */

	ASSERT(sizeof (* buffer) == 3);

#if 0//WITHMDMAHW
	// MDMA implementation

	#error MDMA implementation need
	tgcolor = color;

	arm_hardware_flush((uintptr_t) & tgcolor, sizeof tgcolor);
	arm_hardware_flush_invalidate((uintptr_t) buffer, PIXEL_SIZE * GXSIZE(dx, dy));

	MDMA_CH->CCR &= ~ MDMA_CCR_EN_Msk;
	//while ((MDMA_CH->CCR & MDMA_CCR_EN_Msk) != 0)
	//	;
	MDMA_CH->CDAR = (uintptr_t) & buffer [row * GXADJ(dx) + col];
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
		((PIXEL_SIZE * (GXADJ(dx) - w)) << MDMA_CBRUR_DUV_Pos) |		// Destination address Update Value
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
		hardware_nonguiyield();

#elif WITHDMA2DHW
	// DMA2D implementation

	// just writes the color defined in the DMA2D_OCOLR register
	// to the area located at the address pointed by the DMA2D_OMAR
	// and defined in the DMA2D_NLR and DMA2D_OOR.

	arm_hardware_flush_invalidate((uintptr_t) buffer, sizeof (* buffer) * GXSIZE(dx, dy));

	/* целевой растр */
	DMA2D->OMAR = (uintptr_t) & buffer [row * GXADJ(dx) + col];
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
		hardware_nonguiyield();


#else
	// программная реализация

	const unsigned t = GXADJ(dx) - w;
	buffer += (GXADJ(dx) * row) + col;
	while (h --)
	{
		PACKEDCOLORMAIN_T * const startmem = buffer;

		unsigned n = w;
		while (n --)
			* buffer ++ = color;
		buffer += t;
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

extern const char * savestring;

// получить адрес требуемой позиции в буфере
PACKEDCOLORMAIN_T *
colmain_mem_at_debug(
	PACKEDCOLORMAIN_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t y,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	const char * file,
	int line
	)
{
	if (x >= dx || y >= dy)
	{
		PRINTF("colmain_mem_at(%s/%d): dx=%u, dy=%u, x=%u, y=%u, savestring='%s'\n", file, line, dx, dy, x, y, savestring);
	}
	ASSERT(x < dx);
	ASSERT(y < dy);
#if LCDMODE_HORFILL
	return & buffer [y * GXADJ(dx) + x];
#else /* LCDMODE_HORFILL */
	return & buffer [y * GXADJ(dx) + x];
#endif /* LCDMODE_HORFILL */
}


/// Нарисовать вертикальную цветную полосу
// Формат RGB565
void
display_colorbuf_xor_vline(
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
		colpip_point_xor(buffer, dx, dy, col, row0 ++, color);
}

// Нарисовать вертикальную цветную полосу
// Формат RGB565
void
display_colorbuf_set_vline(
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
		colpip_point(buffer, dx, dy, col, row0 ++, color);
}

// заполнение прямоугольной области в видеобуфере
void colpip_fillrect(
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
	ASSERT(x < dx);
	ASSERT((x + w) <= dx);
	ASSERT(y < dy);
	ASSERT((y + h) <= dy);
#if LCDMODE_HORFILL

	#if LCDMODE_PIP_L8
		hwacc_fillrect_u8(buffer, dx, dy, x, y, w, h, color);

	#elif LCDMODE_PIP_RGB565
		hwacc_fillrect_u16(buffer, dx, dy, x, y, w, h, color);

	#elif LCDMODE_PIP_L24
		hwacc_fillrect_u24(buffer, dx, dy, x, y, w, h, color);
	#elif LCDMODE_MAIN_L8
		hwacc_fillrect_u8(buffer, dx, dy, x, y, w, h, color);

	#elif LCDMODE_MAIN_RGB565
		hwacc_fillrect_u16(buffer, dx, dy, x, y, w, h, color);

	#elif LCDMODE_MAIN_L24
		hwacc_fillrect_u24(buffer, dx, dy, x, y, w, h, color);

	#endif

#else /* LCDMODE_HORFILL */

	#if LCDMODE_PIP_L8
		hwacc_fillrect_u8(buffer, dy, dx, y, x, h, w, color);

	#elif LCDMODE_PIP_RGB565
		hwacc_fillrect_u16(buffer, dy, dx, y, x, h, w, color);

	#elif LCDMODE_PIP_L24
		hwacc_fillrect_u24((buffer, dy, dx, y, x, h, w, color);

	#elif LCDMODE_MAIN_L8
		hwacc_fillrect_u8(buffer, dy, dx, y, x, h, w, color);

	#elif LCDMODE_MAIN_RGB565
		hwacc_fillrect_u16(buffer, dy, dx, y, x, h, w, color);

	#elif LCDMODE_MAIN_L24
		hwacc_fillrect_u24((buffer, dy, dx, y, x, h, w, color);

	#endif

#endif /* LCDMODE_HORFILL */
}


/* заливка замкнутого контура */
void display_floodfill(
	PACKEDCOLORMAIN_T * buffer,
	uint_fast16_t dx,	// ширина буфера
	uint_fast16_t dy,	// высота буфера
	uint_fast16_t x,	// начальная координата
	uint_fast16_t y,	// начальная координата
	COLORMAIN_T newColor,
	COLORMAIN_T oldColor
	)
{
	ASSERT(x < dx);
	ASSERT(y < dy);
	PACKEDCOLORMAIN_T * const tgr = colmain_mem_at(buffer, dx, dy, x, y);
	if (* tgr == oldColor && * tgr != newColor)
	{
		* tgr = newColor;
		display_floodfill(buffer, dx, dy, x + 1, y, newColor, oldColor);
		display_floodfill(buffer, dx, dy, x - 1, y, newColor, oldColor);
		display_floodfill(buffer, dx, dy, x, y + 1, newColor, oldColor);
		display_floodfill(buffer, dx, dy, x, y - 1, newColor, oldColor);
	}
}

// начальная инициализация буфера
// Эта функция используется только в тесте
void colpip_fill(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	COLORPIP_T color
	)
{
#if LCDMODE_HORFILL

	#if LCDMODE_PIP_L8
		hwacc_fillrect_u8(buffer, dx, dy, 0, 0, dx, dy, color);

	#elif LCDMODE_PIP_RGB565
		hwacc_fillrect_u16(buffer, dx, dy, 0, 0, dx, dy, color);

	#elif LCDMODE_PIP_L24
		hwacc_fillrect_u24(buffer, dx, dy, 0, 0, dx, dy, color);

	#elif LCDMODE_MAIN_L8
		hwacc_fillrect_u8(buffer, dx, dy, 0, 0, dx, dy, color);

	#elif LCDMODE_MAIN_RGB565
		hwacc_fillrect_u16(buffer, dx, dy, 0, 0, dx, dy, color);

	#elif LCDMODE_MAIN_L24
		hwacc_fillrect_u24(buffer, dx, dy, 0, 0, dx, dy, color);

	#endif

#else /* LCDMODE_HORFILL */

	#if LCDMODE_PIP_L8
		hwacc_fillrect_u8(buffer, dy, dx, 0, 0, dy, dx, color);

	#elif LCDMODE_PIP_RGB565
		hwacc_fillrect_u16(buffer, dy, dx, 0, 0, dy, dx, color);

	#elif LCDMODE_PIP_L24
		hwacc_fillrect_u24(buffer, dy, dx, 0, 0, dy, dx, color);

	#elif LCDMODE_MAIN_L8
		hwacc_fillrect_u8(buffer, dy, dx, 0, 0, dy, dx, color);

	#elif LCDMODE_MAIN_RGB565
		hwacc_fillrect_u16(buffer, dy, dx, 0, 0, dy, dx, color);

	#elif LCDMODE_MAIN_L24
		hwacc_fillrect_u24(buffer, dy, dx, 0, 0, dy, dx, color);

	#endif

#endif /* LCDMODE_HORFILL */
}

// поставить цветную точку.
void colpip_point(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	)
{
	* colmain_mem_at(buffer, dx, dy, col, row) = color;
}

// поставить цветную точку (модификация с сохранением старого изоьражения).
void colpip_point_xor(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t col,	// горизонтальная координата пикселя (0..dx-1) слева направо
	uint_fast16_t row,	// вертикальная координата пикселя (0..dy-1) сверху вниз
	COLORPIP_T color
	)
{
	* colmain_mem_at(buffer, dx, dy, col, row) ^= color;
}


// копирование в большее или равное окно
// размер пикселя - определяется конфигурацией.
// MDMA, DMA2D или программа
// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
void hwaccel_copy(
	uintptr_t dstinvalidateaddr,
	size_t dstinvalidatesize,
	PACKEDCOLORMAIN_T * dst,
	const PACKEDCOLORMAIN_T * src,
	unsigned w,
	unsigned t,	// разница в размере строки получателя от источника
	unsigned h
	)
{
	if (w == 0 || h == 0)
		return;

#if WITHMDMAHW
	// MDMA реализация

	arm_hardware_flush_invalidate(dstinvalidateaddr, dstinvalidatesize);
	arm_hardware_flush((uintptr_t) src, sizeof (* src) * GXSIZE(w, h));

	MDMA_CH->CCR &= ~ MDMA_CCR_EN_Msk;
	MDMA_CH->CDAR = (uintptr_t) dst;
	MDMA_CH->CSAR = (uintptr_t) src;
	const uint_fast32_t tlen = mdma_tlen(w * sizeof (PACKEDCOLORMAIN_T), sizeof (PACKEDCOLORMAIN_T));
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
		((sizeof (PACKEDCOLORMAIN_T) * (w)) << MDMA_CBNDTR_BNDT_Pos) |	// Block Number of data bytes to transfer
		(0x00 << MDMA_CBNDTR_BRSUM_Pos) |	// Block Repeat Source address Update Mode: 0 - increment
		(0x00 << MDMA_CBNDTR_BRDUM_Pos) |	// Block Repeat Destination address Update Mode: 0 - increment
		((h - 1) << MDMA_CBNDTR_BRC_Pos) |		// Block Repeat Count
		0;
	MDMA_CH->CBRUR =
		((sizeof (PACKEDCOLORMAIN_T) * (GXADJ(w) - w)) << MDMA_CBRUR_SUV_Pos) |	// Source address Update Value
		((sizeof (PACKEDCOLORMAIN_T) * (t)) << MDMA_CBRUR_DUV_Pos) |			// Destination address Update Value
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
		hardware_nonguiyield();
	//TP();

#elif WITHDMA2DHW
	// DMA2D реализация

	arm_hardware_flush_invalidate(dstinvalidateaddr, dstinvalidatesize);
	arm_hardware_flush((uintptr_t) src, sizeof (* src) * GXSIZE(w, h));

	/* исходный растр */
	DMA2D->FGMAR = (uintptr_t) src;
	DMA2D->FGOR = (DMA2D->FGOR & ~ (DMA2D_FGOR_LO)) |
		((GXADJ(w) - w) << DMA2D_FGOR_LO_Pos) |
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
		hardware_nonguiyield();

#else
	// программная реализация

	// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
	if (t == 0)
	{
		const size_t len = (size_t) GXSIZE(w, h) * sizeof * src;
		// ширина строки одинаковая в получателе и источнике
		memcpy(dst, src, len);
		arm_hardware_flush((uintptr_t) dst, len);
	}
	else
	{
		const size_t len = w * sizeof * src;
		while (h --)
		{
			memcpy(dst, src, len);
			arm_hardware_flush((uintptr_t) dst, len);
			src += GXADJ(w);
			dst += w + t;
		}
	}

#endif
}

// Routine to draw a line in the RGB565 color to the LCD.
// The line is drawn from (xmin,ymin) to (xmax,ymax).
// The algorithm used to draw the line is "Bresenham's line
// algorithm".
#define SWAP(a, b)  do { (a) ^= (b); (b) ^= (a); (a) ^= (b); } while (0)
// Нарисовать линию указанным цветом
void colpip_line(
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
	   colpip_point(buffer, dx, dy, xDraw, yDraw, color);
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
void colpip_rect(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,	// размер буфера
	uint_fast16_t dy,	// размер буфера
	uint_fast16_t x1,	// начальная координата
	uint_fast16_t y1,	// начальная координата
	uint_fast16_t x2,	// конечная координата (включена в заполняемую облсть)
	uint_fast16_t y2,	// конечная координата (включена в заполняемую облсть)
	COLORPIP_T color,
	uint_fast8_t fill
	)
{
	ASSERT(x2 > x1);
	ASSERT(y2 > y1);
	ASSERT(x2 < dx);
	ASSERT(y2 < dy);

	const uint_fast16_t w = x2 - x1 + 1;	// размер по горизонтали
	const uint_fast16_t h = y2 - y1 + 1;	// размер по вертикали

	ASSERT((x1 + w) <= dx);
	ASSERT((y1 + h) <= dy);


	if (w < 3 || h < 3)
		return;

	if (fill != 0)
	{
		colpip_fillrect(buffer, dx, dy, x1, y1, w, h, color);
	}
	else
	{
		colpip_fillrect(buffer, dx, dy, x1, y1, w, 1, color);	// верхняя горизонталь
		colpip_fillrect(buffer, dx, dy, x1, y2, w, 1, color);	// нижняя горизонталь
		colpip_fillrect(buffer, dx, dy, x1, y1 + 1, 1, h - 2, color);	// левая вертикаль
		colpip_fillrect(buffer, dx, dy, x2, y1 + 1, 1, h - 2, color);	// правая вертикаль
	}
}

void
colmain_fillrect_pattern(
	PACKEDCOLORMAIN_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x, uint_fast16_t y, 	// координаты в пикселях
	uint_fast16_t w, uint_fast16_t h, 	// размеры в пикселях
	COLORMAIN_T fgcolor,
	COLORMAIN_T bgcolor,
	uint_fast8_t hpattern	// horizontal pattern (LSB - left)
	)
{

#if LCDMODE_HORFILL

	// TODO: bgcolor и hpattern пока игнорируются
	#if LCDMODE_MAIN_L8 && LCDMODE_LTDC
		hwacc_fillrect_u8(buffer, dx, dy, x, y, w, h, fgcolor);
	#elif LCDMODE_LTDC_L24 && LCDMODE_LTDC
		hwacc_fillrect_u24(buffer, dx, dy, x, y, w, h, fgcolor);
	#elif LCDMODE_LTDC
		hwacc_fillrect_u16(buffer, dx, dy, x, y, w, h, fgcolor);
	#endif

#else /* LCDMODE_HORFILL */

	// TODO: bgcolor и hpattern пока игнорируются
	#if LCDMODE_MAIN_L8 && LCDMODE_LTDC
		hwacc_fillrect_u8(buffer, dy, dx, y, x, h, w, fgcolor);
	#elif LCDMODE_LTDC_L24 && LCDMODE_LTDC
		hwacc_fillrect_u24(buffer, dy, dx, y, x, h, w, fgcolor);
	#elif LCDMODE_LTDC
		hwacc_fillrect_u16(buffer, dy, dx, y, x, h, w, fgcolor);
	#endif

#endif /* LCDMODE_HORFILL */
}

/* заполнение прямоугольника в буфере произвольным цветом
*/
void
colmain_fillrect(
	PACKEDCOLORMAIN_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	uint_fast16_t x, uint_fast16_t y, 	// координаты в пикселях
	uint_fast16_t w, uint_fast16_t h, 	// размеры в пикселях
	COLORMAIN_T color
	)
{
	colmain_fillrect_pattern(buffer, dx, dy, x, y, w, h, color, color, 0xFF);
}

#if LCDMODE_HORFILL
// для случая когда горизонтальные пиксели в видеопямяти располагаются подряд
#if 0
// функции работы с colorbuffer не занимаются выталкиванеим кэш-памяти
static void RAMFUNC ltdcpip_horizontal_pixels(
	PACKEDCOLORPIP_T * tgr,		// target raster
	const FLASHMEM uint8_t * raster,
	uint_fast16_t width	// number of bits (start from LSB first byte in raster)
	)
{
	uint_fast16_t col;
	uint_fast16_t w = width;

	for (col = 0; w >= 8; col += 8, w -= 8)
	{
		const FLASHMEM PACKEDCOLORPIP_T * const pcl = (* byte2runpip) [* raster ++];
		memcpy(tgr + col, pcl, sizeof (* tgr) * 8);
	}
	if (w != 0)
	{
		const FLASHMEM PACKEDCOLORPIP_T * const pcl = (* byte2runpip) [* raster];
		memcpy(tgr + col, pcl, sizeof (* tgr) * w);
	}
	// функции работы с colorbuffer не занимаются выталкиванеим кэш-памяти
	//arm_hardware_flush((uintptr_t) tgr, sizeof (* tgr) * width);
}
#endif

// функции работы с colorbuffer не занимаются выталкиванеим кэш-памяти
// Фон не трогаем
static void RAMFUNC ltdcmain_horizontal_pixels_tbg(
	volatile PACKEDCOLORMAIN_T * tgr,		// target raster
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

#if 0//SMALLCHARW
// return new x coordinate
static uint_fast16_t
RAMFUNC_NONILINE
ltdcmain_horizontal_put_char_small(
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
		volatile PACKEDCOLORMAIN_T * const tgr = colmain_mem_at(buffer, dx, dy, x, y + cgrow);
		ltdcpip_horizontal_pixels(tgr, S1D13781_smallfont_LTDC [c] [cgrow], width);
	}
	return x + width;
}
#endif /* SMALLCHARW */

#if defined (SMALLCHARW)
// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
// return new x coordinate
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
		volatile PACKEDCOLORMAIN_T * const tgr = colmain_mem_at(buffer, dx, dy, x, y + cgrow);
		ltdcmain_horizontal_pixels_tbg(tgr, S1D13781_smallfont_LTDC [c] [cgrow], width, fg);
	}
	return x + width;
}

#endif /* defined (SMALLCHARW) */

#if SMALLCHARW2
// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
// return new x coordinate
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
		volatile PACKEDCOLORMAIN_T * const tgr = colmain_mem_at(buffer, dx, dy, x, y + cgrow);
		ltdcmain_horizontal_pixels_tbg(tgr, S1D13781_smallfont2_LTDC [c] [cgrow], width, fg);
	}
	return x + width;
}
#endif /* SMALLCHARW2 */

#if SMALLCHARW3
// возвращаем на сколько пикселей вправо занимет отрисованный символ
// Фон не трогаем
// return new x coordinate
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
		volatile PACKEDCOLORMAIN_T * const tgr = colmain_mem_at(buffer, dx, dy, x, y + cgrow);
		ltdcmain_horizontal_pixels_tbg(tgr, & S1D13781_smallfont3_LTDC [c] [cgrow], width, fg);
	}
	return x + width;
}
#endif /* SMALLCHARW3 */



#if defined (SMALLCHARW)

// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string_tbg(
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

	ASSERT(s != NULL);
	while((c = * s ++) != '\0')
	{
		x = ltdcpip_horizontal_put_char_small_tbg(buffer, dx, dy, x, y, c, fg);
	}
}
#endif /* defined (SMALLCHARW) */

#if defined (SMALLCHARW2)

// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string2_tbg(
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
	ASSERT(s != NULL);
	while((c = * s ++) != '\0')
	{
		x = ltdcpip_horizontal_put_char_small2_tbg(buffer, dx, dy, x, y, c, fg);
	}
}

// Возвращает ширину строки в пикселях
uint_fast16_t strwidth2(
	const char * s
	)
{
	ASSERT(s != NULL);
	return SMALLCHARW2 * strlen(s);
}

#endif /* defined (SMALLCHARW2) */

#if defined (SMALLCHARW3)
// Используется при выводе на графический индикатор,
// transparent background - не меняем цвет фона.
void
colpip_string3_tbg(
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

	ASSERT(s != NULL);
	while((c = * s ++) != '\0')
	{
		x = ltdcpip_horizontal_put_char_small3_tbg(buffer, dx, dy, x, y, c, fg);
	}
}

// Возвращает ширину строки в пикселях
uint_fast16_t strwidth3(
	const char * s
	)
{
	ASSERT(s != NULL);
	return SMALLCHARW3 * strlen(s);
}

#endif /* defined (SMALLCHARW3) */


#if defined (SMALLCHARW) && defined (SMALLCHARH)
// Возвращает ширину строки в пикселях
uint_fast16_t strwidth(
	const char * s
	)
{
	ASSERT(s != NULL);
	return SMALLCHARW * strlen(s);
}

// Возвращает высоту строки в пикселях
uint_fast16_t colpip_string_height(
	PACKEDCOLORPIP_T * buffer,
	uint_fast16_t dx,
	uint_fast16_t dy,
	const char * s
	)
{
	ASSERT(s != NULL);
	(void) buffer;
	(void) dx;
	(void) dy;
	(void) s;
	return SMALLCHARH;
}

#endif /* defined (SMALLCHARW) && defined (SMALLCHARH) */

#else /* LCDMODE_HORFILL */

#endif /* LCDMODE_HORFILL */


// скоприовать прямоугольник с типом пикселей соответствующим основному экрану
void colmain_plot(
	PACKEDCOLORMAIN_T * dst,	// получатель
	uint_fast16_t tdx,	// получатель
	uint_fast16_t tdy,	// получатель
	uint_fast16_t x,	// получатель
	uint_fast16_t y,	// получатель
	const PACKEDCOLORMAIN_T * src, 	// источник
	uint_fast16_t dx,	// источник Размеры окна в пикселях
	uint_fast16_t dy	// источник
	)
{
	ASSERT(src != NULL);
	ASSERT(dst != NULL);
	ASSERT(tdx >= dx);
	ASSERT(tdy >= dy);
#if LCDMODE_HORFILL
	hwaccel_copy(
		(uintptr_t) dst, sizeof (PACKEDCOLORPIP_T) * tdx * tdy,	// target area invalidate parameters
		colmain_mem_at(dst, tdx, tdy, x, y),
		src,
		dx, tdx - dx, dy);	// w, t, h
#else /* LCDMODE_HORFILL */
	hwaccel_copy(
		(uintptr_t) dst, sizeof (PACKEDCOLORPIP_T) * tdx * tdy,	// target area invalidate parameters
		colmain_mem_at(dst, tdx, tdy, x, y),
		src,
		dy, tdy - dy, dx);	// w, t, h
#endif /* LCDMODE_HORFILL */
}

// скоприовать прямоугольник с типом пикселей соответствующим pip
void colpip_plot(
	PACKEDCOLORPIP_T * dst,	// получатель
	uint_fast16_t tdx,	// получатель
	uint_fast16_t tdy,	// получатель
	uint_fast16_t x,	// получатель
	uint_fast16_t y,	// получатель
	const PACKEDCOLORPIP_T * src, 	// источник
	uint_fast16_t dx,	// источник Размеры окна в пикселях
	uint_fast16_t dy	// источник
	)
{
	ASSERT(src != NULL);
	ASSERT(dst != NULL);
	ASSERT(tdx >= dx);
	ASSERT(tdy >= dy);
#if LCDMODE_HORFILL
	hwaccel_copy(
		(uintptr_t) dst, sizeof (PACKEDCOLORPIP_T) * tdx * tdy,	// target area invalidate parameters
		colmain_mem_at(dst, tdx, tdy, x, y),
		src,
		dx, tdx - dx, dy);	// w, t, h
#else /* LCDMODE_HORFILL */
	hwaccel_copy(
		(uintptr_t) dst, sizeof (PACKEDCOLORPIP_T) * tdx * tdy,	// target area invalidate parameters
		colmain_mem_at(dst, tdx, tdy, x, y),
		src,
		dy, tdy - dy, dx);	// w, t, h
#endif /* LCDMODE_HORFILL */
}


#if SMALLCHARH3

static uint_fast16_t
RAMFUNC_NONILINE ltdc_horizontal_put_char_small3(
	PACKEDCOLORMAIN_T * const buffer,
	const uint_fast16_t dx,
	const uint_fast16_t dy,
	uint_fast16_t x, uint_fast16_t y,
	char cc
	)
{
	const uint_fast8_t width = SMALLCHARW3;
	const uint_fast8_t c = smallfont_decode((unsigned char) cc);
	uint_fast8_t cgrow;
	for (cgrow = 0; cgrow < SMALLCHARH3; ++ cgrow)
	{
		PACKEDCOLORMAIN_T * const tgr = colmain_mem_at(buffer, dx, dy, x, y + cgrow);
		ltdc_horizontal_pixels(tgr, & S1D13781_smallfont3_LTDC [c] [cgrow], width);
	}
	return x + width;
}

static void
display_string3(uint_fast16_t x, uint_fast16_t y, const char * s, uint_fast8_t lowhalf)
{
	PACKEDCOLORMAIN_T * const buffer = colmain_fb_draw();
	const uint_fast16_t dx = DIM_X;
	const uint_fast16_t dy = DIM_Y;
	char c;
//	ltdc_secondoffs = 0;
//	ltdc_h = SMALLCHARH3;
	while((c = * s ++) != '\0')
		x = ltdc_horizontal_put_char_small3(buffer, dx, dy, x, y, c);
}

void
colmain_string3_at_xy(
	PACKEDCOLORMAIN_T * const buffer,
	const uint_fast16_t dx,
	const uint_fast16_t dy,
	uint_fast16_t x,
	uint_fast16_t y,
	const char * s
	)
{
	char c;
//	ltdc_secondoffs = 0;
//	ltdc_h = SMALLCHARH3;
	while((c = * s ++) != '\0')
		x = ltdc_horizontal_put_char_small3(buffer, dx, dy, x, y, c);
}

void
display_string3_at_xy(uint_fast16_t x, uint_fast16_t y, const char * s, COLORMAIN_T fg, COLORMAIN_T bg)
{
	uint_fast8_t lowhalf = HALFCOUNT_SMALL - 1;
	colmain_setcolors(fg, bg);
	do
	{
		display_string3(x, y + lowhalf, s, lowhalf);
	} while (lowhalf --);
}

#endif /* SMALLCHARH3 */

