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



#include "spi_flash.h"
#include "1892vm14ya.h"
#include "gpio.h"

#define MRST(A) gpio_pin_set('c', 6, A)

#define SPI_CLK_MHZ     20      //1-100MHz
#define MAX_DATA_SIZE   0xF800
#define HEADER_SIZE     64

//Opcodes
//#define OP_READ         0x3
#define OP_READ         0xB
#define OP_WREN         0x6
#define OP_WRDIS        0x4
#define OP_PAGEPROG     0x2
#define OP_READSTATUS   0x5
#define OP_BLOCKERASE   0xd8
#define OP_CHIPERASE    0x60
//#define OP_CHIPERASE    0xc7

//Protocol
#define MOTOROLA        (0 << 4)
#define TEXAS           (1 << 4)
#define MICROWIRE       (2 << 4)

//FrameSize
#define BIT4            0x3
#define BIT8            0x7
#define BIT16           0xF

//Clock_phase
#define IN_MIDDLE       (0 << 6)
#define AT_START        (1 << 6)

//Clock_polarity
#define INACTIVE_LOW    (0 << 7)
#define INACTIVE_HIGH   (1 << 7)

//Slave_select
#define NO_SLAVES       0x0
#define SLAVE_0         0x1
#define SLAVE_1         0x2
#define SLAVE_2         0x4
#define SLAVE_3         0x8

#define CMCTR_BASE 0x38094000
#define GATE_SYS_CTR	(*(volatile unsigned int*)(CMCTR_BASE + 0x04c))
	#define CLK_NOR_EN					(1 << 22)
	#define CLK_NFC_EN					(1 << 21)
	#define CLK_SPI1_EN					(1 << 20)
	#define CLK_SPI0_EN					(1 << 19)
	#define CLK_I2C2_EN					(1 << 18)
	#define CLK_I2C1_EN					(1 << 17)
	#define CLK_I2C0_EN					(1 << 16)
	#define CLK_UART3_EN				(1 << 15)
	#define CLK_UART2_EN				(1 << 14)
	#define CLK_UART1_EN				(1 << 13)
	#define CLK_UART0_EN				(1 << 12)
	#define CLK_SWIC1_EN				(1 << 11)
	#define CLK_SWIC0_EN				(1 << 10)
	#define CLK_MFBSP1_EN				(1 << 9)
	#define CLK_MFBSP0_EN				(1 << 8)
	#define CLK_PWM_EN					(1 << 7)
	#define CLK_MCC_EN					(1 << 6)
	#define CLK_USB_EN					(1 << 5)
	#define CLK_EMAC_EN					(1 << 4)
	#define CLK_SDMMC1_EN				(1 << 3)
	#define CLK_SDMMC0_EN				(1 << 2)
	#define CLK_I2S_EN					(1 << 1)
	#define CLK_SYS_EN					(1 << 0)

//Регистры контроля PLL
#define SEL_APLL		(*(volatile unsigned int*)(CMCTR_BASE + 0x100))
#define SEL_CPLL		(*(volatile unsigned int*)(CMCTR_BASE + 0x104))
#define SEL_DPLL		(*(volatile unsigned int*)(CMCTR_BASE + 0x108))
#define SEL_SPLL		(*(volatile unsigned int*)(CMCTR_BASE + 0x10c))
#define SEL_VPLL		(*(volatile unsigned int*)(CMCTR_BASE + 0x110))
/*
#define SPIO0_BASE 0x38032000
#define SPI0(a) (*(volatile unsigned int*)(SPIO0_BASE + (a)))
#define SPIO1_BASE 0x38033000
#define SPI1(a) (*(volatile unsigned int*)(SPIO1_BASE + (a)))
#define CTRLR0							0x00
#define CTRLR1							0x04
#define SSIENR							0x08
#define MWCR							0x0c
#define SER								0x10
#define BAUDR							0x14
#define TXFTLR							0x18
#define RXFTLR							0x1c
#define TXFLR							0x20
#define RXFLR							0x24
#define REZERV							0x28
#define IMR								0x2c
#define ISR								0x30
#define RISR							0x34
#define TXOICR							0x38
#define RXOICR							0x3c
#define RXUICR							0x40
#define MSTICR							0x44
#define ICR								0x48
#define DR								0x60 //- 0xEC
#define SS_TOGGLE						0xf4
*/

