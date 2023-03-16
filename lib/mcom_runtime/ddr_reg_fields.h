/*
 * reg_fields.h
 *
 *  Created on: Aug 29, 2013
 *      Author: dzagrebin
 */

#ifndef REG_FIELDS_H_UMCTL2
#define REG_FIELDS_H_UMCTL2

#define MASK_SNPS_DDR_REG_FIELDS_H_FIELD(max_, min_)                           \
    ((1 << (max_ - min_ + 1)) - 1)
#define SNPS_DDR_REG_FIELDS_H_FIELD(a, max_, min_)                             \
    (((a)&MASK_SNPS_DDR_REG_FIELDS_H_FIELD(max_, min_)) << min_)
#define GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, max_, min_)                         \
    (((a) >> min_) & MASK_SNPS_DDR_REG_FIELDS_H_FIELD(max_, min_))
#define SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, max_, min_)                    \
    (((reg) & (~(MASK_SNPS_DDR_REG_FIELDS_H_FIELD(max_, min_) << min_))) |     \
     SNPS_DDR_REG_FIELDS_H_FIELD(f, max_, min_))

#define OFFSET_SNPS_DDR_MSTR_DDR3 0
#define SNPS_DDR_MSTR_DDR3(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 0, 0)
#define GET_SNPS_DDR_MSTR_DDR3(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 0, 0)
#define SET_SNPS_DDR_MSTR_DDR3(reg, f)                                         \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 0, 0)

#define OFFSET_SNPS_DDR_MSTR_MOBILE 1
#define SNPS_DDR_MSTR_MOBILE(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 1, 1)
#define GET_SNPS_DDR_MSTR_MOBILE(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 1, 1)
#define SET_SNPS_DDR_MSTR_MOBILE(reg, f)                                       \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 1, 1)

#define OFFSET_SNPS_DDR_MSTR_LPDDR2 2
#define SNPS_DDR_MSTR_LPDDR2(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 2, 2)
#define GET_SNPS_DDR_MSTR_LPDDR2(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 2, 2)
#define SET_SNPS_DDR_MSTR_LPDDR2(reg, f)                                       \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 2, 2)

#define OFFSET_SNPS_DDR_MSTR_BURST_MODE 8
#define SNPS_DDR_MSTR_BURST_MODE(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 8, 8)
#define GET_SNPS_DDR_MSTR_BURST_MODE(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 8, 8)
#define SET_SNPS_DDR_MSTR_BURST_MODE(reg, f)                                   \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 8, 8)

#define OFFSET_SNPS_DDR_MSTR_BURSTCHOP 9
#define SNPS_DDR_MSTR_BURSTCHOP(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 9, 9)
#define GET_SNPS_DDR_MSTR_BURSTCHOP(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 9, 9)
#define SET_SNPS_DDR_MSTR_BURSTCHOP(reg, f)                                    \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 9, 9)

#define OFFSET_SNPS_DDR_MSTR_EN_2T_TIMING_MODE 10
#define SNPS_DDR_MSTR_EN_2T_TIMING_MODE(a)                                     \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 10, 10)
#define GET_SNPS_DDR_MSTR_EN_2T_TIMING_MODE(a)                                 \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 10, 10)
#define SET_SNPS_DDR_MSTR_EN_2T_TIMING_MODE(reg, f)                            \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 10, 10)

#define OFFSET_SNPS_DDR_MSTR_DATA_BUS_WIDTH 12
#define SNPS_DDR_MSTR_DATA_BUS_WIDTH(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 13, 12)
#define GET_SNPS_DDR_MSTR_DATA_BUS_WIDTH(a)                                    \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 13, 12)
#define SET_SNPS_DDR_MSTR_DATA_BUS_WIDTH(reg, f)                               \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 13, 12)

#define OFFSET_SNPS_DDR_MSTR_BURST_RDWR 16
#define SNPS_DDR_MSTR_BURST_RDWR(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 19, 16)
#define GET_SNPS_DDR_MSTR_BURST_RDWR(a)                                        \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 19, 16)
#define SET_SNPS_DDR_MSTR_BURST_RDWR(reg, f)                                   \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 19, 16)

#define OFFSET_SNPS_DDR_MSTR_ACTIVE_RANKS 24
#define SNPS_DDR_MSTR_ACTIVE_RANKS(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 27, 24)
#define GET_SNPS_DDR_MSTR_ACTIVE_RANKS(a)                                      \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 27, 24)
#define SET_SNPS_DDR_MSTR_ACTIVE_RANKS(reg, f)                                 \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 27, 24)

#define OFFSET_SNPS_DDR_STAT_OPERATING_MODE 0
#define SNPS_DDR_STAT_OPERATING_MODE(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 1, 0)
#define GET_SNPS_DDR_STAT_OPERATING_MODE(a)                                    \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 1, 0)
#define SET_SNPS_DDR_STAT_OPERATING_MODE(reg, f)                               \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 1, 0)

#define OFFSET_SNPS_DDR_STAT_SELFREF_TYPE 4
#define SNPS_DDR_STAT_SELFREF_TYPE(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 5, 4)
#define GET_SNPS_DDR_STAT_SELFREF_TYPE(a)                                      \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 5, 4)
#define SET_SNPS_DDR_STAT_SELFREF_TYPE(reg, f)                                 \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 5, 4)

#define OFFSET_SNPS_DDR_MRCTRL1_MR_DATA 0
#define SNPS_DDR_MRCTRL1_MR_DATA(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 15, 0)
#define GET_SNPS_DDR_MRCTRL1_MR_DATA(a)                                        \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 15, 0)
#define SET_SNPS_DDR_MRCTRL1_MR_DATA(reg, f)                                   \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 15, 0)

#define OFFSET_SNPS_DDR_DERATEINT_MR4_READ_INTERVAL 0
#define SNPS_DDR_DERATEINT_MR4_READ_INTERVAL(a)                                \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 31, 0)
#define GET_SNPS_DDR_DERATEINT_MR4_READ_INTERVAL(a)                            \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 31, 0)
#define SET_SNPS_DDR_DERATEINT_MR4_READ_INTERVAL(reg, f)                       \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 31, 0)

#define OFFSET_SNPS_DDR_PWRCTL_SELFREF_EN 0
#define SNPS_DDR_PWRCTL_SELFREF_EN(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 0, 0)
#define GET_SNPS_DDR_PWRCTL_SELFREF_EN(a)                                      \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 0, 0)
#define SET_SNPS_DDR_PWRCTL_SELFREF_EN(reg, f)                                 \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 0, 0)

#define OFFSET_SNPS_DDR_PWRCTL_POWERDOWN_EN 1
#define SNPS_DDR_PWRCTL_POWERDOWN_EN(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 1, 1)
#define GET_SNPS_DDR_PWRCTL_POWERDOWN_EN(a)                                    \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 1, 1)
#define SET_SNPS_DDR_PWRCTL_POWERDOWN_EN(reg, f)                               \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 1, 1)

#define OFFSET_SNPS_DDR_PWRCTL_DEEPPOWERDOWN_EN 2
#define SNPS_DDR_PWRCTL_DEEPPOWERDOWN_EN(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 2, 2)
#define GET_SNPS_DDR_PWRCTL_DEEPPOWERDOWN_EN(a)                                \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 2, 2)
#define SET_SNPS_DDR_PWRCTL_DEEPPOWERDOWN_EN(reg, f)                           \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 2, 2)

#define OFFSET_SNPS_DDR_PWRCTL_EN_DFI_DRAM_CLK_DISABLE 3
#define SNPS_DDR_PWRCTL_EN_DFI_DRAM_CLK_DISABLE(a)                             \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 3)
#define GET_SNPS_DDR_PWRCTL_EN_DFI_DRAM_CLK_DISABLE(a)                         \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 3)
#define SET_SNPS_DDR_PWRCTL_EN_DFI_DRAM_CLK_DISABLE(reg, f)                    \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 3, 3)

#define OFFSET_SNPS_DDR_PWRCTL_MPSM_EN 4
#define SNPS_DDR_PWRCTL_MPSM_EN(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 4, 4)
#define GET_SNPS_DDR_PWRCTL_MPSM_EN(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 4, 4)
#define SET_SNPS_DDR_PWRCTL_MPSM_EN(reg, f)                                    \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 4, 4)

#define OFFSET_SNPS_DDR_PWRCTL_SELFREF_SW 5
#define SNPS_DDR_PWRCTL_SELFREF_SW(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 5, 5)
#define GET_SNPS_DDR_PWRCTL_SELFREF_SW(a)                                      \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 5, 5)
#define SET_SNPS_DDR_PWRCTL_SELFREF_SW(reg, f)                                 \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 5, 5)

#define OFFSET_SNPS_DDR_RFSHCTL0_PER_BANK_REFRESH 2
#define SNPS_DDR_RFSHCTL0_PER_BANK_REFRESH(a)                                  \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 2, 2)
#define GET_SNPS_DDR_RFSHCTL0_PER_BANK_REFRESH(a)                              \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 2, 2)
#define SET_SNPS_DDR_RFSHCTL0_PER_BANK_REFRESH(reg, f)                         \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 2, 2)

#define OFFSET_SNPS_DDR_RFSHCTL0_REFRESH_BURST 8
#define SNPS_DDR_RFSHCTL0_REFRESH_BURST(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 10, 8)
#define GET_SNPS_DDR_RFSHCTL0_REFRESH_BURST(a)                                 \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 10, 8)
#define SET_SNPS_DDR_RFSHCTL0_REFRESH_BURST(reg, f)                            \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 10, 8)

#define OFFSET_SNPS_DDR_RFSHCTL0_REFRESH_TO_X32 12
#define SNPS_DDR_RFSHCTL0_REFRESH_TO_X32(a)                                    \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 16, 12)
#define GET_SNPS_DDR_RFSHCTL0_REFRESH_TO_X32(a)                                \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 16, 12)
#define SET_SNPS_DDR_RFSHCTL0_REFRESH_TO_X32(reg, f)                           \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 16, 12)

#define OFFSET_SNPS_DDR_RFSHCTL0_REFRESH_MARGIN 20
#define SNPS_DDR_RFSHCTL0_REFRESH_MARGIN(a)                                    \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 23, 20)
#define GET_SNPS_DDR_RFSHCTL0_REFRESH_MARGIN(a)                                \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 23, 20)
#define SET_SNPS_DDR_RFSHCTL0_REFRESH_MARGIN(reg, f)                           \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 23, 20)

#define OFFSET_SNPS_DDR_RFSHCTL3_DIS_AUTO_REFRESH 0
#define SNPS_DDR_RFSHCTL3_DIS_AUTO_REFRESH(a)                                  \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 0, 0)
#define GET_SNPS_DDR_RFSHCTL3_DIS_AUTO_REFRESH(a)                              \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 0, 0)
#define SET_SNPS_DDR_RFSHCTL3_DIS_AUTO_REFRESH(reg, f)                         \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 0, 0)

#define OFFSET_SNPS_DDR_RFSHCTL3_REFRESH_UPDATE_LEVEL 1
#define SNPS_DDR_RFSHCTL3_REFRESH_UPDATE_LEVEL(a)                              \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 1, 1)
#define GET_SNPS_DDR_RFSHCTL3_REFRESH_UPDATE_LEVEL(a)                          \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 1, 1)
#define SET_SNPS_DDR_RFSHCTL3_REFRESH_UPDATE_LEVEL(reg, f)                     \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 1, 1)

#define OFFSET_SNPS_DDR_INIT0_PRE_CKE_X1024 0
#define SNPS_DDR_INIT0_PRE_CKE_X1024(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 9, 0)
#define GET_SNPS_DDR_INIT0_PRE_CKE_X1024(a)                                    \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 9, 0)
#define SET_SNPS_DDR_INIT0_PRE_CKE_X1024(reg, f)                               \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 9, 0)

#define OFFSET_SNPS_DDR_INIT0_POST_CKE_X1024 16
#define SNPS_DDR_INIT0_POST_CKE_X1024(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 25, 16)
#define GET_SNPS_DDR_INIT0_POST_CKE_X1024(a)                                   \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 25, 16)
#define SET_SNPS_DDR_INIT0_POST_CKE_X1024(reg, f)                              \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 25, 16)

#define OFFSET_SNPS_DDR_INIT0_SKIP_DRAM_INIT 31
#define SNPS_DDR_INIT0_SKIP_DRAM_INIT(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 31, 31)
#define GET_SNPS_DDR_INIT0_SKIP_DRAM_INIT(a)                                   \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 31, 31)
#define SET_SNPS_DDR_INIT0_SKIP_DRAM_INIT(reg, f)                              \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 31, 31)

#define OFFSET_SNPS_DDR_INIT1_PRE_OCD_X32 0
#define SNPS_DDR_INIT1_PRE_OCD_X32(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define GET_SNPS_DDR_INIT1_PRE_OCD_X32(a)                                      \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define SET_SNPS_DDR_INIT1_PRE_OCD_X32(reg, f)                                 \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 3, 0)

#define OFFSET_SNPS_DDR_INIT1_FINAL_WAIT_X32 8
#define SNPS_DDR_INIT1_FINAL_WAIT_X32(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 14, 8)
#define GET_SNPS_DDR_INIT1_FINAL_WAIT_X32(a)                                   \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 14, 8)
#define SET_SNPS_DDR_INIT1_FINAL_WAIT_X32(reg, f)                              \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 14, 8)

#define OFFSET_SNPS_DDR_INIT1_DRAM_RSTN_X1024 16
#define SNPS_DDR_INIT1_DRAM_RSTN_X1024(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 23, 16)
#define GET_SNPS_DDR_INIT1_DRAM_RSTN_X1024(a)                                  \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 23, 16)
#define SET_SNPS_DDR_INIT1_DRAM_RSTN_X1024(reg, f)                             \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 23, 16)

#define OFFSET_SNPS_DDR_INIT2_MIN_STABLE_CLOCK_X1 0
#define SNPS_DDR_INIT2_MIN_STABLE_CLOCK_X1(a)                                  \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define GET_SNPS_DDR_INIT2_MIN_STABLE_CLOCK_X1(a)                              \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define SET_SNPS_DDR_INIT2_MIN_STABLE_CLOCK_X1(reg, f)                         \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 3, 0)

