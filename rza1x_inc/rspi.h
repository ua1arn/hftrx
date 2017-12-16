/******************************************************************************
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
******************************************************************************/
/******************************************************************************
* File Name    : r_cg_rspi.h
* Device(s)    : R7S721001
* Tool-Chain   : GNUARM-RZv13.01-EABI
* Description  : This file implements device driver for RSPI module.
*******************************************************************************/
/*******************************************************************************
* History       : DD.MM.YYYY Version Description
*               : 18.06.2013 1.00
*******************************************************************************/

/* Multiple inclusion prevention macro */
#ifndef RSPI_H
#define RSPI_H

/***********************************************************************************************************************
Macro definitions (Register bit)
***********************************************************************************************************************/
/*
    RSPI Control Register (SPCR)
*/
/* RSPI Mode Select (SPMS) */
#define _00_RSPI_MODE_SPI                       (0x00U) /* SPI operation (four-wire method) */
#define _01_RSPI_MODE_CLOCK_SYNCHRONOUS         (0x01U) /* Clock synchronous operation (three-wire method) */
/* Communications Operating Mode Select (TXMD) */
#define _00_RSPI_FULL_DUPLEX_SYNCHRONOUS        (0x00U) /* Full-duplex synchronous serial communications */
#define _02_RSPI_TRANSMIT_ONLY                  (0x02U) /* Serial communications with transmit only operations */
/* Mode Fault Error Detection Enable (MODFEN) */
#define _00_RSPI_MODE_FAULT_DETECT_DISABLED     (0x00U) /* Disables the detection of mode fault error */
#define _04_RSPI_MODE_FAULT_DETECT_ENABLED      (0x04U) /* Enables the detection of mode fault error */
/* RSPI Master/Slave Mode Select (MSTR) */
#define _00_RSPI_SLAVE_MODE                     (0x00U) /* Slave mode */
#define _08_RSPI_MASTER_MODE                    (0x08U) /* Master mode */
/* RSPI Error Interrupt Enable (SPEIE) */
#define _00_RSPI_ERROR_INTERRUPT_DISABLED       (0x00U) /* Disables the generation of RSPI error interrupt */
#define _10_RSPI_ERROR_INTERRUPT_ENABLED        (0x10U) /* Enables the generation of RSPI error interrupt */
/* RSPI Transmit Interrupt Enable (SPTIE) */
#define _00_RSPI_TRANSMIT_INTERRUPT_DISABLED    (0x00U) /* Disables the generation of RSPI transmit interrupt */
#define _20_RSPI_TRANSMIT_INTERRUPT_ENABLED     (0x20U) /* Enables the generation of RSPI transmit interrupt */
/* RSPI Function Enable (SPE) */
#define _00_RSPI_FUNCTION_DISABLED              (0x00U) /* Disables the RSPI function */
#define _40_RSPI_FUNCTION_ENABLED               (0x40U) /* Enables the RSPI function */
/* RSPI Receive Interrupt Enable (SPRIE) */
#define _00_RSPI_RECEIVE_INTERRUPT_DISABLED     (0x00U) /* Disables the generation of RSPI receive interrupt */
#define _80_RSPI_RECEIVE_INTERRUPT_ENABLED      (0x80U) /* Enables the generation of RSPI receive interrupt */

/*
    RSPI Slave Select Polarity Register (SSLP)
*/
/* SSL0 Signal Polarity Setting (SSL0P) */
#define _00_RSPI_SSL0_POLARITY_LOW              (0x00U) /* SSL0 signal is active low */
#define _01_RSPI_SSL0_POLARITY_HIGH             (0x01U) /* SSL0 signal is active high */
/* SSL1 Signal Polarity Setting (SSL1P) */
#define _00_RSPI_SSL1_POLARITY_LOW              (0x00U) /* SSL1 signal is active low */
#define _02_RSPI_SSL1_POLARITY_HIGH             (0x02U) /* SSL1 signal is active high */
/* SSL2 Signal Polarity Setting (SSL2P) */
#define _00_RSPI_SSL2_POLARITY_LOW              (0x00U) /* SSL2 signal is active low */
#define _04_RSPI_SSL2_POLARITY_HIGH             (0x04U) /* SSL2 signal is active high */
/* SSL3 Signal Polarity Setting (SSL3P) */
#define _00_RSPI_SSL3_POLARITY_LOW              (0x00U) /* SSL3 signal is active low */
#define _08_RSPI_SSL3_POLARITY_HIGH             (0x08U) /* SSL3 signal is active high */

