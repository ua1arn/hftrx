/* $Id$ */
//
// ѕроект HF Dream Receiver ( ¬ приЄмник мечты)
// автор √ена «авидовский mgs2001@mail.ru
// UA1ARN
//
// STM32F42XX LCD-TFT Controller (LTDC)

#include "hardware.h"

#include "display.h"
#include <stdint.h>
#include <string.h>

#include "formats.h"	// for debug prints
#include "spifuncs.h"

#if CPUSTYLE_STM32F && LCDMODE_LTDC

//#define GCR_MASK                     ((uint32_t)0x0FFE888F)  /* LTDC GCR Mask */
/** @defgroup LTDC_Pixelformat 
  * @{
  */
#define LTDC_Pixelformat_ARGB8888                  ((uint32_t)0x00000000)
#define LTDC_Pixelformat_RGB888                    ((uint32_t)0x00000001)
#define LTDC_Pixelformat_RGB565                    ((uint32_t)0x00000002)
#define LTDC_Pixelformat_ARGB1555                  ((uint32_t)0x00000003)
#define LTDC_Pixelformat_ARGB4444                  ((uint32_t)0x00000004)
#define LTDC_Pixelformat_L8                        ((uint32_t)0x00000005)
#define LTDC_Pixelformat_AL44                      ((uint32_t)0x00000006)
#define LTDC_Pixelformat_AL88                      ((uint32_t)0x00000007)
/** @defgroup LTDC_BlendingFactor1 
  * @{
  */

#define LTDC_BlendingFactor1_CA                       ((uint32_t)0x00000400)
#define LTDC_BlendingFactor1_PAxCA                    ((uint32_t)0x00000600)

/**
  * @}
  */
      
/** @defgroup LTDC_BlendingFactor2
  * @{
  */

#define LTDC_BlendingFactor2_CA                       ((uint32_t)0x00000005)
#define LTDC_BlendingFactor2_PAxCA                    ((uint32_t)0x00000007)

/**
  * @}
  */

/** @defgroup LTDC_Reload 
  * @{
  */
#define LTDC_IMReload                     LTDC_SRCR_IMR                         /*!< Immediately Reload. */
#define LTDC_VBReload                     LTDC_SRCR_VBR                         /*!< Vertical Blanking Reload. */

/**
  * @}
  */
  
/** @defgroup LTDC_HSPolarity 
  * @{
  */
#define LTDC_HSPolarity_AL                0                /*!< Horizontal Synchronization is active low. */
#define LTDC_HSPolarity_AH                LTDC_GCR_HSPOL                        /*!< Horizontal Synchronization is active high. */

/**
  * @}
  */
  
/** @defgroup LTDC_VSPolarity 
  * @{
  */
#define LTDC_VSPolarity_AL                0                /*!< Vertical Synchronization is active low. */
#define LTDC_VSPolarity_AH                LTDC_GCR_VSPOL                        /*!< Vertical Synchronization is active high. */

/**
  * @}
  */
  
/** @defgroup LTDC_DEPolarity 
  * @{
  */
// 0: Not Data Enable polarity is active low
// 1: Not Data Enable polarity is active high 
#define LTDC_DEPolarity_AL                0                /*!< Data Enable, is active low. */
#define LTDC_DEPolarity_AH                LTDC_GCR_DEPOL                        /*!< Data Enable, is active high. */

/**
  * @}
  */

/** @defgroup LTDC_PCPolarity 
  * @{
  */
#define LTDC_PCPolarity_IPC               0                /*!< input pixel clock. */
#define LTDC_PCPolarity_IIPC              LTDC_GCR_PCPOL                        /*!< inverted input pixel clock. */


/* Exported types ------------------------------------------------------------*/
 
/** 
  * @brief  LTDC Init structure definition  
  */

