/*
 * core64_ca.h
 */
/* $Id$ */
//
// Проект HF Dream Receiver (КВ приёмник мечты)
// автор Гена Завидовский mgs2001@mail.ru
// UA1ARN
//

#ifndef INC_CORE64_CA_H_
#define INC_CORE64_CA_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// __ASM volatile("MSR DAIFClr, %0\n\t" : : "i" (7)  : "memory");

#define __get_RG32(reg, Rt)         __ASM volatile("MRS %0, " reg : "=r" (Rt) : : "memory" )
#define __set_RG32(reg, Rs)         __ASM volatile("MSR " reg ", %0" : : "r" (Rs) : "memory" )
#define __set_RG32C(reg, v)         __ASM volatile("MSR " reg ", #" # v : : : "memory" )
#define __get_RG64(reg, Rt)         __ASM volatile("MRS %0, " reg : "=r" (Rt) : : "memory" )
#define __set_RG64(reg, Rs)         __ASM volatile("MSR " reg ", %0" : : "r" (Rs) : "memory" )

/**
\brief   Send Event Local
\details  Send Event Local is a hint instruction that causes an event to be signaled locally without requiring the event to be  signaled to other PEs in the multiprocessor system.
*/
#define __SEVL()         do { __ASM volatile ("sevl"); } while (0)

 /**
  * This instruction is a hint instruction. Software with a multithreading capability can use a YIELD instruction to indicate to
  * the PE that it is performing a task, for example a spin-lock, that could be swapped out to improve overall system
  * performance. The PE can use this hint to suspend and resume multiple software threads if it supports the capability.
  */
#define __YIELD()         do { __ASM volatile ("YIELD"); } while (0)

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
 __STATIC_FORCEINLINE uint8_t __log2_up(uint32_t n)
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

/**
\brief   Load-acquire exclusive register byte (8 bit)
\details Executes a LDAB exclusive instruction for 8 bit value.
\param [in]    ptr  Pointer to data
\return             value of type uint8_t at (*ptr)
*/
__STATIC_FORCEINLINE uint32_t __LDAXRB(const volatile uint8_t *ptr)
{
	uint32_t result;

	__ASM volatile ("ldaxrb %w0, %1" : "=r" (result) : "Q" (*ptr) : "memory");
	return result;    /* Add explicit type cast here */
}

/**
\brief   Store-Release Exclusive (8 bit)
\details Executes a STLB exclusive instruction for 8 bit values.
\param [in]  value  Value to store
\param [in]    ptr  Pointer to location
\return          0  Function succeeded
\return          1  Function failed
*/
__STATIC_FORCEINLINE uint32_t __STXRB(uint32_t value, volatile uint8_t *ptr)
{
	uint32_t result;

	__ASM volatile ("stxrb %w0, %w2, %1" : "=&r" (result), "=Q" (*ptr) : "r" (value) : "memory");
	return (result);
}

/**
\brief   Store-release register byte
\details Executes a STLB exclusive instruction for 8 bit values.
\param [in]  value  Value to store
\param [in]    ptr  Pointer to location
*/
__STATIC_FORCEINLINE void __STLRB(uint32_t value, volatile uint8_t *ptr)
{
	__ASM volatile ("stlrb %w1, %0" : "=Q" (*ptr) : "r" (value) : "memory" );
}


__STATIC_FORCEINLINE uint64_t __get_SP64(void)
{
	uint64_t result;
	__ASM volatile("MOV  %0, sp" : "=r" (result) : : "memory");
	return result;
}

__STATIC_FORCEINLINE uint32_t __get_MIDR_EL1(void)
{
	uint32_t result;
	// MRS <Xt>, MIDR_EL1 ; Read MIDR_EL1 into Xt
	__get_RG32("MIDR_EL1", result);
	return result;
}

#if 0
__STATIC_FORCEINLINE uint64_t __get_MPIDR_EL1(void)
{
	uint64_t result;
	// MRS <Xt>, MPIDR_EL1 ; Read MPIDR_EL1 into Xt
	__get_RG64("MPIDR_EL1", result);
	return result;
}

__STATIC_FORCEINLINE uint64_t __get_TTBR0_EL1(void)
{
	uint64_t result;
	// MRS <Xt>, TTBR0_EL1 ; Read TTBR0_EL1 into Xt
	__get_RG64("TTBR0_EL1", result);
	return result;
}

__STATIC_FORCEINLINE uint64_t __get_TTBR0_EL3(void)
{
	uint64_t result;
	// MRS <Xt>, TTBR0_EL3 ; Read TTBR0_EL3 into Xt
	__get_RG64("TTBR0_EL3", result);
	return result;
}

__STATIC_FORCEINLINE uint64_t __get_FAR_EL1(void)
{
	uint64_t result;
	__get_RG64("FAR_EL1", result);
	return result;
}

__STATIC_FORCEINLINE uint64_t __get_FAR_EL2(void)
{
	uint64_t result;
	__get_RG64("FAR_EL2", result);
	return result;
}

// 4.3.57 Exception Syndrome Register, EL1
// ESR_EL1 is architecturally mapped to AArch32 register DFSR
__STATIC_FORCEINLINE uint32_t __get_ESR_EL1(void)
{
	uint32_t result;
	__get_RG32("ESR_EL1", result);
	return result;
}

__STATIC_FORCEINLINE uint64_t __get_CLIDR_EL1(void)
{
	uint64_t result;
	__get_RG64("CLIDR_EL1", result);
	return result;
}

__STATIC_FORCEINLINE uint64_t __get_CCSIDR_EL1(void)
{
	uint64_t result;
	__get_RG64("CCSIDR_EL1", result);
	return result;
}

__STATIC_FORCEINLINE uint64_t __get_CPUACTLR_EL1(void)
{
	uint64_t result;
	__get_RG64("S3_1_C15_C2_0", result);
	return result;
}

__STATIC_FORCEINLINE uint64_t __get_MAIR_EL3(void)
{
	uint64_t result;
	__get_RG64("MAIR_EL3", result);
	return result;
}

__STATIC_FORCEINLINE void __set_MAIR_EL1(uint64_t value)
{
	__set_RG64("MAIR_EL1", value);
}

__STATIC_FORCEINLINE void __set_MAIR_EL2(uint64_t value)
{
	__set_RG64("MAIR_EL2", value);
}

__STATIC_FORCEINLINE void __set_MAIR_EL3(uint64_t value)
{
	__set_RG64("MAIR_EL3", value);
}

__STATIC_FORCEINLINE void __set_VBAR_EL1(uint64_t value)
{
	// MSR VBAR_EL1, <Xt> ; Write Xt to VBAR_EL1
	__set_RG64("VBAR_EL1", value);
}

__STATIC_FORCEINLINE void __set_VBAR_EL2(uint64_t value)
{
	// MSR VBAR_EL2, <Xt> ; Write Xt to VBAR_EL2
	__set_RG64("VBAR_EL2", value);
}

__STATIC_FORCEINLINE void __set_VBAR_EL3(uint64_t value)
{
	// MSR VBAR_EL3, <Xt> ; Write Xt to VBAR_EL3
	__set_RG64("VBAR_EL3", value);
}
__STATIC_FORCEINLINE void __set_CSSELR_EL1(uint32_t value)
{
	// MSR CSSELR_EL1, <Xt> ; Write Xt to CSSELR_EL1
	__set_RG32("CSSELR_EL1", value);
}

__STATIC_FORCEINLINE void __set_CPUACTLR_EL1(uint64_t value)
{
	__set_RG64("S3_1_C15_C2_0", value);
}

// RVBAR_EL1, Reset Vector Base Address Register (if EL2 and EL3 not implemented)
__STATIC_FORCEINLINE void __set_RVBAR_EL1(uint64_t value)
{
	__set_RG64("RVBAR_EL1", value);
}
__STATIC_FORCEINLINE uint64_t __get_RVBAR_EL1(void)
{
	uint64_t result;
	__get_RG64("RVBAR_EL1", result);
	return result;
}

// RVBAR_EL2, Reset Vector Base Address Register (if EL3 not implemented)
__STATIC_FORCEINLINE void __set_RVBAR_EL2(uint64_t value)
{
	__set_RG64("RVBAR_EL2", value);
}
__STATIC_FORCEINLINE uint64_t __get_RVBAR_EL2(void)
{
	uint64_t result;
	__get_RG64("RVBAR_EL2", result);
	return result;
}

// RVBAR_EL3, Reset Vector Base Address Register (if EL3 implemented)
__STATIC_FORCEINLINE void __set_RVBAR_EL3(uint64_t value)
{
	__set_RG64("RVBAR_EL3", value);
}
__STATIC_FORCEINLINE uint64_t __get_RVBAR_EL3(void)
{
	uint64_t result;
	__get_RG64("RVBAR_EL3", result);
	return result;
}

__STATIC_FORCEINLINE uint32_t __get_DAIF(void)
{
	uint32_t result;
	// MRS <Xt>, DAIF ; Read DAIF into Xt
	__get_RG32("DAIF", result);
	return result;
}
#endif

__STATIC_FORCEINLINE uint64_t __get_VBAR_EL3(void)
{
	uint64_t result;
	// MRS <Xt>, VBAR_EL3 ; Read VBAR_EL3 into Xt
	__get_RG64("VBAR_EL3", result);
	return result;
}

__STATIC_FORCEINLINE uint64_t __get_ELR_EL3(void)
{
	uint64_t result;
	__get_RG64("ELR_EL3", result);
	return result;
}

__STATIC_FORCEINLINE uint64_t __get_FAR_EL3(void)
{
	uint64_t result;
	__get_RG64("FAR_EL3", result);
	return result;
}

__STATIC_FORCEINLINE uint64_t __get_CPUECTLR_EL1(void)
{
	uint64_t result;
	__get_RG64("S3_1_C15_C2_1", result);
	return result;
}

