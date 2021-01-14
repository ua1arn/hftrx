/******************************************************************************
* Copyright (C) 2010 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xgpiops.c
* @addtogroup gpiops_v3_7
* @{
*
* The XGpioPs driver. Functions in this file are the minimum required functions
* for this driver. See xgpiops.h for a detailed description of the driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a sv   01/15/10 First Release
* 1.01a sv   04/15/12 Removed the APIs XGpioPs_SetMode, XGpioPs_SetModePin
*                     XGpioPs_GetMode, XGpioPs_GetModePin as they are not
*		      relevant to Zynq device. The interrupts are disabled
*		      for output pins on all banks during initialization.
* 2.1   hk   04/29/14 Use Input data register DATA_RO for read. CR# 771667.
* 3.00  kvn  02/13/15 Modified code for MISRA-C:2012 compliance.
* 3.1	kvn  04/13/15 Add support for Zynq Ultrascale+ MP. CR# 856980.
* 3.1   aru  07/13/18 Resolved doxygen reported warnings. CR# 1006331.
* 3.4   aru  08/17/18 Resolved MISRA-C mandatory violations. CR# 1007751
* 3.5   sne  03/01/19 Fixes violations according to MISRAC-2012
*                     in safety mode and modified the code such as
*                     Use of mixed mode arithmetic,Declared the pointer param
*                     as Pointer to const,Casting operation to a pointer,
*                     Literal value requires a U suffix.
* 3.5   sne  03/13/19 Added Versal support.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "xgpiops.h"
#include <src/zynq/xplatform_info.h>

/************************** Constant Definitions *****************************/
/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/


/************************** Function Prototypes ******************************/

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
void StubHandler(const void *CallBackRef, u32 Bank, u32 Status)
{
	(void) CallBackRef;
	(void) Bank;
	(void) Status;

	Xil_AssertVoidAlways();
}

/*****************************************************************************/
/*
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
	s32 Status = XST_SUCCESS;
	u8 i;
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(ConfigPtr != NULL);
	Xil_AssertNonvoid(EffectiveAddr != (u32)0);
	/*
	 * Set some default values for instance data, don't indicate the device
	 * is ready to use until everything has been initialized successfully.
	 */
	InstancePtr->IsReady = 0U;
	InstancePtr->GpioConfig.BaseAddr = EffectiveAddr;
	InstancePtr->GpioConfig.DeviceId = ConfigPtr->DeviceId;
	InstancePtr->Handler = (XGpioPs_Handler)StubHandler;
	InstancePtr->Platform = XGetPlatform_Info();

	/* Initialize the Bank data based on platform */
	if (InstancePtr->Platform == (u32)XPLAT_ZYNQ_ULTRA_MP) {
		/*
		 *	Max pins in the ZynqMP GPIO device
		 *	0 - 25,  Bank 0
		 *	26 - 51, Bank 1
		 *	52 - 77, Bank 2
		 *	78 - 109, Bank 3
		 *	110 - 141, Bank 4
		 *	142 - 173, Bank 5
		 */
		InstancePtr->MaxPinNum = (u32)174;
		InstancePtr->MaxBanks = (u8)6;
	}
        else if (InstancePtr->Platform == (u32)XPLAT_VERSAL)
        {
                if(InstancePtr->PmcGpio == (u32)FALSE)
                {
                        /* Max pins in the PS_GPIO devices
                         *  0 -25, Bank 0
                         *  26-57, Bank 3
                         */
                        InstancePtr->MaxPinNum = (u32)58;
                        InstancePtr->MaxBanks = (u8)4;
                }
                else
                {
                        /* Max pins in the PMC_GPIO devices
                         * 0  - 25,Bank 0
                         * 26 - 51,Bank 1
                         * 52 - 83,Bank 3
                         * 84 - 115, Bank 4
                         */
                        InstancePtr->MaxPinNum = (u32)116;
                        InstancePtr->MaxBanks = (u8)5;
                }
        }
        else {
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

	/*
	 * By default, interrupts are not masked in GPIO. Disable
	 * interrupts for all pins in all the 4 banks.
	 */
	for (i=(u8)0U;i<InstancePtr->MaxBanks;i++) {
                if (InstancePtr->Platform == XPLAT_VERSAL){
                        if(InstancePtr->PmcGpio == (u32)FALSE)
                        {
                                if((i== (u8)XGPIOPS_ONE)||(i== (u8)XGPIOPS_TWO))
                                {
                                        continue;
                                }
                                XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
                                                ((u32)(i) * XGPIOPS_REG_MASK_OFFSET) +
                                                XGPIOPS_INTDIS_OFFSET, 0xFFFFFFFFU);
                        }
                        else
                        {
                                if(i==(u32)XGPIOPS_TWO)
                                {
                                        continue;
                                }
                                XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
                                                ((u32)(i) * XGPIOPS_REG_MASK_OFFSET) +
                                                XGPIOPS_INTDIS_OFFSET, 0xFFFFFFFFU);

                       }
                }
                else
                {
		XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
					  ((u32)(i) * XGPIOPS_REG_MASK_OFFSET) +
					  XGPIOPS_INTDIS_OFFSET, 0xFFFFFFFFU);
                }
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
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
        Xil_AssertNonvoid(Bank < InstancePtr->MaxBanks);
#ifdef versal
        if(InstancePtr->PmcGpio == TRUE) {
                Xil_AssertNonvoid(Bank != XGPIOPS_TWO);
        } else {
                Xil_AssertNonvoid((Bank !=XGPIOPS_ONE) && (Bank !=XGPIOPS_TWO));
        }
#endif

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

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertNonvoid(Pin < InstancePtr->MaxPinNum);

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
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
        Xil_AssertVoid(Bank < InstancePtr->MaxBanks);
#ifdef versal
        if(InstancePtr->PmcGpio == TRUE) {
                Xil_AssertVoid(Bank != XGPIOPS_TWO);
        } else {
                Xil_AssertVoid((Bank !=XGPIOPS_ONE) && (Bank !=XGPIOPS_TWO));
        }
#endif

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

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid(Pin < InstancePtr->MaxPinNum);

	/* Get the Bank number and Pin number within the bank. */
#ifdef versal
	XGpioPs_GetBankPin(InstancePtr,(u8)Pin, &Bank, &PinNumber);
#else
	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);
#endif

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
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
        Xil_AssertVoid(Bank < InstancePtr->MaxBanks);