typedef struct
{
  uint32_t LTDC_HSPolarity;                 /*!< configures the horizontal synchronization polarity.
                                                 This parameter can be one value of @ref LTDC_HSPolarity */

  uint32_t LTDC_VSPolarity;                 /*!< configures the vertical synchronization polarity.
                                                 This parameter can be one value of @ref LTDC_VSPolarity */

  uint32_t LTDC_DEPolarity;                 /*!< configures the data enable polarity. This parameter can
                                                 be one of value of @ref LTDC_DEPolarity */

  uint32_t LTDC_PCPolarity;                 /*!< configures the pixel clock polarity. This parameter can
                                                 be one of value of @ref LTDC_PCPolarity */

  uint32_t LTDC_HorizontalSync;             /*!< configures the number of Horizontal synchronization 
                                                 width. This parameter must range from 0x000 to 0xFFF. */

  uint32_t LTDC_VerticalSync;               /*!< configures the number of Vertical synchronization 
                                                 heigh. This parameter must range from 0x000 to 0x7FF. */

  uint32_t LTDC_AccumulatedHBP;             /*!< configures the accumulated horizontal back porch width.
                                                 This parameter must range from LTDC_HorizontalSync to 0xFFF. */

  uint32_t LTDC_AccumulatedVBP;             /*!< configures the accumulated vertical back porch heigh.
                                                 This parameter must range from LTDC_VerticalSync to 0x7FF. */
            
  uint32_t LTDC_AccumulatedActiveW;         /*!< configures the accumulated active width. This parameter 
                                                 must range from LTDC_AccumulatedHBP to 0xFFF. */

  uint32_t LTDC_AccumulatedActiveH;         /*!< configures the accumulated active heigh. This parameter 
                                                 must range from LTDC_AccumulatedVBP to 0x7FF. */

  uint32_t LTDC_TotalWidth;                 /*!< configures the total width. This parameter 
                                                 must range from LTDC_AccumulatedActiveW to 0xFFF. */

  uint32_t LTDC_TotalHeigh;                 /*!< configures the total heigh. This parameter 
                                                 must range from LTDC_AccumulatedActiveH to 0x7FF. */
            
  uint32_t LTDC_BackgroundRedValue;         /*!< configures the background red value.
                                                 This parameter must range from 0x00 to 0xFF. */

  uint32_t LTDC_BackgroundGreenValue;       /*!< configures the background green value.
                                                 This parameter must range from 0x00 to 0xFF. */ 

   uint32_t LTDC_BackgroundBlueValue;       /*!< configures the background blue value.
                                                 This parameter must range from 0x00 to 0xFF. */
} LTDC_InitTypeDef;

/** 
  * @brief  LTDC Layer structure definition  
  */

typedef struct
{
  uint32_t LTDC_HorizontalStart;            /*!< Configures the Window Horizontal Start Position.
                                                 This parameter must range from 0x000 to 0xFFF. */
            
  uint32_t LTDC_HorizontalStop;             /*!< Configures the Window Horizontal Stop Position.
                                                 This parameter must range from 0x0000 to 0xFFFF. */
  
  uint32_t LTDC_VerticalStart;              /*!< Configures the Window vertical Start Position.
                                                 This parameter must range from 0x000 to 0xFFF. */

  uint32_t LTDC_VerticalStop;               /*!< Configures the Window vaertical Stop Position.
                                                 This parameter must range from 0x0000 to 0xFFFF. */
  
  uint32_t LTDC_PixelFormat;                /*!< Specifies the pixel format. This parameter can be 
                                                 one of value of @ref LTDC_Pixelformat */

  uint32_t LTDC_ConstantAlpha;              /*!< Specifies the constant alpha used for blending.
                                                 This parameter must range from 0x00 to 0xFF. */

  uint32_t LTDC_DefaultColorBlue;           /*!< Configures the default blue value.
                                                 This parameter must range from 0x00 to 0xFF. */

  uint32_t LTDC_DefaultColorGreen;          /*!< Configures the default green value.
                                                 This parameter must range from 0x00 to 0xFF. */
            
  uint32_t LTDC_DefaultColorRed;            /*!< Configures the default red value.
                                                 This parameter must range from 0x00 to 0xFF. */

  uint32_t LTDC_DefaultColorAlpha;          /*!< Configures the default alpha value.
                                                 This parameter must range from 0x00 to 0xFF. */

  uint32_t LTDC_BlendingFactor_1;           /*!< Select the blending factor 1. This parameter 
                                                 can be one of value of @ref LTDC_BlendingFactor1 */

  uint32_t LTDC_BlendingFactor_2;           /*!< Select the blending factor 2. This parameter 
                                                 can be one of value of @ref LTDC_BlendingFactor2 */
            
  uint32_t LTDC_CFBStartAdress;             /*!< Configures the color frame buffer address */

  uint32_t LTDC_CFBLineLength;              /*!< Configures the color frame buffer line length. 
                                                 This parameter must range from 0x0000 to 0x1FFF. */

  uint32_t LTDC_CFBPitch;                   /*!< Configures the color frame buffer pitch in bytes.
                                                 This parameter must range from 0x0000 to 0x1FFF. */
                                                 
  uint32_t LTDC_CFBLineNumber;              /*!< Specifies the number of line in frame buffer. 
                                                 This parameter must range from 0x000 to 0x7FF. */
} LTDC_Layer_InitTypeDef;

