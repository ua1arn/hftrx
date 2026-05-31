/*
 * ca_mmu_64.h
 *
 *  Created on: May 31, 2026
 *      Author: Gena
 */

#ifndef INC_CA_MMU_64_H_
#define INC_CA_MMU_64_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* ##########################  MMU functions  ###################################### */
#if 0

#define SECTION_DESCRIPTOR      (0x2)
#define SECTION_MASK            (0xFFFFFFFC)

#define SECTION_TEXCB_MASK      (0xFFFF8FF3)
#define SECTION_B_SHIFT         (2)
#define SECTION_C_SHIFT         (3)
#define SECTION_TEX0_SHIFT      (12)
#define SECTION_TEX1_SHIFT      (13)
#define SECTION_TEX2_SHIFT      (14)

#define SECTION_XN_MASK         (0xFFFFFFEF)
#define SECTION_XN_SHIFT        (4)

#define SECTION_DOMAIN_MASK     (0xFFFFFE1F)
#define SECTION_DOMAIN_SHIFT    (5)

#define SECTION_P_MASK          (0xFFFFFDFF)
#define SECTION_P_SHIFT         (9)

#define SECTION_AP_MASK         (0xFFFF73FF)
#define SECTION_AP_SHIFT        (10)
#define SECTION_AP2_SHIFT       (15)

#define SECTION_S_MASK          (0xFFFEFFFF)
#define SECTION_S_SHIFT         (16)

#define SECTION_NG_MASK         (0xFFFDFFFF)
#define SECTION_NG_SHIFT        (17)

#define SECTION_NS_MASK         (0xFFF7FFFF)
#define SECTION_NS_SHIFT        (19)

#define PAGE_L1_DESCRIPTOR      (0x1)
#define PAGE_L1_MASK            (0xFFFFFFFC)

#define PAGE_L2_4K_DESC         (0x2)
#define PAGE_L2_4K_MASK         (0xFFFFFFFD)

#define PAGE_L2_64K_DESC        (0x1)
#define PAGE_L2_64K_MASK        (0xFFFFFFFC)

#define PAGE_4K_TEXCB_MASK      (0xFFFFFE33)
#define PAGE_4K_B_SHIFT         (2)
#define PAGE_4K_C_SHIFT         (3)
#define PAGE_4K_TEX0_SHIFT      (6)
#define PAGE_4K_TEX1_SHIFT      (7)
#define PAGE_4K_TEX2_SHIFT      (8)

#define PAGE_64K_TEXCB_MASK     (0xFFFF8FF3)
#define PAGE_64K_B_SHIFT        (2)
#define PAGE_64K_C_SHIFT        (3)
#define PAGE_64K_TEX0_SHIFT     (12)
#define PAGE_64K_TEX1_SHIFT     (13)
#define PAGE_64K_TEX2_SHIFT     (14)

#define PAGE_TEXCB_MASK         (0xFFFF8FF3)
#define PAGE_B_SHIFT            (2)
#define PAGE_C_SHIFT            (3)
#define PAGE_TEX_SHIFT          (12)

#define PAGE_XN_4K_MASK         (0xFFFFFFFE)
#define PAGE_XN_4K_SHIFT        (0)
#define PAGE_XN_64K_MASK        (0xFFFF7FFF)
#define PAGE_XN_64K_SHIFT       (15)

#define PAGE_DOMAIN_MASK        (0xFFFFFE1F)
#define PAGE_DOMAIN_SHIFT       (5)

#define PAGE_P_MASK             (0xFFFFFDFF)
#define PAGE_P_SHIFT            (9)

#define PAGE_AP_MASK            (0xFFFFFDCF)
#define PAGE_AP_SHIFT           (4)
#define PAGE_AP2_SHIFT          (9)

#define PAGE_S_MASK             (0xFFFFFBFF)
#define PAGE_S_SHIFT            (10)

#define PAGE_NG_MASK            (0xFFFFF7FF)
#define PAGE_NG_SHIFT           (11)

#define PAGE_NS_MASK            (0xFFFFFFF7)
#define PAGE_NS_SHIFT           (3)

#define OFFSET_1M               (0x00100000)
#define OFFSET_64K              (0x00010000)
#define OFFSET_4K               (0x00001000)

#define DESCRIPTOR_FAULT        (0x00000000)

/* Attributes enumerations */

/* Region size attributes */
typedef enum
{
   SECTION,
   PAGE_4k,
   PAGE_64k,
} mmu_region_size_Type;

/* Region type attributes */
typedef enum
{
   NORMAL,
   DEVICE,
   SHARED_DEVICE,
   NON_SHARED_DEVICE,
   STRONGLY_ORDERED
} mmu_memory_Type;

/* Region cacheability attributes */
typedef enum
{
   NON_CACHEABLE,
   WB_WA,
   WT,
   WB_NO_WA,
} mmu_cacheability_Type;

/* Region parity check attributes */
typedef enum
{
   ECC_DISABLED,
   ECC_ENABLED,
} mmu_ecc_check_Type;

/* Region execution attributes */
typedef enum
{
   EXECUTE,
   NON_EXECUTE,
} mmu_execute_Type;

/* Region global attributes */
typedef enum
{
   GLOBAL,
   NON_GLOBAL,
} mmu_global_Type;

/* Region shareability attributes */
typedef enum
{
   NON_SHARED,
   SHARED,
} mmu_shared_Type;

