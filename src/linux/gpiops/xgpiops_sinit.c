#include "hardware.h"

#if CPUSTYLE_XC7Z && LINUX_SUBSYSTEM

#include <src/linux/gpiops/xgpiops.h>

XGpioPs_Config *XGpioPs_LookupConfig(u16 DeviceId)
{
	XGpioPs_Config *CfgPtr = NULL;
	u32 Index;

	for (Index = 0U; Index < (u32)XPAR_XGPIOPS_NUM_INSTANCES; Index++) {
		if (XGpioPs_ConfigTable[Index].DeviceId == DeviceId) {
			CfgPtr = &XGpioPs_ConfigTable[Index];
			break;
		}
	}

	return (XGpioPs_Config *)CfgPtr;
}

#endif /* CPUSTYLE_XC7Z && LINUX_SUBSYSTEM */