#ifdef versal
        if(InstancePtr->PmcGpio == TRUE) {
                Xil_AssertVoid(Bank != XGPIOPS_TWO);
        } else {
                Xil_AssertVoid((Bank !=XGPIOPS_ONE) && (Bank !=XGPIOPS_TWO));
        }
#endif

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

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid(Pin < InstancePtr->MaxPinNum);
	Xil_AssertVoid(Direction <= (u32)1);

	/* Get the Bank number and Pin number within the bank. */
#ifdef versal
	XGpioPs_GetBankPin(InstancePtr,(u8)Pin, &Bank, &PinNumber);
#else
	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);
#endif
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
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
        Xil_AssertNonvoid(Bank < InstancePtr->MaxBanks);
#ifdef versal
        if(InstancePtr->PmcGpio == TRUE) {
                Xil_AssertNonvoid(Bank != XGPIOPS_TWO);
        } else {
                Xil_AssertNonvoid((Bank !=XGPIOPS_ONE) && (Bank !=XGPIOPS_TWO));
        }
#endif

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

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertNonvoid(Pin < InstancePtr->MaxPinNum);

	/* Get the Bank number and Pin number within the bank. */
#ifdef versal
	XGpioPs_GetBankPin(InstancePtr,(u8)Pin, &Bank, &PinNumber);
#else
	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);
#endif

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
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
        Xil_AssertVoid(Bank < InstancePtr->MaxBanks);
#ifdef versal
        if(InstancePtr->PmcGpio == TRUE) {
                Xil_AssertVoid(Bank != XGPIOPS_TWO);
        } else {
                Xil_AssertVoid((Bank !=XGPIOPS_ONE) && (Bank !=XGPIOPS_TWO));
        }
#endif

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

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid(Pin < InstancePtr->MaxPinNum);
	Xil_AssertVoid(OpEnable <= (u32)1);

	/* Get the Bank number and Pin number within the bank. */
#ifdef versal
	XGpioPs_GetBankPin(InstancePtr,(u8)Pin, &Bank, &PinNumber);
#else
	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);
#endif

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
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
        Xil_AssertNonvoid(Bank < InstancePtr->MaxBanks);
#ifdef versal
        if(InstancePtr->PmcGpio == TRUE) {
                Xil_AssertNonvoid(Bank != XGPIOPS_TWO);
        } else {
                Xil_AssertNonvoid((Bank !=XGPIOPS_ONE) && (Bank !=XGPIOPS_TWO));
        }
#endif

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

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertNonvoid(Pin < InstancePtr->MaxPinNum);

	/* Get the Bank number and Pin number within the bank. */
#ifdef versal
	XGpioPs_GetBankPin(InstancePtr,(u8)Pin, &Bank, &PinNumber);
#else
	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);
#endif

	return (XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				 ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				 XGPIOPS_OUTEN_OFFSET) >> (u32)PinNumber) & (u32)1;
}

/****************************************************************************/
/*
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
#ifdef versal
void XGpioPs_GetBankPin(const XGpioPs *InstancePtr,u8 PinNumber, u8 *BankNumber, u8 *PinNumberInBank)
#else
void XGpioPs_GetBankPin(u8 PinNumber, u8 *BankNumber, u8 *PinNumberInBank)
#endif
{
	u32 XGpioPsPinTable[6] = {0};
#ifdef versal
        u8 i=(u8)0;
#endif
	u32 Platform = XGetPlatform_Info();

	if (Platform == (u32)XPLAT_ZYNQ_ULTRA_MP) {
		/*
		 * This structure defines the mapping of the pin numbers to the banks when
		 * the driver APIs are used for working on the individual pins.
		 */

		XGpioPsPinTable[0] = (u32)25; /* 0 - 25, Bank 0 */
		XGpioPsPinTable[1] = (u32)51; /* 26 - 51, Bank 1 */
		XGpioPsPinTable[2] = (u32)77; /* 52 - 77, Bank 2 */
		XGpioPsPinTable[3] = (u32)109; /* 78 - 109, Bank 3 */
		XGpioPsPinTable[4] = (u32)141; /* 110 - 141, Bank 4 */
		XGpioPsPinTable[5] = (u32)173; /* 142 - 173 Bank 5 */

		*BankNumber = 0U;
		while (*BankNumber < XGPIOPS_SIX) {
			if (PinNumber <= XGpioPsPinTable[*BankNumber]) {
				break;
			}
			(*BankNumber)++;
		}
	}
#ifdef versal
        else if(Platform == XPLAT_VERSAL)
        {
                if(InstancePtr->PmcGpio == (u32)(FALSE))
                {
                        XGpioPsPinTable[0] = (u32)25; /* 0 - 25, Bank 0 */
                        XGpioPsPinTable[1] = (u32)57; /* 26 - 57, Bank 3 */
                        *BankNumber =0U;
                        if(PinNumber <= XGpioPsPinTable[*BankNumber])
                        {
                                *BankNumber = (u8)XGPIOPS_ZERO;
                        }
                        else
                        {
                                *BankNumber = (u8)XGPIOPS_THREE;
                        }

                }
                else
                {
                        XGpioPsPinTable[0] = (u32)25; /* 0 - 25, Bank 0 */
                        XGpioPsPinTable[1] = (u32)51; /* 26 - 51, Bank 1 */
                        XGpioPsPinTable[2] = (u32)83; /* 52 - 83, Bank 3 */
                        XGpioPsPinTable[3] = (u32)115; /*84 - 115, Bank 4 */

                        *BankNumber =0U;
                        while(i < XGPIOPS_FOUR)
                        {
                                if(i <= (u8)XGPIOPS_ONE)
                                {
                                        if (PinNumber <= XGpioPsPinTable[i])
                                        {
                                                *BankNumber = (u8)i;
                                                break;
                                        }
                                        i++;
                                }
                                else
                                {
                                        if (PinNumber <= XGpioPsPinTable[i])
                                        {
                                                *BankNumber = (u8)i+1U;
                                                break;
                                        }
                                        i++;
                                }

                        }
                }

        }