/*
    RSPI Pin Control Register (SPPCR)
*/
/* RSPI Loopback (SPLP) */
#define _00_RSPI_LOOPBACK_DISABLED              (0x00U) /* Normal mode */
#define _01_RSPI_LOOPBACK_ENABLED               (0x01U) /* Loopback mode (reversed transmit data = receive data) */
/* RSPI Loopback 2 (SPLP2) */
#define _00_RSPI_LOOPBACK2_DISABLED             (0x00U) /* Normal mode */
#define _02_RSPI_LOOPBACK2_ENABLED              (0x02U) /* Loopback mode (transmit data = receive data) */
/* MOSI Idle Fixed Value (MOIFV) */
#define _00_RSPI_MOSI_LEVEL_LOW                 (0x00U) /* Level output on MOSIA during idling corresponds to low */
#define _10_RSPI_MOSI_LEVEL_HIGH                (0x10U) /* Level output on MOSIA during idling corresponds to high */
/* MOSI Idle Value Fixing Enable (MOIFE) */
#define _00_RSPI_MOSI_FIXING_PREV_TRANSFER      (0x00U) /* MOSI output value equals final data from previous transfer */
#define _20_RSPI_MOSI_FIXING_MOIFV_BIT          (0x20U) /* MOSI output value equals the value set in the MOIFV bit */

/*
    RSPI Sequence Control Register (SPSCR)
*/
/* RSPI Sequence Length Specification (SPSLN[2:0]) */
#define _00_RSPI_SEQUENCE_LENGTH_1              (0x00U) /* 0 -> 0... */
#define _01_RSPI_SEQUENCE_LENGTH_2              (0x01U) /* 0 -> 1 -> 0... */
#define _02_RSPI_SEQUENCE_LENGTH_3              (0x02U) /* 0 -> 1 -> 2 -> 0... */
#define _03_RSPI_SEQUENCE_LENGTH_4              (0x03U) /* 0 -> 1 -> 2 -> 3 -> 0... */
#define _04_RSPI_SEQUENCE_LENGTH_5              (0x04U) /* 0 -> 1 -> 2 -> 3 -> 4 -> 0... */
#define _05_RSPI_SEQUENCE_LENGTH_6              (0x05U) /* 0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 0... */
#define _06_RSPI_SEQUENCE_LENGTH_7              (0x06U) /* 0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 0... */
#define _07_RSPI_SEQUENCE_LENGTH_8              (0x07U) /* 0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 0... */

/*
    RSPI Data Control Register (SPDCR)
*/
/* Number of Frames Specification (SPFC[1:0]) */
#define _00_RSPI_FRAMES_1                       (0x00U) /* 1 frame */
#define _01_RSPI_FRAMES_2                       (0x01U) /* 2 frames */
#define _02_RSPI_FRAMES_3                       (0x02U) /* 3 frames */
#define _03_RSPI_FRAMES_4                       (0x03U) /* 4 frames */
/* RSPI Receive/Transmit Data Selection (SPRDTD) */
#define _00_RSPI_READ_SPDR_RX_BUFFER            (0x00U) /* read SPDR values from receive buffer */
#define _10_RSPI_READ_SPDR_TX_BUFFER            (0x10U) /* read SPDR values from transmit buffer (transmit buffer empty) */
/* RSPI Longword Access/Word Access Specification (SPLW) */ 
#define _00_RSPI_ACCESS_WORD                    (0x00U) /* SPDR is accessed in words */
#define _20_RSPI_ACCESS_LONGWORD                (0x20U) /* SPDR is accessed in longwords */

