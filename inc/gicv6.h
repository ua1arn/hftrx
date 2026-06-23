/*
 * gicv6.h
 *
 *  Created on: May 31, 2026
 *      Author: Gena
 */

#ifndef INC_GICV6_H_
#define INC_GICV6_H_

#if ! defined (__aarch64__)
#include "core32_ca.h"
#endif

/* ##########################  GIC functions  ###################################### */
#if (defined(__GIC_PRESENT) && (__GIC_PRESENT == 1U)) || \
     defined(DOXYGEN)

#if defined (GIC_REDISTRIBUTOR_BASE)

/*
 * @brief GICR
 */
/*!< GICR GIC Redistributor  */
typedef struct
{
    __IOM uint32_t CTLR;                         /*!< Offset 0x000 Redistributor Control Register */
    __IM  uint32_t IIDR;                         /*!< Offset 0x004  */
    __IOM uint32_t TYPER;                        /*!< Offset 0x008 4.4.2 Interrupt Controller Type Register, TYPER on page 4-128 */
         RESERVED(0x00C[0x0014 - 0x000C], uint8_t)
    __IOM uint32_t WAKER;                        /*!< Offset 0x014  */
         RESERVED(0x018[0x0020 - 0x0018], uint8_t)
    __IOM uint32_t FCTLR;                        /*!< Offset 0x020  */
    __IOM uint32_t PWRR;                         /*!< Offset 0x024  */
    __IOM uint32_t CLASS;                        /*!< Offset 0x028  */
         RESERVED(0x02C[0x0040 - 0x002C], uint8_t)
    __IOM uint64_t SETLPIR;                      /*!< Offset 0x040  */
    __IOM uint64_t CLRLPIR;                      /*!< Offset 0x048  */
         RESERVED(0x050[0x0070 - 0x0050], uint8_t)
    __IOM uint64_t PROPBASER;                    /*!< Offset 0x070 Redistributor Properties Base Address Register */
    __IOM uint64_t PENDBASER;                    /*!< Offset 0x078 Redistributor LPI Pending Table Base Address Register */
         RESERVED(0x080[0x00A0 - 0x0080], uint8_t)
    __IOM uint64_t INVLPIR;                      /*!< Offset 0x0A0  */
    __IOM uint64_t INVALLR;                      /*!< Offset 0x0A8  */
         RESERVED(0x0B0[0x00C0 - 0x00B0], uint8_t)
    __IOM uint32_t SYNCR;                        /*!< Offset 0x0C0  */
         RESERVED(0x0C4[0xFFD0 - 0x00C4], uint8_t)
    __IM  uint32_t PIDR4;                        /*!< Offset 0xFFD0 Peripheral ID 4 Register */
    __IM  uint32_t PIDR5;                        /*!< Offset 0xFFD4 Peripheral ID 5 Register */
    __IM  uint32_t PIDR6;                        /*!< Offset 0xFFD8 Peripheral ID 6 Register */
    __IM  uint32_t PIDR7;                        /*!< Offset 0xFFDC Peripheral ID 7 Register */
    __IM  uint32_t PIDR0;                        /*!< Offset 0xFFE0 Peripheral ID 0 Register */
    __IM  uint32_t PIDR1;                        /*!< Offset 0xFFE4 Peripheral ID 1 Register */
    __IM  uint32_t PIDR2;                        /*!< Offset 0xFFE8 Peripheral ID 2 Register */
    __IM  uint32_t PIDR3;                        /*!< Offset 0xFFEC Peripheral ID 3 Register */
    __IM  uint32_t CIDR0;                        /*!< Offset 0xFFF0 Component ID 0 Register */
    __IM  uint32_t CIDR1;                        /*!< Offset 0xFFF4 Component ID 1 Register */
    __IM  uint32_t CIDR2;                        /*!< Offset 0xFFF8 Component ID 2 Register */
    __IM  uint32_t CIDR3;                        /*!< Offset 0xFFFC Component ID 3 Register */
} GICRedistributor_Type; /* size of structure = 0x10000 */

#define GICRedistributor      ((GICRedistributor_Type      *)     GIC_REDISTRIBUTOR_BASE ) /*!< \brief GIC ReDistributor register set access pointer */

// https://developer.arm.com/documentation/ddi0601/2025-12/External-Registers/GICR-CTLR--Redistributor-Control-Register?lang=en
__STATIC_FORCEINLINE void GIC_RedistributorWait(void)
{
	while ((GICRedistributor->CTLR & (UINT32_C(1) << 3)) != 0)	// Bit 3, not a 31
			;
}

#endif /* GIC_REDISTRIBUTOR_BASE */

#if defined (GIC_DISTRIBUTOR_BASE)
/** \brief  Structure type to access the Generic Interrupt Controller Distributor (GICD)
*/
typedef struct
{
    __IOM uint32_t CTLR;                         /*!< Offset 0x000 Distributor Control Register */
    __IM  uint32_t TYPER;                        /*!< Offset 0x004 Distributor Controller Type Register */
    __IM  uint32_t IIDR;                         /*!< Offset 0x008 Distributor Implementer Identification Register */
         RESERVED(0x00C[0x0020 - 0x000C], uint8_t)
    __IOM uint32_t FCTLR;                        /*!< Offset 0x020 Function Control Register */
    __IOM uint32_t SAC;                          /*!< Offset 0x024 Secure Access Control Register */
         RESERVED(0x028[0x0040 - 0x0028], uint8_t)
    __OM  uint32_t SETSPI_NSR;                   /*!< Offset 0x040 Non-secure SPI Set Register */
         RESERVED(0x044[0x0048 - 0x0044], uint8_t)
    __OM  uint32_t CLRSPI_NSR;                   /*!< Offset 0x048 Non-secure SPI Clear Register */
         RESERVED(0x04C[0x0050 - 0x004C], uint8_t)
    __OM  uint32_t SETSPI_SR;                    /*!< Offset 0x050 Secure SPI Set Register */
         RESERVED(0x054[0x0058 - 0x0054], uint8_t)
    __OM  uint32_t CLRSPI_SR;                    /*!< Offset 0x058 Non-secure SPI Set Register */
         RESERVED(0x05C[0x0080 - 0x005C], uint8_t)
    __IOM uint32_t IGROUPR [0x020];              /*!< Offset 0x080 Interrupt Group Registers */
    __IOM uint32_t ISENABLER [0x020];            /*!< Offset 0x100 Interrupt Set-Enable Registers */
    __IOM uint32_t ICENABLER [0x020];            /*!< Offset 0x180 Interrupt Clear-Enable Registers */
    __IOM uint32_t ISPENDR [0x020];              /*!< Offset 0x200 Interrupt Set-Pending Registers */
    __IOM uint32_t ICPENDR [0x020];              /*!< Offset 0x280 Interrupt Clear-Pending Registers */
    __IOM uint32_t ISACTIVER [0x020];            /*!< Offset 0x300 Interrupt Set-Active Registers */
    __IOM uint32_t ICACTIVER [0x020];            /*!< Offset 0x380 Interrupt Clear-Active Registers */
    __IOM uint32_t IPRIORITYR [0x100];           /*!< Offset 0x400 Interrupt Priority Registers */
         RESERVED(0x800[0x0C00 - 0x0800], uint8_t)
    __IOM uint32_t ICFGR [0x040];                /*!< Offset 0xC00 Interrupt Configuration Registers */
    __IOM uint32_t IGRPMODR [0x020];             /*!< Offset 0xD00 Interrupt Group Modifier Registers */
         RESERVED(0xD80[0x0E00 - 0x0D80], uint8_t)
    __IOM uint32_t NSACR [0x040];                /*!< Offset 0xE00 Non-secure Access Control Registers */
         RESERVED(0xF00[0x6100 - 0x0F00], uint8_t)
    __IOM uint64_t IROUTER [0x3DC];              /*!< Offset 0x6100 Interrupt Routing Registers. */
         RESERVED(0x7FE0[0xC000 - 0x7FE0], uint8_t)
    __IOM uint32_t CHIPSR;                       /*!< Offset 0xC000 Chip Status Register */
    __IOM uint32_t DCHIPR;                       /*!< Offset 0xC004 Default Chip Register */
    __IOM uint64_t CHIPRn [0x004];               /*!< Offset 0xC008 Chip Registers */
         RESERVED(0xC028[0xE008 - 0xC028], uint8_t)
    __IOM uint32_t ICLARn [0x004];               /*!< Offset 0xE008 The first register is ICLAR2. 4.2.9 Interrupt Class Registers, ICLARn on page 4-118 */
         RESERVED(0xE018[0xE108 - 0xE018], uint8_t)
    __IOM uint32_t IERRRn;                       /*!< Offset 0xE108 The first register is IERRR1. 4.2.10 Interrupt Error Registers, IERRRn on page 4-119 */
         RESERVED(0xE10C[0xF000 - 0xE10C], uint8_t)
    __IM  uint64_t CFGID;                        /*!< Offset 0xF000 Configuration ID Register */
         RESERVED(0xF008[0xFFD0 - 0xF008], uint8_t)
    __IM  uint32_t PIDR4;                        /*!< Offset 0xFFD0 Peripheral ID 4 Register */
    __IM  uint32_t PIDR5;                        /*!< Offset 0xFFD4 Peripheral ID 5 Register */
    __IM  uint32_t PIDR6;                        /*!< Offset 0xFFD8 Peripheral ID 6 Register */
    __IM  uint32_t PIDR7;                        /*!< Offset 0xFFDC Peripheral ID 7 Register */
    __IM  uint32_t PIDR0;                        /*!< Offset 0xFFE0 Peripheral ID 0 Register */
    __IM  uint32_t PIDR1;                        /*!< Offset 0xFFE4 Peripheral ID 1 Register */
    __IM  uint32_t PIDR2;                        /*!< Offset 0xFFE8 Peripheral ID 2 Register */
    __IM  uint32_t PIDR3;                        /*!< Offset 0xFFEC Peripheral ID 3 Register */
    __IM  uint32_t CIDR0;                        /*!< Offset 0xFFF0 Component ID 0 Register */
    __IM  uint32_t CIDR1;                        /*!< Offset 0xFFF4 Component ID 1 Register */
    __IM  uint32_t CIDR2;                        /*!< Offset 0xFFF8 Component ID 2 Register */
    __IM  uint32_t CIDR3;                        /*!< Offset 0xFFFC Component ID 3 Register */
}  GICDistributor_Type;

#define GICDistributor      ((GICDistributor_Type      *)     GIC_DISTRIBUTOR_BASE ) /*!< \brief GIC Distributor register set access pointer */

/* GICDistributor CTLR Register */
#define GICDistributor_CTLR_EnableGrp0_Pos    0U                                                   /*!< GICDistributor CTLR: EnableGrp0 Position */
#define GICDistributor_CTLR_EnableGrp0_Msk    (0x1U /*<< GICDistributor_CTLR_EnableGrp0_Pos*/)     /*!< GICDistributor CTLR: EnableGrp0 Mask */
#define GICDistributor_CTLR_EnableGrp0(x)     (((uint32_t)(((uint32_t)(x)) /*<< GICDistributor_CTLR_EnableGrp0_Pos*/)) & GICDistributor_CTLR_EnableGrp0_Msk)

#define GICDistributor_CTLR_EnableGrp1_Pos    1U                                                   /*!< GICDistributor CTLR: EnableGrp1 Position */
#define GICDistributor_CTLR_EnableGrp1_Msk    (0x1U << GICDistributor_CTLR_EnableGrp1_Pos)         /*!< GICDistributor CTLR: EnableGrp1 Mask */
#define GICDistributor_CTLR_EnableGrp1(x)     (((uint32_t)(((uint32_t)(x)) << GICDistributor_CTLR_EnableGrp1_Pos)) & GICDistributor_CTLR_EnableGrp1_Msk)