#endif
        else {
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

#ifdef versal
        else if(Platform == XPLAT_VERSAL)
        {
                if(InstancePtr->PmcGpio == (u32)(FALSE))
                {
                        *PinNumberInBank = (u8)((u32)PinNumber - (XGpioPsPinTable[0] + (u32)1));
                }
                else {
                        if((*BankNumber ==(u8)XGPIOPS_THREE) || (*BankNumber ==(u8)XGPIOPS_FOUR))
                        {
                                *PinNumberInBank = (u8)((u32)PinNumber %
                                                (XGpioPsPinTable[*BankNumber - (u8)XGPIOPS_TWO] + (u32)1));
                        }
                        else
                        {
                                *PinNumberInBank = (u8)((u32)PinNumber %
                                                (XGpioPsPinTable[*BankNumber - (u8)1] + (u32)1));
                        }
               }

        }
#endif

        else {
		*PinNumberInBank = (u8)((u32)PinNumber %
					(XGpioPsPinTable[*BankNumber - (u8)1] + (u32)1));
        }
}
/** @} */


/*******************************************************************
*
* CAUTION: This file is automatically generated by HSI.
* Version: 2020.1
* DO NOT EDIT.
*
* Copyright (C) 2010-2021 Xilinx, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT 

* 
* Description: Driver configuration
*
*******************************************************************/

#include "xparameters.h"
#include "xgpiops.h"

/*
* The configuration table for devices
*/

XGpioPs_Config XGpioPs_ConfigTable[XPAR_XGPIOPS_NUM_INSTANCES] =
{
	{
		XPAR_PS7_GPIO_0_DEVICE_ID,
		XPAR_PS7_GPIO_0_BASEADDR
	}
};

/******************************************************************************
* Copyright (C) 2013 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xgpiops_hw.c
* @addtogroup gpiops_v3_7
* @{
*
* This file contains low level GPIO functions.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.02a hk   08/22/13 First Release
* 3.00  kvn  02/13/15 Modified code for MISRA-C:2012 compliance.
* 3.1	kvn  04/13/15 Add support for Zynq Ultrascale+ MP. CR# 856980.
* 3.5   sne  03/01/19 Fixes violations according to MISRAC-2012
*                     in safety mode and modified the code such as
*                     Use of mixed mode arithmetic,Declared the pointer param
*                     as Pointer to const,Casting operation to a pointer,
*                     Literal value requires a U suffix.
* 3.5   sne  03/14/19 Added versal support.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/


/*****************************************************************************/
/*
*
* This function resets the GPIO module by writing reset values to
* all registers
*
* @param	Base address of GPIO module
*
* @return	None
*
* @note		None.
*
******************************************************************************/
void XGpioPs_ResetHw(u32 BaseAddress)
{
	u32 BankCount;
	u32 Platform,MaxBanks;

	Platform = XGetPlatform_Info();
        if (Platform == (u32)XPLAT_ZYNQ_ULTRA_MP) {
                MaxBanks = (u32)6;
        }
        else if(Platform == (u32)XPLAT_VERSAL)
        {
                if (BaseAddress == (u32)XGPIOPS_PS_GPIO_BASEADDR)
                {
                        MaxBanks = (u32)4;
                }
                else
                {
                        MaxBanks = (u32)5;
                }
        }
        else {
                MaxBanks = (u32)4;
        }

        if (Platform == (u32)XPLAT_VERSAL)
        {
                /* Write reset values to all mask data registers */
                for(BankCount = 3U; BankCount < (u32)MaxBanks; BankCount++) {

                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_DATA_MASK_OFFSET) +
                                         XGPIOPS_DATA_LSW_OFFSET), 0x0U);
                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_DATA_MASK_OFFSET) +
                                         XGPIOPS_DATA_MSW_OFFSET), 0x0U);
                }
                /* Write reset values to all output data registers */
                for(BankCount = 3U; BankCount < (u32)MaxBanks; BankCount++) {

                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_DATA_BANK_OFFSET) +
                                         XGPIOPS_DATA_OFFSET), 0x0U);
                }

                /* Reset all registers of all GPIO banks */
                for(BankCount = 0U; BankCount < (u32)MaxBanks; BankCount++) {


                        if((BaseAddress == (u32)XGPIOPS_PS_GPIO_BASEADDR) && ((BankCount == (u32)XGPIOPS_ONE) ||(BankCount == (u32)XGPIOPS_TWO)))
                        {
                                continue;
                        }
                        else
                        {
                                if((BaseAddress != (u32)XGPIOPS_PS_GPIO_BASEADDR) && (BankCount == (u32)XGPIOPS_ONE))
                                {
                                        continue;
                                }
                        }
                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_REG_MASK_OFFSET) +
                                         XGPIOPS_DIRM_OFFSET), 0x0U);
                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_REG_MASK_OFFSET) +
                                         XGPIOPS_OUTEN_OFFSET), 0x0U);
                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_REG_MASK_OFFSET) +
                                         XGPIOPS_INTMASK_OFFSET), 0x0U);
                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_REG_MASK_OFFSET) +
                                         XGPIOPS_INTEN_OFFSET), 0x0U);
                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_REG_MASK_OFFSET) +
                                         XGPIOPS_INTDIS_OFFSET), 0x0U);
                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_REG_MASK_OFFSET) +
                                         XGPIOPS_INTSTS_OFFSET), 0x0U);
                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_REG_MASK_OFFSET) +
                                         XGPIOPS_INTPOL_OFFSET), 0x0U);
                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_REG_MASK_OFFSET) +
                                         XGPIOPS_INTANY_OFFSET), 0x0U);

                }

                /* Bank 0 Int type */
                XGpioPs_WriteReg(BaseAddress, XGPIOPS_INTTYPE_OFFSET,
                                XGPIOPS_INTTYPE_BANK0_RESET);
                /* Bank 1 Int type */
                XGpioPs_WriteReg(BaseAddress,
                                ((u32)XGPIOPS_REG_MASK_OFFSET + (u32)XGPIOPS_INTTYPE_OFFSET),
                                XGPIOPS_INTTYPE_BANK1_RESET);
                /* Bank 3 Int type */
                XGpioPs_WriteReg(BaseAddress,
                                (((u32)3 * XGPIOPS_REG_MASK_OFFSET) + XGPIOPS_INTTYPE_OFFSET),
                                XGPIOPS_INTTYPE_BANK3_RESET);
                /* Bank 4 Int type */
                XGpioPs_WriteReg(BaseAddress,
                                (((u32)4 * XGPIOPS_REG_MASK_OFFSET) + XGPIOPS_INTTYPE_OFFSET),
                                XGPIOPS_INTTYPE_BANK4_RESET);
        }
        else
        {
                /* Write reset values to all mask data registers */
                for(BankCount = 2U; BankCount < (u32)MaxBanks; BankCount++) {

                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_DATA_MASK_OFFSET) +
                                         XGPIOPS_DATA_LSW_OFFSET), 0x0U);
                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_DATA_MASK_OFFSET) +
                                         XGPIOPS_DATA_MSW_OFFSET), 0x0U);
                }
                /* Write reset values to all output data registers */
                for(BankCount = 2U; BankCount < (u32)MaxBanks; BankCount++) {

                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_DATA_BANK_OFFSET) +
                                         XGPIOPS_DATA_OFFSET), 0x0U);
                }
                /* Reset all registers of all GPIO banks */
                for(BankCount = 0U; BankCount < (u32)MaxBanks; BankCount++) {

                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_REG_MASK_OFFSET) +
                                         XGPIOPS_DIRM_OFFSET), 0x0U);
                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_REG_MASK_OFFSET) +
                                         XGPIOPS_OUTEN_OFFSET), 0x0U);
                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_REG_MASK_OFFSET) +
                                         XGPIOPS_INTMASK_OFFSET), 0x0U);
                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_REG_MASK_OFFSET) +
                                         XGPIOPS_INTEN_OFFSET), 0x0U);
                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_REG_MASK_OFFSET) +
                                         XGPIOPS_INTDIS_OFFSET), 0x0U);
                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_REG_MASK_OFFSET) +
                                         XGPIOPS_INTSTS_OFFSET), 0x0U);
                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_REG_MASK_OFFSET) +
                                         XGPIOPS_INTPOL_OFFSET), 0x0U);
                        XGpioPs_WriteReg(BaseAddress,
                                        ((BankCount * XGPIOPS_REG_MASK_OFFSET) +
                                         XGPIOPS_INTANY_OFFSET), 0x0U);
                }
                /* Bank 0 Int type */
                XGpioPs_WriteReg(BaseAddress, XGPIOPS_INTTYPE_OFFSET,
                                XGPIOPS_INTTYPE_BANK0_RESET);
                /* Bank 1 Int type */
                XGpioPs_WriteReg(BaseAddress,
                                ((u32)XGPIOPS_REG_MASK_OFFSET + (u32)XGPIOPS_INTTYPE_OFFSET),
                                XGPIOPS_INTTYPE_BANK1_RESET);
                /* Bank 2 Int type */
                XGpioPs_WriteReg(BaseAddress,
                                (((u32)2 * XGPIOPS_REG_MASK_OFFSET) + XGPIOPS_INTTYPE_OFFSET),
                                XGPIOPS_INTTYPE_BANK2_RESET);
                /* Bank 3 Int type */
                XGpioPs_WriteReg(BaseAddress,
                                (((u32)3 * XGPIOPS_REG_MASK_OFFSET) + XGPIOPS_INTTYPE_OFFSET),
                                XGPIOPS_INTTYPE_BANK3_RESET);

                if (Platform == (u32)XPLAT_ZYNQ_ULTRA_MP) {
                        /* Bank 4 Int type */
                        XGpioPs_WriteReg(BaseAddress,
                                        (((u32)4 * XGPIOPS_REG_MASK_OFFSET) + XGPIOPS_INTTYPE_OFFSET),
                                        XGPIOPS_INTTYPE_BANK4_RESET);
                        /* Bank 5 Int type */
                        XGpioPs_WriteReg(BaseAddress,
                                        (((u32)5 * XGPIOPS_REG_MASK_OFFSET) + XGPIOPS_INTTYPE_OFFSET),
                                        XGPIOPS_INTTYPE_BANK5_RESET);
                }
        }

}
/** @} */ 