#define OFFSET_SNPS_DDR_INIT2_IDLE_AFTER_RESET_X32 8
#define SNPS_DDR_INIT2_IDLE_AFTER_RESET_X32(a)                                 \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 15, 8)
#define GET_SNPS_DDR_INIT2_IDLE_AFTER_RESET_X32(a)                             \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 15, 8)
#define SET_SNPS_DDR_INIT2_IDLE_AFTER_RESET_X32(reg, f)                        \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 15, 8)

#define OFFSET_SNPS_DDR_INIT3_EMR 0
#define SNPS_DDR_INIT3_EMR(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 15, 0)
#define GET_SNPS_DDR_INIT3_EMR(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 15, 0)
#define SET_SNPS_DDR_INIT3_EMR(reg, f)                                         \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 15, 0)

#define OFFSET_SNPS_DDR_INIT3_MR 16
#define SNPS_DDR_INIT3_MR(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 31, 16)
#define GET_SNPS_DDR_INIT3_MR(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 31, 16)
#define SET_SNPS_DDR_INIT3_MR(reg, f)                                          \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 31, 16)

#define OFFSET_SNPS_DDR_INIT4_EMR3 0
#define SNPS_DDR_INIT4_EMR3(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 15, 0)
#define GET_SNPS_DDR_INIT4_EMR3(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 15, 0)
#define SET_SNPS_DDR_INIT4_EMR3(reg, f)                                        \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 15, 0)

#define OFFSET_SNPS_DDR_INIT4_EMR2 16
#define SNPS_DDR_INIT4_EMR2(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 31, 16)
#define GET_SNPS_DDR_INIT4_EMR2(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 31, 16)
#define SET_SNPS_DDR_INIT4_EMR2(reg, f)                                        \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 31, 16)

#define OFFSET_SNPS_DDR_INIT5_MAX_AUTO_INIT_X1024 0
#define SNPS_DDR_INIT5_MAX_AUTO_INIT_X1024(a)                                  \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 9, 0)
#define GET_SNPS_DDR_INIT5_MAX_AUTO_INIT_X1024(a)                              \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 9, 0)
#define SET_SNPS_DDR_INIT5_MAX_AUTO_INIT_X1024(reg, f)                         \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 9, 0)

#define OFFSET_SNPS_DDR_INIT5_DEV_ZQINIT_X32 16
#define SNPS_DDR_INIT5_DEV_ZQINIT_X32(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 23, 16)
#define GET_SNPS_DDR_INIT5_DEV_ZQINIT_X32(a)                                   \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 23, 16)
#define SET_SNPS_DDR_INIT5_DEV_ZQINIT_X32(reg, f)                              \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 23, 16)

#define OFFSET_SNPS_DDR_DRAMTMG0_T_RAS_MIN 0
#define SNPS_DDR_DRAMTMG0_T_RAS_MIN(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 5, 0)
#define GET_SNPS_DDR_DRAMTMG0_T_RAS_MIN(a)                                     \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 5, 0)
#define SET_SNPS_DDR_DRAMTMG0_T_RAS_MIN(reg, f)                                \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 5, 0)

#define OFFSET_SNPS_DDR_DRAMTMG0_T_RAS_MAX 8
#define SNPS_DDR_DRAMTMG0_T_RAS_MAX(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 14, 8)
#define GET_SNPS_DDR_DRAMTMG0_T_RAS_MAX(a)                                     \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 14, 8)
#define SET_SNPS_DDR_DRAMTMG0_T_RAS_MAX(reg, f)                                \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 14, 8)

#define OFFSET_SNPS_DDR_DRAMTMG0_T_FAW 16
#define SNPS_DDR_DRAMTMG0_T_FAW(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 21, 16)
#define GET_SNPS_DDR_DRAMTMG0_T_FAW(a)                                         \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 21, 16)
#define SET_SNPS_DDR_DRAMTMG0_T_FAW(reg, f)                                    \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 21, 16)

#define OFFSET_SNPS_DDR_DRAMTMG0_WR2PRE 24
#define SNPS_DDR_DRAMTMG0_WR2PRE(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 29, 24)
#define GET_SNPS_DDR_DRAMTMG0_WR2PRE(a)                                        \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 29, 24)
#define SET_SNPS_DDR_DRAMTMG0_WR2PRE(reg, f)                                   \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 29, 24)

#define OFFSET_SNPS_DDR_DRAMTMG1_T_RC 0
#define SNPS_DDR_DRAMTMG1_T_RC(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 5, 0)
#define GET_SNPS_DDR_DRAMTMG1_T_RC(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 5, 0)
#define SET_SNPS_DDR_DRAMTMG1_T_RC(reg, f)                                     \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 5, 0)

#define OFFSET_SNPS_DDR_DRAMTMG1_RD2PRE 8
#define SNPS_DDR_DRAMTMG1_RD2PRE(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 12, 8)
#define GET_SNPS_DDR_DRAMTMG1_RD2PRE(a)                                        \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 12, 8)
#define SET_SNPS_DDR_DRAMTMG1_RD2PRE(reg, f)                                   \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 12, 8)

#define OFFSET_SNPS_DDR_DRAMTMG1_T_XP 16
#define SNPS_DDR_DRAMTMG1_T_XP(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 20, 16)
#define GET_SNPS_DDR_DRAMTMG1_T_XP(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 20, 16)
#define SET_SNPS_DDR_DRAMTMG1_T_XP(reg, f)                                     \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 20, 16)

#define OFFSET_SNPS_DDR_DRAMTMG2_WR2RD 0
#define SNPS_DDR_DRAMTMG2_WR2RD(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 5, 0)
#define GET_SNPS_DDR_DRAMTMG2_WR2RD(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 5, 0)
#define SET_SNPS_DDR_DRAMTMG2_WR2RD(reg, f)                                    \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 5, 0)

#define OFFSET_SNPS_DDR_DRAMTMG2_RD2WR 8
#define SNPS_DDR_DRAMTMG2_RD2WR(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 12, 8)
#define GET_SNPS_DDR_DRAMTMG2_RD2WR(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 12, 8)
#define SET_SNPS_DDR_DRAMTMG2_RD2WR(reg, f)                                    \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 12, 8)

#define OFFSET_SNPS_DDR_DRAMTMG2_READ_LATENCY 16
#define SNPS_DDR_DRAMTMG2_READ_LATENCY(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 21, 16)
#define GET_SNPS_DDR_DRAMTMG2_READ_LATENCY(a)                                  \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 21, 16)
#define SET_SNPS_DDR_DRAMTMG2_READ_LATENCY(reg, f)                             \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 21, 16)

#define OFFSET_SNPS_DDR_DRAMTMG2_WRITE_LATENCY 24
#define SNPS_DDR_DRAMTMG2_WRITE_LATENCY(a)                                     \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 29, 24)
#define GET_SNPS_DDR_DRAMTMG2_WRITE_LATENCY(a)                                 \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 29, 24)
#define SET_SNPS_DDR_DRAMTMG2_WRITE_LATENCY(reg, f)                            \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 29, 24)

#define OFFSET_SNPS_DDR_DRAMTMG3_T_MOD 0
#define SNPS_DDR_DRAMTMG3_T_MOD(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 9, 0)
#define GET_SNPS_DDR_DRAMTMG3_T_MOD(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 9, 0)
#define SET_SNPS_DDR_DRAMTMG3_T_MOD(reg, f)                                    \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 9, 0)

#define OFFSET_SNPS_DDR_DRAMTMG3_T_MRD 12
#define SNPS_DDR_DRAMTMG3_T_MRD(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 17, 12)
#define GET_SNPS_DDR_DRAMTMG3_T_MRD(a)                                         \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 17, 12)
#define SET_SNPS_DDR_DRAMTMG3_T_MRD(reg, f)                                    \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 17, 12)

#define OFFSET_SNPS_DDR_DRAMTMG3_T_MRW 20
#define SNPS_DDR_DRAMTMG3_T_MRW(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 29, 20)
#define GET_SNPS_DDR_DRAMTMG3_T_MRW(a)                                         \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 29, 20)
#define SET_SNPS_DDR_DRAMTMG3_T_MRW(reg, f)                                    \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 29, 20)

#define OFFSET_SNPS_DDR_DRAMTMG4_T_RP 0
#define SNPS_DDR_DRAMTMG4_T_RP(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 4, 0)
#define GET_SNPS_DDR_DRAMTMG4_T_RP(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 4, 0)
#define SET_SNPS_DDR_DRAMTMG4_T_RP(reg, f)                                     \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 4, 0)

#define OFFSET_SNPS_DDR_DRAMTMG4_T_RRD 8
#define SNPS_DDR_DRAMTMG4_T_RRD(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 11, 8)
#define GET_SNPS_DDR_DRAMTMG4_T_RRD(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 11, 8)
#define SET_SNPS_DDR_DRAMTMG4_T_RRD(reg, f)                                    \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 11, 8)

#define OFFSET_SNPS_DDR_DRAMTMG4_T_CCD 16
#define SNPS_DDR_DRAMTMG4_T_CCD(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 18, 16)
#define GET_SNPS_DDR_DRAMTMG4_T_CCD(a)                                         \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 18, 16)
#define SET_SNPS_DDR_DRAMTMG4_T_CCD(reg, f)                                    \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 18, 16)

#define OFFSET_SNPS_DDR_DRAMTMG4_T_RCD 24
#define SNPS_DDR_DRAMTMG4_T_RCD(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 28, 24)
#define GET_SNPS_DDR_DRAMTMG4_T_RCD(a)                                         \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 28, 24)
#define SET_SNPS_DDR_DRAMTMG4_T_RCD(reg, f)                                    \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 28, 24)

#define OFFSET_SNPS_DDR_ZQCTL0_T_ZQ_SHORT_NOP 0
#define SNPS_DDR_ZQCTL0_T_ZQ_SHORT_NOP(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 9, 0)
#define GET_SNPS_DDR_ZQCTL0_T_ZQ_SHORT_NOP(a)                                  \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 9, 0)
#define SET_SNPS_DDR_ZQCTL0_T_ZQ_SHORT_NOP(reg, f)                             \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 9, 0)

#define OFFSET_SNPS_DDR_ZQCTL0_T_ZQ_LONG_NOP 16
#define SNPS_DDR_ZQCTL0_T_ZQ_LONG_NOP(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 25, 16)
#define GET_SNPS_DDR_ZQCTL0_T_ZQ_LONG_NOP(a)                                   \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 25, 16)
#define SET_SNPS_DDR_ZQCTL0_T_ZQ_LONG_NOP(reg, f)                              \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 25, 16)

#define OFFSET_SNPS_DDR_ZQCTL0_DIS_MPSMX_ZQCL 28
#define SNPS_DDR_ZQCTL0_DIS_MPSMX_ZQCL(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 28, 28)
#define GET_SNPS_DDR_ZQCTL0_DIS_MPSMX_ZQCL(a)                                  \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 28, 28)
#define SET_SNPS_DDR_ZQCTL0_DIS_MPSMX_ZQCL(reg, f)                             \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 28, 28)

#define OFFSET_SNPS_DDR_ZQCTL0_ZQ_RESISTOR_SHARED 29
#define SNPS_DDR_ZQCTL0_ZQ_RESISTOR_SHARED(a)                                  \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 29, 29)
#define GET_SNPS_DDR_ZQCTL0_ZQ_RESISTOR_SHARED(a)                              \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 29, 29)
#define SET_SNPS_DDR_ZQCTL0_ZQ_RESISTOR_SHARED(reg, f)                         \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 29, 29)

#define OFFSET_SNPS_DDR_ZQCTL0_DIS_SRX_ZQCL 30
#define SNPS_DDR_ZQCTL0_DIS_SRX_ZQCL(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 30, 30)
#define GET_SNPS_DDR_ZQCTL0_DIS_SRX_ZQCL(a)                                    \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 30, 30)
#define SET_SNPS_DDR_ZQCTL0_DIS_SRX_ZQCL(reg, f)                               \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 30, 30)

#define OFFSET_SNPS_DDR_ZQCTL0_DIS_AUTO_ZQ 31
#define SNPS_DDR_ZQCTL0_DIS_AUTO_ZQ(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 31, 31)
#define GET_SNPS_DDR_ZQCTL0_DIS_AUTO_ZQ(a)                                     \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 31, 31)
#define SET_SNPS_DDR_ZQCTL0_DIS_AUTO_ZQ(reg, f)                                \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 31, 31)

#define OFFSET_SNPS_DDR_DFITMG0_DFI_TPHY_WRLAT 0
#define SNPS_DDR_DFITMG0_DFI_TPHY_WRLAT(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 5, 0)
#define GET_SNPS_DDR_DFITMG0_DFI_TPHY_WRLAT(a)                                 \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 5, 0)
#define SET_SNPS_DDR_DFITMG0_DFI_TPHY_WRLAT(reg, f)                            \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 5, 0)

#define OFFSET_SNPS_DDR_DFITMG0_DFI_TPHY_WRDATA 8
#define SNPS_DDR_DFITMG0_DFI_TPHY_WRDATA(a)                                    \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 13, 8)
#define GET_SNPS_DDR_DFITMG0_DFI_TPHY_WRDATA(a)                                \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 13, 8)
#define SET_SNPS_DDR_DFITMG0_DFI_TPHY_WRDATA(reg, f)                           \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 13, 8)

#define OFFSET_SNPS_DDR_DFITMG0_DFI_WRDATA_USE_SDR 15
#define SNPS_DDR_DFITMG0_DFI_WRDATA_USE_SDR(a)                                 \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 15, 15)
#define GET_SNPS_DDR_DFITMG0_DFI_WRDATA_USE_SDR(a)                             \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 15, 15)
#define SET_SNPS_DDR_DFITMG0_DFI_WRDATA_USE_SDR(reg, f)                        \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 15, 15)

