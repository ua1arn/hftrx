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
* File Name    : sample_riic_rza1h_rsk_i2c.h
* Description  : Sample Program - RIIC header file
******************************************************************************/
#ifndef SAMPLE_RIIC_RZA1H_RSK_I2C_H
#define SAMPLE_RIIC_RZA1H_RSK_I2C_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "command.h"

#include "r_typedefs.h"

/*****************************************************************************
Macro definitions
******************************************************************************/
#define RIIC_CH_TOTAL           (4)

/* I2C Channel 0 Slave Devices  SLA-ADDR   Description                        */
#define RZA1H_APP_I2C_FT5216    (0x70)           /* FocalTech CTP Controller  */
#define RZA1H_APP_I2C_TFP410    (0x78)    /* TI PanelBus™ DIGITAL TRANSMITTER */

/* I2C Channel 3 Slave Devices  SLA-ADDR   Description                        */
#define RZA1H_RSK_I2C_MAXIM     (0x20)     /* MAXIM9858 Low-Power Audio CODEC */
#define RZA1H_RSK_I2C_PX_IO1    (0x40)  /* On-Semiconductor CMOS I/O expander */
#define RZA1H_RSK_I2C_PX_IO2    (0x41)  /* On-Semiconductor CMOS I/O expander */
#define RZA1H_RSK_I2C_EEPROM    (0xA0)     /* Renesas serial interface EEPROM */

/* Port Expander 1 Default Connectivity Enable / Disable                      */
/* IO[0] PX1_EN0 Selects IC37/IC38 MUXes BL[x] signals.                       */
/* IO[1] PX1_EN1 -                                                            */
/* IO[2] TFT_CS  - Chip select for connecting 3rd  party TFTs requiring SPI   */
/* IO[3] PX1_EN3 - Set Audio signals, set to 1, selects optional PWM signals  */
/* IO[4] USB_OVR_CURRENT - Input signal from IC7, pulled high by default      */
/* IO[5] USB_PWR_ENA - Power to VBUS0, JP11  shorted Pulled low by default    */
/* IO[6] USB_PWR_ENB - Power to VBUS1, JP12  shorted Pulled low by default    */
/* IO[7] PX1_EN7 - CLEAR (0) selects A18 - A21, SET (1) selects SGOUT         */

/* Port Expander 2 Default Connectivity Enable / Disable                      */
/* IO[0] LED1                                                                 */
/* IO[1] LED2                                                                 */
/* IO[2] LED3                                                                 */
/* IO[3] NOR A25                                                              */
/* IO[4] PMOD1 Reset                                                          */
/* IO[5] PMOD2 Reset                                                          */
/* IO[6] SD Header Power Enable                                               */
/* IO[7] SD/MMC Power Enable                                                  */

/* Port Expander  Sub-Address                                                 */
#define PORTX_INPUT_REG 			(0x00u)
#define PORTX_OUTPUT_REG 			(0x01u)
#define PORTX_POL_INV_REG			(0x02u)
#define PORTX_CONFIG_REG 			(0x03u)

#define PX_CMD_READ_REG				(0x00u)
#define PX_CMD_WRITE_OUT_REG		(0x01u)
#define PX_CMD_POLARITY_REG			(0x02u)
#define PX_CMD_CONFIG_REG			(0x03u)


/* RZA1H_RSK_I2C_PX_IO1 / RZA1H_RSK_I2C_PX_IO2 Slave Address Device Codes     */
#define PX_IO_0                 (0x01)                /* Configure I/O port 0 */
#define PX_IO_1	                (0x02)                /* Configure I/O port 1 */
#define PX_IO_2	                (0x04)                /* Configure I/O port 2 */
#define PX_IO_3                 (0x08)                /* Configure I/O port 3 */
#define PX_IO_4	                (0x10)                /* Configure I/O port 4 */
#define PX_IO_5                 (0x20)                /* Configure I/O port 5 */
#define PX_IO_6                 (0x40)                /* Configure I/O port 6 */
#define PX_IO_7                 (0x80)                /* Configure I/O port 7 */