#define GICDistributor_CTLR_ARE_Pos           4U                                                   /*!< GICDistributor CTLR: ARE Position */
#define GICDistributor_CTLR_ARE_Msk           (0x1U << GICDistributor_CTLR_ARE_Pos)                /*!< GICDistributor CTLR: ARE Mask */
#define GICDistributor_CTLR_ARE(x)            (((uint32_t)(((uint32_t)(x)) << GICDistributor_CTLR_ARE_Pos)) & GICDistributor_CTLR_ARE_Msk)

#define GICDistributor_CTLR_DC_Pos            6U                                                   /*!< GICDistributor CTLR: DC Position */
#define GICDistributor_CTLR_DC_Msk            (0x1U << GICDistributor_CTLR_DC_Pos)                 /*!< GICDistributor CTLR: DC Mask */
#define GICDistributor_CTLR_DC(x)             (((uint32_t)(((uint32_t)(x)) << GICDistributor_CTLR_DC_Pos)) & GICDistributor_CTLR_DC_Msk)

#define GICDistributor_CTLR_EINWF_Pos         7U                                                   /*!< GICDistributor CTLR: EINWF Position */
#define GICDistributor_CTLR_EINWF_Msk         (0x1U << GICDistributor_CTLR_EINWF_Pos)              /*!< GICDistributor CTLR: EINWF Mask */
#define GICDistributor_CTLR_EINWF(x)          (((uint32_t)(((uint32_t)(x)) << GICDistributor_CTLR_EINWF_Pos)) & GICDistributor_CTLR_EINWF_Msk)

#define GICDistributor_CTLR_RWP_Pos           31U                                                  /*!< GICDistributor CTLR: RWP Position */
#define GICDistributor_CTLR_RWP_Msk           (0x1U << GICDistributor_CTLR_RWP_Pos)                /*!< GICDistributor CTLR: RWP Mask */
#define GICDistributor_CTLR_RWP(x)            (((uint32_t)(((uint32_t)(x)) << GICDistributor_CTLR_RWP_Pos)) & GICDistributor_CTLR_RWP_Msk)

/* GICDistributor TYPER Register */
#define GICDistributor_TYPER_ITLinesNumber_Pos 0U                                                    /*!< GICDistributor TYPER: ITLinesNumber Position */
#define GICDistributor_TYPER_ITLinesNumber_Msk (0x1FU /*<< GICDistributor_TYPER_ITLinesNumber_Pos*/) /*!< GICDistributor TYPER: ITLinesNumber Mask */
#define GICDistributor_TYPER_ITLinesNumber(x)  (((uint32_t)(((uint32_t)(x)) /*<< GICDistributor_TYPER_ITLinesNumber_Pos*/)) & GICDistributor_CTLR_ITLinesNumber_Msk)

#define GICDistributor_TYPER_CPUNumber_Pos    5U                                                   /*!< GICDistributor TYPER: CPUNumber Position */
#define GICDistributor_TYPER_CPUNumber_Msk    (0x7U << GICDistributor_TYPER_CPUNumber_Pos)         /*!< GICDistributor TYPER: CPUNumber Mask */
#define GICDistributor_TYPER_CPUNumber(x)     (((uint32_t)(((uint32_t)(x)) << GICDistributor_TYPER_CPUNumber_Pos)) & GICDistributor_TYPER_CPUNumber_Msk)

#define GICDistributor_TYPER_SecurityExtn_Pos 10U                                                  /*!< GICDistributor TYPER: SecurityExtn Position */
#define GICDistributor_TYPER_SecurityExtn_Msk (0x1U << GICDistributor_TYPER_SecurityExtn_Pos)      /*!< GICDistributor TYPER: SecurityExtn Mask */
#define GICDistributor_TYPER_SecurityExtn(x)  (((uint32_t)(((uint32_t)(x)) << GICDistributor_TYPER_SecurityExtn_Pos)) & GICDistributor_TYPER_SecurityExtn_Msk)

#define GICDistributor_TYPER_LSPI_Pos         11U                                                  /*!< GICDistributor TYPER: LSPI Position */
#define GICDistributor_TYPER_LSPI_Msk         (0x1FU << GICDistributor_TYPER_LSPI_Pos)             /*!< GICDistributor TYPER: LSPI Mask */
#define GICDistributor_TYPER_LSPI(x)          (((uint32_t)(((uint32_t)(x)) << GICDistributor_TYPER_LSPI_Pos)) & GICDistributor_TYPER_LSPI_Msk)

/* GICDistributor IIDR Register */
#define GICDistributor_IIDR_Implementer_Pos   0U                                                   /*!< GICDistributor IIDR: Implementer Position */
#define GICDistributor_IIDR_Implementer_Msk   (0xFFFU /*<< GICDistributor_IIDR_Implementer_Pos*/)  /*!< GICDistributor IIDR: Implementer Mask */
#define GICDistributor_IIDR_Implementer(x)    (((uint32_t)(((uint32_t)(x)) /*<< GICDistributor_IIDR_Implementer_Pos*/)) & GICDistributor_IIDR_Implementer_Msk)

#define GICDistributor_IIDR_Revision_Pos      12U                                                  /*!< GICDistributor IIDR: Revision Position */
#define GICDistributor_IIDR_Revision_Msk      (0xFU << GICDistributor_IIDR_Revision_Pos)           /*!< GICDistributor IIDR: Revision Mask */
#define GICDistributor_IIDR_Revision(x)       (((uint32_t)(((uint32_t)(x)) << GICDistributor_IIDR_Revision_Pos)) & GICDistributor_IIDR_Revision_Msk)

#define GICDistributor_IIDR_Variant_Pos       16U                                                  /*!< GICDistributor IIDR: Variant Position */
#define GICDistributor_IIDR_Variant_Msk       (0xFU << GICDistributor_IIDR_Variant_Pos)            /*!< GICDistributor IIDR: Variant Mask */
#define GICDistributor_IIDR_Variant(x)        (((uint32_t)(((uint32_t)(x)) << GICDistributor_IIDR_Variant_Pos)) & GICDistributor_IIDR_Variant_Msk)

#define GICDistributor_IIDR_ProductID_Pos     24U                                                  /*!< GICDistributor IIDR: ProductID Position */
#define GICDistributor_IIDR_ProductID_Msk     (0xFFU << GICDistributor_IIDR_ProductID_Pos)         /*!< GICDistributor IIDR: ProductID Mask */
#define GICDistributor_IIDR_ProductID(x)      (((uint32_t)(((uint32_t)(x)) << GICDistributor_IIDR_ProductID_Pos)) & GICDistributor_IIDR_ProductID_Msk)

/* GICDistributor STATUSR Register */
#define GICDistributor_STATUSR_RRD_Pos        0U                                                   /*!< GICDistributor STATUSR: RRD Position */
#define GICDistributor_STATUSR_RRD_Msk        (0x1U /*<< GICDistributor_STATUSR_RRD_Pos*/)         /*!< GICDistributor STATUSR: RRD Mask */
#define GICDistributor_STATUSR_RRD(x)         (((uint32_t)(((uint32_t)(x)) /*<< GICDistributor_STATUSR_RRD_Pos*/)) & GICDistributor_STATUSR_RRD_Msk)

#define GICDistributor_STATUSR_WRD_Pos        1U                                                   /*!< GICDistributor STATUSR: WRD Position */
#define GICDistributor_STATUSR_WRD_Msk        (0x1U << GICDistributor_STATUSR_WRD_Pos)             /*!< GICDistributor STATUSR: WRD Mask */
#define GICDistributor_STATUSR_WRD(x)         (((uint32_t)(((uint32_t)(x)) << GICDistributor_STATUSR_WRD_Pos)) & GICDistributor_STATUSR_WRD_Msk)

#define GICDistributor_STATUSR_RWOD_Pos       2U                                                   /*!< GICDistributor STATUSR: RWOD Position */
#define GICDistributor_STATUSR_RWOD_Msk       (0x1U << GICDistributor_STATUSR_RWOD_Pos)            /*!< GICDistributor STATUSR: RWOD Mask */
#define GICDistributor_STATUSR_RWOD(x)        (((uint32_t)(((uint32_t)(x)) << GICDistributor_STATUSR_RWOD_Pos)) & GICDistributor_STATUSR_RWOD_Msk)

#define GICDistributor_STATUSR_WROD_Pos       3U                                                   /*!< GICDistributor STATUSR: WROD Position */
#define GICDistributor_STATUSR_WROD_Msk       (0x1U << GICDistributor_STATUSR_WROD_Pos)            /*!< GICDistributor STATUSR: WROD Mask */
#define GICDistributor_STATUSR_WROD(x)        (((uint32_t)(((uint32_t)(x)) << GICDistributor_STATUSR_WROD_Pos)) & GICDistributor_STATUSR_WROD_Msk)

/* GICDistributor SETSPI_NSR Register */
#define GICDistributor_SETSPI_NSR_INTID_Pos   0U                                                   /*!< GICDistributor SETSPI_NSR: INTID Position */
#define GICDistributor_SETSPI_NSR_INTID_Msk   (0x3FFU /*<< GICDistributor_SETSPI_NSR_INTID_Pos*/)  /*!< GICDistributor SETSPI_NSR: INTID Mask */
#define GICDistributor_SETSPI_NSR_INTID(x)    (((uint32_t)(((uint32_t)(x)) /*<< GICDistributor_SETSPI_NSR_INTID_Pos*/)) & GICDistributor_SETSPI_NSR_INTID_Msk)

/* GICDistributor CLRSPI_NSR Register */
#define GICDistributor_CLRSPI_NSR_INTID_Pos   0U                                                   /*!< GICDistributor CLRSPI_NSR: INTID Position */
#define GICDistributor_CLRSPI_NSR_INTID_Msk   (0x3FFU /*<< GICDistributor_CLRSPI_NSR_INTID_Pos*/)  /*!< GICDistributor CLRSPI_NSR: INTID Mask */
#define GICDistributor_CLRSPI_NSR_INTID(x)    (((uint32_t)(((uint32_t)(x)) /*<< GICDistributor_CLRSPI_NSR_INTID_Pos*/)) & GICDistributor_CLRSPI_NSR_INTID_Msk)

/* GICDistributor SETSPI_SR Register */
#define GICDistributor_SETSPI_SR_INTID_Pos    0U                                                  /*!< GICDistributor SETSPI_SR: INTID Position */
#define GICDistributor_SETSPI_SR_INTID_Msk    (0x3FFU /*<< GICDistributor_SETSPI_SR_INTID_Pos*/)  /*!< GICDistributor SETSPI_SR: INTID Mask */
#define GICDistributor_SETSPI_SR_INTID(x)     (((uint32_t)(((uint32_t)(x)) /*<< GICDistributor_SETSPI_SR_INTID_Pos*/)) & GICDistributor_SETSPI_SR_INTID_Msk)

/* GICDistributor CLRSPI_SR Register */
#define GICDistributor_CLRSPI_SR_INTID_Pos    0U                                                  /*!< GICDistributor CLRSPI_SR: INTID Position */
#define GICDistributor_CLRSPI_SR_INTID_Msk    (0x3FFU /*<< GICDistributor_CLRSPI_SR_INTID_Pos*/)  /*!< GICDistributor CLRSPI_SR: INTID Mask */
#define GICDistributor_CLRSPI_SR_INTID(x)     (((uint32_t)(((uint32_t)(x)) /*<< GICDistributor_CLRSPI_SR_INTID_Pos*/)) & GICDistributor_CLRSPI_SR_INTID_Msk)