#define OFFSET_SNPS_DDR_DFITMG0_DFI_T_RDDATA_EN 16
#define SNPS_DDR_DFITMG0_DFI_T_RDDATA_EN(a)                                    \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 21, 16)
#define GET_SNPS_DDR_DFITMG0_DFI_T_RDDATA_EN(a)                                \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 21, 16)
#define SET_SNPS_DDR_DFITMG0_DFI_T_RDDATA_EN(reg, f)                           \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 21, 16)

#define OFFSET_SNPS_DDR_DFITMG0_DFI_RDDATA_USE_SDR 23
#define SNPS_DDR_DFITMG0_DFI_RDDATA_USE_SDR(a)                                 \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 23, 23)
#define GET_SNPS_DDR_DFITMG0_DFI_RDDATA_USE_SDR(a)                             \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 23, 23)
#define SET_SNPS_DDR_DFITMG0_DFI_RDDATA_USE_SDR(reg, f)                        \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 23, 23)

#define OFFSET_SNPS_DDR_DFITMG0_DFI_T_CTRL_DELAY 24
#define SNPS_DDR_DFITMG0_DFI_T_CTRL_DELAY(a)                                   \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 28, 24)
#define GET_SNPS_DDR_DFITMG0_DFI_T_CTRL_DELAY(a)                               \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 28, 24)
#define SET_SNPS_DDR_DFITMG0_DFI_T_CTRL_DELAY(reg, f)                          \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 28, 24)

#define OFFSET_SNPS_DDR_DFITMG1_DFI_T_DRAM_CLK_ENABLE 0
#define SNPS_DDR_DFITMG1_DFI_T_DRAM_CLK_ENABLE(a)                              \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define GET_SNPS_DDR_DFITMG1_DFI_T_DRAM_CLK_ENABLE(a)                          \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define SET_SNPS_DDR_DFITMG1_DFI_T_DRAM_CLK_ENABLE(reg, f)                     \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 3, 0)

#define OFFSET_SNPS_DDR_DFITMG1_DFI_T_DRAM_CLK_DISABLE 8
#define SNPS_DDR_DFITMG1_DFI_T_DRAM_CLK_DISABLE(a)                             \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 11, 8)
#define GET_SNPS_DDR_DFITMG1_DFI_T_DRAM_CLK_DISABLE(a)                         \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 11, 8)
#define SET_SNPS_DDR_DFITMG1_DFI_T_DRAM_CLK_DISABLE(reg, f)                    \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 11, 8)

#define OFFSET_SNPS_DDR_DFITMG1_DFI_T_WRDATA_DELAY 16
#define SNPS_DDR_DFITMG1_DFI_T_WRDATA_DELAY(a)                                 \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 20, 16)
#define GET_SNPS_DDR_DFITMG1_DFI_T_WRDATA_DELAY(a)                             \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 20, 16)
#define SET_SNPS_DDR_DFITMG1_DFI_T_WRDATA_DELAY(reg, f)                        \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 20, 16)

#define OFFSET_SNPS_DDR_DFITMG1_DFI_T_PARIN_LAT 24
#define SNPS_DDR_DFITMG1_DFI_T_PARIN_LAT(a)                                    \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 25, 24)
#define GET_SNPS_DDR_DFITMG1_DFI_T_PARIN_LAT(a)                                \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 25, 24)
#define SET_SNPS_DDR_DFITMG1_DFI_T_PARIN_LAT(reg, f)                           \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 25, 24)

#define OFFSET_SNPS_DDR_DFIUPD0_DFI_T_CTRLUP_MIN 0
#define SNPS_DDR_DFIUPD0_DFI_T_CTRLUP_MIN(a)                                   \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 9, 0)
#define GET_SNPS_DDR_DFIUPD0_DFI_T_CTRLUP_MIN(a)                               \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 9, 0)
#define SET_SNPS_DDR_DFIUPD0_DFI_T_CTRLUP_MIN(reg, f)                          \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 9, 0)

#define OFFSET_SNPS_DDR_DFIUPD0_DFI_T_CTRLUP_MAX 16
#define SNPS_DDR_DFIUPD0_DFI_T_CTRLUP_MAX(a)                                   \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 25, 16)
#define GET_SNPS_DDR_DFIUPD0_DFI_T_CTRLUP_MAX(a)                               \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 25, 16)
#define SET_SNPS_DDR_DFIUPD0_DFI_T_CTRLUP_MAX(reg, f)                          \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 25, 16)

#define OFFSET_SNPS_DDR_DFIUPD0_DIS_AUTO_CTRLUPD 31
#define SNPS_DDR_DFIUPD0_DIS_AUTO_CTRLUPD(a)                                   \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 31, 31)
#define GET_SNPS_DDR_DFIUPD0_DIS_AUTO_CTRLUPD(a)                               \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 31, 31)
#define SET_SNPS_DDR_DFIUPD0_DIS_AUTO_CTRLUPD(reg, f)                          \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 31, 31)

#define OFFSET_SNPS_DDR_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MAX 0
#define SNPS_DDR_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MAX(a)                         \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 7, 0)
#define GET_SNPS_DDR_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MAX(a)                     \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 7, 0)
#define SET_SNPS_DDR_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MAX(reg, f)                \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 7, 0)

#define OFFSET_SNPS_DDR_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MIN 16
#define SNPS_DDR_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MIN(a)                         \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 23, 16)
#define GET_SNPS_DDR_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MIN(a)                     \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 23, 16)
#define SET_SNPS_DDR_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MIN(reg, f)                \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 23, 16)

#define OFFSET_SNPS_DDR_DFIMISC_DFI_INIT_COMPLETE_EN 0
#define SNPS_DDR_DFIMISC_DFI_INIT_COMPLETE_EN(a)                               \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 0, 0)
#define GET_SNPS_DDR_DFIMISC_DFI_INIT_COMPLETE_EN(a)                           \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 0, 0)
#define SET_SNPS_DDR_DFIMISC_DFI_INIT_COMPLETE_EN(reg, f)                      \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 0, 0)

#define OFFSET_SNPS_DDR_ADDRMAP0_ADDRMAP_CS_BIT0 0
#define SNPS_DDR_ADDRMAP0_ADDRMAP_CS_BIT0(a)                                   \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 4, 0)
#define GET_SNPS_DDR_ADDRMAP0_ADDRMAP_CS_BIT0(a)                               \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 4, 0)
#define SET_SNPS_DDR_ADDRMAP0_ADDRMAP_CS_BIT0(reg, f)                          \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 4, 0)

#define OFFSET_SNPS_DDR_ADDRMAP0_ADDRMAP_CS_BIT1 8
#define SNPS_DDR_ADDRMAP0_ADDRMAP_CS_BIT1(a)                                   \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 12, 8)
#define GET_SNPS_DDR_ADDRMAP0_ADDRMAP_CS_BIT1(a)                               \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 12, 8)
#define SET_SNPS_DDR_ADDRMAP0_ADDRMAP_CS_BIT1(reg, f)                          \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 12, 8)

#define OFFSET_SNPS_DDR_ADDRMAP1_ADDRMAP_BANK_B0 0
#define SNPS_DDR_ADDRMAP1_ADDRMAP_BANK_B0(a)                                   \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define GET_SNPS_DDR_ADDRMAP1_ADDRMAP_BANK_B0(a)                               \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define SET_SNPS_DDR_ADDRMAP1_ADDRMAP_BANK_B0(reg, f)                          \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 3, 0)

#define OFFSET_SNPS_DDR_ADDRMAP1_ADDRMAP_BANK_B1 8
#define SNPS_DDR_ADDRMAP1_ADDRMAP_BANK_B1(a)                                   \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 11, 8)
#define GET_SNPS_DDR_ADDRMAP1_ADDRMAP_BANK_B1(a)                               \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 11, 8)
#define SET_SNPS_DDR_ADDRMAP1_ADDRMAP_BANK_B1(reg, f)                          \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 11, 8)

#define OFFSET_SNPS_DDR_ADDRMAP1_ADDRMAP_BANK_B2 16
#define SNPS_DDR_ADDRMAP1_ADDRMAP_BANK_B2(a)                                   \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 19, 16)
#define GET_SNPS_DDR_ADDRMAP1_ADDRMAP_BANK_B2(a)                               \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 19, 16)
#define SET_SNPS_DDR_ADDRMAP1_ADDRMAP_BANK_B2(reg, f)                          \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 19, 16)

#define OFFSET_SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B2 0
#define SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B2(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define GET_SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B2(a)                                \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define SET_SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B2(reg, f)                           \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 3, 0)

#define OFFSET_SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B3 8
#define SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B3(a)                                    \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 11, 8)
#define GET_SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B3(a)                                \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 11, 8)
#define SET_SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B3(reg, f)                           \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 11, 8)

#define OFFSET_SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B4 16
#define SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B4(a)                                    \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 19, 16)
#define GET_SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B4(a)                                \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 19, 16)
#define SET_SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B4(reg, f)                           \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 19, 16)

#define OFFSET_SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B5 24
#define SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B5(a)                                    \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 27, 24)
#define GET_SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B5(a)                                \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 27, 24)
#define SET_SNPS_DDR_ADDRMAP2_ADDRMAP_COL_B5(reg, f)                           \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 27, 24)

#define OFFSET_SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B6 0
#define SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B6(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define GET_SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B6(a)                                \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define SET_SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B6(reg, f)                           \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 3, 0)

#define OFFSET_SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B7 8
#define SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B7(a)                                    \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 11, 8)
#define GET_SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B7(a)                                \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 11, 8)
#define SET_SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B7(reg, f)                           \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 11, 8)

#define OFFSET_SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B8 16
#define SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B8(a)                                    \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 19, 16)
#define GET_SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B8(a)                                \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 19, 16)
#define SET_SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B8(reg, f)                           \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 19, 16)

#define OFFSET_SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B9 24
#define SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B9(a)                                    \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 27, 24)
#define GET_SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B9(a)                                \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 27, 24)
#define SET_SNPS_DDR_ADDRMAP3_ADDRMAP_COL_B9(reg, f)                           \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 27, 24)

#define OFFSET_SNPS_DDR_ADDRMAP4_ADDRMAP_COL_B10 0
#define SNPS_DDR_ADDRMAP4_ADDRMAP_COL_B10(a)                                   \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define GET_SNPS_DDR_ADDRMAP4_ADDRMAP_COL_B10(a)                               \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define SET_SNPS_DDR_ADDRMAP4_ADDRMAP_COL_B10(reg, f)                          \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 3, 0)

#define OFFSET_SNPS_DDR_ADDRMAP4_ADDRMAP_COL_B11 8
#define SNPS_DDR_ADDRMAP4_ADDRMAP_COL_B11(a)                                   \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 11, 8)
#define GET_SNPS_DDR_ADDRMAP4_ADDRMAP_COL_B11(a)                               \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 11, 8)
#define SET_SNPS_DDR_ADDRMAP4_ADDRMAP_COL_B11(reg, f)                          \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 11, 8)

#define OFFSET_SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B0 0
#define SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B0(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define GET_SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B0(a)                                \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define SET_SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B0(reg, f)                           \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 3, 0)

#define OFFSET_SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B1 8
#define SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B1(a)                                    \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 11, 8)
#define GET_SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B1(a)                                \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 11, 8)
#define SET_SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B1(reg, f)                           \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 11, 8)

#define OFFSET_SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B2_10 16
#define SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B2_10(a)                                 \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 19, 16)
#define GET_SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B2_10(a)                             \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 19, 16)
#define SET_SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B2_10(reg, f)                        \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 19, 16)

#define OFFSET_SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B11 24
#define SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B11(a)                                   \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 27, 24)
#define GET_SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B11(a)                               \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 27, 24)
#define SET_SNPS_DDR_ADDRMAP5_ADDRMAP_ROW_B11(reg, f)                          \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 27, 24)

#define OFFSET_SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B12 0
#define SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B12(a)                                   \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define GET_SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B12(a)                               \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 3, 0)
#define SET_SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B12(reg, f)                          \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 3, 0)

#define OFFSET_SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B13 8
#define SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B13(a)                                   \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 11, 8)
#define GET_SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B13(a)                               \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 11, 8)
#define SET_SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B13(reg, f)                          \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 11, 8)

#define OFFSET_SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B14 16
#define SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B14(a)                                   \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 19, 16)
#define GET_SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B14(a)                               \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 19, 16)
#define SET_SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B14(reg, f)                          \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 19, 16)

#define OFFSET_SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B15 24
#define SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B15(a)                                   \
    SNPS_DDR_REG_FIELDS_H_FIELD(a, 27, 24)
#define GET_SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B15(a)                               \
    GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 27, 24)
#define SET_SNPS_DDR_ADDRMAP6_ADDRMAP_ROW_B15(reg, f)                          \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 27, 24)

#define OFFSET_SNPS_DDR_PCTRL_0_PORT_EN 0
#define SNPS_DDR_PCTRL_0_PORT_EN(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 0, 0)
#define GET_SNPS_DDR_PCTRL_0_PORT_EN(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 0, 0)
#define SET_SNPS_DDR_PCTRL_0_PORT_EN(reg, f)                                   \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 0, 0)

#define OFFSET_SNPS_DDR_PCTRL1_PORT_EN 0
#define SNPS_DDR_PCTRL1_PORT_EN(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 0, 0)
#define GET_SNPS_DDR_PCTRL1_PORT_EN(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 0, 0)
#define SET_SNPS_DDR_PCTRL1_PORT_EN(reg, f)                                    \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 0, 0)

#define OFFSET_SNPS_DDR_PCTRL2_PORT_EN 0
#define SNPS_DDR_PCTRL2_PORT_EN(a) SNPS_DDR_REG_FIELDS_H_FIELD(a, 0, 0)
#define GET_SNPS_DDR_PCTRL2_PORT_EN(a) GET_SNPS_DDR_REG_FIELDS_H_FIELD(a, 0, 0)
#define SET_SNPS_DDR_PCTRL2_PORT_EN(reg, f)                                    \
    SET_SNPS_DDR_REG_FIELDS_H_FIELD(reg, f, 0, 0)

//------------------------------------------------------------------------------------------------------------

#define MASK_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(max_, min_)                       \
    ((1 << (max_ - min_ + 1)) - 1)