/*
    RSPI Clock Delay Register (SPCKD)
*/
/* RSPCK Delay Setting (SCKDL[2:0]) */
#define _00_RSPI_RSPCK_DELAY_1                  (0x00U) /* 1 RSPCK */
#define _01_RSPI_RSPCK_DELAY_2                  (0x01U) /* 2 RSPCK */
#define _02_RSPI_RSPCK_DELAY_3                  (0x02U) /* 3 RSPCK */
#define _03_RSPI_RSPCK_DELAY_4                  (0x03U) /* 4 RSPCK */
#define _04_RSPI_RSPCK_DELAY_5                  (0x04U) /* 5 RSPCK */
#define _05_RSPI_RSPCK_DELAY_6                  (0x05U) /* 6 RSPCK */
#define _06_RSPI_RSPCK_DELAY_7                  (0x06U) /* 7 RSPCK */
#define _07_RSPI_RSPCK_DELAY_8                  (0x07U) /* 8 RSPCK */

/*
    RSPI Slave Select Negation Delay Register (SSLND)
*/
/* SSL Negation Delay Setting (SLNDL[2:0]) */
#define _00_RSPI_SSL_NEGATION_DELAY_1           (0x00U) /* 1 RSPCK */
#define _01_RSPI_SSL_NEGATION_DELAY_2           (0x01U) /* 2 RSPCK */
#define _02_RSPI_SSL_NEGATION_DELAY_3           (0x02U) /* 3 RSPCK */
#define _03_RSPI_SSL_NEGATION_DELAY_4           (0x03U) /* 4 RSPCK */
#define _04_RSPI_SSL_NEGATION_DELAY_5           (0x04U) /* 5 RSPCK */
#define _05_RSPI_SSL_NEGATION_DELAY_6           (0x05U) /* 6 RSPCK */
#define _06_RSPI_SSL_NEGATION_DELAY_7           (0x06U) /* 7 RSPCK */
#define _07_RSPI_SSL_NEGATION_DELAY_8           (0x07U) /* 8 RSPCK */

/*
    RSPI Next-Access Delay Register (SPND)
*/
/* RSPI Next-Access Delay Setting (SPNDL[2:0]) */
#define _00_RSPI_NEXT_ACCESS_DELAY_1            (0x00U) /* 1 RSPCK + 2 PCLK */
#define _01_RSPI_NEXT_ACCESS_DELAY_2            (0x01U) /* 2 RSPCK + 2 PCLK */
#define _02_RSPI_NEXT_ACCESS_DELAY_3            (0x02U) /* 3 RSPCK + 2 PCLK */
#define _03_RSPI_NEXT_ACCESS_DELAY_4            (0x03U) /* 4 RSPCK + 2 PCLK */
#define _04_RSPI_NEXT_ACCESS_DELAY_5            (0x04U) /* 5 RSPCK + 2 PCLK */
#define _05_RSPI_NEXT_ACCESS_DELAY_6            (0x05U) /* 6 RSPCK + 2 PCLK */
#define _06_RSPI_NEXT_ACCESS_DELAY_7            (0x06U) /* 7 RSPCK + 2 PCLK */
#define _07_RSPI_NEXT_ACCESS_DELAY_8            (0x07U) /* 8 RSPCK + 2 PCLK */

/*
    RSPI Control Register 2 (SPCR2)
*/
/* Parity Enable (SPPE) */
#define _00_RSPI_PARITY_DISABLE                 (0x00U) /* Does not add parity bit to transmit data */
#define _01_RSPI_PARITY_ENABLE                  (0x01U) /* Adds the parity bit to transmit data */
/* Parity Mode (SPOE) */
#define _00_RSPI_PARITY_EVEN                    (0x00U) /* Selects even parity for use in transmission and reception */
#define _02_RSPI_PARITY_ODD                     (0x02U) /* Selects odd parity for use in transmission and reception */
/* RSPI Idle Interrupt Enable (SPIIE) */
#define _00_RSPI_IDLE_INTERRUPT_DISABLED        (0x00U) /* Disables the generation of RSPI idle interrupt */
#define _04_RSPI_IDLE_INTERRUPT_ENABLED         (0x04U) /* Enables the generation of RSPI idle interrupt */
/* Parity Self-Testing (PTE) */
#define _00_RSPI_SELF_TEST_DISABLED             (0x00U) /* Disables the self-diagnosis function of the parity circuit */
#define _08_RSPI_SELF_TEST_ENABLED              (0x08U) /* Enables the self-diagnosis function of the parity circuit */

