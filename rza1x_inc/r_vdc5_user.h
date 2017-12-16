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
* @file         r_vdc5_user.h
* @version      0.10
* @brief        VDC5 driver compilation option
******************************************************************************/

#ifndef R_VDC5_USER_H
#define R_VDC5_USER_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    "r_typedefs.h"
#include    "iodefine.h"


/******************************************************************************
Macro definitions
******************************************************************************/
/*! Compilation switch to enable parameter check functions<br>
    If this definition is valid, the parameter check in each API function will be enabled. */
#define R_VDC5_CHECK_PARAMETERS


/*! GBR to YCbCr conversion <br>
 *  Matrix coefficient (typical value) for SMPTE 293M
 *  - Y  =  0.299 * R + 0.587 * G + 0.114 * B
 *  - Cb = -0.169 * R - 0.331 * G + 0.500 * B
 *  - Cr =  0.500 * R - 0.419 * G - 0.081 * B
 *  @note
 *      Coefficient values are represented in 11-bit two's complement integer
 *      - DEFINITION = int(COEF * 256 + 0.5)
 */
typedef enum
{
    VDC5_COLORCONV_Y_R  = (77u),
    VDC5_COLORCONV_Y_G  = (150u),
    VDC5_COLORCONV_Y_B  = (29u),
    VDC5_COLORCONV_CB_R = (2005u),
    VDC5_COLORCONV_CB_G = (1963u),
    VDC5_COLORCONV_CB_B = (128u),
    VDC5_COLORCONV_CR_R = (128u),
    VDC5_COLORCONV_CR_G = (1941u),
    VDC5_COLORCONV_CR_B = (2027u)
} vdc5_colcnv_rgb_ycbcr_t;

/*! YCbCr to GBR conversion <br>
 *  Matrix coefficient (typical value) for SMPTE 293M
 *  - G = 1.0 * Y - 0.344 * Cb - 0.714 * Cr
 *  - B = 1.0 * Y + 1.772 * Cb +   0.0 * Cr
 *  - R = 1.0 * Y +   0.0 * Cb + 1.402 * Cr
 *  @note
 *      Coefficient values are represented in 11-bit two's complement integer
 *      - DEFINITION = int(COEF * 256 + 0.5)
 */
typedef enum
{
    VDC5_COLORCONV_G_Y  = (256u),
    VDC5_COLORCONV_G_CB = (1960u),
    VDC5_COLORCONV_G_CR = (1865u),
    VDC5_COLORCONV_B_Y  = (256u),
    VDC5_COLORCONV_B_CB = (454u),
    VDC5_COLORCONV_B_CR = (0u),
    VDC5_COLORCONV_R_Y  = (256u),
    VDC5_COLORCONV_R_CB = (0u),
    VDC5_COLORCONV_R_CR = (359u)
} vdc5_colcnv_ycbcr_rgb_t;

/*! Color matrix DC offset typical value */
#define     VDC5_COLORCONV_DC_OFFSET        (128u)
/*! Color matrix coefficient value that represents one times */
#define     VDC5_COLORCONV_1TIMES_GAIN      (256u)

/*! 1usec wait cycle for LVDS PLL */
#define     VDC5_LVDS_PLL_WAIT_CYCLE        (103u)
/*! Retry counter for checking LVDS PLL lock */
#define     VDC5_LVDS_PLL_LOCK_RETRY_CNT    (10u)


#endif  /* R_VDC5_USER_H */

