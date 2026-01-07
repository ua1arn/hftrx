#include "g2d.h"
#include "fnt0.h"
#include "fnt1.h"
#include "txtX.h"
#include "src/display/display.h"
#include <stdio.h>

TXTX_TText  text1;
volatile uint32_t g2dRotReady = 0;
static uint32_t  __ALIGNED(128) layerUI0[GXSIZE(DIM_X, DIM_Y)] = {0x00000000};
//uint32_t  __ALIGNED(128) layerUI1[GXSIZE(DIM_X, DIM_Y)] = {0x00000000};
//TG2D_ROT_Params g2dRotCfg;

void G2D_Callback(uint32_t sts){

  g2dRotReady = 1;
}

void rcq_demo(void)
{
	  //  //LCD
//	  LVDS0_Setup((LCDParams*)&LCD_480x1280, LCD_SRC_DE);
//	  DE_Setup(LCD_480x1280.Width, LCD_480x1280.Height);
//	  DE_SetMixer_Ovl_V_Color(DE_MIX0, DE_LAY0, 0xFF336699);
//	  DE_MixerUpdateBlocking(DE_MIX0);
//
//
//	  G2D_Setup();

	arm_hardware_set_handler_system(G2D_IRQn, G2D_IRQHandler);

	  text1.TextLength = snprintf(text1.Text, sizeof(text1.Text), "Hello!1");
	  text1.Font = (FNTX_TFontASCII*) &font_001;
	  text1.Height = 47;
	  text1.Width = 400;
	  text1.FontColor = 0xFFFFFFFF;
	  text1.Justify = TXTX_CENTER;
	  text1.OffsetX = 200;
	  text1.OffsetY = 100;
	  text1.Interval = 4;
	  text1.Canvas = (uint32_t*)__va_to_pa((uint32_t) &layerUI0[0]);
	  text1.CanvasWidth = DIM_X;
	  text1.CanvasHeight = DIM_Y;

	  TXTX_Update(&text1, &G2D_RCQTask);
	  dcache_clean((uintptr_t) &text1, sizeof(TXTX_TText));
	  //DCacheCleanAddr(&G2D_RCQTask, sizeof(TG2D_RCQTask));

	  g2dRotReady = 0;
	  G2D_Rcq_RunTask(&G2D_RCQTask);
	  while (!g2dRotReady);

	  text1.TextLength = snprintf(text1.Text, sizeof(text1.Text), "Hello!2");
	  text1.FontColor = 0xFFFFFF00;
	  text1.Justify = TXTX_LEFT;
	  text1.OffsetY = 200;
	  TXTX_Update(&text1, &G2D_RCQTask);
	  dcache_clean((uintptr_t) &text1, sizeof(TXTX_TText));

	  g2dRotReady = 0;
	  G2D_Rcq_RunTask(&G2D_RCQTask);
	  while (!g2dRotReady);


	  text1.TextLength = snprintf(text1.Text, sizeof(text1.Text), "Hello!3");
	  text1.FontColor = 0xFF00FFFF;
	  text1.Justify = TXTX_RIGHT;
	  text1.OffsetY = 300;
	  TXTX_Update(&text1, &G2D_RCQTask);
	  dcache_clean((uintptr_t) &text1, sizeof(TXTX_TText));

	  g2dRotReady = 0;
	  G2D_Rcq_RunTask(&G2D_RCQTask);
	  while (!g2dRotReady)
		  ;



//	  g2dRotCfg.Width = 1280;
//	  g2dRotCfg.Height = 600;
//	  g2dRotCfg.srcAddr = (uint32_t) &layerUI0[0];
//	  g2dRotCfg.dstAddr = (uint32_t) &layerUI1[0];
//	  g2dRotCfg.Angle = G2D_ROT_270;
//	  g2dRotCfg.Flip = G2D_ROT_FLIP_NONE;
//
//	  g2dRotReady = 0;
//	  G2D_ROT_Copy_INT(&g2dRotCfg);
//	  while (!g2dRotReady);
//
//	  //DCacheCleanAddr(&layerUI0[0], sizeof(layerUI0));
//	  DE_SetMixerOvl_UI1_Data(DE_MIX0, DE_LAY0, (uint32_t*) &layerUI1[0]);
//	  DE_MixerUpdateBlocking(DE_MIX0);
	  	  dcache_clean((uintptr_t)layerUI0, sizeof layerUI0);
		hardware_ltdc_main_set4(RTMIXIDLCD, (uintptr_t) layerUI0, (uintptr_t) 0, 1*(uintptr_t) 0, 1*(uintptr_t) 0);

		arm_hardware_disable_handler(G2D_IRQn);

}

