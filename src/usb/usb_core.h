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


	/*---------- -----------*/
//#define USBD_MAX_NUM_INTERFACES     7	// ?
/*---------- -----------*/
//#define USBD_MAX_NUM_CONFIGURATION     3
/*---------- -----------*/
#define USBD_DEBUG_LEVEL     0
/*---------- -----------*/
#define USBD_LPM_ENABLED     0
/*---------- -----------*/
#define USBD_SELF_POWERED     1

/*----------   -----------*/
#define USBH_MAX_NUM_ENDPOINTS      8

/*----------   -----------*/
#define USBH_MAX_NUM_INTERFACES      10

/*----------   -----------*/
#define USBH_MAX_NUM_CONFIGURATION      1

/*----------   -----------*/
#define USBH_KEEP_CFG_DESCRIPTOR      1

/*----------   -----------*/
#define USBH_MAX_NUM_SUPPORTED_CLASS      1

/*----------   -----------*/
#define USBH_MAX_SIZE_CONFIGURATION      1024

/*----------   -----------*/
#define USBH_MAX_DATA_BUFFER      1024

/*----------   -----------*/
#define USBH_DEBUG_LEVEL      0

/*----------   -----------*/
#define USBH_USE_OS      0

#define USB_OTG_HS_MAX_PACKET_SIZE           512U
#define USB_OTG_FS_MAX_PACKET_SIZE           64U

#define USB_OTG_MAX_EP0_SIZE                 64U


#define USB_FUNCTION_bRequest                       (0xFF00u)       /* b15-8:bRequest */
#define USB_FUNCTION_bmRequestType                  (0x00FFu)       /* b7-0: bmRequestType */
#define USB_FUNCTION_bmRequestTypeDir               (0x0080u)       /* b7  : Data transfer direction - IN if non-zero */
#define USB_FUNCTION_bmRequestTypeType              (0x0060u)       /* b6-5: Type */
#define USB_FUNCTION_bmRequestTypeRecip             (0x001Fu)       /* b4-0: Recipient USB_RECIPIENT_MASK */


/*  Device Status */
#define USBD_STATE_DEFAULT                                1
#define USBD_STATE_ADDRESSED                              2
#define USBD_STATE_CONFIGURED                             3
#define USBD_STATE_SUSPENDED                              4


/*  EP0 State */
#define USBD_EP0_IDLE                                     0
#define USBD_EP0_SETUP                                    1
#define USBD_EP0_DATA_IN                                  2
#define USBD_EP0_DATA_OUT                                 3
#define USBD_EP0_STATUS_IN                                4
#define USBD_EP0_STATUS_OUT                               5
#define USBD_EP0_STALL                                    6

#define USBD_EP_TYPE_CTRL                                 0
#define USBD_EP_TYPE_ISOC                                 1
#define USBD_EP_TYPE_BULK                                 2
#define USBD_EP_TYPE_INTR                                 3

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

/** @defgroup USB_Core_Phy_Frequency_   USB Core Phy Frequency
  * @{
  */
#define DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ     (0 << 1)
#define DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ     (1 << 1)
#define DSTS_ENUMSPD_LS_PHY_6MHZ               (2 << 1)
#define DSTS_ENUMSPD_FS_PHY_48MHZ              (3 << 1)
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

#define USB_FALSE    0
#define USB_TRUE     (!USB_FALSE)


/** @defgroup USBD_DEF_Exported_TypesDefinitions
  * @{
  */

typedef  struct  usb_setup_req
{

    uint_fast8_t   bmRequest;
    uint_fast8_t   bRequest;
    uint_fast16_t  wValue;
    uint_fast16_t  wIndex;
    uint_fast16_t  wLength;
} USBD_SetupReqTypedef;

struct _USBD_HandleTypeDef;


/* Following USB Device Speed */
typedef enum
{
  USBD_SPEED_HIGH  = 0,
  USBD_SPEED_FULL  = 1,
  USBD_SPEED_LOW   = 2,
} USBD_SpeedTypeDef;

/* Following USB Device status */
typedef enum {
  USBD_OK   = 0,
  USBD_BUSY,
  USBD_FAIL,
} USBD_StatusTypeDef;

typedef struct _Device_cb
{
	void  				(*ColdInit)         (void);	/* вызывается при запрещённых прерываниях. */
	USBD_StatusTypeDef  (*Init)             (struct _USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx);
	USBD_StatusTypeDef  (*DeInit)           (struct _USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx);
	/* Control Endpoints*/
	USBD_StatusTypeDef  (*Setup)            (struct _USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef  *req);
	USBD_StatusTypeDef  (*EP0_TxSent)       (struct _USBD_HandleTypeDef *pdev );
	USBD_StatusTypeDef  (*EP0_RxReady)      (struct _USBD_HandleTypeDef *pdev );
	/* Class Specific Endpoints*/
	USBD_StatusTypeDef  (*DataIn)           (struct _USBD_HandleTypeDef *pdev, uint_fast8_t epnum);
	USBD_StatusTypeDef  (*DataOut)          (struct _USBD_HandleTypeDef *pdev, uint_fast8_t epnum);
	USBD_StatusTypeDef  (*SOF)              (struct _USBD_HandleTypeDef *pdev);
	USBD_StatusTypeDef  (*IsoINIncomplete)  (struct _USBD_HandleTypeDef *pdev, uint_fast8_t epnum);
	USBD_StatusTypeDef  (*IsoOUTIncomplete) (struct _USBD_HandleTypeDef *pdev, uint_fast8_t epnum);

} USBD_ClassTypeDef;


/**
  * @brief  HAL Status structures definition
  */
