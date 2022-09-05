/*
 * usbd_mtp.c
 *
 *  Created on: Sep 5, 2022
 *      Author: gena
 */
#include "hardware.h"

#if WITHUSBHW && WITHUSBDMTP


const USBD_ClassTypeDef USBD_CLASS_MTP =
{
	USBD_MTP_ColdInit,
	USBD_MTP_Init,
	USBD_MTP_DeInit,
	USBD_MTP_Setup,
	NULL, /*EP0_TxSent*/
	NULL, /*EP0_RxReady*/
	USBD_MTP_DataIn,
	USBD_MTP_DataOut,
	NULL, /*SOF */
	NULL, /*ISOIn*/
	NULL, /*ISOOut*/
};

#endif /* WITHUSBHW && WITHUSBDMTP */
