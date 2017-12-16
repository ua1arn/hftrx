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
* @file         video_decoder.h
* @version      0.10
* @brief        Video decoder function header
******************************************************************************/

#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <stdlib.h>
#include    <string.h>
#include    <stdio.h>

#include    "r_vdec.h"
#include    "r_vdec_user.h"


/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/
/*! Video color system */
typedef enum
{
    GRPH_COL_SYS_UNKNOWN    = -1,       /*!< Unknown */
    GRPH_COL_SYS_NTSC_358   = 0,        /*!< NTSC-3.58 */
    GRPH_COL_SYS_NTSC_443   = 1,        /*!< NTSC-4.43 */
    GRPH_COL_SYS_PAL_443    = 2,        /*!< PAL-4.43 */
    GRPH_COL_SYS_PAL_M      = 3,        /*!< PAL-M */
    GRPH_COL_SYS_PAL_N      = 4,        /*!< PAL-N */
    GRPH_COL_SYS_SECAM      = 5,        /*!< SECAM */
    GRPH_COL_SYS_NUM        = 6
} graphics_col_sys_t;


/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
graphics_col_sys_t Graphics_VideoDecoderInit(vdec_adc_vinsel_t adc_vinsel, vdec_channel_t channel);


#endif  /* VIDEO_DECODER_H */