/* Region security attributes */
typedef enum
{
   SECURE,
   NON_SECURE,
} mmu_secure_Type;

/* Region access attributes */
typedef enum
{
   NO_ACCESS,
   RW,
   READ,
} mmu_access_Type;

/* Memory Region definition */
typedef struct RegionStruct {
    mmu_region_size_Type rg_t;
    mmu_memory_Type mem_t;
    uint8_t domain;
    mmu_cacheability_Type inner_norm_t;
    mmu_cacheability_Type outer_norm_t;
    mmu_ecc_check_Type e_t;
    mmu_execute_Type xn_t;
    mmu_global_Type g_t;
    mmu_secure_Type sec_t;
    mmu_access_Type priv_t;
    mmu_access_Type user_t;
    mmu_shared_Type sh_t;

} mmu_region_attributes_Type;

//Following macros define the descriptors and attributes
//Sect_Normal. Outer & inner wb/wa, non-shareable, executable, rw, domain 0
#define section_normal(descriptor_l1, region)     region.rg_t = SECTION; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = WB_WA; \
                                   region.outer_norm_t = WB_WA; \
                                   region.mem_t = NORMAL; \
                                   region.sec_t = SECURE; \
                                   region.xn_t = EXECUTE; \
                                   region.priv_t = RW; \
                                   region.user_t = RW; \
                                   region.sh_t = NON_SHARED; \
                                   MMU_GetSectionDescriptor(&descriptor_l1, region);

//Sect_Normal_NC. Outer & inner non-cacheable, non-shareable, executable, rw, domain 0
#define section_normal_nc(descriptor_l1, region)     region.rg_t = SECTION; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = NON_CACHEABLE; \
                                   region.outer_norm_t = NON_CACHEABLE; \
                                   region.mem_t = NORMAL; \
                                   region.sec_t = SECURE; \
                                   region.xn_t = EXECUTE; \
                                   region.priv_t = RW; \
                                   region.user_t = RW; \
                                   region.sh_t = NON_SHARED; \
                                   MMU_GetSectionDescriptor(&descriptor_l1, region);

//Sect_Normal_Cod. Outer & inner wb/wa, non-shareable, executable, ro, domain 0
#define section_normal_cod(descriptor_l1, region) region.rg_t = SECTION; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = WB_WA; \
                                   region.outer_norm_t = WB_WA; \
                                   region.mem_t = NORMAL; \
                                   region.sec_t = SECURE; \
                                   region.xn_t = EXECUTE; \
                                   region.priv_t = READ; \
                                   region.user_t = READ; \
                                   region.sh_t = NON_SHARED; \
                                   MMU_GetSectionDescriptor(&descriptor_l1, region);

//Sect_Normal_RO. Sect_Normal_Cod, but not executable
#define section_normal_ro(descriptor_l1, region)  region.rg_t = SECTION; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = WB_WA; \
                                   region.outer_norm_t = WB_WA; \
                                   region.mem_t = NORMAL; \
                                   region.sec_t = SECURE; \
                                   region.xn_t = NON_EXECUTE; \
                                   region.priv_t = READ; \
                                   region.user_t = READ; \
                                   region.sh_t = NON_SHARED; \
                                   MMU_GetSectionDescriptor(&descriptor_l1, region);

//Sect_Normal_RW. Sect_Normal_Cod, but writeable and not executable
#define section_normal_rw(descriptor_l1, region) region.rg_t = SECTION; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = WB_WA; \
                                   region.outer_norm_t = WB_WA; \
                                   region.mem_t = NORMAL; \
                                   region.sec_t = SECURE; \
                                   region.xn_t = NON_EXECUTE; \
                                   region.priv_t = RW; \
                                   region.user_t = RW; \
                                   region.sh_t = NON_SHARED; \
                                   MMU_GetSectionDescriptor(&descriptor_l1, region);
//Sect_SO. Strongly-ordered (therefore shareable), not executable, rw, domain 0, base addr 0
#define section_so(descriptor_l1, region) region.rg_t = SECTION; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = NON_CACHEABLE; \
                                   region.outer_norm_t = NON_CACHEABLE; \
                                   region.mem_t = STRONGLY_ORDERED; \
                                   region.sec_t = SECURE; \
                                   region.xn_t = NON_EXECUTE; \
                                   region.priv_t = RW; \
                                   region.user_t = RW; \
                                   region.sh_t = NON_SHARED; \
                                   MMU_GetSectionDescriptor(&descriptor_l1, region);

//Sect_Device_RO. Device, non-shareable, non-executable, ro, domain 0, base addr 0
#define section_device_ro(descriptor_l1, region) region.rg_t = SECTION; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = NON_CACHEABLE; \
                                   region.outer_norm_t = NON_CACHEABLE; \
                                   region.mem_t = STRONGLY_ORDERED; \
                                   region.sec_t = SECURE; \
                                   region.xn_t = NON_EXECUTE; \
                                   region.priv_t = READ; \
                                   region.user_t = READ; \
                                   region.sh_t = NON_SHARED; \
                                   MMU_GetSectionDescriptor(&descriptor_l1, region);