typedef enum
{
  HAL_OK       = 0x00U,
  HAL_ERROR    = 0x01U,
  HAL_BUSY     = 0x02U,
  HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;

/**
  * @brief  PCD State structure definition
  */
typedef enum
{
  HAL_PCD_STATE_RESET   = 0x00U,
  HAL_PCD_STATE_READY   = 0x01U,
  HAL_PCD_STATE_ERROR   = 0x02U,
  HAL_PCD_STATE_BUSY    = 0x03U,
  HAL_PCD_STATE_TIMEOUT = 0x04U
} PCD_StateTypeDef;

/* Device LPM suspend state */
typedef enum
{
  LPM_L0 = 0x00U, /* on */
  LPM_L1 = 0x01U, /* LPM L1 sleep */
  LPM_L2 = 0x02U, /* suspend */
  LPM_L3 = 0x03U, /* off */
} PCD_LPM_StateTypeDef;


/**
  * @brief  USB Mode definition
  */
typedef enum
{
   USB_OTG_DEVICE_MODE  = 0U,
   USB_OTG_HOST_MODE    = 1U,
   USB_OTG_DRD_MODE     = 2U

} USB_OTG_ModeTypeDef;


/**
  * @brief  URB States definition
  */
typedef enum {
  URB_IDLE = 0,
  URB_DONE,
  URB_NOTREADY,
  URB_NYET,
  URB_ERROR,
  URB_STALL

} USB_OTG_URBStateTypeDef;

/**
  * @brief  Host channel States  definition
  */
typedef enum {
  HC_IDLE = 0,
  HC_XFRC,
  HC_HALTED,
  HC_NAK,
  HC_NYET,
  HC_STALL,
  HC_XACTERR,
  HC_BBLERR,
  HC_DATATGLERR

} USB_OTG_HCStateTypeDef;

/**
  * @brief  PCD Initialization Structure definition
  */
typedef struct
{
  uint32_t dev_endpoints;        /*!< Device Endpoints number.
                                      This parameter depends on the used USB core.
                                      This parameter must be a number between Min_Data = 1 and Max_Data = 15 */

  uint32_t Host_channels;        /*!< Host Channels number.
                                      This parameter Depends on the used USB core.
                                      This parameter must be a number between Min_Data = 1 and Max_Data = 15 */

  uint32_t pcd_speed;                /*!< USB Core speed. PCD_SPEED_xxx
                                      This parameter can be any value of @ref USB_Core_Speed_                */

  uint32_t dma_enable;           /*!< Enable or disable of the USB embedded DMA.                             */

#if 0
  // нигде не используется (только устанавливается)
  uint32_t ep0_mps;              /*!< Set the Endpoint 0 Max Packet size.
                                      This parameter can be any value of @ref USB_EP0_MPS_                   */

#endif

  uint32_t phy_itface;           /*!< Select the used PHY interface.
                                      This parameter can be any value of @ref USB_Core_PHY_                  */

  uint32_t Sof_enable;           /*!< Enable or disable the output of the SOF signal.                        */

  uint32_t low_power_enable;     /*!< Enable or disable the low power mode.                                  */

  uint32_t battery_charging_enable; /*!< Enable or disable Battery charging.                                 */

  uint32_t lpm_enable;           /*!< Enable or disable Link Power Management.                               */

  uint32_t vbus_sensing_enable;  /*!< Enable or disable the VBUS Sensing feature.                            */

  uint32_t use_dedicated_ep1;    /*!< Enable or disable the use of the dedicated EP1 interrupt.              */

  uint32_t use_external_vbus;    /*!< Enable or disable the use of the external VBUS.                        */

} USB_OTG_CfgTypeDef;

// Device controller endpoint
typedef struct
{
  uint_fast8_t   num;            /*!< Endpoint number
                                This parameter must be a number between Min_Data = 1 and Max_Data = 15    */

  uint_fast8_t   is_in;          /*!< Endpoint direction
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1     */

  uint_fast8_t   is_stall;       /*!< Endpoint stall condition
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1     */

  uint_fast8_t   type;           /*!< Endpoint type
                                 This parameter can be any value of @ref USB_EP_Type_                     */

#if 0
  // нигде не используется
  uint8_t   data_pid_start; /*!< Initial data PID
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1     */
  // нигде не используется
  uint8_t   even_odd_frame; /*!< IFrame parity
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 1    */
#endif

#if CPUSTYLE_R7S721

  // RENESAS specific field
  //uint_fast8_t	pipe_num;

#elif CPUSTYLE_STM32F || CPUSTYLE_STM32MP1

  // STM32 specific field
  uint_fast8_t  tx_fifo_num;    /*!< Transmission FIFO number
                                 This parameter must be a number between Min_Data = 1 and Max_Data = 15   */

#endif /* CPUSTYLE_R7S721 */

  uint_fast32_t  maxpacket;      /*!< Endpoint Max packet size
                                 This parameter must be a number between Min_Data = 0 and Max_Data = 64KB */

  uint8_t   *xfer_buff;     /*!< Pointer to transfer buffer                                               */

  uintptr_t  dma_addr;       /*!< 32 bits aligned transfer buffer address                                  */

  uint_fast32_t  xfer_len;       /*!< Current transfer length                                                  */

  uint_fast32_t  xfer_count;     /*!< Partial transfer length in case of multi packet transfer                 */

} USB_OTG_EPTypeDef;

// Host controller endpoint
typedef struct
{
  uint8_t   dev_addr;     /*!< USB device address.
                                This parameter must be a number between Min_Data = 1 and Max_Data = 255    */

  uint8_t   ch_num;        /*!< Host channel number.
                                This parameter must be a number between Min_Data = 1 and Max_Data = 15     */

  uint8_t   ep_num;        /*!< Endpoint number.
                                This parameter must be a number between Min_Data = 1 and Max_Data = 15     */

  uint8_t   ep_is_in;      /*!< Endpoint direction
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1      */

  uint8_t   usbh_otg_speed;         /*!< USB Host speed. USB_OTG_SPEED_
                                This parameter can be any value of @ref USB_Core_Speed_                    */

  uint8_t   do_ping;       /*!< Enable or disable the use of the PING protocol for HS mode.                */

  uint8_t   process_ping;  /*!< Execute the PING protocol for HS mode.                                     */

  uint8_t   ep_type;       /*!< Endpoint Type.
                                This parameter can be any value of @ref USB_EP_Type_                       */

  uint16_t  max_packet;    /*!< Endpoint Max packet size.
                                This parameter must be a number between Min_Data = 0 and Max_Data = 64KB   */

  uint8_t   data_pid;      /*!< Initial data PID.
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1      */

  uint8_t   *xfer_buff;    /*!< Pointer to transfer buffer.                                                */

  uint_fast32_t  xfer_len;      /*!< Current transfer length.                                                   */

  uint_fast32_t  xfer_count;    /*!< Partial transfer length in case of multi packet transfer.                  */

  uint8_t   toggle_in;     /*!< IN transfer current toggle flag.
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1      */

  uint8_t   toggle_out;    /*!< OUT transfer current toggle flag
                                This parameter must be a number between Min_Data = 0 and Max_Data = 1      */

  uintptr_t  dma_addr;      /*!< 32 bits aligned transfer buffer address.                                   */

  uint32_t  ErrCnt;        /*!< Host channel error count.*/

  USB_OTG_URBStateTypeDef  urb_state;  /*!< URB state.
                                           This parameter can be any value of @ref USB_OTG_URBStateTypeDef */

  USB_OTG_HCStateTypeDef   state;     /*!< Host Channel state.
                                           This parameter can be any value of @ref USB_OTG_HCStateTypeDef  */

} USB_OTG_HCTypeDef;

/**
  * @brief  HAL Lock structures definition
  */
typedef enum
{
  HAL_UNLOCKED = 0x00,
  HAL_LOCKED   = 0x01
} HAL_LockTypeDef;

#if CPUSTYLE_R7S721
	typedef struct st_usb20  USB_OTG_GlobalTypeDef;
#endif /* CPUSTYLE_R7S721 */

typedef USB_OTG_GlobalTypeDef	PCD_TypeDef;		/* processor peripherial */
typedef USB_OTG_GlobalTypeDef	HCD_TypeDef;	/* processor peripherial */

typedef USB_OTG_CfgTypeDef     PCD_InitTypeDef;
typedef USB_OTG_EPTypeDef      PCD_EPTypeDef;

/**
* @brief  PCD Handle Structure definition
*/
typedef USBALIGN_BEGIN struct
{
	PCD_TypeDef             *Instance;   /*!< Register base address              */
	PCD_InitTypeDef         Init;       /*!< PCD required parameters            */
	PCD_EPTypeDef           IN_ep[15];  /*!< IN endpoint parameters             */
	PCD_EPTypeDef           OUT_ep[15]; /*!< OUT endpoint parameters            */
	HAL_LockTypeDef         Lock;       /*!< PCD peripheral status              */
	volatile PCD_StateTypeDef State;      /*!< PCD communication state            */
	USBALIGN_BEGIN uint32_t PSetup [12] USBALIGN_END;  /*!< Setup packet buffer                */

	#if CPUSTYLE_STM32F4XX || CPUSTYLE_STM32F7XX || CPUSTYLE_STM32H7XX || CPUSTYLE_R7S721 || CPUSTYLE_STM32MP1

		PCD_LPM_StateTypeDef    LPM_State;    /*!< LPM State                          */
		uint32_t                BESL;
		uint32_t                lpm_active;   /*!< Enable or disable the Link Power Management .
								This parameter can be set to USB_ENABLE or USB_DISABLE */
	#endif

  uint32_t battery_charging_active;     /*!< Enable or disable Battery charging.        */
  uint_fast8_t            run_later_ctrl_comp;	// Renesas hardware specific item

  void                    * pData;       /*!< Pointer to upper stack Handler */
} USBALIGN_END PCD_HandleTypeDef;

HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeInit (PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd);


/* USB Device handle structure */
typedef USBALIGN_BEGIN struct
{
  USBALIGN_BEGIN uint8_t	epstatus [32] USBALIGN_END;	// used 2 elements
  uint_fast32_t                total_length;
  uint_fast32_t                rem_length;
  uint_fast32_t                maxpacket;
} USBALIGN_END USBD_EndpointTypeDef;

#define USBD_MAX_NUM_CLASSES 16

/* USB Device handle structure */
typedef USBALIGN_BEGIN struct _USBD_HandleTypeDef
{
  USBALIGN_BEGIN uint8_t  dev_config [32] USBALIGN_END; // used 1 elementt
  USBALIGN_BEGIN uint8_t  dev_default_config [32] USBALIGN_END;	// used 1 enement
  USBALIGN_BEGIN uint8_t  dev_config_status [32] USBALIGN_END;	// used two elements

  USBD_SpeedTypeDef       dev_speed; 	// USBD_SPEED_xxx
  USBD_EndpointTypeDef    ep_in [15];
  USBD_EndpointTypeDef    ep_out [15];
  uint_fast32_t           ep0_state;
  uint_fast32_t           ep0_data_len;	// from setup stage
  uint_fast8_t            dev_state;
  uint_fast8_t            dev_old_state;
  uint_fast8_t            dev_address;
  //uint_fast8_t          dev_connection_status;
  uint_fast8_t            dev_test_mode;
  uint_fast32_t           dev_remote_wakeup;

  USBD_SetupReqTypedef    request;
  //USBD_DescriptorsTypeDef *pDesc;
  uint_fast8_t			nClasses;
  const USBD_ClassTypeDef       *pClasses [USBD_MAX_NUM_CLASSES];
  //void                    *pClassData [USBD_MAX_NUM_CLASSES];
  //void                    *pUserData;
  void                    *pData;  // PCD_HandleTypeDef*
} USBALIGN_END USBD_HandleTypeDef;


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

typedef union
{
  uint16_t w;
  struct BW
  {
    uint8_t msb;
    uint8_t lsb;
  }
  bw;
} uint16_t_uint8_t;


typedef union _USB_Setup
{
  uint32_t d8[2];

  struct _SetupPkt_Struc
  {
    uint8_t           bmRequestType;
    uint8_t           bRequest;
    uint16_t_uint8_t  wValue;
    uint16_t_uint8_t  wIndex;
    uint16_t_uint8_t  wLength;
  } b;
} USB_Setup_TypeDef;

typedef  struct  _DescHeader
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
} USBH_DescHeader_t;