__STATIC_FORCEINLINE void __set_CPUECTLR_EL1(uint64_t value)
{
	__set_RG64("S3_1_C15_C2_1", value);
}

// 4.3.42 Secure Configuration Register
__STATIC_FORCEINLINE void __set_SCR_EL3(uint64_t value)
{
	// MSR CSSELR_EL1, <Xt> ; Write Xt to CSSELR_EL1
	__set_RG64("SCR_EL3", value);
}

__STATIC_FORCEINLINE uint64_t __get_SCR_EL3(void)
{
	uint64_t result;
	__get_RG64("SCR_EL3", result);
	return result;
}

__STATIC_FORCEINLINE uint64_t __get_ESR_EL3(void)
{
	uint64_t result;
	__get_RG64("ESR_EL3", result);
	return result;
}

__STATIC_FORCEINLINE void __set_DACR32_EL2(uint64_t value)
{
	// MSR DACR32_EL2, <Xt> ; Write Xt to DACR32_EL2
	__set_RG64("DACR32_EL2", value);
}

__STATIC_FORCEINLINE uint64_t __get_DACR32_EL2(void)
{
	uint64_t result;
	__get_RG64("DACR32_EL2", result);
	return result;
}

__STATIC_FORCEINLINE void __set_TTBR0_EL1(uint64_t value)
{
	// MSR TTBR0_EL1, <Xt> ; Write Xt to TTBR0_EL1
	__set_RG64("TTBR0_EL1", value);
}

__STATIC_FORCEINLINE void __set_TTBR0_EL2(uint64_t value)
{
	// MSR TTBR0_EL2, <Xt> ; Write Xt to TTBR0_EL2
	__set_RG64("TTBR0_EL2", value);
}

__STATIC_FORCEINLINE void __set_TCR_EL1(uint32_t value)
{
	// MSR TCR_EL1, <Xt> ; Write Xt to TCR_EL1
	__set_RG32("TCR_EL1", value);
}

__STATIC_FORCEINLINE void __set_TCR_EL2(uint32_t value)
{
	// MSR TCR_EL2, <Xt> ; Write Xt to TCR_EL2
	__set_RG32("TCR_EL2", value);
}

__STATIC_FORCEINLINE uint32_t __get_CTR_EL0(void)
{
	uint32_t result;
	__get_RG32("CTR_EL0", result);
	return result;
}


#if 1

#define SCTLR_EL1_DZE_Pos                     14U                                    /*!< \brief SCTLR: DZE Position */
#define SCTLR_EL1_DZE_Msk                     (UINT64_C(1) << SCTLR_EL1_DZE_Pos)                  /*!< \brief SCTLR: DZE Mask */

#define SCTLR_EL3_TE_Pos                     30U                                    /*!< \brief SCTLR: TE Position */
#define SCTLR_EL3_TE_Msk                     (UINT64_C(1) << SCTLR_EL3_TE_Pos)                  /*!< \brief SCTLR: TE Mask */

#define SCTLR_EL3_AFE_Pos                    29U                                    /*!< \brief SCTLR: AFE Position */
#define SCTLR_EL3_AFE_Msk                    (UINT64_C(1) << SCTLR_EL3_AFE_Pos)                 /*!< \brief SCTLR: AFE Mask */

#define SCTLR_EL3_TRE_Pos                    28U                                    /*!< \brief SCTLR: TRE Position */
#define SCTLR_EL3_TRE_Msk                    (UINT64_C(1) << SCTLR_EL3_TRE_Pos)                 /*!< \brief SCTLR: TRE Mask */

#define SCTLR_EL3_NMFI_Pos                   27U                                    /*!< \brief SCTLR: NMFI Position */
#define SCTLR_EL3_NMFI_Msk                   (UINT64_C(1) << SCTLR_EL3_NMFI_Pos)                /*!< \brief SCTLR: NMFI Mask */

#define SCTLR_EL3_EE_Pos                     25U                                    /*!< \brief SCTLR: EE Position */
#define SCTLR_EL3_EE_Msk                     (UINT64_C(1) << SCTLR_EL3_EE_Pos)                  /*!< \brief SCTLR: EE Mask */

#define SCTLR_EL3_VE_Pos                     24U                                    /*!< \brief SCTLR: VE Position */
#define SCTLR_EL3_VE_Msk                     (UINT64_C(1) << SCTLR_EL3_VE_Pos)                  /*!< \brief SCTLR: VE Mask */

#define SCTLR_EL3_U_Pos                      22U                                    /*!< \brief SCTLR: U Position */
#define SCTLR_EL3_U_Msk                      (UINT64_C(1) << SCTLR_EL3_U_Pos)                   /*!< \brief SCTLR: U Mask */

#define SCTLR_EL3_FI_Pos                     21U                                    /*!< \brief SCTLR: FI Position */
#define SCTLR_EL3_FI_Msk                     (UINT64_C(1) << SCTLR_EL3_FI_Pos)                  /*!< \brief SCTLR: FI Mask */

#define SCTLR_EL3_UWXN_Pos                   20U                                    /*!< \brief SCTLR: UWXN Position */
#define SCTLR_EL3_UWXN_Msk                   (UINT64_C(1) << SCTLR_EL3_UWXN_Pos)                /*!< \brief SCTLR: UWXN Mask */

#define SCTLR_EL3_WXN_Pos                    19U                                    /*!< \brief SCTLR: WXN Position */
#define SCTLR_EL3_WXN_Msk                    (UINT64_C(1) << SCTLR_EL3_WXN_Pos)                 /*!< \brief SCTLR: WXN Mask */

#define SCTLR_EL3_HA_Pos                     17U                                    /*!< \brief SCTLR: HA Position */
#define SCTLR_EL3_HA_Msk                     (UINT64_C(1) << SCTLR_EL3_HA_Pos)                  /*!< \brief SCTLR: HA Mask */

//#define SCTLR_EL3_V_Pos                      13U                                    /*!< \brief SCTLR: V Position */
//#define SCTLR_EL3_V_Msk                      (UINT64_C(1) << SCTLR_EL3_V_Pos)                   /*!< \brief SCTLR: V Mask */

#define SCTLR_EL3_I_Pos                      12U                                    /*!< \brief SCTLR: I Position */
#define SCTLR_EL3_I_Msk                      (UINT64_C(1) << SCTLR_EL3_I_Pos)                   /*!< \brief SCTLR: I Mask */

//#define SCTLR_EL3_Z_Pos                      11U                                    /*!< \brief SCTLR: Z Position */
//#define SCTLR_EL3_Z_Msk                      (UINT64_C(1) << SCTLR_EL3_Z_Pos)                   /*!< \brief SCTLR: Z Mask */

//#define SCTLR_EL3_SW_Pos                     10U                                    /*!< \brief SCTLR: SW Position */
//#define SCTLR_EL3_SW_Msk                     (UINT64_C(1) << SCTLR_EL3_SW_Pos)                  /*!< \brief SCTLR: SW Mask */

//#define SCTLR_EL3_B_Pos                      7U                                     /*!< \brief SCTLR: B Position */
//#define SCTLR_EL3_B_Msk                      (UINT64_C(1) << SCTLR_EL3_B_Pos)                   /*!< \brief SCTLR: B Mask */

#define SCTLR_EL3_NAA_Pos                6U                                     /*!< \brief SCTLR: NAA Position */
#define SCTLR_EL3_NAA_Msk                (UINT64_C(1) << SCTLR_EL3_NAA_Pos)             /*!< \brief SCTLR: NAA Mask */

//#define SCTLR_EL3_CP15BEN_Pos                5U                                     /*!< \brief SCTLR: CP15BEN Position */
//#define SCTLR_EL3_CP15BEN_Msk                (UINT64_C(1) << SCTLR_EL3_CP15BEN_Pos)             /*!< \brief SCTLR: CP15BEN Mask */

#define SCTLR_EL3_SA_Pos                 3U                                     /*!< \brief SCTLR: SA Position */
#define SCTLR_EL3_SA_Msk                 (UINT64_C(1) << SCTLR_EL3_SA_Pos)                   /*!< \brief SCTLR: SA Mask */

#define SCTLR_EL3_C_Pos                      2U                                     /*!< \brief SCTLR: C Position */
#define SCTLR_EL3_C_Msk                      (UINT64_C(1) << SCTLR_EL3_C_Pos)                   /*!< \brief SCTLR: C Mask */

#define SCTLR_EL3_A_Pos                      1U                                     /*!< \brief SCTLR: A Position */
#define SCTLR_EL3_A_Msk                      (UINT64_C(1) << SCTLR_EL3_A_Pos)                   /*!< \brief SCTLR: A Mask */

#define SCTLR_EL3_M_Pos                      0U                                     /*!< \brief SCTLR: M Position */
#define SCTLR_EL3_M_Msk                      (UINT64_C(1) << SCTLR_EL3_M_Pos)                   /*!< \brief SCTLR: M Mask */

#endif

