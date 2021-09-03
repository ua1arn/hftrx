/**
  ******************************************************************************
  * @file    stm32h7xx_hal_hcd.h
  * @author  MCD Application Team
  * @brief   Header file of HCD HAL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef xxxxxx_HAL_EHCI_H
#define xxxxxx_HAL_EHCI_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32mp1xx_ll_usb.h"

#if defined (USB1_EHCI)
/** @addtogroup xxxxxx_HAL_Driver
  * @{
  */

/** @addtogroup HCD HCD
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup EHCI_Exported_Types HCD Exported Types
  * @{
  */

/** @defgroup EHCI_Exported_Types_Group1 HCD State Structure definition
  * @{
  */
typedef enum
{
  HAL_EHCI_STATE_RESET    = 0x00,
  HAL_EHCI_STATE_READY    = 0x01,
  HAL_EHCI_STATE_ERROR    = 0x02,
  HAL_EHCI_STATE_BUSY     = 0x03,
  HAL_EHCI_STATE_TIMEOUT  = 0x04
} EHCI_StateTypeDef;

typedef USB_EHCI_CapabilityTypeDef   EHCI_TypeDef;
typedef USB_OTG_CfgTypeDef      EHCI_InitTypeDef;
typedef USB_OTG_HCTypeDef       EHCI_HCTypeDef;
typedef USB_OTG_URBStateTypeDef EHCI_URBStateTypeDef;
typedef USB_OTG_HCStateTypeDef  EHCI_HCStateTypeDef;
/**
  * @}
  */

/** @defgroup EHCI_Exported_Types_Group2 HCD Handle Structure definition
  * @{
  */