typedef struct _DeviceDescriptor
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint16_t  bcdUSB;        /* USB Specification Number which device complies too */
  uint8_t   bDeviceClass;
  uint8_t   bDeviceSubClass;
  uint8_t   bDeviceProtocol;
  /* If equal to Zero, each interfacei specifies its own class
  code if equal to 0xFF, the class code is vendor specified.
  Otherwise field is valid Class Code.*/
  uint8_t   bMaxPacketSize;
  uint16_t  idVendor;      /* Vendor ID (Assigned by USB Org) */
  uint16_t  idProduct;     /* Product ID (Assigned by Manufacturer) */
  uint16_t  bcdDevice;     /* Device Release Number */
  uint8_t   iManufacturer;  /* Index of Manufacturer String Descriptor */
  uint8_t   iProduct;       /* Index of Product String Descriptor */
  uint8_t   iSerialNumber;  /* Index of Serial Number String Descriptor */
  uint8_t   bNumConfigurations; /* Number of Possible Configurations */
} USBH_DevDescTypeDef;

typedef struct _EndpointDescriptor
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint8_t   bEndpointAddress;   /* indicates what endpoint this descriptor is describing */
  uint8_t   bmAttributes;       /* specifies the transfer type. */
  uint16_t  wMaxPacketSize;    /* Maximum Packet Size this endpoint is capable of sending or receiving */
  uint8_t   bInterval;          /* is used to specify the polling interval of certain transfers. */
}
USBH_EpDescTypeDef;

typedef struct _InterfaceDescriptor
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bInterfaceNumber;
  uint8_t bAlternateSetting;    /* Value used to select alternative setting */
  uint8_t bNumEndpoints;        /* Number of Endpoints used for this interfacei */
  uint8_t bInterfaceClass;      /* Class Code (Assigned by USB Org) */
  uint8_t bInterfaceSubClass;   /* Subclass Code (Assigned by USB Org) */
  uint8_t bInterfaceProtocol;   /* Protocol Code */
  uint8_t iInterface;           /* Index of String Descriptor Describing this interfacei */
  USBH_EpDescTypeDef               Ep_Desc[USBH_MAX_NUM_ENDPOINTS];
}
USBH_InterfaceDescTypeDef;


typedef struct _ConfigurationDescriptor
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint16_t  wTotalLength;        /* Total Length of Data Returned */
  uint8_t   bNumInterfaces;       /* Number of Interfaces */
  uint8_t   bConfigurationValue;  /* Value to use as an argument to select this configuration*/
  uint8_t   iConfiguration;       /*Index of String Descriptor Describing this configuration */
  uint8_t   bmAttributes;         /* D7 Bus Powered , D6 Self Powered, D5 Remote Wakeup , D4..0 Reserved (0)*/
  uint8_t   bMaxPower;            /*Maximum Power Consumption */
  USBH_InterfaceDescTypeDef        Itf_Desc[USBH_MAX_NUM_INTERFACES];
} USBH_CfgDescTypeDef;


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

/* Following USB Host status */
typedef enum
{
  USBH_OK   = 0,
  USBH_BUSY,
  USBH_FAIL,
  USBH_NOT_SUPPORTED,
  USBH_UNRECOVERED_ERROR,
  USBH_ERROR_SPEED_UNKNOWN,
} USBH_StatusTypeDef;



/* Following states are used for gState */
typedef enum
{
	HOST_IDLE = 0,
	HOST_DEV_BUS_RESET_ON,
	HOST_DEV_BUS_RESET_OFF,
	HOST_DEV_WAIT_FOR_ATTACHMENT,
	HOST_DEV_BEFORE_ATTACHED,
	HOST_DEV_ATTACHED,
	HOST_DEV_DISCONNECTED,		// 6
	HOST_DETECT_DEVICE_SPEED,
	HOST_ENUMERATION,			// 8
	HOST_CLASS_REQUEST,
	HOST_INPUT,
	HOST_SET_CONFIGURATION,
	HOST_CHECK_CLASS,
	HOST_CLASS,
	HOST_SUSPENDED,
	HOST_ABORT_STATE,
	HOST_DELAY
} HOST_StateTypeDef;

/* Following states are used for EnumerationState */
typedef enum
{
  ENUM_IDLE = 0,
  ENUM_GET_FULL_DEV_DESC,
  ENUM_SET_ADDR,
  ENUM_SET_ADDR2,
  ENUM_GET_CFG_DESC,
  ENUM_GET_FULL_CFG_DESC,
  ENUM_GET_MFC_STRING_DESC,
  ENUM_GET_PRODUCT_STRING_DESC,
  ENUM_GET_SERIALNUM_STRING_DESC,
  ENUM_DELAY,
} ENUM_StateTypeDef;

/* Following states are used for CtrlXferStateMachine */
typedef enum
{
  CTRL_IDLE =0,
  CTRL_SETUP,
  CTRL_SETUP_WAIT,
  CTRL_DATA_IN,
  CTRL_DATA_IN_WAIT,
  CTRL_DATA_OUT,
  CTRL_DATA_OUT_WAIT,
  CTRL_STATUS_IN,
  CTRL_STATUS_IN_WAIT,
  CTRL_STATUS_OUT,
  CTRL_STATUS_OUT_WAIT,
  CTRL_ERROR,
  CTRL_STALLED,
  CTRL_COMPLETE
} CTRL_StateTypeDef;


/* Following states are used for RequestState */
typedef enum
{
  CMD_IDLE =0,
  CMD_SEND,
  CMD_WAIT
} CMD_StateTypeDef;

typedef enum {
  USBH_URB_IDLE = 0,
  USBH_URB_DONE,
  USBH_URB_NOTREADY,
  USBH_URB_NYET,
  USBH_URB_ERROR,
  USBH_URB_STALL
} USBH_URBStateTypeDef;

typedef enum
{
  USBH_PORT_EVENT = 1,
  USBH_URB_EVENT,
  USBH_CONTROL_EVENT,
  USBH_CLASS_EVENT,
  USBH_STATE_CHANGED_EVENT,
} USBH_OSEventTypeDef;

/* Control request structure */
typedef struct
{
  uint8_t               pipe_in;
  uint8_t               pipe_out;
  uint8_t               pipe_size;
  uint8_t               *buff;
  uint16_t              length;
  uint16_t              timer;
  USB_Setup_TypeDef     setup;
  CTRL_StateTypeDef     state;
  uint8_t               errorcount;

} USBH_CtrlTypeDef;