#define GPIO0_BASE 0x38034000
#define GPIO0(a) (*(volatile unsigned int*)(GPIO0_BASE + (a)))
#define SWPORTA_DR						0x00
#define SWPORTA_DDR						0x04
#define SWPORTA_CTL						0x08
#define SWPORTB_DR						0x0c
#define SWPORTB_DDR						0x10
#define SWPORTB_CTL						0x14
#define SWPORTC_DR						0x18
#define SWPORTC_DDR						0x1c
#define SWPORTC_CTL						0x20
#define SWPORTD_DR						0x24
#define SWPORTD_DDR						0x28
#define SWPORTD_CTL						0x2c
/*
#define GPIO_INTEN						0x30
#define GPIO_INTMASK						0x34
#define GPIO_INTTYPE_LEVEL					0x38
#define GPIO_INT_POLARITY					0x3c
#define GPIO_PORTA_EOI						0x4c
#define GPIO_EXT_PORTA						0x50
*/
#define GPIOA22_SPI1_SCLK				(1 << 22)
#define GPIOA23_SPI1_TXD				(1 << 23)
#define GPIOA24_SPI1_RXD				(1 << 24)
#define GPIOA25_SPI1_SSN0				(1 << 25)
#define GPIOA26_SPI1_SSN1				(1 << 26)
#define GPIOA27_SPI1_SSN2				(1 << 27)
#define GPIOA28_SPI1_SSN3				(1 << 28)

#define GPIOD15_SPI0_SCLK				(1 << 15)
#define GPIOD16_SPI0_TXD				(1 << 16)
#define GPIOD17_SPI0_RXD				(1 << 17)
#define GPIOD18_SPI0_SSN0				(1 << 18)
#define GPIOD19_SPI0_SSN1				(1 << 19)
#define GPIOD20_SPI0_SSN2				(1 << 20)
#define GPIOD21_SPI0_SSN3				(1 << 21)

#define PAGE_SIZE ((unsigned int)0x40000)

spi_port* get_spi_dev(int num)
{
	if (num == 0) return ((spi_port volatile *)BASE_ADDR_SPI0);
	else if (num == 1) return ((spi_port volatile *)BASE_ADDR_SPI1);
	else return 0;
}



typedef struct _data_block_spi_
{
    int FrameCount;
    union
    {
        unsigned short Data[8];
        struct
        {
            unsigned short fr5;
            unsigned short fr4;
            unsigned short fr3;
            unsigned short fr2;
            union
            {
                unsigned int Value;
                struct
                {                    
                    unsigned short fr1;
                    unsigned short fr0;
                };
            };
            unsigned int Address : 24;
            unsigned int OpCode : 8;
        };
    };
}data_block_t;

void ConfigSPI(int iPort)
{
    spi_port *spi_dev = get_spi_dev(iPort);
    	if( spi_dev == 0) return;
    //
    if(iPort == 0)
    {
        gpio_pin_config('c', 6, 0, 1, 0);
    	//MRST(0); ^^
    	GATE_SYS_CTR |= CLK_SPI0_EN; //Enable CLK
    	GPIO0(SWPORTD_CTL) |= GPIOD15_SPI0_SCLK | GPIOD16_SPI0_TXD | GPIOD17_SPI0_RXD 
                       | GPIOD18_SPI0_SSN0;// | GPIOD19_SPI0_SSN1 | GPIOD20_SPI0_SSN2 | GPIOD21_SPI0_SSN3; //Set pins
        //-------------------------------------------------
           for(int i=0; i<10000;i++)
           {
           	asm(" NOP");
           }
        //-------------------------------------------------
        MRST(1);
    }
    if(iPort == 1)
    {
    	GATE_SYS_CTR |= CLK_SPI1_EN; //Enable CLK
    	GPIO0(SWPORTD_CTL) |= GPIOA22_SPI1_SCLK | GPIOA23_SPI1_TXD | GPIOA24_SPI1_RXD
                       | GPIOA25_SPI1_SSN0;// | GPIOD19_SPI0_SSN1 | GPIOD20_SPI0_SSN2 | GPIOD21_SPI0_SSN3; //Set pins
    }

    spi_dev->SSIENR = 0;
    spi_dev->CTRLR0 = MOTOROLA | INACTIVE_LOW | BIT8 | AT_START;//BIT16 | IN_MIDDLE
    spi_dev->SER = NO_SLAVES; //Disable all slaves
    spi_dev->CTRLR1 = 32;     //Length of one transfer (changes during operation)
    spi_dev->BAUDR  = 6;       //144 / 4 = 36MHz
    spi_dev->TXFTLR = 256;    //FIFO transfer size
    spi_dev->RXFTLR = 256;    //FIFO receive size
    spi_dev->SS_TOGGLE = 0;   //Hold CSn while transfer data
}