/* CP15 Register ACTLR */
typedef union
{
#if __CORTEX_A == 5 || defined(DOXYGEN)
  /** \brief Structure used for bit access on Cortex-A5 */
  struct
  {
    uint32_t FW:1;                      /*!< \brief bit:      0  Cache and TLB maintenance broadcast */
    RESERVED(0:5, uint32_t)
    uint32_t SMP:1;                      /*!< \brief bit:     6  Enables coherent requests to the processor */
    uint32_t EXCL:1;                     /*!< \brief bit:     7  Exclusive L1/L2 cache control */
    RESERVED(1:2, uint32_t)
    uint32_t DODMBS:1;                   /*!< \brief bit:    10  Disable optimized data memory barrier behavior */
    uint32_t DWBST:1;                    /*!< \brief bit:    11  AXI data write bursts to Normal memory */
    uint32_t RADIS:1;                    /*!< \brief bit:    12  L1 Data Cache read-allocate mode disable */
    uint32_t L1PCTL:2;                   /*!< \brief bit:13..14  L1 Data prefetch control */
    uint32_t BP:2;                       /*!< \brief bit:16..15  Branch prediction policy */
    uint32_t RSDIS:1;                    /*!< \brief bit:    17  Disable return stack operation */
    uint32_t BTDIS:1;                    /*!< \brief bit:    18  Disable indirect Branch Target Address Cache (BTAC) */
    RESERVED(3:9, uint32_t)
    uint32_t DBDI:1;                     /*!< \brief bit:    28  Disable branch dual issue */
    RESERVED(7:3, uint32_t)
 } b;
#endif
#if __CORTEX_A == 7 || defined(DOXYGEN)
  /** \brief Structure used for bit access on Cortex-A7 */
  struct
  {
    RESERVED(0:6, uint32_t)
    uint32_t SMP:1;                      /*!< \brief bit:     6  Enables coherent requests to the processor */
    RESERVED(1:3, uint32_t)
    uint32_t DODMBS:1;                   /*!< \brief bit:    10  Disable optimized data memory barrier behavior */
    uint32_t L2RADIS:1;                  /*!< \brief bit:    11  L2 Data Cache read-allocate mode disable */
    uint32_t L1RADIS:1;                  /*!< \brief bit:    12  L1 Data Cache read-allocate mode disable */
    uint32_t L1PCTL:2;                   /*!< \brief bit:13..14  L1 Data prefetch control */
    uint32_t DDVM:1;                     /*!< \brief bit:    15  Disable Distributed Virtual Memory (DVM) transactions */
    RESERVED(3:12, uint32_t)
    uint32_t DDI:1;                      /*!< \brief bit:    28  Disable dual issue */
    RESERVED(7:3, uint32_t)
  } b;
#endif
#if __CORTEX_A == 9 || defined(DOXYGEN)
  /** \brief Structure used for bit access on Cortex-A9 */
  struct
  {
    uint32_t FW:1;                       /*!< \brief bit:     0  Cache and TLB maintenance broadcast */
    RESERVED(0:1, uint32_t)
    uint32_t L1PE:1;                     /*!< \brief bit:     2  Dside prefetch */
    uint32_t WFLZM:1;                    /*!< \brief bit:     3  Cache and TLB maintenance broadcast */
    RESERVED(1:2, uint32_t)
    uint32_t SMP:1;                      /*!< \brief bit:     6  Enables coherent requests to the processor */
    uint32_t EXCL:1;                     /*!< \brief bit:     7  Exclusive L1/L2 cache control */
    uint32_t AOW:1;                      /*!< \brief bit:     8  Enable allocation in one cache way only */
    uint32_t PARITY:1;                   /*!< \brief bit:     9  Support for parity checking, if implemented */
    RESERVED(7:22, uint32_t)
  } b;
#endif
  uint32_t w;                            /*!< \brief Type      used for word access */
} ACTLR_Type;

#define ACTLR_DDI_Pos                    28U                                     /*!< \brief ACTLR: DDI Position */
#define ACTLR_DDI_Msk                    (UINT32_C(1) << ACTLR_DDI_Pos)                  /*!< \brief ACTLR: DDI Mask */

#define ACTLR_DBDI_Pos                   28U                                     /*!< \brief ACTLR: DBDI Position */
#define ACTLR_DBDI_Msk                   (UINT32_C(1) << ACTLR_DBDI_Pos)                 /*!< \brief ACTLR: DBDI Mask */

#define ACTLR_BTDIS_Pos                  18U                                     /*!< \brief ACTLR: BTDIS Position */
#define ACTLR_BTDIS_Msk                  (UINT32_C(1) << ACTLR_BTDIS_Pos)                /*!< \brief ACTLR: BTDIS Mask */

#define ACTLR_RSDIS_Pos                  17U                                     /*!< \brief ACTLR: RSDIS Position */
#define ACTLR_RSDIS_Msk                  (UINT32_C(1) << ACTLR_RSDIS_Pos)                /*!< \brief ACTLR: RSDIS Mask */

#define ACTLR_BP_Pos                     15U                                     /*!< \brief ACTLR: BP Position */
#define ACTLR_BP_Msk                     (UINT32_C(3) << ACTLR_BP_Pos)                   /*!< \brief ACTLR: BP Mask */

#define ACTLR_DDVM_Pos                   15U                                     /*!< \brief ACTLR: DDVM Position */
#define ACTLR_DDVM_Msk                   (UINT32_C(1) << ACTLR_DDVM_Pos)                 /*!< \brief ACTLR: DDVM Mask */

#define ACTLR_L1PCTL_Pos                 13U                                     /*!< \brief ACTLR: L1PCTL Position */
#define ACTLR_L1PCTL_Msk                 (UINT32_C(3) << ACTLR_L1PCTL_Pos)               /*!< \brief ACTLR: L1PCTL Mask */

#define ACTLR_RADIS_Pos                  12U                                     /*!< \brief ACTLR: RADIS Position */
#define ACTLR_RADIS_Msk                  (UINT32_C(1) << ACTLR_RADIS_Pos)                /*!< \brief ACTLR: RADIS Mask */

#define ACTLR_L1RADIS_Pos                12U                                     /*!< \brief ACTLR: L1RADIS Position */
#define ACTLR_L1RADIS_Msk                (UINT32_C(1) << ACTLR_L1RADIS_Pos)              /*!< \brief ACTLR: L1RADIS Mask */

#define ACTLR_DWBST_Pos                  11U                                     /*!< \brief ACTLR: DWBST Position */
#define ACTLR_DWBST_Msk                  (UINT32_C(1) << ACTLR_DWBST_Pos)                /*!< \brief ACTLR: DWBST Mask */

#define ACTLR_L2RADIS_Pos                11U                                     /*!< \brief ACTLR: L2RADIS Position */
#define ACTLR_L2RADIS_Msk                (UINT32_C(1) << ACTLR_L2RADIS_Pos)              /*!< \brief ACTLR: L2RADIS Mask */

#define ACTLR_DODMBS_Pos                 10U                                     /*!< \brief ACTLR: DODMBS Position */
#define ACTLR_DODMBS_Msk                 (UINT32_C(1) << ACTLR_DODMBS_Pos)               /*!< \brief ACTLR: DODMBS Mask */

#define ACTLR_PARITY_Pos                 9U                                      /*!< \brief ACTLR: PARITY Position */
#define ACTLR_PARITY_Msk                 (UINT32_C(1) << ACTLR_PARITY_Pos)               /*!< \brief ACTLR: PARITY Mask */

#define ACTLR_AOW_Pos                    8U                                      /*!< \brief ACTLR: AOW Position */
#define ACTLR_AOW_Msk                    (UINT32_C(1) << ACTLR_AOW_Pos)                  /*!< \brief ACTLR: AOW Mask */

#define ACTLR_EXCL_Pos                   7U                                      /*!< \brief ACTLR: EXCL Position */
#define ACTLR_EXCL_Msk                   (UINT32_C(1) << ACTLR_EXCL_Pos)                 /*!< \brief ACTLR: EXCL Mask */

#define ACTLR_SMP_Pos                    6U                                      /*!< \brief ACTLR: SMP Position */
#define ACTLR_SMP_Msk                    (UINT32_C(1) << ACTLR_SMP_Pos)                  /*!< \brief ACTLR: SMP Mask */

#define ACTLR_WFLZM_Pos                  3U                                      /*!< \brief ACTLR: WFLZM Position */
#define ACTLR_WFLZM_Msk                  (UINT32_C(1) << ACTLR_WFLZM_Pos)                /*!< \brief ACTLR: WFLZM Mask */

#define ACTLR_L1PE_Pos                   2U                                      /*!< \brief ACTLR: L1PE Position */
#define ACTLR_L1PE_Msk                   (UINT32_C(1) << ACTLR_L1PE_Pos)                 /*!< \brief ACTLR: L1PE Mask */

#define ACTLR_FW_Pos                     0U                                      /*!< \brief ACTLR: FW Position */
#define ACTLR_FW_Msk                     (UINT32_C(1) << ACTLR_FW_Pos)                   /*!< \brief ACTLR: FW Mask */

/* CP15 Register CPACR */
typedef union
{
  struct
  {
    uint32_t CP0:2;                      /*!< \brief bit:  0..1  Access rights for coprocessor 0 */
    uint32_t CP1:2;                      /*!< \brief bit:  2..3  Access rights for coprocessor 1 */
    uint32_t CP2:2;                      /*!< \brief bit:  4..5  Access rights for coprocessor 2 */
    uint32_t CP3:2;                      /*!< \brief bit:  6..7  Access rights for coprocessor 3 */
    uint32_t CP4:2;                      /*!< \brief bit:  8..9  Access rights for coprocessor 4 */
    uint32_t CP5:2;                      /*!< \brief bit:10..11  Access rights for coprocessor 5 */
    uint32_t CP6:2;                      /*!< \brief bit:12..13  Access rights for coprocessor 6 */
    uint32_t CP7:2;                      /*!< \brief bit:14..15  Access rights for coprocessor 7 */
    uint32_t CP8:2;                      /*!< \brief bit:16..17  Access rights for coprocessor 8 */
    uint32_t CP9:2;                      /*!< \brief bit:18..19  Access rights for coprocessor 9 */
    uint32_t CP10:2;                     /*!< \brief bit:20..21  Access rights for coprocessor 10 */
    uint32_t CP11:2;                     /*!< \brief bit:22..23  Access rights for coprocessor 11 */
    uint32_t CP12:2;                     /*!< \brief bit:24..25  Access rights for coprocessor 11 */
    uint32_t CP13:2;                     /*!< \brief bit:26..27  Access rights for coprocessor 11 */
    uint32_t TRCDIS:1;                   /*!< \brief bit:    28  Disable CP14 access to trace registers */
    RESERVED(0:1, uint32_t)
    uint32_t D32DIS:1;                   /*!< \brief bit:    30  Disable use of registers D16-D31 of the VFP register file */
    uint32_t ASEDIS:1;                   /*!< \brief bit:    31  Disable Advanced SIMD Functionality */
  } b;                                   /*!< \brief Structure used for bit  access */
  uint32_t w;                            /*!< \brief Type      used for word access */
} CPACR_Type;