/*
    RSPI Command Registers 0 to 7 (SPCMD0 to SPCMD7)
*/
/* RSPCK Phase Setting (CPHA) */
#define _0000_RSPI_RSPCK_SAMPLING_ODD           (0x0000U) /* Data sampling on odd edge, data variation on even edge */
#define _0001_RSPI_RSPCK_SAMPLING_EVEN          (0x0001U) /* Data variation on odd edge, data sampling on even edge */
/* RSPCK Polarity Setting (CPOL) */
#define _0000_RSPI_RSPCK_POLARITY_LOW           (0x0000U) /* RSPCK is low when idle */
#define _0002_RSPI_RSPCK_POLARITY_HIGH          (0x0002U) /* RSPCK is high when idle */
/* Bit Rate Division Setting (BRDV[1:0]) */
#define _0000_RSPI_BASE_BITRATE_1               (0x0000U) /* These bits select the base bit rate */
#define _0004_RSPI_BASE_BITRATE_2               (0x0004U) /* These bits select the base bit rate divided by 2 */
#define _0008_RSPI_BASE_BITRATE_4               (0x0008U) /* These bits select the base bit rate divided by 4 */
#define _000C_RSPI_BASE_BITRATE_8               (0x000CU) /* These bits select the base bit rate divided by 8 */
/* SSL Signal Assertion Setting (SSLA[2:0]) */
#define _0000_RSPI_SIGNAL_ASSERT_SSL0           (0x0000U) /* SSL0 */
#define _0010_RSPI_SIGNAL_ASSERT_SSL1           (0x0010U) /* SSL1 */
#define _0020_RSPI_SIGNAL_ASSERT_SSL2           (0x0020U) /* SSL2 */
#define _0030_RSPI_SIGNAL_ASSERT_SSL3           (0x0030U) /* SSL3 */
/* SSL Signal Level Keeping (SSLKP) */
#define _0000_RSPI_SSL_KEEP_DISABLE             (0x0000U) /* Negates all SSL signals upon completion of transfer */
#define _0080_RSPI_SSL_KEEP_ENABLE              (0x0080U) /* Keep SSL level from end of transfer till next access */
/* RSPI Data Length Setting (SPB[3:0]) */
#define _0400_RSPI_DATA_LENGTH_BITS_8           (0x0400U) /* 8 bits */
#define _0800_RSPI_DATA_LENGTH_BITS_9           (0x0800U) /* 9 bits */
#define _0900_RSPI_DATA_LENGTH_BITS_10          (0x0900U) /* 10 bits */
#define _0A00_RSPI_DATA_LENGTH_BITS_11          (0x0A00U) /* 11 bits */
#define _0B00_RSPI_DATA_LENGTH_BITS_12          (0x0B00U) /* 12 bits */
#define _0C00_RSPI_DATA_LENGTH_BITS_13          (0x0C00U) /* 13 bits */
#define _0D00_RSPI_DATA_LENGTH_BITS_14          (0x0D00U) /* 14 bits */
#define _0E00_RSPI_DATA_LENGTH_BITS_15          (0x0E00U) /* 15 bits */
#define _0F00_RSPI_DATA_LENGTH_BITS_16          (0x0F00U) /* 16 bits */
#define _0000_RSPI_DATA_LENGTH_BITS_20          (0x0000U) /* 20 bits */
#define _0100_RSPI_DATA_LENGTH_BITS_24          (0x0100U) /* 24 bits */
#define _0200_RSPI_DATA_LENGTH_BITS_32          (0x0200U) /* 32 bits */
/* RSPI LSB First (LSBF) */
#define _0000_RSPI_MSB_FIRST                    (0x0000U) /* MSB first */
#define _1000_RSPI_LSB_FIRST                    (0x1000U) /* LSB first */
/* RSPI Next-Access Delay Enable (SPNDEN) */
#define _0000_RSPI_NEXT_ACCESS_DELAY_DISABLE    (0x0000U) /* Next-access delay of 1 RSPCK + 2 PCLK */
#define _2000_RSPI_NEXT_ACCESS_DELAY_ENABLE     (0x2000U) /* Next-access delay equal to setting of SPND register */
/* SSL Negation Delay Setting Enable (SLNDEN) */
#define _0000_RSPI_NEGATION_DELAY_DISABLE       (0x0000U) /* SSL negation delay of 1 RSPCK */
#define _4000_RSPI_NEGATION_DELAY_ENABLE        (0x4000U) /* SSL negation delay equal to setting of SSLND register */
/* RSPCK Delay Setting Enable (SCKDEN) */
#define _0000_RSPI_RSPCK_DELAY_DISABLE          (0x0000U) /* RSPCK delay of 1 RSPCK */
#define _8000_RSPI_RSPCK_DELAY_ENABLE           (0x8000U) /* RSPCK delay equal to setting of the SPCKD register */

