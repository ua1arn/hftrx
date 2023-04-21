/*******************************************************************************
*  MCom-02 FreeRTOS port                                                       *
*  Copyright (c) 2017 by RnD Center "ELVEES", JSC                              *
*                                                                              *
*  This file is part of the FreeRTOS distribution.                             *
*                                                                              *
*  FreeRTOS is free software; you can redistribute it and/or modify it under   *
*  the terms of the GNU General Public License (version 2) as published by the *
*  Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.  *
*******************************************************************************/


#ifndef SPI_FLASH_H
#define SPI_FLASH_H

typedef volatile struct
{
	volatile unsigned int CTRLR0;
	volatile unsigned int CTRLR1;
	volatile unsigned int SSIENR;
	volatile unsigned int MWCR;
	volatile unsigned int SER;
	volatile unsigned int BAUDR;
	volatile unsigned int TXFTLR;
	volatile unsigned int RXFTLR;
	volatile unsigned int TXFLR;
	volatile unsigned int RXFLR;
	volatile unsigned int hole1;
	volatile unsigned int IMR;
	volatile unsigned int ISR;
	volatile unsigned int RISR;
	volatile unsigned int TXOICR;
	volatile unsigned int RXOICR;
	volatile unsigned int RXUICR;
	volatile unsigned int MSTICR;
	volatile unsigned int ICR;
	volatile unsigned int DMACR;
	volatile unsigned int DMATDLR;
	volatile unsigned int DMARDLR;
	volatile unsigned int hole2[2];
	volatile unsigned int DR;
	volatile unsigned int dr_hole[35];
	volatile unsigned int RX_SAMPLE_DLY;
	volatile unsigned int SS_TOGGLE;
} spi_port;


void ConfigSPI(int iPort);
void DisableSPI(int iPort);
unsigned char ReadStatusRegisterSPIFlash(spi_port *spi);
void SendOpcodeSPI(spi_port *spi, unsigned char opcode);
void BlockEraseSPIFlash(int iPort, unsigned int spiflash_block_address);
void WriteSPIFlash(int iPort, unsigned int address, unsigned char* bytes, unsigned int size);
int  ReadSPIFlash(int iPort, unsigned int address, unsigned char* bytes, unsigned int size);
//
int  GetSPIStatus(void);
void WriteSPIFlashPart(unsigned int address, unsigned char* bytes, unsigned int size);
//
#endif
