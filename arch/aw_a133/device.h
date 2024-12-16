/* Copyright (c) 2024 by Genadi V. Zawidowski. All rights reserved. */
#pragma once
#ifndef HEADER_00003039_INCLUDED
#define HEADER_00003039_INCLUDED
#include <stdint.h>


/* IRQs */

typedef enum IRQn
{
    UART0_IRQn = 32,                                  /*!< UART  */
    UART1_IRQn = 33,                                  /*!< UART  */
    UART2_IRQn = 34,                                  /*!< UART  */
    UART3_IRQn = 35,                                  /*!< UART  */
    UART4_IRQn = 36,                                  /*!< UART  */
    TWI0_IRQn = 39,                                   /*!< TWI  */
    TWI1_IRQn = 40,                                   /*!< TWI  */
    TWI2_IRQn = 41,                                   /*!< TWI  */
    TWI3_IRQn = 42,                                   /*!< TWI  */
    I2S0_IRQn = 58,                                   /*!< I2S_PCM  */
    I2S1_IRQn = 59,                                   /*!< I2S_PCM  */
    I2S2_IRQn = 60,                                   /*!< I2S_PCM  */
    I2S3_IRQn = 62,                                   /*!< I2S_PCM  */
    R_UART_IRQn = 144,                                /*!< UART  */
    R_UART_IRQn = 144,                                /*!< UART R_UART */
    R_TWI0_IRQn = 145,                                /*!< TWI  */
    R_TWI1_IRQn = 146,                                /*!< TWI  */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define UART0_BASE ((uintptr_t) 0x05000000)           /*!< UART  Base */
#define UART1_BASE ((uintptr_t) 0x05000400)           /*!< UART  Base */
#define UART2_BASE ((uintptr_t) 0x05000800)           /*!< UART  Base */
#define UART3_BASE ((uintptr_t) 0x05000C00)           /*!< UART  Base */
#define UART4_BASE ((uintptr_t) 0x05001000)           /*!< UART  Base */
#define TWI0_BASE ((uintptr_t) 0x05002000)            /*!< TWI  Base */
#define TWI1_BASE ((uintptr_t) 0x05002400)            /*!< TWI  Base */
#define TWI2_BASE ((uintptr_t) 0x05002800)            /*!< TWI  Base */
#define TWI3_BASE ((uintptr_t) 0x05002C00)            /*!< TWI  Base */
#define I2S0_BASE ((uintptr_t) 0x05090000)            /*!< I2S_PCM  Base */
#define I2S1_BASE ((uintptr_t) 0x05091000)            /*!< I2S_PCM  Base */
#define I2S2_BASE ((uintptr_t) 0x05092000)            /*!< I2S_PCM  Base */
#define I2S3_BASE ((uintptr_t) 0x05093000)            /*!< I2S_PCM  Base */
#define R_UART_BASE ((uintptr_t) 0x07080000)          /*!< UART  Base */
#define R_UART_BASE ((uintptr_t) 0x07080000)          /*!< UART  Base */
#define R_TWI0_BASE ((uintptr_t) 0x07081400)          /*!< TWI  Base */
#define R_TWI1_BASE ((uintptr_t) 0x07081800)          /*!< TWI  Base */

#if __aarch64__
    #include <core64_ca.h>
#else
    #include <core_ca.h>
#endif

/*
 * @brief I2S_PCM
 */
/*!< I2S_PCM  */
typedef __PACKED_STRUCT I2S_PCM_Type
{
    __IO uint32_t I2S_PCM_CTL;                        /*!< Offset 0x000 I2S/PCM Control Register */
    __IO uint32_t I2S_PCM_FMT0;                       /*!< Offset 0x004 I2S/PCM Format Register 0 */
    __IO uint32_t I2S_PCM_FMT1;                       /*!< Offset 0x008 I2S/PCM Format Register 1 */
    __IO uint32_t I2S_PCM_ISTA;                       /*!< Offset 0x00C I2S/PCM Interrupt Status Register */
    __IO uint32_t I2S_PCM_RXFIFO;                     /*!< Offset 0x010 I2S/PCM RXFIFO Register */
    __IO uint32_t I2S_PCM_FCTL;                       /*!< Offset 0x014 I2S/PCM FIFO Control Register */
    __IO uint32_t I2S_PCM_FSTA;                       /*!< Offset 0x018 I2S/PCM FIFO Status Register */
    __IO uint32_t I2S_PCM_INT;                        /*!< Offset 0x01C I2S/PCM DMA & Interrupt Control Register */
    __IO uint32_t I2S_PCM_TXFIFO;                     /*!< Offset 0x020 I2S/PCM TXFIFO Register */
    __IO uint32_t I2S_PCM_CLKD;                       /*!< Offset 0x024 I2S/PCM Clock Divide Register */
    __IO uint32_t I2S_PCM_TXCNT;                      /*!< Offset 0x028 I2S/PCM TX Sample Counter Register */
    __IO uint32_t I2S_PCM_RXCNT;                      /*!< Offset 0x02C I2S/PCM RX Sample Counter Register */
    __IO uint32_t I2S_PCM_CHCFG;                      /*!< Offset 0x030 I2S/PCM Channel Configuration Register */
    __IO uint32_t I2S_PCM_TX0CHSEL;                   /*!< Offset 0x034 I2S/PCM TX0 Channel Select Register */
    __IO uint32_t I2S_PCM_TX1CHSEL;                   /*!< Offset 0x038 I2S/PCM TX1 Channel Select Register */
    __IO uint32_t I2S_PCM_TX2CHSEL;                   /*!< Offset 0x03C I2S/PCM TX2 Channel Select Register */
    __IO uint32_t I2S_PCM_TX3CHSEL;                   /*!< Offset 0x040 I2S/PCM TX3 Channel Select Register */
    __IO uint32_t I2S_PCM_TX0CHMAP0;                  /*!< Offset 0x044 I2S/PCM TX0 Channel Mapping Register0 */
    __IO uint32_t I2S_PCM_TX0CHMAP1;                  /*!< Offset 0x048 I2S/PCM TX0 Channel Mapping Register1 */
    __IO uint32_t I2S_PCM_TX1CHMAP0;                  /*!< Offset 0x04C I2S/PCM TX1 Channel Mapping Register0 */
    __IO uint32_t I2S_PCM_TX1CHMAP1;                  /*!< Offset 0x050 I2S/PCM TX1 Channel Mapping Register1 */
    __IO uint32_t I2S_PCM_TX2CHMAP0;                  /*!< Offset 0x054 I2S/PCM TX2 Channel Mapping Register0 */
    __IO uint32_t I2S_PCM_TX2CHMAP1;                  /*!< Offset 0x058 I2S/PCM TX2 Channel Mapping Register1 */
    __IO uint32_t I2S_PCM_TX3CHMAP0;                  /*!< Offset 0x05C I2S/PCM TX3 Channel Mapping Register0 */
    __IO uint32_t I2S_PCM_TX3CHMAP1;                  /*!< Offset 0x060 I2S/PCM TX3 Channel Mapping Register1 */
    __IO uint32_t I2S_PCM_RXCHSEL;                    /*!< Offset 0x064 I2S/PCM RX Channel Select Register */
    __IO uint32_t I2S_PCM_RXCHMAP [0x004];            /*!< Offset 0x068 I2S/PCM RX Channel Mapping Register0..3 */
         uint32_t reserved_0x078 [0x0002];
    __IO uint32_t MCLKCFG;                            /*!< Offset 0x080 ASRC MCLK Configuration Register */
    __IO uint32_t FsoutCFG;                           /*!< Offset 0x084 ASRC Out Sample Rate Configuration Register */
    __IO uint32_t FsinEXTCFG;                         /*!< Offset 0x088 ASRC Input Sample Pulse Extend Configuration Register */
    __IO uint32_t ASRCCFG;                            /*!< Offset 0x08C ASRC Enable Register */
    __IO uint32_t ASRCMANCFG;                         /*!< Offset 0x090 ASRC Manual Ratio Configuration Register */
    __IO uint32_t ASRCRATIOSTAT;                      /*!< Offset 0x094 ASRC Status Register */
    __IO uint32_t ASRCFIFOSTAT;                       /*!< Offset 0x098 ASRC FIFO Level Status Register */
    __IO uint32_t ASRCMBISTCFG;                       /*!< Offset 0x09C ASRC MBIST Test Configuration Register */
    __IO uint32_t ASRCMBISTSTAT;                      /*!< Offset 0x0A0 ASRC MBIST Test Status Register */
} I2S_PCM_TypeDef; /* size of structure = 0x0A4 */
/*
 * @brief TWI
 */
/*!< TWI  */
typedef __PACKED_STRUCT TWI_Type
{
    __IO uint32_t TWI_ADDR;                           /*!< Offset 0x000 TWI Slave Address Register */
    __IO uint32_t TWI_XADDR;                          /*!< Offset 0x004 TWI Extended Slave Address Register */
    __IO uint32_t TWI_DATA;                           /*!< Offset 0x008 TWI Data Byte Register */
    __IO uint32_t TWI_CNTR;                           /*!< Offset 0x00C TWI Control Register */
    __IO uint32_t TWI_STAT;                           /*!< Offset 0x010 TWI Status Register */
    __IO uint32_t TWI_CCR;                            /*!< Offset 0x014 TWI Clock Control Register */
    __IO uint32_t TWI_SRST;                           /*!< Offset 0x018 TWI Software Reset Register */
    __IO uint32_t TWI_EFR;                            /*!< Offset 0x01C TWI Enhance Feature Register */
    __IO uint32_t TWI_LCR;                            /*!< Offset 0x020 TWI Line Control Register */
         uint32_t reserved_0x024 [0x0077];
    __IO uint32_t TWI_DRV_CTRL;                       /*!< Offset 0x200 TWI_DRV Control Register */
    __IO uint32_t TWI_DRV_CFG;                        /*!< Offset 0x204 TWI_DRV Transmission Configuration Register */
    __IO uint32_t TWI_DRV_SLV;                        /*!< Offset 0x208 TWI_DRV Slave ID Register */
    __IO uint32_t TWI_DRV_FMT;                        /*!< Offset 0x20C TWI_DRV Packet Format Register */
    __IO uint32_t TWI_DRV_BUS_CTRL;                   /*!< Offset 0x210 TWI_DRV Bus Control Register */
    __IO uint32_t TWI_DRV_INT_CTRL;                   /*!< Offset 0x214 TWI_DRV Interrupt Control Register */
    __IO uint32_t TWI_DRV_DMA_CFG;                    /*!< Offset 0x218 TWI_DRV DMA Configure Register */
    __IO uint32_t TWI_DRV_FIFO_CON;                   /*!< Offset 0x21C TWI_DRV FIFO Content Register */
         uint32_t reserved_0x220 [0x0038];
    __IO uint32_t TWI_DRV_SEND_FIFO_ACC;              /*!< Offset 0x300 TWI_DRV Send Data FIFO Access Register */
    __IO uint32_t TWI_DRV_RECV_FIFO_ACC;              /*!< Offset 0x304 TWI_DRV Receive Data FIFO Access Register */
         uint32_t reserved_0x308 [0x003E];
} TWI_TypeDef; /* size of structure = 0x400 */
/*
 * @brief UART
 */
/*!< UART  */
typedef __PACKED_STRUCT UART_Type
{
    __IO uint32_t UART_RBR_THR_DLL;                   /*!< Offset 0x000 UART Receive Buffer Register/Transmit Holding Register */
    __IO uint32_t UART_DLH_IER;                       /*!< Offset 0x004  */
    __IO uint32_t UART_IIR_FCR;                       /*!< Offset 0x008 UART Interrupt Identity Register/UART FIFO Control Register */
    __IO uint32_t UART_LCR;                           /*!< Offset 0x00C UART Line Control Register */
    __IO uint32_t UART_MCR;                           /*!< Offset 0x010 UART Modem Control Register */
    __IO uint32_t UART_LSR;                           /*!< Offset 0x014 UART Line Status Register */
    __IO uint32_t UART_MSR;                           /*!< Offset 0x018 UART Modem Status Register */
    __IO uint32_t UART_SCH;                           /*!< Offset 0x01C UART Scratch Register */
         uint32_t reserved_0x020 [0x0017];
    __IO uint32_t UART_USR;                           /*!< Offset 0x07C UART Status Register */
    __IO uint32_t UART_TFL;                           /*!< Offset 0x080 UART Transmit FIFO Level Register */
    __IO uint32_t UART_RFL;                           /*!< Offset 0x084 UART Receive FIFO Level Register */
    __IO uint32_t UART_HSK;                           /*!< Offset 0x088 UART DMA Handshake Configuration Register */
         uint32_t reserved_0x08C [0x0009];
    __IO uint32_t UART_DBG_DLL;                       /*!< Offset 0x0B0 UART Debug DLL Register */
    __IO uint32_t UART_DBG_DLH;                       /*!< Offset 0x0B4 UART Debug DLH Register */
         uint32_t reserved_0x0B8 [0x0002];
    __IO uint32_t UART_485_CTL;                       /*!< Offset 0x0C0 UART RS485 Control and Status Register */
    __IO uint32_t RS485_ADDR_MATCH;                   /*!< Offset 0x0C4 UART RS485 Addres Match Register  */
    __IO uint32_t BUS_IDLE_CHECK;                     /*!< Offset 0x0C8 UART RS485 Bus Idle Check Register */
    __IO uint32_t TX_DLY;                             /*!< Offset 0x0CC UART TX Delay Register */
         uint32_t reserved_0x0D0 [0x00CC];
} UART_TypeDef; /* size of structure = 0x400 */


/* Defines */



/* Access pointers */

#define UART0 ((UART_TypeDef *) UART0_BASE)           /*!< UART0  register set access pointer */
#define UART1 ((UART_TypeDef *) UART1_BASE)           /*!< UART1  register set access pointer */
#define UART2 ((UART_TypeDef *) UART2_BASE)           /*!< UART2  register set access pointer */
#define UART3 ((UART_TypeDef *) UART3_BASE)           /*!< UART3  register set access pointer */
#define UART4 ((UART_TypeDef *) UART4_BASE)           /*!< UART4  register set access pointer */
#define TWI0 ((TWI_TypeDef *) TWI0_BASE)              /*!< TWI0  register set access pointer */
#define TWI1 ((TWI_TypeDef *) TWI1_BASE)              /*!< TWI1  register set access pointer */
#define TWI2 ((TWI_TypeDef *) TWI2_BASE)              /*!< TWI2  register set access pointer */
#define TWI3 ((TWI_TypeDef *) TWI3_BASE)              /*!< TWI3  register set access pointer */
#define I2S0 ((I2S_PCM_TypeDef *) I2S0_BASE)          /*!< I2S0  register set access pointer */
#define I2S1 ((I2S_PCM_TypeDef *) I2S1_BASE)          /*!< I2S1  register set access pointer */
#define I2S2 ((I2S_PCM_TypeDef *) I2S2_BASE)          /*!< I2S2  register set access pointer */
#define I2S3 ((I2S_PCM_TypeDef *) I2S3_BASE)          /*!< I2S3  register set access pointer */
#define R_UART ((UART_TypeDef *) R_UART_BASE)         /*!< R_UART  register set access pointer */
#define R_UART ((UART_TypeDef *) R_UART_BASE)         /*!< R_UART  register set access pointer */
#define R_TWI0 ((TWI_TypeDef *) R_TWI0_BASE)          /*!< R_TWI0  register set access pointer */
#define R_TWI1 ((TWI_TypeDef *) R_TWI1_BASE)          /*!< R_TWI1  register set access pointer */


#endif /* HEADER_00003039_INCLUDED */
