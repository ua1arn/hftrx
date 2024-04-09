#include "hardware.h"

#if CPUSTYLE_XC7Z && LINUX_SUBSYSTEM

#include <src/linux/gpiops/xgpiops.h>
#include <src/linux/gpiops/xgpiops_hw.h>

uint32_t XGpioPs_ReadReg(uint32_t BaseAddr, uint32_t RegOffset)
{
	return * (volatile uint32_t *) (BaseAddr + RegOffset);
}

void XGpioPs_WriteReg(uint32_t BaseAddr, uint32_t RegOffset, uint32_t Data)
{
	* (volatile uint32_t *) (BaseAddr + RegOffset) = Data;
}

void XGpioPs_ResetHw(UINTPTR BaseAddress)
{

}

#endif /* CPUSTYLE_XC7Z && LINUX_SUBSYSTEM */