/******************************************************************************
* Copyright (C) 2010 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xgpiops_intr.c
* @addtogroup gpiops_v3_7
* @{
*
* This file contains functions related to GPIO interrupt handling.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a sv   01/18/10 First Release
* 2.2	sk	 10/13/14 Used Pin number in Bank instead of pin number
* 					  passed to API's. CR# 822636
* 3.00  kvn  02/13/15 Modified code for MISRA-C:2012 compliance.
* 3.1	kvn  04/13/15 Add support for Zynq Ultrascale+ MP. CR# 856980.
* 3.1   aru  07/13/18 Ressolved doxygen reported warnings. CR# 1006331.
* 3.4   aru  08/09/18 Ressolved cppcheck warnings.
* 3.4   aru  08/17/18 Resolved MISRA-C mandatory violations. CR# 1007751
* 3.5   sne  03/01/19 Fixes violations according to MISRAC-2012
*                     in safety mode and modified the code such as
*                     Use of mixed mode arithmetic,Declared the pointer param
*                     as Pointer to const,Casting operation to a pointer,
*                     Literal value requires a U suffix.
* 3.5   sne  03/14/19 Added Versal support.
* 3.5   sne  03/20/19 Fixed multiple interrupts problem CR#1024556.
* 3.6	sne  06/12/19 Fixed IAR compiler warning.
* 3.6   sne  08/14/19 Added interrupt handler support on versal.
*
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Variable Definitions *****************************/

/************************** Function Prototypes ******************************/

/****************************************************************************/
/**
*
* This function enables the interrupts for the specified pins in the specified
* bank.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Bank is the bank number of the GPIO to operate on.
*		Valid values are 0-3 in Zynq and 0-5 in Zynq Ultrascale+ MP.
* @param	Mask is the bit mask of the pins for which interrupts are to
*		be enabled. Bit positions of 1 will be enabled. Bit positions
*		of 0 will keep the previous setting.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void XGpioPs_IntrEnable(const XGpioPs *InstancePtr, u8 Bank, u32 Mask)
{
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
        Xil_AssertVoid(Bank < InstancePtr->MaxBanks);
#ifdef versal
        if(InstancePtr->PmcGpio == TRUE) {
                Xil_AssertVoid(Bank != XGPIOPS_TWO);
        } else {
                Xil_AssertVoid((Bank !=XGPIOPS_ONE) && (Bank !=XGPIOPS_TWO));
        }
#endif

	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			  XGPIOPS_INTEN_OFFSET, Mask);
}

/****************************************************************************/
/**
*
* This function enables the interrupt for the specified pin.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Pin is the pin number for which the interrupt is to be enabled.
*		Valid values are 0-117 in Zynq and 0-173 in Zynq Ultrascale+ MP.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void XGpioPs_IntrEnablePin(const XGpioPs *InstancePtr, u32 Pin)
{
	u8 Bank;
	u8 PinNumber;
	u32 IntrReg;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid(Pin < InstancePtr->MaxPinNum);

	/* Get the Bank number and Pin number within the bank. */