#define SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, max_, min_)                         \
    (((a)&MASK_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(max_, min_)) << min_)
#define GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, max_, min_)                     \
    (((a) >> min_) & MASK_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(max_, min_))
#define SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, max_, min_)                \
    (((reg) & (~(MASK_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(max_, min_) << min_))) | \
     SNPS_DDR_PUB_REG_FIELDS_H_FIELD(f, max_, min_))

#define OFFSET_SNPS_DDR_PUB_RIDR_PUBMNR 0
#define SNPS_DDR_PUB_RIDR_PUBMNR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 0)
#define GET_SNPS_DDR_PUB_RIDR_PUBMNR(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 0)
#define SET_SNPS_DDR_PUB_RIDR_PUBMNR(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 3, 0)

#define OFFSET_SNPS_DDR_PUB_RIDR_PUBMDR 4
#define SNPS_DDR_PUB_RIDR_PUBMDR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 4)
#define GET_SNPS_DDR_PUB_RIDR_PUBMDR(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 4)
#define SET_SNPS_DDR_PUB_RIDR_PUBMDR(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 7, 4)

#define OFFSET_SNPS_DDR_PUB_RIDR_PUBMJR 8
#define SNPS_DDR_PUB_RIDR_PUBMJR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 8)
#define GET_SNPS_DDR_PUB_RIDR_PUBMJR(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 8)
#define SET_SNPS_DDR_PUB_RIDR_PUBMJR(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 11, 8)

#define OFFSET_SNPS_DDR_PUB_RIDR_PHYMNR 12
#define SNPS_DDR_PUB_RIDR_PHYMNR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 12)
#define GET_SNPS_DDR_PUB_RIDR_PHYMNR(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 12)
#define SET_SNPS_DDR_PUB_RIDR_PHYMNR(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 15, 12)

#define OFFSET_SNPS_DDR_PUB_RIDR_PHYMDR 16
#define SNPS_DDR_PUB_RIDR_PHYMDR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 19, 16)
#define GET_SNPS_DDR_PUB_RIDR_PHYMDR(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 19, 16)
#define SET_SNPS_DDR_PUB_RIDR_PHYMDR(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 19, 16)

#define OFFSET_SNPS_DDR_PUB_RIDR_PHYMJR 20
#define SNPS_DDR_PUB_RIDR_PHYMJR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 23, 20)
#define GET_SNPS_DDR_PUB_RIDR_PHYMJR(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 23, 20)
#define SET_SNPS_DDR_PUB_RIDR_PHYMJR(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 23, 20)

#define OFFSET_SNPS_DDR_PUB_RIDR_UDRID 24
#define SNPS_DDR_PUB_RIDR_UDRID(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 24)
#define GET_SNPS_DDR_PUB_RIDR_UDRID(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 24)
#define SET_SNPS_DDR_PUB_RIDR_UDRID(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 31, 24)

#define OFFSET_SNPS_DDR_PUB_PIR_INIT 0
#define SNPS_DDR_PUB_PIR_INIT(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 0, 0)
#define GET_SNPS_DDR_PUB_PIR_INIT(a)                                           \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 0, 0)
#define SET_SNPS_DDR_PUB_PIR_INIT(reg, f)                                      \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 0, 0)

#define OFFSET_SNPS_DDR_PUB_PIR_DLLSRST 1
#define SNPS_DDR_PUB_PIR_DLLSRST(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 1)
#define GET_SNPS_DDR_PUB_PIR_DLLSRST(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 1)
#define SET_SNPS_DDR_PUB_PIR_DLLSRST(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 1, 1)

#define OFFSET_SNPS_DDR_PUB_PIR_DLLLOCK 2
#define SNPS_DDR_PUB_PIR_DLLLOCK(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 2)
#define GET_SNPS_DDR_PUB_PIR_DLLLOCK(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 2)
#define SET_SNPS_DDR_PUB_PIR_DLLLOCK(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 2, 2)

#define OFFSET_SNPS_DDR_PUB_PIR_ZCAL 3
#define SNPS_DDR_PUB_PIR_ZCAL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 3)
#define GET_SNPS_DDR_PUB_PIR_ZCAL(a)                                           \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 3)
#define SET_SNPS_DDR_PUB_PIR_ZCAL(reg, f)                                      \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 3, 3)

#define OFFSET_SNPS_DDR_PUB_PIR_ITMSRST 4
#define SNPS_DDR_PUB_PIR_ITMSRST(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 4, 4)
#define GET_SNPS_DDR_PUB_PIR_ITMSRST(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 4, 4)
#define SET_SNPS_DDR_PUB_PIR_ITMSRST(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 4, 4)

#define OFFSET_SNPS_DDR_PUB_PIR_DRAMRST 5
#define SNPS_DDR_PUB_PIR_DRAMRST(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 5, 5)
#define GET_SNPS_DDR_PUB_PIR_DRAMRST(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 5, 5)
#define SET_SNPS_DDR_PUB_PIR_DRAMRST(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 5, 5)

#define OFFSET_SNPS_DDR_PUB_PIR_DRAMINIT 6
#define SNPS_DDR_PUB_PIR_DRAMINIT(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 6, 6)
#define GET_SNPS_DDR_PUB_PIR_DRAMINIT(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 6, 6)
#define SET_SNPS_DDR_PUB_PIR_DRAMINIT(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 6, 6)

#define OFFSET_SNPS_DDR_PUB_PIR_QSTRN 7
#define SNPS_DDR_PUB_PIR_QSTRN(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 7)
#define GET_SNPS_DDR_PUB_PIR_QSTRN(a)                                          \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 7)
#define SET_SNPS_DDR_PUB_PIR_QSTRN(reg, f)                                     \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 7, 7)

#define OFFSET_SNPS_DDR_PUB_PIR_RVTRN 8
#define SNPS_DDR_PUB_PIR_RVTRN(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 8, 8)
#define GET_SNPS_DDR_PUB_PIR_RVTRN(a)                                          \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 8, 8)
#define SET_SNPS_DDR_PUB_PIR_RVTRN(reg, f)                                     \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 8, 8)

#define OFFSET_SNPS_DDR_PUB_PIR_ICPC 16
#define SNPS_DDR_PUB_PIR_ICPC(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 16, 16)
#define GET_SNPS_DDR_PUB_PIR_ICPC(a)                                           \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 16, 16)
#define SET_SNPS_DDR_PUB_PIR_ICPC(reg, f)                                      \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 16, 16)

#define OFFSET_SNPS_DDR_PUB_PIR_DLLBYP 17
#define SNPS_DDR_PUB_PIR_DLLBYP(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 17, 17)
#define GET_SNPS_DDR_PUB_PIR_DLLBYP(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 17, 17)
#define SET_SNPS_DDR_PUB_PIR_DLLBYP(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 17, 17)

#define OFFSET_SNPS_DDR_PUB_PIR_CTLDINIT 18
#define SNPS_DDR_PUB_PIR_CTLDINIT(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 18, 18)
#define GET_SNPS_DDR_PUB_PIR_CTLDINIT(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 18, 18)
#define SET_SNPS_DDR_PUB_PIR_CTLDINIT(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 18, 18)

#define OFFSET_SNPS_DDR_PUB_PIR_CLRSR 28
#define SNPS_DDR_PUB_PIR_CLRSR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 28, 28)
#define GET_SNPS_DDR_PUB_PIR_CLRSR(a)                                          \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 28, 28)
#define SET_SNPS_DDR_PUB_PIR_CLRSR(reg, f)                                     \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 28, 28)

#define OFFSET_SNPS_DDR_PUB_PIR_LOCKBYP 29
#define SNPS_DDR_PUB_PIR_LOCKBYP(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 29, 29)
#define GET_SNPS_DDR_PUB_PIR_LOCKBYP(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 29, 29)
#define SET_SNPS_DDR_PUB_PIR_LOCKBYP(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 29, 29)

#define OFFSET_SNPS_DDR_PUB_PIR_ZCALBYP 30
#define SNPS_DDR_PUB_PIR_ZCALBYP(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 30, 30)
#define GET_SNPS_DDR_PUB_PIR_ZCALBYP(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 30, 30)
#define SET_SNPS_DDR_PUB_PIR_ZCALBYP(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 30, 30)

#define OFFSET_SNPS_DDR_PUB_PIR_INITBYP 31
#define SNPS_DDR_PUB_PIR_INITBYP(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 31)
#define GET_SNPS_DDR_PUB_PIR_INITBYP(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 31)
#define SET_SNPS_DDR_PUB_PIR_INITBYP(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 31, 31)

#define OFFSET_SNPS_DDR_PUB_PGCR_ITMDMD 0
#define SNPS_DDR_PUB_PGCR_ITMDMD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 0, 0)
#define GET_SNPS_DDR_PUB_PGCR_ITMDMD(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 0, 0)
#define SET_SNPS_DDR_PUB_PGCR_ITMDMD(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 0, 0)

#define OFFSET_SNPS_DDR_PUB_PGCR_DQSCFG 1
#define SNPS_DDR_PUB_PGCR_DQSCFG(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 1)
#define GET_SNPS_DDR_PUB_PGCR_DQSCFG(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 1)
#define SET_SNPS_DDR_PUB_PGCR_DQSCFG(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 1, 1)

#define OFFSET_SNPS_DDR_PUB_PGCR_DFTCMP 2
#define SNPS_DDR_PUB_PGCR_DFTCMP(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 2)
#define GET_SNPS_DDR_PUB_PGCR_DFTCMP(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 2)
#define SET_SNPS_DDR_PUB_PGCR_DFTCMP(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 2, 2)

#define OFFSET_SNPS_DDR_PUB_PGCR_DFTLMT 3
#define SNPS_DDR_PUB_PGCR_DFTLMT(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 4, 3)
#define GET_SNPS_DDR_PUB_PGCR_DFTLMT(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 4, 3)
#define SET_SNPS_DDR_PUB_PGCR_DFTLMT(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 4, 3)

#define OFFSET_SNPS_DDR_PUB_PGCR_DTOSEL 5
#define SNPS_DDR_PUB_PGCR_DTOSEL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 8, 5)
#define GET_SNPS_DDR_PUB_PGCR_DTOSEL(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 8, 5)
#define SET_SNPS_DDR_PUB_PGCR_DTOSEL(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 8, 5)

#define OFFSET_SNPS_DDR_PUB_PGCR_CKEN 9
#define SNPS_DDR_PUB_PGCR_CKEN(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 9)
#define GET_SNPS_DDR_PUB_PGCR_CKEN(a)                                          \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 9)
#define SET_SNPS_DDR_PUB_PGCR_CKEN(reg, f)                                     \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 11, 9)

#define OFFSET_SNPS_DDR_PUB_PGCR_CKDV 12
#define SNPS_DDR_PUB_PGCR_CKDV(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 13, 12)
#define GET_SNPS_DDR_PUB_PGCR_CKDV(a)                                          \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 13, 12)
#define SET_SNPS_DDR_PUB_PGCR_CKDV(reg, f)                                     \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 13, 12)

#define OFFSET_SNPS_DDR_PUB_PGCR_CKINV 14
#define SNPS_DDR_PUB_PGCR_CKINV(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 14, 14)
#define GET_SNPS_DDR_PUB_PGCR_CKINV(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 14, 14)
#define SET_SNPS_DDR_PUB_PGCR_CKINV(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 14, 14)

#define OFFSET_SNPS_DDR_PUB_PGCR_IOLB 15
#define SNPS_DDR_PUB_PGCR_IOLB(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 15)
#define GET_SNPS_DDR_PUB_PGCR_IOLB(a)                                          \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 15)
#define SET_SNPS_DDR_PUB_PGCR_IOLB(reg, f)                                     \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 15, 15)

#define OFFSET_SNPS_DDR_PUB_PGCR_IODDRM 16
#define SNPS_DDR_PUB_PGCR_IODDRM(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 17, 16)
#define GET_SNPS_DDR_PUB_PGCR_IODDRM(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 17, 16)
#define SET_SNPS_DDR_PUB_PGCR_IODDRM(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 17, 16)

#define OFFSET_SNPS_DDR_PUB_PGCR_RANKEN 18
#define SNPS_DDR_PUB_PGCR_RANKEN(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 21, 18)
#define GET_SNPS_DDR_PUB_PGCR_RANKEN(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 21, 18)
#define SET_SNPS_DDR_PUB_PGCR_RANKEN(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 21, 18)

#define OFFSET_SNPS_DDR_PUB_PGCR_ZCKSEL 22
#define SNPS_DDR_PUB_PGCR_ZCKSEL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 23, 22)
#define GET_SNPS_DDR_PUB_PGCR_ZCKSEL(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 23, 22)
#define SET_SNPS_DDR_PUB_PGCR_ZCKSEL(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 23, 22)

#define OFFSET_SNPS_DDR_PUB_PGCR_PDDISDX 24
#define SNPS_DDR_PUB_PGCR_PDDISDX(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 24, 24)
#define GET_SNPS_DDR_PUB_PGCR_PDDISDX(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 24, 24)
#define SET_SNPS_DDR_PUB_PGCR_PDDISDX(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 24, 24)

#define OFFSET_SNPS_DDR_PUB_PGCR_RFSHDT 25
#define SNPS_DDR_PUB_PGCR_RFSHDT(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 28, 25)
#define GET_SNPS_DDR_PUB_PGCR_RFSHDT(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 28, 25)
#define SET_SNPS_DDR_PUB_PGCR_RFSHDT(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 28, 25)

#define OFFSET_SNPS_DDR_PUB_PGCR_LBDQSS 29
#define SNPS_DDR_PUB_PGCR_LBDQSS(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 29, 29)
#define GET_SNPS_DDR_PUB_PGCR_LBDQSS(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 29, 29)
#define SET_SNPS_DDR_PUB_PGCR_LBDQSS(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 29, 29)

#define OFFSET_SNPS_DDR_PUB_PGCR_LBGDQS 30
#define SNPS_DDR_PUB_PGCR_LBGDQS(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 30, 30)
#define GET_SNPS_DDR_PUB_PGCR_LBGDQS(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 30, 30)
#define SET_SNPS_DDR_PUB_PGCR_LBGDQS(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 30, 30)

