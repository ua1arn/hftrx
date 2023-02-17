#include "hardware.h"

#if CPUSTYLE_XC7Z && LINUX_SUBSYSTEM

#include <src/linux/gpiops/xgpiops.h>

void StubHandler(void *CallBackRef, u32 Bank, u32 Status); /**< Stub handler */

/*****************************************************************************/
/**
*
* This function initializes a XGpioPs instance/driver.
* All members of the XGpioPs instance structure are initialized and
* StubHandlers are assigned to the Bank Status Handlers.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	ConfigPtr points to the XGpioPs device configuration structure.
* @param	EffectiveAddr is the device base address in the virtual memory
*		address space. If the address translation is not used then the
*		physical address should be passed.
*		Unexpected errors may occur if the address mapping is changed
*		after this function is invoked.
*
* @return	XST_SUCCESS always.
*
* @note		None.
*
******************************************************************************/
s32 XGpioPs_CfgInitialize(XGpioPs *InstancePtr, const XGpioPs_Config *ConfigPtr,
				u32 EffectiveAddr)
{
	s32 Status = (s32)0;
	u8 i;
//	Xil_AssertNonvoid(InstancePtr != NULL);
//	Xil_AssertNonvoid(ConfigPtr != NULL);
//	Xil_AssertNonvoid(EffectiveAddr != (u32)0);
	/*
	 * Set some default values for instance data, don't indicate the device
	 * is ready to use until everything has been initialized successfully.
	 */
	InstancePtr->IsReady = 0U;
	InstancePtr->GpioConfig.BaseAddr = EffectiveAddr;
	InstancePtr->GpioConfig.DeviceId = ConfigPtr->DeviceId;

	/* Initialize the Bank data based on platform */
	{
		/*
		 *	Max pins in the GPIO device
		 *	0 - 31,  Bank 0
		 *	32 - 53, Bank 1
		 *	54 - 85, Bank 2
		 *	86 - 117, Bank 3
		 */
		InstancePtr->MaxPinNum = (u32)118;
		InstancePtr->MaxBanks = (u8)4;
	}

	/* Indicate the component is now ready to use. */
	InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

	return Status;
}

/****************************************************************************/
/**
*
* Read the Data register of the specified GPIO bank.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Bank is the bank number of the GPIO to operate on.
*		Valid values are 0-3 in Zynq and 0-5 in Zynq Ultrascale+ MP.
*
* @return	Current value of the Data register.
*
* @note		This function is used for reading the state of all the GPIO pins
*		of specified bank.
*
*****************************************************************************/
u32 XGpioPs_Read(const XGpioPs *InstancePtr, u8 Bank)
{
//	Xil_AssertNonvoid(InstancePtr != NULL);
//	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
//    Xil_AssertNonvoid(Bank < InstancePtr->MaxBanks);

	return XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				 ((u32)(Bank) * XGPIOPS_DATA_BANK_OFFSET) +
				 XGPIOPS_DATA_RO_OFFSET);
}

/****************************************************************************/
/**
*
* Read Data from the specified pin.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Pin is the pin number for which the data has to be read.
*		Valid values are 0-117 in Zynq and 0-173 in Zynq Ultrascale+ MP.
*		See xgpiops.h for the mapping of the pin numbers in the banks.
*
* @return	Current value of the Pin (0 or 1).
*
* @note		This function is used for reading the state of the specified
*		GPIO pin.
*
*****************************************************************************/
u32 XGpioPs_ReadPin(const XGpioPs *InstancePtr, u32 Pin)
{
	u8 Bank;
	u8 PinNumber;

//	Xil_AssertNonvoid(InstancePtr != NULL);
//	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
//	Xil_AssertNonvoid(Pin < InstancePtr->MaxPinNum);

	/* Get the Bank number and Pin number within the bank. */
#ifdef versal
	XGpioPs_GetBankPin(InstancePtr,(u8)Pin, &Bank, &PinNumber);
#else
	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);
#endif
	return (XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				 ((u32)(Bank) * XGPIOPS_DATA_BANK_OFFSET) +
				 XGPIOPS_DATA_RO_OFFSET) >> (u32)PinNumber) & (u32)1;

}

/****************************************************************************/
/**
*
* Write to the Data register of the specified GPIO bank.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Bank is the bank number of the GPIO to operate on.
*		Valid values are 0-3 in Zynq and 0-5 in Zynq Ultrascale+ MP.
* @param	Data is the value to be written to the Data register.
*
* @return	None.
*
* @note		This function is used for writing to all the GPIO pins of
*		the bank. The previous state of the pins is not maintained.
*
*****************************************************************************/
void XGpioPs_Write(const XGpioPs *InstancePtr, u8 Bank, u32 Data)
{
	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_DATA_BANK_OFFSET) +
			  XGPIOPS_DATA_OFFSET, Data);
}