#define USBH_PID_SETUP                            0
#define USBH_PID_DATA                             1

#define USBH_EP_CONTROL                           0
#define USBH_EP_ISO                               1
#define USBH_EP_BULK                              2
#define USBH_EP_INTERRUPT                         3

#define USBH_SETUP_PKT_SIZE                       8


/* Table 9-5. Descriptor Types of USB Specifications */
#define  USB_DESC_TYPE_DEVICE                              1
#define  USB_DESC_TYPE_CONFIGURATION                       2
#define  USB_DESC_TYPE_STRING                              3
#define  USB_DESC_TYPE_INTERFACE                           4
#define  USB_DESC_TYPE_ENDPOINT                            5
#define  USB_DESC_TYPE_DEVICE_QUALIFIER                    6
#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION           7
#define  USB_DESC_TYPE_INTERFACE_POWER                     8
#define  USB_DESC_TYPE_HID                                 0x21
#define  USB_DESC_TYPE_HID_REPORT                          0x22


#define USB_DEVICE_DESC_SIZE                               18
#define USB_CONFIGURATION_DESC_SIZE                        9
#define USB_HID_DESC_SIZE                                  9
#define USB_INTERFACE_DESC_SIZE                            9
#define USB_ENDPOINT_DESC_SIZE                             7

/* Descriptor Type and Descriptor Index  */
/* Use the following values when calling the function USBH_GetDescriptor  */
#define  USB_DESC_DEVICE                    ((USB_DESC_TYPE_DEVICE << 8) & 0xFF00)
#define  USB_DESC_CONFIGURATION             ((USB_DESC_TYPE_CONFIGURATION << 8) & 0xFF00)
#define  USB_DESC_STRING                    ((USB_DESC_TYPE_STRING << 8) & 0xFF00)
#define  USB_DESC_INTERFACE                 ((USB_DESC_TYPE_INTERFACE << 8) & 0xFF00)
#define  USB_DESC_ENDPOINT                  ((USB_DESC_TYPE_INTERFACE << 8) & 0xFF00)
#define  USB_DESC_DEVICE_QUALIFIER          ((USB_DESC_TYPE_DEVICE_QUALIFIER << 8) & 0xFF00)
#define  USB_DESC_OTHER_SPEED_CONFIGURATION ((USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION << 8) & 0xFF00)
#define  USB_DESC_INTERFACE_POWER           ((USB_DESC_TYPE_INTERFACE_POWER << 8) & 0xFF00)
#define  USB_DESC_HID_REPORT                ((USB_DESC_TYPE_HID_REPORT << 8) & 0xFF00)
#define  USB_DESC_HID                       ((USB_DESC_TYPE_HID << 8) & 0xFF00)


#define  USB_EP_TYPE_CTRL                               0x00
#define  USB_EP_TYPE_ISOC                               0x01
#define  USB_EP_TYPE_BULK                               0x02
#define  USB_EP_TYPE_INTR                               0x03

#define  USB_EP_DIR_OUT                                 0x00
#define  USB_EP_DIR_IN                                  0x80
#define  USB_EP_DIR_MSK                                 0x80

#ifndef USBH_MAX_PIPES_NBR
 #define USBH_MAX_PIPES_NBR                             15
#endif /* USBH_MAX_PIPES_NBR */

#define USBH_DEVICE_ADDRESS_DEFAULT                     0
#define USBH_MAX_ERROR_COUNT                            2
#define USBH_DEVICE_ADDRESS                             1

#define HOST_USER_SELECT_CONFIGURATION          1
#define HOST_USER_CLASS_ACTIVE                  2
#define HOST_USER_CLASS_SELECTED                3
#define HOST_USER_CONNECTION                    4
#define HOST_USER_DISCONNECTION                 5
#define HOST_USER_UNRECOVERED_ERROR             6



/* Attached device structure */
typedef struct
{
#if (USBH_KEEP_CFG_DESCRIPTOR == 1)
	uint8_t                           CfgDesc_Raw [USBH_MAX_SIZE_CONFIGURATION];
#endif
	uint8_t                           Data [USBH_MAX_DATA_BUFFER];
	uint8_t                           address;
	uint8_t                           usb_otg_speed;
	volatile uint8_t                      is_connected;
	uint8_t                           current_interface;
	USBH_DevDescTypeDef               DevDesc;
	USBH_CfgDescTypeDef               CfgDesc;

}USBH_DeviceTypeDef;

struct _USBH_HandleTypeDef;

/* USB Host Class structure */
typedef struct
{
	const char          *Name;
	uint8_t              ClassCode;
	USBH_StatusTypeDef  (*Init)        (struct _USBH_HandleTypeDef *phost);
	USBH_StatusTypeDef  (*DeInit)      (struct _USBH_HandleTypeDef *phost);
	USBH_StatusTypeDef  (*Requests)    (struct _USBH_HandleTypeDef *phost);
	USBH_StatusTypeDef  (*BgndProcess) (struct _USBH_HandleTypeDef *phost);
	USBH_StatusTypeDef  (*SOFProcess) (struct _USBH_HandleTypeDef *phost);
	void*                pData;
} USBH_ClassTypeDef;

#define USBHNPIPES 15
/* USB Host handle structure */
typedef struct _USBH_HandleTypeDef
{
	volatile HOST_StateTypeDef     gState;       /*  Host State Machine Value */
	HOST_StateTypeDef	  gPushState;
	uint_fast16_t		  gPushTicks;
	ENUM_StateTypeDef     EnumState;    /* Enumeration state Machine */
	ENUM_StateTypeDef     EnumPushedState;    /* Enumeration state Machine */
	uint_fast16_t		  EnumPushTicks;
	CMD_StateTypeDef      RequestState;
	USBH_CtrlTypeDef      Control;
	USBH_DeviceTypeDef    device;
	USBH_ClassTypeDef*    pClass[USBH_MAX_NUM_SUPPORTED_CLASS];
	USBH_ClassTypeDef*    pActiveClass;
	uint32_t              ClassNumber;
	uint32_t              Pipes [USBHNPIPES];
	volatile uint32_t         Timer;
	//uint8_t               id;
	void*                 pData;
	void                 (* pUser )(struct _USBH_HandleTypeDef *pHandle, uint8_t id);

#if (USBH_USE_OS == 1)
	osMessageQId          os_event;
	osThreadId            thread;
#endif

} USBH_HandleTypeDef;


USBH_StatusTypeDef  USBH_HandleEnum    (USBH_HandleTypeDef *phost);
void                USBH_HandleSof     (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef  DeInitStateMachine(USBH_HandleTypeDef *phost);

#if (USBH_USE_OS == 1)
void USBH_Process_OS(void const * argument);
#endif

/** @defgroup HCD_Exported_Types_Group1 HCD State Structure definition
  * @{
  */
typedef enum
{
  HAL_HCD_STATE_RESET    = 0x00,
  HAL_HCD_STATE_READY    = 0x01,
  HAL_HCD_STATE_ERROR    = 0x02,
  HAL_HCD_STATE_BUSY     = 0x03,
  HAL_HCD_STATE_TIMEOUT  = 0x04
} HCD_StateTypeDef;

typedef USB_OTG_CfgTypeDef      HCD_InitTypeDef;
typedef USB_OTG_HCTypeDef       HCD_HCTypeDef ;
typedef USB_OTG_URBStateTypeDef HCD_URBStateTypeDef ;
typedef USB_OTG_HCStateTypeDef  HCD_HCStateTypeDef ;
/**
  * @}
  */

/** @defgroup HCD_Exported_Types_Group2 HCD Handle Structure definition
  * @{
  */
typedef struct
{
  HCD_TypeDef               *Instance;  /*!< Register base address    */
  HCD_InitTypeDef           Init;       /*!< HCD required parameters  */
  HCD_HCTypeDef             hc [15];	/*!< Host channels parameters */
  HAL_LockTypeDef           Lock;       /*!< HCD peripheral status    */
  volatile HCD_StateTypeDef     State;      /*!< HCD communication state  */
  void                      *pData;     /*!< Pointer Stack Handler    */
} HCD_HandleTypeDef;



HAL_StatusTypeDef HAL_PCD_Start(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd);
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd);

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd);