///* GICDistributor ITARGETSR Register */
//#define GICDistributor_ITARGETSR_CPU0_Pos     0U                                                   /*!< GICDistributor ITARGETSR: CPU0 Position */
//#define GICDistributor_ITARGETSR_CPU0_Msk     (0x1U /*<< GICDistributor_ITARGETSR_CPU0_Pos*/)      /*!< GICDistributor ITARGETSR: CPU0 Mask */
//#define GICDistributor_ITARGETSR_CPU0(x)      (((uint8_t)(((uint8_t)(x)) /*<< GICDistributor_ITARGETSR_CPU0_Pos*/)) & GICDistributor_ITARGETSR_CPU0_Msk)
//
//#define GICDistributor_ITARGETSR_CPU1_Pos     1U                                                   /*!< GICDistributor ITARGETSR: CPU1 Position */
//#define GICDistributor_ITARGETSR_CPU1_Msk     (0x1U << GICDistributor_ITARGETSR_CPU1_Pos)          /*!< GICDistributor ITARGETSR: CPU1 Mask */
//#define GICDistributor_ITARGETSR_CPU1(x)      (((uint8_t)(((uint8_t)(x)) << GICDistributor_ITARGETSR_CPU1_Pos)) & GICDistributor_ITARGETSR_CPU1_Msk)
//
//#define GICDistributor_ITARGETSR_CPU2_Pos     2U                                                   /*!< GICDistributor ITARGETSR: CPU2 Position */
//#define GICDistributor_ITARGETSR_CPU2_Msk     (0x1U << GICDistributor_ITARGETSR_CPU2_Pos)          /*!< GICDistributor ITARGETSR: CPU2 Mask */
//#define GICDistributor_ITARGETSR_CPU2(x)      (((uint8_t)(((uint8_t)(x)) << GICDistributor_ITARGETSR_CPU2_Pos)) & GICDistributor_ITARGETSR_CPU2_Msk)
//
//#define GICDistributor_ITARGETSR_CPU3_Pos     3U                                                   /*!< GICDistributor ITARGETSR: CPU3 Position */
//#define GICDistributor_ITARGETSR_CPU3_Msk     (0x1U << GICDistributor_ITARGETSR_CPU3_Pos)          /*!< GICDistributor ITARGETSR: CPU3 Mask */
//#define GICDistributor_ITARGETSR_CPU3(x)      (((uint8_t)(((uint8_t)(x)) << GICDistributor_ITARGETSR_CPU3_Pos)) & GICDistributor_ITARGETSR_CPU3_Msk)
//
//#define GICDistributor_ITARGETSR_CPU4_Pos     4U                                                   /*!< GICDistributor ITARGETSR: CPU4 Position */
//#define GICDistributor_ITARGETSR_CPU4_Msk     (0x1U << GICDistributor_ITARGETSR_CPU4_Pos)          /*!< GICDistributor ITARGETSR: CPU4 Mask */
//#define GICDistributor_ITARGETSR_CPU4(x)      (((uint8_t)(((uint8_t)(x)) << GICDistributor_ITARGETSR_CPU4_Pos)) & GICDistributor_ITARGETSR_CPU4_Msk)
//
//#define GICDistributor_ITARGETSR_CPU5_Pos     5U                                                   /*!< GICDistributor ITARGETSR: CPU5 Position */
//#define GICDistributor_ITARGETSR_CPU5_Msk     (0x1U << GICDistributor_ITARGETSR_CPU5_Pos)          /*!< GICDistributor ITARGETSR: CPU5 Mask */
//#define GICDistributor_ITARGETSR_CPU5(x)      (((uint8_t)(((uint8_t)(x)) << GICDistributor_ITARGETSR_CPU5_Pos)) & GICDistributor_ITARGETSR_CPU5_Msk)
//
//#define GICDistributor_ITARGETSR_CPU6_Pos     6U                                                   /*!< GICDistributor ITARGETSR: CPU6 Position */
//#define GICDistributor_ITARGETSR_CPU6_Msk     (0x1U << GICDistributor_ITARGETSR_CPU6_Pos)          /*!< GICDistributor ITARGETSR: CPU6 Mask */
//#define GICDistributor_ITARGETSR_CPU6(x)      (((uint8_t)(((uint8_t)(x)) << GICDistributor_ITARGETSR_CPU6_Pos)) & GICDistributor_ITARGETSR_CPU6_Msk)
//
//#define GICDistributor_ITARGETSR_CPU7_Pos     7U                                                   /*!< GICDistributor ITARGETSR: CPU7 Position */
//#define GICDistributor_ITARGETSR_CPU7_Msk     (0x1U << GICDistributor_ITARGETSR_CPU7_Pos)          /*!< GICDistributor ITARGETSR: CPU7 Mask */
//#define GICDistributor_ITARGETSR_CPU7(x)      (((uint8_t)(((uint8_t)(x)) << GICDistributor_ITARGETSR_CPU7_Pos)) & GICDistributor_ITARGETSR_CPU7_Msk)

/* GICDistributor SGIR Register */
#define GICDistributor_SGIR_INTID_Pos         0U                                                   /*!< GICDistributor SGIR: INTID Position */
#define GICDistributor_SGIR_INTID_Msk         (0xFU /*<< GICDistributor_SGIR_INTID_Pos*/)          /*!< GICDistributor SGIR: INTID Mask */
#define GICDistributor_SGIR_INTID(x)          (((uint32_t)(((uint32_t)(x)) /*<< GICDistributor_SGIR_INTID_Pos*/)) & GICDistributor_SGIR_INTID_Msk)

#define GICDistributor_SGIR_NSATT_Pos         15U                                                  /*!< GICDistributor SGIR: NSATT Position */
#define GICDistributor_SGIR_NSATT_Msk         (0x1U << GICDistributor_SGIR_NSATT_Pos)              /*!< GICDistributor SGIR: NSATT Mask */
#define GICDistributor_SGIR_NSATT(x)          (((uint32_t)(((uint32_t)(x)) << GICDistributor_SGIR_NSATT_Pos)) & GICDistributor_SGIR_NSATT_Msk)

#define GICDistributor_SGIR_CPUTargetList_Pos 16U                                                  /*!< GICDistributor SGIR: CPUTargetList  Position */
#define GICDistributor_SGIR_CPUTargetList_Msk (0xFFU << GICDistributor_SGIR_CPUTargetList_Pos)     /*!< GICDistributor SGIR: CPUTargetList  Mask */
#define GICDistributor_SGIR_CPUTargetList(x)  (((uint32_t)(((uint32_t)(x)) << GICDistributor_SGIR_CPUTargetList_Pos)) & GICDistributor_SGIR_CPUTargetList_Msk)

#define GICDistributor_SGIR_TargetFilterList_Pos 24U                                                /*!< GICDistributor SGIR: TargetFilterList Position */
#define GICDistributor_SGIR_TargetFilterList_Msk (0x3U << GICDistributor_SGIR_TargetFilterList_Pos) /*!< GICDistributor SGIR: TargetFilterList Mask */
#define GICDistributor_SGIR_TargetFilterList(x)  (((uint32_t)(((uint32_t)(x)) << GICDistributor_SGIR_TargetFilterList_Pos)) & GICDistributor_SGIR_TargetFilterList_Msk)

/* GICDistributor IROUTER Register */
#define GICDistributor_IROUTER_Aff0_Pos       0UL                                                  /*!< GICDistributor IROUTER: Aff0 Position */
#define GICDistributor_IROUTER_Aff0_Msk       (0xFFUL /*<< GICDistributor_IROUTER_Aff0_Pos*/)      /*!< GICDistributor IROUTER: Aff0 Mask */
#define GICDistributor_IROUTER_Aff0(x)        (((uint64_t)(((uint64_t)(x)) /*<< GICDistributor_IROUTER_Aff0_Pos*/)) & GICDistributor_IROUTER_Aff0_Msk)

#define GICDistributor_IROUTER_Aff1_Pos       8UL                                                  /*!< GICDistributor IROUTER: Aff1 Position */
#define GICDistributor_IROUTER_Aff1_Msk       (0xFFUL << GICDistributor_IROUTER_Aff1_Pos)          /*!< GICDistributor IROUTER: Aff1 Mask */
#define GICDistributor_IROUTER_Aff1(x)        (((uint64_t)(((uint64_t)(x)) << GICDistributor_IROUTER_Aff1_Pos)) & GICDistributor_IROUTER_Aff1_Msk)

#define GICDistributor_IROUTER_Aff2_Pos       16UL                                                 /*!< GICDistributor IROUTER: Aff2 Position */
#define GICDistributor_IROUTER_Aff2_Msk       (0xFFUL << GICDistributor_IROUTER_Aff2_Pos)          /*!< GICDistributor IROUTER: Aff2 Mask */
#define GICDistributor_IROUTER_Aff2(x)        (((uint64_t)(((uint64_t)(x)) << GICDistributor_IROUTER_Aff2_Pos)) & GICDistributor_IROUTER_Aff2_Msk)

#define GICDistributor_IROUTER_IRM_Pos        31UL                                                 /*!< GICDistributor IROUTER: IRM Position */
#define GICDistributor_IROUTER_IRM_Msk        (0xFFUL << GICDistributor_IROUTER_IRM_Pos)           /*!< GICDistributor IROUTER: IRM Mask */
#define GICDistributor_IROUTER_IRM(x)         (((uint64_t)(((uint64_t)(x)) << GICDistributor_IROUTER_IRM_Pos)) & GICDistributor_IROUTER_IRM_Msk)

#define GICDistributor_IROUTER_Aff3_Pos       32UL                                                 /*!< GICDistributor IROUTER: Aff3 Position */
#define GICDistributor_IROUTER_Aff3_Msk       (0xFFUL << GICDistributor_IROUTER_Aff3_Pos)          /*!< GICDistributor IROUTER: Aff3 Mask */
#define GICDistributor_IROUTER_Aff3(x)        (((uint64_t)(((uint64_t)(x)) << GICDistributor_IROUTER_Aff3_Pos)) & GICDistributor_IROUTER_Aff3_Msk)

#endif /* GIC_DISTRIBUTOR_BASE */

#if defined (GIC_DISTRIBUTOR_BASE)

__STATIC_FORCEINLINE void GIC_DistributorWait(void)
{
	while ((GICDistributor->CTLR & (UINT32_C(1) << 31)) != 0)
		;
}

/** \brief  Enable the interrupt distributor using the GIC's CTLR register.
*/
__STATIC_INLINE void GIC_EnableDistributor(void)
{
  GICDistributor->CTLR |= 1U;
  GIC_DistributorWait();
}

/** \brief Disable the interrupt distributor using the GIC's CTLR register.
*/
__STATIC_INLINE void GIC_DisableDistributor(void)
{
  GICDistributor->CTLR &=~1U;
  GIC_DistributorWait();
}

/** \brief Read the GIC's TYPER register.
* \return GICDistributor_Type::TYPER
*/
__STATIC_INLINE uint32_t GIC_DistributorInfo(void)
{
  return (GICDistributor->TYPER);
}

/** \brief Reads the GIC's IIDR register.
* \return GICDistributor_Type::IIDR
*/
__STATIC_INLINE uint32_t GIC_DistributorImplementer(void)
{
  return (GICDistributor->IIDR);
}

/** \brief Get the Affinity Routing status.
*/
__STATIC_INLINE int GIC_GetARE(void)
{
    return !! (GICDistributor->CTLR & 0x30);
}

/** \brief Sets the GIC's ITARGETSR register for the given interrupt.
* \param [in] IRQn Interrupt to be configured.
* \param [in] cpu_target CPU interfaces to assign this interrupt to.
*/
__STATIC_INLINE void GIC_SetTarget(IRQn_Type IRQn, uint32_t cpu_target)
{
	if (IRQn >= 32)
	{
		if (GIC_GetARE())
		{
			/* affinity routing */
			GICDistributor->IROUTER [IRQn] =
					GICDistributor_IROUTER_IRM(1) |	/* Interrupts routed to any PE defined as a participating node. */
					//GICDistributor_IROUTER_Aff0(cpu_target) |
					GICDistributor_IROUTER_Aff1(cpu_target) |
					//GICDistributor_IROUTER_Aff2(cpu_target) |
					//GICDistributor_IROUTER_Aff3(cpu_target) |
					0;
		}
		else
		{
			/* legacy */
//			uint32_t mask = GICDistributor->ITARGETSR[IRQn / 4U] & ~(0xFFUL << ((IRQn % 4U) * 8U));
//			GICDistributor->ITARGETSR[IRQn / 4U] = mask | ((cpu_target & 0xFFUL) << ((IRQn % 4U) * 8U));
		}
	}
}