#define CPACR_ASEDIS_Pos                 31U                                    /*!< \brief CPACR: ASEDIS Position */
#define CPACR_ASEDIS_Msk                 (UINT32_C(1) << CPACR_ASEDIS_Pos)              /*!< \brief CPACR: ASEDIS Mask */

#define CPACR_D32DIS_Pos                 30U                                    /*!< \brief CPACR: D32DIS Position */
#define CPACR_D32DIS_Msk                 (UINT32_C(1) << CPACR_D32DIS_Pos)              /*!< \brief CPACR: D32DIS Mask */

#define CPACR_TRCDIS_Pos                 28U                                    /*!< \brief CPACR: D32DIS Position */
#define CPACR_TRCDIS_Msk                 (UINT32_C(1) << CPACR_D32DIS_Pos)              /*!< \brief CPACR: D32DIS Mask */

#define CPACR_CP_Pos_(n)                 (n*2U)                                 /*!< \brief CPACR: CPn Position */
#define CPACR_CP_Msk_(n)                 (UINT32_C(3) << CPACR_CP_Pos_(n))              /*!< \brief CPACR: CPn Mask */

#define CPACR_CP_NA                      0U                                     /*!< \brief CPACR CPn field: Access denied. */
#define CPACR_CP_PL1                     1U                                     /*!< \brief CPACR CPn field: Accessible from PL1 only. */
#define CPACR_CP_FA                      3U                                     /*!< \brief CPACR CPn field: Full access. */

/* CP15 Register DFSR */
typedef union
{
  struct
  {
    uint32_t FS0:4;                      /*!< \brief bit: 0.. 3  Fault Status bits bit 0-3 */
    uint32_t Domain:4;                   /*!< \brief bit: 4.. 7  Fault on which domain */
    RESERVED(0:1, uint32_t)
    uint32_t LPAE:1;                     /*!< \brief bit:     9  Large Physical Address Extension */
    uint32_t FS1:1;                      /*!< \brief bit:    10  Fault Status bits bit 4 */
    uint32_t WnR:1;                      /*!< \brief bit:    11  Write not Read bit */
    uint32_t ExT:1;                      /*!< \brief bit:    12  External abort type */
    uint32_t CM:1;                       /*!< \brief bit:    13  Cache maintenance fault */
    RESERVED(1:18, uint32_t)
  } s;                                   /*!< \brief Structure used for bit  access in short format */
  struct
  {
    uint32_t STATUS:5;                   /*!< \brief bit: 0.. 5  Fault Status bits */
    RESERVED(0:3, uint32_t)
    uint32_t LPAE:1;                     /*!< \brief bit:     9  Large Physical Address Extension */
    RESERVED(1:1, uint32_t)
    uint32_t WnR:1;                      /*!< \brief bit:    11  Write not Read bit */
    uint32_t ExT:1;                      /*!< \brief bit:    12  External abort type */
    uint32_t CM:1;                       /*!< \brief bit:    13  Cache maintenance fault */
    RESERVED(2:18, uint32_t)
  } l;                                   /*!< \brief Structure used for bit  access in long format */
  uint32_t w;                            /*!< \brief Type      used for word access */
} DFSR_Type;

#define DFSR_CM_Pos                      13U                                    /*!< \brief DFSR: CM Position */
#define DFSR_CM_Msk                      (UINT32_C(1) << DFSR_CM_Pos)                   /*!< \brief DFSR: CM Mask */

#define DFSR_Ext_Pos                     12U                                    /*!< \brief DFSR: Ext Position */
#define DFSR_Ext_Msk                     (UINT32_C(1) << DFSR_Ext_Pos)                  /*!< \brief DFSR: Ext Mask */

#define DFSR_WnR_Pos                     11U                                    /*!< \brief DFSR: WnR Position */
#define DFSR_WnR_Msk                     (UINT32_C(1) << DFSR_WnR_Pos)                  /*!< \brief DFSR: WnR Mask */

#define DFSR_FS1_Pos                     10U                                    /*!< \brief DFSR: FS1 Position */
#define DFSR_FS1_Msk                     (UINT32_C(1) << DFSR_FS1_Pos)                  /*!< \brief DFSR: FS1 Mask */

#define DFSR_LPAE_Pos                    9U                                    /*!< \brief DFSR: LPAE Position */
#define DFSR_LPAE_Msk                    (UINT32_C(1) << DFSR_LPAE_Pos)                /*!< \brief DFSR: LPAE Mask */

#define DFSR_Domain_Pos                  4U                                     /*!< \brief DFSR: Domain Position */
#define DFSR_Domain_Msk                  (UINT32_C(0x0F) << DFSR_Domain_Pos)             /*!< \brief DFSR: Domain Mask */

#define DFSR_FS0_Pos                     0U                                     /*!< \brief DFSR: FS0 Position */
#define DFSR_FS0_Msk                     (UINT32_C(0x0F) << DFSR_FS0_Pos)                /*!< \brief DFSR: FS0 Mask */

#define DFSR_STATUS_Pos                  0U                                     /*!< \brief DFSR: STATUS Position */
#define DFSR_STATUS_Msk                  (UINT32_C(0x3F) << DFSR_STATUS_Pos)            /*!< \brief DFSR: STATUS Mask */

/* CP15 Register IFSR */
typedef union
{
  struct
  {
    uint32_t FS0:4;                      /*!< \brief bit: 0.. 3  Fault Status bits bit 0-3 */
    RESERVED(0:5, uint32_t)
    uint32_t LPAE:1;                     /*!< \brief bit:     9  Large Physical Address Extension */
    uint32_t FS1:1;                      /*!< \brief bit:    10  Fault Status bits bit 4 */
    RESERVED(1:1, uint32_t)
    uint32_t ExT:1;                      /*!< \brief bit:    12  External abort type */
    RESERVED(2:19, uint32_t)
  } s;                                   /*!< \brief Structure used for bit access in short format */
  struct
  {
    uint32_t STATUS:6;                   /*!< \brief bit: 0.. 5  Fault Status bits */
    RESERVED(0:3, uint32_t)
    uint32_t LPAE:1;                     /*!< \brief bit:     9  Large Physical Address Extension */
    RESERVED(1:2, uint32_t)
    uint32_t ExT:1;                      /*!< \brief bit:    12  External abort type */
    RESERVED(2:19, uint32_t)
  } l;                                   /*!< \brief Structure used for bit access in long format */
  uint32_t w;                            /*!< \brief Type      used for word access */
} IFSR_Type;

#define IFSR_ExT_Pos                     12U                                    /*!< \brief IFSR: ExT Position */
#define IFSR_ExT_Msk                     (UINT32_C(1) << IFSR_ExT_Pos)                  /*!< \brief IFSR: ExT Mask */

#define IFSR_FS1_Pos                     10U                                    /*!< \brief IFSR: FS1 Position */
#define IFSR_FS1_Msk                     (UINT32_C(1) << IFSR_FS1_Pos)                  /*!< \brief IFSR: FS1 Mask */

#define IFSR_LPAE_Pos                    9U                                     /*!< \brief IFSR: LPAE Position */
#define IFSR_LPAE_Msk                    (0xUINT32_C(1) << IFSR_LPAE_Pos)               /*!< \brief IFSR: LPAE Mask */

#define IFSR_FS0_Pos                     0U                                     /*!< \brief IFSR: FS0 Position */
#define IFSR_FS0_Msk                     (UINT32_C(0x0F) << IFSR_FS0_Pos)                /*!< \brief IFSR: FS0 Mask */

#define IFSR_STATUS_Pos                  0U                                     /*!< \brief IFSR: STATUS Position */
#define IFSR_STATUS_Msk                  (UINT32_C(0x3F) << IFSR_STATUS_Pos)            /*!< \brief IFSR: STATUS Mask */

/* CP15 Register ISR */
typedef union
{
  struct
  {
    RESERVED(0:6, uint32_t)
    uint32_t F:1;                        /*!< \brief bit:     6  FIQ pending bit */
    uint32_t I:1;                        /*!< \brief bit:     7  IRQ pending bit */
    uint32_t A:1;                        /*!< \brief bit:     8  External abort pending bit */
    RESERVED(1:23, uint32_t)
  } b;                                   /*!< \brief Structure used for bit  access */
  uint32_t w;                            /*!< \brief Type      used for word access */
} ISR_Type;

#define ISR_A_Pos                        8U                                     /*!< \brief ISR: A Position */
#define ISR_A_Msk                        (1UL << ISR_A_Pos)                     /*!< \brief ISR: A Mask */

#define ISR_I_Pos                        7U                                     /*!< \brief ISR: I Position */
#define ISR_I_Msk                        (1UL << ISR_I_Pos)                     /*!< \brief ISR: I Mask */

#define ISR_F_Pos                        6U                                     /*!< \brief ISR: F Position */
#define ISR_F_Msk                        (1UL << ISR_F_Pos)                     /*!< \brief ISR: F Mask */