#ifdef versal
	XGpioPs_GetBankPin(InstancePtr,(u8)Pin, &Bank, &PinNumber);
#else
	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);
#endif

	IntrReg = ((u32)1 << (u32)PinNumber);
	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			  XGPIOPS_INTEN_OFFSET, IntrReg);
}

/****************************************************************************/
/**
*
* This function disables the interrupts for the specified pins in the specified
* bank.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Bank is the bank number of the GPIO to operate on.
*		Valid values are 0-3 in Zynq and 0-5 in Zynq Ultrascale+ MP.
* @param	Mask is the bit mask of the pins for which interrupts are
*		to be disabled. Bit positions of 1 will be disabled. Bit
*		positions of 0 will keep the previous setting.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void XGpioPs_IntrDisable(const XGpioPs *InstancePtr, u8 Bank, u32 Mask)
{
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid(Bank < InstancePtr->MaxBanks);
#ifdef versal
        if(InstancePtr->PmcGpio == TRUE) {
                Xil_AssertVoid(Bank != XGPIOPS_TWO);
        } else {
                Xil_AssertVoid((Bank !=XGPIOPS_ONE) && (Bank !=XGPIOPS_TWO));
        }
#endif

	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			  XGPIOPS_INTDIS_OFFSET, Mask);
}

/****************************************************************************/
/**
*
* This function disables the interrupts for the specified pin.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Pin is the pin number for which the interrupt is to be disabled.
*		Valid values are 0-117 in Zynq and 0-173 in Zynq Ultrascale+ MP.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void XGpioPs_IntrDisablePin(const XGpioPs *InstancePtr, u32 Pin)
{
	u8 Bank;
	u8 PinNumber;
	u32 IntrReg;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid(Pin < InstancePtr->MaxPinNum);

	/* Get the Bank number and Pin number within the bank. */
#ifdef versal
	XGpioPs_GetBankPin(InstancePtr,(u8)Pin, &Bank, &PinNumber);
#else
	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);
#endif

	IntrReg = ((u32)1 << (u32)PinNumber);
	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			  XGPIOPS_INTDIS_OFFSET, IntrReg);
}

/****************************************************************************/
/**
*
* This function returns the interrupt enable status for a bank.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Bank is the bank number of the GPIO to operate on.
*		Valid values are 0-3 in Zynq and 0-5 in Zynq Ultrascale+ MP.
*
* @return	Enabled interrupt(s) in a 32-bit format. Bit positions with 1
*		indicate that the interrupt for that pin is enabled, bit
*		positions with 0 indicate that the interrupt for that pin is
*		disabled.
*
* @note		None.
*
*****************************************************************************/
u32 XGpioPs_IntrGetEnabled(const XGpioPs *InstancePtr, u8 Bank)
{
	u32 IntrMask;

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
        Xil_AssertNonvoid(Bank < InstancePtr->MaxBanks);
#ifdef versal
        if(InstancePtr->PmcGpio == TRUE) {
                Xil_AssertNonvoid(Bank != XGPIOPS_TWO);
        } else {
                Xil_AssertNonvoid((Bank !=XGPIOPS_ONE) && (Bank !=XGPIOPS_TWO));
        }
#endif

	IntrMask = XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				    ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				    XGPIOPS_INTMASK_OFFSET);
	return (~IntrMask);
}

/****************************************************************************/
/**
*
* This function returns whether interrupts are enabled for the specified pin.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Pin is the pin number for which the interrupt enable status
*		is to be known.
*		Valid values are 0-117 in Zynq and 0-173 in Zynq Ultrascale+ MP.
*
* @return
*		- TRUE if the interrupt is enabled.
*		- FALSE if the interrupt is disabled.
*
* @note		None.
*
*****************************************************************************/
u32 XGpioPs_IntrGetEnabledPin(const XGpioPs *InstancePtr, u32 Pin)
{
	u8 Bank;
	u8 PinNumber;
	u32 IntrReg;

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertNonvoid(Pin < InstancePtr->MaxPinNum);

	/* Get the Bank number and Pin number within the bank. */
#ifdef versal
	XGpioPs_GetBankPin(InstancePtr,(u8)Pin, &Bank, &PinNumber);
#else
	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);
#endif

	IntrReg  = XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				    ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				    XGPIOPS_INTMASK_OFFSET);

	return (((IntrReg & ((u32)1 << PinNumber)) != (u32)0)? FALSE : TRUE);
}

/****************************************************************************/
/**
*
* This function returns interrupt status read from Interrupt Status Register.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Bank is the bank number of the GPIO to operate on.
*		Valid values are 0-3 in Zynq and 0-5 in Zynq Ultrascale+ MP.
*
* @return	The value read from Interrupt Status Register.
*
* @note		None.
*
*****************************************************************************/
u32 XGpioPs_IntrGetStatus(const XGpioPs *InstancePtr, u8 Bank)
{
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
        Xil_AssertNonvoid(Bank < InstancePtr->MaxBanks);
#ifdef versal
        if(InstancePtr->PmcGpio == TRUE) {
                Xil_AssertNonvoid(Bank != XGPIOPS_TWO);
        } else {
                Xil_AssertNonvoid((Bank !=XGPIOPS_ONE) && (Bank !=XGPIOPS_TWO));
        }
#endif

	return XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				XGPIOPS_INTSTS_OFFSET);
}

/****************************************************************************/
/**
*
* This function returns interrupt enable status of the specified pin.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Pin is the pin number for which the interrupt enable status
*		is to be known.
*		Valid values are 0-117 in Zynq and 0-173 in Zynq Ultrascale+ MP.
*
* @return
*		- TRUE if the interrupt has occurred.
*		- FALSE if the interrupt has not occurred.
*
* @note		None.
*
*****************************************************************************/
u32 XGpioPs_IntrGetStatusPin(const XGpioPs *InstancePtr, u32 Pin)
{
	u8 Bank;
	u8 PinNumber;
	u32 IntrReg;

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertNonvoid(Pin < InstancePtr->MaxPinNum);

	/* Get the Bank number and Pin number within the bank. */
#ifdef versal
	XGpioPs_GetBankPin(InstancePtr,(u8)Pin, &Bank, &PinNumber);
#else
	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);