//Sect_Device_RW. Sect_Device_RO, but writeable
#define section_device_rw(descriptor_l1, region) region.rg_t = SECTION; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = NON_CACHEABLE; \
                                   region.outer_norm_t = NON_CACHEABLE; \
                                   region.mem_t = STRONGLY_ORDERED; \
                                   region.sec_t = SECURE; \
                                   region.xn_t = NON_EXECUTE; \
                                   region.priv_t = RW; \
                                   region.user_t = RW; \
                                   region.sh_t = NON_SHARED; \
                                   MMU_GetSectionDescriptor(&descriptor_l1, region);
//Page_4k_Device_RW.  Shared device, not executable, rw, domain 0
#define page4k_device_rw(descriptor_l1, descriptor_l2, region) region.rg_t = PAGE_4k; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = NON_CACHEABLE; \
                                   region.outer_norm_t = NON_CACHEABLE; \
                                   region.mem_t = SHARED_DEVICE; \
                                   region.sec_t = SECURE; \
                                   region.xn_t = NON_EXECUTE; \
                                   region.priv_t = RW; \
                                   region.user_t = RW; \
                                   region.sh_t = NON_SHARED; \
                                   MMU_GetPageDescriptor(&descriptor_l1, &descriptor_l2, region);

//Page_64k_Device_RW.  Shared device, not executable, rw, domain 0
#define page64k_device_rw(descriptor_l1, descriptor_l2, region)  region.rg_t = PAGE_64k; \
                                   region.domain = 0x0; \
                                   region.e_t = ECC_DISABLED; \
                                   region.g_t = GLOBAL; \
                                   region.inner_norm_t = NON_CACHEABLE; \
                                   region.outer_norm_t = NON_CACHEABLE; \
                                   region.mem_t = SHARED_DEVICE; \
                                   region.sec_t = SECURE; \
                                   region.xn_t = NON_EXECUTE; \
                                   region.priv_t = RW; \
                                   region.user_t = RW; \
                                   region.sh_t = NON_SHARED; \
                                   MMU_GetPageDescriptor(&descriptor_l1, &descriptor_l2, region);

/** \brief  Set section execution-never attribute

  \param [out]    descriptor_l1  L1 descriptor.
  \param [in]                xn  Section execution-never attribute : EXECUTE , NON_EXECUTE.

  \return          0
*/
__STATIC_INLINE int MMU_XNSection(uint32_t *descriptor_l1, mmu_execute_Type xn)
{
  *descriptor_l1 &= SECTION_XN_MASK;
  *descriptor_l1 |= ((xn & 0x1) << SECTION_XN_SHIFT);
  return 0;
}

/** \brief  Set section domain

  \param [out]    descriptor_l1  L1 descriptor.
  \param [in]            domain  Section domain

  \return          0
*/
__STATIC_INLINE int MMU_DomainSection(uint32_t *descriptor_l1, uint8_t domain)
{
  *descriptor_l1 &= SECTION_DOMAIN_MASK;
  *descriptor_l1 |= ((domain & 0xF) << SECTION_DOMAIN_SHIFT);
  return 0;
}

/** \brief  Set section parity check

  \param [out]    descriptor_l1  L1 descriptor.
  \param [in]              p_bit Parity check: ECC_DISABLED, ECC_ENABLED

  \return          0
*/
__STATIC_INLINE int MMU_PSection(uint32_t *descriptor_l1, mmu_ecc_check_Type p_bit)
{
  *descriptor_l1 &= SECTION_P_MASK;
  *descriptor_l1 |= ((p_bit & 0x1) << SECTION_P_SHIFT);
  return 0;
}

/** \brief  Set section access privileges

  \param [out]    descriptor_l1  L1 descriptor.
  \param [in]              user  User Level Access: NO_ACCESS, RW, READ
  \param [in]              priv  Privilege Level Access: NO_ACCESS, RW, READ
  \param [in]               afe  Access flag enable

  \return          0
*/
__STATIC_INLINE int MMU_APSection(uint32_t *descriptor_l1, mmu_access_Type user, mmu_access_Type priv, uint32_t afe)
{
  uint32_t ap = 0;

  if (afe == 0) { //full access
    if ((priv == NO_ACCESS) && (user == NO_ACCESS)) { ap = 0x0; }
    else if ((priv == RW) && (user == NO_ACCESS))   { ap = 0x1; }
    else if ((priv == RW) && (user == READ))        { ap = 0x2; }
    else if ((priv == RW) && (user == RW))          { ap = 0x3; }
    else if ((priv == READ) && (user == NO_ACCESS)) { ap = 0x5; }
    else if ((priv == READ) && (user == READ))      { ap = 0x7; }
  }

  else { //Simplified access
    if ((priv == RW) && (user == NO_ACCESS))        { ap = 0x1; }
    else if ((priv == RW) && (user == RW))          { ap = 0x3; }
    else if ((priv == READ) && (user == NO_ACCESS)) { ap = 0x5; }
    else if ((priv == READ) && (user == READ))      { ap = 0x7; }
  }

  *descriptor_l1 &= SECTION_AP_MASK;
  *descriptor_l1 |= (ap & 0x3) << SECTION_AP_SHIFT;
  *descriptor_l1 |= ((ap & 0x4)>>2) << SECTION_AP2_SHIFT;

  return 0;
}

/** \brief  Set section shareability

  \param [out]    descriptor_l1  L1 descriptor.
  \param [in]             s_bit  Section shareability: NON_SHARED, SHARED

  \return          0
*/
__STATIC_INLINE int MMU_SharedSection(uint32_t *descriptor_l1, mmu_shared_Type s_bit)
{
  *descriptor_l1 &= SECTION_S_MASK;
  *descriptor_l1 |= ((s_bit & 0x1) << SECTION_S_SHIFT);
  return 0;
}