#if LCDMODE_LTDC_L24

	#define SCALE_H 3

// —оздаЄм палитру выполн€ющую просто трансл€цию значени€ 
static void
fillLUT_L24(
	LTDC_Layer_TypeDef* LTDC_Layerx
	)
{
	unsigned color;

	for (color = 0; color < 256; ++ color)
	{
		uint_fast8_t r = color, g = color, b = color;
		
		/* запись значений в регистры палитры */
		LTDC_Layerx->CLUTWR = 
			((color << 24) & LTDC_LxCLUTWR_CLUTADD) |
			((r << 16) & LTDC_LxCLUTWR_RED) |
			((g << 8) & LTDC_LxCLUTWR_GREEN) |
			((b << 0) & LTDC_LxCLUTWR_BLUE);
	}

	LTDC_Layerx->CR |= LTDC_LxCR_CLUTEN;
}

#elif LCDMODE_LTDC_L8
	#define SCALE_H 1

static void
fillLUT_L8(
	LTDC_Layer_TypeDef* LTDC_Layerx
	)
{
	unsigned color;

	for (color = 0; color < 256; ++ color)
	{
#define XRGB(zr,zg,zb) do { r = (zr), g = (zg), b = (zb); } while (0)
		uint_fast8_t r, g, b;

		switch (color)
		{
		case TFTRGB(0, 0, 0)			/*COLOR_BLACK*/:		XRGB(0, 0, 0);			break;	// 0x00 черный
		case TFTRGB(255, 0, 0)			/*COLOR_RED*/:			XRGB(255, 0, 0);		break; 	// 0xE0 красный
		case TFTRGB(0, 255, 0)			/*COLOR_GREEN*/:		XRGB(0, 255, 0);		break; 	// 0x1C зеленый
		case TFTRGB(0, 0, 255)			/*COLOR_BLUE*/:			XRGB(0, 0, 255);		break; 	// 0x03 синий
		case TFTRGB(128, 0, 0)			/*COLOR_DARKRED*/:		XRGB(128, 0, 0);		break; 	// 
		case TFTRGB(0, 128, 0)			/*COLOR_DARKGREEN*/:	XRGB(0, 128, 0);		break; 	// 
		case TFTRGB(0, 0, 128)			/*COLOR_DARKBLUE*/:		XRGB(0, 0, 128);		break; 	// 
		case TFTRGB(255, 255, 0)		/*COLOR_YELLOW*/:		XRGB(255, 255, 0);		break; 	// 0xFC желтый
		case TFTRGB(255, 0, 255)		/*COLOR_MAGENTA*/:		XRGB(255, 0, 255);		break; 	// 0x83 пурпурный
		case TFTRGB(0, 255, 255)		/*COLOR_CYAN*/:			XRGB(0, 255, 255);		break; 	// 0x1F голубой
		case TFTRGB(255, 255, 255)		/*COLOR_WHITE*/:		XRGB(255, 255, 255);	break;  // 0xff	белый
		case TFTRGB(128, 128, 128)		/*COLOR_GRAY*/:			XRGB(128, 128, 128);	break; 	// серый
		case TFTRGB(0xa5, 0x2a, 0x2a)	/*COLOR_BROWN*/:		XRGB(0xa5, 0x2a, 0x2a);	break; 	// 0x64 коричневый
		case TFTRGB(0xff, 0xd7, 0x00)	/*COLOR_GOLD*/:			XRGB(0xff, 0xd7, 0x00);	break; 	// 0xF4 золото
		case TFTRGB(0xd1, 0xe2, 0x31)	/*COLOR_PEAR*/:			XRGB(0xd1, 0xe2, 0x31);	break; 	// 0xDC грушевый
#undef XRGB
		default:
			r = ((color & 0xe0) << 0) | ((color & 0x80) ? 0x1f : 0);	// red
			g = ((color & 0x1c) << 3) | ((color & 0x10) ? 0x1f : 0);	// green
			b = ((color & 0x03) << 6) | ((color & 0x02) ? 0x3f : 0);	// blue
			break;
		}
		/* запись значений в регистры палитры */
		LTDC_Layerx->CLUTWR = 
			((color << 24) & LTDC_LxCLUTWR_CLUTADD) |
			((r << 16) & LTDC_LxCLUTWR_RED) |
			((g << 8) & LTDC_LxCLUTWR_GREEN) |
			((b << 0) & LTDC_LxCLUTWR_BLUE);
	}

	LTDC_Layerx->CR |= LTDC_LxCR_CLUTEN;
}
#else
	#define SCALE_H 1

