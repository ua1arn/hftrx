/*
 * usb_core.h
 *
 *  Created on: Oct 13, 2019
 *      Author: gena
 */

#ifndef USB_USB_CORE_H_
#define USB_USB_CORE_H_

#include "hardware.h"
#include <stdint.h>
#include "usb200.h"
#include "usbch9.h"

#include  "usbd_ioreq.h"

#define USB_OTG_HS_MAX_PACKET_SIZE           512U
#define USB_OTG_FS_MAX_PACKET_SIZE           64U

#define USB_OTG_MAX_EP0_SIZE                 64U


#define USB_FUNCTION_bRequest                       (0xFF00u)       /* b15-8:bRequest */
#define USB_FUNCTION_bmRequestType                  (0x00FFu)       /* b7-0: bmRequestType */
#define USB_FUNCTION_bmRequestTypeDir               (0x0080u)       /* b7  : Data transfer direction - IN if non-zero */
#define USB_FUNCTION_bmRequestTypeType              (0x0060u)       /* b6-5: Type */
#define USB_FUNCTION_bmRequestTypeRecip             (0x001Fu)       /* b4-0: Recipient USB_RECIPIENT_MASK */

/** @defgroup USB_Core_Mode_ USB Core Mode
  * @{
  */
#define USB_OTG_MODE_DEVICE                    0U
#define USB_OTG_MODE_HOST                      1U
#define USB_OTG_MODE_DRD                       2U
/**
  * @}
  */

/** @defgroup USB_Core_Speed_   USB Core Speed
  * @{
  */
// Эти значения пишутся в регистр USB_OTG_DCFG после умножения на USB_OTG_DCFG_DSPD_0
#define USB_OTG_SPEED_HIGH                     0U	// 00: High speed
#define USB_OTG_SPEED_HIGH_IN_FULL             1U	// Full speed using HS
#define USB_OTG_SPEED_LOW                      2U	// Reserved
#define USB_OTG_SPEED_FULL                     3U	// Full speed using internal FS PHY
/**
  * @}
  */

/** @defgroup USB_Core_PHY_   USB Core PHY
  * @{
  */
#define USB_OTG_ULPI_PHY                       1U
#define USB_OTG_EMBEDDED_PHY                   2U
#define USB_OTG_HS_EMBEDDED_PHY                3U

//#if !defined  (USB_HS_PHYC_TUNE_VALUE)
  #define USB_HS_PHYC_TUNE_VALUE    0x00000F13U /*!< Value of USB HS PHY Tune */
//#endif /* USB_HS_PHYC_TUNE_VALUE */
/**
  * @}
  */
/**
  * @}
  */

/** @defgroup USB_EP0_MPS_  USB EP0 MPS
  * @{
  */
#define DEP0CTL_MPS_64                         0U
#define DEP0CTL_MPS_32                         1U
#define DEP0CTL_MPS_16                         2U
#define DEP0CTL_MPS_8                          3U
/**
  * @}
  */

/** @defgroup USB_EP_Speed_  USB EP Speed
  * @{
  */
#define EP_SPEED_LOW                           0U
#define EP_SPEED_FULL                          1U
#define EP_SPEED_HIGH                          2U

/**
  * @}
  */

#if CPUSTYLE_R7S721
	typedef struct st_usb20  USB_OTG_GlobalTypeDef;
#endif /* CPUSTYLE_R7S721 */

typedef USB_OTG_GlobalTypeDef	PCD_TypeDef;		/* processor peripherial */
typedef USB_OTG_GlobalTypeDef	HCD_TypeDef;	/* processor peripherial */


/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
  * @{
  */

/** @defgroup USBH_CORE
  * @brief This file handles the basic enumeration when a device is connected
  *          to the host.
  * @{
  */


/** @defgroup USBH_CORE_Private_Defines
  * @{
  */
#define USBH_ADDRESS_DEFAULT                     0
#define USBH_ADDRESS_ASSIGNED                    1
#define USBH_MPS_DEFAULT                         0x40


typedef struct _InterfaceAssocDescriptor
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint8_t	bFirstInterface;
  uint8_t   bInterfaceCount;       /* Number of Interfaces */
  uint8_t   bFunctionClass;
  uint8_t   bFunctionSubClass;
  uint8_t   bFunctionProtocol;
  uint8_t   iConfiguration;       /*Index of String Descriptor Describing this configuration */
} USBH_IfAssocDescTypeDef;

