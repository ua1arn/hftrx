/* $Id$ */
/* Индикатор 160*128 с контроллером Sitronix ST7735 */
/* Индикатор 160*128 с контроллером ILITEK ILI9163 MI0177FT-1 */
/* Индикатор 176*132 с контроллером ILITEK ILI9163 */
/* Индикатор 320*240 с контроллером ILITEK ILI9341 */

#ifndef ST7735_H_INCLUDED
#define ST7735_H_INCLUDED

#include "hardware.h"	/* зависящие от процессора функции работы с портами */

#if LCDMODE_ST7735 || LCDMODE_ILI9163 || LCDMODE_ILI9341

#define DISPLAYMODES_FPS 5	/* количество обновлений отображения режимов работы */
#if LCDMODE_ILI9341
	#define DISPLAY_FPS	10	/* обновление показаний частоты десять раз в секунду */
	#define DISPLAYSWR_FPS 10	/* количество обновлений SWR и панорамы за секунду */
#else /* LCDMODE_ILI9341 */
	#define DISPLAY_FPS	10	/* обновление показаний частоты десять раз в секунду */
	#define DISPLAYSWR_FPS 10	/* количество обновлений SWR и панорамы за секунду */
#endif /* LCDMODE_ILI9341 */

#if LCDMODE_LTDC

	// Определения COLOR_T, PACKEDCOLOR_T и TFTRGB используются из diaplay.h

#elif LCDMODE_ILI9163 || LCDMODE_BGR565
	//#define LCDMODE_BGR565 1

	typedef uint_fast16_t COLOR_T;
	typedef uint16_t PACKEDCOLOR_T;

	// BBBB.BGGG.GGGR.RRRR
	#define TFTRGB(red, green, blue) \
		(  (uint_fast16_t) \
			(	\
				(((uint_fast16_t) (blue) << 8) &   0xf800)  | \
				(((uint_fast16_t) (green) << 3) & 0x07e0) | \
				(((uint_fast16_t) (red) >> 3) &  0x001f) \
			) \
		)

#else /* LCDMODE_LTDC */
	//#define LCDMODE_RGB565 1

	typedef uint_fast16_t COLOR_T;
	typedef uint16_t PACKEDCOLOR_T;

	// RRRR.RGGG.GGGB.BBBB
	#define TFTRGB(red, green, blue) \
		(  (uint_fast16_t) \
			(	\
				(((uint_fast16_t) (red) << 8) &   0xf800)  | \
				(((uint_fast16_t) (green) << 3) & 0x07e0) | \
				(((uint_fast16_t) (blue) >> 3) &  0x001f) \
			) \
		)

#endif /* LCDMODE_LTDC */

// some flags for initR() :(
#define INITR_GREENTAB 0x0
#define INITR_REDTAB   0x1
#define INITR_BLACKTAB   0x2

#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13

#define ST7735_INVOFF  0x20
#define ST7735_INVON   0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON  0x29
#define ST7735_CASET   0x2A
#define ST7735_RASET   0x2B
#define ST7735_RAMWR   0x2C
#define ST7735_RAMRD   0x2E

#define ST7735_PTLAR   	0x30
#define ST7735_COLMOD  	0x3A
#define ST7735_MADCTL	0x36

#define ILI9341_VSCRDEF				0x33//Vertical Scrolling Definition
//#define ILI9341_MAC				0x36
#define ILI9341_VSCRSADD			0x37//Vertical Scrolling Start Address

#define ST7735_IDLEOFF	0x38
#define ST7735_IDLEON	0x39

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6	// In ILITEK not specified

#define ST7735_PWCTR1  0xC0	// Set the GVDD and voltage
#define ST7735_PWCTR2  0xC1	// Set the AVDD, VCL, VGH and VGL supply power level.
#define ST7735_PWCTR3  0xC2	// Set the amount of current in Operation amplifier in normal mode/full colors.
#define ST7735_PWCTR4  0xC3	// Set the amount of current in Operational amplifier in Idle mode/8-colors
#define ST7735_PWCTR5  0xC4	// Set the amount of current in Operational amplifier in Partial mode/full-colors
#define ST7735_VMCTR1  0xC5	// Set VCOMH Voltage

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD

#define ST7735_PWCTR6  0xFC

#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1


/** 
  * ILI9341 LCD Registers  
  */ 
#define LCD_SLEEP_OUT            0x11   /* Sleep out register */
#define LCD_GAMMA                0x26   /* Gamma register */
#define LCD_DISPLAY_OFF          0x28   /* Display off register */
#define LCD_DISPLAY_ON           0x29   /* Display on register */
#define LCD_COLUMN_ADDR          0x2A   /* Colomn address register */ 
#define LCD_PAGE_ADDR            0x2B   /* Page address register */ 
#define LCD_GRAM                 0x2C   /* GRAM register */   
#define LCD_MAC                  0x36   /* Memory Access Control register*/
#define LCD_PIXEL_FORMAT         0x3A   /* Pixel Format register */
#define LCD_WDB                  0x51   /* Write Brightness Display register */
#define LCD_WCD                  0x53   /* Write Control Display register*/
#define LCD_RGB_INTERFACE        0xB0   /* RGB Interface Signal Control */
#define LCD_FRC                  0xB1   /* Frame Rate Control register */
#define LCD_BPC                  0xB5   /* Blanking Porch Control register*/
#define LCD_DFC                  0xB6   /* Display Function Control register*/
#define LCD_POWER1               0xC0   /* Power Control 1 register */
#define LCD_POWER2               0xC1   /* Power Control 2 register */
#define LCD_VCOM1                0xC5   /* VCOM Control 1 register */
#define LCD_VCOM2                0xC7   /* VCOM Control 2 register */
#define LCD_POWERA               0xCB   /* Power control A register */
#define LCD_POWERB               0xCF   /* Power control B register */
#define LCD_PGAMMA               0xE0   /* Positive Gamma Correction register*/
#define LCD_NGAMMA               0xE1   /* Negative Gamma Correction register*/
#define LCD_DTCA                 0xE8   /* Driver timing control A */
#define LCD_DTCB                 0xEA   /* Driver timing control B */
#define LCD_POWER_SEQ            0xED   /* Power on sequence register */
#define LCD_3GAMMA_EN            0xF2   /* 3 Gamma enable register */
#define LCD_INTERFACE            0xF6   /* Interface control register */
#define LCD_PRC                  0xF7   /* Pump ratio control register */

#endif /* LCDMODE_ST7735 || LCDMODE_ILI9163 || LCDMODE_ILI9341 */
#endif /* ST7735_H_INCLUDED */

