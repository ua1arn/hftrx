/* Copyright (c) 2024 by Genadi V. Zawidowski. All rights reserved. */
#pragma once
#ifndef HEADER__2F3EAF83_E6C0_48a6_A374_C924EDE2E572__INCLUDED
#define HEADER__2F3EAF83_E6C0_48a6_A374_C924EDE2E572__INCLUDED
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* IRQs */

typedef enum IRQn
{
    SGI0_IRQn = 0,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI1_IRQn = 1,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI2_IRQn = 2,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI3_IRQn = 3,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI4_IRQn = 4,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI5_IRQn = 5,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI6_IRQn = 6,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI7_IRQn = 7,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI8_IRQn = 8,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI9_IRQn = 9,                                    /*!< GIC_INTERFACE GIC CPU IF */
    SGI10_IRQn = 10,                                  /*!< GIC_INTERFACE GIC CPU IF */
    SGI11_IRQn = 11,                                  /*!< GIC_INTERFACE GIC CPU IF */
    SGI12_IRQn = 12,                                  /*!< GIC_INTERFACE GIC CPU IF */
    SGI13_IRQn = 13,                                  /*!< GIC_INTERFACE GIC CPU IF */
    SGI14_IRQn = 14,                                  /*!< GIC_INTERFACE GIC CPU IF */
    SGI15_IRQn = 15,                                  /*!< GIC_INTERFACE GIC CPU IF */
    VirtualMaintenanceInterrupt_IRQn = 25,            /*!< GIC_INTERFACE GIC CPU IF */
    HypervisorTimer_IRQn = 26,                        /*!< GIC_INTERFACE GIC CPU IF */
    VirtualTimer_IRQn = 27,                           /*!< GIC_INTERFACE GIC CPU IF */
    Legacy_nFIQ_IRQn = 28,                            /*!< GIC_INTERFACE GIC CPU IF */
    SecurePhysicalTimer_IRQn = 29,                    /*!< GIC_INTERFACE GIC CPU IF */
    NonSecurePhysicalTimer_IRQn = 30,                 /*!< GIC_INTERFACE GIC CPU IF */
    Legacy_nIRQ_IRQn = 31,                            /*!< GIC_INTERFACE GIC CPU IF */
    UART0_IRQn = 148,                                 /*!< UART  */
    UART1_IRQn = 149,                                 /*!< UART  */
    UART2_IRQn = 150,                                 /*!< UART  */
    UART3_IRQn = 151,                                 /*!< UART  */
    UART4_IRQn = 152,                                 /*!< UART  */
    UART5_IRQn = 153,                                 /*!< UART  */
    UART6_IRQn = 154,                                 /*!< UART  */
    UART7_IRQn = 155,                                 /*!< UART  */
    UART8_IRQn = 156,                                 /*!< UART  */
    UART9_IRQn = 157,                                 /*!< UART  */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define GIC_BASE ((uintptr_t) 0xFD400000)             /*!< GIC  Base */
#define GIC_DISTRIBUTOR_BASE ((uintptr_t) 0xFD401000) /*!< GIC_DISTRIBUTOR GIC DISTRIBUTOR Base */
#define GIC_INTERFACE_BASE ((uintptr_t) 0xFD402000)   /*!< GIC_INTERFACE GIC CPU IF Base */
#define GICVSELF_BASE ((uintptr_t) 0xFD404000)        /*!< GICV  Base */
#define GICV_BASE ((uintptr_t) 0xFD405000)            /*!< GICV  Base */
#define UART0_BASE ((uintptr_t) 0xFDD50000)           /*!< UART  Base */
#define UART1_BASE ((uintptr_t) 0xFE650000)           /*!< UART  Base */
#define UART2_BASE ((uintptr_t) 0xFE660000)           /*!< UART  Base */
#define UART3_BASE ((uintptr_t) 0xFE670000)           /*!< UART  Base */
#define UART4_BASE ((uintptr_t) 0xFE680000)           /*!< UART  Base */
#define UART5_BASE ((uintptr_t) 0xFE690000)           /*!< UART  Base */
#define UART6_BASE ((uintptr_t) 0xFE6A0000)           /*!< UART  Base */
#define UART7_BASE ((uintptr_t) 0xFE6B0000)           /*!< UART  Base */
#define UART8_BASE ((uintptr_t) 0xFE6C0000)           /*!< UART  Base */
#define UART9_BASE ((uintptr_t) 0xFE6D0000)           /*!< UART  Base */

#if __aarch64__
    #include <core64_ca.h>
#else
    #include <core_ca.h>
#endif

/*
 * @brief GICV
 */
/*!< GICV  */
typedef struct GICV_Type
{
    __IO uint32_t GICH_HCR;                           /*!< Offset 0x000 RW 0x00000000 Hypervisor Control Register */
    __IO uint32_t GICH_VTR;                           /*!< Offset 0x004 RO 0x90000003 VGIC Type Register, GICH_VTR on page 3-13 */
    __IO uint32_t GICH_VMCR;                          /*!< Offset 0x008 RW 0x004C0000 Virtual Machine Control Register */
         RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    __IO uint32_t GICH_MISR;                          /*!< Offset 0x010 RO 0x00000000 Maintenance Interrupt Status Register */
         RESERVED(0x014[0x0020 - 0x0014], uint8_t)
    __IO uint32_t GICH_EISR0;                         /*!< Offset 0x020 RO 0x00000000 End of Interrupt Status Register */
         RESERVED(0x024[0x0030 - 0x0024], uint8_t)
    __IO uint32_t GICH_ELSR0;                         /*!< Offset 0x030 RO 0x0000000F Empty List register Status Register */
         RESERVED(0x034[0x00F0 - 0x0034], uint8_t)
    __IO uint32_t GICH_APR0;                          /*!< Offset 0x0F0 RW 0x00000000 Active Priority Register */
         RESERVED(0x0F4[0x0100 - 0x00F4], uint8_t)
    __IO uint32_t GICH_LR0;                           /*!< Offset 0x100 RW 0x00000000 List Register 0 */
    __IO uint32_t GICH_LR1;                           /*!< Offset 0x104 RW 0x00000000 List Register 1 */
    __IO uint32_t GICH_LR2;                           /*!< Offset 0x108 RW 0x00000000 List Register 2 */
    __IO uint32_t GICH_LR3;                           /*!< Offset 0x10C RW 0x00000000 List Register 3 */
         RESERVED(0x110[0x0200 - 0x0110], uint8_t)
} GICV_TypeDef; /* size of structure = 0x200 */
/*
 * @brief UART
 */
/*!< UART  */
typedef struct UART_Type
{
    __IO uint32_t UART_RBR_THR_DLL;                   /*!< Offset 0x000 UART Receive Buffer Register/Transmit Holding Register */
    __IO uint32_t UART_DLH_IER;                       /*!< Offset 0x004  */
    __IO uint32_t UART_IIR_FCR;                       /*!< Offset 0x008 UART Interrupt Identity Register/UART FIFO Control Register */
    __IO uint32_t UART_LCR;                           /*!< Offset 0x00C UART Line Control Register */
    __IO uint32_t UART_MCR;                           /*!< Offset 0x010 UART Modem Control Register */
    __IO uint32_t UART_LSR;                           /*!< Offset 0x014 UART Line Status Register */
    __IO uint32_t UART_MSR;                           /*!< Offset 0x018 UART Modem Status Register */
    __IO uint32_t UART_SCH;                           /*!< Offset 0x01C UART Scratch Register */
         RESERVED(0x020[0x0030 - 0x0020], uint8_t)
    __IO uint32_t UART_SRBR_STHR;                     /*!< Offset 0x030 W 0x00000000 Shadow Receive Buffer Register, Shadow Transmit Holding Register */
         RESERVED(0x034[0x0070 - 0x0034], uint8_t)
    __IO uint32_t UART_FAR;                           /*!< Offset 0x070 W 0x00000000 FIFO Access Register */
    __IO uint32_t UART_TFR;                           /*!< Offset 0x074 W 0x00000000 Transmit FIFO Read */
    __IO uint32_t UART_RFW;                           /*!< Offset 0x078 W 0x00000000 Receive FIFO write */
    __IO uint32_t UART_USR;                           /*!< Offset 0x07C W 0x00000006 UART Status Register */
    __IO uint32_t UART_TFL;                           /*!< Offset 0x080 W 0x00000000 Transmit FIFO level */
    __IO uint32_t UART_RFL;                           /*!< Offset 0x084 W 0x00000000 Receive FIFO level */
    __IO uint32_t UART_SRR;                           /*!< Offset 0x088 W 0x00000000 Software Reset Register */
    __IO uint32_t UART_SRTS;                          /*!< Offset 0x08C W 0x00000000 Shadow Request to Send */
    __IO uint32_t UART_SBCR;                          /*!< Offset 0x090 W 0x00000000 Shadow Break Control Register */
    __IO uint32_t UART_SDMAM;                         /*!< Offset 0x094 W 0x00000000 Shadow DMA Mode */
    __IO uint32_t UART_SFE;                           /*!< Offset 0x098 W 0x00000000 Shadow FIFO enable */
    __IO uint32_t UART_SRT;                           /*!< Offset 0x09C W 0x00000000 Shadow RCVR Trigger */
    __IO uint32_t UART_STET;                          /*!< Offset 0x0A0 W 0x00000000 Shadow TX Empty Trigger */
    __IO uint32_t UART_HTX;                           /*!< Offset 0x0A4 W 0x00000000 Halt TX */
    __IO uint32_t UART_DMASA;                         /*!< Offset 0x0A8 W 0x00000000 DMA Software Acknowledge */
         RESERVED(0x0AC[0x00F4 - 0x00AC], uint8_t)
    __IO uint32_t UART_CPR;                           /*!< Offset 0x0F4 W 0x00043FF2 Component Parameter Register */
    __IO uint32_t UART_UCV;                           /*!< Offset 0x0F8 W 0x3330382A UART Component Version */
    __IO uint32_t UART_CTR;                           /*!< Offset 0x0FC W 0x44570110 Component Type Register */
} UART_TypeDef; /* size of structure = 0x100 */


/* Defines */



/* Access pointers */

#define GICVSELF ((GICV_TypeDef *) GICVSELF_BASE)     /*!< GICVSELF  register set access pointer */
#define GICV ((GICV_TypeDef *) GICV_BASE)             /*!< GICV  register set access pointer */
#define UART0 ((UART_TypeDef *) UART0_BASE)           /*!< UART0  register set access pointer */
#define UART1 ((UART_TypeDef *) UART1_BASE)           /*!< UART1  register set access pointer */
#define UART2 ((UART_TypeDef *) UART2_BASE)           /*!< UART2  register set access pointer */
#define UART3 ((UART_TypeDef *) UART3_BASE)           /*!< UART3  register set access pointer */
#define UART4 ((UART_TypeDef *) UART4_BASE)           /*!< UART4  register set access pointer */
#define UART5 ((UART_TypeDef *) UART5_BASE)           /*!< UART5  register set access pointer */
#define UART6 ((UART_TypeDef *) UART6_BASE)           /*!< UART6  register set access pointer */
#define UART7 ((UART_TypeDef *) UART7_BASE)           /*!< UART7  register set access pointer */
#define UART8 ((UART_TypeDef *) UART8_BASE)           /*!< UART8  register set access pointer */
#define UART9 ((UART_TypeDef *) UART9_BASE)           /*!< UART9  register set access pointer */

#ifdef __cplusplus
 }
#endif /* __cplusplus */

#endif /* HEADER__2F3EAF83_E6C0_48a6_A374_C924EDE2E572__INCLUDED */