/* DACR Register */
#define DACR_D_Pos_(n)                   (2U*n)                                 /*!< \brief DACR: Dn Position */
#define DACR_D_Msk_(n)                   (UINT32_C(3) << DACR_D_Pos_(n))                /*!< \brief DACR: Dn Mask */
#define DACR_Dn_NOACCESS                 0U                                     /*!< \brief DACR Dn field: No access */
#define DACR_Dn_CLIENT                   1U                                     /*!< \brief DACR Dn field: Client */
#define DACR_Dn_MANAGER                  3U                                     /*!< \brief DACR Dn field: Manager */

/**
  \brief     Mask and shift a bit field value for use in a register bit range.
  \param [in] field  Name of the register bit field.
  \param [in] value  Value of the bit field. This parameter is interpreted as an uint32_t type.
  \return           Masked and shifted value.
*/
#define _VAL2FLD(field, value)    (((uint32_t)(value) << field ## _Pos) & field ## _Msk)

/**
  \brief     Mask and shift a register value to extract a bit field value.
  \param [in] field  Name of the register bit field.
  \param [in] value  Value of register. This parameter is interpreted as an uint32_t type.
  \return           Masked and shifted bit field value.
*/
#define _FLD2VAL(field, value)    (((uint32_t)(value) & field ## _Msk) >> field ## _Pos)


/**
 \brief  Union type to access the L2C_310 Cache Controller.
*/
#if (defined(__L2C_PRESENT) && (__L2C_PRESENT == 1U)) || \
     defined(DOXYGEN)
typedef struct
{
  __IM  uint32_t CACHE_ID;                   /*!< \brief Offset: 0x0000 (R/ ) Cache ID Register               */
  __IM  uint32_t CACHE_TYPE;                 /*!< \brief Offset: 0x0004 (R/ ) Cache Type Register             */
        RESERVED(0[0x3e], uint32_t)
  __IOM uint32_t CONTROL;                    /*!< \brief Offset: 0x0100 (R/W) Control Register                */
  __IOM uint32_t AUX_CNT;                    /*!< \brief Offset: 0x0104 (R/W) Auxiliary Control               */
        RESERVED(1[0x3e], uint32_t)
  __IOM uint32_t EVENT_CONTROL;              /*!< \brief Offset: 0x0200 (R/W) Event Counter Control           */
  __IOM uint32_t EVENT_COUNTER1_CONF;        /*!< \brief Offset: 0x0204 (R/W) Event Counter 1 Configuration   */
  __IOM uint32_t EVENT_COUNTER0_CONF;        /*!< \brief Offset: 0x0208 (R/W) Event Counter 1 Configuration   */
        RESERVED(2[0x2], uint32_t)
  __IOM uint32_t INTERRUPT_MASK;             /*!< \brief Offset: 0x0214 (R/W) Interrupt Mask                  */
  __IM  uint32_t MASKED_INT_STATUS;          /*!< \brief Offset: 0x0218 (R/ ) Masked Interrupt Status         */
  __IM  uint32_t RAW_INT_STATUS;             /*!< \brief Offset: 0x021c (R/ ) Raw Interrupt Status            */
  __OM  uint32_t INTERRUPT_CLEAR;            /*!< \brief Offset: 0x0220 ( /W) Interrupt Clear                 */
        RESERVED(3[0x143], uint32_t)
  __IOM uint32_t CACHE_SYNC;                 /*!< \brief Offset: 0x0730 (R/W) Cache Sync                      */
        RESERVED(4[0xf], uint32_t)
  __IOM uint32_t INV_LINE_PA;                /*!< \brief Offset: 0x0770 (R/W) Invalidate Line By PA           */
        RESERVED(6[2], uint32_t)
  __IOM uint32_t INV_WAY;                    /*!< \brief Offset: 0x077c (R/W) Invalidate by Way               */
        RESERVED(5[0xc], uint32_t)
  __IOM uint32_t CLEAN_LINE_PA;              /*!< \brief Offset: 0x07b0 (R/W) Clean Line by PA                */
        RESERVED(7[1], uint32_t)
  __IOM uint32_t CLEAN_LINE_INDEX_WAY;       /*!< \brief Offset: 0x07b8 (R/W) Clean Line by Index/Way         */
  __IOM uint32_t CLEAN_WAY;                  /*!< \brief Offset: 0x07bc (R/W) Clean by Way                    */
        RESERVED(8[0xc], uint32_t)
  __IOM uint32_t CLEAN_INV_LINE_PA;          /*!< \brief Offset: 0x07f0 (R/W) Clean and Invalidate Line by PA  */
        RESERVED(9[1], uint32_t)
  __IOM uint32_t CLEAN_INV_LINE_INDEX_WAY;   /*!< \brief Offset: 0x07f8 (R/W) Clean and Invalidate Line by Index/Way  */
  __IOM uint32_t CLEAN_INV_WAY;              /*!< \brief Offset: 0x07fc (R/W) Clean and Invalidate by Way     */
        RESERVED(10[0x40], uint32_t)
  __IOM uint32_t DATA_LOCK_0_WAY;            /*!< \brief Offset: 0x0900 (R/W) Data Lockdown 0 by Way          */
  __IOM uint32_t INST_LOCK_0_WAY;            /*!< \brief Offset: 0x0904 (R/W) Instruction Lockdown 0 by Way   */
  __IOM uint32_t DATA_LOCK_1_WAY;            /*!< \brief Offset: 0x0908 (R/W) Data Lockdown 1 by Way          */
  __IOM uint32_t INST_LOCK_1_WAY;            /*!< \brief Offset: 0x090c (R/W) Instruction Lockdown 1 by Way   */
  __IOM uint32_t DATA_LOCK_2_WAY;            /*!< \brief Offset: 0x0910 (R/W) Data Lockdown 2 by Way          */
  __IOM uint32_t INST_LOCK_2_WAY;            /*!< \brief Offset: 0x0914 (R/W) Instruction Lockdown 2 by Way   */
  __IOM uint32_t DATA_LOCK_3_WAY;            /*!< \brief Offset: 0x0918 (R/W) Data Lockdown 3 by Way          */
  __IOM uint32_t INST_LOCK_3_WAY;            /*!< \brief Offset: 0x091c (R/W) Instruction Lockdown 3 by Way   */
  __IOM uint32_t DATA_LOCK_4_WAY;            /*!< \brief Offset: 0x0920 (R/W) Data Lockdown 4 by Way          */
  __IOM uint32_t INST_LOCK_4_WAY;            /*!< \brief Offset: 0x0924 (R/W) Instruction Lockdown 4 by Way   */
  __IOM uint32_t DATA_LOCK_5_WAY;            /*!< \brief Offset: 0x0928 (R/W) Data Lockdown 5 by Way          */
  __IOM uint32_t INST_LOCK_5_WAY;            /*!< \brief Offset: 0x092c (R/W) Instruction Lockdown 5 by Way   */
  __IOM uint32_t DATA_LOCK_6_WAY;            /*!< \brief Offset: 0x0930 (R/W) Data Lockdown 5 by Way          */
  __IOM uint32_t INST_LOCK_6_WAY;            /*!< \brief Offset: 0x0934 (R/W) Instruction Lockdown 5 by Way   */
  __IOM uint32_t DATA_LOCK_7_WAY;            /*!< \brief Offset: 0x0938 (R/W) Data Lockdown 6 by Way          */
  __IOM uint32_t INST_LOCK_7_WAY;            /*!< \brief Offset: 0x093c (R/W) Instruction Lockdown 6 by Way   */
        RESERVED(11[0x4], uint32_t)
  __IOM uint32_t LOCK_LINE_EN;               /*!< \brief Offset: 0x0950 (R/W) Lockdown by Line Enable         */
  __IOM uint32_t UNLOCK_ALL_BY_WAY;          /*!< \brief Offset: 0x0954 (R/W) Unlock All Lines by Way         */
        RESERVED(12[0xaa], uint32_t)
  __IOM uint32_t ADDRESS_FILTER_START;       /*!< \brief Offset: 0x0c00 (R/W) Address Filtering Start         */
  __IOM uint32_t ADDRESS_FILTER_END;         /*!< \brief Offset: 0x0c04 (R/W) Address Filtering End           */
        RESERVED(13[0xce], uint32_t)
  __IOM uint32_t DEBUG_CONTROL;              /*!< \brief Offset: 0x0f40 (R/W) Debug Control Register          */
} L2C_310_TypeDef;

#define L2C_310           ((L2C_310_TypeDef *)L2C_310_BASE) /*!< \brief L2C_310 register set access pointer */
#endif /*  (__L2C_PRESENT == 1U) || defined(DOXYGEN)  */

#if (defined(__TIM_PRESENT) && (__TIM_PRESENT == 1U)) || \
     defined(DOXYGEN)
#if ((__CORTEX_A == 5U) || (__CORTEX_A == 9U)) || defined(DOXYGEN)
/** \brief Structure type to access the Private Timer
*/
typedef struct
{
  __IOM uint32_t LOAD;            //!< \brief  Offset: 0x000 (R/W) Private Timer Load Register
  __IOM uint32_t COUNTER;         //!< \brief  Offset: 0x004 (R/W) Private Timer Counter Register
  __IOM uint32_t CONTROL;         //!< \brief  Offset: 0x008 (R/W) Private Timer Control Register
  __IOM uint32_t ISR;             //!< \brief  Offset: 0x00C (R/W) Private Timer Interrupt Status Register
        RESERVED(0[4], uint32_t)
  __IOM uint32_t WLOAD;           //!< \brief  Offset: 0x020 (R/W) Watchdog Load Register
  __IOM uint32_t WCOUNTER;        //!< \brief  Offset: 0x024 (R/W) Watchdog Counter Register
  __IOM uint32_t WCONTROL;        //!< \brief  Offset: 0x028 (R/W) Watchdog Control Register
  __IOM uint32_t WISR;            //!< \brief  Offset: 0x02C (R/W) Watchdog Interrupt Status Register
  __IOM uint32_t WRESET;          //!< \brief  Offset: 0x030 (R/W) Watchdog Reset Status Register
  __OM  uint32_t WDISABLE;        //!< \brief  Offset: 0x034 ( /W) Watchdog Disable Register
} Timer_Type;
#define PTIM ((Timer_Type *) TIMER_BASE )   /*!< \brief Timer register struct */

