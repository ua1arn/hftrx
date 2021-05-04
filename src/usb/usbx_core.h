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

	/*---------- -----------*/
//#define USBD_MAX_NUM_INTERFACES     7	// ?
/*---------- -----------*/
//#define USBD_MAX_NUM_CONFIGURATION     3
/*---------- -----------*/
//#define USBD_DEBUG_LEVEL     0
/*---------- -----------*/
//#define USBD_LPM_ENABLED     0
/*---------- -----------*/
//#define USBD_SELF_POWERED     1

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

//
///*  Device Status */
//#define USBD_STATE_DEFAULT                                1
//#define USBD_STATE_ADDRESSED                              2
//#define USBD_STATE_CONFIGURED                             3
//#define USBD_STATE_SUSPENDED                              4
//
//
///*  EP0 State */
//#define USBD_EP0_IDLE                                     0
//#define USBD_EP0_SETUP                                    1
//#define USBD_EP0_DATA_IN                                  2
//#define USBD_EP0_DATA_OUT                                 3
//#define USBD_EP0_STATUS_IN                                4
//#define USBD_EP0_STATUS_OUT                               5
//#define USBD_EP0_STALL                                    6
//
//#define USBD_EP_TYPE_CTRL                                 0
//#define USBD_EP_TYPE_ISOC                                 1
//#define USBD_EP_TYPE_BULK                                 2
//#define USBD_EP_TYPE_INTR                                 3

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
//#define DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ     (0 << 1)
//#define DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ     (1 << 1)
//#define DSTS_ENUMSPD_LS_PHY_6MHZ               (2 << 1)
//#define DSTS_ENUMSPD_FS_PHY_48MHZ              (3 << 1)
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
	HOST_DEV_ATTACHED_WAITSPEED,
	HOST_DEV_ATTACHED,
	HOST_DEV_DISCONNECTED,		// 7
	HOST_DETECT_DEVICE_SPEED,
	HOST_ENUMERATION,			// 9
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
//#define  USB_DESC_TYPE_DEVICE                              1
//#define  USB_DESC_TYPE_CONFIGURATION                       2
//#define  USB_DESC_TYPE_STRING                              3
//#define  USB_DESC_TYPE_INTERFACE                           4
//#define  USB_DESC_TYPE_ENDPOINT                            5
//#define  USB_DESC_TYPE_DEVICE_QUALIFIER                    6
//#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION           7
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

void HAL_PCD_AdressedCallback(PCD_HandleTypeDef *hpcd);	// RENESAS specific

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

extern const USBD_ClassTypeDef USBD_CLASS_UAC;
extern const USBD_ClassTypeDef USBD_CLASS_CDCACM;
extern const USBD_ClassTypeDef USBD_CLASS_DFU;
extern const USBD_ClassTypeDef USBD_CLASS_CDC_EEM;
extern const USBD_ClassTypeDef USBD_CLASS_RNDIS;

uint_fast16_t usbd_getuacinrtsmaxpacket(void);
uint_fast16_t usbd_getuacinmaxpacket(void);

/* USB Host defines and prototypes */
#include <string.h>
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


//HAL_StatusTypeDef USB_DoPing(USB_OTG_GlobalTypeDef *USBx, uint_fast8_t ch_num);

//extern USBD_HandleTypeDef hUsbDevice;

#endif /* USB_USB_CORE_H_ */