#define OFFSET_SNPS_DDR_PUB_PGCR_LBMODE 31
#define SNPS_DDR_PUB_PGCR_LBMODE(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 31)
#define GET_SNPS_DDR_PUB_PGCR_LBMODE(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 31)
#define SET_SNPS_DDR_PUB_PGCR_LBMODE(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 31, 31)

#define OFFSET_SNPS_DDR_PUB_PGSR_IDONE 0
#define SNPS_DDR_PUB_PGSR_IDONE(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 0, 0)
#define GET_SNPS_DDR_PUB_PGSR_IDONE(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 0, 0)
#define SET_SNPS_DDR_PUB_PGSR_IDONE(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 0, 0)

#define OFFSET_SNPS_DDR_PUB_PGSR_DLDONE 1
#define SNPS_DDR_PUB_PGSR_DLDONE(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 1)
#define GET_SNPS_DDR_PUB_PGSR_DLDONE(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 1)
#define SET_SNPS_DDR_PUB_PGSR_DLDONE(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 1, 1)

#define OFFSET_SNPS_DDR_PUB_PGSR_ZCDONE 2
#define SNPS_DDR_PUB_PGSR_ZCDONE(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 2)
#define GET_SNPS_DDR_PUB_PGSR_ZCDONE(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 2)
#define SET_SNPS_DDR_PUB_PGSR_ZCDONE(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 2, 2)

#define OFFSET_SNPS_DDR_PUB_PGSR_DIDONE 3
#define SNPS_DDR_PUB_PGSR_DIDONE(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 3)
#define GET_SNPS_DDR_PUB_PGSR_DIDONE(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 3)
#define SET_SNPS_DDR_PUB_PGSR_DIDONE(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 3, 3)

#define OFFSET_SNPS_DDR_PUB_PGSR_DTDONE 4
#define SNPS_DDR_PUB_PGSR_DTDONE(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 4, 4)
#define GET_SNPS_DDR_PUB_PGSR_DTDONE(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 4, 4)
#define SET_SNPS_DDR_PUB_PGSR_DTDONE(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 4, 4)

#define OFFSET_SNPS_DDR_PUB_PGSR_DTERR 5
#define SNPS_DDR_PUB_PGSR_DTERR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 5, 5)
#define GET_SNPS_DDR_PUB_PGSR_DTERR(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 5, 5)
#define SET_SNPS_DDR_PUB_PGSR_DTERR(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 5, 5)

#define OFFSET_SNPS_DDR_PUB_PGSR_DTIERR 6
#define SNPS_DDR_PUB_PGSR_DTIERR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 6, 6)
#define GET_SNPS_DDR_PUB_PGSR_DTIERR(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 6, 6)
#define SET_SNPS_DDR_PUB_PGSR_DTIERR(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 6, 6)

#define OFFSET_SNPS_DDR_PUB_PGSR_DFTERR 7
#define SNPS_DDR_PUB_PGSR_DFTERR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 7)
#define GET_SNPS_DDR_PUB_PGSR_DFTERR(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 7)
#define SET_SNPS_DDR_PUB_PGSR_DFTERR(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 7, 7)

#define OFFSET_SNPS_DDR_PUB_PGSR_RVERR 8
#define SNPS_DDR_PUB_PGSR_RVERR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 8, 8)
#define GET_SNPS_DDR_PUB_PGSR_RVERR(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 8, 8)
#define SET_SNPS_DDR_PUB_PGSR_RVERR(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 8, 8)

#define OFFSET_SNPS_DDR_PUB_PGSR_RVEIRR 9
#define SNPS_DDR_PUB_PGSR_RVEIRR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 9, 9)
#define GET_SNPS_DDR_PUB_PGSR_RVEIRR(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 9, 9)
#define SET_SNPS_DDR_PUB_PGSR_RVEIRR(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 9, 9)

#define OFFSET_SNPS_DDR_PUB_PGSR_TQ 11
#define SNPS_DDR_PUB_PGSR_TQ(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 11)
#define GET_SNPS_DDR_PUB_PGSR_TQ(a)                                            \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 11)
#define SET_SNPS_DDR_PUB_PGSR_TQ(reg, f)                                       \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 11, 11)

#define OFFSET_SNPS_DDR_PUB_PTR0_TDLLSRST 0
#define SNPS_DDR_PUB_PTR0_TDLLSRST(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 5, 0)
#define GET_SNPS_DDR_PUB_PTR0_TDLLSRST(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 5, 0)
#define SET_SNPS_DDR_PUB_PTR0_TDLLSRST(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 5, 0)

#define OFFSET_SNPS_DDR_PUB_PTR0_TDLLLOCK 6
#define SNPS_DDR_PUB_PTR0_TDLLLOCK(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 17, 6)
#define GET_SNPS_DDR_PUB_PTR0_TDLLLOCK(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 17, 6)
#define SET_SNPS_DDR_PUB_PTR0_TDLLLOCK(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 17, 6)

#define OFFSET_SNPS_DDR_PUB_PTR0_TITMSRST 18
#define SNPS_DDR_PUB_PTR0_TITMSRST(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 21, 18)
#define GET_SNPS_DDR_PUB_PTR0_TITMSRST(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 21, 18)
#define SET_SNPS_DDR_PUB_PTR0_TITMSRST(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 21, 18)

#define OFFSET_SNPS_DDR_PUB_PTR1_TDINIT0 0
#define SNPS_DDR_PUB_PTR1_TDINIT0(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 18, 0)
#define GET_SNPS_DDR_PUB_PTR1_TDINIT0(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 18, 0)
#define SET_SNPS_DDR_PUB_PTR1_TDINIT0(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 18, 0)

#define OFFSET_SNPS_DDR_PUB_PTR1_TDINIT1 19
#define SNPS_DDR_PUB_PTR1_TDINIT1(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 26, 19)
#define GET_SNPS_DDR_PUB_PTR1_TDINIT1(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 26, 19)
#define SET_SNPS_DDR_PUB_PTR1_TDINIT1(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 26, 19)

#define OFFSET_SNPS_DDR_PUB_PTR2_TDINIT2 0
#define SNPS_DDR_PUB_PTR2_TDINIT2(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 16, 0)
#define GET_SNPS_DDR_PUB_PTR2_TDINIT2(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 16, 0)
#define SET_SNPS_DDR_PUB_PTR2_TDINIT2(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 16, 0)

#define OFFSET_SNPS_DDR_PUB_PTR2_TDINIT3 17
#define SNPS_DDR_PUB_PTR2_TDINIT3(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 26, 17)
#define GET_SNPS_DDR_PUB_PTR2_TDINIT3(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 26, 17)
#define SET_SNPS_DDR_PUB_PTR2_TDINIT3(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 26, 17)

#define OFFSET_SNPS_DDR_PUB_DXCCR_DXODT 0
#define SNPS_DDR_PUB_DXCCR_DXODT(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 0, 0)
#define GET_SNPS_DDR_PUB_DXCCR_DXODT(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 0, 0)
#define SET_SNPS_DDR_PUB_DXCCR_DXODT(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 0, 0)

#define OFFSET_SNPS_DDR_PUB_DXCCR_DXIOM 1
#define SNPS_DDR_PUB_DXCCR_DXIOM(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 1)
#define GET_SNPS_DDR_PUB_DXCCR_DXIOM(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 1)
#define SET_SNPS_DDR_PUB_DXCCR_DXIOM(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 1, 1)

#define OFFSET_SNPS_DDR_PUB_DXCCR_DXPDD 2
#define SNPS_DDR_PUB_DXCCR_DXPDD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 2)
#define GET_SNPS_DDR_PUB_DXCCR_DXPDD(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 2)
#define SET_SNPS_DDR_PUB_DXCCR_DXPDD(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 2, 2)

#define OFFSET_SNPS_DDR_PUB_DXCCR_DXPDR 3
#define SNPS_DDR_PUB_DXCCR_DXPDR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 3)
#define GET_SNPS_DDR_PUB_DXCCR_DXPDR(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 3)
#define SET_SNPS_DDR_PUB_DXCCR_DXPDR(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 3, 3)

#define OFFSET_SNPS_DDR_PUB_DXCCR_DQSRES 4
#define SNPS_DDR_PUB_DXCCR_DQSRES(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 4)
#define GET_SNPS_DDR_PUB_DXCCR_DQSRES(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 4)
#define SET_SNPS_DDR_PUB_DXCCR_DQSRES(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 7, 4)

#define OFFSET_SNPS_DDR_PUB_DXCCR_DQSNRES 8
#define SNPS_DDR_PUB_DXCCR_DQSNRES(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 8)
#define GET_SNPS_DDR_PUB_DXCCR_DQSNRES(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 8)
#define SET_SNPS_DDR_PUB_DXCCR_DQSNRES(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 11, 8)

#define OFFSET_SNPS_DDR_PUB_DXCCR_DXSR 12
#define SNPS_DDR_PUB_DXCCR_DXSR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 13, 12)
#define GET_SNPS_DDR_PUB_DXCCR_DXSR(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 13, 12)
#define SET_SNPS_DDR_PUB_DXCCR_DXSR(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 13, 12)

#define OFFSET_SNPS_DDR_PUB_DXCCR_DQSNRST 14
#define SNPS_DDR_PUB_DXCCR_DQSNRST(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 14, 14)
#define GET_SNPS_DDR_PUB_DXCCR_DQSNRST(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 14, 14)
#define SET_SNPS_DDR_PUB_DXCCR_DQSNRST(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 14, 14)

#define OFFSET_SNPS_DDR_PUB_DXCCR_RVSEL 15
#define SNPS_DDR_PUB_DXCCR_RVSEL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 15)
#define GET_SNPS_DDR_PUB_DXCCR_RVSEL(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 15)
#define SET_SNPS_DDR_PUB_DXCCR_RVSEL(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 15, 15)

#define OFFSET_SNPS_DDR_PUB_DXCCR_AWDT 16
#define SNPS_DDR_PUB_DXCCR_AWDT(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 16, 16)
#define GET_SNPS_DDR_PUB_DXCCR_AWDT(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 16, 16)
#define SET_SNPS_DDR_PUB_DXCCR_AWDT(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 16, 16)

#define OFFSET_SNPS_DDR_PUB_DSGCR_PUREN 0
#define SNPS_DDR_PUB_DSGCR_PUREN(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 0, 0)
#define GET_SNPS_DDR_PUB_DSGCR_PUREN(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 0, 0)
#define SET_SNPS_DDR_PUB_DSGCR_PUREN(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 0, 0)

#define OFFSET_SNPS_DDR_PUB_DSGCR_BDISEN 1
#define SNPS_DDR_PUB_DSGCR_BDISEN(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 1)
#define GET_SNPS_DDR_PUB_DSGCR_BDISEN(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 1)
#define SET_SNPS_DDR_PUB_DSGCR_BDISEN(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 1, 1)

#define OFFSET_SNPS_DDR_PUB_DSGCR_ZUEN 2
#define SNPS_DDR_PUB_DSGCR_ZUEN(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 2)
#define GET_SNPS_DDR_PUB_DSGCR_ZUEN(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 2)
#define SET_SNPS_DDR_PUB_DSGCR_ZUEN(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 2, 2)

#define OFFSET_SNPS_DDR_PUB_DSGCR_LPIOPD 3
#define SNPS_DDR_PUB_DSGCR_LPIOPD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 3)
#define GET_SNPS_DDR_PUB_DSGCR_LPIOPD(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 3)
#define SET_SNPS_DDR_PUB_DSGCR_LPIOPD(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 3, 3)

#define OFFSET_SNPS_DDR_PUB_DSGCR_LPDLLPD 4
#define SNPS_DDR_PUB_DSGCR_LPDLLPD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 4, 4)
#define GET_SNPS_DDR_PUB_DSGCR_LPDLLPD(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 4, 4)
#define SET_SNPS_DDR_PUB_DSGCR_LPDLLPD(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 4, 4)

#define OFFSET_SNPS_DDR_PUB_DSGCR_DQSGX 5
#define SNPS_DDR_PUB_DSGCR_DQSGX(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 5)
#define GET_SNPS_DDR_PUB_DSGCR_DQSGX(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 5)
#define SET_SNPS_DDR_PUB_DSGCR_DQSGX(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 7, 5)

#define OFFSET_SNPS_DDR_PUB_DSGCR_DQSGE 8
#define SNPS_DDR_PUB_DSGCR_DQSGE(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 10, 8)
#define GET_SNPS_DDR_PUB_DSGCR_DQSGE(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 10, 8)
#define SET_SNPS_DDR_PUB_DSGCR_DQSGE(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 10, 8)

#define OFFSET_SNPS_DDR_PUB_DSGCR_NOBUB 11
#define SNPS_DDR_PUB_DSGCR_NOBUB(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 11)
#define GET_SNPS_DDR_PUB_DSGCR_NOBUB(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 11)
#define SET_SNPS_DDR_PUB_DSGCR_NOBUB(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 11, 11)

#define OFFSET_SNPS_DDR_PUB_DSGCR_FXDLAT 12
#define SNPS_DDR_PUB_DSGCR_FXDLAT(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 12, 12)
#define GET_SNPS_DDR_PUB_DSGCR_FXDLAT(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 12, 12)
#define SET_SNPS_DDR_PUB_DSGCR_FXDLAT(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 12, 12)

#define OFFSET_SNPS_DDR_PUB_DSGCR_CKEPDD 16
#define SNPS_DDR_PUB_DSGCR_CKEPDD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 19, 16)
#define GET_SNPS_DDR_PUB_DSGCR_CKEPDD(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 19, 16)
#define SET_SNPS_DDR_PUB_DSGCR_CKEPDD(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 19, 16)

#define OFFSET_SNPS_DDR_PUB_DSGCR_ODTPDD 20
#define SNPS_DDR_PUB_DSGCR_ODTPDD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 23, 20)
#define GET_SNPS_DDR_PUB_DSGCR_ODTPDD(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 23, 20)
#define SET_SNPS_DDR_PUB_DSGCR_ODTPDD(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 23, 20)