void HAL_PCD_AdressedCallback(PCD_HandleTypeDef *hpcd);	// RENESAS specific
/**
  * @}
  */

/* Peripheral Control functions  **********************************************/
/** @addtogroup PCD_Exported_Functions_Group3 Peripheral Control functions
  * @{
  */
HAL_StatusTypeDef HAL_PCD_SetAddress(PCD_HandleTypeDef *hpcd, uint_fast8_t address);
HAL_StatusTypeDef HAL_PCD_EP_Open(PCD_HandleTypeDef *hpcd, uint_fast8_t ep_addr, uint_fast8_t tx_fifo_num, uint_fast16_t ep_mps, uint_fast8_t ep_type);
HAL_StatusTypeDef HAL_PCD_EP_Close(PCD_HandleTypeDef *hpcd, uint_fast8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint_fast8_t ep_addr, uint8_t *pBuf, uint32_t len);
uint16_t          HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint_fast8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_SetStall(PCD_HandleTypeDef *hpcd, uint_fast8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_ClrStall(PCD_HandleTypeDef *hpcd, uint_fast8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_ActivateRemoteWakeup(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeActivateRemoteWakeup(PCD_HandleTypeDef *hpcd);


/** @defgroup HCD_Speed HCD Speed
  * @{
  */
#define HCD_SPEED_HIGH               0
#define HCD_SPEED_LOW                2
#define HCD_SPEED_FULL               3
/**
  * @}
  */

/** @defgroup HCD_PHY_Module HCD PHY Module
  * @{
  */
#define HCD_PHY_ULPI                 1
#define HCD_PHY_EMBEDDED             2
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup HCD_Exported_Macros HCD Exported Macros
 *  @brief macros to handle interrupts and specific clock configurations
 * @{
 */
#define __HAL_HCD_ENABLE(h)                   do { USB_EnableGlobalInt((h)->Instance); } while (0)
#define __HAL_HCD_DISABLE(h)                  do { USB_DisableGlobalInt((h)->Instance); } while (0)

#define __HAL_PCD_ENABLE(h)                   do { USB_EnableGlobalInt((h)->Instance); } while (0)
#define __HAL_PCD_DISABLE(h)                  do { USB_DisableGlobalInt((h)->Instance); } while (0)

#define __HAL_HCD_GET_FLAG(h, intr)      ((USB_ReadInterrupts((h)->Instance) & (intr)) == (intr))
#define __HAL_HCD_CLEAR_FLAG(h, intr)    do { (((h)->Instance->GINTSTS) = (intr)); } while (0)
#define __HAL_HCD_IS_INVALID_INTERRUPT(h)         (USB_ReadInterrupts((h)->Instance) == 0)

#define __HAL_HCD_CLEAR_HC_INT(chnum, intr)  do { USBx_HC(chnum)->HCINT = (intr); } while (0)
#define __HAL_HCD_MASK_HALT_HC_INT(chnum)             do { USBx_HC(chnum)->HCINTMSK &= ~ USB_OTG_HCINTMSK_CHHM; } while (0)
#define __HAL_HCD_UNMASK_HALT_HC_INT(chnum)           do { USBx_HC(chnum)->HCINTMSK |= USB_OTG_HCINTMSK_CHHM; } while (0)
#define __HAL_HCD_MASK_ACK_HC_INT(chnum)              do { USBx_HC(chnum)->HCINTMSK &= ~ USB_OTG_HCINTMSK_ACKM; } while (0)
#define __HAL_HCD_UNMASK_ACK_HC_INT(chnum)            do { USBx_HC(chnum)->HCINTMSK |= USB_OTG_HCINTMSK_ACKM; } while (0)


/** @defgroup HCD_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
HAL_StatusTypeDef      HAL_HCD_Init(HCD_HandleTypeDef *hhcd);
HAL_StatusTypeDef      HAL_HCD_DeInit (HCD_HandleTypeDef *hhcd);
HAL_StatusTypeDef      HAL_HCD_HC_Init(HCD_HandleTypeDef *hhcd,
                                  uint8_t ch_num,
                                  uint8_t epnum,
                                  uint8_t dev_address,
                                  uint8_t speed,
                                  uint8_t ep_type,
                                  uint16_t mps);

HAL_StatusTypeDef   HAL_HCD_HC_Halt(HCD_HandleTypeDef *hhcd, uint8_t ch_num);
void                HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd);
void                HAL_HCD_MspDeInit(HCD_HandleTypeDef *hhcd);
/**
  * @}
  */

/** @defgroup HCD_Exported_Functions_Group2 IO operation functions
  * @{
  */
HAL_StatusTypeDef       HAL_HCD_HC_SubmitRequest(HCD_HandleTypeDef *hhcd,
                                                 uint8_t pipe,
                                                 uint8_t direction ,
                                                 uint8_t ep_type,
                                                 uint8_t token,
                                                 uint8_t* pbuff,
                                                 uint16_t length,
                                                 uint8_t do_ping);

 /* Non-Blocking mode: Interrupt */
void            HAL_HCD_IRQHandler(HCD_HandleTypeDef *hhcd);
void             HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd);
void             HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd);
void             HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd);
void             HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef *hhcd,
                                                            uint8_t chnum,
                                                            HCD_URBStateTypeDef urb_state);
/**
  * @}
  */

/** @defgroup HCD_Exported_Functions_Group3 Peripheral Control functions
  * @{
  */
HAL_StatusTypeDef       HAL_HCD_ResetPort(HCD_HandleTypeDef *hhcd, uint_fast8_t status);
HAL_StatusTypeDef       HAL_HCD_Start(HCD_HandleTypeDef *hhcd);
HAL_StatusTypeDef       HAL_HCD_Stop(HCD_HandleTypeDef *hhcd);
/**
  * @}
  */

/** @defgroup HCD_Exported_Functions_Group4 Peripheral State functions
  * @{
  */
HCD_StateTypeDef        HAL_HCD_GetState(HCD_HandleTypeDef *hhcd);
HCD_URBStateTypeDef     HAL_HCD_HC_GetURBState(HCD_HandleTypeDef *hhcd, uint8_t chnum);
uint32_t                HAL_HCD_HC_GetXferCount(HCD_HandleTypeDef *hhcd, uint8_t chnum);
HCD_HCStateTypeDef      HAL_HCD_HC_GetState(HCD_HandleTypeDef *hhcd, uint8_t chnum);
uint32_t                HAL_HCD_GetCurrentFrame(HCD_HandleTypeDef *hhcd);
uint32_t                HAL_HCD_GetCurrentSpeed(HCD_HandleTypeDef *hhcd);

/**
  * @}
  */

USBH_StatusTypeDef USBH_HandleControl (USBH_HandleTypeDef *phost);

void USBH_ParseDevDesc (USBH_DevDescTypeDef* , uint8_t *buf, uint16_t length);

void USBH_ParseCfgDesc (USBH_CfgDescTypeDef* cfg_desc,
                               uint8_t *buf,
                               uint16_t length);


void USBH_ParseEPDesc (USBH_EpDescTypeDef  *ep_descriptor, uint8_t *buf);
void USBH_ParseStringDesc (uint8_t* psrc, uint8_t* pdest, uint16_t length);
void USBH_ParseInterfaceDesc (USBH_InterfaceDescTypeDef  *if_descriptor, uint8_t *buf);



