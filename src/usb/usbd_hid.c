
#include "hardware.h"

#if WITHUSBHW && WITHUSBHID

#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_def.h"
#include "usbd_core.h"
//#include "usb200.h"
//#include "usbch9.h"

//#define HID_EPIN_ADDR                              0x81U
//#define HID_EPIN_SIZE                              0x04U

//#define USB_HID_CONFIG_DESC_SIZ                    34U
//#define USB_HID_DESC_SIZ                           9U
//#define HID_MOUSE_REPORT_DESC_SIZE                 74U

//#define HID_DESCRIPTOR_TYPE                        0x21U
//#define HID_REPORT_DESC                            0x22U

#ifndef HID_HS_BINTERVAL
#define HID_HS_BINTERVAL                           0x07U
#endif /* HID_HS_BINTERVAL */

#ifndef HID_FS_BINTERVAL
#define HID_FS_BINTERVAL                           0x0AU
#endif /* HID_FS_BINTERVAL */

#define HID_REQ_SET_PROTOCOL                       0x0BU
#define HID_REQ_GET_PROTOCOL                       0x03U

#define HID_REQ_SET_IDLE                           0x0AU
#define HID_REQ_GET_IDLE                           0x02U

#define HID_REQ_SET_REPORT                         0x09U
#define HID_REQ_GET_REPORT                         0x01U

typedef enum
{
  HID_IDLE = 0,
  HID_BUSY,
} HID_StateTypeDef;


typedef struct
{
  uint32_t Protocol;
  uint32_t IdleState;
  uint32_t AltSetting;
  HID_StateTypeDef state;
} USBD_HID_HandleTypeDef;

static USBD_HID_HandleTypeDef hHid;

/**
  * @brief  USBD_HID_SendReport
  *         Send HID Report
  * @param  pdev: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t USBD_HID_SendReport(USBD_HandleTypeDef *pdev, uint8_t *report, uint16_t len)
{
  USBD_HID_HandleTypeDef * const hhid = & hHid;

  if (hhid == NULL)
  {
    return (uint8_t)USBD_FAIL;
  }

  if (pdev->dev_state == USBD_STATE_CONFIGURED)
  {
    if (hhid->state == HID_IDLE)
    {
      hhid->state = HID_BUSY;
      (void)USBD_LL_Transmit(pdev, HID_EPIN_ADDR, report, len);
    }
  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_HID_GetPollingInterval
  *         return polling interval from endpoint descriptor
  * @param  pdev: device instance
  * @retval polling interval
  */
uint32_t USBD_HID_GetPollingInterval(USBD_HandleTypeDef *pdev)
{
  uint32_t polling_interval;

  /* HIGH-speed endpoints */
  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    /* Sets the data transfer polling interval for high speed transfers.
     Values between 1..16 are allowed. Values correspond to interval
     of 2 ^ (bInterval-1). This option (8 ms, corresponds to HID_HS_BINTERVAL */
    polling_interval = (((1U << (HID_HS_BINTERVAL - 1U))) / 8U);
  }
  else   /* LOW and FULL-speed endpoints */
  {
    /* Sets the data transfer polling interval for low and full
    speed transfers */
    polling_interval =  HID_FS_BINTERVAL;
  }

  return ((uint32_t)(polling_interval));
}