/*
    Interrupt Source Priority Register n (IPRn)
*/
/* Interrupt Priority Level Select (IPR[3:0]) */
#define _00_RSPI_PRIORITY_LEVEL0                (0x00U) /* Level 0 (interrupt disabled) */
#define _01_RSPI_PRIORITY_LEVEL1                (0x01U) /* Level 1 */
#define _02_RSPI_PRIORITY_LEVEL2                (0x02U) /* Level 2 */
#define _03_RSPI_PRIORITY_LEVEL3                (0x03U) /* Level 3 */
#define _04_RSPI_PRIORITY_LEVEL4                (0x04U) /* Level 4 */
#define _05_RSPI_PRIORITY_LEVEL5                (0x05U) /* Level 5 */
#define _06_RSPI_PRIORITY_LEVEL6                (0x06U) /* Level 6 */
#define _07_RSPI_PRIORITY_LEVEL7                (0x07U) /* Level 7 */
#define _08_RSPI_PRIORITY_LEVEL8                (0x08U) /* Level 8 */
#define _09_RSPI_PRIORITY_LEVEL9                (0x09U) /* Level 9 */
#define _0A_RSPI_PRIORITY_LEVEL10               (0x0AU) /* Level 10 */
#define _0B_RSPI_PRIORITY_LEVEL11               (0x0BU) /* Level 11 */
#define _0C_RSPI_PRIORITY_LEVEL12               (0x0CU) /* Level 12 */
#define _0D_RSPI_PRIORITY_LEVEL13               (0x0DU) /* Level 13 */
#define _0E_RSPI_PRIORITY_LEVEL14               (0x0EU) /* Level 14 */
#define _0F_RSPI_PRIORITY_LEVEL15               (0x0FU) /* Level 15 (highest) */


/***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/
#define _20_RSPI0_DIVISOR                       (0x20U)

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
#define ADC_PRI_SPTI1        (8uL)       /* SPI0 transmit interrupt priority                 */
#define ADC_PRI_SPRI1        (6uL)       /* SPI0 receive interrupt priority                  */
#define ADC_PRI_SPEI1        (7uL)       /* SPI0 error interrupt priority                    */
#define ADC_PRI_SPII1        (5uL)       /* SPI0 idle interrupt priority                     */

/***********************************************************************************************************************
Global functions
***********************************************************************************************************************/
void R_RSPI1_Create(void);
//void R_RSPI0_LoopBackDirect(void);
void R_RSPI1_LoopBackReversed(void);
void R_RSPI1_LoopBackDisable(void);
uint8_t R_RSPI1_Send_Receive(uint32_t * const txbuf, uint16_t txnum, uint32_t * const rxbuf);
void r_rspi1_callback_transmitend(void);
void r_rspi1_callback_receiveend(void);
void r_rspi1_callback_error(uint8_t err_type);

/* Start user code for function. Do not edit comment generated here */
void R_RSPI1_Start(void);
void R_RSPI1_Stop(void);

void Init_SPI(void);

void r_rspi1_idle_interrupt(uint32_t int_sense);
void r_rspi1_error_interrupt(uint32_t int_sense);
void r_rspi1_receive_interrupt(uint32_t int_sense);
void r_rspi1_transmit_interrupt(uint32_t int_sense);

/* End user code. Do not edit comment generated here */
#endif /* RSPI_H */
