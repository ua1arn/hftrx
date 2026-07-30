/*
 * core32_ca.h
 *
 *  Created on: Jun 1, 2026
 *      Author: Gena
 */

#ifndef INC_CORE32_CA_H_
#define INC_CORE32_CA_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if ! defined(__aarch64__) && defined(__CORTEX_A)

/** \brief  Get CTR
\return		Cache Type Register value
*/
__STATIC_INLINE uint32_t __get_CTR(void)
{
	uint32_t result;
	__get_CP(15, 0, result, 0, 0, 1);
	return result;
}
#endif

#if (__ARM_ARCH == 8) && ! defined(__aarch64__)

// DDI0500J_cortex_a53_r0p4_trm.pdf

// 4.5.76 CPU Auxiliary Control Register

/** \brief  Get CPUACTLR
    \return                CPU Auxiliary Control Register
 */
__STATIC_FORCEINLINE uint64_t __get_CPUACTLR(void)
{
	uint64_t result;
  __get_CP64(15, 0, result, 15);
  return(result);
}

/** \brief  Set CPUACTLR
    \param [in]    cpuactlr   CPU Auxiliary Control Register
 */
__STATIC_FORCEINLINE void __set_CPUACTLR(uint64_t cpuactlr)
{
	__set_CP64(15, 0, cpuactlr, 15);
}

// 4.5.77 CPU Extended Control Register

/** \brief  Get CPUECTLR
    \return               CPU Extended Control Register
 */
__STATIC_FORCEINLINE uint64_t __get_CPUECTLR(void)
{
	uint64_t result;
	// cp, op1, Rt, CRm
	__get_CP64(15, 1, result, 15);
	return (result);
}

/** \brief  Set CPUECTLR
    \param [in]    cpuectlr  CPU Extended Control Register
 */
__STATIC_FORCEINLINE void __set_CPUECTLR(uint64_t cpuectlr)
{
	__set_CP64(15, 1, cpuectlr, 15);
}

// ICC_CTLR_EL3 and ICC_CTLR
__STATIC_INLINE uint64_t __get_ICC_CTLR_EL3(void)
{
	uint64_t result;
    //__MRC32(sICC_CTLR_EL3, &result);		// mrc	15, 6, r0, cr12, cr12, {4}
	//__get_CP(15, 6, result, 12, 12, 4);	// mrc	15, 6, r0, cr12, cr12, {4}
	__get_CP(15, 6, result, 12, 12, 4);	// mrc	15, 0, r0, cr12, cr12, {4} - недоступно в aarch64
    return result;
}
__STATIC_INLINE void __set_ICC_CTLR_EL3(uint64_t value)
{
    //__MCR32(sICC_CTLR_EL3, value);
	__set_CP(15, 6, value, 12, 12, 4);	// mcr	15, 6, r0, cr12, cr12, {4} - недоступно в aarch64
	//__set_RG32("ICC_CTLR", result);
}

// ICC_CTLR_EL1, Interrupt Controller Control Register (EL1)
__STATIC_INLINE uint32_t __get_ICC_CTLR_EL1(void)
{
    uint32_t result;
    //__MRC32(sICC_CTLR_EL1, &result);		// mrc	15, 6, r0, cr12, cr12, {4}
	//__get_CP(15, 0, result, 12, 12, 4);	// mrc	15, 6, r0, cr12, cr12, {4}
	__get_CP(15, 0, result, 12, 12, 4);	// mrc	15, 0, r0, cr12, cr12, {4}
    return result;
}

// ICC_CTLR_EL1, Interrupt Controller Control Register (EL1)
__STATIC_INLINE void __set_ICC_CTLR_EL1(uint32_t value)
{
    //__MCR32(sICC_CTLR_EL1, value);
	__set_CP(15, 0, value, 12, 12, 4);	// mcr	15, 0, r0, cr12, cr12, {4}
	//__set_RG32("ICC_CTLR", result);
}


//__STATIC_INLINE uint32_t __get_CurrentEL(void)
//{
//	uint32_t result;
//	// cp, op1, Rt, CRn, CRm, op2
//	//__get_CP(15, 0, result, 4, 2, 2);	// mrc	15, 0, r1, cr4, cr2, {2}
//	  __ASM volatile ("MRS %0, CurrentEL" : "=r" (result) );
//	return(result);
//}