#define OFFSET_SNPS_DDR_PUB_DSGCR_NL2PD 24
#define SNPS_DDR_PUB_DSGCR_NL2PD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 24, 24)
#define GET_SNPS_DDR_PUB_DSGCR_NL2PD(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 24, 24)
#define SET_SNPS_DDR_PUB_DSGCR_NL2PD(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 24, 24)

#define OFFSET_SNPS_DDR_PUB_DSGCR_NL2OE 25
#define SNPS_DDR_PUB_DSGCR_NL2OE(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 25, 25)
#define GET_SNPS_DDR_PUB_DSGCR_NL2OE(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 25, 25)
#define SET_SNPS_DDR_PUB_DSGCR_NL2OE(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 25, 25)

#define OFFSET_SNPS_DDR_PUB_DSGCR_TPDPD 26
#define SNPS_DDR_PUB_DSGCR_TPDPD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 26, 26)
#define GET_SNPS_DDR_PUB_DSGCR_TPDPD(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 26, 26)
#define SET_SNPS_DDR_PUB_DSGCR_TPDPD(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 26, 26)

#define OFFSET_SNPS_DDR_PUB_DSGCR_TPDOE 27
#define SNPS_DDR_PUB_DSGCR_TPDOE(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 27, 27)
#define GET_SNPS_DDR_PUB_DSGCR_TPDOE(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 27, 27)
#define SET_SNPS_DDR_PUB_DSGCR_TPDOE(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 27, 27)

#define OFFSET_SNPS_DDR_PUB_DSGCR_CKOE 28
#define SNPS_DDR_PUB_DSGCR_CKOE(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 28, 28)
#define GET_SNPS_DDR_PUB_DSGCR_CKOE(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 28, 28)
#define SET_SNPS_DDR_PUB_DSGCR_CKOE(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 28, 28)

#define OFFSET_SNPS_DDR_PUB_DSGCR_ODTOE 29
#define SNPS_DDR_PUB_DSGCR_ODTOE(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 29, 29)
#define GET_SNPS_DDR_PUB_DSGCR_ODTOE(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 29, 29)
#define SET_SNPS_DDR_PUB_DSGCR_ODTOE(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 29, 29)

#define OFFSET_SNPS_DDR_PUB_DSGCR_RSTOE 30
#define SNPS_DDR_PUB_DSGCR_RSTOE(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 30, 30)
#define GET_SNPS_DDR_PUB_DSGCR_RSTOE(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 30, 30)
#define SET_SNPS_DDR_PUB_DSGCR_RSTOE(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 30, 30)

#define OFFSET_SNPS_DDR_PUB_DSGCR_CKEOE 31
#define SNPS_DDR_PUB_DSGCR_CKEOE(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 31)
#define GET_SNPS_DDR_PUB_DSGCR_CKEOE(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 31)
#define SET_SNPS_DDR_PUB_DSGCR_CKEOE(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 31, 31)

#define OFFSET_SNPS_DDR_PUB_DCR_DDRMD 0
#define SNPS_DDR_PUB_DCR_DDRMD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 0)
#define GET_SNPS_DDR_PUB_DCR_DDRMD(a)                                          \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 0)
#define SET_SNPS_DDR_PUB_DCR_DDRMD(reg, f)                                     \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 2, 0)

#define OFFSET_SNPS_DDR_PUB_DCR_DDR8BNK 3
#define SNPS_DDR_PUB_DCR_DDR8BNK(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 3)
#define GET_SNPS_DDR_PUB_DCR_DDR8BNK(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 3)
#define SET_SNPS_DDR_PUB_DCR_DDR8BNK(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 3, 3)

#define OFFSET_SNPS_DDR_PUB_DCR_PDQ 4
#define SNPS_DDR_PUB_DCR_PDQ(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 6, 4)
#define GET_SNPS_DDR_PUB_DCR_PDQ(a) GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 6, 4)
#define SET_SNPS_DDR_PUB_DCR_PDQ(reg, f)                                       \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 6, 4)

#define OFFSET_SNPS_DDR_PUB_DCR_MPRDQ 7
#define SNPS_DDR_PUB_DCR_MPRDQ(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 7)
#define GET_SNPS_DDR_PUB_DCR_MPRDQ(a)                                          \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 7)
#define SET_SNPS_DDR_PUB_DCR_MPRDQ(reg, f)                                     \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 7, 7)

#define OFFSET_SNPS_DDR_PUB_DCR_DDRTYPE 8
#define SNPS_DDR_PUB_DCR_DDRTYPE(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 9, 8)
#define GET_SNPS_DDR_PUB_DCR_DDRTYPE(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 9, 8)
#define SET_SNPS_DDR_PUB_DCR_DDRTYPE(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 9, 8)

#define OFFSET_SNPS_DDR_PUB_DCR_NOSRA 27
#define SNPS_DDR_PUB_DCR_NOSRA(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 27, 27)
#define GET_SNPS_DDR_PUB_DCR_NOSRA(a)                                          \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 27, 27)
#define SET_SNPS_DDR_PUB_DCR_NOSRA(reg, f)                                     \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 27, 27)

#define OFFSET_SNPS_DDR_PUB_DCR_DDR2T 28
#define SNPS_DDR_PUB_DCR_DDR2T(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 28, 28)
#define GET_SNPS_DDR_PUB_DCR_DDR2T(a)                                          \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 28, 28)
#define SET_SNPS_DDR_PUB_DCR_DDR2T(reg, f)                                     \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 28, 28)

#define OFFSET_SNPS_DDR_PUB_DCR_UDIMM 29
#define SNPS_DDR_PUB_DCR_UDIMM(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 29, 29)
#define GET_SNPS_DDR_PUB_DCR_UDIMM(a)                                          \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 29, 29)
#define SET_SNPS_DDR_PUB_DCR_UDIMM(reg, f)                                     \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 29, 29)

#define OFFSET_SNPS_DDR_PUB_DCR_RDIMM 30
#define SNPS_DDR_PUB_DCR_RDIMM(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 30, 30)
#define GET_SNPS_DDR_PUB_DCR_RDIMM(a)                                          \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 30, 30)
#define SET_SNPS_DDR_PUB_DCR_RDIMM(reg, f)                                     \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 30, 30)

#define OFFSET_SNPS_DDR_PUB_DCR_TPD 31
#define SNPS_DDR_PUB_DCR_TPD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 31)
#define GET_SNPS_DDR_PUB_DCR_TPD(a)                                            \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 31)
#define SET_SNPS_DDR_PUB_DCR_TPD(reg, f)                                       \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 31, 31)

#define OFFSET_SNPS_DDR_PUB_DTPR0_TMRD 0
#define SNPS_DDR_PUB_DTPR0_TMRD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 0)
#define GET_SNPS_DDR_PUB_DTPR0_TMRD(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 0)
#define SET_SNPS_DDR_PUB_DTPR0_TMRD(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 1, 0)

#define OFFSET_SNPS_DDR_PUB_DTPR0_TRTP 2
#define SNPS_DDR_PUB_DTPR0_TRTP(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 4, 2)
#define GET_SNPS_DDR_PUB_DTPR0_TRTP(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 4, 2)
#define SET_SNPS_DDR_PUB_DTPR0_TRTP(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 4, 2)

#define OFFSET_SNPS_DDR_PUB_DTPR0_TWTR 5
#define SNPS_DDR_PUB_DTPR0_TWTR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 5)
#define GET_SNPS_DDR_PUB_DTPR0_TWTR(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 5)
#define SET_SNPS_DDR_PUB_DTPR0_TWTR(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 7, 5)

#define OFFSET_SNPS_DDR_PUB_DTPR0_TRP 8
#define SNPS_DDR_PUB_DTPR0_TRP(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 8)
#define GET_SNPS_DDR_PUB_DTPR0_TRP(a)                                          \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 8)
#define SET_SNPS_DDR_PUB_DTPR0_TRP(reg, f)                                     \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 11, 8)

#define OFFSET_SNPS_DDR_PUB_DTPR0_TRCD 12
#define SNPS_DDR_PUB_DTPR0_TRCD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 12)
#define GET_SNPS_DDR_PUB_DTPR0_TRCD(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 12)
#define SET_SNPS_DDR_PUB_DTPR0_TRCD(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 15, 12)

#define OFFSET_SNPS_DDR_PUB_DTPR0_TRAS 16
#define SNPS_DDR_PUB_DTPR0_TRAS(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 20, 16)
#define GET_SNPS_DDR_PUB_DTPR0_TRAS(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 20, 16)
#define SET_SNPS_DDR_PUB_DTPR0_TRAS(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 20, 16)

#define OFFSET_SNPS_DDR_PUB_DTPR0_TRRD 21
#define SNPS_DDR_PUB_DTPR0_TRRD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 24, 21)
#define GET_SNPS_DDR_PUB_DTPR0_TRRD(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 24, 21)
#define SET_SNPS_DDR_PUB_DTPR0_TRRD(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 24, 21)

#define OFFSET_SNPS_DDR_PUB_DTPR0_TRC 25
#define SNPS_DDR_PUB_DTPR0_TRC(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 30, 25)
#define GET_SNPS_DDR_PUB_DTPR0_TRC(a)                                          \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 30, 25)
#define SET_SNPS_DDR_PUB_DTPR0_TRC(reg, f)                                     \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 30, 25)

#define OFFSET_SNPS_DDR_PUB_DTPR0_TCCD 31
#define SNPS_DDR_PUB_DTPR0_TCCD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 31)
#define GET_SNPS_DDR_PUB_DTPR0_TCCD(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 31)
#define SET_SNPS_DDR_PUB_DTPR0_TCCD(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 31, 31)

#define OFFSET_SNPS_DDR_PUB_DTPR1_TAOND 0
#define SNPS_DDR_PUB_DTPR1_TAOND(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 0)
#define GET_SNPS_DDR_PUB_DTPR1_TAOND(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 0)
#define SET_SNPS_DDR_PUB_DTPR1_TAOND(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 1, 0)

#define OFFSET_SNPS_DDR_PUB_DTPR1_TRTW 2
#define SNPS_DDR_PUB_DTPR1_TRTW(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 2)
#define GET_SNPS_DDR_PUB_DTPR1_TRTW(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 2)
#define SET_SNPS_DDR_PUB_DTPR1_TRTW(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 2, 2)

#define OFFSET_SNPS_DDR_PUB_DTPR1_TFAW 3
#define SNPS_DDR_PUB_DTPR1_TFAW(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 8, 3)
#define GET_SNPS_DDR_PUB_DTPR1_TFAW(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 8, 3)
#define SET_SNPS_DDR_PUB_DTPR1_TFAW(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 8, 3)

#define OFFSET_SNPS_DDR_PUB_DTPR1_TMOD 9
#define SNPS_DDR_PUB_DTPR1_TMOD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 10, 9)
#define GET_SNPS_DDR_PUB_DTPR1_TMOD(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 10, 9)
#define SET_SNPS_DDR_PUB_DTPR1_TMOD(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 10, 9)

#define OFFSET_SNPS_DDR_PUB_DTPR1_TRTODT 11
#define SNPS_DDR_PUB_DTPR1_TRTODT(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 11)
#define GET_SNPS_DDR_PUB_DTPR1_TRTODT(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 11)
#define SET_SNPS_DDR_PUB_DTPR1_TRTODT(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 11, 11)

#define OFFSET_SNPS_DDR_PUB_DTPR1_TRFC 16
#define SNPS_DDR_PUB_DTPR1_TRFC(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 23, 16)
#define GET_SNPS_DDR_PUB_DTPR1_TRFC(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 23, 16)
#define SET_SNPS_DDR_PUB_DTPR1_TRFC(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 23, 16)

#define OFFSET_SNPS_DDR_PUB_DTPR1_TDQSCK 24
#define SNPS_DDR_PUB_DTPR1_TDQSCK(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 26, 24)
#define GET_SNPS_DDR_PUB_DTPR1_TDQSCK(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 26, 24)
#define SET_SNPS_DDR_PUB_DTPR1_TDQSCK(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 26, 24)

#define OFFSET_SNPS_DDR_PUB_DTPR1_TDQSCKMAX 27
#define SNPS_DDR_PUB_DTPR1_TDQSCKMAX(a)                                        \
    SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 29, 27)
#define GET_SNPS_DDR_PUB_DTPR1_TDQSCKMAX(a)                                    \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 29, 27)
#define SET_SNPS_DDR_PUB_DTPR1_TDQSCKMAX(reg, f)                               \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 29, 27)

#define OFFSET_SNPS_DDR_PUB_DTPR2_TXS 0
#define SNPS_DDR_PUB_DTPR2_TXS(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 9, 0)
#define GET_SNPS_DDR_PUB_DTPR2_TXS(a)                                          \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 9, 0)
#define SET_SNPS_DDR_PUB_DTPR2_TXS(reg, f)                                     \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 9, 0)

#define OFFSET_SNPS_DDR_PUB_DTPR2_TXP 10
#define SNPS_DDR_PUB_DTPR2_TXP(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 14, 10)
#define GET_SNPS_DDR_PUB_DTPR2_TXP(a)                                          \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 14, 10)
#define SET_SNPS_DDR_PUB_DTPR2_TXP(reg, f)                                     \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 14, 10)

#define OFFSET_SNPS_DDR_PUB_DTPR2_TCKE 15
#define SNPS_DDR_PUB_DTPR2_TCKE(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 18, 15)
#define GET_SNPS_DDR_PUB_DTPR2_TCKE(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 18, 15)
#define SET_SNPS_DDR_PUB_DTPR2_TCKE(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 18, 15)

#define OFFSET_SNPS_DDR_PUB_DTPR2_TDLLK 19
#define SNPS_DDR_PUB_DTPR2_TDLLK(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 28, 19)
#define GET_SNPS_DDR_PUB_DTPR2_TDLLK(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 28, 19)
#define SET_SNPS_DDR_PUB_DTPR2_TDLLK(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 28, 19)