/* PTIM Control Register */
#define PTIM_CONTROL_Enable_Pos             0U                                         /*!< PTIM CONTROL: Enable Position */
#define PTIM_CONTROL_Enable_Msk             (0x1U /*<< PTIM_CONTROL_Enable_Pos*/)      /*!< PTIM CONTROL: Enable Mask */
#define PTIM_CONTROL_Enable(x)              (((uint32_t)(((uint32_t)(x)) /*<< PTIM_CONTROL_Enable_Pos*/)) & PTIM_CONTROL_Enable_Msk)

#define PTIM_CONTROL_AutoReload_Pos         1U                                         /*!< PTIM CONTROL: Auto Reload Position */
#define PTIM_CONTROL_AutoReload_Msk         (0x1U << PTIM_CONTROL_AutoReload_Pos)      /*!< PTIM CONTROL: Auto Reload Mask */
#define PTIM_CONTROL_AutoReload(x)          (((uint32_t)(((uint32_t)(x)) << PTIM_CONTROL_AutoReload_Pos)) & PTIM_CONTROL_AutoReload_Msk)

#define PTIM_CONTROL_IRQenable_Pos          2U                                         /*!< PTIM CONTROL: IRQ Enabel Position */
#define PTIM_CONTROL_IRQenable_Msk          (0x1U << PTIM_CONTROL_IRQenable_Pos)       /*!< PTIM CONTROL: IRQ Enabel Mask */
#define PTIM_CONTROL_IRQenable(x)           (((uint32_t)(((uint32_t)(x)) << PTIM_CONTROL_IRQenable_Pos)) & PTIM_CONTROL_IRQenable_Msk)

#define PTIM_CONTROL_Prescaler_Pos          8U                                         /*!< PTIM CONTROL: Prescaler Position */
#define PTIM_CONTROL_Prescaler_Msk          (0xFFU << PTIM_CONTROL_Prescaler_Pos)      /*!< PTIM CONTROL: Prescaler Mask */
#define PTIM_CONTROL_Prescaler(x)           (((uint32_t)(((uint32_t)(x)) << PTIM_CONTROL_Prescaler_Pos)) & PTIM_CONTROL_Prescaler_Msk)

/* WCONTROL Watchdog Control Register */
#define PTIM_WCONTROL_Enable_Pos            0U                                         /*!< PTIM WCONTROL: Enable Position */
#define PTIM_WCONTROL_Enable_Msk            (0x1U /*<< PTIM_WCONTROL_Enable_Pos*/)     /*!< PTIM WCONTROL: Enable Mask */
#define PTIM_WCONTROL_Enable(x)             (((uint32_t)(((uint32_t)(x)) /*<< PTIM_WCONTROL_Enable_Pos*/)) & PTIM_WCONTROL_Enable_Msk)

#define PTIM_WCONTROL_AutoReload_Pos        1U                                         /*!< PTIM WCONTROL: Auto Reload Position */
#define PTIM_WCONTROL_AutoReload_Msk        (0x1U << PTIM_WCONTROL_AutoReload_Pos)     /*!< PTIM WCONTROL: Auto Reload Mask */
#define PTIM_WCONTROL_AutoReload(x)         (((uint32_t)(((uint32_t)(x)) << PTIM_WCONTROL_AutoReload_Pos)) & PTIM_WCONTROL_AutoReload_Msk)

#define PTIM_WCONTROL_IRQenable_Pos         2U                                         /*!< PTIM WCONTROL: IRQ Enable Position */
#define PTIM_WCONTROL_IRQenable_Msk         (0x1U << PTIM_WCONTROL_IRQenable_Pos)      /*!< PTIM WCONTROL: IRQ Enable Mask */
#define PTIM_WCONTROL_IRQenable(x)          (((uint32_t)(((uint32_t)(x)) << PTIM_WCONTROL_IRQenable_Pos)) & PTIM_WCONTROL_IRQenable_Msk)

#define PTIM_WCONTROL_Mode_Pos              3U                                         /*!< PTIM WCONTROL: Watchdog Mode Position */
#define PTIM_WCONTROL_Mode_Msk              (0x1U << PTIM_WCONTROL_Mode_Pos)           /*!< PTIM WCONTROL: Watchdog Mode Mask */
#define PTIM_WCONTROL_Mode(x)               (((uint32_t)(((uint32_t)(x)) << PTIM_WCONTROL_Mode_Pos)) & PTIM_WCONTROL_Mode_Msk)

#define PTIM_WCONTROL_Presacler_Pos         8U                                         /*!< PTIM WCONTROL: Prescaler Position */
#define PTIM_WCONTROL_Presacler_Msk         (0xFFU << PTIM_WCONTROL_Presacler_Pos)     /*!< PTIM WCONTROL: Prescaler Mask */
#define PTIM_WCONTROL_Presacler(x)          (((uint32_t)(((uint32_t)(x)) << PTIM_WCONTROL_Presacler_Pos)) & PTIM_WCONTROL_Presacler_Msk)

/* WISR Watchdog Interrupt Status Register */
#define PTIM_WISR_EventFlag_Pos             0U                                         /*!< PTIM WISR: Event Flag Position */
#define PTIM_WISR_EventFlag_Msk             (0x1U /*<< PTIM_WISR_EventFlag_Pos*/)      /*!< PTIM WISR: Event Flag Mask */
#define PTIM_WISR_EventFlag(x)              (((uint32_t)(((uint32_t)(x)) /*<< PTIM_WISR_EventFlag_Pos*/)) & PTIM_WISR_EventFlag_Msk)

/* WRESET Watchdog Reset Status */
#define PTIM_WRESET_ResetFlag_Pos           0U                                         /*!< PTIM WRESET: Reset Flag Position */
#define PTIM_WRESET_ResetFlag_Msk           (0x1U /*<< PTIM_WRESET_ResetFlag_Pos*/)    /*!< PTIM WRESET: Reset Flag Mask */
#define PTIM_WRESET_ResetFlag(x)            (((uint32_t)(((uint32_t)(x)) /*<< PTIM_WRESET_ResetFlag_Pos*/)) & PTIM_WRESET_ResetFlag_Msk)

#endif /* ((__CORTEX_A == 5U) || (__CORTEX_A == 9U)) || defined(DOXYGEN) */
#endif /* (__TIM_PRESENT == 1U) || defined(DOXYGEN) */

 /*******************************************************************************
  *                Hardware Abstraction Layer
   Core Function Interface contains:
   - L1 Cache Functions
   - L2C-310 Cache Controller Functions
   - PL1 Timer Functions
   - GIC Functions
   - MMU Functions
  ******************************************************************************/

/** \brief  Set DCISW
 */
__STATIC_FORCEINLINE void __set_DCISW(uint64_t value)
{
	__ASM volatile("DC ISW, %0" : : "r" (value) : "memory");
}

/** \brief Clean by Set/Way DCCSW
 */
__STATIC_FORCEINLINE void __set_DCCSW(uint64_t value)
{
	__ASM volatile("DC CSW, %0" : : "r" (value) : "memory");
}

/** \brief CISW Clean and invalidate by Set/Way DCCISW
 */
__STATIC_FORCEINLINE void __set_DCCISW(uint64_t value)
{
	__ASM volatile("DC CISW, %0" : : "r" (value) : "memory");
}

/** \brief CIVAC Clean and Invalidate by Virtual Address to Point of Coherency DCCIMVAC
 */
__STATIC_FORCEINLINE void __set_DCCIVAC(uint64_t value)
{
	__ASM volatile("DC CIVAC, %0" : : "r" (value) : "memory");
}

/** \brief CVAC Clean by Virtual Address to Point of Coherency DCCMVAC
 */
__STATIC_FORCEINLINE void __set_DCCVAC(uint64_t value)
{
	__ASM volatile("DC CVAC, %0" : : "r" (value) : "memory");
}

/** \brief IVAC Invalidate by Virtual Address, to Point of Coherency DCIMVAC
 */
__STATIC_FORCEINLINE void __set_DCIVAC(uint64_t value)
{
	__ASM volatile("DC IVAC, %0" : : "r" (value) : "memory");
}

/** \brief  ZVA Cache zero by Virtual Address
 */
__STATIC_FORCEINLINE void __set_DCZVA(uint64_t value)
{
	__ASM volatile("DC ZVA, %0" : : "r" (value) : "memory");
}

__STATIC_FORCEINLINE void __set_SCTLR_EL1(uint64_t value)
{
	__set_RG64("SCTLR_EL1", value);
}

__STATIC_FORCEINLINE uint64_t __get_SCTLR_EL1(void)
{
	uint64_t result;
	// MRS <Xt>, SCTLR_EL1 ; Read SCTLR_EL1 into Xt
	__get_RG64("SCTLR_EL1", result);
	return result;
}

__STATIC_FORCEINLINE void __set_SCTLR_EL2(uint64_t value)
{
	__set_RG64("SCTLR_EL2", value);
}

__STATIC_FORCEINLINE uint64_t __get_SCTLR_EL2(void)
{
	uint64_t result;
	// MRS <Xt>, SCTLR_EL2 ; Read SCTLR_EL2 into Xt
	__get_RG64("SCTLR_EL2", result);
	return result;
}

__STATIC_FORCEINLINE uint64_t __get_ID_AA64MMFR2_EL1(void)
{
	uint64_t result;
	// MRS <Xt>, ID_AA64MMFR2_EL1 ; Read ID_AA64MMFR2_EL1 into Xt
	__get_RG64("ID_AA64MMFR2_EL1", result);
	return result;
}