#define SET_IO_0                (0x01)          /* Set I/O Output 1 on port 0 */
#define SET_IO_1                (0x02)          /* Set I/O Output 1 on port 1 */
#define SET_IO_2                (0x04)          /* Set I/O Output 1 on port 2 */
#define SET_IO_3                (0x08)          /* Set I/O Output 1 on port 3 */
#define SET_IO_4                (0x10)          /* Set I/O Output 1 on port 4 */
#define SET_IO_5                (0x20)          /* Set I/O Output 1 on port 5 */
#define SET_IO_6                (0x40)          /* Set I/O Output 1 on port 6 */
#define SET_IO_7                (0x80)          /* Set I/O Output 1 on port 7 */

#define CLR_IO_0               (~0x01)          /* Set I/O Output 0 on port 0 */
#define CLR_IO_1               (~0x02)          /* Set I/O Output 0 on port 1 */
#define CLR_IO_2               (~0x04)          /* Set I/O Output 0 on port 2 */
#define CLR_IO_3               (~0x08)          /* Set I/O Output 0 on port 3 */
#define CLR_IO_4               (~0x10)          /* Set I/O Output 0 on port 4 */
#define CLR_IO_5               (~0x20)          /* Set I/O Output 0 on port 5 */
#define CLR_IO_6               (~0x40)          /* Set I/O Output 0 on port 6 */
#define CLR_IO_7               (~0x80)          /* Set I/O Output 0 on port 7 */


/* RZA1H_RSK_I2C_PX_IO1 / RZA1H_RSK_I2C_PX_IO2 Slave Address Device Codes     */

/******************************************************************************
Functions Prototypes
******************************************************************************/
int32_t Sample_RIIC_EepAccess(int32_t argc,
		                      char_t ** argv,
		                      uint8_t device_addr,
		                      uint8_t * data_addr,
		                      uint16_t data_length,
		                      uint16_t memory_addr);

int32_t R_RIIC_rza1h_rsk_write(uint32_t channel,             /* Channel 0..3 */
                               uint8_t d_adr,               /* Slave address */
                               uint16_t w_adr,          /* Slave sub-address */
                               uint32_t w_byte,           /* Slave data size */
                               uint8_t * w_buffer);       /* Pointer to data */


int32_t R_RIIC_rza1h_rsk_read(uint32_t channel,              /* Channel 0..3 */
		                      uint8_t d_adr,                /* Slave address */
		                      uint16_t r_adr,           /* Slave sub-address */
		                      uint32_t r_byte,            /* Slave data size */
		                      uint8_t * r_buffer);        /* Pointer to data */

int32_t Sample_RIIC_PortExpAccess(uint32_t channel,
		                          uint8_t d_code,
		                          uint8_t reg_command,
		                          uint8_t config_io,
		                          uint8_t w_byte);

int32_t Sample_RIIC_PortExpReadRegister(uint8_t d_code, uint8_t d_command);

int32_t Sample_RIIC_PortExpRead(uint8_t d_code);

void Sample_RIIC_Ri0_Interrupt(uint32_t int_sense);

void Sample_RIIC_Ti0_Interrupt(uint32_t int_sense);

void Sample_RIIC_Tei0_Interrupt(uint32_t int_sense);

void Sample_RIIC_Ri3_Interrupt(uint32_t int_sense);

void Sample_RIIC_Ti3_Interrupt(uint32_t int_sense);

void Sample_RIIC_Tei3_Interrupt(uint32_t int_sense);

int32_t Sample_RIIC_ScanPort(uint32_t channel);

void R_RIIC_rza1h_rsk_init(void);

#endif  /* SAMPLE_RIIC_RZA1H_RSK_I2C_H */

/* End of File */