/** \brief Read the GIC's ITARGETSR register.
* \param [in] IRQn Interrupt to acquire the configuration for.
* \return GICDistributor_Type::ITARGETSR
*/
__STATIC_INLINE uint32_t GIC_GetTarget(IRQn_Type IRQn)
{
    uint64_t cpu_target = 0;

    if (IRQn >= 32)
    {
        if (GIC_GetARE())
        {
        	/* affinity routing */
            cpu_target = (GICDistributor->IROUTER[IRQn] & GICDistributor_IROUTER_Aff1_Msk) >> GICDistributor_IROUTER_Aff1_Pos;
        }
        else
        {
        	/* legacy */
            cpu_target = 0;//(GICDistributor->ITARGETSR[IRQn / 4U] >> ((IRQn % 4U) * 8U)) & 0xFFUL;
        }
    }
    else
    {
      /* local */
        cpu_target = __get_MPIDR_EL1() & MPIDR_AFFINITY_MASK;
    }

    return cpu_target;
}

#endif /* GIC_DISTRIBUTOR_BASE */

#if defined (GIC_DISTRIBUTOR_BASE)

/** \brief Enables the given interrupt using GIC's ISENABLER register.
* \param [in] IRQn The interrupt to be enabled.
*/
__STATIC_INLINE void GIC_EnableIRQ(IRQn_Type IRQn)
{
  GICDistributor->ISENABLER[IRQn / 32U] = UINT32_C(1) << (IRQn % 32U);
}

/** \brief Get interrupt enable status using GIC's ISENABLER register.
* \param [in] IRQn The interrupt to be queried.
* \return 0 - interrupt is not enabled, 1 - interrupt is enabled.
*/
__STATIC_INLINE uint32_t GIC_GetEnableIRQ(IRQn_Type IRQn)
{
  return (GICDistributor->ISENABLER[IRQn / 32U] >> (IRQn % 32U)) & UINT32_C(1);
}

/** \brief Disables the given interrupt using GIC's ICENABLER register.
* \param [in] IRQn The interrupt to be disabled.
*/
__STATIC_INLINE void GIC_DisableIRQ(IRQn_Type IRQn)
{
  GICDistributor->ICENABLER[IRQn / 32U] = UINT32_C(1) << (IRQn % 32U);
}

/** \brief Get interrupt pending status from GIC's ISPENDR register.
* \param [in] IRQn The interrupt to be queried.
* \return 0 - interrupt is not pending, 1 - interrupt is pendig.
*/
__STATIC_INLINE uint32_t GIC_GetPendingIRQ(IRQn_Type IRQn)
{
	uint32_t pend;

	if (IRQn >= 16) {
		pend = (GICDistributor->ISPENDR[IRQn / 32U] >> (IRQn % 32U)) & UINT32_C(1);
	} else {
	// INTID 0-15 Software Generated Interrupt
		pend = 0;//(GICDistributor->SPENDSGIR[IRQn / 4U] >> ((IRQn % 4U) * 8U)) & 0xFFUL;
		// No CPU identification offered
		if (pend != 0U) {
			pend = 1U;
		} else {
			pend = 0U;
		}
	}

	return (pend);
}

/** \brief Sets the given interrupt as pending using GIC's ISPENDR register.
* \param [in] IRQn The interrupt to be enabled.
*/
__STATIC_INLINE void GIC_SetPendingIRQ(IRQn_Type IRQn)
{
	if (IRQn >= 16) {
		GICDistributor->ISPENDR[IRQn / 32U] = UINT32_C(1) << (IRQn % 32U);
	} else {
		// INTID 0-15 Software Generated Interrupt
//		GICDistributor->SPENDSGIR[IRQn / 4U] = 1U << ((IRQn % 4U) * 8U);
//		// Forward the interrupt to the CPU interface that requested it
//		GICDistributor->SGIR = (IRQn | 0x02000000U);
	}
}

/** \brief Clears the given interrupt from being pending using GIC's ICPENDR register.
* \param [in] IRQn The interrupt to be enabled.
*/
__STATIC_INLINE void GIC_ClearPendingIRQ(IRQn_Type IRQn)
{
	if (IRQn >= 16U) {
		GICDistributor->ICPENDR[IRQn / 32U] = UINT32_C(1) << (IRQn % 32U);
	} else {
		// INTID 0-15 Software Generated Interrupt
		//GICDistributor->CPENDSGIR[IRQn / 4U] = UINT32_C(1) << ((IRQn % 4U) * 8U);
	}
}

/** \brief Sets the interrupt configuration using GIC's ICFGR register.
* \param [in] IRQn The interrupt to be configured.
* \param [in] int_config Int_config field value. Bit 0: Reserved (0 - N-N model, 1 - 1-N model for some GIC before v1)
*                                           Bit 1: 0 - level sensitive, 1 - edge triggered
*/
__STATIC_INLINE void GIC_SetConfiguration(IRQn_Type IRQn, uint32_t int_config)
{
  uint32_t icfgr = GICDistributor->ICFGR[IRQn / 16U];  /* read current register content */
  const uint32_t shift = (IRQn % 16U) << 1U;                 /* calculate shift value */

  int_config &= 3U;                                    /* only 2 bits are valid */
  icfgr &= (~ (UINT32_C(0x03) << shift));                   /* clear bits to change */
  icfgr |= (  int_config 	  << shift);                    /* set new configuration */

  GICDistributor->ICFGR[IRQn / 16U] = icfgr;           /* write new register content */
}

/** \brief Get the interrupt configuration from the GIC's ICFGR register.
* \param [in] IRQn Interrupt to acquire the configuration for.
* \return Int_config field value. Bit 0: Reserved (0 - N-N model, 1 - 1-N model for some GIC before v1)
*                                 Bit 1: 0 - level sensitive, 1 - edge triggered
*/
__STATIC_INLINE uint32_t GIC_GetConfiguration(IRQn_Type IRQn)
{
  return (GICDistributor->ICFGR[IRQn / 16U] >> ((IRQn % 16U) << 1U)) & UINT32_C(0x03);
}

/** \brief Set the priority for the given interrupt in the GIC's IPRIORITYR register.
* \param [in] IRQn The interrupt to be configured.
* \param [in] priority The priority for the interrupt, lower values denote higher priorities.
*/
__STATIC_INLINE void GIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  uint32_t mask = GICDistributor->IPRIORITYR[IRQn / 4U] & ~ (UINT32_C(0xFF) << ((IRQn % 4U) * 8U));
  GICDistributor->IPRIORITYR[IRQn / 4U] = mask | ((priority & UINT32_C(0xFF)) << ((IRQn % 4U) * 8U));
}

/** \brief Read the current interrupt priority from GIC's IPRIORITYR register.
* \param [in] IRQn The interrupt to be queried.
*/
__STATIC_INLINE uint32_t GIC_GetPriority(IRQn_Type IRQn)
{
  return (GICDistributor->IPRIORITYR[IRQn / 4U] >> ((IRQn % 4U) * 8U)) & UINT32_C(0xFF);
}
#endif /* GIC_DISTRIBUTOR_BASE */

#if defined (GIC_INTERFACE_BASE)

/** \brief  Structure type to access the Generic Interrupt Controller Interface (GICC)
*/
typedef struct
{
  __IOM uint32_t CTLR;                 /*!< \brief  Offset: 0x000 (R/W) CPU Interface Control Register */
  __IOM uint32_t PMR;                  /*!< \brief  Offset: 0x004 (R/W) Interrupt Priority Mask Register */
  __IOM uint32_t BPR;                  /*!< \brief  Offset: 0x008 (R/W) Binary Point Register */
  __IM  uint32_t IAR;                  /*!< \brief  Offset: 0x00C (R/ ) Interrupt Acknowledge Register */
  __OM  uint32_t EOIR;                 /*!< \brief  Offset: 0x010 ( /W) End Of Interrupt Register */
  __IM  uint32_t RPR;                  /*!< \brief  Offset: 0x014 (R/ ) Running Priority Register */
  __IM  uint32_t HPPIR;                /*!< \brief  Offset: 0x018 (R/ ) Highest Priority Pending Interrupt Register */
  __IOM uint32_t ABPR;                 /*!< \brief  Offset: 0x01C (R/W) Aliased Binary Point Register */
  __IM  uint32_t AIAR;                 /*!< \brief  Offset: 0x020 (R/ ) Aliased Interrupt Acknowledge Register */
  __OM  uint32_t AEOIR;                /*!< \brief  Offset: 0x024 ( /W) Aliased End Of Interrupt Register */
  __IM  uint32_t AHPPIR;               /*!< \brief  Offset: 0x028 (R/ ) Aliased Highest Priority Pending Interrupt Register */
  __IOM uint32_t STATUSR;              /*!< \brief  Offset: 0x02C (R/W) Error Reporting Status Register, optional */
        RESERVED(1[40], uint32_t)
  __IOM uint32_t APR[4];               /*!< \brief  Offset: 0x0D0 (R/W) Active Priority Register */
  __IOM uint32_t NSAPR[4];             /*!< \brief  Offset: 0x0E0 (R/W) Non-secure Active Priority Register */
        RESERVED(2[3], uint32_t)
  __IM  uint32_t IIDR;                 /*!< \brief  Offset: 0x0FC (R/ ) CPU Interface Identification Register */
        RESERVED(3[960], uint32_t)
  __OM  uint32_t DIR;                  /*!< \brief  Offset: 0x1000( /W) Deactivate Interrupt Register */
}  GICInterface_Type;

#define GICInterface        ((GICInterface_Type        *)     GIC_INTERFACE_BASE )   /*!< \brief GIC Interface register set access pointer */

/* GICInterface CTLR Register */
#define GICInterface_CTLR_Enable_Pos        0U                                              /*!< PTIM CTLR: Enable Position */
#define GICInterface_CTLR_Enable_Msk        (0x1U /*<< GICInterface_CTLR_Enable_Pos*/)      /*!< PTIM CTLR: Enable Mask */
#define GICInterface_CTLR_Enable(x)         (((uint32_t)(((uint32_t)(x)) /*<< GICInterface_CTLR_Enable_Pos*/)) & GICInterface_CTLR_Enable_Msk)

/* GICInterface PMR Register */
#define GICInterface_PMR_Priority_Pos       0U                                              /*!< PTIM PMR: Priority Position */
#define GICInterface_PMR_Priority_Msk       (0xFFU /*<< GICInterface_PMR_Priority_Pos*/)    /*!< PTIM PMR: Priority Mask */
#define GICInterface_PMR_Priority(x)        (((uint32_t)(((uint32_t)(x)) /*<< GICInterface_PMR_Priority_Pos*/)) & GICInterface_PMR_Priority_Msk)

/* GICInterface BPR Register */
#define GICInterface_BPR_Binary_Point_Pos   0U                                              /*!< PTIM BPR: Binary_Point Position */
#define GICInterface_BPR_Binary_Point_Msk   (0x7U /*<< GICInterface_BPR_Binary_Point_Pos*/) /*!< PTIM BPR: Binary_Point Mask */
#define GICInterface_BPR_Binary_Point(x)    (((uint32_t)(((uint32_t)(x)) /*<< GICInterface_BPR_Binary_Point_Pos*/)) & GICInterface_BPR_Binary_Point_Msk)

/* GICInterface IAR Register */
#define GICInterface_IAR_INTID_Pos          0U                                              /*!< PTIM IAR: INTID Position */
#define GICInterface_IAR_INTID_Msk          (0xFFFFFFU /*<< GICInterface_IAR_INTID_Pos*/)   /*!< PTIM IAR: INTID Mask */
#define GICInterface_IAR_INTID(x)           (((uint32_t)(((uint32_t)(x)) /*<< GICInterface_IAR_INTID_Pos*/)) & GICInterface_IAR_INTID_Msk)

