/*
 * gic600_a.h
 *
 *  Created on: Oct 11, 2026
 *      Author: Gena
 */

#ifndef SRC_GIC600_GIC600_A_H_
#define SRC_GIC600_GIC600_A_H_

#include <stdint.h>

#define GICD_BASE       ((uintptr_t) 0x03400000)
#define GICR_BASE       ((uintptr_t) 0x03460000)

#define TIMER1_0_IRQn   131  /* Allwinner A733 Hardware SPI ID for Timer */

#include <stdint.h>

#define GICD_BASE       ((uintptr_t) 0x03400000)

typedef struct {
    volatile uint32_t CTLR;              /* 0x0000 - Distributor Control Register */
    const volatile uint32_t TYPER;       /* 0x0004 - Interrupt Controller Type Register */
    const volatile uint32_t IIDR;        /* 0x0008 - Implementer Identification Register */
    uint32_t RESERVED0;              /* Pad 0x000C to 0x007F */

    volatile uint32_t STATUSR;           /* 0x0080 - Status Register */
    uint32_t RESERVED1[31];          /* Pad 0x0084 to 0x00FF */

    /* Arrays for 1024 interrupts (32 registers each) */
    volatile uint32_t IGROUPR[32];   /* 0x0100 - Interrupt Group Registers */
    volatile uint32_t ISENABLER[32]; /* 0x0180 - Interrupt Set-Enable Registers */
    volatile uint32_t ICENABLER[32]; /* 0x0200 - Interrupt Clear-Enable Registers */

    uint32_t RESERVED2[224];         /* Pad from 0x0280 to 0x05FF */

    /* Configuration array (2 bits per INTID, 16 INTIDs per 32-bit register -> 64 registers total) */
    volatile uint32_t ICFGR[64];     /* 0x0600 - Interrupt Configuration Registers */

    uint32_t RESERVED3[448];         /* Pad from 0x0700 to 0x0DFF */

    volatile uint32_t IGRPMODR[32];  /* 0x0D00 - Interrupt Group Modifier Registers */

    uint32_t RESERVED4[352];         /* Pad from 0x0E80 to 0x13FF */

    /* 64-bit Interrupt Routing Registers for SPIs (1 register per INTID) */
    /* SPIs start from index 32, indexes 0-31 are reserved/unimplemented in GICD */
    volatile uint64_t IROUTER[1024]; /* 0x1400 - Interrupt Routing Registers */
} GICD_Type;

#define GICD ((GICD_Type *) GICD_BASE)
/* GIC Redistributor (GICR) register structure mapping per core (128 KB total) */
typedef struct {
    /* RD_base: Control page (First 64 KB) */
    volatile uint32_t CTLR;        /* 0x0000 - Redistributor Control Register */
    const volatile uint32_t IIDR;  /* 0x0004 - Implementer Identification Register */
    const volatile uint64_t TYPER; /* 0x0008 - Type Register */
    volatile uint32_t STATUSR;     /* 0x0010 - Status Register */
    volatile uint32_t WAKER;       /* 0x0014 - Wake Request Register */
    uint32_t RESERVED0;            /* Offset pad to the next 64 KB page */

    /* SGI_base: SGI/PPI frame page (Second 64 KB) */
    uint32_t RESERVED1;
    volatile uint32_t IGROUPR0;    /* 0x0080 - Interrupt Group Register 0 (SGIs/PPIs) */
    uint32_t RESERVED2;
    volatile uint32_t ISENABLER0;  /* 0x0100 - Interrupt Set-Enable Register 0 */
    uint32_t RESERVED3;
    volatile uint32_t ICENABLER0;  /* 0x0180 - Interrupt Clear-Enable Register 0 */
    uint32_t RESERVED4;
    volatile uint32_t IGRPMODR0;   /* 0x0D00 - Interrupt Group Modifier Register 0 */
} GICR_Type;

/* CMSIS style built-in wrappers for ARM system registers (__set_xxx / __get_xxx) */
static inline void __set_ICC_SRE_EL3(uint64_t value) {
    __asm volatile("msr ICC_SRE_EL3, %0" : : "r" (value));
}

static inline void __set_ICC_PMR_EL1(uint64_t value) {
    __asm volatile("msr ICC_PMR_EL1, %0" : : "r" (value));
}

static inline void __set_ICC_IGRPEN1_EL3(uint64_t value) {
    __asm volatile("msr ICC_IGRPEN1_EL3, %0" : : "r" (value));
}

static inline void __set_SCR_EL3(uint64_t value) {
    __asm volatile("msr SCR_EL3, %0" : : "r" (value));
}

static inline uint64_t __get_SCR_EL3(void) {
    uint64_t value;
    __asm volatile("mrs %0, SCR_EL3" : "=r" (value));
    return value;
}

#ifndef __ISB
static inline  void __ISB(void)
{
	__asm volatile ("isb":::"memory");
}
#endif

#endif /* SRC_GIC600_GIC600_A_H_ */
