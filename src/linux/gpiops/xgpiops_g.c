#include "hardware.h"

#if CPUSTYLE_XC7Z && LINUX_SUBSYSTEM
#include <src/linux/gpiops/xgpiops.h>

XGpioPs_Config XGpioPs_ConfigTable[1] = {
	{
		(u16)0,				/* Unique ID of device */
		(u32)0xE000A000		/* Base address of device */
	}
};

#endif /* CPUSTYLE_XC7Z && LINUX_SUBSYSTEM */