/** \brief  Set section Global attribute

  \param [out]    descriptor_l1  L1 descriptor.
  \param [in]             g_bit  Section attribute: GLOBAL, NON_GLOBAL

  \return          0
*/
__STATIC_INLINE int MMU_GlobalSection(uint32_t *descriptor_l1, mmu_global_Type g_bit)
{
  *descriptor_l1 &= SECTION_NG_MASK;
  *descriptor_l1 |= ((g_bit & 0x1) << SECTION_NG_SHIFT);
  return 0;
}

/** \brief  Set section Security attribute

  \param [out]    descriptor_l1  L1 descriptor.
  \param [in]             s_bit  Section Security attribute: SECURE, NON_SECURE

  \return          0
*/
__STATIC_INLINE int MMU_SecureSection(uint32_t *descriptor_l1, mmu_secure_Type s_bit)
{
  *descriptor_l1 &= SECTION_NS_MASK;
  *descriptor_l1 |= ((s_bit & 0x1) << SECTION_NS_SHIFT);
  return 0;
}

/* Page 4k or 64k */
/** \brief  Set 4k/64k page execution-never attribute

  \param [out]    descriptor_l2  L2 descriptor.
  \param [in]                xn  Page execution-never attribute : EXECUTE , NON_EXECUTE.
  \param [in]              page  Page size: PAGE_4k, PAGE_64k,

  \return          0
*/
__STATIC_INLINE int MMU_XNPage(uint32_t *descriptor_l2, mmu_execute_Type xn, mmu_region_size_Type page)
{
  if (page == PAGE_4k)
  {
      *descriptor_l2 &= PAGE_XN_4K_MASK;
      *descriptor_l2 |= ((xn & 0x1) << PAGE_XN_4K_SHIFT);
  }
  else
  {
      *descriptor_l2 &= PAGE_XN_64K_MASK;
      *descriptor_l2 |= ((xn & 0x1) << PAGE_XN_64K_SHIFT);
  }
  return 0;
}

/** \brief  Set 4k/64k page domain

  \param [out]    descriptor_l1  L1 descriptor.
  \param [in]            domain  Page domain

  \return          0
*/
__STATIC_INLINE int MMU_DomainPage(uint32_t *descriptor_l1, uint8_t domain)
{
  *descriptor_l1 &= PAGE_DOMAIN_MASK;
  *descriptor_l1 |= ((domain & 0xf) << PAGE_DOMAIN_SHIFT);
  return 0;
}

/** \brief  Set 4k/64k page parity check

  \param [out]    descriptor_l1  L1 descriptor.
  \param [in]              p_bit Parity check: ECC_DISABLED, ECC_ENABLED

  \return          0
*/
__STATIC_INLINE int MMU_PPage(uint32_t *descriptor_l1, mmu_ecc_check_Type p_bit)
{
  *descriptor_l1 &= SECTION_P_MASK;
  *descriptor_l1 |= ((p_bit & 0x1) << SECTION_P_SHIFT);
  return 0;
}

/** \brief  Set 4k/64k page access privileges

  \param [out]    descriptor_l2  L2 descriptor.
  \param [in]              user  User Level Access: NO_ACCESS, RW, READ
  \param [in]              priv  Privilege Level Access: NO_ACCESS, RW, READ
  \param [in]               afe  Access flag enable

  \return          0
*/
__STATIC_INLINE int MMU_APPage(uint32_t *descriptor_l2, mmu_access_Type user, mmu_access_Type priv, uint32_t afe)
{
  uint32_t ap = 0;

  if (afe == 0) { //full access
    if ((priv == NO_ACCESS) && (user == NO_ACCESS)) { ap = 0x0; }
    else if ((priv == RW) && (user == NO_ACCESS))   { ap = 0x1; }
    else if ((priv == RW) && (user == READ))        { ap = 0x2; }
    else if ((priv == RW) && (user == RW))          { ap = 0x3; }
    else if ((priv == READ) && (user == NO_ACCESS)) { ap = 0x5; }
    else if ((priv == READ) && (user == READ))      { ap = 0x6; }
  }

  else { //Simplified access
    if ((priv == RW) && (user == NO_ACCESS))        { ap = 0x1; }
    else if ((priv == RW) && (user == RW))          { ap = 0x3; }
    else if ((priv == READ) && (user == NO_ACCESS)) { ap = 0x5; }
    else if ((priv == READ) && (user == READ))      { ap = 0x7; }
  }

  *descriptor_l2 &= PAGE_AP_MASK;
  *descriptor_l2 |= (ap & 0x3) << PAGE_AP_SHIFT;
  *descriptor_l2 |= ((ap & 0x4)>>2) << PAGE_AP2_SHIFT;

  return 0;
}

/** \brief  Set 4k/64k page shareability

  \param [out]    descriptor_l2  L2 descriptor.
  \param [in]             s_bit  4k/64k page shareability: NON_SHARED, SHARED

  \return          0
*/
__STATIC_INLINE int MMU_SharedPage(uint32_t *descriptor_l2, mmu_shared_Type s_bit)
{
  *descriptor_l2 &= PAGE_S_MASK;
  *descriptor_l2 |= ((s_bit & 0x1) << PAGE_S_SHIFT);
  return 0;
}

