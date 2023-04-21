#ifndef XGPIOPS_HW_H		/* prevent circular inclusions */
#define XGPIOPS_HW_H		/**< by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "hardware.h"

#if CPUSTYLE_XC7Z && LINUX_SUBSYSTEM

typedef uintptr_t UINTPTR;
typedef int32_t s32;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

#define XIL_COMPONENT_IS_READY		1
#define XPAR_XGPIOPS_NUM_INSTANCES	1

/** @name Register offsets for the GPIO. Each register is 32 bits.
 *  @{
 */
#define XGPIOPS_DATA_LSW_OFFSET  0x00000000U  /**< Mask and Data Register LSW, WO */
#define XGPIOPS_DATA_MSW_OFFSET  0x00000004U  /**< Mask and Data Register MSW, WO */
#define XGPIOPS_DATA_OFFSET	 	 0x00000040U  /**< Data Register, RW */
#define XGPIOPS_DATA_RO_OFFSET	 0x00000060U  /**< Data Register - Input, RO */
#define XGPIOPS_DIRM_OFFSET	 	 0x00000204U  /**< Direction Mode Register, RW */
#define XGPIOPS_OUTEN_OFFSET	 0x00000208U  /**< Output Enable Register, RW */
/** @} */

/** @name Register offsets for each Bank.
 *  @{
 */
#define XGPIOPS_DATA_MASK_OFFSET 0x00000008U  /**< Data/Mask Registers offset */
#define XGPIOPS_DATA_BANK_OFFSET 0x00000004U  /**< Data Registers offset */
#define XGPIOPS_REG_MASK_OFFSET  0x00000040U  /**< Registers offset */
/** @} */

/* For backwards compatibility */
#define XGPIOPS_BYPM_MASK_OFFSET	(u32)0x40 /**< Mask for backward support */

/** @} */
#define XGPIOPS_PS_GPIO_BASEADDR     0xFF0B0000U     /**< Flag for Base Address for PS_GPIO in Versal */
#define XGPIOPS_ZERO    0U  /**< Flag for 0 Value */
#define XGPIOPS_ONE     1U  /**< Flag for 1 Value */
#define XGPIOPS_TWO     2U  /**< Flag for 2 Value */
#define XGPIOPS_THREE   3U  /**< Flag for 3 Value */
#define XGPIOPS_FOUR    4U  /**< Flag for 4 Value */
#define XGPIOPS_SIX     6U  /**< Flag for 6 Value */

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/****************************************************************************/
/**
*
* This macro reads the given register.
*
* @param	BaseAddr is the base address of the device.
* @param	RegOffset is the register offset to be read.
*
* @return	The 32-bit value of the register
*
* @note		None.
*
*****************************************************************************/
void XGpioPs_WriteReg(uint32_t BaseAddr, uint32_t RegOffset, uint32_t Data);
uint32_t XGpioPs_ReadReg(uint32_t BaseAddr, uint32_t RegOffset);
void XGpioPs_ResetHw(UINTPTR BaseAddress);

#endif /* CPUSTYLE_XC7Z && LINUX_SUBSYSTEM */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XGPIOPS_HW_H */
/** @} */