#if (USE_HAL_EHCI_REGISTER_CALLBACKS == 1U)
typedef struct __EHCI_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_EHCI_REGISTER_CALLBACKS */
{
  EHCI_TypeDef               *Instance;  /*!< Register base address    */
  EHCI_InitTypeDef           Init;       /*!< HCD required parameters  */
  EHCI_HCTypeDef             hc[16];     /*!< Host channels parameters */
  HAL_LockTypeDef           Lock;       /*!< HCD peripheral status    */
  __IO EHCI_StateTypeDef     State;      /*!< HCD communication state  */
  __IO  uint32_t            ErrorCode;  /*!< HCD Error code           */
  void                      *pData;     /*!< Pointer Stack Handler    */
#if (USE_HAL_EHCI_REGISTER_CALLBACKS == 1U)
  void (* SOFCallback)(struct __EHCI_HandleTypeDef *hhcd);                               /*!< USB OTG HCD SOF callback                */
  void (* ConnectCallback)(struct __EHCI_HandleTypeDef *hhcd);                           /*!< USB OTG HCD Connect callback            */
  void (* DisconnectCallback)(struct __EHCI_HandleTypeDef *hhcd);                        /*!< USB OTG HCD Disconnect callback         */
  void (* PortEnabledCallback)(struct __EHCI_HandleTypeDef *hhcd);                       /*!< USB OTG HCD Port Enable callback        */
  void (* PortDisabledCallback)(struct __EHCI_HandleTypeDef *hhcd);                      /*!< USB OTG HCD Port Disable callback       */
  void (* HC_NotifyURBChangeCallback)(struct __EHCI_HandleTypeDef *hhcd, uint8_t chnum,
                                      EHCI_URBStateTypeDef urb_state);                   /*!< USB OTG HCD Host Channel Notify URB Change callback  */

  void (* MspInitCallback)(struct __EHCI_HandleTypeDef *hhcd);                           /*!< USB OTG HCD Msp Init callback           */
  void (* MspDeInitCallback)(struct __EHCI_HandleTypeDef *hhcd);                         /*!< USB OTG HCD Msp DeInit callback         */
#endif /* USE_HAL_EHCI_REGISTER_CALLBACKS */
} EHCI_HandleTypeDef;
/**
  * @}
  */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup EHCI_Exported_Constants HCD Exported Constants
  * @{
  */

/** @defgroup EHCI_Speed HCD Speed
  * @{
  */
#define EHCI_SPEED_HIGH               USBH_HS_SPEED
#define EHCI_SPEED_FULL               USBH_FSLS_SPEED
#define EHCI_SPEED_LOW                USBH_FSLS_SPEED

#define EHCI_DEVICE_SPEED_HIGH               0U
#define EHCI_DEVICE_SPEED_FULL               1U
#define EHCI_DEVICE_SPEED_LOW                2U

/**
  * @}
  */

/** @defgroup EHCI_PHY_Module HCD PHY Module
  * @{
  */
#define EHCI_PHY_ULPI                 1U
#define EHCI_PHY_EMBEDDED             2U
/**
  * @}
  */

/** @defgroup EHCI_Error_Code_definition HCD Error Code definition
  * @brief  HCD Error Code definition
  * @{
  */
#if (USE_HAL_EHCI_REGISTER_CALLBACKS == 1U)
#define  HAL_EHCI_ERROR_INVALID_CALLBACK                        (0x00000010U)    /*!< Invalid Callback error  */
#endif /* USE_HAL_EHCI_REGISTER_CALLBACKS */

/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup EHCI_Exported_Macros HCD Exported Macros
  *  @brief macros to handle interrupts and specific clock configurations
  * @{
  */
#define __HAL_EHCI_ENABLE(__HANDLE__)                   do { } while (0) //(void)USB_EnableGlobalInt ((__HANDLE__)->Instance)
#define __HAL_EHCI_DISABLE(__HANDLE__)                  do { } while (0) //(void)USB_DisableGlobalInt ((__HANDLE__)->Instance)

//#define __HAL_EHCI_GET_FLAG(__HANDLE__, __INTERRUPT__)      ((USB_ReadInterrupts((__HANDLE__)->Instance) & (__INTERRUPT__)) == (__INTERRUPT__))
//#define __HAL_EHCI_CLEAR_FLAG(__HANDLE__, __INTERRUPT__)    (((__HANDLE__)->Instance->GINTSTS) = (__INTERRUPT__))
//#define __HAL_EHCI_IS_INVALID_INTERRUPT(__HANDLE__)         (USB_ReadInterrupts((__HANDLE__)->Instance) == 0U)
//
//#define __HAL_EHCI_CLEAR_HC_INT(chnum, __INTERRUPT__)  (USBx_HC(chnum)->HCINT = (__INTERRUPT__))
//#define __HAL_EHCI_MASK_HALT_HC_INT(chnum)             (USBx_HC(chnum)->HCINTMSK &= ~USB_OTG_HCINTMSK_CHHM)
//#define __HAL_EHCI_UNMASK_HALT_HC_INT(chnum)           (USBx_HC(chnum)->HCINTMSK |= USB_OTG_HCINTMSK_CHHM)
//#define __HAL_EHCI_MASK_ACK_HC_INT(chnum)              (USBx_HC(chnum)->HCINTMSK &= ~USB_OTG_HCINTMSK_ACKM)
//#define __HAL_EHCI_UNMASK_ACK_HC_INT(chnum)            (USBx_HC(chnum)->HCINTMSK |= USB_OTG_HCINTMSK_ACKM)
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup EHCI_Exported_Functions HCD Exported Functions
  * @{
  */

/** @defgroup EHCI_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
HAL_StatusTypeDef HAL_EHCI_Init(EHCI_HandleTypeDef *hhcd);
HAL_StatusTypeDef HAL_EHCI_DeInit(EHCI_HandleTypeDef *hhcd);
HAL_StatusTypeDef HAL_EHCI_HC_Init(EHCI_HandleTypeDef *hhcd, uint8_t ch_num,
                                  uint8_t epnum, uint8_t dev_address,
                                  uint8_t speed, uint8_t ep_type, uint16_t mps);

HAL_StatusTypeDef HAL_EHCI_HC_Halt(EHCI_HandleTypeDef *hhcd, uint8_t ch_num);
void              HAL_EHCI_MspInit(EHCI_HandleTypeDef *hhcd);
void              HAL_EHCI_MspDeInit(EHCI_HandleTypeDef *hhcd);

#if (USE_HAL_EHCI_REGISTER_CALLBACKS == 1U)
/** @defgroup HAL_EHCI_Callback_ID_enumeration_definition HAL USB OTG HCD Callback ID enumeration definition
  * @brief  HAL USB OTG HCD Callback ID enumeration definition
  * @{
  */
typedef enum
{
  HAL_EHCI_SOF_CB_ID            = 0x01,       /*!< USB HCD SOF callback ID           */
  HAL_EHCI_CONNECT_CB_ID        = 0x02,       /*!< USB HCD Connect callback ID       */
  HAL_EHCI_DISCONNECT_CB_ID     = 0x03,       /*!< USB HCD Disconnect callback ID    */
  HAL_EHCI_PORT_ENABLED_CB_ID   = 0x04,       /*!< USB HCD Port Enable callback ID   */
  HAL_EHCI_PORT_DISABLED_CB_ID  = 0x05,       /*!< USB HCD Port Disable callback ID  */

  HAL_EHCI_MSPINIT_CB_ID        = 0x06,       /*!< USB HCD MspInit callback ID       */
  HAL_EHCI_MSPDEINIT_CB_ID      = 0x07        /*!< USB HCD MspDeInit callback ID     */

} HAL_EHCI_CallbackIDTypeDef;
/**
  * @}
  */

/** @defgroup HAL_EHCI_Callback_pointer_definition HAL USB OTG HCD Callback pointer definition
  * @brief  HAL USB OTG HCD Callback pointer definition
  * @{
  */

typedef void (*pEHCI_CallbackTypeDef)(EHCI_HandleTypeDef *hhcd);                   /*!< pointer to a common USB OTG HCD callback function  */
typedef void (*pEHCI_HC_NotifyURBChangeCallbackTypeDef)(EHCI_HandleTypeDef *hhcd,
                                                       uint8_t epnum,
                                                       EHCI_URBStateTypeDef urb_state);   /*!< pointer to USB OTG HCD host channel  callback */
/**
  * @}
  */

HAL_StatusTypeDef HAL_EHCI_RegisterCallback(EHCI_HandleTypeDef *hhcd,
                                           HAL_EHCI_CallbackIDTypeDef CallbackID,
                                           pEHCI_CallbackTypeDef pCallback);

HAL_StatusTypeDef HAL_EHCI_UnRegisterCallback(EHCI_HandleTypeDef *hhcd,
                                             HAL_EHCI_CallbackIDTypeDef CallbackID);

HAL_StatusTypeDef HAL_EHCI_RegisterHC_NotifyURBChangeCallback(EHCI_HandleTypeDef *hhcd,
                                                             pEHCI_HC_NotifyURBChangeCallbackTypeDef pCallback);

HAL_StatusTypeDef HAL_EHCI_UnRegisterHC_NotifyURBChangeCallback(EHCI_HandleTypeDef *hhcd);
#endif /* USE_HAL_EHCI_REGISTER_CALLBACKS */
/**
  * @}
  */

/* I/O operation functions  ***************************************************/
/** @addtogroup EHCI_Exported_Functions_Group2 Input and Output operation functions
  * @{
  */
HAL_StatusTypeDef HAL_EHCI_HC_SubmitRequest(EHCI_HandleTypeDef *hhcd, uint8_t ch_num,
                                           uint8_t direction, uint8_t ep_type,
                                           uint8_t token, uint8_t *pbuff,
                                           uint16_t length, uint8_t do_ping);

/* Non-Blocking mode: Interrupt */
void HAL_EHCI_IRQHandler(EHCI_HandleTypeDef *hhcd);
void HAL_EHCI_SOF_Callback(EHCI_HandleTypeDef *hhcd);
void HAL_EHCI_Connect_Callback(EHCI_HandleTypeDef *hhcd);
void HAL_EHCI_Disconnect_Callback(EHCI_HandleTypeDef *hhcd);
void HAL_EHCI_PortEnabled_Callback(EHCI_HandleTypeDef *hhcd);
void HAL_EHCI_PortDisabled_Callback(EHCI_HandleTypeDef *hhcd);
void HAL_EHCI_HC_NotifyURBChange_Callback(EHCI_HandleTypeDef *hhcd, uint8_t chnum,
                                         EHCI_URBStateTypeDef urb_state);
/**
  * @}
  */

/* Peripheral Control functions  **********************************************/
/** @addtogroup EHCI_Exported_Functions_Group3 Peripheral Control functions
  * @{
  */
HAL_StatusTypeDef HAL_EHCI_ResetPort(EHCI_HandleTypeDef *hhcd);
HAL_StatusTypeDef HAL_EHCI_ResetPort2(EHCI_HandleTypeDef *hhcd, uint8_t resetActiveState);
HAL_StatusTypeDef HAL_EHCI_Start(EHCI_HandleTypeDef *hhcd);
HAL_StatusTypeDef HAL_EHCI_Stop(EHCI_HandleTypeDef *hhcd);
/**
  * @}
  */

/* Peripheral State functions  ************************************************/
/** @addtogroup EHCI_Exported_Functions_Group4 Peripheral State functions
  * @{
  */
EHCI_StateTypeDef        HAL_EHCI_GetState(EHCI_HandleTypeDef *hhcd);
EHCI_URBStateTypeDef     HAL_EHCI_HC_GetURBState(EHCI_HandleTypeDef *hhcd, uint8_t chnum);
EHCI_HCStateTypeDef      HAL_EHCI_HC_GetState(EHCI_HandleTypeDef *hhcd, uint8_t chnum);
uint32_t                HAL_EHCI_HC_GetXferCount(EHCI_HandleTypeDef *hhcd, uint8_t chnum);
uint32_t                HAL_EHCI_GetCurrentFrame(EHCI_HandleTypeDef *hhcd);
uint32_t                HAL_EHCI_GetCurrentSpeed(EHCI_HandleTypeDef *hhcd);
uint_fast8_t 			HAL_EHCI_GetCurrentSpeedReady(EHCI_HandleTypeDef *hhcd);

/**
  * @}
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @defgroup EHCI_Private_Macros HCD Private Macros
  * @{
  */
/**
  * @}
  */
/* Private functions prototypes ----------------------------------------------*/

/**
 * @}
 */
/**
 * @}
 */
#endif /* defined (USB1_EHCI) */

#ifdef __cplusplus
}
#endif

#endif /* xxxxxx_HAL_EHCI_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