/** \brief  Set 4k/64k page Global attribute

  \param [out]    descriptor_l2  L2 descriptor.
  \param [in]             g_bit  4k/64k page attribute: GLOBAL, NON_GLOBAL

  \return          0
*/
__STATIC_INLINE int MMU_GlobalPage(uint32_t *descriptor_l2, mmu_global_Type g_bit)
{
  *descriptor_l2 &= PAGE_NG_MASK;
  *descriptor_l2 |= ((g_bit & 0x1) << PAGE_NG_SHIFT);
  return 0;
}

/** \brief  Set 4k/64k page Security attribute

  \param [out]    descriptor_l1  L1 descriptor.
  \param [in]             s_bit  4k/64k page Security attribute: SECURE, NON_SECURE

  \return          0
*/
__STATIC_INLINE int MMU_SecurePage(uint32_t *descriptor_l1, mmu_secure_Type s_bit)
{
  *descriptor_l1 &= PAGE_NS_MASK;
  *descriptor_l1 |= ((s_bit & 0x1) << PAGE_NS_SHIFT);
  return 0;
}

/** \brief  Set Section memory attributes

  \param [out]    descriptor_l1  L1 descriptor.
  \param [in]               mem  Section memory type: NORMAL, DEVICE, SHARED_DEVICE, NON_SHARED_DEVICE, STRONGLY_ORDERED
  \param [in]             outer  Outer cacheability: NON_CACHEABLE, WB_WA, WT, WB_NO_WA,
  \param [in]             inner  Inner cacheability: NON_CACHEABLE, WB_WA, WT, WB_NO_WA,

  \return          0
*/
__STATIC_INLINE int MMU_MemorySection(uint32_t *descriptor_l1, mmu_memory_Type mem, mmu_cacheability_Type outer, mmu_cacheability_Type inner)
{
  *descriptor_l1 &= SECTION_TEXCB_MASK;

  if (STRONGLY_ORDERED == mem)
  {
    return 0;
  }
  else if (SHARED_DEVICE == mem)
  {
    *descriptor_l1 |= (UINT32_C(1) << SECTION_B_SHIFT);
  }
  else if (NON_SHARED_DEVICE == mem)
  {
    *descriptor_l1 |= (UINT32_C(1) << SECTION_TEX1_SHIFT);
  }
  else if (NORMAL == mem)
  {
   *descriptor_l1 |= 1 << SECTION_TEX2_SHIFT;
   switch(inner)
   {
      case NON_CACHEABLE:
        break;
      case WB_WA:
        *descriptor_l1 |= (UINT32_C(1) << SECTION_B_SHIFT);
        break;
      case WT:
        *descriptor_l1 |= 1 << SECTION_C_SHIFT;
        break;
      case WB_NO_WA:
        *descriptor_l1 |= (UINT32_C(1) << SECTION_B_SHIFT) | (UINT32_C(1) << SECTION_C_SHIFT);
        break;
    }
    switch(outer)
    {
      case NON_CACHEABLE:
        break;
      case WB_WA:
        *descriptor_l1 |= (UINT32_C(1) << SECTION_TEX0_SHIFT);
        break;
      case WT:
        *descriptor_l1 |= 1 << SECTION_TEX1_SHIFT;
        break;
      case WB_NO_WA:
        *descriptor_l1 |= (UINT32_C(1) << SECTION_TEX0_SHIFT) | (UINT32_C(1) << SECTION_TEX0_SHIFT);
        break;
    }
  }
  return 0;
}

/** \brief  Set 4k/64k page memory attributes

  \param [out]    descriptor_l2  L2 descriptor.
  \param [in]               mem  4k/64k page memory type: NORMAL, DEVICE, SHARED_DEVICE, NON_SHARED_DEVICE, STRONGLY_ORDERED
  \param [in]             outer  Outer cacheability: NON_CACHEABLE, WB_WA, WT, WB_NO_WA,
  \param [in]             inner  Inner cacheability: NON_CACHEABLE, WB_WA, WT, WB_NO_WA,
  \param [in]              page  Page size

  \return          0
*/
__STATIC_INLINE int MMU_MemoryPage(uint32_t *descriptor_l2, mmu_memory_Type mem, mmu_cacheability_Type outer, mmu_cacheability_Type inner, mmu_region_size_Type page)
{
  *descriptor_l2 &= PAGE_4K_TEXCB_MASK;

  if (page == PAGE_64k)
  {
    //same as section
    MMU_MemorySection(descriptor_l2, mem, outer, inner);
  }
  else
  {
    if (STRONGLY_ORDERED == mem)
    {
      return 0;
    }
    else if (SHARED_DEVICE == mem)
    {
      *descriptor_l2 |= (UINT32_C(1) << PAGE_4K_B_SHIFT);
    }
    else if (NON_SHARED_DEVICE == mem)
    {
      *descriptor_l2 |= (UINT32_C(1) << PAGE_4K_TEX1_SHIFT);
    }
    else if (NORMAL == mem)
    {
      *descriptor_l2 |= 1 << PAGE_4K_TEX2_SHIFT;
      switch(inner)
      {
        case NON_CACHEABLE:
          break;
        case WB_WA:
          *descriptor_l2 |= (UINT32_C(1) << PAGE_4K_B_SHIFT);
          break;
        case WT:
          *descriptor_l2 |= 1 << PAGE_4K_C_SHIFT;
          break;
        case WB_NO_WA:
          *descriptor_l2 |= (UINT32_C(1) << PAGE_4K_B_SHIFT) | (UINT32_C(1) << PAGE_4K_C_SHIFT);
          break;
      }
      switch(outer)
      {
        case NON_CACHEABLE:
          break;
        case WB_WA:
          *descriptor_l2 |= (UINT32_C(1) << PAGE_4K_TEX0_SHIFT);
          break;
        case WT:
          *descriptor_l2 |= 1 << PAGE_4K_TEX1_SHIFT;
          break;
        case WB_NO_WA:
          *descriptor_l2 |= (UINT32_C(1) << PAGE_4K_TEX0_SHIFT) | (UINT32_C(1) << PAGE_4K_TEX0_SHIFT);
          break;
      }
    }
  }

  return 0;
}

