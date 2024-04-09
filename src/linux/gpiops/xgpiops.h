#ifndef XGPIOPS_H		/* prevent circular inclusions */
#define XGPIOPS_H		/**< by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

#if CPUSTYLE_XC7Z && LINUX_SUBSYSTEM

/***************************** Include Files *********************************/

#include <src/linux/gpiops/xgpiops_hw.h>

/************************** Constant Definitions *****************************/

/** @name Interrupt types
 *  @{
 * The following constants define the interrupt types that can be set for each
 * GPIO pin.
 */
#define XGPIOPS_IRQ_TYPE_EDGE_RISING	0x00U  /**< Interrupt on Rising edge */
#define XGPIOPS_IRQ_TYPE_EDGE_FALLING	0x01U  /**< Interrupt Falling edge */
#define XGPIOPS_IRQ_TYPE_EDGE_BOTH	0x02U  /**< Interrupt on both edges */
#define XGPIOPS_IRQ_TYPE_LEVEL_HIGH	0x03U  /**< Interrupt on high level */
#define XGPIOPS_IRQ_TYPE_LEVEL_LOW	0x04U  /**< Interrupt on low level */
/** @}*/

#define XGPIOPS_BANK_MAX_PINS		(u32)32 /**< Max pins in a GPIO bank */
#define XGPIOPS_BANK0			0x00U  /**< GPIO Bank 0 */
#define XGPIOPS_BANK1			0x01U  /**< GPIO Bank 1 */
#define XGPIOPS_BANK2			0x02U  /**< GPIO Bank 2 */
#define XGPIOPS_BANK3			0x03U  /**< GPIO Bank 3 */

#ifdef XPAR_PSU_GPIO_0_BASEADDR
#define XGPIOPS_BANK4			0x04U  /**< GPIO Bank 4 */
#define XGPIOPS_BANK5			0x05U  /**< GPIO Bank 5 */
#endif

#define XGPIOPS_MAX_BANKS_ZYNQMP		0x06U  /**< Max banks in a
										*	Zynq Ultrascale+ MP GPIO device
										*/
#define XGPIOPS_MAX_BANKS		0x04U  /**< Max banks in a Zynq GPIO device */

#define XGPIOPS_DEVICE_MAX_PIN_NUM_ZYNQMP	(u32)174 /**< Max pins in the
						  *	Zynq Ultrascale+ MP GPIO device
					      * 0 - 25,  Bank 0
					      * 26 - 51, Bank 1
					      *	52 - 77, Bank 2
					      *	78 - 109, Bank 3
					      *	110 - 141, Bank 4
					      *	142 - 173, Bank 5
					      */
#define XGPIOPS_DEVICE_MAX_PIN_NUM	(u32)118 /**< Max pins in the Zynq GPIO device
					      * 0 - 31,  Bank 0
					      * 32 - 53, Bank 1
					      *	54 - 85, Bank 2
					      *	86 - 117, Bank 3
					      */

/**************************** Type Definitions *******************************/

/****************************************************************************/
/**
 * This handler data type allows the user to define a callback function to
 * handle the interrupts for the GPIO device. The application using this
 * driver is expected to define a handler of this type, to support interrupt
 * driven mode. The handler executes in an interrupt context such that minimal
 * processing should be performed.
 *
 * @param	CallBackRef is a callback reference passed in by the upper layer
 *		when setting the callback functions for a GPIO bank. It is
 *		passed back to the upper layer when the callback is invoked. Its
 *		type is not important to the driver component, so it is a void
 *		pointer.
 * @param	Bank is the bank for which the interrupt status has changed.
 * @param	Status is the Interrupt status of the GPIO bank.
 *
 *****************************************************************************/
typedef void (*XGpioPs_Handler) (void *CallBackRef, u32 Bank, u32 Status);

/**
 * This typedef contains configuration information for a device.
 */
typedef struct {
	u16 DeviceId;		/**< Unique ID of device */
	UINTPTR BaseAddr;		/**< Register base address */
} XGpioPs_Config;

/**
 * The XGpioPs driver instance data. The user is required to allocate a
 * variable of this type for the GPIO device in the system. A pointer
 * to a variable of this type is then passed to the driver API functions.
 */
typedef struct {
	XGpioPs_Config GpioConfig;	/**< Device configuration */
	u32 IsReady;			/**< Device is initialized and ready */
	XGpioPs_Handler Handler;	/**< Status handlers for all banks */
	void *CallBackRef; 		/**< Callback ref for bank handlers */
	u32 Platform;			/**< Platform data */
	u32 MaxPinNum;			/**< Max pins in the GPIO device */
	u8 MaxBanks;			/**< Max banks in a GPIO device */
        u32 PmcGpio;                    /**< Flag for accessing PS GPIO for versal*/
} XGpioPs;

/************************** Variable Definitions *****************************/
extern XGpioPs_Config XGpioPs_ConfigTable[];

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/* Functions in xgpiops.c */
s32 XGpioPs_CfgInitialize(XGpioPs *InstancePtr, const XGpioPs_Config *ConfigPtr,
			   u32 EffectiveAddr);

/* Bank APIs in xgpiops.c */
u32 XGpioPs_Read(const XGpioPs *InstancePtr, u8 Bank);
void XGpioPs_Write(const XGpioPs *InstancePtr, u8 Bank, u32 Data);
void XGpioPs_SetDirection(const XGpioPs *InstancePtr, u8 Bank, u32 Direction);
u32 XGpioPs_GetDirection(const XGpioPs *InstancePtr, u8 Bank);
void XGpioPs_SetOutputEnable(const XGpioPs *InstancePtr, u8 Bank, u32 OpEnable);
u32 XGpioPs_GetOutputEnable(const XGpioPs *InstancePtr, u8 Bank);
#ifdef versal
void XGpioPs_GetBankPin(const XGpioPs *InstancePtr,u8 PinNumber,u8 *BankNumber, u8 *PinNumberInBank);
#else
void XGpioPs_GetBankPin(u8 PinNumber,u8 *BankNumber, u8 *PinNumberInBank);
#endif

/* Pin APIs in xgpiops.c */
u32 XGpioPs_ReadPin(const XGpioPs *InstancePtr, u32 Pin);
void XGpioPs_WritePin(const XGpioPs *InstancePtr, u32 Pin, u32 Data);
void XGpioPs_SetDirectionPin(const XGpioPs *InstancePtr, u32 Pin, u32 Direction);
u32 XGpioPs_GetDirectionPin(const XGpioPs *InstancePtr, u32 Pin);
void XGpioPs_SetOutputEnablePin(const XGpioPs *InstancePtr, u32 Pin, u32 OpEnable);
u32 XGpioPs_GetOutputEnablePin(const XGpioPs *InstancePtr, u32 Pin);

/* Functions in xgpiops_sinit.c */
XGpioPs_Config *XGpioPs_LookupConfig(u16 DeviceId);

#endif /* CPUSTYLE_XC7Z && LINUX_SUBSYSTEM */

#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
/** @} */
