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
* @file         lcd_analog_rgb.h
* @version      0.07
* @brief        LCD panel  definition header
******************************************************************************/

#ifndef LCD_ANALOG_RGB_H
#define LCD_ANALOG_RGB_H

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
/* SVGA signal 800x600
    Pixel clock frequency: 40.0 MHz
    Refresh rate: 60.3 Hz
    Polarity of horizontal sync pulse: Positive
    Polarity of vertical sync pulse: Positive
*/
#define     LCD_SVGA_H_VISIBLE_AREA     (800u)                      /* Horizontal visible area [pixel] */
#define     LCD_SVGA_H_FRONT_PORCH      (40u)                       /* Horizontal front porch [pixel] */
#define     LCD_SVGA_H_SYNC_WIDTH       (128u)                      /* Horizontal sync pulse width [pixel] */
#define     LCD_SVGA_H_BACK_PORCH       (88u)                       /* Horizontal back porch [pixel] */
                                                                    /* Horizontal total (one line) [pixel] */
#define     LCD_SVGA_H_TOTAL            (LCD_SVGA_H_VISIBLE_AREA +\
                                        LCD_SVGA_H_FRONT_PORCH +\
                                        LCD_SVGA_H_SYNC_WIDTH +\
                                        LCD_SVGA_H_BACK_PORCH)
#define     LCD_SVGA_H_POLARITY         (VDC5_SIG_POL_NOT_INVERTED) /* Polarity of horizontal sync pulse */

#define     LCD_SVGA_V_VISIBLE_AREA     (600u)                      /* Vertical visible area [line] */
#define     LCD_SVGA_V_FRONT_PORCH      (1u)                        /* Vertical front porch [line] */
#define     LCD_SVGA_V_SYNC_WIDTH       (4u)                        /* Vertical sync pulse width [line] */
#define     LCD_SVGA_V_BACK_PORCH       (23u)                       /* Vertical back porch [line] */
                                                                    /* Vertical total (one frame) [line] */
#define     LCD_SVGA_V_TOTAL            (LCD_SVGA_V_VISIBLE_AREA +\
                                        LCD_SVGA_V_FRONT_PORCH +\
                                        LCD_SVGA_V_SYNC_WIDTH +\
                                        LCD_SVGA_V_BACK_PORCH)
#define     LCD_SVGA_V_POLARITY         (VDC5_SIG_POL_NOT_INVERTED) /* Polarity of vertical sync pulse */

/* XGA signal 1024x768
    Pixel clock frequency: 65.0 MHz
    Refresh rate: 60.0 Hz
    Polarity of horizontal sync pulse: Negative
    Polarity of vertical sync pulse: Negative
*/
#define     LCD_XGA_H_VISIBLE_AREA      (1024u)                     /* Horizontal visible area [pixel] */
#define     LCD_XGA_H_FRONT_PORCH       (24u)                       /* Horizontal front porch [pixel] */
#define     LCD_XGA_H_SYNC_WIDTH        (136u)                      /* Horizontal sync pulse width [pixel] */
#define     LCD_XGA_H_BACK_PORCH        (160u)                      /* Horizontal back porch [pixel] */
                                                                    /* Horizontal total (one line) [pixel] */
#define     LCD_XGA_H_TOTAL             (LCD_XGA_H_VISIBLE_AREA +\
                                        LCD_XGA_H_FRONT_PORCH +\
                                        LCD_XGA_H_SYNC_WIDTH +\
                                        LCD_XGA_H_BACK_PORCH)
#define     LCD_XGA_H_POLARITY          (VDC5_SIG_POL_INVERTED)     /* Polarity of horizontal sync pulse */

#define     LCD_XGA_V_VISIBLE_AREA      (768u)                      /* Vertical visible area [line] */
#define     LCD_XGA_V_FRONT_PORCH       (3u)                        /* Vertical front porch [line] */
#define     LCD_XGA_V_SYNC_WIDTH        (6u)                        /* Vertical sync pulse width [line] */
#define     LCD_XGA_V_BACK_PORCH        (29u)                       /* Vertical back porch [line] */
                                                                    /* Vertical total (one frame) [line] */
#define     LCD_XGA_V_TOTAL             (LCD_XGA_V_VISIBLE_AREA +\
                                        LCD_XGA_V_FRONT_PORCH +\
                                        LCD_XGA_V_SYNC_WIDTH +\
                                        LCD_XGA_V_BACK_PORCH)
#define     LCD_XGA_V_POLARITY          (VDC5_SIG_POL_INVERTED)     /* Polarity of vertical sync pulse */


/* VGA signal 640x480
    Pixel clock frequency: 25.175 MHz
    Refresh rate: 59.94 Hz
    Polarity of horizontal sync pulse: Negative
    Polarity of vertical sync pulse: Negative
*/
#define     LCD_VGA_H_VISIBLE_AREA      (640u)                      /* Horizontal visible area [pixel] */
#define     LCD_VGA_H_FRONT_PORCH       (16u)                       /* Horizontal front porch [pixel] */
#define     LCD_VGA_H_SYNC_WIDTH        (96u)                       /* Horizontal sync pulse width [pixel] */
#define     LCD_VGA_H_BACK_PORCH        (48u)                       /* Horizontal back porch [pixel] */
                                                                    /* Horizontal total (one line) [pixel] */
#define     LCD_VGA_H_TOTAL             (LCD_VGA_H_VISIBLE_AREA +\
                                        LCD_VGA_H_FRONT_PORCH +\
                                        LCD_VGA_H_SYNC_WIDTH +\
                                        LCD_VGA_H_BACK_PORCH)
#define     LCD_VGA_H_POLARITY          (VDC5_SIG_POL_INVERTED)     /* Polarity of horizontal sync pulse */

#define     LCD_VGA_V_VISIBLE_AREA      (480u)                      /* Vertical visible area [line] */
#define     LCD_VGA_V_FRONT_PORCH       (10u)                       /* Vertical front porch [line] */
#define     LCD_VGA_V_SYNC_WIDTH        (2u)                        /* Vertical sync pulse width [line] */
#define     LCD_VGA_V_BACK_PORCH        (33u)                       /* Vertical back porch [line] */
                                                                    /* Vertical total (one frame) [line] */
#define     LCD_VGA_V_TOTAL             (LCD_VGA_V_VISIBLE_AREA +\
                                        LCD_VGA_V_FRONT_PORCH +\
                                        LCD_VGA_V_SYNC_WIDTH +\
                                        LCD_VGA_V_BACK_PORCH)
#define     LCD_VGA_V_POLARITY          (VDC5_SIG_POL_INVERTED)     /* Polarity of vertical sync pulse */


#endif  /* LCD_ANALOG_RGB_H */