/* GICInterface EOIR Register */
#define GICInterface_EOIR_INTID_Pos         0U                                              /*!< PTIM EOIR: INTID Position */
#define GICInterface_EOIR_INTID_Msk         (0xFFFFFFU /*<< GICInterface_EOIR_INTID_Pos*/)  /*!< PTIM EOIR: INTID Mask */
#define GICInterface_EOIR_INTID(x)          (((uint32_t)(((uint32_t)(x)) /*<< GICInterface_EOIR_INTID_Pos*/)) & GICInterface_EOIR_INTID_Msk)

/* GICInterface RPR Register */
#define GICInterface_RPR_INTID_Pos          0U                                              /*!< PTIM RPR: INTID Position */
#define GICInterface_RPR_INTID_Msk          (0xFFU /*<< GICInterface_RPR_INTID_Pos*/)       /*!< PTIM RPR: INTID Mask */
#define GICInterface_RPR_INTID(x)           (((uint32_t)(((uint32_t)(x)) /*<< GICInterface_RPR_INTID_Pos*/)) & GICInterface_RPR_INTID_Msk)

/* GICInterface HPPIR Register */
#define GICInterface_HPPIR_INTID_Pos        0U                                               /*!< PTIM HPPIR: INTID Position */
#define GICInterface_HPPIR_INTID_Msk        (0x3FFU /*<< GICInterface_HPPIR_INTID_Pos*/)  /*!< PTIM HPPIR: INTID Mask */
#define GICInterface_HPPIR_INTID(x)         (((uint32_t)(((uint32_t)(x)) /*<< GICInterface_HPPIR_INTID_Pos*/)) & GICInterface_HPPIR_INTID_Msk)

/* GICInterface ABPR Register */
#define GICInterface_ABPR_Binary_Point_Pos  0U                                               /*!< PTIM ABPR: Binary_Point Position */
#define GICInterface_ABPR_Binary_Point_Msk  (0x7U /*<< GICInterface_ABPR_Binary_Point_Pos*/) /*!< PTIM ABPR: Binary_Point Mask */
#define GICInterface_ABPR_Binary_Point(x)   (((uint32_t)(((uint32_t)(x)) /*<< GICInterface_ABPR_Binary_Point_Pos*/)) & GICInterface_ABPR_Binary_Point_Msk)

/* GICInterface AIAR Register */
#define GICInterface_AIAR_INTID_Pos         0U                                              /*!< PTIM AIAR: INTID Position */
#define GICInterface_AIAR_INTID_Msk         (0xFFFFFFU /*<< GICInterface_AIAR_INTID_Pos*/)  /*!< PTIM AIAR: INTID Mask */
#define GICInterface_AIAR_INTID(x)          (((uint32_t)(((uint32_t)(x)) /*<< GICInterface_AIAR_INTID_Pos*/)) & GICInterface_AIAR_INTID_Msk)

/* GICInterface AEOIR Register */
#define GICInterface_AEOIR_INTID_Pos        0U                                              /*!< PTIM AEOIR: INTID Position */
#define GICInterface_AEOIR_INTID_Msk        (0xFFFFFFU /*<< GICInterface_AEOIR_INTID_Pos*/) /*!< PTIM AEOIR: INTID Mask */
#define GICInterface_AEOIR_INTID(x)         (((uint32_t)(((uint32_t)(x)) /*<< GICInterface_AEOIR_INTID_Pos*/)) & GICInterface_AEOIR_INTID_Msk)

/* GICInterface AHPPIR Register */
#define GICInterface_AHPPIR_INTID_Pos       0U                                               /*!< PTIM AHPPIR: INTID Position */
#define GICInterface_AHPPIR_INTID_Msk       (0xFFFFFFU /*<< GICInterface_AHPPIR_INTID_Pos*/) /*!< PTIM AHPPIR: INTID Mask */
#define GICInterface_AHPPIR_INTID(x)        (((uint32_t)(((uint32_t)(x)) /*<< GICInterface_AHPPIR_INTID_Pos*/)) & GICInterface_AHPPIR_INTID_Msk)

/* GICInterface STATUSR Register */
#define GICInterface_STATUSR_RRD_Pos        0U                                              /*!< GICInterface STATUSR: RRD Position */
#define GICInterface_STATUSR_RRD_Msk        (0x1U /*<< GICInterface_STATUSR_RRD_Pos*/)      /*!< GICInterface STATUSR: RRD Mask */
#define GICInterface_STATUSR_RRD(x)         (((uint32_t)(((uint32_t)(x)) /*<< GICInterface_STATUSR_RRD_Pos*/)) & GICInterface_STATUSR_RRD_Msk)

#define GICInterface_STATUSR_WRD_Pos        1U                                              /*!< GICInterface STATUSR: WRD Position */
#define GICInterface_STATUSR_WRD_Msk        (0x1U << GICInterface_STATUSR_WRD_Pos)          /*!< GICInterface STATUSR: WRD Mask */
#define GICInterface_STATUSR_WRD(x)         (((uint32_t)(((uint32_t)(x)) << GICInterface_STATUSR_WRD_Pos)) & GICInterface_STATUSR_WRD_Msk)

#define GICInterface_STATUSR_RWOD_Pos       2U                                              /*!< GICInterface STATUSR: RWOD Position */
#define GICInterface_STATUSR_RWOD_Msk       (0x1U << GICInterface_STATUSR_RWOD_Pos)         /*!< GICInterface STATUSR: RWOD Mask */
#define GICInterface_STATUSR_RWOD(x)        (((uint32_t)(((uint32_t)(x)) << GICInterface_STATUSR_RWOD_Pos)) & GICInterface_STATUSR_RWOD_Msk)

#define GICInterface_STATUSR_WROD_Pos       3U                                              /*!< GICInterface STATUSR: WROD Position */
#define GICInterface_STATUSR_WROD_Msk       (0x1U << GICInterface_STATUSR_WROD_Pos)         /*!< GICInterface STATUSR: WROD Mask */
#define GICInterface_STATUSR_WROD(x)        (((uint32_t)(((uint32_t)(x)) << GICInterface_STATUSR_WROD_Pos)) & GICInterface_STATUSR_WROD_Msk)

#define GICInterface_STATUSR_ASV_Pos        4U                                              /*!< GICInterface STATUSR: ASV Position */
#define GICInterface_STATUSR_ASV_Msk        (0x1U << GICInterface_STATUSR_ASV_Pos)          /*!< GICInterface STATUSR: ASV Mask */
#define GICInterface_STATUSR_ASV(x)         (((uint32_t)(((uint32_t)(x)) << GICInterface_STATUSR_ASV_Pos)) & GICInterface_STATUSR_ASV_Msk)

/* GICInterface IIDR Register */
#define GICInterface_IIDR_Implementer_Pos   0U                                                 /*!< GICInterface IIDR: Implementer Position */
#define GICInterface_IIDR_Implementer_Msk   (0xFFFU /*<< GICInterface_IIDR_Implementer_Pos*/)  /*!< GICInterface IIDR: Implementer Mask */
#define GICInterface_IIDR_Implementer(x)    (((uint32_t)(((uint32_t)(x)) /*<< GICInterface_IIDR_Implementer_Pos*/)) & GICInterface_IIDR_Implementer_Msk)

#define GICInterface_IIDR_Revision_Pos      12U                                             /*!< GICInterface IIDR: Revision Position */
#define GICInterface_IIDR_Revision_Msk      (0xFU << GICInterface_IIDR_Revision_Pos)        /*!< GICInterface IIDR: Revision Mask */
#define GICInterface_IIDR_Revision(x)       (((uint32_t)(((uint32_t)(x)) << GICInterface_IIDR_Revision_Pos)) & GICInterface_IIDR_Revision_Msk)

#define GICInterface_IIDR_Arch_version_Pos  16U                                             /*!< GICInterface IIDR: Arch_version Position */
#define GICInterface_IIDR_Arch_version_Msk  (0xFU << GICInterface_IIDR_Arch_version_Pos)    /*!< GICInterface IIDR: Arch_version Mask */
#define GICInterface_IIDR_Arch_version(x)   (((uint32_t)(((uint32_t)(x)) << GICInterface_IIDR_Arch_version_Pos)) & GICInterface_IIDR_Arch_version_Msk)

#define GICInterface_IIDR_ProductID_Pos     20U                                             /*!< GICInterface IIDR: ProductID Position */
#define GICInterface_IIDR_ProductID_Msk     (0xFFFU << GICInterface_IIDR_ProductID_Pos)     /*!< GICInterface IIDR: ProductID Mask */
#define GICInterface_IIDR_ProductID(x)      (((uint32_t)(((uint32_t)(x)) << GICInterface_IIDR_ProductID_Pos)) & GICInterface_IIDR_ProductID_Msk)

/* GICInterface DIR Register */
#define GICInterface_DIR_INTID_Pos          0U                                              /*!< GICInterface DIR: INTID Position */
#define GICInterface_DIR_INTID_Msk          (0xFFFFFFU /*<< GICInterface_DIR_INTID_Pos*/)   /*!< GICInterface DIR: INTID Mask */
#define GICInterface_DIR_INTID(x)           (((uint32_t)(((uint32_t)(x)) /*<< GICInterface_DIR_INTID_Pos*/)) & GICInterface_DIR_INTID_Msk)

#endif /* GIC_INTERFACE_BASE */

#if defined (GIC_INTERFACE_BASE)

/** \brief Enable the CPU's interrupt interface.
*/
__STATIC_INLINE void GIC_EnableInterface(void)
{
  GICInterface->CTLR |= 1U; //enable interface
}

/** \brief Disable the CPU's interrupt interface.
*/
__STATIC_INLINE void GIC_DisableInterface(void)
{
  GICInterface->CTLR &=~1U; //disable distributor
}

/** \brief Read the CPU's IAR register.
* \return GICInterface_Type::IAR
*/
__STATIC_INLINE IRQn_Type GIC_AcknowledgePending(void)
{
  return (IRQn_Type)(GICInterface->IAR);
}

/** \brief Writes the given interrupt number to the CPU's EOIR register.
* \param [in] IRQn The interrupt to be signaled as finished.
*/
__STATIC_INLINE void GIC_EndInterrupt(IRQn_Type IRQn)
{
  GICInterface->EOIR = IRQn;
}

#endif /* GIC_INTERFACE_BASE */

#if defined (GIC_INTERFACE_BASE)
/** \brief Set the interrupt priority mask using CPU's PMR register.
* \param [in] priority Priority mask to be set.
*/
__STATIC_INLINE void GIC_SetInterfacePriorityMask(uint32_t priority)
{
  GICInterface->PMR = priority & 0xFFUL; //set priority mask
}

/** \brief Read the current interrupt priority mask from CPU's PMR register.
* \result GICInterface_Type::PMR
*/
__STATIC_INLINE uint32_t GIC_GetInterfacePriorityMask(void)
{
  return GICInterface->PMR;
}

/** \brief Configures the group priority and subpriority split point using CPU's BPR register.
* \param [in] binary_point Amount of bits used as subpriority.
*/
__STATIC_INLINE void GIC_SetBinaryPoint(uint32_t binary_point)
{
  GICInterface->BPR = binary_point & 7U; //set binary point
}

/** \brief Read the current group priority and subpriority split point from CPU's BPR register.
* \return GICInterface_Type::BPR
*/
__STATIC_INLINE uint32_t GIC_GetBinaryPoint(void)
{
  return GICInterface->BPR;
}

#else  /* GIC_INTERFACE_BASE */

#if defined (__aarch64__)