/****************************************************************************/
/**
*
* Write data to the specified pin.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Pin is the pin number to which the Data is to be written.
*		Valid values are 0-117 in Zynq and 0-173 in Zynq Ultrascale+ MP.
* @param	Data is the data to be written to the specified pin (0 or 1).
*
* @return	None.
*
* @note		This function does a masked write to the specified pin of
*		the specified GPIO bank. The previous state of other pins
*		is maintained.
*
*****************************************************************************/
void XGpioPs_WritePin(const XGpioPs *InstancePtr, u32 Pin, u32 Data)
{
	u32 RegOffset;
	u32 Value;
	u8 Bank;
	u8 PinNumber;
	u32 DataVar = Data;

	/* Get the Bank number and Pin number within the bank. */
	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);

	if (PinNumber > 15U) {
		/* There are only 16 data bits in bit maskable register. */
		PinNumber -= (u8)16;
		RegOffset = XGPIOPS_DATA_MSW_OFFSET;
	} else {
		RegOffset = XGPIOPS_DATA_LSW_OFFSET;
	}

	/*
	 * Get the 32 bit value to be written to the Mask/Data register where
	 * the upper 16 bits is the mask and lower 16 bits is the data.
	 */
	DataVar &= (u32)0x01;
	Value = ~((u32)1 << (PinNumber + 16U)) & ((DataVar << PinNumber) | 0xFFFF0000U);
	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_DATA_MASK_OFFSET) +
			  RegOffset, Value);

}



/****************************************************************************/
/**
*
* Set the Direction of the pins of the specified GPIO Bank.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Bank is the bank number of the GPIO to operate on.
*		Valid values are 0-3 in Zynq and 0-5 in Zynq Ultrascale+ MP.
* @param	Direction is the 32 bit mask of the Pin direction to be set for
*		all the pins in the Bank. Bits with 0 are set to Input mode,
*		bits with 1 are	set to Output Mode.
*
* @return	None.
*
* @note		This function is used for setting the direction of all the pins
*		in the specified bank. The previous state of the pins is
*		not maintained.
*
*****************************************************************************/
void XGpioPs_SetDirection(const XGpioPs *InstancePtr, u8 Bank, u32 Direction)
{
	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			  XGPIOPS_DIRM_OFFSET, Direction);
}

/****************************************************************************/
/**
*
* Set the Direction of the specified pin.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Pin is the pin number to which the Data is to be written.
*		Valid values are 0-117 in Zynq and 0-173 in Zynq Ultrascale+ MP.
* @param	Direction is the direction to be set for the specified pin.
*		Valid values are 0 for Input Direction, 1 for Output Direction.
*
* @return	None.
*
*****************************************************************************/
void XGpioPs_SetDirectionPin(const XGpioPs *InstancePtr, u32 Pin, u32 Direction)
{
	u8 Bank;
	u8 PinNumber;
	u32 DirModeReg;

	/* Get the Bank number and Pin number within the bank. */
	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);
	DirModeReg = XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				      ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				      XGPIOPS_DIRM_OFFSET);

	if (Direction!=(u32)0) { /*  Output Direction */
		DirModeReg |= ((u32)1 << (u32)PinNumber);
	} else { /* Input Direction */
		DirModeReg &= ~ ((u32)1 << (u32)PinNumber);
	}

	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			 ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			 XGPIOPS_DIRM_OFFSET, DirModeReg);
}

/****************************************************************************/
/**
*
* Get the Direction of the pins of the specified GPIO Bank.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Bank is the bank number of the GPIO to operate on.
*		Valid values are 0-3 in Zynq and 0-5 in Zynq Ultrascale+ MP.
*
* @return	Returns a 32 bit mask of the Direction register. Bits with 0 are
* 		in Input mode, bits with 1 are in Output Mode.
*
* @note		None.
*
*****************************************************************************/
u32 XGpioPs_GetDirection(const XGpioPs *InstancePtr, u8 Bank)
{
	return XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				XGPIOPS_DIRM_OFFSET);
}

/****************************************************************************/
/**
*
* Get the Direction of the specified pin.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Pin is the pin number for which the Direction is to be
*		retrieved.
*		Valid values are 0-117 in Zynq and 0-173 in Zynq Ultrascale+ MP.
*
* @return	Direction of the specified pin.
*		- 0 for Input Direction
*		- 1 for Output Direction
*
* @note		None.
*
*****************************************************************************/
u32 XGpioPs_GetDirectionPin(const XGpioPs *InstancePtr, u32 Pin)
{
	u8 Bank;
	u8 PinNumber;

	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);

	return (XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				 ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				 XGPIOPS_DIRM_OFFSET) >> (u32)PinNumber) & (u32)1;
}

/****************************************************************************/
/**
*
* Set the Output Enable of the pins of the specified GPIO Bank.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Bank is the bank number of the GPIO to operate on.
*		Valid values are 0-3 in Zynq and 0-5 in Zynq Ultrascale+ MP.
* @param	OpEnable is the 32 bit mask of the Output Enables to be set for
*		all the pins in the Bank. The Output Enable of bits with 0 are
*		disabled, the Output Enable of bits with 1 are enabled.
*
* @return	None.
*
* @note		This function is used for setting the Output Enables of all the
*		pins in the specified bank. The previous state of the Output
*		Enables is not maintained.
*
*****************************************************************************/
void XGpioPs_SetOutputEnable(const XGpioPs *InstancePtr, u8 Bank, u32 OpEnable)
{
	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			  XGPIOPS_OUTEN_OFFSET, OpEnable);
}

