/*
 * usbd_msc.c
 *
 *  Created on: Sep 7, 2022
 *      Author: gena
 */

#include "hardware.h"

#if WITHUSBHW && WITHUSBDMSC

#include "formats.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usb200.h"
#include "usbch9.h"



static void USBD_MSC_ColdInit(void)
{
}

const USBD_ClassTypeDef USBD_CLASS_MSC =
{
	USBD_MSC_ColdInit,
//	USBD_MSC_Init,
//	USBD_MSC_DeInit,
//	USBD_MSC_Setup,
//	NULL, /*EP0_TxSent*/
//	NULL, /*EP0_RxReady*/
//	USBD_MSC_DataIn,
//	USBD_MSC_DataOut,
//	NULL, /*SOF */
//	NULL, /*ISOIn*/
//	NULL, /*ISOOut*/
};

#endif /* WITHUSBHW && WITHUSBDMSC */