/** \brief  Create a L1 section descriptor

  \param [out]     descriptor  L1 descriptor
  \param [in]      reg  Section attributes

  \return          0
*/
__STATIC_INLINE int MMU_GetSectionDescriptor(uint32_t *descriptor, mmu_region_attributes_Type reg)
{
  *descriptor  = 0;

  MMU_MemorySection(descriptor, reg.mem_t, reg.outer_norm_t, reg.inner_norm_t);
  MMU_XNSection(descriptor,reg.xn_t);
  MMU_DomainSection(descriptor, reg.domain);
  MMU_PSection(descriptor, reg.e_t);
  MMU_APSection(descriptor, reg.user_t, reg.priv_t, 1);
  MMU_SharedSection(descriptor,reg.sh_t);
  MMU_GlobalSection(descriptor,reg.g_t);
  MMU_SecureSection(descriptor,reg.sec_t);
  *descriptor &= SECTION_MASK;
  *descriptor |= SECTION_DESCRIPTOR;

  return 0;
}


/** \brief  Create a L1 and L2 4k/64k page descriptor

  \param [out]       descriptor  L1 descriptor
  \param [out]      descriptor2  L2 descriptor
  \param [in]               reg  4k/64k page attributes

  \return          0
*/
__STATIC_INLINE int MMU_GetPageDescriptor(uint32_t *descriptor, uint32_t *descriptor2, mmu_region_attributes_Type reg)
{
  *descriptor  = 0;
  *descriptor2 = 0;

  switch (reg.rg_t)
  {
    case PAGE_4k:
      MMU_MemoryPage(descriptor2, reg.mem_t, reg.outer_norm_t, reg.inner_norm_t, PAGE_4k);
      MMU_XNPage(descriptor2, reg.xn_t, PAGE_4k);
      MMU_DomainPage(descriptor, reg.domain);
      MMU_PPage(descriptor, reg.e_t);
      MMU_APPage(descriptor2, reg.user_t, reg.priv_t, 1);
      MMU_SharedPage(descriptor2,reg.sh_t);
      MMU_GlobalPage(descriptor2,reg.g_t);
      MMU_SecurePage(descriptor,reg.sec_t);
      *descriptor &= PAGE_L1_MASK;
      *descriptor |= PAGE_L1_DESCRIPTOR;
      *descriptor2 &= PAGE_L2_4K_MASK;
      *descriptor2 |= PAGE_L2_4K_DESC;
      break;

    case PAGE_64k:
      MMU_MemoryPage(descriptor2, reg.mem_t, reg.outer_norm_t, reg.inner_norm_t, PAGE_64k);
      MMU_XNPage(descriptor2, reg.xn_t, PAGE_64k);
      MMU_DomainPage(descriptor, reg.domain);
      MMU_PPage(descriptor, reg.e_t);
      MMU_APPage(descriptor2, reg.user_t, reg.priv_t, 1);
      MMU_SharedPage(descriptor2,reg.sh_t);
      MMU_GlobalPage(descriptor2,reg.g_t);
      MMU_SecurePage(descriptor,reg.sec_t);
      *descriptor &= PAGE_L1_MASK;
      *descriptor |= PAGE_L1_DESCRIPTOR;
      *descriptor2 &= PAGE_L2_64K_MASK;
      *descriptor2 |= PAGE_L2_64K_DESC;
      break;

    case SECTION:
      //error
      break;
  }

  return 0;
}

/** \brief  Create a 1MB Section

  \param [in]               ttb  Translation table base address
  \param [in]      base_address  Section base address
  \param [in]             count  Number of sections to create
  \param [in]     descriptor_l1  L1 descriptor (region attributes)

*/
__STATIC_INLINE void MMU_TTSection(uint32_t *ttb, uint32_t base_address, uint32_t count, uint32_t descriptor_l1)
{
  uint32_t offset;
  uint32_t entry;
  uint32_t i;

  offset = base_address >> 20;
  entry  = (base_address & 0xFFF00000) | descriptor_l1;

  //4 bytes aligned
  ttb = ttb + offset;

  for (i = 0; i < count; i++ )
  {
    //4 bytes aligned
    *ttb++ = entry;
    entry += OFFSET_1M;
  }
}