/** \brief  AArch64 System registers to access the Generic Interrupt Controller CPU interface
*/
#if defined(__GNUC__)
  #define ssICC_BPR0_EL1           S3_0_C12_C8_3
  #define ssICC_BPR1_EL1           S3_0_C12_C12_3
  #define ssICC_CTLR_EL1           S3_0_C12_C12_4
  #define ssICC_CTLR_EL3           S3_6_C12_C12_4
  #define ssICC_EOIR0_EL1          S3_0_C12_C8_1
  #define ssICC_EOIR1_EL1          S3_0_C12_C12_1
  #define ssICC_HPPIR0_EL1         S3_0_C12_C8_2
  #define ssICC_HPPIR1_EL1         S3_0_C12_C12_2
  #define ssICC_IAR0_EL1           S3_0_C12_C8_0
  #define ssICC_IAR1_EL1           S3_0_C12_C12_0
  #define ssICC_IGRPEN0_EL1        S3_0_C12_C12_6
  #define ssICC_IGRPEN1_EL1        S3_0_C12_C12_7
  #define ssICC_IGRPEN1_EL3        S3_6_C12_C12_7
  #define ssICC_PMR_EL1            S3_0_C4_C6_0
  #define ssICC_RPR_EL1            S3_0_C12_C11_3
  #define ssICC_SGI0R_EL1          S3_0_C12_C11_7
  #define ssICC_SGI1R_EL1          S3_0_C12_C11_5
  #define ssICC_SRE_EL1            S3_0_C12_C12_5
  #define ssICC_SRE_EL2            S3_4_C12_C9_5
  #define ssICC_SRE_EL3            S3_6_C12_C12_5
#endif /* __GNUC__ */

#ifndef __STRINGIFY
  #define __STRINGIFY(x)                         #x
#endif

#ifndef __MSR
  #define __MSR(sysreg, val) \
    __asm volatile ("msr " __STRINGIFY(sysreg) ", %0\n" : : "r"((uint64_t)(val)))
#endif

#ifndef __MRS
#define __MRS(sysreg, pVal) \
  __asm volatile ("mrs  %0, " __STRINGIFY(sysreg) "\n" : "=r"((*pVal)))
#endif

__STATIC_FORCEINLINE uint32_t __get_ICC_SRE_EL1(void)
{
	uint64_t result;
	__MRS(ssICC_SRE_EL1, & result);
	return result;
}

__STATIC_FORCEINLINE void __set_ICC_SRE_EL1(uint32_t value)
{
	__MSR(ssICC_SRE_EL1, value);
}

__STATIC_FORCEINLINE uint32_t __get_ICC_SRE_EL2(void)
{
	uint64_t result;
	__MRS(ssICC_SRE_EL2, & result);
	return result;
}

__STATIC_FORCEINLINE void __set_ICC_SRE_EL2(uint32_t value)
{
	__MSR(ssICC_SRE_EL2, value);
}

__STATIC_FORCEINLINE uint32_t __get_ICC_SRE_EL3(void)
{
	uint64_t result;
	__MRS(ssICC_SRE_EL3, & result);
	return result;
}

__STATIC_FORCEINLINE uint32_t __get_ICC_BPR1_EL1(void)
{
	uint64_t result;
	__MRS(ssICC_BPR1_EL1, & result);
	return result;
}

__STATIC_FORCEINLINE uint32_t __get_ICC_BPR0_EL1(void)
{
	uint64_t result;
	__MRS(ssICC_BPR0_EL1, & result);
	return result;
}

__STATIC_FORCEINLINE uint32_t __get_ICC_PMR_EL1(void)
{
	uint64_t result;
	__MRS(ssICC_PMR_EL1, & result);
	return result;
}

__STATIC_FORCEINLINE uint32_t __get_ICC_HPPIR0_EL1(void)
{
	uint64_t result;
	__MRS(ssICC_HPPIR0_EL1, & result);
	return result;
}

__STATIC_FORCEINLINE uint32_t __get_ICC_HPPIR1_EL1(void)
{
	uint64_t result;
	__MRS(ssICC_HPPIR1_EL1, & result);
	return result;
}

__STATIC_FORCEINLINE void __set_ICC_SRE_EL3(uint64_t value)
{
	__MSR(ssICC_SRE_EL3, value);
}

__STATIC_FORCEINLINE void __set_ICC_PMR_EL1(uint64_t value)
{
	__MSR(ssICC_PMR_EL1, value);
}


__STATIC_FORCEINLINE void __set_ICC_EOIR0_EL1(uint64_t value)
{
	__MSR(ssICC_EOIR0_EL1, value);
}

__STATIC_FORCEINLINE void __set_ICC_EOIR1_EL1(uint64_t value)
{
	__MSR(ssICC_EOIR1_EL1, value);
}

__STATIC_FORCEINLINE void __set_ICC_BPR0_EL1(uint64_t value)
{
	__MSR(ssICC_BPR0_EL1, value);
}

__STATIC_FORCEINLINE void __set_ICC_BPR1_EL1(uint64_t value)
{
	__MSR(ssICC_BPR1_EL1, value);
}

// ICC_CTLR_EL1, Interrupt Controller Control Register (EL1)
__STATIC_FORCEINLINE void __set_ICC_CTLR_EL1(uint64_t value)
{
	__MSR(ssICC_CTLR_EL1, value);
}

// ICC_CTLR_EL1, Interrupt Controller Control Register (EL1)
__STATIC_FORCEINLINE uint64_t __get_ICC_CTLR_EL1(void)
{
	uint64_t result;
    __MRS(ssICC_CTLR_EL1, &result);
    return result;
}

__STATIC_FORCEINLINE void __set_ICC_CTLR_EL3(uint64_t value)
{
	__MSR(ssICC_CTLR_EL3, value);
}

__STATIC_FORCEINLINE uint64_t __get_ICC_CTLR_EL3(void)
{
	uint64_t result;
    __MRS(ssICC_CTLR_EL3, &result);
    return result;
}

__STATIC_FORCEINLINE uint32_t __get_ICC_IAR0_EL1(void)
{
	uint32_t result;
    __MRS(ssICC_IAR0_EL1, &result);
    return result;
}

__STATIC_FORCEINLINE uint32_t __get_ICC_IAR1_EL1(void)
{
	uint32_t result;
    __MRS(ssICC_IAR1_EL1, &result);
    return result;
}

/** \brief  Set __set_ICC_IGRPEN0 EL1
    \param [in]    igrpen0  VBAR value to set
 */
__STATIC_FORCEINLINE void __set_ICC_IGRPEN1_EL1(uint64_t igrpen1)
{
	__MSR(ssICC_IGRPEN1_EL1, igrpen1);
  //__ASM volatile("MSR  icc_igrpen1_el1, %0" : : "r" (igrpen1) : "memory");
}

//__STATIC_FORCEINLINE void __set_ICC_SGI1R_EL1(uint64_t value)
//{
//	__MSR(ssICC_SGI1R_EL1, value);
//}

__STATIC_FORCEINLINE void __set_ICC_SGI0R_EL1(uint64_t value)
{
	__MSR(ssICC_SGI0R_EL1, value);
}

#else /* defined (__aarch64__) */

/** \brief  AArch64 System registers to access the Generic Interrupt Controller CPU interface
*/
#if defined(__GNUC__)
	#define sICC_BPR0_EL1           "p15, 0, %0, C12, C8, 3"
	#define sICC_BPR1_EL1           "p15, 0, %0, C12, C12, 3"
	#define sICC_CTLR_EL1           "p15, 0, %0, C12, C12, 4"
	#define sICC_CTLR_EL3           "p15, 6, %0, C12, C12, 4"	// p15, op1 CRn CRm op2
	#define sICC_EOIR0_EL1          "p15, 0, %0, C12, C8, 1"
	#define sICC_EOIR1_EL1          "p15, 0, %0, C12, C12, 1"
	#define sICC_HPPIR0_EL1         "p15, 0, %0, C12, C8, 2"
	#define sICC_HPPIR1_EL1         "p15, 0, %0, C12, C12, 2"
	#define sICC_IAR0_EL1           "p15, 0, %0, C12, C8, 0"
	#define sICC_IAR1_EL1           "p15, 0, %0, C12, C12, 0"
	#define sICC_IGRPEN0_EL1        "p15, 0, %0, C12, C12, 6"
	#define sICC_IGRPEN1_EL1        "p15, 0, %0, C12, C12, 7"
	#define sICC_IGRPEN1_EL3        "p15, 6, %0, C12, C12, 7"
	#define sICC_PMR_EL1            "p15, 0, %0, C4, C6, 0"
	#define sICC_RPR_EL1            "p15, 0, %0, C12, C11, 3"
	#define sICC_SGI0R_EL1          "p15, 0, %0, C12, C11, 7"
	#define sICC_SGI1R_EL1          "p15, 0, %0, C12, C11, 5"
	#define sICC_SRE_EL1            "p15, 0, %0, C12, C12, 5"
	#define sICC_SRE_EL2            "p15, 4, %0, C12, C9, 5"
	#define sICC_SRE_EL3            "p15, 6, %0, C12, C12, 5"
#endif /* __GNUC__ */

// write to register
#define __MCR32(sysreg, val) \
  __asm volatile ("MCR " sysreg "\n" : : "r"((uint32_t)(val)))

// read from regioster
#define __MRC32(sysreg, pVal) \
  __asm volatile ("MRC  " sysreg "\n" : "=r"((*pVal)))

__STATIC_FORCEINLINE uint32_t __get_ICC_HPPIR0_EL1(void)
{
	uint32_t result;
    __MRC32(sICC_HPPIR0_EL1, &result);
    return result;
}

__STATIC_FORCEINLINE uint32_t __get_ICC_HPPIR1_EL1(void)
{
	uint32_t result;
    __MRC32(sICC_HPPIR1_EL1, &result);
    return result;
}

__STATIC_FORCEINLINE uint32_t __get_ICC_BPR0_EL1(void)
{
	uint32_t result;
    __MRC32(sICC_BPR0_EL1, &result);
    return result;
}

__STATIC_FORCEINLINE uint32_t __get_ICC_BPR1_EL1(void)
{
	uint32_t result;
    __MRC32(sICC_BPR1_EL1, &result);
    return result;
}

__STATIC_FORCEINLINE uint32_t __get_ICC_IAR0_EL1(void)
{
	uint32_t result;
    __MRC32(sICC_IAR0_EL1, &result);
    return result;
}

__STATIC_FORCEINLINE uint32_t __get_ICC_IAR1_EL1(void)
{
	uint32_t result;
    __MRC32(sICC_IAR1_EL1, &result);
    return result;
}

__STATIC_FORCEINLINE uint32_t __get_ICC_PMR_EL1(void)
{
	uint32_t result;
    __MRC32(sICC_PMR_EL1, &result);
    return result;
}

__STATIC_FORCEINLINE void __set_ICC_EOIR0_EL1(uint32_t value)
{
    __MCR32(sICC_EOIR0_EL1, value);
}

__STATIC_FORCEINLINE void __set_ICC_EOIR1_EL1(uint32_t value)
{
    __MCR32(sICC_EOIR1_EL1, value);
}

__STATIC_FORCEINLINE void __set_ICC_BPR0_EL1(uint32_t value)
{
    __MCR32(sICC_BPR0_EL1, value);
}

__STATIC_FORCEINLINE void __set_ICC_BPR1_EL1(uint32_t value)
{
    __MCR32(sICC_BPR1_EL1, value);
}

__STATIC_FORCEINLINE void __set_ICC_IGRPEN0_EL1(uint32_t value)
{
    __MCR32(sICC_IGRPEN0_EL1, value);
}

__STATIC_FORCEINLINE void __set_ICC_IGRPEN1_EL1(uint32_t value)
{
    __MCR32(sICC_IGRPEN1_EL1, value);
}

__STATIC_FORCEINLINE void __set_ICC_PMR_EL1(uint32_t value)
{
    __MCR32(sICC_PMR_EL1, value);
}

__STATIC_FORCEINLINE void __set_ICC_SGI1R_EL1(uint64_t value)
{
    __MCR32(sICC_SGI1R_EL1, value);
}

#endif	/* defined (__aarch64__) */

/** \brief Enable the CPU's interrupt interface.
*/
__STATIC_INLINE void GIC_EnableInterface(void)
{
	__set_ICC_IGRPEN0_EL1(1);
	__set_ICC_IGRPEN1_EL1(1);
}