USBH_StatusTypeDef  USBH_Init(USBH_HandleTypeDef *phost, void (*pUsrFunc)(USBH_HandleTypeDef *phost, uint8_t ));
USBH_StatusTypeDef  USBH_DeInit(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef  USBH_RegisterClass(USBH_HandleTypeDef *phost, USBH_ClassTypeDef *pclass);
USBH_StatusTypeDef  USBH_SelectInterface(USBH_HandleTypeDef *phost, uint8_t interfacei);
uint8_t             USBH_FindInterface(USBH_HandleTypeDef *phost,
                                            uint8_t Class,
                                            uint8_t SubClass,
                                            uint8_t Protocol);
uint8_t             USBH_GetActiveClass(USBH_HandleTypeDef *phost);

uint8_t             USBH_FindInterfaceIndex(USBH_HandleTypeDef *phost,
                                            uint8_t interface_number,
                                            uint8_t alt_settings);

USBH_StatusTypeDef  USBH_Start            (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef  USBH_Stop             (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef  USBH_Process          (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef  USBH_ReEnumerate      (USBH_HandleTypeDef *phost);

/* USBH Low Level Driver */
USBH_StatusTypeDef   USBH_LL_Init         (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef   USBH_LL_DeInit       (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef   USBH_LL_Start        (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef   USBH_LL_Stop         (USBH_HandleTypeDef *phost);

USBH_StatusTypeDef   USBH_LL_Connect      (USBH_HandleTypeDef *phost);
USBH_StatusTypeDef   USBH_LL_Disconnect   (USBH_HandleTypeDef *phost);

uint8_t    USBH_LL_GetSpeed     (USBH_HandleTypeDef *phost);

USBH_StatusTypeDef   USBH_LL_ResetPort    (USBH_HandleTypeDef *phost, uint_fast8_t status);
uint32_t             USBH_LL_GetLastXferSize   (USBH_HandleTypeDef *phost, uint8_t );
USBH_StatusTypeDef   USBH_LL_DriverVBUS   (USBH_HandleTypeDef *phost, uint_fast8_t );

USBH_StatusTypeDef   USBH_LL_OpenPipe     (USBH_HandleTypeDef *phost, uint8_t, uint8_t, uint8_t, uint8_t , uint8_t, uint16_t );
USBH_StatusTypeDef   USBH_LL_ClosePipe    (USBH_HandleTypeDef *phost, uint8_t );
USBH_StatusTypeDef   USBH_LL_SubmitURB    (USBH_HandleTypeDef *phost, uint8_t, uint8_t,uint8_t,  uint8_t, uint8_t*, uint16_t, uint8_t );
USBH_URBStateTypeDef USBH_LL_GetURBState  (USBH_HandleTypeDef *phost, uint8_t );
#if (USBH_USE_OS == 1)
USBH_StatusTypeDef  USBH_LL_NotifyURBChange (USBH_HandleTypeDef *phost);
#endif
USBH_StatusTypeDef   USBH_LL_SetToggle    (USBH_HandleTypeDef *phost, uint8_t , uint8_t );
uint8_t              USBH_LL_GetToggle    (USBH_HandleTypeDef *phost, uint8_t );

/* USBH Time base */
void                 USBH_LL_SetTimer     (USBH_HandleTypeDef *phost, uint32_t );
void                 USBH_LL_IncTimer     (USBH_HandleTypeDef *phost);




typedef enum {
  APPLICATION_IDLE = 0,
  APPLICATION_START,
  APPLICATION_READY,
  APPLICATION_DISCONNECT,
} ApplicationTypeDef;



USBD_StatusTypeDef  USBD_LL_Stop(USBD_HandleTypeDef *pdev);
USBD_StatusTypeDef  USBD_LL_Start(USBD_HandleTypeDef *pdev);


/** @defgroup USBD_CORE_Exported_FunctionsPrototype
  * @{
  */
USBD_StatusTypeDef USBD_DeInit(USBD_HandleTypeDef *pdev);
USBD_StatusTypeDef USBD_Start(USBD_HandleTypeDef *pdev);
USBD_StatusTypeDef USBD_AddClass(USBD_HandleTypeDef *pdev, const USBD_ClassTypeDef *pclass);

USBD_StatusTypeDef USBD_RunTestMode (USBD_HandleTypeDef  *pdev);
USBD_StatusTypeDef USBD_SetClassConfig(USBD_HandleTypeDef  *pdev, uint_fast8_t cfgidx);
USBD_StatusTypeDef USBD_ClrClassConfig(USBD_HandleTypeDef  *pdev, uint_fast8_t cfgidx);

USBD_StatusTypeDef USBD_LL_SetupStage(USBD_HandleTypeDef *pdev, const uint32_t *psetup);
USBD_StatusTypeDef USBD_LL_DataOutStage(USBD_HandleTypeDef *pdev, uint_fast8_t epnum, uint8_t *pdata);
USBD_StatusTypeDef USBD_LL_DataInStage(USBD_HandleTypeDef *pdev, uint_fast8_t epnum, uint8_t *pdata);

USBD_StatusTypeDef USBD_LL_Reset(USBD_HandleTypeDef  *pdev);
USBD_StatusTypeDef USBD_LL_SetSpeed(USBD_HandleTypeDef  *pdev, USBD_SpeedTypeDef speed);
USBD_StatusTypeDef USBD_LL_Suspend(USBD_HandleTypeDef  *pdev);
USBD_StatusTypeDef USBD_LL_Resume(USBD_HandleTypeDef  *pdev);

USBD_StatusTypeDef USBD_LL_SOF(USBD_HandleTypeDef  *pdev);
USBD_StatusTypeDef USBD_LL_IsoINIncomplete(USBD_HandleTypeDef  *pdev, uint_fast8_t epnum);
USBD_StatusTypeDef USBD_LL_IsoOUTIncomplete(USBD_HandleTypeDef  *pdev, uint_fast8_t epnum);

USBD_StatusTypeDef USBD_LL_DevConnected(USBD_HandleTypeDef  *pdev);
USBD_StatusTypeDef USBD_LL_DevDisconnected(USBD_HandleTypeDef  *pdev);

/* USBD Low Level Driver */

USBD_StatusTypeDef  USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr);
USBD_StatusTypeDef  USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr);
USBD_StatusTypeDef  USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr);
uint8_t             USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr);
USBD_StatusTypeDef  USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr);
USBD_StatusTypeDef  USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint_fast8_t  ep_addr, const uint8_t  *pbuf, uint_fast32_t  size);

uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr);
USBD_StatusTypeDef  USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr);
USBD_StatusTypeDef  USBD_LL_OpenEP(USBD_HandleTypeDef * pdev, uint8_t ep_addr, uint8_t  ep_type, uint16_t ep_mps);



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

#define USBx_PCGCCTL    (* (__IO uint32_t *) ((uintptr_t) (USBx) + USB_OTG_PCGCCTL_BASE))
#define USBx_HPRT0      (* (__IO uint32_t *) ((uintptr_t) (USBx) + USB_OTG_HOST_PORT_BASE))

#define USBx_DEVICE     ((USB_OTG_DeviceTypeDef *) ((uintptr_t) (USBx) + USB_OTG_DEVICE_BASE))
#define USBx_INEP(i)    ((USB_OTG_INEndpointTypeDef *) ((uintptr_t) (USBx) + USB_OTG_IN_ENDPOINT_BASE + (i) * USB_OTG_EP_REG_SIZE))
#define USBx_OUTEP(i)   ((USB_OTG_OUTEndpointTypeDef *) ((uintptr_t) (USBx) + USB_OTG_OUT_ENDPOINT_BASE + (i) * USB_OTG_EP_REG_SIZE))
#define USBx_DFIFO(i)   (* (__IO uint32_t *) ((uintptr_t) (USBx) + USB_OTG_FIFO_BASE + (i) * USB_OTG_FIFO_SIZE))

#define USBx_HOST       ((USB_OTG_HostTypeDef *) ((uintptr_t) (USBx) + USB_OTG_HOST_BASE))
#define USBx_HC(i)      ((USB_OTG_HostChannelTypeDef *) ((uintptr_t) (USBx) + USB_OTG_HOST_CHANNEL_BASE + (i) * USB_OTG_HOST_CHANNEL_SIZE))

/* Exported macro ------------------------------------------------------------*/
#define USB_MASK_INTERRUPT(i, m)     do { (i)->GINTMSK &= ~ (m); (void) (i)->GINTMSK; } while (0)
#define USB_UNMASK_INTERRUPT(i, m)   do { (i)->GINTMSK |= (m); (void) (i)->GINTMSK; } while (0)
#define CLEAR_IN_EP_INTR(ep, m)      do { USBx_INEP(ep)->DIEPINT = (m); (void) USBx_INEP(ep)->DIEPINT; } while (0)
#define CLEAR_OUT_EP_INTR(ep, m)     do { USBx_OUTEP(ep)->DOEPINT = (m); (void) USBx_OUTEP(ep)->DOEPINT; } while (0)
/**
  * @}
  */