/****************************************************************************/
/**
*
* Set the Output Enable of the specified pin.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Pin is the pin number to which the Data is to be written.
*		Valid values are 0-117 in Zynq and 0-173 in Zynq Ultrascale+ MP.
* @param	OpEnable specifies whether the Output Enable for the specified
*		pin should be enabled.
*		Valid values are 0 for Disabling Output Enable,
*		1 for Enabling Output Enable.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void XGpioPs_SetOutputEnablePin(const XGpioPs *InstancePtr, u32 Pin, u32 OpEnable)
{
	u8 Bank;
	u8 PinNumber;
	u32 OpEnableReg;

	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);

	OpEnableReg = XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				       ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				       XGPIOPS_OUTEN_OFFSET);

	if (OpEnable != (u32)0) { /*  Enable Output Enable */
		OpEnableReg |= ((u32)1 << (u32)PinNumber);
	} else { /* Disable Output Enable */
		OpEnableReg &= ~ ((u32)1 << (u32)PinNumber);
	}

	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			  XGPIOPS_OUTEN_OFFSET, OpEnableReg);
}
/****************************************************************************/
/**
*
* Get the Output Enable status of the pins of the specified GPIO Bank.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Bank is the bank number of the GPIO to operate on.
*		Valid values are 0-3 in Zynq and 0-5 in Zynq Ultrascale+ MP.
*
* @return	Returns a a 32 bit mask of the Output Enable register.
*		Bits with 0 are in Disabled state, bits with 1 are in
*		Enabled State.
*
* @note		None.
*
*****************************************************************************/
u32 XGpioPs_GetOutputEnable(const XGpioPs *InstancePtr, u8 Bank)
{
	return XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				XGPIOPS_OUTEN_OFFSET);
}

/****************************************************************************/
/**
*
* Get the Output Enable status of the specified pin.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Pin is the pin number for which the Output Enable status is to
*		be retrieved.
*		Valid values are 0-117 in Zynq and 0-173 in Zynq Ultrascale+ MP.
*
* @return	Output Enable of the specified pin.
*		- 0 if Output Enable is disabled for this pin
*		- 1 if Output Enable is enabled for this pin
*
* @note		None.
*
*****************************************************************************/
u32 XGpioPs_GetOutputEnablePin(const XGpioPs *InstancePtr, u32 Pin)
{
	u8 Bank;
	u8 PinNumber;

	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);

	return (XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				 ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				 XGPIOPS_OUTEN_OFFSET) >> (u32)PinNumber) & (u32)1;
}

/****************************************************************************/
/**
*
* Get the Bank number and the Pin number in the Bank, for the given PinNumber
* in the GPIO device.
*
* @param	PinNumber is the Pin number in the GPIO device.
* @param	BankNumber returns the Bank in which this GPIO pin is present.
*		Valid values are 0 to XGPIOPS_MAX_BANKS - 1.
* @param	PinNumberInBank returns the Pin Number within the Bank.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void XGpioPs_GetBankPin(u8 PinNumber, u8 *BankNumber, u8 *PinNumberInBank)
{
	u32 XGpioPsPinTable[6] = {0};

	{
		XGpioPsPinTable[0] = (u32)31; /* 0 - 31, Bank 0 */
		XGpioPsPinTable[1] = (u32)53; /* 32 - 53, Bank 1 */
		XGpioPsPinTable[2] = (u32)85; /* 54 - 85, Bank 2 */
		XGpioPsPinTable[3] = (u32)117; /* 86 - 117 Bank 3 */

		*BankNumber = 0U;
		while (*BankNumber < XGPIOPS_FOUR) {
			if (PinNumber <= XGpioPsPinTable[*BankNumber]) {
				break;
			}
			(*BankNumber)++;
		}
	}
	if (*BankNumber == (u8)0) {
		*PinNumberInBank = PinNumber;
	}
	else {
		*PinNumberInBank = (u8)((u32)PinNumber % (XGpioPsPinTable[*BankNumber - (u8)1] + (u32)1));
	}
}

/*****************************************************************************/
/**
*
* This is a stub for the status callback. The stub is here in case the upper
* layers do not set the handler.
*
* @param	CallBackRef is a pointer to the upper layer callback reference
* @param	Bank is the GPIO Bank in which an interrupt occurred.
* @param	Status is the Interrupt status of the GPIO bank.
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
void StubHandler(void *CallBackRef, u32 Bank, u32 Status)
{
	(void) CallBackRef;
	(void) Bank;
	(void) Status;
}

#endif /* CPUSTYLE_XC7Z && LINUX_SUBSYSTEM */
