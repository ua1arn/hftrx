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
	USBD_MTP_Init,	// Init
	USBD_MTP_DeInit,	// DeInit
	USBD_MTP_Setup,		// Setup
	NULL,	//USBD_XXX_EP0_TxSent,	// EP0_TxSent
	USBD_MTP_EP0_RxReady,	//USBD_XXX_EP0_RxReady,	// EP0_RxReady
	USBD_MTP_DataIn,	// DataIn
	USBD_MTP_DataOut,	// DataOut
	NULL,	//USBD_XXX_SOF,	// SOF
	NULL,	//USBD_XXX_IsoINIncomplete,	// IsoINIncomplete
	NULL,	//USBD_XXX_IsoOUTIncomplete,	// IsoOUTIncomplete
};

#endif /* WITHUSBHW && WITHUSBDMTP */