#endif

	IntrReg = XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				   ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				   XGPIOPS_INTSTS_OFFSET);

	return (((IntrReg & ((u32)1 << PinNumber)) != (u32)0)? TRUE : FALSE);
}

/****************************************************************************/
/**
*
* This function clears pending interrupt(s) with the provided mask. This
* function should be called after the software has serviced the interrupts
* that are pending.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Bank is the bank number of the GPIO to operate on.
*		Valid values are 0-3 in Zynq and 0-5 in Zynq Ultrascale+ MP.
* @param	Mask is the mask of the interrupts to be cleared. Bit positions
*		of 1 will be cleared. Bit positions of 0 will not change the
*		previous interrupt status.
*
* @note		None.
*
*****************************************************************************/
void XGpioPs_IntrClear(const XGpioPs *InstancePtr, u8 Bank, u32 Mask)
{
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid(Bank < InstancePtr->MaxBanks);
#ifdef versal
        if(InstancePtr->PmcGpio == TRUE) {
                Xil_AssertVoid(Bank != XGPIOPS_TWO);
        } else {
                Xil_AssertVoid((Bank !=XGPIOPS_ONE) && (Bank !=XGPIOPS_TWO));
        }
#endif

	/* Clear the currently pending interrupts. */
	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			  XGPIOPS_INTSTS_OFFSET, Mask);
}

/****************************************************************************/
/**
*
* This function clears the specified pending interrupt. This function should be
* called after the software has serviced the interrupts that are pending.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	Pin is the pin number for which the interrupt status is to be
*		cleared. Valid values are 0-117 in Zynq and 0-173 in Zynq Ultrascale+ MP.
*
* @note		None.
*
*****************************************************************************/
void XGpioPs_IntrClearPin(const XGpioPs *InstancePtr, u32 Pin)
{
	u8 Bank;
	u8 PinNumber;
	u32 IntrReg;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid(Pin < InstancePtr->MaxPinNum);

	/* Get the Bank number and Pin number within the bank. */
#ifdef versal
	XGpioPs_GetBankPin(InstancePtr,(u8)Pin, &Bank, &PinNumber);
#else
	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);
#endif

	/* Clear the specified pending interrupts. */
	IntrReg = XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				   ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				   XGPIOPS_INTSTS_OFFSET);

	IntrReg &= ((u32)1 << PinNumber);
	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			  XGPIOPS_INTSTS_OFFSET, IntrReg);
}

/****************************************************************************/
/**
*
* This function is used for setting the Interrupt Type, Interrupt Polarity and
* Interrupt On Any for the specified GPIO Bank pins.
*
* @param	InstancePtr is a pointer to an XGpioPs instance.
* @param	Bank is the bank number of the GPIO to operate on.
*		Valid values are 0-3 in Zynq and 0-5 in Zynq Ultrascale+ MP.
* @param	IntrType is the 32 bit mask of the interrupt type.
*		0 means Level Sensitive and 1 means Edge Sensitive.
* @param	IntrPolarity is the 32 bit mask of the interrupt polarity.
*		0 means Active Low or Falling Edge and 1 means Active High or
*		Rising Edge.
* @param	IntrOnAny is the 32 bit mask of the interrupt trigger for
*		edge triggered interrupts. 0 means trigger on single edge using
*		the configured interrupt polarity and 1 means  trigger on both
*		edges.
*
* @return	None.
*
* @note		This function is used for setting the interrupt related
*		properties of all the pins in the specified bank. The previous
*		state of the pins is not maintained.
*		To change the Interrupt properties of a single GPIO pin, use the
*		function XGpioPs_SetPinIntrType().
*
*****************************************************************************/
void XGpioPs_SetIntrType(const XGpioPs *InstancePtr, u8 Bank, u32 IntrType,
			  u32 IntrPolarity, u32 IntrOnAny)
{
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid(Bank < InstancePtr->MaxBanks);
#ifdef versal
        if(InstancePtr->PmcGpio == TRUE) {
                Xil_AssertVoid(Bank != XGPIOPS_TWO);
        } else {
                Xil_AssertVoid((Bank !=XGPIOPS_ONE) && (Bank !=XGPIOPS_TWO));
        }
#endif

	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			  XGPIOPS_INTTYPE_OFFSET, IntrType);

	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			  XGPIOPS_INTPOL_OFFSET, IntrPolarity);

	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			  XGPIOPS_INTANY_OFFSET, IntrOnAny);
}

/****************************************************************************/
/**
*
* This function is used for getting the Interrupt Type, Interrupt Polarity and
* Interrupt On Any for the specified GPIO Bank pins.
*
* @param	InstancePtr is a pointer to an XGpioPs instance.
* @param	Bank is the bank number of the GPIO to operate on.
*		Valid values are 0-3 in Zynq and 0-5 in Zynq Ultrascale+ MP.
* @param	IntrType returns the 32 bit mask of the interrupt type.
*		0 means Level Sensitive and 1 means Edge Sensitive.
* @param	IntrPolarity returns the 32 bit mask of the interrupt
*		polarity. 0 means Active Low or Falling Edge and 1 means
*		Active High or Rising Edge.
* @param	IntrOnAny returns the 32 bit mask of the interrupt trigger for
*		edge triggered interrupts. 0 means trigger on single edge using
*		the configured interrupt polarity and 1 means trigger on both
*		edges.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void XGpioPs_GetIntrType(const XGpioPs *InstancePtr, u8 Bank, u32 *IntrType,
			  u32 *IntrPolarity, u32 *IntrOnAny)

{
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid(Bank < InstancePtr->MaxBanks);
#ifdef versal
        if(InstancePtr->PmcGpio == TRUE) {
                Xil_AssertVoid(Bank != XGPIOPS_TWO);
        } else {
                Xil_AssertVoid((Bank !=XGPIOPS_ONE) && (Bank !=XGPIOPS_TWO));
        }
#endif

	*IntrType = XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				     ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				     XGPIOPS_INTTYPE_OFFSET);

	*IntrPolarity = XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
					 ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
					 XGPIOPS_INTPOL_OFFSET);

	*IntrOnAny = XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				      ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				      XGPIOPS_INTANY_OFFSET);
}

/****************************************************************************/
/**
*
* This function is used for setting the IRQ Type of a single GPIO pin.
*
* @param	InstancePtr is a pointer to an XGpioPs instance.
* @param	Pin is the pin number whose IRQ type is to be set.
*		Valid values are 0-117 in Zynq and 0-173 in Zynq Ultrascale+ MP.
* @param	IrqType is the IRQ type for GPIO Pin. Use XGPIOPS_IRQ_TYPE_*
*		defined in xgpiops.h to specify the IRQ type.
*
* @return	None.
*
* @note		None.
*
*****************************************************************************/
void XGpioPs_SetIntrTypePin(const XGpioPs *InstancePtr, u32 Pin, u8 IrqType)
{
	u32 IntrTypeReg;
	u32 IntrPolReg;
	u32 IntrOnAnyReg;
	u8 Bank;
	u8 PinNumber;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertVoid(Pin < InstancePtr->MaxPinNum);
	Xil_AssertVoid(IrqType <= XGPIOPS_IRQ_TYPE_LEVEL_LOW);

	/* Get the Bank number and Pin number within the bank. */
#ifdef versal
	XGpioPs_GetBankPin(InstancePtr,(u8)Pin, &Bank, &PinNumber);
#else
	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);