#endif /* LCDMODE_LTDC_L8 */


/**
  * @brief  Initializes the LTDC Layer according to the specified parameters
  *         in the LTDC_LayerStruct.
  * @note   This function can be used only when the LTDC is disabled.
  * @param  LTDC_layerx: Select the layer to be configured, this parameter can be 
  *         one of the following values: LTDC_Layer1, LTDC_Layer2    
  * @param  LTDC_LayerStruct: pointer to a LTDC_LayerTypeDef structure that contains
  *         the configuration information for the specified LTDC peripheral.
  * @retval None
  */

static void 
LTDC_LayerInit(LTDC_Layer_TypeDef* LTDC_Layerx, const LTDC_Layer_InitTypeDef* LTDC_Layer_InitStruct)
{

	uint32_t whsppos = 0;
	uint32_t wvsppos = 0;
	uint32_t dcgreen = 0;
	uint32_t dcred = 0;
	uint32_t dcalpha = 0;
	uint32_t cfbp = 0;

	/* Configures the horizontal start and stop position */
	whsppos = LTDC_Layer_InitStruct->LTDC_HorizontalStop << 16;
	LTDC_Layerx->WHPCR &= ~(LTDC_LxWHPCR_WHSTPOS | LTDC_LxWHPCR_WHSPPOS);
	LTDC_Layerx->WHPCR |= (LTDC_Layer_InitStruct->LTDC_HorizontalStart | whsppos);

	/* Configures the vertical start and stop position */
	wvsppos = LTDC_Layer_InitStruct->LTDC_VerticalStop << 16;
	LTDC_Layerx->WVPCR &= ~(LTDC_LxWVPCR_WVSTPOS | LTDC_LxWVPCR_WVSPPOS);
	LTDC_Layerx->WVPCR |= (LTDC_Layer_InitStruct->LTDC_VerticalStart | wvsppos);

	/* Specifies the pixel format */
	LTDC_Layerx->PFCR &= ~(LTDC_LxPFCR_PF);
	LTDC_Layerx->PFCR |= (LTDC_Layer_InitStruct->LTDC_PixelFormat);

	/* Configures the default color values */
	dcgreen = (LTDC_Layer_InitStruct->LTDC_DefaultColorGreen << 8);
	dcred = (LTDC_Layer_InitStruct->LTDC_DefaultColorRed << 16);
	dcalpha = (LTDC_Layer_InitStruct->LTDC_DefaultColorAlpha << 24);
	LTDC_Layerx->DCCR &=  ~(LTDC_LxDCCR_DCBLUE | LTDC_LxDCCR_DCGREEN | LTDC_LxDCCR_DCRED | LTDC_LxDCCR_DCALPHA);
	LTDC_Layerx->DCCR |= (LTDC_Layer_InitStruct->LTDC_DefaultColorBlue | dcgreen | \
						dcred | dcalpha);

	/* Specifies the constant alpha value */      
	LTDC_Layerx->CACR &= ~(LTDC_LxCACR_CONSTA);
	LTDC_Layerx->CACR |= (LTDC_Layer_InitStruct->LTDC_ConstantAlpha);

	/* Specifies the blending factors */
	LTDC_Layerx->BFCR &= ~(LTDC_LxBFCR_BF2 | LTDC_LxBFCR_BF1);
	LTDC_Layerx->BFCR |= (LTDC_Layer_InitStruct->LTDC_BlendingFactor_1 | LTDC_Layer_InitStruct->LTDC_BlendingFactor_2);

	/* Configures the color frame buffer start address */
	LTDC_Layerx->CFBAR &= ~(LTDC_LxCFBAR_CFBADD);
	LTDC_Layerx->CFBAR |= (LTDC_Layer_InitStruct->LTDC_CFBStartAdress);

	/* Configures the color frame buffer pitch in byte */
	cfbp = (LTDC_Layer_InitStruct->LTDC_CFBPitch << 16);
	LTDC_Layerx->CFBLR  &= ~(LTDC_LxCFBLR_CFBLL | LTDC_LxCFBLR_CFBP);
	LTDC_Layerx->CFBLR  |= (LTDC_Layer_InitStruct->LTDC_CFBLineLength | cfbp);

	/* Configures the frame buffer line number */
	LTDC_Layerx->CFBLNR  &= ~(LTDC_LxCFBLNR_CFBLNBR);
	LTDC_Layerx->CFBLNR  |= (LTDC_Layer_InitStruct->LTDC_CFBLineNumber);

}
/**
  * @brief  Initializes the LTDC peripheral according to the specified parameters
  *         in the LTDC_InitStruct.
  * @note   This function can be used only when the LTDC is disabled.
  * @param  LTDC_InitStruct: pointer to a LTDC_InitTypeDef structure that contains
  *         the configuration information for the specified LTDC peripheral.
  * @retval None
  */