void DisableSPI(int iPort)
{
    spi_port *spi_dev = get_spi_dev(iPort);if( spi_dev == 0) return;
    spi_dev->SSIENR = 0;

    if(iPort == 0)
    {
    	GATE_SYS_CTR &= ~CLK_SPI0_EN; //Disable CLK
    	GPIO0(SWPORTD_CTL) &= ~(GPIOD15_SPI0_SCLK | GPIOD16_SPI0_TXD | GPIOD17_SPI0_RXD 
                       | GPIOD18_SPI0_SSN0);// | GPIOD19_SPI0_SSN1 | GPIOD20_SPI0_SSN2 | GPIOD21_SPI0_SSN3; //Unset pins
    }
    if(iPort == 1)
    {
    	GATE_SYS_CTR &= ~CLK_SPI1_EN; //Disable CLK
    	GPIO0(SWPORTD_CTL) &= ~(GPIOA22_SPI1_SCLK | GPIOA23_SPI1_TXD | GPIOA24_SPI1_RXD
                       | GPIOA25_SPI1_SSN0);// | GPIOD19_SPI0_SSN1 | GPIOD20_SPI0_SSN2 | GPIOD21_SPI0_SSN3; //Unset pins
    }
    
}
void SetLengthSPITransfer(spi_port *spi_dev, unsigned int len)
{
    if(spi_dev->CTRLR1 != len) 
    {
        spi_dev->SSIENR = 0;
        spi_dev->CTRLR1 = len;
        spi_dev->SSIENR = 1;
    }
}
void SetFrameSizeSPI(spi_port *spi_dev, unsigned int size)
{
    unsigned int ctrlr = spi_dev->CTRLR0;
    size &= 0xF;
    if((ctrlr & 0xF) != size)
    {
        spi_dev->SSIENR = 0;
        spi_dev->CTRLR0 = (ctrlr & 0xFFFFFFF0) | size;
        spi_dev->SSIENR = 1;
    }
}

int ReadSPIFlash(int iPort, unsigned int address, unsigned char* bytes, unsigned int size)
{
    unsigned short tmp;
    unsigned int framelength;
    //unsigned int CRC = 0;
    int i = 0, j = 0, count;
    data_block_t block;
    spi_port *spi_dev = get_spi_dev(iPort);
    	if( spi_dev == 0) return 0;
    
    block.OpCode  = OP_READ;
    block.Address = address;
    block.Value   = 0;
  
    spi_dev->SSIENR = 1;


    SetLengthSPITransfer(spi_dev, 2);
    SetFrameSizeSPI(spi_dev, BIT16);
    spi_dev->SER = SLAVE_0;
    spi_dev->DR = block.Data[7];
    spi_dev->DR = block.Data[6];
    while(spi_dev->RXFLR < 2) ; // < count) ;//Wait Recieve
    tmp = spi_dev->DR;
    tmp = spi_dev->DR;
    SetLengthSPITransfer(spi_dev, 1);
    SetFrameSizeSPI(spi_dev, BIT8);
    spi_dev->DR = 0;//fake read - write
    while(spi_dev->RXFLR == 0) ; // < count) ;//Wait Recieve
    tmp = spi_dev->DR;


    while(i < size)
    {
        SetLengthSPITransfer(spi_dev, 1);
        spi_dev->DR = 0;//fake
        while(spi_dev->RXFLR == 0) ; // < count) ;//Wait Recieve
        tmp = spi_dev->DR;
        //spi_dev->SER = SLAVE_0;
        *bytes++ = spi_dev->DR;
        //--------------------------
        i++;
    }
    //---------------------
        spi_dev->SER = NO_SLAVES;
    //---------------------
    spi_dev->SSIENR = 0;

    return 0;//xFFFFFFFF - CRC;
}
void SendOpcodeSPI(spi_port *spi_dev, unsigned char opcode)
{
    spi_dev->SSIENR = 1;
    SetFrameSizeSPI(spi_dev, BIT8);
    SetLengthSPITransfer(spi_dev, 1);
    //spi_dev->SER = SLAVE_0;
    spi_dev->DR = opcode;
    while(spi_dev->RXFLR == 0);
    spi_dev->SSIENR = 0;
}
unsigned char ReadStatusRegisterSPIFlash(spi_port *spi_dev)
{
    spi_dev->SSIENR = 1;
    SetFrameSizeSPI(spi_dev, BIT16);
    SetLengthSPITransfer(spi_dev, 1);
    //spi_dev->SER = SLAVE_0;
    spi_dev->DR = OP_READSTATUS << 8;
    while(spi_dev->RXFLR == 0);
    return (unsigned char)(spi_dev->DR);
}

