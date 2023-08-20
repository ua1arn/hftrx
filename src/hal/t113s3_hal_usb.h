/*
 * allwnr_hal_usb.h
 *
 */

#ifndef SRC_T113_S3_HAL_USB_H_
#define SRC_T113_S3_HAL_USB_H_

#include "hardware.h"

#include "t113s3_hal.h"

#define WITHWAWXXUSB	1	// check PFX in usbd_cdc.c

#ifdef __cplusplus
extern "C" {
#endif

#define USB_MAX_EP_NO		5

typedef enum {
	USB_RETVAL_NOTCOMP = 0,
	USB_RETVAL_COMPOK,
	USB_RETVAL_COMPERR
	} USB_RETVAL;

typedef struct
{
    uint8_t bmRequest;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} uSetupPKG, *pSetupPKG;

typedef struct {

	//uint32_t ConfigDesc_Len;
	uint8_t MaxLUNv [4];

	//EP protocol
	#define USB_PRTCL_ILL  	0
	#define USB_PRTCL_ISO  	1
	#define USB_PRTCL_BULK 	2
	#define USB_PRTCL_INT  	3

	//Bulk Only Device State Machine
	#define USB_BO_IDLE				0
	#define USB_BO_CBW				1
	#define USB_BO_RXDATA			2
	#define USB_BO_TXDATA			3
	#define USB_BO_CSW				4
	uint32_t bo_state;						//Bulk only device state
	uintptr_t bo_xfer_addr;				//Bulk only data address
	uint32_t bo_xfer_residue;			//Bulk only data residue length
	uint32_t bo_xfer_tranferred;  //Bulk only data transferred length
	uintptr_t bo_memory_base;
	//uint32_t bo_capacity;
	//uint32_t bo_seccnt;
	uintptr_t bo_bufbase;

	uint32_t csw_fail_flag;
} usb_device_msc;

typedef struct {
	//USB SIE Hardware Config
	//uint32_t index;
	//uintptr_t reg_base;
	//uint32_t irq_no;
	//uint32_t drq_no;

	//volatile uint32_t sof_count;

	//USB user initial parameters
	#define USB_ROLE_DEV		2
	uint32_t role;
	#define USB_SPEED_UNK		0
	#define USB_SPEED_HS		1
	#define USB_SPEED_FS		2
	#define USB_SPEED_LS		3
	uint32_t speed;
	uint32_t srp;
	uint32_t hnp;

	//usb irq record
//	volatile uint32_t busirq_status;
//	volatile uint32_t busirq_flag;

	//Signals for usb state
//	volatile uint32_t connect;
//	volatile uint32_t suspend;
//	volatile uint32_t reset;
	#define USB_OTG_A_DEVICE			0
	#define USB_OTG_B_DEVICE			1
	volatile uint32_t otg_dev;

	//Signals for usb debug
//	uint32_t rst_cnt;
//	uint32_t cur_fsm;
//	uint32_t fsm_cnt;
//	uint8_t  fsm_rcd[512];

	//Signals for Data Transfer
	//volatile uint32_t ep0_flag;
	uint32_t ep0_maxpktsz;
	#define USB_EP0_SETUP					0
	#define USB_EP0_DATA					1
	//#define USB_EP0_STATUS				2
	uint32_t ep0_xfer_state;
	uintptr_t ep0_xfer_srcaddr;
	uint32_t ep0_xfer_residue;
	uint32_t ep0_xfer_tranferred;
	//uSetupPKG ep0_setup;
	USB_RETVAL ep0_ret;

	USB_RETVAL eptx_ret[USB_MAX_EP_NO];
	USB_RETVAL eprx_ret[USB_MAX_EP_NO];
	volatile uint32_t eptx_flag[USB_MAX_EP_NO];
	volatile uint32_t eprx_flag[USB_MAX_EP_NO];
	#define USB_EPX_SETUP					0
	#define USB_EPX_DATA					1
	#define USB_EPX_END						2
	uint32_t eptx_xfer_state[USB_MAX_EP_NO];
	uint32_t eprx_xfer_state[USB_MAX_EP_NO];

	//uint32_t eptx_prtcl[USB_MAX_EP_NO];
	//uint32_t eprx_prtcl[USB_MAX_EP_NO];
	//uint32_t eptx_fifo[USB_MAX_EP_NO];  //[31:16]-fifo address; [15]-double buffer; [14:0]-fifo size
	//uint32_t eprx_fifo[USB_MAX_EP_NO];  //[31:16]-fifo address; [15]-double buffer; [14:0]-fifo size
	uintptr_t eprx_xfer_addrv[USB_MAX_EP_NO];
	uint32_t eprx_xfer_residuev[USB_MAX_EP_NO];
	uint32_t eprx_xfer_tranferredv[USB_MAX_EP_NO];
	uint32_t eprx_buf_tagv[USB_MAX_EP_NO];

	uintptr_t eptx_xfer_addrv[USB_MAX_EP_NO];
	uint32_t eptx_xfer_residuev[USB_MAX_EP_NO];
	uint32_t eptx_xfer_tranferredv[USB_MAX_EP_NO];
	uint32_t eptx_buf_tagv[USB_MAX_EP_NO];

	//Function Description
#if WITHUSBDMSC
	usb_device_msc device_msc;
#endif /* WITHUSBDMSC */

#if WITHUSBDEV_DMAENABLE || 1
	//DMA Description
	//DMADESC    dma;
	uint32_t		 dmarxv[USB_MAX_EP_NO];
	uint32_t		 dmatxv[USB_MAX_EP_NO];
	uint32_t		 dmarx_last_transferv[USB_MAX_EP_NO];
	uint32_t		 dmatx_last_transferv[USB_MAX_EP_NO];
#endif /* WITHUSBDEV_DMAENABLE */
	//Timer
	#define USB_IDLE_TIMER								0x0
	#define USB_DEVICE_VBUS_DET_TIMER			0x1
	#define USB_HOST_RESET_TIMER					0x2
	#define USB_HOST_RESUME_TIMER					0x3
	#define USB_HOST_SUSPEND_TIMER				0x4
	#define USB_HOST_DELAY_TIMER					0x5
//	uint32_t timer;     //timer purpose
//	#define USB_DEVICE_VBUS_DET_TIMEOUT		10  //10ms
//	uint32_t timeout;   //timeout value (in ms)
//	uint32_t loop;      //Loop counter

	#define USB_BUFFER_SIZE							256

#if WITHWAWXXUSB
	//#define USB_EP0_OUT_BUFFER_START		8
	volatile uint8_t buffer[USB_BUFFER_SIZE];
#endif
	//uint32_t power_debouce;
} usb_struct, *pusb_struct;

#if WITHUSBDMSC

typedef struct
{
    uint32_t dCBWSig;
    uint32_t dCBWTag;
    uint32_t dCBWDTL;
    uint8_t  bmCBWFlg;
    uint8_t  bCBWLUN   : 4;
    uint8_t  bCBWRes1  : 4;   //Reserved
    uint8_t  bCBWCBL   : 5;
    uint8_t  bCBWRes2  : 3;   //Reserved
    uint8_t  CBWCB[16];
} uCBWPKG, *pCBWPKG;

typedef struct
{
  	uint32_t dCSWSig;
  	uint32_t dCSWTag;
  	uint32_t dCSWDataRes;
  	uint8_t  bCSWStatus;
} uCSWPKG, *pCSWPKG;

#endif /* WITHUSBDMSC */

//VBUS Level
#define USB_VBUS_SESSEND		0
#define USB_VUBS_SESSVLD		1
#define USB_VBUS_ABVALID		2
#define USB_VBUS_VBUSVLD		3




//////////////////////////////////////////////////////////////////////////
//#define   USB2DRAM_PARAMS    	0x0f000f0f
//#define   DRAM2USB_PARAMS    	0x0f0f0f00

#define __HAL_PCD_ENABLE(__HANDLE__)            //           (void)USB_EnableGlobalInt ((__HANDLE__)->Instance)
#define __HAL_PCD_DISABLE(__HANDLE__)           //           (void)USB_DisableGlobalInt ((__HANDLE__)->Instance)

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

/** @defgroup PCD_Speed PCD Speed
  * @{
  */
//#define PCD_SPEED_HIGH               0U
//#define PCD_SPEED_HIGH_IN_FULL       1U
//#define PCD_SPEED_FULL               2U
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


#define USB_OTG_HS_MAX_PACKET_SIZE           512U
#define USB_OTG_FS_MAX_PACKET_SIZE           64U

#define USB_OTG_MAX_EP0_SIZE                 64U


/** @defgroup USB_Core_PHY_   USB Core PHY
  * @{
  */
#define USB_OTG_ULPI_PHY                       1U
#define USB_OTG_EMBEDDED_PHY                   2U
#define USB_OTG_HS_EMBEDDED_PHY                3U
/**
  * @}
  */

typedef USBOTG_TypeDef	PCD_TypeDef;	/* processor peripherial */
//typedef USBOTG_TypeDef	HCD_TypeDef;	/* processor peripherial */


//typedef enum
//{
//  USB_DEVICE_MODE  = 0,
//  USB_HOST_MODE    = 1,
//  USB_DRD_MODE     = 2
//} USB_OTG_ModeTypeDef;

/**
  * @brief  USB Mode definition
  */
typedef enum
{
   USB_OTG_DEVICE_MODE  = 0U,
   USB_OTG_HOST_MODE    = 1U,
   USB_OTG_DRD_MODE     = 2U

} xUSB_OTG_ModeTypeDef;

/**
  * @brief  URB States definition
  */
//typedef enum
//{
//  URB_IDLE = 0,
//  URB_DONE,
//  URB_NOTREADY,
//  URB_NYET,
//  URB_ERROR,
//  URB_STALL
//} USB_OTG_URBStateTypeDef;

/**
  * @brief  Host channel States  definition
  */
//typedef enum
//{
//  HC_IDLE = 0,
//  HC_XFRC,
//  HC_HALTED,
//  HC_NAK,
//  HC_NYET,
//  HC_STALL,
//  HC_XACTERR,
//  HC_BBLERR,
//  HC_DATATGLERR
//} USB_OTG_HCStateTypeDef;

/**
  * @brief  USB Instance Initialization Structure definition
  */
//typedef struct
//{
//  uint32_t dev_endpoints;           /*!< Device Endpoints number.
//                                         This parameter depends on the used USB core.
//                                         This parameter must be a number between Min_Data = 1 and Max_Data = 15 */
//
//  uint32_t Host_channels;           /*!< Host Channels number.
//                                         This parameter Depends on the used USB core.
//                                         This parameter must be a number between Min_Data = 1 and Max_Data = 15 */
//
//  uint32_t speed;                   /*!< USB Core speed.
//                                         This parameter can be any value of @ref USB_Core_Speed                 */
//
//  uint32_t dma_enable;              /*!< Enable or disable of the USB embedded DMA used only for OTG HS.        */
//
//  uint32_t ep0_mps;                 /*!< Set the Endpoint 0 Max Packet size.                                    */
//
//  uint32_t phy_itface;              /*!< Select the used PHY interface.
//                                         This parameter can be any value of @ref USB_Core_PHY                   */
//
//  uint32_t Sof_enable;              /*!< Enable or disable the output of the SOF signal.                        */
//
//  uint32_t low_power_enable;        /*!< Enable or disable the low power mode.                                  */
//
//  uint32_t lpm_enable;              /*!< Enable or disable Link Power Management.                               */
//
//  uint32_t battery_charging_enable; /*!< Enable or disable Battery charging.                                    */
//
//  uint32_t vbus_sensing_enable;     /*!< Enable or disable the VBUS Sensing feature.                            */
//
//  uint32_t use_dedicated_ep1;       /*!< Enable or disable the use of the dedicated EP1 interrupt.              */
//
//  uint32_t use_external_vbus;       /*!< Enable or disable the use of the external VBUS.                        */
//
//} USB_OTG_CfgTypeDef;

//typedef struct
//{
//  uint8_t   num;                  /*!< Endpoint number
//                                       This parameter must be a number between Min_Data = 1 and Max_Data = 15   */
//
//  uint8_t   is_in;                /*!< Endpoint direction
//                                       This parameter must be a number between Min_Data = 0 and Max_Data = 1    */
//
//  uint8_t   is_stall;             /*!< Endpoint stall condition
//                                       This parameter must be a number between Min_Data = 0 and Max_Data = 1    */
//
//  uint8_t   type;                 /*!< Endpoint type
//                                       This parameter can be any value of @ref USB_EP_Type_                     */
//
//  uint8_t   data_pid_start;       /*!< Initial data PID
//                                       This parameter must be a number between Min_Data = 0 and Max_Data = 1    */
//
//  uint8_t   even_odd_frame;       /*!< IFrame parity
//                                       This parameter must be a number between Min_Data = 0 and Max_Data = 1    */
//  uint32_t  maxpacket;            /*!< Endpoint Max packet size
//                                       This parameter must be a number between Min_Data = 0 and Max_Data = 64KB */
//
//  uint8_t   *xfer_buff;           /*!< Pointer to transfer buffer                                               */
//  uintptr_t  dma_addr;       /*!< 32 bits aligned transfer buffer address                                  */
//
//
//  uint32_t  xfer_len;             /*!< Current transfer length                                                  */
//
//  uint32_t  xfer_count;           /*!< Partial transfer length in case of multi packet transfer                 */
//} USB_OTG_EPTypeDef;
//
//typedef struct
//{
//  uint8_t   dev_addr;           /*!< USB device address.
//                                     This parameter must be a number between Min_Data = 1 and Max_Data = 255    */
//
//  uint8_t   ch_num;             /*!< Host channel number.
//                                     This parameter must be a number between Min_Data = 1 and Max_Data = 15     */
//
//  uint8_t   ep_num;             /*!< Endpoint number.
//                                     This parameter must be a number between Min_Data = 1 and Max_Data = 15     */
//
//  uint8_t   ep_is_in;           /*!< Endpoint direction
//                                     This parameter must be a number between Min_Data = 0 and Max_Data = 1      */
//
//  uint8_t   speed;              /*!< USB Host speed.
//                                     This parameter can be any value of @ref USB_Core_Speed_                    */
//
//  uint8_t   do_ping;            /*!< Enable or disable the use of the PING protocol for HS mode.                */
//
//  uint8_t   process_ping;       /*!< Execute the PING protocol for HS mode.                                     */
//
//  uint8_t   ep_type;            /*!< Endpoint Type.
//                                     This parameter can be any value of @ref USB_EP_Type_                       */
//
//  uint16_t  max_packet;         /*!< Endpoint Max packet size.
//                                     This parameter must be a number between Min_Data = 0 and Max_Data = 64KB   */
//
//  uint8_t   data_pid;           /*!< Initial data PID.
//                                     This parameter must be a number between Min_Data = 0 and Max_Data = 1      */
//
//  uint8_t   *xfer_buff;         /*!< Pointer to transfer buffer.                                                */
//
//  uint32_t  XferSize;             /*!< OTG Channel transfer size.                                                   */
//
//  uint32_t  xfer_len;           /*!< Current transfer length.                                                   */
//
//  uint32_t  xfer_count;         /*!< Partial transfer length in case of multi packet transfer.                  */
//
//  uint8_t   toggle_in;          /*!< IN transfer current toggle flag.
//                                     This parameter must be a number between Min_Data = 0 and Max_Data = 1      */
//
//  uint8_t   toggle_out;         /*!< OUT transfer current toggle flag
//                                     This parameter must be a number between Min_Data = 0 and Max_Data = 1      */
//
//  uint32_t  dma_addr;           /*!< 32 bits aligned transfer buffer address.                                   */
//
//  uint32_t  ErrCnt;             /*!< Host channel error count.                                                  */
//
//  USB_OTG_URBStateTypeDef urb_state;  /*!< URB state.
//                                            This parameter can be any value of @ref USB_OTG_URBStateTypeDef */
//
//  USB_OTG_HCStateTypeDef state;       /*!< Host Channel state.
//                                            This parameter can be any value of @ref USB_OTG_HCStateTypeDef  */
//
//  uint8_t tt_hubaddr;
//  uint8_t tt_prtaddr;
//} USB_OTG_HCTypeDef;


/**
  * @brief  PCD State structure definition
  */
typedef enum
{
  HAL_PCD_STATE_RESET   = 0x00,
  HAL_PCD_STATE_READY   = 0x01,
  HAL_PCD_STATE_ERROR   = 0x02,
  HAL_PCD_STATE_BUSY    = 0x03,
  HAL_PCD_STATE_TIMEOUT = 0x04
} PCD_StateTypeDef;

/* Device LPM suspend state */
typedef enum
{
  LPM_L0 = 0x00, /* on */
  LPM_L1 = 0x01, /* LPM L1 sleep */
  LPM_L2 = 0x02, /* suspend */
  LPM_L3 = 0x03, /* off */
} PCD_LPM_StateTypeDef;

typedef enum
{
  PCD_LPM_L0_ACTIVE = 0x00, /* on */
  PCD_LPM_L1_ACTIVE = 0x01, /* LPM L1 sleep */
} PCD_LPM_MsgTypeDef;

typedef enum
{
  PCD_BCD_ERROR                     = 0xFF,
  PCD_BCD_CONTACT_DETECTION         = 0xFE,
  PCD_BCD_STD_DOWNSTREAM_PORT       = 0xFD,
  PCD_BCD_CHARGING_DOWNSTREAM_PORT  = 0xFC,
  PCD_BCD_DEDICATED_CHARGING_PORT   = 0xFB,
  PCD_BCD_DISCOVERY_COMPLETED       = 0x00,

} PCD_BCD_MsgTypeDef;

typedef USBOTG_TypeDef  PCD_TypeDef;
typedef USB_OTG_CfgTypeDef     PCD_InitTypeDef;
typedef USB_OTG_EPTypeDef      PCD_EPTypeDef;


#define PIPE_NUM      (16)

typedef struct {
	int        enable;
	uint16_t    status;
	uint32_t    req_size;
	uint32_t    data_cnt;
	uint8_t     *p_data;
} pipe_ctrl_t;

/**
  * @brief  PCD Handle Structure definition
  */
#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
typedef struct __PCD_HandleTypeDef
#else
typedef struct
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  PCD_TypeDef             *Instance;   /*!< Register base address             */
  PCD_InitTypeDef         Init;        /*!< PCD required parameters           */
  __IO uint8_t            USB_Address; /*!< USB Address                       */
  PCD_EPTypeDef           IN_ep[16];   /*!< IN endpoint parameters            */
  PCD_EPTypeDef           OUT_ep[16];  /*!< OUT endpoint parameters           */
  HAL_LockTypeDef         Lock;        /*!< PCD peripheral status             */
  __IO PCD_StateTypeDef   State;       /*!< PCD communication state           */
  __IO  uint32_t          ErrorCode;   /*!< PCD Error code                    */
  PCD_LPM_StateTypeDef    LPM_State;   /*!< LPM State                         */
  uint32_t                BESL;
  __ALIGN_BEGIN uint32_t Setup [12] __ALIGN_END;  /*!< Setup packet buffer                */


  uint32_t lpm_active;                 /*!< Enable or disable the Link Power Management .
                                       This parameter can be set to ENABLE or DISABLE        */

  uint32_t battery_charging_active;    /*!< Enable or disable Battery charging.
                                       This parameter can be set to ENABLE or DISABLE        */
  void                    *pData;      /*!< Pointer to upper stack Handler */

  usb_struct awxx_usb;			/* RC0LAM (repstosw) usb stack */

#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
  void (* SOFCallback)(struct __PCD_HandleTypeDef *hpcd);                              /*!< USB OTG PCD SOF callback                */
  void (* SetupStageCallback)(struct __PCD_HandleTypeDef *hpcd);                       /*!< USB OTG PCD Setup Stage callback        */
  void (* ResetCallback)(struct __PCD_HandleTypeDef *hpcd);                            /*!< USB OTG PCD Reset callback              */
  void (* SuspendCallback)(struct __PCD_HandleTypeDef *hpcd);                          /*!< USB OTG PCD Suspend callback            */
  void (* ResumeCallback)(struct __PCD_HandleTypeDef *hpcd);                           /*!< USB OTG PCD Resume callback             */
  void (* ConnectCallback)(struct __PCD_HandleTypeDef *hpcd);                          /*!< USB OTG PCD Connect callback            */
  void (* DisconnectCallback)(struct __PCD_HandleTypeDef *hpcd);                       /*!< USB OTG PCD Disconnect callback         */

  void (* DataOutStageCallback)(struct __PCD_HandleTypeDef *hpcd, uint8_t epnum);      /*!< USB OTG PCD Data OUT Stage callback     */
  void (* DataInStageCallback)(struct __PCD_HandleTypeDef *hpcd, uint8_t epnum);       /*!< USB OTG PCD Data IN Stage callback      */
  void (* ISOOUTIncompleteCallback)(struct __PCD_HandleTypeDef *hpcd, uint8_t epnum);  /*!< USB OTG PCD ISO OUT Incomplete callback */
  void (* ISOINIncompleteCallback)(struct __PCD_HandleTypeDef *hpcd, uint8_t epnum);   /*!< USB OTG PCD ISO IN Incomplete callback  */
  void (* BCDCallback)(struct __PCD_HandleTypeDef *hpcd, PCD_BCD_MsgTypeDef msg);      /*!< USB OTG PCD BCD callback                */
  void (* LPMCallback)(struct __PCD_HandleTypeDef *hpcd, PCD_LPM_MsgTypeDef msg);      /*!< USB OTG PCD LPM callback                */

  void (* MspInitCallback)(struct __PCD_HandleTypeDef *hpcd);                          /*!< USB OTG PCD Msp Init callback           */
  void (* MspDeInitCallback)(struct __PCD_HandleTypeDef *hpcd);                        /*!< USB OTG PCD Msp DeInit callback         */
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
} PCD_HandleTypeDef;