/** \brief  Create a 4k page entry

  \param [in]               ttb  L1 table base address
  \param [in]      base_address  4k base address
  \param [in]             count  Number of 4k pages to create
  \param [in]     descriptor_l1  L1 descriptor (region attributes)
  \param [in]            ttb_l2  L2 table base address
  \param [in]     descriptor_l2  L2 descriptor (region attributes)

*/
__STATIC_INLINE void MMU_TTPage4k(uint32_t *ttb, uint32_t base_address, uint32_t count, uint32_t descriptor_l1, uint32_t *ttb_l2, uint32_t descriptor_l2 )
{

	uintptr_t offset, offset2;
  uintptr_t entry, entry2;
  uint32_t i;

  offset = base_address >> 20;
  entry  = ((uintptr_t)ttb_l2 & 0xFFFFFC00) | descriptor_l1;

  //4 bytes aligned
  ttb += offset;
  //create l1_entry
  *ttb = entry;

  offset2 = (base_address & 0xff000) >> 12;
  ttb_l2 += offset2;
  entry2 = (base_address & 0xFFFFF000) | descriptor_l2;
  for (i = 0; i < count; i++ )
  {
    //4 bytes aligned
    *ttb_l2++ = entry2;
    entry2 += OFFSET_4K;
  }
}

/** \brief  Create a 64k page entry

  \param [in]               ttb  L1 table base address
  \param [in]      base_address  64k base address
  \param [in]             count  Number of 64k pages to create
  \param [in]     descriptor_l1  L1 descriptor (region attributes)
  \param [in]            ttb_l2  L2 table base address
  \param [in]     descriptor_l2  L2 descriptor (region attributes)

*/
__STATIC_INLINE void MMU_TTPage64k(uint32_t *ttb, uint32_t base_address, uint32_t count, uint32_t descriptor_l1, uint32_t *ttb_l2, uint32_t descriptor_l2 )
{
	uintptr_t offset, offset2;
  uintptr_t entry, entry2;
  uint32_t i,j;


  offset = base_address >> 20;
  entry  = ((uintptr_t)ttb_l2 & 0xFFFFFC00) | descriptor_l1;

  //4 bytes aligned
  ttb += offset;
  //create l1_entry
  *ttb = entry;

  offset2 = (base_address & 0xff000) >> 12;
  ttb_l2 += offset2;
  entry2 = (base_address & 0xFFFF0000) | descriptor_l2;
  for (i = 0; i < count; i++ )
  {
    //create 16 entries
    for (j = 0; j < 16; j++)
    {
      //4 bytes aligned
      *ttb_l2++ = entry2;
    }
    entry2 += OFFSET_64K;
  }
}

#endif


/** \brief  Enable MMU
*/
//__STATIC_INLINE void MMU_Enable(void)
//{
//	// Set M bit 0 to enable the MMU
//	// Set AFE bit to enable simplified access permissions model
//	// Clear TRE bit to disable TEX remap and A bit to disable strict alignment fault checking
//	__set_SCTLR_EL3(__get_SCTLR_EL3() | SCTLR_EL3_M_Msk);
//	__ISB();
//}

/** \brief  Disable MMU
*/
//__STATIC_INLINE void MMU_Disable(void)
//{
//	// Clear M bit 0 to disable the MMU
//	__set_SCTLR_EL3(__get_SCTLR_EL3() & ~ SCTLR_EL3_M_Msk);
//	__ISB();
//}

/** \brief  Invalidate entire unified TLB
*/

//__STATIC_INLINE void MMU_InvalidateTLB(void)
//{
//#if 0
//  __set_TLBIALL(0);
//  __DSB();     //ensure completion of the invalidation
//  __ISB();     //ensure instruction fetch path sees new state
//#endif
//}


/** \brief  Enable Floating Point Unit */
//__STATIC_INLINE void __FPU_Enable(void)
//{
//  uint32_t temp=0;
//  __ASM volatile
//  (
//    // In AArch64, you do not need to enable access to the NEON and FP registers.
//    // However, access to  the NEON and FP registers can still be trapped.
//
//    // Disable trapping of   accessing in EL3 and EL2.
//    "        MSR    CPTR_EL3, XZR    \n"
//    "        MSR    CPTR_EL2, XZR    \n"
//
//    // Disable access trapping in EL1 and EL0.
//    "        MOV    %0, #(0x3 << 20) \n"
//
//    // FPEN disables trapping to EL1.
//    "        MSR    CPACR_EL1, %0    \n"
//
//    //Ensure that subsequent instructions occur in the context of VFP/NEON access permitted
//    "        ISB                       "
//
//    : : "r"(temp): "cc"
//  );
//}

#if 0
/* ##########################  L1 Cache functions  ################################# */

/** \brief Enable Caches by setting I and C bits in SCTLR register.
*/
__STATIC_FORCEINLINE void L1C_EnableCaches(void)
{
	__set_SCTLR_EL3( __get_SCTLR_EL3() | SCTLR_EL3_I_Msk | SCTLR_EL3_C_Msk);
	__ISB();
}

/** \brief Disable Caches by clearing I and C bits in SCTLR register.
*/
__STATIC_FORCEINLINE void L1C_DisableCaches(void)
{
	__set_SCTLR_EL3( __get_SCTLR_EL3() & (~ SCTLR_EL3_I_Msk) & (~ SCTLR_EL3_C_Msk));
	__ISB();
}

/** \brief  Enable Branch Prediction by setting Z bit in SCTLR register.
*/
__STATIC_FORCEINLINE void L1C_EnableBTAC(void) {
#if 0
  __set_SCTLR_EL3( __get_SCTLR_EL3() | SCTLR_EL3_Z_Msk);
  __ISB();
#endif
}