/* Exported macro ------------------------------------------------------------*/

/** @defgroup PCD_Speed PCD Speed
  * @{
  */
#define PCD_SPEED_HIGH               0U
#define PCD_SPEED_HIGH_IN_FULL       1U
#define PCD_SPEED_FULL               2U
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


#define USB_OTG_FS_WAKEUP_EXTI_RISING_EDGE                ((uint32_t)0x08U)
#define USB_OTG_FS_WAKEUP_EXTI_FALLING_EDGE               ((uint32_t)0x0CU)
#define USB_OTG_FS_WAKEUP_EXTI_RISING_FALLING_EDGE        ((uint32_t)0x10U)

#define USB_OTG_HS_WAKEUP_EXTI_RISING_EDGE                ((uint32_t)0x08U)
#define USB_OTG_HS_WAKEUP_EXTI_FALLING_EDGE               ((uint32_t)0x0CU)
#define USB_OTG_HS_WAKEUP_EXTI_RISING_FALLING_EDGE        ((uint32_t)0x10U)

#define USB_OTG_HS_WAKEUP_EXTI_LINE                       ((uint32_t)0x00100000U)  /*!< External interrupt line 20 Connected to the USB HS EXTI Line */
#define USB_OTG_FS_WAKEUP_EXTI_LINE                       ((uint32_t)0x00040000U)  /*!< External interrupt line 18 Connected to the USB FS EXTI Line */

#define __HAL_USB_OTG_HS_WAKEUP_EXTI_ENABLE_IT()    do { EXTI->IMR |= (USB_OTG_HS_WAKEUP_EXTI_LINE); } while (0)
#define __HAL_USB_OTG_HS_WAKEUP_EXTI_DISABLE_IT()   do { EXTI->IMR &= ~(USB_OTG_HS_WAKEUP_EXTI_LINE); } while (0)
#define __HAL_USB_OTG_HS_WAKEUP_EXTI_GET_FLAG()     do { EXTI->PR & (USB_OTG_HS_WAKEUP_EXTI_LINE); } while (0)
#define __HAL_USB_OTG_HS_WAKEUP_EXTI_CLEAR_FLAG()   do { EXTI->PR = (USB_OTG_HS_WAKEUP_EXTI_LINE); } while (0)

#define __HAL_USB_OTG_HS_WAKEUP_EXTI_ENABLE_RISING_EDGE() do { \
		EXTI->FTSR &= ~(USB_OTG_HS_WAKEUP_EXTI_LINE);\
		EXTI->RTSR |= USB_OTG_HS_WAKEUP_EXTI_LINE; \
		} while (0)

#define __HAL_USB_OTG_HS_WAKEUP_EXTI_ENABLE_FALLING_EDGE()  do { EXTI->FTSR |= (USB_OTG_HS_WAKEUP_EXTI_LINE);\
                                                            EXTI->RTSR &= ~(USB_OTG_HS_WAKEUP_EXTI_LINE); } while (0)

#define __HAL_USB_OTG_HS_WAKEUP_EXTI_ENABLE_RISING_FALLING_EDGE()   do { EXTI->RTSR &= ~(USB_OTG_HS_WAKEUP_EXTI_LINE);\
                                                                    do { EXTI->FTSR &= ~(USB_OTG_HS_WAKEUP_EXTI_LINE;)\
                                                                    do { EXTI->RTSR |= USB_OTG_HS_WAKEUP_EXTI_LINE;\
                                                                    do { EXTI->FTSR |= USB_OTG_HS_WAKEUP_EXTI_LINE

#define __HAL_USB_OTG_HS_WAKEUP_EXTI_GENERATE_SWIT()   do { (EXTI->SWIER |= USB_OTG_FS_WAKEUP_EXTI_LINE) ; } while (0)

#define __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_IT()    do { EXTI->IMR |= USB_OTG_FS_WAKEUP_EXTI_LINE; } while (0)
#define __HAL_USB_OTG_FS_WAKEUP_EXTI_DISABLE_IT()   do { EXTI->IMR &= ~(USB_OTG_FS_WAKEUP_EXTI_LINE); } while (0)
#define __HAL_USB_OTG_FS_WAKEUP_EXTI_GET_FLAG()     do { EXTI->PR & (USB_OTG_FS_WAKEUP_EXTI_LINE); } while (0)
#define __HAL_USB_OTG_FS_WAKEUP_EXTI_CLEAR_FLAG()   do { EXTI->PR = USB_OTG_FS_WAKEUP_EXTI_LINE; } while (0)

#define __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_RISING_EDGE() do { EXTI->FTSR &= ~(USB_OTG_FS_WAKEUP_EXTI_LINE);\
                                                          EXTI->RTSR |= USB_OTG_FS_WAKEUP_EXTI_LINE; } while (0)


#define __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_FALLING_EDGE()  do { EXTI->FTSR |= (USB_OTG_FS_WAKEUP_EXTI_LINE);\
                                                            EXTI->RTSR &= ~(USB_OTG_FS_WAKEUP_EXTI_LINE); } while (0)

#define __HAL_USB_OTG_FS_WAKEUP_EXTI_ENABLE_RISING_FALLING_EDGE()  do { EXTI->RTSR &= ~(USB_OTG_FS_WAKEUP_EXTI_LINE);\
                                                                   EXTI->FTSR &= ~(USB_OTG_FS_WAKEUP_EXTI_LINE);\
                                                                   EXTI->RTSR |= USB_OTG_FS_WAKEUP_EXTI_LINE;\
                                                                   EXTI->FTSR |= USB_OTG_FS_WAKEUP_EXTI_LINE ; } while (0)

#define __HAL_USB_OTG_FS_WAKEUP_EXTI_GENERATE_SWIT()  do { (EXTI->SWIER |= USB_OTG_FS_WAKEUP_EXTI_LINE); } while (0)

/* Exported functions --------------------------------------------------------*/
HAL_StatusTypeDef USB_EnableGlobalInt(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_DisableGlobalInt(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_SetCurrentMode(USB_OTG_GlobalTypeDef *USBx, USB_OTG_ModeTypeDef mode);
HAL_StatusTypeDef USB_SetDevSpeed(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t speed);
HAL_StatusTypeDef USB_FlushRxFifo(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_FlushTxFifoEx(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t num);
HAL_StatusTypeDef USB_FlushTxFifoAll(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_ActivateEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_DeactivateEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_ActivateDedicatedEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_DeactivateDedicatedEndpoint(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_EPStartXfer(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep, uint_fast8_t dma);
HAL_StatusTypeDef USB_EP0StartXfer(USB_OTG_GlobalTypeDef *USBx, USB_OTG_EPTypeDef *ep, uint_fast8_t dma);
void USB_WritePacket(USB_OTG_GlobalTypeDef *USBx, const uint8_t *src, uint_fast8_t tx_fifo_num, uint_fast16_t len, uint_fast8_t dma);
void USB_ReadPacket(USB_OTG_GlobalTypeDef *USBx, uint8_t *dest, uint_fast16_t len);
HAL_StatusTypeDef USB_EPSetStall(USB_OTG_GlobalTypeDef *USBx, const USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_EPClearStall(USB_OTG_GlobalTypeDef *USBx, const USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_SetDevAddress (USB_OTG_GlobalTypeDef *USBx, uint_fast8_t address);
HAL_StatusTypeDef USB_DevConnect(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_DevDisconnect (USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_StopDevice(USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_ActivateSetup (USB_OTG_GlobalTypeDef *USBx);
HAL_StatusTypeDef USB_EP0_OutStart(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t dma, uint8_t *psetup);
uint_fast8_t USB_GetDevSpeed(USB_OTG_GlobalTypeDef *USBx);
uint32_t USB_GetMode(USB_OTG_GlobalTypeDef *USBx);
uint32_t USB_ReadInterrupts (USB_OTG_GlobalTypeDef *USBx);
uint32_t USB_ReadDevAllOutEpInterrupt(USB_OTG_GlobalTypeDef *USBx);
uint32_t USB_ReadDevOutEPInterrupt(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t epnum);
uint32_t USB_ReadDevAllInEpInterrupt(USB_OTG_GlobalTypeDef *USBx);
uint32_t USB_ReadDevInEPInterrupt(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t epnum);
void USB_ClearInterrupts(USB_OTG_GlobalTypeDef *USBx, uint32_t interrupt);

HAL_StatusTypeDef USB_InitFSLSPClkSel(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t freq);
HAL_StatusTypeDef USB_ResetPort(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t state);
HAL_StatusTypeDef USB_DriveVbus(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t state);
uint32_t  USB_GetHostSpeed(USB_OTG_GlobalTypeDef *USBx);

#define  USB_DISABLE   0
#define  USB_ENABLE    1


#define ValBit(VAR,POS)                               (VAR & (1 << POS))
#define SetBit(VAR,POS)                               (VAR |= (1 << POS))
#define ClrBit(VAR,POS)                               (VAR &= ((1 << POS)^255))

#define  LE16(addr)             (((uint16_t)(*((uint8_t *)(addr))))\
                                + (((uint16_t)(*(((uint8_t *)(addr)) + 1))) << 8))

#define  LE16S(addr)              (uint16_t)(LE16((addr)))

#define  LE32(addr)              ((((uint32_t)(*(((uint8_t *)(addr)) + 0))) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 1))) << 8) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 2))) << 16) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 3))) << 24)))

#define  LE64(addr)              ((((uint64_t)(*(((uint8_t *)(addr)) + 0))) + \
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 1))) <<  8) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 2))) << 16) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 3))) << 24) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 4))) << 32) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 5))) << 40) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 6))) << 48) +\
                                              (((uint64_t)(*(((uint8_t *)(addr)) + 7))) << 56)))