/**
  * @}
  */

/** @addtogroup STM32H7xx_HAL_Driver
  * @{
  */

/** @addtogroup HCD HCD
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup HCD_Exported_Types HCD Exported Types
  * @{
  */

/** @defgroup HCD_Exported_Types_Group1 HCD State Structure definition
  * @{
  */
//typedef enum
//{
//  HAL_HCD_STATE_RESET    = 0x00,
//  HAL_HCD_STATE_READY    = 0x01,
//  HAL_HCD_STATE_ERROR    = 0x02,
//  HAL_HCD_STATE_BUSY     = 0x03,
//  HAL_HCD_STATE_TIMEOUT  = 0x04
//} HCD_StateTypeDef;

//typedef USBOTG_TypeDef   HCD_TypeDef;
typedef USB_OTG_CfgTypeDef      HCD_InitTypeDef;
typedef USB_OTG_HCTypeDef       RZ_HCD_HCTypeDef;
typedef USB_OTG_URBStateTypeDef HCD_URBStateTypeDef;
typedef USB_OTG_HCStateTypeDef  HCD_HCStateTypeDef;
/**
  * @}
  */

/** @defgroup HCD_Exported_Types_Group2 HCD Handle Structure definition
  * @{
  */
//#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
//typedef struct __HCD_HandleTypeDef
//#else
//typedef struct
//#endif /* USE_HAL_HCD_REGISTER_CALLBACKS */
//{
//  HCD_TypeDef               *Instance;  /*!< Register base address    */
//  HCD_InitTypeDef           Init;       /*!< HCD required parameters  */
//  RZ_HCD_HCTypeDef          hc[16];     /*!< Host channels parameters */
//  HAL_LockTypeDef           Lock;       /*!< HCD peripheral status    */
//  __IO HCD_StateTypeDef     State;      /*!< HCD communication state  */
//  __IO  uint32_t            ErrorCode;  /*!< HCD Error code           */
//  void                      *pData;     /*!< Pointer Stack Handler    */
//#if (USE_HAL_HCD_REGISTER_CALLBACKS == 1U)
//  void (* SOFCallback)(struct __HCD_HandleTypeDef *hhcd);                               /*!< USB OTG HCD SOF callback                */
//  void (* ConnectCallback)(struct __HCD_HandleTypeDef *hhcd);                           /*!< USB OTG HCD Connect callback            */
//  void (* DisconnectCallback)(struct __HCD_HandleTypeDef *hhcd);                        /*!< USB OTG HCD Disconnect callback         */
//  void (* PortEnabledCallback)(struct __HCD_HandleTypeDef *hhcd);                       /*!< USB OTG HCD Port Enable callback        */
//  void (* PortDisabledCallback)(struct __HCD_HandleTypeDef *hhcd);                      /*!< USB OTG HCD Port Disable callback       */
//  void (* HC_NotifyURBChangeCallback)(struct __HCD_HandleTypeDef *hhcd, uint8_t chnum,
//                                      HCD_URBStateTypeDef urb_state);                   /*!< USB OTG HCD Host Channel Notify URB Change callback  */
//
//  void (* MspInitCallback)(struct __HCD_HandleTypeDef *hhcd);                           /*!< USB OTG HCD Msp Init callback           */
//  void (* MspDeInitCallback)(struct __HCD_HandleTypeDef *hhcd);                         /*!< USB OTG HCD Msp DeInit callback         */
//#endif /* USE_HAL_HCD_REGISTER_CALLBACKS */
//} HCD_HandleTypeDef;
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup USB_LL_Exported_Functions USB Low Layer Exported Functions
  * @{
  */
HAL_StatusTypeDef USB_CoreInit(USBOTG_TypeDef *USBx, USB_OTG_CfgTypeDef cfg);
HAL_StatusTypeDef USB_DevInit(USBOTG_TypeDef *USBx, USB_OTG_CfgTypeDef cfg);
HAL_StatusTypeDef USB_EnableGlobalInt(USBOTG_TypeDef *USBx);
HAL_StatusTypeDef USB_DisableGlobalInt(USBOTG_TypeDef *USBx);
HAL_StatusTypeDef USB_SetTurnaroundTime(USBOTG_TypeDef *USBx, uint32_t hclk, uint8_t speed);
HAL_StatusTypeDef USB_SetCurrentMode(USBOTG_TypeDef *USBx, USB_OTG_ModeTypeDef mode);
HAL_StatusTypeDef USB_SetDevSpeed(USBOTG_TypeDef *USBx, uint8_t speed);
HAL_StatusTypeDef USB_FlushRxFifo(USBOTG_TypeDef *USBx);
HAL_StatusTypeDef USB_FlushTxFifo(USBOTG_TypeDef *USBx, uint32_t num);
HAL_StatusTypeDef USB_ActivateEndpoint(USBOTG_TypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_DeactivateEndpoint(USBOTG_TypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_ActivateDedicatedEndpoint(USBOTG_TypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_DeactivateDedicatedEndpoint(USBOTG_TypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_EPStartXfer(USBOTG_TypeDef *USBx, USB_OTG_EPTypeDef *ep, uint8_t dma);
HAL_StatusTypeDef USB_EP0StartXfer(USBOTG_TypeDef *USBx, USB_OTG_EPTypeDef *ep, uint8_t dma);
HAL_StatusTypeDef USB_WritePacket(USBOTG_TypeDef *USBx, uint8_t *src,
                                  uint8_t ch_ep_num, uint16_t len, uint8_t dma);

void             *USB_ReadPacket(USBOTG_TypeDef *USBx, uint8_t *dest, uint16_t len);
HAL_StatusTypeDef USB_EPSetStall(USBOTG_TypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_EPClearStall(USBOTG_TypeDef *USBx, USB_OTG_EPTypeDef *ep);
HAL_StatusTypeDef USB_SetDevAddress(USBOTG_TypeDef *USBx, uint8_t address);
HAL_StatusTypeDef USB_DevConnect(USBOTG_TypeDef *USBx);
HAL_StatusTypeDef USB_DevDisconnect(USBOTG_TypeDef *USBx);
HAL_StatusTypeDef USB_StopDevice(USBOTG_TypeDef *USBx);
HAL_StatusTypeDef USB_ActivateSetup(USBOTG_TypeDef *USBx);
HAL_StatusTypeDef USB_EP0_OutStart(USBOTG_TypeDef *USBx, uint8_t dma, uint8_t *psetup);
uint8_t           USB_GetDevSpeed(USBOTG_TypeDef *USBx);
uint32_t          USB_GetMode(USBOTG_TypeDef *USBx);
uint32_t          USB_ReadInterrupts(USBOTG_TypeDef *USBx);
uint32_t          USB_ReadDevAllOutEpInterrupt(USBOTG_TypeDef *USBx);
uint32_t          USB_ReadDevOutEPInterrupt(USBOTG_TypeDef *USBx, uint8_t epnum);
uint32_t          USB_ReadDevAllInEpInterrupt(USBOTG_TypeDef *USBx);
uint32_t          USB_ReadDevInEPInterrupt(USBOTG_TypeDef *USBx, uint8_t epnum);
void              USB_ClearInterrupts(USBOTG_TypeDef *USBx, uint32_t interrupt);

HAL_StatusTypeDef USB_HostInit(USBOTG_TypeDef *USBx, USB_OTG_CfgTypeDef cfg);
HAL_StatusTypeDef USB_InitFSLSPClkSel(USBOTG_TypeDef *USBx, uint8_t freq);
HAL_StatusTypeDef USB_ResetPort(USBOTG_TypeDef *USBx);
HAL_StatusTypeDef USB_ResetPort2(USBOTG_TypeDef *USBx, uint8_t status);
HAL_StatusTypeDef USB_DriveVbus(USBOTG_TypeDef *USBx, uint8_t state);
uint32_t          USB_GetHostSpeed(USBOTG_TypeDef *USBx);
uint32_t          USB_GetCurrentFrame(USBOTG_TypeDef *USBx);
HAL_StatusTypeDef USB_HC_Init(USBOTG_TypeDef *USBx, uint8_t ch_num,
                              uint8_t epnum, uint8_t dev_address, uint8_t speed,
                              uint8_t ep_type, uint16_t mps, uint8_t tt_hubaddr, uint8_t tt_prtaddr);
HAL_StatusTypeDef USB_HC_StartXfer(USBOTG_TypeDef *USBx,
                                   USB_OTG_HCTypeDef *hc, uint8_t dma);

uint32_t          USB_HC_ReadInterrupt(USBOTG_TypeDef *USBx);
HAL_StatusTypeDef USB_HC_Halt(USBOTG_TypeDef *USBx, uint8_t hc_num);
HAL_StatusTypeDef USB_DoPing(USBOTG_TypeDef *USBx, uint8_t ch_num);
HAL_StatusTypeDef USB_StopHost(USBOTG_TypeDef *USBx);
HAL_StatusTypeDef USB_ActivateRemoteWakeup(USBOTG_TypeDef *USBx);
HAL_StatusTypeDef USB_DeActivateRemoteWakeup(USBOTG_TypeDef *USBx);

/* Exported functions --------------------------------------------------------*/
/** @addtogroup PCD_Exported_Functions PCD Exported Functions
  * @{
  */

/* Initialization/de-initialization functions  ********************************/
/** @addtogroup PCD_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeInit(PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd);

//HAL_StatusTypeDef HAL_HCD_Init(HCD_HandleTypeDef *hpcd);
//HAL_StatusTypeDef HAL_HCD_DeInit (HCD_HandleTypeDef *hpcd);
//void HAL_HCD_MspInit(HCD_HandleTypeDef *hpcd);
//void HAL_HCD_MspDeInit(HCD_HandleTypeDef *hpcd);

#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
/** @defgroup HAL_PCD_Callback_ID_enumeration_definition HAL USB OTG PCD Callback ID enumeration definition
  * @brief  HAL USB OTG PCD Callback ID enumeration definition
  * @{
  */
typedef enum
{
  HAL_PCD_SOF_CB_ID          = 0x01,      /*!< USB PCD SOF callback ID          */
  HAL_PCD_SETUPSTAGE_CB_ID   = 0x02,      /*!< USB PCD Setup Stage callback ID  */
  HAL_PCD_RESET_CB_ID        = 0x03,      /*!< USB PCD Reset callback ID        */
  HAL_PCD_SUSPEND_CB_ID      = 0x04,      /*!< USB PCD Suspend callback ID      */
  HAL_PCD_RESUME_CB_ID       = 0x05,      /*!< USB PCD Resume callback ID       */
  HAL_PCD_CONNECT_CB_ID      = 0x06,      /*!< USB PCD Connect callback ID      */
  HAL_PCD_DISCONNECT_CB_ID   = 0x07,      /*!< USB PCD Disconnect callback ID   */

  HAL_PCD_MSPINIT_CB_ID      = 0x08,      /*!< USB PCD MspInit callback ID      */
  HAL_PCD_MSPDEINIT_CB_ID    = 0x09       /*!< USB PCD MspDeInit callback ID    */

} HAL_PCD_CallbackIDTypeDef;
/**
  * @}
  */

/** @defgroup HAL_PCD_Callback_pointer_definition HAL USB OTG PCD Callback pointer definition
  * @brief  HAL USB OTG PCD Callback pointer definition
  * @{
  */

typedef void (*pPCD_CallbackTypeDef)(PCD_HandleTypeDef *hpcd);                                   /*!< pointer to a common USB OTG PCD callback function  */
typedef void (*pPCD_DataOutStageCallbackTypeDef)(PCD_HandleTypeDef *hpcd, uint8_t epnum);        /*!< pointer to USB OTG PCD Data OUT Stage callback     */
typedef void (*pPCD_DataInStageCallbackTypeDef)(PCD_HandleTypeDef *hpcd, uint8_t epnum);         /*!< pointer to USB OTG PCD Data IN Stage callback      */
typedef void (*pPCD_IsoOutIncpltCallbackTypeDef)(PCD_HandleTypeDef *hpcd, uint8_t epnum);        /*!< pointer to USB OTG PCD ISO OUT Incomplete callback */
typedef void (*pPCD_IsoInIncpltCallbackTypeDef)(PCD_HandleTypeDef *hpcd, uint8_t epnum);         /*!< pointer to USB OTG PCD ISO IN Incomplete callback  */
typedef void (*pPCD_LpmCallbackTypeDef)(PCD_HandleTypeDef *hpcd, PCD_LPM_MsgTypeDef msg);        /*!< pointer to USB OTG PCD LPM callback                */
typedef void (*pPCD_BcdCallbackTypeDef)(PCD_HandleTypeDef *hpcd, PCD_BCD_MsgTypeDef msg);        /*!< pointer to USB OTG PCD BCD callback                */

/**
  * @}
  */

HAL_StatusTypeDef HAL_PCD_RegisterCallback(PCD_HandleTypeDef *hpcd,
                                           HAL_PCD_CallbackIDTypeDef CallbackID,
                                           pPCD_CallbackTypeDef pCallback);

HAL_StatusTypeDef HAL_PCD_UnRegisterCallback(PCD_HandleTypeDef *hpcd,
                                             HAL_PCD_CallbackIDTypeDef CallbackID);

HAL_StatusTypeDef HAL_PCD_RegisterDataOutStageCallback(PCD_HandleTypeDef *hpcd,
                                                       pPCD_DataOutStageCallbackTypeDef pCallback);

HAL_StatusTypeDef HAL_PCD_UnRegisterDataOutStageCallback(PCD_HandleTypeDef *hpcd);

HAL_StatusTypeDef HAL_PCD_RegisterDataInStageCallback(PCD_HandleTypeDef *hpcd,
                                                      pPCD_DataInStageCallbackTypeDef pCallback);

HAL_StatusTypeDef HAL_PCD_UnRegisterDataInStageCallback(PCD_HandleTypeDef *hpcd);

HAL_StatusTypeDef HAL_PCD_RegisterIsoOutIncpltCallback(PCD_HandleTypeDef *hpcd,
                                                       pPCD_IsoOutIncpltCallbackTypeDef pCallback);

HAL_StatusTypeDef HAL_PCD_UnRegisterIsoOutIncpltCallback(PCD_HandleTypeDef *hpcd);

HAL_StatusTypeDef HAL_PCD_RegisterIsoInIncpltCallback(PCD_HandleTypeDef *hpcd,
                                                      pPCD_IsoInIncpltCallbackTypeDef pCallback);

HAL_StatusTypeDef HAL_PCD_UnRegisterIsoInIncpltCallback(PCD_HandleTypeDef *hpcd);

HAL_StatusTypeDef HAL_PCD_RegisterBcdCallback(PCD_HandleTypeDef *hpcd,
                                              pPCD_BcdCallbackTypeDef pCallback);

HAL_StatusTypeDef HAL_PCD_UnRegisterBcdCallback(PCD_HandleTypeDef *hpcd);

HAL_StatusTypeDef HAL_PCD_RegisterLpmCallback(PCD_HandleTypeDef *hpcd,
                                              pPCD_LpmCallbackTypeDef pCallback);

HAL_StatusTypeDef HAL_PCD_UnRegisterLpmCallback(PCD_HandleTypeDef *hpcd);
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
/**
  * @}
  */

/* I/O operation functions  ***************************************************/
/* Non-Blocking mode: Interrupt */
/** @addtogroup PCD_Exported_Functions_Group2 Input and Output operation functions
  * @{
  */
HAL_StatusTypeDef HAL_PCD_Start(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd);
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd);

void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
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

void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd);
void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd);


/* Peripheral Control functions  **********************************************/
/** @addtogroup PCD_Exported_Functions_Group3 Peripheral Control functions
  * @{
  */
HAL_StatusTypeDef HAL_PCD_DevConnect(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DevDisconnect(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_SetAddress(PCD_HandleTypeDef *hpcd, uint8_t address);
HAL_StatusTypeDef HAL_PCD_EP_Open(PCD_HandleTypeDef *hpcd, uint8_t ep_addr,
                                  uint16_t ep_mps, uint8_t ep_type);

HAL_StatusTypeDef HAL_PCD_EP_Close(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint8_t ep_addr,
                                     uint8_t *pBuf, uint32_t len);

HAL_StatusTypeDef HAL_PCD_EP_Transmit(PCD_HandleTypeDef *hpcd, uint8_t ep_addr,
                                      const uint8_t *pBuf, uint32_t len);


HAL_StatusTypeDef HAL_PCD_EP_SetStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_ClrStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_Flush(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_ActivateRemoteWakeup(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeActivateRemoteWakeup(PCD_HandleTypeDef *hpcd);

uint32_t          HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);


void HAL_PCD_AdressedCallback(PCD_HandleTypeDef *hpcd);	// RENESAS specific

/**
  * @}
  */

#define EP_ADDR_MSK                            0xFU


/** @defgroup USB_LL_EP_Type USB Low Layer EP Type
  * @{
  */
#define EP_TYPE_CTRL                           0U
#define EP_TYPE_ISOC                           1U
#define EP_TYPE_BULK                           2U
#define EP_TYPE_INTR                           3U
#define EP_TYPE_MSK                            3U
/**
  * @}
  */

//uint16_t USBPhyHw_EP2PIPE(uint16_t ep_addr);

//#define __HAL_HCD_ENABLE(__HANDLE__)       //            (void)USB_EnableGlobalInt ((__HANDLE__)->Instance)
//#define __HAL_HCD_DISABLE(__HANDLE__)       //           (void)USB_DisableGlobalInt ((__HANDLE__)->Instance)


/** @defgroup USBH_CORE_Private_Defines
  * @{
  */


//#define USBH_MAX_PIPES_NBR                             15

#define USBHNPIPES 15

/* Exported macro ------------------------------------------------------------*/

/**
  * @}
  */
/* I/O operation functions  ***************************************************/
/** @addtogroup HCD_Exported_Functions_Group2 Input and Output operation functions
  * @{
  */
//HAL_StatusTypeDef HAL_HCD_HC_SubmitRequest(HCD_HandleTypeDef *hhcd, uint8_t ch_num,
//                                           uint8_t direction, uint8_t ep_type,
//                                           uint8_t token, uint8_t *pbuff,
//										   uint32_t length, uint8_t do_ping);
//
///* Non-Blocking mode: Interrupt */
//void HAL_HCD_IRQHandler(HCD_HandleTypeDef *hhcd);
//void HAL_HCD_SOF_Callback(HCD_HandleTypeDef *hhcd);
//void HAL_HCD_Connect_Callback(HCD_HandleTypeDef *hhcd);
//void HAL_HCD_Disconnect_Callback(HCD_HandleTypeDef *hhcd);
//void HAL_HCD_PortEnabled_Callback(HCD_HandleTypeDef *hhcd);
//void HAL_HCD_PortDisabled_Callback(HCD_HandleTypeDef *hhcd);
//void HAL_HCD_HC_NotifyURBChange_Callback(HCD_HandleTypeDef *hhcd, uint8_t chnum,
//                                         HCD_URBStateTypeDef urb_state);
///**
//  * @}
//  */
//
//HAL_StatusTypeDef HAL_HCD_Init(HCD_HandleTypeDef *hhcd);
//HAL_StatusTypeDef HAL_HCD_DeInit(HCD_HandleTypeDef *hhcd);
//HAL_StatusTypeDef HAL_HCD_HC_Init(HCD_HandleTypeDef *hhcd, uint8_t ch_num,
//                                  uint8_t epnum, uint8_t dev_address,
//                                  uint8_t speed, uint8_t ep_type, uint16_t mps, uint8_t tt_hubaddr, uint8_t tt_prtaddr);
//
//HAL_StatusTypeDef HAL_HCD_HC_Halt(HCD_HandleTypeDef *hhcd, uint8_t ch_num);
//void              HAL_HCD_MspInit(HCD_HandleTypeDef *hhcd);
//void              HAL_HCD_MspDeInit(HCD_HandleTypeDef *hhcd);
//void HAL_HCD_IRQHandler(HCD_HandleTypeDef *hhcd);
//HAL_StatusTypeDef HAL_HCD_ResetPort(HCD_HandleTypeDef *hhcd);
//HAL_StatusTypeDef HAL_HCD_ResetPort2(HCD_HandleTypeDef *hhcd, uint8_t resetActiveState);
//HAL_StatusTypeDef HAL_HCD_Start(HCD_HandleTypeDef *hhcd);
//HAL_StatusTypeDef HAL_HCD_Stop(HCD_HandleTypeDef *hhcd);
//
///* Peripheral State functions  ************************************************/
///** @addtogroup HCD_Exported_Functions_Group4 Peripheral State functions
//  * @{
//  */
//HCD_StateTypeDef        HAL_HCD_GetState(HCD_HandleTypeDef *hhcd);
//HCD_URBStateTypeDef     HAL_HCD_HC_GetURBState(HCD_HandleTypeDef *hhcd, uint8_t chnum);
//HCD_HCStateTypeDef      HAL_HCD_HC_GetState(HCD_HandleTypeDef *hhcd, uint8_t chnum);
//uint32_t                HAL_HCD_HC_GetXferCount(HCD_HandleTypeDef *hhcd, uint8_t chnum);
//uint32_t                HAL_HCD_HC_GetMaxPacket(HCD_HandleTypeDef *hhcd, uint8_t chnum);
//uint32_t                HAL_HCD_GetCurrentFrame(HCD_HandleTypeDef *hhcd);
//uint32_t                HAL_HCD_GetCurrentSpeed(HCD_HandleTypeDef *hhcd);
//uint_fast8_t 			HAL_HCD_GetCurrentSpeedReady(HCD_HandleTypeDef *hhcd);

/**
  * @}
  */

/** @defgroup HCD_Speed PCD Speed
  * @{
  */
//#define HCD_SPEED_HIGH               0U
//#define HCD_SPEED_HIGH_IN_FULL       1U
//#define HCD_SPEED_FULL               2U
/**
  * @}
  */

//#define PCD_PHY_ULPI                 1U
//#define PCD_PHY_EMBEDDED             2U
//
//#define HC_PID_DATA0                           0U
//#define HC_PID_DATA2                           1U
//#define HC_PID_DATA1                           2U
//#define HC_PID_SETUP                           3U


/* Common registers */

//#define	MUSB2_REG_FADDR 0x0000		/* function address register */
//#define	MUSB2_MASK_FADDR 0x7F

//#define	MUSB2_REG_POWER 0x0001		/* power register */
#define	MUSB2_MASK_SUSPM_ENA 0x01
#define	MUSB2_MASK_SUSPMODE 0x02
#define	MUSB2_MASK_RESUME 0x04
#define	MUSB2_MASK_RESET 0x08
#define	MUSB2_MASK_HSMODE 0x10
#define	MUSB2_MASK_HSENAB 0x20
#define	MUSB2_MASK_SOFTC 0x40
#define	MUSB2_MASK_ISOUPD 0x80


//#define USB_CSRX_REQPKT            	(0x1u << 21)
#define USB_TXCSR_AUTOSET			(0x1u << 15)
#define USB_TXCSR_ISO				(0x1u << 14)
#define USB_TXCSR_TXFIFO			(0x1u << 13)
#define USB_TXCSR_DMAREQEN			(0x1u << 12)
#define USB_TXCSR_DMAREQMODE		(0x1u << 10)
#define USB_TXCSR_RXSTALL			(0x1u << 5)
#define USB_TXCSR_FLUSHFIFO			(0x1u << 3)
#define USB_TXCSR_ERROR				(0x1u << 2)
#define USB_TXCSR_FIFONOTEMP		(0x1u << 1)
#define USB_TXCSR_TXPKTRDY			(0x1u << 0)

#define USB_BUSINT_VBUSERROR		(0x1u << 7)
#define USB_BUSINT_SESSREQ			(0x1u << 6)
#define USB_BUSINT_SESSEND			(0x1u << 5)
#define USB_BUSINT_DISCONN			(0x1u << 5)
#define USB_BUSINT_CONNECT			(0x1u << 4)
#define USB_BUSINT_SOF				(0x1u << 3)
#define USB_BUSINT_RESET			(0x1u << 2)
#define USB_BUSINT_BABBLE			(0x1u << 2)
#define USB_BUSINT_RESUME			(0x1u << 1)
#define USB_BUSINT_SUSPEND			(0x1u << 0)

#define USB_RXCSR_AUTOCLR			(0x1u << 15)
#define USB_RXCSR_AUTOREQ			(0x1u << 14)  //for Host only
#define USB_RXCSR_ISO				(0x1u << 14)  //for device only
#define USB_RXCSR_DMAREQEN			(0x1u << 13)
#define USB_RXCSR_PIDERROR			(0x1u << 12)  //for Host only
#define USB_RXCSR_DMAREQMODE		(0x1u << 11)
#define USB_RXCSR_RXSTALL			(0x1u << 6)   //for Host only
#define USB_RXCSR_REQPKT			(0x1u << 5)   //for Host only
#define USB_RXCSR_FLUSHFIFO			(0x1u << 4)
#define USB_RXCSR_ERROR				(0x1u << 2)
#define USB_RXCSR_FIFOFULL			(0x1u << 1)
#define USB_RXCSR_RXPKTRDY			(0x1u << 0)

// 0xF7
#define USB_BUSINT_DEV_WORK ( \
		0*USB_BUSINT_VBUSERROR | \
		0*USB_BUSINT_SESSREQ | \
		USB_BUSINT_DISCONN | \
		USB_BUSINT_CONNECT | \
		1*USB_BUSINT_SOF | \
		USB_BUSINT_RESET | \
		USB_BUSINT_RESUME | \
		USB_BUSINT_SUSPEND | \
		0)




#ifdef __cplusplus
}
#endif



#endif /* SRC_T113_S3_HAL_USB_H_ */