#ifndef USBH_MAX_PIPES_NBR
 #define USBH_MAX_PIPES_NBR                             15
#endif /* USBH_MAX_PIPES_NBR */

struct _USBH_HandleTypeDef;

#define USBHNPIPES 15



typedef enum {
  APPLICATION_IDLE = 0,
  APPLICATION_START,
  APPLICATION_READY,
  APPLICATION_DISCONNECT,
} ApplicationTypeDef;


HAL_StatusTypeDef USB_CoreReset(USB_OTG_GlobalTypeDef *USBx);

/**
  * @}
  */

/** @defgroup USB_STS_Defines_   USB STS Defines
  * @{
  */
#define STS_GOUT_NAK                           1U
#define STS_DATA_UPDT                          2U
#define STS_XFER_COMP                          3U
#define STS_SETUP_COMP                         4U
#define STS_SETUP_UPDT                         6U
/**
  * @}
  */

/** @defgroup HCFG_SPEED_Defines_   HCFG SPEED Defines
  * @{
  */
#define HCFG_30_60_MHZ                         0U
#define HCFG_48_MHZ                            1U
#define HCFG_6_MHZ                             2U
/**
  * @}
  */

/** @defgroup HPRT0_PRTSPD_SPEED_Defines_  HPRT0 PRTSPD SPEED Defines
  * @{
  */
#define HPRT0_PRTSPD_HIGH_SPEED                0U
#define HPRT0_PRTSPD_FULL_SPEED                1U
#define HPRT0_PRTSPD_LOW_SPEED                 2U
/**
  * @}
  */

#define HCCHAR_CTRL                            0U
#define HCCHAR_ISOC                            1U
#define HCCHAR_BULK                            2U
#define HCCHAR_INTR                            3U

#define HC_PID_DATA0                           0U
#define HC_PID_DATA2                           1U
#define HC_PID_DATA1                           2U
#define HC_PID_SETUP                           3U

#define GRXSTS_PKTSTS_IN                       2U
#define GRXSTS_PKTSTS_IN_XFER_COMP             3U
#define GRXSTS_PKTSTS_DATA_TOGGLE_ERR          5U
#define GRXSTS_PKTSTS_CH_HALTED                7U
/**
  * @}
  */
/* Exported macro ------------------------------------------------------------*/

/**
  * @}
  */

/** @defgroup PCD_PHY_Module PCD PHY Module
  * @{
  */
#define PCD_PHY_ULPI                 1U
#define PCD_PHY_EMBEDDED             2U
/**
  * @}
  */
#define USBD_HS_TRDT_VALUE           9U
#define USBD_FS_TRDT_VALUE           5U

uint32_t  USB_GetHostSpeed(USB_OTG_GlobalTypeDef *USBx);
uint_fast8_t USB_GetHostSpeedReady(USB_OTG_GlobalTypeDef *USBx);

#define  USB_DISABLE   0
#define  USB_ENABLE    1

#define ValBit(VAR,POS)                               (VAR & (1 << POS))
#define SetBit(VAR,POS)                               (VAR |= (1 << POS))
#define ClrBit(VAR,POS)                               (VAR &= ((1 << POS)^255))

extern const USBD_ClassTypeDef USBD_CLASS_UAC;
extern const USBD_ClassTypeDef USBD_CLASS_CDCACM;
extern const USBD_ClassTypeDef USBD_CLASS_DFU;
extern const USBD_ClassTypeDef USBD_CLASS_CDC_EEM;
extern const USBD_ClassTypeDef USBD_CLASS_RNDIS;

uint_fast16_t usbd_getuacinrtsmaxpacket(void);
uint_fast16_t usbd_getuacinmaxpacket(void);
uint_fast16_t usbd_getuacoutmaxpacket(void);


//HAL_StatusTypeDef USB_DoPing(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t ch_num);

//extern USBD_HandleTypeDef hUsbDevice;

#endif /* USB_USB_CORE_H_ */
