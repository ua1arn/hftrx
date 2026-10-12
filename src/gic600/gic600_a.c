/*
 * gic600_a.c
 *
 *  Created on: Oct 11, 2026
 *      Author: Gena
 */

#include "hardware.h"

#if __aarch64__ && CPUSTYLE_A733
#include "formats.h"

//void GICv3_Distributor_Init(void) {
//   /* Disable Distributor before configuration changes */
//    //GICDistributor->CTLR = 0;
//    GICDistributor->CTLR = (1U << 6) | (1U << 5) | (1U << 4) | (1U << 1) | (1U << 0);
//    GICDistributor->CTLR = 0x77;
//    /* Wait for Register Write Pending (RWP) bit to clear */
////    while (GICDistributor->STATUSR & (1 << 4));
//	while ((GICDistributor->CTLR & (UINT32_C(1) << 31)) != 0)
//		;
//
//	PRINTF("GICDistributor->CTLR=0x%08X\n", (unsigned) GICDistributor->CTLR);
//
//    /* Route peripheral SPI interrupts (e.g. indexes 32 to 95) to Non-Secure Group 1 */
//    for (int i = 1; i < 32; i++) {
//        GICDistributor->IGROUPR[i]  = 1*0xFFFFFFFF; /* 1 = Group 1 */
//        GICDistributor->IGRPMODR[i] = 0x00000000; /* 0 = Non-Secure (when coupled with IGROUPR=1) */
//    }
//
//    /* Enable Distributor with Affinity Routing Enabled (ARE) for both Secure and Non-Secure.
//       Acknowledge only Group 1 Non-Secure enabled context. */
//    GICDistributor->CTLR = (1 << 4) | (1 << 5) | (1 << 1);
//
//    /* Ensure synchronization across the memory interconnect */
//    //while (GICDistributor->STATUSR & (1 << 4));
//	while ((GICDistributor->CTLR & (UINT32_C(1) << 31)) != 0)
//		;
//}

/**
 * @brief Configures TIMER1_0_IRQn (131), sets its priority, trigger type, and routes it using a core mask.
 * @param priority: Interrupt priority value (0x00 to 0xFF, where lower value means higher priority).
 * @param cpu_mask: Bitmask of target cores (Bit 0 = CPU0, Bit 1 = CPU1, ..., Bit 7 = CPU7).
 */
//void GICv3_Configure_Interrupt(uint32_t int_id, uint8_t priority, uint8_t cpu_mask) {
//
//    /* 1. Disable the targeted interrupt line before altering its configurations */
//    GICDistributor->GICD_ICENABLER[int_id / 32] = (1U << (int_id % 32));
//    GIC_DistributorWait();
//
//    /* 2. Group Allocation: Direct the SPI towards Non-Secure Group 1 context */
////    GICDistributor->IGROUPR[int_id / 32]  |= (1U << (int_id % 32));  /* 1 = Group 1 */
////    GICDistributor->IGRPMODR[int_id / 32] &= ~(1U << (int_id % 32)); /* 0 = Non-Secure */
//
//    /* 3. Priority Configuration: Set the 8-bit priority value
//       Each 32-bit register holds 4 priority fields. */
//    uint32_t prio_index = int_id / 4;
//    uint32_t prio_shift = (int_id % 4) * 8;
//
//    if (0) {
//    	   uint32_t prio_val = GICDistributor->GICD_IPRIORITYR[prio_index];
//    	    prio_val &= ~(0xFFU << prio_shift);          /* Clear old priority byte */
//    	    prio_val |= ((uint32_t)priority << prio_shift); /* Set new priority byte */
//    	    GICDistributor->GICD_IPRIORITYR[prio_index] = prio_val;
//    }
//
//    /* 4. Trigger Mapping: Enforce Level-Sensitive mode (00b) */
//    uint32_t config_index = int_id / 16;
//    uint32_t config_bit_shift = (int_id % 16) * 2;
//
//    uint32_t icfgr_val = GICDistributor->GICD_ICFGR[config_index];
//    icfgr_val &= ~(3U << config_bit_shift);      /* 00b = Level-Sensitive */
//    GICDistributor->GICD_ICFGR[config_index] = icfgr_val;
//
//    /* 5. Routing Mapping: Convert the user cpu_mask into GICv3 IROUTER format */
//    if ((cpu_mask == 0) || (cpu_mask & (cpu_mask - 1))) {
//        /* If multiple bits are set in the mask (or mask is 0), enable 1-of-N routing mode.
//           Bit 31 = 1 means the interrupt can be handled by ANY operational core. */
//        GICDistributor->GICD_IROUTER[int_id] = (UINT64_C(1) << 31);
//    } else {
//       /* Exactly one core is selected in the mask. Find its index (0 to 7). */
//        uint32_t core_index = 0;
//        while ((cpu_mask & (1U << core_index)) == 0) {
//            core_index++;
//        }
//
//        /*
//         * Construct the GICv3 target affinity for Allwinner A733.
//         * As proven by the log, the hardware topology places Core ID in Aff1:
//         * MPIDR_EL1 layout for A733: Aff3=0, Aff2=0, Aff1=core_index, Aff0=0.
//         * target_affinity format: (Aff1 << 8)
//         */
//        uint64_t target_affinity = ((uint64_t)core_index << 8);
//        GICDistributor->GICD_IROUTER[int_id] = target_affinity; /* Route strictly to the selected core (Bit 31 = 0) */
//        ASSERT(GICDistributor->GICD_IROUTER[int_id] == target_affinity);
//    }
//
//    /* 6. Enable the configured interrupt line back */
//    GICDistributor->GICD_ISENABLER[int_id / 32] = (1U << (int_id % 32));
//    GIC_DistributorWait();
//}

