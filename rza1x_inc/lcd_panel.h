/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file         lcd_panel.h
* @version      0.10
* @brief        LCD panel definition header
******************************************************************************/

#ifndef LCD_PANEL_H
#define LCD_PANEL_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <stdlib.h>
#include    <stdio.h>

#include    "r_typedefs.h"

#include    "r_vdc5.h"


/******************************************************************************
Macro definitions
******************************************************************************/
/* LCD panel select (CH0)
    1: Analog RGB D-sub15, SVGA 800x600
    2: LCD-KIT-B01, WVGA 800x480
  The channel 0 LCD panel requires the control of port 10 and port 11.
  Port 10 and port 11 are implemented only in 324 pin PKG.
*/
#define     LCD_CH0_PANEL_ANALOG_RGB            (1)
#define     LCD_CH0_PANEL_LCD_KIT_B01           (2)
#define     LCD_CH0_PANEL_LCD_KIT_B02           (3)
#define     LCD_CH0_PANEL_LCD_HDMI              (4)

#define     LCD_VDC5_CH0_PANEL                  LCD_CH0_PANEL_LCD_HDMI
//#define     LCD_VDC5_CH0_PANEL                  LCD_CH0_PANEL_LCD_KIT_B02


/* LCD panel select (CH1)
    1: Analog RGB D-sub15, SVGA 800x600
    2: LCD-KIT-B01, WVGA 800x480
    3: R0P7724LE0011RL, WVGA 800x480
*/
#define     LCD_CH1_PANEL_ANALOG_RGB            (1)
#define     LCD_CH1_PANEL_LCD_KIT_B01           (2)
#define     LCD_CH1_PANEL_R0P7724LE0011RL       (3)

#define     LCD_VDC5_CH1_PANEL                  LCD_CH1_PANEL_R0P7724LE0011RL


#if     (LCD_VDC5_CH0_PANEL==1)
    #include    "lcd/lcd_analog_rgb_ch0.h"
#elif   (LCD_VDC5_CH0_PANEL==2)
    #include    "lcd/lcd_lcd_kit_b01_ch0.h"
#elif   (LCD_VDC5_CH0_PANEL==3)
    #include    "lcd/lcd_lcd_kit_b02_ch0.h"
#elif   (LCD_VDC5_CH0_PANEL==4)
    #include    "lcd/lcd_hdmi_ch0.h"
#else
    #error  Undefined LCD panel!
#endif

#if     (LCD_VDC5_CH1_PANEL==1)
    #include    "lcd/lcd_analog_rgb_ch1.h"
#elif   (LCD_VDC5_CH1_PANEL==2)
    #include    "lcd/lcd_lcd_kit_b01_ch1.h"
#elif   (LCD_VDC5_CH1_PANEL==3)
    #include    "lcd/lcd_r0p7724le0011rl_ch1.h"
#else
    #error  Undefined LCD panel!
#endif


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
void Graphics_SetLcdPanel(vdc5_channel_t channel);
void Graphics_SetLcdTconSettings(vdc5_channel_t channel, const vdc5_lcd_tcon_timing_t * * outctrl);
vdc5_lvds_t * Graphics_GetLvdsParam(vdc5_channel_t channel);


#endif  /* LCD_PANEL_H */
