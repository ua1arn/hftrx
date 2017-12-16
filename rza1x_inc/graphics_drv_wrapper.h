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
* @file         graphics_drv_wrapper.h
* @version      0.10
* @brief        Graphics driver wrapper function header
******************************************************************************/

#ifndef GRAPHICS_DRV_WRAPPER_H
#define GRAPHICS_DRV_WRAPPER_H

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

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
vdc5_error_t GrpDrv_Init(vdc5_channel_t ch, vdc5_onoff_t vd_in_0, vdc5_onoff_t vd_in_1);
vdc5_error_t GrpDrv_Term(vdc5_channel_t ch);
vdc5_error_t GrpDrv_GraphicsCreateSurface(
    vdc5_channel_t              ch,
    vdc5_layer_id_t             layer_id,
    void                      * framebuff,
    uint32_t                    fb_stride,
    vdc5_gr_format_t            gr_format,
    const vdc5_period_rect_t  * period_rect);
vdc5_error_t GrpDrv_VideoCreateSurface(
    vdc5_channel_t              ch,
    vdc5_layer_id_t             layer_id,
    void                      * framebuff,
    uint32_t                    fb_stride,
    vdc5_res_md_t               res_md,
    const vdc5_period_rect_t  * res,
    const vdc5_period_rect_t  * period_rect);
vdc5_error_t GrpDrv_DestroySurfaces(vdc5_channel_t ch);
vdc5_error_t GrpDrv_StartSurfaces(vdc5_channel_t ch, vdc5_gr_disp_sel_t * gr_disp_sel);
vdc5_error_t GrpDrv_StopSurfaces(vdc5_channel_t ch);


#endif  /* GRAPHICS_DRV_WRAPPER_H */