/** \brief  Disable Branch Prediction by clearing Z bit in SCTLR register.
*/
__STATIC_FORCEINLINE void L1C_DisableBTAC(void) {
#if 0
  __set_SCTLR_EL3( __get_SCTLR_EL3() & (~SCTLR_EL3_Z_Msk));
  __ISB();
#endif
}

/** \brief  Invalidate entire branch predictor array
*/
__STATIC_FORCEINLINE void L1C_InvalidateBTAC(void) {
#if 0
  __set_BPIALL(0);
  __DSB();     //ensure completion of the invalidation
  __ISB();     //ensure instruction fetch path sees new state
#endif
}

/** \brief  Clean instruction cache line by address.
* \param [in] va Pointer to instructions to clear the cache for.
*/
__STATIC_FORCEINLINE void L1C_InvalidateICacheMVA(void *va) {
#if 0
  __set_ICIMVAC((uintptr_t)va);
  __DSB();     //ensure completion of the invalidation
  __ISB();     //ensure instruction fetch path sees new I cache state
#endif
}

/** \brief  Invalidate the whole instruction cache
*/
__STATIC_FORCEINLINE void L1C_InvalidateICacheAll(void) {
#if 0
  __set_ICIALLU(0);
  __DSB();     //ensure completion of the invalidation
  __ISB();     //ensure instruction fetch path sees new I cache state
#endif
}

/** \brief  Clean data cache line by address.
* \param [in] va Pointer to data to clear the cache for.
*/
__STATIC_FORCEINLINE void L1C_CleanDCacheMVA(void *va) {
  __set_DCCVAC((uintptr_t)va);
  __DMB();     //ensure the ordering of data cache maintenance operations and their effects
}

/** \brief  Invalidate data cache line by address.
* \param [in] va Pointer to data to invalidate the cache for.
*/
__STATIC_FORCEINLINE void L1C_InvalidateDCacheMVA(void *va) {
  __set_DCIVAC((uintptr_t)va);
  __DMB();     //ensure the ordering of data cache maintenance operations and their effects
}

/** \brief  Clean and Invalidate data cache by address.
* \param [in] va Pointer to data to invalidate the cache for.
*/
__STATIC_FORCEINLINE void L1C_CleanInvalidateDCacheMVA(void *va) {
	__set_DCCIVAC((uintptr_t)va);
	__DMB();     //ensure the ordering of data cache maintenance operations and their effects
}

/** \brief  Apply cache maintenance to given cache level.
* \param [in] level cache level to be maintained
* \param [in] maint 0 - invalidate, 1 - clean, otherwise - invalidate and clean
*/
__STATIC_FORCEINLINE void __L1C_MaintainDCacheSetWay(uint32_t level, uint32_t maint)
{
  uint32_t Dummy;
  uint32_t ccsidr;
  uint32_t num_sets;
  uint32_t num_ways;
  uint32_t shift_way;
  uint32_t log2_linesize;
   uint8_t log2_num_ways;

  Dummy = level << 1U;
  /* set csselr, select ccsidr register */
  __set_CSSELR_EL1(Dummy);
  /* get current ccsidr register */
  ccsidr = __get_CCSIDR_EL1();
  num_sets = ((ccsidr & 0x0FFFE000U) >> 13U) + 1U;
  num_ways = ((ccsidr & 0x00001FF8U) >> 3U) + 1U;
  log2_linesize = (ccsidr & 0x00000007U) + 2U + 2U;
  log2_num_ways = __log2_up(num_ways);
  if (log2_num_ways > 32U) {
    return; // FATAL ERROR
  }
  shift_way = 32U - log2_num_ways;
  for(int32_t way = num_ways-1; way >= 0; way--)
  {
    for(int32_t set = num_sets-1; set >= 0; set--)
    {
    	uint64_t Dummy = (level << 1U) | (((uint32_t)set) << log2_linesize) | (((uint32_t)way) << shift_way);
      switch (maint)
      {
        case 0U: __set_DCISW(Dummy);  break;
        case 1U: __set_DCCSW(Dummy);  break;
        default: __set_DCCISW(Dummy); break;
      }
    }
  }
  __DMB();
}

#endif

/** \brief  Clean and Invalidate the entire data or unified cache
* \param [in] op 0 - invalidate, 1 - clean, otherwise - invalidate and clean
*/
//__STATIC_FORCEINLINE void L1C_CleanInvalidateCache(uint32_t op) {
//  uint32_t clidr;
//  uint32_t cache_type;
//  clidr =  __get_CLIDR_EL1();
//  for(uint32_t i = 0U; i<7U; i++)
//  {
//    cache_type = (clidr >> i*3U) & 0x7UL;
//    if ((cache_type >= 2U) && (cache_type <= 4U))
//    {
//      __L1C_MaintainDCacheSetWay(i, op);
//    }
//  }
//}

/** \brief  Invalidate the whole data cache.
*/
//__STATIC_FORCEINLINE void L1C_InvalidateDCacheAll(void) {
//  L1C_CleanInvalidateCache(0);
//}

/** \brief  Clean the whole data cache.
 */
//__STATIC_FORCEINLINE void L1C_CleanDCacheAll(void) {
//  L1C_CleanInvalidateCache(1);
//}


#ifdef __cplusplus
}
#endif


#endif /* INC_CA_MMU_64_H_ */