static void LTDC_Init(LTDC_InitTypeDef* LTDC_InitStruct)
{
	uint32_t horizontalsync = 0;
	uint32_t accumulatedHBP = 0;
	uint32_t accumulatedactiveW = 0;
	uint32_t totalwidth = 0;
	uint32_t backgreen = 0;
	uint32_t backred = 0;

	/* Sets Synchronization size */
	LTDC->SSCR &= ~(LTDC_SSCR_VSH | LTDC_SSCR_HSW);
	horizontalsync = (LTDC_InitStruct->LTDC_HorizontalSync << LTDC_SSCR_HSW_Pos);
	LTDC->SSCR |= (horizontalsync | LTDC_InitStruct->LTDC_VerticalSync);

	/* Sets Accumulated Back porch */
	LTDC->BPCR &= ~(LTDC_BPCR_AVBP | LTDC_BPCR_AHBP);
	accumulatedHBP = (LTDC_InitStruct->LTDC_AccumulatedHBP << LTDC_BPCR_AHBP_Pos);
	LTDC->BPCR |= (accumulatedHBP | LTDC_InitStruct->LTDC_AccumulatedVBP);

	/* Sets Accumulated Active Width */
	LTDC->AWCR &= ~(LTDC_AWCR_AAH | LTDC_AWCR_AAW);
	accumulatedactiveW = (LTDC_InitStruct->LTDC_AccumulatedActiveW << LTDC_AWCR_AAW_Pos);
	LTDC->AWCR |= (accumulatedactiveW | LTDC_InitStruct->LTDC_AccumulatedActiveH);

	/* Sets Total Width */
	LTDC->TWCR &= ~(LTDC_TWCR_TOTALH | LTDC_TWCR_TOTALW);
	totalwidth = (LTDC_InitStruct->LTDC_TotalWidth << LTDC_TWCR_TOTALW_Pos);
	LTDC->TWCR |= (totalwidth | LTDC_InitStruct->LTDC_TotalHeigh);

	//LTDC->GCR &= (uint32_t)GCR_MASK;
	LTDC->GCR = 0;
	LTDC->GCR |=  (uint32_t)(LTDC_InitStruct->LTDC_HSPolarity | LTDC_InitStruct->LTDC_VSPolarity |
			   LTDC_InitStruct->LTDC_DEPolarity | LTDC_InitStruct->LTDC_PCPolarity);

	/* sets the background color value */
	backgreen = (LTDC_InitStruct->LTDC_BackgroundGreenValue << 8);
	backred = (LTDC_InitStruct->LTDC_BackgroundRedValue << 16);

	LTDC->BCCR &= ~(LTDC_BCCR_BCBLUE | LTDC_BCCR_BCGREEN | LTDC_BCCR_BCRED);
	LTDC->BCCR |= (backred | backgreen | LTDC_InitStruct->LTDC_BackgroundBlueValue);
}