static USBD_StatusTypeDef USBD_HID_Init(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{

	return USBD_OK;
}

static USBD_StatusTypeDef USBD_HID_DeInit(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{

	return USBD_OK;
}

static USBD_StatusTypeDef USBD_HID_Setup(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
 {
	USBD_HID_HandleTypeDef *const hhid = &hHid;
	USBD_StatusTypeDef ret = USBD_OK;
	uint16_t len;
	const uint8_t *pbuf;
	uint16_t status_info = 0U;
	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	if (hhid == NULL) {
		return (uint8_t) USBD_FAIL;
	}

	if (interfacev != INTERFACE_HID_CONTROL)
		return USBD_OK;

	switch (req->bmRequest & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_CLASS:
		switch (req->bRequest) {
		case HID_REQ_SET_PROTOCOL:
			hhid->Protocol = (uint8_t) (req->wValue);
			USBD_CtlSendStatus(pdev);
			break;

		case HID_REQ_GET_PROTOCOL:
			(void) USBD_CtlSendData(pdev, (uint8_t*) &hhid->Protocol, 1U);
			break;

		case HID_REQ_SET_IDLE:
			hhid->IdleState = (uint8_t) (req->wValue >> 8);
			USBD_CtlSendStatus(pdev);
			break;

		case HID_REQ_GET_IDLE:
			(void) USBD_CtlSendData(pdev, (uint8_t*) &hhid->IdleState, 1U);
			break;

		default:
			USBD_CtlError(pdev, req);
			ret = USBD_FAIL;
			break;
		}
		break;
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_GET_STATUS:
			if (pdev->dev_state == USBD_STATE_CONFIGURED) {
				(void) USBD_CtlSendData(pdev, (uint8_t*) &status_info, 2U);
			} else {
				USBD_CtlError(pdev, req);
				ret = USBD_FAIL;
			}
			break;
//
	        case USB_REQ_GET_DESCRIPTOR:
	          if ((req->wValue >> 8) == HID_REPORT_DESC)
	          {
	            len = MIN(HIDReportDescrTbl [0].size, req->wLength);
	            pbuf = HIDReportDescrTbl [0].data;
	          }
//	          else if ((req->wValue >> 8) == HID_DESCRIPTOR_TYPE)
//	          {
//	            pbuf = USBD_HID_Desc;
//	            len = MIN(USB_HID_DESC_SIZ, req->wLength);
//	          }
	          else
	          {
	            USBD_CtlError(pdev, req);
	            ret = USBD_FAIL;
	            break;
	          }
	          (void)USBD_CtlSendData(pdev, pbuf, len);
	          break;

		case USB_REQ_GET_INTERFACE:
			if (pdev->dev_state == USBD_STATE_CONFIGURED) {
				(void) USBD_CtlSendData(pdev, (uint8_t*) &hhid->AltSetting, 1U);
			} else {
				USBD_CtlError(pdev, req);
				ret = USBD_FAIL;
			}
			break;

		case USB_REQ_SET_INTERFACE:
			if (pdev->dev_state == USBD_STATE_CONFIGURED) {
				hhid->AltSetting = (uint8_t) (req->wValue);
				USBD_CtlSendStatus(pdev);
			} else {
				USBD_CtlError(pdev, req);
				ret = USBD_FAIL;
			}
			break;

		case USB_REQ_CLEAR_FEATURE:
			USBD_CtlSendStatus(pdev);
			break;

		default:
			USBD_CtlError(pdev, req);
			ret = USBD_FAIL;
			break;
		}
		break;

	default:
		USBD_CtlError(pdev, req);
		ret = USBD_FAIL;
		break;
	}

	return ret;
}

static USBD_StatusTypeDef USBD_HID_DataIn(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	  /* Ensure that the FIFO is empty before a new transfer, this condition could
	  be caused by  a new transfer before the end of the previous transfer */
	  //((USBD_HID_HandleTypeDef *)pdev->pClassData)->state = HID_IDLE;


	return USBD_OK;
}

static void USBD_HID_ColdInit(void)
{

}

const USBD_ClassTypeDef USBD_CLASS_HID =
{
	USBD_HID_ColdInit,
	USBD_HID_Init,
	USBD_HID_DeInit,
	USBD_HID_Setup,
	NULL,              /* EP0_TxSent */
	NULL,              /* EP0_RxReady */
	USBD_HID_DataIn,   /* DataIn */
	NULL,              /* DataOut */
	NULL,              /* SOF */
	NULL,
	NULL,
};


#endif /* WITHUSBHW && WITHUSBHID */