#if 0
__STATIC_FORCEINLINE void __set_SCTLR_EL3(uint64_t value)
{
	__set_RG64("SCTLR_EL3", value);
}

__STATIC_FORCEINLINE uint64_t __get_SCTLR_EL3(void)
{
	uint64_t result;
	// MRS <Xt>, SCTLR_EL3 ; Read SCTLR_EL3 into Xt
	__get_RG64("SCTLR_EL3", result);
	return result;
}
#endif

__STATIC_FORCEINLINE void __set_HCR_EL2(uint64_t value)
{
	__set_RG64("HCR_EL2", value);
}

__STATIC_FORCEINLINE uint64_t __get_HCR_EL2(void)
{
	uint64_t result;
	// MRS <Xt>, HCR_EL2 ; Read HCR_EL2 into Xt
	__get_RG64("HCR_EL2", result);
	return result;
}

__STATIC_FORCEINLINE void __set_ACTLR_EL3(uint32_t value)
{
	__set_RG32("ACTLR_EL3", value);
}

__STATIC_FORCEINLINE uint32_t __get_ACTLR_EL3(void)
{
	uint32_t result;
	// MRS <Xt>, ACTLR_EL3 ; Read ACTLR_EL3 into Xt
	__get_RG32("ACTLR_EL3", result);
	return result;
}

// PMCCNTR_EL0, Performance Monitors Cycle Count Register
__STATIC_FORCEINLINE void __set_PMCCNTR_EL0(uint64_t value)
{
	__set_RG64("PMCCNTR_EL0", value);
}

// PMCCNTR_EL0, Performance Monitors Cycle Count Register
__STATIC_FORCEINLINE uint64_t __get_PMCCNTR_EL0(void)
{
	uint64_t result;
	// MRS <Xt>, PMCCNTR_EL0 ; Read PMCCNTR_EL0 into Xt
	__get_RG64("PMCCNTR_EL0", result);
	return result;
}

//  PMCR_EL0, Performance Monitors Control Register
__STATIC_FORCEINLINE void __set_PMCR_EL0(uint64_t value)
{
	__set_RG64("PMCR_EL0", value);
}

__STATIC_FORCEINLINE uint64_t __get_PMCR_EL0(void)
{
	uint64_t result;
	// MRS <Xt>, PMCR_EL0 ; Read PMCR_EL0 into Xt
	__get_RG64("PMCR_EL0", result);
	return result;
}

// PMCNTENSET_EL0, Performance Monitors Count Enable Set register
__STATIC_FORCEINLINE void __set_PMCNTENSET_EL0(uint64_t value)
{
	__set_RG64("PMCNTENSET_EL0", value);
}

__STATIC_FORCEINLINE uint64_t __get_PMCNTENSET_EL0(void)
{
	uint64_t result;
	// MRS <Xt>, PMCNTENSET_EL0 ; Read PMCNTENSET_EL0 into Xt
	__get_RG64("PMCNTENSET_EL0", result);
	return result;
}

__STATIC_INLINE void __set_CPACR_EL1(uint32_t value)
{
	__set_RG32("CPACR_EL1", value);
}

__STATIC_INLINE uint32_t __get_CPACR_EL1(void)
{
	uint32_t result;
	// MRS <Xt>, CPACR_EL1 ; Read CPACR_EL1 into Xt
	__get_RG32("CPACR_EL1", result);
	return result;
}

// D24.2.44 DCZID_EL0, Data Cache Zero ID Register
__STATIC_INLINE uint64_t __get_DCZID_EL0(void)
{
	uint64_t result;
	// MRS <Xt>, DCZID_EL0 ; Read DCZID_EL0 into Xt
	__get_RG64("DCZID_EL0", result);
	return result;
}

// Rise trap
//__STATIC_INLINE void __set_DCZID_EL0(uint64_t value)
//{
//	// MRS <Xt>, DCZID_EL0 ; Read DCZID_EL0 into Xt
//	__set_RG64("DCZID_EL0", value);
//}

/* CurrentEL, Current Exception Level
	EL, bits [3:2]
		Current exception level. Possible values of this field are:
		00 EL0
		01 EL1
		10 EL2
		11 EL3
*/

__STATIC_INLINE uint64_t __get_CurrentEL(void)
{
	uint64_t result;
    __get_RG32("CurrentEL", result);
    return result;
}

// AArch32 (CLUSTERCFR) and AArch64 (CLUSTERCFR_EL1)
// MRS <Xt>, S3_0_C15_C3_0; Read CLUSTERCFR_EL1 into Xt
// MRC p15, 0, <Rt>, c15, c3, 0; Read CLUSTERCFR into Rt
__STATIC_FORCEINLINE uint32_t __get_CLUSTERCFR_EL1(void)
{
	uint32_t result;
	// MRS <Xt>, MIDR_EL1 ; Read MIDR_EL1 into Xt
	__get_RG32("S3_0_C15_C3_0", result);
	return result;
}

// AArch32 (CLUSTERECTLR) and AArch64 (CLUSTERECTLR_EL1)
// MRS <Xt>, S3_0_C15_C3_4; Read CLUSTERECTLR_EL1 into Xt
// MSR S3_0_C15_C3_4, <Xt>; Write Xt into CLUSTERECTLR_EL1
// MRC p15, 0, <Rt>, c15, c3, 4; Read CLUSTERECTLR into Rt
// MCR p15, 0, <Rt>, c15, c3, 4; Write Rt into CLUSTERECTLR
__STATIC_FORCEINLINE uint32_t __get_CLUSTERECTLR_EL1(void)
{
	uint64_t result;
	__get_RG32("S3_0_C15_C3_4", result);
	return result;
}
__STATIC_FORCEINLINE void __set_CLUSTERECTLR_EL1(uint32_t v)
{
	uint64_t value = v;
	__set_RG32("S3_0_C15_C3_4", value);
}
// MRS <Xt>, S3_0_C15_C3_5; Read CLUSTERPWRCTLR_EL1 into Xt
__STATIC_FORCEINLINE uint32_t __get_CLUSTERPWRCTLR_EL1(void)
{
	uint64_t result;
	__get_RG32("S3_0_C15_C3_5", result);
	return result;
}
__STATIC_FORCEINLINE void __set_CLUSTERPWRCTLR_EL1(uint32_t v)
{
	uint64_t value = v;
	__set_RG32("S3_0_C15_C3_5", value);
}
// MSR S3_0_C15_C3_6, <Xt>; Write Xt into CLUSTERPWRDN_EL1
__STATIC_FORCEINLINE void __set_CLUSTERPWRDN_EL1(uint32_t v)
{
	uint64_t value = v;
	__set_RG32("S3_0_C15_C3_6", value);
}
// MRS <Xt>, S3_0_C15_C3_6; Read CLUSTERPWRDN_EL1 into Xt
__STATIC_FORCEINLINE uint32_t __get_CLUSTERPWRDN_EL1(void)
{
	uint64_t result;
	__get_RG32("S3_0_C15_C3_6", result);
	return result;
}

// Counter-timer Frequency Register
// high 32 bit are zeroes (reserved).
__STATIC_FORCEINLINE void __set_CNTFRQ_EL0(uint32_t v)
{
	uint64_t value = v;
	__set_RG32("CNTFRQ_EL0", value);
}
__STATIC_FORCEINLINE uint32_t __get_CNTFRQ_EL0(void)
{
	uint32_t result;
	__get_RG32("CNTFRQ_EL0", result);
	return result;
}

// Counter-timer Physical Timer Control Register
__STATIC_FORCEINLINE void __set_CNTP_CTL_EL0(uint64_t value)
{
	__set_RG64("CNTP_CTL_EL0", value);
}
__STATIC_FORCEINLINE uint64_t __get_CNTP_CTL_EL0(void)
{
	uint64_t result;
	__get_RG32("CNTP_CTL_EL0", result);
	return result;
}

// Counter-timer Physical Timer CompareValue Register
__STATIC_FORCEINLINE void __set_CNTP_CVAL_EL0(uint64_t value)
{
	__set_RG64("CNTP_CVAL_EL0", value);
}
__STATIC_FORCEINLINE uint64_t __get_CNTP_CVAL_EL0(void)
{
	uint64_t result;
	__get_RG64("CNTP_CVAL_EL0", result);
	return result;
}

// Counter-timer Physical Timer TimerValue Register
__STATIC_FORCEINLINE void __set_CNTP_TVAL_EL0(uint64_t value)
{
	__set_RG64("CNTP_TVAL_EL0", value);
}
__STATIC_FORCEINLINE uint64_t __get_CNTP_TVAL_EL0(void)
{
	uint64_t result;
	__get_RG64("CNTP_TVAL_EL0", result);
	return result;
}

// Counter-timer Physical Count Register
// Reads of CNTPCT_EL0 return the 64-bit physical count value minus a physical offset.

__STATIC_FORCEINLINE uint64_t __get_CNTPCT_EL0(void)
{
	uint64_t result;
	__get_RG64("CNTPCT_EL0", result);
	return result;
}

// Counter-timer Self-Synchronized Physical Count Register
__STATIC_FORCEINLINE uint64_t __get_CNTPCTSS_EL0(void)
{
	uint64_t result;
	__get_RG64("CNTPCTSS_EL0", result);
	return result;
}

// Counter-timer Physical Offset Register
__STATIC_FORCEINLINE void __set_CNTPOFF_EL2(uint64_t value)
{
	__set_RG64("CNTPOFF_EL2", value);
}
__STATIC_FORCEINLINE uint64_t __get_CNTPOFF_EL2(void)
{
	uint64_t result;
	__get_RG64("CNTPOFF_EL2", result);
	return result;
}