#define  LE24(addr)              ((((uint32_t)(*(((uint8_t *)(addr)) + 0))) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 1))) << 8) + \
                                              (((uint32_t)(*(((uint8_t *)(addr)) + 2))) << 16)))


#define  LE32S(addr)              (int32_t)(LE32((addr)))

USBD_StatusTypeDef  USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev,
                                           uint8_t  ep_addr,
                                           uint8_t  *pbuf,
                                           uint16_t  size);
USBD_StatusTypeDef  USBD_CtlSendData(USBD_HandleTypeDef  *pdev,
                              const uint8_t *pbuf,
                               uint16_t len);
void USBD_CtlError( USBD_HandleTypeDef *pdev,
                            const USBD_SetupReqTypedef *req);

USBD_StatusTypeDef  USBD_CtlPrepareRx(USBD_HandleTypeDef  *pdev,
                                  uint8_t *pbuf,
                                  uint16_t len);
USBD_StatusTypeDef  USBD_CtlSendStatus(USBD_HandleTypeDef  *pdev);

USBD_StatusTypeDef  USBD_Stop(USBD_HandleTypeDef *pdev);

extern const USBD_ClassTypeDef USBD_CLASS_UAC;
extern const USBD_ClassTypeDef USBD_CLASS_CDC;
extern const USBD_ClassTypeDef USBD_CLASS_DFU;
extern const USBD_ClassTypeDef USBD_CLASS_CDC_EEM;

uint_fast16_t usbd_getuacinrtsmaxpacket(void);
uint_fast16_t usbd_getuacinmaxpacket(void);

void usbd_descriptors_initialize(uint_fast8_t deschs);

struct descholder
{
	const uint8_t * data;
	unsigned size;
};

/* получить 32-бит значение */
uint_fast32_t
/* Low endian memory layout */
USBD_peek_u32(
	const uint8_t * buff
	);

/* записать в буфер для ответа 32-бит значение */
/* Low endian memory layout */
unsigned USBD_poke_u32(uint8_t * buff, uint_fast32_t v);

/* получить 24-бит значение */
/* Low endian memory layout */
uint_fast32_t
USBD_peek_u24(
	const uint8_t * buff
	);

/* записать в буфер для ответа 24-бит значение */
/* Low endian memory layout */
unsigned USBD_poke_u24(uint8_t * buff, uint_fast32_t v);

/* получить 16-бит значение */
/* Low endian memory layout */
uint_fast32_t
USBD_peek_u16(
	const uint8_t * buff
	);

/* записать в буфер для ответа 16-бит значение */
unsigned USBD_poke_u16(uint8_t * buff, uint_fast16_t v);

/* получить 8-бит значение */
uint_fast8_t
USBD_peek_u8(
	const uint8_t * buff
	);

/* записать в буфер для ответа 8-бит значение */
unsigned USBD_poke_u8(uint8_t * buff, uint_fast8_t v);

/* получить 32-бит значение */
/* Big endian memory layout */
uint_fast32_t
USBD_peek_u32_BE(
	const uint8_t * buff
	);

/* записать в буфер для ответа n-бит значение */
/* Big endian memory layout */
unsigned USBD_poke_u32_BE(uint8_t * buff, uint_fast32_t v);
unsigned USBD_poke_u24_BE(uint8_t * buff, uint_fast32_t v);
unsigned USBD_poke_u16_BE(uint8_t * buff, uint_fast16_t v);

#define USBD_CONFIGCOUNT 4

extern struct descholder MsftStringDescr [1];	// Microsoft OS String Descriptor
extern struct descholder MsftCompFeatureDescr [1];	// Microsoft Compatible ID Feature Descriptor
extern struct descholder StringDescrTbl [];
extern struct descholder ConfigDescrTbl [USBD_CONFIGCOUNT];
extern struct descholder DeviceDescrTbl [USBD_CONFIGCOUNT];
extern struct descholder DeviceQualifierTbl [USBD_CONFIGCOUNT];
extern struct descholder OtherSpeedConfigurationTbl [USBD_CONFIGCOUNT];
extern struct descholder BinaryDeviceObjectStoreTbl [1];
extern struct descholder HIDReportDescrTbl [1];
uint_fast8_t usbd_get_stringsdesc_count(void);
extern struct descholder ExtOsPropDescTbl [INTERFACE_count];


#define USBD_WCID_VENDOR_CODE 0x44
uint_fast16_t usbd_dfu_get_xfer_size(uint_fast8_t alt);

/* USB Host defines and prototypes */

/* Memory management macros */
//#define USBH_malloc               malloc
//#define USBH_free                 free
#define USBH_memset               memset
#define USBH_memcpy               memcpy

extern USBH_HandleTypeDef hUSB_Host;

USBH_StatusTypeDef USBH_CtlReq(USBH_HandleTypeDef *phost,
                             uint8_t             *buff,
                             uint16_t            length);
USBH_StatusTypeDef USBH_ClrFeature(USBH_HandleTypeDef *phost,
                                   uint8_t ep_num);
USBH_StatusTypeDef USBH_BulkSendData(USBH_HandleTypeDef *phost,
                                uint8_t *buff,
                                uint16_t length,
                                uint8_t pipe_num,
                                uint8_t do_ping );
USBH_StatusTypeDef USBH_BulkReceiveData(USBH_HandleTypeDef *phost,
                                uint8_t *buff,
                                uint16_t length,
                                uint8_t pipe_num);

uint8_t USBH_AllocPipe  (USBH_HandleTypeDef *phost, uint8_t ep_addr);
USBH_StatusTypeDef USBH_FreePipe  (USBH_HandleTypeDef *phost, uint8_t idx);
USBH_StatusTypeDef USBH_OpenPipe  (USBH_HandleTypeDef *phost,
                           uint8_t pipe_num,
                           uint8_t epnum,
                           uint8_t dev_address,
                           uint8_t speed,
                           uint8_t ep_type,
                           uint16_t mps);
USBH_StatusTypeDef USBH_ClosePipe  (USBH_HandleTypeDef *phost,
                            uint8_t pipe_num);

#define USBH_UsrLog(...) do { PRINTF(__VA_ARGS__); PRINTF("\n"); } while (0)
#define USBH_DbgLog(...) do { PRINTF(__VA_ARGS__); PRINTF("\n"); } while (0)


HAL_StatusTypeDef USB_DoPing(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t ch_num);

#endif /* USB_USB_CORE_H_ */