#endif /* (__ARM_ARCH == 8) && ! defined(__aarch64__) */

#if (__CORTEX_A == 55U) && ! __aarch64__
// AArch32 (CLUSTERCFR) and AArch64 (CLUSTERCFR_EL1)
// MRS <Xt>, S3_0_C15_C3_0; Read CLUSTERCFR_EL1 into Xt
// MRC p15, 0, <Rt>, c15, c3, 0; Read CLUSTERCFR into Rt
__STATIC_FORCEINLINE uint32_t __get_CLUSTERCFR(void)
{
	uint32_t result;
	__get_CP(15, 0, result, 15, 3, 0);
	return result;
}

// AArch32 (CLUSTERECTLR) and AArch64 (CLUSTERECTLR_EL1)
// MRS <Xt>, S3_0_C15_C3_4; Read CLUSTERECTLR_EL1 into Xt
// MSR S3_0_C15_C3_4, <Xt>; Write Xt into CLUSTERECTLR_EL1
// MRC p15, 0, <Rt>, c15, c3, 4; Read CLUSTERECTLR into Rt
// MCR p15, 0, <Rt>, c15, c3, 4; Write Rt into CLUSTERECTLR
__STATIC_FORCEINLINE uint32_t __get_CLUSTERECTLR(void)
{
	uint32_t result;
	__get_CP(15, 0, result, 15, 3, 4);
	return result;
}

// MRC p15, 0, <Rt>, c15, c3, 5; Read CLUSTERPWRCTLR into Rt
__STATIC_FORCEINLINE uint32_t __get_CLUSTERPWRCTLR(void)
{
	uint32_t result;
	__get_CP(15, 0, result, 15, 3, 5);
	return result;
}
// MCR p15, 0, <Rt>, c15, c3, 5; Write Rt into CLUSTERPWRCTLR
__STATIC_FORCEINLINE void __set_CLUSTERPWRCTLR(uint32_t value)
{
	__set_CP(15, 0, value, 15, 3, 5);
}

//MRC p15, 0, <Rt>, c15, c3, 6; Read CLUSTERPWRDN into Rt
__STATIC_FORCEINLINE uint32_t __get_CLUSTERPWRDN(void)
{
	uint32_t result;
	__get_CP(15, 0, result, 15, 3, 6);
	return result;
}

#endif /* (__CORTEX_A == 55U)  */


/* Register MPIDR_EL1 */
typedef union
{
  struct
  {
    uint64_t Aff0:8;
    uint64_t Aff1:8;
    uint64_t Aff2:8;
    uint64_t MT:1;
    RESERVED(0:5, uint64_t)
    uint64_t U:1;
    RESERVED(1:1, uint64_t)
    uint64_t Aff3:8;
    RESERVED(2:24, uint64_t)
  } b;                                   /*!< \brief Structure used for bit  access */
  uint64_t w;                            /*!< \brief Type      used for word access */
} MPIDR_EL1_Type;

#define MPIDR_AFFLVL_MASK (0xffULL)
#define MPIDR_AFF0_SHIFT     (0ULL)
#define MPIDR_AFF1_SHIFT     (8ULL)
#define MPIDR_AFF2_SHIFT    (16ULL)
#define MPIDR_AFF3_SHIFT    (32ULL)
#define MPIDR_MT_MASK      (0x1ULL)
#define MPIDR_MT_SHIFT	    (24ULL)

#define MPIDR_SUPPORT_MT(mpidr) ((mpidr >> MPIDR_MT_SHIFT) & MPIDR_MT_MASK)


#define MPIDR_TO_AFF_LEVEL(mpidr, aff_level) (((mpidr) >> MPIDR_AFF##aff_level##_SHIFT) & MPIDR_AFFLVL_MASK)

#define MPIDR_AFFINITY_MASK                   \
  ((MPIDR_AFFLVL_MASK << MPIDR_AFF3_SHIFT)  | \
   (MPIDR_AFFLVL_MASK << MPIDR_AFF2_SHIFT)  | \
   (MPIDR_AFFLVL_MASK << MPIDR_AFF1_SHIFT)  | \
   (MPIDR_AFFLVL_MASK << MPIDR_AFF0_SHIFT))

#ifdef __cplusplus
 }
#endif /* __cplusplus */


#endif /* INC_CORE32_CA_H_ */
