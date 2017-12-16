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
*
* Copyright (C) 2013 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/******************************************************************************
* File Name    : devdrv_riic.h
* Description  : RIIC driver header
******************************************************************************/
#ifndef DEVDRV_RIIC_H
#define DEVDRV_RIIC_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/


/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/
#define RIIC_CKS_DIVISION_1     (0)
#define RIIC_CKS_DIVISION_2     (1)
#define RIIC_CKS_DIVISION_4     (2)
#define RIIC_CKS_DIVISION_8     (3)
#define RIIC_CKS_DIVISION_16    (4)
#define RIIC_CKS_DIVISION_32    (5)
#define RIIC_CKS_DIVISION_64    (6)
#define RIIC_CKS_DIVISION_128   (7)

#define RIIC_TX_MODE_START      (0)
#define RIIC_TX_MODE_RESTART    (1)
#define RIIC_TX_MODE_STOP       (2)

#define RIIC_TEND_WAIT_TRANSMIT (0)
#define RIIC_TEND_WAIT_RECEIVE  (1)

#define RIIC_BUS_MASTERSHIP_WAIT_FREE   (0)
#define RIIC_BUS_MASTERSHIP_WAIT_BUSY   (1)

/******************************************************************************
Variable Externs
******************************************************************************/


/******************************************************************************
Functions Prototypes
******************************************************************************/
int32_t R_RIIC_Init(uint32_t channel, uint32_t cks, uint32_t brl, uint32_t brh);
int32_t R_RIIC_WriteCond(uint32_t channel, uint32_t mode);
int32_t R_RIIC_SingleWrite(uint32_t channel, uint8_t data, uint32_t mode);
int32_t R_RIIC_Write(uint32_t channel, const uint8_t * buffer, uint32_t byte);
int32_t R_RIIC_SingleRead(uint32_t channel, uint8_t * data);
int32_t R_RIIC_Read(uint32_t channel, uint8_t * buffer, uint32_t byte);
int32_t R_RIIC_ReadDummy(uint32_t channel);
int32_t R_RIIC_DetectStop(uint32_t channel);
int32_t R_RIIC_ClearNack(uint32_t channel);
int32_t R_RIIC_RiInterrupt(uint32_t channel);
int32_t R_RIIC_TiInterrupt(uint32_t channel);
int32_t R_RIIC_TeiInterrupt(uint32_t channel);

void Userdef_RIIC0_Init(uint32_t cks, uint32_t brl, uint32_t brh);
void Userdef_RIIC1_Init(uint32_t cks, uint32_t brl, uint32_t brh);
void Userdef_RIIC2_Init(uint32_t cks, uint32_t brl, uint32_t brh);
void Userdef_RIIC3_Init(uint32_t cks, uint32_t brl, uint32_t brh);
void Userdef_RIIC0_InitReceiveFull(void);
void Userdef_RIIC1_InitReceiveFull(void);
void Userdef_RIIC2_InitReceiveFull(void);
void Userdef_RIIC3_InitReceiveFull(void);
void Userdef_RIIC0_InitTransmitEmpty(void);
void Userdef_RIIC1_InitTransmitEmpty(void);
void Userdef_RIIC2_InitTransmitEmpty(void);
void Userdef_RIIC3_InitTransmitEmpty(void);
void Userdef_RIIC0_InitTransmitEnd(void);
void Userdef_RIIC1_InitTransmitEnd(void);
void Userdef_RIIC2_InitTransmitEnd(void);
void Userdef_RIIC3_InitTransmitEnd(void);
void Userdef_RIIC0_SetReceiveFull(void);
void Userdef_RIIC1_SetReceiveFull(void);
void Userdef_RIIC2_SetReceiveFull(void);
void Userdef_RIIC3_SetReceiveFull(void);
void Userdef_RIIC0_SetTransmitEmpty(void);
void Userdef_RIIC1_SetTransmitEmpty(void);
void Userdef_RIIC2_SetTransmitEmpty(void);
void Userdef_RIIC3_SetTransmitEmpty(void);
void Userdef_RIIC0_SetTransmitEnd(void);
void Userdef_RIIC1_SetTransmitEnd(void);
void Userdef_RIIC2_SetTransmitEnd(void);
void Userdef_RIIC3_SetTransmitEnd(void);
void Userdef_RIIC0_WaitReceiveFull(void);
void Userdef_RIIC1_WaitReceiveFull(void);
void Userdef_RIIC2_WaitReceiveFull(void);
void Userdef_RIIC3_WaitReceiveFull(void);
void Userdef_RIIC0_WaitTransmitEmpty(void);
void Userdef_RIIC1_WaitTransmitEmpty(void);
void Userdef_RIIC2_WaitTransmitEmpty(void);
void Userdef_RIIC3_WaitTransmitEmpty(void);
void Userdef_RIIC0_WaitTransmitEnd(uint32_t mode);
void Userdef_RIIC1_WaitTransmitEnd(uint32_t mode);
void Userdef_RIIC2_WaitTransmitEnd(uint32_t mode);
void Userdef_RIIC3_WaitTransmitEnd(uint32_t mode);
void Userdef_RIIC0_WaitBusMastership(uint32_t mode);
void Userdef_RIIC1_WaitBusMastership(uint32_t mode);
void Userdef_RIIC2_WaitBusMastership(uint32_t mode);
void Userdef_RIIC3_WaitBusMastership(uint32_t mode);
void Userdef_RIIC0_WaitStop(void);
void Userdef_RIIC1_WaitStop(void);
void Userdef_RIIC2_WaitStop(void);
void Userdef_RIIC3_WaitStop(void);


#endif  /* DEVDRV_RIIC_H */

/* End of File */
