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
* http://www.renesas.com/disclaimer*
* Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name : mcu_board_select.h
* $Rev: 29 $
* $Date:: 2016-12-20 17:40:26 +0900#$
* Description : Compile switch for mcu select
******************************************************************************/

#ifndef MCU_SEL_H
#define MCU_SEL_H

/* MCU Lineup */
#define TARGET_RZA1LC           (0x00200000)
#define TARGET_RZA1L            (0x00300000)
#define TARGET_RZA1LU           (0x00300010)
#define TARGET_RZA1M            (0x00500000)
#define TARGET_RZA1H            (0x00A00000)

/* Board Lineup */
#define TARGET_BOARD_RSK        (0x000A0002)
#define TARGET_BOARD_STREAM_IT2 (0x00030002)

/* User Select macro */
//#define TARGET_RZA1             (TARGET_RZA1LU)
//#define TARGET_BOARD            (TARGET_BOARD_STREAM_IT2)
//#define TARGET_RZA1             (TARGET_RZA1L)	// перенесено в Makefile

#endif /* MCU_SEL_H */
