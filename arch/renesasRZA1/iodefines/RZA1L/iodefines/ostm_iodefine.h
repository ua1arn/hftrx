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
* Copyright (C) 2013-2017 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/*******************************************************************************
* File Name : ostm_iodefine.h
* $Rev: 29 $
* $Date:: 2016-12-20 17:40:26 +0900#$
* Description : Definition of I/O Register for RZ/A1L (V2.00l)
******************************************************************************/
#ifndef OSTM_IODEFINE_H
#define OSTM_IODEFINE_H
/* ->QAC 0639 : Over 127 members (C90) */
/* ->QAC 0857 : Over 1024 #define (C90) */
/* ->MISRA 18.4 : Pack unpack union */ /* ->SEC M1.6.2 */
/* ->SEC M1.10.1 : Not magic number */

#define OSTM0   (*(struct st_ostm    *)0xFCFEC000uL) /* OSTM0 */
#define OSTM1   (*(struct st_ostm    *)0xFCFEC400uL) /* OSTM1 */


/* Start of channel array defines of OSTM */

/* Channel array defines of OSTM */
/*(Sample) value = OSTM[ channel ]->OSTMnCMP; */
#define OSTM_COUNT  (2)
#define OSTM_ADDRESS_LIST \
{   /* ->MISRA 11.3 */ /* ->SEC R2.7.1 */ \
    &OSTM0, &OSTM1 \
}   /* <-MISRA 11.3 */ /* <-SEC R2.7.1 */ /* { } is for MISRA 19.4 */

/* End of channel array defines of OSTM */


#define OSTM0CMP (OSTM0.OSTMnCMP)
#define OSTM0CNT (OSTM0.OSTMnCNT)
#define OSTM0TE (OSTM0.OSTMnTE)
#define OSTM0TS (OSTM0.OSTMnTS)
#define OSTM0TT (OSTM0.OSTMnTT)
#define OSTM0CTL (OSTM0.OSTMnCTL)
#define OSTM1CMP (OSTM1.OSTMnCMP)
#define OSTM1CNT (OSTM1.OSTMnCNT)
#define OSTM1TE (OSTM1.OSTMnTE)
#define OSTM1TS (OSTM1.OSTMnTS)
#define OSTM1TT (OSTM1.OSTMnTT)
#define OSTM1CTL (OSTM1.OSTMnCTL)


typedef struct st_ostm
{
                                                           /* OSTM             */
    volatile uint32_t  OSTMnCMP;                               /*  OSTMnCMP        */
    volatile uint32_t  OSTMnCNT;                               /*  OSTMnCNT        */
    volatile uint8_t   dummy1[8];                              /*                  */
    volatile uint8_t   OSTMnTE;                                /*  OSTMnTE         */
    volatile uint8_t   dummy2[3];                              /*                  */
    volatile uint8_t   OSTMnTS;                                /*  OSTMnTS         */
    volatile uint8_t   dummy3[3];                              /*                  */
    volatile uint8_t   OSTMnTT;                                /*  OSTMnTT         */
    volatile uint8_t   dummy4[7];                              /*                  */
    volatile uint8_t   OSTMnCTL;                               /*  OSTMnCTL        */
} r_io_ostm_t;


/* Channel array defines of OSTM (2)*/
#ifdef  DECLARE_OSTM_CHANNELS
volatile struct st_ostm*  OSTM[ OSTM_COUNT ] =
    /* ->MISRA 11.3 */ /* ->SEC R2.7.1 */
    OSTM_ADDRESS_LIST;
    /* <-MISRA 11.3 */ /* <-SEC R2.7.1 */
#endif  /* DECLARE_OSTM_CHANNELS */
/* End of channel array defines of OSTM (2)*/


/* <-SEC M1.10.1 */
/* <-MISRA 18.4 */ /* <-SEC M1.6.2 */
/* <-QAC 0857 */
/* <-QAC 0639 */
#endif
