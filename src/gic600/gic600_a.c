/*
 * gic600_a.c
 *
 *  Created on: Oct 11, 2026
 *      Author: Gena
 */

#include "gic600_a.h"
#include <stdint.h>

void GICv3_Distributor_Init(void) {
    /* Disable Distributor before configuration changes */
    GICD->CTLR = 0;

    /* Wait for Register Write Pending (RWP) bit to clear */
    while (GICD->STATUSR & (1 << 4));

    /* Route peripheral SPI interrupts (e.g. indexes 32 to 95) to Non-Secure Group 1 */
    for (int i = 1; i < 3; i++) {
        GICD->IGROUPR[i]  = 0xFFFFFFFF; /* 1 = Group 1 */
        GICD->IGRPMODR[i] = 0x00000000; /* 0 = Non-Secure (when coupled with IGROUPR=1) */
    }

    /* Enable Distributor with Affinity Routing Enabled (ARE) for both Secure and Non-Secure.
       Acknowledge only Group 1 Non-Secure enabled context. */
    GICD->CTLR = (1 << 4) | (1 << 5) | (1 << 1);

    /* Ensure synchronization across the memory interconnect */
    while (GICD->STATUSR & (1 << 4));
}
void GICv3_Core_Init(uint32_t core_id) {
    /* Calculate precise pointer to the target core's 128 KB Redistributor frame using uintptr_t math */
    GICR_Type *GICR = (GICR_Type *)(GICR_BASE + (core_id * (uintptr_t)0x20000));

    /* Step 1: Wake up the core's Redistributor interface */
    uint32_t waker = GICR->WAKER;
    waker &= ~(1 << 1); /* Clear ProcessorSleep bit */
    GICR->WAKER = waker;

    /* Wait until the interface is operational (ChildrenAsleep bit must become 0) */
    while (GICR->WAKER & (1 << 2));

    /* Step 2: Route local core interrupts (SGIs 0-15 and PPIs 16-31) to Non-Secure Group 1 */
    GICR->IGROUPR0  = 0xFFFFFFFF; /* 1 = Group 1 */
    GICR->IGRPMODR0 = 0x00000000; /* 0 = Non-Secure */

    /* Step 3: Enable System Register Access (SRE) via the CPU interface */
    __set_ICC_SRE_EL3(0xF);
    __ISB(); /* Force architectural register state changes to execute immediately */

    /* Open the interrupt priority filter gateway completely (0xFF allows all priorities) */
    __set_ICC_PMR_EL1(0xFF);

    /* Enable signaling of Non-Secure Group 1 interrupts to the core */
    __set_ICC_IGRPEN1_EL3(1);

    /* Step 4: Configure routing at the core architecture level (SCR_EL3) */
    uint64_t scr = __get_SCR_EL3();
    scr &= ~(1 << 2); /* Clear FIQ bit: No FIQ handling at EL3 */
    scr |= (1 << 1);  /* Set IRQ bit: Route Non-Secure Group 1 interrupts to lower EL levels as IRQ */
    scr |= (1 << 10); /* Set RW bit: Enforce Execution at lower ELs to run in 64-bit mode (AArch64) */
    __set_SCR_EL3(scr);

    __ISB(); /* Final pipeline instruction synchronization */
}

/**
 * @brief Configures TIMER1_0_IRQn (131) as Level-Sensitive and routes it via GICD_IROUTER.
 * @param mpidr: Target core affinity mapping byte layout from MPIDR_EL1 register.
 */
void GICv3_Configure_Timer_Interrupt(uint64_t mpidr) {
    uint32_t int_id = TIMER1_0_IRQn; /* 131 */

    /* 1. Disable the targeted interrupt line before altering its structural properties */
    GICD->ICENABLER[int_id / 32] = (1U << (int_id % 32));
    while (GICD->STATUSR & (1 << 4)); /* Wait for Register Write Pending (RWP) to clear */

    /* 2. Group Allocation: Direct the SPI towards Non-Secure Group 1 context */
    GICD->IGROUPR[int_id / 32]  |= (1U << (int_id % 32));  /* 1 = Group 1 */
    GICD->IGRPMODR[int_id / 32] &= ~(1U << (int_id % 32)); /* 0 = Non-Secure */

    /* 3. Trigger Mapping: Set as Level-Sensitive (High level)
       Each INTID utilizes 2 bits configuration inside the register block.
       Bit 1 of the pair represents: 0 = Level-Sensitive, 1 = Edge-Triggered. */
    uint32_t config_index = int_id / 16;
    uint32_t config_bit_shift = (int_id % 16) * 2;

    uint32_t icfgr_val = GICD->ICFGR[config_index];
    icfgr_val &= ~(3U << config_bit_shift); /* Clear both bits to enforce Level-Sensitive mode (00b) */
    GICD->ICFGR[config_index] = icfgr_val;

    /* 4. Target Routing: Assign delivery to the exact CPU target fetched from 'mpidr' */
    uint64_t target_affinity = mpidr & 0xFF00FFFFFFULL; /* Clear specific execution flags from MPIDR */
    GICD->IROUTER[int_id] = target_affinity;            /* Explicit routing execution (Mode Bit 31 = 0) */

    /* 5. Enable the configured interrupt line back */
    GICD->ISENABLER[int_id / 32] = (1U << (int_id % 32));
    while (GICD->STATUSR & (1 << 4)); /* Execute trailing hardware sync lock wait */
}
#include <stdint.h>

#include <stdint.h>

/**
 * @brief Reads the Multiprocessor Affinity Register (MPIDR_EL1).
 * @return 64-bit hardware affinity value.
 */
static inline uint64_t __get_MPIDR_EL1(void) {
    uint64_t value;
    __asm volatile("mrs %0, MPIDR_EL1" : "=r" (value));
    return value;
}

/**
 * @brief Extracts a linear core index compatible with the GICv3_Core_Init offset math.
 * @return Linear core ID from 0 to 7 based on Aff1 field for Allwinner A733.
 */
static inline uint32_t Get_Linear_Core_ID(void) {
    uint64_t mpidr = __get_MPIDR_EL1();

    /*
     * Based on actual hardware dump:
     * MPIDR_EL1 = 0x81000X00 (where X is the core number)
     * - Bit [24] (MT) = 1 (Multi-threading integrated layout)
     * - Aff1 (bits [15:8]) = Physical Core ID (0x00 to 0x07)
     * - Aff0 (bits [7:0])  = Thread ID (always 0x00)
     */
    uint32_t core_id = (uint32_t)((mpidr >> 8) & 0xFFU);

    return core_id;
}

void gic600_global_init(void)
{
	GICv3_Distributor_Init();
}

void gic600_local_init(void)
{

}

void gic600_test(void)
{

}