#define OFFSET_SNPS_DDR_PUB_MR0_BL 0
#define SNPS_DDR_PUB_MR0_BL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 0)
#define GET_SNPS_DDR_PUB_MR0_BL(a) GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 0)
#define SET_SNPS_DDR_PUB_MR0_BL(reg, f)                                        \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 1, 0)

#define OFFSET_SNPS_DDR_PUB_MR0_CL_LOWEST 2
#define SNPS_DDR_PUB_MR0_CL_LOWEST(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 2)
#define GET_SNPS_DDR_PUB_MR0_CL_LOWEST(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 2)
#define SET_SNPS_DDR_PUB_MR0_CL_LOWEST(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 2, 2)

#define OFFSET_SNPS_DDR_PUB_MR0_BT 3
#define SNPS_DDR_PUB_MR0_BT(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 3)
#define GET_SNPS_DDR_PUB_MR0_BT(a) GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 3)
#define SET_SNPS_DDR_PUB_MR0_BT(reg, f)                                        \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 3, 3)

#define OFFSET_SNPS_DDR_PUB_MR0_CL 4
#define SNPS_DDR_PUB_MR0_CL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 6, 4)
#define GET_SNPS_DDR_PUB_MR0_CL(a) GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 6, 4)
#define SET_SNPS_DDR_PUB_MR0_CL(reg, f)                                        \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 6, 4)

#define OFFSET_SNPS_DDR_PUB_MR0_TM 7
#define SNPS_DDR_PUB_MR0_TM(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 7)
#define GET_SNPS_DDR_PUB_MR0_TM(a) GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 7)
#define SET_SNPS_DDR_PUB_MR0_TM(reg, f)                                        \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 7, 7)

#define OFFSET_SNPS_DDR_PUB_MR0_DR 8
#define SNPS_DDR_PUB_MR0_DR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 8, 8)
#define GET_SNPS_DDR_PUB_MR0_DR(a) GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 8, 8)
#define SET_SNPS_DDR_PUB_MR0_DR(reg, f)                                        \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 8, 8)

#define OFFSET_SNPS_DDR_PUB_MR0_WR 9
#define SNPS_DDR_PUB_MR0_WR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 9)
#define GET_SNPS_DDR_PUB_MR0_WR(a) GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 9)
#define SET_SNPS_DDR_PUB_MR0_WR(reg, f)                                        \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 11, 9)

#define OFFSET_SNPS_DDR_PUB_MR0_PD 12
#define SNPS_DDR_PUB_MR0_PD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 12, 12)
#define GET_SNPS_DDR_PUB_MR0_PD(a)                                             \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 12, 12)
#define SET_SNPS_DDR_PUB_MR0_PD(reg, f)                                        \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 12, 12)

#define OFFSET_SNPS_DDR_PUB_MR0_RSVD 13
#define SNPS_DDR_PUB_MR0_RSVD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 13)
#define GET_SNPS_DDR_PUB_MR0_RSVD(a)                                           \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 13)
#define SET_SNPS_DDR_PUB_MR0_RSVD(reg, f)                                      \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 15, 13)

#define OFFSET_SNPS_DDR_PUB_MR1_LPDDR2_BL 0
#define SNPS_DDR_PUB_MR1_LPDDR2_BL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 0)
#define GET_SNPS_DDR_PUB_MR1_LPDDR2_BL(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 0)
#define SET_SNPS_DDR_PUB_MR1_LPDDR2_BL(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 2, 0)

#define OFFSET_SNPS_DDR_PUB_MR1_LPDDR2_BT 3
#define SNPS_DDR_PUB_MR1_LPDDR2_BT(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 3)
#define GET_SNPS_DDR_PUB_MR1_LPDDR2_BT(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 3)
#define SET_SNPS_DDR_PUB_MR1_LPDDR2_BT(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 3, 3)

#define OFFSET_SNPS_DDR_PUB_MR1_LPDDR2_WC 4
#define SNPS_DDR_PUB_MR1_LPDDR2_WC(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 4, 4)
#define GET_SNPS_DDR_PUB_MR1_LPDDR2_WC(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 4, 4)
#define SET_SNPS_DDR_PUB_MR1_LPDDR2_WC(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 4, 4)

#define OFFSET_SNPS_DDR_PUB_MR1_LPDDR2_NWR 5
#define SNPS_DDR_PUB_MR1_LPDDR2_NWR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 5)
#define GET_SNPS_DDR_PUB_MR1_LPDDR2_NWR(a)                                     \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 5)
#define SET_SNPS_DDR_PUB_MR1_LPDDR2_NWR(reg, f)                                \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 7, 5)

#define OFFSET_SNPS_DDR_PUB_MR2_PASR 0
#define SNPS_DDR_PUB_MR2_PASR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 0)
#define GET_SNPS_DDR_PUB_MR2_PASR(a)                                           \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 0)
#define SET_SNPS_DDR_PUB_MR2_PASR(reg, f)                                      \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 2, 0)

#define OFFSET_SNPS_DDR_PUB_MR2_CWL 3
#define SNPS_DDR_PUB_MR2_CWL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 5, 3)
#define GET_SNPS_DDR_PUB_MR2_CWL(a) GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 5, 3)
#define SET_SNPS_DDR_PUB_MR2_CWL(reg, f)                                       \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 5, 3)

#define OFFSET_SNPS_DDR_PUB_MR2_ASR 6
#define SNPS_DDR_PUB_MR2_ASR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 6, 6)
#define GET_SNPS_DDR_PUB_MR2_ASR(a) GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 6, 6)
#define SET_SNPS_DDR_PUB_MR2_ASR(reg, f)                                       \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 6, 6)

#define OFFSET_SNPS_DDR_PUB_MR2_SRT 7
#define SNPS_DDR_PUB_MR2_SRT(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 7)
#define GET_SNPS_DDR_PUB_MR2_SRT(a) GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 7)
#define SET_SNPS_DDR_PUB_MR2_SRT(reg, f)                                       \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 7, 7)

#define OFFSET_SNPS_DDR_PUB_MR2_RTTWR 9
#define SNPS_DDR_PUB_MR2_RTTWR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 10, 9)
#define GET_SNPS_DDR_PUB_MR2_RTTWR(a)                                          \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 10, 9)
#define SET_SNPS_DDR_PUB_MR2_RTTWR(reg, f)                                     \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 10, 9)

#define OFFSET_SNPS_DDR_PUB_MR3_DS 0
#define SNPS_DDR_PUB_MR3_DS(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 0)
#define GET_SNPS_DDR_PUB_MR3_DS(a) GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 0)
#define SET_SNPS_DDR_PUB_MR3_DS(reg, f)                                        \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 3, 0)

#define OFFSET_SNPS_DDR_PUB_DCUAR_CWADDR 0
#define SNPS_DDR_PUB_DCUAR_CWADDR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 0)
#define GET_SNPS_DDR_PUB_DCUAR_CWADDR(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 0)
#define SET_SNPS_DDR_PUB_DCUAR_CWADDR(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 3, 0)

#define OFFSET_SNPS_DDR_PUB_DCUAR_CSADDR 4
#define SNPS_DDR_PUB_DCUAR_CSADDR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 4)
#define GET_SNPS_DDR_PUB_DCUAR_CSADDR(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 4)
#define SET_SNPS_DDR_PUB_DCUAR_CSADDR(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 7, 4)

#define OFFSET_SNPS_DDR_PUB_DCUAR_CSEL 8
#define SNPS_DDR_PUB_DCUAR_CSEL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 9, 8)
#define GET_SNPS_DDR_PUB_DCUAR_CSEL(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 9, 8)
#define SET_SNPS_DDR_PUB_DCUAR_CSEL(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 9, 8)

#define OFFSET_SNPS_DDR_PUB_DCUAR_INCA 10
#define SNPS_DDR_PUB_DCUAR_INCA(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 10, 10)
#define GET_SNPS_DDR_PUB_DCUAR_INCA(a)                                         \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 10, 10)
#define SET_SNPS_DDR_PUB_DCUAR_INCA(reg, f)                                    \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 10, 10)

#define OFFSET_SNPS_DDR_PUB_DCUAR_ATYPE 11
#define SNPS_DDR_PUB_DCUAR_ATYPE(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 11)
#define GET_SNPS_DDR_PUB_DCUAR_ATYPE(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 11)
#define SET_SNPS_DDR_PUB_DCUAR_ATYPE(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 11, 11)

#define OFFSET_SNPS_DDR_PUB_DCUTPR_TDCUT0 0
#define SNPS_DDR_PUB_DCUTPR_TDCUT0(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 0)
#define GET_SNPS_DDR_PUB_DCUTPR_TDCUT0(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 0)
#define SET_SNPS_DDR_PUB_DCUTPR_TDCUT0(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 7, 0)

#define OFFSET_SNPS_DDR_PUB_DCUTPR_TDCUT1 8
#define SNPS_DDR_PUB_DCUTPR_TDCUT1(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 8)
#define GET_SNPS_DDR_PUB_DCUTPR_TDCUT1(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 8)
#define SET_SNPS_DDR_PUB_DCUTPR_TDCUT1(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 15, 8)

#define OFFSET_SNPS_DDR_PUB_DCUTPR_TDCUT2 16
#define SNPS_DDR_PUB_DCUTPR_TDCUT2(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 23, 16)
#define GET_SNPS_DDR_PUB_DCUTPR_TDCUT2(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 23, 16)
#define SET_SNPS_DDR_PUB_DCUTPR_TDCUT2(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 23, 16)

#define OFFSET_SNPS_DDR_PUB_DCUTPR_TDCUT3 24
#define SNPS_DDR_PUB_DCUTPR_TDCUT3(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 24)
#define GET_SNPS_DDR_PUB_DCUTPR_TDCUT3(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 24)
#define SET_SNPS_DDR_PUB_DCUTPR_TDCUT3(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 31, 24)

#define OFFSET_SNPS_DDR_PUB_BISTUDPR_BUDP0 0
#define SNPS_DDR_PUB_BISTUDPR_BUDP0(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 0)
#define GET_SNPS_DDR_PUB_BISTUDPR_BUDP0(a)                                     \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 0)
#define SET_SNPS_DDR_PUB_BISTUDPR_BUDP0(reg, f)                                \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 15, 0)

#define OFFSET_SNPS_DDR_PUB_BISTUDPR_BUDP1 16
#define SNPS_DDR_PUB_BISTUDPR_BUDP1(a)                                         \
    SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 16)
#define GET_SNPS_DDR_PUB_BISTUDPR_BUDP1(a)                                     \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 16)
#define SET_SNPS_DDR_PUB_BISTUDPR_BUDP1(reg, f)                                \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 31, 16)

#define OFFSET_SNPS_DDR_PUB_DX0GCR_DXEN 0
#define SNPS_DDR_PUB_DX0GCR_DXEN(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 0, 0)
#define GET_SNPS_DDR_PUB_DX0GCR_DXEN(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 0, 0)
#define SET_SNPS_DDR_PUB_DX0GCR_DXEN(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 0, 0)

#define OFFSET_SNPS_DDR_PUB_DX0GCR_DQSODT 1
#define SNPS_DDR_PUB_DX0GCR_DQSODT(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 1)
#define GET_SNPS_DDR_PUB_DX0GCR_DQSODT(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 1, 1)
#define SET_SNPS_DDR_PUB_DX0GCR_DQSODT(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 1, 1)

#define OFFSET_SNPS_DDR_PUB_DX0GCR_DQODT 2
#define SNPS_DDR_PUB_DX0GCR_DQODT(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 2)
#define GET_SNPS_DDR_PUB_DX0GCR_DQODT(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 2)
#define SET_SNPS_DDR_PUB_DX0GCR_DQODT(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 2, 2)

#define OFFSET_SNPS_DDR_PUB_DX0GCR_DXIOM 3
#define SNPS_DDR_PUB_DX0GCR_DXIOM(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 3)
#define GET_SNPS_DDR_PUB_DX0GCR_DXIOM(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 3)
#define SET_SNPS_DDR_PUB_DX0GCR_DXIOM(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 3, 3)

#define OFFSET_SNPS_DDR_PUB_DX0GCR_DXPDD 4
#define SNPS_DDR_PUB_DX0GCR_DXPDD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 4, 4)
#define GET_SNPS_DDR_PUB_DX0GCR_DXPDD(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 4, 4)
#define SET_SNPS_DDR_PUB_DX0GCR_DXPDD(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 4, 4)

#define OFFSET_SNPS_DDR_PUB_DX0GCR_DXPDR 5
#define SNPS_DDR_PUB_DX0GCR_DXPDR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 5, 5)
#define GET_SNPS_DDR_PUB_DX0GCR_DXPDR(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 5, 5)
#define SET_SNPS_DDR_PUB_DX0GCR_DXPDR(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 5, 5)

#define OFFSET_SNPS_DDR_PUB_DX0GCR_DQSRPD 6
#define SNPS_DDR_PUB_DX0GCR_DQSRPD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 6, 6)
#define GET_SNPS_DDR_PUB_DX0GCR_DQSRPD(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 6, 6)
#define SET_SNPS_DDR_PUB_DX0GCR_DQSRPD(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 6, 6)

#define OFFSET_SNPS_DDR_PUB_DX0GCR_DSEN 7
#define SNPS_DDR_PUB_DX0GCR_DSEN(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 8, 7)
#define GET_SNPS_DDR_PUB_DX0GCR_DSEN(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 8, 7)
#define SET_SNPS_DDR_PUB_DX0GCR_DSEN(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 8, 7)

#define OFFSET_SNPS_DDR_PUB_DX0GCR_DQSRTT 9
#define SNPS_DDR_PUB_DX0GCR_DQSRTT(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 9, 9)
#define GET_SNPS_DDR_PUB_DX0GCR_DQSRTT(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 9, 9)
#define SET_SNPS_DDR_PUB_DX0GCR_DQSRTT(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 9, 9)

#define OFFSET_SNPS_DDR_PUB_DX0GCR_DQRTT 10
#define SNPS_DDR_PUB_DX0GCR_DQRTT(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 10, 10)
#define GET_SNPS_DDR_PUB_DX0GCR_DQRTT(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 10, 10)
#define SET_SNPS_DDR_PUB_DX0GCR_DQRTT(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 10, 10)