static void LCD_LayerInit(
	unsigned hs,	// same as AccumulatedHBP + 1
	unsigned vs		// same as LTDC_AccumulatedVBP + 1
	)
{
	const unsigned rowsize = sizeof framebuff [0];	// размер одной строки в байтах
	const unsigned rowsize2 = (sizeof (PACKEDCOLOR_T) * DIM_SECOND);
	ASSERT(rowsize == rowsize2);

	LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct; 
	/* Windowing configuration */
	/* In this case all the active display area is used to display a picture then :
	Horizontal start = horizontal synchronization + Horizontal back porch = 30 
	Horizontal stop = Horizontal start + window width -1 = 30 + 240 -1
	Vertical start   = vertical synchronization + vertical back porch     = 4
	Vertical stop   = Vertical start + window height -1  = 4 + 320 -1      */      
	LTDC_Layer_InitStruct.LTDC_HorizontalStart = hs;
	LTDC_Layer_InitStruct.LTDC_HorizontalStop = (DIM_SECOND * SCALE_H + hs - 1); 
	LTDC_Layer_InitStruct.LTDC_VerticalStart = vs;
	LTDC_Layer_InitStruct.LTDC_VerticalStop = (DIM_FIRST + vs - 1);

	/* Pixel Format configuration*/
	#if LCDMODE_LTDC_L8
		LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_L8;
	#elif LCDMODE_LTDC_L24
		LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_L8;
	#else /* LCDMODE_LTDC_L8 */
		LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;
	#endif /* LCDMODE_LTDC_L8 */
	/* Alpha constant (255 totally opaque) */
	LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255; 
	/* Default Color configuration (configure A,R,G,B component values) */          
	LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;        
	LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;       
	LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;         
	LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
	/* Configure blending factors */       
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;    
	LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;

	/* the length of one line of pixels in bytes + 3 then :
	Line Lenth = Active high width x number of bytes per pixel + 3 
	Active high width         = DIM_SECOND 
	number of bytes per pixel = 2    (pixel_format : RGB565) 
	number of bytes per pixel = 1    (pixel_format : L8) 
	*/
	LTDC_Layer_InitStruct.LTDC_CFBLineLength = rowsize + 3; //((DIM_SECOND * 2) + 3);
	//LTDC_Layer1->CFBLR = ((rowsize << 16) & LTDC_LxCFBLR_CFBP) | (((rowsize + 3) << 0) & LTDC_LxCFBLR_CFBLL);
	/* the pitch is the increment from the start of one line of pixels to the 
	start of the next line in bytes, then :
	Pitch = Active high width x number of bytes per pixel */ 
	LTDC_Layer_InitStruct.LTDC_CFBPitch = rowsize; // (DIM_SECOND * 2);

	/* Configure the number of lines */  
	LTDC_Layer_InitStruct.LTDC_CFBLineNumber = DIM_FIRST;

	/* Start Address configuration : the LCD Frame buffer is defined on SDRAM */    
	LTDC_Layer_InitStruct.LTDC_CFBStartAdress = (uintptr_t) & framebuff;
	//LTDC_Layer1->CFBAR = (uint32_t) & framebuff;

	/* Initialize LTDC layer 1 */
	LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

	/* Enable foreground & background Layers */
	//LTDC_LayerCmd(LTDC_Layer1, ENABLE); 
	//LTDC_LayerCmd(LTDC_Layer2, ENABLE);
	/* Enable LTDC_Layer by setting LEN bit */
	LTDC_Layer1->CR |= (uint32_t) LTDC_LxCR_LEN;

	/* LTDC configuration reload */  
}