#endif

	IntrTypeReg = XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				       ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				       XGPIOPS_INTTYPE_OFFSET);

	IntrPolReg = XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				      ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				      XGPIOPS_INTPOL_OFFSET);

	IntrOnAnyReg = XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
					((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
					XGPIOPS_INTANY_OFFSET);

	switch (IrqType) {
		case XGPIOPS_IRQ_TYPE_EDGE_RISING:
			IntrTypeReg |= ((u32)1 << (u32)PinNumber);
			IntrPolReg |= ((u32)1 << (u32)PinNumber);
			IntrOnAnyReg &= ~((u32)1 << (u32)PinNumber);
			break;
		case XGPIOPS_IRQ_TYPE_EDGE_FALLING:
			IntrTypeReg |= ((u32)1 << (u32)PinNumber);
			IntrPolReg &= ~((u32)1 << (u32)PinNumber);
			IntrOnAnyReg &= ~((u32)1 << (u32)PinNumber);
			break;
		case XGPIOPS_IRQ_TYPE_EDGE_BOTH:
			IntrTypeReg |= ((u32)1 << (u32)PinNumber);
			IntrOnAnyReg |= ((u32)1 << (u32)PinNumber);
			break;
		case XGPIOPS_IRQ_TYPE_LEVEL_HIGH:
			IntrTypeReg &= ~((u32)1 << (u32)PinNumber);
			IntrPolReg |= ((u32)1 << (u32)PinNumber);
			break;
		case XGPIOPS_IRQ_TYPE_LEVEL_LOW:
			IntrTypeReg &= ~((u32)1 << (u32)PinNumber);
			IntrPolReg &= ~((u32)1 << (u32)PinNumber);
			break;
		default:
			/**< Default statement is added for MISRA C compliance. */
			break;
	}

	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			  XGPIOPS_INTTYPE_OFFSET, IntrTypeReg);

	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			  XGPIOPS_INTPOL_OFFSET, IntrPolReg);

	XGpioPs_WriteReg(InstancePtr->GpioConfig.BaseAddr,
			  ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
			  XGPIOPS_INTANY_OFFSET, IntrOnAnyReg);
}

/****************************************************************************/
/**
*
* This function returns the IRQ Type of a given GPIO pin.
*
* @param	InstancePtr is a pointer to an XGpioPs instance.
* @param	Pin is the pin number whose IRQ type is to be obtained.
*		Valid values are 0-117 in Zynq and 0-173 in Zynq Ultrascale+ MP.
*
* @return	None.
*
* @note		Use XGPIOPS_IRQ_TYPE_* defined in xgpiops.h for the IRQ type
*		returned by this function.
*
*****************************************************************************/
u8 XGpioPs_GetIntrTypePin(const XGpioPs *InstancePtr, u32 Pin)
{
	u32 IntrType;
	u32 IntrPol;
	u32 IntrOnAny;
	u8 Bank;
	u8 PinNumber;
	u8 IrqType;

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertNonvoid(Pin < InstancePtr->MaxPinNum);

	/* Get the Bank number and Pin number within the bank. */
#ifdef versal
	XGpioPs_GetBankPin(InstancePtr,(u8)Pin, &Bank, &PinNumber);
#else
	XGpioPs_GetBankPin((u8)Pin, &Bank, &PinNumber);
#endif

	IntrType = XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				    ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				    XGPIOPS_INTTYPE_OFFSET) & ((u32)1 << PinNumber);

	if (IntrType == ((u32)1 << PinNumber)) {

		IntrOnAny = XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				     ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				     XGPIOPS_INTANY_OFFSET) & ((u32)1 << PinNumber);

		IntrPol = XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				   ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				   XGPIOPS_INTPOL_OFFSET) & ((u32)1 << PinNumber);


		if (IntrOnAny == ((u32)1 << PinNumber)) {
			IrqType = XGPIOPS_IRQ_TYPE_EDGE_BOTH;
		} else if (IntrPol == ((u32)1 << PinNumber)) {
			IrqType = XGPIOPS_IRQ_TYPE_EDGE_RISING;
		} else {
			IrqType = XGPIOPS_IRQ_TYPE_EDGE_FALLING;
		}
	} else {

		IntrPol = XGpioPs_ReadReg(InstancePtr->GpioConfig.BaseAddr,
				   ((u32)(Bank) * XGPIOPS_REG_MASK_OFFSET) +
				   XGPIOPS_INTPOL_OFFSET) & ((u32)1 << PinNumber);

		if (IntrPol == ((u32)1 << PinNumber)) {
			IrqType = XGPIOPS_IRQ_TYPE_LEVEL_HIGH;
		} else {
			IrqType = XGPIOPS_IRQ_TYPE_LEVEL_LOW;
		}
	}

	return IrqType;
}

