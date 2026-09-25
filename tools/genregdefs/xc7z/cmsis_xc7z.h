/* Copyright (c) 2024 by Genadi V. Zawidowski. All rights reserved. */
#pragma once
#ifndef HEADER__193A12F0_6292_4f92_BE20_2DBDD3EB1B8A__INCLUDED
#define HEADER__193A12F0_6292_4f92_BE20_2DBDD3EB1B8A__INCLUDED
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* IRQs */

typedef enum IRQn
{
    SGI0_IRQn = 0,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI1_IRQn = 1,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI2_IRQn = 2,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI3_IRQn = 3,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI4_IRQn = 4,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI5_IRQn = 5,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI6_IRQn = 6,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI7_IRQn = 7,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI8_IRQn = 8,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI9_IRQn = 9,                                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI10_IRQn = 10,                                  /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI11_IRQn = 11,                                  /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI12_IRQn = 12,                                  /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI13_IRQn = 13,                                  /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI14_IRQn = 14,                                  /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SGI15_IRQn = 15,                                  /*!< GIC_INTERFACE GICC GIC CPU Interface */
    VirtualMaintenanceInterrupt_IRQn = 25,            /*!< GIC_INTERFACE GICC GIC CPU Interface */
    HypervisorTimer_IRQn = 26,                        /*!< GIC_INTERFACE GICC GIC CPU Interface */
    VirtualTimer_IRQn = 27,                           /*!< GIC_INTERFACE GICC GIC CPU Interface */
    Legacy_nFIQ_IRQn = 28,                            /*!< GIC_INTERFACE GICC GIC CPU Interface */
    SecurePhysicalTimer_IRQn = 29,                    /*!< GIC_INTERFACE GICC GIC CPU Interface */
    NonSecurePhysicalTimer_IRQn = 30,                 /*!< GIC_INTERFACE GICC GIC CPU Interface */
    Legacy_nIRQ_IRQn = 31,                            /*!< GIC_INTERFACE GICC GIC CPU Interface */

    MAX_IRQ_n,
    Force_IRQn_enum_size = 1048 /* Dummy entry to ensure IRQn_Type is more than 8 bits. Otherwise GIC init loop would fail */
} IRQn_Type;


/* Peripheral and RAM base address */

#define GIC_BASE ((uintptr_t) 0xF8F00000)             /*!< GIC  Base */
#define GIC_INTERFACE_BASE ((uintptr_t) 0xF8F00100)   /*!< GIC_INTERFACE GICC GIC CPU Interface Base */
#define GIC_DISTRIBUTOR_BASE ((uintptr_t) 0xF8F01000) /*!< GIC_DISTRIBUTOR GICD Base */
#define GICVSELF_BASE ((uintptr_t) 0xF8F04000)        /*!< GICV  Base */
#define GICV_BASE ((uintptr_t) 0xF8F05000)            /*!< GICV  Base */

#include <core_ca.h>

/*
 * @brief GICV
 */
/*!< GICV  */
typedef struct GICV_Type
{
    __IOM uint32_t GICH_HCR;                          /*!< Offset 0x000 RW 0x00000000 Hypervisor Control Register */
    __IOM uint32_t GICH_VTR;                          /*!< Offset 0x004 RO 0x90000003 VGIC Type Register, GICH_VTR on page 3-13 */
    __IOM uint32_t GICH_VMCR;                         /*!< Offset 0x008 RW 0x004C0000 Virtual Machine Control Register */
         RESERVED(0x00C[0x0010 - 0x000C], uint8_t)
    __IOM uint32_t GICH_MISR;                         /*!< Offset 0x010 RO 0x00000000 Maintenance Interrupt Status Register */
         RESERVED(0x014[0x0020 - 0x0014], uint8_t)
    __IOM uint32_t GICH_EISR0;                        /*!< Offset 0x020 RO 0x00000000 End of Interrupt Status Register */
         RESERVED(0x024[0x0030 - 0x0024], uint8_t)
    __IOM uint32_t GICH_ELSR0;                        /*!< Offset 0x030 RO 0x0000000F Empty List register Status Register */
         RESERVED(0x034[0x00F0 - 0x0034], uint8_t)
    __IOM uint32_t GICH_APR0;                         /*!< Offset 0x0F0 RW 0x00000000 Active Priority Register */
         RESERVED(0x0F4[0x0100 - 0x00F4], uint8_t)
    __IOM uint32_t GICH_LR0;                          /*!< Offset 0x100 RW 0x00000000 List Register 0 */
    __IOM uint32_t GICH_LR1;                          /*!< Offset 0x104 RW 0x00000000 List Register 1 */
    __IOM uint32_t GICH_LR2;                          /*!< Offset 0x108 RW 0x00000000 List Register 2 */
    __IOM uint32_t GICH_LR3;                          /*!< Offset 0x10C RW 0x00000000 List Register 3 */
         RESERVED(0x110[0x0200 - 0x0110], uint8_t)
} GICV_TypeDef; /* size of structure = 0x200 */


/* Defines */



/* Access pointers */

#define GICVSELF ((GICV_TypeDef *) GICVSELF_BASE)     /*!< GICVSELF  register set access pointer */
#define GICV ((GICV_TypeDef *) GICV_BASE)             /*!< GICV  register set access pointer */

#ifdef __cplusplus
 }
#endif /* __cplusplus */

#endif /* HEADER__193A12F0_6292_4f92_BE20_2DBDD3EB1B8A__INCLUDED */