void BlockEraseSPIFlash(int iPort, unsigned int spiflash_block_address)
{
    data_block_t block;
    spi_port *spi_dev = get_spi_dev(iPort);
    	if( spi_dev == 0) return;
    block.OpCode = OP_BLOCKERASE;
    block.Address = spiflash_block_address;

    SendOpcodeSPI(spi_dev, OP_WREN);
    SetFrameSizeSPI(spi_dev, BIT16);
    SetLengthSPITransfer(spi_dev, 2);
    spi_dev->SSIENR = 1;
    spi_dev->SER = NO_SLAVES;
    spi_dev->DR = block.Data[7];
    spi_dev->DR = block.Data[6];
    //spi_dev->DR = block.Data[5];
    //spi_dev->SER = SLAVE_0;
    //*spiflash_block_address += 0x10000;
    while(spi_dev->RXFLR < 2);
    spi_dev->SSIENR = 0; //Clear FIFO
    /*
    while(ReadStatusRegisterSPIFlash(spi_dev) & 0x1);
    spi_dev->SSIENR = 0;
    */
}

static int iSpiFlashDataCount[2];

int GetSPIStatus(void)
{
    spi_port *spi_dev = get_spi_dev(0);
	if(spi_dev->RXFLR < iSpiFlashDataCount[0]) return 1;
        spi_dev->SSIENR = 0; //Clear FIFO
	return ((ReadStatusRegisterSPIFlash(spi_dev) & 0x1));
}


void WriteSPIFlashPart(unsigned int address, unsigned char* bytes, unsigned int size)
{
    unsigned int count;
    unsigned int framelength = size; // 130;
    data_block_t block;

    spi_port *spi_dev = get_spi_dev(0);
    	if( spi_dev == 0) return;
    
        //framelength = (size >> 1) + 2;

    	block.OpCode = OP_PAGEPROG;
    	block.Address = address;

        //count = 2; //????

        SendOpcodeSPI(spi_dev, OP_WREN);
        SetFrameSizeSPI(spi_dev, BIT16);
        SetLengthSPITransfer(spi_dev, framelength);
        spi_dev->SSIENR = 1;
        spi_dev->SER = NO_SLAVES;

        spi_dev->DR = block.Data[7];
        spi_dev->DR = block.Data[6];

        spi_dev->DR = bytes[0];
}



void WriteSPIFlash(int iPort, unsigned int address, unsigned char* bytes, unsigned int size)
{
    unsigned int i = 0, page_address, total_size, j, count = 0, spiflash_block_address;
    data_block_t block;
    spi_port *spi_dev = get_spi_dev(iPort);
    	if( spi_dev == 0) return;
    
    if(size == 0)
    {
        return;
    }
    else if(size & 0x00000001)
    {
        ++size;
    }
    
    page_address = address & ~(PAGE_SIZE-1);
    total_size = (address - page_address + size);

    //SendOpcodeSPI(spi_dev, OP_WREN);
    
    do
    {
    	spiflash_block_address = address & (PAGE_SIZE-1);
    	if(spiflash_block_address == 0) BlockEraseSPIFlash(iPort, page_address);

        SendOpcodeSPI(spi_dev, OP_WREN);
        SetFrameSizeSPI(spi_dev, BIT16);//BIT16
        //SetLengthSPITransfer(spi_dev, framelength);
        spi_dev->SSIENR = 1;
        spi_dev->SER = NO_SLAVES;

    	block.OpCode = OP_PAGEPROG;
    	block.Address = address;

        spi_dev->DR = block.Data[7];
        spi_dev->DR = block.Data[6];
        //spi_dev->DR = block.Data[5];
        SetFrameSizeSPI(spi_dev, BIT8);//BIT16

        spi_dev->DR = bytes[count];

        //spi_dev->SER = SLAVE_0;

        spi_dev->SSIENR = 0; //Clear FIFO

        while(ReadStatusRegisterSPIFlash(spi_dev) & 0x1); //Wait Writing

        address++; count++;
        block.Address = address;
    }while(count < size);

    spi_dev->SSIENR = 0;

    SendOpcodeSPI(spi_dev, OP_WRDIS);
}