///**
// * @brief Reads the Multiprocessor Affinity Register (MPIDR_EL1).
// * @return 64-bit hardware affinity value.
// */
//static inline uint64_t __get_MPIDR_EL1(void) {
//    uint64_t value;
//    __asm volatile("mrs %0, MPIDR_EL1" : "=r" (value));
//    return value;
//}


///**
// * @brief Extracts a linear core index compatible with GICv3/v4 offset math,
// *        supporting both traditional (Cortex-A53) and DynamIQ (Cortex-A76/A55) layouts.
// * @return Linear core ID (typically 0 to 7 depending on the SoC topology).
// */
//static inline uint32_t Get_Linear_Core_ID(void) {
//    uint64_t mpidr = __get_MPIDR_EL1();
//    uint32_t core_id;
//
//    /* Check Bit 24 (MT - Multi-threading flag) to determine layout style */
//    if (mpidr & (1U << 24)) {
//        /*
//         * DynamIQ Layout (e.g., Allwinner A733 Cortex-A76 / Cortex-A55):
//         * - MT bit is 1.
//         * - Aff1 (bits [15:8]) represents the Core ID.
//         * - Aff0 (bits [7:0]) represents the Thread ID (always 0).
//         */
//        core_id = (uint32_t)((mpidr >> 8) & 0xFFU);
//    } else {
//        /*
//         * Traditional Layout (e.g., Cortex-A53 / Cortex-A7):
//         * - MT bit is 0.
//         * - Aff1 (bits [15:8]) represents the Cluster ID.
//         * - Aff0 (bits [7:0]) represents the Core ID inside the cluster.
//         */
//        core_id = (uint32_t)(mpidr & 0xFFU);
//
//        /*
//         * Optional: If dealing with multi-cluster Cortex-A53 chips (like 8-core big.LITTLE),
//         * you can factor in the cluster ID (Aff1) to prevent core ID overlapping:
//         * uint33_t cluster_id = (uint32_t)((mpidr >> 8) & 0xFFU);
//         * core_id = (cluster_id * 4) + core_id;
//         */
//    }
//
//    return core_id;
//}

/**
 * @brief Checks if the CPU core supports ARM GICv3/v4 System Registers.
 * @return 1 if GICv3/v4 is supported, 0 if only GICv2 (or legacy) is available.
 */
static uint32_t Is_GICv3_Supported(void) {
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

//static void Hardware_Interrupt_System_Init(void) {
//    uint32_t core_id = arm_hardware_cpuid();
//
//    if (Is_GICv3_Supported()) {
//        /* Safe to execute your GIC-600 / GICv3 code for Allwinner A733 */
//        if (core_id == 0) {
//            GICv3_Distributor_Init();
//        }
//        GICv3_Core_Init(core_id);
//    } else {
//        /* STOP or fallback to GICv2 legacy initialization here */
//        /* Do NOT touch GICR or ICC registers here to prevent hardfaults */
//    }
//}

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
	//GICv3_Distributor_Init();
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

void gic600_local_init(void)
{
	GICv3_Core_Init(Get_Linear_Core_ID());
}

void gic600_test(void)
{
//	PRINTF("GIC_GetARE()=%d\n", GIC_GetARE());
//	GICv3_Fix_Routing_Mode(TIMER1_0_IRQn);
	unsigned irqn = TIMER1_1_IRQn;
	PRINTF("1 GICDistributor->GICD_CTLR=%08X, GICDistributor->GICD_IROUTER [0x%03X]=%08X\n", (unsigned) GICDistributor->GICD_CTLR, (unsigned) irqn, (unsigned) GICDistributor->GICD_IROUTER [irqn]);
	GIC_SetTarget(irqn, UINT32_C(1) << 1);
	PRINTF("3 GICDistributor->GICD_CTLR=%08X, GICDistributor->GICD_IROUTER [0x%03X]=%08X\n", (unsigned) GICDistributor->GICD_CTLR, (unsigned) irqn, (unsigned) GICDistributor->GICD_IROUTER [irqn]);
}
#else
void gic600_global_init(void)
{
}

void gic600_local_init(void)
{
}

void gic600_test(void)
{
}

#endif

