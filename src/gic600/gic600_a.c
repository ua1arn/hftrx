/*
 * gic600_a.c
 *
 *  Created on: Oct 11, 2026
 *      Author: Gena
 */

#if __aarch64__
//#include "hardware.h"
#include "gic600_a.h"
#include <stdint.h>

void GICv3_Distributor_Init(void) {
    /* Disable Distributor before configuration changes */
    GICD->CTLR = 0;

    /* Wait for Register Write Pending (RWP) bit to clear */
    while (GICD->STATUSR & (1 << 4));

    /* Route peripheral SPI interrupts (e.g. indexes 32 to 95) to Non-Secure Group 1 */
    for (int i = 1; i < 32; i++) {
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
    GICR_Type *GICR = (GICR_Type *)(GIC_REDISTRIBUTOR_BASE + (core_id * (uintptr_t)0x10000));

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
 //   __set_ICC_PMR_EL1(0xFF);

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
 * @brief Configures TIMER1_0_IRQn (131), sets its priority, trigger type, and routes it using a core mask.
 * @param priority: Interrupt priority value (0x00 to 0xFF, where lower value means higher priority).
 * @param cpu_mask: Bitmask of target cores (Bit 0 = CPU0, Bit 1 = CPU1, ..., Bit 7 = CPU7).
 */
void GICv3_Configure_Timer_Interrupt(uint8_t priority, uint8_t cpu_mask) {
    uint32_t int_id = TIMER1_0_IRQn; /* 131 */

    /* 1. Disable the targeted interrupt line before altering its configurations */
    GICD->ICENABLER[int_id / 32] = (1U << (int_id % 32));
    while (GICD->STATUSR & (1 << 4)); /* Wait for Register Write Pending (RWP) to clear */

    /* 2. Group Allocation: Direct the SPI towards Non-Secure Group 1 context */
    GICD->IGROUPR[int_id / 32]  |= (1U << (int_id % 32));  /* 1 = Group 1 */
    GICD->IGRPMODR[int_id / 32] &= ~(1U << (int_id % 32)); /* 0 = Non-Secure */

    /* 3. Priority Configuration: Set the 8-bit priority value
       Each 32-bit register holds 4 priority fields. */
    uint32_t prio_index = int_id / 4;
    uint32_t prio_shift = (int_id % 4) * 8;

    if (0) {
    	   uint32_t prio_val = GICD->IPRIORITYR[prio_index];
    	    prio_val &= ~(0xFFU << prio_shift);          /* Clear old priority byte */
    	    prio_val |= ((uint32_t)priority << prio_shift); /* Set new priority byte */
    	    GICD->IPRIORITYR[prio_index] = prio_val;
    }

    /* 4. Trigger Mapping: Enforce Level-Sensitive mode (00b) */
    uint32_t config_index = int_id / 16;
    uint32_t config_bit_shift = (int_id % 16) * 2;

    uint32_t icfgr_val = GICD->ICFGR[config_index];
    icfgr_val &= ~(3U << config_bit_shift);      /* 00b = Level-Sensitive */
    GICD->ICFGR[config_index] = icfgr_val;

    /* 5. Routing Mapping: Convert the user cpu_mask into GICv3 IROUTER format */
    if ((cpu_mask == 0) || (cpu_mask & (cpu_mask - 1))) {
        /* If multiple bits are set in the mask (or mask is 0), enable 1-of-N routing mode.
           Bit 31 = 1 means the interrupt can be handled by ANY operational core. */
        GICD->IROUTER[int_id] = (1ULL << 31);
    } else {
        /* Exactly one core is selected in the mask. Find its index (0 to 7). */
        uint32_t core_index = 0;
        while ((cpu_mask & (1U << core_index)) == 0) {
            core_index++;
        }

        /*
         * Construct the GICv3 target affinity for Allwinner A733.
         * As proven by the log, the hardware topology places Core ID in Aff1:
         * MPIDR_EL1 layout for A733: Aff3=0, Aff2=0, Aff1=core_index, Aff0=0.
         * target_affinity format: (Aff1 << 8)
         */
        uint64_t target_affinity = ((uint64_t)core_index << 8);
        GICD->IROUTER[int_id] = target_affinity; /* Route strictly to the selected core (Bit 31 = 0) */
    }

    /* 6. Enable the configured interrupt line back */
    GICD->ISENABLER[int_id / 32] = (1U << (int_id % 32));
    while (GICD->STATUSR & (1 << 4)); /* Final RWP hardware synchronization wait */
}

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
 * @brief Extracts a linear core index compatible with GICv3/v4 offset math,
 *        supporting both traditional (Cortex-A53) and DynamIQ (Cortex-A76/A55) layouts.
 * @return Linear core ID (typically 0 to 7 depending on the SoC topology).
 */
static inline uint32_t Get_Linear_Core_ID(void) {
    uint64_t mpidr = __get_MPIDR_EL1();
    uint32_t core_id;

    /* Check Bit 24 (MT - Multi-threading flag) to determine layout style */
    if (mpidr & (1U << 24)) {
        /*
         * DynamIQ Layout (e.g., Allwinner A733 Cortex-A76 / Cortex-A55):
         * - MT bit is 1.
         * - Aff1 (bits [15:8]) represents the Core ID.
         * - Aff0 (bits [7:0]) represents the Thread ID (always 0).
         */
        core_id = (uint32_t)((mpidr >> 8) & 0xFFU);
    } else {
        /*
         * Traditional Layout (e.g., Cortex-A53 / Cortex-A7):
         * - MT bit is 0.
         * - Aff1 (bits [15:8]) represents the Cluster ID.
         * - Aff0 (bits [7:0]) represents the Core ID inside the cluster.
         */
        core_id = (uint32_t)(mpidr & 0xFFU);

        /*
         * Optional: If dealing with multi-cluster Cortex-A53 chips (like 8-core big.LITTLE),
         * you can factor in the cluster ID (Aff1) to prevent core ID overlapping:
         * uint33_t cluster_id = (uint32_t)((mpidr >> 8) & 0xFFU);
         * core_id = (cluster_id * 4) + core_id;
         */
    }

    return core_id;
}

/**
 * @brief Checks if the CPU core supports ARM GICv3/v4 System Registers.
 * @return 1 if GICv3/v4 is supported, 0 if only GICv2 (or legacy) is available.
 */
uint32_t Is_GICv3_Supported(void) {
    uint64_t pfr0 = __get_ID_AA64PFR0_EL1();

    /*
     * In ARM Architecture Reference Manual:
     * ID_AA64PFR0_EL1.GIC bits [27:24] specify GIC system register support:
     * - 0b0000: GICv3/v4 system register interface is NOT implemented (Legacy GICv2 mode).
     * - 0b0001: GICv3/v4 system register interface IS implemented.
     */
    uint32_t gic_status = (uint32_t)((pfr0 >> 24) & 0x0FU);

    return (gic_status == 1U) ? 1U : 0U;
}

void Hardware_Interrupt_System_Init(void) {
    uint32_t core_id = Get_Linear_Core_ID();

    if (1 || Is_GICv3_Supported()) {
        /* Safe to execute your GIC-600 / GICv3 code for Allwinner A733 */
        if (core_id == 0) {
            GICv3_Distributor_Init();
        }
        GICv3_Core_Init(core_id);
    } else {
        /* STOP or fallback to GICv2 legacy initialization here */
        /* Do NOT touch GICR or ICC registers here to prevent hardfaults */
    }
}

/* Usage example inside the primary core configuration block */
//void Setup_System_Timer(void) {
//    /* Get the raw MPIDR value of the current core (e.g. CPU0 = 0x81000000) */
//    uint64_t primary_mpidr = __get_MPIDR_EL1();
//
//    /* Route TIMER1_0_IRQn (131) directly to this core */
//    GICv3_Configure_Timer_Interrupt(primary_mpidr, UINT32_C(1) << 0);
//}

void gic600_global_init(void)
{
	GICv3_Distributor_Init();
}

void gic600_local_init(void)
{
	//GICv3_Core_Init(Get_Linear_Core_ID());
}

void gic600_test(void)
{
	//GICv3_Configure_Timer_Interrupt(0, UINT32_C(1) << 0);
}

#endif

