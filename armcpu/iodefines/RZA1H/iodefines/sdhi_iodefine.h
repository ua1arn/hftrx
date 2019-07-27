/*******************************************************************************
* File Name : sdhi_iodefine.h
* Description : Definition of I/O Register
******************************************************************************/
#ifndef SDHI_IODEFINE_H
#define SDHI_IODEFINE_H

/* SD Host Interface */

/* Table 50.2.. Register Configuration */
typedef struct st_sdhi
{		 
	volatile uint16_t	SD_CMD;				/* 	Command type register					0xE804E000 0xE804E800  */
	volatile uint8_t	dummy1 [2];
	volatile uint16_t	SD_ARG0;			/* 	SD command argument register			0xE804E004 0xE804E804  */
	volatile uint16_t	SD_ARG1;			/* 	SD command argument register			0xE804E006 0xE804E806  */
	volatile uint16_t	SD_STOP;			/* 	Data stop register						0xE804E008 0xE804E808  */
	volatile uint16_t	SD_SECCNT;			/* 	Block count register					0xE804E00A 0xE804E80A  */
	volatile uint16_t	SD_RSP00;			/* 	SD card response register				0xE804E00C 0xE804E80C  */
	volatile uint16_t	SD_RSP01;			/* 	SD card response register				0xE804E00E 0xE804E80E  */
	volatile uint16_t	SD_RSP02;			/* 	SD card response register				0xE804E010 0xE804E810  */
	volatile uint16_t	SD_RSP03;			/* 	SD card response register				0xE804E012 0xE804E812  */
	volatile uint16_t	SD_RSP04;			/* 	SD card response register				0xE804E014 0xE804E814  */
	volatile uint16_t	SD_RSP05;			/* 	SD card response register				0xE804E016 0xE804E816  */
	volatile uint16_t	SD_RSP06;			/* 	SD card response register				0xE804E018 0xE804E818  */
	volatile uint16_t	SD_RSP07;			/* 	SD card response register				0xE804E01A 0xE804E81A  */
	volatile uint16_t	SD_INFO1;			/* 	SD card interrupt flag register 1		0xE804E01C 0xE804E81C  */
	volatile uint16_t	SD_INFO2;			/* 	SD card interrupt flag register 2		0xE804E01E 0xE804E81E  */
	volatile uint16_t	SD_INFO1_MASK;		/* 	SD_INFO1 interrupt mask register		0xE804E020 0xE804E820  */
	volatile uint16_t	SD_INFO2_MASK;		/* 	SD_INFO2 interrupt mask register		0xE804E022 0xE804E822  */
	volatile uint16_t	SD_CLK_CTRL;		/* 	SD clock control register				0xE804E024 0xE804E824  */
	volatile uint16_t	SD_SIZE;			/* 	Transfer data length register			0xE804E026 0xE804E826  */
	volatile uint16_t	SD_OPTION;			/* 	SD card access control option register	0xE804E028 0xE804E828  */
	volatile uint8_t	dummy2 [2];
	volatile uint16_t	SD_ERR_STS1;		/* 	SD error status register 1				0xE804E02C 0xE804E82C  */
	volatile uint16_t	SD_ERR_STS2;		/* 	SD error status register 2				0xE804E02E 0xE804E82E  */
	volatile uint32_t	SD_BUF0;			/* 	SD buffer read/write register			0xE804E030 0xE804E830  */
	volatile uint16_t	SDIO_MODE;			/* 	SDIO mode control register				0xE804E034 0xE804E834  */
	volatile uint16_t	SDIO_INFO1;			/* 	SDIO interrupt flag register			0xE804E036 0xE804E836  */
	volatile uint16_t	SDIO_INFO1_MASK;	/* 	SDIO_INFO1 interrupt mask register		0xE804E038 0xE804E838  */
	volatile uint8_t	dummy3 [158];
	volatile uint16_t	CC_EXT_MODE;		/* 	DMA mode enable register				0xE804E0D8 0xE804E8D8  */
	volatile uint8_t	dummy4 [6];
	volatile uint16_t	SOFT_RST;			/* 	Software reset register					0xE804E0E0 0xE804E8E0  */
	volatile uint16_t	VERSION;			/* 	Version register						0xE804E0E2 0xE804E8E2  */
	volatile uint8_t	dummy5 [12];
	volatile uint16_t	EXT_SWAP;			/* 	Swap control register					0xE804E0F0 0xE804E8F0  */
	volatile uint8_t	dummy6 [14];
} r_io_sdhi_t;

#define SDHI0    (*(struct st_sdhi    *)0xE804E000uL) /* SDHI */
#define SDHI1    (*(struct st_sdhi    *)0xE804E800uL) /* SDHI */

#define SDHI_SD_CMD_0 SDHI0.SD_CMD
#define SDHI_SD_SD_ARG0_0 SDHI0.SD_ARG0


#endif /* SDHI_IODEFINE_H */