/** \brief Disable the CPU's interrupt interface.
*/
__STATIC_INLINE void GIC_DisableInterface(void)
{
	__set_ICC_IGRPEN0_EL1(0);
	__set_ICC_IGRPEN1_EL1(0);
}

/** \brief Read the CPU's IAR register.
* \return GICInterface_Type::IAR
*/
__STATIC_INLINE IRQn_Type GIC_AcknowledgePending(void)
{
    return (IRQn_Type) __get_ICC_IAR1_EL1();
}

/** \brief Read the CPU's IAR register.
* \return GICInterface_Type::IAR
*/
__STATIC_INLINE IRQn_Type GIC_AcknowledgePendingG0(void)
{
    return (IRQn_Type) __get_ICC_IAR0_EL1();
}

/** \brief Writes the given interrupt number to the CPU's EOIR register.
* \param [in] IRQn The interrupt to be signaled as finished.
*/
__STATIC_INLINE void GIC_EndInterrupt(IRQn_Type IRQn)
{
	__set_ICC_EOIR1_EL1((uint32_t)IRQn);
}

/** \brief Writes the given interrupt number to the CPU's EOIR register.
* \param [in] IRQn The interrupt to be signaled as finished.
*/
__STATIC_INLINE void GIC_EndInterruptG0(IRQn_Type IRQn)
{
	__set_ICC_EOIR0_EL1((uint32_t)IRQn);
}

/** \brief Set the interrupt priority mask using CPU's PMR register.
* \param [in] priority Priority mask to be set.
*/
__STATIC_INLINE void GIC_SetInterfacePriorityMask(uint32_t priority)
{
	__set_ICC_PMR_EL1(priority & 0xFFUL);
}

/** \brief Read the current interrupt priority mask from CPU's PMR register.
* \result GICInterface_Type::PMR
*/
__STATIC_INLINE uint32_t GIC_GetInterfacePriorityMask(void)
{
    return __get_ICC_PMR_EL1() & 0xFFUL;
}

/** \brief Configures the group priority and subpriority split point using CPU's BPR register.
* \param [in] binary_point Amount of bits used as subpriority.
*/
__STATIC_INLINE void GIC_SetBinaryPoint(uint32_t binary_point)
{
	__set_ICC_BPR0_EL1(binary_point & 7U);
	__set_ICC_BPR1_EL1(binary_point & 7U);
}

/** \brief Read the current group priority and subpriority split point from CPU's BPR register.
* \return GICInterface_Type::BPR
*/
__STATIC_INLINE uint32_t GIC_GetBinaryPoint(void)
{
    return __get_ICC_BPR1_EL1();
}

/** \brief Get the interrupt number of the highest interrupt pending from CPU's HPPIR register.
* \return GICInterface_Type::HPPIR
*/
__STATIC_INLINE uint32_t GIC_GetHighPendingIRQ(void)
{
    return __get_ICC_HPPIR1_EL1();
}

/** \brief Get the interrupt number of the highest interrupt pending from CPU's HPPIR register.
* \return GICInterface_Type::HPPIR
*/
__STATIC_INLINE uint32_t GIC_GetHighPendingIRQG0(void)
{
    return __get_ICC_HPPIR0_EL1();
}

#endif /* GIC_INTERFACE_BASE */

/** \brief Get the status for a given interrupt.
* \param [in] IRQn The interrupt to get status for.
* \return 0 - not pending/active, 1 - pending, 2 - active, 3 - pending and active
*/
__STATIC_INLINE uint32_t GIC_GetIRQStatus(IRQn_Type IRQn)
{
  uint32_t pending, active;

  active = ((GICDistributor->ISACTIVER[IRQn / 32U])  >> (IRQn % 32U)) & 1UL;
  pending = ((GICDistributor->ISPENDR[IRQn / 32U]) >> (IRQn % 32U)) & 1UL;

  return ((active<<1U) | pending);
}

/* ICC_SGIR */
#define ICC_SGIR_TARGETLIST_SHIFT (0)
#define ICC_SGIR_TARGETLIST_MASK  (0xffff)
#define ICC_SGIR_AFF_MASK         (0xff)
#define ICC_SGIR_AFF1_SHIFT       (16)
#define ICC_SGIR_INTID_SHIFT      (24)
#define ICC_SGIR_INTID_MASK       (0xf)
#define ICC_SGIR_AFF2_SHIFT       (32)
#define ICC_SGIR_IRM_SHIFT        (40)
#define ICC_SGIR_IRM_MASK         (0x1)
#define ICC_SGIR_RS_SHIFT         (44)
#define ICC_SGIR_RS_MASK          (0xf)
#define ICC_SGIR_AFF3_SHIFT       (48)

#define COMPOSE_ICC_SGIR_VALUE(aff3, aff2, aff1, intid, irm, rs, tlist) \
    ((((uint64_t)(aff3) & ICC_SGIR_AFF_MASK) << ICC_SGIR_AFF3_SHIFT) |  \
     (((uint64_t)(rs) & ICC_SGIR_RS_MASK) << ICC_SGIR_RS_SHIFT) |       \
     (((uint64_t)(irm) & ICC_SGIR_IRM_MASK) << ICC_SGIR_IRM_SHIFT) |    \
     (((uint64_t)(aff2) & ICC_SGIR_AFF_MASK) << ICC_SGIR_AFF2_SHIFT) |  \
     (((intid) & ICC_SGIR_INTID_MASK) << ICC_SGIR_INTID_SHIFT) |        \
     (((aff1) & ICC_SGIR_AFF_MASK) << ICC_SGIR_AFF1_SHIFT) |            \
     (((tlist) & ICC_SGIR_TARGETLIST_MASK) << ICC_SGIR_TARGETLIST_SHIFT))

#define MPIDR_TO_RS(mpidr) (MPIDR_TO_AFF_LEVEL(mpidr, 0) >> 4)

/** \brief Generate a software interrupt (Affinity Routing version).
* \param [in] IRQn Software interrupt to be generated.
* \param [in] target_aff Target affinity in MPIDR form.
* \param [in] tlist List of CPUs the software interrupt should be forwarded to.
*/
__STATIC_INLINE void GIC_SendSGI_ARE(IRQn_Type IRQn, uint64_t target_aff, uint16_t tlist)
{
    uint32_t aff3, aff2, aff1, rs;
    uint64_t val;

    if (IRQn >= 16)
        return;

    aff1 = MPIDR_TO_AFF_LEVEL(target_aff, 1);
    aff2 = MPIDR_TO_AFF_LEVEL(target_aff, 2);
    aff3 = MPIDR_TO_AFF_LEVEL(target_aff, 3);
    rs = MPIDR_TO_RS(target_aff);
    val = COMPOSE_ICC_SGIR_VALUE(aff3, aff2, aff1, IRQn, 0, rs, tlist);

    rs = 0; /* RangeSelector */
    val =
    		(target_aff & 0xFF) * (UINT64_C(1) << 16) |	/* aff1 */
    		(target_aff & 0xFF) * (UINT64_C(1) << 32) |	/* aff2 */
    		(target_aff & 0xFF) * (UINT64_C(1) << 48) |	/* aff3 */
    		(1) * (UINT64_C(1) << 40) |	/* IRM */
    		(rs & 0x0F) * (UINT64_C(1) << 44) |	/* RangeSelector RS */
    		(IRQn & 0xF) * (UINT64_C(1) << 24) | /* INTID */
			(tlist & 0xFF) * (UINT64_C(1) << 0) |
		0;

    __DSB();
    __set_ICC_SGI1R_EL1(val);
    __set_ICC_SGI0R_EL1(val);	/* added by mgs */
    __ISB();
}

/** \brief Calculate log2 rounded up
*  - log(0)  => 0
*  - log(1)  => 0
*  - log(2)  => 1
*  - log(3)  => 2
*  - log(4)  => 2
*  - log(5)  => 3
*        :      :
*  - log(16) => 4
*  - log(32) => 5
*        :      :
* \param [in] n input value parameter
* \return log2(n)
*/
__STATIC_FORCEINLINE uint8_t __log2_upX(uint32_t n)
{
  if (n < 2U) {
    return 0U;
  }
  uint8_t log = 0U;
  uint32_t t = n;
  while(t > 1U)
  {
    log++;
    t >>= 1U;
  }
  if (n & 1U) { log++; }
  return log;
}

/** \brief Generate a software interrupt using GIC's SGIR register.
* \param [in] IRQn Software interrupt to be generated.
* \param [in] target_list List of CPUs the software interrupt should be forwarded to.
* \param [in] filter_list Filter to be applied to determine interrupt receivers.
*/
__STATIC_INLINE void GIC_SendSGI(IRQn_Type IRQn, uint32_t target_list, uint32_t filter_list)
{
	if (IRQn >= 16)
		return;

	const uint64_t target_aff = __log2_upX(target_list);
	if (GIC_GetARE()) {
		/* affinity routing */
		GIC_SendSGI_ARE(IRQn, target_aff, target_list);
	} else {
		//GICDistributor->SGIR = ((target_list & 0xFFUL) << 16U) | (IRQn & 0x0FUL);
	}
}

/** \brief Generate a software interrupt using GIC's SGIR register.
* \param [in] IRQn Software interrupt to be generated.
* \param [in] target_list List of CPUs the software interrupt should be forwarded to.
* \param [in] filter_list Filter to be applied to determine interrupt receivers.
*/
__STATIC_INLINE void GIC_SendSGI_v2(IRQn_Type IRQn, uint32_t target_list, uint32_t filter_list)
{
  //GICDistributor->SGIR = ((filter_list & 3U) << 24U) | ((target_list & 0xFFUL) << 16U) | (IRQn & 0x0FUL);
}

#if defined (GIC_INTERFACE_BASE)

/** \brief Get the interrupt number of the highest interrupt pending from CPU's HPPIR register.
* \return GICInterface_Type::HPPIR
*/
__STATIC_INLINE uint32_t GIC_GetHighPendingIRQ(void)
{
  return GICInterface->HPPIR;
}

/** \brief Provides information about the implementer and revision of the CPU interface.
* \return GICInterface_Type::IIDR
*/
__STATIC_INLINE uint32_t GIC_GetInterfaceId(void)
{
  return GICInterface->IIDR;
}
#endif /* GIC_INTERFACE_BASE */

#if defined (GIC_DISTRIBUTOR_BASE)
/** \brief Set the interrupt group from the GIC's IGROUPR register.
* \param [in] IRQn The interrupt to be queried.
* \param [in] group Interrupt group number: 0 - Group 0, 1 - Group 1
*/
__STATIC_INLINE void GIC_SetGroup(IRQn_Type IRQn, uint32_t group)
{
  uint32_t igroupr = GICDistributor->IGROUPR[IRQn / 32U];
  uint32_t shift   = (IRQn % 32U);

  igroupr &= (~(1U          << shift));
  igroupr |= ( (group & 1U) << shift);

  GICDistributor->IGROUPR[IRQn / 32U] = igroupr;
}
#define GIC_SetSecurity         GIC_SetGroup

/** \brief Get the interrupt group from the GIC's IGROUPR register.
* \param [in] IRQn The interrupt to be queried.
* \return 0 - Group 0, 1 - Group 1
*/
__STATIC_INLINE uint32_t GIC_GetGroup(IRQn_Type IRQn)
{
  return (GICDistributor->IGROUPR[IRQn / 32U] >> (IRQn % 32U)) & 1UL;
}
#define GIC_GetSecurity         GIC_GetGroup



#if defined(GIC_REDISTRIBUTOR_BASE)

#define GIC_REDISTRIBUTOR_STRIDE (0x20000)
#define GICR_SGI_BASE_OFF        (0x10000)
#define GICR_WAKER_PS_SHIFT (1)
#define GICR_WAKER_CA_SHIFT (2)

/** \brief Get the Redistributor SGI_base.
*/
__STATIC_INLINE void *GIC_GetRdistSGIBase(void *rd_base)
{
    return (void *)((uintptr_t)rd_base + GICR_SGI_BASE_OFF);
}