/* ##########################  L2 Cache functions  ################################# */
#if (defined(__L2C_PRESENT) && (__L2C_PRESENT == 1U)) || \
     defined(DOXYGEN)
/** \brief Cache Sync operation by writing CACHE_SYNC register.
*/
__STATIC_INLINE void L2C_Sync(void)
{
  L2C_310->CACHE_SYNC = 0x0;
}

/** \brief Read cache controller cache ID from CACHE_ID register.
 * \return L2C_310_TypeDef::CACHE_ID
 */
__STATIC_INLINE int L2C_GetID (void)
{
  return L2C_310->CACHE_ID;
}

/** \brief Read cache controller cache type from CACHE_TYPE register.
*  \return L2C_310_TypeDef::CACHE_TYPE
*/
__STATIC_INLINE int L2C_GetType (void)
{
  return L2C_310->CACHE_TYPE;
}

/** \brief Invalidate all cache by way
*/
__STATIC_INLINE void L2C_InvAllByWay (void)
{
  unsigned int assoc;

  if (L2C_310->AUX_CNT & (1U << 16U)) {
    assoc = 16U;
  } else {
    assoc =  8U;
  }

  L2C_310->INV_WAY = (1U << assoc) - 1U;
  while(L2C_310->INV_WAY & ((1U << assoc) - 1U)); //poll invalidate

  L2C_Sync();
}

/** \brief Clean and Invalidate all cache by way
*/
__STATIC_INLINE void L2C_CleanInvAllByWay (void)
{
  unsigned int assoc;

  if (L2C_310->AUX_CNT & (1U << 16U)) {
    assoc = 16U;
  } else {
    assoc =  8U;
  }

  L2C_310->CLEAN_INV_WAY = (1U << assoc) - 1U;
  while(L2C_310->CLEAN_INV_WAY & ((1U << assoc) - 1U)); //poll invalidate

  L2C_Sync();
}

/** \brief Enable Level 2 Cache
*/
__STATIC_INLINE void L2C_Enable(void)
{
  L2C_310->CONTROL = 0;
  L2C_310->INTERRUPT_CLEAR = 0x000001FFuL;
  L2C_310->DEBUG_CONTROL = 0;
  L2C_310->DATA_LOCK_0_WAY = 0;
  L2C_310->CACHE_SYNC = 0;
  L2C_310->CONTROL = 0x01;
  L2C_Sync();
}

/** \brief Disable Level 2 Cache
*/
__STATIC_INLINE void L2C_Disable(void)
{
  L2C_310->CONTROL = 0x00;
  L2C_Sync();
}

/** \brief Invalidate cache by physical address
* \param [in] pa Pointer to data to invalidate cache for.
*/
__STATIC_INLINE void L2C_InvPa (void *pa)
{
  L2C_310->INV_LINE_PA = (unsigned int)pa;
  L2C_Sync();
}

/** \brief Clean cache by physical address
* \param [in] pa Pointer to data to invalidate cache for.
*/
__STATIC_INLINE void L2C_CleanPa (void *pa)
{
  L2C_310->CLEAN_LINE_PA = (unsigned int)pa;
  L2C_Sync();
}

/** \brief Clean and invalidate cache by physical address
* \param [in] pa Pointer to data to invalidate cache for.
*/
__STATIC_INLINE void L2C_CleanInvPa (void *pa)
{
  L2C_310->CLEAN_INV_LINE_PA = (unsigned int)pa;
  L2C_Sync();
}
#endif

/* ##########################  Generic Timer functions  ############################ */
#if (defined(__TIM_PRESENT) && (__TIM_PRESENT == 1U)) || \
    defined(DOXYGEN)

/* PL1 Physical Timer */
#if (__CORTEX_A == 7U) || defined(DOXYGEN)

/** \brief Physical Timer Control register */
typedef union
{
  struct
  {
    uint32_t ENABLE:1;      /*!< \brief bit: 0      Enables the timer. */
    uint32_t IMASK:1;       /*!< \brief bit: 1      Timer output signal mask bit. */
    uint32_t ISTATUS:1;     /*!< \brief bit: 2      The status of the timer. */
    RESERVED(0:29, uint32_t)
  } b;                      /*!< \brief Structure used for bit  access */
  uint32_t w;               /*!< \brief Type      used for word access */
} CNTP_CTL_Type;

/** \brief Configures the frequency the timer shall run at.
* \param [in] value The timer frequency in Hz.
*/
__STATIC_INLINE void PL1_SetCounterFrequency(uint32_t value)
{
  __set_CNTFRQ(value);
  __ISB();
}

/** \brief Sets the reset value of the timer.
* \param [in] value The value the timer is loaded with.
*/
__STATIC_INLINE void PL1_SetLoadValue(uint32_t value)
{
  __set_CNTP_TVAL(value);
  __ISB();
}

/** \brief Get the current counter value.
* \return Current counter value.
*/
__STATIC_INLINE uint32_t PL1_GetCurrentValue(void)
{
  return(__get_CNTP_TVAL());
}

/** \brief Get the current physical counter value.
* \return Current physical counter value.
*/
__STATIC_INLINE uint64_t PL1_GetCurrentPhysicalValue(void)
{
  return(__get_CNTPCT());
}

/** \brief Set the physical compare value.
* \param [in] value New physical timer compare value.
*/
__STATIC_INLINE void PL1_SetPhysicalCompareValue(uint64_t value)
{
  __set_CNTP_CVAL(value);
  __ISB();
}

/** \brief Get the physical compare value.
* \return Physical compare value.
*/
__STATIC_INLINE uint64_t PL1_GetPhysicalCompareValue(void)
{
  return(__get_CNTP_CVAL());
}

/** \brief Configure the timer by setting the control value.
* \param [in] value New timer control value.
*/
__STATIC_INLINE void PL1_SetControl(uint32_t value)
{
  __set_CNTP_CTL(value);
  __ISB();
}

/** \brief Get the control value.
* \return Control value.
*/
__STATIC_INLINE uint32_t PL1_GetControl(void)
{
  return(__get_CNTP_CTL());
}

/******************************* VIRTUAL TIMER *******************************/
/** \brief Virtual Timer Control register */

/** \brief Sets the reset value of the virtual timer.
* \param [in] value The value the virtual timer is loaded with.
*/
__STATIC_INLINE void VL1_SetCurrentTimerValue(uint32_t value)
{
  __set_CNTV_TVAL(value);
  __ISB();
}

/** \brief Get the current virtual timer value.
* \return Current virtual timer value.
*/
__STATIC_INLINE uint32_t VL1_GetCurrentTimerValue(void)
{
  return(__get_CNTV_TVAL());
}

/** \brief Get the current virtual count value.
* \return Current virtual count value.
*/
__STATIC_INLINE uint64_t VL1_GetCurrentCountValue(void)
{
  return(__get_CNTVCT());
}

/** \brief Set the virtual timer compare value.
* \param [in] value New virtual timer compare value.
*/
__STATIC_INLINE void VL1_SetTimerCompareValue(uint64_t value)
{
  __set_CNTV_CVAL(value);
  __ISB();
}

/** \brief Get the virtual timer compare value.
* \return Virtual timer compare value.
*/
__STATIC_INLINE uint64_t VL1_GetTimerCompareValue(void)
{
  return(__get_CNTV_CVAL());
}

/** \brief Configure the virtual timer by setting the control value.
* \param [in] value New virtual timer control value.
*/
__STATIC_INLINE void VL1_SetControl(uint32_t value)
{
  __set_CNTV_CTL(value);
  __ISB();
}

/** \brief Get the virtual timer control value.
* \return Virtual timer control value.
*/
__STATIC_INLINE uint32_t VL1_GetControl(void)
{
  return(__get_CNTV_CTL());
}
/***************************** VIRTUAL TIMER END *****************************/
#endif

/* Private Timer */
#if ((__CORTEX_A == 5U) || (__CORTEX_A == 9U)) || defined(DOXYGEN)
/** \brief Set the load value to timers LOAD register.
* \param [in] value The load value to be set.
*/
__STATIC_INLINE void PTIM_SetLoadValue(uint32_t value)
{
  PTIM->LOAD = value;
}

/** \brief Get the load value from timers LOAD register.
* \return Timer_Type::LOAD
*/
__STATIC_INLINE uint32_t PTIM_GetLoadValue(void)
{
  return(PTIM->LOAD);
}

/** \brief Set current counter value from its COUNTER register.
*/
__STATIC_INLINE void PTIM_SetCurrentValue(uint32_t value)
{
  PTIM->COUNTER = value;
}

/** \brief Get current counter value from timers COUNTER register.
* \result Timer_Type::COUNTER
*/
__STATIC_INLINE uint32_t PTIM_GetCurrentValue(void)
{
  return(PTIM->COUNTER);
}

/** \brief Configure the timer using its CONTROL register.
* \param [in] value The new configuration value to be set.
*/
__STATIC_INLINE void PTIM_SetControl(uint32_t value)
{
  PTIM->CONTROL = value;
}

/** ref Timer_Type::CONTROL Get the current timer configuration from its CONTROL register.
* \return Timer_Type::CONTROL
*/
__STATIC_INLINE uint32_t PTIM_GetControl(void)
{
  return(PTIM->CONTROL);
}

/** ref Timer_Type::CONTROL Get the event flag in timers ISR register.
* \return 0 - flag is not set, 1- flag is set
*/
__STATIC_INLINE uint32_t PTIM_GetEventFlag(void)
{
  return (PTIM->ISR & 1UL);
}

/** ref Timer_Type::CONTROL Clears the event flag in timers ISR register.
*/
__STATIC_INLINE void PTIM_ClearEventFlag(void)
{
  PTIM->ISR = 1;
}
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* INC_CORE64_CA_H_ */