/*****************************************************************************/
/**
*
* This function sets the status callback function. The callback function is
* called by the  XGpioPs_IntrHandler when an interrupt occurs.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
* @param	CallBackRef is the upper layer callback reference passed back
*		when the callback function is invoked.
* @param	FuncPointer is the pointer to the callback function.
*
*
* @return	None.
*
* @note		The handler is called within interrupt context, so it should do
*		its work quickly and queue potentially time-consuming work to a
*		task-level thread.
*
******************************************************************************/
void XGpioPs_SetCallbackHandler(XGpioPs *InstancePtr, void *CallBackRef,
				 XGpioPs_Handler FuncPointer)
{
	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(FuncPointer != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	InstancePtr->Handler = FuncPointer;
	InstancePtr->CallBackRef = CallBackRef;
}

/*****************************************************************************/
/**
*
* This function is the interrupt handler for GPIO interrupts.It checks the
* interrupt status registers of all the banks to determine the actual bank in
* which an interrupt has been triggered. It then calls the upper layer callback
* handler set by the function XGpioPs_SetBankHandler(). The callback is called
* when an interrupt
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
*
* @return	None.
*
* @note		This function does not save and restore the processor context
*		such that the user must provide this processing.
*
******************************************************************************/
void XGpioPs_IntrHandler(const XGpioPs *InstancePtr)
{
	u8 Bank;
	u32 IntrStatus;
	u32 IntrEnabled;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	for (Bank = 0U; Bank < InstancePtr->MaxBanks; Bank++) {
#ifdef versal
		if(InstancePtr->PmcGpio == TRUE) {
			if(Bank == XGPIOPS_TWO) {
				continue;
			}
		} else {
			if((Bank == XGPIOPS_ONE) || (Bank == XGPIOPS_TWO)) {
				continue;
			}
		}
#endif
		IntrStatus = XGpioPs_IntrGetStatus(InstancePtr, Bank);
		IntrEnabled = XGpioPs_IntrGetEnabled(InstancePtr,Bank);
		if ((IntrStatus & IntrEnabled) != (u32)0) {
			XGpioPs_IntrClear(InstancePtr, Bank,
					(IntrStatus & IntrEnabled));
			InstancePtr->Handler(InstancePtr->
					CallBackRef, Bank,
					(IntrStatus & IntrEnabled));
		}
	}
}


/** @} */ 

/******************************************************************************
* Copyright (C) 2010 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xgpiops_selftest.c
* @addtogroup gpiops_v3_7
* @{
*
* This file contains a diagnostic self-test function for the XGpioPs driver.
*
* Read xgpiops.h file for more information.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a sv   01/18/10 First Release
* 3.00  kvn  02/13/15 Modified code for MISRA-C:2012 compliance.
* 3.5   sne  03/01/19 Fixes violations according to MISRAC-2012
*                     in safety mode and modified the code such as
*                     Use of mixed mode arithmetic,Declared the pointer param
*                     as Pointer to const,Casting operation to a pointer,
*                     Literal value requires a U suffix.
* </pre>
*
*****************************************************************************/

/***************************** Include Files ********************************/

#include "xstatus.h"

/************************** Constant Definitions ****************************/


/**************************** Type Definitions ******************************/

/***************** Macros (Inline Functions) Definitions ********************/

/************************** Variable Definitions ****************************/

/************************** Function Prototypes *****************************/

/*****************************************************************************/
/**
*
* This function runs a self-test on the GPIO driver/device. This function
* does a register read/write test on some of the Interrupt Registers.
*
* @param	InstancePtr is a pointer to the XGpioPs instance.
*
* @return
*		- XST_SUCCESS if the self-test passed.
* 		- XST_FAILURE otherwise.
*
*
******************************************************************************/
s32 XGpioPs_SelfTest(const XGpioPs *InstancePtr)
{
	s32 Status = XST_SUCCESS;
	u32 IntrEnabled;
	u32 CurrentIntrType = 0U;
	u32 CurrentIntrPolarity = 0U;
	u32 CurrentIntrOnAny = 0U;
	u32 IntrType = 0U;
	u32 IntrPolarity = 0U;
	u32 IntrOnAny = 0U;
	u32 IntrTestValue = 0x22U;

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/* Disable the Interrupts for Bank 0 . */
	IntrEnabled = XGpioPs_IntrGetEnabled(InstancePtr, XGPIOPS_BANK0);
	XGpioPs_IntrDisable(InstancePtr, XGPIOPS_BANK0, IntrEnabled);

	/*
	 * Get the Current Interrupt properties for Bank 0.
	 * Set them to a known value, read it back and compare.
	 */
	XGpioPs_GetIntrType(InstancePtr, XGPIOPS_BANK0, &CurrentIntrType,
			     &CurrentIntrPolarity, &CurrentIntrOnAny);

	XGpioPs_SetIntrType(InstancePtr, XGPIOPS_BANK0, IntrTestValue,
			     IntrTestValue, IntrTestValue);

	XGpioPs_GetIntrType(InstancePtr, XGPIOPS_BANK0, &IntrType,
			     &IntrPolarity, &IntrOnAny);

	if ((IntrType != IntrTestValue) && (IntrPolarity != IntrTestValue) &&
	    (IntrOnAny != IntrTestValue)) {

		Status = XST_FAILURE;
	}

	/*
	 * Restore the contents of all the interrupt registers modified in this
	 * test.
	 */
	XGpioPs_SetIntrType(InstancePtr, XGPIOPS_BANK0, CurrentIntrType,
			     CurrentIntrPolarity, CurrentIntrOnAny);

	XGpioPs_IntrEnable(InstancePtr, XGPIOPS_BANK0, IntrEnabled);

	return Status;
}
/** @} */ 

/******************************************************************************
* Copyright (C) 2010 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

/*****************************************************************************/
/**
*
* @file xgpiops_sinit.c
* @addtogroup gpiops_v3_7
* @{
*
* This file contains the implementation of the XGpioPs driver's static
* initialization functionality.
*
* @note		None.
*
* <pre>
*
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -----------------------------------------------
* 1.00a sv   01/15/10 First Release
* 3.00  kvn  02/13/15 Modified code for MISRA-C:2012 compliance.
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/


/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/
extern XGpioPs_Config XGpioPs_ConfigTable[XPAR_XGPIOPS_NUM_INSTANCES];

/*****************************************************************************/
/**
*
* This function looks for the device configuration based on the unique device
* ID. The table XGpioPs_ConfigTable[] contains the configuration information
* for each device in the system.
*
* @param	DeviceId is the unique device ID of the device being looked up.
*
* @return	A pointer to the configuration table entry corresponding to the
*		given device ID, or NULL if no match is found.
*
* @note		None.
*
******************************************************************************/
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
/** @} */ 