static uint64_t sread_mpidr(void)
{
#if __aarch64__
	return __get_MPIDR_EL1();
#else /* defined(__aarch64__) */
	return __get_MPIDR();
#endif /* defined(__aarch64__) */
}

/** \brief Get the recomposed MPIDR_EL1 Affinity fields.
* the recomposed Affinity value format is (aff3:aff2:aff1:aff0)
*/
__STATIC_INLINE uint32_t GIC_MPIDRtoAffinity(void)
{
    uint32_t aff3, aff2, aff1, aff0, aff;
    uint64_t mpidr = sread_mpidr();

    aff0 = MPIDR_TO_AFF_LEVEL(mpidr, 0);
    aff1 = MPIDR_TO_AFF_LEVEL(mpidr, 1);
    aff2 = MPIDR_TO_AFF_LEVEL(mpidr, 2);
    aff3 = MPIDR_TO_AFF_LEVEL(mpidr, 3);

    aff = (aff0 & MPIDR_AFFLVL_MASK) << 0 |
          (aff1 & MPIDR_AFFLVL_MASK) << 8 |
          (aff2 & MPIDR_AFFLVL_MASK) << 16 |
          (aff3 & MPIDR_AFFLVL_MASK) << 24;

    return aff;
}

#define GIC_REDISTRIBUTOR_STRIDE (0x20000)
#define GICR_SGI_BASE_OFF        (0x10000)

#define GICR_TYPER_LAST_SHIFT (4)
#define GICR_TYPER_LAST_MASK  (1 << GICR_TYPER_LAST_SHIFT)
#define GICR_TYPER_AFF_SHIFT  (32)

/** \brief Get the Redistributor base.
*/
__STATIC_INLINE GICRedistributor_Type *GIC_GetRdist(void)
{
    uintptr_t rd_addr = GIC_REDISTRIBUTOR_BASE;
    const uint32_t aff = GIC_MPIDRtoAffinity();
    uint64_t rd_typer;

  do {
	    uint32_t rd_aff;
	    rd_typer = ((GICRedistributor_Type *)rd_addr)->TYPER;
	    //PRINTF("rd_typer=%08X\n", (unsigned) rd_typer);
        rd_aff = rd_typer >> GICR_TYPER_AFF_SHIFT;
        rd_aff = rd_typer & (0xFF << 8);

        if (rd_aff == aff)
            return (GICRedistributor_Type *)rd_addr;

        rd_addr += GIC_REDISTRIBUTOR_STRIDE;
    } while (!(rd_typer & GICR_TYPER_LAST_MASK));

    return NULL;
}

/**
 *
 */
__STATIC_INLINE uint32_t GIC_GetRedistPriority(IRQn_Type IRQn)
{
    GICDistributor_Type *const s_RedistPPIBaseAddrs = (GICDistributor_Type *)GIC_GetRdistSGIBase(GIC_GetRdist());

    return (s_RedistPPIBaseAddrs->IPRIORITYR[IRQn / 4U] >> ((IRQn % 4U) * 8U)) & 0xFFUL;
}

/**
 *
 */
__STATIC_INLINE void GIC_RedistWakeUp(void)
{
  GICRedistributor_Type *const s_RedistBaseAddrs = GIC_GetRdist();

    if (!s_RedistBaseAddrs)
        return;

    if (!(s_RedistBaseAddrs->WAKER & (1 << GICR_WAKER_CA_SHIFT)))
        return;

  s_RedistBaseAddrs->WAKER &= ~ (1 << GICR_WAKER_PS_SHIFT);
    while (s_RedistBaseAddrs->WAKER & (1 << GICR_WAKER_CA_SHIFT))
        ;
}

/**
 *
 */
__STATIC_INLINE void GIC_SetRedistPriority(IRQn_Type IRQn, uint32_t priority)
{
    GICDistributor_Type *const s_RedistPPIBaseAddrs = (GICDistributor_Type *)GIC_GetRdistSGIBase(GIC_GetRdist());
    const uint32_t mask = s_RedistPPIBaseAddrs->IPRIORITYR[IRQn / 4U] & ~(0xFFUL << ((IRQn % 4U) * 8U));

    s_RedistPPIBaseAddrs->IPRIORITYR[IRQn / 4U] = mask | ((priority & 0xFFUL) << ((IRQn % 4U) * 8U));
}

/** \brief Initialize the interrupt redistributor.
*/
__STATIC_INLINE void GIC_RedistInit(void)
{
    uint32_t i;
    uint32_t priority_field;

  /* Priority level is implementation defined.
   To determine the number of priority bits implemented write 0xFF to an IPRIORITYR
   priority field and read back the value stored.*/
    GIC_SetRedistPriority((IRQn_Type)31U, 0xFFU);
    priority_field = GIC_GetRedistPriority((IRQn_Type)31U);

  /* Wakeup the GIC */
    GIC_RedistWakeUp();

    for (i = 0; i < 32; i++)
    {
      //Disable the SPI interrupt
        GIC_DisableIRQ((IRQn_Type)i);
      //Set priority
      GIC_SetRedistPriority((IRQn_Type)i, priority_field*2U/3U);
    }
}
#endif /* GIC_REDISTRIBUTOR_BASE */


/* ctrl register access in non-secure */
#define GICD_CTLR_RWP     31
#define GICD_CTLR_ARE_NS  4
#define GICD_CTLR_ENGRP1A 1
#define GICD_CTLR_ENGRP1  0

#define GICR_CTLR_RWP 3

enum gic_rwp {
    GICD_RWP,
    GICR_RWP,
};

#define GICV6_BIT(pos) (UINT32_C(1) << (pos))

/** \brief Wait for register write pending.
*/
//__STATIC_INLINE void GIC_WaitRWP(enum gic_rwp rwp)
//{
//	volatile int ii;
//	for (ii = 10000000; ii --;) __NOP();
//	return;
//    uint32_t rwp_mask;
//    uint32_t __IM *base;
//
//  if (rwp == GICR_RWP) {
//        base = &GIC_GetRdist()->CTLR;
//        if (!base)
//            return;
//        rwp_mask = GICV6_BIT(GICR_CTLR_RWP);
//  } else if (rwp == GICD_RWP) {
//        base = &GICDistributor->CTLR;
//        rwp_mask = GICV6_BIT(GICD_CTLR_RWP);
//  } else {
//        return;
//    }
//
//    while (*base & rwp_mask)
//        ;
//}

/** \brief Initialize the interrupt distributor.
*/
__STATIC_INLINE void GIC_DistInitZ(void)
{
    uint32_t i;
    uint32_t num_irq = 0U;
    uint32_t priority_field;
    uint32_t ppi_priority;

  //A reset sets all bits in the IGROUPRs corresponding to the SPIs to 0,
  //configuring all of the interrupts as Secure.

  //Disable interrupt forwarding
    GIC_DisableDistributor();
  //Get the maximum number of interrupts that the GIC supports
    num_irq = 32U * ((GIC_DistributorInfo() & 0x1FU) + 1U);

  /* Priority level is implementation defined.
   To determine the number of priority bits implemented write 0xFF to an IPRIORITYR
   priority field and read back the value stored.
   Use PPI, as it is always accessible, even for a Guest OS using a hypervisor.
   Then restore the initial state.*/
    ppi_priority = GIC_GetPriority((IRQn_Type)31U);
    GIC_SetPriority((IRQn_Type)31U, 0xFFU);
    priority_field = GIC_GetPriority((IRQn_Type)31U);
    GIC_SetPriority((IRQn_Type)31U, ppi_priority);

    for (i = 32U; i < num_irq; i++)
    {
      /* Use non secure group1 for all SPI */
        GIC_SetGroup((IRQn_Type) i, 1);
      //Disable the SPI interrupt
        GIC_DisableIRQ((IRQn_Type)i);
      //Set level-sensitive (and N-N model)
        GIC_SetConfiguration((IRQn_Type)i, 0U);
      //Set priority
      GIC_SetPriority((IRQn_Type)i, priority_field*2U/3U);
    }

  /* Enable distributor with ARE_NS and NS_Group1 */
    GICDistributor->CTLR = ((1U << GICD_CTLR_ARE_NS) | (1U << GICD_CTLR_ENGRP1A));
    //GIC_WaitRWP(GICD_RWP);
    GIC_DistributorWait();
}

/** \brief Initialize the interrupt distributor.
*/
__STATIC_INLINE void GIC_DistInit(void)
{
  uint32_t i;
  uint32_t num_irq = 0U;
  uint32_t priority_field;
  uint32_t ppi_priority;

  //A reset sets all bits in the IGROUPRs corresponding to the SPIs to 0,
  //configuring all of the interrupts as Secure.

  //Disable interrupt forwarding
  GIC_DisableDistributor();
  //Get the maximum number of interrupts that the GIC supports
  num_irq = 32U * ((GIC_DistributorInfo() & 0x1FU) + 1U);

  /* Priority level is implementation defined.
   To determine the number of priority bits implemented write 0xFF to an IPRIORITYR
   priority field and read back the value stored.
   Use PPI, as it is always accessible, even for a Guest OS using a hypervisor.
   Then restore the initial state.*/
    ppi_priority = GIC_GetPriority((IRQn_Type)31U);
    GIC_SetPriority((IRQn_Type)31U, 0xFFU);
    priority_field = GIC_GetPriority((IRQn_Type)31U);
    GIC_SetPriority((IRQn_Type)31U, ppi_priority);

  for (i = 32U; i < num_irq; i++)
  {
      //Disable the SPI interrupt
      GIC_DisableIRQ((IRQn_Type)i);
      //Set level-sensitive (and N-N model)
      GIC_SetConfiguration((IRQn_Type)i, 0U);
      //Set priority
      GIC_SetPriority((IRQn_Type)i, priority_field/2U);
      //Set target list to CPU0
      GIC_SetTarget((IRQn_Type)i, 1U);
  }

  /* Enable distributor with ARE_NS and NS_Group1 */
//    GICDistributor->CTLR = ((1U << GICD_CTLR_ARE_NS) | (1U << GICD_CTLR_ENGRP1A));
//    GIC_DistributorWait();
   //GIC_WaitRWP(GICD_RWP);
    //Enable distributor
    GIC_EnableDistributor();
}
#endif /* GIC_DISTRIBUTOR_BASE */

/** \brief Initialize the CPU's interrupt interface
*/
__STATIC_INLINE void GIC_CPUInterfaceInit(void)
{
  uint32_t i;
  uint32_t priority_field;

  //A reset sets all bits in the IGROUPRs corresponding to the SPIs to 0,
  //configuring all of the interrupts as Secure.

  //Disable interrupt forwarding
  GIC_DisableInterface();

  /* Priority level is implementation defined.
   To determine the number of priority bits implemented write 0xFF to an IPRIORITYR
   priority field and read back the value stored.*/
  GIC_SetPriority((IRQn_Type)0U, 0xFFU);
  priority_field = GIC_GetPriority((IRQn_Type)0U);

  //SGI and PPI
  for (i = 0U; i < 32U; i++)
  {
    if(i > 15U) {
      //Set level-sensitive (and N-N model) for PPI
      GIC_SetConfiguration((IRQn_Type)i, 0U);
    }
    //Disable SGI and PPI interrupts
    GIC_DisableIRQ((IRQn_Type)i);
    //Set priority
    GIC_SetPriority((IRQn_Type)i, priority_field/2U);
  }
  //Enable interface
  GIC_EnableInterface();
  //Set binary point to 0
  GIC_SetBinaryPoint(0U);
  //Set priority mask
  GIC_SetInterfacePriorityMask(0xFFU);
}

/** \brief Initialize and enable the GIC
*/
__STATIC_INLINE void GIC_Enable(void)
{
	// int init_dist
  GIC_DistInit();
  GIC_CPUInterfaceInit(); //per CPU
}

#endif /* (__GIC_PRESENT == 1U)) || defined(DOXYGEN) */

#endif /* INC_GICV6_H_ */