#define OFFSET_SNPS_DDR_PUB_DX0GCR_RTTOH 11
#define SNPS_DDR_PUB_DX0GCR_RTTOH(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 12, 11)
#define GET_SNPS_DDR_PUB_DX0GCR_RTTOH(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 12, 11)
#define SET_SNPS_DDR_PUB_DX0GCR_RTTOH(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 12, 11)

#define OFFSET_SNPS_DDR_PUB_DX0GCR_RTTOAL 13
#define SNPS_DDR_PUB_DX0GCR_RTTOAL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 13, 13)
#define GET_SNPS_DDR_PUB_DX0GCR_RTTOAL(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 13, 13)
#define SET_SNPS_DDR_PUB_DX0GCR_RTTOAL(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 13, 13)

#define OFFSET_SNPS_DDR_PUB_DX0GCR_R0RVSL 14
#define SNPS_DDR_PUB_DX0GCR_R0RVSL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 16, 14)
#define GET_SNPS_DDR_PUB_DX0GCR_R0RVSL(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 16, 14)
#define SET_SNPS_DDR_PUB_DX0GCR_R0RVSL(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 16, 14)

#define OFFSET_SNPS_DDR_PUB_DX0GCR_R1RVSL 17
#define SNPS_DDR_PUB_DX0GCR_R1RVSL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 19, 17)
#define GET_SNPS_DDR_PUB_DX0GCR_R1RVSL(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 19, 17)
#define SET_SNPS_DDR_PUB_DX0GCR_R1RVSL(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 19, 17)

#define OFFSET_SNPS_DDR_PUB_DX0GCR_R2RVSL 20
#define SNPS_DDR_PUB_DX0GCR_R2RVSL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 22, 20)
#define GET_SNPS_DDR_PUB_DX0GCR_R2RVSL(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 22, 20)
#define SET_SNPS_DDR_PUB_DX0GCR_R2RVSL(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 22, 20)

#define OFFSET_SNPS_DDR_PUB_DX0GCR_R3RVSL 23
#define SNPS_DDR_PUB_DX0GCR_R3RVSL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 25, 23)
#define GET_SNPS_DDR_PUB_DX0GCR_R3RVSL(a)                                      \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 25, 23)
#define SET_SNPS_DDR_PUB_DX0GCR_R3RVSL(reg, f)                                 \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 25, 23)

#define OFFSET_SNPS_DDR_PUB_DX0DQSTR_R0DGSL 0
#define SNPS_DDR_PUB_DX0DQSTR_R0DGSL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 0)
#define GET_SNPS_DDR_PUB_DX0DQSTR_R0DGSL(a)                                    \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 2, 0)
#define SET_SNPS_DDR_PUB_DX0DQSTR_R0DGSL(reg, f)                               \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 2, 0)

#define OFFSET_SNPS_DDR_PUB_DX0DQSTR_R1DGSL 3
#define SNPS_DDR_PUB_DX0DQSTR_R1DGSL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 5, 3)
#define GET_SNPS_DDR_PUB_DX0DQSTR_R1DGSL(a)                                    \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 5, 3)
#define SET_SNPS_DDR_PUB_DX0DQSTR_R1DGSL(reg, f)                               \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 5, 3)

#define OFFSET_SNPS_DDR_PUB_DX0DQSTR_R2DGSL 6
#define SNPS_DDR_PUB_DX0DQSTR_R2DGSL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 8, 6)
#define GET_SNPS_DDR_PUB_DX0DQSTR_R2DGSL(a)                                    \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 8, 6)
#define SET_SNPS_DDR_PUB_DX0DQSTR_R2DGSL(reg, f)                               \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 8, 6)

#define OFFSET_SNPS_DDR_PUB_DX0DQSTR_R3DGSL 9
#define SNPS_DDR_PUB_DX0DQSTR_R3DGSL(a)                                        \
    SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 9)
#define GET_SNPS_DDR_PUB_DX0DQSTR_R3DGSL(a)                                    \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 9)
#define SET_SNPS_DDR_PUB_DX0DQSTR_R3DGSL(reg, f)                               \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 11, 9)

#define OFFSET_SNPS_DDR_PUB_DX0DQSTR_R0DGPS 12
#define SNPS_DDR_PUB_DX0DQSTR_R0DGPS(a)                                        \
    SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 13, 12)
#define GET_SNPS_DDR_PUB_DX0DQSTR_R0DGPS(a)                                    \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 13, 12)
#define SET_SNPS_DDR_PUB_DX0DQSTR_R0DGPS(reg, f)                               \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 13, 12)

#define OFFSET_SNPS_DDR_PUB_DX0DQSTR_R1DGPS 14
#define SNPS_DDR_PUB_DX0DQSTR_R1DGPS(a)                                        \
    SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 14)
#define GET_SNPS_DDR_PUB_DX0DQSTR_R1DGPS(a)                                    \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 14)
#define SET_SNPS_DDR_PUB_DX0DQSTR_R1DGPS(reg, f)                               \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 15, 14)

#define OFFSET_SNPS_DDR_PUB_DX0DQSTR_R2DGPS 16
#define SNPS_DDR_PUB_DX0DQSTR_R2DGPS(a)                                        \
    SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 17, 16)
#define GET_SNPS_DDR_PUB_DX0DQSTR_R2DGPS(a)                                    \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 17, 16)
#define SET_SNPS_DDR_PUB_DX0DQSTR_R2DGPS(reg, f)                               \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 17, 16)

#define OFFSET_SNPS_DDR_PUB_DX0DQSTR_R3DGPS 18
#define SNPS_DDR_PUB_DX0DQSTR_R3DGPS(a)                                        \
    SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 19, 18)
#define GET_SNPS_DDR_PUB_DX0DQSTR_R3DGPS(a)                                    \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 19, 18)
#define SET_SNPS_DDR_PUB_DX0DQSTR_R3DGPS(reg, f)                               \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 19, 18)

#define OFFSET_SNPS_DDR_PUB_DX0DQSTR_DQSDLY 20
#define SNPS_DDR_PUB_DX0DQSTR_DQSDLY(a)                                        \
    SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 22, 20)
#define GET_SNPS_DDR_PUB_DX0DQSTR_DQSDLY(a)                                    \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 22, 20)
#define SET_SNPS_DDR_PUB_DX0DQSTR_DQSDLY(reg, f)                               \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 22, 20)

#define OFFSET_SNPS_DDR_PUB_DX0DQSTR_DQSNDLY 23
#define SNPS_DDR_PUB_DX0DQSTR_DQSNDLY(a)                                       \
    SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 25, 23)
#define GET_SNPS_DDR_PUB_DX0DQSTR_DQSNDLY(a)                                   \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 25, 23)
#define SET_SNPS_DDR_PUB_DX0DQSTR_DQSNDLY(reg, f)                              \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 25, 23)

#define OFFSET_SNPS_DDR_PUB_DX0DQSTR_DMDLY 26
#define SNPS_DDR_PUB_DX0DQSTR_DMDLY(a)                                         \
    SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 29, 26)
#define GET_SNPS_DDR_PUB_DX0DQSTR_DMDLY(a)                                     \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 29, 26)
#define SET_SNPS_DDR_PUB_DX0DQSTR_DMDLY(reg, f)                                \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 29, 26)

#define OFFSET_SNPS_DDR_PUB_DX3DQTR_DQDLY0 0
#define SNPS_DDR_PUB_DX3DQTR_DQDLY0(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 0)
#define GET_SNPS_DDR_PUB_DX3DQTR_DQDLY0(a)                                     \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 3, 0)
#define SET_SNPS_DDR_PUB_DX3DQTR_DQDLY0(reg, f)                                \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 3, 0)

#define OFFSET_SNPS_DDR_PUB_DX3DQTR_DQDLY1 4
#define SNPS_DDR_PUB_DX3DQTR_DQDLY1(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 4)
#define GET_SNPS_DDR_PUB_DX3DQTR_DQDLY1(a)                                     \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 7, 4)
#define SET_SNPS_DDR_PUB_DX3DQTR_DQDLY1(reg, f)                                \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 7, 4)

#define OFFSET_SNPS_DDR_PUB_DX3DQTR_DQDLY2 8
#define SNPS_DDR_PUB_DX3DQTR_DQDLY2(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 8)
#define GET_SNPS_DDR_PUB_DX3DQTR_DQDLY2(a)                                     \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 11, 8)
#define SET_SNPS_DDR_PUB_DX3DQTR_DQDLY2(reg, f)                                \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 11, 8)

#define OFFSET_SNPS_DDR_PUB_DX3DQTR_DQDLY3 12
#define SNPS_DDR_PUB_DX3DQTR_DQDLY3(a)                                         \
    SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 12)
#define GET_SNPS_DDR_PUB_DX3DQTR_DQDLY3(a)                                     \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 15, 12)
#define SET_SNPS_DDR_PUB_DX3DQTR_DQDLY3(reg, f)                                \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 15, 12)

#define OFFSET_SNPS_DDR_PUB_DX3DQTR_DQDLY4 16
#define SNPS_DDR_PUB_DX3DQTR_DQDLY4(a)                                         \
    SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 19, 16)
#define GET_SNPS_DDR_PUB_DX3DQTR_DQDLY4(a)                                     \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 19, 16)
#define SET_SNPS_DDR_PUB_DX3DQTR_DQDLY4(reg, f)                                \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 19, 16)

#define OFFSET_SNPS_DDR_PUB_DX3DQTR_DQDLY5 20
#define SNPS_DDR_PUB_DX3DQTR_DQDLY5(a)                                         \
    SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 23, 20)
#define GET_SNPS_DDR_PUB_DX3DQTR_DQDLY5(a)                                     \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 23, 20)
#define SET_SNPS_DDR_PUB_DX3DQTR_DQDLY5(reg, f)                                \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 23, 20)

#define OFFSET_SNPS_DDR_PUB_DX3DQTR_DQDLY6 24
#define SNPS_DDR_PUB_DX3DQTR_DQDLY6(a)                                         \
    SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 27, 24)
#define GET_SNPS_DDR_PUB_DX3DQTR_DQDLY6(a)                                     \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 27, 24)
#define SET_SNPS_DDR_PUB_DX3DQTR_DQDLY6(reg, f)                                \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 27, 24)

#define OFFSET_SNPS_DDR_PUB_DX3DQTR_DQDLY7 28
#define SNPS_DDR_PUB_DX3DQTR_DQDLY7(a)                                         \
    SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 28)
#define GET_SNPS_DDR_PUB_DX3DQTR_DQDLY7(a)                                     \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 28)
#define SET_SNPS_DDR_PUB_DX3DQTR_DQDLY7(reg, f)                                \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 31, 28)

#define OFFSET_SNPS_DDR_PUB_ZQ0CR0_ZDATA 0
#define SNPS_DDR_PUB_ZQ0CR0_ZDATA(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 27, 0)
#define GET_SNPS_DDR_PUB_ZQ0CR0_ZDATA(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 27, 0)
#define SET_SNPS_DDR_PUB_ZQ0CR0_ZDATA(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 27, 0)

#define OFFSET_SNPS_DDR_PUB_ZQ0CR0_ZDEN 28
#define SNPS_DDR_PUB_ZQ0CR0_ZDEN(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 28, 28)
#define GET_SNPS_DDR_PUB_ZQ0CR0_ZDEN(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 28, 28)
#define SET_SNPS_DDR_PUB_ZQ0CR0_ZDEN(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 28, 28)

#define OFFSET_SNPS_DDR_PUB_ZQ0CR0_ZCALBYP 29
#define SNPS_DDR_PUB_ZQ0CR0_ZCALBYP(a)                                         \
    SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 29, 29)
#define GET_SNPS_DDR_PUB_ZQ0CR0_ZCALBYP(a)                                     \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 29, 29)
#define SET_SNPS_DDR_PUB_ZQ0CR0_ZCALBYP(reg, f)                                \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 29, 29)

#define OFFSET_SNPS_DDR_PUB_ZQ0CR0_ZCAL 30
#define SNPS_DDR_PUB_ZQ0CR0_ZCAL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 30, 30)
#define GET_SNPS_DDR_PUB_ZQ0CR0_ZCAL(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 30, 30)
#define SET_SNPS_DDR_PUB_ZQ0CR0_ZCAL(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 30, 30)

#define OFFSET_SNPS_DDR_PUB_ZQ0CR0_ZQPD 31
#define SNPS_DDR_PUB_ZQ0CR0_ZQPD(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 31)
#define GET_SNPS_DDR_PUB_ZQ0CR0_ZQPD(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 31)
#define SET_SNPS_DDR_PUB_ZQ0CR0_ZQPD(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 31, 31)

#define OFFSET_SNPS_DDR_PUB_ZQ0SR0_ZCTRL 0
#define SNPS_DDR_PUB_ZQ0SR0_ZCTRL(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 27, 0)
#define GET_SNPS_DDR_PUB_ZQ0SR0_ZCTRL(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 27, 0)
#define SET_SNPS_DDR_PUB_ZQ0SR0_ZCTRL(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 27, 0)

#define OFFSET_SNPS_DDR_PUB_ZQ0SR0_ZERR 30
#define SNPS_DDR_PUB_ZQ0SR0_ZERR(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 30, 30)
#define GET_SNPS_DDR_PUB_ZQ0SR0_ZERR(a)                                        \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 30, 30)
#define SET_SNPS_DDR_PUB_ZQ0SR0_ZERR(reg, f)                                   \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 30, 30)

#define OFFSET_SNPS_DDR_PUB_ZQ0SR0_ZDONE 31
#define SNPS_DDR_PUB_ZQ0SR0_ZDONE(a) SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 31)
#define GET_SNPS_DDR_PUB_ZQ0SR0_ZDONE(a)                                       \
    GET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(a, 31, 31)
#define SET_SNPS_DDR_PUB_ZQ0SR0_ZDONE(reg, f)                                  \
    SET_SNPS_DDR_PUB_REG_FIELDS_H_FIELD(reg, f, 31, 31)

#endif /* REG_FIELDS_H_UMCTL2 */