void
arm_hardware_ltdc_initialize(void)
{
	debug_printf_P(PSTR("arm_hardware_ltdc_initialize start\n"));

	const unsigned rowsize = sizeof framebuff [0];	// размер одной строки в байтах
	const unsigned rowsize2 = (sizeof (PACKEDCOLOR_T) * DIM_SECOND);
	ASSERT(rowsize == rowsize2);
	debug_printf_P(PSTR("arm_hardware_ltdc_initialize: framebuff=%p, rowsize=%u\n"), framebuff, rowsize);

	/*
	framebuff [0][0] = 1;
	framebuff [0][1] = 2;
	framebuff [0][2] = 3;
	framebuff [0][3] = 4;

	ASSERT(framebuff [0][0] == 1);
	ASSERT(framebuff [0][1] == 2);
	ASSERT(framebuff [0][2] == 3);
	ASSERT(framebuff [0][3] == 4);
	*/
	/* Initialize the LCD */
	//LCD_Init();

#if CPUSTYLE_STM32H7XX
	/* Enable the LTDC Clock */
	RCC->APB3ENR |= RCC_APB3ENR_LTDCEN;	/* LTDC clock enable */
	__DSB();

	/* Enable the DMA2D Clock */
	RCC->AHB3ENR |= RCC_AHB3ENR_DMA2DEN;	/* DMA2D clock enable */
	__DSB();
#else /* CPUSTYLE_STM32H7XX */
	/* Enable the LTDC Clock */
	RCC->APB2ENR |= RCC_APB2ENR_LTDCEN;	/* LTDC clock enable */
	__DSB();

	/* Enable the DMA2D Clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2DEN;	/* DMA2D clock enable */
	__DSB();
#endif /* CPUSTYLE_STM32H7XX */

	/* Configure the LCD Control pins */
	HARDWARE_LTDC_INITIALIZE();	// подключение к выводам процессора сигналов периферийного контроллера

	/* Enable Pixel Clock --------------------------------------------------------*/

	/* Configure PLLSAI prescalers for LCD */
	/* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
	/* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAI_N = 192 Mhz */
	/* PLLLCDCLK = PLLSAI_VCO Output/PLLSAI_R = 192/3 = 64 Mhz */
	/* LTDC clock frequency = PLLLCDCLK / RCC_PLLSAIDivR = 64/8 = 8 Mhz */
	//RCC_PLLSAIConfig(192, 7, 3);
	//RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div8);

	/* Enable PLLSAI Clock */
	//RCC_PLLSAICmd(ENABLE);
	/* Wait for PLLSAI activation */
	//while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET)
	//{
	//}


	/* LTDC Initialization -------------------------------------------------------*/
	LTDC_InitTypeDef LTDC_InitStruct;

#if LCDMODE_LQ043T3DX02K
	// Sony PSP-1000 display panel
	// LQ043T3DX02K panel (272*480)
	// RK043FN48H-CT672B  panel (272*480) - плата STM32F746G-DISCO
	/** 
	  * @brief  RK043FN48H Size  
	  */    
	enum
	{

		WIDTH = 480,				/* LCD PIXEL WIDTH            */
		HEIGHT = 272,			/* LCD PIXEL HEIGHT           */

		/** 
		  * @brief  RK043FN48H Timing  
		  */     
		HSYNC = 41,				/* Horizontal synchronization */
		HBP = 2,				/* Horizontal back porch      */
		HFP = 2,				/* Horizontal front porch     */

		VSYNC = 10,				/* Vertical synchronization   */
		VBP = 2,					/* Vertical back porch        */
		VFP = 2,					/* Vertical front porch       */
	};
#elif LCDMODE_ILI8961
	// HHT270C-8961-6A6 (320*240)
	enum
	{

		WIDTH = 320 * 3,				/* LCD PIXEL WIDTH            */
		HEIGHT = 240,			/* LCD PIXEL HEIGHT           */

		/** 
		  * @brief  RK043FN48H Timing  
		  */     
		HSYNC = 1,				/* Horizontal synchronization */
		HBP = 2,				/* Horizontal back porch      */
		HFP = 2,				/* Horizontal front porch     */

		VSYNC = 1,				/* Vertical synchronization   */
		VBP = 2,					/* Vertical back porch        */
		VFP = 2,					/* Vertical front porch       */
	};
#elif LCDMODE_ILI9341
	// SF-TC240T-9370-T (320*240)
	enum
	{

		WIDTH = 240,				/* LCD PIXEL WIDTH            */
		HEIGHT = 320,			/* LCD PIXEL HEIGHT           */

		/** 
		  * @brief  RK043FN48H Timing  
		  */     
		HSYNC = 10,				/* Horizontal synchronization */
		HBP = 20,				/* Horizontal back porch      */
		HFP = 10,				/* Horizontal front porch     */

		VSYNC = 2,				/* Vertical synchronization   */
		VBP = 4,					/* Vertical back porch        */
		VFP = 4,					/* Vertical front porch       */
	};
#else
	#error Unsupported LCDMODE_xxx
#endif

	LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;     
	//LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AH;     
	/* Initialize the vertical synchronization polarity as active low */  
	LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;     
	//LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AH;     
	/* Initialize the data enable polarity as active low */ 
	//LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AH;		// While VSYNC is low, do not change DISP signal "Low" or "High"
	LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;		// While VSYNC is low, do not change DISP signal "Low" or "High"
	/* Initialize the pixel clock polarity as input pixel clock */ 
	LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;

	/* Timing configuration */
	/* Configure horizontal synchronization width */     
	LTDC_InitStruct.LTDC_HorizontalSync = (HSYNC - 1);
	/* Configure accumulated horizontal back porch */
	LTDC_InitStruct.LTDC_AccumulatedHBP = (HSYNC + HBP - 1);
	/* Configure accumulated active width */  
	LTDC_InitStruct.LTDC_AccumulatedActiveW = (WIDTH + HSYNC + HBP - 1);
	/* Configure total width */
	LTDC_InitStruct.LTDC_TotalWidth = (WIDTH + HSYNC + HBP + HFP - 1);

	/* Configure vertical synchronization height */
	LTDC_InitStruct.LTDC_VerticalSync = (VSYNC - 1);
	/* Configure accumulated vertical back porch */
	LTDC_InitStruct.LTDC_AccumulatedVBP = (VSYNC + VBP - 1);
	/* Configure accumulated active height */
	LTDC_InitStruct.LTDC_AccumulatedActiveH = (HEIGHT + VSYNC + VBP - 1);
	/* Configure total height */
	LTDC_InitStruct.LTDC_TotalHeigh = (HEIGHT + VSYNC + VBP + VFP - 1);

	LTDC_Init(&LTDC_InitStruct);

	/* Configure R,G,B component values for LCD background color */                   
	LTDC_InitStruct.LTDC_BackgroundRedValue = 0;            
	LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;          
	LTDC_InitStruct.LTDC_BackgroundBlueValue = 0; 

	LTDC_Init(&LTDC_InitStruct);


	/* LTDC initialization end ---------------------------------------------------*/

	LCD_LayerInit(HSYNC + HBP, VSYNC + VBP);

	LTDC->SRCR = LTDC_SRCR_IMR;	/*!< Immediately Reload. */


	/* LCD Log initialization */
	//LCD_LOG_Init(); 

	LTDC_Layer1->CFBLR = 
		((rowsize << LTDC_LxCFBLR_CFBP_Pos) & LTDC_LxCFBLR_CFBP) | 
		(((rowsize + 3) << LTDC_LxCFBLR_CFBLL_Pos) & LTDC_LxCFBLR_CFBLL) |
		0;
	LTDC_Layer1->CFBAR = (uintptr_t) & framebuff;
	//LTDC_Layer2->CFBAR = (uintptr_t) & framebuff;

	/* Enable the LTDC */
	LTDC->GCR |= LTDC_GCR_LTDCEN;

#if LCDMODE_LTDC_L24
	fillLUT_L24(LTDC_Layer1);	// пр€ма€ трансл€ци€ всех байтов из пам€ти на выход. загрузка палитры - имеет смысл до Reload
#elif LCDMODE_LTDC_L8
	fillLUT_L8(LTDC_Layer1);	// загрузка палитры - имеет смысл до Reload
#endif /* LCDMODE_LTDC_L8 */

	/* LTDC reload configuration */  
	LTDC->SRCR = LTDC_SRCR_IMR;	/*!< Immediately Reload. */

	// While УVSYNCФ is УLowФ, donТt change УDISPФ signal УLowФ to УHighФ. 
	HARDWARE_LTDC_SET_DISP(0);
	local_delay_ms(20);
	HARDWARE_LTDC_SET_DISP(1);

	debug_printf_P(PSTR("arm_hardware_ltdc_initialize done\n"));
}

#endif /* CPUSTYLE_STM32F && LCDMODE_LTDC */
